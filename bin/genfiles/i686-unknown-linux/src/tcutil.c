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
struct _RegionHandle*Cyc_Core_heap_region;typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{int __pos;
Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
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
q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};
struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
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
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Codegen_e_struct{int tag;struct
Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Fill_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct _tuple3{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;struct _tuple3 condition;
struct _tuple3 delta;struct Cyc_Absyn_Stmt*body;};struct Cyc_Absyn_Exp_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple3
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct
Cyc_Absyn_Goto_s_struct{int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple3 f2;
struct _tuple3 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_SwitchC_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Cut_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Splice_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Label_s_struct{int tag;struct _tagged_arr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct _tuple3 f2;};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*
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
struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*
cnst_exp;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
void*sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct
_tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*
exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple1*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _tagged_arr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{
void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
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
void*);int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);void*
Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void Cyc_Tc_tcAggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Enumdecl*);char Cyc_Tcutil_Unify[10]="\000\000\000\000Unify\000";void
Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)0;int Cyc_Tcutil_tq1_const=
0;void*Cyc_Tcutil_t2_failure=(void*)0;int Cyc_Tcutil_tq2_const=0;struct
_tagged_arr Cyc_Tcutil_failure_reason=(struct _tagged_arr){(void*)0,(void*)0,(void*)(
0 + 0)};void Cyc_Tcutil_explain_failure(){if(Cyc_Position_num_errors >= Cyc_Position_max_errors)
return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct _tagged_arr s1=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure));{
struct Cyc_String_pa_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(struct _tagged_arr)(Cyc_Tcutil_tq1_const?({
const char*_tmp23="const ";_tag_arr(_tmp23,sizeof(char),_get_zero_arr_size(_tmp23,
7));}):({const char*_tmp24="";_tag_arr(_tmp24,sizeof(char),_get_zero_arr_size(
_tmp24,1));}));{void*_tmp1F[2]={& _tmp21,& _tmp22};Cyc_aprintf(({const char*_tmp20="%s%s";
_tag_arr(_tmp20,sizeof(char),_get_zero_arr_size(_tmp20,5));}),_tag_arr(_tmp1F,
sizeof(void*),2));}}});struct _tagged_arr s2=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t2_failure));{struct Cyc_String_pa_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(
struct _tagged_arr)(Cyc_Tcutil_tq2_const?({const char*_tmp1D="const ";_tag_arr(
_tmp1D,sizeof(char),_get_zero_arr_size(_tmp1D,7));}):({const char*_tmp1E="";
_tag_arr(_tmp1E,sizeof(char),_get_zero_arr_size(_tmp1E,1));}));{void*_tmp19[2]={&
_tmp1B,& _tmp1C};Cyc_aprintf(({const char*_tmp1A="%s%s";_tag_arr(_tmp1A,sizeof(
char),_get_zero_arr_size(_tmp1A,5));}),_tag_arr(_tmp19,sizeof(void*),2));}}});
int pos=2;({struct Cyc_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(struct
_tagged_arr)((struct _tagged_arr)s1);{void*_tmp0[1]={& _tmp2};Cyc_fprintf(Cyc_stderr,({
const char*_tmp1="  %s";_tag_arr(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,5));}),
_tag_arr(_tmp0,sizeof(void*),1));}});pos +=_get_arr_size(s1,sizeof(char));if(pos + 
5 >= 80){({void*_tmp3[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp4="\n\t";
_tag_arr(_tmp4,sizeof(char),_get_zero_arr_size(_tmp4,3));}),_tag_arr(_tmp3,
sizeof(void*),0));});pos=8;}else{({void*_tmp5[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp6=" ";_tag_arr(_tmp6,sizeof(char),_get_zero_arr_size(_tmp6,2));}),
_tag_arr(_tmp5,sizeof(void*),0));});pos ++;}({void*_tmp7[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp8="and ";_tag_arr(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,5));}),
_tag_arr(_tmp7,sizeof(void*),0));});pos +=4;if(pos + _get_arr_size(s2,sizeof(char))
>= 80){({void*_tmp9[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpA="\n\t";
_tag_arr(_tmpA,sizeof(char),_get_zero_arr_size(_tmpA,3));}),_tag_arr(_tmp9,
sizeof(void*),0));});pos=8;}({struct Cyc_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(
struct _tagged_arr)((struct _tagged_arr)s2);{void*_tmpB[1]={& _tmpD};Cyc_fprintf(
Cyc_stderr,({const char*_tmpC="%s ";_tag_arr(_tmpC,sizeof(char),
_get_zero_arr_size(_tmpC,4));}),_tag_arr(_tmpB,sizeof(void*),1));}});pos +=
_get_arr_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){({void*_tmpE[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpF="\n\t";_tag_arr(_tmpF,sizeof(char),
_get_zero_arr_size(_tmpF,3));}),_tag_arr(_tmpE,sizeof(void*),0));});pos=8;}({
void*_tmp10[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp11="are not compatible. ";
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
_tmp42;_LL1: if(_tmp3C <= (void*)3?1:*((int*)_tmp3C)!= 0)goto _LL3;_tmp3D=((struct
Cyc_Absyn_Evar_struct*)_tmp3C)->f2;if(_tmp3D != 0)goto _LL3;_LL2: goto _LL4;_LL3: if(
_tmp3C <= (void*)3?1:*((int*)_tmp3C)!= 16)goto _LL5;_tmp3E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp3C)->f4;if(_tmp3E != 0)goto _LL5;_LL4: return t;_LL5: if(_tmp3C <= (void*)3?1:*((
int*)_tmp3C)!= 16)goto _LL7;_tmp3F=((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;
_tmp40=(void***)&((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;_LL6: {void*
t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp40)));if(t2 != *((void**)
_check_null(*_tmp40)))*_tmp40=({void**_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=
t2;_tmp43;});return t2;}_LL7: if(_tmp3C <= (void*)3?1:*((int*)_tmp3C)!= 0)goto _LL9;
_tmp41=((struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_tmp42=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_LL8: {void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(*_tmp42))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp42))->v)*_tmp42=({struct Cyc_Core_Opt*_tmp44=_cycalloc(sizeof(*
_tmp44));_tmp44->v=(void*)t2;_tmp44;});return t2;}_LL9:;_LLA: return t;_LL0:;}void*
Cyc_Tcutil_copy_type(void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(
struct Cyc_List_List*ts){return Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static
struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(struct Cyc_Absyn_Conref*c){void*
_tmp45=(void*)c->v;void*_tmp46;struct Cyc_Absyn_Conref*_tmp47;_LLC: if((int)_tmp45
!= 0)goto _LLE;_LLD: return Cyc_Absyn_empty_conref();_LLE: if(_tmp45 <= (void*)1?1:*((
int*)_tmp45)!= 0)goto _LL10;_tmp46=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp45)->f1;_LLF: return Cyc_Absyn_new_conref(_tmp46);_LL10: if(_tmp45 <= (void*)1?1:*((
int*)_tmp45)!= 1)goto _LLB;_tmp47=((struct Cyc_Absyn_Forward_constr_struct*)_tmp45)->f1;
_LL11: return Cyc_Tcutil_copy_conref(_tmp47);_LLB:;}static void*Cyc_Tcutil_copy_kindbound(
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
void*)3?1:*((int*)_tmp64)!= 0)goto _LL1E;_LL1D: return t;_LL1E: if(_tmp64 <= (void*)3?
1:*((int*)_tmp64)!= 1)goto _LL20;_tmp65=((struct Cyc_Absyn_VarType_struct*)_tmp64)->f1;
_LL1F: return(void*)({struct Cyc_Absyn_VarType_struct*_tmp9F=_cycalloc(sizeof(*
_tmp9F));_tmp9F[0]=({struct Cyc_Absyn_VarType_struct _tmpA0;_tmpA0.tag=1;_tmpA0.f1=
Cyc_Tcutil_copy_tvar(_tmp65);_tmpA0;});_tmp9F;});_LL20: if(_tmp64 <= (void*)3?1:*((
int*)_tmp64)!= 2)goto _LL22;_tmp66=((struct Cyc_Absyn_TunionType_struct*)_tmp64)->f1;
_tmp67=(void*)_tmp66.tunion_info;_tmp68=_tmp66.targs;_tmp69=(void*)_tmp66.rgn;
_LL21: return(void*)({struct Cyc_Absyn_TunionType_struct*_tmpA1=_cycalloc(sizeof(*
_tmpA1));_tmpA1[0]=({struct Cyc_Absyn_TunionType_struct _tmpA2;_tmpA2.tag=2;_tmpA2.f1=({
struct Cyc_Absyn_TunionInfo _tmpA3;_tmpA3.tunion_info=(void*)_tmp67;_tmpA3.targs=
Cyc_Tcutil_copy_types(_tmp68);_tmpA3.rgn=(void*)Cyc_Tcutil_copy_type(_tmp69);
_tmpA3;});_tmpA2;});_tmpA1;});_LL22: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 3)
goto _LL24;_tmp6A=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp64)->f1;_tmp6B=(
void*)_tmp6A.field_info;_tmp6C=_tmp6A.targs;_LL23: return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpA5;_tmpA5.tag=3;_tmpA5.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpA6;_tmpA6.field_info=(
void*)_tmp6B;_tmpA6.targs=Cyc_Tcutil_copy_types(_tmp6C);_tmpA6;});_tmpA5;});
_tmpA4;});_LL24: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 4)goto _LL26;_tmp6D=((
struct Cyc_Absyn_PointerType_struct*)_tmp64)->f1;_tmp6E=(void*)_tmp6D.elt_typ;
_tmp6F=_tmp6D.elt_tq;_tmp70=_tmp6D.ptr_atts;_tmp71=(void*)_tmp70.rgn;_tmp72=
_tmp70.nullable;_tmp73=_tmp70.bounds;_tmp74=_tmp70.zero_term;_LL25: {void*_tmpA7=
Cyc_Tcutil_copy_type(_tmp6E);void*_tmpA8=Cyc_Tcutil_copy_type(_tmp71);struct Cyc_Absyn_Conref*
_tmpA9=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(
_tmp72);struct Cyc_Absyn_Tqual _tmpAA=_tmp6F;struct Cyc_Absyn_Conref*_tmpAB=Cyc_Tcutil_copy_conref(
_tmp73);struct Cyc_Absyn_Conref*_tmpAC=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp74);return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_PointerType_struct
_tmpAE;_tmpAE.tag=4;_tmpAE.f1=({struct Cyc_Absyn_PtrInfo _tmpAF;_tmpAF.elt_typ=(
void*)_tmpA7;_tmpAF.elt_tq=_tmpAA;_tmpAF.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpB0;_tmpB0.rgn=(void*)_tmpA8;_tmpB0.nullable=_tmpA9;_tmpB0.bounds=_tmpAB;
_tmpB0.zero_term=_tmpAC;_tmpB0;});_tmpAF;});_tmpAE;});_tmpAD;});}_LL26: if(_tmp64
<= (void*)3?1:*((int*)_tmp64)!= 5)goto _LL28;_tmp75=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp64)->f1;_tmp76=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp64)->f2;_LL27:
return(void*)({struct Cyc_Absyn_IntType_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));
_tmpB1[0]=({struct Cyc_Absyn_IntType_struct _tmpB2;_tmpB2.tag=5;_tmpB2.f1=(void*)
_tmp75;_tmpB2.f2=(void*)_tmp76;_tmpB2;});_tmpB1;});_LL28: if((int)_tmp64 != 1)goto
_LL2A;_LL29: return t;_LL2A: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 6)goto _LL2C;
_tmp77=((struct Cyc_Absyn_DoubleType_struct*)_tmp64)->f1;_LL2B: return(void*)({
struct Cyc_Absyn_DoubleType_struct*_tmpB3=_cycalloc_atomic(sizeof(*_tmpB3));
_tmpB3[0]=({struct Cyc_Absyn_DoubleType_struct _tmpB4;_tmpB4.tag=6;_tmpB4.f1=
_tmp77;_tmpB4;});_tmpB3;});_LL2C: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 7)goto
_LL2E;_tmp78=((struct Cyc_Absyn_ArrayType_struct*)_tmp64)->f1;_tmp79=(void*)
_tmp78.elt_type;_tmp7A=_tmp78.tq;_tmp7B=_tmp78.num_elts;_tmp7C=_tmp78.zero_term;
_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmpB5=_cycalloc(sizeof(*
_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_ArrayType_struct _tmpB6;_tmpB6.tag=7;_tmpB6.f1=({
struct Cyc_Absyn_ArrayInfo _tmpB7;_tmpB7.elt_type=(void*)Cyc_Tcutil_copy_type(
_tmp79);_tmpB7.tq=_tmp7A;_tmpB7.num_elts=_tmp7B;_tmpB7.zero_term=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp7C);_tmpB7;});_tmpB6;});
_tmpB5;});_LL2E: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 8)goto _LL30;_tmp7D=((
struct Cyc_Absyn_FnType_struct*)_tmp64)->f1;_tmp7E=_tmp7D.tvars;_tmp7F=_tmp7D.effect;
_tmp80=(void*)_tmp7D.ret_typ;_tmp81=_tmp7D.args;_tmp82=_tmp7D.c_varargs;_tmp83=
_tmp7D.cyc_varargs;_tmp84=_tmp7D.rgn_po;_tmp85=_tmp7D.attributes;_LL2F: {struct
Cyc_List_List*_tmpB8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,
_tmp7E);struct Cyc_Core_Opt*_tmpB9=_tmp7F == 0?0:({struct Cyc_Core_Opt*_tmpC3=
_cycalloc(sizeof(*_tmpC3));_tmpC3->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp7F->v);
_tmpC3;});void*_tmpBA=Cyc_Tcutil_copy_type(_tmp80);struct Cyc_List_List*_tmpBB=((
struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp81);int _tmpBC=_tmp82;struct Cyc_Absyn_VarargInfo*
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
_tmpC0;});}}_LL30: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 9)goto _LL32;_tmp86=((
struct Cyc_Absyn_TupleType_struct*)_tmp64)->f1;_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_TupleType_struct
_tmpC5;_tmpC5.tag=9;_tmpC5.f1=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(
struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp86);
_tmpC5;});_tmpC4;});_LL32: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 10)goto _LL34;
_tmp87=((struct Cyc_Absyn_AggrType_struct*)_tmp64)->f1;_tmp88=(void*)_tmp87.aggr_info;
if(*((int*)_tmp88)!= 0)goto _LL34;_tmp89=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp88)->f1;_tmp8A=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp88)->f2;_tmp8B=
_tmp87.targs;_LL33: return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpC6=
_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_AggrType_struct _tmpC7;
_tmpC7.tag=10;_tmpC7.f1=({struct Cyc_Absyn_AggrInfo _tmpC8;_tmpC8.aggr_info=(void*)((
void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));
_tmpC9[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmpCA;_tmpCA.tag=0;_tmpCA.f1=(
void*)_tmp89;_tmpCA.f2=_tmp8A;_tmpCA;});_tmpC9;}));_tmpC8.targs=Cyc_Tcutil_copy_types(
_tmp8B);_tmpC8;});_tmpC7;});_tmpC6;});_LL34: if(_tmp64 <= (void*)3?1:*((int*)
_tmp64)!= 10)goto _LL36;_tmp8C=((struct Cyc_Absyn_AggrType_struct*)_tmp64)->f1;
_tmp8D=(void*)_tmp8C.aggr_info;if(*((int*)_tmp8D)!= 1)goto _LL36;_tmp8E=((struct
Cyc_Absyn_KnownAggr_struct*)_tmp8D)->f1;_tmp8F=_tmp8C.targs;_LL35: return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({
struct Cyc_Absyn_AggrType_struct _tmpCC;_tmpCC.tag=10;_tmpCC.f1=({struct Cyc_Absyn_AggrInfo
_tmpCD;_tmpCD.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpCE=
_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpCF;
_tmpCF.tag=1;_tmpCF.f1=_tmp8E;_tmpCF;});_tmpCE;}));_tmpCD.targs=Cyc_Tcutil_copy_types(
_tmp8F);_tmpCD;});_tmpCC;});_tmpCB;});_LL36: if(_tmp64 <= (void*)3?1:*((int*)
_tmp64)!= 11)goto _LL38;_tmp90=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp64)->f1;_tmp91=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp64)->f2;_LL37:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpD0=_cycalloc(sizeof(*
_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpD1;_tmpD1.tag=11;
_tmpD1.f1=(void*)_tmp90;_tmpD1.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmp91);_tmpD1;});_tmpD0;});_LL38: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 12)
goto _LL3A;_tmp92=((struct Cyc_Absyn_EnumType_struct*)_tmp64)->f1;_tmp93=((struct
Cyc_Absyn_EnumType_struct*)_tmp64)->f2;_LL39: return(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_EnumType_struct
_tmpD3;_tmpD3.tag=12;_tmpD3.f1=_tmp92;_tmpD3.f2=_tmp93;_tmpD3;});_tmpD2;});_LL3A:
if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 13)goto _LL3C;_tmp94=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp64)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmpD4=
_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_Absyn_AnonEnumType_struct _tmpD5;
_tmpD5.tag=13;_tmpD5.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp94);_tmpD5;});_tmpD4;});_LL3C: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 14)
goto _LL3E;_tmp95=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp64)->f1;_LL3D:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));
_tmpD6[0]=({struct Cyc_Absyn_SizeofType_struct _tmpD7;_tmpD7.tag=14;_tmpD7.f1=(
void*)Cyc_Tcutil_copy_type(_tmp95);_tmpD7;});_tmpD6;});_LL3E: if(_tmp64 <= (void*)
3?1:*((int*)_tmp64)!= 18)goto _LL40;_tmp96=((struct Cyc_Absyn_TypeInt_struct*)
_tmp64)->f1;_LL3F: return(void*)({struct Cyc_Absyn_TypeInt_struct*_tmpD8=
_cycalloc_atomic(sizeof(*_tmpD8));_tmpD8[0]=({struct Cyc_Absyn_TypeInt_struct
_tmpD9;_tmpD9.tag=18;_tmpD9.f1=_tmp96;_tmpD9;});_tmpD8;});_LL40: if(_tmp64 <= (
void*)3?1:*((int*)_tmp64)!= 17)goto _LL42;_tmp97=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp64)->f1;_LL41: return(void*)({struct Cyc_Absyn_TagType_struct*_tmpDA=_cycalloc(
sizeof(*_tmpDA));_tmpDA[0]=({struct Cyc_Absyn_TagType_struct _tmpDB;_tmpDB.tag=17;
_tmpDB.f1=(void*)Cyc_Tcutil_copy_type(_tmp97);_tmpDB;});_tmpDA;});_LL42: if(
_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 15)goto _LL44;_tmp98=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp64)->f1;_LL43: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpDC=
_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmpDD;_tmpDD.tag=15;_tmpDD.f1=(void*)Cyc_Tcutil_copy_type(_tmp98);_tmpDD;});
_tmpDC;});_LL44: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 16)goto _LL46;_tmp99=((
struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f1;_tmp9A=((struct Cyc_Absyn_TypedefType_struct*)
_tmp64)->f2;_tmp9B=((struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f3;_LL45:
return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpDE=_cycalloc(sizeof(*
_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_TypedefType_struct _tmpDF;_tmpDF.tag=16;
_tmpDF.f1=_tmp99;_tmpDF.f2=Cyc_Tcutil_copy_types(_tmp9A);_tmpDF.f3=_tmp9B;_tmpDF.f4=
0;_tmpDF;});_tmpDE;});_LL46: if((int)_tmp64 != 2)goto _LL48;_LL47: return t;_LL48: if(
_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 19)goto _LL4A;_tmp9C=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp64)->f1;_LL49: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmpE0=
_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_AccessEff_struct _tmpE1;
_tmpE1.tag=19;_tmpE1.f1=(void*)Cyc_Tcutil_copy_type(_tmp9C);_tmpE1;});_tmpE0;});
_LL4A: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 20)goto _LL4C;_tmp9D=((struct Cyc_Absyn_JoinEff_struct*)
_tmp64)->f1;_LL4B: return(void*)({struct Cyc_Absyn_JoinEff_struct*_tmpE2=_cycalloc(
sizeof(*_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_JoinEff_struct _tmpE3;_tmpE3.tag=20;
_tmpE3.f1=Cyc_Tcutil_copy_types(_tmp9D);_tmpE3;});_tmpE2;});_LL4C: if(_tmp64 <= (
void*)3?1:*((int*)_tmp64)!= 21)goto _LL19;_tmp9E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
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
_tmp108;_LL5F: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 0)goto _LL61;_tmpF4=((
struct Cyc_Absyn_Evar_struct*)_tmpF3)->f1;_tmpF5=((struct Cyc_Absyn_Evar_struct*)
_tmpF3)->f2;_LL60: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF4))->v;
_LL61: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 1)goto _LL63;_tmpF6=((struct Cyc_Absyn_VarType_struct*)
_tmpF3)->f1;_LL62: return Cyc_Tcutil_tvar_kind(_tmpF6);_LL63: if((int)_tmpF3 != 0)
goto _LL65;_LL64: return(void*)1;_LL65: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 5)
goto _LL67;_tmpF7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpF3)->f2;_LL66:
return _tmpF7 == (void*)((void*)2)?(void*)2:(void*)1;_LL67: if((int)_tmpF3 != 1)goto
_LL69;_LL68: goto _LL6A;_LL69: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 6)goto _LL6B;
_LL6A: goto _LL6C;_LL6B: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 8)goto _LL6D;_LL6C:
return(void*)1;_LL6D: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 15)goto _LL6F;_LL6E:
return(void*)2;_LL6F: if((int)_tmpF3 != 2)goto _LL71;_LL70: return(void*)3;_LL71: if(
_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 2)goto _LL73;_LL72: return(void*)2;_LL73: if(
_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 3)goto _LL75;_tmpF8=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF3)->f1;_tmpF9=(void*)_tmpF8.field_info;if(*((int*)_tmpF9)!= 1)goto _LL75;
_tmpFA=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpF9)->f2;_LL74: if(_tmpFA->typs
== 0)return(void*)2;else{return(void*)1;}_LL75: if(_tmpF3 <= (void*)3?1:*((int*)
_tmpF3)!= 3)goto _LL77;_tmpFB=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpF3)->f1;
_tmpFC=(void*)_tmpFB.field_info;if(*((int*)_tmpFC)!= 0)goto _LL77;_LL76:({void*
_tmp109[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp10A="typ_kind: Unresolved TunionFieldType";_tag_arr(_tmp10A,
sizeof(char),_get_zero_arr_size(_tmp10A,37));}),_tag_arr(_tmp109,sizeof(void*),0));});
_LL77: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 12)goto _LL79;_tmpFD=((struct Cyc_Absyn_EnumType_struct*)
_tmpF3)->f2;if(_tmpFD != 0)goto _LL79;_LL78: goto _LL7A;_LL79: if(_tmpF3 <= (void*)3?1:*((
int*)_tmpF3)!= 10)goto _LL7B;_tmpFE=((struct Cyc_Absyn_AggrType_struct*)_tmpF3)->f1;
_tmpFF=(void*)_tmpFE.aggr_info;if(*((int*)_tmpFF)!= 0)goto _LL7B;_LL7A: return(
void*)0;_LL7B: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 10)goto _LL7D;_tmp100=((
struct Cyc_Absyn_AggrType_struct*)_tmpF3)->f1;_tmp101=(void*)_tmp100.aggr_info;
if(*((int*)_tmp101)!= 1)goto _LL7D;_tmp102=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp101)->f1;_tmp103=*_tmp102;_tmp104=*_tmp103;_tmp105=_tmp104.impl;_LL7C: return
_tmp105 == 0?(void*)0:(void*)1;_LL7D: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 11)
goto _LL7F;_LL7E: goto _LL80;_LL7F: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 13)goto
_LL81;_LL80: return(void*)1;_LL81: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 12)
goto _LL83;_tmp106=((struct Cyc_Absyn_EnumType_struct*)_tmpF3)->f2;_LL82: if(
_tmp106->fields == 0)return(void*)0;else{return(void*)1;}_LL83: if(_tmpF3 <= (void*)
3?1:*((int*)_tmpF3)!= 4)goto _LL85;_tmp107=((struct Cyc_Absyn_PointerType_struct*)
_tmpF3)->f1;_LL84: {void*_tmp10B=(void*)(Cyc_Absyn_compress_conref((_tmp107.ptr_atts).bounds))->v;
void*_tmp10C;void*_tmp10D;void*_tmp10E;_LL98: if((int)_tmp10B != 0)goto _LL9A;_LL99:
goto _LL9B;_LL9A: if(_tmp10B <= (void*)1?1:*((int*)_tmp10B)!= 0)goto _LL9C;_tmp10C=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10B)->f1;if((int)_tmp10C != 0)goto
_LL9C;_LL9B: return(void*)1;_LL9C: if(_tmp10B <= (void*)1?1:*((int*)_tmp10B)!= 0)
goto _LL9E;_tmp10D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10B)->f1;if(
_tmp10D <= (void*)1?1:*((int*)_tmp10D)!= 0)goto _LL9E;_LL9D: goto _LL9F;_LL9E: if(
_tmp10B <= (void*)1?1:*((int*)_tmp10B)!= 0)goto _LLA0;_tmp10E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp10B)->f1;if(_tmp10E <= (void*)1?1:*((int*)_tmp10E)!= 1)goto _LLA0;_LL9F: return(
void*)2;_LLA0: if(_tmp10B <= (void*)1?1:*((int*)_tmp10B)!= 1)goto _LL97;_LLA1:({
void*_tmp10F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp110="typ_kind: forward constr in ptr bounds";_tag_arr(_tmp110,
sizeof(char),_get_zero_arr_size(_tmp110,39));}),_tag_arr(_tmp10F,sizeof(void*),0));});
_LL97:;}_LL85: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 14)goto _LL87;_LL86: return(
void*)2;_LL87: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 18)goto _LL89;_LL88: return(
void*)5;_LL89: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 17)goto _LL8B;_LL8A: return(
void*)2;_LL8B: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 7)goto _LL8D;_LL8C: goto
_LL8E;_LL8D: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 9)goto _LL8F;_LL8E: return(
void*)1;_LL8F: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 16)goto _LL91;_tmp108=((
struct Cyc_Absyn_TypedefType_struct*)_tmpF3)->f3;_LL90: if(_tmp108 == 0?1: _tmp108->kind
== 0)({struct Cyc_String_pa_struct _tmp113;_tmp113.tag=0;_tmp113.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp111[1]={&
_tmp113};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp112="typ_kind: typedef found: %s";_tag_arr(_tmp112,sizeof(char),
_get_zero_arr_size(_tmp112,28));}),_tag_arr(_tmp111,sizeof(void*),1));}});return(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp108->kind))->v;_LL91: if(_tmpF3 <= (
void*)3?1:*((int*)_tmpF3)!= 19)goto _LL93;_LL92: goto _LL94;_LL93: if(_tmpF3 <= (void*)
3?1:*((int*)_tmpF3)!= 20)goto _LL95;_LL94: goto _LL96;_LL95: if(_tmpF3 <= (void*)3?1:*((
int*)_tmpF3)!= 21)goto _LL5E;_LL96: return(void*)4;_LL5E:;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp114;_push_handler(& _tmp114);{int _tmp116=
0;if(setjmp(_tmp114.handler))_tmp116=1;if(!_tmp116){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp117=1;_npop_handler(0);return _tmp117;};_pop_handler();}else{void*_tmp115=(
void*)_exn_thrown;void*_tmp119=_tmp115;_LLA3: if(_tmp119 != Cyc_Tcutil_Unify)goto
_LLA5;_LLA4: return 0;_LLA5:;_LLA6:(void)_throw(_tmp119);_LLA2:;}}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t){t=Cyc_Tcutil_compress(t);{void*_tmp11A=t;struct Cyc_Absyn_Tvar*
_tmp11B;struct Cyc_Core_Opt*_tmp11C;struct Cyc_Core_Opt*_tmp11D;struct Cyc_Core_Opt**
_tmp11E;struct Cyc_Absyn_PtrInfo _tmp11F;struct Cyc_Absyn_ArrayInfo _tmp120;void*
_tmp121;struct Cyc_Absyn_FnInfo _tmp122;struct Cyc_List_List*_tmp123;struct Cyc_Core_Opt*
_tmp124;void*_tmp125;struct Cyc_List_List*_tmp126;int _tmp127;struct Cyc_Absyn_VarargInfo*
_tmp128;struct Cyc_List_List*_tmp129;struct Cyc_List_List*_tmp12A;struct Cyc_List_List*
_tmp12B;struct Cyc_Absyn_TunionInfo _tmp12C;struct Cyc_List_List*_tmp12D;void*
_tmp12E;struct Cyc_List_List*_tmp12F;struct Cyc_Absyn_TunionFieldInfo _tmp130;
struct Cyc_List_List*_tmp131;struct Cyc_Absyn_AggrInfo _tmp132;struct Cyc_List_List*
_tmp133;struct Cyc_List_List*_tmp134;void*_tmp135;void*_tmp136;void*_tmp137;void*
_tmp138;void*_tmp139;struct Cyc_List_List*_tmp13A;_LLA8: if(_tmp11A <= (void*)3?1:*((
int*)_tmp11A)!= 1)goto _LLAA;_tmp11B=((struct Cyc_Absyn_VarType_struct*)_tmp11A)->f1;
_LLA9: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp11B)){Cyc_Tcutil_failure_reason=({const char*_tmp13B="(type variable would escape scope)";
_tag_arr(_tmp13B,sizeof(char),_get_zero_arr_size(_tmp13B,35));});(int)_throw((
void*)Cyc_Tcutil_Unify);}goto _LLA7;_LLAA: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)
!= 0)goto _LLAC;_tmp11C=((struct Cyc_Absyn_Evar_struct*)_tmp11A)->f2;_tmp11D=((
struct Cyc_Absyn_Evar_struct*)_tmp11A)->f4;_tmp11E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp11A)->f4;_LLAB: if(t == evar){Cyc_Tcutil_failure_reason=({
const char*_tmp13C="(occurs check)";_tag_arr(_tmp13C,sizeof(char),
_get_zero_arr_size(_tmp13C,15));});(int)_throw((void*)Cyc_Tcutil_Unify);}else{
if(_tmp11C != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp11C->v);else{int problem=0;{
struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*
_tmp11E))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){problem=1;break;}}}
if(problem){struct Cyc_List_List*_tmp13D=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp11E))->v;for(0;s != 0;s=s->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd))_tmp13D=({struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->hd=(
struct Cyc_Absyn_Tvar*)s->hd;_tmp13E->tl=_tmp13D;_tmp13E;});}}*_tmp11E=({struct
Cyc_Core_Opt*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->v=_tmp13D;_tmp13F;});}}}
goto _LLA7;_LLAC: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 4)goto _LLAE;_tmp11F=((
struct Cyc_Absyn_PointerType_struct*)_tmp11A)->f1;_LLAD: Cyc_Tcutil_occurs(evar,r,
env,(void*)_tmp11F.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(void*)(_tmp11F.ptr_atts).rgn);{
void*_tmp140=(void*)(Cyc_Absyn_compress_conref((_tmp11F.ptr_atts).bounds))->v;
void*_tmp141;void*_tmp142;_LLCD: if(_tmp140 <= (void*)1?1:*((int*)_tmp140)!= 0)
goto _LLCF;_tmp141=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp140)->f1;if(
_tmp141 <= (void*)1?1:*((int*)_tmp141)!= 1)goto _LLCF;_tmp142=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp141)->f1;_LLCE: Cyc_Tcutil_occurs(evar,r,env,_tmp142);goto _LLCC;_LLCF:;_LLD0:
goto _LLCC;_LLCC:;}goto _LLA7;_LLAE: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 7)
goto _LLB0;_tmp120=((struct Cyc_Absyn_ArrayType_struct*)_tmp11A)->f1;_tmp121=(void*)
_tmp120.elt_type;_LLAF: Cyc_Tcutil_occurs(evar,r,env,_tmp121);goto _LLA7;_LLB0: if(
_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 8)goto _LLB2;_tmp122=((struct Cyc_Absyn_FnType_struct*)
_tmp11A)->f1;_tmp123=_tmp122.tvars;_tmp124=_tmp122.effect;_tmp125=(void*)_tmp122.ret_typ;
_tmp126=_tmp122.args;_tmp127=_tmp122.c_varargs;_tmp128=_tmp122.cyc_varargs;
_tmp129=_tmp122.rgn_po;_tmp12A=_tmp122.attributes;_LLB1: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp123,env);if(_tmp124 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp124->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp125);for(0;_tmp126 != 0;_tmp126=_tmp126->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple2*)_tmp126->hd)).f3);}if(_tmp128 != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp128->type);for(0;_tmp129 != 0;_tmp129=_tmp129->tl){struct
_tuple6 _tmp144;void*_tmp145;void*_tmp146;struct _tuple6*_tmp143=(struct _tuple6*)
_tmp129->hd;_tmp144=*_tmp143;_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp145);Cyc_Tcutil_occurs(evar,r,env,_tmp146);}goto _LLA7;_LLB2: if(
_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 9)goto _LLB4;_tmp12B=((struct Cyc_Absyn_TupleType_struct*)
_tmp11A)->f1;_LLB3: for(0;_tmp12B != 0;_tmp12B=_tmp12B->tl){Cyc_Tcutil_occurs(evar,
r,env,(*((struct _tuple4*)_tmp12B->hd)).f2);}goto _LLA7;_LLB4: if(_tmp11A <= (void*)
3?1:*((int*)_tmp11A)!= 2)goto _LLB6;_tmp12C=((struct Cyc_Absyn_TunionType_struct*)
_tmp11A)->f1;_tmp12D=_tmp12C.targs;_tmp12E=(void*)_tmp12C.rgn;_LLB5: Cyc_Tcutil_occurs(
evar,r,env,_tmp12E);Cyc_Tcutil_occurslist(evar,r,env,_tmp12D);goto _LLA7;_LLB6:
if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 16)goto _LLB8;_tmp12F=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11A)->f2;_LLB7: _tmp131=_tmp12F;goto _LLB9;_LLB8: if(_tmp11A <= (void*)3?1:*((
int*)_tmp11A)!= 3)goto _LLBA;_tmp130=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp11A)->f1;_tmp131=_tmp130.targs;_LLB9: _tmp133=_tmp131;goto _LLBB;_LLBA: if(
_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 10)goto _LLBC;_tmp132=((struct Cyc_Absyn_AggrType_struct*)
_tmp11A)->f1;_tmp133=_tmp132.targs;_LLBB: Cyc_Tcutil_occurslist(evar,r,env,
_tmp133);goto _LLA7;_LLBC: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 11)goto _LLBE;
_tmp134=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11A)->f2;_LLBD: for(0;_tmp134
!= 0;_tmp134=_tmp134->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmp134->hd)->type);}goto _LLA7;_LLBE: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 
17)goto _LLC0;_tmp135=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp11A)->f1;_LLBF:
_tmp136=_tmp135;goto _LLC1;_LLC0: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 14)
goto _LLC2;_tmp136=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp11A)->f1;_LLC1:
_tmp137=_tmp136;goto _LLC3;_LLC2: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 19)
goto _LLC4;_tmp137=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp11A)->f1;_LLC3:
_tmp138=_tmp137;goto _LLC5;_LLC4: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 15)
goto _LLC6;_tmp138=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp11A)->f1;
_LLC5: _tmp139=_tmp138;goto _LLC7;_LLC6: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 
21)goto _LLC8;_tmp139=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp11A)->f1;_LLC7:
Cyc_Tcutil_occurs(evar,r,env,_tmp139);goto _LLA7;_LLC8: if(_tmp11A <= (void*)3?1:*((
int*)_tmp11A)!= 20)goto _LLCA;_tmp13A=((struct Cyc_Absyn_JoinEff_struct*)_tmp11A)->f1;
_LLC9: Cyc_Tcutil_occurslist(evar,r,env,_tmp13A);goto _LLA7;_LLCA:;_LLCB: goto _LLA7;
_LLA7:;}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct
Cyc_List_List*env,struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(
evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*
t1,struct Cyc_List_List*t2){for(0;t1 != 0?t2 != 0: 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((
void*)t1->hd,(void*)t2->hd);}if(t1 != 0?1: t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual
tq2,void*t2){if((tq1.q_const != tq2.q_const?1: tq1.q_volatile != tq2.q_volatile)?1:
tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.q_const;Cyc_Tcutil_tq2_const=tq2.q_const;Cyc_Tcutil_failure_reason=({
const char*_tmp147="(qualifiers don't match)";_tag_arr(_tmp147,sizeof(char),
_get_zero_arr_size(_tmp147,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_tq1_const=
0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){return(tq1.q_const == tq2.q_const?tq1.q_volatile == tq2.q_volatile:
0)?tq1.q_restrict == tq2.q_restrict: 0;}static void Cyc_Tcutil_unify_it_conrefs(int(*
cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{void*_tmp148=(void*)x->v;void*_tmp149;_LLD2: if((int)_tmp148 != 0)
goto _LLD4;_LLD3:(void*)(x->v=(void*)((void*)({struct Cyc_Absyn_Forward_constr_struct*
_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A[0]=({struct Cyc_Absyn_Forward_constr_struct
_tmp14B;_tmp14B.tag=1;_tmp14B.f1=y;_tmp14B;});_tmp14A;})));return;_LLD4: if(
_tmp148 <= (void*)1?1:*((int*)_tmp148)!= 0)goto _LLD6;_tmp149=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp148)->f1;_LLD5: {void*_tmp14C=(void*)y->v;void*_tmp14D;_LLD9: if((int)_tmp14C
!= 0)goto _LLDB;_LLDA:(void*)(y->v=(void*)((void*)x->v));return;_LLDB: if(_tmp14C
<= (void*)1?1:*((int*)_tmp14C)!= 0)goto _LLDD;_tmp14D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp14C)->f1;_LLDC: if(cmp(_tmp149,_tmp14D)!= 0){Cyc_Tcutil_failure_reason=reason;(
int)_throw((void*)Cyc_Tcutil_Unify);}return;_LLDD: if(_tmp14C <= (void*)1?1:*((int*)
_tmp14C)!= 1)goto _LLD8;_LLDE:({void*_tmp14E[0]={};Cyc_Tcutil_impos(({const char*
_tmp14F="unify_conref: forward after compress(2)";_tag_arr(_tmp14F,sizeof(char),
_get_zero_arr_size(_tmp14F,40));}),_tag_arr(_tmp14E,sizeof(void*),0));});_LLD8:;}
_LLD6: if(_tmp148 <= (void*)1?1:*((int*)_tmp148)!= 1)goto _LLD1;_LLD7:({void*
_tmp150[0]={};Cyc_Tcutil_impos(({const char*_tmp151="unify_conref: forward after compress";
_tag_arr(_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,37));}),_tag_arr(
_tmp150,sizeof(void*),0));});_LLD1:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(
void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp152;_push_handler(& _tmp152);{int _tmp154=0;if(setjmp(_tmp152.handler))
_tmp154=1;if(!_tmp154){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _tagged_arr)
_tag_arr(0,0,0));{int _tmp155=1;_npop_handler(0);return _tmp155;};_pop_handler();}
else{void*_tmp153=(void*)_exn_thrown;void*_tmp157=_tmp153;_LLE0: if(_tmp157 != Cyc_Tcutil_Unify)
goto _LLE2;_LLE1: return 0;_LLE2:;_LLE3:(void)_throw(_tmp157);_LLDF:;}}}static int
Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple6 _tmp159=({struct _tuple6
_tmp158;_tmp158.f1=b1;_tmp158.f2=b2;_tmp158;});void*_tmp15A;void*_tmp15B;void*
_tmp15C;void*_tmp15D;void*_tmp15E;struct Cyc_Absyn_Exp*_tmp15F;void*_tmp160;
struct Cyc_Absyn_Exp*_tmp161;void*_tmp162;void*_tmp163;void*_tmp164;void*_tmp165;
void*_tmp166;void*_tmp167;void*_tmp168;void*_tmp169;_LLE5: _tmp15A=_tmp159.f1;if((
int)_tmp15A != 0)goto _LLE7;_tmp15B=_tmp159.f2;if((int)_tmp15B != 0)goto _LLE7;_LLE6:
return 0;_LLE7: _tmp15C=_tmp159.f1;if((int)_tmp15C != 0)goto _LLE9;_LLE8: return - 1;
_LLE9: _tmp15D=_tmp159.f2;if((int)_tmp15D != 0)goto _LLEB;_LLEA: return 1;_LLEB:
_tmp15E=_tmp159.f1;if(_tmp15E <= (void*)1?1:*((int*)_tmp15E)!= 0)goto _LLED;
_tmp15F=((struct Cyc_Absyn_Upper_b_struct*)_tmp15E)->f1;_tmp160=_tmp159.f2;if(
_tmp160 <= (void*)1?1:*((int*)_tmp160)!= 0)goto _LLED;_tmp161=((struct Cyc_Absyn_Upper_b_struct*)
_tmp160)->f1;_LLEC: return Cyc_Evexp_const_exp_cmp(_tmp15F,_tmp161);_LLED: _tmp162=
_tmp159.f1;if(_tmp162 <= (void*)1?1:*((int*)_tmp162)!= 0)goto _LLEF;_tmp163=
_tmp159.f2;if(_tmp163 <= (void*)1?1:*((int*)_tmp163)!= 1)goto _LLEF;_LLEE: return - 1;
_LLEF: _tmp164=_tmp159.f1;if(_tmp164 <= (void*)1?1:*((int*)_tmp164)!= 1)goto _LLF1;
_tmp165=_tmp159.f2;if(_tmp165 <= (void*)1?1:*((int*)_tmp165)!= 0)goto _LLF1;_LLF0:
return 1;_LLF1: _tmp166=_tmp159.f1;if(_tmp166 <= (void*)1?1:*((int*)_tmp166)!= 1)
goto _LLE4;_tmp167=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp166)->f1;
_tmp168=_tmp159.f2;if(_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 1)goto _LLE4;
_tmp169=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp168)->f1;_LLF2: return Cyc_Tcutil_typecmp(
_tmp167,_tmp169);_LLE4:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple6 _tmp16B=({struct _tuple6 _tmp16A;_tmp16A.f1=b1;_tmp16A.f2=b2;_tmp16A;});
void*_tmp16C;void*_tmp16D;void*_tmp16E;void*_tmp16F;void*_tmp170;struct Cyc_Absyn_Exp*
_tmp171;void*_tmp172;struct Cyc_Absyn_Exp*_tmp173;void*_tmp174;struct Cyc_Absyn_Exp*
_tmp175;void*_tmp176;void*_tmp177;void*_tmp178;void*_tmp179;void*_tmp17A;struct
Cyc_Absyn_Exp*_tmp17B;void*_tmp17C;void*_tmp17D;void*_tmp17E;void*_tmp17F;_LLF4:
_tmp16C=_tmp16B.f1;if((int)_tmp16C != 0)goto _LLF6;_tmp16D=_tmp16B.f2;if((int)
_tmp16D != 0)goto _LLF6;_LLF5: return 0;_LLF6: _tmp16E=_tmp16B.f1;if((int)_tmp16E != 0)
goto _LLF8;_LLF7: return - 1;_LLF8: _tmp16F=_tmp16B.f2;if((int)_tmp16F != 0)goto _LLFA;
_LLF9: return 1;_LLFA: _tmp170=_tmp16B.f1;if(_tmp170 <= (void*)1?1:*((int*)_tmp170)
!= 0)goto _LLFC;_tmp171=((struct Cyc_Absyn_Upper_b_struct*)_tmp170)->f1;_tmp172=
_tmp16B.f2;if(_tmp172 <= (void*)1?1:*((int*)_tmp172)!= 0)goto _LLFC;_tmp173=((
struct Cyc_Absyn_Upper_b_struct*)_tmp172)->f1;_LLFB: return Cyc_Evexp_const_exp_cmp(
_tmp171,_tmp173);_LLFC: _tmp174=_tmp16B.f1;if(_tmp174 <= (void*)1?1:*((int*)
_tmp174)!= 0)goto _LLFE;_tmp175=((struct Cyc_Absyn_Upper_b_struct*)_tmp174)->f1;
_tmp176=_tmp16B.f2;if(_tmp176 <= (void*)1?1:*((int*)_tmp176)!= 1)goto _LLFE;
_tmp177=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp176)->f1;_LLFD: _tmp179=
_tmp177;_tmp17B=_tmp175;goto _LLFF;_LLFE: _tmp178=_tmp16B.f1;if(_tmp178 <= (void*)1?
1:*((int*)_tmp178)!= 1)goto _LL100;_tmp179=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp178)->f1;_tmp17A=_tmp16B.f2;if(_tmp17A <= (void*)1?1:*((int*)_tmp17A)!= 0)
goto _LL100;_tmp17B=((struct Cyc_Absyn_Upper_b_struct*)_tmp17A)->f1;_LLFF: {
unsigned int _tmp181;int _tmp182;struct _tuple7 _tmp180=Cyc_Evexp_eval_const_uint_exp(
_tmp17B);_tmp181=_tmp180.f1;_tmp182=_tmp180.f2;if(!_tmp182)return 1;_tmp17D=
_tmp179;_tmp17F=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp183=_cycalloc_atomic(
sizeof(*_tmp183));_tmp183[0]=({struct Cyc_Absyn_TypeInt_struct _tmp184;_tmp184.tag=
18;_tmp184.f1=(int)_tmp181;_tmp184;});_tmp183;});goto _LL101;}_LL100: _tmp17C=
_tmp16B.f1;if(_tmp17C <= (void*)1?1:*((int*)_tmp17C)!= 1)goto _LLF3;_tmp17D=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmp17C)->f1;_tmp17E=_tmp16B.f2;if(_tmp17E <= (
void*)1?1:*((int*)_tmp17E)!= 1)goto _LLF3;_tmp17F=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp17E)->f1;_LL101: Cyc_Tcutil_unify_it(_tmp17D,_tmp17F);return 0;_LLF3:;}static
int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp185=att;_LL103: if(_tmp185
<= (void*)17?1:*((int*)_tmp185)!= 0)goto _LL105;_LL104: return 0;_LL105: if((int)
_tmp185 != 0)goto _LL107;_LL106: return 1;_LL107: if((int)_tmp185 != 1)goto _LL109;
_LL108: return 2;_LL109: if((int)_tmp185 != 2)goto _LL10B;_LL10A: return 3;_LL10B: if((
int)_tmp185 != 3)goto _LL10D;_LL10C: return 4;_LL10D: if((int)_tmp185 != 4)goto _LL10F;
_LL10E: return 5;_LL10F: if(_tmp185 <= (void*)17?1:*((int*)_tmp185)!= 1)goto _LL111;
_LL110: return 6;_LL111: if((int)_tmp185 != 5)goto _LL113;_LL112: return 7;_LL113: if(
_tmp185 <= (void*)17?1:*((int*)_tmp185)!= 2)goto _LL115;_LL114: return 8;_LL115: if((
int)_tmp185 != 6)goto _LL117;_LL116: return 9;_LL117: if((int)_tmp185 != 7)goto _LL119;
_LL118: return 10;_LL119: if((int)_tmp185 != 8)goto _LL11B;_LL11A: return 11;_LL11B: if((
int)_tmp185 != 9)goto _LL11D;_LL11C: return 12;_LL11D: if((int)_tmp185 != 10)goto
_LL11F;_LL11E: return 13;_LL11F: if((int)_tmp185 != 11)goto _LL121;_LL120: return 14;
_LL121: if((int)_tmp185 != 12)goto _LL123;_LL122: return 15;_LL123: if((int)_tmp185 != 
13)goto _LL125;_LL124: return 16;_LL125: if((int)_tmp185 != 14)goto _LL127;_LL126:
return 17;_LL127: if((int)_tmp185 != 15)goto _LL129;_LL128: return 18;_LL129: if(
_tmp185 <= (void*)17?1:*((int*)_tmp185)!= 3)goto _LL12B;_LL12A: return 19;_LL12B: if(
_tmp185 <= (void*)17?1:*((int*)_tmp185)!= 4)goto _LL12D;_LL12C: return 20;_LL12D:;
_LL12E: return 21;_LL102:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple6 _tmp187=({struct _tuple6 _tmp186;_tmp186.f1=att1;_tmp186.f2=att2;
_tmp186;});void*_tmp188;int _tmp189;void*_tmp18A;int _tmp18B;void*_tmp18C;int
_tmp18D;void*_tmp18E;int _tmp18F;void*_tmp190;int _tmp191;void*_tmp192;int _tmp193;
void*_tmp194;struct _tagged_arr _tmp195;void*_tmp196;struct _tagged_arr _tmp197;void*
_tmp198;void*_tmp199;int _tmp19A;int _tmp19B;void*_tmp19C;void*_tmp19D;int _tmp19E;
int _tmp19F;_LL130: _tmp188=_tmp187.f1;if(_tmp188 <= (void*)17?1:*((int*)_tmp188)!= 
0)goto _LL132;_tmp189=((struct Cyc_Absyn_Regparm_att_struct*)_tmp188)->f1;_tmp18A=
_tmp187.f2;if(_tmp18A <= (void*)17?1:*((int*)_tmp18A)!= 0)goto _LL132;_tmp18B=((
struct Cyc_Absyn_Regparm_att_struct*)_tmp18A)->f1;_LL131: _tmp18D=_tmp189;_tmp18F=
_tmp18B;goto _LL133;_LL132: _tmp18C=_tmp187.f1;if(_tmp18C <= (void*)17?1:*((int*)
_tmp18C)!= 4)goto _LL134;_tmp18D=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp18C)->f1;_tmp18E=_tmp187.f2;if(_tmp18E <= (void*)17?1:*((int*)_tmp18E)!= 4)
goto _LL134;_tmp18F=((struct Cyc_Absyn_Initializes_att_struct*)_tmp18E)->f1;_LL133:
_tmp191=_tmp18D;_tmp193=_tmp18F;goto _LL135;_LL134: _tmp190=_tmp187.f1;if(_tmp190
<= (void*)17?1:*((int*)_tmp190)!= 1)goto _LL136;_tmp191=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp190)->f1;_tmp192=_tmp187.f2;if(_tmp192 <= (void*)17?1:*((int*)_tmp192)!= 1)
goto _LL136;_tmp193=((struct Cyc_Absyn_Aligned_att_struct*)_tmp192)->f1;_LL135:
return Cyc_Core_intcmp(_tmp191,_tmp193);_LL136: _tmp194=_tmp187.f1;if(_tmp194 <= (
void*)17?1:*((int*)_tmp194)!= 2)goto _LL138;_tmp195=((struct Cyc_Absyn_Section_att_struct*)
_tmp194)->f1;_tmp196=_tmp187.f2;if(_tmp196 <= (void*)17?1:*((int*)_tmp196)!= 2)
goto _LL138;_tmp197=((struct Cyc_Absyn_Section_att_struct*)_tmp196)->f1;_LL137:
return Cyc_strcmp((struct _tagged_arr)_tmp195,(struct _tagged_arr)_tmp197);_LL138:
_tmp198=_tmp187.f1;if(_tmp198 <= (void*)17?1:*((int*)_tmp198)!= 3)goto _LL13A;
_tmp199=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp198)->f1;_tmp19A=((
struct Cyc_Absyn_Format_att_struct*)_tmp198)->f2;_tmp19B=((struct Cyc_Absyn_Format_att_struct*)
_tmp198)->f3;_tmp19C=_tmp187.f2;if(_tmp19C <= (void*)17?1:*((int*)_tmp19C)!= 3)
goto _LL13A;_tmp19D=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp19C)->f1;
_tmp19E=((struct Cyc_Absyn_Format_att_struct*)_tmp19C)->f2;_tmp19F=((struct Cyc_Absyn_Format_att_struct*)
_tmp19C)->f3;_LL139: {int _tmp1A0=Cyc_Core_intcmp((int)((unsigned int)_tmp199),(
int)((unsigned int)_tmp19D));if(_tmp1A0 != 0)return _tmp1A0;{int _tmp1A1=Cyc_Core_intcmp(
_tmp19A,_tmp19E);if(_tmp1A1 != 0)return _tmp1A1;return Cyc_Core_intcmp(_tmp19B,
_tmp19F);}}_LL13A:;_LL13B: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL12F:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return
0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static
void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of((
void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*
tv){void*t;{void*_tmp1A2=Cyc_Tcutil_tvar_kind(tv);_LL13D: if((int)_tmp1A2 != 3)
goto _LL13F;_LL13E: t=(void*)2;goto _LL13C;_LL13F: if((int)_tmp1A2 != 4)goto _LL141;
_LL140: t=Cyc_Absyn_empty_effect;goto _LL13C;_LL141: if((int)_tmp1A2 != 5)goto _LL143;
_LL142: t=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp1A3=_cycalloc_atomic(
sizeof(*_tmp1A3));_tmp1A3[0]=({struct Cyc_Absyn_TypeInt_struct _tmp1A4;_tmp1A4.tag=
18;_tmp1A4.f1=0;_tmp1A4;});_tmp1A3;});goto _LL13C;_LL143:;_LL144: t=Cyc_Absyn_sint_typ;
goto _LL13C;_LL13C:;}return({struct _tuple8*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));
_tmp1A5->f1=tv;_tmp1A5->f2=t;_tmp1A5;});}static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp1A6=Cyc_Tcutil_compress(t);void*_tmp1A7;struct Cyc_Absyn_TunionInfo
_tmp1A8;struct Cyc_List_List*_tmp1A9;void*_tmp1AA;struct Cyc_Absyn_PtrInfo _tmp1AB;
void*_tmp1AC;struct Cyc_Absyn_PtrAtts _tmp1AD;void*_tmp1AE;struct Cyc_Absyn_ArrayInfo
_tmp1AF;void*_tmp1B0;struct Cyc_List_List*_tmp1B1;struct Cyc_Absyn_TunionFieldInfo
_tmp1B2;struct Cyc_List_List*_tmp1B3;struct Cyc_Absyn_AggrInfo _tmp1B4;struct Cyc_List_List*
_tmp1B5;struct Cyc_List_List*_tmp1B6;void*_tmp1B7;struct Cyc_Absyn_FnInfo _tmp1B8;
struct Cyc_List_List*_tmp1B9;struct Cyc_Core_Opt*_tmp1BA;void*_tmp1BB;struct Cyc_List_List*
_tmp1BC;struct Cyc_Absyn_VarargInfo*_tmp1BD;struct Cyc_List_List*_tmp1BE;void*
_tmp1BF;struct Cyc_List_List*_tmp1C0;_LL146: if((int)_tmp1A6 != 0)goto _LL148;_LL147:
goto _LL149;_LL148: if((int)_tmp1A6 != 1)goto _LL14A;_LL149: goto _LL14B;_LL14A: if(
_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 6)goto _LL14C;_LL14B: goto _LL14D;_LL14C:
if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 12)goto _LL14E;_LL14D: goto _LL14F;
_LL14E: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 13)goto _LL150;_LL14F: goto
_LL151;_LL150: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 18)goto _LL152;_LL151:
goto _LL153;_LL152: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 5)goto _LL154;_LL153:
return Cyc_Absyn_empty_effect;_LL154: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 0)
goto _LL156;_LL155: goto _LL157;_LL156: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 1)
goto _LL158;_LL157: {void*_tmp1C1=Cyc_Tcutil_typ_kind(t);_LL179: if((int)_tmp1C1 != 
3)goto _LL17B;_LL17A: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1C2=
_cycalloc(sizeof(*_tmp1C2));_tmp1C2[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1C3;
_tmp1C3.tag=19;_tmp1C3.f1=(void*)t;_tmp1C3;});_tmp1C2;});_LL17B: if((int)_tmp1C1
!= 4)goto _LL17D;_LL17C: return t;_LL17D: if((int)_tmp1C1 != 5)goto _LL17F;_LL17E:
return Cyc_Absyn_empty_effect;_LL17F:;_LL180: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1C5;_tmp1C5.tag=21;_tmp1C5.f1=(void*)t;_tmp1C5;});_tmp1C4;});_LL178:;}_LL158:
if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 15)goto _LL15A;_tmp1A7=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp1A6)->f1;_LL159: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1C7;_tmp1C7.tag=19;_tmp1C7.f1=(void*)_tmp1A7;_tmp1C7;});_tmp1C6;});_LL15A:
if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 2)goto _LL15C;_tmp1A8=((struct Cyc_Absyn_TunionType_struct*)
_tmp1A6)->f1;_tmp1A9=_tmp1A8.targs;_tmp1AA=(void*)_tmp1A8.rgn;_LL15B: {struct Cyc_List_List*
ts=({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));
_tmp1CB[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1CC;_tmp1CC.tag=19;_tmp1CC.f1=(
void*)_tmp1AA;_tmp1CC;});_tmp1CB;}));_tmp1CA->tl=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1A9);_tmp1CA;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1C8=_cycalloc(sizeof(*_tmp1C8));_tmp1C8[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1C9;_tmp1C9.tag=20;_tmp1C9.f1=ts;_tmp1C9;});_tmp1C8;}));}_LL15C: if(_tmp1A6 <= (
void*)3?1:*((int*)_tmp1A6)!= 4)goto _LL15E;_tmp1AB=((struct Cyc_Absyn_PointerType_struct*)
_tmp1A6)->f1;_tmp1AC=(void*)_tmp1AB.elt_typ;_tmp1AD=_tmp1AB.ptr_atts;_tmp1AE=(
void*)_tmp1AD.rgn;_LL15D: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1CE;_tmp1CE.tag=20;_tmp1CE.f1=({void*_tmp1CF[2];_tmp1CF[1]=Cyc_Tcutil_rgns_of(
_tmp1AC);_tmp1CF[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1D0=_cycalloc(
sizeof(*_tmp1D0));_tmp1D0[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1D1;_tmp1D1.tag=
19;_tmp1D1.f1=(void*)_tmp1AE;_tmp1D1;});_tmp1D0;});Cyc_List_list(_tag_arr(
_tmp1CF,sizeof(void*),2));});_tmp1CE;});_tmp1CD;}));_LL15E: if(_tmp1A6 <= (void*)3?
1:*((int*)_tmp1A6)!= 7)goto _LL160;_tmp1AF=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1A6)->f1;_tmp1B0=(void*)_tmp1AF.elt_type;_LL15F: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp1B0));_LL160: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 9)
goto _LL162;_tmp1B1=((struct Cyc_Absyn_TupleType_struct*)_tmp1A6)->f1;_LL161: {
struct Cyc_List_List*_tmp1D2=0;for(0;_tmp1B1 != 0;_tmp1B1=_tmp1B1->tl){_tmp1D2=({
struct Cyc_List_List*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->hd=(void*)(*((
struct _tuple4*)_tmp1B1->hd)).f2;_tmp1D3->tl=_tmp1D2;_tmp1D3;});}_tmp1B3=_tmp1D2;
goto _LL163;}_LL162: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 3)goto _LL164;
_tmp1B2=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1A6)->f1;_tmp1B3=_tmp1B2.targs;
_LL163: _tmp1B5=_tmp1B3;goto _LL165;_LL164: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)
!= 10)goto _LL166;_tmp1B4=((struct Cyc_Absyn_AggrType_struct*)_tmp1A6)->f1;_tmp1B5=
_tmp1B4.targs;_LL165: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1D5;_tmp1D5.tag=20;_tmp1D5.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1B5);
_tmp1D5;});_tmp1D4;}));_LL166: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 11)goto
_LL168;_tmp1B6=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1A6)->f2;_LL167: return
Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1D6=
_cycalloc(sizeof(*_tmp1D6));_tmp1D6[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1D7;
_tmp1D7.tag=20;_tmp1D7.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1B6);_tmp1D7;});
_tmp1D6;}));_LL168: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 14)goto _LL16A;
_tmp1B7=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp1A6)->f1;_LL169: return
Cyc_Tcutil_rgns_of(_tmp1B7);_LL16A: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 17)
goto _LL16C;_LL16B: return Cyc_Tcutil_rgns_of(t);_LL16C: if(_tmp1A6 <= (void*)3?1:*((
int*)_tmp1A6)!= 8)goto _LL16E;_tmp1B8=((struct Cyc_Absyn_FnType_struct*)_tmp1A6)->f1;
_tmp1B9=_tmp1B8.tvars;_tmp1BA=_tmp1B8.effect;_tmp1BB=(void*)_tmp1B8.ret_typ;
_tmp1BC=_tmp1B8.args;_tmp1BD=_tmp1B8.cyc_varargs;_tmp1BE=_tmp1B8.rgn_po;_LL16D: {
void*_tmp1D8=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1B9),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1BA))->v);return Cyc_Tcutil_normalize_effect(
_tmp1D8);}_LL16E: if((int)_tmp1A6 != 2)goto _LL170;_LL16F: return Cyc_Absyn_empty_effect;
_LL170: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 19)goto _LL172;_LL171: goto
_LL173;_LL172: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 20)goto _LL174;_LL173:
return t;_LL174: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 21)goto _LL176;_tmp1BF=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1A6)->f1;_LL175: return Cyc_Tcutil_rgns_of(
_tmp1BF);_LL176: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 16)goto _LL145;_tmp1C0=((
struct Cyc_Absyn_TypedefType_struct*)_tmp1A6)->f2;_LL177: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));
_tmp1D9[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1DA;_tmp1DA.tag=20;_tmp1DA.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp1C0);_tmp1DA;});_tmp1D9;}));_LL145:;}void*Cyc_Tcutil_normalize_effect(
void*e){e=Cyc_Tcutil_compress(e);{void*_tmp1DB=e;struct Cyc_List_List*_tmp1DC;
struct Cyc_List_List**_tmp1DD;void*_tmp1DE;_LL182: if(_tmp1DB <= (void*)3?1:*((int*)
_tmp1DB)!= 20)goto _LL184;_tmp1DC=((struct Cyc_Absyn_JoinEff_struct*)_tmp1DB)->f1;
_tmp1DD=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_struct*)_tmp1DB)->f1;
_LL183: {int redo_join=0;{struct Cyc_List_List*effs=*_tmp1DD;for(0;effs != 0;effs=
effs->tl){void*_tmp1DF=(void*)effs->hd;(void*)(effs->hd=(void*)Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect(_tmp1DF)));{void*_tmp1E0=(void*)effs->hd;void*
_tmp1E1;_LL189: if(_tmp1E0 <= (void*)3?1:*((int*)_tmp1E0)!= 20)goto _LL18B;_LL18A:
goto _LL18C;_LL18B: if(_tmp1E0 <= (void*)3?1:*((int*)_tmp1E0)!= 19)goto _LL18D;
_tmp1E1=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1E0)->f1;if((int)_tmp1E1
!= 2)goto _LL18D;_LL18C: redo_join=1;goto _LL188;_LL18D:;_LL18E: goto _LL188;_LL188:;}}}
if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct Cyc_List_List*effs=*
_tmp1DD;for(0;effs != 0;effs=effs->tl){void*_tmp1E2=Cyc_Tcutil_compress((void*)
effs->hd);struct Cyc_List_List*_tmp1E3;void*_tmp1E4;_LL190: if(_tmp1E2 <= (void*)3?
1:*((int*)_tmp1E2)!= 20)goto _LL192;_tmp1E3=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1E2)->f1;_LL191: effects=Cyc_List_revappend(_tmp1E3,effects);goto _LL18F;
_LL192: if(_tmp1E2 <= (void*)3?1:*((int*)_tmp1E2)!= 19)goto _LL194;_tmp1E4=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp1E2)->f1;if((int)_tmp1E4 != 2)goto _LL194;
_LL193: goto _LL18F;_LL194:;_LL195: effects=({struct Cyc_List_List*_tmp1E5=_cycalloc(
sizeof(*_tmp1E5));_tmp1E5->hd=(void*)_tmp1E2;_tmp1E5->tl=effects;_tmp1E5;});goto
_LL18F;_LL18F:;}}*_tmp1DD=Cyc_List_imp_rev(effects);return e;}}_LL184: if(_tmp1DB
<= (void*)3?1:*((int*)_tmp1DB)!= 21)goto _LL186;_tmp1DE=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1DB)->f1;_LL185: {void*_tmp1E6=Cyc_Tcutil_compress(_tmp1DE);_LL197: if(
_tmp1E6 <= (void*)3?1:*((int*)_tmp1E6)!= 0)goto _LL199;_LL198: goto _LL19A;_LL199:
if(_tmp1E6 <= (void*)3?1:*((int*)_tmp1E6)!= 1)goto _LL19B;_LL19A: return e;_LL19B:;
_LL19C: return Cyc_Tcutil_rgns_of(_tmp1DE);_LL196:;}_LL186:;_LL187: return e;_LL181:;}}
static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};static void*Cyc_Tcutil_dummy_fntype(
void*eff){struct Cyc_Absyn_FnType_struct*_tmp1E7=({struct Cyc_Absyn_FnType_struct*
_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8[0]=({struct Cyc_Absyn_FnType_struct
_tmp1E9;_tmp1E9.tag=8;_tmp1E9.f1=({struct Cyc_Absyn_FnInfo _tmp1EA;_tmp1EA.tvars=0;
_tmp1EA.effect=({struct Cyc_Core_Opt*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB->v=(
void*)eff;_tmp1EB;});_tmp1EA.ret_typ=(void*)((void*)0);_tmp1EA.args=0;_tmp1EA.c_varargs=
0;_tmp1EA.cyc_varargs=0;_tmp1EA.rgn_po=0;_tmp1EA.attributes=0;_tmp1EA;});_tmp1E9;});
_tmp1E8;});return Cyc_Absyn_atb_typ((void*)_tmp1E7,(void*)2,Cyc_Absyn_empty_tqual(),
Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(int
constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)2)return 1;{void*
_tmp1EC=Cyc_Tcutil_compress(e);void*_tmp1ED;struct Cyc_List_List*_tmp1EE;void*
_tmp1EF;struct Cyc_Core_Opt*_tmp1F0;struct Cyc_Core_Opt*_tmp1F1;struct Cyc_Core_Opt**
_tmp1F2;struct Cyc_Core_Opt*_tmp1F3;_LL19E: if(_tmp1EC <= (void*)3?1:*((int*)
_tmp1EC)!= 19)goto _LL1A0;_tmp1ED=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1EC)->f1;_LL19F: if(constrain)return Cyc_Tcutil_unify(r,_tmp1ED);_tmp1ED=Cyc_Tcutil_compress(
_tmp1ED);if(r == _tmp1ED)return 1;{struct _tuple6 _tmp1F5=({struct _tuple6 _tmp1F4;
_tmp1F4.f1=r;_tmp1F4.f2=_tmp1ED;_tmp1F4;});void*_tmp1F6;struct Cyc_Absyn_Tvar*
_tmp1F7;void*_tmp1F8;struct Cyc_Absyn_Tvar*_tmp1F9;_LL1A9: _tmp1F6=_tmp1F5.f1;if(
_tmp1F6 <= (void*)3?1:*((int*)_tmp1F6)!= 1)goto _LL1AB;_tmp1F7=((struct Cyc_Absyn_VarType_struct*)
_tmp1F6)->f1;_tmp1F8=_tmp1F5.f2;if(_tmp1F8 <= (void*)3?1:*((int*)_tmp1F8)!= 1)
goto _LL1AB;_tmp1F9=((struct Cyc_Absyn_VarType_struct*)_tmp1F8)->f1;_LL1AA: return
Cyc_Absyn_tvar_cmp(_tmp1F7,_tmp1F9)== 0;_LL1AB:;_LL1AC: return 0;_LL1A8:;}_LL1A0:
if(_tmp1EC <= (void*)3?1:*((int*)_tmp1EC)!= 20)goto _LL1A2;_tmp1EE=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1EC)->f1;_LL1A1: for(0;_tmp1EE != 0;_tmp1EE=_tmp1EE->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp1EE->hd))return 1;}return 0;_LL1A2: if(_tmp1EC <= (void*)3?1:*((
int*)_tmp1EC)!= 21)goto _LL1A4;_tmp1EF=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1EC)->f1;_LL1A3: {void*_tmp1FA=Cyc_Tcutil_rgns_of(_tmp1EF);void*_tmp1FB;
_LL1AE: if(_tmp1FA <= (void*)3?1:*((int*)_tmp1FA)!= 21)goto _LL1B0;_tmp1FB=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1FA)->f1;_LL1AF: if(!constrain)return 0;{void*
_tmp1FC=Cyc_Tcutil_compress(_tmp1FB);struct Cyc_Core_Opt*_tmp1FD;struct Cyc_Core_Opt*
_tmp1FE;struct Cyc_Core_Opt**_tmp1FF;struct Cyc_Core_Opt*_tmp200;_LL1B3: if(_tmp1FC
<= (void*)3?1:*((int*)_tmp1FC)!= 0)goto _LL1B5;_tmp1FD=((struct Cyc_Absyn_Evar_struct*)
_tmp1FC)->f1;_tmp1FE=((struct Cyc_Absyn_Evar_struct*)_tmp1FC)->f2;_tmp1FF=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1FC)->f2;_tmp200=((struct Cyc_Absyn_Evar_struct*)
_tmp1FC)->f4;_LL1B4: {void*_tmp201=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp200);Cyc_Tcutil_occurs(_tmp201,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp200))->v,r);{void*_tmp202=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp204=_cycalloc(sizeof(*_tmp204));
_tmp204[0]=({struct Cyc_Absyn_JoinEff_struct _tmp205;_tmp205.tag=20;_tmp205.f1=({
void*_tmp206[2];_tmp206[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp207=
_cycalloc(sizeof(*_tmp207));_tmp207[0]=({struct Cyc_Absyn_AccessEff_struct _tmp208;
_tmp208.tag=19;_tmp208.f1=(void*)r;_tmp208;});_tmp207;});_tmp206[0]=_tmp201;Cyc_List_list(
_tag_arr(_tmp206,sizeof(void*),2));});_tmp205;});_tmp204;}));*_tmp1FF=({struct
Cyc_Core_Opt*_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203->v=(void*)_tmp202;
_tmp203;});return 1;}}_LL1B5:;_LL1B6: return 0;_LL1B2:;}_LL1B0:;_LL1B1: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp1FA);_LL1AD:;}_LL1A4: if(_tmp1EC <= (void*)3?1:*((int*)_tmp1EC)!= 
0)goto _LL1A6;_tmp1F0=((struct Cyc_Absyn_Evar_struct*)_tmp1EC)->f1;_tmp1F1=((
struct Cyc_Absyn_Evar_struct*)_tmp1EC)->f2;_tmp1F2=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1EC)->f2;_tmp1F3=((struct Cyc_Absyn_Evar_struct*)
_tmp1EC)->f4;_LL1A5: if(_tmp1F0 == 0?1:(void*)_tmp1F0->v != (void*)4)({void*_tmp209[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp20A="effect evar has wrong kind";_tag_arr(_tmp20A,sizeof(char),
_get_zero_arr_size(_tmp20A,27));}),_tag_arr(_tmp209,sizeof(void*),0));});if(!
constrain)return 0;{void*_tmp20B=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1F3);Cyc_Tcutil_occurs(_tmp20B,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1F3))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp20C=({struct Cyc_Absyn_JoinEff_struct*_tmp20E=_cycalloc(sizeof(*_tmp20E));
_tmp20E[0]=({struct Cyc_Absyn_JoinEff_struct _tmp20F;_tmp20F.tag=20;_tmp20F.f1=({
struct Cyc_List_List*_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210->hd=(void*)
_tmp20B;_tmp210->tl=({struct Cyc_List_List*_tmp211=_cycalloc(sizeof(*_tmp211));
_tmp211->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp212=_cycalloc(
sizeof(*_tmp212));_tmp212[0]=({struct Cyc_Absyn_AccessEff_struct _tmp213;_tmp213.tag=
19;_tmp213.f1=(void*)r;_tmp213;});_tmp212;}));_tmp211->tl=0;_tmp211;});_tmp210;});
_tmp20F;});_tmp20E;});*_tmp1F2=({struct Cyc_Core_Opt*_tmp20D=_cycalloc(sizeof(*
_tmp20D));_tmp20D->v=(void*)((void*)_tmp20C);_tmp20D;});return 1;}}_LL1A6:;_LL1A7:
return 0;_LL19D:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp214=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp215;void*_tmp216;struct Cyc_Core_Opt*_tmp217;struct
Cyc_Core_Opt*_tmp218;struct Cyc_Core_Opt**_tmp219;struct Cyc_Core_Opt*_tmp21A;
_LL1B8: if(_tmp214 <= (void*)3?1:*((int*)_tmp214)!= 19)goto _LL1BA;_LL1B9: return 0;
_LL1BA: if(_tmp214 <= (void*)3?1:*((int*)_tmp214)!= 20)goto _LL1BC;_tmp215=((struct
Cyc_Absyn_JoinEff_struct*)_tmp214)->f1;_LL1BB: for(0;_tmp215 != 0;_tmp215=_tmp215->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp215->hd))return 1;}
return 0;_LL1BC: if(_tmp214 <= (void*)3?1:*((int*)_tmp214)!= 21)goto _LL1BE;_tmp216=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp214)->f1;_LL1BD: _tmp216=Cyc_Tcutil_compress(
_tmp216);if(t == _tmp216)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,
_tmp216);{void*_tmp21B=Cyc_Tcutil_rgns_of(t);void*_tmp21C;_LL1C3: if(_tmp21B <= (
void*)3?1:*((int*)_tmp21B)!= 21)goto _LL1C5;_tmp21C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp21B)->f1;_LL1C4: {struct _tuple6 _tmp21E=({struct _tuple6 _tmp21D;_tmp21D.f1=Cyc_Tcutil_compress(
_tmp21C);_tmp21D.f2=_tmp216;_tmp21D;});void*_tmp21F;struct Cyc_Absyn_Tvar*_tmp220;
void*_tmp221;struct Cyc_Absyn_Tvar*_tmp222;_LL1C8: _tmp21F=_tmp21E.f1;if(_tmp21F <= (
void*)3?1:*((int*)_tmp21F)!= 1)goto _LL1CA;_tmp220=((struct Cyc_Absyn_VarType_struct*)
_tmp21F)->f1;_tmp221=_tmp21E.f2;if(_tmp221 <= (void*)3?1:*((int*)_tmp221)!= 1)
goto _LL1CA;_tmp222=((struct Cyc_Absyn_VarType_struct*)_tmp221)->f1;_LL1C9: return
Cyc_Tcutil_unify(t,_tmp216);_LL1CA:;_LL1CB: return _tmp21C == _tmp216;_LL1C7:;}
_LL1C5:;_LL1C6: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp21B);
_LL1C2:;}_LL1BE: if(_tmp214 <= (void*)3?1:*((int*)_tmp214)!= 0)goto _LL1C0;_tmp217=((
struct Cyc_Absyn_Evar_struct*)_tmp214)->f1;_tmp218=((struct Cyc_Absyn_Evar_struct*)
_tmp214)->f2;_tmp219=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp214)->f2;_tmp21A=((struct Cyc_Absyn_Evar_struct*)_tmp214)->f4;_LL1BF: if(
_tmp217 == 0?1:(void*)_tmp217->v != (void*)4)({void*_tmp223[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp224="effect evar has wrong kind";
_tag_arr(_tmp224,sizeof(char),_get_zero_arr_size(_tmp224,27));}),_tag_arr(
_tmp223,sizeof(void*),0));});if(!may_constrain_evars)return 0;{void*_tmp225=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp21A);Cyc_Tcutil_occurs(_tmp225,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp21A))->v,t);{struct
Cyc_Absyn_JoinEff_struct*_tmp226=({struct Cyc_Absyn_JoinEff_struct*_tmp228=
_cycalloc(sizeof(*_tmp228));_tmp228[0]=({struct Cyc_Absyn_JoinEff_struct _tmp229;
_tmp229.tag=20;_tmp229.f1=({struct Cyc_List_List*_tmp22A=_cycalloc(sizeof(*
_tmp22A));_tmp22A->hd=(void*)_tmp225;_tmp22A->tl=({struct Cyc_List_List*_tmp22B=
_cycalloc(sizeof(*_tmp22B));_tmp22B->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp22C=_cycalloc(sizeof(*_tmp22C));_tmp22C[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp22D;_tmp22D.tag=21;_tmp22D.f1=(void*)t;_tmp22D;});_tmp22C;}));_tmp22B->tl=0;
_tmp22B;});_tmp22A;});_tmp229;});_tmp228;});*_tmp219=({struct Cyc_Core_Opt*
_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227->v=(void*)((void*)_tmp226);_tmp227;});
return 1;}}_LL1C0:;_LL1C1: return 0;_LL1B7:;}}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp22E=e;struct Cyc_Absyn_Tvar*_tmp22F;struct Cyc_List_List*_tmp230;void*
_tmp231;struct Cyc_Core_Opt*_tmp232;struct Cyc_Core_Opt*_tmp233;struct Cyc_Core_Opt**
_tmp234;struct Cyc_Core_Opt*_tmp235;_LL1CD: if(_tmp22E <= (void*)3?1:*((int*)
_tmp22E)!= 1)goto _LL1CF;_tmp22F=((struct Cyc_Absyn_VarType_struct*)_tmp22E)->f1;
_LL1CE: return Cyc_Absyn_tvar_cmp(v,_tmp22F)== 0;_LL1CF: if(_tmp22E <= (void*)3?1:*((
int*)_tmp22E)!= 20)goto _LL1D1;_tmp230=((struct Cyc_Absyn_JoinEff_struct*)_tmp22E)->f1;
_LL1D0: for(0;_tmp230 != 0;_tmp230=_tmp230->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp230->hd))return 1;}return 0;_LL1D1: if(_tmp22E <= (
void*)3?1:*((int*)_tmp22E)!= 21)goto _LL1D3;_tmp231=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp22E)->f1;_LL1D2: {void*_tmp236=Cyc_Tcutil_rgns_of(_tmp231);void*_tmp237;
_LL1D8: if(_tmp236 <= (void*)3?1:*((int*)_tmp236)!= 21)goto _LL1DA;_tmp237=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp236)->f1;_LL1D9: if(!may_constrain_evars)
return 0;{void*_tmp238=Cyc_Tcutil_compress(_tmp237);struct Cyc_Core_Opt*_tmp239;
struct Cyc_Core_Opt*_tmp23A;struct Cyc_Core_Opt**_tmp23B;struct Cyc_Core_Opt*
_tmp23C;_LL1DD: if(_tmp238 <= (void*)3?1:*((int*)_tmp238)!= 0)goto _LL1DF;_tmp239=((
struct Cyc_Absyn_Evar_struct*)_tmp238)->f1;_tmp23A=((struct Cyc_Absyn_Evar_struct*)
_tmp238)->f2;_tmp23B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp238)->f2;_tmp23C=((struct Cyc_Absyn_Evar_struct*)_tmp238)->f4;_LL1DE: {void*
_tmp23D=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp23C);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp23C))->v,v))return 0;{void*_tmp23E=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp240=_cycalloc(sizeof(*_tmp240));
_tmp240[0]=({struct Cyc_Absyn_JoinEff_struct _tmp241;_tmp241.tag=20;_tmp241.f1=({
void*_tmp242[2];_tmp242[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp243=
_cycalloc(sizeof(*_tmp243));_tmp243[0]=({struct Cyc_Absyn_VarType_struct _tmp244;
_tmp244.tag=1;_tmp244.f1=v;_tmp244;});_tmp243;});_tmp242[0]=_tmp23D;Cyc_List_list(
_tag_arr(_tmp242,sizeof(void*),2));});_tmp241;});_tmp240;}));*_tmp23B=({struct
Cyc_Core_Opt*_tmp23F=_cycalloc(sizeof(*_tmp23F));_tmp23F->v=(void*)_tmp23E;
_tmp23F;});return 1;}}_LL1DF:;_LL1E0: return 0;_LL1DC:;}_LL1DA:;_LL1DB: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp236);_LL1D7:;}_LL1D3: if(_tmp22E <= (void*)3?1:*((int*)
_tmp22E)!= 0)goto _LL1D5;_tmp232=((struct Cyc_Absyn_Evar_struct*)_tmp22E)->f1;
_tmp233=((struct Cyc_Absyn_Evar_struct*)_tmp22E)->f2;_tmp234=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp22E)->f2;_tmp235=((struct Cyc_Absyn_Evar_struct*)
_tmp22E)->f4;_LL1D4: if(_tmp232 == 0?1:(void*)_tmp232->v != (void*)4)({void*_tmp245[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp246="effect evar has wrong kind";_tag_arr(_tmp246,sizeof(char),
_get_zero_arr_size(_tmp246,27));}),_tag_arr(_tmp245,sizeof(void*),0));});{void*
_tmp247=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp235);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp235))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*
_tmp248=({struct Cyc_Absyn_JoinEff_struct*_tmp24A=_cycalloc(sizeof(*_tmp24A));
_tmp24A[0]=({struct Cyc_Absyn_JoinEff_struct _tmp24B;_tmp24B.tag=20;_tmp24B.f1=({
struct Cyc_List_List*_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C->hd=(void*)
_tmp247;_tmp24C->tl=({struct Cyc_List_List*_tmp24D=_cycalloc(sizeof(*_tmp24D));
_tmp24D->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp24E=_cycalloc(
sizeof(*_tmp24E));_tmp24E[0]=({struct Cyc_Absyn_VarType_struct _tmp24F;_tmp24F.tag=
1;_tmp24F.f1=v;_tmp24F;});_tmp24E;}));_tmp24D->tl=0;_tmp24D;});_tmp24C;});
_tmp24B;});_tmp24A;});*_tmp234=({struct Cyc_Core_Opt*_tmp249=_cycalloc(sizeof(*
_tmp249));_tmp249->v=(void*)((void*)_tmp248);_tmp249;});return 1;}}_LL1D5:;_LL1D6:
return 0;_LL1CC:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp250=e;struct Cyc_List_List*_tmp251;void*_tmp252;_LL1E2: if(_tmp250 <= (
void*)3?1:*((int*)_tmp250)!= 20)goto _LL1E4;_tmp251=((struct Cyc_Absyn_JoinEff_struct*)
_tmp250)->f1;_LL1E3: for(0;_tmp251 != 0;_tmp251=_tmp251->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp251->hd))return 1;}return 0;_LL1E4: if(_tmp250 <= (void*)3?1:*((int*)
_tmp250)!= 21)goto _LL1E6;_tmp252=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp250)->f1;_LL1E5: {void*_tmp253=Cyc_Tcutil_rgns_of(_tmp252);void*_tmp254;
_LL1EB: if(_tmp253 <= (void*)3?1:*((int*)_tmp253)!= 21)goto _LL1ED;_tmp254=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp253)->f1;_LL1EC: return 0;_LL1ED:;_LL1EE:
return Cyc_Tcutil_evar_in_effect(evar,_tmp253);_LL1EA:;}_LL1E6: if(_tmp250 <= (void*)
3?1:*((int*)_tmp250)!= 0)goto _LL1E8;_LL1E7: return evar == e;_LL1E8:;_LL1E9: return 0;
_LL1E1:;}}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
void*_tmp255=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp256;void*_tmp257;
struct Cyc_Absyn_Tvar*_tmp258;void*_tmp259;struct Cyc_Core_Opt*_tmp25A;struct Cyc_Core_Opt**
_tmp25B;struct Cyc_Core_Opt*_tmp25C;_LL1F0: if(_tmp255 <= (void*)3?1:*((int*)
_tmp255)!= 20)goto _LL1F2;_tmp256=((struct Cyc_Absyn_JoinEff_struct*)_tmp255)->f1;
_LL1F1: for(0;_tmp256 != 0;_tmp256=_tmp256->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp256->hd,e2))return 0;}return 1;_LL1F2: if(_tmp255 <= (
void*)3?1:*((int*)_tmp255)!= 19)goto _LL1F4;_tmp257=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp255)->f1;_LL1F3: return Cyc_Tcutil_region_in_effect(0,_tmp257,e2)?1:(
may_constrain_evars?Cyc_Tcutil_unify(_tmp257,(void*)2): 0);_LL1F4: if(_tmp255 <= (
void*)3?1:*((int*)_tmp255)!= 1)goto _LL1F6;_tmp258=((struct Cyc_Absyn_VarType_struct*)
_tmp255)->f1;_LL1F5: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,
_tmp258,e2);_LL1F6: if(_tmp255 <= (void*)3?1:*((int*)_tmp255)!= 21)goto _LL1F8;
_tmp259=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp255)->f1;_LL1F7: {void*
_tmp25D=Cyc_Tcutil_rgns_of(_tmp259);void*_tmp25E;_LL1FD: if(_tmp25D <= (void*)3?1:*((
int*)_tmp25D)!= 21)goto _LL1FF;_tmp25E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp25D)->f1;_LL1FE: return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp25E,
e2)?1:(may_constrain_evars?Cyc_Tcutil_unify(_tmp25E,Cyc_Absyn_sint_typ): 0);
_LL1FF:;_LL200: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp25D,e2);
_LL1FC:;}_LL1F8: if(_tmp255 <= (void*)3?1:*((int*)_tmp255)!= 0)goto _LL1FA;_tmp25A=((
struct Cyc_Absyn_Evar_struct*)_tmp255)->f2;_tmp25B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp255)->f2;_tmp25C=((struct Cyc_Absyn_Evar_struct*)
_tmp255)->f4;_LL1F9: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp25B=({struct Cyc_Core_Opt*
_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F->v=(void*)Cyc_Absyn_empty_effect;
_tmp25F;});return 1;_LL1FA:;_LL1FB:({struct Cyc_String_pa_struct _tmp262;_tmp262.tag=
0;_tmp262.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(e1));{
void*_tmp260[1]={& _tmp262};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp261="subset_effect: bad effect: %s";_tag_arr(
_tmp261,sizeof(char),_get_zero_arr_size(_tmp261,30));}),_tag_arr(_tmp260,sizeof(
void*),1));}});_LL1EF:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2)?Cyc_Tcutil_subset_effect(
0,e2,e1): 0)return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)?Cyc_Tcutil_subset_effect(
1,e2,e1): 0)return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple6 _tmp264;void*_tmp265;void*_tmp266;struct _tuple6*_tmp263=(struct
_tuple6*)r1->hd;_tmp264=*_tmp263;_tmp265=_tmp264.f1;_tmp266=_tmp264.f2;{int found=
_tmp265 == (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0?!found: 0;r2=r2->tl){
struct _tuple6 _tmp268;void*_tmp269;void*_tmp26A;struct _tuple6*_tmp267=(struct
_tuple6*)r2->hd;_tmp268=*_tmp267;_tmp269=_tmp268.f1;_tmp26A=_tmp268.f2;if(Cyc_Tcutil_unify(
_tmp265,_tmp269)?Cyc_Tcutil_unify(_tmp266,_tmp26A): 0){found=1;break;}}}if(!found)
return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)?Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1): 0;}struct _tuple11{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _tagged_arr)_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp26B=t1;struct Cyc_Core_Opt*_tmp26C;struct Cyc_Core_Opt*
_tmp26D;struct Cyc_Core_Opt**_tmp26E;struct Cyc_Core_Opt*_tmp26F;_LL202: if(_tmp26B
<= (void*)3?1:*((int*)_tmp26B)!= 0)goto _LL204;_tmp26C=((struct Cyc_Absyn_Evar_struct*)
_tmp26B)->f1;_tmp26D=((struct Cyc_Absyn_Evar_struct*)_tmp26B)->f2;_tmp26E=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp26B)->f2;_tmp26F=((struct Cyc_Absyn_Evar_struct*)
_tmp26B)->f4;_LL203: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp26F))->v,t2);{void*_tmp270=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp270,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp26C))->v)){*_tmp26E=({struct Cyc_Core_Opt*_tmp271=_cycalloc(sizeof(*_tmp271));
_tmp271->v=(void*)t2;_tmp271;});return;}else{{void*_tmp272=t2;struct Cyc_Core_Opt*
_tmp273;struct Cyc_Core_Opt**_tmp274;struct Cyc_Core_Opt*_tmp275;struct Cyc_Absyn_PtrInfo
_tmp276;_LL207: if(_tmp272 <= (void*)3?1:*((int*)_tmp272)!= 0)goto _LL209;_tmp273=((
struct Cyc_Absyn_Evar_struct*)_tmp272)->f2;_tmp274=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp272)->f2;_tmp275=((struct Cyc_Absyn_Evar_struct*)
_tmp272)->f4;_LL208: {struct Cyc_List_List*_tmp277=(struct Cyc_List_List*)_tmp26F->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp275))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp277,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp278="(type variable would escape scope)";_tag_arr(_tmp278,sizeof(
char),_get_zero_arr_size(_tmp278,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}}}
if(Cyc_Tcutil_kind_leq((void*)_tmp26C->v,_tmp270)){*_tmp274=({struct Cyc_Core_Opt*
_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->v=(void*)t1;_tmp279;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp27A="(kinds are incompatible)";_tag_arr(_tmp27A,sizeof(char),
_get_zero_arr_size(_tmp27A,25));});goto _LL206;}_LL209: if(_tmp272 <= (void*)3?1:*((
int*)_tmp272)!= 4)goto _LL20B;_tmp276=((struct Cyc_Absyn_PointerType_struct*)
_tmp272)->f1;if(!((void*)_tmp26C->v == (void*)2))goto _LL20B;_LL20A: {struct Cyc_Absyn_Conref*
_tmp27B=Cyc_Absyn_compress_conref((_tmp276.ptr_atts).bounds);{void*_tmp27C=(void*)
_tmp27B->v;_LL20E: if((int)_tmp27C != 0)goto _LL210;_LL20F:(void*)(_tmp27B->v=(void*)((
void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp27D=_cycalloc(sizeof(*_tmp27D));
_tmp27D[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp27E;_tmp27E.tag=0;_tmp27E.f1=(
void*)Cyc_Absyn_bounds_one;_tmp27E;});_tmp27D;})));*_tmp26E=({struct Cyc_Core_Opt*
_tmp27F=_cycalloc(sizeof(*_tmp27F));_tmp27F->v=(void*)t2;_tmp27F;});return;
_LL210:;_LL211: goto _LL20D;_LL20D:;}goto _LL206;}_LL20B:;_LL20C: goto _LL206;_LL206:;}
Cyc_Tcutil_failure_reason=({const char*_tmp280="(kinds are incompatible)";
_tag_arr(_tmp280,sizeof(char),_get_zero_arr_size(_tmp280,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}_LL204:;_LL205: goto _LL201;_LL201:;}{struct _tuple6
_tmp282=({struct _tuple6 _tmp281;_tmp281.f1=t2;_tmp281.f2=t1;_tmp281;});void*
_tmp283;void*_tmp284;void*_tmp285;void*_tmp286;struct Cyc_Absyn_Tvar*_tmp287;void*
_tmp288;struct Cyc_Absyn_Tvar*_tmp289;void*_tmp28A;struct Cyc_Absyn_AggrInfo
_tmp28B;void*_tmp28C;struct Cyc_List_List*_tmp28D;void*_tmp28E;struct Cyc_Absyn_AggrInfo
_tmp28F;void*_tmp290;struct Cyc_List_List*_tmp291;void*_tmp292;struct _tuple1*
_tmp293;void*_tmp294;struct _tuple1*_tmp295;void*_tmp296;struct Cyc_List_List*
_tmp297;void*_tmp298;struct Cyc_List_List*_tmp299;void*_tmp29A;struct Cyc_Absyn_TunionInfo
_tmp29B;void*_tmp29C;struct Cyc_Absyn_Tuniondecl**_tmp29D;struct Cyc_Absyn_Tuniondecl*
_tmp29E;struct Cyc_List_List*_tmp29F;void*_tmp2A0;void*_tmp2A1;struct Cyc_Absyn_TunionInfo
_tmp2A2;void*_tmp2A3;struct Cyc_Absyn_Tuniondecl**_tmp2A4;struct Cyc_Absyn_Tuniondecl*
_tmp2A5;struct Cyc_List_List*_tmp2A6;void*_tmp2A7;void*_tmp2A8;struct Cyc_Absyn_TunionFieldInfo
_tmp2A9;void*_tmp2AA;struct Cyc_Absyn_Tuniondecl*_tmp2AB;struct Cyc_Absyn_Tunionfield*
_tmp2AC;struct Cyc_List_List*_tmp2AD;void*_tmp2AE;struct Cyc_Absyn_TunionFieldInfo
_tmp2AF;void*_tmp2B0;struct Cyc_Absyn_Tuniondecl*_tmp2B1;struct Cyc_Absyn_Tunionfield*
_tmp2B2;struct Cyc_List_List*_tmp2B3;void*_tmp2B4;struct Cyc_Absyn_PtrInfo _tmp2B5;
void*_tmp2B6;struct Cyc_Absyn_Tqual _tmp2B7;struct Cyc_Absyn_PtrAtts _tmp2B8;void*
_tmp2B9;struct Cyc_Absyn_Conref*_tmp2BA;struct Cyc_Absyn_Conref*_tmp2BB;struct Cyc_Absyn_Conref*
_tmp2BC;void*_tmp2BD;struct Cyc_Absyn_PtrInfo _tmp2BE;void*_tmp2BF;struct Cyc_Absyn_Tqual
_tmp2C0;struct Cyc_Absyn_PtrAtts _tmp2C1;void*_tmp2C2;struct Cyc_Absyn_Conref*
_tmp2C3;struct Cyc_Absyn_Conref*_tmp2C4;struct Cyc_Absyn_Conref*_tmp2C5;void*
_tmp2C6;void*_tmp2C7;void*_tmp2C8;void*_tmp2C9;void*_tmp2CA;void*_tmp2CB;void*
_tmp2CC;void*_tmp2CD;void*_tmp2CE;int _tmp2CF;void*_tmp2D0;int _tmp2D1;void*
_tmp2D2;void*_tmp2D3;void*_tmp2D4;void*_tmp2D5;void*_tmp2D6;int _tmp2D7;void*
_tmp2D8;int _tmp2D9;void*_tmp2DA;void*_tmp2DB;void*_tmp2DC;void*_tmp2DD;void*
_tmp2DE;struct Cyc_Absyn_ArrayInfo _tmp2DF;void*_tmp2E0;struct Cyc_Absyn_Tqual
_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E2;struct Cyc_Absyn_Conref*_tmp2E3;void*_tmp2E4;
struct Cyc_Absyn_ArrayInfo _tmp2E5;void*_tmp2E6;struct Cyc_Absyn_Tqual _tmp2E7;
struct Cyc_Absyn_Exp*_tmp2E8;struct Cyc_Absyn_Conref*_tmp2E9;void*_tmp2EA;struct
Cyc_Absyn_FnInfo _tmp2EB;struct Cyc_List_List*_tmp2EC;struct Cyc_Core_Opt*_tmp2ED;
void*_tmp2EE;struct Cyc_List_List*_tmp2EF;int _tmp2F0;struct Cyc_Absyn_VarargInfo*
_tmp2F1;struct Cyc_List_List*_tmp2F2;struct Cyc_List_List*_tmp2F3;void*_tmp2F4;
struct Cyc_Absyn_FnInfo _tmp2F5;struct Cyc_List_List*_tmp2F6;struct Cyc_Core_Opt*
_tmp2F7;void*_tmp2F8;struct Cyc_List_List*_tmp2F9;int _tmp2FA;struct Cyc_Absyn_VarargInfo*
_tmp2FB;struct Cyc_List_List*_tmp2FC;struct Cyc_List_List*_tmp2FD;void*_tmp2FE;
struct Cyc_List_List*_tmp2FF;void*_tmp300;struct Cyc_List_List*_tmp301;void*
_tmp302;void*_tmp303;struct Cyc_List_List*_tmp304;void*_tmp305;void*_tmp306;
struct Cyc_List_List*_tmp307;void*_tmp308;void*_tmp309;void*_tmp30A;void*_tmp30B;
void*_tmp30C;void*_tmp30D;void*_tmp30E;void*_tmp30F;void*_tmp310;void*_tmp311;
void*_tmp312;void*_tmp313;_LL213: _tmp283=_tmp282.f1;if(_tmp283 <= (void*)3?1:*((
int*)_tmp283)!= 0)goto _LL215;_LL214: Cyc_Tcutil_unify_it(t2,t1);return;_LL215:
_tmp284=_tmp282.f1;if((int)_tmp284 != 0)goto _LL217;_tmp285=_tmp282.f2;if((int)
_tmp285 != 0)goto _LL217;_LL216: return;_LL217: _tmp286=_tmp282.f1;if(_tmp286 <= (
void*)3?1:*((int*)_tmp286)!= 1)goto _LL219;_tmp287=((struct Cyc_Absyn_VarType_struct*)
_tmp286)->f1;_tmp288=_tmp282.f2;if(_tmp288 <= (void*)3?1:*((int*)_tmp288)!= 1)
goto _LL219;_tmp289=((struct Cyc_Absyn_VarType_struct*)_tmp288)->f1;_LL218: {
struct _tagged_arr*_tmp314=_tmp287->name;struct _tagged_arr*_tmp315=_tmp289->name;
int _tmp316=*((int*)_check_null(_tmp287->identity));int _tmp317=*((int*)
_check_null(_tmp289->identity));void*_tmp318=Cyc_Tcutil_tvar_kind(_tmp287);void*
_tmp319=Cyc_Tcutil_tvar_kind(_tmp289);if(_tmp317 == _tmp316?Cyc_strptrcmp(_tmp314,
_tmp315)== 0: 0){if(_tmp318 != _tmp319)({struct Cyc_String_pa_struct _tmp31E;_tmp31E.tag=
0;_tmp31E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp319));{struct Cyc_String_pa_struct _tmp31D;_tmp31D.tag=0;_tmp31D.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp318));{struct Cyc_String_pa_struct
_tmp31C;_tmp31C.tag=0;_tmp31C.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp314);{
void*_tmp31A[3]={& _tmp31C,& _tmp31D,& _tmp31E};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp31B="same type variable %s has kinds %s and %s";
_tag_arr(_tmp31B,sizeof(char),_get_zero_arr_size(_tmp31B,42));}),_tag_arr(
_tmp31A,sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=({const char*
_tmp31F="(variable types are not the same)";_tag_arr(_tmp31F,sizeof(char),
_get_zero_arr_size(_tmp31F,34));});goto _LL212;}_LL219: _tmp28A=_tmp282.f1;if(
_tmp28A <= (void*)3?1:*((int*)_tmp28A)!= 10)goto _LL21B;_tmp28B=((struct Cyc_Absyn_AggrType_struct*)
_tmp28A)->f1;_tmp28C=(void*)_tmp28B.aggr_info;_tmp28D=_tmp28B.targs;_tmp28E=
_tmp282.f2;if(_tmp28E <= (void*)3?1:*((int*)_tmp28E)!= 10)goto _LL21B;_tmp28F=((
struct Cyc_Absyn_AggrType_struct*)_tmp28E)->f1;_tmp290=(void*)_tmp28F.aggr_info;
_tmp291=_tmp28F.targs;_LL21A: {void*_tmp321;struct _tuple1*_tmp322;struct _tuple5
_tmp320=Cyc_Absyn_aggr_kinded_name(_tmp290);_tmp321=_tmp320.f1;_tmp322=_tmp320.f2;{
void*_tmp324;struct _tuple1*_tmp325;struct _tuple5 _tmp323=Cyc_Absyn_aggr_kinded_name(
_tmp28C);_tmp324=_tmp323.f1;_tmp325=_tmp323.f2;if(_tmp321 != _tmp324){Cyc_Tcutil_failure_reason=({
const char*_tmp326="(struct and union type)";_tag_arr(_tmp326,sizeof(char),
_get_zero_arr_size(_tmp326,24));});goto _LL212;}if(Cyc_Absyn_qvar_cmp(_tmp322,
_tmp325)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp327="(different type name)";
_tag_arr(_tmp327,sizeof(char),_get_zero_arr_size(_tmp327,22));});goto _LL212;}Cyc_Tcutil_unify_list(
_tmp291,_tmp28D);return;}}_LL21B: _tmp292=_tmp282.f1;if(_tmp292 <= (void*)3?1:*((
int*)_tmp292)!= 12)goto _LL21D;_tmp293=((struct Cyc_Absyn_EnumType_struct*)_tmp292)->f1;
_tmp294=_tmp282.f2;if(_tmp294 <= (void*)3?1:*((int*)_tmp294)!= 12)goto _LL21D;
_tmp295=((struct Cyc_Absyn_EnumType_struct*)_tmp294)->f1;_LL21C: if(Cyc_Absyn_qvar_cmp(
_tmp293,_tmp295)== 0)return;Cyc_Tcutil_failure_reason=({const char*_tmp328="(different enum types)";
_tag_arr(_tmp328,sizeof(char),_get_zero_arr_size(_tmp328,23));});goto _LL212;
_LL21D: _tmp296=_tmp282.f1;if(_tmp296 <= (void*)3?1:*((int*)_tmp296)!= 13)goto
_LL21F;_tmp297=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp296)->f1;_tmp298=
_tmp282.f2;if(_tmp298 <= (void*)3?1:*((int*)_tmp298)!= 13)goto _LL21F;_tmp299=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp298)->f1;_LL21E: {int bad=0;for(0;
_tmp297 != 0?_tmp299 != 0: 0;(_tmp297=_tmp297->tl,_tmp299=_tmp299->tl)){struct Cyc_Absyn_Enumfield*
_tmp329=(struct Cyc_Absyn_Enumfield*)_tmp297->hd;struct Cyc_Absyn_Enumfield*
_tmp32A=(struct Cyc_Absyn_Enumfield*)_tmp299->hd;if(Cyc_Absyn_qvar_cmp(_tmp329->name,
_tmp32A->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp32B="(different names for enum fields)";
_tag_arr(_tmp32B,sizeof(char),_get_zero_arr_size(_tmp32B,34));});bad=1;break;}
if(_tmp329->tag == _tmp32A->tag)continue;if(_tmp329->tag == 0?1: _tmp32A->tag == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp32C="(different tag values for enum fields)";
_tag_arr(_tmp32C,sizeof(char),_get_zero_arr_size(_tmp32C,39));});bad=1;break;}
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp329->tag),(
struct Cyc_Absyn_Exp*)_check_null(_tmp32A->tag))){Cyc_Tcutil_failure_reason=({
const char*_tmp32D="(different tag values for enum fields)";_tag_arr(_tmp32D,
sizeof(char),_get_zero_arr_size(_tmp32D,39));});bad=1;break;}}if(bad)goto _LL212;
if(_tmp297 == 0?_tmp299 == 0: 0)return;Cyc_Tcutil_failure_reason=({const char*
_tmp32E="(different number of fields for enums)";_tag_arr(_tmp32E,sizeof(char),
_get_zero_arr_size(_tmp32E,39));});goto _LL212;}_LL21F: _tmp29A=_tmp282.f1;if(
_tmp29A <= (void*)3?1:*((int*)_tmp29A)!= 2)goto _LL221;_tmp29B=((struct Cyc_Absyn_TunionType_struct*)
_tmp29A)->f1;_tmp29C=(void*)_tmp29B.tunion_info;if(*((int*)_tmp29C)!= 1)goto
_LL221;_tmp29D=((struct Cyc_Absyn_KnownTunion_struct*)_tmp29C)->f1;_tmp29E=*
_tmp29D;_tmp29F=_tmp29B.targs;_tmp2A0=(void*)_tmp29B.rgn;_tmp2A1=_tmp282.f2;if(
_tmp2A1 <= (void*)3?1:*((int*)_tmp2A1)!= 2)goto _LL221;_tmp2A2=((struct Cyc_Absyn_TunionType_struct*)
_tmp2A1)->f1;_tmp2A3=(void*)_tmp2A2.tunion_info;if(*((int*)_tmp2A3)!= 1)goto
_LL221;_tmp2A4=((struct Cyc_Absyn_KnownTunion_struct*)_tmp2A3)->f1;_tmp2A5=*
_tmp2A4;_tmp2A6=_tmp2A2.targs;_tmp2A7=(void*)_tmp2A2.rgn;_LL220: if(_tmp29E == 
_tmp2A5?1: Cyc_Absyn_qvar_cmp(_tmp29E->name,_tmp2A5->name)== 0){Cyc_Tcutil_unify_it(
_tmp2A7,_tmp2A0);Cyc_Tcutil_unify_list(_tmp2A6,_tmp29F);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp32F="(different tunion types)";_tag_arr(_tmp32F,sizeof(char),
_get_zero_arr_size(_tmp32F,25));});goto _LL212;_LL221: _tmp2A8=_tmp282.f1;if(
_tmp2A8 <= (void*)3?1:*((int*)_tmp2A8)!= 3)goto _LL223;_tmp2A9=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2A8)->f1;_tmp2AA=(void*)_tmp2A9.field_info;if(*((int*)_tmp2AA)!= 1)goto
_LL223;_tmp2AB=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2AA)->f1;_tmp2AC=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2AA)->f2;_tmp2AD=_tmp2A9.targs;
_tmp2AE=_tmp282.f2;if(_tmp2AE <= (void*)3?1:*((int*)_tmp2AE)!= 3)goto _LL223;
_tmp2AF=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp2AE)->f1;_tmp2B0=(void*)
_tmp2AF.field_info;if(*((int*)_tmp2B0)!= 1)goto _LL223;_tmp2B1=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp2B0)->f1;_tmp2B2=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2B0)->f2;
_tmp2B3=_tmp2AF.targs;_LL222: if((_tmp2AB == _tmp2B1?1: Cyc_Absyn_qvar_cmp(_tmp2AB->name,
_tmp2B1->name)== 0)?_tmp2AC == _tmp2B2?1: Cyc_Absyn_qvar_cmp(_tmp2AC->name,_tmp2B2->name)
== 0: 0){Cyc_Tcutil_unify_list(_tmp2B3,_tmp2AD);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp330="(different tunion field types)";_tag_arr(_tmp330,sizeof(char),
_get_zero_arr_size(_tmp330,31));});goto _LL212;_LL223: _tmp2B4=_tmp282.f1;if(
_tmp2B4 <= (void*)3?1:*((int*)_tmp2B4)!= 4)goto _LL225;_tmp2B5=((struct Cyc_Absyn_PointerType_struct*)
_tmp2B4)->f1;_tmp2B6=(void*)_tmp2B5.elt_typ;_tmp2B7=_tmp2B5.elt_tq;_tmp2B8=
_tmp2B5.ptr_atts;_tmp2B9=(void*)_tmp2B8.rgn;_tmp2BA=_tmp2B8.nullable;_tmp2BB=
_tmp2B8.bounds;_tmp2BC=_tmp2B8.zero_term;_tmp2BD=_tmp282.f2;if(_tmp2BD <= (void*)
3?1:*((int*)_tmp2BD)!= 4)goto _LL225;_tmp2BE=((struct Cyc_Absyn_PointerType_struct*)
_tmp2BD)->f1;_tmp2BF=(void*)_tmp2BE.elt_typ;_tmp2C0=_tmp2BE.elt_tq;_tmp2C1=
_tmp2BE.ptr_atts;_tmp2C2=(void*)_tmp2C1.rgn;_tmp2C3=_tmp2C1.nullable;_tmp2C4=
_tmp2C1.bounds;_tmp2C5=_tmp2C1.zero_term;_LL224: Cyc_Tcutil_unify_it(_tmp2BF,
_tmp2B6);Cyc_Tcutil_unify_it(_tmp2B9,_tmp2C2);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2C5,
_tmp2BC,({const char*_tmp331="(not both zero terminated)";_tag_arr(_tmp331,
sizeof(char),_get_zero_arr_size(_tmp331,27));}));Cyc_Tcutil_unify_tqual(_tmp2C0,
_tmp2BF,_tmp2B7,_tmp2B6);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp2C4,_tmp2BB,({const char*_tmp332="(different pointer bounds)";_tag_arr(
_tmp332,sizeof(char),_get_zero_arr_size(_tmp332,27));}));{void*_tmp333=(void*)(
Cyc_Absyn_compress_conref(_tmp2C4))->v;void*_tmp334;_LL24C: if(_tmp333 <= (void*)1?
1:*((int*)_tmp333)!= 0)goto _LL24E;_tmp334=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp333)->f1;if((int)_tmp334 != 0)goto _LL24E;_LL24D: return;_LL24E:;_LL24F: goto
_LL24B;_LL24B:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2C3,
_tmp2BA,({const char*_tmp335="(incompatible pointer types)";_tag_arr(_tmp335,
sizeof(char),_get_zero_arr_size(_tmp335,29));}));return;_LL225: _tmp2C6=_tmp282.f1;
if(_tmp2C6 <= (void*)3?1:*((int*)_tmp2C6)!= 5)goto _LL227;_tmp2C7=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp2C6)->f1;_tmp2C8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2C6)->f2;_tmp2C9=_tmp282.f2;if(_tmp2C9 <= (void*)3?1:*((int*)_tmp2C9)!= 5)
goto _LL227;_tmp2CA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C9)->f1;_tmp2CB=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C9)->f2;_LL226: if(_tmp2CA == _tmp2C7?
_tmp2CB == _tmp2C8: 0)return;Cyc_Tcutil_failure_reason=({const char*_tmp336="(different integral types)";
_tag_arr(_tmp336,sizeof(char),_get_zero_arr_size(_tmp336,27));});goto _LL212;
_LL227: _tmp2CC=_tmp282.f1;if((int)_tmp2CC != 1)goto _LL229;_tmp2CD=_tmp282.f2;if((
int)_tmp2CD != 1)goto _LL229;_LL228: return;_LL229: _tmp2CE=_tmp282.f1;if(_tmp2CE <= (
void*)3?1:*((int*)_tmp2CE)!= 6)goto _LL22B;_tmp2CF=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2CE)->f1;_tmp2D0=_tmp282.f2;if(_tmp2D0 <= (void*)3?1:*((int*)_tmp2D0)!= 6)
goto _LL22B;_tmp2D1=((struct Cyc_Absyn_DoubleType_struct*)_tmp2D0)->f1;_LL22A: if(
_tmp2CF == _tmp2D1)return;goto _LL212;_LL22B: _tmp2D2=_tmp282.f1;if(_tmp2D2 <= (void*)
3?1:*((int*)_tmp2D2)!= 14)goto _LL22D;_tmp2D3=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp2D2)->f1;_tmp2D4=_tmp282.f2;if(_tmp2D4 <= (void*)3?1:*((int*)_tmp2D4)!= 14)
goto _LL22D;_tmp2D5=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2D4)->f1;
_LL22C: Cyc_Tcutil_unify_it(_tmp2D3,_tmp2D5);return;_LL22D: _tmp2D6=_tmp282.f1;if(
_tmp2D6 <= (void*)3?1:*((int*)_tmp2D6)!= 18)goto _LL22F;_tmp2D7=((struct Cyc_Absyn_TypeInt_struct*)
_tmp2D6)->f1;_tmp2D8=_tmp282.f2;if(_tmp2D8 <= (void*)3?1:*((int*)_tmp2D8)!= 18)
goto _LL22F;_tmp2D9=((struct Cyc_Absyn_TypeInt_struct*)_tmp2D8)->f1;_LL22E: if(
_tmp2D7 == _tmp2D9)return;Cyc_Tcutil_failure_reason=({const char*_tmp337="(different type integers)";
_tag_arr(_tmp337,sizeof(char),_get_zero_arr_size(_tmp337,26));});goto _LL212;
_LL22F: _tmp2DA=_tmp282.f1;if(_tmp2DA <= (void*)3?1:*((int*)_tmp2DA)!= 17)goto
_LL231;_tmp2DB=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp2DA)->f1;_tmp2DC=
_tmp282.f2;if(_tmp2DC <= (void*)3?1:*((int*)_tmp2DC)!= 17)goto _LL231;_tmp2DD=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp2DC)->f1;_LL230: Cyc_Tcutil_unify_it(
_tmp2DB,_tmp2DD);return;_LL231: _tmp2DE=_tmp282.f1;if(_tmp2DE <= (void*)3?1:*((int*)
_tmp2DE)!= 7)goto _LL233;_tmp2DF=((struct Cyc_Absyn_ArrayType_struct*)_tmp2DE)->f1;
_tmp2E0=(void*)_tmp2DF.elt_type;_tmp2E1=_tmp2DF.tq;_tmp2E2=_tmp2DF.num_elts;
_tmp2E3=_tmp2DF.zero_term;_tmp2E4=_tmp282.f2;if(_tmp2E4 <= (void*)3?1:*((int*)
_tmp2E4)!= 7)goto _LL233;_tmp2E5=((struct Cyc_Absyn_ArrayType_struct*)_tmp2E4)->f1;
_tmp2E6=(void*)_tmp2E5.elt_type;_tmp2E7=_tmp2E5.tq;_tmp2E8=_tmp2E5.num_elts;
_tmp2E9=_tmp2E5.zero_term;_LL232: Cyc_Tcutil_unify_it(_tmp2E6,_tmp2E0);Cyc_Tcutil_unify_tqual(
_tmp2E7,_tmp2E6,_tmp2E1,_tmp2E0);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2E3,
_tmp2E9,({const char*_tmp338="(not both zero terminated)";_tag_arr(_tmp338,
sizeof(char),_get_zero_arr_size(_tmp338,27));}));if(_tmp2E2 == _tmp2E8)return;if(
_tmp2E2 == 0?1: _tmp2E8 == 0)goto _LL212;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_tmp2E2,(struct Cyc_Absyn_Exp*)_tmp2E8))return;Cyc_Tcutil_failure_reason=({const
char*_tmp339="(different array sizes)";_tag_arr(_tmp339,sizeof(char),
_get_zero_arr_size(_tmp339,24));});goto _LL212;_LL233: _tmp2EA=_tmp282.f1;if(
_tmp2EA <= (void*)3?1:*((int*)_tmp2EA)!= 8)goto _LL235;_tmp2EB=((struct Cyc_Absyn_FnType_struct*)
_tmp2EA)->f1;_tmp2EC=_tmp2EB.tvars;_tmp2ED=_tmp2EB.effect;_tmp2EE=(void*)_tmp2EB.ret_typ;
_tmp2EF=_tmp2EB.args;_tmp2F0=_tmp2EB.c_varargs;_tmp2F1=_tmp2EB.cyc_varargs;
_tmp2F2=_tmp2EB.rgn_po;_tmp2F3=_tmp2EB.attributes;_tmp2F4=_tmp282.f2;if(_tmp2F4
<= (void*)3?1:*((int*)_tmp2F4)!= 8)goto _LL235;_tmp2F5=((struct Cyc_Absyn_FnType_struct*)
_tmp2F4)->f1;_tmp2F6=_tmp2F5.tvars;_tmp2F7=_tmp2F5.effect;_tmp2F8=(void*)_tmp2F5.ret_typ;
_tmp2F9=_tmp2F5.args;_tmp2FA=_tmp2F5.c_varargs;_tmp2FB=_tmp2F5.cyc_varargs;
_tmp2FC=_tmp2F5.rgn_po;_tmp2FD=_tmp2F5.attributes;_LL234: {int done=0;{struct
_RegionHandle _tmp33A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp33A;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp2F6 != 0){if(_tmp2EC == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp33B="(second function type has too few type variables)";
_tag_arr(_tmp33B,sizeof(char),_get_zero_arr_size(_tmp33B,50));});(int)_throw((
void*)Cyc_Tcutil_Unify);}{void*_tmp33C=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp2F6->hd);void*_tmp33D=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2EC->hd);
if(_tmp33C != _tmp33D){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp342;_tmp342.tag=0;_tmp342.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp33D));{struct Cyc_String_pa_struct _tmp341;_tmp341.tag=0;_tmp341.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp33C));{struct Cyc_String_pa_struct
_tmp340;_tmp340.tag=0;_tmp340.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp2F6->hd));{void*_tmp33E[3]={& _tmp340,& _tmp341,& _tmp342};
Cyc_aprintf(({const char*_tmp33F="(type var %s has different kinds %s and %s)";
_tag_arr(_tmp33F,sizeof(char),_get_zero_arr_size(_tmp33F,44));}),_tag_arr(
_tmp33E,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}inst=({
struct Cyc_List_List*_tmp343=_region_malloc(rgn,sizeof(*_tmp343));_tmp343->hd=({
struct _tuple8*_tmp344=_region_malloc(rgn,sizeof(*_tmp344));_tmp344->f1=(struct
Cyc_Absyn_Tvar*)_tmp2EC->hd;_tmp344->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345[0]=({struct Cyc_Absyn_VarType_struct
_tmp346;_tmp346.tag=1;_tmp346.f1=(struct Cyc_Absyn_Tvar*)_tmp2F6->hd;_tmp346;});
_tmp345;});_tmp344;});_tmp343->tl=inst;_tmp343;});_tmp2F6=_tmp2F6->tl;_tmp2EC=
_tmp2EC->tl;}}if(_tmp2EC != 0){Cyc_Tcutil_failure_reason=({const char*_tmp347="(second function type has too many type variables)";
_tag_arr(_tmp347,sizeof(char),_get_zero_arr_size(_tmp347,51));});_npop_handler(0);
goto _LL212;}if(inst != 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*
_tmp348=_cycalloc(sizeof(*_tmp348));_tmp348[0]=({struct Cyc_Absyn_FnType_struct
_tmp349;_tmp349.tag=8;_tmp349.f1=({struct Cyc_Absyn_FnInfo _tmp34A;_tmp34A.tvars=0;
_tmp34A.effect=_tmp2F7;_tmp34A.ret_typ=(void*)_tmp2F8;_tmp34A.args=_tmp2F9;
_tmp34A.c_varargs=_tmp2FA;_tmp34A.cyc_varargs=_tmp2FB;_tmp34A.rgn_po=_tmp2FC;
_tmp34A.attributes=_tmp2FD;_tmp34A;});_tmp349;});_tmp348;}),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*_tmp34B=_cycalloc(sizeof(*
_tmp34B));_tmp34B[0]=({struct Cyc_Absyn_FnType_struct _tmp34C;_tmp34C.tag=8;
_tmp34C.f1=({struct Cyc_Absyn_FnInfo _tmp34D;_tmp34D.tvars=0;_tmp34D.effect=
_tmp2ED;_tmp34D.ret_typ=(void*)_tmp2EE;_tmp34D.args=_tmp2EF;_tmp34D.c_varargs=
_tmp2F0;_tmp34D.cyc_varargs=_tmp2F1;_tmp34D.rgn_po=_tmp2F2;_tmp34D.attributes=
_tmp2F3;_tmp34D;});_tmp34C;});_tmp34B;})));done=1;}};_pop_region(rgn);}if(done)
return;Cyc_Tcutil_unify_it(_tmp2F8,_tmp2EE);for(0;_tmp2F9 != 0?_tmp2EF != 0: 0;(
_tmp2F9=_tmp2F9->tl,_tmp2EF=_tmp2EF->tl)){struct Cyc_Absyn_Tqual _tmp34F;void*
_tmp350;struct _tuple2 _tmp34E=*((struct _tuple2*)_tmp2F9->hd);_tmp34F=_tmp34E.f2;
_tmp350=_tmp34E.f3;{struct Cyc_Absyn_Tqual _tmp352;void*_tmp353;struct _tuple2
_tmp351=*((struct _tuple2*)_tmp2EF->hd);_tmp352=_tmp351.f2;_tmp353=_tmp351.f3;Cyc_Tcutil_unify_it(
_tmp350,_tmp353);Cyc_Tcutil_unify_tqual(_tmp34F,_tmp350,_tmp352,_tmp353);}}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp2F9 != 0?1: _tmp2EF != 0){Cyc_Tcutil_failure_reason=({
const char*_tmp354="(function types have different number of arguments)";_tag_arr(
_tmp354,sizeof(char),_get_zero_arr_size(_tmp354,52));});goto _LL212;}if(_tmp2FA != 
_tmp2F0){Cyc_Tcutil_failure_reason=({const char*_tmp355="(only one function type takes C varargs)";
_tag_arr(_tmp355,sizeof(char),_get_zero_arr_size(_tmp355,41));});goto _LL212;}{
int bad_cyc_vararg=0;{struct _tuple11 _tmp357=({struct _tuple11 _tmp356;_tmp356.f1=
_tmp2FB;_tmp356.f2=_tmp2F1;_tmp356;});struct Cyc_Absyn_VarargInfo*_tmp358;struct
Cyc_Absyn_VarargInfo*_tmp359;struct Cyc_Absyn_VarargInfo*_tmp35A;struct Cyc_Absyn_VarargInfo*
_tmp35B;struct Cyc_Absyn_VarargInfo*_tmp35C;struct Cyc_Absyn_VarargInfo _tmp35D;
struct Cyc_Core_Opt*_tmp35E;struct Cyc_Absyn_Tqual _tmp35F;void*_tmp360;int _tmp361;
struct Cyc_Absyn_VarargInfo*_tmp362;struct Cyc_Absyn_VarargInfo _tmp363;struct Cyc_Core_Opt*
_tmp364;struct Cyc_Absyn_Tqual _tmp365;void*_tmp366;int _tmp367;_LL251: _tmp358=
_tmp357.f1;if(_tmp358 != 0)goto _LL253;_tmp359=_tmp357.f2;if(_tmp359 != 0)goto
_LL253;_LL252: goto _LL250;_LL253: _tmp35A=_tmp357.f1;if(_tmp35A != 0)goto _LL255;
_LL254: goto _LL256;_LL255: _tmp35B=_tmp357.f2;if(_tmp35B != 0)goto _LL257;_LL256:
bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp368="(only one function type takes varargs)";
_tag_arr(_tmp368,sizeof(char),_get_zero_arr_size(_tmp368,39));});goto _LL250;
_LL257: _tmp35C=_tmp357.f1;if(_tmp35C == 0)goto _LL250;_tmp35D=*_tmp35C;_tmp35E=
_tmp35D.name;_tmp35F=_tmp35D.tq;_tmp360=(void*)_tmp35D.type;_tmp361=_tmp35D.inject;
_tmp362=_tmp357.f2;if(_tmp362 == 0)goto _LL250;_tmp363=*_tmp362;_tmp364=_tmp363.name;
_tmp365=_tmp363.tq;_tmp366=(void*)_tmp363.type;_tmp367=_tmp363.inject;_LL258: Cyc_Tcutil_unify_it(
_tmp360,_tmp366);Cyc_Tcutil_unify_tqual(_tmp35F,_tmp360,_tmp365,_tmp366);if(
_tmp361 != _tmp367){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*
_tmp369="(only one function type injects varargs)";_tag_arr(_tmp369,sizeof(char),
_get_zero_arr_size(_tmp369,41));});}goto _LL250;_LL250:;}if(bad_cyc_vararg)goto
_LL212;{int bad_effect=0;{struct _tuple12 _tmp36B=({struct _tuple12 _tmp36A;_tmp36A.f1=
_tmp2F7;_tmp36A.f2=_tmp2ED;_tmp36A;});struct Cyc_Core_Opt*_tmp36C;struct Cyc_Core_Opt*
_tmp36D;struct Cyc_Core_Opt*_tmp36E;struct Cyc_Core_Opt*_tmp36F;_LL25A: _tmp36C=
_tmp36B.f1;if(_tmp36C != 0)goto _LL25C;_tmp36D=_tmp36B.f2;if(_tmp36D != 0)goto
_LL25C;_LL25B: goto _LL259;_LL25C: _tmp36E=_tmp36B.f1;if(_tmp36E != 0)goto _LL25E;
_LL25D: goto _LL25F;_LL25E: _tmp36F=_tmp36B.f2;if(_tmp36F != 0)goto _LL260;_LL25F:
bad_effect=1;goto _LL259;_LL260:;_LL261: bad_effect=!Cyc_Tcutil_unify_effect((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp2F7))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2ED))->v);goto _LL259;_LL259:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=({const char*_tmp370="(function type effects do not unify)";
_tag_arr(_tmp370,sizeof(char),_get_zero_arr_size(_tmp370,37));});goto _LL212;}if(
!Cyc_Tcutil_same_atts(_tmp2F3,_tmp2FD)){Cyc_Tcutil_failure_reason=({const char*
_tmp371="(function types have different attributes)";_tag_arr(_tmp371,sizeof(
char),_get_zero_arr_size(_tmp371,43));});goto _LL212;}if(!Cyc_Tcutil_same_rgn_po(
_tmp2F2,_tmp2FC)){Cyc_Tcutil_failure_reason=({const char*_tmp372="(function types have different region lifetime orderings)";
_tag_arr(_tmp372,sizeof(char),_get_zero_arr_size(_tmp372,58));});goto _LL212;}
return;}}}_LL235: _tmp2FE=_tmp282.f1;if(_tmp2FE <= (void*)3?1:*((int*)_tmp2FE)!= 9)
goto _LL237;_tmp2FF=((struct Cyc_Absyn_TupleType_struct*)_tmp2FE)->f1;_tmp300=
_tmp282.f2;if(_tmp300 <= (void*)3?1:*((int*)_tmp300)!= 9)goto _LL237;_tmp301=((
struct Cyc_Absyn_TupleType_struct*)_tmp300)->f1;_LL236: for(0;_tmp301 != 0?_tmp2FF
!= 0: 0;(_tmp301=_tmp301->tl,_tmp2FF=_tmp2FF->tl)){struct Cyc_Absyn_Tqual _tmp374;
void*_tmp375;struct _tuple4 _tmp373=*((struct _tuple4*)_tmp301->hd);_tmp374=_tmp373.f1;
_tmp375=_tmp373.f2;{struct Cyc_Absyn_Tqual _tmp377;void*_tmp378;struct _tuple4
_tmp376=*((struct _tuple4*)_tmp2FF->hd);_tmp377=_tmp376.f1;_tmp378=_tmp376.f2;Cyc_Tcutil_unify_it(
_tmp375,_tmp378);Cyc_Tcutil_unify_tqual(_tmp374,_tmp375,_tmp377,_tmp378);}}if(
_tmp301 == 0?_tmp2FF == 0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_failure_reason=({const char*_tmp379="(tuple types have different numbers of components)";
_tag_arr(_tmp379,sizeof(char),_get_zero_arr_size(_tmp379,51));});goto _LL212;
_LL237: _tmp302=_tmp282.f1;if(_tmp302 <= (void*)3?1:*((int*)_tmp302)!= 11)goto
_LL239;_tmp303=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp302)->f1;
_tmp304=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp302)->f2;_tmp305=_tmp282.f2;
if(_tmp305 <= (void*)3?1:*((int*)_tmp305)!= 11)goto _LL239;_tmp306=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp305)->f1;_tmp307=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp305)->f2;_LL238: if(_tmp306 != _tmp303){Cyc_Tcutil_failure_reason=({const char*
_tmp37A="(struct and union type)";_tag_arr(_tmp37A,sizeof(char),
_get_zero_arr_size(_tmp37A,24));});goto _LL212;}for(0;_tmp307 != 0?_tmp304 != 0: 0;(
_tmp307=_tmp307->tl,_tmp304=_tmp304->tl)){struct Cyc_Absyn_Aggrfield*_tmp37B=(
struct Cyc_Absyn_Aggrfield*)_tmp307->hd;struct Cyc_Absyn_Aggrfield*_tmp37C=(struct
Cyc_Absyn_Aggrfield*)_tmp304->hd;if(Cyc_strptrcmp(_tmp37B->name,_tmp37C->name)!= 
0){Cyc_Tcutil_failure_reason=({const char*_tmp37D="(different member names)";
_tag_arr(_tmp37D,sizeof(char),_get_zero_arr_size(_tmp37D,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_it((void*)_tmp37B->type,(void*)_tmp37C->type);
Cyc_Tcutil_unify_tqual(_tmp37B->tq,(void*)_tmp37B->type,_tmp37C->tq,(void*)
_tmp37C->type);if(!Cyc_Tcutil_same_atts(_tmp37B->attributes,_tmp37C->attributes)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp37E="(different attributes on member)";_tag_arr(_tmp37E,sizeof(
char),_get_zero_arr_size(_tmp37E,33));});(int)_throw((void*)Cyc_Tcutil_Unify);}
if(((_tmp37B->width != 0?_tmp37C->width == 0: 0)?1:(_tmp37C->width != 0?_tmp37B->width
== 0: 0))?1:((_tmp37B->width != 0?_tmp37C->width != 0: 0)?!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp37B->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp37C->width)): 0)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_failure_reason=({const char*_tmp37F="(different bitfield widths on member)";
_tag_arr(_tmp37F,sizeof(char),_get_zero_arr_size(_tmp37F,38));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}if(_tmp307 == 0?_tmp304 == 0: 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp380="(different number of members)";
_tag_arr(_tmp380,sizeof(char),_get_zero_arr_size(_tmp380,30));});goto _LL212;
_LL239: _tmp308=_tmp282.f1;if((int)_tmp308 != 2)goto _LL23B;_tmp309=_tmp282.f2;if((
int)_tmp309 != 2)goto _LL23B;_LL23A: return;_LL23B: _tmp30A=_tmp282.f1;if(_tmp30A <= (
void*)3?1:*((int*)_tmp30A)!= 15)goto _LL23D;_tmp30B=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp30A)->f1;_tmp30C=_tmp282.f2;if(_tmp30C <= (void*)3?1:*((int*)_tmp30C)!= 15)
goto _LL23D;_tmp30D=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp30C)->f1;
_LL23C: Cyc_Tcutil_unify_it(_tmp30B,_tmp30D);return;_LL23D: _tmp30E=_tmp282.f1;if(
_tmp30E <= (void*)3?1:*((int*)_tmp30E)!= 20)goto _LL23F;_LL23E: goto _LL240;_LL23F:
_tmp30F=_tmp282.f2;if(_tmp30F <= (void*)3?1:*((int*)_tmp30F)!= 20)goto _LL241;
_LL240: goto _LL242;_LL241: _tmp310=_tmp282.f1;if(_tmp310 <= (void*)3?1:*((int*)
_tmp310)!= 19)goto _LL243;_LL242: goto _LL244;_LL243: _tmp311=_tmp282.f1;if(_tmp311
<= (void*)3?1:*((int*)_tmp311)!= 21)goto _LL245;_LL244: goto _LL246;_LL245: _tmp312=
_tmp282.f2;if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 21)goto _LL247;_LL246: goto
_LL248;_LL247: _tmp313=_tmp282.f2;if(_tmp313 <= (void*)3?1:*((int*)_tmp313)!= 19)
goto _LL249;_LL248: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=({
const char*_tmp381="(effects don't unify)";_tag_arr(_tmp381,sizeof(char),
_get_zero_arr_size(_tmp381,22));});goto _LL212;_LL249:;_LL24A: goto _LL212;_LL212:;}(
int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0?a2 != 0: 0)return - 1;if(a1 != 0?a2 == 0:
0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int _tmp382=(tq1.q_const + (tq1.q_volatile
<< 1))+ (tq1.q_restrict << 2);int _tmp383=(tq2.q_const + (tq2.q_volatile << 1))+ (
tq2.q_restrict << 2);return Cyc_Core_intcmp(_tmp382,_tmp383);}static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp384=(void*)x->v;
void*_tmp385;_LL263: if((int)_tmp384 != 0)goto _LL265;_LL264: return - 1;_LL265: if(
_tmp384 <= (void*)1?1:*((int*)_tmp384)!= 0)goto _LL267;_tmp385=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp384)->f1;_LL266: {void*_tmp386=(void*)y->v;void*_tmp387;_LL26A: if((int)
_tmp386 != 0)goto _LL26C;_LL26B: return 1;_LL26C: if(_tmp386 <= (void*)1?1:*((int*)
_tmp386)!= 0)goto _LL26E;_tmp387=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp386)->f1;_LL26D: return cmp(_tmp385,_tmp387);_LL26E: if(_tmp386 <= (void*)1?1:*((
int*)_tmp386)!= 1)goto _LL269;_LL26F:({void*_tmp388[0]={};Cyc_Tcutil_impos(({
const char*_tmp389="unify_conref: forward after compress(2)";_tag_arr(_tmp389,
sizeof(char),_get_zero_arr_size(_tmp389,40));}),_tag_arr(_tmp388,sizeof(void*),0));});
_LL269:;}_LL267: if(_tmp384 <= (void*)1?1:*((int*)_tmp384)!= 1)goto _LL262;_LL268:({
void*_tmp38A[0]={};Cyc_Tcutil_impos(({const char*_tmp38B="unify_conref: forward after compress";
_tag_arr(_tmp38B,sizeof(char),_get_zero_arr_size(_tmp38B,37));}),_tag_arr(
_tmp38A,sizeof(void*),0));});_LL262:;}}static int Cyc_Tcutil_tqual_type_cmp(struct
_tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp38D;struct Cyc_Absyn_Tqual
_tmp38E;void*_tmp38F;struct _tuple4*_tmp38C=tqt1;_tmp38D=*_tmp38C;_tmp38E=_tmp38D.f1;
_tmp38F=_tmp38D.f2;{struct _tuple4 _tmp391;struct Cyc_Absyn_Tqual _tmp392;void*
_tmp393;struct _tuple4*_tmp390=tqt2;_tmp391=*_tmp390;_tmp392=_tmp391.f1;_tmp393=
_tmp391.f2;{int _tmp394=Cyc_Tcutil_tqual_cmp(_tmp38E,_tmp392);if(_tmp394 != 0)
return _tmp394;return Cyc_Tcutil_typecmp(_tmp38F,_tmp393);}}}static int Cyc_Tcutil_aggrfield_cmp(
struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp395=Cyc_strptrcmp(
f1->name,f2->name);if(_tmp395 != 0)return _tmp395;{int _tmp396=Cyc_Tcutil_tqual_cmp(
f1->tq,f2->tq);if(_tmp396 != 0)return _tmp396;{int _tmp397=Cyc_Tcutil_typecmp((void*)
f1->type,(void*)f2->type);if(_tmp397 != 0)return _tmp397;{int _tmp398=Cyc_List_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp398 != 0)return
_tmp398;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp399=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp399 != 0)return _tmp399;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp39A=t;_LL271: if((int)_tmp39A != 0)goto _LL273;_LL272: return 0;
_LL273: if(_tmp39A <= (void*)3?1:*((int*)_tmp39A)!= 0)goto _LL275;_LL274: return 1;
_LL275: if(_tmp39A <= (void*)3?1:*((int*)_tmp39A)!= 1)goto _LL277;_LL276: return 2;
_LL277: if(_tmp39A <= (void*)3?1:*((int*)_tmp39A)!= 2)goto _LL279;_LL278: return 3;
_LL279: if(_tmp39A <= (void*)3?1:*((int*)_tmp39A)!= 3)goto _LL27B;_LL27A: return 4;
_LL27B: if(_tmp39A <= (void*)3?1:*((int*)_tmp39A)!= 4)goto _LL27D;_LL27C: return 5;
_LL27D: if(_tmp39A <= (void*)3?1:*((int*)_tmp39A)!= 5)goto _LL27F;_LL27E: return 6;
_LL27F: if((int)_tmp39A != 1)goto _LL281;_LL280: return 7;_LL281: if(_tmp39A <= (void*)
3?1:*((int*)_tmp39A)!= 6)goto _LL283;_LL282: return 8;_LL283: if(_tmp39A <= (void*)3?
1:*((int*)_tmp39A)!= 7)goto _LL285;_LL284: return 9;_LL285: if(_tmp39A <= (void*)3?1:*((
int*)_tmp39A)!= 8)goto _LL287;_LL286: return 10;_LL287: if(_tmp39A <= (void*)3?1:*((
int*)_tmp39A)!= 9)goto _LL289;_LL288: return 11;_LL289: if(_tmp39A <= (void*)3?1:*((
int*)_tmp39A)!= 10)goto _LL28B;_LL28A: return 12;_LL28B: if(_tmp39A <= (void*)3?1:*((
int*)_tmp39A)!= 11)goto _LL28D;_LL28C: return 14;_LL28D: if(_tmp39A <= (void*)3?1:*((
int*)_tmp39A)!= 12)goto _LL28F;_LL28E: return 16;_LL28F: if(_tmp39A <= (void*)3?1:*((
int*)_tmp39A)!= 13)goto _LL291;_LL290: return 17;_LL291: if(_tmp39A <= (void*)3?1:*((
int*)_tmp39A)!= 15)goto _LL293;_LL292: return 18;_LL293: if(_tmp39A <= (void*)3?1:*((
int*)_tmp39A)!= 16)goto _LL295;_LL294: return 19;_LL295: if((int)_tmp39A != 2)goto
_LL297;_LL296: return 20;_LL297: if(_tmp39A <= (void*)3?1:*((int*)_tmp39A)!= 19)goto
_LL299;_LL298: return 21;_LL299: if(_tmp39A <= (void*)3?1:*((int*)_tmp39A)!= 20)goto
_LL29B;_LL29A: return 22;_LL29B: if(_tmp39A <= (void*)3?1:*((int*)_tmp39A)!= 21)goto
_LL29D;_LL29C: return 23;_LL29D: if(_tmp39A <= (void*)3?1:*((int*)_tmp39A)!= 14)goto
_LL29F;_LL29E: return 24;_LL29F: if(_tmp39A <= (void*)3?1:*((int*)_tmp39A)!= 18)goto
_LL2A1;_LL2A0: return 25;_LL2A1: if(_tmp39A <= (void*)3?1:*((int*)_tmp39A)!= 17)goto
_LL270;_LL2A2: return 26;_LL270:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp39B=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp39B != 0)
return _tmp39B;{struct _tuple6 _tmp39D=({struct _tuple6 _tmp39C;_tmp39C.f1=t2;_tmp39C.f2=
t1;_tmp39C;});void*_tmp39E;void*_tmp39F;void*_tmp3A0;struct Cyc_Absyn_Tvar*
_tmp3A1;void*_tmp3A2;struct Cyc_Absyn_Tvar*_tmp3A3;void*_tmp3A4;struct Cyc_Absyn_AggrInfo
_tmp3A5;void*_tmp3A6;struct Cyc_List_List*_tmp3A7;void*_tmp3A8;struct Cyc_Absyn_AggrInfo
_tmp3A9;void*_tmp3AA;struct Cyc_List_List*_tmp3AB;void*_tmp3AC;struct _tuple1*
_tmp3AD;void*_tmp3AE;struct _tuple1*_tmp3AF;void*_tmp3B0;struct Cyc_List_List*
_tmp3B1;void*_tmp3B2;struct Cyc_List_List*_tmp3B3;void*_tmp3B4;struct Cyc_Absyn_TunionInfo
_tmp3B5;void*_tmp3B6;struct Cyc_Absyn_Tuniondecl**_tmp3B7;struct Cyc_Absyn_Tuniondecl*
_tmp3B8;struct Cyc_List_List*_tmp3B9;void*_tmp3BA;void*_tmp3BB;struct Cyc_Absyn_TunionInfo
_tmp3BC;void*_tmp3BD;struct Cyc_Absyn_Tuniondecl**_tmp3BE;struct Cyc_Absyn_Tuniondecl*
_tmp3BF;struct Cyc_List_List*_tmp3C0;void*_tmp3C1;void*_tmp3C2;struct Cyc_Absyn_TunionFieldInfo
_tmp3C3;void*_tmp3C4;struct Cyc_Absyn_Tuniondecl*_tmp3C5;struct Cyc_Absyn_Tunionfield*
_tmp3C6;struct Cyc_List_List*_tmp3C7;void*_tmp3C8;struct Cyc_Absyn_TunionFieldInfo
_tmp3C9;void*_tmp3CA;struct Cyc_Absyn_Tuniondecl*_tmp3CB;struct Cyc_Absyn_Tunionfield*
_tmp3CC;struct Cyc_List_List*_tmp3CD;void*_tmp3CE;struct Cyc_Absyn_PtrInfo _tmp3CF;
void*_tmp3D0;struct Cyc_Absyn_Tqual _tmp3D1;struct Cyc_Absyn_PtrAtts _tmp3D2;void*
_tmp3D3;struct Cyc_Absyn_Conref*_tmp3D4;struct Cyc_Absyn_Conref*_tmp3D5;struct Cyc_Absyn_Conref*
_tmp3D6;void*_tmp3D7;struct Cyc_Absyn_PtrInfo _tmp3D8;void*_tmp3D9;struct Cyc_Absyn_Tqual
_tmp3DA;struct Cyc_Absyn_PtrAtts _tmp3DB;void*_tmp3DC;struct Cyc_Absyn_Conref*
_tmp3DD;struct Cyc_Absyn_Conref*_tmp3DE;struct Cyc_Absyn_Conref*_tmp3DF;void*
_tmp3E0;void*_tmp3E1;void*_tmp3E2;void*_tmp3E3;void*_tmp3E4;void*_tmp3E5;void*
_tmp3E6;void*_tmp3E7;void*_tmp3E8;int _tmp3E9;void*_tmp3EA;int _tmp3EB;void*
_tmp3EC;struct Cyc_Absyn_ArrayInfo _tmp3ED;void*_tmp3EE;struct Cyc_Absyn_Tqual
_tmp3EF;struct Cyc_Absyn_Exp*_tmp3F0;struct Cyc_Absyn_Conref*_tmp3F1;void*_tmp3F2;
struct Cyc_Absyn_ArrayInfo _tmp3F3;void*_tmp3F4;struct Cyc_Absyn_Tqual _tmp3F5;
struct Cyc_Absyn_Exp*_tmp3F6;struct Cyc_Absyn_Conref*_tmp3F7;void*_tmp3F8;struct
Cyc_Absyn_FnInfo _tmp3F9;struct Cyc_List_List*_tmp3FA;struct Cyc_Core_Opt*_tmp3FB;
void*_tmp3FC;struct Cyc_List_List*_tmp3FD;int _tmp3FE;struct Cyc_Absyn_VarargInfo*
_tmp3FF;struct Cyc_List_List*_tmp400;struct Cyc_List_List*_tmp401;void*_tmp402;
struct Cyc_Absyn_FnInfo _tmp403;struct Cyc_List_List*_tmp404;struct Cyc_Core_Opt*
_tmp405;void*_tmp406;struct Cyc_List_List*_tmp407;int _tmp408;struct Cyc_Absyn_VarargInfo*
_tmp409;struct Cyc_List_List*_tmp40A;struct Cyc_List_List*_tmp40B;void*_tmp40C;
struct Cyc_List_List*_tmp40D;void*_tmp40E;struct Cyc_List_List*_tmp40F;void*
_tmp410;void*_tmp411;struct Cyc_List_List*_tmp412;void*_tmp413;void*_tmp414;
struct Cyc_List_List*_tmp415;void*_tmp416;void*_tmp417;void*_tmp418;void*_tmp419;
void*_tmp41A;void*_tmp41B;void*_tmp41C;void*_tmp41D;void*_tmp41E;void*_tmp41F;
void*_tmp420;void*_tmp421;void*_tmp422;int _tmp423;void*_tmp424;int _tmp425;void*
_tmp426;void*_tmp427;void*_tmp428;void*_tmp429;void*_tmp42A;void*_tmp42B;_LL2A4:
_tmp39E=_tmp39D.f1;if(_tmp39E <= (void*)3?1:*((int*)_tmp39E)!= 0)goto _LL2A6;
_tmp39F=_tmp39D.f2;if(_tmp39F <= (void*)3?1:*((int*)_tmp39F)!= 0)goto _LL2A6;
_LL2A5:({void*_tmp42C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp42D="typecmp: can only compare closed types";
_tag_arr(_tmp42D,sizeof(char),_get_zero_arr_size(_tmp42D,39));}),_tag_arr(
_tmp42C,sizeof(void*),0));});_LL2A6: _tmp3A0=_tmp39D.f1;if(_tmp3A0 <= (void*)3?1:*((
int*)_tmp3A0)!= 1)goto _LL2A8;_tmp3A1=((struct Cyc_Absyn_VarType_struct*)_tmp3A0)->f1;
_tmp3A2=_tmp39D.f2;if(_tmp3A2 <= (void*)3?1:*((int*)_tmp3A2)!= 1)goto _LL2A8;
_tmp3A3=((struct Cyc_Absyn_VarType_struct*)_tmp3A2)->f1;_LL2A7: return Cyc_Core_intcmp(*((
int*)_check_null(_tmp3A3->identity)),*((int*)_check_null(_tmp3A1->identity)));
_LL2A8: _tmp3A4=_tmp39D.f1;if(_tmp3A4 <= (void*)3?1:*((int*)_tmp3A4)!= 10)goto
_LL2AA;_tmp3A5=((struct Cyc_Absyn_AggrType_struct*)_tmp3A4)->f1;_tmp3A6=(void*)
_tmp3A5.aggr_info;_tmp3A7=_tmp3A5.targs;_tmp3A8=_tmp39D.f2;if(_tmp3A8 <= (void*)3?
1:*((int*)_tmp3A8)!= 10)goto _LL2AA;_tmp3A9=((struct Cyc_Absyn_AggrType_struct*)
_tmp3A8)->f1;_tmp3AA=(void*)_tmp3A9.aggr_info;_tmp3AB=_tmp3A9.targs;_LL2A9: {
struct _tuple1*_tmp42F;struct _tuple5 _tmp42E=Cyc_Absyn_aggr_kinded_name(_tmp3A6);
_tmp42F=_tmp42E.f2;{struct _tuple1*_tmp431;struct _tuple5 _tmp430=Cyc_Absyn_aggr_kinded_name(
_tmp3AA);_tmp431=_tmp430.f2;{int _tmp432=Cyc_Absyn_qvar_cmp(_tmp42F,_tmp431);if(
_tmp432 != 0)return _tmp432;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3A7,_tmp3AB);}}}}_LL2AA: _tmp3AC=_tmp39D.f1;if(_tmp3AC <= (void*)3?1:*((int*)
_tmp3AC)!= 12)goto _LL2AC;_tmp3AD=((struct Cyc_Absyn_EnumType_struct*)_tmp3AC)->f1;
_tmp3AE=_tmp39D.f2;if(_tmp3AE <= (void*)3?1:*((int*)_tmp3AE)!= 12)goto _LL2AC;
_tmp3AF=((struct Cyc_Absyn_EnumType_struct*)_tmp3AE)->f1;_LL2AB: return Cyc_Absyn_qvar_cmp(
_tmp3AD,_tmp3AF);_LL2AC: _tmp3B0=_tmp39D.f1;if(_tmp3B0 <= (void*)3?1:*((int*)
_tmp3B0)!= 13)goto _LL2AE;_tmp3B1=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp3B0)->f1;
_tmp3B2=_tmp39D.f2;if(_tmp3B2 <= (void*)3?1:*((int*)_tmp3B2)!= 13)goto _LL2AE;
_tmp3B3=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp3B2)->f1;_LL2AD: return((int(*)(
int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp3B1,
_tmp3B3);_LL2AE: _tmp3B4=_tmp39D.f1;if(_tmp3B4 <= (void*)3?1:*((int*)_tmp3B4)!= 2)
goto _LL2B0;_tmp3B5=((struct Cyc_Absyn_TunionType_struct*)_tmp3B4)->f1;_tmp3B6=(
void*)_tmp3B5.tunion_info;if(*((int*)_tmp3B6)!= 1)goto _LL2B0;_tmp3B7=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3B6)->f1;_tmp3B8=*_tmp3B7;_tmp3B9=_tmp3B5.targs;_tmp3BA=(void*)_tmp3B5.rgn;
_tmp3BB=_tmp39D.f2;if(_tmp3BB <= (void*)3?1:*((int*)_tmp3BB)!= 2)goto _LL2B0;
_tmp3BC=((struct Cyc_Absyn_TunionType_struct*)_tmp3BB)->f1;_tmp3BD=(void*)_tmp3BC.tunion_info;
if(*((int*)_tmp3BD)!= 1)goto _LL2B0;_tmp3BE=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3BD)->f1;_tmp3BF=*_tmp3BE;_tmp3C0=_tmp3BC.targs;_tmp3C1=(void*)_tmp3BC.rgn;
_LL2AF: if(_tmp3BF == _tmp3B8)return 0;{int _tmp433=Cyc_Absyn_qvar_cmp(_tmp3BF->name,
_tmp3B8->name);if(_tmp433 != 0)return _tmp433;{int _tmp434=Cyc_Tcutil_typecmp(
_tmp3C1,_tmp3BA);if(_tmp434 != 0)return _tmp434;return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3C0,_tmp3B9);}}_LL2B0: _tmp3C2=_tmp39D.f1;if(_tmp3C2 <= (void*)3?1:*((int*)
_tmp3C2)!= 3)goto _LL2B2;_tmp3C3=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3C2)->f1;_tmp3C4=(void*)_tmp3C3.field_info;if(*((int*)_tmp3C4)!= 1)goto
_LL2B2;_tmp3C5=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3C4)->f1;_tmp3C6=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3C4)->f2;_tmp3C7=_tmp3C3.targs;
_tmp3C8=_tmp39D.f2;if(_tmp3C8 <= (void*)3?1:*((int*)_tmp3C8)!= 3)goto _LL2B2;
_tmp3C9=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3C8)->f1;_tmp3CA=(void*)
_tmp3C9.field_info;if(*((int*)_tmp3CA)!= 1)goto _LL2B2;_tmp3CB=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp3CA)->f1;_tmp3CC=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3CA)->f2;
_tmp3CD=_tmp3C9.targs;_LL2B1: if(_tmp3CB == _tmp3C5)return 0;{int _tmp435=Cyc_Absyn_qvar_cmp(
_tmp3C5->name,_tmp3CB->name);if(_tmp435 != 0)return _tmp435;{int _tmp436=Cyc_Absyn_qvar_cmp(
_tmp3C6->name,_tmp3CC->name);if(_tmp436 != 0)return _tmp436;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3CD,_tmp3C7);}}_LL2B2: _tmp3CE=_tmp39D.f1;if(_tmp3CE <= (
void*)3?1:*((int*)_tmp3CE)!= 4)goto _LL2B4;_tmp3CF=((struct Cyc_Absyn_PointerType_struct*)
_tmp3CE)->f1;_tmp3D0=(void*)_tmp3CF.elt_typ;_tmp3D1=_tmp3CF.elt_tq;_tmp3D2=
_tmp3CF.ptr_atts;_tmp3D3=(void*)_tmp3D2.rgn;_tmp3D4=_tmp3D2.nullable;_tmp3D5=
_tmp3D2.bounds;_tmp3D6=_tmp3D2.zero_term;_tmp3D7=_tmp39D.f2;if(_tmp3D7 <= (void*)
3?1:*((int*)_tmp3D7)!= 4)goto _LL2B4;_tmp3D8=((struct Cyc_Absyn_PointerType_struct*)
_tmp3D7)->f1;_tmp3D9=(void*)_tmp3D8.elt_typ;_tmp3DA=_tmp3D8.elt_tq;_tmp3DB=
_tmp3D8.ptr_atts;_tmp3DC=(void*)_tmp3DB.rgn;_tmp3DD=_tmp3DB.nullable;_tmp3DE=
_tmp3DB.bounds;_tmp3DF=_tmp3DB.zero_term;_LL2B3: {int _tmp437=Cyc_Tcutil_typecmp(
_tmp3D9,_tmp3D0);if(_tmp437 != 0)return _tmp437;{int _tmp438=Cyc_Tcutil_typecmp(
_tmp3DC,_tmp3D3);if(_tmp438 != 0)return _tmp438;{int _tmp439=Cyc_Tcutil_tqual_cmp(
_tmp3DA,_tmp3D1);if(_tmp439 != 0)return _tmp439;{int _tmp43A=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3DE,_tmp3D5);if(_tmp43A != 0)return _tmp43A;{int _tmp43B=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3DF,_tmp3D6);if(_tmp43B != 0)return _tmp43B;{void*_tmp43C=(
void*)(Cyc_Absyn_compress_conref(_tmp3DE))->v;void*_tmp43D;_LL2D9: if(_tmp43C <= (
void*)1?1:*((int*)_tmp43C)!= 0)goto _LL2DB;_tmp43D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp43C)->f1;if((int)_tmp43D != 0)goto _LL2DB;_LL2DA: return 0;_LL2DB:;_LL2DC: goto
_LL2D8;_LL2D8:;}return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3DD,
_tmp3D4);}}}}}_LL2B4: _tmp3E0=_tmp39D.f1;if(_tmp3E0 <= (void*)3?1:*((int*)_tmp3E0)
!= 5)goto _LL2B6;_tmp3E1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E0)->f1;
_tmp3E2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E0)->f2;_tmp3E3=_tmp39D.f2;
if(_tmp3E3 <= (void*)3?1:*((int*)_tmp3E3)!= 5)goto _LL2B6;_tmp3E4=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp3E3)->f1;_tmp3E5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3E3)->f2;_LL2B5: if(_tmp3E4 != _tmp3E1)return Cyc_Core_intcmp((int)((
unsigned int)_tmp3E4),(int)((unsigned int)_tmp3E1));if(_tmp3E5 != _tmp3E2)return
Cyc_Core_intcmp((int)((unsigned int)_tmp3E5),(int)((unsigned int)_tmp3E2));
return 0;_LL2B6: _tmp3E6=_tmp39D.f1;if((int)_tmp3E6 != 1)goto _LL2B8;_tmp3E7=_tmp39D.f2;
if((int)_tmp3E7 != 1)goto _LL2B8;_LL2B7: return 0;_LL2B8: _tmp3E8=_tmp39D.f1;if(
_tmp3E8 <= (void*)3?1:*((int*)_tmp3E8)!= 6)goto _LL2BA;_tmp3E9=((struct Cyc_Absyn_DoubleType_struct*)
_tmp3E8)->f1;_tmp3EA=_tmp39D.f2;if(_tmp3EA <= (void*)3?1:*((int*)_tmp3EA)!= 6)
goto _LL2BA;_tmp3EB=((struct Cyc_Absyn_DoubleType_struct*)_tmp3EA)->f1;_LL2B9: if(
_tmp3E9 == _tmp3EB)return 0;else{if(_tmp3E9)return - 1;else{return 1;}}_LL2BA: _tmp3EC=
_tmp39D.f1;if(_tmp3EC <= (void*)3?1:*((int*)_tmp3EC)!= 7)goto _LL2BC;_tmp3ED=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3EC)->f1;_tmp3EE=(void*)_tmp3ED.elt_type;
_tmp3EF=_tmp3ED.tq;_tmp3F0=_tmp3ED.num_elts;_tmp3F1=_tmp3ED.zero_term;_tmp3F2=
_tmp39D.f2;if(_tmp3F2 <= (void*)3?1:*((int*)_tmp3F2)!= 7)goto _LL2BC;_tmp3F3=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F2)->f1;_tmp3F4=(void*)_tmp3F3.elt_type;
_tmp3F5=_tmp3F3.tq;_tmp3F6=_tmp3F3.num_elts;_tmp3F7=_tmp3F3.zero_term;_LL2BB: {
int _tmp43E=Cyc_Tcutil_tqual_cmp(_tmp3F5,_tmp3EF);if(_tmp43E != 0)return _tmp43E;{
int _tmp43F=Cyc_Tcutil_typecmp(_tmp3F4,_tmp3EE);if(_tmp43F != 0)return _tmp43F;{int
_tmp440=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3F1,_tmp3F7);if(_tmp440 != 0)
return _tmp440;if(_tmp3F0 == _tmp3F6)return 0;if(_tmp3F0 == 0?1: _tmp3F6 == 0)({void*
_tmp441[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp442="missing expression in array index";_tag_arr(_tmp442,sizeof(
char),_get_zero_arr_size(_tmp442,34));}),_tag_arr(_tmp441,sizeof(void*),0));});
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*
a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3F0,
_tmp3F6);}}}_LL2BC: _tmp3F8=_tmp39D.f1;if(_tmp3F8 <= (void*)3?1:*((int*)_tmp3F8)!= 
8)goto _LL2BE;_tmp3F9=((struct Cyc_Absyn_FnType_struct*)_tmp3F8)->f1;_tmp3FA=
_tmp3F9.tvars;_tmp3FB=_tmp3F9.effect;_tmp3FC=(void*)_tmp3F9.ret_typ;_tmp3FD=
_tmp3F9.args;_tmp3FE=_tmp3F9.c_varargs;_tmp3FF=_tmp3F9.cyc_varargs;_tmp400=
_tmp3F9.rgn_po;_tmp401=_tmp3F9.attributes;_tmp402=_tmp39D.f2;if(_tmp402 <= (void*)
3?1:*((int*)_tmp402)!= 8)goto _LL2BE;_tmp403=((struct Cyc_Absyn_FnType_struct*)
_tmp402)->f1;_tmp404=_tmp403.tvars;_tmp405=_tmp403.effect;_tmp406=(void*)_tmp403.ret_typ;
_tmp407=_tmp403.args;_tmp408=_tmp403.c_varargs;_tmp409=_tmp403.cyc_varargs;
_tmp40A=_tmp403.rgn_po;_tmp40B=_tmp403.attributes;_LL2BD:({void*_tmp443[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp444="typecmp: function types not handled";_tag_arr(_tmp444,sizeof(char),
_get_zero_arr_size(_tmp444,36));}),_tag_arr(_tmp443,sizeof(void*),0));});_LL2BE:
_tmp40C=_tmp39D.f1;if(_tmp40C <= (void*)3?1:*((int*)_tmp40C)!= 9)goto _LL2C0;
_tmp40D=((struct Cyc_Absyn_TupleType_struct*)_tmp40C)->f1;_tmp40E=_tmp39D.f2;if(
_tmp40E <= (void*)3?1:*((int*)_tmp40E)!= 9)goto _LL2C0;_tmp40F=((struct Cyc_Absyn_TupleType_struct*)
_tmp40E)->f1;_LL2BF: return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,
_tmp40F,_tmp40D);_LL2C0: _tmp410=_tmp39D.f1;if(_tmp410 <= (void*)3?1:*((int*)
_tmp410)!= 11)goto _LL2C2;_tmp411=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp410)->f1;_tmp412=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp410)->f2;_tmp413=
_tmp39D.f2;if(_tmp413 <= (void*)3?1:*((int*)_tmp413)!= 11)goto _LL2C2;_tmp414=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp413)->f1;_tmp415=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp413)->f2;_LL2C1: if(_tmp414 != _tmp411){if(_tmp414 == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp415,_tmp412);_LL2C2: _tmp416=_tmp39D.f1;if(_tmp416 <= (void*)3?1:*((int*)
_tmp416)!= 15)goto _LL2C4;_tmp417=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp416)->f1;_tmp418=_tmp39D.f2;if(_tmp418 <= (void*)3?1:*((int*)_tmp418)!= 15)
goto _LL2C4;_tmp419=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp418)->f1;
_LL2C3: return Cyc_Tcutil_typecmp(_tmp417,_tmp419);_LL2C4: _tmp41A=_tmp39D.f1;if(
_tmp41A <= (void*)3?1:*((int*)_tmp41A)!= 14)goto _LL2C6;_tmp41B=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp41A)->f1;_tmp41C=_tmp39D.f2;if(_tmp41C <= (void*)3?1:*((int*)_tmp41C)!= 14)
goto _LL2C6;_tmp41D=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp41C)->f1;
_LL2C5: return Cyc_Tcutil_typecmp(_tmp41B,_tmp41D);_LL2C6: _tmp41E=_tmp39D.f1;if(
_tmp41E <= (void*)3?1:*((int*)_tmp41E)!= 17)goto _LL2C8;_tmp41F=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp41E)->f1;_tmp420=_tmp39D.f2;if(_tmp420 <= (void*)3?1:*((int*)_tmp420)!= 17)
goto _LL2C8;_tmp421=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp420)->f1;_LL2C7:
return Cyc_Tcutil_typecmp(_tmp41F,_tmp421);_LL2C8: _tmp422=_tmp39D.f1;if(_tmp422 <= (
void*)3?1:*((int*)_tmp422)!= 18)goto _LL2CA;_tmp423=((struct Cyc_Absyn_TypeInt_struct*)
_tmp422)->f1;_tmp424=_tmp39D.f2;if(_tmp424 <= (void*)3?1:*((int*)_tmp424)!= 18)
goto _LL2CA;_tmp425=((struct Cyc_Absyn_TypeInt_struct*)_tmp424)->f1;_LL2C9: return
Cyc_Core_intcmp(_tmp423,_tmp425);_LL2CA: _tmp426=_tmp39D.f1;if(_tmp426 <= (void*)3?
1:*((int*)_tmp426)!= 20)goto _LL2CC;_LL2CB: goto _LL2CD;_LL2CC: _tmp427=_tmp39D.f2;
if(_tmp427 <= (void*)3?1:*((int*)_tmp427)!= 20)goto _LL2CE;_LL2CD: goto _LL2CF;
_LL2CE: _tmp428=_tmp39D.f1;if(_tmp428 <= (void*)3?1:*((int*)_tmp428)!= 19)goto
_LL2D0;_LL2CF: goto _LL2D1;_LL2D0: _tmp429=_tmp39D.f1;if(_tmp429 <= (void*)3?1:*((
int*)_tmp429)!= 21)goto _LL2D2;_LL2D1: goto _LL2D3;_LL2D2: _tmp42A=_tmp39D.f2;if(
_tmp42A <= (void*)3?1:*((int*)_tmp42A)!= 21)goto _LL2D4;_LL2D3: goto _LL2D5;_LL2D4:
_tmp42B=_tmp39D.f2;if(_tmp42B <= (void*)3?1:*((int*)_tmp42B)!= 19)goto _LL2D6;
_LL2D5:({void*_tmp445[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp446="typecmp: effects not handled";_tag_arr(
_tmp446,sizeof(char),_get_zero_arr_size(_tmp446,29));}),_tag_arr(_tmp445,sizeof(
void*),0));});_LL2D6:;_LL2D7:({void*_tmp447[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp448="Unmatched case in typecmp";
_tag_arr(_tmp448,sizeof(char),_get_zero_arr_size(_tmp448,26));}),_tag_arr(
_tmp447,sizeof(void*),0));});_LL2A3:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp449=Cyc_Tcutil_compress(t);_LL2DE: if(_tmp449 <= (void*)3?1:*((int*)
_tmp449)!= 5)goto _LL2E0;_LL2DF: goto _LL2E1;_LL2E0: if((int)_tmp449 != 1)goto _LL2E2;
_LL2E1: goto _LL2E3;_LL2E2: if(_tmp449 <= (void*)3?1:*((int*)_tmp449)!= 6)goto _LL2E4;
_LL2E3: goto _LL2E5;_LL2E4: if(_tmp449 <= (void*)3?1:*((int*)_tmp449)!= 12)goto
_LL2E6;_LL2E5: goto _LL2E7;_LL2E6: if(_tmp449 <= (void*)3?1:*((int*)_tmp449)!= 13)
goto _LL2E8;_LL2E7: return 1;_LL2E8:;_LL2E9: return 0;_LL2DD:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){struct _tuple6 _tmp44B=({struct _tuple6 _tmp44A;_tmp44A.f1=Cyc_Tcutil_compress(
t1);_tmp44A.f2=Cyc_Tcutil_compress(t2);_tmp44A;});void*_tmp44C;int _tmp44D;void*
_tmp44E;int _tmp44F;void*_tmp450;void*_tmp451;void*_tmp452;void*_tmp453;void*
_tmp454;void*_tmp455;void*_tmp456;void*_tmp457;void*_tmp458;void*_tmp459;void*
_tmp45A;void*_tmp45B;void*_tmp45C;void*_tmp45D;void*_tmp45E;void*_tmp45F;void*
_tmp460;void*_tmp461;void*_tmp462;void*_tmp463;void*_tmp464;void*_tmp465;void*
_tmp466;void*_tmp467;void*_tmp468;void*_tmp469;void*_tmp46A;void*_tmp46B;void*
_tmp46C;void*_tmp46D;void*_tmp46E;void*_tmp46F;void*_tmp470;void*_tmp471;void*
_tmp472;void*_tmp473;void*_tmp474;void*_tmp475;void*_tmp476;void*_tmp477;void*
_tmp478;void*_tmp479;void*_tmp47A;void*_tmp47B;void*_tmp47C;void*_tmp47D;void*
_tmp47E;_LL2EB: _tmp44C=_tmp44B.f1;if(_tmp44C <= (void*)3?1:*((int*)_tmp44C)!= 6)
goto _LL2ED;_tmp44D=((struct Cyc_Absyn_DoubleType_struct*)_tmp44C)->f1;_tmp44E=
_tmp44B.f2;if(_tmp44E <= (void*)3?1:*((int*)_tmp44E)!= 6)goto _LL2ED;_tmp44F=((
struct Cyc_Absyn_DoubleType_struct*)_tmp44E)->f1;_LL2EC: return !_tmp44F?_tmp44D: 0;
_LL2ED: _tmp450=_tmp44B.f1;if(_tmp450 <= (void*)3?1:*((int*)_tmp450)!= 6)goto
_LL2EF;_tmp451=_tmp44B.f2;if((int)_tmp451 != 1)goto _LL2EF;_LL2EE: goto _LL2F0;
_LL2EF: _tmp452=_tmp44B.f1;if(_tmp452 <= (void*)3?1:*((int*)_tmp452)!= 6)goto
_LL2F1;_tmp453=_tmp44B.f2;if(_tmp453 <= (void*)3?1:*((int*)_tmp453)!= 5)goto
_LL2F1;_LL2F0: goto _LL2F2;_LL2F1: _tmp454=_tmp44B.f1;if(_tmp454 <= (void*)3?1:*((
int*)_tmp454)!= 6)goto _LL2F3;_tmp455=_tmp44B.f2;if(_tmp455 <= (void*)3?1:*((int*)
_tmp455)!= 14)goto _LL2F3;_LL2F2: goto _LL2F4;_LL2F3: _tmp456=_tmp44B.f1;if((int)
_tmp456 != 1)goto _LL2F5;_tmp457=_tmp44B.f2;if(_tmp457 <= (void*)3?1:*((int*)
_tmp457)!= 14)goto _LL2F5;_LL2F4: goto _LL2F6;_LL2F5: _tmp458=_tmp44B.f1;if(_tmp458
<= (void*)3?1:*((int*)_tmp458)!= 6)goto _LL2F7;_tmp459=_tmp44B.f2;if(_tmp459 <= (
void*)3?1:*((int*)_tmp459)!= 17)goto _LL2F7;_LL2F6: goto _LL2F8;_LL2F7: _tmp45A=
_tmp44B.f1;if((int)_tmp45A != 1)goto _LL2F9;_tmp45B=_tmp44B.f2;if(_tmp45B <= (void*)
3?1:*((int*)_tmp45B)!= 17)goto _LL2F9;_LL2F8: goto _LL2FA;_LL2F9: _tmp45C=_tmp44B.f1;
if((int)_tmp45C != 1)goto _LL2FB;_tmp45D=_tmp44B.f2;if(_tmp45D <= (void*)3?1:*((int*)
_tmp45D)!= 5)goto _LL2FB;_LL2FA: return 1;_LL2FB: _tmp45E=_tmp44B.f1;if(_tmp45E <= (
void*)3?1:*((int*)_tmp45E)!= 5)goto _LL2FD;_tmp45F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp45E)->f2;if((int)_tmp45F != 3)goto _LL2FD;_tmp460=_tmp44B.f2;if(_tmp460 <= (
void*)3?1:*((int*)_tmp460)!= 5)goto _LL2FD;_tmp461=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp460)->f2;if((int)_tmp461 != 3)goto _LL2FD;_LL2FC: return 0;_LL2FD: _tmp462=
_tmp44B.f1;if(_tmp462 <= (void*)3?1:*((int*)_tmp462)!= 5)goto _LL2FF;_tmp463=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp462)->f2;if((int)_tmp463 != 3)goto _LL2FF;
_LL2FE: goto _LL300;_LL2FF: _tmp464=_tmp44B.f1;if(_tmp464 <= (void*)3?1:*((int*)
_tmp464)!= 5)goto _LL301;_tmp465=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp464)->f2;
if((int)_tmp465 != 2)goto _LL301;_tmp466=_tmp44B.f2;if((int)_tmp466 != 1)goto _LL301;
_LL300: goto _LL302;_LL301: _tmp467=_tmp44B.f1;if(_tmp467 <= (void*)3?1:*((int*)
_tmp467)!= 5)goto _LL303;_tmp468=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp467)->f2;
if((int)_tmp468 != 2)goto _LL303;_tmp469=_tmp44B.f2;if(_tmp469 <= (void*)3?1:*((int*)
_tmp469)!= 5)goto _LL303;_tmp46A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp469)->f2;
if((int)_tmp46A != 1)goto _LL303;_LL302: goto _LL304;_LL303: _tmp46B=_tmp44B.f1;if(
_tmp46B <= (void*)3?1:*((int*)_tmp46B)!= 5)goto _LL305;_tmp46C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46B)->f2;if((int)_tmp46C != 2)goto _LL305;_tmp46D=_tmp44B.f2;if(_tmp46D <= (
void*)3?1:*((int*)_tmp46D)!= 5)goto _LL305;_tmp46E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46D)->f2;if((int)_tmp46E != 0)goto _LL305;_LL304: goto _LL306;_LL305: _tmp46F=
_tmp44B.f1;if(_tmp46F <= (void*)3?1:*((int*)_tmp46F)!= 5)goto _LL307;_tmp470=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp46F)->f2;if((int)_tmp470 != 1)goto _LL307;
_tmp471=_tmp44B.f2;if(_tmp471 <= (void*)3?1:*((int*)_tmp471)!= 5)goto _LL307;
_tmp472=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp471)->f2;if((int)_tmp472 != 
0)goto _LL307;_LL306: goto _LL308;_LL307: _tmp473=_tmp44B.f1;if(_tmp473 <= (void*)3?1:*((
int*)_tmp473)!= 17)goto _LL309;_tmp474=_tmp44B.f2;if(_tmp474 <= (void*)3?1:*((int*)
_tmp474)!= 5)goto _LL309;_tmp475=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp474)->f2;
if((int)_tmp475 != 1)goto _LL309;_LL308: goto _LL30A;_LL309: _tmp476=_tmp44B.f1;if(
_tmp476 <= (void*)3?1:*((int*)_tmp476)!= 17)goto _LL30B;_tmp477=_tmp44B.f2;if(
_tmp477 <= (void*)3?1:*((int*)_tmp477)!= 5)goto _LL30B;_tmp478=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp477)->f2;if((int)_tmp478 != 0)goto _LL30B;_LL30A: goto _LL30C;_LL30B: _tmp479=
_tmp44B.f1;if(_tmp479 <= (void*)3?1:*((int*)_tmp479)!= 14)goto _LL30D;_tmp47A=
_tmp44B.f2;if(_tmp47A <= (void*)3?1:*((int*)_tmp47A)!= 5)goto _LL30D;_tmp47B=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp47A)->f2;if((int)_tmp47B != 1)goto _LL30D;
_LL30C: goto _LL30E;_LL30D: _tmp47C=_tmp44B.f1;if(_tmp47C <= (void*)3?1:*((int*)
_tmp47C)!= 14)goto _LL30F;_tmp47D=_tmp44B.f2;if(_tmp47D <= (void*)3?1:*((int*)
_tmp47D)!= 5)goto _LL30F;_tmp47E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47D)->f2;
if((int)_tmp47E != 0)goto _LL30F;_LL30E: return 1;_LL30F:;_LL310: return 0;_LL2EA:;}
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=
el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0?1: Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp47F=_cycalloc(sizeof(*_tmp47F));_tmp47F->v=(
void*)t1;_tmp47F;});}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)
max_arith_type->v))return 0;}{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){({struct Cyc_String_pa_struct
_tmp483;_tmp483.tag=0;_tmp483.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{
struct Cyc_String_pa_struct _tmp482;_tmp482.tag=0;_tmp482.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp480[2]={& _tmp482,& _tmp483};
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp481="type mismatch: expecting %s but found %s";
_tag_arr(_tmp481,sizeof(char),_get_zero_arr_size(_tmp481,41));}),_tag_arr(
_tmp480,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp484=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL312: if(_tmp484 <= (void*)3?1:*((int*)_tmp484)!= 4)goto _LL314;_LL313: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL311;_LL314:;_LL315: return 0;_LL311:;}
return 1;}int Cyc_Tcutil_is_integral_type(void*t){void*_tmp485=Cyc_Tcutil_compress(
t);_LL317: if(_tmp485 <= (void*)3?1:*((int*)_tmp485)!= 5)goto _LL319;_LL318: goto
_LL31A;_LL319: if(_tmp485 <= (void*)3?1:*((int*)_tmp485)!= 14)goto _LL31B;_LL31A:
goto _LL31C;_LL31B: if(_tmp485 <= (void*)3?1:*((int*)_tmp485)!= 17)goto _LL31D;
_LL31C: goto _LL31E;_LL31D: if(_tmp485 <= (void*)3?1:*((int*)_tmp485)!= 12)goto
_LL31F;_LL31E: goto _LL320;_LL31F: if(_tmp485 <= (void*)3?1:*((int*)_tmp485)!= 13)
goto _LL321;_LL320: return 1;_LL321:;_LL322: return 0;_LL316:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*
_tmp486[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp487="integral size mismatch; conversion supplied";
_tag_arr(_tmp487,sizeof(char),_get_zero_arr_size(_tmp487,44));}),_tag_arr(
_tmp486,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp488[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp489="integral size mismatch; conversion supplied";
_tag_arr(_tmp489,sizeof(char),_get_zero_arr_size(_tmp489,44));}),_tag_arr(
_tmp488,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp48B=({struct _tuple6 _tmp48A;_tmp48A.f1=
t1;_tmp48A.f2=t2;_tmp48A;});void*_tmp48C;struct Cyc_Absyn_PtrInfo _tmp48D;void*
_tmp48E;struct Cyc_Absyn_PtrInfo _tmp48F;void*_tmp490;struct Cyc_Absyn_ArrayInfo
_tmp491;void*_tmp492;struct Cyc_Absyn_Tqual _tmp493;struct Cyc_Absyn_Exp*_tmp494;
struct Cyc_Absyn_Conref*_tmp495;void*_tmp496;struct Cyc_Absyn_ArrayInfo _tmp497;
void*_tmp498;struct Cyc_Absyn_Tqual _tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Conref*
_tmp49B;void*_tmp49C;struct Cyc_Absyn_TunionFieldInfo _tmp49D;void*_tmp49E;struct
Cyc_Absyn_Tuniondecl*_tmp49F;struct Cyc_Absyn_Tunionfield*_tmp4A0;struct Cyc_List_List*
_tmp4A1;void*_tmp4A2;struct Cyc_Absyn_TunionInfo _tmp4A3;void*_tmp4A4;struct Cyc_Absyn_Tuniondecl**
_tmp4A5;struct Cyc_Absyn_Tuniondecl*_tmp4A6;struct Cyc_List_List*_tmp4A7;void*
_tmp4A8;struct Cyc_Absyn_PtrInfo _tmp4A9;void*_tmp4AA;struct Cyc_Absyn_Tqual _tmp4AB;
struct Cyc_Absyn_PtrAtts _tmp4AC;void*_tmp4AD;struct Cyc_Absyn_Conref*_tmp4AE;
struct Cyc_Absyn_Conref*_tmp4AF;struct Cyc_Absyn_Conref*_tmp4B0;void*_tmp4B1;
struct Cyc_Absyn_TunionInfo _tmp4B2;void*_tmp4B3;struct Cyc_Absyn_Tuniondecl**
_tmp4B4;struct Cyc_Absyn_Tuniondecl*_tmp4B5;struct Cyc_List_List*_tmp4B6;void*
_tmp4B7;void*_tmp4B8;void*_tmp4B9;void*_tmp4BA;void*_tmp4BB;void*_tmp4BC;_LL324:
_tmp48C=_tmp48B.f1;if(_tmp48C <= (void*)3?1:*((int*)_tmp48C)!= 4)goto _LL326;
_tmp48D=((struct Cyc_Absyn_PointerType_struct*)_tmp48C)->f1;_tmp48E=_tmp48B.f2;
if(_tmp48E <= (void*)3?1:*((int*)_tmp48E)!= 4)goto _LL326;_tmp48F=((struct Cyc_Absyn_PointerType_struct*)
_tmp48E)->f1;_LL325: {int okay=1;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp48D.ptr_atts).nullable,(
_tmp48F.ptr_atts).nullable)){void*_tmp4BD=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp48D.ptr_atts).nullable))->v;
int _tmp4BE;_LL333: if(_tmp4BD <= (void*)1?1:*((int*)_tmp4BD)!= 0)goto _LL335;
_tmp4BE=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4BD)->f1;_LL334: okay=!
_tmp4BE;goto _LL332;_LL335:;_LL336:({void*_tmp4BF[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp4C0="silent_castable conref not eq";
_tag_arr(_tmp4C0,sizeof(char),_get_zero_arr_size(_tmp4C0,30));}),_tag_arr(
_tmp4BF,sizeof(void*),0));});_LL332:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(
_tmp48D.ptr_atts).bounds,(_tmp48F.ptr_atts).bounds)){struct _tuple6 _tmp4C2=({
struct _tuple6 _tmp4C1;_tmp4C1.f1=(void*)(Cyc_Absyn_compress_conref((_tmp48D.ptr_atts).bounds))->v;
_tmp4C1.f2=(void*)(Cyc_Absyn_compress_conref((_tmp48F.ptr_atts).bounds))->v;
_tmp4C1;});void*_tmp4C3;void*_tmp4C4;void*_tmp4C5;void*_tmp4C6;void*_tmp4C7;
_LL338: _tmp4C3=_tmp4C2.f1;if(_tmp4C3 <= (void*)1?1:*((int*)_tmp4C3)!= 0)goto
_LL33A;_tmp4C4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4C3)->f1;_tmp4C5=
_tmp4C2.f2;if(_tmp4C5 <= (void*)1?1:*((int*)_tmp4C5)!= 0)goto _LL33A;_tmp4C6=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp4C5)->f1;_LL339:{struct _tuple6 _tmp4C9=({
struct _tuple6 _tmp4C8;_tmp4C8.f1=_tmp4C4;_tmp4C8.f2=_tmp4C6;_tmp4C8;});void*
_tmp4CA;void*_tmp4CB;void*_tmp4CC;void*_tmp4CD;void*_tmp4CE;struct Cyc_Absyn_Exp*
_tmp4CF;void*_tmp4D0;struct Cyc_Absyn_Exp*_tmp4D1;void*_tmp4D2;void*_tmp4D3;
struct Cyc_Absyn_Exp*_tmp4D4;void*_tmp4D5;void*_tmp4D6;void*_tmp4D7;void*_tmp4D8;
void*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4DA;void*_tmp4DB;void*_tmp4DC;void*_tmp4DD;
void*_tmp4DE;_LL33F: _tmp4CA=_tmp4C9.f1;if(_tmp4CA <= (void*)1?1:*((int*)_tmp4CA)
!= 0)goto _LL341;_tmp4CB=_tmp4C9.f2;if((int)_tmp4CB != 0)goto _LL341;_LL340: goto
_LL342;_LL341: _tmp4CC=_tmp4C9.f1;if((int)_tmp4CC != 0)goto _LL343;_tmp4CD=_tmp4C9.f2;
if((int)_tmp4CD != 0)goto _LL343;_LL342: okay=1;goto _LL33E;_LL343: _tmp4CE=_tmp4C9.f1;
if(_tmp4CE <= (void*)1?1:*((int*)_tmp4CE)!= 0)goto _LL345;_tmp4CF=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4CE)->f1;_tmp4D0=_tmp4C9.f2;if(_tmp4D0 <= (void*)1?1:*((int*)_tmp4D0)!= 0)
goto _LL345;_tmp4D1=((struct Cyc_Absyn_Upper_b_struct*)_tmp4D0)->f1;_LL344: okay=
okay?Cyc_Evexp_lte_const_exp(_tmp4D1,_tmp4CF): 0;if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,(_tmp48F.ptr_atts).zero_term))({void*_tmp4DF[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp4E0="implicit cast to shorter array";
_tag_arr(_tmp4E0,sizeof(char),_get_zero_arr_size(_tmp4E0,31));}),_tag_arr(
_tmp4DF,sizeof(void*),0));});goto _LL33E;_LL345: _tmp4D2=_tmp4C9.f1;if((int)
_tmp4D2 != 0)goto _LL347;_tmp4D3=_tmp4C9.f2;if(_tmp4D3 <= (void*)1?1:*((int*)
_tmp4D3)!= 0)goto _LL347;_tmp4D4=((struct Cyc_Absyn_Upper_b_struct*)_tmp4D3)->f1;
_LL346: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(
_tmp48D.ptr_atts).zero_term)?Cyc_Tcutil_is_bound_one((_tmp48F.ptr_atts).bounds):
0)goto _LL33E;okay=0;goto _LL33E;_LL347: _tmp4D5=_tmp4C9.f1;if(_tmp4D5 <= (void*)1?1:*((
int*)_tmp4D5)!= 1)goto _LL349;_tmp4D6=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp4D5)->f1;_tmp4D7=_tmp4C9.f2;if(_tmp4D7 <= (void*)1?1:*((int*)_tmp4D7)!= 1)
goto _LL349;_tmp4D8=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4D7)->f1;
_LL348: if(!Cyc_Tcutil_unify(_tmp4D6,_tmp4D8)){struct _tuple6 _tmp4E2=({struct
_tuple6 _tmp4E1;_tmp4E1.f1=Cyc_Tcutil_compress(_tmp4D6);_tmp4E1.f2=Cyc_Tcutil_compress(
_tmp4D8);_tmp4E1;});void*_tmp4E3;int _tmp4E4;void*_tmp4E5;int _tmp4E6;_LL350:
_tmp4E3=_tmp4E2.f1;if(_tmp4E3 <= (void*)3?1:*((int*)_tmp4E3)!= 18)goto _LL352;
_tmp4E4=((struct Cyc_Absyn_TypeInt_struct*)_tmp4E3)->f1;_tmp4E5=_tmp4E2.f2;if(
_tmp4E5 <= (void*)3?1:*((int*)_tmp4E5)!= 18)goto _LL352;_tmp4E6=((struct Cyc_Absyn_TypeInt_struct*)
_tmp4E5)->f1;_LL351: if(_tmp4E4 < _tmp4E6)okay=0;goto _LL34F;_LL352:;_LL353: okay=0;
goto _LL34F;_LL34F:;}goto _LL33E;_LL349: _tmp4D9=_tmp4C9.f1;if(_tmp4D9 <= (void*)1?1:*((
int*)_tmp4D9)!= 0)goto _LL34B;_tmp4DA=((struct Cyc_Absyn_Upper_b_struct*)_tmp4D9)->f1;
_tmp4DB=_tmp4C9.f2;if(_tmp4DB <= (void*)1?1:*((int*)_tmp4DB)!= 1)goto _LL34B;
_tmp4DC=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4DB)->f1;_LL34A: {
unsigned int _tmp4E8;int _tmp4E9;struct _tuple7 _tmp4E7=Cyc_Evexp_eval_const_uint_exp(
_tmp4DA);_tmp4E8=_tmp4E7.f1;_tmp4E9=_tmp4E7.f2;if(!_tmp4E9){okay=0;goto _LL33E;}{
void*_tmp4EA=Cyc_Tcutil_compress(_tmp4DC);int _tmp4EB;_LL355: if(_tmp4EA <= (void*)
3?1:*((int*)_tmp4EA)!= 18)goto _LL357;_tmp4EB=((struct Cyc_Absyn_TypeInt_struct*)
_tmp4EA)->f1;_LL356: if(_tmp4E8 < _tmp4EB)okay=0;goto _LL354;_LL357:;_LL358: okay=0;
goto _LL354;_LL354:;}goto _LL33E;}_LL34B: _tmp4DD=_tmp4C9.f1;if(_tmp4DD <= (void*)1?
1:*((int*)_tmp4DD)!= 1)goto _LL34D;_LL34C: goto _LL34E;_LL34D: _tmp4DE=_tmp4C9.f2;
if(_tmp4DE <= (void*)1?1:*((int*)_tmp4DE)!= 1)goto _LL33E;_LL34E: okay=0;goto _LL33E;
_LL33E:;}goto _LL337;_LL33A: _tmp4C7=_tmp4C2.f1;if((int)_tmp4C7 != 0)goto _LL33C;
_LL33B: okay=0;goto _LL337;_LL33C:;_LL33D:({void*_tmp4EC[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp4ED="silent_castable conrefs didn't unify";
_tag_arr(_tmp4ED,sizeof(char),_get_zero_arr_size(_tmp4ED,37));}),_tag_arr(
_tmp4EC,sizeof(void*),0));});_LL337:;}okay=okay?Cyc_Tcutil_unify((void*)_tmp48D.elt_typ,(
void*)_tmp48F.elt_typ): 0;okay=okay?Cyc_Tcutil_unify((void*)(_tmp48D.ptr_atts).rgn,(
void*)(_tmp48F.ptr_atts).rgn)?1: Cyc_Tcenv_region_outlives(te,(void*)(_tmp48D.ptr_atts).rgn,(
void*)(_tmp48F.ptr_atts).rgn): 0;okay=okay?!(_tmp48D.elt_tq).q_const?1:(_tmp48F.elt_tq).q_const:
0;okay=okay?((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp48D.ptr_atts).zero_term,(
_tmp48F.ptr_atts).zero_term)?1:(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,(_tmp48D.ptr_atts).zero_term)?(_tmp48F.elt_tq).q_const: 0): 0;return okay;}_LL326:
_tmp490=_tmp48B.f1;if(_tmp490 <= (void*)3?1:*((int*)_tmp490)!= 7)goto _LL328;
_tmp491=((struct Cyc_Absyn_ArrayType_struct*)_tmp490)->f1;_tmp492=(void*)_tmp491.elt_type;
_tmp493=_tmp491.tq;_tmp494=_tmp491.num_elts;_tmp495=_tmp491.zero_term;_tmp496=
_tmp48B.f2;if(_tmp496 <= (void*)3?1:*((int*)_tmp496)!= 7)goto _LL328;_tmp497=((
struct Cyc_Absyn_ArrayType_struct*)_tmp496)->f1;_tmp498=(void*)_tmp497.elt_type;
_tmp499=_tmp497.tq;_tmp49A=_tmp497.num_elts;_tmp49B=_tmp497.zero_term;_LL327: {
int okay;okay=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp495,_tmp49B)?(_tmp494 != 0?
_tmp49A != 0: 0)?Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp494,(struct Cyc_Absyn_Exp*)
_tmp49A): 0: 0;return(okay?Cyc_Tcutil_unify(_tmp492,_tmp498): 0)?!_tmp493.q_const?1:
_tmp499.q_const: 0;}_LL328: _tmp49C=_tmp48B.f1;if(_tmp49C <= (void*)3?1:*((int*)
_tmp49C)!= 3)goto _LL32A;_tmp49D=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp49C)->f1;_tmp49E=(void*)_tmp49D.field_info;if(*((int*)_tmp49E)!= 1)goto
_LL32A;_tmp49F=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp49E)->f1;_tmp4A0=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp49E)->f2;_tmp4A1=_tmp49D.targs;
_tmp4A2=_tmp48B.f2;if(_tmp4A2 <= (void*)3?1:*((int*)_tmp4A2)!= 2)goto _LL32A;
_tmp4A3=((struct Cyc_Absyn_TunionType_struct*)_tmp4A2)->f1;_tmp4A4=(void*)_tmp4A3.tunion_info;
if(*((int*)_tmp4A4)!= 1)goto _LL32A;_tmp4A5=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp4A4)->f1;_tmp4A6=*_tmp4A5;_tmp4A7=_tmp4A3.targs;_LL329: if((_tmp49F == _tmp4A6?
1: Cyc_Absyn_qvar_cmp(_tmp49F->name,_tmp4A6->name)== 0)?_tmp4A0->typs == 0: 0){for(
0;_tmp4A1 != 0?_tmp4A7 != 0: 0;(_tmp4A1=_tmp4A1->tl,_tmp4A7=_tmp4A7->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp4A1->hd,(void*)_tmp4A7->hd))break;}if(_tmp4A1 == 0?_tmp4A7 == 0: 0)return
1;}return 0;_LL32A: _tmp4A8=_tmp48B.f1;if(_tmp4A8 <= (void*)3?1:*((int*)_tmp4A8)!= 
4)goto _LL32C;_tmp4A9=((struct Cyc_Absyn_PointerType_struct*)_tmp4A8)->f1;_tmp4AA=(
void*)_tmp4A9.elt_typ;_tmp4AB=_tmp4A9.elt_tq;_tmp4AC=_tmp4A9.ptr_atts;_tmp4AD=(
void*)_tmp4AC.rgn;_tmp4AE=_tmp4AC.nullable;_tmp4AF=_tmp4AC.bounds;_tmp4B0=
_tmp4AC.zero_term;_tmp4B1=_tmp48B.f2;if(_tmp4B1 <= (void*)3?1:*((int*)_tmp4B1)!= 
2)goto _LL32C;_tmp4B2=((struct Cyc_Absyn_TunionType_struct*)_tmp4B1)->f1;_tmp4B3=(
void*)_tmp4B2.tunion_info;if(*((int*)_tmp4B3)!= 1)goto _LL32C;_tmp4B4=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp4B3)->f1;_tmp4B5=*_tmp4B4;_tmp4B6=_tmp4B2.targs;_tmp4B7=(void*)_tmp4B2.rgn;
_LL32B:{void*_tmp4EE=Cyc_Tcutil_compress(_tmp4AA);struct Cyc_Absyn_TunionFieldInfo
_tmp4EF;void*_tmp4F0;struct Cyc_Absyn_Tuniondecl*_tmp4F1;struct Cyc_Absyn_Tunionfield*
_tmp4F2;struct Cyc_List_List*_tmp4F3;_LL35A: if(_tmp4EE <= (void*)3?1:*((int*)
_tmp4EE)!= 3)goto _LL35C;_tmp4EF=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp4EE)->f1;_tmp4F0=(void*)_tmp4EF.field_info;if(*((int*)_tmp4F0)!= 1)goto
_LL35C;_tmp4F1=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4F0)->f1;_tmp4F2=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4F0)->f2;_tmp4F3=_tmp4EF.targs;
_LL35B: if(!Cyc_Tcutil_unify(_tmp4AD,_tmp4B7)?!Cyc_Tcenv_region_outlives(te,
_tmp4AD,_tmp4B7): 0)return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4AE,
Cyc_Absyn_false_conref))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp4AF,Cyc_Absyn_bounds_one_conref))return 0;if(!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp4B0,Cyc_Absyn_false_conref))return 0;if(Cyc_Absyn_qvar_cmp(_tmp4B5->name,
_tmp4F1->name)== 0?_tmp4F2->typs != 0: 0){int okay=1;for(0;_tmp4F3 != 0?_tmp4B6 != 0:
0;(_tmp4F3=_tmp4F3->tl,_tmp4B6=_tmp4B6->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4F3->hd,(
void*)_tmp4B6->hd)){okay=0;break;}}if((!okay?1: _tmp4F3 != 0)?1: _tmp4B6 != 0)return
0;return 1;}goto _LL359;_LL35C:;_LL35D: goto _LL359;_LL359:;}return 0;_LL32C: _tmp4B8=
_tmp48B.f1;if(_tmp4B8 <= (void*)3?1:*((int*)_tmp4B8)!= 14)goto _LL32E;_tmp4B9=
_tmp48B.f2;if(_tmp4B9 <= (void*)3?1:*((int*)_tmp4B9)!= 5)goto _LL32E;_tmp4BA=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4B9)->f2;if((int)_tmp4BA != 2)goto _LL32E;
_LL32D: goto _LL32F;_LL32E: _tmp4BB=_tmp48B.f1;if(_tmp4BB <= (void*)3?1:*((int*)
_tmp4BB)!= 17)goto _LL330;_tmp4BC=_tmp48B.f2;if(_tmp4BC <= (void*)3?1:*((int*)
_tmp4BC)!= 5)goto _LL330;_LL32F: return 1;_LL330:;_LL331: return Cyc_Tcutil_unify(t1,
t2);_LL323:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp4F4=Cyc_Tcutil_compress(
t);_LL35F: if(_tmp4F4 <= (void*)3?1:*((int*)_tmp4F4)!= 4)goto _LL361;_LL360: return 1;
_LL361:;_LL362: return 0;_LL35E:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp4F5=(void*)e->r;void*_tmp4F6;int _tmp4F7;void*_tmp4F8;char _tmp4F9;void*
_tmp4FA;short _tmp4FB;void*_tmp4FC;long long _tmp4FD;void*_tmp4FE;struct Cyc_Absyn_Exp*
_tmp4FF;_LL364: if(*((int*)_tmp4F5)!= 0)goto _LL366;_tmp4F6=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp4F5)->f1;if(_tmp4F6 <= (void*)1?1:*((int*)_tmp4F6)!= 2)goto _LL366;_tmp4F7=((
struct Cyc_Absyn_Int_c_struct*)_tmp4F6)->f2;if(_tmp4F7 != 0)goto _LL366;_LL365: goto
_LL367;_LL366: if(*((int*)_tmp4F5)!= 0)goto _LL368;_tmp4F8=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp4F5)->f1;if(_tmp4F8 <= (void*)1?1:*((int*)_tmp4F8)!= 0)goto _LL368;_tmp4F9=((
struct Cyc_Absyn_Char_c_struct*)_tmp4F8)->f2;if(_tmp4F9 != 0)goto _LL368;_LL367:
goto _LL369;_LL368: if(*((int*)_tmp4F5)!= 0)goto _LL36A;_tmp4FA=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp4F5)->f1;if(_tmp4FA <= (void*)1?1:*((int*)_tmp4FA)!= 1)goto _LL36A;_tmp4FB=((
struct Cyc_Absyn_Short_c_struct*)_tmp4FA)->f2;if(_tmp4FB != 0)goto _LL36A;_LL369:
goto _LL36B;_LL36A: if(*((int*)_tmp4F5)!= 0)goto _LL36C;_tmp4FC=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp4F5)->f1;if(_tmp4FC <= (void*)1?1:*((int*)_tmp4FC)!= 3)goto _LL36C;_tmp4FD=((
struct Cyc_Absyn_LongLong_c_struct*)_tmp4FC)->f2;if(_tmp4FD != 0)goto _LL36C;_LL36B:
return 1;_LL36C: if(*((int*)_tmp4F5)!= 13)goto _LL36E;_tmp4FE=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp4F5)->f1;_tmp4FF=((struct Cyc_Absyn_Cast_e_struct*)_tmp4F5)->f2;_LL36D: return
Cyc_Tcutil_is_zero(_tmp4FF)?Cyc_Tcutil_admits_zero(_tmp4FE): 0;_LL36E:;_LL36F:
return 0;_LL363:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt
Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)
2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)1)};int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2)?Cyc_Tcutil_is_zero(e1): 0){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500[0]=({struct Cyc_Absyn_Const_e_struct
_tmp501;_tmp501.tag=0;_tmp501.f1=(void*)((void*)0);_tmp501;});_tmp500;})));{
struct Cyc_List_List*_tmp502=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp503=({struct Cyc_Absyn_PointerType_struct*_tmp504=_cycalloc(sizeof(*_tmp504));
_tmp504[0]=({struct Cyc_Absyn_PointerType_struct _tmp505;_tmp505.tag=4;_tmp505.f1=({
struct Cyc_Absyn_PtrInfo _tmp506;_tmp506.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp509=_cycalloc(sizeof(*
_tmp509));_tmp509->v=_tmp502;_tmp509;}));_tmp506.elt_tq=Cyc_Absyn_empty_tqual();
_tmp506.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp507;_tmp507.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp508=_cycalloc(
sizeof(*_tmp508));_tmp508->v=_tmp502;_tmp508;}));_tmp507.nullable=Cyc_Absyn_true_conref;
_tmp507.bounds=Cyc_Absyn_empty_conref();_tmp507.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp507;});_tmp506;});_tmp505;});_tmp504;});(void*)(((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp503));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}struct _tagged_arr Cyc_Tcutil_coercion2string(void*c){void*
_tmp50A=c;_LL371: if((int)_tmp50A != 0)goto _LL373;_LL372: return({const char*_tmp50B="unknown";
_tag_arr(_tmp50B,sizeof(char),_get_zero_arr_size(_tmp50B,8));});_LL373: if((int)
_tmp50A != 1)goto _LL375;_LL374: return({const char*_tmp50C="no coercion";_tag_arr(
_tmp50C,sizeof(char),_get_zero_arr_size(_tmp50C,12));});_LL375: if((int)_tmp50A != 
2)goto _LL377;_LL376: return({const char*_tmp50D="null check";_tag_arr(_tmp50D,
sizeof(char),_get_zero_arr_size(_tmp50D,11));});_LL377: if((int)_tmp50A != 3)goto
_LL370;_LL378: return({const char*_tmp50E="other coercion";_tag_arr(_tmp50E,
sizeof(char),_get_zero_arr_size(_tmp50E,15));});_LL370:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(
t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2)?Cyc_Tcutil_is_arithmetic_type(
t1): 0){if(Cyc_Tcutil_will_lose_precision(t1,t2))({struct Cyc_String_pa_struct
_tmp512;_tmp512.tag=0;_tmp512.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp511;_tmp511.tag=0;_tmp511.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp50F[2]={&
_tmp511,& _tmp512};Cyc_Tcutil_warn(e->loc,({const char*_tmp510="integral size mismatch; %s -> %s conversion supplied";
_tag_arr(_tmp510,sizeof(char),_get_zero_arr_size(_tmp510,53));}),_tag_arr(
_tmp50F,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)1);
return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,(void*)3);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;
else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(
te,e,t2,c);if(c != (void*)2)({struct Cyc_String_pa_struct _tmp516;_tmp516.tag=0;
_tmp516.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp515;_tmp515.tag=0;_tmp515.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp513[2]={& _tmp515,&
_tmp516};Cyc_Tcutil_warn(e->loc,({const char*_tmp514="implicit cast from %s to %s";
_tag_arr(_tmp514,sizeof(char),_get_zero_arr_size(_tmp514,28));}),_tag_arr(
_tmp513,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp517=Cyc_Tcutil_compress(t);
_LL37A: if(_tmp517 <= (void*)3?1:*((int*)_tmp517)!= 5)goto _LL37C;_LL37B: goto _LL37D;
_LL37C: if((int)_tmp517 != 1)goto _LL37E;_LL37D: goto _LL37F;_LL37E: if(_tmp517 <= (
void*)3?1:*((int*)_tmp517)!= 6)goto _LL380;_LL37F: return 1;_LL380:;_LL381: return 0;
_LL379:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*inst,
struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*_tmp518=_cycalloc(sizeof(*
_tmp518));_tmp518->f1=x->tq;_tmp518->f2=Cyc_Tcutil_substitute(inst,(void*)x->type);
_tmp518;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*
te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp519=t1;struct Cyc_List_List*
_tmp51A;struct Cyc_Absyn_AggrInfo _tmp51B;void*_tmp51C;struct Cyc_Absyn_Aggrdecl**
_tmp51D;struct Cyc_Absyn_Aggrdecl*_tmp51E;struct Cyc_List_List*_tmp51F;void*
_tmp520;struct Cyc_List_List*_tmp521;struct Cyc_Absyn_FnInfo _tmp522;_LL383: if((int)
_tmp519 != 0)goto _LL385;_LL384: return 0;_LL385: if(_tmp519 <= (void*)3?1:*((int*)
_tmp519)!= 9)goto _LL387;_tmp51A=((struct Cyc_Absyn_TupleType_struct*)_tmp519)->f1;
_LL386: return _tmp51A;_LL387: if(_tmp519 <= (void*)3?1:*((int*)_tmp519)!= 10)goto
_LL389;_tmp51B=((struct Cyc_Absyn_AggrType_struct*)_tmp519)->f1;_tmp51C=(void*)
_tmp51B.aggr_info;if(*((int*)_tmp51C)!= 1)goto _LL389;_tmp51D=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp51C)->f1;_tmp51E=*_tmp51D;_tmp51F=_tmp51B.targs;_LL388: if((((void*)_tmp51E->kind
== (void*)1?1: _tmp51E->impl == 0)?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp51E->impl))->exist_vars != 0)?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp51E->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp523=_cycalloc(
sizeof(*_tmp523));_tmp523->hd=({struct _tuple4*_tmp524=_cycalloc(sizeof(*_tmp524));
_tmp524->f1=Cyc_Absyn_empty_tqual();_tmp524->f2=t1;_tmp524;});_tmp523->tl=0;
_tmp523;});{struct Cyc_List_List*_tmp525=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp51E->tvs,_tmp51F);return((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp525,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51E->impl))->fields);}_LL389: if(
_tmp519 <= (void*)3?1:*((int*)_tmp519)!= 11)goto _LL38B;_tmp520=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp519)->f1;if((int)_tmp520 != 0)goto _LL38B;_tmp521=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp519)->f2;_LL38A: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp521);_LL38B: if(_tmp519 <= (void*)3?1:*((int*)
_tmp519)!= 8)goto _LL38D;_tmp522=((struct Cyc_Absyn_FnType_struct*)_tmp519)->f1;
_LL38C: return({struct Cyc_List_List*_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526->hd=({
struct _tuple4*_tmp527=_cycalloc(sizeof(*_tmp527));_tmp527->f1=Cyc_Absyn_const_tqual();
_tmp527->f2=t1;_tmp527;});_tmp526->tl=0;_tmp526;});_LL38D:;_LL38E: return({struct
Cyc_List_List*_tmp528=_cycalloc(sizeof(*_tmp528));_tmp528->hd=({struct _tuple4*
_tmp529=_cycalloc(sizeof(*_tmp529));_tmp529->f1=Cyc_Absyn_empty_tqual();_tmp529->f2=
t1;_tmp529;});_tmp528->tl=0;_tmp528;});_LL382:;}}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp52A=(void*)t->hd;_LL390: if((int)_tmp52A != 16)goto _LL392;
_LL391: goto _LL393;_LL392: if((int)_tmp52A != 3)goto _LL394;_LL393: goto _LL395;_LL394:
if(_tmp52A <= (void*)17?1:*((int*)_tmp52A)!= 4)goto _LL396;_LL395: continue;_LL396:;
_LL397: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;
_LL38F:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1)?Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2):
0)return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp52C=({struct _tuple6 _tmp52B;_tmp52B.f1=t1;_tmp52B.f2=t2;_tmp52B;});void*
_tmp52D;struct Cyc_Absyn_PtrInfo _tmp52E;void*_tmp52F;struct Cyc_Absyn_Tqual _tmp530;
struct Cyc_Absyn_PtrAtts _tmp531;void*_tmp532;struct Cyc_Absyn_Conref*_tmp533;
struct Cyc_Absyn_Conref*_tmp534;struct Cyc_Absyn_Conref*_tmp535;void*_tmp536;
struct Cyc_Absyn_PtrInfo _tmp537;void*_tmp538;struct Cyc_Absyn_Tqual _tmp539;struct
Cyc_Absyn_PtrAtts _tmp53A;void*_tmp53B;struct Cyc_Absyn_Conref*_tmp53C;struct Cyc_Absyn_Conref*
_tmp53D;struct Cyc_Absyn_Conref*_tmp53E;void*_tmp53F;struct Cyc_Absyn_FnInfo
_tmp540;void*_tmp541;struct Cyc_Absyn_FnInfo _tmp542;_LL399: _tmp52D=_tmp52C.f1;if(
_tmp52D <= (void*)3?1:*((int*)_tmp52D)!= 4)goto _LL39B;_tmp52E=((struct Cyc_Absyn_PointerType_struct*)
_tmp52D)->f1;_tmp52F=(void*)_tmp52E.elt_typ;_tmp530=_tmp52E.elt_tq;_tmp531=
_tmp52E.ptr_atts;_tmp532=(void*)_tmp531.rgn;_tmp533=_tmp531.nullable;_tmp534=
_tmp531.bounds;_tmp535=_tmp531.zero_term;_tmp536=_tmp52C.f2;if(_tmp536 <= (void*)
3?1:*((int*)_tmp536)!= 4)goto _LL39B;_tmp537=((struct Cyc_Absyn_PointerType_struct*)
_tmp536)->f1;_tmp538=(void*)_tmp537.elt_typ;_tmp539=_tmp537.elt_tq;_tmp53A=
_tmp537.ptr_atts;_tmp53B=(void*)_tmp53A.rgn;_tmp53C=_tmp53A.nullable;_tmp53D=
_tmp53A.bounds;_tmp53E=_tmp53A.zero_term;_LL39A: if(_tmp530.q_const?!_tmp539.q_const:
0)return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp533,_tmp53C)?((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp533): 0)?!((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp53C): 0)return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp535,
_tmp53E)?!((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp535): 0)?((
int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp53E): 0)return 0;if(!
Cyc_Tcutil_unify(_tmp532,_tmp53B)?!Cyc_Tcenv_region_outlives(te,_tmp532,_tmp53B):
0)return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp534,
_tmp53D)){struct _tuple6 _tmp544=({struct _tuple6 _tmp543;_tmp543.f1=Cyc_Absyn_conref_val(
_tmp534);_tmp543.f2=Cyc_Absyn_conref_val(_tmp53D);_tmp543;});void*_tmp545;void*
_tmp546;void*_tmp547;struct Cyc_Absyn_Exp*_tmp548;void*_tmp549;struct Cyc_Absyn_Exp*
_tmp54A;_LL3A0: _tmp545=_tmp544.f1;if(_tmp545 <= (void*)1?1:*((int*)_tmp545)!= 0)
goto _LL3A2;_tmp546=_tmp544.f2;if((int)_tmp546 != 0)goto _LL3A2;_LL3A1: goto _LL39F;
_LL3A2: _tmp547=_tmp544.f1;if(_tmp547 <= (void*)1?1:*((int*)_tmp547)!= 0)goto
_LL3A4;_tmp548=((struct Cyc_Absyn_Upper_b_struct*)_tmp547)->f1;_tmp549=_tmp544.f2;
if(_tmp549 <= (void*)1?1:*((int*)_tmp549)!= 0)goto _LL3A4;_tmp54A=((struct Cyc_Absyn_Upper_b_struct*)
_tmp549)->f1;_LL3A3: if(!Cyc_Evexp_lte_const_exp(_tmp54A,_tmp548))return 0;goto
_LL39F;_LL3A4:;_LL3A5: return 0;_LL39F:;}return Cyc_Tcutil_ptrsubtype(te,({struct
Cyc_List_List*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B->hd=({struct _tuple6*
_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C->f1=t1;_tmp54C->f2=t2;_tmp54C;});
_tmp54B->tl=assume;_tmp54B;}),_tmp52F,_tmp538);_LL39B: _tmp53F=_tmp52C.f1;if(
_tmp53F <= (void*)3?1:*((int*)_tmp53F)!= 8)goto _LL39D;_tmp540=((struct Cyc_Absyn_FnType_struct*)
_tmp53F)->f1;_tmp541=_tmp52C.f2;if(_tmp541 <= (void*)3?1:*((int*)_tmp541)!= 8)
goto _LL39D;_tmp542=((struct Cyc_Absyn_FnType_struct*)_tmp541)->f1;_LL39C: if(
_tmp540.tvars != 0?1: _tmp542.tvars != 0){struct Cyc_List_List*_tmp54D=_tmp540.tvars;
struct Cyc_List_List*_tmp54E=_tmp542.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp54D)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp54E))return 0;{
struct _RegionHandle _tmp54F=_new_region("r");struct _RegionHandle*r=& _tmp54F;
_push_region(r);{struct Cyc_List_List*inst=0;while(_tmp54D != 0){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp54D->hd)!= Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp54E))->hd)){int _tmp550=0;_npop_handler(0);
return _tmp550;}inst=({struct Cyc_List_List*_tmp551=_region_malloc(r,sizeof(*
_tmp551));_tmp551->hd=({struct _tuple8*_tmp552=_region_malloc(r,sizeof(*_tmp552));
_tmp552->f1=(struct Cyc_Absyn_Tvar*)_tmp54E->hd;_tmp552->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553[0]=({struct Cyc_Absyn_VarType_struct
_tmp554;_tmp554.tag=1;_tmp554.f1=(struct Cyc_Absyn_Tvar*)_tmp54D->hd;_tmp554;});
_tmp553;});_tmp552;});_tmp551->tl=inst;_tmp551;});_tmp54D=_tmp54D->tl;_tmp54E=
_tmp54E->tl;}if(inst != 0){_tmp540.tvars=0;_tmp542.tvars=0;{int _tmp559=Cyc_Tcutil_subtype(
te,assume,(void*)({struct Cyc_Absyn_FnType_struct*_tmp555=_cycalloc(sizeof(*
_tmp555));_tmp555[0]=({struct Cyc_Absyn_FnType_struct _tmp556;_tmp556.tag=8;
_tmp556.f1=_tmp540;_tmp556;});_tmp555;}),(void*)({struct Cyc_Absyn_FnType_struct*
_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557[0]=({struct Cyc_Absyn_FnType_struct
_tmp558;_tmp558.tag=8;_tmp558.f1=_tmp542;_tmp558;});_tmp557;}));_npop_handler(0);
return _tmp559;}}};_pop_region(r);}}if(!Cyc_Tcutil_subtype(te,assume,(void*)
_tmp540.ret_typ,(void*)_tmp542.ret_typ))return 0;{struct Cyc_List_List*_tmp55A=
_tmp540.args;struct Cyc_List_List*_tmp55B=_tmp542.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp55A)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp55B))return 0;for(0;_tmp55A != 0;(_tmp55A=_tmp55A->tl,_tmp55B=_tmp55B->tl)){
struct Cyc_Absyn_Tqual _tmp55D;void*_tmp55E;struct _tuple2 _tmp55C=*((struct _tuple2*)
_tmp55A->hd);_tmp55D=_tmp55C.f2;_tmp55E=_tmp55C.f3;{struct Cyc_Absyn_Tqual _tmp560;
void*_tmp561;struct _tuple2 _tmp55F=*((struct _tuple2*)((struct Cyc_List_List*)
_check_null(_tmp55B))->hd);_tmp560=_tmp55F.f2;_tmp561=_tmp55F.f3;if((_tmp560.q_const?
!_tmp55D.q_const: 0)?1: !Cyc_Tcutil_subtype(te,assume,_tmp561,_tmp55E))return 0;}}
if(_tmp540.c_varargs != _tmp542.c_varargs)return 0;if(_tmp540.cyc_varargs != 0?
_tmp542.cyc_varargs != 0: 0){struct Cyc_Absyn_VarargInfo _tmp562=*_tmp540.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp563=*_tmp542.cyc_varargs;if(((_tmp563.tq).q_const?
!(_tmp562.tq).q_const: 0)?1: !Cyc_Tcutil_subtype(te,assume,(void*)_tmp563.type,(
void*)_tmp562.type))return 0;}else{if(_tmp540.cyc_varargs != 0?1: _tmp542.cyc_varargs
!= 0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp540.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp542.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp540.rgn_po,_tmp542.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp540.attributes,_tmp542.attributes))return 0;return 1;}_LL39D:;_LL39E: return 0;
_LL398:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp565=({
struct _tuple6 _tmp564;_tmp564.f1=Cyc_Tcutil_compress(t1);_tmp564.f2=Cyc_Tcutil_compress(
t2);_tmp564;});void*_tmp566;void*_tmp567;void*_tmp568;void*_tmp569;_LL3A7:
_tmp566=_tmp565.f1;if(_tmp566 <= (void*)3?1:*((int*)_tmp566)!= 5)goto _LL3A9;
_tmp567=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp566)->f2;_tmp568=_tmp565.f2;
if(_tmp568 <= (void*)3?1:*((int*)_tmp568)!= 5)goto _LL3A9;_tmp569=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp568)->f2;_LL3A8: return _tmp567 == _tmp569;_LL3A9:;
_LL3AA: return 0;_LL3A6:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple4 _tmp56B;struct
Cyc_Absyn_Tqual _tmp56C;void*_tmp56D;struct _tuple4*_tmp56A=(struct _tuple4*)tqs1->hd;
_tmp56B=*_tmp56A;_tmp56C=_tmp56B.f1;_tmp56D=_tmp56B.f2;{struct _tuple4 _tmp56F;
struct Cyc_Absyn_Tqual _tmp570;void*_tmp571;struct _tuple4*_tmp56E=(struct _tuple4*)
tqs2->hd;_tmp56F=*_tmp56E;_tmp570=_tmp56F.f1;_tmp571=_tmp56F.f2;if(_tmp570.q_const?
Cyc_Tcutil_subtype(te,assume,_tmp56D,_tmp571): 0)continue;else{if(Cyc_Tcutil_unify(
_tmp56D,_tmp571))continue;else{if(Cyc_Tcutil_isomorphic(_tmp56D,_tmp571))
continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp572=Cyc_Tcutil_compress(t);void*_tmp573;_LL3AC: if(_tmp572 <= (void*)3?1:*((
int*)_tmp572)!= 5)goto _LL3AE;_tmp573=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp572)->f2;if((int)_tmp573 != 0)goto _LL3AE;_LL3AD: return 1;_LL3AE:;_LL3AF: return
0;_LL3AB:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp574=t2;
void*_tmp575;_LL3B1: if(_tmp574 <= (void*)3?1:*((int*)_tmp574)!= 5)goto _LL3B3;
_tmp575=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp574)->f2;if((int)_tmp575 != 
2)goto _LL3B3;_LL3B2: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)1;goto
_LL3B0;_LL3B3:;_LL3B4: goto _LL3B0;_LL3B0:;}{void*_tmp576=t1;struct Cyc_Absyn_PtrInfo
_tmp577;void*_tmp578;struct Cyc_Absyn_Tqual _tmp579;struct Cyc_Absyn_PtrAtts _tmp57A;
void*_tmp57B;struct Cyc_Absyn_Conref*_tmp57C;struct Cyc_Absyn_Conref*_tmp57D;
struct Cyc_Absyn_Conref*_tmp57E;struct Cyc_Absyn_ArrayInfo _tmp57F;void*_tmp580;
struct Cyc_Absyn_Tqual _tmp581;struct Cyc_Absyn_Exp*_tmp582;struct Cyc_Absyn_Conref*
_tmp583;struct Cyc_Absyn_Enumdecl*_tmp584;_LL3B6: if(_tmp576 <= (void*)3?1:*((int*)
_tmp576)!= 4)goto _LL3B8;_tmp577=((struct Cyc_Absyn_PointerType_struct*)_tmp576)->f1;
_tmp578=(void*)_tmp577.elt_typ;_tmp579=_tmp577.elt_tq;_tmp57A=_tmp577.ptr_atts;
_tmp57B=(void*)_tmp57A.rgn;_tmp57C=_tmp57A.nullable;_tmp57D=_tmp57A.bounds;
_tmp57E=_tmp57A.zero_term;_LL3B7:{void*_tmp585=t2;struct Cyc_Absyn_PtrInfo _tmp586;
void*_tmp587;struct Cyc_Absyn_Tqual _tmp588;struct Cyc_Absyn_PtrAtts _tmp589;void*
_tmp58A;struct Cyc_Absyn_Conref*_tmp58B;struct Cyc_Absyn_Conref*_tmp58C;struct Cyc_Absyn_Conref*
_tmp58D;_LL3C5: if(_tmp585 <= (void*)3?1:*((int*)_tmp585)!= 4)goto _LL3C7;_tmp586=((
struct Cyc_Absyn_PointerType_struct*)_tmp585)->f1;_tmp587=(void*)_tmp586.elt_typ;
_tmp588=_tmp586.elt_tq;_tmp589=_tmp586.ptr_atts;_tmp58A=(void*)_tmp589.rgn;
_tmp58B=_tmp589.nullable;_tmp58C=_tmp589.bounds;_tmp58D=_tmp589.zero_term;_LL3C6: {
void*coercion=(void*)3;struct Cyc_List_List*_tmp58E=({struct Cyc_List_List*_tmp59D=
_cycalloc(sizeof(*_tmp59D));_tmp59D->hd=({struct _tuple6*_tmp59E=_cycalloc(
sizeof(*_tmp59E));_tmp59E->f1=t1;_tmp59E->f2=t2;_tmp59E;});_tmp59D->tl=0;_tmp59D;});
int _tmp58F=Cyc_Tcutil_ptrsubtype(te,_tmp58E,_tmp578,_tmp587)?!_tmp579.q_const?1:
_tmp588.q_const: 0;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int
zeroterm_ok=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp57E,_tmp58D)?1: !((int(*)(struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp58D);int _tmp590=_tmp58F?0:(((Cyc_Tcutil_bits_only(
_tmp578)?Cyc_Tcutil_is_char_type(_tmp587): 0)?!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp58D): 0)?_tmp588.q_const?1: !_tmp579.q_const: 0);int
bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp57D,_tmp58C);
if(!bounds_ok?!_tmp590: 0){struct _tuple6 _tmp592=({struct _tuple6 _tmp591;_tmp591.f1=
Cyc_Absyn_conref_val(_tmp57D);_tmp591.f2=Cyc_Absyn_conref_val(_tmp58C);_tmp591;});
void*_tmp593;struct Cyc_Absyn_Exp*_tmp594;void*_tmp595;struct Cyc_Absyn_Exp*
_tmp596;void*_tmp597;void*_tmp598;void*_tmp599;void*_tmp59A;void*_tmp59B;void*
_tmp59C;_LL3CA: _tmp593=_tmp592.f1;if(_tmp593 <= (void*)1?1:*((int*)_tmp593)!= 0)
goto _LL3CC;_tmp594=((struct Cyc_Absyn_Upper_b_struct*)_tmp593)->f1;_tmp595=
_tmp592.f2;if(_tmp595 <= (void*)1?1:*((int*)_tmp595)!= 0)goto _LL3CC;_tmp596=((
struct Cyc_Absyn_Upper_b_struct*)_tmp595)->f1;_LL3CB: if(Cyc_Evexp_lte_const_exp(
_tmp596,_tmp594))bounds_ok=1;goto _LL3C9;_LL3CC: _tmp597=_tmp592.f1;if(_tmp597 <= (
void*)1?1:*((int*)_tmp597)!= 1)goto _LL3CE;_tmp598=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp597)->f1;_tmp599=_tmp592.f2;if(_tmp599 <= (void*)1?1:*((int*)_tmp599)!= 1)
goto _LL3CE;_tmp59A=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp599)->f1;
_LL3CD: bounds_ok=Cyc_Tcutil_unify(_tmp598,_tmp59A);goto _LL3C9;_LL3CE: _tmp59B=
_tmp592.f1;if(_tmp59B <= (void*)1?1:*((int*)_tmp59B)!= 1)goto _LL3D0;_LL3CF: goto
_LL3D1;_LL3D0: _tmp59C=_tmp592.f2;if(_tmp59C <= (void*)1?1:*((int*)_tmp59C)!= 1)
goto _LL3D2;_LL3D1: bounds_ok=0;goto _LL3C9;_LL3D2:;_LL3D3: bounds_ok=1;goto _LL3C9;
_LL3C9:;}if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp57C)?!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp58B):
0)coercion=(void*)2;if(((bounds_ok?zeroterm_ok: 0)?_tmp58F?1: _tmp590: 0)?Cyc_Tcutil_unify(
_tmp57B,_tmp58A)?1: Cyc_Tcenv_region_outlives(te,_tmp57B,_tmp58A): 0)return
coercion;else{return(void*)0;}}}_LL3C7:;_LL3C8: goto _LL3C4;_LL3C4:;}return(void*)
0;_LL3B8: if(_tmp576 <= (void*)3?1:*((int*)_tmp576)!= 7)goto _LL3BA;_tmp57F=((
struct Cyc_Absyn_ArrayType_struct*)_tmp576)->f1;_tmp580=(void*)_tmp57F.elt_type;
_tmp581=_tmp57F.tq;_tmp582=_tmp57F.num_elts;_tmp583=_tmp57F.zero_term;_LL3B9:{
void*_tmp59F=t2;struct Cyc_Absyn_ArrayInfo _tmp5A0;void*_tmp5A1;struct Cyc_Absyn_Tqual
_tmp5A2;struct Cyc_Absyn_Exp*_tmp5A3;struct Cyc_Absyn_Conref*_tmp5A4;_LL3D5: if(
_tmp59F <= (void*)3?1:*((int*)_tmp59F)!= 7)goto _LL3D7;_tmp5A0=((struct Cyc_Absyn_ArrayType_struct*)
_tmp59F)->f1;_tmp5A1=(void*)_tmp5A0.elt_type;_tmp5A2=_tmp5A0.tq;_tmp5A3=_tmp5A0.num_elts;
_tmp5A4=_tmp5A0.zero_term;_LL3D6: {int okay;okay=((_tmp582 != 0?_tmp5A3 != 0: 0)?((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp583,_tmp5A4): 0)?Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)
_tmp5A3,(struct Cyc_Absyn_Exp*)_tmp582): 0;return((okay?Cyc_Tcutil_unify(_tmp580,
_tmp5A1): 0)?!_tmp581.q_const?1: _tmp5A2.q_const: 0)?(void*)3:(void*)0;}_LL3D7:;
_LL3D8: return(void*)0;_LL3D4:;}return(void*)0;_LL3BA: if(_tmp576 <= (void*)3?1:*((
int*)_tmp576)!= 12)goto _LL3BC;_tmp584=((struct Cyc_Absyn_EnumType_struct*)_tmp576)->f2;
_LL3BB:{void*_tmp5A5=t2;struct Cyc_Absyn_Enumdecl*_tmp5A6;_LL3DA: if(_tmp5A5 <= (
void*)3?1:*((int*)_tmp5A5)!= 12)goto _LL3DC;_tmp5A6=((struct Cyc_Absyn_EnumType_struct*)
_tmp5A5)->f2;_LL3DB: if((_tmp584->fields != 0?_tmp5A6->fields != 0: 0)?((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp584->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5A6->fields))->v): 0)
return(void*)1;goto _LL3D9;_LL3DC:;_LL3DD: goto _LL3D9;_LL3D9:;}goto _LL3BD;_LL3BC:
if(_tmp576 <= (void*)3?1:*((int*)_tmp576)!= 5)goto _LL3BE;_LL3BD: goto _LL3BF;_LL3BE:
if((int)_tmp576 != 1)goto _LL3C0;_LL3BF: goto _LL3C1;_LL3C0: if(_tmp576 <= (void*)3?1:*((
int*)_tmp576)!= 6)goto _LL3C2;_LL3C1: return Cyc_Tcutil_coerceable(t2)?(void*)1:(
void*)0;_LL3C2:;_LL3C3: return(void*)0;_LL3B5:;}}void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*
_tmp5A7=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*
_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8[0]=({struct Cyc_Absyn_Cast_e_struct
_tmp5A9;_tmp5A9.tag=13;_tmp5A9.f1=(void*)t;_tmp5A9.f2=_tmp5A7;_tmp5A9.f3=0;
_tmp5A9.f4=(void*)c;_tmp5A9;});_tmp5A8;})));e->topt=({struct Cyc_Core_Opt*_tmp5AA=
_cycalloc(sizeof(*_tmp5AA));_tmp5AA->v=(void*)t;_tmp5AA;});}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmp5AB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL3DF: if(_tmp5AB <= (void*)3?1:*((int*)_tmp5AB)!= 5)
goto _LL3E1;_LL3E0: goto _LL3E2;_LL3E1: if(_tmp5AB <= (void*)3?1:*((int*)_tmp5AB)!= 
12)goto _LL3E3;_LL3E2: goto _LL3E4;_LL3E3: if(_tmp5AB <= (void*)3?1:*((int*)_tmp5AB)
!= 13)goto _LL3E5;_LL3E4: goto _LL3E6;_LL3E5: if(_tmp5AB <= (void*)3?1:*((int*)
_tmp5AB)!= 17)goto _LL3E7;_LL3E6: goto _LL3E8;_LL3E7: if(_tmp5AB <= (void*)3?1:*((int*)
_tmp5AB)!= 14)goto _LL3E9;_LL3E8: return 1;_LL3E9: if(_tmp5AB <= (void*)3?1:*((int*)
_tmp5AB)!= 0)goto _LL3EB;_LL3EA: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL3EB:;_LL3EC: return 0;_LL3DE:;}int
Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return
1;{void*_tmp5AC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL3EE: if((int)_tmp5AC != 1)goto _LL3F0;_LL3EF: goto _LL3F1;_LL3F0: if(_tmp5AC <= (
void*)3?1:*((int*)_tmp5AC)!= 6)goto _LL3F2;_LL3F1: return 1;_LL3F2:;_LL3F3: return 0;
_LL3ED:;}}int Cyc_Tcutil_is_function_type(void*t){void*_tmp5AD=Cyc_Tcutil_compress(
t);_LL3F5: if(_tmp5AD <= (void*)3?1:*((int*)_tmp5AD)!= 8)goto _LL3F7;_LL3F6: return 1;
_LL3F7:;_LL3F8: return 0;_LL3F4:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*
t2){struct _tuple6 _tmp5AF=({struct _tuple6 _tmp5AE;_tmp5AE.f1=t1;_tmp5AE.f2=t2;
_tmp5AE;});void*_tmp5B0;int _tmp5B1;void*_tmp5B2;int _tmp5B3;void*_tmp5B4;void*
_tmp5B5;void*_tmp5B6;void*_tmp5B7;void*_tmp5B8;void*_tmp5B9;void*_tmp5BA;void*
_tmp5BB;void*_tmp5BC;void*_tmp5BD;void*_tmp5BE;void*_tmp5BF;void*_tmp5C0;void*
_tmp5C1;void*_tmp5C2;void*_tmp5C3;void*_tmp5C4;void*_tmp5C5;void*_tmp5C6;void*
_tmp5C7;void*_tmp5C8;void*_tmp5C9;void*_tmp5CA;void*_tmp5CB;void*_tmp5CC;void*
_tmp5CD;void*_tmp5CE;void*_tmp5CF;_LL3FA: _tmp5B0=_tmp5AF.f1;if(_tmp5B0 <= (void*)
3?1:*((int*)_tmp5B0)!= 6)goto _LL3FC;_tmp5B1=((struct Cyc_Absyn_DoubleType_struct*)
_tmp5B0)->f1;_tmp5B2=_tmp5AF.f2;if(_tmp5B2 <= (void*)3?1:*((int*)_tmp5B2)!= 6)
goto _LL3FC;_tmp5B3=((struct Cyc_Absyn_DoubleType_struct*)_tmp5B2)->f1;_LL3FB: if(
_tmp5B1)return t1;else{return t2;}_LL3FC: _tmp5B4=_tmp5AF.f1;if(_tmp5B4 <= (void*)3?
1:*((int*)_tmp5B4)!= 6)goto _LL3FE;_LL3FD: return t1;_LL3FE: _tmp5B5=_tmp5AF.f2;if(
_tmp5B5 <= (void*)3?1:*((int*)_tmp5B5)!= 6)goto _LL400;_LL3FF: return t2;_LL400:
_tmp5B6=_tmp5AF.f1;if((int)_tmp5B6 != 1)goto _LL402;_LL401: goto _LL403;_LL402:
_tmp5B7=_tmp5AF.f2;if((int)_tmp5B7 != 1)goto _LL404;_LL403: return(void*)1;_LL404:
_tmp5B8=_tmp5AF.f1;if(_tmp5B8 <= (void*)3?1:*((int*)_tmp5B8)!= 5)goto _LL406;
_tmp5B9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5B8)->f1;if((int)_tmp5B9 != 
1)goto _LL406;_tmp5BA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5B8)->f2;if((
int)_tmp5BA != 3)goto _LL406;_LL405: goto _LL407;_LL406: _tmp5BB=_tmp5AF.f2;if(
_tmp5BB <= (void*)3?1:*((int*)_tmp5BB)!= 5)goto _LL408;_tmp5BC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5BB)->f1;if((int)_tmp5BC != 1)goto _LL408;_tmp5BD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5BB)->f2;if((int)_tmp5BD != 3)goto _LL408;_LL407: return Cyc_Absyn_ulonglong_typ;
_LL408: _tmp5BE=_tmp5AF.f1;if(_tmp5BE <= (void*)3?1:*((int*)_tmp5BE)!= 5)goto
_LL40A;_tmp5BF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5BE)->f1;if((int)
_tmp5BF != 2)goto _LL40A;_tmp5C0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5BE)->f2;
if((int)_tmp5C0 != 3)goto _LL40A;_LL409: goto _LL40B;_LL40A: _tmp5C1=_tmp5AF.f2;if(
_tmp5C1 <= (void*)3?1:*((int*)_tmp5C1)!= 5)goto _LL40C;_tmp5C2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C1)->f1;if((int)_tmp5C2 != 2)goto _LL40C;_tmp5C3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C1)->f2;if((int)_tmp5C3 != 3)goto _LL40C;_LL40B: goto _LL40D;_LL40C: _tmp5C4=
_tmp5AF.f1;if(_tmp5C4 <= (void*)3?1:*((int*)_tmp5C4)!= 5)goto _LL40E;_tmp5C5=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5C4)->f1;if((int)_tmp5C5 != 0)goto _LL40E;
_tmp5C6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C4)->f2;if((int)_tmp5C6 != 
3)goto _LL40E;_LL40D: goto _LL40F;_LL40E: _tmp5C7=_tmp5AF.f2;if(_tmp5C7 <= (void*)3?1:*((
int*)_tmp5C7)!= 5)goto _LL410;_tmp5C8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C7)->f1;if((int)_tmp5C8 != 0)goto _LL410;_tmp5C9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C7)->f2;if((int)_tmp5C9 != 3)goto _LL410;_LL40F: return Cyc_Absyn_slonglong_typ;
_LL410: _tmp5CA=_tmp5AF.f1;if(_tmp5CA <= (void*)3?1:*((int*)_tmp5CA)!= 5)goto
_LL412;_tmp5CB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5CA)->f1;if((int)
_tmp5CB != 1)goto _LL412;_tmp5CC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5CA)->f2;
if((int)_tmp5CC != 2)goto _LL412;_LL411: goto _LL413;_LL412: _tmp5CD=_tmp5AF.f2;if(
_tmp5CD <= (void*)3?1:*((int*)_tmp5CD)!= 5)goto _LL414;_tmp5CE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5CD)->f1;if((int)_tmp5CE != 1)goto _LL414;_tmp5CF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5CD)->f2;if((int)_tmp5CF != 2)goto _LL414;_LL413: return Cyc_Absyn_uint_typ;
_LL414:;_LL415: return Cyc_Absyn_sint_typ;_LL3F9:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp5D0=(void*)e->r;struct Cyc_Core_Opt*_tmp5D1;
_LL417: if(*((int*)_tmp5D0)!= 4)goto _LL419;_tmp5D1=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp5D0)->f2;if(_tmp5D1 != 0)goto _LL419;_LL418:({void*_tmp5D2[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp5D3="assignment in test";_tag_arr(_tmp5D3,sizeof(char),
_get_zero_arr_size(_tmp5D3,19));}),_tag_arr(_tmp5D2,sizeof(void*),0));});goto
_LL416;_LL419:;_LL41A: goto _LL416;_LL416:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp5D5=({struct _tuple6 _tmp5D4;_tmp5D4.f1=c1;_tmp5D4.f2=c2;_tmp5D4;});
void*_tmp5D6;void*_tmp5D7;void*_tmp5D8;void*_tmp5D9;void*_tmp5DA;struct Cyc_Core_Opt*
_tmp5DB;struct Cyc_Core_Opt**_tmp5DC;void*_tmp5DD;struct Cyc_Core_Opt*_tmp5DE;
struct Cyc_Core_Opt**_tmp5DF;void*_tmp5E0;struct Cyc_Core_Opt*_tmp5E1;struct Cyc_Core_Opt**
_tmp5E2;void*_tmp5E3;void*_tmp5E4;void*_tmp5E5;void*_tmp5E6;void*_tmp5E7;void*
_tmp5E8;struct Cyc_Core_Opt*_tmp5E9;struct Cyc_Core_Opt**_tmp5EA;void*_tmp5EB;void*
_tmp5EC;struct Cyc_Core_Opt*_tmp5ED;struct Cyc_Core_Opt**_tmp5EE;void*_tmp5EF;void*
_tmp5F0;struct Cyc_Core_Opt*_tmp5F1;struct Cyc_Core_Opt**_tmp5F2;void*_tmp5F3;
_LL41C: _tmp5D6=_tmp5D5.f1;if(*((int*)_tmp5D6)!= 0)goto _LL41E;_tmp5D7=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp5D6)->f1;_tmp5D8=_tmp5D5.f2;if(*((int*)_tmp5D8)
!= 0)goto _LL41E;_tmp5D9=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5D8)->f1;
_LL41D: return _tmp5D7 == _tmp5D9;_LL41E: _tmp5DA=_tmp5D5.f2;if(*((int*)_tmp5DA)!= 1)
goto _LL420;_tmp5DB=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5DA)->f1;_tmp5DC=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5DA)->f1;_LL41F:*
_tmp5DC=({struct Cyc_Core_Opt*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->v=(
void*)c1;_tmp5F4;});return 1;_LL420: _tmp5DD=_tmp5D5.f1;if(*((int*)_tmp5DD)!= 1)
goto _LL422;_tmp5DE=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5DD)->f1;_tmp5DF=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5DD)->f1;_LL421:*
_tmp5DF=({struct Cyc_Core_Opt*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->v=(
void*)c2;_tmp5F5;});return 1;_LL422: _tmp5E0=_tmp5D5.f1;if(*((int*)_tmp5E0)!= 2)
goto _LL424;_tmp5E1=((struct Cyc_Absyn_Less_kb_struct*)_tmp5E0)->f1;_tmp5E2=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp5E0)->f1;_tmp5E3=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp5E0)->f2;_tmp5E4=_tmp5D5.f2;if(*((
int*)_tmp5E4)!= 0)goto _LL424;_tmp5E5=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp5E4)->f1;_LL423: if(Cyc_Tcutil_kind_leq(_tmp5E5,_tmp5E3)){*_tmp5E2=({struct
Cyc_Core_Opt*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->v=(void*)c2;_tmp5F6;});
return 1;}else{return 0;}_LL424: _tmp5E6=_tmp5D5.f1;if(*((int*)_tmp5E6)!= 0)goto
_LL426;_tmp5E7=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5E6)->f1;_tmp5E8=
_tmp5D5.f2;if(*((int*)_tmp5E8)!= 2)goto _LL426;_tmp5E9=((struct Cyc_Absyn_Less_kb_struct*)
_tmp5E8)->f1;_tmp5EA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp5E8)->f1;_tmp5EB=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp5E8)->f2;
_LL425: if(Cyc_Tcutil_kind_leq(_tmp5E7,_tmp5EB)){*_tmp5EA=({struct Cyc_Core_Opt*
_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7->v=(void*)c1;_tmp5F7;});return 1;}
else{return 0;}_LL426: _tmp5EC=_tmp5D5.f1;if(*((int*)_tmp5EC)!= 2)goto _LL41B;
_tmp5ED=((struct Cyc_Absyn_Less_kb_struct*)_tmp5EC)->f1;_tmp5EE=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp5EC)->f1;_tmp5EF=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp5EC)->f2;_tmp5F0=_tmp5D5.f2;if(*((int*)_tmp5F0)!= 2)goto _LL41B;_tmp5F1=((
struct Cyc_Absyn_Less_kb_struct*)_tmp5F0)->f1;_tmp5F2=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp5F0)->f1;_tmp5F3=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp5F0)->f2;_LL427: if(Cyc_Tcutil_kind_leq(_tmp5EF,_tmp5F3)){*_tmp5F2=({struct
Cyc_Core_Opt*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->v=(void*)c1;_tmp5F8;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp5F3,_tmp5EF)){*_tmp5EE=({struct Cyc_Core_Opt*
_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->v=(void*)c2;_tmp5F9;});return 1;}
else{return 0;}}_LL41B:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp5FA=_cycalloc_atomic(sizeof(*_tmp5FA));_tmp5FA[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp5FA;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp5FF;_tmp5FF.tag=1;_tmp5FF.f1=(unsigned int)i;{void*
_tmp5FD[1]={& _tmp5FF};Cyc_aprintf(({const char*_tmp5FE="#%d";_tag_arr(_tmp5FE,
sizeof(char),_get_zero_arr_size(_tmp5FE,4));}),_tag_arr(_tmp5FD,sizeof(void*),1));}});
return({struct Cyc_Absyn_Tvar*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->name=({
struct _tagged_arr*_tmp5FC=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp5FC[0]=s;
_tmp5FC;});_tmp5FB->identity=0;_tmp5FB->kind=(void*)k;_tmp5FB;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _tagged_arr _tmp600=*t->name;return*((const char*)
_check_unknown_subscript(_tmp600,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp603;_tmp603.tag=0;
_tmp603.f1=(struct _tagged_arr)((struct _tagged_arr)*t->name);{void*_tmp601[1]={&
_tmp603};Cyc_printf(({const char*_tmp602="%s";_tag_arr(_tmp602,sizeof(char),
_get_zero_arr_size(_tmp602,3));}),_tag_arr(_tmp601,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(
t))return;{struct _tagged_arr _tmp604=Cyc_strconcat(({const char*_tmp609="`";
_tag_arr(_tmp609,sizeof(char),_get_zero_arr_size(_tmp609,2));}),(struct
_tagged_arr)*t->name);({struct _tagged_arr _tmp605=_tagged_arr_plus(_tmp604,
sizeof(char),1);char _tmp606=*((char*)_check_unknown_subscript(_tmp605,sizeof(
char),0));char _tmp607='t';if(_get_arr_size(_tmp605,sizeof(char))== 1?_tmp606 == '\000'?
_tmp607 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp605.curr)=_tmp607;});t->name=({
struct _tagged_arr*_tmp608=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp608[0]=(
struct _tagged_arr)_tmp604;_tmp608;});}}struct _tuple13{struct _tagged_arr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(
struct _tuple13*x){return({struct _tuple2*_tmp60A=_cycalloc(sizeof(*_tmp60A));
_tmp60A->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp60B=_cycalloc(sizeof(*
_tmp60B));_tmp60B->v=(*x).f1;_tmp60B;});_tmp60A->f2=(*x).f2;_tmp60A->f3=(*x).f3;
_tmp60A;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp60C=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp60C=({
struct Cyc_List_List*_tmp60D=_cycalloc(sizeof(*_tmp60D));_tmp60D->hd=(void*)((
void*)atts->hd);_tmp60D->tl=_tmp60C;_tmp60D;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E[0]=({struct Cyc_Absyn_FnType_struct
_tmp60F;_tmp60F.tag=8;_tmp60F.f1=({struct Cyc_Absyn_FnInfo _tmp610;_tmp610.tvars=
fd->tvs;_tmp610.effect=fd->effect;_tmp610.ret_typ=(void*)((void*)fd->ret_type);
_tmp610.args=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple13*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp610.c_varargs=
fd->c_varargs;_tmp610.cyc_varargs=fd->cyc_varargs;_tmp610.rgn_po=fd->rgn_po;
_tmp610.attributes=_tmp60C;_tmp610;});_tmp60F;});_tmp60E;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple14{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple14*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*t).f2;}static
struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({struct _tuple4*
_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611->f1=(*pr).f1;_tmp611->f2=t;_tmp611;});}
struct _tuple15{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple16{
struct _tuple15*f1;void*f2;};static struct _tuple16*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple2*y){return({struct _tuple16*_tmp612=_region_malloc(
rgn,sizeof(*_tmp612));_tmp612->f1=({struct _tuple15*_tmp613=_region_malloc(rgn,
sizeof(*_tmp613));_tmp613->f1=(*y).f1;_tmp613->f2=(*y).f2;_tmp613;});_tmp612->f2=(*
y).f3;_tmp612;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(struct _tuple16*w){
struct _tuple15*_tmp615;void*_tmp616;struct _tuple16 _tmp614=*w;_tmp615=_tmp614.f1;
_tmp616=_tmp614.f2;{struct Cyc_Core_Opt*_tmp618;struct Cyc_Absyn_Tqual _tmp619;
struct _tuple15 _tmp617=*_tmp615;_tmp618=_tmp617.f1;_tmp619=_tmp617.f2;return({
struct _tuple2*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->f1=_tmp618;_tmp61A->f2=
_tmp619;_tmp61A->f3=_tmp616;_tmp61A;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp61B=
_cycalloc(sizeof(*_tmp61B));_tmp61B->name=f->name;_tmp61B->tq=f->tq;_tmp61B->type=(
void*)t;_tmp61B->width=f->width;_tmp61B->attributes=f->attributes;_tmp61B;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp61C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp61D;struct Cyc_Absyn_AggrInfo _tmp61E;void*_tmp61F;struct Cyc_List_List*_tmp620;
struct Cyc_Absyn_TunionInfo _tmp621;void*_tmp622;struct Cyc_List_List*_tmp623;void*
_tmp624;struct Cyc_Absyn_TunionFieldInfo _tmp625;void*_tmp626;struct Cyc_List_List*
_tmp627;struct _tuple1*_tmp628;struct Cyc_List_List*_tmp629;struct Cyc_Absyn_Typedefdecl*
_tmp62A;void**_tmp62B;struct Cyc_Absyn_ArrayInfo _tmp62C;void*_tmp62D;struct Cyc_Absyn_Tqual
_tmp62E;struct Cyc_Absyn_Exp*_tmp62F;struct Cyc_Absyn_Conref*_tmp630;struct Cyc_Absyn_PtrInfo
_tmp631;void*_tmp632;struct Cyc_Absyn_Tqual _tmp633;struct Cyc_Absyn_PtrAtts _tmp634;
void*_tmp635;struct Cyc_Absyn_Conref*_tmp636;struct Cyc_Absyn_Conref*_tmp637;
struct Cyc_Absyn_Conref*_tmp638;struct Cyc_Absyn_FnInfo _tmp639;struct Cyc_List_List*
_tmp63A;struct Cyc_Core_Opt*_tmp63B;void*_tmp63C;struct Cyc_List_List*_tmp63D;int
_tmp63E;struct Cyc_Absyn_VarargInfo*_tmp63F;struct Cyc_List_List*_tmp640;struct Cyc_List_List*
_tmp641;struct Cyc_List_List*_tmp642;void*_tmp643;struct Cyc_List_List*_tmp644;
struct Cyc_Core_Opt*_tmp645;void*_tmp646;void*_tmp647;void*_tmp648;void*_tmp649;
void*_tmp64A;struct Cyc_List_List*_tmp64B;_LL429: if(_tmp61C <= (void*)3?1:*((int*)
_tmp61C)!= 1)goto _LL42B;_tmp61D=((struct Cyc_Absyn_VarType_struct*)_tmp61C)->f1;
_LL42A: {struct _handler_cons _tmp64C;_push_handler(& _tmp64C);{int _tmp64E=0;if(
setjmp(_tmp64C.handler))_tmp64E=1;if(!_tmp64E){{void*_tmp64F=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp61D);_npop_handler(0);return
_tmp64F;};_pop_handler();}else{void*_tmp64D=(void*)_exn_thrown;void*_tmp651=
_tmp64D;_LL45C: if(_tmp651 != Cyc_Core_Not_found)goto _LL45E;_LL45D: return t;_LL45E:;
_LL45F:(void)_throw(_tmp651);_LL45B:;}}}_LL42B: if(_tmp61C <= (void*)3?1:*((int*)
_tmp61C)!= 10)goto _LL42D;_tmp61E=((struct Cyc_Absyn_AggrType_struct*)_tmp61C)->f1;
_tmp61F=(void*)_tmp61E.aggr_info;_tmp620=_tmp61E.targs;_LL42C: {struct Cyc_List_List*
_tmp652=Cyc_Tcutil_substs(rgn,inst,_tmp620);return _tmp652 == _tmp620?t:(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653[0]=({
struct Cyc_Absyn_AggrType_struct _tmp654;_tmp654.tag=10;_tmp654.f1=({struct Cyc_Absyn_AggrInfo
_tmp655;_tmp655.aggr_info=(void*)_tmp61F;_tmp655.targs=_tmp652;_tmp655;});
_tmp654;});_tmp653;});}_LL42D: if(_tmp61C <= (void*)3?1:*((int*)_tmp61C)!= 2)goto
_LL42F;_tmp621=((struct Cyc_Absyn_TunionType_struct*)_tmp61C)->f1;_tmp622=(void*)
_tmp621.tunion_info;_tmp623=_tmp621.targs;_tmp624=(void*)_tmp621.rgn;_LL42E: {
struct Cyc_List_List*_tmp656=Cyc_Tcutil_substs(rgn,inst,_tmp623);void*_tmp657=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp624);return(_tmp656 == _tmp623?_tmp657 == _tmp624: 0)?t:(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658[0]=({
struct Cyc_Absyn_TunionType_struct _tmp659;_tmp659.tag=2;_tmp659.f1=({struct Cyc_Absyn_TunionInfo
_tmp65A;_tmp65A.tunion_info=(void*)_tmp622;_tmp65A.targs=_tmp656;_tmp65A.rgn=(
void*)_tmp657;_tmp65A;});_tmp659;});_tmp658;});}_LL42F: if(_tmp61C <= (void*)3?1:*((
int*)_tmp61C)!= 3)goto _LL431;_tmp625=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp61C)->f1;_tmp626=(void*)_tmp625.field_info;_tmp627=_tmp625.targs;_LL430: {
struct Cyc_List_List*_tmp65B=Cyc_Tcutil_substs(rgn,inst,_tmp627);return _tmp65B == 
_tmp627?t:(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp65C=_cycalloc(
sizeof(*_tmp65C));_tmp65C[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp65D;
_tmp65D.tag=3;_tmp65D.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp65E;_tmp65E.field_info=(
void*)_tmp626;_tmp65E.targs=_tmp65B;_tmp65E;});_tmp65D;});_tmp65C;});}_LL431: if(
_tmp61C <= (void*)3?1:*((int*)_tmp61C)!= 16)goto _LL433;_tmp628=((struct Cyc_Absyn_TypedefType_struct*)
_tmp61C)->f1;_tmp629=((struct Cyc_Absyn_TypedefType_struct*)_tmp61C)->f2;_tmp62A=((
struct Cyc_Absyn_TypedefType_struct*)_tmp61C)->f3;_tmp62B=((struct Cyc_Absyn_TypedefType_struct*)
_tmp61C)->f4;_LL432: {struct Cyc_List_List*_tmp65F=Cyc_Tcutil_substs(rgn,inst,
_tmp629);return _tmp65F == _tmp629?t:(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp661;_tmp661.tag=16;_tmp661.f1=_tmp628;_tmp661.f2=_tmp65F;_tmp661.f3=_tmp62A;
_tmp661.f4=_tmp62B;_tmp661;});_tmp660;});}_LL433: if(_tmp61C <= (void*)3?1:*((int*)
_tmp61C)!= 7)goto _LL435;_tmp62C=((struct Cyc_Absyn_ArrayType_struct*)_tmp61C)->f1;
_tmp62D=(void*)_tmp62C.elt_type;_tmp62E=_tmp62C.tq;_tmp62F=_tmp62C.num_elts;
_tmp630=_tmp62C.zero_term;_LL434: {void*_tmp662=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp62D);return _tmp662 == _tmp62D?t:(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp664;_tmp664.tag=7;_tmp664.f1=({struct Cyc_Absyn_ArrayInfo _tmp665;_tmp665.elt_type=(
void*)_tmp662;_tmp665.tq=_tmp62E;_tmp665.num_elts=_tmp62F;_tmp665.zero_term=
_tmp630;_tmp665;});_tmp664;});_tmp663;});}_LL435: if(_tmp61C <= (void*)3?1:*((int*)
_tmp61C)!= 4)goto _LL437;_tmp631=((struct Cyc_Absyn_PointerType_struct*)_tmp61C)->f1;
_tmp632=(void*)_tmp631.elt_typ;_tmp633=_tmp631.elt_tq;_tmp634=_tmp631.ptr_atts;
_tmp635=(void*)_tmp634.rgn;_tmp636=_tmp634.nullable;_tmp637=_tmp634.bounds;
_tmp638=_tmp634.zero_term;_LL436: {void*_tmp666=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp632);void*_tmp667=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp635);struct Cyc_Absyn_Conref*
_tmp668=_tmp637;{void*_tmp669=(void*)(Cyc_Absyn_compress_conref(_tmp637))->v;
void*_tmp66A;void*_tmp66B;_LL461: if(_tmp669 <= (void*)1?1:*((int*)_tmp669)!= 0)
goto _LL463;_tmp66A=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp669)->f1;if(
_tmp66A <= (void*)1?1:*((int*)_tmp66A)!= 1)goto _LL463;_tmp66B=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp66A)->f1;_LL462: {void*_tmp66C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp66B);if(
_tmp66B != _tmp66C)_tmp668=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_AbsUpper_b_struct*
_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D[0]=({struct Cyc_Absyn_AbsUpper_b_struct
_tmp66E;_tmp66E.tag=1;_tmp66E.f1=(void*)_tmp66C;_tmp66E;});_tmp66D;}));goto
_LL460;}_LL463:;_LL464: goto _LL460;_LL460:;}if((_tmp666 == _tmp632?_tmp667 == 
_tmp635: 0)?_tmp668 == _tmp637: 0)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F[0]=({struct Cyc_Absyn_PointerType_struct
_tmp670;_tmp670.tag=4;_tmp670.f1=({struct Cyc_Absyn_PtrInfo _tmp671;_tmp671.elt_typ=(
void*)_tmp666;_tmp671.elt_tq=_tmp633;_tmp671.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp672;_tmp672.rgn=(void*)_tmp667;_tmp672.nullable=_tmp636;_tmp672.bounds=
_tmp668;_tmp672.zero_term=_tmp638;_tmp672;});_tmp671;});_tmp670;});_tmp66F;});}
_LL437: if(_tmp61C <= (void*)3?1:*((int*)_tmp61C)!= 8)goto _LL439;_tmp639=((struct
Cyc_Absyn_FnType_struct*)_tmp61C)->f1;_tmp63A=_tmp639.tvars;_tmp63B=_tmp639.effect;
_tmp63C=(void*)_tmp639.ret_typ;_tmp63D=_tmp639.args;_tmp63E=_tmp639.c_varargs;
_tmp63F=_tmp639.cyc_varargs;_tmp640=_tmp639.rgn_po;_tmp641=_tmp639.attributes;
_LL438:{struct Cyc_List_List*_tmp673=_tmp63A;for(0;_tmp673 != 0;_tmp673=_tmp673->tl){
inst=({struct Cyc_List_List*_tmp674=_region_malloc(rgn,sizeof(*_tmp674));_tmp674->hd=({
struct _tuple8*_tmp675=_region_malloc(rgn,sizeof(*_tmp675));_tmp675->f1=(struct
Cyc_Absyn_Tvar*)_tmp673->hd;_tmp675->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676[0]=({struct Cyc_Absyn_VarType_struct
_tmp677;_tmp677.tag=1;_tmp677.f1=(struct Cyc_Absyn_Tvar*)_tmp673->hd;_tmp677;});
_tmp676;});_tmp675;});_tmp674->tl=inst;_tmp674;});}}{struct Cyc_List_List*_tmp679;
struct Cyc_List_List*_tmp67A;struct _tuple0 _tmp678=((struct _tuple0(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple16*(*f)(
struct _RegionHandle*,struct _tuple2*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp63D));_tmp679=_tmp678.f1;
_tmp67A=_tmp678.f2;{struct Cyc_List_List*_tmp67B=Cyc_Tcutil_substs(rgn,inst,
_tmp67A);struct Cyc_List_List*_tmp67C=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(
struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp679,_tmp67B));struct Cyc_Core_Opt*
eff2;if(_tmp63B == 0)eff2=0;else{void*_tmp67D=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp63B->v);if(_tmp67D == (void*)_tmp63B->v)eff2=_tmp63B;else{eff2=({struct
Cyc_Core_Opt*_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E->v=(void*)_tmp67D;
_tmp67E;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp63F == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp680;struct Cyc_Absyn_Tqual _tmp681;void*
_tmp682;int _tmp683;struct Cyc_Absyn_VarargInfo _tmp67F=*_tmp63F;_tmp680=_tmp67F.name;
_tmp681=_tmp67F.tq;_tmp682=(void*)_tmp67F.type;_tmp683=_tmp67F.inject;{void*
_tmp684=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp682);if(_tmp684 == _tmp682)
cyc_varargs2=_tmp63F;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp685=
_cycalloc(sizeof(*_tmp685));_tmp685->name=_tmp680;_tmp685->tq=_tmp681;_tmp685->type=(
void*)_tmp684;_tmp685->inject=_tmp683;_tmp685;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp687;struct Cyc_List_List*_tmp688;struct _tuple0 _tmp686=Cyc_List_rsplit(
rgn,rgn,_tmp640);_tmp687=_tmp686.f1;_tmp688=_tmp686.f2;{struct Cyc_List_List*
_tmp689=Cyc_Tcutil_substs(rgn,inst,_tmp687);struct Cyc_List_List*_tmp68A=Cyc_Tcutil_substs(
rgn,inst,_tmp688);if(_tmp689 == _tmp687?_tmp68A == _tmp688: 0)rgn_po2=_tmp640;else{
rgn_po2=Cyc_List_zip(_tmp689,_tmp68A);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B[0]=({struct Cyc_Absyn_FnType_struct
_tmp68C;_tmp68C.tag=8;_tmp68C.f1=({struct Cyc_Absyn_FnInfo _tmp68D;_tmp68D.tvars=
_tmp63A;_tmp68D.effect=eff2;_tmp68D.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp63C);_tmp68D.args=_tmp67C;_tmp68D.c_varargs=_tmp63E;_tmp68D.cyc_varargs=
cyc_varargs2;_tmp68D.rgn_po=rgn_po2;_tmp68D.attributes=_tmp641;_tmp68D;});
_tmp68C;});_tmp68B;});}}}}}_LL439: if(_tmp61C <= (void*)3?1:*((int*)_tmp61C)!= 9)
goto _LL43B;_tmp642=((struct Cyc_Absyn_TupleType_struct*)_tmp61C)->f1;_LL43A: {
struct Cyc_List_List*_tmp68E=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp642);struct Cyc_List_List*_tmp68F=Cyc_Tcutil_substs(rgn,inst,_tmp68E);if(
_tmp68F == _tmp68E)return t;{struct Cyc_List_List*_tmp690=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp642,_tmp68F);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691[0]=({struct Cyc_Absyn_TupleType_struct
_tmp692;_tmp692.tag=9;_tmp692.f1=_tmp690;_tmp692;});_tmp691;});}}_LL43B: if(
_tmp61C <= (void*)3?1:*((int*)_tmp61C)!= 11)goto _LL43D;_tmp643=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp61C)->f1;_tmp644=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp61C)->f2;_LL43C: {
struct Cyc_List_List*_tmp693=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp644);struct Cyc_List_List*_tmp694=Cyc_Tcutil_substs(rgn,inst,_tmp693);if(
_tmp694 == _tmp693)return t;{struct Cyc_List_List*_tmp695=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp644,
_tmp694);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp696=_cycalloc(
sizeof(*_tmp696));_tmp696[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp697;
_tmp697.tag=11;_tmp697.f1=(void*)_tmp643;_tmp697.f2=_tmp695;_tmp697;});_tmp696;});}}
_LL43D: if(_tmp61C <= (void*)3?1:*((int*)_tmp61C)!= 0)goto _LL43F;_tmp645=((struct
Cyc_Absyn_Evar_struct*)_tmp61C)->f2;_LL43E: if(_tmp645 != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp645->v);else{return t;}_LL43F: if(_tmp61C <= (void*)3?1:*((int*)
_tmp61C)!= 15)goto _LL441;_tmp646=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp61C)->f1;_LL440: {void*_tmp698=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp646);
return _tmp698 == _tmp646?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp699=
_cycalloc(sizeof(*_tmp699));_tmp699[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp69A;_tmp69A.tag=15;_tmp69A.f1=(void*)_tmp698;_tmp69A;});_tmp699;});}_LL441:
if(_tmp61C <= (void*)3?1:*((int*)_tmp61C)!= 14)goto _LL443;_tmp647=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp61C)->f1;_LL442: {void*_tmp69B=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp647);return _tmp69B == _tmp647?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp69D;_tmp69D.tag=14;_tmp69D.f1=(void*)_tmp69B;_tmp69D;});_tmp69C;});}_LL443:
if(_tmp61C <= (void*)3?1:*((int*)_tmp61C)!= 17)goto _LL445;_tmp648=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp61C)->f1;_LL444: {void*_tmp69E=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp648);return _tmp69E == _tmp648?t:(void*)({struct Cyc_Absyn_TagType_struct*
_tmp69F=_cycalloc(sizeof(*_tmp69F));_tmp69F[0]=({struct Cyc_Absyn_TagType_struct
_tmp6A0;_tmp6A0.tag=17;_tmp6A0.f1=(void*)_tmp69E;_tmp6A0;});_tmp69F;});}_LL445:
if(_tmp61C <= (void*)3?1:*((int*)_tmp61C)!= 18)goto _LL447;_LL446: goto _LL448;
_LL447: if(_tmp61C <= (void*)3?1:*((int*)_tmp61C)!= 12)goto _LL449;_LL448: goto
_LL44A;_LL449: if(_tmp61C <= (void*)3?1:*((int*)_tmp61C)!= 13)goto _LL44B;_LL44A:
goto _LL44C;_LL44B: if((int)_tmp61C != 0)goto _LL44D;_LL44C: goto _LL44E;_LL44D: if(
_tmp61C <= (void*)3?1:*((int*)_tmp61C)!= 5)goto _LL44F;_LL44E: goto _LL450;_LL44F:
if((int)_tmp61C != 1)goto _LL451;_LL450: goto _LL452;_LL451: if(_tmp61C <= (void*)3?1:*((
int*)_tmp61C)!= 6)goto _LL453;_LL452: goto _LL454;_LL453: if((int)_tmp61C != 2)goto
_LL455;_LL454: return t;_LL455: if(_tmp61C <= (void*)3?1:*((int*)_tmp61C)!= 21)goto
_LL457;_tmp649=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp61C)->f1;_LL456: {
void*_tmp6A1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp649);return _tmp6A1 == _tmp649?t:(
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));
_tmp6A2[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6A3;_tmp6A3.tag=21;_tmp6A3.f1=(
void*)_tmp6A1;_tmp6A3;});_tmp6A2;});}_LL457: if(_tmp61C <= (void*)3?1:*((int*)
_tmp61C)!= 19)goto _LL459;_tmp64A=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp61C)->f1;_LL458: {void*_tmp6A4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp64A);
return _tmp6A4 == _tmp64A?t:(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6A5=
_cycalloc(sizeof(*_tmp6A5));_tmp6A5[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6A6;
_tmp6A6.tag=19;_tmp6A6.f1=(void*)_tmp6A4;_tmp6A6;});_tmp6A5;});}_LL459: if(
_tmp61C <= (void*)3?1:*((int*)_tmp61C)!= 20)goto _LL428;_tmp64B=((struct Cyc_Absyn_JoinEff_struct*)
_tmp61C)->f1;_LL45A: {struct Cyc_List_List*_tmp6A7=Cyc_Tcutil_substs(rgn,inst,
_tmp64B);return _tmp6A7 == _tmp64B?t:(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6A9;_tmp6A9.tag=20;_tmp6A9.f1=_tmp6A7;_tmp6A9;});_tmp6A8;});}_LL428:;}static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp6AA=(void*)ts->hd;
struct Cyc_List_List*_tmp6AB=ts->tl;void*_tmp6AC=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp6AA);struct Cyc_List_List*_tmp6AD=Cyc_Tcutil_substs(rgn,inst,_tmp6AB);if(
_tmp6AA == _tmp6AC?_tmp6AB == _tmp6AD: 0)return ts;return(struct Cyc_List_List*)((
struct Cyc_List_List*)({struct Cyc_List_List*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));
_tmp6AE->hd=(void*)_tmp6AC;_tmp6AE->tl=_tmp6AD;_tmp6AE;}));}}extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp6AF=_cycalloc(
sizeof(*_tmp6AF));_tmp6AF->f1=tv;_tmp6AF->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp6B0=_cycalloc(sizeof(*_tmp6B0));_tmp6B0->v=(void*)k;_tmp6B0;}),({struct Cyc_Core_Opt*
_tmp6B1=_cycalloc(sizeof(*_tmp6B1));_tmp6B1->v=s;_tmp6B1;}));_tmp6AF;});}struct
_tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple9 _tmp6B3;struct Cyc_List_List*_tmp6B4;struct _RegionHandle*_tmp6B5;
struct _tuple9*_tmp6B2=env;_tmp6B3=*_tmp6B2;_tmp6B4=_tmp6B3.f1;_tmp6B5=_tmp6B3.f2;{
void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp6B6=_region_malloc(
_tmp6B5,sizeof(*_tmp6B6));_tmp6B6->f1=tv;_tmp6B6->f2=Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7->v=(void*)k;_tmp6B7;}),({
struct Cyc_Core_Opt*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8->v=_tmp6B4;
_tmp6B8;}));_tmp6B6;});}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2))({
struct Cyc_String_pa_struct _tmp6BD;_tmp6BD.tag=0;_tmp6BD.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_kindbound2string(k2));{struct Cyc_String_pa_struct
_tmp6BC;_tmp6BC.tag=0;_tmp6BC.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kindbound2string(
k1));{struct Cyc_String_pa_struct _tmp6BB;_tmp6BB.tag=0;_tmp6BB.f1=(struct
_tagged_arr)((struct _tagged_arr)*tv->name);{void*_tmp6B9[3]={& _tmp6BB,& _tmp6BC,&
_tmp6BD};Cyc_Tcutil_terr(loc,({const char*_tmp6BA="type variable %s is used with inconsistent kinds %s and %s";
_tag_arr(_tmp6BA,sizeof(char),_get_zero_arr_size(_tmp6BA,59));}),_tag_arr(
_tmp6B9,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))!= *((int*)
_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*_tmp6BE[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp6BF="same type variable has different identity!";_tag_arr(_tmp6BF,sizeof(
char),_get_zero_arr_size(_tmp6BF,43));}),_tag_arr(_tmp6BE,sizeof(void*),0));});}
return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();return({struct Cyc_List_List*
_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0->hd=tv;_tmp6C0->tl=tvs;_tmp6C0;});}
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*_tmp6C1[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp6C2="fast_add_free_tvar: bad identity in tv";
_tag_arr(_tmp6C2,sizeof(char),_get_zero_arr_size(_tmp6C2,39));}),_tag_arr(
_tmp6C1,sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=
tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp6C3[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp6C4="fast_add_free_tvar: bad identity in tvs2";_tag_arr(_tmp6C4,sizeof(char),
_get_zero_arr_size(_tmp6C4,41));}),_tag_arr(_tmp6C3,sizeof(void*),0));});if(*((
int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))== *((int*)
_check_null(tv->identity)))return tvs;}}return({struct Cyc_List_List*_tmp6C5=
_cycalloc(sizeof(*_tmp6C5));_tmp6C5->hd=tv;_tmp6C5->tl=tvs;_tmp6C5;});}static
struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*
tv){if(tv->identity == 0)({struct Cyc_String_pa_struct _tmp6C8;_tmp6C8.tag=0;
_tmp6C8.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string(tv));{
void*_tmp6C6[1]={& _tmp6C8};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp6C7="bound tvar id for %s is NULL";_tag_arr(
_tmp6C7,sizeof(char),_get_zero_arr_size(_tmp6C7,29));}),_tag_arr(_tmp6C6,sizeof(
void*),1));}});return({struct Cyc_List_List*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));
_tmp6C9->hd=tv;_tmp6C9->tl=tvs;_tmp6C9;});}static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(
struct Cyc_List_List*es,void*e){void*_tmp6CA=Cyc_Tcutil_compress(e);int _tmp6CB;
_LL466: if(_tmp6CA <= (void*)3?1:*((int*)_tmp6CA)!= 0)goto _LL468;_tmp6CB=((struct
Cyc_Absyn_Evar_struct*)_tmp6CA)->f3;_LL467:{struct Cyc_List_List*es2=es;for(0;es2
!= 0;es2=es2->tl){void*_tmp6CC=Cyc_Tcutil_compress((void*)es2->hd);int _tmp6CD;
_LL46B: if(_tmp6CC <= (void*)3?1:*((int*)_tmp6CC)!= 0)goto _LL46D;_tmp6CD=((struct
Cyc_Absyn_Evar_struct*)_tmp6CC)->f3;_LL46C: if(_tmp6CB == _tmp6CD)return es;goto
_LL46A;_LL46D:;_LL46E: goto _LL46A;_LL46A:;}}return({struct Cyc_List_List*_tmp6CE=
_cycalloc(sizeof(*_tmp6CE));_tmp6CE->hd=(void*)e;_tmp6CE->tl=es;_tmp6CE;});
_LL468:;_LL469: return es;_LL465:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;
tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))== *((int*)
_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(!
present)r=({struct Cyc_List_List*_tmp6CF=_cycalloc(sizeof(*_tmp6CF));_tmp6CF->hd=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6CF->tl=r;_tmp6CF;});}r=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)width))({struct Cyc_String_pa_struct _tmp6D2;_tmp6D2.tag=
0;_tmp6D2.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{void*_tmp6D0[1]={&
_tmp6D2};Cyc_Tcutil_terr(loc,({const char*_tmp6D1="bitfield %s does not have constant width";
_tag_arr(_tmp6D1,sizeof(char),_get_zero_arr_size(_tmp6D1,41));}),_tag_arr(
_tmp6D0,sizeof(void*),1));}});else{unsigned int _tmp6D4;int _tmp6D5;struct _tuple7
_tmp6D3=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)width);_tmp6D4=
_tmp6D3.f1;_tmp6D5=_tmp6D3.f2;if(!_tmp6D5)({void*_tmp6D6[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp6D7="bitfield width cannot use sizeof or offsetof";_tag_arr(
_tmp6D7,sizeof(char),_get_zero_arr_size(_tmp6D7,45));}),_tag_arr(_tmp6D6,sizeof(
void*),0));});w=_tmp6D4;}{void*_tmp6D8=Cyc_Tcutil_compress(field_typ);void*
_tmp6D9;_LL470: if(_tmp6D8 <= (void*)3?1:*((int*)_tmp6D8)!= 5)goto _LL472;_tmp6D9=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp6D8)->f2;_LL471:{void*_tmp6DA=
_tmp6D9;_LL475: if((int)_tmp6DA != 0)goto _LL477;_LL476: if(w > 8)({void*_tmp6DB[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp6DC="bitfield larger than type";_tag_arr(
_tmp6DC,sizeof(char),_get_zero_arr_size(_tmp6DC,26));}),_tag_arr(_tmp6DB,sizeof(
void*),0));});goto _LL474;_LL477: if((int)_tmp6DA != 1)goto _LL479;_LL478: if(w > 16)({
void*_tmp6DD[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6DE="bitfield larger than type";
_tag_arr(_tmp6DE,sizeof(char),_get_zero_arr_size(_tmp6DE,26));}),_tag_arr(
_tmp6DD,sizeof(void*),0));});goto _LL474;_LL479: if((int)_tmp6DA != 2)goto _LL47B;
_LL47A: if(w > 32)({void*_tmp6DF[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6E0="bitfield larger than type";
_tag_arr(_tmp6E0,sizeof(char),_get_zero_arr_size(_tmp6E0,26));}),_tag_arr(
_tmp6DF,sizeof(void*),0));});goto _LL474;_LL47B: if((int)_tmp6DA != 3)goto _LL474;
_LL47C: if(w > 64)({void*_tmp6E1[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6E2="bitfield larger than type";
_tag_arr(_tmp6E2,sizeof(char),_get_zero_arr_size(_tmp6E2,26));}),_tag_arr(
_tmp6E1,sizeof(void*),0));});goto _LL474;_LL474:;}goto _LL46F;_LL472:;_LL473:({
struct Cyc_String_pa_struct _tmp6E6;_tmp6E6.tag=0;_tmp6E6.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(field_typ));{struct Cyc_String_pa_struct
_tmp6E5;_tmp6E5.tag=0;_tmp6E5.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
void*_tmp6E3[2]={& _tmp6E5,& _tmp6E6};Cyc_Tcutil_terr(loc,({const char*_tmp6E4="bitfield %s must have integral type but has type %s";
_tag_arr(_tmp6E4,sizeof(char),_get_zero_arr_size(_tmp6E4,52));}),_tag_arr(
_tmp6E3,sizeof(void*),2));}}});goto _LL46F;_LL46F:;}}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _tagged_arr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp6E7=(void*)atts->hd;_LL47E: if((int)
_tmp6E7 != 5)goto _LL480;_LL47F: continue;_LL480: if(_tmp6E7 <= (void*)17?1:*((int*)
_tmp6E7)!= 1)goto _LL482;_LL481: continue;_LL482:;_LL483:({struct Cyc_String_pa_struct
_tmp6EB;_tmp6EB.tag=0;_tmp6EB.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
struct Cyc_String_pa_struct _tmp6EA;_tmp6EA.tag=0;_tmp6EA.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp6E8[2]={&
_tmp6EA,& _tmp6EB};Cyc_Tcutil_terr(loc,({const char*_tmp6E9="bad attribute %s on member %s";
_tag_arr(_tmp6E9,sizeof(char),_get_zero_arr_size(_tmp6E9,30));}),_tag_arr(
_tmp6E8,sizeof(void*),2));}}});_LL47D:;}}struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};struct _tuple17{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t){{void*_tmp6EC=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmp6ED;struct Cyc_Core_Opt**_tmp6EE;struct Cyc_Core_Opt*
_tmp6EF;struct Cyc_Core_Opt**_tmp6F0;struct Cyc_Absyn_Tvar*_tmp6F1;struct Cyc_List_List*
_tmp6F2;struct _tuple1*_tmp6F3;struct Cyc_Absyn_Enumdecl*_tmp6F4;struct Cyc_Absyn_Enumdecl**
_tmp6F5;struct Cyc_Absyn_TunionInfo _tmp6F6;void*_tmp6F7;void**_tmp6F8;struct Cyc_List_List*
_tmp6F9;struct Cyc_List_List**_tmp6FA;void*_tmp6FB;struct Cyc_Absyn_TunionFieldInfo
_tmp6FC;void*_tmp6FD;void**_tmp6FE;struct Cyc_List_List*_tmp6FF;struct Cyc_Absyn_PtrInfo
_tmp700;void*_tmp701;struct Cyc_Absyn_Tqual _tmp702;struct Cyc_Absyn_PtrAtts _tmp703;
void*_tmp704;struct Cyc_Absyn_Conref*_tmp705;struct Cyc_Absyn_Conref*_tmp706;
struct Cyc_Absyn_Conref*_tmp707;void*_tmp708;void*_tmp709;struct Cyc_Absyn_ArrayInfo
_tmp70A;void*_tmp70B;struct Cyc_Absyn_Tqual _tmp70C;struct Cyc_Absyn_Exp*_tmp70D;
struct Cyc_Absyn_Exp**_tmp70E;struct Cyc_Absyn_Conref*_tmp70F;struct Cyc_Absyn_FnInfo
_tmp710;struct Cyc_List_List*_tmp711;struct Cyc_List_List**_tmp712;struct Cyc_Core_Opt*
_tmp713;struct Cyc_Core_Opt**_tmp714;void*_tmp715;struct Cyc_List_List*_tmp716;int
_tmp717;struct Cyc_Absyn_VarargInfo*_tmp718;struct Cyc_List_List*_tmp719;struct Cyc_List_List*
_tmp71A;struct Cyc_List_List*_tmp71B;void*_tmp71C;struct Cyc_List_List*_tmp71D;
struct Cyc_Absyn_AggrInfo _tmp71E;void*_tmp71F;void**_tmp720;struct Cyc_List_List*
_tmp721;struct Cyc_List_List**_tmp722;struct _tuple1*_tmp723;struct Cyc_List_List*
_tmp724;struct Cyc_List_List**_tmp725;struct Cyc_Absyn_Typedefdecl*_tmp726;struct
Cyc_Absyn_Typedefdecl**_tmp727;void**_tmp728;void***_tmp729;void*_tmp72A;void*
_tmp72B;void*_tmp72C;struct Cyc_List_List*_tmp72D;_LL485: if((int)_tmp6EC != 0)goto
_LL487;_LL486: goto _LL484;_LL487: if(_tmp6EC <= (void*)3?1:*((int*)_tmp6EC)!= 0)
goto _LL489;_tmp6ED=((struct Cyc_Absyn_Evar_struct*)_tmp6EC)->f1;_tmp6EE=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp6EC)->f1;_tmp6EF=((struct Cyc_Absyn_Evar_struct*)
_tmp6EC)->f2;_tmp6F0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp6EC)->f2;_LL488: if(*_tmp6EE == 0)*_tmp6EE=({struct Cyc_Core_Opt*_tmp72E=
_cycalloc(sizeof(*_tmp72E));_tmp72E->v=(void*)expected_kind;_tmp72E;});if((
cvtenv.fn_result?cvtenv.generalize_evars: 0)?expected_kind == (void*)3: 0)*_tmp6F0=({
struct Cyc_Core_Opt*_tmp72F=_cycalloc(sizeof(*_tmp72F));_tmp72F->v=(void*)((void*)
2);_tmp72F;});else{if(cvtenv.generalize_evars){struct Cyc_Absyn_Tvar*_tmp730=Cyc_Tcutil_new_tvar((
void*)({struct Cyc_Absyn_Less_kb_struct*_tmp734=_cycalloc(sizeof(*_tmp734));
_tmp734[0]=({struct Cyc_Absyn_Less_kb_struct _tmp735;_tmp735.tag=2;_tmp735.f1=0;
_tmp735.f2=(void*)expected_kind;_tmp735;});_tmp734;}));*_tmp6F0=({struct Cyc_Core_Opt*
_tmp731=_cycalloc(sizeof(*_tmp731));_tmp731->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732[0]=({struct Cyc_Absyn_VarType_struct
_tmp733;_tmp733.tag=1;_tmp733.f1=_tmp730;_tmp733;});_tmp732;}));_tmp731;});
_tmp6F1=_tmp730;goto _LL48A;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.free_evars,t);}}goto _LL484;_LL489: if(_tmp6EC <= (void*)3?1:*((int*)_tmp6EC)
!= 1)goto _LL48B;_tmp6F1=((struct Cyc_Absyn_VarType_struct*)_tmp6EC)->f1;_LL48A:{
void*_tmp736=Cyc_Absyn_compress_kb((void*)_tmp6F1->kind);struct Cyc_Core_Opt*
_tmp737;struct Cyc_Core_Opt**_tmp738;_LL4B8: if(*((int*)_tmp736)!= 1)goto _LL4BA;
_tmp737=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp736)->f1;_tmp738=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp736)->f1;_LL4B9:*_tmp738=({struct Cyc_Core_Opt*
_tmp739=_cycalloc(sizeof(*_tmp739));_tmp739->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp73A=_cycalloc(sizeof(*_tmp73A));_tmp73A[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp73B;_tmp73B.tag=2;_tmp73B.f1=0;_tmp73B.f2=(void*)expected_kind;_tmp73B;});
_tmp73A;}));_tmp739;});goto _LL4B7;_LL4BA:;_LL4BB: goto _LL4B7;_LL4B7:;}cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp6F1);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp6F1);goto _LL484;_LL48B: if(_tmp6EC <= (void*)3?1:*((int*)
_tmp6EC)!= 13)goto _LL48D;_tmp6F2=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp6EC)->f1;
_LL48C: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp73C=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp73C;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp6F2 != 0;_tmp6F2=_tmp6F2->tl){struct Cyc_Absyn_Enumfield*_tmp73D=(
struct Cyc_Absyn_Enumfield*)_tmp6F2->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_strptrcmp,prev_fields,(*_tmp73D->name).f2))({struct Cyc_String_pa_struct
_tmp740;_tmp740.tag=0;_tmp740.f1=(struct _tagged_arr)((struct _tagged_arr)*(*
_tmp73D->name).f2);{void*_tmp73E[1]={& _tmp740};Cyc_Tcutil_terr(_tmp73D->loc,({
const char*_tmp73F="duplicate enum field name %s";_tag_arr(_tmp73F,sizeof(char),
_get_zero_arr_size(_tmp73F,29));}),_tag_arr(_tmp73E,sizeof(void*),1));}});else{
prev_fields=({struct Cyc_List_List*_tmp741=_region_malloc(uprev_rgn,sizeof(*
_tmp741));_tmp741->hd=(*_tmp73D->name).f2;_tmp741->tl=prev_fields;_tmp741;});}
if(_tmp73D->tag == 0)_tmp73D->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp73D->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp73D->tag)))({struct Cyc_String_pa_struct _tmp744;_tmp744.tag=0;
_tmp744.f1=(struct _tagged_arr)((struct _tagged_arr)*(*_tmp73D->name).f2);{void*
_tmp742[1]={& _tmp744};Cyc_Tcutil_terr(loc,({const char*_tmp743="enum field %s: expression is not constant";
_tag_arr(_tmp743,sizeof(char),_get_zero_arr_size(_tmp743,42));}),_tag_arr(
_tmp742,sizeof(void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp73D->tag))).f1;tag_count=t1 + 1;(*_tmp73D->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp746;_tmp746.tag=1;_tmp746.f1=te->ns;_tmp746;});
_tmp745;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple17*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp73D->name).f2,({
struct _tuple17*_tmp747=_cycalloc(sizeof(*_tmp747));_tmp747->f1=(void*)({struct
Cyc_Tcenv_AnonEnumRes_struct*_tmp748=_cycalloc(sizeof(*_tmp748));_tmp748[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmp749;_tmp749.tag=4;_tmp749.f1=(void*)t;
_tmp749.f2=_tmp73D;_tmp749;});_tmp748;});_tmp747->f2=1;_tmp747;}));}}};
_pop_region(uprev_rgn);}goto _LL484;}_LL48D: if(_tmp6EC <= (void*)3?1:*((int*)
_tmp6EC)!= 12)goto _LL48F;_tmp6F3=((struct Cyc_Absyn_EnumType_struct*)_tmp6EC)->f1;
_tmp6F4=((struct Cyc_Absyn_EnumType_struct*)_tmp6EC)->f2;_tmp6F5=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp6EC)->f2;_LL48E: if(*_tmp6F5 == 0?1:((struct
Cyc_Absyn_Enumdecl*)_check_null(*_tmp6F5))->fields == 0){struct _handler_cons
_tmp74A;_push_handler(& _tmp74A);{int _tmp74C=0;if(setjmp(_tmp74A.handler))_tmp74C=
1;if(!_tmp74C){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,
_tmp6F3);*_tmp6F5=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{void*
_tmp74B=(void*)_exn_thrown;void*_tmp74E=_tmp74B;_LL4BD: if(_tmp74E != Cyc_Dict_Absent)
goto _LL4BF;_LL4BE: {struct Cyc_Tcenv_Genv*_tmp74F=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp750=({struct Cyc_Absyn_Enumdecl*_tmp751=_cycalloc(
sizeof(*_tmp751));_tmp751->sc=(void*)((void*)3);_tmp751->name=_tmp6F3;_tmp751->fields=
0;_tmp751;});Cyc_Tc_tcEnumdecl(te,_tmp74F,loc,_tmp750);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp6F3);*_tmp6F5=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL4BC;}}_LL4BF:;_LL4C0:(void)_throw(_tmp74E);_LL4BC:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp6F5);*_tmp6F3=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL484;}_LL48F: if(_tmp6EC <= (void*)3?1:*((
int*)_tmp6EC)!= 2)goto _LL491;_tmp6F6=((struct Cyc_Absyn_TunionType_struct*)
_tmp6EC)->f1;_tmp6F7=(void*)_tmp6F6.tunion_info;_tmp6F8=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp6EC)->f1).tunion_info;_tmp6F9=_tmp6F6.targs;_tmp6FA=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp6EC)->f1).targs;_tmp6FB=(void*)_tmp6F6.rgn;
_LL490: {struct Cyc_List_List*_tmp752=*_tmp6FA;{void*_tmp753=*_tmp6F8;struct Cyc_Absyn_UnknownTunionInfo
_tmp754;struct _tuple1*_tmp755;int _tmp756;struct Cyc_Absyn_Tuniondecl**_tmp757;
struct Cyc_Absyn_Tuniondecl*_tmp758;_LL4C2: if(*((int*)_tmp753)!= 0)goto _LL4C4;
_tmp754=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp753)->f1;_tmp755=_tmp754.name;
_tmp756=_tmp754.is_xtunion;_LL4C3: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp759;_push_handler(& _tmp759);{int _tmp75B=0;if(setjmp(_tmp759.handler))
_tmp75B=1;if(!_tmp75B){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp755);;
_pop_handler();}else{void*_tmp75A=(void*)_exn_thrown;void*_tmp75D=_tmp75A;_LL4C7:
if(_tmp75D != Cyc_Dict_Absent)goto _LL4C9;_LL4C8: {struct Cyc_Tcenv_Genv*_tmp75E=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp75F=({struct Cyc_Absyn_Tuniondecl*
_tmp766=_cycalloc(sizeof(*_tmp766));_tmp766->sc=(void*)((void*)3);_tmp766->name=
_tmp755;_tmp766->tvs=0;_tmp766->fields=0;_tmp766->is_xtunion=_tmp756;_tmp766;});
Cyc_Tc_tcTuniondecl(te,_tmp75E,loc,_tmp75F);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp755);if(_tmp752 != 0){({struct Cyc_String_pa_struct _tmp763;_tmp763.tag=0;
_tmp763.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp755));{struct Cyc_String_pa_struct _tmp762;_tmp762.tag=0;_tmp762.f1=(struct
_tagged_arr)(_tmp756?(struct _tagged_arr)((struct _tagged_arr)({const char*_tmp764="xtunion";
_tag_arr(_tmp764,sizeof(char),_get_zero_arr_size(_tmp764,8));})):(struct
_tagged_arr)({const char*_tmp765="tunion";_tag_arr(_tmp765,sizeof(char),
_get_zero_arr_size(_tmp765,7));}));{void*_tmp760[2]={& _tmp762,& _tmp763};Cyc_Tcutil_terr(
loc,({const char*_tmp761="declare parameterized %s %s before using";_tag_arr(
_tmp761,sizeof(char),_get_zero_arr_size(_tmp761,41));}),_tag_arr(_tmp760,sizeof(
void*),2));}}});return cvtenv;}goto _LL4C6;}_LL4C9:;_LL4CA:(void)_throw(_tmp75D);
_LL4C6:;}}}if((*tudp)->is_xtunion != _tmp756)({struct Cyc_String_pa_struct _tmp769;
_tmp769.tag=0;_tmp769.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp755));{void*_tmp767[1]={& _tmp769};Cyc_Tcutil_terr(loc,({const char*_tmp768="[x]tunion is different from type declaration %s";
_tag_arr(_tmp768,sizeof(char),_get_zero_arr_size(_tmp768,48));}),_tag_arr(
_tmp767,sizeof(void*),1));}});*_tmp6F8=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp76A=_cycalloc(sizeof(*_tmp76A));_tmp76A[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp76B;_tmp76B.tag=1;_tmp76B.f1=tudp;_tmp76B;});_tmp76A;});_tmp758=*tudp;goto
_LL4C5;}_LL4C4: if(*((int*)_tmp753)!= 1)goto _LL4C1;_tmp757=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp753)->f1;_tmp758=*_tmp757;_LL4C5: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp6FB);{struct Cyc_List_List*tvs=_tmp758->tvs;for(0;_tmp752 != 0?
tvs != 0: 0;(_tmp752=_tmp752->tl,tvs=tvs->tl)){void*t1=(void*)_tmp752->hd;void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1);}if(_tmp752 != 0)({struct Cyc_String_pa_struct _tmp76E;_tmp76E.tag=
0;_tmp76E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp758->name));{void*_tmp76C[1]={& _tmp76E};Cyc_Tcutil_terr(loc,({const char*
_tmp76D="too many type arguments for tunion %s";_tag_arr(_tmp76D,sizeof(char),
_get_zero_arr_size(_tmp76D,38));}),_tag_arr(_tmp76C,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp76F=_cycalloc(sizeof(*_tmp76F));_tmp76F->hd=(void*)e;_tmp76F->tl=
hidden_ts;_tmp76F;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*
_tmp6FA=Cyc_List_imp_append(*_tmp6FA,Cyc_List_imp_rev(hidden_ts));}goto _LL4C1;}
_LL4C1:;}goto _LL484;}_LL491: if(_tmp6EC <= (void*)3?1:*((int*)_tmp6EC)!= 3)goto
_LL493;_tmp6FC=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp6EC)->f1;_tmp6FD=(
void*)_tmp6FC.field_info;_tmp6FE=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp6EC)->f1).field_info;_tmp6FF=_tmp6FC.targs;_LL492:{void*_tmp770=*_tmp6FE;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp771;struct _tuple1*_tmp772;struct
_tuple1*_tmp773;int _tmp774;struct Cyc_Absyn_Tuniondecl*_tmp775;struct Cyc_Absyn_Tunionfield*
_tmp776;_LL4CC: if(*((int*)_tmp770)!= 0)goto _LL4CE;_tmp771=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp770)->f1;_tmp772=_tmp771.tunion_name;_tmp773=_tmp771.field_name;_tmp774=
_tmp771.is_xtunion;_LL4CD: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp777;_push_handler(& _tmp777);{int _tmp779=0;if(setjmp(
_tmp777.handler))_tmp779=1;if(!_tmp779){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp772);;_pop_handler();}else{void*_tmp778=(void*)_exn_thrown;void*_tmp77B=
_tmp778;_LL4D1: if(_tmp77B != Cyc_Dict_Absent)goto _LL4D3;_LL4D2:({struct Cyc_String_pa_struct
_tmp77E;_tmp77E.tag=0;_tmp77E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp772));{void*_tmp77C[1]={& _tmp77E};Cyc_Tcutil_terr(loc,({const char*_tmp77D="unbound type tunion %s";
_tag_arr(_tmp77D,sizeof(char),_get_zero_arr_size(_tmp77D,23));}),_tag_arr(
_tmp77C,sizeof(void*),1));}});return cvtenv;_LL4D3:;_LL4D4:(void)_throw(_tmp77B);
_LL4D0:;}}}{struct _handler_cons _tmp77F;_push_handler(& _tmp77F);{int _tmp781=0;if(
setjmp(_tmp77F.handler))_tmp781=1;if(!_tmp781){{void*_tmp782=Cyc_Tcenv_lookup_ordinary(
te,loc,_tmp773);struct Cyc_Absyn_Tuniondecl*_tmp783;struct Cyc_Absyn_Tunionfield*
_tmp784;_LL4D6: if(*((int*)_tmp782)!= 2)goto _LL4D8;_tmp783=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp782)->f1;_tmp784=((struct Cyc_Tcenv_TunionRes_struct*)_tmp782)->f2;_LL4D7: tuf=
_tmp784;tud=_tmp783;if(tud->is_xtunion != _tmp774)({struct Cyc_String_pa_struct
_tmp787;_tmp787.tag=0;_tmp787.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp772));{void*_tmp785[1]={& _tmp787};Cyc_Tcutil_terr(loc,({const char*_tmp786="[x]tunion is different from type declaration %s";
_tag_arr(_tmp786,sizeof(char),_get_zero_arr_size(_tmp786,48));}),_tag_arr(
_tmp785,sizeof(void*),1));}});goto _LL4D5;_LL4D8:;_LL4D9:({struct Cyc_String_pa_struct
_tmp78B;_tmp78B.tag=0;_tmp78B.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp772));{struct Cyc_String_pa_struct _tmp78A;_tmp78A.tag=0;_tmp78A.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp773));{void*_tmp788[
2]={& _tmp78A,& _tmp78B};Cyc_Tcutil_terr(loc,({const char*_tmp789="unbound field %s in type tunion %s";
_tag_arr(_tmp789,sizeof(char),_get_zero_arr_size(_tmp789,35));}),_tag_arr(
_tmp788,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv _tmp78C=cvtenv;
_npop_handler(0);return _tmp78C;}_LL4D5:;};_pop_handler();}else{void*_tmp780=(
void*)_exn_thrown;void*_tmp78E=_tmp780;_LL4DB: if(_tmp78E != Cyc_Dict_Absent)goto
_LL4DD;_LL4DC:({struct Cyc_String_pa_struct _tmp792;_tmp792.tag=0;_tmp792.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp772));{struct
Cyc_String_pa_struct _tmp791;_tmp791.tag=0;_tmp791.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp773));{void*_tmp78F[2]={& _tmp791,&
_tmp792};Cyc_Tcutil_terr(loc,({const char*_tmp790="unbound field %s in type tunion %s";
_tag_arr(_tmp790,sizeof(char),_get_zero_arr_size(_tmp790,35));}),_tag_arr(
_tmp78F,sizeof(void*),2));}}});return cvtenv;_LL4DD:;_LL4DE:(void)_throw(_tmp78E);
_LL4DA:;}}}*_tmp6FE=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmp793=
_cycalloc(sizeof(*_tmp793));_tmp793[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp794;_tmp794.tag=1;_tmp794.f1=tud;_tmp794.f2=tuf;_tmp794;});_tmp793;});
_tmp775=tud;_tmp776=tuf;goto _LL4CF;}_LL4CE: if(*((int*)_tmp770)!= 1)goto _LL4CB;
_tmp775=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp770)->f1;_tmp776=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp770)->f2;_LL4CF: {struct Cyc_List_List*tvs=
_tmp775->tvs;for(0;_tmp6FF != 0?tvs != 0: 0;(_tmp6FF=_tmp6FF->tl,tvs=tvs->tl)){void*
t1=(void*)_tmp6FF->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1);}if(_tmp6FF != 0)({
struct Cyc_String_pa_struct _tmp798;_tmp798.tag=0;_tmp798.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp776->name));{struct Cyc_String_pa_struct
_tmp797;_tmp797.tag=0;_tmp797.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp775->name));{void*_tmp795[2]={& _tmp797,& _tmp798};Cyc_Tcutil_terr(loc,({const
char*_tmp796="too many type arguments for tunion %s.%s";_tag_arr(_tmp796,sizeof(
char),_get_zero_arr_size(_tmp796,41));}),_tag_arr(_tmp795,sizeof(void*),2));}}});
if(tvs != 0)({struct Cyc_String_pa_struct _tmp79C;_tmp79C.tag=0;_tmp79C.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp776->name));{struct
Cyc_String_pa_struct _tmp79B;_tmp79B.tag=0;_tmp79B.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp775->name));{void*_tmp799[2]={& _tmp79B,&
_tmp79C};Cyc_Tcutil_terr(loc,({const char*_tmp79A="too few type arguments for tunion %s.%s";
_tag_arr(_tmp79A,sizeof(char),_get_zero_arr_size(_tmp79A,40));}),_tag_arr(
_tmp799,sizeof(void*),2));}}});goto _LL4CB;}_LL4CB:;}goto _LL484;_LL493: if(_tmp6EC
<= (void*)3?1:*((int*)_tmp6EC)!= 4)goto _LL495;_tmp700=((struct Cyc_Absyn_PointerType_struct*)
_tmp6EC)->f1;_tmp701=(void*)_tmp700.elt_typ;_tmp702=_tmp700.elt_tq;_tmp703=
_tmp700.ptr_atts;_tmp704=(void*)_tmp703.rgn;_tmp705=_tmp703.nullable;_tmp706=
_tmp703.bounds;_tmp707=_tmp703.zero_term;_LL494: {int is_zero_terminated;cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp701);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp704);{void*_tmp79D=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp707))->v;int _tmp79E;
_LL4E0: if((int)_tmp79D != 0)goto _LL4E2;_LL4E1:{void*_tmp79F=Cyc_Tcutil_compress(
_tmp701);void*_tmp7A0;_LL4E7: if(_tmp79F <= (void*)3?1:*((int*)_tmp79F)!= 5)goto
_LL4E9;_tmp7A0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp79F)->f2;if((int)
_tmp7A0 != 0)goto _LL4E9;_LL4E8:((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp707,
Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL4E6;_LL4E9:;_LL4EA:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp707,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL4E6;
_LL4E6:;}goto _LL4DF;_LL4E2: if(_tmp79D <= (void*)1?1:*((int*)_tmp79D)!= 0)goto
_LL4E4;_tmp79E=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp79D)->f1;if(_tmp79E
!= 1)goto _LL4E4;_LL4E3: if(!Cyc_Tcutil_admits_zero(_tmp701))({struct Cyc_String_pa_struct
_tmp7A3;_tmp7A3.tag=0;_tmp7A3.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp701));{void*_tmp7A1[1]={& _tmp7A3};Cyc_Tcutil_terr(loc,({const char*_tmp7A2="cannot have a pointer to zero-terminated %s elements";
_tag_arr(_tmp7A2,sizeof(char),_get_zero_arr_size(_tmp7A2,53));}),_tag_arr(
_tmp7A1,sizeof(void*),1));}});is_zero_terminated=1;goto _LL4DF;_LL4E4:;_LL4E5:
is_zero_terminated=0;goto _LL4DF;_LL4DF:;}{void*_tmp7A4=(void*)(Cyc_Absyn_compress_conref(
_tmp706))->v;void*_tmp7A5;void*_tmp7A6;struct Cyc_Absyn_Exp*_tmp7A7;void*_tmp7A8;
void*_tmp7A9;_LL4EC: if((int)_tmp7A4 != 0)goto _LL4EE;_LL4ED: goto _LL4EF;_LL4EE: if(
_tmp7A4 <= (void*)1?1:*((int*)_tmp7A4)!= 0)goto _LL4F0;_tmp7A5=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7A4)->f1;if((int)_tmp7A5 != 0)goto _LL4F0;_LL4EF: goto _LL4EB;_LL4F0: if(_tmp7A4
<= (void*)1?1:*((int*)_tmp7A4)!= 0)goto _LL4F2;_tmp7A6=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7A4)->f1;if(_tmp7A6 <= (void*)1?1:*((int*)_tmp7A6)!= 0)goto _LL4F2;_tmp7A7=((
struct Cyc_Absyn_Upper_b_struct*)_tmp7A6)->f1;_LL4F1: Cyc_Tcexp_tcExp(te,0,_tmp7A7);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7A7))({void*_tmp7AA[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp7AB="pointer bounds expression is not an unsigned int";
_tag_arr(_tmp7AB,sizeof(char),_get_zero_arr_size(_tmp7AB,49));}),_tag_arr(
_tmp7AA,sizeof(void*),0));});{unsigned int _tmp7AD;int _tmp7AE;struct _tuple7
_tmp7AC=Cyc_Evexp_eval_const_uint_exp(_tmp7A7);_tmp7AD=_tmp7AC.f1;_tmp7AE=
_tmp7AC.f2;if(is_zero_terminated?!_tmp7AE?1: _tmp7AD < 1: 0)({void*_tmp7AF[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp7B0="zero-terminated pointer cannot point to empty sequence";
_tag_arr(_tmp7B0,sizeof(char),_get_zero_arr_size(_tmp7B0,55));}),_tag_arr(
_tmp7AF,sizeof(void*),0));});goto _LL4EB;}_LL4F2: if(_tmp7A4 <= (void*)1?1:*((int*)
_tmp7A4)!= 0)goto _LL4F4;_tmp7A8=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7A4)->f1;if(_tmp7A8 <= (void*)1?1:*((int*)_tmp7A8)!= 1)goto _LL4F4;_tmp7A9=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp7A8)->f1;_LL4F3: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)5,_tmp7A9);goto _LL4EB;_LL4F4: if(_tmp7A4 <= (void*)1?1:*((int*)
_tmp7A4)!= 1)goto _LL4EB;_LL4F5:({void*_tmp7B1[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp7B2="forward constraint";
_tag_arr(_tmp7B2,sizeof(char),_get_zero_arr_size(_tmp7B2,19));}),_tag_arr(
_tmp7B1,sizeof(void*),0));});_LL4EB:;}goto _LL484;}_LL495: if(_tmp6EC <= (void*)3?1:*((
int*)_tmp6EC)!= 17)goto _LL497;_tmp708=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp6EC)->f1;_LL496: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp708);goto _LL484;_LL497: if(_tmp6EC <= (void*)3?1:*((int*)_tmp6EC)!= 14)goto
_LL499;_tmp709=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp6EC)->f1;_LL498:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp709);goto _LL484;
_LL499: if(_tmp6EC <= (void*)3?1:*((int*)_tmp6EC)!= 5)goto _LL49B;_LL49A: goto _LL49C;
_LL49B: if((int)_tmp6EC != 1)goto _LL49D;_LL49C: goto _LL49E;_LL49D: if(_tmp6EC <= (
void*)3?1:*((int*)_tmp6EC)!= 6)goto _LL49F;_LL49E: goto _LL484;_LL49F: if(_tmp6EC <= (
void*)3?1:*((int*)_tmp6EC)!= 7)goto _LL4A1;_tmp70A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp6EC)->f1;_tmp70B=(void*)_tmp70A.elt_type;_tmp70C=_tmp70A.tq;_tmp70D=_tmp70A.num_elts;
_tmp70E=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp6EC)->f1).num_elts;
_tmp70F=_tmp70A.zero_term;_LL4A0: {struct Cyc_Absyn_Exp*_tmp7B3=*_tmp70E;cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,_tmp70B);{int
is_zero_terminated;{void*_tmp7B4=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmp70F))->v;int _tmp7B5;_LL4F7: if((int)_tmp7B4 != 0)
goto _LL4F9;_LL4F8:((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp70F,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL4F6;_LL4F9: if(_tmp7B4 <= (void*)1?1:*((int*)_tmp7B4)
!= 0)goto _LL4FB;_tmp7B5=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7B4)->f1;
if(_tmp7B5 != 1)goto _LL4FB;_LL4FA: if(!Cyc_Tcutil_admits_zero(_tmp70B))({struct Cyc_String_pa_struct
_tmp7B8;_tmp7B8.tag=0;_tmp7B8.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp70B));{void*_tmp7B6[1]={& _tmp7B8};Cyc_Tcutil_terr(loc,({const char*_tmp7B7="cannot have a zero-terminated array of %s elements";
_tag_arr(_tmp7B7,sizeof(char),_get_zero_arr_size(_tmp7B7,51));}),_tag_arr(
_tmp7B6,sizeof(void*),1));}});is_zero_terminated=1;goto _LL4F6;_LL4FB:;_LL4FC:
is_zero_terminated=0;goto _LL4F6;_LL4F6:;}if(_tmp7B3 == 0){if(is_zero_terminated)*
_tmp70E=(_tmp7B3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{({void*
_tmp7B9[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp7BA="array bound defaults to 0 here";
_tag_arr(_tmp7BA,sizeof(char),_get_zero_arr_size(_tmp7BA,31));}),_tag_arr(
_tmp7B9,sizeof(void*),0));});*_tmp70E=(_tmp7B3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
0,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp7B3);if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_tmp7B3))({void*_tmp7BB[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp7BC="array bounds expression is not constant";_tag_arr(_tmp7BC,
sizeof(char),_get_zero_arr_size(_tmp7BC,40));}),_tag_arr(_tmp7BB,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmp7B3))({void*_tmp7BD[
0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7BE="array bounds expression is not an unsigned int";
_tag_arr(_tmp7BE,sizeof(char),_get_zero_arr_size(_tmp7BE,47));}),_tag_arr(
_tmp7BD,sizeof(void*),0));});{unsigned int _tmp7C0;int _tmp7C1;struct _tuple7
_tmp7BF=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp7B3);_tmp7C0=
_tmp7BF.f1;_tmp7C1=_tmp7BF.f2;if((is_zero_terminated?_tmp7C1: 0)?_tmp7C0 < 1: 0)({
void*_tmp7C2[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp7C3="zero terminated array cannot have zero elements";
_tag_arr(_tmp7C3,sizeof(char),_get_zero_arr_size(_tmp7C3,48));}),_tag_arr(
_tmp7C2,sizeof(void*),0));});goto _LL484;}}}_LL4A1: if(_tmp6EC <= (void*)3?1:*((int*)
_tmp6EC)!= 8)goto _LL4A3;_tmp710=((struct Cyc_Absyn_FnType_struct*)_tmp6EC)->f1;
_tmp711=_tmp710.tvars;_tmp712=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp6EC)->f1).tvars;_tmp713=_tmp710.effect;_tmp714=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp6EC)->f1).effect;_tmp715=(void*)_tmp710.ret_typ;
_tmp716=_tmp710.args;_tmp717=_tmp710.c_varargs;_tmp718=_tmp710.cyc_varargs;
_tmp719=_tmp710.rgn_po;_tmp71A=_tmp710.attributes;_LL4A2: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp71A != 0;_tmp71A=_tmp71A->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp71A->hd))({struct Cyc_String_pa_struct _tmp7C6;
_tmp7C6.tag=0;_tmp7C6.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp71A->hd));{void*_tmp7C4[1]={& _tmp7C6};Cyc_Tcutil_terr(loc,({const char*
_tmp7C5="bad function type attribute %s";_tag_arr(_tmp7C5,sizeof(char),
_get_zero_arr_size(_tmp7C5,31));}),_tag_arr(_tmp7C4,sizeof(void*),1));}});{void*
_tmp7C7=(void*)_tmp71A->hd;void*_tmp7C8;int _tmp7C9;int _tmp7CA;_LL4FE: if((int)
_tmp7C7 != 0)goto _LL500;_LL4FF: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto
_LL4FD;_LL500: if((int)_tmp7C7 != 1)goto _LL502;_LL501: if(!seen_cdecl){seen_cdecl=1;
++ num_convs;}goto _LL4FD;_LL502: if((int)_tmp7C7 != 2)goto _LL504;_LL503: if(!
seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL4FD;_LL504: if(_tmp7C7 <= (void*)
17?1:*((int*)_tmp7C7)!= 3)goto _LL506;_tmp7C8=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp7C7)->f1;_tmp7C9=((struct Cyc_Absyn_Format_att_struct*)_tmp7C7)->f2;_tmp7CA=((
struct Cyc_Absyn_Format_att_struct*)_tmp7C7)->f3;_LL505: if(!seen_format){
seen_format=1;ft=_tmp7C8;fmt_desc_arg=_tmp7C9;fmt_arg_start=_tmp7CA;}else{({void*
_tmp7CB[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7CC="function can't have multiple format attributes";
_tag_arr(_tmp7CC,sizeof(char),_get_zero_arr_size(_tmp7CC,47));}),_tag_arr(
_tmp7CB,sizeof(void*),0));});}goto _LL4FD;_LL506:;_LL507: goto _LL4FD;_LL4FD:;}}if(
num_convs > 1)({void*_tmp7CD[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7CE="function can't have multiple calling conventions";
_tag_arr(_tmp7CE,sizeof(char),_get_zero_arr_size(_tmp7CE,49));}),_tag_arr(
_tmp7CD,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*_tmp712);{struct
Cyc_List_List*b=*_tmp712;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=
Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp7CF=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp7D0;_LL509: if(*((int*)_tmp7CF)!= 0)goto
_LL50B;_tmp7D0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7CF)->f1;if((int)
_tmp7D0 != 1)goto _LL50B;_LL50A:({struct Cyc_String_pa_struct _tmp7D3;_tmp7D3.tag=0;
_tmp7D3.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);{
void*_tmp7D1[1]={& _tmp7D3};Cyc_Tcutil_terr(loc,({const char*_tmp7D2="function attempts to abstract Mem type variable %s";
_tag_arr(_tmp7D2,sizeof(char),_get_zero_arr_size(_tmp7D2,51));}),_tag_arr(
_tmp7D1,sizeof(void*),1));}});goto _LL508;_LL50B:;_LL50C: goto _LL508;_LL508:;}}}{
struct Cyc_Tcutil_CVTEnv _tmp7D4=({struct Cyc_Tcutil_CVTEnv _tmp84E;_tmp84E.kind_env=
cvtenv.kind_env;_tmp84E.free_vars=0;_tmp84E.free_evars=0;_tmp84E.generalize_evars=
cvtenv.generalize_evars;_tmp84E.fn_result=1;_tmp84E;});_tmp7D4=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7D4,(void*)1,_tmp715);_tmp7D4.fn_result=0;{struct Cyc_List_List*a=
_tmp716;for(0;a != 0;a=a->tl){_tmp7D4=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp7D4,(void*)1,(*((struct _tuple2*)a->hd)).f3);}}if(_tmp718 != 0){if(_tmp717)({
void*_tmp7D5[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp7D6="both c_vararg and cyc_vararg";_tag_arr(_tmp7D6,sizeof(char),
_get_zero_arr_size(_tmp7D6,29));}),_tag_arr(_tmp7D5,sizeof(void*),0));});{void*
_tmp7D8;int _tmp7D9;struct Cyc_Absyn_VarargInfo _tmp7D7=*_tmp718;_tmp7D8=(void*)
_tmp7D7.type;_tmp7D9=_tmp7D7.inject;_tmp7D4=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp7D4,(void*)1,_tmp7D8);if(_tmp7D9){void*_tmp7DA=Cyc_Tcutil_compress(_tmp7D8);
struct Cyc_Absyn_TunionInfo _tmp7DB;void*_tmp7DC;_LL50E: if(_tmp7DA <= (void*)3?1:*((
int*)_tmp7DA)!= 2)goto _LL510;_tmp7DB=((struct Cyc_Absyn_TunionType_struct*)
_tmp7DA)->f1;_tmp7DC=(void*)_tmp7DB.tunion_info;if(*((int*)_tmp7DC)!= 1)goto
_LL510;_LL50F: goto _LL50D;_LL510:;_LL511:({void*_tmp7DD[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp7DE="can't inject a non-[x]tunion type";_tag_arr(_tmp7DE,sizeof(
char),_get_zero_arr_size(_tmp7DE,34));}),_tag_arr(_tmp7DD,sizeof(void*),0));});
goto _LL50D;_LL50D:;}}}if(seen_format){int _tmp7DF=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp716);if((((fmt_desc_arg < 0?1: fmt_desc_arg > _tmp7DF)?1:
fmt_arg_start < 0)?1:(_tmp718 == 0?fmt_arg_start != 0: 0))?1:(_tmp718 != 0?
fmt_arg_start != _tmp7DF + 1: 0))({void*_tmp7E0[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp7E1="bad format descriptor";_tag_arr(_tmp7E1,sizeof(char),
_get_zero_arr_size(_tmp7E1,22));}),_tag_arr(_tmp7E0,sizeof(void*),0));});else{
void*_tmp7E3;struct _tuple2 _tmp7E2=*((struct _tuple2*(*)(struct Cyc_List_List*x,int
n))Cyc_List_nth)(_tmp716,fmt_desc_arg - 1);_tmp7E3=_tmp7E2.f3;{void*_tmp7E4=Cyc_Tcutil_compress(
_tmp7E3);struct Cyc_Absyn_PtrInfo _tmp7E5;void*_tmp7E6;struct Cyc_Absyn_PtrAtts
_tmp7E7;struct Cyc_Absyn_Conref*_tmp7E8;struct Cyc_Absyn_Conref*_tmp7E9;_LL513: if(
_tmp7E4 <= (void*)3?1:*((int*)_tmp7E4)!= 4)goto _LL515;_tmp7E5=((struct Cyc_Absyn_PointerType_struct*)
_tmp7E4)->f1;_tmp7E6=(void*)_tmp7E5.elt_typ;_tmp7E7=_tmp7E5.ptr_atts;_tmp7E8=
_tmp7E7.bounds;_tmp7E9=_tmp7E7.zero_term;_LL514:{struct _tuple6 _tmp7EB=({struct
_tuple6 _tmp7EA;_tmp7EA.f1=Cyc_Tcutil_compress(_tmp7E6);_tmp7EA.f2=Cyc_Absyn_conref_def((
void*)((void*)0),_tmp7E8);_tmp7EA;});void*_tmp7EC;void*_tmp7ED;void*_tmp7EE;void*
_tmp7EF;_LL518: _tmp7EC=_tmp7EB.f1;if(_tmp7EC <= (void*)3?1:*((int*)_tmp7EC)!= 5)
goto _LL51A;_tmp7ED=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7EC)->f1;if((int)
_tmp7ED != 2)goto _LL51A;_tmp7EE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7EC)->f2;
if((int)_tmp7EE != 0)goto _LL51A;_tmp7EF=_tmp7EB.f2;if((int)_tmp7EF != 0)goto _LL51A;
_LL519: goto _LL517;_LL51A:;_LL51B:({void*_tmp7F0[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp7F1="format descriptor is not a char ? type";_tag_arr(_tmp7F1,
sizeof(char),_get_zero_arr_size(_tmp7F1,39));}),_tag_arr(_tmp7F0,sizeof(void*),0));});
goto _LL517;_LL517:;}goto _LL512;_LL515:;_LL516:({void*_tmp7F2[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp7F3="format descriptor is not a char ? type";_tag_arr(
_tmp7F3,sizeof(char),_get_zero_arr_size(_tmp7F3,39));}),_tag_arr(_tmp7F2,sizeof(
void*),0));});goto _LL512;_LL512:;}if(fmt_arg_start != 0){int problem;{struct
_tuple6 _tmp7F5=({struct _tuple6 _tmp7F4;_tmp7F4.f1=ft;_tmp7F4.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp718))->type);_tmp7F4;});void*
_tmp7F6;void*_tmp7F7;struct Cyc_Absyn_TunionInfo _tmp7F8;void*_tmp7F9;struct Cyc_Absyn_Tuniondecl**
_tmp7FA;struct Cyc_Absyn_Tuniondecl*_tmp7FB;void*_tmp7FC;void*_tmp7FD;struct Cyc_Absyn_TunionInfo
_tmp7FE;void*_tmp7FF;struct Cyc_Absyn_Tuniondecl**_tmp800;struct Cyc_Absyn_Tuniondecl*
_tmp801;_LL51D: _tmp7F6=_tmp7F5.f1;if((int)_tmp7F6 != 0)goto _LL51F;_tmp7F7=_tmp7F5.f2;
if(_tmp7F7 <= (void*)3?1:*((int*)_tmp7F7)!= 2)goto _LL51F;_tmp7F8=((struct Cyc_Absyn_TunionType_struct*)
_tmp7F7)->f1;_tmp7F9=(void*)_tmp7F8.tunion_info;if(*((int*)_tmp7F9)!= 1)goto
_LL51F;_tmp7FA=((struct Cyc_Absyn_KnownTunion_struct*)_tmp7F9)->f1;_tmp7FB=*
_tmp7FA;_LL51E: problem=Cyc_Absyn_qvar_cmp(_tmp7FB->name,Cyc_Absyn_tunion_print_arg_qvar)
!= 0;goto _LL51C;_LL51F: _tmp7FC=_tmp7F5.f1;if((int)_tmp7FC != 1)goto _LL521;_tmp7FD=
_tmp7F5.f2;if(_tmp7FD <= (void*)3?1:*((int*)_tmp7FD)!= 2)goto _LL521;_tmp7FE=((
struct Cyc_Absyn_TunionType_struct*)_tmp7FD)->f1;_tmp7FF=(void*)_tmp7FE.tunion_info;
if(*((int*)_tmp7FF)!= 1)goto _LL521;_tmp800=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp7FF)->f1;_tmp801=*_tmp800;_LL520: problem=Cyc_Absyn_qvar_cmp(_tmp801->name,
Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL51C;_LL521:;_LL522: problem=1;goto
_LL51C;_LL51C:;}if(problem)({void*_tmp802[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp803="format attribute and vararg types don't match";_tag_arr(_tmp803,sizeof(
char),_get_zero_arr_size(_tmp803,46));}),_tag_arr(_tmp802,sizeof(void*),0));});}}}{
struct Cyc_List_List*rpo=_tmp719;for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp805;
void*_tmp806;void*_tmp807;struct _tuple6*_tmp804=(struct _tuple6*)rpo->hd;_tmp805=*
_tmp804;_tmp806=_tmp805.f1;_tmp807=_tmp805.f2;_tmp7D4=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7D4,(void*)4,_tmp806);_tmp7D4=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp7D4,(void*)3,_tmp807);}}if(*_tmp714 != 0)_tmp7D4=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7D4,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp714))->v);
else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp7D4.free_vars;
for(0;tvs != 0;tvs=tvs->tl){void*_tmp808=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmp809;struct Cyc_Core_Opt**_tmp80A;void*
_tmp80B;void*_tmp80C;void*_tmp80D;void*_tmp80E;struct Cyc_Core_Opt*_tmp80F;struct
Cyc_Core_Opt**_tmp810;void*_tmp811;void*_tmp812;struct Cyc_Core_Opt*_tmp813;
struct Cyc_Core_Opt**_tmp814;_LL524: if(*((int*)_tmp808)!= 2)goto _LL526;_tmp809=((
struct Cyc_Absyn_Less_kb_struct*)_tmp808)->f1;_tmp80A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp808)->f1;_tmp80B=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp808)->f2;if((int)_tmp80B != 3)goto _LL526;_LL525:*_tmp80A=({struct Cyc_Core_Opt*
_tmp815=_cycalloc(sizeof(*_tmp815));_tmp815->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp817;_tmp817.tag=0;_tmp817.f1=(void*)((void*)3);_tmp817;});_tmp816;}));
_tmp815;});goto _LL527;_LL526: if(*((int*)_tmp808)!= 0)goto _LL528;_tmp80C=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp808)->f1;if((int)_tmp80C != 3)goto _LL528;_LL527:
effect=({struct Cyc_List_List*_tmp818=_cycalloc(sizeof(*_tmp818));_tmp818->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp819=_cycalloc(sizeof(*
_tmp819));_tmp819[0]=({struct Cyc_Absyn_AccessEff_struct _tmp81A;_tmp81A.tag=19;
_tmp81A.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp81B=_cycalloc(
sizeof(*_tmp81B));_tmp81B[0]=({struct Cyc_Absyn_VarType_struct _tmp81C;_tmp81C.tag=
1;_tmp81C.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp81C;});_tmp81B;}));_tmp81A;});
_tmp819;}));_tmp818->tl=effect;_tmp818;});goto _LL523;_LL528: if(*((int*)_tmp808)
!= 2)goto _LL52A;_tmp80D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp808)->f2;
if((int)_tmp80D != 5)goto _LL52A;_LL529: goto _LL52B;_LL52A: if(*((int*)_tmp808)!= 0)
goto _LL52C;_tmp80E=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp808)->f1;if((int)
_tmp80E != 5)goto _LL52C;_LL52B: goto _LL523;_LL52C: if(*((int*)_tmp808)!= 2)goto
_LL52E;_tmp80F=((struct Cyc_Absyn_Less_kb_struct*)_tmp808)->f1;_tmp810=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp808)->f1;_tmp811=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp808)->f2;if((int)_tmp811 != 4)goto _LL52E;_LL52D:*_tmp810=({struct Cyc_Core_Opt*
_tmp81D=_cycalloc(sizeof(*_tmp81D));_tmp81D->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp81F;_tmp81F.tag=0;_tmp81F.f1=(void*)((void*)4);_tmp81F;});_tmp81E;}));
_tmp81D;});goto _LL52F;_LL52E: if(*((int*)_tmp808)!= 0)goto _LL530;_tmp812=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp808)->f1;if((int)_tmp812 != 4)goto _LL530;_LL52F:
effect=({struct Cyc_List_List*_tmp820=_cycalloc(sizeof(*_tmp820));_tmp820->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp821=_cycalloc(sizeof(*_tmp821));
_tmp821[0]=({struct Cyc_Absyn_VarType_struct _tmp822;_tmp822.tag=1;_tmp822.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp822;});_tmp821;}));_tmp820->tl=effect;_tmp820;});
goto _LL523;_LL530: if(*((int*)_tmp808)!= 1)goto _LL532;_tmp813=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp808)->f1;_tmp814=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp808)->f1;_LL531:*_tmp814=({struct Cyc_Core_Opt*_tmp823=_cycalloc(sizeof(*
_tmp823));_tmp823->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp824=
_cycalloc(sizeof(*_tmp824));_tmp824[0]=({struct Cyc_Absyn_Less_kb_struct _tmp825;
_tmp825.tag=2;_tmp825.f1=0;_tmp825.f2=(void*)((void*)0);_tmp825;});_tmp824;}));
_tmp823;});goto _LL533;_LL532:;_LL533: effect=({struct Cyc_List_List*_tmp826=
_cycalloc(sizeof(*_tmp826));_tmp826->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp827=_cycalloc(sizeof(*_tmp827));_tmp827[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp828;_tmp828.tag=21;_tmp828.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp829=_cycalloc(sizeof(*_tmp829));_tmp829[0]=({struct Cyc_Absyn_VarType_struct
_tmp82A;_tmp82A.tag=1;_tmp82A.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp82A;});
_tmp829;}));_tmp828;});_tmp827;}));_tmp826->tl=effect;_tmp826;});goto _LL523;
_LL523:;}}{struct Cyc_List_List*ts=_tmp7D4.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmp82B=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL535: if((int)_tmp82B != 3)goto
_LL537;_LL536: effect=({struct Cyc_List_List*_tmp82C=_cycalloc(sizeof(*_tmp82C));
_tmp82C->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp82D=_cycalloc(
sizeof(*_tmp82D));_tmp82D[0]=({struct Cyc_Absyn_AccessEff_struct _tmp82E;_tmp82E.tag=
19;_tmp82E.f1=(void*)((void*)ts->hd);_tmp82E;});_tmp82D;}));_tmp82C->tl=effect;
_tmp82C;});goto _LL534;_LL537: if((int)_tmp82B != 4)goto _LL539;_LL538: effect=({
struct Cyc_List_List*_tmp82F=_cycalloc(sizeof(*_tmp82F));_tmp82F->hd=(void*)((
void*)ts->hd);_tmp82F->tl=effect;_tmp82F;});goto _LL534;_LL539: if((int)_tmp82B != 
5)goto _LL53B;_LL53A: goto _LL534;_LL53B:;_LL53C: effect=({struct Cyc_List_List*
_tmp830=_cycalloc(sizeof(*_tmp830));_tmp830->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp831=_cycalloc(sizeof(*_tmp831));_tmp831[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp832;_tmp832.tag=21;_tmp832.f1=(void*)((void*)ts->hd);_tmp832;});_tmp831;}));
_tmp830->tl=effect;_tmp830;});goto _LL534;_LL534:;}}*_tmp714=({struct Cyc_Core_Opt*
_tmp833=_cycalloc(sizeof(*_tmp833));_tmp833->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp834=_cycalloc(sizeof(*_tmp834));_tmp834[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp835;_tmp835.tag=20;_tmp835.f1=effect;_tmp835;});_tmp834;}));_tmp833;});}if(*
_tmp712 != 0){struct Cyc_List_List*bs=*_tmp712;for(0;bs != 0;bs=bs->tl){void*
_tmp836=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp837;struct Cyc_Core_Opt**_tmp838;struct Cyc_Core_Opt*
_tmp839;struct Cyc_Core_Opt**_tmp83A;void*_tmp83B;struct Cyc_Core_Opt*_tmp83C;
struct Cyc_Core_Opt**_tmp83D;void*_tmp83E;struct Cyc_Core_Opt*_tmp83F;struct Cyc_Core_Opt**
_tmp840;void*_tmp841;void*_tmp842;_LL53E: if(*((int*)_tmp836)!= 1)goto _LL540;
_tmp837=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp836)->f1;_tmp838=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp836)->f1;_LL53F:({struct Cyc_String_pa_struct
_tmp845;_tmp845.tag=0;_tmp845.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)bs->hd)->name);{void*_tmp843[1]={& _tmp845};Cyc_Tcutil_warn(
loc,({const char*_tmp844="Type variable %s unconstrained, assuming boxed";
_tag_arr(_tmp844,sizeof(char),_get_zero_arr_size(_tmp844,47));}),_tag_arr(
_tmp843,sizeof(void*),1));}});_tmp83A=_tmp838;goto _LL541;_LL540: if(*((int*)
_tmp836)!= 2)goto _LL542;_tmp839=((struct Cyc_Absyn_Less_kb_struct*)_tmp836)->f1;
_tmp83A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp836)->f1;
_tmp83B=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp836)->f2;if((int)_tmp83B != 
1)goto _LL542;_LL541: _tmp83D=_tmp83A;goto _LL543;_LL542: if(*((int*)_tmp836)!= 2)
goto _LL544;_tmp83C=((struct Cyc_Absyn_Less_kb_struct*)_tmp836)->f1;_tmp83D=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp836)->f1;_tmp83E=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp836)->f2;if((int)_tmp83E != 0)goto
_LL544;_LL543:*_tmp83D=({struct Cyc_Core_Opt*_tmp846=_cycalloc(sizeof(*_tmp846));
_tmp846->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp847=_cycalloc(
sizeof(*_tmp847));_tmp847[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp848;_tmp848.tag=0;
_tmp848.f1=(void*)((void*)2);_tmp848;});_tmp847;}));_tmp846;});goto _LL53D;_LL544:
if(*((int*)_tmp836)!= 2)goto _LL546;_tmp83F=((struct Cyc_Absyn_Less_kb_struct*)
_tmp836)->f1;_tmp840=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp836)->f1;_tmp841=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp836)->f2;
_LL545:*_tmp840=({struct Cyc_Core_Opt*_tmp849=_cycalloc(sizeof(*_tmp849));_tmp849->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp84A=_cycalloc(sizeof(*_tmp84A));
_tmp84A[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp84B;_tmp84B.tag=0;_tmp84B.f1=(void*)
_tmp841;_tmp84B;});_tmp84A;}));_tmp849;});goto _LL53D;_LL546: if(*((int*)_tmp836)
!= 0)goto _LL548;_tmp842=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp836)->f1;if((
int)_tmp842 != 1)goto _LL548;_LL547:({void*_tmp84C[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp84D="functions can't abstract M types";_tag_arr(_tmp84D,sizeof(
char),_get_zero_arr_size(_tmp84D,33));}),_tag_arr(_tmp84C,sizeof(void*),0));});
goto _LL53D;_LL548:;_LL549: goto _LL53D;_LL53D:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmp7D4.kind_env,*_tmp712);_tmp7D4.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp7D4.free_vars,*_tmp712);{struct Cyc_List_List*tvs=_tmp7D4.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp7D4.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL484;}}_LL4A3: if(_tmp6EC <= (void*)3?1:*((int*)_tmp6EC)!= 
9)goto _LL4A5;_tmp71B=((struct Cyc_Absyn_TupleType_struct*)_tmp6EC)->f1;_LL4A4:
for(0;_tmp71B != 0;_tmp71B=_tmp71B->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,(void*)1,(*((struct _tuple4*)_tmp71B->hd)).f2);}goto _LL484;_LL4A5: if(
_tmp6EC <= (void*)3?1:*((int*)_tmp6EC)!= 11)goto _LL4A7;_tmp71C=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6EC)->f1;_tmp71D=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp6EC)->f2;_LL4A6:{
struct _RegionHandle _tmp84F=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp84F;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp71D != 0;_tmp71D=_tmp71D->tl){struct Cyc_Absyn_Aggrfield _tmp851;struct
_tagged_arr*_tmp852;struct Cyc_Absyn_Tqual _tmp853;void*_tmp854;struct Cyc_Absyn_Exp*
_tmp855;struct Cyc_List_List*_tmp856;struct Cyc_Absyn_Aggrfield*_tmp850=(struct Cyc_Absyn_Aggrfield*)
_tmp71D->hd;_tmp851=*_tmp850;_tmp852=_tmp851.name;_tmp853=_tmp851.tq;_tmp854=(
void*)_tmp851.type;_tmp855=_tmp851.width;_tmp856=_tmp851.attributes;if(((int(*)(
int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,
struct _tagged_arr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp852))({struct
Cyc_String_pa_struct _tmp859;_tmp859.tag=0;_tmp859.f1=(struct _tagged_arr)((struct
_tagged_arr)*_tmp852);{void*_tmp857[1]={& _tmp859};Cyc_Tcutil_terr(loc,({const
char*_tmp858="duplicate field %s";_tag_arr(_tmp858,sizeof(char),
_get_zero_arr_size(_tmp858,19));}),_tag_arr(_tmp857,sizeof(void*),1));}});if(Cyc_strcmp((
struct _tagged_arr)*_tmp852,({const char*_tmp85A="";_tag_arr(_tmp85A,sizeof(char),
_get_zero_arr_size(_tmp85A,1));}))!= 0)prev_fields=({struct Cyc_List_List*_tmp85B=
_region_malloc(aprev_rgn,sizeof(*_tmp85B));_tmp85B->hd=_tmp852;_tmp85B->tl=
prev_fields;_tmp85B;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)
1,_tmp854);if(_tmp71C == (void*)1?!Cyc_Tcutil_bits_only(_tmp854): 0)({struct Cyc_String_pa_struct
_tmp85E;_tmp85E.tag=0;_tmp85E.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp852);{
void*_tmp85C[1]={& _tmp85E};Cyc_Tcutil_warn(loc,({const char*_tmp85D="union member %s is not `bits-only' so it can only be written and not read";
_tag_arr(_tmp85D,sizeof(char),_get_zero_arr_size(_tmp85D,74));}),_tag_arr(
_tmp85C,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(loc,te,_tmp854,_tmp855,
_tmp852);Cyc_Tcutil_check_field_atts(loc,_tmp852,_tmp856);}};_pop_region(
aprev_rgn);}goto _LL484;_LL4A7: if(_tmp6EC <= (void*)3?1:*((int*)_tmp6EC)!= 10)goto
_LL4A9;_tmp71E=((struct Cyc_Absyn_AggrType_struct*)_tmp6EC)->f1;_tmp71F=(void*)
_tmp71E.aggr_info;_tmp720=(void**)&(((struct Cyc_Absyn_AggrType_struct*)_tmp6EC)->f1).aggr_info;
_tmp721=_tmp71E.targs;_tmp722=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp6EC)->f1).targs;_LL4A8:{void*_tmp85F=*_tmp720;void*_tmp860;struct _tuple1*
_tmp861;struct Cyc_Absyn_Aggrdecl**_tmp862;struct Cyc_Absyn_Aggrdecl*_tmp863;
_LL54B: if(*((int*)_tmp85F)!= 0)goto _LL54D;_tmp860=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp85F)->f1;_tmp861=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp85F)->f2;_LL54C: {
struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp864;_push_handler(& _tmp864);{
int _tmp866=0;if(setjmp(_tmp864.handler))_tmp866=1;if(!_tmp866){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp861);*_tmp720=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp867=
_cycalloc(sizeof(*_tmp867));_tmp867[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp868;
_tmp868.tag=1;_tmp868.f1=adp;_tmp868;});_tmp867;});;_pop_handler();}else{void*
_tmp865=(void*)_exn_thrown;void*_tmp86A=_tmp865;_LL550: if(_tmp86A != Cyc_Dict_Absent)
goto _LL552;_LL551: {struct Cyc_Tcenv_Genv*_tmp86B=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp86C=({struct Cyc_Absyn_Aggrdecl*_tmp872=_cycalloc(
sizeof(*_tmp872));_tmp872->kind=(void*)_tmp860;_tmp872->sc=(void*)((void*)3);
_tmp872->name=_tmp861;_tmp872->tvs=0;_tmp872->impl=0;_tmp872->attributes=0;
_tmp872;});Cyc_Tc_tcAggrdecl(te,_tmp86B,loc,_tmp86C);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp861);*_tmp720=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp86D=
_cycalloc(sizeof(*_tmp86D));_tmp86D[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp86E;
_tmp86E.tag=1;_tmp86E.f1=adp;_tmp86E;});_tmp86D;});if(*_tmp722 != 0){({struct Cyc_String_pa_struct
_tmp871;_tmp871.tag=0;_tmp871.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp861));{void*_tmp86F[1]={& _tmp871};Cyc_Tcutil_terr(loc,({const char*_tmp870="declare parameterized type %s before using";
_tag_arr(_tmp870,sizeof(char),_get_zero_arr_size(_tmp870,43));}),_tag_arr(
_tmp86F,sizeof(void*),1));}});return cvtenv;}goto _LL54F;}_LL552:;_LL553:(void)
_throw(_tmp86A);_LL54F:;}}}_tmp863=*adp;goto _LL54E;}_LL54D: if(*((int*)_tmp85F)!= 
1)goto _LL54A;_tmp862=((struct Cyc_Absyn_KnownAggr_struct*)_tmp85F)->f1;_tmp863=*
_tmp862;_LL54E: {struct Cyc_List_List*tvs=_tmp863->tvs;struct Cyc_List_List*ts=*
_tmp722;for(0;ts != 0?tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd);}if(ts != 0)({struct Cyc_String_pa_struct _tmp875;_tmp875.tag=
0;_tmp875.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp863->name));{void*_tmp873[1]={& _tmp875};Cyc_Tcutil_terr(loc,({const char*
_tmp874="too many parameters for type %s";_tag_arr(_tmp874,sizeof(char),
_get_zero_arr_size(_tmp874,32));}),_tag_arr(_tmp873,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp876=_cycalloc(sizeof(*_tmp876));_tmp876->hd=(void*)e;_tmp876->tl=
hidden_ts;_tmp876;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*
_tmp722=Cyc_List_imp_append(*_tmp722,Cyc_List_imp_rev(hidden_ts));}}_LL54A:;}
goto _LL484;_LL4A9: if(_tmp6EC <= (void*)3?1:*((int*)_tmp6EC)!= 16)goto _LL4AB;
_tmp723=((struct Cyc_Absyn_TypedefType_struct*)_tmp6EC)->f1;_tmp724=((struct Cyc_Absyn_TypedefType_struct*)
_tmp6EC)->f2;_tmp725=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp6EC)->f2;_tmp726=((struct Cyc_Absyn_TypedefType_struct*)_tmp6EC)->f3;_tmp727=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp6EC)->f3;
_tmp728=((struct Cyc_Absyn_TypedefType_struct*)_tmp6EC)->f4;_tmp729=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp6EC)->f4;_LL4AA: {struct Cyc_List_List*
targs=*_tmp725;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp877;
_push_handler(& _tmp877);{int _tmp879=0;if(setjmp(_tmp877.handler))_tmp879=1;if(!
_tmp879){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp723);;_pop_handler();}else{
void*_tmp878=(void*)_exn_thrown;void*_tmp87B=_tmp878;_LL555: if(_tmp87B != Cyc_Dict_Absent)
goto _LL557;_LL556:({struct Cyc_String_pa_struct _tmp87E;_tmp87E.tag=0;_tmp87E.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp723));{void*
_tmp87C[1]={& _tmp87E};Cyc_Tcutil_terr(loc,({const char*_tmp87D="unbound typedef name %s";
_tag_arr(_tmp87D,sizeof(char),_get_zero_arr_size(_tmp87D,24));}),_tag_arr(
_tmp87C,sizeof(void*),1));}});return cvtenv;_LL557:;_LL558:(void)_throw(_tmp87B);
_LL554:;}}}*_tmp727=(struct Cyc_Absyn_Typedefdecl*)td;_tmp723[0]=(td->name)[
_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*
ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0?tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);inst=({struct Cyc_List_List*_tmp87F=_cycalloc(
sizeof(*_tmp87F));_tmp87F->hd=({struct _tuple8*_tmp880=_cycalloc(sizeof(*_tmp880));
_tmp880->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp880->f2=(void*)ts->hd;_tmp880;});
_tmp87F->tl=inst;_tmp87F;});}if(ts != 0)({struct Cyc_String_pa_struct _tmp883;
_tmp883.tag=0;_tmp883.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp723));{void*_tmp881[1]={& _tmp883};Cyc_Tcutil_terr(loc,({const char*_tmp882="too many parameters for typedef %s";
_tag_arr(_tmp882,sizeof(char),_get_zero_arr_size(_tmp882,35));}),_tag_arr(
_tmp881,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;
tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp884=_cycalloc(
sizeof(*_tmp884));_tmp884->hd=(void*)e;_tmp884->tl=hidden_ts;_tmp884;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);inst=({struct Cyc_List_List*
_tmp885=_cycalloc(sizeof(*_tmp885));_tmp885->hd=({struct _tuple8*_tmp886=
_cycalloc(sizeof(*_tmp886));_tmp886->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp886->f2=
e;_tmp886;});_tmp885->tl=inst;_tmp885;});}*_tmp725=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp729=({void**_tmp887=
_cycalloc(sizeof(*_tmp887));_tmp887[0]=new_typ;_tmp887;});}goto _LL484;}}_LL4AB:
if(_tmp6EC <= (void*)3?1:*((int*)_tmp6EC)!= 18)goto _LL4AD;_LL4AC: goto _LL4AE;
_LL4AD: if((int)_tmp6EC != 2)goto _LL4AF;_LL4AE: goto _LL484;_LL4AF: if(_tmp6EC <= (
void*)3?1:*((int*)_tmp6EC)!= 15)goto _LL4B1;_tmp72A=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6EC)->f1;_LL4B0: _tmp72B=_tmp72A;goto _LL4B2;_LL4B1: if(_tmp6EC <= (void*)3?1:*((
int*)_tmp6EC)!= 19)goto _LL4B3;_tmp72B=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp6EC)->f1;_LL4B2: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp72B);goto _LL484;_LL4B3: if(_tmp6EC <= (void*)3?1:*((int*)_tmp6EC)!= 21)goto
_LL4B5;_tmp72C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp6EC)->f1;_LL4B4:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp72C);goto _LL484;
_LL4B5: if(_tmp6EC <= (void*)3?1:*((int*)_tmp6EC)!= 20)goto _LL484;_tmp72D=((struct
Cyc_Absyn_JoinEff_struct*)_tmp6EC)->f1;_LL4B6: for(0;_tmp72D != 0;_tmp72D=_tmp72D->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)4,(void*)_tmp72D->hd);}
goto _LL484;_LL484:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind))({
struct Cyc_String_pa_struct _tmp88C;_tmp88C.tag=0;_tmp88C.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_kind2string(expected_kind));{struct Cyc_String_pa_struct
_tmp88B;_tmp88B.tag=0;_tmp88B.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind(t)));{struct Cyc_String_pa_struct _tmp88A;_tmp88A.tag=0;
_tmp88A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
void*_tmp888[3]={& _tmp88A,& _tmp88B,& _tmp88C};Cyc_Tcutil_terr(loc,({const char*
_tmp889="type %s has kind %s but as used here needs kind %s";_tag_arr(_tmp889,
sizeof(char),_get_zero_arr_size(_tmp889,51));}),_tag_arr(_tmp888,sizeof(void*),3));}}}});
return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*kind_env,void*expected_kind,int
generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp88D=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp896;_tmp896.kind_env=kind_env;_tmp896.free_vars=
0;_tmp896.free_evars=0;_tmp896.generalize_evars=generalize_evars;_tmp896.fn_result=
0;_tmp896;}),expected_kind,t);{struct Cyc_List_List*vs=_tmp88D.free_vars;for(0;vs
!= 0;vs=vs->tl){_tmp88D.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(struct
Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp88D.free_evars;for(0;evs
!= 0;evs=evs->tl){void*_tmp88E=Cyc_Tcutil_compress((void*)evs->hd);struct Cyc_Core_Opt*
_tmp88F;struct Cyc_Core_Opt**_tmp890;_LL55A: if(_tmp88E <= (void*)3?1:*((int*)
_tmp88E)!= 0)goto _LL55C;_tmp88F=((struct Cyc_Absyn_Evar_struct*)_tmp88E)->f4;
_tmp890=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp88E)->f4;
_LL55B: if(*_tmp890 == 0)*_tmp890=({struct Cyc_Core_Opt*_tmp891=_cycalloc(sizeof(*
_tmp891));_tmp891->v=kind_env;_tmp891;});else{struct Cyc_List_List*_tmp892=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp890))->v;struct Cyc_List_List*
_tmp893=0;for(0;_tmp892 != 0;_tmp892=_tmp892->tl){if(((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)
_tmp892->hd))_tmp893=({struct Cyc_List_List*_tmp894=_cycalloc(sizeof(*_tmp894));
_tmp894->hd=(struct Cyc_Absyn_Tvar*)_tmp892->hd;_tmp894->tl=_tmp893;_tmp894;});}*
_tmp890=({struct Cyc_Core_Opt*_tmp895=_cycalloc(sizeof(*_tmp895));_tmp895->v=
_tmp893;_tmp895;});}goto _LL559;_LL55C:;_LL55D: goto _LL559;_LL559:;}}return _tmp88D;}
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_Tcutil_CVTEnv _tmp897=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t);struct Cyc_List_List*_tmp898=_tmp897.free_vars;struct Cyc_List_List*
_tmp899=_tmp897.free_evars;{struct Cyc_List_List*x=_tmp898;for(0;x != 0;x=x->tl){
void*_tmp89A=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp89B;struct Cyc_Core_Opt**_tmp89C;struct Cyc_Core_Opt*
_tmp89D;struct Cyc_Core_Opt**_tmp89E;void*_tmp89F;struct Cyc_Core_Opt*_tmp8A0;
struct Cyc_Core_Opt**_tmp8A1;void*_tmp8A2;struct Cyc_Core_Opt*_tmp8A3;struct Cyc_Core_Opt**
_tmp8A4;void*_tmp8A5;void*_tmp8A6;_LL55F: if(*((int*)_tmp89A)!= 1)goto _LL561;
_tmp89B=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp89A)->f1;_tmp89C=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp89A)->f1;_LL560: _tmp89E=_tmp89C;goto
_LL562;_LL561: if(*((int*)_tmp89A)!= 2)goto _LL563;_tmp89D=((struct Cyc_Absyn_Less_kb_struct*)
_tmp89A)->f1;_tmp89E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp89A)->f1;_tmp89F=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp89A)->f2;if((
int)_tmp89F != 1)goto _LL563;_LL562: _tmp8A1=_tmp89E;goto _LL564;_LL563: if(*((int*)
_tmp89A)!= 2)goto _LL565;_tmp8A0=((struct Cyc_Absyn_Less_kb_struct*)_tmp89A)->f1;
_tmp8A1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp89A)->f1;
_tmp8A2=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp89A)->f2;if((int)_tmp8A2 != 
0)goto _LL565;_LL564:*_tmp8A1=({struct Cyc_Core_Opt*_tmp8A7=_cycalloc(sizeof(*
_tmp8A7));_tmp8A7->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8A8=
_cycalloc(sizeof(*_tmp8A8));_tmp8A8[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8A9;
_tmp8A9.tag=0;_tmp8A9.f1=(void*)((void*)2);_tmp8A9;});_tmp8A8;}));_tmp8A7;});
goto _LL55E;_LL565: if(*((int*)_tmp89A)!= 2)goto _LL567;_tmp8A3=((struct Cyc_Absyn_Less_kb_struct*)
_tmp89A)->f1;_tmp8A4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp89A)->f1;_tmp8A5=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp89A)->f2;
_LL566:*_tmp8A4=({struct Cyc_Core_Opt*_tmp8AA=_cycalloc(sizeof(*_tmp8AA));_tmp8AA->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8AB=_cycalloc(sizeof(*_tmp8AB));
_tmp8AB[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8AC;_tmp8AC.tag=0;_tmp8AC.f1=(void*)
_tmp8A5;_tmp8AC;});_tmp8AB;}));_tmp8AA;});goto _LL55E;_LL567: if(*((int*)_tmp89A)
!= 0)goto _LL569;_tmp8A6=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp89A)->f1;if((
int)_tmp8A6 != 1)goto _LL569;_LL568:({struct Cyc_String_pa_struct _tmp8AF;_tmp8AF.tag=
0;_tmp8AF.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp8AD[1]={& _tmp8AF};Cyc_Tcutil_terr(loc,({
const char*_tmp8AE="type variable %s cannot have kind M";_tag_arr(_tmp8AE,sizeof(
char),_get_zero_arr_size(_tmp8AE,36));}),_tag_arr(_tmp8AD,sizeof(void*),1));}});
goto _LL55E;_LL569:;_LL56A: goto _LL55E;_LL55E:;}}if(_tmp898 != 0?1: _tmp899 != 0){{
void*_tmp8B0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp8B1;struct Cyc_List_List*
_tmp8B2;struct Cyc_List_List**_tmp8B3;struct Cyc_Core_Opt*_tmp8B4;void*_tmp8B5;
struct Cyc_List_List*_tmp8B6;int _tmp8B7;struct Cyc_Absyn_VarargInfo*_tmp8B8;struct
Cyc_List_List*_tmp8B9;struct Cyc_List_List*_tmp8BA;_LL56C: if(_tmp8B0 <= (void*)3?1:*((
int*)_tmp8B0)!= 8)goto _LL56E;_tmp8B1=((struct Cyc_Absyn_FnType_struct*)_tmp8B0)->f1;
_tmp8B2=_tmp8B1.tvars;_tmp8B3=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp8B0)->f1).tvars;_tmp8B4=_tmp8B1.effect;_tmp8B5=(void*)_tmp8B1.ret_typ;
_tmp8B6=_tmp8B1.args;_tmp8B7=_tmp8B1.c_varargs;_tmp8B8=_tmp8B1.cyc_varargs;
_tmp8B9=_tmp8B1.rgn_po;_tmp8BA=_tmp8B1.attributes;_LL56D: if(*_tmp8B3 == 0){*
_tmp8B3=_tmp898;_tmp898=0;}goto _LL56B;_LL56E:;_LL56F: goto _LL56B;_LL56B:;}if(
_tmp898 != 0)({struct Cyc_String_pa_struct _tmp8BD;_tmp8BD.tag=0;_tmp8BD.f1=(struct
_tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)_tmp898->hd)->name);{
void*_tmp8BB[1]={& _tmp8BD};Cyc_Tcutil_terr(loc,({const char*_tmp8BC="unbound type variable %s";
_tag_arr(_tmp8BC,sizeof(char),_get_zero_arr_size(_tmp8BC,25));}),_tag_arr(
_tmp8BB,sizeof(void*),1));}});if(_tmp899 != 0)for(0;_tmp899 != 0;_tmp899=_tmp899->tl){
void*e=(void*)_tmp899->hd;void*_tmp8BE=Cyc_Tcutil_typ_kind(e);_LL571: if((int)
_tmp8BE != 3)goto _LL573;_LL572: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp8BF[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp8C0="can't unify evar with heap!";_tag_arr(_tmp8C0,sizeof(char),
_get_zero_arr_size(_tmp8C0,28));}),_tag_arr(_tmp8BF,sizeof(void*),0));});goto
_LL570;_LL573: if((int)_tmp8BE != 4)goto _LL575;_LL574: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({
void*_tmp8C1[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp8C2="can't unify evar with {}!";_tag_arr(_tmp8C2,sizeof(char),
_get_zero_arr_size(_tmp8C2,26));}),_tag_arr(_tmp8C1,sizeof(void*),0));});goto
_LL570;_LL575:;_LL576:({struct Cyc_String_pa_struct _tmp8C6;_tmp8C6.tag=0;_tmp8C6.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp8C5;_tmp8C5.tag=0;_tmp8C5.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
e));{void*_tmp8C3[2]={& _tmp8C5,& _tmp8C6};Cyc_Tcutil_terr(loc,({const char*_tmp8C4="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp8C4,sizeof(char),_get_zero_arr_size(_tmp8C4,52));}),_tag_arr(
_tmp8C3,sizeof(void*),2));}}});goto _LL570;_LL570:;}}}void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmp8C7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp8C8;struct Cyc_List_List*
_tmp8C9;struct Cyc_Core_Opt*_tmp8CA;_LL578: if(_tmp8C7 <= (void*)3?1:*((int*)
_tmp8C7)!= 8)goto _LL57A;_tmp8C8=((struct Cyc_Absyn_FnType_struct*)_tmp8C7)->f1;
_tmp8C9=_tmp8C8.tvars;_tmp8CA=_tmp8C8.effect;_LL579: fd->tvs=_tmp8C9;fd->effect=
_tmp8CA;goto _LL577;_LL57A:;_LL57B:({void*_tmp8CB[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8CC="check_fndecl_valid_type: not a FnType";
_tag_arr(_tmp8CC,sizeof(char),_get_zero_arr_size(_tmp8CC,38));}),_tag_arr(
_tmp8CB,sizeof(void*),0));});_LL577:;}{struct _RegionHandle _tmp8CD=_new_region("r");
struct _RegionHandle*r=& _tmp8CD;_push_region(r);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tagged_arr*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct
_tuple13*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp8CE="function declaration has repeated parameter";
_tag_arr(_tmp8CE,sizeof(char),_get_zero_arr_size(_tmp8CE,44));}));;_pop_region(r);}
fd->cached_typ=({struct Cyc_Core_Opt*_tmp8CF=_cycalloc(sizeof(*_tmp8CF));_tmp8CF->v=(
void*)t;_tmp8CF;});}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,int
allow_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp8D0=Cyc_Tcutil_check_valid_type(
loc,te,bound_tvars,expected_kind,0,t);struct Cyc_List_List*_tmp8D1=Cyc_Tcutil_remove_bound_tvars(
_tmp8D0.free_vars,bound_tvars);struct Cyc_List_List*_tmp8D2=_tmp8D0.free_evars;{
struct Cyc_List_List*fs=_tmp8D1;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp8D3=((
struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct _tmp8D7;_tmp8D7.tag=
0;_tmp8D7.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
struct Cyc_String_pa_struct _tmp8D6;_tmp8D6.tag=0;_tmp8D6.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp8D3);{void*_tmp8D4[2]={& _tmp8D6,& _tmp8D7};Cyc_Tcutil_terr(
loc,({const char*_tmp8D5="unbound type variable %s in type %s";_tag_arr(_tmp8D5,
sizeof(char),_get_zero_arr_size(_tmp8D5,36));}),_tag_arr(_tmp8D4,sizeof(void*),2));}}});}}
if(!allow_evars?_tmp8D2 != 0: 0)for(0;_tmp8D2 != 0;_tmp8D2=_tmp8D2->tl){void*e=(
void*)_tmp8D2->hd;void*_tmp8D8=Cyc_Tcutil_typ_kind(e);_LL57D: if((int)_tmp8D8 != 3)
goto _LL57F;_LL57E: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp8D9[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8DA="can't unify evar with heap!";
_tag_arr(_tmp8DA,sizeof(char),_get_zero_arr_size(_tmp8DA,28));}),_tag_arr(
_tmp8D9,sizeof(void*),0));});goto _LL57C;_LL57F: if((int)_tmp8D8 != 4)goto _LL581;
_LL580: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp8DB[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8DC="can't unify evar with {}!";
_tag_arr(_tmp8DC,sizeof(char),_get_zero_arr_size(_tmp8DC,26));}),_tag_arr(
_tmp8DB,sizeof(void*),0));});goto _LL57C;_LL581:;_LL582:({struct Cyc_String_pa_struct
_tmp8E0;_tmp8E0.tag=0;_tmp8E0.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp8DF;_tmp8DF.tag=0;_tmp8DF.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(e));{void*_tmp8DD[2]={&
_tmp8DF,& _tmp8E0};Cyc_Tcutil_terr(loc,({const char*_tmp8DE="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp8DE,sizeof(char),_get_zero_arr_size(_tmp8DE,52));}),_tag_arr(
_tmp8DD,sizeof(void*),2));}}});goto _LL57C;_LL57C:;}}void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,
tvs);}static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct
Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr(*a2string)(
void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){struct Cyc_List_List*vs2=vs->tl;
for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(void*)vs2->hd)== 0)({struct Cyc_String_pa_struct
_tmp8E4;_tmp8E4.tag=0;_tmp8E4.f1=(struct _tagged_arr)((struct _tagged_arr)a2string((
void*)vs->hd));{struct Cyc_String_pa_struct _tmp8E3;_tmp8E3.tag=0;_tmp8E3.f1=(
struct _tagged_arr)((struct _tagged_arr)msg);{void*_tmp8E1[2]={& _tmp8E3,& _tmp8E4};
Cyc_Tcutil_terr(loc,({const char*_tmp8E2="%s: %s";_tag_arr(_tmp8E2,sizeof(char),
_get_zero_arr_size(_tmp8E2,7));}),_tag_arr(_tmp8E1,sizeof(void*),2));}}});}}}
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
const char*_tmp8E5="duplicate type variable";_tag_arr(_tmp8E5,sizeof(char),
_get_zero_arr_size(_tmp8E5,24));}));}struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;
int f2;};struct _tuple19{struct Cyc_List_List*f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*
f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){if(Cyc_strcmp((
struct _tagged_arr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,({const char*
_tmp8E6="";_tag_arr(_tmp8E6,sizeof(char),_get_zero_arr_size(_tmp8E6,1));}))!= 0)
fields=({struct Cyc_List_List*_tmp8E7=_cycalloc(sizeof(*_tmp8E7));_tmp8E7->hd=({
struct _tuple18*_tmp8E8=_cycalloc(sizeof(*_tmp8E8));_tmp8E8->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd;_tmp8E8->f2=0;_tmp8E8;});_tmp8E7->tl=fields;_tmp8E7;});}}fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct
Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple19 _tmp8EA;struct Cyc_List_List*
_tmp8EB;void*_tmp8EC;struct _tuple19*_tmp8E9=(struct _tuple19*)des->hd;_tmp8EA=*
_tmp8E9;_tmp8EB=_tmp8EA.f1;_tmp8EC=_tmp8EA.f2;if(_tmp8EB == 0){struct Cyc_List_List*
_tmp8ED=fields;for(0;_tmp8ED != 0;_tmp8ED=_tmp8ED->tl){if(!(*((struct _tuple18*)
_tmp8ED->hd)).f2){(*((struct _tuple18*)_tmp8ED->hd)).f2=1;(*((struct _tuple19*)des->hd)).f1=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp8EE=_cycalloc(sizeof(*_tmp8EE));
_tmp8EE->hd=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp8EF=_cycalloc(
sizeof(*_tmp8EF));_tmp8EF[0]=({struct Cyc_Absyn_FieldName_struct _tmp8F0;_tmp8F0.tag=
1;_tmp8F0.f1=((*((struct _tuple18*)_tmp8ED->hd)).f1)->name;_tmp8F0;});_tmp8EF;}));
_tmp8EE->tl=0;_tmp8EE;});ans=({struct Cyc_List_List*_tmp8F1=_region_malloc(rgn,
sizeof(*_tmp8F1));_tmp8F1->hd=({struct _tuple20*_tmp8F2=_region_malloc(rgn,
sizeof(*_tmp8F2));_tmp8F2->f1=(*((struct _tuple18*)_tmp8ED->hd)).f1;_tmp8F2->f2=
_tmp8EC;_tmp8F2;});_tmp8F1->tl=ans;_tmp8F1;});break;}}if(_tmp8ED == 0)({void*
_tmp8F3[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp8F4="too many arguments to struct";
_tag_arr(_tmp8F4,sizeof(char),_get_zero_arr_size(_tmp8F4,29));}),_tag_arr(
_tmp8F3,sizeof(void*),0));});}else{if(_tmp8EB->tl != 0)({void*_tmp8F5[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp8F6="multiple designators are not supported";_tag_arr(
_tmp8F6,sizeof(char),_get_zero_arr_size(_tmp8F6,39));}),_tag_arr(_tmp8F5,sizeof(
void*),0));});else{void*_tmp8F7=(void*)_tmp8EB->hd;struct _tagged_arr*_tmp8F8;
_LL584: if(*((int*)_tmp8F7)!= 0)goto _LL586;_LL585:({void*_tmp8F9[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp8FA="array designator used in argument to struct";_tag_arr(
_tmp8FA,sizeof(char),_get_zero_arr_size(_tmp8FA,44));}),_tag_arr(_tmp8F9,sizeof(
void*),0));});goto _LL583;_LL586: if(*((int*)_tmp8F7)!= 1)goto _LL583;_tmp8F8=((
struct Cyc_Absyn_FieldName_struct*)_tmp8F7)->f1;_LL587: {struct Cyc_List_List*
_tmp8FB=fields;for(0;_tmp8FB != 0;_tmp8FB=_tmp8FB->tl){if(Cyc_strptrcmp(_tmp8F8,((*((
struct _tuple18*)_tmp8FB->hd)).f1)->name)== 0){if((*((struct _tuple18*)_tmp8FB->hd)).f2)({
struct Cyc_String_pa_struct _tmp8FE;_tmp8FE.tag=0;_tmp8FE.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp8F8);{void*_tmp8FC[1]={& _tmp8FE};Cyc_Tcutil_terr(loc,({
const char*_tmp8FD="field %s has already been used as an argument";_tag_arr(
_tmp8FD,sizeof(char),_get_zero_arr_size(_tmp8FD,46));}),_tag_arr(_tmp8FC,sizeof(
void*),1));}});(*((struct _tuple18*)_tmp8FB->hd)).f2=1;ans=({struct Cyc_List_List*
_tmp8FF=_region_malloc(rgn,sizeof(*_tmp8FF));_tmp8FF->hd=({struct _tuple20*
_tmp900=_region_malloc(rgn,sizeof(*_tmp900));_tmp900->f1=(*((struct _tuple18*)
_tmp8FB->hd)).f1;_tmp900->f2=_tmp8EC;_tmp900;});_tmp8FF->tl=ans;_tmp8FF;});
break;}}if(_tmp8FB == 0)({struct Cyc_String_pa_struct _tmp903;_tmp903.tag=0;_tmp903.f1=(
struct _tagged_arr)((struct _tagged_arr)*_tmp8F8);{void*_tmp901[1]={& _tmp903};Cyc_Tcutil_terr(
loc,({const char*_tmp902="bad field designator %s";_tag_arr(_tmp902,sizeof(char),
_get_zero_arr_size(_tmp902,24));}),_tag_arr(_tmp901,sizeof(void*),1));}});goto
_LL583;}_LL583:;}}}for(0;fields != 0;fields=fields->tl){if(!(*((struct _tuple18*)
fields->hd)).f2){({void*_tmp904[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp905="too few arguments to struct";
_tag_arr(_tmp905,sizeof(char),_get_zero_arr_size(_tmp905,28));}),_tag_arr(
_tmp904,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmp906=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp907;
void*_tmp908;struct Cyc_Absyn_PtrAtts _tmp909;struct Cyc_Absyn_Conref*_tmp90A;
_LL589: if(_tmp906 <= (void*)3?1:*((int*)_tmp906)!= 4)goto _LL58B;_tmp907=((struct
Cyc_Absyn_PointerType_struct*)_tmp906)->f1;_tmp908=(void*)_tmp907.elt_typ;
_tmp909=_tmp907.ptr_atts;_tmp90A=_tmp909.bounds;_LL58A: {struct Cyc_Absyn_Conref*
_tmp90B=Cyc_Absyn_compress_conref(_tmp90A);void*_tmp90C=(void*)(Cyc_Absyn_compress_conref(
_tmp90B))->v;void*_tmp90D;_LL58E: if(_tmp90C <= (void*)1?1:*((int*)_tmp90C)!= 0)
goto _LL590;_tmp90D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp90C)->f1;if((
int)_tmp90D != 0)goto _LL590;_LL58F:*elt_typ_dest=_tmp908;return 1;_LL590: if((int)
_tmp90C != 0)goto _LL592;_LL591:(void*)(_tmp90B->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp90E=_cycalloc(sizeof(*_tmp90E));_tmp90E[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp90F;_tmp90F.tag=0;_tmp90F.f1=(void*)((void*)0);_tmp90F;});_tmp90E;})));*
elt_typ_dest=_tmp908;return 1;_LL592:;_LL593: return 0;_LL58D:;}_LL58B:;_LL58C:
return 0;_LL588:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp910=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp911;void*_tmp912;
struct Cyc_Absyn_PtrAtts _tmp913;struct Cyc_Absyn_Conref*_tmp914;_LL595: if(_tmp910
<= (void*)3?1:*((int*)_tmp910)!= 4)goto _LL597;_tmp911=((struct Cyc_Absyn_PointerType_struct*)
_tmp910)->f1;_tmp912=(void*)_tmp911.elt_typ;_tmp913=_tmp911.ptr_atts;_tmp914=
_tmp913.zero_term;_LL596:*elt_typ_dest=_tmp912;return((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp914);_LL597:;_LL598: return 0;_LL594:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmp958;_tmp958.f1=0;
_tmp958.f2=(void*)2;_tmp958;});void*_tmp915=(void*)e->r;struct _tuple1*_tmp916;
void*_tmp917;struct Cyc_Absyn_Exp*_tmp918;struct _tagged_arr*_tmp919;struct Cyc_Absyn_Exp*
_tmp91A;struct _tagged_arr*_tmp91B;struct Cyc_Absyn_Exp*_tmp91C;struct Cyc_Absyn_Exp*
_tmp91D;struct Cyc_Absyn_Exp*_tmp91E;_LL59A: if(*((int*)_tmp915)!= 1)goto _LL59C;
_tmp916=((struct Cyc_Absyn_Var_e_struct*)_tmp915)->f1;_tmp917=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp915)->f2;_LL59B: {void*_tmp91F=_tmp917;struct Cyc_Absyn_Vardecl*_tmp920;
struct Cyc_Absyn_Vardecl*_tmp921;struct Cyc_Absyn_Vardecl*_tmp922;struct Cyc_Absyn_Vardecl*
_tmp923;_LL5A7: if((int)_tmp91F != 0)goto _LL5A9;_LL5A8: goto _LL5AA;_LL5A9: if(
_tmp91F <= (void*)1?1:*((int*)_tmp91F)!= 1)goto _LL5AB;_LL5AA: return bogus_ans;
_LL5AB: if(_tmp91F <= (void*)1?1:*((int*)_tmp91F)!= 0)goto _LL5AD;_tmp920=((struct
Cyc_Absyn_Global_b_struct*)_tmp91F)->f1;_LL5AC: {void*_tmp924=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL5B4: if(_tmp924 <= (void*)
3?1:*((int*)_tmp924)!= 7)goto _LL5B6;_LL5B5: return({struct _tuple10 _tmp925;_tmp925.f1=
1;_tmp925.f2=(void*)2;_tmp925;});_LL5B6:;_LL5B7: return({struct _tuple10 _tmp926;
_tmp926.f1=(_tmp920->tq).q_const;_tmp926.f2=(void*)2;_tmp926;});_LL5B3:;}_LL5AD:
if(_tmp91F <= (void*)1?1:*((int*)_tmp91F)!= 3)goto _LL5AF;_tmp921=((struct Cyc_Absyn_Local_b_struct*)
_tmp91F)->f1;_LL5AE: {void*_tmp927=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL5B9: if(_tmp927 <= (void*)3?1:*((int*)_tmp927)!= 7)
goto _LL5BB;_LL5BA: return({struct _tuple10 _tmp928;_tmp928.f1=1;_tmp928.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp921->rgn))->v;_tmp928;});_LL5BB:;_LL5BC:
_tmp921->escapes=1;return({struct _tuple10 _tmp929;_tmp929.f1=(_tmp921->tq).q_const;
_tmp929.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp921->rgn))->v;_tmp929;});
_LL5B8:;}_LL5AF: if(_tmp91F <= (void*)1?1:*((int*)_tmp91F)!= 4)goto _LL5B1;_tmp922=((
struct Cyc_Absyn_Pat_b_struct*)_tmp91F)->f1;_LL5B0: _tmp923=_tmp922;goto _LL5B2;
_LL5B1: if(_tmp91F <= (void*)1?1:*((int*)_tmp91F)!= 2)goto _LL5A6;_tmp923=((struct
Cyc_Absyn_Param_b_struct*)_tmp91F)->f1;_LL5B2: _tmp923->escapes=1;return({struct
_tuple10 _tmp92A;_tmp92A.f1=(_tmp923->tq).q_const;_tmp92A.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp923->rgn))->v;_tmp92A;});_LL5A6:;}_LL59C: if(*((int*)_tmp915)!= 
21)goto _LL59E;_tmp918=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp915)->f1;
_tmp919=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp915)->f2;_LL59D: {void*
_tmp92B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp918->topt))->v);
struct Cyc_List_List*_tmp92C;struct Cyc_Absyn_AggrInfo _tmp92D;void*_tmp92E;struct
Cyc_Absyn_Aggrdecl**_tmp92F;struct Cyc_Absyn_Aggrdecl*_tmp930;_LL5BE: if(_tmp92B <= (
void*)3?1:*((int*)_tmp92B)!= 11)goto _LL5C0;_tmp92C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp92B)->f2;_LL5BF: {struct Cyc_Absyn_Aggrfield*_tmp931=Cyc_Absyn_lookup_field(
_tmp92C,_tmp919);if(_tmp931 != 0?_tmp931->width != 0: 0)return({struct _tuple10
_tmp932;_tmp932.f1=(_tmp931->tq).q_const;_tmp932.f2=(Cyc_Tcutil_addressof_props(
te,_tmp918)).f2;_tmp932;});return bogus_ans;}_LL5C0: if(_tmp92B <= (void*)3?1:*((
int*)_tmp92B)!= 10)goto _LL5C2;_tmp92D=((struct Cyc_Absyn_AggrType_struct*)_tmp92B)->f1;
_tmp92E=(void*)_tmp92D.aggr_info;if(*((int*)_tmp92E)!= 1)goto _LL5C2;_tmp92F=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp92E)->f1;_tmp930=*_tmp92F;_LL5C1: {struct
Cyc_Absyn_Aggrfield*_tmp933=Cyc_Absyn_lookup_decl_field(_tmp930,_tmp919);if(
_tmp933 != 0?_tmp933->width != 0: 0)return({struct _tuple10 _tmp934;_tmp934.f1=(
_tmp933->tq).q_const;_tmp934.f2=(Cyc_Tcutil_addressof_props(te,_tmp918)).f2;
_tmp934;});return bogus_ans;}_LL5C2:;_LL5C3: return bogus_ans;_LL5BD:;}_LL59E: if(*((
int*)_tmp915)!= 22)goto _LL5A0;_tmp91A=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp915)->f1;_tmp91B=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp915)->f2;_LL59F: {
void*_tmp935=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp91A->topt))->v);struct Cyc_Absyn_PtrInfo _tmp936;void*_tmp937;struct Cyc_Absyn_PtrAtts
_tmp938;void*_tmp939;_LL5C5: if(_tmp935 <= (void*)3?1:*((int*)_tmp935)!= 4)goto
_LL5C7;_tmp936=((struct Cyc_Absyn_PointerType_struct*)_tmp935)->f1;_tmp937=(void*)
_tmp936.elt_typ;_tmp938=_tmp936.ptr_atts;_tmp939=(void*)_tmp938.rgn;_LL5C6: {
struct Cyc_Absyn_Aggrfield*finfo;{void*_tmp93A=Cyc_Tcutil_compress(_tmp937);
struct Cyc_List_List*_tmp93B;struct Cyc_Absyn_AggrInfo _tmp93C;void*_tmp93D;struct
Cyc_Absyn_Aggrdecl**_tmp93E;struct Cyc_Absyn_Aggrdecl*_tmp93F;_LL5CA: if(_tmp93A <= (
void*)3?1:*((int*)_tmp93A)!= 11)goto _LL5CC;_tmp93B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp93A)->f2;_LL5CB: finfo=Cyc_Absyn_lookup_field(_tmp93B,_tmp91B);goto _LL5C9;
_LL5CC: if(_tmp93A <= (void*)3?1:*((int*)_tmp93A)!= 10)goto _LL5CE;_tmp93C=((struct
Cyc_Absyn_AggrType_struct*)_tmp93A)->f1;_tmp93D=(void*)_tmp93C.aggr_info;if(*((
int*)_tmp93D)!= 1)goto _LL5CE;_tmp93E=((struct Cyc_Absyn_KnownAggr_struct*)_tmp93D)->f1;
_tmp93F=*_tmp93E;_LL5CD: finfo=Cyc_Absyn_lookup_decl_field(_tmp93F,_tmp91B);goto
_LL5C9;_LL5CE:;_LL5CF: return bogus_ans;_LL5C9:;}if(finfo != 0?finfo->width != 0: 0)
return({struct _tuple10 _tmp940;_tmp940.f1=(finfo->tq).q_const;_tmp940.f2=_tmp939;
_tmp940;});return bogus_ans;}_LL5C7:;_LL5C8: return bogus_ans;_LL5C4:;}_LL5A0: if(*((
int*)_tmp915)!= 20)goto _LL5A2;_tmp91C=((struct Cyc_Absyn_Deref_e_struct*)_tmp915)->f1;
_LL5A1: {void*_tmp941=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp91C->topt))->v);struct Cyc_Absyn_PtrInfo _tmp942;struct Cyc_Absyn_Tqual
_tmp943;struct Cyc_Absyn_PtrAtts _tmp944;void*_tmp945;_LL5D1: if(_tmp941 <= (void*)3?
1:*((int*)_tmp941)!= 4)goto _LL5D3;_tmp942=((struct Cyc_Absyn_PointerType_struct*)
_tmp941)->f1;_tmp943=_tmp942.elt_tq;_tmp944=_tmp942.ptr_atts;_tmp945=(void*)
_tmp944.rgn;_LL5D2: return({struct _tuple10 _tmp946;_tmp946.f1=_tmp943.q_const;
_tmp946.f2=_tmp945;_tmp946;});_LL5D3:;_LL5D4: return bogus_ans;_LL5D0:;}_LL5A2: if(*((
int*)_tmp915)!= 23)goto _LL5A4;_tmp91D=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp915)->f1;_tmp91E=((struct Cyc_Absyn_Subscript_e_struct*)_tmp915)->f2;_LL5A3: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp91D->topt))->v);
void*_tmp947=t;struct Cyc_List_List*_tmp948;struct Cyc_Absyn_PtrInfo _tmp949;struct
Cyc_Absyn_Tqual _tmp94A;struct Cyc_Absyn_PtrAtts _tmp94B;void*_tmp94C;struct Cyc_Absyn_ArrayInfo
_tmp94D;struct Cyc_Absyn_Tqual _tmp94E;_LL5D6: if(_tmp947 <= (void*)3?1:*((int*)
_tmp947)!= 9)goto _LL5D8;_tmp948=((struct Cyc_Absyn_TupleType_struct*)_tmp947)->f1;
_LL5D7: {unsigned int _tmp950;int _tmp951;struct _tuple7 _tmp94F=Cyc_Evexp_eval_const_uint_exp(
_tmp91E);_tmp950=_tmp94F.f1;_tmp951=_tmp94F.f2;if(!_tmp951)return bogus_ans;{
struct _tuple4*_tmp952=Cyc_Absyn_lookup_tuple_field(_tmp948,(int)_tmp950);if(
_tmp952 != 0)return({struct _tuple10 _tmp953;_tmp953.f1=((*_tmp952).f1).q_const;
_tmp953.f2=(Cyc_Tcutil_addressof_props(te,_tmp91D)).f2;_tmp953;});return
bogus_ans;}}_LL5D8: if(_tmp947 <= (void*)3?1:*((int*)_tmp947)!= 4)goto _LL5DA;
_tmp949=((struct Cyc_Absyn_PointerType_struct*)_tmp947)->f1;_tmp94A=_tmp949.elt_tq;
_tmp94B=_tmp949.ptr_atts;_tmp94C=(void*)_tmp94B.rgn;_LL5D9: return({struct
_tuple10 _tmp954;_tmp954.f1=_tmp94A.q_const;_tmp954.f2=_tmp94C;_tmp954;});_LL5DA:
if(_tmp947 <= (void*)3?1:*((int*)_tmp947)!= 7)goto _LL5DC;_tmp94D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp947)->f1;_tmp94E=_tmp94D.tq;_LL5DB: return({struct _tuple10 _tmp955;_tmp955.f1=
_tmp94E.q_const;_tmp955.f2=(Cyc_Tcutil_addressof_props(te,_tmp91D)).f2;_tmp955;});
_LL5DC:;_LL5DD: return bogus_ans;_LL5D5:;}_LL5A4:;_LL5A5:({void*_tmp956[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp957="unary & applied to non-lvalue";_tag_arr(_tmp957,
sizeof(char),_get_zero_arr_size(_tmp957,30));}),_tag_arr(_tmp956,sizeof(void*),0));});
return bogus_ans;_LL599:;}void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,
void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmp959=Cyc_Tcutil_compress(e_typ);struct
Cyc_Absyn_ArrayInfo _tmp95A;void*_tmp95B;struct Cyc_Absyn_Tqual _tmp95C;struct Cyc_Absyn_Conref*
_tmp95D;_LL5DF: if(_tmp959 <= (void*)3?1:*((int*)_tmp959)!= 7)goto _LL5E1;_tmp95A=((
struct Cyc_Absyn_ArrayType_struct*)_tmp959)->f1;_tmp95B=(void*)_tmp95A.elt_type;
_tmp95C=_tmp95A.tq;_tmp95D=_tmp95A.zero_term;_LL5E0: {void*_tmp95F;struct
_tuple10 _tmp95E=Cyc_Tcutil_addressof_props(te,e);_tmp95F=_tmp95E.f2;return Cyc_Absyn_atb_typ(
_tmp95B,_tmp95F,_tmp95C,(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp960=
_cycalloc(sizeof(*_tmp960));_tmp960[0]=({struct Cyc_Absyn_Upper_b_struct _tmp961;
_tmp961.tag=0;_tmp961.f1=e;_tmp961;});_tmp960;}),_tmp95D);}_LL5E1:;_LL5E2: return
e_typ;_LL5DE:;}void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,
unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(b);{void*
_tmp962=(void*)b->v;void*_tmp963;void*_tmp964;struct Cyc_Absyn_Exp*_tmp965;void*
_tmp966;_LL5E4: if(_tmp962 <= (void*)1?1:*((int*)_tmp962)!= 0)goto _LL5E6;_tmp963=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp962)->f1;if((int)_tmp963 != 0)goto
_LL5E6;_LL5E5: return;_LL5E6: if(_tmp962 <= (void*)1?1:*((int*)_tmp962)!= 0)goto
_LL5E8;_tmp964=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp962)->f1;if(
_tmp964 <= (void*)1?1:*((int*)_tmp964)!= 0)goto _LL5E8;_tmp965=((struct Cyc_Absyn_Upper_b_struct*)
_tmp964)->f1;_LL5E7: {unsigned int _tmp968;int _tmp969;struct _tuple7 _tmp967=Cyc_Evexp_eval_const_uint_exp(
_tmp965);_tmp968=_tmp967.f1;_tmp969=_tmp967.f2;if(_tmp969?_tmp968 <= i: 0)({struct
Cyc_Int_pa_struct _tmp96D;_tmp96D.tag=1;_tmp96D.f1=(unsigned int)((int)i);{struct
Cyc_Int_pa_struct _tmp96C;_tmp96C.tag=1;_tmp96C.f1=(unsigned int)((int)_tmp968);{
void*_tmp96A[2]={& _tmp96C,& _tmp96D};Cyc_Tcutil_terr(loc,({const char*_tmp96B="dereference is out of bounds: %d <= %d";
_tag_arr(_tmp96B,sizeof(char),_get_zero_arr_size(_tmp96B,39));}),_tag_arr(
_tmp96A,sizeof(void*),2));}}});return;}_LL5E8: if(_tmp962 <= (void*)1?1:*((int*)
_tmp962)!= 0)goto _LL5EA;_tmp966=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp962)->f1;if(_tmp966 <= (void*)1?1:*((int*)_tmp966)!= 1)goto _LL5EA;_LL5E9:
return;_LL5EA: if((int)_tmp962 != 0)goto _LL5EC;_LL5EB:(void*)(b->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp96E=_cycalloc(sizeof(*_tmp96E));_tmp96E[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp96F;_tmp96F.tag=0;_tmp96F.f1=(void*)((void*)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp970=_cycalloc(sizeof(*_tmp970));
_tmp970[0]=({struct Cyc_Absyn_Upper_b_struct _tmp971;_tmp971.tag=0;_tmp971.f1=Cyc_Absyn_uint_exp(
i + 1,0);_tmp971;});_tmp970;})));_tmp96F;});_tmp96E;})));return;_LL5EC:;_LL5ED:({
void*_tmp972[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp973="check_bound -- bad compressed conref";_tag_arr(_tmp973,
sizeof(char),_get_zero_arr_size(_tmp973,37));}),_tag_arr(_tmp972,sizeof(void*),0));});
_LL5E3:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){void*_tmp974=(void*)(Cyc_Absyn_compress_conref(b))->v;
void*_tmp975;struct Cyc_Absyn_Exp*_tmp976;_LL5EF: if(_tmp974 <= (void*)1?1:*((int*)
_tmp974)!= 0)goto _LL5F1;_tmp975=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp974)->f1;if(_tmp975 <= (void*)1?1:*((int*)_tmp975)!= 0)goto _LL5F1;_tmp976=((
struct Cyc_Absyn_Upper_b_struct*)_tmp975)->f1;_LL5F0: {unsigned int _tmp978;int
_tmp979;struct _tuple7 _tmp977=Cyc_Evexp_eval_const_uint_exp(_tmp976);_tmp978=
_tmp977.f1;_tmp979=_tmp977.f2;return _tmp979?_tmp978 == 1: 0;}_LL5F1:;_LL5F2: return
0;_LL5EE:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp97A=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_ArrayInfo _tmp97B;void*_tmp97C;struct Cyc_Absyn_Conref*_tmp97D;
struct Cyc_List_List*_tmp97E;struct Cyc_Absyn_AggrInfo _tmp97F;void*_tmp980;struct
Cyc_Absyn_AggrInfo _tmp981;void*_tmp982;struct Cyc_Absyn_Aggrdecl**_tmp983;struct
Cyc_Absyn_Aggrdecl*_tmp984;struct Cyc_List_List*_tmp985;struct Cyc_List_List*
_tmp986;_LL5F4: if((int)_tmp97A != 0)goto _LL5F6;_LL5F5: goto _LL5F7;_LL5F6: if(
_tmp97A <= (void*)3?1:*((int*)_tmp97A)!= 5)goto _LL5F8;_LL5F7: goto _LL5F9;_LL5F8:
if((int)_tmp97A != 1)goto _LL5FA;_LL5F9: goto _LL5FB;_LL5FA: if(_tmp97A <= (void*)3?1:*((
int*)_tmp97A)!= 6)goto _LL5FC;_LL5FB: return 1;_LL5FC: if(_tmp97A <= (void*)3?1:*((
int*)_tmp97A)!= 12)goto _LL5FE;_LL5FD: goto _LL5FF;_LL5FE: if(_tmp97A <= (void*)3?1:*((
int*)_tmp97A)!= 13)goto _LL600;_LL5FF: return 0;_LL600: if(_tmp97A <= (void*)3?1:*((
int*)_tmp97A)!= 7)goto _LL602;_tmp97B=((struct Cyc_Absyn_ArrayType_struct*)_tmp97A)->f1;
_tmp97C=(void*)_tmp97B.elt_type;_tmp97D=_tmp97B.zero_term;_LL601: return !((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp97D)?Cyc_Tcutil_bits_only(
_tmp97C): 0;_LL602: if(_tmp97A <= (void*)3?1:*((int*)_tmp97A)!= 9)goto _LL604;
_tmp97E=((struct Cyc_Absyn_TupleType_struct*)_tmp97A)->f1;_LL603: for(0;_tmp97E != 
0;_tmp97E=_tmp97E->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple4*)_tmp97E->hd)).f2))
return 0;}return 1;_LL604: if(_tmp97A <= (void*)3?1:*((int*)_tmp97A)!= 10)goto _LL606;
_tmp97F=((struct Cyc_Absyn_AggrType_struct*)_tmp97A)->f1;_tmp980=(void*)_tmp97F.aggr_info;
if(*((int*)_tmp980)!= 0)goto _LL606;_LL605: return 0;_LL606: if(_tmp97A <= (void*)3?1:*((
int*)_tmp97A)!= 10)goto _LL608;_tmp981=((struct Cyc_Absyn_AggrType_struct*)_tmp97A)->f1;
_tmp982=(void*)_tmp981.aggr_info;if(*((int*)_tmp982)!= 1)goto _LL608;_tmp983=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp982)->f1;_tmp984=*_tmp983;_tmp985=_tmp981.targs;
_LL607: if(_tmp984->impl == 0)return 0;{struct _RegionHandle _tmp987=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp987;_push_region(rgn);{struct Cyc_List_List*_tmp988=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp984->tvs,_tmp985);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp984->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp988,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp989=0;_npop_handler(0);
return _tmp989;}}}{int _tmp98A=1;_npop_handler(0);return _tmp98A;}};_pop_region(rgn);}
_LL608: if(_tmp97A <= (void*)3?1:*((int*)_tmp97A)!= 11)goto _LL60A;_tmp986=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp97A)->f2;_LL609: for(0;_tmp986 != 0;_tmp986=
_tmp986->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp986->hd)->type))return 0;}return 1;_LL60A:;_LL60B: return 0;_LL5F3:;}struct
_tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmp98B=(void*)e->r;
struct _tuple1*_tmp98C;void*_tmp98D;struct Cyc_Absyn_Exp*_tmp98E;struct Cyc_Absyn_Exp*
_tmp98F;struct Cyc_Absyn_Exp*_tmp990;struct Cyc_Absyn_Exp*_tmp991;struct Cyc_Absyn_Exp*
_tmp992;struct Cyc_Absyn_Exp*_tmp993;struct Cyc_Absyn_Exp*_tmp994;void*_tmp995;
struct Cyc_Absyn_Exp*_tmp996;void*_tmp997;void*_tmp998;struct Cyc_Absyn_Exp*
_tmp999;struct Cyc_Absyn_Exp*_tmp99A;struct Cyc_Absyn_Exp*_tmp99B;struct Cyc_Absyn_Exp*
_tmp99C;struct Cyc_List_List*_tmp99D;struct Cyc_List_List*_tmp99E;struct Cyc_List_List*
_tmp99F;void*_tmp9A0;struct Cyc_List_List*_tmp9A1;struct Cyc_List_List*_tmp9A2;
struct Cyc_List_List*_tmp9A3;_LL60D: if(*((int*)_tmp98B)!= 0)goto _LL60F;_LL60E:
goto _LL610;_LL60F: if(*((int*)_tmp98B)!= 16)goto _LL611;_LL610: goto _LL612;_LL611:
if(*((int*)_tmp98B)!= 17)goto _LL613;_LL612: goto _LL614;_LL613: if(*((int*)_tmp98B)
!= 18)goto _LL615;_LL614: goto _LL616;_LL615: if(*((int*)_tmp98B)!= 19)goto _LL617;
_LL616: goto _LL618;_LL617: if(*((int*)_tmp98B)!= 31)goto _LL619;_LL618: goto _LL61A;
_LL619: if(*((int*)_tmp98B)!= 32)goto _LL61B;_LL61A: return 1;_LL61B: if(*((int*)
_tmp98B)!= 1)goto _LL61D;_tmp98C=((struct Cyc_Absyn_Var_e_struct*)_tmp98B)->f1;
_tmp98D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp98B)->f2;_LL61C: {void*
_tmp9A4=_tmp98D;struct Cyc_Absyn_Vardecl*_tmp9A5;_LL63C: if(_tmp9A4 <= (void*)1?1:*((
int*)_tmp9A4)!= 1)goto _LL63E;_LL63D: return 1;_LL63E: if(_tmp9A4 <= (void*)1?1:*((
int*)_tmp9A4)!= 0)goto _LL640;_tmp9A5=((struct Cyc_Absyn_Global_b_struct*)_tmp9A4)->f1;
_LL63F: {void*_tmp9A6=Cyc_Tcutil_compress((void*)_tmp9A5->type);_LL645: if(
_tmp9A6 <= (void*)3?1:*((int*)_tmp9A6)!= 7)goto _LL647;_LL646: goto _LL648;_LL647:
if(_tmp9A6 <= (void*)3?1:*((int*)_tmp9A6)!= 8)goto _LL649;_LL648: return 1;_LL649:;
_LL64A: return var_okay;_LL644:;}_LL640: if((int)_tmp9A4 != 0)goto _LL642;_LL641:
return 0;_LL642:;_LL643: return var_okay;_LL63B:;}_LL61D: if(*((int*)_tmp98B)!= 6)
goto _LL61F;_tmp98E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp98B)->f1;_tmp98F=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp98B)->f2;_tmp990=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp98B)->f3;_LL61E: return(Cyc_Tcutil_cnst_exp(te,0,_tmp98E)?Cyc_Tcutil_cnst_exp(
te,0,_tmp98F): 0)?Cyc_Tcutil_cnst_exp(te,0,_tmp990): 0;_LL61F: if(*((int*)_tmp98B)
!= 7)goto _LL621;_tmp991=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp98B)->f1;_tmp992=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp98B)->f2;_LL620: return Cyc_Tcutil_cnst_exp(
te,0,_tmp991)?Cyc_Tcutil_cnst_exp(te,0,_tmp992): 0;_LL621: if(*((int*)_tmp98B)!= 
11)goto _LL623;_tmp993=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp98B)->f1;
_LL622: _tmp994=_tmp993;goto _LL624;_LL623: if(*((int*)_tmp98B)!= 12)goto _LL625;
_tmp994=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp98B)->f1;_LL624: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp994);_LL625: if(*((int*)_tmp98B)!= 13)goto _LL627;_tmp995=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp98B)->f1;_tmp996=((struct Cyc_Absyn_Cast_e_struct*)
_tmp98B)->f2;_tmp997=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp98B)->f4;if((
int)_tmp997 != 1)goto _LL627;_LL626: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp996);
_LL627: if(*((int*)_tmp98B)!= 13)goto _LL629;_tmp998=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp98B)->f1;_tmp999=((struct Cyc_Absyn_Cast_e_struct*)_tmp98B)->f2;_LL628: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmp999);_LL629: if(*((int*)_tmp98B)!= 14)goto
_LL62B;_tmp99A=((struct Cyc_Absyn_Address_e_struct*)_tmp98B)->f1;_LL62A: return Cyc_Tcutil_cnst_exp(
te,1,_tmp99A);_LL62B: if(*((int*)_tmp98B)!= 27)goto _LL62D;_tmp99B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp98B)->f2;_tmp99C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp98B)->f3;
_LL62C: return Cyc_Tcutil_cnst_exp(te,0,_tmp99B)?Cyc_Tcutil_cnst_exp(te,0,_tmp99C):
0;_LL62D: if(*((int*)_tmp98B)!= 26)goto _LL62F;_tmp99D=((struct Cyc_Absyn_Array_e_struct*)
_tmp98B)->f1;_LL62E: _tmp99E=_tmp99D;goto _LL630;_LL62F: if(*((int*)_tmp98B)!= 29)
goto _LL631;_tmp99E=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp98B)->f2;_LL630:
_tmp99F=_tmp99E;goto _LL632;_LL631: if(*((int*)_tmp98B)!= 28)goto _LL633;_tmp99F=((
struct Cyc_Absyn_Struct_e_struct*)_tmp98B)->f3;_LL632: for(0;_tmp99F != 0;_tmp99F=
_tmp99F->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple21*)_tmp99F->hd)).f2))
return 0;}return 1;_LL633: if(*((int*)_tmp98B)!= 3)goto _LL635;_tmp9A0=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp98B)->f1;_tmp9A1=((struct Cyc_Absyn_Primop_e_struct*)
_tmp98B)->f2;_LL634: _tmp9A2=_tmp9A1;goto _LL636;_LL635: if(*((int*)_tmp98B)!= 24)
goto _LL637;_tmp9A2=((struct Cyc_Absyn_Tuple_e_struct*)_tmp98B)->f1;_LL636: _tmp9A3=
_tmp9A2;goto _LL638;_LL637: if(*((int*)_tmp98B)!= 30)goto _LL639;_tmp9A3=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp98B)->f1;_LL638: for(0;_tmp9A3 != 0;_tmp9A3=_tmp9A3->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp9A3->hd))return 0;}return 1;
_LL639:;_LL63A: return 0;_LL60C:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp9A7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9A8;
struct Cyc_Absyn_PtrAtts _tmp9A9;struct Cyc_Absyn_Conref*_tmp9AA;struct Cyc_Absyn_Conref*
_tmp9AB;struct Cyc_Absyn_ArrayInfo _tmp9AC;void*_tmp9AD;struct Cyc_List_List*
_tmp9AE;struct Cyc_Absyn_AggrInfo _tmp9AF;void*_tmp9B0;struct Cyc_List_List*_tmp9B1;
struct Cyc_List_List*_tmp9B2;_LL64C: if((int)_tmp9A7 != 0)goto _LL64E;_LL64D: goto
_LL64F;_LL64E: if(_tmp9A7 <= (void*)3?1:*((int*)_tmp9A7)!= 5)goto _LL650;_LL64F:
goto _LL651;_LL650: if((int)_tmp9A7 != 1)goto _LL652;_LL651: goto _LL653;_LL652: if(
_tmp9A7 <= (void*)3?1:*((int*)_tmp9A7)!= 6)goto _LL654;_LL653: return 1;_LL654: if(
_tmp9A7 <= (void*)3?1:*((int*)_tmp9A7)!= 4)goto _LL656;_tmp9A8=((struct Cyc_Absyn_PointerType_struct*)
_tmp9A7)->f1;_tmp9A9=_tmp9A8.ptr_atts;_tmp9AA=_tmp9A9.nullable;_tmp9AB=_tmp9A9.bounds;
_LL655: {void*_tmp9B3=(void*)(Cyc_Absyn_compress_conref(_tmp9AB))->v;void*
_tmp9B4;_LL665: if(_tmp9B3 <= (void*)1?1:*((int*)_tmp9B3)!= 0)goto _LL667;_tmp9B4=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9B3)->f1;if((int)_tmp9B4 != 0)goto
_LL667;_LL666: return 1;_LL667: if(_tmp9B3 <= (void*)1?1:*((int*)_tmp9B3)!= 0)goto
_LL669;_LL668: {void*_tmp9B5=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmp9AA))->v;int _tmp9B6;_LL66C: if(_tmp9B5 <= (void*)
1?1:*((int*)_tmp9B5)!= 0)goto _LL66E;_tmp9B6=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp9B5)->f1;_LL66D: return _tmp9B6;_LL66E:;_LL66F: return 0;_LL66B:;}_LL669:;_LL66A:
return 0;_LL664:;}_LL656: if(_tmp9A7 <= (void*)3?1:*((int*)_tmp9A7)!= 7)goto _LL658;
_tmp9AC=((struct Cyc_Absyn_ArrayType_struct*)_tmp9A7)->f1;_tmp9AD=(void*)_tmp9AC.elt_type;
_LL657: return Cyc_Tcutil_supports_default(_tmp9AD);_LL658: if(_tmp9A7 <= (void*)3?1:*((
int*)_tmp9A7)!= 9)goto _LL65A;_tmp9AE=((struct Cyc_Absyn_TupleType_struct*)_tmp9A7)->f1;
_LL659: for(0;_tmp9AE != 0;_tmp9AE=_tmp9AE->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple4*)_tmp9AE->hd)).f2))return 0;}return 1;_LL65A: if(_tmp9A7 <= (void*)3?1:*((
int*)_tmp9A7)!= 10)goto _LL65C;_tmp9AF=((struct Cyc_Absyn_AggrType_struct*)_tmp9A7)->f1;
_tmp9B0=(void*)_tmp9AF.aggr_info;_tmp9B1=_tmp9AF.targs;_LL65B: {struct Cyc_Absyn_Aggrdecl*
_tmp9B7=Cyc_Absyn_get_known_aggrdecl(_tmp9B0);if(_tmp9B7->impl == 0)return 0;if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9B7->impl))->exist_vars != 0)return
0;return Cyc_Tcutil_fields_support_default(_tmp9B7->tvs,_tmp9B1,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp9B7->impl))->fields);}_LL65C: if(_tmp9A7 <= (void*)3?1:*((int*)
_tmp9A7)!= 11)goto _LL65E;_tmp9B2=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp9A7)->f2;
_LL65D: return Cyc_Tcutil_fields_support_default(0,0,_tmp9B2);_LL65E: if(_tmp9A7 <= (
void*)3?1:*((int*)_tmp9A7)!= 13)goto _LL660;_LL65F: goto _LL661;_LL660: if(_tmp9A7 <= (
void*)3?1:*((int*)_tmp9A7)!= 12)goto _LL662;_LL661: return 1;_LL662:;_LL663: return 0;
_LL64B:;}static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,
struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmp9B8=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp9B8;_push_region(rgn);{struct Cyc_List_List*
_tmp9B9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(
0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp9B9,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type);if(!Cyc_Tcutil_supports_default(t)){int _tmp9BA=0;_npop_handler(0);
return _tmp9BA;}}};_pop_region(rgn);}return 1;}int Cyc_Tcutil_admits_zero(void*t){
void*_tmp9BB=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9BC;struct Cyc_Absyn_PtrAtts
_tmp9BD;struct Cyc_Absyn_Conref*_tmp9BE;struct Cyc_Absyn_Conref*_tmp9BF;_LL671: if(
_tmp9BB <= (void*)3?1:*((int*)_tmp9BB)!= 5)goto _LL673;_LL672: goto _LL674;_LL673:
if((int)_tmp9BB != 1)goto _LL675;_LL674: goto _LL676;_LL675: if(_tmp9BB <= (void*)3?1:*((
int*)_tmp9BB)!= 6)goto _LL677;_LL676: return 1;_LL677: if(_tmp9BB <= (void*)3?1:*((
int*)_tmp9BB)!= 4)goto _LL679;_tmp9BC=((struct Cyc_Absyn_PointerType_struct*)
_tmp9BB)->f1;_tmp9BD=_tmp9BC.ptr_atts;_tmp9BE=_tmp9BD.nullable;_tmp9BF=_tmp9BD.bounds;
_LL678: {void*_tmp9C0=(void*)(Cyc_Absyn_compress_conref(_tmp9BF))->v;void*
_tmp9C1;_LL67C: if(_tmp9C0 <= (void*)1?1:*((int*)_tmp9C0)!= 0)goto _LL67E;_tmp9C1=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9C0)->f1;if((int)_tmp9C1 != 0)goto
_LL67E;_LL67D: return 0;_LL67E: if(_tmp9C0 <= (void*)1?1:*((int*)_tmp9C0)!= 0)goto
_LL680;_LL67F: {void*_tmp9C2=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmp9BE))->v;int _tmp9C3;_LL683: if(_tmp9C2 <= (void*)
1?1:*((int*)_tmp9C2)!= 0)goto _LL685;_tmp9C3=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp9C2)->f1;_LL684: return _tmp9C3;_LL685:;_LL686: return 0;_LL682:;}_LL680:;_LL681:
return 0;_LL67B:;}_LL679:;_LL67A: return 0;_LL670:;}int Cyc_Tcutil_is_noreturn(void*
t){{void*_tmp9C4=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9C5;void*
_tmp9C6;struct Cyc_Absyn_FnInfo _tmp9C7;struct Cyc_List_List*_tmp9C8;_LL688: if(
_tmp9C4 <= (void*)3?1:*((int*)_tmp9C4)!= 4)goto _LL68A;_tmp9C5=((struct Cyc_Absyn_PointerType_struct*)
_tmp9C4)->f1;_tmp9C6=(void*)_tmp9C5.elt_typ;_LL689: return Cyc_Tcutil_is_noreturn(
_tmp9C6);_LL68A: if(_tmp9C4 <= (void*)3?1:*((int*)_tmp9C4)!= 8)goto _LL68C;_tmp9C7=((
struct Cyc_Absyn_FnType_struct*)_tmp9C4)->f1;_tmp9C8=_tmp9C7.attributes;_LL68B:
for(0;_tmp9C8 != 0;_tmp9C8=_tmp9C8->tl){void*_tmp9C9=(void*)_tmp9C8->hd;_LL68F:
if((int)_tmp9C9 != 3)goto _LL691;_LL690: return 1;_LL691:;_LL692: continue;_LL68E:;}
goto _LL687;_LL68C:;_LL68D: goto _LL687;_LL687:;}return 0;}
