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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*
x);int Cyc_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);int Cyc_strptrcmp(
struct _tagged_arr*s1,struct _tagged_arr*s2);struct Cyc_Lineno_Pos{struct
_tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[
14];struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};
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
_tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _tagged_arr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{
void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual();
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_string_typ(void*rgn);void*Cyc_Absyn_const_string_typ(
void*rgn);void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct
Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct _tagged_arr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(void*
a);typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{int __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _tagged_arr f1;};extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*
Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);struct Cyc_Set_Set*Cyc_Set_rinsert(
struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*
s,void*elt);extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];int Cyc_Dict_is_empty(struct Cyc_Dict_Dict*d);
int Cyc_Dict_member(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*k);
struct Cyc_Dict_Dict*Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d);struct _tuple3{void*f1;void*f2;};struct _tuple3*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple3*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_filter_c(
int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);struct Cyc_Iter_Iter
Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,
struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*
rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*
po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
ae;struct Cyc_Core_Opt*le;};struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv();struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_new_fenv(struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct
Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tagged_arr*,struct Cyc_List_List*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_xtuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple0*);int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*
te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int
Cyc_Tcutil_is_function_type(void*t);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(
void*,void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*
k,int allow_evars,void*);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,
struct Cyc_List_List*);void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_supports_default(void*);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void Cyc_Tcstmt_tcStmt(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple4{
unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*
ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*field;};struct Cyc_Absyn_Aggrdecl*
Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Absyn_Tuniondecl*
Cyc_Tcdecl_merge_tuniondecl(struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Absyn_Enumdecl*
Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);void*Cyc_Tcdecl_merge_binding(
void*d0,void*d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_List_List*
Cyc_Tcdecl_sort_xtunion_fields(struct Cyc_List_List*f,int*res,struct _tagged_arr*v,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Tcgenrep_RepInfo;
struct Cyc_Dict_Dict*Cyc_Tcgenrep_empty_typerep_dict();struct _tuple5{struct Cyc_Dict_Dict*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple5 Cyc_Tcgenrep_tcGenrep(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
void*type,struct Cyc_Dict_Dict*dict);static char _tmp0[1]="";static struct
_tagged_arr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};static struct _tagged_arr*Cyc_Tc_tc_msg=(
struct _tagged_arr*)& Cyc_Tc_tc_msg_c;static struct Cyc_List_List*Cyc_Tc_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmp1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmp2;struct Cyc_List_List*_tmp3;struct Cyc_List_List**_tmp4;_LL1: if(_tmp1 <= (void*)
3?1:*((int*)_tmp1)!= 8)goto _LL3;_tmp2=((struct Cyc_Absyn_FnType_struct*)_tmp1)->f1;
_tmp3=_tmp2.attributes;_tmp4=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp1)->f1).attributes;_LL2: {struct Cyc_List_List*_tmp5=0;for(0;atts != 0;atts=
atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))*_tmp4=({struct Cyc_List_List*
_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->hd=(void*)((void*)atts->hd);_tmp6->tl=*
_tmp4;_tmp6;});else{_tmp5=({struct Cyc_List_List*_tmp7=_cycalloc(sizeof(*_tmp7));
_tmp7->hd=(void*)((void*)atts->hd);_tmp7->tl=_tmp5;_tmp7;});}}return _tmp5;}_LL3:;
_LL4:({void*_tmp8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp9="transfer_fn_type_atts";_tag_arr(_tmp9,sizeof(char),
_get_zero_arr_size(_tmp9,22));}),_tag_arr(_tmp8,sizeof(void*),0));});_LL0:;}
struct _tuple6{void*f1;int f2;};static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Vardecl*
vd,int check_var_init){struct Cyc_Absyn_Vardecl _tmpB;void*_tmpC;struct _tuple0*
_tmpD;struct _tuple0 _tmpE;void*_tmpF;struct _tagged_arr*_tmp10;void*_tmp11;struct
Cyc_Absyn_Exp*_tmp12;struct Cyc_List_List*_tmp13;struct Cyc_Absyn_Vardecl*_tmpA=vd;
_tmpB=*_tmpA;_tmpC=(void*)_tmpB.sc;_tmpD=_tmpB.name;_tmpE=*_tmpD;_tmpF=_tmpE.f1;
_tmp10=_tmpE.f2;_tmp11=(void*)_tmpB.type;_tmp12=_tmpB.initializer;_tmp13=_tmpB.attributes;{
void*_tmp14=_tmpF;struct Cyc_List_List*_tmp15;struct Cyc_List_List*_tmp16;_LL6: if(
_tmp14 <= (void*)1?1:*((int*)_tmp14)!= 0)goto _LL8;_tmp15=((struct Cyc_Absyn_Rel_n_struct*)
_tmp14)->f1;if(_tmp15 != 0)goto _LL8;_LL7: goto _LL9;_LL8: if(_tmp14 <= (void*)1?1:*((
int*)_tmp14)!= 1)goto _LLA;_tmp16=((struct Cyc_Absyn_Abs_n_struct*)_tmp14)->f1;if(
_tmp16 != 0)goto _LLA;_LL9: goto _LL5;_LLA:;_LLB:({struct Cyc_String_pa_struct _tmp19;
_tmp19.tag=0;_tmp19.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
vd->name));{void*_tmp17[1]={& _tmp19};Cyc_Tcutil_terr(loc,({const char*_tmp18="qualified variable declarations are not implemented (%s)";
_tag_arr(_tmp18,sizeof(char),_get_zero_arr_size(_tmp18,57));}),_tag_arr(_tmp17,
sizeof(void*),1));}});return;_LL5:;}(*vd->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A[0]=({struct Cyc_Absyn_Abs_n_struct _tmp1B;
_tmp1B.tag=1;_tmp1B.f1=te->ns;_tmp1B;});_tmp1A;});{void*_tmp1C=Cyc_Tcutil_compress(
_tmp11);struct Cyc_Absyn_ArrayInfo _tmp1D;void*_tmp1E;struct Cyc_Absyn_Tqual _tmp1F;
struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Conref*_tmp21;_LLD: if(_tmp1C <= (void*)
3?1:*((int*)_tmp1C)!= 7)goto _LLF;_tmp1D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1C)->f1;_tmp1E=(void*)_tmp1D.elt_type;_tmp1F=_tmp1D.tq;_tmp20=_tmp1D.num_elts;
if(_tmp20 != 0)goto _LLF;_tmp21=_tmp1D.zero_term;if(!(_tmp12 != 0))goto _LLF;_LLE:{
void*_tmp22=(void*)_tmp12->r;void*_tmp23;struct _tagged_arr _tmp24;struct Cyc_Absyn_Exp*
_tmp25;struct Cyc_List_List*_tmp26;struct Cyc_List_List*_tmp27;_LL12: if(*((int*)
_tmp22)!= 0)goto _LL14;_tmp23=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp22)->f1;
if(_tmp23 <= (void*)1?1:*((int*)_tmp23)!= 5)goto _LL14;_tmp24=((struct Cyc_Absyn_String_c_struct*)
_tmp23)->f1;_LL13: _tmp11=(void*)(vd->type=(void*)Cyc_Absyn_array_typ(_tmp1E,
_tmp1F,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_arr_size(_tmp24,sizeof(char)),
0),_tmp21));goto _LL11;_LL14: if(*((int*)_tmp22)!= 27)goto _LL16;_tmp25=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp22)->f2;_LL15: _tmp11=(void*)(vd->type=(void*)Cyc_Absyn_array_typ(_tmp1E,
_tmp1F,(struct Cyc_Absyn_Exp*)_tmp25,_tmp21));goto _LL11;_LL16: if(*((int*)_tmp22)
!= 34)goto _LL18;_tmp26=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp22)->f2;
_LL17: _tmp27=_tmp26;goto _LL19;_LL18: if(*((int*)_tmp22)!= 26)goto _LL1A;_tmp27=((
struct Cyc_Absyn_Array_e_struct*)_tmp22)->f1;_LL19: _tmp11=(void*)(vd->type=(void*)
Cyc_Absyn_array_typ(_tmp1E,_tmp1F,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp27),0),_tmp21));
goto _LL11;_LL1A:;_LL1B: goto _LL11;_LL11:;}goto _LLC;_LLF:;_LL10: goto _LLC;_LLC:;}
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp11);{void*_tmp28=Cyc_Tcutil_compress(
_tmp11);_LL1D: if(_tmp28 <= (void*)3?1:*((int*)_tmp28)!= 7)goto _LL1F;_LL1E: vd->escapes=
0;goto _LL1C;_LL1F:;_LL20: vd->escapes=1;goto _LL1C;_LL1C:;}if(Cyc_Tcutil_is_function_type(
_tmp11))_tmp13=Cyc_Tc_transfer_fn_type_atts(_tmp11,_tmp13);if(_tmpC == (void*)3?1:
_tmpC == (void*)4){if(_tmp12 != 0)({void*_tmp29[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp2A="extern declaration should not have initializer";_tag_arr(_tmp2A,
sizeof(char),_get_zero_arr_size(_tmp2A,47));}),_tag_arr(_tmp29,sizeof(void*),0));});}
else{if(!Cyc_Tcutil_is_function_type(_tmp11)){for(0;_tmp13 != 0;_tmp13=_tmp13->tl){
void*_tmp2B=(void*)_tmp13->hd;_LL22: if(_tmp2B <= (void*)17?1:*((int*)_tmp2B)!= 1)
goto _LL24;_LL23: goto _LL25;_LL24: if(_tmp2B <= (void*)17?1:*((int*)_tmp2B)!= 2)goto
_LL26;_LL25: goto _LL27;_LL26: if((int)_tmp2B != 6)goto _LL28;_LL27: goto _LL29;_LL28:
if((int)_tmp2B != 7)goto _LL2A;_LL29: goto _LL2B;_LL2A: if((int)_tmp2B != 8)goto _LL2C;
_LL2B: goto _LL2D;_LL2C: if((int)_tmp2B != 9)goto _LL2E;_LL2D: goto _LL2F;_LL2E: if((int)
_tmp2B != 10)goto _LL30;_LL2F: goto _LL31;_LL30: if((int)_tmp2B != 11)goto _LL32;_LL31:
continue;_LL32:;_LL33:({struct Cyc_String_pa_struct _tmp2F;_tmp2F.tag=0;_tmp2F.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name));{struct
Cyc_String_pa_struct _tmp2E;_tmp2E.tag=0;_tmp2E.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absyn_attribute2string((void*)_tmp13->hd));{void*_tmp2C[2]={&
_tmp2E,& _tmp2F};Cyc_Tcutil_terr(loc,({const char*_tmp2D="bad attribute %s for variable %s";
_tag_arr(_tmp2D,sizeof(char),_get_zero_arr_size(_tmp2D,33));}),_tag_arr(_tmp2C,
sizeof(void*),2));}}});goto _LL21;_LL21:;}if(_tmp12 == 0){if(check_var_init?!Cyc_Tcutil_supports_default(
_tmp11): 0)({struct Cyc_String_pa_struct _tmp33;_tmp33.tag=0;_tmp33.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp11));{struct Cyc_String_pa_struct
_tmp32;_tmp32.tag=0;_tmp32.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
vd->name));{void*_tmp30[2]={& _tmp32,& _tmp33};Cyc_Tcutil_terr(loc,({const char*
_tmp31="initializer required for variable %s of type %s";_tag_arr(_tmp31,sizeof(
char),_get_zero_arr_size(_tmp31,48));}),_tag_arr(_tmp30,sizeof(void*),2));}}});}
else{struct Cyc_Absyn_Exp*_tmp34=(struct Cyc_Absyn_Exp*)_tmp12;void*_tmp35=Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp11,_tmp34);if(!Cyc_Tcutil_coerce_assign(te,_tmp34,_tmp11)){({
struct Cyc_String_pa_struct _tmp3A;_tmp3A.tag=0;_tmp3A.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp35));{struct Cyc_String_pa_struct
_tmp39;_tmp39.tag=0;_tmp39.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp11));{struct Cyc_String_pa_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name));{void*_tmp36[
3]={& _tmp38,& _tmp39,& _tmp3A};Cyc_Tcutil_terr(loc,({const char*_tmp37="%s declared with type \n%s\n but initialized with type \n%s";
_tag_arr(_tmp37,sizeof(char),_get_zero_arr_size(_tmp37,57));}),_tag_arr(_tmp36,
sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}if(!Cyc_Tcutil_is_const_exp(
te,_tmp34))({void*_tmp3B[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3C="initializer is not a constant expression";
_tag_arr(_tmp3C,sizeof(char),_get_zero_arr_size(_tmp3C,41));}),_tag_arr(_tmp3B,
sizeof(void*),0));});}}else{for(0;_tmp13 != 0;_tmp13=_tmp13->tl){void*_tmp3D=(
void*)_tmp13->hd;_LL35: if(_tmp3D <= (void*)17?1:*((int*)_tmp3D)!= 0)goto _LL37;
_LL36: goto _LL38;_LL37: if((int)_tmp3D != 0)goto _LL39;_LL38: goto _LL3A;_LL39: if((int)
_tmp3D != 1)goto _LL3B;_LL3A: goto _LL3C;_LL3B: if((int)_tmp3D != 2)goto _LL3D;_LL3C:
goto _LL3E;_LL3D: if((int)_tmp3D != 3)goto _LL3F;_LL3E: goto _LL40;_LL3F: if(_tmp3D <= (
void*)17?1:*((int*)_tmp3D)!= 3)goto _LL41;_LL40: goto _LL42;_LL41: if(_tmp3D <= (void*)
17?1:*((int*)_tmp3D)!= 4)goto _LL43;_LL42: goto _LL44;_LL43: if((int)_tmp3D != 16)
goto _LL45;_LL44: goto _LL46;_LL45: if((int)_tmp3D != 4)goto _LL47;_LL46:({void*_tmp3E[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp3F="tcVardecl: fn type atts in function var decl";_tag_arr(_tmp3F,
sizeof(char),_get_zero_arr_size(_tmp3F,45));}),_tag_arr(_tmp3E,sizeof(void*),0));});
_LL47: if(_tmp3D <= (void*)17?1:*((int*)_tmp3D)!= 1)goto _LL49;_LL48: goto _LL4A;
_LL49: if((int)_tmp3D != 5)goto _LL4B;_LL4A:({struct Cyc_String_pa_struct _tmp42;
_tmp42.tag=0;_tmp42.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp13->hd));{void*_tmp40[1]={& _tmp42};Cyc_Tcutil_terr(loc,({const char*
_tmp41="bad attribute %s in function declaration";_tag_arr(_tmp41,sizeof(char),
_get_zero_arr_size(_tmp41,41));}),_tag_arr(_tmp40,sizeof(void*),1));}});goto
_LL34;_LL4B:;_LL4C: continue;_LL34:;}}}{struct _handler_cons _tmp43;_push_handler(&
_tmp43);{int _tmp45=0;if(setjmp(_tmp43.handler))_tmp45=1;if(!_tmp45){{struct
_tuple6*_tmp46=((struct _tuple6*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))
Cyc_Dict_lookup)(ge->ordinaries,_tmp10);void*_tmp47=(*_tmp46).f1;void*_tmp48;
_LL4E: if(*((int*)_tmp47)!= 0)goto _LL50;_tmp48=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp47)->f1;_LL4F: {struct Cyc_Absyn_Global_b_struct*_tmp49=({struct Cyc_Absyn_Global_b_struct*
_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({struct Cyc_Absyn_Global_b_struct
_tmp4F;_tmp4F.tag=0;_tmp4F.f1=vd;_tmp4F;});_tmp4E;});void*_tmp4A=Cyc_Tcdecl_merge_binding(
_tmp48,(void*)_tmp49,loc,Cyc_Tc_tc_msg);if(_tmp4A == (void*)0){_npop_handler(0);
return;}if(_tmp4A == _tmp48?(*_tmp46).f2: 0){_npop_handler(0);return;}ge->ordinaries=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple6*
v))Cyc_Dict_insert)(ge->ordinaries,_tmp10,({struct _tuple6*_tmp4B=_cycalloc(
sizeof(*_tmp4B));_tmp4B->f1=(void*)({struct Cyc_Tcenv_VarRes_struct*_tmp4C=
_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=({struct Cyc_Tcenv_VarRes_struct _tmp4D;
_tmp4D.tag=0;_tmp4D.f1=(void*)_tmp4A;_tmp4D;});_tmp4C;});_tmp4B->f2=1;_tmp4B;}));
_npop_handler(0);return;}_LL50: if(*((int*)_tmp47)!= 1)goto _LL52;_LL51:({void*
_tmp50[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp51="variable declaration shadows previous struct declaration";
_tag_arr(_tmp51,sizeof(char),_get_zero_arr_size(_tmp51,57));}),_tag_arr(_tmp50,
sizeof(void*),0));});goto _LL4D;_LL52: if(*((int*)_tmp47)!= 2)goto _LL54;_LL53:({
void*_tmp52[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp53="variable declaration shadows previous [x]tunion constructor";
_tag_arr(_tmp53,sizeof(char),_get_zero_arr_size(_tmp53,60));}),_tag_arr(_tmp52,
sizeof(void*),0));});goto _LL4D;_LL54: if(*((int*)_tmp47)!= 4)goto _LL56;_LL55: goto
_LL57;_LL56: if(*((int*)_tmp47)!= 3)goto _LL4D;_LL57:({void*_tmp54[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp55="variable declaration shadows previous enum tag";_tag_arr(
_tmp55,sizeof(char),_get_zero_arr_size(_tmp55,47));}),_tag_arr(_tmp54,sizeof(
void*),0));});goto _LL4D;_LL4D:;};_pop_handler();}else{void*_tmp44=(void*)
_exn_thrown;void*_tmp57=_tmp44;_LL59: if(_tmp57 != Cyc_Dict_Absent)goto _LL5B;_LL5A:
goto _LL58;_LL5B:;_LL5C:(void)_throw(_tmp57);_LL58:;}}}ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple6*v))Cyc_Dict_insert)(ge->ordinaries,
_tmp10,({struct _tuple6*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->f1=(void*)({
struct Cyc_Tcenv_VarRes_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59[0]=({
struct Cyc_Tcenv_VarRes_struct _tmp5A;_tmp5A.tag=0;_tmp5A.f1=(void*)((void*)({
struct Cyc_Absyn_Global_b_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({
struct Cyc_Absyn_Global_b_struct _tmp5C;_tmp5C.tag=0;_tmp5C.f1=vd;_tmp5C;});_tmp5B;}));
_tmp5A;});_tmp59;});_tmp58->f2=0;_tmp58;}));}static int Cyc_Tc_is_main(struct
_tuple0*n){struct _tuple0 _tmp5E;void*_tmp5F;struct _tagged_arr*_tmp60;struct
_tuple0*_tmp5D=n;_tmp5E=*_tmp5D;_tmp5F=_tmp5E.f1;_tmp60=_tmp5E.f2;{void*_tmp61=
_tmp5F;struct Cyc_List_List*_tmp62;_LL5E: if(_tmp61 <= (void*)1?1:*((int*)_tmp61)!= 
1)goto _LL60;_tmp62=((struct Cyc_Absyn_Abs_n_struct*)_tmp61)->f1;if(_tmp62 != 0)
goto _LL60;_LL5F: return Cyc_strcmp((struct _tagged_arr)*_tmp60,({const char*_tmp63="main";
_tag_arr(_tmp63,sizeof(char),_get_zero_arr_size(_tmp63,5));}))== 0;_LL60:;_LL61:
return 0;_LL5D:;}}struct _tuple7{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*
f3;};static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*fd){struct _tagged_arr*v=(*
fd->name).f2;if((void*)fd->sc == (void*)4)({void*_tmp64[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp65="extern \"C\" functions cannot be implemented in Cyclone";
_tag_arr(_tmp65,sizeof(char),_get_zero_arr_size(_tmp65,54));}),_tag_arr(_tmp64,
sizeof(void*),0));});{void*_tmp66=(*fd->name).f1;struct Cyc_List_List*_tmp67;
struct Cyc_List_List*_tmp68;_LL63: if(_tmp66 <= (void*)1?1:*((int*)_tmp66)!= 0)goto
_LL65;_tmp67=((struct Cyc_Absyn_Rel_n_struct*)_tmp66)->f1;if(_tmp67 != 0)goto _LL65;
_LL64: goto _LL62;_LL65: if(_tmp66 <= (void*)1?1:*((int*)_tmp66)!= 1)goto _LL67;
_tmp68=((struct Cyc_Absyn_Abs_n_struct*)_tmp66)->f1;_LL66:({void*_tmp69[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp6A="tc: Abs_n in tcFndecl";_tag_arr(_tmp6A,sizeof(char),_get_zero_arr_size(
_tmp6A,22));}),_tag_arr(_tmp69,sizeof(void*),0));});_LL67:;_LL68:({struct Cyc_String_pa_struct
_tmp6D;_tmp6D.tag=0;_tmp6D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
fd->name));{void*_tmp6B[1]={& _tmp6D};Cyc_Tcutil_terr(loc,({const char*_tmp6C="qualified function declarations are not implemented (%s)";
_tag_arr(_tmp6C,sizeof(char),_get_zero_arr_size(_tmp6C,57));}),_tag_arr(_tmp6B,
sizeof(void*),1));}});return;_LL62:;}(*fd->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E[0]=({struct Cyc_Absyn_Abs_n_struct _tmp6F;
_tmp6F.tag=1;_tmp6F.f1=te->ns;_tmp6F;});_tmp6E;});Cyc_Tcutil_check_fndecl_valid_type(
loc,te,fd);{void*t=Cyc_Tcutil_fndecl2typ(fd);fd->attributes=Cyc_Tc_transfer_fn_type_atts(
t,fd->attributes);{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=
atts->tl){void*_tmp70=(void*)atts->hd;_LL6A: if((int)_tmp70 != 5)goto _LL6C;_LL6B:
goto _LL6D;_LL6C: if(_tmp70 <= (void*)17?1:*((int*)_tmp70)!= 1)goto _LL6E;_LL6D:({
struct Cyc_String_pa_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp71[1]={&
_tmp73};Cyc_Tcutil_terr(loc,({const char*_tmp72="bad attribute %s for function";
_tag_arr(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,30));}),_tag_arr(_tmp71,
sizeof(void*),1));}});goto _LL69;_LL6E:;_LL6F: goto _LL69;_LL69:;}}{struct
_handler_cons _tmp74;_push_handler(& _tmp74);{int _tmp76=0;if(setjmp(_tmp74.handler))
_tmp76=1;if(!_tmp76){{struct _tuple6*_tmp77=((struct _tuple6*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->ordinaries,v);void*_tmp78=(*_tmp77).f1;
void*_tmp79;_LL71: if(*((int*)_tmp78)!= 0)goto _LL73;_tmp79=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp78)->f1;_LL72: {struct Cyc_Absyn_Funname_b_struct*_tmp7A=({struct Cyc_Absyn_Funname_b_struct*
_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[0]=({struct Cyc_Absyn_Funname_b_struct
_tmp80;_tmp80.tag=1;_tmp80.f1=fd;_tmp80;});_tmp7F;});void*_tmp7B=Cyc_Tcdecl_merge_binding(
_tmp79,(void*)_tmp7A,loc,Cyc_Tc_tc_msg);if(_tmp7B == (void*)0)goto _LL70;if(_tmp7B
== _tmp79?(*_tmp77).f2: 0)goto _LL70;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple6*v))Cyc_Dict_insert)(ge->ordinaries,
v,({struct _tuple6*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->f1=(void*)({struct
Cyc_Tcenv_VarRes_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=({struct Cyc_Tcenv_VarRes_struct
_tmp7E;_tmp7E.tag=0;_tmp7E.f1=(void*)_tmp7B;_tmp7E;});_tmp7D;});_tmp7C->f2=1;
_tmp7C;}));goto _LL70;}_LL73: if(*((int*)_tmp78)!= 1)goto _LL75;_LL74:({void*_tmp81[
0]={};Cyc_Tcutil_warn(loc,({const char*_tmp82="function declaration shadows previous type declaration";
_tag_arr(_tmp82,sizeof(char),_get_zero_arr_size(_tmp82,55));}),_tag_arr(_tmp81,
sizeof(void*),0));});goto _LL70;_LL75: if(*((int*)_tmp78)!= 2)goto _LL77;_LL76:({
void*_tmp83[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp84="function declaration shadows previous [x]tunion constructor";
_tag_arr(_tmp84,sizeof(char),_get_zero_arr_size(_tmp84,60));}),_tag_arr(_tmp83,
sizeof(void*),0));});goto _LL70;_LL77: if(*((int*)_tmp78)!= 4)goto _LL79;_LL78: goto
_LL7A;_LL79: if(*((int*)_tmp78)!= 3)goto _LL70;_LL7A:({void*_tmp85[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp86="function declaration shadows previous enum tag";_tag_arr(
_tmp86,sizeof(char),_get_zero_arr_size(_tmp86,47));}),_tag_arr(_tmp85,sizeof(
void*),0));});goto _LL70;_LL70:;};_pop_handler();}else{void*_tmp75=(void*)
_exn_thrown;void*_tmp88=_tmp75;_LL7C: if(_tmp88 != Cyc_Dict_Absent)goto _LL7E;_LL7D:
ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*
k,struct _tuple6*v))Cyc_Dict_insert)(ge->ordinaries,v,({struct _tuple6*_tmp89=
_cycalloc(sizeof(*_tmp89));_tmp89->f1=(void*)({struct Cyc_Tcenv_VarRes_struct*
_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A[0]=({struct Cyc_Tcenv_VarRes_struct
_tmp8B;_tmp8B.tag=0;_tmp8B.f1=(void*)((void*)({struct Cyc_Absyn_Funname_b_struct*
_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Absyn_Funname_b_struct
_tmp8D;_tmp8D.tag=1;_tmp8D.f1=fd;_tmp8D;});_tmp8C;}));_tmp8B;});_tmp8A;});_tmp89->f2=
0;_tmp89;}));goto _LL7B;_LL7E:;_LL7F:(void)_throw(_tmp88);_LL7B:;}}}te->le=({
struct Cyc_Core_Opt*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->v=(void*)((void*)({
struct Cyc_Tcenv_Outermost_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({
struct Cyc_Tcenv_Outermost_struct _tmp90;_tmp90.tag=0;_tmp90.f1=(void*)Cyc_Tcenv_new_fenv(
loc,fd);_tmp90;});_tmp8F;}));_tmp8E;});Cyc_Tcstmt_tcStmt(te,fd->body,0);Cyc_Tcenv_check_delayed_effects(
te);Cyc_Tcenv_check_delayed_constraints(te);if(!Cyc_Tcenv_all_labels_resolved(te))({
void*_tmp91[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp92="function has goto statements to undefined labels";
_tag_arr(_tmp92,sizeof(char),_get_zero_arr_size(_tmp92,49));}),_tag_arr(_tmp91,
sizeof(void*),0));});if(Cyc_Tc_is_main(fd->name)){{void*_tmp93=Cyc_Tcutil_compress((
void*)fd->ret_type);void*_tmp94;_LL81: if((int)_tmp93 != 0)goto _LL83;_LL82:({void*
_tmp95[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp96="main declared with return type void";
_tag_arr(_tmp96,sizeof(char),_get_zero_arr_size(_tmp96,36));}),_tag_arr(_tmp95,
sizeof(void*),0));});goto _LL80;_LL83: if(_tmp93 <= (void*)3?1:*((int*)_tmp93)!= 5)
goto _LL85;_tmp94=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp93)->f2;if((int)
_tmp94 != 2)goto _LL85;_LL84: goto _LL80;_LL85:;_LL86:({struct Cyc_String_pa_struct
_tmp99;_tmp99.tag=0;_tmp99.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)fd->ret_type));{void*_tmp97[1]={& _tmp99};Cyc_Tcutil_terr(loc,({const char*
_tmp98="main declared with return type %s instead of int or void";_tag_arr(
_tmp98,sizeof(char),_get_zero_arr_size(_tmp98,57));}),_tag_arr(_tmp97,sizeof(
void*),1));}});goto _LL80;_LL80:;}if(fd->c_varargs?1: fd->cyc_varargs != 0)({void*
_tmp9A[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp9B="main declared with varargs";
_tag_arr(_tmp9B,sizeof(char),_get_zero_arr_size(_tmp9B,27));}),_tag_arr(_tmp9A,
sizeof(void*),0));});{struct Cyc_List_List*_tmp9C=fd->args;if(_tmp9C != 0){struct
_tuple7 _tmp9E;void*_tmp9F;struct _tuple7*_tmp9D=(struct _tuple7*)_tmp9C->hd;_tmp9E=*
_tmp9D;_tmp9F=_tmp9E.f3;{void*_tmpA0=Cyc_Tcutil_compress(_tmp9F);void*_tmpA1;
_LL88: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 5)goto _LL8A;_tmpA1=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpA0)->f2;if((int)_tmpA1 != 2)goto _LL8A;_LL89:
goto _LL87;_LL8A:;_LL8B:({struct Cyc_String_pa_struct _tmpA4;_tmpA4.tag=0;_tmpA4.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp9F));{void*
_tmpA2[1]={& _tmpA4};Cyc_Tcutil_terr(loc,({const char*_tmpA3="main declared with first argument of type %s instead of int";
_tag_arr(_tmpA3,sizeof(char),_get_zero_arr_size(_tmpA3,60));}),_tag_arr(_tmpA2,
sizeof(void*),1));}});goto _LL87;_LL87:;}_tmp9C=_tmp9C->tl;if(_tmp9C != 0){struct
_tuple7 _tmpA6;void*_tmpA7;struct _tuple7*_tmpA5=(struct _tuple7*)_tmp9C->hd;_tmpA6=*
_tmpA5;_tmpA7=_tmpA6.f3;_tmp9C=_tmp9C->tl;if(_tmp9C != 0)({void*_tmpA8[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmpA9="main declared with too many arguments";_tag_arr(_tmpA9,
sizeof(char),_get_zero_arr_size(_tmpA9,38));}),_tag_arr(_tmpA8,sizeof(void*),0));});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmpB5=_cycalloc(sizeof(*_tmpB5));
_tmpB5->v=fd->tvs;_tmpB5;});if(((!Cyc_Tcutil_unify(_tmpA7,Cyc_Absyn_tagged_typ(
Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpAA=_cycalloc(
sizeof(*_tmpAA));_tmpAA->v=(void*)((void*)3);_tmpAA;}),tvs)),Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->v=(void*)((void*)3);
_tmpAB;}),tvs),Cyc_Absyn_empty_tqual(),((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)()))?
!Cyc_Tcutil_unify(_tmpA7,Cyc_Absyn_tagged_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->v=(void*)((void*)3);
_tmpAC;}),tvs)),Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpAD=_cycalloc(sizeof(*
_tmpAD));_tmpAD->v=(void*)((void*)3);_tmpAD;}),tvs),Cyc_Absyn_empty_tqual(),((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)())): 0)?!Cyc_Tcutil_unify(
_tmpA7,Cyc_Absyn_tagged_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->v=(void*)((void*)3);_tmpAE;}),tvs)),
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->v=(
void*)((void*)3);_tmpAF;}),tvs),Cyc_Absyn_const_tqual(),((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)())): 0)?!Cyc_Tcutil_unify(_tmpA7,Cyc_Absyn_tagged_typ(Cyc_Absyn_const_string_typ(
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->v=(
void*)((void*)3);_tmpB0;}),tvs)),Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpB1=
_cycalloc(sizeof(*_tmpB1));_tmpB1->v=(void*)((void*)3);_tmpB1;}),tvs),Cyc_Absyn_const_tqual(),((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)())): 0)({struct Cyc_String_pa_struct
_tmpB4;_tmpB4.tag=0;_tmpB4.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmpA7));{void*_tmpB2[1]={& _tmpB4};Cyc_Tcutil_terr(loc,({const char*_tmpB3="second argument of main has type %s instead of char??";
_tag_arr(_tmpB3,sizeof(char),_get_zero_arr_size(_tmpB3,54));}),_tag_arr(_tmpB2,
sizeof(void*),1));}});}}}}}te->le=0;}}static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*
td){struct _tagged_arr*v=(*td->name).f2;{void*_tmpB6=(*td->name).f1;struct Cyc_List_List*
_tmpB7;struct Cyc_List_List*_tmpB8;_LL8D: if(_tmpB6 <= (void*)1?1:*((int*)_tmpB6)!= 
0)goto _LL8F;_tmpB7=((struct Cyc_Absyn_Rel_n_struct*)_tmpB6)->f1;if(_tmpB7 != 0)
goto _LL8F;_LL8E: goto _LL90;_LL8F: if(_tmpB6 <= (void*)1?1:*((int*)_tmpB6)!= 1)goto
_LL91;_tmpB8=((struct Cyc_Absyn_Abs_n_struct*)_tmpB6)->f1;if(_tmpB8 != 0)goto _LL91;
_LL90: goto _LL8C;_LL91:;_LL92:({struct Cyc_String_pa_struct _tmpBB;_tmpBB.tag=0;
_tmpBB.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(td->name));{
void*_tmpB9[1]={& _tmpBB};Cyc_Tcutil_terr(loc,({const char*_tmpBA="qualified typedef declarations are not implemented (%s)";
_tag_arr(_tmpBA,sizeof(char),_get_zero_arr_size(_tmpBA,56));}),_tag_arr(_tmpB9,
sizeof(void*),1));}});return;_LL8C:;}if(((int(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_member)(ge->typedefs,v)){({struct Cyc_String_pa_struct
_tmpBE;_tmpBE.tag=0;_tmpBE.f1=(struct _tagged_arr)((struct _tagged_arr)*v);{void*
_tmpBC[1]={& _tmpBE};Cyc_Tcutil_terr(loc,({const char*_tmpBD="redeclaration of typedef %s";
_tag_arr(_tmpBD,sizeof(char),_get_zero_arr_size(_tmpBD,28));}),_tag_arr(_tmpBC,
sizeof(void*),1));}});return;}(*td->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_Abs_n_struct _tmpC0;
_tmpC0.tag=1;_tmpC0.f1=te->ns;_tmpC0;});_tmpBF;});Cyc_Tcutil_check_unique_tvars(
loc,td->tvs);Cyc_Tcutil_add_tvar_identities(td->tvs);if(td->defn != 0)Cyc_Tcutil_check_type(
loc,te,td->tvs,(void*)0,0,(void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);
else{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){void*_tmpC1=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);struct Cyc_Core_Opt*_tmpC2;struct
Cyc_Core_Opt**_tmpC3;struct Cyc_Core_Opt*_tmpC4;struct Cyc_Core_Opt**_tmpC5;void*
_tmpC6;_LL94: if(*((int*)_tmpC1)!= 1)goto _LL96;_tmpC2=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC1)->f1;_tmpC3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC1)->f1;_LL95:*_tmpC3=({struct Cyc_Core_Opt*_tmpC7=_cycalloc(sizeof(*_tmpC7));
_tmpC7->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmpC8=_cycalloc(sizeof(*
_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpC9;_tmpC9.tag=0;_tmpC9.f1=(
void*)((void*)2);_tmpC9;});_tmpC8;}));_tmpC7;});goto _LL93;_LL96: if(*((int*)
_tmpC1)!= 2)goto _LL98;_tmpC4=((struct Cyc_Absyn_Less_kb_struct*)_tmpC1)->f1;
_tmpC5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpC1)->f1;
_tmpC6=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpC1)->f2;_LL97:*_tmpC5=({
struct Cyc_Core_Opt*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->v=(void*)((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmpCC;_tmpCC.tag=0;_tmpCC.f1=(void*)_tmpC6;_tmpCC;});
_tmpCB;}));_tmpCA;});goto _LL93;_LL98:;_LL99: continue;_LL93:;}}ge->typedefs=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Typedefdecl*
v))Cyc_Dict_insert)(ge->typedefs,v,td);}static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){struct _RegionHandle
_tmpCD=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpCD;
_push_region(uprev_rgn);for(0;rpo != 0;rpo=rpo->tl){struct _tuple3 _tmpCF;void*
_tmpD0;void*_tmpD1;struct _tuple3*_tmpCE=(struct _tuple3*)rpo->hd;_tmpCF=*_tmpCE;
_tmpD0=_tmpCF.f1;_tmpD1=_tmpCF.f2;Cyc_Tcutil_check_type(loc,te,tvs,(void*)4,0,
_tmpD0);Cyc_Tcutil_check_type(loc,te,tvs,(void*)3,0,_tmpD1);}{struct Cyc_List_List*
prev_fields=0;struct Cyc_List_List*_tmpD2=fields;for(0;_tmpD2 != 0;_tmpD2=_tmpD2->tl){
struct Cyc_Absyn_Aggrfield _tmpD4;struct _tagged_arr*_tmpD5;struct Cyc_Absyn_Tqual
_tmpD6;void*_tmpD7;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_List_List*_tmpD9;struct
Cyc_Absyn_Aggrfield*_tmpD3=(struct Cyc_Absyn_Aggrfield*)_tmpD2->hd;_tmpD4=*_tmpD3;
_tmpD5=_tmpD4.name;_tmpD6=_tmpD4.tq;_tmpD7=(void*)_tmpD4.type;_tmpD8=_tmpD4.width;
_tmpD9=_tmpD4.attributes;if(((int(*)(int(*compare)(struct _tagged_arr*,struct
_tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_strptrcmp,
prev_fields,_tmpD5))({struct Cyc_String_pa_struct _tmpDC;_tmpDC.tag=0;_tmpDC.f1=(
struct _tagged_arr)((struct _tagged_arr)*_tmpD5);{void*_tmpDA[1]={& _tmpDC};Cyc_Tcutil_terr(
loc,({const char*_tmpDB="duplicate field %s";_tag_arr(_tmpDB,sizeof(char),
_get_zero_arr_size(_tmpDB,19));}),_tag_arr(_tmpDA,sizeof(void*),1));}});if(Cyc_strcmp((
struct _tagged_arr)*_tmpD5,({const char*_tmpDD="";_tag_arr(_tmpDD,sizeof(char),
_get_zero_arr_size(_tmpDD,1));}))!= 0)prev_fields=({struct Cyc_List_List*_tmpDE=
_region_malloc(uprev_rgn,sizeof(*_tmpDE));_tmpDE->hd=_tmpD5;_tmpDE->tl=
prev_fields;_tmpDE;});Cyc_Tcutil_check_type(loc,te,tvs,(void*)1,0,_tmpD7);Cyc_Tcutil_check_bitfield(
loc,te,_tmpD7,_tmpD8,_tmpD5);}};_pop_region(uprev_rgn);}struct _tuple8{struct Cyc_Absyn_AggrdeclImpl*
f1;struct Cyc_Core_Opt*f2;};void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tagged_arr*_tmpDF=(*ad->name).f2;{struct Cyc_List_List*atts=ad->attributes;
for(0;atts != 0;atts=atts->tl){void*_tmpE0=(void*)atts->hd;_LL9B: if((int)_tmpE0 != 
5)goto _LL9D;_LL9C: goto _LL9E;_LL9D: if(_tmpE0 <= (void*)17?1:*((int*)_tmpE0)!= 1)
goto _LL9F;_LL9E: continue;_LL9F:;_LLA0:({struct Cyc_String_pa_struct _tmpE4;_tmpE4.tag=
0;_tmpE4.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmpDF);{struct Cyc_String_pa_struct
_tmpE3;_tmpE3.tag=0;_tmpE3.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd));{void*_tmpE1[2]={& _tmpE3,& _tmpE4};Cyc_Tcutil_terr(loc,({const
char*_tmpE2="bad attribute %s in  %s definition";_tag_arr(_tmpE2,sizeof(char),
_get_zero_arr_size(_tmpE2,35));}),_tag_arr(_tmpE1,sizeof(void*),2));}}});goto
_LL9A;_LL9A:;}}{struct Cyc_List_List*_tmpE5=ad->tvs;{struct Cyc_List_List*tvs2=
_tmpE5;for(0;tvs2 != 0;tvs2=tvs2->tl){void*_tmpE6=Cyc_Absyn_compress_kb((void*)((
struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmpE7;struct Cyc_Core_Opt**
_tmpE8;struct Cyc_Core_Opt*_tmpE9;struct Cyc_Core_Opt**_tmpEA;void*_tmpEB;struct
Cyc_Core_Opt*_tmpEC;struct Cyc_Core_Opt**_tmpED;void*_tmpEE;void*_tmpEF;_LLA2: if(*((
int*)_tmpE6)!= 1)goto _LLA4;_tmpE7=((struct Cyc_Absyn_Unknown_kb_struct*)_tmpE6)->f1;
_tmpE8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmpE6)->f1;
_LLA3: _tmpEA=_tmpE8;goto _LLA5;_LLA4: if(*((int*)_tmpE6)!= 2)goto _LLA6;_tmpE9=((
struct Cyc_Absyn_Less_kb_struct*)_tmpE6)->f1;_tmpEA=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmpE6)->f1;_tmpEB=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpE6)->f2;if((int)_tmpEB != 1)goto _LLA6;_LLA5: _tmpED=_tmpEA;goto _LLA7;_LLA6: if(*((
int*)_tmpE6)!= 2)goto _LLA8;_tmpEC=((struct Cyc_Absyn_Less_kb_struct*)_tmpE6)->f1;
_tmpED=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpE6)->f1;
_tmpEE=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpE6)->f2;if((int)_tmpEE != 0)
goto _LLA8;_LLA7:*_tmpED=({struct Cyc_Core_Opt*_tmpF0=_cycalloc(sizeof(*_tmpF0));
_tmpF0->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmpF1=_cycalloc(sizeof(*
_tmpF1));_tmpF1[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpF2;_tmpF2.tag=0;_tmpF2.f1=(
void*)((void*)2);_tmpF2;});_tmpF1;}));_tmpF0;});continue;_LLA8: if(*((int*)_tmpE6)
!= 0)goto _LLAA;_tmpEF=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpE6)->f1;if((int)
_tmpEF != 1)goto _LLAA;_LLA9:({struct Cyc_String_pa_struct _tmpF6;_tmpF6.tag=0;
_tmpF6.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);{
struct Cyc_String_pa_struct _tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmpDF);{void*_tmpF3[2]={& _tmpF5,& _tmpF6};Cyc_Tcutil_terr(loc,({
const char*_tmpF4="type %s attempts to abstract type variable %s of kind M";
_tag_arr(_tmpF4,sizeof(char),_get_zero_arr_size(_tmpF4,56));}),_tag_arr(_tmpF3,
sizeof(void*),2));}}});continue;_LLAA:;_LLAB: continue;_LLA1:;}}{void*_tmpF7=(*ad->name).f1;
struct Cyc_List_List*_tmpF8;struct Cyc_List_List*_tmpF9;_LLAD: if(_tmpF7 <= (void*)1?
1:*((int*)_tmpF7)!= 0)goto _LLAF;_tmpF8=((struct Cyc_Absyn_Rel_n_struct*)_tmpF7)->f1;
if(_tmpF8 != 0)goto _LLAF;_LLAE: goto _LLB0;_LLAF: if(_tmpF7 <= (void*)1?1:*((int*)
_tmpF7)!= 1)goto _LLB1;_tmpF9=((struct Cyc_Absyn_Abs_n_struct*)_tmpF7)->f1;if(
_tmpF9 != 0)goto _LLB1;_LLB0: goto _LLAC;_LLB1:;_LLB2:({struct Cyc_String_pa_struct
_tmpFC;_tmpFC.tag=0;_tmpFC.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
ad->name));{void*_tmpFA[1]={& _tmpFC};Cyc_Tcutil_terr(loc,({const char*_tmpFB="qualified struct declarations are not implemented (%s)";
_tag_arr(_tmpFB,sizeof(char),_get_zero_arr_size(_tmpFB,55));}),_tag_arr(_tmpFA,
sizeof(void*),1));}});return;_LLAC:;}(*ad->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({struct Cyc_Absyn_Abs_n_struct _tmpFE;
_tmpFE.tag=1;_tmpFE.f1=te->ns;_tmpFE;});_tmpFD;});Cyc_Tcutil_check_unique_tvars(
loc,ad->tvs);Cyc_Tcutil_add_tvar_identities(ad->tvs);{struct _tuple8 _tmp100=({
struct _tuple8 _tmpFF;_tmpFF.f1=ad->impl;_tmpFF.f2=((struct Cyc_Core_Opt*(*)(struct
Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,_tmpDF);
_tmpFF;});struct Cyc_Absyn_AggrdeclImpl*_tmp101;struct Cyc_Core_Opt*_tmp102;struct
Cyc_Absyn_AggrdeclImpl*_tmp103;struct Cyc_Absyn_AggrdeclImpl _tmp104;struct Cyc_List_List*
_tmp105;struct Cyc_List_List*_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_Core_Opt*
_tmp108;struct Cyc_Absyn_AggrdeclImpl*_tmp109;struct Cyc_Absyn_AggrdeclImpl _tmp10A;
struct Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10C;struct Cyc_List_List*
_tmp10D;struct Cyc_Core_Opt*_tmp10E;struct Cyc_Core_Opt _tmp10F;struct Cyc_Absyn_Aggrdecl**
_tmp110;struct Cyc_Absyn_AggrdeclImpl*_tmp111;struct Cyc_Core_Opt*_tmp112;struct
Cyc_Core_Opt _tmp113;struct Cyc_Absyn_Aggrdecl**_tmp114;_LLB4: _tmp101=_tmp100.f1;
if(_tmp101 != 0)goto _LLB6;_tmp102=_tmp100.f2;if(_tmp102 != 0)goto _LLB6;_LLB5: ge->aggrdecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl**
v))Cyc_Dict_insert)(ge->aggrdecls,_tmpDF,({struct Cyc_Absyn_Aggrdecl**_tmp115=
_cycalloc(sizeof(*_tmp115));_tmp115[0]=ad;_tmp115;}));goto _LLB3;_LLB6: _tmp103=
_tmp100.f1;if(_tmp103 == 0)goto _LLB8;_tmp104=*_tmp103;_tmp105=_tmp104.exist_vars;
_tmp106=_tmp104.rgn_po;_tmp107=_tmp104.fields;_tmp108=_tmp100.f2;if(_tmp108 != 0)
goto _LLB8;_LLB7: {struct Cyc_Absyn_Aggrdecl**_tmp116=({struct Cyc_Absyn_Aggrdecl**
_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C[0]=({struct Cyc_Absyn_Aggrdecl*
_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->kind=(void*)((void*)ad->kind);
_tmp11D->sc=(void*)((void*)3);_tmp11D->name=ad->name;_tmp11D->tvs=_tmpE5;_tmp11D->impl=
0;_tmp11D->attributes=ad->attributes;_tmp11D;});_tmp11C;});ge->aggrdecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl**
v))Cyc_Dict_insert)(ge->aggrdecls,_tmpDF,_tmp116);Cyc_Tcutil_check_unique_tvars(
loc,_tmp105);Cyc_Tcutil_add_tvar_identities(_tmp105);Cyc_Tc_tcAggrImpl(te,ge,loc,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmpE5,_tmp105),_tmp106,_tmp107);if((void*)ad->kind == (void*)1){struct Cyc_List_List*
f=_tmp107;for(0;f != 0;f=f->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
f->hd)->type))({struct Cyc_String_pa_struct _tmp11B;_tmp11B.tag=0;_tmp11B.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Absyn_Aggrfield*)
f->hd)->type));{struct Cyc_String_pa_struct _tmp11A;_tmp11A.tag=0;_tmp11A.f1=(
struct _tagged_arr)((struct _tagged_arr)*_tmpDF);{struct Cyc_String_pa_struct
_tmp119;_tmp119.tag=0;_tmp119.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct Cyc_Absyn_Aggrfield*)f->hd)->name);{void*_tmp117[3]={& _tmp119,& _tmp11A,&
_tmp11B};Cyc_Tcutil_terr(loc,({const char*_tmp118="field %s of union %s has type %s which is not `bits-only'";
_tag_arr(_tmp118,sizeof(char),_get_zero_arr_size(_tmp118,58));}),_tag_arr(
_tmp117,sizeof(void*),3));}}}});}}*_tmp116=ad;goto _LLB3;}_LLB8: _tmp109=_tmp100.f1;
if(_tmp109 == 0)goto _LLBA;_tmp10A=*_tmp109;_tmp10B=_tmp10A.exist_vars;_tmp10C=
_tmp10A.rgn_po;_tmp10D=_tmp10A.fields;_tmp10E=_tmp100.f2;if(_tmp10E == 0)goto
_LLBA;_tmp10F=*_tmp10E;_tmp110=(struct Cyc_Absyn_Aggrdecl**)_tmp10F.v;_LLB9: if((
void*)ad->kind != (void*)(*_tmp110)->kind)({void*_tmp11E[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp11F="cannot reuse struct names for unions and vice-versa";
_tag_arr(_tmp11F,sizeof(char),_get_zero_arr_size(_tmp11F,52));}),_tag_arr(
_tmp11E,sizeof(void*),0));});{struct Cyc_Absyn_Aggrdecl*_tmp120=*_tmp110;*_tmp110=({
struct Cyc_Absyn_Aggrdecl*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->kind=(void*)((
void*)ad->kind);_tmp121->sc=(void*)((void*)3);_tmp121->name=ad->name;_tmp121->tvs=
_tmpE5;_tmp121->impl=0;_tmp121->attributes=ad->attributes;_tmp121;});Cyc_Tcutil_check_unique_tvars(
loc,_tmp10B);Cyc_Tcutil_add_tvar_identities(_tmp10B);Cyc_Tc_tcAggrImpl(te,ge,loc,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmpE5,_tmp10B),_tmp10C,_tmp10D);*_tmp110=_tmp120;_tmp114=_tmp110;goto _LLBB;}
_LLBA: _tmp111=_tmp100.f1;if(_tmp111 != 0)goto _LLB3;_tmp112=_tmp100.f2;if(_tmp112
== 0)goto _LLB3;_tmp113=*_tmp112;_tmp114=(struct Cyc_Absyn_Aggrdecl**)_tmp113.v;
_LLBB: {struct Cyc_Absyn_Aggrdecl*_tmp122=Cyc_Tcdecl_merge_aggrdecl(*_tmp114,ad,
loc,Cyc_Tc_tc_msg);if(_tmp122 == 0)return;else{*_tmp114=(struct Cyc_Absyn_Aggrdecl*)
_tmp122;ad=(struct Cyc_Absyn_Aggrdecl*)_tmp122;goto _LLB3;}}_LLB3:;}ge->ordinaries=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple6*
v))Cyc_Dict_insert)(ge->ordinaries,_tmpDF,({struct _tuple6*_tmp123=_cycalloc(
sizeof(*_tmp123));_tmp123->f1=(void*)({struct Cyc_Tcenv_AggrRes_struct*_tmp124=
_cycalloc(sizeof(*_tmp124));_tmp124[0]=({struct Cyc_Tcenv_AggrRes_struct _tmp125;
_tmp125.tag=1;_tmp125.f1=ad;_tmp125;});_tmp124;});_tmp123->f2=1;_tmp123;}));}}
struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_List_List*Cyc_Tc_tcTunionFields(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct _tagged_arr obj,int is_xtunion,struct _tuple0*name,struct Cyc_List_List*fields,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tuniondecl*tudres){{struct Cyc_List_List*
_tmp126=fields;for(0;_tmp126 != 0;_tmp126=_tmp126->tl){struct Cyc_Absyn_Tunionfield*
_tmp127=(struct Cyc_Absyn_Tunionfield*)_tmp126->hd;{struct Cyc_List_List*typs=
_tmp127->typs;for(0;typs != 0;typs=typs->tl){Cyc_Tcutil_check_type(_tmp127->loc,
te,tvs,(void*)1,0,(*((struct _tuple9*)typs->hd)).f2);}}{void*_tmp128=(*_tmp127->name).f1;
struct Cyc_List_List*_tmp129;_LLBD: if(_tmp128 <= (void*)1?1:*((int*)_tmp128)!= 0)
goto _LLBF;_tmp129=((struct Cyc_Absyn_Rel_n_struct*)_tmp128)->f1;if(_tmp129 != 0)
goto _LLBF;_LLBE: if(is_xtunion)(*_tmp127->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp12B;_tmp12B.tag=1;_tmp12B.f1=te->ns;_tmp12B;});_tmp12A;});else{(*_tmp127->name).f1=(*
name).f1;}goto _LLBC;_LLBF: if(_tmp128 <= (void*)1?1:*((int*)_tmp128)!= 0)goto _LLC1;
_LLC0:({struct Cyc_String_pa_struct _tmp12E;_tmp12E.tag=0;_tmp12E.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp127->name));{void*
_tmp12C[1]={& _tmp12E};Cyc_Tcutil_terr(_tmp127->loc,({const char*_tmp12D="qualified tunionfield declarations are not allowed (%s)";
_tag_arr(_tmp12D,sizeof(char),_get_zero_arr_size(_tmp12D,56));}),_tag_arr(
_tmp12C,sizeof(void*),1));}});goto _LLBC;_LLC1: if(_tmp128 <= (void*)1?1:*((int*)
_tmp128)!= 1)goto _LLC3;_LLC2: goto _LLBC;_LLC3: if((int)_tmp128 != 0)goto _LLBC;_LLC4:({
void*_tmp12F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp130="tcTunionFields: Loc_n";_tag_arr(_tmp130,sizeof(char),
_get_zero_arr_size(_tmp130,22));}),_tag_arr(_tmp12F,sizeof(void*),0));});_LLBC:;}}}{
struct Cyc_List_List*fields2;if(is_xtunion){int _tmp131=1;struct Cyc_List_List*
_tmp132=Cyc_Tcdecl_sort_xtunion_fields(fields,& _tmp131,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp131)fields2=_tmp132;else{fields2=0;}}else{struct _RegionHandle _tmp133=
_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp133;_push_region(
uprev_rgn);{struct Cyc_List_List*prev_fields=0;{struct Cyc_List_List*fs=fields;
for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Tunionfield*_tmp134=(struct Cyc_Absyn_Tunionfield*)
fs->hd;if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*
_tmp134->name).f2))({struct Cyc_String_pa_struct _tmp138;_tmp138.tag=0;_tmp138.f1=(
struct _tagged_arr)((struct _tagged_arr)obj);{struct Cyc_String_pa_struct _tmp137;
_tmp137.tag=0;_tmp137.f1=(struct _tagged_arr)((struct _tagged_arr)*(*_tmp134->name).f2);{
void*_tmp135[2]={& _tmp137,& _tmp138};Cyc_Tcutil_terr(_tmp134->loc,({const char*
_tmp136="duplicate field name %s in %s";_tag_arr(_tmp136,sizeof(char),
_get_zero_arr_size(_tmp136,30));}),_tag_arr(_tmp135,sizeof(void*),2));}}});else{
prev_fields=({struct Cyc_List_List*_tmp139=_region_malloc(uprev_rgn,sizeof(*
_tmp139));_tmp139->hd=(*_tmp134->name).f2;_tmp139->tl=prev_fields;_tmp139;});}
if((void*)_tmp134->sc != (void*)2){({struct Cyc_String_pa_struct _tmp13C;_tmp13C.tag=
0;_tmp13C.f1=(struct _tagged_arr)((struct _tagged_arr)*(*_tmp134->name).f2);{void*
_tmp13A[1]={& _tmp13C};Cyc_Tcutil_warn(loc,({const char*_tmp13B="ignoring scope of field %s";
_tag_arr(_tmp13B,sizeof(char),_get_zero_arr_size(_tmp13B,27));}),_tag_arr(
_tmp13A,sizeof(void*),1));}});(void*)(_tmp134->sc=(void*)((void*)2));}}}fields2=
fields;};_pop_region(uprev_rgn);}{struct Cyc_List_List*_tmp13D=fields;for(0;
_tmp13D != 0;_tmp13D=_tmp13D->tl){struct Cyc_Absyn_Tunionfield*_tmp13E=(struct Cyc_Absyn_Tunionfield*)
_tmp13D->hd;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct _tuple6*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp13E->name).f2,({
struct _tuple6*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->f1=(void*)({struct Cyc_Tcenv_TunionRes_struct*
_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140[0]=({struct Cyc_Tcenv_TunionRes_struct
_tmp141;_tmp141.tag=2;_tmp141.f1=tudres;_tmp141.f2=_tmp13E;_tmp141;});_tmp140;});
_tmp13F->f2=1;_tmp13F;}));}}return fields2;}}struct _tuple10{struct Cyc_Core_Opt*f1;
struct Cyc_Core_Opt*f2;};void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl*tud){
struct _tagged_arr*v=(*tud->name).f2;struct _tagged_arr obj=tud->is_xtunion?({const
char*_tmp180="xtunion";_tag_arr(_tmp180,sizeof(char),_get_zero_arr_size(_tmp180,
8));}):({const char*_tmp181="tunion";_tag_arr(_tmp181,sizeof(char),
_get_zero_arr_size(_tmp181,7));});struct Cyc_List_List*tvs=tud->tvs;{struct Cyc_List_List*
tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){void*_tmp142=Cyc_Absyn_compress_kb((void*)((
struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp143;struct Cyc_Core_Opt**
_tmp144;struct Cyc_Core_Opt*_tmp145;struct Cyc_Core_Opt**_tmp146;void*_tmp147;
struct Cyc_Core_Opt*_tmp148;struct Cyc_Core_Opt**_tmp149;void*_tmp14A;void*_tmp14B;
_LLC6: if(*((int*)_tmp142)!= 1)goto _LLC8;_tmp143=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp142)->f1;_tmp144=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp142)->f1;_LLC7: _tmp146=_tmp144;goto _LLC9;_LLC8: if(*((int*)_tmp142)!= 2)goto
_LLCA;_tmp145=((struct Cyc_Absyn_Less_kb_struct*)_tmp142)->f1;_tmp146=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp142)->f1;_tmp147=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp142)->f2;if((int)_tmp147 != 1)goto _LLCA;_LLC9: _tmp149=_tmp146;goto _LLCB;_LLCA:
if(*((int*)_tmp142)!= 2)goto _LLCC;_tmp148=((struct Cyc_Absyn_Less_kb_struct*)
_tmp142)->f1;_tmp149=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp142)->f1;_tmp14A=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp142)->f2;if((
int)_tmp14A != 0)goto _LLCC;_LLCB:*_tmp149=({struct Cyc_Core_Opt*_tmp14C=_cycalloc(
sizeof(*_tmp14C));_tmp14C->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp14E;_tmp14E.tag=0;_tmp14E.f1=(void*)((void*)2);_tmp14E;});_tmp14D;}));
_tmp14C;});goto _LLC5;_LLCC: if(*((int*)_tmp142)!= 0)goto _LLCE;_tmp14B=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp142)->f1;if((int)_tmp14B != 1)goto _LLCE;_LLCD:({
struct Cyc_String_pa_struct _tmp153;_tmp153.tag=0;_tmp153.f1=(struct _tagged_arr)((
struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);{struct Cyc_String_pa_struct
_tmp152;_tmp152.tag=0;_tmp152.f1=(struct _tagged_arr)((struct _tagged_arr)*v);{
struct Cyc_String_pa_struct _tmp151;_tmp151.tag=0;_tmp151.f1=(struct _tagged_arr)((
struct _tagged_arr)obj);{void*_tmp14F[3]={& _tmp151,& _tmp152,& _tmp153};Cyc_Tcutil_terr(
loc,({const char*_tmp150="%s %s attempts to abstract type variable %s of kind M";
_tag_arr(_tmp150,sizeof(char),_get_zero_arr_size(_tmp150,54));}),_tag_arr(
_tmp14F,sizeof(void*),3));}}}});goto _LLC5;_LLCE:;_LLCF: goto _LLC5;_LLC5:;}}Cyc_Tcutil_check_unique_tvars(
loc,tvs);Cyc_Tcutil_add_tvar_identities(tvs);{struct Cyc_Core_Opt*tud_opt;if(tud->is_xtunion){{
struct _handler_cons _tmp154;_push_handler(& _tmp154);{int _tmp156=0;if(setjmp(
_tmp154.handler))_tmp156=1;if(!_tmp156){tud_opt=Cyc_Tcenv_lookup_xtuniondecl(te,
loc,tud->name);;_pop_handler();}else{void*_tmp155=(void*)_exn_thrown;void*
_tmp158=_tmp155;_LLD1: if(_tmp158 != Cyc_Dict_Absent)goto _LLD3;_LLD2:({struct Cyc_String_pa_struct
_tmp15B;_tmp15B.tag=0;_tmp15B.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
tud->name));{void*_tmp159[1]={& _tmp15B};Cyc_Tcutil_terr(loc,({const char*_tmp15A="qualified xtunion declaration %s is not an existing xtunion";
_tag_arr(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,60));}),_tag_arr(
_tmp159,sizeof(void*),1));}});return;_LLD3:;_LLD4:(void)_throw(_tmp158);_LLD0:;}}}
if(tud_opt != 0)tud->name=(*((struct Cyc_Absyn_Tuniondecl**)tud_opt->v))->name;
else{(*tud->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp15C=_cycalloc(
sizeof(*_tmp15C));_tmp15C[0]=({struct Cyc_Absyn_Abs_n_struct _tmp15D;_tmp15D.tag=1;
_tmp15D.f1=te->ns;_tmp15D;});_tmp15C;});}}else{{void*_tmp15E=(*tud->name).f1;
struct Cyc_List_List*_tmp15F;_LLD6: if(_tmp15E <= (void*)1?1:*((int*)_tmp15E)!= 0)
goto _LLD8;_tmp15F=((struct Cyc_Absyn_Rel_n_struct*)_tmp15E)->f1;if(_tmp15F != 0)
goto _LLD8;_LLD7:(*tud->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp160=
_cycalloc(sizeof(*_tmp160));_tmp160[0]=({struct Cyc_Absyn_Abs_n_struct _tmp161;
_tmp161.tag=1;_tmp161.f1=te->ns;_tmp161;});_tmp160;});goto _LLD5;_LLD8: if(_tmp15E
<= (void*)1?1:*((int*)_tmp15E)!= 1)goto _LLDA;_LLD9: goto _LLDB;_LLDA:;_LLDB:({
struct Cyc_String_pa_struct _tmp164;_tmp164.tag=0;_tmp164.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(tud->name));{void*_tmp162[1]={& _tmp164};
Cyc_Tcutil_terr(loc,({const char*_tmp163="qualified tunion declarations are not implemented (%s)";
_tag_arr(_tmp163,sizeof(char),_get_zero_arr_size(_tmp163,55));}),_tag_arr(
_tmp162,sizeof(void*),1));}});return;_LLD5:;}tud_opt=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)(ge->tuniondecls,
v);}{struct _tuple10 _tmp166=({struct _tuple10 _tmp165;_tmp165.f1=tud->fields;
_tmp165.f2=tud_opt;_tmp165;});struct Cyc_Core_Opt*_tmp167;struct Cyc_Core_Opt*
_tmp168;struct Cyc_Core_Opt*_tmp169;struct Cyc_Core_Opt _tmp16A;struct Cyc_List_List*
_tmp16B;struct Cyc_List_List**_tmp16C;struct Cyc_Core_Opt*_tmp16D;struct Cyc_Core_Opt*
_tmp16E;struct Cyc_Core_Opt _tmp16F;struct Cyc_List_List*_tmp170;struct Cyc_List_List**
_tmp171;struct Cyc_Core_Opt*_tmp172;struct Cyc_Core_Opt _tmp173;struct Cyc_Absyn_Tuniondecl**
_tmp174;struct Cyc_Core_Opt*_tmp175;struct Cyc_Core_Opt*_tmp176;struct Cyc_Core_Opt
_tmp177;struct Cyc_Absyn_Tuniondecl**_tmp178;_LLDD: _tmp167=_tmp166.f1;if(_tmp167
!= 0)goto _LLDF;_tmp168=_tmp166.f2;if(_tmp168 != 0)goto _LLDF;_LLDE: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl**
v))Cyc_Dict_insert)(ge->tuniondecls,v,({struct Cyc_Absyn_Tuniondecl**_tmp179=
_cycalloc(sizeof(*_tmp179));_tmp179[0]=tud;_tmp179;}));goto _LLDC;_LLDF: _tmp169=
_tmp166.f1;if(_tmp169 == 0)goto _LLE1;_tmp16A=*_tmp169;_tmp16B=(struct Cyc_List_List*)
_tmp16A.v;_tmp16C=(struct Cyc_List_List**)&(*_tmp166.f1).v;_tmp16D=_tmp166.f2;if(
_tmp16D != 0)goto _LLE1;_LLE0: {struct Cyc_Absyn_Tuniondecl**_tmp17A=({struct Cyc_Absyn_Tuniondecl**
_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B[0]=({struct Cyc_Absyn_Tuniondecl*
_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->sc=(void*)((void*)3);_tmp17C->name=
tud->name;_tmp17C->tvs=tvs;_tmp17C->fields=0;_tmp17C->is_xtunion=tud->is_xtunion;
_tmp17C;});_tmp17B;});ge->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl**v))Cyc_Dict_insert)(ge->tuniondecls,
v,_tmp17A);*_tmp16C=Cyc_Tc_tcTunionFields(te,ge,loc,obj,tud->is_xtunion,tud->name,*
_tmp16C,tvs,tud);*_tmp17A=tud;goto _LLDC;}_LLE1: _tmp16E=_tmp166.f1;if(_tmp16E == 0)
goto _LLE3;_tmp16F=*_tmp16E;_tmp170=(struct Cyc_List_List*)_tmp16F.v;_tmp171=(
struct Cyc_List_List**)&(*_tmp166.f1).v;_tmp172=_tmp166.f2;if(_tmp172 == 0)goto
_LLE3;_tmp173=*_tmp172;_tmp174=(struct Cyc_Absyn_Tuniondecl**)_tmp173.v;_LLE2: {
struct Cyc_Absyn_Tuniondecl*_tmp17D=*_tmp174;*_tmp174=({struct Cyc_Absyn_Tuniondecl*
_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->sc=(void*)((void*)3);_tmp17E->name=
tud->name;_tmp17E->tvs=tvs;_tmp17E->fields=0;_tmp17E->is_xtunion=tud->is_xtunion;
_tmp17E;});*_tmp171=Cyc_Tc_tcTunionFields(te,ge,loc,obj,tud->is_xtunion,tud->name,*
_tmp171,tvs,tud);*_tmp174=_tmp17D;_tmp178=_tmp174;goto _LLE4;}_LLE3: _tmp175=
_tmp166.f1;if(_tmp175 != 0)goto _LLDC;_tmp176=_tmp166.f2;if(_tmp176 == 0)goto _LLDC;
_tmp177=*_tmp176;_tmp178=(struct Cyc_Absyn_Tuniondecl**)_tmp177.v;_LLE4: {struct
Cyc_Absyn_Tuniondecl*_tmp17F=Cyc_Tcdecl_merge_tuniondecl(*_tmp178,tud,loc,Cyc_Tc_tc_msg);
if(_tmp17F == 0)return;else{*_tmp178=(struct Cyc_Absyn_Tuniondecl*)_tmp17F;goto
_LLDC;}}_LLDC:;}}}void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Enumdecl*ed){struct _tagged_arr*
v=(*ed->name).f2;{void*_tmp182=(*ed->name).f1;struct Cyc_List_List*_tmp183;struct
Cyc_List_List*_tmp184;_LLE6: if(_tmp182 <= (void*)1?1:*((int*)_tmp182)!= 0)goto
_LLE8;_tmp183=((struct Cyc_Absyn_Rel_n_struct*)_tmp182)->f1;if(_tmp183 != 0)goto
_LLE8;_LLE7: goto _LLE9;_LLE8: if(_tmp182 <= (void*)1?1:*((int*)_tmp182)!= 1)goto
_LLEA;_tmp184=((struct Cyc_Absyn_Abs_n_struct*)_tmp182)->f1;if(_tmp184 != 0)goto
_LLEA;_LLE9: goto _LLE5;_LLEA:;_LLEB:({struct Cyc_String_pa_struct _tmp187;_tmp187.tag=
0;_tmp187.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(ed->name));{
void*_tmp185[1]={& _tmp187};Cyc_Tcutil_terr(loc,({const char*_tmp186="qualified enum declarations are not implemented (%s)";
_tag_arr(_tmp186,sizeof(char),_get_zero_arr_size(_tmp186,53));}),_tag_arr(
_tmp185,sizeof(void*),1));}});return;_LLE5:;}(*ed->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp189;_tmp189.tag=1;_tmp189.f1=te->ns;_tmp189;});_tmp188;});if(ed->fields != 0){
struct _RegionHandle _tmp18A=_new_region("uprev_rgn");struct _RegionHandle*
uprev_rgn=& _tmp18A;_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp18B=(struct Cyc_Absyn_Enumfield*)fs->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_strptrcmp,prev_fields,(*_tmp18B->name).f2))({struct Cyc_String_pa_struct
_tmp18E;_tmp18E.tag=0;_tmp18E.f1=(struct _tagged_arr)((struct _tagged_arr)*(*
_tmp18B->name).f2);{void*_tmp18C[1]={& _tmp18E};Cyc_Tcutil_terr(_tmp18B->loc,({
const char*_tmp18D="duplicate field name %s";_tag_arr(_tmp18D,sizeof(char),
_get_zero_arr_size(_tmp18D,24));}),_tag_arr(_tmp18C,sizeof(void*),1));}});else{
prev_fields=({struct Cyc_List_List*_tmp18F=_region_malloc(uprev_rgn,sizeof(*
_tmp18F));_tmp18F->hd=(*_tmp18B->name).f2;_tmp18F->tl=prev_fields;_tmp18F;});}
if(_tmp18B->tag == 0)_tmp18B->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp18B->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp18B->tag)))({struct Cyc_String_pa_struct _tmp193;_tmp193.tag=0;
_tmp193.f1=(struct _tagged_arr)((struct _tagged_arr)*(*_tmp18B->name).f2);{struct
Cyc_String_pa_struct _tmp192;_tmp192.tag=0;_tmp192.f1=(struct _tagged_arr)((struct
_tagged_arr)*v);{void*_tmp190[2]={& _tmp192,& _tmp193};Cyc_Tcutil_terr(loc,({const
char*_tmp191="enum %s, field %s: expression is not constant";_tag_arr(_tmp191,
sizeof(char),_get_zero_arr_size(_tmp191,46));}),_tag_arr(_tmp190,sizeof(void*),2));}}});}{
unsigned int _tmp195;int _tmp196;struct _tuple4 _tmp194=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp18B->tag));_tmp195=_tmp194.f1;_tmp196=
_tmp194.f2;if(!_tmp196)({void*_tmp197[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp198="Cyclone enum tags cannot use sizeof or offsetof";_tag_arr(_tmp198,
sizeof(char),_get_zero_arr_size(_tmp198,48));}),_tag_arr(_tmp197,sizeof(void*),0));});
tag_count=_tmp195 + 1;(*_tmp18B->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp19A;_tmp19A.tag=1;_tmp19A.f1=te->ns;_tmp19A;});_tmp199;});}}};_pop_region(
uprev_rgn);}{struct _handler_cons _tmp19B;_push_handler(& _tmp19B);{int _tmp19D=0;
if(setjmp(_tmp19B.handler))_tmp19D=1;if(!_tmp19D){{struct Cyc_Absyn_Enumdecl**
_tmp19E=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*
k))Cyc_Dict_lookup)(ge->enumdecls,v);struct Cyc_Absyn_Enumdecl*_tmp19F=Cyc_Tcdecl_merge_enumdecl(*
_tmp19E,ed,loc,Cyc_Tc_tc_msg);if(_tmp19F == 0){_npop_handler(0);return;}*_tmp19E=(
struct Cyc_Absyn_Enumdecl*)_tmp19F;};_pop_handler();}else{void*_tmp19C=(void*)
_exn_thrown;void*_tmp1A1=_tmp19C;_LLED: if(_tmp1A1 != Cyc_Dict_Absent)goto _LLEF;
_LLEE: {struct Cyc_Absyn_Enumdecl**_tmp1A2=({struct Cyc_Absyn_Enumdecl**_tmp1A3=
_cycalloc(sizeof(*_tmp1A3));_tmp1A3[0]=ed;_tmp1A3;});ge->enumdecls=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(
ge->enumdecls,v,_tmp1A2);goto _LLEC;}_LLEF:;_LLF0:(void)_throw(_tmp1A1);_LLEC:;}}}
if(ed->fields != 0){struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp1A4=(struct Cyc_Absyn_Enumfield*)fs->hd;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple6*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp1A4->name).f2,({struct _tuple6*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5->f1=(
void*)({struct Cyc_Tcenv_EnumRes_struct*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));
_tmp1A6[0]=({struct Cyc_Tcenv_EnumRes_struct _tmp1A7;_tmp1A7.tag=3;_tmp1A7.f1=ed;
_tmp1A7.f2=_tmp1A4;_tmp1A7;});_tmp1A6;});_tmp1A5->f2=1;_tmp1A5;}));}}}static int
Cyc_Tc_okay_externC(struct Cyc_Position_Segment*loc,void*sc){void*_tmp1A8=sc;
_LLF2: if((int)_tmp1A8 != 0)goto _LLF4;_LLF3:({void*_tmp1A9[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp1AA="static declaration nested within extern \"C\"";_tag_arr(
_tmp1AA,sizeof(char),_get_zero_arr_size(_tmp1AA,44));}),_tag_arr(_tmp1A9,sizeof(
void*),0));});return 0;_LLF4: if((int)_tmp1A8 != 1)goto _LLF6;_LLF5:({void*_tmp1AB[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp1AC="abstract declaration nested within extern \"C\"";
_tag_arr(_tmp1AC,sizeof(char),_get_zero_arr_size(_tmp1AC,46));}),_tag_arr(
_tmp1AB,sizeof(void*),0));});return 0;_LLF6: if((int)_tmp1A8 != 2)goto _LLF8;_LLF7:
goto _LLF9;_LLF8: if((int)_tmp1A8 != 5)goto _LLFA;_LLF9: goto _LLFB;_LLFA: if((int)
_tmp1A8 != 3)goto _LLFC;_LLFB: return 1;_LLFC: if((int)_tmp1A8 != 4)goto _LLF1;_LLFD:({
void*_tmp1AD[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp1AE="nested extern \"C\" declaration";
_tag_arr(_tmp1AE,sizeof(char),_get_zero_arr_size(_tmp1AE,30));}),_tag_arr(
_tmp1AD,sizeof(void*),0));});return 1;_LLF1:;}static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*ds0,int in_externC,int check_var_init){struct Cyc_Tcenv_Genv*
ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_List_List*last=0;struct Cyc_Dict_Dict*dict=Cyc_Tcgenrep_empty_typerep_dict();
struct Cyc_List_List*_tmp1AF=ds0;for(0;_tmp1AF != 0;(last=_tmp1AF,_tmp1AF=_tmp1AF->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1AF->hd;struct Cyc_Position_Segment*
loc=d->loc;void*_tmp1B0=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp1B1;struct Cyc_Absyn_Fndecl*
_tmp1B2;struct Cyc_Absyn_Typedefdecl*_tmp1B3;struct Cyc_Absyn_Aggrdecl*_tmp1B4;
struct Cyc_Absyn_Tuniondecl*_tmp1B5;struct Cyc_Absyn_Enumdecl*_tmp1B6;struct
_tagged_arr*_tmp1B7;struct Cyc_List_List*_tmp1B8;struct _tuple0*_tmp1B9;struct
_tuple0 _tmp1BA;void*_tmp1BB;struct _tagged_arr*_tmp1BC;struct Cyc_List_List*
_tmp1BD;struct Cyc_List_List*_tmp1BE;_LLFF: if(*((int*)_tmp1B0)!= 2)goto _LL101;
_LL100: goto _LL102;_LL101: if(*((int*)_tmp1B0)!= 3)goto _LL103;_LL102:({void*
_tmp1BF[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1C0="top level let-declarations are not implemented";
_tag_arr(_tmp1C0,sizeof(char),_get_zero_arr_size(_tmp1C0,47));}),_tag_arr(
_tmp1BF,sizeof(void*),0));});goto _LLFE;_LL103: if(*((int*)_tmp1B0)!= 0)goto _LL105;
_tmp1B1=((struct Cyc_Absyn_Var_d_struct*)_tmp1B0)->f1;_LL104: if(in_externC?Cyc_Tc_okay_externC(
d->loc,(void*)_tmp1B1->sc): 0)(void*)(_tmp1B1->sc=(void*)((void*)4));if(_tmp1B1->initializer
!= 0){void*_tmp1C1=(void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp1B1->initializer))->r;
void*_tmp1C2;_LL116: if(*((int*)_tmp1C1)!= 19)goto _LL118;_tmp1C2=(void*)((struct
Cyc_Absyn_Gentyp_e_struct*)_tmp1C1)->f2;_LL117: {struct Cyc_Dict_Dict*_tmp1C4;
struct Cyc_List_List*_tmp1C5;struct Cyc_Absyn_Exp*_tmp1C6;struct _tuple5 _tmp1C3=Cyc_Tcgenrep_tcGenrep(
te,ge,loc,_tmp1C2,dict);_tmp1C4=_tmp1C3.f1;_tmp1C5=_tmp1C3.f2;_tmp1C6=_tmp1C3.f3;
dict=_tmp1C4;Cyc_Tc_tc_decls(te,_tmp1C5,in_externC,check_var_init);_tmp1B1->initializer=(
struct Cyc_Absyn_Exp*)_tmp1C6;Cyc_Tc_tcVardecl(te,ge,loc,_tmp1B1,check_var_init);
if(_tmp1C5 != 0){if(last != 0){((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1C5,_tmp1AF);last->tl=_tmp1C5;}
else{struct Cyc_List_List tmp=({struct Cyc_List_List _tmp1C7;_tmp1C7.hd=(struct Cyc_Absyn_Decl*)
_tmp1AF->hd;_tmp1C7.tl=_tmp1AF->tl;_tmp1C7;});(struct Cyc_Absyn_Decl*)(_tmp1AF->hd=(
void*)((struct Cyc_Absyn_Decl*)_tmp1C5->hd));_tmp1AF->tl=_tmp1C5->tl;(struct Cyc_Absyn_Decl*)(
_tmp1C5->hd=(void*)((struct Cyc_Absyn_Decl*)tmp.hd));_tmp1C5->tl=tmp.tl;((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp1AF,_tmp1C5);}}continue;}_LL118:;_LL119: goto _LL115;_LL115:;}Cyc_Tc_tcVardecl(
te,ge,loc,_tmp1B1,check_var_init);goto _LLFE;_LL105: if(*((int*)_tmp1B0)!= 1)goto
_LL107;_tmp1B2=((struct Cyc_Absyn_Fn_d_struct*)_tmp1B0)->f1;_LL106: if(in_externC?
Cyc_Tc_okay_externC(d->loc,(void*)_tmp1B2->sc): 0)(void*)(_tmp1B2->sc=(void*)((
void*)4));Cyc_Tc_tcFndecl(te,ge,loc,_tmp1B2);goto _LLFE;_LL107: if(*((int*)_tmp1B0)
!= 7)goto _LL109;_tmp1B3=((struct Cyc_Absyn_Typedef_d_struct*)_tmp1B0)->f1;_LL108:
Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp1B3);goto _LLFE;_LL109: if(*((int*)_tmp1B0)!= 4)
goto _LL10B;_tmp1B4=((struct Cyc_Absyn_Aggr_d_struct*)_tmp1B0)->f1;_LL10A: if(
in_externC?Cyc_Tc_okay_externC(d->loc,(void*)_tmp1B4->sc): 0)(void*)(_tmp1B4->sc=(
void*)((void*)4));Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp1B4);goto _LLFE;_LL10B: if(*((
int*)_tmp1B0)!= 5)goto _LL10D;_tmp1B5=((struct Cyc_Absyn_Tunion_d_struct*)_tmp1B0)->f1;
_LL10C: if(in_externC?Cyc_Tc_okay_externC(d->loc,(void*)_tmp1B5->sc): 0)(void*)(
_tmp1B5->sc=(void*)((void*)4));Cyc_Tc_tcTuniondecl(te,ge,loc,_tmp1B5);goto _LLFE;
_LL10D: if(*((int*)_tmp1B0)!= 6)goto _LL10F;_tmp1B6=((struct Cyc_Absyn_Enum_d_struct*)
_tmp1B0)->f1;_LL10E: if(in_externC?Cyc_Tc_okay_externC(d->loc,(void*)_tmp1B6->sc):
0)(void*)(_tmp1B6->sc=(void*)((void*)4));Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp1B6);
goto _LLFE;_LL10F: if(*((int*)_tmp1B0)!= 8)goto _LL111;_tmp1B7=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp1B0)->f1;_tmp1B8=((struct Cyc_Absyn_Namespace_d_struct*)_tmp1B0)->f2;_LL110: {
struct Cyc_List_List*_tmp1C8=te->ns;struct Cyc_List_List*_tmp1C9=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C8,({struct
Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=_tmp1B7;_tmp1CA->tl=
0;_tmp1CA;}));if(!((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
ge->namespaces,_tmp1B7)){ge->namespaces=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_insert)(ge->namespaces,_tmp1B7);te->ae=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*
v))Cyc_Dict_insert)(te->ae,_tmp1C9,Cyc_Tcenv_empty_genv());}te->ns=_tmp1C9;Cyc_Tc_tc_decls(
te,_tmp1B8,in_externC,check_var_init);te->ns=_tmp1C8;goto _LLFE;}_LL111: if(*((int*)
_tmp1B0)!= 9)goto _LL113;_tmp1B9=((struct Cyc_Absyn_Using_d_struct*)_tmp1B0)->f1;
_tmp1BA=*_tmp1B9;_tmp1BB=_tmp1BA.f1;_tmp1BC=_tmp1BA.f2;_tmp1BD=((struct Cyc_Absyn_Using_d_struct*)
_tmp1B0)->f2;_LL112: {struct _tagged_arr*first;struct Cyc_List_List*rest;{void*
_tmp1CB=_tmp1BB;struct Cyc_List_List*_tmp1CC;struct Cyc_List_List*_tmp1CD;struct
Cyc_List_List*_tmp1CE;struct Cyc_List_List _tmp1CF;struct _tagged_arr*_tmp1D0;
struct Cyc_List_List*_tmp1D1;struct Cyc_List_List*_tmp1D2;struct Cyc_List_List
_tmp1D3;struct _tagged_arr*_tmp1D4;struct Cyc_List_List*_tmp1D5;_LL11B: if((int)
_tmp1CB != 0)goto _LL11D;_LL11C: goto _LL11E;_LL11D: if(_tmp1CB <= (void*)1?1:*((int*)
_tmp1CB)!= 0)goto _LL11F;_tmp1CC=((struct Cyc_Absyn_Rel_n_struct*)_tmp1CB)->f1;if(
_tmp1CC != 0)goto _LL11F;_LL11E: goto _LL120;_LL11F: if(_tmp1CB <= (void*)1?1:*((int*)
_tmp1CB)!= 1)goto _LL121;_tmp1CD=((struct Cyc_Absyn_Abs_n_struct*)_tmp1CB)->f1;if(
_tmp1CD != 0)goto _LL121;_LL120: first=_tmp1BC;rest=0;goto _LL11A;_LL121: if(_tmp1CB
<= (void*)1?1:*((int*)_tmp1CB)!= 0)goto _LL123;_tmp1CE=((struct Cyc_Absyn_Rel_n_struct*)
_tmp1CB)->f1;if(_tmp1CE == 0)goto _LL123;_tmp1CF=*_tmp1CE;_tmp1D0=(struct
_tagged_arr*)_tmp1CF.hd;_tmp1D1=_tmp1CF.tl;_LL122: _tmp1D4=_tmp1D0;_tmp1D5=
_tmp1D1;goto _LL124;_LL123: if(_tmp1CB <= (void*)1?1:*((int*)_tmp1CB)!= 1)goto
_LL11A;_tmp1D2=((struct Cyc_Absyn_Abs_n_struct*)_tmp1CB)->f1;if(_tmp1D2 == 0)goto
_LL11A;_tmp1D3=*_tmp1D2;_tmp1D4=(struct _tagged_arr*)_tmp1D3.hd;_tmp1D5=_tmp1D3.tl;
_LL124: first=_tmp1D4;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp1D5,({struct Cyc_List_List*_tmp1D6=
_cycalloc(sizeof(*_tmp1D6));_tmp1D6->hd=_tmp1BC;_tmp1D6->tl=0;_tmp1D6;}));goto
_LL11A;_LL11A:;}{struct Cyc_List_List*_tmp1D7=Cyc_Tcenv_resolve_namespace(te,loc,
first,rest);ge->availables=({struct Cyc_List_List*_tmp1D8=_cycalloc(sizeof(*
_tmp1D8));_tmp1D8->hd=_tmp1D7;_tmp1D8->tl=ge->availables;_tmp1D8;});Cyc_Tc_tc_decls(
te,_tmp1BD,in_externC,check_var_init);ge->availables=((struct Cyc_List_List*)
_check_null(ge->availables))->tl;goto _LLFE;}}_LL113: if(*((int*)_tmp1B0)!= 10)
goto _LLFE;_tmp1BE=((struct Cyc_Absyn_ExternC_d_struct*)_tmp1B0)->f1;_LL114: Cyc_Tc_tc_decls(
te,_tmp1BE,1,check_var_init);goto _LLFE;_LLFE:;}}void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*
te,int check_var_init,struct Cyc_List_List*ds){Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,check_var_init);}struct Cyc_Tc_TreeshakeEnv{struct
_RegionHandle*rgn;struct Cyc_Dict_Dict*nsdict;};static int Cyc_Tc_vardecl_needed(
struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);static struct Cyc_List_List*
Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){return((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),
struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,
env,ds);}struct _tuple11{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};static int
Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){void*
_tmp1D9=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp1DA;struct Cyc_List_List*_tmp1DB;
struct Cyc_List_List**_tmp1DC;struct Cyc_List_List*_tmp1DD;struct Cyc_List_List**
_tmp1DE;struct Cyc_List_List*_tmp1DF;struct Cyc_List_List**_tmp1E0;_LL126: if(*((
int*)_tmp1D9)!= 0)goto _LL128;_tmp1DA=((struct Cyc_Absyn_Var_d_struct*)_tmp1D9)->f1;
_LL127: if((void*)_tmp1DA->sc != (void*)3?(void*)_tmp1DA->sc != (void*)4: 0)return 1;{
struct _tuple0 _tmp1E2;void*_tmp1E3;struct _tagged_arr*_tmp1E4;struct _tuple0*
_tmp1E1=_tmp1DA->name;_tmp1E2=*_tmp1E1;_tmp1E3=_tmp1E2.f1;_tmp1E4=_tmp1E2.f2;{
struct Cyc_List_List*ns;{void*_tmp1E5=_tmp1E3;struct Cyc_List_List*_tmp1E6;struct
Cyc_List_List*_tmp1E7;_LL131: if((int)_tmp1E5 != 0)goto _LL133;_LL132: ns=0;goto
_LL130;_LL133: if(_tmp1E5 <= (void*)1?1:*((int*)_tmp1E5)!= 0)goto _LL135;_tmp1E6=((
struct Cyc_Absyn_Rel_n_struct*)_tmp1E5)->f1;_LL134: ns=_tmp1E6;goto _LL130;_LL135:
if(_tmp1E5 <= (void*)1?1:*((int*)_tmp1E5)!= 1)goto _LL130;_tmp1E7=((struct Cyc_Absyn_Abs_n_struct*)
_tmp1E5)->f1;_LL136: ns=_tmp1E7;goto _LL130;_LL130:;}{struct _tuple11*_tmp1E8=((
struct _tuple11*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
env->nsdict,ns);struct Cyc_Tcenv_Genv*_tmp1E9=(*_tmp1E8).f1;int _tmp1EA=(*((struct
_tuple6*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp1E9->ordinaries,
_tmp1E4)).f2;if(!_tmp1EA)(*_tmp1E8).f2=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_rinsert)(env->rgn,(*
_tmp1E8).f2,_tmp1E4);return _tmp1EA;}}}_LL128: if(*((int*)_tmp1D9)!= 10)goto _LL12A;
_tmp1DB=((struct Cyc_Absyn_ExternC_d_struct*)_tmp1D9)->f1;_tmp1DC=(struct Cyc_List_List**)&((
struct Cyc_Absyn_ExternC_d_struct*)_tmp1D9)->f1;_LL129: _tmp1DE=_tmp1DC;goto _LL12B;
_LL12A: if(*((int*)_tmp1D9)!= 9)goto _LL12C;_tmp1DD=((struct Cyc_Absyn_Using_d_struct*)
_tmp1D9)->f2;_tmp1DE=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_struct*)
_tmp1D9)->f2;_LL12B: _tmp1E0=_tmp1DE;goto _LL12D;_LL12C: if(*((int*)_tmp1D9)!= 8)
goto _LL12E;_tmp1DF=((struct Cyc_Absyn_Namespace_d_struct*)_tmp1D9)->f2;_tmp1E0=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_struct*)_tmp1D9)->f2;_LL12D:*
_tmp1E0=Cyc_Tc_treeshake_f(env,*_tmp1E0);return 1;_LL12E:;_LL12F: return 1;_LL125:;}
static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _tagged_arr*x,
void*y){return !((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
set,x);}static struct _tuple11*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,
struct Cyc_Tcenv_Genv*ge){return({struct _tuple11*_tmp1EB=_region_malloc(rgn,
sizeof(*_tmp1EB));_tmp1EB->f1=ge;_tmp1EB->f2=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_rempty)(
rgn,Cyc_strptrcmp);_tmp1EB;});}struct _tuple12{struct Cyc_List_List*f1;struct
_tuple11*f2;};struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*ds){struct _RegionHandle _tmp1EC=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp1EC;_push_region(rgn);{struct Cyc_Tc_TreeshakeEnv _tmp1ED=({
struct Cyc_Tc_TreeshakeEnv _tmp1F8;_tmp1F8.rgn=rgn;_tmp1F8.nsdict=((struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*,struct _tuple11*(*f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),
struct _RegionHandle*env,struct Cyc_Dict_Dict*d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,
rgn,te->ae);_tmp1F8;});struct Cyc_List_List*_tmp1EE=Cyc_Tc_treeshake_f(& _tmp1ED,
ds);if(((int(*)(struct Cyc_Dict_Dict*d))Cyc_Dict_is_empty)(_tmp1ED.nsdict)){
struct Cyc_List_List*_tmp1EF=_tmp1EE;_npop_handler(0);return _tmp1EF;}{struct Cyc_Iter_Iter
_tmp1F0=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict*d))
Cyc_Dict_make_iter)(Cyc_Core_heap_region,_tmp1ED.nsdict);struct _tuple12 _tmp1F1=*((
struct _tuple12*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict*d))Cyc_Dict_rchoose)(
rgn,_tmp1ED.nsdict);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple12*))Cyc_Iter_next)(
_tmp1F0,& _tmp1F1)){struct _tuple11*_tmp1F3;struct _tuple11 _tmp1F4;struct Cyc_Tcenv_Genv*
_tmp1F5;struct Cyc_Set_Set*_tmp1F6;struct _tuple12 _tmp1F2=_tmp1F1;_tmp1F3=_tmp1F2.f2;
_tmp1F4=*_tmp1F3;_tmp1F5=_tmp1F4.f1;_tmp1F6=_tmp1F4.f2;_tmp1F5->ordinaries=((
struct Cyc_Dict_Dict*(*)(int(*f)(struct Cyc_Set_Set*,struct _tagged_arr*,struct
_tuple6*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict*d))Cyc_Dict_filter_c)((int(*)(
struct Cyc_Set_Set*set,struct _tagged_arr*x,struct _tuple6*y))Cyc_Tc_treeshake_remove_f,
_tmp1F6,_tmp1F5->ordinaries);}{struct Cyc_List_List*_tmp1F7=_tmp1EE;_npop_handler(
0);return _tmp1F7;}}};_pop_region(rgn);}
