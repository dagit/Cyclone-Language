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
void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct
Cyc_List_List*x);extern char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct
_tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[
14];struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};
struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int
q_restrict;int real_const;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Conref{
void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*
name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_AbsUpper_b_struct{int tag;void*f1;};struct Cyc_Absyn_PtrLoc{
struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;struct Cyc_Absyn_Conref*nullable;struct
Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*zero_term;struct Cyc_Absyn_PtrLoc*
ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct
Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;
struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*
tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct _tuple0*
name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct Cyc_Absyn_UnknownTunionInfo
f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};
struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*targs;void*rgn;}
;struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*tunion_name;struct _tuple0*
field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int tag;
struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_exn_typ;void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct
Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
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
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern
char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct _tuple3{void*f1;
void*f2;};struct _tuple3*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d);struct _tuple3*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct
Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_type_vars(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct
Cyc_List_List*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*
clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_enter_try(struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct
Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_goto(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _tagged_arr*,struct Cyc_Absyn_Stmt**);
struct _tuple4{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct _tuple4*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Switch_clause***);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,
struct _tagged_arr*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*cs);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*r,int resetable);void Cyc_Tcenv_check_rgn_resetable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
int*Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct
Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,
struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _tagged_arr msg_part);struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*
patvars;};struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[
15];struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;struct Cyc_List_List*f1;};
extern char Cyc_CfFlowInfo_IsZero[11];extern char Cyc_CfFlowInfo_NotZero[12];struct
Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[
13];struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct
Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ReachableFL_struct{
int tag;struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);static void Cyc_Tcstmt_decorate_stmt(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));s->try_depth=Cyc_Tcenv_get_try_depth(te);}static
void Cyc_Tcstmt_pattern_synth(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
where_opt,int new_block){struct _tuple6*_tmp1;struct Cyc_List_List*_tmp2;struct Cyc_Tcpat_TcPatResult
_tmp0=pat_res;_tmp1=_tmp0.tvars_and_bounds_opt;_tmp2=_tmp0.patvars;{struct Cyc_List_List*
_tmp3=_tmp1 == 0?0:(*_tmp1).f1;struct Cyc_List_List*_tmp4=_tmp1 == 0?0:(*_tmp1).f2;
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(loc,te,_tmp3);te2=Cyc_Tcenv_new_outlives_constraints(
te2,_tmp4);if(new_block)te2=Cyc_Tcenv_new_block(loc,te2);{struct Cyc_Core_Opt*
_tmp5=({struct Cyc_Core_Opt*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8->v=(void*)Cyc_Tcenv_curr_rgn(
te2);_tmp8;});{struct Cyc_List_List*_tmp6=_tmp2;for(0;_tmp6 != 0;_tmp6=_tmp6->tl){
te2=Cyc_Tcenv_add_pat_var(loc,te2,(struct Cyc_Absyn_Vardecl*)_tmp6->hd);((struct
Cyc_Absyn_Vardecl*)_tmp6->hd)->rgn=_tmp5;}}if(where_opt != 0)Cyc_Tcexp_tcTest(te2,(
struct Cyc_Absyn_Exp*)where_opt,({const char*_tmp7="switch clause guard";_tag_arr(
_tmp7,sizeof(char),_get_zero_arr_size(_tmp7,20));}));if(_tmp2 != 0)te2=Cyc_Tcenv_set_encloser(
te2,s);Cyc_Tcstmt_tcStmt(te2,s,0);if(_tmp2 != 0)Cyc_NewControlFlow_set_encloser(s,
Cyc_Tcenv_get_encloser(te));}}}struct _tuple7{struct Cyc_Tcenv_Tenv*f1;struct Cyc_List_List*
f2;};struct _tuple7 Cyc_Tcstmt_check_forarray_decls(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,struct Cyc_List_List*decls){({void*_tmp9[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpA="check_forarray_decls";
_tag_arr(_tmpA,sizeof(char),_get_zero_arr_size(_tmpA,21));}),_tag_arr(_tmp9,
sizeof(void*),0));});return({struct _tuple7 _tmpB;_tmpB.f1=te;_tmpB.f2=0;_tmpB;});}
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int
new_block){Cyc_Tcstmt_decorate_stmt(te,s0);{void*_tmpC=(void*)s0->r;struct Cyc_Absyn_Exp*
_tmpD;struct Cyc_Absyn_Stmt*_tmpE;struct Cyc_Absyn_Stmt*_tmpF;struct Cyc_Absyn_Exp*
_tmp10;struct Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Stmt*_tmp12;struct Cyc_Absyn_Stmt*
_tmp13;struct _tuple2 _tmp14;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Stmt*
_tmp16;struct Cyc_Absyn_Stmt*_tmp17;struct Cyc_Absyn_Exp*_tmp18;struct _tuple2
_tmp19;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Stmt*_tmp1B;struct _tuple2
_tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Stmt*_tmp1E;struct Cyc_Absyn_Stmt*
_tmp1F;struct Cyc_Absyn_Stmt*_tmp20;struct _tuple2 _tmp21;struct Cyc_Absyn_Exp*
_tmp22;struct Cyc_Absyn_Stmt*_tmp23;struct Cyc_Absyn_Stmt*_tmp24;struct Cyc_Absyn_Stmt**
_tmp25;struct Cyc_Absyn_Stmt*_tmp26;struct Cyc_Absyn_Stmt**_tmp27;struct
_tagged_arr*_tmp28;struct Cyc_Absyn_Stmt*_tmp29;struct Cyc_Absyn_Stmt**_tmp2A;
struct Cyc_List_List*_tmp2B;struct Cyc_Absyn_Switch_clause**_tmp2C;struct Cyc_Absyn_Switch_clause***
_tmp2D;struct _tagged_arr*_tmp2E;struct Cyc_Absyn_Stmt*_tmp2F;struct Cyc_Absyn_Exp*
_tmp30;struct Cyc_List_List*_tmp31;struct Cyc_Absyn_Stmt*_tmp32;struct Cyc_List_List*
_tmp33;struct Cyc_Absyn_Decl*_tmp34;struct Cyc_Absyn_Stmt*_tmp35;struct Cyc_Absyn_Tvar*
_tmp36;struct Cyc_Absyn_Vardecl*_tmp37;int _tmp38;struct Cyc_Absyn_Stmt*_tmp39;
struct Cyc_Absyn_Exp*_tmp3A;_LL1: if((int)_tmpC != 0)goto _LL3;_LL2: return;_LL3: if(
_tmpC <= (void*)1  || *((int*)_tmpC)!= 0)goto _LL5;_tmpD=((struct Cyc_Absyn_Exp_s_struct*)
_tmpC)->f1;_LL4: Cyc_Tcexp_tcExp(te,0,_tmpD);return;_LL5: if(_tmpC <= (void*)1  || *((
int*)_tmpC)!= 1)goto _LL7;_tmpE=((struct Cyc_Absyn_Seq_s_struct*)_tmpC)->f1;_tmpF=((
struct Cyc_Absyn_Seq_s_struct*)_tmpC)->f2;_LL6: {struct Cyc_Tcenv_Tenv*_tmp3B=Cyc_Tcenv_set_next(
te,(void*)({struct Cyc_Tcenv_Stmt_j_struct*_tmp41=_cycalloc(sizeof(*_tmp41));
_tmp41[0]=({struct Cyc_Tcenv_Stmt_j_struct _tmp42;_tmp42.tag=0;_tmp42.f1=_tmpF;
_tmp42;});_tmp41;}));Cyc_Tcstmt_tcStmt(_tmp3B,_tmpE,1);while(1){void*_tmp3C=(
void*)_tmpF->r;struct Cyc_Absyn_Stmt*_tmp3D;struct Cyc_Absyn_Stmt*_tmp3E;_LL26: if(
_tmp3C <= (void*)1  || *((int*)_tmp3C)!= 1)goto _LL28;_tmp3D=((struct Cyc_Absyn_Seq_s_struct*)
_tmp3C)->f1;_tmp3E=((struct Cyc_Absyn_Seq_s_struct*)_tmp3C)->f2;_LL27: Cyc_Tcstmt_decorate_stmt(
te,_tmpF);_tmp3B=Cyc_Tcenv_set_next(te,(void*)({struct Cyc_Tcenv_Stmt_j_struct*
_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F[0]=({struct Cyc_Tcenv_Stmt_j_struct
_tmp40;_tmp40.tag=0;_tmp40.f1=_tmp3E;_tmp40;});_tmp3F;}));Cyc_Tcstmt_tcStmt(
_tmp3B,_tmp3D,1);_tmpF=_tmp3E;continue;_LL28:;_LL29: goto seq_end;_LL25:;}seq_end:
Cyc_Tcstmt_tcStmt(te,_tmpF,1);return;}_LL7: if(_tmpC <= (void*)1  || *((int*)_tmpC)
!= 2)goto _LL9;_tmp10=((struct Cyc_Absyn_Return_s_struct*)_tmpC)->f1;_LL8: {void*t=
Cyc_Tcenv_return_typ(te);if(_tmp10 == 0){void*_tmp43=Cyc_Tcutil_compress(t);_LL2B:
if((int)_tmp43 != 0)goto _LL2D;_LL2C: goto _LL2A;_LL2D:;_LL2E:({struct Cyc_String_pa_struct
_tmp46;_tmp46.tag=0;_tmp46.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp44[1]={& _tmp46};Cyc_Tcutil_terr(s0->loc,({const char*_tmp45="must return a value of type %s";
_tag_arr(_tmp45,sizeof(char),_get_zero_arr_size(_tmp45,31));}),_tag_arr(_tmp44,
sizeof(void*),1));}});goto _LL2A;_LL2A:;}else{struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
_tmp10;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmp4A;_tmp4A.tag=0;_tmp4A.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp49;
_tmp49.tag=0;_tmp49.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp47[2]={& _tmp49,&
_tmp4A};Cyc_Tcutil_terr(s0->loc,({const char*_tmp48="returns value of type %s but requires %s";
_tag_arr(_tmp48,sizeof(char),_get_zero_arr_size(_tmp48,41));}),_tag_arr(_tmp47,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}return;}_LL9: if(_tmpC <= (
void*)1  || *((int*)_tmpC)!= 3)goto _LLB;_tmp11=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpC)->f1;_tmp12=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpC)->f2;_tmp13=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmpC)->f3;_LLA: Cyc_Tcexp_tcTest(te,_tmp11,({
const char*_tmp4B="if statement";_tag_arr(_tmp4B,sizeof(char),_get_zero_arr_size(
_tmp4B,13));}));Cyc_Tcstmt_tcStmt(te,_tmp12,1);Cyc_Tcstmt_tcStmt(te,_tmp13,1);
return;_LLB: if(_tmpC <= (void*)1  || *((int*)_tmpC)!= 4)goto _LLD;_tmp14=((struct
Cyc_Absyn_While_s_struct*)_tmpC)->f1;_tmp15=_tmp14.f1;_tmp16=_tmp14.f2;_tmp17=((
struct Cyc_Absyn_While_s_struct*)_tmpC)->f2;_LLC: Cyc_Tcstmt_decorate_stmt(te,
_tmp16);Cyc_Tcexp_tcTest(te,_tmp15,({const char*_tmp4C="while loop";_tag_arr(
_tmp4C,sizeof(char),_get_zero_arr_size(_tmp4C,11));}));Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(
te,_tmp16),_tmp17,1);return;_LLD: if(_tmpC <= (void*)1  || *((int*)_tmpC)!= 8)goto
_LLF;_tmp18=((struct Cyc_Absyn_For_s_struct*)_tmpC)->f1;_tmp19=((struct Cyc_Absyn_For_s_struct*)
_tmpC)->f2;_tmp1A=_tmp19.f1;_tmp1B=_tmp19.f2;_tmp1C=((struct Cyc_Absyn_For_s_struct*)
_tmpC)->f3;_tmp1D=_tmp1C.f1;_tmp1E=_tmp1C.f2;_tmp1F=((struct Cyc_Absyn_For_s_struct*)
_tmpC)->f4;_LLE: Cyc_Tcstmt_decorate_stmt(te,_tmp1B);Cyc_Tcstmt_decorate_stmt(te,
_tmp1E);Cyc_Tcexp_tcExp(te,0,_tmp18);Cyc_Tcexp_tcTest(te,_tmp1A,({const char*
_tmp4D="for loop";_tag_arr(_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,9));}));
te=Cyc_Tcenv_set_in_loop(te,_tmp1E);Cyc_Tcstmt_tcStmt(te,_tmp1F,1);Cyc_Tcexp_tcExp(
te,0,_tmp1D);return;_LLF: if(_tmpC <= (void*)1  || *((int*)_tmpC)!= 13)goto _LL11;
_tmp20=((struct Cyc_Absyn_Do_s_struct*)_tmpC)->f1;_tmp21=((struct Cyc_Absyn_Do_s_struct*)
_tmpC)->f2;_tmp22=_tmp21.f1;_tmp23=_tmp21.f2;_LL10: Cyc_Tcstmt_decorate_stmt(te,
_tmp23);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(te,_tmp23),_tmp20,1);Cyc_Tcexp_tcTest(
te,_tmp22,({const char*_tmp4E="do loop";_tag_arr(_tmp4E,sizeof(char),
_get_zero_arr_size(_tmp4E,8));}));return;_LL11: if(_tmpC <= (void*)1  || *((int*)
_tmpC)!= 5)goto _LL13;_tmp24=((struct Cyc_Absyn_Break_s_struct*)_tmpC)->f1;_tmp25=(
struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Break_s_struct*)_tmpC)->f1;_LL12: Cyc_Tcenv_process_break(
te,s0,_tmp25);return;_LL13: if(_tmpC <= (void*)1  || *((int*)_tmpC)!= 6)goto _LL15;
_tmp26=((struct Cyc_Absyn_Continue_s_struct*)_tmpC)->f1;_tmp27=(struct Cyc_Absyn_Stmt**)&((
struct Cyc_Absyn_Continue_s_struct*)_tmpC)->f1;_LL14: Cyc_Tcenv_process_continue(
te,s0,_tmp27);return;_LL15: if(_tmpC <= (void*)1  || *((int*)_tmpC)!= 7)goto _LL17;
_tmp28=((struct Cyc_Absyn_Goto_s_struct*)_tmpC)->f1;_tmp29=((struct Cyc_Absyn_Goto_s_struct*)
_tmpC)->f2;_tmp2A=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_struct*)
_tmpC)->f2;_LL16: Cyc_Tcenv_process_goto(te,s0,_tmp28,_tmp2A);return;_LL17: if(
_tmpC <= (void*)1  || *((int*)_tmpC)!= 10)goto _LL19;_tmp2B=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmpC)->f1;_tmp2C=((struct Cyc_Absyn_Fallthru_s_struct*)_tmpC)->f2;_tmp2D=(struct
Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_struct*)_tmpC)->f2;
_LL18: {struct _tuple4*_tmp4F=Cyc_Tcenv_process_fallthru(te,s0,_tmp2D);if(_tmp4F
== 0){({void*_tmp50[0]={};Cyc_Tcutil_terr(s0->loc,({const char*_tmp51="fallthru not in a non-last case";
_tag_arr(_tmp51,sizeof(char),_get_zero_arr_size(_tmp51,32));}),_tag_arr(_tmp50,
sizeof(void*),0));});return;}{struct _tuple4 _tmp53;struct Cyc_List_List*_tmp54;
struct Cyc_List_List*_tmp55;struct _tuple4*_tmp52=(struct _tuple4*)_tmp4F;_tmp53=*
_tmp52;_tmp54=_tmp53.f2;_tmp55=_tmp53.f3;{struct Cyc_List_List*instantiation=((
struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,
Cyc_Tcenv_lookup_type_vars(te),_tmp54);_tmp55=((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_List_List*,void*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_substitute,instantiation,_tmp55);for(0;_tmp55 != 0  && _tmp2B != 0;(
_tmp55=_tmp55->tl,_tmp2B=_tmp2B->tl)){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp2B->hd);if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp2B->hd,(void*)
_tmp55->hd)){({struct Cyc_String_pa_struct _tmp59;_tmp59.tag=0;_tmp59.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)_tmp55->hd));{
struct Cyc_String_pa_struct _tmp58;_tmp58.tag=0;_tmp58.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)_tmp2B->hd)->topt))->v));{void*_tmp56[2]={&
_tmp58,& _tmp59};Cyc_Tcutil_terr(s0->loc,({const char*_tmp57="fallthru argument has type %s but pattern variable has type %s";
_tag_arr(_tmp57,sizeof(char),_get_zero_arr_size(_tmp57,63));}),_tag_arr(_tmp56,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}if(_tmp2B != 0)({void*
_tmp5A[0]={};Cyc_Tcutil_terr(s0->loc,({const char*_tmp5B="too many arguments to explicit fallthru";
_tag_arr(_tmp5B,sizeof(char),_get_zero_arr_size(_tmp5B,40));}),_tag_arr(_tmp5A,
sizeof(void*),0));});if(_tmp55 != 0)({void*_tmp5C[0]={};Cyc_Tcutil_terr(s0->loc,({
const char*_tmp5D="too few arguments to explicit fallthru";_tag_arr(_tmp5D,
sizeof(char),_get_zero_arr_size(_tmp5D,39));}),_tag_arr(_tmp5C,sizeof(void*),0));});
return;}}}_LL19: if(_tmpC <= (void*)1  || *((int*)_tmpC)!= 12)goto _LL1B;_tmp2E=((
struct Cyc_Absyn_Label_s_struct*)_tmpC)->f1;_tmp2F=((struct Cyc_Absyn_Label_s_struct*)
_tmpC)->f2;_LL1A: Cyc_Tcstmt_tcStmt(Cyc_Tcenv_new_named_block(s0->loc,Cyc_Tcenv_add_label(
te,_tmp2E,_tmp2F),({struct Cyc_Absyn_Tvar*_tmp5E=_cycalloc(sizeof(*_tmp5E));
_tmp5E->name=({struct _tagged_arr*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61[0]=(
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp64;_tmp64.tag=0;_tmp64.f1=(
struct _tagged_arr)((struct _tagged_arr)*_tmp2E);{void*_tmp62[1]={& _tmp64};Cyc_aprintf(({
const char*_tmp63="`%s";_tag_arr(_tmp63,sizeof(char),_get_zero_arr_size(_tmp63,4));}),
_tag_arr(_tmp62,sizeof(void*),1));}});_tmp61;});_tmp5E->identity=Cyc_Tcutil_new_tvar_id();
_tmp5E->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp5F=_cycalloc(
sizeof(*_tmp5F));_tmp5F[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp60;_tmp60.tag=0;
_tmp60.f1=(void*)((void*)3);_tmp60;});_tmp5F;}));_tmp5E;})),_tmp2F,0);return;
_LL1B: if(_tmpC <= (void*)1  || *((int*)_tmpC)!= 9)goto _LL1D;_tmp30=((struct Cyc_Absyn_Switch_s_struct*)
_tmpC)->f1;_tmp31=((struct Cyc_Absyn_Switch_s_struct*)_tmpC)->f2;_LL1C: Cyc_Tcexp_tcExp(
te,0,_tmp30);{void*_tmp65=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp30->topt))->v;
te=Cyc_Tcenv_set_in_switch(te);te=Cyc_Tcenv_clear_fallthru(te);{struct Cyc_List_List*
scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp31);for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Pat*_tmp66=((struct Cyc_Absyn_Switch_clause*)
scs->hd)->pattern;struct Cyc_Tcpat_TcPatResult _tmp67=Cyc_Tcpat_tcPat(te,_tmp66,(
void**)& _tmp65);struct Cyc_List_List*_tmp68=_tmp67.tvars_and_bounds_opt == 0?0:(*
_tmp67.tvars_and_bounds_opt).f1;if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp66->topt))->v,_tmp65)){({struct Cyc_String_pa_struct _tmp6C;_tmp6C.tag=
0;_tmp6C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp66->topt))->v));{struct Cyc_String_pa_struct
_tmp6B;_tmp6B.tag=0;_tmp6B.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp65));{void*_tmp69[2]={& _tmp6B,& _tmp6C};Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,({const char*_tmp6A="switch on type %s, but case expects type %s";
_tag_arr(_tmp6A,sizeof(char),_get_zero_arr_size(_tmp6A,44));}),_tag_arr(_tmp69,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}else{Cyc_Tcpat_check_pat_regions(
te,_tmp66);}((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=({struct Cyc_Core_Opt*
_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->v=_tmp67.patvars;_tmp6D;});Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,_tmp67,((struct Cyc_Absyn_Switch_clause*)
scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);if(
_tmp67.tvars_and_bounds_opt != 0  && (*_tmp67.tvars_and_bounds_opt).f2 != 0)te=Cyc_Tcenv_clear_fallthru(
te);else{te=Cyc_Tcenv_set_fallthru(te,_tmp68,_tmp67.patvars,(struct Cyc_Absyn_Switch_clause*)
scs->hd);}}}Cyc_Tcpat_check_switch_exhaustive(s0->loc,_tmp31);return;}_LL1D: if(
_tmpC <= (void*)1  || *((int*)_tmpC)!= 14)goto _LL1F;_tmp32=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmpC)->f1;_tmp33=((struct Cyc_Absyn_TryCatch_s_struct*)_tmpC)->f2;_LL1E:(void*)(
_tmp32->r=(void*)((void*)(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Seq_s_struct*
_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E[0]=({struct Cyc_Absyn_Seq_s_struct _tmp6F;
_tmp6F.tag=1;_tmp6F.f1=Cyc_Absyn_new_stmt((void*)_tmp32->r,_tmp32->loc);_tmp6F.f2=
Cyc_Absyn_skip_stmt(_tmp32->loc);_tmp6F;});_tmp6E;}),_tmp32->loc))->r));Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser(Cyc_Tcenv_enter_try(te),s0),_tmp32,1);te=Cyc_Tcenv_set_in_switch(
te);te=Cyc_Tcenv_clear_fallthru(te);{struct Cyc_List_List*_tmp70=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp33);for(0;_tmp70 != 0;_tmp70=_tmp70->tl){
struct Cyc_Absyn_Pat*_tmp71=((struct Cyc_Absyn_Switch_clause*)_tmp70->hd)->pattern;
struct Cyc_Tcpat_TcPatResult _tmp72=Cyc_Tcpat_tcPat(te,_tmp71,(void**)& Cyc_Absyn_exn_typ);
struct Cyc_List_List*_tmp73=_tmp72.tvars_and_bounds_opt == 0?0:(*_tmp72.tvars_and_bounds_opt).f1;
if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp71->topt))->v,
Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct _tmp76;_tmp76.tag=0;_tmp76.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp71->topt))->v));{void*_tmp74[1]={& _tmp76};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Switch_clause*)_tmp70->hd)->loc,({const char*_tmp75="expected xtunion exn but found %s";
_tag_arr(_tmp75,sizeof(char),_get_zero_arr_size(_tmp75,34));}),_tag_arr(_tmp74,
sizeof(void*),1));}});else{Cyc_Tcpat_check_pat_regions(te,_tmp71);}((struct Cyc_Absyn_Switch_clause*)
_tmp70->hd)->pat_vars=({struct Cyc_Core_Opt*_tmp77=_cycalloc(sizeof(*_tmp77));
_tmp77->v=_tmp72.patvars;_tmp77;});Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)
_tmp70->hd)->loc,te,_tmp72,((struct Cyc_Absyn_Switch_clause*)_tmp70->hd)->body,((
struct Cyc_Absyn_Switch_clause*)_tmp70->hd)->where_clause,1);if(_tmp72.tvars_and_bounds_opt
!= 0  && (*_tmp72.tvars_and_bounds_opt).f2 != 0)te=Cyc_Tcenv_clear_fallthru(te);
else{te=Cyc_Tcenv_set_fallthru(te,_tmp73,_tmp72.patvars,(struct Cyc_Absyn_Switch_clause*)
_tmp70->hd);}}}Cyc_Tcpat_check_catch_overlap(s0->loc,_tmp33);return;_LL1F: if(
_tmpC <= (void*)1  || *((int*)_tmpC)!= 11)goto _LL21;_tmp34=((struct Cyc_Absyn_Decl_s_struct*)
_tmpC)->f1;_tmp35=((struct Cyc_Absyn_Decl_s_struct*)_tmpC)->f2;_LL20: {struct
_tagged_arr unimp_msg_part;if(new_block)te=Cyc_Tcenv_new_block(s0->loc,te);{void*
_tmp78=(void*)_tmp34->r;struct Cyc_Absyn_Vardecl*_tmp79;struct Cyc_Absyn_Pat*
_tmp7A;struct Cyc_Core_Opt*_tmp7B;struct Cyc_Core_Opt**_tmp7C;struct Cyc_Absyn_Exp*
_tmp7D;struct Cyc_List_List*_tmp7E;struct _tagged_arr*_tmp7F;struct Cyc_List_List*
_tmp80;struct _tuple0*_tmp81;struct Cyc_List_List*_tmp82;_LL30: if(_tmp78 <= (void*)
2  || *((int*)_tmp78)!= 0)goto _LL32;_tmp79=((struct Cyc_Absyn_Var_d_struct*)_tmp78)->f1;
_LL31: {struct Cyc_Absyn_Vardecl _tmp84;void*_tmp85;struct _tuple0*_tmp86;struct
_tuple0 _tmp87;void*_tmp88;struct _tagged_arr*_tmp89;struct Cyc_Absyn_Tqual _tmp8A;
void*_tmp8B;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_Core_Opt*_tmp8D;struct Cyc_Core_Opt**
_tmp8E;struct Cyc_List_List*_tmp8F;struct Cyc_Absyn_Vardecl*_tmp83=_tmp79;_tmp84=*
_tmp83;_tmp85=(void*)_tmp84.sc;_tmp86=_tmp84.name;_tmp87=*_tmp86;_tmp88=_tmp87.f1;
_tmp89=_tmp87.f2;_tmp8A=_tmp84.tq;_tmp8B=(void*)_tmp84.type;_tmp8C=_tmp84.initializer;
_tmp8D=_tmp84.rgn;_tmp8E=(struct Cyc_Core_Opt**)&(*_tmp83).rgn;_tmp8F=_tmp84.attributes;{
void*_tmp90=Cyc_Tcenv_curr_rgn(te);int is_local;{void*_tmp91=_tmp85;_LL4D: if((int)
_tmp91 != 0)goto _LL4F;_LL4E: goto _LL50;_LL4F: if((int)_tmp91 != 3)goto _LL51;_LL50:
goto _LL52;_LL51: if((int)_tmp91 != 4)goto _LL53;_LL52: is_local=0;goto _LL4C;_LL53:
if((int)_tmp91 != 1)goto _LL55;_LL54:({void*_tmp92[0]={};Cyc_Tcutil_terr(_tmp34->loc,({
const char*_tmp93="bad abstract scope for local variable";_tag_arr(_tmp93,sizeof(
char),_get_zero_arr_size(_tmp93,38));}),_tag_arr(_tmp92,sizeof(void*),0));});
goto _LL56;_LL55: if((int)_tmp91 != 5)goto _LL57;_LL56: goto _LL58;_LL57: if((int)
_tmp91 != 2)goto _LL4C;_LL58: is_local=1;goto _LL4C;_LL4C:;}*_tmp8E=is_local?({
struct Cyc_Core_Opt*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->v=(void*)_tmp90;
_tmp94;}):({struct Cyc_Core_Opt*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->v=(void*)((
void*)2);_tmp95;});{void*_tmp96=_tmp88;struct Cyc_List_List*_tmp97;struct Cyc_List_List*
_tmp98;_LL5A: if((int)_tmp96 != 0)goto _LL5C;_LL5B: goto _LL59;_LL5C: if(_tmp96 <= (
void*)1  || *((int*)_tmp96)!= 0)goto _LL5E;_tmp97=((struct Cyc_Absyn_Rel_n_struct*)
_tmp96)->f1;if(_tmp97 != 0)goto _LL5E;_LL5D:(*_tmp79->name).f1=(void*)0;goto _LL59;
_LL5E: if(_tmp96 <= (void*)1  || *((int*)_tmp96)!= 1)goto _LL60;_tmp98=((struct Cyc_Absyn_Abs_n_struct*)
_tmp96)->f1;_LL5F:(int)_throw(({void*_tmp99[0]={};Cyc_Tcutil_impos(({const char*
_tmp9A="tcstmt: Abs_n declaration";_tag_arr(_tmp9A,sizeof(char),
_get_zero_arr_size(_tmp9A,26));}),_tag_arr(_tmp99,sizeof(void*),0));}));_LL60:;
_LL61:({void*_tmp9B[0]={};Cyc_Tcutil_terr(_tmp34->loc,({const char*_tmp9C="cannot declare a qualified local variable";
_tag_arr(_tmp9C,sizeof(char),_get_zero_arr_size(_tmp9C,42));}),_tag_arr(_tmp9B,
sizeof(void*),0));});goto _LL59;_LL59:;}{void*_tmp9D=Cyc_Tcutil_compress(_tmp8B);
struct Cyc_Absyn_ArrayInfo _tmp9E;void*_tmp9F;struct Cyc_Absyn_Tqual _tmpA0;struct
Cyc_Absyn_Exp*_tmpA1;struct Cyc_Absyn_Conref*_tmpA2;struct Cyc_Position_Segment*
_tmpA3;_LL63: if(_tmp9D <= (void*)3  || *((int*)_tmp9D)!= 7)goto _LL65;_tmp9E=((
struct Cyc_Absyn_ArrayType_struct*)_tmp9D)->f1;_tmp9F=(void*)_tmp9E.elt_type;
_tmpA0=_tmp9E.tq;_tmpA1=_tmp9E.num_elts;if(_tmpA1 != 0)goto _LL65;_tmpA2=_tmp9E.zero_term;
_tmpA3=_tmp9E.zt_loc;if(!(_tmp79->initializer != 0))goto _LL65;_LL64:{void*_tmpA4=(
void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp79->initializer))->r;void*_tmpA5;
struct _tagged_arr _tmpA6;struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_List_List*_tmpA8;
struct Cyc_List_List*_tmpA9;_LL68: if(*((int*)_tmpA4)!= 0)goto _LL6A;_tmpA5=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmpA4)->f1;if(_tmpA5 <= (void*)1  || *((int*)
_tmpA5)!= 5)goto _LL6A;_tmpA6=((struct Cyc_Absyn_String_c_struct*)_tmpA5)->f1;
_LL69: _tmp8B=(void*)(_tmp79->type=(void*)Cyc_Absyn_array_typ(_tmp9F,_tmpA0,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_arr_size(_tmpA6,sizeof(char)),0),
_tmpA2,_tmpA3));goto _LL67;_LL6A: if(*((int*)_tmpA4)!= 29)goto _LL6C;_tmpA7=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmpA4)->f2;_LL6B: _tmp8B=(void*)(_tmp79->type=(
void*)Cyc_Absyn_array_typ(_tmp9F,_tmpA0,(struct Cyc_Absyn_Exp*)_tmpA7,_tmpA2,
_tmpA3));goto _LL67;_LL6C: if(*((int*)_tmpA4)!= 36)goto _LL6E;_tmpA8=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpA4)->f2;_LL6D: _tmpA9=_tmpA8;goto _LL6F;_LL6E: if(*((int*)_tmpA4)!= 28)goto
_LL70;_tmpA9=((struct Cyc_Absyn_Array_e_struct*)_tmpA4)->f1;_LL6F: _tmp8B=(void*)(
_tmp79->type=(void*)Cyc_Absyn_array_typ(_tmp9F,_tmpA0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA9),0),_tmpA2,
_tmpA3));goto _LL67;_LL70:;_LL71: goto _LL67;_LL67:;}goto _LL62;_LL65:;_LL66: goto
_LL62;_LL62:;}{struct Cyc_List_List*_tmpAA=!is_local?0: Cyc_Tcenv_lookup_type_vars(
te);int _tmpAB=!is_local?0: 1;Cyc_Tcutil_check_type(s0->loc,te,_tmpAA,(void*)1,
_tmpAB,_tmp8B);{struct Cyc_Tcenv_Tenv*_tmpAC=Cyc_Tcenv_add_local_var(_tmp35->loc,
te,_tmp79);if(_tmp85 == (void*)3  || _tmp85 == (void*)4)({void*_tmpAD[0]={};Cyc_Tcutil_terr(
_tmp34->loc,({const char*_tmpAE="local extern declarations not yet supported";
_tag_arr(_tmpAE,sizeof(char),_get_zero_arr_size(_tmpAE,44));}),_tag_arr(_tmpAD,
sizeof(void*),0));});if(_tmp8C != 0){Cyc_Tcexp_tcExpInitializer(_tmpAC,(void**)&
_tmp8B,(struct Cyc_Absyn_Exp*)_tmp8C);if(!is_local  && !Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)_tmp8C))({void*_tmpAF[0]={};Cyc_Tcutil_terr(_tmp34->loc,({
const char*_tmpB0="initializer needs to be a constant expression";_tag_arr(_tmpB0,
sizeof(char),_get_zero_arr_size(_tmpB0,46));}),_tag_arr(_tmpAF,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_assign(_tmpAC,(struct Cyc_Absyn_Exp*)_tmp8C,_tmp8B)){struct
_tagged_arr _tmpB1=*_tmp89;struct _tagged_arr _tmpB2=Cyc_Absynpp_typ2string(_tmp8B);
struct _tagged_arr _tmpB3=Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp8C->topt))->v);if(((_get_arr_size(_tmpB1,sizeof(char))+ 
_get_arr_size(_tmpB2,sizeof(char)))+ _get_arr_size(_tmpB3,sizeof(char)))+ 50 < 80)({
struct Cyc_String_pa_struct _tmpB8;_tmpB8.tag=0;_tmpB8.f1=(struct _tagged_arr)((
struct _tagged_arr)_tmpB3);{struct Cyc_String_pa_struct _tmpB7;_tmpB7.tag=0;_tmpB7.f1=(
struct _tagged_arr)((struct _tagged_arr)_tmpB2);{struct Cyc_String_pa_struct _tmpB6;
_tmpB6.tag=0;_tmpB6.f1=(struct _tagged_arr)((struct _tagged_arr)_tmpB1);{void*
_tmpB4[3]={& _tmpB6,& _tmpB7,& _tmpB8};Cyc_Tcutil_terr(_tmp34->loc,({const char*
_tmpB5="%s declared with type %s but initialized with type %s.";_tag_arr(_tmpB5,
sizeof(char),_get_zero_arr_size(_tmpB5,55));}),_tag_arr(_tmpB4,sizeof(void*),3));}}}});
else{if((_get_arr_size(_tmpB1,sizeof(char))+ _get_arr_size(_tmpB2,sizeof(char)))
+ 25 < 80  && _get_arr_size(_tmpB3,sizeof(char))+ 25 < 80)({struct Cyc_String_pa_struct
_tmpBD;_tmpBD.tag=0;_tmpBD.f1=(struct _tagged_arr)((struct _tagged_arr)_tmpB3);{
struct Cyc_String_pa_struct _tmpBC;_tmpBC.tag=0;_tmpBC.f1=(struct _tagged_arr)((
struct _tagged_arr)_tmpB2);{struct Cyc_String_pa_struct _tmpBB;_tmpBB.tag=0;_tmpBB.f1=(
struct _tagged_arr)((struct _tagged_arr)_tmpB1);{void*_tmpB9[3]={& _tmpBB,& _tmpBC,&
_tmpBD};Cyc_Tcutil_terr(_tmp34->loc,({const char*_tmpBA="%s declared with type %s\n but initialized with type %s.";
_tag_arr(_tmpBA,sizeof(char),_get_zero_arr_size(_tmpBA,56));}),_tag_arr(_tmpB9,
sizeof(void*),3));}}}});else{({struct Cyc_String_pa_struct _tmpC2;_tmpC2.tag=0;
_tmpC2.f1=(struct _tagged_arr)((struct _tagged_arr)_tmpB3);{struct Cyc_String_pa_struct
_tmpC1;_tmpC1.tag=0;_tmpC1.f1=(struct _tagged_arr)((struct _tagged_arr)_tmpB2);{
struct Cyc_String_pa_struct _tmpC0;_tmpC0.tag=0;_tmpC0.f1=(struct _tagged_arr)((
struct _tagged_arr)_tmpB1);{void*_tmpBE[3]={& _tmpC0,& _tmpC1,& _tmpC2};Cyc_Tcutil_terr(
_tmp34->loc,({const char*_tmpBF="%s declared with type \n%s\n but initialized with type \n%s.";
_tag_arr(_tmpBF,sizeof(char),_get_zero_arr_size(_tmpBF,58));}),_tag_arr(_tmpBE,
sizeof(void*),3));}}}});}}Cyc_Tcutil_unify(_tmp8B,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp8C->topt))->v);Cyc_Tcutil_explain_failure();}}Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser(_tmpAC,s0),_tmp35,0);return;}}}}_LL32: if(_tmp78 <= (void*)
2  || *((int*)_tmp78)!= 2)goto _LL34;_tmp7A=((struct Cyc_Absyn_Let_d_struct*)_tmp78)->f1;
_tmp7B=((struct Cyc_Absyn_Let_d_struct*)_tmp78)->f2;_tmp7C=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Let_d_struct*)_tmp78)->f2;_tmp7D=((struct Cyc_Absyn_Let_d_struct*)
_tmp78)->f3;_LL33: Cyc_Tcexp_tcExpInitializer(te,0,_tmp7D);{struct Cyc_Tcpat_TcPatResult
_tmpC3=Cyc_Tcpat_tcPat(te,_tmp7A,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)
_check_null(_tmp7D->topt))->v)));*_tmp7C=({struct Cyc_Core_Opt*_tmpC4=_cycalloc(
sizeof(*_tmpC4));_tmpC4->v=_tmpC3.patvars;_tmpC4;});if(!Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp7A->topt))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7D->topt))->v) && !Cyc_Tcutil_coerce_assign(te,_tmp7D,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp7A->topt))->v)){({struct Cyc_String_pa_struct
_tmpC8;_tmpC8.tag=0;_tmpC8.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp7D->topt))->v));{struct Cyc_String_pa_struct
_tmpC7;_tmpC7.tag=0;_tmpC7.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp7A->topt))->v));{void*_tmpC5[2]={&
_tmpC7,& _tmpC8};Cyc_Tcutil_terr(_tmp34->loc,({const char*_tmpC6="pattern type %s does not match definition type %s";
_tag_arr(_tmpC6,sizeof(char),_get_zero_arr_size(_tmpC6,50));}),_tag_arr(_tmpC5,
sizeof(void*),2));}}});Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7A->topt))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7D->topt))->v);
Cyc_Tcutil_explain_failure();}else{Cyc_Tcpat_check_pat_regions(te,_tmp7A);}Cyc_Tcpat_check_let_pat_exhaustive(
_tmp7A->loc,_tmp7A);Cyc_Tcstmt_pattern_synth(s0->loc,te,_tmpC3,_tmp35,0,0);
return;}_LL34: if(_tmp78 <= (void*)2  || *((int*)_tmp78)!= 3)goto _LL36;_tmp7E=((
struct Cyc_Absyn_Letv_d_struct*)_tmp78)->f1;_LL35: {void*_tmpC9=Cyc_Tcenv_curr_rgn(
te);struct Cyc_Tcenv_Tenv*_tmpCA=te;for(0;_tmp7E != 0;_tmp7E=_tmp7E->tl){struct Cyc_Absyn_Vardecl*
_tmpCB=(struct Cyc_Absyn_Vardecl*)_tmp7E->hd;struct Cyc_Absyn_Vardecl _tmpCD;struct
_tuple0*_tmpCE;struct _tuple0 _tmpCF;void*_tmpD0;void*_tmpD1;struct Cyc_Core_Opt*
_tmpD2;struct Cyc_Core_Opt**_tmpD3;struct Cyc_Absyn_Vardecl*_tmpCC=_tmpCB;_tmpCD=*
_tmpCC;_tmpCE=_tmpCD.name;_tmpCF=*_tmpCE;_tmpD0=_tmpCF.f1;_tmpD1=(void*)_tmpCD.type;
_tmpD2=_tmpCD.rgn;_tmpD3=(struct Cyc_Core_Opt**)&(*_tmpCC).rgn;*_tmpD3=({struct
Cyc_Core_Opt*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->v=(void*)_tmpC9;_tmpD4;});{
void*_tmpD5=_tmpD0;struct Cyc_List_List*_tmpD6;struct Cyc_List_List*_tmpD7;_LL73:
if((int)_tmpD5 != 0)goto _LL75;_LL74: goto _LL72;_LL75: if(_tmpD5 <= (void*)1  || *((
int*)_tmpD5)!= 0)goto _LL77;_tmpD6=((struct Cyc_Absyn_Rel_n_struct*)_tmpD5)->f1;
if(_tmpD6 != 0)goto _LL77;_LL76:(*_tmpCB->name).f1=(void*)0;goto _LL72;_LL77: if(
_tmpD5 <= (void*)1  || *((int*)_tmpD5)!= 1)goto _LL79;_tmpD7=((struct Cyc_Absyn_Abs_n_struct*)
_tmpD5)->f1;_LL78:(int)_throw(({void*_tmpD8[0]={};Cyc_Tcutil_impos(({const char*
_tmpD9="tcstmt: Abs_n declaration";_tag_arr(_tmpD9,sizeof(char),
_get_zero_arr_size(_tmpD9,26));}),_tag_arr(_tmpD8,sizeof(void*),0));}));_LL79:;
_LL7A:({void*_tmpDA[0]={};Cyc_Tcutil_terr(_tmp34->loc,({const char*_tmpDB="cannot declare a qualified local variable";
_tag_arr(_tmpDB,sizeof(char),_get_zero_arr_size(_tmpDB,42));}),_tag_arr(_tmpDA,
sizeof(void*),0));});goto _LL72;_LL72:;}Cyc_Tcutil_check_type(s0->loc,_tmpCA,Cyc_Tcenv_lookup_type_vars(
_tmpCA),(void*)1,1,_tmpD1);_tmpCA=Cyc_Tcenv_add_local_var(_tmp35->loc,_tmpCA,
_tmpCB);}Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmpCA,s0),_tmp35,0);return;}
_LL36: if(_tmp78 <= (void*)2  || *((int*)_tmp78)!= 8)goto _LL38;_tmp7F=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp78)->f1;_tmp80=((struct Cyc_Absyn_Namespace_d_struct*)_tmp78)->f2;_LL37:
unimp_msg_part=({const char*_tmpDC="namespace";_tag_arr(_tmpDC,sizeof(char),
_get_zero_arr_size(_tmpDC,10));});goto _LL2F;_LL38: if(_tmp78 <= (void*)2  || *((int*)
_tmp78)!= 9)goto _LL3A;_tmp81=((struct Cyc_Absyn_Using_d_struct*)_tmp78)->f1;
_tmp82=((struct Cyc_Absyn_Using_d_struct*)_tmp78)->f2;_LL39: unimp_msg_part=({
const char*_tmpDD="using";_tag_arr(_tmpDD,sizeof(char),_get_zero_arr_size(_tmpDD,
6));});goto _LL2F;_LL3A: if(_tmp78 <= (void*)2  || *((int*)_tmp78)!= 1)goto _LL3C;
_LL3B: unimp_msg_part=({const char*_tmpDE="function";_tag_arr(_tmpDE,sizeof(char),
_get_zero_arr_size(_tmpDE,9));});goto _LL2F;_LL3C: if(_tmp78 <= (void*)2  || *((int*)
_tmp78)!= 4)goto _LL3E;_LL3D: unimp_msg_part=({const char*_tmpDF="type";_tag_arr(
_tmpDF,sizeof(char),_get_zero_arr_size(_tmpDF,5));});goto _LL2F;_LL3E: if(_tmp78 <= (
void*)2  || *((int*)_tmp78)!= 5)goto _LL40;_LL3F: unimp_msg_part=({const char*_tmpE0="[x]tunion";
_tag_arr(_tmpE0,sizeof(char),_get_zero_arr_size(_tmpE0,10));});goto _LL2F;_LL40:
if(_tmp78 <= (void*)2  || *((int*)_tmp78)!= 6)goto _LL42;_LL41: unimp_msg_part=({
const char*_tmpE1="enum";_tag_arr(_tmpE1,sizeof(char),_get_zero_arr_size(_tmpE1,5));});
goto _LL2F;_LL42: if(_tmp78 <= (void*)2  || *((int*)_tmp78)!= 7)goto _LL44;_LL43:
unimp_msg_part=({const char*_tmpE2="typedef";_tag_arr(_tmpE2,sizeof(char),
_get_zero_arr_size(_tmpE2,8));});goto _LL2F;_LL44: if(_tmp78 <= (void*)2  || *((int*)
_tmp78)!= 10)goto _LL46;_LL45: unimp_msg_part=({const char*_tmpE3="extern \"C\"";
_tag_arr(_tmpE3,sizeof(char),_get_zero_arr_size(_tmpE3,11));});goto _LL2F;_LL46:
if(_tmp78 <= (void*)2  || *((int*)_tmp78)!= 11)goto _LL48;_LL47: unimp_msg_part=({
const char*_tmpE4="extern \"C include\"";_tag_arr(_tmpE4,sizeof(char),
_get_zero_arr_size(_tmpE4,19));});goto _LL2F;_LL48: if((int)_tmp78 != 0)goto _LL4A;
_LL49: unimp_msg_part=({const char*_tmpE5="__cyclone_port_on__";_tag_arr(_tmpE5,
sizeof(char),_get_zero_arr_size(_tmpE5,20));});goto _LL2F;_LL4A: if((int)_tmp78 != 
1)goto _LL2F;_LL4B: unimp_msg_part=({const char*_tmpE6="__cyclone_port_off__";
_tag_arr(_tmpE6,sizeof(char),_get_zero_arr_size(_tmpE6,21));});goto _LL2F;_LL2F:;}(
int)_throw(({struct Cyc_String_pa_struct _tmpE9;_tmpE9.tag=0;_tmpE9.f1=(struct
_tagged_arr)((struct _tagged_arr)unimp_msg_part);{void*_tmpE7[1]={& _tmpE9};Cyc_Tcutil_impos(({
const char*_tmpE8="tcStmt: nested %s declarations unimplemented";_tag_arr(_tmpE8,
sizeof(char),_get_zero_arr_size(_tmpE8,45));}),_tag_arr(_tmpE7,sizeof(void*),1));}}));}
_LL21: if(_tmpC <= (void*)1  || *((int*)_tmpC)!= 15)goto _LL23;_tmp36=((struct Cyc_Absyn_Region_s_struct*)
_tmpC)->f1;_tmp37=((struct Cyc_Absyn_Region_s_struct*)_tmpC)->f2;_tmp38=((struct
Cyc_Absyn_Region_s_struct*)_tmpC)->f3;_tmp39=((struct Cyc_Absyn_Region_s_struct*)
_tmpC)->f4;_LL22:(void*)(_tmp39->r=(void*)((void*)(Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Seq_s_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=({
struct Cyc_Absyn_Seq_s_struct _tmpEB;_tmpEB.tag=1;_tmpEB.f1=Cyc_Absyn_new_stmt((
void*)_tmp39->r,_tmp39->loc);_tmpEB.f2=Cyc_Absyn_skip_stmt(_tmp39->loc);_tmpEB;});
_tmpEA;}),_tmp39->loc))->r));if(new_block)te=Cyc_Tcenv_new_block(s0->loc,te);{
struct Cyc_Absyn_Vardecl _tmpED;void*_tmpEE;struct Cyc_Core_Opt*_tmpEF;struct Cyc_Core_Opt**
_tmpF0;struct Cyc_Absyn_Vardecl*_tmpEC=_tmp37;_tmpED=*_tmpEC;_tmpEE=(void*)_tmpED.type;
_tmpEF=_tmpED.rgn;_tmpF0=(struct Cyc_Core_Opt**)&(*_tmpEC).rgn;{void*_tmpF1=Cyc_Tcenv_curr_rgn(
te);void*rgn_typ=(void*)({struct Cyc_Absyn_VarType_struct*_tmpF8=_cycalloc(
sizeof(*_tmpF8));_tmpF8[0]=({struct Cyc_Absyn_VarType_struct _tmpF9;_tmpF9.tag=1;
_tmpF9.f1=_tmp36;_tmpF9;});_tmpF8;});*_tmpF0=({struct Cyc_Core_Opt*_tmpF2=
_cycalloc(sizeof(*_tmpF2));_tmpF2->v=(void*)_tmpF1;_tmpF2;});te=Cyc_Tcenv_add_type_vars(
s0->loc,te,({struct Cyc_List_List*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->hd=
_tmp36;_tmpF3->tl=0;_tmpF3;}));Cyc_Tcutil_check_type(s0->loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)2,1,_tmpEE);te=Cyc_Tcenv_add_region(te,rgn_typ,_tmp38);if(!Cyc_Tcutil_unify(
_tmpEE,(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpF4=_cycalloc(sizeof(*
_tmpF4));_tmpF4[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmpF5;_tmpF5.tag=15;
_tmpF5.f1=(void*)rgn_typ;_tmpF5;});_tmpF4;})))({void*_tmpF6[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpF7="region stmt: type of region handle is wrong!";
_tag_arr(_tmpF7,sizeof(char),_get_zero_arr_size(_tmpF7,45));}),_tag_arr(_tmpF6,
sizeof(void*),0));});Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(Cyc_Tcenv_add_local_var(
_tmp39->loc,Cyc_Tcenv_enter_try(te),_tmp37),s0),_tmp39,0);return;}}_LL23: if(
_tmpC <= (void*)1  || *((int*)_tmpC)!= 16)goto _LL0;_tmp3A=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmpC)->f1;_LL24: {void*rgn_type=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpFF=
_cycalloc(sizeof(*_tmpFF));_tmpFF->v=(void*)((void*)3);_tmpFF;}),({struct Cyc_Core_Opt*
_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp100;}));void*etype=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpFD=
_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmpFE;_tmpFE.tag=15;_tmpFE.f1=(void*)rgn_type;_tmpFE;});_tmpFD;});if(!Cyc_Tcutil_unify(
Cyc_Tcexp_tcExp(te,(void**)& etype,_tmp3A),etype))({struct Cyc_String_pa_struct
_tmpFC;_tmpFC.tag=0;_tmpFC.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3A->topt))->v));{void*_tmpFA[1]={&
_tmpFC};Cyc_Tcutil_terr(_tmp3A->loc,({const char*_tmpFB="expecting region_t but found %s";
_tag_arr(_tmpFB,sizeof(char),_get_zero_arr_size(_tmpFB,32));}),_tag_arr(_tmpFA,
sizeof(void*),1));}});Cyc_Tcenv_check_rgn_resetable(te,s0->loc,rgn_type);return;}
_LL0:;}}
