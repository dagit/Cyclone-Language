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
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];extern char
Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc___cycFILE;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};int Cyc_strptrcmp(struct _tagged_arr*s1,struct
_tagged_arr*s2);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple0{void*
f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;};struct Cyc_Absyn_Conref{
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
struct Cyc_List_List*f2;};struct Cyc_Absyn_Exp_p_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*cnst_exp;struct Cyc_Absyn_Stmt*
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
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[
15];struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct Cyc_Iter_Iter{
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr
fmt,struct _tagged_arr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);void*
Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(
void*,void*);struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_okay_szofarg(void*t);struct _tuple5{void*f1;int f2;};static struct
_tuple5 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*);static void*Cyc_Evexp_promote_const(
void*cn){void*_tmp0=cn;void*_tmp1;char _tmp2;void*_tmp3;short _tmp4;_LL1: if(_tmp0
<= (void*)1  || *((int*)_tmp0)!= 0)goto _LL3;_tmp1=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp0)->f1;_tmp2=((struct Cyc_Absyn_Char_c_struct*)_tmp0)->f2;_LL2: return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=({struct
Cyc_Absyn_Int_c_struct _tmp6;_tmp6.tag=2;_tmp6.f1=(void*)_tmp1;_tmp6.f2=(int)
_tmp2;_tmp6;});_tmp5;});_LL3: if(_tmp0 <= (void*)1  || *((int*)_tmp0)!= 1)goto _LL5;
_tmp3=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp0)->f1;_tmp4=((struct Cyc_Absyn_Short_c_struct*)
_tmp0)->f2;_LL4: return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp7=_cycalloc(
sizeof(*_tmp7));_tmp7[0]=({struct Cyc_Absyn_Int_c_struct _tmp8;_tmp8.tag=2;_tmp8.f1=(
void*)_tmp3;_tmp8.f2=(int)_tmp4;_tmp8;});_tmp7;});_LL5:;_LL6: return cn;_LL0:;}
struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){void*_tmpA;int
_tmpB;struct _tuple5 _tmp9=Cyc_Evexp_eval_const_exp(e);_tmpA=_tmp9.f1;_tmpB=_tmp9.f2;
if(!_tmpB)return({struct _tuple4 _tmpC;_tmpC.f1=0;_tmpC.f2=0;_tmpC;});{void*_tmpD=
Cyc_Evexp_promote_const(_tmpA);int _tmpE;_LL8: if(_tmpD <= (void*)1  || *((int*)
_tmpD)!= 2)goto _LLA;_tmpE=((struct Cyc_Absyn_Int_c_struct*)_tmpD)->f2;_LL9: return({
struct _tuple4 _tmpF;_tmpF.f1=(unsigned int)_tmpE;_tmpF.f2=1;_tmpF;});_LLA:;_LLB:({
void*_tmp10[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp11="expecting unsigned int";
_tag_arr(_tmp11,sizeof(char),_get_zero_arr_size(_tmp11,23));}),_tag_arr(_tmp10,
sizeof(void*),0));});return({struct _tuple4 _tmp12;_tmp12.f1=0;_tmp12.f2=1;_tmp12;});
_LL7:;}}struct _tuple6{int f1;int f2;};static struct _tuple6 Cyc_Evexp_eval_const_bool_exp(
struct Cyc_Absyn_Exp*e){void*_tmp14;int _tmp15;struct _tuple5 _tmp13=Cyc_Evexp_eval_const_exp(
e);_tmp14=_tmp13.f1;_tmp15=_tmp13.f2;if(!_tmp15)return({struct _tuple6 _tmp16;
_tmp16.f1=0;_tmp16.f2=0;_tmp16;});{void*_tmp17=Cyc_Evexp_promote_const(_tmp14);
int _tmp18;long long _tmp19;_LLD: if(_tmp17 <= (void*)1  || *((int*)_tmp17)!= 2)goto
_LLF;_tmp18=((struct Cyc_Absyn_Int_c_struct*)_tmp17)->f2;_LLE: return({struct
_tuple6 _tmp1A;_tmp1A.f1=_tmp18 != 0;_tmp1A.f2=1;_tmp1A;});_LLF: if(_tmp17 <= (void*)
1  || *((int*)_tmp17)!= 3)goto _LL11;_tmp19=((struct Cyc_Absyn_LongLong_c_struct*)
_tmp17)->f2;_LL10: return({struct _tuple6 _tmp1B;_tmp1B.f1=_tmp19 != 0;_tmp1B.f2=1;
_tmp1B;});_LL11: if((int)_tmp17 != 0)goto _LL13;_LL12: return({struct _tuple6 _tmp1C;
_tmp1C.f1=0;_tmp1C.f2=0;_tmp1C;});_LL13:;_LL14:({void*_tmp1D[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp1E="expecting bool";_tag_arr(_tmp1E,sizeof(char),
_get_zero_arr_size(_tmp1E,15));}),_tag_arr(_tmp1D,sizeof(void*),0));});return({
struct _tuple6 _tmp1F;_tmp1F.f1=0;_tmp1F.f2=0;_tmp1F;});_LLC:;}}static struct
_tuple5 Cyc_Evexp_eval_const_unprimop(void*p,struct Cyc_Absyn_Exp*e){void*_tmp21;
int _tmp22;struct _tuple5 _tmp20=Cyc_Evexp_eval_const_exp(e);_tmp21=_tmp20.f1;
_tmp22=_tmp20.f2;if(!_tmp22)return({struct _tuple5 _tmp23;_tmp23.f1=_tmp21;_tmp23.f2=
0;_tmp23;});{struct _tuple3 _tmp25=({struct _tuple3 _tmp24;_tmp24.f1=p;_tmp24.f2=
_tmp21;_tmp24;});void*_tmp26;void*_tmp27;void*_tmp28;void*_tmp29;int _tmp2A;void*
_tmp2B;void*_tmp2C;void*_tmp2D;int _tmp2E;void*_tmp2F;void*_tmp30;int _tmp31;void*
_tmp32;void*_tmp33;_LL16: _tmp26=_tmp25.f1;if((int)_tmp26 != 0)goto _LL18;_LL17:
goto _LL15;_LL18: _tmp27=_tmp25.f1;if((int)_tmp27 != 2)goto _LL1A;_tmp28=_tmp25.f2;
if(_tmp28 <= (void*)1  || *((int*)_tmp28)!= 2)goto _LL1A;_tmp29=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp28)->f1;_tmp2A=((struct Cyc_Absyn_Int_c_struct*)_tmp28)->f2;_LL19: _tmp21=(
void*)({struct Cyc_Absyn_Int_c_struct*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34[0]=({
struct Cyc_Absyn_Int_c_struct _tmp35;_tmp35.tag=2;_tmp35.f1=(void*)((void*)0);
_tmp35.f2=- _tmp2A;_tmp35;});_tmp34;});goto _LL15;_LL1A: _tmp2B=_tmp25.f1;if((int)
_tmp2B != 12)goto _LL1C;_tmp2C=_tmp25.f2;if(_tmp2C <= (void*)1  || *((int*)_tmp2C)!= 
2)goto _LL1C;_tmp2D=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp2C)->f1;_tmp2E=((
struct Cyc_Absyn_Int_c_struct*)_tmp2C)->f2;_LL1B: _tmp21=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=({struct Cyc_Absyn_Int_c_struct _tmp37;
_tmp37.tag=2;_tmp37.f1=(void*)((void*)1);_tmp37.f2=~ _tmp2E;_tmp37;});_tmp36;});
goto _LL15;_LL1C: _tmp2F=_tmp25.f1;if((int)_tmp2F != 11)goto _LL1E;_tmp30=_tmp25.f2;
if(_tmp30 <= (void*)1  || *((int*)_tmp30)!= 2)goto _LL1E;_tmp31=((struct Cyc_Absyn_Int_c_struct*)
_tmp30)->f2;_LL1D: _tmp21=(void*)({struct Cyc_Absyn_Int_c_struct*_tmp38=_cycalloc(
sizeof(*_tmp38));_tmp38[0]=({struct Cyc_Absyn_Int_c_struct _tmp39;_tmp39.tag=2;
_tmp39.f1=(void*)((void*)0);_tmp39.f2=_tmp31 == 0?1: 0;_tmp39;});_tmp38;});goto
_LL15;_LL1E: _tmp32=_tmp25.f1;if((int)_tmp32 != 11)goto _LL20;_tmp33=_tmp25.f2;if((
int)_tmp33 != 0)goto _LL20;_LL1F: _tmp21=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A[0]=({struct Cyc_Absyn_Int_c_struct _tmp3B;
_tmp3B.tag=2;_tmp3B.f1=(void*)((void*)0);_tmp3B.f2=1;_tmp3B;});_tmp3A;});goto
_LL15;_LL20:;_LL21:({void*_tmp3C[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp3D="bad constant expression";
_tag_arr(_tmp3D,sizeof(char),_get_zero_arr_size(_tmp3D,24));}),_tag_arr(_tmp3C,
sizeof(void*),0));});goto _LL15;_LL15:;}return({struct _tuple5 _tmp3E;_tmp3E.f1=
_tmp21;_tmp3E.f2=1;_tmp3E;});}static struct _tuple5 Cyc_Evexp_eval_const_binprimop(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp40;int _tmp41;
struct _tuple5 _tmp3F=Cyc_Evexp_eval_const_exp(e1);_tmp40=_tmp3F.f1;_tmp41=_tmp3F.f2;{
void*_tmp43;int _tmp44;struct _tuple5 _tmp42=Cyc_Evexp_eval_const_exp(e2);_tmp43=
_tmp42.f1;_tmp44=_tmp42.f2;if(!_tmp41  || !_tmp44)return({struct _tuple5 _tmp45;
_tmp45.f1=_tmp40;_tmp45.f2=0;_tmp45;});_tmp40=Cyc_Evexp_promote_const(_tmp40);
_tmp43=Cyc_Evexp_promote_const(_tmp43);{void*s1;void*s2;int i1;int i2;{void*_tmp46=
_tmp40;void*_tmp47;int _tmp48;_LL23: if(_tmp46 <= (void*)1  || *((int*)_tmp46)!= 2)
goto _LL25;_tmp47=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp46)->f1;_tmp48=((
struct Cyc_Absyn_Int_c_struct*)_tmp46)->f2;_LL24: s1=_tmp47;i1=_tmp48;goto _LL22;
_LL25:;_LL26:({void*_tmp49[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmp4A="bad constant expression";
_tag_arr(_tmp4A,sizeof(char),_get_zero_arr_size(_tmp4A,24));}),_tag_arr(_tmp49,
sizeof(void*),0));});return({struct _tuple5 _tmp4B;_tmp4B.f1=_tmp40;_tmp4B.f2=1;
_tmp4B;});_LL22:;}{void*_tmp4C=_tmp43;void*_tmp4D;int _tmp4E;_LL28: if(_tmp4C <= (
void*)1  || *((int*)_tmp4C)!= 2)goto _LL2A;_tmp4D=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp4C)->f1;_tmp4E=((struct Cyc_Absyn_Int_c_struct*)_tmp4C)->f2;_LL29: s2=_tmp4D;
i2=_tmp4E;goto _LL27;_LL2A:;_LL2B:({void*_tmp4F[0]={};Cyc_Tcutil_terr(e2->loc,({
const char*_tmp50="bad constant expression";_tag_arr(_tmp50,sizeof(char),
_get_zero_arr_size(_tmp50,24));}),_tag_arr(_tmp4F,sizeof(void*),0));});return({
struct _tuple5 _tmp51;_tmp51.f1=_tmp40;_tmp51.f2=1;_tmp51;});_LL27:;}{void*_tmp52=
p;_LL2D: if((int)_tmp52 != 3)goto _LL2F;_LL2E: goto _LL30;_LL2F: if((int)_tmp52 != 4)
goto _LL31;_LL30: if(i2 == 0){({void*_tmp53[0]={};Cyc_Tcutil_terr(e2->loc,({const
char*_tmp54="division by zero in constant expression";_tag_arr(_tmp54,sizeof(
char),_get_zero_arr_size(_tmp54,40));}),_tag_arr(_tmp53,sizeof(void*),0));});
return({struct _tuple5 _tmp55;_tmp55.f1=_tmp40;_tmp55.f2=1;_tmp55;});}goto _LL2C;
_LL31:;_LL32: goto _LL2C;_LL2C:;}{int has_u_arg=s1 == (void*)1  || s2 == (void*)1;
unsigned int u1=(unsigned int)i1;unsigned int u2=(unsigned int)i2;int i3=0;
unsigned int u3=0;int b3=1;int use_i3=0;int use_u3=0;int use_b3=0;{struct _tuple5
_tmp57=({struct _tuple5 _tmp56;_tmp56.f1=p;_tmp56.f2=has_u_arg;_tmp56;});void*
_tmp58;int _tmp59;void*_tmp5A;int _tmp5B;void*_tmp5C;int _tmp5D;void*_tmp5E;int
_tmp5F;void*_tmp60;int _tmp61;void*_tmp62;int _tmp63;void*_tmp64;int _tmp65;void*
_tmp66;int _tmp67;void*_tmp68;int _tmp69;void*_tmp6A;int _tmp6B;void*_tmp6C;void*
_tmp6D;void*_tmp6E;int _tmp6F;void*_tmp70;int _tmp71;void*_tmp72;int _tmp73;void*
_tmp74;int _tmp75;void*_tmp76;int _tmp77;void*_tmp78;int _tmp79;void*_tmp7A;int
_tmp7B;void*_tmp7C;int _tmp7D;void*_tmp7E;void*_tmp7F;void*_tmp80;void*_tmp81;
void*_tmp82;void*_tmp83;_LL34: _tmp58=_tmp57.f1;if((int)_tmp58 != 0)goto _LL36;
_tmp59=_tmp57.f2;if(_tmp59 != 0)goto _LL36;_LL35: i3=i1 + i2;use_i3=1;goto _LL33;
_LL36: _tmp5A=_tmp57.f1;if((int)_tmp5A != 1)goto _LL38;_tmp5B=_tmp57.f2;if(_tmp5B != 
0)goto _LL38;_LL37: i3=i1 * i2;use_i3=1;goto _LL33;_LL38: _tmp5C=_tmp57.f1;if((int)
_tmp5C != 2)goto _LL3A;_tmp5D=_tmp57.f2;if(_tmp5D != 0)goto _LL3A;_LL39: i3=i1 - i2;
use_i3=1;goto _LL33;_LL3A: _tmp5E=_tmp57.f1;if((int)_tmp5E != 3)goto _LL3C;_tmp5F=
_tmp57.f2;if(_tmp5F != 0)goto _LL3C;_LL3B: i3=i1 / i2;use_i3=1;goto _LL33;_LL3C:
_tmp60=_tmp57.f1;if((int)_tmp60 != 4)goto _LL3E;_tmp61=_tmp57.f2;if(_tmp61 != 0)
goto _LL3E;_LL3D: i3=i1 % i2;use_i3=1;goto _LL33;_LL3E: _tmp62=_tmp57.f1;if((int)
_tmp62 != 0)goto _LL40;_tmp63=_tmp57.f2;if(_tmp63 != 1)goto _LL40;_LL3F: u3=u1 + u2;
use_u3=1;goto _LL33;_LL40: _tmp64=_tmp57.f1;if((int)_tmp64 != 1)goto _LL42;_tmp65=
_tmp57.f2;if(_tmp65 != 1)goto _LL42;_LL41: u3=u1 * u2;use_u3=1;goto _LL33;_LL42:
_tmp66=_tmp57.f1;if((int)_tmp66 != 2)goto _LL44;_tmp67=_tmp57.f2;if(_tmp67 != 1)
goto _LL44;_LL43: u3=u1 - u2;use_u3=1;goto _LL33;_LL44: _tmp68=_tmp57.f1;if((int)
_tmp68 != 3)goto _LL46;_tmp69=_tmp57.f2;if(_tmp69 != 1)goto _LL46;_LL45: u3=u1 / u2;
use_u3=1;goto _LL33;_LL46: _tmp6A=_tmp57.f1;if((int)_tmp6A != 4)goto _LL48;_tmp6B=
_tmp57.f2;if(_tmp6B != 1)goto _LL48;_LL47: u3=u1 % u2;use_u3=1;goto _LL33;_LL48:
_tmp6C=_tmp57.f1;if((int)_tmp6C != 5)goto _LL4A;_LL49: b3=i1 == i2;use_b3=1;goto
_LL33;_LL4A: _tmp6D=_tmp57.f1;if((int)_tmp6D != 6)goto _LL4C;_LL4B: b3=i1 != i2;
use_b3=1;goto _LL33;_LL4C: _tmp6E=_tmp57.f1;if((int)_tmp6E != 7)goto _LL4E;_tmp6F=
_tmp57.f2;if(_tmp6F != 0)goto _LL4E;_LL4D: b3=i1 > i2;use_b3=1;goto _LL33;_LL4E:
_tmp70=_tmp57.f1;if((int)_tmp70 != 8)goto _LL50;_tmp71=_tmp57.f2;if(_tmp71 != 0)
goto _LL50;_LL4F: b3=i1 < i2;use_b3=1;goto _LL33;_LL50: _tmp72=_tmp57.f1;if((int)
_tmp72 != 9)goto _LL52;_tmp73=_tmp57.f2;if(_tmp73 != 0)goto _LL52;_LL51: b3=i1 >= i2;
use_b3=1;goto _LL33;_LL52: _tmp74=_tmp57.f1;if((int)_tmp74 != 10)goto _LL54;_tmp75=
_tmp57.f2;if(_tmp75 != 0)goto _LL54;_LL53: b3=i1 <= i2;use_b3=1;goto _LL33;_LL54:
_tmp76=_tmp57.f1;if((int)_tmp76 != 7)goto _LL56;_tmp77=_tmp57.f2;if(_tmp77 != 1)
goto _LL56;_LL55: b3=u1 > u2;use_b3=1;goto _LL33;_LL56: _tmp78=_tmp57.f1;if((int)
_tmp78 != 8)goto _LL58;_tmp79=_tmp57.f2;if(_tmp79 != 1)goto _LL58;_LL57: b3=u1 < u2;
use_b3=1;goto _LL33;_LL58: _tmp7A=_tmp57.f1;if((int)_tmp7A != 9)goto _LL5A;_tmp7B=
_tmp57.f2;if(_tmp7B != 1)goto _LL5A;_LL59: b3=u1 >= u2;use_b3=1;goto _LL33;_LL5A:
_tmp7C=_tmp57.f1;if((int)_tmp7C != 10)goto _LL5C;_tmp7D=_tmp57.f2;if(_tmp7D != 1)
goto _LL5C;_LL5B: b3=u1 <= u2;use_b3=1;goto _LL33;_LL5C: _tmp7E=_tmp57.f1;if((int)
_tmp7E != 13)goto _LL5E;_LL5D: u3=u1 & u2;use_u3=1;goto _LL33;_LL5E: _tmp7F=_tmp57.f1;
if((int)_tmp7F != 14)goto _LL60;_LL5F: u3=u1 | u2;use_u3=1;goto _LL33;_LL60: _tmp80=
_tmp57.f1;if((int)_tmp80 != 15)goto _LL62;_LL61: u3=u1 ^ u2;use_u3=1;goto _LL33;_LL62:
_tmp81=_tmp57.f1;if((int)_tmp81 != 16)goto _LL64;_LL63: u3=u1 << u2;use_u3=1;goto
_LL33;_LL64: _tmp82=_tmp57.f1;if((int)_tmp82 != 17)goto _LL66;_LL65: u3=u1 >> u2;
use_u3=1;goto _LL33;_LL66: _tmp83=_tmp57.f1;if((int)_tmp83 != 18)goto _LL68;_LL67:({
void*_tmp84[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmp85=">>> NOT IMPLEMENTED";
_tag_arr(_tmp85,sizeof(char),_get_zero_arr_size(_tmp85,20));}),_tag_arr(_tmp84,
sizeof(void*),0));});goto _LL69;_LL68:;_LL69:({void*_tmp86[0]={};Cyc_Tcutil_terr(
e1->loc,({const char*_tmp87="bad constant expression";_tag_arr(_tmp87,sizeof(char),
_get_zero_arr_size(_tmp87,24));}),_tag_arr(_tmp86,sizeof(void*),0));});return({
struct _tuple5 _tmp88;_tmp88.f1=_tmp40;_tmp88.f2=1;_tmp88;});_LL33:;}if(use_i3)
return({struct _tuple5 _tmp89;_tmp89.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A[0]=({struct Cyc_Absyn_Int_c_struct _tmp8B;
_tmp8B.tag=2;_tmp8B.f1=(void*)((void*)0);_tmp8B.f2=i3;_tmp8B;});_tmp8A;});_tmp89.f2=
1;_tmp89;});if(use_u3)return({struct _tuple5 _tmp8C;_tmp8C.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Absyn_Int_c_struct _tmp8E;
_tmp8E.tag=2;_tmp8E.f1=(void*)((void*)1);_tmp8E.f2=(int)u3;_tmp8E;});_tmp8D;});
_tmp8C.f2=1;_tmp8C;});if(use_b3)return({struct _tuple5 _tmp8F;_tmp8F.f1=(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=({
struct Cyc_Absyn_Int_c_struct _tmp91;_tmp91.tag=2;_tmp91.f1=(void*)((void*)0);
_tmp91.f2=b3?1: 0;_tmp91;});_tmp90;});_tmp8F.f2=1;_tmp8F;});({void*_tmp92[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp93="Evexp::eval_const_binop";_tag_arr(_tmp93,sizeof(char),_get_zero_arr_size(
_tmp93,24));}),_tag_arr(_tmp92,sizeof(void*),0));});}}}}static struct _tuple5 Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp*e){struct _tuple5 ans;{void*_tmp94=(void*)e->r;void*_tmp95;
struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Exp*_tmp98;
struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_Absyn_Exp*_tmp9B;
struct Cyc_Absyn_Exp*_tmp9C;void*_tmp9D;struct Cyc_List_List*_tmp9E;void*_tmp9F;
struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Enumfield*_tmpA1;struct Cyc_Absyn_Enumfield*
_tmpA2;_LL6B: if(*((int*)_tmp94)!= 0)goto _LL6D;_tmp95=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp94)->f1;_LL6C: return({struct _tuple5 _tmpA3;_tmpA3.f1=_tmp95;_tmpA3.f2=1;
_tmpA3;});_LL6D: if(*((int*)_tmp94)!= 6)goto _LL6F;_tmp96=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp94)->f1;_tmp97=((struct Cyc_Absyn_Conditional_e_struct*)_tmp94)->f2;_tmp98=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp94)->f3;_LL6E: {int _tmpA5;int _tmpA6;
struct _tuple6 _tmpA4=Cyc_Evexp_eval_const_bool_exp(_tmp96);_tmpA5=_tmpA4.f1;
_tmpA6=_tmpA4.f2;if(!_tmpA6){Cyc_Evexp_eval_const_exp(_tmp97);Cyc_Evexp_eval_const_exp(
_tmp98);return({struct _tuple5 _tmpA7;_tmpA7.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_Absyn_Int_c_struct _tmpA9;
_tmpA9.tag=2;_tmpA9.f1=(void*)((void*)0);_tmpA9.f2=0;_tmpA9;});_tmpA8;});_tmpA7.f2=
0;_tmpA7;});}ans=_tmpA5?Cyc_Evexp_eval_const_exp(_tmp97): Cyc_Evexp_eval_const_exp(
_tmp98);goto _LL6A;}_LL6F: if(*((int*)_tmp94)!= 7)goto _LL71;_tmp99=((struct Cyc_Absyn_And_e_struct*)
_tmp94)->f1;_tmp9A=((struct Cyc_Absyn_And_e_struct*)_tmp94)->f2;_LL70: {int _tmpAB;
int _tmpAC;struct _tuple6 _tmpAA=Cyc_Evexp_eval_const_bool_exp(_tmp99);_tmpAB=
_tmpAA.f1;_tmpAC=_tmpAA.f2;if(!_tmpAC){Cyc_Evexp_eval_const_exp(_tmp9A);return({
struct _tuple5 _tmpAD;_tmpAD.f1=(void*)({struct Cyc_Absyn_Int_c_struct*_tmpAE=
_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({struct Cyc_Absyn_Int_c_struct _tmpAF;_tmpAF.tag=
2;_tmpAF.f1=(void*)((void*)0);_tmpAF.f2=0;_tmpAF;});_tmpAE;});_tmpAD.f2=0;_tmpAD;});}
if(_tmpAB)ans=Cyc_Evexp_eval_const_exp(_tmp9A);else{ans=({struct _tuple5 _tmpB0;
_tmpB0.f1=(void*)({struct Cyc_Absyn_Int_c_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));
_tmpB1[0]=({struct Cyc_Absyn_Int_c_struct _tmpB2;_tmpB2.tag=2;_tmpB2.f1=(void*)((
void*)0);_tmpB2.f2=0;_tmpB2;});_tmpB1;});_tmpB0.f2=1;_tmpB0;});}goto _LL6A;}_LL71:
if(*((int*)_tmp94)!= 8)goto _LL73;_tmp9B=((struct Cyc_Absyn_Or_e_struct*)_tmp94)->f1;
_tmp9C=((struct Cyc_Absyn_Or_e_struct*)_tmp94)->f2;_LL72: {int _tmpB4;int _tmpB5;
struct _tuple6 _tmpB3=Cyc_Evexp_eval_const_bool_exp(_tmp9B);_tmpB4=_tmpB3.f1;
_tmpB5=_tmpB3.f2;if(!_tmpB5){Cyc_Evexp_eval_const_exp(_tmp9C);return({struct
_tuple5 _tmpB6;_tmpB6.f1=(void*)({struct Cyc_Absyn_Int_c_struct*_tmpB7=_cycalloc(
sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_Int_c_struct _tmpB8;_tmpB8.tag=2;
_tmpB8.f1=(void*)((void*)0);_tmpB8.f2=0;_tmpB8;});_tmpB7;});_tmpB6.f2=0;_tmpB6;});}
if(!_tmpB4)ans=Cyc_Evexp_eval_const_exp(_tmp9C);else{ans=({struct _tuple5 _tmpB9;
_tmpB9.f1=(void*)({struct Cyc_Absyn_Int_c_struct*_tmpBA=_cycalloc(sizeof(*_tmpBA));
_tmpBA[0]=({struct Cyc_Absyn_Int_c_struct _tmpBB;_tmpBB.tag=2;_tmpBB.f1=(void*)((
void*)0);_tmpBB.f2=1;_tmpBB;});_tmpBA;});_tmpB9.f2=1;_tmpB9;});}goto _LL6A;}_LL73:
if(*((int*)_tmp94)!= 3)goto _LL75;_tmp9D=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp94)->f1;_tmp9E=((struct Cyc_Absyn_Primop_e_struct*)_tmp94)->f2;_LL74: if(
_tmp9E == 0){({void*_tmpBC[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmpBD="bad static expression (no args to primop)";
_tag_arr(_tmpBD,sizeof(char),_get_zero_arr_size(_tmpBD,42));}),_tag_arr(_tmpBC,
sizeof(void*),0));});return({struct _tuple5 _tmpBE;_tmpBE.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_Int_c_struct _tmpC0;
_tmpC0.tag=2;_tmpC0.f1=(void*)((void*)0);_tmpC0.f2=0;_tmpC0;});_tmpBF;});_tmpBE.f2=
1;_tmpBE;});}if(_tmp9E->tl == 0){ans=Cyc_Evexp_eval_const_unprimop(_tmp9D,(struct
Cyc_Absyn_Exp*)_tmp9E->hd);goto _LL6A;}if(((struct Cyc_List_List*)_check_null(
_tmp9E->tl))->tl == 0){ans=Cyc_Evexp_eval_const_binprimop(_tmp9D,(struct Cyc_Absyn_Exp*)
_tmp9E->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9E->tl))->hd);
goto _LL6A;}({void*_tmpC1[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmpC2="bad static expression (too many args to primop)";
_tag_arr(_tmpC2,sizeof(char),_get_zero_arr_size(_tmpC2,48));}),_tag_arr(_tmpC1,
sizeof(void*),0));});return({struct _tuple5 _tmpC3;_tmpC3.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_Int_c_struct _tmpC5;
_tmpC5.tag=2;_tmpC5.f1=(void*)((void*)0);_tmpC5.f2=0;_tmpC5;});_tmpC4;});_tmpC3.f2=
1;_tmpC3;});_LL75: if(*((int*)_tmp94)!= 18)goto _LL77;_LL76: goto _LL78;_LL77: if(*((
int*)_tmp94)!= 19)goto _LL79;_LL78: goto _LL7A;_LL79: if(*((int*)_tmp94)!= 20)goto
_LL7B;_LL7A: ans=({struct _tuple5 _tmpC6;_tmpC6.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=({struct Cyc_Absyn_Int_c_struct _tmpC8;
_tmpC8.tag=2;_tmpC8.f1=(void*)((void*)0);_tmpC8.f2=0;_tmpC8;});_tmpC7;});_tmpC6.f2=
0;_tmpC6;});goto _LL6A;_LL7B: if(*((int*)_tmp94)!= 15)goto _LL7D;_tmp9F=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp94)->f1;_tmpA0=((struct Cyc_Absyn_Cast_e_struct*)
_tmp94)->f2;_LL7C: ans=Cyc_Evexp_eval_const_exp(_tmpA0);if(ans.f2){struct _tuple3
_tmpCA=({struct _tuple3 _tmpC9;_tmpC9.f1=Cyc_Tcutil_compress(_tmp9F);_tmpC9.f2=ans.f1;
_tmpC9;});void*_tmpCB;void*_tmpCC;void*_tmpCD;void*_tmpCE;void*_tmpCF;char _tmpD0;
void*_tmpD1;void*_tmpD2;void*_tmpD3;void*_tmpD4;void*_tmpD5;short _tmpD6;void*
_tmpD7;void*_tmpD8;void*_tmpD9;void*_tmpDA;void*_tmpDB;int _tmpDC;void*_tmpDD;
void*_tmpDE;void*_tmpDF;void*_tmpE0;char _tmpE1;void*_tmpE2;void*_tmpE3;void*
_tmpE4;void*_tmpE5;short _tmpE6;void*_tmpE7;void*_tmpE8;void*_tmpE9;void*_tmpEA;
int _tmpEB;_LL84: _tmpCB=_tmpCA.f1;if(_tmpCB <= (void*)3  || *((int*)_tmpCB)!= 5)
goto _LL86;_tmpCC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpCB)->f1;_tmpCD=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmpCB)->f2;_tmpCE=_tmpCA.f2;if(_tmpCE <= (
void*)1  || *((int*)_tmpCE)!= 0)goto _LL86;_tmpCF=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmpCE)->f1;_tmpD0=((struct Cyc_Absyn_Char_c_struct*)_tmpCE)->f2;_LL85: _tmpD2=
_tmpCC;_tmpD3=_tmpCD;_tmpD5=_tmpCF;_tmpD6=(short)_tmpD0;goto _LL87;_LL86: _tmpD1=
_tmpCA.f1;if(_tmpD1 <= (void*)3  || *((int*)_tmpD1)!= 5)goto _LL88;_tmpD2=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpD1)->f1;_tmpD3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpD1)->f2;_tmpD4=_tmpCA.f2;if(_tmpD4 <= (void*)1  || *((int*)_tmpD4)!= 1)goto
_LL88;_tmpD5=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmpD4)->f1;_tmpD6=((
struct Cyc_Absyn_Short_c_struct*)_tmpD4)->f2;_LL87: _tmpD8=_tmpD2;_tmpD9=_tmpD3;
_tmpDB=_tmpD5;_tmpDC=(int)_tmpD6;goto _LL89;_LL88: _tmpD7=_tmpCA.f1;if(_tmpD7 <= (
void*)3  || *((int*)_tmpD7)!= 5)goto _LL8A;_tmpD8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpD7)->f1;_tmpD9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpD7)->f2;_tmpDA=
_tmpCA.f2;if(_tmpDA <= (void*)1  || *((int*)_tmpDA)!= 2)goto _LL8A;_tmpDB=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmpDA)->f1;_tmpDC=((struct Cyc_Absyn_Int_c_struct*)
_tmpDA)->f2;_LL89: if(_tmpD8 != _tmpDB)ans.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=({struct Cyc_Absyn_Int_c_struct _tmpED;
_tmpED.tag=2;_tmpED.f1=(void*)_tmpD8;_tmpED.f2=_tmpDC;_tmpED;});_tmpEC;});goto
_LL83;_LL8A: _tmpDD=_tmpCA.f1;if(_tmpDD <= (void*)3  || *((int*)_tmpDD)!= 17)goto
_LL8C;_tmpDE=(void*)((struct Cyc_Absyn_TagType_struct*)_tmpDD)->f1;_tmpDF=_tmpCA.f2;
if(_tmpDF <= (void*)1  || *((int*)_tmpDF)!= 0)goto _LL8C;_tmpE0=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmpDF)->f1;_tmpE1=((struct Cyc_Absyn_Char_c_struct*)_tmpDF)->f2;_LL8B: _tmpE3=
_tmpDE;_tmpE5=_tmpE0;_tmpE6=(short)_tmpE1;goto _LL8D;_LL8C: _tmpE2=_tmpCA.f1;if(
_tmpE2 <= (void*)3  || *((int*)_tmpE2)!= 17)goto _LL8E;_tmpE3=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmpE2)->f1;_tmpE4=_tmpCA.f2;if(_tmpE4 <= (void*)1  || *((int*)_tmpE4)!= 1)goto
_LL8E;_tmpE5=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmpE4)->f1;_tmpE6=((
struct Cyc_Absyn_Short_c_struct*)_tmpE4)->f2;_LL8D: _tmpE8=_tmpE3;_tmpEA=_tmpE5;
_tmpEB=(int)_tmpE6;goto _LL8F;_LL8E: _tmpE7=_tmpCA.f1;if(_tmpE7 <= (void*)3  || *((
int*)_tmpE7)!= 17)goto _LL90;_tmpE8=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmpE7)->f1;_tmpE9=_tmpCA.f2;if(_tmpE9 <= (void*)1  || *((int*)_tmpE9)!= 2)goto
_LL90;_tmpEA=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmpE9)->f1;_tmpEB=((struct
Cyc_Absyn_Int_c_struct*)_tmpE9)->f2;_LL8F: if(_tmpEB < 0)({void*_tmpEE[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmpEF="cannot cast negative number to a tag type";_tag_arr(
_tmpEF,sizeof(char),_get_zero_arr_size(_tmpEF,42));}),_tag_arr(_tmpEE,sizeof(
void*),0));});Cyc_Tcutil_unify(_tmpE8,(void*)({struct Cyc_Absyn_TypeInt_struct*
_tmpF0=_cycalloc_atomic(sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_Absyn_TypeInt_struct
_tmpF1;_tmpF1.tag=18;_tmpF1.f1=_tmpEB;_tmpF1;});_tmpF0;}));ans.f1=(void*)({
struct Cyc_Absyn_Int_c_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2[0]=({
struct Cyc_Absyn_Int_c_struct _tmpF3;_tmpF3.tag=2;_tmpF3.f1=(void*)((void*)1);
_tmpF3.f2=_tmpEB;_tmpF3;});_tmpF2;});goto _LL83;_LL90:;_LL91:({struct Cyc_String_pa_struct
_tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp9F));{void*_tmpF4[1]={& _tmpF6};Cyc_Tcutil_terr(e->loc,({const char*_tmpF5="cannot cast to %s";
_tag_arr(_tmpF5,sizeof(char),_get_zero_arr_size(_tmpF5,18));}),_tag_arr(_tmpF4,
sizeof(void*),1));}});goto _LL83;_LL83:;}goto _LL6A;_LL7D: if(*((int*)_tmp94)!= 34)
goto _LL7F;_tmpA1=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp94)->f3;_LL7E: _tmpA2=
_tmpA1;goto _LL80;_LL7F: if(*((int*)_tmp94)!= 33)goto _LL81;_tmpA2=((struct Cyc_Absyn_Enum_e_struct*)
_tmp94)->f3;_LL80: ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(((
struct Cyc_Absyn_Enumfield*)_check_null(_tmpA2))->tag));goto _LL6A;_LL81:;_LL82:({
struct Cyc_String_pa_struct _tmpF9;_tmpF9.tag=0;_tmpF9.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_exp2string(e));{void*_tmpF7[1]={& _tmpF9};Cyc_Tcutil_terr(
e->loc,({const char*_tmpF8="bad static expression %s";_tag_arr(_tmpF8,sizeof(char),
_get_zero_arr_size(_tmpF8,25));}),_tag_arr(_tmpF7,sizeof(void*),1));}});return({
struct _tuple5 _tmpFA;_tmpFA.f1=(void*)({struct Cyc_Absyn_Int_c_struct*_tmpFB=
_cycalloc(sizeof(*_tmpFB));_tmpFB[0]=({struct Cyc_Absyn_Int_c_struct _tmpFC;_tmpFC.tag=
2;_tmpFC.f1=(void*)((void*)0);_tmpFC.f2=0;_tmpFC;});_tmpFB;});_tmpFA.f2=1;_tmpFA;});
_LL6A:;}if(ans.f2)(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({struct Cyc_Absyn_Const_e_struct
_tmpFE;_tmpFE.tag=0;_tmpFE.f1=(void*)ans.f1;_tmpFE;});_tmpFD;})));return ans;}
static int Cyc_Evexp_const_exp_case_number(struct Cyc_Absyn_Exp*e){void*_tmpFF=(
void*)e->r;_LL93: if(*((int*)_tmpFF)!= 0)goto _LL95;_LL94: return 1;_LL95: if(*((int*)
_tmpFF)!= 6)goto _LL97;_LL96: return 2;_LL97: if(*((int*)_tmpFF)!= 3)goto _LL99;_LL98:
return 3;_LL99: if(*((int*)_tmpFF)!= 18)goto _LL9B;_LL9A: goto _LL9C;_LL9B: if(*((int*)
_tmpFF)!= 19)goto _LL9D;_LL9C: return 4;_LL9D: if(*((int*)_tmpFF)!= 20)goto _LL9F;
_LL9E: return 5;_LL9F: if(*((int*)_tmpFF)!= 15)goto _LLA1;_LLA0: return 6;_LLA1: if(*((
int*)_tmpFF)!= 7)goto _LLA3;_LLA2: return 7;_LLA3: if(*((int*)_tmpFF)!= 8)goto _LLA5;
_LLA4: return 8;_LLA5:;_LLA6:({struct Cyc_String_pa_struct _tmp102;_tmp102.tag=0;
_tmp102.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e));{
void*_tmp100[1]={& _tmp102};Cyc_Tcutil_terr(e->loc,({const char*_tmp101="bad static expression %s";
_tag_arr(_tmp101,sizeof(char),_get_zero_arr_size(_tmp101,25));}),_tag_arr(
_tmp100,sizeof(void*),1));}});return 0;_LL92:;}int Cyc_Evexp_const_exp_cmp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){unsigned int _tmp104;int _tmp105;struct
_tuple4 _tmp103=Cyc_Evexp_eval_const_uint_exp(e1);_tmp104=_tmp103.f1;_tmp105=
_tmp103.f2;{unsigned int _tmp107;int _tmp108;struct _tuple4 _tmp106=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp107=_tmp106.f1;_tmp108=_tmp106.f2;if(_tmp105  && _tmp108)return(int)(
_tmp104 - _tmp107);{int _tmp109=Cyc_Evexp_const_exp_case_number(e1);int _tmp10A=Cyc_Evexp_const_exp_case_number(
e2);if(_tmp109 != _tmp10A)return _tmp109 - _tmp10A;{struct _tuple3 _tmp10C=({struct
_tuple3 _tmp10B;_tmp10B.f1=(void*)e1->r;_tmp10B.f2=(void*)e2->r;_tmp10B;});void*
_tmp10D;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*
_tmp110;void*_tmp111;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp113;
struct Cyc_Absyn_Exp*_tmp114;void*_tmp115;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*
_tmp117;void*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;
void*_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;void*
_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;void*_tmp121;
void*_tmp122;struct Cyc_List_List*_tmp123;void*_tmp124;void*_tmp125;struct Cyc_List_List*
_tmp126;void*_tmp127;void*_tmp128;void*_tmp129;void*_tmp12A;void*_tmp12B;void*
_tmp12C;void*_tmp12D;struct Cyc_Absyn_Exp*_tmp12E;void*_tmp12F;struct Cyc_Absyn_Exp*
_tmp130;void*_tmp131;void*_tmp132;void*_tmp133;struct Cyc_Absyn_Exp*_tmp134;void*
_tmp135;struct Cyc_Absyn_Exp*_tmp136;void*_tmp137;void*_tmp138;void*_tmp139;void*
_tmp13A;void*_tmp13B;void*_tmp13C;void*_tmp13D;void*_tmp13E;struct Cyc_Absyn_Exp*
_tmp13F;void*_tmp140;void*_tmp141;struct Cyc_Absyn_Exp*_tmp142;_LLA8: _tmp10D=
_tmp10C.f1;if(*((int*)_tmp10D)!= 6)goto _LLAA;_tmp10E=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp10D)->f1;_tmp10F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp10D)->f2;
_tmp110=((struct Cyc_Absyn_Conditional_e_struct*)_tmp10D)->f3;_tmp111=_tmp10C.f2;
if(*((int*)_tmp111)!= 6)goto _LLAA;_tmp112=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp111)->f1;_tmp113=((struct Cyc_Absyn_Conditional_e_struct*)_tmp111)->f2;
_tmp114=((struct Cyc_Absyn_Conditional_e_struct*)_tmp111)->f3;_LLA9: {int _tmp143=
Cyc_Evexp_const_exp_cmp(_tmp110,_tmp114);if(_tmp143 != 0)return _tmp143;_tmp116=
_tmp10E;_tmp117=_tmp10F;_tmp119=_tmp112;_tmp11A=_tmp113;goto _LLAB;}_LLAA: _tmp115=
_tmp10C.f1;if(*((int*)_tmp115)!= 7)goto _LLAC;_tmp116=((struct Cyc_Absyn_And_e_struct*)
_tmp115)->f1;_tmp117=((struct Cyc_Absyn_And_e_struct*)_tmp115)->f2;_tmp118=
_tmp10C.f2;if(*((int*)_tmp118)!= 7)goto _LLAC;_tmp119=((struct Cyc_Absyn_And_e_struct*)
_tmp118)->f1;_tmp11A=((struct Cyc_Absyn_And_e_struct*)_tmp118)->f2;_LLAB: _tmp11C=
_tmp116;_tmp11D=_tmp117;_tmp11F=_tmp119;_tmp120=_tmp11A;goto _LLAD;_LLAC: _tmp11B=
_tmp10C.f1;if(*((int*)_tmp11B)!= 8)goto _LLAE;_tmp11C=((struct Cyc_Absyn_Or_e_struct*)
_tmp11B)->f1;_tmp11D=((struct Cyc_Absyn_Or_e_struct*)_tmp11B)->f2;_tmp11E=_tmp10C.f2;
if(*((int*)_tmp11E)!= 8)goto _LLAE;_tmp11F=((struct Cyc_Absyn_Or_e_struct*)_tmp11E)->f1;
_tmp120=((struct Cyc_Absyn_Or_e_struct*)_tmp11E)->f2;_LLAD: {int _tmp144=Cyc_Evexp_const_exp_cmp(
_tmp11C,_tmp11F);if(_tmp144 != 0)return _tmp144;return Cyc_Evexp_const_exp_cmp(
_tmp11D,_tmp120);}_LLAE: _tmp121=_tmp10C.f1;if(*((int*)_tmp121)!= 3)goto _LLB0;
_tmp122=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp121)->f1;_tmp123=((struct
Cyc_Absyn_Primop_e_struct*)_tmp121)->f2;_tmp124=_tmp10C.f2;if(*((int*)_tmp124)!= 
3)goto _LLB0;_tmp125=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp124)->f1;
_tmp126=((struct Cyc_Absyn_Primop_e_struct*)_tmp124)->f2;_LLAF: {int _tmp145=(int)
_tmp122 - (int)_tmp125;if(_tmp145 != 0)return _tmp145;for(0;_tmp123 != 0  && _tmp126
!= 0;(_tmp123=_tmp123->tl,_tmp126=_tmp126->tl)){int _tmp146=Cyc_Evexp_const_exp_cmp((
struct Cyc_Absyn_Exp*)_tmp123->hd,(struct Cyc_Absyn_Exp*)_tmp126->hd);if(_tmp146 != 
0)return _tmp146;}return 0;}_LLB0: _tmp127=_tmp10C.f1;if(*((int*)_tmp127)!= 18)goto
_LLB2;_tmp128=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp127)->f1;_tmp129=
_tmp10C.f2;if(*((int*)_tmp129)!= 18)goto _LLB2;_tmp12A=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp129)->f1;_LLB1: return Cyc_Tcutil_typecmp(_tmp128,_tmp12A);_LLB2: _tmp12B=
_tmp10C.f1;if(*((int*)_tmp12B)!= 18)goto _LLB4;_tmp12C=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp12B)->f1;_tmp12D=_tmp10C.f2;if(*((int*)_tmp12D)!= 19)goto _LLB4;_tmp12E=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp12D)->f1;_LLB3: if(_tmp12E->topt == 0)({
void*_tmp147[0]={};Cyc_Tcutil_terr(e2->loc,({const char*_tmp148="cannot handle sizeof(exp) here -- use sizeof(type)";
_tag_arr(_tmp148,sizeof(char),_get_zero_arr_size(_tmp148,51));}),_tag_arr(
_tmp147,sizeof(void*),0));});return Cyc_Tcutil_typecmp(_tmp12C,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp12E->topt))->v);_LLB4: _tmp12F=_tmp10C.f1;if(*((int*)_tmp12F)!= 
19)goto _LLB6;_tmp130=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp12F)->f1;_tmp131=
_tmp10C.f2;if(*((int*)_tmp131)!= 18)goto _LLB6;_tmp132=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp131)->f1;_LLB5: if(_tmp130->topt == 0)({void*_tmp149[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp14A="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_arr(
_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,51));}),_tag_arr(_tmp149,sizeof(
void*),0));});return Cyc_Tcutil_typecmp((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp130->topt))->v,_tmp132);_LLB6: _tmp133=_tmp10C.f1;if(*((int*)_tmp133)!= 19)
goto _LLB8;_tmp134=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp133)->f1;_tmp135=
_tmp10C.f2;if(*((int*)_tmp135)!= 19)goto _LLB8;_tmp136=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp135)->f1;_LLB7: if(_tmp134->topt == 0)({void*_tmp14B[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp14C="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_arr(
_tmp14C,sizeof(char),_get_zero_arr_size(_tmp14C,51));}),_tag_arr(_tmp14B,sizeof(
void*),0));});if(_tmp136->topt == 0)({void*_tmp14D[0]={};Cyc_Tcutil_terr(e2->loc,({
const char*_tmp14E="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_arr(
_tmp14E,sizeof(char),_get_zero_arr_size(_tmp14E,51));}),_tag_arr(_tmp14D,sizeof(
void*),0));});return Cyc_Tcutil_typecmp((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp134->topt))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp136->topt))->v);
_LLB8: _tmp137=_tmp10C.f1;if(*((int*)_tmp137)!= 20)goto _LLBA;_tmp138=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp137)->f1;_tmp139=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp137)->f2;_tmp13A=_tmp10C.f2;if(*((int*)_tmp13A)!= 20)goto _LLBA;_tmp13B=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp13A)->f1;_tmp13C=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp13A)->f2;_LLB9: {int _tmp14F=Cyc_Tcutil_typecmp(_tmp138,_tmp13B);if(_tmp14F != 
0)return _tmp14F;{struct _tuple3 _tmp151=({struct _tuple3 _tmp150;_tmp150.f1=_tmp139;
_tmp150.f2=_tmp13C;_tmp150;});void*_tmp152;void*_tmp153;void*_tmp154;void*
_tmp155;void*_tmp156;struct _tagged_arr*_tmp157;void*_tmp158;struct _tagged_arr*
_tmp159;void*_tmp15A;unsigned int _tmp15B;void*_tmp15C;unsigned int _tmp15D;_LLBF:
_tmp152=_tmp151.f1;if(*((int*)_tmp152)!= 0)goto _LLC1;_tmp153=_tmp151.f2;if(*((
int*)_tmp153)!= 1)goto _LLC1;_LLC0: return - 1;_LLC1: _tmp154=_tmp151.f1;if(*((int*)
_tmp154)!= 1)goto _LLC3;_tmp155=_tmp151.f2;if(*((int*)_tmp155)!= 0)goto _LLC3;
_LLC2: return 1;_LLC3: _tmp156=_tmp151.f1;if(*((int*)_tmp156)!= 0)goto _LLC5;_tmp157=((
struct Cyc_Absyn_StructField_struct*)_tmp156)->f1;_tmp158=_tmp151.f2;if(*((int*)
_tmp158)!= 0)goto _LLC5;_tmp159=((struct Cyc_Absyn_StructField_struct*)_tmp158)->f1;
_LLC4: return Cyc_strptrcmp(_tmp157,_tmp159);_LLC5: _tmp15A=_tmp151.f1;if(*((int*)
_tmp15A)!= 1)goto _LLBE;_tmp15B=((struct Cyc_Absyn_TupleIndex_struct*)_tmp15A)->f1;
_tmp15C=_tmp151.f2;if(*((int*)_tmp15C)!= 1)goto _LLBE;_tmp15D=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp15C)->f1;_LLC6: return(int)(_tmp15B - _tmp15D);_LLBE:;}}_LLBA: _tmp13D=_tmp10C.f1;
if(*((int*)_tmp13D)!= 15)goto _LLBC;_tmp13E=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp13D)->f1;_tmp13F=((struct Cyc_Absyn_Cast_e_struct*)_tmp13D)->f2;_tmp140=
_tmp10C.f2;if(*((int*)_tmp140)!= 15)goto _LLBC;_tmp141=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp140)->f1;_tmp142=((struct Cyc_Absyn_Cast_e_struct*)_tmp140)->f2;_LLBB: {int
_tmp15E=Cyc_Tcutil_typecmp(_tmp13E,_tmp141);if(_tmp15E != 0)return _tmp15E;return
Cyc_Evexp_const_exp_cmp(_tmp13F,_tmp142);}_LLBC:;_LLBD:({void*_tmp15F[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp160="Evexp::const_exp_cmp, unexpected case";_tag_arr(_tmp160,sizeof(char),
_get_zero_arr_size(_tmp160,38));}),_tag_arr(_tmp15F,sizeof(void*),0));});_LLA7:;}}}}
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return Cyc_Evexp_const_exp_cmp(e1,e2)== 0;}int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){unsigned int _tmp162;int _tmp163;struct _tuple4 _tmp161=
Cyc_Evexp_eval_const_uint_exp(e1);_tmp162=_tmp161.f1;_tmp163=_tmp161.f2;{
unsigned int _tmp165;int _tmp166;struct _tuple4 _tmp164=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp165=_tmp164.f1;_tmp166=_tmp164.f2;if(_tmp163  && _tmp166)return _tmp162 <= 
_tmp165;return Cyc_Evexp_same_const_exp(e1,e2);}}struct _tuple7{struct Cyc_Absyn_Tqual
f1;void*f2;};int Cyc_Evexp_okay_szofarg(void*t){void*_tmp167=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp168;struct Cyc_Core_Opt*_tmp169;struct Cyc_Core_Opt
_tmp16A;void*_tmp16B;struct Cyc_Absyn_TunionFieldInfo _tmp16C;void*_tmp16D;struct
Cyc_Absyn_Tunionfield*_tmp16E;struct Cyc_List_List*_tmp16F;struct Cyc_Absyn_ArrayInfo
_tmp170;void*_tmp171;struct Cyc_Absyn_Exp*_tmp172;struct Cyc_Absyn_AggrInfo _tmp173;
void*_tmp174;struct Cyc_List_List*_tmp175;struct Cyc_Absyn_Typedefdecl*_tmp176;
_LLC8: if((int)_tmp167 != 0)goto _LLCA;_LLC9: return 0;_LLCA: if(_tmp167 <= (void*)3
 || *((int*)_tmp167)!= 1)goto _LLCC;_tmp168=((struct Cyc_Absyn_VarType_struct*)
_tmp167)->f1;_LLCB: {void*_tmp177=Cyc_Tcutil_tvar_kind(_tmp168);_LLFF: if((int)
_tmp177 != 2)goto _LL101;_LL100: return 1;_LL101:;_LL102: return 0;_LLFE:;}_LLCC: if(
_tmp167 <= (void*)3  || *((int*)_tmp167)!= 0)goto _LLCE;_tmp169=((struct Cyc_Absyn_Evar_struct*)
_tmp167)->f1;if(_tmp169 == 0)goto _LLCE;_tmp16A=*_tmp169;_tmp16B=(void*)_tmp16A.v;
if((int)_tmp16B != 2)goto _LLCE;_LLCD: return 1;_LLCE: if(_tmp167 <= (void*)3  || *((
int*)_tmp167)!= 0)goto _LLD0;_LLCF: return 0;_LLD0: if(_tmp167 <= (void*)3  || *((int*)
_tmp167)!= 2)goto _LLD2;_LLD1: return 1;_LLD2: if(_tmp167 <= (void*)3  || *((int*)
_tmp167)!= 3)goto _LLD4;_tmp16C=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp167)->f1;
_tmp16D=(void*)_tmp16C.field_info;if(*((int*)_tmp16D)!= 1)goto _LLD4;_tmp16E=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp16D)->f2;_LLD3: _tmp16F=_tmp16E->typs;
goto _LLD5;_LLD4: if(_tmp167 <= (void*)3  || *((int*)_tmp167)!= 9)goto _LLD6;_tmp16F=((
struct Cyc_Absyn_TupleType_struct*)_tmp167)->f1;_LLD5: for(0;_tmp16F != 0;_tmp16F=
_tmp16F->tl){if(!Cyc_Evexp_okay_szofarg((*((struct _tuple7*)_tmp16F->hd)).f2))
return 0;}return 1;_LLD6: if(_tmp167 <= (void*)3  || *((int*)_tmp167)!= 3)goto _LLD8;
_LLD7: return 0;_LLD8: if(_tmp167 <= (void*)3  || *((int*)_tmp167)!= 4)goto _LLDA;
_LLD9: return 1;_LLDA: if(_tmp167 <= (void*)3  || *((int*)_tmp167)!= 14)goto _LLDC;
_LLDB: goto _LLDD;_LLDC: if(_tmp167 <= (void*)3  || *((int*)_tmp167)!= 17)goto _LLDE;
_LLDD: goto _LLDF;_LLDE: if(_tmp167 <= (void*)3  || *((int*)_tmp167)!= 5)goto _LLE0;
_LLDF: goto _LLE1;_LLE0: if((int)_tmp167 != 1)goto _LLE2;_LLE1: goto _LLE3;_LLE2: if(
_tmp167 <= (void*)3  || *((int*)_tmp167)!= 6)goto _LLE4;_LLE3: return 1;_LLE4: if(
_tmp167 <= (void*)3  || *((int*)_tmp167)!= 7)goto _LLE6;_tmp170=((struct Cyc_Absyn_ArrayType_struct*)
_tmp167)->f1;_tmp171=(void*)_tmp170.elt_type;_tmp172=_tmp170.num_elts;_LLE5:
return _tmp172 != 0;_LLE6: if(_tmp167 <= (void*)3  || *((int*)_tmp167)!= 8)goto _LLE8;
_LLE7: return 0;_LLE8: if(_tmp167 <= (void*)3  || *((int*)_tmp167)!= 15)goto _LLEA;
_LLE9: return 1;_LLEA: if(_tmp167 <= (void*)3  || *((int*)_tmp167)!= 10)goto _LLEC;
_tmp173=((struct Cyc_Absyn_AggrType_struct*)_tmp167)->f1;_tmp174=(void*)_tmp173.aggr_info;
_LLEB: {struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp178=_tmp174;struct Cyc_Absyn_Aggrdecl**
_tmp179;struct Cyc_Absyn_Aggrdecl*_tmp17A;_LL104: if(*((int*)_tmp178)!= 0)goto
_LL106;_LL105:({void*_tmp17B[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp17C="szof on unchecked StructType";
_tag_arr(_tmp17C,sizeof(char),_get_zero_arr_size(_tmp17C,29));}),_tag_arr(
_tmp17B,sizeof(void*),0));});_LL106: if(*((int*)_tmp178)!= 1)goto _LL103;_tmp179=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp178)->f1;_tmp17A=*_tmp179;_LL107: ad=
_tmp17A;_LL103:;}if(ad->impl == 0)return 0;_tmp175=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;goto _LLED;}_LLEC: if(_tmp167 <= (void*)3  || *((int*)
_tmp167)!= 11)goto _LLEE;_tmp175=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp167)->f2;
_LLED: for(0;_tmp175 != 0;_tmp175=_tmp175->tl){if(!Cyc_Evexp_okay_szofarg((void*)((
struct Cyc_Absyn_Aggrfield*)_tmp175->hd)->type))return 0;}return 1;_LLEE: if(_tmp167
<= (void*)3  || *((int*)_tmp167)!= 12)goto _LLF0;_LLEF: goto _LLF1;_LLF0: if(_tmp167
<= (void*)3  || *((int*)_tmp167)!= 13)goto _LLF2;_LLF1: return 1;_LLF2: if(_tmp167 <= (
void*)3  || *((int*)_tmp167)!= 18)goto _LLF4;_LLF3: goto _LLF5;_LLF4: if((int)_tmp167
!= 2)goto _LLF6;_LLF5: goto _LLF7;_LLF6: if(_tmp167 <= (void*)3  || *((int*)_tmp167)!= 
20)goto _LLF8;_LLF7: goto _LLF9;_LLF8: if(_tmp167 <= (void*)3  || *((int*)_tmp167)!= 
19)goto _LLFA;_LLF9: goto _LLFB;_LLFA: if(_tmp167 <= (void*)3  || *((int*)_tmp167)!= 
21)goto _LLFC;_LLFB: return 0;_LLFC: if(_tmp167 <= (void*)3  || *((int*)_tmp167)!= 16)
goto _LLC7;_tmp176=((struct Cyc_Absyn_TypedefType_struct*)_tmp167)->f3;_LLFD: if(
_tmp176 == 0  || _tmp176->kind == 0)({void*_tmp17D[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp180;_tmp180.tag=0;_tmp180.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp17E[1]={& _tmp180};Cyc_aprintf(({const char*_tmp17F="szof typedeftype %s";
_tag_arr(_tmp17F,sizeof(char),_get_zero_arr_size(_tmp17F,20));}),_tag_arr(
_tmp17E,sizeof(void*),1));}}),_tag_arr(_tmp17D,sizeof(void*),0));});return(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp176->kind))->v == (void*)2;_LLC7:;}
