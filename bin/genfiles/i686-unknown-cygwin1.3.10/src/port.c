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
_tagged_arr);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,
void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[8];int Cyc_strptrcmp(struct
_tagged_arr*s1,struct _tagged_arr*s2);struct _tagged_arr Cyc_strconcat_l(struct Cyc_List_List*);
struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];extern int
Cyc_Position_use_gcc_style_location;struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
struct _tuple0{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{
int print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
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
_tuple0*,struct _tuple0*);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);
extern void*Cyc_Absyn_sint_typ;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _tagged_arr,struct
_tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};int
Cyc_printf(struct _tagged_arr,struct _tagged_arr);extern char Cyc_FileCloseError[19];
extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct
_tagged_arr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple0*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));int Cyc_Dict_member(
struct Cyc_Dict_Dict*d,void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*k);struct _tuple3{void*f1;void*f2;
};struct _tuple3*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);
struct _tuple3*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);extern char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*field;};struct Cyc_Tcgenrep_RepInfo;
struct Cyc_Position_Segment{int start;int end;};struct Cyc_Port_Edit{struct Cyc_Position_Segment*
loc;struct _tagged_arr old_string;struct _tagged_arr new_string;};int Cyc_Port_cmp_edit(
struct Cyc_Port_Edit*e1,struct Cyc_Port_Edit*e2){if((unsigned int)e1->loc  && (
unsigned int)e2->loc)return((struct Cyc_Position_Segment*)_check_null(e1->loc))->start
- ((struct Cyc_Position_Segment*)_check_null(e2->loc))->start;else{if(e1->loc == 
e2->loc)return 0;else{if((unsigned int)e1->loc)return 1;else{return - 1;}}}}struct
Cyc_Port_Cvar{int id;void**eq;struct Cyc_List_List*uppers;struct Cyc_List_List*
lowers;};struct Cyc_Port_Cfield{void*qual;struct _tagged_arr*f;void*type;};struct
Cyc_Port_Ptr_ct_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_Port_Array_ct_struct{
int tag;void*f1;void*f2;};struct Cyc_Port_KnownAggr_ct_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;struct Cyc_List_List*f2;};struct Cyc_Port_UnknownAggr_ct_struct{int tag;struct
Cyc_List_List*f1;void**f2;};struct Cyc_Port_Fn_ct_struct{int tag;void*f1;struct Cyc_List_List*
f2;};struct Cyc_Port_Var_ct_struct{int tag;struct Cyc_Port_Cvar*f1;};static struct
_tagged_arr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts);static struct
_tagged_arr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*ts);static struct
_tagged_arr Cyc_Port_ctype2string(int deep,void*t){void*_tmp0=t;void*_tmp1;void*
_tmp2;void*_tmp3;void*_tmp4;void*_tmp5;struct Cyc_Absyn_Aggrdecl*_tmp6;struct Cyc_List_List*
_tmp7;struct Cyc_List_List*_tmp8;void**_tmp9;void*_tmpA;struct Cyc_List_List*_tmpB;
void*_tmpC;struct Cyc_List_List*_tmpD;struct Cyc_Port_Cvar*_tmpE;_LL1: if((int)
_tmp0 != 0)goto _LL3;_LL2: return({const char*_tmpF="const";_tag_arr(_tmpF,sizeof(
char),_get_zero_arr_size(_tmpF,6));});_LL3: if((int)_tmp0 != 1)goto _LL5;_LL4:
return({const char*_tmp10="notconst";_tag_arr(_tmp10,sizeof(char),
_get_zero_arr_size(_tmp10,9));});_LL5: if((int)_tmp0 != 2)goto _LL7;_LL6: return({
const char*_tmp11="thin";_tag_arr(_tmp11,sizeof(char),_get_zero_arr_size(_tmp11,5));});
_LL7: if((int)_tmp0 != 3)goto _LL9;_LL8: return({const char*_tmp12="fat";_tag_arr(
_tmp12,sizeof(char),_get_zero_arr_size(_tmp12,4));});_LL9: if((int)_tmp0 != 4)goto
_LLB;_LLA: return({const char*_tmp13="void";_tag_arr(_tmp13,sizeof(char),
_get_zero_arr_size(_tmp13,5));});_LLB: if((int)_tmp0 != 5)goto _LLD;_LLC: return({
const char*_tmp14="zero";_tag_arr(_tmp14,sizeof(char),_get_zero_arr_size(_tmp14,5));});
_LLD: if((int)_tmp0 != 6)goto _LLF;_LLE: return({const char*_tmp15="arith";_tag_arr(
_tmp15,sizeof(char),_get_zero_arr_size(_tmp15,6));});_LLF: if(_tmp0 <= (void*)7
 || *((int*)_tmp0)!= 0)goto _LL11;_tmp1=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp0)->f1;_tmp2=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp0)->f2;_tmp3=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmp0)->f3;_LL10: return(struct _tagged_arr)({struct
Cyc_String_pa_struct _tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Port_ctype2string(deep,_tmp3));{struct Cyc_String_pa_struct _tmp19;
_tmp19.tag=0;_tmp19.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Port_ctype2string(
deep,_tmp2));{struct Cyc_String_pa_struct _tmp18;_tmp18.tag=0;_tmp18.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Port_ctype2string(deep,_tmp1));{void*_tmp16[
3]={& _tmp18,& _tmp19,& _tmp1A};Cyc_aprintf(({const char*_tmp17="ptr(%s,%s,%s)";
_tag_arr(_tmp17,sizeof(char),_get_zero_arr_size(_tmp17,14));}),_tag_arr(_tmp16,
sizeof(void*),3));}}}});_LL11: if(_tmp0 <= (void*)7  || *((int*)_tmp0)!= 1)goto
_LL13;_tmp4=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp0)->f1;_tmp5=(void*)((
struct Cyc_Port_Array_ct_struct*)_tmp0)->f2;_LL12: return(struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp1E;_tmp1E.tag=0;_tmp1E.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Port_ctype2string(deep,_tmp5));{struct Cyc_String_pa_struct
_tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Port_ctype2string(
deep,_tmp4));{void*_tmp1B[2]={& _tmp1D,& _tmp1E};Cyc_aprintf(({const char*_tmp1C="array(%s,%s)";
_tag_arr(_tmp1C,sizeof(char),_get_zero_arr_size(_tmp1C,13));}),_tag_arr(_tmp1B,
sizeof(void*),2));}}});_LL13: if(_tmp0 <= (void*)7  || *((int*)_tmp0)!= 2)goto _LL15;
_tmp6=((struct Cyc_Port_KnownAggr_ct_struct*)_tmp0)->f1;_tmp7=((struct Cyc_Port_KnownAggr_ct_struct*)
_tmp0)->f2;_LL14: {struct _tagged_arr s=(void*)_tmp6->kind == (void*)0?({const char*
_tmp24="struct";_tag_arr(_tmp24,sizeof(char),_get_zero_arr_size(_tmp24,7));}):({
const char*_tmp25="union";_tag_arr(_tmp25,sizeof(char),_get_zero_arr_size(_tmp25,
6));});return(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp23;_tmp23.tag=0;
_tmp23.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Port_cfields2string(deep,
_tmp7));{struct Cyc_String_pa_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp6->name));{struct Cyc_String_pa_struct
_tmp21;_tmp21.tag=0;_tmp21.f1=(struct _tagged_arr)((struct _tagged_arr)s);{void*
_tmp1F[3]={& _tmp21,& _tmp22,& _tmp23};Cyc_aprintf(({const char*_tmp20="%s %s {%s}";
_tag_arr(_tmp20,sizeof(char),_get_zero_arr_size(_tmp20,11));}),_tag_arr(_tmp1F,
sizeof(void*),3));}}}});}_LL15: if(_tmp0 <= (void*)7  || *((int*)_tmp0)!= 3)goto
_LL17;_tmp8=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmp0)->f1;_tmp9=((struct Cyc_Port_UnknownAggr_ct_struct*)
_tmp0)->f2;if(_tmp9 == 0)goto _LL17;_tmpA=*_tmp9;_LL16: return Cyc_Port_ctype2string(
deep,_tmpA);_LL17: if(_tmp0 <= (void*)7  || *((int*)_tmp0)!= 3)goto _LL19;_tmpB=((
struct Cyc_Port_UnknownAggr_ct_struct*)_tmp0)->f1;_LL18: return(struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Port_cfields2string(deep,_tmpB));{void*_tmp26[1]={& _tmp28};
Cyc_aprintf(({const char*_tmp27="aggr {%s}";_tag_arr(_tmp27,sizeof(char),
_get_zero_arr_size(_tmp27,10));}),_tag_arr(_tmp26,sizeof(void*),1));}});_LL19:
if(_tmp0 <= (void*)7  || *((int*)_tmp0)!= 4)goto _LL1B;_tmpC=(void*)((struct Cyc_Port_Fn_ct_struct*)
_tmp0)->f1;_tmpD=((struct Cyc_Port_Fn_ct_struct*)_tmp0)->f2;_LL1A: return(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Port_ctype2string(deep,_tmpC));{struct Cyc_String_pa_struct
_tmp2B;_tmp2B.tag=0;_tmp2B.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Port_ctypes2string(
deep,_tmpD));{void*_tmp29[2]={& _tmp2B,& _tmp2C};Cyc_aprintf(({const char*_tmp2A="fn(%s)->%s";
_tag_arr(_tmp2A,sizeof(char),_get_zero_arr_size(_tmp2A,11));}),_tag_arr(_tmp29,
sizeof(void*),2));}}});_LL1B: if(_tmp0 <= (void*)7  || *((int*)_tmp0)!= 5)goto _LL0;
_tmpE=((struct Cyc_Port_Var_ct_struct*)_tmp0)->f1;_LL1C: if((unsigned int)_tmpE->eq)
return Cyc_Port_ctype2string(deep,*((void**)_check_null(_tmpE->eq)));else{if(!
deep)return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp2F;_tmp2F.tag=1;
_tmp2F.f1=(unsigned int)_tmpE->id;{void*_tmp2D[1]={& _tmp2F};Cyc_aprintf(({const
char*_tmp2E="var(%d)";_tag_arr(_tmp2E,sizeof(char),_get_zero_arr_size(_tmp2E,8));}),
_tag_arr(_tmp2D,sizeof(void*),1));}});else{return(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp34;_tmp34.tag=0;_tmp34.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Port_ctypes2string(
0,_tmpE->lowers));{struct Cyc_String_pa_struct _tmp33;_tmp33.tag=0;_tmp33.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Port_ctypes2string(0,_tmpE->uppers));{
struct Cyc_Int_pa_struct _tmp32;_tmp32.tag=1;_tmp32.f1=(unsigned int)_tmpE->id;{
void*_tmp30[3]={& _tmp32,& _tmp33,& _tmp34};Cyc_aprintf(({const char*_tmp31="var(%d[>=%s][<=%s])";
_tag_arr(_tmp31,sizeof(char),_get_zero_arr_size(_tmp31,20));}),_tag_arr(_tmp30,
sizeof(void*),3));}}}});}}_LL0:;}static struct _tagged_arr*Cyc_Port_ctype2stringptr(
int deep,void*t){return({struct _tagged_arr*_tmp35=_cycalloc(sizeof(*_tmp35));
_tmp35[0]=Cyc_Port_ctype2string(deep,t);_tmp35;});}struct Cyc_List_List*Cyc_Port_sep(
struct _tagged_arr s,struct Cyc_List_List*xs){struct _tagged_arr*_tmp36=({struct
_tagged_arr*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A[0]=s;_tmp3A;});if(xs == 0)
return xs;{struct Cyc_List_List*_tmp37=xs;struct Cyc_List_List*_tmp38=xs->tl;for(0;
_tmp38 != 0;(_tmp37=_tmp38,_tmp38=_tmp38->tl)){((struct Cyc_List_List*)_check_null(
_tmp37))->tl=({struct Cyc_List_List*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->hd=
_tmp36;_tmp39->tl=_tmp38;_tmp39;});}return xs;}}static struct _tagged_arr*Cyc_Port_cfield2stringptr(
int deep,struct Cyc_Port_Cfield*f){struct _tagged_arr s=(struct _tagged_arr)({struct
Cyc_String_pa_struct _tmp40;_tmp40.tag=0;_tmp40.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Port_ctype2string(deep,(void*)f->type));{struct Cyc_String_pa_struct
_tmp3F;_tmp3F.tag=0;_tmp3F.f1=(struct _tagged_arr)((struct _tagged_arr)*f->f);{
struct Cyc_String_pa_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Port_ctype2string(deep,(void*)f->qual));{void*_tmp3C[3]={&
_tmp3E,& _tmp3F,& _tmp40};Cyc_aprintf(({const char*_tmp3D="%s %s: %s";_tag_arr(
_tmp3D,sizeof(char),_get_zero_arr_size(_tmp3D,10));}),_tag_arr(_tmp3C,sizeof(
void*),3));}}}});return({struct _tagged_arr*_tmp3B=_cycalloc(sizeof(*_tmp3B));
_tmp3B[0]=s;_tmp3B;});}static struct _tagged_arr Cyc_Port_ctypes2string(int deep,
struct Cyc_List_List*ts){return(struct _tagged_arr)Cyc_strconcat_l(Cyc_Port_sep(({
const char*_tmp41=",";_tag_arr(_tmp41,sizeof(char),_get_zero_arr_size(_tmp41,2));}),((
struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(int,void*),int env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Port_ctype2stringptr,deep,ts)));}static struct _tagged_arr
Cyc_Port_cfields2string(int deep,struct Cyc_List_List*fs){return(struct _tagged_arr)
Cyc_strconcat_l(Cyc_Port_sep(({const char*_tmp42=";";_tag_arr(_tmp42,sizeof(char),
_get_zero_arr_size(_tmp42,2));}),((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
int,struct Cyc_Port_Cfield*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_cfield2stringptr,
deep,fs)));}static void*Cyc_Port_notconst_ct(){return(void*)1;}static void*Cyc_Port_const_ct(){
return(void*)0;}static void*Cyc_Port_thin_ct(){return(void*)2;}static void*Cyc_Port_fat_ct(){
return(void*)3;}static void*Cyc_Port_void_ct(){return(void*)4;}static void*Cyc_Port_zero_ct(){
return(void*)5;}static void*Cyc_Port_arith_ct(){return(void*)6;}static void*Cyc_Port_unknown_aggr_ct(
struct Cyc_List_List*fs){return(void*)({struct Cyc_Port_UnknownAggr_ct_struct*
_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=({struct Cyc_Port_UnknownAggr_ct_struct
_tmp44;_tmp44.tag=3;_tmp44.f1=fs;_tmp44.f2=0;_tmp44;});_tmp43;});}static void*Cyc_Port_known_aggr_ct(
struct Cyc_Absyn_Aggrdecl*ad,struct Cyc_List_List*fs){return(void*)({struct Cyc_Port_KnownAggr_ct_struct*
_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45[0]=({struct Cyc_Port_KnownAggr_ct_struct
_tmp46;_tmp46.tag=2;_tmp46.f1=ad;_tmp46.f2=fs;_tmp46;});_tmp45;});}static void*
Cyc_Port_ptr_ct(void*elt,void*qual,void*ptr_kind){return(void*)({struct Cyc_Port_Ptr_ct_struct*
_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47[0]=({struct Cyc_Port_Ptr_ct_struct _tmp48;
_tmp48.tag=0;_tmp48.f1=(void*)elt;_tmp48.f2=(void*)qual;_tmp48.f3=(void*)
ptr_kind;_tmp48;});_tmp47;});}static void*Cyc_Port_array_ct(void*elt,void*qual){
return(void*)({struct Cyc_Port_Array_ct_struct*_tmp49=_cycalloc(sizeof(*_tmp49));
_tmp49[0]=({struct Cyc_Port_Array_ct_struct _tmp4A;_tmp4A.tag=1;_tmp4A.f1=(void*)
elt;_tmp4A.f2=(void*)qual;_tmp4A;});_tmp49;});}static void*Cyc_Port_fn_ct(void*
return_type,struct Cyc_List_List*args){return(void*)({struct Cyc_Port_Fn_ct_struct*
_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B[0]=({struct Cyc_Port_Fn_ct_struct _tmp4C;
_tmp4C.tag=4;_tmp4C.f1=(void*)return_type;_tmp4C.f2=args;_tmp4C;});_tmp4B;});}
static void*Cyc_Port_var(){static int counter=0;return(void*)({struct Cyc_Port_Var_ct_struct*
_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D[0]=({struct Cyc_Port_Var_ct_struct _tmp4E;
_tmp4E.tag=5;_tmp4E.f1=({struct Cyc_Port_Cvar*_tmp4F=_cycalloc(sizeof(*_tmp4F));
_tmp4F->id=counter ++;_tmp4F->eq=0;_tmp4F->uppers=0;_tmp4F->lowers=0;_tmp4F;});
_tmp4E;});_tmp4D;});}static void*Cyc_Port_new_var(void*x){return Cyc_Port_var();}
static void*Cyc_Port_compress_ct(void*t){void*_tmp50=t;struct Cyc_Port_Cvar*_tmp51;
struct Cyc_Port_Cvar _tmp52;void**_tmp53;struct Cyc_List_List*_tmp54;struct Cyc_List_List*
_tmp55;void**_tmp56;_LL1E: if(_tmp50 <= (void*)7  || *((int*)_tmp50)!= 5)goto _LL20;
_tmp51=((struct Cyc_Port_Var_ct_struct*)_tmp50)->f1;_tmp52=*_tmp51;_tmp53=_tmp52.eq;
_tmp54=_tmp52.uppers;_tmp55=_tmp52.lowers;_LL1F: if((unsigned int)_tmp53)return
Cyc_Port_compress_ct(*_tmp53);else{return t;}_LL20: if(_tmp50 <= (void*)7  || *((int*)
_tmp50)!= 3)goto _LL22;_tmp56=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmp50)->f2;
_LL21: if((unsigned int)_tmp56)return Cyc_Port_compress_ct(*_tmp56);else{return t;}
_LL22:;_LL23: return t;_LL1D:;}static int Cyc_Port_occurs(void*v,void*t){t=Cyc_Port_compress_ct(
t);if(t == v)return 1;{void*_tmp57=t;void*_tmp58;void*_tmp59;void*_tmp5A;void*
_tmp5B;void*_tmp5C;void*_tmp5D;struct Cyc_List_List*_tmp5E;struct Cyc_List_List*
_tmp5F;struct Cyc_List_List*_tmp60;_LL25: if(_tmp57 <= (void*)7  || *((int*)_tmp57)
!= 0)goto _LL27;_tmp58=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp57)->f1;_tmp59=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp57)->f2;_tmp5A=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp57)->f3;_LL26: return(Cyc_Port_occurs(v,_tmp58) || Cyc_Port_occurs(v,_tmp59))
 || Cyc_Port_occurs(v,_tmp5A);_LL27: if(_tmp57 <= (void*)7  || *((int*)_tmp57)!= 1)
goto _LL29;_tmp5B=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp57)->f1;_tmp5C=(
void*)((struct Cyc_Port_Array_ct_struct*)_tmp57)->f2;_LL28: return Cyc_Port_occurs(
v,_tmp5B) || Cyc_Port_occurs(v,_tmp5C);_LL29: if(_tmp57 <= (void*)7  || *((int*)
_tmp57)!= 4)goto _LL2B;_tmp5D=(void*)((struct Cyc_Port_Fn_ct_struct*)_tmp57)->f1;
_tmp5E=((struct Cyc_Port_Fn_ct_struct*)_tmp57)->f2;_LL2A: if(Cyc_Port_occurs(v,
_tmp5D))return 1;for(0;(unsigned int)_tmp5E;_tmp5E=_tmp5E->tl){if(Cyc_Port_occurs(
v,(void*)_tmp5E->hd))return 1;}return 0;_LL2B: if(_tmp57 <= (void*)7  || *((int*)
_tmp57)!= 2)goto _LL2D;_tmp5F=((struct Cyc_Port_KnownAggr_ct_struct*)_tmp57)->f2;
_LL2C: _tmp60=_tmp5F;goto _LL2E;_LL2D: if(_tmp57 <= (void*)7  || *((int*)_tmp57)!= 3)
goto _LL2F;_tmp60=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmp57)->f1;_LL2E: for(0;(
unsigned int)_tmp60;_tmp60=_tmp60->tl){if(Cyc_Port_occurs(v,(void*)((struct Cyc_Port_Cfield*)
_tmp60->hd)->qual) || Cyc_Port_occurs(v,(void*)((struct Cyc_Port_Cfield*)_tmp60->hd)->type))
return 1;}return 0;_LL2F:;_LL30: return 0;_LL24:;}}char Cyc_Port_Unify_ct[13]="\000\000\000\000Unify_ct\000";
static int Cyc_Port_leq(void*t1,void*t2);static void Cyc_Port_unify_cts(struct Cyc_List_List*
t1,struct Cyc_List_List*t2);static struct Cyc_List_List*Cyc_Port_merge_fields(
struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_subset);static void Cyc_Port_unify_ct(
void*t1,void*t2){t1=Cyc_Port_compress_ct(t1);t2=Cyc_Port_compress_ct(t2);if(t1 == 
t2)return;{struct _tuple3 _tmp62=({struct _tuple3 _tmp61;_tmp61.f1=t1;_tmp61.f2=t2;
_tmp61;});void*_tmp63;struct Cyc_Port_Cvar*_tmp64;void*_tmp65;struct Cyc_Port_Cvar*
_tmp66;void*_tmp67;void*_tmp68;void*_tmp69;void*_tmp6A;void*_tmp6B;void*_tmp6C;
void*_tmp6D;void*_tmp6E;void*_tmp6F;void*_tmp70;void*_tmp71;void*_tmp72;void*
_tmp73;void*_tmp74;void*_tmp75;void*_tmp76;struct Cyc_List_List*_tmp77;void*
_tmp78;void*_tmp79;struct Cyc_List_List*_tmp7A;void*_tmp7B;struct Cyc_Absyn_Aggrdecl*
_tmp7C;struct Cyc_List_List*_tmp7D;void*_tmp7E;struct Cyc_Absyn_Aggrdecl*_tmp7F;
struct Cyc_List_List*_tmp80;void*_tmp81;struct Cyc_List_List*_tmp82;void**_tmp83;
void***_tmp84;void*_tmp85;struct Cyc_List_List*_tmp86;void**_tmp87;void***_tmp88;
void*_tmp89;struct Cyc_List_List*_tmp8A;void**_tmp8B;void***_tmp8C;void*_tmp8D;
struct Cyc_Absyn_Aggrdecl*_tmp8E;struct Cyc_List_List*_tmp8F;void*_tmp90;struct Cyc_Absyn_Aggrdecl*
_tmp91;struct Cyc_List_List*_tmp92;void*_tmp93;struct Cyc_List_List*_tmp94;void**
_tmp95;void***_tmp96;_LL32: _tmp63=_tmp62.f1;if(_tmp63 <= (void*)7  || *((int*)
_tmp63)!= 5)goto _LL34;_tmp64=((struct Cyc_Port_Var_ct_struct*)_tmp63)->f1;_LL33:
if(!Cyc_Port_occurs(t1,t2)){{struct Cyc_List_List*_tmp97=_tmp64->uppers;for(0;(
unsigned int)_tmp97;_tmp97=_tmp97->tl){if(!Cyc_Port_leq(t2,(void*)_tmp97->hd))(
int)_throw((void*)Cyc_Port_Unify_ct);}}{struct Cyc_List_List*_tmp98=_tmp64->lowers;
for(0;(unsigned int)_tmp98;_tmp98=_tmp98->tl){if(!Cyc_Port_leq((void*)_tmp98->hd,
t2))(int)_throw((void*)Cyc_Port_Unify_ct);}}_tmp64->eq=({void**_tmp99=_cycalloc(
sizeof(*_tmp99));_tmp99[0]=t2;_tmp99;});return;}else{(int)_throw((void*)Cyc_Port_Unify_ct);}
_LL34: _tmp65=_tmp62.f2;if(_tmp65 <= (void*)7  || *((int*)_tmp65)!= 5)goto _LL36;
_tmp66=((struct Cyc_Port_Var_ct_struct*)_tmp65)->f1;_LL35: Cyc_Port_unify_ct(t2,t1);
return;_LL36: _tmp67=_tmp62.f1;if(_tmp67 <= (void*)7  || *((int*)_tmp67)!= 0)goto
_LL38;_tmp68=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp67)->f1;_tmp69=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmp67)->f2;_tmp6A=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp67)->f3;_tmp6B=_tmp62.f2;if(_tmp6B <= (void*)7  || *((int*)_tmp6B)!= 0)goto
_LL38;_tmp6C=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp6B)->f1;_tmp6D=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmp6B)->f2;_tmp6E=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp6B)->f3;_LL37: Cyc_Port_unify_ct(_tmp68,_tmp6C);Cyc_Port_unify_ct(_tmp69,
_tmp6D);Cyc_Port_unify_ct(_tmp6A,_tmp6E);return;_LL38: _tmp6F=_tmp62.f1;if(_tmp6F
<= (void*)7  || *((int*)_tmp6F)!= 1)goto _LL3A;_tmp70=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp6F)->f1;_tmp71=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp6F)->f2;_tmp72=
_tmp62.f2;if(_tmp72 <= (void*)7  || *((int*)_tmp72)!= 1)goto _LL3A;_tmp73=(void*)((
struct Cyc_Port_Array_ct_struct*)_tmp72)->f1;_tmp74=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp72)->f2;_LL39: Cyc_Port_unify_ct(_tmp70,_tmp73);Cyc_Port_unify_ct(_tmp71,
_tmp74);return;_LL3A: _tmp75=_tmp62.f1;if(_tmp75 <= (void*)7  || *((int*)_tmp75)!= 
4)goto _LL3C;_tmp76=(void*)((struct Cyc_Port_Fn_ct_struct*)_tmp75)->f1;_tmp77=((
struct Cyc_Port_Fn_ct_struct*)_tmp75)->f2;_tmp78=_tmp62.f2;if(_tmp78 <= (void*)7
 || *((int*)_tmp78)!= 4)goto _LL3C;_tmp79=(void*)((struct Cyc_Port_Fn_ct_struct*)
_tmp78)->f1;_tmp7A=((struct Cyc_Port_Fn_ct_struct*)_tmp78)->f2;_LL3B: Cyc_Port_unify_ct(
_tmp76,_tmp79);Cyc_Port_unify_cts(_tmp77,_tmp7A);return;_LL3C: _tmp7B=_tmp62.f1;
if(_tmp7B <= (void*)7  || *((int*)_tmp7B)!= 2)goto _LL3E;_tmp7C=((struct Cyc_Port_KnownAggr_ct_struct*)
_tmp7B)->f1;_tmp7D=((struct Cyc_Port_KnownAggr_ct_struct*)_tmp7B)->f2;_tmp7E=
_tmp62.f2;if(_tmp7E <= (void*)7  || *((int*)_tmp7E)!= 2)goto _LL3E;_tmp7F=((struct
Cyc_Port_KnownAggr_ct_struct*)_tmp7E)->f1;_tmp80=((struct Cyc_Port_KnownAggr_ct_struct*)
_tmp7E)->f2;_LL3D: if(_tmp7C == _tmp7F)return;else{(int)_throw((void*)Cyc_Port_Unify_ct);}
_LL3E: _tmp81=_tmp62.f1;if(_tmp81 <= (void*)7  || *((int*)_tmp81)!= 3)goto _LL40;
_tmp82=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmp81)->f1;_tmp83=((struct Cyc_Port_UnknownAggr_ct_struct*)
_tmp81)->f2;_tmp84=(void***)&((struct Cyc_Port_UnknownAggr_ct_struct*)_tmp81)->f2;
_tmp85=_tmp62.f2;if(_tmp85 <= (void*)7  || *((int*)_tmp85)!= 3)goto _LL40;_tmp86=((
struct Cyc_Port_UnknownAggr_ct_struct*)_tmp85)->f1;_tmp87=((struct Cyc_Port_UnknownAggr_ct_struct*)
_tmp85)->f2;_tmp88=(void***)&((struct Cyc_Port_UnknownAggr_ct_struct*)_tmp85)->f2;
_LL3F: {void*_tmp9A=Cyc_Port_unknown_aggr_ct(Cyc_Port_merge_fields(_tmp82,_tmp86,
1));*_tmp84=(*_tmp88=({void**_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=_tmp9A;
_tmp9B;}));return;}_LL40: _tmp89=_tmp62.f1;if(_tmp89 <= (void*)7  || *((int*)_tmp89)
!= 3)goto _LL42;_tmp8A=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmp89)->f1;_tmp8B=((
struct Cyc_Port_UnknownAggr_ct_struct*)_tmp89)->f2;_tmp8C=(void***)&((struct Cyc_Port_UnknownAggr_ct_struct*)
_tmp89)->f2;_tmp8D=_tmp62.f2;if(_tmp8D <= (void*)7  || *((int*)_tmp8D)!= 2)goto
_LL42;_tmp8E=((struct Cyc_Port_KnownAggr_ct_struct*)_tmp8D)->f1;_tmp8F=((struct
Cyc_Port_KnownAggr_ct_struct*)_tmp8D)->f2;_LL41: Cyc_Port_merge_fields(_tmp8F,
_tmp8A,0);*_tmp8C=({void**_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=t2;_tmp9C;});
return;_LL42: _tmp90=_tmp62.f1;if(_tmp90 <= (void*)7  || *((int*)_tmp90)!= 2)goto
_LL44;_tmp91=((struct Cyc_Port_KnownAggr_ct_struct*)_tmp90)->f1;_tmp92=((struct
Cyc_Port_KnownAggr_ct_struct*)_tmp90)->f2;_tmp93=_tmp62.f2;if(_tmp93 <= (void*)7
 || *((int*)_tmp93)!= 3)goto _LL44;_tmp94=((struct Cyc_Port_UnknownAggr_ct_struct*)
_tmp93)->f1;_tmp95=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmp93)->f2;_tmp96=(
void***)&((struct Cyc_Port_UnknownAggr_ct_struct*)_tmp93)->f2;_LL43: Cyc_Port_merge_fields(
_tmp92,_tmp94,0);*_tmp96=({void**_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D[0]=t1;
_tmp9D;});return;_LL44:;_LL45:(int)_throw((void*)Cyc_Port_Unify_ct);_LL31:;}}
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(
0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Port_unify_ct((void*)t1->hd,(void*)
t2->hd);}if(t1 != 0  || t2 != 0)(int)_throw((void*)Cyc_Port_Unify_ct);}static struct
Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*
fs2,int allow_f1_subset_f2){struct Cyc_List_List*common=0;{struct Cyc_List_List*
_tmp9E=fs2;for(0;(unsigned int)_tmp9E;_tmp9E=_tmp9E->tl){struct Cyc_Port_Cfield*
_tmp9F=(struct Cyc_Port_Cfield*)_tmp9E->hd;int found=0;{struct Cyc_List_List*_tmpA0=
fs1;for(0;(unsigned int)_tmpA0;_tmpA0=_tmpA0->tl){struct Cyc_Port_Cfield*_tmpA1=(
struct Cyc_Port_Cfield*)_tmpA0->hd;if(Cyc_strptrcmp(_tmpA1->f,_tmp9F->f)== 0){
common=({struct Cyc_List_List*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->hd=_tmpA1;
_tmpA2->tl=common;_tmpA2;});Cyc_Port_unify_ct((void*)_tmpA1->qual,(void*)_tmp9F->qual);
Cyc_Port_unify_ct((void*)_tmpA1->type,(void*)_tmp9F->type);found=1;break;}}}if(!
found){if(allow_f1_subset_f2)common=({struct Cyc_List_List*_tmpA3=_cycalloc(
sizeof(*_tmpA3));_tmpA3->hd=_tmp9F;_tmpA3->tl=common;_tmpA3;});else{(int)_throw((
void*)Cyc_Port_Unify_ct);}}}}{struct Cyc_List_List*_tmpA4=fs1;for(0;(unsigned int)
_tmpA4;_tmpA4=_tmpA4->tl){struct Cyc_Port_Cfield*_tmpA5=(struct Cyc_Port_Cfield*)
_tmpA4->hd;int found=0;{struct Cyc_List_List*_tmpA6=fs2;for(0;(unsigned int)_tmpA6;
_tmpA6=_tmpA6->tl){struct Cyc_Port_Cfield*_tmpA7=(struct Cyc_Port_Cfield*)_tmpA6->hd;
if(Cyc_strptrcmp(_tmpA5->f,_tmpA7->f))found=1;}}if(!found)common=({struct Cyc_List_List*
_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->hd=_tmpA5;_tmpA8->tl=common;_tmpA8;});}}
return common;}static int Cyc_Port_unifies(void*t1,void*t2){{struct _handler_cons
_tmpA9;_push_handler(& _tmpA9);{int _tmpAB=0;if(setjmp(_tmpA9.handler))_tmpAB=1;
if(!_tmpAB){Cyc_Port_unify_ct(t1,t2);;_pop_handler();}else{void*_tmpAA=(void*)
_exn_thrown;void*_tmpAD=_tmpAA;_LL47: if(_tmpAD != Cyc_Port_Unify_ct)goto _LL49;
_LL48: return 0;_LL49:;_LL4A:(void)_throw(_tmpAD);_LL46:;}}}return 1;}static struct
Cyc_List_List*Cyc_Port_insert_upper(void*v,void*t,struct Cyc_List_List*uppers){t=
Cyc_Port_compress_ct(t);{struct Cyc_List_List*_tmpAE=uppers;for(0;(unsigned int)
_tmpAE;_tmpAE=_tmpAE->tl){void*_tmpAF=Cyc_Port_compress_ct((void*)_tmpAE->hd);
if(t == _tmpAF)return uppers;{struct _tuple3 _tmpB1=({struct _tuple3 _tmpB0;_tmpB0.f1=
t;_tmpB0.f2=_tmpAF;_tmpB0;});void*_tmpB2;void*_tmpB3;void*_tmpB4;void*_tmpB5;
void*_tmpB6;void*_tmpB7;void*_tmpB8;void*_tmpB9;void*_tmpBA;void*_tmpBB;void*
_tmpBC;void*_tmpBD;void*_tmpBE;void*_tmpBF;_LL4C: _tmpB2=_tmpB1.f1;if((int)_tmpB2
!= 1)goto _LL4E;_LL4D: goto _LL4F;_LL4E: _tmpB3=_tmpB1.f1;if((int)_tmpB3 != 5)goto
_LL50;_LL4F: goto _LL51;_LL50: _tmpB4=_tmpB1.f1;if((int)_tmpB4 != 2)goto _LL52;_LL51:
goto _LL53;_LL52: _tmpB5=_tmpB1.f1;if((int)_tmpB5 != 3)goto _LL54;_LL53: goto _LL55;
_LL54: _tmpB6=_tmpB1.f1;if(_tmpB6 <= (void*)7  || *((int*)_tmpB6)!= 1)goto _LL56;
_LL55: goto _LL57;_LL56: _tmpB7=_tmpB1.f1;if(_tmpB7 <= (void*)7  || *((int*)_tmpB7)!= 
2)goto _LL58;_LL57: goto _LL59;_LL58: _tmpB8=_tmpB1.f1;if(_tmpB8 <= (void*)7  || *((
int*)_tmpB8)!= 4)goto _LL5A;_LL59: Cyc_Port_unifies(v,t);return uppers;_LL5A: _tmpB9=
_tmpB1.f1;if((int)_tmpB9 != 4)goto _LL5C;_LL5B: goto _LL5D;_LL5C: _tmpBA=_tmpB1.f1;
if((int)_tmpBA != 6)goto _LL5E;_tmpBB=_tmpB1.f2;if(_tmpBB <= (void*)7  || *((int*)
_tmpBB)!= 0)goto _LL5E;_LL5D: goto _LL5F;_LL5E: _tmpBC=_tmpB1.f1;if((int)_tmpBC != 6)
goto _LL60;_tmpBD=_tmpB1.f2;if((int)_tmpBD != 5)goto _LL60;_LL5F: goto _LL61;_LL60:
_tmpBE=_tmpB1.f1;if((int)_tmpBE != 6)goto _LL62;_tmpBF=_tmpB1.f2;if(_tmpBF <= (void*)
7  || *((int*)_tmpBF)!= 1)goto _LL62;_LL61: return uppers;_LL62:;_LL63: goto _LL4B;
_LL4B:;}}}return({struct Cyc_List_List*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->hd=(
void*)t;_tmpC0->tl=uppers;_tmpC0;});}static struct Cyc_List_List*Cyc_Port_insert_lower(
void*v,void*t,struct Cyc_List_List*lowers){t=Cyc_Port_compress_ct(t);{struct Cyc_List_List*
_tmpC1=lowers;for(0;(unsigned int)_tmpC1;_tmpC1=_tmpC1->tl){void*_tmpC2=Cyc_Port_compress_ct((
void*)_tmpC1->hd);if(t == _tmpC2)return lowers;{struct _tuple3 _tmpC4=({struct
_tuple3 _tmpC3;_tmpC3.f1=t;_tmpC3.f2=_tmpC2;_tmpC3;});void*_tmpC5;void*_tmpC6;
void*_tmpC7;void*_tmpC8;void*_tmpC9;void*_tmpCA;void*_tmpCB;void*_tmpCC;void*
_tmpCD;void*_tmpCE;void*_tmpCF;void*_tmpD0;void*_tmpD1;void*_tmpD2;void*_tmpD3;
_LL65: _tmpC5=_tmpC4.f1;if((int)_tmpC5 != 0)goto _LL67;_LL66: goto _LL68;_LL67: _tmpC6=
_tmpC4.f1;if((int)_tmpC6 != 2)goto _LL69;_LL68: goto _LL6A;_LL69: _tmpC7=_tmpC4.f1;
if((int)_tmpC7 != 3)goto _LL6B;_LL6A: goto _LL6C;_LL6B: _tmpC8=_tmpC4.f1;if((int)
_tmpC8 != 4)goto _LL6D;_LL6C: goto _LL6E;_LL6D: _tmpC9=_tmpC4.f1;if(_tmpC9 <= (void*)7
 || *((int*)_tmpC9)!= 2)goto _LL6F;_LL6E: goto _LL70;_LL6F: _tmpCA=_tmpC4.f1;if(
_tmpCA <= (void*)7  || *((int*)_tmpCA)!= 4)goto _LL71;_LL70: Cyc_Port_unifies(v,t);
return lowers;_LL71: _tmpCB=_tmpC4.f2;if((int)_tmpCB != 4)goto _LL73;_LL72: goto _LL74;
_LL73: _tmpCC=_tmpC4.f1;if((int)_tmpCC != 5)goto _LL75;_tmpCD=_tmpC4.f2;if((int)
_tmpCD != 6)goto _LL75;_LL74: goto _LL76;_LL75: _tmpCE=_tmpC4.f1;if((int)_tmpCE != 5)
goto _LL77;_tmpCF=_tmpC4.f2;if(_tmpCF <= (void*)7  || *((int*)_tmpCF)!= 0)goto _LL77;
_LL76: goto _LL78;_LL77: _tmpD0=_tmpC4.f1;if(_tmpD0 <= (void*)7  || *((int*)_tmpD0)!= 
0)goto _LL79;_tmpD1=_tmpC4.f2;if((int)_tmpD1 != 6)goto _LL79;_LL78: goto _LL7A;_LL79:
_tmpD2=_tmpC4.f1;if(_tmpD2 <= (void*)7  || *((int*)_tmpD2)!= 1)goto _LL7B;_tmpD3=
_tmpC4.f2;if((int)_tmpD3 != 6)goto _LL7B;_LL7A: return lowers;_LL7B:;_LL7C: goto _LL64;
_LL64:;}}}return({struct Cyc_List_List*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->hd=(
void*)t;_tmpD4->tl=lowers;_tmpD4;});}static int Cyc_Port_leq(void*t1,void*t2){if(
t1 == t2)return 1;t1=Cyc_Port_compress_ct(t1);t2=Cyc_Port_compress_ct(t2);{struct
_tuple3 _tmpD6=({struct _tuple3 _tmpD5;_tmpD5.f1=t1;_tmpD5.f2=t2;_tmpD5;});void*
_tmpD7;void*_tmpD8;void*_tmpD9;void*_tmpDA;void*_tmpDB;void*_tmpDC;void*_tmpDD;
void*_tmpDE;void*_tmpDF;void*_tmpE0;void*_tmpE1;void*_tmpE2;void*_tmpE3;void*
_tmpE4;void*_tmpE5;void*_tmpE6;void*_tmpE7;void*_tmpE8;void*_tmpE9;void*_tmpEA;
void*_tmpEB;void*_tmpEC;void*_tmpED;void*_tmpEE;void*_tmpEF;void*_tmpF0;void*
_tmpF1;void*_tmpF2;void*_tmpF3;void*_tmpF4;void*_tmpF5;void*_tmpF6;void*_tmpF7;
void*_tmpF8;void*_tmpF9;void*_tmpFA;void*_tmpFB;void*_tmpFC;void*_tmpFD;void*
_tmpFE;void*_tmpFF;void*_tmp100;void*_tmp101;void*_tmp102;void*_tmp103;struct Cyc_Port_Cvar*
_tmp104;void*_tmp105;struct Cyc_Port_Cvar*_tmp106;void*_tmp107;struct Cyc_Port_Cvar*
_tmp108;void*_tmp109;struct Cyc_Port_Cvar*_tmp10A;_LL7E: _tmpD7=_tmpD6.f1;if((int)
_tmpD7 != 1)goto _LL80;_tmpD8=_tmpD6.f2;if((int)_tmpD8 != 0)goto _LL80;_LL7F: return 1;
_LL80: _tmpD9=_tmpD6.f1;if((int)_tmpD9 != 0)goto _LL82;_tmpDA=_tmpD6.f2;if((int)
_tmpDA != 1)goto _LL82;_LL81: return 0;_LL82: _tmpDB=_tmpD6.f1;if(_tmpDB <= (void*)7
 || *((int*)_tmpDB)!= 5)goto _LL84;_tmpDC=_tmpD6.f2;if((int)_tmpDC != 0)goto _LL84;
_LL83: return 1;_LL84: _tmpDD=_tmpD6.f1;if(_tmpDD <= (void*)7  || *((int*)_tmpDD)!= 5)
goto _LL86;_tmpDE=_tmpD6.f2;if((int)_tmpDE != 4)goto _LL86;_LL85: return 1;_LL86:
_tmpDF=_tmpD6.f1;if((int)_tmpDF != 4)goto _LL88;_LL87: return 0;_LL88: _tmpE0=_tmpD6.f1;
if((int)_tmpE0 != 5)goto _LL8A;_tmpE1=_tmpD6.f2;if((int)_tmpE1 != 6)goto _LL8A;_LL89:
return 1;_LL8A: _tmpE2=_tmpD6.f1;if((int)_tmpE2 != 5)goto _LL8C;_tmpE3=_tmpD6.f2;if(
_tmpE3 <= (void*)7  || *((int*)_tmpE3)!= 0)goto _LL8C;_LL8B: return 1;_LL8C: _tmpE4=
_tmpD6.f1;if((int)_tmpE4 != 5)goto _LL8E;_tmpE5=_tmpD6.f2;if((int)_tmpE5 != 4)goto
_LL8E;_LL8D: return 1;_LL8E: _tmpE6=_tmpD6.f1;if(_tmpE6 <= (void*)7  || *((int*)
_tmpE6)!= 0)goto _LL90;_tmpE7=_tmpD6.f2;if((int)_tmpE7 != 6)goto _LL90;_LL8F: return
1;_LL90: _tmpE8=_tmpD6.f1;if(_tmpE8 <= (void*)7  || *((int*)_tmpE8)!= 0)goto _LL92;
_tmpE9=_tmpD6.f2;if((int)_tmpE9 != 4)goto _LL92;_LL91: return 1;_LL92: _tmpEA=_tmpD6.f1;
if(_tmpEA <= (void*)7  || *((int*)_tmpEA)!= 1)goto _LL94;_tmpEB=_tmpD6.f2;if((int)
_tmpEB != 6)goto _LL94;_LL93: return 1;_LL94: _tmpEC=_tmpD6.f1;if(_tmpEC <= (void*)7
 || *((int*)_tmpEC)!= 1)goto _LL96;_tmpED=_tmpD6.f2;if((int)_tmpED != 4)goto _LL96;
_LL95: return 1;_LL96: _tmpEE=_tmpD6.f1;if(_tmpEE <= (void*)7  || *((int*)_tmpEE)!= 0)
goto _LL98;_tmpEF=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpEE)->f1;_tmpF0=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmpEE)->f2;_tmpF1=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmpEE)->f3;_tmpF2=_tmpD6.f2;if(_tmpF2 <= (void*)7  || *((int*)_tmpF2)!= 0)goto
_LL98;_tmpF3=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpF2)->f1;_tmpF4=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmpF2)->f2;_tmpF5=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmpF2)->f3;_LL97: return(Cyc_Port_leq(_tmpEF,_tmpF3) && Cyc_Port_leq(_tmpF0,
_tmpF4)) && Cyc_Port_unifies(_tmpF1,_tmpF5);_LL98: _tmpF6=_tmpD6.f1;if(_tmpF6 <= (
void*)7  || *((int*)_tmpF6)!= 1)goto _LL9A;_tmpF7=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmpF6)->f1;_tmpF8=(void*)((struct Cyc_Port_Array_ct_struct*)_tmpF6)->f2;_tmpF9=
_tmpD6.f2;if(_tmpF9 <= (void*)7  || *((int*)_tmpF9)!= 1)goto _LL9A;_tmpFA=(void*)((
struct Cyc_Port_Array_ct_struct*)_tmpF9)->f1;_tmpFB=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmpF9)->f2;_LL99: return Cyc_Port_leq(_tmpF7,_tmpFA) && Cyc_Port_leq(_tmpF8,
_tmpFB);_LL9A: _tmpFC=_tmpD6.f1;if(_tmpFC <= (void*)7  || *((int*)_tmpFC)!= 1)goto
_LL9C;_tmpFD=(void*)((struct Cyc_Port_Array_ct_struct*)_tmpFC)->f1;_tmpFE=(void*)((
struct Cyc_Port_Array_ct_struct*)_tmpFC)->f2;_tmpFF=_tmpD6.f2;if(_tmpFF <= (void*)
7  || *((int*)_tmpFF)!= 0)goto _LL9C;_tmp100=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmpFF)->f1;_tmp101=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpFF)->f2;_tmp102=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpFF)->f3;_LL9B: return(Cyc_Port_leq(
_tmpFD,_tmp100) && Cyc_Port_leq(_tmpFE,_tmp101)) && Cyc_Port_unifies((void*)3,
_tmp102);_LL9C: _tmp103=_tmpD6.f1;if(_tmp103 <= (void*)7  || *((int*)_tmp103)!= 5)
goto _LL9E;_tmp104=((struct Cyc_Port_Var_ct_struct*)_tmp103)->f1;_tmp105=_tmpD6.f2;
if(_tmp105 <= (void*)7  || *((int*)_tmp105)!= 5)goto _LL9E;_tmp106=((struct Cyc_Port_Var_ct_struct*)
_tmp105)->f1;_LL9D: _tmp104->uppers=Cyc_Port_insert_upper(t1,t2,_tmp104->uppers);
_tmp106->lowers=Cyc_Port_insert_lower(t2,t1,_tmp106->lowers);return 1;_LL9E:
_tmp107=_tmpD6.f1;if(_tmp107 <= (void*)7  || *((int*)_tmp107)!= 5)goto _LLA0;
_tmp108=((struct Cyc_Port_Var_ct_struct*)_tmp107)->f1;_LL9F: _tmp108->uppers=Cyc_Port_insert_upper(
t1,t2,_tmp108->uppers);return 1;_LLA0: _tmp109=_tmpD6.f2;if(_tmp109 <= (void*)7  || *((
int*)_tmp109)!= 5)goto _LLA2;_tmp10A=((struct Cyc_Port_Var_ct_struct*)_tmp109)->f1;
_LLA1: _tmp10A->lowers=Cyc_Port_insert_lower(t2,t1,_tmp10A->lowers);return 1;_LLA2:;
_LLA3: return Cyc_Port_unifies(t1,t2);_LL7D:;}}struct Cyc_Port_GlobalCenv{struct Cyc_Dict_Dict*
typedef_dict;struct Cyc_Dict_Dict*struct_dict;struct Cyc_Dict_Dict*union_dict;void*
return_type;struct Cyc_List_List*qualifier_edits;struct Cyc_List_List*
pointer_edits;int porting;};struct Cyc_Port_Cenv{struct Cyc_Port_GlobalCenv*gcenv;
struct Cyc_Dict_Dict*var_dict;};static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv(){
struct Cyc_Port_GlobalCenv*g=({struct Cyc_Port_GlobalCenv*_tmp10C=_cycalloc(
sizeof(*_tmp10C));_tmp10C->typedef_dict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp10C->struct_dict=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp);_tmp10C->union_dict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp10C->qualifier_edits=
0;_tmp10C->pointer_edits=0;_tmp10C->porting=0;_tmp10C->return_type=(void*)Cyc_Port_void_ct();
_tmp10C;});return({struct Cyc_Port_Cenv*_tmp10B=_cycalloc(sizeof(*_tmp10B));
_tmp10B->gcenv=g;_tmp10B->var_dict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp10B;});}static
void*Cyc_Port_lookup_return_type(struct Cyc_Port_Cenv*env){return(void*)(env->gcenv)->return_type;}
static void*Cyc_Port_lookup_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmp10D;_push_handler(& _tmp10D);{int _tmp10F=0;if(setjmp(
_tmp10D.handler))_tmp10F=1;if(!_tmp10F){{void*_tmp111;struct _tuple3 _tmp110=*((
struct _tuple3*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,
n);_tmp111=_tmp110.f1;{void*_tmp112=_tmp111;_npop_handler(0);return _tmp112;}};
_pop_handler();}else{void*_tmp10E=(void*)_exn_thrown;void*_tmp114=_tmp10E;_LLA5:
if(_tmp114 != Cyc_Dict_Absent)goto _LLA7;_LLA6: return Cyc_Absyn_sint_typ;_LLA7:;
_LLA8:(void)_throw(_tmp114);_LLA4:;}}}static void*Cyc_Port_lookup_typedef_ctype(
struct Cyc_Port_Cenv*env,struct _tuple0*n){struct _handler_cons _tmp115;
_push_handler(& _tmp115);{int _tmp117=0;if(setjmp(_tmp115.handler))_tmp117=1;if(!
_tmp117){{void*_tmp119;struct _tuple3 _tmp118=*((struct _tuple3*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);_tmp119=
_tmp118.f2;{void*_tmp11A=_tmp119;_npop_handler(0);return _tmp11A;}};_pop_handler();}
else{void*_tmp116=(void*)_exn_thrown;void*_tmp11C=_tmp116;_LLAA: if(_tmp11C != Cyc_Dict_Absent)
goto _LLAC;_LLAB: return Cyc_Port_var();_LLAC:;_LLAD:(void)_throw(_tmp11C);_LLA9:;}}}
struct _tuple4{struct Cyc_Absyn_Aggrdecl*f1;struct Cyc_List_List*f2;};static struct
_tuple4 Cyc_Port_lookup_struct_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct Cyc_Core_Opt*_tmp11D=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct
_tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->struct_dict,n);if((unsigned int)
_tmp11D)return*((struct _tuple4*)_tmp11D->v);else{struct Cyc_Absyn_Aggrdecl*
_tmp11E=({struct Cyc_Absyn_Aggrdecl*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->kind=(
void*)((void*)0);_tmp120->sc=(void*)((void*)2);_tmp120->name=n;_tmp120->tvs=0;
_tmp120->impl=0;_tmp120->attributes=0;_tmp120;});struct _tuple4*p=({struct _tuple4*
_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->f1=_tmp11E;_tmp11F->f2=0;_tmp11F;});(
env->gcenv)->struct_dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tuple0*k,struct _tuple4*v))Cyc_Dict_insert)((env->gcenv)->struct_dict,n,p);
return*p;}}static struct _tuple4 Cyc_Port_lookup_union_decl(struct Cyc_Port_Cenv*env,
struct _tuple0*n){struct Cyc_Core_Opt*_tmp121=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->union_dict,n);if((
unsigned int)_tmp121)return*((struct _tuple4*)_tmp121->v);else{struct Cyc_Absyn_Aggrdecl*
_tmp122=({struct Cyc_Absyn_Aggrdecl*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->kind=(
void*)((void*)1);_tmp124->sc=(void*)((void*)2);_tmp124->name=n;_tmp124->tvs=0;
_tmp124->impl=0;_tmp124->attributes=0;_tmp124;});struct _tuple4*p=({struct _tuple4*
_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->f1=_tmp122;_tmp123->f2=0;_tmp123;});(
env->gcenv)->union_dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tuple0*k,struct _tuple4*v))Cyc_Dict_insert)((env->gcenv)->union_dict,n,p);return*
p;}}struct _tuple5{void*f1;struct _tuple3*f2;};static struct _tuple3 Cyc_Port_lookup_var(
struct Cyc_Port_Cenv*env,struct _tuple0*x){struct _handler_cons _tmp125;
_push_handler(& _tmp125);{int _tmp127=0;if(setjmp(_tmp125.handler))_tmp127=1;if(!
_tmp127){{struct _tuple3*_tmp129;struct _tuple5 _tmp128=*((struct _tuple5*(*)(struct
Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);_tmp129=
_tmp128.f2;{struct _tuple3 _tmp12A=*_tmp129;_npop_handler(0);return _tmp12A;}};
_pop_handler();}else{void*_tmp126=(void*)_exn_thrown;void*_tmp12C=_tmp126;_LLAF:
if(_tmp12C != Cyc_Dict_Absent)goto _LLB1;_LLB0: return({struct _tuple3 _tmp12D;
_tmp12D.f1=Cyc_Port_var();_tmp12D.f2=Cyc_Port_var();_tmp12D;});_LLB1:;_LLB2:(
void)_throw(_tmp12C);_LLAE:;}}}static int Cyc_Port_declared_var(struct Cyc_Port_Cenv*
env,struct _tuple0*x){return((int(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_member)(
env->var_dict,x);}static int Cyc_Port_isfn(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp12E;_push_handler(& _tmp12E);{int _tmp130=0;if(setjmp(
_tmp12E.handler))_tmp130=1;if(!_tmp130){{void*_tmp132;struct _tuple5 _tmp131=*((
struct _tuple5*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,
x);_tmp132=_tmp131.f1;LOOP: {void*_tmp133=_tmp132;struct _tuple0*_tmp134;_LLB4:
if(_tmp133 <= (void*)3  || *((int*)_tmp133)!= 16)goto _LLB6;_tmp134=((struct Cyc_Absyn_TypedefType_struct*)
_tmp133)->f1;_LLB5: _tmp132=Cyc_Port_lookup_typedef(env,_tmp134);goto LOOP;_LLB6:
if(_tmp133 <= (void*)3  || *((int*)_tmp133)!= 8)goto _LLB8;_LLB7: {int _tmp135=1;
_npop_handler(0);return _tmp135;}_LLB8:;_LLB9: {int _tmp136=0;_npop_handler(0);
return _tmp136;}_LLB3:;}};_pop_handler();}else{void*_tmp12F=(void*)_exn_thrown;
void*_tmp138=_tmp12F;_LLBB: if(_tmp138 != Cyc_Dict_Absent)goto _LLBD;_LLBC: return 0;
_LLBD:;_LLBE:(void)_throw(_tmp138);_LLBA:;}}}static int Cyc_Port_isarray(struct Cyc_Port_Cenv*
env,struct _tuple0*x){struct _handler_cons _tmp139;_push_handler(& _tmp139);{int
_tmp13B=0;if(setjmp(_tmp139.handler))_tmp13B=1;if(!_tmp13B){{void*_tmp13D;struct
_tuple5 _tmp13C=*((struct _tuple5*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_lookup)(
env->var_dict,x);_tmp13D=_tmp13C.f1;LOOP: {void*_tmp13E=_tmp13D;struct _tuple0*
_tmp13F;_LLC0: if(_tmp13E <= (void*)3  || *((int*)_tmp13E)!= 16)goto _LLC2;_tmp13F=((
struct Cyc_Absyn_TypedefType_struct*)_tmp13E)->f1;_LLC1: _tmp13D=Cyc_Port_lookup_typedef(
env,_tmp13F);goto LOOP;_LLC2: if(_tmp13E <= (void*)3  || *((int*)_tmp13E)!= 7)goto
_LLC4;_LLC3: {int _tmp140=1;_npop_handler(0);return _tmp140;}_LLC4:;_LLC5: {int
_tmp141=0;_npop_handler(0);return _tmp141;}_LLBF:;}};_pop_handler();}else{void*
_tmp13A=(void*)_exn_thrown;void*_tmp143=_tmp13A;_LLC7: if(_tmp143 != Cyc_Dict_Absent)
goto _LLC9;_LLC8: return 0;_LLC9:;_LLCA:(void)_throw(_tmp143);_LLC6:;}}}static void
Cyc_Port_add_return_type(struct Cyc_Port_Cenv*env,void*t){(void*)((env->gcenv)->return_type=(
void*)t);}static struct Cyc_Port_Cenv*Cyc_Port_add_var(struct Cyc_Port_Cenv*env,
struct _tuple0*x,void*t,void*qual,void*ctype){return({struct Cyc_Port_Cenv*_tmp144=
_cycalloc(sizeof(*_tmp144));_tmp144->gcenv=env->gcenv;_tmp144->var_dict=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,struct _tuple5*v))Cyc_Dict_insert)(
env->var_dict,x,({struct _tuple5*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->f1=
t;_tmp145->f2=({struct _tuple3*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->f1=
qual;_tmp146->f2=ctype;_tmp146;});_tmp145;}));_tmp144;});}static void Cyc_Port_add_typedef(
struct Cyc_Port_Cenv*env,struct _tuple0*n,void*t,void*ct){(env->gcenv)->typedef_dict=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,struct _tuple3*v))
Cyc_Dict_insert)((env->gcenv)->typedef_dict,n,({struct _tuple3*_tmp147=_cycalloc(
sizeof(*_tmp147));_tmp147->f1=t;_tmp147->f2=ct;_tmp147;}));}static void Cyc_Port_add_struct_decl(
struct Cyc_Port_Cenv*env,struct _tuple0*n,struct Cyc_Absyn_Aggrdecl*ad,struct Cyc_List_List*
cfs){struct _tuple4*p=({struct _tuple4*_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148->f1=
ad;_tmp148->f2=cfs;_tmp148;});(env->gcenv)->struct_dict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple0*k,struct _tuple4*v))Cyc_Dict_insert)((env->gcenv)->struct_dict,
n,p);}static void Cyc_Port_add_union_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n,
struct Cyc_Absyn_Aggrdecl*ad,struct Cyc_List_List*cfs){struct _tuple4*p=({struct
_tuple4*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->f1=ad;_tmp149->f2=cfs;
_tmp149;});(env->gcenv)->union_dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k,struct _tuple4*v))Cyc_Dict_insert)((env->gcenv)->union_dict,n,p);}
struct _tuple6{void*f1;void*f2;struct Cyc_Position_Segment*f3;};static void Cyc_Port_register_const_cvar(
struct Cyc_Port_Cenv*env,void*new_qual,void*orig_qual,struct Cyc_Position_Segment*
loc){(env->gcenv)->qualifier_edits=({struct Cyc_List_List*_tmp14A=_cycalloc(
sizeof(*_tmp14A));_tmp14A->hd=({struct _tuple6*_tmp14B=_cycalloc(sizeof(*_tmp14B));
_tmp14B->f1=new_qual;_tmp14B->f2=orig_qual;_tmp14B->f3=loc;_tmp14B;});_tmp14A->tl=(
env->gcenv)->qualifier_edits;_tmp14A;});}static void Cyc_Port_register_ptr_cvars(
struct Cyc_Port_Cenv*env,void*new_ptr,void*orig_ptr,struct Cyc_Position_Segment*
loc){(env->gcenv)->pointer_edits=({struct Cyc_List_List*_tmp14C=_cycalloc(sizeof(*
_tmp14C));_tmp14C->hd=({struct _tuple6*_tmp14D=_cycalloc(sizeof(*_tmp14D));
_tmp14D->f1=new_ptr;_tmp14D->f2=orig_ptr;_tmp14D->f3=loc;_tmp14D;});_tmp14C->tl=(
env->gcenv)->pointer_edits;_tmp14C;});}static void*Cyc_Port_gen_exp(struct Cyc_Port_Cenv*
env,struct Cyc_Absyn_Exp*e);static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,
struct Cyc_Absyn_Stmt*s);static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct
Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d);static void*Cyc_Port_type_to_ctype(
struct Cyc_Port_Cenv*env,void*t){void*_tmp14E=t;struct _tuple0*_tmp14F;struct Cyc_Absyn_PtrInfo
_tmp150;void*_tmp151;struct Cyc_Absyn_Tqual _tmp152;struct Cyc_Absyn_PtrAtts _tmp153;
struct Cyc_Absyn_Conref*_tmp154;struct Cyc_Absyn_Conref*_tmp155;struct Cyc_Absyn_Conref*
_tmp156;struct Cyc_Position_Segment*_tmp157;struct Cyc_Absyn_ArrayInfo _tmp158;void*
_tmp159;struct Cyc_Absyn_Tqual _tmp15A;struct Cyc_Absyn_FnInfo _tmp15B;void*_tmp15C;
struct Cyc_List_List*_tmp15D;struct Cyc_Absyn_AggrInfo _tmp15E;void*_tmp15F;struct
Cyc_List_List*_tmp160;_LLCC: if(_tmp14E <= (void*)3  || *((int*)_tmp14E)!= 16)goto
_LLCE;_tmp14F=((struct Cyc_Absyn_TypedefType_struct*)_tmp14E)->f1;_LLCD:(*_tmp14F).f1=(
void*)0;return Cyc_Port_lookup_typedef_ctype(env,_tmp14F);_LLCE: if((int)_tmp14E != 
0)goto _LLD0;_LLCF: return Cyc_Port_void_ct();_LLD0: if(_tmp14E <= (void*)3  || *((int*)
_tmp14E)!= 4)goto _LLD2;_tmp150=((struct Cyc_Absyn_PointerType_struct*)_tmp14E)->f1;
_tmp151=(void*)_tmp150.elt_typ;_tmp152=_tmp150.elt_tq;_tmp153=_tmp150.ptr_atts;
_tmp154=_tmp153.nullable;_tmp155=_tmp153.bounds;_tmp156=_tmp153.zero_term;
_tmp157=_tmp153.loc;_LLD1: {void*_tmp161=Cyc_Port_type_to_ctype(env,_tmp151);int
_tmp162=Cyc_Absyn_conref_val(_tmp155)== (void*)0;if((env->gcenv)->porting){void*
_tmp163=Cyc_Port_var();Cyc_Port_register_const_cvar(env,_tmp163,_tmp152.print_const?
Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp152.loc);if(_tmp152.print_const)
Cyc_Port_unify_ct(_tmp163,Cyc_Port_const_ct());{void*_tmp164=Cyc_Port_var();Cyc_Port_register_ptr_cvars(
env,_tmp164,_tmp162?Cyc_Port_fat_ct(): Cyc_Port_thin_ct(),_tmp157);if(_tmp162)Cyc_Port_unify_ct(
_tmp164,Cyc_Port_fat_ct());return Cyc_Port_ptr_ct(_tmp161,_tmp163,_tmp164);}}
else{return Cyc_Port_ptr_ct(_tmp161,_tmp152.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),
_tmp162?Cyc_Port_fat_ct(): Cyc_Port_thin_ct());}}_LLD2: if(_tmp14E <= (void*)3  || *((
int*)_tmp14E)!= 5)goto _LLD4;_LLD3: goto _LLD5;_LLD4: if((int)_tmp14E != 1)goto _LLD6;
_LLD5: goto _LLD7;_LLD6: if(_tmp14E <= (void*)3  || *((int*)_tmp14E)!= 6)goto _LLD8;
_LLD7: return Cyc_Port_arith_ct();_LLD8: if(_tmp14E <= (void*)3  || *((int*)_tmp14E)
!= 7)goto _LLDA;_tmp158=((struct Cyc_Absyn_ArrayType_struct*)_tmp14E)->f1;_tmp159=(
void*)_tmp158.elt_type;_tmp15A=_tmp158.tq;_LLD9: {void*_tmp165=Cyc_Port_type_to_ctype(
env,_tmp159);if((env->gcenv)->porting){void*_tmp166=Cyc_Port_var();Cyc_Port_register_const_cvar(
env,_tmp166,_tmp15A.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),
_tmp15A.loc);return Cyc_Port_array_ct(_tmp165,_tmp166);}else{return Cyc_Port_array_ct(
_tmp165,_tmp15A.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct());}}_LLDA:
if(_tmp14E <= (void*)3  || *((int*)_tmp14E)!= 8)goto _LLDC;_tmp15B=((struct Cyc_Absyn_FnType_struct*)
_tmp14E)->f1;_tmp15C=(void*)_tmp15B.ret_typ;_tmp15D=_tmp15B.args;_LLDB: {void*
_tmp167=Cyc_Port_type_to_ctype(env,_tmp15C);struct Cyc_List_List*cargs=0;for(0;
_tmp15D != 0;_tmp15D=_tmp15D->tl){struct Cyc_Absyn_Tqual _tmp169;void*_tmp16A;
struct _tuple1 _tmp168=*((struct _tuple1*)_tmp15D->hd);_tmp169=_tmp168.f2;_tmp16A=
_tmp168.f3;cargs=({struct Cyc_List_List*_tmp16B=_cycalloc(sizeof(*_tmp16B));
_tmp16B->hd=(void*)Cyc_Port_type_to_ctype(env,_tmp16A);_tmp16B->tl=cargs;_tmp16B;});}
return Cyc_Port_fn_ct(_tmp167,Cyc_List_imp_rev(cargs));}_LLDC: if(_tmp14E <= (void*)
3  || *((int*)_tmp14E)!= 10)goto _LLDE;_tmp15E=((struct Cyc_Absyn_AggrType_struct*)
_tmp14E)->f1;_tmp15F=(void*)_tmp15E.aggr_info;_LLDD: {void*_tmp16C=_tmp15F;void*
_tmp16D;struct _tuple0*_tmp16E;void*_tmp16F;struct _tuple0*_tmp170;struct Cyc_Absyn_Aggrdecl**
_tmp171;_LLE5: if(*((int*)_tmp16C)!= 0)goto _LLE7;_tmp16D=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp16C)->f1;if((int)_tmp16D != 0)goto _LLE7;_tmp16E=((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp16C)->f2;_LLE6:(*_tmp16E).f1=(void*)0;{struct Cyc_Absyn_Aggrdecl*_tmp173;
struct Cyc_List_List*_tmp174;struct _tuple4 _tmp172=Cyc_Port_lookup_struct_decl(env,
_tmp16E);_tmp173=_tmp172.f1;_tmp174=_tmp172.f2;return Cyc_Port_known_aggr_ct(
_tmp173,_tmp174);}_LLE7: if(*((int*)_tmp16C)!= 0)goto _LLE9;_tmp16F=(void*)((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp16C)->f1;if((int)_tmp16F != 1)goto _LLE9;
_tmp170=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp16C)->f2;_LLE8:(*_tmp170).f1=(
void*)0;{struct Cyc_Absyn_Aggrdecl*_tmp176;struct Cyc_List_List*_tmp177;struct
_tuple4 _tmp175=Cyc_Port_lookup_union_decl(env,_tmp170);_tmp176=_tmp175.f1;
_tmp177=_tmp175.f2;return Cyc_Port_known_aggr_ct(_tmp176,_tmp177);}_LLE9: if(*((
int*)_tmp16C)!= 1)goto _LLE4;_tmp171=((struct Cyc_Absyn_KnownAggr_struct*)_tmp16C)->f1;
_LLEA: {struct Cyc_Absyn_Aggrdecl*_tmp178=*_tmp171;void*_tmp179=(void*)_tmp178->kind;
_LLEC: if((int)_tmp179 != 0)goto _LLEE;_LLED: {struct Cyc_List_List*_tmp17B;struct
_tuple4 _tmp17A=Cyc_Port_lookup_struct_decl(env,_tmp178->name);_tmp17B=_tmp17A.f2;
return Cyc_Port_known_aggr_ct(_tmp178,_tmp17B);}_LLEE: if((int)_tmp179 != 1)goto
_LLEB;_LLEF: {struct Cyc_List_List*_tmp17D;struct _tuple4 _tmp17C=Cyc_Port_lookup_union_decl(
env,_tmp178->name);_tmp17D=_tmp17C.f2;return Cyc_Port_known_aggr_ct(_tmp178,
_tmp17D);}_LLEB:;}_LLE4:;}_LLDE: if(_tmp14E <= (void*)3  || *((int*)_tmp14E)!= 12)
goto _LLE0;_LLDF: return Cyc_Port_arith_ct();_LLE0: if(_tmp14E <= (void*)3  || *((int*)
_tmp14E)!= 13)goto _LLE2;_tmp160=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp14E)->f1;
_LLE1: for(0;(unsigned int)_tmp160;_tmp160=_tmp160->tl){(*((struct Cyc_Absyn_Enumfield*)
_tmp160->hd)->name).f1=(void*)0;env=Cyc_Port_add_var(env,((struct Cyc_Absyn_Enumfield*)
_tmp160->hd)->name,Cyc_Absyn_sint_typ,Cyc_Port_const_ct(),Cyc_Port_arith_ct());}
return Cyc_Port_arith_ct();_LLE2:;_LLE3: return Cyc_Port_arith_ct();_LLCB:;}static
void*Cyc_Port_gen_primop(struct Cyc_Port_Cenv*env,void*p,struct Cyc_List_List*args){
void*_tmp17E=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(args))->hd);
struct Cyc_List_List*_tmp17F=args->tl;void*_tmp180=p;_LLF1: if((int)_tmp180 != 0)
goto _LLF3;_LLF2: {void*_tmp181=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)
_check_null(_tmp17F))->hd);if(Cyc_Port_leq(_tmp17E,Cyc_Port_ptr_ct(Cyc_Port_var(),
Cyc_Port_var(),Cyc_Port_fat_ct()))){Cyc_Port_leq(_tmp181,Cyc_Port_arith_ct());
return _tmp17E;}else{if(Cyc_Port_leq(_tmp181,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),
Cyc_Port_fat_ct()))){Cyc_Port_leq(_tmp17E,Cyc_Port_arith_ct());return _tmp181;}
else{Cyc_Port_leq(_tmp17E,Cyc_Port_arith_ct());Cyc_Port_leq(_tmp181,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();}}}_LLF3: if((int)_tmp180 != 2)goto _LLF5;_LLF4: if(
_tmp17F == 0){Cyc_Port_leq(_tmp17E,Cyc_Port_arith_ct());return Cyc_Port_arith_ct();}
else{void*_tmp182=Cyc_Port_compress_ct((void*)_tmp17F->hd);if(Cyc_Port_leq(
_tmp17E,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct()))){if(
Cyc_Port_leq(_tmp182,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct())))
return Cyc_Port_arith_ct();else{Cyc_Port_leq(_tmp182,Cyc_Port_arith_ct());return
_tmp17E;}}else{Cyc_Port_leq(_tmp17E,Cyc_Port_arith_ct());Cyc_Port_leq(_tmp17E,
Cyc_Port_arith_ct());return Cyc_Port_arith_ct();}}_LLF5: if((int)_tmp180 != 1)goto
_LLF7;_LLF6: goto _LLF8;_LLF7: if((int)_tmp180 != 3)goto _LLF9;_LLF8: goto _LLFA;_LLF9:
if((int)_tmp180 != 4)goto _LLFB;_LLFA: goto _LLFC;_LLFB: if((int)_tmp180 != 5)goto
_LLFD;_LLFC: goto _LLFE;_LLFD: if((int)_tmp180 != 6)goto _LLFF;_LLFE: goto _LL100;_LLFF:
if((int)_tmp180 != 8)goto _LL101;_LL100: goto _LL102;_LL101: if((int)_tmp180 != 7)goto
_LL103;_LL102: goto _LL104;_LL103: if((int)_tmp180 != 10)goto _LL105;_LL104: goto
_LL106;_LL105: if((int)_tmp180 != 9)goto _LL107;_LL106: goto _LL108;_LL107: if((int)
_tmp180 != 13)goto _LL109;_LL108: goto _LL10A;_LL109: if((int)_tmp180 != 14)goto _LL10B;
_LL10A: goto _LL10C;_LL10B: if((int)_tmp180 != 15)goto _LL10D;_LL10C: goto _LL10E;
_LL10D: if((int)_tmp180 != 16)goto _LL10F;_LL10E: goto _LL110;_LL10F: if((int)_tmp180
!= 17)goto _LL111;_LL110: goto _LL112;_LL111: if((int)_tmp180 != 18)goto _LL113;_LL112:
Cyc_Port_leq(_tmp17E,Cyc_Port_arith_ct());Cyc_Port_leq((void*)((struct Cyc_List_List*)
_check_null(_tmp17F))->hd,Cyc_Port_arith_ct());return Cyc_Port_arith_ct();_LL113:
if((int)_tmp180 != 11)goto _LL115;_LL114: goto _LL116;_LL115: if((int)_tmp180 != 12)
goto _LL117;_LL116: Cyc_Port_leq(_tmp17E,Cyc_Port_arith_ct());return Cyc_Port_arith_ct();
_LL117: if((int)_tmp180 != 19)goto _LLF0;_LL118:({void*_tmp183[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp184=".size primop";
_tag_arr(_tmp184,sizeof(char),_get_zero_arr_size(_tmp184,13));}),_tag_arr(
_tmp183,sizeof(void*),0));});_LLF0:;}static struct _tuple3 Cyc_Port_gen_lhs(struct
Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){void*_tmp185=(void*)e->r;struct _tuple0*
_tmp186;struct _tuple0*_tmp187;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*
_tmp189;struct Cyc_Absyn_Exp*_tmp18A;struct Cyc_Absyn_Exp*_tmp18B;struct
_tagged_arr*_tmp18C;struct Cyc_Absyn_Exp*_tmp18D;struct _tagged_arr*_tmp18E;_LL11A:
if(*((int*)_tmp185)!= 1)goto _LL11C;_tmp186=((struct Cyc_Absyn_Var_e_struct*)
_tmp185)->f1;_LL11B: _tmp187=_tmp186;goto _LL11D;_LL11C: if(*((int*)_tmp185)!= 2)
goto _LL11E;_tmp187=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp185)->f1;_LL11D:(*
_tmp187).f1=(void*)0;return Cyc_Port_lookup_var(env,_tmp187);_LL11E: if(*((int*)
_tmp185)!= 25)goto _LL120;_tmp188=((struct Cyc_Absyn_Subscript_e_struct*)_tmp185)->f1;
_tmp189=((struct Cyc_Absyn_Subscript_e_struct*)_tmp185)->f2;_LL11F: {void*_tmp18F=
Cyc_Port_var();void*_tmp190=Cyc_Port_var();void*_tmp191=Cyc_Port_gen_exp(env,
_tmp188);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp189),Cyc_Port_arith_ct());{void*
_tmp192=Cyc_Port_ptr_ct(_tmp18F,_tmp190,Cyc_Port_fat_ct());Cyc_Port_leq(_tmp191,
_tmp192);return({struct _tuple3 _tmp193;_tmp193.f1=_tmp190;_tmp193.f2=_tmp18F;
_tmp193;});}}_LL120: if(*((int*)_tmp185)!= 22)goto _LL122;_tmp18A=((struct Cyc_Absyn_Deref_e_struct*)
_tmp185)->f1;_LL121: {void*_tmp194=Cyc_Port_var();void*_tmp195=Cyc_Port_var();
void*_tmp196=Cyc_Port_ptr_ct(_tmp194,_tmp195,Cyc_Port_var());Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp18A),_tmp196);return({struct _tuple3 _tmp197;_tmp197.f1=_tmp195;_tmp197.f2=
_tmp194;_tmp197;});}_LL122: if(*((int*)_tmp185)!= 23)goto _LL124;_tmp18B=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmp185)->f1;_tmp18C=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp185)->f2;_LL123: {void*_tmp198=Cyc_Port_var();void*_tmp199=Cyc_Port_var();
void*_tmp19A=Cyc_Port_gen_exp(env,_tmp18B);Cyc_Port_leq(Cyc_Port_gen_exp(env,
_tmp18B),Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp19B[1];_tmp19B[0]=({
struct Cyc_Port_Cfield*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->qual=(void*)
_tmp199;_tmp19C->f=_tmp18C;_tmp19C->type=(void*)_tmp198;_tmp19C;});((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp19B,sizeof(struct Cyc_Port_Cfield*),
1));})));return({struct _tuple3 _tmp19D;_tmp19D.f1=_tmp199;_tmp19D.f2=_tmp198;
_tmp19D;});}_LL124: if(*((int*)_tmp185)!= 24)goto _LL126;_tmp18D=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp185)->f1;_tmp18E=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp185)->f2;_LL125: {
void*_tmp19E=Cyc_Port_var();void*_tmp19F=Cyc_Port_var();void*_tmp1A0=Cyc_Port_gen_exp(
env,_tmp18D);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp18D),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(({
struct Cyc_Port_Cfield*_tmp1A1[1];_tmp1A1[0]=({struct Cyc_Port_Cfield*_tmp1A2=
_cycalloc(sizeof(*_tmp1A2));_tmp1A2->qual=(void*)_tmp19F;_tmp1A2->f=_tmp18E;
_tmp1A2->type=(void*)_tmp19E;_tmp1A2;});((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp1A1,sizeof(struct Cyc_Port_Cfield*),1));})),
Cyc_Port_var(),Cyc_Port_var()));return({struct _tuple3 _tmp1A3;_tmp1A3.f1=_tmp19F;
_tmp1A3.f2=_tmp19E;_tmp1A3;});}_LL126:;_LL127:({void*_tmp1A4[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)((struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp1A7;_tmp1A7.tag=0;_tmp1A7.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_exp2string(e));{void*_tmp1A5[1]={& _tmp1A7};Cyc_aprintf(({
const char*_tmp1A6="gen_lhs: %s";_tag_arr(_tmp1A6,sizeof(char),_get_zero_arr_size(
_tmp1A6,12));}),_tag_arr(_tmp1A5,sizeof(void*),1));}}),_tag_arr(_tmp1A4,sizeof(
void*),0));});_LL119:;}static void*Cyc_Port_gen_exp(struct Cyc_Port_Cenv*env,
struct Cyc_Absyn_Exp*e){void*_tmp1A8=(void*)e->r;void*_tmp1A9;void*_tmp1AA;void*
_tmp1AB;void*_tmp1AC;void*_tmp1AD;int _tmp1AE;void*_tmp1AF;void*_tmp1B0;void*
_tmp1B1;struct _tuple0*_tmp1B2;struct _tuple0*_tmp1B3;void*_tmp1B4;struct Cyc_List_List*
_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_Core_Opt*_tmp1B7;struct Cyc_Absyn_Exp*
_tmp1B8;struct Cyc_Absyn_Exp*_tmp1B9;void*_tmp1BA;struct Cyc_Absyn_Exp*_tmp1BB;
struct Cyc_Absyn_Exp*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BD;struct Cyc_Absyn_Exp*
_tmp1BE;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Exp*
_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_Exp*
_tmp1C4;struct Cyc_List_List*_tmp1C5;struct Cyc_Absyn_Exp*_tmp1C6;struct Cyc_List_List*
_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C8;void*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1CA;
struct Cyc_Absyn_Exp*_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CC;struct Cyc_Absyn_Exp*
_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CE;struct Cyc_Absyn_Exp*_tmp1CF;struct
_tagged_arr*_tmp1D0;struct Cyc_Absyn_Exp*_tmp1D1;struct _tagged_arr*_tmp1D2;struct
Cyc_Absyn_MallocInfo _tmp1D3;void**_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D5;struct Cyc_Absyn_Stmt*
_tmp1D6;_LL129: if(*((int*)_tmp1A8)!= 0)goto _LL12B;_tmp1A9=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1A8)->f1;if(_tmp1A9 <= (void*)1  || *((int*)_tmp1A9)!= 0)goto _LL12B;_LL12A:
goto _LL12C;_LL12B: if(*((int*)_tmp1A8)!= 0)goto _LL12D;_tmp1AA=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1A8)->f1;if(_tmp1AA <= (void*)1  || *((int*)_tmp1AA)!= 1)goto _LL12D;_LL12C:
goto _LL12E;_LL12D: if(*((int*)_tmp1A8)!= 0)goto _LL12F;_tmp1AB=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1A8)->f1;if(_tmp1AB <= (void*)1  || *((int*)_tmp1AB)!= 3)goto _LL12F;_LL12E:
goto _LL130;_LL12F: if(*((int*)_tmp1A8)!= 18)goto _LL131;_LL130: goto _LL132;_LL131:
if(*((int*)_tmp1A8)!= 19)goto _LL133;_LL132: goto _LL134;_LL133: if(*((int*)_tmp1A8)
!= 20)goto _LL135;_LL134: goto _LL136;_LL135: if(*((int*)_tmp1A8)!= 33)goto _LL137;
_LL136: goto _LL138;_LL137: if(*((int*)_tmp1A8)!= 34)goto _LL139;_LL138: goto _LL13A;
_LL139: if(*((int*)_tmp1A8)!= 0)goto _LL13B;_tmp1AC=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1A8)->f1;if(_tmp1AC <= (void*)1  || *((int*)_tmp1AC)!= 4)goto _LL13B;_LL13A:
return Cyc_Port_arith_ct();_LL13B: if(*((int*)_tmp1A8)!= 0)goto _LL13D;_tmp1AD=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp1A8)->f1;if(_tmp1AD <= (void*)1  || *((
int*)_tmp1AD)!= 2)goto _LL13D;_tmp1AE=((struct Cyc_Absyn_Int_c_struct*)_tmp1AD)->f2;
if(_tmp1AE != 0)goto _LL13D;_LL13C: return Cyc_Port_zero_ct();_LL13D: if(*((int*)
_tmp1A8)!= 0)goto _LL13F;_tmp1AF=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp1A8)->f1;
if(_tmp1AF <= (void*)1  || *((int*)_tmp1AF)!= 2)goto _LL13F;_LL13E: return Cyc_Port_arith_ct();
_LL13F: if(*((int*)_tmp1A8)!= 0)goto _LL141;_tmp1B0=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1A8)->f1;if(_tmp1B0 <= (void*)1  || *((int*)_tmp1B0)!= 5)goto _LL141;_LL140:
return Cyc_Port_ptr_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_fat_ct());
_LL141: if(*((int*)_tmp1A8)!= 0)goto _LL143;_tmp1B1=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1A8)->f1;if((int)_tmp1B1 != 0)goto _LL143;_LL142: return Cyc_Port_zero_ct();
_LL143: if(*((int*)_tmp1A8)!= 1)goto _LL145;_tmp1B2=((struct Cyc_Absyn_Var_e_struct*)
_tmp1A8)->f1;_LL144: _tmp1B3=_tmp1B2;goto _LL146;_LL145: if(*((int*)_tmp1A8)!= 2)
goto _LL147;_tmp1B3=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp1A8)->f1;_LL146:(*
_tmp1B3).f1=(void*)0;{void*_tmp1D8;struct _tuple3 _tmp1D7=Cyc_Port_lookup_var(env,
_tmp1B3);_tmp1D8=_tmp1D7.f2;if(Cyc_Port_isfn(env,_tmp1B3))return Cyc_Port_ptr_ct(
_tmp1D8,Cyc_Port_var(),Cyc_Port_var());else{if(Cyc_Port_isarray(env,_tmp1B3)){
void*_tmp1D9=Cyc_Port_var();void*_tmp1DA=Cyc_Port_var();void*_tmp1DB=Cyc_Port_array_ct(
_tmp1D9,_tmp1DA);Cyc_Port_unifies(_tmp1D8,_tmp1DB);{void*_tmp1DC=Cyc_Port_ptr_ct(
_tmp1D9,_tmp1DA,Cyc_Port_fat_ct());return _tmp1DC;}}else{return _tmp1D8;}}}_LL147:
if(*((int*)_tmp1A8)!= 3)goto _LL149;_tmp1B4=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1A8)->f1;_tmp1B5=((struct Cyc_Absyn_Primop_e_struct*)_tmp1A8)->f2;_LL148:
return Cyc_Port_gen_primop(env,_tmp1B4,((struct Cyc_List_List*(*)(void*(*f)(struct
Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Port_gen_exp,env,_tmp1B5));_LL149: if(*((int*)_tmp1A8)!= 4)
goto _LL14B;_tmp1B6=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1A8)->f1;_tmp1B7=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp1A8)->f2;_tmp1B8=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1A8)->f3;_LL14A: {void*_tmp1DE;void*_tmp1DF;struct _tuple3 _tmp1DD=Cyc_Port_gen_lhs(
env,_tmp1B6);_tmp1DE=_tmp1DD.f1;_tmp1DF=_tmp1DD.f2;Cyc_Port_unifies(_tmp1DE,Cyc_Port_notconst_ct());{
void*_tmp1E0=Cyc_Port_gen_exp(env,_tmp1B8);if((unsigned int)_tmp1B7){struct Cyc_List_List
x2=({struct Cyc_List_List _tmp1E3;_tmp1E3.hd=(void*)_tmp1E0;_tmp1E3.tl=0;_tmp1E3;});
struct Cyc_List_List x1=({struct Cyc_List_List _tmp1E2;_tmp1E2.hd=(void*)_tmp1DF;
_tmp1E2.tl=(struct Cyc_List_List*)& x2;_tmp1E2;});void*_tmp1E1=Cyc_Port_gen_primop(
env,(void*)_tmp1B7->v,(struct Cyc_List_List*)& x1);Cyc_Port_leq(_tmp1E1,_tmp1DF);
return _tmp1DF;}else{Cyc_Port_leq(_tmp1E0,_tmp1DF);return _tmp1DF;}}}_LL14B: if(*((
int*)_tmp1A8)!= 5)goto _LL14D;_tmp1B9=((struct Cyc_Absyn_Increment_e_struct*)
_tmp1A8)->f1;_tmp1BA=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1A8)->f2;
_LL14C: {void*_tmp1E5;void*_tmp1E6;struct _tuple3 _tmp1E4=Cyc_Port_gen_lhs(env,
_tmp1B9);_tmp1E5=_tmp1E4.f1;_tmp1E6=_tmp1E4.f2;Cyc_Port_unifies(_tmp1E5,Cyc_Port_notconst_ct());
Cyc_Port_leq(_tmp1E6,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct()));
Cyc_Port_leq(_tmp1E6,Cyc_Port_arith_ct());return _tmp1E6;}_LL14D: if(*((int*)
_tmp1A8)!= 6)goto _LL14F;_tmp1BB=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1A8)->f1;
_tmp1BC=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1A8)->f2;_tmp1BD=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1A8)->f3;_LL14E: {void*_tmp1E7=Cyc_Port_var();Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp1BB),Cyc_Port_arith_ct());Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp1BC),
_tmp1E7);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp1BD),_tmp1E7);return _tmp1E7;}
_LL14F: if(*((int*)_tmp1A8)!= 7)goto _LL151;_tmp1BE=((struct Cyc_Absyn_And_e_struct*)
_tmp1A8)->f1;_tmp1BF=((struct Cyc_Absyn_And_e_struct*)_tmp1A8)->f2;_LL150: _tmp1C0=
_tmp1BE;_tmp1C1=_tmp1BF;goto _LL152;_LL151: if(*((int*)_tmp1A8)!= 8)goto _LL153;
_tmp1C0=((struct Cyc_Absyn_Or_e_struct*)_tmp1A8)->f1;_tmp1C1=((struct Cyc_Absyn_Or_e_struct*)
_tmp1A8)->f2;_LL152: Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp1C0),Cyc_Port_arith_ct());
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp1C1),Cyc_Port_arith_ct());return Cyc_Port_arith_ct();
_LL153: if(*((int*)_tmp1A8)!= 9)goto _LL155;_tmp1C2=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1A8)->f1;_tmp1C3=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1A8)->f2;_LL154: Cyc_Port_gen_exp(
env,_tmp1C2);return Cyc_Port_gen_exp(env,_tmp1C3);_LL155: if(*((int*)_tmp1A8)!= 10)
goto _LL157;_tmp1C4=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1A8)->f1;_tmp1C5=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1A8)->f2;_LL156: _tmp1C6=_tmp1C4;
_tmp1C7=_tmp1C5;goto _LL158;_LL157: if(*((int*)_tmp1A8)!= 11)goto _LL159;_tmp1C6=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp1A8)->f1;_tmp1C7=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1A8)->f2;_LL158: {void*_tmp1E8=Cyc_Port_var();void*_tmp1E9=Cyc_Port_gen_exp(
env,_tmp1C6);struct Cyc_List_List*_tmp1EA=((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Port_gen_exp,env,_tmp1C7);struct Cyc_List_List*_tmp1EB=Cyc_List_map(
Cyc_Port_new_var,_tmp1EA);Cyc_Port_unifies(_tmp1E9,Cyc_Port_ptr_ct(Cyc_Port_fn_ct(
_tmp1E8,_tmp1EB),Cyc_Port_var(),Cyc_Port_var()));for(0;_tmp1EA != 0;(_tmp1EA=
_tmp1EA->tl,_tmp1EB=_tmp1EB->tl)){Cyc_Port_leq((void*)_tmp1EA->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp1EB))->hd);}return _tmp1E8;}_LL159: if(*((int*)
_tmp1A8)!= 12)goto _LL15B;_LL15A:({void*_tmp1EC[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1ED="throw";_tag_arr(
_tmp1ED,sizeof(char),_get_zero_arr_size(_tmp1ED,6));}),_tag_arr(_tmp1EC,sizeof(
void*),0));});_LL15B: if(*((int*)_tmp1A8)!= 13)goto _LL15D;_tmp1C8=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp1A8)->f1;_LL15C: return Cyc_Port_gen_exp(env,_tmp1C8);_LL15D: if(*((int*)
_tmp1A8)!= 14)goto _LL15F;_LL15E:({void*_tmp1EE[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1EF="instantiate";
_tag_arr(_tmp1EF,sizeof(char),_get_zero_arr_size(_tmp1EF,12));}),_tag_arr(
_tmp1EE,sizeof(void*),0));});_LL15F: if(*((int*)_tmp1A8)!= 15)goto _LL161;_tmp1C9=(
void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1A8)->f1;_tmp1CA=((struct Cyc_Absyn_Cast_e_struct*)
_tmp1A8)->f2;_LL160: {void*_tmp1F0=Cyc_Port_type_to_ctype(env,_tmp1C9);void*
_tmp1F1=Cyc_Port_gen_exp(env,_tmp1CA);Cyc_Port_leq(_tmp1F0,_tmp1F1);return
_tmp1F1;}_LL161: if(*((int*)_tmp1A8)!= 16)goto _LL163;_tmp1CB=((struct Cyc_Absyn_Address_e_struct*)
_tmp1A8)->f1;_LL162: {void*_tmp1F3;void*_tmp1F4;struct _tuple3 _tmp1F2=Cyc_Port_gen_lhs(
env,_tmp1CB);_tmp1F3=_tmp1F2.f1;_tmp1F4=_tmp1F2.f2;return Cyc_Port_ptr_ct(_tmp1F4,
_tmp1F3,Cyc_Port_var());}_LL163: if(*((int*)_tmp1A8)!= 25)goto _LL165;_tmp1CC=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp1A8)->f1;_tmp1CD=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1A8)->f2;_LL164: {void*_tmp1F5=Cyc_Port_var();Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp1CD),Cyc_Port_arith_ct());{void*_tmp1F6=Cyc_Port_gen_exp(env,_tmp1CC);
Cyc_Port_leq(_tmp1F6,Cyc_Port_ptr_ct(_tmp1F5,Cyc_Port_var(),Cyc_Port_fat_ct()));
return _tmp1F5;}}_LL165: if(*((int*)_tmp1A8)!= 22)goto _LL167;_tmp1CE=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1A8)->f1;_LL166: {void*_tmp1F7=Cyc_Port_var();Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp1CE),Cyc_Port_ptr_ct(_tmp1F7,Cyc_Port_var(),Cyc_Port_var()));return
_tmp1F7;}_LL167: if(*((int*)_tmp1A8)!= 23)goto _LL169;_tmp1CF=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1A8)->f1;_tmp1D0=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1A8)->f2;_LL168: {
void*_tmp1F8=Cyc_Port_var();void*_tmp1F9=Cyc_Port_gen_exp(env,_tmp1CF);Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp1CF),Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*
_tmp1FA[1];_tmp1FA[0]=({struct Cyc_Port_Cfield*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));
_tmp1FB->qual=(void*)Cyc_Port_var();_tmp1FB->f=_tmp1D0;_tmp1FB->type=(void*)
_tmp1F8;_tmp1FB;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1FA,sizeof(struct Cyc_Port_Cfield*),1));})));return _tmp1F8;}_LL169:
if(*((int*)_tmp1A8)!= 24)goto _LL16B;_tmp1D1=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1A8)->f1;_tmp1D2=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1A8)->f2;_LL16A: {
void*_tmp1FC=Cyc_Port_var();void*_tmp1FD=Cyc_Port_gen_exp(env,_tmp1D1);Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp1D1),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(({struct
Cyc_Port_Cfield*_tmp1FE[1];_tmp1FE[0]=({struct Cyc_Port_Cfield*_tmp1FF=_cycalloc(
sizeof(*_tmp1FF));_tmp1FF->qual=(void*)Cyc_Port_var();_tmp1FF->f=_tmp1D2;_tmp1FF->type=(
void*)_tmp1FC;_tmp1FF;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1FE,sizeof(struct Cyc_Port_Cfield*),1));})),Cyc_Port_var(),Cyc_Port_var()));
return _tmp1FC;}_LL16B: if(*((int*)_tmp1A8)!= 35)goto _LL16D;_tmp1D3=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp1A8)->f1;_tmp1D4=_tmp1D3.elt_type;_tmp1D5=_tmp1D3.num_elts;_LL16C: Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp1D5),Cyc_Port_arith_ct());{void*_tmp200=(unsigned int)
_tmp1D4?Cyc_Port_type_to_ctype(env,*_tmp1D4): Cyc_Port_var();return Cyc_Port_ptr_ct(
_tmp200,Cyc_Port_var(),Cyc_Port_fat_ct());}_LL16D: if(*((int*)_tmp1A8)!= 21)goto
_LL16F;_LL16E:({void*_tmp201[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp202="Gen";_tag_arr(_tmp202,
sizeof(char),_get_zero_arr_size(_tmp202,4));}),_tag_arr(_tmp201,sizeof(void*),0));});
_LL16F: if(*((int*)_tmp1A8)!= 17)goto _LL171;_LL170:({void*_tmp203[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp204="New_e";
_tag_arr(_tmp204,sizeof(char),_get_zero_arr_size(_tmp204,6));}),_tag_arr(_tmp203,
sizeof(void*),0));});_LL171: if(*((int*)_tmp1A8)!= 26)goto _LL173;_LL172:({void*
_tmp205[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp206="Tuple_e";_tag_arr(_tmp206,sizeof(char),_get_zero_arr_size(
_tmp206,8));}),_tag_arr(_tmp205,sizeof(void*),0));});_LL173: if(*((int*)_tmp1A8)
!= 27)goto _LL175;_LL174:({void*_tmp207[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp208="CompoundLit_e";
_tag_arr(_tmp208,sizeof(char),_get_zero_arr_size(_tmp208,14));}),_tag_arr(
_tmp207,sizeof(void*),0));});_LL175: if(*((int*)_tmp1A8)!= 28)goto _LL177;_LL176:({
void*_tmp209[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp20A="Array_e";_tag_arr(_tmp20A,sizeof(char),_get_zero_arr_size(
_tmp20A,8));}),_tag_arr(_tmp209,sizeof(void*),0));});_LL177: if(*((int*)_tmp1A8)
!= 29)goto _LL179;_LL178:({void*_tmp20B[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp20C="Comprehension_e";
_tag_arr(_tmp20C,sizeof(char),_get_zero_arr_size(_tmp20C,16));}),_tag_arr(
_tmp20B,sizeof(void*),0));});_LL179: if(*((int*)_tmp1A8)!= 30)goto _LL17B;_LL17A:({
void*_tmp20D[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp20E="Struct_e";_tag_arr(_tmp20E,sizeof(char),_get_zero_arr_size(
_tmp20E,9));}),_tag_arr(_tmp20D,sizeof(void*),0));});_LL17B: if(*((int*)_tmp1A8)
!= 31)goto _LL17D;_LL17C:({void*_tmp20F[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp210="AnonStruct_e";
_tag_arr(_tmp210,sizeof(char),_get_zero_arr_size(_tmp210,13));}),_tag_arr(
_tmp20F,sizeof(void*),0));});_LL17D: if(*((int*)_tmp1A8)!= 32)goto _LL17F;_LL17E:({
void*_tmp211[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp212="Tunion_e";_tag_arr(_tmp212,sizeof(char),_get_zero_arr_size(
_tmp212,9));}),_tag_arr(_tmp211,sizeof(void*),0));});_LL17F: if(*((int*)_tmp1A8)
!= 36)goto _LL181;_LL180:({void*_tmp213[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp214="UnresolvedMem_e";
_tag_arr(_tmp214,sizeof(char),_get_zero_arr_size(_tmp214,16));}),_tag_arr(
_tmp213,sizeof(void*),0));});_LL181: if(*((int*)_tmp1A8)!= 37)goto _LL128;_tmp1D6=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp1A8)->f1;_LL182:({void*_tmp215[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp216="StmtExp_e";
_tag_arr(_tmp216,sizeof(char),_get_zero_arr_size(_tmp216,10));}),_tag_arr(
_tmp215,sizeof(void*),0));});_LL128:;}static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*
env,struct Cyc_Absyn_Stmt*s){void*_tmp217=(void*)s->r;struct Cyc_Absyn_Exp*_tmp218;
struct Cyc_Absyn_Stmt*_tmp219;struct Cyc_Absyn_Stmt*_tmp21A;struct Cyc_Absyn_Exp*
_tmp21B;struct Cyc_Absyn_Exp*_tmp21C;struct Cyc_Absyn_Stmt*_tmp21D;struct Cyc_Absyn_Stmt*
_tmp21E;struct _tuple2 _tmp21F;struct Cyc_Absyn_Exp*_tmp220;struct Cyc_Absyn_Stmt*
_tmp221;struct Cyc_Absyn_Exp*_tmp222;struct _tuple2 _tmp223;struct Cyc_Absyn_Exp*
_tmp224;struct _tuple2 _tmp225;struct Cyc_Absyn_Exp*_tmp226;struct Cyc_Absyn_Stmt*
_tmp227;struct Cyc_Absyn_Exp*_tmp228;struct Cyc_List_List*_tmp229;struct Cyc_Absyn_Decl*
_tmp22A;struct Cyc_Absyn_Stmt*_tmp22B;struct Cyc_Absyn_Stmt*_tmp22C;struct Cyc_Absyn_Stmt*
_tmp22D;struct _tuple2 _tmp22E;struct Cyc_Absyn_Exp*_tmp22F;_LL184: if((int)_tmp217
!= 0)goto _LL186;_LL185: return;_LL186: if(_tmp217 <= (void*)1  || *((int*)_tmp217)!= 
0)goto _LL188;_tmp218=((struct Cyc_Absyn_Exp_s_struct*)_tmp217)->f1;_LL187: Cyc_Port_gen_exp(
env,_tmp218);return;_LL188: if(_tmp217 <= (void*)1  || *((int*)_tmp217)!= 1)goto
_LL18A;_tmp219=((struct Cyc_Absyn_Seq_s_struct*)_tmp217)->f1;_tmp21A=((struct Cyc_Absyn_Seq_s_struct*)
_tmp217)->f2;_LL189: Cyc_Port_gen_stmt(env,_tmp219);Cyc_Port_gen_stmt(env,_tmp21A);
return;_LL18A: if(_tmp217 <= (void*)1  || *((int*)_tmp217)!= 2)goto _LL18C;_tmp21B=((
struct Cyc_Absyn_Return_s_struct*)_tmp217)->f1;_LL18B: {void*_tmp230=Cyc_Port_lookup_return_type(
env);void*_tmp231=(unsigned int)_tmp21B?Cyc_Port_gen_exp(env,(struct Cyc_Absyn_Exp*)
_tmp21B): Cyc_Port_void_ct();Cyc_Port_leq(_tmp231,_tmp230);return;}_LL18C: if(
_tmp217 <= (void*)1  || *((int*)_tmp217)!= 3)goto _LL18E;_tmp21C=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp217)->f1;_tmp21D=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp217)->f2;_tmp21E=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp217)->f3;_LL18D: Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp21C),Cyc_Port_arith_ct());Cyc_Port_gen_stmt(env,_tmp21D);Cyc_Port_gen_stmt(
env,_tmp21E);return;_LL18E: if(_tmp217 <= (void*)1  || *((int*)_tmp217)!= 4)goto
_LL190;_tmp21F=((struct Cyc_Absyn_While_s_struct*)_tmp217)->f1;_tmp220=_tmp21F.f1;
_tmp221=((struct Cyc_Absyn_While_s_struct*)_tmp217)->f2;_LL18F: Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp220),Cyc_Port_arith_ct());Cyc_Port_gen_stmt(env,_tmp221);return;_LL190:
if(_tmp217 <= (void*)1  || *((int*)_tmp217)!= 5)goto _LL192;_LL191: goto _LL193;
_LL192: if(_tmp217 <= (void*)1  || *((int*)_tmp217)!= 6)goto _LL194;_LL193: goto
_LL195;_LL194: if(_tmp217 <= (void*)1  || *((int*)_tmp217)!= 7)goto _LL196;_LL195:
return;_LL196: if(_tmp217 <= (void*)1  || *((int*)_tmp217)!= 8)goto _LL198;_tmp222=((
struct Cyc_Absyn_For_s_struct*)_tmp217)->f1;_tmp223=((struct Cyc_Absyn_For_s_struct*)
_tmp217)->f2;_tmp224=_tmp223.f1;_tmp225=((struct Cyc_Absyn_For_s_struct*)_tmp217)->f3;
_tmp226=_tmp225.f1;_tmp227=((struct Cyc_Absyn_For_s_struct*)_tmp217)->f4;_LL197:
Cyc_Port_gen_exp(env,_tmp222);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp224),Cyc_Port_arith_ct());
Cyc_Port_gen_exp(env,_tmp226);Cyc_Port_gen_stmt(env,_tmp227);return;_LL198: if(
_tmp217 <= (void*)1  || *((int*)_tmp217)!= 9)goto _LL19A;_tmp228=((struct Cyc_Absyn_Switch_s_struct*)
_tmp217)->f1;_tmp229=((struct Cyc_Absyn_Switch_s_struct*)_tmp217)->f2;_LL199: Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp228),Cyc_Port_arith_ct());for(0;(unsigned int)_tmp229;
_tmp229=_tmp229->tl){Cyc_Port_gen_stmt(env,((struct Cyc_Absyn_Switch_clause*)
_tmp229->hd)->body);}return;_LL19A: if(_tmp217 <= (void*)1  || *((int*)_tmp217)!= 
10)goto _LL19C;_LL19B:({void*_tmp232[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp233="fallthru";_tag_arr(_tmp233,
sizeof(char),_get_zero_arr_size(_tmp233,9));}),_tag_arr(_tmp232,sizeof(void*),0));});
_LL19C: if(_tmp217 <= (void*)1  || *((int*)_tmp217)!= 11)goto _LL19E;_tmp22A=((
struct Cyc_Absyn_Decl_s_struct*)_tmp217)->f1;_tmp22B=((struct Cyc_Absyn_Decl_s_struct*)
_tmp217)->f2;_LL19D: env=Cyc_Port_gen_localdecl(env,_tmp22A);Cyc_Port_gen_stmt(
env,_tmp22B);return;_LL19E: if(_tmp217 <= (void*)1  || *((int*)_tmp217)!= 12)goto
_LL1A0;_tmp22C=((struct Cyc_Absyn_Label_s_struct*)_tmp217)->f2;_LL19F: Cyc_Port_gen_stmt(
env,_tmp22C);return;_LL1A0: if(_tmp217 <= (void*)1  || *((int*)_tmp217)!= 13)goto
_LL1A2;_tmp22D=((struct Cyc_Absyn_Do_s_struct*)_tmp217)->f1;_tmp22E=((struct Cyc_Absyn_Do_s_struct*)
_tmp217)->f2;_tmp22F=_tmp22E.f1;_LL1A1: Cyc_Port_gen_stmt(env,_tmp22D);Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp22F),Cyc_Port_arith_ct());return;_LL1A2: if(_tmp217 <= (
void*)1  || *((int*)_tmp217)!= 14)goto _LL1A4;_LL1A3:({void*_tmp234[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp235="try/catch";
_tag_arr(_tmp235,sizeof(char),_get_zero_arr_size(_tmp235,10));}),_tag_arr(
_tmp234,sizeof(void*),0));});_LL1A4: if(_tmp217 <= (void*)1  || *((int*)_tmp217)!= 
15)goto _LL1A6;_LL1A5:({void*_tmp236[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp237="region";_tag_arr(_tmp237,
sizeof(char),_get_zero_arr_size(_tmp237,7));}),_tag_arr(_tmp236,sizeof(void*),0));});
_LL1A6: if(_tmp217 <= (void*)1  || *((int*)_tmp217)!= 16)goto _LL183;_LL1A7:({void*
_tmp238[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp239="reset region";_tag_arr(_tmp239,sizeof(char),
_get_zero_arr_size(_tmp239,13));}),_tag_arr(_tmp238,sizeof(void*),0));});_LL183:;}
struct _tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Port_gen_initializer(
struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e){void*_tmp23A=(void*)e->r;
struct Cyc_List_List*_tmp23B;struct Cyc_List_List*_tmp23C;struct Cyc_List_List*
_tmp23D;void*_tmp23E;struct _tagged_arr _tmp23F;_LL1A9: if(*((int*)_tmp23A)!= 36)
goto _LL1AB;_tmp23B=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp23A)->f2;_LL1AA:
_tmp23C=_tmp23B;goto _LL1AC;_LL1AB: if(*((int*)_tmp23A)!= 28)goto _LL1AD;_tmp23C=((
struct Cyc_Absyn_Array_e_struct*)_tmp23A)->f1;_LL1AC: _tmp23D=_tmp23C;goto _LL1AE;
_LL1AD: if(*((int*)_tmp23A)!= 27)goto _LL1AF;_tmp23D=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp23A)->f2;_LL1AE: LOOP: {void*_tmp240=t;struct _tuple0*_tmp241;struct Cyc_Absyn_ArrayInfo
_tmp242;void*_tmp243;struct Cyc_Absyn_AggrInfo _tmp244;void*_tmp245;void*_tmp246;
struct _tuple0*_tmp247;struct Cyc_Absyn_AggrInfo _tmp248;void*_tmp249;struct Cyc_Absyn_Aggrdecl**
_tmp24A;struct Cyc_Absyn_Aggrdecl*_tmp24B;_LL1B4: if(_tmp240 <= (void*)3  || *((int*)
_tmp240)!= 16)goto _LL1B6;_tmp241=((struct Cyc_Absyn_TypedefType_struct*)_tmp240)->f1;
_LL1B5:(*_tmp241).f1=(void*)0;t=Cyc_Port_lookup_typedef(env,_tmp241);goto LOOP;
_LL1B6: if(_tmp240 <= (void*)3  || *((int*)_tmp240)!= 7)goto _LL1B8;_tmp242=((struct
Cyc_Absyn_ArrayType_struct*)_tmp240)->f1;_tmp243=(void*)_tmp242.elt_type;_LL1B7: {
void*_tmp24C=Cyc_Port_var();void*_tmp24D=Cyc_Port_var();void*_tmp24E=Cyc_Port_type_to_ctype(
env,_tmp243);for(0;_tmp23D != 0;_tmp23D=_tmp23D->tl){struct Cyc_List_List*_tmp250;
struct Cyc_Absyn_Exp*_tmp251;struct _tuple7 _tmp24F=*((struct _tuple7*)_tmp23D->hd);
_tmp250=_tmp24F.f1;_tmp251=_tmp24F.f2;if((unsigned int)_tmp250)({void*_tmp252[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp253="designators in initializers";_tag_arr(_tmp253,sizeof(char),
_get_zero_arr_size(_tmp253,28));}),_tag_arr(_tmp252,sizeof(void*),0));});{void*
_tmp254=Cyc_Port_gen_initializer(env,_tmp243,_tmp251);Cyc_Port_leq(_tmp254,
_tmp24C);}}return Cyc_Port_array_ct(_tmp24C,_tmp24D);}_LL1B8: if(_tmp240 <= (void*)
3  || *((int*)_tmp240)!= 10)goto _LL1BA;_tmp244=((struct Cyc_Absyn_AggrType_struct*)
_tmp240)->f1;_tmp245=(void*)_tmp244.aggr_info;if(*((int*)_tmp245)!= 0)goto _LL1BA;
_tmp246=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp245)->f1;if((int)
_tmp246 != 0)goto _LL1BA;_tmp247=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp245)->f2;
_LL1B9:(*_tmp247).f1=(void*)0;{struct Cyc_Absyn_Aggrdecl*_tmp256;struct _tuple4
_tmp255=Cyc_Port_lookup_struct_decl(env,_tmp247);_tmp256=_tmp255.f1;if((struct
Cyc_Absyn_Aggrdecl*)_tmp256 == 0)({void*_tmp257[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp258="struct is not yet defined";
_tag_arr(_tmp258,sizeof(char),_get_zero_arr_size(_tmp258,26));}),_tag_arr(
_tmp257,sizeof(void*),0));});_tmp24B=_tmp256;goto _LL1BB;}_LL1BA: if(_tmp240 <= (
void*)3  || *((int*)_tmp240)!= 10)goto _LL1BC;_tmp248=((struct Cyc_Absyn_AggrType_struct*)
_tmp240)->f1;_tmp249=(void*)_tmp248.aggr_info;if(*((int*)_tmp249)!= 1)goto _LL1BC;
_tmp24A=((struct Cyc_Absyn_KnownAggr_struct*)_tmp249)->f1;_tmp24B=*_tmp24A;_LL1BB: {
struct Cyc_List_List*_tmp259=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp24B->impl))->fields;
for(0;_tmp23D != 0;_tmp23D=_tmp23D->tl){struct Cyc_List_List*_tmp25B;struct Cyc_Absyn_Exp*
_tmp25C;struct _tuple7 _tmp25A=*((struct _tuple7*)_tmp23D->hd);_tmp25B=_tmp25A.f1;
_tmp25C=_tmp25A.f2;if((unsigned int)_tmp25B)({void*_tmp25D[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp25E="designators in initializers";
_tag_arr(_tmp25E,sizeof(char),_get_zero_arr_size(_tmp25E,28));}),_tag_arr(
_tmp25D,sizeof(void*),0));});{struct Cyc_Absyn_Aggrfield*_tmp25F=(struct Cyc_Absyn_Aggrfield*)((
struct Cyc_List_List*)_check_null(_tmp259))->hd;_tmp259=_tmp259->tl;{void*_tmp260=
Cyc_Port_gen_initializer(env,(void*)_tmp25F->type,_tmp25C);;}}}return Cyc_Port_type_to_ctype(
env,t);}_LL1BC:;_LL1BD:({void*_tmp261[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp262="bad type for aggregate initializer";
_tag_arr(_tmp262,sizeof(char),_get_zero_arr_size(_tmp262,35));}),_tag_arr(
_tmp261,sizeof(void*),0));});_LL1B3:;}_LL1AF: if(*((int*)_tmp23A)!= 0)goto _LL1B1;
_tmp23E=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp23A)->f1;if(_tmp23E <= (void*)
1  || *((int*)_tmp23E)!= 5)goto _LL1B1;_tmp23F=((struct Cyc_Absyn_String_c_struct*)
_tmp23E)->f1;_LL1B0: LOOP2: {void*_tmp263=t;struct _tuple0*_tmp264;_LL1BF: if(
_tmp263 <= (void*)3  || *((int*)_tmp263)!= 16)goto _LL1C1;_tmp264=((struct Cyc_Absyn_TypedefType_struct*)
_tmp263)->f1;_LL1C0:(*_tmp264).f1=(void*)0;t=Cyc_Port_lookup_typedef(env,_tmp264);
goto LOOP2;_LL1C1: if(_tmp263 <= (void*)3  || *((int*)_tmp263)!= 7)goto _LL1C3;_LL1C2:
return Cyc_Port_array_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct());_LL1C3:;_LL1C4:
return Cyc_Port_gen_exp(env,e);_LL1BE:;}_LL1B1:;_LL1B2: return Cyc_Port_gen_exp(env,
e);_LL1A8:;}static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*
env,struct Cyc_Absyn_Decl*d){void*_tmp265=(void*)d->r;struct Cyc_Absyn_Vardecl*
_tmp266;_LL1C6: if(_tmp265 <= (void*)2  || *((int*)_tmp265)!= 0)goto _LL1C8;_tmp266=((
struct Cyc_Absyn_Var_d_struct*)_tmp265)->f1;_LL1C7: {void*_tmp267=Cyc_Port_var();
void*q;if((env->gcenv)->porting){q=Cyc_Port_var();Cyc_Port_register_const_cvar(
env,q,(_tmp266->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(
_tmp266->tq).loc);}else{q=(_tmp266->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}(*
_tmp266->name).f1=(void*)0;env=Cyc_Port_add_var(env,_tmp266->name,(void*)_tmp266->type,
q,_tmp267);Cyc_Port_unifies(_tmp267,Cyc_Port_type_to_ctype(env,(void*)_tmp266->type));
if((unsigned int)_tmp266->initializer){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
_check_null(_tmp266->initializer);void*t2=Cyc_Port_gen_initializer(env,(void*)
_tmp266->type,e);Cyc_Port_leq(_tmp267,t2);}return env;}_LL1C8:;_LL1C9:({void*
_tmp268[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp269="non-local decl that isn't a variable";_tag_arr(_tmp269,
sizeof(char),_get_zero_arr_size(_tmp269,37));}),_tag_arr(_tmp268,sizeof(void*),0));});
_LL1C5:;}struct _tuple8{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static struct _tuple1*Cyc_Port_make_targ(struct _tuple8*arg){struct _tagged_arr*
_tmp26B;struct Cyc_Absyn_Tqual _tmp26C;void*_tmp26D;struct _tuple8 _tmp26A=*arg;
_tmp26B=_tmp26A.f1;_tmp26C=_tmp26A.f2;_tmp26D=_tmp26A.f3;return({struct _tuple1*
_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->f1=0;_tmp26E->f2=_tmp26C;_tmp26E->f3=
_tmp26D;_tmp26E;});}static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*
env,struct Cyc_List_List*ds);struct _tuple9{struct _tagged_arr*f1;void*f2;void*f3;
void*f4;};static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*env,
struct Cyc_Absyn_Decl*d){void*_tmp26F=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp270;
struct Cyc_Absyn_Fndecl*_tmp271;struct Cyc_Absyn_Typedefdecl*_tmp272;struct Cyc_Absyn_Aggrdecl*
_tmp273;struct Cyc_Absyn_Enumdecl*_tmp274;_LL1CB: if((int)_tmp26F != 0)goto _LL1CD;
_LL1CC:(env->gcenv)->porting=1;return env;_LL1CD: if((int)_tmp26F != 1)goto _LL1CF;
_LL1CE:(env->gcenv)->porting=1;return env;_LL1CF: if(_tmp26F <= (void*)2  || *((int*)
_tmp26F)!= 0)goto _LL1D1;_tmp270=((struct Cyc_Absyn_Var_d_struct*)_tmp26F)->f1;
_LL1D0:(*_tmp270->name).f1=(void*)0;if(Cyc_Port_declared_var(env,_tmp270->name)){
void*_tmp276;void*_tmp277;struct _tuple3 _tmp275=Cyc_Port_lookup_var(env,_tmp270->name);
_tmp276=_tmp275.f1;_tmp277=_tmp275.f2;{void*q2;if((env->gcenv)->porting  && !Cyc_Port_isfn(
env,_tmp270->name)){q2=Cyc_Port_var();Cyc_Port_register_const_cvar(env,q2,(
_tmp270->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(_tmp270->tq).loc);}
else{q2=(_tmp270->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
Cyc_Port_unifies(_tmp276,q2);Cyc_Port_unifies(_tmp277,Cyc_Port_type_to_ctype(env,(
void*)_tmp270->type));if((unsigned int)_tmp270->initializer){struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_check_null(_tmp270->initializer);Cyc_Port_leq(Cyc_Port_gen_initializer(
env,(void*)_tmp270->type,e),_tmp277);}return env;}}else{return Cyc_Port_gen_localdecl(
env,d);}_LL1D1: if(_tmp26F <= (void*)2  || *((int*)_tmp26F)!= 1)goto _LL1D3;_tmp271=((
struct Cyc_Absyn_Fn_d_struct*)_tmp26F)->f1;_LL1D2:(*_tmp271->name).f1=(void*)0;{
struct _tuple3*predeclared=0;if(Cyc_Port_declared_var(env,_tmp271->name))
predeclared=({struct _tuple3*_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278[0]=Cyc_Port_lookup_var(
env,_tmp271->name);_tmp278;});{void*_tmp279=(void*)_tmp271->ret_type;struct Cyc_List_List*
_tmp27A=_tmp271->args;struct Cyc_List_List*_tmp27B=((struct Cyc_List_List*(*)(
struct _tuple1*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_make_targ,
_tmp27A);struct Cyc_Absyn_FnType_struct*_tmp27C=({struct Cyc_Absyn_FnType_struct*
_tmp293=_cycalloc(sizeof(*_tmp293));_tmp293[0]=({struct Cyc_Absyn_FnType_struct
_tmp294;_tmp294.tag=8;_tmp294.f1=({struct Cyc_Absyn_FnInfo _tmp295;_tmp295.tvars=0;
_tmp295.effect=0;_tmp295.ret_typ=(void*)_tmp279;_tmp295.args=_tmp27B;_tmp295.c_varargs=
0;_tmp295.cyc_varargs=0;_tmp295.rgn_po=0;_tmp295.attributes=0;_tmp295;});_tmp294;});
_tmp293;});void*_tmp27D=Cyc_Port_type_to_ctype(env,_tmp279);struct Cyc_List_List*
c_args=0;struct Cyc_List_List*c_arg_types=0;{struct Cyc_List_List*_tmp27E=_tmp27A;
for(0;(unsigned int)_tmp27E;_tmp27E=_tmp27E->tl){struct _tagged_arr*_tmp280;
struct Cyc_Absyn_Tqual _tmp281;void*_tmp282;struct _tuple8 _tmp27F=*((struct _tuple8*)
_tmp27E->hd);_tmp280=_tmp27F.f1;_tmp281=_tmp27F.f2;_tmp282=_tmp27F.f3;{void*
_tmp283=Cyc_Port_type_to_ctype(env,_tmp282);void*tqv;if((env->gcenv)->porting){
tqv=Cyc_Port_var();Cyc_Port_register_const_cvar(env,tqv,_tmp281.print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct(),_tmp281.loc);}else{tqv=_tmp281.print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct();}c_args=({struct Cyc_List_List*_tmp284=_cycalloc(sizeof(*
_tmp284));_tmp284->hd=({struct _tuple9*_tmp285=_cycalloc(sizeof(*_tmp285));
_tmp285->f1=_tmp280;_tmp285->f2=_tmp282;_tmp285->f3=tqv;_tmp285->f4=_tmp283;
_tmp285;});_tmp284->tl=c_args;_tmp284;});c_arg_types=({struct Cyc_List_List*
_tmp286=_cycalloc(sizeof(*_tmp286));_tmp286->hd=(void*)_tmp283;_tmp286->tl=
c_arg_types;_tmp286;});}}}c_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(c_args);c_arg_types=Cyc_List_imp_rev(c_arg_types);{void*
_tmp287=Cyc_Port_fn_ct(_tmp27D,c_arg_types);if((unsigned int)predeclared){void*
_tmp289;void*_tmp28A;struct _tuple3 _tmp288=*predeclared;_tmp289=_tmp288.f1;
_tmp28A=_tmp288.f2;Cyc_Port_unifies(_tmp289,Cyc_Port_const_ct());Cyc_Port_unifies(
_tmp287,_tmp28A);}(*_tmp271->name).f1=(void*)0;env=Cyc_Port_add_var(env,_tmp271->name,(
void*)_tmp27C,Cyc_Port_const_ct(),_tmp287);Cyc_Port_add_return_type(env,_tmp27D);{
struct Cyc_Port_Cenv*_tmp28B=env;{struct Cyc_List_List*_tmp28C=c_args;for(0;(
unsigned int)_tmp28C;_tmp28C=_tmp28C->tl){struct _tagged_arr*_tmp28E;void*_tmp28F;
void*_tmp290;void*_tmp291;struct _tuple9 _tmp28D=*((struct _tuple9*)_tmp28C->hd);
_tmp28E=_tmp28D.f1;_tmp28F=_tmp28D.f2;_tmp290=_tmp28D.f3;_tmp291=_tmp28D.f4;
_tmp28B=Cyc_Port_add_var(_tmp28B,({struct _tuple0*_tmp292=_cycalloc(sizeof(*
_tmp292));_tmp292->f1=(void*)0;_tmp292->f2=_tmp28E;_tmp292;}),_tmp28F,_tmp290,
_tmp291);}}Cyc_Port_gen_stmt(_tmp28B,_tmp271->body);return env;}}}}_LL1D3: if(
_tmp26F <= (void*)2  || *((int*)_tmp26F)!= 7)goto _LL1D5;_tmp272=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp26F)->f1;_LL1D4: {void*_tmp296=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp272->defn))->v;void*_tmp297=Cyc_Port_type_to_ctype(env,_tmp296);(*_tmp272->name).f1=(
void*)0;Cyc_Port_add_typedef(env,_tmp272->name,_tmp296,_tmp297);return env;}
_LL1D5: if(_tmp26F <= (void*)2  || *((int*)_tmp26F)!= 4)goto _LL1D7;_tmp273=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp26F)->f1;_LL1D6: {struct _tuple0*_tmp298=_tmp273->name;(*
_tmp273->name).f1=(void*)0;{struct _tuple4 previous;struct Cyc_List_List*curr=0;
void(*add_decl)(struct Cyc_Port_Cenv*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,
struct Cyc_List_List*);{void*_tmp299=(void*)_tmp273->kind;_LL1DC: if((int)_tmp299
!= 0)goto _LL1DE;_LL1DD: previous=Cyc_Port_lookup_struct_decl(env,_tmp298);
add_decl=Cyc_Port_add_struct_decl;goto _LL1DB;_LL1DE: if((int)_tmp299 != 1)goto
_LL1DB;_LL1DF: previous=Cyc_Port_lookup_union_decl(env,_tmp298);add_decl=Cyc_Port_add_union_decl;
goto _LL1DB;_LL1DB:;}if((unsigned int)_tmp273->impl){struct Cyc_List_List*_tmp29A=
previous.f2;{struct Cyc_List_List*_tmp29B=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp273->impl))->fields;for(0;(unsigned int)_tmp29B;_tmp29B=_tmp29B->tl){
struct Cyc_Absyn_Aggrfield*_tmp29C=(struct Cyc_Absyn_Aggrfield*)_tmp29B->hd;void*
qv;if((env->gcenv)->porting){qv=Cyc_Port_var();Cyc_Port_register_const_cvar(env,
qv,(_tmp29C->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(_tmp29C->tq).loc);}
else{qv=(_tmp29C->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}{
void*_tmp29D=Cyc_Port_type_to_ctype(env,(void*)_tmp29C->type);if(_tmp29A != 0){
void*_tmp29F;void*_tmp2A0;struct Cyc_Port_Cfield _tmp29E=*((struct Cyc_Port_Cfield*)
_tmp29A->hd);_tmp29F=(void*)_tmp29E.qual;_tmp2A0=(void*)_tmp29E.type;_tmp29A=
_tmp29A->tl;Cyc_Port_unifies(qv,_tmp29F);Cyc_Port_unifies(_tmp29D,_tmp2A0);}curr=({
struct Cyc_List_List*_tmp2A1=_cycalloc(sizeof(*_tmp2A1));_tmp2A1->hd=({struct Cyc_Port_Cfield*
_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2->qual=(void*)qv;_tmp2A2->f=_tmp29C->name;
_tmp2A2->type=(void*)_tmp29D;_tmp2A2;});_tmp2A1->tl=curr;_tmp2A1;});}}}curr=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(curr);if(!((
unsigned int)previous.f2))add_decl(env,_tmp298,_tmp273,curr);}return env;}}_LL1D7:
if(_tmp26F <= (void*)2  || *((int*)_tmp26F)!= 6)goto _LL1D9;_tmp274=((struct Cyc_Absyn_Enum_d_struct*)
_tmp26F)->f1;_LL1D8:(*_tmp274->name).f1=(void*)0;if((unsigned int)_tmp274->fields){
struct Cyc_List_List*_tmp2A3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp274->fields))->v;for(0;(unsigned int)_tmp2A3;_tmp2A3=_tmp2A3->tl){(*((
struct Cyc_Absyn_Enumfield*)_tmp2A3->hd)->name).f1=(void*)0;env=Cyc_Port_add_var(
env,((struct Cyc_Absyn_Enumfield*)_tmp2A3->hd)->name,(void*)({struct Cyc_Absyn_EnumType_struct*
_tmp2A4=_cycalloc(sizeof(*_tmp2A4));_tmp2A4[0]=({struct Cyc_Absyn_EnumType_struct
_tmp2A5;_tmp2A5.tag=12;_tmp2A5.f1=_tmp274->name;_tmp2A5.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp274;_tmp2A5;});_tmp2A4;}),Cyc_Port_const_ct(),Cyc_Port_arith_ct());}}return
env;_LL1D9:;_LL1DA: if((env->gcenv)->porting)({void*_tmp2A6[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp2A7="Warning: Cyclone declaration found in code to be ported -- skipping.";
_tag_arr(_tmp2A7,sizeof(char),_get_zero_arr_size(_tmp2A7,69));}),_tag_arr(
_tmp2A6,sizeof(void*),0));});return env;_LL1CA:;}static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(
struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds){for(0;(unsigned int)ds;ds=ds->tl){
env=Cyc_Port_gen_topdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);}return env;}static
struct Cyc_List_List*Cyc_Port_gen_edits(struct Cyc_List_List*ds){struct Cyc_List_List*
edits=0;struct Cyc_Port_Cenv*env=Cyc_Port_gen_topdecls(Cyc_Port_initial_cenv(),ds);
struct Cyc_List_List*_tmp2A8=(env->gcenv)->pointer_edits;struct Cyc_List_List*
_tmp2A9=(env->gcenv)->qualifier_edits;{struct Cyc_List_List*_tmp2AA=_tmp2A8;for(0;(
unsigned int)_tmp2AA;_tmp2AA=_tmp2AA->tl){void*_tmp2AC;void*_tmp2AD;struct Cyc_Position_Segment*
_tmp2AE;struct _tuple6 _tmp2AB=*((struct _tuple6*)_tmp2AA->hd);_tmp2AC=_tmp2AB.f1;
_tmp2AD=_tmp2AB.f2;_tmp2AE=_tmp2AB.f3;Cyc_Port_unifies(_tmp2AC,_tmp2AD);}}{
struct Cyc_List_List*_tmp2AF=_tmp2A9;for(0;(unsigned int)_tmp2AF;_tmp2AF=_tmp2AF->tl){
void*_tmp2B1;void*_tmp2B2;struct Cyc_Position_Segment*_tmp2B3;struct _tuple6
_tmp2B0=*((struct _tuple6*)_tmp2AF->hd);_tmp2B1=_tmp2B0.f1;_tmp2B2=_tmp2B0.f2;
_tmp2B3=_tmp2B0.f3;Cyc_Port_unifies(_tmp2B1,_tmp2B2);}}for(0;(unsigned int)
_tmp2A8;_tmp2A8=_tmp2A8->tl){void*_tmp2B5;void*_tmp2B6;struct Cyc_Position_Segment*
_tmp2B7;struct _tuple6 _tmp2B4=*((struct _tuple6*)_tmp2A8->hd);_tmp2B5=_tmp2B4.f1;
_tmp2B6=_tmp2B4.f2;_tmp2B7=_tmp2B4.f3;if(!Cyc_Port_unifies(_tmp2B5,_tmp2B6) && (
unsigned int)_tmp2B7){void*_tmp2B8=Cyc_Port_compress_ct(_tmp2B6);_LL1E1: if((int)
_tmp2B8 != 2)goto _LL1E3;_LL1E2: edits=({struct Cyc_List_List*_tmp2B9=_cycalloc(
sizeof(*_tmp2B9));_tmp2B9->hd=({struct Cyc_Port_Edit*_tmp2BA=_cycalloc(sizeof(*
_tmp2BA));_tmp2BA->loc=_tmp2B7;_tmp2BA->old_string=({const char*_tmp2BC="?";
_tag_arr(_tmp2BC,sizeof(char),_get_zero_arr_size(_tmp2BC,2));});_tmp2BA->new_string=({
const char*_tmp2BB="*";_tag_arr(_tmp2BB,sizeof(char),_get_zero_arr_size(_tmp2BB,2));});
_tmp2BA;});_tmp2B9->tl=edits;_tmp2B9;});goto _LL1E0;_LL1E3: if((int)_tmp2B8 != 3)
goto _LL1E5;_LL1E4: edits=({struct Cyc_List_List*_tmp2BD=_cycalloc(sizeof(*_tmp2BD));
_tmp2BD->hd=({struct Cyc_Port_Edit*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE->loc=
_tmp2B7;_tmp2BE->old_string=({const char*_tmp2C0="*";_tag_arr(_tmp2C0,sizeof(char),
_get_zero_arr_size(_tmp2C0,2));});_tmp2BE->new_string=({const char*_tmp2BF="?";
_tag_arr(_tmp2BF,sizeof(char),_get_zero_arr_size(_tmp2BF,2));});_tmp2BE;});
_tmp2BD->tl=edits;_tmp2BD;});goto _LL1E0;_LL1E5:;_LL1E6: goto _LL1E0;_LL1E0:;}}for(
0;(unsigned int)_tmp2A9;_tmp2A9=_tmp2A9->tl){void*_tmp2C2;void*_tmp2C3;struct Cyc_Position_Segment*
_tmp2C4;struct _tuple6 _tmp2C1=*((struct _tuple6*)_tmp2A9->hd);_tmp2C2=_tmp2C1.f1;
_tmp2C3=_tmp2C1.f2;_tmp2C4=_tmp2C1.f3;if(!Cyc_Port_unifies(_tmp2C2,_tmp2C3) && (
unsigned int)_tmp2C4){void*_tmp2C5=Cyc_Port_compress_ct(_tmp2C3);_LL1E8: if((int)
_tmp2C5 != 1)goto _LL1EA;_LL1E9: edits=({struct Cyc_List_List*_tmp2C6=_cycalloc(
sizeof(*_tmp2C6));_tmp2C6->hd=({struct Cyc_Port_Edit*_tmp2C7=_cycalloc(sizeof(*
_tmp2C7));_tmp2C7->loc=_tmp2C4;_tmp2C7->old_string=({const char*_tmp2C9="const ";
_tag_arr(_tmp2C9,sizeof(char),_get_zero_arr_size(_tmp2C9,7));});_tmp2C7->new_string=({
const char*_tmp2C8="";_tag_arr(_tmp2C8,sizeof(char),_get_zero_arr_size(_tmp2C8,1));});
_tmp2C7;});_tmp2C6->tl=edits;_tmp2C6;});goto _LL1E7;_LL1EA: if((int)_tmp2C5 != 0)
goto _LL1EC;_LL1EB: edits=({struct Cyc_List_List*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));
_tmp2CA->hd=({struct Cyc_Port_Edit*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));_tmp2CB->loc=
_tmp2C4;_tmp2CB->old_string=({const char*_tmp2CD="";_tag_arr(_tmp2CD,sizeof(char),
_get_zero_arr_size(_tmp2CD,1));});_tmp2CB->new_string=({const char*_tmp2CC="const ";
_tag_arr(_tmp2CC,sizeof(char),_get_zero_arr_size(_tmp2CC,7));});_tmp2CB;});
_tmp2CA->tl=edits;_tmp2CA;});goto _LL1E7;_LL1EC:;_LL1ED: goto _LL1E7;_LL1E7:;}}
edits=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),
struct Cyc_List_List*x))Cyc_List_merge_sort)(Cyc_Port_cmp_edit,edits);while((
unsigned int)edits  && ((struct Cyc_Position_Segment*)_check_null(((struct Cyc_Port_Edit*)
edits->hd)->loc))->start == 0){edits=edits->tl;}return edits;}static struct Cyc_Position_Segment*
Cyc_Port_get_seg(struct Cyc_Port_Edit*e){return e->loc;}void Cyc_Port_port(struct
Cyc_List_List*ds){struct Cyc_List_List*_tmp2CE=Cyc_Port_gen_edits(ds);struct Cyc_List_List*
_tmp2CF=((struct Cyc_List_List*(*)(struct Cyc_Position_Segment*(*f)(struct Cyc_Port_Edit*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_get_seg,_tmp2CE);Cyc_Position_use_gcc_style_location=
0;{struct Cyc_List_List*_tmp2D0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_Position_strings_of_segments(_tmp2CF));for(0;(unsigned int)
_tmp2CE;(_tmp2CE=_tmp2CE->tl,_tmp2D0=_tmp2D0->tl)){struct Cyc_Position_Segment*
_tmp2D2;struct _tagged_arr _tmp2D3;struct _tagged_arr _tmp2D4;struct Cyc_Port_Edit
_tmp2D1=*((struct Cyc_Port_Edit*)_tmp2CE->hd);_tmp2D2=_tmp2D1.loc;_tmp2D3=_tmp2D1.old_string;
_tmp2D4=_tmp2D1.new_string;{struct _tagged_arr sloc=(struct _tagged_arr)*((struct
_tagged_arr*)((struct Cyc_List_List*)_check_null(_tmp2D0))->hd);({struct Cyc_String_pa_struct
_tmp2D9;_tmp2D9.tag=0;_tmp2D9.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp2D4);{
struct Cyc_String_pa_struct _tmp2D8;_tmp2D8.tag=0;_tmp2D8.f1=(struct _tagged_arr)((
struct _tagged_arr)_tmp2D3);{struct Cyc_String_pa_struct _tmp2D7;_tmp2D7.tag=0;
_tmp2D7.f1=(struct _tagged_arr)((struct _tagged_arr)sloc);{void*_tmp2D5[3]={&
_tmp2D7,& _tmp2D8,& _tmp2D9};Cyc_printf(({const char*_tmp2D6="%s: insert `%s' for `%s'\n";
_tag_arr(_tmp2D6,sizeof(char),_get_zero_arr_size(_tmp2D6,26));}),_tag_arr(
_tmp2D5,sizeof(void*),3));}}}});}}}}
