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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(
struct _tuple0*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*
x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*
r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct
_tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[
14];struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};
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
_tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[
15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual();struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_unknown_conref;
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(
struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;
extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_exn_typ;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_star_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_tagged_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);void*Cyc_Absyn_pointer_expand(void*);int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct
Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
void*info);int Cyc_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);struct Cyc___cycFILE;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned int*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct _tagged_arr Cyc_vrprintf(struct _RegionHandle*,struct _tagged_arr,struct
_tagged_arr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];
struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
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
struct _tuple1*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*
te);int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*
Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);int Cyc_Tcutil_kind_leq(
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
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
struct _tuple4{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple5{
struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*Cyc_Tcutil_r_make_inst_var(
struct _tuple4*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);
void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);
int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);struct _tuple6{int f1;void*f2;};struct _tuple6 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);struct
_tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);int Cyc_Evexp_okay_szofarg(void*t);void Cyc_Tcstmt_tcStmt(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(
struct Cyc_Tcenv_Tenv*,struct _tagged_arr,struct Cyc_Position_Segment*);struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs(struct Cyc_Tcenv_Tenv*,struct _tagged_arr,struct Cyc_Position_Segment*);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tagged_arr msg_part);struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;void*rop;};struct Cyc_CfFlowInfo_TagCmp{
void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[15];struct Cyc_CfFlowInfo_HasTagCmps_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[11];extern char
Cyc_CfFlowInfo_NotZero[12];struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct
Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[13];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*
f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct
Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict*
f1;struct Cyc_List_List*f2;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tagged_arr msg,struct
_tagged_arr ap){({void*_tmp0[0]={};Cyc_Tcutil_terr(loc,(struct _tagged_arr)Cyc_vrprintf(
Cyc_Core_heap_region,msg,ap),_tag_arr(_tmp0,sizeof(void*),0));});if(topt == 0)
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp1=_cycalloc(sizeof(*_tmp1));
_tmp1->v=Cyc_Tcenv_lookup_type_vars(te);_tmp1;}));else{return*topt;}}static void
Cyc_Tcexp_resolve_unknown_id(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,
struct _tuple1*q){struct _handler_cons _tmp2;_push_handler(& _tmp2);{int _tmp4=0;if(
setjmp(_tmp2.handler))_tmp4=1;if(!_tmp4){{void*_tmp5=Cyc_Tcenv_lookup_ordinary(
te,e->loc,q);void*_tmp6;struct Cyc_Absyn_Enumdecl*_tmp7;struct Cyc_Absyn_Enumfield*
_tmp8;void*_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;struct Cyc_Absyn_Tuniondecl*
_tmpB;struct Cyc_Absyn_Tunionfield*_tmpC;_LL1: if(*((int*)_tmp5)!= 0)goto _LL3;
_tmp6=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp5)->f1;_LL2:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Var_e_struct*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD[0]=({
struct Cyc_Absyn_Var_e_struct _tmpE;_tmpE.tag=1;_tmpE.f1=q;_tmpE.f2=(void*)_tmp6;
_tmpE;});_tmpD;})));goto _LL0;_LL3: if(*((int*)_tmp5)!= 3)goto _LL5;_tmp7=((struct
Cyc_Tcenv_EnumRes_struct*)_tmp5)->f1;_tmp8=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp5)->f2;_LL4:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Enum_e_struct*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Absyn_Enum_e_struct _tmp10;
_tmp10.tag=31;_tmp10.f1=q;_tmp10.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7;_tmp10.f3=(
struct Cyc_Absyn_Enumfield*)_tmp8;_tmp10;});_tmpF;})));goto _LL0;_LL5: if(*((int*)
_tmp5)!= 4)goto _LL7;_tmp9=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f1;
_tmpA=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f2;_LL6:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp11=_cycalloc(sizeof(*_tmp11));
_tmp11[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp12;_tmp12.tag=32;_tmp12.f1=q;
_tmp12.f2=(void*)_tmp9;_tmp12.f3=(struct Cyc_Absyn_Enumfield*)_tmpA;_tmp12;});
_tmp11;})));goto _LL0;_LL7: if(*((int*)_tmp5)!= 2)goto _LL9;_tmpB=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp5)->f1;_tmpC=((struct Cyc_Tcenv_TunionRes_struct*)_tmp5)->f2;_LL8:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*_tmp13=_cycalloc(sizeof(*_tmp13));
_tmp13[0]=({struct Cyc_Absyn_Tunion_e_struct _tmp14;_tmp14.tag=30;_tmp14.f1=0;
_tmp14.f2=_tmpB;_tmp14.f3=_tmpC;_tmp14;});_tmp13;})));goto _LL0;_LL9: if(*((int*)
_tmp5)!= 1)goto _LL0;_LLA:({struct Cyc_String_pa_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(q));{void*_tmp15[1]={&
_tmp17};Cyc_Tcutil_terr(e->loc,({const char*_tmp16="bad occurrence of type name %s";
_tag_arr(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,31));}),_tag_arr(_tmp15,
sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*
_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=({struct Cyc_Absyn_Var_e_struct _tmp19;
_tmp19.tag=1;_tmp19.f1=q;_tmp19.f2=(void*)((void*)0);_tmp19;});_tmp18;})));goto
_LL0;_LL0:;};_pop_handler();}else{void*_tmp3=(void*)_exn_thrown;void*_tmp1B=
_tmp3;_LLC: if(_tmp1B != Cyc_Dict_Absent)goto _LLE;_LLD:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Var_e_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=({
struct Cyc_Absyn_Var_e_struct _tmp1D;_tmp1D.tag=1;_tmp1D.f1=q;_tmp1D.f2=(void*)((
void*)0);_tmp1D;});_tmp1C;})));goto _LLB;_LLE:;_LLF:(void)_throw(_tmp1B);_LLB:;}}}
struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){return({struct _tuple8*
_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->f1=0;_tmp1E->f2=e;_tmp1E;});}static
void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp1F=(void*)e1->r;struct
_tuple1*_tmp20;_LL11: if(*((int*)_tmp1F)!= 2)goto _LL13;_tmp20=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1F)->f1;_LL12: {struct _handler_cons _tmp21;_push_handler(& _tmp21);{int _tmp23=
0;if(setjmp(_tmp21.handler))_tmp23=1;if(!_tmp23){{void*_tmp24=Cyc_Tcenv_lookup_ordinary(
te,e1->loc,_tmp20);void*_tmp25;struct Cyc_Absyn_Tuniondecl*_tmp26;struct Cyc_Absyn_Tunionfield*
_tmp27;struct Cyc_Absyn_Aggrdecl*_tmp28;_LL16: if(*((int*)_tmp24)!= 0)goto _LL18;
_tmp25=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp24)->f1;_LL17:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp29=_cycalloc(sizeof(*_tmp29));
_tmp29[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp2A;_tmp2A.tag=9;_tmp2A.f1=e1;
_tmp2A.f2=es;_tmp2A.f3=0;_tmp2A;});_tmp29;})));_npop_handler(0);return;_LL18: if(*((
int*)_tmp24)!= 2)goto _LL1A;_tmp26=((struct Cyc_Tcenv_TunionRes_struct*)_tmp24)->f1;
_tmp27=((struct Cyc_Tcenv_TunionRes_struct*)_tmp24)->f2;_LL19: if(_tmp27->typs == 0)({
struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp27->name));{void*_tmp2B[1]={&
_tmp2D};Cyc_Tcutil_terr(e->loc,({const char*_tmp2C="%s is a constant, not a function";
_tag_arr(_tmp2C,sizeof(char),_get_zero_arr_size(_tmp2C,33));}),_tag_arr(_tmp2B,
sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*
_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E[0]=({struct Cyc_Absyn_Tunion_e_struct
_tmp2F;_tmp2F.tag=30;_tmp2F.f1=es;_tmp2F.f2=_tmp26;_tmp2F.f3=_tmp27;_tmp2F;});
_tmp2E;})));_npop_handler(0);return;_LL1A: if(*((int*)_tmp24)!= 1)goto _LL1C;
_tmp28=((struct Cyc_Tcenv_AggrRes_struct*)_tmp24)->f1;_LL1B: {struct Cyc_List_List*
_tmp30=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Struct_e_struct*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31[
0]=({struct Cyc_Absyn_Struct_e_struct _tmp32;_tmp32.tag=28;_tmp32.f1=_tmp28->name;
_tmp32.f2=0;_tmp32.f3=_tmp30;_tmp32.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp28;_tmp32;});
_tmp31;})));_npop_handler(0);return;}_LL1C: if(*((int*)_tmp24)!= 4)goto _LL1E;
_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp24)!= 3)goto _LL15;_LL1F:({struct Cyc_String_pa_struct
_tmp35;_tmp35.tag=0;_tmp35.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp20));{void*_tmp33[1]={& _tmp35};Cyc_Tcutil_terr(e->loc,({const char*_tmp34="%s is an enum constructor, not a function";
_tag_arr(_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,42));}),_tag_arr(_tmp33,
sizeof(void*),1));}});_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*
_tmp22=(void*)_exn_thrown;void*_tmp37=_tmp22;_LL21: if(_tmp37 != Cyc_Dict_Absent)
goto _LL23;_LL22:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp39;_tmp39.tag=9;_tmp39.f1=e1;_tmp39.f2=es;_tmp39.f3=0;_tmp39;});_tmp38;})));
return;_LL23:;_LL24:(void)_throw(_tmp37);_LL20:;}}}_LL13:;_LL14:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));
_tmp3A[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp3B;_tmp3B.tag=9;_tmp3B.f1=e1;
_tmp3B.f2=es;_tmp3B.f3=0;_tmp3B;});_tmp3A;})));return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des){if(topt == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({struct Cyc_Absyn_Array_e_struct
_tmp3D;_tmp3D.tag=26;_tmp3D.f1=des;_tmp3D;});_tmp3C;})));return;}{void*t=*topt;
void*_tmp3E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp3F;void*_tmp40;
struct Cyc_Absyn_ArrayInfo _tmp41;void*_tmp42;struct Cyc_Absyn_Tqual _tmp43;_LL26:
if(_tmp3E <= (void*)3?1:*((int*)_tmp3E)!= 10)goto _LL28;_tmp3F=((struct Cyc_Absyn_AggrType_struct*)
_tmp3E)->f1;_tmp40=(void*)_tmp3F.aggr_info;_LL27:{void*_tmp44=_tmp40;struct Cyc_Absyn_Aggrdecl**
_tmp45;struct Cyc_Absyn_Aggrdecl*_tmp46;_LL2F: if(*((int*)_tmp44)!= 0)goto _LL31;
_LL30:({void*_tmp47[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp48="struct type not properly set";_tag_arr(_tmp48,sizeof(char),
_get_zero_arr_size(_tmp48,29));}),_tag_arr(_tmp47,sizeof(void*),0));});_LL31: if(*((
int*)_tmp44)!= 1)goto _LL2E;_tmp45=((struct Cyc_Absyn_KnownAggr_struct*)_tmp44)->f1;
_tmp46=*_tmp45;_LL32:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp4A;_tmp4A.tag=28;_tmp4A.f1=_tmp46->name;_tmp4A.f2=0;_tmp4A.f3=des;_tmp4A.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp46;_tmp4A;});_tmp49;})));_LL2E:;}goto _LL25;_LL28:
if(_tmp3E <= (void*)3?1:*((int*)_tmp3E)!= 7)goto _LL2A;_tmp41=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3E)->f1;_tmp42=(void*)_tmp41.elt_type;_tmp43=_tmp41.tq;_LL29:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));
_tmp4B[0]=({struct Cyc_Absyn_Array_e_struct _tmp4C;_tmp4C.tag=26;_tmp4C.f1=des;
_tmp4C;});_tmp4B;})));goto _LL25;_LL2A: if(_tmp3E <= (void*)3?1:*((int*)_tmp3E)!= 
11)goto _LL2C;_LL2B:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonStruct_e_struct*
_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D[0]=({struct Cyc_Absyn_AnonStruct_e_struct
_tmp4E;_tmp4E.tag=29;_tmp4E.f1=(void*)t;_tmp4E.f2=des;_tmp4E;});_tmp4D;})));goto
_LL25;_LL2C:;_LL2D:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_Array_e_struct
_tmp50;_tmp50.tag=26;_tmp50.f1=des;_tmp50;});_tmp4F;})));goto _LL25;_LL25:;}}
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct
Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_tagged_arr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(te,(
void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct
_tmp54;_tmp54.tag=0;_tmp54.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmp53;_tmp53.tag=0;_tmp53.f1=(struct _tagged_arr)((struct _tagged_arr)msg_part);{
void*_tmp51[2]={& _tmp53,& _tmp54};Cyc_Tcutil_terr(e->loc,({const char*_tmp52="test of %s has type %s instead of integral or * type";
_tag_arr(_tmp52,sizeof(char),_get_zero_arr_size(_tmp52,53));}),_tag_arr(_tmp51,
sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,void*c,struct Cyc_Absyn_Exp*e){void*t;{
void*_tmp55=c;void*_tmp56;void*_tmp57;void*_tmp58;void*_tmp59;void*_tmp5A;void*
_tmp5B;int _tmp5C;struct _tagged_arr _tmp5D;_LL34: if(_tmp55 <= (void*)1?1:*((int*)
_tmp55)!= 0)goto _LL36;_tmp56=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp55)->f1;
if((int)_tmp56 != 0)goto _LL36;_LL35: t=Cyc_Absyn_schar_typ;goto _LL33;_LL36: if(
_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 0)goto _LL38;_tmp57=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp55)->f1;if((int)_tmp57 != 1)goto _LL38;_LL37: t=Cyc_Absyn_uchar_typ;goto _LL33;
_LL38: if(_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 0)goto _LL3A;_tmp58=(void*)((
struct Cyc_Absyn_Char_c_struct*)_tmp55)->f1;if((int)_tmp58 != 2)goto _LL3A;_LL39: t=
Cyc_Absyn_char_typ;goto _LL33;_LL3A: if(_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 1)
goto _LL3C;_tmp59=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp55)->f1;_LL3B: t=
_tmp59 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL33;_LL3C: if(
_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 3)goto _LL3E;_tmp5A=(void*)((struct Cyc_Absyn_LongLong_c_struct*)
_tmp55)->f1;_LL3D: t=_tmp5A == (void*)1?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;
goto _LL33;_LL3E: if(_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 4)goto _LL40;_LL3F: t=Cyc_Absyn_float_typ;
goto _LL33;_LL40: if(_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 2)goto _LL42;_tmp5B=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp55)->f1;_tmp5C=((struct Cyc_Absyn_Int_c_struct*)
_tmp55)->f2;_LL41: if(topt == 0)t=_tmp5B == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
else{void*_tmp5E=Cyc_Tcutil_compress(*topt);void*_tmp5F;void*_tmp60;void*_tmp61;
void*_tmp62;void*_tmp63;void*_tmp64;_LL47: if(_tmp5E <= (void*)3?1:*((int*)_tmp5E)
!= 5)goto _LL49;_tmp5F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E)->f1;
_tmp60=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E)->f2;if((int)_tmp60 != 0)
goto _LL49;_LL48:{void*_tmp65=_tmp5F;_LL54: if((int)_tmp65 != 1)goto _LL56;_LL55: t=
Cyc_Absyn_uchar_typ;goto _LL53;_LL56: if((int)_tmp65 != 0)goto _LL58;_LL57: t=Cyc_Absyn_schar_typ;
goto _LL53;_LL58: if((int)_tmp65 != 2)goto _LL53;_LL59: t=Cyc_Absyn_char_typ;goto
_LL53;_LL53:;}(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66[0]=({struct Cyc_Absyn_Const_e_struct
_tmp67;_tmp67.tag=0;_tmp67.f1=(void*)((void*)({struct Cyc_Absyn_Char_c_struct*
_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68[0]=({struct Cyc_Absyn_Char_c_struct
_tmp69;_tmp69.tag=0;_tmp69.f1=(void*)_tmp5F;_tmp69.f2=(char)_tmp5C;_tmp69;});
_tmp68;}));_tmp67;});_tmp66;})));goto _LL46;_LL49: if(_tmp5E <= (void*)3?1:*((int*)
_tmp5E)!= 5)goto _LL4B;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E)->f1;
_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E)->f2;if((int)_tmp62 != 1)
goto _LL4B;_LL4A: t=_tmp61 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp6A=_cycalloc(
sizeof(*_tmp6A));_tmp6A[0]=({struct Cyc_Absyn_Const_e_struct _tmp6B;_tmp6B.tag=0;
_tmp6B.f1=(void*)((void*)({struct Cyc_Absyn_Short_c_struct*_tmp6C=_cycalloc(
sizeof(*_tmp6C));_tmp6C[0]=({struct Cyc_Absyn_Short_c_struct _tmp6D;_tmp6D.tag=1;
_tmp6D.f1=(void*)_tmp61;_tmp6D.f2=(short)_tmp5C;_tmp6D;});_tmp6C;}));_tmp6B;});
_tmp6A;})));goto _LL46;_LL4B: if(_tmp5E <= (void*)3?1:*((int*)_tmp5E)!= 5)goto _LL4D;
_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E)->f1;_LL4C: t=_tmp63 == (
void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL46;_LL4D: if(_tmp5E <= (void*)
3?1:*((int*)_tmp5E)!= 4)goto _LL4F;if(!(_tmp5C == 0))goto _LL4F;_LL4E:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));
_tmp6E[0]=({struct Cyc_Absyn_Const_e_struct _tmp6F;_tmp6F.tag=0;_tmp6F.f1=(void*)((
void*)0);_tmp6F;});_tmp6E;})));{struct Cyc_List_List*_tmp70=Cyc_Tcenv_lookup_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp71=_cycalloc(sizeof(*
_tmp71));_tmp71[0]=({struct Cyc_Absyn_PointerType_struct _tmp72;_tmp72.tag=4;
_tmp72.f1=({struct Cyc_Absyn_PtrInfo _tmp73;_tmp73.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp76=_cycalloc(sizeof(*
_tmp76));_tmp76->v=_tmp70;_tmp76;}));_tmp73.elt_tq=Cyc_Absyn_empty_tqual();
_tmp73.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp74;_tmp74.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp75=_cycalloc(sizeof(*
_tmp75));_tmp75->v=_tmp70;_tmp75;}));_tmp74.nullable=Cyc_Absyn_true_conref;
_tmp74.bounds=Cyc_Absyn_empty_conref();_tmp74.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp74;});_tmp73;});_tmp72;});_tmp71;});goto _LL46;}
_LL4F: if(_tmp5E <= (void*)3?1:*((int*)_tmp5E)!= 17)goto _LL51;_tmp64=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp5E)->f1;_LL50: {struct Cyc_Absyn_TypeInt_struct*
_tmp77=({struct Cyc_Absyn_TypeInt_struct*_tmp7E=_cycalloc_atomic(sizeof(*_tmp7E));
_tmp7E[0]=({struct Cyc_Absyn_TypeInt_struct _tmp7F;_tmp7F.tag=18;_tmp7F.f1=_tmp5C;
_tmp7F;});_tmp7E;});if(!Cyc_Tcutil_unify(_tmp64,(void*)_tmp77)){({struct Cyc_String_pa_struct
_tmp7B;_tmp7B.tag=0;_tmp7B.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)_tmp77));{struct Cyc_String_pa_struct _tmp7A;_tmp7A.tag=0;_tmp7A.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp64));{void*_tmp78[2]={&
_tmp7A,& _tmp7B};Cyc_Tcutil_terr(loc,({const char*_tmp79="expecting %s but found %s";
_tag_arr(_tmp79,sizeof(char),_get_zero_arr_size(_tmp79,26));}),_tag_arr(_tmp78,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}t=(void*)({struct Cyc_Absyn_TagType_struct*
_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Absyn_TagType_struct
_tmp7D;_tmp7D.tag=17;_tmp7D.f1=(void*)((void*)_tmp77);_tmp7D;});_tmp7C;});goto
_LL46;}_LL51:;_LL52: t=_tmp5B == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL46:;}goto _LL33;_LL42: if(_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 5)
goto _LL44;_tmp5D=((struct Cyc_Absyn_String_c_struct*)_tmp55)->f1;_LL43: {int len=(
int)_get_arr_size(_tmp5D,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Int_c_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=({
struct Cyc_Absyn_Int_c_struct _tmp8C;_tmp8C.tag=2;_tmp8C.f1=(void*)((void*)1);
_tmp8C.f2=len;_tmp8C;});_tmp8B;}),loc);elen->topt=({struct Cyc_Core_Opt*_tmp80=
_cycalloc(sizeof(*_tmp80));_tmp80->v=(void*)Cyc_Absyn_uint_typ;_tmp80;});t=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp82;_tmp82.tag=0;_tmp82.f1=elen;_tmp82;});_tmp81;}),Cyc_Absyn_true_conref);
if(topt != 0){void*_tmp83=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo
_tmp84;struct Cyc_Absyn_Tqual _tmp85;_LL5B: if(_tmp83 <= (void*)3?1:*((int*)_tmp83)
!= 7)goto _LL5D;_tmp84=((struct Cyc_Absyn_ArrayType_struct*)_tmp83)->f1;_tmp85=
_tmp84.tq;_LL5C: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmp85,(struct Cyc_Absyn_Exp*)
elen,((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)());_LL5D: if(_tmp83 <= (
void*)3?1:*((int*)_tmp83)!= 4)goto _LL5F;_LL5E: if(!Cyc_Tcutil_unify(*topt,t)?Cyc_Tcutil_silent_castable(
te,loc,t,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp86=_cycalloc(sizeof(*_tmp86));
_tmp86->v=(void*)t;_tmp86;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);t=*
topt;}else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp87;})),Cyc_Absyn_const_tqual(),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp89;_tmp89.tag=0;_tmp89.f1=elen;_tmp89;});
_tmp88;}),Cyc_Absyn_true_conref);if(!Cyc_Tcutil_unify(*topt,t)?Cyc_Tcutil_silent_castable(
te,loc,t,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp8A=_cycalloc(sizeof(*_tmp8A));
_tmp8A->v=(void*)t;_tmp8A;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);t=*
topt;}}goto _LL5A;_LL5F:;_LL60: goto _LL5A;_LL5A:;}return t;}_LL44: if((int)_tmp55 != 
0)goto _LL33;_LL45: {struct Cyc_List_List*_tmp8D=Cyc_Tcenv_lookup_type_vars(te);t=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));
_tmp8E[0]=({struct Cyc_Absyn_PointerType_struct _tmp8F;_tmp8F.tag=4;_tmp8F.f1=({
struct Cyc_Absyn_PtrInfo _tmp90;_tmp90.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp93=_cycalloc(sizeof(*
_tmp93));_tmp93->v=_tmp8D;_tmp93;}));_tmp90.elt_tq=Cyc_Absyn_empty_tqual();
_tmp90.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp91;_tmp91.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp92=_cycalloc(sizeof(*
_tmp92));_tmp92->v=_tmp8D;_tmp92;}));_tmp91.nullable=Cyc_Absyn_true_conref;
_tmp91.bounds=Cyc_Absyn_empty_conref();_tmp91.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp91;});_tmp90;});_tmp8F;});_tmp8E;});goto _LL33;}
_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q,void*b){void*_tmp94=b;struct Cyc_Absyn_Vardecl*_tmp95;struct
Cyc_Absyn_Fndecl*_tmp96;struct Cyc_Absyn_Vardecl*_tmp97;struct Cyc_Absyn_Vardecl*
_tmp98;struct Cyc_Absyn_Vardecl*_tmp99;_LL62: if((int)_tmp94 != 0)goto _LL64;_LL63:
return({struct Cyc_String_pa_struct _tmp9C;_tmp9C.tag=0;_tmp9C.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(q));{void*_tmp9A[1]={&
_tmp9C};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp9B="undeclared identifier: %s";
_tag_arr(_tmp9B,sizeof(char),_get_zero_arr_size(_tmp9B,26));}),_tag_arr(_tmp9A,
sizeof(void*),1));}});_LL64: if(_tmp94 <= (void*)1?1:*((int*)_tmp94)!= 0)goto _LL66;
_tmp95=((struct Cyc_Absyn_Global_b_struct*)_tmp94)->f1;_LL65:*q=*_tmp95->name;
return(void*)_tmp95->type;_LL66: if(_tmp94 <= (void*)1?1:*((int*)_tmp94)!= 1)goto
_LL68;_tmp96=((struct Cyc_Absyn_Funname_b_struct*)_tmp94)->f1;_LL67:*q=*_tmp96->name;
return Cyc_Tcutil_fndecl2typ(_tmp96);_LL68: if(_tmp94 <= (void*)1?1:*((int*)_tmp94)
!= 4)goto _LL6A;_tmp97=((struct Cyc_Absyn_Pat_b_struct*)_tmp94)->f1;_LL69: _tmp98=
_tmp97;goto _LL6B;_LL6A: if(_tmp94 <= (void*)1?1:*((int*)_tmp94)!= 3)goto _LL6C;
_tmp98=((struct Cyc_Absyn_Local_b_struct*)_tmp94)->f1;_LL6B: _tmp99=_tmp98;goto
_LL6D;_LL6C: if(_tmp94 <= (void*)1?1:*((int*)_tmp94)!= 2)goto _LL61;_tmp99=((struct
Cyc_Absyn_Param_b_struct*)_tmp94)->f1;_LL6D:(*q).f1=(void*)0;return(void*)_tmp99->type;
_LL61:;}static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
fmt,struct Cyc_Core_Opt*opt_args,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,
struct _tagged_arr,struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{
void*_tmp9D=(void*)fmt->r;void*_tmp9E;struct _tagged_arr _tmp9F;struct Cyc_Absyn_Exp*
_tmpA0;struct Cyc_Absyn_Exp _tmpA1;void*_tmpA2;void*_tmpA3;struct _tagged_arr _tmpA4;
_LL6F: if(*((int*)_tmp9D)!= 0)goto _LL71;_tmp9E=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp9D)->f1;if(_tmp9E <= (void*)1?1:*((int*)_tmp9E)!= 5)goto _LL71;_tmp9F=((struct
Cyc_Absyn_String_c_struct*)_tmp9E)->f1;_LL70: _tmpA4=_tmp9F;goto _LL72;_LL71: if(*((
int*)_tmp9D)!= 13)goto _LL73;_tmpA0=((struct Cyc_Absyn_Cast_e_struct*)_tmp9D)->f2;
_tmpA1=*_tmpA0;_tmpA2=(void*)_tmpA1.r;if(*((int*)_tmpA2)!= 0)goto _LL73;_tmpA3=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmpA2)->f1;if(_tmpA3 <= (void*)1?1:*((
int*)_tmpA3)!= 5)goto _LL73;_tmpA4=((struct Cyc_Absyn_String_c_struct*)_tmpA3)->f1;
_LL72: desc_types=type_getter(te,(struct _tagged_arr)_tmpA4,fmt->loc);goto _LL6E;
_LL73:;_LL74: if(opt_args != 0){struct Cyc_List_List*_tmpA5=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpA5 != 0;_tmpA5=_tmpA5->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpA5->hd);}}return;_LL6E:;}if(opt_args != 0){struct Cyc_List_List*_tmpA6=(struct
Cyc_List_List*)opt_args->v;for(0;desc_types != 0?_tmpA6 != 0: 0;(desc_types=
desc_types->tl,_tmpA6=_tmpA6->tl)){void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_tmpA6->hd;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t)){({struct Cyc_String_pa_struct _tmpAA;_tmpAA.tag=0;_tmpAA.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{struct Cyc_String_pa_struct _tmpA9;_tmpA9.tag=0;_tmpA9.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmpA7[2]={&
_tmpA9,& _tmpAA};Cyc_Tcutil_terr(e->loc,({const char*_tmpA8="descriptor has type \n%s\n but argument has type \n%s";
_tag_arr(_tmpA8,sizeof(char),_get_zero_arr_size(_tmpA8,51));}),_tag_arr(_tmpA7,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}if(desc_types != 0)({void*
_tmpAB[0]={};Cyc_Tcutil_terr(fmt->loc,({const char*_tmpAC="too few arguments";
_tag_arr(_tmpAC,sizeof(char),_get_zero_arr_size(_tmpAC,18));}),_tag_arr(_tmpAB,
sizeof(void*),0));});if(_tmpA6 != 0)({void*_tmpAD[0]={};Cyc_Tcutil_terr(((struct
Cyc_Absyn_Exp*)_tmpA6->hd)->loc,({const char*_tmpAE="too many arguments";_tag_arr(
_tmpAE,sizeof(char),_get_zero_arr_size(_tmpAE,19));}),_tag_arr(_tmpAD,sizeof(
void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmpAF=p;_LL76: if((
int)_tmpAF != 0)goto _LL78;_LL77: goto _LL79;_LL78: if((int)_tmpAF != 2)goto _LL7A;
_LL79: if(!Cyc_Tcutil_is_numeric(e))({struct Cyc_String_pa_struct _tmpB2;_tmpB2.tag=
0;_tmpB2.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
void*_tmpB0[1]={& _tmpB2};Cyc_Tcutil_terr(loc,({const char*_tmpB1="expecting arithmetic type but found %s";
_tag_arr(_tmpB1,sizeof(char),_get_zero_arr_size(_tmpB1,39));}),_tag_arr(_tmpB0,
sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
_LL7A: if((int)_tmpAF != 11)goto _LL7C;_LL7B: Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct _tmpB5;_tmpB5.tag=
0;_tmpB5.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
void*_tmpB3[1]={& _tmpB5};Cyc_Tcutil_terr(loc,({const char*_tmpB4="expecting integral or * type but found %s";
_tag_arr(_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,42));}),_tag_arr(_tmpB3,
sizeof(void*),1));}});return Cyc_Absyn_sint_typ;_LL7C: if((int)_tmpAF != 12)goto
_LL7E;_LL7D: if(!Cyc_Tcutil_is_integral(e))({struct Cyc_String_pa_struct _tmpB8;
_tmpB8.tag=0;_tmpB8.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmpB6[1]={& _tmpB8};Cyc_Tcutil_terr(loc,({const char*_tmpB7="expecting integral type but found %s";
_tag_arr(_tmpB7,sizeof(char),_get_zero_arr_size(_tmpB7,37));}),_tag_arr(_tmpB6,
sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
_LL7E: if((int)_tmpAF != 19)goto _LL80;_LL7F:{void*_tmpB9=t;struct Cyc_Absyn_PtrInfo
_tmpBA;struct Cyc_Absyn_PtrAtts _tmpBB;struct Cyc_Absyn_Conref*_tmpBC;_LL83: if(
_tmpB9 <= (void*)3?1:*((int*)_tmpB9)!= 7)goto _LL85;_LL84: goto _LL82;_LL85: if(
_tmpB9 <= (void*)3?1:*((int*)_tmpB9)!= 4)goto _LL87;_tmpBA=((struct Cyc_Absyn_PointerType_struct*)
_tmpB9)->f1;_tmpBB=_tmpBA.ptr_atts;_tmpBC=_tmpBB.bounds;_LL86:{void*_tmpBD=(void*)(
Cyc_Absyn_compress_conref(_tmpBC))->v;void*_tmpBE;void*_tmpBF;_LL8A: if(_tmpBD <= (
void*)1?1:*((int*)_tmpBD)!= 0)goto _LL8C;_tmpBE=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpBD)->f1;if((int)_tmpBE != 0)goto _LL8C;_LL8B: goto _LL8D;_LL8C: if(_tmpBD <= (void*)
1?1:*((int*)_tmpBD)!= 0)goto _LL8E;_tmpBF=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpBD)->f1;if(_tmpBF <= (void*)1?1:*((int*)_tmpBF)!= 0)goto _LL8E;_LL8D: goto _LL89;
_LL8E:;_LL8F:({void*_tmpC0[0]={};Cyc_Tcutil_terr(loc,({const char*_tmpC1="can't apply size to pointer/array of abstract length";
_tag_arr(_tmpC1,sizeof(char),_get_zero_arr_size(_tmpC1,53));}),_tag_arr(_tmpC0,
sizeof(void*),0));});_LL89:;}goto _LL82;_LL87:;_LL88:({struct Cyc_String_pa_struct
_tmpC4;_tmpC4.tag=0;_tmpC4.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmpC2[1]={& _tmpC4};Cyc_Tcutil_terr(loc,({const char*_tmpC3="size requires pointer or array type, not %s";
_tag_arr(_tmpC3,sizeof(char),_get_zero_arr_size(_tmpC3,44));}),_tag_arr(_tmpC2,
sizeof(void*),1));}});_LL82:;}return Cyc_Absyn_uint_typ;_LL80:;_LL81:({void*
_tmpC5[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmpC6="Non-unary primop";_tag_arr(_tmpC6,sizeof(char),
_get_zero_arr_size(_tmpC6,17));}),_tag_arr(_tmpC5,sizeof(void*),0));});_LL75:;}
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({
struct Cyc_String_pa_struct _tmpC9;_tmpC9.tag=0;_tmpC9.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v));{void*_tmpC7[1]={& _tmpC9};Cyc_Tcutil_terr(e1->loc,({
const char*_tmpC8="type %s cannot be used here";_tag_arr(_tmpC8,sizeof(char),
_get_zero_arr_size(_tmpC8,28));}),_tag_arr(_tmpC7,sizeof(void*),1));}});return
Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->v=
Cyc_Tcenv_lookup_type_vars(te);_tmpCA;}));}if(!checker(e2)){({struct Cyc_String_pa_struct
_tmpCD;_tmpCD.tag=0;_tmpCD.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmpCB[1]={& _tmpCD};
Cyc_Tcutil_terr(e2->loc,({const char*_tmpCC="type %s cannot be used here";
_tag_arr(_tmpCC,sizeof(char),_get_zero_arr_size(_tmpCC,28));}),_tag_arr(_tmpCB,
sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpCE=
_cycalloc(sizeof(*_tmpCE));_tmpCE->v=Cyc_Tcenv_lookup_type_vars(te);_tmpCE;}));}{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);void*_tmpCF=t1;struct Cyc_Absyn_PtrInfo
_tmpD0;void*_tmpD1;struct Cyc_Absyn_Tqual _tmpD2;struct Cyc_Absyn_PtrAtts _tmpD3;
void*_tmpD4;struct Cyc_Absyn_Conref*_tmpD5;struct Cyc_Absyn_Conref*_tmpD6;struct
Cyc_Absyn_Conref*_tmpD7;_LL91: if(_tmpCF <= (void*)3?1:*((int*)_tmpCF)!= 4)goto
_LL93;_tmpD0=((struct Cyc_Absyn_PointerType_struct*)_tmpCF)->f1;_tmpD1=(void*)
_tmpD0.elt_typ;_tmpD2=_tmpD0.elt_tq;_tmpD3=_tmpD0.ptr_atts;_tmpD4=(void*)_tmpD3.rgn;
_tmpD5=_tmpD3.nullable;_tmpD6=_tmpD3.bounds;_tmpD7=_tmpD3.zero_term;_LL92: if(!
Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpD1),(void*)1))({void*_tmpD8[0]={};
Cyc_Tcutil_terr(e1->loc,({const char*_tmpD9="can't perform arithmetic on abstract pointer type";
_tag_arr(_tmpD9,sizeof(char),_get_zero_arr_size(_tmpD9,50));}),_tag_arr(_tmpD8,
sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({struct Cyc_String_pa_struct
_tmpDC;_tmpDC.tag=0;_tmpDC.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{void*_tmpDA[1]={& _tmpDC};Cyc_Tcutil_terr(e2->loc,({const char*_tmpDB="expecting int but found %s";
_tag_arr(_tmpDB,sizeof(char),_get_zero_arr_size(_tmpDB,27));}),_tag_arr(_tmpDA,
sizeof(void*),1));}});_tmpD6=Cyc_Absyn_compress_conref(_tmpD6);{void*_tmpDD=(
void*)_tmpD6->v;void*_tmpDE;void*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;void*_tmpE1;
_LL96: if(_tmpDD <= (void*)1?1:*((int*)_tmpDD)!= 0)goto _LL98;_tmpDE=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpDD)->f1;if((int)_tmpDE != 0)goto _LL98;_LL97:
return t1;_LL98: if(_tmpDD <= (void*)1?1:*((int*)_tmpDD)!= 0)goto _LL9A;_tmpDF=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpDD)->f1;if(_tmpDF <= (void*)1?1:*((int*)
_tmpDF)!= 0)goto _LL9A;_tmpE0=((struct Cyc_Absyn_Upper_b_struct*)_tmpDF)->f1;_LL99:
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpD7))({void*
_tmpE2[0]={};Cyc_Tcutil_warn(e1->loc,({const char*_tmpE3="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_arr(_tmpE3,sizeof(char),_get_zero_arr_size(_tmpE3,70));}),_tag_arr(_tmpE2,
sizeof(void*),0));});{struct Cyc_Absyn_PointerType_struct*_tmpE4=({struct Cyc_Absyn_PointerType_struct*
_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5[0]=({struct Cyc_Absyn_PointerType_struct
_tmpE6;_tmpE6.tag=4;_tmpE6.f1=({struct Cyc_Absyn_PtrInfo _tmpE7;_tmpE7.elt_typ=(
void*)_tmpD1;_tmpE7.elt_tq=_tmpD2;_tmpE7.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpE8;_tmpE8.rgn=(void*)_tmpD4;_tmpE8.nullable=Cyc_Absyn_true_conref;_tmpE8.bounds=
Cyc_Absyn_bounds_unknown_conref;_tmpE8.zero_term=_tmpD7;_tmpE8;});_tmpE7;});
_tmpE6;});_tmpE5;});Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpE4,(void*)3);
return(void*)_tmpE4;}_LL9A: if(_tmpDD <= (void*)1?1:*((int*)_tmpDD)!= 0)goto _LL9C;
_tmpE1=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpDD)->f1;if(_tmpE1 <= (void*)
1?1:*((int*)_tmpE1)!= 1)goto _LL9C;_LL9B:({void*_tmpE9[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmpEA="pointer arithmetic not allowed on pointers with abstract bounds";
_tag_arr(_tmpEA,sizeof(char),_get_zero_arr_size(_tmpEA,64));}),_tag_arr(_tmpE9,
sizeof(void*),0));});return t1;_LL9C:;_LL9D:(void*)(_tmpD6->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmpEC;_tmpEC.tag=0;_tmpEC.f1=(void*)((void*)0);
_tmpEC;});_tmpEB;})));return t1;_LL95:;}_LL93:;_LL94: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LL90:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_String_pa_struct _tmpF0;_tmpF0.tag=
0;_tmpF0.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{struct Cyc_String_pa_struct
_tmpEF;_tmpEF.tag=0;_tmpEF.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpED[2]={& _tmpEF,&
_tmpF0};Cyc_Tcutil_terr(e1->loc,({const char*_tmpEE="pointer arithmetic on values of different types (%s != %s)";
_tag_arr(_tmpEE,sizeof(char),_get_zero_arr_size(_tmpEE,59));}),_tag_arr(_tmpED,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}
else{if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),(void*)1))({void*_tmpF1[
0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpF2="can't perform arithmetic on abstract pointer type";
_tag_arr(_tmpF2,sizeof(char),_get_zero_arr_size(_tmpF2,50));}),_tag_arr(_tmpF1,
sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){({struct Cyc_String_pa_struct
_tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmpF3[2]={& _tmpF5,& _tmpF6};
Cyc_Tcutil_terr(e2->loc,({const char*_tmpF4="expecting either %s or int but found %s";
_tag_arr(_tmpF4,sizeof(char),_get_zero_arr_size(_tmpF4,40));}),_tag_arr(_tmpF3,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(
t1))Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,(void*)3);if(Cyc_Tcutil_is_pointer_type(
t2))Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,(void*)3);return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);}static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(e1_is_num?e2_is_num: 0)return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_unify(t1,t2)?
Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({
struct Cyc_Core_Opt*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpF7;}))): 0)return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,
loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,e2,t1,(void*)3);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,
e1,t2,(void*)3);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(te,t2,
e1)?1: Cyc_Tcutil_zero_to_null(te,t1,e2))return Cyc_Absyn_sint_typ;else{{struct
_tuple0 _tmpF9=({struct _tuple0 _tmpF8;_tmpF8.f1=Cyc_Tcutil_compress(t1);_tmpF8.f2=
Cyc_Tcutil_compress(t2);_tmpF8;});void*_tmpFA;struct Cyc_Absyn_PtrInfo _tmpFB;void*
_tmpFC;void*_tmpFD;struct Cyc_Absyn_PtrInfo _tmpFE;void*_tmpFF;_LL9F: _tmpFA=_tmpF9.f1;
if(_tmpFA <= (void*)3?1:*((int*)_tmpFA)!= 4)goto _LLA1;_tmpFB=((struct Cyc_Absyn_PointerType_struct*)
_tmpFA)->f1;_tmpFC=(void*)_tmpFB.elt_typ;_tmpFD=_tmpF9.f2;if(_tmpFD <= (void*)3?1:*((
int*)_tmpFD)!= 4)goto _LLA1;_tmpFE=((struct Cyc_Absyn_PointerType_struct*)_tmpFD)->f1;
_tmpFF=(void*)_tmpFE.elt_typ;_LLA0: if(Cyc_Tcutil_unify(_tmpFC,_tmpFF))return Cyc_Absyn_sint_typ;
goto _LL9E;_LLA1:;_LLA2: goto _LL9E;_LL9E:;}({struct Cyc_String_pa_struct _tmp103;
_tmp103.tag=0;_tmp103.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp102;_tmp102.tag=0;_tmp102.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp100[2]={&
_tmp102,& _tmp103};Cyc_Tcutil_terr(loc,({const char*_tmp101="comparison not allowed between %s and %s";
_tag_arr(_tmp101,sizeof(char),_get_zero_arr_size(_tmp101,41));}),_tag_arr(
_tmp100,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp104;}));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){void*_tmp105=p;_LLA4: if((int)_tmp105 != 0)goto _LLA6;_LLA5:
return Cyc_Tcexp_tcPlus(te,e1,e2);_LLA6: if((int)_tmp105 != 2)goto _LLA8;_LLA7:
return Cyc_Tcexp_tcMinus(te,e1,e2);_LLA8: if((int)_tmp105 != 1)goto _LLAA;_LLA9: goto
_LLAB;_LLAA: if((int)_tmp105 != 3)goto _LLAC;_LLAB: return Cyc_Tcexp_tcArithBinop(te,
e1,e2,Cyc_Tcutil_is_numeric);_LLAC: if((int)_tmp105 != 4)goto _LLAE;_LLAD: goto _LLAF;
_LLAE: if((int)_tmp105 != 13)goto _LLB0;_LLAF: goto _LLB1;_LLB0: if((int)_tmp105 != 14)
goto _LLB2;_LLB1: goto _LLB3;_LLB2: if((int)_tmp105 != 15)goto _LLB4;_LLB3: goto _LLB5;
_LLB4: if((int)_tmp105 != 16)goto _LLB6;_LLB5: goto _LLB7;_LLB6: if((int)_tmp105 != 17)
goto _LLB8;_LLB7: goto _LLB9;_LLB8: if((int)_tmp105 != 18)goto _LLBA;_LLB9: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_integral);_LLBA: if((int)_tmp105 != 5)goto _LLBC;_LLBB: goto
_LLBD;_LLBC: if((int)_tmp105 != 6)goto _LLBE;_LLBD: goto _LLBF;_LLBE: if((int)_tmp105
!= 7)goto _LLC0;_LLBF: goto _LLC1;_LLC0: if((int)_tmp105 != 8)goto _LLC2;_LLC1: goto
_LLC3;_LLC2: if((int)_tmp105 != 9)goto _LLC4;_LLC3: goto _LLC5;_LLC4: if((int)_tmp105
!= 10)goto _LLC6;_LLC5: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_LLC6:;_LLC7:({
void*_tmp106[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp107="bad binary primop";_tag_arr(_tmp107,sizeof(char),
_get_zero_arr_size(_tmp107,18));}),_tag_arr(_tmp106,sizeof(void*),0));});_LLA3:;}
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_List_List*es){if(p == (void*)2?((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es)== 1: 0)return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(te,es);{void*t;
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLC8: return({
void*_tmp108[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp109="primitive operator has 0 arguments";
_tag_arr(_tmp109,sizeof(char),_get_zero_arr_size(_tmp109,35));}),_tag_arr(
_tmp108,sizeof(void*),0));});case 1: _LLC9: t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2:
_LLCA: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((
struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCB: return({
void*_tmp10A[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp10B="primitive operator has > 2 arguments";
_tag_arr(_tmp10B,sizeof(char),_get_zero_arr_size(_tmp10B,37));}),_tag_arr(
_tmp10A,sizeof(void*),0));});}return t;}}struct _tuple9{struct Cyc_Absyn_Tqual f1;
void*f2;};static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){{void*_tmp10C=(void*)e->r;void*_tmp10D;struct Cyc_Absyn_Vardecl*_tmp10E;void*
_tmp10F;struct Cyc_Absyn_Vardecl*_tmp110;void*_tmp111;struct Cyc_Absyn_Vardecl*
_tmp112;void*_tmp113;struct Cyc_Absyn_Vardecl*_tmp114;struct Cyc_Absyn_Exp*_tmp115;
struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp117;struct _tagged_arr*
_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct _tagged_arr*_tmp11A;struct Cyc_Absyn_Exp*
_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;_LLCE: if(*((int*)
_tmp10C)!= 1)goto _LLD0;_tmp10D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;
if(_tmp10D <= (void*)1?1:*((int*)_tmp10D)!= 2)goto _LLD0;_tmp10E=((struct Cyc_Absyn_Param_b_struct*)
_tmp10D)->f1;_LLCF: _tmp110=_tmp10E;goto _LLD1;_LLD0: if(*((int*)_tmp10C)!= 1)goto
_LLD2;_tmp10F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;if(_tmp10F <= (
void*)1?1:*((int*)_tmp10F)!= 3)goto _LLD2;_tmp110=((struct Cyc_Absyn_Local_b_struct*)
_tmp10F)->f1;_LLD1: _tmp112=_tmp110;goto _LLD3;_LLD2: if(*((int*)_tmp10C)!= 1)goto
_LLD4;_tmp111=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;if(_tmp111 <= (
void*)1?1:*((int*)_tmp111)!= 4)goto _LLD4;_tmp112=((struct Cyc_Absyn_Pat_b_struct*)
_tmp111)->f1;_LLD3: _tmp114=_tmp112;goto _LLD5;_LLD4: if(*((int*)_tmp10C)!= 1)goto
_LLD6;_tmp113=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;if(_tmp113 <= (
void*)1?1:*((int*)_tmp113)!= 0)goto _LLD6;_tmp114=((struct Cyc_Absyn_Global_b_struct*)
_tmp113)->f1;_LLD5: if(!(_tmp114->tq).real_const)return;goto _LLCD;_LLD6: if(*((int*)
_tmp10C)!= 23)goto _LLD8;_tmp115=((struct Cyc_Absyn_Subscript_e_struct*)_tmp10C)->f1;
_tmp116=((struct Cyc_Absyn_Subscript_e_struct*)_tmp10C)->f2;_LLD7:{void*_tmp11E=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp115->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp11F;struct Cyc_Absyn_Tqual _tmp120;struct Cyc_Absyn_ArrayInfo
_tmp121;struct Cyc_Absyn_Tqual _tmp122;struct Cyc_List_List*_tmp123;_LLE5: if(
_tmp11E <= (void*)3?1:*((int*)_tmp11E)!= 4)goto _LLE7;_tmp11F=((struct Cyc_Absyn_PointerType_struct*)
_tmp11E)->f1;_tmp120=_tmp11F.elt_tq;_LLE6: _tmp122=_tmp120;goto _LLE8;_LLE7: if(
_tmp11E <= (void*)3?1:*((int*)_tmp11E)!= 7)goto _LLE9;_tmp121=((struct Cyc_Absyn_ArrayType_struct*)
_tmp11E)->f1;_tmp122=_tmp121.tq;_LLE8: if(!_tmp122.real_const)return;goto _LLE4;
_LLE9: if(_tmp11E <= (void*)3?1:*((int*)_tmp11E)!= 9)goto _LLEB;_tmp123=((struct Cyc_Absyn_TupleType_struct*)
_tmp11E)->f1;_LLEA: {unsigned int _tmp125;int _tmp126;struct _tuple7 _tmp124=Cyc_Evexp_eval_const_uint_exp(
_tmp116);_tmp125=_tmp124.f1;_tmp126=_tmp124.f2;if(!_tmp126){({void*_tmp127[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp128="tuple projection cannot use sizeof or offsetof";
_tag_arr(_tmp128,sizeof(char),_get_zero_arr_size(_tmp128,47));}),_tag_arr(
_tmp127,sizeof(void*),0));});return;}{struct _handler_cons _tmp129;_push_handler(&
_tmp129);{int _tmp12B=0;if(setjmp(_tmp129.handler))_tmp12B=1;if(!_tmp12B){{struct
_tuple9 _tmp12D;struct Cyc_Absyn_Tqual _tmp12E;struct _tuple9*_tmp12C=((struct
_tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp123,(int)_tmp125);
_tmp12D=*_tmp12C;_tmp12E=_tmp12D.f1;if(!_tmp12E.real_const){_npop_handler(0);
return;}};_pop_handler();}else{void*_tmp12A=(void*)_exn_thrown;void*_tmp130=
_tmp12A;_LLEE: if(_tmp130 != Cyc_List_Nth)goto _LLF0;_LLEF: return;_LLF0:;_LLF1:(
void)_throw(_tmp130);_LLED:;}}}goto _LLE4;}_LLEB:;_LLEC: goto _LLE4;_LLE4:;}goto
_LLCD;_LLD8: if(*((int*)_tmp10C)!= 21)goto _LLDA;_tmp117=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp10C)->f1;_tmp118=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp10C)->f2;_LLD9:{
void*_tmp131=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp117->topt))->v);struct Cyc_Absyn_AggrInfo _tmp132;void*_tmp133;struct Cyc_Absyn_Aggrdecl**
_tmp134;struct Cyc_List_List*_tmp135;_LLF3: if(_tmp131 <= (void*)3?1:*((int*)
_tmp131)!= 10)goto _LLF5;_tmp132=((struct Cyc_Absyn_AggrType_struct*)_tmp131)->f1;
_tmp133=(void*)_tmp132.aggr_info;if(*((int*)_tmp133)!= 1)goto _LLF5;_tmp134=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp133)->f1;_LLF4: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp134 == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp134,_tmp118);if(sf == 0?1: !(sf->tq).real_const)return;goto _LLF2;}_LLF5: if(
_tmp131 <= (void*)3?1:*((int*)_tmp131)!= 11)goto _LLF7;_tmp135=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp131)->f2;_LLF6: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp135,
_tmp118);if(sf == 0?1: !(sf->tq).real_const)return;goto _LLF2;}_LLF7:;_LLF8: goto
_LLF2;_LLF2:;}goto _LLCD;_LLDA: if(*((int*)_tmp10C)!= 22)goto _LLDC;_tmp119=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp10C)->f1;_tmp11A=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp10C)->f2;_LLDB:{void*_tmp136=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp119->topt))->v);struct Cyc_Absyn_PtrInfo _tmp137;void*_tmp138;
_LLFA: if(_tmp136 <= (void*)3?1:*((int*)_tmp136)!= 4)goto _LLFC;_tmp137=((struct Cyc_Absyn_PointerType_struct*)
_tmp136)->f1;_tmp138=(void*)_tmp137.elt_typ;_LLFB:{void*_tmp139=Cyc_Tcutil_compress(
_tmp138);struct Cyc_Absyn_AggrInfo _tmp13A;void*_tmp13B;struct Cyc_Absyn_Aggrdecl**
_tmp13C;struct Cyc_List_List*_tmp13D;_LLFF: if(_tmp139 <= (void*)3?1:*((int*)
_tmp139)!= 10)goto _LL101;_tmp13A=((struct Cyc_Absyn_AggrType_struct*)_tmp139)->f1;
_tmp13B=(void*)_tmp13A.aggr_info;if(*((int*)_tmp13B)!= 1)goto _LL101;_tmp13C=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp13B)->f1;_LL100: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp13C == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp13C,_tmp11A);if(sf == 0?1: !(sf->tq).real_const)return;goto _LLFE;}_LL101: if(
_tmp139 <= (void*)3?1:*((int*)_tmp139)!= 11)goto _LL103;_tmp13D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp139)->f2;_LL102: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp13D,_tmp11A);if(sf == 0?1: !(sf->tq).real_const)return;goto _LLFE;}_LL103:;
_LL104: goto _LLFE;_LLFE:;}goto _LLF9;_LLFC:;_LLFD: goto _LLF9;_LLF9:;}goto _LLCD;
_LLDC: if(*((int*)_tmp10C)!= 20)goto _LLDE;_tmp11B=((struct Cyc_Absyn_Deref_e_struct*)
_tmp10C)->f1;_LLDD:{void*_tmp13E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp11B->topt))->v);struct Cyc_Absyn_PtrInfo _tmp13F;struct Cyc_Absyn_Tqual
_tmp140;struct Cyc_Absyn_ArrayInfo _tmp141;struct Cyc_Absyn_Tqual _tmp142;_LL106: if(
_tmp13E <= (void*)3?1:*((int*)_tmp13E)!= 4)goto _LL108;_tmp13F=((struct Cyc_Absyn_PointerType_struct*)
_tmp13E)->f1;_tmp140=_tmp13F.elt_tq;_LL107: _tmp142=_tmp140;goto _LL109;_LL108: if(
_tmp13E <= (void*)3?1:*((int*)_tmp13E)!= 7)goto _LL10A;_tmp141=((struct Cyc_Absyn_ArrayType_struct*)
_tmp13E)->f1;_tmp142=_tmp141.tq;_LL109: if(!_tmp142.real_const)return;goto _LL105;
_LL10A:;_LL10B: goto _LL105;_LL105:;}goto _LLCD;_LLDE: if(*((int*)_tmp10C)!= 11)goto
_LLE0;_tmp11C=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp10C)->f1;_LLDF:
_tmp11D=_tmp11C;goto _LLE1;_LLE0: if(*((int*)_tmp10C)!= 12)goto _LLE2;_tmp11D=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp10C)->f1;_LLE1: Cyc_Tcexp_check_writable(
te,_tmp11D);return;_LLE2:;_LLE3: goto _LLCD;_LLCD:;}({struct Cyc_String_pa_struct
_tmp145;_tmp145.tag=0;_tmp145.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(
e));{void*_tmp143[1]={& _tmp145};Cyc_Tcutil_terr(e->loc,({const char*_tmp144="attempt to write a const location: %s";
_tag_arr(_tmp144,sizeof(char),_get_zero_arr_size(_tmp144,38));}),_tag_arr(
_tmp143,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,void*i){Cyc_Tcexp_tcExpNoPromote(
te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({void*_tmp146[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp147="increment/decrement of non-lvalue";_tag_arr(
_tmp147,sizeof(char),_get_zero_arr_size(_tmp147,34));}),_tag_arr(_tmp146,sizeof(
void*),0));});Cyc_Tcexp_check_writable(te,e);{void*t=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt)?1:(Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt)?i == (void*)0?1: i == (void*)
1: 0)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(void*)1))({void*_tmp148[
0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp149="can't perform arithmetic on abstract pointer type";
_tag_arr(_tmp149,sizeof(char),_get_zero_arr_size(_tmp149,50));}),_tag_arr(
_tmp148,sizeof(void*),0));});}else{({struct Cyc_String_pa_struct _tmp14C;_tmp14C.tag=
0;_tmp14C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
void*_tmp14A[1]={& _tmp14C};Cyc_Tcutil_terr(e->loc,({const char*_tmp14B="expecting arithmetic or ? type but found %s";
_tag_arr(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,44));}),_tag_arr(
_tmp14A,sizeof(void*),1));}});}}return t;}}static void*Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){Cyc_Tcexp_tcTest(te,e1,({const
char*_tmp14D="conditional expression";_tag_arr(_tmp14D,sizeof(char),
_get_zero_arr_size(_tmp14D,23));}));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp156;}));struct Cyc_List_List _tmp14E=({struct Cyc_List_List _tmp155;_tmp155.hd=
e3;_tmp155.tl=0;_tmp155;});struct Cyc_List_List _tmp14F=({struct Cyc_List_List
_tmp154;_tmp154.hd=e2;_tmp154.tl=(struct Cyc_List_List*)& _tmp14E;_tmp154;});if(!
Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)& _tmp14F)){({struct Cyc_String_pa_struct
_tmp153;_tmp153.tag=0;_tmp153.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v));{struct Cyc_String_pa_struct
_tmp152;_tmp152.tag=0;_tmp152.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmp150[2]={&
_tmp152,& _tmp153};Cyc_Tcutil_terr(loc,({const char*_tmp151="conditional clause types do not match: %s != %s";
_tag_arr(_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,48));}),_tag_arr(
_tmp150,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}return t;}}static
void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(te),0,e1);Cyc_Tcexp_tcExp(
te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),
e2);{void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{void*_tmp157=Cyc_Tcutil_compress(
t1);_LL10D: if(_tmp157 <= (void*)3?1:*((int*)_tmp157)!= 7)goto _LL10F;_LL10E:({void*
_tmp158[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp159="cannot assign to an array";
_tag_arr(_tmp159,sizeof(char),_get_zero_arr_size(_tmp159,26));}),_tag_arr(
_tmp158,sizeof(void*),0));});goto _LL10C;_LL10F:;_LL110: goto _LL10C;_LL10C:;}if(!
Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),(void*)1))({void*_tmp15A[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp15B="type is abstract (can't determine size).";_tag_arr(
_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,41));}),_tag_arr(_tmp15A,sizeof(
void*),0));});if(!Cyc_Absyn_is_lvalue(e1))return({void*_tmp15C[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp15D="assignment to non-lvalue";_tag_arr(_tmp15D,
sizeof(char),_get_zero_arr_size(_tmp15D,25));}),_tag_arr(_tmp15C,sizeof(void*),0));});
Cyc_Tcexp_check_writable(te,e1);if(po == 0){if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
void*_tmp15E=({struct Cyc_String_pa_struct _tmp162;_tmp162.tag=0;_tmp162.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp161;_tmp161.tag=0;_tmp161.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t1));{void*_tmp15F[2]={& _tmp161,& _tmp162};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp160="type mismatch: %s != %s";_tag_arr(_tmp160,sizeof(char),
_get_zero_arr_size(_tmp160,24));}),_tag_arr(_tmp15F,sizeof(void*),2));}}});Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp15E;}}else{void*_tmp163=(void*)po->v;
void*_tmp164=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp163,e1,e2);if(!(Cyc_Tcutil_unify(
_tmp164,t1)?1:(Cyc_Tcutil_coerceable(_tmp164)?Cyc_Tcutil_coerceable(t1): 0))){
void*_tmp165=({struct Cyc_String_pa_struct _tmp169;_tmp169.tag=0;_tmp169.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp168;_tmp168.tag=0;_tmp168.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t1));{void*_tmp166[2]={& _tmp168,& _tmp169};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp167="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_arr(_tmp167,sizeof(char),_get_zero_arr_size(_tmp167,82));}),_tag_arr(
_tmp166,sizeof(void*),2));}}});Cyc_Tcutil_unify(_tmp164,t1);Cyc_Tcutil_explain_failure();
return _tmp165;}return _tmp164;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp16A=Cyc_Tcutil_compress(
t1);void*_tmp16B;void*_tmp16C;_LL112: if((int)_tmp16A != 1)goto _LL114;_LL113: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ,(void*)1);t1=dbl_typ;goto _LL111;_LL114: if(_tmp16A <= (void*)3?1:*((
int*)_tmp16A)!= 5)goto _LL116;_tmp16B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp16A)->f2;if((int)_tmp16B != 0)goto _LL116;_LL115: goto _LL117;_LL116: if(_tmp16A
<= (void*)3?1:*((int*)_tmp16A)!= 5)goto _LL118;_tmp16C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp16A)->f2;if((int)_tmp16C != 1)goto _LL118;_LL117: Cyc_Tcutil_unchecked_cast(te,
e,Cyc_Absyn_sint_typ,(void*)1);t1=Cyc_Absyn_sint_typ;goto _LL111;_LL118:;_LL119:
goto _LL111;_LL111:;}for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*
_tmp16E;struct Cyc_List_List*_tmp16F;struct Cyc_Position_Segment*_tmp170;void*
_tmp171;struct Cyc_Absyn_Tunionfield _tmp16D=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_tmp16E=_tmp16D.name;_tmp16F=_tmp16D.typs;_tmp170=_tmp16D.loc;
_tmp171=(void*)_tmp16D.sc;if(_tmp16F == 0?1: _tmp16F->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(
r,inst,(*((struct _tuple9*)_tmp16F->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(
struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(
fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp173;struct Cyc_List_List*
_tmp174;struct Cyc_Position_Segment*_tmp175;void*_tmp176;struct Cyc_Absyn_Tunionfield
_tmp172=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp173=_tmp172.name;_tmp174=
_tmp172.typs;_tmp175=_tmp172.loc;_tmp176=(void*)_tmp172.sc;if(_tmp174 == 0?1:
_tmp174->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp174->hd)).f2);if(Cyc_Tcutil_coerce_arg(te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}({struct Cyc_String_pa_struct _tmp17A;
_tmp17A.tag=0;_tmp17A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t1));{struct Cyc_String_pa_struct _tmp179;_tmp179.tag=0;_tmp179.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(tu));{void*_tmp177[2]={&
_tmp179,& _tmp17A};Cyc_Tcutil_terr(e->loc,({const char*_tmp178="can't find a field in %s to inject a value of type %s";
_tag_arr(_tmp178,sizeof(char),_get_zero_arr_size(_tmp178,54));}),_tag_arr(
_tmp177,sizeof(void*),2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){struct Cyc_List_List*
_tmp17C=args;te=Cyc_Tcenv_new_block(loc,te);Cyc_Tcexp_tcExp(te,0,e);{void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp17D=t;struct Cyc_Absyn_PtrInfo
_tmp17E;void*_tmp17F;struct Cyc_Absyn_Tqual _tmp180;struct Cyc_Absyn_PtrAtts _tmp181;
void*_tmp182;struct Cyc_Absyn_Conref*_tmp183;struct Cyc_Absyn_Conref*_tmp184;
struct Cyc_Absyn_Conref*_tmp185;_LL11B: if(_tmp17D <= (void*)3?1:*((int*)_tmp17D)!= 
4)goto _LL11D;_tmp17E=((struct Cyc_Absyn_PointerType_struct*)_tmp17D)->f1;_tmp17F=(
void*)_tmp17E.elt_typ;_tmp180=_tmp17E.elt_tq;_tmp181=_tmp17E.ptr_atts;_tmp182=(
void*)_tmp181.rgn;_tmp183=_tmp181.nullable;_tmp184=_tmp181.bounds;_tmp185=
_tmp181.zero_term;_LL11C: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp182);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp184);{void*_tmp186=Cyc_Tcutil_compress(_tmp17F);struct Cyc_Absyn_FnInfo
_tmp187;struct Cyc_List_List*_tmp188;struct Cyc_Core_Opt*_tmp189;void*_tmp18A;
struct Cyc_List_List*_tmp18B;int _tmp18C;struct Cyc_Absyn_VarargInfo*_tmp18D;struct
Cyc_List_List*_tmp18E;struct Cyc_List_List*_tmp18F;_LL120: if(_tmp186 <= (void*)3?1:*((
int*)_tmp186)!= 8)goto _LL122;_tmp187=((struct Cyc_Absyn_FnType_struct*)_tmp186)->f1;
_tmp188=_tmp187.tvars;_tmp189=_tmp187.effect;_tmp18A=(void*)_tmp187.ret_typ;
_tmp18B=_tmp187.args;_tmp18C=_tmp187.c_varargs;_tmp18D=_tmp187.cyc_varargs;
_tmp18E=_tmp187.rgn_po;_tmp18F=_tmp187.attributes;_LL121: if(topt != 0)Cyc_Tcutil_unify(
_tmp18A,*topt);while(_tmp17C != 0?_tmp18B != 0: 0){struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp17C->hd;void*t2=(*((struct _tuple2*)_tmp18B->hd)).f3;Cyc_Tcexp_tcExp(
te,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(te,e1,t2)){({struct Cyc_String_pa_struct
_tmp193;_tmp193.tag=0;_tmp193.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp192;_tmp192.tag=0;_tmp192.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v));{void*_tmp190[2]={& _tmp192,& _tmp193};Cyc_Tcutil_terr(
e1->loc,({const char*_tmp191="actual argument has type \n\t%s\n but formal has type \n\t%s.";
_tag_arr(_tmp191,sizeof(char),_get_zero_arr_size(_tmp191,57));}),_tag_arr(
_tmp190,sizeof(void*),2));}}});Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}_tmp17C=_tmp17C->tl;
_tmp18B=_tmp18B->tl;}{int args_already_checked=0;{struct Cyc_List_List*a=_tmp18F;
for(0;a != 0;a=a->tl){void*_tmp194=(void*)a->hd;void*_tmp195;int _tmp196;int
_tmp197;_LL125: if(_tmp194 <= (void*)17?1:*((int*)_tmp194)!= 3)goto _LL127;_tmp195=(
void*)((struct Cyc_Absyn_Format_att_struct*)_tmp194)->f1;_tmp196=((struct Cyc_Absyn_Format_att_struct*)
_tmp194)->f2;_tmp197=((struct Cyc_Absyn_Format_att_struct*)_tmp194)->f3;_LL126:{
struct _handler_cons _tmp198;_push_handler(& _tmp198);{int _tmp19A=0;if(setjmp(
_tmp198.handler))_tmp19A=1;if(!_tmp19A){{struct Cyc_Absyn_Exp*_tmp19B=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp196 - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp197 == 0)fmt_args=0;else{fmt_args=({struct Cyc_Core_Opt*_tmp19C=
_cycalloc(sizeof(*_tmp19C));_tmp19C->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,int i))Cyc_List_nth_tail)(args,_tmp197 - 1);_tmp19C;});}args_already_checked=1;{
void*_tmp19D=_tmp195;_LL12A: if((int)_tmp19D != 0)goto _LL12C;_LL12B: Cyc_Tcexp_check_format_args(
te,_tmp19B,fmt_args,Cyc_Formatstr_get_format_typs);goto _LL129;_LL12C: if((int)
_tmp19D != 1)goto _LL129;_LL12D: Cyc_Tcexp_check_format_args(te,_tmp19B,fmt_args,
Cyc_Formatstr_get_scanf_typs);goto _LL129;_LL129:;}};_pop_handler();}else{void*
_tmp199=(void*)_exn_thrown;void*_tmp19F=_tmp199;_LL12F: if(_tmp19F != Cyc_List_Nth)
goto _LL131;_LL130:({void*_tmp1A0[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1A1="bad format arguments";
_tag_arr(_tmp1A1,sizeof(char),_get_zero_arr_size(_tmp1A1,21));}),_tag_arr(
_tmp1A0,sizeof(void*),0));});goto _LL12E;_LL131:;_LL132:(void)_throw(_tmp19F);
_LL12E:;}}}goto _LL124;_LL127:;_LL128: goto _LL124;_LL124:;}}if(_tmp18B != 0)({void*
_tmp1A2[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1A3="too few arguments for function";
_tag_arr(_tmp1A3,sizeof(char),_get_zero_arr_size(_tmp1A3,31));}),_tag_arr(
_tmp1A2,sizeof(void*),0));});else{if((_tmp17C != 0?1: _tmp18C)?1: _tmp18D != 0){if(
_tmp18C)for(0;_tmp17C != 0;_tmp17C=_tmp17C->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp17C->hd);}else{if(_tmp18D == 0)({void*_tmp1A4[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp1A5="too many arguments for function";_tag_arr(_tmp1A5,sizeof(char),
_get_zero_arr_size(_tmp1A5,32));}),_tag_arr(_tmp1A4,sizeof(void*),0));});else{
void*_tmp1A7;int _tmp1A8;struct Cyc_Absyn_VarargInfo _tmp1A6=*_tmp18D;_tmp1A7=(void*)
_tmp1A6.type;_tmp1A8=_tmp1A6.inject;{struct Cyc_Absyn_VarargCallInfo*_tmp1A9=({
struct Cyc_Absyn_VarargCallInfo*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->num_varargs=
0;_tmp1C1->injectors=0;_tmp1C1->vai=(struct Cyc_Absyn_VarargInfo*)_tmp18D;_tmp1C1;});*
vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp1A9;if(!_tmp1A8)for(0;
_tmp17C != 0;_tmp17C=_tmp17C->tl){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)
_tmp17C->hd;_tmp1A9->num_varargs ++;Cyc_Tcexp_tcExp(te,(void**)& _tmp1A7,e1);if(!
Cyc_Tcutil_coerce_arg(te,e1,_tmp1A7)){({struct Cyc_String_pa_struct _tmp1AD;
_tmp1AD.tag=0;_tmp1AD.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{struct Cyc_String_pa_struct
_tmp1AC;_tmp1AC.tag=0;_tmp1AC.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp1A7));{void*_tmp1AA[2]={& _tmp1AC,& _tmp1AD};Cyc_Tcutil_terr(loc,({const char*
_tmp1AB="vararg requires type %s but argument has type %s";_tag_arr(_tmp1AB,
sizeof(char),_get_zero_arr_size(_tmp1AB,49));}),_tag_arr(_tmp1AA,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}}else{void*_tmp1AE=Cyc_Tcutil_compress(_tmp1A7);
struct Cyc_Absyn_TunionInfo _tmp1AF;void*_tmp1B0;struct Cyc_Absyn_Tuniondecl**
_tmp1B1;struct Cyc_Absyn_Tuniondecl*_tmp1B2;struct Cyc_List_List*_tmp1B3;void*
_tmp1B4;_LL134: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 2)goto _LL136;_tmp1AF=((
struct Cyc_Absyn_TunionType_struct*)_tmp1AE)->f1;_tmp1B0=(void*)_tmp1AF.tunion_info;
if(*((int*)_tmp1B0)!= 1)goto _LL136;_tmp1B1=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp1B0)->f1;_tmp1B2=*_tmp1B1;_tmp1B3=_tmp1AF.targs;_tmp1B4=(void*)_tmp1AF.rgn;
_LL135: {struct Cyc_Absyn_Tuniondecl*_tmp1B5=*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp1B2->name);struct Cyc_List_List*fields=0;if(_tmp1B5->fields == 0)({struct Cyc_String_pa_struct
_tmp1B8;_tmp1B8.tag=0;_tmp1B8.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp1A7));{void*_tmp1B6[1]={& _tmp1B8};Cyc_Tcutil_terr(loc,({const char*_tmp1B7="can't inject into %s";
_tag_arr(_tmp1B7,sizeof(char),_get_zero_arr_size(_tmp1B7,21));}),_tag_arr(
_tmp1B6,sizeof(void*),1));}});else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1B5->fields))->v;}if(!Cyc_Tcutil_unify(_tmp1B4,Cyc_Tcenv_curr_rgn(
te)))({void*_tmp1B9[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1BA="bad region for injected varargs";
_tag_arr(_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,32));}),_tag_arr(
_tmp1B9,sizeof(void*),0));});{struct _RegionHandle _tmp1BB=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp1BB;_push_region(rgn);{struct Cyc_List_List*_tmp1BC=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp1B5->tvs,_tmp1B3);for(0;
_tmp17C != 0;_tmp17C=_tmp17C->tl){_tmp1A9->num_varargs ++;{struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp17C->hd;if(!args_already_checked)Cyc_Tcexp_tcExp(te,0,
e1);{struct Cyc_Absyn_Tunionfield*_tmp1BD=Cyc_Tcexp_tcInjection(te,e1,_tmp1A7,rgn,
_tmp1BC,fields);if(_tmp1BD != 0)_tmp1A9->injectors=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1A9->injectors,({
struct Cyc_List_List*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->hd=(struct Cyc_Absyn_Tunionfield*)
_tmp1BD;_tmp1BE->tl=0;_tmp1BE;}));}}}};_pop_region(rgn);}goto _LL133;}_LL136:;
_LL137:({void*_tmp1BF[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1C0="bad inject vararg type";
_tag_arr(_tmp1C0,sizeof(char),_get_zero_arr_size(_tmp1C0,23));}),_tag_arr(
_tmp1BF,sizeof(void*),0));});goto _LL133;_LL133:;}}}}}}Cyc_Tcenv_check_effect_accessible(
te,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp189))->v);Cyc_Tcenv_check_rgn_partial_order(
te,loc,_tmp18E);return _tmp18A;}_LL122:;_LL123: return({void*_tmp1C2[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp1C3="expected pointer to function";_tag_arr(_tmp1C3,
sizeof(char),_get_zero_arr_size(_tmp1C3,29));}),_tag_arr(_tmp1C2,sizeof(void*),0));});
_LL11F:;}_LL11D:;_LL11E: return({void*_tmp1C4[0]={};Cyc_Tcexp_expr_err(te,loc,
topt,({const char*_tmp1C5="expected pointer to function";_tag_arr(_tmp1C5,sizeof(
char),_get_zero_arr_size(_tmp1C5,29));}),_tag_arr(_tmp1C4,sizeof(void*),0));});
_LL11A:;}}static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,
e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct
_tmp1C8;_tmp1C8.tag=0;_tmp1C8.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp1C6[1]={&
_tmp1C8};Cyc_Tcutil_terr(loc,({const char*_tmp1C7="expected xtunion exn but found %s";
_tag_arr(_tmp1C7,sizeof(char),_get_zero_arr_size(_tmp1C7,34));}),_tag_arr(
_tmp1C6,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp1C9;}));}static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
Cyc_Tcexp_tcExpNoInst(te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp1CA=t1;struct Cyc_Absyn_PtrInfo _tmp1CB;void*_tmp1CC;struct Cyc_Absyn_Tqual
_tmp1CD;struct Cyc_Absyn_PtrAtts _tmp1CE;void*_tmp1CF;struct Cyc_Absyn_Conref*
_tmp1D0;struct Cyc_Absyn_Conref*_tmp1D1;struct Cyc_Absyn_Conref*_tmp1D2;_LL139: if(
_tmp1CA <= (void*)3?1:*((int*)_tmp1CA)!= 4)goto _LL13B;_tmp1CB=((struct Cyc_Absyn_PointerType_struct*)
_tmp1CA)->f1;_tmp1CC=(void*)_tmp1CB.elt_typ;_tmp1CD=_tmp1CB.elt_tq;_tmp1CE=
_tmp1CB.ptr_atts;_tmp1CF=(void*)_tmp1CE.rgn;_tmp1D0=_tmp1CE.nullable;_tmp1D1=
_tmp1CE.bounds;_tmp1D2=_tmp1CE.zero_term;_LL13A:{void*_tmp1D3=Cyc_Tcutil_compress(
_tmp1CC);struct Cyc_Absyn_FnInfo _tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_Core_Opt*
_tmp1D6;void*_tmp1D7;struct Cyc_List_List*_tmp1D8;int _tmp1D9;struct Cyc_Absyn_VarargInfo*
_tmp1DA;struct Cyc_List_List*_tmp1DB;struct Cyc_List_List*_tmp1DC;_LL13E: if(
_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 8)goto _LL140;_tmp1D4=((struct Cyc_Absyn_FnType_struct*)
_tmp1D3)->f1;_tmp1D5=_tmp1D4.tvars;_tmp1D6=_tmp1D4.effect;_tmp1D7=(void*)_tmp1D4.ret_typ;
_tmp1D8=_tmp1D4.args;_tmp1D9=_tmp1D4.c_varargs;_tmp1DA=_tmp1D4.cyc_varargs;
_tmp1DB=_tmp1D4.rgn_po;_tmp1DC=_tmp1D4.attributes;_LL13F: {struct Cyc_List_List*
instantiation=0;for(0;ts != 0?_tmp1D5 != 0: 0;(ts=ts->tl,_tmp1D5=_tmp1D5->tl)){void*
k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp1D5->hd);Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);instantiation=({struct
Cyc_List_List*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD->hd=({struct _tuple5*
_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE->f1=(struct Cyc_Absyn_Tvar*)_tmp1D5->hd;
_tmp1DE->f2=(void*)ts->hd;_tmp1DE;});_tmp1DD->tl=instantiation;_tmp1DD;});}if(ts
!= 0)return({void*_tmp1DF[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp1E0="too many type variables in instantiation";_tag_arr(_tmp1E0,sizeof(char),
_get_zero_arr_size(_tmp1E0,41));}),_tag_arr(_tmp1DF,sizeof(void*),0));});{void*
new_fn_typ=Cyc_Tcutil_substitute(instantiation,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5[0]=({struct Cyc_Absyn_FnType_struct
_tmp1E6;_tmp1E6.tag=8;_tmp1E6.f1=({struct Cyc_Absyn_FnInfo _tmp1E7;_tmp1E7.tvars=
_tmp1D5;_tmp1E7.effect=_tmp1D6;_tmp1E7.ret_typ=(void*)_tmp1D7;_tmp1E7.args=
_tmp1D8;_tmp1E7.c_varargs=_tmp1D9;_tmp1E7.cyc_varargs=_tmp1DA;_tmp1E7.rgn_po=
_tmp1DB;_tmp1E7.attributes=_tmp1DC;_tmp1E7;});_tmp1E6;});_tmp1E5;}));return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1[0]=({
struct Cyc_Absyn_PointerType_struct _tmp1E2;_tmp1E2.tag=4;_tmp1E2.f1=({struct Cyc_Absyn_PtrInfo
_tmp1E3;_tmp1E3.elt_typ=(void*)new_fn_typ;_tmp1E3.elt_tq=_tmp1CD;_tmp1E3.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp1E4;_tmp1E4.rgn=(void*)_tmp1CF;_tmp1E4.nullable=
_tmp1D0;_tmp1E4.bounds=_tmp1D1;_tmp1E4.zero_term=_tmp1D2;_tmp1E4;});_tmp1E3;});
_tmp1E2;});_tmp1E1;});}}_LL140:;_LL141: goto _LL13D;_LL13D:;}goto _LL138;_LL13B:;
_LL13C: goto _LL138;_LL138:;}return({struct Cyc_String_pa_struct _tmp1EA;_tmp1EA.tag=
0;_tmp1EA.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{
void*_tmp1E8[1]={& _tmp1EA};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp1E9="expecting polymorphic type but found %s";
_tag_arr(_tmp1E9,sizeof(char),_get_zero_arr_size(_tmp1E9,40));}),_tag_arr(
_tmp1E8,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,void**
c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);Cyc_Tcexp_tcExp(
te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((void**)_check_null(c))=(void*)1;
else{void*crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != (void*)0)*((void**)
_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((void**)
_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{void*_tmp1EB=({struct Cyc_String_pa_struct
_tmp1EF;_tmp1EF.tag=0;_tmp1EF.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp1EE;_tmp1EE.tag=0;_tmp1EE.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{void*_tmp1EC[2]={&
_tmp1EE,& _tmp1EF};Cyc_Tcexp_expr_err(te,loc,(void**)& t,({const char*_tmp1ED="cannot cast %s to %s";
_tag_arr(_tmp1ED,sizeof(char),_get_zero_arr_size(_tmp1ED,21));}),_tag_arr(
_tmp1EC,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return _tmp1EB;}}}}{
struct _tuple0 _tmp1F1=({struct _tuple0 _tmp1F0;_tmp1F0.f1=(void*)e->r;_tmp1F0.f2=
Cyc_Tcutil_compress(t);_tmp1F0;});void*_tmp1F2;struct Cyc_Absyn_MallocInfo _tmp1F3;
int _tmp1F4;void*_tmp1F5;struct Cyc_Absyn_PtrInfo _tmp1F6;struct Cyc_Absyn_PtrAtts
_tmp1F7;struct Cyc_Absyn_Conref*_tmp1F8;struct Cyc_Absyn_Conref*_tmp1F9;struct Cyc_Absyn_Conref*
_tmp1FA;_LL143: _tmp1F2=_tmp1F1.f1;if(*((int*)_tmp1F2)!= 33)goto _LL145;_tmp1F3=((
struct Cyc_Absyn_Malloc_e_struct*)_tmp1F2)->f1;_tmp1F4=_tmp1F3.fat_result;_tmp1F5=
_tmp1F1.f2;if(_tmp1F5 <= (void*)3?1:*((int*)_tmp1F5)!= 4)goto _LL145;_tmp1F6=((
struct Cyc_Absyn_PointerType_struct*)_tmp1F5)->f1;_tmp1F7=_tmp1F6.ptr_atts;
_tmp1F8=_tmp1F7.nullable;_tmp1F9=_tmp1F7.bounds;_tmp1FA=_tmp1F7.zero_term;_LL144:
if((_tmp1F4?!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp1FA): 0)?((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp1F8): 0){void*_tmp1FB=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp1F9);
struct Cyc_Absyn_Exp*_tmp1FC;_LL148: if(_tmp1FB <= (void*)1?1:*((int*)_tmp1FB)!= 0)
goto _LL14A;_tmp1FC=((struct Cyc_Absyn_Upper_b_struct*)_tmp1FB)->f1;_LL149: if((Cyc_Evexp_eval_const_uint_exp(
_tmp1FC)).f1 == 1)({void*_tmp1FD[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp1FE="cast from ? pointer to * pointer will lose size information";
_tag_arr(_tmp1FE,sizeof(char),_get_zero_arr_size(_tmp1FE,60));}),_tag_arr(
_tmp1FD,sizeof(void*),0));});goto _LL147;_LL14A:;_LL14B: goto _LL147;_LL147:;}goto
_LL142;_LL145:;_LL146: goto _LL142;_LL142:;}return t;}}static void*Cyc_Tcexp_tcAddress(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,
void**topt,struct Cyc_Absyn_Exp*e){void**_tmp1FF=0;struct Cyc_Absyn_Tqual _tmp200=
Cyc_Absyn_empty_tqual();if(topt != 0){void*_tmp201=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmp202;void*_tmp203;struct Cyc_Absyn_Tqual _tmp204;struct
Cyc_Absyn_PtrAtts _tmp205;struct Cyc_Absyn_Conref*_tmp206;_LL14D: if(_tmp201 <= (
void*)3?1:*((int*)_tmp201)!= 4)goto _LL14F;_tmp202=((struct Cyc_Absyn_PointerType_struct*)
_tmp201)->f1;_tmp203=(void*)_tmp202.elt_typ;_tmp204=_tmp202.elt_tq;_tmp205=
_tmp202.ptr_atts;_tmp206=_tmp205.zero_term;_LL14E: _tmp1FF=({void**_tmp207=
_cycalloc(sizeof(*_tmp207));_tmp207[0]=_tmp203;_tmp207;});_tmp200=_tmp204;goto
_LL14C;_LL14F:;_LL150: goto _LL14C;_LL14C:;}Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(
te),_tmp1FF,e);{void*_tmp208=(void*)e->r;struct Cyc_Absyn_Exp*_tmp209;struct Cyc_Absyn_Exp*
_tmp20A;_LL152: if(*((int*)_tmp208)!= 23)goto _LL154;_tmp209=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp208)->f1;_tmp20A=((struct Cyc_Absyn_Subscript_e_struct*)_tmp208)->f2;_LL153:{
void*_tmp20B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp209->topt))->v);_LL157: if(_tmp20B <= (void*)3?1:*((int*)_tmp20B)!= 9)goto
_LL159;_LL158: goto _LL156;_LL159:;_LL15A:(void*)(e0->r=(void*)((void*)(Cyc_Absyn_add_exp(
_tmp209,_tmp20A,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp209,_tmp20A);_LL156:;}
goto _LL151;_LL154:;_LL155: goto _LL151;_LL151:;}{int _tmp20D;void*_tmp20E;struct
_tuple6 _tmp20C=Cyc_Tcutil_addressof_props(te,e);_tmp20D=_tmp20C.f1;_tmp20E=
_tmp20C.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual();if(_tmp20D){tq.print_const=
1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp20E,tq,Cyc_Absyn_false_conref);return t;}}}}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({struct Cyc_String_pa_struct _tmp211;
_tmp211.tag=0;_tmp211.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp20F[1]={& _tmp211};Cyc_Tcutil_terr(loc,({const char*_tmp210="sizeof applied to type %s, which has unknown size here";
_tag_arr(_tmp210,sizeof(char),_get_zero_arr_size(_tmp210,55));}),_tag_arr(
_tmp20F,sizeof(void*),1));}});if(topt != 0){void*_tmp212=Cyc_Tcutil_compress(*
topt);_LL15C: if(_tmp212 <= (void*)3?1:*((int*)_tmp212)!= 14)goto _LL15E;_LL15D:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp213=_cycalloc(sizeof(*
_tmp213));_tmp213[0]=({struct Cyc_Absyn_SizeofType_struct _tmp214;_tmp214.tag=14;
_tmp214.f1=(void*)t;_tmp214;});_tmp213;});_LL15E:;_LL15F: goto _LL15B;_LL15B:;}
return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(struct _tagged_arr*n,
struct Cyc_Absyn_Aggrfield*sf){return Cyc_strcmp((struct _tagged_arr)*n,(struct
_tagged_arr)*sf->name)== 0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*_tmp215=n;struct
_tagged_arr*_tmp216;unsigned int _tmp217;_LL161: if(*((int*)_tmp215)!= 0)goto
_LL163;_tmp216=((struct Cyc_Absyn_StructField_struct*)_tmp215)->f1;_LL162: {int
bad_type=1;{void*_tmp218=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp219;
void*_tmp21A;struct Cyc_Absyn_Aggrdecl**_tmp21B;struct Cyc_List_List*_tmp21C;
_LL166: if(_tmp218 <= (void*)3?1:*((int*)_tmp218)!= 10)goto _LL168;_tmp219=((struct
Cyc_Absyn_AggrType_struct*)_tmp218)->f1;_tmp21A=(void*)_tmp219.aggr_info;if(*((
int*)_tmp21A)!= 1)goto _LL168;_tmp21B=((struct Cyc_Absyn_KnownAggr_struct*)_tmp21A)->f1;
_LL167: if((*_tmp21B)->impl == 0)goto _LL165;if(!((int(*)(int(*pred)(struct
_tagged_arr*,struct Cyc_Absyn_Aggrfield*),struct _tagged_arr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp216,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp21B)->impl))->fields))({struct Cyc_String_pa_struct _tmp21F;
_tmp21F.tag=0;_tmp21F.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp216);{void*
_tmp21D[1]={& _tmp21F};Cyc_Tcutil_terr(loc,({const char*_tmp21E="no field of struct/union has name %s";
_tag_arr(_tmp21E,sizeof(char),_get_zero_arr_size(_tmp21E,37));}),_tag_arr(
_tmp21D,sizeof(void*),1));}});bad_type=0;goto _LL165;_LL168: if(_tmp218 <= (void*)3?
1:*((int*)_tmp218)!= 11)goto _LL16A;_tmp21C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp218)->f2;_LL169: if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp216,_tmp21C))({struct Cyc_String_pa_struct _tmp222;_tmp222.tag=0;_tmp222.f1=(
struct _tagged_arr)((struct _tagged_arr)*_tmp216);{void*_tmp220[1]={& _tmp222};Cyc_Tcutil_terr(
loc,({const char*_tmp221="no field of struct/union has name %s";_tag_arr(_tmp221,
sizeof(char),_get_zero_arr_size(_tmp221,37));}),_tag_arr(_tmp220,sizeof(void*),1));}});
bad_type=0;goto _LL165;_LL16A:;_LL16B: goto _LL165;_LL165:;}if(bad_type)({struct Cyc_String_pa_struct
_tmp225;_tmp225.tag=0;_tmp225.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp223[1]={& _tmp225};Cyc_Tcutil_terr(loc,({const char*_tmp224="%s is not a known struct/union type";
_tag_arr(_tmp224,sizeof(char),_get_zero_arr_size(_tmp224,36));}),_tag_arr(
_tmp223,sizeof(void*),1));}});goto _LL160;}_LL163: if(*((int*)_tmp215)!= 1)goto
_LL160;_tmp217=((struct Cyc_Absyn_TupleIndex_struct*)_tmp215)->f1;_LL164: {int
bad_type=1;{void*_tmp226=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp227;
void*_tmp228;struct Cyc_Absyn_Aggrdecl**_tmp229;struct Cyc_List_List*_tmp22A;
struct Cyc_List_List*_tmp22B;struct Cyc_Absyn_TunionFieldInfo _tmp22C;void*_tmp22D;
struct Cyc_Absyn_Tunionfield*_tmp22E;_LL16D: if(_tmp226 <= (void*)3?1:*((int*)
_tmp226)!= 10)goto _LL16F;_tmp227=((struct Cyc_Absyn_AggrType_struct*)_tmp226)->f1;
_tmp228=(void*)_tmp227.aggr_info;if(*((int*)_tmp228)!= 1)goto _LL16F;_tmp229=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp228)->f1;_LL16E: if((*_tmp229)->impl == 0)
goto _LL16C;_tmp22A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp229)->impl))->fields;
goto _LL170;_LL16F: if(_tmp226 <= (void*)3?1:*((int*)_tmp226)!= 11)goto _LL171;
_tmp22A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp226)->f2;_LL170: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp22A)<= _tmp217)({struct Cyc_Int_pa_struct
_tmp232;_tmp232.tag=1;_tmp232.f1=(unsigned int)((int)_tmp217);{struct Cyc_Int_pa_struct
_tmp231;_tmp231.tag=1;_tmp231.f1=(unsigned int)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp22A);{void*_tmp22F[2]={& _tmp231,& _tmp232};Cyc_Tcutil_terr(
loc,({const char*_tmp230="struct/union has too few components: %d <= %d";_tag_arr(
_tmp230,sizeof(char),_get_zero_arr_size(_tmp230,46));}),_tag_arr(_tmp22F,sizeof(
void*),2));}}});bad_type=0;goto _LL16C;_LL171: if(_tmp226 <= (void*)3?1:*((int*)
_tmp226)!= 9)goto _LL173;_tmp22B=((struct Cyc_Absyn_TupleType_struct*)_tmp226)->f1;
_LL172: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp22B)<= _tmp217)({
struct Cyc_Int_pa_struct _tmp236;_tmp236.tag=1;_tmp236.f1=(unsigned int)((int)
_tmp217);{struct Cyc_Int_pa_struct _tmp235;_tmp235.tag=1;_tmp235.f1=(unsigned int)((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp22B);{void*_tmp233[2]={&
_tmp235,& _tmp236};Cyc_Tcutil_terr(loc,({const char*_tmp234="tuple has too few components: %d <= %d";
_tag_arr(_tmp234,sizeof(char),_get_zero_arr_size(_tmp234,39));}),_tag_arr(
_tmp233,sizeof(void*),2));}}});bad_type=0;goto _LL16C;_LL173: if(_tmp226 <= (void*)
3?1:*((int*)_tmp226)!= 3)goto _LL175;_tmp22C=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp226)->f1;_tmp22D=(void*)_tmp22C.field_info;if(*((int*)_tmp22D)!= 1)goto
_LL175;_tmp22E=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp22D)->f2;_LL174:
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp22E->typs)< _tmp217)({
struct Cyc_Int_pa_struct _tmp23A;_tmp23A.tag=1;_tmp23A.f1=(unsigned int)((int)
_tmp217);{struct Cyc_Int_pa_struct _tmp239;_tmp239.tag=1;_tmp239.f1=(unsigned int)((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp22E->typs);{void*_tmp237[2]={&
_tmp239,& _tmp23A};Cyc_Tcutil_terr(loc,({const char*_tmp238="tunionfield has too few components: %d < %d";
_tag_arr(_tmp238,sizeof(char),_get_zero_arr_size(_tmp238,44));}),_tag_arr(
_tmp237,sizeof(void*),2));}}});bad_type=0;goto _LL16C;_LL175:;_LL176: goto _LL16C;
_LL16C:;}if(bad_type)({struct Cyc_String_pa_struct _tmp23D;_tmp23D.tag=0;_tmp23D.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp23B[1]={&
_tmp23D};Cyc_Tcutil_terr(loc,({const char*_tmp23C="%s is not a known type";
_tag_arr(_tmp23C,sizeof(char),_get_zero_arr_size(_tmp23C,23));}),_tag_arr(
_tmp23B,sizeof(void*),1));}});goto _LL160;}_LL160:;}return Cyc_Absyn_uint_typ;}
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){te=Cyc_Tcenv_clear_notreadctxt(te);Cyc_Tcexp_tcExp(
te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp23E=t;struct Cyc_Absyn_PtrInfo _tmp23F;void*_tmp240;struct Cyc_Absyn_PtrAtts
_tmp241;void*_tmp242;struct Cyc_Absyn_Conref*_tmp243;struct Cyc_Absyn_Conref*
_tmp244;_LL178: if(_tmp23E <= (void*)3?1:*((int*)_tmp23E)!= 4)goto _LL17A;_tmp23F=((
struct Cyc_Absyn_PointerType_struct*)_tmp23E)->f1;_tmp240=(void*)_tmp23F.elt_typ;
_tmp241=_tmp23F.ptr_atts;_tmp242=(void*)_tmp241.rgn;_tmp243=_tmp241.bounds;
_tmp244=_tmp241.zero_term;_LL179: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp242);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp243);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
_tmp240),(void*)1))({void*_tmp245[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp246="can't dereference abstract pointer type";
_tag_arr(_tmp246,sizeof(char),_get_zero_arr_size(_tmp246,40));}),_tag_arr(
_tmp245,sizeof(void*),0));});return _tmp240;_LL17A:;_LL17B: return({struct Cyc_String_pa_struct
_tmp249;_tmp249.tag=0;_tmp249.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp247[1]={& _tmp249};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp248="expecting * or @ type but found %s";_tag_arr(_tmp248,sizeof(char),
_get_zero_arr_size(_tmp248,35));}),_tag_arr(_tmp247,sizeof(void*),1));}});_LL177:;}}
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_tagged_arr*f){Cyc_Tcexp_tcExpNoPromote(te,0,e);{void*_tmp24A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp24B;void*_tmp24C;struct Cyc_Absyn_Aggrdecl**_tmp24D;struct Cyc_Absyn_Aggrdecl*
_tmp24E;struct Cyc_List_List*_tmp24F;void*_tmp250;struct Cyc_List_List*_tmp251;
_LL17D: if(_tmp24A <= (void*)3?1:*((int*)_tmp24A)!= 10)goto _LL17F;_tmp24B=((struct
Cyc_Absyn_AggrType_struct*)_tmp24A)->f1;_tmp24C=(void*)_tmp24B.aggr_info;if(*((
int*)_tmp24C)!= 1)goto _LL17F;_tmp24D=((struct Cyc_Absyn_KnownAggr_struct*)_tmp24C)->f1;
_tmp24E=*_tmp24D;_tmp24F=_tmp24B.targs;_LL17E: {struct Cyc_Absyn_Aggrfield*
_tmp254=Cyc_Absyn_lookup_decl_field(_tmp24E,f);if(_tmp254 == 0)return({struct Cyc_String_pa_struct
_tmp258;_tmp258.tag=0;_tmp258.f1=(struct _tagged_arr)((struct _tagged_arr)*f);{
struct Cyc_String_pa_struct _tmp257;_tmp257.tag=0;_tmp257.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp24E->name));{void*_tmp255[2]={&
_tmp257,& _tmp258};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp256="type %s has no %s field";
_tag_arr(_tmp256,sizeof(char),_get_zero_arr_size(_tmp256,24));}),_tag_arr(
_tmp255,sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp24E->impl))->exist_vars != 0)return({void*_tmp259[0]={};Cyc_Tcexp_expr_err(te,
loc,topt,({const char*_tmp25A="must use pattern-matching to access fields of existential types";
_tag_arr(_tmp25A,sizeof(char),_get_zero_arr_size(_tmp25A,64));}),_tag_arr(
_tmp259,sizeof(void*),0));});{void*t2;{struct _RegionHandle _tmp25B=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp25B;_push_region(rgn);{struct Cyc_List_List*_tmp25C=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp24E->tvs,_tmp24F);t2=Cyc_Tcutil_rsubstitute(
rgn,_tmp25C,(void*)_tmp254->type);if(((void*)_tmp24E->kind == (void*)1?!Cyc_Tcenv_in_notreadctxt(
te): 0)?!Cyc_Tcutil_bits_only(t2): 0){void*_tmp260=({struct Cyc_String_pa_struct
_tmp25F;_tmp25F.tag=0;_tmp25F.f1=(struct _tagged_arr)((struct _tagged_arr)*f);{
void*_tmp25D[1]={& _tmp25F};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp25E="cannot read union member %s since it is not `bits-only'";
_tag_arr(_tmp25E,sizeof(char),_get_zero_arr_size(_tmp25E,56));}),_tag_arr(
_tmp25D,sizeof(void*),1));}});_npop_handler(0);return _tmp260;}};_pop_region(rgn);}
return t2;}}_LL17F: if(_tmp24A <= (void*)3?1:*((int*)_tmp24A)!= 11)goto _LL181;
_tmp250=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp24A)->f1;_tmp251=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp24A)->f2;_LL180: {struct Cyc_Absyn_Aggrfield*
_tmp261=Cyc_Absyn_lookup_field(_tmp251,f);if(_tmp261 == 0)return({struct Cyc_String_pa_struct
_tmp264;_tmp264.tag=0;_tmp264.f1=(struct _tagged_arr)((struct _tagged_arr)*f);{
void*_tmp262[1]={& _tmp264};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp263="type has no %s field";
_tag_arr(_tmp263,sizeof(char),_get_zero_arr_size(_tmp263,21));}),_tag_arr(
_tmp262,sizeof(void*),1));}});if((_tmp250 == (void*)1?!Cyc_Tcenv_in_notreadctxt(
te): 0)?!Cyc_Tcutil_bits_only((void*)_tmp261->type): 0)return({struct Cyc_String_pa_struct
_tmp267;_tmp267.tag=0;_tmp267.f1=(struct _tagged_arr)((struct _tagged_arr)*f);{
void*_tmp265[1]={& _tmp267};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp266="cannot read union member %s since it is not `bits-only'";
_tag_arr(_tmp266,sizeof(char),_get_zero_arr_size(_tmp266,56));}),_tag_arr(
_tmp265,sizeof(void*),1));}});return(void*)_tmp261->type;}_LL181: if(_tmp24A <= (
void*)3?1:*((int*)_tmp24A)!= 7)goto _LL183;if(!(Cyc_strcmp((struct _tagged_arr)*f,({
const char*_tmp252="size";_tag_arr(_tmp252,sizeof(char),_get_zero_arr_size(
_tmp252,5));}))== 0))goto _LL183;_LL182: goto _LL184;_LL183: if(_tmp24A <= (void*)3?1:*((
int*)_tmp24A)!= 4)goto _LL185;if(!(Cyc_strcmp((struct _tagged_arr)*f,({const char*
_tmp253="size";_tag_arr(_tmp253,sizeof(char),_get_zero_arr_size(_tmp253,5));}))
== 0))goto _LL185;_LL184:(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp269;_tmp269.tag=3;_tmp269.f1=(void*)((void*)19);_tmp269.f2=({struct Cyc_List_List*
_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->hd=e;_tmp26A->tl=0;_tmp26A;});
_tmp269;});_tmp268;})));return Cyc_Absyn_uint_typ;_LL185:;_LL186: if(Cyc_strcmp((
struct _tagged_arr)*f,({const char*_tmp26B="size";_tag_arr(_tmp26B,sizeof(char),
_get_zero_arr_size(_tmp26B,5));}))== 0)return({struct Cyc_String_pa_struct _tmp26E;
_tmp26E.tag=0;_tmp26E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp26C[1]={&
_tmp26E};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp26D="expecting struct, union, or array, found %s";
_tag_arr(_tmp26D,sizeof(char),_get_zero_arr_size(_tmp26D,44));}),_tag_arr(
_tmp26C,sizeof(void*),1));}});else{return({struct Cyc_String_pa_struct _tmp271;
_tmp271.tag=0;_tmp271.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp26F[1]={&
_tmp271};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp270="expecting struct or union, found %s";
_tag_arr(_tmp270,sizeof(char),_get_zero_arr_size(_tmp270,36));}),_tag_arr(
_tmp26F,sizeof(void*),1));}});}_LL17C:;}}static void*Cyc_Tcexp_tcAggrArrow(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _tagged_arr*f){Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(te),0,e);{
void*_tmp272=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp273;void*_tmp274;struct Cyc_Absyn_PtrAtts _tmp275;void*
_tmp276;struct Cyc_Absyn_Conref*_tmp277;struct Cyc_Absyn_Conref*_tmp278;_LL188: if(
_tmp272 <= (void*)3?1:*((int*)_tmp272)!= 4)goto _LL18A;_tmp273=((struct Cyc_Absyn_PointerType_struct*)
_tmp272)->f1;_tmp274=(void*)_tmp273.elt_typ;_tmp275=_tmp273.ptr_atts;_tmp276=(
void*)_tmp275.rgn;_tmp277=_tmp275.bounds;_tmp278=_tmp275.zero_term;_LL189: Cyc_Tcutil_check_nonzero_bound(
loc,_tmp277);{void*_tmp279=Cyc_Tcutil_compress(_tmp274);struct Cyc_Absyn_AggrInfo
_tmp27A;void*_tmp27B;struct Cyc_Absyn_Aggrdecl**_tmp27C;struct Cyc_Absyn_Aggrdecl*
_tmp27D;struct Cyc_List_List*_tmp27E;void*_tmp27F;struct Cyc_List_List*_tmp280;
_LL18D: if(_tmp279 <= (void*)3?1:*((int*)_tmp279)!= 10)goto _LL18F;_tmp27A=((struct
Cyc_Absyn_AggrType_struct*)_tmp279)->f1;_tmp27B=(void*)_tmp27A.aggr_info;if(*((
int*)_tmp27B)!= 1)goto _LL18F;_tmp27C=((struct Cyc_Absyn_KnownAggr_struct*)_tmp27B)->f1;
_tmp27D=*_tmp27C;_tmp27E=_tmp27A.targs;_LL18E: {struct Cyc_Absyn_Aggrfield*
_tmp281=Cyc_Absyn_lookup_decl_field(_tmp27D,f);if(_tmp281 == 0)return({struct Cyc_String_pa_struct
_tmp285;_tmp285.tag=0;_tmp285.f1=(struct _tagged_arr)((struct _tagged_arr)*f);{
struct Cyc_String_pa_struct _tmp284;_tmp284.tag=0;_tmp284.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp27D->name));{void*_tmp282[2]={&
_tmp284,& _tmp285};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp283="type %s has no %s field";
_tag_arr(_tmp283,sizeof(char),_get_zero_arr_size(_tmp283,24));}),_tag_arr(
_tmp282,sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp27D->impl))->exist_vars != 0)return({void*_tmp286[0]={};Cyc_Tcexp_expr_err(te,
loc,topt,({const char*_tmp287="must use pattern-matching to access fields of existential types";
_tag_arr(_tmp287,sizeof(char),_get_zero_arr_size(_tmp287,64));}),_tag_arr(
_tmp286,sizeof(void*),0));});{void*t3;{struct _RegionHandle _tmp288=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp288;_push_region(rgn);{struct Cyc_List_List*_tmp289=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp27D->tvs,_tmp27E);t3=Cyc_Tcutil_rsubstitute(
rgn,_tmp289,(void*)_tmp281->type);};_pop_region(rgn);}if(((void*)_tmp27D->kind == (
void*)1?!Cyc_Tcenv_in_notreadctxt(te): 0)?!Cyc_Tcutil_bits_only(t3): 0)return({
struct Cyc_String_pa_struct _tmp28C;_tmp28C.tag=0;_tmp28C.f1=(struct _tagged_arr)((
struct _tagged_arr)*f);{void*_tmp28A[1]={& _tmp28C};Cyc_Tcexp_expr_err(te,loc,topt,({
const char*_tmp28B="cannot read union member %s since it is not `bits-only'";
_tag_arr(_tmp28B,sizeof(char),_get_zero_arr_size(_tmp28B,56));}),_tag_arr(
_tmp28A,sizeof(void*),1));}});return t3;}}_LL18F: if(_tmp279 <= (void*)3?1:*((int*)
_tmp279)!= 11)goto _LL191;_tmp27F=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp279)->f1;_tmp280=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp279)->f2;_LL190: {
struct Cyc_Absyn_Aggrfield*_tmp28D=Cyc_Absyn_lookup_field(_tmp280,f);if(_tmp28D == 
0)return({struct Cyc_String_pa_struct _tmp290;_tmp290.tag=0;_tmp290.f1=(struct
_tagged_arr)((struct _tagged_arr)*f);{void*_tmp28E[1]={& _tmp290};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp28F="type has no %s field";_tag_arr(_tmp28F,sizeof(
char),_get_zero_arr_size(_tmp28F,21));}),_tag_arr(_tmp28E,sizeof(void*),1));}});
if((_tmp27F == (void*)1?!Cyc_Tcenv_in_notreadctxt(te): 0)?!Cyc_Tcutil_bits_only((
void*)_tmp28D->type): 0)return({struct Cyc_String_pa_struct _tmp293;_tmp293.tag=0;
_tmp293.f1=(struct _tagged_arr)((struct _tagged_arr)*f);{void*_tmp291[1]={& _tmp293};
Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp292="cannot read union member %s since it is not `bits-only'";
_tag_arr(_tmp292,sizeof(char),_get_zero_arr_size(_tmp292,56));}),_tag_arr(
_tmp291,sizeof(void*),1));}});return(void*)_tmp28D->type;}_LL191:;_LL192: goto
_LL18C;_LL18C:;}goto _LL187;_LL18A:;_LL18B: goto _LL187;_LL187:;}return({struct Cyc_String_pa_struct
_tmp296;_tmp296.tag=0;_tmp296.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp294[1]={&
_tmp296};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp295="expecting struct or union pointer, found %s";
_tag_arr(_tmp295,sizeof(char),_get_zero_arr_size(_tmp295,44));}),_tag_arr(
_tmp294,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*
index){unsigned int _tmp298;int _tmp299;struct _tuple7 _tmp297=Cyc_Evexp_eval_const_uint_exp(
index);_tmp298=_tmp297.f1;_tmp299=_tmp297.f2;if(!_tmp299)return({void*_tmp29A[0]={};
Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp29B="tuple projection cannot use sizeof or offsetof";
_tag_arr(_tmp29B,sizeof(char),_get_zero_arr_size(_tmp29B,47));}),_tag_arr(
_tmp29A,sizeof(void*),0));});{struct _handler_cons _tmp29C;_push_handler(& _tmp29C);{
int _tmp29E=0;if(setjmp(_tmp29C.handler))_tmp29E=1;if(!_tmp29E){{void*_tmp29F=(*((
struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp298)).f2;
_npop_handler(0);return _tmp29F;};_pop_handler();}else{void*_tmp29D=(void*)
_exn_thrown;void*_tmp2A1=_tmp29D;_LL194: if(_tmp2A1 != Cyc_List_Nth)goto _LL196;
_LL195: return({struct Cyc_Int_pa_struct _tmp2A5;_tmp2A5.tag=1;_tmp2A5.f1=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);{struct Cyc_Int_pa_struct
_tmp2A4;_tmp2A4.tag=1;_tmp2A4.f1=(unsigned int)((int)_tmp298);{void*_tmp2A2[2]={&
_tmp2A4,& _tmp2A5};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp2A3="index is %d but tuple has only %d fields";
_tag_arr(_tmp2A3,sizeof(char),_get_zero_arr_size(_tmp2A3,41));}),_tag_arr(
_tmp2A2,sizeof(void*),2));}}});_LL196:;_LL197:(void)_throw(_tmp2A1);_LL193:;}}}}
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){te=Cyc_Tcenv_clear_notreadctxt(
te);Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,0,e2);{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!Cyc_Tcutil_coerce_sint_typ(
te,e2))return({struct Cyc_String_pa_struct _tmp2A8;_tmp2A8.tag=0;_tmp2A8.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{void*_tmp2A6[
1]={& _tmp2A8};Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*_tmp2A7="expecting int subscript, found %s";
_tag_arr(_tmp2A7,sizeof(char),_get_zero_arr_size(_tmp2A7,34));}),_tag_arr(
_tmp2A6,sizeof(void*),1));}});{void*_tmp2A9=t1;struct Cyc_Absyn_PtrInfo _tmp2AA;
void*_tmp2AB;struct Cyc_Absyn_Tqual _tmp2AC;struct Cyc_Absyn_PtrAtts _tmp2AD;void*
_tmp2AE;struct Cyc_Absyn_Conref*_tmp2AF;struct Cyc_Absyn_Conref*_tmp2B0;struct Cyc_List_List*
_tmp2B1;_LL199: if(_tmp2A9 <= (void*)3?1:*((int*)_tmp2A9)!= 4)goto _LL19B;_tmp2AA=((
struct Cyc_Absyn_PointerType_struct*)_tmp2A9)->f1;_tmp2AB=(void*)_tmp2AA.elt_typ;
_tmp2AC=_tmp2AA.elt_tq;_tmp2AD=_tmp2AA.ptr_atts;_tmp2AE=(void*)_tmp2AD.rgn;
_tmp2AF=_tmp2AD.bounds;_tmp2B0=_tmp2AD.zero_term;_LL19A: if(((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2B0)){int emit_warning=1;{void*
_tmp2B2=(void*)(Cyc_Absyn_compress_conref(_tmp2AF))->v;void*_tmp2B3;struct Cyc_Absyn_Exp*
_tmp2B4;void*_tmp2B5;_LL1A0: if(_tmp2B2 <= (void*)1?1:*((int*)_tmp2B2)!= 0)goto
_LL1A2;_tmp2B3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp2B2)->f1;if(
_tmp2B3 <= (void*)1?1:*((int*)_tmp2B3)!= 0)goto _LL1A2;_tmp2B4=((struct Cyc_Absyn_Upper_b_struct*)
_tmp2B3)->f1;_LL1A1: if(Cyc_Tcutil_is_const_exp(te,e2)){unsigned int _tmp2B7;int
_tmp2B8;struct _tuple7 _tmp2B6=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2B7=_tmp2B6.f1;
_tmp2B8=_tmp2B6.f2;if(_tmp2B8){unsigned int _tmp2BA;int _tmp2BB;struct _tuple7
_tmp2B9=Cyc_Evexp_eval_const_uint_exp(_tmp2B4);_tmp2BA=_tmp2B9.f1;_tmp2BB=
_tmp2B9.f2;if(_tmp2BB?_tmp2BA > _tmp2B7: 0)emit_warning=0;}}goto _LL19F;_LL1A2: if(
_tmp2B2 <= (void*)1?1:*((int*)_tmp2B2)!= 0)goto _LL1A4;_tmp2B5=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2B2)->f1;if((int)_tmp2B5 != 0)goto _LL1A4;_LL1A3: emit_warning=0;goto _LL19F;
_LL1A4:;_LL1A5: goto _LL19F;_LL19F:;}if(emit_warning)({void*_tmp2BC[0]={};Cyc_Tcutil_warn(
e2->loc,({const char*_tmp2BD="subscript on thin, zero-terminated pointer could be expensive.";
_tag_arr(_tmp2BD,sizeof(char),_get_zero_arr_size(_tmp2BD,63));}),_tag_arr(
_tmp2BC,sizeof(void*),0));});}else{if(Cyc_Tcutil_is_const_exp(te,e2)){
unsigned int _tmp2BF;int _tmp2C0;struct _tuple7 _tmp2BE=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp2BF=_tmp2BE.f1;_tmp2C0=_tmp2BE.f2;if(_tmp2C0)Cyc_Tcutil_check_bound(loc,
_tmp2BF,_tmp2AF);}else{if(Cyc_Tcutil_is_bound_one(_tmp2AF)?!((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2B0): 0)({void*_tmp2C1[0]={};Cyc_Tcutil_warn(
e1->loc,({const char*_tmp2C2="subscript applied to pointer to one object";
_tag_arr(_tmp2C2,sizeof(char),_get_zero_arr_size(_tmp2C2,43));}),_tag_arr(
_tmp2C1,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(loc,_tmp2AF);}}Cyc_Tcenv_check_rgn_accessible(
te,loc,_tmp2AE);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp2AB),(void*)1))({
void*_tmp2C3[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmp2C4="can't subscript an abstract pointer";
_tag_arr(_tmp2C4,sizeof(char),_get_zero_arr_size(_tmp2C4,36));}),_tag_arr(
_tmp2C3,sizeof(void*),0));});return _tmp2AB;_LL19B: if(_tmp2A9 <= (void*)3?1:*((int*)
_tmp2A9)!= 9)goto _LL19D;_tmp2B1=((struct Cyc_Absyn_TupleType_struct*)_tmp2A9)->f1;
_LL19C: return Cyc_Tcexp_ithTupleType(te,loc,_tmp2B1,e2);_LL19D:;_LL19E: return({
struct Cyc_String_pa_struct _tmp2C7;_tmp2C7.tag=0;_tmp2C7.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp2C5[1]={& _tmp2C7};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2C6="subscript applied to %s";_tag_arr(_tmp2C6,
sizeof(char),_get_zero_arr_size(_tmp2C6,24));}),_tag_arr(_tmp2C5,sizeof(void*),1));}});
_LL198:;}}}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp2C8=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp2C9;
_LL1A7: if(_tmp2C8 <= (void*)3?1:*((int*)_tmp2C8)!= 9)goto _LL1A9;_tmp2C9=((struct
Cyc_Absyn_TupleType_struct*)_tmp2C8)->f1;_LL1A8: if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp2C9)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
es))goto _LL1A6;for(0;es != 0;(es=es->tl,_tmp2C9=_tmp2C9->tl)){void*_tmp2CA=(*((
struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp2C9))->hd)).f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp2CA,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(te,(
struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple9*)_tmp2C9->hd)).f2);fields=({struct
Cyc_List_List*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));_tmp2CB->hd=({struct _tuple9*
_tmp2CC=_cycalloc(sizeof(*_tmp2CC));_tmp2CC->f1=(*((struct _tuple9*)_tmp2C9->hd)).f1;
_tmp2CC->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp2CC;});_tmp2CB->tl=fields;_tmp2CB;});}done=1;goto _LL1A6;_LL1A9:;_LL1AA: goto
_LL1A6;_LL1A6:;}if(!done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(te,0,(
struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*_tmp2CD=_cycalloc(
sizeof(*_tmp2CD));_tmp2CD->hd=({struct _tuple9*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));
_tmp2CE->f1=Cyc_Absyn_empty_tqual();_tmp2CE->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp2CE;});_tmp2CD->tl=
fields;_tmp2CD;});}return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp2CF=
_cycalloc(sizeof(*_tmp2CF));_tmp2CF[0]=({struct Cyc_Absyn_TupleType_struct _tmp2D0;
_tmp2D0.tag=9;_tmp2D0.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields);_tmp2D0;});_tmp2CF;});}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*t,struct Cyc_List_List*
des){return({void*_tmp2D1[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp2D2="tcCompoundLit";_tag_arr(_tmp2D2,sizeof(char),_get_zero_arr_size(_tmp2D2,
14));}),_tag_arr(_tmp2D1,sizeof(void*),0));});}static void*Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**elt_topt,int
zero_term,struct Cyc_List_List*des){struct Cyc_List_List*es=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple8*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp2ED=_cycalloc(
sizeof(*_tmp2ED));_tmp2ED->v=Cyc_Tcenv_lookup_type_vars(te);_tmp2ED;}));int
_tmp2D3=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);struct Cyc_Absyn_Const_e_struct*
_tmp2D4=({struct Cyc_Absyn_Const_e_struct*_tmp2E9=_cycalloc(sizeof(*_tmp2E9));
_tmp2E9[0]=({struct Cyc_Absyn_Const_e_struct _tmp2EA;_tmp2EA.tag=0;_tmp2EA.f1=(
void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp2EB=_cycalloc(sizeof(*_tmp2EB));
_tmp2EB[0]=({struct Cyc_Absyn_Int_c_struct _tmp2EC;_tmp2EC.tag=2;_tmp2EC.f1=(void*)((
void*)1);_tmp2EC.f2=_tmp2D3;_tmp2EC;});_tmp2EB;}));_tmp2EA;});_tmp2E9;});struct
Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp2D4,loc);if(zero_term){struct
Cyc_Absyn_Exp*_tmp2D5=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
es,_tmp2D3 - 1);if(!Cyc_Tcutil_is_zero(_tmp2D5))({void*_tmp2D6[0]={};Cyc_Tcutil_terr(
_tmp2D5->loc,({const char*_tmp2D7="zero-terminated array doesn't end with zero.";
_tag_arr(_tmp2D7,sizeof(char),_get_zero_arr_size(_tmp2D7,45));}),_tag_arr(
_tmp2D6,sizeof(void*),0));});}sz_exp->topt=({struct Cyc_Core_Opt*_tmp2D8=
_cycalloc(sizeof(*_tmp2D8));_tmp2D8->v=(void*)Cyc_Absyn_uint_typ;_tmp2D8;});{
void*res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(),(struct Cyc_Absyn_Exp*)
sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref);{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct
Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,es))({struct Cyc_String_pa_struct
_tmp2DB;_tmp2DB.tag=0;_tmp2DB.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
res));{void*_tmp2D9[1]={& _tmp2DB};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,({const char*_tmp2DA="elements of array do not all have the same type (%s)";
_tag_arr(_tmp2DA,sizeof(char),_get_zero_arr_size(_tmp2DA,53));}),_tag_arr(
_tmp2D9,sizeof(void*),1));}});{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple8*)des->hd)).f1;if(ds != 0){void*_tmp2DC=(
void*)ds->hd;struct Cyc_Absyn_Exp*_tmp2DD;_LL1AC: if(*((int*)_tmp2DC)!= 1)goto
_LL1AE;_LL1AD:({void*_tmp2DE[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp2DF="only array index designators are supported";
_tag_arr(_tmp2DF,sizeof(char),_get_zero_arr_size(_tmp2DF,43));}),_tag_arr(
_tmp2DE,sizeof(void*),0));});goto _LL1AB;_LL1AE: if(*((int*)_tmp2DC)!= 0)goto
_LL1AB;_tmp2DD=((struct Cyc_Absyn_ArrayElement_struct*)_tmp2DC)->f1;_LL1AF: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp2DD);{unsigned int _tmp2E1;int _tmp2E2;struct _tuple7 _tmp2E0=Cyc_Evexp_eval_const_uint_exp(
_tmp2DD);_tmp2E1=_tmp2E0.f1;_tmp2E2=_tmp2E0.f2;if(!_tmp2E2)({void*_tmp2E3[0]={};
Cyc_Tcutil_terr(_tmp2DD->loc,({const char*_tmp2E4="index designator cannot use sizeof or offsetof";
_tag_arr(_tmp2E4,sizeof(char),_get_zero_arr_size(_tmp2E4,47));}),_tag_arr(
_tmp2E3,sizeof(void*),0));});else{if(_tmp2E1 != offset)({struct Cyc_Int_pa_struct
_tmp2E8;_tmp2E8.tag=1;_tmp2E8.f1=(unsigned int)((int)_tmp2E1);{struct Cyc_Int_pa_struct
_tmp2E7;_tmp2E7.tag=1;_tmp2E7.f1=(unsigned int)offset;{void*_tmp2E5[2]={& _tmp2E7,&
_tmp2E8};Cyc_Tcutil_terr(_tmp2DD->loc,({const char*_tmp2E6="expecting index designator %d but found %d";
_tag_arr(_tmp2E6,sizeof(char),_get_zero_arr_size(_tmp2E6,43));}),_tag_arr(
_tmp2E5,sizeof(void*),2));}}});}goto _LL1AB;}_LL1AB:;}}}return res_t2;}}static void*
Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*
body,int*is_zero_term){Cyc_Tcexp_tcExp(te,0,bound);{void*_tmp2EE=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v);_LL1B1: if(_tmp2EE <= (
void*)3?1:*((int*)_tmp2EE)!= 17)goto _LL1B3;_LL1B2: goto _LL1B0;_LL1B3:;_LL1B4: if(!
Cyc_Tcutil_coerce_uint_typ(te,bound))({struct Cyc_String_pa_struct _tmp2F1;_tmp2F1.tag=
0;_tmp2F1.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(bound->topt))->v));{void*_tmp2EF[1]={& _tmp2F1};
Cyc_Tcutil_terr(bound->loc,({const char*_tmp2F0="expecting unsigned int, found %s";
_tag_arr(_tmp2F0,sizeof(char),_get_zero_arr_size(_tmp2F0,33));}),_tag_arr(
_tmp2EF,sizeof(void*),1));}});_LL1B0:;}if(!(vd->tq).real_const)({void*_tmp2F2[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp2F3="comprehension index variable is not declared const!";_tag_arr(_tmp2F3,
sizeof(char),_get_zero_arr_size(_tmp2F3,52));}),_tag_arr(_tmp2F2,sizeof(void*),0));});
if(te->le != 0){te=Cyc_Tcenv_new_block(loc,te);te=Cyc_Tcenv_add_local_var(loc,te,
vd);}{struct Cyc_Absyn_PtrInfo pinfo;void**_tmp2F4=0;struct Cyc_Absyn_Tqual*_tmp2F5=
0;struct Cyc_Absyn_Conref**_tmp2F6=0;if(topt != 0){void*_tmp2F7=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp2F8;struct Cyc_Absyn_ArrayInfo _tmp2F9;void*
_tmp2FA;void**_tmp2FB;struct Cyc_Absyn_Tqual _tmp2FC;struct Cyc_Absyn_Tqual*_tmp2FD;
struct Cyc_Absyn_Exp*_tmp2FE;struct Cyc_Absyn_Conref*_tmp2FF;struct Cyc_Absyn_Conref**
_tmp300;_LL1B6: if(_tmp2F7 <= (void*)3?1:*((int*)_tmp2F7)!= 4)goto _LL1B8;_tmp2F8=((
struct Cyc_Absyn_PointerType_struct*)_tmp2F7)->f1;_LL1B7: pinfo=_tmp2F8;_tmp2F4=(
void**)((void**)((void*)& pinfo.elt_typ));_tmp2F5=(struct Cyc_Absyn_Tqual*)& pinfo.elt_tq;
_tmp2F6=(struct Cyc_Absyn_Conref**)&(pinfo.ptr_atts).zero_term;goto _LL1B5;_LL1B8:
if(_tmp2F7 <= (void*)3?1:*((int*)_tmp2F7)!= 7)goto _LL1BA;_tmp2F9=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2F7)->f1;_tmp2FA=(void*)_tmp2F9.elt_type;_tmp2FB=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp2F7)->f1).elt_type;_tmp2FC=_tmp2F9.tq;_tmp2FD=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp2F7)->f1).tq;_tmp2FE=_tmp2F9.num_elts;
_tmp2FF=_tmp2F9.zero_term;_tmp300=(struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp2F7)->f1).zero_term;_LL1B9: _tmp2F4=(void**)_tmp2FB;_tmp2F5=(struct Cyc_Absyn_Tqual*)
_tmp2FD;_tmp2F6=(struct Cyc_Absyn_Conref**)_tmp300;goto _LL1B5;_LL1BA:;_LL1BB: goto
_LL1B5;_LL1B5:;}{void*t=Cyc_Tcexp_tcExp(te,_tmp2F4,body);if(te->le == 0){if(!Cyc_Tcutil_is_const_exp(
te,bound))({void*_tmp301[0]={};Cyc_Tcutil_terr(bound->loc,({const char*_tmp302="bound is not constant";
_tag_arr(_tmp302,sizeof(char),_get_zero_arr_size(_tmp302,22));}),_tag_arr(
_tmp301,sizeof(void*),0));});if(!Cyc_Tcutil_is_const_exp(te,body))({void*_tmp303[
0]={};Cyc_Tcutil_terr(bound->loc,({const char*_tmp304="body is not constant";
_tag_arr(_tmp304,sizeof(char),_get_zero_arr_size(_tmp304,21));}),_tag_arr(
_tmp303,sizeof(void*),0));});}if(_tmp2F6 != 0?((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,*_tmp2F6): 0){struct Cyc_Absyn_Exp*_tmp305=Cyc_Absyn_uint_exp(
1,0);_tmp305->topt=({struct Cyc_Core_Opt*_tmp306=_cycalloc(sizeof(*_tmp306));
_tmp306->v=(void*)Cyc_Absyn_uint_typ;_tmp306;});bound=Cyc_Absyn_add_exp(bound,
_tmp305,0);bound->topt=({struct Cyc_Core_Opt*_tmp307=_cycalloc(sizeof(*_tmp307));
_tmp307->v=(void*)Cyc_Absyn_uint_typ;_tmp307;});*is_zero_term=1;}return Cyc_Absyn_array_typ(
t,_tmp2F5 == 0?Cyc_Absyn_empty_tqual():*_tmp2F5,(struct Cyc_Absyn_Exp*)bound,
_tmp2F6 == 0?Cyc_Absyn_false_conref:*_tmp2F6);}}}struct _tuple10{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp308;_push_handler(& _tmp308);{int _tmp30A=0;if(setjmp(
_tmp308.handler))_tmp30A=1;if(!_tmp30A){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp309=(void*)_exn_thrown;void*_tmp30C=_tmp309;_LL1BD:
if(_tmp30C != Cyc_Dict_Absent)goto _LL1BF;_LL1BE:({struct Cyc_String_pa_struct
_tmp30F;_tmp30F.tag=0;_tmp30F.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(*
tn));{void*_tmp30D[1]={& _tmp30F};Cyc_Tcutil_terr(loc,({const char*_tmp30E="unbound struct name %s";
_tag_arr(_tmp30E,sizeof(char),_get_zero_arr_size(_tmp30E,23));}),_tag_arr(
_tmp30D,sizeof(void*),1));}});return topt != 0?*topt:(void*)0;_LL1BF:;_LL1C0:(void)
_throw(_tmp30C);_LL1BC:;}}}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}
if((void*)ad->kind == (void*)1)({void*_tmp310[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp311="expecting struct but found union";_tag_arr(_tmp311,sizeof(char),
_get_zero_arr_size(_tmp311,33));}),_tag_arr(_tmp310,sizeof(void*),0));});if(ad->impl
== 0){({void*_tmp312[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp313="can't build abstract struct";
_tag_arr(_tmp313,sizeof(char),_get_zero_arr_size(_tmp313,28));}),_tag_arr(
_tmp312,sizeof(void*),0));});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp314;}));}{struct _RegionHandle _tmp315=_new_region("rgn");struct _RegionHandle*
rgn=& _tmp315;_push_region(rgn);{struct _tuple4 _tmp316=({struct _tuple4 _tmp337;
_tmp337.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp337.f2=rgn;_tmp337;});struct Cyc_List_List*
_tmp317=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp316,ad->tvs);struct Cyc_List_List*_tmp318=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp316,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars);struct Cyc_List_List*_tmp319=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp317);struct Cyc_List_List*_tmp31A=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp318);struct Cyc_List_List*_tmp31B=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp317,_tmp318);struct Cyc_Absyn_AggrType_struct*_tmp31C=({struct Cyc_Absyn_AggrType_struct*
_tmp331=_cycalloc(sizeof(*_tmp331));_tmp331[0]=({struct Cyc_Absyn_AggrType_struct
_tmp332;_tmp332.tag=10;_tmp332.f1=({struct Cyc_Absyn_AggrInfo _tmp333;_tmp333.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp334=_cycalloc(sizeof(*
_tmp334));_tmp334[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp335;_tmp335.tag=1;
_tmp335.f1=({struct Cyc_Absyn_Aggrdecl**_tmp336=_cycalloc(sizeof(*_tmp336));
_tmp336[0]=ad;_tmp336;});_tmp335;});_tmp334;}));_tmp333.targs=_tmp319;_tmp333;});
_tmp332;});_tmp331;});struct Cyc_List_List*_tmp31D=*ts;struct Cyc_List_List*
_tmp31E=_tmp31A;while(_tmp31D != 0?_tmp31E != 0: 0){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp31D->hd);Cyc_Tcutil_unify((
void*)_tmp31D->hd,(void*)_tmp31E->hd);_tmp31D=_tmp31D->tl;_tmp31E=_tmp31E->tl;}
if(_tmp31D != 0)({void*_tmp31F[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp320="too many explicit witness types";
_tag_arr(_tmp320,sizeof(char),_get_zero_arr_size(_tmp320,32));}),_tag_arr(
_tmp31F,sizeof(void*),0));});*ts=_tmp31A;if(topt != 0)Cyc_Tcutil_unify((void*)
_tmp31C,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);for(0;fields != 0;
fields=fields->tl){struct _tuple10 _tmp322;struct Cyc_Absyn_Aggrfield*_tmp323;
struct Cyc_Absyn_Exp*_tmp324;struct _tuple10*_tmp321=(struct _tuple10*)fields->hd;
_tmp322=*_tmp321;_tmp323=_tmp322.f1;_tmp324=_tmp322.f2;{void*_tmp325=Cyc_Tcutil_rsubstitute(
rgn,_tmp31B,(void*)_tmp323->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp325,
_tmp324);if(!Cyc_Tcutil_coerce_arg(te,_tmp324,_tmp325)){({struct Cyc_String_pa_struct
_tmp32B;_tmp32B.tag=0;_tmp32B.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp324->topt))->v));{struct Cyc_String_pa_struct
_tmp32A;_tmp32A.tag=0;_tmp32A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp325));{struct Cyc_String_pa_struct _tmp329;_tmp329.tag=0;_tmp329.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(*tn));{struct Cyc_String_pa_struct
_tmp328;_tmp328.tag=0;_tmp328.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp323->name);{
void*_tmp326[4]={& _tmp328,& _tmp329,& _tmp32A,& _tmp32B};Cyc_Tcutil_terr(_tmp324->loc,({
const char*_tmp327="field %s of struct %s expects type %s != %s";_tag_arr(_tmp327,
sizeof(char),_get_zero_arr_size(_tmp327,44));}),_tag_arr(_tmp326,sizeof(void*),4));}}}}});
Cyc_Tcutil_explain_failure();}}}{struct Cyc_List_List*_tmp32C=0;{struct Cyc_List_List*
_tmp32D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp32D != 0;_tmp32D=_tmp32D->tl){_tmp32C=({struct Cyc_List_List*_tmp32E=_cycalloc(
sizeof(*_tmp32E));_tmp32E->hd=({struct _tuple0*_tmp32F=_cycalloc(sizeof(*_tmp32F));
_tmp32F->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp31B,(*((struct _tuple0*)_tmp32D->hd)).f1);
_tmp32F->f2=Cyc_Tcutil_rsubstitute(rgn,_tmp31B,(*((struct _tuple0*)_tmp32D->hd)).f2);
_tmp32F;});_tmp32E->tl=_tmp32C;_tmp32E;});}}_tmp32C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp32C);Cyc_Tcenv_check_rgn_partial_order(
te,loc,_tmp32C);{void*_tmp330=(void*)_tmp31C;_npop_handler(0);return _tmp330;}}}};
_pop_region(rgn);}}static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*args){{struct
_RegionHandle _tmp338=_new_region("rgn");struct _RegionHandle*rgn=& _tmp338;
_push_region(rgn);{void*_tmp339=Cyc_Tcutil_compress(ts);void*_tmp33A;struct Cyc_List_List*
_tmp33B;_LL1C2: if(_tmp339 <= (void*)3?1:*((int*)_tmp339)!= 11)goto _LL1C4;_tmp33A=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp339)->f1;_tmp33B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp339)->f2;_LL1C3: if(_tmp33A == (void*)1)({void*_tmp33C[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp33D="expecting struct but found union";_tag_arr(_tmp33D,
sizeof(char),_get_zero_arr_size(_tmp33D,33));}),_tag_arr(_tmp33C,sizeof(void*),0));});{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,_tmp33B);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp33F;struct Cyc_Absyn_Aggrfield*
_tmp340;struct Cyc_Absyn_Exp*_tmp341;struct _tuple10*_tmp33E=(struct _tuple10*)
fields->hd;_tmp33F=*_tmp33E;_tmp340=_tmp33F.f1;_tmp341=_tmp33F.f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp340->type)),_tmp341);if(!Cyc_Tcutil_coerce_arg(
te,_tmp341,(void*)_tmp340->type)){({struct Cyc_String_pa_struct _tmp346;_tmp346.tag=
0;_tmp346.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp341->topt))->v));{struct Cyc_String_pa_struct
_tmp345;_tmp345.tag=0;_tmp345.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)_tmp340->type));{struct Cyc_String_pa_struct _tmp344;_tmp344.tag=0;_tmp344.f1=(
struct _tagged_arr)((struct _tagged_arr)*_tmp340->name);{void*_tmp342[3]={& _tmp344,&
_tmp345,& _tmp346};Cyc_Tcutil_terr(_tmp341->loc,({const char*_tmp343="field %s of struct expects type %s != %s";
_tag_arr(_tmp343,sizeof(char),_get_zero_arr_size(_tmp343,41));}),_tag_arr(
_tmp342,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}goto _LL1C1;}
_LL1C4:;_LL1C5:({void*_tmp347[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp348="tcAnonStruct: wrong type";
_tag_arr(_tmp348,sizeof(char),_get_zero_arr_size(_tmp348,25));}),_tag_arr(
_tmp347,sizeof(void*),0));});_LL1C1:;};_pop_region(rgn);}return ts;}static void*
Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*tud,
struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp349=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp349;_push_region(rgn);{struct _tuple4 _tmp34A=({
struct _tuple4 _tmp36D;_tmp36D.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp36D.f2=rgn;
_tmp36D;});struct Cyc_List_List*_tmp34B=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp34A,tud->tvs);struct Cyc_List_List*_tmp34C=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp34B);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp368=_cycalloc(sizeof(*_tmp368));_tmp368[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp369;_tmp369.tag=3;_tmp369.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp36A;
_tmp36A.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp36B=_cycalloc(sizeof(*_tmp36B));_tmp36B[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp36C;_tmp36C.tag=1;_tmp36C.f1=tud;_tmp36C.f2=tuf;_tmp36C;});_tmp36B;}));
_tmp36A.targs=_tmp34C;_tmp36A;});_tmp369;});_tmp368;});if(topt != 0){void*_tmp34D=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo _tmp34E;struct Cyc_List_List*
_tmp34F;void*_tmp350;_LL1C7: if(_tmp34D <= (void*)3?1:*((int*)_tmp34D)!= 3)goto
_LL1C9;_LL1C8: Cyc_Tcutil_unify(*topt,res);goto _LL1C6;_LL1C9: if(_tmp34D <= (void*)
3?1:*((int*)_tmp34D)!= 2)goto _LL1CB;_tmp34E=((struct Cyc_Absyn_TunionType_struct*)
_tmp34D)->f1;_tmp34F=_tmp34E.targs;_tmp350=(void*)_tmp34E.rgn;_LL1CA:{struct Cyc_List_List*
a=_tmp34C;for(0;a != 0?_tmp34F != 0: 0;(a=a->tl,_tmp34F=_tmp34F->tl)){Cyc_Tcutil_unify((
void*)a->hd,(void*)_tmp34F->hd);}}if(tuf->typs == 0?es == 0: 0){e->topt=({struct Cyc_Core_Opt*
_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351->v=(void*)res;_tmp351;});res=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352[0]=({
struct Cyc_Absyn_TunionType_struct _tmp353;_tmp353.tag=2;_tmp353.f1=({struct Cyc_Absyn_TunionInfo
_tmp354;_tmp354.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp356;_tmp356.tag=1;_tmp356.f1=({struct Cyc_Absyn_Tuniondecl**_tmp357=_cycalloc(
sizeof(*_tmp357));_tmp357[0]=tud;_tmp357;});_tmp356;});_tmp355;}));_tmp354.targs=
_tmp34C;_tmp354.rgn=(void*)_tmp350;_tmp354;});_tmp353;});_tmp352;});Cyc_Tcutil_unchecked_cast(
te,e,res,(void*)1);{void*_tmp358=res;_npop_handler(0);return _tmp358;}}goto _LL1C6;
_LL1CB:;_LL1CC: goto _LL1C6;_LL1C6:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 
0?ts != 0: 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp34B,(*((struct _tuple9*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_String_pa_struct
_tmp35D;_tmp35D.tag=0;_tmp35D.f1=(struct _tagged_arr)((struct _tagged_arr)(e->topt
== 0?(struct _tagged_arr)({const char*_tmp35E="?";_tag_arr(_tmp35E,sizeof(char),
_get_zero_arr_size(_tmp35E,2));}): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)));{struct Cyc_String_pa_struct _tmp35C;_tmp35C.tag=0;
_tmp35C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
struct Cyc_String_pa_struct _tmp35B;_tmp35B.tag=0;_tmp35B.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name));{void*_tmp359[3]={& _tmp35B,&
_tmp35C,& _tmp35D};Cyc_Tcutil_terr(e->loc,({const char*_tmp35A="tunion constructor %s expects argument of type %s but this argument has type %s";
_tag_arr(_tmp35A,sizeof(char),_get_zero_arr_size(_tmp35A,80));}),_tag_arr(
_tmp359,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}if(es != 0){void*
_tmp362=({struct Cyc_String_pa_struct _tmp361;_tmp361.tag=0;_tmp361.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name));{void*
_tmp35F[1]={& _tmp361};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,({const char*_tmp360="too many arguments for tunion constructor %s";_tag_arr(
_tmp360,sizeof(char),_get_zero_arr_size(_tmp360,45));}),_tag_arr(_tmp35F,sizeof(
void*),1));}});_npop_handler(0);return _tmp362;}if(ts != 0){void*_tmp366=({struct
Cyc_String_pa_struct _tmp365;_tmp365.tag=0;_tmp365.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(tuf->name));{void*_tmp363[1]={& _tmp365};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp364="too few arguments for tunion constructor %s";
_tag_arr(_tmp364,sizeof(char),_get_zero_arr_size(_tmp364,44));}),_tag_arr(
_tmp363,sizeof(void*),1));}});_npop_handler(0);return _tmp366;}{void*_tmp367=res;
_npop_handler(0);return _tmp367;}}};_pop_region(rgn);}static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp36E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp36F;
struct Cyc_Absyn_PtrAtts _tmp370;struct Cyc_Absyn_Conref*_tmp371;struct Cyc_Absyn_ArrayInfo
_tmp372;void*_tmp373;struct Cyc_List_List*_tmp374;struct Cyc_Absyn_AggrInfo _tmp375;
void*_tmp376;struct Cyc_List_List*_tmp377;struct Cyc_List_List*_tmp378;_LL1CE: if((
int)_tmp36E != 0)goto _LL1D0;_LL1CF: return 1;_LL1D0: if(_tmp36E <= (void*)3?1:*((int*)
_tmp36E)!= 0)goto _LL1D2;_LL1D1: goto _LL1D3;_LL1D2: if(_tmp36E <= (void*)3?1:*((int*)
_tmp36E)!= 1)goto _LL1D4;_LL1D3: goto _LL1D5;_LL1D4: if(_tmp36E <= (void*)3?1:*((int*)
_tmp36E)!= 2)goto _LL1D6;_LL1D5: goto _LL1D7;_LL1D6: if(_tmp36E <= (void*)3?1:*((int*)
_tmp36E)!= 3)goto _LL1D8;_LL1D7: return 0;_LL1D8: if(_tmp36E <= (void*)3?1:*((int*)
_tmp36E)!= 4)goto _LL1DA;_tmp36F=((struct Cyc_Absyn_PointerType_struct*)_tmp36E)->f1;
_tmp370=_tmp36F.ptr_atts;_tmp371=_tmp370.nullable;_LL1D9: return((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp371);_LL1DA: if(_tmp36E <= (
void*)3?1:*((int*)_tmp36E)!= 5)goto _LL1DC;_LL1DB: goto _LL1DD;_LL1DC: if((int)
_tmp36E != 1)goto _LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if(_tmp36E <= (void*)3?1:*((int*)
_tmp36E)!= 6)goto _LL1E0;_LL1DF: return 1;_LL1E0: if(_tmp36E <= (void*)3?1:*((int*)
_tmp36E)!= 7)goto _LL1E2;_tmp372=((struct Cyc_Absyn_ArrayType_struct*)_tmp36E)->f1;
_tmp373=(void*)_tmp372.elt_type;_LL1E1: return Cyc_Tcexp_zeroable_type(_tmp373);
_LL1E2: if(_tmp36E <= (void*)3?1:*((int*)_tmp36E)!= 8)goto _LL1E4;_LL1E3: return 0;
_LL1E4: if(_tmp36E <= (void*)3?1:*((int*)_tmp36E)!= 9)goto _LL1E6;_tmp374=((struct
Cyc_Absyn_TupleType_struct*)_tmp36E)->f1;_LL1E5: for(0;(unsigned int)_tmp374;
_tmp374=_tmp374->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)_tmp374->hd)).f2))
return 0;}return 1;_LL1E6: if(_tmp36E <= (void*)3?1:*((int*)_tmp36E)!= 10)goto _LL1E8;
_tmp375=((struct Cyc_Absyn_AggrType_struct*)_tmp36E)->f1;_tmp376=(void*)_tmp375.aggr_info;
_tmp377=_tmp375.targs;_LL1E7: {struct Cyc_Absyn_Aggrdecl*_tmp379=Cyc_Absyn_get_known_aggrdecl(
_tmp376);if(_tmp379->impl == 0?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp379->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp37A=_new_region("r");
struct _RegionHandle*r=& _tmp37A;_push_region(r);{struct Cyc_List_List*_tmp37B=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp379->tvs,_tmp377);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp379->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp37B,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp37C=0;_npop_handler(0);
return _tmp37C;}}}{int _tmp37D=1;_npop_handler(0);return _tmp37D;}};_pop_region(r);}}
_LL1E8: if(_tmp36E <= (void*)3?1:*((int*)_tmp36E)!= 12)goto _LL1EA;_LL1E9: return 1;
_LL1EA: if(_tmp36E <= (void*)3?1:*((int*)_tmp36E)!= 14)goto _LL1EC;_LL1EB: return 1;
_LL1EC: if(_tmp36E <= (void*)3?1:*((int*)_tmp36E)!= 17)goto _LL1EE;_LL1ED: return 1;
_LL1EE: if(_tmp36E <= (void*)3?1:*((int*)_tmp36E)!= 11)goto _LL1F0;_tmp378=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp36E)->f2;_LL1EF: for(0;_tmp378 != 0;_tmp378=
_tmp378->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp378->hd)->type))return 0;}return 1;_LL1F0: if(_tmp36E <= (void*)3?1:*((int*)
_tmp36E)!= 13)goto _LL1F2;_LL1F1: return 1;_LL1F2: if(_tmp36E <= (void*)3?1:*((int*)
_tmp36E)!= 16)goto _LL1F4;_LL1F3: return 0;_LL1F4: if(_tmp36E <= (void*)3?1:*((int*)
_tmp36E)!= 15)goto _LL1F6;_LL1F5: return 0;_LL1F6: if(_tmp36E <= (void*)3?1:*((int*)
_tmp36E)!= 18)goto _LL1F8;_LL1F7: goto _LL1F9;_LL1F8: if((int)_tmp36E != 2)goto _LL1FA;
_LL1F9: goto _LL1FB;_LL1FA: if(_tmp36E <= (void*)3?1:*((int*)_tmp36E)!= 19)goto
_LL1FC;_LL1FB: goto _LL1FD;_LL1FC: if(_tmp36E <= (void*)3?1:*((int*)_tmp36E)!= 20)
goto _LL1FE;_LL1FD: goto _LL1FF;_LL1FE: if(_tmp36E <= (void*)3?1:*((int*)_tmp36E)!= 
21)goto _LL1CD;_LL1FF:({struct Cyc_String_pa_struct _tmp380;_tmp380.tag=0;_tmp380.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp37E[1]={&
_tmp380};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp37F="bad type `%s' in zeroable type";_tag_arr(_tmp37F,sizeof(char),
_get_zero_arr_size(_tmp37F,31));}),_tag_arr(_tmp37E,sizeof(void*),1));}});_LL1CD:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t)?1:(allow_zero?Cyc_Tcexp_zeroable_type(
t): 0))return;if(topt != 0){void*_tmp381=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp382;void*_tmp383;_LL201: if(_tmp381 <= (void*)3?1:*((int*)_tmp381)!= 4)goto
_LL203;_tmp382=((struct Cyc_Absyn_PointerType_struct*)_tmp381)->f1;_tmp383=(void*)
_tmp382.elt_typ;_LL202: Cyc_Tcutil_unify(_tmp383,t);if(Cyc_Tcutil_bits_only(t)?1:(
allow_zero?Cyc_Tcexp_zeroable_type(t): 0))return;goto _LL200;_LL203:;_LL204: goto
_LL200;_LL200:;}({struct Cyc_String_pa_struct _tmp387;_tmp387.tag=0;_tmp387.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp386;_tmp386.tag=0;_tmp386.f1=(struct _tagged_arr)((struct _tagged_arr)(
allow_zero?(struct _tagged_arr)({const char*_tmp388="calloc";_tag_arr(_tmp388,
sizeof(char),_get_zero_arr_size(_tmp388,7));}):(struct _tagged_arr)({const char*
_tmp389="malloc";_tag_arr(_tmp389,sizeof(char),_get_zero_arr_size(_tmp389,7));})));{
void*_tmp384[2]={& _tmp386,& _tmp387};Cyc_Tcutil_terr(loc,({const char*_tmp385="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_arr(_tmp385,sizeof(char),_get_zero_arr_size(_tmp385,60));}),_tag_arr(
_tmp384,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,
struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;if(ropt != 0){
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp38F=
_cycalloc(sizeof(*_tmp38F));_tmp38F[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp390;_tmp390.tag=15;_tmp390.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp391=_cycalloc(sizeof(*_tmp391));_tmp391->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp391;}));_tmp390;});_tmp38F;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp38A=Cyc_Tcutil_compress(handle_type);void*_tmp38B;_LL206: if(_tmp38A <= (
void*)3?1:*((int*)_tmp38A)!= 15)goto _LL208;_tmp38B=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp38A)->f1;_LL207: rgn=_tmp38B;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto
_LL205;_LL208:;_LL209:({struct Cyc_String_pa_struct _tmp38E;_tmp38E.tag=0;_tmp38E.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(handle_type));{void*
_tmp38C[1]={& _tmp38E};Cyc_Tcutil_terr(ropt->loc,({const char*_tmp38D="expecting region_t type but found %s";
_tag_arr(_tmp38D,sizeof(char),_get_zero_arr_size(_tmp38D,37));}),_tag_arr(
_tmp38C,sizeof(void*),1));}});goto _LL205;_LL205:;}Cyc_Tcexp_tcExp(te,(void**)&
Cyc_Absyn_uint_typ,*e);{void*elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;
if(*is_calloc){if(*t == 0)({void*_tmp392[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp393="calloc with empty type";
_tag_arr(_tmp393,sizeof(char),_get_zero_arr_size(_tmp393,23));}),_tag_arr(
_tmp392,sizeof(void*),0));});elt_type=*((void**)_check_null(*t));Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(
1,loc,topt,elt_type);num_elts=*e;one_elt=0;}else{void*_tmp394=(void*)(*e)->r;
void*_tmp395;void*_tmp396;struct Cyc_List_List*_tmp397;struct Cyc_List_List _tmp398;
struct Cyc_Absyn_Exp*_tmp399;struct Cyc_List_List*_tmp39A;struct Cyc_List_List
_tmp39B;struct Cyc_Absyn_Exp*_tmp39C;struct Cyc_List_List*_tmp39D;_LL20B: if(*((int*)
_tmp394)!= 16)goto _LL20D;_tmp395=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp394)->f1;_LL20C: elt_type=_tmp395;*t=(void**)({void**_tmp39E=_cycalloc(
sizeof(*_tmp39E));_tmp39E[0]=elt_type;_tmp39E;});num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL20A;
_LL20D: if(*((int*)_tmp394)!= 3)goto _LL20F;_tmp396=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp394)->f1;if((int)_tmp396 != 1)goto _LL20F;_tmp397=((struct Cyc_Absyn_Primop_e_struct*)
_tmp394)->f2;if(_tmp397 == 0)goto _LL20F;_tmp398=*_tmp397;_tmp399=(struct Cyc_Absyn_Exp*)
_tmp398.hd;_tmp39A=_tmp398.tl;if(_tmp39A == 0)goto _LL20F;_tmp39B=*_tmp39A;_tmp39C=(
struct Cyc_Absyn_Exp*)_tmp39B.hd;_tmp39D=_tmp39B.tl;if(_tmp39D != 0)goto _LL20F;
_LL20E:{struct _tuple0 _tmp3A0=({struct _tuple0 _tmp39F;_tmp39F.f1=(void*)_tmp399->r;
_tmp39F.f2=(void*)_tmp39C->r;_tmp39F;});void*_tmp3A1;void*_tmp3A2;void*_tmp3A3;
void*_tmp3A4;_LL212: _tmp3A1=_tmp3A0.f1;if(*((int*)_tmp3A1)!= 16)goto _LL214;
_tmp3A2=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3A1)->f1;_LL213: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3A2);elt_type=_tmp3A2;*t=(void**)({void**_tmp3A5=_cycalloc(
sizeof(*_tmp3A5));_tmp3A5[0]=elt_type;_tmp3A5;});num_elts=_tmp39C;one_elt=0;goto
_LL211;_LL214: _tmp3A3=_tmp3A0.f2;if(*((int*)_tmp3A3)!= 16)goto _LL216;_tmp3A4=(
void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3A3)->f1;_LL215: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3A4);elt_type=_tmp3A4;*t=(void**)({void**_tmp3A6=_cycalloc(
sizeof(*_tmp3A6));_tmp3A6[0]=elt_type;_tmp3A6;});num_elts=_tmp399;one_elt=0;goto
_LL211;_LL216:;_LL217: goto No_sizeof;_LL211:;}goto _LL20A;_LL20F:;_LL210: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp3A7=_cycalloc(sizeof(*_tmp3A7));
_tmp3A7[0]=elt_type;_tmp3A7;});num_elts=*e;one_elt=0;goto _LL20A;_LL20A:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp3A8=elt_type;struct Cyc_Absyn_AggrInfo _tmp3A9;
void*_tmp3AA;struct Cyc_Absyn_Aggrdecl**_tmp3AB;struct Cyc_Absyn_Aggrdecl*_tmp3AC;
_LL219: if(_tmp3A8 <= (void*)3?1:*((int*)_tmp3A8)!= 10)goto _LL21B;_tmp3A9=((struct
Cyc_Absyn_AggrType_struct*)_tmp3A8)->f1;_tmp3AA=(void*)_tmp3A9.aggr_info;if(*((
int*)_tmp3AA)!= 1)goto _LL21B;_tmp3AB=((struct Cyc_Absyn_KnownAggr_struct*)_tmp3AA)->f1;
_tmp3AC=*_tmp3AB;_LL21A: if(_tmp3AC->impl != 0?((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3AC->impl))->exist_vars != 0: 0)({void*_tmp3AD[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp3AE="malloc with existential types not yet implemented";
_tag_arr(_tmp3AE,sizeof(char),_get_zero_arr_size(_tmp3AE,50));}),_tag_arr(
_tmp3AD,sizeof(void*),0));});goto _LL218;_LL21B:;_LL21C: goto _LL218;_LL218:;}{void*(*
_tmp3AF)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term)=Cyc_Absyn_at_typ;struct Cyc_Absyn_Conref*_tmp3B0=Cyc_Absyn_false_conref;
if(topt != 0){void*_tmp3B1=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp3B2;struct Cyc_Absyn_PtrAtts _tmp3B3;struct Cyc_Absyn_Conref*_tmp3B4;struct Cyc_Absyn_Conref*
_tmp3B5;_LL21E: if(_tmp3B1 <= (void*)3?1:*((int*)_tmp3B1)!= 4)goto _LL220;_tmp3B2=((
struct Cyc_Absyn_PointerType_struct*)_tmp3B1)->f1;_tmp3B3=_tmp3B2.ptr_atts;
_tmp3B4=_tmp3B3.nullable;_tmp3B5=_tmp3B3.zero_term;_LL21F: _tmp3B0=_tmp3B5;if(((
int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp3B4))_tmp3AF=Cyc_Absyn_star_typ;
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp3B5)?!(*
is_calloc): 0){({void*_tmp3B6[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp3B7="converting malloc to calloc to ensure zero-termination";
_tag_arr(_tmp3B7,sizeof(char),_get_zero_arr_size(_tmp3B7,55));}),_tag_arr(
_tmp3B6,sizeof(void*),0));});*is_calloc=1;}goto _LL21D;_LL220:;_LL221: goto _LL21D;
_LL21D:;}if(!one_elt)_tmp3AF=Cyc_Absyn_tagged_typ;return _tmp3AF(elt_type,rgn,Cyc_Absyn_empty_tqual(),
_tmp3B0);}}}static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
te,s),s,1);Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));while(1){
void*_tmp3B8=(void*)s->r;struct Cyc_Absyn_Exp*_tmp3B9;struct Cyc_Absyn_Stmt*
_tmp3BA;struct Cyc_Absyn_Stmt*_tmp3BB;struct Cyc_Absyn_Decl*_tmp3BC;struct Cyc_Absyn_Stmt*
_tmp3BD;_LL223: if(_tmp3B8 <= (void*)1?1:*((int*)_tmp3B8)!= 0)goto _LL225;_tmp3B9=((
struct Cyc_Absyn_Exp_s_struct*)_tmp3B8)->f1;_LL224: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3B9->topt))->v;_LL225: if(_tmp3B8 <= (void*)1?1:*((int*)_tmp3B8)!= 
1)goto _LL227;_tmp3BA=((struct Cyc_Absyn_Seq_s_struct*)_tmp3B8)->f1;_tmp3BB=((
struct Cyc_Absyn_Seq_s_struct*)_tmp3B8)->f2;_LL226: s=_tmp3BB;continue;_LL227: if(
_tmp3B8 <= (void*)1?1:*((int*)_tmp3B8)!= 12)goto _LL229;_tmp3BC=((struct Cyc_Absyn_Decl_s_struct*)
_tmp3B8)->f1;_tmp3BD=((struct Cyc_Absyn_Decl_s_struct*)_tmp3B8)->f2;_LL228: s=
_tmp3BD;continue;_LL229:;_LL22A: return({void*_tmp3BE[0]={};Cyc_Tcexp_expr_err(te,
loc,topt,({const char*_tmp3BF="statement expression must end with expression";
_tag_arr(_tmp3BF,sizeof(char),_get_zero_arr_size(_tmp3BF,46));}),_tag_arr(
_tmp3BE,sizeof(void*),0));});_LL222:;}}static void*Cyc_Tcexp_tcCodegen(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Fndecl*fd){return({
void*_tmp3C0[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp3C1="tcCodegen";
_tag_arr(_tmp3C1,sizeof(char),_get_zero_arr_size(_tmp3C1,10));}),_tag_arr(
_tmp3C0,sizeof(void*),0));});}static void*Cyc_Tcexp_tcFill(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){return({void*
_tmp3C2[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp3C3="tcFill";
_tag_arr(_tmp3C3,sizeof(char),_get_zero_arr_size(_tmp3C3,7));}),_tag_arr(_tmp3C2,
sizeof(void*),0));});}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)2;if(rgn_handle != 0){void*
expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp3C9=_cycalloc(
sizeof(*_tmp3C9));_tmp3C9[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp3CA;
_tmp3CA.tag=15;_tmp3CA.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));_tmp3CB->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp3CB;}));_tmp3CA;});_tmp3C9;});void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)rgn_handle);void*_tmp3C4=Cyc_Tcutil_compress(
handle_type);void*_tmp3C5;_LL22C: if(_tmp3C4 <= (void*)3?1:*((int*)_tmp3C4)!= 15)
goto _LL22E;_tmp3C5=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp3C4)->f1;
_LL22D: rgn=_tmp3C5;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL22B;_LL22E:;
_LL22F:({struct Cyc_String_pa_struct _tmp3C8;_tmp3C8.tag=0;_tmp3C8.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(handle_type));{void*
_tmp3C6[1]={& _tmp3C8};Cyc_Tcutil_terr(rgn_handle->loc,({const char*_tmp3C7="expecting region_t type but found %s";
_tag_arr(_tmp3C7,sizeof(char),_get_zero_arr_size(_tmp3C7,37));}),_tag_arr(
_tmp3C6,sizeof(void*),1));}});goto _LL22B;_LL22B:;}{void*_tmp3CC=(void*)e1->r;
struct Cyc_Core_Opt*_tmp3CD;struct Cyc_List_List*_tmp3CE;struct Cyc_List_List*
_tmp3CF;void*_tmp3D0;struct _tagged_arr _tmp3D1;_LL231: if(*((int*)_tmp3CC)!= 27)
goto _LL233;_LL232: {void*_tmp3D2=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);void*
_tmp3D3=Cyc_Tcutil_compress(_tmp3D2);struct Cyc_Absyn_ArrayInfo _tmp3D4;void*
_tmp3D5;struct Cyc_Absyn_Tqual _tmp3D6;struct Cyc_Absyn_Exp*_tmp3D7;struct Cyc_Absyn_Conref*
_tmp3D8;_LL23C: if(_tmp3D3 <= (void*)3?1:*((int*)_tmp3D3)!= 7)goto _LL23E;_tmp3D4=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3D3)->f1;_tmp3D5=(void*)_tmp3D4.elt_type;
_tmp3D6=_tmp3D4.tq;_tmp3D7=_tmp3D4.num_elts;_tmp3D8=_tmp3D4.zero_term;_LL23D: {
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp3D7);void*b;{void*
_tmp3D9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);
void*_tmp3DA;_LL241: if(_tmp3D9 <= (void*)3?1:*((int*)_tmp3D9)!= 17)goto _LL243;
_tmp3DA=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp3D9)->f1;_LL242: b=(void*)({
struct Cyc_Absyn_AbsUpper_b_struct*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB[0]=({
struct Cyc_Absyn_AbsUpper_b_struct _tmp3DC;_tmp3DC.tag=1;_tmp3DC.f1=(void*)_tmp3DA;
_tmp3DC;});_tmp3DB;});goto _LL240;_LL243:;_LL244: if(Cyc_Tcutil_is_const_exp(te,
bnd))b=(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));
_tmp3DD[0]=({struct Cyc_Absyn_Upper_b_struct _tmp3DE;_tmp3DE.tag=0;_tmp3DE.f1=bnd;
_tmp3DE;});_tmp3DD;});else{b=(void*)0;}_LL240:;}{void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp3E0=_cycalloc(sizeof(*_tmp3E0));_tmp3E0[0]=({struct Cyc_Absyn_PointerType_struct
_tmp3E1;_tmp3E1.tag=4;_tmp3E1.f1=({struct Cyc_Absyn_PtrInfo _tmp3E2;_tmp3E2.elt_typ=(
void*)_tmp3D5;_tmp3E2.elt_tq=_tmp3D6;_tmp3E2.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp3E3;_tmp3E3.rgn=(void*)rgn;_tmp3E3.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp3E3.bounds=Cyc_Absyn_new_conref(b);_tmp3E3.zero_term=
_tmp3D8;_tmp3E3;});_tmp3E2;});_tmp3E1;});_tmp3E0;});if(topt != 0){if(!Cyc_Tcutil_unify(*
topt,res_typ)?Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt): 0){e->topt=({
struct Cyc_Core_Opt*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF->v=(void*)res_typ;
_tmp3DF;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*topt;}}return
res_typ;}}_LL23E:;_LL23F:({void*_tmp3E4[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp3E5="tcNew: comprehension returned non-array type";
_tag_arr(_tmp3E5,sizeof(char),_get_zero_arr_size(_tmp3E5,45));}),_tag_arr(
_tmp3E4,sizeof(void*),0));});_LL23B:;}_LL233: if(*((int*)_tmp3CC)!= 34)goto _LL235;
_tmp3CD=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp3CC)->f1;_tmp3CE=((struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp3CC)->f2;_LL234:(void*)(e1->r=(void*)((
void*)({struct Cyc_Absyn_Array_e_struct*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));
_tmp3E6[0]=({struct Cyc_Absyn_Array_e_struct _tmp3E7;_tmp3E7.tag=26;_tmp3E7.f1=
_tmp3CE;_tmp3E7;});_tmp3E6;})));_tmp3CF=_tmp3CE;goto _LL236;_LL235: if(*((int*)
_tmp3CC)!= 26)goto _LL237;_tmp3CF=((struct Cyc_Absyn_Array_e_struct*)_tmp3CC)->f1;
_LL236: {void**elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp3E8=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3E9;void*_tmp3EA;void**_tmp3EB;struct Cyc_Absyn_Tqual
_tmp3EC;struct Cyc_Absyn_PtrAtts _tmp3ED;struct Cyc_Absyn_Conref*_tmp3EE;_LL246: if(
_tmp3E8 <= (void*)3?1:*((int*)_tmp3E8)!= 4)goto _LL248;_tmp3E9=((struct Cyc_Absyn_PointerType_struct*)
_tmp3E8)->f1;_tmp3EA=(void*)_tmp3E9.elt_typ;_tmp3EB=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp3E8)->f1).elt_typ;_tmp3EC=_tmp3E9.elt_tq;_tmp3ED=_tmp3E9.ptr_atts;_tmp3EE=
_tmp3ED.zero_term;_LL247: elt_typ_opt=(void**)_tmp3EB;zero_term=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp3EE);goto _LL245;_LL248:;
_LL249: goto _LL245;_LL245:;}{void*_tmp3EF=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp3CF);e1->topt=({struct Cyc_Core_Opt*_tmp3F0=_cycalloc(
sizeof(*_tmp3F0));_tmp3F0->v=(void*)_tmp3EF;_tmp3F0;});{void*res_typ;{void*
_tmp3F1=Cyc_Tcutil_compress(_tmp3EF);struct Cyc_Absyn_ArrayInfo _tmp3F2;void*
_tmp3F3;struct Cyc_Absyn_Tqual _tmp3F4;struct Cyc_Absyn_Exp*_tmp3F5;struct Cyc_Absyn_Conref*
_tmp3F6;_LL24B: if(_tmp3F1 <= (void*)3?1:*((int*)_tmp3F1)!= 7)goto _LL24D;_tmp3F2=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F1)->f1;_tmp3F3=(void*)_tmp3F2.elt_type;
_tmp3F4=_tmp3F2.tq;_tmp3F5=_tmp3F2.num_elts;_tmp3F6=_tmp3F2.zero_term;_LL24C:
res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp3F7=_cycalloc(sizeof(*
_tmp3F7));_tmp3F7[0]=({struct Cyc_Absyn_PointerType_struct _tmp3F8;_tmp3F8.tag=4;
_tmp3F8.f1=({struct Cyc_Absyn_PtrInfo _tmp3F9;_tmp3F9.elt_typ=(void*)_tmp3F3;
_tmp3F9.elt_tq=_tmp3F4;_tmp3F9.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp3FA;_tmp3FA.rgn=(
void*)rgn;_tmp3FA.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp3FA.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp3FC;_tmp3FC.tag=0;_tmp3FC.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp3F5);
_tmp3FC;});_tmp3FB;}));_tmp3FA.zero_term=_tmp3F6;_tmp3FA;});_tmp3F9;});_tmp3F8;});
_tmp3F7;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)?Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp3FD=_cycalloc(sizeof(*
_tmp3FD));_tmp3FD->v=(void*)res_typ;_tmp3FD;});Cyc_Tcutil_unchecked_cast(te,e,*
topt,(void*)3);res_typ=*topt;}}goto _LL24A;_LL24D:;_LL24E:({void*_tmp3FE[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp3FF="tcExpNoPromote on Array_e returned non-array type";_tag_arr(_tmp3FF,
sizeof(char),_get_zero_arr_size(_tmp3FF,50));}),_tag_arr(_tmp3FE,sizeof(void*),0));});
_LL24A:;}return res_typ;}}}_LL237: if(*((int*)_tmp3CC)!= 0)goto _LL239;_tmp3D0=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp3CC)->f1;if(_tmp3D0 <= (void*)1?1:*((
int*)_tmp3D0)!= 5)goto _LL239;_tmp3D1=((struct Cyc_Absyn_String_c_struct*)_tmp3D0)->f1;
_LL238: {void*_tmp400=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(),(
void*)0,Cyc_Absyn_true_conref);void*_tmp401=Cyc_Tcexp_tcExp(te,(void**)& _tmp400,
e1);return Cyc_Absyn_atb_typ(_tmp401,rgn,Cyc_Absyn_empty_tqual(),(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp403;_tmp403.tag=0;_tmp403.f1=Cyc_Absyn_uint_exp(1,0);
_tmp403;});_tmp402;}),Cyc_Absyn_false_conref);}_LL239:;_LL23A: {void**topt2=0;
if(topt != 0){void*_tmp404=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp405;void*_tmp406;void**_tmp407;struct Cyc_Absyn_Tqual _tmp408;_LL250: if(
_tmp404 <= (void*)3?1:*((int*)_tmp404)!= 4)goto _LL252;_tmp405=((struct Cyc_Absyn_PointerType_struct*)
_tmp404)->f1;_tmp406=(void*)_tmp405.elt_typ;_tmp407=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp404)->f1).elt_typ;_tmp408=_tmp405.elt_tq;_LL251: topt2=(void**)_tmp407;goto
_LL24F;_LL252: if(_tmp404 <= (void*)3?1:*((int*)_tmp404)!= 2)goto _LL254;_LL253:
topt2=({void**_tmp409=_cycalloc(sizeof(*_tmp409));_tmp409[0]=*topt;_tmp409;});
goto _LL24F;_LL254:;_LL255: goto _LL24F;_LL24F:;}{void*telt=Cyc_Tcexp_tcExp(te,
topt2,e1);void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp40B=
_cycalloc(sizeof(*_tmp40B));_tmp40B[0]=({struct Cyc_Absyn_PointerType_struct
_tmp40C;_tmp40C.tag=4;_tmp40C.f1=({struct Cyc_Absyn_PtrInfo _tmp40D;_tmp40D.elt_typ=(
void*)telt;_tmp40D.elt_tq=Cyc_Absyn_empty_tqual();_tmp40D.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp40E;_tmp40E.rgn=(void*)rgn;_tmp40E.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp40E.bounds=Cyc_Absyn_bounds_one_conref;_tmp40E.zero_term=
Cyc_Absyn_false_conref;_tmp40E;});_tmp40D;});_tmp40C;});_tmp40B;});if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ)?Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp40A=_cycalloc(sizeof(*_tmp40A));
_tmp40A->v=(void*)res_typ;_tmp40A;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)
3);res_typ=*topt;}}return res_typ;}}_LL230:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp40F=t;struct Cyc_Absyn_ArrayInfo _tmp410;void*_tmp411;struct
Cyc_Absyn_Tqual _tmp412;struct Cyc_Absyn_Exp*_tmp413;struct Cyc_Absyn_Conref*
_tmp414;_LL257: if(_tmp40F <= (void*)3?1:*((int*)_tmp40F)!= 7)goto _LL259;_tmp410=((
struct Cyc_Absyn_ArrayType_struct*)_tmp40F)->f1;_tmp411=(void*)_tmp410.elt_type;
_tmp412=_tmp410.tq;_tmp413=_tmp410.num_elts;_tmp414=_tmp410.zero_term;_LL258: {
void*_tmp416;struct _tuple6 _tmp415=Cyc_Tcutil_addressof_props(te,e);_tmp416=
_tmp415.f2;{void*_tmp417=_tmp413 == 0?(void*)((void*)0):(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp419;_tmp419.tag=0;_tmp419.f1=(struct Cyc_Absyn_Exp*)_tmp413;_tmp419;});
_tmp418;});t=Cyc_Absyn_atb_typ(_tmp411,_tmp416,_tmp412,_tmp417,_tmp414);(void*)(((
struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);return t;}}_LL259:;_LL25A:
return t;_LL256:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);void*
_tmp41A=(void*)e->r;void*_tmp41B;_LL25C: if(*((int*)_tmp41A)!= 26)goto _LL25E;
_LL25D: goto _LL25F;_LL25E: if(*((int*)_tmp41A)!= 27)goto _LL260;_LL25F: goto _LL261;
_LL260: if(*((int*)_tmp41A)!= 0)goto _LL262;_tmp41B=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp41A)->f1;if(_tmp41B <= (void*)1?1:*((int*)_tmp41B)!= 5)goto _LL262;_LL261:
return t;_LL262:;_LL263: t=Cyc_Tcutil_compress(t);{void*_tmp41C=t;struct Cyc_Absyn_ArrayInfo
_tmp41D;void*_tmp41E;struct Cyc_Absyn_Tqual _tmp41F;struct Cyc_Absyn_Exp*_tmp420;
struct Cyc_Absyn_Conref*_tmp421;_LL265: if(_tmp41C <= (void*)3?1:*((int*)_tmp41C)!= 
7)goto _LL267;_tmp41D=((struct Cyc_Absyn_ArrayType_struct*)_tmp41C)->f1;_tmp41E=(
void*)_tmp41D.elt_type;_tmp41F=_tmp41D.tq;_tmp420=_tmp41D.num_elts;_tmp421=
_tmp41D.zero_term;_LL266: {void*_tmp423;struct _tuple6 _tmp422=Cyc_Tcutil_addressof_props(
te,e);_tmp423=_tmp422.f2;{void*b=_tmp420 == 0?(void*)((void*)0):(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp424=_cycalloc(sizeof(*_tmp424));_tmp424[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp425;_tmp425.tag=0;_tmp425.f1=(struct Cyc_Absyn_Exp*)
_tmp420;_tmp425;});_tmp424;});t=Cyc_Absyn_atb_typ(_tmp41E,_tmp423,_tmp41F,b,
_tmp421);Cyc_Tcutil_unchecked_cast(te,e,t,(void*)3);return t;}}_LL267:;_LL268:
return t;_LL264:;}_LL25B:;}static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp426=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp427;_LL26A: if(*((int*)_tmp426)!= 11)goto _LL26C;_tmp427=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp426)->f1;_LL26B: Cyc_Tcexp_tcExpNoInst(te,topt,_tmp427);(void*)(((struct Cyc_Core_Opt*)
_check_null(_tmp427->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp427->topt))->v));e->topt=_tmp427->topt;goto _LL269;
_LL26C:;_LL26D: Cyc_Tcexp_tcExpNoInst(te,topt,e);(void*)(((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{void*_tmp428=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp429;void*_tmp42A;struct Cyc_Absyn_Tqual
_tmp42B;struct Cyc_Absyn_PtrAtts _tmp42C;void*_tmp42D;struct Cyc_Absyn_Conref*
_tmp42E;struct Cyc_Absyn_Conref*_tmp42F;struct Cyc_Absyn_Conref*_tmp430;_LL26F: if(
_tmp428 <= (void*)3?1:*((int*)_tmp428)!= 4)goto _LL271;_tmp429=((struct Cyc_Absyn_PointerType_struct*)
_tmp428)->f1;_tmp42A=(void*)_tmp429.elt_typ;_tmp42B=_tmp429.elt_tq;_tmp42C=
_tmp429.ptr_atts;_tmp42D=(void*)_tmp42C.rgn;_tmp42E=_tmp42C.nullable;_tmp42F=
_tmp42C.bounds;_tmp430=_tmp42C.zero_term;_LL270:{void*_tmp431=Cyc_Tcutil_compress(
_tmp42A);struct Cyc_Absyn_FnInfo _tmp432;struct Cyc_List_List*_tmp433;struct Cyc_Core_Opt*
_tmp434;void*_tmp435;struct Cyc_List_List*_tmp436;int _tmp437;struct Cyc_Absyn_VarargInfo*
_tmp438;struct Cyc_List_List*_tmp439;struct Cyc_List_List*_tmp43A;_LL274: if(
_tmp431 <= (void*)3?1:*((int*)_tmp431)!= 8)goto _LL276;_tmp432=((struct Cyc_Absyn_FnType_struct*)
_tmp431)->f1;_tmp433=_tmp432.tvars;_tmp434=_tmp432.effect;_tmp435=(void*)_tmp432.ret_typ;
_tmp436=_tmp432.args;_tmp437=_tmp432.c_varargs;_tmp438=_tmp432.cyc_varargs;
_tmp439=_tmp432.rgn_po;_tmp43A=_tmp432.attributes;_LL275: if(_tmp433 != 0){struct
_RegionHandle _tmp43B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp43B;
_push_region(rgn);{struct _tuple4 _tmp43C=({struct _tuple4 _tmp449;_tmp449.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp449.f2=rgn;_tmp449;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp43C,_tmp433);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp446=_cycalloc(sizeof(*_tmp446));_tmp446[0]=({struct Cyc_Absyn_FnType_struct
_tmp447;_tmp447.tag=8;_tmp447.f1=({struct Cyc_Absyn_FnInfo _tmp448;_tmp448.tvars=0;
_tmp448.effect=_tmp434;_tmp448.ret_typ=(void*)_tmp435;_tmp448.args=_tmp436;
_tmp448.c_varargs=_tmp437;_tmp448.cyc_varargs=_tmp438;_tmp448.rgn_po=_tmp439;
_tmp448.attributes=_tmp43A;_tmp448;});_tmp447;});_tmp446;}));struct Cyc_Absyn_PointerType_struct*
_tmp43D=({struct Cyc_Absyn_PointerType_struct*_tmp442=_cycalloc(sizeof(*_tmp442));
_tmp442[0]=({struct Cyc_Absyn_PointerType_struct _tmp443;_tmp443.tag=4;_tmp443.f1=({
struct Cyc_Absyn_PtrInfo _tmp444;_tmp444.elt_typ=(void*)ftyp;_tmp444.elt_tq=
_tmp42B;_tmp444.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp445;_tmp445.rgn=(void*)
_tmp42D;_tmp445.nullable=_tmp42E;_tmp445.bounds=_tmp42F;_tmp445.zero_term=
_tmp430;_tmp445;});_tmp444;});_tmp443;});_tmp442;});struct Cyc_Absyn_Exp*_tmp43E=
Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmp440;_tmp440.tag=12;_tmp440.f1=_tmp43E;_tmp440.f2=ts;_tmp440;});_tmp43F;})));
e->topt=({struct Cyc_Core_Opt*_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441->v=(
void*)((void*)_tmp43D);_tmp441;});};_pop_region(rgn);}goto _LL273;_LL276:;_LL277:
goto _LL273;_LL273:;}goto _LL26E;_LL271:;_LL272: goto _LL26E;_LL26E:;}goto _LL269;
_LL269:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static void
Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Position_Segment*loc=e->loc;void*t;{void*_tmp44A=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp44B;struct _tuple1*_tmp44C;struct Cyc_Absyn_Exp*_tmp44D;struct Cyc_List_List*
_tmp44E;struct Cyc_Core_Opt*_tmp44F;struct Cyc_List_List*_tmp450;void*_tmp451;
struct _tuple1*_tmp452;void*_tmp453;void*_tmp454;struct Cyc_List_List*_tmp455;
struct Cyc_Absyn_Exp*_tmp456;void*_tmp457;struct Cyc_Absyn_Exp*_tmp458;struct Cyc_Core_Opt*
_tmp459;struct Cyc_Absyn_Exp*_tmp45A;struct Cyc_Absyn_Exp*_tmp45B;struct Cyc_Absyn_Exp*
_tmp45C;struct Cyc_Absyn_Exp*_tmp45D;struct Cyc_Absyn_Exp*_tmp45E;struct Cyc_Absyn_Exp*
_tmp45F;struct Cyc_Absyn_Exp*_tmp460;struct Cyc_List_List*_tmp461;struct Cyc_Absyn_VarargCallInfo*
_tmp462;struct Cyc_Absyn_VarargCallInfo**_tmp463;struct Cyc_Absyn_Exp*_tmp464;
struct Cyc_Absyn_Exp*_tmp465;struct Cyc_List_List*_tmp466;void*_tmp467;struct Cyc_Absyn_Exp*
_tmp468;void*_tmp469;void**_tmp46A;struct Cyc_Absyn_Exp*_tmp46B;struct Cyc_Absyn_Exp*
_tmp46C;struct Cyc_Absyn_Exp*_tmp46D;struct Cyc_Absyn_Exp*_tmp46E;void*_tmp46F;
void*_tmp470;void*_tmp471;struct Cyc_Absyn_Exp*_tmp472;struct Cyc_Absyn_Exp*
_tmp473;struct _tagged_arr*_tmp474;struct Cyc_Absyn_Exp*_tmp475;struct _tagged_arr*
_tmp476;struct Cyc_Absyn_Exp*_tmp477;struct Cyc_Absyn_Exp*_tmp478;struct Cyc_List_List*
_tmp479;struct _tuple2*_tmp47A;struct Cyc_List_List*_tmp47B;struct Cyc_List_List*
_tmp47C;struct Cyc_Absyn_Stmt*_tmp47D;struct Cyc_Absyn_Fndecl*_tmp47E;struct Cyc_Absyn_Exp*
_tmp47F;struct Cyc_Absyn_Vardecl*_tmp480;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_Absyn_Exp*
_tmp482;int _tmp483;int*_tmp484;struct _tuple1*_tmp485;struct _tuple1**_tmp486;
struct Cyc_List_List*_tmp487;struct Cyc_List_List**_tmp488;struct Cyc_List_List*
_tmp489;struct Cyc_Absyn_Aggrdecl*_tmp48A;struct Cyc_Absyn_Aggrdecl**_tmp48B;void*
_tmp48C;struct Cyc_List_List*_tmp48D;struct Cyc_List_List*_tmp48E;struct Cyc_Absyn_Tuniondecl*
_tmp48F;struct Cyc_Absyn_Tunionfield*_tmp490;struct _tuple1*_tmp491;struct _tuple1**
_tmp492;struct Cyc_Absyn_Enumdecl*_tmp493;struct Cyc_Absyn_Enumfield*_tmp494;
struct _tuple1*_tmp495;struct _tuple1**_tmp496;void*_tmp497;struct Cyc_Absyn_Enumfield*
_tmp498;struct Cyc_Absyn_MallocInfo _tmp499;int _tmp49A;int*_tmp49B;struct Cyc_Absyn_Exp*
_tmp49C;void**_tmp49D;void***_tmp49E;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_Absyn_Exp**
_tmp4A0;int _tmp4A1;int*_tmp4A2;_LL279: if(*((int*)_tmp44A)!= 11)goto _LL27B;
_tmp44B=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp44A)->f1;_LL27A: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp44B);return;_LL27B: if(*((int*)_tmp44A)!= 2)goto _LL27D;_tmp44C=((struct
Cyc_Absyn_UnknownId_e_struct*)_tmp44A)->f1;_LL27C: Cyc_Tcexp_resolve_unknown_id(
te,e,_tmp44C);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL27D: if(*((int*)_tmp44A)
!= 8)goto _LL27F;_tmp44D=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp44A)->f1;
_tmp44E=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp44A)->f2;_LL27E: Cyc_Tcexp_resolve_unknown_fn(
te,e,_tmp44D,_tmp44E);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL27F: if(*((int*)
_tmp44A)!= 34)goto _LL281;_tmp44F=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp44A)->f1;_tmp450=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp44A)->f2;
_LL280: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp450);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL281: if(*((int*)_tmp44A)!= 0)goto _LL283;_tmp451=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp44A)->f1;_LL282: t=Cyc_Tcexp_tcConst(te,loc,
topt,_tmp451,e);goto _LL278;_LL283: if(*((int*)_tmp44A)!= 1)goto _LL285;_tmp452=((
struct Cyc_Absyn_Var_e_struct*)_tmp44A)->f1;_tmp453=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp44A)->f2;_LL284: t=Cyc_Tcexp_tcVar(te,loc,_tmp452,_tmp453);goto _LL278;_LL285:
if(*((int*)_tmp44A)!= 3)goto _LL287;_tmp454=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp44A)->f1;_tmp455=((struct Cyc_Absyn_Primop_e_struct*)_tmp44A)->f2;_LL286: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp454,_tmp455);goto _LL278;_LL287: if(*((int*)
_tmp44A)!= 5)goto _LL289;_tmp456=((struct Cyc_Absyn_Increment_e_struct*)_tmp44A)->f1;
_tmp457=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp44A)->f2;_LL288: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp456,_tmp457);goto _LL278;_LL289: if(*((int*)_tmp44A)!= 4)goto
_LL28B;_tmp458=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp44A)->f1;_tmp459=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp44A)->f2;_tmp45A=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp44A)->f3;_LL28A: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp458,_tmp459,_tmp45A);
goto _LL278;_LL28B: if(*((int*)_tmp44A)!= 6)goto _LL28D;_tmp45B=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp44A)->f1;_tmp45C=((struct Cyc_Absyn_Conditional_e_struct*)_tmp44A)->f2;
_tmp45D=((struct Cyc_Absyn_Conditional_e_struct*)_tmp44A)->f3;_LL28C: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp45B,_tmp45C,_tmp45D);goto _LL278;_LL28D: if(*((int*)_tmp44A)!= 7)
goto _LL28F;_tmp45E=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp44A)->f1;_tmp45F=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp44A)->f2;_LL28E: t=Cyc_Tcexp_tcSeqExp(te,loc,
topt,_tmp45E,_tmp45F);goto _LL278;_LL28F: if(*((int*)_tmp44A)!= 9)goto _LL291;
_tmp460=((struct Cyc_Absyn_FnCall_e_struct*)_tmp44A)->f1;_tmp461=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp44A)->f2;_tmp462=((struct Cyc_Absyn_FnCall_e_struct*)_tmp44A)->f3;_tmp463=(
struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)_tmp44A)->f3;
_LL290: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp460,_tmp461,_tmp463);goto _LL278;
_LL291: if(*((int*)_tmp44A)!= 10)goto _LL293;_tmp464=((struct Cyc_Absyn_Throw_e_struct*)
_tmp44A)->f1;_LL292: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp464);goto _LL278;_LL293:
if(*((int*)_tmp44A)!= 12)goto _LL295;_tmp465=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp44A)->f1;_tmp466=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp44A)->f2;_LL294:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp465,_tmp466);goto _LL278;_LL295: if(*((
int*)_tmp44A)!= 13)goto _LL297;_tmp467=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp44A)->f1;_tmp468=((struct Cyc_Absyn_Cast_e_struct*)_tmp44A)->f2;_tmp469=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp44A)->f4;_tmp46A=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp44A)->f4);_LL296: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp467,_tmp468,(void**)
_tmp46A);goto _LL278;_LL297: if(*((int*)_tmp44A)!= 14)goto _LL299;_tmp46B=((struct
Cyc_Absyn_Address_e_struct*)_tmp44A)->f1;_LL298: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp46B);goto _LL278;_LL299: if(*((int*)_tmp44A)!= 15)goto _LL29B;_tmp46C=((
struct Cyc_Absyn_New_e_struct*)_tmp44A)->f1;_tmp46D=((struct Cyc_Absyn_New_e_struct*)
_tmp44A)->f2;_LL29A: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp46C,e,_tmp46D);goto _LL278;
_LL29B: if(*((int*)_tmp44A)!= 17)goto _LL29D;_tmp46E=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp44A)->f1;_LL29C: {void*_tmp4A3=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp46E);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4A3);goto _LL278;}_LL29D: if(*((int*)_tmp44A)!= 16)goto _LL29F;
_tmp46F=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp44A)->f1;_LL29E: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp46F);goto _LL278;_LL29F: if(*((int*)_tmp44A)!= 18)goto _LL2A1;
_tmp470=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp44A)->f1;_tmp471=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp44A)->f2;_LL2A0: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp470,_tmp471);goto _LL278;_LL2A1: if(*((int*)_tmp44A)!= 19)goto _LL2A3;
_LL2A2:({void*_tmp4A4[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp4A5="gen() not in top-level initializer";
_tag_arr(_tmp4A5,sizeof(char),_get_zero_arr_size(_tmp4A5,35));}),_tag_arr(
_tmp4A4,sizeof(void*),0));});return;_LL2A3: if(*((int*)_tmp44A)!= 20)goto _LL2A5;
_tmp472=((struct Cyc_Absyn_Deref_e_struct*)_tmp44A)->f1;_LL2A4: t=Cyc_Tcexp_tcDeref(
te,loc,topt,_tmp472);goto _LL278;_LL2A5: if(*((int*)_tmp44A)!= 21)goto _LL2A7;
_tmp473=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp44A)->f1;_tmp474=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp44A)->f2;_LL2A6: t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp473,_tmp474);goto
_LL278;_LL2A7: if(*((int*)_tmp44A)!= 22)goto _LL2A9;_tmp475=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp44A)->f1;_tmp476=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp44A)->f2;_LL2A8: t=
Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp475,_tmp476);goto _LL278;_LL2A9: if(*((int*)
_tmp44A)!= 23)goto _LL2AB;_tmp477=((struct Cyc_Absyn_Subscript_e_struct*)_tmp44A)->f1;
_tmp478=((struct Cyc_Absyn_Subscript_e_struct*)_tmp44A)->f2;_LL2AA: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp477,_tmp478);goto _LL278;_LL2AB: if(*((int*)_tmp44A)!= 24)goto
_LL2AD;_tmp479=((struct Cyc_Absyn_Tuple_e_struct*)_tmp44A)->f1;_LL2AC: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp479);goto _LL278;_LL2AD: if(*((int*)_tmp44A)!= 25)goto _LL2AF;
_tmp47A=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp44A)->f1;_tmp47B=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp44A)->f2;_LL2AE: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp47A,_tmp47B);goto
_LL278;_LL2AF: if(*((int*)_tmp44A)!= 26)goto _LL2B1;_tmp47C=((struct Cyc_Absyn_Array_e_struct*)
_tmp44A)->f1;_LL2B0: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp4A6=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp4A7;void*_tmp4A8;void**
_tmp4A9;struct Cyc_Absyn_Conref*_tmp4AA;_LL2C6: if(_tmp4A6 <= (void*)3?1:*((int*)
_tmp4A6)!= 7)goto _LL2C8;_tmp4A7=((struct Cyc_Absyn_ArrayType_struct*)_tmp4A6)->f1;
_tmp4A8=(void*)_tmp4A7.elt_type;_tmp4A9=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp4A6)->f1).elt_type;_tmp4AA=_tmp4A7.zero_term;_LL2C7: elt_topt=(void**)_tmp4A9;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4AA);
goto _LL2C5;_LL2C8:;_LL2C9: goto _LL2C5;_LL2C5:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp47C);goto _LL278;}_LL2B1: if(*((int*)_tmp44A)!= 35)goto
_LL2B3;_tmp47D=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp44A)->f1;_LL2B2: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp47D);goto _LL278;_LL2B3: if(*((int*)_tmp44A)!= 36)goto _LL2B5;
_tmp47E=((struct Cyc_Absyn_Codegen_e_struct*)_tmp44A)->f1;_LL2B4: t=Cyc_Tcexp_tcCodegen(
te,loc,topt,_tmp47E);goto _LL278;_LL2B5: if(*((int*)_tmp44A)!= 37)goto _LL2B7;
_tmp47F=((struct Cyc_Absyn_Fill_e_struct*)_tmp44A)->f1;_LL2B6: t=Cyc_Tcexp_tcFill(
te,loc,topt,_tmp47F);goto _LL278;_LL2B7: if(*((int*)_tmp44A)!= 27)goto _LL2B9;
_tmp480=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp44A)->f1;_tmp481=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp44A)->f2;_tmp482=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp44A)->f3;_tmp483=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp44A)->f4;
_tmp484=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp44A)->f4;_LL2B8: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp480,_tmp481,_tmp482,_tmp484);goto
_LL278;_LL2B9: if(*((int*)_tmp44A)!= 28)goto _LL2BB;_tmp485=((struct Cyc_Absyn_Struct_e_struct*)
_tmp44A)->f1;_tmp486=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp44A)->f1;_tmp487=((struct Cyc_Absyn_Struct_e_struct*)_tmp44A)->f2;_tmp488=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp44A)->f2;_tmp489=((
struct Cyc_Absyn_Struct_e_struct*)_tmp44A)->f3;_tmp48A=((struct Cyc_Absyn_Struct_e_struct*)
_tmp44A)->f4;_tmp48B=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp44A)->f4;_LL2BA: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp486,_tmp488,_tmp489,
_tmp48B);goto _LL278;_LL2BB: if(*((int*)_tmp44A)!= 29)goto _LL2BD;_tmp48C=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp44A)->f1;_tmp48D=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp44A)->f2;_LL2BC: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp48C,_tmp48D);goto _LL278;
_LL2BD: if(*((int*)_tmp44A)!= 30)goto _LL2BF;_tmp48E=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp44A)->f1;_tmp48F=((struct Cyc_Absyn_Tunion_e_struct*)_tmp44A)->f2;_tmp490=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp44A)->f3;_LL2BE: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp48E,_tmp48F,_tmp490);goto _LL278;_LL2BF: if(*((int*)_tmp44A)!= 31)goto
_LL2C1;_tmp491=((struct Cyc_Absyn_Enum_e_struct*)_tmp44A)->f1;_tmp492=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp44A)->f1;_tmp493=((struct Cyc_Absyn_Enum_e_struct*)
_tmp44A)->f2;_tmp494=((struct Cyc_Absyn_Enum_e_struct*)_tmp44A)->f3;_LL2C0:*
_tmp492=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp494))->name;t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB[0]=({
struct Cyc_Absyn_EnumType_struct _tmp4AC;_tmp4AC.tag=12;_tmp4AC.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp493))->name;_tmp4AC.f2=_tmp493;_tmp4AC;});_tmp4AB;});goto _LL278;
_LL2C1: if(*((int*)_tmp44A)!= 32)goto _LL2C3;_tmp495=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp44A)->f1;_tmp496=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp44A)->f1;_tmp497=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp44A)->f2;
_tmp498=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp44A)->f3;_LL2C2:*_tmp496=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp498))->name;t=_tmp497;goto _LL278;
_LL2C3: if(*((int*)_tmp44A)!= 33)goto _LL278;_tmp499=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp44A)->f1;_tmp49A=_tmp499.is_calloc;_tmp49B=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp44A)->f1).is_calloc;_tmp49C=_tmp499.rgn;_tmp49D=_tmp499.elt_type;_tmp49E=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp44A)->f1).elt_type;_tmp49F=
_tmp499.num_elts;_tmp4A0=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp44A)->f1).num_elts;_tmp4A1=_tmp499.fat_result;_tmp4A2=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp44A)->f1).fat_result;_LL2C4: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp49C,_tmp49E,
_tmp4A0,_tmp49B,_tmp4A2);goto _LL278;_LL278:;}e->topt=({struct Cyc_Core_Opt*
_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD->v=(void*)t;_tmp4AD;});}
