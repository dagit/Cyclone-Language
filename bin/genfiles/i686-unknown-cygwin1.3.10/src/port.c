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
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter_c(void(*f)(void*,void*),
void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*
x);extern char Cyc_List_Nth[8];int Cyc_strcmp(struct _tagged_arr s1,struct _tagged_arr
s2);int Cyc_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*s2);struct
_tagged_arr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Lineno_Pos{struct
_tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_List_List*Cyc_Position_strings_of_segments(
struct Cyc_List_List*);struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*
seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];
extern int Cyc_Position_use_gcc_style_location;struct Cyc_Absyn_Rel_n_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(struct
_tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();void*Cyc_Absyn_conref_val(struct
Cyc_Absyn_Conref*x);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_sint_typ;
void*Cyc_Absyn_string_typ(void*rgn);void*Cyc_Absyn_tagged_typ(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
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
Cyc_Port_RgnVar_ct_struct{int tag;struct _tagged_arr*f1;};struct Cyc_Port_Ptr_ct_struct{
int tag;void*f1;void*f2;void*f3;void*f4;void*f5;};struct Cyc_Port_Array_ct_struct{
int tag;void*f1;void*f2;void*f3;};struct _tuple4{struct Cyc_Absyn_Aggrdecl*f1;
struct Cyc_List_List*f2;};struct Cyc_Port_KnownAggr_ct_struct{int tag;struct _tuple4*
f1;};struct Cyc_Port_UnknownAggr_ct_struct{int tag;struct Cyc_List_List*f1;void**f2;
};struct Cyc_Port_Fn_ct_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Port_Var_ct_struct{
int tag;struct Cyc_Port_Cvar*f1;};static struct _tagged_arr Cyc_Port_ctypes2string(
int deep,struct Cyc_List_List*ts);static struct _tagged_arr Cyc_Port_cfields2string(
int deep,struct Cyc_List_List*ts);static struct _tagged_arr Cyc_Port_ctype2string(int
deep,void*t){void*_tmp0=t;struct _tagged_arr*_tmp1;void*_tmp2;void*_tmp3;void*
_tmp4;void*_tmp5;void*_tmp6;void*_tmp7;void*_tmp8;void*_tmp9;struct _tuple4*_tmpA;
struct _tuple4 _tmpB;struct Cyc_Absyn_Aggrdecl*_tmpC;struct Cyc_List_List*_tmpD;
struct Cyc_List_List*_tmpE;void**_tmpF;void*_tmp10;struct Cyc_List_List*_tmp11;
void*_tmp12;struct Cyc_List_List*_tmp13;struct Cyc_Port_Cvar*_tmp14;_LL1: if((int)
_tmp0 != 0)goto _LL3;_LL2: return({const char*_tmp15="const";_tag_arr(_tmp15,sizeof(
char),_get_zero_arr_size(_tmp15,6));});_LL3: if((int)_tmp0 != 1)goto _LL5;_LL4:
return({const char*_tmp16="notconst";_tag_arr(_tmp16,sizeof(char),
_get_zero_arr_size(_tmp16,9));});_LL5: if((int)_tmp0 != 2)goto _LL7;_LL6: return({
const char*_tmp17="thin";_tag_arr(_tmp17,sizeof(char),_get_zero_arr_size(_tmp17,5));});
_LL7: if((int)_tmp0 != 3)goto _LL9;_LL8: return({const char*_tmp18="fat";_tag_arr(
_tmp18,sizeof(char),_get_zero_arr_size(_tmp18,4));});_LL9: if((int)_tmp0 != 4)goto
_LLB;_LLA: return({const char*_tmp19="void";_tag_arr(_tmp19,sizeof(char),
_get_zero_arr_size(_tmp19,5));});_LLB: if((int)_tmp0 != 5)goto _LLD;_LLC: return({
const char*_tmp1A="zero";_tag_arr(_tmp1A,sizeof(char),_get_zero_arr_size(_tmp1A,5));});
_LLD: if((int)_tmp0 != 6)goto _LLF;_LLE: return({const char*_tmp1B="arith";_tag_arr(
_tmp1B,sizeof(char),_get_zero_arr_size(_tmp1B,6));});_LLF: if((int)_tmp0 != 7)goto
_LL11;_LL10: return({const char*_tmp1C="heap";_tag_arr(_tmp1C,sizeof(char),
_get_zero_arr_size(_tmp1C,5));});_LL11: if((int)_tmp0 != 8)goto _LL13;_LL12: return({
const char*_tmp1D="ZT";_tag_arr(_tmp1D,sizeof(char),_get_zero_arr_size(_tmp1D,3));});
_LL13: if((int)_tmp0 != 9)goto _LL15;_LL14: return({const char*_tmp1E="NZT";_tag_arr(
_tmp1E,sizeof(char),_get_zero_arr_size(_tmp1E,4));});_LL15: if(_tmp0 <= (void*)10
 || *((int*)_tmp0)!= 0)goto _LL17;_tmp1=((struct Cyc_Port_RgnVar_ct_struct*)_tmp0)->f1;
_LL16: return(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp21;_tmp21.tag=0;
_tmp21.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp1);{void*_tmp1F[1]={&
_tmp21};Cyc_aprintf(({const char*_tmp20="%s";_tag_arr(_tmp20,sizeof(char),
_get_zero_arr_size(_tmp20,3));}),_tag_arr(_tmp1F,sizeof(void*),1));}});_LL17: if(
_tmp0 <= (void*)10  || *((int*)_tmp0)!= 1)goto _LL19;_tmp2=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp0)->f1;_tmp3=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp0)->f2;_tmp4=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmp0)->f3;_tmp5=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp0)->f4;_tmp6=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp0)->f5;_LL18: return(
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Port_ctype2string(deep,_tmp6));{struct
Cyc_String_pa_struct _tmp27;_tmp27.tag=0;_tmp27.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Port_ctype2string(deep,_tmp5));{struct Cyc_String_pa_struct _tmp26;
_tmp26.tag=0;_tmp26.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Port_ctype2string(
deep,_tmp4));{struct Cyc_String_pa_struct _tmp25;_tmp25.tag=0;_tmp25.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Port_ctype2string(deep,_tmp3));{struct Cyc_String_pa_struct
_tmp24;_tmp24.tag=0;_tmp24.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Port_ctype2string(
deep,_tmp2));{void*_tmp22[5]={& _tmp24,& _tmp25,& _tmp26,& _tmp27,& _tmp28};Cyc_aprintf(({
const char*_tmp23="ptr(%s,%s,%s,%s,%s)";_tag_arr(_tmp23,sizeof(char),
_get_zero_arr_size(_tmp23,20));}),_tag_arr(_tmp22,sizeof(void*),5));}}}}}});
_LL19: if(_tmp0 <= (void*)10  || *((int*)_tmp0)!= 2)goto _LL1B;_tmp7=(void*)((struct
Cyc_Port_Array_ct_struct*)_tmp0)->f1;_tmp8=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp0)->f2;_tmp9=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp0)->f3;_LL1A:
return(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;_tmp2D.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Port_ctype2string(deep,_tmp9));{struct
Cyc_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Port_ctype2string(deep,_tmp8));{struct Cyc_String_pa_struct _tmp2B;
_tmp2B.tag=0;_tmp2B.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Port_ctype2string(
deep,_tmp7));{void*_tmp29[3]={& _tmp2B,& _tmp2C,& _tmp2D};Cyc_aprintf(({const char*
_tmp2A="array(%s,%s,%s)";_tag_arr(_tmp2A,sizeof(char),_get_zero_arr_size(_tmp2A,
16));}),_tag_arr(_tmp29,sizeof(void*),3));}}}});_LL1B: if(_tmp0 <= (void*)10  || *((
int*)_tmp0)!= 3)goto _LL1D;_tmpA=((struct Cyc_Port_KnownAggr_ct_struct*)_tmp0)->f1;
_tmpB=*_tmpA;_tmpC=_tmpB.f1;_tmpD=_tmpB.f2;_LL1C: {struct _tagged_arr s=(void*)
_tmpC->kind == (void*)0?({const char*_tmp37="struct";_tag_arr(_tmp37,sizeof(char),
_get_zero_arr_size(_tmp37,7));}):({const char*_tmp38="union";_tag_arr(_tmp38,
sizeof(char),_get_zero_arr_size(_tmp38,6));});if(!deep)return(struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp31;_tmp31.tag=0;_tmp31.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpC->name));{struct Cyc_String_pa_struct
_tmp30;_tmp30.tag=0;_tmp30.f1=(struct _tagged_arr)((struct _tagged_arr)s);{void*
_tmp2E[2]={& _tmp30,& _tmp31};Cyc_aprintf(({const char*_tmp2F="%s %s";_tag_arr(
_tmp2F,sizeof(char),_get_zero_arr_size(_tmp2F,6));}),_tag_arr(_tmp2E,sizeof(void*),
2));}}});else{return(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp36;_tmp36.tag=
0;_tmp36.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Port_cfields2string(0,
_tmpD));{struct Cyc_String_pa_struct _tmp35;_tmp35.tag=0;_tmp35.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpC->name));{struct Cyc_String_pa_struct
_tmp34;_tmp34.tag=0;_tmp34.f1=(struct _tagged_arr)((struct _tagged_arr)s);{void*
_tmp32[3]={& _tmp34,& _tmp35,& _tmp36};Cyc_aprintf(({const char*_tmp33="%s %s {%s}";
_tag_arr(_tmp33,sizeof(char),_get_zero_arr_size(_tmp33,11));}),_tag_arr(_tmp32,
sizeof(void*),3));}}}});}}_LL1D: if(_tmp0 <= (void*)10  || *((int*)_tmp0)!= 4)goto
_LL1F;_tmpE=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmp0)->f1;_tmpF=((struct Cyc_Port_UnknownAggr_ct_struct*)
_tmp0)->f2;if(_tmpF == 0)goto _LL1F;_tmp10=*_tmpF;_LL1E: return Cyc_Port_ctype2string(
deep,_tmp10);_LL1F: if(_tmp0 <= (void*)10  || *((int*)_tmp0)!= 4)goto _LL21;_tmp11=((
struct Cyc_Port_UnknownAggr_ct_struct*)_tmp0)->f1;_LL20: return(struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp3B;_tmp3B.tag=0;_tmp3B.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Port_cfields2string(deep,_tmp11));{void*_tmp39[1]={& _tmp3B};
Cyc_aprintf(({const char*_tmp3A="aggr {%s}";_tag_arr(_tmp3A,sizeof(char),
_get_zero_arr_size(_tmp3A,10));}),_tag_arr(_tmp39,sizeof(void*),1));}});_LL21:
if(_tmp0 <= (void*)10  || *((int*)_tmp0)!= 5)goto _LL23;_tmp12=(void*)((struct Cyc_Port_Fn_ct_struct*)
_tmp0)->f1;_tmp13=((struct Cyc_Port_Fn_ct_struct*)_tmp0)->f2;_LL22: return(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp3F;_tmp3F.tag=0;_tmp3F.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Port_ctype2string(deep,_tmp12));{struct Cyc_String_pa_struct
_tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Port_ctypes2string(
deep,_tmp13));{void*_tmp3C[2]={& _tmp3E,& _tmp3F};Cyc_aprintf(({const char*_tmp3D="fn(%s)->%s";
_tag_arr(_tmp3D,sizeof(char),_get_zero_arr_size(_tmp3D,11));}),_tag_arr(_tmp3C,
sizeof(void*),2));}}});_LL23: if(_tmp0 <= (void*)10  || *((int*)_tmp0)!= 6)goto _LL0;
_tmp14=((struct Cyc_Port_Var_ct_struct*)_tmp0)->f1;_LL24: if((unsigned int)_tmp14->eq)
return Cyc_Port_ctype2string(deep,*((void**)_check_null(_tmp14->eq)));else{if(!
deep  || _tmp14->uppers == 0  && _tmp14->lowers == 0)return(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp42;_tmp42.tag=1;_tmp42.f1=(unsigned int)_tmp14->id;{
void*_tmp40[1]={& _tmp42};Cyc_aprintf(({const char*_tmp41="var(%d)";_tag_arr(
_tmp41,sizeof(char),_get_zero_arr_size(_tmp41,8));}),_tag_arr(_tmp40,sizeof(void*),
1));}});else{return(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp47;_tmp47.tag=
0;_tmp47.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Port_ctypes2string(0,
_tmp14->uppers));{struct Cyc_Int_pa_struct _tmp46;_tmp46.tag=1;_tmp46.f1=(
unsigned int)_tmp14->id;{struct Cyc_String_pa_struct _tmp45;_tmp45.tag=0;_tmp45.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Port_ctypes2string(0,_tmp14->lowers));{
void*_tmp43[3]={& _tmp45,& _tmp46,& _tmp47};Cyc_aprintf(({const char*_tmp44="var([%s]<=%d<=[%s])";
_tag_arr(_tmp44,sizeof(char),_get_zero_arr_size(_tmp44,20));}),_tag_arr(_tmp43,
sizeof(void*),3));}}}});}}_LL0:;}static struct _tagged_arr*Cyc_Port_ctype2stringptr(
int deep,void*t){return({struct _tagged_arr*_tmp48=_cycalloc(sizeof(*_tmp48));
_tmp48[0]=Cyc_Port_ctype2string(deep,t);_tmp48;});}struct Cyc_List_List*Cyc_Port_sep(
struct _tagged_arr s,struct Cyc_List_List*xs){struct _tagged_arr*_tmp49=({struct
_tagged_arr*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D[0]=s;_tmp4D;});if(xs == 0)
return xs;{struct Cyc_List_List*_tmp4A=xs;struct Cyc_List_List*_tmp4B=xs->tl;for(0;
_tmp4B != 0;(_tmp4A=_tmp4B,_tmp4B=_tmp4B->tl)){((struct Cyc_List_List*)_check_null(
_tmp4A))->tl=({struct Cyc_List_List*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->hd=
_tmp49;_tmp4C->tl=_tmp4B;_tmp4C;});}return xs;}}static struct _tagged_arr*Cyc_Port_cfield2stringptr(
int deep,struct Cyc_Port_Cfield*f){struct _tagged_arr s=(struct _tagged_arr)({struct
Cyc_String_pa_struct _tmp53;_tmp53.tag=0;_tmp53.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Port_ctype2string(deep,(void*)f->type));{struct Cyc_String_pa_struct
_tmp52;_tmp52.tag=0;_tmp52.f1=(struct _tagged_arr)((struct _tagged_arr)*f->f);{
struct Cyc_String_pa_struct _tmp51;_tmp51.tag=0;_tmp51.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Port_ctype2string(deep,(void*)f->qual));{void*_tmp4F[3]={&
_tmp51,& _tmp52,& _tmp53};Cyc_aprintf(({const char*_tmp50="%s %s: %s";_tag_arr(
_tmp50,sizeof(char),_get_zero_arr_size(_tmp50,10));}),_tag_arr(_tmp4F,sizeof(
void*),3));}}}});return({struct _tagged_arr*_tmp4E=_cycalloc(sizeof(*_tmp4E));
_tmp4E[0]=s;_tmp4E;});}static struct _tagged_arr Cyc_Port_ctypes2string(int deep,
struct Cyc_List_List*ts){return(struct _tagged_arr)Cyc_strconcat_l(Cyc_Port_sep(({
const char*_tmp54=",";_tag_arr(_tmp54,sizeof(char),_get_zero_arr_size(_tmp54,2));}),((
struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(int,void*),int env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Port_ctype2stringptr,deep,ts)));}static struct _tagged_arr
Cyc_Port_cfields2string(int deep,struct Cyc_List_List*fs){return(struct _tagged_arr)
Cyc_strconcat_l(Cyc_Port_sep(({const char*_tmp55=";";_tag_arr(_tmp55,sizeof(char),
_get_zero_arr_size(_tmp55,2));}),((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
int,struct Cyc_Port_Cfield*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_cfield2stringptr,
deep,fs)));}static void*Cyc_Port_notconst_ct(){return(void*)1;}static void*Cyc_Port_const_ct(){
return(void*)0;}static void*Cyc_Port_thin_ct(){return(void*)2;}static void*Cyc_Port_fat_ct(){
return(void*)3;}static void*Cyc_Port_void_ct(){return(void*)4;}static void*Cyc_Port_zero_ct(){
return(void*)5;}static void*Cyc_Port_arith_ct(){return(void*)6;}static void*Cyc_Port_heap_ct(){
return(void*)7;}static void*Cyc_Port_zterm_ct(){return(void*)8;}static void*Cyc_Port_nozterm_ct(){
return(void*)9;}static void*Cyc_Port_rgnvar_ct(struct _tagged_arr*n){return(void*)({
struct Cyc_Port_RgnVar_ct_struct*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56[0]=({
struct Cyc_Port_RgnVar_ct_struct _tmp57;_tmp57.tag=0;_tmp57.f1=n;_tmp57;});_tmp56;});}
static void*Cyc_Port_unknown_aggr_ct(struct Cyc_List_List*fs){return(void*)({
struct Cyc_Port_UnknownAggr_ct_struct*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58[0]=({
struct Cyc_Port_UnknownAggr_ct_struct _tmp59;_tmp59.tag=4;_tmp59.f1=fs;_tmp59.f2=0;
_tmp59;});_tmp58;});}static void*Cyc_Port_known_aggr_ct(struct _tuple4*p){return(
void*)({struct Cyc_Port_KnownAggr_ct_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));
_tmp5A[0]=({struct Cyc_Port_KnownAggr_ct_struct _tmp5B;_tmp5B.tag=3;_tmp5B.f1=p;
_tmp5B;});_tmp5A;});}static void*Cyc_Port_ptr_ct(void*elt,void*qual,void*ptr_kind,
void*r,void*zt){return(void*)({struct Cyc_Port_Ptr_ct_struct*_tmp5C=_cycalloc(
sizeof(*_tmp5C));_tmp5C[0]=({struct Cyc_Port_Ptr_ct_struct _tmp5D;_tmp5D.tag=1;
_tmp5D.f1=(void*)elt;_tmp5D.f2=(void*)qual;_tmp5D.f3=(void*)ptr_kind;_tmp5D.f4=(
void*)r;_tmp5D.f5=(void*)zt;_tmp5D;});_tmp5C;});}static void*Cyc_Port_array_ct(
void*elt,void*qual,void*zt){return(void*)({struct Cyc_Port_Array_ct_struct*_tmp5E=
_cycalloc(sizeof(*_tmp5E));_tmp5E[0]=({struct Cyc_Port_Array_ct_struct _tmp5F;
_tmp5F.tag=2;_tmp5F.f1=(void*)elt;_tmp5F.f2=(void*)qual;_tmp5F.f3=(void*)zt;
_tmp5F;});_tmp5E;});}static void*Cyc_Port_fn_ct(void*return_type,struct Cyc_List_List*
args){return(void*)({struct Cyc_Port_Fn_ct_struct*_tmp60=_cycalloc(sizeof(*_tmp60));
_tmp60[0]=({struct Cyc_Port_Fn_ct_struct _tmp61;_tmp61.tag=5;_tmp61.f1=(void*)
return_type;_tmp61.f2=args;_tmp61;});_tmp60;});}static void*Cyc_Port_var(){static
int counter=0;return(void*)({struct Cyc_Port_Var_ct_struct*_tmp62=_cycalloc(
sizeof(*_tmp62));_tmp62[0]=({struct Cyc_Port_Var_ct_struct _tmp63;_tmp63.tag=6;
_tmp63.f1=({struct Cyc_Port_Cvar*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->id=
counter ++;_tmp64->eq=0;_tmp64->uppers=0;_tmp64->lowers=0;_tmp64;});_tmp63;});
_tmp62;});}static void*Cyc_Port_new_var(void*x){return Cyc_Port_var();}static
struct _tagged_arr*Cyc_Port_new_region_var(){static int counter=0;struct _tagged_arr
s=(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp68;_tmp68.tag=1;_tmp68.f1=(
unsigned int)counter ++;{void*_tmp66[1]={& _tmp68};Cyc_aprintf(({const char*_tmp67="`R%d";
_tag_arr(_tmp67,sizeof(char),_get_zero_arr_size(_tmp67,5));}),_tag_arr(_tmp66,
sizeof(void*),1));}});return({struct _tagged_arr*_tmp65=_cycalloc(sizeof(*_tmp65));
_tmp65[0]=s;_tmp65;});}static int Cyc_Port_unifies(void*t1,void*t2);static void*Cyc_Port_compress_ct(
void*t){void*_tmp69=t;struct Cyc_Port_Cvar*_tmp6A;struct Cyc_Port_Cvar _tmp6B;void**
_tmp6C;void***_tmp6D;struct Cyc_List_List*_tmp6E;struct Cyc_List_List*_tmp6F;void**
_tmp70;_LL26: if(_tmp69 <= (void*)10  || *((int*)_tmp69)!= 6)goto _LL28;_tmp6A=((
struct Cyc_Port_Var_ct_struct*)_tmp69)->f1;_tmp6B=*_tmp6A;_tmp6C=_tmp6B.eq;_tmp6D=(
void***)&(*((struct Cyc_Port_Var_ct_struct*)_tmp69)->f1).eq;_tmp6E=_tmp6B.uppers;
_tmp6F=_tmp6B.lowers;_LL27: {void**_tmp71=*_tmp6D;if((unsigned int)_tmp71){void*
r=Cyc_Port_compress_ct(*_tmp71);if(*_tmp71 != r)*_tmp6D=({void**_tmp72=_cycalloc(
sizeof(*_tmp72));_tmp72[0]=r;_tmp72;});return r;}for(0;_tmp6F != 0;_tmp6F=_tmp6F->tl){
void*_tmp73=(void*)_tmp6F->hd;_LL2D: if((int)_tmp73 != 0)goto _LL2F;_LL2E: goto _LL30;
_LL2F: if((int)_tmp73 != 9)goto _LL31;_LL30: goto _LL32;_LL31: if((int)_tmp73 != 4)goto
_LL33;_LL32: goto _LL34;_LL33: if(_tmp73 <= (void*)10  || *((int*)_tmp73)!= 3)goto
_LL35;_LL34: goto _LL36;_LL35: if(_tmp73 <= (void*)10  || *((int*)_tmp73)!= 5)goto
_LL37;_LL36:*_tmp6D=({void**_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=(void*)
_tmp6F->hd;_tmp74;});return(void*)_tmp6F->hd;_LL37:;_LL38: goto _LL2C;_LL2C:;}for(
0;_tmp6E != 0;_tmp6E=_tmp6E->tl){void*_tmp75=(void*)_tmp6E->hd;_LL3A: if((int)
_tmp75 != 1)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if((int)_tmp75 != 8)goto _LL3E;_LL3D:
goto _LL3F;_LL3E: if((int)_tmp75 != 5)goto _LL40;_LL3F: goto _LL41;_LL40: if(_tmp75 <= (
void*)10  || *((int*)_tmp75)!= 3)goto _LL42;_LL41: goto _LL43;_LL42: if(_tmp75 <= (
void*)10  || *((int*)_tmp75)!= 5)goto _LL44;_LL43:*_tmp6D=({void**_tmp76=_cycalloc(
sizeof(*_tmp76));_tmp76[0]=(void*)_tmp6E->hd;_tmp76;});return(void*)_tmp6E->hd;
_LL44:;_LL45: goto _LL39;_LL39:;}return t;}_LL28: if(_tmp69 <= (void*)10  || *((int*)
_tmp69)!= 4)goto _LL2A;_tmp70=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmp69)->f2;
_LL29: if((unsigned int)_tmp70)return Cyc_Port_compress_ct(*_tmp70);else{return t;}
_LL2A:;_LL2B: return t;_LL25:;}static void*Cyc_Port_inst(struct Cyc_Dict_Dict**
instenv,void*t){void*_tmp77=Cyc_Port_compress_ct(t);struct _tagged_arr*_tmp78;
void*_tmp79;void*_tmp7A;void*_tmp7B;void*_tmp7C;void*_tmp7D;void*_tmp7E;void*
_tmp7F;void*_tmp80;void*_tmp81;struct Cyc_List_List*_tmp82;_LL47: if((int)_tmp77 != 
0)goto _LL49;_LL48: goto _LL4A;_LL49: if((int)_tmp77 != 1)goto _LL4B;_LL4A: goto _LL4C;
_LL4B: if((int)_tmp77 != 2)goto _LL4D;_LL4C: goto _LL4E;_LL4D: if((int)_tmp77 != 3)goto
_LL4F;_LL4E: goto _LL50;_LL4F: if((int)_tmp77 != 4)goto _LL51;_LL50: goto _LL52;_LL51:
if((int)_tmp77 != 5)goto _LL53;_LL52: goto _LL54;_LL53: if((int)_tmp77 != 6)goto _LL55;
_LL54: goto _LL56;_LL55: if((int)_tmp77 != 8)goto _LL57;_LL56: goto _LL58;_LL57: if((int)
_tmp77 != 9)goto _LL59;_LL58: goto _LL5A;_LL59: if(_tmp77 <= (void*)10  || *((int*)
_tmp77)!= 4)goto _LL5B;_LL5A: goto _LL5C;_LL5B: if(_tmp77 <= (void*)10  || *((int*)
_tmp77)!= 3)goto _LL5D;_LL5C: goto _LL5E;_LL5D: if(_tmp77 <= (void*)10  || *((int*)
_tmp77)!= 6)goto _LL5F;_LL5E: goto _LL60;_LL5F: if((int)_tmp77 != 7)goto _LL61;_LL60:
return t;_LL61: if(_tmp77 <= (void*)10  || *((int*)_tmp77)!= 0)goto _LL63;_tmp78=((
struct Cyc_Port_RgnVar_ct_struct*)_tmp77)->f1;_LL62: if(!((int(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_member)(*instenv,_tmp78))*instenv=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(*instenv,
_tmp78,Cyc_Port_var());return((void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*
k))Cyc_Dict_lookup)(*instenv,_tmp78);_LL63: if(_tmp77 <= (void*)10  || *((int*)
_tmp77)!= 1)goto _LL65;_tmp79=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp77)->f1;
_tmp7A=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp77)->f2;_tmp7B=(void*)((struct
Cyc_Port_Ptr_ct_struct*)_tmp77)->f3;_tmp7C=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp77)->f4;_tmp7D=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp77)->f5;_LL64: {
void*_tmp85;void*_tmp86;struct _tuple3 _tmp84=({struct _tuple3 _tmp83;_tmp83.f1=Cyc_Port_inst(
instenv,_tmp79);_tmp83.f2=Cyc_Port_inst(instenv,_tmp7C);_tmp83;});_tmp85=_tmp84.f1;
_tmp86=_tmp84.f2;if(_tmp85 == _tmp79  && _tmp86 == _tmp7C)return t;return(void*)({
struct Cyc_Port_Ptr_ct_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87[0]=({
struct Cyc_Port_Ptr_ct_struct _tmp88;_tmp88.tag=1;_tmp88.f1=(void*)_tmp85;_tmp88.f2=(
void*)_tmp7A;_tmp88.f3=(void*)_tmp7B;_tmp88.f4=(void*)_tmp86;_tmp88.f5=(void*)
_tmp7D;_tmp88;});_tmp87;});}_LL65: if(_tmp77 <= (void*)10  || *((int*)_tmp77)!= 2)
goto _LL67;_tmp7E=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp77)->f1;_tmp7F=(
void*)((struct Cyc_Port_Array_ct_struct*)_tmp77)->f2;_tmp80=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp77)->f3;_LL66: {void*_tmp89=Cyc_Port_inst(instenv,_tmp7E);if(_tmp89 == _tmp7E)
return t;return(void*)({struct Cyc_Port_Array_ct_struct*_tmp8A=_cycalloc(sizeof(*
_tmp8A));_tmp8A[0]=({struct Cyc_Port_Array_ct_struct _tmp8B;_tmp8B.tag=2;_tmp8B.f1=(
void*)_tmp89;_tmp8B.f2=(void*)_tmp7F;_tmp8B.f3=(void*)_tmp80;_tmp8B;});_tmp8A;});}
_LL67: if(_tmp77 <= (void*)10  || *((int*)_tmp77)!= 5)goto _LL46;_tmp81=(void*)((
struct Cyc_Port_Fn_ct_struct*)_tmp77)->f1;_tmp82=((struct Cyc_Port_Fn_ct_struct*)
_tmp77)->f2;_LL68: return(void*)({struct Cyc_Port_Fn_ct_struct*_tmp8C=_cycalloc(
sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Port_Fn_ct_struct _tmp8D;_tmp8D.tag=5;
_tmp8D.f1=(void*)Cyc_Port_inst(instenv,_tmp81);_tmp8D.f2=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Dict_Dict**,void*),struct Cyc_Dict_Dict**env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Port_inst,instenv,_tmp82);_tmp8D;});_tmp8C;});_LL46:;}
void*Cyc_Port_instantiate(void*t){return Cyc_Port_inst(({struct Cyc_Dict_Dict**
_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E[0]=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp8E;}),
t);}struct Cyc_List_List*Cyc_Port_filter_self(void*t,struct Cyc_List_List*ts){int
found=0;{struct Cyc_List_List*_tmp8F=ts;for(0;(unsigned int)_tmp8F;_tmp8F=_tmp8F->tl){
void*_tmp90=Cyc_Port_compress_ct((void*)_tmp8F->hd);if(t == _tmp90)found=1;}}if(!
found)return ts;{struct Cyc_List_List*res=0;for(0;ts != 0;ts=ts->tl){if(t == Cyc_Port_compress_ct((
void*)ts->hd))continue;res=({struct Cyc_List_List*_tmp91=_cycalloc(sizeof(*_tmp91));
_tmp91->hd=(void*)((void*)ts->hd);_tmp91->tl=res;_tmp91;});}return res;}}void Cyc_Port_generalize(
int is_rgn,void*t){t=Cyc_Port_compress_ct(t);{void*_tmp92=t;struct Cyc_Port_Cvar*
_tmp93;void*_tmp94;void*_tmp95;void*_tmp96;void*_tmp97;void*_tmp98;void*_tmp99;
void*_tmp9A;void*_tmp9B;void*_tmp9C;struct Cyc_List_List*_tmp9D;_LL6A: if(_tmp92 <= (
void*)10  || *((int*)_tmp92)!= 6)goto _LL6C;_tmp93=((struct Cyc_Port_Var_ct_struct*)
_tmp92)->f1;_LL6B: _tmp93->uppers=Cyc_Port_filter_self(t,_tmp93->uppers);_tmp93->lowers=
Cyc_Port_filter_self(t,_tmp93->lowers);if(is_rgn){if(_tmp93->uppers == 0  && 
_tmp93->lowers == 0){Cyc_Port_unifies(t,Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var()));
return;}if((unsigned int)_tmp93->uppers){Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)
_check_null(_tmp93->uppers))->hd);Cyc_Port_generalize(1,t);}else{Cyc_Port_unifies(
t,(void*)((struct Cyc_List_List*)_check_null(_tmp93->lowers))->hd);Cyc_Port_generalize(
1,t);}}return;_LL6C: if((int)_tmp92 != 0)goto _LL6E;_LL6D: goto _LL6F;_LL6E: if((int)
_tmp92 != 1)goto _LL70;_LL6F: goto _LL71;_LL70: if((int)_tmp92 != 2)goto _LL72;_LL71:
goto _LL73;_LL72: if((int)_tmp92 != 3)goto _LL74;_LL73: goto _LL75;_LL74: if((int)
_tmp92 != 4)goto _LL76;_LL75: goto _LL77;_LL76: if((int)_tmp92 != 5)goto _LL78;_LL77:
goto _LL79;_LL78: if((int)_tmp92 != 6)goto _LL7A;_LL79: goto _LL7B;_LL7A: if(_tmp92 <= (
void*)10  || *((int*)_tmp92)!= 4)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if(_tmp92 <= (
void*)10  || *((int*)_tmp92)!= 3)goto _LL7E;_LL7D: goto _LL7F;_LL7E: if(_tmp92 <= (
void*)10  || *((int*)_tmp92)!= 0)goto _LL80;_LL7F: goto _LL81;_LL80: if((int)_tmp92 != 
9)goto _LL82;_LL81: goto _LL83;_LL82: if((int)_tmp92 != 8)goto _LL84;_LL83: goto _LL85;
_LL84: if((int)_tmp92 != 7)goto _LL86;_LL85: return;_LL86: if(_tmp92 <= (void*)10  || *((
int*)_tmp92)!= 1)goto _LL88;_tmp94=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp92)->f1;
_tmp95=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp92)->f2;_tmp96=(void*)((struct
Cyc_Port_Ptr_ct_struct*)_tmp92)->f3;_tmp97=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp92)->f4;_tmp98=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp92)->f5;_LL87: Cyc_Port_generalize(
0,_tmp94);Cyc_Port_generalize(1,_tmp97);goto _LL69;_LL88: if(_tmp92 <= (void*)10
 || *((int*)_tmp92)!= 2)goto _LL8A;_tmp99=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp92)->f1;_tmp9A=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp92)->f2;_tmp9B=(
void*)((struct Cyc_Port_Array_ct_struct*)_tmp92)->f3;_LL89: Cyc_Port_generalize(0,
_tmp99);Cyc_Port_generalize(0,_tmp9A);goto _LL69;_LL8A: if(_tmp92 <= (void*)10  || *((
int*)_tmp92)!= 5)goto _LL69;_tmp9C=(void*)((struct Cyc_Port_Fn_ct_struct*)_tmp92)->f1;
_tmp9D=((struct Cyc_Port_Fn_ct_struct*)_tmp92)->f2;_LL8B: Cyc_Port_generalize(0,
_tmp9C);((void(*)(void(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Port_generalize,0,_tmp9D);goto _LL69;_LL69:;}}static int Cyc_Port_occurs(void*v,
void*t){t=Cyc_Port_compress_ct(t);if(t == v)return 1;{void*_tmp9E=t;void*_tmp9F;
void*_tmpA0;void*_tmpA1;void*_tmpA2;void*_tmpA3;void*_tmpA4;void*_tmpA5;void*
_tmpA6;void*_tmpA7;struct Cyc_List_List*_tmpA8;struct _tuple4*_tmpA9;struct _tuple4
_tmpAA;struct Cyc_List_List*_tmpAB;struct Cyc_List_List*_tmpAC;_LL8D: if(_tmp9E <= (
void*)10  || *((int*)_tmp9E)!= 1)goto _LL8F;_tmp9F=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp9E)->f1;_tmpA0=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp9E)->f2;_tmpA1=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp9E)->f3;_tmpA2=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp9E)->f4;_tmpA3=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp9E)->f5;_LL8E:
return(((Cyc_Port_occurs(v,_tmp9F) || Cyc_Port_occurs(v,_tmpA0)) || Cyc_Port_occurs(
v,_tmpA1)) || Cyc_Port_occurs(v,_tmpA2)) || Cyc_Port_occurs(v,_tmpA3);_LL8F: if(
_tmp9E <= (void*)10  || *((int*)_tmp9E)!= 2)goto _LL91;_tmpA4=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp9E)->f1;_tmpA5=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp9E)->f2;_tmpA6=(
void*)((struct Cyc_Port_Array_ct_struct*)_tmp9E)->f3;_LL90: return(Cyc_Port_occurs(
v,_tmpA4) || Cyc_Port_occurs(v,_tmpA5)) || Cyc_Port_occurs(v,_tmpA6);_LL91: if(
_tmp9E <= (void*)10  || *((int*)_tmp9E)!= 5)goto _LL93;_tmpA7=(void*)((struct Cyc_Port_Fn_ct_struct*)
_tmp9E)->f1;_tmpA8=((struct Cyc_Port_Fn_ct_struct*)_tmp9E)->f2;_LL92: if(Cyc_Port_occurs(
v,_tmpA7))return 1;for(0;(unsigned int)_tmpA8;_tmpA8=_tmpA8->tl){if(Cyc_Port_occurs(
v,(void*)_tmpA8->hd))return 1;}return 0;_LL93: if(_tmp9E <= (void*)10  || *((int*)
_tmp9E)!= 3)goto _LL95;_tmpA9=((struct Cyc_Port_KnownAggr_ct_struct*)_tmp9E)->f1;
_tmpAA=*_tmpA9;_tmpAB=_tmpAA.f2;_LL94: return 0;_LL95: if(_tmp9E <= (void*)10  || *((
int*)_tmp9E)!= 4)goto _LL97;_tmpAC=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmp9E)->f1;
_LL96: for(0;(unsigned int)_tmpAC;_tmpAC=_tmpAC->tl){if(Cyc_Port_occurs(v,(void*)((
struct Cyc_Port_Cfield*)_tmpAC->hd)->qual) || Cyc_Port_occurs(v,(void*)((struct
Cyc_Port_Cfield*)_tmpAC->hd)->type))return 1;}return 0;_LL97:;_LL98: return 0;_LL8C:;}}
char Cyc_Port_Unify_ct[13]="\000\000\000\000Unify_ct\000";static int Cyc_Port_leq(
void*t1,void*t2);static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*
t2);static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,
struct Cyc_List_List*fs2,int allow_subset);static void Cyc_Port_unify_ct(void*t1,
void*t2){t1=Cyc_Port_compress_ct(t1);t2=Cyc_Port_compress_ct(t2);if(t1 == t2)
return;{struct _tuple3 _tmpAE=({struct _tuple3 _tmpAD;_tmpAD.f1=t1;_tmpAD.f2=t2;
_tmpAD;});void*_tmpAF;struct Cyc_Port_Cvar*_tmpB0;void*_tmpB1;struct Cyc_Port_Cvar*
_tmpB2;void*_tmpB3;void*_tmpB4;void*_tmpB5;void*_tmpB6;void*_tmpB7;void*_tmpB8;
void*_tmpB9;void*_tmpBA;void*_tmpBB;void*_tmpBC;void*_tmpBD;void*_tmpBE;void*
_tmpBF;struct _tagged_arr*_tmpC0;struct _tagged_arr _tmpC1;void*_tmpC2;struct
_tagged_arr*_tmpC3;struct _tagged_arr _tmpC4;void*_tmpC5;void*_tmpC6;void*_tmpC7;
void*_tmpC8;void*_tmpC9;void*_tmpCA;void*_tmpCB;void*_tmpCC;void*_tmpCD;void*
_tmpCE;struct Cyc_List_List*_tmpCF;void*_tmpD0;void*_tmpD1;struct Cyc_List_List*
_tmpD2;void*_tmpD3;struct _tuple4*_tmpD4;void*_tmpD5;struct _tuple4*_tmpD6;void*
_tmpD7;struct Cyc_List_List*_tmpD8;void**_tmpD9;void***_tmpDA;void*_tmpDB;struct
Cyc_List_List*_tmpDC;void**_tmpDD;void***_tmpDE;void*_tmpDF;struct Cyc_List_List*
_tmpE0;void**_tmpE1;void***_tmpE2;void*_tmpE3;struct _tuple4*_tmpE4;struct _tuple4
_tmpE5;struct Cyc_Absyn_Aggrdecl*_tmpE6;struct Cyc_List_List*_tmpE7;void*_tmpE8;
struct _tuple4*_tmpE9;struct _tuple4 _tmpEA;struct Cyc_Absyn_Aggrdecl*_tmpEB;struct
Cyc_List_List*_tmpEC;void*_tmpED;struct Cyc_List_List*_tmpEE;void**_tmpEF;void***
_tmpF0;_LL9A: _tmpAF=_tmpAE.f1;if(_tmpAF <= (void*)10  || *((int*)_tmpAF)!= 6)goto
_LL9C;_tmpB0=((struct Cyc_Port_Var_ct_struct*)_tmpAF)->f1;_LL9B: if(!Cyc_Port_occurs(
t1,t2)){{struct Cyc_List_List*_tmpF1=Cyc_Port_filter_self(t1,_tmpB0->uppers);for(
0;(unsigned int)_tmpF1;_tmpF1=_tmpF1->tl){if(!Cyc_Port_leq(t2,(void*)_tmpF1->hd))(
int)_throw((void*)Cyc_Port_Unify_ct);}}{struct Cyc_List_List*_tmpF2=Cyc_Port_filter_self(
t1,_tmpB0->lowers);for(0;(unsigned int)_tmpF2;_tmpF2=_tmpF2->tl){if(!Cyc_Port_leq((
void*)_tmpF2->hd,t2))(int)_throw((void*)Cyc_Port_Unify_ct);}}_tmpB0->eq=({void**
_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=t2;_tmpF3;});return;}else{(int)
_throw((void*)Cyc_Port_Unify_ct);}_LL9C: _tmpB1=_tmpAE.f2;if(_tmpB1 <= (void*)10
 || *((int*)_tmpB1)!= 6)goto _LL9E;_tmpB2=((struct Cyc_Port_Var_ct_struct*)_tmpB1)->f1;
_LL9D: Cyc_Port_unify_ct(t2,t1);return;_LL9E: _tmpB3=_tmpAE.f1;if(_tmpB3 <= (void*)
10  || *((int*)_tmpB3)!= 1)goto _LLA0;_tmpB4=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmpB3)->f1;_tmpB5=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpB3)->f2;_tmpB6=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpB3)->f3;_tmpB7=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmpB3)->f4;_tmpB8=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpB3)->f5;_tmpB9=
_tmpAE.f2;if(_tmpB9 <= (void*)10  || *((int*)_tmpB9)!= 1)goto _LLA0;_tmpBA=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmpB9)->f1;_tmpBB=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmpB9)->f2;_tmpBC=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpB9)->f3;_tmpBD=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpB9)->f4;_tmpBE=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmpB9)->f5;_LL9F: Cyc_Port_unify_ct(_tmpB4,_tmpBA);Cyc_Port_unify_ct(_tmpB5,
_tmpBB);Cyc_Port_unify_ct(_tmpB6,_tmpBC);Cyc_Port_unify_ct(_tmpB7,_tmpBD);Cyc_Port_unify_ct(
_tmpB8,_tmpBE);return;_LLA0: _tmpBF=_tmpAE.f1;if(_tmpBF <= (void*)10  || *((int*)
_tmpBF)!= 0)goto _LLA2;_tmpC0=((struct Cyc_Port_RgnVar_ct_struct*)_tmpBF)->f1;
_tmpC1=*_tmpC0;_tmpC2=_tmpAE.f2;if(_tmpC2 <= (void*)10  || *((int*)_tmpC2)!= 0)
goto _LLA2;_tmpC3=((struct Cyc_Port_RgnVar_ct_struct*)_tmpC2)->f1;_tmpC4=*_tmpC3;
_LLA1: if(Cyc_strcmp((struct _tagged_arr)_tmpC1,(struct _tagged_arr)_tmpC4)!= 0)(
int)_throw((void*)Cyc_Port_Unify_ct);return;_LLA2: _tmpC5=_tmpAE.f1;if(_tmpC5 <= (
void*)10  || *((int*)_tmpC5)!= 2)goto _LLA4;_tmpC6=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmpC5)->f1;_tmpC7=(void*)((struct Cyc_Port_Array_ct_struct*)_tmpC5)->f2;_tmpC8=(
void*)((struct Cyc_Port_Array_ct_struct*)_tmpC5)->f3;_tmpC9=_tmpAE.f2;if(_tmpC9 <= (
void*)10  || *((int*)_tmpC9)!= 2)goto _LLA4;_tmpCA=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmpC9)->f1;_tmpCB=(void*)((struct Cyc_Port_Array_ct_struct*)_tmpC9)->f2;_tmpCC=(
void*)((struct Cyc_Port_Array_ct_struct*)_tmpC9)->f3;_LLA3: Cyc_Port_unify_ct(
_tmpC6,_tmpCA);Cyc_Port_unify_ct(_tmpC7,_tmpCB);Cyc_Port_unify_ct(_tmpC8,_tmpCC);
return;_LLA4: _tmpCD=_tmpAE.f1;if(_tmpCD <= (void*)10  || *((int*)_tmpCD)!= 5)goto
_LLA6;_tmpCE=(void*)((struct Cyc_Port_Fn_ct_struct*)_tmpCD)->f1;_tmpCF=((struct
Cyc_Port_Fn_ct_struct*)_tmpCD)->f2;_tmpD0=_tmpAE.f2;if(_tmpD0 <= (void*)10  || *((
int*)_tmpD0)!= 5)goto _LLA6;_tmpD1=(void*)((struct Cyc_Port_Fn_ct_struct*)_tmpD0)->f1;
_tmpD2=((struct Cyc_Port_Fn_ct_struct*)_tmpD0)->f2;_LLA5: Cyc_Port_unify_ct(_tmpCE,
_tmpD1);Cyc_Port_unify_cts(_tmpCF,_tmpD2);return;_LLA6: _tmpD3=_tmpAE.f1;if(
_tmpD3 <= (void*)10  || *((int*)_tmpD3)!= 3)goto _LLA8;_tmpD4=((struct Cyc_Port_KnownAggr_ct_struct*)
_tmpD3)->f1;_tmpD5=_tmpAE.f2;if(_tmpD5 <= (void*)10  || *((int*)_tmpD5)!= 3)goto
_LLA8;_tmpD6=((struct Cyc_Port_KnownAggr_ct_struct*)_tmpD5)->f1;_LLA7: if(_tmpD4 == 
_tmpD6)return;else{(int)_throw((void*)Cyc_Port_Unify_ct);}_LLA8: _tmpD7=_tmpAE.f1;
if(_tmpD7 <= (void*)10  || *((int*)_tmpD7)!= 4)goto _LLAA;_tmpD8=((struct Cyc_Port_UnknownAggr_ct_struct*)
_tmpD7)->f1;_tmpD9=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmpD7)->f2;_tmpDA=(
void***)&((struct Cyc_Port_UnknownAggr_ct_struct*)_tmpD7)->f2;_tmpDB=_tmpAE.f2;
if(_tmpDB <= (void*)10  || *((int*)_tmpDB)!= 4)goto _LLAA;_tmpDC=((struct Cyc_Port_UnknownAggr_ct_struct*)
_tmpDB)->f1;_tmpDD=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmpDB)->f2;_tmpDE=(
void***)&((struct Cyc_Port_UnknownAggr_ct_struct*)_tmpDB)->f2;_LLA9: {void*_tmpF4=
Cyc_Port_unknown_aggr_ct(Cyc_Port_merge_fields(_tmpD8,_tmpDC,1));*_tmpDA=(*
_tmpDE=({void**_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5[0]=_tmpF4;_tmpF5;}));
return;}_LLAA: _tmpDF=_tmpAE.f1;if(_tmpDF <= (void*)10  || *((int*)_tmpDF)!= 4)goto
_LLAC;_tmpE0=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmpDF)->f1;_tmpE1=((struct
Cyc_Port_UnknownAggr_ct_struct*)_tmpDF)->f2;_tmpE2=(void***)&((struct Cyc_Port_UnknownAggr_ct_struct*)
_tmpDF)->f2;_tmpE3=_tmpAE.f2;if(_tmpE3 <= (void*)10  || *((int*)_tmpE3)!= 3)goto
_LLAC;_tmpE4=((struct Cyc_Port_KnownAggr_ct_struct*)_tmpE3)->f1;_tmpE5=*_tmpE4;
_tmpE6=_tmpE5.f1;_tmpE7=_tmpE5.f2;_LLAB: Cyc_Port_merge_fields(_tmpE7,_tmpE0,0);*
_tmpE2=({void**_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=t2;_tmpF6;});return;
_LLAC: _tmpE8=_tmpAE.f1;if(_tmpE8 <= (void*)10  || *((int*)_tmpE8)!= 3)goto _LLAE;
_tmpE9=((struct Cyc_Port_KnownAggr_ct_struct*)_tmpE8)->f1;_tmpEA=*_tmpE9;_tmpEB=
_tmpEA.f1;_tmpEC=_tmpEA.f2;_tmpED=_tmpAE.f2;if(_tmpED <= (void*)10  || *((int*)
_tmpED)!= 4)goto _LLAE;_tmpEE=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmpED)->f1;
_tmpEF=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmpED)->f2;_tmpF0=(void***)&((
struct Cyc_Port_UnknownAggr_ct_struct*)_tmpED)->f2;_LLAD: Cyc_Port_merge_fields(
_tmpEC,_tmpEE,0);*_tmpF0=({void**_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7[0]=t1;
_tmpF7;});return;_LLAE:;_LLAF:(int)_throw((void*)Cyc_Port_Unify_ct);_LL99:;}}
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(
0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Port_unify_ct((void*)t1->hd,(void*)
t2->hd);}if(t1 != 0  || t2 != 0)(int)_throw((void*)Cyc_Port_Unify_ct);}static struct
Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*
fs2,int allow_f1_subset_f2){struct Cyc_List_List*common=0;{struct Cyc_List_List*
_tmpF8=fs2;for(0;(unsigned int)_tmpF8;_tmpF8=_tmpF8->tl){struct Cyc_Port_Cfield*
_tmpF9=(struct Cyc_Port_Cfield*)_tmpF8->hd;int found=0;{struct Cyc_List_List*_tmpFA=
fs1;for(0;(unsigned int)_tmpFA;_tmpFA=_tmpFA->tl){struct Cyc_Port_Cfield*_tmpFB=(
struct Cyc_Port_Cfield*)_tmpFA->hd;if(Cyc_strptrcmp(_tmpFB->f,_tmpF9->f)== 0){
common=({struct Cyc_List_List*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->hd=_tmpFB;
_tmpFC->tl=common;_tmpFC;});Cyc_Port_unify_ct((void*)_tmpFB->qual,(void*)_tmpF9->qual);
Cyc_Port_unify_ct((void*)_tmpFB->type,(void*)_tmpF9->type);found=1;break;}}}if(!
found){if(allow_f1_subset_f2)common=({struct Cyc_List_List*_tmpFD=_cycalloc(
sizeof(*_tmpFD));_tmpFD->hd=_tmpF9;_tmpFD->tl=common;_tmpFD;});else{(int)_throw((
void*)Cyc_Port_Unify_ct);}}}}{struct Cyc_List_List*_tmpFE=fs1;for(0;(unsigned int)
_tmpFE;_tmpFE=_tmpFE->tl){struct Cyc_Port_Cfield*_tmpFF=(struct Cyc_Port_Cfield*)
_tmpFE->hd;int found=0;{struct Cyc_List_List*_tmp100=fs2;for(0;(unsigned int)
_tmp100;_tmp100=_tmp100->tl){struct Cyc_Port_Cfield*_tmp101=(struct Cyc_Port_Cfield*)
_tmp100->hd;if(Cyc_strptrcmp(_tmpFF->f,_tmp101->f))found=1;}}if(!found)common=({
struct Cyc_List_List*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->hd=_tmpFF;
_tmp102->tl=common;_tmp102;});}}return common;}static int Cyc_Port_unifies(void*t1,
void*t2){{struct _handler_cons _tmp103;_push_handler(& _tmp103);{int _tmp105=0;if(
setjmp(_tmp103.handler))_tmp105=1;if(!_tmp105){Cyc_Port_unify_ct(t1,t2);;
_pop_handler();}else{void*_tmp104=(void*)_exn_thrown;void*_tmp107=_tmp104;_LLB1:
if(_tmp107 != Cyc_Port_Unify_ct)goto _LLB3;_LLB2: return 0;_LLB3:;_LLB4:(void)_throw(
_tmp107);_LLB0:;}}}return 1;}struct _tuple5{void*f1;void*f2;void*f3;void*f4;void*
f5;};static struct Cyc_List_List*Cyc_Port_insert_upper(void*v,void*t,struct Cyc_List_List**
uppers){t=Cyc_Port_compress_ct(t);{void*_tmp108=t;_LLB6: if((int)_tmp108 != 1)goto
_LLB8;_LLB7: goto _LLB9;_LLB8: if((int)_tmp108 != 8)goto _LLBA;_LLB9: goto _LLBB;_LLBA:
if((int)_tmp108 != 5)goto _LLBC;_LLBB: goto _LLBD;_LLBC: if((int)_tmp108 != 2)goto
_LLBE;_LLBD: goto _LLBF;_LLBE: if((int)_tmp108 != 3)goto _LLC0;_LLBF: goto _LLC1;_LLC0:
if(_tmp108 <= (void*)10  || *((int*)_tmp108)!= 2)goto _LLC2;_LLC1: goto _LLC3;_LLC2:
if(_tmp108 <= (void*)10  || *((int*)_tmp108)!= 3)goto _LLC4;_LLC3: goto _LLC5;_LLC4:
if(_tmp108 <= (void*)10  || *((int*)_tmp108)!= 5)goto _LLC6;_LLC5: goto _LLC7;_LLC6:
if((int)_tmp108 != 7)goto _LLC8;_LLC7:*uppers=0;Cyc_Port_unifies(v,t);return*
uppers;_LLC8: if((int)_tmp108 != 4)goto _LLCA;_LLC9: goto _LLCB;_LLCA: if((int)_tmp108
!= 0)goto _LLCC;_LLCB: goto _LLCD;_LLCC: if((int)_tmp108 != 9)goto _LLCE;_LLCD: return*
uppers;_LLCE:;_LLCF: goto _LLB5;_LLB5:;}{struct Cyc_List_List*_tmp109=*uppers;for(0;(
unsigned int)_tmp109;_tmp109=_tmp109->tl){void*_tmp10A=Cyc_Port_compress_ct((
void*)_tmp109->hd);if(t == _tmp10A)return*uppers;{struct _tuple3 _tmp10C=({struct
_tuple3 _tmp10B;_tmp10B.f1=t;_tmp10B.f2=_tmp10A;_tmp10B;});void*_tmp10D;void*
_tmp10E;void*_tmp10F;void*_tmp110;void*_tmp111;void*_tmp112;void*_tmp113;void*
_tmp114;void*_tmp115;void*_tmp116;void*_tmp117;void*_tmp118;void*_tmp119;void*
_tmp11A;void*_tmp11B;void*_tmp11C;void*_tmp11D;void*_tmp11E;_LLD1: _tmp10D=
_tmp10C.f1;if((int)_tmp10D != 6)goto _LLD3;_tmp10E=_tmp10C.f2;if(_tmp10E <= (void*)
10  || *((int*)_tmp10E)!= 1)goto _LLD3;_LLD2: goto _LLD4;_LLD3: _tmp10F=_tmp10C.f1;
if((int)_tmp10F != 6)goto _LLD5;_tmp110=_tmp10C.f2;if((int)_tmp110 != 5)goto _LLD5;
_LLD4: goto _LLD6;_LLD5: _tmp111=_tmp10C.f1;if((int)_tmp111 != 6)goto _LLD7;_tmp112=
_tmp10C.f2;if(_tmp112 <= (void*)10  || *((int*)_tmp112)!= 2)goto _LLD7;_LLD6: return*
uppers;_LLD7: _tmp113=_tmp10C.f1;if(_tmp113 <= (void*)10  || *((int*)_tmp113)!= 1)
goto _LLD9;_tmp114=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp113)->f1;_tmp115=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp113)->f2;_tmp116=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp113)->f3;_tmp117=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp113)->f4;_tmp118=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp113)->f5;_tmp119=_tmp10C.f2;if(_tmp119
<= (void*)10  || *((int*)_tmp119)!= 1)goto _LLD9;_tmp11A=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp119)->f1;_tmp11B=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp119)->f2;_tmp11C=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp119)->f3;_tmp11D=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp119)->f4;_tmp11E=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp119)->f5;_LLD8: {
void*_tmp121;void*_tmp122;void*_tmp123;void*_tmp124;void*_tmp125;struct _tuple5
_tmp120=({struct _tuple5 _tmp11F;_tmp11F.f1=Cyc_Port_var();_tmp11F.f2=Cyc_Port_var();
_tmp11F.f3=Cyc_Port_var();_tmp11F.f4=Cyc_Port_var();_tmp11F.f5=Cyc_Port_var();
_tmp11F;});_tmp121=_tmp120.f1;_tmp122=_tmp120.f2;_tmp123=_tmp120.f3;_tmp124=
_tmp120.f4;_tmp125=_tmp120.f5;{struct Cyc_Port_Ptr_ct_struct*_tmp126=({struct Cyc_Port_Ptr_ct_struct*
_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127[0]=({struct Cyc_Port_Ptr_ct_struct
_tmp128;_tmp128.tag=1;_tmp128.f1=(void*)_tmp121;_tmp128.f2=(void*)_tmp122;
_tmp128.f3=(void*)_tmp123;_tmp128.f4=(void*)_tmp124;_tmp128.f5=(void*)_tmp125;
_tmp128;});_tmp127;});Cyc_Port_leq(_tmp121,_tmp114);Cyc_Port_leq(_tmp121,_tmp11A);
Cyc_Port_leq(_tmp122,_tmp115);Cyc_Port_leq(_tmp122,_tmp11B);Cyc_Port_leq(_tmp123,
_tmp116);Cyc_Port_leq(_tmp123,_tmp11B);Cyc_Port_leq(_tmp124,_tmp117);Cyc_Port_leq(
_tmp124,_tmp11D);Cyc_Port_leq(_tmp125,_tmp118);Cyc_Port_leq(_tmp125,_tmp11E);(
void*)(_tmp109->hd=(void*)((void*)_tmp126));return*uppers;}}_LLD9:;_LLDA: goto
_LLD0;_LLD0:;}}}return({struct Cyc_List_List*_tmp129=_cycalloc(sizeof(*_tmp129));
_tmp129->hd=(void*)t;_tmp129->tl=*uppers;_tmp129;});}static struct Cyc_List_List*
Cyc_Port_insert_lower(void*v,void*t,struct Cyc_List_List**lowers){t=Cyc_Port_compress_ct(
t);{void*_tmp12A=t;_LLDC: if((int)_tmp12A != 0)goto _LLDE;_LLDD: goto _LLDF;_LLDE: if((
int)_tmp12A != 8)goto _LLE0;_LLDF: goto _LLE1;_LLE0: if((int)_tmp12A != 2)goto _LLE2;
_LLE1: goto _LLE3;_LLE2: if((int)_tmp12A != 3)goto _LLE4;_LLE3: goto _LLE5;_LLE4: if((
int)_tmp12A != 4)goto _LLE6;_LLE5: goto _LLE7;_LLE6: if(_tmp12A <= (void*)10  || *((int*)
_tmp12A)!= 3)goto _LLE8;_LLE7: goto _LLE9;_LLE8: if(_tmp12A <= (void*)10  || *((int*)
_tmp12A)!= 5)goto _LLEA;_LLE9: goto _LLEB;_LLEA: if(_tmp12A <= (void*)10  || *((int*)
_tmp12A)!= 0)goto _LLEC;_LLEB:*lowers=0;Cyc_Port_unifies(v,t);return*lowers;_LLEC:
if((int)_tmp12A != 7)goto _LLEE;_LLED: goto _LLEF;_LLEE: if((int)_tmp12A != 1)goto
_LLF0;_LLEF: goto _LLF1;_LLF0: if((int)_tmp12A != 9)goto _LLF2;_LLF1: return*lowers;
_LLF2:;_LLF3: goto _LLDB;_LLDB:;}{struct Cyc_List_List*_tmp12B=*lowers;for(0;(
unsigned int)_tmp12B;_tmp12B=_tmp12B->tl){void*_tmp12C=Cyc_Port_compress_ct((
void*)_tmp12B->hd);if(t == _tmp12C)return*lowers;{struct _tuple3 _tmp12E=({struct
_tuple3 _tmp12D;_tmp12D.f1=t;_tmp12D.f2=_tmp12C;_tmp12D;});void*_tmp12F;void*
_tmp130;void*_tmp131;void*_tmp132;void*_tmp133;void*_tmp134;void*_tmp135;void*
_tmp136;void*_tmp137;void*_tmp138;void*_tmp139;void*_tmp13A;void*_tmp13B;void*
_tmp13C;void*_tmp13D;void*_tmp13E;void*_tmp13F;void*_tmp140;void*_tmp141;void*
_tmp142;void*_tmp143;_LLF5: _tmp12F=_tmp12E.f2;if((int)_tmp12F != 4)goto _LLF7;
_LLF6: goto _LLF8;_LLF7: _tmp130=_tmp12E.f1;if((int)_tmp130 != 5)goto _LLF9;_tmp131=
_tmp12E.f2;if((int)_tmp131 != 6)goto _LLF9;_LLF8: goto _LLFA;_LLF9: _tmp132=_tmp12E.f1;
if((int)_tmp132 != 5)goto _LLFB;_tmp133=_tmp12E.f2;if(_tmp133 <= (void*)10  || *((
int*)_tmp133)!= 1)goto _LLFB;_LLFA: goto _LLFC;_LLFB: _tmp134=_tmp12E.f1;if(_tmp134
<= (void*)10  || *((int*)_tmp134)!= 1)goto _LLFD;_tmp135=_tmp12E.f2;if((int)
_tmp135 != 6)goto _LLFD;_LLFC: goto _LLFE;_LLFD: _tmp136=_tmp12E.f1;if(_tmp136 <= (
void*)10  || *((int*)_tmp136)!= 2)goto _LLFF;_tmp137=_tmp12E.f2;if((int)_tmp137 != 
6)goto _LLFF;_LLFE: return*lowers;_LLFF: _tmp138=_tmp12E.f1;if(_tmp138 <= (void*)10
 || *((int*)_tmp138)!= 1)goto _LL101;_tmp139=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp138)->f1;_tmp13A=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp138)->f2;_tmp13B=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp138)->f3;_tmp13C=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp138)->f4;_tmp13D=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp138)->f5;_tmp13E=
_tmp12E.f2;if(_tmp13E <= (void*)10  || *((int*)_tmp13E)!= 1)goto _LL101;_tmp13F=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp13E)->f1;_tmp140=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp13E)->f2;_tmp141=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp13E)->f3;_tmp142=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp13E)->f4;_tmp143=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp13E)->f5;_LL100: {void*_tmp146;void*_tmp147;void*_tmp148;void*_tmp149;void*
_tmp14A;struct _tuple5 _tmp145=({struct _tuple5 _tmp144;_tmp144.f1=Cyc_Port_var();
_tmp144.f2=Cyc_Port_var();_tmp144.f3=Cyc_Port_var();_tmp144.f4=Cyc_Port_var();
_tmp144.f5=Cyc_Port_var();_tmp144;});_tmp146=_tmp145.f1;_tmp147=_tmp145.f2;
_tmp148=_tmp145.f3;_tmp149=_tmp145.f4;_tmp14A=_tmp145.f5;{struct Cyc_Port_Ptr_ct_struct*
_tmp14B=({struct Cyc_Port_Ptr_ct_struct*_tmp14C=_cycalloc(sizeof(*_tmp14C));
_tmp14C[0]=({struct Cyc_Port_Ptr_ct_struct _tmp14D;_tmp14D.tag=1;_tmp14D.f1=(void*)
_tmp146;_tmp14D.f2=(void*)_tmp147;_tmp14D.f3=(void*)_tmp148;_tmp14D.f4=(void*)
_tmp149;_tmp14D.f5=(void*)_tmp14A;_tmp14D;});_tmp14C;});Cyc_Port_leq(_tmp139,
_tmp146);Cyc_Port_leq(_tmp13F,_tmp146);Cyc_Port_leq(_tmp13A,_tmp147);Cyc_Port_leq(
_tmp140,_tmp147);Cyc_Port_leq(_tmp13B,_tmp148);Cyc_Port_leq(_tmp140,_tmp148);Cyc_Port_leq(
_tmp13C,_tmp149);Cyc_Port_leq(_tmp142,_tmp149);Cyc_Port_leq(_tmp13D,_tmp14A);Cyc_Port_leq(
_tmp143,_tmp14A);(void*)(_tmp12B->hd=(void*)((void*)_tmp14B));return*lowers;}}
_LL101:;_LL102: goto _LLF4;_LLF4:;}}}return({struct Cyc_List_List*_tmp14E=_cycalloc(
sizeof(*_tmp14E));_tmp14E->hd=(void*)t;_tmp14E->tl=*lowers;_tmp14E;});}static int
Cyc_Port_leq(void*t1,void*t2){if(t1 == t2)return 1;t1=Cyc_Port_compress_ct(t1);t2=
Cyc_Port_compress_ct(t2);{struct _tuple3 _tmp150=({struct _tuple3 _tmp14F;_tmp14F.f1=
t1;_tmp14F.f2=t2;_tmp14F;});void*_tmp151;void*_tmp152;struct _tagged_arr*_tmp153;
struct _tagged_arr _tmp154;void*_tmp155;struct _tagged_arr*_tmp156;struct
_tagged_arr _tmp157;void*_tmp158;struct _tagged_arr*_tmp159;struct _tagged_arr
_tmp15A;void*_tmp15B;void*_tmp15C;void*_tmp15D;void*_tmp15E;void*_tmp15F;void*
_tmp160;void*_tmp161;void*_tmp162;void*_tmp163;void*_tmp164;void*_tmp165;void*
_tmp166;void*_tmp167;void*_tmp168;void*_tmp169;void*_tmp16A;void*_tmp16B;void*
_tmp16C;void*_tmp16D;void*_tmp16E;void*_tmp16F;void*_tmp170;void*_tmp171;void*
_tmp172;void*_tmp173;void*_tmp174;void*_tmp175;void*_tmp176;void*_tmp177;void*
_tmp178;void*_tmp179;void*_tmp17A;void*_tmp17B;void*_tmp17C;void*_tmp17D;void*
_tmp17E;void*_tmp17F;void*_tmp180;void*_tmp181;void*_tmp182;void*_tmp183;void*
_tmp184;void*_tmp185;void*_tmp186;void*_tmp187;void*_tmp188;void*_tmp189;void*
_tmp18A;void*_tmp18B;void*_tmp18C;void*_tmp18D;void*_tmp18E;void*_tmp18F;void*
_tmp190;void*_tmp191;void*_tmp192;void*_tmp193;void*_tmp194;struct Cyc_Port_Cvar*
_tmp195;void*_tmp196;struct Cyc_Port_Cvar*_tmp197;void*_tmp198;struct Cyc_Port_Cvar*
_tmp199;void*_tmp19A;struct Cyc_Port_Cvar*_tmp19B;_LL104: _tmp151=_tmp150.f1;if((
int)_tmp151 != 7)goto _LL106;_LL105: return 1;_LL106: _tmp152=_tmp150.f1;if(_tmp152 <= (
void*)10  || *((int*)_tmp152)!= 0)goto _LL108;_tmp153=((struct Cyc_Port_RgnVar_ct_struct*)
_tmp152)->f1;_tmp154=*_tmp153;_tmp155=_tmp150.f2;if(_tmp155 <= (void*)10  || *((
int*)_tmp155)!= 0)goto _LL108;_tmp156=((struct Cyc_Port_RgnVar_ct_struct*)_tmp155)->f1;
_tmp157=*_tmp156;_LL107: return Cyc_strcmp((struct _tagged_arr)_tmp154,(struct
_tagged_arr)_tmp157)== 0;_LL108: _tmp158=_tmp150.f1;if(_tmp158 <= (void*)10  || *((
int*)_tmp158)!= 0)goto _LL10A;_tmp159=((struct Cyc_Port_RgnVar_ct_struct*)_tmp158)->f1;
_tmp15A=*_tmp159;_tmp15B=_tmp150.f2;if((int)_tmp15B != 7)goto _LL10A;_LL109: return
0;_LL10A: _tmp15C=_tmp150.f1;if((int)_tmp15C != 1)goto _LL10C;_tmp15D=_tmp150.f2;
if((int)_tmp15D != 0)goto _LL10C;_LL10B: return 1;_LL10C: _tmp15E=_tmp150.f1;if((int)
_tmp15E != 0)goto _LL10E;_tmp15F=_tmp150.f2;if((int)_tmp15F != 1)goto _LL10E;_LL10D:
return 0;_LL10E: _tmp160=_tmp150.f1;if((int)_tmp160 != 9)goto _LL110;_tmp161=_tmp150.f2;
if((int)_tmp161 != 8)goto _LL110;_LL10F: return 0;_LL110: _tmp162=_tmp150.f1;if((int)
_tmp162 != 8)goto _LL112;_tmp163=_tmp150.f2;if((int)_tmp163 != 9)goto _LL112;_LL111:
return 1;_LL112: _tmp164=_tmp150.f1;if(_tmp164 <= (void*)10  || *((int*)_tmp164)!= 6)
goto _LL114;_tmp165=_tmp150.f2;if((int)_tmp165 != 0)goto _LL114;_LL113: return 1;
_LL114: _tmp166=_tmp150.f1;if(_tmp166 <= (void*)10  || *((int*)_tmp166)!= 6)goto
_LL116;_tmp167=_tmp150.f2;if((int)_tmp167 != 4)goto _LL116;_LL115: return 1;_LL116:
_tmp168=_tmp150.f1;if((int)_tmp168 != 4)goto _LL118;_LL117: return 0;_LL118: _tmp169=
_tmp150.f1;if((int)_tmp169 != 5)goto _LL11A;_tmp16A=_tmp150.f2;if((int)_tmp16A != 6)
goto _LL11A;_LL119: return 1;_LL11A: _tmp16B=_tmp150.f1;if((int)_tmp16B != 5)goto
_LL11C;_tmp16C=_tmp150.f2;if(_tmp16C <= (void*)10  || *((int*)_tmp16C)!= 1)goto
_LL11C;_LL11B: return 1;_LL11C: _tmp16D=_tmp150.f1;if((int)_tmp16D != 5)goto _LL11E;
_tmp16E=_tmp150.f2;if((int)_tmp16E != 4)goto _LL11E;_LL11D: return 1;_LL11E: _tmp16F=
_tmp150.f1;if(_tmp16F <= (void*)10  || *((int*)_tmp16F)!= 1)goto _LL120;_tmp170=
_tmp150.f2;if((int)_tmp170 != 6)goto _LL120;_LL11F: return 1;_LL120: _tmp171=_tmp150.f1;
if(_tmp171 <= (void*)10  || *((int*)_tmp171)!= 1)goto _LL122;_tmp172=_tmp150.f2;if((
int)_tmp172 != 4)goto _LL122;_LL121: return 1;_LL122: _tmp173=_tmp150.f1;if(_tmp173 <= (
void*)10  || *((int*)_tmp173)!= 2)goto _LL124;_tmp174=_tmp150.f2;if((int)_tmp174 != 
6)goto _LL124;_LL123: return 1;_LL124: _tmp175=_tmp150.f1;if(_tmp175 <= (void*)10  || *((
int*)_tmp175)!= 2)goto _LL126;_tmp176=_tmp150.f2;if((int)_tmp176 != 4)goto _LL126;
_LL125: return 1;_LL126: _tmp177=_tmp150.f1;if(_tmp177 <= (void*)10  || *((int*)
_tmp177)!= 1)goto _LL128;_tmp178=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp177)->f1;
_tmp179=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp177)->f2;_tmp17A=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmp177)->f3;_tmp17B=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp177)->f4;_tmp17C=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp177)->f5;_tmp17D=
_tmp150.f2;if(_tmp17D <= (void*)10  || *((int*)_tmp17D)!= 1)goto _LL128;_tmp17E=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp17D)->f1;_tmp17F=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp17D)->f2;_tmp180=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp17D)->f3;_tmp181=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp17D)->f4;_tmp182=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp17D)->f5;_LL127: return(((Cyc_Port_leq(_tmp178,_tmp17E) && Cyc_Port_leq(
_tmp179,_tmp17F)) && Cyc_Port_unifies(_tmp17A,_tmp180)) && Cyc_Port_leq(_tmp17B,
_tmp181)) && Cyc_Port_leq(_tmp17C,_tmp182);_LL128: _tmp183=_tmp150.f1;if(_tmp183
<= (void*)10  || *((int*)_tmp183)!= 2)goto _LL12A;_tmp184=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp183)->f1;_tmp185=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp183)->f2;
_tmp186=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp183)->f3;_tmp187=_tmp150.f2;
if(_tmp187 <= (void*)10  || *((int*)_tmp187)!= 2)goto _LL12A;_tmp188=(void*)((
struct Cyc_Port_Array_ct_struct*)_tmp187)->f1;_tmp189=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp187)->f2;_tmp18A=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp187)->f3;
_LL129: return(Cyc_Port_leq(_tmp184,_tmp188) && Cyc_Port_leq(_tmp185,_tmp189))
 && Cyc_Port_leq(_tmp186,_tmp18A);_LL12A: _tmp18B=_tmp150.f1;if(_tmp18B <= (void*)
10  || *((int*)_tmp18B)!= 2)goto _LL12C;_tmp18C=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp18B)->f1;_tmp18D=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp18B)->f2;
_tmp18E=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp18B)->f3;_tmp18F=_tmp150.f2;
if(_tmp18F <= (void*)10  || *((int*)_tmp18F)!= 1)goto _LL12C;_tmp190=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmp18F)->f1;_tmp191=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp18F)->f2;_tmp192=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp18F)->f3;_tmp193=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp18F)->f5;_LL12B: return((Cyc_Port_leq(
_tmp18C,_tmp190) && Cyc_Port_leq(_tmp18D,_tmp191)) && Cyc_Port_unifies((void*)3,
_tmp192)) && Cyc_Port_leq(_tmp18E,_tmp193);_LL12C: _tmp194=_tmp150.f1;if(_tmp194
<= (void*)10  || *((int*)_tmp194)!= 6)goto _LL12E;_tmp195=((struct Cyc_Port_Var_ct_struct*)
_tmp194)->f1;_tmp196=_tmp150.f2;if(_tmp196 <= (void*)10  || *((int*)_tmp196)!= 6)
goto _LL12E;_tmp197=((struct Cyc_Port_Var_ct_struct*)_tmp196)->f1;_LL12D: _tmp195->uppers=
Cyc_Port_filter_self(t1,_tmp195->uppers);_tmp197->lowers=Cyc_Port_filter_self(t2,
_tmp197->lowers);_tmp195->uppers=Cyc_Port_insert_upper(t1,t2,& _tmp195->uppers);
_tmp197->lowers=Cyc_Port_insert_lower(t2,t1,& _tmp197->lowers);return 1;_LL12E:
_tmp198=_tmp150.f1;if(_tmp198 <= (void*)10  || *((int*)_tmp198)!= 6)goto _LL130;
_tmp199=((struct Cyc_Port_Var_ct_struct*)_tmp198)->f1;_LL12F: _tmp199->uppers=Cyc_Port_filter_self(
t1,_tmp199->uppers);_tmp199->uppers=Cyc_Port_insert_upper(t1,t2,& _tmp199->uppers);
return 1;_LL130: _tmp19A=_tmp150.f2;if(_tmp19A <= (void*)10  || *((int*)_tmp19A)!= 6)
goto _LL132;_tmp19B=((struct Cyc_Port_Var_ct_struct*)_tmp19A)->f1;_LL131: _tmp19B->lowers=
Cyc_Port_filter_self(t2,_tmp19B->lowers);_tmp19B->lowers=Cyc_Port_insert_lower(
t2,t1,& _tmp19B->lowers);return 1;_LL132:;_LL133: return Cyc_Port_unifies(t1,t2);
_LL103:;}}struct Cyc_Port_GlobalCenv{struct Cyc_Dict_Dict*typedef_dict;struct Cyc_Dict_Dict*
struct_dict;struct Cyc_Dict_Dict*union_dict;void*return_type;struct Cyc_List_List*
qualifier_edits;struct Cyc_List_List*pointer_edits;struct Cyc_List_List*
zeroterm_edits;struct Cyc_List_List*edits;int porting;};struct Cyc_Port_Cenv{struct
Cyc_Port_GlobalCenv*gcenv;struct Cyc_Dict_Dict*var_dict;void*cpos;};static struct
Cyc_Port_Cenv*Cyc_Port_empty_cenv(){struct Cyc_Port_GlobalCenv*g=({struct Cyc_Port_GlobalCenv*
_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->typedef_dict=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp19D->struct_dict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp19D->union_dict=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp19D->qualifier_edits=0;_tmp19D->pointer_edits=0;_tmp19D->zeroterm_edits=0;
_tmp19D->edits=0;_tmp19D->porting=0;_tmp19D->return_type=(void*)Cyc_Port_void_ct();
_tmp19D;});return({struct Cyc_Port_Cenv*_tmp19C=_cycalloc(sizeof(*_tmp19C));
_tmp19C->gcenv=g;_tmp19C->cpos=(void*)((void*)3);_tmp19C->var_dict=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp19C;});}static int Cyc_Port_in_fn_arg(struct Cyc_Port_Cenv*env){return(void*)
env->cpos == (void*)1;}static int Cyc_Port_in_fn_res(struct Cyc_Port_Cenv*env){
return(void*)env->cpos == (void*)0;}static int Cyc_Port_in_toplevel(struct Cyc_Port_Cenv*
env){return(void*)env->cpos == (void*)3;}static void*Cyc_Port_lookup_return_type(
struct Cyc_Port_Cenv*env){return(void*)(env->gcenv)->return_type;}static void*Cyc_Port_lookup_typedef(
struct Cyc_Port_Cenv*env,struct _tuple0*n){struct _handler_cons _tmp19E;
_push_handler(& _tmp19E);{int _tmp1A0=0;if(setjmp(_tmp19E.handler))_tmp1A0=1;if(!
_tmp1A0){{void*_tmp1A2;struct _tuple3 _tmp1A1=*((struct _tuple3*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);_tmp1A2=
_tmp1A1.f1;{void*_tmp1A3=_tmp1A2;_npop_handler(0);return _tmp1A3;}};_pop_handler();}
else{void*_tmp19F=(void*)_exn_thrown;void*_tmp1A5=_tmp19F;_LL135: if(_tmp1A5 != 
Cyc_Dict_Absent)goto _LL137;_LL136: return Cyc_Absyn_sint_typ;_LL137:;_LL138:(void)
_throw(_tmp1A5);_LL134:;}}}static void*Cyc_Port_lookup_typedef_ctype(struct Cyc_Port_Cenv*
env,struct _tuple0*n){struct _handler_cons _tmp1A6;_push_handler(& _tmp1A6);{int
_tmp1A8=0;if(setjmp(_tmp1A6.handler))_tmp1A8=1;if(!_tmp1A8){{void*_tmp1AA;struct
_tuple3 _tmp1A9=*((struct _tuple3*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_lookup)((
env->gcenv)->typedef_dict,n);_tmp1AA=_tmp1A9.f2;{void*_tmp1AB=_tmp1AA;
_npop_handler(0);return _tmp1AB;}};_pop_handler();}else{void*_tmp1A7=(void*)
_exn_thrown;void*_tmp1AD=_tmp1A7;_LL13A: if(_tmp1AD != Cyc_Dict_Absent)goto _LL13C;
_LL13B: return Cyc_Port_var();_LL13C:;_LL13D:(void)_throw(_tmp1AD);_LL139:;}}}
static struct _tuple4*Cyc_Port_lookup_struct_decl(struct Cyc_Port_Cenv*env,struct
_tuple0*n){struct Cyc_Core_Opt*_tmp1AE=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->struct_dict,n);if((
unsigned int)_tmp1AE)return(struct _tuple4*)_tmp1AE->v;else{struct Cyc_Absyn_Aggrdecl*
_tmp1AF=({struct Cyc_Absyn_Aggrdecl*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1->kind=(
void*)((void*)0);_tmp1B1->sc=(void*)((void*)2);_tmp1B1->name=n;_tmp1B1->tvs=0;
_tmp1B1->impl=0;_tmp1B1->attributes=0;_tmp1B1;});struct _tuple4*p=({struct _tuple4*
_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->f1=_tmp1AF;_tmp1B0->f2=0;_tmp1B0;});(
env->gcenv)->struct_dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tuple0*k,struct _tuple4*v))Cyc_Dict_insert)((env->gcenv)->struct_dict,n,p);
return p;}}static struct _tuple4*Cyc_Port_lookup_union_decl(struct Cyc_Port_Cenv*env,
struct _tuple0*n){struct Cyc_Core_Opt*_tmp1B2=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->union_dict,n);if((
unsigned int)_tmp1B2)return(struct _tuple4*)_tmp1B2->v;else{struct Cyc_Absyn_Aggrdecl*
_tmp1B3=({struct Cyc_Absyn_Aggrdecl*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5->kind=(
void*)((void*)1);_tmp1B5->sc=(void*)((void*)2);_tmp1B5->name=n;_tmp1B5->tvs=0;
_tmp1B5->impl=0;_tmp1B5->attributes=0;_tmp1B5;});struct _tuple4*p=({struct _tuple4*
_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4->f1=_tmp1B3;_tmp1B4->f2=0;_tmp1B4;});(
env->gcenv)->union_dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tuple0*k,struct _tuple4*v))Cyc_Dict_insert)((env->gcenv)->union_dict,n,p);return
p;}}struct _tuple6{void*f1;struct _tuple3*f2;};static struct _tuple3 Cyc_Port_lookup_var(
struct Cyc_Port_Cenv*env,struct _tuple0*x){struct _handler_cons _tmp1B6;
_push_handler(& _tmp1B6);{int _tmp1B8=0;if(setjmp(_tmp1B6.handler))_tmp1B8=1;if(!
_tmp1B8){{struct _tuple3*_tmp1BA;struct _tuple6 _tmp1B9=*((struct _tuple6*(*)(struct
Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);_tmp1BA=
_tmp1B9.f2;{struct _tuple3 _tmp1BB=*_tmp1BA;_npop_handler(0);return _tmp1BB;}};
_pop_handler();}else{void*_tmp1B7=(void*)_exn_thrown;void*_tmp1BD=_tmp1B7;_LL13F:
if(_tmp1BD != Cyc_Dict_Absent)goto _LL141;_LL140: return({struct _tuple3 _tmp1BE;
_tmp1BE.f1=Cyc_Port_var();_tmp1BE.f2=Cyc_Port_var();_tmp1BE;});_LL141:;_LL142:(
void)_throw(_tmp1BD);_LL13E:;}}}static struct _tuple6*Cyc_Port_lookup_full_var(
struct Cyc_Port_Cenv*env,struct _tuple0*x){return((struct _tuple6*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);}static int Cyc_Port_declared_var(
struct Cyc_Port_Cenv*env,struct _tuple0*x){return((int(*)(struct Cyc_Dict_Dict*d,
struct _tuple0*k))Cyc_Dict_member)(env->var_dict,x);}static int Cyc_Port_isfn(
struct Cyc_Port_Cenv*env,struct _tuple0*x){struct _handler_cons _tmp1BF;
_push_handler(& _tmp1BF);{int _tmp1C1=0;if(setjmp(_tmp1BF.handler))_tmp1C1=1;if(!
_tmp1C1){{void*_tmp1C3;struct _tuple6 _tmp1C2=*((struct _tuple6*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);_tmp1C3=_tmp1C2.f1;LOOP: {
void*_tmp1C4=_tmp1C3;struct _tuple0*_tmp1C5;_LL144: if(_tmp1C4 <= (void*)4  || *((
int*)_tmp1C4)!= 16)goto _LL146;_tmp1C5=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1C4)->f1;_LL145: _tmp1C3=Cyc_Port_lookup_typedef(env,_tmp1C5);goto LOOP;_LL146:
if(_tmp1C4 <= (void*)4  || *((int*)_tmp1C4)!= 8)goto _LL148;_LL147: {int _tmp1C6=1;
_npop_handler(0);return _tmp1C6;}_LL148:;_LL149: {int _tmp1C7=0;_npop_handler(0);
return _tmp1C7;}_LL143:;}};_pop_handler();}else{void*_tmp1C0=(void*)_exn_thrown;
void*_tmp1C9=_tmp1C0;_LL14B: if(_tmp1C9 != Cyc_Dict_Absent)goto _LL14D;_LL14C:
return 0;_LL14D:;_LL14E:(void)_throw(_tmp1C9);_LL14A:;}}}static int Cyc_Port_isarray(
struct Cyc_Port_Cenv*env,struct _tuple0*x){struct _handler_cons _tmp1CA;
_push_handler(& _tmp1CA);{int _tmp1CC=0;if(setjmp(_tmp1CA.handler))_tmp1CC=1;if(!
_tmp1CC){{void*_tmp1CE;struct _tuple6 _tmp1CD=*((struct _tuple6*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);_tmp1CE=_tmp1CD.f1;LOOP: {
void*_tmp1CF=_tmp1CE;struct _tuple0*_tmp1D0;_LL150: if(_tmp1CF <= (void*)4  || *((
int*)_tmp1CF)!= 16)goto _LL152;_tmp1D0=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1CF)->f1;_LL151: _tmp1CE=Cyc_Port_lookup_typedef(env,_tmp1D0);goto LOOP;_LL152:
if(_tmp1CF <= (void*)4  || *((int*)_tmp1CF)!= 7)goto _LL154;_LL153: {int _tmp1D1=1;
_npop_handler(0);return _tmp1D1;}_LL154:;_LL155: {int _tmp1D2=0;_npop_handler(0);
return _tmp1D2;}_LL14F:;}};_pop_handler();}else{void*_tmp1CB=(void*)_exn_thrown;
void*_tmp1D4=_tmp1CB;_LL157: if(_tmp1D4 != Cyc_Dict_Absent)goto _LL159;_LL158:
return 0;_LL159:;_LL15A:(void)_throw(_tmp1D4);_LL156:;}}}static struct Cyc_Port_Cenv*
Cyc_Port_set_cpos(struct Cyc_Port_Cenv*env,void*cpos){return({struct Cyc_Port_Cenv*
_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->gcenv=env->gcenv;_tmp1D5->var_dict=
env->var_dict;_tmp1D5->cpos=(void*)cpos;_tmp1D5;});}static void Cyc_Port_add_return_type(
struct Cyc_Port_Cenv*env,void*t){(void*)((env->gcenv)->return_type=(void*)t);}
static struct Cyc_Port_Cenv*Cyc_Port_add_var(struct Cyc_Port_Cenv*env,struct _tuple0*
x,void*t,void*qual,void*ctype){return({struct Cyc_Port_Cenv*_tmp1D6=_cycalloc(
sizeof(*_tmp1D6));_tmp1D6->gcenv=env->gcenv;_tmp1D6->var_dict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple0*k,struct _tuple6*v))Cyc_Dict_insert)(env->var_dict,
x,({struct _tuple6*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->f1=t;_tmp1D7->f2=({
struct _tuple3*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8->f1=qual;_tmp1D8->f2=
ctype;_tmp1D8;});_tmp1D7;}));_tmp1D6->cpos=(void*)((void*)env->cpos);_tmp1D6;});}
static void Cyc_Port_add_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n,void*t,
void*ct){(env->gcenv)->typedef_dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k,struct _tuple3*v))Cyc_Dict_insert)((env->gcenv)->typedef_dict,n,({
struct _tuple3*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->f1=t;_tmp1D9->f2=ct;
_tmp1D9;}));}struct _tuple7{void*f1;void*f2;struct Cyc_Position_Segment*f3;};
static void Cyc_Port_register_const_cvar(struct Cyc_Port_Cenv*env,void*new_qual,
void*orig_qual,struct Cyc_Position_Segment*loc){(env->gcenv)->qualifier_edits=({
struct Cyc_List_List*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA->hd=({struct
_tuple7*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB->f1=new_qual;_tmp1DB->f2=
orig_qual;_tmp1DB->f3=loc;_tmp1DB;});_tmp1DA->tl=(env->gcenv)->qualifier_edits;
_tmp1DA;});}static void Cyc_Port_register_ptr_cvars(struct Cyc_Port_Cenv*env,void*
new_ptr,void*orig_ptr,struct Cyc_Position_Segment*loc){(env->gcenv)->pointer_edits=({
struct Cyc_List_List*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC->hd=({struct
_tuple7*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD->f1=new_ptr;_tmp1DD->f2=
orig_ptr;_tmp1DD->f3=loc;_tmp1DD;});_tmp1DC->tl=(env->gcenv)->pointer_edits;
_tmp1DC;});}static void Cyc_Port_register_zeroterm_cvars(struct Cyc_Port_Cenv*env,
void*new_zt,void*orig_zt,struct Cyc_Position_Segment*loc){(env->gcenv)->zeroterm_edits=({
struct Cyc_List_List*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE->hd=({struct
_tuple7*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF->f1=new_zt;_tmp1DF->f2=
orig_zt;_tmp1DF->f3=loc;_tmp1DF;});_tmp1DE->tl=(env->gcenv)->zeroterm_edits;
_tmp1DE;});}static void*Cyc_Port_main_type(){struct _tuple1*arg1=({struct _tuple1*
_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5->f1=0;_tmp1E5->f2=Cyc_Absyn_empty_tqual(
0);_tmp1E5->f3=Cyc_Absyn_sint_typ;_tmp1E5;});struct _tuple1*arg2=({struct _tuple1*
_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4->f1=0;_tmp1E4->f2=Cyc_Absyn_empty_tqual(
0);_tmp1E4->f3=Cyc_Absyn_tagged_typ(Cyc_Absyn_string_typ(Cyc_Absyn_wildtyp(0)),
Cyc_Absyn_wildtyp(0),Cyc_Absyn_empty_tqual(0),((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)());
_tmp1E4;});return(void*)({struct Cyc_Absyn_FnType_struct*_tmp1E0=_cycalloc(
sizeof(*_tmp1E0));_tmp1E0[0]=({struct Cyc_Absyn_FnType_struct _tmp1E1;_tmp1E1.tag=
8;_tmp1E1.f1=({struct Cyc_Absyn_FnInfo _tmp1E2;_tmp1E2.tvars=0;_tmp1E2.effect=0;
_tmp1E2.ret_typ=(void*)Cyc_Absyn_sint_typ;_tmp1E2.args=({struct _tuple1*_tmp1E3[2];
_tmp1E3[1]=arg2;_tmp1E3[0]=arg1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1E3,sizeof(struct _tuple1*),2));});_tmp1E2.c_varargs=0;_tmp1E2.cyc_varargs=
0;_tmp1E2.rgn_po=0;_tmp1E2.attributes=0;_tmp1E2;});_tmp1E1;});_tmp1E0;});}static
void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t);static struct Cyc_Port_Cenv*
Cyc_Port_initial_cenv(){struct Cyc_Port_Cenv*_tmp1E6=Cyc_Port_empty_cenv();
_tmp1E6=Cyc_Port_add_var(_tmp1E6,({struct _tuple0*_tmp1E7=_cycalloc(sizeof(*
_tmp1E7));_tmp1E7->f1=(void*)0;_tmp1E7->f2=_init_tag_arr(_cycalloc(sizeof(struct
_tagged_arr)),"main",sizeof(char),5);_tmp1E7;}),Cyc_Port_main_type(),Cyc_Port_const_ct(),
Cyc_Port_type_to_ctype(_tmp1E6,Cyc_Port_main_type()));return _tmp1E6;}static void
Cyc_Port_region_counts(struct Cyc_Dict_Dict**cts,void*t){void*_tmp1E9=Cyc_Port_compress_ct(
t);struct _tagged_arr*_tmp1EA;void*_tmp1EB;void*_tmp1EC;void*_tmp1ED;void*_tmp1EE;
void*_tmp1EF;void*_tmp1F0;void*_tmp1F1;void*_tmp1F2;void*_tmp1F3;struct Cyc_List_List*
_tmp1F4;_LL15C: if((int)_tmp1E9 != 0)goto _LL15E;_LL15D: goto _LL15F;_LL15E: if((int)
_tmp1E9 != 1)goto _LL160;_LL15F: goto _LL161;_LL160: if((int)_tmp1E9 != 2)goto _LL162;
_LL161: goto _LL163;_LL162: if((int)_tmp1E9 != 3)goto _LL164;_LL163: goto _LL165;_LL164:
if((int)_tmp1E9 != 4)goto _LL166;_LL165: goto _LL167;_LL166: if((int)_tmp1E9 != 5)goto
_LL168;_LL167: goto _LL169;_LL168: if((int)_tmp1E9 != 6)goto _LL16A;_LL169: goto _LL16B;
_LL16A: if(_tmp1E9 <= (void*)10  || *((int*)_tmp1E9)!= 4)goto _LL16C;_LL16B: goto
_LL16D;_LL16C: if(_tmp1E9 <= (void*)10  || *((int*)_tmp1E9)!= 3)goto _LL16E;_LL16D:
goto _LL16F;_LL16E: if(_tmp1E9 <= (void*)10  || *((int*)_tmp1E9)!= 6)goto _LL170;
_LL16F: goto _LL171;_LL170: if((int)_tmp1E9 != 8)goto _LL172;_LL171: goto _LL173;_LL172:
if((int)_tmp1E9 != 9)goto _LL174;_LL173: goto _LL175;_LL174: if((int)_tmp1E9 != 7)goto
_LL176;_LL175: return;_LL176: if(_tmp1E9 <= (void*)10  || *((int*)_tmp1E9)!= 0)goto
_LL178;_tmp1EA=((struct Cyc_Port_RgnVar_ct_struct*)_tmp1E9)->f1;_LL177: if(!((int(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_member)(*cts,_tmp1EA))*cts=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,int*v))Cyc_Dict_insert)(*
cts,_tmp1EA,(int*)({int*_tmp1F5=_cycalloc_atomic(sizeof(*_tmp1F5));_tmp1F5[0]=0;
_tmp1F5;}));{int*_tmp1F6=((int*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))
Cyc_Dict_lookup)(*cts,_tmp1EA);*_tmp1F6=*_tmp1F6 + 1;return;}_LL178: if(_tmp1E9 <= (
void*)10  || *((int*)_tmp1E9)!= 1)goto _LL17A;_tmp1EB=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp1E9)->f1;_tmp1EC=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp1E9)->f2;_tmp1ED=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp1E9)->f3;_tmp1EE=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp1E9)->f4;_tmp1EF=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp1E9)->f5;_LL179:
Cyc_Port_region_counts(cts,_tmp1EB);Cyc_Port_region_counts(cts,_tmp1EE);return;
_LL17A: if(_tmp1E9 <= (void*)10  || *((int*)_tmp1E9)!= 2)goto _LL17C;_tmp1F0=(void*)((
struct Cyc_Port_Array_ct_struct*)_tmp1E9)->f1;_tmp1F1=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp1E9)->f2;_tmp1F2=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp1E9)->f3;
_LL17B: Cyc_Port_region_counts(cts,_tmp1F0);return;_LL17C: if(_tmp1E9 <= (void*)10
 || *((int*)_tmp1E9)!= 5)goto _LL15B;_tmp1F3=(void*)((struct Cyc_Port_Fn_ct_struct*)
_tmp1E9)->f1;_tmp1F4=((struct Cyc_Port_Fn_ct_struct*)_tmp1E9)->f2;_LL17D: Cyc_Port_region_counts(
cts,_tmp1F3);for(0;(unsigned int)_tmp1F4;_tmp1F4=_tmp1F4->tl){Cyc_Port_region_counts(
cts,(void*)_tmp1F4->hd);}return;_LL15B:;}static void Cyc_Port_register_rgns(struct
Cyc_Port_Cenv*env,struct Cyc_Dict_Dict*counts,int fn_res,void*t,void*c){c=Cyc_Port_compress_ct(
c);{struct _tuple3 _tmp1F8=({struct _tuple3 _tmp1F7;_tmp1F7.f1=t;_tmp1F7.f2=c;
_tmp1F7;});void*_tmp1F9;struct Cyc_Absyn_PtrInfo _tmp1FA;void*_tmp1FB;struct Cyc_Absyn_PtrAtts
_tmp1FC;void*_tmp1FD;struct Cyc_Absyn_PtrLoc*_tmp1FE;void*_tmp1FF;void*_tmp200;
void*_tmp201;void*_tmp202;struct Cyc_Absyn_ArrayInfo _tmp203;void*_tmp204;void*
_tmp205;void*_tmp206;void*_tmp207;struct Cyc_Absyn_FnInfo _tmp208;void*_tmp209;
struct Cyc_List_List*_tmp20A;void*_tmp20B;void*_tmp20C;struct Cyc_List_List*
_tmp20D;_LL17F: _tmp1F9=_tmp1F8.f1;if(_tmp1F9 <= (void*)4  || *((int*)_tmp1F9)!= 4)
goto _LL181;_tmp1FA=((struct Cyc_Absyn_PointerType_struct*)_tmp1F9)->f1;_tmp1FB=(
void*)_tmp1FA.elt_typ;_tmp1FC=_tmp1FA.ptr_atts;_tmp1FD=(void*)_tmp1FC.rgn;
_tmp1FE=_tmp1FC.ptrloc;_tmp1FF=_tmp1F8.f2;if(_tmp1FF <= (void*)10  || *((int*)
_tmp1FF)!= 1)goto _LL181;_tmp200=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp1FF)->f1;
_tmp201=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp1FF)->f4;_LL180: Cyc_Port_register_rgns(
env,counts,fn_res,_tmp1FB,_tmp200);{struct Cyc_Position_Segment*_tmp20E=(
unsigned int)_tmp1FE?_tmp1FE->rgn_loc: 0;_tmp201=Cyc_Port_compress_ct(_tmp201);{
struct _tuple3 _tmp210=({struct _tuple3 _tmp20F;_tmp20F.f1=_tmp1FD;_tmp20F.f2=
_tmp201;_tmp20F;});void*_tmp211;void*_tmp212;void*_tmp213;void*_tmp214;struct
_tagged_arr*_tmp215;_LL188: _tmp211=_tmp210.f1;if(_tmp211 <= (void*)4  || *((int*)
_tmp211)!= 0)goto _LL18A;_tmp212=_tmp210.f2;if((int)_tmp212 != 7)goto _LL18A;if(!(!
fn_res))goto _LL18A;_LL189:(env->gcenv)->edits=({struct Cyc_List_List*_tmp216=
_cycalloc(sizeof(*_tmp216));_tmp216->hd=({struct Cyc_Port_Edit*_tmp217=_cycalloc(
sizeof(*_tmp217));_tmp217->loc=_tmp20E;_tmp217->old_string=({const char*_tmp219="`H ";
_tag_arr(_tmp219,sizeof(char),_get_zero_arr_size(_tmp219,4));});_tmp217->new_string=({
const char*_tmp218="";_tag_arr(_tmp218,sizeof(char),_get_zero_arr_size(_tmp218,1));});
_tmp217;});_tmp216->tl=(env->gcenv)->edits;_tmp216;});goto _LL187;_LL18A: _tmp213=
_tmp210.f1;if(_tmp213 <= (void*)4  || *((int*)_tmp213)!= 0)goto _LL18C;_tmp214=
_tmp210.f2;if(_tmp214 <= (void*)10  || *((int*)_tmp214)!= 0)goto _LL18C;_tmp215=((
struct Cyc_Port_RgnVar_ct_struct*)_tmp214)->f1;_LL18B: {int _tmp21A=*((int*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(counts,_tmp215);if(
_tmp21A > 1)(env->gcenv)->edits=({struct Cyc_List_List*_tmp21B=_cycalloc(sizeof(*
_tmp21B));_tmp21B->hd=({struct Cyc_Port_Edit*_tmp21C=_cycalloc(sizeof(*_tmp21C));
_tmp21C->loc=_tmp20E;_tmp21C->old_string=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp220;_tmp220.tag=0;_tmp220.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp215);{
void*_tmp21E[1]={& _tmp220};Cyc_aprintf(({const char*_tmp21F="%s ";_tag_arr(
_tmp21F,sizeof(char),_get_zero_arr_size(_tmp21F,4));}),_tag_arr(_tmp21E,sizeof(
void*),1));}});_tmp21C->new_string=({const char*_tmp21D="";_tag_arr(_tmp21D,
sizeof(char),_get_zero_arr_size(_tmp21D,1));});_tmp21C;});_tmp21B->tl=(env->gcenv)->edits;
_tmp21B;});goto _LL187;}_LL18C:;_LL18D: goto _LL187;_LL187:;}goto _LL17E;}_LL181:
_tmp202=_tmp1F8.f1;if(_tmp202 <= (void*)4  || *((int*)_tmp202)!= 7)goto _LL183;
_tmp203=((struct Cyc_Absyn_ArrayType_struct*)_tmp202)->f1;_tmp204=(void*)_tmp203.elt_type;
_tmp205=_tmp1F8.f2;if(_tmp205 <= (void*)10  || *((int*)_tmp205)!= 2)goto _LL183;
_tmp206=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp205)->f1;_LL182: Cyc_Port_register_rgns(
env,counts,fn_res,_tmp204,_tmp206);goto _LL17E;_LL183: _tmp207=_tmp1F8.f1;if(
_tmp207 <= (void*)4  || *((int*)_tmp207)!= 8)goto _LL185;_tmp208=((struct Cyc_Absyn_FnType_struct*)
_tmp207)->f1;_tmp209=(void*)_tmp208.ret_typ;_tmp20A=_tmp208.args;_tmp20B=_tmp1F8.f2;
if(_tmp20B <= (void*)10  || *((int*)_tmp20B)!= 5)goto _LL185;_tmp20C=(void*)((
struct Cyc_Port_Fn_ct_struct*)_tmp20B)->f1;_tmp20D=((struct Cyc_Port_Fn_ct_struct*)
_tmp20B)->f2;_LL184: Cyc_Port_register_rgns(env,counts,1,_tmp209,_tmp20C);for(0;
_tmp20A != 0  && _tmp20D != 0;(_tmp20A=_tmp20A->tl,_tmp20D=_tmp20D->tl)){void*
_tmp222;struct _tuple1 _tmp221=*((struct _tuple1*)_tmp20A->hd);_tmp222=_tmp221.f3;{
void*_tmp223=(void*)_tmp20D->hd;Cyc_Port_register_rgns(env,counts,0,_tmp222,
_tmp223);}}goto _LL17E;_LL185:;_LL186: goto _LL17E;_LL17E:;}}static void*Cyc_Port_gen_exp(
struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e);static void Cyc_Port_gen_stmt(
struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s);static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(
struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d);static int Cyc_Port_is_char(struct
Cyc_Port_Cenv*env,void*t){void*_tmp224=t;struct _tuple0*_tmp225;void*_tmp226;
_LL18F: if(_tmp224 <= (void*)4  || *((int*)_tmp224)!= 16)goto _LL191;_tmp225=((
struct Cyc_Absyn_TypedefType_struct*)_tmp224)->f1;_LL190:(*_tmp225).f1=(void*)0;
return Cyc_Port_is_char(env,Cyc_Port_lookup_typedef(env,_tmp225));_LL191: if(
_tmp224 <= (void*)4  || *((int*)_tmp224)!= 5)goto _LL193;_tmp226=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp224)->f2;if((int)_tmp226 != 0)goto _LL193;_LL192: return 1;_LL193:;_LL194: return
0;_LL18E:;}static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t){
void*_tmp227=t;struct _tuple0*_tmp228;struct Cyc_Absyn_PtrInfo _tmp229;void*_tmp22A;
struct Cyc_Absyn_Tqual _tmp22B;struct Cyc_Absyn_PtrAtts _tmp22C;void*_tmp22D;struct
Cyc_Absyn_Conref*_tmp22E;struct Cyc_Absyn_Conref*_tmp22F;struct Cyc_Absyn_Conref*
_tmp230;struct Cyc_Absyn_PtrLoc*_tmp231;struct Cyc_Absyn_ArrayInfo _tmp232;void*
_tmp233;struct Cyc_Absyn_Tqual _tmp234;struct Cyc_Absyn_Conref*_tmp235;struct Cyc_Position_Segment*
_tmp236;struct Cyc_Absyn_FnInfo _tmp237;void*_tmp238;struct Cyc_List_List*_tmp239;
struct Cyc_Absyn_AggrInfo _tmp23A;void*_tmp23B;struct Cyc_List_List*_tmp23C;_LL196:
if(_tmp227 <= (void*)4  || *((int*)_tmp227)!= 16)goto _LL198;_tmp228=((struct Cyc_Absyn_TypedefType_struct*)
_tmp227)->f1;_LL197:(*_tmp228).f1=(void*)0;return Cyc_Port_lookup_typedef_ctype(
env,_tmp228);_LL198: if((int)_tmp227 != 0)goto _LL19A;_LL199: return Cyc_Port_void_ct();
_LL19A: if(_tmp227 <= (void*)4  || *((int*)_tmp227)!= 4)goto _LL19C;_tmp229=((struct
Cyc_Absyn_PointerType_struct*)_tmp227)->f1;_tmp22A=(void*)_tmp229.elt_typ;
_tmp22B=_tmp229.elt_tq;_tmp22C=_tmp229.ptr_atts;_tmp22D=(void*)_tmp22C.rgn;
_tmp22E=_tmp22C.nullable;_tmp22F=_tmp22C.bounds;_tmp230=_tmp22C.zero_term;
_tmp231=_tmp22C.ptrloc;_LL19B: {struct Cyc_Position_Segment*_tmp23D=(unsigned int)
_tmp231?_tmp231->ptr_loc: 0;struct Cyc_Position_Segment*_tmp23E=(unsigned int)
_tmp231?_tmp231->rgn_loc: 0;struct Cyc_Position_Segment*_tmp23F=(unsigned int)
_tmp231?_tmp231->zt_loc: 0;void*_tmp240=Cyc_Port_type_to_ctype(env,_tmp22A);void*
new_rgn;{void*_tmp241=_tmp22D;struct Cyc_Absyn_Tvar*_tmp242;struct Cyc_Absyn_Tvar
_tmp243;struct _tagged_arr*_tmp244;_LL1AF: if((int)_tmp241 != 2)goto _LL1B1;_LL1B0:
new_rgn=Cyc_Port_heap_ct();goto _LL1AE;_LL1B1: if(_tmp241 <= (void*)4  || *((int*)
_tmp241)!= 1)goto _LL1B3;_tmp242=((struct Cyc_Absyn_VarType_struct*)_tmp241)->f1;
_tmp243=*_tmp242;_tmp244=_tmp243.name;_LL1B2: new_rgn=Cyc_Port_rgnvar_ct(_tmp244);
goto _LL1AE;_LL1B3: if(_tmp241 <= (void*)4  || *((int*)_tmp241)!= 0)goto _LL1B5;
_LL1B4: if(Cyc_Port_in_fn_arg(env))new_rgn=Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var());
else{if(Cyc_Port_in_fn_res(env) || Cyc_Port_in_toplevel(env))new_rgn=Cyc_Port_heap_ct();
else{new_rgn=Cyc_Port_var();}}goto _LL1AE;_LL1B5:;_LL1B6: new_rgn=Cyc_Port_heap_ct();
goto _LL1AE;_LL1AE:;}{int _tmp245=Cyc_Absyn_conref_val(_tmp22F)== (void*)0;int
_tmp246=(void*)_tmp230->v == (void*)0?Cyc_Port_is_char(env,_tmp22A):((int(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp230);if((env->gcenv)->porting){
void*_tmp247=Cyc_Port_var();Cyc_Port_register_const_cvar(env,_tmp247,_tmp22B.print_const?
Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp22B.loc);if(_tmp22B.print_const)
Cyc_Port_unify_ct(_tmp247,Cyc_Port_const_ct());{void*_tmp248=Cyc_Port_var();Cyc_Port_register_ptr_cvars(
env,_tmp248,_tmp245?Cyc_Port_fat_ct(): Cyc_Port_thin_ct(),_tmp23D);if(_tmp245)Cyc_Port_unify_ct(
_tmp248,Cyc_Port_fat_ct());{void*_tmp249=Cyc_Port_var();Cyc_Port_register_zeroterm_cvars(
env,_tmp249,_tmp246?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct(),_tmp23F);if((void*)
_tmp230->v != (void*)0)Cyc_Port_unify_ct(_tmp249,((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp230)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());
return Cyc_Port_ptr_ct(_tmp240,_tmp247,_tmp248,new_rgn,_tmp249);}}}else{return Cyc_Port_ptr_ct(
_tmp240,_tmp22B.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp245?
Cyc_Port_fat_ct(): Cyc_Port_thin_ct(),new_rgn,_tmp246?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}}}
_LL19C: if(_tmp227 <= (void*)4  || *((int*)_tmp227)!= 5)goto _LL19E;_LL19D: goto
_LL19F;_LL19E: if((int)_tmp227 != 1)goto _LL1A0;_LL19F: goto _LL1A1;_LL1A0: if(_tmp227
<= (void*)4  || *((int*)_tmp227)!= 6)goto _LL1A2;_LL1A1: return Cyc_Port_arith_ct();
_LL1A2: if(_tmp227 <= (void*)4  || *((int*)_tmp227)!= 7)goto _LL1A4;_tmp232=((struct
Cyc_Absyn_ArrayType_struct*)_tmp227)->f1;_tmp233=(void*)_tmp232.elt_type;_tmp234=
_tmp232.tq;_tmp235=_tmp232.zero_term;_tmp236=_tmp232.zt_loc;_LL1A3: {void*
_tmp24A=Cyc_Port_type_to_ctype(env,_tmp233);int _tmp24B=(void*)_tmp235->v == (void*)
0?0:((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp235);if((env->gcenv)->porting){
void*_tmp24C=Cyc_Port_var();Cyc_Port_register_const_cvar(env,_tmp24C,_tmp234.print_const?
Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp234.loc);{void*_tmp24D=Cyc_Port_var();
Cyc_Port_register_zeroterm_cvars(env,_tmp24D,_tmp24B?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct(),
_tmp236);if((void*)_tmp235->v != (void*)0)Cyc_Port_unify_ct(_tmp24D,((int(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp235)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());
return Cyc_Port_array_ct(_tmp24A,_tmp24C,_tmp24D);}}else{return Cyc_Port_array_ct(
_tmp24A,_tmp234.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp24B?
Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}}_LL1A4: if(_tmp227 <= (void*)4  || *((
int*)_tmp227)!= 8)goto _LL1A6;_tmp237=((struct Cyc_Absyn_FnType_struct*)_tmp227)->f1;
_tmp238=(void*)_tmp237.ret_typ;_tmp239=_tmp237.args;_LL1A5: {void*_tmp24E=Cyc_Port_type_to_ctype(
Cyc_Port_set_cpos(env,(void*)0),_tmp238);struct Cyc_Port_Cenv*_tmp24F=Cyc_Port_set_cpos(
env,(void*)1);struct Cyc_List_List*cargs=0;for(0;_tmp239 != 0;_tmp239=_tmp239->tl){
struct Cyc_Absyn_Tqual _tmp251;void*_tmp252;struct _tuple1 _tmp250=*((struct _tuple1*)
_tmp239->hd);_tmp251=_tmp250.f2;_tmp252=_tmp250.f3;cargs=({struct Cyc_List_List*
_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253->hd=(void*)Cyc_Port_type_to_ctype(
_tmp24F,_tmp252);_tmp253->tl=cargs;_tmp253;});}return Cyc_Port_fn_ct(_tmp24E,Cyc_List_imp_rev(
cargs));}_LL1A6: if(_tmp227 <= (void*)4  || *((int*)_tmp227)!= 10)goto _LL1A8;
_tmp23A=((struct Cyc_Absyn_AggrType_struct*)_tmp227)->f1;_tmp23B=(void*)_tmp23A.aggr_info;
_LL1A7: {void*_tmp254=_tmp23B;void*_tmp255;struct _tuple0*_tmp256;void*_tmp257;
struct _tuple0*_tmp258;struct Cyc_Absyn_Aggrdecl**_tmp259;_LL1B8: if(*((int*)
_tmp254)!= 0)goto _LL1BA;_tmp255=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp254)->f1;if((int)_tmp255 != 0)goto _LL1BA;_tmp256=((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp254)->f2;_LL1B9:(*_tmp256).f1=(void*)0;{struct _tuple4*_tmp25A=Cyc_Port_lookup_struct_decl(
env,_tmp256);return Cyc_Port_known_aggr_ct(_tmp25A);}_LL1BA: if(*((int*)_tmp254)!= 
0)goto _LL1BC;_tmp257=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp254)->f1;
if((int)_tmp257 != 1)goto _LL1BC;_tmp258=((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp254)->f2;_LL1BB:(*_tmp258).f1=(void*)0;{struct _tuple4*_tmp25B=Cyc_Port_lookup_union_decl(
env,_tmp258);return Cyc_Port_known_aggr_ct(_tmp25B);}_LL1BC: if(*((int*)_tmp254)!= 
1)goto _LL1B7;_tmp259=((struct Cyc_Absyn_KnownAggr_struct*)_tmp254)->f1;_LL1BD: {
struct Cyc_Absyn_Aggrdecl*_tmp25C=*_tmp259;void*_tmp25D=(void*)_tmp25C->kind;
_LL1BF: if((int)_tmp25D != 0)goto _LL1C1;_LL1C0: {struct _tuple4*_tmp25E=Cyc_Port_lookup_struct_decl(
env,_tmp25C->name);return Cyc_Port_known_aggr_ct(_tmp25E);}_LL1C1: if((int)_tmp25D
!= 1)goto _LL1BE;_LL1C2: {struct _tuple4*_tmp25F=Cyc_Port_lookup_union_decl(env,
_tmp25C->name);return Cyc_Port_known_aggr_ct(_tmp25F);}_LL1BE:;}_LL1B7:;}_LL1A8:
if(_tmp227 <= (void*)4  || *((int*)_tmp227)!= 12)goto _LL1AA;_LL1A9: return Cyc_Port_arith_ct();
_LL1AA: if(_tmp227 <= (void*)4  || *((int*)_tmp227)!= 13)goto _LL1AC;_tmp23C=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp227)->f1;_LL1AB: for(0;(unsigned int)
_tmp23C;_tmp23C=_tmp23C->tl){(*((struct Cyc_Absyn_Enumfield*)_tmp23C->hd)->name).f1=(
void*)0;env=Cyc_Port_add_var(env,((struct Cyc_Absyn_Enumfield*)_tmp23C->hd)->name,
Cyc_Absyn_sint_typ,Cyc_Port_const_ct(),Cyc_Port_arith_ct());}return Cyc_Port_arith_ct();
_LL1AC:;_LL1AD: return Cyc_Port_arith_ct();_LL195:;}static void*Cyc_Port_gen_primop(
struct Cyc_Port_Cenv*env,void*p,struct Cyc_List_List*args){void*_tmp260=Cyc_Port_compress_ct((
void*)((struct Cyc_List_List*)_check_null(args))->hd);struct Cyc_List_List*_tmp261=
args->tl;void*_tmp262=p;_LL1C4: if((int)_tmp262 != 0)goto _LL1C6;_LL1C5: {void*
_tmp263=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(_tmp261))->hd);
if(Cyc_Port_leq(_tmp260,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()))){Cyc_Port_leq(_tmp263,Cyc_Port_arith_ct());
return _tmp260;}else{if(Cyc_Port_leq(_tmp263,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),
Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()))){Cyc_Port_leq(_tmp260,Cyc_Port_arith_ct());
return _tmp263;}else{Cyc_Port_leq(_tmp260,Cyc_Port_arith_ct());Cyc_Port_leq(
_tmp263,Cyc_Port_arith_ct());return Cyc_Port_arith_ct();}}}_LL1C6: if((int)_tmp262
!= 2)goto _LL1C8;_LL1C7: if(_tmp261 == 0){Cyc_Port_leq(_tmp260,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();}else{void*_tmp264=Cyc_Port_compress_ct((void*)_tmp261->hd);
if(Cyc_Port_leq(_tmp260,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()))){if(Cyc_Port_leq(_tmp264,Cyc_Port_ptr_ct(Cyc_Port_var(),
Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var())))return Cyc_Port_arith_ct();
else{Cyc_Port_leq(_tmp264,Cyc_Port_arith_ct());return _tmp260;}}else{Cyc_Port_leq(
_tmp260,Cyc_Port_arith_ct());Cyc_Port_leq(_tmp260,Cyc_Port_arith_ct());return Cyc_Port_arith_ct();}}
_LL1C8: if((int)_tmp262 != 1)goto _LL1CA;_LL1C9: goto _LL1CB;_LL1CA: if((int)_tmp262 != 
3)goto _LL1CC;_LL1CB: goto _LL1CD;_LL1CC: if((int)_tmp262 != 4)goto _LL1CE;_LL1CD: goto
_LL1CF;_LL1CE: if((int)_tmp262 != 5)goto _LL1D0;_LL1CF: goto _LL1D1;_LL1D0: if((int)
_tmp262 != 6)goto _LL1D2;_LL1D1: goto _LL1D3;_LL1D2: if((int)_tmp262 != 8)goto _LL1D4;
_LL1D3: goto _LL1D5;_LL1D4: if((int)_tmp262 != 7)goto _LL1D6;_LL1D5: goto _LL1D7;_LL1D6:
if((int)_tmp262 != 10)goto _LL1D8;_LL1D7: goto _LL1D9;_LL1D8: if((int)_tmp262 != 9)
goto _LL1DA;_LL1D9: goto _LL1DB;_LL1DA: if((int)_tmp262 != 13)goto _LL1DC;_LL1DB: goto
_LL1DD;_LL1DC: if((int)_tmp262 != 14)goto _LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if((int)
_tmp262 != 15)goto _LL1E0;_LL1DF: goto _LL1E1;_LL1E0: if((int)_tmp262 != 16)goto _LL1E2;
_LL1E1: goto _LL1E3;_LL1E2: if((int)_tmp262 != 17)goto _LL1E4;_LL1E3: goto _LL1E5;
_LL1E4: if((int)_tmp262 != 18)goto _LL1E6;_LL1E5: Cyc_Port_leq(_tmp260,Cyc_Port_arith_ct());
Cyc_Port_leq((void*)((struct Cyc_List_List*)_check_null(_tmp261))->hd,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();_LL1E6: if((int)_tmp262 != 11)goto _LL1E8;_LL1E7: goto
_LL1E9;_LL1E8: if((int)_tmp262 != 12)goto _LL1EA;_LL1E9: Cyc_Port_leq(_tmp260,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();_LL1EA: if((int)_tmp262 != 19)goto _LL1C3;_LL1EB:({void*
_tmp265[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp266=".size primop";_tag_arr(_tmp266,sizeof(char),
_get_zero_arr_size(_tmp266,13));}),_tag_arr(_tmp265,sizeof(void*),0));});_LL1C3:;}
static struct _tuple3 Cyc_Port_gen_lhs(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*
e){void*_tmp267=(void*)e->r;struct _tuple0*_tmp268;struct _tuple0*_tmp269;struct
Cyc_Absyn_Exp*_tmp26A;struct Cyc_Absyn_Exp*_tmp26B;struct Cyc_Absyn_Exp*_tmp26C;
struct Cyc_Absyn_Exp*_tmp26D;struct _tagged_arr*_tmp26E;struct Cyc_Absyn_Exp*
_tmp26F;struct _tagged_arr*_tmp270;_LL1ED: if(*((int*)_tmp267)!= 1)goto _LL1EF;
_tmp268=((struct Cyc_Absyn_Var_e_struct*)_tmp267)->f1;_LL1EE: _tmp269=_tmp268;goto
_LL1F0;_LL1EF: if(*((int*)_tmp267)!= 2)goto _LL1F1;_tmp269=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp267)->f1;_LL1F0:(*_tmp269).f1=(void*)0;return Cyc_Port_lookup_var(env,_tmp269);
_LL1F1: if(*((int*)_tmp267)!= 25)goto _LL1F3;_tmp26A=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp267)->f1;_tmp26B=((struct Cyc_Absyn_Subscript_e_struct*)_tmp267)->f2;_LL1F2: {
void*_tmp271=Cyc_Port_var();void*_tmp272=Cyc_Port_var();void*_tmp273=Cyc_Port_gen_exp(
env,_tmp26A);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp26B),Cyc_Port_arith_ct());{
void*_tmp274=Cyc_Port_ptr_ct(_tmp271,_tmp272,Cyc_Port_fat_ct(),Cyc_Port_var(),
Cyc_Port_var());Cyc_Port_leq(_tmp273,_tmp274);return({struct _tuple3 _tmp275;
_tmp275.f1=_tmp272;_tmp275.f2=_tmp271;_tmp275;});}}_LL1F3: if(*((int*)_tmp267)!= 
22)goto _LL1F5;_tmp26C=((struct Cyc_Absyn_Deref_e_struct*)_tmp267)->f1;_LL1F4: {
void*_tmp276=Cyc_Port_var();void*_tmp277=Cyc_Port_var();void*_tmp278=Cyc_Port_ptr_ct(
_tmp276,_tmp277,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var());Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp26C),_tmp278);return({struct _tuple3 _tmp279;_tmp279.f1=_tmp277;_tmp279.f2=
_tmp276;_tmp279;});}_LL1F5: if(*((int*)_tmp267)!= 23)goto _LL1F7;_tmp26D=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmp267)->f1;_tmp26E=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp267)->f2;_LL1F6: {void*_tmp27A=Cyc_Port_var();void*_tmp27B=Cyc_Port_var();
void*_tmp27C=Cyc_Port_gen_exp(env,_tmp26D);Cyc_Port_leq(Cyc_Port_gen_exp(env,
_tmp26D),Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp27D[1];_tmp27D[0]=({
struct Cyc_Port_Cfield*_tmp27E=_cycalloc(sizeof(*_tmp27E));_tmp27E->qual=(void*)
_tmp27B;_tmp27E->f=_tmp26E;_tmp27E->type=(void*)_tmp27A;_tmp27E;});((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp27D,sizeof(struct Cyc_Port_Cfield*),
1));})));return({struct _tuple3 _tmp27F;_tmp27F.f1=_tmp27B;_tmp27F.f2=_tmp27A;
_tmp27F;});}_LL1F7: if(*((int*)_tmp267)!= 24)goto _LL1F9;_tmp26F=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp267)->f1;_tmp270=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp267)->f2;_LL1F8: {
void*_tmp280=Cyc_Port_var();void*_tmp281=Cyc_Port_var();void*_tmp282=Cyc_Port_gen_exp(
env,_tmp26F);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp26F),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(({
struct Cyc_Port_Cfield*_tmp283[1];_tmp283[0]=({struct Cyc_Port_Cfield*_tmp284=
_cycalloc(sizeof(*_tmp284));_tmp284->qual=(void*)_tmp281;_tmp284->f=_tmp270;
_tmp284->type=(void*)_tmp280;_tmp284;});((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp283,sizeof(struct Cyc_Port_Cfield*),1));})),
Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));return({struct
_tuple3 _tmp285;_tmp285.f1=_tmp281;_tmp285.f2=_tmp280;_tmp285;});}_LL1F9:;_LL1FA:({
void*_tmp286[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)((
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp289;_tmp289.tag=0;_tmp289.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e));{void*_tmp287[1]={&
_tmp289};Cyc_aprintf(({const char*_tmp288="gen_lhs: %s";_tag_arr(_tmp288,sizeof(
char),_get_zero_arr_size(_tmp288,12));}),_tag_arr(_tmp287,sizeof(void*),1));}}),
_tag_arr(_tmp286,sizeof(void*),0));});_LL1EC:;}static void*Cyc_Port_gen_exp(
struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){void*_tmp28A=(void*)e->r;void*
_tmp28B;void*_tmp28C;void*_tmp28D;void*_tmp28E;void*_tmp28F;int _tmp290;void*
_tmp291;void*_tmp292;void*_tmp293;struct _tuple0*_tmp294;struct _tuple0*_tmp295;
void*_tmp296;struct Cyc_List_List*_tmp297;struct Cyc_Absyn_Exp*_tmp298;struct Cyc_Core_Opt*
_tmp299;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Absyn_Exp*_tmp29B;void*_tmp29C;
struct Cyc_Absyn_Exp*_tmp29D;struct Cyc_Absyn_Exp*_tmp29E;struct Cyc_Absyn_Exp*
_tmp29F;struct Cyc_Absyn_Exp*_tmp2A0;struct Cyc_Absyn_Exp*_tmp2A1;struct Cyc_Absyn_Exp*
_tmp2A2;struct Cyc_Absyn_Exp*_tmp2A3;struct Cyc_Absyn_Exp*_tmp2A4;struct Cyc_Absyn_Exp*
_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A6;struct Cyc_List_List*_tmp2A7;struct Cyc_Absyn_Exp*
_tmp2A8;struct Cyc_List_List*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2AA;void*_tmp2AB;
struct Cyc_Absyn_Exp*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*
_tmp2AE;struct Cyc_Absyn_Exp*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2B0;struct Cyc_Absyn_Exp*
_tmp2B1;struct _tagged_arr*_tmp2B2;struct Cyc_Absyn_Exp*_tmp2B3;struct _tagged_arr*
_tmp2B4;struct Cyc_Absyn_MallocInfo _tmp2B5;void**_tmp2B6;struct Cyc_Absyn_Exp*
_tmp2B7;struct Cyc_Absyn_Stmt*_tmp2B8;_LL1FC: if(*((int*)_tmp28A)!= 0)goto _LL1FE;
_tmp28B=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp28A)->f1;if(_tmp28B <= (void*)
1  || *((int*)_tmp28B)!= 0)goto _LL1FE;_LL1FD: goto _LL1FF;_LL1FE: if(*((int*)_tmp28A)
!= 0)goto _LL200;_tmp28C=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp28A)->f1;
if(_tmp28C <= (void*)1  || *((int*)_tmp28C)!= 1)goto _LL200;_LL1FF: goto _LL201;
_LL200: if(*((int*)_tmp28A)!= 0)goto _LL202;_tmp28D=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp28A)->f1;if(_tmp28D <= (void*)1  || *((int*)_tmp28D)!= 3)goto _LL202;_LL201:
goto _LL203;_LL202: if(*((int*)_tmp28A)!= 18)goto _LL204;_LL203: goto _LL205;_LL204:
if(*((int*)_tmp28A)!= 19)goto _LL206;_LL205: goto _LL207;_LL206: if(*((int*)_tmp28A)
!= 20)goto _LL208;_LL207: goto _LL209;_LL208: if(*((int*)_tmp28A)!= 33)goto _LL20A;
_LL209: goto _LL20B;_LL20A: if(*((int*)_tmp28A)!= 34)goto _LL20C;_LL20B: goto _LL20D;
_LL20C: if(*((int*)_tmp28A)!= 0)goto _LL20E;_tmp28E=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp28A)->f1;if(_tmp28E <= (void*)1  || *((int*)_tmp28E)!= 4)goto _LL20E;_LL20D:
return Cyc_Port_arith_ct();_LL20E: if(*((int*)_tmp28A)!= 0)goto _LL210;_tmp28F=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp28A)->f1;if(_tmp28F <= (void*)1  || *((
int*)_tmp28F)!= 2)goto _LL210;_tmp290=((struct Cyc_Absyn_Int_c_struct*)_tmp28F)->f2;
if(_tmp290 != 0)goto _LL210;_LL20F: return Cyc_Port_zero_ct();_LL210: if(*((int*)
_tmp28A)!= 0)goto _LL212;_tmp291=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp28A)->f1;
if(_tmp291 <= (void*)1  || *((int*)_tmp291)!= 2)goto _LL212;_LL211: return Cyc_Port_arith_ct();
_LL212: if(*((int*)_tmp28A)!= 0)goto _LL214;_tmp292=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp28A)->f1;if(_tmp292 <= (void*)1  || *((int*)_tmp292)!= 5)goto _LL214;_LL213:
return Cyc_Port_ptr_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_fat_ct(),
Cyc_Port_heap_ct(),Cyc_Port_zterm_ct());_LL214: if(*((int*)_tmp28A)!= 0)goto
_LL216;_tmp293=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp28A)->f1;if((int)
_tmp293 != 0)goto _LL216;_LL215: return Cyc_Port_zero_ct();_LL216: if(*((int*)_tmp28A)
!= 1)goto _LL218;_tmp294=((struct Cyc_Absyn_Var_e_struct*)_tmp28A)->f1;_LL217:
_tmp295=_tmp294;goto _LL219;_LL218: if(*((int*)_tmp28A)!= 2)goto _LL21A;_tmp295=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp28A)->f1;_LL219:(*_tmp295).f1=(void*)0;{
void*_tmp2BA;struct _tuple3 _tmp2B9=Cyc_Port_lookup_var(env,_tmp295);_tmp2BA=
_tmp2B9.f2;if(Cyc_Port_isfn(env,_tmp295)){_tmp2BA=Cyc_Port_instantiate(_tmp2BA);
return Cyc_Port_ptr_ct(_tmp2BA,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_heap_ct(),
Cyc_Port_nozterm_ct());}else{if(Cyc_Port_isarray(env,_tmp295)){void*_tmp2BB=Cyc_Port_var();
void*_tmp2BC=Cyc_Port_var();void*_tmp2BD=Cyc_Port_var();void*_tmp2BE=Cyc_Port_array_ct(
_tmp2BB,_tmp2BC,_tmp2BD);Cyc_Port_unifies(_tmp2BA,_tmp2BE);{void*_tmp2BF=Cyc_Port_ptr_ct(
_tmp2BB,_tmp2BC,Cyc_Port_fat_ct(),Cyc_Port_var(),_tmp2BD);return _tmp2BF;}}else{
return _tmp2BA;}}}_LL21A: if(*((int*)_tmp28A)!= 3)goto _LL21C;_tmp296=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp28A)->f1;_tmp297=((struct Cyc_Absyn_Primop_e_struct*)
_tmp28A)->f2;_LL21B: return Cyc_Port_gen_primop(env,_tmp296,((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp,env,_tmp297));_LL21C: if(*((
int*)_tmp28A)!= 4)goto _LL21E;_tmp298=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp28A)->f1;_tmp299=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp28A)->f2;_tmp29A=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp28A)->f3;_LL21D: {void*_tmp2C1;void*
_tmp2C2;struct _tuple3 _tmp2C0=Cyc_Port_gen_lhs(env,_tmp298);_tmp2C1=_tmp2C0.f1;
_tmp2C2=_tmp2C0.f2;Cyc_Port_unifies(_tmp2C1,Cyc_Port_notconst_ct());{void*
_tmp2C3=Cyc_Port_gen_exp(env,_tmp29A);if((unsigned int)_tmp299){struct Cyc_List_List
x2=({struct Cyc_List_List _tmp2C6;_tmp2C6.hd=(void*)_tmp2C3;_tmp2C6.tl=0;_tmp2C6;});
struct Cyc_List_List x1=({struct Cyc_List_List _tmp2C5;_tmp2C5.hd=(void*)_tmp2C2;
_tmp2C5.tl=(struct Cyc_List_List*)& x2;_tmp2C5;});void*_tmp2C4=Cyc_Port_gen_primop(
env,(void*)_tmp299->v,(struct Cyc_List_List*)& x1);Cyc_Port_leq(_tmp2C4,_tmp2C2);
return _tmp2C2;}else{Cyc_Port_leq(_tmp2C3,_tmp2C2);return _tmp2C2;}}}_LL21E: if(*((
int*)_tmp28A)!= 5)goto _LL220;_tmp29B=((struct Cyc_Absyn_Increment_e_struct*)
_tmp28A)->f1;_tmp29C=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp28A)->f2;
_LL21F: {void*_tmp2C8;void*_tmp2C9;struct _tuple3 _tmp2C7=Cyc_Port_gen_lhs(env,
_tmp29B);_tmp2C8=_tmp2C7.f1;_tmp2C9=_tmp2C7.f2;Cyc_Port_unifies(_tmp2C8,Cyc_Port_notconst_ct());
Cyc_Port_leq(_tmp2C9,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()));Cyc_Port_leq(_tmp2C9,Cyc_Port_arith_ct());return
_tmp2C9;}_LL220: if(*((int*)_tmp28A)!= 6)goto _LL222;_tmp29D=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp28A)->f1;_tmp29E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp28A)->f2;
_tmp29F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp28A)->f3;_LL221: {void*
_tmp2CA=Cyc_Port_var();Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp29D),Cyc_Port_arith_ct());
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp29E),_tmp2CA);Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp29F),_tmp2CA);return _tmp2CA;}_LL222: if(*((int*)_tmp28A)!= 7)goto _LL224;
_tmp2A0=((struct Cyc_Absyn_And_e_struct*)_tmp28A)->f1;_tmp2A1=((struct Cyc_Absyn_And_e_struct*)
_tmp28A)->f2;_LL223: _tmp2A2=_tmp2A0;_tmp2A3=_tmp2A1;goto _LL225;_LL224: if(*((int*)
_tmp28A)!= 8)goto _LL226;_tmp2A2=((struct Cyc_Absyn_Or_e_struct*)_tmp28A)->f1;
_tmp2A3=((struct Cyc_Absyn_Or_e_struct*)_tmp28A)->f2;_LL225: Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp2A2),Cyc_Port_arith_ct());Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2A3),Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();_LL226: if(*((int*)_tmp28A)!= 9)goto _LL228;_tmp2A4=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp28A)->f1;_tmp2A5=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp28A)->f2;_LL227: Cyc_Port_gen_exp(env,_tmp2A4);return Cyc_Port_gen_exp(env,
_tmp2A5);_LL228: if(*((int*)_tmp28A)!= 10)goto _LL22A;_tmp2A6=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp28A)->f1;_tmp2A7=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp28A)->f2;_LL229:
_tmp2A8=_tmp2A6;_tmp2A9=_tmp2A7;goto _LL22B;_LL22A: if(*((int*)_tmp28A)!= 11)goto
_LL22C;_tmp2A8=((struct Cyc_Absyn_FnCall_e_struct*)_tmp28A)->f1;_tmp2A9=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp28A)->f2;_LL22B: {void*_tmp2CB=Cyc_Port_var();
void*_tmp2CC=Cyc_Port_gen_exp(env,_tmp2A8);struct Cyc_List_List*_tmp2CD=((struct
Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp,env,_tmp2A9);struct
Cyc_List_List*_tmp2CE=Cyc_List_map(Cyc_Port_new_var,_tmp2CD);Cyc_Port_unifies(
_tmp2CC,Cyc_Port_ptr_ct(Cyc_Port_fn_ct(_tmp2CB,_tmp2CE),Cyc_Port_var(),Cyc_Port_var(),
Cyc_Port_var(),Cyc_Port_nozterm_ct()));for(0;_tmp2CD != 0;(_tmp2CD=_tmp2CD->tl,
_tmp2CE=_tmp2CE->tl)){Cyc_Port_leq((void*)_tmp2CD->hd,(void*)((struct Cyc_List_List*)
_check_null(_tmp2CE))->hd);}return _tmp2CB;}_LL22C: if(*((int*)_tmp28A)!= 12)goto
_LL22E;_LL22D:({void*_tmp2CF[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp2D0="throw";_tag_arr(_tmp2D0,
sizeof(char),_get_zero_arr_size(_tmp2D0,6));}),_tag_arr(_tmp2CF,sizeof(void*),0));});
_LL22E: if(*((int*)_tmp28A)!= 13)goto _LL230;_tmp2AA=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp28A)->f1;_LL22F: return Cyc_Port_gen_exp(env,_tmp2AA);_LL230: if(*((int*)
_tmp28A)!= 14)goto _LL232;_LL231:({void*_tmp2D1[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp2D2="instantiate";
_tag_arr(_tmp2D2,sizeof(char),_get_zero_arr_size(_tmp2D2,12));}),_tag_arr(
_tmp2D1,sizeof(void*),0));});_LL232: if(*((int*)_tmp28A)!= 15)goto _LL234;_tmp2AB=(
void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp28A)->f1;_tmp2AC=((struct Cyc_Absyn_Cast_e_struct*)
_tmp28A)->f2;_LL233: {void*_tmp2D3=Cyc_Port_type_to_ctype(env,_tmp2AB);void*
_tmp2D4=Cyc_Port_gen_exp(env,_tmp2AC);Cyc_Port_leq(_tmp2D3,_tmp2D4);return
_tmp2D4;}_LL234: if(*((int*)_tmp28A)!= 16)goto _LL236;_tmp2AD=((struct Cyc_Absyn_Address_e_struct*)
_tmp28A)->f1;_LL235: {void*_tmp2D6;void*_tmp2D7;struct _tuple3 _tmp2D5=Cyc_Port_gen_lhs(
env,_tmp2AD);_tmp2D6=_tmp2D5.f1;_tmp2D7=_tmp2D5.f2;return Cyc_Port_ptr_ct(_tmp2D7,
_tmp2D6,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var());}_LL236: if(*((int*)_tmp28A)
!= 25)goto _LL238;_tmp2AE=((struct Cyc_Absyn_Subscript_e_struct*)_tmp28A)->f1;
_tmp2AF=((struct Cyc_Absyn_Subscript_e_struct*)_tmp28A)->f2;_LL237: {void*_tmp2D8=
Cyc_Port_var();Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2AF),Cyc_Port_arith_ct());{
void*_tmp2D9=Cyc_Port_gen_exp(env,_tmp2AE);Cyc_Port_leq(_tmp2D9,Cyc_Port_ptr_ct(
_tmp2D8,Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()));return
_tmp2D8;}}_LL238: if(*((int*)_tmp28A)!= 22)goto _LL23A;_tmp2B0=((struct Cyc_Absyn_Deref_e_struct*)
_tmp28A)->f1;_LL239: {void*_tmp2DA=Cyc_Port_var();Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp2B0),Cyc_Port_ptr_ct(_tmp2DA,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),
Cyc_Port_var()));return _tmp2DA;}_LL23A: if(*((int*)_tmp28A)!= 23)goto _LL23C;
_tmp2B1=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp28A)->f1;_tmp2B2=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp28A)->f2;_LL23B: {void*_tmp2DB=Cyc_Port_var();void*_tmp2DC=Cyc_Port_gen_exp(
env,_tmp2B1);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2B1),Cyc_Port_unknown_aggr_ct(({
struct Cyc_Port_Cfield*_tmp2DD[1];_tmp2DD[0]=({struct Cyc_Port_Cfield*_tmp2DE=
_cycalloc(sizeof(*_tmp2DE));_tmp2DE->qual=(void*)Cyc_Port_var();_tmp2DE->f=
_tmp2B2;_tmp2DE->type=(void*)_tmp2DB;_tmp2DE;});((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp2DD,sizeof(struct Cyc_Port_Cfield*),1));})));
return _tmp2DB;}_LL23C: if(*((int*)_tmp28A)!= 24)goto _LL23E;_tmp2B3=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp28A)->f1;_tmp2B4=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp28A)->f2;_LL23D: {
void*_tmp2DF=Cyc_Port_var();void*_tmp2E0=Cyc_Port_gen_exp(env,_tmp2B3);Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp2B3),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(({struct
Cyc_Port_Cfield*_tmp2E1[1];_tmp2E1[0]=({struct Cyc_Port_Cfield*_tmp2E2=_cycalloc(
sizeof(*_tmp2E2));_tmp2E2->qual=(void*)Cyc_Port_var();_tmp2E2->f=_tmp2B4;_tmp2E2->type=(
void*)_tmp2DF;_tmp2E2;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp2E1,sizeof(struct Cyc_Port_Cfield*),1));})),Cyc_Port_var(),Cyc_Port_var(),
Cyc_Port_var(),Cyc_Port_var()));return _tmp2DF;}_LL23E: if(*((int*)_tmp28A)!= 35)
goto _LL240;_tmp2B5=((struct Cyc_Absyn_Malloc_e_struct*)_tmp28A)->f1;_tmp2B6=
_tmp2B5.elt_type;_tmp2B7=_tmp2B5.num_elts;_LL23F: Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp2B7),Cyc_Port_arith_ct());{void*_tmp2E3=(unsigned int)_tmp2B6?Cyc_Port_type_to_ctype(
env,*_tmp2B6): Cyc_Port_var();return Cyc_Port_ptr_ct(_tmp2E3,Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_heap_ct(),Cyc_Port_var());}_LL240: if(*((int*)_tmp28A)!= 21)goto _LL242;
_LL241:({void*_tmp2E4[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp2E5="Gen";_tag_arr(_tmp2E5,sizeof(char),
_get_zero_arr_size(_tmp2E5,4));}),_tag_arr(_tmp2E4,sizeof(void*),0));});_LL242:
if(*((int*)_tmp28A)!= 17)goto _LL244;_LL243:({void*_tmp2E6[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp2E7="New_e";
_tag_arr(_tmp2E7,sizeof(char),_get_zero_arr_size(_tmp2E7,6));}),_tag_arr(_tmp2E6,
sizeof(void*),0));});_LL244: if(*((int*)_tmp28A)!= 26)goto _LL246;_LL245:({void*
_tmp2E8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp2E9="Tuple_e";_tag_arr(_tmp2E9,sizeof(char),_get_zero_arr_size(
_tmp2E9,8));}),_tag_arr(_tmp2E8,sizeof(void*),0));});_LL246: if(*((int*)_tmp28A)
!= 27)goto _LL248;_LL247:({void*_tmp2EA[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp2EB="CompoundLit_e";
_tag_arr(_tmp2EB,sizeof(char),_get_zero_arr_size(_tmp2EB,14));}),_tag_arr(
_tmp2EA,sizeof(void*),0));});_LL248: if(*((int*)_tmp28A)!= 28)goto _LL24A;_LL249:({
void*_tmp2EC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp2ED="Array_e";_tag_arr(_tmp2ED,sizeof(char),_get_zero_arr_size(
_tmp2ED,8));}),_tag_arr(_tmp2EC,sizeof(void*),0));});_LL24A: if(*((int*)_tmp28A)
!= 29)goto _LL24C;_LL24B:({void*_tmp2EE[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp2EF="Comprehension_e";
_tag_arr(_tmp2EF,sizeof(char),_get_zero_arr_size(_tmp2EF,16));}),_tag_arr(
_tmp2EE,sizeof(void*),0));});_LL24C: if(*((int*)_tmp28A)!= 30)goto _LL24E;_LL24D:({
void*_tmp2F0[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp2F1="Struct_e";_tag_arr(_tmp2F1,sizeof(char),_get_zero_arr_size(
_tmp2F1,9));}),_tag_arr(_tmp2F0,sizeof(void*),0));});_LL24E: if(*((int*)_tmp28A)
!= 31)goto _LL250;_LL24F:({void*_tmp2F2[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp2F3="AnonStruct_e";
_tag_arr(_tmp2F3,sizeof(char),_get_zero_arr_size(_tmp2F3,13));}),_tag_arr(
_tmp2F2,sizeof(void*),0));});_LL250: if(*((int*)_tmp28A)!= 32)goto _LL252;_LL251:({
void*_tmp2F4[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp2F5="Tunion_e";_tag_arr(_tmp2F5,sizeof(char),_get_zero_arr_size(
_tmp2F5,9));}),_tag_arr(_tmp2F4,sizeof(void*),0));});_LL252: if(*((int*)_tmp28A)
!= 36)goto _LL254;_LL253:({void*_tmp2F6[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp2F7="UnresolvedMem_e";
_tag_arr(_tmp2F7,sizeof(char),_get_zero_arr_size(_tmp2F7,16));}),_tag_arr(
_tmp2F6,sizeof(void*),0));});_LL254: if(*((int*)_tmp28A)!= 37)goto _LL1FB;_tmp2B8=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp28A)->f1;_LL255:({void*_tmp2F8[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp2F9="StmtExp_e";
_tag_arr(_tmp2F9,sizeof(char),_get_zero_arr_size(_tmp2F9,10));}),_tag_arr(
_tmp2F8,sizeof(void*),0));});_LL1FB:;}static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*
env,struct Cyc_Absyn_Stmt*s){void*_tmp2FA=(void*)s->r;struct Cyc_Absyn_Exp*_tmp2FB;
struct Cyc_Absyn_Stmt*_tmp2FC;struct Cyc_Absyn_Stmt*_tmp2FD;struct Cyc_Absyn_Exp*
_tmp2FE;struct Cyc_Absyn_Exp*_tmp2FF;struct Cyc_Absyn_Stmt*_tmp300;struct Cyc_Absyn_Stmt*
_tmp301;struct _tuple2 _tmp302;struct Cyc_Absyn_Exp*_tmp303;struct Cyc_Absyn_Stmt*
_tmp304;struct Cyc_Absyn_Exp*_tmp305;struct _tuple2 _tmp306;struct Cyc_Absyn_Exp*
_tmp307;struct _tuple2 _tmp308;struct Cyc_Absyn_Exp*_tmp309;struct Cyc_Absyn_Stmt*
_tmp30A;struct Cyc_Absyn_Exp*_tmp30B;struct Cyc_List_List*_tmp30C;struct Cyc_Absyn_Decl*
_tmp30D;struct Cyc_Absyn_Stmt*_tmp30E;struct Cyc_Absyn_Stmt*_tmp30F;struct Cyc_Absyn_Stmt*
_tmp310;struct _tuple2 _tmp311;struct Cyc_Absyn_Exp*_tmp312;_LL257: if((int)_tmp2FA
!= 0)goto _LL259;_LL258: return;_LL259: if(_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 
0)goto _LL25B;_tmp2FB=((struct Cyc_Absyn_Exp_s_struct*)_tmp2FA)->f1;_LL25A: Cyc_Port_gen_exp(
env,_tmp2FB);return;_LL25B: if(_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 1)goto
_LL25D;_tmp2FC=((struct Cyc_Absyn_Seq_s_struct*)_tmp2FA)->f1;_tmp2FD=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2FA)->f2;_LL25C: Cyc_Port_gen_stmt(env,_tmp2FC);Cyc_Port_gen_stmt(env,_tmp2FD);
return;_LL25D: if(_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 2)goto _LL25F;_tmp2FE=((
struct Cyc_Absyn_Return_s_struct*)_tmp2FA)->f1;_LL25E: {void*_tmp313=Cyc_Port_lookup_return_type(
env);void*_tmp314=(unsigned int)_tmp2FE?Cyc_Port_gen_exp(env,(struct Cyc_Absyn_Exp*)
_tmp2FE): Cyc_Port_void_ct();Cyc_Port_leq(_tmp314,_tmp313);return;}_LL25F: if(
_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 3)goto _LL261;_tmp2FF=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2FA)->f1;_tmp300=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2FA)->f2;_tmp301=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2FA)->f3;_LL260: Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp2FF),Cyc_Port_arith_ct());Cyc_Port_gen_stmt(env,_tmp300);Cyc_Port_gen_stmt(
env,_tmp301);return;_LL261: if(_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 4)goto
_LL263;_tmp302=((struct Cyc_Absyn_While_s_struct*)_tmp2FA)->f1;_tmp303=_tmp302.f1;
_tmp304=((struct Cyc_Absyn_While_s_struct*)_tmp2FA)->f2;_LL262: Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp303),Cyc_Port_arith_ct());Cyc_Port_gen_stmt(env,_tmp304);return;_LL263:
if(_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 5)goto _LL265;_LL264: goto _LL266;
_LL265: if(_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 6)goto _LL267;_LL266: goto
_LL268;_LL267: if(_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 7)goto _LL269;_LL268:
return;_LL269: if(_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 8)goto _LL26B;_tmp305=((
struct Cyc_Absyn_For_s_struct*)_tmp2FA)->f1;_tmp306=((struct Cyc_Absyn_For_s_struct*)
_tmp2FA)->f2;_tmp307=_tmp306.f1;_tmp308=((struct Cyc_Absyn_For_s_struct*)_tmp2FA)->f3;
_tmp309=_tmp308.f1;_tmp30A=((struct Cyc_Absyn_For_s_struct*)_tmp2FA)->f4;_LL26A:
Cyc_Port_gen_exp(env,_tmp305);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp307),Cyc_Port_arith_ct());
Cyc_Port_gen_exp(env,_tmp309);Cyc_Port_gen_stmt(env,_tmp30A);return;_LL26B: if(
_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 9)goto _LL26D;_tmp30B=((struct Cyc_Absyn_Switch_s_struct*)
_tmp2FA)->f1;_tmp30C=((struct Cyc_Absyn_Switch_s_struct*)_tmp2FA)->f2;_LL26C: Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp30B),Cyc_Port_arith_ct());for(0;(unsigned int)_tmp30C;
_tmp30C=_tmp30C->tl){Cyc_Port_gen_stmt(env,((struct Cyc_Absyn_Switch_clause*)
_tmp30C->hd)->body);}return;_LL26D: if(_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 
10)goto _LL26F;_LL26E:({void*_tmp315[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp316="fallthru";_tag_arr(_tmp316,
sizeof(char),_get_zero_arr_size(_tmp316,9));}),_tag_arr(_tmp315,sizeof(void*),0));});
_LL26F: if(_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 11)goto _LL271;_tmp30D=((
struct Cyc_Absyn_Decl_s_struct*)_tmp2FA)->f1;_tmp30E=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2FA)->f2;_LL270: env=Cyc_Port_gen_localdecl(env,_tmp30D);Cyc_Port_gen_stmt(
env,_tmp30E);return;_LL271: if(_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 12)goto
_LL273;_tmp30F=((struct Cyc_Absyn_Label_s_struct*)_tmp2FA)->f2;_LL272: Cyc_Port_gen_stmt(
env,_tmp30F);return;_LL273: if(_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 13)goto
_LL275;_tmp310=((struct Cyc_Absyn_Do_s_struct*)_tmp2FA)->f1;_tmp311=((struct Cyc_Absyn_Do_s_struct*)
_tmp2FA)->f2;_tmp312=_tmp311.f1;_LL274: Cyc_Port_gen_stmt(env,_tmp310);Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp312),Cyc_Port_arith_ct());return;_LL275: if(_tmp2FA <= (
void*)1  || *((int*)_tmp2FA)!= 14)goto _LL277;_LL276:({void*_tmp317[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp318="try/catch";
_tag_arr(_tmp318,sizeof(char),_get_zero_arr_size(_tmp318,10));}),_tag_arr(
_tmp317,sizeof(void*),0));});_LL277: if(_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 
15)goto _LL279;_LL278:({void*_tmp319[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp31A="region";_tag_arr(_tmp31A,
sizeof(char),_get_zero_arr_size(_tmp31A,7));}),_tag_arr(_tmp319,sizeof(void*),0));});
_LL279: if(_tmp2FA <= (void*)1  || *((int*)_tmp2FA)!= 16)goto _LL256;_LL27A:({void*
_tmp31B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp31C="reset region";_tag_arr(_tmp31C,sizeof(char),
_get_zero_arr_size(_tmp31C,13));}),_tag_arr(_tmp31B,sizeof(void*),0));});_LL256:;}
struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Port_gen_initializer(
struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e){void*_tmp31D=(void*)e->r;
struct Cyc_List_List*_tmp31E;struct Cyc_List_List*_tmp31F;struct Cyc_List_List*
_tmp320;void*_tmp321;struct _tagged_arr _tmp322;_LL27C: if(*((int*)_tmp31D)!= 36)
goto _LL27E;_tmp31E=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp31D)->f2;_LL27D:
_tmp31F=_tmp31E;goto _LL27F;_LL27E: if(*((int*)_tmp31D)!= 28)goto _LL280;_tmp31F=((
struct Cyc_Absyn_Array_e_struct*)_tmp31D)->f1;_LL27F: _tmp320=_tmp31F;goto _LL281;
_LL280: if(*((int*)_tmp31D)!= 27)goto _LL282;_tmp320=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp31D)->f2;_LL281: LOOP: {void*_tmp323=t;struct _tuple0*_tmp324;struct Cyc_Absyn_ArrayInfo
_tmp325;void*_tmp326;struct Cyc_Absyn_Conref*_tmp327;struct Cyc_Position_Segment*
_tmp328;struct Cyc_Absyn_AggrInfo _tmp329;void*_tmp32A;void*_tmp32B;struct _tuple0*
_tmp32C;struct Cyc_Absyn_AggrInfo _tmp32D;void*_tmp32E;struct Cyc_Absyn_Aggrdecl**
_tmp32F;struct Cyc_Absyn_Aggrdecl*_tmp330;_LL287: if(_tmp323 <= (void*)4  || *((int*)
_tmp323)!= 16)goto _LL289;_tmp324=((struct Cyc_Absyn_TypedefType_struct*)_tmp323)->f1;
_LL288:(*_tmp324).f1=(void*)0;t=Cyc_Port_lookup_typedef(env,_tmp324);goto LOOP;
_LL289: if(_tmp323 <= (void*)4  || *((int*)_tmp323)!= 7)goto _LL28B;_tmp325=((struct
Cyc_Absyn_ArrayType_struct*)_tmp323)->f1;_tmp326=(void*)_tmp325.elt_type;_tmp327=
_tmp325.zero_term;_tmp328=_tmp325.zt_loc;_LL28A: {void*_tmp331=Cyc_Port_var();
void*_tmp332=Cyc_Port_var();void*_tmp333=Cyc_Port_var();void*_tmp334=Cyc_Port_type_to_ctype(
env,_tmp326);for(0;_tmp320 != 0;_tmp320=_tmp320->tl){struct Cyc_List_List*_tmp336;
struct Cyc_Absyn_Exp*_tmp337;struct _tuple8 _tmp335=*((struct _tuple8*)_tmp320->hd);
_tmp336=_tmp335.f1;_tmp337=_tmp335.f2;if((unsigned int)_tmp336)({void*_tmp338[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp339="designators in initializers";_tag_arr(_tmp339,sizeof(char),
_get_zero_arr_size(_tmp339,28));}),_tag_arr(_tmp338,sizeof(void*),0));});{void*
_tmp33A=Cyc_Port_gen_initializer(env,_tmp326,_tmp337);Cyc_Port_leq(_tmp33A,
_tmp331);}}return Cyc_Port_array_ct(_tmp331,_tmp332,_tmp333);}_LL28B: if(_tmp323 <= (
void*)4  || *((int*)_tmp323)!= 10)goto _LL28D;_tmp329=((struct Cyc_Absyn_AggrType_struct*)
_tmp323)->f1;_tmp32A=(void*)_tmp329.aggr_info;if(*((int*)_tmp32A)!= 0)goto _LL28D;
_tmp32B=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp32A)->f1;if((int)
_tmp32B != 0)goto _LL28D;_tmp32C=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp32A)->f2;
_LL28C:(*_tmp32C).f1=(void*)0;{struct Cyc_Absyn_Aggrdecl*_tmp33C;struct _tuple4
_tmp33B=*Cyc_Port_lookup_struct_decl(env,_tmp32C);_tmp33C=_tmp33B.f1;if((struct
Cyc_Absyn_Aggrdecl*)_tmp33C == 0)({void*_tmp33D[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp33E="struct is not yet defined";
_tag_arr(_tmp33E,sizeof(char),_get_zero_arr_size(_tmp33E,26));}),_tag_arr(
_tmp33D,sizeof(void*),0));});_tmp330=_tmp33C;goto _LL28E;}_LL28D: if(_tmp323 <= (
void*)4  || *((int*)_tmp323)!= 10)goto _LL28F;_tmp32D=((struct Cyc_Absyn_AggrType_struct*)
_tmp323)->f1;_tmp32E=(void*)_tmp32D.aggr_info;if(*((int*)_tmp32E)!= 1)goto _LL28F;
_tmp32F=((struct Cyc_Absyn_KnownAggr_struct*)_tmp32E)->f1;_tmp330=*_tmp32F;_LL28E: {
struct Cyc_List_List*_tmp33F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp330->impl))->fields;
for(0;_tmp320 != 0;_tmp320=_tmp320->tl){struct Cyc_List_List*_tmp341;struct Cyc_Absyn_Exp*
_tmp342;struct _tuple8 _tmp340=*((struct _tuple8*)_tmp320->hd);_tmp341=_tmp340.f1;
_tmp342=_tmp340.f2;if((unsigned int)_tmp341)({void*_tmp343[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp344="designators in initializers";
_tag_arr(_tmp344,sizeof(char),_get_zero_arr_size(_tmp344,28));}),_tag_arr(
_tmp343,sizeof(void*),0));});{struct Cyc_Absyn_Aggrfield*_tmp345=(struct Cyc_Absyn_Aggrfield*)((
struct Cyc_List_List*)_check_null(_tmp33F))->hd;_tmp33F=_tmp33F->tl;{void*_tmp346=
Cyc_Port_gen_initializer(env,(void*)_tmp345->type,_tmp342);;}}}return Cyc_Port_type_to_ctype(
env,t);}_LL28F:;_LL290:({void*_tmp347[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp348="bad type for aggregate initializer";
_tag_arr(_tmp348,sizeof(char),_get_zero_arr_size(_tmp348,35));}),_tag_arr(
_tmp347,sizeof(void*),0));});_LL286:;}_LL282: if(*((int*)_tmp31D)!= 0)goto _LL284;
_tmp321=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp31D)->f1;if(_tmp321 <= (void*)
1  || *((int*)_tmp321)!= 5)goto _LL284;_tmp322=((struct Cyc_Absyn_String_c_struct*)
_tmp321)->f1;_LL283: LOOP2: {void*_tmp349=t;struct _tuple0*_tmp34A;_LL292: if(
_tmp349 <= (void*)4  || *((int*)_tmp349)!= 16)goto _LL294;_tmp34A=((struct Cyc_Absyn_TypedefType_struct*)
_tmp349)->f1;_LL293:(*_tmp34A).f1=(void*)0;t=Cyc_Port_lookup_typedef(env,_tmp34A);
goto LOOP2;_LL294: if(_tmp349 <= (void*)4  || *((int*)_tmp349)!= 7)goto _LL296;_LL295:
return Cyc_Port_array_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_zterm_ct());
_LL296:;_LL297: return Cyc_Port_gen_exp(env,e);_LL291:;}_LL284:;_LL285: return Cyc_Port_gen_exp(
env,e);_LL27B:;}static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*
env,struct Cyc_Absyn_Decl*d){void*_tmp34B=(void*)d->r;struct Cyc_Absyn_Vardecl*
_tmp34C;_LL299: if(_tmp34B <= (void*)2  || *((int*)_tmp34B)!= 0)goto _LL29B;_tmp34C=((
struct Cyc_Absyn_Var_d_struct*)_tmp34B)->f1;_LL29A: {void*_tmp34D=Cyc_Port_var();
void*q;if((env->gcenv)->porting){q=Cyc_Port_var();Cyc_Port_register_const_cvar(
env,q,(_tmp34C->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(
_tmp34C->tq).loc);}else{q=(_tmp34C->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}(*
_tmp34C->name).f1=(void*)0;env=Cyc_Port_add_var(env,_tmp34C->name,(void*)_tmp34C->type,
q,_tmp34D);Cyc_Port_unifies(_tmp34D,Cyc_Port_type_to_ctype(env,(void*)_tmp34C->type));
if((unsigned int)_tmp34C->initializer){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
_check_null(_tmp34C->initializer);void*t2=Cyc_Port_gen_initializer(env,(void*)
_tmp34C->type,e);Cyc_Port_leq(t2,_tmp34D);}return env;}_LL29B:;_LL29C:({void*
_tmp34E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp34F="non-local decl that isn't a variable";_tag_arr(_tmp34F,
sizeof(char),_get_zero_arr_size(_tmp34F,37));}),_tag_arr(_tmp34E,sizeof(void*),0));});
_LL298:;}struct _tuple9{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static struct _tuple1*Cyc_Port_make_targ(struct _tuple9*arg){struct _tagged_arr*
_tmp351;struct Cyc_Absyn_Tqual _tmp352;void*_tmp353;struct _tuple9 _tmp350=*arg;
_tmp351=_tmp350.f1;_tmp352=_tmp350.f2;_tmp353=_tmp350.f3;return({struct _tuple1*
_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354->f1=0;_tmp354->f2=_tmp352;_tmp354->f3=
_tmp353;_tmp354;});}static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*
env,struct Cyc_List_List*ds);struct _tuple10{struct _tagged_arr*f1;void*f2;void*f3;
void*f4;};static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*env,
struct Cyc_Absyn_Decl*d){void*_tmp355=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp356;
struct Cyc_Absyn_Fndecl*_tmp357;struct Cyc_Absyn_Typedefdecl*_tmp358;struct Cyc_Absyn_Aggrdecl*
_tmp359;struct Cyc_Absyn_Enumdecl*_tmp35A;_LL29E: if((int)_tmp355 != 0)goto _LL2A0;
_LL29F:(env->gcenv)->porting=1;return env;_LL2A0: if((int)_tmp355 != 1)goto _LL2A2;
_LL2A1:(env->gcenv)->porting=1;return env;_LL2A2: if(_tmp355 <= (void*)2  || *((int*)
_tmp355)!= 0)goto _LL2A4;_tmp356=((struct Cyc_Absyn_Var_d_struct*)_tmp355)->f1;
_LL2A3:(*_tmp356->name).f1=(void*)0;if(Cyc_Port_declared_var(env,_tmp356->name)){
void*_tmp35C;void*_tmp35D;struct _tuple3 _tmp35B=Cyc_Port_lookup_var(env,_tmp356->name);
_tmp35C=_tmp35B.f1;_tmp35D=_tmp35B.f2;{void*q2;if((env->gcenv)->porting  && !Cyc_Port_isfn(
env,_tmp356->name)){q2=Cyc_Port_var();Cyc_Port_register_const_cvar(env,q2,(
_tmp356->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(_tmp356->tq).loc);}
else{q2=(_tmp356->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
Cyc_Port_unifies(_tmp35C,q2);Cyc_Port_unifies(_tmp35D,Cyc_Port_type_to_ctype(env,(
void*)_tmp356->type));if((unsigned int)_tmp356->initializer){struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_check_null(_tmp356->initializer);Cyc_Port_leq(Cyc_Port_gen_initializer(
env,(void*)_tmp356->type,e),_tmp35D);}return env;}}else{return Cyc_Port_gen_localdecl(
env,d);}_LL2A4: if(_tmp355 <= (void*)2  || *((int*)_tmp355)!= 1)goto _LL2A6;_tmp357=((
struct Cyc_Absyn_Fn_d_struct*)_tmp355)->f1;_LL2A5:(*_tmp357->name).f1=(void*)0;{
struct _tuple6*predeclared=0;if(Cyc_Port_declared_var(env,_tmp357->name))
predeclared=(struct _tuple6*)Cyc_Port_lookup_full_var(env,_tmp357->name);{void*
_tmp35E=(void*)_tmp357->ret_type;struct Cyc_List_List*_tmp35F=_tmp357->args;
struct Cyc_List_List*_tmp360=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct
_tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_make_targ,_tmp35F);
struct Cyc_Absyn_FnType_struct*_tmp361=({struct Cyc_Absyn_FnType_struct*_tmp37B=
_cycalloc(sizeof(*_tmp37B));_tmp37B[0]=({struct Cyc_Absyn_FnType_struct _tmp37C;
_tmp37C.tag=8;_tmp37C.f1=({struct Cyc_Absyn_FnInfo _tmp37D;_tmp37D.tvars=0;_tmp37D.effect=
0;_tmp37D.ret_typ=(void*)_tmp35E;_tmp37D.args=_tmp360;_tmp37D.c_varargs=0;
_tmp37D.cyc_varargs=0;_tmp37D.rgn_po=0;_tmp37D.attributes=0;_tmp37D;});_tmp37C;});
_tmp37B;});struct Cyc_Port_Cenv*_tmp362=Cyc_Port_set_cpos(env,(void*)2);void*
_tmp363=Cyc_Port_type_to_ctype(_tmp362,_tmp35E);struct Cyc_List_List*c_args=0;
struct Cyc_List_List*c_arg_types=0;{struct Cyc_List_List*_tmp364=_tmp35F;for(0;(
unsigned int)_tmp364;_tmp364=_tmp364->tl){struct _tagged_arr*_tmp366;struct Cyc_Absyn_Tqual
_tmp367;void*_tmp368;struct _tuple9 _tmp365=*((struct _tuple9*)_tmp364->hd);_tmp366=
_tmp365.f1;_tmp367=_tmp365.f2;_tmp368=_tmp365.f3;{void*_tmp369=Cyc_Port_type_to_ctype(
_tmp362,_tmp368);void*tqv;if((env->gcenv)->porting){tqv=Cyc_Port_var();Cyc_Port_register_const_cvar(
env,tqv,_tmp367.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp367.loc);}
else{tqv=_tmp367.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}c_args=({
struct Cyc_List_List*_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A->hd=({struct
_tuple10*_tmp36B=_cycalloc(sizeof(*_tmp36B));_tmp36B->f1=_tmp366;_tmp36B->f2=
_tmp368;_tmp36B->f3=tqv;_tmp36B->f4=_tmp369;_tmp36B;});_tmp36A->tl=c_args;
_tmp36A;});c_arg_types=({struct Cyc_List_List*_tmp36C=_cycalloc(sizeof(*_tmp36C));
_tmp36C->hd=(void*)_tmp369;_tmp36C->tl=c_arg_types;_tmp36C;});}}}c_args=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_args);c_arg_types=
Cyc_List_imp_rev(c_arg_types);{void*_tmp36D=Cyc_Port_fn_ct(_tmp363,c_arg_types);(*
_tmp357->name).f1=(void*)0;_tmp362=Cyc_Port_add_var(_tmp362,_tmp357->name,(void*)
_tmp361,Cyc_Port_const_ct(),_tmp36D);Cyc_Port_add_return_type(_tmp362,_tmp363);{
struct Cyc_List_List*_tmp36E=c_args;for(0;(unsigned int)_tmp36E;_tmp36E=_tmp36E->tl){
struct _tagged_arr*_tmp370;void*_tmp371;void*_tmp372;void*_tmp373;struct _tuple10
_tmp36F=*((struct _tuple10*)_tmp36E->hd);_tmp370=_tmp36F.f1;_tmp371=_tmp36F.f2;
_tmp372=_tmp36F.f3;_tmp373=_tmp36F.f4;_tmp362=Cyc_Port_add_var(_tmp362,({struct
_tuple0*_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374->f1=(void*)0;_tmp374->f2=
_tmp370;_tmp374;}),_tmp371,_tmp372,_tmp373);}}Cyc_Port_gen_stmt(_tmp362,_tmp357->body);
Cyc_Port_generalize(0,_tmp36D);{struct Cyc_Dict_Dict*counts=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);
Cyc_Port_region_counts(& counts,_tmp36D);Cyc_Port_register_rgns(env,counts,1,(
void*)_tmp361,_tmp36D);env=Cyc_Port_add_var(_tmp362,_tmp357->name,(void*)_tmp361,
Cyc_Port_const_ct(),_tmp36D);if((unsigned int)predeclared){void*_tmp376;struct
_tuple3*_tmp377;struct _tuple3 _tmp378;void*_tmp379;void*_tmp37A;struct _tuple6
_tmp375=*predeclared;_tmp376=_tmp375.f1;_tmp377=_tmp375.f2;_tmp378=*_tmp377;
_tmp379=_tmp378.f1;_tmp37A=_tmp378.f2;Cyc_Port_unifies(_tmp379,Cyc_Port_const_ct());
Cyc_Port_unifies(_tmp37A,Cyc_Port_instantiate(_tmp36D));Cyc_Port_register_rgns(
env,counts,1,_tmp376,_tmp36D);}return env;}}}}_LL2A6: if(_tmp355 <= (void*)2  || *((
int*)_tmp355)!= 7)goto _LL2A8;_tmp358=((struct Cyc_Absyn_Typedef_d_struct*)_tmp355)->f1;
_LL2A7: {void*_tmp37E=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp358->defn))->v;
void*_tmp37F=Cyc_Port_type_to_ctype(env,_tmp37E);(*_tmp358->name).f1=(void*)0;
Cyc_Port_add_typedef(env,_tmp358->name,_tmp37E,_tmp37F);return env;}_LL2A8: if(
_tmp355 <= (void*)2  || *((int*)_tmp355)!= 4)goto _LL2AA;_tmp359=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp355)->f1;_LL2A9: {struct _tuple0*_tmp380=_tmp359->name;(*_tmp359->name).f1=(
void*)0;{struct _tuple4*previous;struct Cyc_List_List*curr=0;{void*_tmp381=(void*)
_tmp359->kind;_LL2AF: if((int)_tmp381 != 0)goto _LL2B1;_LL2B0: previous=Cyc_Port_lookup_struct_decl(
env,_tmp380);goto _LL2AE;_LL2B1: if((int)_tmp381 != 1)goto _LL2AE;_LL2B2: previous=
Cyc_Port_lookup_union_decl(env,_tmp380);goto _LL2AE;_LL2AE:;}if((unsigned int)
_tmp359->impl){struct Cyc_List_List*cf=(*previous).f2;{struct Cyc_List_List*
_tmp382=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp359->impl))->fields;for(
0;(unsigned int)_tmp382;_tmp382=_tmp382->tl){struct Cyc_Absyn_Aggrfield*_tmp383=(
struct Cyc_Absyn_Aggrfield*)_tmp382->hd;void*qv;if((env->gcenv)->porting){qv=Cyc_Port_var();
Cyc_Port_register_const_cvar(env,qv,(_tmp383->tq).print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct(),(_tmp383->tq).loc);}else{qv=(_tmp383->tq).print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct();}{void*_tmp384=Cyc_Port_type_to_ctype(env,(void*)_tmp383->type);
if(cf != 0){void*_tmp386;void*_tmp387;struct Cyc_Port_Cfield _tmp385=*((struct Cyc_Port_Cfield*)
cf->hd);_tmp386=(void*)_tmp385.qual;_tmp387=(void*)_tmp385.type;cf=cf->tl;Cyc_Port_unifies(
qv,_tmp386);Cyc_Port_unifies(_tmp384,_tmp387);}curr=({struct Cyc_List_List*
_tmp388=_cycalloc(sizeof(*_tmp388));_tmp388->hd=({struct Cyc_Port_Cfield*_tmp389=
_cycalloc(sizeof(*_tmp389));_tmp389->qual=(void*)qv;_tmp389->f=_tmp383->name;
_tmp389->type=(void*)_tmp384;_tmp389;});_tmp388->tl=curr;_tmp388;});}}}curr=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(curr);if((*
previous).f2 == 0)(*previous).f2=curr;}return env;}}_LL2AA: if(_tmp355 <= (void*)2
 || *((int*)_tmp355)!= 6)goto _LL2AC;_tmp35A=((struct Cyc_Absyn_Enum_d_struct*)
_tmp355)->f1;_LL2AB:(*_tmp35A->name).f1=(void*)0;if((unsigned int)_tmp35A->fields){
struct Cyc_List_List*_tmp38A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp35A->fields))->v;for(0;(unsigned int)_tmp38A;_tmp38A=_tmp38A->tl){(*((
struct Cyc_Absyn_Enumfield*)_tmp38A->hd)->name).f1=(void*)0;env=Cyc_Port_add_var(
env,((struct Cyc_Absyn_Enumfield*)_tmp38A->hd)->name,(void*)({struct Cyc_Absyn_EnumType_struct*
_tmp38B=_cycalloc(sizeof(*_tmp38B));_tmp38B[0]=({struct Cyc_Absyn_EnumType_struct
_tmp38C;_tmp38C.tag=12;_tmp38C.f1=_tmp35A->name;_tmp38C.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp35A;_tmp38C;});_tmp38B;}),Cyc_Port_const_ct(),Cyc_Port_arith_ct());}}return
env;_LL2AC:;_LL2AD: if((env->gcenv)->porting)({void*_tmp38D[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp38E="Warning: Cyclone declaration found in code to be ported -- skipping.";
_tag_arr(_tmp38E,sizeof(char),_get_zero_arr_size(_tmp38E,69));}),_tag_arr(
_tmp38D,sizeof(void*),0));});return env;_LL29D:;}static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(
struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds){for(0;(unsigned int)ds;ds=ds->tl){
env=Cyc_Port_gen_topdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);}return env;}static
struct Cyc_List_List*Cyc_Port_gen_edits(struct Cyc_List_List*ds){struct Cyc_Port_Cenv*
env=Cyc_Port_gen_topdecls(Cyc_Port_initial_cenv(),ds);struct Cyc_List_List*
_tmp38F=(env->gcenv)->pointer_edits;struct Cyc_List_List*_tmp390=(env->gcenv)->qualifier_edits;
struct Cyc_List_List*_tmp391=(env->gcenv)->zeroterm_edits;struct Cyc_List_List*
_tmp392=(env->gcenv)->edits;{struct Cyc_List_List*_tmp393=_tmp38F;for(0;(
unsigned int)_tmp393;_tmp393=_tmp393->tl){void*_tmp395;void*_tmp396;struct Cyc_Position_Segment*
_tmp397;struct _tuple7 _tmp394=*((struct _tuple7*)_tmp393->hd);_tmp395=_tmp394.f1;
_tmp396=_tmp394.f2;_tmp397=_tmp394.f3;Cyc_Port_unifies(_tmp395,_tmp396);}}{
struct Cyc_List_List*_tmp398=_tmp390;for(0;(unsigned int)_tmp398;_tmp398=_tmp398->tl){
void*_tmp39A;void*_tmp39B;struct Cyc_Position_Segment*_tmp39C;struct _tuple7
_tmp399=*((struct _tuple7*)_tmp398->hd);_tmp39A=_tmp399.f1;_tmp39B=_tmp399.f2;
_tmp39C=_tmp399.f3;Cyc_Port_unifies(_tmp39A,_tmp39B);}}{struct Cyc_List_List*
_tmp39D=_tmp391;for(0;(unsigned int)_tmp39D;_tmp39D=_tmp39D->tl){void*_tmp39F;
void*_tmp3A0;struct Cyc_Position_Segment*_tmp3A1;struct _tuple7 _tmp39E=*((struct
_tuple7*)_tmp39D->hd);_tmp39F=_tmp39E.f1;_tmp3A0=_tmp39E.f2;_tmp3A1=_tmp39E.f3;
Cyc_Port_unifies(_tmp39F,_tmp3A0);}}for(0;(unsigned int)_tmp38F;_tmp38F=_tmp38F->tl){
void*_tmp3A3;void*_tmp3A4;struct Cyc_Position_Segment*_tmp3A5;struct _tuple7
_tmp3A2=*((struct _tuple7*)_tmp38F->hd);_tmp3A3=_tmp3A2.f1;_tmp3A4=_tmp3A2.f2;
_tmp3A5=_tmp3A2.f3;if(!Cyc_Port_unifies(_tmp3A3,_tmp3A4) && (unsigned int)
_tmp3A5){void*_tmp3A6=Cyc_Port_compress_ct(_tmp3A4);_LL2B4: if((int)_tmp3A6 != 2)
goto _LL2B6;_LL2B5: _tmp392=({struct Cyc_List_List*_tmp3A7=_cycalloc(sizeof(*
_tmp3A7));_tmp3A7->hd=({struct Cyc_Port_Edit*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));
_tmp3A8->loc=_tmp3A5;_tmp3A8->old_string=({const char*_tmp3AA="?";_tag_arr(
_tmp3AA,sizeof(char),_get_zero_arr_size(_tmp3AA,2));});_tmp3A8->new_string=({
const char*_tmp3A9="*";_tag_arr(_tmp3A9,sizeof(char),_get_zero_arr_size(_tmp3A9,2));});
_tmp3A8;});_tmp3A7->tl=_tmp392;_tmp3A7;});goto _LL2B3;_LL2B6: if((int)_tmp3A6 != 3)
goto _LL2B8;_LL2B7: _tmp392=({struct Cyc_List_List*_tmp3AB=_cycalloc(sizeof(*
_tmp3AB));_tmp3AB->hd=({struct Cyc_Port_Edit*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));
_tmp3AC->loc=_tmp3A5;_tmp3AC->old_string=({const char*_tmp3AE="*";_tag_arr(
_tmp3AE,sizeof(char),_get_zero_arr_size(_tmp3AE,2));});_tmp3AC->new_string=({
const char*_tmp3AD="?";_tag_arr(_tmp3AD,sizeof(char),_get_zero_arr_size(_tmp3AD,2));});
_tmp3AC;});_tmp3AB->tl=_tmp392;_tmp3AB;});goto _LL2B3;_LL2B8:;_LL2B9: goto _LL2B3;
_LL2B3:;}}for(0;(unsigned int)_tmp390;_tmp390=_tmp390->tl){void*_tmp3B0;void*
_tmp3B1;struct Cyc_Position_Segment*_tmp3B2;struct _tuple7 _tmp3AF=*((struct _tuple7*)
_tmp390->hd);_tmp3B0=_tmp3AF.f1;_tmp3B1=_tmp3AF.f2;_tmp3B2=_tmp3AF.f3;if(!Cyc_Port_unifies(
_tmp3B0,_tmp3B1) && (unsigned int)_tmp3B2){void*_tmp3B3=Cyc_Port_compress_ct(
_tmp3B1);_LL2BB: if((int)_tmp3B3 != 1)goto _LL2BD;_LL2BC: _tmp392=({struct Cyc_List_List*
_tmp3B4=_cycalloc(sizeof(*_tmp3B4));_tmp3B4->hd=({struct Cyc_Port_Edit*_tmp3B5=
_cycalloc(sizeof(*_tmp3B5));_tmp3B5->loc=_tmp3B2;_tmp3B5->old_string=({const char*
_tmp3B7="const ";_tag_arr(_tmp3B7,sizeof(char),_get_zero_arr_size(_tmp3B7,7));});
_tmp3B5->new_string=({const char*_tmp3B6="";_tag_arr(_tmp3B6,sizeof(char),
_get_zero_arr_size(_tmp3B6,1));});_tmp3B5;});_tmp3B4->tl=_tmp392;_tmp3B4;});goto
_LL2BA;_LL2BD: if((int)_tmp3B3 != 0)goto _LL2BF;_LL2BE: _tmp392=({struct Cyc_List_List*
_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8->hd=({struct Cyc_Port_Edit*_tmp3B9=
_cycalloc(sizeof(*_tmp3B9));_tmp3B9->loc=_tmp3B2;_tmp3B9->old_string=({const char*
_tmp3BB="";_tag_arr(_tmp3BB,sizeof(char),_get_zero_arr_size(_tmp3BB,1));});
_tmp3B9->new_string=({const char*_tmp3BA="const ";_tag_arr(_tmp3BA,sizeof(char),
_get_zero_arr_size(_tmp3BA,7));});_tmp3B9;});_tmp3B8->tl=_tmp392;_tmp3B8;});goto
_LL2BA;_LL2BF:;_LL2C0: goto _LL2BA;_LL2BA:;}}for(0;(unsigned int)_tmp391;_tmp391=
_tmp391->tl){void*_tmp3BD;void*_tmp3BE;struct Cyc_Position_Segment*_tmp3BF;struct
_tuple7 _tmp3BC=*((struct _tuple7*)_tmp391->hd);_tmp3BD=_tmp3BC.f1;_tmp3BE=_tmp3BC.f2;
_tmp3BF=_tmp3BC.f3;if(!Cyc_Port_unifies(_tmp3BD,_tmp3BE) && (unsigned int)
_tmp3BF){void*_tmp3C0=Cyc_Port_compress_ct(_tmp3BE);_LL2C2: if((int)_tmp3C0 != 8)
goto _LL2C4;_LL2C3: _tmp392=({struct Cyc_List_List*_tmp3C1=_cycalloc(sizeof(*
_tmp3C1));_tmp3C1->hd=({struct Cyc_Port_Edit*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));
_tmp3C2->loc=_tmp3BF;_tmp3C2->old_string=({const char*_tmp3C4="NOZEROTERM ";
_tag_arr(_tmp3C4,sizeof(char),_get_zero_arr_size(_tmp3C4,12));});_tmp3C2->new_string=({
const char*_tmp3C3="";_tag_arr(_tmp3C3,sizeof(char),_get_zero_arr_size(_tmp3C3,1));});
_tmp3C2;});_tmp3C1->tl=_tmp392;_tmp3C1;});goto _LL2C1;_LL2C4: if((int)_tmp3C0 != 9)
goto _LL2C6;_LL2C5: _tmp392=({struct Cyc_List_List*_tmp3C5=_cycalloc(sizeof(*
_tmp3C5));_tmp3C5->hd=({struct Cyc_Port_Edit*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));
_tmp3C6->loc=_tmp3BF;_tmp3C6->old_string=({const char*_tmp3C8="ZEROTERM ";
_tag_arr(_tmp3C8,sizeof(char),_get_zero_arr_size(_tmp3C8,10));});_tmp3C6->new_string=({
const char*_tmp3C7="";_tag_arr(_tmp3C7,sizeof(char),_get_zero_arr_size(_tmp3C7,1));});
_tmp3C6;});_tmp3C5->tl=_tmp392;_tmp3C5;});goto _LL2C1;_LL2C6:;_LL2C7: goto _LL2C1;
_LL2C1:;}}_tmp392=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_Port_Edit*,
struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_merge_sort)(Cyc_Port_cmp_edit,
_tmp392);while((unsigned int)_tmp392  && ((struct Cyc_Position_Segment*)
_check_null(((struct Cyc_Port_Edit*)_tmp392->hd)->loc))->start == 0){_tmp392=
_tmp392->tl;}return _tmp392;}static struct Cyc_Position_Segment*Cyc_Port_get_seg(
struct Cyc_Port_Edit*e){return e->loc;}void Cyc_Port_port(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp3C9=Cyc_Port_gen_edits(ds);struct Cyc_List_List*_tmp3CA=((
struct Cyc_List_List*(*)(struct Cyc_Position_Segment*(*f)(struct Cyc_Port_Edit*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_get_seg,_tmp3C9);Cyc_Position_use_gcc_style_location=
0;{struct Cyc_List_List*_tmp3CB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_Position_strings_of_segments(_tmp3CA));for(0;(unsigned int)
_tmp3C9;(_tmp3C9=_tmp3C9->tl,_tmp3CB=_tmp3CB->tl)){struct Cyc_Position_Segment*
_tmp3CD;struct _tagged_arr _tmp3CE;struct _tagged_arr _tmp3CF;struct Cyc_Port_Edit
_tmp3CC=*((struct Cyc_Port_Edit*)_tmp3C9->hd);_tmp3CD=_tmp3CC.loc;_tmp3CE=_tmp3CC.old_string;
_tmp3CF=_tmp3CC.new_string;{struct _tagged_arr sloc=(struct _tagged_arr)*((struct
_tagged_arr*)((struct Cyc_List_List*)_check_null(_tmp3CB))->hd);({struct Cyc_String_pa_struct
_tmp3D4;_tmp3D4.tag=0;_tmp3D4.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp3CF);{
struct Cyc_String_pa_struct _tmp3D3;_tmp3D3.tag=0;_tmp3D3.f1=(struct _tagged_arr)((
struct _tagged_arr)_tmp3CE);{struct Cyc_String_pa_struct _tmp3D2;_tmp3D2.tag=0;
_tmp3D2.f1=(struct _tagged_arr)((struct _tagged_arr)sloc);{void*_tmp3D0[3]={&
_tmp3D2,& _tmp3D3,& _tmp3D4};Cyc_printf(({const char*_tmp3D1="%s: insert `%s' for `%s'\n";
_tag_arr(_tmp3D1,sizeof(char),_get_zero_arr_size(_tmp3D1,26));}),_tag_arr(
_tmp3D0,sizeof(void*),3));}}}});}}}}
