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
 struct Cyc_Core_Opt{void*v;};void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc___cycFILE;
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
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);extern char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct
_tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[
14];struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};
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
_tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
15];int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;extern struct _tuple0*
Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;void*Cyc_Absyn_tagged_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);struct
Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init);void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*
eff_typ,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(void*info);struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));int Cyc_Dict_member(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d,
void*accum);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*
d);struct Cyc_Dict_Dict*Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict*d);
struct _tuple3{void*f1;void*f2;};struct _tuple3*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple3*Cyc_Dict_rchoose(struct
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
ae;struct Cyc_Core_Opt*le;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();struct Cyc_Interface_I;
struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*Cyc_Interface_final();
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict*ae);struct
_tuple4{struct _tagged_arr f1;struct _tagged_arr f2;};int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple4*info);struct Cyc_Interface_I*
Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct
_tuple4*info);struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*
li,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(
struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*
linfo);void Cyc_Interface_print(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);void Cyc_Interface_save(
struct Cyc_Interface_I*,struct Cyc___cycFILE*);struct Cyc_Interface_I*Cyc_Interface_load(
struct Cyc___cycFILE*);int Cyc_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*s2);
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
void*rep);struct _tuple9{struct Cyc_Dict_Dict*f1;int f2;};struct _tuple10{struct
_tagged_arr f1;int f2;};struct _tuple9 Cyc_Marshal_write_type_base(void*rep,struct
_tuple9 env,struct Cyc___cycFILE*fp,void*val);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*);void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct
Cyc_List_List*ds);extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct
Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_List_List*
Cyc_Parse_parse_file(struct Cyc___cycFILE*f);struct Cyc_Declaration_spec;struct Cyc_Declarator;
struct Cyc_Abstractdeclarator;int Cyc_yyparse();extern char Cyc_AbstractDeclarator_tok[
27];struct Cyc_AbstractDeclarator_tok_struct{char*tag;struct Cyc_Abstractdeclarator*
f1;};extern char Cyc_AggrFieldDeclListList_tok[30];struct Cyc_AggrFieldDeclListList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_AggrFieldDeclList_tok[26];struct
Cyc_AggrFieldDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_AttributeList_tok[
22];struct Cyc_AttributeList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern
char Cyc_Attribute_tok[18];struct Cyc_Attribute_tok_struct{char*tag;void*f1;};
extern char Cyc_Bool_tok[13];struct Cyc_Bool_tok_struct{char*tag;int f1;};extern char
Cyc_Char_tok[13];struct Cyc_Char_tok_struct{char*tag;char f1;};extern char Cyc_DeclList_tok[
17];struct Cyc_DeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char
Cyc_DeclSpec_tok[17];struct Cyc_DeclSpec_tok_struct{char*tag;struct Cyc_Declaration_spec*
f1;};extern char Cyc_Declarator_tok[19];struct Cyc_Declarator_tok_struct{char*tag;
struct Cyc_Declarator*f1;};extern char Cyc_DesignatorList_tok[23];struct Cyc_DesignatorList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Designator_tok[19];struct Cyc_Designator_tok_struct{
char*tag;void*f1;};extern char Cyc_EnumfieldList_tok[22];struct Cyc_EnumfieldList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Enumfield_tok[18];struct Cyc_Enumfield_tok_struct{
char*tag;struct Cyc_Absyn_Enumfield*f1;};extern char Cyc_ExpList_tok[16];struct Cyc_ExpList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Exp_tok[12];struct Cyc_Exp_tok_struct{
char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_FieldPatternList_tok[25];struct
Cyc_FieldPatternList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_FieldPattern_tok[
21];struct _tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
char*tag;struct _tuple11*f1;};extern char Cyc_FnDecl_tok[15];struct Cyc_FnDecl_tok_struct{
char*tag;struct Cyc_Absyn_Fndecl*f1;};extern char Cyc_IdList_tok[15];struct Cyc_IdList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_InitDeclList_tok[21];struct Cyc_InitDeclList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_InitDecl_tok[17];struct _tuple12{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
char*tag;struct _tuple12*f1;};extern char Cyc_InitializerList_tok[24];struct Cyc_InitializerList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Int_tok[12];struct _tuple13{void*
f1;int f2;};struct Cyc_Int_tok_struct{char*tag;struct _tuple13*f1;};extern char Cyc_Kind_tok[
13];struct Cyc_Kind_tok_struct{char*tag;void*f1;};extern char Cyc_Okay_tok[13];
extern char Cyc_ParamDeclListBool_tok[26];struct _tuple14{struct Cyc_List_List*f1;
int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;
};struct Cyc_ParamDeclListBool_tok_struct{char*tag;struct _tuple14*f1;};extern char
Cyc_ParamDeclList_tok[22];struct Cyc_ParamDeclList_tok_struct{char*tag;struct Cyc_List_List*
f1;};extern char Cyc_ParamDecl_tok[18];struct Cyc_ParamDecl_tok_struct{char*tag;
struct _tuple1*f1;};extern char Cyc_PatternList_tok[20];struct Cyc_PatternList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Pattern_tok[16];struct Cyc_Pattern_tok_struct{
char*tag;struct Cyc_Absyn_Pat*f1;};extern char Cyc_Pointer_Sort_tok[21];struct Cyc_Pointer_Sort_tok_struct{
char*tag;void*f1;};extern char Cyc_Primop_tok[15];struct Cyc_Primop_tok_struct{char*
tag;void*f1;};extern char Cyc_Primopopt_tok[18];struct Cyc_Primopopt_tok_struct{
char*tag;struct Cyc_Core_Opt*f1;};extern char Cyc_QualId_tok[15];struct Cyc_QualId_tok_struct{
char*tag;struct _tuple0*f1;};extern char Cyc_QualSpecList_tok[21];struct _tuple15{
struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
Cyc_QualSpecList_tok_struct{char*tag;struct _tuple15*f1;};extern char Cyc_Rgnorder_tok[
17];struct Cyc_Rgnorder_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char
Cyc_Scope_tok[14];struct Cyc_Scope_tok_struct{char*tag;void*f1;};extern char Cyc_Short_tok[
14];struct Cyc_Short_tok_struct{char*tag;short f1;};extern char Cyc_Stmt_tok[13];
struct Cyc_Stmt_tok_struct{char*tag;struct Cyc_Absyn_Stmt*f1;};extern char Cyc_StorageClass_tok[
21];struct Cyc_StorageClass_tok_struct{char*tag;void*f1;};extern char Cyc_String_tok[
15];struct Cyc_String_tok_struct{char*tag;struct _tagged_arr f1;};extern char Cyc_Stringopt_tok[
18];struct Cyc_Stringopt_tok_struct{char*tag;struct Cyc_Core_Opt*f1;};extern char
Cyc_StructOrUnion_tok[22];struct Cyc_StructOrUnion_tok_struct{char*tag;void*f1;};
extern char Cyc_SwitchCClauseList_tok[26];struct Cyc_SwitchCClauseList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_SwitchClauseList_tok[25];struct
Cyc_SwitchClauseList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_TunionFieldList_tok[
24];struct Cyc_TunionFieldList_tok_struct{char*tag;struct Cyc_List_List*f1;};
extern char Cyc_TunionField_tok[20];struct Cyc_TunionField_tok_struct{char*tag;
struct Cyc_Absyn_Tunionfield*f1;};extern char Cyc_TypeList_tok[17];struct Cyc_TypeList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_TypeModifierList_tok[25];struct
Cyc_TypeModifierList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_TypeOpt_tok[
16];struct Cyc_TypeOpt_tok_struct{char*tag;struct Cyc_Core_Opt*f1;};extern char Cyc_TypeQual_tok[
17];struct Cyc_TypeQual_tok_struct{char*tag;struct Cyc_Absyn_Tqual f1;};extern char
Cyc_TypeSpecifier_tok[22];struct Cyc_TypeSpecifier_tok_struct{char*tag;void*f1;};
extern char Cyc_Type_tok[13];struct Cyc_Type_tok_struct{char*tag;void*f1;};int Cyc_yyparse();
extern char Cyc_YY1[8];struct _tuple16{struct Cyc_Absyn_Conref*f1;struct Cyc_Absyn_Conref*
f2;};struct Cyc_YY1_struct{char*tag;struct _tuple16*f1;};extern char Cyc_YY2[8];
struct Cyc_YY2_struct{char*tag;struct Cyc_Absyn_Conref*f1;};extern char Cyc_YY3[8];
struct Cyc_YY3_struct{char*tag;struct _tuple14*f1;};extern char Cyc_YY4[8];struct Cyc_YY4_struct{
char*tag;struct Cyc_Absyn_Conref*f1;};extern char Cyc_YYINITIALSVAL[18];struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(
void*t);int Cyc_Tcutil_is_function_type(void*t);extern char Cyc_Tcdecl_Incompatible[
17];struct Cyc_Tcdecl_Xtunionfielddecl{struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*
field;};void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*loc,struct _tagged_arr*
msg1,struct _tagged_arr fmt,struct _tagged_arr ap);struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(
struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);struct Cyc_Absyn_Tuniondecl*Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);struct Cyc_Tcdecl_Xtunionfielddecl*Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl*d0,struct Cyc_Tcdecl_Xtunionfielddecl*d1,struct
Cyc_Position_Segment*loc,struct _tagged_arr*msg);void Cyc_Lex_lex_init();struct Cyc_Interface_Ienv{
struct Cyc_Dict_Dict*aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*
enumdecls;struct Cyc_Dict_Dict*typedefdecls;struct Cyc_Dict_Dict*vardecls;struct
Cyc_Dict_Dict*xtunionfielddecls;};struct Cyc_Interface_I{struct Cyc_Dict_Dict*
imports;struct Cyc_Dict_Dict*exports;};static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv(){
return({struct Cyc_Interface_Ienv*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->aggrdecls=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_strptrcmp);_tmp0->tuniondecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp0->enumdecls=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_strptrcmp);_tmp0->typedefdecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp0->vardecls=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_strptrcmp);_tmp0->xtunionfielddecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp0;});}
static struct Cyc_Interface_Ienv*Cyc_Interface_lazy_new_ienv(){static struct Cyc_Interface_Ienv*
ie=0;if(ie == 0)ie=(struct Cyc_Interface_Ienv*)Cyc_Interface_new_ienv();return(
struct Cyc_Interface_Ienv*)ie;}static struct Cyc_Interface_Ienv*Cyc_Interface_skel2i_f(
void*ignored){return Cyc_Interface_new_ienv();}static struct Cyc_Interface_I*Cyc_Interface_skel2i(
struct Cyc_Dict_Dict*skel){return({struct Cyc_Interface_I*_tmp1=_cycalloc(sizeof(*
_tmp1));_tmp1->imports=((struct Cyc_Dict_Dict*(*)(struct Cyc_Interface_Ienv*(*f)(
void*),struct Cyc_Dict_Dict*d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel);_tmp1->exports=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Interface_Ienv*(*f)(void*),struct Cyc_Dict_Dict*
d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel);_tmp1;});}struct Cyc_Interface_I*
Cyc_Interface_empty(){return({struct Cyc_Interface_I*_tmp2=_cycalloc(sizeof(*
_tmp2));_tmp2->imports=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct Cyc_List_List*,
struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp);_tmp2->exports=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))
Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp);_tmp2;});}struct Cyc_Interface_I*Cyc_Interface_final(){
struct Cyc_Interface_I*_tmp3=Cyc_Interface_empty();struct Cyc_Interface_Ienv*_tmp4=
Cyc_Interface_new_ienv();struct Cyc_Absyn_Tuniondecl*_tmp5=({struct Cyc_Absyn_Tuniondecl*
_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=*Cyc_Absyn_exn_tud;_tmp16;});(void*)(
_tmp5->sc=(void*)((void*)2));_tmp4->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct
Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl*v))Cyc_Dict_insert)(
_tmp4->tuniondecls,(*Cyc_Absyn_exn_name).f2,_tmp5);{struct Cyc_List_List*_tmp6=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;
for(0;_tmp6 != 0;_tmp6=_tmp6->tl){struct Cyc_Absyn_Tunionfield*_tmp7=({struct Cyc_Absyn_Tunionfield*
_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA[0]=*((struct Cyc_Absyn_Tunionfield*)_tmp6->hd);
_tmpA;});(void*)(_tmp7->sc=(void*)((void*)2));{struct Cyc_Tcdecl_Xtunionfielddecl*
_tmp8=({struct Cyc_Tcdecl_Xtunionfielddecl*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->base=
_tmp5;_tmp9->field=_tmp7;_tmp9;});_tmp4->xtunionfielddecls=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Tcdecl_Xtunionfielddecl*v))
Cyc_Dict_insert)(_tmp4->xtunionfielddecls,(*((struct Cyc_Absyn_Tunionfield*)_tmp6->hd)->name).f2,
_tmp8);}}}_tmp3->exports=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(_tmp3->exports,0,
_tmp4);{struct Cyc_Interface_Ienv*_tmpB=Cyc_Interface_new_ienv();struct _tuple0*
qmain=({struct _tuple0*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->f1=(void*)({
struct Cyc_Absyn_Abs_n_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14[0]=({
struct Cyc_Absyn_Abs_n_struct _tmp15;_tmp15.tag=1;_tmp15.f1=0;_tmp15;});_tmp14;});
_tmp11->f2=({struct _tagged_arr*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[0]=(
struct _tagged_arr)({const char*_tmp13="main";_tag_arr(_tmp13,sizeof(char),
_get_zero_arr_size(_tmp13,5));});_tmp12;});_tmp11;});struct Cyc_Absyn_Vardecl*
_tmpC=Cyc_Absyn_new_vardecl(qmain,Cyc_Absyn_function_typ(0,0,Cyc_Absyn_sint_typ,({
struct Cyc_List_List*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD->hd=({struct _tuple1*
_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->f1=0;_tmp10->f2=Cyc_Absyn_empty_tqual();
_tmp10->f3=Cyc_Absyn_uint_typ;_tmp10;});_tmpD->tl=({struct Cyc_List_List*_tmpE=
_cycalloc(sizeof(*_tmpE));_tmpE->hd=({struct _tuple1*_tmpF=_cycalloc(sizeof(*
_tmpF));_tmpF->f1=0;_tmpF->f2=Cyc_Absyn_empty_tqual();_tmpF->f3=Cyc_Absyn_tagged_typ(
Cyc_Absyn_tagged_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_empty_tqual(),Cyc_Absyn_false_conref),(
void*)2,Cyc_Absyn_empty_tqual(),Cyc_Absyn_false_conref);_tmpF;});_tmpE->tl=0;
_tmpE;});_tmpD;}),0,0,0,0),0);(void*)(_tmpC->sc=(void*)((void*)3));_tmpB->vardecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Vardecl*
v))Cyc_Dict_insert)(_tmpB->vardecls,(*qmain).f2,_tmpC);_tmp3->imports=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*
v))Cyc_Dict_insert)(_tmp3->exports,0,_tmpB);return _tmp3;}}static void Cyc_Interface_err(
struct _tagged_arr msg){({void*_tmp17[0]={};Cyc_Tcutil_terr(0,msg,_tag_arr(_tmp17,
sizeof(void*),0));});}static void*Cyc_Interface_invalid_arg(struct _tagged_arr s){(
int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp18=_cycalloc(
sizeof(*_tmp18));_tmp18[0]=({struct Cyc_Core_Invalid_argument_struct _tmp19;_tmp19.tag=
Cyc_Core_Invalid_argument;_tmp19.f1=s;_tmp19;});_tmp18;}));}static void Cyc_Interface_fields_err(
struct _tagged_arr sc,struct _tagged_arr t,struct _tuple0*n){Cyc_Interface_err((
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp1E;_tmp1E.tag=0;_tmp1E.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(n));{struct Cyc_String_pa_struct
_tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _tagged_arr)((struct _tagged_arr)t);{struct
Cyc_String_pa_struct _tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct _tagged_arr)((struct
_tagged_arr)sc);{void*_tmp1A[3]={& _tmp1C,& _tmp1D,& _tmp1E};Cyc_aprintf(({const
char*_tmp1B="fields of %s %s %s have never been defined";_tag_arr(_tmp1B,sizeof(
char),_get_zero_arr_size(_tmp1B,43));}),_tag_arr(_tmp1A,sizeof(void*),3));}}}}));}
static void Cyc_Interface_body_err(struct _tagged_arr sc,struct _tuple0*n){Cyc_Interface_err((
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(n));{struct Cyc_String_pa_struct
_tmp21;_tmp21.tag=0;_tmp21.f1=(struct _tagged_arr)((struct _tagged_arr)sc);{void*
_tmp1F[2]={& _tmp21,& _tmp22};Cyc_aprintf(({const char*_tmp20="the body of %s function %s has never been defined";
_tag_arr(_tmp20,sizeof(char),_get_zero_arr_size(_tmp20,50));}),_tag_arr(_tmp1F,
sizeof(void*),2));}}}));}static void Cyc_Interface_static_err(struct _tagged_arr
obj1,struct _tuple0*name1,struct _tagged_arr obj2,struct _tuple0*name2){if(obj1.curr
!= ((struct _tagged_arr)_tag_arr(0,0,0)).curr)Cyc_Interface_err((struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(name2));{struct Cyc_String_pa_struct
_tmp27;_tmp27.tag=0;_tmp27.f1=(struct _tagged_arr)((struct _tagged_arr)obj2);{
struct Cyc_String_pa_struct _tmp26;_tmp26.tag=0;_tmp26.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(name1));{struct Cyc_String_pa_struct
_tmp25;_tmp25.tag=0;_tmp25.f1=(struct _tagged_arr)((struct _tagged_arr)obj1);{void*
_tmp23[4]={& _tmp25,& _tmp26,& _tmp27,& _tmp28};Cyc_aprintf(({const char*_tmp24="declaration of %s %s relies on static %s %s";
_tag_arr(_tmp24,sizeof(char),_get_zero_arr_size(_tmp24,44));}),_tag_arr(_tmp23,
sizeof(void*),4));}}}}}));}static void Cyc_Interface_abstract_err(struct
_tagged_arr obj1,struct _tuple0*name1,struct _tagged_arr obj2,struct _tuple0*name2){
if(obj1.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr)Cyc_Interface_err((
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp2E;_tmp2E.tag=0;_tmp2E.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(name2));{struct Cyc_String_pa_struct
_tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _tagged_arr)((struct _tagged_arr)obj2);{
struct Cyc_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(name1));{struct Cyc_String_pa_struct
_tmp2B;_tmp2B.tag=0;_tmp2B.f1=(struct _tagged_arr)((struct _tagged_arr)obj1);{void*
_tmp29[4]={& _tmp2B,& _tmp2C,& _tmp2D,& _tmp2E};Cyc_aprintf(({const char*_tmp2A="declaration of %s %s relies on fields of abstract %s %s";
_tag_arr(_tmp2A,sizeof(char),_get_zero_arr_size(_tmp2A,56));}),_tag_arr(_tmp29,
sizeof(void*),4));}}}}}));}struct Cyc_Interface_Seen{struct Cyc_Dict_Dict*aggrs;
struct Cyc_Dict_Dict*tunions;};static struct Cyc_Interface_Seen*Cyc_Interface_new_seen(){
return({struct Cyc_Interface_Seen*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->aggrs=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp);_tmp2F->tunions=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp2F;});}static
int Cyc_Interface_check_public_type(struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*
seen,struct _tagged_arr obj,struct _tuple0*name,void*t);static int Cyc_Interface_check_public_type_list(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,struct
_tuple0*name,void*(*f)(void*),struct Cyc_List_List*l){int _tmp30=1;for(0;l != 0;l=l->tl){
if(!Cyc_Interface_check_public_type(ae,seen,obj,name,f((void*)l->hd)))_tmp30=0;}
return _tmp30;}static int Cyc_Interface_check_public_aggrdecl(struct Cyc_Dict_Dict*
ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d){{struct
_handler_cons _tmp31;_push_handler(& _tmp31);{int _tmp33=0;if(setjmp(_tmp31.handler))
_tmp33=1;if(!_tmp33){{int _tmp34=((int(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))
Cyc_Dict_lookup)(seen->aggrs,d->name);_npop_handler(0);return _tmp34;};
_pop_handler();}else{void*_tmp32=(void*)_exn_thrown;void*_tmp36=_tmp32;_LL1: if(
_tmp36 != Cyc_Dict_Absent)goto _LL3;_LL2: goto _LL0;_LL3:;_LL4:(void)_throw(_tmp36);
_LL0:;}}}{int _tmp37=1;seen->aggrs=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp37);if(d->impl
!= 0){struct Cyc_List_List*_tmp38=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(d->impl))->fields;
for(0;_tmp38 != 0;_tmp38=_tmp38->tl){if(!Cyc_Interface_check_public_type(ae,seen,({
const char*_tmp39="type";_tag_arr(_tmp39,sizeof(char),_get_zero_arr_size(_tmp39,5));}),
d->name,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp38->hd)->type))_tmp37=0;}}seen->aggrs=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)(
seen->aggrs,d->name,_tmp37);return _tmp37;}}struct _tuple17{struct Cyc_Absyn_Tqual
f1;void*f2;};static void*Cyc_Interface_get_type1(struct _tuple17*x){return(*x).f2;}
static void*Cyc_Interface_get_type2(struct _tuple1*x){return(*x).f3;}static int Cyc_Interface_check_public_tuniondecl(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Tuniondecl*
d){{struct _handler_cons _tmp3A;_push_handler(& _tmp3A);{int _tmp3C=0;if(setjmp(
_tmp3A.handler))_tmp3C=1;if(!_tmp3C){{int _tmp3D=((int(*)(struct Cyc_Dict_Dict*d,
struct _tuple0*k))Cyc_Dict_lookup)(seen->tunions,d->name);_npop_handler(0);return
_tmp3D;};_pop_handler();}else{void*_tmp3B=(void*)_exn_thrown;void*_tmp3F=_tmp3B;
_LL6: if(_tmp3F != Cyc_Dict_Absent)goto _LL8;_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(
_tmp3F);_LL5:;}}}{int _tmp40=1;seen->tunions=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->tunions,d->name,_tmp40);if(d->fields
!= 0){struct Cyc_List_List*_tmp41=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(d->fields))->v;for(0;_tmp41 != 0;_tmp41=_tmp41->tl){if(!((int(*)(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,struct
_tuple0*name,void*(*f)(struct _tuple17*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(
ae,seen,({const char*_tmp42="[x]tunion";_tag_arr(_tmp42,sizeof(char),
_get_zero_arr_size(_tmp42,10));}),d->name,Cyc_Interface_get_type1,((struct Cyc_Absyn_Tunionfield*)
_tmp41->hd)->typs))_tmp40=0;}}seen->tunions=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->tunions,d->name,_tmp40);return
_tmp40;}}static int Cyc_Interface_check_public_enumdecl(struct Cyc_Dict_Dict*ae,
struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Enumdecl*d){return 1;}static int Cyc_Interface_check_public_typedefdecl(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*
d){if(d->defn != 0)return Cyc_Interface_check_public_type(ae,seen,(struct
_tagged_arr)_tag_arr(0,0,0),d->name,(void*)((struct Cyc_Core_Opt*)_check_null(d->defn))->v);
else{return 1;}}static int Cyc_Interface_check_public_vardecl(struct Cyc_Dict_Dict*
ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d){return Cyc_Interface_check_public_type(
ae,seen,({const char*_tmp43="variable";_tag_arr(_tmp43,sizeof(char),
_get_zero_arr_size(_tmp43,9));}),d->name,(void*)d->type);}static struct Cyc_List_List*
Cyc_Interface_get_abs_ns(void*ns){void*_tmp44=ns;struct Cyc_List_List*_tmp45;_LLB:
if(_tmp44 <= (void*)1?1:*((int*)_tmp44)!= 1)goto _LLD;_tmp45=((struct Cyc_Absyn_Abs_n_struct*)
_tmp44)->f1;_LLC: return _tmp45;_LLD:;_LLE:((int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)(({
const char*_tmp46="get_abs_ns";_tag_arr(_tmp46,sizeof(char),_get_zero_arr_size(
_tmp46,11));}));_LLA:;}static int Cyc_Interface_check_public_type(struct Cyc_Dict_Dict*
ae,struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,struct _tuple0*name,void*t){
void*_tmp47=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp48;void*_tmp49;
struct Cyc_Absyn_ArrayInfo _tmp4A;void*_tmp4B;void**_tmp4C;void*_tmp4D;struct Cyc_Absyn_FnInfo
_tmp4E;void*_tmp4F;struct Cyc_List_List*_tmp50;struct Cyc_Absyn_VarargInfo*_tmp51;
struct Cyc_List_List*_tmp52;struct Cyc_Absyn_AggrInfo _tmp53;void*_tmp54;struct Cyc_List_List*
_tmp55;struct _tuple0*_tmp56;struct Cyc_Absyn_TunionInfo _tmp57;void*_tmp58;struct
Cyc_Absyn_Tuniondecl**_tmp59;struct Cyc_Absyn_Tuniondecl*_tmp5A;struct Cyc_List_List*
_tmp5B;struct Cyc_Absyn_TunionFieldInfo _tmp5C;void*_tmp5D;struct Cyc_Absyn_Tuniondecl*
_tmp5E;struct Cyc_Absyn_Tunionfield*_tmp5F;struct Cyc_List_List*_tmp60;_LL10: if(
_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 4)goto _LL12;_tmp48=((struct Cyc_Absyn_PointerType_struct*)
_tmp47)->f1;_tmp49=(void*)_tmp48.elt_typ;_LL11: _tmp4B=_tmp49;goto _LL13;_LL12: if(
_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 7)goto _LL14;_tmp4A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp47)->f1;_tmp4B=(void*)_tmp4A.elt_type;_LL13: _tmp4D=_tmp4B;goto _LL15;_LL14:
if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 16)goto _LL16;_tmp4C=((struct Cyc_Absyn_TypedefType_struct*)
_tmp47)->f4;if(_tmp4C == 0)goto _LL16;_tmp4D=*_tmp4C;_LL15: return Cyc_Interface_check_public_type(
ae,seen,obj,name,_tmp4D);_LL16: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 8)goto
_LL18;_tmp4E=((struct Cyc_Absyn_FnType_struct*)_tmp47)->f1;_tmp4F=(void*)_tmp4E.ret_typ;
_tmp50=_tmp4E.args;_tmp51=_tmp4E.cyc_varargs;_LL17: {int b=((int(*)(struct Cyc_Dict_Dict*
ae,struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,struct _tuple0*name,void*(*
f)(struct _tuple1*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(
ae,seen,obj,name,Cyc_Interface_get_type2,_tmp50)?Cyc_Interface_check_public_type(
ae,seen,obj,name,_tmp4F): 0;if(_tmp51 != 0){void*_tmp62;struct Cyc_Absyn_VarargInfo
_tmp61=*_tmp51;_tmp62=(void*)_tmp61.type;b=Cyc_Interface_check_public_type(ae,
seen,obj,name,_tmp62);}return b;}_LL18: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 9)
goto _LL1A;_tmp52=((struct Cyc_Absyn_TupleType_struct*)_tmp47)->f1;_LL19: return((
int(*)(struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,
struct _tuple0*name,void*(*f)(struct _tuple17*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(
ae,seen,obj,name,Cyc_Interface_get_type1,_tmp52);_LL1A: if(_tmp47 <= (void*)3?1:*((
int*)_tmp47)!= 10)goto _LL1C;_tmp53=((struct Cyc_Absyn_AggrType_struct*)_tmp47)->f1;
_tmp54=(void*)_tmp53.aggr_info;_tmp55=_tmp53.targs;_LL1B: {struct Cyc_Absyn_Aggrdecl*
_tmp63=Cyc_Absyn_get_known_aggrdecl(_tmp54);if((void*)_tmp63->sc == (void*)0){Cyc_Interface_static_err(
obj,name,({const char*_tmp64="type";_tag_arr(_tmp64,sizeof(char),
_get_zero_arr_size(_tmp64,5));}),_tmp63->name);return 0;}return Cyc_Interface_check_public_type_list(
ae,seen,obj,name,Cyc_Core_identity,_tmp55)?Cyc_Interface_check_public_aggrdecl(
ae,seen,_tmp63): 0;}_LL1C: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 12)goto _LL1E;
_tmp56=((struct Cyc_Absyn_EnumType_struct*)_tmp47)->f1;_LL1D: {struct _tuple0
_tmp66;void*_tmp67;struct _tagged_arr*_tmp68;struct _tuple0*_tmp65=_tmp56;_tmp66=*
_tmp65;_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{struct Cyc_List_List*_tmp69=Cyc_Interface_get_abs_ns(
_tmp67);struct Cyc_Absyn_Enumdecl*ed;{struct _handler_cons _tmp6A;_push_handler(&
_tmp6A);{int _tmp6C=0;if(setjmp(_tmp6A.handler))_tmp6C=1;if(!_tmp6C){{struct Cyc_Tcenv_Genv*
_tmp6D=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(ae,_tmp69);ed=*((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp6D->enumdecls,_tmp68);};_pop_handler();}
else{void*_tmp6B=(void*)_exn_thrown;void*_tmp6F=_tmp6B;_LL25: if(_tmp6F != Cyc_Dict_Absent)
goto _LL27;_LL26:((int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)((struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp72;_tmp72.tag=0;_tmp72.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp56));{void*_tmp70[1]={&
_tmp72};Cyc_aprintf(({const char*_tmp71="check_public_type (can't find enum %s)";
_tag_arr(_tmp71,sizeof(char),_get_zero_arr_size(_tmp71,39));}),_tag_arr(_tmp70,
sizeof(void*),1));}}));_LL27:;_LL28:(void)_throw(_tmp6F);_LL24:;}}}if((void*)ed->sc
== (void*)0){Cyc_Interface_static_err(obj,_tmp56,({const char*_tmp73="enum";
_tag_arr(_tmp73,sizeof(char),_get_zero_arr_size(_tmp73,5));}),ed->name);return 0;}
return 1;}}_LL1E: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 2)goto _LL20;_tmp57=((
struct Cyc_Absyn_TunionType_struct*)_tmp47)->f1;_tmp58=(void*)_tmp57.tunion_info;
if(*((int*)_tmp58)!= 1)goto _LL20;_tmp59=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp58)->f1;_tmp5A=*_tmp59;_tmp5B=_tmp57.targs;_LL1F: {struct _tuple0 _tmp75;void*
_tmp76;struct _tagged_arr*_tmp77;struct _tuple0*_tmp74=_tmp5A->name;_tmp75=*_tmp74;
_tmp76=_tmp75.f1;_tmp77=_tmp75.f2;{struct Cyc_List_List*_tmp78=Cyc_Interface_get_abs_ns(
_tmp76);struct Cyc_Absyn_Tuniondecl*tud;{struct _handler_cons _tmp79;_push_handler(&
_tmp79);{int _tmp7B=0;if(setjmp(_tmp79.handler))_tmp7B=1;if(!_tmp7B){{struct Cyc_Tcenv_Genv*
_tmp7C=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(ae,_tmp78);tud=*((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp7C->tuniondecls,_tmp77);};
_pop_handler();}else{void*_tmp7A=(void*)_exn_thrown;void*_tmp7E=_tmp7A;_LL2A: if(
_tmp7E != Cyc_Dict_Absent)goto _LL2C;_LL2B:((int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)((
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp81;_tmp81.tag=0;_tmp81.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp5A->name));{
void*_tmp7F[1]={& _tmp81};Cyc_aprintf(({const char*_tmp80="check_public_type (can't find [x]tunion %s)";
_tag_arr(_tmp80,sizeof(char),_get_zero_arr_size(_tmp80,44));}),_tag_arr(_tmp7F,
sizeof(void*),1));}}));_LL2C:;_LL2D:(void)_throw(_tmp7E);_LL29:;}}}if((void*)tud->sc
== (void*)0){Cyc_Interface_static_err(obj,name,({const char*_tmp82="[x]tunion";
_tag_arr(_tmp82,sizeof(char),_get_zero_arr_size(_tmp82,10));}),tud->name);return
0;}return Cyc_Interface_check_public_type_list(ae,seen,obj,name,Cyc_Core_identity,
_tmp5B)?Cyc_Interface_check_public_tuniondecl(ae,seen,tud): 0;}}_LL20: if(_tmp47 <= (
void*)3?1:*((int*)_tmp47)!= 3)goto _LL22;_tmp5C=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp47)->f1;_tmp5D=(void*)_tmp5C.field_info;if(*((int*)_tmp5D)!= 1)goto _LL22;
_tmp5E=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp5D)->f1;_tmp5F=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp5D)->f2;_tmp60=_tmp5C.targs;_LL21: {struct _tuple0 _tmp84;void*_tmp85;struct
_tagged_arr*_tmp86;struct _tuple0*_tmp83=_tmp5E->name;_tmp84=*_tmp83;_tmp85=
_tmp84.f1;_tmp86=_tmp84.f2;{struct Cyc_List_List*_tmp87=Cyc_Interface_get_abs_ns(
_tmp85);struct Cyc_Absyn_Tuniondecl*tud;{struct _handler_cons _tmp88;_push_handler(&
_tmp88);{int _tmp8A=0;if(setjmp(_tmp88.handler))_tmp8A=1;if(!_tmp8A){{struct Cyc_Tcenv_Genv*
_tmp8B=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(ae,_tmp87);tud=*((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp8B->tuniondecls,_tmp86);};
_pop_handler();}else{void*_tmp89=(void*)_exn_thrown;void*_tmp8D=_tmp89;_LL2F: if(
_tmp8D != Cyc_Dict_Absent)goto _LL31;_LL30:((int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)((
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp90;_tmp90.tag=0;_tmp90.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp5E->name));{
void*_tmp8E[1]={& _tmp90};Cyc_aprintf(({const char*_tmp8F="check_public_type (can't find [x]tunion %s and search its fields)";
_tag_arr(_tmp8F,sizeof(char),_get_zero_arr_size(_tmp8F,66));}),_tag_arr(_tmp8E,
sizeof(void*),1));}}));_LL31:;_LL32:(void)_throw(_tmp8D);_LL2E:;}}}if(tud->fields
== 0)((int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)((struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp93;_tmp93.tag=0;_tmp93.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(tud->name));{void*_tmp91[1]={& _tmp93};
Cyc_aprintf(({const char*_tmp92="check_public_type ([x]tunion %s has no fields)";
_tag_arr(_tmp92,sizeof(char),_get_zero_arr_size(_tmp92,47));}),_tag_arr(_tmp91,
sizeof(void*),1));}}));{struct Cyc_Absyn_Tunionfield*tuf1=0;{struct Cyc_List_List*
_tmp94=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;
for(0;_tmp94 != 0;_tmp94=_tmp94->tl){if(Cyc_strptrcmp((*_tmp5F->name).f2,(*((
struct Cyc_Absyn_Tunionfield*)_tmp94->hd)->name).f2)== 0){tuf1=(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)_tmp94->hd);break;}}}if(tuf1 == 0)((int(*)(struct
_tagged_arr s))Cyc_Interface_invalid_arg)((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp97;_tmp97.tag=0;_tmp97.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp5F->name));{void*_tmp95[1]={& _tmp97};Cyc_aprintf(({const char*_tmp96="check_public_type (can't find [x]tunionfield %s)";
_tag_arr(_tmp96,sizeof(char),_get_zero_arr_size(_tmp96,49));}),_tag_arr(_tmp95,
sizeof(void*),1));}}));{struct Cyc_Absyn_Tunionfield*tuf=(struct Cyc_Absyn_Tunionfield*)
tuf1;if((void*)tud->sc == (void*)0){Cyc_Interface_static_err(obj,name,({const char*
_tmp98="[x]tunion";_tag_arr(_tmp98,sizeof(char),_get_zero_arr_size(_tmp98,10));}),
tud->name);return 0;}if((void*)tud->sc == (void*)1){Cyc_Interface_abstract_err(obj,
name,({const char*_tmp99="[x]tunion";_tag_arr(_tmp99,sizeof(char),
_get_zero_arr_size(_tmp99,10));}),tud->name);return 0;}if((void*)tuf->sc == (void*)
0){Cyc_Interface_static_err(obj,name,(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp9C;_tmp9C.tag=0;_tmp9C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp9A[1]={& _tmp9C};Cyc_aprintf(({const char*_tmp9B="field %s of";
_tag_arr(_tmp9B,sizeof(char),_get_zero_arr_size(_tmp9B,12));}),_tag_arr(_tmp9A,
sizeof(void*),1));}}),tud->name);return 0;}return Cyc_Interface_check_public_type_list(
ae,seen,obj,name,Cyc_Core_identity,_tmp60)?Cyc_Interface_check_public_tuniondecl(
ae,seen,tud): 0;}}}}_LL22:;_LL23: return 1;_LLF:;}struct _tuple18{struct Cyc_Interface_Ienv*
f1;struct Cyc_Interface_Ienv*f2;int f3;struct Cyc_Dict_Dict*f4;struct Cyc_Interface_Seen*
f5;struct Cyc_Interface_I*f6;};static void Cyc_Interface_extract_aggrdecl(struct
_tuple18*env,struct _tagged_arr*x,struct Cyc_Absyn_Aggrdecl**dp){struct _tuple18
_tmp9E;struct Cyc_Interface_Ienv*_tmp9F;struct Cyc_Interface_Ienv*_tmpA0;int _tmpA1;
struct Cyc_Dict_Dict*_tmpA2;struct Cyc_Interface_Seen*_tmpA3;struct _tuple18*_tmp9D=
env;_tmp9E=*_tmp9D;_tmp9F=_tmp9E.f1;_tmpA0=_tmp9E.f2;_tmpA1=_tmp9E.f3;_tmpA2=
_tmp9E.f4;_tmpA3=_tmp9E.f5;{struct Cyc_Absyn_Aggrdecl*_tmpA4=*dp;void*_tmpA5=(
void*)_tmpA4->sc;_LL34: if((int)_tmpA5 != 0)goto _LL36;_LL35: if(_tmpA1?_tmpA4->impl
== 0: 0)Cyc_Interface_fields_err(({const char*_tmpA6="static";_tag_arr(_tmpA6,
sizeof(char),_get_zero_arr_size(_tmpA6,7));}),({const char*_tmpA7="struct/union";
_tag_arr(_tmpA7,sizeof(char),_get_zero_arr_size(_tmpA7,13));}),_tmpA4->name);
goto _LL33;_LL36: if((int)_tmpA5 != 1)goto _LL38;_LL37: if(_tmpA4->impl == 0){if(
_tmpA1)Cyc_Interface_fields_err(({const char*_tmpA8="abstract";_tag_arr(_tmpA8,
sizeof(char),_get_zero_arr_size(_tmpA8,9));}),({const char*_tmpA9="struct/union";
_tag_arr(_tmpA9,sizeof(char),_get_zero_arr_size(_tmpA9,13));}),_tmpA4->name);}
else{_tmpA4=({struct Cyc_Absyn_Aggrdecl*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[
0]=*_tmpA4;_tmpAA;});_tmpA4->impl=0;}if(Cyc_Interface_check_public_aggrdecl(
_tmpA2,_tmpA3,_tmpA4))_tmpA0->aggrdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmpA0->aggrdecls,
x,_tmpA4);goto _LL33;_LL38: if((int)_tmpA5 != 2)goto _LL3A;_LL39: if(_tmpA4->impl == 0){
Cyc_Interface_fields_err(({const char*_tmpAB="public";_tag_arr(_tmpAB,sizeof(char),
_get_zero_arr_size(_tmpAB,7));}),({const char*_tmpAC="struct/union";_tag_arr(
_tmpAC,sizeof(char),_get_zero_arr_size(_tmpAC,13));}),_tmpA4->name);_tmpA4=({
struct Cyc_Absyn_Aggrdecl*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=*_tmpA4;
_tmpAD;});(void*)(_tmpA4->sc=(void*)((void*)1));}if(Cyc_Interface_check_public_aggrdecl(
_tmpA2,_tmpA3,_tmpA4))_tmpA0->aggrdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmpA0->aggrdecls,
x,_tmpA4);goto _LL33;_LL3A: if((int)_tmpA5 != 4)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if((
int)_tmpA5 != 3)goto _LL3E;_LL3D: if(Cyc_Interface_check_public_aggrdecl(_tmpA2,
_tmpA3,_tmpA4))_tmp9F->aggrdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp9F->aggrdecls,
x,_tmpA4);goto _LL33;_LL3E: if((int)_tmpA5 != 5)goto _LL33;_LL3F:((int(*)(struct
_tagged_arr s))Cyc_Interface_invalid_arg)(({const char*_tmpAE="add_aggrdecl";
_tag_arr(_tmpAE,sizeof(char),_get_zero_arr_size(_tmpAE,13));}));goto _LL33;_LL33:;}}
static void Cyc_Interface_extract_enumdecl(struct _tuple18*env,struct _tagged_arr*x,
struct Cyc_Absyn_Enumdecl**dp){struct _tuple18 _tmpB0;struct Cyc_Interface_Ienv*
_tmpB1;struct Cyc_Interface_Ienv*_tmpB2;int _tmpB3;struct Cyc_Dict_Dict*_tmpB4;
struct Cyc_Interface_Seen*_tmpB5;struct _tuple18*_tmpAF=env;_tmpB0=*_tmpAF;_tmpB1=
_tmpB0.f1;_tmpB2=_tmpB0.f2;_tmpB3=_tmpB0.f3;_tmpB4=_tmpB0.f4;_tmpB5=_tmpB0.f5;{
struct Cyc_Absyn_Enumdecl*_tmpB6=*dp;void*_tmpB7=(void*)_tmpB6->sc;_LL41: if((int)
_tmpB7 != 0)goto _LL43;_LL42: if(_tmpB3?_tmpB6->fields == 0: 0)Cyc_Interface_fields_err(({
const char*_tmpB8="static";_tag_arr(_tmpB8,sizeof(char),_get_zero_arr_size(_tmpB8,
7));}),({const char*_tmpB9="enum";_tag_arr(_tmpB9,sizeof(char),_get_zero_arr_size(
_tmpB9,5));}),_tmpB6->name);goto _LL40;_LL43: if((int)_tmpB7 != 1)goto _LL45;_LL44:
if(_tmpB6->fields == 0){if(_tmpB3)Cyc_Interface_fields_err(({const char*_tmpBA="abstract";
_tag_arr(_tmpBA,sizeof(char),_get_zero_arr_size(_tmpBA,9));}),({const char*_tmpBB="enum";
_tag_arr(_tmpBB,sizeof(char),_get_zero_arr_size(_tmpBB,5));}),_tmpB6->name);}
else{_tmpB6=({struct Cyc_Absyn_Enumdecl*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[
0]=*_tmpB6;_tmpBC;});_tmpB6->fields=0;}if(Cyc_Interface_check_public_enumdecl(
_tmpB4,_tmpB5,_tmpB6))_tmpB2->enumdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpB2->enumdecls,
x,_tmpB6);goto _LL40;_LL45: if((int)_tmpB7 != 2)goto _LL47;_LL46: if(_tmpB6->fields == 
0){Cyc_Interface_fields_err(({const char*_tmpBD="public";_tag_arr(_tmpBD,sizeof(
char),_get_zero_arr_size(_tmpBD,7));}),({const char*_tmpBE="enum";_tag_arr(_tmpBE,
sizeof(char),_get_zero_arr_size(_tmpBE,5));}),_tmpB6->name);_tmpB6=({struct Cyc_Absyn_Enumdecl*
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=*_tmpB6;_tmpBF;});(void*)(_tmpB6->sc=(
void*)((void*)1));}if(Cyc_Interface_check_public_enumdecl(_tmpB4,_tmpB5,_tmpB6))
_tmpB2->enumdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpB2->enumdecls,x,
_tmpB6);goto _LL40;_LL47: if((int)_tmpB7 != 4)goto _LL49;_LL48: goto _LL4A;_LL49: if((
int)_tmpB7 != 3)goto _LL4B;_LL4A: if(Cyc_Interface_check_public_enumdecl(_tmpB4,
_tmpB5,_tmpB6))_tmpB1->enumdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpB1->enumdecls,
x,_tmpB6);goto _LL40;_LL4B: if((int)_tmpB7 != 5)goto _LL40;_LL4C:((int(*)(struct
_tagged_arr s))Cyc_Interface_invalid_arg)(({const char*_tmpC0="add_enumdecl";
_tag_arr(_tmpC0,sizeof(char),_get_zero_arr_size(_tmpC0,13));}));goto _LL40;_LL40:;}}
static void Cyc_Interface_extract_xtunionfielddecl(struct Cyc_Interface_I*i,struct
Cyc_Absyn_Tuniondecl*d,struct Cyc_Absyn_Tunionfield*f){struct Cyc_List_List*_tmpC1=
0;{void*_tmpC2=(*f->name).f1;struct Cyc_List_List*_tmpC3;struct Cyc_List_List*
_tmpC4;_LL4E: if(_tmpC2 <= (void*)1?1:*((int*)_tmpC2)!= 0)goto _LL50;_tmpC3=((
struct Cyc_Absyn_Rel_n_struct*)_tmpC2)->f1;_LL4F: _tmpC4=_tmpC3;goto _LL51;_LL50:
if(_tmpC2 <= (void*)1?1:*((int*)_tmpC2)!= 1)goto _LL52;_tmpC4=((struct Cyc_Absyn_Abs_n_struct*)
_tmpC2)->f1;_LL51: _tmpC1=_tmpC4;goto _LL4D;_LL52:;_LL53: goto _LL4D;_LL4D:;}{struct
Cyc_Dict_Dict**dict;{void*_tmpC5=(void*)f->sc;_LL55: if((int)_tmpC5 != 0)goto _LL57;
_LL56: return;_LL57: if((int)_tmpC5 != 3)goto _LL59;_LL58: dict=& i->imports;goto _LL54;
_LL59: if((int)_tmpC5 != 2)goto _LL5B;_LL5A: dict=& i->exports;goto _LL54;_LL5B:;_LL5C:((
int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)(({const char*_tmpC6="add_xtunionfielddecl";
_tag_arr(_tmpC6,sizeof(char),_get_zero_arr_size(_tmpC6,21));}));_LL54:;}{struct
Cyc_Interface_Ienv*env;{struct _handler_cons _tmpC7;_push_handler(& _tmpC7);{int
_tmpC9=0;if(setjmp(_tmpC7.handler))_tmpC9=1;if(!_tmpC9){env=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(*dict,_tmpC1);;
_pop_handler();}else{void*_tmpC8=(void*)_exn_thrown;void*_tmpCB=_tmpC8;_LL5E: if(
_tmpCB != Cyc_Dict_Absent)goto _LL60;_LL5F: env=Cyc_Interface_new_ienv();*dict=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*
v))Cyc_Dict_insert)(*dict,_tmpC1,env);goto _LL5D;_LL60:;_LL61:(void)_throw(_tmpCB);
_LL5D:;}}}env->xtunionfielddecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct Cyc_Tcdecl_Xtunionfielddecl*v))Cyc_Dict_insert)(env->xtunionfielddecls,(*
f->name).f2,({struct Cyc_Tcdecl_Xtunionfielddecl*_tmpCC=_cycalloc(sizeof(*_tmpCC));
_tmpCC->base=d;_tmpCC->field=f;_tmpCC;}));}}}static void Cyc_Interface_extract_tuniondecl(
struct _tuple18*env,struct _tagged_arr*x,struct Cyc_Absyn_Tuniondecl**dp){struct
_tuple18 _tmpCE;struct Cyc_Interface_Ienv*_tmpCF;struct Cyc_Interface_Ienv*_tmpD0;
int _tmpD1;struct Cyc_Dict_Dict*_tmpD2;struct Cyc_Interface_Seen*_tmpD3;struct Cyc_Interface_I*
_tmpD4;struct _tuple18*_tmpCD=env;_tmpCE=*_tmpCD;_tmpCF=_tmpCE.f1;_tmpD0=_tmpCE.f2;
_tmpD1=_tmpCE.f3;_tmpD2=_tmpCE.f4;_tmpD3=_tmpCE.f5;_tmpD4=_tmpCE.f6;{struct Cyc_Absyn_Tuniondecl*
_tmpD5=*dp;void*_tmpD6=(void*)_tmpD5->sc;_LL63: if((int)_tmpD6 != 0)goto _LL65;
_LL64: if((!_tmpD5->is_xtunion?_tmpD1: 0)?_tmpD5->fields == 0: 0)Cyc_Interface_fields_err(({
const char*_tmpD7="static";_tag_arr(_tmpD7,sizeof(char),_get_zero_arr_size(_tmpD7,
7));}),({const char*_tmpD8="tunion";_tag_arr(_tmpD8,sizeof(char),
_get_zero_arr_size(_tmpD8,7));}),_tmpD5->name);goto _LL62;_LL65: if((int)_tmpD6 != 
1)goto _LL67;_LL66: if(_tmpD5->fields == 0){if(!_tmpD5->is_xtunion?_tmpD1: 0)Cyc_Interface_fields_err(({
const char*_tmpD9="abstract";_tag_arr(_tmpD9,sizeof(char),_get_zero_arr_size(
_tmpD9,9));}),({const char*_tmpDA="tunion";_tag_arr(_tmpDA,sizeof(char),
_get_zero_arr_size(_tmpDA,7));}),_tmpD5->name);}else{_tmpD5=({struct Cyc_Absyn_Tuniondecl*
_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=*_tmpD5;_tmpDB;});_tmpD5->fields=0;}
if(Cyc_Interface_check_public_tuniondecl(_tmpD2,_tmpD3,_tmpD5))_tmpD0->tuniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl*
v))Cyc_Dict_insert)(_tmpD0->tuniondecls,x,_tmpD5);goto _LL62;_LL67: if((int)_tmpD6
!= 2)goto _LL69;_LL68: _tmpD5=({struct Cyc_Absyn_Tuniondecl*_tmpDC=_cycalloc(
sizeof(*_tmpDC));_tmpDC[0]=*_tmpD5;_tmpDC;});if(!_tmpD5->is_xtunion?_tmpD5->fields
== 0: 0){Cyc_Interface_fields_err(({const char*_tmpDD="public";_tag_arr(_tmpDD,
sizeof(char),_get_zero_arr_size(_tmpDD,7));}),({const char*_tmpDE="tunion";
_tag_arr(_tmpDE,sizeof(char),_get_zero_arr_size(_tmpDE,7));}),_tmpD5->name);(
void*)(_tmpD5->sc=(void*)((void*)1));}if(Cyc_Interface_check_public_tuniondecl(
_tmpD2,_tmpD3,_tmpD5)){if(_tmpD5->is_xtunion?_tmpD5->fields != 0: 0){struct Cyc_List_List*
_tmpDF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpD5->fields))->v;
_tmpD5->fields=0;{struct Cyc_List_List*_tmpE0=_tmpDF;for(0;_tmpE0 != 0;_tmpE0=
_tmpE0->tl){Cyc_Interface_extract_xtunionfielddecl(_tmpD4,_tmpD5,(struct Cyc_Absyn_Tunionfield*)
_tmpE0->hd);}}}_tmpD0->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl*v))Cyc_Dict_insert)(_tmpD0->tuniondecls,
x,_tmpD5);}goto _LL62;_LL69: if((int)_tmpD6 != 4)goto _LL6B;_LL6A:((int(*)(struct
_tagged_arr s))Cyc_Interface_invalid_arg)(({const char*_tmpE1="extract_tuniondecl";
_tag_arr(_tmpE1,sizeof(char),_get_zero_arr_size(_tmpE1,19));}));_LL6B: if((int)
_tmpD6 != 3)goto _LL6D;_LL6C: if(Cyc_Interface_check_public_tuniondecl(_tmpD2,
_tmpD3,_tmpD5)){if(_tmpD5->is_xtunion?_tmpD5->fields != 0: 0){_tmpD5=({struct Cyc_Absyn_Tuniondecl*
_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2[0]=*_tmpD5;_tmpE2;});{struct Cyc_List_List*
_tmpE3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpD5->fields))->v;
_tmpD5->fields=0;{struct Cyc_List_List*_tmpE4=_tmpE3;for(0;_tmpE4 != 0;_tmpE4=
_tmpE4->tl){Cyc_Interface_extract_xtunionfielddecl(_tmpD4,_tmpD5,(struct Cyc_Absyn_Tunionfield*)
_tmpE4->hd);}}}}_tmpCF->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl*v))Cyc_Dict_insert)(_tmpCF->tuniondecls,
x,_tmpD5);}goto _LL62;_LL6D: if((int)_tmpD6 != 5)goto _LL62;_LL6E:((int(*)(struct
_tagged_arr s))Cyc_Interface_invalid_arg)(({const char*_tmpE5="add_tuniondecl";
_tag_arr(_tmpE5,sizeof(char),_get_zero_arr_size(_tmpE5,15));}));goto _LL62;_LL62:;}}
static void Cyc_Interface_extract_typedef(struct _tuple18*env,struct _tagged_arr*x,
struct Cyc_Absyn_Typedefdecl*d){struct _tuple18 _tmpE7;struct Cyc_Interface_Ienv*
_tmpE8;struct Cyc_Dict_Dict*_tmpE9;struct Cyc_Interface_Seen*_tmpEA;struct _tuple18*
_tmpE6=env;_tmpE7=*_tmpE6;_tmpE8=_tmpE7.f2;_tmpE9=_tmpE7.f4;_tmpEA=_tmpE7.f5;if(
Cyc_Interface_check_public_typedefdecl(_tmpE9,_tmpEA,d))_tmpE8->typedefdecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Typedefdecl*
v))Cyc_Dict_insert)(_tmpE8->typedefdecls,x,d);}static void Cyc_Interface_extract_ordinarie(
struct _tuple18*env,struct _tagged_arr*x,struct _tuple13*v){struct _tuple18 _tmpEC;
struct Cyc_Interface_Ienv*_tmpED;struct Cyc_Interface_Ienv*_tmpEE;int _tmpEF;struct
Cyc_Dict_Dict*_tmpF0;struct Cyc_Interface_Seen*_tmpF1;struct _tuple18*_tmpEB=env;
_tmpEC=*_tmpEB;_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;_tmpEF=_tmpEC.f3;_tmpF0=_tmpEC.f4;
_tmpF1=_tmpEC.f5;{void*_tmpF2=(*v).f1;void*_tmpF3=_tmpF2;void*_tmpF4;_LL70: if(*((
int*)_tmpF3)!= 0)goto _LL72;_tmpF4=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmpF3)->f1;
_LL71:{void*_tmpF5=_tmpF4;struct Cyc_Absyn_Fndecl*_tmpF6;struct Cyc_Absyn_Vardecl*
_tmpF7;_LL75: if(_tmpF5 <= (void*)1?1:*((int*)_tmpF5)!= 1)goto _LL77;_tmpF6=((
struct Cyc_Absyn_Funname_b_struct*)_tmpF5)->f1;_LL76: {struct Cyc_Absyn_Vardecl*
_tmpF8=({struct Cyc_Absyn_Vardecl*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->sc=(
void*)((void*)_tmpF6->sc);_tmpF9->name=_tmpF6->name;_tmpF9->tq=Cyc_Absyn_empty_tqual();
_tmpF9->type=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmpF6->cached_typ))->v);
_tmpF9->initializer=0;_tmpF9->rgn=0;_tmpF9->attributes=0;_tmpF9->escapes=0;
_tmpF9;});_tmpEF=0;_tmpF7=_tmpF8;goto _LL78;}_LL77: if(_tmpF5 <= (void*)1?1:*((int*)
_tmpF5)!= 0)goto _LL79;_tmpF7=((struct Cyc_Absyn_Global_b_struct*)_tmpF5)->f1;
_LL78: if(_tmpF7->initializer != 0){_tmpF7=({struct Cyc_Absyn_Vardecl*_tmpFA=
_cycalloc(sizeof(*_tmpFA));_tmpFA[0]=*_tmpF7;_tmpFA;});_tmpF7->initializer=0;}{
void*_tmpFB=(void*)_tmpF7->sc;_LL7C: if((int)_tmpFB != 0)goto _LL7E;_LL7D: if(_tmpEF?
Cyc_Tcutil_is_function_type((void*)_tmpF7->type): 0)Cyc_Interface_body_err(({
const char*_tmpFC="static";_tag_arr(_tmpFC,sizeof(char),_get_zero_arr_size(_tmpFC,
7));}),_tmpF7->name);goto _LL7B;_LL7E: if((int)_tmpFB != 5)goto _LL80;_LL7F: goto
_LL81;_LL80: if((int)_tmpFB != 1)goto _LL82;_LL81:((int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)(({
const char*_tmpFD="extract_ordinarie";_tag_arr(_tmpFD,sizeof(char),
_get_zero_arr_size(_tmpFD,18));}));_LL82: if((int)_tmpFB != 2)goto _LL84;_LL83: if(
_tmpEF?Cyc_Tcutil_is_function_type((void*)_tmpF7->type): 0)Cyc_Interface_body_err(({
const char*_tmpFE="public";_tag_arr(_tmpFE,sizeof(char),_get_zero_arr_size(_tmpFE,
7));}),_tmpF7->name);if(Cyc_Interface_check_public_vardecl(_tmpF0,_tmpF1,_tmpF7))
_tmpEE->vardecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpEE->vardecls,x,
_tmpF7);goto _LL7B;_LL84: if((int)_tmpFB != 4)goto _LL86;_LL85: goto _LL87;_LL86: if((
int)_tmpFB != 3)goto _LL7B;_LL87: if(Cyc_Interface_check_public_vardecl(_tmpF0,
_tmpF1,_tmpF7))_tmpED->vardecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpED->vardecls,
x,_tmpF7);goto _LL7B;_LL7B:;}goto _LL74;_LL79:;_LL7A: goto _LL74;_LL74:;}goto _LL6F;
_LL72:;_LL73: goto _LL6F;_LL6F:;}}struct _tuple19{struct Cyc_Interface_I*f1;int f2;
struct Cyc_Dict_Dict*f3;struct Cyc_Interface_Seen*f4;};static void Cyc_Interface_extract_f(
struct _tuple19*env_f,struct Cyc_List_List*ns,struct Cyc_Tcenv_Genv*ge){struct
_tuple19 _tmp100;struct Cyc_Interface_I*_tmp101;int _tmp102;struct Cyc_Dict_Dict*
_tmp103;struct Cyc_Interface_Seen*_tmp104;struct _tuple19*_tmpFF=env_f;_tmp100=*
_tmpFF;_tmp101=_tmp100.f1;_tmp102=_tmp100.f2;_tmp103=_tmp100.f3;_tmp104=_tmp100.f4;{
struct _tuple18 _tmp105=({struct _tuple18 _tmp106;_tmp106.f1=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp101->imports,
ns);_tmp106.f2=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(_tmp101->exports,ns);_tmp106.f3=_tmp102;_tmp106.f4=_tmp103;
_tmp106.f5=_tmp104;_tmp106.f6=_tmp101;_tmp106;});((void(*)(void(*f)(struct
_tuple18*,struct _tagged_arr*,struct Cyc_Absyn_Aggrdecl**),struct _tuple18*env,
struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_extract_aggrdecl,& _tmp105,
ge->aggrdecls);((void(*)(void(*f)(struct _tuple18*,struct _tagged_arr*,struct Cyc_Absyn_Tuniondecl**),
struct _tuple18*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_extract_tuniondecl,&
_tmp105,ge->tuniondecls);((void(*)(void(*f)(struct _tuple18*,struct _tagged_arr*,
struct Cyc_Absyn_Enumdecl**),struct _tuple18*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_extract_enumdecl,& _tmp105,ge->enumdecls);((void(*)(void(*f)(struct
_tuple18*,struct _tagged_arr*,struct Cyc_Absyn_Typedefdecl*),struct _tuple18*env,
struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_extract_typedef,& _tmp105,
ge->typedefs);((void(*)(void(*f)(struct _tuple18*,struct _tagged_arr*,struct
_tuple13*),struct _tuple18*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_extract_ordinarie,&
_tmp105,ge->ordinaries);}}static struct Cyc_Interface_I*Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict*ae,int check_complete_defs){struct _tuple19 _tmp107=({struct
_tuple19 _tmp108;_tmp108.f1=((struct Cyc_Interface_I*(*)(struct Cyc_Dict_Dict*skel))
Cyc_Interface_skel2i)(ae);_tmp108.f2=check_complete_defs;_tmp108.f3=ae;_tmp108.f4=
Cyc_Interface_new_seen();_tmp108;});((void(*)(void(*f)(struct _tuple19*,struct Cyc_List_List*,
struct Cyc_Tcenv_Genv*),struct _tuple19*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_extract_f,& _tmp107,ae);return _tmp107.f1;}struct Cyc_Interface_I*Cyc_Interface_extract(
struct Cyc_Dict_Dict*ae){return Cyc_Interface_gen_extract(ae,1);}inline static void
Cyc_Interface_check_err(struct _tagged_arr*msg1,struct _tagged_arr msg2){({struct
Cyc_String_pa_struct _tmp10B;_tmp10B.tag=0;_tmp10B.f1=(struct _tagged_arr)((struct
_tagged_arr)msg2);{void*_tmp109[1]={& _tmp10B};Cyc_Tcdecl_merr(0,msg1,({const char*
_tmp10A="%s";_tag_arr(_tmp10A,sizeof(char),_get_zero_arr_size(_tmp10A,3));}),
_tag_arr(_tmp109,sizeof(void*),1));}});}struct _tuple20{int f1;struct Cyc_Dict_Dict*
f2;int(*f3)(void*,void*,struct _tagged_arr*);struct _tagged_arr f4;struct
_tagged_arr*f5;};static void Cyc_Interface_incl_dict_f(struct _tuple20*env,struct
_tagged_arr*x,void*y1){struct _tuple20 _tmp10D;int _tmp10E;int*_tmp10F;struct Cyc_Dict_Dict*
_tmp110;int(*_tmp111)(void*,void*,struct _tagged_arr*);struct _tagged_arr _tmp112;
struct _tagged_arr*_tmp113;struct _tuple20*_tmp10C=env;_tmp10D=*_tmp10C;_tmp10E=
_tmp10D.f1;_tmp10F=(int*)&(*_tmp10C).f1;_tmp110=_tmp10D.f2;_tmp111=_tmp10D.f3;
_tmp112=_tmp10D.f4;_tmp113=_tmp10D.f5;{struct _handler_cons _tmp114;_push_handler(&
_tmp114);{int _tmp116=0;if(setjmp(_tmp114.handler))_tmp116=1;if(!_tmp116){{void*
_tmp117=((void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp110,x);if(!_tmp111(y1,_tmp117,_tmp113))*_tmp10F=0;};_pop_handler();}else{
void*_tmp115=(void*)_exn_thrown;void*_tmp119=_tmp115;_LL89: if(_tmp119 != Cyc_Dict_Absent)
goto _LL8B;_LL8A: Cyc_Interface_check_err(_tmp113,(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp11D;_tmp11D.tag=0;_tmp11D.f1=(struct _tagged_arr)((struct _tagged_arr)*x);{
struct Cyc_String_pa_struct _tmp11C;_tmp11C.tag=0;_tmp11C.f1=(struct _tagged_arr)((
struct _tagged_arr)_tmp112);{void*_tmp11A[2]={& _tmp11C,& _tmp11D};Cyc_aprintf(({
const char*_tmp11B="%s %s is missing";_tag_arr(_tmp11B,sizeof(char),
_get_zero_arr_size(_tmp11B,17));}),_tag_arr(_tmp11A,sizeof(void*),2));}}}));*
_tmp10F=0;goto _LL88;_LL8B:;_LL8C:(void)_throw(_tmp119);_LL88:;}}}}static int Cyc_Interface_incl_dict(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,int(*incl_f)(void*,void*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg){struct _tuple20
_tmp11E=({struct _tuple20 _tmp11F;_tmp11F.f1=1;_tmp11F.f2=dic2;_tmp11F.f3=incl_f;
_tmp11F.f4=t;_tmp11F.f5=msg;_tmp11F;});((void(*)(void(*f)(struct _tuple20*,struct
_tagged_arr*,void*),struct _tuple20*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_incl_dict_f,& _tmp11E,dic1);return _tmp11E.f1;}static int Cyc_Interface_incl_aggrdecl(
struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct _tagged_arr*msg){
struct Cyc_Absyn_Aggrdecl*_tmp120=Cyc_Tcdecl_merge_aggrdecl(d0,d1,0,msg);if(
_tmp120 == 0)return 0;if((struct Cyc_Absyn_Aggrdecl*)d0 != _tmp120){Cyc_Interface_check_err(
msg,(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp123;_tmp123.tag=0;_tmp123.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(d1->name));{void*
_tmp121[1]={& _tmp123};Cyc_aprintf(({const char*_tmp122="declaration of type %s discloses too much information";
_tag_arr(_tmp122,sizeof(char),_get_zero_arr_size(_tmp122,54));}),_tag_arr(
_tmp121,sizeof(void*),1));}}));return 0;}return 1;}static int Cyc_Interface_incl_tuniondecl(
struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*d1,struct _tagged_arr*
msg){struct Cyc_Absyn_Tuniondecl*_tmp124=Cyc_Tcdecl_merge_tuniondecl(d0,d1,0,msg);
if(_tmp124 == 0)return 0;if((struct Cyc_Absyn_Tuniondecl*)d0 != _tmp124){Cyc_Interface_check_err(
msg,(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp127;_tmp127.tag=0;_tmp127.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(d1->name));{void*
_tmp125[1]={& _tmp127};Cyc_aprintf(({const char*_tmp126="declaration of tunion %s discloses too much information";
_tag_arr(_tmp126,sizeof(char),_get_zero_arr_size(_tmp126,56));}),_tag_arr(
_tmp125,sizeof(void*),1));}}));return 0;}return 1;}static int Cyc_Interface_incl_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct _tagged_arr*msg){
struct Cyc_Absyn_Enumdecl*_tmp128=Cyc_Tcdecl_merge_enumdecl(d0,d1,0,msg);if(
_tmp128 == 0)return 0;if((struct Cyc_Absyn_Enumdecl*)d0 != _tmp128){Cyc_Interface_check_err(
msg,(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp12B;_tmp12B.tag=0;_tmp12B.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(d1->name));{void*
_tmp129[1]={& _tmp12B};Cyc_aprintf(({const char*_tmp12A="declaration of enum %s discloses too much information";
_tag_arr(_tmp12A,sizeof(char),_get_zero_arr_size(_tmp12A,54));}),_tag_arr(
_tmp129,sizeof(void*),1));}}));return 0;}return 1;}static int Cyc_Interface_incl_vardecl(
struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct _tagged_arr*msg){
struct Cyc_Absyn_Vardecl*_tmp12C=Cyc_Tcdecl_merge_vardecl(d0,d1,0,msg);if(_tmp12C
== 0)return 0;if((struct Cyc_Absyn_Vardecl*)d0 != _tmp12C){Cyc_Interface_check_err(
msg,(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp12F;_tmp12F.tag=0;_tmp12F.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(d1->name));{void*
_tmp12D[1]={& _tmp12F};Cyc_aprintf(({const char*_tmp12E="declaration of variable %s discloses too much information";
_tag_arr(_tmp12E,sizeof(char),_get_zero_arr_size(_tmp12E,58));}),_tag_arr(
_tmp12D,sizeof(void*),1));}}));return 0;}return 1;}static int Cyc_Interface_incl_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct _tagged_arr*
msg){struct Cyc_Absyn_Typedefdecl*_tmp130=Cyc_Tcdecl_merge_typedefdecl(d0,d1,0,
msg);if(_tmp130 == 0)return 0;if((struct Cyc_Absyn_Typedefdecl*)d0 != _tmp130){Cyc_Interface_check_err(
msg,(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp133;_tmp133.tag=0;_tmp133.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(d1->name));{void*
_tmp131[1]={& _tmp133};Cyc_aprintf(({const char*_tmp132="declaration of typedef %s discloses too much information";
_tag_arr(_tmp132,sizeof(char),_get_zero_arr_size(_tmp132,57));}),_tag_arr(
_tmp131,sizeof(void*),1));}}));return 0;}return 1;}static int Cyc_Interface_incl_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl*d0,struct Cyc_Tcdecl_Xtunionfielddecl*d1,struct
_tagged_arr*msg){struct Cyc_Tcdecl_Xtunionfielddecl*_tmp134=Cyc_Tcdecl_merge_xtunionfielddecl(
d0,d1,0,msg);if(_tmp134 == 0)return 0;if((struct Cyc_Tcdecl_Xtunionfielddecl*)d0 != 
_tmp134){Cyc_Interface_check_err(msg,(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp137;_tmp137.tag=0;_tmp137.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string((
d1->field)->name));{void*_tmp135[1]={& _tmp137};Cyc_aprintf(({const char*_tmp136="declaration of xtunionfield %s discloses too much information";
_tag_arr(_tmp136,sizeof(char),_get_zero_arr_size(_tmp136,62));}),_tag_arr(
_tmp135,sizeof(void*),1));}}));return 0;}return 1;}struct Cyc_Core_Opt*Cyc_Interface_compat_merge_dict(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,void*(*
merge_f)(void*,void*,struct Cyc_Position_Segment*,struct _tagged_arr*),struct
_tagged_arr t,struct _tagged_arr*msg);static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*
ie1,struct Cyc_Interface_Ienv*ie2,struct _tagged_arr*msg){int _tmp138=((int(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Absyn_Aggrdecl*,
struct Cyc_Absyn_Aggrdecl*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_incl_dict)(ie1->aggrdecls,ie2->aggrdecls,Cyc_Interface_incl_aggrdecl,({
const char*_tmp143="type";_tag_arr(_tmp143,sizeof(char),_get_zero_arr_size(
_tmp143,5));}),msg);int _tmp139=((int(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,int(*incl_f)(struct Cyc_Absyn_Tuniondecl*,struct Cyc_Absyn_Tuniondecl*,struct
_tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_incl_dict)(
ie1->tuniondecls,ie2->tuniondecls,Cyc_Interface_incl_tuniondecl,({const char*
_tmp142="tunion";_tag_arr(_tmp142,sizeof(char),_get_zero_arr_size(_tmp142,7));}),
msg);int _tmp13A=((int(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,int(*
incl_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct _tagged_arr*),
struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_incl_dict)(ie1->enumdecls,
ie2->enumdecls,Cyc_Interface_incl_enumdecl,({const char*_tmp141="enum";_tag_arr(
_tmp141,sizeof(char),_get_zero_arr_size(_tmp141,5));}),msg);int _tmp13B=((struct
Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*
excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct
Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,struct _tagged_arr*),struct
_tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,
ie2->typedefdecls,((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,({
const char*_tmp140="typedef";_tag_arr(_tmp140,sizeof(char),_get_zero_arr_size(
_tmp140,8));}),msg)!= 0;int _tmp13C=((int(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,int(*incl_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct
_tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_incl_dict)(
ie1->vardecls,ie2->vardecls,Cyc_Interface_incl_vardecl,({const char*_tmp13F="variable";
_tag_arr(_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,9));}),msg);int _tmp13D=((
int(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Tcdecl_Xtunionfielddecl*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_incl_dict)(ie1->xtunionfielddecls,ie2->xtunionfielddecls,
Cyc_Interface_incl_xtunionfielddecl,({const char*_tmp13E="xtunionfield";_tag_arr(
_tmp13E,sizeof(char),_get_zero_arr_size(_tmp13E,13));}),msg);return((((_tmp138?
_tmp139: 0)?_tmp13A: 0)?_tmp13B: 0)?_tmp13C: 0)?_tmp13D: 0;}struct _tuple21{int f1;
struct Cyc_Dict_Dict*f2;struct _tagged_arr*f3;};static void Cyc_Interface_incl_ns_f(
struct _tuple21*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie1){struct
_tuple21 _tmp145;int _tmp146;int*_tmp147;struct Cyc_Dict_Dict*_tmp148;struct
_tagged_arr*_tmp149;struct _tuple21*_tmp144=env;_tmp145=*_tmp144;_tmp146=_tmp145.f1;
_tmp147=(int*)&(*_tmp144).f1;_tmp148=_tmp145.f2;_tmp149=_tmp145.f3;{struct Cyc_Interface_Ienv*
ie2;{struct _handler_cons _tmp14A;_push_handler(& _tmp14A);{int _tmp14C=0;if(setjmp(
_tmp14A.handler))_tmp14C=1;if(!_tmp14C){ie2=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp148,ns);;
_pop_handler();}else{void*_tmp14B=(void*)_exn_thrown;void*_tmp14E=_tmp14B;_LL8E:
if(_tmp14E != Cyc_Dict_Absent)goto _LL90;_LL8F: ie2=Cyc_Interface_lazy_new_ienv();
goto _LL8D;_LL90:;_LL91:(void)_throw(_tmp14E);_LL8D:;}}}if(!Cyc_Interface_incl_ienv(
ie1,ie2,_tmp149))*_tmp147=0;}}static int Cyc_Interface_incl_ns(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct _tagged_arr*msg){struct _tuple21 _tmp14F=({
struct _tuple21 _tmp150;_tmp150.f1=1;_tmp150.f2=dic2;_tmp150.f3=msg;_tmp150;});((
void(*)(void(*f)(struct _tuple21*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct _tuple21*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_incl_ns_f,&
_tmp14F,dic1);return _tmp14F.f1;}int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*
i1,struct Cyc_Interface_I*i2,struct _tuple4*info){struct _handler_cons _tmp151;
_push_handler(& _tmp151);{int _tmp153=0;if(setjmp(_tmp151.handler))_tmp153=1;if(!
_tmp153){{int _tmp154=1;struct _tagged_arr*msg=0;if(info != 0)msg=({struct
_tagged_arr*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155[0]=(struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp159;_tmp159.tag=0;_tmp159.f1=(struct _tagged_arr)((
struct _tagged_arr)(*info).f2);{struct Cyc_String_pa_struct _tmp158;_tmp158.tag=0;
_tmp158.f1=(struct _tagged_arr)((struct _tagged_arr)(*info).f1);{void*_tmp156[2]={&
_tmp158,& _tmp159};Cyc_aprintf(({const char*_tmp157="checking inclusion of %s exports into %s exports,";
_tag_arr(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,50));}),_tag_arr(
_tmp156,sizeof(void*),2));}}});_tmp155;});if(!Cyc_Interface_incl_ns(i1->exports,
i2->exports,msg))_tmp154=0;if(info != 0)msg=({struct _tagged_arr*_tmp15A=_cycalloc(
sizeof(*_tmp15A));_tmp15A[0]=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp15E;_tmp15E.tag=0;_tmp15E.f1=(struct _tagged_arr)((struct _tagged_arr)(*info).f1);{
struct Cyc_String_pa_struct _tmp15D;_tmp15D.tag=0;_tmp15D.f1=(struct _tagged_arr)((
struct _tagged_arr)(*info).f2);{void*_tmp15B[2]={& _tmp15D,& _tmp15E};Cyc_aprintf(({
const char*_tmp15C="checking inclusion of %s imports into %s imports,";_tag_arr(
_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,50));}),_tag_arr(_tmp15B,sizeof(
void*),2));}}});_tmp15A;});if(!Cyc_Interface_incl_ns(i2->imports,i1->imports,msg))
_tmp154=0;{int _tmp15F=_tmp154;_npop_handler(0);return _tmp15F;}};_pop_handler();}
else{void*_tmp152=(void*)_exn_thrown;void*_tmp161=_tmp152;_LL93: if(_tmp161 != Cyc_Tcdecl_Incompatible)
goto _LL95;_LL94: return 0;_LL95:;_LL96:(void)_throw(_tmp161);_LL92:;}}}struct
_tuple22{int f1;struct Cyc_Dict_Dict*f2;struct Cyc_Dict_Dict*f3;struct Cyc_Dict_Dict*
f4;void*(*f5)(void*,void*,struct Cyc_Position_Segment*,struct _tagged_arr*);struct
_tagged_arr f6;struct _tagged_arr*f7;};void Cyc_Interface_compat_merge_dict_f(
struct _tuple22*env,struct _tagged_arr*x,void*y2){struct _tuple22 _tmp163;int _tmp164;
int*_tmp165;struct Cyc_Dict_Dict*_tmp166;struct Cyc_Dict_Dict**_tmp167;struct Cyc_Dict_Dict*
_tmp168;struct Cyc_Dict_Dict*_tmp169;void*(*_tmp16A)(void*,void*,struct Cyc_Position_Segment*,
struct _tagged_arr*);struct _tagged_arr _tmp16B;struct _tagged_arr*_tmp16C;struct
_tuple22*_tmp162=env;_tmp163=*_tmp162;_tmp164=_tmp163.f1;_tmp165=(int*)&(*
_tmp162).f1;_tmp166=_tmp163.f2;_tmp167=(struct Cyc_Dict_Dict**)&(*_tmp162).f2;
_tmp168=_tmp163.f3;_tmp169=_tmp163.f4;_tmp16A=_tmp163.f5;_tmp16B=_tmp163.f6;
_tmp16C=_tmp163.f7;{void*y;{struct _handler_cons _tmp16D;_push_handler(& _tmp16D);{
int _tmp16F=0;if(setjmp(_tmp16D.handler))_tmp16F=1;if(!_tmp16F){{void*_tmp170=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp168,x);
void*_tmp171=_tmp16A(_tmp170,y2,0,_tmp16C);if(!((unsigned int)_tmp171)){*_tmp165=
0;_npop_handler(0);return;}y=(void*)_tmp171;};_pop_handler();}else{void*_tmp16E=(
void*)_exn_thrown;void*_tmp173=_tmp16E;_LL98: if(_tmp173 != Cyc_Dict_Absent)goto
_LL9A;_LL99: y=y2;goto _LL97;_LL9A:;_LL9B:(void)_throw(_tmp173);_LL97:;}}}{struct
_handler_cons _tmp174;_push_handler(& _tmp174);{int _tmp176=0;if(setjmp(_tmp174.handler))
_tmp176=1;if(!_tmp176){{void*_tmp177=((void*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_lookup)(_tmp169,x);void*_tmp178=_tmp16A(_tmp177,y,0,
_tmp16C);if(_tmp178 != (void*)_tmp177){if((unsigned int)_tmp178)Cyc_Interface_check_err(
_tmp16C,(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp17C;_tmp17C.tag=0;
_tmp17C.f1=(struct _tagged_arr)((struct _tagged_arr)*x);{struct Cyc_String_pa_struct
_tmp17B;_tmp17B.tag=0;_tmp17B.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp16B);{
void*_tmp179[2]={& _tmp17B,& _tmp17C};Cyc_aprintf(({const char*_tmp17A="abstract %s %s is being imported as non-abstract";
_tag_arr(_tmp17A,sizeof(char),_get_zero_arr_size(_tmp17A,49));}),_tag_arr(
_tmp179,sizeof(void*),2));}}}));*_tmp165=0;}};_pop_handler();}else{void*_tmp175=(
void*)_exn_thrown;void*_tmp17E=_tmp175;_LL9D: if(_tmp17E != Cyc_Dict_Absent)goto
_LL9F;_LL9E: if(*_tmp165)*_tmp167=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(*_tmp167,x,y);goto _LL9C;_LL9F:;
_LLA0:(void)_throw(_tmp17E);_LL9C:;}}}}}struct Cyc_Core_Opt*Cyc_Interface_compat_merge_dict(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,void*(*
merge_f)(void*,void*,struct Cyc_Position_Segment*,struct _tagged_arr*),struct
_tagged_arr t,struct _tagged_arr*msg){struct _tuple22 _tmp17F=({struct _tuple22
_tmp181;_tmp181.f1=1;_tmp181.f2=dic1;_tmp181.f3=dic1;_tmp181.f4=excl;_tmp181.f5=
merge_f;_tmp181.f6=t;_tmp181.f7=msg;_tmp181;});((void(*)(void(*f)(struct _tuple22*,
struct _tagged_arr*,void*),struct _tuple22*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_compat_merge_dict_f,& _tmp17F,dic2);if(_tmp17F.f1)return({struct Cyc_Core_Opt*
_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->v=_tmp17F.f2;_tmp180;});return 0;}
struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*
ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct
_tagged_arr*msg){struct Cyc_Core_Opt*_tmp182=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Aggrdecl*(*
merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_Position_Segment*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(
ie1->aggrdecls,ie2->aggrdecls,iexcl->aggrdecls,Cyc_Tcdecl_merge_aggrdecl,({const
char*_tmp18E="type";_tag_arr(_tmp18E,sizeof(char),_get_zero_arr_size(_tmp18E,5));}),
msg);struct Cyc_Core_Opt*_tmp183=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Tuniondecl*(*
merge_f)(struct Cyc_Absyn_Tuniondecl*,struct Cyc_Absyn_Tuniondecl*,struct Cyc_Position_Segment*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(
ie1->tuniondecls,ie2->tuniondecls,iexcl->tuniondecls,Cyc_Tcdecl_merge_tuniondecl,({
const char*_tmp18D="tunion";_tag_arr(_tmp18D,sizeof(char),_get_zero_arr_size(
_tmp18D,7));}),msg);struct Cyc_Core_Opt*_tmp184=((struct Cyc_Core_Opt*(*)(struct
Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Enumdecl*(*
merge_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct Cyc_Position_Segment*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(
ie1->enumdecls,ie2->enumdecls,iexcl->enumdecls,Cyc_Tcdecl_merge_enumdecl,({const
char*_tmp18C="enum";_tag_arr(_tmp18C,sizeof(char),_get_zero_arr_size(_tmp18C,5));}),
msg);struct Cyc_Core_Opt*_tmp185=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Typedefdecl*(*
merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,iexcl->typedefdecls,Cyc_Tcdecl_merge_typedefdecl,({
const char*_tmp18B="typedef";_tag_arr(_tmp18B,sizeof(char),_get_zero_arr_size(
_tmp18B,8));}),msg);struct Cyc_Core_Opt*_tmp186=((struct Cyc_Core_Opt*(*)(struct
Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Vardecl*(*
merge_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(
ie1->vardecls,ie2->vardecls,iexcl->vardecls,Cyc_Tcdecl_merge_vardecl,({const char*
_tmp18A="variable";_tag_arr(_tmp18A,sizeof(char),_get_zero_arr_size(_tmp18A,9));}),
msg);struct Cyc_Core_Opt*_tmp187=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Tcdecl_Xtunionfielddecl*(*
merge_f)(struct Cyc_Tcdecl_Xtunionfielddecl*,struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Position_Segment*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->xtunionfielddecls,ie2->xtunionfielddecls,
iexcl->xtunionfielddecls,Cyc_Tcdecl_merge_xtunionfielddecl,({const char*_tmp189="xtunionfield";
_tag_arr(_tmp189,sizeof(char),_get_zero_arr_size(_tmp189,13));}),msg);if(((((!((
unsigned int)_tmp182)?1: !((unsigned int)_tmp183))?1: !((unsigned int)_tmp184))?1:
!((unsigned int)_tmp185))?1: !((unsigned int)_tmp186))?1: !((unsigned int)_tmp187))
return 0;return({struct Cyc_Interface_Ienv*_tmp188=_cycalloc(sizeof(*_tmp188));
_tmp188->aggrdecls=(struct Cyc_Dict_Dict*)_tmp182->v;_tmp188->tuniondecls=(struct
Cyc_Dict_Dict*)_tmp183->v;_tmp188->enumdecls=(struct Cyc_Dict_Dict*)_tmp184->v;
_tmp188->typedefdecls=(struct Cyc_Dict_Dict*)_tmp185->v;_tmp188->vardecls=(struct
Cyc_Dict_Dict*)_tmp186->v;_tmp188->xtunionfielddecls=(struct Cyc_Dict_Dict*)
_tmp187->v;_tmp188;});}struct _tuple23{int f1;struct Cyc_Dict_Dict*f2;struct Cyc_Dict_Dict*
f3;struct Cyc_Dict_Dict*f4;struct _tagged_arr*f5;};void Cyc_Interface_compat_merge_ns_f(
struct _tuple23*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){struct
_tuple23 _tmp190;int _tmp191;int*_tmp192;struct Cyc_Dict_Dict*_tmp193;struct Cyc_Dict_Dict**
_tmp194;struct Cyc_Dict_Dict*_tmp195;struct Cyc_Dict_Dict*_tmp196;struct
_tagged_arr*_tmp197;struct _tuple23*_tmp18F=env;_tmp190=*_tmp18F;_tmp191=_tmp190.f1;
_tmp192=(int*)&(*_tmp18F).f1;_tmp193=_tmp190.f2;_tmp194=(struct Cyc_Dict_Dict**)&(*
_tmp18F).f2;_tmp195=_tmp190.f3;_tmp196=_tmp190.f4;_tmp197=_tmp190.f5;{struct Cyc_Interface_Ienv*
ie1;struct Cyc_Interface_Ienv*iexcl;{struct _handler_cons _tmp198;_push_handler(&
_tmp198);{int _tmp19A=0;if(setjmp(_tmp198.handler))_tmp19A=1;if(!_tmp19A){iexcl=((
struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
_tmp196,ns);;_pop_handler();}else{void*_tmp199=(void*)_exn_thrown;void*_tmp19C=
_tmp199;_LLA2: if(_tmp19C != Cyc_Dict_Absent)goto _LLA4;_LLA3: iexcl=Cyc_Interface_lazy_new_ienv();
goto _LLA1;_LLA4:;_LLA5:(void)_throw(_tmp19C);_LLA1:;}}}{struct _handler_cons
_tmp19D;_push_handler(& _tmp19D);{int _tmp19F=0;if(setjmp(_tmp19D.handler))_tmp19F=
1;if(!_tmp19F){ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(_tmp195,ns);{struct Cyc_Interface_Ienv*_tmp1A0=
Cyc_Interface_compat_merge_ienv(Cyc_Interface_lazy_new_ienv(),ie1,iexcl,_tmp197);
if(_tmp1A0 == 0){*_tmp192=0;_npop_handler(0);return;}ie1=(struct Cyc_Interface_Ienv*)
_tmp1A0;};_pop_handler();}else{void*_tmp19E=(void*)_exn_thrown;void*_tmp1A2=
_tmp19E;_LLA7: if(_tmp1A2 != Cyc_Dict_Absent)goto _LLA9;_LLA8: ie1=Cyc_Interface_lazy_new_ienv();
goto _LLA6;_LLA9:;_LLAA:(void)_throw(_tmp1A2);_LLA6:;}}}{struct Cyc_Interface_Ienv*
_tmp1A3=Cyc_Interface_compat_merge_ienv(ie1,ie2,iexcl,_tmp197);if(_tmp1A3 == 0)*
_tmp192=0;else{if(*_tmp192)*_tmp194=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*_tmp194,ns,(
struct Cyc_Interface_Ienv*)_tmp1A3);}}}}struct Cyc_Core_Opt*Cyc_Interface_compat_merge_ns(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,
struct _tagged_arr*msg){struct Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp);
struct _tuple23 _tmp1A4=({struct _tuple23 _tmp1A6;_tmp1A6.f1=1;_tmp1A6.f2=d;_tmp1A6.f3=
dic1;_tmp1A6.f4=excl;_tmp1A6.f5=msg;_tmp1A6;});((void(*)(void(*f)(struct _tuple23*,
struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct _tuple23*env,struct Cyc_Dict_Dict*
d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_ns_f,& _tmp1A4,dic2);if(_tmp1A4.f1)
return({struct Cyc_Core_Opt*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5->v=
_tmp1A4.f2;_tmp1A5;});return 0;}struct _tuple24{int f1;struct Cyc_Dict_Dict*f2;
struct Cyc_Dict_Dict*f3;struct _tagged_arr f4;struct _tagged_arr*f5;};void Cyc_Interface_disj_merge_dict_f(
struct _tuple24*env,struct _tagged_arr*x,void*y){struct _tuple24 _tmp1A8;int _tmp1A9;
int*_tmp1AA;struct Cyc_Dict_Dict*_tmp1AB;struct Cyc_Dict_Dict**_tmp1AC;struct Cyc_Dict_Dict*
_tmp1AD;struct _tagged_arr _tmp1AE;struct _tagged_arr*_tmp1AF;struct _tuple24*
_tmp1A7=env;_tmp1A8=*_tmp1A7;_tmp1A9=_tmp1A8.f1;_tmp1AA=(int*)&(*_tmp1A7).f1;
_tmp1AB=_tmp1A8.f2;_tmp1AC=(struct Cyc_Dict_Dict**)&(*_tmp1A7).f2;_tmp1AD=_tmp1A8.f3;
_tmp1AE=_tmp1A8.f4;_tmp1AF=_tmp1A8.f5;if(((int(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_member)(_tmp1AD,x)){Cyc_Interface_check_err(_tmp1AF,(
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp1B3;_tmp1B3.tag=0;_tmp1B3.f1=(
struct _tagged_arr)((struct _tagged_arr)*x);{struct Cyc_String_pa_struct _tmp1B2;
_tmp1B2.tag=0;_tmp1B2.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp1AE);{void*
_tmp1B0[2]={& _tmp1B2,& _tmp1B3};Cyc_aprintf(({const char*_tmp1B1="%s %s is exported more than once";
_tag_arr(_tmp1B1,sizeof(char),_get_zero_arr_size(_tmp1B1,33));}),_tag_arr(
_tmp1B0,sizeof(void*),2));}}}));*_tmp1AA=0;}else{if(*_tmp1AA)*_tmp1AC=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(*
_tmp1AC,x,y);}}struct Cyc_Core_Opt*Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct _tagged_arr t,struct _tagged_arr*msg){struct
_tuple24 _tmp1B4=({struct _tuple24 _tmp1B6;_tmp1B6.f1=1;_tmp1B6.f2=dic1;_tmp1B6.f3=
dic1;_tmp1B6.f4=t;_tmp1B6.f5=msg;_tmp1B6;});((void(*)(void(*f)(struct _tuple24*,
struct _tagged_arr*,void*),struct _tuple24*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_disj_merge_dict_f,& _tmp1B4,dic2);if(_tmp1B4.f1)return({struct Cyc_Core_Opt*
_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5->v=_tmp1B4.f2;_tmp1B5;});return 0;}
struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*
ie1,struct Cyc_Interface_Ienv*ie2,struct _tagged_arr*msg){struct Cyc_Core_Opt*
_tmp1B7=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,
struct Cyc_Absyn_Aggrdecl*,struct Cyc_Position_Segment*,struct _tagged_arr*),struct
_tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,
ie2->aggrdecls,((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_aggrdecl,({const
char*_tmp1C3="type";_tag_arr(_tmp1C3,sizeof(char),_get_zero_arr_size(_tmp1C3,5));}),
msg);struct Cyc_Core_Opt*_tmp1B8=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_disj_merge_dict)(
ie1->tuniondecls,ie2->tuniondecls,({const char*_tmp1C2="[x]tunion";_tag_arr(
_tmp1C2,sizeof(char),_get_zero_arr_size(_tmp1C2,10));}),msg);struct Cyc_Core_Opt*
_tmp1B9=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_disj_merge_dict)(
ie1->enumdecls,ie2->enumdecls,({const char*_tmp1C1="enum";_tag_arr(_tmp1C1,
sizeof(char),_get_zero_arr_size(_tmp1C1,5));}),msg);struct Cyc_Core_Opt*_tmp1BA=((
struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct
Cyc_Dict_Dict*excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,struct _tagged_arr*),
struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,
ie2->typedefdecls,((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,({
const char*_tmp1C0="typedef";_tag_arr(_tmp1C0,sizeof(char),_get_zero_arr_size(
_tmp1C0,8));}),msg);struct Cyc_Core_Opt*_tmp1BB=((struct Cyc_Core_Opt*(*)(struct
Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct _tagged_arr t,struct _tagged_arr*
msg))Cyc_Interface_disj_merge_dict)(ie1->vardecls,ie2->vardecls,({const char*
_tmp1BF="variable";_tag_arr(_tmp1BF,sizeof(char),_get_zero_arr_size(_tmp1BF,9));}),
msg);struct Cyc_Core_Opt*_tmp1BC=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_disj_merge_dict)(
ie1->xtunionfielddecls,ie2->xtunionfielddecls,({const char*_tmp1BE="xtunionfield";
_tag_arr(_tmp1BE,sizeof(char),_get_zero_arr_size(_tmp1BE,13));}),msg);if(((((!((
unsigned int)_tmp1B7)?1: !((unsigned int)_tmp1B8))?1: !((unsigned int)_tmp1B9))?1:
!((unsigned int)_tmp1BA))?1: !((unsigned int)_tmp1BB))?1: !((unsigned int)_tmp1BC))
return 0;return({struct Cyc_Interface_Ienv*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));
_tmp1BD->aggrdecls=(struct Cyc_Dict_Dict*)_tmp1B7->v;_tmp1BD->tuniondecls=(struct
Cyc_Dict_Dict*)_tmp1B8->v;_tmp1BD->enumdecls=(struct Cyc_Dict_Dict*)_tmp1B9->v;
_tmp1BD->typedefdecls=(struct Cyc_Dict_Dict*)_tmp1BA->v;_tmp1BD->vardecls=(struct
Cyc_Dict_Dict*)_tmp1BB->v;_tmp1BD->xtunionfielddecls=(struct Cyc_Dict_Dict*)
_tmp1BC->v;_tmp1BD;});}struct _tuple25{int f1;struct Cyc_Dict_Dict*f2;struct Cyc_Dict_Dict*
f3;struct _tagged_arr*f4;};void Cyc_Interface_disj_merge_ns_f(struct _tuple25*env,
struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){struct _tuple25 _tmp1C5;int
_tmp1C6;int*_tmp1C7;struct Cyc_Dict_Dict*_tmp1C8;struct Cyc_Dict_Dict**_tmp1C9;
struct Cyc_Dict_Dict*_tmp1CA;struct _tagged_arr*_tmp1CB;struct _tuple25*_tmp1C4=env;
_tmp1C5=*_tmp1C4;_tmp1C6=_tmp1C5.f1;_tmp1C7=(int*)&(*_tmp1C4).f1;_tmp1C8=_tmp1C5.f2;
_tmp1C9=(struct Cyc_Dict_Dict**)&(*_tmp1C4).f2;_tmp1CA=_tmp1C5.f3;_tmp1CB=_tmp1C5.f4;{
struct Cyc_Interface_Ienv*ie1;{struct _handler_cons _tmp1CC;_push_handler(& _tmp1CC);{
int _tmp1CE=0;if(setjmp(_tmp1CC.handler))_tmp1CE=1;if(!_tmp1CE){ie1=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp1CA,ns);;
_pop_handler();}else{void*_tmp1CD=(void*)_exn_thrown;void*_tmp1D0=_tmp1CD;_LLAC:
if(_tmp1D0 != Cyc_Dict_Absent)goto _LLAE;_LLAD: ie1=Cyc_Interface_lazy_new_ienv();
goto _LLAB;_LLAE:;_LLAF:(void)_throw(_tmp1D0);_LLAB:;}}}{struct Cyc_Interface_Ienv*
_tmp1D1=Cyc_Interface_disj_merge_ienv(ie1,ie2,_tmp1CB);if(_tmp1D1 == 0)*_tmp1C7=0;
else{if(*_tmp1C7)*_tmp1C9=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*_tmp1C9,ns,(struct
Cyc_Interface_Ienv*)_tmp1D1);}}}}struct Cyc_Core_Opt*Cyc_Interface_disj_merge_ns(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct _tagged_arr*msg){struct
_tuple25 _tmp1D2=({struct _tuple25 _tmp1D4;_tmp1D4.f1=1;_tmp1D4.f2=dic1;_tmp1D4.f3=
dic1;_tmp1D4.f4=msg;_tmp1D4;});((void(*)(void(*f)(struct _tuple25*,struct Cyc_List_List*,
struct Cyc_Interface_Ienv*),struct _tuple25*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_disj_merge_ns_f,& _tmp1D2,dic2);if(_tmp1D2.f1)return({struct Cyc_Core_Opt*
_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->v=_tmp1D2.f2;_tmp1D3;});return 0;}
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*
i2,struct _tuple4*info){struct _handler_cons _tmp1D5;_push_handler(& _tmp1D5);{int
_tmp1D7=0;if(setjmp(_tmp1D5.handler))_tmp1D7=1;if(!_tmp1D7){{struct _tagged_arr*
msg=0;if(info != 0)msg=({struct _tagged_arr*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));
_tmp1D8[0]=(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp1DC;_tmp1DC.tag=0;
_tmp1DC.f1=(struct _tagged_arr)((struct _tagged_arr)(*info).f2);{struct Cyc_String_pa_struct
_tmp1DB;_tmp1DB.tag=0;_tmp1DB.f1=(struct _tagged_arr)((struct _tagged_arr)(*info).f1);{
void*_tmp1D9[2]={& _tmp1DB,& _tmp1DC};Cyc_aprintf(({const char*_tmp1DA="merging exports of %s and %s,";
_tag_arr(_tmp1DA,sizeof(char),_get_zero_arr_size(_tmp1DA,30));}),_tag_arr(
_tmp1D9,sizeof(void*),2));}}});_tmp1D8;});{struct Cyc_Core_Opt*_tmp1DD=Cyc_Interface_disj_merge_ns(
i1->exports,i2->exports,msg);if(_tmp1DD == 0){struct Cyc_Interface_I*_tmp1DE=0;
_npop_handler(0);return _tmp1DE;}if(info != 0)msg=({struct _tagged_arr*_tmp1DF=
_cycalloc(sizeof(*_tmp1DF));_tmp1DF[0]=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp1E3;_tmp1E3.tag=0;_tmp1E3.f1=(struct _tagged_arr)((struct _tagged_arr)(*info).f2);{
struct Cyc_String_pa_struct _tmp1E2;_tmp1E2.tag=0;_tmp1E2.f1=(struct _tagged_arr)((
struct _tagged_arr)(*info).f1);{void*_tmp1E0[2]={& _tmp1E2,& _tmp1E3};Cyc_aprintf(({
const char*_tmp1E1="merging imports of %s and %s,";_tag_arr(_tmp1E1,sizeof(char),
_get_zero_arr_size(_tmp1E1,30));}),_tag_arr(_tmp1E0,sizeof(void*),2));}}});
_tmp1DF;});{struct Cyc_Core_Opt*_tmp1E4=Cyc_Interface_compat_merge_ns(i1->imports,
i2->imports,(struct Cyc_Dict_Dict*)_tmp1DD->v,msg);if(_tmp1E4 == 0){struct Cyc_Interface_I*
_tmp1E5=0;_npop_handler(0);return _tmp1E5;}{struct Cyc_Interface_I*_tmp1E7=({
struct Cyc_Interface_I*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6->imports=(
struct Cyc_Dict_Dict*)_tmp1E4->v;_tmp1E6->exports=(struct Cyc_Dict_Dict*)_tmp1DD->v;
_tmp1E6;});_npop_handler(0);return _tmp1E7;}}}};_pop_handler();}else{void*_tmp1D6=(
void*)_exn_thrown;void*_tmp1E9=_tmp1D6;_LLB1: if(_tmp1E9 != Cyc_Tcdecl_Incompatible)
goto _LLB3;_LLB2: return 0;_LLB3:;_LLB4:(void)_throw(_tmp1E9);_LLB0:;}}}struct Cyc_Interface_I*
Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo){if(li
== 0)return(struct Cyc_Interface_I*)Cyc_Interface_empty();{struct Cyc_Interface_I*
curr_i=(struct Cyc_Interface_I*)li->hd;struct _tagged_arr*curr_info=linfo != 0?(
struct _tagged_arr*)((struct _tagged_arr*)linfo->hd): 0;li=li->tl;if(linfo != 0)
linfo=linfo->tl;for(0;li != 0;li=li->tl){struct Cyc_Interface_I*_tmp1EA=Cyc_Interface_merge(
curr_i,(struct Cyc_Interface_I*)li->hd,(curr_info != 0?linfo != 0: 0)?({struct
_tuple4*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0->f1=*curr_info;_tmp1F0->f2=*((
struct _tagged_arr*)linfo->hd);_tmp1F0;}): 0);if(_tmp1EA == 0)return 0;curr_i=(
struct Cyc_Interface_I*)_tmp1EA;if(curr_info != 0)curr_info=linfo != 0?({struct
_tagged_arr*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB[0]=(struct _tagged_arr)({
struct Cyc_String_pa_struct _tmp1EF;_tmp1EF.tag=0;_tmp1EF.f1=(struct _tagged_arr)((
struct _tagged_arr)*((struct _tagged_arr*)linfo->hd));{struct Cyc_String_pa_struct
_tmp1EE;_tmp1EE.tag=0;_tmp1EE.f1=(struct _tagged_arr)((struct _tagged_arr)*
curr_info);{void*_tmp1EC[2]={& _tmp1EE,& _tmp1EF};Cyc_aprintf(({const char*_tmp1ED="%s+%s";
_tag_arr(_tmp1ED,sizeof(char),_get_zero_arr_size(_tmp1ED,6));}),_tag_arr(_tmp1EC,
sizeof(void*),2));}}});_tmp1EB;}): 0;if(linfo != 0)linfo=linfo->tl;}return(struct
Cyc_Interface_I*)curr_i;}}struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(
struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*
linfo){if(la == 0)return(struct Cyc_Interface_I*)Cyc_Interface_empty();{struct Cyc_Interface_I*
curr_i=get((void*)la->hd);struct _tagged_arr*curr_info=linfo != 0?(struct
_tagged_arr*)((struct _tagged_arr*)linfo->hd): 0;la=la->tl;if(linfo != 0)linfo=
linfo->tl;for(0;la != 0;la=la->tl){struct Cyc_Interface_I*_tmp1F1=Cyc_Interface_merge(
curr_i,get((void*)la->hd),(curr_info != 0?linfo != 0: 0)?({struct _tuple4*_tmp1F7=
_cycalloc(sizeof(*_tmp1F7));_tmp1F7->f1=*curr_info;_tmp1F7->f2=*((struct
_tagged_arr*)linfo->hd);_tmp1F7;}): 0);if(_tmp1F1 == 0)return 0;curr_i=(struct Cyc_Interface_I*)
_tmp1F1;if(curr_info != 0)curr_info=linfo != 0?({struct _tagged_arr*_tmp1F2=
_cycalloc(sizeof(*_tmp1F2));_tmp1F2[0]=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp1F6;_tmp1F6.tag=0;_tmp1F6.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct _tagged_arr*)linfo->hd));{struct Cyc_String_pa_struct _tmp1F5;_tmp1F5.tag=0;
_tmp1F5.f1=(struct _tagged_arr)((struct _tagged_arr)*curr_info);{void*_tmp1F3[2]={&
_tmp1F5,& _tmp1F6};Cyc_aprintf(({const char*_tmp1F4="%s+%s";_tag_arr(_tmp1F4,
sizeof(char),_get_zero_arr_size(_tmp1F4,6));}),_tag_arr(_tmp1F3,sizeof(void*),2));}}});
_tmp1F2;}): 0;if(linfo != 0)linfo=linfo->tl;}return(struct Cyc_Interface_I*)curr_i;}}
static struct Cyc_List_List*Cyc_Interface_add_namespace(struct Cyc_List_List*tds,
struct Cyc_List_List*ns){if(ns == 0)return tds;return({struct Cyc_List_List*_tmp1F8=
_cycalloc(sizeof(*_tmp1F8));_tmp1F8->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp1F9=_cycalloc(sizeof(*_tmp1F9));_tmp1F9[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp1FA;_tmp1FA.tag=8;_tmp1FA.f1=(struct _tagged_arr*)ns->hd;_tmp1FA.f2=Cyc_Interface_add_namespace(
tds,ns->tl);_tmp1FA;});_tmp1F9;}),0);_tmp1F8->tl=0;_tmp1F8;});}static struct Cyc_List_List*
Cyc_Interface_add_aggrdecl(struct _tagged_arr*x,struct Cyc_Absyn_Aggrdecl*d,struct
Cyc_List_List*tds){return({struct Cyc_List_List*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));
_tmp1FB->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp1FC=
_cycalloc(sizeof(*_tmp1FC));_tmp1FC[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp1FD;
_tmp1FD.tag=4;_tmp1FD.f1=d;_tmp1FD;});_tmp1FC;}),0);_tmp1FB->tl=tds;_tmp1FB;});}
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl_header(struct _tagged_arr*x,
struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){d=({struct Cyc_Absyn_Aggrdecl*
_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE[0]=*d;_tmp1FE;});d->impl=0;if((void*)
d->sc != (void*)4)(void*)(d->sc=(void*)((void*)3));return({struct Cyc_List_List*
_tmp1FF=_cycalloc(sizeof(*_tmp1FF));_tmp1FF->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Aggr_d_struct*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200[0]=({
struct Cyc_Absyn_Aggr_d_struct _tmp201;_tmp201.tag=4;_tmp201.f1=d;_tmp201;});
_tmp200;}),0);_tmp1FF->tl=tds;_tmp1FF;});}static struct Cyc_List_List*Cyc_Interface_add_tuniondecl(
struct _tagged_arr*x,struct Cyc_Absyn_Tuniondecl*d,struct Cyc_List_List*tds){return({
struct Cyc_List_List*_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp203=_cycalloc(sizeof(*_tmp203));
_tmp203[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp204;_tmp204.tag=5;_tmp204.f1=d;
_tmp204;});_tmp203;}),0);_tmp202->tl=tds;_tmp202;});}static char _tmp205[2]="_";
static struct _tagged_arr Cyc_Interface_us={_tmp205,_tmp205,_tmp205 + 2};static
struct _tagged_arr*Cyc_Interface_us_p=& Cyc_Interface_us;static struct _tuple17*Cyc_Interface_rewrite_tunionfield_type(
struct _tuple17*x){return({struct _tuple17*_tmp206=_cycalloc(sizeof(*_tmp206));
_tmp206->f1=Cyc_Absyn_empty_tqual();_tmp206->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207[0]=({struct Cyc_Absyn_VarType_struct
_tmp208;_tmp208.tag=1;_tmp208.f1=({struct Cyc_Absyn_Tvar*_tmp209=_cycalloc(
sizeof(*_tmp209));_tmp209->name=Cyc_Interface_us_p;_tmp209->identity=0;_tmp209->kind=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp20A=_cycalloc(sizeof(*_tmp20A));
_tmp20A[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp20B;_tmp20B.tag=0;_tmp20B.f1=(void*)((
void*)2);_tmp20B;});_tmp20A;}));_tmp209;});_tmp208;});_tmp207;});_tmp206;});}
static struct Cyc_Absyn_Tunionfield*Cyc_Interface_rewrite_tunionfield(struct Cyc_Absyn_Tunionfield*
f){f=({struct Cyc_Absyn_Tunionfield*_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C[0]=*
f;_tmp20C;});f->typs=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct
_tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_tunionfield_type,
f->typs);return f;}static struct Cyc_List_List*Cyc_Interface_add_tuniondecl_header(
struct _tagged_arr*x,struct Cyc_Absyn_Tuniondecl*d,struct Cyc_List_List*tds){d=({
struct Cyc_Absyn_Tuniondecl*_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D[0]=*d;
_tmp20D;});if(d->fields != 0)d->fields=({struct Cyc_Core_Opt*_tmp20E=_cycalloc(
sizeof(*_tmp20E));_tmp20E->v=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tunionfield*(*
f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_tunionfield,(
struct Cyc_List_List*)(d->fields)->v);_tmp20E;});if((void*)d->sc != (void*)4)(void*)(
d->sc=(void*)((void*)3));return({struct Cyc_List_List*_tmp20F=_cycalloc(sizeof(*
_tmp20F));_tmp20F->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp211;_tmp211.tag=5;_tmp211.f1=d;_tmp211;});_tmp210;}),0);_tmp20F->tl=tds;
_tmp20F;});}static struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct
_tagged_arr*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){return({struct
Cyc_List_List*_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Enum_d_struct*_tmp213=_cycalloc(sizeof(*_tmp213));
_tmp213[0]=({struct Cyc_Absyn_Enum_d_struct _tmp214;_tmp214.tag=6;_tmp214.f1=d;
_tmp214;});_tmp213;}),0);_tmp212->tl=tds;_tmp212;});}static struct Cyc_List_List*
Cyc_Interface_add_enumdecl_header(struct _tagged_arr*x,struct Cyc_Absyn_Enumdecl*d,
struct Cyc_List_List*tds){d=({struct Cyc_Absyn_Enumdecl*_tmp215=_cycalloc(sizeof(*
_tmp215));_tmp215[0]=*d;_tmp215;});d->fields=0;if((void*)d->sc != (void*)4)(void*)(
d->sc=(void*)((void*)3));return({struct Cyc_List_List*_tmp216=_cycalloc(sizeof(*
_tmp216));_tmp216->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp217=_cycalloc(sizeof(*_tmp217));_tmp217[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp218;_tmp218.tag=6;_tmp218.f1=d;_tmp218;});_tmp217;}),0);_tmp216->tl=tds;
_tmp216;});}static struct Cyc_List_List*Cyc_Interface_add_typedef(struct
_tagged_arr*x,struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds){return({
struct Cyc_List_List*_tmp219=_cycalloc(sizeof(*_tmp219));_tmp219->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Typedef_d_struct*_tmp21A=_cycalloc(sizeof(*_tmp21A));
_tmp21A[0]=({struct Cyc_Absyn_Typedef_d_struct _tmp21B;_tmp21B.tag=7;_tmp21B.f1=d;
_tmp21B;});_tmp21A;}),0);_tmp219->tl=tds;_tmp219;});}static struct Cyc_List_List*
Cyc_Interface_add_vardecl(struct _tagged_arr*x,struct Cyc_Absyn_Vardecl*d,struct
Cyc_List_List*tds){return({struct Cyc_List_List*_tmp21C=_cycalloc(sizeof(*_tmp21C));
_tmp21C->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp21D=
_cycalloc(sizeof(*_tmp21D));_tmp21D[0]=({struct Cyc_Absyn_Var_d_struct _tmp21E;
_tmp21E.tag=0;_tmp21E.f1=d;_tmp21E;});_tmp21D;}),0);_tmp21C->tl=tds;_tmp21C;});}
static struct Cyc_List_List*Cyc_Interface_add_xtunionfielddecl(struct _tagged_arr*x,
struct Cyc_Tcdecl_Xtunionfielddecl*d,struct Cyc_List_List*tds){struct Cyc_Tcdecl_Xtunionfielddecl
_tmp220;struct Cyc_Absyn_Tuniondecl*_tmp221;struct Cyc_Absyn_Tunionfield*_tmp222;
struct Cyc_Tcdecl_Xtunionfielddecl*_tmp21F=d;_tmp220=*_tmp21F;_tmp221=_tmp220.base;
_tmp222=_tmp220.field;_tmp221=({struct Cyc_Absyn_Tuniondecl*_tmp223=_cycalloc(
sizeof(*_tmp223));_tmp223[0]=*_tmp221;_tmp223;});_tmp221->fields=({struct Cyc_Core_Opt*
_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224->v=({struct Cyc_List_List*_tmp225=
_cycalloc(sizeof(*_tmp225));_tmp225->hd=_tmp222;_tmp225->tl=0;_tmp225;});_tmp224;});(
void*)(_tmp221->sc=(void*)((void*)3));return({struct Cyc_List_List*_tmp226=
_cycalloc(sizeof(*_tmp226));_tmp226->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp228;_tmp228.tag=5;_tmp228.f1=_tmp221;_tmp228;});_tmp227;}),0);_tmp226->tl=
tds;_tmp226;});}static struct Cyc_List_List*Cyc_Interface_add_xtunionfielddecl_header(
struct _tagged_arr*x,struct Cyc_Tcdecl_Xtunionfielddecl*d,struct Cyc_List_List*tds){
struct Cyc_Tcdecl_Xtunionfielddecl _tmp22A;struct Cyc_Absyn_Tuniondecl*_tmp22B;
struct Cyc_Absyn_Tunionfield*_tmp22C;struct Cyc_Tcdecl_Xtunionfielddecl*_tmp229=d;
_tmp22A=*_tmp229;_tmp22B=_tmp22A.base;_tmp22C=_tmp22A.field;_tmp22B=({struct Cyc_Absyn_Tuniondecl*
_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D[0]=*_tmp22B;_tmp22D;});_tmp22C=Cyc_Interface_rewrite_tunionfield(
_tmp22C);(void*)(_tmp22C->sc=(void*)((void*)3));_tmp22B->fields=({struct Cyc_Core_Opt*
_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E->v=({struct Cyc_List_List*_tmp22F=
_cycalloc(sizeof(*_tmp22F));_tmp22F->hd=_tmp22C;_tmp22F->tl=0;_tmp22F;});_tmp22E;});(
void*)(_tmp22B->sc=(void*)((void*)3));return({struct Cyc_List_List*_tmp230=
_cycalloc(sizeof(*_tmp230));_tmp230->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp232;_tmp232.tag=5;_tmp232.f1=_tmp22B;_tmp232;});_tmp231;}),0);_tmp230->tl=
tds;_tmp230;});}static void Cyc_Interface_print_ns_headers(struct Cyc___cycFILE*f,
struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*_tmp233=
0;_tmp233=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tagged_arr*,
struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*d,struct Cyc_List_List*
accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl_header,ie->aggrdecls,_tmp233);
_tmp233=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tagged_arr*,
struct Cyc_Absyn_Tuniondecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*d,struct
Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_tuniondecl_header,ie->tuniondecls,
_tmp233);_tmp233=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl_header,ie->enumdecls,
_tmp233);if(_tmp233 != 0){_tmp233=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp233);_tmp233=Cyc_Interface_add_namespace(_tmp233,ns);Cyc_Absynpp_decllist2file(
_tmp233,f);}}static void Cyc_Interface_print_ns_xtunionfielddecl_headers(struct Cyc___cycFILE*
f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*
_tmp234=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tagged_arr*,
struct Cyc_Tcdecl_Xtunionfielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*d,
struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xtunionfielddecl_header,
ie->xtunionfielddecls,0);if(_tmp234 != 0){_tmp234=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp234);_tmp234=Cyc_Interface_add_namespace(
_tmp234,ns);Cyc_Absynpp_decllist2file(_tmp234,f);}}static void Cyc_Interface_print_ns_typedefs(
struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct
Cyc_List_List*_tmp235=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_typedef,ie->typedefdecls,
0);if(_tmp235 != 0){_tmp235=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp235);_tmp235=Cyc_Interface_add_namespace(_tmp235,ns);Cyc_Absynpp_decllist2file(
_tmp235,f);}}static void Cyc_Interface_print_ns_decls(struct Cyc___cycFILE*f,struct
Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*_tmp236=0;
_tmp236=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tagged_arr*,
struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*d,struct Cyc_List_List*
accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl,ie->aggrdecls,_tmp236);_tmp236=((
struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tagged_arr*,struct Cyc_Absyn_Tuniondecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict*d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_tuniondecl,ie->tuniondecls,_tmp236);_tmp236=((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _tagged_arr*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),
struct Cyc_Dict_Dict*d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl,
ie->enumdecls,_tmp236);_tmp236=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(
struct _tagged_arr*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_vardecl,ie->vardecls,
_tmp236);_tmp236=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Tcdecl_Xtunionfielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls,_tmp236);if(_tmp236 != 0){_tmp236=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp236);_tmp236=Cyc_Interface_add_namespace(
_tmp236,ns);Cyc_Absynpp_decllist2file(_tmp236,f);}}void Cyc_Interface_print(
struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);({
void*_tmp237[0]={};Cyc_fprintf(f,({const char*_tmp238="/****** needed (headers) ******/\n";
_tag_arr(_tmp238,sizeof(char),_get_zero_arr_size(_tmp238,34));}),_tag_arr(
_tmp237,sizeof(void*),0));});((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,
struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_print_ns_headers,f,i->imports);({void*_tmp239[0]={};Cyc_fprintf(f,({
const char*_tmp23A="\n/****** provided (headers) ******/\n";_tag_arr(_tmp23A,
sizeof(char),_get_zero_arr_size(_tmp23A,37));}),_tag_arr(_tmp239,sizeof(void*),0));});((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,
f,i->exports);({void*_tmp23B[0]={};Cyc_fprintf(f,({const char*_tmp23C="\n/****** needed (headers of xtunionfielddecls) ******/\n";
_tag_arr(_tmp23C,sizeof(char),_get_zero_arr_size(_tmp23C,56));}),_tag_arr(
_tmp23B,sizeof(void*),0));});((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,
struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_print_ns_xtunionfielddecl_headers,f,i->imports);({void*_tmp23D[0]={};
Cyc_fprintf(f,({const char*_tmp23E="\n/****** provided (headers of xtunionfielddecls) ******/\n";
_tag_arr(_tmp23E,sizeof(char),_get_zero_arr_size(_tmp23E,58));}),_tag_arr(
_tmp23D,sizeof(void*),0));});((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,
struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_print_ns_xtunionfielddecl_headers,f,i->exports);({void*_tmp23F[0]={};
Cyc_fprintf(f,({const char*_tmp240="\n/****** provided (typedefs) ******/\n";
_tag_arr(_tmp240,sizeof(char),_get_zero_arr_size(_tmp240,38));}),_tag_arr(
_tmp23F,sizeof(void*),0));});((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,
struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_print_ns_typedefs,f,i->exports);({void*_tmp241[0]={};Cyc_fprintf(f,({
const char*_tmp242="\n/****** needed (declarations) ******/\n";_tag_arr(_tmp242,
sizeof(char),_get_zero_arr_size(_tmp242,40));}),_tag_arr(_tmp241,sizeof(void*),0));});((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,
f,i->imports);({void*_tmp243[0]={};Cyc_fprintf(f,({const char*_tmp244="\n/****** provided (declarations) ******/\n";
_tag_arr(_tmp244,sizeof(char),_get_zero_arr_size(_tmp244,42));}),_tag_arr(
_tmp243,sizeof(void*),0));});((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,
struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_print_ns_decls,f,i->exports);}struct Cyc_Interface_I*Cyc_Interface_parse(
struct Cyc___cycFILE*f){Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp245=Cyc_Parse_parse_file(
f);struct Cyc_Tcenv_Tenv*_tmp246=Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp246,0,_tmp245);
return Cyc_Interface_gen_extract(_tmp246->ae,0);}}extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Aggrdecl_rep;static struct Cyc_Typerep_ThinPtr_struct Cyc_aggrdecl_p={
1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};void*Cyc_aggrdecl_rep=(void*)&
Cyc_aggrdecl_p;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tuniondecl_rep;
static struct Cyc_Typerep_ThinPtr_struct Cyc_tuniondecl_p={1,1,(void*)((void*)& Cyc_struct_Absyn_Tuniondecl_rep)};
void*Cyc_tuniondecl_rep=(void*)& Cyc_tuniondecl_p;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumdecl_rep;static struct Cyc_Typerep_ThinPtr_struct Cyc_enumdecl_p={
1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};void*Cyc_enumdecl_rep=(void*)&
Cyc_enumdecl_p;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep;
static struct Cyc_Typerep_ThinPtr_struct Cyc_typedefdecl_p={1,1,(void*)((void*)& Cyc_struct_Absyn_Typedefdecl_rep)};
void*Cyc_typedefdecl_rep=(void*)& Cyc_typedefdecl_p;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Vardecl_rep;static struct Cyc_Typerep_ThinPtr_struct Cyc_vardecl_p={
1,1,(void*)((void*)& Cyc_struct_Absyn_Vardecl_rep)};void*Cyc_vardecl_rep=(void*)&
Cyc_vardecl_p;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep;
static struct Cyc_Typerep_ThinPtr_struct Cyc_tunionfield_p={1,1,(void*)((void*)& Cyc_struct_Absyn_Tunionfield_rep)};
void*Cyc_tunionfield_rep=(void*)& Cyc_tunionfield_p;static struct _tuple9 Cyc_Interface_write_aggrdecl_t(
struct _tuple9 env,struct Cyc___cycFILE*fp,struct Cyc_Absyn_Aggrdecl*decl){return((
struct _tuple9(*)(void*rep,struct _tuple9 env,struct Cyc___cycFILE*fp,struct Cyc_Absyn_Aggrdecl*
val))Cyc_Marshal_write_type_base)(Cyc_aggrdecl_rep,env,fp,decl);}static struct
_tuple9 Cyc_Interface_write_tuniondecl_t(struct _tuple9 env,struct Cyc___cycFILE*fp,
struct Cyc_Absyn_Tuniondecl*decl){return((struct _tuple9(*)(void*rep,struct _tuple9
env,struct Cyc___cycFILE*fp,struct Cyc_Absyn_Tuniondecl*val))Cyc_Marshal_write_type_base)(
Cyc_tuniondecl_rep,env,fp,decl);}static struct _tuple9 Cyc_Interface_write_enumdecl_t(
struct _tuple9 env,struct Cyc___cycFILE*fp,struct Cyc_Absyn_Enumdecl*decl){return((
struct _tuple9(*)(void*rep,struct _tuple9 env,struct Cyc___cycFILE*fp,struct Cyc_Absyn_Enumdecl*
val))Cyc_Marshal_write_type_base)(Cyc_enumdecl_rep,env,fp,decl);}static struct
_tuple9 Cyc_Interface_write_typedefdecl_t(struct _tuple9 env,struct Cyc___cycFILE*fp,
struct Cyc_Absyn_Typedefdecl*decl){return((struct _tuple9(*)(void*rep,struct
_tuple9 env,struct Cyc___cycFILE*fp,struct Cyc_Absyn_Typedefdecl*val))Cyc_Marshal_write_type_base)(
Cyc_typedefdecl_rep,env,fp,decl);}static struct _tuple9 Cyc_Interface_write_vardecl_t(
struct _tuple9 env,struct Cyc___cycFILE*fp,struct Cyc_Absyn_Vardecl*decl){return((
struct _tuple9(*)(void*rep,struct _tuple9 env,struct Cyc___cycFILE*fp,struct Cyc_Absyn_Vardecl*
val))Cyc_Marshal_write_type_base)(Cyc_vardecl_rep,env,fp,decl);}static struct
_tuple9 Cyc_Interface_write_tunionfield_t(struct _tuple9 env,struct Cyc___cycFILE*fp,
struct Cyc_Absyn_Tunionfield*decl){return((struct _tuple9(*)(void*rep,struct
_tuple9 env,struct Cyc___cycFILE*fp,struct Cyc_Absyn_Tunionfield*val))Cyc_Marshal_write_type_base)(
Cyc_tunionfield_rep,env,fp,decl);}static struct _tuple9 Cyc_Interface_write_xtunionfielddecl_t(
struct _tuple9 env,struct Cyc___cycFILE*fp,struct Cyc_Tcdecl_Xtunionfielddecl*decl){
env=Cyc_Interface_write_tuniondecl_t(env,fp,decl->base);env=Cyc_Interface_write_tunionfield_t(
env,fp,decl->field);return env;}void Cyc_Interface_save(struct Cyc_Interface_I*i,
struct Cyc___cycFILE*f){Cyc_Interface_print(i,f);}struct Cyc_Interface_I*Cyc_Interface_load(
struct Cyc___cycFILE*f){return Cyc_Interface_parse(f);}
