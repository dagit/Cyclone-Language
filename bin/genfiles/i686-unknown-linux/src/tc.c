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
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_List_List*Cyc_List_filter_c(
int(*f)(void*,void*),void*env,struct Cyc_List_List*x);int Cyc_strcmp(struct
_tagged_arr s1,struct _tagged_arr s2);int Cyc_strptrcmp(struct _tagged_arr*s1,struct
_tagged_arr*s2);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;struct
Cyc_Position_Segment*loc;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(struct
_tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();void*Cyc_Absyn_compress_kb(void*);void*
Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_tagged_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct _tagged_arr Cyc_Absyn_attribute2string(
void*);int Cyc_Absyn_fntype_att(void*a);typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{int __pos;
Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned int*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,
int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,
void*elt);struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*
s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[
11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[
11];int Cyc_Dict_is_empty(struct Cyc_Dict_Dict*d);int Cyc_Dict_member(struct Cyc_Dict_Dict*
d,void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*
v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict*d,void*k);struct Cyc_Dict_Dict*Cyc_Dict_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict*d);struct
_tuple3{void*f1;void*f2;};struct _tuple3*Cyc_Dict_rchoose(struct _RegionHandle*r,
struct Cyc_Dict_Dict*d);struct _tuple3*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_filter_c(int(*f)(void*,void*,
void*),void*env,struct Cyc_Dict_Dict*d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct
Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*
po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_supports_default(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);void*Cyc_Tcexp_tcExpInitializer(
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
3  || *((int*)_tmp1)!= 8)goto _LL3;_tmp2=((struct Cyc_Absyn_FnType_struct*)_tmp1)->f1;
_tmp3=_tmp2.attributes;_tmp4=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp1)->f1).attributes;_LL2: {struct Cyc_List_List*_tmp5=0;for(0;atts != 0;atts=
atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))*_tmp4=({struct Cyc_List_List*
_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->hd=(void*)((void*)atts->hd);_tmp6->tl=*
_tmp4;_tmp6;});else{_tmp5=({struct Cyc_List_List*_tmp7=_cycalloc(sizeof(*_tmp7));
_tmp7->hd=(void*)((void*)atts->hd);_tmp7->tl=_tmp5;_tmp7;});}}return _tmp5;}_LL3:;
_LL4:({void*_tmp8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp9="transfer_fn_type_atts";_tag_arr(_tmp9,sizeof(char),
_get_zero_arr_size(_tmp9,22));}),_tag_arr(_tmp8,sizeof(void*),0));});_LL0:;}
struct _tuple6{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};static int
Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){for(0;exports
!= 0;exports=exports->tl){struct _tuple6*_tmpA=(struct _tuple6*)exports->hd;if(Cyc_Absyn_qvar_cmp(
x,(*_tmpA).f2)== 0){(*_tmpA).f3=1;return 1;}}return 0;}struct _tuple7{void*f1;int f2;
};static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int
ignore_init,struct Cyc_List_List**exports){struct Cyc_Absyn_Vardecl _tmpC;void*
_tmpD;struct _tuple0*_tmpE;struct _tuple0 _tmpF;void*_tmp10;struct _tagged_arr*
_tmp11;void*_tmp12;struct Cyc_Absyn_Exp*_tmp13;struct Cyc_List_List*_tmp14;struct
Cyc_Absyn_Vardecl*_tmpB=vd;_tmpC=*_tmpB;_tmpD=(void*)_tmpC.sc;_tmpE=_tmpC.name;
_tmpF=*_tmpE;_tmp10=_tmpF.f1;_tmp11=_tmpF.f2;_tmp12=(void*)_tmpC.type;_tmp13=
_tmpC.initializer;_tmp14=_tmpC.attributes;{void*_tmp15=_tmp10;struct Cyc_List_List*
_tmp16;struct Cyc_List_List*_tmp17;_LL6: if(_tmp15 <= (void*)1  || *((int*)_tmp15)!= 
0)goto _LL8;_tmp16=((struct Cyc_Absyn_Rel_n_struct*)_tmp15)->f1;if(_tmp16 != 0)goto
_LL8;_LL7: goto _LL9;_LL8: if(_tmp15 <= (void*)1  || *((int*)_tmp15)!= 1)goto _LLA;
_tmp17=((struct Cyc_Absyn_Abs_n_struct*)_tmp15)->f1;if(_tmp17 != 0)goto _LLA;_LL9:
goto _LL5;_LLA:;_LLB:({struct Cyc_String_pa_struct _tmp1A;_tmp1A.tag=0;_tmp1A.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name));{void*
_tmp18[1]={& _tmp1A};Cyc_Tcutil_terr(loc,({const char*_tmp19="qualified variable declarations are not implemented (%s)";
_tag_arr(_tmp19,sizeof(char),_get_zero_arr_size(_tmp19,57));}),_tag_arr(_tmp18,
sizeof(void*),1));}});return;_LL5:;}(*vd->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=({struct Cyc_Absyn_Abs_n_struct _tmp1C;
_tmp1C.tag=1;_tmp1C.f1=te->ns;_tmp1C;});_tmp1B;});{void*_tmp1D=Cyc_Tcutil_compress(
_tmp12);struct Cyc_Absyn_ArrayInfo _tmp1E;void*_tmp1F;struct Cyc_Absyn_Tqual _tmp20;
struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Conref*_tmp22;_LLD: if(_tmp1D <= (void*)
3  || *((int*)_tmp1D)!= 7)goto _LLF;_tmp1E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1D)->f1;_tmp1F=(void*)_tmp1E.elt_type;_tmp20=_tmp1E.tq;_tmp21=_tmp1E.num_elts;
if(_tmp21 != 0)goto _LLF;_tmp22=_tmp1E.zero_term;if(!(_tmp13 != 0))goto _LLF;_LLE:{
void*_tmp23=(void*)_tmp13->r;void*_tmp24;struct _tagged_arr _tmp25;struct Cyc_Absyn_Exp*
_tmp26;struct Cyc_List_List*_tmp27;struct Cyc_List_List*_tmp28;_LL12: if(*((int*)
_tmp23)!= 0)goto _LL14;_tmp24=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp23)->f1;
if(_tmp24 <= (void*)1  || *((int*)_tmp24)!= 5)goto _LL14;_tmp25=((struct Cyc_Absyn_String_c_struct*)
_tmp24)->f1;_LL13: _tmp12=(void*)(vd->type=(void*)Cyc_Absyn_array_typ(_tmp1F,
_tmp20,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_arr_size(_tmp25,sizeof(char)),
0),_tmp22));goto _LL11;_LL14: if(*((int*)_tmp23)!= 29)goto _LL16;_tmp26=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp23)->f2;_LL15: _tmp12=(void*)(vd->type=(void*)Cyc_Absyn_array_typ(_tmp1F,
_tmp20,(struct Cyc_Absyn_Exp*)_tmp26,_tmp22));goto _LL11;_LL16: if(*((int*)_tmp23)
!= 36)goto _LL18;_tmp27=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp23)->f2;
_LL17: _tmp28=_tmp27;goto _LL19;_LL18: if(*((int*)_tmp23)!= 28)goto _LL1A;_tmp28=((
struct Cyc_Absyn_Array_e_struct*)_tmp23)->f1;_LL19: _tmp12=(void*)(vd->type=(void*)
Cyc_Absyn_array_typ(_tmp1F,_tmp20,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp28),0),_tmp22));
goto _LL11;_LL1A:;_LL1B: goto _LL11;_LL11:;}goto _LLC;_LLF:;_LL10: goto _LLC;_LLC:;}
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp12);(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(vd->tq).print_const,_tmp12);{void*_tmp29=Cyc_Tcutil_compress(_tmp12);_LL1D:
if(_tmp29 <= (void*)3  || *((int*)_tmp29)!= 7)goto _LL1F;_LL1E: vd->escapes=0;goto
_LL1C;_LL1F:;_LL20: vd->escapes=1;goto _LL1C;_LL1C:;}if(Cyc_Tcutil_is_function_type(
_tmp12))_tmp14=Cyc_Tc_transfer_fn_type_atts(_tmp12,_tmp14);if(_tmpD == (void*)3
 || _tmpD == (void*)4){if(_tmp13 != 0)({void*_tmp2A[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp2B="extern declaration should not have initializer";_tag_arr(
_tmp2B,sizeof(char),_get_zero_arr_size(_tmp2B,47));}),_tag_arr(_tmp2A,sizeof(
void*),0));});}else{if(!Cyc_Tcutil_is_function_type(_tmp12)){for(0;_tmp14 != 0;
_tmp14=_tmp14->tl){void*_tmp2C=(void*)_tmp14->hd;_LL22: if(_tmp2C <= (void*)17  || *((
int*)_tmp2C)!= 1)goto _LL24;_LL23: goto _LL25;_LL24: if(_tmp2C <= (void*)17  || *((int*)
_tmp2C)!= 2)goto _LL26;_LL25: goto _LL27;_LL26: if((int)_tmp2C != 6)goto _LL28;_LL27:
goto _LL29;_LL28: if((int)_tmp2C != 7)goto _LL2A;_LL29: goto _LL2B;_LL2A: if((int)
_tmp2C != 8)goto _LL2C;_LL2B: goto _LL2D;_LL2C: if((int)_tmp2C != 9)goto _LL2E;_LL2D:
goto _LL2F;_LL2E: if((int)_tmp2C != 10)goto _LL30;_LL2F: goto _LL31;_LL30: if((int)
_tmp2C != 11)goto _LL32;_LL31: continue;_LL32:;_LL33:({struct Cyc_String_pa_struct
_tmp30;_tmp30.tag=0;_tmp30.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
vd->name));{struct Cyc_String_pa_struct _tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absyn_attribute2string((void*)_tmp14->hd));{
void*_tmp2D[2]={& _tmp2F,& _tmp30};Cyc_Tcutil_terr(loc,({const char*_tmp2E="bad attribute %s for variable %s";
_tag_arr(_tmp2E,sizeof(char),_get_zero_arr_size(_tmp2E,33));}),_tag_arr(_tmp2D,
sizeof(void*),2));}}});goto _LL21;_LL21:;}if(_tmp13 == 0  || ignore_init){if(
check_var_init  && !Cyc_Tcutil_supports_default(_tmp12))({struct Cyc_String_pa_struct
_tmp34;_tmp34.tag=0;_tmp34.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp12));{struct Cyc_String_pa_struct _tmp33;_tmp33.tag=0;_tmp33.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name));{void*_tmp31[
2]={& _tmp33,& _tmp34};Cyc_Tcutil_terr(loc,({const char*_tmp32="initializer required for variable %s of type %s";
_tag_arr(_tmp32,sizeof(char),_get_zero_arr_size(_tmp32,48));}),_tag_arr(_tmp31,
sizeof(void*),2));}}});}else{struct Cyc_Absyn_Exp*_tmp35=(struct Cyc_Absyn_Exp*)
_tmp13;void*_tmp36=Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp12,_tmp35);if(!Cyc_Tcutil_coerce_assign(
te,_tmp35,_tmp12)){({struct Cyc_String_pa_struct _tmp3B;_tmp3B.tag=0;_tmp3B.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp36));{struct Cyc_String_pa_struct
_tmp3A;_tmp3A.tag=0;_tmp3A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp12));{struct Cyc_String_pa_struct _tmp39;_tmp39.tag=0;_tmp39.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name));{void*_tmp37[
3]={& _tmp39,& _tmp3A,& _tmp3B};Cyc_Tcutil_terr(loc,({const char*_tmp38="%s declared with type \n%s\n but initialized with type \n%s";
_tag_arr(_tmp38,sizeof(char),_get_zero_arr_size(_tmp38,57));}),_tag_arr(_tmp37,
sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}if(!Cyc_Tcutil_is_const_exp(
te,_tmp35))({void*_tmp3C[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3D="initializer is not a constant expression";
_tag_arr(_tmp3D,sizeof(char),_get_zero_arr_size(_tmp3D,41));}),_tag_arr(_tmp3C,
sizeof(void*),0));});}}else{for(0;_tmp14 != 0;_tmp14=_tmp14->tl){void*_tmp3E=(
void*)_tmp14->hd;_LL35: if(_tmp3E <= (void*)17  || *((int*)_tmp3E)!= 0)goto _LL37;
_LL36: goto _LL38;_LL37: if((int)_tmp3E != 0)goto _LL39;_LL38: goto _LL3A;_LL39: if((int)
_tmp3E != 1)goto _LL3B;_LL3A: goto _LL3C;_LL3B: if((int)_tmp3E != 2)goto _LL3D;_LL3C:
goto _LL3E;_LL3D: if((int)_tmp3E != 3)goto _LL3F;_LL3E: goto _LL40;_LL3F: if(_tmp3E <= (
void*)17  || *((int*)_tmp3E)!= 3)goto _LL41;_LL40: goto _LL42;_LL41: if(_tmp3E <= (
void*)17  || *((int*)_tmp3E)!= 4)goto _LL43;_LL42: goto _LL44;_LL43: if((int)_tmp3E != 
16)goto _LL45;_LL44: goto _LL46;_LL45: if((int)_tmp3E != 4)goto _LL47;_LL46:({void*
_tmp3F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp40="tcVardecl: fn type atts in function var decl";_tag_arr(_tmp40,
sizeof(char),_get_zero_arr_size(_tmp40,45));}),_tag_arr(_tmp3F,sizeof(void*),0));});
_LL47: if(_tmp3E <= (void*)17  || *((int*)_tmp3E)!= 1)goto _LL49;_LL48: goto _LL4A;
_LL49: if((int)_tmp3E != 5)goto _LL4B;_LL4A:({struct Cyc_String_pa_struct _tmp43;
_tmp43.tag=0;_tmp43.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp14->hd));{void*_tmp41[1]={& _tmp43};Cyc_Tcutil_terr(loc,({const char*
_tmp42="bad attribute %s in function declaration";_tag_arr(_tmp42,sizeof(char),
_get_zero_arr_size(_tmp42,41));}),_tag_arr(_tmp41,sizeof(void*),1));}});goto
_LL34;_LL4B:;_LL4C: continue;_LL34:;}}}{struct _handler_cons _tmp44;_push_handler(&
_tmp44);{int _tmp46=0;if(setjmp(_tmp44.handler))_tmp46=1;if(!_tmp46){{struct
_tuple7*_tmp47=((struct _tuple7*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))
Cyc_Dict_lookup)(ge->ordinaries,_tmp11);void*_tmp48=(*_tmp47).f1;void*_tmp49;
_LL4E: if(*((int*)_tmp48)!= 0)goto _LL50;_tmp49=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp48)->f1;_LL4F: {struct Cyc_Absyn_Global_b_struct*_tmp4A=({struct Cyc_Absyn_Global_b_struct*
_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_Global_b_struct
_tmp50;_tmp50.tag=0;_tmp50.f1=vd;_tmp50;});_tmp4F;});void*_tmp4B=Cyc_Tcdecl_merge_binding(
_tmp49,(void*)_tmp4A,loc,Cyc_Tc_tc_msg);if(_tmp4B == (void*)0){_npop_handler(0);
return;}if(_tmp4B == _tmp49  && (*_tmp47).f2){_npop_handler(0);return;}if(exports
== 0  || Cyc_Tc_export_member(vd->name,*exports))ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple7*v))Cyc_Dict_insert)(ge->ordinaries,
_tmp11,({struct _tuple7*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->f1=(void*)({
struct Cyc_Tcenv_VarRes_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D[0]=({
struct Cyc_Tcenv_VarRes_struct _tmp4E;_tmp4E.tag=0;_tmp4E.f1=(void*)_tmp4B;_tmp4E;});
_tmp4D;});_tmp4C->f2=1;_tmp4C;}));_npop_handler(0);return;}_LL50: if(*((int*)
_tmp48)!= 1)goto _LL52;_LL51:({void*_tmp51[0]={};Cyc_Tcutil_warn(loc,({const char*
_tmp52="variable declaration shadows previous struct declaration";_tag_arr(
_tmp52,sizeof(char),_get_zero_arr_size(_tmp52,57));}),_tag_arr(_tmp51,sizeof(
void*),0));});goto _LL4D;_LL52: if(*((int*)_tmp48)!= 2)goto _LL54;_LL53:({void*
_tmp53[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp54="variable declaration shadows previous [x]tunion constructor";
_tag_arr(_tmp54,sizeof(char),_get_zero_arr_size(_tmp54,60));}),_tag_arr(_tmp53,
sizeof(void*),0));});goto _LL4D;_LL54: if(*((int*)_tmp48)!= 4)goto _LL56;_LL55: goto
_LL57;_LL56: if(*((int*)_tmp48)!= 3)goto _LL4D;_LL57:({void*_tmp55[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp56="variable declaration shadows previous enum tag";_tag_arr(
_tmp56,sizeof(char),_get_zero_arr_size(_tmp56,47));}),_tag_arr(_tmp55,sizeof(
void*),0));});goto _LL4D;_LL4D:;};_pop_handler();}else{void*_tmp45=(void*)
_exn_thrown;void*_tmp58=_tmp45;_LL59: if(_tmp58 != Cyc_Dict_Absent)goto _LL5B;_LL5A:
goto _LL58;_LL5B:;_LL5C:(void)_throw(_tmp58);_LL58:;}}}if(exports == 0  || Cyc_Tc_export_member(
vd->name,*exports))ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct _tuple7*v))Cyc_Dict_insert)(ge->ordinaries,_tmp11,({
struct _tuple7*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->f1=(void*)({struct Cyc_Tcenv_VarRes_struct*
_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A[0]=({struct Cyc_Tcenv_VarRes_struct
_tmp5B;_tmp5B.tag=0;_tmp5B.f1=(void*)((void*)({struct Cyc_Absyn_Global_b_struct*
_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C[0]=({struct Cyc_Absyn_Global_b_struct
_tmp5D;_tmp5D.tag=0;_tmp5D.f1=vd;_tmp5D;});_tmp5C;}));_tmp5B;});_tmp5A;});_tmp59->f2=
0;_tmp59;}));}static int Cyc_Tc_is_main(struct _tuple0*n){struct _tuple0 _tmp5F;void*
_tmp60;struct _tagged_arr*_tmp61;struct _tuple0*_tmp5E=n;_tmp5F=*_tmp5E;_tmp60=
_tmp5F.f1;_tmp61=_tmp5F.f2;{void*_tmp62=_tmp60;struct Cyc_List_List*_tmp63;_LL5E:
if(_tmp62 <= (void*)1  || *((int*)_tmp62)!= 1)goto _LL60;_tmp63=((struct Cyc_Absyn_Abs_n_struct*)
_tmp62)->f1;if(_tmp63 != 0)goto _LL60;_LL5F: return Cyc_strcmp((struct _tagged_arr)*
_tmp61,({const char*_tmp64="main";_tag_arr(_tmp64,sizeof(char),_get_zero_arr_size(
_tmp64,5));}))== 0;_LL60:;_LL61: return 0;_LL5D:;}}struct _tuple8{struct _tagged_arr*
f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*
fd,int ignore_body,struct Cyc_List_List**exports){struct _tagged_arr*v=(*fd->name).f2;
if((void*)fd->sc == (void*)4  && !ignore_body)({void*_tmp65[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp66="extern \"C\" functions cannot be implemented in Cyclone";
_tag_arr(_tmp66,sizeof(char),_get_zero_arr_size(_tmp66,54));}),_tag_arr(_tmp65,
sizeof(void*),0));});{void*_tmp67=(*fd->name).f1;struct Cyc_List_List*_tmp68;
struct Cyc_List_List*_tmp69;_LL63: if(_tmp67 <= (void*)1  || *((int*)_tmp67)!= 0)
goto _LL65;_tmp68=((struct Cyc_Absyn_Rel_n_struct*)_tmp67)->f1;if(_tmp68 != 0)goto
_LL65;_LL64: goto _LL62;_LL65: if(_tmp67 <= (void*)1  || *((int*)_tmp67)!= 1)goto
_LL67;_tmp69=((struct Cyc_Absyn_Abs_n_struct*)_tmp67)->f1;_LL66:({void*_tmp6A[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp6B="tc: Abs_n in tcFndecl";_tag_arr(_tmp6B,sizeof(char),_get_zero_arr_size(
_tmp6B,22));}),_tag_arr(_tmp6A,sizeof(void*),0));});_LL67:;_LL68:({struct Cyc_String_pa_struct
_tmp6E;_tmp6E.tag=0;_tmp6E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
fd->name));{void*_tmp6C[1]={& _tmp6E};Cyc_Tcutil_terr(loc,({const char*_tmp6D="qualified function declarations are not implemented (%s)";
_tag_arr(_tmp6D,sizeof(char),_get_zero_arr_size(_tmp6D,57));}),_tag_arr(_tmp6C,
sizeof(void*),1));}});return;_LL62:;}(*fd->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F[0]=({struct Cyc_Absyn_Abs_n_struct _tmp70;
_tmp70.tag=1;_tmp70.f1=te->ns;_tmp70;});_tmp6F;});Cyc_Tcutil_check_fndecl_valid_type(
loc,te,fd);{void*t=Cyc_Tcutil_fndecl2typ(fd);fd->attributes=Cyc_Tc_transfer_fn_type_atts(
t,fd->attributes);{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=
atts->tl){void*_tmp71=(void*)atts->hd;_LL6A: if((int)_tmp71 != 5)goto _LL6C;_LL6B:
goto _LL6D;_LL6C: if(_tmp71 <= (void*)17  || *((int*)_tmp71)!= 1)goto _LL6E;_LL6D:({
struct Cyc_String_pa_struct _tmp74;_tmp74.tag=0;_tmp74.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp72[1]={&
_tmp74};Cyc_Tcutil_terr(loc,({const char*_tmp73="bad attribute %s for function";
_tag_arr(_tmp73,sizeof(char),_get_zero_arr_size(_tmp73,30));}),_tag_arr(_tmp72,
sizeof(void*),1));}});goto _LL69;_LL6E:;_LL6F: goto _LL69;_LL69:;}}{struct
_handler_cons _tmp75;_push_handler(& _tmp75);{int _tmp77=0;if(setjmp(_tmp75.handler))
_tmp77=1;if(!_tmp77){{struct _tuple7*_tmp78=((struct _tuple7*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->ordinaries,v);void*_tmp79=(*_tmp78).f1;
void*_tmp7A;_LL71: if(*((int*)_tmp79)!= 0)goto _LL73;_tmp7A=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp79)->f1;_LL72: {struct Cyc_Absyn_Funname_b_struct*_tmp7B=({struct Cyc_Absyn_Funname_b_struct*
_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[0]=({struct Cyc_Absyn_Funname_b_struct
_tmp81;_tmp81.tag=1;_tmp81.f1=fd;_tmp81;});_tmp80;});void*_tmp7C=Cyc_Tcdecl_merge_binding(
_tmp7A,(void*)_tmp7B,loc,Cyc_Tc_tc_msg);if(_tmp7C == (void*)0)goto _LL70;if(_tmp7C
== _tmp7A  && (*_tmp78).f2)goto _LL70;if(exports == 0  || Cyc_Tc_export_member(fd->name,*
exports))ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple7*v))Cyc_Dict_insert)(ge->ordinaries,v,({struct _tuple7*
_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->f1=(void*)({struct Cyc_Tcenv_VarRes_struct*
_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Tcenv_VarRes_struct
_tmp7F;_tmp7F.tag=0;_tmp7F.f1=(void*)_tmp7C;_tmp7F;});_tmp7E;});_tmp7D->f2=1;
_tmp7D;}));goto _LL70;}_LL73: if(*((int*)_tmp79)!= 1)goto _LL75;_LL74:({void*_tmp82[
0]={};Cyc_Tcutil_warn(loc,({const char*_tmp83="function declaration shadows previous type declaration";
_tag_arr(_tmp83,sizeof(char),_get_zero_arr_size(_tmp83,55));}),_tag_arr(_tmp82,
sizeof(void*),0));});goto _LL70;_LL75: if(*((int*)_tmp79)!= 2)goto _LL77;_LL76:({
void*_tmp84[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp85="function declaration shadows previous [x]tunion constructor";
_tag_arr(_tmp85,sizeof(char),_get_zero_arr_size(_tmp85,60));}),_tag_arr(_tmp84,
sizeof(void*),0));});goto _LL70;_LL77: if(*((int*)_tmp79)!= 4)goto _LL79;_LL78: goto
_LL7A;_LL79: if(*((int*)_tmp79)!= 3)goto _LL70;_LL7A:({void*_tmp86[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp87="function declaration shadows previous enum tag";_tag_arr(
_tmp87,sizeof(char),_get_zero_arr_size(_tmp87,47));}),_tag_arr(_tmp86,sizeof(
void*),0));});goto _LL70;_LL70:;};_pop_handler();}else{void*_tmp76=(void*)
_exn_thrown;void*_tmp89=_tmp76;_LL7C: if(_tmp89 != Cyc_Dict_Absent)goto _LL7E;_LL7D:
if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports))ge->ordinaries=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple7*
v))Cyc_Dict_insert)(ge->ordinaries,v,({struct _tuple7*_tmp8A=_cycalloc(sizeof(*
_tmp8A));_tmp8A->f1=(void*)({struct Cyc_Tcenv_VarRes_struct*_tmp8B=_cycalloc(
sizeof(*_tmp8B));_tmp8B[0]=({struct Cyc_Tcenv_VarRes_struct _tmp8C;_tmp8C.tag=0;
_tmp8C.f1=(void*)((void*)({struct Cyc_Absyn_Funname_b_struct*_tmp8D=_cycalloc(
sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Absyn_Funname_b_struct _tmp8E;_tmp8E.tag=1;
_tmp8E.f1=fd;_tmp8E;});_tmp8D;}));_tmp8C;});_tmp8B;});_tmp8A->f2=0;_tmp8A;}));
goto _LL7B;_LL7E:;_LL7F:(void)_throw(_tmp89);_LL7B:;}}}if(ignore_body)return;te->le=({
struct Cyc_Core_Opt*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->v=(void*)((void*)({
struct Cyc_Tcenv_Outermost_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=({
struct Cyc_Tcenv_Outermost_struct _tmp91;_tmp91.tag=0;_tmp91.f1=(void*)Cyc_Tcenv_new_fenv(
loc,fd);_tmp91;});_tmp90;}));_tmp8F;});Cyc_Tcstmt_tcStmt(te,fd->body,0);Cyc_Tcenv_check_delayed_effects(
te);Cyc_Tcenv_check_delayed_constraints(te);if(!Cyc_Tcenv_all_labels_resolved(te))({
void*_tmp92[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp93="function has goto statements to undefined labels";
_tag_arr(_tmp93,sizeof(char),_get_zero_arr_size(_tmp93,49));}),_tag_arr(_tmp92,
sizeof(void*),0));});if(Cyc_Tc_is_main(fd->name)){{void*_tmp94=Cyc_Tcutil_compress((
void*)fd->ret_type);void*_tmp95;_LL81: if((int)_tmp94 != 0)goto _LL83;_LL82:({void*
_tmp96[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp97="main declared with return type void";
_tag_arr(_tmp97,sizeof(char),_get_zero_arr_size(_tmp97,36));}),_tag_arr(_tmp96,
sizeof(void*),0));});goto _LL80;_LL83: if(_tmp94 <= (void*)3  || *((int*)_tmp94)!= 5)
goto _LL85;_tmp95=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp94)->f2;if((int)
_tmp95 != 2)goto _LL85;_LL84: goto _LL80;_LL85:;_LL86:({struct Cyc_String_pa_struct
_tmp9A;_tmp9A.tag=0;_tmp9A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)fd->ret_type));{void*_tmp98[1]={& _tmp9A};Cyc_Tcutil_terr(loc,({const char*
_tmp99="main declared with return type %s instead of int or void";_tag_arr(
_tmp99,sizeof(char),_get_zero_arr_size(_tmp99,57));}),_tag_arr(_tmp98,sizeof(
void*),1));}});goto _LL80;_LL80:;}if(fd->c_varargs  || fd->cyc_varargs != 0)({void*
_tmp9B[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp9C="main declared with varargs";
_tag_arr(_tmp9C,sizeof(char),_get_zero_arr_size(_tmp9C,27));}),_tag_arr(_tmp9B,
sizeof(void*),0));});{struct Cyc_List_List*_tmp9D=fd->args;if(_tmp9D != 0){struct
_tuple8 _tmp9F;void*_tmpA0;struct _tuple8*_tmp9E=(struct _tuple8*)_tmp9D->hd;_tmp9F=*
_tmp9E;_tmpA0=_tmp9F.f3;{void*_tmpA1=Cyc_Tcutil_compress(_tmpA0);void*_tmpA2;
_LL88: if(_tmpA1 <= (void*)3  || *((int*)_tmpA1)!= 5)goto _LL8A;_tmpA2=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpA1)->f2;if((int)_tmpA2 != 2)goto _LL8A;_LL89:
goto _LL87;_LL8A:;_LL8B:({struct Cyc_String_pa_struct _tmpA5;_tmpA5.tag=0;_tmpA5.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(_tmpA0));{void*
_tmpA3[1]={& _tmpA5};Cyc_Tcutil_terr(loc,({const char*_tmpA4="main declared with first argument of type %s instead of int";
_tag_arr(_tmpA4,sizeof(char),_get_zero_arr_size(_tmpA4,60));}),_tag_arr(_tmpA3,
sizeof(void*),1));}});goto _LL87;_LL87:;}_tmp9D=_tmp9D->tl;if(_tmp9D != 0){struct
_tuple8 _tmpA7;void*_tmpA8;struct _tuple8*_tmpA6=(struct _tuple8*)_tmp9D->hd;_tmpA7=*
_tmpA6;_tmpA8=_tmpA7.f3;_tmp9D=_tmp9D->tl;if(_tmp9D != 0)({void*_tmpA9[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmpAA="main declared with too many arguments";_tag_arr(_tmpAA,
sizeof(char),_get_zero_arr_size(_tmpAA,38));}),_tag_arr(_tmpA9,sizeof(void*),0));});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmpB6=_cycalloc(sizeof(*_tmpB6));
_tmpB6->v=fd->tvs;_tmpB6;});if(((!Cyc_Tcutil_unify(_tmpA8,Cyc_Absyn_tagged_typ(
Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpAB=_cycalloc(
sizeof(*_tmpAB));_tmpAB->v=(void*)((void*)3);_tmpAB;}),tvs)),Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->v=(void*)((void*)3);
_tmpAC;}),tvs),Cyc_Absyn_empty_tqual(0),((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)()))
 && !Cyc_Tcutil_unify(_tmpA8,Cyc_Absyn_tagged_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->v=(void*)((void*)3);
_tmpAD;}),tvs)),Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpAE=_cycalloc(sizeof(*
_tmpAE));_tmpAE->v=(void*)((void*)3);_tmpAE;}),tvs),Cyc_Absyn_empty_tqual(0),((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)()))) && !Cyc_Tcutil_unify(
_tmpA8,Cyc_Absyn_tagged_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->v=(void*)((void*)3);_tmpAF;}),tvs)),
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->v=(
void*)((void*)3);_tmpB0;}),tvs),Cyc_Absyn_const_tqual(0),((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)()))) && !Cyc_Tcutil_unify(_tmpA8,Cyc_Absyn_tagged_typ(
Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpB1=
_cycalloc(sizeof(*_tmpB1));_tmpB1->v=(void*)((void*)3);_tmpB1;}),tvs)),Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->v=(void*)((void*)3);
_tmpB2;}),tvs),Cyc_Absyn_const_tqual(0),((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)())))({
struct Cyc_String_pa_struct _tmpB5;_tmpB5.tag=0;_tmpB5.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmpA8));{void*_tmpB3[1]={& _tmpB5};Cyc_Tcutil_terr(
loc,({const char*_tmpB4="second argument of main has type %s instead of char??";
_tag_arr(_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,54));}),_tag_arr(_tmpB3,
sizeof(void*),1));}});}}}}}te->le=0;}}static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*
td){struct _tagged_arr*v=(*td->name).f2;{void*_tmpB7=(*td->name).f1;struct Cyc_List_List*
_tmpB8;struct Cyc_List_List*_tmpB9;_LL8D: if(_tmpB7 <= (void*)1  || *((int*)_tmpB7)
!= 0)goto _LL8F;_tmpB8=((struct Cyc_Absyn_Rel_n_struct*)_tmpB7)->f1;if(_tmpB8 != 0)
goto _LL8F;_LL8E: goto _LL90;_LL8F: if(_tmpB7 <= (void*)1  || *((int*)_tmpB7)!= 1)goto
_LL91;_tmpB9=((struct Cyc_Absyn_Abs_n_struct*)_tmpB7)->f1;if(_tmpB9 != 0)goto _LL91;
_LL90: goto _LL8C;_LL91:;_LL92:({struct Cyc_String_pa_struct _tmpBC;_tmpBC.tag=0;
_tmpBC.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(td->name));{
void*_tmpBA[1]={& _tmpBC};Cyc_Tcutil_terr(loc,({const char*_tmpBB="qualified typedef declarations are not implemented (%s)";
_tag_arr(_tmpBB,sizeof(char),_get_zero_arr_size(_tmpBB,56));}),_tag_arr(_tmpBA,
sizeof(void*),1));}});return;_LL8C:;}if(((int(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_member)(ge->typedefs,v)){({struct Cyc_String_pa_struct
_tmpBF;_tmpBF.tag=0;_tmpBF.f1=(struct _tagged_arr)((struct _tagged_arr)*v);{void*
_tmpBD[1]={& _tmpBF};Cyc_Tcutil_terr(loc,({const char*_tmpBE="redeclaration of typedef %s";
_tag_arr(_tmpBE,sizeof(char),_get_zero_arr_size(_tmpBE,28));}),_tag_arr(_tmpBD,
sizeof(void*),1));}});return;}(*td->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({struct Cyc_Absyn_Abs_n_struct _tmpC1;
_tmpC1.tag=1;_tmpC1.f1=te->ns;_tmpC1;});_tmpC0;});Cyc_Tcutil_check_unique_tvars(
loc,td->tvs);Cyc_Tcutil_add_tvar_identities(td->tvs);if(td->defn != 0){Cyc_Tcutil_check_type(
loc,te,td->tvs,(void*)0,0,(void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);(
td->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(
void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);}else{struct Cyc_List_List*
tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){void*_tmpC2=Cyc_Absyn_compress_kb((void*)((
struct Cyc_Absyn_Tvar*)tvs->hd)->kind);struct Cyc_Core_Opt*_tmpC3;struct Cyc_Core_Opt**
_tmpC4;struct Cyc_Core_Opt*_tmpC5;struct Cyc_Core_Opt**_tmpC6;void*_tmpC7;_LL94:
if(*((int*)_tmpC2)!= 1)goto _LL96;_tmpC3=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC2)->f1;_tmpC4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC2)->f1;_LL95:*_tmpC4=({struct Cyc_Core_Opt*_tmpC8=_cycalloc(sizeof(*_tmpC8));
_tmpC8->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmpC9=_cycalloc(sizeof(*
_tmpC9));_tmpC9[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpCA;_tmpCA.tag=0;_tmpCA.f1=(
void*)((void*)2);_tmpCA;});_tmpC9;}));_tmpC8;});goto _LL93;_LL96: if(*((int*)
_tmpC2)!= 2)goto _LL98;_tmpC5=((struct Cyc_Absyn_Less_kb_struct*)_tmpC2)->f1;
_tmpC6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpC2)->f1;
_tmpC7=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpC2)->f2;_LL97:*_tmpC6=({
struct Cyc_Core_Opt*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->v=(void*)((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmpCD;_tmpCD.tag=0;_tmpCD.f1=(void*)_tmpC7;_tmpCD;});
_tmpCC;}));_tmpCB;});goto _LL93;_LL98:;_LL99: continue;_LL93:;}}ge->typedefs=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Typedefdecl*
v))Cyc_Dict_insert)(ge->typedefs,v,td);}static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){struct _RegionHandle
_tmpCE=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpCE;
_push_region(uprev_rgn);for(0;rpo != 0;rpo=rpo->tl){struct _tuple3 _tmpD0;void*
_tmpD1;void*_tmpD2;struct _tuple3*_tmpCF=(struct _tuple3*)rpo->hd;_tmpD0=*_tmpCF;
_tmpD1=_tmpD0.f1;_tmpD2=_tmpD0.f2;Cyc_Tcutil_check_type(loc,te,tvs,(void*)4,0,
_tmpD1);Cyc_Tcutil_check_type(loc,te,tvs,(void*)3,0,_tmpD2);}{struct Cyc_List_List*
prev_fields=0;struct Cyc_List_List*_tmpD3=fields;for(0;_tmpD3 != 0;_tmpD3=_tmpD3->tl){
struct Cyc_Absyn_Aggrfield _tmpD5;struct _tagged_arr*_tmpD6;struct Cyc_Absyn_Tqual
_tmpD7;void*_tmpD8;struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_List_List*_tmpDA;struct
Cyc_Absyn_Aggrfield*_tmpD4=(struct Cyc_Absyn_Aggrfield*)_tmpD3->hd;_tmpD5=*_tmpD4;
_tmpD6=_tmpD5.name;_tmpD7=_tmpD5.tq;_tmpD8=(void*)_tmpD5.type;_tmpD9=_tmpD5.width;
_tmpDA=_tmpD5.attributes;if(((int(*)(int(*compare)(struct _tagged_arr*,struct
_tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_strptrcmp,
prev_fields,_tmpD6))({struct Cyc_String_pa_struct _tmpDD;_tmpDD.tag=0;_tmpDD.f1=(
struct _tagged_arr)((struct _tagged_arr)*_tmpD6);{void*_tmpDB[1]={& _tmpDD};Cyc_Tcutil_terr(
loc,({const char*_tmpDC="duplicate field %s";_tag_arr(_tmpDC,sizeof(char),
_get_zero_arr_size(_tmpDC,19));}),_tag_arr(_tmpDB,sizeof(void*),1));}});if(Cyc_strcmp((
struct _tagged_arr)*_tmpD6,({const char*_tmpDE="";_tag_arr(_tmpDE,sizeof(char),
_get_zero_arr_size(_tmpDE,1));}))!= 0)prev_fields=({struct Cyc_List_List*_tmpDF=
_region_malloc(uprev_rgn,sizeof(*_tmpDF));_tmpDF->hd=_tmpD6;_tmpDF->tl=
prev_fields;_tmpDF;});Cyc_Tcutil_check_type(loc,te,tvs,(void*)1,0,_tmpD8);(((
struct Cyc_Absyn_Aggrfield*)_tmpD3->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(((struct Cyc_Absyn_Aggrfield*)_tmpD3->hd)->tq).print_const,_tmpD8);Cyc_Tcutil_check_bitfield(
loc,te,_tmpD8,_tmpD9,_tmpD6);}};_pop_region(uprev_rgn);}struct _tuple9{struct Cyc_Absyn_AggrdeclImpl*
f1;struct Cyc_Core_Opt*f2;};void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tagged_arr*_tmpE0=(*ad->name).f2;{struct Cyc_List_List*atts=ad->attributes;
for(0;atts != 0;atts=atts->tl){void*_tmpE1=(void*)atts->hd;_LL9B: if((int)_tmpE1 != 
5)goto _LL9D;_LL9C: goto _LL9E;_LL9D: if(_tmpE1 <= (void*)17  || *((int*)_tmpE1)!= 1)
goto _LL9F;_LL9E: continue;_LL9F:;_LLA0:({struct Cyc_String_pa_struct _tmpE5;_tmpE5.tag=
0;_tmpE5.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmpE0);{struct Cyc_String_pa_struct
_tmpE4;_tmpE4.tag=0;_tmpE4.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd));{void*_tmpE2[2]={& _tmpE4,& _tmpE5};Cyc_Tcutil_terr(loc,({const
char*_tmpE3="bad attribute %s in  %s definition";_tag_arr(_tmpE3,sizeof(char),
_get_zero_arr_size(_tmpE3,35));}),_tag_arr(_tmpE2,sizeof(void*),2));}}});goto
_LL9A;_LL9A:;}}{struct Cyc_List_List*_tmpE6=ad->tvs;{struct Cyc_List_List*tvs2=
_tmpE6;for(0;tvs2 != 0;tvs2=tvs2->tl){void*_tmpE7=Cyc_Absyn_compress_kb((void*)((
struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmpE8;struct Cyc_Core_Opt**
_tmpE9;struct Cyc_Core_Opt*_tmpEA;struct Cyc_Core_Opt**_tmpEB;void*_tmpEC;struct
Cyc_Core_Opt*_tmpED;struct Cyc_Core_Opt**_tmpEE;void*_tmpEF;void*_tmpF0;_LLA2: if(*((
int*)_tmpE7)!= 1)goto _LLA4;_tmpE8=((struct Cyc_Absyn_Unknown_kb_struct*)_tmpE7)->f1;
_tmpE9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmpE7)->f1;
_LLA3: _tmpEB=_tmpE9;goto _LLA5;_LLA4: if(*((int*)_tmpE7)!= 2)goto _LLA6;_tmpEA=((
struct Cyc_Absyn_Less_kb_struct*)_tmpE7)->f1;_tmpEB=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmpE7)->f1;_tmpEC=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpE7)->f2;if((int)_tmpEC != 1)goto _LLA6;_LLA5: _tmpEE=_tmpEB;goto _LLA7;_LLA6: if(*((
int*)_tmpE7)!= 2)goto _LLA8;_tmpED=((struct Cyc_Absyn_Less_kb_struct*)_tmpE7)->f1;
_tmpEE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpE7)->f1;
_tmpEF=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpE7)->f2;if((int)_tmpEF != 0)
goto _LLA8;_LLA7:*_tmpEE=({struct Cyc_Core_Opt*_tmpF1=_cycalloc(sizeof(*_tmpF1));
_tmpF1->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmpF2=_cycalloc(sizeof(*
_tmpF2));_tmpF2[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpF3;_tmpF3.tag=0;_tmpF3.f1=(
void*)((void*)2);_tmpF3;});_tmpF2;}));_tmpF1;});continue;_LLA8: if(*((int*)_tmpE7)
!= 0)goto _LLAA;_tmpF0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpE7)->f1;if((int)
_tmpF0 != 1)goto _LLAA;_LLA9:({struct Cyc_String_pa_struct _tmpF7;_tmpF7.tag=0;
_tmpF7.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);{
struct Cyc_String_pa_struct _tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmpE0);{void*_tmpF4[2]={& _tmpF6,& _tmpF7};Cyc_Tcutil_terr(loc,({
const char*_tmpF5="type %s attempts to abstract type variable %s of kind M";
_tag_arr(_tmpF5,sizeof(char),_get_zero_arr_size(_tmpF5,56));}),_tag_arr(_tmpF4,
sizeof(void*),2));}}});continue;_LLAA:;_LLAB: continue;_LLA1:;}}{void*_tmpF8=(*ad->name).f1;
struct Cyc_List_List*_tmpF9;struct Cyc_List_List*_tmpFA;_LLAD: if(_tmpF8 <= (void*)1
 || *((int*)_tmpF8)!= 0)goto _LLAF;_tmpF9=((struct Cyc_Absyn_Rel_n_struct*)_tmpF8)->f1;
if(_tmpF9 != 0)goto _LLAF;_LLAE: goto _LLB0;_LLAF: if(_tmpF8 <= (void*)1  || *((int*)
_tmpF8)!= 1)goto _LLB1;_tmpFA=((struct Cyc_Absyn_Abs_n_struct*)_tmpF8)->f1;if(
_tmpFA != 0)goto _LLB1;_LLB0: goto _LLAC;_LLB1:;_LLB2:({struct Cyc_String_pa_struct
_tmpFD;_tmpFD.tag=0;_tmpFD.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
ad->name));{void*_tmpFB[1]={& _tmpFD};Cyc_Tcutil_terr(loc,({const char*_tmpFC="qualified struct declarations are not implemented (%s)";
_tag_arr(_tmpFC,sizeof(char),_get_zero_arr_size(_tmpFC,55));}),_tag_arr(_tmpFB,
sizeof(void*),1));}});return;_LLAC:;}(*ad->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_Abs_n_struct _tmpFF;
_tmpFF.tag=1;_tmpFF.f1=te->ns;_tmpFF;});_tmpFE;});Cyc_Tcutil_check_unique_tvars(
loc,ad->tvs);Cyc_Tcutil_add_tvar_identities(ad->tvs);{struct _tuple9 _tmp101=({
struct _tuple9 _tmp100;_tmp100.f1=ad->impl;_tmp100.f2=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,
_tmpE0);_tmp100;});struct Cyc_Absyn_AggrdeclImpl*_tmp102;struct Cyc_Core_Opt*
_tmp103;struct Cyc_Absyn_AggrdeclImpl*_tmp104;struct Cyc_Absyn_AggrdeclImpl _tmp105;
struct Cyc_List_List*_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_List_List*
_tmp108;struct Cyc_Core_Opt*_tmp109;struct Cyc_Absyn_AggrdeclImpl*_tmp10A;struct
Cyc_Absyn_AggrdeclImpl _tmp10B;struct Cyc_List_List*_tmp10C;struct Cyc_List_List*
_tmp10D;struct Cyc_List_List*_tmp10E;struct Cyc_Core_Opt*_tmp10F;struct Cyc_Core_Opt
_tmp110;struct Cyc_Absyn_Aggrdecl**_tmp111;struct Cyc_Absyn_AggrdeclImpl*_tmp112;
struct Cyc_Core_Opt*_tmp113;struct Cyc_Core_Opt _tmp114;struct Cyc_Absyn_Aggrdecl**
_tmp115;_LLB4: _tmp102=_tmp101.f1;if(_tmp102 != 0)goto _LLB6;_tmp103=_tmp101.f2;if(
_tmp103 != 0)goto _LLB6;_LLB5: ge->aggrdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,
_tmpE0,({struct Cyc_Absyn_Aggrdecl**_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116[0]=
ad;_tmp116;}));goto _LLB3;_LLB6: _tmp104=_tmp101.f1;if(_tmp104 == 0)goto _LLB8;
_tmp105=*_tmp104;_tmp106=_tmp105.exist_vars;_tmp107=_tmp105.rgn_po;_tmp108=
_tmp105.fields;_tmp109=_tmp101.f2;if(_tmp109 != 0)goto _LLB8;_LLB7: {struct Cyc_Absyn_Aggrdecl**
_tmp117=({struct Cyc_Absyn_Aggrdecl**_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D[
0]=({struct Cyc_Absyn_Aggrdecl*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->kind=(
void*)((void*)ad->kind);_tmp11E->sc=(void*)((void*)3);_tmp11E->name=ad->name;
_tmp11E->tvs=_tmpE6;_tmp11E->impl=0;_tmp11E->attributes=ad->attributes;_tmp11E;});
_tmp11D;});ge->aggrdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmpE0,
_tmp117);Cyc_Tcutil_check_unique_tvars(loc,_tmp106);Cyc_Tcutil_add_tvar_identities(
_tmp106);Cyc_Tc_tcAggrImpl(te,ge,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmpE6,_tmp106),_tmp107,_tmp108);if((
void*)ad->kind == (void*)1){struct Cyc_List_List*f=_tmp108;for(0;f != 0;f=f->tl){
if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)f->hd)->type))({
struct Cyc_String_pa_struct _tmp11C;_tmp11C.tag=0;_tmp11C.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Absyn_Aggrfield*)f->hd)->type));{
struct Cyc_String_pa_struct _tmp11B;_tmp11B.tag=0;_tmp11B.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmpE0);{struct Cyc_String_pa_struct _tmp11A;_tmp11A.tag=0;
_tmp11A.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Aggrfield*)
f->hd)->name);{void*_tmp118[3]={& _tmp11A,& _tmp11B,& _tmp11C};Cyc_Tcutil_warn(loc,({
const char*_tmp119="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read";
_tag_arr(_tmp119,sizeof(char),_get_zero_arr_size(_tmp119,98));}),_tag_arr(
_tmp118,sizeof(void*),3));}}}});}}*_tmp117=ad;goto _LLB3;}_LLB8: _tmp10A=_tmp101.f1;
if(_tmp10A == 0)goto _LLBA;_tmp10B=*_tmp10A;_tmp10C=_tmp10B.exist_vars;_tmp10D=
_tmp10B.rgn_po;_tmp10E=_tmp10B.fields;_tmp10F=_tmp101.f2;if(_tmp10F == 0)goto
_LLBA;_tmp110=*_tmp10F;_tmp111=(struct Cyc_Absyn_Aggrdecl**)_tmp110.v;_LLB9: if((
void*)ad->kind != (void*)(*_tmp111)->kind)({void*_tmp11F[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp120="cannot reuse struct names for unions and vice-versa";
_tag_arr(_tmp120,sizeof(char),_get_zero_arr_size(_tmp120,52));}),_tag_arr(
_tmp11F,sizeof(void*),0));});{struct Cyc_Absyn_Aggrdecl*_tmp121=*_tmp111;*_tmp111=({
struct Cyc_Absyn_Aggrdecl*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->kind=(void*)((
void*)ad->kind);_tmp122->sc=(void*)((void*)3);_tmp122->name=ad->name;_tmp122->tvs=
_tmpE6;_tmp122->impl=0;_tmp122->attributes=ad->attributes;_tmp122;});Cyc_Tcutil_check_unique_tvars(
loc,_tmp10C);Cyc_Tcutil_add_tvar_identities(_tmp10C);Cyc_Tc_tcAggrImpl(te,ge,loc,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmpE6,_tmp10C),_tmp10D,_tmp10E);*_tmp111=_tmp121;_tmp115=_tmp111;goto _LLBB;}
_LLBA: _tmp112=_tmp101.f1;if(_tmp112 != 0)goto _LLB3;_tmp113=_tmp101.f2;if(_tmp113
== 0)goto _LLB3;_tmp114=*_tmp113;_tmp115=(struct Cyc_Absyn_Aggrdecl**)_tmp114.v;
_LLBB: {struct Cyc_Absyn_Aggrdecl*_tmp123=Cyc_Tcdecl_merge_aggrdecl(*_tmp115,ad,
loc,Cyc_Tc_tc_msg);if(_tmp123 == 0)return;else{*_tmp115=(struct Cyc_Absyn_Aggrdecl*)
_tmp123;ad=(struct Cyc_Absyn_Aggrdecl*)_tmp123;goto _LLB3;}}_LLB3:;}ge->ordinaries=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple7*
v))Cyc_Dict_insert)(ge->ordinaries,_tmpE0,({struct _tuple7*_tmp124=_cycalloc(
sizeof(*_tmp124));_tmp124->f1=(void*)({struct Cyc_Tcenv_AggrRes_struct*_tmp125=
_cycalloc(sizeof(*_tmp125));_tmp125[0]=({struct Cyc_Tcenv_AggrRes_struct _tmp126;
_tmp126.tag=1;_tmp126.f1=ad;_tmp126;});_tmp125;});_tmp124->f2=1;_tmp124;}));}}
struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_List_List*Cyc_Tc_tcTunionFields(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct _tagged_arr obj,int is_xtunion,struct _tuple0*name,struct Cyc_List_List*fields,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tuniondecl*tudres){{struct Cyc_List_List*
_tmp127=fields;for(0;_tmp127 != 0;_tmp127=_tmp127->tl){struct Cyc_Absyn_Tunionfield*
_tmp128=(struct Cyc_Absyn_Tunionfield*)_tmp127->hd;{struct Cyc_List_List*typs=
_tmp128->typs;for(0;typs != 0;typs=typs->tl){Cyc_Tcutil_check_type(_tmp128->loc,
te,tvs,(void*)1,0,(*((struct _tuple10*)typs->hd)).f2);((*((struct _tuple10*)typs->hd)).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(_tmp128->loc,((*((struct _tuple10*)typs->hd)).f1).print_const,(*((
struct _tuple10*)typs->hd)).f2);}}{void*_tmp129=(*_tmp128->name).f1;struct Cyc_List_List*
_tmp12A;_LLBD: if(_tmp129 <= (void*)1  || *((int*)_tmp129)!= 0)goto _LLBF;_tmp12A=((
struct Cyc_Absyn_Rel_n_struct*)_tmp129)->f1;if(_tmp12A != 0)goto _LLBF;_LLBE: if(
is_xtunion)(*_tmp128->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp12B=
_cycalloc(sizeof(*_tmp12B));_tmp12B[0]=({struct Cyc_Absyn_Abs_n_struct _tmp12C;
_tmp12C.tag=1;_tmp12C.f1=te->ns;_tmp12C;});_tmp12B;});else{(*_tmp128->name).f1=(*
name).f1;}goto _LLBC;_LLBF: if(_tmp129 <= (void*)1  || *((int*)_tmp129)!= 0)goto
_LLC1;_LLC0:({struct Cyc_String_pa_struct _tmp12F;_tmp12F.tag=0;_tmp12F.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp128->name));{void*
_tmp12D[1]={& _tmp12F};Cyc_Tcutil_terr(_tmp128->loc,({const char*_tmp12E="qualified tunionfield declarations are not allowed (%s)";
_tag_arr(_tmp12E,sizeof(char),_get_zero_arr_size(_tmp12E,56));}),_tag_arr(
_tmp12D,sizeof(void*),1));}});goto _LLBC;_LLC1: if(_tmp129 <= (void*)1  || *((int*)
_tmp129)!= 1)goto _LLC3;_LLC2: goto _LLBC;_LLC3: if((int)_tmp129 != 0)goto _LLBC;_LLC4:({
void*_tmp130[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp131="tcTunionFields: Loc_n";_tag_arr(_tmp131,sizeof(char),
_get_zero_arr_size(_tmp131,22));}),_tag_arr(_tmp130,sizeof(void*),0));});_LLBC:;}}}{
struct Cyc_List_List*fields2;if(is_xtunion){int _tmp132=1;struct Cyc_List_List*
_tmp133=Cyc_Tcdecl_sort_xtunion_fields(fields,& _tmp132,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp132)fields2=_tmp133;else{fields2=0;}}else{struct _RegionHandle _tmp134=
_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp134;_push_region(
uprev_rgn);{struct Cyc_List_List*prev_fields=0;{struct Cyc_List_List*fs=fields;
for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Tunionfield*_tmp135=(struct Cyc_Absyn_Tunionfield*)
fs->hd;if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*
_tmp135->name).f2))({struct Cyc_String_pa_struct _tmp139;_tmp139.tag=0;_tmp139.f1=(
struct _tagged_arr)((struct _tagged_arr)obj);{struct Cyc_String_pa_struct _tmp138;
_tmp138.tag=0;_tmp138.f1=(struct _tagged_arr)((struct _tagged_arr)*(*_tmp135->name).f2);{
void*_tmp136[2]={& _tmp138,& _tmp139};Cyc_Tcutil_terr(_tmp135->loc,({const char*
_tmp137="duplicate field name %s in %s";_tag_arr(_tmp137,sizeof(char),
_get_zero_arr_size(_tmp137,30));}),_tag_arr(_tmp136,sizeof(void*),2));}}});else{
prev_fields=({struct Cyc_List_List*_tmp13A=_region_malloc(uprev_rgn,sizeof(*
_tmp13A));_tmp13A->hd=(*_tmp135->name).f2;_tmp13A->tl=prev_fields;_tmp13A;});}
if((void*)_tmp135->sc != (void*)2){({struct Cyc_String_pa_struct _tmp13D;_tmp13D.tag=
0;_tmp13D.f1=(struct _tagged_arr)((struct _tagged_arr)*(*_tmp135->name).f2);{void*
_tmp13B[1]={& _tmp13D};Cyc_Tcutil_warn(loc,({const char*_tmp13C="ignoring scope of field %s";
_tag_arr(_tmp13C,sizeof(char),_get_zero_arr_size(_tmp13C,27));}),_tag_arr(
_tmp13B,sizeof(void*),1));}});(void*)(_tmp135->sc=(void*)((void*)2));}}}fields2=
fields;};_pop_region(uprev_rgn);}{struct Cyc_List_List*_tmp13E=fields;for(0;
_tmp13E != 0;_tmp13E=_tmp13E->tl){struct Cyc_Absyn_Tunionfield*_tmp13F=(struct Cyc_Absyn_Tunionfield*)
_tmp13E->hd;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct _tuple7*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp13F->name).f2,({
struct _tuple7*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140->f1=(void*)({struct Cyc_Tcenv_TunionRes_struct*
_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141[0]=({struct Cyc_Tcenv_TunionRes_struct
_tmp142;_tmp142.tag=2;_tmp142.f1=tudres;_tmp142.f2=_tmp13F;_tmp142;});_tmp141;});
_tmp140->f2=1;_tmp140;}));}}return fields2;}}struct _tuple11{struct Cyc_Core_Opt*f1;
struct Cyc_Core_Opt*f2;};void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl*tud){
struct _tagged_arr*v=(*tud->name).f2;struct _tagged_arr obj=tud->is_xtunion?({const
char*_tmp181="xtunion";_tag_arr(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,
8));}):({const char*_tmp182="tunion";_tag_arr(_tmp182,sizeof(char),
_get_zero_arr_size(_tmp182,7));});struct Cyc_List_List*tvs=tud->tvs;{struct Cyc_List_List*
tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){void*_tmp143=Cyc_Absyn_compress_kb((void*)((
struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp144;struct Cyc_Core_Opt**
_tmp145;struct Cyc_Core_Opt*_tmp146;struct Cyc_Core_Opt**_tmp147;void*_tmp148;
struct Cyc_Core_Opt*_tmp149;struct Cyc_Core_Opt**_tmp14A;void*_tmp14B;void*_tmp14C;
_LLC6: if(*((int*)_tmp143)!= 1)goto _LLC8;_tmp144=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp143)->f1;_tmp145=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp143)->f1;_LLC7: _tmp147=_tmp145;goto _LLC9;_LLC8: if(*((int*)_tmp143)!= 2)goto
_LLCA;_tmp146=((struct Cyc_Absyn_Less_kb_struct*)_tmp143)->f1;_tmp147=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp143)->f1;_tmp148=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp143)->f2;if((int)_tmp148 != 1)goto _LLCA;_LLC9: _tmp14A=_tmp147;goto _LLCB;_LLCA:
if(*((int*)_tmp143)!= 2)goto _LLCC;_tmp149=((struct Cyc_Absyn_Less_kb_struct*)
_tmp143)->f1;_tmp14A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp143)->f1;_tmp14B=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp143)->f2;if((
int)_tmp14B != 0)goto _LLCC;_LLCB:*_tmp14A=({struct Cyc_Core_Opt*_tmp14D=_cycalloc(
sizeof(*_tmp14D));_tmp14D->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp14F;_tmp14F.tag=0;_tmp14F.f1=(void*)((void*)2);_tmp14F;});_tmp14E;}));
_tmp14D;});goto _LLC5;_LLCC: if(*((int*)_tmp143)!= 0)goto _LLCE;_tmp14C=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp143)->f1;if((int)_tmp14C != 1)goto _LLCE;_LLCD:({
struct Cyc_String_pa_struct _tmp154;_tmp154.tag=0;_tmp154.f1=(struct _tagged_arr)((
struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);{struct Cyc_String_pa_struct
_tmp153;_tmp153.tag=0;_tmp153.f1=(struct _tagged_arr)((struct _tagged_arr)*v);{
struct Cyc_String_pa_struct _tmp152;_tmp152.tag=0;_tmp152.f1=(struct _tagged_arr)((
struct _tagged_arr)obj);{void*_tmp150[3]={& _tmp152,& _tmp153,& _tmp154};Cyc_Tcutil_terr(
loc,({const char*_tmp151="%s %s attempts to abstract type variable %s of kind M";
_tag_arr(_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,54));}),_tag_arr(
_tmp150,sizeof(void*),3));}}}});goto _LLC5;_LLCE:;_LLCF: goto _LLC5;_LLC5:;}}Cyc_Tcutil_check_unique_tvars(
loc,tvs);Cyc_Tcutil_add_tvar_identities(tvs);{struct Cyc_Core_Opt*tud_opt;if(tud->is_xtunion){{
struct _handler_cons _tmp155;_push_handler(& _tmp155);{int _tmp157=0;if(setjmp(
_tmp155.handler))_tmp157=1;if(!_tmp157){tud_opt=Cyc_Tcenv_lookup_xtuniondecl(te,
loc,tud->name);;_pop_handler();}else{void*_tmp156=(void*)_exn_thrown;void*
_tmp159=_tmp156;_LLD1: if(_tmp159 != Cyc_Dict_Absent)goto _LLD3;_LLD2:({struct Cyc_String_pa_struct
_tmp15C;_tmp15C.tag=0;_tmp15C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
tud->name));{void*_tmp15A[1]={& _tmp15C};Cyc_Tcutil_terr(loc,({const char*_tmp15B="qualified xtunion declaration %s is not an existing xtunion";
_tag_arr(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,60));}),_tag_arr(
_tmp15A,sizeof(void*),1));}});return;_LLD3:;_LLD4:(void)_throw(_tmp159);_LLD0:;}}}
if(tud_opt != 0)tud->name=(*((struct Cyc_Absyn_Tuniondecl**)tud_opt->v))->name;
else{(*tud->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp15D=_cycalloc(
sizeof(*_tmp15D));_tmp15D[0]=({struct Cyc_Absyn_Abs_n_struct _tmp15E;_tmp15E.tag=1;
_tmp15E.f1=te->ns;_tmp15E;});_tmp15D;});}}else{{void*_tmp15F=(*tud->name).f1;
struct Cyc_List_List*_tmp160;_LLD6: if(_tmp15F <= (void*)1  || *((int*)_tmp15F)!= 0)
goto _LLD8;_tmp160=((struct Cyc_Absyn_Rel_n_struct*)_tmp15F)->f1;if(_tmp160 != 0)
goto _LLD8;_LLD7:(*tud->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp161=
_cycalloc(sizeof(*_tmp161));_tmp161[0]=({struct Cyc_Absyn_Abs_n_struct _tmp162;
_tmp162.tag=1;_tmp162.f1=te->ns;_tmp162;});_tmp161;});goto _LLD5;_LLD8: if(_tmp15F
<= (void*)1  || *((int*)_tmp15F)!= 1)goto _LLDA;_LLD9: goto _LLDB;_LLDA:;_LLDB:({
struct Cyc_String_pa_struct _tmp165;_tmp165.tag=0;_tmp165.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(tud->name));{void*_tmp163[1]={& _tmp165};
Cyc_Tcutil_terr(loc,({const char*_tmp164="qualified tunion declarations are not implemented (%s)";
_tag_arr(_tmp164,sizeof(char),_get_zero_arr_size(_tmp164,55));}),_tag_arr(
_tmp163,sizeof(void*),1));}});return;_LLD5:;}tud_opt=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)(ge->tuniondecls,
v);}{struct _tuple11 _tmp167=({struct _tuple11 _tmp166;_tmp166.f1=tud->fields;
_tmp166.f2=tud_opt;_tmp166;});struct Cyc_Core_Opt*_tmp168;struct Cyc_Core_Opt*
_tmp169;struct Cyc_Core_Opt*_tmp16A;struct Cyc_Core_Opt _tmp16B;struct Cyc_List_List*
_tmp16C;struct Cyc_List_List**_tmp16D;struct Cyc_Core_Opt*_tmp16E;struct Cyc_Core_Opt*
_tmp16F;struct Cyc_Core_Opt _tmp170;struct Cyc_List_List*_tmp171;struct Cyc_List_List**
_tmp172;struct Cyc_Core_Opt*_tmp173;struct Cyc_Core_Opt _tmp174;struct Cyc_Absyn_Tuniondecl**
_tmp175;struct Cyc_Core_Opt*_tmp176;struct Cyc_Core_Opt*_tmp177;struct Cyc_Core_Opt
_tmp178;struct Cyc_Absyn_Tuniondecl**_tmp179;_LLDD: _tmp168=_tmp167.f1;if(_tmp168
!= 0)goto _LLDF;_tmp169=_tmp167.f2;if(_tmp169 != 0)goto _LLDF;_LLDE: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl**
v))Cyc_Dict_insert)(ge->tuniondecls,v,({struct Cyc_Absyn_Tuniondecl**_tmp17A=
_cycalloc(sizeof(*_tmp17A));_tmp17A[0]=tud;_tmp17A;}));goto _LLDC;_LLDF: _tmp16A=
_tmp167.f1;if(_tmp16A == 0)goto _LLE1;_tmp16B=*_tmp16A;_tmp16C=(struct Cyc_List_List*)
_tmp16B.v;_tmp16D=(struct Cyc_List_List**)&(*_tmp167.f1).v;_tmp16E=_tmp167.f2;if(
_tmp16E != 0)goto _LLE1;_LLE0: {struct Cyc_Absyn_Tuniondecl**_tmp17B=({struct Cyc_Absyn_Tuniondecl**
_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C[0]=({struct Cyc_Absyn_Tuniondecl*
_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->sc=(void*)((void*)3);_tmp17D->name=
tud->name;_tmp17D->tvs=tvs;_tmp17D->fields=0;_tmp17D->is_xtunion=tud->is_xtunion;
_tmp17D;});_tmp17C;});ge->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl**v))Cyc_Dict_insert)(ge->tuniondecls,
v,_tmp17B);*_tmp16D=Cyc_Tc_tcTunionFields(te,ge,loc,obj,tud->is_xtunion,tud->name,*
_tmp16D,tvs,tud);*_tmp17B=tud;goto _LLDC;}_LLE1: _tmp16F=_tmp167.f1;if(_tmp16F == 0)
goto _LLE3;_tmp170=*_tmp16F;_tmp171=(struct Cyc_List_List*)_tmp170.v;_tmp172=(
struct Cyc_List_List**)&(*_tmp167.f1).v;_tmp173=_tmp167.f2;if(_tmp173 == 0)goto
_LLE3;_tmp174=*_tmp173;_tmp175=(struct Cyc_Absyn_Tuniondecl**)_tmp174.v;_LLE2: {
struct Cyc_Absyn_Tuniondecl*_tmp17E=*_tmp175;*_tmp175=({struct Cyc_Absyn_Tuniondecl*
_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->sc=(void*)((void*)3);_tmp17F->name=
tud->name;_tmp17F->tvs=tvs;_tmp17F->fields=0;_tmp17F->is_xtunion=tud->is_xtunion;
_tmp17F;});*_tmp172=Cyc_Tc_tcTunionFields(te,ge,loc,obj,tud->is_xtunion,tud->name,*
_tmp172,tvs,tud);*_tmp175=_tmp17E;_tmp179=_tmp175;goto _LLE4;}_LLE3: _tmp176=
_tmp167.f1;if(_tmp176 != 0)goto _LLDC;_tmp177=_tmp167.f2;if(_tmp177 == 0)goto _LLDC;
_tmp178=*_tmp177;_tmp179=(struct Cyc_Absyn_Tuniondecl**)_tmp178.v;_LLE4: {struct
Cyc_Absyn_Tuniondecl*_tmp180=Cyc_Tcdecl_merge_tuniondecl(*_tmp179,tud,loc,Cyc_Tc_tc_msg);
if(_tmp180 == 0)return;else{*_tmp179=(struct Cyc_Absyn_Tuniondecl*)_tmp180;goto
_LLDC;}}_LLDC:;}}}void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Enumdecl*ed){struct _tagged_arr*
v=(*ed->name).f2;{void*_tmp183=(*ed->name).f1;struct Cyc_List_List*_tmp184;struct
Cyc_List_List*_tmp185;_LLE6: if(_tmp183 <= (void*)1  || *((int*)_tmp183)!= 0)goto
_LLE8;_tmp184=((struct Cyc_Absyn_Rel_n_struct*)_tmp183)->f1;if(_tmp184 != 0)goto
_LLE8;_LLE7: goto _LLE9;_LLE8: if(_tmp183 <= (void*)1  || *((int*)_tmp183)!= 1)goto
_LLEA;_tmp185=((struct Cyc_Absyn_Abs_n_struct*)_tmp183)->f1;if(_tmp185 != 0)goto
_LLEA;_LLE9: goto _LLE5;_LLEA:;_LLEB:({struct Cyc_String_pa_struct _tmp188;_tmp188.tag=
0;_tmp188.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(ed->name));{
void*_tmp186[1]={& _tmp188};Cyc_Tcutil_terr(loc,({const char*_tmp187="qualified enum declarations are not implemented (%s)";
_tag_arr(_tmp187,sizeof(char),_get_zero_arr_size(_tmp187,53));}),_tag_arr(
_tmp186,sizeof(void*),1));}});return;_LLE5:;}(*ed->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp189=_cycalloc(sizeof(*_tmp189));_tmp189[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp18A;_tmp18A.tag=1;_tmp18A.f1=te->ns;_tmp18A;});_tmp189;});if(ed->fields != 0){
struct _RegionHandle _tmp18B=_new_region("uprev_rgn");struct _RegionHandle*
uprev_rgn=& _tmp18B;_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp18C=(struct Cyc_Absyn_Enumfield*)fs->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_strptrcmp,prev_fields,(*_tmp18C->name).f2))({struct Cyc_String_pa_struct
_tmp18F;_tmp18F.tag=0;_tmp18F.f1=(struct _tagged_arr)((struct _tagged_arr)*(*
_tmp18C->name).f2);{void*_tmp18D[1]={& _tmp18F};Cyc_Tcutil_terr(_tmp18C->loc,({
const char*_tmp18E="duplicate field name %s";_tag_arr(_tmp18E,sizeof(char),
_get_zero_arr_size(_tmp18E,24));}),_tag_arr(_tmp18D,sizeof(void*),1));}});else{
prev_fields=({struct Cyc_List_List*_tmp190=_region_malloc(uprev_rgn,sizeof(*
_tmp190));_tmp190->hd=(*_tmp18C->name).f2;_tmp190->tl=prev_fields;_tmp190;});}
if(_tmp18C->tag == 0)_tmp18C->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp18C->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp18C->tag)))({struct Cyc_String_pa_struct _tmp194;_tmp194.tag=0;
_tmp194.f1=(struct _tagged_arr)((struct _tagged_arr)*(*_tmp18C->name).f2);{struct
Cyc_String_pa_struct _tmp193;_tmp193.tag=0;_tmp193.f1=(struct _tagged_arr)((struct
_tagged_arr)*v);{void*_tmp191[2]={& _tmp193,& _tmp194};Cyc_Tcutil_terr(loc,({const
char*_tmp192="enum %s, field %s: expression is not constant";_tag_arr(_tmp192,
sizeof(char),_get_zero_arr_size(_tmp192,46));}),_tag_arr(_tmp191,sizeof(void*),2));}}});}{
unsigned int _tmp196;int _tmp197;struct _tuple4 _tmp195=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp18C->tag));_tmp196=_tmp195.f1;_tmp197=
_tmp195.f2;if(!_tmp197)({void*_tmp198[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp199="Cyclone enum tags cannot use sizeof or offsetof";_tag_arr(_tmp199,
sizeof(char),_get_zero_arr_size(_tmp199,48));}),_tag_arr(_tmp198,sizeof(void*),0));});
tag_count=_tmp196 + 1;(*_tmp18C->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp19B;_tmp19B.tag=1;_tmp19B.f1=te->ns;_tmp19B;});_tmp19A;});}}};_pop_region(
uprev_rgn);}{struct _handler_cons _tmp19C;_push_handler(& _tmp19C);{int _tmp19E=0;
if(setjmp(_tmp19C.handler))_tmp19E=1;if(!_tmp19E){{struct Cyc_Absyn_Enumdecl**
_tmp19F=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*
k))Cyc_Dict_lookup)(ge->enumdecls,v);struct Cyc_Absyn_Enumdecl*_tmp1A0=Cyc_Tcdecl_merge_enumdecl(*
_tmp19F,ed,loc,Cyc_Tc_tc_msg);if(_tmp1A0 == 0){_npop_handler(0);return;}*_tmp19F=(
struct Cyc_Absyn_Enumdecl*)_tmp1A0;};_pop_handler();}else{void*_tmp19D=(void*)
_exn_thrown;void*_tmp1A2=_tmp19D;_LLED: if(_tmp1A2 != Cyc_Dict_Absent)goto _LLEF;
_LLEE: {struct Cyc_Absyn_Enumdecl**_tmp1A3=({struct Cyc_Absyn_Enumdecl**_tmp1A4=
_cycalloc(sizeof(*_tmp1A4));_tmp1A4[0]=ed;_tmp1A4;});ge->enumdecls=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(
ge->enumdecls,v,_tmp1A3);goto _LLEC;}_LLEF:;_LLF0:(void)_throw(_tmp1A2);_LLEC:;}}}
if(ed->fields != 0){struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp1A5=(struct Cyc_Absyn_Enumfield*)fs->hd;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple7*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp1A5->name).f2,({struct _tuple7*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->f1=(
void*)({struct Cyc_Tcenv_EnumRes_struct*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));
_tmp1A7[0]=({struct Cyc_Tcenv_EnumRes_struct _tmp1A8;_tmp1A8.tag=3;_tmp1A8.f1=ed;
_tmp1A8.f2=_tmp1A5;_tmp1A8;});_tmp1A7;});_tmp1A6->f2=1;_tmp1A6;}));}}}static int
Cyc_Tc_okay_externC(struct Cyc_Position_Segment*loc,void*sc){void*_tmp1A9=sc;
_LLF2: if((int)_tmp1A9 != 0)goto _LLF4;_LLF3:({void*_tmp1AA[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp1AB="static declaration nested within extern \"C\"";_tag_arr(
_tmp1AB,sizeof(char),_get_zero_arr_size(_tmp1AB,44));}),_tag_arr(_tmp1AA,sizeof(
void*),0));});return 0;_LLF4: if((int)_tmp1A9 != 1)goto _LLF6;_LLF5:({void*_tmp1AC[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp1AD="abstract declaration nested within extern \"C\"";
_tag_arr(_tmp1AD,sizeof(char),_get_zero_arr_size(_tmp1AD,46));}),_tag_arr(
_tmp1AC,sizeof(void*),0));});return 0;_LLF6: if((int)_tmp1A9 != 2)goto _LLF8;_LLF7:
goto _LLF9;_LLF8: if((int)_tmp1A9 != 5)goto _LLFA;_LLF9: goto _LLFB;_LLFA: if((int)
_tmp1A9 != 3)goto _LLFC;_LLFB: return 1;_LLFC: if((int)_tmp1A9 != 4)goto _LLF1;_LLFD:({
void*_tmp1AE[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp1AF="nested extern \"C\" declaration";
_tag_arr(_tmp1AF,sizeof(char),_get_zero_arr_size(_tmp1AF,30));}),_tag_arr(
_tmp1AE,sizeof(void*),0));});return 1;_LLF1:;}static void Cyc_Tc_resolve_export_namespace(
struct Cyc_Tcenv_Tenv*te,struct _tuple6*exp){struct Cyc_Position_Segment*_tmp1B1;
struct _tuple0*_tmp1B2;struct _tuple6 _tmp1B0=*exp;_tmp1B1=_tmp1B0.f1;_tmp1B2=
_tmp1B0.f2;{struct _tuple0 _tmp1B4;void*_tmp1B5;struct _tagged_arr*_tmp1B6;struct
_tuple0*_tmp1B3=_tmp1B2;_tmp1B4=*_tmp1B3;_tmp1B5=_tmp1B4.f1;_tmp1B6=_tmp1B4.f2;{
void*_tmp1B7=_tmp1B5;struct Cyc_List_List*_tmp1B8;struct Cyc_List_List*_tmp1B9;
_LLFF: if(_tmp1B7 <= (void*)1  || *((int*)_tmp1B7)!= 0)goto _LL101;_tmp1B8=((struct
Cyc_Absyn_Rel_n_struct*)_tmp1B7)->f1;if(_tmp1B8 != 0)goto _LL101;_LL100: goto _LL102;
_LL101: if(_tmp1B7 <= (void*)1  || *((int*)_tmp1B7)!= 1)goto _LL103;_tmp1B9=((struct
Cyc_Absyn_Abs_n_struct*)_tmp1B7)->f1;if(_tmp1B9 != 0)goto _LL103;_LL102: goto _LLFE;
_LL103:;_LL104:({struct Cyc_String_pa_struct _tmp1BC;_tmp1BC.tag=0;_tmp1BC.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1B2));{void*
_tmp1BA[1]={& _tmp1BC};Cyc_Tcutil_terr(_tmp1B1,({const char*_tmp1BB="qualified export variables are not implemented (%s)";
_tag_arr(_tmp1BB,sizeof(char),_get_zero_arr_size(_tmp1BB,52));}),_tag_arr(
_tmp1BA,sizeof(void*),1));}});return;_LLFE:;}(*_tmp1B2).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp1BE;_tmp1BE.tag=1;_tmp1BE.f1=te->ns;_tmp1BE;});_tmp1BD;});}}static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int
in_externCinclude,int check_var_init,struct Cyc_List_List**exports){struct Cyc_Tcenv_Genv*
ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_List_List*last=0;struct Cyc_Dict_Dict*dict=Cyc_Tcgenrep_empty_typerep_dict();
struct Cyc_List_List*_tmp1BF=ds0;for(0;_tmp1BF != 0;(last=_tmp1BF,_tmp1BF=_tmp1BF->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1BF->hd;struct Cyc_Position_Segment*
loc=d->loc;void*_tmp1C0=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp1C1;struct Cyc_Absyn_Fndecl*
_tmp1C2;struct Cyc_Absyn_Typedefdecl*_tmp1C3;struct Cyc_Absyn_Aggrdecl*_tmp1C4;
struct Cyc_Absyn_Tuniondecl*_tmp1C5;struct Cyc_Absyn_Enumdecl*_tmp1C6;struct
_tagged_arr*_tmp1C7;struct Cyc_List_List*_tmp1C8;struct _tuple0*_tmp1C9;struct
_tuple0 _tmp1CA;void*_tmp1CB;struct _tagged_arr*_tmp1CC;struct Cyc_List_List*
_tmp1CD;struct Cyc_List_List*_tmp1CE;struct Cyc_List_List*_tmp1CF;struct Cyc_List_List*
_tmp1D0;_LL106: if(_tmp1C0 <= (void*)2  || *((int*)_tmp1C0)!= 2)goto _LL108;_LL107:
goto _LL109;_LL108: if(_tmp1C0 <= (void*)2  || *((int*)_tmp1C0)!= 3)goto _LL10A;
_LL109:({void*_tmp1D1[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1D2="top level let-declarations are not implemented";
_tag_arr(_tmp1D2,sizeof(char),_get_zero_arr_size(_tmp1D2,47));}),_tag_arr(
_tmp1D1,sizeof(void*),0));});goto _LL105;_LL10A: if(_tmp1C0 <= (void*)2  || *((int*)
_tmp1C0)!= 0)goto _LL10C;_tmp1C1=((struct Cyc_Absyn_Var_d_struct*)_tmp1C0)->f1;
_LL10B: if(in_externC  && Cyc_Tc_okay_externC(d->loc,(void*)_tmp1C1->sc))(void*)(
_tmp1C1->sc=(void*)((void*)4));if(_tmp1C1->initializer != 0){void*_tmp1D3=(void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp1C1->initializer))->r;void*_tmp1D4;_LL123:
if(*((int*)_tmp1D3)!= 21)goto _LL125;_tmp1D4=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp1D3)->f2;_LL124: {struct Cyc_Dict_Dict*_tmp1D6;struct Cyc_List_List*_tmp1D7;
struct Cyc_Absyn_Exp*_tmp1D8;struct _tuple5 _tmp1D5=Cyc_Tcgenrep_tcGenrep(te,ge,loc,
_tmp1D4,dict);_tmp1D6=_tmp1D5.f1;_tmp1D7=_tmp1D5.f2;_tmp1D8=_tmp1D5.f3;dict=
_tmp1D6;Cyc_Tc_tc_decls(te,_tmp1D7,in_externC,in_externCinclude,check_var_init,
exports);_tmp1C1->initializer=(struct Cyc_Absyn_Exp*)_tmp1D8;Cyc_Tc_tcVardecl(te,
ge,loc,_tmp1C1,check_var_init,in_externCinclude,exports);if(_tmp1D7 != 0){if(last
!= 0){((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp1D7,_tmp1BF);last->tl=_tmp1D7;}else{struct Cyc_List_List tmp=({struct Cyc_List_List
_tmp1D9;_tmp1D9.hd=(struct Cyc_Absyn_Decl*)_tmp1BF->hd;_tmp1D9.tl=_tmp1BF->tl;
_tmp1D9;});(struct Cyc_Absyn_Decl*)(_tmp1BF->hd=(void*)((struct Cyc_Absyn_Decl*)
_tmp1D7->hd));_tmp1BF->tl=_tmp1D7->tl;(struct Cyc_Absyn_Decl*)(_tmp1D7->hd=(void*)((
struct Cyc_Absyn_Decl*)tmp.hd));_tmp1D7->tl=tmp.tl;((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1BF,
_tmp1D7);}}continue;}_LL125:;_LL126: goto _LL122;_LL122:;}Cyc_Tc_tcVardecl(te,ge,
loc,_tmp1C1,check_var_init,in_externCinclude,exports);goto _LL105;_LL10C: if(
_tmp1C0 <= (void*)2  || *((int*)_tmp1C0)!= 1)goto _LL10E;_tmp1C2=((struct Cyc_Absyn_Fn_d_struct*)
_tmp1C0)->f1;_LL10D: if(in_externC  && Cyc_Tc_okay_externC(d->loc,(void*)_tmp1C2->sc))(
void*)(_tmp1C2->sc=(void*)((void*)4));Cyc_Tc_tcFndecl(te,ge,loc,_tmp1C2,
in_externCinclude,exports);goto _LL105;_LL10E: if(_tmp1C0 <= (void*)2  || *((int*)
_tmp1C0)!= 7)goto _LL110;_tmp1C3=((struct Cyc_Absyn_Typedef_d_struct*)_tmp1C0)->f1;
_LL10F: Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp1C3);goto _LL105;_LL110: if(_tmp1C0 <= (
void*)2  || *((int*)_tmp1C0)!= 4)goto _LL112;_tmp1C4=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp1C0)->f1;_LL111: if(in_externC  && Cyc_Tc_okay_externC(d->loc,(void*)_tmp1C4->sc))(
void*)(_tmp1C4->sc=(void*)((void*)4));Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp1C4);goto
_LL105;_LL112: if(_tmp1C0 <= (void*)2  || *((int*)_tmp1C0)!= 5)goto _LL114;_tmp1C5=((
struct Cyc_Absyn_Tunion_d_struct*)_tmp1C0)->f1;_LL113: if(in_externC  && Cyc_Tc_okay_externC(
d->loc,(void*)_tmp1C5->sc))(void*)(_tmp1C5->sc=(void*)((void*)4));Cyc_Tc_tcTuniondecl(
te,ge,loc,_tmp1C5);goto _LL105;_LL114: if(_tmp1C0 <= (void*)2  || *((int*)_tmp1C0)!= 
6)goto _LL116;_tmp1C6=((struct Cyc_Absyn_Enum_d_struct*)_tmp1C0)->f1;_LL115: if(
in_externC  && Cyc_Tc_okay_externC(d->loc,(void*)_tmp1C6->sc))(void*)(_tmp1C6->sc=(
void*)((void*)4));Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp1C6);goto _LL105;_LL116: if((int)
_tmp1C0 != 0)goto _LL118;_LL117:({void*_tmp1DA[0]={};Cyc_Tcutil_warn(d->loc,({
const char*_tmp1DB="spurious __cyclone_port_on__";_tag_arr(_tmp1DB,sizeof(char),
_get_zero_arr_size(_tmp1DB,29));}),_tag_arr(_tmp1DA,sizeof(void*),0));});goto
_LL105;_LL118: if((int)_tmp1C0 != 1)goto _LL11A;_LL119: goto _LL105;_LL11A: if(_tmp1C0
<= (void*)2  || *((int*)_tmp1C0)!= 8)goto _LL11C;_tmp1C7=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp1C0)->f1;_tmp1C8=((struct Cyc_Absyn_Namespace_d_struct*)_tmp1C0)->f2;_LL11B: {
struct Cyc_List_List*_tmp1DC=te->ns;struct Cyc_List_List*_tmp1DD=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1DC,({struct
Cyc_List_List*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE->hd=_tmp1C7;_tmp1DE->tl=
0;_tmp1DE;}));if(!((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
ge->namespaces,_tmp1C7)){ge->namespaces=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_insert)(ge->namespaces,_tmp1C7);te->ae=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*
v))Cyc_Dict_insert)(te->ae,_tmp1DD,Cyc_Tcenv_empty_genv());}te->ns=_tmp1DD;Cyc_Tc_tc_decls(
te,_tmp1C8,in_externC,in_externCinclude,check_var_init,exports);te->ns=_tmp1DC;
goto _LL105;}_LL11C: if(_tmp1C0 <= (void*)2  || *((int*)_tmp1C0)!= 9)goto _LL11E;
_tmp1C9=((struct Cyc_Absyn_Using_d_struct*)_tmp1C0)->f1;_tmp1CA=*_tmp1C9;_tmp1CB=
_tmp1CA.f1;_tmp1CC=_tmp1CA.f2;_tmp1CD=((struct Cyc_Absyn_Using_d_struct*)_tmp1C0)->f2;
_LL11D: {struct _tagged_arr*first;struct Cyc_List_List*rest;{void*_tmp1DF=_tmp1CB;
struct Cyc_List_List*_tmp1E0;struct Cyc_List_List*_tmp1E1;struct Cyc_List_List*
_tmp1E2;struct Cyc_List_List _tmp1E3;struct _tagged_arr*_tmp1E4;struct Cyc_List_List*
_tmp1E5;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List _tmp1E7;struct _tagged_arr*
_tmp1E8;struct Cyc_List_List*_tmp1E9;_LL128: if((int)_tmp1DF != 0)goto _LL12A;_LL129:
goto _LL12B;_LL12A: if(_tmp1DF <= (void*)1  || *((int*)_tmp1DF)!= 0)goto _LL12C;
_tmp1E0=((struct Cyc_Absyn_Rel_n_struct*)_tmp1DF)->f1;if(_tmp1E0 != 0)goto _LL12C;
_LL12B: goto _LL12D;_LL12C: if(_tmp1DF <= (void*)1  || *((int*)_tmp1DF)!= 1)goto
_LL12E;_tmp1E1=((struct Cyc_Absyn_Abs_n_struct*)_tmp1DF)->f1;if(_tmp1E1 != 0)goto
_LL12E;_LL12D: first=_tmp1CC;rest=0;goto _LL127;_LL12E: if(_tmp1DF <= (void*)1  || *((
int*)_tmp1DF)!= 0)goto _LL130;_tmp1E2=((struct Cyc_Absyn_Rel_n_struct*)_tmp1DF)->f1;
if(_tmp1E2 == 0)goto _LL130;_tmp1E3=*_tmp1E2;_tmp1E4=(struct _tagged_arr*)_tmp1E3.hd;
_tmp1E5=_tmp1E3.tl;_LL12F: _tmp1E8=_tmp1E4;_tmp1E9=_tmp1E5;goto _LL131;_LL130: if(
_tmp1DF <= (void*)1  || *((int*)_tmp1DF)!= 1)goto _LL127;_tmp1E6=((struct Cyc_Absyn_Abs_n_struct*)
_tmp1DF)->f1;if(_tmp1E6 == 0)goto _LL127;_tmp1E7=*_tmp1E6;_tmp1E8=(struct
_tagged_arr*)_tmp1E7.hd;_tmp1E9=_tmp1E7.tl;_LL131: first=_tmp1E8;rest=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1E9,({struct
Cyc_List_List*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA->hd=_tmp1CC;_tmp1EA->tl=
0;_tmp1EA;}));goto _LL127;_LL127:;}{struct Cyc_List_List*_tmp1EB=Cyc_Tcenv_resolve_namespace(
te,loc,first,rest);ge->availables=({struct Cyc_List_List*_tmp1EC=_cycalloc(
sizeof(*_tmp1EC));_tmp1EC->hd=_tmp1EB;_tmp1EC->tl=ge->availables;_tmp1EC;});Cyc_Tc_tc_decls(
te,_tmp1CD,in_externC,in_externCinclude,check_var_init,exports);ge->availables=((
struct Cyc_List_List*)_check_null(ge->availables))->tl;goto _LL105;}}_LL11E: if(
_tmp1C0 <= (void*)2  || *((int*)_tmp1C0)!= 10)goto _LL120;_tmp1CE=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp1C0)->f1;_LL11F: Cyc_Tc_tc_decls(te,_tmp1CE,1,in_externCinclude,
check_var_init,exports);goto _LL105;_LL120: if(_tmp1C0 <= (void*)2  || *((int*)
_tmp1C0)!= 11)goto _LL105;_tmp1CF=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp1C0)->f1;_tmp1D0=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp1C0)->f2;
_LL121:((void(*)(void(*f)(struct Cyc_Tcenv_Tenv*,struct _tuple6*),struct Cyc_Tcenv_Tenv*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,
_tmp1D0);{struct Cyc_List_List*newexs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1D0,(unsigned int)exports?*exports:
0);Cyc_Tc_tc_decls(te,_tmp1CF,1,1,check_var_init,(struct Cyc_List_List**)& newexs);
for(0;_tmp1D0 != 0;_tmp1D0=_tmp1D0->tl){struct _tuple6*_tmp1ED=(struct _tuple6*)
_tmp1D0->hd;if(!(*_tmp1ED).f3)({struct Cyc_String_pa_struct _tmp1F0;_tmp1F0.tag=0;
_tmp1F0.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string((*
_tmp1ED).f2));{void*_tmp1EE[1]={& _tmp1F0};Cyc_Tcutil_warn((*_tmp1ED).f1,({const
char*_tmp1EF="%s is exported but not defined";_tag_arr(_tmp1EF,sizeof(char),
_get_zero_arr_size(_tmp1EF,31));}),_tag_arr(_tmp1EE,sizeof(void*),1));}});}goto
_LL105;}_LL105:;}}void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int check_var_init,struct
Cyc_List_List*ds){Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);Cyc_Tc_tc_decls(
te,ds,0,0,check_var_init,0);}struct Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;
int in_cinclude;struct Cyc_Dict_Dict*nsdict;};static int Cyc_Tc_vardecl_needed(
struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);static struct Cyc_List_List*
Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){return((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),
struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,
env,ds);}struct _tuple12{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};static int
Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){void*
_tmp1F1=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp1F2;struct Cyc_List_List*_tmp1F3;
struct Cyc_List_List**_tmp1F4;struct Cyc_List_List*_tmp1F5;struct Cyc_List_List**
_tmp1F6;struct Cyc_List_List*_tmp1F7;struct Cyc_List_List**_tmp1F8;struct Cyc_List_List*
_tmp1F9;struct Cyc_List_List**_tmp1FA;_LL133: if(_tmp1F1 <= (void*)2  || *((int*)
_tmp1F1)!= 0)goto _LL135;_tmp1F2=((struct Cyc_Absyn_Var_d_struct*)_tmp1F1)->f1;
_LL134: if(env->in_cinclude  || (void*)_tmp1F2->sc != (void*)3  && (void*)_tmp1F2->sc
!= (void*)4)return 1;{struct _tuple0 _tmp1FC;void*_tmp1FD;struct _tagged_arr*_tmp1FE;
struct _tuple0*_tmp1FB=_tmp1F2->name;_tmp1FC=*_tmp1FB;_tmp1FD=_tmp1FC.f1;_tmp1FE=
_tmp1FC.f2;{struct Cyc_List_List*ns;{void*_tmp1FF=_tmp1FD;struct Cyc_List_List*
_tmp200;struct Cyc_List_List*_tmp201;_LL140: if((int)_tmp1FF != 0)goto _LL142;_LL141:
ns=0;goto _LL13F;_LL142: if(_tmp1FF <= (void*)1  || *((int*)_tmp1FF)!= 0)goto _LL144;
_tmp200=((struct Cyc_Absyn_Rel_n_struct*)_tmp1FF)->f1;_LL143: ns=_tmp200;goto
_LL13F;_LL144: if(_tmp1FF <= (void*)1  || *((int*)_tmp1FF)!= 1)goto _LL13F;_tmp201=((
struct Cyc_Absyn_Abs_n_struct*)_tmp1FF)->f1;_LL145: ns=_tmp201;goto _LL13F;_LL13F:;}{
struct _tuple12*_tmp202=((struct _tuple12*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(env->nsdict,ns);struct Cyc_Tcenv_Genv*_tmp203=(*_tmp202).f1;
int _tmp204=(*((struct _tuple7*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp203->ordinaries,_tmp1FE)).f2;if(!_tmp204)(*_tmp202).f2=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_rinsert)(
env->rgn,(*_tmp202).f2,_tmp1FE);return _tmp204;}}}_LL135: if(_tmp1F1 <= (void*)2
 || *((int*)_tmp1F1)!= 10)goto _LL137;_tmp1F3=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp1F1)->f1;_tmp1F4=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_struct*)
_tmp1F1)->f1;_LL136: _tmp1F6=_tmp1F4;goto _LL138;_LL137: if(_tmp1F1 <= (void*)2  || *((
int*)_tmp1F1)!= 9)goto _LL139;_tmp1F5=((struct Cyc_Absyn_Using_d_struct*)_tmp1F1)->f2;
_tmp1F6=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_struct*)_tmp1F1)->f2;
_LL138: _tmp1F8=_tmp1F6;goto _LL13A;_LL139: if(_tmp1F1 <= (void*)2  || *((int*)
_tmp1F1)!= 8)goto _LL13B;_tmp1F7=((struct Cyc_Absyn_Namespace_d_struct*)_tmp1F1)->f2;
_tmp1F8=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_struct*)_tmp1F1)->f2;
_LL13A:*_tmp1F8=Cyc_Tc_treeshake_f(env,*_tmp1F8);return 1;_LL13B: if(_tmp1F1 <= (
void*)2  || *((int*)_tmp1F1)!= 11)goto _LL13D;_tmp1F9=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp1F1)->f1;_tmp1FA=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp1F1)->f1;_LL13C: {int in_cinclude=env->in_cinclude;env->in_cinclude=1;*
_tmp1FA=Cyc_Tc_treeshake_f(env,*_tmp1FA);env->in_cinclude=in_cinclude;return 1;}
_LL13D:;_LL13E: return 1;_LL132:;}static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*
set,struct _tagged_arr*x,void*y){return !((int(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_member)(set,x);}static struct _tuple12*Cyc_Tc_treeshake_make_env_f(
struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){return({struct _tuple12*_tmp205=
_region_malloc(rgn,sizeof(*_tmp205));_tmp205->f1=ge;_tmp205->f2=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_rempty)(
rgn,Cyc_strptrcmp);_tmp205;});}struct _tuple13{struct Cyc_List_List*f1;struct
_tuple12*f2;};struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*ds){struct _RegionHandle _tmp206=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp206;_push_region(rgn);{struct Cyc_Tc_TreeshakeEnv _tmp207=({
struct Cyc_Tc_TreeshakeEnv _tmp212;_tmp212.rgn=rgn;_tmp212.in_cinclude=0;_tmp212.nsdict=((
struct Cyc_Dict_Dict*(*)(struct _RegionHandle*,struct _tuple12*(*f)(struct
_RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict*
d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae);_tmp212;});
struct Cyc_List_List*_tmp208=Cyc_Tc_treeshake_f(& _tmp207,ds);if(((int(*)(struct
Cyc_Dict_Dict*d))Cyc_Dict_is_empty)(_tmp207.nsdict)){struct Cyc_List_List*_tmp209=
_tmp208;_npop_handler(0);return _tmp209;}{struct Cyc_Iter_Iter _tmp20A=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict*d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,
_tmp207.nsdict);struct _tuple13 _tmp20B=*((struct _tuple13*(*)(struct _RegionHandle*
r,struct Cyc_Dict_Dict*d))Cyc_Dict_rchoose)(rgn,_tmp207.nsdict);while(((int(*)(
struct Cyc_Iter_Iter,struct _tuple13*))Cyc_Iter_next)(_tmp20A,& _tmp20B)){struct
_tuple12*_tmp20D;struct _tuple12 _tmp20E;struct Cyc_Tcenv_Genv*_tmp20F;struct Cyc_Set_Set*
_tmp210;struct _tuple13 _tmp20C=_tmp20B;_tmp20D=_tmp20C.f2;_tmp20E=*_tmp20D;
_tmp20F=_tmp20E.f1;_tmp210=_tmp20E.f2;_tmp20F->ordinaries=((struct Cyc_Dict_Dict*(*)(
int(*f)(struct Cyc_Set_Set*,struct _tagged_arr*,struct _tuple7*),struct Cyc_Set_Set*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_filter_c)((int(*)(struct Cyc_Set_Set*set,
struct _tagged_arr*x,struct _tuple7*y))Cyc_Tc_treeshake_remove_f,_tmp210,_tmp20F->ordinaries);}{
struct Cyc_List_List*_tmp211=_tmp208;_npop_handler(0);return _tmp211;}}};
_pop_region(rgn);}
