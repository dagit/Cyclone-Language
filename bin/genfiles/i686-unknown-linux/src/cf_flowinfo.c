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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);void*Cyc_List_hd(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tl(
struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(
void(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[8];int Cyc_List_exists_c(
int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);int Cyc_List_mem(int(*
compare)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];struct Cyc_Iter_Iter
Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);typedef struct{int
__count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef
struct{int __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _tagged_arr,struct
_tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);void*Cyc_Dict_fold_c(void*(*f)(
void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d,void*accum);void Cyc_Dict_iter_c(
void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*
Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*
Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2);struct Cyc_Dict_Dict*Cyc_Dict_intersect_c(void*(*f)(
void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);
int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d1,
struct Cyc_Dict_Dict*d2);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);int Cyc_strptrcmp(struct _tagged_arr*
s1,struct _tagged_arr*s2);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;
struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct
Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*
seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};
struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int
q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
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
Cyc_Absyn_UnknownTunionInfo{struct _tuple1*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{
int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{
int tag;struct Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*
tunion_info;struct Cyc_List_List*targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple1*tunion_name;struct _tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_tvar_cmp(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);struct _tagged_arr*Cyc_Absyn_fieldname(int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
void*info);int Cyc_Absyn_is_union_type(void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
ae;struct Cyc_Core_Opt*le;};void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_typecmp(void*,void*);int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple1*);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};char Cyc_CfFlowInfo_HasTagCmps[
15]="\000\000\000\000HasTagCmps\000";struct Cyc_CfFlowInfo_HasTagCmps_struct{char*
tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_IsZero[11]="\000\000\000\000IsZero\000";
char Cyc_CfFlowInfo_NotZero[12]="\000\000\000\000NotZero\000";struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[13]="\000\000\000\000UnknownZ\000";
struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int
tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct
Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict*
f1;struct Cyc_List_List*f2;};struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();
extern void*Cyc_CfFlowInfo_unknown_none;extern void*Cyc_CfFlowInfo_unknown_this;
extern void*Cyc_CfFlowInfo_unknown_all;extern void*Cyc_CfFlowInfo_esc_none;extern
void*Cyc_CfFlowInfo_esc_this;extern void*Cyc_CfFlowInfo_esc_all;int Cyc_CfFlowInfo_root_cmp(
void*,void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict*d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(void*
f1,void*f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,
struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,void*r);struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,
struct Cyc_Set_Set**all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**,void*,void*);struct _tuple0 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_Set_Set**all_changed,struct _tuple0 pr1,struct _tuple0 pr2);void*Cyc_CfFlowInfo_after_flow(
struct Cyc_Set_Set**,void*,void*,struct Cyc_Set_Set*,struct Cyc_Set_Set*);void*Cyc_CfFlowInfo_kill_flow_region(
void*f,void*rgn);static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_none_v={
0,(void*)((void*)0)};static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_this_v={
0,(void*)((void*)1)};static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_all_v={
0,(void*)((void*)2)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_none_v={
1,(void*)((void*)0)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_this_v={
1,(void*)((void*)1)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_all_v={
1,(void*)((void*)2)};void*Cyc_CfFlowInfo_unknown_none=(void*)& Cyc_CfFlowInfo_unknown_none_v;
void*Cyc_CfFlowInfo_unknown_this=(void*)& Cyc_CfFlowInfo_unknown_this_v;void*Cyc_CfFlowInfo_unknown_all=(
void*)& Cyc_CfFlowInfo_unknown_all_v;void*Cyc_CfFlowInfo_esc_none=(void*)& Cyc_CfFlowInfo_esc_none_v;
void*Cyc_CfFlowInfo_esc_this=(void*)& Cyc_CfFlowInfo_esc_this_v;void*Cyc_CfFlowInfo_esc_all=(
void*)& Cyc_CfFlowInfo_esc_all_v;struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set(){
static struct Cyc_Set_Set**mt_place_set_opt=0;if(mt_place_set_opt == 0)
mt_place_set_opt=({struct Cyc_Set_Set**_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Set_empty)(Cyc_CfFlowInfo_place_cmp);_tmp6;});return*mt_place_set_opt;}int
Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{struct
_tuple0 _tmp8=({struct _tuple0 _tmp7;_tmp7.f1=r1;_tmp7.f2=r2;_tmp7;});void*_tmp9;
struct Cyc_Absyn_Vardecl*_tmpA;void*_tmpB;struct Cyc_Absyn_Vardecl*_tmpC;void*
_tmpD;void*_tmpE;void*_tmpF;struct Cyc_Absyn_Exp*_tmp10;void*_tmp11;struct Cyc_Absyn_Exp*
_tmp12;void*_tmp13;void*_tmp14;void*_tmp15;int _tmp16;void*_tmp17;int _tmp18;_LL1:
_tmp9=_tmp8.f1;if(*((int*)_tmp9)!= 0)goto _LL3;_tmpA=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp9)->f1;_tmpB=_tmp8.f2;if(*((int*)_tmpB)!= 0)goto _LL3;_tmpC=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmpB)->f1;_LL2: return(int)_tmpA - (int)_tmpC;_LL3: _tmpD=_tmp8.f1;if(*((int*)
_tmpD)!= 0)goto _LL5;_LL4: return - 1;_LL5: _tmpE=_tmp8.f2;if(*((int*)_tmpE)!= 0)goto
_LL7;_LL6: return 1;_LL7: _tmpF=_tmp8.f1;if(*((int*)_tmpF)!= 1)goto _LL9;_tmp10=((
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmpF)->f1;_tmp11=_tmp8.f2;if(*((int*)
_tmp11)!= 1)goto _LL9;_tmp12=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp11)->f1;
_LL8: return(int)_tmp10 - (int)_tmp12;_LL9: _tmp13=_tmp8.f1;if(*((int*)_tmp13)!= 1)
goto _LLB;_LLA: return - 1;_LLB: _tmp14=_tmp8.f2;if(*((int*)_tmp14)!= 1)goto _LLD;_LLC:
return 1;_LLD: _tmp15=_tmp8.f1;if(*((int*)_tmp15)!= 2)goto _LL0;_tmp16=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp15)->f1;_tmp17=_tmp8.f2;if(*((int*)_tmp17)!= 2)goto _LL0;_tmp18=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp17)->f1;_LLE: return _tmp16 - _tmp18;_LL0:;}}int Cyc_CfFlowInfo_place_cmp(struct
Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)p2)
return 0;{int i=Cyc_CfFlowInfo_root_cmp((void*)p1->root,(void*)p2->root);if(i != 0)
return i;return((int(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,p1->fields,
p2->fields);}}static struct _tagged_arr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*
p){struct Cyc_List_List*sl=0;{void*_tmp19=(void*)p->root;struct Cyc_Absyn_Vardecl*
_tmp1A;struct Cyc_Absyn_Exp*_tmp1B;int _tmp1C;_LL10: if(*((int*)_tmp19)!= 0)goto
_LL12;_tmp1A=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp19)->f1;_LL11: sl=({
struct Cyc_List_List*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->hd=({struct
_tagged_arr*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=({struct Cyc_String_pa_struct
_tmp21;_tmp21.tag=0;_tmp21.f1=(struct _tagged_arr)((struct _tagged_arr)*(*_tmp1A->name).f2);{
void*_tmp1F[1]={& _tmp21};Cyc_aprintf(({const char*_tmp20="%s";_tag_arr(_tmp20,
sizeof(char),_get_zero_arr_size(_tmp20,3));}),_tag_arr(_tmp1F,sizeof(void*),1));}});
_tmp1E;});_tmp1D->tl=sl;_tmp1D;});goto _LLF;_LL12: if(*((int*)_tmp19)!= 1)goto
_LL14;_tmp1B=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp19)->f1;_LL13: sl=({
struct Cyc_List_List*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->hd=({struct
_tagged_arr*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23[0]=({struct Cyc_Int_pa_struct
_tmp26;_tmp26.tag=1;_tmp26.f1=(unsigned int)((int)_tmp1B);{void*_tmp24[1]={&
_tmp26};Cyc_aprintf(({const char*_tmp25="mpt%d";_tag_arr(_tmp25,sizeof(char),
_get_zero_arr_size(_tmp25,6));}),_tag_arr(_tmp24,sizeof(void*),1));}});_tmp23;});
_tmp22->tl=sl;_tmp22;});goto _LLF;_LL14: if(*((int*)_tmp19)!= 2)goto _LLF;_tmp1C=((
struct Cyc_CfFlowInfo_InitParam_struct*)_tmp19)->f1;_LL15: sl=({struct Cyc_List_List*
_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->hd=({struct _tagged_arr*_tmp28=
_cycalloc(sizeof(*_tmp28));_tmp28[0]=({struct Cyc_Int_pa_struct _tmp2B;_tmp2B.tag=
1;_tmp2B.f1=(unsigned int)_tmp1C;{void*_tmp29[1]={& _tmp2B};Cyc_aprintf(({const
char*_tmp2A="param%d";_tag_arr(_tmp2A,sizeof(char),_get_zero_arr_size(_tmp2A,8));}),
_tag_arr(_tmp29,sizeof(void*),1));}});_tmp28;});_tmp27->tl=sl;_tmp27;});goto _LLF;
_LLF:;}{struct Cyc_List_List*fields=p->fields;for(0;fields != 0;fields=fields->tl){
sl=({struct Cyc_List_List*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->hd=({struct
_tagged_arr*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D[0]=({struct Cyc_String_pa_struct
_tmp30;_tmp30.tag=0;_tmp30.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct
_tagged_arr*)fields->hd));{void*_tmp2E[1]={& _tmp30};Cyc_aprintf(({const char*
_tmp2F="%s";_tag_arr(_tmp2F,sizeof(char),_get_zero_arr_size(_tmp2F,3));}),
_tag_arr(_tmp2E,sizeof(void*),1));}});_tmp2D;});_tmp2C->tl=sl;_tmp2C;});}}{
struct _tagged_arr*_tmp31=({struct _tagged_arr*_tmp36=_cycalloc(sizeof(*_tmp36));
_tmp36[0]=({struct Cyc_String_pa_struct _tmp39;_tmp39.tag=0;_tmp39.f1=(struct
_tagged_arr)({const char*_tmp3A="";_tag_arr(_tmp3A,sizeof(char),
_get_zero_arr_size(_tmp3A,1));});{void*_tmp37[1]={& _tmp39};Cyc_aprintf(({const
char*_tmp38="%s";_tag_arr(_tmp38,sizeof(char),_get_zero_arr_size(_tmp38,3));}),
_tag_arr(_tmp37,sizeof(void*),1));}});_tmp36;});for(0;sl != 0;sl=sl->tl){*_tmp31=({
struct Cyc_String_pa_struct _tmp35;_tmp35.tag=0;_tmp35.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp31);{struct Cyc_String_pa_struct _tmp34;_tmp34.tag=0;_tmp34.f1=(
struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)sl->hd));{void*
_tmp32[2]={& _tmp34,& _tmp35};Cyc_aprintf(({const char*_tmp33="%s.%s";_tag_arr(
_tmp33,sizeof(char),_get_zero_arr_size(_tmp33,6));}),_tag_arr(_tmp32,sizeof(void*),
2));}}});}return _tmp31;}}struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};static
void*Cyc_CfFlowInfo_i_typ_to_absrval(int allow_zeroterm,void*t,void*leafval){if(
Cyc_Absyn_is_union_type(t))return Cyc_CfFlowInfo_unknown_all;{void*_tmp3B=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_TunionFieldInfo _tmp3C;void*_tmp3D;struct Cyc_Absyn_Tunionfield*
_tmp3E;struct Cyc_List_List*_tmp3F;struct Cyc_Absyn_AggrInfo _tmp40;void*_tmp41;
void*_tmp42;struct Cyc_List_List*_tmp43;struct Cyc_Absyn_ArrayInfo _tmp44;void*
_tmp45;struct Cyc_Absyn_Conref*_tmp46;void*_tmp47;struct Cyc_Absyn_PtrInfo _tmp48;
struct Cyc_Absyn_PtrAtts _tmp49;struct Cyc_Absyn_Conref*_tmp4A;_LL17: if(_tmp3B <= (
void*)3?1:*((int*)_tmp3B)!= 3)goto _LL19;_tmp3C=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3B)->f1;_tmp3D=(void*)_tmp3C.field_info;if(*((int*)_tmp3D)!= 1)goto _LL19;
_tmp3E=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3D)->f2;_LL18: if(_tmp3E->typs
== 0)return leafval;_tmp3F=_tmp3E->typs;goto _LL1A;_LL19: if(_tmp3B <= (void*)3?1:*((
int*)_tmp3B)!= 9)goto _LL1B;_tmp3F=((struct Cyc_Absyn_TupleType_struct*)_tmp3B)->f1;
_LL1A: {struct Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{int i=0;for(0;
_tmp3F != 0;(_tmp3F=_tmp3F->tl,++ i)){d=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(d,Cyc_Absyn_fieldname(i),Cyc_CfFlowInfo_i_typ_to_absrval(
0,(*((struct _tuple4*)_tmp3F->hd)).f2,leafval));}}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp4C;_tmp4C.tag=4;_tmp4C.f1=d;_tmp4C;});_tmp4B;});}_LL1B: if(_tmp3B <= (void*)3?
1:*((int*)_tmp3B)!= 10)goto _LL1D;_tmp40=((struct Cyc_Absyn_AggrType_struct*)
_tmp3B)->f1;_tmp41=(void*)_tmp40.aggr_info;_LL1C: {struct Cyc_Absyn_Aggrdecl*
_tmp4D=Cyc_Absyn_get_known_aggrdecl(_tmp41);if(_tmp4D->impl == 0)goto _LL16;_tmp43=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4D->impl))->fields;goto _LL1E;}
_LL1D: if(_tmp3B <= (void*)3?1:*((int*)_tmp3B)!= 11)goto _LL1F;_tmp42=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp3B)->f1;if((int)_tmp42 != 0)goto _LL1F;
_tmp43=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp3B)->f2;_LL1E: {struct Cyc_Dict_Dict*
d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))
Cyc_Dict_empty)(Cyc_strptrcmp);for(0;_tmp43 != 0;_tmp43=_tmp43->tl){struct Cyc_Absyn_Aggrfield
_tmp4F;struct _tagged_arr*_tmp50;void*_tmp51;struct Cyc_Absyn_Aggrfield*_tmp4E=(
struct Cyc_Absyn_Aggrfield*)_tmp43->hd;_tmp4F=*_tmp4E;_tmp50=_tmp4F.name;_tmp51=(
void*)_tmp4F.type;if(_get_arr_size(*_tmp50,sizeof(char))!= 1)d=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(d,_tmp50,Cyc_CfFlowInfo_i_typ_to_absrval(
0,_tmp51,leafval));}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp52=
_cycalloc(sizeof(*_tmp52));_tmp52[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp53;_tmp53.tag=4;_tmp53.f1=d;_tmp53;});_tmp52;});}_LL1F: if(_tmp3B <= (void*)3?
1:*((int*)_tmp3B)!= 7)goto _LL21;_tmp44=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3B)->f1;_tmp45=(void*)_tmp44.elt_type;_tmp46=_tmp44.zero_term;if(!((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp46))goto _LL21;_LL20:
return(allow_zeroterm?Cyc_Tcutil_bits_only(_tmp45): 0)?Cyc_CfFlowInfo_unknown_all:
leafval;_LL21: if(_tmp3B <= (void*)3?1:*((int*)_tmp3B)!= 17)goto _LL23;_tmp47=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp3B)->f1;_LL22: {void*_tmp54=leafval;void*
_tmp55;void*_tmp56;_LL28: if(_tmp54 <= (void*)3?1:*((int*)_tmp54)!= 0)goto _LL2A;
_tmp55=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp54)->f1;if((int)_tmp55
!= 2)goto _LL2A;_LL29: goto _LL2B;_LL2A: if(_tmp54 <= (void*)3?1:*((int*)_tmp54)!= 1)
goto _LL2C;_tmp56=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp54)->f1;if((int)
_tmp56 != 2)goto _LL2C;_LL2B: goto _LL2D;_LL2C: if((int)_tmp54 != 0)goto _LL2E;_LL2D:
goto _LL2F;_LL2E: if((int)_tmp54 != 1)goto _LL30;_LL2F: return(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp58;_tmp58.tag=3;_tmp58.f1=({struct Cyc_CfFlowInfo_TagCmp*_tmp59[1];_tmp59[0]=({
struct Cyc_CfFlowInfo_TagCmp*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->cmp=(void*)((
void*)5);_tmp5A->bd=(void*)_tmp47;_tmp5A;});((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp59,sizeof(struct Cyc_CfFlowInfo_TagCmp*),
1));});_tmp58;});_tmp57;});_LL30:;_LL31: return leafval;_LL27:;}_LL23: if(_tmp3B <= (
void*)3?1:*((int*)_tmp3B)!= 4)goto _LL25;_tmp48=((struct Cyc_Absyn_PointerType_struct*)
_tmp3B)->f1;_tmp49=_tmp48.ptr_atts;_tmp4A=_tmp49.nullable;if(!(!((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4A)))goto _LL25;_LL24:{void*
_tmp5B=leafval;void*_tmp5C;void*_tmp5D;_LL33: if(_tmp5B <= (void*)3?1:*((int*)
_tmp5B)!= 0)goto _LL35;_tmp5C=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp5B)->f1;if((int)_tmp5C != 1)goto _LL35;_LL34: return(void*)2;_LL35: if(_tmp5B <= (
void*)3?1:*((int*)_tmp5B)!= 0)goto _LL37;_tmp5D=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp5B)->f1;if((int)_tmp5D != 2)goto _LL37;_LL36: return(void*)1;_LL37:;_LL38: goto
_LL32;_LL32:;}goto _LL16;_LL25:;_LL26: goto _LL16;_LL16:;}return Cyc_Tcutil_bits_only(
t)?Cyc_CfFlowInfo_unknown_all: leafval;}void*Cyc_CfFlowInfo_typ_to_absrval(void*t,
void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(1,t,leafval);}static int Cyc_CfFlowInfo_prefix_of_member(
struct _RegionHandle*r,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Set_Set*set){
struct Cyc_CfFlowInfo_Place*_tmp5E=place;struct Cyc_Iter_Iter _tmp5F=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(r,set);while(((
int(*)(struct Cyc_Iter_Iter,struct Cyc_CfFlowInfo_Place**))Cyc_Iter_next)(_tmp5F,&
_tmp5E)){if(Cyc_CfFlowInfo_root_cmp((void*)place->root,(void*)_tmp5E->root)!= 0)
continue;{struct Cyc_List_List*_tmp60=place->fields;struct Cyc_List_List*_tmp61=
_tmp5E->fields;for(0;_tmp60 != 0?_tmp61 != 0: 0;(_tmp60=_tmp60->tl,_tmp61=_tmp61->tl)){
if(Cyc_strptrcmp((struct _tagged_arr*)_tmp60->hd,(struct _tagged_arr*)_tmp61->hd)
!= 0)break;}if(_tmp60 == 0)return 1;}}return 0;}struct Cyc_CfFlowInfo_EscPile{struct
_RegionHandle*rgn;struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(
struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,
place))pile->places=({struct Cyc_List_List*_tmp62=_region_malloc(pile->rgn,
sizeof(*_tmp62));_tmp62->hd=place;_tmp62->tl=pile->places;_tmp62;});}static void
Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*a,void*r){void*
_tmp63=r;struct Cyc_CfFlowInfo_Place*_tmp64;struct Cyc_Dict_Dict*_tmp65;_LL3A: if(
_tmp63 <= (void*)3?1:*((int*)_tmp63)!= 2)goto _LL3C;_tmp64=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp63)->f1;_LL3B: Cyc_CfFlowInfo_add_place(pile,_tmp64);return;_LL3C: if(_tmp63 <= (
void*)3?1:*((int*)_tmp63)!= 4)goto _LL3E;_tmp65=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp63)->f1;_LL3D:((void(*)(void(*f)(struct Cyc_CfFlowInfo_EscPile*,struct
_tagged_arr*,void*),struct Cyc_CfFlowInfo_EscPile*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)((
void(*)(struct Cyc_CfFlowInfo_EscPile*pile,struct _tagged_arr*a,void*r))Cyc_CfFlowInfo_add_places,
pile,_tmp65);return;_LL3E:;_LL3F: return;_LL39:;}static void*Cyc_CfFlowInfo_insert_place_inner(
void*new_val,void*old_val){void*_tmp66=old_val;struct Cyc_Dict_Dict*_tmp67;_LL41:
if(_tmp66 <= (void*)3?1:*((int*)_tmp66)!= 4)goto _LL43;_tmp67=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp66)->f1;_LL42: return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp68=
_cycalloc(sizeof(*_tmp68));_tmp68[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp69;_tmp69.tag=4;_tmp69.f1=((struct Cyc_Dict_Dict*(*)(void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d))Cyc_Dict_map_c)(Cyc_CfFlowInfo_insert_place_inner,
new_val,_tmp67);_tmp69;});_tmp68;});_LL43:;_LL44: return new_val;_LL40:;}struct
_tuple5{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
new_val,old_val);{struct _tuple5 _tmp6B=({struct _tuple5 _tmp6A;_tmp6A.f1=fs;_tmp6A.f2=
old_val;_tmp6A;});struct Cyc_List_List*_tmp6C;struct Cyc_List_List _tmp6D;struct
_tagged_arr*_tmp6E;struct Cyc_List_List*_tmp6F;void*_tmp70;struct Cyc_Dict_Dict*
_tmp71;_LL46: _tmp6C=_tmp6B.f1;if(_tmp6C == 0)goto _LL48;_tmp6D=*_tmp6C;_tmp6E=(
struct _tagged_arr*)_tmp6D.hd;_tmp6F=_tmp6D.tl;_tmp70=_tmp6B.f2;if(_tmp70 <= (void*)
3?1:*((int*)_tmp70)!= 4)goto _LL48;_tmp71=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp70)->f1;_LL47: {void*_tmp72=Cyc_CfFlowInfo_insert_place_outer(_tmp6F,((void*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp71,_tmp6E),
new_val);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp73=_cycalloc(
sizeof(*_tmp73));_tmp73[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp74;_tmp74.tag=
4;_tmp74.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,
void*v))Cyc_Dict_insert)(_tmp71,_tmp6E,_tmp72);_tmp74;});_tmp73;});}_LL48:;_LL49:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp75=_cycalloc(sizeof(*
_tmp75));_tmp75[0]=({struct Cyc_Core_Impossible_struct _tmp76;_tmp76.tag=Cyc_Core_Impossible;
_tmp76.f1=({const char*_tmp77="bad insert place";_tag_arr(_tmp77,sizeof(char),
_get_zero_arr_size(_tmp77,17));});_tmp76;});_tmp75;}));_LL45:;}}static struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Set_Set**
all_changed,struct Cyc_Dict_Dict*d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*
_tmp78=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;if(
all_changed != 0)*all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*all_changed,_tmp78);{void*oldval;void*
newval;{struct _handler_cons _tmp79;_push_handler(& _tmp79);{int _tmp7B=0;if(setjmp(
_tmp79.handler))_tmp7B=1;if(!_tmp7B){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp78);;
_pop_handler();}else{void*_tmp7A=(void*)_exn_thrown;void*_tmp7D=_tmp7A;_LL4B: if(
_tmp7D != Cyc_Dict_Absent)goto _LL4D;_LL4C: continue;_LL4D:;_LL4E:(void)_throw(
_tmp7D);_LL4A:;}}}{void*_tmp7E=Cyc_CfFlowInfo_initlevel(d,oldval);_LL50: if((int)
_tmp7E != 2)goto _LL52;_LL51: newval=Cyc_CfFlowInfo_esc_all;goto _LL4F;_LL52: if((int)
_tmp7E != 1)goto _LL54;_LL53: newval=Cyc_CfFlowInfo_esc_this;goto _LL4F;_LL54: if((
int)_tmp7E != 0)goto _LL4F;_LL55: newval=Cyc_CfFlowInfo_esc_none;goto _LL4F;_LL4F:;}((
void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,oldval);d=Cyc_Dict_insert(d,(void*)_tmp78->root,Cyc_CfFlowInfo_insert_place_outer(
_tmp78->fields,Cyc_Dict_lookup(d,(void*)_tmp78->root),newval));}}return d;}struct
Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict*d;struct Cyc_List_List*seen;};
static void*Cyc_CfFlowInfo_initlevel_approx(void*r){void*_tmp7F=r;void*_tmp80;
void*_tmp81;_LL57: if(_tmp7F <= (void*)3?1:*((int*)_tmp7F)!= 0)goto _LL59;_tmp80=(
void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp7F)->f1;_LL58: return _tmp80;
_LL59: if(_tmp7F <= (void*)3?1:*((int*)_tmp7F)!= 1)goto _LL5B;_tmp81=(void*)((
struct Cyc_CfFlowInfo_Esc_struct*)_tmp7F)->f1;_LL5A: return _tmp81;_LL5B: if((int)
_tmp7F != 0)goto _LL5D;_LL5C: goto _LL5E;_LL5D: if((int)_tmp7F != 1)goto _LL5F;_LL5E:
return(void*)2;_LL5F: if((int)_tmp7F != 2)goto _LL61;_LL60: return(void*)1;_LL61: if(
_tmp7F <= (void*)3?1:*((int*)_tmp7F)!= 3)goto _LL63;_LL62: return(void*)2;_LL63:;
_LL64:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp82=_cycalloc(
sizeof(*_tmp82));_tmp82[0]=({struct Cyc_Core_Impossible_struct _tmp83;_tmp83.tag=
Cyc_Core_Impossible;_tmp83.f1=({const char*_tmp84="initlevel_approx";_tag_arr(
_tmp84,sizeof(char),_get_zero_arr_size(_tmp84,17));});_tmp83;});_tmp82;}));_LL56:;}
static void*Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,
void*a,void*r,void*acc){void*this_ans;if(acc == (void*)0)return(void*)0;{void*
_tmp85=r;struct Cyc_Dict_Dict*_tmp86;struct Cyc_CfFlowInfo_Place*_tmp87;_LL66: if(
_tmp85 <= (void*)3?1:*((int*)_tmp85)!= 4)goto _LL68;_tmp86=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp85)->f1;_LL67: this_ans=((void*(*)(void*(*f)(struct Cyc_CfFlowInfo_InitlevelEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_InitlevelEnv*env,struct Cyc_Dict_Dict*
d,void*accum))Cyc_Dict_fold_c)((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,
struct _tagged_arr*a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec,env,_tmp86,(
void*)2);goto _LL65;_LL68: if(_tmp85 <= (void*)3?1:*((int*)_tmp85)!= 2)goto _LL6A;
_tmp87=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp85)->f1;_LL69: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp87))this_ans=(void*)2;else{env->seen=({struct Cyc_List_List*_tmp88=_cycalloc(
sizeof(*_tmp88));_tmp88->hd=_tmp87;_tmp88->tl=env->seen;_tmp88;});this_ans=((
void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(
env,0,Cyc_CfFlowInfo_lookup_place(env->d,_tmp87),(void*)2);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == (void*)0)this_ans=(void*)1;}goto _LL65;
_LL6A:;_LL6B: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL65:;}if(this_ans == (
void*)0)return(void*)0;if(this_ans == (void*)1?1: acc == (void*)1)return(void*)1;
return(void*)2;}void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,void*r){
struct Cyc_CfFlowInfo_InitlevelEnv _tmp89=({struct Cyc_CfFlowInfo_InitlevelEnv
_tmp8A;_tmp8A.d=d;_tmp8A.seen=0;_tmp8A;});return((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*
env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(& _tmp89,0,r,(void*)2);}
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place){struct Cyc_CfFlowInfo_Place _tmp8C;void*_tmp8D;struct Cyc_List_List*_tmp8E;
struct Cyc_CfFlowInfo_Place*_tmp8B=place;_tmp8C=*_tmp8B;_tmp8D=(void*)_tmp8C.root;
_tmp8E=_tmp8C.fields;{void*_tmp8F=Cyc_Dict_lookup(d,_tmp8D);for(0;_tmp8E != 0;
_tmp8E=_tmp8E->tl){struct _tuple1 _tmp91=({struct _tuple1 _tmp90;_tmp90.f1=_tmp8F;
_tmp90.f2=(struct _tagged_arr*)_tmp8E->hd;_tmp90;});void*_tmp92;struct Cyc_Dict_Dict*
_tmp93;struct _tagged_arr*_tmp94;_LL6D: _tmp92=_tmp91.f1;if(_tmp92 <= (void*)3?1:*((
int*)_tmp92)!= 4)goto _LL6F;_tmp93=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp92)->f1;_tmp94=_tmp91.f2;_LL6E: _tmp8F=((void*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp93,_tmp94);goto _LL6C;_LL6F:;_LL70:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp95=_cycalloc(sizeof(*_tmp95));
_tmp95[0]=({struct Cyc_Core_Impossible_struct _tmp96;_tmp96.tag=Cyc_Core_Impossible;
_tmp96.f1=({const char*_tmp97="bad lookup_place";_tag_arr(_tmp97,sizeof(char),
_get_zero_arr_size(_tmp97,17));});_tmp96;});_tmp95;}));_LL6C:;}return _tmp8F;}}
static int Cyc_CfFlowInfo_is_rval_unescaped(void*a,void*b,void*rval){void*_tmp98=
rval;struct Cyc_Dict_Dict*_tmp99;_LL72: if(_tmp98 <= (void*)3?1:*((int*)_tmp98)!= 1)
goto _LL74;_LL73: return 0;_LL74: if(_tmp98 <= (void*)3?1:*((int*)_tmp98)!= 4)goto
_LL76;_tmp99=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp98)->f1;_LL75: return((
int(*)(int(*f)(int,struct _tagged_arr*,void*),int env,struct Cyc_Dict_Dict*d))Cyc_Dict_forall_c)((
int(*)(int a,struct _tagged_arr*b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped,0,
_tmp99);_LL76:;_LL77: return 1;_LL71:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place){return((int(*)(int a,int b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped)(
0,0,Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,void*r){struct _RegionHandle
_tmp9A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp9A;_push_region(rgn);{
struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmp9C=
_region_malloc(rgn,sizeof(*_tmp9C));_tmp9C->rgn=rgn;_tmp9C->places=0;_tmp9C;});((
void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,r);{struct Cyc_Dict_Dict*_tmp9B=Cyc_CfFlowInfo_escape_these(pile,
all_changed,d);_npop_handler(0);return _tmp9B;}};_pop_region(rgn);}struct Cyc_CfFlowInfo_AssignEnv{
struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict*d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*a,void*oldval,void*newval){struct _tuple0 _tmp9E=({struct _tuple0 _tmp9D;
_tmp9D.f1=oldval;_tmp9D.f2=newval;_tmp9D;});void*_tmp9F;void*_tmpA0;struct Cyc_CfFlowInfo_Place*
_tmpA1;void*_tmpA2;void*_tmpA3;struct Cyc_Dict_Dict*_tmpA4;void*_tmpA5;struct Cyc_Dict_Dict*
_tmpA6;void*_tmpA7;void*_tmpA8;_LL79: _tmp9F=_tmp9E.f1;if(_tmp9F <= (void*)3?1:*((
int*)_tmp9F)!= 1)goto _LL7B;_tmpA0=_tmp9E.f2;if(_tmpA0 <= (void*)3?1:*((int*)
_tmpA0)!= 2)goto _LL7B;_tmpA1=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpA0)->f1;
_LL7A: Cyc_CfFlowInfo_add_place(env->pile,_tmpA1);goto _LL7C;_LL7B: _tmpA2=_tmp9E.f1;
if(_tmpA2 <= (void*)3?1:*((int*)_tmpA2)!= 1)goto _LL7D;_LL7C: if(Cyc_CfFlowInfo_initlevel(
env->d,newval)!= (void*)2)({void*_tmpA9[0]={};Cyc_Tcutil_terr(env->loc,({const
char*_tmpAA="assignment puts possibly-uninitialized data in an escaped location";
_tag_arr(_tmpAA,sizeof(char),_get_zero_arr_size(_tmpAA,67));}),_tag_arr(_tmpA9,
sizeof(void*),0));});return Cyc_CfFlowInfo_esc_all;_LL7D: _tmpA3=_tmp9E.f1;if(
_tmpA3 <= (void*)3?1:*((int*)_tmpA3)!= 4)goto _LL7F;_tmpA4=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpA3)->f1;_tmpA5=_tmp9E.f2;if(_tmpA5 <= (void*)3?1:*((int*)_tmpA5)!= 4)goto
_LL7F;_tmpA6=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpA5)->f1;_LL7E: {struct
Cyc_Dict_Dict*_tmpAB=((struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_AssignEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(struct Cyc_CfFlowInfo_AssignEnv*
env,struct _tagged_arr*a,void*oldval,void*newval))Cyc_CfFlowInfo_assign_place_inner,
env,_tmpA4,_tmpA6);if(_tmpAB == _tmpA4)return oldval;if(_tmpAB == _tmpA6)return
newval;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpAC=_cycalloc(
sizeof(*_tmpAC));_tmpAC[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmpAD;_tmpAD.tag=
4;_tmpAD.f1=_tmpAB;_tmpAD;});_tmpAC;});}_LL7F: _tmpA7=_tmp9E.f2;if(_tmpA7 <= (void*)
3?1:*((int*)_tmpA7)!= 1)goto _LL81;_tmpA8=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmpA7)->f1;_LL80: {void*_tmpAE=_tmpA8;_LL84: if((int)_tmpAE != 0)goto _LL86;_LL85:
return Cyc_CfFlowInfo_unknown_none;_LL86: if((int)_tmpAE != 1)goto _LL88;_LL87:
return Cyc_CfFlowInfo_unknown_this;_LL88: if((int)_tmpAE != 2)goto _LL83;_LL89:
return Cyc_CfFlowInfo_unknown_all;_LL83:;}_LL81:;_LL82: return newval;_LL78:;}
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,
struct Cyc_List_List*fs,void*oldval,void*newval){if(fs == 0)return((void*(*)(
struct Cyc_CfFlowInfo_AssignEnv*env,int a,void*oldval,void*newval))Cyc_CfFlowInfo_assign_place_inner)(
env,0,oldval,newval);{struct _tuple5 _tmpB0=({struct _tuple5 _tmpAF;_tmpAF.f1=fs;
_tmpAF.f2=oldval;_tmpAF;});struct Cyc_List_List*_tmpB1;struct Cyc_List_List _tmpB2;
struct _tagged_arr*_tmpB3;struct Cyc_List_List*_tmpB4;void*_tmpB5;struct Cyc_Dict_Dict*
_tmpB6;_LL8B: _tmpB1=_tmpB0.f1;if(_tmpB1 == 0)goto _LL8D;_tmpB2=*_tmpB1;_tmpB3=(
struct _tagged_arr*)_tmpB2.hd;_tmpB4=_tmpB2.tl;_tmpB5=_tmpB0.f2;if(_tmpB5 <= (void*)
3?1:*((int*)_tmpB5)!= 4)goto _LL8D;_tmpB6=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpB5)->f1;_LL8C: {void*_tmpB7=Cyc_CfFlowInfo_assign_place_outer(env,_tmpB4,((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmpB6,
_tmpB3),newval);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpB8=
_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpB9;_tmpB9.tag=4;_tmpB9.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(_tmpB6,_tmpB3,_tmpB7);_tmpB9;});
_tmpB8;});}_LL8D:;_LL8E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA[0]=({struct Cyc_Core_Impossible_struct
_tmpBB;_tmpBB.tag=Cyc_Core_Impossible;_tmpBB.f1=({const char*_tmpBC="bad assign place";
_tag_arr(_tmpBC,sizeof(char),_get_zero_arr_size(_tmpBC,17));});_tmpBB;});_tmpBA;}));
_LL8A:;}}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r){if(all_changed != 0)*all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*all_changed,place);{struct
_RegionHandle _tmpBD=_new_region("rgn");struct _RegionHandle*rgn=& _tmpBD;
_push_region(rgn);{struct Cyc_CfFlowInfo_Place _tmpBF;void*_tmpC0;struct Cyc_List_List*
_tmpC1;struct Cyc_CfFlowInfo_Place*_tmpBE=place;_tmpBF=*_tmpBE;_tmpC0=(void*)
_tmpBF.root;_tmpC1=_tmpBF.fields;{struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv
_tmpC3;_tmpC3.pile=({struct Cyc_CfFlowInfo_EscPile*_tmpC4=_region_malloc(rgn,
sizeof(*_tmpC4));_tmpC4->rgn=rgn;_tmpC4->places=0;_tmpC4;});_tmpC3.d=d;_tmpC3.loc=
loc;_tmpC3;});void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmpC1,Cyc_Dict_lookup(
d,_tmpC0),r);struct Cyc_Dict_Dict*_tmpC2=Cyc_CfFlowInfo_escape_these(env.pile,
all_changed,Cyc_Dict_insert(d,_tmpC0,newval));_npop_handler(0);return _tmpC2;}};
_pop_region(rgn);}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict*d1;struct Cyc_Dict_Dict*d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Set_Set*
chg1;struct Cyc_Set_Set*chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);static struct Cyc_List_List*Cyc_CfFlowInfo_join_tag_cmps(
struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2)return l1;{struct Cyc_List_List*
_tmpC5=0;for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp _tmpC7;void*_tmpC8;
void*_tmpC9;struct Cyc_CfFlowInfo_TagCmp*_tmpC6=(struct Cyc_CfFlowInfo_TagCmp*)l2->hd;
_tmpC7=*_tmpC6;_tmpC8=(void*)_tmpC7.cmp;_tmpC9=(void*)_tmpC7.bd;{int found=0;void*
joined_cmp=(void*)10;{struct Cyc_List_List*_tmpCA=l1;for(0;_tmpCA != 0;_tmpCA=
_tmpCA->tl){struct Cyc_CfFlowInfo_TagCmp _tmpCC;void*_tmpCD;void*_tmpCE;struct Cyc_CfFlowInfo_TagCmp*
_tmpCB=(struct Cyc_CfFlowInfo_TagCmp*)_tmpCA->hd;_tmpCC=*_tmpCB;_tmpCD=(void*)
_tmpCC.cmp;_tmpCE=(void*)_tmpCC.bd;if(Cyc_Tcutil_typecmp(_tmpC9,_tmpCE)== 0){
found=1;if(_tmpCD == _tmpC8){joined_cmp=_tmpCD;break;}}}}if(found)_tmpC5=({struct
Cyc_List_List*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->hd=({struct Cyc_CfFlowInfo_TagCmp*
_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->cmp=(void*)joined_cmp;_tmpD0->bd=(void*)
_tmpC9;_tmpD0;});_tmpCF->tl=_tmpC5;_tmpCF;});}}return _tmpC5;}}static void*Cyc_CfFlowInfo_join_absRval(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{
struct _tuple0 _tmpD2=({struct _tuple0 _tmpD1;_tmpD1.f1=r1;_tmpD1.f2=r2;_tmpD1;});
void*_tmpD3;struct Cyc_CfFlowInfo_Place*_tmpD4;void*_tmpD5;struct Cyc_CfFlowInfo_Place*
_tmpD6;void*_tmpD7;struct Cyc_CfFlowInfo_Place*_tmpD8;void*_tmpD9;struct Cyc_CfFlowInfo_Place*
_tmpDA;void*_tmpDB;void*_tmpDC;void*_tmpDD;void*_tmpDE;void*_tmpDF;struct Cyc_Dict_Dict*
_tmpE0;void*_tmpE1;struct Cyc_Dict_Dict*_tmpE2;void*_tmpE3;struct Cyc_List_List*
_tmpE4;void*_tmpE5;struct Cyc_List_List*_tmpE6;void*_tmpE7;void*_tmpE8;_LL90:
_tmpD3=_tmpD2.f1;if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 2)goto _LL92;_tmpD4=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpD3)->f1;_tmpD5=_tmpD2.f2;if(_tmpD5 <= (
void*)3?1:*((int*)_tmpD5)!= 2)goto _LL92;_tmpD6=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpD5)->f1;_LL91: if(Cyc_CfFlowInfo_place_cmp(_tmpD4,_tmpD6)== 0)return r1;Cyc_CfFlowInfo_add_place(
env->pile,_tmpD4);Cyc_CfFlowInfo_add_place(env->pile,_tmpD6);goto _LL8F;_LL92:
_tmpD7=_tmpD2.f1;if(_tmpD7 <= (void*)3?1:*((int*)_tmpD7)!= 2)goto _LL94;_tmpD8=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpD7)->f1;_LL93: Cyc_CfFlowInfo_add_place(
env->pile,_tmpD8);goto _LL8F;_LL94: _tmpD9=_tmpD2.f2;if(_tmpD9 <= (void*)3?1:*((int*)
_tmpD9)!= 2)goto _LL96;_tmpDA=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpD9)->f1;
_LL95: Cyc_CfFlowInfo_add_place(env->pile,_tmpDA);goto _LL8F;_LL96: _tmpDB=_tmpD2.f1;
if((int)_tmpDB != 1)goto _LL98;_tmpDC=_tmpD2.f2;if((int)_tmpDC != 2)goto _LL98;_LL97:
goto _LL99;_LL98: _tmpDD=_tmpD2.f1;if((int)_tmpDD != 2)goto _LL9A;_tmpDE=_tmpD2.f2;
if((int)_tmpDE != 1)goto _LL9A;_LL99: return(void*)2;_LL9A: _tmpDF=_tmpD2.f1;if(
_tmpDF <= (void*)3?1:*((int*)_tmpDF)!= 4)goto _LL9C;_tmpE0=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpDF)->f1;_tmpE1=_tmpD2.f2;if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 4)goto
_LL9C;_tmpE2=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpE1)->f1;_LL9B: {struct
Cyc_Dict_Dict*_tmpE9=((struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,struct _tagged_arr*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,env,_tmpE0,
_tmpE2);if(_tmpE9 == _tmpE0)return r1;if(_tmpE9 == _tmpE2)return r2;return(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _tmpEB;_tmpEB.tag=4;_tmpEB.f1=_tmpE9;_tmpEB;});
_tmpEA;});}_LL9C: _tmpE3=_tmpD2.f1;if(_tmpE3 <= (void*)3?1:*((int*)_tmpE3)!= 3)
goto _LL9E;_tmpE4=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmpE3)->f1;_tmpE5=
_tmpD2.f2;if(_tmpE5 <= (void*)3?1:*((int*)_tmpE5)!= 3)goto _LL9E;_tmpE6=((struct
Cyc_CfFlowInfo_TagCmps_struct*)_tmpE5)->f1;_LL9D: {struct Cyc_List_List*_tmpEC=
Cyc_CfFlowInfo_join_tag_cmps(_tmpE4,_tmpE6);if(_tmpEC == _tmpE4)return r1;return(
void*)({struct Cyc_CfFlowInfo_TagCmps_struct*_tmpED=_cycalloc(sizeof(*_tmpED));
_tmpED[0]=({struct Cyc_CfFlowInfo_TagCmps_struct _tmpEE;_tmpEE.tag=3;_tmpEE.f1=
_tmpEC;_tmpEE;});_tmpED;});}_LL9E: _tmpE7=_tmpD2.f1;if(_tmpE7 <= (void*)3?1:*((int*)
_tmpE7)!= 3)goto _LLA0;_LL9F: return r2;_LLA0: _tmpE8=_tmpD2.f2;if(_tmpE8 <= (void*)3?
1:*((int*)_tmpE8)!= 3)goto _LLA2;_LLA1: return r1;_LLA2:;_LLA3: goto _LL8F;_LL8F:;}{
void*il1=Cyc_CfFlowInfo_initlevel(env->d1,r1);void*il2=Cyc_CfFlowInfo_initlevel(
env->d2,r2);struct _tuple0 _tmpF0=({struct _tuple0 _tmpEF;_tmpEF.f1=r1;_tmpEF.f2=r2;
_tmpEF;});void*_tmpF1;void*_tmpF2;_LLA5: _tmpF1=_tmpF0.f1;if(_tmpF1 <= (void*)3?1:*((
int*)_tmpF1)!= 1)goto _LLA7;_LLA6: goto _LLA8;_LLA7: _tmpF2=_tmpF0.f2;if(_tmpF2 <= (
void*)3?1:*((int*)_tmpF2)!= 1)goto _LLA9;_LLA8: {struct _tuple0 _tmpF4=({struct
_tuple0 _tmpF3;_tmpF3.f1=il1;_tmpF3.f2=il2;_tmpF3;});void*_tmpF5;void*_tmpF6;void*
_tmpF7;void*_tmpF8;_LLAC: _tmpF5=_tmpF4.f2;if((int)_tmpF5 != 0)goto _LLAE;_LLAD:
goto _LLAF;_LLAE: _tmpF6=_tmpF4.f1;if((int)_tmpF6 != 0)goto _LLB0;_LLAF: return Cyc_CfFlowInfo_esc_none;
_LLB0: _tmpF7=_tmpF4.f2;if((int)_tmpF7 != 1)goto _LLB2;_LLB1: goto _LLB3;_LLB2: _tmpF8=
_tmpF4.f1;if((int)_tmpF8 != 1)goto _LLB4;_LLB3: return Cyc_CfFlowInfo_esc_this;_LLB4:;
_LLB5: return Cyc_CfFlowInfo_esc_all;_LLAB:;}_LLA9:;_LLAA: {struct _tuple0 _tmpFA=({
struct _tuple0 _tmpF9;_tmpF9.f1=il1;_tmpF9.f2=il2;_tmpF9;});void*_tmpFB;void*
_tmpFC;void*_tmpFD;void*_tmpFE;_LLB7: _tmpFB=_tmpFA.f2;if((int)_tmpFB != 0)goto
_LLB9;_LLB8: goto _LLBA;_LLB9: _tmpFC=_tmpFA.f1;if((int)_tmpFC != 0)goto _LLBB;_LLBA:
return Cyc_CfFlowInfo_unknown_none;_LLBB: _tmpFD=_tmpFA.f2;if((int)_tmpFD != 1)goto
_LLBD;_LLBC: goto _LLBE;_LLBD: _tmpFE=_tmpFA.f1;if((int)_tmpFE != 1)goto _LLBF;_LLBE:
return Cyc_CfFlowInfo_unknown_this;_LLBF:;_LLC0: return Cyc_CfFlowInfo_unknown_all;
_LLB6:;}_LLA4:;}}static int Cyc_CfFlowInfo_same_relop(void*r1,void*r2){if(r1 == r2)
return 1;{struct _tuple0 _tmp100=({struct _tuple0 _tmpFF;_tmpFF.f1=r1;_tmpFF.f2=r2;
_tmpFF;});void*_tmp101;unsigned int _tmp102;void*_tmp103;unsigned int _tmp104;void*
_tmp105;struct Cyc_Absyn_Vardecl*_tmp106;void*_tmp107;struct Cyc_Absyn_Vardecl*
_tmp108;void*_tmp109;struct Cyc_Absyn_Vardecl*_tmp10A;void*_tmp10B;struct Cyc_Absyn_Vardecl*
_tmp10C;void*_tmp10D;unsigned int _tmp10E;void*_tmp10F;unsigned int _tmp110;void*
_tmp111;struct Cyc_Absyn_Vardecl*_tmp112;void*_tmp113;struct Cyc_Absyn_Vardecl*
_tmp114;_LLC2: _tmp101=_tmp100.f1;if(*((int*)_tmp101)!= 0)goto _LLC4;_tmp102=((
struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp101)->f1;_tmp103=_tmp100.f2;if(*((
int*)_tmp103)!= 0)goto _LLC4;_tmp104=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmp103)->f1;_LLC3: return _tmp102 == _tmp104;_LLC4: _tmp105=_tmp100.f1;if(*((int*)
_tmp105)!= 1)goto _LLC6;_tmp106=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp105)->f1;
_tmp107=_tmp100.f2;if(*((int*)_tmp107)!= 1)goto _LLC6;_tmp108=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp107)->f1;_LLC5: return _tmp106 == _tmp108;_LLC6: _tmp109=_tmp100.f1;if(*((int*)
_tmp109)!= 2)goto _LLC8;_tmp10A=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp109)->f1;
_tmp10B=_tmp100.f2;if(*((int*)_tmp10B)!= 2)goto _LLC8;_tmp10C=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp10B)->f1;_LLC7: return _tmp10A == _tmp10C;_LLC8: _tmp10D=_tmp100.f1;if(*((int*)
_tmp10D)!= 3)goto _LLCA;_tmp10E=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp10D)->f1;
_tmp10F=_tmp100.f2;if(*((int*)_tmp10F)!= 3)goto _LLCA;_tmp110=((struct Cyc_CfFlowInfo_LessConst_struct*)
_tmp10F)->f1;_LLC9: return _tmp10E == _tmp110;_LLCA: _tmp111=_tmp100.f1;if(*((int*)
_tmp111)!= 4)goto _LLCC;_tmp112=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp111)->f1;
_tmp113=_tmp100.f2;if(*((int*)_tmp113)!= 4)goto _LLCC;_tmp114=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp113)->f1;_LLCB: return _tmp112 == _tmp114;_LLCC:;_LLCD: return 0;_LLC1:;}}static
struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct Cyc_List_List*r1s,struct Cyc_List_List*
r2s){if(r1s == r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*
_tmp115=r1s;for(0;_tmp115 != 0;_tmp115=_tmp115->tl){struct Cyc_CfFlowInfo_Reln*
_tmp116=(struct Cyc_CfFlowInfo_Reln*)_tmp115->hd;int found=0;{struct Cyc_List_List*
_tmp117=r2s;for(0;_tmp117 != 0;_tmp117=_tmp117->tl){struct Cyc_CfFlowInfo_Reln*
_tmp118=(struct Cyc_CfFlowInfo_Reln*)_tmp117->hd;if(_tmp116 == _tmp118?1:(_tmp116->vd
== _tmp118->vd?Cyc_CfFlowInfo_same_relop((void*)_tmp116->rop,(void*)_tmp118->rop):
0)){res=({struct Cyc_List_List*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->hd=
_tmp116;_tmp119->tl=res;_tmp119;});found=1;break;}}}if(!found)diff=1;}}if(!diff)
res=r1s;return res;}}void*Cyc_CfFlowInfo_join_flow(struct Cyc_Set_Set**all_changed,
void*f1,void*f2){if(f1 == f2)return f1;{struct _tuple0 _tmp11B=({struct _tuple0
_tmp11A;_tmp11A.f1=f1;_tmp11A.f2=f2;_tmp11A;});void*_tmp11C;void*_tmp11D;void*
_tmp11E;struct Cyc_Dict_Dict*_tmp11F;struct Cyc_List_List*_tmp120;void*_tmp121;
struct Cyc_Dict_Dict*_tmp122;struct Cyc_List_List*_tmp123;_LLCF: _tmp11C=_tmp11B.f1;
if((int)_tmp11C != 0)goto _LLD1;_LLD0: return f2;_LLD1: _tmp11D=_tmp11B.f2;if((int)
_tmp11D != 0)goto _LLD3;_LLD2: return f1;_LLD3: _tmp11E=_tmp11B.f1;if(_tmp11E <= (void*)
1?1:*((int*)_tmp11E)!= 0)goto _LLCE;_tmp11F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp11E)->f1;_tmp120=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp11E)->f2;
_tmp121=_tmp11B.f2;if(_tmp121 <= (void*)1?1:*((int*)_tmp121)!= 0)goto _LLCE;
_tmp122=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp121)->f1;_tmp123=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp121)->f2;_LLD4: if(_tmp11F == _tmp122?
_tmp120 == _tmp123: 0)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))
return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle _tmp124=_new_region("rgn");struct _RegionHandle*rgn=& _tmp124;
_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmp125=({struct Cyc_CfFlowInfo_JoinEnv
_tmp12B;_tmp12B.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp12C=_region_malloc(rgn,
sizeof(*_tmp12C));_tmp12C->rgn=rgn;_tmp12C->places=0;_tmp12C;});_tmp12B.d1=
_tmp11F;_tmp12B.d2=_tmp122;_tmp12B;});struct Cyc_Dict_Dict*_tmp126=((struct Cyc_Dict_Dict*(*)(
void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,&
_tmp125,_tmp11F,_tmp122);struct Cyc_List_List*_tmp127=Cyc_CfFlowInfo_join_relns(
_tmp120,_tmp123);void*_tmp12A=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp129;_tmp129.tag=0;_tmp129.f1=Cyc_CfFlowInfo_escape_these(_tmp125.pile,
all_changed,_tmp126);_tmp129.f2=_tmp127;_tmp129;});_tmp128;});_npop_handler(0);
return _tmp12A;};_pop_region(rgn);}_LLCE:;}}struct _tuple6{void*f1;void*f2;void*f3;
};struct _tuple0 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_Set_Set**all_changed,
struct _tuple0 pr1,struct _tuple0 pr2){void*_tmp12E;void*_tmp12F;struct _tuple0
_tmp12D=pr1;_tmp12E=_tmp12D.f1;_tmp12F=_tmp12D.f2;{void*_tmp131;void*_tmp132;
struct _tuple0 _tmp130=pr2;_tmp131=_tmp130.f1;_tmp132=_tmp130.f2;{void*outflow=Cyc_CfFlowInfo_join_flow(
all_changed,_tmp12E,_tmp131);struct _tuple6 _tmp134=({struct _tuple6 _tmp133;_tmp133.f1=
_tmp12E;_tmp133.f2=_tmp131;_tmp133.f3=outflow;_tmp133;});void*_tmp135;void*
_tmp136;void*_tmp137;struct Cyc_Dict_Dict*_tmp138;void*_tmp139;struct Cyc_Dict_Dict*
_tmp13A;void*_tmp13B;struct Cyc_Dict_Dict*_tmp13C;struct Cyc_List_List*_tmp13D;
_LLD6: _tmp135=_tmp134.f1;if((int)_tmp135 != 0)goto _LLD8;_LLD7: return({struct
_tuple0 _tmp13E;_tmp13E.f1=outflow;_tmp13E.f2=_tmp132;_tmp13E;});_LLD8: _tmp136=
_tmp134.f2;if((int)_tmp136 != 0)goto _LLDA;_LLD9: return({struct _tuple0 _tmp13F;
_tmp13F.f1=outflow;_tmp13F.f2=_tmp12F;_tmp13F;});_LLDA: _tmp137=_tmp134.f1;if(
_tmp137 <= (void*)1?1:*((int*)_tmp137)!= 0)goto _LLDC;_tmp138=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp137)->f1;_tmp139=_tmp134.f2;if(_tmp139 <= (void*)1?1:*((int*)_tmp139)!= 0)
goto _LLDC;_tmp13A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp139)->f1;
_tmp13B=_tmp134.f3;if(_tmp13B <= (void*)1?1:*((int*)_tmp13B)!= 0)goto _LLDC;
_tmp13C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp13B)->f1;_tmp13D=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp13B)->f2;_LLDB: if(((int(*)(int ignore,void*
r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp12F,_tmp132))return({
struct _tuple0 _tmp140;_tmp140.f1=outflow;_tmp140.f2=_tmp132;_tmp140;});if(((int(*)(
int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp132,
_tmp12F))return({struct _tuple0 _tmp141;_tmp141.f1=outflow;_tmp141.f2=_tmp12F;
_tmp141;});{struct _RegionHandle _tmp142=_new_region("rgn");struct _RegionHandle*
rgn=& _tmp142;_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmp143=({struct Cyc_CfFlowInfo_JoinEnv
_tmp149;_tmp149.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp14A=_region_malloc(rgn,
sizeof(*_tmp14A));_tmp14A->rgn=rgn;_tmp14A->places=0;_tmp14A;});_tmp149.d1=
_tmp138;_tmp149.d2=_tmp13A;_tmp149;});void*_tmp144=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp143,0,_tmp12F,
_tmp132);struct _tuple0 _tmp148=({struct _tuple0 _tmp145;_tmp145.f1=(void*)({struct
Cyc_CfFlowInfo_ReachableFL_struct*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp147;_tmp147.tag=0;_tmp147.f1=Cyc_CfFlowInfo_escape_these(
_tmp143.pile,all_changed,_tmp13C);_tmp147.f2=_tmp13D;_tmp147;});_tmp146;});
_tmp145.f2=_tmp144;_tmp145;});_npop_handler(0);return _tmp148;};_pop_region(rgn);}
_LLDC:;_LLDD:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp14B=
_cycalloc(sizeof(*_tmp14B));_tmp14B[0]=({struct Cyc_Core_Impossible_struct _tmp14C;
_tmp14C.tag=Cyc_Core_Impossible;_tmp14C.f1=({const char*_tmp14D="join_flow_and_rval: BottomFL outflow";
_tag_arr(_tmp14D,sizeof(char),_get_zero_arr_size(_tmp14D,37));});_tmp14C;});
_tmp14B;}));_LLD5:;}}}static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,
struct _tagged_arr*,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct
Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){int changed1=env->changed == Cyc_CfFlowInfo_One?
1:((int(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_member)(
env->chg1,env->curr_place);int changed2=env->changed == Cyc_CfFlowInfo_Two?1:((int(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_member)(env->chg2,
env->curr_place);if(changed1?changed2: 0)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
if(changed1){if(!Cyc_CfFlowInfo_prefix_of_member(((env->joinenv).pile)->rgn,env->curr_place,
env->chg2))return r1;env->changed=Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(((
env->joinenv).pile)->rgn,env->curr_place,env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{
struct _tuple0 _tmp14F=({struct _tuple0 _tmp14E;_tmp14E.f1=r1;_tmp14E.f2=r2;_tmp14E;});
void*_tmp150;struct Cyc_Dict_Dict*_tmp151;void*_tmp152;struct Cyc_Dict_Dict*
_tmp153;_LLDF: _tmp150=_tmp14F.f1;if(_tmp150 <= (void*)3?1:*((int*)_tmp150)!= 4)
goto _LLE1;_tmp151=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp150)->f1;_tmp152=
_tmp14F.f2;if(_tmp152 <= (void*)3?1:*((int*)_tmp152)!= 4)goto _LLE1;_tmp153=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp152)->f1;_LLE0: {struct Cyc_Dict_Dict*
_tmp154=((struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_absRval_d,
env,_tmp151,_tmp153);if(_tmp154 == _tmp151)return r1;if(_tmp154 == _tmp153)return r2;
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp155=_cycalloc(sizeof(*
_tmp155));_tmp155[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp156;_tmp156.tag=
4;_tmp156.f1=_tmp154;_tmp156;});_tmp155;});}_LLE1:;_LLE2:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157[0]=({
struct Cyc_Core_Impossible_struct _tmp158;_tmp158.tag=Cyc_Core_Impossible;_tmp158.f1=({
const char*_tmp159="after_absRval -- non-aggregates!";_tag_arr(_tmp159,sizeof(
char),_get_zero_arr_size(_tmp159,33));});_tmp158;});_tmp157;}));_LLDE:;}}static
void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,struct
_tagged_arr*key,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_List_List**
_tmp15A=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged _tmp15B=env->changed;*
env->last_field_cell=({struct Cyc_List_List*_tmp15C=_cycalloc(sizeof(*_tmp15C));
_tmp15C->hd=key;_tmp15C->tl=0;_tmp15C;});env->last_field_cell=&((struct Cyc_List_List*)
_check_null(*env->last_field_cell))->tl;{void*_tmp15D=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp15A;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp15B;return _tmp15D;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;*
env->curr_place=({struct Cyc_CfFlowInfo_Place _tmp15E;_tmp15E.root=(void*)root;
_tmp15E.fields=0;_tmp15E;});env->last_field_cell=&(env->curr_place)->fields;env->changed=
Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}void*Cyc_CfFlowInfo_after_flow(
struct Cyc_Set_Set**all_changed,void*f1,void*f2,struct Cyc_Set_Set*chg1,struct Cyc_Set_Set*
chg2){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,(void*)((void*)0)};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((void*)& dummy_rawexp),0,(void*)((
void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_CfFlowInfo_MallocPt_struct
dummy_root={1,& dummy_exp,(void*)((void*)0)};if(f1 == f2)return f1;{struct _tuple0
_tmp160=({struct _tuple0 _tmp15F;_tmp15F.f1=f1;_tmp15F.f2=f2;_tmp15F;});void*
_tmp161;void*_tmp162;void*_tmp163;struct Cyc_Dict_Dict*_tmp164;struct Cyc_List_List*
_tmp165;void*_tmp166;struct Cyc_Dict_Dict*_tmp167;struct Cyc_List_List*_tmp168;
_LLE4: _tmp161=_tmp160.f1;if((int)_tmp161 != 0)goto _LLE6;_LLE5: goto _LLE7;_LLE6:
_tmp162=_tmp160.f2;if((int)_tmp162 != 0)goto _LLE8;_LLE7: return(void*)0;_LLE8:
_tmp163=_tmp160.f1;if(_tmp163 <= (void*)1?1:*((int*)_tmp163)!= 0)goto _LLE3;
_tmp164=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp163)->f1;_tmp165=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp163)->f2;_tmp166=_tmp160.f2;if(_tmp166 <= (
void*)1?1:*((int*)_tmp166)!= 0)goto _LLE3;_tmp167=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp166)->f1;_tmp168=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp166)->f2;
_LLE9: if(_tmp164 == _tmp167?_tmp165 == _tmp168: 0)return f1;{struct _RegionHandle
_tmp169=_new_region("rgn");struct _RegionHandle*rgn=& _tmp169;_push_region(rgn);{
struct Cyc_CfFlowInfo_Place*_tmp16A=({struct Cyc_CfFlowInfo_Place*_tmp174=
_cycalloc(sizeof(*_tmp174));_tmp174->root=(void*)((void*)& dummy_root);_tmp174->fields=
0;_tmp174;});struct Cyc_CfFlowInfo_AfterEnv _tmp16B=({struct Cyc_CfFlowInfo_AfterEnv
_tmp171;_tmp171.joinenv=({struct Cyc_CfFlowInfo_JoinEnv _tmp172;_tmp172.pile=({
struct Cyc_CfFlowInfo_EscPile*_tmp173=_region_malloc(rgn,sizeof(*_tmp173));
_tmp173->rgn=rgn;_tmp173->places=0;_tmp173;});_tmp172.d1=_tmp164;_tmp172.d2=
_tmp167;_tmp172;});_tmp171.chg1=chg1;_tmp171.chg2=chg2;_tmp171.curr_place=
_tmp16A;_tmp171.last_field_cell=& _tmp16A->fields;_tmp171.changed=Cyc_CfFlowInfo_Neither;
_tmp171;});struct Cyc_Dict_Dict*_tmp16C=((struct Cyc_Dict_Dict*(*)(void*(*f)(
struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp16B,_tmp164,_tmp167);struct Cyc_List_List*_tmp16D=Cyc_CfFlowInfo_join_relns(
_tmp165,_tmp168);void*_tmp170=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp16F;_tmp16F.tag=0;_tmp16F.f1=Cyc_CfFlowInfo_escape_these((_tmp16B.joinenv).pile,
all_changed,_tmp16C);_tmp16F.f2=_tmp16D;_tmp16F;});_tmp16E;});_npop_handler(0);
return _tmp170;};_pop_region(rgn);}_LLE3:;}}static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
struct Cyc_List_List*l1,struct Cyc_List_List*l2){for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp178;void*_tmp179;void*_tmp17A;struct Cyc_CfFlowInfo_TagCmp*_tmp177=(struct Cyc_CfFlowInfo_TagCmp*)
l2->hd;_tmp178=*_tmp177;_tmp179=(void*)_tmp178.cmp;_tmp17A=(void*)_tmp178.bd;{
struct Cyc_List_List*_tmp17B=l1;for(0;_tmp17B != 0;_tmp17B=_tmp17B->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp17D;void*_tmp17E;void*_tmp17F;struct Cyc_CfFlowInfo_TagCmp*_tmp17C=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp17B->hd;_tmp17D=*_tmp17C;_tmp17E=(void*)_tmp17D.cmp;_tmp17F=(void*)_tmp17D.bd;
if(_tmp17E == _tmp179?Cyc_Tcutil_typecmp(_tmp17F,_tmp17A)== 0: 0)break;}if(_tmp17B
== 0)return 0;}}return 1;}static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*
ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp181=({struct _tuple0
_tmp180;_tmp180.f1=r1;_tmp180.f2=r2;_tmp180;});void*_tmp182;struct Cyc_CfFlowInfo_Place*
_tmp183;void*_tmp184;struct Cyc_CfFlowInfo_Place*_tmp185;void*_tmp186;void*
_tmp187;void*_tmp188;struct Cyc_Dict_Dict*_tmp189;void*_tmp18A;struct Cyc_Dict_Dict*
_tmp18B;void*_tmp18C;void*_tmp18D;void*_tmp18E;void*_tmp18F;void*_tmp190;void*
_tmp191;void*_tmp192;struct Cyc_List_List*_tmp193;void*_tmp194;struct Cyc_List_List*
_tmp195;void*_tmp196;_LLEB: _tmp182=_tmp181.f1;if(_tmp182 <= (void*)3?1:*((int*)
_tmp182)!= 2)goto _LLED;_tmp183=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp182)->f1;
_tmp184=_tmp181.f2;if(_tmp184 <= (void*)3?1:*((int*)_tmp184)!= 2)goto _LLED;
_tmp185=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp184)->f1;_LLEC: return Cyc_CfFlowInfo_place_cmp(
_tmp183,_tmp185)== 0;_LLED: _tmp186=_tmp181.f1;if(_tmp186 <= (void*)3?1:*((int*)
_tmp186)!= 2)goto _LLEF;_LLEE: goto _LLF0;_LLEF: _tmp187=_tmp181.f2;if(_tmp187 <= (
void*)3?1:*((int*)_tmp187)!= 2)goto _LLF1;_LLF0: return 0;_LLF1: _tmp188=_tmp181.f1;
if(_tmp188 <= (void*)3?1:*((int*)_tmp188)!= 4)goto _LLF3;_tmp189=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp188)->f1;_tmp18A=_tmp181.f2;if(_tmp18A <= (void*)3?1:*((int*)_tmp18A)!= 4)
goto _LLF3;_tmp18B=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp18A)->f1;_LLF2:
return _tmp189 == _tmp18B?1:((int(*)(int(*f)(struct _tagged_arr*,void*,void*),
struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_forall_intersect)((int(*)(
struct _tagged_arr*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp189,_tmp18B);_LLF3: _tmp18C=_tmp181.f2;if((int)_tmp18C != 2)goto _LLF5;_LLF4:
return r1 == (void*)1;_LLF5: _tmp18D=_tmp181.f2;if((int)_tmp18D != 0)goto _LLF7;_LLF6:
goto _LLF8;_LLF7: _tmp18E=_tmp181.f2;if((int)_tmp18E != 1)goto _LLF9;_LLF8: return 0;
_LLF9: _tmp18F=_tmp181.f1;if(_tmp18F <= (void*)3?1:*((int*)_tmp18F)!= 1)goto _LLFB;
_tmp190=_tmp181.f2;if(_tmp190 <= (void*)3?1:*((int*)_tmp190)!= 1)goto _LLFB;_LLFA:
goto _LLEA;_LLFB: _tmp191=_tmp181.f1;if(_tmp191 <= (void*)3?1:*((int*)_tmp191)!= 1)
goto _LLFD;_LLFC: return 0;_LLFD: _tmp192=_tmp181.f1;if(_tmp192 <= (void*)3?1:*((int*)
_tmp192)!= 3)goto _LLFF;_tmp193=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp192)->f1;
_tmp194=_tmp181.f2;if(_tmp194 <= (void*)3?1:*((int*)_tmp194)!= 3)goto _LLFF;
_tmp195=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp194)->f1;_LLFE: return Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
_tmp193,_tmp195);_LLFF: _tmp196=_tmp181.f2;if(_tmp196 <= (void*)3?1:*((int*)
_tmp196)!= 3)goto _LL101;_LL100: return 0;_LL101:;_LL102: goto _LLEA;_LLEA:;}{struct
_tuple0 _tmp198=({struct _tuple0 _tmp197;_tmp197.f1=Cyc_CfFlowInfo_initlevel_approx(
r1);_tmp197.f2=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp197;});void*_tmp199;void*
_tmp19A;void*_tmp19B;void*_tmp19C;void*_tmp19D;void*_tmp19E;_LL104: _tmp199=
_tmp198.f1;if((int)_tmp199 != 2)goto _LL106;_tmp19A=_tmp198.f2;if((int)_tmp19A != 2)
goto _LL106;_LL105: return 1;_LL106: _tmp19B=_tmp198.f2;if((int)_tmp19B != 0)goto
_LL108;_LL107: return 1;_LL108: _tmp19C=_tmp198.f1;if((int)_tmp19C != 0)goto _LL10A;
_LL109: return 0;_LL10A: _tmp19D=_tmp198.f2;if((int)_tmp19D != 1)goto _LL10C;_LL10B:
return 1;_LL10C: _tmp19E=_tmp198.f1;if((int)_tmp19E != 1)goto _LL103;_LL10D: return 0;
_LL103:;}}static int Cyc_CfFlowInfo_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*
r1s){if(r1s == r2s)return 1;for(0;r1s != 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*
_tmp19F=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;int found=0;{struct Cyc_List_List*
_tmp1A0=r2s;for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){struct Cyc_CfFlowInfo_Reln*
_tmp1A1=(struct Cyc_CfFlowInfo_Reln*)_tmp1A0->hd;if(_tmp19F == _tmp1A1?1:(_tmp19F->vd
== _tmp1A1->vd?Cyc_CfFlowInfo_same_relop((void*)_tmp19F->rop,(void*)_tmp1A1->rop):
0)){found=1;break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(
void*f1,void*f2){if(f1 == f2)return 1;{struct _tuple0 _tmp1A3=({struct _tuple0 _tmp1A2;
_tmp1A2.f1=f1;_tmp1A2.f2=f2;_tmp1A2;});void*_tmp1A4;void*_tmp1A5;void*_tmp1A6;
struct Cyc_Dict_Dict*_tmp1A7;struct Cyc_List_List*_tmp1A8;void*_tmp1A9;struct Cyc_Dict_Dict*
_tmp1AA;struct Cyc_List_List*_tmp1AB;_LL10F: _tmp1A4=_tmp1A3.f1;if((int)_tmp1A4 != 
0)goto _LL111;_LL110: return 1;_LL111: _tmp1A5=_tmp1A3.f2;if((int)_tmp1A5 != 0)goto
_LL113;_LL112: return 0;_LL113: _tmp1A6=_tmp1A3.f1;if(_tmp1A6 <= (void*)1?1:*((int*)
_tmp1A6)!= 0)goto _LL10E;_tmp1A7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1A6)->f1;_tmp1A8=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1A6)->f2;
_tmp1A9=_tmp1A3.f2;if(_tmp1A9 <= (void*)1?1:*((int*)_tmp1A9)!= 0)goto _LL10E;
_tmp1AA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1A9)->f1;_tmp1AB=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1A9)->f2;_LL114: if(_tmp1A7 == _tmp1AA?
_tmp1A8 == _tmp1AB: 0)return 1;return Cyc_Dict_forall_intersect(Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp1A7,_tmp1AA)?Cyc_CfFlowInfo_relns_approx(_tmp1A8,_tmp1AB): 0;_LL10E:;}}struct
Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*
v){struct Cyc_List_List*p;int found=0;for(p=rs;!found?p != 0: 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp1AC=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp1AC->vd == v){found=1;break;}{
void*_tmp1AD=(void*)_tmp1AC->rop;struct Cyc_Absyn_Vardecl*_tmp1AE;struct Cyc_Absyn_Vardecl*
_tmp1AF;struct Cyc_Absyn_Vardecl*_tmp1B0;_LL116: if(*((int*)_tmp1AD)!= 1)goto
_LL118;_tmp1AE=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1AD)->f1;_LL117:
_tmp1AF=_tmp1AE;goto _LL119;_LL118: if(*((int*)_tmp1AD)!= 2)goto _LL11A;_tmp1AF=((
struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1AD)->f1;_LL119: _tmp1B0=_tmp1AF;goto
_LL11B;_LL11A: if(*((int*)_tmp1AD)!= 4)goto _LL11C;_tmp1B0=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp1AD)->f1;_LL11B: if(v == _tmp1B0)found=1;goto _LL115;_LL11C:;_LL11D: goto _LL115;
_LL115:;}}if(!found)return rs;{struct Cyc_List_List*_tmp1B1=0;for(p=rs;p != 0;p=p->tl){
struct Cyc_CfFlowInfo_Reln*_tmp1B2=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp1B2->vd
!= v){{void*_tmp1B3=(void*)_tmp1B2->rop;struct Cyc_Absyn_Vardecl*_tmp1B4;struct
Cyc_Absyn_Vardecl*_tmp1B5;struct Cyc_Absyn_Vardecl*_tmp1B6;_LL11F: if(*((int*)
_tmp1B3)!= 1)goto _LL121;_tmp1B4=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1B3)->f1;
_LL120: _tmp1B5=_tmp1B4;goto _LL122;_LL121: if(*((int*)_tmp1B3)!= 2)goto _LL123;
_tmp1B5=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1B3)->f1;_LL122: _tmp1B6=
_tmp1B5;goto _LL124;_LL123: if(*((int*)_tmp1B3)!= 4)goto _LL125;_tmp1B6=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp1B3)->f1;_LL124: if(v == _tmp1B6)continue;goto _LL11E;_LL125:;_LL126: goto _LL11E;
_LL11E:;}_tmp1B1=({struct Cyc_List_List*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));
_tmp1B7->hd=_tmp1B2;_tmp1B7->tl=_tmp1B1;_tmp1B7;});}}return _tmp1B1;}}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*
_tmp1B8=(void*)e->r;void*_tmp1B9;struct Cyc_Absyn_Vardecl*_tmp1BA;void*_tmp1BB;
struct Cyc_Absyn_Vardecl*_tmp1BC;void*_tmp1BD;struct Cyc_Absyn_Vardecl*_tmp1BE;
void*_tmp1BF;struct Cyc_Absyn_Vardecl*_tmp1C0;_LL128: if(*((int*)_tmp1B8)!= 1)goto
_LL12A;_tmp1B9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1B8)->f2;if(_tmp1B9 <= (
void*)1?1:*((int*)_tmp1B9)!= 0)goto _LL12A;_tmp1BA=((struct Cyc_Absyn_Global_b_struct*)
_tmp1B9)->f1;_LL129: _tmp1BC=_tmp1BA;goto _LL12B;_LL12A: if(*((int*)_tmp1B8)!= 1)
goto _LL12C;_tmp1BB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1B8)->f2;if(
_tmp1BB <= (void*)1?1:*((int*)_tmp1BB)!= 2)goto _LL12C;_tmp1BC=((struct Cyc_Absyn_Param_b_struct*)
_tmp1BB)->f1;_LL12B: _tmp1BE=_tmp1BC;goto _LL12D;_LL12C: if(*((int*)_tmp1B8)!= 1)
goto _LL12E;_tmp1BD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1B8)->f2;if(
_tmp1BD <= (void*)1?1:*((int*)_tmp1BD)!= 3)goto _LL12E;_tmp1BE=((struct Cyc_Absyn_Local_b_struct*)
_tmp1BD)->f1;_LL12D: _tmp1C0=_tmp1BE;goto _LL12F;_LL12E: if(*((int*)_tmp1B8)!= 1)
goto _LL130;_tmp1BF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1B8)->f2;if(
_tmp1BF <= (void*)1?1:*((int*)_tmp1BF)!= 4)goto _LL130;_tmp1C0=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1BF)->f1;_LL12F: if(!_tmp1C0->escapes)return Cyc_CfFlowInfo_reln_kill_var(r,
_tmp1C0);goto _LL127;_LL130:;_LL131: goto _LL127;_LL127:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,
struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(r,v);{
void*_tmp1C1=(void*)e->r;struct Cyc_Absyn_MallocInfo _tmp1C2;struct Cyc_Absyn_Exp*
_tmp1C3;int _tmp1C4;_LL133: if(*((int*)_tmp1C1)!= 33)goto _LL135;_tmp1C2=((struct
Cyc_Absyn_Malloc_e_struct*)_tmp1C1)->f1;_tmp1C3=_tmp1C2.num_elts;_tmp1C4=_tmp1C2.fat_result;
if(_tmp1C4 != 1)goto _LL135;_LL134: malloc_loop: {void*_tmp1C5=(void*)_tmp1C3->r;
struct Cyc_Absyn_Exp*_tmp1C6;void*_tmp1C7;struct Cyc_Absyn_Vardecl*_tmp1C8;void*
_tmp1C9;struct Cyc_Absyn_Vardecl*_tmp1CA;void*_tmp1CB;struct Cyc_Absyn_Vardecl*
_tmp1CC;void*_tmp1CD;struct Cyc_Absyn_Vardecl*_tmp1CE;_LL138: if(*((int*)_tmp1C5)
!= 13)goto _LL13A;_tmp1C6=((struct Cyc_Absyn_Cast_e_struct*)_tmp1C5)->f2;_LL139:
_tmp1C3=_tmp1C6;goto malloc_loop;_LL13A: if(*((int*)_tmp1C5)!= 1)goto _LL13C;
_tmp1C7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1C5)->f2;if(_tmp1C7 <= (void*)
1?1:*((int*)_tmp1C7)!= 4)goto _LL13C;_tmp1C8=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1C7)->f1;_LL13B: _tmp1CA=_tmp1C8;goto _LL13D;_LL13C: if(*((int*)_tmp1C5)!= 1)
goto _LL13E;_tmp1C9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1C5)->f2;if(
_tmp1C9 <= (void*)1?1:*((int*)_tmp1C9)!= 3)goto _LL13E;_tmp1CA=((struct Cyc_Absyn_Local_b_struct*)
_tmp1C9)->f1;_LL13D: _tmp1CC=_tmp1CA;goto _LL13F;_LL13E: if(*((int*)_tmp1C5)!= 1)
goto _LL140;_tmp1CB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1C5)->f2;if(
_tmp1CB <= (void*)1?1:*((int*)_tmp1CB)!= 2)goto _LL140;_tmp1CC=((struct Cyc_Absyn_Param_b_struct*)
_tmp1CB)->f1;_LL13F: _tmp1CE=_tmp1CC;goto _LL141;_LL140: if(*((int*)_tmp1C5)!= 1)
goto _LL142;_tmp1CD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1C5)->f2;if(
_tmp1CD <= (void*)1?1:*((int*)_tmp1CD)!= 0)goto _LL142;_tmp1CE=((struct Cyc_Absyn_Global_b_struct*)
_tmp1CD)->f1;_LL141: if(_tmp1CE->escapes)return r;return({struct Cyc_List_List*
_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->vd=_tmp1CE;_tmp1D0->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));
_tmp1D1[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct _tmp1D2;_tmp1D2.tag=4;_tmp1D2.f1=
v;_tmp1D2;});_tmp1D1;}));_tmp1D0;});_tmp1CF->tl=r;_tmp1CF;});_LL142:;_LL143:
return r;_LL137:;}_LL135:;_LL136: goto _LL132;_LL132:;}{void*_tmp1D3=Cyc_Tcutil_compress((
void*)v->type);_LL145: if(_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 5)goto _LL147;
_LL146: goto _LL144;_LL147:;_LL148: return r;_LL144:;}loop: {void*_tmp1D4=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp1D5;void*_tmp1D6;int _tmp1D7;void*_tmp1D8;struct Cyc_List_List*
_tmp1D9;struct Cyc_List_List _tmp1DA;struct Cyc_List_List*_tmp1DB;struct Cyc_List_List
_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DD;void*_tmp1DE;struct Cyc_List_List*_tmp1DF;
struct Cyc_List_List _tmp1E0;struct Cyc_Absyn_Exp*_tmp1E1;_LL14A: if(*((int*)_tmp1D4)
!= 13)goto _LL14C;_tmp1D5=((struct Cyc_Absyn_Cast_e_struct*)_tmp1D4)->f2;_LL14B: e=
_tmp1D5;goto loop;_LL14C: if(*((int*)_tmp1D4)!= 0)goto _LL14E;_tmp1D6=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp1D4)->f1;if(_tmp1D6 <= (void*)1?1:*((int*)
_tmp1D6)!= 2)goto _LL14E;_tmp1D7=((struct Cyc_Absyn_Int_c_struct*)_tmp1D6)->f2;
_LL14D: return({struct Cyc_List_List*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3->vd=v;
_tmp1E3->rop=(void*)((void*)({struct Cyc_CfFlowInfo_EqualConst_struct*_tmp1E4=
_cycalloc_atomic(sizeof(*_tmp1E4));_tmp1E4[0]=({struct Cyc_CfFlowInfo_EqualConst_struct
_tmp1E5;_tmp1E5.tag=0;_tmp1E5.f1=(unsigned int)_tmp1D7;_tmp1E5;});_tmp1E4;}));
_tmp1E3;});_tmp1E2->tl=r;_tmp1E2;});_LL14E: if(*((int*)_tmp1D4)!= 3)goto _LL150;
_tmp1D8=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1D4)->f1;if((int)_tmp1D8 != 
4)goto _LL150;_tmp1D9=((struct Cyc_Absyn_Primop_e_struct*)_tmp1D4)->f2;if(_tmp1D9
== 0)goto _LL150;_tmp1DA=*_tmp1D9;_tmp1DB=_tmp1DA.tl;if(_tmp1DB == 0)goto _LL150;
_tmp1DC=*_tmp1DB;_tmp1DD=(struct Cyc_Absyn_Exp*)_tmp1DC.hd;_LL14F:{void*_tmp1E6=(
void*)_tmp1DD->r;void*_tmp1E7;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List
_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EA;_LL155: if(*((int*)_tmp1E6)!= 3)goto _LL157;
_tmp1E7=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1E6)->f1;if((int)_tmp1E7 != 
19)goto _LL157;_tmp1E8=((struct Cyc_Absyn_Primop_e_struct*)_tmp1E6)->f2;if(_tmp1E8
== 0)goto _LL157;_tmp1E9=*_tmp1E8;_tmp1EA=(struct Cyc_Absyn_Exp*)_tmp1E9.hd;_LL156:{
void*_tmp1EB=(void*)_tmp1EA->r;void*_tmp1EC;struct Cyc_Absyn_Vardecl*_tmp1ED;void*
_tmp1EE;struct Cyc_Absyn_Vardecl*_tmp1EF;void*_tmp1F0;struct Cyc_Absyn_Vardecl*
_tmp1F1;void*_tmp1F2;struct Cyc_Absyn_Vardecl*_tmp1F3;_LL15A: if(*((int*)_tmp1EB)
!= 1)goto _LL15C;_tmp1EC=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1EB)->f2;if(
_tmp1EC <= (void*)1?1:*((int*)_tmp1EC)!= 0)goto _LL15C;_tmp1ED=((struct Cyc_Absyn_Global_b_struct*)
_tmp1EC)->f1;_LL15B: _tmp1EF=_tmp1ED;goto _LL15D;_LL15C: if(*((int*)_tmp1EB)!= 1)
goto _LL15E;_tmp1EE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1EB)->f2;if(
_tmp1EE <= (void*)1?1:*((int*)_tmp1EE)!= 3)goto _LL15E;_tmp1EF=((struct Cyc_Absyn_Local_b_struct*)
_tmp1EE)->f1;_LL15D: _tmp1F1=_tmp1EF;goto _LL15F;_LL15E: if(*((int*)_tmp1EB)!= 1)
goto _LL160;_tmp1F0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1EB)->f2;if(
_tmp1F0 <= (void*)1?1:*((int*)_tmp1F0)!= 2)goto _LL160;_tmp1F1=((struct Cyc_Absyn_Param_b_struct*)
_tmp1F0)->f1;_LL15F: _tmp1F3=_tmp1F1;goto _LL161;_LL160: if(*((int*)_tmp1EB)!= 1)
goto _LL162;_tmp1F2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1EB)->f2;if(
_tmp1F2 <= (void*)1?1:*((int*)_tmp1F2)!= 4)goto _LL162;_tmp1F3=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1F2)->f1;_LL161: if(_tmp1F3->escapes)return r;return({struct Cyc_List_List*
_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->vd=v;_tmp1F5->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_LessSize_struct*_tmp1F6=_cycalloc(sizeof(*_tmp1F6));_tmp1F6[
0]=({struct Cyc_CfFlowInfo_LessSize_struct _tmp1F7;_tmp1F7.tag=2;_tmp1F7.f1=
_tmp1F3;_tmp1F7;});_tmp1F6;}));_tmp1F5;});_tmp1F4->tl=r;_tmp1F4;});_LL162:;
_LL163: goto _LL159;_LL159:;}goto _LL154;_LL157:;_LL158: goto _LL154;_LL154:;}goto
_LL149;_LL150: if(*((int*)_tmp1D4)!= 3)goto _LL152;_tmp1DE=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D4)->f1;if((int)_tmp1DE != 19)goto _LL152;_tmp1DF=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D4)->f2;if(_tmp1DF == 0)goto _LL152;_tmp1E0=*_tmp1DF;_tmp1E1=(struct Cyc_Absyn_Exp*)
_tmp1E0.hd;_LL151:{void*_tmp1F8=(void*)_tmp1E1->r;void*_tmp1F9;struct Cyc_Absyn_Vardecl*
_tmp1FA;void*_tmp1FB;struct Cyc_Absyn_Vardecl*_tmp1FC;void*_tmp1FD;struct Cyc_Absyn_Vardecl*
_tmp1FE;void*_tmp1FF;struct Cyc_Absyn_Vardecl*_tmp200;_LL165: if(*((int*)_tmp1F8)
!= 1)goto _LL167;_tmp1F9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F8)->f2;if(
_tmp1F9 <= (void*)1?1:*((int*)_tmp1F9)!= 0)goto _LL167;_tmp1FA=((struct Cyc_Absyn_Global_b_struct*)
_tmp1F9)->f1;_LL166: _tmp1FC=_tmp1FA;goto _LL168;_LL167: if(*((int*)_tmp1F8)!= 1)
goto _LL169;_tmp1FB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F8)->f2;if(
_tmp1FB <= (void*)1?1:*((int*)_tmp1FB)!= 3)goto _LL169;_tmp1FC=((struct Cyc_Absyn_Local_b_struct*)
_tmp1FB)->f1;_LL168: _tmp1FE=_tmp1FC;goto _LL16A;_LL169: if(*((int*)_tmp1F8)!= 1)
goto _LL16B;_tmp1FD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F8)->f2;if(
_tmp1FD <= (void*)1?1:*((int*)_tmp1FD)!= 2)goto _LL16B;_tmp1FE=((struct Cyc_Absyn_Param_b_struct*)
_tmp1FD)->f1;_LL16A: _tmp200=_tmp1FE;goto _LL16C;_LL16B: if(*((int*)_tmp1F8)!= 1)
goto _LL16D;_tmp1FF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F8)->f2;if(
_tmp1FF <= (void*)1?1:*((int*)_tmp1FF)!= 4)goto _LL16D;_tmp200=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1FF)->f1;_LL16C: if(_tmp200->escapes)return r;return({struct Cyc_List_List*
_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202->vd=v;_tmp202->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp203=_cycalloc(sizeof(*_tmp203));
_tmp203[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct _tmp204;_tmp204.tag=4;_tmp204.f1=
_tmp200;_tmp204;});_tmp203;}));_tmp202;});_tmp201->tl=r;_tmp201;});_LL16D:;
_LL16E: goto _LL164;_LL164:;}goto _LL149;_LL152:;_LL153: goto _LL149;_LL149:;}return r;}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct Cyc_List_List*r,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*_tmp205=(void*)e1->r;void*_tmp206;
struct Cyc_Absyn_Vardecl*_tmp207;void*_tmp208;struct Cyc_Absyn_Vardecl*_tmp209;
void*_tmp20A;struct Cyc_Absyn_Vardecl*_tmp20B;void*_tmp20C;struct Cyc_Absyn_Vardecl*
_tmp20D;_LL170: if(*((int*)_tmp205)!= 1)goto _LL172;_tmp206=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp205)->f2;if(_tmp206 <= (void*)1?1:*((int*)_tmp206)!= 0)goto _LL172;_tmp207=((
struct Cyc_Absyn_Global_b_struct*)_tmp206)->f1;_LL171: _tmp209=_tmp207;goto _LL173;
_LL172: if(*((int*)_tmp205)!= 1)goto _LL174;_tmp208=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp205)->f2;if(_tmp208 <= (void*)1?1:*((int*)_tmp208)!= 2)goto _LL174;_tmp209=((
struct Cyc_Absyn_Param_b_struct*)_tmp208)->f1;_LL173: _tmp20B=_tmp209;goto _LL175;
_LL174: if(*((int*)_tmp205)!= 1)goto _LL176;_tmp20A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp205)->f2;if(_tmp20A <= (void*)1?1:*((int*)_tmp20A)!= 3)goto _LL176;_tmp20B=((
struct Cyc_Absyn_Local_b_struct*)_tmp20A)->f1;_LL175: _tmp20D=_tmp20B;goto _LL177;
_LL176: if(*((int*)_tmp205)!= 1)goto _LL178;_tmp20C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp205)->f2;if(_tmp20C <= (void*)1?1:*((int*)_tmp20C)!= 4)goto _LL178;_tmp20D=((
struct Cyc_Absyn_Pat_b_struct*)_tmp20C)->f1;_LL177: if(!_tmp20D->escapes)return Cyc_CfFlowInfo_reln_assign_var(
r,_tmp20D,e2);goto _LL16F;_LL178:;_LL179: goto _LL16F;_LL16F:;}return r;}static void
Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*r){({struct Cyc_String_pa_struct
_tmp210;_tmp210.tag=0;_tmp210.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string((
r->vd)->name));{void*_tmp20E[1]={& _tmp210};Cyc_fprintf(Cyc_stderr,({const char*
_tmp20F="%s";_tag_arr(_tmp20F,sizeof(char),_get_zero_arr_size(_tmp20F,3));}),
_tag_arr(_tmp20E,sizeof(void*),1));}});{void*_tmp211=(void*)r->rop;unsigned int
_tmp212;struct Cyc_Absyn_Vardecl*_tmp213;struct Cyc_Absyn_Vardecl*_tmp214;
unsigned int _tmp215;struct Cyc_Absyn_Vardecl*_tmp216;_LL17B: if(*((int*)_tmp211)!= 
0)goto _LL17D;_tmp212=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp211)->f1;
_LL17C:({struct Cyc_Int_pa_struct _tmp219;_tmp219.tag=1;_tmp219.f1=(unsigned int)((
int)_tmp212);{void*_tmp217[1]={& _tmp219};Cyc_fprintf(Cyc_stderr,({const char*
_tmp218="==%d";_tag_arr(_tmp218,sizeof(char),_get_zero_arr_size(_tmp218,5));}),
_tag_arr(_tmp217,sizeof(void*),1));}});goto _LL17A;_LL17D: if(*((int*)_tmp211)!= 1)
goto _LL17F;_tmp213=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp211)->f1;_LL17E:({
struct Cyc_String_pa_struct _tmp21C;_tmp21C.tag=0;_tmp21C.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp213->name));{void*_tmp21A[1]={&
_tmp21C};Cyc_fprintf(Cyc_stderr,({const char*_tmp21B="<%s";_tag_arr(_tmp21B,
sizeof(char),_get_zero_arr_size(_tmp21B,4));}),_tag_arr(_tmp21A,sizeof(void*),1));}});
goto _LL17A;_LL17F: if(*((int*)_tmp211)!= 2)goto _LL181;_tmp214=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp211)->f1;_LL180:({struct Cyc_String_pa_struct _tmp21F;_tmp21F.tag=0;_tmp21F.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp214->name));{
void*_tmp21D[1]={& _tmp21F};Cyc_fprintf(Cyc_stderr,({const char*_tmp21E="<%s.size";
_tag_arr(_tmp21E,sizeof(char),_get_zero_arr_size(_tmp21E,9));}),_tag_arr(_tmp21D,
sizeof(void*),1));}});goto _LL17A;_LL181: if(*((int*)_tmp211)!= 3)goto _LL183;
_tmp215=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp211)->f1;_LL182:({struct Cyc_Int_pa_struct
_tmp222;_tmp222.tag=1;_tmp222.f1=(unsigned int)((int)_tmp215);{void*_tmp220[1]={&
_tmp222};Cyc_fprintf(Cyc_stderr,({const char*_tmp221="<%d";_tag_arr(_tmp221,
sizeof(char),_get_zero_arr_size(_tmp221,4));}),_tag_arr(_tmp220,sizeof(void*),1));}});
goto _LL17A;_LL183: if(*((int*)_tmp211)!= 4)goto _LL17A;_tmp216=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp211)->f1;_LL184:({struct Cyc_String_pa_struct _tmp225;_tmp225.tag=0;_tmp225.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp216->name));{
void*_tmp223[1]={& _tmp225};Cyc_fprintf(Cyc_stderr,({const char*_tmp224="<=%s.size";
_tag_arr(_tmp224,sizeof(char),_get_zero_arr_size(_tmp224,10));}),_tag_arr(
_tmp223,sizeof(void*),1));}});goto _LL17A;_LL17A:;}}void Cyc_CfFlowInfo_print_relns(
struct Cyc_List_List*r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)
r->hd);if(r->tl != 0)({void*_tmp226[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp227=",";_tag_arr(_tmp227,sizeof(char),_get_zero_arr_size(_tmp227,2));}),
_tag_arr(_tmp226,sizeof(void*),0));});}}static int Cyc_CfFlowInfo_contains_region(
struct Cyc_Absyn_Tvar*rgn,void*t){void*_tmp228=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp229;struct Cyc_Absyn_TunionInfo _tmp22A;struct Cyc_List_List*_tmp22B;void*
_tmp22C;struct Cyc_List_List*_tmp22D;struct Cyc_Absyn_AggrInfo _tmp22E;struct Cyc_List_List*
_tmp22F;struct Cyc_Absyn_TunionFieldInfo _tmp230;struct Cyc_List_List*_tmp231;
struct Cyc_List_List*_tmp232;struct Cyc_Absyn_PtrInfo _tmp233;void*_tmp234;struct
Cyc_Absyn_PtrAtts _tmp235;void*_tmp236;struct Cyc_List_List*_tmp237;struct Cyc_List_List*
_tmp238;struct Cyc_Absyn_ArrayInfo _tmp239;void*_tmp23A;void*_tmp23B;void*_tmp23C;
void*_tmp23D;_LL186: if((int)_tmp228 != 0)goto _LL188;_LL187: goto _LL189;_LL188: if(
_tmp228 <= (void*)3?1:*((int*)_tmp228)!= 5)goto _LL18A;_LL189: goto _LL18B;_LL18A:
if((int)_tmp228 != 1)goto _LL18C;_LL18B: goto _LL18D;_LL18C: if(_tmp228 <= (void*)3?1:*((
int*)_tmp228)!= 6)goto _LL18E;_LL18D: goto _LL18F;_LL18E: if(_tmp228 <= (void*)3?1:*((
int*)_tmp228)!= 12)goto _LL190;_LL18F: goto _LL191;_LL190: if(_tmp228 <= (void*)3?1:*((
int*)_tmp228)!= 13)goto _LL192;_LL191: goto _LL193;_LL192: if(_tmp228 <= (void*)3?1:*((
int*)_tmp228)!= 14)goto _LL194;_LL193: goto _LL195;_LL194: if(_tmp228 <= (void*)3?1:*((
int*)_tmp228)!= 17)goto _LL196;_LL195: goto _LL197;_LL196: if(_tmp228 <= (void*)3?1:*((
int*)_tmp228)!= 18)goto _LL198;_LL197: goto _LL199;_LL198: if((int)_tmp228 != 2)goto
_LL19A;_LL199: goto _LL19B;_LL19A: if(_tmp228 <= (void*)3?1:*((int*)_tmp228)!= 0)
goto _LL19C;_LL19B: return 0;_LL19C: if(_tmp228 <= (void*)3?1:*((int*)_tmp228)!= 1)
goto _LL19E;_tmp229=((struct Cyc_Absyn_VarType_struct*)_tmp228)->f1;_LL19D: return
Cyc_Absyn_tvar_cmp(_tmp229,rgn)== 0;_LL19E: if(_tmp228 <= (void*)3?1:*((int*)
_tmp228)!= 2)goto _LL1A0;_tmp22A=((struct Cyc_Absyn_TunionType_struct*)_tmp228)->f1;
_tmp22B=_tmp22A.targs;_tmp22C=(void*)_tmp22A.rgn;_LL19F: if(Cyc_CfFlowInfo_contains_region(
rgn,_tmp22C))return 1;_tmp22D=_tmp22B;goto _LL1A1;_LL1A0: if(_tmp228 <= (void*)3?1:*((
int*)_tmp228)!= 16)goto _LL1A2;_tmp22D=((struct Cyc_Absyn_TypedefType_struct*)
_tmp228)->f2;_LL1A1: _tmp22F=_tmp22D;goto _LL1A3;_LL1A2: if(_tmp228 <= (void*)3?1:*((
int*)_tmp228)!= 10)goto _LL1A4;_tmp22E=((struct Cyc_Absyn_AggrType_struct*)_tmp228)->f1;
_tmp22F=_tmp22E.targs;_LL1A3: _tmp231=_tmp22F;goto _LL1A5;_LL1A4: if(_tmp228 <= (
void*)3?1:*((int*)_tmp228)!= 3)goto _LL1A6;_tmp230=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp228)->f1;_tmp231=_tmp230.targs;_LL1A5: _tmp232=_tmp231;goto _LL1A7;_LL1A6: if(
_tmp228 <= (void*)3?1:*((int*)_tmp228)!= 20)goto _LL1A8;_tmp232=((struct Cyc_Absyn_JoinEff_struct*)
_tmp228)->f1;_LL1A7: return((int(*)(int(*pred)(struct Cyc_Absyn_Tvar*,void*),
struct Cyc_Absyn_Tvar*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,
rgn,_tmp232);_LL1A8: if(_tmp228 <= (void*)3?1:*((int*)_tmp228)!= 4)goto _LL1AA;
_tmp233=((struct Cyc_Absyn_PointerType_struct*)_tmp228)->f1;_tmp234=(void*)
_tmp233.elt_typ;_tmp235=_tmp233.ptr_atts;_tmp236=(void*)_tmp235.rgn;_LL1A9:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp236)?1: Cyc_CfFlowInfo_contains_region(
rgn,_tmp234);_LL1AA: if(_tmp228 <= (void*)3?1:*((int*)_tmp228)!= 8)goto _LL1AC;
_LL1AB: return 0;_LL1AC: if(_tmp228 <= (void*)3?1:*((int*)_tmp228)!= 9)goto _LL1AE;
_tmp237=((struct Cyc_Absyn_TupleType_struct*)_tmp228)->f1;_LL1AD: for(0;_tmp237 != 
0;_tmp237=_tmp237->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((struct _tuple4*)
_tmp237->hd)).f2))return 1;}return 0;_LL1AE: if(_tmp228 <= (void*)3?1:*((int*)
_tmp228)!= 11)goto _LL1B0;_tmp238=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp228)->f2;
_LL1AF: for(0;_tmp238 != 0;_tmp238=_tmp238->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp238->hd)->type))return 1;}return 0;
_LL1B0: if(_tmp228 <= (void*)3?1:*((int*)_tmp228)!= 7)goto _LL1B2;_tmp239=((struct
Cyc_Absyn_ArrayType_struct*)_tmp228)->f1;_tmp23A=(void*)_tmp239.elt_type;_LL1B1:
_tmp23B=_tmp23A;goto _LL1B3;_LL1B2: if(_tmp228 <= (void*)3?1:*((int*)_tmp228)!= 19)
goto _LL1B4;_tmp23B=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp228)->f1;
_LL1B3: _tmp23C=_tmp23B;goto _LL1B5;_LL1B4: if(_tmp228 <= (void*)3?1:*((int*)_tmp228)
!= 21)goto _LL1B6;_tmp23C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp228)->f1;
_LL1B5: return Cyc_CfFlowInfo_contains_region(rgn,_tmp23C);_LL1B6: if(_tmp228 <= (
void*)3?1:*((int*)_tmp228)!= 15)goto _LL185;_tmp23D=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp228)->f1;_LL1B7: return 0;_LL185:;}struct _tuple7{struct Cyc_Dict_Dict*f1;struct
Cyc_Absyn_Tvar*f2;};static void Cyc_CfFlowInfo_kill_root(struct _tuple7*env,void*
root,void*rval){struct _tuple7 _tmp23F;struct Cyc_Dict_Dict*_tmp240;struct Cyc_Dict_Dict**
_tmp241;struct Cyc_Absyn_Tvar*_tmp242;struct _tuple7*_tmp23E=env;_tmp23F=*_tmp23E;
_tmp240=_tmp23F.f1;_tmp241=(struct Cyc_Dict_Dict**)&(*_tmp23E).f1;_tmp242=_tmp23F.f2;{
void*_tmp243=root;struct Cyc_Absyn_Vardecl*_tmp244;void*_tmp245;_LL1B9: if(*((int*)
_tmp243)!= 0)goto _LL1BB;_tmp244=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp243)->f1;
_LL1BA: if(Cyc_CfFlowInfo_contains_region(_tmp242,(void*)_tmp244->type))rval=Cyc_CfFlowInfo_typ_to_absrval((
void*)_tmp244->type,Cyc_CfFlowInfo_unknown_none);*_tmp241=Cyc_Dict_insert(*
_tmp241,root,rval);goto _LL1B8;_LL1BB: if(*((int*)_tmp243)!= 1)goto _LL1BD;_tmp245=(
void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp243)->f2;_LL1BC: if(!Cyc_CfFlowInfo_contains_region(
_tmp242,_tmp245))*_tmp241=Cyc_Dict_insert(*_tmp241,root,rval);goto _LL1B8;_LL1BD:
if(*((int*)_tmp243)!= 2)goto _LL1B8;_LL1BE: goto _LL1B8;_LL1B8:;}}static struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_Dict_Dict*fd,void*rgn){struct Cyc_Absyn_Tvar*
rgn_tvar;{void*_tmp246=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp247;
_LL1C0: if(_tmp246 <= (void*)3?1:*((int*)_tmp246)!= 1)goto _LL1C2;_tmp247=((struct
Cyc_Absyn_VarType_struct*)_tmp246)->f1;_LL1C1: rgn_tvar=_tmp247;goto _LL1BF;_LL1C2:;
_LL1C3:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp248=_cycalloc(
sizeof(*_tmp248));_tmp248[0]=({struct Cyc_Core_Impossible_struct _tmp249;_tmp249.tag=
Cyc_Core_Impossible;_tmp249.f1=({const char*_tmp24A="kill_flowdict_region";
_tag_arr(_tmp24A,sizeof(char),_get_zero_arr_size(_tmp24A,21));});_tmp249;});
_tmp248;}));goto _LL1BF;_LL1BF:;}{struct _tuple7 env=({struct _tuple7 _tmp24B;_tmp24B.f1=
Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp24B.f2=rgn_tvar;_tmp24B;});((void(*)(
void(*f)(struct _tuple7*,void*,void*),struct _tuple7*env,struct Cyc_Dict_Dict*d))
Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return env.f1;}}static struct
Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*relns,void*
rgn){return 0;}void*Cyc_CfFlowInfo_kill_flow_region(void*f,void*rgn){void*_tmp24C=
f;struct Cyc_Dict_Dict*_tmp24D;struct Cyc_List_List*_tmp24E;_LL1C5: if((int)_tmp24C
!= 0)goto _LL1C7;_LL1C6: return f;_LL1C7: if(_tmp24C <= (void*)1?1:*((int*)_tmp24C)!= 
0)goto _LL1C4;_tmp24D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp24C)->f1;
_tmp24E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp24C)->f2;_LL1C8: {struct
Cyc_Dict_Dict*_tmp24F=Cyc_CfFlowInfo_kill_flowdict_region(_tmp24D,rgn);struct Cyc_List_List*
_tmp250=Cyc_CfFlowInfo_kill_relns_region(_tmp24E,rgn);return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp251=_cycalloc(sizeof(*_tmp251));_tmp251[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp252;_tmp252.tag=0;_tmp252.f1=_tmp24F;_tmp252.f2=_tmp250;_tmp252;});_tmp251;});}
_LL1C4:;}
