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
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*
Cyc_Position_segment_join(struct Cyc_Position_Segment*,struct Cyc_Position_Segment*);
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
int tag;void*f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;
struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{
void*rgn;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
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
_tagged_arr*f1;};char Cyc_Absyn_EmptyAnnot[15]="\000\000\000\000EmptyAnnot\000";
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value;
extern void*Cyc_Absyn_rel_ns_null;int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual
y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
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
tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_bool_exp(int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _tagged_arr f,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _tagged_arr s,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*,struct Cyc_List_List*es,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
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
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);
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
Cyc_Absyn_label_stmt(struct _tagged_arr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _tagged_arr*lab,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct
Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*
Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct
Cyc_List_List*fs);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct
_tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_union_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_xtunion,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
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
Cyc_Absyn_print_decls(struct Cyc_List_List*);extern int Cyc_Absyn_porting_c_code;
struct Cyc_Typerep_Int_struct{int tag;int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{
int tag;unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;}
;struct _tuple5{unsigned int f1;struct _tagged_arr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
int tag;struct _tagged_arr*f1;unsigned int f2;struct _tagged_arr f3;};struct _tuple6{
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
int Cyc_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*s2);struct Cyc___cycFILE;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
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
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_impos(struct
_tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);void Cyc_Marshal_print_type(
void*rep,void*val);struct _tuple10{struct Cyc_Dict_Dict*f1;int f2;};struct _tuple11{
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
_tmp5 != 0)goto _LL3;_LL2: goto _LL0;_LL3: _tmp6=_tmp3.f1;if(_tmp6 <= (void*)1  || *((
int*)_tmp6)!= 0)goto _LL5;_tmp7=((struct Cyc_Absyn_Rel_n_struct*)_tmp6)->f1;_tmp8=
_tmp3.f2;if(_tmp8 <= (void*)1  || *((int*)_tmp8)!= 0)goto _LL5;_tmp9=((struct Cyc_Absyn_Rel_n_struct*)
_tmp8)->f1;_LL4: _tmpB=_tmp7;_tmpD=_tmp9;goto _LL6;_LL5: _tmpA=_tmp3.f1;if(_tmpA <= (
void*)1  || *((int*)_tmpA)!= 1)goto _LL7;_tmpB=((struct Cyc_Absyn_Abs_n_struct*)
_tmpA)->f1;_tmpC=_tmp3.f2;if(_tmpC <= (void*)1  || *((int*)_tmpC)!= 1)goto _LL7;
_tmpD=((struct Cyc_Absyn_Abs_n_struct*)_tmpC)->f1;_LL6: {int i=Cyc_Absyn_strlist_cmp(
_tmpB,_tmpD);if(i != 0)return i;goto _LL0;}_LL7: _tmpE=_tmp3.f1;if((int)_tmpE != 0)
goto _LL9;_LL8: return - 1;_LL9: _tmpF=_tmp3.f2;if((int)_tmpF != 0)goto _LLB;_LLA:
return 1;_LLB: _tmp10=_tmp3.f1;if(_tmp10 <= (void*)1  || *((int*)_tmp10)!= 0)goto
_LLD;_LLC: return - 1;_LLD: _tmp11=_tmp3.f2;if(_tmp11 <= (void*)1  || *((int*)_tmp11)
!= 0)goto _LL0;_LLE: return 1;_LL0:;}return Cyc_strptrcmp((*q1).f2,(*q2).f2);}int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){int i=Cyc_strptrcmp(tv1->name,
tv2->name);if(i != 0)return i;if(tv1->identity == tv2->identity)return 0;if(tv1->identity
!= 0  && tv2->identity != 0)return*((int*)_check_null(tv1->identity))- *((int*)
_check_null(tv2->identity));else{if(tv1->identity == 0)return - 1;else{return 1;}}}
struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value={0,0};void*Cyc_Absyn_rel_ns_null=(
void*)& Cyc_Absyn_rel_ns_null_value;int Cyc_Absyn_is_qvar_qualified(struct _tuple0*
qv){void*_tmp13=(*qv).f1;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp15;
_LL10: if(_tmp13 <= (void*)1  || *((int*)_tmp13)!= 0)goto _LL12;_tmp14=((struct Cyc_Absyn_Rel_n_struct*)
_tmp13)->f1;if(_tmp14 != 0)goto _LL12;_LL11: goto _LL13;_LL12: if(_tmp13 <= (void*)1
 || *((int*)_tmp13)!= 1)goto _LL14;_tmp15=((struct Cyc_Absyn_Abs_n_struct*)_tmp13)->f1;
if(_tmp15 != 0)goto _LL14;_LL13: goto _LL15;_LL14: if((int)_tmp13 != 0)goto _LL16;_LL15:
return 0;_LL16:;_LL17: return 1;_LLF:;}static int Cyc_Absyn_new_type_counter=0;void*
Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){return(void*)({
struct Cyc_Absyn_Evar_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=({struct
Cyc_Absyn_Evar_struct _tmp17;_tmp17.tag=0;_tmp17.f1=k;_tmp17.f2=0;_tmp17.f3=Cyc_Absyn_new_type_counter
++;_tmp17.f4=env;_tmp17;});_tmp16;});}static struct Cyc_Core_Opt Cyc_Absyn_mk={(
void*)((void*)1)};void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Absyn_mk,tenv);}struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){return({struct Cyc_Absyn_Tqual
_tmp18;_tmp18.print_const=x.print_const  || y.print_const;_tmp18.q_volatile=x.q_volatile
 || y.q_volatile;_tmp18.q_restrict=x.q_restrict  || y.q_restrict;_tmp18.real_const=
x.real_const  || y.real_const;_tmp18.loc=Cyc_Position_segment_join(x.loc,y.loc);
_tmp18;});}struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Tqual _tmp19;_tmp19.print_const=0;_tmp19.q_volatile=
0;_tmp19.q_restrict=0;_tmp19.real_const=0;_tmp19.loc=loc;_tmp19;});}struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Tqual
_tmp1A;_tmp1A.print_const=1;_tmp1A.q_volatile=0;_tmp1A.q_restrict=0;_tmp1A.real_const=
1;_tmp1A.loc=loc;_tmp1A;});}static struct Cyc_Absyn_Int_c_struct Cyc_Absyn_one_intc={
2,(void*)((void*)1),1};static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={
0,(void*)((void*)& Cyc_Absyn_one_intc)};struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={
0,(void*)((void*)& Cyc_Absyn_one_b_raw),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
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
_LL19: if((int)_tmp26 != 0)goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp26 <= (void*)1
 || *((int*)_tmp26)!= 0)goto _LL1D;_LL1C: return x;_LL1D: if(_tmp26 <= (void*)1  || *((
int*)_tmp26)!= 1)goto _LL18;_tmp27=((struct Cyc_Absyn_Forward_constr_struct*)
_tmp26)->f1;_tmp28=(struct Cyc_Absyn_Conref**)&((struct Cyc_Absyn_Forward_constr_struct*)
_tmp26)->f1;_LL1E: {struct Cyc_Absyn_Conref*_tmp29=Cyc_Absyn_compress_conref(*
_tmp28);*_tmp28=_tmp29;return _tmp29;}_LL18:;}void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*
x){void*_tmp2A=(void*)(Cyc_Absyn_compress_conref(x))->v;void*_tmp2B;_LL20: if(
_tmp2A <= (void*)1  || *((int*)_tmp2A)!= 0)goto _LL22;_tmp2B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2A)->f1;_LL21: return _tmp2B;_LL22:;_LL23:({void*_tmp2C[0]={};Cyc_Tcutil_impos(({
const char*_tmp2D="conref_val";_tag_arr(_tmp2D,sizeof(char),_get_zero_arr_size(
_tmp2D,11));}),_tag_arr(_tmp2C,sizeof(void*),0));});_LL1F:;}void*Cyc_Absyn_conref_def(
void*y,struct Cyc_Absyn_Conref*x){void*_tmp2E=(void*)(Cyc_Absyn_compress_conref(x))->v;
void*_tmp2F;_LL25: if(_tmp2E <= (void*)1  || *((int*)_tmp2E)!= 0)goto _LL27;_tmp2F=(
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
void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(
0),(void*)0,Cyc_Absyn_true_conref);if(Cyc_Absyn_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_empty_tqual(0),(void*)0,Cyc_Absyn_true_conref);
Cyc_Absyn_string_t_opt=({void**_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=t;
_tmp5D;});}return*((void**)_check_null(Cyc_Absyn_string_t_opt));}static void**Cyc_Absyn_const_string_t_opt=
0;void*Cyc_Absyn_const_string_typ(void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
if(Cyc_Absyn_const_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);Cyc_Absyn_const_string_t_opt=({
void**_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E[0]=t;_tmp5E;});}return*((void**)
_check_null(Cyc_Absyn_const_string_t_opt));}void*Cyc_Absyn_starb_typ(void*t,void*
r,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zeroterm){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F[0]=({
struct Cyc_Absyn_PointerType_struct _tmp60;_tmp60.tag=4;_tmp60.f1=({struct Cyc_Absyn_PtrInfo
_tmp61;_tmp61.elt_typ=(void*)t;_tmp61.elt_tq=tq;_tmp61.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp62;_tmp62.rgn=(void*)r;_tmp62.nullable=Cyc_Absyn_true_conref;_tmp62.bounds=
Cyc_Absyn_new_conref(b);_tmp62.zero_term=zeroterm;_tmp62.ptrloc=0;_tmp62;});
_tmp61;});_tmp60;});_tmp5F;});}void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=({struct Cyc_Absyn_PointerType_struct
_tmp64;_tmp64.tag=4;_tmp64.f1=({struct Cyc_Absyn_PtrInfo _tmp65;_tmp65.elt_typ=(
void*)t;_tmp65.elt_tq=tq;_tmp65.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp66;_tmp66.rgn=(
void*)r;_tmp66.nullable=Cyc_Absyn_false_conref;_tmp66.bounds=Cyc_Absyn_new_conref(
b);_tmp66.zero_term=zeroterm;_tmp66.ptrloc=0;_tmp66;});_tmp65;});_tmp64;});
_tmp63;});}void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Conref*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67[0]=({struct Cyc_Absyn_PointerType_struct
_tmp68;_tmp68.tag=4;_tmp68.f1=({struct Cyc_Absyn_PtrInfo _tmp69;_tmp69.elt_typ=(
void*)t;_tmp69.elt_tq=tq;_tmp69.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp6A;_tmp6A.rgn=(
void*)r;_tmp6A.nullable=Cyc_Absyn_true_conref;_tmp6A.bounds=Cyc_Absyn_bounds_one_conref;
_tmp6A.zero_term=zeroterm;_tmp6A.ptrloc=0;_tmp6A;});_tmp69;});_tmp68;});_tmp67;});}
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B[0]=({
struct Cyc_Absyn_PointerType_struct _tmp6C;_tmp6C.tag=4;_tmp6C.f1=({struct Cyc_Absyn_PtrInfo
_tmp6D;_tmp6D.elt_typ=(void*)t;_tmp6D.elt_tq=tq;_tmp6D.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp6E;_tmp6E.rgn=(void*)((void*)2);_tmp6E.nullable=Cyc_Absyn_true_conref;_tmp6E.bounds=
Cyc_Absyn_bounds_one_conref;_tmp6E.zero_term=Cyc_Absyn_false_conref;_tmp6E.ptrloc=
0;_tmp6E;});_tmp6D;});_tmp6C;});_tmp6B;});}void*Cyc_Absyn_at_typ(void*t,void*r,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F[0]=({
struct Cyc_Absyn_PointerType_struct _tmp70;_tmp70.tag=4;_tmp70.f1=({struct Cyc_Absyn_PtrInfo
_tmp71;_tmp71.elt_typ=(void*)t;_tmp71.elt_tq=tq;_tmp71.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp72;_tmp72.rgn=(void*)r;_tmp72.nullable=Cyc_Absyn_false_conref;_tmp72.bounds=
Cyc_Absyn_bounds_one_conref;_tmp72.zero_term=zeroterm;_tmp72.ptrloc=0;_tmp72;});
_tmp71;});_tmp70;});_tmp6F;});}void*Cyc_Absyn_tagged_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73[0]=({struct Cyc_Absyn_PointerType_struct
_tmp74;_tmp74.tag=4;_tmp74.f1=({struct Cyc_Absyn_PtrInfo _tmp75;_tmp75.elt_typ=(
void*)t;_tmp75.elt_tq=tq;_tmp75.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp76;_tmp76.rgn=(
void*)r;_tmp76.nullable=Cyc_Absyn_true_conref;_tmp76.bounds=Cyc_Absyn_bounds_unknown_conref;
_tmp76.zero_term=zeroterm;_tmp76.ptrloc=0;_tmp76;});_tmp75;});_tmp74;});_tmp73;});}
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc){
return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp77=_cycalloc(sizeof(*_tmp77));
_tmp77[0]=({struct Cyc_Absyn_ArrayType_struct _tmp78;_tmp78.tag=7;_tmp78.f1=({
struct Cyc_Absyn_ArrayInfo _tmp79;_tmp79.elt_type=(void*)elt_type;_tmp79.tq=tq;
_tmp79.num_elts=num_elts;_tmp79.zero_term=zero_term;_tmp79.zt_loc=ztloc;_tmp79;});
_tmp78;});_tmp77;});}static char _tmp83[8]="__sFILE";void*Cyc_Absyn_file_typ(){
static void**file_t_opt=0;static struct _tagged_arr sf_str={_tmp83,_tmp83,_tmp83 + 8};
static struct _tagged_arr*sf=& sf_str;if(file_t_opt == 0){struct _tuple0*file_t_name=({
struct _tuple0*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=(void*)& Cyc_Absyn_std_namespace;
_tmp82->f2=sf;_tmp82;});struct Cyc_Absyn_Aggrdecl*sd=({struct Cyc_Absyn_Aggrdecl*
_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->kind=(void*)((void*)0);_tmp81->sc=(
void*)((void*)1);_tmp81->name=file_t_name;_tmp81->tvs=0;_tmp81->impl=0;_tmp81->attributes=
0;_tmp81;});void*file_struct_typ=(void*)({struct Cyc_Absyn_AggrType_struct*_tmp7B=
_cycalloc(sizeof(*_tmp7B));_tmp7B[0]=({struct Cyc_Absyn_AggrType_struct _tmp7C;
_tmp7C.tag=10;_tmp7C.f1=({struct Cyc_Absyn_AggrInfo _tmp7D;_tmp7D.aggr_info=(void*)((
void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));
_tmp7E[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp7F;_tmp7F.tag=1;_tmp7F.f1=({
struct Cyc_Absyn_Aggrdecl**_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[0]=sd;_tmp80;});
_tmp7F;});_tmp7E;}));_tmp7D.targs=0;_tmp7D;});_tmp7C;});_tmp7B;});file_t_opt=({
void**_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=Cyc_Absyn_at_typ(
file_struct_typ,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);_tmp7A;});}
return*file_t_opt;}void*Cyc_Absyn_void_star_typ(){static void**void_star_t_opt=0;
if(void_star_t_opt == 0)void_star_t_opt=({void**_tmp84=_cycalloc(sizeof(*_tmp84));
_tmp84[0]=Cyc_Absyn_star_typ((void*)0,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
_tmp84;});return*void_star_t_opt;}static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={
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
17;_tmp98.f1=rgn_handle;_tmp98.f2=e;_tmp98;});_tmp97;}),loc);}struct Cyc_Absyn_Exp*
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
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_And_e_struct*
_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_And_e_struct _tmpB8;
_tmpB8.tag=7;_tmpB8.f1=e1;_tmpB8.f2=e2;_tmpB8;});_tmpB7;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Or_e_struct*_tmpB9=
_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({struct Cyc_Absyn_Or_e_struct _tmpBA;_tmpBA.tag=
8;_tmpBA.f1=e1;_tmpBA.f2=e2;_tmpBA;});_tmpB9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_SeqExp_e_struct*_tmpBB=
_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_SeqExp_e_struct _tmpBC;
_tmpBC.tag=9;_tmpBC.f1=e1;_tmpBC.f2=e2;_tmpBC;});_tmpBB;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownCall_e_struct*
_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_UnknownCall_e_struct
_tmpBE;_tmpBE.tag=10;_tmpBE.f1=e;_tmpBE.f2=es;_tmpBE;});_tmpBD;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpC0;_tmpC0.tag=11;_tmpC0.f1=e;_tmpC0.f2=es;_tmpC0.f3=0;_tmpC0;});_tmpBF;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=({struct Cyc_Absyn_NoInstantiate_e_struct
_tmpC2;_tmpC2.tag=13;_tmpC2.f1=e;_tmpC2;});_tmpC1;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmpC4;_tmpC4.tag=14;_tmpC4.f1=e;_tmpC4.f2=ts;_tmpC4;});_tmpC3;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,void*
c,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Cast_e_struct*
_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_Cast_e_struct
_tmpC6;_tmpC6.tag=15;_tmpC6.f1=(void*)t;_tmpC6.f2=e;_tmpC6.f3=user_cast;_tmpC6.f4=(
void*)c;_tmpC6;});_tmpC5;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Throw_e_struct*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=({
struct Cyc_Absyn_Throw_e_struct _tmpC8;_tmpC8.tag=12;_tmpC8.f1=e;_tmpC8;});_tmpC7;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Address_e_struct*_tmpC9=
_cycalloc(sizeof(*_tmpC9));_tmpC9[0]=({struct Cyc_Absyn_Address_e_struct _tmpCA;
_tmpCA.tag=16;_tmpCA.f1=e;_tmpCA;});_tmpC9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Sizeoftyp_e_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpCC;_tmpCC.tag=18;_tmpCC.f1=(void*)t;_tmpCC;});
_tmpCB;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Absyn_Sizeofexp_e_struct
_tmpCE;_tmpCE.tag=19;_tmpCE.f1=e;_tmpCE;});_tmpCD;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_offsetof_exp(void*t,void*of,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));
_tmpCF[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmpD0;_tmpD0.tag=20;_tmpD0.f1=(
void*)t;_tmpD0.f2=(void*)of;_tmpD0;});_tmpCF;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*tvs,void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Gentyp_e_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1[
0]=({struct Cyc_Absyn_Gentyp_e_struct _tmpD2;_tmpD2.tag=21;_tmpD2.f1=tvs;_tmpD2.f2=(
void*)t;_tmpD2;});_tmpD1;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Deref_e_struct*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3[0]=({
struct Cyc_Absyn_Deref_e_struct _tmpD4;_tmpD4.tag=22;_tmpD4.f1=e;_tmpD4;});_tmpD3;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_tagged_arr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_AggrMember_e_struct*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({
struct Cyc_Absyn_AggrMember_e_struct _tmpD6;_tmpD6.tag=23;_tmpD6.f1=e;_tmpD6.f2=n;
_tmpD6;});_tmpD5;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*
e,struct _tagged_arr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmpD7=_cycalloc(sizeof(*_tmpD7));
_tmpD7[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmpD8;_tmpD8.tag=24;_tmpD8.f1=e;
_tmpD8.f2=n;_tmpD8;});_tmpD7;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpD9=
_cycalloc(sizeof(*_tmpD9));_tmpD9[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpDA;
_tmpDA.tag=25;_tmpDA.f1=e1;_tmpDA.f2=e2;_tmpDA;});_tmpD9;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tuple_e_struct*_tmpDB=_cycalloc(
sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Absyn_Tuple_e_struct _tmpDC;_tmpDC.tag=26;
_tmpDC.f1=es;_tmpDC;});_tmpDB;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpDD=_cycalloc(sizeof(*_tmpDD));
_tmpDD[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpDE;_tmpDE.tag=37;_tmpDE.f1=s;
_tmpDE;});_tmpDD;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}struct _tuple12{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){dles=({struct Cyc_List_List*_tmpDF=_cycalloc(sizeof(*
_tmpDF));_tmpDF->hd=({struct _tuple12*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->f1=
0;_tmpE0->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpE0;});_tmpDF->tl=dles;_tmpDF;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_struct*_tmpE1=_cycalloc(
sizeof(*_tmpE1));_tmpE1[0]=({struct Cyc_Absyn_Array_e_struct _tmpE2;_tmpE2.tag=28;
_tmpE2.f1=dles;_tmpE2;});_tmpE1;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpE3=
_cycalloc(sizeof(*_tmpE3));_tmpE3[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpE4;_tmpE4.tag=36;_tmpE4.f1=n;_tmpE4.f2=dles;_tmpE4;});_tmpE3;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){return({
struct Cyc_Absyn_Stmt*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->r=(void*)s;_tmpE5->loc=
loc;_tmpE5->non_local_preds=0;_tmpE5->try_depth=0;_tmpE5->annot=(void*)((void*)
Cyc_Absyn_EmptyAnnot);_tmpE5;});}struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)0,loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Exp_s_struct*_tmpE6=_cycalloc(
sizeof(*_tmpE6));_tmpE6[0]=({struct Cyc_Absyn_Exp_s_struct _tmpE7;_tmpE7.tag=0;
_tmpE7.f1=e;_tmpE7;});_tmpE6;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(
struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc){if(ss == 0)return Cyc_Absyn_skip_stmt(
loc);else{if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{return Cyc_Absyn_seq_stmt((
struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Return_s_struct*_tmpE8=
_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=({struct Cyc_Absyn_Return_s_struct _tmpE9;
_tmpE9.tag=2;_tmpE9.f1=e;_tmpE9;});_tmpE8;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_IfThenElse_s_struct*
_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=({struct Cyc_Absyn_IfThenElse_s_struct
_tmpEB;_tmpEB.tag=3;_tmpEB.f1=e;_tmpEB.f2=s1;_tmpEB.f3=s2;_tmpEB;});_tmpEA;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_While_s_struct*
_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=({struct Cyc_Absyn_While_s_struct
_tmpED;_tmpED.tag=4;_tmpED.f1=({struct _tuple2 _tmpEE;_tmpEE.f1=e;_tmpEE.f2=Cyc_Absyn_skip_stmt(
e->loc);_tmpEE;});_tmpED.f2=s;_tmpED;});_tmpEC;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Break_s_struct*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Absyn_Break_s_struct
_tmpF0;_tmpF0.tag=5;_tmpF0.f1=0;_tmpF0;});_tmpEF;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Continue_s_struct*_tmpF1=_cycalloc(sizeof(*_tmpF1));
_tmpF1[0]=({struct Cyc_Absyn_Continue_s_struct _tmpF2;_tmpF2.tag=6;_tmpF2.f1=0;
_tmpF2;});_tmpF1;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_For_s_struct*
_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({struct Cyc_Absyn_For_s_struct _tmpF4;
_tmpF4.tag=8;_tmpF4.f1=e1;_tmpF4.f2=({struct _tuple2 _tmpF5;_tmpF5.f1=e2;_tmpF5.f2=
Cyc_Absyn_skip_stmt(e3->loc);_tmpF5;});_tmpF4.f3=({struct _tuple2 _tmpF6;_tmpF6.f1=
e3;_tmpF6.f2=Cyc_Absyn_skip_stmt(e3->loc);_tmpF6;});_tmpF4.f4=s;_tmpF4;});_tmpF3;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_struct*
_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7[0]=({struct Cyc_Absyn_Switch_s_struct
_tmpF8;_tmpF8.tag=9;_tmpF8.f1=e;_tmpF8.f2=scs;_tmpF8;});_tmpF7;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc){struct _tuple9 _tmpFA=({struct _tuple9 _tmpF9;
_tmpF9.f1=(void*)s1->r;_tmpF9.f2=(void*)s2->r;_tmpF9;});void*_tmpFB;void*_tmpFC;
_LL3C: _tmpFB=_tmpFA.f1;if((int)_tmpFB != 0)goto _LL3E;_LL3D: return s2;_LL3E: _tmpFC=
_tmpFA.f2;if((int)_tmpFC != 0)goto _LL40;_LL3F: return s1;_LL40:;_LL41: return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Seq_s_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({
struct Cyc_Absyn_Seq_s_struct _tmpFE;_tmpFE.tag=1;_tmpFE.f1=s1;_tmpFE.f2=s2;_tmpFE;});
_tmpFD;}),loc);_LL3B:;}struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*
el,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Fallthru_s_struct*
_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=({struct Cyc_Absyn_Fallthru_s_struct
_tmp100;_tmp100.tag=10;_tmp100.f1=el;_tmp100.f2=0;_tmp100;});_tmpFF;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp102;_tmp102.tag=11;_tmp102.f1=d;_tmp102.f2=s;_tmp102;});_tmp101;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*
d=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp105=_cycalloc(
sizeof(*_tmp105));_tmp105[0]=({struct Cyc_Absyn_Var_d_struct _tmp106;_tmp106.tag=0;
_tmp106.f1=Cyc_Absyn_new_vardecl(x,t,init);_tmp106;});_tmp105;}),loc);return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Decl_s_struct*_tmp103=_cycalloc(sizeof(*_tmp103));
_tmp103[0]=({struct Cyc_Absyn_Decl_s_struct _tmp104;_tmp104.tag=11;_tmp104.f1=d;
_tmp104.f2=s;_tmp104;});_tmp103;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(
struct _tagged_arr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_struct*_tmp107=
_cycalloc(sizeof(*_tmp107));_tmp107[0]=({struct Cyc_Absyn_Label_s_struct _tmp108;
_tmp108.tag=12;_tmp108.f1=v;_tmp108.f2=s;_tmp108;});_tmp107;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Do_s_struct*_tmp109=
_cycalloc(sizeof(*_tmp109));_tmp109[0]=({struct Cyc_Absyn_Do_s_struct _tmp10A;
_tmp10A.tag=13;_tmp10A.f1=s;_tmp10A.f2=({struct _tuple2 _tmp10B;_tmp10B.f1=e;
_tmp10B.f2=Cyc_Absyn_skip_stmt(e->loc);_tmp10B;});_tmp10A;});_tmp109;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_TryCatch_s_struct*
_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C[0]=({struct Cyc_Absyn_TryCatch_s_struct
_tmp10D;_tmp10D.tag=14;_tmp10D.f1=s;_tmp10D.f2=scs;_tmp10D;});_tmp10C;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _tagged_arr*lab,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Goto_s_struct*_tmp10E=
_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=({struct Cyc_Absyn_Goto_s_struct _tmp10F;
_tmp10F.tag=7;_tmp10F.f1=lab;_tmp10F.f2=0;_tmp10F;});_tmp10E;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s){return({struct Cyc_Absyn_Pat*
_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->r=(void*)p;_tmp110->topt=0;_tmp110->loc=
s;_tmp110;});}struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_struct*_tmp111=_cycalloc(
sizeof(*_tmp111));_tmp111[0]=({struct Cyc_Absyn_Exp_p_struct _tmp112;_tmp112.tag=
14;_tmp112.f1=e;_tmp112;});_tmp111;}),e->loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Decl*_tmp113=
_cycalloc(sizeof(*_tmp113));_tmp113->r=(void*)r;_tmp113->loc=loc;_tmp113;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Let_d_struct*
_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114[0]=({struct Cyc_Absyn_Let_d_struct
_tmp115;_tmp115.tag=2;_tmp115.f1=p;_tmp115.f2=0;_tmp115.f3=e;_tmp115;});_tmp114;}),
loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Letv_d_struct*_tmp116=
_cycalloc(sizeof(*_tmp116));_tmp116[0]=({struct Cyc_Absyn_Letv_d_struct _tmp117;
_tmp117.tag=3;_tmp117.f1=vds;_tmp117;});_tmp116;}),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){return({
struct Cyc_Absyn_Vardecl*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->sc=(void*)((
void*)2);_tmp118->name=x;_tmp118->tq=Cyc_Absyn_empty_tqual(0);_tmp118->type=(
void*)t;_tmp118->initializer=init;_tmp118->rgn=0;_tmp118->attributes=0;_tmp118->escapes=
0;_tmp118;});}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,
void*t,struct Cyc_Absyn_Exp*init){return({struct Cyc_Absyn_Vardecl*_tmp119=
_cycalloc(sizeof(*_tmp119));_tmp119->sc=(void*)((void*)0);_tmp119->name=x;
_tmp119->tq=Cyc_Absyn_empty_tqual(0);_tmp119->type=(void*)t;_tmp119->initializer=
init;_tmp119->rgn=0;_tmp119->attributes=0;_tmp119->escapes=0;_tmp119;});}struct
Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct
Cyc_List_List*po,struct Cyc_List_List*fs){return({struct Cyc_Absyn_AggrdeclImpl*
_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->exist_vars=exists;_tmp11A->rgn_po=
po;_tmp11A->fields=fs;_tmp11A;});}struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*
k,void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));
_tmp11B[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp11C;_tmp11C.tag=4;_tmp11C.f1=({
struct Cyc_Absyn_Aggrdecl*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->kind=(void*)
k;_tmp11D->sc=(void*)s;_tmp11D->name=n;_tmp11D->tvs=ts;_tmp11D->impl=i;_tmp11D->attributes=
atts;_tmp11D;});_tmp11C;});_tmp11B;}),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((
void*)0,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((void*)1,s,n,ts,i,
atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,
struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_xtunion,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp11E=
_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp11F;
_tmp11F.tag=5;_tmp11F.f1=({struct Cyc_Absyn_Tuniondecl*_tmp120=_cycalloc(sizeof(*
_tmp120));_tmp120->sc=(void*)s;_tmp120->name=n;_tmp120->tvs=ts;_tmp120->fields=
fs;_tmp120->is_xtunion=is_xtunion;_tmp120;});_tmp11F;});_tmp11E;}),loc);}static
struct _tuple1*Cyc_Absyn_expand_arg(struct _tuple1*a){return({struct _tuple1*
_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->f1=(*a).f1;_tmp121->f2=(*a).f2;
_tmp121->f3=Cyc_Absyn_pointer_expand((*a).f3);_tmp121;});}void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts){return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122[0]=({struct Cyc_Absyn_FnType_struct
_tmp123;_tmp123.tag=8;_tmp123.f1=({struct Cyc_Absyn_FnInfo _tmp124;_tmp124.tvars=
tvs;_tmp124.ret_typ=(void*)Cyc_Absyn_pointer_expand(ret_typ);_tmp124.effect=
eff_typ;_tmp124.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple1*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,args);_tmp124.c_varargs=
c_varargs;_tmp124.cyc_varargs=cyc_varargs;_tmp124.rgn_po=rgn_po;_tmp124.attributes=
atts;_tmp124;});_tmp123;});_tmp122;});}void*Cyc_Absyn_pointer_expand(void*t){
void*_tmp125=Cyc_Tcutil_compress(t);_LL43: if(_tmp125 <= (void*)4  || *((int*)
_tmp125)!= 8)goto _LL45;_LL44: return Cyc_Absyn_at_typ(t,(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_LL45:;_LL46: return t;_LL42:;}int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*e){void*_tmp126=(void*)e->r;void*_tmp127;void*_tmp128;struct
Cyc_Absyn_Vardecl*_tmp129;void*_tmp12A;struct Cyc_Absyn_Vardecl*_tmp12B;struct Cyc_Absyn_Exp*
_tmp12C;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12E;_LL48: if(*((int*)
_tmp126)!= 1)goto _LL4A;_tmp127=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp126)->f2;
if(_tmp127 <= (void*)1  || *((int*)_tmp127)!= 1)goto _LL4A;_LL49: return 0;_LL4A: if(*((
int*)_tmp126)!= 1)goto _LL4C;_tmp128=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp126)->f2;if(_tmp128 <= (void*)1  || *((int*)_tmp128)!= 0)goto _LL4C;_tmp129=((
struct Cyc_Absyn_Global_b_struct*)_tmp128)->f1;_LL4B: _tmp12B=_tmp129;goto _LL4D;
_LL4C: if(*((int*)_tmp126)!= 1)goto _LL4E;_tmp12A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp126)->f2;if(_tmp12A <= (void*)1  || *((int*)_tmp12A)!= 3)goto _LL4E;_tmp12B=((
struct Cyc_Absyn_Local_b_struct*)_tmp12A)->f1;_LL4D: {void*_tmp12F=Cyc_Tcutil_compress((
void*)_tmp12B->type);_LL5F: if(_tmp12F <= (void*)4  || *((int*)_tmp12F)!= 7)goto
_LL61;_LL60: return 0;_LL61:;_LL62: return 1;_LL5E:;}_LL4E: if(*((int*)_tmp126)!= 1)
goto _LL50;_LL4F: goto _LL51;_LL50: if(*((int*)_tmp126)!= 24)goto _LL52;_LL51: goto
_LL53;_LL52: if(*((int*)_tmp126)!= 22)goto _LL54;_LL53: goto _LL55;_LL54: if(*((int*)
_tmp126)!= 25)goto _LL56;_LL55: return 1;_LL56: if(*((int*)_tmp126)!= 23)goto _LL58;
_tmp12C=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp126)->f1;_LL57: return Cyc_Absyn_is_lvalue(
_tmp12C);_LL58: if(*((int*)_tmp126)!= 14)goto _LL5A;_tmp12D=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp126)->f1;_LL59: return Cyc_Absyn_is_lvalue(_tmp12D);_LL5A: if(*((int*)_tmp126)
!= 13)goto _LL5C;_tmp12E=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp126)->f1;
_LL5B: return Cyc_Absyn_is_lvalue(_tmp12E);_LL5C:;_LL5D: return 0;_LL47:;}struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _tagged_arr*v){{struct
Cyc_List_List*_tmp130=fields;for(0;_tmp130 != 0;_tmp130=_tmp130->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Aggrfield*)_tmp130->hd)->name,v)== 0)return(struct Cyc_Absyn_Aggrfield*)((
struct Cyc_Absyn_Aggrfield*)_tmp130->hd);}}return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*ad,struct _tagged_arr*v){return ad->impl == 0?0: Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple3*
Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts
== 0)return 0;ts=ts->tl;}if(ts == 0)return 0;return(struct _tuple3*)((struct _tuple3*)
ts->hd);}struct _tagged_arr Cyc_Absyn_attribute2string(void*a){void*_tmp131=a;int
_tmp132;int _tmp133;struct _tagged_arr _tmp134;void*_tmp135;int _tmp136;int _tmp137;
void*_tmp138;int _tmp139;int _tmp13A;int _tmp13B;struct _tagged_arr _tmp13C;_LL64: if(
_tmp131 <= (void*)17  || *((int*)_tmp131)!= 0)goto _LL66;_tmp132=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp131)->f1;_LL65: return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp13F;
_tmp13F.tag=1;_tmp13F.f1=(unsigned int)_tmp132;{void*_tmp13D[1]={& _tmp13F};Cyc_aprintf(({
const char*_tmp13E="regparm(%d)";_tag_arr(_tmp13E,sizeof(char),_get_zero_arr_size(
_tmp13E,12));}),_tag_arr(_tmp13D,sizeof(void*),1));}});_LL66: if((int)_tmp131 != 0)
goto _LL68;_LL67: return({const char*_tmp140="stdcall";_tag_arr(_tmp140,sizeof(char),
_get_zero_arr_size(_tmp140,8));});_LL68: if((int)_tmp131 != 1)goto _LL6A;_LL69:
return({const char*_tmp141="cdecl";_tag_arr(_tmp141,sizeof(char),
_get_zero_arr_size(_tmp141,6));});_LL6A: if((int)_tmp131 != 2)goto _LL6C;_LL6B:
return({const char*_tmp142="fastcall";_tag_arr(_tmp142,sizeof(char),
_get_zero_arr_size(_tmp142,9));});_LL6C: if((int)_tmp131 != 3)goto _LL6E;_LL6D:
return({const char*_tmp143="noreturn";_tag_arr(_tmp143,sizeof(char),
_get_zero_arr_size(_tmp143,9));});_LL6E: if((int)_tmp131 != 4)goto _LL70;_LL6F:
return({const char*_tmp144="const";_tag_arr(_tmp144,sizeof(char),
_get_zero_arr_size(_tmp144,6));});_LL70: if(_tmp131 <= (void*)17  || *((int*)
_tmp131)!= 1)goto _LL72;_tmp133=((struct Cyc_Absyn_Aligned_att_struct*)_tmp131)->f1;
_LL71: if(_tmp133 == - 1)return({const char*_tmp145="aligned";_tag_arr(_tmp145,
sizeof(char),_get_zero_arr_size(_tmp145,8));});else{return(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp148;_tmp148.tag=1;_tmp148.f1=(unsigned int)_tmp133;{
void*_tmp146[1]={& _tmp148};Cyc_aprintf(({const char*_tmp147="aligned(%d)";
_tag_arr(_tmp147,sizeof(char),_get_zero_arr_size(_tmp147,12));}),_tag_arr(
_tmp146,sizeof(void*),1));}});}_LL72: if((int)_tmp131 != 5)goto _LL74;_LL73: return({
const char*_tmp149="packed";_tag_arr(_tmp149,sizeof(char),_get_zero_arr_size(
_tmp149,7));});_LL74: if(_tmp131 <= (void*)17  || *((int*)_tmp131)!= 2)goto _LL76;
_tmp134=((struct Cyc_Absyn_Section_att_struct*)_tmp131)->f1;_LL75: return(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp14C;_tmp14C.tag=0;_tmp14C.f1=(struct
_tagged_arr)((struct _tagged_arr)_tmp134);{void*_tmp14A[1]={& _tmp14C};Cyc_aprintf(({
const char*_tmp14B="section(\"%s\")";_tag_arr(_tmp14B,sizeof(char),
_get_zero_arr_size(_tmp14B,14));}),_tag_arr(_tmp14A,sizeof(void*),1));}});_LL76:
if((int)_tmp131 != 6)goto _LL78;_LL77: return({const char*_tmp14D="nocommon";
_tag_arr(_tmp14D,sizeof(char),_get_zero_arr_size(_tmp14D,9));});_LL78: if((int)
_tmp131 != 7)goto _LL7A;_LL79: return({const char*_tmp14E="shared";_tag_arr(_tmp14E,
sizeof(char),_get_zero_arr_size(_tmp14E,7));});_LL7A: if((int)_tmp131 != 8)goto
_LL7C;_LL7B: return({const char*_tmp14F="unused";_tag_arr(_tmp14F,sizeof(char),
_get_zero_arr_size(_tmp14F,7));});_LL7C: if((int)_tmp131 != 9)goto _LL7E;_LL7D:
return({const char*_tmp150="weak";_tag_arr(_tmp150,sizeof(char),
_get_zero_arr_size(_tmp150,5));});_LL7E: if((int)_tmp131 != 10)goto _LL80;_LL7F:
return({const char*_tmp151="dllimport";_tag_arr(_tmp151,sizeof(char),
_get_zero_arr_size(_tmp151,10));});_LL80: if((int)_tmp131 != 11)goto _LL82;_LL81:
return({const char*_tmp152="dllexport";_tag_arr(_tmp152,sizeof(char),
_get_zero_arr_size(_tmp152,10));});_LL82: if((int)_tmp131 != 12)goto _LL84;_LL83:
return({const char*_tmp153="no_instrument_function";_tag_arr(_tmp153,sizeof(char),
_get_zero_arr_size(_tmp153,23));});_LL84: if((int)_tmp131 != 13)goto _LL86;_LL85:
return({const char*_tmp154="constructor";_tag_arr(_tmp154,sizeof(char),
_get_zero_arr_size(_tmp154,12));});_LL86: if((int)_tmp131 != 14)goto _LL88;_LL87:
return({const char*_tmp155="destructor";_tag_arr(_tmp155,sizeof(char),
_get_zero_arr_size(_tmp155,11));});_LL88: if((int)_tmp131 != 15)goto _LL8A;_LL89:
return({const char*_tmp156="no_check_memory_usage";_tag_arr(_tmp156,sizeof(char),
_get_zero_arr_size(_tmp156,22));});_LL8A: if(_tmp131 <= (void*)17  || *((int*)
_tmp131)!= 3)goto _LL8C;_tmp135=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp131)->f1;if((int)_tmp135 != 0)goto _LL8C;_tmp136=((struct Cyc_Absyn_Format_att_struct*)
_tmp131)->f2;_tmp137=((struct Cyc_Absyn_Format_att_struct*)_tmp131)->f3;_LL8B:
return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp15A;_tmp15A.tag=1;_tmp15A.f1=(
unsigned int)_tmp137;{struct Cyc_Int_pa_struct _tmp159;_tmp159.tag=1;_tmp159.f1=(
unsigned int)_tmp136;{void*_tmp157[2]={& _tmp159,& _tmp15A};Cyc_aprintf(({const
char*_tmp158="format(printf,%u,%u)";_tag_arr(_tmp158,sizeof(char),
_get_zero_arr_size(_tmp158,21));}),_tag_arr(_tmp157,sizeof(void*),2));}}});_LL8C:
if(_tmp131 <= (void*)17  || *((int*)_tmp131)!= 3)goto _LL8E;_tmp138=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp131)->f1;if((int)_tmp138 != 1)goto _LL8E;_tmp139=((
struct Cyc_Absyn_Format_att_struct*)_tmp131)->f2;_tmp13A=((struct Cyc_Absyn_Format_att_struct*)
_tmp131)->f3;_LL8D: return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp15E;
_tmp15E.tag=1;_tmp15E.f1=(unsigned int)_tmp13A;{struct Cyc_Int_pa_struct _tmp15D;
_tmp15D.tag=1;_tmp15D.f1=(unsigned int)_tmp139;{void*_tmp15B[2]={& _tmp15D,&
_tmp15E};Cyc_aprintf(({const char*_tmp15C="format(scanf,%u,%u)";_tag_arr(_tmp15C,
sizeof(char),_get_zero_arr_size(_tmp15C,20));}),_tag_arr(_tmp15B,sizeof(void*),2));}}});
_LL8E: if(_tmp131 <= (void*)17  || *((int*)_tmp131)!= 4)goto _LL90;_tmp13B=((struct
Cyc_Absyn_Initializes_att_struct*)_tmp131)->f1;_LL8F: return(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp161;_tmp161.tag=1;_tmp161.f1=(unsigned int)_tmp13B;{
void*_tmp15F[1]={& _tmp161};Cyc_aprintf(({const char*_tmp160="initializes(%d)";
_tag_arr(_tmp160,sizeof(char),_get_zero_arr_size(_tmp160,16));}),_tag_arr(
_tmp15F,sizeof(void*),1));}});_LL90: if((int)_tmp131 != 16)goto _LL92;_LL91: return({
const char*_tmp162="pure";_tag_arr(_tmp162,sizeof(char),_get_zero_arr_size(
_tmp162,5));});_LL92: if(_tmp131 <= (void*)17  || *((int*)_tmp131)!= 5)goto _LL63;
_tmp13C=((struct Cyc_Absyn_Mode_att_struct*)_tmp131)->f1;_LL93: return(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp165;_tmp165.tag=0;_tmp165.f1=(struct
_tagged_arr)((struct _tagged_arr)_tmp13C);{void*_tmp163[1]={& _tmp165};Cyc_aprintf(({
const char*_tmp164="__mode__(\"%s\")";_tag_arr(_tmp164,sizeof(char),
_get_zero_arr_size(_tmp164,15));}),_tag_arr(_tmp163,sizeof(void*),1));}});_LL63:;}
int Cyc_Absyn_fntype_att(void*a){void*_tmp166=a;_LL95: if(_tmp166 <= (void*)17  || *((
int*)_tmp166)!= 0)goto _LL97;_LL96: goto _LL98;_LL97: if((int)_tmp166 != 2)goto _LL99;
_LL98: goto _LL9A;_LL99: if((int)_tmp166 != 0)goto _LL9B;_LL9A: goto _LL9C;_LL9B: if((
int)_tmp166 != 1)goto _LL9D;_LL9C: goto _LL9E;_LL9D: if((int)_tmp166 != 3)goto _LL9F;
_LL9E: goto _LLA0;_LL9F: if((int)_tmp166 != 16)goto _LLA1;_LLA0: goto _LLA2;_LLA1: if(
_tmp166 <= (void*)17  || *((int*)_tmp166)!= 3)goto _LLA3;_LLA2: goto _LLA4;_LLA3: if((
int)_tmp166 != 4)goto _LLA5;_LLA4: return 1;_LLA5: if(_tmp166 <= (void*)17  || *((int*)
_tmp166)!= 4)goto _LLA7;_LLA6: return 1;_LLA7:;_LLA8: return 0;_LL94:;}static char
_tmp167[3]="f0";static struct _tagged_arr Cyc_Absyn_f0={_tmp167,_tmp167,_tmp167 + 3};
static struct _tagged_arr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};static struct
_tagged_arr Cyc_Absyn_field_names={(void*)((struct _tagged_arr**)Cyc_Absyn_field_names_v),(
void*)((struct _tagged_arr**)Cyc_Absyn_field_names_v),(void*)((struct _tagged_arr**)
Cyc_Absyn_field_names_v + 1)};struct _tagged_arr*Cyc_Absyn_fieldname(int i){
unsigned int fsz=_get_arr_size(Cyc_Absyn_field_names,sizeof(struct _tagged_arr*));
if(i >= fsz)Cyc_Absyn_field_names=({unsigned int _tmp168=(unsigned int)(i + 1);
struct _tagged_arr**_tmp169=(struct _tagged_arr**)_cycalloc(_check_times(sizeof(
struct _tagged_arr*),_tmp168));struct _tagged_arr _tmp16F=_tag_arr(_tmp169,sizeof(
struct _tagged_arr*),_tmp168);{unsigned int _tmp16A=_tmp168;unsigned int j;for(j=0;
j < _tmp16A;j ++){_tmp169[j]=j < fsz?*((struct _tagged_arr**)
_check_unknown_subscript(Cyc_Absyn_field_names,sizeof(struct _tagged_arr*),(int)j)):({
struct _tagged_arr*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B[0]=(struct
_tagged_arr)({struct Cyc_Int_pa_struct _tmp16E;_tmp16E.tag=1;_tmp16E.f1=(
unsigned int)((int)j);{void*_tmp16C[1]={& _tmp16E};Cyc_aprintf(({const char*
_tmp16D="f%d";_tag_arr(_tmp16D,sizeof(char),_get_zero_arr_size(_tmp16D,4));}),
_tag_arr(_tmp16C,sizeof(void*),1));}});_tmp16B;});}}_tmp16F;});return*((struct
_tagged_arr**)_check_unknown_subscript(Cyc_Absyn_field_names,sizeof(struct
_tagged_arr*),i));}struct _tuple4 Cyc_Absyn_aggr_kinded_name(void*info){void*
_tmp170=info;void*_tmp171;struct _tuple0*_tmp172;struct Cyc_Absyn_Aggrdecl**
_tmp173;struct Cyc_Absyn_Aggrdecl*_tmp174;struct Cyc_Absyn_Aggrdecl _tmp175;void*
_tmp176;struct _tuple0*_tmp177;_LLAA: if(*((int*)_tmp170)!= 0)goto _LLAC;_tmp171=(
void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp170)->f1;_tmp172=((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp170)->f2;_LLAB: return({struct _tuple4 _tmp178;_tmp178.f1=_tmp171;_tmp178.f2=
_tmp172;_tmp178;});_LLAC: if(*((int*)_tmp170)!= 1)goto _LLA9;_tmp173=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp170)->f1;_tmp174=*_tmp173;_tmp175=*_tmp174;_tmp176=(void*)_tmp175.kind;
_tmp177=_tmp175.name;_LLAD: return({struct _tuple4 _tmp179;_tmp179.f1=_tmp176;
_tmp179.f2=_tmp177;_tmp179;});_LLA9:;}struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
void*info){void*_tmp17A=info;void*_tmp17B;struct _tuple0*_tmp17C;struct Cyc_Absyn_Aggrdecl**
_tmp17D;struct Cyc_Absyn_Aggrdecl*_tmp17E;_LLAF: if(*((int*)_tmp17A)!= 0)goto _LLB1;
_tmp17B=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp17A)->f1;_tmp17C=((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp17A)->f2;_LLB0:({void*_tmp17F[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp180="unchecked aggrdecl";_tag_arr(_tmp180,sizeof(char),_get_zero_arr_size(
_tmp180,19));}),_tag_arr(_tmp17F,sizeof(void*),0));});_LLB1: if(*((int*)_tmp17A)
!= 1)goto _LLAE;_tmp17D=((struct Cyc_Absyn_KnownAggr_struct*)_tmp17A)->f1;_tmp17E=*
_tmp17D;_LLB2: return _tmp17E;_LLAE:;}int Cyc_Absyn_is_union_type(void*t){void*
_tmp181=Cyc_Tcutil_compress(t);void*_tmp182;struct Cyc_Absyn_AggrInfo _tmp183;void*
_tmp184;_LLB4: if(_tmp181 <= (void*)4  || *((int*)_tmp181)!= 11)goto _LLB6;_tmp182=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp181)->f1;if((int)_tmp182 != 1)
goto _LLB6;_LLB5: return 1;_LLB6: if(_tmp181 <= (void*)4  || *((int*)_tmp181)!= 10)
goto _LLB8;_tmp183=((struct Cyc_Absyn_AggrType_struct*)_tmp181)->f1;_tmp184=(void*)
_tmp183.aggr_info;_LLB7: return(Cyc_Absyn_aggr_kinded_name(_tmp184)).f1 == (void*)
1;_LLB8:;_LLB9: return 0;_LLB3:;}void Cyc_Absyn_print_decls(struct Cyc_List_List*
decls){((void(*)(void*rep,struct Cyc_List_List**val))Cyc_Marshal_print_type)(Cyc_decls_rep,&
decls);({void*_tmp185[0]={};Cyc_printf(({const char*_tmp186="\n";_tag_arr(_tmp186,
sizeof(char),_get_zero_arr_size(_tmp186,2));}),_tag_arr(_tmp185,sizeof(void*),0));});}
int Cyc_Absyn_porting_c_code=0;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Decl_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_443;static struct Cyc_Typerep_Int_struct
Cyc__genrep_24={0,0,32};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_159;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep;extern struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep;static char _tmp188[7]="Static";
static struct _tuple7 Cyc__gentuple_167={0,(struct _tagged_arr){_tmp188,_tmp188,
_tmp188 + 7}};static char _tmp189[9]="Abstract";static struct _tuple7 Cyc__gentuple_168={
1,(struct _tagged_arr){_tmp189,_tmp189,_tmp189 + 9}};static char _tmp18A[7]="Public";
static struct _tuple7 Cyc__gentuple_169={2,(struct _tagged_arr){_tmp18A,_tmp18A,
_tmp18A + 7}};static char _tmp18B[7]="Extern";static struct _tuple7 Cyc__gentuple_170={
3,(struct _tagged_arr){_tmp18B,_tmp18B,_tmp18B + 7}};static char _tmp18C[8]="ExternC";
static struct _tuple7 Cyc__gentuple_171={4,(struct _tagged_arr){_tmp18C,_tmp18C,
_tmp18C + 8}};static char _tmp18D[9]="Register";static struct _tuple7 Cyc__gentuple_172={
5,(struct _tagged_arr){_tmp18D,_tmp18D,_tmp18D + 9}};static struct _tuple7*Cyc__genarr_173[
6]={& Cyc__gentuple_167,& Cyc__gentuple_168,& Cyc__gentuple_169,& Cyc__gentuple_170,&
Cyc__gentuple_171,& Cyc__gentuple_172};static struct _tuple5*Cyc__genarr_174[0]={};
static char _tmp18F[6]="Scope";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep={
5,(struct _tagged_arr){_tmp18F,_tmp18F,_tmp18F + 6},{(void*)((struct _tuple7**)Cyc__genarr_173),(
void*)((struct _tuple7**)Cyc__genarr_173),(void*)((struct _tuple7**)Cyc__genarr_173
+ 6)},{(void*)((struct _tuple5**)Cyc__genarr_174),(void*)((struct _tuple5**)Cyc__genarr_174),(
void*)((struct _tuple5**)Cyc__genarr_174 + 0)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_11;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_12;extern struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_18;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_19;extern struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_var_t46H2_rep;extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_13;extern struct Cyc_Typerep_FatPtr_struct
Cyc__genrep_14;static struct Cyc_Typerep_Int_struct Cyc__genrep_15={0,0,8};static
struct Cyc_Typerep_FatPtr_struct Cyc__genrep_14={2,(void*)((void*)& Cyc__genrep_15)};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_13={1,1,(void*)((void*)& Cyc__genrep_14)};
static char _tmp193[5]="List";static struct _tagged_arr Cyc__genname_23=(struct
_tagged_arr){_tmp193,_tmp193,_tmp193 + 5};static char _tmp194[3]="hd";static struct
_tuple5 Cyc__gentuple_20={offsetof(struct Cyc_List_List,hd),(struct _tagged_arr){
_tmp194,_tmp194,_tmp194 + 3},(void*)& Cyc__genrep_13};static char _tmp195[3]="tl";
static struct _tuple5 Cyc__gentuple_21={offsetof(struct Cyc_List_List,tl),(struct
_tagged_arr){_tmp195,_tmp195,_tmp195 + 3},(void*)& Cyc__genrep_19};static struct
_tuple5*Cyc__genarr_22[2]={& Cyc__gentuple_20,& Cyc__gentuple_21};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_var_t46H2_rep={3,(struct _tagged_arr*)& Cyc__genname_23,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_22),(void*)((
struct _tuple5**)Cyc__genarr_22),(void*)((struct _tuple5**)Cyc__genarr_22 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_19={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_var_t46H2_rep)};
struct _tuple13{unsigned int f1;struct Cyc_List_List*f2;};static struct _tuple6 Cyc__gentuple_25={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_26={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_19};static struct _tuple6*Cyc__genarr_27[
2]={& Cyc__gentuple_25,& Cyc__gentuple_26};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_18={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_27),(void*)((
struct _tuple6**)Cyc__genarr_27),(void*)((struct _tuple6**)Cyc__genarr_27 + 2)}};
static char _tmp199[6]="Loc_n";static struct _tuple7 Cyc__gentuple_16={0,(struct
_tagged_arr){_tmp199,_tmp199,_tmp199 + 6}};static struct _tuple7*Cyc__genarr_17[1]={&
Cyc__gentuple_16};static char _tmp19A[6]="Rel_n";static struct _tuple5 Cyc__gentuple_28={
0,(struct _tagged_arr){_tmp19A,_tmp19A,_tmp19A + 6},(void*)& Cyc__genrep_18};static
char _tmp19B[6]="Abs_n";static struct _tuple5 Cyc__gentuple_29={1,(struct _tagged_arr){
_tmp19B,_tmp19B,_tmp19B + 6},(void*)& Cyc__genrep_18};static struct _tuple5*Cyc__genarr_30[
2]={& Cyc__gentuple_28,& Cyc__gentuple_29};static char _tmp19D[8]="Nmspace";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep={5,(struct _tagged_arr){_tmp19D,
_tmp19D,_tmp19D + 8},{(void*)((struct _tuple7**)Cyc__genarr_17),(void*)((struct
_tuple7**)Cyc__genarr_17),(void*)((struct _tuple7**)Cyc__genarr_17 + 1)},{(void*)((
struct _tuple5**)Cyc__genarr_30),(void*)((struct _tuple5**)Cyc__genarr_30),(void*)((
struct _tuple5**)Cyc__genarr_30 + 2)}};static struct _tuple6 Cyc__gentuple_31={
offsetof(struct _tuple0,f1),(void*)& Cyc_Absyn_nmspace_t_rep};static struct _tuple6
Cyc__gentuple_32={offsetof(struct _tuple0,f2),(void*)& Cyc__genrep_13};static
struct _tuple6*Cyc__genarr_33[2]={& Cyc__gentuple_31,& Cyc__gentuple_32};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_12={4,sizeof(struct _tuple0),{(void*)((
struct _tuple6**)Cyc__genarr_33),(void*)((struct _tuple6**)Cyc__genarr_33),(void*)((
struct _tuple6**)Cyc__genarr_33 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11={
1,1,(void*)((void*)& Cyc__genrep_12)};extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_tqual_t_rep;
static struct Cyc_Typerep_Int_struct Cyc__genrep_10={0,1,32};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_2;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2={1,1,(void*)((void*)& Cyc_struct_Position_Segment_rep)};
static char _tmp1A2[6]="Tqual";static struct _tagged_arr Cyc__genname_166=(struct
_tagged_arr){_tmp1A2,_tmp1A2,_tmp1A2 + 6};static char _tmp1A3[12]="print_const";
static struct _tuple5 Cyc__gentuple_160={offsetof(struct Cyc_Absyn_Tqual,print_const),(
struct _tagged_arr){_tmp1A3,_tmp1A3,_tmp1A3 + 12},(void*)((void*)& Cyc__genrep_10)};
static char _tmp1A4[11]="q_volatile";static struct _tuple5 Cyc__gentuple_161={
offsetof(struct Cyc_Absyn_Tqual,q_volatile),(struct _tagged_arr){_tmp1A4,_tmp1A4,
_tmp1A4 + 11},(void*)((void*)& Cyc__genrep_10)};static char _tmp1A5[11]="q_restrict";
static struct _tuple5 Cyc__gentuple_162={offsetof(struct Cyc_Absyn_Tqual,q_restrict),(
struct _tagged_arr){_tmp1A5,_tmp1A5,_tmp1A5 + 11},(void*)((void*)& Cyc__genrep_10)};
static char _tmp1A6[11]="real_const";static struct _tuple5 Cyc__gentuple_163={
offsetof(struct Cyc_Absyn_Tqual,real_const),(struct _tagged_arr){_tmp1A6,_tmp1A6,
_tmp1A6 + 11},(void*)((void*)& Cyc__genrep_10)};static char _tmp1A7[4]="loc";static
struct _tuple5 Cyc__gentuple_164={offsetof(struct Cyc_Absyn_Tqual,loc),(struct
_tagged_arr){_tmp1A7,_tmp1A7,_tmp1A7 + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_165[5]={& Cyc__gentuple_160,& Cyc__gentuple_161,& Cyc__gentuple_162,&
Cyc__gentuple_163,& Cyc__gentuple_164};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tqual_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_166,sizeof(struct Cyc_Absyn_Tqual),{(void*)((
struct _tuple5**)Cyc__genarr_165),(void*)((struct _tuple5**)Cyc__genarr_165),(void*)((
struct _tuple5**)Cyc__genarr_165 + 5)}};extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1092;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1097;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep;static char _tmp1A9[8]="AnyKind";
static struct _tuple7 Cyc__gentuple_188={0,(struct _tagged_arr){_tmp1A9,_tmp1A9,
_tmp1A9 + 8}};static char _tmp1AA[8]="MemKind";static struct _tuple7 Cyc__gentuple_189={
1,(struct _tagged_arr){_tmp1AA,_tmp1AA,_tmp1AA + 8}};static char _tmp1AB[8]="BoxKind";
static struct _tuple7 Cyc__gentuple_190={2,(struct _tagged_arr){_tmp1AB,_tmp1AB,
_tmp1AB + 8}};static char _tmp1AC[8]="RgnKind";static struct _tuple7 Cyc__gentuple_191={
3,(struct _tagged_arr){_tmp1AC,_tmp1AC,_tmp1AC + 8}};static char _tmp1AD[8]="EffKind";
static struct _tuple7 Cyc__gentuple_192={4,(struct _tagged_arr){_tmp1AD,_tmp1AD,
_tmp1AD + 8}};static char _tmp1AE[8]="IntKind";static struct _tuple7 Cyc__gentuple_193={
5,(struct _tagged_arr){_tmp1AE,_tmp1AE,_tmp1AE + 8}};static struct _tuple7*Cyc__genarr_194[
6]={& Cyc__gentuple_188,& Cyc__gentuple_189,& Cyc__gentuple_190,& Cyc__gentuple_191,&
Cyc__gentuple_192,& Cyc__gentuple_193};static struct _tuple5*Cyc__genarr_195[0]={};
static char _tmp1B0[5]="Kind";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep={
5,(struct _tagged_arr){_tmp1B0,_tmp1B0,_tmp1B0 + 5},{(void*)((struct _tuple7**)Cyc__genarr_194),(
void*)((struct _tuple7**)Cyc__genarr_194),(void*)((struct _tuple7**)Cyc__genarr_194
+ 6)},{(void*)((struct _tuple5**)Cyc__genarr_195),(void*)((struct _tuple5**)Cyc__genarr_195),(
void*)((struct _tuple5**)Cyc__genarr_195 + 0)}};static char _tmp1B1[4]="Opt";static
struct _tagged_arr Cyc__genname_1100=(struct _tagged_arr){_tmp1B1,_tmp1B1,_tmp1B1 + 
4};static char _tmp1B2[2]="v";static struct _tuple5 Cyc__gentuple_1098={offsetof(
struct Cyc_Core_Opt,v),(struct _tagged_arr){_tmp1B2,_tmp1B2,_tmp1B2 + 2},(void*)&
Cyc_Absyn_kind_t_rep};static struct _tuple5*Cyc__genarr_1099[1]={& Cyc__gentuple_1098};
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep={3,(struct
_tagged_arr*)& Cyc__genname_1100,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_1099),(void*)((struct _tuple5**)Cyc__genarr_1099),(void*)((
struct _tuple5**)Cyc__genarr_1099 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1097={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kind_t2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_110;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_type_t2_rep;
static char _tmp1B5[4]="Opt";static struct _tagged_arr Cyc__genname_1132=(struct
_tagged_arr){_tmp1B5,_tmp1B5,_tmp1B5 + 4};static char _tmp1B6[2]="v";static struct
_tuple5 Cyc__gentuple_1130={offsetof(struct Cyc_Core_Opt,v),(struct _tagged_arr){
_tmp1B6,_tmp1B6,_tmp1B6 + 2},(void*)& Cyc_Absyn_type_t_rep};static struct _tuple5*
Cyc__genarr_1131[1]={& Cyc__gentuple_1130};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_type_t2_rep={
3,(struct _tagged_arr*)& Cyc__genname_1132,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_1131),(void*)((struct _tuple5**)Cyc__genarr_1131),(
void*)((struct _tuple5**)Cyc__genarr_1131 + 1)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_110={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_type_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1093;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_321;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tvar_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_185;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tvar_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_216;
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_216={1,1,(void*)((void*)& Cyc__genrep_10)};
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_208;static struct _tuple6 Cyc__gentuple_209={offsetof(struct _tuple6,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_210={offsetof(struct
_tuple6,f2),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple6*Cyc__genarr_211[2]={&
Cyc__gentuple_209,& Cyc__gentuple_210};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_208={
4,sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_211),(void*)((
struct _tuple6**)Cyc__genarr_211),(void*)((struct _tuple6**)Cyc__genarr_211 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_204;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_196;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep;
static char _tmp1BB[4]="Opt";static struct _tagged_arr Cyc__genname_199=(struct
_tagged_arr){_tmp1BB,_tmp1BB,_tmp1BB + 4};static char _tmp1BC[2]="v";static struct
_tuple5 Cyc__gentuple_197={offsetof(struct Cyc_Core_Opt,v),(struct _tagged_arr){
_tmp1BC,_tmp1BC,_tmp1BC + 2},(void*)& Cyc_Absyn_kindbound_t_rep};static struct
_tuple5*Cyc__genarr_198[1]={& Cyc__gentuple_197};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_199,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_198),(void*)((
struct _tuple5**)Cyc__genarr_198),(void*)((struct _tuple5**)Cyc__genarr_198 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_196={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep)};
struct _tuple14{unsigned int f1;struct Cyc_Core_Opt*f2;};static struct _tuple6 Cyc__gentuple_205={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_206={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_196};static struct _tuple6*Cyc__genarr_207[
2]={& Cyc__gentuple_205,& Cyc__gentuple_206};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_204={4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_207),(
void*)((struct _tuple6**)Cyc__genarr_207),(void*)((struct _tuple6**)Cyc__genarr_207
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_187;struct _tuple15{
unsigned int f1;struct Cyc_Core_Opt*f2;void*f3;};static struct _tuple6 Cyc__gentuple_200={
offsetof(struct _tuple15,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_201={
offsetof(struct _tuple15,f2),(void*)& Cyc__genrep_196};static struct _tuple6 Cyc__gentuple_202={
offsetof(struct _tuple15,f3),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple6*
Cyc__genarr_203[3]={& Cyc__gentuple_200,& Cyc__gentuple_201,& Cyc__gentuple_202};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_187={4,sizeof(struct _tuple15),{(
void*)((struct _tuple6**)Cyc__genarr_203),(void*)((struct _tuple6**)Cyc__genarr_203),(
void*)((struct _tuple6**)Cyc__genarr_203 + 3)}};static struct _tuple7*Cyc__genarr_186[
0]={};static char _tmp1C1[6]="Eq_kb";static struct _tuple5 Cyc__gentuple_212={0,(
struct _tagged_arr){_tmp1C1,_tmp1C1,_tmp1C1 + 6},(void*)& Cyc__genrep_208};static
char _tmp1C2[11]="Unknown_kb";static struct _tuple5 Cyc__gentuple_213={1,(struct
_tagged_arr){_tmp1C2,_tmp1C2,_tmp1C2 + 11},(void*)& Cyc__genrep_204};static char
_tmp1C3[8]="Less_kb";static struct _tuple5 Cyc__gentuple_214={2,(struct _tagged_arr){
_tmp1C3,_tmp1C3,_tmp1C3 + 8},(void*)& Cyc__genrep_187};static struct _tuple5*Cyc__genarr_215[
3]={& Cyc__gentuple_212,& Cyc__gentuple_213,& Cyc__gentuple_214};static char _tmp1C5[
10]="KindBound";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep={5,(
struct _tagged_arr){_tmp1C5,_tmp1C5,_tmp1C5 + 10},{(void*)((struct _tuple7**)Cyc__genarr_186),(
void*)((struct _tuple7**)Cyc__genarr_186),(void*)((struct _tuple7**)Cyc__genarr_186
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_215),(void*)((struct _tuple5**)Cyc__genarr_215),(
void*)((struct _tuple5**)Cyc__genarr_215 + 3)}};static char _tmp1C6[5]="Tvar";static
struct _tagged_arr Cyc__genname_221=(struct _tagged_arr){_tmp1C6,_tmp1C6,_tmp1C6 + 5};
static char _tmp1C7[5]="name";static struct _tuple5 Cyc__gentuple_217={offsetof(
struct Cyc_Absyn_Tvar,name),(struct _tagged_arr){_tmp1C7,_tmp1C7,_tmp1C7 + 5},(void*)&
Cyc__genrep_13};static char _tmp1C8[9]="identity";static struct _tuple5 Cyc__gentuple_218={
offsetof(struct Cyc_Absyn_Tvar,identity),(struct _tagged_arr){_tmp1C8,_tmp1C8,
_tmp1C8 + 9},(void*)& Cyc__genrep_216};static char _tmp1C9[5]="kind";static struct
_tuple5 Cyc__gentuple_219={offsetof(struct Cyc_Absyn_Tvar,kind),(struct _tagged_arr){
_tmp1C9,_tmp1C9,_tmp1C9 + 5},(void*)& Cyc_Absyn_kindbound_t_rep};static struct
_tuple5*Cyc__genarr_220[3]={& Cyc__gentuple_217,& Cyc__gentuple_218,& Cyc__gentuple_219};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep={3,(struct _tagged_arr*)&
Cyc__genname_221,sizeof(struct Cyc_Absyn_Tvar),{(void*)((struct _tuple5**)Cyc__genarr_220),(
void*)((struct _tuple5**)Cyc__genarr_220),(void*)((struct _tuple5**)Cyc__genarr_220
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_185={1,1,(void*)((void*)&
Cyc_struct_Absyn_Tvar_rep)};static char _tmp1CC[5]="List";static struct _tagged_arr
Cyc__genname_325=(struct _tagged_arr){_tmp1CC,_tmp1CC,_tmp1CC + 5};static char
_tmp1CD[3]="hd";static struct _tuple5 Cyc__gentuple_322={offsetof(struct Cyc_List_List,hd),(
struct _tagged_arr){_tmp1CD,_tmp1CD,_tmp1CD + 3},(void*)& Cyc__genrep_185};static
char _tmp1CE[3]="tl";static struct _tuple5 Cyc__gentuple_323={offsetof(struct Cyc_List_List,tl),(
struct _tagged_arr){_tmp1CE,_tmp1CE,_tmp1CE + 3},(void*)& Cyc__genrep_321};static
struct _tuple5*Cyc__genarr_324[2]={& Cyc__gentuple_322,& Cyc__gentuple_323};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tvar_t46H2_rep={3,(struct
_tagged_arr*)& Cyc__genname_325,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_324),(void*)((struct _tuple5**)Cyc__genarr_324),(void*)((
struct _tuple5**)Cyc__genarr_324 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_321={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tvar_t46H2_rep)};static char
_tmp1D1[4]="Opt";static struct _tagged_arr Cyc__genname_1096=(struct _tagged_arr){
_tmp1D1,_tmp1D1,_tmp1D1 + 4};static char _tmp1D2[2]="v";static struct _tuple5 Cyc__gentuple_1094={
offsetof(struct Cyc_Core_Opt,v),(struct _tagged_arr){_tmp1D2,_tmp1D2,_tmp1D2 + 2},(
void*)& Cyc__genrep_321};static struct _tuple5*Cyc__genarr_1095[1]={& Cyc__gentuple_1094};
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep={
3,(struct _tagged_arr*)& Cyc__genname_1096,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_1095),(void*)((struct _tuple5**)Cyc__genarr_1095),(
void*)((struct _tuple5**)Cyc__genarr_1095 + 1)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1093={1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep)};
struct _tuple16{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;int f4;
struct Cyc_Core_Opt*f5;};static struct _tuple6 Cyc__gentuple_1101={offsetof(struct
_tuple16,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1102={
offsetof(struct _tuple16,f2),(void*)& Cyc__genrep_1097};static struct _tuple6 Cyc__gentuple_1103={
offsetof(struct _tuple16,f3),(void*)& Cyc__genrep_110};static struct _tuple6 Cyc__gentuple_1104={
offsetof(struct _tuple16,f4),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6
Cyc__gentuple_1105={offsetof(struct _tuple16,f5),(void*)& Cyc__genrep_1093};static
struct _tuple6*Cyc__genarr_1106[5]={& Cyc__gentuple_1101,& Cyc__gentuple_1102,& Cyc__gentuple_1103,&
Cyc__gentuple_1104,& Cyc__gentuple_1105};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1092={
4,sizeof(struct _tuple16),{(void*)((struct _tuple6**)Cyc__genarr_1106),(void*)((
struct _tuple6**)Cyc__genarr_1106),(void*)((struct _tuple6**)Cyc__genarr_1106 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1088;struct _tuple17{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;};static struct _tuple6 Cyc__gentuple_1089={offsetof(
struct _tuple17,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1090={
offsetof(struct _tuple17,f2),(void*)& Cyc__genrep_185};static struct _tuple6*Cyc__genarr_1091[
2]={& Cyc__gentuple_1089,& Cyc__gentuple_1090};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1088={4,sizeof(struct _tuple17),{(void*)((struct _tuple6**)Cyc__genarr_1091),(
void*)((struct _tuple6**)Cyc__genarr_1091),(void*)((struct _tuple6**)Cyc__genarr_1091
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1062;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1069;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep;static char _tmp1D7[18]="UnknownTunionInfo";
static struct _tagged_arr Cyc__genname_1073=(struct _tagged_arr){_tmp1D7,_tmp1D7,
_tmp1D7 + 18};static char _tmp1D8[5]="name";static struct _tuple5 Cyc__gentuple_1070={
offsetof(struct Cyc_Absyn_UnknownTunionInfo,name),(struct _tagged_arr){_tmp1D8,
_tmp1D8,_tmp1D8 + 5},(void*)& Cyc__genrep_11};static char _tmp1D9[11]="is_xtunion";
static struct _tuple5 Cyc__gentuple_1071={offsetof(struct Cyc_Absyn_UnknownTunionInfo,is_xtunion),(
struct _tagged_arr){_tmp1D9,_tmp1D9,_tmp1D9 + 11},(void*)((void*)& Cyc__genrep_10)};
static struct _tuple5*Cyc__genarr_1072[2]={& Cyc__gentuple_1070,& Cyc__gentuple_1071};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_UnknownTunionInfo_rep={3,(struct
_tagged_arr*)& Cyc__genname_1073,sizeof(struct Cyc_Absyn_UnknownTunionInfo),{(void*)((
struct _tuple5**)Cyc__genarr_1072),(void*)((struct _tuple5**)Cyc__genarr_1072),(
void*)((struct _tuple5**)Cyc__genarr_1072 + 2)}};struct _tuple18{unsigned int f1;
struct Cyc_Absyn_UnknownTunionInfo f2;};static struct _tuple6 Cyc__gentuple_1074={
offsetof(struct _tuple18,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1075={
offsetof(struct _tuple18,f2),(void*)& Cyc_struct_Absyn_UnknownTunionInfo_rep};
static struct _tuple6*Cyc__genarr_1076[2]={& Cyc__gentuple_1074,& Cyc__gentuple_1075};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1069={4,sizeof(struct _tuple18),{(
void*)((struct _tuple6**)Cyc__genarr_1076),(void*)((struct _tuple6**)Cyc__genarr_1076),(
void*)((struct _tuple6**)Cyc__genarr_1076 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1064;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1065;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_311;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tuniondecl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_312;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_313;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_294;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_295;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_296;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_297;static struct
_tuple6 Cyc__gentuple_298={offsetof(struct _tuple3,f1),(void*)& Cyc_Absyn_tqual_t_rep};
static struct _tuple6 Cyc__gentuple_299={offsetof(struct _tuple3,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_300[2]={& Cyc__gentuple_298,&
Cyc__gentuple_299};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_297={4,
sizeof(struct _tuple3),{(void*)((struct _tuple6**)Cyc__genarr_300),(void*)((struct
_tuple6**)Cyc__genarr_300),(void*)((struct _tuple6**)Cyc__genarr_300 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_296={1,1,(void*)((void*)& Cyc__genrep_297)};
static char _tmp1DE[5]="List";static struct _tagged_arr Cyc__genname_304=(struct
_tagged_arr){_tmp1DE,_tmp1DE,_tmp1DE + 5};static char _tmp1DF[3]="hd";static struct
_tuple5 Cyc__gentuple_301={offsetof(struct Cyc_List_List,hd),(struct _tagged_arr){
_tmp1DF,_tmp1DF,_tmp1DF + 3},(void*)& Cyc__genrep_296};static char _tmp1E0[3]="tl";
static struct _tuple5 Cyc__gentuple_302={offsetof(struct Cyc_List_List,tl),(struct
_tagged_arr){_tmp1E0,_tmp1E0,_tmp1E0 + 3},(void*)& Cyc__genrep_295};static struct
_tuple5*Cyc__genarr_303[2]={& Cyc__gentuple_301,& Cyc__gentuple_302};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep={3,(struct
_tagged_arr*)& Cyc__genname_304,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_303),(void*)((struct _tuple5**)Cyc__genarr_303),(void*)((
struct _tuple5**)Cyc__genarr_303 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_295={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
static char _tmp1E3[12]="Tunionfield";static struct _tagged_arr Cyc__genname_310=(
struct _tagged_arr){_tmp1E3,_tmp1E3,_tmp1E3 + 12};static char _tmp1E4[5]="name";
static struct _tuple5 Cyc__gentuple_305={offsetof(struct Cyc_Absyn_Tunionfield,name),(
struct _tagged_arr){_tmp1E4,_tmp1E4,_tmp1E4 + 5},(void*)& Cyc__genrep_11};static
char _tmp1E5[5]="typs";static struct _tuple5 Cyc__gentuple_306={offsetof(struct Cyc_Absyn_Tunionfield,typs),(
struct _tagged_arr){_tmp1E5,_tmp1E5,_tmp1E5 + 5},(void*)& Cyc__genrep_295};static
char _tmp1E6[4]="loc";static struct _tuple5 Cyc__gentuple_307={offsetof(struct Cyc_Absyn_Tunionfield,loc),(
struct _tagged_arr){_tmp1E6,_tmp1E6,_tmp1E6 + 4},(void*)& Cyc__genrep_2};static char
_tmp1E7[3]="sc";static struct _tuple5 Cyc__gentuple_308={offsetof(struct Cyc_Absyn_Tunionfield,sc),(
struct _tagged_arr){_tmp1E7,_tmp1E7,_tmp1E7 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static struct _tuple5*Cyc__genarr_309[4]={& Cyc__gentuple_305,& Cyc__gentuple_306,&
Cyc__gentuple_307,& Cyc__gentuple_308};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep={
3,(struct _tagged_arr*)& Cyc__genname_310,sizeof(struct Cyc_Absyn_Tunionfield),{(
void*)((struct _tuple5**)Cyc__genarr_309),(void*)((struct _tuple5**)Cyc__genarr_309),(
void*)((struct _tuple5**)Cyc__genarr_309 + 4)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_294={1,1,(void*)((void*)& Cyc_struct_Absyn_Tunionfield_rep)};static
char _tmp1EA[5]="List";static struct _tagged_arr Cyc__genname_317=(struct _tagged_arr){
_tmp1EA,_tmp1EA,_tmp1EA + 5};static char _tmp1EB[3]="hd";static struct _tuple5 Cyc__gentuple_314={
offsetof(struct Cyc_List_List,hd),(struct _tagged_arr){_tmp1EB,_tmp1EB,_tmp1EB + 3},(
void*)& Cyc__genrep_294};static char _tmp1EC[3]="tl";static struct _tuple5 Cyc__gentuple_315={
offsetof(struct Cyc_List_List,tl),(struct _tagged_arr){_tmp1EC,_tmp1EC,_tmp1EC + 3},(
void*)& Cyc__genrep_313};static struct _tuple5*Cyc__genarr_316[2]={& Cyc__gentuple_314,&
Cyc__gentuple_315};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_317,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_316),(void*)((struct _tuple5**)Cyc__genarr_316),(void*)((
struct _tuple5**)Cyc__genarr_316 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_313={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep)};static
char _tmp1EF[4]="Opt";static struct _tagged_arr Cyc__genname_320=(struct _tagged_arr){
_tmp1EF,_tmp1EF,_tmp1EF + 4};static char _tmp1F0[2]="v";static struct _tuple5 Cyc__gentuple_318={
offsetof(struct Cyc_Core_Opt,v),(struct _tagged_arr){_tmp1F0,_tmp1F0,_tmp1F0 + 2},(
void*)& Cyc__genrep_313};static struct _tuple5*Cyc__genarr_319[1]={& Cyc__gentuple_318};
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep={
3,(struct _tagged_arr*)& Cyc__genname_320,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_319),(void*)((struct _tuple5**)Cyc__genarr_319),(void*)((
struct _tuple5**)Cyc__genarr_319 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_312={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep)};
static char _tmp1F3[11]="Tuniondecl";static struct _tagged_arr Cyc__genname_332=(
struct _tagged_arr){_tmp1F3,_tmp1F3,_tmp1F3 + 11};static char _tmp1F4[3]="sc";static
struct _tuple5 Cyc__gentuple_326={offsetof(struct Cyc_Absyn_Tuniondecl,sc),(struct
_tagged_arr){_tmp1F4,_tmp1F4,_tmp1F4 + 3},(void*)& Cyc_Absyn_scope_t_rep};static
char _tmp1F5[5]="name";static struct _tuple5 Cyc__gentuple_327={offsetof(struct Cyc_Absyn_Tuniondecl,name),(
struct _tagged_arr){_tmp1F5,_tmp1F5,_tmp1F5 + 5},(void*)& Cyc__genrep_11};static
char _tmp1F6[4]="tvs";static struct _tuple5 Cyc__gentuple_328={offsetof(struct Cyc_Absyn_Tuniondecl,tvs),(
struct _tagged_arr){_tmp1F6,_tmp1F6,_tmp1F6 + 4},(void*)& Cyc__genrep_321};static
char _tmp1F7[7]="fields";static struct _tuple5 Cyc__gentuple_329={offsetof(struct Cyc_Absyn_Tuniondecl,fields),(
struct _tagged_arr){_tmp1F7,_tmp1F7,_tmp1F7 + 7},(void*)& Cyc__genrep_312};static
char _tmp1F8[11]="is_xtunion";static struct _tuple5 Cyc__gentuple_330={offsetof(
struct Cyc_Absyn_Tuniondecl,is_xtunion),(struct _tagged_arr){_tmp1F8,_tmp1F8,
_tmp1F8 + 11},(void*)((void*)& Cyc__genrep_10)};static struct _tuple5*Cyc__genarr_331[
5]={& Cyc__gentuple_326,& Cyc__gentuple_327,& Cyc__gentuple_328,& Cyc__gentuple_329,&
Cyc__gentuple_330};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tuniondecl_rep={
3,(struct _tagged_arr*)& Cyc__genname_332,sizeof(struct Cyc_Absyn_Tuniondecl),{(
void*)((struct _tuple5**)Cyc__genarr_331),(void*)((struct _tuple5**)Cyc__genarr_331),(
void*)((struct _tuple5**)Cyc__genarr_331 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_311={1,1,(void*)((void*)& Cyc_struct_Absyn_Tuniondecl_rep)};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1065={1,1,(void*)((void*)& Cyc__genrep_311)};
struct _tuple19{unsigned int f1;struct Cyc_Absyn_Tuniondecl**f2;};static struct
_tuple6 Cyc__gentuple_1066={offsetof(struct _tuple19,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1067={offsetof(struct _tuple19,f2),(void*)& Cyc__genrep_1065};
static struct _tuple6*Cyc__genarr_1068[2]={& Cyc__gentuple_1066,& Cyc__gentuple_1067};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1064={4,sizeof(struct _tuple19),{(
void*)((struct _tuple6**)Cyc__genarr_1068),(void*)((struct _tuple6**)Cyc__genarr_1068),(
void*)((struct _tuple6**)Cyc__genarr_1068 + 2)}};static struct _tuple7*Cyc__genarr_1063[
0]={};static char _tmp1FD[14]="UnknownTunion";static struct _tuple5 Cyc__gentuple_1077={
0,(struct _tagged_arr){_tmp1FD,_tmp1FD,_tmp1FD + 14},(void*)& Cyc__genrep_1069};
static char _tmp1FE[12]="KnownTunion";static struct _tuple5 Cyc__gentuple_1078={1,(
struct _tagged_arr){_tmp1FE,_tmp1FE,_tmp1FE + 12},(void*)& Cyc__genrep_1064};static
struct _tuple5*Cyc__genarr_1079[2]={& Cyc__gentuple_1077,& Cyc__gentuple_1078};
static char _tmp200[12]="TunionInfoU";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep={
5,(struct _tagged_arr){_tmp200,_tmp200,_tmp200 + 12},{(void*)((struct _tuple7**)Cyc__genarr_1063),(
void*)((struct _tuple7**)Cyc__genarr_1063),(void*)((struct _tuple7**)Cyc__genarr_1063
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1079),(void*)((struct _tuple5**)Cyc__genarr_1079),(
void*)((struct _tuple5**)Cyc__genarr_1079 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_121;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep;
static char _tmp201[5]="List";static struct _tagged_arr Cyc__genname_125=(struct
_tagged_arr){_tmp201,_tmp201,_tmp201 + 5};static char _tmp202[3]="hd";static struct
_tuple5 Cyc__gentuple_122={offsetof(struct Cyc_List_List,hd),(struct _tagged_arr){
_tmp202,_tmp202,_tmp202 + 3},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char
_tmp203[3]="tl";static struct _tuple5 Cyc__gentuple_123={offsetof(struct Cyc_List_List,tl),(
struct _tagged_arr){_tmp203,_tmp203,_tmp203 + 3},(void*)& Cyc__genrep_121};static
struct _tuple5*Cyc__genarr_124[2]={& Cyc__gentuple_122,& Cyc__gentuple_123};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep={3,(struct
_tagged_arr*)& Cyc__genname_125,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_124),(void*)((struct _tuple5**)Cyc__genarr_124),(void*)((
struct _tuple5**)Cyc__genarr_124 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_121={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_type_t46H2_rep)};static char
_tmp206[11]="TunionInfo";static struct _tagged_arr Cyc__genname_1084=(struct
_tagged_arr){_tmp206,_tmp206,_tmp206 + 11};static char _tmp207[12]="tunion_info";
static struct _tuple5 Cyc__gentuple_1080={offsetof(struct Cyc_Absyn_TunionInfo,tunion_info),(
struct _tagged_arr){_tmp207,_tmp207,_tmp207 + 12},(void*)& Cyc_tunion_Absyn_TunionInfoU_rep};
static char _tmp208[6]="targs";static struct _tuple5 Cyc__gentuple_1081={offsetof(
struct Cyc_Absyn_TunionInfo,targs),(struct _tagged_arr){_tmp208,_tmp208,_tmp208 + 6},(
void*)& Cyc__genrep_121};static char _tmp209[4]="rgn";static struct _tuple5 Cyc__gentuple_1082={
offsetof(struct Cyc_Absyn_TunionInfo,rgn),(struct _tagged_arr){_tmp209,_tmp209,
_tmp209 + 4},(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple5*Cyc__genarr_1083[
3]={& Cyc__gentuple_1080,& Cyc__gentuple_1081,& Cyc__gentuple_1082};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_info_t_rep={3,(struct _tagged_arr*)& Cyc__genname_1084,sizeof(
struct Cyc_Absyn_TunionInfo),{(void*)((struct _tuple5**)Cyc__genarr_1083),(void*)((
struct _tuple5**)Cyc__genarr_1083),(void*)((struct _tuple5**)Cyc__genarr_1083 + 3)}};
struct _tuple20{unsigned int f1;struct Cyc_Absyn_TunionInfo f2;};static struct _tuple6
Cyc__gentuple_1085={offsetof(struct _tuple20,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_1086={offsetof(struct _tuple20,f2),(void*)& Cyc_Absyn_tunion_info_t_rep};
static struct _tuple6*Cyc__genarr_1087[2]={& Cyc__gentuple_1085,& Cyc__gentuple_1086};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1062={4,sizeof(struct _tuple20),{(
void*)((struct _tuple6**)Cyc__genarr_1087),(void*)((struct _tuple6**)Cyc__genarr_1087),(
void*)((struct _tuple6**)Cyc__genarr_1087 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1036;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_field_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1043;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep;static char _tmp20C[23]="UnknownTunionFieldInfo";
static struct _tagged_arr Cyc__genname_1048=(struct _tagged_arr){_tmp20C,_tmp20C,
_tmp20C + 23};static char _tmp20D[12]="tunion_name";static struct _tuple5 Cyc__gentuple_1044={
offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,tunion_name),(struct _tagged_arr){
_tmp20D,_tmp20D,_tmp20D + 12},(void*)& Cyc__genrep_11};static char _tmp20E[11]="field_name";
static struct _tuple5 Cyc__gentuple_1045={offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,field_name),(
struct _tagged_arr){_tmp20E,_tmp20E,_tmp20E + 11},(void*)& Cyc__genrep_11};static
char _tmp20F[11]="is_xtunion";static struct _tuple5 Cyc__gentuple_1046={offsetof(
struct Cyc_Absyn_UnknownTunionFieldInfo,is_xtunion),(struct _tagged_arr){_tmp20F,
_tmp20F,_tmp20F + 11},(void*)((void*)& Cyc__genrep_10)};static struct _tuple5*Cyc__genarr_1047[
3]={& Cyc__gentuple_1044,& Cyc__gentuple_1045,& Cyc__gentuple_1046};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep={3,(struct _tagged_arr*)& Cyc__genname_1048,
sizeof(struct Cyc_Absyn_UnknownTunionFieldInfo),{(void*)((struct _tuple5**)Cyc__genarr_1047),(
void*)((struct _tuple5**)Cyc__genarr_1047),(void*)((struct _tuple5**)Cyc__genarr_1047
+ 3)}};struct _tuple21{unsigned int f1;struct Cyc_Absyn_UnknownTunionFieldInfo f2;};
static struct _tuple6 Cyc__gentuple_1049={offsetof(struct _tuple21,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1050={offsetof(struct _tuple21,f2),(void*)& Cyc_struct_Absyn_UnknownTunionFieldInfo_rep};
static struct _tuple6*Cyc__genarr_1051[2]={& Cyc__gentuple_1049,& Cyc__gentuple_1050};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1043={4,sizeof(struct _tuple21),{(
void*)((struct _tuple6**)Cyc__genarr_1051),(void*)((struct _tuple6**)Cyc__genarr_1051),(
void*)((struct _tuple6**)Cyc__genarr_1051 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1038;struct _tuple22{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;
struct Cyc_Absyn_Tunionfield*f3;};static struct _tuple6 Cyc__gentuple_1039={
offsetof(struct _tuple22,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1040={
offsetof(struct _tuple22,f2),(void*)((void*)& Cyc__genrep_311)};static struct
_tuple6 Cyc__gentuple_1041={offsetof(struct _tuple22,f3),(void*)& Cyc__genrep_294};
static struct _tuple6*Cyc__genarr_1042[3]={& Cyc__gentuple_1039,& Cyc__gentuple_1040,&
Cyc__gentuple_1041};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1038={4,
sizeof(struct _tuple22),{(void*)((struct _tuple6**)Cyc__genarr_1042),(void*)((
struct _tuple6**)Cyc__genarr_1042),(void*)((struct _tuple6**)Cyc__genarr_1042 + 3)}};
static struct _tuple7*Cyc__genarr_1037[0]={};static char _tmp213[19]="UnknownTunionfield";
static struct _tuple5 Cyc__gentuple_1052={0,(struct _tagged_arr){_tmp213,_tmp213,
_tmp213 + 19},(void*)& Cyc__genrep_1043};static char _tmp214[17]="KnownTunionfield";
static struct _tuple5 Cyc__gentuple_1053={1,(struct _tagged_arr){_tmp214,_tmp214,
_tmp214 + 17},(void*)& Cyc__genrep_1038};static struct _tuple5*Cyc__genarr_1054[2]={&
Cyc__gentuple_1052,& Cyc__gentuple_1053};static char _tmp216[17]="TunionFieldInfoU";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep={5,(struct
_tagged_arr){_tmp216,_tmp216,_tmp216 + 17},{(void*)((struct _tuple7**)Cyc__genarr_1037),(
void*)((struct _tuple7**)Cyc__genarr_1037),(void*)((struct _tuple7**)Cyc__genarr_1037
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1054),(void*)((struct _tuple5**)Cyc__genarr_1054),(
void*)((struct _tuple5**)Cyc__genarr_1054 + 2)}};static char _tmp217[16]="TunionFieldInfo";
static struct _tagged_arr Cyc__genname_1058=(struct _tagged_arr){_tmp217,_tmp217,
_tmp217 + 16};static char _tmp218[11]="field_info";static struct _tuple5 Cyc__gentuple_1055={
offsetof(struct Cyc_Absyn_TunionFieldInfo,field_info),(struct _tagged_arr){_tmp218,
_tmp218,_tmp218 + 11},(void*)& Cyc_tunion_Absyn_TunionFieldInfoU_rep};static char
_tmp219[6]="targs";static struct _tuple5 Cyc__gentuple_1056={offsetof(struct Cyc_Absyn_TunionFieldInfo,targs),(
struct _tagged_arr){_tmp219,_tmp219,_tmp219 + 6},(void*)& Cyc__genrep_121};static
struct _tuple5*Cyc__genarr_1057[2]={& Cyc__gentuple_1055,& Cyc__gentuple_1056};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_field_info_t_rep={3,(struct
_tagged_arr*)& Cyc__genname_1058,sizeof(struct Cyc_Absyn_TunionFieldInfo),{(void*)((
struct _tuple5**)Cyc__genarr_1057),(void*)((struct _tuple5**)Cyc__genarr_1057),(
void*)((struct _tuple5**)Cyc__genarr_1057 + 2)}};struct _tuple23{unsigned int f1;
struct Cyc_Absyn_TunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1059={
offsetof(struct _tuple23,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1060={
offsetof(struct _tuple23,f2),(void*)& Cyc_Absyn_tunion_field_info_t_rep};static
struct _tuple6*Cyc__genarr_1061[2]={& Cyc__gentuple_1059,& Cyc__gentuple_1060};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1036={4,sizeof(struct _tuple23),{(
void*)((struct _tuple6**)Cyc__genarr_1061),(void*)((struct _tuple6**)Cyc__genarr_1061),(
void*)((struct _tuple6**)Cyc__genarr_1061 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1010;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep;
extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_976;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0bool2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_85;struct _tuple24{unsigned int f1;
int f2;};static struct _tuple6 Cyc__gentuple_86={offsetof(struct _tuple24,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_87={offsetof(struct _tuple24,f2),(
void*)((void*)& Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_88[2]={& Cyc__gentuple_86,&
Cyc__gentuple_87};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_85={4,sizeof(
struct _tuple24),{(void*)((struct _tuple6**)Cyc__genarr_88),(void*)((struct _tuple6**)
Cyc__genarr_88),(void*)((struct _tuple6**)Cyc__genarr_88 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_979;struct _tuple25{unsigned int f1;struct Cyc_Absyn_Conref*f2;};static
struct _tuple6 Cyc__gentuple_980={offsetof(struct _tuple25,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_981={offsetof(struct _tuple25,f2),(void*)& Cyc__genrep_976};
static struct _tuple6*Cyc__genarr_982[2]={& Cyc__gentuple_980,& Cyc__gentuple_981};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_979={4,sizeof(struct _tuple25),{(
void*)((struct _tuple6**)Cyc__genarr_982),(void*)((struct _tuple6**)Cyc__genarr_982),(
void*)((struct _tuple6**)Cyc__genarr_982 + 2)}};static char _tmp21E[10]="No_constr";
static struct _tuple7 Cyc__gentuple_977={0,(struct _tagged_arr){_tmp21E,_tmp21E,
_tmp21E + 10}};static struct _tuple7*Cyc__genarr_978[1]={& Cyc__gentuple_977};static
char _tmp21F[10]="Eq_constr";static struct _tuple5 Cyc__gentuple_983={0,(struct
_tagged_arr){_tmp21F,_tmp21F,_tmp21F + 10},(void*)& Cyc__genrep_85};static char
_tmp220[15]="Forward_constr";static struct _tuple5 Cyc__gentuple_984={1,(struct
_tagged_arr){_tmp220,_tmp220,_tmp220 + 15},(void*)& Cyc__genrep_979};static struct
_tuple5*Cyc__genarr_985[2]={& Cyc__gentuple_983,& Cyc__gentuple_984};static char
_tmp222[11]="Constraint";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep={
5,(struct _tagged_arr){_tmp222,_tmp222,_tmp222 + 11},{(void*)((struct _tuple7**)Cyc__genarr_978),(
void*)((struct _tuple7**)Cyc__genarr_978),(void*)((struct _tuple7**)Cyc__genarr_978
+ 1)},{(void*)((struct _tuple5**)Cyc__genarr_985),(void*)((struct _tuple5**)Cyc__genarr_985),(
void*)((struct _tuple5**)Cyc__genarr_985 + 2)}};static char _tmp223[7]="Conref";
static struct _tagged_arr Cyc__genname_988=(struct _tagged_arr){_tmp223,_tmp223,
_tmp223 + 7};static char _tmp224[2]="v";static struct _tuple5 Cyc__gentuple_986={
offsetof(struct Cyc_Absyn_Conref,v),(struct _tagged_arr){_tmp224,_tmp224,_tmp224 + 
2},(void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};static struct _tuple5*Cyc__genarr_987[
1]={& Cyc__gentuple_986};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0bool2_rep={
3,(struct _tagged_arr*)& Cyc__genname_988,sizeof(struct Cyc_Absyn_Conref),{(void*)((
struct _tuple5**)Cyc__genarr_987),(void*)((struct _tuple5**)Cyc__genarr_987),(void*)((
struct _tuple5**)Cyc__genarr_987 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_976={
1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0bool2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1017;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep;
static char _tmp227[7]="Conref";static struct _tagged_arr Cyc__genname_1020=(struct
_tagged_arr){_tmp227,_tmp227,_tmp227 + 7};static char _tmp228[2]="v";static struct
_tuple5 Cyc__gentuple_1018={offsetof(struct Cyc_Absyn_Conref,v),(struct _tagged_arr){
_tmp228,_tmp228,_tmp228 + 2},(void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};
static struct _tuple5*Cyc__genarr_1019[1]={& Cyc__gentuple_1018};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_1020,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_1019),(void*)((
struct _tuple5**)Cyc__genarr_1019),(void*)((struct _tuple5**)Cyc__genarr_1019 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1017={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1011;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_PtrLoc_rep;static char _tmp22B[7]="PtrLoc";static struct
_tagged_arr Cyc__genname_1016=(struct _tagged_arr){_tmp22B,_tmp22B,_tmp22B + 7};
static char _tmp22C[8]="ptr_loc";static struct _tuple5 Cyc__gentuple_1012={offsetof(
struct Cyc_Absyn_PtrLoc,ptr_loc),(struct _tagged_arr){_tmp22C,_tmp22C,_tmp22C + 8},(
void*)& Cyc__genrep_2};static char _tmp22D[8]="rgn_loc";static struct _tuple5 Cyc__gentuple_1013={
offsetof(struct Cyc_Absyn_PtrLoc,rgn_loc),(struct _tagged_arr){_tmp22D,_tmp22D,
_tmp22D + 8},(void*)& Cyc__genrep_2};static char _tmp22E[7]="zt_loc";static struct
_tuple5 Cyc__gentuple_1014={offsetof(struct Cyc_Absyn_PtrLoc,zt_loc),(struct
_tagged_arr){_tmp22E,_tmp22E,_tmp22E + 7},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_1015[3]={& Cyc__gentuple_1012,& Cyc__gentuple_1013,& Cyc__gentuple_1014};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_PtrLoc_rep={3,(struct _tagged_arr*)&
Cyc__genname_1016,sizeof(struct Cyc_Absyn_PtrLoc),{(void*)((struct _tuple5**)Cyc__genarr_1015),(
void*)((struct _tuple5**)Cyc__genarr_1015),(void*)((struct _tuple5**)Cyc__genarr_1015
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1011={1,1,(void*)((void*)&
Cyc_struct_Absyn_PtrLoc_rep)};static char _tmp231[8]="PtrAtts";static struct
_tagged_arr Cyc__genname_1027=(struct _tagged_arr){_tmp231,_tmp231,_tmp231 + 8};
static char _tmp232[4]="rgn";static struct _tuple5 Cyc__gentuple_1021={offsetof(
struct Cyc_Absyn_PtrAtts,rgn),(struct _tagged_arr){_tmp232,_tmp232,_tmp232 + 4},(
void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp233[9]="nullable";static
struct _tuple5 Cyc__gentuple_1022={offsetof(struct Cyc_Absyn_PtrAtts,nullable),(
struct _tagged_arr){_tmp233,_tmp233,_tmp233 + 9},(void*)& Cyc__genrep_976};static
char _tmp234[7]="bounds";static struct _tuple5 Cyc__gentuple_1023={offsetof(struct
Cyc_Absyn_PtrAtts,bounds),(struct _tagged_arr){_tmp234,_tmp234,_tmp234 + 7},(void*)&
Cyc__genrep_1017};static char _tmp235[10]="zero_term";static struct _tuple5 Cyc__gentuple_1024={
offsetof(struct Cyc_Absyn_PtrAtts,zero_term),(struct _tagged_arr){_tmp235,_tmp235,
_tmp235 + 10},(void*)& Cyc__genrep_976};static char _tmp236[7]="ptrloc";static struct
_tuple5 Cyc__gentuple_1025={offsetof(struct Cyc_Absyn_PtrAtts,ptrloc),(struct
_tagged_arr){_tmp236,_tmp236,_tmp236 + 7},(void*)& Cyc__genrep_1011};static struct
_tuple5*Cyc__genarr_1026[5]={& Cyc__gentuple_1021,& Cyc__gentuple_1022,& Cyc__gentuple_1023,&
Cyc__gentuple_1024,& Cyc__gentuple_1025};struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_1027,sizeof(struct Cyc_Absyn_PtrAtts),{(void*)((
struct _tuple5**)Cyc__genarr_1026),(void*)((struct _tuple5**)Cyc__genarr_1026),(
void*)((struct _tuple5**)Cyc__genarr_1026 + 5)}};static char _tmp238[8]="PtrInfo";
static struct _tagged_arr Cyc__genname_1032=(struct _tagged_arr){_tmp238,_tmp238,
_tmp238 + 8};static char _tmp239[8]="elt_typ";static struct _tuple5 Cyc__gentuple_1028={
offsetof(struct Cyc_Absyn_PtrInfo,elt_typ),(struct _tagged_arr){_tmp239,_tmp239,
_tmp239 + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp23A[7]="elt_tq";
static struct _tuple5 Cyc__gentuple_1029={offsetof(struct Cyc_Absyn_PtrInfo,elt_tq),(
struct _tagged_arr){_tmp23A,_tmp23A,_tmp23A + 7},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp23B[9]="ptr_atts";static struct _tuple5 Cyc__gentuple_1030={offsetof(
struct Cyc_Absyn_PtrInfo,ptr_atts),(struct _tagged_arr){_tmp23B,_tmp23B,_tmp23B + 9},(
void*)& Cyc_Absyn_ptr_atts_t_rep};static struct _tuple5*Cyc__genarr_1031[3]={& Cyc__gentuple_1028,&
Cyc__gentuple_1029,& Cyc__gentuple_1030};struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_1032,sizeof(struct Cyc_Absyn_PtrInfo),{(void*)((
struct _tuple5**)Cyc__genarr_1031),(void*)((struct _tuple5**)Cyc__genarr_1031),(
void*)((struct _tuple5**)Cyc__genarr_1031 + 3)}};struct _tuple26{unsigned int f1;
struct Cyc_Absyn_PtrInfo f2;};static struct _tuple6 Cyc__gentuple_1033={offsetof(
struct _tuple26,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1034={
offsetof(struct _tuple26,f2),(void*)& Cyc_Absyn_ptr_info_t_rep};static struct
_tuple6*Cyc__genarr_1035[2]={& Cyc__gentuple_1033,& Cyc__gentuple_1034};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1010={4,sizeof(struct _tuple26),{(void*)((
struct _tuple6**)Cyc__genarr_1035),(void*)((struct _tuple6**)Cyc__genarr_1035),(
void*)((struct _tuple6**)Cyc__genarr_1035 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_999;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep;static
char _tmp23E[7]="Signed";static struct _tuple7 Cyc__gentuple_284={0,(struct
_tagged_arr){_tmp23E,_tmp23E,_tmp23E + 7}};static char _tmp23F[9]="Unsigned";static
struct _tuple7 Cyc__gentuple_285={1,(struct _tagged_arr){_tmp23F,_tmp23F,_tmp23F + 9}};
static char _tmp240[5]="None";static struct _tuple7 Cyc__gentuple_286={2,(struct
_tagged_arr){_tmp240,_tmp240,_tmp240 + 5}};static struct _tuple7*Cyc__genarr_287[3]={&
Cyc__gentuple_284,& Cyc__gentuple_285,& Cyc__gentuple_286};static struct _tuple5*Cyc__genarr_288[
0]={};static char _tmp242[5]="Sign";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep={
5,(struct _tagged_arr){_tmp242,_tmp242,_tmp242 + 5},{(void*)((struct _tuple7**)Cyc__genarr_287),(
void*)((struct _tuple7**)Cyc__genarr_287),(void*)((struct _tuple7**)Cyc__genarr_287
+ 3)},{(void*)((struct _tuple5**)Cyc__genarr_288),(void*)((struct _tuple5**)Cyc__genarr_288),(
void*)((struct _tuple5**)Cyc__genarr_288 + 0)}};extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_size_of_t_rep;static char _tmp243[3]="B1";static struct _tuple7 Cyc__gentuple_1000={
0,(struct _tagged_arr){_tmp243,_tmp243,_tmp243 + 3}};static char _tmp244[3]="B2";
static struct _tuple7 Cyc__gentuple_1001={1,(struct _tagged_arr){_tmp244,_tmp244,
_tmp244 + 3}};static char _tmp245[3]="B4";static struct _tuple7 Cyc__gentuple_1002={2,(
struct _tagged_arr){_tmp245,_tmp245,_tmp245 + 3}};static char _tmp246[3]="B8";static
struct _tuple7 Cyc__gentuple_1003={3,(struct _tagged_arr){_tmp246,_tmp246,_tmp246 + 
3}};static struct _tuple7*Cyc__genarr_1004[4]={& Cyc__gentuple_1000,& Cyc__gentuple_1001,&
Cyc__gentuple_1002,& Cyc__gentuple_1003};static struct _tuple5*Cyc__genarr_1005[0]={};
static char _tmp248[8]="Size_of";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep={
5,(struct _tagged_arr){_tmp248,_tmp248,_tmp248 + 8},{(void*)((struct _tuple7**)Cyc__genarr_1004),(
void*)((struct _tuple7**)Cyc__genarr_1004),(void*)((struct _tuple7**)Cyc__genarr_1004
+ 4)},{(void*)((struct _tuple5**)Cyc__genarr_1005),(void*)((struct _tuple5**)Cyc__genarr_1005),(
void*)((struct _tuple5**)Cyc__genarr_1005 + 0)}};struct _tuple27{unsigned int f1;
void*f2;void*f3;};static struct _tuple6 Cyc__gentuple_1006={offsetof(struct _tuple27,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1007={offsetof(struct
_tuple27,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_1008={
offsetof(struct _tuple27,f3),(void*)& Cyc_Absyn_size_of_t_rep};static struct _tuple6*
Cyc__genarr_1009[3]={& Cyc__gentuple_1006,& Cyc__gentuple_1007,& Cyc__gentuple_1008};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_999={4,sizeof(struct _tuple27),{(
void*)((struct _tuple6**)Cyc__genarr_1009),(void*)((struct _tuple6**)Cyc__genarr_1009),(
void*)((struct _tuple6**)Cyc__genarr_1009 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_975;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_array_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_141;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Exp_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_851;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_cnst_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_866;struct
_tuple28{unsigned int f1;void*f2;char f3;};static struct _tuple6 Cyc__gentuple_867={
offsetof(struct _tuple28,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_868={
offsetof(struct _tuple28,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_869={
offsetof(struct _tuple28,f3),(void*)((void*)& Cyc__genrep_15)};static struct _tuple6*
Cyc__genarr_870[3]={& Cyc__gentuple_867,& Cyc__gentuple_868,& Cyc__gentuple_869};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_866={4,sizeof(struct _tuple28),{(
void*)((struct _tuple6**)Cyc__genarr_870),(void*)((struct _tuple6**)Cyc__genarr_870),(
void*)((struct _tuple6**)Cyc__genarr_870 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_860;static struct Cyc_Typerep_Int_struct Cyc__genrep_861={0,1,16};
struct _tuple29{unsigned int f1;void*f2;short f3;};static struct _tuple6 Cyc__gentuple_862={
offsetof(struct _tuple29,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_863={
offsetof(struct _tuple29,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_864={
offsetof(struct _tuple29,f3),(void*)& Cyc__genrep_861};static struct _tuple6*Cyc__genarr_865[
3]={& Cyc__gentuple_862,& Cyc__gentuple_863,& Cyc__gentuple_864};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_860={4,sizeof(struct _tuple29),{(void*)((struct _tuple6**)Cyc__genarr_865),(
void*)((struct _tuple6**)Cyc__genarr_865),(void*)((struct _tuple6**)Cyc__genarr_865
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_283;struct _tuple30{
unsigned int f1;void*f2;int f3;};static struct _tuple6 Cyc__gentuple_289={offsetof(
struct _tuple30,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_290={
offsetof(struct _tuple30,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_291={
offsetof(struct _tuple30,f3),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6*
Cyc__genarr_292[3]={& Cyc__gentuple_289,& Cyc__gentuple_290,& Cyc__gentuple_291};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_283={4,sizeof(struct _tuple30),{(
void*)((struct _tuple6**)Cyc__genarr_292),(void*)((struct _tuple6**)Cyc__genarr_292),(
void*)((struct _tuple6**)Cyc__genarr_292 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_854;static struct Cyc_Typerep_Int_struct Cyc__genrep_855={0,1,64};
struct _tuple31{unsigned int f1;void*f2;long long f3;};static struct _tuple6 Cyc__gentuple_856={
offsetof(struct _tuple31,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_857={
offsetof(struct _tuple31,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_858={
offsetof(struct _tuple31,f3),(void*)& Cyc__genrep_855};static struct _tuple6*Cyc__genarr_859[
3]={& Cyc__gentuple_856,& Cyc__gentuple_857,& Cyc__gentuple_858};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_854={4,sizeof(struct _tuple31),{(void*)((struct _tuple6**)Cyc__genarr_859),(
void*)((struct _tuple6**)Cyc__genarr_859),(void*)((struct _tuple6**)Cyc__genarr_859
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_81;static struct _tuple6 Cyc__gentuple_82={
offsetof(struct _tuple7,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_83={
offsetof(struct _tuple7,f2),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*
Cyc__genarr_84[2]={& Cyc__gentuple_82,& Cyc__gentuple_83};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_81={4,sizeof(struct _tuple7),{(void*)((struct _tuple6**)Cyc__genarr_84),(
void*)((struct _tuple6**)Cyc__genarr_84),(void*)((struct _tuple6**)Cyc__genarr_84 + 
2)}};static char _tmp251[7]="Null_c";static struct _tuple7 Cyc__gentuple_852={0,(
struct _tagged_arr){_tmp251,_tmp251,_tmp251 + 7}};static struct _tuple7*Cyc__genarr_853[
1]={& Cyc__gentuple_852};static char _tmp252[7]="Char_c";static struct _tuple5 Cyc__gentuple_871={
0,(struct _tagged_arr){_tmp252,_tmp252,_tmp252 + 7},(void*)& Cyc__genrep_866};
static char _tmp253[8]="Short_c";static struct _tuple5 Cyc__gentuple_872={1,(struct
_tagged_arr){_tmp253,_tmp253,_tmp253 + 8},(void*)& Cyc__genrep_860};static char
_tmp254[6]="Int_c";static struct _tuple5 Cyc__gentuple_873={2,(struct _tagged_arr){
_tmp254,_tmp254,_tmp254 + 6},(void*)& Cyc__genrep_283};static char _tmp255[11]="LongLong_c";
static struct _tuple5 Cyc__gentuple_874={3,(struct _tagged_arr){_tmp255,_tmp255,
_tmp255 + 11},(void*)& Cyc__genrep_854};static char _tmp256[8]="Float_c";static
struct _tuple5 Cyc__gentuple_875={4,(struct _tagged_arr){_tmp256,_tmp256,_tmp256 + 8},(
void*)& Cyc__genrep_81};static char _tmp257[9]="String_c";static struct _tuple5 Cyc__gentuple_876={
5,(struct _tagged_arr){_tmp257,_tmp257,_tmp257 + 9},(void*)& Cyc__genrep_81};static
struct _tuple5*Cyc__genarr_877[6]={& Cyc__gentuple_871,& Cyc__gentuple_872,& Cyc__gentuple_873,&
Cyc__gentuple_874,& Cyc__gentuple_875,& Cyc__gentuple_876};static char _tmp259[5]="Cnst";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_cnst_t_rep={5,(struct _tagged_arr){
_tmp259,_tmp259,_tmp259 + 5},{(void*)((struct _tuple7**)Cyc__genarr_853),(void*)((
struct _tuple7**)Cyc__genarr_853),(void*)((struct _tuple7**)Cyc__genarr_853 + 1)},{(
void*)((struct _tuple5**)Cyc__genarr_877),(void*)((struct _tuple5**)Cyc__genarr_877),(
void*)((struct _tuple5**)Cyc__genarr_877 + 6)}};static struct _tuple6 Cyc__gentuple_878={
offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_879={
offsetof(struct _tuple6,f2),(void*)& Cyc_Absyn_cnst_t_rep};static struct _tuple6*Cyc__genarr_880[
2]={& Cyc__gentuple_878,& Cyc__gentuple_879};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_851={4,sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_880),(
void*)((struct _tuple6**)Cyc__genarr_880),(void*)((struct _tuple6**)Cyc__genarr_880
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_805;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_binding_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_808;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_809;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Fndecl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_811;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_812;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_813;struct _tuple32{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*
f3;};static struct _tuple6 Cyc__gentuple_814={offsetof(struct _tuple32,f1),(void*)&
Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_815={offsetof(struct _tuple32,f2),(
void*)& Cyc_Absyn_tqual_t_rep};static struct _tuple6 Cyc__gentuple_816={offsetof(
struct _tuple32,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6*
Cyc__genarr_817[3]={& Cyc__gentuple_814,& Cyc__gentuple_815,& Cyc__gentuple_816};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_813={4,sizeof(struct _tuple32),{(
void*)((struct _tuple6**)Cyc__genarr_817),(void*)((struct _tuple6**)Cyc__genarr_817),(
void*)((struct _tuple6**)Cyc__genarr_817 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_812={1,1,(void*)((void*)& Cyc__genrep_813)};static char _tmp25D[5]="List";
static struct _tagged_arr Cyc__genname_821=(struct _tagged_arr){_tmp25D,_tmp25D,
_tmp25D + 5};static char _tmp25E[3]="hd";static struct _tuple5 Cyc__gentuple_818={
offsetof(struct Cyc_List_List,hd),(struct _tagged_arr){_tmp25E,_tmp25E,_tmp25E + 3},(
void*)& Cyc__genrep_812};static char _tmp25F[3]="tl";static struct _tuple5 Cyc__gentuple_819={
offsetof(struct Cyc_List_List,tl),(struct _tagged_arr){_tmp25F,_tmp25F,_tmp25F + 3},(
void*)& Cyc__genrep_811};static struct _tuple5*Cyc__genarr_820[2]={& Cyc__gentuple_818,&
Cyc__gentuple_819};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_821,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_820),(void*)((struct _tuple5**)Cyc__genarr_820),(void*)((
struct _tuple5**)Cyc__genarr_820 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_811={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_810;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_660;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep;static
char _tmp262[4]="Opt";static struct _tagged_arr Cyc__genname_663=(struct _tagged_arr){
_tmp262,_tmp262,_tmp262 + 4};static char _tmp263[2]="v";static struct _tuple5 Cyc__gentuple_661={
offsetof(struct Cyc_Core_Opt,v),(struct _tagged_arr){_tmp263,_tmp263,_tmp263 + 2},(
void*)& Cyc__genrep_13};static struct _tuple5*Cyc__genarr_662[1]={& Cyc__gentuple_661};
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep={3,(struct
_tagged_arr*)& Cyc__genname_663,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_662),(void*)((struct _tuple5**)Cyc__genarr_662),(void*)((
struct _tuple5**)Cyc__genarr_662 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_660={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_var_t2_rep)};static char _tmp266[11]="VarargInfo";
static struct _tagged_arr Cyc__genname_735=(struct _tagged_arr){_tmp266,_tmp266,
_tmp266 + 11};static char _tmp267[5]="name";static struct _tuple5 Cyc__gentuple_730={
offsetof(struct Cyc_Absyn_VarargInfo,name),(struct _tagged_arr){_tmp267,_tmp267,
_tmp267 + 5},(void*)& Cyc__genrep_660};static char _tmp268[3]="tq";static struct
_tuple5 Cyc__gentuple_731={offsetof(struct Cyc_Absyn_VarargInfo,tq),(struct
_tagged_arr){_tmp268,_tmp268,_tmp268 + 3},(void*)& Cyc_Absyn_tqual_t_rep};static
char _tmp269[5]="type";static struct _tuple5 Cyc__gentuple_732={offsetof(struct Cyc_Absyn_VarargInfo,type),(
struct _tagged_arr){_tmp269,_tmp269,_tmp269 + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp26A[7]="inject";static struct _tuple5 Cyc__gentuple_733={offsetof(
struct Cyc_Absyn_VarargInfo,inject),(struct _tagged_arr){_tmp26A,_tmp26A,_tmp26A + 
7},(void*)((void*)& Cyc__genrep_10)};static struct _tuple5*Cyc__genarr_734[4]={& Cyc__gentuple_730,&
Cyc__gentuple_731,& Cyc__gentuple_732,& Cyc__gentuple_733};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep={3,(struct _tagged_arr*)& Cyc__genname_735,sizeof(
struct Cyc_Absyn_VarargInfo),{(void*)((struct _tuple5**)Cyc__genarr_734),(void*)((
struct _tuple5**)Cyc__genarr_734),(void*)((struct _tuple5**)Cyc__genarr_734 + 4)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_810={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_381;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_382;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_383;static struct
_tuple6 Cyc__gentuple_384={offsetof(struct _tuple9,f1),(void*)((void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple6 Cyc__gentuple_385={offsetof(struct _tuple9,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_386[2]={& Cyc__gentuple_384,&
Cyc__gentuple_385};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_383={4,
sizeof(struct _tuple9),{(void*)((struct _tuple6**)Cyc__genarr_386),(void*)((struct
_tuple6**)Cyc__genarr_386),(void*)((struct _tuple6**)Cyc__genarr_386 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_382={1,1,(void*)((void*)& Cyc__genrep_383)};
static char _tmp26F[5]="List";static struct _tagged_arr Cyc__genname_390=(struct
_tagged_arr){_tmp26F,_tmp26F,_tmp26F + 5};static char _tmp270[3]="hd";static struct
_tuple5 Cyc__gentuple_387={offsetof(struct Cyc_List_List,hd),(struct _tagged_arr){
_tmp270,_tmp270,_tmp270 + 3},(void*)& Cyc__genrep_382};static char _tmp271[3]="tl";
static struct _tuple5 Cyc__gentuple_388={offsetof(struct Cyc_List_List,tl),(struct
_tagged_arr){_tmp271,_tmp271,_tmp271 + 3},(void*)& Cyc__genrep_381};static struct
_tuple5*Cyc__genarr_389[2]={& Cyc__gentuple_387,& Cyc__gentuple_388};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep={3,(struct
_tagged_arr*)& Cyc__genname_390,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_389),(void*)((struct _tuple5**)Cyc__genarr_389),(void*)((
struct _tuple5**)Cyc__genarr_389 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_381={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_145;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_153;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_154;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_154={1,1,(void*)((
void*)& Cyc_struct_Absyn_Exp_rep)};struct _tuple33{unsigned int f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple6 Cyc__gentuple_155={offsetof(struct _tuple33,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_156={offsetof(struct _tuple33,f2),(
void*)& Cyc__genrep_154};static struct _tuple6*Cyc__genarr_157[2]={& Cyc__gentuple_155,&
Cyc__gentuple_156};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_153={4,
sizeof(struct _tuple33),{(void*)((struct _tuple6**)Cyc__genarr_157),(void*)((
struct _tuple6**)Cyc__genarr_157),(void*)((struct _tuple6**)Cyc__genarr_157 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_550;struct _tuple34{unsigned int
f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_551={
offsetof(struct _tuple34,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_552={
offsetof(struct _tuple34,f2),(void*)& Cyc__genrep_145};static struct _tuple6 Cyc__gentuple_553={
offsetof(struct _tuple34,f3),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_554[
3]={& Cyc__gentuple_551,& Cyc__gentuple_552,& Cyc__gentuple_553};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_550={4,sizeof(struct _tuple34),{(void*)((struct _tuple6**)Cyc__genarr_554),(
void*)((struct _tuple6**)Cyc__genarr_554),(void*)((struct _tuple6**)Cyc__genarr_554
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_546;static struct _tuple6
Cyc__gentuple_547={offsetof(struct _tuple33,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_548={offsetof(struct _tuple33,f2),(void*)& Cyc__genrep_141};
static struct _tuple6*Cyc__genarr_549[2]={& Cyc__gentuple_547,& Cyc__gentuple_548};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_546={4,sizeof(struct _tuple33),{(
void*)((struct _tuple6**)Cyc__genarr_549),(void*)((struct _tuple6**)Cyc__genarr_549),(
void*)((struct _tuple6**)Cyc__genarr_549 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_540;struct _tuple35{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Stmt*
f3;struct Cyc_Absyn_Stmt*f4;};static struct _tuple6 Cyc__gentuple_541={offsetof(
struct _tuple35,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_542={
offsetof(struct _tuple35,f2),(void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_543={
offsetof(struct _tuple35,f3),(void*)& Cyc__genrep_145};static struct _tuple6 Cyc__gentuple_544={
offsetof(struct _tuple35,f4),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_545[
4]={& Cyc__gentuple_541,& Cyc__gentuple_542,& Cyc__gentuple_543,& Cyc__gentuple_544};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_540={4,sizeof(struct _tuple35),{(
void*)((struct _tuple6**)Cyc__genarr_545),(void*)((struct _tuple6**)Cyc__genarr_545),(
void*)((struct _tuple6**)Cyc__genarr_545 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_535;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_484;static struct
_tuple6 Cyc__gentuple_485={offsetof(struct _tuple2,f1),(void*)& Cyc__genrep_154};
static struct _tuple6 Cyc__gentuple_486={offsetof(struct _tuple2,f2),(void*)& Cyc__genrep_145};
static struct _tuple6*Cyc__genarr_487[2]={& Cyc__gentuple_485,& Cyc__gentuple_486};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_484={4,sizeof(struct _tuple2),{(
void*)((struct _tuple6**)Cyc__genarr_487),(void*)((struct _tuple6**)Cyc__genarr_487),(
void*)((struct _tuple6**)Cyc__genarr_487 + 2)}};struct _tuple36{unsigned int f1;
struct _tuple2 f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_536={
offsetof(struct _tuple36,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_537={
offsetof(struct _tuple36,f2),(void*)& Cyc__genrep_484};static struct _tuple6 Cyc__gentuple_538={
offsetof(struct _tuple36,f3),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_539[
3]={& Cyc__gentuple_536,& Cyc__gentuple_537,& Cyc__gentuple_538};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_535={4,sizeof(struct _tuple36),{(void*)((struct _tuple6**)Cyc__genarr_539),(
void*)((struct _tuple6**)Cyc__genarr_539),(void*)((struct _tuple6**)Cyc__genarr_539
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_531;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_526;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_526={1,1,(void*)((
void*)& Cyc_struct_Absyn_Stmt_rep)};struct _tuple37{unsigned int f1;struct Cyc_Absyn_Stmt*
f2;};static struct _tuple6 Cyc__gentuple_532={offsetof(struct _tuple37,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_533={offsetof(struct _tuple37,f2),(
void*)& Cyc__genrep_526};static struct _tuple6*Cyc__genarr_534[2]={& Cyc__gentuple_532,&
Cyc__gentuple_533};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_531={4,
sizeof(struct _tuple37),{(void*)((struct _tuple6**)Cyc__genarr_534),(void*)((
struct _tuple6**)Cyc__genarr_534),(void*)((struct _tuple6**)Cyc__genarr_534 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_525;struct _tuple38{unsigned int
f1;struct _tagged_arr*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_527={
offsetof(struct _tuple38,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_528={
offsetof(struct _tuple38,f2),(void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_529={
offsetof(struct _tuple38,f3),(void*)& Cyc__genrep_526};static struct _tuple6*Cyc__genarr_530[
3]={& Cyc__gentuple_527,& Cyc__gentuple_528,& Cyc__gentuple_529};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_525={4,sizeof(struct _tuple38),{(void*)((struct _tuple6**)Cyc__genarr_530),(
void*)((struct _tuple6**)Cyc__genarr_530),(void*)((struct _tuple6**)Cyc__genarr_530
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_518;struct _tuple39{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct _tuple2 f3;struct _tuple2 f4;struct Cyc_Absyn_Stmt*
f5;};static struct _tuple6 Cyc__gentuple_519={offsetof(struct _tuple39,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_520={offsetof(struct _tuple39,f2),(
void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_521={offsetof(struct
_tuple39,f3),(void*)& Cyc__genrep_484};static struct _tuple6 Cyc__gentuple_522={
offsetof(struct _tuple39,f4),(void*)& Cyc__genrep_484};static struct _tuple6 Cyc__gentuple_523={
offsetof(struct _tuple39,f5),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_524[
5]={& Cyc__gentuple_519,& Cyc__gentuple_520,& Cyc__gentuple_521,& Cyc__gentuple_522,&
Cyc__gentuple_523};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_518={4,
sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_524),(void*)((
struct _tuple6**)Cyc__genarr_524),(void*)((struct _tuple6**)Cyc__genarr_524 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_513;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_229;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_230;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Switch_clause_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_240;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_438;
struct _tuple40{unsigned int f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;
};static struct _tuple6 Cyc__gentuple_439={offsetof(struct _tuple40,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_440={offsetof(struct _tuple40,f2),(void*)& Cyc__genrep_185};
static struct _tuple6 Cyc__gentuple_441={offsetof(struct _tuple40,f3),(void*)& Cyc__genrep_159};
static struct _tuple6*Cyc__genarr_442[3]={& Cyc__gentuple_439,& Cyc__gentuple_440,&
Cyc__gentuple_441};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_438={4,
sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_442),(void*)((
struct _tuple6**)Cyc__genarr_442),(void*)((struct _tuple6**)Cyc__genarr_442 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_433;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_245;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep;
static char _tmp280[5]="List";static struct _tagged_arr Cyc__genname_249=(struct
_tagged_arr){_tmp280,_tmp280,_tmp280 + 5};static char _tmp281[3]="hd";static struct
_tuple5 Cyc__gentuple_246={offsetof(struct Cyc_List_List,hd),(struct _tagged_arr){
_tmp281,_tmp281,_tmp281 + 3},(void*)& Cyc__genrep_240};static char _tmp282[3]="tl";
static struct _tuple5 Cyc__gentuple_247={offsetof(struct Cyc_List_List,tl),(struct
_tagged_arr){_tmp282,_tmp282,_tmp282 + 3},(void*)& Cyc__genrep_245};static struct
_tuple5*Cyc__genarr_248[2]={& Cyc__gentuple_246,& Cyc__gentuple_247};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_pat_t46H2_rep={3,(struct _tagged_arr*)& Cyc__genname_249,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_248),(void*)((
struct _tuple5**)Cyc__genarr_248),(void*)((struct _tuple5**)Cyc__genarr_248 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_245={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_pat_t46H2_rep)};
struct _tuple41{unsigned int f1;struct Cyc_List_List*f2;int f3;};static struct _tuple6
Cyc__gentuple_434={offsetof(struct _tuple41,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_435={offsetof(struct _tuple41,f2),(void*)& Cyc__genrep_245};
static struct _tuple6 Cyc__gentuple_436={offsetof(struct _tuple41,f3),(void*)((void*)&
Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_437[3]={& Cyc__gentuple_434,& Cyc__gentuple_435,&
Cyc__gentuple_436};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_433={4,
sizeof(struct _tuple41),{(void*)((struct _tuple6**)Cyc__genarr_437),(void*)((
struct _tuple6**)Cyc__genarr_437),(void*)((struct _tuple6**)Cyc__genarr_437 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_429;struct _tuple42{unsigned int
f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple6 Cyc__gentuple_430={offsetof(
struct _tuple42,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_431={
offsetof(struct _tuple42,f2),(void*)& Cyc__genrep_240};static struct _tuple6*Cyc__genarr_432[
2]={& Cyc__gentuple_430,& Cyc__gentuple_431};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_429={4,sizeof(struct _tuple42),{(void*)((struct _tuple6**)Cyc__genarr_432),(
void*)((struct _tuple6**)Cyc__genarr_432),(void*)((struct _tuple6**)Cyc__genarr_432
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_339;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_aggr_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_411;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_aggr_kind_t_rep;static char _tmp287[8]="StructA";static struct _tuple7 Cyc__gentuple_396={
0,(struct _tagged_arr){_tmp287,_tmp287,_tmp287 + 8}};static char _tmp288[7]="UnionA";
static struct _tuple7 Cyc__gentuple_397={1,(struct _tagged_arr){_tmp288,_tmp288,
_tmp288 + 7}};static struct _tuple7*Cyc__genarr_398[2]={& Cyc__gentuple_396,& Cyc__gentuple_397};
static struct _tuple5*Cyc__genarr_399[0]={};static char _tmp28A[9]="AggrKind";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep={5,(struct _tagged_arr){
_tmp28A,_tmp28A,_tmp28A + 9},{(void*)((struct _tuple7**)Cyc__genarr_398),(void*)((
struct _tuple7**)Cyc__genarr_398),(void*)((struct _tuple7**)Cyc__genarr_398 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_399),(void*)((struct _tuple5**)Cyc__genarr_399),(
void*)((struct _tuple5**)Cyc__genarr_399 + 0)}};struct _tuple43{unsigned int f1;void*
f2;struct _tuple0*f3;};static struct _tuple6 Cyc__gentuple_412={offsetof(struct
_tuple43,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_413={
offsetof(struct _tuple43,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};static struct
_tuple6 Cyc__gentuple_414={offsetof(struct _tuple43,f3),(void*)& Cyc__genrep_11};
static struct _tuple6*Cyc__genarr_415[3]={& Cyc__gentuple_412,& Cyc__gentuple_413,&
Cyc__gentuple_414};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_411={4,
sizeof(struct _tuple43),{(void*)((struct _tuple6**)Cyc__genarr_415),(void*)((
struct _tuple6**)Cyc__genarr_415),(void*)((struct _tuple6**)Cyc__genarr_415 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_364;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_365;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_366;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_367;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_368;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_369;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_attribute_t46H2_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_attribute_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_89;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep;static char
_tmp28C[10]="Printf_ft";static struct _tuple7 Cyc__gentuple_90={0,(struct
_tagged_arr){_tmp28C,_tmp28C,_tmp28C + 10}};static char _tmp28D[9]="Scanf_ft";
static struct _tuple7 Cyc__gentuple_91={1,(struct _tagged_arr){_tmp28D,_tmp28D,
_tmp28D + 9}};static struct _tuple7*Cyc__genarr_92[2]={& Cyc__gentuple_90,& Cyc__gentuple_91};
static struct _tuple5*Cyc__genarr_93[0]={};static char _tmp28F[12]="Format_Type";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep={5,(struct
_tagged_arr){_tmp28F,_tmp28F,_tmp28F + 12},{(void*)((struct _tuple7**)Cyc__genarr_92),(
void*)((struct _tuple7**)Cyc__genarr_92),(void*)((struct _tuple7**)Cyc__genarr_92 + 
2)},{(void*)((struct _tuple5**)Cyc__genarr_93),(void*)((struct _tuple5**)Cyc__genarr_93),(
void*)((struct _tuple5**)Cyc__genarr_93 + 0)}};struct _tuple44{unsigned int f1;void*
f2;int f3;int f4;};static struct _tuple6 Cyc__gentuple_94={offsetof(struct _tuple44,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_95={offsetof(struct
_tuple44,f2),(void*)& Cyc_tunion_Absyn_Format_Type_rep};static struct _tuple6 Cyc__gentuple_96={
offsetof(struct _tuple44,f3),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6
Cyc__gentuple_97={offsetof(struct _tuple44,f4),(void*)((void*)& Cyc__genrep_10)};
static struct _tuple6*Cyc__genarr_98[4]={& Cyc__gentuple_94,& Cyc__gentuple_95,& Cyc__gentuple_96,&
Cyc__gentuple_97};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_89={4,sizeof(
struct _tuple44),{(void*)((struct _tuple6**)Cyc__genarr_98),(void*)((struct _tuple6**)
Cyc__genarr_98),(void*)((struct _tuple6**)Cyc__genarr_98 + 4)}};static char _tmp291[
12]="Stdcall_att";static struct _tuple7 Cyc__gentuple_63={0,(struct _tagged_arr){
_tmp291,_tmp291,_tmp291 + 12}};static char _tmp292[10]="Cdecl_att";static struct
_tuple7 Cyc__gentuple_64={1,(struct _tagged_arr){_tmp292,_tmp292,_tmp292 + 10}};
static char _tmp293[13]="Fastcall_att";static struct _tuple7 Cyc__gentuple_65={2,(
struct _tagged_arr){_tmp293,_tmp293,_tmp293 + 13}};static char _tmp294[13]="Noreturn_att";
static struct _tuple7 Cyc__gentuple_66={3,(struct _tagged_arr){_tmp294,_tmp294,
_tmp294 + 13}};static char _tmp295[10]="Const_att";static struct _tuple7 Cyc__gentuple_67={
4,(struct _tagged_arr){_tmp295,_tmp295,_tmp295 + 10}};static char _tmp296[11]="Packed_att";
static struct _tuple7 Cyc__gentuple_68={5,(struct _tagged_arr){_tmp296,_tmp296,
_tmp296 + 11}};static char _tmp297[13]="Nocommon_att";static struct _tuple7 Cyc__gentuple_69={
6,(struct _tagged_arr){_tmp297,_tmp297,_tmp297 + 13}};static char _tmp298[11]="Shared_att";
static struct _tuple7 Cyc__gentuple_70={7,(struct _tagged_arr){_tmp298,_tmp298,
_tmp298 + 11}};static char _tmp299[11]="Unused_att";static struct _tuple7 Cyc__gentuple_71={
8,(struct _tagged_arr){_tmp299,_tmp299,_tmp299 + 11}};static char _tmp29A[9]="Weak_att";
static struct _tuple7 Cyc__gentuple_72={9,(struct _tagged_arr){_tmp29A,_tmp29A,
_tmp29A + 9}};static char _tmp29B[14]="Dllimport_att";static struct _tuple7 Cyc__gentuple_73={
10,(struct _tagged_arr){_tmp29B,_tmp29B,_tmp29B + 14}};static char _tmp29C[14]="Dllexport_att";
static struct _tuple7 Cyc__gentuple_74={11,(struct _tagged_arr){_tmp29C,_tmp29C,
_tmp29C + 14}};static char _tmp29D[27]="No_instrument_function_att";static struct
_tuple7 Cyc__gentuple_75={12,(struct _tagged_arr){_tmp29D,_tmp29D,_tmp29D + 27}};
static char _tmp29E[16]="Constructor_att";static struct _tuple7 Cyc__gentuple_76={13,(
struct _tagged_arr){_tmp29E,_tmp29E,_tmp29E + 16}};static char _tmp29F[15]="Destructor_att";
static struct _tuple7 Cyc__gentuple_77={14,(struct _tagged_arr){_tmp29F,_tmp29F,
_tmp29F + 15}};static char _tmp2A0[26]="No_check_memory_usage_att";static struct
_tuple7 Cyc__gentuple_78={15,(struct _tagged_arr){_tmp2A0,_tmp2A0,_tmp2A0 + 26}};
static char _tmp2A1[9]="Pure_att";static struct _tuple7 Cyc__gentuple_79={16,(struct
_tagged_arr){_tmp2A1,_tmp2A1,_tmp2A1 + 9}};static struct _tuple7*Cyc__genarr_80[17]={&
Cyc__gentuple_63,& Cyc__gentuple_64,& Cyc__gentuple_65,& Cyc__gentuple_66,& Cyc__gentuple_67,&
Cyc__gentuple_68,& Cyc__gentuple_69,& Cyc__gentuple_70,& Cyc__gentuple_71,& Cyc__gentuple_72,&
Cyc__gentuple_73,& Cyc__gentuple_74,& Cyc__gentuple_75,& Cyc__gentuple_76,& Cyc__gentuple_77,&
Cyc__gentuple_78,& Cyc__gentuple_79};static char _tmp2A2[12]="Regparm_att";static
struct _tuple5 Cyc__gentuple_99={0,(struct _tagged_arr){_tmp2A2,_tmp2A2,_tmp2A2 + 12},(
void*)& Cyc__genrep_85};static char _tmp2A3[12]="Aligned_att";static struct _tuple5
Cyc__gentuple_100={1,(struct _tagged_arr){_tmp2A3,_tmp2A3,_tmp2A3 + 12},(void*)&
Cyc__genrep_85};static char _tmp2A4[12]="Section_att";static struct _tuple5 Cyc__gentuple_101={
2,(struct _tagged_arr){_tmp2A4,_tmp2A4,_tmp2A4 + 12},(void*)& Cyc__genrep_81};
static char _tmp2A5[11]="Format_att";static struct _tuple5 Cyc__gentuple_102={3,(
struct _tagged_arr){_tmp2A5,_tmp2A5,_tmp2A5 + 11},(void*)& Cyc__genrep_89};static
char _tmp2A6[16]="Initializes_att";static struct _tuple5 Cyc__gentuple_103={4,(
struct _tagged_arr){_tmp2A6,_tmp2A6,_tmp2A6 + 16},(void*)& Cyc__genrep_85};static
char _tmp2A7[9]="Mode_att";static struct _tuple5 Cyc__gentuple_104={5,(struct
_tagged_arr){_tmp2A7,_tmp2A7,_tmp2A7 + 9},(void*)& Cyc__genrep_81};static struct
_tuple5*Cyc__genarr_105[6]={& Cyc__gentuple_99,& Cyc__gentuple_100,& Cyc__gentuple_101,&
Cyc__gentuple_102,& Cyc__gentuple_103,& Cyc__gentuple_104};static char _tmp2A9[10]="Attribute";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep={5,(struct _tagged_arr){
_tmp2A9,_tmp2A9,_tmp2A9 + 10},{(void*)((struct _tuple7**)Cyc__genarr_80),(void*)((
struct _tuple7**)Cyc__genarr_80),(void*)((struct _tuple7**)Cyc__genarr_80 + 17)},{(
void*)((struct _tuple5**)Cyc__genarr_105),(void*)((struct _tuple5**)Cyc__genarr_105),(
void*)((struct _tuple5**)Cyc__genarr_105 + 6)}};static char _tmp2AA[5]="List";static
struct _tagged_arr Cyc__genname_109=(struct _tagged_arr){_tmp2AA,_tmp2AA,_tmp2AA + 5};
static char _tmp2AB[3]="hd";static struct _tuple5 Cyc__gentuple_106={offsetof(struct
Cyc_List_List,hd),(struct _tagged_arr){_tmp2AB,_tmp2AB,_tmp2AB + 3},(void*)& Cyc_Absyn_attribute_t_rep};
static char _tmp2AC[3]="tl";static struct _tuple5 Cyc__gentuple_107={offsetof(struct
Cyc_List_List,tl),(struct _tagged_arr){_tmp2AC,_tmp2AC,_tmp2AC + 3},(void*)& Cyc__genrep_62};
static struct _tuple5*Cyc__genarr_108[2]={& Cyc__gentuple_106,& Cyc__gentuple_107};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_109,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_108),(void*)((struct _tuple5**)Cyc__genarr_108),(void*)((
struct _tuple5**)Cyc__genarr_108 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_attribute_t46H2_rep)};static char
_tmp2AF[10]="Aggrfield";static struct _tagged_arr Cyc__genname_376=(struct
_tagged_arr){_tmp2AF,_tmp2AF,_tmp2AF + 10};static char _tmp2B0[5]="name";static
struct _tuple5 Cyc__gentuple_370={offsetof(struct Cyc_Absyn_Aggrfield,name),(struct
_tagged_arr){_tmp2B0,_tmp2B0,_tmp2B0 + 5},(void*)& Cyc__genrep_13};static char
_tmp2B1[3]="tq";static struct _tuple5 Cyc__gentuple_371={offsetof(struct Cyc_Absyn_Aggrfield,tq),(
struct _tagged_arr){_tmp2B1,_tmp2B1,_tmp2B1 + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp2B2[5]="type";static struct _tuple5 Cyc__gentuple_372={offsetof(
struct Cyc_Absyn_Aggrfield,type),(struct _tagged_arr){_tmp2B2,_tmp2B2,_tmp2B2 + 5},(
void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp2B3[6]="width";static struct
_tuple5 Cyc__gentuple_373={offsetof(struct Cyc_Absyn_Aggrfield,width),(struct
_tagged_arr){_tmp2B3,_tmp2B3,_tmp2B3 + 6},(void*)& Cyc__genrep_141};static char
_tmp2B4[11]="attributes";static struct _tuple5 Cyc__gentuple_374={offsetof(struct
Cyc_Absyn_Aggrfield,attributes),(struct _tagged_arr){_tmp2B4,_tmp2B4,_tmp2B4 + 11},(
void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_375[5]={& Cyc__gentuple_370,&
Cyc__gentuple_371,& Cyc__gentuple_372,& Cyc__gentuple_373,& Cyc__gentuple_374};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep={3,(struct
_tagged_arr*)& Cyc__genname_376,sizeof(struct Cyc_Absyn_Aggrfield),{(void*)((
struct _tuple5**)Cyc__genarr_375),(void*)((struct _tuple5**)Cyc__genarr_375),(void*)((
struct _tuple5**)Cyc__genarr_375 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_369={
1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrfield_rep)};static char _tmp2B7[5]="List";
static struct _tagged_arr Cyc__genname_380=(struct _tagged_arr){_tmp2B7,_tmp2B7,
_tmp2B7 + 5};static char _tmp2B8[3]="hd";static struct _tuple5 Cyc__gentuple_377={
offsetof(struct Cyc_List_List,hd),(struct _tagged_arr){_tmp2B8,_tmp2B8,_tmp2B8 + 3},(
void*)& Cyc__genrep_369};static char _tmp2B9[3]="tl";static struct _tuple5 Cyc__gentuple_378={
offsetof(struct Cyc_List_List,tl),(struct _tagged_arr){_tmp2B9,_tmp2B9,_tmp2B9 + 3},(
void*)& Cyc__genrep_368};static struct _tuple5*Cyc__genarr_379[2]={& Cyc__gentuple_377,&
Cyc__gentuple_378};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_380,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_379),(void*)((struct _tuple5**)Cyc__genarr_379),(void*)((
struct _tuple5**)Cyc__genarr_379 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_368={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep)};static char
_tmp2BC[13]="AggrdeclImpl";static struct _tagged_arr Cyc__genname_395=(struct
_tagged_arr){_tmp2BC,_tmp2BC,_tmp2BC + 13};static char _tmp2BD[11]="exist_vars";
static struct _tuple5 Cyc__gentuple_391={offsetof(struct Cyc_Absyn_AggrdeclImpl,exist_vars),(
struct _tagged_arr){_tmp2BD,_tmp2BD,_tmp2BD + 11},(void*)& Cyc__genrep_321};static
char _tmp2BE[7]="rgn_po";static struct _tuple5 Cyc__gentuple_392={offsetof(struct Cyc_Absyn_AggrdeclImpl,rgn_po),(
struct _tagged_arr){_tmp2BE,_tmp2BE,_tmp2BE + 7},(void*)& Cyc__genrep_381};static
char _tmp2BF[7]="fields";static struct _tuple5 Cyc__gentuple_393={offsetof(struct Cyc_Absyn_AggrdeclImpl,fields),(
struct _tagged_arr){_tmp2BF,_tmp2BF,_tmp2BF + 7},(void*)& Cyc__genrep_368};static
struct _tuple5*Cyc__genarr_394[3]={& Cyc__gentuple_391,& Cyc__gentuple_392,& Cyc__gentuple_393};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep={3,(struct
_tagged_arr*)& Cyc__genname_395,sizeof(struct Cyc_Absyn_AggrdeclImpl),{(void*)((
struct _tuple5**)Cyc__genarr_394),(void*)((struct _tuple5**)Cyc__genarr_394),(void*)((
struct _tuple5**)Cyc__genarr_394 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_367={
1,1,(void*)((void*)& Cyc_struct_Absyn_AggrdeclImpl_rep)};static char _tmp2C2[9]="Aggrdecl";
static struct _tagged_arr Cyc__genname_407=(struct _tagged_arr){_tmp2C2,_tmp2C2,
_tmp2C2 + 9};static char _tmp2C3[5]="kind";static struct _tuple5 Cyc__gentuple_400={
offsetof(struct Cyc_Absyn_Aggrdecl,kind),(struct _tagged_arr){_tmp2C3,_tmp2C3,
_tmp2C3 + 5},(void*)& Cyc_Absyn_aggr_kind_t_rep};static char _tmp2C4[3]="sc";static
struct _tuple5 Cyc__gentuple_401={offsetof(struct Cyc_Absyn_Aggrdecl,sc),(struct
_tagged_arr){_tmp2C4,_tmp2C4,_tmp2C4 + 3},(void*)& Cyc_Absyn_scope_t_rep};static
char _tmp2C5[5]="name";static struct _tuple5 Cyc__gentuple_402={offsetof(struct Cyc_Absyn_Aggrdecl,name),(
struct _tagged_arr){_tmp2C5,_tmp2C5,_tmp2C5 + 5},(void*)& Cyc__genrep_11};static
char _tmp2C6[4]="tvs";static struct _tuple5 Cyc__gentuple_403={offsetof(struct Cyc_Absyn_Aggrdecl,tvs),(
struct _tagged_arr){_tmp2C6,_tmp2C6,_tmp2C6 + 4},(void*)& Cyc__genrep_321};static
char _tmp2C7[5]="impl";static struct _tuple5 Cyc__gentuple_404={offsetof(struct Cyc_Absyn_Aggrdecl,impl),(
struct _tagged_arr){_tmp2C7,_tmp2C7,_tmp2C7 + 5},(void*)& Cyc__genrep_367};static
char _tmp2C8[11]="attributes";static struct _tuple5 Cyc__gentuple_405={offsetof(
struct Cyc_Absyn_Aggrdecl,attributes),(struct _tagged_arr){_tmp2C8,_tmp2C8,_tmp2C8
+ 11},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_406[6]={& Cyc__gentuple_400,&
Cyc__gentuple_401,& Cyc__gentuple_402,& Cyc__gentuple_403,& Cyc__gentuple_404,& Cyc__gentuple_405};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep={3,(struct
_tagged_arr*)& Cyc__genname_407,sizeof(struct Cyc_Absyn_Aggrdecl),{(void*)((struct
_tuple5**)Cyc__genarr_406),(void*)((struct _tuple5**)Cyc__genarr_406),(void*)((
struct _tuple5**)Cyc__genarr_406 + 6)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_366={
1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_365={1,1,(void*)((void*)& Cyc__genrep_366)};struct _tuple45{
unsigned int f1;struct Cyc_Absyn_Aggrdecl**f2;};static struct _tuple6 Cyc__gentuple_408={
offsetof(struct _tuple45,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_409={
offsetof(struct _tuple45,f2),(void*)& Cyc__genrep_365};static struct _tuple6*Cyc__genarr_410[
2]={& Cyc__gentuple_408,& Cyc__gentuple_409};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_364={4,sizeof(struct _tuple45),{(void*)((struct _tuple6**)Cyc__genarr_410),(
void*)((struct _tuple6**)Cyc__genarr_410),(void*)((struct _tuple6**)Cyc__genarr_410
+ 2)}};static struct _tuple7*Cyc__genarr_363[0]={};static char _tmp2CD[12]="UnknownAggr";
static struct _tuple5 Cyc__gentuple_416={0,(struct _tagged_arr){_tmp2CD,_tmp2CD,
_tmp2CD + 12},(void*)& Cyc__genrep_411};static char _tmp2CE[10]="KnownAggr";static
struct _tuple5 Cyc__gentuple_417={1,(struct _tagged_arr){_tmp2CE,_tmp2CE,_tmp2CE + 
10},(void*)& Cyc__genrep_364};static struct _tuple5*Cyc__genarr_418[2]={& Cyc__gentuple_416,&
Cyc__gentuple_417};static char _tmp2D0[10]="AggrInfoU";struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_AggrInfoU_rep={5,(struct _tagged_arr){_tmp2D0,_tmp2D0,_tmp2D0 + 
10},{(void*)((struct _tuple7**)Cyc__genarr_363),(void*)((struct _tuple7**)Cyc__genarr_363),(
void*)((struct _tuple7**)Cyc__genarr_363 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_418),(
void*)((struct _tuple5**)Cyc__genarr_418),(void*)((struct _tuple5**)Cyc__genarr_418
+ 2)}};static char _tmp2D1[9]="AggrInfo";static struct _tagged_arr Cyc__genname_422=(
struct _tagged_arr){_tmp2D1,_tmp2D1,_tmp2D1 + 9};static char _tmp2D2[10]="aggr_info";
static struct _tuple5 Cyc__gentuple_419={offsetof(struct Cyc_Absyn_AggrInfo,aggr_info),(
struct _tagged_arr){_tmp2D2,_tmp2D2,_tmp2D2 + 10},(void*)& Cyc_tunion_Absyn_AggrInfoU_rep};
static char _tmp2D3[6]="targs";static struct _tuple5 Cyc__gentuple_420={offsetof(
struct Cyc_Absyn_AggrInfo,targs),(struct _tagged_arr){_tmp2D3,_tmp2D3,_tmp2D3 + 6},(
void*)& Cyc__genrep_121};static struct _tuple5*Cyc__genarr_421[2]={& Cyc__gentuple_419,&
Cyc__gentuple_420};struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep={3,(
struct _tagged_arr*)& Cyc__genname_422,sizeof(struct Cyc_Absyn_AggrInfo),{(void*)((
struct _tuple5**)Cyc__genarr_421),(void*)((struct _tuple5**)Cyc__genarr_421),(void*)((
struct _tuple5**)Cyc__genarr_421 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_340;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_341;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_342;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_343;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_345;struct _tuple46{unsigned int f1;struct _tagged_arr*f2;};static
struct _tuple6 Cyc__gentuple_346={offsetof(struct _tuple46,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_347={offsetof(struct _tuple46,f2),(void*)& Cyc__genrep_13};
static struct _tuple6*Cyc__genarr_348[2]={& Cyc__gentuple_346,& Cyc__gentuple_347};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_345={4,sizeof(struct _tuple46),{(
void*)((struct _tuple6**)Cyc__genarr_348),(void*)((struct _tuple6**)Cyc__genarr_348),(
void*)((struct _tuple6**)Cyc__genarr_348 + 2)}};static struct _tuple7*Cyc__genarr_344[
0]={};static char _tmp2D6[13]="ArrayElement";static struct _tuple5 Cyc__gentuple_349={
0,(struct _tagged_arr){_tmp2D6,_tmp2D6,_tmp2D6 + 13},(void*)& Cyc__genrep_153};
static char _tmp2D7[10]="FieldName";static struct _tuple5 Cyc__gentuple_350={1,(
struct _tagged_arr){_tmp2D7,_tmp2D7,_tmp2D7 + 10},(void*)& Cyc__genrep_345};static
struct _tuple5*Cyc__genarr_351[2]={& Cyc__gentuple_349,& Cyc__gentuple_350};static
char _tmp2D9[11]="Designator";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep={
5,(struct _tagged_arr){_tmp2D9,_tmp2D9,_tmp2D9 + 11},{(void*)((struct _tuple7**)Cyc__genarr_344),(
void*)((struct _tuple7**)Cyc__genarr_344),(void*)((struct _tuple7**)Cyc__genarr_344
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_351),(void*)((struct _tuple5**)Cyc__genarr_351),(
void*)((struct _tuple5**)Cyc__genarr_351 + 2)}};static char _tmp2DA[5]="List";static
struct _tagged_arr Cyc__genname_355=(struct _tagged_arr){_tmp2DA,_tmp2DA,_tmp2DA + 5};
static char _tmp2DB[3]="hd";static struct _tuple5 Cyc__gentuple_352={offsetof(struct
Cyc_List_List,hd),(struct _tagged_arr){_tmp2DB,_tmp2DB,_tmp2DB + 3},(void*)& Cyc_Absyn_designator_t_rep};
static char _tmp2DC[3]="tl";static struct _tuple5 Cyc__gentuple_353={offsetof(struct
Cyc_List_List,tl),(struct _tagged_arr){_tmp2DC,_tmp2DC,_tmp2DC + 3},(void*)& Cyc__genrep_343};
static struct _tuple5*Cyc__genarr_354[2]={& Cyc__gentuple_352,& Cyc__gentuple_353};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_355,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_354),(void*)((struct _tuple5**)Cyc__genarr_354),(void*)((
struct _tuple5**)Cyc__genarr_354 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_343={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_designator_t46H2_rep)};struct
_tuple47{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple6 Cyc__gentuple_356={
offsetof(struct _tuple47,f1),(void*)& Cyc__genrep_343};static struct _tuple6 Cyc__gentuple_357={
offsetof(struct _tuple47,f2),(void*)& Cyc__genrep_240};static struct _tuple6*Cyc__genarr_358[
2]={& Cyc__gentuple_356,& Cyc__gentuple_357};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_342={4,sizeof(struct _tuple47),{(void*)((struct _tuple6**)Cyc__genarr_358),(
void*)((struct _tuple6**)Cyc__genarr_358),(void*)((struct _tuple6**)Cyc__genarr_358
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_341={1,1,(void*)((void*)&
Cyc__genrep_342)};static char _tmp2E1[5]="List";static struct _tagged_arr Cyc__genname_362=(
struct _tagged_arr){_tmp2E1,_tmp2E1,_tmp2E1 + 5};static char _tmp2E2[3]="hd";static
struct _tuple5 Cyc__gentuple_359={offsetof(struct Cyc_List_List,hd),(struct
_tagged_arr){_tmp2E2,_tmp2E2,_tmp2E2 + 3},(void*)& Cyc__genrep_341};static char
_tmp2E3[3]="tl";static struct _tuple5 Cyc__gentuple_360={offsetof(struct Cyc_List_List,tl),(
struct _tagged_arr){_tmp2E3,_tmp2E3,_tmp2E3 + 3},(void*)& Cyc__genrep_340};static
struct _tuple5*Cyc__genarr_361[2]={& Cyc__gentuple_359,& Cyc__gentuple_360};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_362,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_361),(void*)((struct _tuple5**)Cyc__genarr_361),(void*)((
struct _tuple5**)Cyc__genarr_361 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_340={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep)};
struct _tuple48{unsigned int f1;struct Cyc_Absyn_AggrInfo f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;int f5;};static struct _tuple6 Cyc__gentuple_423={offsetof(
struct _tuple48,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_424={
offsetof(struct _tuple48,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6 Cyc__gentuple_425={offsetof(struct _tuple48,f3),(void*)& Cyc__genrep_321};
static struct _tuple6 Cyc__gentuple_426={offsetof(struct _tuple48,f4),(void*)& Cyc__genrep_340};
static struct _tuple6 Cyc__gentuple_427={offsetof(struct _tuple48,f5),(void*)((void*)&
Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_428[5]={& Cyc__gentuple_423,& Cyc__gentuple_424,&
Cyc__gentuple_425,& Cyc__gentuple_426,& Cyc__gentuple_427};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_339={4,sizeof(struct _tuple48),{(void*)((struct _tuple6**)Cyc__genarr_428),(
void*)((struct _tuple6**)Cyc__genarr_428),(void*)((struct _tuple6**)Cyc__genarr_428
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_293;struct _tuple49{
unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*f3;
struct Cyc_List_List*f4;int f5;};static struct _tuple6 Cyc__gentuple_333={offsetof(
struct _tuple49,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_334={
offsetof(struct _tuple49,f2),(void*)((void*)& Cyc__genrep_311)};static struct
_tuple6 Cyc__gentuple_335={offsetof(struct _tuple49,f3),(void*)& Cyc__genrep_294};
static struct _tuple6 Cyc__gentuple_336={offsetof(struct _tuple49,f4),(void*)& Cyc__genrep_245};
static struct _tuple6 Cyc__gentuple_337={offsetof(struct _tuple49,f5),(void*)((void*)&
Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_338[5]={& Cyc__gentuple_333,& Cyc__gentuple_334,&
Cyc__gentuple_335,& Cyc__gentuple_336,& Cyc__gentuple_337};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_293={4,sizeof(struct _tuple49),{(void*)((struct _tuple6**)Cyc__genarr_338),(
void*)((struct _tuple6**)Cyc__genarr_338),(void*)((struct _tuple6**)Cyc__genarr_338
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_279;struct _tuple50{
unsigned int f1;char f2;};static struct _tuple6 Cyc__gentuple_280={offsetof(struct
_tuple50,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_281={
offsetof(struct _tuple50,f2),(void*)((void*)& Cyc__genrep_15)};static struct _tuple6*
Cyc__genarr_282[2]={& Cyc__gentuple_280,& Cyc__gentuple_281};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_279={4,sizeof(struct _tuple50),{(void*)((struct _tuple6**)Cyc__genarr_282),(
void*)((struct _tuple6**)Cyc__genarr_282),(void*)((struct _tuple6**)Cyc__genarr_282
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_264;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_265;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_266;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_139;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_140;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumfield_rep;static char _tmp2E9[10]="Enumfield";static struct
_tagged_arr Cyc__genname_930=(struct _tagged_arr){_tmp2E9,_tmp2E9,_tmp2E9 + 10};
static char _tmp2EA[5]="name";static struct _tuple5 Cyc__gentuple_926={offsetof(
struct Cyc_Absyn_Enumfield,name),(struct _tagged_arr){_tmp2EA,_tmp2EA,_tmp2EA + 5},(
void*)& Cyc__genrep_11};static char _tmp2EB[4]="tag";static struct _tuple5 Cyc__gentuple_927={
offsetof(struct Cyc_Absyn_Enumfield,tag),(struct _tagged_arr){_tmp2EB,_tmp2EB,
_tmp2EB + 4},(void*)& Cyc__genrep_141};static char _tmp2EC[4]="loc";static struct
_tuple5 Cyc__gentuple_928={offsetof(struct Cyc_Absyn_Enumfield,loc),(struct
_tagged_arr){_tmp2EC,_tmp2EC,_tmp2EC + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_929[3]={& Cyc__gentuple_926,& Cyc__gentuple_927,& Cyc__gentuple_928};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep={3,(struct
_tagged_arr*)& Cyc__genname_930,sizeof(struct Cyc_Absyn_Enumfield),{(void*)((
struct _tuple5**)Cyc__genarr_929),(void*)((struct _tuple5**)Cyc__genarr_929),(void*)((
struct _tuple5**)Cyc__genarr_929 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_140={
1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};static char _tmp2EF[5]="List";
static struct _tagged_arr Cyc__genname_934=(struct _tagged_arr){_tmp2EF,_tmp2EF,
_tmp2EF + 5};static char _tmp2F0[3]="hd";static struct _tuple5 Cyc__gentuple_931={
offsetof(struct Cyc_List_List,hd),(struct _tagged_arr){_tmp2F0,_tmp2F0,_tmp2F0 + 3},(
void*)& Cyc__genrep_140};static char _tmp2F1[3]="tl";static struct _tuple5 Cyc__gentuple_932={
offsetof(struct Cyc_List_List,tl),(struct _tagged_arr){_tmp2F1,_tmp2F1,_tmp2F1 + 3},(
void*)& Cyc__genrep_139};static struct _tuple5*Cyc__genarr_933[2]={& Cyc__gentuple_931,&
Cyc__gentuple_932};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_934,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_933),(void*)((struct _tuple5**)Cyc__genarr_933),(void*)((
struct _tuple5**)Cyc__genarr_933 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_139={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_enumfield_t46H2_rep)};static char
_tmp2F4[4]="Opt";static struct _tagged_arr Cyc__genname_269=(struct _tagged_arr){
_tmp2F4,_tmp2F4,_tmp2F4 + 4};static char _tmp2F5[2]="v";static struct _tuple5 Cyc__gentuple_267={
offsetof(struct Cyc_Core_Opt,v),(struct _tagged_arr){_tmp2F5,_tmp2F5,_tmp2F5 + 2},(
void*)& Cyc__genrep_139};static struct _tuple5*Cyc__genarr_268[1]={& Cyc__gentuple_267};
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep={
3,(struct _tagged_arr*)& Cyc__genname_269,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_268),(void*)((struct _tuple5**)Cyc__genarr_268),(void*)((
struct _tuple5**)Cyc__genarr_268 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_266={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep)};
static char _tmp2F8[9]="Enumdecl";static struct _tagged_arr Cyc__genname_274=(struct
_tagged_arr){_tmp2F8,_tmp2F8,_tmp2F8 + 9};static char _tmp2F9[3]="sc";static struct
_tuple5 Cyc__gentuple_270={offsetof(struct Cyc_Absyn_Enumdecl,sc),(struct
_tagged_arr){_tmp2F9,_tmp2F9,_tmp2F9 + 3},(void*)& Cyc_Absyn_scope_t_rep};static
char _tmp2FA[5]="name";static struct _tuple5 Cyc__gentuple_271={offsetof(struct Cyc_Absyn_Enumdecl,name),(
struct _tagged_arr){_tmp2FA,_tmp2FA,_tmp2FA + 5},(void*)& Cyc__genrep_11};static
char _tmp2FB[7]="fields";static struct _tuple5 Cyc__gentuple_272={offsetof(struct Cyc_Absyn_Enumdecl,fields),(
struct _tagged_arr){_tmp2FB,_tmp2FB,_tmp2FB + 7},(void*)& Cyc__genrep_266};static
struct _tuple5*Cyc__genarr_273[3]={& Cyc__gentuple_270,& Cyc__gentuple_271,& Cyc__gentuple_272};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep={3,(struct
_tagged_arr*)& Cyc__genname_274,sizeof(struct Cyc_Absyn_Enumdecl),{(void*)((struct
_tuple5**)Cyc__genarr_273),(void*)((struct _tuple5**)Cyc__genarr_273),(void*)((
struct _tuple5**)Cyc__genarr_273 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_265={
1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};struct _tuple51{unsigned int
f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};static struct
_tuple6 Cyc__gentuple_275={offsetof(struct _tuple51,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_276={offsetof(struct _tuple51,f2),(void*)& Cyc__genrep_265};
static struct _tuple6 Cyc__gentuple_277={offsetof(struct _tuple51,f3),(void*)& Cyc__genrep_140};
static struct _tuple6*Cyc__genarr_278[3]={& Cyc__gentuple_275,& Cyc__gentuple_276,&
Cyc__gentuple_277};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_264={4,
sizeof(struct _tuple51),{(void*)((struct _tuple6**)Cyc__genarr_278),(void*)((
struct _tuple6**)Cyc__genarr_278),(void*)((struct _tuple6**)Cyc__genarr_278 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_259;struct _tuple52{unsigned int
f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static struct _tuple6 Cyc__gentuple_260={
offsetof(struct _tuple52,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_261={
offsetof(struct _tuple52,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_262={offsetof(struct _tuple52,f3),(void*)& Cyc__genrep_140};
static struct _tuple6*Cyc__genarr_263[3]={& Cyc__gentuple_260,& Cyc__gentuple_261,&
Cyc__gentuple_262};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_259={4,
sizeof(struct _tuple52),{(void*)((struct _tuple6**)Cyc__genarr_263),(void*)((
struct _tuple6**)Cyc__genarr_263),(void*)((struct _tuple6**)Cyc__genarr_263 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_255;struct _tuple53{unsigned int
f1;struct _tuple0*f2;};static struct _tuple6 Cyc__gentuple_256={offsetof(struct
_tuple53,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_257={
offsetof(struct _tuple53,f2),(void*)& Cyc__genrep_11};static struct _tuple6*Cyc__genarr_258[
2]={& Cyc__gentuple_256,& Cyc__gentuple_257};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_255={4,sizeof(struct _tuple53),{(void*)((struct _tuple6**)Cyc__genarr_258),(
void*)((struct _tuple6**)Cyc__genarr_258),(void*)((struct _tuple6**)Cyc__genarr_258
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_244;struct _tuple54{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;int f4;};static struct
_tuple6 Cyc__gentuple_250={offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_251={offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_252={offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_245};
static struct _tuple6 Cyc__gentuple_253={offsetof(struct _tuple54,f4),(void*)((void*)&
Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_254[4]={& Cyc__gentuple_250,& Cyc__gentuple_251,&
Cyc__gentuple_252,& Cyc__gentuple_253};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_244={
4,sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_254),(void*)((
struct _tuple6**)Cyc__genarr_254),(void*)((struct _tuple6**)Cyc__genarr_254 + 4)}};
static char _tmp302[7]="Wild_p";static struct _tuple7 Cyc__gentuple_241={0,(struct
_tagged_arr){_tmp302,_tmp302,_tmp302 + 7}};static char _tmp303[7]="Null_p";static
struct _tuple7 Cyc__gentuple_242={1,(struct _tagged_arr){_tmp303,_tmp303,_tmp303 + 7}};
static struct _tuple7*Cyc__genarr_243[2]={& Cyc__gentuple_241,& Cyc__gentuple_242};
static char _tmp304[6]="Var_p";static struct _tuple5 Cyc__gentuple_447={0,(struct
_tagged_arr){_tmp304,_tmp304,_tmp304 + 6},(void*)& Cyc__genrep_443};static char
_tmp305[12]="Reference_p";static struct _tuple5 Cyc__gentuple_448={1,(struct
_tagged_arr){_tmp305,_tmp305,_tmp305 + 12},(void*)& Cyc__genrep_443};static char
_tmp306[9]="TagInt_p";static struct _tuple5 Cyc__gentuple_449={2,(struct _tagged_arr){
_tmp306,_tmp306,_tmp306 + 9},(void*)& Cyc__genrep_438};static char _tmp307[8]="Tuple_p";
static struct _tuple5 Cyc__gentuple_450={3,(struct _tagged_arr){_tmp307,_tmp307,
_tmp307 + 8},(void*)& Cyc__genrep_433};static char _tmp308[10]="Pointer_p";static
struct _tuple5 Cyc__gentuple_451={4,(struct _tagged_arr){_tmp308,_tmp308,_tmp308 + 
10},(void*)& Cyc__genrep_429};static char _tmp309[7]="Aggr_p";static struct _tuple5
Cyc__gentuple_452={5,(struct _tagged_arr){_tmp309,_tmp309,_tmp309 + 7},(void*)& Cyc__genrep_339};
static char _tmp30A[9]="Tunion_p";static struct _tuple5 Cyc__gentuple_453={6,(struct
_tagged_arr){_tmp30A,_tmp30A,_tmp30A + 9},(void*)& Cyc__genrep_293};static char
_tmp30B[6]="Int_p";static struct _tuple5 Cyc__gentuple_454={7,(struct _tagged_arr){
_tmp30B,_tmp30B,_tmp30B + 6},(void*)& Cyc__genrep_283};static char _tmp30C[7]="Char_p";
static struct _tuple5 Cyc__gentuple_455={8,(struct _tagged_arr){_tmp30C,_tmp30C,
_tmp30C + 7},(void*)& Cyc__genrep_279};static char _tmp30D[8]="Float_p";static struct
_tuple5 Cyc__gentuple_456={9,(struct _tagged_arr){_tmp30D,_tmp30D,_tmp30D + 8},(
void*)& Cyc__genrep_81};static char _tmp30E[7]="Enum_p";static struct _tuple5 Cyc__gentuple_457={
10,(struct _tagged_arr){_tmp30E,_tmp30E,_tmp30E + 7},(void*)& Cyc__genrep_264};
static char _tmp30F[11]="AnonEnum_p";static struct _tuple5 Cyc__gentuple_458={11,(
struct _tagged_arr){_tmp30F,_tmp30F,_tmp30F + 11},(void*)& Cyc__genrep_259};static
char _tmp310[12]="UnknownId_p";static struct _tuple5 Cyc__gentuple_459={12,(struct
_tagged_arr){_tmp310,_tmp310,_tmp310 + 12},(void*)& Cyc__genrep_255};static char
_tmp311[14]="UnknownCall_p";static struct _tuple5 Cyc__gentuple_460={13,(struct
_tagged_arr){_tmp311,_tmp311,_tmp311 + 14},(void*)& Cyc__genrep_244};static char
_tmp312[6]="Exp_p";static struct _tuple5 Cyc__gentuple_461={14,(struct _tagged_arr){
_tmp312,_tmp312,_tmp312 + 6},(void*)& Cyc__genrep_153};static struct _tuple5*Cyc__genarr_462[
15]={& Cyc__gentuple_447,& Cyc__gentuple_448,& Cyc__gentuple_449,& Cyc__gentuple_450,&
Cyc__gentuple_451,& Cyc__gentuple_452,& Cyc__gentuple_453,& Cyc__gentuple_454,& Cyc__gentuple_455,&
Cyc__gentuple_456,& Cyc__gentuple_457,& Cyc__gentuple_458,& Cyc__gentuple_459,& Cyc__gentuple_460,&
Cyc__gentuple_461};static char _tmp314[8]="Raw_pat";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep={5,(struct _tagged_arr){_tmp314,_tmp314,_tmp314 + 8},{(
void*)((struct _tuple7**)Cyc__genarr_243),(void*)((struct _tuple7**)Cyc__genarr_243),(
void*)((struct _tuple7**)Cyc__genarr_243 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_462),(
void*)((struct _tuple5**)Cyc__genarr_462),(void*)((struct _tuple5**)Cyc__genarr_462
+ 15)}};static char _tmp315[4]="Pat";static struct _tagged_arr Cyc__genname_467=(
struct _tagged_arr){_tmp315,_tmp315,_tmp315 + 4};static char _tmp316[2]="r";static
struct _tuple5 Cyc__gentuple_463={offsetof(struct Cyc_Absyn_Pat,r),(struct
_tagged_arr){_tmp316,_tmp316,_tmp316 + 2},(void*)& Cyc_Absyn_raw_pat_t_rep};static
char _tmp317[5]="topt";static struct _tuple5 Cyc__gentuple_464={offsetof(struct Cyc_Absyn_Pat,topt),(
struct _tagged_arr){_tmp317,_tmp317,_tmp317 + 5},(void*)& Cyc__genrep_110};static
char _tmp318[4]="loc";static struct _tuple5 Cyc__gentuple_465={offsetof(struct Cyc_Absyn_Pat,loc),(
struct _tagged_arr){_tmp318,_tmp318,_tmp318 + 4},(void*)& Cyc__genrep_2};static
struct _tuple5*Cyc__genarr_466[3]={& Cyc__gentuple_463,& Cyc__gentuple_464,& Cyc__gentuple_465};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep={3,(struct _tagged_arr*)&
Cyc__genname_467,sizeof(struct Cyc_Absyn_Pat),{(void*)((struct _tuple5**)Cyc__genarr_466),(
void*)((struct _tuple5**)Cyc__genarr_466),(void*)((struct _tuple5**)Cyc__genarr_466
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_240={1,1,(void*)((void*)&
Cyc_struct_Absyn_Pat_rep)};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_231;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_232;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_vardecl_t46H2_rep;static char _tmp31B[5]="List";static
struct _tagged_arr Cyc__genname_236=(struct _tagged_arr){_tmp31B,_tmp31B,_tmp31B + 5};
static char _tmp31C[3]="hd";static struct _tuple5 Cyc__gentuple_233={offsetof(struct
Cyc_List_List,hd),(struct _tagged_arr){_tmp31C,_tmp31C,_tmp31C + 3},(void*)& Cyc__genrep_159};
static char _tmp31D[3]="tl";static struct _tuple5 Cyc__gentuple_234={offsetof(struct
Cyc_List_List,tl),(struct _tagged_arr){_tmp31D,_tmp31D,_tmp31D + 3},(void*)& Cyc__genrep_232};
static struct _tuple5*Cyc__genarr_235[2]={& Cyc__gentuple_233,& Cyc__gentuple_234};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_vardecl_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_236,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_235),(void*)((struct _tuple5**)Cyc__genarr_235),(void*)((
struct _tuple5**)Cyc__genarr_235 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_232={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_vardecl_t46H2_rep)};static char
_tmp320[4]="Opt";static struct _tagged_arr Cyc__genname_239=(struct _tagged_arr){
_tmp320,_tmp320,_tmp320 + 4};static char _tmp321[2]="v";static struct _tuple5 Cyc__gentuple_237={
offsetof(struct Cyc_Core_Opt,v),(struct _tagged_arr){_tmp321,_tmp321,_tmp321 + 2},(
void*)& Cyc__genrep_232};static struct _tuple5*Cyc__genarr_238[1]={& Cyc__gentuple_237};
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep={
3,(struct _tagged_arr*)& Cyc__genname_239,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_238),(void*)((struct _tuple5**)Cyc__genarr_238),(void*)((
struct _tuple5**)Cyc__genarr_238 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_231={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep)};
static char _tmp324[14]="Switch_clause";static struct _tagged_arr Cyc__genname_474=(
struct _tagged_arr){_tmp324,_tmp324,_tmp324 + 14};static char _tmp325[8]="pattern";
static struct _tuple5 Cyc__gentuple_468={offsetof(struct Cyc_Absyn_Switch_clause,pattern),(
struct _tagged_arr){_tmp325,_tmp325,_tmp325 + 8},(void*)& Cyc__genrep_240};static
char _tmp326[9]="pat_vars";static struct _tuple5 Cyc__gentuple_469={offsetof(struct
Cyc_Absyn_Switch_clause,pat_vars),(struct _tagged_arr){_tmp326,_tmp326,_tmp326 + 9},(
void*)& Cyc__genrep_231};static char _tmp327[13]="where_clause";static struct _tuple5
Cyc__gentuple_470={offsetof(struct Cyc_Absyn_Switch_clause,where_clause),(struct
_tagged_arr){_tmp327,_tmp327,_tmp327 + 13},(void*)& Cyc__genrep_141};static char
_tmp328[5]="body";static struct _tuple5 Cyc__gentuple_471={offsetof(struct Cyc_Absyn_Switch_clause,body),(
struct _tagged_arr){_tmp328,_tmp328,_tmp328 + 5},(void*)& Cyc__genrep_145};static
char _tmp329[4]="loc";static struct _tuple5 Cyc__gentuple_472={offsetof(struct Cyc_Absyn_Switch_clause,loc),(
struct _tagged_arr){_tmp329,_tmp329,_tmp329 + 4},(void*)& Cyc__genrep_2};static
struct _tuple5*Cyc__genarr_473[5]={& Cyc__gentuple_468,& Cyc__gentuple_469,& Cyc__gentuple_470,&
Cyc__gentuple_471,& Cyc__gentuple_472};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep={
3,(struct _tagged_arr*)& Cyc__genname_474,sizeof(struct Cyc_Absyn_Switch_clause),{(
void*)((struct _tuple5**)Cyc__genarr_473),(void*)((struct _tuple5**)Cyc__genarr_473),(
void*)((struct _tuple5**)Cyc__genarr_473 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_230={1,1,(void*)((void*)& Cyc_struct_Absyn_Switch_clause_rep)};static
char _tmp32C[5]="List";static struct _tagged_arr Cyc__genname_478=(struct _tagged_arr){
_tmp32C,_tmp32C,_tmp32C + 5};static char _tmp32D[3]="hd";static struct _tuple5 Cyc__gentuple_475={
offsetof(struct Cyc_List_List,hd),(struct _tagged_arr){_tmp32D,_tmp32D,_tmp32D + 3},(
void*)((void*)& Cyc__genrep_230)};static char _tmp32E[3]="tl";static struct _tuple5
Cyc__gentuple_476={offsetof(struct Cyc_List_List,tl),(struct _tagged_arr){_tmp32E,
_tmp32E,_tmp32E + 3},(void*)& Cyc__genrep_229};static struct _tuple5*Cyc__genarr_477[
2]={& Cyc__gentuple_475,& Cyc__gentuple_476};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_478,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_477),(void*)((struct _tuple5**)Cyc__genarr_477),(void*)((
struct _tuple5**)Cyc__genarr_477 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_229={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep)};struct
_tuple55{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_514={offsetof(struct _tuple55,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_515={offsetof(struct _tuple55,f2),(void*)& Cyc__genrep_154};
static struct _tuple6 Cyc__gentuple_516={offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_229};
static struct _tuple6*Cyc__genarr_517[3]={& Cyc__gentuple_514,& Cyc__gentuple_515,&
Cyc__gentuple_516};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_513={4,
sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_517),(void*)((
struct _tuple6**)Cyc__genarr_517),(void*)((struct _tuple6**)Cyc__genarr_517 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_502;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_504;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep;
static char _tmp332[5]="List";static struct _tagged_arr Cyc__genname_508=(struct
_tagged_arr){_tmp332,_tmp332,_tmp332 + 5};static char _tmp333[3]="hd";static struct
_tuple5 Cyc__gentuple_505={offsetof(struct Cyc_List_List,hd),(struct _tagged_arr){
_tmp333,_tmp333,_tmp333 + 3},(void*)& Cyc__genrep_154};static char _tmp334[3]="tl";
static struct _tuple5 Cyc__gentuple_506={offsetof(struct Cyc_List_List,tl),(struct
_tagged_arr){_tmp334,_tmp334,_tmp334 + 3},(void*)& Cyc__genrep_504};static struct
_tuple5*Cyc__genarr_507[2]={& Cyc__gentuple_505,& Cyc__gentuple_506};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_exp_t46H2_rep={3,(struct _tagged_arr*)& Cyc__genname_508,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_507),(void*)((
struct _tuple5**)Cyc__genarr_507),(void*)((struct _tuple5**)Cyc__genarr_507 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_504={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_exp_t46H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_503;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_503={1,1,(void*)((void*)& Cyc__genrep_230)};struct _tuple56{
unsigned int f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Switch_clause**f3;};static
struct _tuple6 Cyc__gentuple_509={offsetof(struct _tuple56,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_510={offsetof(struct _tuple56,f2),(void*)& Cyc__genrep_504};
static struct _tuple6 Cyc__gentuple_511={offsetof(struct _tuple56,f3),(void*)& Cyc__genrep_503};
static struct _tuple6*Cyc__genarr_512[3]={& Cyc__gentuple_509,& Cyc__gentuple_510,&
Cyc__gentuple_511};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_502={4,
sizeof(struct _tuple56),{(void*)((struct _tuple6**)Cyc__genarr_512),(void*)((
struct _tuple6**)Cyc__genarr_512),(void*)((struct _tuple6**)Cyc__genarr_512 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_497;struct _tuple57{unsigned int
f1;struct Cyc_Absyn_Decl*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_498={
offsetof(struct _tuple57,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_499={
offsetof(struct _tuple57,f2),(void*)& Cyc__genrep_1};static struct _tuple6 Cyc__gentuple_500={
offsetof(struct _tuple57,f3),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_501[
3]={& Cyc__gentuple_498,& Cyc__gentuple_499,& Cyc__gentuple_500};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_497={4,sizeof(struct _tuple57),{(void*)((struct _tuple6**)Cyc__genarr_501),(
void*)((struct _tuple6**)Cyc__genarr_501),(void*)((struct _tuple6**)Cyc__genarr_501
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_492;static struct _tuple6
Cyc__gentuple_493={offsetof(struct _tuple38,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_494={offsetof(struct _tuple38,f2),(void*)& Cyc__genrep_13};
static struct _tuple6 Cyc__gentuple_495={offsetof(struct _tuple38,f3),(void*)& Cyc__genrep_145};
static struct _tuple6*Cyc__genarr_496[3]={& Cyc__gentuple_493,& Cyc__gentuple_494,&
Cyc__gentuple_495};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_492={4,
sizeof(struct _tuple38),{(void*)((struct _tuple6**)Cyc__genarr_496),(void*)((
struct _tuple6**)Cyc__genarr_496),(void*)((struct _tuple6**)Cyc__genarr_496 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_483;struct _tuple58{unsigned int
f1;struct Cyc_Absyn_Stmt*f2;struct _tuple2 f3;};static struct _tuple6 Cyc__gentuple_488={
offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_489={
offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_145};static struct _tuple6 Cyc__gentuple_490={
offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_484};static struct _tuple6*Cyc__genarr_491[
3]={& Cyc__gentuple_488,& Cyc__gentuple_489,& Cyc__gentuple_490};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_483={4,sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_491),(
void*)((struct _tuple6**)Cyc__genarr_491),(void*)((struct _tuple6**)Cyc__genarr_491
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_228;struct _tuple59{
unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_479={offsetof(struct _tuple59,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_480={offsetof(struct _tuple59,f2),(void*)& Cyc__genrep_145};
static struct _tuple6 Cyc__gentuple_481={offsetof(struct _tuple59,f3),(void*)& Cyc__genrep_229};
static struct _tuple6*Cyc__genarr_482[3]={& Cyc__gentuple_479,& Cyc__gentuple_480,&
Cyc__gentuple_481};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_228={4,
sizeof(struct _tuple59),{(void*)((struct _tuple6**)Cyc__genarr_482),(void*)((
struct _tuple6**)Cyc__genarr_482),(void*)((struct _tuple6**)Cyc__genarr_482 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_158;struct _tuple60{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;int f4;struct Cyc_Absyn_Stmt*
f5;};static struct _tuple6 Cyc__gentuple_222={offsetof(struct _tuple60,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_223={offsetof(struct _tuple60,f2),(
void*)& Cyc__genrep_185};static struct _tuple6 Cyc__gentuple_224={offsetof(struct
_tuple60,f3),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_225={
offsetof(struct _tuple60,f4),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6
Cyc__gentuple_226={offsetof(struct _tuple60,f5),(void*)& Cyc__genrep_145};static
struct _tuple6*Cyc__genarr_227[5]={& Cyc__gentuple_222,& Cyc__gentuple_223,& Cyc__gentuple_224,&
Cyc__gentuple_225,& Cyc__gentuple_226};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_158={
4,sizeof(struct _tuple60),{(void*)((struct _tuple6**)Cyc__genarr_227),(void*)((
struct _tuple6**)Cyc__genarr_227),(void*)((struct _tuple6**)Cyc__genarr_227 + 5)}};
static char _tmp33E[7]="Skip_s";static struct _tuple7 Cyc__gentuple_151={0,(struct
_tagged_arr){_tmp33E,_tmp33E,_tmp33E + 7}};static struct _tuple7*Cyc__genarr_152[1]={&
Cyc__gentuple_151};static char _tmp33F[6]="Exp_s";static struct _tuple5 Cyc__gentuple_555={
0,(struct _tagged_arr){_tmp33F,_tmp33F,_tmp33F + 6},(void*)& Cyc__genrep_153};
static char _tmp340[6]="Seq_s";static struct _tuple5 Cyc__gentuple_556={1,(struct
_tagged_arr){_tmp340,_tmp340,_tmp340 + 6},(void*)& Cyc__genrep_550};static char
_tmp341[9]="Return_s";static struct _tuple5 Cyc__gentuple_557={2,(struct _tagged_arr){
_tmp341,_tmp341,_tmp341 + 9},(void*)& Cyc__genrep_546};static char _tmp342[13]="IfThenElse_s";
static struct _tuple5 Cyc__gentuple_558={3,(struct _tagged_arr){_tmp342,_tmp342,
_tmp342 + 13},(void*)& Cyc__genrep_540};static char _tmp343[8]="While_s";static
struct _tuple5 Cyc__gentuple_559={4,(struct _tagged_arr){_tmp343,_tmp343,_tmp343 + 8},(
void*)& Cyc__genrep_535};static char _tmp344[8]="Break_s";static struct _tuple5 Cyc__gentuple_560={
5,(struct _tagged_arr){_tmp344,_tmp344,_tmp344 + 8},(void*)& Cyc__genrep_531};
static char _tmp345[11]="Continue_s";static struct _tuple5 Cyc__gentuple_561={6,(
struct _tagged_arr){_tmp345,_tmp345,_tmp345 + 11},(void*)& Cyc__genrep_531};static
char _tmp346[7]="Goto_s";static struct _tuple5 Cyc__gentuple_562={7,(struct
_tagged_arr){_tmp346,_tmp346,_tmp346 + 7},(void*)& Cyc__genrep_525};static char
_tmp347[6]="For_s";static struct _tuple5 Cyc__gentuple_563={8,(struct _tagged_arr){
_tmp347,_tmp347,_tmp347 + 6},(void*)& Cyc__genrep_518};static char _tmp348[9]="Switch_s";
static struct _tuple5 Cyc__gentuple_564={9,(struct _tagged_arr){_tmp348,_tmp348,
_tmp348 + 9},(void*)& Cyc__genrep_513};static char _tmp349[11]="Fallthru_s";static
struct _tuple5 Cyc__gentuple_565={10,(struct _tagged_arr){_tmp349,_tmp349,_tmp349 + 
11},(void*)& Cyc__genrep_502};static char _tmp34A[7]="Decl_s";static struct _tuple5
Cyc__gentuple_566={11,(struct _tagged_arr){_tmp34A,_tmp34A,_tmp34A + 7},(void*)&
Cyc__genrep_497};static char _tmp34B[8]="Label_s";static struct _tuple5 Cyc__gentuple_567={
12,(struct _tagged_arr){_tmp34B,_tmp34B,_tmp34B + 8},(void*)& Cyc__genrep_492};
static char _tmp34C[5]="Do_s";static struct _tuple5 Cyc__gentuple_568={13,(struct
_tagged_arr){_tmp34C,_tmp34C,_tmp34C + 5},(void*)& Cyc__genrep_483};static char
_tmp34D[11]="TryCatch_s";static struct _tuple5 Cyc__gentuple_569={14,(struct
_tagged_arr){_tmp34D,_tmp34D,_tmp34D + 11},(void*)& Cyc__genrep_228};static char
_tmp34E[9]="Region_s";static struct _tuple5 Cyc__gentuple_570={15,(struct
_tagged_arr){_tmp34E,_tmp34E,_tmp34E + 9},(void*)& Cyc__genrep_158};static char
_tmp34F[14]="ResetRegion_s";static struct _tuple5 Cyc__gentuple_571={16,(struct
_tagged_arr){_tmp34F,_tmp34F,_tmp34F + 14},(void*)& Cyc__genrep_153};static struct
_tuple5*Cyc__genarr_572[17]={& Cyc__gentuple_555,& Cyc__gentuple_556,& Cyc__gentuple_557,&
Cyc__gentuple_558,& Cyc__gentuple_559,& Cyc__gentuple_560,& Cyc__gentuple_561,& Cyc__gentuple_562,&
Cyc__gentuple_563,& Cyc__gentuple_564,& Cyc__gentuple_565,& Cyc__gentuple_566,& Cyc__gentuple_567,&
Cyc__gentuple_568,& Cyc__gentuple_569,& Cyc__gentuple_570,& Cyc__gentuple_571};
static char _tmp351[9]="Raw_stmt";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep={
5,(struct _tagged_arr){_tmp351,_tmp351,_tmp351 + 9},{(void*)((struct _tuple7**)Cyc__genarr_152),(
void*)((struct _tuple7**)Cyc__genarr_152),(void*)((struct _tuple7**)Cyc__genarr_152
+ 1)},{(void*)((struct _tuple5**)Cyc__genarr_572),(void*)((struct _tuple5**)Cyc__genarr_572),(
void*)((struct _tuple5**)Cyc__genarr_572 + 17)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_146;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep;
static char _tmp352[5]="List";static struct _tagged_arr Cyc__genname_150=(struct
_tagged_arr){_tmp352,_tmp352,_tmp352 + 5};static char _tmp353[3]="hd";static struct
_tuple5 Cyc__gentuple_147={offsetof(struct Cyc_List_List,hd),(struct _tagged_arr){
_tmp353,_tmp353,_tmp353 + 3},(void*)& Cyc__genrep_145};static char _tmp354[3]="tl";
static struct _tuple5 Cyc__gentuple_148={offsetof(struct Cyc_List_List,tl),(struct
_tagged_arr){_tmp354,_tmp354,_tmp354 + 3},(void*)& Cyc__genrep_146};static struct
_tuple5*Cyc__genarr_149[2]={& Cyc__gentuple_147,& Cyc__gentuple_148};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_stmt_t46H2_rep={3,(struct _tagged_arr*)& Cyc__genname_150,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_149),(void*)((
struct _tuple5**)Cyc__genarr_149),(void*)((struct _tuple5**)Cyc__genarr_149 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_146={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_stmt_t46H2_rep)};
extern struct Cyc_Typerep_XTUnion_struct Cyc_Absyn_absyn_annot_t_rep;static struct
_tuple8*Cyc__genarr_142[0]={};static char _tmp358[11]="AbsynAnnot";struct Cyc_Typerep_XTUnion_struct
Cyc_Absyn_absyn_annot_t_rep={7,(struct _tagged_arr){_tmp358,_tmp358,_tmp358 + 11},{(
void*)((struct _tuple8**)Cyc__genarr_142),(void*)((struct _tuple8**)Cyc__genarr_142),(
void*)((struct _tuple8**)Cyc__genarr_142 + 0)}};static char _tmp359[5]="Stmt";static
struct _tagged_arr Cyc__genname_579=(struct _tagged_arr){_tmp359,_tmp359,_tmp359 + 5};
static char _tmp35A[2]="r";static struct _tuple5 Cyc__gentuple_573={offsetof(struct
Cyc_Absyn_Stmt,r),(struct _tagged_arr){_tmp35A,_tmp35A,_tmp35A + 2},(void*)& Cyc_Absyn_raw_stmt_t_rep};
static char _tmp35B[4]="loc";static struct _tuple5 Cyc__gentuple_574={offsetof(struct
Cyc_Absyn_Stmt,loc),(struct _tagged_arr){_tmp35B,_tmp35B,_tmp35B + 4},(void*)& Cyc__genrep_2};
static char _tmp35C[16]="non_local_preds";static struct _tuple5 Cyc__gentuple_575={
offsetof(struct Cyc_Absyn_Stmt,non_local_preds),(struct _tagged_arr){_tmp35C,
_tmp35C,_tmp35C + 16},(void*)& Cyc__genrep_146};static char _tmp35D[10]="try_depth";
static struct _tuple5 Cyc__gentuple_576={offsetof(struct Cyc_Absyn_Stmt,try_depth),(
struct _tagged_arr){_tmp35D,_tmp35D,_tmp35D + 10},(void*)((void*)& Cyc__genrep_10)};
static char _tmp35E[6]="annot";static struct _tuple5 Cyc__gentuple_577={offsetof(
struct Cyc_Absyn_Stmt,annot),(struct _tagged_arr){_tmp35E,_tmp35E,_tmp35E + 6},(
void*)& Cyc_Absyn_absyn_annot_t_rep};static struct _tuple5*Cyc__genarr_578[5]={& Cyc__gentuple_573,&
Cyc__gentuple_574,& Cyc__gentuple_575,& Cyc__gentuple_576,& Cyc__gentuple_577};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Stmt_rep={3,(struct _tagged_arr*)&
Cyc__genname_579,sizeof(struct Cyc_Absyn_Stmt),{(void*)((struct _tuple5**)Cyc__genarr_578),(
void*)((struct _tuple5**)Cyc__genarr_578),(void*)((struct _tuple5**)Cyc__genarr_578
+ 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_145={1,1,(void*)((void*)&
Cyc_struct_Absyn_Stmt_rep)};static char _tmp361[7]="Fndecl";static struct
_tagged_arr Cyc__genname_837=(struct _tagged_arr){_tmp361,_tmp361,_tmp361 + 7};
static char _tmp362[3]="sc";static struct _tuple5 Cyc__gentuple_822={offsetof(struct
Cyc_Absyn_Fndecl,sc),(struct _tagged_arr){_tmp362,_tmp362,_tmp362 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp363[10]="is_inline";static struct _tuple5 Cyc__gentuple_823={
offsetof(struct Cyc_Absyn_Fndecl,is_inline),(struct _tagged_arr){_tmp363,_tmp363,
_tmp363 + 10},(void*)((void*)& Cyc__genrep_10)};static char _tmp364[5]="name";static
struct _tuple5 Cyc__gentuple_824={offsetof(struct Cyc_Absyn_Fndecl,name),(struct
_tagged_arr){_tmp364,_tmp364,_tmp364 + 5},(void*)& Cyc__genrep_11};static char
_tmp365[4]="tvs";static struct _tuple5 Cyc__gentuple_825={offsetof(struct Cyc_Absyn_Fndecl,tvs),(
struct _tagged_arr){_tmp365,_tmp365,_tmp365 + 4},(void*)& Cyc__genrep_321};static
char _tmp366[7]="effect";static struct _tuple5 Cyc__gentuple_826={offsetof(struct Cyc_Absyn_Fndecl,effect),(
struct _tagged_arr){_tmp366,_tmp366,_tmp366 + 7},(void*)& Cyc__genrep_110};static
char _tmp367[9]="ret_type";static struct _tuple5 Cyc__gentuple_827={offsetof(struct
Cyc_Absyn_Fndecl,ret_type),(struct _tagged_arr){_tmp367,_tmp367,_tmp367 + 9},(void*)((
void*)& Cyc_Absyn_type_t_rep)};static char _tmp368[5]="args";static struct _tuple5 Cyc__gentuple_828={
offsetof(struct Cyc_Absyn_Fndecl,args),(struct _tagged_arr){_tmp368,_tmp368,
_tmp368 + 5},(void*)& Cyc__genrep_811};static char _tmp369[10]="c_varargs";static
struct _tuple5 Cyc__gentuple_829={offsetof(struct Cyc_Absyn_Fndecl,c_varargs),(
struct _tagged_arr){_tmp369,_tmp369,_tmp369 + 10},(void*)((void*)& Cyc__genrep_10)};
static char _tmp36A[12]="cyc_varargs";static struct _tuple5 Cyc__gentuple_830={
offsetof(struct Cyc_Absyn_Fndecl,cyc_varargs),(struct _tagged_arr){_tmp36A,_tmp36A,
_tmp36A + 12},(void*)& Cyc__genrep_810};static char _tmp36B[7]="rgn_po";static struct
_tuple5 Cyc__gentuple_831={offsetof(struct Cyc_Absyn_Fndecl,rgn_po),(struct
_tagged_arr){_tmp36B,_tmp36B,_tmp36B + 7},(void*)& Cyc__genrep_381};static char
_tmp36C[5]="body";static struct _tuple5 Cyc__gentuple_832={offsetof(struct Cyc_Absyn_Fndecl,body),(
struct _tagged_arr){_tmp36C,_tmp36C,_tmp36C + 5},(void*)& Cyc__genrep_145};static
char _tmp36D[11]="cached_typ";static struct _tuple5 Cyc__gentuple_833={offsetof(
struct Cyc_Absyn_Fndecl,cached_typ),(struct _tagged_arr){_tmp36D,_tmp36D,_tmp36D + 
11},(void*)& Cyc__genrep_110};static char _tmp36E[15]="param_vardecls";static struct
_tuple5 Cyc__gentuple_834={offsetof(struct Cyc_Absyn_Fndecl,param_vardecls),(
struct _tagged_arr){_tmp36E,_tmp36E,_tmp36E + 15},(void*)& Cyc__genrep_231};static
char _tmp36F[11]="attributes";static struct _tuple5 Cyc__gentuple_835={offsetof(
struct Cyc_Absyn_Fndecl,attributes),(struct _tagged_arr){_tmp36F,_tmp36F,_tmp36F + 
11},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_836[14]={& Cyc__gentuple_822,&
Cyc__gentuple_823,& Cyc__gentuple_824,& Cyc__gentuple_825,& Cyc__gentuple_826,& Cyc__gentuple_827,&
Cyc__gentuple_828,& Cyc__gentuple_829,& Cyc__gentuple_830,& Cyc__gentuple_831,& Cyc__gentuple_832,&
Cyc__gentuple_833,& Cyc__gentuple_834,& Cyc__gentuple_835};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Fndecl_rep={3,(struct _tagged_arr*)& Cyc__genname_837,sizeof(
struct Cyc_Absyn_Fndecl),{(void*)((struct _tuple5**)Cyc__genarr_836),(void*)((
struct _tuple5**)Cyc__genarr_836),(void*)((struct _tuple5**)Cyc__genarr_836 + 14)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_809={1,1,(void*)((void*)& Cyc_struct_Absyn_Fndecl_rep)};
struct _tuple61{unsigned int f1;struct Cyc_Absyn_Fndecl*f2;};static struct _tuple6 Cyc__gentuple_838={
offsetof(struct _tuple61,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_839={
offsetof(struct _tuple61,f2),(void*)& Cyc__genrep_809};static struct _tuple6*Cyc__genarr_840[
2]={& Cyc__gentuple_838,& Cyc__gentuple_839};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_808={4,sizeof(struct _tuple61),{(void*)((struct _tuple6**)Cyc__genarr_840),(
void*)((struct _tuple6**)Cyc__genarr_840),(void*)((struct _tuple6**)Cyc__genarr_840
+ 2)}};static char _tmp373[13]="Unresolved_b";static struct _tuple7 Cyc__gentuple_806={
0,(struct _tagged_arr){_tmp373,_tmp373,_tmp373 + 13}};static struct _tuple7*Cyc__genarr_807[
1]={& Cyc__gentuple_806};static char _tmp374[9]="Global_b";static struct _tuple5 Cyc__gentuple_841={
0,(struct _tagged_arr){_tmp374,_tmp374,_tmp374 + 9},(void*)& Cyc__genrep_443};
static char _tmp375[10]="Funname_b";static struct _tuple5 Cyc__gentuple_842={1,(
struct _tagged_arr){_tmp375,_tmp375,_tmp375 + 10},(void*)& Cyc__genrep_808};static
char _tmp376[8]="Param_b";static struct _tuple5 Cyc__gentuple_843={2,(struct
_tagged_arr){_tmp376,_tmp376,_tmp376 + 8},(void*)& Cyc__genrep_443};static char
_tmp377[8]="Local_b";static struct _tuple5 Cyc__gentuple_844={3,(struct _tagged_arr){
_tmp377,_tmp377,_tmp377 + 8},(void*)& Cyc__genrep_443};static char _tmp378[6]="Pat_b";
static struct _tuple5 Cyc__gentuple_845={4,(struct _tagged_arr){_tmp378,_tmp378,
_tmp378 + 6},(void*)& Cyc__genrep_443};static struct _tuple5*Cyc__genarr_846[5]={&
Cyc__gentuple_841,& Cyc__gentuple_842,& Cyc__gentuple_843,& Cyc__gentuple_844,& Cyc__gentuple_845};
static char _tmp37A[8]="Binding";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_binding_t_rep={
5,(struct _tagged_arr){_tmp37A,_tmp37A,_tmp37A + 8},{(void*)((struct _tuple7**)Cyc__genarr_807),(
void*)((struct _tuple7**)Cyc__genarr_807),(void*)((struct _tuple7**)Cyc__genarr_807
+ 1)},{(void*)((struct _tuple5**)Cyc__genarr_846),(void*)((struct _tuple5**)Cyc__genarr_846),(
void*)((struct _tuple5**)Cyc__genarr_846 + 5)}};struct _tuple62{unsigned int f1;
struct _tuple0*f2;void*f3;};static struct _tuple6 Cyc__gentuple_847={offsetof(struct
_tuple62,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_848={
offsetof(struct _tuple62,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_849={
offsetof(struct _tuple62,f3),(void*)& Cyc_Absyn_binding_t_rep};static struct _tuple6*
Cyc__genarr_850[3]={& Cyc__gentuple_847,& Cyc__gentuple_848,& Cyc__gentuple_849};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_805={4,sizeof(struct _tuple62),{(
void*)((struct _tuple6**)Cyc__genarr_850),(void*)((struct _tuple6**)Cyc__genarr_850),(
void*)((struct _tuple6**)Cyc__genarr_850 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_800;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep;
static char _tmp37C[5]="Plus";static struct _tuple7 Cyc__gentuple_770={0,(struct
_tagged_arr){_tmp37C,_tmp37C,_tmp37C + 5}};static char _tmp37D[6]="Times";static
struct _tuple7 Cyc__gentuple_771={1,(struct _tagged_arr){_tmp37D,_tmp37D,_tmp37D + 6}};
static char _tmp37E[6]="Minus";static struct _tuple7 Cyc__gentuple_772={2,(struct
_tagged_arr){_tmp37E,_tmp37E,_tmp37E + 6}};static char _tmp37F[4]="Div";static
struct _tuple7 Cyc__gentuple_773={3,(struct _tagged_arr){_tmp37F,_tmp37F,_tmp37F + 4}};
static char _tmp380[4]="Mod";static struct _tuple7 Cyc__gentuple_774={4,(struct
_tagged_arr){_tmp380,_tmp380,_tmp380 + 4}};static char _tmp381[3]="Eq";static struct
_tuple7 Cyc__gentuple_775={5,(struct _tagged_arr){_tmp381,_tmp381,_tmp381 + 3}};
static char _tmp382[4]="Neq";static struct _tuple7 Cyc__gentuple_776={6,(struct
_tagged_arr){_tmp382,_tmp382,_tmp382 + 4}};static char _tmp383[3]="Gt";static struct
_tuple7 Cyc__gentuple_777={7,(struct _tagged_arr){_tmp383,_tmp383,_tmp383 + 3}};
static char _tmp384[3]="Lt";static struct _tuple7 Cyc__gentuple_778={8,(struct
_tagged_arr){_tmp384,_tmp384,_tmp384 + 3}};static char _tmp385[4]="Gte";static
struct _tuple7 Cyc__gentuple_779={9,(struct _tagged_arr){_tmp385,_tmp385,_tmp385 + 4}};
static char _tmp386[4]="Lte";static struct _tuple7 Cyc__gentuple_780={10,(struct
_tagged_arr){_tmp386,_tmp386,_tmp386 + 4}};static char _tmp387[4]="Not";static
struct _tuple7 Cyc__gentuple_781={11,(struct _tagged_arr){_tmp387,_tmp387,_tmp387 + 
4}};static char _tmp388[7]="Bitnot";static struct _tuple7 Cyc__gentuple_782={12,(
struct _tagged_arr){_tmp388,_tmp388,_tmp388 + 7}};static char _tmp389[7]="Bitand";
static struct _tuple7 Cyc__gentuple_783={13,(struct _tagged_arr){_tmp389,_tmp389,
_tmp389 + 7}};static char _tmp38A[6]="Bitor";static struct _tuple7 Cyc__gentuple_784={
14,(struct _tagged_arr){_tmp38A,_tmp38A,_tmp38A + 6}};static char _tmp38B[7]="Bitxor";
static struct _tuple7 Cyc__gentuple_785={15,(struct _tagged_arr){_tmp38B,_tmp38B,
_tmp38B + 7}};static char _tmp38C[10]="Bitlshift";static struct _tuple7 Cyc__gentuple_786={
16,(struct _tagged_arr){_tmp38C,_tmp38C,_tmp38C + 10}};static char _tmp38D[11]="Bitlrshift";
static struct _tuple7 Cyc__gentuple_787={17,(struct _tagged_arr){_tmp38D,_tmp38D,
_tmp38D + 11}};static char _tmp38E[11]="Bitarshift";static struct _tuple7 Cyc__gentuple_788={
18,(struct _tagged_arr){_tmp38E,_tmp38E,_tmp38E + 11}};static char _tmp38F[5]="Size";
static struct _tuple7 Cyc__gentuple_789={19,(struct _tagged_arr){_tmp38F,_tmp38F,
_tmp38F + 5}};static struct _tuple7*Cyc__genarr_790[20]={& Cyc__gentuple_770,& Cyc__gentuple_771,&
Cyc__gentuple_772,& Cyc__gentuple_773,& Cyc__gentuple_774,& Cyc__gentuple_775,& Cyc__gentuple_776,&
Cyc__gentuple_777,& Cyc__gentuple_778,& Cyc__gentuple_779,& Cyc__gentuple_780,& Cyc__gentuple_781,&
Cyc__gentuple_782,& Cyc__gentuple_783,& Cyc__gentuple_784,& Cyc__gentuple_785,& Cyc__gentuple_786,&
Cyc__gentuple_787,& Cyc__gentuple_788,& Cyc__gentuple_789};static struct _tuple5*Cyc__genarr_791[
0]={};static char _tmp391[7]="Primop";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep={
5,(struct _tagged_arr){_tmp391,_tmp391,_tmp391 + 7},{(void*)((struct _tuple7**)Cyc__genarr_790),(
void*)((struct _tuple7**)Cyc__genarr_790),(void*)((struct _tuple7**)Cyc__genarr_790
+ 20)},{(void*)((struct _tuple5**)Cyc__genarr_791),(void*)((struct _tuple5**)Cyc__genarr_791),(
void*)((struct _tuple5**)Cyc__genarr_791 + 0)}};struct _tuple63{unsigned int f1;void*
f2;struct Cyc_List_List*f3;};static struct _tuple6 Cyc__gentuple_801={offsetof(
struct _tuple63,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_802={
offsetof(struct _tuple63,f2),(void*)& Cyc_Absyn_primop_t_rep};static struct _tuple6
Cyc__gentuple_803={offsetof(struct _tuple63,f3),(void*)& Cyc__genrep_504};static
struct _tuple6*Cyc__genarr_804[3]={& Cyc__gentuple_801,& Cyc__gentuple_802,& Cyc__gentuple_803};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_800={4,sizeof(struct _tuple63),{(
void*)((struct _tuple6**)Cyc__genarr_804),(void*)((struct _tuple6**)Cyc__genarr_804),(
void*)((struct _tuple6**)Cyc__genarr_804 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_768;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_769;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep;static
char _tmp393[4]="Opt";static struct _tagged_arr Cyc__genname_794=(struct _tagged_arr){
_tmp393,_tmp393,_tmp393 + 4};static char _tmp394[2]="v";static struct _tuple5 Cyc__gentuple_792={
offsetof(struct Cyc_Core_Opt,v),(struct _tagged_arr){_tmp394,_tmp394,_tmp394 + 2},(
void*)& Cyc_Absyn_primop_t_rep};static struct _tuple5*Cyc__genarr_793[1]={& Cyc__gentuple_792};
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep={3,(
struct _tagged_arr*)& Cyc__genname_794,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_793),(void*)((struct _tuple5**)Cyc__genarr_793),(void*)((
struct _tuple5**)Cyc__genarr_793 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_769={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_primop_t2_rep)};struct _tuple64{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_795={offsetof(struct _tuple64,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_796={offsetof(struct _tuple64,f2),(
void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_797={offsetof(struct
_tuple64,f3),(void*)& Cyc__genrep_769};static struct _tuple6 Cyc__gentuple_798={
offsetof(struct _tuple64,f4),(void*)& Cyc__genrep_154};static struct _tuple6*Cyc__genarr_799[
4]={& Cyc__gentuple_795,& Cyc__gentuple_796,& Cyc__gentuple_797,& Cyc__gentuple_798};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_768={4,sizeof(struct _tuple64),{(
void*)((struct _tuple6**)Cyc__genarr_799),(void*)((struct _tuple6**)Cyc__genarr_799),(
void*)((struct _tuple6**)Cyc__genarr_799 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_757;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep;
static char _tmp398[7]="PreInc";static struct _tuple7 Cyc__gentuple_758={0,(struct
_tagged_arr){_tmp398,_tmp398,_tmp398 + 7}};static char _tmp399[8]="PostInc";static
struct _tuple7 Cyc__gentuple_759={1,(struct _tagged_arr){_tmp399,_tmp399,_tmp399 + 8}};
static char _tmp39A[7]="PreDec";static struct _tuple7 Cyc__gentuple_760={2,(struct
_tagged_arr){_tmp39A,_tmp39A,_tmp39A + 7}};static char _tmp39B[8]="PostDec";static
struct _tuple7 Cyc__gentuple_761={3,(struct _tagged_arr){_tmp39B,_tmp39B,_tmp39B + 8}};
static struct _tuple7*Cyc__genarr_762[4]={& Cyc__gentuple_758,& Cyc__gentuple_759,&
Cyc__gentuple_760,& Cyc__gentuple_761};static struct _tuple5*Cyc__genarr_763[0]={};
static char _tmp39D[12]="Incrementor";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep={
5,(struct _tagged_arr){_tmp39D,_tmp39D,_tmp39D + 12},{(void*)((struct _tuple7**)Cyc__genarr_762),(
void*)((struct _tuple7**)Cyc__genarr_762),(void*)((struct _tuple7**)Cyc__genarr_762
+ 4)},{(void*)((struct _tuple5**)Cyc__genarr_763),(void*)((struct _tuple5**)Cyc__genarr_763),(
void*)((struct _tuple5**)Cyc__genarr_763 + 0)}};struct _tuple65{unsigned int f1;
struct Cyc_Absyn_Exp*f2;void*f3;};static struct _tuple6 Cyc__gentuple_764={offsetof(
struct _tuple65,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_765={
offsetof(struct _tuple65,f2),(void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_766={
offsetof(struct _tuple65,f3),(void*)& Cyc_Absyn_incrementor_t_rep};static struct
_tuple6*Cyc__genarr_767[3]={& Cyc__gentuple_764,& Cyc__gentuple_765,& Cyc__gentuple_766};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_757={4,sizeof(struct _tuple65),{(
void*)((struct _tuple6**)Cyc__genarr_767),(void*)((struct _tuple6**)Cyc__genarr_767),(
void*)((struct _tuple6**)Cyc__genarr_767 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_751;struct _tuple66{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*
f3;struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_752={offsetof(
struct _tuple66,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_753={
offsetof(struct _tuple66,f2),(void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_754={
offsetof(struct _tuple66,f3),(void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_755={
offsetof(struct _tuple66,f4),(void*)& Cyc__genrep_154};static struct _tuple6*Cyc__genarr_756[
4]={& Cyc__gentuple_752,& Cyc__gentuple_753,& Cyc__gentuple_754,& Cyc__gentuple_755};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_751={4,sizeof(struct _tuple66),{(
void*)((struct _tuple6**)Cyc__genarr_756),(void*)((struct _tuple6**)Cyc__genarr_756),(
void*)((struct _tuple6**)Cyc__genarr_756 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_676;struct _tuple67{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*
f3;};static struct _tuple6 Cyc__gentuple_677={offsetof(struct _tuple67,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_678={offsetof(struct _tuple67,f2),(
void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_679={offsetof(struct
_tuple67,f3),(void*)& Cyc__genrep_154};static struct _tuple6*Cyc__genarr_680[3]={&
Cyc__gentuple_677,& Cyc__gentuple_678,& Cyc__gentuple_679};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_676={4,sizeof(struct _tuple67),{(void*)((struct _tuple6**)Cyc__genarr_680),(
void*)((struct _tuple6**)Cyc__genarr_680),(void*)((struct _tuple6**)Cyc__genarr_680
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_746;static struct _tuple6
Cyc__gentuple_747={offsetof(struct _tuple55,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_748={offsetof(struct _tuple55,f2),(void*)& Cyc__genrep_154};
static struct _tuple6 Cyc__gentuple_749={offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_504};
static struct _tuple6*Cyc__genarr_750[3]={& Cyc__gentuple_747,& Cyc__gentuple_748,&
Cyc__gentuple_749};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_746={4,
sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_750),(void*)((
struct _tuple6**)Cyc__genarr_750),(void*)((struct _tuple6**)Cyc__genarr_750 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_727;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_728;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_729;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_729={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};static char
_tmp3A3[15]="VarargCallInfo";static struct _tagged_arr Cyc__genname_740=(struct
_tagged_arr){_tmp3A3,_tmp3A3,_tmp3A3 + 15};static char _tmp3A4[12]="num_varargs";
static struct _tuple5 Cyc__gentuple_736={offsetof(struct Cyc_Absyn_VarargCallInfo,num_varargs),(
struct _tagged_arr){_tmp3A4,_tmp3A4,_tmp3A4 + 12},(void*)((void*)& Cyc__genrep_10)};
static char _tmp3A5[10]="injectors";static struct _tuple5 Cyc__gentuple_737={
offsetof(struct Cyc_Absyn_VarargCallInfo,injectors),(struct _tagged_arr){_tmp3A5,
_tmp3A5,_tmp3A5 + 10},(void*)& Cyc__genrep_313};static char _tmp3A6[4]="vai";static
struct _tuple5 Cyc__gentuple_738={offsetof(struct Cyc_Absyn_VarargCallInfo,vai),(
struct _tagged_arr){_tmp3A6,_tmp3A6,_tmp3A6 + 4},(void*)& Cyc__genrep_729};static
struct _tuple5*Cyc__genarr_739[3]={& Cyc__gentuple_736,& Cyc__gentuple_737,& Cyc__gentuple_738};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep={3,(struct
_tagged_arr*)& Cyc__genname_740,sizeof(struct Cyc_Absyn_VarargCallInfo),{(void*)((
struct _tuple5**)Cyc__genarr_739),(void*)((struct _tuple5**)Cyc__genarr_739),(void*)((
struct _tuple5**)Cyc__genarr_739 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_728={
1,1,(void*)((void*)& Cyc_Absyn_vararg_call_info_t_rep)};struct _tuple68{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_VarargCallInfo*
f4;};static struct _tuple6 Cyc__gentuple_741={offsetof(struct _tuple68,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_742={offsetof(struct _tuple68,f2),(
void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_743={offsetof(struct
_tuple68,f3),(void*)& Cyc__genrep_504};static struct _tuple6 Cyc__gentuple_744={
offsetof(struct _tuple68,f4),(void*)& Cyc__genrep_728};static struct _tuple6*Cyc__genarr_745[
4]={& Cyc__gentuple_741,& Cyc__gentuple_742,& Cyc__gentuple_743,& Cyc__gentuple_744};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_727={4,sizeof(struct _tuple68),{(
void*)((struct _tuple6**)Cyc__genarr_745),(void*)((struct _tuple6**)Cyc__genarr_745),(
void*)((struct _tuple6**)Cyc__genarr_745 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_722;static struct _tuple6 Cyc__gentuple_723={offsetof(struct _tuple55,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_724={offsetof(struct
_tuple55,f2),(void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_725={
offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_121};static struct _tuple6*Cyc__genarr_726[
3]={& Cyc__gentuple_723,& Cyc__gentuple_724,& Cyc__gentuple_725};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_722={4,sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_726),(
void*)((struct _tuple6**)Cyc__genarr_726),(void*)((struct _tuple6**)Cyc__genarr_726
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_709;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_coercion_t_rep;static char _tmp3AB[17]="Unknown_coercion";static struct
_tuple7 Cyc__gentuple_710={0,(struct _tagged_arr){_tmp3AB,_tmp3AB,_tmp3AB + 17}};
static char _tmp3AC[12]="No_coercion";static struct _tuple7 Cyc__gentuple_711={1,(
struct _tagged_arr){_tmp3AC,_tmp3AC,_tmp3AC + 12}};static char _tmp3AD[16]="NonNull_to_Null";
static struct _tuple7 Cyc__gentuple_712={2,(struct _tagged_arr){_tmp3AD,_tmp3AD,
_tmp3AD + 16}};static char _tmp3AE[15]="Other_coercion";static struct _tuple7 Cyc__gentuple_713={
3,(struct _tagged_arr){_tmp3AE,_tmp3AE,_tmp3AE + 15}};static struct _tuple7*Cyc__genarr_714[
4]={& Cyc__gentuple_710,& Cyc__gentuple_711,& Cyc__gentuple_712,& Cyc__gentuple_713};
static struct _tuple5*Cyc__genarr_715[0]={};static char _tmp3B0[9]="Coercion";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_coercion_t_rep={5,(struct _tagged_arr){_tmp3B0,
_tmp3B0,_tmp3B0 + 9},{(void*)((struct _tuple7**)Cyc__genarr_714),(void*)((struct
_tuple7**)Cyc__genarr_714),(void*)((struct _tuple7**)Cyc__genarr_714 + 4)},{(void*)((
struct _tuple5**)Cyc__genarr_715),(void*)((struct _tuple5**)Cyc__genarr_715),(void*)((
struct _tuple5**)Cyc__genarr_715 + 0)}};struct _tuple69{unsigned int f1;void*f2;
struct Cyc_Absyn_Exp*f3;int f4;void*f5;};static struct _tuple6 Cyc__gentuple_716={
offsetof(struct _tuple69,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_717={
offsetof(struct _tuple69,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_718={offsetof(struct _tuple69,f3),(void*)& Cyc__genrep_154};
static struct _tuple6 Cyc__gentuple_719={offsetof(struct _tuple69,f4),(void*)((void*)&
Cyc__genrep_10)};static struct _tuple6 Cyc__gentuple_720={offsetof(struct _tuple69,f5),(
void*)& Cyc_Absyn_coercion_t_rep};static struct _tuple6*Cyc__genarr_721[5]={& Cyc__gentuple_716,&
Cyc__gentuple_717,& Cyc__gentuple_718,& Cyc__gentuple_719,& Cyc__gentuple_720};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_709={4,sizeof(struct _tuple69),{(
void*)((struct _tuple6**)Cyc__genarr_721),(void*)((struct _tuple6**)Cyc__genarr_721),(
void*)((struct _tuple6**)Cyc__genarr_721 + 5)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_704;static struct _tuple6 Cyc__gentuple_705={offsetof(struct _tuple67,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_706={offsetof(struct
_tuple67,f2),(void*)& Cyc__genrep_141};static struct _tuple6 Cyc__gentuple_707={
offsetof(struct _tuple67,f3),(void*)& Cyc__genrep_154};static struct _tuple6*Cyc__genarr_708[
3]={& Cyc__gentuple_705,& Cyc__gentuple_706,& Cyc__gentuple_707};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_704={4,sizeof(struct _tuple67),{(void*)((struct _tuple6**)Cyc__genarr_708),(
void*)((struct _tuple6**)Cyc__genarr_708),(void*)((struct _tuple6**)Cyc__genarr_708
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_116;static struct _tuple6
Cyc__gentuple_117={offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_118={offsetof(struct _tuple6,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple6*Cyc__genarr_119[2]={& Cyc__gentuple_117,& Cyc__gentuple_118};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_116={4,sizeof(struct _tuple6),{(
void*)((struct _tuple6**)Cyc__genarr_119),(void*)((struct _tuple6**)Cyc__genarr_119),(
void*)((struct _tuple6**)Cyc__genarr_119 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_691;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_offsetof_field_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_693;struct _tuple70{unsigned int
f1;unsigned int f2;};static struct _tuple6 Cyc__gentuple_694={offsetof(struct
_tuple70,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_695={
offsetof(struct _tuple70,f2),(void*)& Cyc__genrep_24};static struct _tuple6*Cyc__genarr_696[
2]={& Cyc__gentuple_694,& Cyc__gentuple_695};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_693={4,sizeof(struct _tuple70),{(void*)((struct _tuple6**)Cyc__genarr_696),(
void*)((struct _tuple6**)Cyc__genarr_696),(void*)((struct _tuple6**)Cyc__genarr_696
+ 2)}};static struct _tuple7*Cyc__genarr_692[0]={};static char _tmp3B5[12]="StructField";
static struct _tuple5 Cyc__gentuple_697={0,(struct _tagged_arr){_tmp3B5,_tmp3B5,
_tmp3B5 + 12},(void*)& Cyc__genrep_345};static char _tmp3B6[11]="TupleIndex";static
struct _tuple5 Cyc__gentuple_698={1,(struct _tagged_arr){_tmp3B6,_tmp3B6,_tmp3B6 + 
11},(void*)& Cyc__genrep_693};static struct _tuple5*Cyc__genarr_699[2]={& Cyc__gentuple_697,&
Cyc__gentuple_698};static char _tmp3B8[14]="OffsetofField";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_offsetof_field_t_rep={5,(struct _tagged_arr){_tmp3B8,_tmp3B8,_tmp3B8 + 
14},{(void*)((struct _tuple7**)Cyc__genarr_692),(void*)((struct _tuple7**)Cyc__genarr_692),(
void*)((struct _tuple7**)Cyc__genarr_692 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_699),(
void*)((struct _tuple5**)Cyc__genarr_699),(void*)((struct _tuple5**)Cyc__genarr_699
+ 2)}};static struct _tuple6 Cyc__gentuple_700={offsetof(struct _tuple27,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_701={offsetof(struct _tuple27,f2),(
void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_702={
offsetof(struct _tuple27,f3),(void*)& Cyc_Absyn_offsetof_field_t_rep};static struct
_tuple6*Cyc__genarr_703[3]={& Cyc__gentuple_700,& Cyc__gentuple_701,& Cyc__gentuple_702};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_691={4,sizeof(struct _tuple27),{(
void*)((struct _tuple6**)Cyc__genarr_703),(void*)((struct _tuple6**)Cyc__genarr_703),(
void*)((struct _tuple6**)Cyc__genarr_703 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_686;struct _tuple71{unsigned int f1;struct Cyc_List_List*f2;void*f3;};
static struct _tuple6 Cyc__gentuple_687={offsetof(struct _tuple71,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_688={offsetof(struct _tuple71,f2),(void*)& Cyc__genrep_321};
static struct _tuple6 Cyc__gentuple_689={offsetof(struct _tuple71,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_690[3]={& Cyc__gentuple_687,&
Cyc__gentuple_688,& Cyc__gentuple_689};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_686={
4,sizeof(struct _tuple71),{(void*)((struct _tuple6**)Cyc__genarr_690),(void*)((
struct _tuple6**)Cyc__genarr_690),(void*)((struct _tuple6**)Cyc__genarr_690 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_681;struct _tuple72{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct _tagged_arr*f3;};static struct _tuple6 Cyc__gentuple_682={
offsetof(struct _tuple72,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_683={
offsetof(struct _tuple72,f2),(void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_684={
offsetof(struct _tuple72,f3),(void*)& Cyc__genrep_13};static struct _tuple6*Cyc__genarr_685[
3]={& Cyc__gentuple_682,& Cyc__gentuple_683,& Cyc__gentuple_684};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_681={4,sizeof(struct _tuple72),{(void*)((struct _tuple6**)Cyc__genarr_685),(
void*)((struct _tuple6**)Cyc__genarr_685),(void*)((struct _tuple6**)Cyc__genarr_685
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_672;static struct _tuple6
Cyc__gentuple_673={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_674={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_504};
static struct _tuple6*Cyc__genarr_675[2]={& Cyc__gentuple_673,& Cyc__gentuple_674};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_672={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_675),(void*)((struct _tuple6**)Cyc__genarr_675),(
void*)((struct _tuple6**)Cyc__genarr_675 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_657;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_658;extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_659;static struct _tuple6 Cyc__gentuple_664={
offsetof(struct _tuple1,f1),(void*)& Cyc__genrep_660};static struct _tuple6 Cyc__gentuple_665={
offsetof(struct _tuple1,f2),(void*)& Cyc_Absyn_tqual_t_rep};static struct _tuple6 Cyc__gentuple_666={
offsetof(struct _tuple1,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_667[3]={& Cyc__gentuple_664,& Cyc__gentuple_665,& Cyc__gentuple_666};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_659={4,sizeof(struct _tuple1),{(
void*)((struct _tuple6**)Cyc__genarr_667),(void*)((struct _tuple6**)Cyc__genarr_667),(
void*)((struct _tuple6**)Cyc__genarr_667 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_658={1,1,(void*)((void*)& Cyc__genrep_659)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_584;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_585;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_586;static struct _tuple6 Cyc__gentuple_587={offsetof(struct _tuple12,f1),(
void*)& Cyc__genrep_343};static struct _tuple6 Cyc__gentuple_588={offsetof(struct
_tuple12,f2),(void*)& Cyc__genrep_154};static struct _tuple6*Cyc__genarr_589[2]={&
Cyc__gentuple_587,& Cyc__gentuple_588};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_586={
4,sizeof(struct _tuple12),{(void*)((struct _tuple6**)Cyc__genarr_589),(void*)((
struct _tuple6**)Cyc__genarr_589),(void*)((struct _tuple6**)Cyc__genarr_589 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_585={1,1,(void*)((void*)& Cyc__genrep_586)};
static char _tmp3C1[5]="List";static struct _tagged_arr Cyc__genname_593=(struct
_tagged_arr){_tmp3C1,_tmp3C1,_tmp3C1 + 5};static char _tmp3C2[3]="hd";static struct
_tuple5 Cyc__gentuple_590={offsetof(struct Cyc_List_List,hd),(struct _tagged_arr){
_tmp3C2,_tmp3C2,_tmp3C2 + 3},(void*)& Cyc__genrep_585};static char _tmp3C3[3]="tl";
static struct _tuple5 Cyc__gentuple_591={offsetof(struct Cyc_List_List,tl),(struct
_tagged_arr){_tmp3C3,_tmp3C3,_tmp3C3 + 3},(void*)& Cyc__genrep_584};static struct
_tuple5*Cyc__genarr_592[2]={& Cyc__gentuple_590,& Cyc__gentuple_591};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_593,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_592),(void*)((struct _tuple5**)Cyc__genarr_592),(void*)((
struct _tuple5**)Cyc__genarr_592 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_584={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep)};
struct _tuple73{unsigned int f1;struct _tuple1*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_668={offsetof(struct _tuple73,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_669={offsetof(struct _tuple73,f2),(void*)& Cyc__genrep_658};
static struct _tuple6 Cyc__gentuple_670={offsetof(struct _tuple73,f3),(void*)& Cyc__genrep_584};
static struct _tuple6*Cyc__genarr_671[3]={& Cyc__gentuple_668,& Cyc__gentuple_669,&
Cyc__gentuple_670};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_657={4,
sizeof(struct _tuple73),{(void*)((struct _tuple6**)Cyc__genarr_671),(void*)((
struct _tuple6**)Cyc__genarr_671),(void*)((struct _tuple6**)Cyc__genarr_671 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_653;static struct _tuple6 Cyc__gentuple_654={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_655={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_584};static struct _tuple6*Cyc__genarr_656[
2]={& Cyc__gentuple_654,& Cyc__gentuple_655};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_653={4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_656),(
void*)((struct _tuple6**)Cyc__genarr_656),(void*)((struct _tuple6**)Cyc__genarr_656
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_646;struct _tuple74{
unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;int f5;};static struct _tuple6 Cyc__gentuple_647={offsetof(struct _tuple74,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_648={offsetof(struct
_tuple74,f2),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_649={
offsetof(struct _tuple74,f3),(void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_650={
offsetof(struct _tuple74,f4),(void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_651={
offsetof(struct _tuple74,f5),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6*
Cyc__genarr_652[5]={& Cyc__gentuple_647,& Cyc__gentuple_648,& Cyc__gentuple_649,&
Cyc__gentuple_650,& Cyc__gentuple_651};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_646={
4,sizeof(struct _tuple74),{(void*)((struct _tuple6**)Cyc__genarr_652),(void*)((
struct _tuple6**)Cyc__genarr_652),(void*)((struct _tuple6**)Cyc__genarr_652 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_638;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_639;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_639={1,1,(void*)((
void*)& Cyc_struct_Absyn_Aggrdecl_rep)};struct _tuple75{unsigned int f1;struct
_tuple0*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_Absyn_Aggrdecl*
f5;};static struct _tuple6 Cyc__gentuple_640={offsetof(struct _tuple75,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_641={offsetof(struct _tuple75,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_642={offsetof(struct
_tuple75,f3),(void*)& Cyc__genrep_121};static struct _tuple6 Cyc__gentuple_643={
offsetof(struct _tuple75,f4),(void*)& Cyc__genrep_584};static struct _tuple6 Cyc__gentuple_644={
offsetof(struct _tuple75,f5),(void*)& Cyc__genrep_639};static struct _tuple6*Cyc__genarr_645[
5]={& Cyc__gentuple_640,& Cyc__gentuple_641,& Cyc__gentuple_642,& Cyc__gentuple_643,&
Cyc__gentuple_644};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_638={4,
sizeof(struct _tuple75),{(void*)((struct _tuple6**)Cyc__genarr_645),(void*)((
struct _tuple6**)Cyc__genarr_645),(void*)((struct _tuple6**)Cyc__genarr_645 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_633;static struct _tuple6 Cyc__gentuple_634={
offsetof(struct _tuple63,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_635={
offsetof(struct _tuple63,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_636={offsetof(struct _tuple63,f3),(void*)& Cyc__genrep_584};
static struct _tuple6*Cyc__genarr_637[3]={& Cyc__gentuple_634,& Cyc__gentuple_635,&
Cyc__gentuple_636};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_633={4,
sizeof(struct _tuple63),{(void*)((struct _tuple6**)Cyc__genarr_637),(void*)((
struct _tuple6**)Cyc__genarr_637),(void*)((struct _tuple6**)Cyc__genarr_637 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_627;struct _tuple76{unsigned int
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Tuniondecl*f3;struct Cyc_Absyn_Tunionfield*
f4;};static struct _tuple6 Cyc__gentuple_628={offsetof(struct _tuple76,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_629={offsetof(struct _tuple76,f2),(
void*)& Cyc__genrep_504};static struct _tuple6 Cyc__gentuple_630={offsetof(struct
_tuple76,f3),(void*)((void*)& Cyc__genrep_311)};static struct _tuple6 Cyc__gentuple_631={
offsetof(struct _tuple76,f4),(void*)& Cyc__genrep_294};static struct _tuple6*Cyc__genarr_632[
4]={& Cyc__gentuple_628,& Cyc__gentuple_629,& Cyc__gentuple_630,& Cyc__gentuple_631};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_627={4,sizeof(struct _tuple76),{(
void*)((struct _tuple6**)Cyc__genarr_632),(void*)((struct _tuple6**)Cyc__genarr_632),(
void*)((struct _tuple6**)Cyc__genarr_632 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_620;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_621;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_621={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_614;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_614={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};struct
_tuple77{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_622={offsetof(struct _tuple77,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_623={offsetof(struct _tuple77,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_624={offsetof(struct
_tuple77,f3),(void*)& Cyc__genrep_621};static struct _tuple6 Cyc__gentuple_625={
offsetof(struct _tuple77,f4),(void*)& Cyc__genrep_614};static struct _tuple6*Cyc__genarr_626[
4]={& Cyc__gentuple_622,& Cyc__gentuple_623,& Cyc__gentuple_624,& Cyc__gentuple_625};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_620={4,sizeof(struct _tuple77),{(
void*)((struct _tuple6**)Cyc__genarr_626),(void*)((struct _tuple6**)Cyc__genarr_626),(
void*)((struct _tuple6**)Cyc__genarr_626 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_613;struct _tuple78{unsigned int f1;struct _tuple0*f2;void*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_615={offsetof(struct _tuple78,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_616={offsetof(struct _tuple78,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_617={offsetof(struct
_tuple78,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_618={
offsetof(struct _tuple78,f4),(void*)& Cyc__genrep_614};static struct _tuple6*Cyc__genarr_619[
4]={& Cyc__gentuple_615,& Cyc__gentuple_616,& Cyc__gentuple_617,& Cyc__gentuple_618};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_613={4,sizeof(struct _tuple78),{(
void*)((struct _tuple6**)Cyc__genarr_619),(void*)((struct _tuple6**)Cyc__genarr_619),(
void*)((struct _tuple6**)Cyc__genarr_619 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_602;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_130={1,1,(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp3D2[11]="MallocInfo";
static struct _tagged_arr Cyc__genname_609=(struct _tagged_arr){_tmp3D2,_tmp3D2,
_tmp3D2 + 11};static char _tmp3D3[10]="is_calloc";static struct _tuple5 Cyc__gentuple_603={
offsetof(struct Cyc_Absyn_MallocInfo,is_calloc),(struct _tagged_arr){_tmp3D3,
_tmp3D3,_tmp3D3 + 10},(void*)((void*)& Cyc__genrep_10)};static char _tmp3D4[4]="rgn";
static struct _tuple5 Cyc__gentuple_604={offsetof(struct Cyc_Absyn_MallocInfo,rgn),(
struct _tagged_arr){_tmp3D4,_tmp3D4,_tmp3D4 + 4},(void*)& Cyc__genrep_141};static
char _tmp3D5[9]="elt_type";static struct _tuple5 Cyc__gentuple_605={offsetof(struct
Cyc_Absyn_MallocInfo,elt_type),(struct _tagged_arr){_tmp3D5,_tmp3D5,_tmp3D5 + 9},(
void*)& Cyc__genrep_130};static char _tmp3D6[9]="num_elts";static struct _tuple5 Cyc__gentuple_606={
offsetof(struct Cyc_Absyn_MallocInfo,num_elts),(struct _tagged_arr){_tmp3D6,
_tmp3D6,_tmp3D6 + 9},(void*)& Cyc__genrep_154};static char _tmp3D7[11]="fat_result";
static struct _tuple5 Cyc__gentuple_607={offsetof(struct Cyc_Absyn_MallocInfo,fat_result),(
struct _tagged_arr){_tmp3D7,_tmp3D7,_tmp3D7 + 11},(void*)((void*)& Cyc__genrep_10)};
static struct _tuple5*Cyc__genarr_608[5]={& Cyc__gentuple_603,& Cyc__gentuple_604,&
Cyc__gentuple_605,& Cyc__gentuple_606,& Cyc__gentuple_607};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_malloc_info_t_rep={3,(struct _tagged_arr*)& Cyc__genname_609,sizeof(
struct Cyc_Absyn_MallocInfo),{(void*)((struct _tuple5**)Cyc__genarr_608),(void*)((
struct _tuple5**)Cyc__genarr_608),(void*)((struct _tuple5**)Cyc__genarr_608 + 5)}};
struct _tuple79{unsigned int f1;struct Cyc_Absyn_MallocInfo f2;};static struct _tuple6
Cyc__gentuple_610={offsetof(struct _tuple79,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_611={offsetof(struct _tuple79,f2),(void*)& Cyc_Absyn_malloc_info_t_rep};
static struct _tuple6*Cyc__genarr_612[2]={& Cyc__gentuple_610,& Cyc__gentuple_611};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_602={4,sizeof(struct _tuple79),{(
void*)((struct _tuple6**)Cyc__genarr_612),(void*)((struct _tuple6**)Cyc__genarr_612),(
void*)((struct _tuple6**)Cyc__genarr_612 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_583;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_594;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep;
static char _tmp3DA[4]="Opt";static struct _tagged_arr Cyc__genname_597=(struct
_tagged_arr){_tmp3DA,_tmp3DA,_tmp3DA + 4};static char _tmp3DB[2]="v";static struct
_tuple5 Cyc__gentuple_595={offsetof(struct Cyc_Core_Opt,v),(struct _tagged_arr){
_tmp3DB,_tmp3DB,_tmp3DB + 2},(void*)& Cyc__genrep_11};static struct _tuple5*Cyc__genarr_596[
1]={& Cyc__gentuple_595};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep={
3,(struct _tagged_arr*)& Cyc__genname_597,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_596),(void*)((struct _tuple5**)Cyc__genarr_596),(void*)((
struct _tuple5**)Cyc__genarr_596 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_594={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep)};struct
_tuple80{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_598={offsetof(struct _tuple80,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_599={offsetof(struct _tuple80,f2),(void*)& Cyc__genrep_594};
static struct _tuple6 Cyc__gentuple_600={offsetof(struct _tuple80,f3),(void*)& Cyc__genrep_584};
static struct _tuple6*Cyc__genarr_601[3]={& Cyc__gentuple_598,& Cyc__gentuple_599,&
Cyc__gentuple_600};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_583={4,
sizeof(struct _tuple80),{(void*)((struct _tuple6**)Cyc__genarr_601),(void*)((
struct _tuple6**)Cyc__genarr_601),(void*)((struct _tuple6**)Cyc__genarr_601 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_144;static struct _tuple6 Cyc__gentuple_580={
offsetof(struct _tuple37,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_581={
offsetof(struct _tuple37,f2),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_582[
2]={& Cyc__gentuple_580,& Cyc__gentuple_581};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_144={4,sizeof(struct _tuple37),{(void*)((struct _tuple6**)Cyc__genarr_582),(
void*)((struct _tuple6**)Cyc__genarr_582),(void*)((struct _tuple6**)Cyc__genarr_582
+ 2)}};static struct _tuple7*Cyc__genarr_143[0]={};static char _tmp3E0[8]="Const_e";
static struct _tuple5 Cyc__gentuple_881={0,(struct _tagged_arr){_tmp3E0,_tmp3E0,
_tmp3E0 + 8},(void*)& Cyc__genrep_851};static char _tmp3E1[6]="Var_e";static struct
_tuple5 Cyc__gentuple_882={1,(struct _tagged_arr){_tmp3E1,_tmp3E1,_tmp3E1 + 6},(
void*)& Cyc__genrep_805};static char _tmp3E2[12]="UnknownId_e";static struct _tuple5
Cyc__gentuple_883={2,(struct _tagged_arr){_tmp3E2,_tmp3E2,_tmp3E2 + 12},(void*)&
Cyc__genrep_255};static char _tmp3E3[9]="Primop_e";static struct _tuple5 Cyc__gentuple_884={
3,(struct _tagged_arr){_tmp3E3,_tmp3E3,_tmp3E3 + 9},(void*)& Cyc__genrep_800};
static char _tmp3E4[11]="AssignOp_e";static struct _tuple5 Cyc__gentuple_885={4,(
struct _tagged_arr){_tmp3E4,_tmp3E4,_tmp3E4 + 11},(void*)& Cyc__genrep_768};static
char _tmp3E5[12]="Increment_e";static struct _tuple5 Cyc__gentuple_886={5,(struct
_tagged_arr){_tmp3E5,_tmp3E5,_tmp3E5 + 12},(void*)& Cyc__genrep_757};static char
_tmp3E6[14]="Conditional_e";static struct _tuple5 Cyc__gentuple_887={6,(struct
_tagged_arr){_tmp3E6,_tmp3E6,_tmp3E6 + 14},(void*)& Cyc__genrep_751};static char
_tmp3E7[6]="And_e";static struct _tuple5 Cyc__gentuple_888={7,(struct _tagged_arr){
_tmp3E7,_tmp3E7,_tmp3E7 + 6},(void*)& Cyc__genrep_676};static char _tmp3E8[5]="Or_e";
static struct _tuple5 Cyc__gentuple_889={8,(struct _tagged_arr){_tmp3E8,_tmp3E8,
_tmp3E8 + 5},(void*)& Cyc__genrep_676};static char _tmp3E9[9]="SeqExp_e";static
struct _tuple5 Cyc__gentuple_890={9,(struct _tagged_arr){_tmp3E9,_tmp3E9,_tmp3E9 + 9},(
void*)& Cyc__genrep_676};static char _tmp3EA[14]="UnknownCall_e";static struct
_tuple5 Cyc__gentuple_891={10,(struct _tagged_arr){_tmp3EA,_tmp3EA,_tmp3EA + 14},(
void*)& Cyc__genrep_746};static char _tmp3EB[9]="FnCall_e";static struct _tuple5 Cyc__gentuple_892={
11,(struct _tagged_arr){_tmp3EB,_tmp3EB,_tmp3EB + 9},(void*)& Cyc__genrep_727};
static char _tmp3EC[8]="Throw_e";static struct _tuple5 Cyc__gentuple_893={12,(struct
_tagged_arr){_tmp3EC,_tmp3EC,_tmp3EC + 8},(void*)& Cyc__genrep_153};static char
_tmp3ED[16]="NoInstantiate_e";static struct _tuple5 Cyc__gentuple_894={13,(struct
_tagged_arr){_tmp3ED,_tmp3ED,_tmp3ED + 16},(void*)& Cyc__genrep_153};static char
_tmp3EE[14]="Instantiate_e";static struct _tuple5 Cyc__gentuple_895={14,(struct
_tagged_arr){_tmp3EE,_tmp3EE,_tmp3EE + 14},(void*)& Cyc__genrep_722};static char
_tmp3EF[7]="Cast_e";static struct _tuple5 Cyc__gentuple_896={15,(struct _tagged_arr){
_tmp3EF,_tmp3EF,_tmp3EF + 7},(void*)& Cyc__genrep_709};static char _tmp3F0[10]="Address_e";
static struct _tuple5 Cyc__gentuple_897={16,(struct _tagged_arr){_tmp3F0,_tmp3F0,
_tmp3F0 + 10},(void*)& Cyc__genrep_153};static char _tmp3F1[6]="New_e";static struct
_tuple5 Cyc__gentuple_898={17,(struct _tagged_arr){_tmp3F1,_tmp3F1,_tmp3F1 + 6},(
void*)& Cyc__genrep_704};static char _tmp3F2[12]="Sizeoftyp_e";static struct _tuple5
Cyc__gentuple_899={18,(struct _tagged_arr){_tmp3F2,_tmp3F2,_tmp3F2 + 12},(void*)&
Cyc__genrep_116};static char _tmp3F3[12]="Sizeofexp_e";static struct _tuple5 Cyc__gentuple_900={
19,(struct _tagged_arr){_tmp3F3,_tmp3F3,_tmp3F3 + 12},(void*)& Cyc__genrep_153};
static char _tmp3F4[11]="Offsetof_e";static struct _tuple5 Cyc__gentuple_901={20,(
struct _tagged_arr){_tmp3F4,_tmp3F4,_tmp3F4 + 11},(void*)& Cyc__genrep_691};static
char _tmp3F5[9]="Gentyp_e";static struct _tuple5 Cyc__gentuple_902={21,(struct
_tagged_arr){_tmp3F5,_tmp3F5,_tmp3F5 + 9},(void*)& Cyc__genrep_686};static char
_tmp3F6[8]="Deref_e";static struct _tuple5 Cyc__gentuple_903={22,(struct _tagged_arr){
_tmp3F6,_tmp3F6,_tmp3F6 + 8},(void*)& Cyc__genrep_153};static char _tmp3F7[13]="AggrMember_e";
static struct _tuple5 Cyc__gentuple_904={23,(struct _tagged_arr){_tmp3F7,_tmp3F7,
_tmp3F7 + 13},(void*)& Cyc__genrep_681};static char _tmp3F8[12]="AggrArrow_e";static
struct _tuple5 Cyc__gentuple_905={24,(struct _tagged_arr){_tmp3F8,_tmp3F8,_tmp3F8 + 
12},(void*)& Cyc__genrep_681};static char _tmp3F9[12]="Subscript_e";static struct
_tuple5 Cyc__gentuple_906={25,(struct _tagged_arr){_tmp3F9,_tmp3F9,_tmp3F9 + 12},(
void*)& Cyc__genrep_676};static char _tmp3FA[8]="Tuple_e";static struct _tuple5 Cyc__gentuple_907={
26,(struct _tagged_arr){_tmp3FA,_tmp3FA,_tmp3FA + 8},(void*)& Cyc__genrep_672};
static char _tmp3FB[14]="CompoundLit_e";static struct _tuple5 Cyc__gentuple_908={27,(
struct _tagged_arr){_tmp3FB,_tmp3FB,_tmp3FB + 14},(void*)& Cyc__genrep_657};static
char _tmp3FC[8]="Array_e";static struct _tuple5 Cyc__gentuple_909={28,(struct
_tagged_arr){_tmp3FC,_tmp3FC,_tmp3FC + 8},(void*)& Cyc__genrep_653};static char
_tmp3FD[16]="Comprehension_e";static struct _tuple5 Cyc__gentuple_910={29,(struct
_tagged_arr){_tmp3FD,_tmp3FD,_tmp3FD + 16},(void*)& Cyc__genrep_646};static char
_tmp3FE[9]="Struct_e";static struct _tuple5 Cyc__gentuple_911={30,(struct
_tagged_arr){_tmp3FE,_tmp3FE,_tmp3FE + 9},(void*)& Cyc__genrep_638};static char
_tmp3FF[13]="AnonStruct_e";static struct _tuple5 Cyc__gentuple_912={31,(struct
_tagged_arr){_tmp3FF,_tmp3FF,_tmp3FF + 13},(void*)& Cyc__genrep_633};static char
_tmp400[9]="Tunion_e";static struct _tuple5 Cyc__gentuple_913={32,(struct
_tagged_arr){_tmp400,_tmp400,_tmp400 + 9},(void*)& Cyc__genrep_627};static char
_tmp401[7]="Enum_e";static struct _tuple5 Cyc__gentuple_914={33,(struct _tagged_arr){
_tmp401,_tmp401,_tmp401 + 7},(void*)& Cyc__genrep_620};static char _tmp402[11]="AnonEnum_e";
static struct _tuple5 Cyc__gentuple_915={34,(struct _tagged_arr){_tmp402,_tmp402,
_tmp402 + 11},(void*)& Cyc__genrep_613};static char _tmp403[9]="Malloc_e";static
struct _tuple5 Cyc__gentuple_916={35,(struct _tagged_arr){_tmp403,_tmp403,_tmp403 + 
9},(void*)& Cyc__genrep_602};static char _tmp404[16]="UnresolvedMem_e";static struct
_tuple5 Cyc__gentuple_917={36,(struct _tagged_arr){_tmp404,_tmp404,_tmp404 + 16},(
void*)& Cyc__genrep_583};static char _tmp405[10]="StmtExp_e";static struct _tuple5 Cyc__gentuple_918={
37,(struct _tagged_arr){_tmp405,_tmp405,_tmp405 + 10},(void*)& Cyc__genrep_144};
static struct _tuple5*Cyc__genarr_919[38]={& Cyc__gentuple_881,& Cyc__gentuple_882,&
Cyc__gentuple_883,& Cyc__gentuple_884,& Cyc__gentuple_885,& Cyc__gentuple_886,& Cyc__gentuple_887,&
Cyc__gentuple_888,& Cyc__gentuple_889,& Cyc__gentuple_890,& Cyc__gentuple_891,& Cyc__gentuple_892,&
Cyc__gentuple_893,& Cyc__gentuple_894,& Cyc__gentuple_895,& Cyc__gentuple_896,& Cyc__gentuple_897,&
Cyc__gentuple_898,& Cyc__gentuple_899,& Cyc__gentuple_900,& Cyc__gentuple_901,& Cyc__gentuple_902,&
Cyc__gentuple_903,& Cyc__gentuple_904,& Cyc__gentuple_905,& Cyc__gentuple_906,& Cyc__gentuple_907,&
Cyc__gentuple_908,& Cyc__gentuple_909,& Cyc__gentuple_910,& Cyc__gentuple_911,& Cyc__gentuple_912,&
Cyc__gentuple_913,& Cyc__gentuple_914,& Cyc__gentuple_915,& Cyc__gentuple_916,& Cyc__gentuple_917,&
Cyc__gentuple_918};static char _tmp407[8]="Raw_exp";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_exp_t_rep={5,(struct _tagged_arr){_tmp407,_tmp407,_tmp407 + 8},{(
void*)((struct _tuple7**)Cyc__genarr_143),(void*)((struct _tuple7**)Cyc__genarr_143),(
void*)((struct _tuple7**)Cyc__genarr_143 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_919),(
void*)((struct _tuple5**)Cyc__genarr_919),(void*)((struct _tuple5**)Cyc__genarr_919
+ 38)}};static char _tmp408[4]="Exp";static struct _tagged_arr Cyc__genname_925=(
struct _tagged_arr){_tmp408,_tmp408,_tmp408 + 4};static char _tmp409[5]="topt";
static struct _tuple5 Cyc__gentuple_920={offsetof(struct Cyc_Absyn_Exp,topt),(struct
_tagged_arr){_tmp409,_tmp409,_tmp409 + 5},(void*)& Cyc__genrep_110};static char
_tmp40A[2]="r";static struct _tuple5 Cyc__gentuple_921={offsetof(struct Cyc_Absyn_Exp,r),(
struct _tagged_arr){_tmp40A,_tmp40A,_tmp40A + 2},(void*)& Cyc_Absyn_raw_exp_t_rep};
static char _tmp40B[4]="loc";static struct _tuple5 Cyc__gentuple_922={offsetof(struct
Cyc_Absyn_Exp,loc),(struct _tagged_arr){_tmp40B,_tmp40B,_tmp40B + 4},(void*)& Cyc__genrep_2};
static char _tmp40C[6]="annot";static struct _tuple5 Cyc__gentuple_923={offsetof(
struct Cyc_Absyn_Exp,annot),(struct _tagged_arr){_tmp40C,_tmp40C,_tmp40C + 6},(void*)&
Cyc_Absyn_absyn_annot_t_rep};static struct _tuple5*Cyc__genarr_924[4]={& Cyc__gentuple_920,&
Cyc__gentuple_921,& Cyc__gentuple_922,& Cyc__gentuple_923};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Exp_rep={3,(struct _tagged_arr*)& Cyc__genname_925,sizeof(struct
Cyc_Absyn_Exp),{(void*)((struct _tuple5**)Cyc__genarr_924),(void*)((struct _tuple5**)
Cyc__genarr_924),(void*)((struct _tuple5**)Cyc__genarr_924 + 4)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_141={1,1,(void*)((void*)& Cyc_struct_Absyn_Exp_rep)};static char
_tmp40F[10]="ArrayInfo";static struct _tagged_arr Cyc__genname_995=(struct
_tagged_arr){_tmp40F,_tmp40F,_tmp40F + 10};static char _tmp410[9]="elt_type";static
struct _tuple5 Cyc__gentuple_989={offsetof(struct Cyc_Absyn_ArrayInfo,elt_type),(
struct _tagged_arr){_tmp410,_tmp410,_tmp410 + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp411[3]="tq";static struct _tuple5 Cyc__gentuple_990={offsetof(struct
Cyc_Absyn_ArrayInfo,tq),(struct _tagged_arr){_tmp411,_tmp411,_tmp411 + 3},(void*)&
Cyc_Absyn_tqual_t_rep};static char _tmp412[9]="num_elts";static struct _tuple5 Cyc__gentuple_991={
offsetof(struct Cyc_Absyn_ArrayInfo,num_elts),(struct _tagged_arr){_tmp412,_tmp412,
_tmp412 + 9},(void*)& Cyc__genrep_141};static char _tmp413[10]="zero_term";static
struct _tuple5 Cyc__gentuple_992={offsetof(struct Cyc_Absyn_ArrayInfo,zero_term),(
struct _tagged_arr){_tmp413,_tmp413,_tmp413 + 10},(void*)& Cyc__genrep_976};static
char _tmp414[7]="zt_loc";static struct _tuple5 Cyc__gentuple_993={offsetof(struct Cyc_Absyn_ArrayInfo,zt_loc),(
struct _tagged_arr){_tmp414,_tmp414,_tmp414 + 7},(void*)& Cyc__genrep_2};static
struct _tuple5*Cyc__genarr_994[5]={& Cyc__gentuple_989,& Cyc__gentuple_990,& Cyc__gentuple_991,&
Cyc__gentuple_992,& Cyc__gentuple_993};struct Cyc_Typerep_Struct_struct Cyc_Absyn_array_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_995,sizeof(struct Cyc_Absyn_ArrayInfo),{(void*)((
struct _tuple5**)Cyc__genarr_994),(void*)((struct _tuple5**)Cyc__genarr_994),(void*)((
struct _tuple5**)Cyc__genarr_994 + 5)}};struct _tuple81{unsigned int f1;struct Cyc_Absyn_ArrayInfo
f2;};static struct _tuple6 Cyc__gentuple_996={offsetof(struct _tuple81,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_997={offsetof(struct _tuple81,f2),(
void*)& Cyc_Absyn_array_info_t_rep};static struct _tuple6*Cyc__genarr_998[2]={& Cyc__gentuple_996,&
Cyc__gentuple_997};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_975={4,
sizeof(struct _tuple81),{(void*)((struct _tuple6**)Cyc__genarr_998),(void*)((
struct _tuple6**)Cyc__genarr_998),(void*)((struct _tuple6**)Cyc__genarr_998 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_956;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_fn_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_957;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep;
static char _tmp417[5]="List";static struct _tagged_arr Cyc__genname_961=(struct
_tagged_arr){_tmp417,_tmp417,_tmp417 + 5};static char _tmp418[3]="hd";static struct
_tuple5 Cyc__gentuple_958={offsetof(struct Cyc_List_List,hd),(struct _tagged_arr){
_tmp418,_tmp418,_tmp418 + 3},(void*)& Cyc__genrep_658};static char _tmp419[3]="tl";
static struct _tuple5 Cyc__gentuple_959={offsetof(struct Cyc_List_List,tl),(struct
_tagged_arr){_tmp419,_tmp419,_tmp419 + 3},(void*)& Cyc__genrep_957};static struct
_tuple5*Cyc__genarr_960[2]={& Cyc__gentuple_958,& Cyc__gentuple_959};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_961,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_960),(void*)((struct _tuple5**)Cyc__genarr_960),(void*)((
struct _tuple5**)Cyc__genarr_960 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_957={
1,1,(void*)((void*)& Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep)};
static char _tmp41C[7]="FnInfo";static struct _tagged_arr Cyc__genname_971=(struct
_tagged_arr){_tmp41C,_tmp41C,_tmp41C + 7};static char _tmp41D[6]="tvars";static
struct _tuple5 Cyc__gentuple_962={offsetof(struct Cyc_Absyn_FnInfo,tvars),(struct
_tagged_arr){_tmp41D,_tmp41D,_tmp41D + 6},(void*)& Cyc__genrep_321};static char
_tmp41E[7]="effect";static struct _tuple5 Cyc__gentuple_963={offsetof(struct Cyc_Absyn_FnInfo,effect),(
struct _tagged_arr){_tmp41E,_tmp41E,_tmp41E + 7},(void*)& Cyc__genrep_110};static
char _tmp41F[8]="ret_typ";static struct _tuple5 Cyc__gentuple_964={offsetof(struct
Cyc_Absyn_FnInfo,ret_typ),(struct _tagged_arr){_tmp41F,_tmp41F,_tmp41F + 8},(void*)((
void*)& Cyc_Absyn_type_t_rep)};static char _tmp420[5]="args";static struct _tuple5 Cyc__gentuple_965={
offsetof(struct Cyc_Absyn_FnInfo,args),(struct _tagged_arr){_tmp420,_tmp420,
_tmp420 + 5},(void*)& Cyc__genrep_957};static char _tmp421[10]="c_varargs";static
struct _tuple5 Cyc__gentuple_966={offsetof(struct Cyc_Absyn_FnInfo,c_varargs),(
struct _tagged_arr){_tmp421,_tmp421,_tmp421 + 10},(void*)((void*)& Cyc__genrep_10)};
static char _tmp422[12]="cyc_varargs";static struct _tuple5 Cyc__gentuple_967={
offsetof(struct Cyc_Absyn_FnInfo,cyc_varargs),(struct _tagged_arr){_tmp422,_tmp422,
_tmp422 + 12},(void*)& Cyc__genrep_810};static char _tmp423[7]="rgn_po";static struct
_tuple5 Cyc__gentuple_968={offsetof(struct Cyc_Absyn_FnInfo,rgn_po),(struct
_tagged_arr){_tmp423,_tmp423,_tmp423 + 7},(void*)& Cyc__genrep_381};static char
_tmp424[11]="attributes";static struct _tuple5 Cyc__gentuple_969={offsetof(struct
Cyc_Absyn_FnInfo,attributes),(struct _tagged_arr){_tmp424,_tmp424,_tmp424 + 11},(
void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_970[8]={& Cyc__gentuple_962,&
Cyc__gentuple_963,& Cyc__gentuple_964,& Cyc__gentuple_965,& Cyc__gentuple_966,& Cyc__gentuple_967,&
Cyc__gentuple_968,& Cyc__gentuple_969};struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_971,sizeof(struct Cyc_Absyn_FnInfo),{(void*)((
struct _tuple5**)Cyc__genarr_970),(void*)((struct _tuple5**)Cyc__genarr_970),(void*)((
struct _tuple5**)Cyc__genarr_970 + 8)}};struct _tuple82{unsigned int f1;struct Cyc_Absyn_FnInfo
f2;};static struct _tuple6 Cyc__gentuple_972={offsetof(struct _tuple82,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_973={offsetof(struct _tuple82,f2),(
void*)& Cyc_Absyn_fn_info_t_rep};static struct _tuple6*Cyc__genarr_974[2]={& Cyc__gentuple_972,&
Cyc__gentuple_973};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_956={4,
sizeof(struct _tuple82),{(void*)((struct _tuple6**)Cyc__genarr_974),(void*)((
struct _tuple6**)Cyc__genarr_974),(void*)((struct _tuple6**)Cyc__genarr_974 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_952;static struct _tuple6 Cyc__gentuple_953={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_954={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_295};static struct _tuple6*Cyc__genarr_955[
2]={& Cyc__gentuple_953,& Cyc__gentuple_954};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_952={4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_955),(
void*)((struct _tuple6**)Cyc__genarr_955),(void*)((struct _tuple6**)Cyc__genarr_955
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_948;struct _tuple83{
unsigned int f1;struct Cyc_Absyn_AggrInfo f2;};static struct _tuple6 Cyc__gentuple_949={
offsetof(struct _tuple83,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_950={
offsetof(struct _tuple83,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6*Cyc__genarr_951[2]={& Cyc__gentuple_949,& Cyc__gentuple_950};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_948={4,sizeof(struct _tuple83),{(void*)((
struct _tuple6**)Cyc__genarr_951),(void*)((struct _tuple6**)Cyc__genarr_951),(void*)((
struct _tuple6**)Cyc__genarr_951 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_943;
static struct _tuple6 Cyc__gentuple_944={offsetof(struct _tuple63,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_945={offsetof(struct _tuple63,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};
static struct _tuple6 Cyc__gentuple_946={offsetof(struct _tuple63,f3),(void*)& Cyc__genrep_368};
static struct _tuple6*Cyc__genarr_947[3]={& Cyc__gentuple_944,& Cyc__gentuple_945,&
Cyc__gentuple_946};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_943={4,
sizeof(struct _tuple63),{(void*)((struct _tuple6**)Cyc__genarr_947),(void*)((
struct _tuple6**)Cyc__genarr_947),(void*)((struct _tuple6**)Cyc__genarr_947 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_938;struct _tuple84{unsigned int
f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;};static struct _tuple6 Cyc__gentuple_939={
offsetof(struct _tuple84,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_940={
offsetof(struct _tuple84,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_941={
offsetof(struct _tuple84,f3),(void*)& Cyc__genrep_621};static struct _tuple6*Cyc__genarr_942[
3]={& Cyc__gentuple_939,& Cyc__gentuple_940,& Cyc__gentuple_941};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_938={4,sizeof(struct _tuple84),{(void*)((struct _tuple6**)Cyc__genarr_942),(
void*)((struct _tuple6**)Cyc__genarr_942),(void*)((struct _tuple6**)Cyc__genarr_942
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_138;static struct _tuple6
Cyc__gentuple_935={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_936={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_139};
static struct _tuple6*Cyc__genarr_937[2]={& Cyc__gentuple_935,& Cyc__gentuple_936};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_138={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_937),(void*)((struct _tuple6**)Cyc__genarr_937),(
void*)((struct _tuple6**)Cyc__genarr_937 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_129;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_131;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep;static char
_tmp42C[12]="Typedefdecl";static struct _tagged_arr Cyc__genname_1140=(struct
_tagged_arr){_tmp42C,_tmp42C,_tmp42C + 12};static char _tmp42D[5]="name";static
struct _tuple5 Cyc__gentuple_1133={offsetof(struct Cyc_Absyn_Typedefdecl,name),(
struct _tagged_arr){_tmp42D,_tmp42D,_tmp42D + 5},(void*)& Cyc__genrep_11};static
char _tmp42E[3]="tq";static struct _tuple5 Cyc__gentuple_1134={offsetof(struct Cyc_Absyn_Typedefdecl,tq),(
struct _tagged_arr){_tmp42E,_tmp42E,_tmp42E + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp42F[4]="tvs";static struct _tuple5 Cyc__gentuple_1135={offsetof(
struct Cyc_Absyn_Typedefdecl,tvs),(struct _tagged_arr){_tmp42F,_tmp42F,_tmp42F + 4},(
void*)& Cyc__genrep_321};static char _tmp430[5]="kind";static struct _tuple5 Cyc__gentuple_1136={
offsetof(struct Cyc_Absyn_Typedefdecl,kind),(struct _tagged_arr){_tmp430,_tmp430,
_tmp430 + 5},(void*)& Cyc__genrep_1097};static char _tmp431[5]="defn";static struct
_tuple5 Cyc__gentuple_1137={offsetof(struct Cyc_Absyn_Typedefdecl,defn),(struct
_tagged_arr){_tmp431,_tmp431,_tmp431 + 5},(void*)& Cyc__genrep_110};static char
_tmp432[5]="atts";static struct _tuple5 Cyc__gentuple_1138={offsetof(struct Cyc_Absyn_Typedefdecl,atts),(
struct _tagged_arr){_tmp432,_tmp432,_tmp432 + 5},(void*)& Cyc__genrep_62};static
struct _tuple5*Cyc__genarr_1139[6]={& Cyc__gentuple_1133,& Cyc__gentuple_1134,& Cyc__gentuple_1135,&
Cyc__gentuple_1136,& Cyc__gentuple_1137,& Cyc__gentuple_1138};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Typedefdecl_rep={3,(struct _tagged_arr*)& Cyc__genname_1140,
sizeof(struct Cyc_Absyn_Typedefdecl),{(void*)((struct _tuple5**)Cyc__genarr_1139),(
void*)((struct _tuple5**)Cyc__genarr_1139),(void*)((struct _tuple5**)Cyc__genarr_1139
+ 6)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_131={1,1,(void*)((void*)&
Cyc_struct_Absyn_Typedefdecl_rep)};struct _tuple85{unsigned int f1;struct _tuple0*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Typedefdecl*f4;void**f5;};static struct
_tuple6 Cyc__gentuple_132={offsetof(struct _tuple85,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_133={offsetof(struct _tuple85,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_134={offsetof(struct _tuple85,f3),(void*)& Cyc__genrep_121};
static struct _tuple6 Cyc__gentuple_135={offsetof(struct _tuple85,f4),(void*)& Cyc__genrep_131};
static struct _tuple6 Cyc__gentuple_136={offsetof(struct _tuple85,f5),(void*)& Cyc__genrep_130};
static struct _tuple6*Cyc__genarr_137[5]={& Cyc__gentuple_132,& Cyc__gentuple_133,&
Cyc__gentuple_134,& Cyc__gentuple_135,& Cyc__gentuple_136};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_129={4,sizeof(struct _tuple85),{(void*)((struct _tuple6**)Cyc__genarr_137),(
void*)((struct _tuple6**)Cyc__genarr_137),(void*)((struct _tuple6**)Cyc__genarr_137
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_120;static struct _tuple6
Cyc__gentuple_126={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_127={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_121};
static struct _tuple6*Cyc__genarr_128[2]={& Cyc__gentuple_126,& Cyc__gentuple_127};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_120={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_128),(void*)((struct _tuple6**)Cyc__genarr_128),(
void*)((struct _tuple6**)Cyc__genarr_128 + 2)}};static char _tmp437[9]="VoidType";
static struct _tuple7 Cyc__gentuple_111={0,(struct _tagged_arr){_tmp437,_tmp437,
_tmp437 + 9}};static char _tmp438[10]="FloatType";static struct _tuple7 Cyc__gentuple_112={
1,(struct _tagged_arr){_tmp438,_tmp438,_tmp438 + 10}};static char _tmp439[8]="HeapRgn";
static struct _tuple7 Cyc__gentuple_113={2,(struct _tagged_arr){_tmp439,_tmp439,
_tmp439 + 8}};static char _tmp43A[10]="UniqueRgn";static struct _tuple7 Cyc__gentuple_114={
3,(struct _tagged_arr){_tmp43A,_tmp43A,_tmp43A + 10}};static struct _tuple7*Cyc__genarr_115[
4]={& Cyc__gentuple_111,& Cyc__gentuple_112,& Cyc__gentuple_113,& Cyc__gentuple_114};
static char _tmp43B[5]="Evar";static struct _tuple5 Cyc__gentuple_1107={0,(struct
_tagged_arr){_tmp43B,_tmp43B,_tmp43B + 5},(void*)& Cyc__genrep_1092};static char
_tmp43C[8]="VarType";static struct _tuple5 Cyc__gentuple_1108={1,(struct _tagged_arr){
_tmp43C,_tmp43C,_tmp43C + 8},(void*)& Cyc__genrep_1088};static char _tmp43D[11]="TunionType";
static struct _tuple5 Cyc__gentuple_1109={2,(struct _tagged_arr){_tmp43D,_tmp43D,
_tmp43D + 11},(void*)& Cyc__genrep_1062};static char _tmp43E[16]="TunionFieldType";
static struct _tuple5 Cyc__gentuple_1110={3,(struct _tagged_arr){_tmp43E,_tmp43E,
_tmp43E + 16},(void*)& Cyc__genrep_1036};static char _tmp43F[12]="PointerType";
static struct _tuple5 Cyc__gentuple_1111={4,(struct _tagged_arr){_tmp43F,_tmp43F,
_tmp43F + 12},(void*)& Cyc__genrep_1010};static char _tmp440[8]="IntType";static
struct _tuple5 Cyc__gentuple_1112={5,(struct _tagged_arr){_tmp440,_tmp440,_tmp440 + 
8},(void*)& Cyc__genrep_999};static char _tmp441[11]="DoubleType";static struct
_tuple5 Cyc__gentuple_1113={6,(struct _tagged_arr){_tmp441,_tmp441,_tmp441 + 11},(
void*)& Cyc__genrep_85};static char _tmp442[10]="ArrayType";static struct _tuple5 Cyc__gentuple_1114={
7,(struct _tagged_arr){_tmp442,_tmp442,_tmp442 + 10},(void*)& Cyc__genrep_975};
static char _tmp443[7]="FnType";static struct _tuple5 Cyc__gentuple_1115={8,(struct
_tagged_arr){_tmp443,_tmp443,_tmp443 + 7},(void*)& Cyc__genrep_956};static char
_tmp444[10]="TupleType";static struct _tuple5 Cyc__gentuple_1116={9,(struct
_tagged_arr){_tmp444,_tmp444,_tmp444 + 10},(void*)& Cyc__genrep_952};static char
_tmp445[9]="AggrType";static struct _tuple5 Cyc__gentuple_1117={10,(struct
_tagged_arr){_tmp445,_tmp445,_tmp445 + 9},(void*)& Cyc__genrep_948};static char
_tmp446[13]="AnonAggrType";static struct _tuple5 Cyc__gentuple_1118={11,(struct
_tagged_arr){_tmp446,_tmp446,_tmp446 + 13},(void*)& Cyc__genrep_943};static char
_tmp447[9]="EnumType";static struct _tuple5 Cyc__gentuple_1119={12,(struct
_tagged_arr){_tmp447,_tmp447,_tmp447 + 9},(void*)& Cyc__genrep_938};static char
_tmp448[13]="AnonEnumType";static struct _tuple5 Cyc__gentuple_1120={13,(struct
_tagged_arr){_tmp448,_tmp448,_tmp448 + 13},(void*)& Cyc__genrep_138};static char
_tmp449[11]="SizeofType";static struct _tuple5 Cyc__gentuple_1121={14,(struct
_tagged_arr){_tmp449,_tmp449,_tmp449 + 11},(void*)& Cyc__genrep_116};static char
_tmp44A[14]="RgnHandleType";static struct _tuple5 Cyc__gentuple_1122={15,(struct
_tagged_arr){_tmp44A,_tmp44A,_tmp44A + 14},(void*)& Cyc__genrep_116};static char
_tmp44B[12]="TypedefType";static struct _tuple5 Cyc__gentuple_1123={16,(struct
_tagged_arr){_tmp44B,_tmp44B,_tmp44B + 12},(void*)& Cyc__genrep_129};static char
_tmp44C[8]="TagType";static struct _tuple5 Cyc__gentuple_1124={17,(struct
_tagged_arr){_tmp44C,_tmp44C,_tmp44C + 8},(void*)& Cyc__genrep_116};static char
_tmp44D[8]="TypeInt";static struct _tuple5 Cyc__gentuple_1125={18,(struct
_tagged_arr){_tmp44D,_tmp44D,_tmp44D + 8},(void*)& Cyc__genrep_85};static char
_tmp44E[10]="AccessEff";static struct _tuple5 Cyc__gentuple_1126={19,(struct
_tagged_arr){_tmp44E,_tmp44E,_tmp44E + 10},(void*)& Cyc__genrep_116};static char
_tmp44F[8]="JoinEff";static struct _tuple5 Cyc__gentuple_1127={20,(struct
_tagged_arr){_tmp44F,_tmp44F,_tmp44F + 8},(void*)& Cyc__genrep_120};static char
_tmp450[8]="RgnsEff";static struct _tuple5 Cyc__gentuple_1128={21,(struct
_tagged_arr){_tmp450,_tmp450,_tmp450 + 8},(void*)& Cyc__genrep_116};static struct
_tuple5*Cyc__genarr_1129[22]={& Cyc__gentuple_1107,& Cyc__gentuple_1108,& Cyc__gentuple_1109,&
Cyc__gentuple_1110,& Cyc__gentuple_1111,& Cyc__gentuple_1112,& Cyc__gentuple_1113,&
Cyc__gentuple_1114,& Cyc__gentuple_1115,& Cyc__gentuple_1116,& Cyc__gentuple_1117,&
Cyc__gentuple_1118,& Cyc__gentuple_1119,& Cyc__gentuple_1120,& Cyc__gentuple_1121,&
Cyc__gentuple_1122,& Cyc__gentuple_1123,& Cyc__gentuple_1124,& Cyc__gentuple_1125,&
Cyc__gentuple_1126,& Cyc__gentuple_1127,& Cyc__gentuple_1128};static char _tmp452[5]="Type";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep={5,(struct _tagged_arr){
_tmp452,_tmp452,_tmp452 + 5},{(void*)((struct _tuple7**)Cyc__genarr_115),(void*)((
struct _tuple7**)Cyc__genarr_115),(void*)((struct _tuple7**)Cyc__genarr_115 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_1129),(void*)((struct _tuple5**)Cyc__genarr_1129),(
void*)((struct _tuple5**)Cyc__genarr_1129 + 22)}};static char _tmp453[8]="Vardecl";
static struct _tagged_arr Cyc__genname_184=(struct _tagged_arr){_tmp453,_tmp453,
_tmp453 + 8};static char _tmp454[3]="sc";static struct _tuple5 Cyc__gentuple_175={
offsetof(struct Cyc_Absyn_Vardecl,sc),(struct _tagged_arr){_tmp454,_tmp454,_tmp454
+ 3},(void*)& Cyc_Absyn_scope_t_rep};static char _tmp455[5]="name";static struct
_tuple5 Cyc__gentuple_176={offsetof(struct Cyc_Absyn_Vardecl,name),(struct
_tagged_arr){_tmp455,_tmp455,_tmp455 + 5},(void*)& Cyc__genrep_11};static char
_tmp456[3]="tq";static struct _tuple5 Cyc__gentuple_177={offsetof(struct Cyc_Absyn_Vardecl,tq),(
struct _tagged_arr){_tmp456,_tmp456,_tmp456 + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp457[5]="type";static struct _tuple5 Cyc__gentuple_178={offsetof(
struct Cyc_Absyn_Vardecl,type),(struct _tagged_arr){_tmp457,_tmp457,_tmp457 + 5},(
void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp458[12]="initializer";static
struct _tuple5 Cyc__gentuple_179={offsetof(struct Cyc_Absyn_Vardecl,initializer),(
struct _tagged_arr){_tmp458,_tmp458,_tmp458 + 12},(void*)& Cyc__genrep_141};static
char _tmp459[4]="rgn";static struct _tuple5 Cyc__gentuple_180={offsetof(struct Cyc_Absyn_Vardecl,rgn),(
struct _tagged_arr){_tmp459,_tmp459,_tmp459 + 4},(void*)& Cyc__genrep_110};static
char _tmp45A[11]="attributes";static struct _tuple5 Cyc__gentuple_181={offsetof(
struct Cyc_Absyn_Vardecl,attributes),(struct _tagged_arr){_tmp45A,_tmp45A,_tmp45A + 
11},(void*)& Cyc__genrep_62};static char _tmp45B[8]="escapes";static struct _tuple5
Cyc__gentuple_182={offsetof(struct Cyc_Absyn_Vardecl,escapes),(struct _tagged_arr){
_tmp45B,_tmp45B,_tmp45B + 8},(void*)((void*)& Cyc__genrep_10)};static struct _tuple5*
Cyc__genarr_183[8]={& Cyc__gentuple_175,& Cyc__gentuple_176,& Cyc__gentuple_177,&
Cyc__gentuple_178,& Cyc__gentuple_179,& Cyc__gentuple_180,& Cyc__gentuple_181,& Cyc__gentuple_182};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep={3,(struct
_tagged_arr*)& Cyc__genname_184,sizeof(struct Cyc_Absyn_Vardecl),{(void*)((struct
_tuple5**)Cyc__genarr_183),(void*)((struct _tuple5**)Cyc__genarr_183),(void*)((
struct _tuple5**)Cyc__genarr_183 + 8)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_159={
1,1,(void*)((void*)& Cyc_struct_Absyn_Vardecl_rep)};struct _tuple86{unsigned int f1;
struct Cyc_Absyn_Vardecl*f2;};static struct _tuple6 Cyc__gentuple_444={offsetof(
struct _tuple86,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_445={
offsetof(struct _tuple86,f2),(void*)& Cyc__genrep_159};static struct _tuple6*Cyc__genarr_446[
2]={& Cyc__gentuple_444,& Cyc__gentuple_445};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_443={4,sizeof(struct _tuple86),{(void*)((struct _tuple6**)Cyc__genarr_446),(
void*)((struct _tuple6**)Cyc__genarr_446),(void*)((struct _tuple6**)Cyc__genarr_446
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1160;struct _tuple87{
unsigned int f1;struct Cyc_Absyn_Pat*f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_1161={offsetof(struct _tuple87,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1162={offsetof(struct _tuple87,f2),(
void*)& Cyc__genrep_240};static struct _tuple6 Cyc__gentuple_1163={offsetof(struct
_tuple87,f3),(void*)& Cyc__genrep_231};static struct _tuple6 Cyc__gentuple_1164={
offsetof(struct _tuple87,f4),(void*)& Cyc__genrep_154};static struct _tuple6*Cyc__genarr_1165[
4]={& Cyc__gentuple_1161,& Cyc__gentuple_1162,& Cyc__gentuple_1163,& Cyc__gentuple_1164};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1160={4,sizeof(struct _tuple87),{(
void*)((struct _tuple6**)Cyc__genarr_1165),(void*)((struct _tuple6**)Cyc__genarr_1165),(
void*)((struct _tuple6**)Cyc__genarr_1165 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1156;static struct _tuple6 Cyc__gentuple_1157={offsetof(struct _tuple13,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1158={offsetof(struct
_tuple13,f2),(void*)& Cyc__genrep_232};static struct _tuple6*Cyc__genarr_1159[2]={&
Cyc__gentuple_1157,& Cyc__gentuple_1158};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1156={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_1159),(void*)((
struct _tuple6**)Cyc__genarr_1159),(void*)((struct _tuple6**)Cyc__genarr_1159 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1152;struct _tuple88{unsigned int
f1;struct Cyc_Absyn_Aggrdecl*f2;};static struct _tuple6 Cyc__gentuple_1153={
offsetof(struct _tuple88,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1154={
offsetof(struct _tuple88,f2),(void*)((void*)& Cyc__genrep_366)};static struct
_tuple6*Cyc__genarr_1155[2]={& Cyc__gentuple_1153,& Cyc__gentuple_1154};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1152={4,sizeof(struct _tuple88),{(void*)((
struct _tuple6**)Cyc__genarr_1155),(void*)((struct _tuple6**)Cyc__genarr_1155),(
void*)((struct _tuple6**)Cyc__genarr_1155 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1148;struct _tuple89{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;};
static struct _tuple6 Cyc__gentuple_1149={offsetof(struct _tuple89,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1150={offsetof(struct _tuple89,f2),(void*)((void*)&
Cyc__genrep_311)};static struct _tuple6*Cyc__genarr_1151[2]={& Cyc__gentuple_1149,&
Cyc__gentuple_1150};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1148={4,
sizeof(struct _tuple89),{(void*)((struct _tuple6**)Cyc__genarr_1151),(void*)((
struct _tuple6**)Cyc__genarr_1151),(void*)((struct _tuple6**)Cyc__genarr_1151 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1144;struct _tuple90{unsigned int
f1;struct Cyc_Absyn_Enumdecl*f2;};static struct _tuple6 Cyc__gentuple_1145={
offsetof(struct _tuple90,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1146={
offsetof(struct _tuple90,f2),(void*)& Cyc__genrep_265};static struct _tuple6*Cyc__genarr_1147[
2]={& Cyc__gentuple_1145,& Cyc__gentuple_1146};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1144={4,sizeof(struct _tuple90),{(void*)((struct _tuple6**)Cyc__genarr_1147),(
void*)((struct _tuple6**)Cyc__genarr_1147),(void*)((struct _tuple6**)Cyc__genarr_1147
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_60;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_61;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_61={1,1,(void*)((
void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct _tuple91{unsigned int f1;struct
Cyc_Absyn_Typedefdecl*f2;};static struct _tuple6 Cyc__gentuple_1141={offsetof(
struct _tuple91,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1142={
offsetof(struct _tuple91,f2),(void*)& Cyc__genrep_61};static struct _tuple6*Cyc__genarr_1143[
2]={& Cyc__gentuple_1141,& Cyc__gentuple_1142};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_60={4,sizeof(struct _tuple91),{(void*)((struct _tuple6**)Cyc__genarr_1143),(
void*)((struct _tuple6**)Cyc__genarr_1143),(void*)((struct _tuple6**)Cyc__genarr_1143
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_55;struct _tuple92{
unsigned int f1;struct _tagged_arr*f2;struct Cyc_List_List*f3;};static struct _tuple6
Cyc__gentuple_56={offsetof(struct _tuple92,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_57={offsetof(struct _tuple92,f2),(void*)& Cyc__genrep_13};
static struct _tuple6 Cyc__gentuple_58={offsetof(struct _tuple92,f3),(void*)& Cyc__genrep_0};
static struct _tuple6*Cyc__genarr_59[3]={& Cyc__gentuple_56,& Cyc__gentuple_57,& Cyc__gentuple_58};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_55={4,sizeof(struct _tuple92),{(
void*)((struct _tuple6**)Cyc__genarr_59),(void*)((struct _tuple6**)Cyc__genarr_59),(
void*)((struct _tuple6**)Cyc__genarr_59 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_50;struct _tuple93{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*
f3;};static struct _tuple6 Cyc__gentuple_51={offsetof(struct _tuple93,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_52={offsetof(struct _tuple93,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_53={offsetof(struct
_tuple93,f3),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_54[3]={& Cyc__gentuple_51,&
Cyc__gentuple_52,& Cyc__gentuple_53};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_50={
4,sizeof(struct _tuple93),{(void*)((struct _tuple6**)Cyc__genarr_54),(void*)((
struct _tuple6**)Cyc__genarr_54),(void*)((struct _tuple6**)Cyc__genarr_54 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_46;static struct _tuple6 Cyc__gentuple_47={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_48={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_49[
2]={& Cyc__gentuple_47,& Cyc__gentuple_48};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_46={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_49),(void*)((
struct _tuple6**)Cyc__genarr_49),(void*)((struct _tuple6**)Cyc__genarr_49 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_6;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_7;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_8;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_9;struct _tuple94{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int
f3;};static struct _tuple6 Cyc__gentuple_34={offsetof(struct _tuple94,f1),(void*)&
Cyc__genrep_2};static struct _tuple6 Cyc__gentuple_35={offsetof(struct _tuple94,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_36={offsetof(struct
_tuple94,f3),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_37[
3]={& Cyc__gentuple_34,& Cyc__gentuple_35,& Cyc__gentuple_36};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_9={4,sizeof(struct _tuple94),{(void*)((struct _tuple6**)Cyc__genarr_37),(
void*)((struct _tuple6**)Cyc__genarr_37),(void*)((struct _tuple6**)Cyc__genarr_37 + 
3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_8={1,1,(void*)((void*)&
Cyc__genrep_9)};static char _tmp46B[5]="List";static struct _tagged_arr Cyc__genname_41=(
struct _tagged_arr){_tmp46B,_tmp46B,_tmp46B + 5};static char _tmp46C[3]="hd";static
struct _tuple5 Cyc__gentuple_38={offsetof(struct Cyc_List_List,hd),(struct
_tagged_arr){_tmp46C,_tmp46C,_tmp46C + 3},(void*)& Cyc__genrep_8};static char
_tmp46D[3]="tl";static struct _tuple5 Cyc__gentuple_39={offsetof(struct Cyc_List_List,tl),(
struct _tagged_arr){_tmp46D,_tmp46D,_tmp46D + 3},(void*)& Cyc__genrep_7};static
struct _tuple5*Cyc__genarr_40[2]={& Cyc__gentuple_38,& Cyc__gentuple_39};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep={3,(struct
_tagged_arr*)& Cyc__genname_41,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_40),(void*)((struct _tuple5**)Cyc__genarr_40),(void*)((
struct _tuple5**)Cyc__genarr_40 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_7={
1,1,(void*)((void*)& Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep)};
struct _tuple95{unsigned int f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_42={offsetof(struct _tuple95,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_43={offsetof(struct _tuple95,f2),(void*)& Cyc__genrep_0};
static struct _tuple6 Cyc__gentuple_44={offsetof(struct _tuple95,f3),(void*)& Cyc__genrep_7};
static struct _tuple6*Cyc__genarr_45[3]={& Cyc__gentuple_42,& Cyc__gentuple_43,& Cyc__gentuple_44};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_6={4,sizeof(struct _tuple95),{(
void*)((struct _tuple6**)Cyc__genarr_45),(void*)((struct _tuple6**)Cyc__genarr_45),(
void*)((struct _tuple6**)Cyc__genarr_45 + 3)}};static char _tmp471[9]="Porton_d";
static struct _tuple7 Cyc__gentuple_3={0,(struct _tagged_arr){_tmp471,_tmp471,
_tmp471 + 9}};static char _tmp472[10]="Portoff_d";static struct _tuple7 Cyc__gentuple_4={
1,(struct _tagged_arr){_tmp472,_tmp472,_tmp472 + 10}};static struct _tuple7*Cyc__genarr_5[
2]={& Cyc__gentuple_3,& Cyc__gentuple_4};static char _tmp473[6]="Var_d";static struct
_tuple5 Cyc__gentuple_1166={0,(struct _tagged_arr){_tmp473,_tmp473,_tmp473 + 6},(
void*)& Cyc__genrep_443};static char _tmp474[5]="Fn_d";static struct _tuple5 Cyc__gentuple_1167={
1,(struct _tagged_arr){_tmp474,_tmp474,_tmp474 + 5},(void*)& Cyc__genrep_808};
static char _tmp475[6]="Let_d";static struct _tuple5 Cyc__gentuple_1168={2,(struct
_tagged_arr){_tmp475,_tmp475,_tmp475 + 6},(void*)& Cyc__genrep_1160};static char
_tmp476[7]="Letv_d";static struct _tuple5 Cyc__gentuple_1169={3,(struct _tagged_arr){
_tmp476,_tmp476,_tmp476 + 7},(void*)& Cyc__genrep_1156};static char _tmp477[7]="Aggr_d";
static struct _tuple5 Cyc__gentuple_1170={4,(struct _tagged_arr){_tmp477,_tmp477,
_tmp477 + 7},(void*)& Cyc__genrep_1152};static char _tmp478[9]="Tunion_d";static
struct _tuple5 Cyc__gentuple_1171={5,(struct _tagged_arr){_tmp478,_tmp478,_tmp478 + 
9},(void*)& Cyc__genrep_1148};static char _tmp479[7]="Enum_d";static struct _tuple5
Cyc__gentuple_1172={6,(struct _tagged_arr){_tmp479,_tmp479,_tmp479 + 7},(void*)&
Cyc__genrep_1144};static char _tmp47A[10]="Typedef_d";static struct _tuple5 Cyc__gentuple_1173={
7,(struct _tagged_arr){_tmp47A,_tmp47A,_tmp47A + 10},(void*)& Cyc__genrep_60};
static char _tmp47B[12]="Namespace_d";static struct _tuple5 Cyc__gentuple_1174={8,(
struct _tagged_arr){_tmp47B,_tmp47B,_tmp47B + 12},(void*)& Cyc__genrep_55};static
char _tmp47C[8]="Using_d";static struct _tuple5 Cyc__gentuple_1175={9,(struct
_tagged_arr){_tmp47C,_tmp47C,_tmp47C + 8},(void*)& Cyc__genrep_50};static char
_tmp47D[10]="ExternC_d";static struct _tuple5 Cyc__gentuple_1176={10,(struct
_tagged_arr){_tmp47D,_tmp47D,_tmp47D + 10},(void*)& Cyc__genrep_46};static char
_tmp47E[17]="ExternCinclude_d";static struct _tuple5 Cyc__gentuple_1177={11,(struct
_tagged_arr){_tmp47E,_tmp47E,_tmp47E + 17},(void*)& Cyc__genrep_6};static struct
_tuple5*Cyc__genarr_1178[12]={& Cyc__gentuple_1166,& Cyc__gentuple_1167,& Cyc__gentuple_1168,&
Cyc__gentuple_1169,& Cyc__gentuple_1170,& Cyc__gentuple_1171,& Cyc__gentuple_1172,&
Cyc__gentuple_1173,& Cyc__gentuple_1174,& Cyc__gentuple_1175,& Cyc__gentuple_1176,&
Cyc__gentuple_1177};static char _tmp480[9]="Raw_decl";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_decl_t_rep={5,(struct _tagged_arr){_tmp480,_tmp480,_tmp480 + 9},{(
void*)((struct _tuple7**)Cyc__genarr_5),(void*)((struct _tuple7**)Cyc__genarr_5),(
void*)((struct _tuple7**)Cyc__genarr_5 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_1178),(
void*)((struct _tuple5**)Cyc__genarr_1178),(void*)((struct _tuple5**)Cyc__genarr_1178
+ 12)}};static char _tmp481[5]="Decl";static struct _tagged_arr Cyc__genname_1182=(
struct _tagged_arr){_tmp481,_tmp481,_tmp481 + 5};static char _tmp482[2]="r";static
struct _tuple5 Cyc__gentuple_1179={offsetof(struct Cyc_Absyn_Decl,r),(struct
_tagged_arr){_tmp482,_tmp482,_tmp482 + 2},(void*)& Cyc_Absyn_raw_decl_t_rep};
static char _tmp483[4]="loc";static struct _tuple5 Cyc__gentuple_1180={offsetof(
struct Cyc_Absyn_Decl,loc),(struct _tagged_arr){_tmp483,_tmp483,_tmp483 + 4},(void*)&
Cyc__genrep_2};static struct _tuple5*Cyc__genarr_1181[2]={& Cyc__gentuple_1179,& Cyc__gentuple_1180};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Decl_rep={3,(struct _tagged_arr*)&
Cyc__genname_1182,sizeof(struct Cyc_Absyn_Decl),{(void*)((struct _tuple5**)Cyc__genarr_1181),(
void*)((struct _tuple5**)Cyc__genarr_1181),(void*)((struct _tuple5**)Cyc__genarr_1181
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1={1,1,(void*)((void*)&
Cyc_struct_Absyn_Decl_rep)};static char _tmp486[5]="List";static struct _tagged_arr
Cyc__genname_1186=(struct _tagged_arr){_tmp486,_tmp486,_tmp486 + 5};static char
_tmp487[3]="hd";static struct _tuple5 Cyc__gentuple_1183={offsetof(struct Cyc_List_List,hd),(
struct _tagged_arr){_tmp487,_tmp487,_tmp487 + 3},(void*)& Cyc__genrep_1};static char
_tmp488[3]="tl";static struct _tuple5 Cyc__gentuple_1184={offsetof(struct Cyc_List_List,tl),(
struct _tagged_arr){_tmp488,_tmp488,_tmp488 + 3},(void*)& Cyc__genrep_0};static
struct _tuple5*Cyc__genarr_1185[2]={& Cyc__gentuple_1183,& Cyc__gentuple_1184};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_1186,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_1185),(void*)((struct _tuple5**)Cyc__genarr_1185),(
void*)((struct _tuple5**)Cyc__genarr_1185 + 2)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_0={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_decl_t46H2_rep)};
void*Cyc_decls_rep=(void*)& Cyc__genrep_0;
