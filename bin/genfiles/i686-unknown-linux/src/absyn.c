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
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),
struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[8];int Cyc_List_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{
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
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Carray_mod_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
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
struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Address_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*f1;void*f2;
};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
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
_tagged_arr*f1;};char Cyc_Absyn_EmptyAnnot[15]="\000\000\000\000EmptyAnnot\000";
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value;
extern void*Cyc_Absyn_rel_ns_null;int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual();struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_unknown_conref;
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_force_kb(void*kb);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);extern void*Cyc_Absyn_empty_effect;extern struct
_tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;
extern void*Cyc_Absyn_exn_typ;extern struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar;void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_file_typ();
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct
Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_star_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_tagged_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _tagged_arr*name);void*Cyc_Absyn_strctq(
struct _tuple0*name);void*Cyc_Absyn_unionq_typ(struct _tuple0*name);void*Cyc_Absyn_union_typ(
struct _tagged_arr*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*
Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(
struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _tagged_arr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _tagged_arr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple0*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple0*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(
struct _tuple0*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(
void*,struct Cyc_List_List*es,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct
Cyc_Absyn_Exp*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_cut_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_splice_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _tagged_arr*v,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct
Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt(struct _tagged_arr*lab,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Stmt*Cyc_Absyn_forarray_stmt(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct
Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct
_tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs);struct
Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_xtunion,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);void*Cyc_Absyn_pointer_expand(void*);int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct
Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct _tuple3{struct Cyc_Absyn_Tqual f1;
void*f2;};struct _tuple3*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
struct _tagged_arr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(void*
a);struct _tagged_arr*Cyc_Absyn_fieldname(int);struct _tuple4{void*f1;struct
_tuple0*f2;};struct _tuple4 Cyc_Absyn_aggr_kinded_name(void*);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(void*info);int Cyc_Absyn_is_union_type(void*);void
Cyc_Absyn_print_decls(struct Cyc_List_List*);struct Cyc_Typerep_Int_struct{int tag;
int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;
void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple5{
unsigned int f1;struct _tagged_arr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int
tag;struct _tagged_arr*f1;unsigned int f2;struct _tagged_arr f3;};struct _tuple6{
unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;
struct _tagged_arr f2;};struct _tuple7{unsigned int f1;struct _tagged_arr f2;};struct
Cyc_Typerep_TUnion_struct{int tag;struct _tagged_arr f1;struct _tagged_arr f2;struct
_tagged_arr f3;};struct Cyc_Typerep_TUnionField_struct{int tag;struct _tagged_arr f1;
struct _tagged_arr f2;unsigned int f3;struct _tagged_arr f4;};struct _tuple8{struct
_tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{int tag;struct
_tagged_arr f1;struct _tagged_arr f2;};struct Cyc_Typerep_Union_struct{int tag;struct
_tagged_arr*f1;int f2;struct _tagged_arr f3;};struct Cyc_Typerep_Enum_struct{int tag;
struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);extern void*Cyc_decls_rep;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
int Cyc_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*s2);typedef struct{int
__count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef
struct{int __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;
short*f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;};struct _tagged_arr
Cyc_aprintf(struct _tagged_arr,struct _tagged_arr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};int
Cyc_printf(struct _tagged_arr,struct _tagged_arr);extern char Cyc_FileCloseError[19];
extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct
_tagged_arr f1;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[
11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[
11];struct _tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple9*Cyc_Dict_rchoose(struct
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void*Cyc_Tcutil_compress(void*t);void Cyc_Marshal_print_type(void*
rep,void*val);struct _tuple10{struct Cyc_Dict_Dict*f1;int f2;};struct _tuple11{
struct _tagged_arr f1;int f2;};static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*
ss1,struct Cyc_List_List*ss2){return((int(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_strptrcmp,ss1,ss2);}int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct
Cyc_List_List*vs2){if((int)vs1 == (int)vs2)return 0;return Cyc_Absyn_strlist_cmp(
vs1,vs2);}int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){void*_tmp0=(*
q1).f1;void*_tmp1=(*q2).f1;{struct _tuple9 _tmp3=({struct _tuple9 _tmp2;_tmp2.f1=
_tmp0;_tmp2.f2=_tmp1;_tmp2;});void*_tmp4;void*_tmp5;void*_tmp6;struct Cyc_List_List*
_tmp7;void*_tmp8;struct Cyc_List_List*_tmp9;void*_tmpA;struct Cyc_List_List*_tmpB;
void*_tmpC;struct Cyc_List_List*_tmpD;void*_tmpE;void*_tmpF;void*_tmp10;void*
_tmp11;_LL1: _tmp4=_tmp3.f1;if((int)_tmp4 != 0)goto _LL3;_tmp5=_tmp3.f2;if((int)
_tmp5 != 0)goto _LL3;_LL2: goto _LL0;_LL3: _tmp6=_tmp3.f1;if(_tmp6 <= (void*)1?1:*((
int*)_tmp6)!= 0)goto _LL5;_tmp7=((struct Cyc_Absyn_Rel_n_struct*)_tmp6)->f1;_tmp8=
_tmp3.f2;if(_tmp8 <= (void*)1?1:*((int*)_tmp8)!= 0)goto _LL5;_tmp9=((struct Cyc_Absyn_Rel_n_struct*)
_tmp8)->f1;_LL4: _tmpB=_tmp7;_tmpD=_tmp9;goto _LL6;_LL5: _tmpA=_tmp3.f1;if(_tmpA <= (
void*)1?1:*((int*)_tmpA)!= 1)goto _LL7;_tmpB=((struct Cyc_Absyn_Abs_n_struct*)
_tmpA)->f1;_tmpC=_tmp3.f2;if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 1)goto _LL7;
_tmpD=((struct Cyc_Absyn_Abs_n_struct*)_tmpC)->f1;_LL6: {int i=Cyc_Absyn_strlist_cmp(
_tmpB,_tmpD);if(i != 0)return i;goto _LL0;}_LL7: _tmpE=_tmp3.f1;if((int)_tmpE != 0)
goto _LL9;_LL8: return - 1;_LL9: _tmpF=_tmp3.f2;if((int)_tmpF != 0)goto _LLB;_LLA:
return 1;_LLB: _tmp10=_tmp3.f1;if(_tmp10 <= (void*)1?1:*((int*)_tmp10)!= 0)goto _LLD;
_LLC: return - 1;_LLD: _tmp11=_tmp3.f2;if(_tmp11 <= (void*)1?1:*((int*)_tmp11)!= 0)
goto _LL0;_LLE: return 1;_LL0:;}return Cyc_strptrcmp((*q1).f2,(*q2).f2);}int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){int i=Cyc_strptrcmp(tv1->name,
tv2->name);if(i != 0)return i;if(tv1->identity == tv2->identity)return 0;if(tv1->identity
!= 0?tv2->identity != 0: 0)return*((int*)_check_null(tv1->identity))- *((int*)
_check_null(tv2->identity));else{if(tv1->identity == 0)return - 1;else{return 1;}}}
struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value={0,0};void*Cyc_Absyn_rel_ns_null=(
void*)& Cyc_Absyn_rel_ns_null_value;int Cyc_Absyn_is_qvar_qualified(struct _tuple0*
qv){void*_tmp13=(*qv).f1;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp15;
_LL10: if(_tmp13 <= (void*)1?1:*((int*)_tmp13)!= 0)goto _LL12;_tmp14=((struct Cyc_Absyn_Rel_n_struct*)
_tmp13)->f1;if(_tmp14 != 0)goto _LL12;_LL11: goto _LL13;_LL12: if(_tmp13 <= (void*)1?1:*((
int*)_tmp13)!= 1)goto _LL14;_tmp15=((struct Cyc_Absyn_Abs_n_struct*)_tmp13)->f1;
if(_tmp15 != 0)goto _LL14;_LL13: goto _LL15;_LL14: if((int)_tmp13 != 0)goto _LL16;_LL15:
return 0;_LL16:;_LL17: return 1;_LLF:;}static int Cyc_Absyn_new_type_counter=0;void*
Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){return(void*)({
struct Cyc_Absyn_Evar_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=({struct
Cyc_Absyn_Evar_struct _tmp17;_tmp17.tag=0;_tmp17.f1=k;_tmp17.f2=0;_tmp17.f3=Cyc_Absyn_new_type_counter
++;_tmp17.f4=env;_tmp17;});_tmp16;});}static struct Cyc_Core_Opt Cyc_Absyn_mk={(
void*)((void*)1)};void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Absyn_mk,tenv);}struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){return({struct Cyc_Absyn_Tqual
_tmp18;_tmp18.q_const=x.q_const?1: y.q_const;_tmp18.q_volatile=x.q_volatile?1: y.q_volatile;
_tmp18.q_restrict=x.q_restrict?1: y.q_restrict;_tmp18;});}struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(){return({struct Cyc_Absyn_Tqual _tmp19;_tmp19.q_const=0;
_tmp19.q_volatile=0;_tmp19.q_restrict=0;_tmp19;});}struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(){
return({struct Cyc_Absyn_Tqual _tmp1A;_tmp1A.q_const=1;_tmp1A.q_volatile=0;_tmp1A.q_restrict=
0;_tmp1A;});}static struct Cyc_Absyn_Int_c_struct Cyc_Absyn_one_intc={2,(void*)((
void*)1),1};static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={0,(void*)((
void*)& Cyc_Absyn_one_intc)};struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(
void*)((void*)& Cyc_Absyn_one_b_raw),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=& Cyc_Absyn_exp_unsigned_one_v;
static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={0,& Cyc_Absyn_exp_unsigned_one_v};
void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(
void*x){return({struct Cyc_Absyn_Conref*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->v=(
void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));
_tmp1F[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp20;_tmp20.tag=0;_tmp20.f1=(void*)
x;_tmp20;});_tmp1F;}));_tmp1E;});}struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref(){
return({struct Cyc_Absyn_Conref*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->v=(void*)((
void*)0);_tmp21;});}static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_true_conref_constraint={
0,(void*)1};static struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v={(void*)((void*)&
Cyc_Absyn_true_conref_constraint)};struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref=&
Cyc_Absyn_true_conref_v;static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_false_conref_constraint={
0,(void*)0};static struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v={(void*)((void*)&
Cyc_Absyn_false_conref_constraint)};struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref=&
Cyc_Absyn_false_conref_v;static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_bounds_one_conref_constraint={
0,(void*)((void*)& Cyc_Absyn_one_bt)};static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_one_conref_v={(
void*)((void*)& Cyc_Absyn_bounds_one_conref_constraint)};struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref=& Cyc_Absyn_bounds_one_conref_v;static struct Cyc_Absyn_Eq_constr_struct
Cyc_Absyn_bounds_unknown_conref_constraint={0,(void*)((void*)0)};static struct Cyc_Absyn_Conref
Cyc_Absyn_bounds_unknown_conref_v={(void*)((void*)& Cyc_Absyn_bounds_unknown_conref_constraint)};
struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_unknown_conref=& Cyc_Absyn_bounds_unknown_conref_v;
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x){void*
_tmp26=(void*)x->v;struct Cyc_Absyn_Conref*_tmp27;struct Cyc_Absyn_Conref**_tmp28;
_LL19: if((int)_tmp26 != 0)goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp26 <= (void*)1?1:*((
int*)_tmp26)!= 0)goto _LL1D;_LL1C: return x;_LL1D: if(_tmp26 <= (void*)1?1:*((int*)
_tmp26)!= 1)goto _LL18;_tmp27=((struct Cyc_Absyn_Forward_constr_struct*)_tmp26)->f1;
_tmp28=(struct Cyc_Absyn_Conref**)&((struct Cyc_Absyn_Forward_constr_struct*)
_tmp26)->f1;_LL1E: {struct Cyc_Absyn_Conref*_tmp29=Cyc_Absyn_compress_conref(*
_tmp28);*_tmp28=_tmp29;return _tmp29;}_LL18:;}void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*
x){void*_tmp2A=(void*)(Cyc_Absyn_compress_conref(x))->v;void*_tmp2B;_LL20: if(
_tmp2A <= (void*)1?1:*((int*)_tmp2A)!= 0)goto _LL22;_tmp2B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2A)->f1;_LL21: return _tmp2B;_LL22:;_LL23:({void*_tmp2C[0]={};Cyc_Tcutil_impos(({
const char*_tmp2D="conref_val";_tag_arr(_tmp2D,sizeof(char),_get_zero_arr_size(
_tmp2D,11));}),_tag_arr(_tmp2C,sizeof(void*),0));});_LL1F:;}void*Cyc_Absyn_conref_def(
void*y,struct Cyc_Absyn_Conref*x){void*_tmp2E=(void*)(Cyc_Absyn_compress_conref(x))->v;
void*_tmp2F;_LL25: if(_tmp2E <= (void*)1?1:*((int*)_tmp2E)!= 0)goto _LL27;_tmp2F=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp2E)->f1;_LL26: return _tmp2F;_LL27:;
_LL28: return y;_LL24:;}void*Cyc_Absyn_compress_kb(void*k){void*_tmp30=k;struct Cyc_Core_Opt*
_tmp31;struct Cyc_Core_Opt*_tmp32;struct Cyc_Core_Opt*_tmp33;struct Cyc_Core_Opt
_tmp34;void*_tmp35;void**_tmp36;struct Cyc_Core_Opt*_tmp37;struct Cyc_Core_Opt
_tmp38;void*_tmp39;void**_tmp3A;_LL2A: if(*((int*)_tmp30)!= 0)goto _LL2C;_LL2B:
goto _LL2D;_LL2C: if(*((int*)_tmp30)!= 1)goto _LL2E;_tmp31=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp30)->f1;if(_tmp31 != 0)goto _LL2E;_LL2D: goto _LL2F;_LL2E: if(*((int*)_tmp30)!= 2)
goto _LL30;_tmp32=((struct Cyc_Absyn_Less_kb_struct*)_tmp30)->f1;if(_tmp32 != 0)
goto _LL30;_LL2F: return k;_LL30: if(*((int*)_tmp30)!= 1)goto _LL32;_tmp33=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmp30)->f1;if(_tmp33 == 0)goto _LL32;_tmp34=*_tmp33;
_tmp35=(void*)_tmp34.v;_tmp36=(void**)&(*((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp30)->f1).v;_LL31: _tmp3A=_tmp36;goto _LL33;_LL32: if(*((int*)_tmp30)!= 2)goto
_LL29;_tmp37=((struct Cyc_Absyn_Less_kb_struct*)_tmp30)->f1;if(_tmp37 == 0)goto
_LL29;_tmp38=*_tmp37;_tmp39=(void*)_tmp38.v;_tmp3A=(void**)&(*((struct Cyc_Absyn_Less_kb_struct*)
_tmp30)->f1).v;_LL33:*_tmp3A=Cyc_Absyn_compress_kb(*_tmp3A);return*_tmp3A;_LL29:;}
void*Cyc_Absyn_force_kb(void*kb){void*_tmp3B=Cyc_Absyn_compress_kb(kb);void*
_tmp3C;struct Cyc_Core_Opt*_tmp3D;struct Cyc_Core_Opt**_tmp3E;struct Cyc_Core_Opt*
_tmp3F;struct Cyc_Core_Opt**_tmp40;void*_tmp41;_LL35: if(*((int*)_tmp3B)!= 0)goto
_LL37;_tmp3C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp3B)->f1;_LL36: return
_tmp3C;_LL37: if(*((int*)_tmp3B)!= 1)goto _LL39;_tmp3D=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp3B)->f1;_tmp3E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp3B)->f1;_LL38: _tmp40=_tmp3E;_tmp41=(void*)2;goto _LL3A;_LL39: if(*((int*)
_tmp3B)!= 2)goto _LL34;_tmp3F=((struct Cyc_Absyn_Less_kb_struct*)_tmp3B)->f1;
_tmp40=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp3B)->f1;
_tmp41=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp3B)->f2;_LL3A:*_tmp40=({
struct Cyc_Core_Opt*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->v=(void*)((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp44;_tmp44.tag=0;_tmp44.f1=(void*)_tmp41;_tmp44;});
_tmp43;}));_tmp42;});return _tmp41;_LL34:;}static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_char_tt={5,(void*)((void*)2),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_uchar_tt={5,(void*)((void*)1),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ushort_tt={5,(void*)((void*)1),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_uint_tt={5,(void*)((void*)1),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulong_tt={5,(void*)((void*)1),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulonglong_tt={5,(void*)((void*)1),(void*)((void*)3)};void*Cyc_Absyn_char_typ=(
void*)& Cyc_Absyn_char_tt;void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;
void*Cyc_Absyn_ushort_typ=(void*)& Cyc_Absyn_ushort_tt;void*Cyc_Absyn_uint_typ=(
void*)& Cyc_Absyn_uint_tt;void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;
void*Cyc_Absyn_ulonglong_typ=(void*)& Cyc_Absyn_ulonglong_tt;static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_schar_tt={5,(void*)((void*)0),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sshort_tt={5,(void*)((void*)0),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sint_tt={5,(void*)((void*)0),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_slong_tt={5,(void*)((void*)0),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_slonglong_tt={5,(void*)((void*)0),(void*)((void*)3)};void*Cyc_Absyn_schar_typ=(
void*)& Cyc_Absyn_schar_tt;void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;
void*Cyc_Absyn_sint_typ=(void*)& Cyc_Absyn_sint_tt;void*Cyc_Absyn_slong_typ=(void*)&
Cyc_Absyn_slong_tt;void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;
void*Cyc_Absyn_float_typ=(void*)1;void*Cyc_Absyn_double_typ(int b){return(void*)({
struct Cyc_Absyn_DoubleType_struct*_tmp50=_cycalloc_atomic(sizeof(*_tmp50));
_tmp50[0]=({struct Cyc_Absyn_DoubleType_struct _tmp51;_tmp51.tag=6;_tmp51.f1=b;
_tmp51;});_tmp50;});}static struct Cyc_Absyn_Abs_n_struct Cyc_Absyn_abs_null={1,0};
static char _tmp53[4]="exn";static struct _tagged_arr Cyc_Absyn_exn_str={_tmp53,
_tmp53,_tmp53 + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={(void*)& Cyc_Absyn_abs_null,&
Cyc_Absyn_exn_str};struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static
char _tmp54[15]="Null_Exception";static struct _tagged_arr Cyc_Absyn_Null_Exception_str={
_tmp54,_tmp54,_tmp54 + 15};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={(void*)&
Cyc_Absyn_abs_null,& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=&
Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Null_Exception_tuf_v={&
Cyc_Absyn_Null_Exception_pr,0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static char _tmp55[13]="Array_bounds";
static struct _tagged_arr Cyc_Absyn_Array_bounds_str={_tmp55,_tmp55,_tmp55 + 13};
static struct _tuple0 Cyc_Absyn_Array_bounds_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Array_bounds_str};
struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static
struct Cyc_Absyn_Tunionfield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Array_bounds_tuf=&
Cyc_Absyn_Array_bounds_tuf_v;static char _tmp56[16]="Match_Exception";static struct
_tagged_arr Cyc_Absyn_Match_Exception_str={_tmp56,_tmp56,_tmp56 + 16};static struct
_tuple0 Cyc_Absyn_Match_Exception_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Match_Exception_str};
struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;
static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Match_Exception_tuf=&
Cyc_Absyn_Match_Exception_tuf_v;static char _tmp57[10]="Bad_alloc";static struct
_tagged_arr Cyc_Absyn_Bad_alloc_str={_tmp57,_tmp57,_tmp57 + 10};static struct
_tuple0 Cyc_Absyn_Bad_alloc_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Bad_alloc_str};
struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Tunionfield
Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0,(void*)((void*)3)};struct
Cyc_Absyn_Tunionfield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;static
struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,0};
static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l0};static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2};static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};static struct Cyc_Absyn_Tuniondecl
Cyc_Absyn_exn_tud_v={(void*)((void*)3),& Cyc_Absyn_exn_name_v,0,(struct Cyc_Core_Opt*)&
Cyc_Absyn_exn_ol,1};struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
static struct Cyc_Absyn_KnownTunion_struct Cyc_Absyn_exn_tinfou={1,& Cyc_Absyn_exn_tud};
static struct Cyc_Absyn_TunionType_struct Cyc_Absyn_exn_typ_tt={2,{(void*)((void*)&
Cyc_Absyn_exn_tinfou),0,(void*)((void*)2)}};void*Cyc_Absyn_exn_typ=(void*)& Cyc_Absyn_exn_typ_tt;
static char _tmp5A[9]="PrintArg";static struct _tagged_arr Cyc_Absyn_printarg_str={
_tmp5A,_tmp5A,_tmp5A + 9};static char _tmp5B[9]="ScanfArg";static struct _tagged_arr
Cyc_Absyn_scanfarg_str={_tmp5B,_tmp5B,_tmp5B + 9};static struct Cyc_Absyn_Abs_n_struct
Cyc_Absyn_std_namespace={1,0};static struct _tuple0 Cyc_Absyn_tunion_print_arg_qvar_p={(
void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_printarg_str};static struct _tuple0 Cyc_Absyn_tunion_scanf_arg_qvar_p={(
void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar=&
Cyc_Absyn_tunion_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar=&
Cyc_Absyn_tunion_scanf_arg_qvar_p;static void**Cyc_Absyn_string_t_opt=0;void*Cyc_Absyn_string_typ(
void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(),(
void*)0,Cyc_Absyn_true_conref);if(Cyc_Absyn_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_empty_tqual(),(void*)0,Cyc_Absyn_true_conref);
Cyc_Absyn_string_t_opt=({void**_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=t;
_tmp5D;});}return*((void**)_check_null(Cyc_Absyn_string_t_opt));}static void**Cyc_Absyn_const_string_t_opt=
0;void*Cyc_Absyn_const_string_typ(void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(),(void*)0,Cyc_Absyn_true_conref);
if(Cyc_Absyn_const_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Absyn_const_tqual(),(void*)0,Cyc_Absyn_true_conref);Cyc_Absyn_const_string_t_opt=({
void**_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E[0]=t;_tmp5E;});}return*((void**)
_check_null(Cyc_Absyn_const_string_t_opt));}void*Cyc_Absyn_starb_typ(void*t,void*
r,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zeroterm){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F[0]=({
struct Cyc_Absyn_PointerType_struct _tmp60;_tmp60.tag=4;_tmp60.f1=({struct Cyc_Absyn_PtrInfo
_tmp61;_tmp61.elt_typ=(void*)t;_tmp61.elt_tq=tq;_tmp61.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp62;_tmp62.rgn=(void*)r;_tmp62.nullable=Cyc_Absyn_true_conref;_tmp62.bounds=
Cyc_Absyn_new_conref(b);_tmp62.zero_term=zeroterm;_tmp62;});_tmp61;});_tmp60;});
_tmp5F;});}void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,
struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=({struct Cyc_Absyn_PointerType_struct
_tmp64;_tmp64.tag=4;_tmp64.f1=({struct Cyc_Absyn_PtrInfo _tmp65;_tmp65.elt_typ=(
void*)t;_tmp65.elt_tq=tq;_tmp65.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp66;_tmp66.rgn=(
void*)r;_tmp66.nullable=Cyc_Absyn_false_conref;_tmp66.bounds=Cyc_Absyn_new_conref(
b);_tmp66.zero_term=zeroterm;_tmp66;});_tmp65;});_tmp64;});_tmp63;});}void*Cyc_Absyn_star_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp67=_cycalloc(sizeof(*_tmp67));
_tmp67[0]=({struct Cyc_Absyn_PointerType_struct _tmp68;_tmp68.tag=4;_tmp68.f1=({
struct Cyc_Absyn_PtrInfo _tmp69;_tmp69.elt_typ=(void*)t;_tmp69.elt_tq=tq;_tmp69.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp6A;_tmp6A.rgn=(void*)r;_tmp6A.nullable=Cyc_Absyn_true_conref;
_tmp6A.bounds=Cyc_Absyn_bounds_one_conref;_tmp6A.zero_term=zeroterm;_tmp6A;});
_tmp69;});_tmp68;});_tmp67;});}void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual
tq){return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp6B=_cycalloc(sizeof(*
_tmp6B));_tmp6B[0]=({struct Cyc_Absyn_PointerType_struct _tmp6C;_tmp6C.tag=4;
_tmp6C.f1=({struct Cyc_Absyn_PtrInfo _tmp6D;_tmp6D.elt_typ=(void*)t;_tmp6D.elt_tq=
tq;_tmp6D.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp6E;_tmp6E.rgn=(void*)((void*)2);
_tmp6E.nullable=Cyc_Absyn_true_conref;_tmp6E.bounds=Cyc_Absyn_bounds_one_conref;
_tmp6E.zero_term=Cyc_Absyn_false_conref;_tmp6E;});_tmp6D;});_tmp6C;});_tmp6B;});}
void*Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp6F=_cycalloc(
sizeof(*_tmp6F));_tmp6F[0]=({struct Cyc_Absyn_PointerType_struct _tmp70;_tmp70.tag=
4;_tmp70.f1=({struct Cyc_Absyn_PtrInfo _tmp71;_tmp71.elt_typ=(void*)t;_tmp71.elt_tq=
tq;_tmp71.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp72;_tmp72.rgn=(void*)r;_tmp72.nullable=
Cyc_Absyn_false_conref;_tmp72.bounds=Cyc_Absyn_bounds_one_conref;_tmp72.zero_term=
zeroterm;_tmp72;});_tmp71;});_tmp70;});_tmp6F;});}void*Cyc_Absyn_tagged_typ(void*
t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73[0]=({
struct Cyc_Absyn_PointerType_struct _tmp74;_tmp74.tag=4;_tmp74.f1=({struct Cyc_Absyn_PtrInfo
_tmp75;_tmp75.elt_typ=(void*)t;_tmp75.elt_tq=tq;_tmp75.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp76;_tmp76.rgn=(void*)r;_tmp76.nullable=Cyc_Absyn_true_conref;_tmp76.bounds=
Cyc_Absyn_bounds_unknown_conref;_tmp76.zero_term=zeroterm;_tmp76;});_tmp75;});
_tmp74;});_tmp73;});}void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term){return(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77[0]=({
struct Cyc_Absyn_ArrayType_struct _tmp78;_tmp78.tag=7;_tmp78.f1=({struct Cyc_Absyn_ArrayInfo
_tmp79;_tmp79.elt_type=(void*)elt_type;_tmp79.tq=tq;_tmp79.num_elts=num_elts;
_tmp79.zero_term=zero_term;_tmp79;});_tmp78;});_tmp77;});}static char _tmp83[8]="__sFILE";
void*Cyc_Absyn_file_typ(){static void**file_t_opt=0;static struct _tagged_arr sf_str={
_tmp83,_tmp83,_tmp83 + 8};static struct _tagged_arr*sf=& sf_str;if(file_t_opt == 0){
struct _tuple0*file_t_name=({struct _tuple0*_tmp82=_cycalloc(sizeof(*_tmp82));
_tmp82->f1=(void*)& Cyc_Absyn_std_namespace;_tmp82->f2=sf;_tmp82;});struct Cyc_Absyn_Aggrdecl*
sd=({struct Cyc_Absyn_Aggrdecl*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->kind=(
void*)((void*)0);_tmp81->sc=(void*)((void*)1);_tmp81->name=file_t_name;_tmp81->tvs=
0;_tmp81->impl=0;_tmp81->attributes=0;_tmp81;});void*file_struct_typ=(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B[0]=({
struct Cyc_Absyn_AggrType_struct _tmp7C;_tmp7C.tag=10;_tmp7C.f1=({struct Cyc_Absyn_AggrInfo
_tmp7D;_tmp7D.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp7E=
_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp7F;
_tmp7F.tag=1;_tmp7F.f1=({struct Cyc_Absyn_Aggrdecl**_tmp80=_cycalloc(sizeof(*
_tmp80));_tmp80[0]=sd;_tmp80;});_tmp7F;});_tmp7E;}));_tmp7D.targs=0;_tmp7D;});
_tmp7C;});_tmp7B;});file_t_opt=({void**_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[
0]=Cyc_Absyn_at_typ(file_struct_typ,(void*)2,Cyc_Absyn_empty_tqual(),Cyc_Absyn_false_conref);
_tmp7A;});}return*((void**)_check_null(file_t_opt));}void*Cyc_Absyn_void_star_typ(){
static void**void_star_t_opt=0;if(void_star_t_opt == 0)void_star_t_opt=({void**
_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=Cyc_Absyn_star_typ((void*)0,(void*)2,
Cyc_Absyn_empty_tqual(),Cyc_Absyn_false_conref);_tmp84;});return*((void**)
_check_null(void_star_t_opt));}static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={
20,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;void*Cyc_Absyn_aggr_typ(
void*k,struct _tagged_arr*name){return(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=({struct Cyc_Absyn_AggrType_struct
_tmp87;_tmp87.tag=10;_tmp87.f1=({struct Cyc_Absyn_AggrInfo _tmp88;_tmp88.aggr_info=(
void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmp89=_cycalloc(sizeof(*
_tmp89));_tmp89[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmp8A;_tmp8A.tag=0;
_tmp8A.f1=(void*)k;_tmp8A.f2=({struct _tuple0*_tmp8B=_cycalloc(sizeof(*_tmp8B));
_tmp8B->f1=Cyc_Absyn_rel_ns_null;_tmp8B->f2=name;_tmp8B;});_tmp8A;});_tmp89;}));
_tmp88.targs=0;_tmp88;});_tmp87;});_tmp86;});}void*Cyc_Absyn_strct(struct
_tagged_arr*name){return Cyc_Absyn_aggr_typ((void*)0,name);}void*Cyc_Absyn_union_typ(
struct _tagged_arr*name){return Cyc_Absyn_aggr_typ((void*)1,name);}void*Cyc_Absyn_strctq(
struct _tuple0*name){return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp8C=
_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Absyn_AggrType_struct _tmp8D;
_tmp8D.tag=10;_tmp8D.f1=({struct Cyc_Absyn_AggrInfo _tmp8E;_tmp8E.aggr_info=(void*)((
void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));
_tmp8F[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmp90;_tmp90.tag=0;_tmp90.f1=(
void*)((void*)0);_tmp90.f2=name;_tmp90;});_tmp8F;}));_tmp8E.targs=0;_tmp8E;});
_tmp8D;});_tmp8C;});}void*Cyc_Absyn_unionq_typ(struct _tuple0*name){return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({
struct Cyc_Absyn_AggrType_struct _tmp92;_tmp92.tag=10;_tmp92.f1=({struct Cyc_Absyn_AggrInfo
_tmp93;_tmp93.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp95;_tmp95.tag=0;_tmp95.f1=(void*)((void*)1);_tmp95.f2=name;_tmp95;});_tmp94;}));
_tmp93.targs=0;_tmp93;});_tmp92;});_tmp91;});}struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Exp*_tmp96=
_cycalloc(sizeof(*_tmp96));_tmp96->topt=0;_tmp96->r=(void*)r;_tmp96->loc=loc;
_tmp96->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot);_tmp96;});}struct Cyc_Absyn_Exp*
Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_New_e_struct*_tmp97=
_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Absyn_New_e_struct _tmp98;_tmp98.tag=
15;_tmp98.f1=rgn_handle;_tmp98.f2=e;_tmp98;});_tmp97;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){return({struct Cyc_Absyn_Exp*_tmp99=
_cycalloc(sizeof(*_tmp99));_tmp99[0]=*e;_tmp99;});}struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
void*c,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Const_e_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_Absyn_Const_e_struct
_tmp9B;_tmp9B.tag=0;_tmp9B.f1=(void*)c;_tmp9B;});_tmp9A;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Const_e_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({
struct Cyc_Absyn_Const_e_struct _tmp9D;_tmp9D.tag=0;_tmp9D.f1=(void*)((void*)0);
_tmp9D;});_tmp9C;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*s,int i,
struct Cyc_Position_Segment*seg){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*
_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({struct Cyc_Absyn_Int_c_struct _tmp9F;
_tmp9F.tag=2;_tmp9F.f1=(void*)s;_tmp9F.f2=i;_tmp9F;});_tmp9E;}),seg);}struct Cyc_Absyn_Exp*
Cyc_Absyn_signed_int_exp(int i,struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp((
void*)0,i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_int_exp((void*)1,(int)i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(
int b,struct Cyc_Position_Segment*loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(
1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc){
return Cyc_Absyn_bool_exp(0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Char_c_struct*
_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({struct Cyc_Absyn_Char_c_struct
_tmpA1;_tmpA1.tag=0;_tmpA1.f1=(void*)((void*)2);_tmpA1.f2=c;_tmpA1;});_tmpA0;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _tagged_arr f,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Float_c_struct*_tmpA2=
_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_Float_c_struct _tmpA3;
_tmpA3.tag=4;_tmpA3.f1=f;_tmpA3;});_tmpA2;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_String_c_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[
0]=({struct Cyc_Absyn_String_c_struct _tmpA5;_tmpA5.tag=5;_tmpA5.f1=s;_tmpA5;});
_tmpA4;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmpA6=
_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_Var_e_struct _tmpA7;_tmpA7.tag=
1;_tmpA7.f1=q;_tmpA7.f2=(void*)((void*)0);_tmpA7;});_tmpA6;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmpA8=_cycalloc(sizeof(*
_tmpA8));_tmpA8[0]=({struct Cyc_Absyn_Var_e_struct _tmpA9;_tmpA9.tag=1;_tmpA9.f1=q;
_tmpA9.f2=(void*)b;_tmpA9;});_tmpA8;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(
struct _tuple0*q,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmpAB;_tmpAB.tag=2;_tmpAB.f1=q;_tmpAB;});
_tmpAA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*p,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({struct Cyc_Absyn_Primop_e_struct
_tmpAD;_tmpAD.tag=3;_tmpAD.f1=(void*)p;_tmpAD.f2=es;_tmpAD;});_tmpAC;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpAE=_cycalloc(sizeof(*
_tmpAE));_tmpAE->hd=e;_tmpAE->tl=0;_tmpAE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpAF=_cycalloc(sizeof(*
_tmpAF));_tmpAF->hd=e1;_tmpAF->tl=({struct Cyc_List_List*_tmpB0=_cycalloc(sizeof(*
_tmpB0));_tmpB0->hd=e2;_tmpB0->tl=0;_tmpB0;});_tmpAF;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_prim2_exp((void*)0,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)1,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)3,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)5,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)6,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)7,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)8,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)9,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)10,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AssignOp_e_struct*_tmpB1=
_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_AssignOp_e_struct _tmpB2;
_tmpB2.tag=4;_tmpB2.f1=e1;_tmpB2.f2=popt;_tmpB2.f3=e2;_tmpB2;});_tmpB1;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,void*i,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_Increment_e_struct
_tmpB4;_tmpB4.tag=5;_tmpB4.f1=e;_tmpB4.f2=(void*)i;_tmpB4;});_tmpB3;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,(void*)0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,(void*)2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)3,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Conditional_e_struct*
_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_Conditional_e_struct
_tmpB6;_tmpB6.tag=6;_tmpB6.f1=e1;_tmpB6.f2=e2;_tmpB6.f3=e3;_tmpB6;});_tmpB5;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_conditional_exp(e1,e2,Cyc_Absyn_false_exp(
loc),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_conditional_exp(e1,Cyc_Absyn_true_exp(
loc),e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_SeqExp_e_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[
0]=({struct Cyc_Absyn_SeqExp_e_struct _tmpB8;_tmpB8.tag=7;_tmpB8.f1=e1;_tmpB8.f2=
e2;_tmpB8;});_tmpB7;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownCall_e_struct*_tmpB9=
_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({struct Cyc_Absyn_UnknownCall_e_struct
_tmpBA;_tmpBA.tag=8;_tmpBA.f1=e;_tmpBA.f2=es;_tmpBA;});_tmpB9;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmpBB=
_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_FnCall_e_struct _tmpBC;
_tmpBC.tag=9;_tmpBC.f1=e;_tmpBC.f2=es;_tmpBC.f3=0;_tmpBC;});_tmpBB;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_NoInstantiate_e_struct
_tmpBE;_tmpBE.tag=11;_tmpBE.f1=e;_tmpBE;});_tmpBD;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmpC0;_tmpC0.tag=12;_tmpC0.f1=e;_tmpC0.f2=ts;_tmpC0;});_tmpBF;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Cast_e_struct*_tmpC1=
_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=({struct Cyc_Absyn_Cast_e_struct _tmpC2;
_tmpC2.tag=13;_tmpC2.f1=(void*)t;_tmpC2.f2=e;_tmpC2;});_tmpC1;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_struct*_tmpC3=_cycalloc(
sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Absyn_Throw_e_struct _tmpC4;_tmpC4.tag=10;
_tmpC4.f1=e;_tmpC4;});_tmpC3;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Address_e_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));
_tmpC5[0]=({struct Cyc_Absyn_Address_e_struct _tmpC6;_tmpC6.tag=14;_tmpC6.f1=e;
_tmpC6;});_tmpC5;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpC8;_tmpC8.tag=16;_tmpC8.f1=(void*)t;_tmpC8;});_tmpC7;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeofexp_e_struct*_tmpC9=
_cycalloc(sizeof(*_tmpC9));_tmpC9[0]=({struct Cyc_Absyn_Sizeofexp_e_struct _tmpCA;
_tmpCA.tag=17;_tmpCA.f1=e;_tmpCA;});_tmpC9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(
void*t,void*of,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmpCC;_tmpCC.tag=18;_tmpCC.f1=(void*)t;_tmpCC.f2=(
void*)of;_tmpCC;});_tmpCB;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*tvs,void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Gentyp_e_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[
0]=({struct Cyc_Absyn_Gentyp_e_struct _tmpCE;_tmpCE.tag=19;_tmpCE.f1=tvs;_tmpCE.f2=(
void*)t;_tmpCE;});_tmpCD;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Deref_e_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({
struct Cyc_Absyn_Deref_e_struct _tmpD0;_tmpD0.tag=20;_tmpD0.f1=e;_tmpD0;});_tmpCF;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_tagged_arr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_AggrMember_e_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1[0]=({
struct Cyc_Absyn_AggrMember_e_struct _tmpD2;_tmpD2.tag=21;_tmpD2.f1=e;_tmpD2.f2=n;
_tmpD2;});_tmpD1;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*
e,struct _tagged_arr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmpD3=_cycalloc(sizeof(*_tmpD3));
_tmpD3[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmpD4;_tmpD4.tag=22;_tmpD4.f1=e;
_tmpD4.f2=n;_tmpD4;});_tmpD3;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpD5=
_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpD6;
_tmpD6.tag=23;_tmpD6.f1=e1;_tmpD6.f2=e2;_tmpD6;});_tmpD5;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tuple_e_struct*_tmpD7=_cycalloc(
sizeof(*_tmpD7));_tmpD7[0]=({struct Cyc_Absyn_Tuple_e_struct _tmpD8;_tmpD8.tag=24;
_tmpD8.f1=es;_tmpD8;});_tmpD7;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));
_tmpD9[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpDA;_tmpDA.tag=35;_tmpDA.f1=s;
_tmpDA;});_tmpD9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}struct _tuple12{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){dles=({struct Cyc_List_List*_tmpDB=_cycalloc(sizeof(*
_tmpDB));_tmpDB->hd=({struct _tuple12*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->f1=
0;_tmpDC->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpDC;});_tmpDB->tl=dles;_tmpDB;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_struct*_tmpDD=_cycalloc(
sizeof(*_tmpDD));_tmpDD[0]=({struct Cyc_Absyn_Array_e_struct _tmpDE;_tmpDE.tag=26;
_tmpDE.f1=dles;_tmpDE;});_tmpDD;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpDF=
_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpE0;_tmpE0.tag=34;_tmpE0.f1=n;_tmpE0.f2=dles;_tmpE0;});_tmpDF;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){return({
struct Cyc_Absyn_Stmt*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->r=(void*)s;_tmpE1->loc=
loc;_tmpE1->non_local_preds=0;_tmpE1->try_depth=0;_tmpE1->annot=(void*)((void*)
Cyc_Absyn_EmptyAnnot);_tmpE1;});}struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)0,loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Exp_s_struct*_tmpE2=_cycalloc(
sizeof(*_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_Exp_s_struct _tmpE3;_tmpE3.tag=0;
_tmpE3.f1=e;_tmpE3;});_tmpE2;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(
struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc){if(ss == 0)return Cyc_Absyn_skip_stmt(
loc);else{if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{return Cyc_Absyn_seq_stmt((
struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Return_s_struct*_tmpE4=
_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_Return_s_struct _tmpE5;
_tmpE5.tag=2;_tmpE5.f1=e;_tmpE5;});_tmpE4;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_IfThenElse_s_struct*
_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6[0]=({struct Cyc_Absyn_IfThenElse_s_struct
_tmpE7;_tmpE7.tag=3;_tmpE7.f1=e;_tmpE7.f2=s1;_tmpE7.f3=s2;_tmpE7;});_tmpE6;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_While_s_struct*
_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=({struct Cyc_Absyn_While_s_struct
_tmpE9;_tmpE9.tag=4;_tmpE9.f1=({struct _tuple2 _tmpEA;_tmpEA.f1=e;_tmpEA.f2=Cyc_Absyn_skip_stmt(
e->loc);_tmpEA;});_tmpE9.f2=s;_tmpE9;});_tmpE8;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Break_s_struct*
_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB[0]=({struct Cyc_Absyn_Break_s_struct
_tmpEC;_tmpEC.tag=5;_tmpEC.f1=0;_tmpEC;});_tmpEB;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Continue_s_struct*_tmpED=_cycalloc(sizeof(*_tmpED));
_tmpED[0]=({struct Cyc_Absyn_Continue_s_struct _tmpEE;_tmpEE.tag=6;_tmpEE.f1=0;
_tmpEE;});_tmpED;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_For_s_struct*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Absyn_For_s_struct _tmpF0;
_tmpF0.tag=8;_tmpF0.f1=e1;_tmpF0.f2=({struct _tuple2 _tmpF1;_tmpF1.f1=e2;_tmpF1.f2=
Cyc_Absyn_skip_stmt(e3->loc);_tmpF1;});_tmpF0.f3=({struct _tuple2 _tmpF2;_tmpF2.f1=
e3;_tmpF2.f2=Cyc_Absyn_skip_stmt(e3->loc);_tmpF2;});_tmpF0.f4=s;_tmpF0;});_tmpEF;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_struct*
_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({struct Cyc_Absyn_Switch_s_struct
_tmpF4;_tmpF4.tag=9;_tmpF4.f1=e;_tmpF4.f2=scs;_tmpF4;});_tmpF3;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc){struct _tuple9 _tmpF6=({struct _tuple9 _tmpF5;
_tmpF5.f1=(void*)s1->r;_tmpF5.f2=(void*)s2->r;_tmpF5;});void*_tmpF7;void*_tmpF8;
_LL3C: _tmpF7=_tmpF6.f1;if((int)_tmpF7 != 0)goto _LL3E;_LL3D: return s2;_LL3E: _tmpF8=
_tmpF6.f2;if((int)_tmpF8 != 0)goto _LL40;_LL3F: return s1;_LL40:;_LL41: return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Seq_s_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({
struct Cyc_Absyn_Seq_s_struct _tmpFA;_tmpFA.tag=1;_tmpFA.f1=s1;_tmpFA.f2=s2;_tmpFA;});
_tmpF9;}),loc);_LL3B:;}struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*
el,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Fallthru_s_struct*
_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB[0]=({struct Cyc_Absyn_Fallthru_s_struct
_tmpFC;_tmpFC.tag=11;_tmpFC.f1=el;_tmpFC.f2=0;_tmpFC;});_tmpFB;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpFE;_tmpFE.tag=12;_tmpFE.f1=d;_tmpFE.f2=s;_tmpFE;});_tmpFD;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101[
0]=({struct Cyc_Absyn_Var_d_struct _tmp102;_tmp102.tag=0;_tmp102.f1=Cyc_Absyn_new_vardecl(
x,t,init);_tmp102;});_tmp101;}),loc);return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp100;_tmp100.tag=12;_tmp100.f1=d;_tmp100.f2=s;_tmp100;});_tmpFF;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_cut_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Cut_s_struct*_tmp103=
_cycalloc(sizeof(*_tmp103));_tmp103[0]=({struct Cyc_Absyn_Cut_s_struct _tmp104;
_tmp104.tag=13;_tmp104.f1=s;_tmp104;});_tmp103;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmp105=_cycalloc(sizeof(*_tmp105));
_tmp105[0]=({struct Cyc_Absyn_Splice_s_struct _tmp106;_tmp106.tag=14;_tmp106.f1=s;
_tmp106;});_tmp105;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct
_tagged_arr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp107=_cycalloc(sizeof(*_tmp107));
_tmp107[0]=({struct Cyc_Absyn_Label_s_struct _tmp108;_tmp108.tag=15;_tmp108.f1=v;
_tmp108.f2=s;_tmp108;});_tmp107;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Do_s_struct*_tmp109=_cycalloc(
sizeof(*_tmp109));_tmp109[0]=({struct Cyc_Absyn_Do_s_struct _tmp10A;_tmp10A.tag=16;
_tmp10A.f1=s;_tmp10A.f2=({struct _tuple2 _tmp10B;_tmp10B.f1=e;_tmp10B.f2=Cyc_Absyn_skip_stmt(
e->loc);_tmp10B;});_tmp10A;});_tmp109;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_TryCatch_s_struct*_tmp10C=
_cycalloc(sizeof(*_tmp10C));_tmp10C[0]=({struct Cyc_Absyn_TryCatch_s_struct
_tmp10D;_tmp10D.tag=17;_tmp10D.f1=s;_tmp10D.f2=scs;_tmp10D;});_tmp10C;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _tagged_arr*lab,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Goto_s_struct*_tmp10E=
_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=({struct Cyc_Absyn_Goto_s_struct _tmp10F;
_tmp10F.tag=7;_tmp10F.f1=lab;_tmp10F.f2=0;_tmp10F;});_tmp10E;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_forarray_stmt(struct Cyc_List_List*d,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_ForArray_s_struct*_tmp110=_cycalloc(sizeof(*_tmp110));
_tmp110[0]=({struct Cyc_Absyn_ForArray_s_struct _tmp111;_tmp111.tag=19;_tmp111.f1=({
struct Cyc_Absyn_ForArrayInfo _tmp112;_tmp112.defns=d;_tmp112.condition=({struct
_tuple2 _tmp114;_tmp114.f1=e1;_tmp114.f2=Cyc_Absyn_skip_stmt(e1->loc);_tmp114;});
_tmp112.delta=({struct _tuple2 _tmp113;_tmp113.f1=e2;_tmp113.f2=Cyc_Absyn_skip_stmt(
e2->loc);_tmp113;});_tmp112.body=s;_tmp112;});_tmp111;});_tmp110;}),loc);}struct
Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s){return({
struct Cyc_Absyn_Pat*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->r=(void*)p;
_tmp115->topt=0;_tmp115->loc=s;_tmp115;});}struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Decl*_tmp116=
_cycalloc(sizeof(*_tmp116));_tmp116->r=(void*)r;_tmp116->loc=loc;_tmp116;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Let_d_struct*
_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117[0]=({struct Cyc_Absyn_Let_d_struct
_tmp118;_tmp118.tag=2;_tmp118.f1=p;_tmp118.f2=0;_tmp118.f3=e;_tmp118;});_tmp117;}),
loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Letv_d_struct*_tmp119=
_cycalloc(sizeof(*_tmp119));_tmp119[0]=({struct Cyc_Absyn_Letv_d_struct _tmp11A;
_tmp11A.tag=3;_tmp11A.f1=vds;_tmp11A;});_tmp119;}),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){return({
struct Cyc_Absyn_Vardecl*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->sc=(void*)((
void*)2);_tmp11B->name=x;_tmp11B->tq=Cyc_Absyn_empty_tqual();_tmp11B->type=(void*)
t;_tmp11B->initializer=init;_tmp11B->rgn=0;_tmp11B->attributes=0;_tmp11B->escapes=
0;_tmp11B;});}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,
void*t,struct Cyc_Absyn_Exp*init){return({struct Cyc_Absyn_Vardecl*_tmp11C=
_cycalloc(sizeof(*_tmp11C));_tmp11C->sc=(void*)((void*)0);_tmp11C->name=x;
_tmp11C->tq=Cyc_Absyn_empty_tqual();_tmp11C->type=(void*)t;_tmp11C->initializer=
init;_tmp11C->rgn=0;_tmp11C->attributes=0;_tmp11C->escapes=0;_tmp11C;});}struct
Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct
Cyc_List_List*po,struct Cyc_List_List*fs){return({struct Cyc_Absyn_AggrdeclImpl*
_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->exist_vars=exists;_tmp11D->rgn_po=
po;_tmp11D->fields=fs;_tmp11D;});}struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*
k,void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp11E=_cycalloc(sizeof(*_tmp11E));
_tmp11E[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp11F;_tmp11F.tag=4;_tmp11F.f1=({
struct Cyc_Absyn_Aggrdecl*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->kind=(void*)
k;_tmp120->sc=(void*)s;_tmp120->name=n;_tmp120->tvs=ts;_tmp120->impl=i;_tmp120->attributes=
atts;_tmp120;});_tmp11F;});_tmp11E;}),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((
void*)0,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((void*)1,s,n,ts,i,
atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,
struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_xtunion,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp121=
_cycalloc(sizeof(*_tmp121));_tmp121[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp122;
_tmp122.tag=5;_tmp122.f1=({struct Cyc_Absyn_Tuniondecl*_tmp123=_cycalloc(sizeof(*
_tmp123));_tmp123->sc=(void*)s;_tmp123->name=n;_tmp123->tvs=ts;_tmp123->fields=
fs;_tmp123->is_xtunion=is_xtunion;_tmp123;});_tmp122;});_tmp121;}),loc);}static
struct _tuple1*Cyc_Absyn_expand_arg(struct _tuple1*a){return({struct _tuple1*
_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->f1=(*a).f1;_tmp124->f2=(*a).f2;
_tmp124->f3=Cyc_Absyn_pointer_expand((*a).f3);_tmp124;});}void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts){return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125[0]=({struct Cyc_Absyn_FnType_struct
_tmp126;_tmp126.tag=8;_tmp126.f1=({struct Cyc_Absyn_FnInfo _tmp127;_tmp127.tvars=
tvs;_tmp127.ret_typ=(void*)Cyc_Absyn_pointer_expand(ret_typ);_tmp127.effect=
eff_typ;_tmp127.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple1*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,args);_tmp127.c_varargs=
c_varargs;_tmp127.cyc_varargs=cyc_varargs;_tmp127.rgn_po=rgn_po;_tmp127.attributes=
atts;_tmp127;});_tmp126;});_tmp125;});}void*Cyc_Absyn_pointer_expand(void*t){
void*_tmp128=Cyc_Tcutil_compress(t);_LL43: if(_tmp128 <= (void*)3?1:*((int*)
_tmp128)!= 8)goto _LL45;_LL44: return Cyc_Absyn_at_typ(t,(void*)2,Cyc_Absyn_empty_tqual(),
Cyc_Absyn_false_conref);_LL45:;_LL46: return t;_LL42:;}int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*e){void*_tmp129=(void*)e->r;void*_tmp12A;void*_tmp12B;struct
Cyc_Absyn_Vardecl*_tmp12C;void*_tmp12D;struct Cyc_Absyn_Vardecl*_tmp12E;struct Cyc_Absyn_Exp*
_tmp12F;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp131;_LL48: if(*((int*)
_tmp129)!= 1)goto _LL4A;_tmp12A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp129)->f2;
if(_tmp12A <= (void*)1?1:*((int*)_tmp12A)!= 1)goto _LL4A;_LL49: return 0;_LL4A: if(*((
int*)_tmp129)!= 1)goto _LL4C;_tmp12B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp129)->f2;if(_tmp12B <= (void*)1?1:*((int*)_tmp12B)!= 0)goto _LL4C;_tmp12C=((
struct Cyc_Absyn_Global_b_struct*)_tmp12B)->f1;_LL4B: _tmp12E=_tmp12C;goto _LL4D;
_LL4C: if(*((int*)_tmp129)!= 1)goto _LL4E;_tmp12D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp129)->f2;if(_tmp12D <= (void*)1?1:*((int*)_tmp12D)!= 3)goto _LL4E;_tmp12E=((
struct Cyc_Absyn_Local_b_struct*)_tmp12D)->f1;_LL4D: {void*_tmp132=Cyc_Tcutil_compress((
void*)_tmp12E->type);_LL5F: if(_tmp132 <= (void*)3?1:*((int*)_tmp132)!= 7)goto
_LL61;_LL60: return 0;_LL61:;_LL62: return 1;_LL5E:;}_LL4E: if(*((int*)_tmp129)!= 1)
goto _LL50;_LL4F: goto _LL51;_LL50: if(*((int*)_tmp129)!= 22)goto _LL52;_LL51: goto
_LL53;_LL52: if(*((int*)_tmp129)!= 20)goto _LL54;_LL53: goto _LL55;_LL54: if(*((int*)
_tmp129)!= 23)goto _LL56;_LL55: return 1;_LL56: if(*((int*)_tmp129)!= 21)goto _LL58;
_tmp12F=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp129)->f1;_LL57: return Cyc_Absyn_is_lvalue(
_tmp12F);_LL58: if(*((int*)_tmp129)!= 12)goto _LL5A;_tmp130=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp129)->f1;_LL59: return Cyc_Absyn_is_lvalue(_tmp130);_LL5A: if(*((int*)_tmp129)
!= 11)goto _LL5C;_tmp131=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp129)->f1;
_LL5B: return Cyc_Absyn_is_lvalue(_tmp131);_LL5C:;_LL5D: return 0;_LL47:;}struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _tagged_arr*v){{struct
Cyc_List_List*_tmp133=fields;for(0;_tmp133 != 0;_tmp133=_tmp133->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Aggrfield*)_tmp133->hd)->name,v)== 0)return(struct Cyc_Absyn_Aggrfield*)((
struct Cyc_Absyn_Aggrfield*)_tmp133->hd);}}return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*ad,struct _tagged_arr*v){return ad->impl == 0?0: Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple3*
Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts
== 0)return 0;ts=ts->tl;}if(ts == 0)return 0;return(struct _tuple3*)((struct _tuple3*)
ts->hd);}struct _tagged_arr Cyc_Absyn_attribute2string(void*a){void*_tmp134=a;int
_tmp135;int _tmp136;struct _tagged_arr _tmp137;void*_tmp138;int _tmp139;int _tmp13A;
void*_tmp13B;int _tmp13C;int _tmp13D;int _tmp13E;_LL64: if(_tmp134 <= (void*)17?1:*((
int*)_tmp134)!= 0)goto _LL66;_tmp135=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp134)->f1;_LL65: return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp141;
_tmp141.tag=1;_tmp141.f1=(unsigned int)_tmp135;{void*_tmp13F[1]={& _tmp141};Cyc_aprintf(({
const char*_tmp140="regparm(%d)";_tag_arr(_tmp140,sizeof(char),_get_zero_arr_size(
_tmp140,12));}),_tag_arr(_tmp13F,sizeof(void*),1));}});_LL66: if((int)_tmp134 != 0)
goto _LL68;_LL67: return({const char*_tmp142="stdcall";_tag_arr(_tmp142,sizeof(char),
_get_zero_arr_size(_tmp142,8));});_LL68: if((int)_tmp134 != 1)goto _LL6A;_LL69:
return({const char*_tmp143="cdecl";_tag_arr(_tmp143,sizeof(char),
_get_zero_arr_size(_tmp143,6));});_LL6A: if((int)_tmp134 != 2)goto _LL6C;_LL6B:
return({const char*_tmp144="fastcall";_tag_arr(_tmp144,sizeof(char),
_get_zero_arr_size(_tmp144,9));});_LL6C: if((int)_tmp134 != 3)goto _LL6E;_LL6D:
return({const char*_tmp145="noreturn";_tag_arr(_tmp145,sizeof(char),
_get_zero_arr_size(_tmp145,9));});_LL6E: if((int)_tmp134 != 4)goto _LL70;_LL6F:
return({const char*_tmp146="const";_tag_arr(_tmp146,sizeof(char),
_get_zero_arr_size(_tmp146,6));});_LL70: if(_tmp134 <= (void*)17?1:*((int*)_tmp134)
!= 1)goto _LL72;_tmp136=((struct Cyc_Absyn_Aligned_att_struct*)_tmp134)->f1;_LL71:
if(_tmp136 == - 1)return({const char*_tmp147="aligned";_tag_arr(_tmp147,sizeof(char),
_get_zero_arr_size(_tmp147,8));});else{return(struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp14A;_tmp14A.tag=1;_tmp14A.f1=(unsigned int)_tmp136;{void*_tmp148[1]={&
_tmp14A};Cyc_aprintf(({const char*_tmp149="aligned(%d)";_tag_arr(_tmp149,sizeof(
char),_get_zero_arr_size(_tmp149,12));}),_tag_arr(_tmp148,sizeof(void*),1));}});}
_LL72: if((int)_tmp134 != 5)goto _LL74;_LL73: return({const char*_tmp14B="packed";
_tag_arr(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,7));});_LL74: if(_tmp134
<= (void*)17?1:*((int*)_tmp134)!= 2)goto _LL76;_tmp137=((struct Cyc_Absyn_Section_att_struct*)
_tmp134)->f1;_LL75: return(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp14E;
_tmp14E.tag=0;_tmp14E.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp137);{void*
_tmp14C[1]={& _tmp14E};Cyc_aprintf(({const char*_tmp14D="section(\"%s\")";_tag_arr(
_tmp14D,sizeof(char),_get_zero_arr_size(_tmp14D,14));}),_tag_arr(_tmp14C,sizeof(
void*),1));}});_LL76: if((int)_tmp134 != 6)goto _LL78;_LL77: return({const char*
_tmp14F="nocommon";_tag_arr(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,9));});
_LL78: if((int)_tmp134 != 7)goto _LL7A;_LL79: return({const char*_tmp150="shared";
_tag_arr(_tmp150,sizeof(char),_get_zero_arr_size(_tmp150,7));});_LL7A: if((int)
_tmp134 != 8)goto _LL7C;_LL7B: return({const char*_tmp151="unused";_tag_arr(_tmp151,
sizeof(char),_get_zero_arr_size(_tmp151,7));});_LL7C: if((int)_tmp134 != 9)goto
_LL7E;_LL7D: return({const char*_tmp152="weak";_tag_arr(_tmp152,sizeof(char),
_get_zero_arr_size(_tmp152,5));});_LL7E: if((int)_tmp134 != 10)goto _LL80;_LL7F:
return({const char*_tmp153="dllimport";_tag_arr(_tmp153,sizeof(char),
_get_zero_arr_size(_tmp153,10));});_LL80: if((int)_tmp134 != 11)goto _LL82;_LL81:
return({const char*_tmp154="dllexport";_tag_arr(_tmp154,sizeof(char),
_get_zero_arr_size(_tmp154,10));});_LL82: if((int)_tmp134 != 12)goto _LL84;_LL83:
return({const char*_tmp155="no_instrument_function";_tag_arr(_tmp155,sizeof(char),
_get_zero_arr_size(_tmp155,23));});_LL84: if((int)_tmp134 != 13)goto _LL86;_LL85:
return({const char*_tmp156="constructor";_tag_arr(_tmp156,sizeof(char),
_get_zero_arr_size(_tmp156,12));});_LL86: if((int)_tmp134 != 14)goto _LL88;_LL87:
return({const char*_tmp157="destructor";_tag_arr(_tmp157,sizeof(char),
_get_zero_arr_size(_tmp157,11));});_LL88: if((int)_tmp134 != 15)goto _LL8A;_LL89:
return({const char*_tmp158="no_check_memory_usage";_tag_arr(_tmp158,sizeof(char),
_get_zero_arr_size(_tmp158,22));});_LL8A: if(_tmp134 <= (void*)17?1:*((int*)
_tmp134)!= 3)goto _LL8C;_tmp138=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp134)->f1;if((int)_tmp138 != 0)goto _LL8C;_tmp139=((struct Cyc_Absyn_Format_att_struct*)
_tmp134)->f2;_tmp13A=((struct Cyc_Absyn_Format_att_struct*)_tmp134)->f3;_LL8B:
return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp15C;_tmp15C.tag=1;_tmp15C.f1=(
unsigned int)_tmp13A;{struct Cyc_Int_pa_struct _tmp15B;_tmp15B.tag=1;_tmp15B.f1=(
unsigned int)_tmp139;{void*_tmp159[2]={& _tmp15B,& _tmp15C};Cyc_aprintf(({const
char*_tmp15A="format(printf,%u,%u)";_tag_arr(_tmp15A,sizeof(char),
_get_zero_arr_size(_tmp15A,21));}),_tag_arr(_tmp159,sizeof(void*),2));}}});_LL8C:
if(_tmp134 <= (void*)17?1:*((int*)_tmp134)!= 3)goto _LL8E;_tmp13B=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp134)->f1;if((int)_tmp13B != 1)goto _LL8E;_tmp13C=((
struct Cyc_Absyn_Format_att_struct*)_tmp134)->f2;_tmp13D=((struct Cyc_Absyn_Format_att_struct*)
_tmp134)->f3;_LL8D: return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp160;
_tmp160.tag=1;_tmp160.f1=(unsigned int)_tmp13D;{struct Cyc_Int_pa_struct _tmp15F;
_tmp15F.tag=1;_tmp15F.f1=(unsigned int)_tmp13C;{void*_tmp15D[2]={& _tmp15F,&
_tmp160};Cyc_aprintf(({const char*_tmp15E="format(scanf,%u,%u)";_tag_arr(_tmp15E,
sizeof(char),_get_zero_arr_size(_tmp15E,20));}),_tag_arr(_tmp15D,sizeof(void*),2));}}});
_LL8E: if(_tmp134 <= (void*)17?1:*((int*)_tmp134)!= 4)goto _LL90;_tmp13E=((struct
Cyc_Absyn_Initializes_att_struct*)_tmp134)->f1;_LL8F: return(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp163;_tmp163.tag=1;_tmp163.f1=(unsigned int)_tmp13E;{
void*_tmp161[1]={& _tmp163};Cyc_aprintf(({const char*_tmp162="initializes(%d)";
_tag_arr(_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,16));}),_tag_arr(
_tmp161,sizeof(void*),1));}});_LL90: if((int)_tmp134 != 16)goto _LL63;_LL91: return({
const char*_tmp164="pure";_tag_arr(_tmp164,sizeof(char),_get_zero_arr_size(
_tmp164,5));});_LL63:;}int Cyc_Absyn_fntype_att(void*a){void*_tmp165=a;_LL93: if(
_tmp165 <= (void*)17?1:*((int*)_tmp165)!= 0)goto _LL95;_LL94: goto _LL96;_LL95: if((
int)_tmp165 != 2)goto _LL97;_LL96: goto _LL98;_LL97: if((int)_tmp165 != 0)goto _LL99;
_LL98: goto _LL9A;_LL99: if((int)_tmp165 != 1)goto _LL9B;_LL9A: goto _LL9C;_LL9B: if((
int)_tmp165 != 3)goto _LL9D;_LL9C: goto _LL9E;_LL9D: if((int)_tmp165 != 16)goto _LL9F;
_LL9E: goto _LLA0;_LL9F: if(_tmp165 <= (void*)17?1:*((int*)_tmp165)!= 3)goto _LLA1;
_LLA0: goto _LLA2;_LLA1: if((int)_tmp165 != 4)goto _LLA3;_LLA2: return 1;_LLA3: if(
_tmp165 <= (void*)17?1:*((int*)_tmp165)!= 4)goto _LLA5;_LLA4: return 1;_LLA5:;_LLA6:
return 0;_LL92:;}static char _tmp166[3]="f0";static struct _tagged_arr Cyc_Absyn_f0={
_tmp166,_tmp166,_tmp166 + 3};static struct _tagged_arr*Cyc_Absyn_field_names_v[1]={&
Cyc_Absyn_f0};static struct _tagged_arr Cyc_Absyn_field_names={(void*)((struct
_tagged_arr**)Cyc_Absyn_field_names_v),(void*)((struct _tagged_arr**)Cyc_Absyn_field_names_v),(
void*)((struct _tagged_arr**)Cyc_Absyn_field_names_v + 1)};struct _tagged_arr*Cyc_Absyn_fieldname(
int i){unsigned int fsz=_get_arr_size(Cyc_Absyn_field_names,sizeof(struct
_tagged_arr*));if(i >= fsz)Cyc_Absyn_field_names=({unsigned int _tmp167=(
unsigned int)(i + 1);struct _tagged_arr**_tmp168=(struct _tagged_arr**)_cycalloc(
_check_times(sizeof(struct _tagged_arr*),_tmp167));struct _tagged_arr _tmp16E=
_tag_arr(_tmp168,sizeof(struct _tagged_arr*),_tmp167);{unsigned int _tmp169=
_tmp167;unsigned int j;for(j=0;j < _tmp169;j ++){_tmp168[j]=j < fsz?*((struct
_tagged_arr**)_check_unknown_subscript(Cyc_Absyn_field_names,sizeof(struct
_tagged_arr*),(int)j)):({struct _tagged_arr*_tmp16A=_cycalloc(sizeof(*_tmp16A));
_tmp16A[0]=(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp16D;_tmp16D.tag=1;
_tmp16D.f1=(unsigned int)((int)j);{void*_tmp16B[1]={& _tmp16D};Cyc_aprintf(({
const char*_tmp16C="f%d";_tag_arr(_tmp16C,sizeof(char),_get_zero_arr_size(_tmp16C,
4));}),_tag_arr(_tmp16B,sizeof(void*),1));}});_tmp16A;});}}_tmp16E;});return*((
struct _tagged_arr**)_check_unknown_subscript(Cyc_Absyn_field_names,sizeof(struct
_tagged_arr*),i));}struct _tuple4 Cyc_Absyn_aggr_kinded_name(void*info){void*
_tmp16F=info;void*_tmp170;struct _tuple0*_tmp171;struct Cyc_Absyn_Aggrdecl**
_tmp172;struct Cyc_Absyn_Aggrdecl*_tmp173;struct Cyc_Absyn_Aggrdecl _tmp174;void*
_tmp175;struct _tuple0*_tmp176;_LLA8: if(*((int*)_tmp16F)!= 0)goto _LLAA;_tmp170=(
void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp16F)->f1;_tmp171=((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp16F)->f2;_LLA9: return({struct _tuple4 _tmp177;_tmp177.f1=_tmp170;_tmp177.f2=
_tmp171;_tmp177;});_LLAA: if(*((int*)_tmp16F)!= 1)goto _LLA7;_tmp172=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp16F)->f1;_tmp173=*_tmp172;_tmp174=*_tmp173;_tmp175=(void*)_tmp174.kind;
_tmp176=_tmp174.name;_LLAB: return({struct _tuple4 _tmp178;_tmp178.f1=_tmp175;
_tmp178.f2=_tmp176;_tmp178;});_LLA7:;}struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
void*info){void*_tmp179=info;void*_tmp17A;struct _tuple0*_tmp17B;struct Cyc_Absyn_Aggrdecl**
_tmp17C;struct Cyc_Absyn_Aggrdecl*_tmp17D;_LLAD: if(*((int*)_tmp179)!= 0)goto _LLAF;
_tmp17A=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp179)->f1;_tmp17B=((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp179)->f2;_LLAE:({void*_tmp17E[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp17F="unchecked aggrdecl";_tag_arr(_tmp17F,sizeof(char),_get_zero_arr_size(
_tmp17F,19));}),_tag_arr(_tmp17E,sizeof(void*),0));});_LLAF: if(*((int*)_tmp179)
!= 1)goto _LLAC;_tmp17C=((struct Cyc_Absyn_KnownAggr_struct*)_tmp179)->f1;_tmp17D=*
_tmp17C;_LLB0: return _tmp17D;_LLAC:;}int Cyc_Absyn_is_union_type(void*t){void*
_tmp180=Cyc_Tcutil_compress(t);void*_tmp181;struct Cyc_Absyn_AggrInfo _tmp182;void*
_tmp183;_LLB2: if(_tmp180 <= (void*)3?1:*((int*)_tmp180)!= 11)goto _LLB4;_tmp181=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp180)->f1;if((int)_tmp181 != 1)
goto _LLB4;_LLB3: return 1;_LLB4: if(_tmp180 <= (void*)3?1:*((int*)_tmp180)!= 10)goto
_LLB6;_tmp182=((struct Cyc_Absyn_AggrType_struct*)_tmp180)->f1;_tmp183=(void*)
_tmp182.aggr_info;_LLB5: return(Cyc_Absyn_aggr_kinded_name(_tmp183)).f1 == (void*)
1;_LLB6:;_LLB7: return 0;_LLB1:;}void Cyc_Absyn_print_decls(struct Cyc_List_List*
decls){((void(*)(void*rep,struct Cyc_List_List**val))Cyc_Marshal_print_type)(Cyc_decls_rep,&
decls);({void*_tmp184[0]={};Cyc_printf(({const char*_tmp185="\n";_tag_arr(_tmp185,
sizeof(char),_get_zero_arr_size(_tmp185,2));}),_tag_arr(_tmp184,sizeof(void*),0));});}
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_decl_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Decl_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_430;static struct Cyc_Typerep_Int_struct Cyc__genrep_5={0,0,32};extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_133;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Vardecl_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep;
static char _tmp187[7]="Static";static struct _tuple7 Cyc__gentuple_137={0,{_tmp187,
_tmp187,_tmp187 + 7}};static char _tmp188[9]="Abstract";static struct _tuple7 Cyc__gentuple_138={
1,{_tmp188,_tmp188,_tmp188 + 9}};static char _tmp189[7]="Public";static struct
_tuple7 Cyc__gentuple_139={2,{_tmp189,_tmp189,_tmp189 + 7}};static char _tmp18A[7]="Extern";
static struct _tuple7 Cyc__gentuple_140={3,{_tmp18A,_tmp18A,_tmp18A + 7}};static char
_tmp18B[8]="ExternC";static struct _tuple7 Cyc__gentuple_141={4,{_tmp18B,_tmp18B,
_tmp18B + 8}};static char _tmp18C[9]="Register";static struct _tuple7 Cyc__gentuple_142={
5,{_tmp18C,_tmp18C,_tmp18C + 9}};static struct _tuple7*Cyc__genarr_143[6]={& Cyc__gentuple_137,&
Cyc__gentuple_138,& Cyc__gentuple_139,& Cyc__gentuple_140,& Cyc__gentuple_141,& Cyc__gentuple_142};
static struct _tuple5*Cyc__genarr_144[0]={};static char _tmp18E[6]="Scope";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_scope_t_rep={5,{_tmp18E,_tmp18E,_tmp18E + 6},{(void*)((struct _tuple7**)
Cyc__genarr_143),(void*)((struct _tuple7**)Cyc__genarr_143),(void*)((struct
_tuple7**)Cyc__genarr_143 + 6)},{(void*)((struct _tuple5**)Cyc__genarr_144),(void*)((
struct _tuple5**)Cyc__genarr_144),(void*)((struct _tuple5**)Cyc__genarr_144 + 0)}};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_10;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_11;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_17;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_18;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_var_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_12;extern struct Cyc_Typerep_FatPtr_struct
Cyc__genrep_13;static struct Cyc_Typerep_Int_struct Cyc__genrep_14={0,0,8};static
struct Cyc_Typerep_FatPtr_struct Cyc__genrep_13={2,(void*)((void*)& Cyc__genrep_14)};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_12={1,1,(void*)((void*)& Cyc__genrep_13)};
static char _tmp192[5]="List";static struct _tagged_arr Cyc__genname_22={_tmp192,
_tmp192,_tmp192 + 5};static char _tmp193[3]="hd";static struct _tuple5 Cyc__gentuple_19={
offsetof(struct Cyc_List_List,hd),{_tmp193,_tmp193,_tmp193 + 3},(void*)& Cyc__genrep_12};
static char _tmp194[3]="tl";static struct _tuple5 Cyc__gentuple_20={offsetof(struct
Cyc_List_List,tl),{_tmp194,_tmp194,_tmp194 + 3},(void*)& Cyc__genrep_18};static
struct _tuple5*Cyc__genarr_21[2]={& Cyc__gentuple_19,& Cyc__gentuple_20};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_var_t46H2_rep={3,(struct _tagged_arr*)& Cyc__genname_22,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_21),(void*)((
struct _tuple5**)Cyc__genarr_21),(void*)((struct _tuple5**)Cyc__genarr_21 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_18={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_var_t46H2_rep)};
struct _tuple13{unsigned int f1;struct Cyc_List_List*f2;};static struct _tuple6 Cyc__gentuple_23={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_24={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_18};static struct _tuple6*Cyc__genarr_25[
2]={& Cyc__gentuple_23,& Cyc__gentuple_24};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_17={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_25),(void*)((
struct _tuple6**)Cyc__genarr_25),(void*)((struct _tuple6**)Cyc__genarr_25 + 2)}};
static char _tmp198[6]="Loc_n";static struct _tuple7 Cyc__gentuple_15={0,{_tmp198,
_tmp198,_tmp198 + 6}};static struct _tuple7*Cyc__genarr_16[1]={& Cyc__gentuple_15};
static char _tmp199[6]="Rel_n";static struct _tuple5 Cyc__gentuple_26={0,{_tmp199,
_tmp199,_tmp199 + 6},(void*)& Cyc__genrep_17};static char _tmp19A[6]="Abs_n";static
struct _tuple5 Cyc__gentuple_27={1,{_tmp19A,_tmp19A,_tmp19A + 6},(void*)& Cyc__genrep_17};
static struct _tuple5*Cyc__genarr_28[2]={& Cyc__gentuple_26,& Cyc__gentuple_27};
static char _tmp19C[8]="Nmspace";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep={
5,{_tmp19C,_tmp19C,_tmp19C + 8},{(void*)((struct _tuple7**)Cyc__genarr_16),(void*)((
struct _tuple7**)Cyc__genarr_16),(void*)((struct _tuple7**)Cyc__genarr_16 + 1)},{(
void*)((struct _tuple5**)Cyc__genarr_28),(void*)((struct _tuple5**)Cyc__genarr_28),(
void*)((struct _tuple5**)Cyc__genarr_28 + 2)}};static struct _tuple6 Cyc__gentuple_29={
offsetof(struct _tuple0,f1),(void*)& Cyc_Absyn_nmspace_t_rep};static struct _tuple6
Cyc__gentuple_30={offsetof(struct _tuple0,f2),(void*)& Cyc__genrep_12};static
struct _tuple6*Cyc__genarr_31[2]={& Cyc__gentuple_29,& Cyc__gentuple_30};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_11={4,sizeof(struct _tuple0),{(void*)((
struct _tuple6**)Cyc__genarr_31),(void*)((struct _tuple6**)Cyc__genarr_31),(void*)((
struct _tuple6**)Cyc__genarr_31 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_10={
1,1,(void*)((void*)& Cyc__genrep_11)};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_134;
struct _tuple14{char f1;};static struct _tuple6 Cyc__gentuple_135={offsetof(struct
_tuple14,f1),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*Cyc__genarr_136[
1]={& Cyc__gentuple_135};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_134={4,
sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_136),(void*)((
struct _tuple6**)Cyc__genarr_136),(void*)((struct _tuple6**)Cyc__genarr_136 + 1)}};
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1077;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1082;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep;extern
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep;static char _tmp1A0[8]="AnyKind";
static struct _tuple7 Cyc__gentuple_188={0,{_tmp1A0,_tmp1A0,_tmp1A0 + 8}};static char
_tmp1A1[8]="MemKind";static struct _tuple7 Cyc__gentuple_189={1,{_tmp1A1,_tmp1A1,
_tmp1A1 + 8}};static char _tmp1A2[8]="BoxKind";static struct _tuple7 Cyc__gentuple_190={
2,{_tmp1A2,_tmp1A2,_tmp1A2 + 8}};static char _tmp1A3[8]="RgnKind";static struct
_tuple7 Cyc__gentuple_191={3,{_tmp1A3,_tmp1A3,_tmp1A3 + 8}};static char _tmp1A4[8]="EffKind";
static struct _tuple7 Cyc__gentuple_192={4,{_tmp1A4,_tmp1A4,_tmp1A4 + 8}};static char
_tmp1A5[8]="IntKind";static struct _tuple7 Cyc__gentuple_193={5,{_tmp1A5,_tmp1A5,
_tmp1A5 + 8}};static struct _tuple7*Cyc__genarr_194[6]={& Cyc__gentuple_188,& Cyc__gentuple_189,&
Cyc__gentuple_190,& Cyc__gentuple_191,& Cyc__gentuple_192,& Cyc__gentuple_193};
static struct _tuple5*Cyc__genarr_195[0]={};static char _tmp1A7[5]="Kind";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kind_t_rep={5,{_tmp1A7,_tmp1A7,_tmp1A7 + 5},{(void*)((struct _tuple7**)
Cyc__genarr_194),(void*)((struct _tuple7**)Cyc__genarr_194),(void*)((struct
_tuple7**)Cyc__genarr_194 + 6)},{(void*)((struct _tuple5**)Cyc__genarr_195),(void*)((
struct _tuple5**)Cyc__genarr_195),(void*)((struct _tuple5**)Cyc__genarr_195 + 0)}};
static char _tmp1A8[4]="Opt";static struct _tagged_arr Cyc__genname_1085={_tmp1A8,
_tmp1A8,_tmp1A8 + 4};static char _tmp1A9[2]="v";static struct _tuple5 Cyc__gentuple_1083={
offsetof(struct Cyc_Core_Opt,v),{_tmp1A9,_tmp1A9,_tmp1A9 + 2},(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple5*Cyc__genarr_1084[1]={& Cyc__gentuple_1083};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kind_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_1085,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1084),(void*)((
struct _tuple5**)Cyc__genarr_1084),(void*)((struct _tuple5**)Cyc__genarr_1084 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1082={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kind_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_43;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_type_t2_rep;static char _tmp1AC[4]="Opt";static struct
_tagged_arr Cyc__genname_1117={_tmp1AC,_tmp1AC,_tmp1AC + 4};static char _tmp1AD[2]="v";
static struct _tuple5 Cyc__gentuple_1115={offsetof(struct Cyc_Core_Opt,v),{_tmp1AD,
_tmp1AD,_tmp1AD + 2},(void*)& Cyc_Absyn_type_t_rep};static struct _tuple5*Cyc__genarr_1116[
1]={& Cyc__gentuple_1115};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_type_t2_rep={
3,(struct _tagged_arr*)& Cyc__genname_1117,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_1116),(void*)((struct _tuple5**)Cyc__genarr_1116),(
void*)((struct _tuple5**)Cyc__genarr_1116 + 1)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_43={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_type_t2_rep)};
static struct Cyc_Typerep_Int_struct Cyc__genrep_62={0,1,32};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1078;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_311;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tvar_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_185;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_216;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_216={1,1,(void*)((void*)& Cyc__genrep_62)};extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kindbound_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_208;
static struct _tuple6 Cyc__gentuple_209={offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_210={offsetof(struct _tuple6,f2),(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple6*Cyc__genarr_211[2]={& Cyc__gentuple_209,& Cyc__gentuple_210};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_208={4,sizeof(struct _tuple6),{(
void*)((struct _tuple6**)Cyc__genarr_211),(void*)((struct _tuple6**)Cyc__genarr_211),(
void*)((struct _tuple6**)Cyc__genarr_211 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_204;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_196;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep;static
char _tmp1B3[4]="Opt";static struct _tagged_arr Cyc__genname_199={_tmp1B3,_tmp1B3,
_tmp1B3 + 4};static char _tmp1B4[2]="v";static struct _tuple5 Cyc__gentuple_197={
offsetof(struct Cyc_Core_Opt,v),{_tmp1B4,_tmp1B4,_tmp1B4 + 2},(void*)& Cyc_Absyn_kindbound_t_rep};
static struct _tuple5*Cyc__genarr_198[1]={& Cyc__gentuple_197};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_199,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_198),(void*)((
struct _tuple5**)Cyc__genarr_198),(void*)((struct _tuple5**)Cyc__genarr_198 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_196={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep)};
struct _tuple15{unsigned int f1;struct Cyc_Core_Opt*f2;};static struct _tuple6 Cyc__gentuple_205={
offsetof(struct _tuple15,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_206={
offsetof(struct _tuple15,f2),(void*)& Cyc__genrep_196};static struct _tuple6*Cyc__genarr_207[
2]={& Cyc__gentuple_205,& Cyc__gentuple_206};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_204={4,sizeof(struct _tuple15),{(void*)((struct _tuple6**)Cyc__genarr_207),(
void*)((struct _tuple6**)Cyc__genarr_207),(void*)((struct _tuple6**)Cyc__genarr_207
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_187;struct _tuple16{
unsigned int f1;struct Cyc_Core_Opt*f2;void*f3;};static struct _tuple6 Cyc__gentuple_200={
offsetof(struct _tuple16,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_201={
offsetof(struct _tuple16,f2),(void*)& Cyc__genrep_196};static struct _tuple6 Cyc__gentuple_202={
offsetof(struct _tuple16,f3),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple6*
Cyc__genarr_203[3]={& Cyc__gentuple_200,& Cyc__gentuple_201,& Cyc__gentuple_202};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_187={4,sizeof(struct _tuple16),{(
void*)((struct _tuple6**)Cyc__genarr_203),(void*)((struct _tuple6**)Cyc__genarr_203),(
void*)((struct _tuple6**)Cyc__genarr_203 + 3)}};static struct _tuple7*Cyc__genarr_186[
0]={};static char _tmp1B9[6]="Eq_kb";static struct _tuple5 Cyc__gentuple_212={0,{
_tmp1B9,_tmp1B9,_tmp1B9 + 6},(void*)& Cyc__genrep_208};static char _tmp1BA[11]="Unknown_kb";
static struct _tuple5 Cyc__gentuple_213={1,{_tmp1BA,_tmp1BA,_tmp1BA + 11},(void*)&
Cyc__genrep_204};static char _tmp1BB[8]="Less_kb";static struct _tuple5 Cyc__gentuple_214={
2,{_tmp1BB,_tmp1BB,_tmp1BB + 8},(void*)& Cyc__genrep_187};static struct _tuple5*Cyc__genarr_215[
3]={& Cyc__gentuple_212,& Cyc__gentuple_213,& Cyc__gentuple_214};static char _tmp1BD[
10]="KindBound";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep={5,{
_tmp1BD,_tmp1BD,_tmp1BD + 10},{(void*)((struct _tuple7**)Cyc__genarr_186),(void*)((
struct _tuple7**)Cyc__genarr_186),(void*)((struct _tuple7**)Cyc__genarr_186 + 0)},{(
void*)((struct _tuple5**)Cyc__genarr_215),(void*)((struct _tuple5**)Cyc__genarr_215),(
void*)((struct _tuple5**)Cyc__genarr_215 + 3)}};static char _tmp1BE[5]="Tvar";static
struct _tagged_arr Cyc__genname_221={_tmp1BE,_tmp1BE,_tmp1BE + 5};static char _tmp1BF[
5]="name";static struct _tuple5 Cyc__gentuple_217={offsetof(struct Cyc_Absyn_Tvar,name),{
_tmp1BF,_tmp1BF,_tmp1BF + 5},(void*)& Cyc__genrep_12};static char _tmp1C0[9]="identity";
static struct _tuple5 Cyc__gentuple_218={offsetof(struct Cyc_Absyn_Tvar,identity),{
_tmp1C0,_tmp1C0,_tmp1C0 + 9},(void*)& Cyc__genrep_216};static char _tmp1C1[5]="kind";
static struct _tuple5 Cyc__gentuple_219={offsetof(struct Cyc_Absyn_Tvar,kind),{
_tmp1C1,_tmp1C1,_tmp1C1 + 5},(void*)& Cyc_Absyn_kindbound_t_rep};static struct
_tuple5*Cyc__genarr_220[3]={& Cyc__gentuple_217,& Cyc__gentuple_218,& Cyc__gentuple_219};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep={3,(struct _tagged_arr*)&
Cyc__genname_221,sizeof(struct Cyc_Absyn_Tvar),{(void*)((struct _tuple5**)Cyc__genarr_220),(
void*)((struct _tuple5**)Cyc__genarr_220),(void*)((struct _tuple5**)Cyc__genarr_220
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_185={1,1,(void*)((void*)&
Cyc_struct_Absyn_Tvar_rep)};static char _tmp1C4[5]="List";static struct _tagged_arr
Cyc__genname_315={_tmp1C4,_tmp1C4,_tmp1C4 + 5};static char _tmp1C5[3]="hd";static
struct _tuple5 Cyc__gentuple_312={offsetof(struct Cyc_List_List,hd),{_tmp1C5,
_tmp1C5,_tmp1C5 + 3},(void*)& Cyc__genrep_185};static char _tmp1C6[3]="tl";static
struct _tuple5 Cyc__gentuple_313={offsetof(struct Cyc_List_List,tl),{_tmp1C6,
_tmp1C6,_tmp1C6 + 3},(void*)& Cyc__genrep_311};static struct _tuple5*Cyc__genarr_314[
2]={& Cyc__gentuple_312,& Cyc__gentuple_313};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tvar_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_315,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_314),(void*)((struct _tuple5**)Cyc__genarr_314),(void*)((
struct _tuple5**)Cyc__genarr_314 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_311={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tvar_t46H2_rep)};static char
_tmp1C9[4]="Opt";static struct _tagged_arr Cyc__genname_1081={_tmp1C9,_tmp1C9,
_tmp1C9 + 4};static char _tmp1CA[2]="v";static struct _tuple5 Cyc__gentuple_1079={
offsetof(struct Cyc_Core_Opt,v),{_tmp1CA,_tmp1CA,_tmp1CA + 2},(void*)& Cyc__genrep_311};
static struct _tuple5*Cyc__genarr_1080[1]={& Cyc__gentuple_1079};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_1081,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1080),(
void*)((struct _tuple5**)Cyc__genarr_1080),(void*)((struct _tuple5**)Cyc__genarr_1080
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1078={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep)};struct _tuple17{
unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;int f4;struct Cyc_Core_Opt*
f5;};static struct _tuple6 Cyc__gentuple_1086={offsetof(struct _tuple17,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1087={offsetof(struct _tuple17,f2),(
void*)& Cyc__genrep_1082};static struct _tuple6 Cyc__gentuple_1088={offsetof(struct
_tuple17,f3),(void*)& Cyc__genrep_43};static struct _tuple6 Cyc__gentuple_1089={
offsetof(struct _tuple17,f4),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6
Cyc__gentuple_1090={offsetof(struct _tuple17,f5),(void*)& Cyc__genrep_1078};static
struct _tuple6*Cyc__genarr_1091[5]={& Cyc__gentuple_1086,& Cyc__gentuple_1087,& Cyc__gentuple_1088,&
Cyc__gentuple_1089,& Cyc__gentuple_1090};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1077={
4,sizeof(struct _tuple17),{(void*)((struct _tuple6**)Cyc__genarr_1091),(void*)((
struct _tuple6**)Cyc__genarr_1091),(void*)((struct _tuple6**)Cyc__genarr_1091 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1073;struct _tuple18{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;};static struct _tuple6 Cyc__gentuple_1074={offsetof(
struct _tuple18,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1075={
offsetof(struct _tuple18,f2),(void*)& Cyc__genrep_185};static struct _tuple6*Cyc__genarr_1076[
2]={& Cyc__gentuple_1074,& Cyc__gentuple_1075};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1073={4,sizeof(struct _tuple18),{(void*)((struct _tuple6**)Cyc__genarr_1076),(
void*)((struct _tuple6**)Cyc__genarr_1076),(void*)((struct _tuple6**)Cyc__genarr_1076
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1047;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1054;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep;static char _tmp1CF[18]="UnknownTunionInfo";
static struct _tagged_arr Cyc__genname_1058={_tmp1CF,_tmp1CF,_tmp1CF + 18};static
char _tmp1D0[5]="name";static struct _tuple5 Cyc__gentuple_1055={offsetof(struct Cyc_Absyn_UnknownTunionInfo,name),{
_tmp1D0,_tmp1D0,_tmp1D0 + 5},(void*)& Cyc__genrep_10};static char _tmp1D1[11]="is_xtunion";
static struct _tuple5 Cyc__gentuple_1056={offsetof(struct Cyc_Absyn_UnknownTunionInfo,is_xtunion),{
_tmp1D1,_tmp1D1,_tmp1D1 + 11},(void*)((void*)& Cyc__genrep_62)};static struct
_tuple5*Cyc__genarr_1057[2]={& Cyc__gentuple_1055,& Cyc__gentuple_1056};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep={3,(struct _tagged_arr*)& Cyc__genname_1058,
sizeof(struct Cyc_Absyn_UnknownTunionInfo),{(void*)((struct _tuple5**)Cyc__genarr_1057),(
void*)((struct _tuple5**)Cyc__genarr_1057),(void*)((struct _tuple5**)Cyc__genarr_1057
+ 2)}};struct _tuple19{unsigned int f1;struct Cyc_Absyn_UnknownTunionInfo f2;};
static struct _tuple6 Cyc__gentuple_1059={offsetof(struct _tuple19,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_1060={offsetof(struct _tuple19,f2),(void*)& Cyc_struct_Absyn_UnknownTunionInfo_rep};
static struct _tuple6*Cyc__genarr_1061[2]={& Cyc__gentuple_1059,& Cyc__gentuple_1060};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1054={4,sizeof(struct _tuple19),{(
void*)((struct _tuple6**)Cyc__genarr_1061),(void*)((struct _tuple6**)Cyc__genarr_1061),(
void*)((struct _tuple6**)Cyc__genarr_1061 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1049;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1050;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_301;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tuniondecl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_302;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_303;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_284;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_285;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_286;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_287;static struct
_tuple6 Cyc__gentuple_288={offsetof(struct _tuple3,f1),(void*)& Cyc__genrep_134};
static struct _tuple6 Cyc__gentuple_289={offsetof(struct _tuple3,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_290[2]={& Cyc__gentuple_288,&
Cyc__gentuple_289};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_287={4,
sizeof(struct _tuple3),{(void*)((struct _tuple6**)Cyc__genarr_290),(void*)((struct
_tuple6**)Cyc__genarr_290),(void*)((struct _tuple6**)Cyc__genarr_290 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_286={1,1,(void*)((void*)& Cyc__genrep_287)};
static char _tmp1D6[5]="List";static struct _tagged_arr Cyc__genname_294={_tmp1D6,
_tmp1D6,_tmp1D6 + 5};static char _tmp1D7[3]="hd";static struct _tuple5 Cyc__gentuple_291={
offsetof(struct Cyc_List_List,hd),{_tmp1D7,_tmp1D7,_tmp1D7 + 3},(void*)& Cyc__genrep_286};
static char _tmp1D8[3]="tl";static struct _tuple5 Cyc__gentuple_292={offsetof(struct
Cyc_List_List,tl),{_tmp1D8,_tmp1D8,_tmp1D8 + 3},(void*)& Cyc__genrep_285};static
struct _tuple5*Cyc__genarr_293[2]={& Cyc__gentuple_291,& Cyc__gentuple_292};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_294,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_293),(void*)((struct _tuple5**)Cyc__genarr_293),(void*)((
struct _tuple5**)Cyc__genarr_293 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_285={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Position_Segment_rep;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2={
1,1,(void*)((void*)& Cyc_struct_Position_Segment_rep)};static char _tmp1DC[12]="Tunionfield";
static struct _tagged_arr Cyc__genname_300={_tmp1DC,_tmp1DC,_tmp1DC + 12};static char
_tmp1DD[5]="name";static struct _tuple5 Cyc__gentuple_295={offsetof(struct Cyc_Absyn_Tunionfield,name),{
_tmp1DD,_tmp1DD,_tmp1DD + 5},(void*)& Cyc__genrep_10};static char _tmp1DE[5]="typs";
static struct _tuple5 Cyc__gentuple_296={offsetof(struct Cyc_Absyn_Tunionfield,typs),{
_tmp1DE,_tmp1DE,_tmp1DE + 5},(void*)& Cyc__genrep_285};static char _tmp1DF[4]="loc";
static struct _tuple5 Cyc__gentuple_297={offsetof(struct Cyc_Absyn_Tunionfield,loc),{
_tmp1DF,_tmp1DF,_tmp1DF + 4},(void*)& Cyc__genrep_2};static char _tmp1E0[3]="sc";
static struct _tuple5 Cyc__gentuple_298={offsetof(struct Cyc_Absyn_Tunionfield,sc),{
_tmp1E0,_tmp1E0,_tmp1E0 + 3},(void*)& Cyc_Absyn_scope_t_rep};static struct _tuple5*
Cyc__genarr_299[4]={& Cyc__gentuple_295,& Cyc__gentuple_296,& Cyc__gentuple_297,&
Cyc__gentuple_298};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep={
3,(struct _tagged_arr*)& Cyc__genname_300,sizeof(struct Cyc_Absyn_Tunionfield),{(
void*)((struct _tuple5**)Cyc__genarr_299),(void*)((struct _tuple5**)Cyc__genarr_299),(
void*)((struct _tuple5**)Cyc__genarr_299 + 4)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_284={1,1,(void*)((void*)& Cyc_struct_Absyn_Tunionfield_rep)};static
char _tmp1E3[5]="List";static struct _tagged_arr Cyc__genname_307={_tmp1E3,_tmp1E3,
_tmp1E3 + 5};static char _tmp1E4[3]="hd";static struct _tuple5 Cyc__gentuple_304={
offsetof(struct Cyc_List_List,hd),{_tmp1E4,_tmp1E4,_tmp1E4 + 3},(void*)& Cyc__genrep_284};
static char _tmp1E5[3]="tl";static struct _tuple5 Cyc__gentuple_305={offsetof(struct
Cyc_List_List,tl),{_tmp1E5,_tmp1E5,_tmp1E5 + 3},(void*)& Cyc__genrep_303};static
struct _tuple5*Cyc__genarr_306[2]={& Cyc__gentuple_304,& Cyc__gentuple_305};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_307,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_306),(void*)((struct _tuple5**)Cyc__genarr_306),(void*)((
struct _tuple5**)Cyc__genarr_306 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_303={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep)};static
char _tmp1E8[4]="Opt";static struct _tagged_arr Cyc__genname_310={_tmp1E8,_tmp1E8,
_tmp1E8 + 4};static char _tmp1E9[2]="v";static struct _tuple5 Cyc__gentuple_308={
offsetof(struct Cyc_Core_Opt,v),{_tmp1E9,_tmp1E9,_tmp1E9 + 2},(void*)& Cyc__genrep_303};
static struct _tuple5*Cyc__genarr_309[1]={& Cyc__gentuple_308};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep={3,(struct
_tagged_arr*)& Cyc__genname_310,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_309),(void*)((struct _tuple5**)Cyc__genarr_309),(void*)((
struct _tuple5**)Cyc__genarr_309 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_302={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep)};
static char _tmp1EC[11]="Tuniondecl";static struct _tagged_arr Cyc__genname_322={
_tmp1EC,_tmp1EC,_tmp1EC + 11};static char _tmp1ED[3]="sc";static struct _tuple5 Cyc__gentuple_316={
offsetof(struct Cyc_Absyn_Tuniondecl,sc),{_tmp1ED,_tmp1ED,_tmp1ED + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp1EE[5]="name";static struct _tuple5 Cyc__gentuple_317={offsetof(
struct Cyc_Absyn_Tuniondecl,name),{_tmp1EE,_tmp1EE,_tmp1EE + 5},(void*)& Cyc__genrep_10};
static char _tmp1EF[4]="tvs";static struct _tuple5 Cyc__gentuple_318={offsetof(struct
Cyc_Absyn_Tuniondecl,tvs),{_tmp1EF,_tmp1EF,_tmp1EF + 4},(void*)& Cyc__genrep_311};
static char _tmp1F0[7]="fields";static struct _tuple5 Cyc__gentuple_319={offsetof(
struct Cyc_Absyn_Tuniondecl,fields),{_tmp1F0,_tmp1F0,_tmp1F0 + 7},(void*)& Cyc__genrep_302};
static char _tmp1F1[11]="is_xtunion";static struct _tuple5 Cyc__gentuple_320={
offsetof(struct Cyc_Absyn_Tuniondecl,is_xtunion),{_tmp1F1,_tmp1F1,_tmp1F1 + 11},(
void*)((void*)& Cyc__genrep_62)};static struct _tuple5*Cyc__genarr_321[5]={& Cyc__gentuple_316,&
Cyc__gentuple_317,& Cyc__gentuple_318,& Cyc__gentuple_319,& Cyc__gentuple_320};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tuniondecl_rep={3,(struct
_tagged_arr*)& Cyc__genname_322,sizeof(struct Cyc_Absyn_Tuniondecl),{(void*)((
struct _tuple5**)Cyc__genarr_321),(void*)((struct _tuple5**)Cyc__genarr_321),(void*)((
struct _tuple5**)Cyc__genarr_321 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_301={
1,1,(void*)((void*)& Cyc_struct_Absyn_Tuniondecl_rep)};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1050={1,1,(void*)((void*)& Cyc__genrep_301)};struct _tuple20{
unsigned int f1;struct Cyc_Absyn_Tuniondecl**f2;};static struct _tuple6 Cyc__gentuple_1051={
offsetof(struct _tuple20,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1052={
offsetof(struct _tuple20,f2),(void*)& Cyc__genrep_1050};static struct _tuple6*Cyc__genarr_1053[
2]={& Cyc__gentuple_1051,& Cyc__gentuple_1052};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1049={4,sizeof(struct _tuple20),{(void*)((struct _tuple6**)Cyc__genarr_1053),(
void*)((struct _tuple6**)Cyc__genarr_1053),(void*)((struct _tuple6**)Cyc__genarr_1053
+ 2)}};static struct _tuple7*Cyc__genarr_1048[0]={};static char _tmp1F6[14]="UnknownTunion";
static struct _tuple5 Cyc__gentuple_1062={0,{_tmp1F6,_tmp1F6,_tmp1F6 + 14},(void*)&
Cyc__genrep_1054};static char _tmp1F7[12]="KnownTunion";static struct _tuple5 Cyc__gentuple_1063={
1,{_tmp1F7,_tmp1F7,_tmp1F7 + 12},(void*)& Cyc__genrep_1049};static struct _tuple5*
Cyc__genarr_1064[2]={& Cyc__gentuple_1062,& Cyc__gentuple_1063};static char _tmp1F9[
12]="TunionInfoU";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep={
5,{_tmp1F9,_tmp1F9,_tmp1F9 + 12},{(void*)((struct _tuple7**)Cyc__genarr_1048),(
void*)((struct _tuple7**)Cyc__genarr_1048),(void*)((struct _tuple7**)Cyc__genarr_1048
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1064),(void*)((struct _tuple5**)Cyc__genarr_1064),(
void*)((struct _tuple5**)Cyc__genarr_1064 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_53;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep;
static char _tmp1FA[5]="List";static struct _tagged_arr Cyc__genname_57={_tmp1FA,
_tmp1FA,_tmp1FA + 5};static char _tmp1FB[3]="hd";static struct _tuple5 Cyc__gentuple_54={
offsetof(struct Cyc_List_List,hd),{_tmp1FB,_tmp1FB,_tmp1FB + 3},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp1FC[3]="tl";static struct _tuple5 Cyc__gentuple_55={
offsetof(struct Cyc_List_List,tl),{_tmp1FC,_tmp1FC,_tmp1FC + 3},(void*)& Cyc__genrep_53};
static struct _tuple5*Cyc__genarr_56[2]={& Cyc__gentuple_54,& Cyc__gentuple_55};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_57,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_56),(void*)((struct _tuple5**)Cyc__genarr_56),(void*)((
struct _tuple5**)Cyc__genarr_56 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_53={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_type_t46H2_rep)};static char
_tmp1FF[11]="TunionInfo";static struct _tagged_arr Cyc__genname_1069={_tmp1FF,
_tmp1FF,_tmp1FF + 11};static char _tmp200[12]="tunion_info";static struct _tuple5 Cyc__gentuple_1065={
offsetof(struct Cyc_Absyn_TunionInfo,tunion_info),{_tmp200,_tmp200,_tmp200 + 12},(
void*)& Cyc_tunion_Absyn_TunionInfoU_rep};static char _tmp201[6]="targs";static
struct _tuple5 Cyc__gentuple_1066={offsetof(struct Cyc_Absyn_TunionInfo,targs),{
_tmp201,_tmp201,_tmp201 + 6},(void*)& Cyc__genrep_53};static char _tmp202[4]="rgn";
static struct _tuple5 Cyc__gentuple_1067={offsetof(struct Cyc_Absyn_TunionInfo,rgn),{
_tmp202,_tmp202,_tmp202 + 4},(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple5*Cyc__genarr_1068[3]={& Cyc__gentuple_1065,& Cyc__gentuple_1066,& Cyc__gentuple_1067};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_info_t_rep={3,(struct _tagged_arr*)&
Cyc__genname_1069,sizeof(struct Cyc_Absyn_TunionInfo),{(void*)((struct _tuple5**)
Cyc__genarr_1068),(void*)((struct _tuple5**)Cyc__genarr_1068),(void*)((struct
_tuple5**)Cyc__genarr_1068 + 3)}};struct _tuple21{unsigned int f1;struct Cyc_Absyn_TunionInfo
f2;};static struct _tuple6 Cyc__gentuple_1070={offsetof(struct _tuple21,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1071={offsetof(struct _tuple21,f2),(
void*)& Cyc_Absyn_tunion_info_t_rep};static struct _tuple6*Cyc__genarr_1072[2]={&
Cyc__gentuple_1070,& Cyc__gentuple_1071};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1047={
4,sizeof(struct _tuple21),{(void*)((struct _tuple6**)Cyc__genarr_1072),(void*)((
struct _tuple6**)Cyc__genarr_1072),(void*)((struct _tuple6**)Cyc__genarr_1072 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1021;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_field_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1028;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep;static char _tmp205[23]="UnknownTunionFieldInfo";
static struct _tagged_arr Cyc__genname_1033={_tmp205,_tmp205,_tmp205 + 23};static
char _tmp206[12]="tunion_name";static struct _tuple5 Cyc__gentuple_1029={offsetof(
struct Cyc_Absyn_UnknownTunionFieldInfo,tunion_name),{_tmp206,_tmp206,_tmp206 + 12},(
void*)& Cyc__genrep_10};static char _tmp207[11]="field_name";static struct _tuple5 Cyc__gentuple_1030={
offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,field_name),{_tmp207,_tmp207,
_tmp207 + 11},(void*)& Cyc__genrep_10};static char _tmp208[11]="is_xtunion";static
struct _tuple5 Cyc__gentuple_1031={offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,is_xtunion),{
_tmp208,_tmp208,_tmp208 + 11},(void*)((void*)& Cyc__genrep_62)};static struct
_tuple5*Cyc__genarr_1032[3]={& Cyc__gentuple_1029,& Cyc__gentuple_1030,& Cyc__gentuple_1031};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_UnknownTunionFieldInfo_rep={3,(
struct _tagged_arr*)& Cyc__genname_1033,sizeof(struct Cyc_Absyn_UnknownTunionFieldInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1032),(void*)((struct _tuple5**)Cyc__genarr_1032),(
void*)((struct _tuple5**)Cyc__genarr_1032 + 3)}};struct _tuple22{unsigned int f1;
struct Cyc_Absyn_UnknownTunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1034={
offsetof(struct _tuple22,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1035={
offsetof(struct _tuple22,f2),(void*)& Cyc_struct_Absyn_UnknownTunionFieldInfo_rep};
static struct _tuple6*Cyc__genarr_1036[2]={& Cyc__gentuple_1034,& Cyc__gentuple_1035};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1028={4,sizeof(struct _tuple22),{(
void*)((struct _tuple6**)Cyc__genarr_1036),(void*)((struct _tuple6**)Cyc__genarr_1036),(
void*)((struct _tuple6**)Cyc__genarr_1036 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1023;struct _tuple23{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;
struct Cyc_Absyn_Tunionfield*f3;};static struct _tuple6 Cyc__gentuple_1024={
offsetof(struct _tuple23,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1025={
offsetof(struct _tuple23,f2),(void*)((void*)& Cyc__genrep_301)};static struct
_tuple6 Cyc__gentuple_1026={offsetof(struct _tuple23,f3),(void*)& Cyc__genrep_284};
static struct _tuple6*Cyc__genarr_1027[3]={& Cyc__gentuple_1024,& Cyc__gentuple_1025,&
Cyc__gentuple_1026};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1023={4,
sizeof(struct _tuple23),{(void*)((struct _tuple6**)Cyc__genarr_1027),(void*)((
struct _tuple6**)Cyc__genarr_1027),(void*)((struct _tuple6**)Cyc__genarr_1027 + 3)}};
static struct _tuple7*Cyc__genarr_1022[0]={};static char _tmp20C[19]="UnknownTunionfield";
static struct _tuple5 Cyc__gentuple_1037={0,{_tmp20C,_tmp20C,_tmp20C + 19},(void*)&
Cyc__genrep_1028};static char _tmp20D[17]="KnownTunionfield";static struct _tuple5
Cyc__gentuple_1038={1,{_tmp20D,_tmp20D,_tmp20D + 17},(void*)& Cyc__genrep_1023};
static struct _tuple5*Cyc__genarr_1039[2]={& Cyc__gentuple_1037,& Cyc__gentuple_1038};
static char _tmp20F[17]="TunionFieldInfoU";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep={
5,{_tmp20F,_tmp20F,_tmp20F + 17},{(void*)((struct _tuple7**)Cyc__genarr_1022),(
void*)((struct _tuple7**)Cyc__genarr_1022),(void*)((struct _tuple7**)Cyc__genarr_1022
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1039),(void*)((struct _tuple5**)Cyc__genarr_1039),(
void*)((struct _tuple5**)Cyc__genarr_1039 + 2)}};static char _tmp210[16]="TunionFieldInfo";
static struct _tagged_arr Cyc__genname_1043={_tmp210,_tmp210,_tmp210 + 16};static
char _tmp211[11]="field_info";static struct _tuple5 Cyc__gentuple_1040={offsetof(
struct Cyc_Absyn_TunionFieldInfo,field_info),{_tmp211,_tmp211,_tmp211 + 11},(void*)&
Cyc_tunion_Absyn_TunionFieldInfoU_rep};static char _tmp212[6]="targs";static struct
_tuple5 Cyc__gentuple_1041={offsetof(struct Cyc_Absyn_TunionFieldInfo,targs),{
_tmp212,_tmp212,_tmp212 + 6},(void*)& Cyc__genrep_53};static struct _tuple5*Cyc__genarr_1042[
2]={& Cyc__gentuple_1040,& Cyc__gentuple_1041};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_field_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_1043,sizeof(struct Cyc_Absyn_TunionFieldInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1042),(void*)((struct _tuple5**)Cyc__genarr_1042),(
void*)((struct _tuple5**)Cyc__genarr_1042 + 2)}};struct _tuple24{unsigned int f1;
struct Cyc_Absyn_TunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1044={
offsetof(struct _tuple24,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1045={
offsetof(struct _tuple24,f2),(void*)& Cyc_Absyn_tunion_field_info_t_rep};static
struct _tuple6*Cyc__genarr_1046[2]={& Cyc__gentuple_1044,& Cyc__gentuple_1045};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1021={4,sizeof(struct _tuple24),{(
void*)((struct _tuple6**)Cyc__genarr_1046),(void*)((struct _tuple6**)Cyc__genarr_1046),(
void*)((struct _tuple6**)Cyc__genarr_1046 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1002;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep;
extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_969;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0bool2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_61;struct _tuple25{unsigned int f1;
int f2;};static struct _tuple6 Cyc__gentuple_63={offsetof(struct _tuple25,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_64={offsetof(struct _tuple25,f2),(
void*)((void*)& Cyc__genrep_62)};static struct _tuple6*Cyc__genarr_65[2]={& Cyc__gentuple_63,&
Cyc__gentuple_64};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_61={4,sizeof(
struct _tuple25),{(void*)((struct _tuple6**)Cyc__genarr_65),(void*)((struct _tuple6**)
Cyc__genarr_65),(void*)((struct _tuple6**)Cyc__genarr_65 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_972;struct _tuple26{unsigned int f1;struct Cyc_Absyn_Conref*f2;};static
struct _tuple6 Cyc__gentuple_973={offsetof(struct _tuple26,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_974={offsetof(struct _tuple26,f2),(void*)& Cyc__genrep_969};
static struct _tuple6*Cyc__genarr_975[2]={& Cyc__gentuple_973,& Cyc__gentuple_974};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_972={4,sizeof(struct _tuple26),{(
void*)((struct _tuple6**)Cyc__genarr_975),(void*)((struct _tuple6**)Cyc__genarr_975),(
void*)((struct _tuple6**)Cyc__genarr_975 + 2)}};static char _tmp217[10]="No_constr";
static struct _tuple7 Cyc__gentuple_970={0,{_tmp217,_tmp217,_tmp217 + 10}};static
struct _tuple7*Cyc__genarr_971[1]={& Cyc__gentuple_970};static char _tmp218[10]="Eq_constr";
static struct _tuple5 Cyc__gentuple_976={0,{_tmp218,_tmp218,_tmp218 + 10},(void*)&
Cyc__genrep_61};static char _tmp219[15]="Forward_constr";static struct _tuple5 Cyc__gentuple_977={
1,{_tmp219,_tmp219,_tmp219 + 15},(void*)& Cyc__genrep_972};static struct _tuple5*Cyc__genarr_978[
2]={& Cyc__gentuple_976,& Cyc__gentuple_977};static char _tmp21B[11]="Constraint";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep={5,{_tmp21B,
_tmp21B,_tmp21B + 11},{(void*)((struct _tuple7**)Cyc__genarr_971),(void*)((struct
_tuple7**)Cyc__genarr_971),(void*)((struct _tuple7**)Cyc__genarr_971 + 1)},{(void*)((
struct _tuple5**)Cyc__genarr_978),(void*)((struct _tuple5**)Cyc__genarr_978),(void*)((
struct _tuple5**)Cyc__genarr_978 + 2)}};static char _tmp21C[7]="Conref";static struct
_tagged_arr Cyc__genname_981={_tmp21C,_tmp21C,_tmp21C + 7};static char _tmp21D[2]="v";
static struct _tuple5 Cyc__gentuple_979={offsetof(struct Cyc_Absyn_Conref,v),{
_tmp21D,_tmp21D,_tmp21D + 2},(void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};
static struct _tuple5*Cyc__genarr_980[1]={& Cyc__gentuple_979};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep={3,(struct _tagged_arr*)& Cyc__genname_981,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_980),(void*)((
struct _tuple5**)Cyc__genarr_980),(void*)((struct _tuple5**)Cyc__genarr_980 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_969={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0bool2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1003;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep;static char _tmp220[7]="Conref";static
struct _tagged_arr Cyc__genname_1006={_tmp220,_tmp220,_tmp220 + 7};static char
_tmp221[2]="v";static struct _tuple5 Cyc__gentuple_1004={offsetof(struct Cyc_Absyn_Conref,v),{
_tmp221,_tmp221,_tmp221 + 2},(void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};
static struct _tuple5*Cyc__genarr_1005[1]={& Cyc__gentuple_1004};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_1006,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_1005),(void*)((
struct _tuple5**)Cyc__genarr_1005),(void*)((struct _tuple5**)Cyc__genarr_1005 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1003={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep)};
static char _tmp224[8]="PtrAtts";static struct _tagged_arr Cyc__genname_1012={_tmp224,
_tmp224,_tmp224 + 8};static char _tmp225[4]="rgn";static struct _tuple5 Cyc__gentuple_1007={
offsetof(struct Cyc_Absyn_PtrAtts,rgn),{_tmp225,_tmp225,_tmp225 + 4},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp226[9]="nullable";static struct _tuple5 Cyc__gentuple_1008={
offsetof(struct Cyc_Absyn_PtrAtts,nullable),{_tmp226,_tmp226,_tmp226 + 9},(void*)&
Cyc__genrep_969};static char _tmp227[7]="bounds";static struct _tuple5 Cyc__gentuple_1009={
offsetof(struct Cyc_Absyn_PtrAtts,bounds),{_tmp227,_tmp227,_tmp227 + 7},(void*)&
Cyc__genrep_1003};static char _tmp228[10]="zero_term";static struct _tuple5 Cyc__gentuple_1010={
offsetof(struct Cyc_Absyn_PtrAtts,zero_term),{_tmp228,_tmp228,_tmp228 + 10},(void*)&
Cyc__genrep_969};static struct _tuple5*Cyc__genarr_1011[4]={& Cyc__gentuple_1007,&
Cyc__gentuple_1008,& Cyc__gentuple_1009,& Cyc__gentuple_1010};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_ptr_atts_t_rep={3,(struct _tagged_arr*)& Cyc__genname_1012,sizeof(struct
Cyc_Absyn_PtrAtts),{(void*)((struct _tuple5**)Cyc__genarr_1011),(void*)((struct
_tuple5**)Cyc__genarr_1011),(void*)((struct _tuple5**)Cyc__genarr_1011 + 4)}};
static char _tmp22A[8]="PtrInfo";static struct _tagged_arr Cyc__genname_1017={_tmp22A,
_tmp22A,_tmp22A + 8};static char _tmp22B[8]="elt_typ";static struct _tuple5 Cyc__gentuple_1013={
offsetof(struct Cyc_Absyn_PtrInfo,elt_typ),{_tmp22B,_tmp22B,_tmp22B + 8},(void*)((
void*)& Cyc_Absyn_type_t_rep)};static char _tmp22C[7]="elt_tq";static struct _tuple5
Cyc__gentuple_1014={offsetof(struct Cyc_Absyn_PtrInfo,elt_tq),{_tmp22C,_tmp22C,
_tmp22C + 7},(void*)& Cyc__genrep_134};static char _tmp22D[9]="ptr_atts";static
struct _tuple5 Cyc__gentuple_1015={offsetof(struct Cyc_Absyn_PtrInfo,ptr_atts),{
_tmp22D,_tmp22D,_tmp22D + 9},(void*)& Cyc_Absyn_ptr_atts_t_rep};static struct
_tuple5*Cyc__genarr_1016[3]={& Cyc__gentuple_1013,& Cyc__gentuple_1014,& Cyc__gentuple_1015};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep={3,(struct _tagged_arr*)&
Cyc__genname_1017,sizeof(struct Cyc_Absyn_PtrInfo),{(void*)((struct _tuple5**)Cyc__genarr_1016),(
void*)((struct _tuple5**)Cyc__genarr_1016),(void*)((struct _tuple5**)Cyc__genarr_1016
+ 3)}};struct _tuple27{unsigned int f1;struct Cyc_Absyn_PtrInfo f2;};static struct
_tuple6 Cyc__gentuple_1018={offsetof(struct _tuple27,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_1019={offsetof(struct _tuple27,f2),(void*)& Cyc_Absyn_ptr_info_t_rep};
static struct _tuple6*Cyc__genarr_1020[2]={& Cyc__gentuple_1018,& Cyc__gentuple_1019};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1002={4,sizeof(struct _tuple27),{(
void*)((struct _tuple6**)Cyc__genarr_1020),(void*)((struct _tuple6**)Cyc__genarr_1020),(
void*)((struct _tuple6**)Cyc__genarr_1020 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_991;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep;static
char _tmp230[7]="Signed";static struct _tuple7 Cyc__gentuple_274={0,{_tmp230,_tmp230,
_tmp230 + 7}};static char _tmp231[9]="Unsigned";static struct _tuple7 Cyc__gentuple_275={
1,{_tmp231,_tmp231,_tmp231 + 9}};static char _tmp232[5]="None";static struct _tuple7
Cyc__gentuple_276={2,{_tmp232,_tmp232,_tmp232 + 5}};static struct _tuple7*Cyc__genarr_277[
3]={& Cyc__gentuple_274,& Cyc__gentuple_275,& Cyc__gentuple_276};static struct
_tuple5*Cyc__genarr_278[0]={};static char _tmp234[5]="Sign";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_sign_t_rep={5,{_tmp234,_tmp234,_tmp234 + 5},{(void*)((struct _tuple7**)
Cyc__genarr_277),(void*)((struct _tuple7**)Cyc__genarr_277),(void*)((struct
_tuple7**)Cyc__genarr_277 + 3)},{(void*)((struct _tuple5**)Cyc__genarr_278),(void*)((
struct _tuple5**)Cyc__genarr_278),(void*)((struct _tuple5**)Cyc__genarr_278 + 0)}};
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep;static char _tmp235[3]="B1";
static struct _tuple7 Cyc__gentuple_992={0,{_tmp235,_tmp235,_tmp235 + 3}};static char
_tmp236[3]="B2";static struct _tuple7 Cyc__gentuple_993={1,{_tmp236,_tmp236,_tmp236
+ 3}};static char _tmp237[3]="B4";static struct _tuple7 Cyc__gentuple_994={2,{_tmp237,
_tmp237,_tmp237 + 3}};static char _tmp238[3]="B8";static struct _tuple7 Cyc__gentuple_995={
3,{_tmp238,_tmp238,_tmp238 + 3}};static struct _tuple7*Cyc__genarr_996[4]={& Cyc__gentuple_992,&
Cyc__gentuple_993,& Cyc__gentuple_994,& Cyc__gentuple_995};static struct _tuple5*Cyc__genarr_997[
0]={};static char _tmp23A[8]="Size_of";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep={
5,{_tmp23A,_tmp23A,_tmp23A + 8},{(void*)((struct _tuple7**)Cyc__genarr_996),(void*)((
struct _tuple7**)Cyc__genarr_996),(void*)((struct _tuple7**)Cyc__genarr_996 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_997),(void*)((struct _tuple5**)Cyc__genarr_997),(
void*)((struct _tuple5**)Cyc__genarr_997 + 0)}};struct _tuple28{unsigned int f1;void*
f2;void*f3;};static struct _tuple6 Cyc__gentuple_998={offsetof(struct _tuple28,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_999={offsetof(struct
_tuple28,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_1000={
offsetof(struct _tuple28,f3),(void*)& Cyc_Absyn_size_of_t_rep};static struct _tuple6*
Cyc__genarr_1001[3]={& Cyc__gentuple_998,& Cyc__gentuple_999,& Cyc__gentuple_1000};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_991={4,sizeof(struct _tuple28),{(
void*)((struct _tuple6**)Cyc__genarr_1001),(void*)((struct _tuple6**)Cyc__genarr_1001),(
void*)((struct _tuple6**)Cyc__genarr_1001 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_968;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_array_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_78;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Exp_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_844;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_cnst_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_859;struct
_tuple29{unsigned int f1;void*f2;char f3;};static struct _tuple6 Cyc__gentuple_860={
offsetof(struct _tuple29,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_861={
offsetof(struct _tuple29,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_862={
offsetof(struct _tuple29,f3),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*
Cyc__genarr_863[3]={& Cyc__gentuple_860,& Cyc__gentuple_861,& Cyc__gentuple_862};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_859={4,sizeof(struct _tuple29),{(
void*)((struct _tuple6**)Cyc__genarr_863),(void*)((struct _tuple6**)Cyc__genarr_863),(
void*)((struct _tuple6**)Cyc__genarr_863 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_853;static struct Cyc_Typerep_Int_struct Cyc__genrep_854={0,1,16};
struct _tuple30{unsigned int f1;void*f2;short f3;};static struct _tuple6 Cyc__gentuple_855={
offsetof(struct _tuple30,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_856={
offsetof(struct _tuple30,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_857={
offsetof(struct _tuple30,f3),(void*)& Cyc__genrep_854};static struct _tuple6*Cyc__genarr_858[
3]={& Cyc__gentuple_855,& Cyc__gentuple_856,& Cyc__gentuple_857};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_853={4,sizeof(struct _tuple30),{(void*)((struct _tuple6**)Cyc__genarr_858),(
void*)((struct _tuple6**)Cyc__genarr_858),(void*)((struct _tuple6**)Cyc__genarr_858
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_273;struct _tuple31{
unsigned int f1;void*f2;int f3;};static struct _tuple6 Cyc__gentuple_279={offsetof(
struct _tuple31,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_280={
offsetof(struct _tuple31,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_281={
offsetof(struct _tuple31,f3),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6*
Cyc__genarr_282[3]={& Cyc__gentuple_279,& Cyc__gentuple_280,& Cyc__gentuple_281};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_273={4,sizeof(struct _tuple31),{(
void*)((struct _tuple6**)Cyc__genarr_282),(void*)((struct _tuple6**)Cyc__genarr_282),(
void*)((struct _tuple6**)Cyc__genarr_282 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_847;static struct Cyc_Typerep_Int_struct Cyc__genrep_848={0,1,64};
struct _tuple32{unsigned int f1;void*f2;long long f3;};static struct _tuple6 Cyc__gentuple_849={
offsetof(struct _tuple32,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_850={
offsetof(struct _tuple32,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_851={
offsetof(struct _tuple32,f3),(void*)& Cyc__genrep_848};static struct _tuple6*Cyc__genarr_852[
3]={& Cyc__gentuple_849,& Cyc__gentuple_850,& Cyc__gentuple_851};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_847={4,sizeof(struct _tuple32),{(void*)((struct _tuple6**)Cyc__genarr_852),(
void*)((struct _tuple6**)Cyc__genarr_852),(void*)((struct _tuple6**)Cyc__genarr_852
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_117;static struct _tuple6
Cyc__gentuple_118={offsetof(struct _tuple7,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_119={offsetof(struct _tuple7,f2),(void*)((void*)& Cyc__genrep_13)};
static struct _tuple6*Cyc__genarr_120[2]={& Cyc__gentuple_118,& Cyc__gentuple_119};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_117={4,sizeof(struct _tuple7),{(
void*)((struct _tuple6**)Cyc__genarr_120),(void*)((struct _tuple6**)Cyc__genarr_120),(
void*)((struct _tuple6**)Cyc__genarr_120 + 2)}};static char _tmp243[7]="Null_c";
static struct _tuple7 Cyc__gentuple_845={0,{_tmp243,_tmp243,_tmp243 + 7}};static
struct _tuple7*Cyc__genarr_846[1]={& Cyc__gentuple_845};static char _tmp244[7]="Char_c";
static struct _tuple5 Cyc__gentuple_864={0,{_tmp244,_tmp244,_tmp244 + 7},(void*)& Cyc__genrep_859};
static char _tmp245[8]="Short_c";static struct _tuple5 Cyc__gentuple_865={1,{_tmp245,
_tmp245,_tmp245 + 8},(void*)& Cyc__genrep_853};static char _tmp246[6]="Int_c";static
struct _tuple5 Cyc__gentuple_866={2,{_tmp246,_tmp246,_tmp246 + 6},(void*)& Cyc__genrep_273};
static char _tmp247[11]="LongLong_c";static struct _tuple5 Cyc__gentuple_867={3,{
_tmp247,_tmp247,_tmp247 + 11},(void*)& Cyc__genrep_847};static char _tmp248[8]="Float_c";
static struct _tuple5 Cyc__gentuple_868={4,{_tmp248,_tmp248,_tmp248 + 8},(void*)& Cyc__genrep_117};
static char _tmp249[9]="String_c";static struct _tuple5 Cyc__gentuple_869={5,{_tmp249,
_tmp249,_tmp249 + 9},(void*)& Cyc__genrep_117};static struct _tuple5*Cyc__genarr_870[
6]={& Cyc__gentuple_864,& Cyc__gentuple_865,& Cyc__gentuple_866,& Cyc__gentuple_867,&
Cyc__gentuple_868,& Cyc__gentuple_869};static char _tmp24B[5]="Cnst";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_cnst_t_rep={5,{_tmp24B,_tmp24B,_tmp24B + 5},{(void*)((struct _tuple7**)
Cyc__genarr_846),(void*)((struct _tuple7**)Cyc__genarr_846),(void*)((struct
_tuple7**)Cyc__genarr_846 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_870),(void*)((
struct _tuple5**)Cyc__genarr_870),(void*)((struct _tuple5**)Cyc__genarr_870 + 6)}};
static struct _tuple6 Cyc__gentuple_871={offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_872={offsetof(struct _tuple6,f2),(void*)& Cyc_Absyn_cnst_t_rep};
static struct _tuple6*Cyc__genarr_873[2]={& Cyc__gentuple_871,& Cyc__gentuple_872};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_844={4,sizeof(struct _tuple6),{(
void*)((struct _tuple6**)Cyc__genarr_873),(void*)((struct _tuple6**)Cyc__genarr_873),(
void*)((struct _tuple6**)Cyc__genarr_873 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_831;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_binding_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_86;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_87;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_597;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_598;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_599;struct _tuple33{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*
f3;};static struct _tuple6 Cyc__gentuple_600={offsetof(struct _tuple33,f1),(void*)&
Cyc__genrep_12};static struct _tuple6 Cyc__gentuple_601={offsetof(struct _tuple33,f2),(
void*)& Cyc__genrep_134};static struct _tuple6 Cyc__gentuple_602={offsetof(struct
_tuple33,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_603[
3]={& Cyc__gentuple_600,& Cyc__gentuple_601,& Cyc__gentuple_602};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_599={4,sizeof(struct _tuple33),{(void*)((struct _tuple6**)Cyc__genarr_603),(
void*)((struct _tuple6**)Cyc__genarr_603),(void*)((struct _tuple6**)Cyc__genarr_603
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_598={1,1,(void*)((void*)&
Cyc__genrep_599)};static char _tmp24F[5]="List";static struct _tagged_arr Cyc__genname_607={
_tmp24F,_tmp24F,_tmp24F + 5};static char _tmp250[3]="hd";static struct _tuple5 Cyc__gentuple_604={
offsetof(struct Cyc_List_List,hd),{_tmp250,_tmp250,_tmp250 + 3},(void*)& Cyc__genrep_598};
static char _tmp251[3]="tl";static struct _tuple5 Cyc__gentuple_605={offsetof(struct
Cyc_List_List,tl),{_tmp251,_tmp251,_tmp251 + 3},(void*)& Cyc__genrep_597};static
struct _tuple5*Cyc__genarr_606[2]={& Cyc__gentuple_604,& Cyc__gentuple_605};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_607,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_606),(void*)((struct _tuple5**)Cyc__genarr_606),(void*)((
struct _tuple5**)Cyc__genarr_606 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_597={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_586;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_587;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep;static
char _tmp254[4]="Opt";static struct _tagged_arr Cyc__genname_590={_tmp254,_tmp254,
_tmp254 + 4};static char _tmp255[2]="v";static struct _tuple5 Cyc__gentuple_588={
offsetof(struct Cyc_Core_Opt,v),{_tmp255,_tmp255,_tmp255 + 2},(void*)& Cyc__genrep_12};
static struct _tuple5*Cyc__genarr_589[1]={& Cyc__gentuple_588};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_var_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_590,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_589),(void*)((
struct _tuple5**)Cyc__genarr_589),(void*)((struct _tuple5**)Cyc__genarr_589 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_587={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_var_t2_rep)};
static char _tmp258[11]="VarargInfo";static struct _tagged_arr Cyc__genname_596={
_tmp258,_tmp258,_tmp258 + 11};static char _tmp259[5]="name";static struct _tuple5 Cyc__gentuple_591={
offsetof(struct Cyc_Absyn_VarargInfo,name),{_tmp259,_tmp259,_tmp259 + 5},(void*)&
Cyc__genrep_587};static char _tmp25A[3]="tq";static struct _tuple5 Cyc__gentuple_592={
offsetof(struct Cyc_Absyn_VarargInfo,tq),{_tmp25A,_tmp25A,_tmp25A + 3},(void*)& Cyc__genrep_134};
static char _tmp25B[5]="type";static struct _tuple5 Cyc__gentuple_593={offsetof(
struct Cyc_Absyn_VarargInfo,type),{_tmp25B,_tmp25B,_tmp25B + 5},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp25C[7]="inject";static struct _tuple5 Cyc__gentuple_594={
offsetof(struct Cyc_Absyn_VarargInfo,inject),{_tmp25C,_tmp25C,_tmp25C + 7},(void*)((
void*)& Cyc__genrep_62)};static struct _tuple5*Cyc__genarr_595[4]={& Cyc__gentuple_591,&
Cyc__gentuple_592,& Cyc__gentuple_593,& Cyc__gentuple_594};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep={3,(struct _tagged_arr*)& Cyc__genname_596,sizeof(
struct Cyc_Absyn_VarargInfo),{(void*)((struct _tuple5**)Cyc__genarr_595),(void*)((
struct _tuple5**)Cyc__genarr_595),(void*)((struct _tuple5**)Cyc__genarr_595 + 4)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_586={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_370;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_371;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_372;static struct
_tuple6 Cyc__gentuple_373={offsetof(struct _tuple9,f1),(void*)((void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple6 Cyc__gentuple_374={offsetof(struct _tuple9,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_375[2]={& Cyc__gentuple_373,&
Cyc__gentuple_374};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_372={4,
sizeof(struct _tuple9),{(void*)((struct _tuple6**)Cyc__genarr_375),(void*)((struct
_tuple6**)Cyc__genarr_375),(void*)((struct _tuple6**)Cyc__genarr_375 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_371={1,1,(void*)((void*)& Cyc__genrep_372)};
static char _tmp261[5]="List";static struct _tagged_arr Cyc__genname_379={_tmp261,
_tmp261,_tmp261 + 5};static char _tmp262[3]="hd";static struct _tuple5 Cyc__gentuple_376={
offsetof(struct Cyc_List_List,hd),{_tmp262,_tmp262,_tmp262 + 3},(void*)& Cyc__genrep_371};
static char _tmp263[3]="tl";static struct _tuple5 Cyc__gentuple_377={offsetof(struct
Cyc_List_List,tl),{_tmp263,_tmp263,_tmp263 + 3},(void*)& Cyc__genrep_370};static
struct _tuple5*Cyc__genarr_378[2]={& Cyc__gentuple_376,& Cyc__gentuple_377};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_379,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_378),(void*)((struct _tuple5**)Cyc__genarr_378),(void*)((
struct _tuple5**)Cyc__genarr_378 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_370={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_162;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_81;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_82;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_82={1,1,(void*)((
void*)& Cyc_struct_Absyn_Exp_rep)};struct _tuple34{unsigned int f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple6 Cyc__gentuple_83={offsetof(struct _tuple34,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_84={offsetof(struct _tuple34,f2),(
void*)& Cyc__genrep_82};static struct _tuple6*Cyc__genarr_85[2]={& Cyc__gentuple_83,&
Cyc__gentuple_84};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_81={4,sizeof(
struct _tuple34),{(void*)((struct _tuple6**)Cyc__genarr_85),(void*)((struct _tuple6**)
Cyc__genarr_85),(void*)((struct _tuple6**)Cyc__genarr_85 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_552;struct _tuple35{unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*
f3;};static struct _tuple6 Cyc__gentuple_553={offsetof(struct _tuple35,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_554={offsetof(struct _tuple35,f2),(
void*)& Cyc__genrep_162};static struct _tuple6 Cyc__gentuple_555={offsetof(struct
_tuple35,f3),(void*)& Cyc__genrep_162};static struct _tuple6*Cyc__genarr_556[3]={&
Cyc__gentuple_553,& Cyc__gentuple_554,& Cyc__gentuple_555};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_552={4,sizeof(struct _tuple35),{(void*)((struct _tuple6**)Cyc__genarr_556),(
void*)((struct _tuple6**)Cyc__genarr_556),(void*)((struct _tuple6**)Cyc__genarr_556
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_548;static struct _tuple6
Cyc__gentuple_549={offsetof(struct _tuple34,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_550={offsetof(struct _tuple34,f2),(void*)& Cyc__genrep_78};
static struct _tuple6*Cyc__genarr_551[2]={& Cyc__gentuple_549,& Cyc__gentuple_550};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_548={4,sizeof(struct _tuple34),{(
void*)((struct _tuple6**)Cyc__genarr_551),(void*)((struct _tuple6**)Cyc__genarr_551),(
void*)((struct _tuple6**)Cyc__genarr_551 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_542;struct _tuple36{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Stmt*
f3;struct Cyc_Absyn_Stmt*f4;};static struct _tuple6 Cyc__gentuple_543={offsetof(
struct _tuple36,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_544={
offsetof(struct _tuple36,f2),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_545={
offsetof(struct _tuple36,f3),(void*)& Cyc__genrep_162};static struct _tuple6 Cyc__gentuple_546={
offsetof(struct _tuple36,f4),(void*)& Cyc__genrep_162};static struct _tuple6*Cyc__genarr_547[
4]={& Cyc__gentuple_543,& Cyc__gentuple_544,& Cyc__gentuple_545,& Cyc__gentuple_546};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_542={4,sizeof(struct _tuple36),{(
void*)((struct _tuple6**)Cyc__genarr_547),(void*)((struct _tuple6**)Cyc__genarr_547),(
void*)((struct _tuple6**)Cyc__genarr_547 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_537;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_171;static struct
_tuple6 Cyc__gentuple_172={offsetof(struct _tuple2,f1),(void*)& Cyc__genrep_82};
static struct _tuple6 Cyc__gentuple_173={offsetof(struct _tuple2,f2),(void*)& Cyc__genrep_162};
static struct _tuple6*Cyc__genarr_174[2]={& Cyc__gentuple_172,& Cyc__gentuple_173};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_171={4,sizeof(struct _tuple2),{(
void*)((struct _tuple6**)Cyc__genarr_174),(void*)((struct _tuple6**)Cyc__genarr_174),(
void*)((struct _tuple6**)Cyc__genarr_174 + 2)}};struct _tuple37{unsigned int f1;
struct _tuple2 f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_538={
offsetof(struct _tuple37,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_539={
offsetof(struct _tuple37,f2),(void*)& Cyc__genrep_171};static struct _tuple6 Cyc__gentuple_540={
offsetof(struct _tuple37,f3),(void*)& Cyc__genrep_162};static struct _tuple6*Cyc__genarr_541[
3]={& Cyc__gentuple_538,& Cyc__gentuple_539,& Cyc__gentuple_540};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_537={4,sizeof(struct _tuple37),{(void*)((struct _tuple6**)Cyc__genarr_541),(
void*)((struct _tuple6**)Cyc__genarr_541),(void*)((struct _tuple6**)Cyc__genarr_541
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_533;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_528;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_528={1,1,(void*)((
void*)& Cyc_struct_Absyn_Stmt_rep)};struct _tuple38{unsigned int f1;struct Cyc_Absyn_Stmt*
f2;};static struct _tuple6 Cyc__gentuple_534={offsetof(struct _tuple38,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_535={offsetof(struct _tuple38,f2),(
void*)& Cyc__genrep_528};static struct _tuple6*Cyc__genarr_536[2]={& Cyc__gentuple_534,&
Cyc__gentuple_535};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_533={4,
sizeof(struct _tuple38),{(void*)((struct _tuple6**)Cyc__genarr_536),(void*)((
struct _tuple6**)Cyc__genarr_536),(void*)((struct _tuple6**)Cyc__genarr_536 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_527;struct _tuple39{unsigned int
f1;struct _tagged_arr*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_529={
offsetof(struct _tuple39,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_530={
offsetof(struct _tuple39,f2),(void*)& Cyc__genrep_12};static struct _tuple6 Cyc__gentuple_531={
offsetof(struct _tuple39,f3),(void*)& Cyc__genrep_528};static struct _tuple6*Cyc__genarr_532[
3]={& Cyc__gentuple_529,& Cyc__gentuple_530,& Cyc__gentuple_531};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_527={4,sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_532),(
void*)((struct _tuple6**)Cyc__genarr_532),(void*)((struct _tuple6**)Cyc__genarr_532
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_520;struct _tuple40{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct _tuple2 f3;struct _tuple2 f4;struct Cyc_Absyn_Stmt*
f5;};static struct _tuple6 Cyc__gentuple_521={offsetof(struct _tuple40,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_522={offsetof(struct _tuple40,f2),(
void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_523={offsetof(struct
_tuple40,f3),(void*)& Cyc__genrep_171};static struct _tuple6 Cyc__gentuple_524={
offsetof(struct _tuple40,f4),(void*)& Cyc__genrep_171};static struct _tuple6 Cyc__gentuple_525={
offsetof(struct _tuple40,f5),(void*)& Cyc__genrep_162};static struct _tuple6*Cyc__genarr_526[
5]={& Cyc__gentuple_521,& Cyc__gentuple_522,& Cyc__gentuple_523,& Cyc__gentuple_524,&
Cyc__gentuple_525};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_520={4,
sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_526),(void*)((
struct _tuple6**)Cyc__genarr_526),(void*)((struct _tuple6**)Cyc__genarr_526 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_515;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_229;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_230;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Switch_clause_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_231;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_425;
struct _tuple41{unsigned int f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;
};static struct _tuple6 Cyc__gentuple_426={offsetof(struct _tuple41,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_427={offsetof(struct _tuple41,f2),(void*)& Cyc__genrep_185};
static struct _tuple6 Cyc__gentuple_428={offsetof(struct _tuple41,f3),(void*)& Cyc__genrep_133};
static struct _tuple6*Cyc__genarr_429[3]={& Cyc__gentuple_426,& Cyc__gentuple_427,&
Cyc__gentuple_428};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_425={4,
sizeof(struct _tuple41),{(void*)((struct _tuple6**)Cyc__genarr_429),(void*)((
struct _tuple6**)Cyc__genarr_429),(void*)((struct _tuple6**)Cyc__genarr_429 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_421;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_236;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep;
static char _tmp272[5]="List";static struct _tagged_arr Cyc__genname_240={_tmp272,
_tmp272,_tmp272 + 5};static char _tmp273[3]="hd";static struct _tuple5 Cyc__gentuple_237={
offsetof(struct Cyc_List_List,hd),{_tmp273,_tmp273,_tmp273 + 3},(void*)& Cyc__genrep_231};
static char _tmp274[3]="tl";static struct _tuple5 Cyc__gentuple_238={offsetof(struct
Cyc_List_List,tl),{_tmp274,_tmp274,_tmp274 + 3},(void*)& Cyc__genrep_236};static
struct _tuple5*Cyc__genarr_239[2]={& Cyc__gentuple_237,& Cyc__gentuple_238};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep={3,(struct
_tagged_arr*)& Cyc__genname_240,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_239),(void*)((struct _tuple5**)Cyc__genarr_239),(void*)((
struct _tuple5**)Cyc__genarr_239 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_236={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_pat_t46H2_rep)};static struct
_tuple6 Cyc__gentuple_422={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_423={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_236};
static struct _tuple6*Cyc__genarr_424[2]={& Cyc__gentuple_422,& Cyc__gentuple_423};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_421={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_424),(void*)((struct _tuple6**)Cyc__genarr_424),(
void*)((struct _tuple6**)Cyc__genarr_424 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_417;struct _tuple42{unsigned int f1;struct Cyc_Absyn_Pat*f2;};static
struct _tuple6 Cyc__gentuple_418={offsetof(struct _tuple42,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_419={offsetof(struct _tuple42,f2),(void*)& Cyc__genrep_231};
static struct _tuple6*Cyc__genarr_420[2]={& Cyc__gentuple_418,& Cyc__gentuple_419};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_417={4,sizeof(struct _tuple42),{(
void*)((struct _tuple6**)Cyc__genarr_420),(void*)((struct _tuple6**)Cyc__genarr_420),(
void*)((struct _tuple6**)Cyc__genarr_420 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_328;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep;extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_400;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep;
static char _tmp279[8]="StructA";static struct _tuple7 Cyc__gentuple_385={0,{_tmp279,
_tmp279,_tmp279 + 8}};static char _tmp27A[7]="UnionA";static struct _tuple7 Cyc__gentuple_386={
1,{_tmp27A,_tmp27A,_tmp27A + 7}};static struct _tuple7*Cyc__genarr_387[2]={& Cyc__gentuple_385,&
Cyc__gentuple_386};static struct _tuple5*Cyc__genarr_388[0]={};static char _tmp27C[9]="AggrKind";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep={5,{_tmp27C,_tmp27C,
_tmp27C + 9},{(void*)((struct _tuple7**)Cyc__genarr_387),(void*)((struct _tuple7**)
Cyc__genarr_387),(void*)((struct _tuple7**)Cyc__genarr_387 + 2)},{(void*)((struct
_tuple5**)Cyc__genarr_388),(void*)((struct _tuple5**)Cyc__genarr_388),(void*)((
struct _tuple5**)Cyc__genarr_388 + 0)}};struct _tuple43{unsigned int f1;void*f2;
struct _tuple0*f3;};static struct _tuple6 Cyc__gentuple_401={offsetof(struct _tuple43,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_402={offsetof(struct
_tuple43,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};static struct _tuple6 Cyc__gentuple_403={
offsetof(struct _tuple43,f3),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_404[
3]={& Cyc__gentuple_401,& Cyc__gentuple_402,& Cyc__gentuple_403};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_400={4,sizeof(struct _tuple43),{(void*)((struct _tuple6**)Cyc__genarr_404),(
void*)((struct _tuple6**)Cyc__genarr_404),(void*)((struct _tuple6**)Cyc__genarr_404
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_353;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_354;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_355;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_356;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_357;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_358;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_88;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_attribute_t46H2_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_attribute_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_107;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep;static char
_tmp27E[10]="Printf_ft";static struct _tuple7 Cyc__gentuple_108={0,{_tmp27E,_tmp27E,
_tmp27E + 10}};static char _tmp27F[9]="Scanf_ft";static struct _tuple7 Cyc__gentuple_109={
1,{_tmp27F,_tmp27F,_tmp27F + 9}};static struct _tuple7*Cyc__genarr_110[2]={& Cyc__gentuple_108,&
Cyc__gentuple_109};static struct _tuple5*Cyc__genarr_111[0]={};static char _tmp281[
12]="Format_Type";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep={
5,{_tmp281,_tmp281,_tmp281 + 12},{(void*)((struct _tuple7**)Cyc__genarr_110),(void*)((
struct _tuple7**)Cyc__genarr_110),(void*)((struct _tuple7**)Cyc__genarr_110 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_111),(void*)((struct _tuple5**)Cyc__genarr_111),(
void*)((struct _tuple5**)Cyc__genarr_111 + 0)}};struct _tuple44{unsigned int f1;void*
f2;int f3;int f4;};static struct _tuple6 Cyc__gentuple_112={offsetof(struct _tuple44,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_113={offsetof(struct
_tuple44,f2),(void*)& Cyc_tunion_Absyn_Format_Type_rep};static struct _tuple6 Cyc__gentuple_114={
offsetof(struct _tuple44,f3),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6
Cyc__gentuple_115={offsetof(struct _tuple44,f4),(void*)((void*)& Cyc__genrep_62)};
static struct _tuple6*Cyc__genarr_116[4]={& Cyc__gentuple_112,& Cyc__gentuple_113,&
Cyc__gentuple_114,& Cyc__gentuple_115};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_107={
4,sizeof(struct _tuple44),{(void*)((struct _tuple6**)Cyc__genarr_116),(void*)((
struct _tuple6**)Cyc__genarr_116),(void*)((struct _tuple6**)Cyc__genarr_116 + 4)}};
static char _tmp283[12]="Stdcall_att";static struct _tuple7 Cyc__gentuple_89={0,{
_tmp283,_tmp283,_tmp283 + 12}};static char _tmp284[10]="Cdecl_att";static struct
_tuple7 Cyc__gentuple_90={1,{_tmp284,_tmp284,_tmp284 + 10}};static char _tmp285[13]="Fastcall_att";
static struct _tuple7 Cyc__gentuple_91={2,{_tmp285,_tmp285,_tmp285 + 13}};static char
_tmp286[13]="Noreturn_att";static struct _tuple7 Cyc__gentuple_92={3,{_tmp286,
_tmp286,_tmp286 + 13}};static char _tmp287[10]="Const_att";static struct _tuple7 Cyc__gentuple_93={
4,{_tmp287,_tmp287,_tmp287 + 10}};static char _tmp288[11]="Packed_att";static struct
_tuple7 Cyc__gentuple_94={5,{_tmp288,_tmp288,_tmp288 + 11}};static char _tmp289[13]="Nocommon_att";
static struct _tuple7 Cyc__gentuple_95={6,{_tmp289,_tmp289,_tmp289 + 13}};static char
_tmp28A[11]="Shared_att";static struct _tuple7 Cyc__gentuple_96={7,{_tmp28A,_tmp28A,
_tmp28A + 11}};static char _tmp28B[11]="Unused_att";static struct _tuple7 Cyc__gentuple_97={
8,{_tmp28B,_tmp28B,_tmp28B + 11}};static char _tmp28C[9]="Weak_att";static struct
_tuple7 Cyc__gentuple_98={9,{_tmp28C,_tmp28C,_tmp28C + 9}};static char _tmp28D[14]="Dllimport_att";
static struct _tuple7 Cyc__gentuple_99={10,{_tmp28D,_tmp28D,_tmp28D + 14}};static
char _tmp28E[14]="Dllexport_att";static struct _tuple7 Cyc__gentuple_100={11,{
_tmp28E,_tmp28E,_tmp28E + 14}};static char _tmp28F[27]="No_instrument_function_att";
static struct _tuple7 Cyc__gentuple_101={12,{_tmp28F,_tmp28F,_tmp28F + 27}};static
char _tmp290[16]="Constructor_att";static struct _tuple7 Cyc__gentuple_102={13,{
_tmp290,_tmp290,_tmp290 + 16}};static char _tmp291[15]="Destructor_att";static
struct _tuple7 Cyc__gentuple_103={14,{_tmp291,_tmp291,_tmp291 + 15}};static char
_tmp292[26]="No_check_memory_usage_att";static struct _tuple7 Cyc__gentuple_104={15,{
_tmp292,_tmp292,_tmp292 + 26}};static char _tmp293[9]="Pure_att";static struct
_tuple7 Cyc__gentuple_105={16,{_tmp293,_tmp293,_tmp293 + 9}};static struct _tuple7*
Cyc__genarr_106[17]={& Cyc__gentuple_89,& Cyc__gentuple_90,& Cyc__gentuple_91,& Cyc__gentuple_92,&
Cyc__gentuple_93,& Cyc__gentuple_94,& Cyc__gentuple_95,& Cyc__gentuple_96,& Cyc__gentuple_97,&
Cyc__gentuple_98,& Cyc__gentuple_99,& Cyc__gentuple_100,& Cyc__gentuple_101,& Cyc__gentuple_102,&
Cyc__gentuple_103,& Cyc__gentuple_104,& Cyc__gentuple_105};static char _tmp294[12]="Regparm_att";
static struct _tuple5 Cyc__gentuple_121={0,{_tmp294,_tmp294,_tmp294 + 12},(void*)&
Cyc__genrep_61};static char _tmp295[12]="Aligned_att";static struct _tuple5 Cyc__gentuple_122={
1,{_tmp295,_tmp295,_tmp295 + 12},(void*)& Cyc__genrep_61};static char _tmp296[12]="Section_att";
static struct _tuple5 Cyc__gentuple_123={2,{_tmp296,_tmp296,_tmp296 + 12},(void*)&
Cyc__genrep_117};static char _tmp297[11]="Format_att";static struct _tuple5 Cyc__gentuple_124={
3,{_tmp297,_tmp297,_tmp297 + 11},(void*)& Cyc__genrep_107};static char _tmp298[16]="Initializes_att";
static struct _tuple5 Cyc__gentuple_125={4,{_tmp298,_tmp298,_tmp298 + 16},(void*)&
Cyc__genrep_61};static struct _tuple5*Cyc__genarr_126[5]={& Cyc__gentuple_121,& Cyc__gentuple_122,&
Cyc__gentuple_123,& Cyc__gentuple_124,& Cyc__gentuple_125};static char _tmp29A[10]="Attribute";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep={5,{_tmp29A,_tmp29A,
_tmp29A + 10},{(void*)((struct _tuple7**)Cyc__genarr_106),(void*)((struct _tuple7**)
Cyc__genarr_106),(void*)((struct _tuple7**)Cyc__genarr_106 + 17)},{(void*)((struct
_tuple5**)Cyc__genarr_126),(void*)((struct _tuple5**)Cyc__genarr_126),(void*)((
struct _tuple5**)Cyc__genarr_126 + 5)}};static char _tmp29B[5]="List";static struct
_tagged_arr Cyc__genname_130={_tmp29B,_tmp29B,_tmp29B + 5};static char _tmp29C[3]="hd";
static struct _tuple5 Cyc__gentuple_127={offsetof(struct Cyc_List_List,hd),{_tmp29C,
_tmp29C,_tmp29C + 3},(void*)& Cyc_Absyn_attribute_t_rep};static char _tmp29D[3]="tl";
static struct _tuple5 Cyc__gentuple_128={offsetof(struct Cyc_List_List,tl),{_tmp29D,
_tmp29D,_tmp29D + 3},(void*)& Cyc__genrep_88};static struct _tuple5*Cyc__genarr_129[
2]={& Cyc__gentuple_127,& Cyc__gentuple_128};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_130,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_129),(void*)((struct _tuple5**)Cyc__genarr_129),(void*)((
struct _tuple5**)Cyc__genarr_129 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_88={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_attribute_t46H2_rep)};static char
_tmp2A0[10]="Aggrfield";static struct _tagged_arr Cyc__genname_365={_tmp2A0,_tmp2A0,
_tmp2A0 + 10};static char _tmp2A1[5]="name";static struct _tuple5 Cyc__gentuple_359={
offsetof(struct Cyc_Absyn_Aggrfield,name),{_tmp2A1,_tmp2A1,_tmp2A1 + 5},(void*)&
Cyc__genrep_12};static char _tmp2A2[3]="tq";static struct _tuple5 Cyc__gentuple_360={
offsetof(struct Cyc_Absyn_Aggrfield,tq),{_tmp2A2,_tmp2A2,_tmp2A2 + 3},(void*)& Cyc__genrep_134};
static char _tmp2A3[5]="type";static struct _tuple5 Cyc__gentuple_361={offsetof(
struct Cyc_Absyn_Aggrfield,type),{_tmp2A3,_tmp2A3,_tmp2A3 + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp2A4[6]="width";static struct _tuple5 Cyc__gentuple_362={offsetof(
struct Cyc_Absyn_Aggrfield,width),{_tmp2A4,_tmp2A4,_tmp2A4 + 6},(void*)& Cyc__genrep_78};
static char _tmp2A5[11]="attributes";static struct _tuple5 Cyc__gentuple_363={
offsetof(struct Cyc_Absyn_Aggrfield,attributes),{_tmp2A5,_tmp2A5,_tmp2A5 + 11},(
void*)& Cyc__genrep_88};static struct _tuple5*Cyc__genarr_364[5]={& Cyc__gentuple_359,&
Cyc__gentuple_360,& Cyc__gentuple_361,& Cyc__gentuple_362,& Cyc__gentuple_363};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep={3,(struct
_tagged_arr*)& Cyc__genname_365,sizeof(struct Cyc_Absyn_Aggrfield),{(void*)((
struct _tuple5**)Cyc__genarr_364),(void*)((struct _tuple5**)Cyc__genarr_364),(void*)((
struct _tuple5**)Cyc__genarr_364 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_358={
1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrfield_rep)};static char _tmp2A8[5]="List";
static struct _tagged_arr Cyc__genname_369={_tmp2A8,_tmp2A8,_tmp2A8 + 5};static char
_tmp2A9[3]="hd";static struct _tuple5 Cyc__gentuple_366={offsetof(struct Cyc_List_List,hd),{
_tmp2A9,_tmp2A9,_tmp2A9 + 3},(void*)& Cyc__genrep_358};static char _tmp2AA[3]="tl";
static struct _tuple5 Cyc__gentuple_367={offsetof(struct Cyc_List_List,tl),{_tmp2AA,
_tmp2AA,_tmp2AA + 3},(void*)& Cyc__genrep_357};static struct _tuple5*Cyc__genarr_368[
2]={& Cyc__gentuple_366,& Cyc__gentuple_367};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_369,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_368),(void*)((struct _tuple5**)Cyc__genarr_368),(void*)((
struct _tuple5**)Cyc__genarr_368 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_357={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep)};static char
_tmp2AD[13]="AggrdeclImpl";static struct _tagged_arr Cyc__genname_384={_tmp2AD,
_tmp2AD,_tmp2AD + 13};static char _tmp2AE[11]="exist_vars";static struct _tuple5 Cyc__gentuple_380={
offsetof(struct Cyc_Absyn_AggrdeclImpl,exist_vars),{_tmp2AE,_tmp2AE,_tmp2AE + 11},(
void*)& Cyc__genrep_311};static char _tmp2AF[7]="rgn_po";static struct _tuple5 Cyc__gentuple_381={
offsetof(struct Cyc_Absyn_AggrdeclImpl,rgn_po),{_tmp2AF,_tmp2AF,_tmp2AF + 7},(void*)&
Cyc__genrep_370};static char _tmp2B0[7]="fields";static struct _tuple5 Cyc__gentuple_382={
offsetof(struct Cyc_Absyn_AggrdeclImpl,fields),{_tmp2B0,_tmp2B0,_tmp2B0 + 7},(void*)&
Cyc__genrep_357};static struct _tuple5*Cyc__genarr_383[3]={& Cyc__gentuple_380,& Cyc__gentuple_381,&
Cyc__gentuple_382};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep={
3,(struct _tagged_arr*)& Cyc__genname_384,sizeof(struct Cyc_Absyn_AggrdeclImpl),{(
void*)((struct _tuple5**)Cyc__genarr_383),(void*)((struct _tuple5**)Cyc__genarr_383),(
void*)((struct _tuple5**)Cyc__genarr_383 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_356={1,1,(void*)((void*)& Cyc_struct_Absyn_AggrdeclImpl_rep)};static
char _tmp2B3[9]="Aggrdecl";static struct _tagged_arr Cyc__genname_396={_tmp2B3,
_tmp2B3,_tmp2B3 + 9};static char _tmp2B4[5]="kind";static struct _tuple5 Cyc__gentuple_389={
offsetof(struct Cyc_Absyn_Aggrdecl,kind),{_tmp2B4,_tmp2B4,_tmp2B4 + 5},(void*)& Cyc_Absyn_aggr_kind_t_rep};
static char _tmp2B5[3]="sc";static struct _tuple5 Cyc__gentuple_390={offsetof(struct
Cyc_Absyn_Aggrdecl,sc),{_tmp2B5,_tmp2B5,_tmp2B5 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp2B6[5]="name";static struct _tuple5 Cyc__gentuple_391={offsetof(
struct Cyc_Absyn_Aggrdecl,name),{_tmp2B6,_tmp2B6,_tmp2B6 + 5},(void*)& Cyc__genrep_10};
static char _tmp2B7[4]="tvs";static struct _tuple5 Cyc__gentuple_392={offsetof(struct
Cyc_Absyn_Aggrdecl,tvs),{_tmp2B7,_tmp2B7,_tmp2B7 + 4},(void*)& Cyc__genrep_311};
static char _tmp2B8[5]="impl";static struct _tuple5 Cyc__gentuple_393={offsetof(
struct Cyc_Absyn_Aggrdecl,impl),{_tmp2B8,_tmp2B8,_tmp2B8 + 5},(void*)& Cyc__genrep_356};
static char _tmp2B9[11]="attributes";static struct _tuple5 Cyc__gentuple_394={
offsetof(struct Cyc_Absyn_Aggrdecl,attributes),{_tmp2B9,_tmp2B9,_tmp2B9 + 11},(
void*)& Cyc__genrep_88};static struct _tuple5*Cyc__genarr_395[6]={& Cyc__gentuple_389,&
Cyc__gentuple_390,& Cyc__gentuple_391,& Cyc__gentuple_392,& Cyc__gentuple_393,& Cyc__gentuple_394};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep={3,(struct
_tagged_arr*)& Cyc__genname_396,sizeof(struct Cyc_Absyn_Aggrdecl),{(void*)((struct
_tuple5**)Cyc__genarr_395),(void*)((struct _tuple5**)Cyc__genarr_395),(void*)((
struct _tuple5**)Cyc__genarr_395 + 6)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_355={
1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_354={1,1,(void*)((void*)& Cyc__genrep_355)};struct _tuple45{
unsigned int f1;struct Cyc_Absyn_Aggrdecl**f2;};static struct _tuple6 Cyc__gentuple_397={
offsetof(struct _tuple45,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_398={
offsetof(struct _tuple45,f2),(void*)& Cyc__genrep_354};static struct _tuple6*Cyc__genarr_399[
2]={& Cyc__gentuple_397,& Cyc__gentuple_398};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_353={4,sizeof(struct _tuple45),{(void*)((struct _tuple6**)Cyc__genarr_399),(
void*)((struct _tuple6**)Cyc__genarr_399),(void*)((struct _tuple6**)Cyc__genarr_399
+ 2)}};static struct _tuple7*Cyc__genarr_352[0]={};static char _tmp2BE[12]="UnknownAggr";
static struct _tuple5 Cyc__gentuple_405={0,{_tmp2BE,_tmp2BE,_tmp2BE + 12},(void*)&
Cyc__genrep_400};static char _tmp2BF[10]="KnownAggr";static struct _tuple5 Cyc__gentuple_406={
1,{_tmp2BF,_tmp2BF,_tmp2BF + 10},(void*)& Cyc__genrep_353};static struct _tuple5*Cyc__genarr_407[
2]={& Cyc__gentuple_405,& Cyc__gentuple_406};static char _tmp2C1[10]="AggrInfoU";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep={5,{_tmp2C1,
_tmp2C1,_tmp2C1 + 10},{(void*)((struct _tuple7**)Cyc__genarr_352),(void*)((struct
_tuple7**)Cyc__genarr_352),(void*)((struct _tuple7**)Cyc__genarr_352 + 0)},{(void*)((
struct _tuple5**)Cyc__genarr_407),(void*)((struct _tuple5**)Cyc__genarr_407),(void*)((
struct _tuple5**)Cyc__genarr_407 + 2)}};static char _tmp2C2[9]="AggrInfo";static
struct _tagged_arr Cyc__genname_411={_tmp2C2,_tmp2C2,_tmp2C2 + 9};static char _tmp2C3[
10]="aggr_info";static struct _tuple5 Cyc__gentuple_408={offsetof(struct Cyc_Absyn_AggrInfo,aggr_info),{
_tmp2C3,_tmp2C3,_tmp2C3 + 10},(void*)& Cyc_tunion_Absyn_AggrInfoU_rep};static char
_tmp2C4[6]="targs";static struct _tuple5 Cyc__gentuple_409={offsetof(struct Cyc_Absyn_AggrInfo,targs),{
_tmp2C4,_tmp2C4,_tmp2C4 + 6},(void*)& Cyc__genrep_53};static struct _tuple5*Cyc__genarr_410[
2]={& Cyc__gentuple_408,& Cyc__gentuple_409};struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_411,sizeof(struct Cyc_Absyn_AggrInfo),{(void*)((
struct _tuple5**)Cyc__genarr_410),(void*)((struct _tuple5**)Cyc__genarr_410),(void*)((
struct _tuple5**)Cyc__genarr_410 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_329;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_330;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_331;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_332;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_334;struct _tuple46{unsigned int f1;struct _tagged_arr*f2;};static
struct _tuple6 Cyc__gentuple_335={offsetof(struct _tuple46,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_336={offsetof(struct _tuple46,f2),(void*)& Cyc__genrep_12};
static struct _tuple6*Cyc__genarr_337[2]={& Cyc__gentuple_335,& Cyc__gentuple_336};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_334={4,sizeof(struct _tuple46),{(
void*)((struct _tuple6**)Cyc__genarr_337),(void*)((struct _tuple6**)Cyc__genarr_337),(
void*)((struct _tuple6**)Cyc__genarr_337 + 2)}};static struct _tuple7*Cyc__genarr_333[
0]={};static char _tmp2C7[13]="ArrayElement";static struct _tuple5 Cyc__gentuple_338={
0,{_tmp2C7,_tmp2C7,_tmp2C7 + 13},(void*)& Cyc__genrep_81};static char _tmp2C8[10]="FieldName";
static struct _tuple5 Cyc__gentuple_339={1,{_tmp2C8,_tmp2C8,_tmp2C8 + 10},(void*)&
Cyc__genrep_334};static struct _tuple5*Cyc__genarr_340[2]={& Cyc__gentuple_338,& Cyc__gentuple_339};
static char _tmp2CA[11]="Designator";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep={
5,{_tmp2CA,_tmp2CA,_tmp2CA + 11},{(void*)((struct _tuple7**)Cyc__genarr_333),(void*)((
struct _tuple7**)Cyc__genarr_333),(void*)((struct _tuple7**)Cyc__genarr_333 + 0)},{(
void*)((struct _tuple5**)Cyc__genarr_340),(void*)((struct _tuple5**)Cyc__genarr_340),(
void*)((struct _tuple5**)Cyc__genarr_340 + 2)}};static char _tmp2CB[5]="List";static
struct _tagged_arr Cyc__genname_344={_tmp2CB,_tmp2CB,_tmp2CB + 5};static char _tmp2CC[
3]="hd";static struct _tuple5 Cyc__gentuple_341={offsetof(struct Cyc_List_List,hd),{
_tmp2CC,_tmp2CC,_tmp2CC + 3},(void*)& Cyc_Absyn_designator_t_rep};static char
_tmp2CD[3]="tl";static struct _tuple5 Cyc__gentuple_342={offsetof(struct Cyc_List_List,tl),{
_tmp2CD,_tmp2CD,_tmp2CD + 3},(void*)& Cyc__genrep_332};static struct _tuple5*Cyc__genarr_343[
2]={& Cyc__gentuple_341,& Cyc__gentuple_342};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_344,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_343),(void*)((struct _tuple5**)Cyc__genarr_343),(void*)((
struct _tuple5**)Cyc__genarr_343 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_332={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_designator_t46H2_rep)};struct
_tuple47{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple6 Cyc__gentuple_345={
offsetof(struct _tuple47,f1),(void*)& Cyc__genrep_332};static struct _tuple6 Cyc__gentuple_346={
offsetof(struct _tuple47,f2),(void*)& Cyc__genrep_231};static struct _tuple6*Cyc__genarr_347[
2]={& Cyc__gentuple_345,& Cyc__gentuple_346};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_331={4,sizeof(struct _tuple47),{(void*)((struct _tuple6**)Cyc__genarr_347),(
void*)((struct _tuple6**)Cyc__genarr_347),(void*)((struct _tuple6**)Cyc__genarr_347
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_330={1,1,(void*)((void*)&
Cyc__genrep_331)};static char _tmp2D2[5]="List";static struct _tagged_arr Cyc__genname_351={
_tmp2D2,_tmp2D2,_tmp2D2 + 5};static char _tmp2D3[3]="hd";static struct _tuple5 Cyc__gentuple_348={
offsetof(struct Cyc_List_List,hd),{_tmp2D3,_tmp2D3,_tmp2D3 + 3},(void*)& Cyc__genrep_330};
static char _tmp2D4[3]="tl";static struct _tuple5 Cyc__gentuple_349={offsetof(struct
Cyc_List_List,tl),{_tmp2D4,_tmp2D4,_tmp2D4 + 3},(void*)& Cyc__genrep_329};static
struct _tuple5*Cyc__genarr_350[2]={& Cyc__gentuple_348,& Cyc__gentuple_349};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_351,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_350),(void*)((struct _tuple5**)Cyc__genarr_350),(void*)((
struct _tuple5**)Cyc__genarr_350 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_329={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep)};
struct _tuple48{unsigned int f1;struct Cyc_Absyn_AggrInfo f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;};static struct _tuple6 Cyc__gentuple_412={offsetof(struct
_tuple48,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_413={
offsetof(struct _tuple48,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6 Cyc__gentuple_414={offsetof(struct _tuple48,f3),(void*)& Cyc__genrep_311};
static struct _tuple6 Cyc__gentuple_415={offsetof(struct _tuple48,f4),(void*)& Cyc__genrep_329};
static struct _tuple6*Cyc__genarr_416[4]={& Cyc__gentuple_412,& Cyc__gentuple_413,&
Cyc__gentuple_414,& Cyc__gentuple_415};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_328={
4,sizeof(struct _tuple48),{(void*)((struct _tuple6**)Cyc__genarr_416),(void*)((
struct _tuple6**)Cyc__genarr_416),(void*)((struct _tuple6**)Cyc__genarr_416 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_283;struct _tuple49{unsigned int
f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*f3;struct Cyc_List_List*
f4;};static struct _tuple6 Cyc__gentuple_323={offsetof(struct _tuple49,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_324={offsetof(struct _tuple49,f2),(
void*)((void*)& Cyc__genrep_301)};static struct _tuple6 Cyc__gentuple_325={offsetof(
struct _tuple49,f3),(void*)& Cyc__genrep_284};static struct _tuple6 Cyc__gentuple_326={
offsetof(struct _tuple49,f4),(void*)& Cyc__genrep_236};static struct _tuple6*Cyc__genarr_327[
4]={& Cyc__gentuple_323,& Cyc__gentuple_324,& Cyc__gentuple_325,& Cyc__gentuple_326};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_283={4,sizeof(struct _tuple49),{(
void*)((struct _tuple6**)Cyc__genarr_327),(void*)((struct _tuple6**)Cyc__genarr_327),(
void*)((struct _tuple6**)Cyc__genarr_327 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_269;struct _tuple50{unsigned int f1;char f2;};static struct _tuple6 Cyc__gentuple_270={
offsetof(struct _tuple50,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_271={
offsetof(struct _tuple50,f2),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*
Cyc__genarr_272[2]={& Cyc__gentuple_270,& Cyc__gentuple_271};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_269={4,sizeof(struct _tuple50),{(void*)((struct _tuple6**)Cyc__genarr_272),(
void*)((struct _tuple6**)Cyc__genarr_272),(void*)((struct _tuple6**)Cyc__genarr_272
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_254;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_255;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_256;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_76;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_77;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumfield_rep;static char _tmp2DA[10]="Enumfield";static struct
_tagged_arr Cyc__genname_923={_tmp2DA,_tmp2DA,_tmp2DA + 10};static char _tmp2DB[5]="name";
static struct _tuple5 Cyc__gentuple_919={offsetof(struct Cyc_Absyn_Enumfield,name),{
_tmp2DB,_tmp2DB,_tmp2DB + 5},(void*)& Cyc__genrep_10};static char _tmp2DC[4]="tag";
static struct _tuple5 Cyc__gentuple_920={offsetof(struct Cyc_Absyn_Enumfield,tag),{
_tmp2DC,_tmp2DC,_tmp2DC + 4},(void*)& Cyc__genrep_78};static char _tmp2DD[4]="loc";
static struct _tuple5 Cyc__gentuple_921={offsetof(struct Cyc_Absyn_Enumfield,loc),{
_tmp2DD,_tmp2DD,_tmp2DD + 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_922[
3]={& Cyc__gentuple_919,& Cyc__gentuple_920,& Cyc__gentuple_921};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumfield_rep={3,(struct _tagged_arr*)& Cyc__genname_923,sizeof(
struct Cyc_Absyn_Enumfield),{(void*)((struct _tuple5**)Cyc__genarr_922),(void*)((
struct _tuple5**)Cyc__genarr_922),(void*)((struct _tuple5**)Cyc__genarr_922 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_77={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};
static char _tmp2E0[5]="List";static struct _tagged_arr Cyc__genname_927={_tmp2E0,
_tmp2E0,_tmp2E0 + 5};static char _tmp2E1[3]="hd";static struct _tuple5 Cyc__gentuple_924={
offsetof(struct Cyc_List_List,hd),{_tmp2E1,_tmp2E1,_tmp2E1 + 3},(void*)& Cyc__genrep_77};
static char _tmp2E2[3]="tl";static struct _tuple5 Cyc__gentuple_925={offsetof(struct
Cyc_List_List,tl),{_tmp2E2,_tmp2E2,_tmp2E2 + 3},(void*)& Cyc__genrep_76};static
struct _tuple5*Cyc__genarr_926[2]={& Cyc__gentuple_924,& Cyc__gentuple_925};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_927,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_926),(void*)((struct _tuple5**)Cyc__genarr_926),(void*)((
struct _tuple5**)Cyc__genarr_926 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_76={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_enumfield_t46H2_rep)};static char
_tmp2E5[4]="Opt";static struct _tagged_arr Cyc__genname_259={_tmp2E5,_tmp2E5,
_tmp2E5 + 4};static char _tmp2E6[2]="v";static struct _tuple5 Cyc__gentuple_257={
offsetof(struct Cyc_Core_Opt,v),{_tmp2E6,_tmp2E6,_tmp2E6 + 2},(void*)& Cyc__genrep_76};
static struct _tuple5*Cyc__genarr_258[1]={& Cyc__gentuple_257};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_259,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_258),(
void*)((struct _tuple5**)Cyc__genarr_258),(void*)((struct _tuple5**)Cyc__genarr_258
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_256={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep)};static char _tmp2E9[9]="Enumdecl";
static struct _tagged_arr Cyc__genname_264={_tmp2E9,_tmp2E9,_tmp2E9 + 9};static char
_tmp2EA[3]="sc";static struct _tuple5 Cyc__gentuple_260={offsetof(struct Cyc_Absyn_Enumdecl,sc),{
_tmp2EA,_tmp2EA,_tmp2EA + 3},(void*)& Cyc_Absyn_scope_t_rep};static char _tmp2EB[5]="name";
static struct _tuple5 Cyc__gentuple_261={offsetof(struct Cyc_Absyn_Enumdecl,name),{
_tmp2EB,_tmp2EB,_tmp2EB + 5},(void*)& Cyc__genrep_10};static char _tmp2EC[7]="fields";
static struct _tuple5 Cyc__gentuple_262={offsetof(struct Cyc_Absyn_Enumdecl,fields),{
_tmp2EC,_tmp2EC,_tmp2EC + 7},(void*)& Cyc__genrep_256};static struct _tuple5*Cyc__genarr_263[
3]={& Cyc__gentuple_260,& Cyc__gentuple_261,& Cyc__gentuple_262};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumdecl_rep={3,(struct _tagged_arr*)& Cyc__genname_264,sizeof(
struct Cyc_Absyn_Enumdecl),{(void*)((struct _tuple5**)Cyc__genarr_263),(void*)((
struct _tuple5**)Cyc__genarr_263),(void*)((struct _tuple5**)Cyc__genarr_263 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_255={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
struct _tuple51{unsigned int f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*
f3;};static struct _tuple6 Cyc__gentuple_265={offsetof(struct _tuple51,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_266={offsetof(struct _tuple51,f2),(
void*)& Cyc__genrep_255};static struct _tuple6 Cyc__gentuple_267={offsetof(struct
_tuple51,f3),(void*)& Cyc__genrep_77};static struct _tuple6*Cyc__genarr_268[3]={&
Cyc__gentuple_265,& Cyc__gentuple_266,& Cyc__gentuple_267};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_254={4,sizeof(struct _tuple51),{(void*)((struct _tuple6**)Cyc__genarr_268),(
void*)((struct _tuple6**)Cyc__genarr_268),(void*)((struct _tuple6**)Cyc__genarr_268
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_249;struct _tuple52{
unsigned int f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static struct _tuple6 Cyc__gentuple_250={
offsetof(struct _tuple52,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_251={
offsetof(struct _tuple52,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_252={offsetof(struct _tuple52,f3),(void*)& Cyc__genrep_77};
static struct _tuple6*Cyc__genarr_253[3]={& Cyc__gentuple_250,& Cyc__gentuple_251,&
Cyc__gentuple_252};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_249={4,
sizeof(struct _tuple52),{(void*)((struct _tuple6**)Cyc__genarr_253),(void*)((
struct _tuple6**)Cyc__genarr_253),(void*)((struct _tuple6**)Cyc__genarr_253 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_245;struct _tuple53{unsigned int
f1;struct _tuple0*f2;};static struct _tuple6 Cyc__gentuple_246={offsetof(struct
_tuple53,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_247={
offsetof(struct _tuple53,f2),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_248[
2]={& Cyc__gentuple_246,& Cyc__gentuple_247};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_245={4,sizeof(struct _tuple53),{(void*)((struct _tuple6**)Cyc__genarr_248),(
void*)((struct _tuple6**)Cyc__genarr_248),(void*)((struct _tuple6**)Cyc__genarr_248
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_235;struct _tuple54{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;};static struct _tuple6 Cyc__gentuple_241={
offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_242={
offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_243={
offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_236};static struct _tuple6*Cyc__genarr_244[
3]={& Cyc__gentuple_241,& Cyc__gentuple_242,& Cyc__gentuple_243};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_235={4,sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_244),(
void*)((struct _tuple6**)Cyc__genarr_244),(void*)((struct _tuple6**)Cyc__genarr_244
+ 3)}};static char _tmp2F3[7]="Wild_p";static struct _tuple7 Cyc__gentuple_232={0,{
_tmp2F3,_tmp2F3,_tmp2F3 + 7}};static char _tmp2F4[7]="Null_p";static struct _tuple7
Cyc__gentuple_233={1,{_tmp2F4,_tmp2F4,_tmp2F4 + 7}};static struct _tuple7*Cyc__genarr_234[
2]={& Cyc__gentuple_232,& Cyc__gentuple_233};static char _tmp2F5[6]="Var_p";static
struct _tuple5 Cyc__gentuple_434={0,{_tmp2F5,_tmp2F5,_tmp2F5 + 6},(void*)& Cyc__genrep_430};
static char _tmp2F6[12]="Reference_p";static struct _tuple5 Cyc__gentuple_435={1,{
_tmp2F6,_tmp2F6,_tmp2F6 + 12},(void*)& Cyc__genrep_430};static char _tmp2F7[9]="TagInt_p";
static struct _tuple5 Cyc__gentuple_436={2,{_tmp2F7,_tmp2F7,_tmp2F7 + 9},(void*)& Cyc__genrep_425};
static char _tmp2F8[8]="Tuple_p";static struct _tuple5 Cyc__gentuple_437={3,{_tmp2F8,
_tmp2F8,_tmp2F8 + 8},(void*)& Cyc__genrep_421};static char _tmp2F9[10]="Pointer_p";
static struct _tuple5 Cyc__gentuple_438={4,{_tmp2F9,_tmp2F9,_tmp2F9 + 10},(void*)&
Cyc__genrep_417};static char _tmp2FA[7]="Aggr_p";static struct _tuple5 Cyc__gentuple_439={
5,{_tmp2FA,_tmp2FA,_tmp2FA + 7},(void*)& Cyc__genrep_328};static char _tmp2FB[9]="Tunion_p";
static struct _tuple5 Cyc__gentuple_440={6,{_tmp2FB,_tmp2FB,_tmp2FB + 9},(void*)& Cyc__genrep_283};
static char _tmp2FC[6]="Int_p";static struct _tuple5 Cyc__gentuple_441={7,{_tmp2FC,
_tmp2FC,_tmp2FC + 6},(void*)& Cyc__genrep_273};static char _tmp2FD[7]="Char_p";
static struct _tuple5 Cyc__gentuple_442={8,{_tmp2FD,_tmp2FD,_tmp2FD + 7},(void*)& Cyc__genrep_269};
static char _tmp2FE[8]="Float_p";static struct _tuple5 Cyc__gentuple_443={9,{_tmp2FE,
_tmp2FE,_tmp2FE + 8},(void*)& Cyc__genrep_117};static char _tmp2FF[7]="Enum_p";
static struct _tuple5 Cyc__gentuple_444={10,{_tmp2FF,_tmp2FF,_tmp2FF + 7},(void*)&
Cyc__genrep_254};static char _tmp300[11]="AnonEnum_p";static struct _tuple5 Cyc__gentuple_445={
11,{_tmp300,_tmp300,_tmp300 + 11},(void*)& Cyc__genrep_249};static char _tmp301[12]="UnknownId_p";
static struct _tuple5 Cyc__gentuple_446={12,{_tmp301,_tmp301,_tmp301 + 12},(void*)&
Cyc__genrep_245};static char _tmp302[14]="UnknownCall_p";static struct _tuple5 Cyc__gentuple_447={
13,{_tmp302,_tmp302,_tmp302 + 14},(void*)& Cyc__genrep_235};static struct _tuple5*
Cyc__genarr_448[14]={& Cyc__gentuple_434,& Cyc__gentuple_435,& Cyc__gentuple_436,&
Cyc__gentuple_437,& Cyc__gentuple_438,& Cyc__gentuple_439,& Cyc__gentuple_440,& Cyc__gentuple_441,&
Cyc__gentuple_442,& Cyc__gentuple_443,& Cyc__gentuple_444,& Cyc__gentuple_445,& Cyc__gentuple_446,&
Cyc__gentuple_447};static char _tmp304[8]="Raw_pat";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep={5,{_tmp304,_tmp304,_tmp304 + 8},{(void*)((struct _tuple7**)
Cyc__genarr_234),(void*)((struct _tuple7**)Cyc__genarr_234),(void*)((struct
_tuple7**)Cyc__genarr_234 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_448),(void*)((
struct _tuple5**)Cyc__genarr_448),(void*)((struct _tuple5**)Cyc__genarr_448 + 14)}};
static char _tmp305[4]="Pat";static struct _tagged_arr Cyc__genname_453={_tmp305,
_tmp305,_tmp305 + 4};static char _tmp306[2]="r";static struct _tuple5 Cyc__gentuple_449={
offsetof(struct Cyc_Absyn_Pat,r),{_tmp306,_tmp306,_tmp306 + 2},(void*)& Cyc_Absyn_raw_pat_t_rep};
static char _tmp307[5]="topt";static struct _tuple5 Cyc__gentuple_450={offsetof(
struct Cyc_Absyn_Pat,topt),{_tmp307,_tmp307,_tmp307 + 5},(void*)& Cyc__genrep_43};
static char _tmp308[4]="loc";static struct _tuple5 Cyc__gentuple_451={offsetof(struct
Cyc_Absyn_Pat,loc),{_tmp308,_tmp308,_tmp308 + 4},(void*)& Cyc__genrep_2};static
struct _tuple5*Cyc__genarr_452[3]={& Cyc__gentuple_449,& Cyc__gentuple_450,& Cyc__gentuple_451};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep={3,(struct _tagged_arr*)&
Cyc__genname_453,sizeof(struct Cyc_Absyn_Pat),{(void*)((struct _tuple5**)Cyc__genarr_452),(
void*)((struct _tuple5**)Cyc__genarr_452),(void*)((struct _tuple5**)Cyc__genarr_452
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_231={1,1,(void*)((void*)&
Cyc_struct_Absyn_Pat_rep)};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_131;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_132;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_vardecl_t46H2_rep;static char _tmp30B[5]="List";static
struct _tagged_arr Cyc__genname_158={_tmp30B,_tmp30B,_tmp30B + 5};static char _tmp30C[
3]="hd";static struct _tuple5 Cyc__gentuple_155={offsetof(struct Cyc_List_List,hd),{
_tmp30C,_tmp30C,_tmp30C + 3},(void*)& Cyc__genrep_133};static char _tmp30D[3]="tl";
static struct _tuple5 Cyc__gentuple_156={offsetof(struct Cyc_List_List,tl),{_tmp30D,
_tmp30D,_tmp30D + 3},(void*)& Cyc__genrep_132};static struct _tuple5*Cyc__genarr_157[
2]={& Cyc__gentuple_155,& Cyc__gentuple_156};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_vardecl_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_158,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_157),(void*)((struct _tuple5**)Cyc__genarr_157),(void*)((
struct _tuple5**)Cyc__genarr_157 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_132={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_vardecl_t46H2_rep)};static char
_tmp310[4]="Opt";static struct _tagged_arr Cyc__genname_161={_tmp310,_tmp310,
_tmp310 + 4};static char _tmp311[2]="v";static struct _tuple5 Cyc__gentuple_159={
offsetof(struct Cyc_Core_Opt,v),{_tmp311,_tmp311,_tmp311 + 2},(void*)& Cyc__genrep_132};
static struct _tuple5*Cyc__genarr_160[1]={& Cyc__gentuple_159};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_161,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_160),(
void*)((struct _tuple5**)Cyc__genarr_160),(void*)((struct _tuple5**)Cyc__genarr_160
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_131={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep)};static char _tmp314[14]="Switch_clause";
static struct _tagged_arr Cyc__genname_460={_tmp314,_tmp314,_tmp314 + 14};static char
_tmp315[8]="pattern";static struct _tuple5 Cyc__gentuple_454={offsetof(struct Cyc_Absyn_Switch_clause,pattern),{
_tmp315,_tmp315,_tmp315 + 8},(void*)& Cyc__genrep_231};static char _tmp316[9]="pat_vars";
static struct _tuple5 Cyc__gentuple_455={offsetof(struct Cyc_Absyn_Switch_clause,pat_vars),{
_tmp316,_tmp316,_tmp316 + 9},(void*)& Cyc__genrep_131};static char _tmp317[13]="where_clause";
static struct _tuple5 Cyc__gentuple_456={offsetof(struct Cyc_Absyn_Switch_clause,where_clause),{
_tmp317,_tmp317,_tmp317 + 13},(void*)& Cyc__genrep_78};static char _tmp318[5]="body";
static struct _tuple5 Cyc__gentuple_457={offsetof(struct Cyc_Absyn_Switch_clause,body),{
_tmp318,_tmp318,_tmp318 + 5},(void*)& Cyc__genrep_162};static char _tmp319[4]="loc";
static struct _tuple5 Cyc__gentuple_458={offsetof(struct Cyc_Absyn_Switch_clause,loc),{
_tmp319,_tmp319,_tmp319 + 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_459[
5]={& Cyc__gentuple_454,& Cyc__gentuple_455,& Cyc__gentuple_456,& Cyc__gentuple_457,&
Cyc__gentuple_458};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep={
3,(struct _tagged_arr*)& Cyc__genname_460,sizeof(struct Cyc_Absyn_Switch_clause),{(
void*)((struct _tuple5**)Cyc__genarr_459),(void*)((struct _tuple5**)Cyc__genarr_459),(
void*)((struct _tuple5**)Cyc__genarr_459 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_230={1,1,(void*)((void*)& Cyc_struct_Absyn_Switch_clause_rep)};static
char _tmp31C[5]="List";static struct _tagged_arr Cyc__genname_464={_tmp31C,_tmp31C,
_tmp31C + 5};static char _tmp31D[3]="hd";static struct _tuple5 Cyc__gentuple_461={
offsetof(struct Cyc_List_List,hd),{_tmp31D,_tmp31D,_tmp31D + 3},(void*)((void*)&
Cyc__genrep_230)};static char _tmp31E[3]="tl";static struct _tuple5 Cyc__gentuple_462={
offsetof(struct Cyc_List_List,tl),{_tmp31E,_tmp31E,_tmp31E + 3},(void*)& Cyc__genrep_229};
static struct _tuple5*Cyc__genarr_463[2]={& Cyc__gentuple_461,& Cyc__gentuple_462};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_464,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_463),(void*)((struct _tuple5**)Cyc__genarr_463),(void*)((
struct _tuple5**)Cyc__genarr_463 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_229={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep)};struct
_tuple55{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_516={offsetof(struct _tuple55,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_517={offsetof(struct _tuple55,f2),(void*)& Cyc__genrep_82};
static struct _tuple6 Cyc__gentuple_518={offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_229};
static struct _tuple6*Cyc__genarr_519[3]={& Cyc__gentuple_516,& Cyc__gentuple_517,&
Cyc__gentuple_518};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_515={4,
sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_519),(void*)((
struct _tuple6**)Cyc__genarr_519),(void*)((struct _tuple6**)Cyc__genarr_519 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_499;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_500;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_501;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_SwitchC_clause_rep;static char _tmp322[15]="SwitchC_clause";
static struct _tagged_arr Cyc__genname_506={_tmp322,_tmp322,_tmp322 + 15};static char
_tmp323[9]="cnst_exp";static struct _tuple5 Cyc__gentuple_502={offsetof(struct Cyc_Absyn_SwitchC_clause,cnst_exp),{
_tmp323,_tmp323,_tmp323 + 9},(void*)& Cyc__genrep_78};static char _tmp324[5]="body";
static struct _tuple5 Cyc__gentuple_503={offsetof(struct Cyc_Absyn_SwitchC_clause,body),{
_tmp324,_tmp324,_tmp324 + 5},(void*)& Cyc__genrep_162};static char _tmp325[4]="loc";
static struct _tuple5 Cyc__gentuple_504={offsetof(struct Cyc_Absyn_SwitchC_clause,loc),{
_tmp325,_tmp325,_tmp325 + 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_505[
3]={& Cyc__gentuple_502,& Cyc__gentuple_503,& Cyc__gentuple_504};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_SwitchC_clause_rep={3,(struct _tagged_arr*)& Cyc__genname_506,
sizeof(struct Cyc_Absyn_SwitchC_clause),{(void*)((struct _tuple5**)Cyc__genarr_505),(
void*)((struct _tuple5**)Cyc__genarr_505),(void*)((struct _tuple5**)Cyc__genarr_505
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_501={1,1,(void*)((void*)&
Cyc_struct_Absyn_SwitchC_clause_rep)};static char _tmp328[5]="List";static struct
_tagged_arr Cyc__genname_510={_tmp328,_tmp328,_tmp328 + 5};static char _tmp329[3]="hd";
static struct _tuple5 Cyc__gentuple_507={offsetof(struct Cyc_List_List,hd),{_tmp329,
_tmp329,_tmp329 + 3},(void*)& Cyc__genrep_501};static char _tmp32A[3]="tl";static
struct _tuple5 Cyc__gentuple_508={offsetof(struct Cyc_List_List,tl),{_tmp32A,
_tmp32A,_tmp32A + 3},(void*)& Cyc__genrep_500};static struct _tuple5*Cyc__genarr_509[
2]={& Cyc__gentuple_507,& Cyc__gentuple_508};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_510,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_509),(void*)((struct _tuple5**)Cyc__genarr_509),(void*)((
struct _tuple5**)Cyc__genarr_509 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_500={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep)};static
struct _tuple6 Cyc__gentuple_511={offsetof(struct _tuple55,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_512={offsetof(struct _tuple55,f2),(void*)& Cyc__genrep_82};
static struct _tuple6 Cyc__gentuple_513={offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_500};
static struct _tuple6*Cyc__genarr_514[3]={& Cyc__gentuple_511,& Cyc__gentuple_512,&
Cyc__gentuple_513};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_499={4,
sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_514),(void*)((
struct _tuple6**)Cyc__genarr_514),(void*)((struct _tuple6**)Cyc__genarr_514 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_488;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_490;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep;
static char _tmp32E[5]="List";static struct _tagged_arr Cyc__genname_494={_tmp32E,
_tmp32E,_tmp32E + 5};static char _tmp32F[3]="hd";static struct _tuple5 Cyc__gentuple_491={
offsetof(struct Cyc_List_List,hd),{_tmp32F,_tmp32F,_tmp32F + 3},(void*)& Cyc__genrep_82};
static char _tmp330[3]="tl";static struct _tuple5 Cyc__gentuple_492={offsetof(struct
Cyc_List_List,tl),{_tmp330,_tmp330,_tmp330 + 3},(void*)& Cyc__genrep_490};static
struct _tuple5*Cyc__genarr_493[2]={& Cyc__gentuple_491,& Cyc__gentuple_492};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep={3,(struct
_tagged_arr*)& Cyc__genname_494,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_493),(void*)((struct _tuple5**)Cyc__genarr_493),(void*)((
struct _tuple5**)Cyc__genarr_493 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_490={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_exp_t46H2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_489;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_489={1,1,(void*)((
void*)& Cyc__genrep_230)};struct _tuple56{unsigned int f1;struct Cyc_List_List*f2;
struct Cyc_Absyn_Switch_clause**f3;};static struct _tuple6 Cyc__gentuple_495={
offsetof(struct _tuple56,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_496={
offsetof(struct _tuple56,f2),(void*)& Cyc__genrep_490};static struct _tuple6 Cyc__gentuple_497={
offsetof(struct _tuple56,f3),(void*)& Cyc__genrep_489};static struct _tuple6*Cyc__genarr_498[
3]={& Cyc__gentuple_495,& Cyc__gentuple_496,& Cyc__gentuple_497};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_488={4,sizeof(struct _tuple56),{(void*)((struct _tuple6**)Cyc__genarr_498),(
void*)((struct _tuple6**)Cyc__genarr_498),(void*)((struct _tuple6**)Cyc__genarr_498
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_483;struct _tuple57{
unsigned int f1;struct Cyc_Absyn_Decl*f2;struct Cyc_Absyn_Stmt*f3;};static struct
_tuple6 Cyc__gentuple_484={offsetof(struct _tuple57,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_485={offsetof(struct _tuple57,f2),(void*)& Cyc__genrep_1};
static struct _tuple6 Cyc__gentuple_486={offsetof(struct _tuple57,f3),(void*)& Cyc__genrep_162};
static struct _tuple6*Cyc__genarr_487[3]={& Cyc__gentuple_484,& Cyc__gentuple_485,&
Cyc__gentuple_486};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_483={4,
sizeof(struct _tuple57),{(void*)((struct _tuple6**)Cyc__genarr_487),(void*)((
struct _tuple6**)Cyc__genarr_487),(void*)((struct _tuple6**)Cyc__genarr_487 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_479;static struct _tuple6 Cyc__gentuple_480={
offsetof(struct _tuple38,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_481={
offsetof(struct _tuple38,f2),(void*)& Cyc__genrep_162};static struct _tuple6*Cyc__genarr_482[
2]={& Cyc__gentuple_480,& Cyc__gentuple_481};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_479={4,sizeof(struct _tuple38),{(void*)((struct _tuple6**)Cyc__genarr_482),(
void*)((struct _tuple6**)Cyc__genarr_482),(void*)((struct _tuple6**)Cyc__genarr_482
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_474;static struct _tuple6
Cyc__gentuple_475={offsetof(struct _tuple39,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_476={offsetof(struct _tuple39,f2),(void*)& Cyc__genrep_12};
static struct _tuple6 Cyc__gentuple_477={offsetof(struct _tuple39,f3),(void*)& Cyc__genrep_162};
static struct _tuple6*Cyc__genarr_478[3]={& Cyc__gentuple_475,& Cyc__gentuple_476,&
Cyc__gentuple_477};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_474={4,
sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_478),(void*)((
struct _tuple6**)Cyc__genarr_478),(void*)((struct _tuple6**)Cyc__genarr_478 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_469;struct _tuple58{unsigned int
f1;struct Cyc_Absyn_Stmt*f2;struct _tuple2 f3;};static struct _tuple6 Cyc__gentuple_470={
offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_471={
offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_162};static struct _tuple6 Cyc__gentuple_472={
offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_171};static struct _tuple6*Cyc__genarr_473[
3]={& Cyc__gentuple_470,& Cyc__gentuple_471,& Cyc__gentuple_472};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_469={4,sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_473),(
void*)((struct _tuple6**)Cyc__genarr_473),(void*)((struct _tuple6**)Cyc__genarr_473
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_228;struct _tuple59{
unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_465={offsetof(struct _tuple59,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_466={offsetof(struct _tuple59,f2),(void*)& Cyc__genrep_162};
static struct _tuple6 Cyc__gentuple_467={offsetof(struct _tuple59,f3),(void*)& Cyc__genrep_229};
static struct _tuple6*Cyc__genarr_468[3]={& Cyc__gentuple_465,& Cyc__gentuple_466,&
Cyc__gentuple_467};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_228={4,
sizeof(struct _tuple59),{(void*)((struct _tuple6**)Cyc__genarr_468),(void*)((
struct _tuple6**)Cyc__genarr_468),(void*)((struct _tuple6**)Cyc__genarr_468 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_184;struct _tuple60{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;int f4;struct Cyc_Absyn_Stmt*
f5;};static struct _tuple6 Cyc__gentuple_222={offsetof(struct _tuple60,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_223={offsetof(struct _tuple60,f2),(
void*)& Cyc__genrep_185};static struct _tuple6 Cyc__gentuple_224={offsetof(struct
_tuple60,f3),(void*)& Cyc__genrep_133};static struct _tuple6 Cyc__gentuple_225={
offsetof(struct _tuple60,f4),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6
Cyc__gentuple_226={offsetof(struct _tuple60,f5),(void*)& Cyc__genrep_162};static
struct _tuple6*Cyc__genarr_227[5]={& Cyc__gentuple_222,& Cyc__gentuple_223,& Cyc__gentuple_224,&
Cyc__gentuple_225,& Cyc__gentuple_226};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_184={
4,sizeof(struct _tuple60),{(void*)((struct _tuple6**)Cyc__genarr_227),(void*)((
struct _tuple6**)Cyc__genarr_227),(void*)((struct _tuple6**)Cyc__genarr_227 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_170;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_forarray_info_t_rep;static char _tmp33B[13]="ForArrayInfo";static struct
_tagged_arr Cyc__genname_180={_tmp33B,_tmp33B,_tmp33B + 13};static char _tmp33C[6]="defns";
static struct _tuple5 Cyc__gentuple_175={offsetof(struct Cyc_Absyn_ForArrayInfo,defns),{
_tmp33C,_tmp33C,_tmp33C + 6},(void*)& Cyc__genrep_132};static char _tmp33D[10]="condition";
static struct _tuple5 Cyc__gentuple_176={offsetof(struct Cyc_Absyn_ForArrayInfo,condition),{
_tmp33D,_tmp33D,_tmp33D + 10},(void*)& Cyc__genrep_171};static char _tmp33E[6]="delta";
static struct _tuple5 Cyc__gentuple_177={offsetof(struct Cyc_Absyn_ForArrayInfo,delta),{
_tmp33E,_tmp33E,_tmp33E + 6},(void*)& Cyc__genrep_171};static char _tmp33F[5]="body";
static struct _tuple5 Cyc__gentuple_178={offsetof(struct Cyc_Absyn_ForArrayInfo,body),{
_tmp33F,_tmp33F,_tmp33F + 5},(void*)& Cyc__genrep_162};static struct _tuple5*Cyc__genarr_179[
4]={& Cyc__gentuple_175,& Cyc__gentuple_176,& Cyc__gentuple_177,& Cyc__gentuple_178};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_forarray_info_t_rep={3,(struct
_tagged_arr*)& Cyc__genname_180,sizeof(struct Cyc_Absyn_ForArrayInfo),{(void*)((
struct _tuple5**)Cyc__genarr_179),(void*)((struct _tuple5**)Cyc__genarr_179),(void*)((
struct _tuple5**)Cyc__genarr_179 + 4)}};struct _tuple61{unsigned int f1;struct Cyc_Absyn_ForArrayInfo
f2;};static struct _tuple6 Cyc__gentuple_181={offsetof(struct _tuple61,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_182={offsetof(struct _tuple61,f2),(
void*)& Cyc_Absyn_forarray_info_t_rep};static struct _tuple6*Cyc__genarr_183[2]={&
Cyc__gentuple_181,& Cyc__gentuple_182};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_170={
4,sizeof(struct _tuple61),{(void*)((struct _tuple6**)Cyc__genarr_183),(void*)((
struct _tuple6**)Cyc__genarr_183),(void*)((struct _tuple6**)Cyc__genarr_183 + 2)}};
static char _tmp342[7]="Skip_s";static struct _tuple7 Cyc__gentuple_168={0,{_tmp342,
_tmp342,_tmp342 + 7}};static struct _tuple7*Cyc__genarr_169[1]={& Cyc__gentuple_168};
static char _tmp343[6]="Exp_s";static struct _tuple5 Cyc__gentuple_557={0,{_tmp343,
_tmp343,_tmp343 + 6},(void*)& Cyc__genrep_81};static char _tmp344[6]="Seq_s";static
struct _tuple5 Cyc__gentuple_558={1,{_tmp344,_tmp344,_tmp344 + 6},(void*)& Cyc__genrep_552};
static char _tmp345[9]="Return_s";static struct _tuple5 Cyc__gentuple_559={2,{_tmp345,
_tmp345,_tmp345 + 9},(void*)& Cyc__genrep_548};static char _tmp346[13]="IfThenElse_s";
static struct _tuple5 Cyc__gentuple_560={3,{_tmp346,_tmp346,_tmp346 + 13},(void*)&
Cyc__genrep_542};static char _tmp347[8]="While_s";static struct _tuple5 Cyc__gentuple_561={
4,{_tmp347,_tmp347,_tmp347 + 8},(void*)& Cyc__genrep_537};static char _tmp348[8]="Break_s";
static struct _tuple5 Cyc__gentuple_562={5,{_tmp348,_tmp348,_tmp348 + 8},(void*)& Cyc__genrep_533};
static char _tmp349[11]="Continue_s";static struct _tuple5 Cyc__gentuple_563={6,{
_tmp349,_tmp349,_tmp349 + 11},(void*)& Cyc__genrep_533};static char _tmp34A[7]="Goto_s";
static struct _tuple5 Cyc__gentuple_564={7,{_tmp34A,_tmp34A,_tmp34A + 7},(void*)& Cyc__genrep_527};
static char _tmp34B[6]="For_s";static struct _tuple5 Cyc__gentuple_565={8,{_tmp34B,
_tmp34B,_tmp34B + 6},(void*)& Cyc__genrep_520};static char _tmp34C[9]="Switch_s";
static struct _tuple5 Cyc__gentuple_566={9,{_tmp34C,_tmp34C,_tmp34C + 9},(void*)& Cyc__genrep_515};
static char _tmp34D[10]="SwitchC_s";static struct _tuple5 Cyc__gentuple_567={10,{
_tmp34D,_tmp34D,_tmp34D + 10},(void*)& Cyc__genrep_499};static char _tmp34E[11]="Fallthru_s";
static struct _tuple5 Cyc__gentuple_568={11,{_tmp34E,_tmp34E,_tmp34E + 11},(void*)&
Cyc__genrep_488};static char _tmp34F[7]="Decl_s";static struct _tuple5 Cyc__gentuple_569={
12,{_tmp34F,_tmp34F,_tmp34F + 7},(void*)& Cyc__genrep_483};static char _tmp350[6]="Cut_s";
static struct _tuple5 Cyc__gentuple_570={13,{_tmp350,_tmp350,_tmp350 + 6},(void*)&
Cyc__genrep_479};static char _tmp351[9]="Splice_s";static struct _tuple5 Cyc__gentuple_571={
14,{_tmp351,_tmp351,_tmp351 + 9},(void*)& Cyc__genrep_479};static char _tmp352[8]="Label_s";
static struct _tuple5 Cyc__gentuple_572={15,{_tmp352,_tmp352,_tmp352 + 8},(void*)&
Cyc__genrep_474};static char _tmp353[5]="Do_s";static struct _tuple5 Cyc__gentuple_573={
16,{_tmp353,_tmp353,_tmp353 + 5},(void*)& Cyc__genrep_469};static char _tmp354[11]="TryCatch_s";
static struct _tuple5 Cyc__gentuple_574={17,{_tmp354,_tmp354,_tmp354 + 11},(void*)&
Cyc__genrep_228};static char _tmp355[9]="Region_s";static struct _tuple5 Cyc__gentuple_575={
18,{_tmp355,_tmp355,_tmp355 + 9},(void*)& Cyc__genrep_184};static char _tmp356[11]="ForArray_s";
static struct _tuple5 Cyc__gentuple_576={19,{_tmp356,_tmp356,_tmp356 + 11},(void*)&
Cyc__genrep_170};static char _tmp357[14]="ResetRegion_s";static struct _tuple5 Cyc__gentuple_577={
20,{_tmp357,_tmp357,_tmp357 + 14},(void*)& Cyc__genrep_81};static struct _tuple5*Cyc__genarr_578[
21]={& Cyc__gentuple_557,& Cyc__gentuple_558,& Cyc__gentuple_559,& Cyc__gentuple_560,&
Cyc__gentuple_561,& Cyc__gentuple_562,& Cyc__gentuple_563,& Cyc__gentuple_564,& Cyc__gentuple_565,&
Cyc__gentuple_566,& Cyc__gentuple_567,& Cyc__gentuple_568,& Cyc__gentuple_569,& Cyc__gentuple_570,&
Cyc__gentuple_571,& Cyc__gentuple_572,& Cyc__gentuple_573,& Cyc__gentuple_574,& Cyc__gentuple_575,&
Cyc__gentuple_576,& Cyc__gentuple_577};static char _tmp359[9]="Raw_stmt";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_stmt_t_rep={5,{_tmp359,_tmp359,_tmp359 + 9},{(void*)((struct _tuple7**)
Cyc__genarr_169),(void*)((struct _tuple7**)Cyc__genarr_169),(void*)((struct
_tuple7**)Cyc__genarr_169 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_578),(void*)((
struct _tuple5**)Cyc__genarr_578),(void*)((struct _tuple5**)Cyc__genarr_578 + 21)}};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_163;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_stmt_t46H2_rep;static char _tmp35A[5]="List";static
struct _tagged_arr Cyc__genname_167={_tmp35A,_tmp35A,_tmp35A + 5};static char _tmp35B[
3]="hd";static struct _tuple5 Cyc__gentuple_164={offsetof(struct Cyc_List_List,hd),{
_tmp35B,_tmp35B,_tmp35B + 3},(void*)& Cyc__genrep_162};static char _tmp35C[3]="tl";
static struct _tuple5 Cyc__gentuple_165={offsetof(struct Cyc_List_List,tl),{_tmp35C,
_tmp35C,_tmp35C + 3},(void*)& Cyc__genrep_163};static struct _tuple5*Cyc__genarr_166[
2]={& Cyc__gentuple_164,& Cyc__gentuple_165};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_167,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_166),(void*)((struct _tuple5**)Cyc__genarr_166),(void*)((
struct _tuple5**)Cyc__genarr_166 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_163={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_stmt_t46H2_rep)};extern struct Cyc_Typerep_XTUnion_struct
Cyc_Absyn_absyn_annot_t_rep;static struct _tuple8*Cyc__genarr_79[0]={};static char
_tmp360[11]="AbsynAnnot";struct Cyc_Typerep_XTUnion_struct Cyc_Absyn_absyn_annot_t_rep={
7,{_tmp360,_tmp360,_tmp360 + 11},{(void*)((struct _tuple8**)Cyc__genarr_79),(void*)((
struct _tuple8**)Cyc__genarr_79),(void*)((struct _tuple8**)Cyc__genarr_79 + 0)}};
static char _tmp361[5]="Stmt";static struct _tagged_arr Cyc__genname_585={_tmp361,
_tmp361,_tmp361 + 5};static char _tmp362[2]="r";static struct _tuple5 Cyc__gentuple_579={
offsetof(struct Cyc_Absyn_Stmt,r),{_tmp362,_tmp362,_tmp362 + 2},(void*)& Cyc_Absyn_raw_stmt_t_rep};
static char _tmp363[4]="loc";static struct _tuple5 Cyc__gentuple_580={offsetof(struct
Cyc_Absyn_Stmt,loc),{_tmp363,_tmp363,_tmp363 + 4},(void*)& Cyc__genrep_2};static
char _tmp364[16]="non_local_preds";static struct _tuple5 Cyc__gentuple_581={
offsetof(struct Cyc_Absyn_Stmt,non_local_preds),{_tmp364,_tmp364,_tmp364 + 16},(
void*)& Cyc__genrep_163};static char _tmp365[10]="try_depth";static struct _tuple5 Cyc__gentuple_582={
offsetof(struct Cyc_Absyn_Stmt,try_depth),{_tmp365,_tmp365,_tmp365 + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp366[6]="annot";static struct _tuple5 Cyc__gentuple_583={
offsetof(struct Cyc_Absyn_Stmt,annot),{_tmp366,_tmp366,_tmp366 + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};
static struct _tuple5*Cyc__genarr_584[5]={& Cyc__gentuple_579,& Cyc__gentuple_580,&
Cyc__gentuple_581,& Cyc__gentuple_582,& Cyc__gentuple_583};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep={3,(struct _tagged_arr*)& Cyc__genname_585,sizeof(struct
Cyc_Absyn_Stmt),{(void*)((struct _tuple5**)Cyc__genarr_584),(void*)((struct
_tuple5**)Cyc__genarr_584),(void*)((struct _tuple5**)Cyc__genarr_584 + 5)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_162={1,1,(void*)((void*)& Cyc_struct_Absyn_Stmt_rep)};
static char _tmp369[7]="Fndecl";static struct _tagged_arr Cyc__genname_623={_tmp369,
_tmp369,_tmp369 + 7};static char _tmp36A[3]="sc";static struct _tuple5 Cyc__gentuple_608={
offsetof(struct Cyc_Absyn_Fndecl,sc),{_tmp36A,_tmp36A,_tmp36A + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp36B[10]="is_inline";static struct _tuple5 Cyc__gentuple_609={
offsetof(struct Cyc_Absyn_Fndecl,is_inline),{_tmp36B,_tmp36B,_tmp36B + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp36C[5]="name";static struct _tuple5 Cyc__gentuple_610={
offsetof(struct Cyc_Absyn_Fndecl,name),{_tmp36C,_tmp36C,_tmp36C + 5},(void*)& Cyc__genrep_10};
static char _tmp36D[4]="tvs";static struct _tuple5 Cyc__gentuple_611={offsetof(struct
Cyc_Absyn_Fndecl,tvs),{_tmp36D,_tmp36D,_tmp36D + 4},(void*)& Cyc__genrep_311};
static char _tmp36E[7]="effect";static struct _tuple5 Cyc__gentuple_612={offsetof(
struct Cyc_Absyn_Fndecl,effect),{_tmp36E,_tmp36E,_tmp36E + 7},(void*)& Cyc__genrep_43};
static char _tmp36F[9]="ret_type";static struct _tuple5 Cyc__gentuple_613={offsetof(
struct Cyc_Absyn_Fndecl,ret_type),{_tmp36F,_tmp36F,_tmp36F + 9},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp370[5]="args";static struct _tuple5 Cyc__gentuple_614={
offsetof(struct Cyc_Absyn_Fndecl,args),{_tmp370,_tmp370,_tmp370 + 5},(void*)& Cyc__genrep_597};
static char _tmp371[10]="c_varargs";static struct _tuple5 Cyc__gentuple_615={
offsetof(struct Cyc_Absyn_Fndecl,c_varargs),{_tmp371,_tmp371,_tmp371 + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp372[12]="cyc_varargs";static struct _tuple5
Cyc__gentuple_616={offsetof(struct Cyc_Absyn_Fndecl,cyc_varargs),{_tmp372,_tmp372,
_tmp372 + 12},(void*)& Cyc__genrep_586};static char _tmp373[7]="rgn_po";static struct
_tuple5 Cyc__gentuple_617={offsetof(struct Cyc_Absyn_Fndecl,rgn_po),{_tmp373,
_tmp373,_tmp373 + 7},(void*)& Cyc__genrep_370};static char _tmp374[5]="body";static
struct _tuple5 Cyc__gentuple_618={offsetof(struct Cyc_Absyn_Fndecl,body),{_tmp374,
_tmp374,_tmp374 + 5},(void*)& Cyc__genrep_162};static char _tmp375[11]="cached_typ";
static struct _tuple5 Cyc__gentuple_619={offsetof(struct Cyc_Absyn_Fndecl,cached_typ),{
_tmp375,_tmp375,_tmp375 + 11},(void*)& Cyc__genrep_43};static char _tmp376[15]="param_vardecls";
static struct _tuple5 Cyc__gentuple_620={offsetof(struct Cyc_Absyn_Fndecl,param_vardecls),{
_tmp376,_tmp376,_tmp376 + 15},(void*)& Cyc__genrep_131};static char _tmp377[11]="attributes";
static struct _tuple5 Cyc__gentuple_621={offsetof(struct Cyc_Absyn_Fndecl,attributes),{
_tmp377,_tmp377,_tmp377 + 11},(void*)& Cyc__genrep_88};static struct _tuple5*Cyc__genarr_622[
14]={& Cyc__gentuple_608,& Cyc__gentuple_609,& Cyc__gentuple_610,& Cyc__gentuple_611,&
Cyc__gentuple_612,& Cyc__gentuple_613,& Cyc__gentuple_614,& Cyc__gentuple_615,& Cyc__gentuple_616,&
Cyc__gentuple_617,& Cyc__gentuple_618,& Cyc__gentuple_619,& Cyc__gentuple_620,& Cyc__gentuple_621};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep={3,(struct _tagged_arr*)&
Cyc__genname_623,sizeof(struct Cyc_Absyn_Fndecl),{(void*)((struct _tuple5**)Cyc__genarr_622),(
void*)((struct _tuple5**)Cyc__genarr_622),(void*)((struct _tuple5**)Cyc__genarr_622
+ 14)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_87={1,1,(void*)((void*)&
Cyc_struct_Absyn_Fndecl_rep)};struct _tuple62{unsigned int f1;struct Cyc_Absyn_Fndecl*
f2;};static struct _tuple6 Cyc__gentuple_624={offsetof(struct _tuple62,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_625={offsetof(struct _tuple62,f2),(
void*)& Cyc__genrep_87};static struct _tuple6*Cyc__genarr_626[2]={& Cyc__gentuple_624,&
Cyc__gentuple_625};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_86={4,sizeof(
struct _tuple62),{(void*)((struct _tuple6**)Cyc__genarr_626),(void*)((struct
_tuple6**)Cyc__genarr_626),(void*)((struct _tuple6**)Cyc__genarr_626 + 2)}};static
char _tmp37B[13]="Unresolved_b";static struct _tuple7 Cyc__gentuple_832={0,{_tmp37B,
_tmp37B,_tmp37B + 13}};static struct _tuple7*Cyc__genarr_833[1]={& Cyc__gentuple_832};
static char _tmp37C[9]="Global_b";static struct _tuple5 Cyc__gentuple_834={0,{_tmp37C,
_tmp37C,_tmp37C + 9},(void*)& Cyc__genrep_430};static char _tmp37D[10]="Funname_b";
static struct _tuple5 Cyc__gentuple_835={1,{_tmp37D,_tmp37D,_tmp37D + 10},(void*)&
Cyc__genrep_86};static char _tmp37E[8]="Param_b";static struct _tuple5 Cyc__gentuple_836={
2,{_tmp37E,_tmp37E,_tmp37E + 8},(void*)& Cyc__genrep_430};static char _tmp37F[8]="Local_b";
static struct _tuple5 Cyc__gentuple_837={3,{_tmp37F,_tmp37F,_tmp37F + 8},(void*)& Cyc__genrep_430};
static char _tmp380[6]="Pat_b";static struct _tuple5 Cyc__gentuple_838={4,{_tmp380,
_tmp380,_tmp380 + 6},(void*)& Cyc__genrep_430};static struct _tuple5*Cyc__genarr_839[
5]={& Cyc__gentuple_834,& Cyc__gentuple_835,& Cyc__gentuple_836,& Cyc__gentuple_837,&
Cyc__gentuple_838};static char _tmp382[8]="Binding";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_binding_t_rep={5,{_tmp382,_tmp382,_tmp382 + 8},{(void*)((struct _tuple7**)
Cyc__genarr_833),(void*)((struct _tuple7**)Cyc__genarr_833),(void*)((struct
_tuple7**)Cyc__genarr_833 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_839),(void*)((
struct _tuple5**)Cyc__genarr_839),(void*)((struct _tuple5**)Cyc__genarr_839 + 5)}};
struct _tuple63{unsigned int f1;struct _tuple0*f2;void*f3;};static struct _tuple6 Cyc__gentuple_840={
offsetof(struct _tuple63,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_841={
offsetof(struct _tuple63,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_842={
offsetof(struct _tuple63,f3),(void*)& Cyc_Absyn_binding_t_rep};static struct _tuple6*
Cyc__genarr_843[3]={& Cyc__gentuple_840,& Cyc__gentuple_841,& Cyc__gentuple_842};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_831={4,sizeof(struct _tuple63),{(
void*)((struct _tuple6**)Cyc__genarr_843),(void*)((struct _tuple6**)Cyc__genarr_843),(
void*)((struct _tuple6**)Cyc__genarr_843 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_826;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep;
static char _tmp384[5]="Plus";static struct _tuple7 Cyc__gentuple_796={0,{_tmp384,
_tmp384,_tmp384 + 5}};static char _tmp385[6]="Times";static struct _tuple7 Cyc__gentuple_797={
1,{_tmp385,_tmp385,_tmp385 + 6}};static char _tmp386[6]="Minus";static struct _tuple7
Cyc__gentuple_798={2,{_tmp386,_tmp386,_tmp386 + 6}};static char _tmp387[4]="Div";
static struct _tuple7 Cyc__gentuple_799={3,{_tmp387,_tmp387,_tmp387 + 4}};static char
_tmp388[4]="Mod";static struct _tuple7 Cyc__gentuple_800={4,{_tmp388,_tmp388,
_tmp388 + 4}};static char _tmp389[3]="Eq";static struct _tuple7 Cyc__gentuple_801={5,{
_tmp389,_tmp389,_tmp389 + 3}};static char _tmp38A[4]="Neq";static struct _tuple7 Cyc__gentuple_802={
6,{_tmp38A,_tmp38A,_tmp38A + 4}};static char _tmp38B[3]="Gt";static struct _tuple7 Cyc__gentuple_803={
7,{_tmp38B,_tmp38B,_tmp38B + 3}};static char _tmp38C[3]="Lt";static struct _tuple7 Cyc__gentuple_804={
8,{_tmp38C,_tmp38C,_tmp38C + 3}};static char _tmp38D[4]="Gte";static struct _tuple7
Cyc__gentuple_805={9,{_tmp38D,_tmp38D,_tmp38D + 4}};static char _tmp38E[4]="Lte";
static struct _tuple7 Cyc__gentuple_806={10,{_tmp38E,_tmp38E,_tmp38E + 4}};static
char _tmp38F[4]="Not";static struct _tuple7 Cyc__gentuple_807={11,{_tmp38F,_tmp38F,
_tmp38F + 4}};static char _tmp390[7]="Bitnot";static struct _tuple7 Cyc__gentuple_808={
12,{_tmp390,_tmp390,_tmp390 + 7}};static char _tmp391[7]="Bitand";static struct
_tuple7 Cyc__gentuple_809={13,{_tmp391,_tmp391,_tmp391 + 7}};static char _tmp392[6]="Bitor";
static struct _tuple7 Cyc__gentuple_810={14,{_tmp392,_tmp392,_tmp392 + 6}};static
char _tmp393[7]="Bitxor";static struct _tuple7 Cyc__gentuple_811={15,{_tmp393,
_tmp393,_tmp393 + 7}};static char _tmp394[10]="Bitlshift";static struct _tuple7 Cyc__gentuple_812={
16,{_tmp394,_tmp394,_tmp394 + 10}};static char _tmp395[11]="Bitlrshift";static
struct _tuple7 Cyc__gentuple_813={17,{_tmp395,_tmp395,_tmp395 + 11}};static char
_tmp396[11]="Bitarshift";static struct _tuple7 Cyc__gentuple_814={18,{_tmp396,
_tmp396,_tmp396 + 11}};static char _tmp397[5]="Size";static struct _tuple7 Cyc__gentuple_815={
19,{_tmp397,_tmp397,_tmp397 + 5}};static struct _tuple7*Cyc__genarr_816[20]={& Cyc__gentuple_796,&
Cyc__gentuple_797,& Cyc__gentuple_798,& Cyc__gentuple_799,& Cyc__gentuple_800,& Cyc__gentuple_801,&
Cyc__gentuple_802,& Cyc__gentuple_803,& Cyc__gentuple_804,& Cyc__gentuple_805,& Cyc__gentuple_806,&
Cyc__gentuple_807,& Cyc__gentuple_808,& Cyc__gentuple_809,& Cyc__gentuple_810,& Cyc__gentuple_811,&
Cyc__gentuple_812,& Cyc__gentuple_813,& Cyc__gentuple_814,& Cyc__gentuple_815};
static struct _tuple5*Cyc__genarr_817[0]={};static char _tmp399[7]="Primop";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep={5,{_tmp399,_tmp399,_tmp399 + 7},{(
void*)((struct _tuple7**)Cyc__genarr_816),(void*)((struct _tuple7**)Cyc__genarr_816),(
void*)((struct _tuple7**)Cyc__genarr_816 + 20)},{(void*)((struct _tuple5**)Cyc__genarr_817),(
void*)((struct _tuple5**)Cyc__genarr_817),(void*)((struct _tuple5**)Cyc__genarr_817
+ 0)}};struct _tuple64{unsigned int f1;void*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_827={offsetof(struct _tuple64,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_828={offsetof(struct _tuple64,f2),(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple6 Cyc__gentuple_829={offsetof(struct _tuple64,f3),(void*)& Cyc__genrep_490};
static struct _tuple6*Cyc__genarr_830[3]={& Cyc__gentuple_827,& Cyc__gentuple_828,&
Cyc__gentuple_829};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_826={4,
sizeof(struct _tuple64),{(void*)((struct _tuple6**)Cyc__genarr_830),(void*)((
struct _tuple6**)Cyc__genarr_830),(void*)((struct _tuple6**)Cyc__genarr_830 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_794;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_795;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep;
static char _tmp39B[4]="Opt";static struct _tagged_arr Cyc__genname_820={_tmp39B,
_tmp39B,_tmp39B + 4};static char _tmp39C[2]="v";static struct _tuple5 Cyc__gentuple_818={
offsetof(struct Cyc_Core_Opt,v),{_tmp39C,_tmp39C,_tmp39C + 2},(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple5*Cyc__genarr_819[1]={& Cyc__gentuple_818};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_primop_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_820,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_819),(void*)((
struct _tuple5**)Cyc__genarr_819),(void*)((struct _tuple5**)Cyc__genarr_819 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_795={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_primop_t2_rep)};
struct _tuple65{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Core_Opt*f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_821={offsetof(struct
_tuple65,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_822={
offsetof(struct _tuple65,f2),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_823={
offsetof(struct _tuple65,f3),(void*)& Cyc__genrep_795};static struct _tuple6 Cyc__gentuple_824={
offsetof(struct _tuple65,f4),(void*)& Cyc__genrep_82};static struct _tuple6*Cyc__genarr_825[
4]={& Cyc__gentuple_821,& Cyc__gentuple_822,& Cyc__gentuple_823,& Cyc__gentuple_824};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_794={4,sizeof(struct _tuple65),{(
void*)((struct _tuple6**)Cyc__genarr_825),(void*)((struct _tuple6**)Cyc__genarr_825),(
void*)((struct _tuple6**)Cyc__genarr_825 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_783;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep;
static char _tmp3A0[7]="PreInc";static struct _tuple7 Cyc__gentuple_784={0,{_tmp3A0,
_tmp3A0,_tmp3A0 + 7}};static char _tmp3A1[8]="PostInc";static struct _tuple7 Cyc__gentuple_785={
1,{_tmp3A1,_tmp3A1,_tmp3A1 + 8}};static char _tmp3A2[7]="PreDec";static struct
_tuple7 Cyc__gentuple_786={2,{_tmp3A2,_tmp3A2,_tmp3A2 + 7}};static char _tmp3A3[8]="PostDec";
static struct _tuple7 Cyc__gentuple_787={3,{_tmp3A3,_tmp3A3,_tmp3A3 + 8}};static
struct _tuple7*Cyc__genarr_788[4]={& Cyc__gentuple_784,& Cyc__gentuple_785,& Cyc__gentuple_786,&
Cyc__gentuple_787};static struct _tuple5*Cyc__genarr_789[0]={};static char _tmp3A5[
12]="Incrementor";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep={5,{
_tmp3A5,_tmp3A5,_tmp3A5 + 12},{(void*)((struct _tuple7**)Cyc__genarr_788),(void*)((
struct _tuple7**)Cyc__genarr_788),(void*)((struct _tuple7**)Cyc__genarr_788 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_789),(void*)((struct _tuple5**)Cyc__genarr_789),(
void*)((struct _tuple5**)Cyc__genarr_789 + 0)}};struct _tuple66{unsigned int f1;
struct Cyc_Absyn_Exp*f2;void*f3;};static struct _tuple6 Cyc__gentuple_790={offsetof(
struct _tuple66,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_791={
offsetof(struct _tuple66,f2),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_792={
offsetof(struct _tuple66,f3),(void*)& Cyc_Absyn_incrementor_t_rep};static struct
_tuple6*Cyc__genarr_793[3]={& Cyc__gentuple_790,& Cyc__gentuple_791,& Cyc__gentuple_792};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_783={4,sizeof(struct _tuple66),{(
void*)((struct _tuple6**)Cyc__genarr_793),(void*)((struct _tuple6**)Cyc__genarr_793),(
void*)((struct _tuple6**)Cyc__genarr_793 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_777;struct _tuple67{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*
f3;struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_778={offsetof(
struct _tuple67,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_779={
offsetof(struct _tuple67,f2),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_780={
offsetof(struct _tuple67,f3),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_781={
offsetof(struct _tuple67,f4),(void*)& Cyc__genrep_82};static struct _tuple6*Cyc__genarr_782[
4]={& Cyc__gentuple_778,& Cyc__gentuple_779,& Cyc__gentuple_780,& Cyc__gentuple_781};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_777={4,sizeof(struct _tuple67),{(
void*)((struct _tuple6**)Cyc__genarr_782),(void*)((struct _tuple6**)Cyc__genarr_782),(
void*)((struct _tuple6**)Cyc__genarr_782 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_716;struct _tuple68{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*
f3;};static struct _tuple6 Cyc__gentuple_717={offsetof(struct _tuple68,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_718={offsetof(struct _tuple68,f2),(
void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_719={offsetof(struct
_tuple68,f3),(void*)& Cyc__genrep_82};static struct _tuple6*Cyc__genarr_720[3]={&
Cyc__gentuple_717,& Cyc__gentuple_718,& Cyc__gentuple_719};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_716={4,sizeof(struct _tuple68),{(void*)((struct _tuple6**)Cyc__genarr_720),(
void*)((struct _tuple6**)Cyc__genarr_720),(void*)((struct _tuple6**)Cyc__genarr_720
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_772;static struct _tuple6
Cyc__gentuple_773={offsetof(struct _tuple55,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_774={offsetof(struct _tuple55,f2),(void*)& Cyc__genrep_82};
static struct _tuple6 Cyc__gentuple_775={offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_490};
static struct _tuple6*Cyc__genarr_776[3]={& Cyc__gentuple_773,& Cyc__gentuple_774,&
Cyc__gentuple_775};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_772={4,
sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_776),(void*)((
struct _tuple6**)Cyc__genarr_776),(void*)((struct _tuple6**)Cyc__genarr_776 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_759;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_760;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_761;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_761={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};static char
_tmp3AB[15]="VarargCallInfo";static struct _tagged_arr Cyc__genname_766={_tmp3AB,
_tmp3AB,_tmp3AB + 15};static char _tmp3AC[12]="num_varargs";static struct _tuple5 Cyc__gentuple_762={
offsetof(struct Cyc_Absyn_VarargCallInfo,num_varargs),{_tmp3AC,_tmp3AC,_tmp3AC + 
12},(void*)((void*)& Cyc__genrep_62)};static char _tmp3AD[10]="injectors";static
struct _tuple5 Cyc__gentuple_763={offsetof(struct Cyc_Absyn_VarargCallInfo,injectors),{
_tmp3AD,_tmp3AD,_tmp3AD + 10},(void*)& Cyc__genrep_303};static char _tmp3AE[4]="vai";
static struct _tuple5 Cyc__gentuple_764={offsetof(struct Cyc_Absyn_VarargCallInfo,vai),{
_tmp3AE,_tmp3AE,_tmp3AE + 4},(void*)& Cyc__genrep_761};static struct _tuple5*Cyc__genarr_765[
3]={& Cyc__gentuple_762,& Cyc__gentuple_763,& Cyc__gentuple_764};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_call_info_t_rep={3,(struct _tagged_arr*)& Cyc__genname_766,
sizeof(struct Cyc_Absyn_VarargCallInfo),{(void*)((struct _tuple5**)Cyc__genarr_765),(
void*)((struct _tuple5**)Cyc__genarr_765),(void*)((struct _tuple5**)Cyc__genarr_765
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_760={1,1,(void*)((void*)&
Cyc_Absyn_vararg_call_info_t_rep)};struct _tuple69{unsigned int f1;struct Cyc_Absyn_Exp*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_VarargCallInfo*f4;};static struct
_tuple6 Cyc__gentuple_767={offsetof(struct _tuple69,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_768={offsetof(struct _tuple69,f2),(void*)& Cyc__genrep_82};
static struct _tuple6 Cyc__gentuple_769={offsetof(struct _tuple69,f3),(void*)& Cyc__genrep_490};
static struct _tuple6 Cyc__gentuple_770={offsetof(struct _tuple69,f4),(void*)& Cyc__genrep_760};
static struct _tuple6*Cyc__genarr_771[4]={& Cyc__gentuple_767,& Cyc__gentuple_768,&
Cyc__gentuple_769,& Cyc__gentuple_770};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_759={
4,sizeof(struct _tuple69),{(void*)((struct _tuple6**)Cyc__genarr_771),(void*)((
struct _tuple6**)Cyc__genarr_771),(void*)((struct _tuple6**)Cyc__genarr_771 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_754;static struct _tuple6 Cyc__gentuple_755={
offsetof(struct _tuple55,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_756={
offsetof(struct _tuple55,f2),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_757={
offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_53};static struct _tuple6*Cyc__genarr_758[
3]={& Cyc__gentuple_755,& Cyc__gentuple_756,& Cyc__gentuple_757};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_754={4,sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_758),(
void*)((struct _tuple6**)Cyc__genarr_758),(void*)((struct _tuple6**)Cyc__genarr_758
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_749;struct _tuple70{
unsigned int f1;void*f2;struct Cyc_Absyn_Exp*f3;};static struct _tuple6 Cyc__gentuple_750={
offsetof(struct _tuple70,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_751={
offsetof(struct _tuple70,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_752={offsetof(struct _tuple70,f3),(void*)& Cyc__genrep_82};
static struct _tuple6*Cyc__genarr_753[3]={& Cyc__gentuple_750,& Cyc__gentuple_751,&
Cyc__gentuple_752};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_749={4,
sizeof(struct _tuple70),{(void*)((struct _tuple6**)Cyc__genarr_753),(void*)((
struct _tuple6**)Cyc__genarr_753),(void*)((struct _tuple6**)Cyc__genarr_753 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_744;static struct _tuple6 Cyc__gentuple_745={
offsetof(struct _tuple68,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_746={
offsetof(struct _tuple68,f2),(void*)& Cyc__genrep_78};static struct _tuple6 Cyc__gentuple_747={
offsetof(struct _tuple68,f3),(void*)& Cyc__genrep_82};static struct _tuple6*Cyc__genarr_748[
3]={& Cyc__gentuple_745,& Cyc__gentuple_746,& Cyc__gentuple_747};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_744={4,sizeof(struct _tuple68),{(void*)((struct _tuple6**)Cyc__genarr_748),(
void*)((struct _tuple6**)Cyc__genarr_748),(void*)((struct _tuple6**)Cyc__genarr_748
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_48;static struct _tuple6 Cyc__gentuple_49={
offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_50={
offsetof(struct _tuple6,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_51[2]={& Cyc__gentuple_49,& Cyc__gentuple_50};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_48={4,sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_51),(
void*)((struct _tuple6**)Cyc__genarr_51),(void*)((struct _tuple6**)Cyc__genarr_51 + 
2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_731;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_offsetof_field_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_733;
struct _tuple71{unsigned int f1;unsigned int f2;};static struct _tuple6 Cyc__gentuple_734={
offsetof(struct _tuple71,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_735={
offsetof(struct _tuple71,f2),(void*)& Cyc__genrep_5};static struct _tuple6*Cyc__genarr_736[
2]={& Cyc__gentuple_734,& Cyc__gentuple_735};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_733={4,sizeof(struct _tuple71),{(void*)((struct _tuple6**)Cyc__genarr_736),(
void*)((struct _tuple6**)Cyc__genarr_736),(void*)((struct _tuple6**)Cyc__genarr_736
+ 2)}};static struct _tuple7*Cyc__genarr_732[0]={};static char _tmp3B7[12]="StructField";
static struct _tuple5 Cyc__gentuple_737={0,{_tmp3B7,_tmp3B7,_tmp3B7 + 12},(void*)&
Cyc__genrep_334};static char _tmp3B8[11]="TupleIndex";static struct _tuple5 Cyc__gentuple_738={
1,{_tmp3B8,_tmp3B8,_tmp3B8 + 11},(void*)& Cyc__genrep_733};static struct _tuple5*Cyc__genarr_739[
2]={& Cyc__gentuple_737,& Cyc__gentuple_738};static char _tmp3BA[14]="OffsetofField";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_offsetof_field_t_rep={5,{_tmp3BA,
_tmp3BA,_tmp3BA + 14},{(void*)((struct _tuple7**)Cyc__genarr_732),(void*)((struct
_tuple7**)Cyc__genarr_732),(void*)((struct _tuple7**)Cyc__genarr_732 + 0)},{(void*)((
struct _tuple5**)Cyc__genarr_739),(void*)((struct _tuple5**)Cyc__genarr_739),(void*)((
struct _tuple5**)Cyc__genarr_739 + 2)}};static struct _tuple6 Cyc__gentuple_740={
offsetof(struct _tuple28,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_741={
offsetof(struct _tuple28,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_742={offsetof(struct _tuple28,f3),(void*)& Cyc_Absyn_offsetof_field_t_rep};
static struct _tuple6*Cyc__genarr_743[3]={& Cyc__gentuple_740,& Cyc__gentuple_741,&
Cyc__gentuple_742};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_731={4,
sizeof(struct _tuple28),{(void*)((struct _tuple6**)Cyc__genarr_743),(void*)((
struct _tuple6**)Cyc__genarr_743),(void*)((struct _tuple6**)Cyc__genarr_743 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_726;struct _tuple72{unsigned int
f1;struct Cyc_List_List*f2;void*f3;};static struct _tuple6 Cyc__gentuple_727={
offsetof(struct _tuple72,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_728={
offsetof(struct _tuple72,f2),(void*)& Cyc__genrep_311};static struct _tuple6 Cyc__gentuple_729={
offsetof(struct _tuple72,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_730[3]={& Cyc__gentuple_727,& Cyc__gentuple_728,& Cyc__gentuple_729};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_726={4,sizeof(struct _tuple72),{(
void*)((struct _tuple6**)Cyc__genarr_730),(void*)((struct _tuple6**)Cyc__genarr_730),(
void*)((struct _tuple6**)Cyc__genarr_730 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_721;struct _tuple73{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct
_tagged_arr*f3;};static struct _tuple6 Cyc__gentuple_722={offsetof(struct _tuple73,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_723={offsetof(struct
_tuple73,f2),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_724={
offsetof(struct _tuple73,f3),(void*)& Cyc__genrep_12};static struct _tuple6*Cyc__genarr_725[
3]={& Cyc__gentuple_722,& Cyc__gentuple_723,& Cyc__gentuple_724};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_721={4,sizeof(struct _tuple73),{(void*)((struct _tuple6**)Cyc__genarr_725),(
void*)((struct _tuple6**)Cyc__genarr_725),(void*)((struct _tuple6**)Cyc__genarr_725
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_712;static struct _tuple6
Cyc__gentuple_713={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_714={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_490};
static struct _tuple6*Cyc__genarr_715[2]={& Cyc__gentuple_713,& Cyc__gentuple_714};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_712={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_715),(void*)((struct _tuple6**)Cyc__genarr_715),(
void*)((struct _tuple6**)Cyc__genarr_715 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_701;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_702;extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_703;static struct _tuple6 Cyc__gentuple_704={
offsetof(struct _tuple1,f1),(void*)& Cyc__genrep_587};static struct _tuple6 Cyc__gentuple_705={
offsetof(struct _tuple1,f2),(void*)& Cyc__genrep_134};static struct _tuple6 Cyc__gentuple_706={
offsetof(struct _tuple1,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_707[3]={& Cyc__gentuple_704,& Cyc__gentuple_705,& Cyc__gentuple_706};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_703={4,sizeof(struct _tuple1),{(
void*)((struct _tuple6**)Cyc__genarr_707),(void*)((struct _tuple6**)Cyc__genarr_707),(
void*)((struct _tuple6**)Cyc__genarr_707 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_702={1,1,(void*)((void*)& Cyc__genrep_703)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_628;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_629;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_630;static struct _tuple6 Cyc__gentuple_631={offsetof(struct _tuple12,f1),(
void*)& Cyc__genrep_332};static struct _tuple6 Cyc__gentuple_632={offsetof(struct
_tuple12,f2),(void*)& Cyc__genrep_82};static struct _tuple6*Cyc__genarr_633[2]={&
Cyc__gentuple_631,& Cyc__gentuple_632};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_630={
4,sizeof(struct _tuple12),{(void*)((struct _tuple6**)Cyc__genarr_633),(void*)((
struct _tuple6**)Cyc__genarr_633),(void*)((struct _tuple6**)Cyc__genarr_633 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_629={1,1,(void*)((void*)& Cyc__genrep_630)};
static char _tmp3C3[5]="List";static struct _tagged_arr Cyc__genname_637={_tmp3C3,
_tmp3C3,_tmp3C3 + 5};static char _tmp3C4[3]="hd";static struct _tuple5 Cyc__gentuple_634={
offsetof(struct Cyc_List_List,hd),{_tmp3C4,_tmp3C4,_tmp3C4 + 3},(void*)& Cyc__genrep_629};
static char _tmp3C5[3]="tl";static struct _tuple5 Cyc__gentuple_635={offsetof(struct
Cyc_List_List,tl),{_tmp3C5,_tmp3C5,_tmp3C5 + 3},(void*)& Cyc__genrep_628};static
struct _tuple5*Cyc__genarr_636[2]={& Cyc__gentuple_634,& Cyc__gentuple_635};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_637,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_636),(void*)((struct _tuple5**)Cyc__genarr_636),(void*)((
struct _tuple5**)Cyc__genarr_636 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_628={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep)};
struct _tuple74{unsigned int f1;struct _tuple1*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_708={offsetof(struct _tuple74,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_709={offsetof(struct _tuple74,f2),(void*)& Cyc__genrep_702};
static struct _tuple6 Cyc__gentuple_710={offsetof(struct _tuple74,f3),(void*)& Cyc__genrep_628};
static struct _tuple6*Cyc__genarr_711[3]={& Cyc__gentuple_708,& Cyc__gentuple_709,&
Cyc__gentuple_710};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_701={4,
sizeof(struct _tuple74),{(void*)((struct _tuple6**)Cyc__genarr_711),(void*)((
struct _tuple6**)Cyc__genarr_711),(void*)((struct _tuple6**)Cyc__genarr_711 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_697;static struct _tuple6 Cyc__gentuple_698={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_699={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_628};static struct _tuple6*Cyc__genarr_700[
2]={& Cyc__gentuple_698,& Cyc__gentuple_699};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_697={4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_700),(
void*)((struct _tuple6**)Cyc__genarr_700),(void*)((struct _tuple6**)Cyc__genarr_700
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_690;struct _tuple75{
unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;int f5;};static struct _tuple6 Cyc__gentuple_691={offsetof(struct _tuple75,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_692={offsetof(struct
_tuple75,f2),(void*)& Cyc__genrep_133};static struct _tuple6 Cyc__gentuple_693={
offsetof(struct _tuple75,f3),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_694={
offsetof(struct _tuple75,f4),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_695={
offsetof(struct _tuple75,f5),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6*
Cyc__genarr_696[5]={& Cyc__gentuple_691,& Cyc__gentuple_692,& Cyc__gentuple_693,&
Cyc__gentuple_694,& Cyc__gentuple_695};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_690={
4,sizeof(struct _tuple75),{(void*)((struct _tuple6**)Cyc__genarr_696),(void*)((
struct _tuple6**)Cyc__genarr_696),(void*)((struct _tuple6**)Cyc__genarr_696 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_682;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_683;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_683={1,1,(void*)((
void*)& Cyc_struct_Absyn_Aggrdecl_rep)};struct _tuple76{unsigned int f1;struct
_tuple0*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_Absyn_Aggrdecl*
f5;};static struct _tuple6 Cyc__gentuple_684={offsetof(struct _tuple76,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_685={offsetof(struct _tuple76,f2),(
void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_686={offsetof(struct
_tuple76,f3),(void*)& Cyc__genrep_53};static struct _tuple6 Cyc__gentuple_687={
offsetof(struct _tuple76,f4),(void*)& Cyc__genrep_628};static struct _tuple6 Cyc__gentuple_688={
offsetof(struct _tuple76,f5),(void*)& Cyc__genrep_683};static struct _tuple6*Cyc__genarr_689[
5]={& Cyc__gentuple_684,& Cyc__gentuple_685,& Cyc__gentuple_686,& Cyc__gentuple_687,&
Cyc__gentuple_688};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_682={4,
sizeof(struct _tuple76),{(void*)((struct _tuple6**)Cyc__genarr_689),(void*)((
struct _tuple6**)Cyc__genarr_689),(void*)((struct _tuple6**)Cyc__genarr_689 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_677;static struct _tuple6 Cyc__gentuple_678={
offsetof(struct _tuple64,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_679={
offsetof(struct _tuple64,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_680={offsetof(struct _tuple64,f3),(void*)& Cyc__genrep_628};
static struct _tuple6*Cyc__genarr_681[3]={& Cyc__gentuple_678,& Cyc__gentuple_679,&
Cyc__gentuple_680};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_677={4,
sizeof(struct _tuple64),{(void*)((struct _tuple6**)Cyc__genarr_681),(void*)((
struct _tuple6**)Cyc__genarr_681),(void*)((struct _tuple6**)Cyc__genarr_681 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_671;struct _tuple77{unsigned int
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Tuniondecl*f3;struct Cyc_Absyn_Tunionfield*
f4;};static struct _tuple6 Cyc__gentuple_672={offsetof(struct _tuple77,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_673={offsetof(struct _tuple77,f2),(
void*)& Cyc__genrep_490};static struct _tuple6 Cyc__gentuple_674={offsetof(struct
_tuple77,f3),(void*)((void*)& Cyc__genrep_301)};static struct _tuple6 Cyc__gentuple_675={
offsetof(struct _tuple77,f4),(void*)& Cyc__genrep_284};static struct _tuple6*Cyc__genarr_676[
4]={& Cyc__gentuple_672,& Cyc__gentuple_673,& Cyc__gentuple_674,& Cyc__gentuple_675};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_671={4,sizeof(struct _tuple77),{(
void*)((struct _tuple6**)Cyc__genarr_676),(void*)((struct _tuple6**)Cyc__genarr_676),(
void*)((struct _tuple6**)Cyc__genarr_676 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_664;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_665;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_665={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_658;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_658={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};struct
_tuple78{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_666={offsetof(struct _tuple78,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_667={offsetof(struct _tuple78,f2),(
void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_668={offsetof(struct
_tuple78,f3),(void*)& Cyc__genrep_665};static struct _tuple6 Cyc__gentuple_669={
offsetof(struct _tuple78,f4),(void*)& Cyc__genrep_658};static struct _tuple6*Cyc__genarr_670[
4]={& Cyc__gentuple_666,& Cyc__gentuple_667,& Cyc__gentuple_668,& Cyc__gentuple_669};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_664={4,sizeof(struct _tuple78),{(
void*)((struct _tuple6**)Cyc__genarr_670),(void*)((struct _tuple6**)Cyc__genarr_670),(
void*)((struct _tuple6**)Cyc__genarr_670 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_657;struct _tuple79{unsigned int f1;struct _tuple0*f2;void*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_659={offsetof(struct _tuple79,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_660={offsetof(struct _tuple79,f2),(
void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_661={offsetof(struct
_tuple79,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_662={
offsetof(struct _tuple79,f4),(void*)& Cyc__genrep_658};static struct _tuple6*Cyc__genarr_663[
4]={& Cyc__gentuple_659,& Cyc__gentuple_660,& Cyc__gentuple_661,& Cyc__gentuple_662};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_657={4,sizeof(struct _tuple79),{(
void*)((struct _tuple6**)Cyc__genarr_663),(void*)((struct _tuple6**)Cyc__genarr_663),(
void*)((struct _tuple6**)Cyc__genarr_663 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_646;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_67;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_67={1,1,(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp3D4[11]="MallocInfo";
static struct _tagged_arr Cyc__genname_653={_tmp3D4,_tmp3D4,_tmp3D4 + 11};static char
_tmp3D5[10]="is_calloc";static struct _tuple5 Cyc__gentuple_647={offsetof(struct Cyc_Absyn_MallocInfo,is_calloc),{
_tmp3D5,_tmp3D5,_tmp3D5 + 10},(void*)((void*)& Cyc__genrep_62)};static char _tmp3D6[
4]="rgn";static struct _tuple5 Cyc__gentuple_648={offsetof(struct Cyc_Absyn_MallocInfo,rgn),{
_tmp3D6,_tmp3D6,_tmp3D6 + 4},(void*)& Cyc__genrep_78};static char _tmp3D7[9]="elt_type";
static struct _tuple5 Cyc__gentuple_649={offsetof(struct Cyc_Absyn_MallocInfo,elt_type),{
_tmp3D7,_tmp3D7,_tmp3D7 + 9},(void*)& Cyc__genrep_67};static char _tmp3D8[9]="num_elts";
static struct _tuple5 Cyc__gentuple_650={offsetof(struct Cyc_Absyn_MallocInfo,num_elts),{
_tmp3D8,_tmp3D8,_tmp3D8 + 9},(void*)& Cyc__genrep_82};static char _tmp3D9[11]="fat_result";
static struct _tuple5 Cyc__gentuple_651={offsetof(struct Cyc_Absyn_MallocInfo,fat_result),{
_tmp3D9,_tmp3D9,_tmp3D9 + 11},(void*)((void*)& Cyc__genrep_62)};static struct
_tuple5*Cyc__genarr_652[5]={& Cyc__gentuple_647,& Cyc__gentuple_648,& Cyc__gentuple_649,&
Cyc__gentuple_650,& Cyc__gentuple_651};struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_653,sizeof(struct Cyc_Absyn_MallocInfo),{(
void*)((struct _tuple5**)Cyc__genarr_652),(void*)((struct _tuple5**)Cyc__genarr_652),(
void*)((struct _tuple5**)Cyc__genarr_652 + 5)}};struct _tuple80{unsigned int f1;
struct Cyc_Absyn_MallocInfo f2;};static struct _tuple6 Cyc__gentuple_654={offsetof(
struct _tuple80,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_655={
offsetof(struct _tuple80,f2),(void*)& Cyc_Absyn_malloc_info_t_rep};static struct
_tuple6*Cyc__genarr_656[2]={& Cyc__gentuple_654,& Cyc__gentuple_655};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_646={4,sizeof(struct _tuple80),{(void*)((
struct _tuple6**)Cyc__genarr_656),(void*)((struct _tuple6**)Cyc__genarr_656),(void*)((
struct _tuple6**)Cyc__genarr_656 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_627;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_638;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep;static char _tmp3DC[4]="Opt";static
struct _tagged_arr Cyc__genname_641={_tmp3DC,_tmp3DC,_tmp3DC + 4};static char _tmp3DD[
2]="v";static struct _tuple5 Cyc__gentuple_639={offsetof(struct Cyc_Core_Opt,v),{
_tmp3DD,_tmp3DD,_tmp3DD + 2},(void*)& Cyc__genrep_10};static struct _tuple5*Cyc__genarr_640[
1]={& Cyc__gentuple_639};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep={
3,(struct _tagged_arr*)& Cyc__genname_641,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_640),(void*)((struct _tuple5**)Cyc__genarr_640),(void*)((
struct _tuple5**)Cyc__genarr_640 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_638={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep)};struct
_tuple81{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_642={offsetof(struct _tuple81,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_643={offsetof(struct _tuple81,f2),(void*)& Cyc__genrep_638};
static struct _tuple6 Cyc__gentuple_644={offsetof(struct _tuple81,f3),(void*)& Cyc__genrep_628};
static struct _tuple6*Cyc__genarr_645[3]={& Cyc__gentuple_642,& Cyc__gentuple_643,&
Cyc__gentuple_644};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_627={4,
sizeof(struct _tuple81),{(void*)((struct _tuple6**)Cyc__genarr_645),(void*)((
struct _tuple6**)Cyc__genarr_645),(void*)((struct _tuple6**)Cyc__genarr_645 + 3)}};
static struct _tuple7*Cyc__genarr_80[0]={};static char _tmp3E1[8]="Const_e";static
struct _tuple5 Cyc__gentuple_874={0,{_tmp3E1,_tmp3E1,_tmp3E1 + 8},(void*)& Cyc__genrep_844};
static char _tmp3E2[6]="Var_e";static struct _tuple5 Cyc__gentuple_875={1,{_tmp3E2,
_tmp3E2,_tmp3E2 + 6},(void*)& Cyc__genrep_831};static char _tmp3E3[12]="UnknownId_e";
static struct _tuple5 Cyc__gentuple_876={2,{_tmp3E3,_tmp3E3,_tmp3E3 + 12},(void*)&
Cyc__genrep_245};static char _tmp3E4[9]="Primop_e";static struct _tuple5 Cyc__gentuple_877={
3,{_tmp3E4,_tmp3E4,_tmp3E4 + 9},(void*)& Cyc__genrep_826};static char _tmp3E5[11]="AssignOp_e";
static struct _tuple5 Cyc__gentuple_878={4,{_tmp3E5,_tmp3E5,_tmp3E5 + 11},(void*)&
Cyc__genrep_794};static char _tmp3E6[12]="Increment_e";static struct _tuple5 Cyc__gentuple_879={
5,{_tmp3E6,_tmp3E6,_tmp3E6 + 12},(void*)& Cyc__genrep_783};static char _tmp3E7[14]="Conditional_e";
static struct _tuple5 Cyc__gentuple_880={6,{_tmp3E7,_tmp3E7,_tmp3E7 + 14},(void*)&
Cyc__genrep_777};static char _tmp3E8[9]="SeqExp_e";static struct _tuple5 Cyc__gentuple_881={
7,{_tmp3E8,_tmp3E8,_tmp3E8 + 9},(void*)& Cyc__genrep_716};static char _tmp3E9[14]="UnknownCall_e";
static struct _tuple5 Cyc__gentuple_882={8,{_tmp3E9,_tmp3E9,_tmp3E9 + 14},(void*)&
Cyc__genrep_772};static char _tmp3EA[9]="FnCall_e";static struct _tuple5 Cyc__gentuple_883={
9,{_tmp3EA,_tmp3EA,_tmp3EA + 9},(void*)& Cyc__genrep_759};static char _tmp3EB[8]="Throw_e";
static struct _tuple5 Cyc__gentuple_884={10,{_tmp3EB,_tmp3EB,_tmp3EB + 8},(void*)&
Cyc__genrep_81};static char _tmp3EC[16]="NoInstantiate_e";static struct _tuple5 Cyc__gentuple_885={
11,{_tmp3EC,_tmp3EC,_tmp3EC + 16},(void*)& Cyc__genrep_81};static char _tmp3ED[14]="Instantiate_e";
static struct _tuple5 Cyc__gentuple_886={12,{_tmp3ED,_tmp3ED,_tmp3ED + 14},(void*)&
Cyc__genrep_754};static char _tmp3EE[7]="Cast_e";static struct _tuple5 Cyc__gentuple_887={
13,{_tmp3EE,_tmp3EE,_tmp3EE + 7},(void*)& Cyc__genrep_749};static char _tmp3EF[10]="Address_e";
static struct _tuple5 Cyc__gentuple_888={14,{_tmp3EF,_tmp3EF,_tmp3EF + 10},(void*)&
Cyc__genrep_81};static char _tmp3F0[6]="New_e";static struct _tuple5 Cyc__gentuple_889={
15,{_tmp3F0,_tmp3F0,_tmp3F0 + 6},(void*)& Cyc__genrep_744};static char _tmp3F1[12]="Sizeoftyp_e";
static struct _tuple5 Cyc__gentuple_890={16,{_tmp3F1,_tmp3F1,_tmp3F1 + 12},(void*)&
Cyc__genrep_48};static char _tmp3F2[12]="Sizeofexp_e";static struct _tuple5 Cyc__gentuple_891={
17,{_tmp3F2,_tmp3F2,_tmp3F2 + 12},(void*)& Cyc__genrep_81};static char _tmp3F3[11]="Offsetof_e";
static struct _tuple5 Cyc__gentuple_892={18,{_tmp3F3,_tmp3F3,_tmp3F3 + 11},(void*)&
Cyc__genrep_731};static char _tmp3F4[9]="Gentyp_e";static struct _tuple5 Cyc__gentuple_893={
19,{_tmp3F4,_tmp3F4,_tmp3F4 + 9},(void*)& Cyc__genrep_726};static char _tmp3F5[8]="Deref_e";
static struct _tuple5 Cyc__gentuple_894={20,{_tmp3F5,_tmp3F5,_tmp3F5 + 8},(void*)&
Cyc__genrep_81};static char _tmp3F6[13]="AggrMember_e";static struct _tuple5 Cyc__gentuple_895={
21,{_tmp3F6,_tmp3F6,_tmp3F6 + 13},(void*)& Cyc__genrep_721};static char _tmp3F7[12]="AggrArrow_e";
static struct _tuple5 Cyc__gentuple_896={22,{_tmp3F7,_tmp3F7,_tmp3F7 + 12},(void*)&
Cyc__genrep_721};static char _tmp3F8[12]="Subscript_e";static struct _tuple5 Cyc__gentuple_897={
23,{_tmp3F8,_tmp3F8,_tmp3F8 + 12},(void*)& Cyc__genrep_716};static char _tmp3F9[8]="Tuple_e";
static struct _tuple5 Cyc__gentuple_898={24,{_tmp3F9,_tmp3F9,_tmp3F9 + 8},(void*)&
Cyc__genrep_712};static char _tmp3FA[14]="CompoundLit_e";static struct _tuple5 Cyc__gentuple_899={
25,{_tmp3FA,_tmp3FA,_tmp3FA + 14},(void*)& Cyc__genrep_701};static char _tmp3FB[8]="Array_e";
static struct _tuple5 Cyc__gentuple_900={26,{_tmp3FB,_tmp3FB,_tmp3FB + 8},(void*)&
Cyc__genrep_697};static char _tmp3FC[16]="Comprehension_e";static struct _tuple5 Cyc__gentuple_901={
27,{_tmp3FC,_tmp3FC,_tmp3FC + 16},(void*)& Cyc__genrep_690};static char _tmp3FD[9]="Struct_e";
static struct _tuple5 Cyc__gentuple_902={28,{_tmp3FD,_tmp3FD,_tmp3FD + 9},(void*)&
Cyc__genrep_682};static char _tmp3FE[13]="AnonStruct_e";static struct _tuple5 Cyc__gentuple_903={
29,{_tmp3FE,_tmp3FE,_tmp3FE + 13},(void*)& Cyc__genrep_677};static char _tmp3FF[9]="Tunion_e";
static struct _tuple5 Cyc__gentuple_904={30,{_tmp3FF,_tmp3FF,_tmp3FF + 9},(void*)&
Cyc__genrep_671};static char _tmp400[7]="Enum_e";static struct _tuple5 Cyc__gentuple_905={
31,{_tmp400,_tmp400,_tmp400 + 7},(void*)& Cyc__genrep_664};static char _tmp401[11]="AnonEnum_e";
static struct _tuple5 Cyc__gentuple_906={32,{_tmp401,_tmp401,_tmp401 + 11},(void*)&
Cyc__genrep_657};static char _tmp402[9]="Malloc_e";static struct _tuple5 Cyc__gentuple_907={
33,{_tmp402,_tmp402,_tmp402 + 9},(void*)& Cyc__genrep_646};static char _tmp403[16]="UnresolvedMem_e";
static struct _tuple5 Cyc__gentuple_908={34,{_tmp403,_tmp403,_tmp403 + 16},(void*)&
Cyc__genrep_627};static char _tmp404[10]="StmtExp_e";static struct _tuple5 Cyc__gentuple_909={
35,{_tmp404,_tmp404,_tmp404 + 10},(void*)& Cyc__genrep_479};static char _tmp405[10]="Codegen_e";
static struct _tuple5 Cyc__gentuple_910={36,{_tmp405,_tmp405,_tmp405 + 10},(void*)&
Cyc__genrep_86};static char _tmp406[7]="Fill_e";static struct _tuple5 Cyc__gentuple_911={
37,{_tmp406,_tmp406,_tmp406 + 7},(void*)& Cyc__genrep_81};static struct _tuple5*Cyc__genarr_912[
38]={& Cyc__gentuple_874,& Cyc__gentuple_875,& Cyc__gentuple_876,& Cyc__gentuple_877,&
Cyc__gentuple_878,& Cyc__gentuple_879,& Cyc__gentuple_880,& Cyc__gentuple_881,& Cyc__gentuple_882,&
Cyc__gentuple_883,& Cyc__gentuple_884,& Cyc__gentuple_885,& Cyc__gentuple_886,& Cyc__gentuple_887,&
Cyc__gentuple_888,& Cyc__gentuple_889,& Cyc__gentuple_890,& Cyc__gentuple_891,& Cyc__gentuple_892,&
Cyc__gentuple_893,& Cyc__gentuple_894,& Cyc__gentuple_895,& Cyc__gentuple_896,& Cyc__gentuple_897,&
Cyc__gentuple_898,& Cyc__gentuple_899,& Cyc__gentuple_900,& Cyc__gentuple_901,& Cyc__gentuple_902,&
Cyc__gentuple_903,& Cyc__gentuple_904,& Cyc__gentuple_905,& Cyc__gentuple_906,& Cyc__gentuple_907,&
Cyc__gentuple_908,& Cyc__gentuple_909,& Cyc__gentuple_910,& Cyc__gentuple_911};
static char _tmp408[8]="Raw_exp";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep={
5,{_tmp408,_tmp408,_tmp408 + 8},{(void*)((struct _tuple7**)Cyc__genarr_80),(void*)((
struct _tuple7**)Cyc__genarr_80),(void*)((struct _tuple7**)Cyc__genarr_80 + 0)},{(
void*)((struct _tuple5**)Cyc__genarr_912),(void*)((struct _tuple5**)Cyc__genarr_912),(
void*)((struct _tuple5**)Cyc__genarr_912 + 38)}};static char _tmp409[4]="Exp";static
struct _tagged_arr Cyc__genname_918={_tmp409,_tmp409,_tmp409 + 4};static char _tmp40A[
5]="topt";static struct _tuple5 Cyc__gentuple_913={offsetof(struct Cyc_Absyn_Exp,topt),{
_tmp40A,_tmp40A,_tmp40A + 5},(void*)& Cyc__genrep_43};static char _tmp40B[2]="r";
static struct _tuple5 Cyc__gentuple_914={offsetof(struct Cyc_Absyn_Exp,r),{_tmp40B,
_tmp40B,_tmp40B + 2},(void*)& Cyc_Absyn_raw_exp_t_rep};static char _tmp40C[4]="loc";
static struct _tuple5 Cyc__gentuple_915={offsetof(struct Cyc_Absyn_Exp,loc),{_tmp40C,
_tmp40C,_tmp40C + 4},(void*)& Cyc__genrep_2};static char _tmp40D[6]="annot";static
struct _tuple5 Cyc__gentuple_916={offsetof(struct Cyc_Absyn_Exp,annot),{_tmp40D,
_tmp40D,_tmp40D + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};static struct _tuple5*Cyc__genarr_917[
4]={& Cyc__gentuple_913,& Cyc__gentuple_914,& Cyc__gentuple_915,& Cyc__gentuple_916};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Exp_rep={3,(struct _tagged_arr*)&
Cyc__genname_918,sizeof(struct Cyc_Absyn_Exp),{(void*)((struct _tuple5**)Cyc__genarr_917),(
void*)((struct _tuple5**)Cyc__genarr_917),(void*)((struct _tuple5**)Cyc__genarr_917
+ 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_78={1,1,(void*)((void*)&
Cyc_struct_Absyn_Exp_rep)};static char _tmp410[10]="ArrayInfo";static struct
_tagged_arr Cyc__genname_987={_tmp410,_tmp410,_tmp410 + 10};static char _tmp411[9]="elt_type";
static struct _tuple5 Cyc__gentuple_982={offsetof(struct Cyc_Absyn_ArrayInfo,elt_type),{
_tmp411,_tmp411,_tmp411 + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char
_tmp412[3]="tq";static struct _tuple5 Cyc__gentuple_983={offsetof(struct Cyc_Absyn_ArrayInfo,tq),{
_tmp412,_tmp412,_tmp412 + 3},(void*)& Cyc__genrep_134};static char _tmp413[9]="num_elts";
static struct _tuple5 Cyc__gentuple_984={offsetof(struct Cyc_Absyn_ArrayInfo,num_elts),{
_tmp413,_tmp413,_tmp413 + 9},(void*)& Cyc__genrep_78};static char _tmp414[10]="zero_term";
static struct _tuple5 Cyc__gentuple_985={offsetof(struct Cyc_Absyn_ArrayInfo,zero_term),{
_tmp414,_tmp414,_tmp414 + 10},(void*)& Cyc__genrep_969};static struct _tuple5*Cyc__genarr_986[
4]={& Cyc__gentuple_982,& Cyc__gentuple_983,& Cyc__gentuple_984,& Cyc__gentuple_985};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_array_info_t_rep={3,(struct _tagged_arr*)&
Cyc__genname_987,sizeof(struct Cyc_Absyn_ArrayInfo),{(void*)((struct _tuple5**)Cyc__genarr_986),(
void*)((struct _tuple5**)Cyc__genarr_986),(void*)((struct _tuple5**)Cyc__genarr_986
+ 4)}};struct _tuple82{unsigned int f1;struct Cyc_Absyn_ArrayInfo f2;};static struct
_tuple6 Cyc__gentuple_988={offsetof(struct _tuple82,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_989={offsetof(struct _tuple82,f2),(void*)& Cyc_Absyn_array_info_t_rep};
static struct _tuple6*Cyc__genarr_990[2]={& Cyc__gentuple_988,& Cyc__gentuple_989};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_968={4,sizeof(struct _tuple82),{(
void*)((struct _tuple6**)Cyc__genarr_990),(void*)((struct _tuple6**)Cyc__genarr_990),(
void*)((struct _tuple6**)Cyc__genarr_990 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_949;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_950;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep;
static char _tmp417[5]="List";static struct _tagged_arr Cyc__genname_954={_tmp417,
_tmp417,_tmp417 + 5};static char _tmp418[3]="hd";static struct _tuple5 Cyc__gentuple_951={
offsetof(struct Cyc_List_List,hd),{_tmp418,_tmp418,_tmp418 + 3},(void*)& Cyc__genrep_702};
static char _tmp419[3]="tl";static struct _tuple5 Cyc__gentuple_952={offsetof(struct
Cyc_List_List,tl),{_tmp419,_tmp419,_tmp419 + 3},(void*)& Cyc__genrep_950};static
struct _tuple5*Cyc__genarr_953[2]={& Cyc__gentuple_951,& Cyc__gentuple_952};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_954,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_953),(void*)((struct _tuple5**)Cyc__genarr_953),(void*)((
struct _tuple5**)Cyc__genarr_953 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_950={
1,1,(void*)((void*)& Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep)};
static char _tmp41C[7]="FnInfo";static struct _tagged_arr Cyc__genname_964={_tmp41C,
_tmp41C,_tmp41C + 7};static char _tmp41D[6]="tvars";static struct _tuple5 Cyc__gentuple_955={
offsetof(struct Cyc_Absyn_FnInfo,tvars),{_tmp41D,_tmp41D,_tmp41D + 6},(void*)& Cyc__genrep_311};
static char _tmp41E[7]="effect";static struct _tuple5 Cyc__gentuple_956={offsetof(
struct Cyc_Absyn_FnInfo,effect),{_tmp41E,_tmp41E,_tmp41E + 7},(void*)& Cyc__genrep_43};
static char _tmp41F[8]="ret_typ";static struct _tuple5 Cyc__gentuple_957={offsetof(
struct Cyc_Absyn_FnInfo,ret_typ),{_tmp41F,_tmp41F,_tmp41F + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp420[5]="args";static struct _tuple5 Cyc__gentuple_958={offsetof(
struct Cyc_Absyn_FnInfo,args),{_tmp420,_tmp420,_tmp420 + 5},(void*)& Cyc__genrep_950};
static char _tmp421[10]="c_varargs";static struct _tuple5 Cyc__gentuple_959={
offsetof(struct Cyc_Absyn_FnInfo,c_varargs),{_tmp421,_tmp421,_tmp421 + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp422[12]="cyc_varargs";static struct _tuple5
Cyc__gentuple_960={offsetof(struct Cyc_Absyn_FnInfo,cyc_varargs),{_tmp422,_tmp422,
_tmp422 + 12},(void*)& Cyc__genrep_586};static char _tmp423[7]="rgn_po";static struct
_tuple5 Cyc__gentuple_961={offsetof(struct Cyc_Absyn_FnInfo,rgn_po),{_tmp423,
_tmp423,_tmp423 + 7},(void*)& Cyc__genrep_370};static char _tmp424[11]="attributes";
static struct _tuple5 Cyc__gentuple_962={offsetof(struct Cyc_Absyn_FnInfo,attributes),{
_tmp424,_tmp424,_tmp424 + 11},(void*)& Cyc__genrep_88};static struct _tuple5*Cyc__genarr_963[
8]={& Cyc__gentuple_955,& Cyc__gentuple_956,& Cyc__gentuple_957,& Cyc__gentuple_958,&
Cyc__gentuple_959,& Cyc__gentuple_960,& Cyc__gentuple_961,& Cyc__gentuple_962};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep={3,(struct _tagged_arr*)&
Cyc__genname_964,sizeof(struct Cyc_Absyn_FnInfo),{(void*)((struct _tuple5**)Cyc__genarr_963),(
void*)((struct _tuple5**)Cyc__genarr_963),(void*)((struct _tuple5**)Cyc__genarr_963
+ 8)}};struct _tuple83{unsigned int f1;struct Cyc_Absyn_FnInfo f2;};static struct
_tuple6 Cyc__gentuple_965={offsetof(struct _tuple83,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_966={offsetof(struct _tuple83,f2),(void*)& Cyc_Absyn_fn_info_t_rep};
static struct _tuple6*Cyc__genarr_967[2]={& Cyc__gentuple_965,& Cyc__gentuple_966};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_949={4,sizeof(struct _tuple83),{(
void*)((struct _tuple6**)Cyc__genarr_967),(void*)((struct _tuple6**)Cyc__genarr_967),(
void*)((struct _tuple6**)Cyc__genarr_967 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_945;static struct _tuple6 Cyc__gentuple_946={offsetof(struct _tuple13,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_947={offsetof(struct
_tuple13,f2),(void*)& Cyc__genrep_285};static struct _tuple6*Cyc__genarr_948[2]={&
Cyc__gentuple_946,& Cyc__gentuple_947};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_945={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_948),(void*)((
struct _tuple6**)Cyc__genarr_948),(void*)((struct _tuple6**)Cyc__genarr_948 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_941;struct _tuple84{unsigned int
f1;struct Cyc_Absyn_AggrInfo f2;};static struct _tuple6 Cyc__gentuple_942={offsetof(
struct _tuple84,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_943={
offsetof(struct _tuple84,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6*Cyc__genarr_944[2]={& Cyc__gentuple_942,& Cyc__gentuple_943};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_941={4,sizeof(struct _tuple84),{(void*)((
struct _tuple6**)Cyc__genarr_944),(void*)((struct _tuple6**)Cyc__genarr_944),(void*)((
struct _tuple6**)Cyc__genarr_944 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_936;
static struct _tuple6 Cyc__gentuple_937={offsetof(struct _tuple64,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_938={offsetof(struct _tuple64,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};
static struct _tuple6 Cyc__gentuple_939={offsetof(struct _tuple64,f3),(void*)& Cyc__genrep_357};
static struct _tuple6*Cyc__genarr_940[3]={& Cyc__gentuple_937,& Cyc__gentuple_938,&
Cyc__gentuple_939};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_936={4,
sizeof(struct _tuple64),{(void*)((struct _tuple6**)Cyc__genarr_940),(void*)((
struct _tuple6**)Cyc__genarr_940),(void*)((struct _tuple6**)Cyc__genarr_940 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_931;struct _tuple85{unsigned int
f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;};static struct _tuple6 Cyc__gentuple_932={
offsetof(struct _tuple85,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_933={
offsetof(struct _tuple85,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_934={
offsetof(struct _tuple85,f3),(void*)& Cyc__genrep_665};static struct _tuple6*Cyc__genarr_935[
3]={& Cyc__gentuple_932,& Cyc__gentuple_933,& Cyc__gentuple_934};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_931={4,sizeof(struct _tuple85),{(void*)((struct _tuple6**)Cyc__genarr_935),(
void*)((struct _tuple6**)Cyc__genarr_935),(void*)((struct _tuple6**)Cyc__genarr_935
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_75;static struct _tuple6 Cyc__gentuple_928={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_929={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_76};static struct _tuple6*Cyc__genarr_930[
2]={& Cyc__gentuple_928,& Cyc__gentuple_929};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_75={4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_930),(
void*)((struct _tuple6**)Cyc__genarr_930),(void*)((struct _tuple6**)Cyc__genarr_930
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_66;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_68;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep;
static char _tmp42C[12]="Typedefdecl";static struct _tagged_arr Cyc__genname_1123={
_tmp42C,_tmp42C,_tmp42C + 12};static char _tmp42D[5]="name";static struct _tuple5 Cyc__gentuple_1118={
offsetof(struct Cyc_Absyn_Typedefdecl,name),{_tmp42D,_tmp42D,_tmp42D + 5},(void*)&
Cyc__genrep_10};static char _tmp42E[4]="tvs";static struct _tuple5 Cyc__gentuple_1119={
offsetof(struct Cyc_Absyn_Typedefdecl,tvs),{_tmp42E,_tmp42E,_tmp42E + 4},(void*)&
Cyc__genrep_311};static char _tmp42F[5]="kind";static struct _tuple5 Cyc__gentuple_1120={
offsetof(struct Cyc_Absyn_Typedefdecl,kind),{_tmp42F,_tmp42F,_tmp42F + 5},(void*)&
Cyc__genrep_1082};static char _tmp430[5]="defn";static struct _tuple5 Cyc__gentuple_1121={
offsetof(struct Cyc_Absyn_Typedefdecl,defn),{_tmp430,_tmp430,_tmp430 + 5},(void*)&
Cyc__genrep_43};static struct _tuple5*Cyc__genarr_1122[4]={& Cyc__gentuple_1118,&
Cyc__gentuple_1119,& Cyc__gentuple_1120,& Cyc__gentuple_1121};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Typedefdecl_rep={3,(struct _tagged_arr*)& Cyc__genname_1123,
sizeof(struct Cyc_Absyn_Typedefdecl),{(void*)((struct _tuple5**)Cyc__genarr_1122),(
void*)((struct _tuple5**)Cyc__genarr_1122),(void*)((struct _tuple5**)Cyc__genarr_1122
+ 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_68={1,1,(void*)((void*)&
Cyc_struct_Absyn_Typedefdecl_rep)};struct _tuple86{unsigned int f1;struct _tuple0*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Typedefdecl*f4;void**f5;};static struct
_tuple6 Cyc__gentuple_69={offsetof(struct _tuple86,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_70={offsetof(struct _tuple86,f2),(void*)& Cyc__genrep_10};
static struct _tuple6 Cyc__gentuple_71={offsetof(struct _tuple86,f3),(void*)& Cyc__genrep_53};
static struct _tuple6 Cyc__gentuple_72={offsetof(struct _tuple86,f4),(void*)& Cyc__genrep_68};
static struct _tuple6 Cyc__gentuple_73={offsetof(struct _tuple86,f5),(void*)& Cyc__genrep_67};
static struct _tuple6*Cyc__genarr_74[5]={& Cyc__gentuple_69,& Cyc__gentuple_70,& Cyc__gentuple_71,&
Cyc__gentuple_72,& Cyc__gentuple_73};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_66={
4,sizeof(struct _tuple86),{(void*)((struct _tuple6**)Cyc__genarr_74),(void*)((
struct _tuple6**)Cyc__genarr_74),(void*)((struct _tuple6**)Cyc__genarr_74 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_52;static struct _tuple6 Cyc__gentuple_58={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_59={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_53};static struct _tuple6*Cyc__genarr_60[
2]={& Cyc__gentuple_58,& Cyc__gentuple_59};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_52={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_60),(void*)((
struct _tuple6**)Cyc__genarr_60),(void*)((struct _tuple6**)Cyc__genarr_60 + 2)}};
static char _tmp435[9]="VoidType";static struct _tuple7 Cyc__gentuple_44={0,{_tmp435,
_tmp435,_tmp435 + 9}};static char _tmp436[10]="FloatType";static struct _tuple7 Cyc__gentuple_45={
1,{_tmp436,_tmp436,_tmp436 + 10}};static char _tmp437[8]="HeapRgn";static struct
_tuple7 Cyc__gentuple_46={2,{_tmp437,_tmp437,_tmp437 + 8}};static struct _tuple7*Cyc__genarr_47[
3]={& Cyc__gentuple_44,& Cyc__gentuple_45,& Cyc__gentuple_46};static char _tmp438[5]="Evar";
static struct _tuple5 Cyc__gentuple_1092={0,{_tmp438,_tmp438,_tmp438 + 5},(void*)&
Cyc__genrep_1077};static char _tmp439[8]="VarType";static struct _tuple5 Cyc__gentuple_1093={
1,{_tmp439,_tmp439,_tmp439 + 8},(void*)& Cyc__genrep_1073};static char _tmp43A[11]="TunionType";
static struct _tuple5 Cyc__gentuple_1094={2,{_tmp43A,_tmp43A,_tmp43A + 11},(void*)&
Cyc__genrep_1047};static char _tmp43B[16]="TunionFieldType";static struct _tuple5 Cyc__gentuple_1095={
3,{_tmp43B,_tmp43B,_tmp43B + 16},(void*)& Cyc__genrep_1021};static char _tmp43C[12]="PointerType";
static struct _tuple5 Cyc__gentuple_1096={4,{_tmp43C,_tmp43C,_tmp43C + 12},(void*)&
Cyc__genrep_1002};static char _tmp43D[8]="IntType";static struct _tuple5 Cyc__gentuple_1097={
5,{_tmp43D,_tmp43D,_tmp43D + 8},(void*)& Cyc__genrep_991};static char _tmp43E[11]="DoubleType";
static struct _tuple5 Cyc__gentuple_1098={6,{_tmp43E,_tmp43E,_tmp43E + 11},(void*)&
Cyc__genrep_61};static char _tmp43F[10]="ArrayType";static struct _tuple5 Cyc__gentuple_1099={
7,{_tmp43F,_tmp43F,_tmp43F + 10},(void*)& Cyc__genrep_968};static char _tmp440[7]="FnType";
static struct _tuple5 Cyc__gentuple_1100={8,{_tmp440,_tmp440,_tmp440 + 7},(void*)&
Cyc__genrep_949};static char _tmp441[10]="TupleType";static struct _tuple5 Cyc__gentuple_1101={
9,{_tmp441,_tmp441,_tmp441 + 10},(void*)& Cyc__genrep_945};static char _tmp442[9]="AggrType";
static struct _tuple5 Cyc__gentuple_1102={10,{_tmp442,_tmp442,_tmp442 + 9},(void*)&
Cyc__genrep_941};static char _tmp443[13]="AnonAggrType";static struct _tuple5 Cyc__gentuple_1103={
11,{_tmp443,_tmp443,_tmp443 + 13},(void*)& Cyc__genrep_936};static char _tmp444[9]="EnumType";
static struct _tuple5 Cyc__gentuple_1104={12,{_tmp444,_tmp444,_tmp444 + 9},(void*)&
Cyc__genrep_931};static char _tmp445[13]="AnonEnumType";static struct _tuple5 Cyc__gentuple_1105={
13,{_tmp445,_tmp445,_tmp445 + 13},(void*)& Cyc__genrep_75};static char _tmp446[11]="SizeofType";
static struct _tuple5 Cyc__gentuple_1106={14,{_tmp446,_tmp446,_tmp446 + 11},(void*)&
Cyc__genrep_48};static char _tmp447[14]="RgnHandleType";static struct _tuple5 Cyc__gentuple_1107={
15,{_tmp447,_tmp447,_tmp447 + 14},(void*)& Cyc__genrep_48};static char _tmp448[12]="TypedefType";
static struct _tuple5 Cyc__gentuple_1108={16,{_tmp448,_tmp448,_tmp448 + 12},(void*)&
Cyc__genrep_66};static char _tmp449[8]="TagType";static struct _tuple5 Cyc__gentuple_1109={
17,{_tmp449,_tmp449,_tmp449 + 8},(void*)& Cyc__genrep_48};static char _tmp44A[8]="TypeInt";
static struct _tuple5 Cyc__gentuple_1110={18,{_tmp44A,_tmp44A,_tmp44A + 8},(void*)&
Cyc__genrep_61};static char _tmp44B[10]="AccessEff";static struct _tuple5 Cyc__gentuple_1111={
19,{_tmp44B,_tmp44B,_tmp44B + 10},(void*)& Cyc__genrep_48};static char _tmp44C[8]="JoinEff";
static struct _tuple5 Cyc__gentuple_1112={20,{_tmp44C,_tmp44C,_tmp44C + 8},(void*)&
Cyc__genrep_52};static char _tmp44D[8]="RgnsEff";static struct _tuple5 Cyc__gentuple_1113={
21,{_tmp44D,_tmp44D,_tmp44D + 8},(void*)& Cyc__genrep_48};static struct _tuple5*Cyc__genarr_1114[
22]={& Cyc__gentuple_1092,& Cyc__gentuple_1093,& Cyc__gentuple_1094,& Cyc__gentuple_1095,&
Cyc__gentuple_1096,& Cyc__gentuple_1097,& Cyc__gentuple_1098,& Cyc__gentuple_1099,&
Cyc__gentuple_1100,& Cyc__gentuple_1101,& Cyc__gentuple_1102,& Cyc__gentuple_1103,&
Cyc__gentuple_1104,& Cyc__gentuple_1105,& Cyc__gentuple_1106,& Cyc__gentuple_1107,&
Cyc__gentuple_1108,& Cyc__gentuple_1109,& Cyc__gentuple_1110,& Cyc__gentuple_1111,&
Cyc__gentuple_1112,& Cyc__gentuple_1113};static char _tmp44F[5]="Type";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_type_t_rep={5,{_tmp44F,_tmp44F,_tmp44F + 5},{(void*)((struct _tuple7**)
Cyc__genarr_47),(void*)((struct _tuple7**)Cyc__genarr_47),(void*)((struct _tuple7**)
Cyc__genarr_47 + 3)},{(void*)((struct _tuple5**)Cyc__genarr_1114),(void*)((struct
_tuple5**)Cyc__genarr_1114),(void*)((struct _tuple5**)Cyc__genarr_1114 + 22)}};
static char _tmp450[8]="Vardecl";static struct _tagged_arr Cyc__genname_154={_tmp450,
_tmp450,_tmp450 + 8};static char _tmp451[3]="sc";static struct _tuple5 Cyc__gentuple_145={
offsetof(struct Cyc_Absyn_Vardecl,sc),{_tmp451,_tmp451,_tmp451 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp452[5]="name";static struct _tuple5 Cyc__gentuple_146={offsetof(
struct Cyc_Absyn_Vardecl,name),{_tmp452,_tmp452,_tmp452 + 5},(void*)& Cyc__genrep_10};
static char _tmp453[3]="tq";static struct _tuple5 Cyc__gentuple_147={offsetof(struct
Cyc_Absyn_Vardecl,tq),{_tmp453,_tmp453,_tmp453 + 3},(void*)& Cyc__genrep_134};
static char _tmp454[5]="type";static struct _tuple5 Cyc__gentuple_148={offsetof(
struct Cyc_Absyn_Vardecl,type),{_tmp454,_tmp454,_tmp454 + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp455[12]="initializer";static struct _tuple5 Cyc__gentuple_149={
offsetof(struct Cyc_Absyn_Vardecl,initializer),{_tmp455,_tmp455,_tmp455 + 12},(
void*)& Cyc__genrep_78};static char _tmp456[4]="rgn";static struct _tuple5 Cyc__gentuple_150={
offsetof(struct Cyc_Absyn_Vardecl,rgn),{_tmp456,_tmp456,_tmp456 + 4},(void*)& Cyc__genrep_43};
static char _tmp457[11]="attributes";static struct _tuple5 Cyc__gentuple_151={
offsetof(struct Cyc_Absyn_Vardecl,attributes),{_tmp457,_tmp457,_tmp457 + 11},(void*)&
Cyc__genrep_88};static char _tmp458[8]="escapes";static struct _tuple5 Cyc__gentuple_152={
offsetof(struct Cyc_Absyn_Vardecl,escapes),{_tmp458,_tmp458,_tmp458 + 8},(void*)((
void*)& Cyc__genrep_62)};static struct _tuple5*Cyc__genarr_153[8]={& Cyc__gentuple_145,&
Cyc__gentuple_146,& Cyc__gentuple_147,& Cyc__gentuple_148,& Cyc__gentuple_149,& Cyc__gentuple_150,&
Cyc__gentuple_151,& Cyc__gentuple_152};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep={
3,(struct _tagged_arr*)& Cyc__genname_154,sizeof(struct Cyc_Absyn_Vardecl),{(void*)((
struct _tuple5**)Cyc__genarr_153),(void*)((struct _tuple5**)Cyc__genarr_153),(void*)((
struct _tuple5**)Cyc__genarr_153 + 8)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_133={
1,1,(void*)((void*)& Cyc_struct_Absyn_Vardecl_rep)};struct _tuple87{unsigned int f1;
struct Cyc_Absyn_Vardecl*f2;};static struct _tuple6 Cyc__gentuple_431={offsetof(
struct _tuple87,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_432={
offsetof(struct _tuple87,f2),(void*)& Cyc__genrep_133};static struct _tuple6*Cyc__genarr_433[
2]={& Cyc__gentuple_431,& Cyc__gentuple_432};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_430={4,sizeof(struct _tuple87),{(void*)((struct _tuple6**)Cyc__genarr_433),(
void*)((struct _tuple6**)Cyc__genarr_433),(void*)((struct _tuple6**)Cyc__genarr_433
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1143;struct _tuple88{
unsigned int f1;struct Cyc_Absyn_Pat*f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_1144={offsetof(struct _tuple88,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1145={offsetof(struct _tuple88,f2),(
void*)& Cyc__genrep_231};static struct _tuple6 Cyc__gentuple_1146={offsetof(struct
_tuple88,f3),(void*)& Cyc__genrep_131};static struct _tuple6 Cyc__gentuple_1147={
offsetof(struct _tuple88,f4),(void*)& Cyc__genrep_82};static struct _tuple6*Cyc__genarr_1148[
4]={& Cyc__gentuple_1144,& Cyc__gentuple_1145,& Cyc__gentuple_1146,& Cyc__gentuple_1147};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1143={4,sizeof(struct _tuple88),{(
void*)((struct _tuple6**)Cyc__genarr_1148),(void*)((struct _tuple6**)Cyc__genarr_1148),(
void*)((struct _tuple6**)Cyc__genarr_1148 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1139;static struct _tuple6 Cyc__gentuple_1140={offsetof(struct _tuple13,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1141={offsetof(struct
_tuple13,f2),(void*)& Cyc__genrep_132};static struct _tuple6*Cyc__genarr_1142[2]={&
Cyc__gentuple_1140,& Cyc__gentuple_1141};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1139={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_1142),(void*)((
struct _tuple6**)Cyc__genarr_1142),(void*)((struct _tuple6**)Cyc__genarr_1142 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1135;struct _tuple89{unsigned int
f1;struct Cyc_Absyn_Aggrdecl*f2;};static struct _tuple6 Cyc__gentuple_1136={
offsetof(struct _tuple89,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1137={
offsetof(struct _tuple89,f2),(void*)((void*)& Cyc__genrep_355)};static struct
_tuple6*Cyc__genarr_1138[2]={& Cyc__gentuple_1136,& Cyc__gentuple_1137};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1135={4,sizeof(struct _tuple89),{(void*)((
struct _tuple6**)Cyc__genarr_1138),(void*)((struct _tuple6**)Cyc__genarr_1138),(
void*)((struct _tuple6**)Cyc__genarr_1138 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1131;struct _tuple90{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;};
static struct _tuple6 Cyc__gentuple_1132={offsetof(struct _tuple90,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_1133={offsetof(struct _tuple90,f2),(void*)((void*)&
Cyc__genrep_301)};static struct _tuple6*Cyc__genarr_1134[2]={& Cyc__gentuple_1132,&
Cyc__gentuple_1133};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1131={4,
sizeof(struct _tuple90),{(void*)((struct _tuple6**)Cyc__genarr_1134),(void*)((
struct _tuple6**)Cyc__genarr_1134),(void*)((struct _tuple6**)Cyc__genarr_1134 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1127;struct _tuple91{unsigned int
f1;struct Cyc_Absyn_Enumdecl*f2;};static struct _tuple6 Cyc__gentuple_1128={
offsetof(struct _tuple91,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1129={
offsetof(struct _tuple91,f2),(void*)& Cyc__genrep_255};static struct _tuple6*Cyc__genarr_1130[
2]={& Cyc__gentuple_1128,& Cyc__gentuple_1129};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1127={4,sizeof(struct _tuple91),{(void*)((struct _tuple6**)Cyc__genarr_1130),(
void*)((struct _tuple6**)Cyc__genarr_1130),(void*)((struct _tuple6**)Cyc__genarr_1130
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_41;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_42;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_42={1,1,(void*)((
void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct _tuple92{unsigned int f1;struct
Cyc_Absyn_Typedefdecl*f2;};static struct _tuple6 Cyc__gentuple_1124={offsetof(
struct _tuple92,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1125={
offsetof(struct _tuple92,f2),(void*)& Cyc__genrep_42};static struct _tuple6*Cyc__genarr_1126[
2]={& Cyc__gentuple_1124,& Cyc__gentuple_1125};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_41={4,sizeof(struct _tuple92),{(void*)((struct _tuple6**)Cyc__genarr_1126),(
void*)((struct _tuple6**)Cyc__genarr_1126),(void*)((struct _tuple6**)Cyc__genarr_1126
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_36;struct _tuple93{
unsigned int f1;struct _tagged_arr*f2;struct Cyc_List_List*f3;};static struct _tuple6
Cyc__gentuple_37={offsetof(struct _tuple93,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_38={offsetof(struct _tuple93,f2),(void*)& Cyc__genrep_12};
static struct _tuple6 Cyc__gentuple_39={offsetof(struct _tuple93,f3),(void*)& Cyc__genrep_0};
static struct _tuple6*Cyc__genarr_40[3]={& Cyc__gentuple_37,& Cyc__gentuple_38,& Cyc__gentuple_39};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_36={4,sizeof(struct _tuple93),{(
void*)((struct _tuple6**)Cyc__genarr_40),(void*)((struct _tuple6**)Cyc__genarr_40),(
void*)((struct _tuple6**)Cyc__genarr_40 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_9;static struct _tuple6 Cyc__gentuple_32={offsetof(struct _tuple54,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_33={offsetof(struct
_tuple54,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_34={
offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_35[
3]={& Cyc__gentuple_32,& Cyc__gentuple_33,& Cyc__gentuple_34};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_9={4,sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_35),(
void*)((struct _tuple6**)Cyc__genarr_35),(void*)((struct _tuple6**)Cyc__genarr_35 + 
3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_4;static struct _tuple6 Cyc__gentuple_6={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_7={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_8[
2]={& Cyc__gentuple_6,& Cyc__gentuple_7};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_4={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_8),(void*)((
struct _tuple6**)Cyc__genarr_8),(void*)((struct _tuple6**)Cyc__genarr_8 + 2)}};
static struct _tuple7*Cyc__genarr_3[0]={};static char _tmp466[6]="Var_d";static
struct _tuple5 Cyc__gentuple_1149={0,{_tmp466,_tmp466,_tmp466 + 6},(void*)& Cyc__genrep_430};
static char _tmp467[5]="Fn_d";static struct _tuple5 Cyc__gentuple_1150={1,{_tmp467,
_tmp467,_tmp467 + 5},(void*)& Cyc__genrep_86};static char _tmp468[6]="Let_d";static
struct _tuple5 Cyc__gentuple_1151={2,{_tmp468,_tmp468,_tmp468 + 6},(void*)& Cyc__genrep_1143};
static char _tmp469[7]="Letv_d";static struct _tuple5 Cyc__gentuple_1152={3,{_tmp469,
_tmp469,_tmp469 + 7},(void*)& Cyc__genrep_1139};static char _tmp46A[7]="Aggr_d";
static struct _tuple5 Cyc__gentuple_1153={4,{_tmp46A,_tmp46A,_tmp46A + 7},(void*)&
Cyc__genrep_1135};static char _tmp46B[9]="Tunion_d";static struct _tuple5 Cyc__gentuple_1154={
5,{_tmp46B,_tmp46B,_tmp46B + 9},(void*)& Cyc__genrep_1131};static char _tmp46C[7]="Enum_d";
static struct _tuple5 Cyc__gentuple_1155={6,{_tmp46C,_tmp46C,_tmp46C + 7},(void*)&
Cyc__genrep_1127};static char _tmp46D[10]="Typedef_d";static struct _tuple5 Cyc__gentuple_1156={
7,{_tmp46D,_tmp46D,_tmp46D + 10},(void*)& Cyc__genrep_41};static char _tmp46E[12]="Namespace_d";
static struct _tuple5 Cyc__gentuple_1157={8,{_tmp46E,_tmp46E,_tmp46E + 12},(void*)&
Cyc__genrep_36};static char _tmp46F[8]="Using_d";static struct _tuple5 Cyc__gentuple_1158={
9,{_tmp46F,_tmp46F,_tmp46F + 8},(void*)& Cyc__genrep_9};static char _tmp470[10]="ExternC_d";
static struct _tuple5 Cyc__gentuple_1159={10,{_tmp470,_tmp470,_tmp470 + 10},(void*)&
Cyc__genrep_4};static struct _tuple5*Cyc__genarr_1160[11]={& Cyc__gentuple_1149,&
Cyc__gentuple_1150,& Cyc__gentuple_1151,& Cyc__gentuple_1152,& Cyc__gentuple_1153,&
Cyc__gentuple_1154,& Cyc__gentuple_1155,& Cyc__gentuple_1156,& Cyc__gentuple_1157,&
Cyc__gentuple_1158,& Cyc__gentuple_1159};static char _tmp472[9]="Raw_decl";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep={5,{_tmp472,_tmp472,_tmp472 + 9},{(
void*)((struct _tuple7**)Cyc__genarr_3),(void*)((struct _tuple7**)Cyc__genarr_3),(
void*)((struct _tuple7**)Cyc__genarr_3 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_1160),(
void*)((struct _tuple5**)Cyc__genarr_1160),(void*)((struct _tuple5**)Cyc__genarr_1160
+ 11)}};static char _tmp473[5]="Decl";static struct _tagged_arr Cyc__genname_1164={
_tmp473,_tmp473,_tmp473 + 5};static char _tmp474[2]="r";static struct _tuple5 Cyc__gentuple_1161={
offsetof(struct Cyc_Absyn_Decl,r),{_tmp474,_tmp474,_tmp474 + 2},(void*)& Cyc_Absyn_raw_decl_t_rep};
static char _tmp475[4]="loc";static struct _tuple5 Cyc__gentuple_1162={offsetof(
struct Cyc_Absyn_Decl,loc),{_tmp475,_tmp475,_tmp475 + 4},(void*)& Cyc__genrep_2};
static struct _tuple5*Cyc__genarr_1163[2]={& Cyc__gentuple_1161,& Cyc__gentuple_1162};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Decl_rep={3,(struct _tagged_arr*)&
Cyc__genname_1164,sizeof(struct Cyc_Absyn_Decl),{(void*)((struct _tuple5**)Cyc__genarr_1163),(
void*)((struct _tuple5**)Cyc__genarr_1163),(void*)((struct _tuple5**)Cyc__genarr_1163
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1={1,1,(void*)((void*)&
Cyc_struct_Absyn_Decl_rep)};static char _tmp478[5]="List";static struct _tagged_arr
Cyc__genname_1168={_tmp478,_tmp478,_tmp478 + 5};static char _tmp479[3]="hd";static
struct _tuple5 Cyc__gentuple_1165={offsetof(struct Cyc_List_List,hd),{_tmp479,
_tmp479,_tmp479 + 3},(void*)& Cyc__genrep_1};static char _tmp47A[3]="tl";static
struct _tuple5 Cyc__gentuple_1166={offsetof(struct Cyc_List_List,tl),{_tmp47A,
_tmp47A,_tmp47A + 3},(void*)& Cyc__genrep_0};static struct _tuple5*Cyc__genarr_1167[
2]={& Cyc__gentuple_1165,& Cyc__gentuple_1166};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_1168,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_1167),(void*)((struct _tuple5**)Cyc__genarr_1167),(
void*)((struct _tuple5**)Cyc__genarr_1167 + 2)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_0={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_decl_t46H2_rep)};
void*Cyc_decls_rep=(void*)& Cyc__genrep_0;
