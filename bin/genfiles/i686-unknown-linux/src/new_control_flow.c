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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);int Cyc_List_memq(struct Cyc_List_List*
l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_insert(
struct Cyc_Set_Set*s,void*elt);struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*
s1,struct Cyc_Set_Set*s2);extern char Cyc_Set_Absent[11];typedef struct{int __count;
union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{
int __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _tagged_arr f1;};extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Dict_Dict;
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*
Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
int Cyc_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*s2);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;
};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*
f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*f1;};
struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
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
_tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Aggrfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;};struct Cyc_Absyn_Aggrdecl{void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
15];struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(
void*,struct Cyc_Absyn_Conref*x);struct _tagged_arr*Cyc_Absyn_fieldname(int);int
Cyc_Absyn_is_union_type(void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
_tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr
fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b);
int Cyc_Tcutil_is_noreturn(void*);struct _tuple4{unsigned int f1;int f2;};struct
_tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{
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
f1;struct Cyc_List_List*f2;};struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();
extern void*Cyc_CfFlowInfo_unknown_none;extern void*Cyc_CfFlowInfo_unknown_all;
extern void*Cyc_CfFlowInfo_esc_none;extern void*Cyc_CfFlowInfo_esc_all;int Cyc_CfFlowInfo_root_cmp(
void*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict*d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(void*
f1,void*f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,
struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);struct
Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**
all_changed,void*r);struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);void*Cyc_CfFlowInfo_join_flow(struct Cyc_Set_Set**,void*,void*);
struct _tuple0 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_Set_Set**all_changed,
struct _tuple0 pr1,struct _tuple0 pr2);void*Cyc_CfFlowInfo_after_flow(struct Cyc_Set_Set**,
void*,void*,struct Cyc_Set_Set*,struct Cyc_Set_Set*);void*Cyc_CfFlowInfo_kill_flow_region(
void*f,void*rgn);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct
Cyc_List_List*ds);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*
encloser;int visited;void*flow;};static char Cyc_NewControlFlow_CFAnnot[12]="\000\000\000\000CFAnnot\000";
struct Cyc_NewControlFlow_CFAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser){(void*)(enclosee->annot=(void*)((void*)({struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct
_tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot
_tmp2;_tmp2.encloser=encloser;_tmp2.visited=0;_tmp2.flow=(void*)((void*)0);_tmp2;});
_tmp1;});_tmp0;})));}static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s){void*_tmp3=(void*)s->annot;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp4;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: if(*((void**)_tmp3)!= Cyc_NewControlFlow_CFAnnot)
goto _LL3;_tmp4=((struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmp5=(
struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp3)->f1;_LL2: return _tmp5;_LL3:;_LL4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Core_Impossible_struct _tmp7;
_tmp7.tag=Cyc_Core_Impossible;_tmp7.f1=({const char*_tmp8="ControlFlow -- wrong stmt annotation";
_tag_arr(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,37));});_tmp7;});_tmp6;}));
_LL0:;}struct Cyc_NewControlFlow_AnalEnv{int iterate_again;int iteration_num;int
in_try;void*tryflow;struct Cyc_Set_Set**all_changed;int noreturn;struct Cyc_List_List*
param_roots;};static void*Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Stmt*);static void*Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Decl*);static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,void*,struct Cyc_Absyn_Exp*);static struct
_tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,void*,
struct Cyc_Absyn_Exp*);static struct _tuple0 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Exp*);static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Stmt*s){struct
Cyc_NewControlFlow_CFStmtAnnot*_tmp9=Cyc_NewControlFlow_get_stmt_annot(s);(void*)(
_tmp9->flow=(void*)Cyc_CfFlowInfo_join_flow(env->all_changed,inflow,(void*)_tmp9->flow));
_tmp9->visited=env->iteration_num;return _tmp9;}static void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv*env,void*new_flow){if(env->in_try)(void*)(env->tryflow=(
void*)Cyc_CfFlowInfo_join_flow(env->all_changed,new_flow,(void*)env->tryflow));}
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_Absyn_Stmt*s,void*flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpA=Cyc_NewControlFlow_get_stmt_annot(
s);void*_tmpB=Cyc_CfFlowInfo_join_flow(env->all_changed,flow,(void*)_tmpA->flow);
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmpB,(void*)_tmpA->flow)){(void*)(_tmpA->flow=(
void*)_tmpB);if(_tmpA->visited == env->iteration_num)env->iterate_again=1;}}
static void*Cyc_NewControlFlow_add_vars(void*inflow,struct Cyc_List_List*vds,void*
leafval){void*_tmpC=inflow;struct Cyc_Dict_Dict*_tmpD;struct Cyc_List_List*_tmpE;
_LL6: if((int)_tmpC != 0)goto _LL8;_LL7: return(void*)0;_LL8: if(_tmpC <= (void*)1  || *((
int*)_tmpC)!= 0)goto _LL5;_tmpD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpC)->f1;
_tmpE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpC)->f2;_LL9: for(0;vds != 0;
vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct*_tmpF=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp11;_tmp11.tag=0;_tmp11.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp11;});_tmp10;});
_tmpD=Cyc_Dict_insert(_tmpD,(void*)_tmpF,Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp13;_tmp13.tag=0;_tmp13.f1=_tmpD;_tmp13.f2=_tmpE;_tmp13;});_tmp12;});_LL5:;}
static void*Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_Position_Segment*loc,void*inflow,void*r){void*_tmp14=inflow;struct Cyc_Dict_Dict*
_tmp15;struct Cyc_List_List*_tmp16;_LLB: if((int)_tmp14 != 0)goto _LLD;_LLC: return(
void*)0;_LLD: if(_tmp14 <= (void*)1  || *((int*)_tmp14)!= 0)goto _LLA;_tmp15=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp14)->f1;_tmp16=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp14)->f2;_LLE: if(Cyc_CfFlowInfo_initlevel(_tmp15,r)!= (void*)2)({void*_tmp17[
0]={};Cyc_Tcutil_terr(loc,({const char*_tmp18="expression may not be fully initialized";
_tag_arr(_tmp18,sizeof(char),_get_zero_arr_size(_tmp18,40));}),_tag_arr(_tmp17,
sizeof(void*),0));});{struct Cyc_Dict_Dict*_tmp19=Cyc_CfFlowInfo_escape_deref(
_tmp15,env->all_changed,r);if(_tmp15 == _tmp19)return inflow;{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1A=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));
_tmp1B[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp1C;_tmp1C.tag=0;_tmp1C.f1=
_tmp19;_tmp1C.f2=_tmp16;_tmp1C;});_tmp1B;});Cyc_NewControlFlow_update_tryflow(
env,(void*)_tmp1A);return(void*)_tmp1A;}}_LLA:;}struct _tuple5{void*f1;struct Cyc_List_List*
f2;};static struct _tuple5 Cyc_NewControlFlow_anal_unordered_Rexps(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es){if(es == 0)return({struct _tuple5 _tmp1D;
_tmp1D.f1=inflow;_tmp1D.f2=0;_tmp1D;});if(es->tl == 0){void*_tmp1F;void*_tmp20;
struct _tuple0 _tmp1E=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp1F=_tmp1E.f1;_tmp20=_tmp1E.f2;return({struct _tuple5 _tmp21;_tmp21.f1=
_tmp1F;_tmp21.f2=({struct Cyc_List_List*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->hd=(
void*)_tmp20;_tmp22->tl=0;_tmp22;});_tmp21;});}{struct Cyc_Set_Set**
outer_all_changed=env->all_changed;struct Cyc_Set_Set*this_all_changed;void*
old_inflow;void*outflow;struct Cyc_List_List*rvals;do{this_all_changed=Cyc_CfFlowInfo_mt_place_set();
env->all_changed=({struct Cyc_Set_Set**_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23[0]=
Cyc_CfFlowInfo_mt_place_set();_tmp23;});{void*_tmp25;void*_tmp26;struct _tuple0
_tmp24=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)es->hd);
_tmp25=_tmp24.f1;_tmp26=_tmp24.f2;outflow=_tmp25;rvals=({struct Cyc_List_List*
_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->hd=(void*)_tmp26;_tmp27->tl=0;_tmp27;});
this_all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(this_all_changed,*((struct Cyc_Set_Set**)_check_null(env->all_changed)));{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){env->all_changed=({
struct Cyc_Set_Set**_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp28;});{void*_tmp2A;void*_tmp2B;struct _tuple0 _tmp29=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp2A=_tmp29.f1;_tmp2B=_tmp29.f2;
rvals=({struct Cyc_List_List*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->hd=(void*)
_tmp2B;_tmp2C->tl=rvals;_tmp2C;});outflow=Cyc_CfFlowInfo_after_flow((struct Cyc_Set_Set**)&
this_all_changed,outflow,_tmp2A,this_all_changed,*((struct Cyc_Set_Set**)
_check_null(env->all_changed)));this_all_changed=((struct Cyc_Set_Set*(*)(struct
Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(this_all_changed,*((
struct Cyc_Set_Set**)_check_null(env->all_changed)));}}}old_inflow=inflow;inflow=
Cyc_CfFlowInfo_join_flow(outer_all_changed,inflow,outflow);}}while(!Cyc_CfFlowInfo_flow_lessthan_approx(
inflow,old_inflow));if(outer_all_changed == 0)env->all_changed=0;else{env->all_changed=({
struct Cyc_Set_Set**_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D[0]=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*outer_all_changed,
this_all_changed);_tmp2D;});}Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple5 _tmp2E;_tmp2E.f1=outflow;_tmp2E.f2=Cyc_List_imp_rev(rvals);
_tmp2E;});}}static struct _tuple0 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es){void*_tmp30;struct Cyc_List_List*_tmp31;
struct _tuple5 _tmp2F=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,es);
_tmp30=_tmp2F.f1;_tmp31=_tmp2F.f2;{void*_tmp32=_tmp30;struct Cyc_Dict_Dict*_tmp33;
struct Cyc_List_List*_tmp34;_LL10: if(_tmp32 <= (void*)1  || *((int*)_tmp32)!= 0)
goto _LL12;_tmp33=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp32)->f1;_tmp34=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp32)->f2;_LL11: for(0;_tmp31 != 0;(
_tmp31=_tmp31->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
_tmp33,(void*)_tmp31->hd)== (void*)0)({void*_tmp35[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp36="expression may not be initialized";_tag_arr(_tmp36,sizeof(char),
_get_zero_arr_size(_tmp36,34));}),_tag_arr(_tmp35,sizeof(void*),0));});}goto _LLF;
_LL12: if((int)_tmp32 != 0)goto _LLF;_LL13: goto _LLF;_LLF:;}return({struct _tuple0
_tmp37;_tmp37.f1=_tmp30;_tmp37.f2=Cyc_CfFlowInfo_unknown_all;_tmp37;});}static
void*Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,
void*outflow,struct Cyc_Absyn_Exp*e,void*il){void*_tmp38=outflow;struct Cyc_Dict_Dict*
_tmp39;struct Cyc_List_List*_tmp3A;_LL15: if((int)_tmp38 != 0)goto _LL17;_LL16:
return outflow;_LL17: if(_tmp38 <= (void*)1  || *((int*)_tmp38)!= 0)goto _LL14;_tmp39=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp38)->f1;_tmp3A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp38)->f2;_LL18: {void*_tmp3B=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;
struct Cyc_CfFlowInfo_Place*_tmp3C;_LL1A: if((int)_tmp3B != 0)goto _LL1C;_LL1B:
return outflow;_LL1C: if(_tmp3B <= (void*)1  || *((int*)_tmp3B)!= 0)goto _LL19;_tmp3C=((
struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp3B)->f1;_LL1D: {void*nzval=il == (void*)2?(
void*)1:(void*)2;return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3D=
_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp3E;_tmp3E.tag=0;_tmp3E.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp39,env->all_changed,
_tmp3C,nzval);_tmp3E.f2=_tmp3A;_tmp3E;});_tmp3D;});}_LL19:;}_LL14:;}static struct
_tuple0 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,void*outflow,struct Cyc_Absyn_Exp*e,void*il){void*_tmp3F=outflow;struct Cyc_Dict_Dict*
_tmp40;struct Cyc_List_List*_tmp41;_LL1F: if((int)_tmp3F != 0)goto _LL21;_LL20:
return({struct _tuple0 _tmp42;_tmp42.f1=outflow;_tmp42.f2=outflow;_tmp42;});_LL21:
if(_tmp3F <= (void*)1  || *((int*)_tmp3F)!= 0)goto _LL1E;_tmp40=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp3F)->f1;_tmp41=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3F)->f2;_LL22: {
void*_tmp43=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*
_tmp44;_LL24: if((int)_tmp43 != 0)goto _LL26;_LL25: return({struct _tuple0 _tmp45;
_tmp45.f1=outflow;_tmp45.f2=outflow;_tmp45;});_LL26: if(_tmp43 <= (void*)1  || *((
int*)_tmp43)!= 0)goto _LL23;_tmp44=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp43)->f1;
_LL27: {void*nzval=il == (void*)2?(void*)1:(void*)2;return({struct _tuple0 _tmp46;
_tmp46.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp49=_cycalloc(
sizeof(*_tmp49));_tmp49[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp4A;
_tmp4A.tag=0;_tmp4A.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp40,env->all_changed,
_tmp44,nzval);_tmp4A.f2=_tmp41;_tmp4A;});_tmp49;});_tmp46.f2=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp48;_tmp48.tag=0;_tmp48.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp40,env->all_changed,
_tmp44,(void*)0);_tmp48.f2=_tmp41;_tmp48;});_tmp47;});_tmp46;});}_LL23:;}_LL1E:;}
static void*Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*env,
void*flow,struct Cyc_Absyn_Exp*e1,void*r1,void*p,void*r2){void*_tmp4B=flow;struct
Cyc_Dict_Dict*_tmp4C;struct Cyc_List_List*_tmp4D;_LL29: if((int)_tmp4B != 0)goto
_LL2B;_LL2A: return flow;_LL2B: if(_tmp4B <= (void*)1  || *((int*)_tmp4B)!= 0)goto
_LL28;_tmp4C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4B)->f1;_tmp4D=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4B)->f2;_LL2C: {void*_tmp4E=r2;
struct Cyc_List_List*_tmp4F;_LL2E: if(_tmp4E <= (void*)3  || *((int*)_tmp4E)!= 3)
goto _LL30;_tmp4F=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp4E)->f1;_LL2F: {void*
_tmp50=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*
_tmp51;_LL33: if((int)_tmp50 != 0)goto _LL35;_LL34: return flow;_LL35: if(_tmp50 <= (
void*)1  || *((int*)_tmp50)!= 0)goto _LL32;_tmp51=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp50)->f1;_LL36: {struct Cyc_List_List*new_cl;{void*_tmp52=r1;struct Cyc_List_List*
_tmp53;void*_tmp54;_LL38: if(_tmp52 <= (void*)3  || *((int*)_tmp52)!= 3)goto _LL3A;
_tmp53=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp52)->f1;_LL39: new_cl=_tmp53;
goto _LL37;_LL3A: if(_tmp52 <= (void*)3  || *((int*)_tmp52)!= 0)goto _LL3C;_tmp54=(
void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp52)->f1;if((int)_tmp54 != 2)
goto _LL3C;_LL3B: goto _LL3D;_LL3C: if((int)_tmp52 != 0)goto _LL3E;_LL3D: goto _LL3F;
_LL3E: if((int)_tmp52 != 1)goto _LL40;_LL3F: new_cl=0;goto _LL37;_LL40:;_LL41: return
flow;_LL37:;}for(0;_tmp4F != 0;_tmp4F=_tmp4F->tl){void*new_cmp;{struct _tuple0
_tmp56=({struct _tuple0 _tmp55;_tmp55.f1=p;_tmp55.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp4F->hd)->cmp;_tmp55;});void*_tmp57;void*_tmp58;void*_tmp59;void*_tmp5A;_LL43:
_tmp57=_tmp56.f1;if((int)_tmp57 != 8)goto _LL45;_LL44: goto _LL46;_LL45: _tmp58=
_tmp56.f2;if((int)_tmp58 != 8)goto _LL47;_LL46: new_cmp=(void*)8;goto _LL42;_LL47:
_tmp59=_tmp56.f1;if((int)_tmp59 != 5)goto _LL49;_tmp5A=_tmp56.f2;if((int)_tmp5A != 
5)goto _LL49;_LL48: new_cmp=(void*)5;goto _LL42;_LL49:;_LL4A: new_cmp=(void*)10;goto
_LL42;_LL42:;}new_cl=({struct Cyc_List_List*_tmp5B=_cycalloc(sizeof(*_tmp5B));
_tmp5B->hd=({struct Cyc_CfFlowInfo_TagCmp*_tmp5C=_cycalloc(sizeof(*_tmp5C));
_tmp5C->cmp=(void*)new_cmp;_tmp5C->bd=(void*)((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp4F->hd)->bd);_tmp5C;});_tmp5B->tl=new_cl;_tmp5B;});}return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp5E;_tmp5E.tag=0;_tmp5E.f1=Cyc_CfFlowInfo_assign_place(e1->loc,_tmp4C,env->all_changed,
_tmp51,(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*_tmp5F=_cycalloc(sizeof(*
_tmp5F));_tmp5F[0]=({struct Cyc_CfFlowInfo_TagCmps_struct _tmp60;_tmp60.tag=3;
_tmp60.f1=new_cl;_tmp60;});_tmp5F;}));_tmp5E.f2=_tmp4D;_tmp5E;});_tmp5D;});}
_LL32:;}_LL30:;_LL31: return flow;_LL2D:;}_LL28:;}static struct _tuple0 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,void*f,struct Cyc_Absyn_Exp*e,
void*r){void*_tmp61=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp62;void*_tmp63;struct Cyc_Absyn_PtrAtts
_tmp64;struct Cyc_Absyn_Conref*_tmp65;struct Cyc_Absyn_Conref*_tmp66;_LL4C: if(
_tmp61 <= (void*)3  || *((int*)_tmp61)!= 4)goto _LL4E;_tmp62=((struct Cyc_Absyn_PointerType_struct*)
_tmp61)->f1;_tmp63=(void*)_tmp62.elt_typ;_tmp64=_tmp62.ptr_atts;_tmp65=_tmp64.bounds;
_tmp66=_tmp64.zero_term;_LL4D: {void*_tmp67=f;struct Cyc_Dict_Dict*_tmp68;struct
Cyc_List_List*_tmp69;_LL51: if((int)_tmp67 != 0)goto _LL53;_LL52: return({struct
_tuple0 _tmp6A;_tmp6A.f1=f;_tmp6A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp63,Cyc_CfFlowInfo_unknown_all);
_tmp6A;});_LL53: if(_tmp67 <= (void*)1  || *((int*)_tmp67)!= 0)goto _LL50;_tmp68=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp67)->f1;_tmp69=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp67)->f2;_LL54: if(Cyc_Tcutil_is_bound_one(_tmp65)){void*_tmp6B=r;struct Cyc_CfFlowInfo_Place*
_tmp6C;void*_tmp6D;_LL56: if((int)_tmp6B != 1)goto _LL58;_LL57: goto _LL59;_LL58: if((
int)_tmp6B != 2)goto _LL5A;_LL59:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp6F;_tmp6F.tag=Cyc_CfFlowInfo_NotZero;_tmp6F.f1=_tmp69;_tmp6F;});_tmp6E;})));
goto _LL55;_LL5A: if(_tmp6B <= (void*)3  || *((int*)_tmp6B)!= 2)goto _LL5C;_tmp6C=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp6B)->f1;_LL5B:(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp70=_cycalloc(sizeof(*_tmp70));
_tmp70[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp71;_tmp71.tag=Cyc_CfFlowInfo_NotZero;
_tmp71.f1=_tmp69;_tmp71;});_tmp70;})));return({struct _tuple0 _tmp72;_tmp72.f1=f;
_tmp72.f2=Cyc_CfFlowInfo_lookup_place(_tmp68,_tmp6C);_tmp72;});_LL5C: if((int)
_tmp6B != 0)goto _LL5E;_LL5D:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple0 _tmp73;_tmp73.f1=(void*)0;_tmp73.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp63,Cyc_CfFlowInfo_unknown_all);_tmp73;});_LL5E: if(_tmp6B <= (void*)3  || *((
int*)_tmp6B)!= 0)goto _LL60;_tmp6D=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp6B)->f1;_LL5F: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp6D);goto _LL61;
_LL60:;_LL61:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp75;_tmp75.tag=Cyc_CfFlowInfo_UnknownZ;_tmp75.f1=_tmp69;_tmp75;});_tmp74;})));
_LL55:;}else{(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp77;_tmp77.tag=Cyc_CfFlowInfo_UnknownZ;_tmp77.f1=_tmp69;_tmp77;});_tmp76;})));}{
void*_tmp78=Cyc_CfFlowInfo_initlevel(_tmp68,r);_LL63: if((int)_tmp78 != 0)goto
_LL65;_LL64:({void*_tmp79[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp7A="dereference of possibly uninitialized pointer";
_tag_arr(_tmp7A,sizeof(char),_get_zero_arr_size(_tmp7A,46));}),_tag_arr(_tmp79,
sizeof(void*),0));});goto _LL66;_LL65: if((int)_tmp78 != 2)goto _LL67;_LL66: return({
struct _tuple0 _tmp7B;_tmp7B.f1=f;_tmp7B.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp63,
Cyc_CfFlowInfo_unknown_all);_tmp7B;});_LL67: if((int)_tmp78 != 1)goto _LL62;_LL68:
return({struct _tuple0 _tmp7C;_tmp7C.f1=f;_tmp7C.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp63,Cyc_CfFlowInfo_unknown_none);_tmp7C;});_LL62:;}_LL50:;}_LL4E:;_LL4F:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));
_tmp7D[0]=({struct Cyc_Core_Impossible_struct _tmp7E;_tmp7E.tag=Cyc_Core_Impossible;
_tmp7E.f1=({const char*_tmp7F="right deref of non-pointer-type";_tag_arr(_tmp7F,
sizeof(char),_get_zero_arr_size(_tmp7F,32));});_tmp7E;});_tmp7D;}));_LL4B:;}
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp80=(void*)e1->r;
void*_tmp81;struct Cyc_Absyn_Vardecl*_tmp82;void*_tmp83;struct Cyc_Absyn_Vardecl*
_tmp84;void*_tmp85;struct Cyc_Absyn_Vardecl*_tmp86;void*_tmp87;struct Cyc_Absyn_Vardecl*
_tmp88;_LL6A: if(*((int*)_tmp80)!= 1)goto _LL6C;_tmp81=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp80)->f2;if(_tmp81 <= (void*)1  || *((int*)_tmp81)!= 4)goto _LL6C;_tmp82=((
struct Cyc_Absyn_Pat_b_struct*)_tmp81)->f1;_LL6B: _tmp84=_tmp82;goto _LL6D;_LL6C:
if(*((int*)_tmp80)!= 1)goto _LL6E;_tmp83=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp80)->f2;if(_tmp83 <= (void*)1  || *((int*)_tmp83)!= 3)goto _LL6E;_tmp84=((
struct Cyc_Absyn_Local_b_struct*)_tmp83)->f1;_LL6D: _tmp86=_tmp84;goto _LL6F;_LL6E:
if(*((int*)_tmp80)!= 1)goto _LL70;_tmp85=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp80)->f2;if(_tmp85 <= (void*)1  || *((int*)_tmp85)!= 2)goto _LL70;_tmp86=((
struct Cyc_Absyn_Param_b_struct*)_tmp85)->f1;_LL6F: _tmp88=_tmp86;goto _LL71;_LL70:
if(*((int*)_tmp80)!= 1)goto _LL72;_tmp87=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp80)->f2;if(_tmp87 <= (void*)1  || *((int*)_tmp87)!= 0)goto _LL72;_tmp88=((
struct Cyc_Absyn_Global_b_struct*)_tmp87)->f1;_LL71: if(!_tmp88->escapes){void*
_tmp89=(void*)e2->r;void*_tmp8A;struct Cyc_Absyn_Vardecl*_tmp8B;void*_tmp8C;
struct Cyc_Absyn_Vardecl*_tmp8D;void*_tmp8E;struct Cyc_Absyn_Vardecl*_tmp8F;void*
_tmp90;struct Cyc_Absyn_Vardecl*_tmp91;_LL75: if(*((int*)_tmp89)!= 1)goto _LL77;
_tmp8A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp8A <= (void*)1
 || *((int*)_tmp8A)!= 4)goto _LL77;_tmp8B=((struct Cyc_Absyn_Pat_b_struct*)_tmp8A)->f1;
_LL76: _tmp8D=_tmp8B;goto _LL78;_LL77: if(*((int*)_tmp89)!= 1)goto _LL79;_tmp8C=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp8C <= (void*)1  || *((int*)
_tmp8C)!= 3)goto _LL79;_tmp8D=((struct Cyc_Absyn_Local_b_struct*)_tmp8C)->f1;_LL78:
_tmp8F=_tmp8D;goto _LL7A;_LL79: if(*((int*)_tmp89)!= 1)goto _LL7B;_tmp8E=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp8E <= (void*)1  || *((int*)_tmp8E)
!= 2)goto _LL7B;_tmp8F=((struct Cyc_Absyn_Param_b_struct*)_tmp8E)->f1;_LL7A: _tmp91=
_tmp8F;goto _LL7C;_LL7B: if(*((int*)_tmp89)!= 1)goto _LL7D;_tmp90=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp90 <= (void*)1  || *((int*)_tmp90)!= 0)
goto _LL7D;_tmp91=((struct Cyc_Absyn_Global_b_struct*)_tmp90)->f1;_LL7C: if(!_tmp91->escapes){
int found=0;{struct Cyc_List_List*_tmp92=relns;for(0;_tmp92 != 0;_tmp92=_tmp92->tl){
struct Cyc_CfFlowInfo_Reln*_tmp93=(struct Cyc_CfFlowInfo_Reln*)_tmp92->hd;if(
_tmp93->vd == _tmp91){void*_tmp94=(void*)_tmp93->rop;struct Cyc_Absyn_Vardecl*
_tmp95;_LL80: if(*((int*)_tmp94)!= 2)goto _LL82;_tmp95=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp94)->f1;if(!(_tmp95 == _tmp88))goto _LL82;_LL81: return relns;_LL82:;_LL83:
continue;_LL7F:;}}}if(!found)return({struct Cyc_List_List*_tmp96=_cycalloc(
sizeof(*_tmp96));_tmp96->hd=({struct Cyc_CfFlowInfo_Reln*_tmp97=_cycalloc(sizeof(*
_tmp97));_tmp97->vd=_tmp91;_tmp97->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp99;_tmp99.tag=2;_tmp99.f1=_tmp88;_tmp99;});_tmp98;}));_tmp97;});_tmp96->tl=
relns;_tmp96;});}return relns;_LL7D:;_LL7E: return relns;_LL74:;}else{return relns;}
_LL72:;_LL73: return relns;_LL69:;}struct _tuple6{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){struct Cyc_Dict_Dict*d;struct Cyc_List_List*
relns;{void*_tmp9A=inflow;struct Cyc_Dict_Dict*_tmp9B;struct Cyc_List_List*_tmp9C;
_LL85: if((int)_tmp9A != 0)goto _LL87;_LL86: return({struct _tuple0 _tmp9D;_tmp9D.f1=(
void*)0;_tmp9D.f2=Cyc_CfFlowInfo_unknown_all;_tmp9D;});_LL87: if(_tmp9A <= (void*)
1  || *((int*)_tmp9A)!= 0)goto _LL84;_tmp9B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp9A)->f1;_tmp9C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp9A)->f2;_LL88:
d=_tmp9B;relns=_tmp9C;_LL84:;}{void*_tmp9E=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp9F;void*_tmpA0;struct Cyc_Absyn_Exp*_tmpA1;struct Cyc_Absyn_Exp*_tmpA2;struct
Cyc_Absyn_Exp*_tmpA3;void*_tmpA4;void*_tmpA5;int _tmpA6;void*_tmpA7;void*_tmpA8;
struct Cyc_List_List*_tmpA9;void*_tmpAA;void*_tmpAB;struct Cyc_Absyn_Vardecl*
_tmpAC;void*_tmpAD;struct Cyc_Absyn_Vardecl*_tmpAE;void*_tmpAF;struct Cyc_Absyn_Vardecl*
_tmpB0;struct Cyc_List_List*_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_Exp*
_tmpB3;struct Cyc_Core_Opt*_tmpB4;struct Cyc_Core_Opt _tmpB5;struct Cyc_Absyn_Exp*
_tmpB6;struct Cyc_Absyn_Exp*_tmpB7;struct Cyc_Core_Opt*_tmpB8;struct Cyc_Absyn_Exp*
_tmpB9;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_Absyn_Exp*
_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_List_List*_tmpBE;struct Cyc_Absyn_MallocInfo
_tmpBF;int _tmpC0;struct Cyc_Absyn_Exp*_tmpC1;void**_tmpC2;struct Cyc_Absyn_Exp*
_tmpC3;int _tmpC4;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*
_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpC9;struct _tagged_arr*
_tmpCA;struct Cyc_Absyn_Exp*_tmpCB;struct _tagged_arr*_tmpCC;struct Cyc_Absyn_Exp*
_tmpCD;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*
_tmpD0;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*
_tmpD3;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD5;struct Cyc_List_List*
_tmpD6;struct Cyc_List_List*_tmpD7;struct Cyc_List_List*_tmpD8;struct Cyc_List_List*
_tmpD9;struct Cyc_List_List*_tmpDA;struct Cyc_Absyn_Vardecl*_tmpDB;struct Cyc_Absyn_Exp*
_tmpDC;struct Cyc_Absyn_Exp*_tmpDD;int _tmpDE;struct Cyc_Absyn_Stmt*_tmpDF;void*
_tmpE0;_LL8A: if(*((int*)_tmp9E)!= 15)goto _LL8C;_tmp9F=((struct Cyc_Absyn_Cast_e_struct*)
_tmp9E)->f2;_tmpA0=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp9E)->f4;if((int)
_tmpA0 != 2)goto _LL8C;_LL8B: {void*_tmpE2;void*_tmpE3;struct _tuple0 _tmpE1=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp9F);_tmpE2=_tmpE1.f1;_tmpE3=_tmpE1.f2;{void*_tmpE5;void*_tmpE6;
struct _tuple0 _tmpE4=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmpE2,_tmp9F,
_tmpE3);_tmpE5=_tmpE4.f1;_tmpE6=_tmpE4.f2;return({struct _tuple0 _tmpE7;_tmpE7.f1=
_tmpE5;_tmpE7.f2=_tmpE3;_tmpE7;});}}_LL8C: if(*((int*)_tmp9E)!= 15)goto _LL8E;
_tmpA1=((struct Cyc_Absyn_Cast_e_struct*)_tmp9E)->f2;_LL8D: _tmpA2=_tmpA1;goto
_LL8F;_LL8E: if(*((int*)_tmp9E)!= 13)goto _LL90;_tmpA2=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp9E)->f1;_LL8F: _tmpA3=_tmpA2;goto _LL91;_LL90: if(*((int*)_tmp9E)!= 14)goto
_LL92;_tmpA3=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp9E)->f1;_LL91: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpA3);_LL92: if(*((int*)_tmp9E)!= 0)goto _LL94;_tmpA4=(void*)((struct
Cyc_Absyn_Const_e_struct*)_tmp9E)->f1;if((int)_tmpA4 != 0)goto _LL94;_LL93: goto
_LL95;_LL94: if(*((int*)_tmp9E)!= 0)goto _LL96;_tmpA5=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp9E)->f1;if(_tmpA5 <= (void*)1  || *((int*)_tmpA5)!= 2)goto _LL96;_tmpA6=((
struct Cyc_Absyn_Int_c_struct*)_tmpA5)->f2;if(_tmpA6 != 0)goto _LL96;_LL95: return({
struct _tuple0 _tmpE8;_tmpE8.f1=inflow;_tmpE8.f2=(void*)0;_tmpE8;});_LL96: if(*((
int*)_tmp9E)!= 0)goto _LL98;_tmpA7=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp9E)->f1;if(_tmpA7 <= (void*)1  || *((int*)_tmpA7)!= 2)goto _LL98;_LL97: goto
_LL99;_LL98: if(*((int*)_tmp9E)!= 1)goto _LL9A;_tmpA8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp9E)->f2;if(_tmpA8 <= (void*)1  || *((int*)_tmpA8)!= 1)goto _LL9A;_LL99: return({
struct _tuple0 _tmpE9;_tmpE9.f1=inflow;_tmpE9.f2=(void*)1;_tmpE9;});_LL9A: if(*((
int*)_tmp9E)!= 32)goto _LL9C;_tmpA9=((struct Cyc_Absyn_Tunion_e_struct*)_tmp9E)->f1;
if(_tmpA9 != 0)goto _LL9C;_LL9B: goto _LL9D;_LL9C: if(*((int*)_tmp9E)!= 0)goto _LL9E;
_LL9D: goto _LL9F;_LL9E: if(*((int*)_tmp9E)!= 19)goto _LLA0;_LL9F: goto _LLA1;_LLA0:
if(*((int*)_tmp9E)!= 18)goto _LLA2;_LLA1: goto _LLA3;_LLA2: if(*((int*)_tmp9E)!= 20)
goto _LLA4;_LLA3: goto _LLA5;_LLA4: if(*((int*)_tmp9E)!= 21)goto _LLA6;_LLA5: goto
_LLA7;_LLA6: if(*((int*)_tmp9E)!= 34)goto _LLA8;_LLA7: goto _LLA9;_LLA8: if(*((int*)
_tmp9E)!= 33)goto _LLAA;_LLA9: return({struct _tuple0 _tmpEA;_tmpEA.f1=inflow;_tmpEA.f2=
Cyc_CfFlowInfo_unknown_all;_tmpEA;});_LLAA: if(*((int*)_tmp9E)!= 1)goto _LLAC;
_tmpAA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9E)->f2;if(_tmpAA <= (void*)1
 || *((int*)_tmpAA)!= 0)goto _LLAC;_LLAB: return({struct _tuple0 _tmpEB;_tmpEB.f1=
inflow;_tmpEB.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmpEB;});_LLAC: if(*((int*)
_tmp9E)!= 1)goto _LLAE;_tmpAB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9E)->f2;
if(_tmpAB <= (void*)1  || *((int*)_tmpAB)!= 2)goto _LLAE;_tmpAC=((struct Cyc_Absyn_Param_b_struct*)
_tmpAB)->f1;_LLAD: _tmpAE=_tmpAC;goto _LLAF;_LLAE: if(*((int*)_tmp9E)!= 1)goto _LLB0;
_tmpAD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9E)->f2;if(_tmpAD <= (void*)1
 || *((int*)_tmpAD)!= 3)goto _LLB0;_tmpAE=((struct Cyc_Absyn_Local_b_struct*)
_tmpAD)->f1;_LLAF: _tmpB0=_tmpAE;goto _LLB1;_LLB0: if(*((int*)_tmp9E)!= 1)goto _LLB2;
_tmpAF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9E)->f2;if(_tmpAF <= (void*)1
 || *((int*)_tmpAF)!= 4)goto _LLB2;_tmpB0=((struct Cyc_Absyn_Pat_b_struct*)_tmpAF)->f1;
_LLB1: return({struct _tuple0 _tmpEC;_tmpEC.f1=inflow;_tmpEC.f2=Cyc_Dict_lookup(d,(
void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmpED=_cycalloc(sizeof(*_tmpED));
_tmpED[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmpEE;_tmpEE.tag=0;_tmpEE.f1=
_tmpB0;_tmpEE;});_tmpED;}));_tmpEC;});_LLB2: if(*((int*)_tmp9E)!= 3)goto _LLB4;
_tmpB1=((struct Cyc_Absyn_Primop_e_struct*)_tmp9E)->f2;_LLB3: return Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmpB1);_LLB4: if(*((int*)_tmp9E)!= 5)goto _LLB6;_tmpB2=((struct Cyc_Absyn_Increment_e_struct*)
_tmp9E)->f1;_LLB5: {void*_tmpF1;struct _tuple0 _tmpF0=Cyc_NewControlFlow_anal_use_ints(
env,inflow,({struct Cyc_Absyn_Exp*_tmpEF[1];_tmpEF[0]=_tmpB2;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpEF,sizeof(struct Cyc_Absyn_Exp*),1));}));
_tmpF1=_tmpF0.f1;{void*_tmpF3;struct _tuple0 _tmpF2=Cyc_NewControlFlow_anal_Lexp(
env,_tmpF1,_tmpB2);_tmpF3=_tmpF2.f2;{struct _tuple0 _tmpF5=({struct _tuple0 _tmpF4;
_tmpF4.f1=_tmpF3;_tmpF4.f2=_tmpF1;_tmpF4;});void*_tmpF6;struct Cyc_CfFlowInfo_Place*
_tmpF7;void*_tmpF8;struct Cyc_Dict_Dict*_tmpF9;struct Cyc_List_List*_tmpFA;_LLF1:
_tmpF6=_tmpF5.f1;if(_tmpF6 <= (void*)1  || *((int*)_tmpF6)!= 0)goto _LLF3;_tmpF7=((
struct Cyc_CfFlowInfo_PlaceL_struct*)_tmpF6)->f1;_tmpF8=_tmpF5.f2;if(_tmpF8 <= (
void*)1  || *((int*)_tmpF8)!= 0)goto _LLF3;_tmpF9=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpF8)->f1;_tmpFA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpF8)->f2;_LLF2:
_tmpFA=Cyc_CfFlowInfo_reln_kill_exp(_tmpFA,_tmpB2);_tmpF1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmpFC;_tmpFC.tag=0;_tmpFC.f1=Cyc_CfFlowInfo_assign_place(_tmpB2->loc,_tmpF9,env->all_changed,
_tmpF7,Cyc_CfFlowInfo_unknown_all);_tmpFC.f2=_tmpFA;_tmpFC;});_tmpFB;});goto
_LLF0;_LLF3:;_LLF4: goto _LLF0;_LLF0:;}return({struct _tuple0 _tmpFD;_tmpFD.f1=
_tmpF1;_tmpFD.f2=Cyc_CfFlowInfo_unknown_all;_tmpFD;});}}_LLB6: if(*((int*)_tmp9E)
!= 4)goto _LLB8;_tmpB3=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f1;_tmpB4=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f2;if(_tmpB4 == 0)goto _LLB8;_tmpB5=*
_tmpB4;_tmpB6=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f3;_LLB7: {void*
_tmp100;struct _tuple0 _tmpFF=Cyc_NewControlFlow_anal_use_ints(env,inflow,({struct
Cyc_Absyn_Exp*_tmpFE[2];_tmpFE[1]=_tmpB6;_tmpFE[0]=_tmpB3;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpFE,sizeof(struct Cyc_Absyn_Exp*),2));}));
_tmp100=_tmpFF.f1;{void*_tmp102;struct _tuple0 _tmp101=Cyc_NewControlFlow_anal_Lexp(
env,_tmp100,e);_tmp102=_tmp101.f2;{struct _tuple0 _tmp104=({struct _tuple0 _tmp103;
_tmp103.f1=_tmp102;_tmp103.f2=_tmp100;_tmp103;});void*_tmp105;struct Cyc_CfFlowInfo_Place*
_tmp106;void*_tmp107;struct Cyc_Dict_Dict*_tmp108;struct Cyc_List_List*_tmp109;
_LLF6: _tmp105=_tmp104.f1;if(_tmp105 <= (void*)1  || *((int*)_tmp105)!= 0)goto _LLF8;
_tmp106=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp105)->f1;_tmp107=_tmp104.f2;
if(_tmp107 <= (void*)1  || *((int*)_tmp107)!= 0)goto _LLF8;_tmp108=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp107)->f1;_tmp109=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp107)->f2;
_LLF7: _tmp109=Cyc_CfFlowInfo_reln_kill_exp(_tmp109,_tmpB3);_tmp100=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp10A=_cycalloc(sizeof(*_tmp10A));
_tmp10A[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp10B;_tmp10B.tag=0;
_tmp10B.f1=Cyc_CfFlowInfo_assign_place(_tmpB3->loc,_tmp108,env->all_changed,
_tmp106,Cyc_CfFlowInfo_unknown_all);_tmp10B.f2=_tmp109;_tmp10B;});_tmp10A;});
goto _LLF5;_LLF8:;_LLF9: goto _LLF5;_LLF5:;}return({struct _tuple0 _tmp10C;_tmp10C.f1=
_tmp100;_tmp10C.f2=Cyc_CfFlowInfo_unknown_all;_tmp10C;});}}_LLB8: if(*((int*)
_tmp9E)!= 4)goto _LLBA;_tmpB7=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f1;
_tmpB8=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f2;if(_tmpB8 != 0)goto _LLBA;
_tmpB9=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f3;_LLB9: {struct Cyc_Set_Set**
_tmp10D=env->all_changed;while(1){env->all_changed=({struct Cyc_Set_Set**_tmp10E=
_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=Cyc_CfFlowInfo_mt_place_set();_tmp10E;});{
void*_tmp110;void*_tmp111;struct _tuple0 _tmp10F=Cyc_NewControlFlow_anal_Lexp(env,
inflow,_tmpB7);_tmp110=_tmp10F.f1;_tmp111=_tmp10F.f2;{struct Cyc_Set_Set*_tmp112=*((
struct Cyc_Set_Set**)_check_null(env->all_changed));env->all_changed=({struct Cyc_Set_Set**
_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp113;});{void*_tmp115;void*_tmp116;struct _tuple0 _tmp114=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpB9);_tmp115=_tmp114.f1;_tmp116=_tmp114.f2;{struct Cyc_Set_Set*
_tmp117=*((struct Cyc_Set_Set**)_check_null(env->all_changed));void*_tmp118=Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& _tmp112,_tmp110,_tmp115,_tmp112,_tmp117);void*_tmp119=Cyc_CfFlowInfo_join_flow(
_tmp10D,inflow,_tmp118);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp119,inflow)){
if(_tmp10D == 0)env->all_changed=0;else{*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*
_tmp10D,((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp112,_tmp117));}{void*_tmp11A=_tmp118;struct Cyc_Dict_Dict*_tmp11B;struct Cyc_List_List*
_tmp11C;_LLFB: if((int)_tmp11A != 0)goto _LLFD;_LLFC: return({struct _tuple0 _tmp11D;
_tmp11D.f1=(void*)0;_tmp11D.f2=_tmp116;_tmp11D;});_LLFD: if(_tmp11A <= (void*)1
 || *((int*)_tmp11A)!= 0)goto _LLFA;_tmp11B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp11A)->f1;_tmp11C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp11A)->f2;
_LLFE: {void*_tmp11E=_tmp111;struct Cyc_CfFlowInfo_Place*_tmp11F;_LL100: if(
_tmp11E <= (void*)1  || *((int*)_tmp11E)!= 0)goto _LL102;_tmp11F=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp11E)->f1;_LL101: _tmp11B=Cyc_CfFlowInfo_assign_place(e->loc,_tmp11B,env->all_changed,
_tmp11F,_tmp116);_tmp11C=Cyc_CfFlowInfo_reln_assign_exp(_tmp11C,_tmpB7,_tmpB9);
_tmp118=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp120=_cycalloc(
sizeof(*_tmp120));_tmp120[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp121;
_tmp121.tag=0;_tmp121.f1=_tmp11B;_tmp121.f2=_tmp11C;_tmp121;});_tmp120;});Cyc_NewControlFlow_update_tryflow(
env,_tmp118);return({struct _tuple0 _tmp122;_tmp122.f1=_tmp118;_tmp122.f2=_tmp116;
_tmp122;});_LL102: if((int)_tmp11E != 0)goto _LLFF;_LL103: return({struct _tuple0
_tmp123;_tmp123.f1=Cyc_NewControlFlow_use_Rval(env,_tmpB9->loc,_tmp118,_tmp116);
_tmp123.f2=_tmp116;_tmp123;});_LLFF:;}_LLFA:;}}inflow=_tmp119;}}}}}}_LLBA: if(*((
int*)_tmp9E)!= 9)goto _LLBC;_tmpBA=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp9E)->f1;
_tmpBB=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp9E)->f2;_LLBB: return Cyc_NewControlFlow_anal_Rexp(
env,(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpBA)).f1,_tmpBB);_LLBC: if(*((int*)
_tmp9E)!= 12)goto _LLBE;_tmpBC=((struct Cyc_Absyn_Throw_e_struct*)_tmp9E)->f1;
_LLBD: {void*_tmp125;void*_tmp126;struct _tuple0 _tmp124=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpBC);_tmp125=_tmp124.f1;_tmp126=_tmp124.f2;Cyc_NewControlFlow_use_Rval(
env,_tmpBC->loc,_tmp125,_tmp126);return({struct _tuple0 _tmp127;_tmp127.f1=(void*)
0;_tmp127.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp127;});}_LLBE: if(*((int*)
_tmp9E)!= 11)goto _LLC0;_tmpBD=((struct Cyc_Absyn_FnCall_e_struct*)_tmp9E)->f1;
_tmpBE=((struct Cyc_Absyn_FnCall_e_struct*)_tmp9E)->f2;_LLBF: {void*_tmp12A;
struct Cyc_List_List*_tmp12B;struct _tuple5 _tmp129=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_List_List*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->hd=
_tmpBD;_tmp128->tl=_tmpBE;_tmp128;}));_tmp12A=_tmp129.f1;_tmp12B=_tmp129.f2;{
void*_tmp12C=Cyc_NewControlFlow_use_Rval(env,_tmpBD->loc,_tmp12A,(void*)((struct
Cyc_List_List*)_check_null(_tmp12B))->hd);_tmp12B=_tmp12B->tl;{struct Cyc_List_List*
init_params=0;{void*_tmp12D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpBD->topt))->v);struct Cyc_Absyn_PtrInfo _tmp12E;void*_tmp12F;
_LL105: if(_tmp12D <= (void*)3  || *((int*)_tmp12D)!= 4)goto _LL107;_tmp12E=((struct
Cyc_Absyn_PointerType_struct*)_tmp12D)->f1;_tmp12F=(void*)_tmp12E.elt_typ;_LL106:{
void*_tmp130=Cyc_Tcutil_compress(_tmp12F);struct Cyc_Absyn_FnInfo _tmp131;struct
Cyc_List_List*_tmp132;_LL10A: if(_tmp130 <= (void*)3  || *((int*)_tmp130)!= 8)goto
_LL10C;_tmp131=((struct Cyc_Absyn_FnType_struct*)_tmp130)->f1;_tmp132=_tmp131.attributes;
_LL10B: for(0;_tmp132 != 0;_tmp132=_tmp132->tl){void*_tmp133=(void*)_tmp132->hd;
int _tmp134;_LL10F: if(_tmp133 <= (void*)17  || *((int*)_tmp133)!= 4)goto _LL111;
_tmp134=((struct Cyc_Absyn_Initializes_att_struct*)_tmp133)->f1;_LL110:
init_params=({struct Cyc_List_List*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->hd=(
void*)_tmp134;_tmp135->tl=init_params;_tmp135;});goto _LL10E;_LL111:;_LL112: goto
_LL10E;_LL10E:;}goto _LL109;_LL10C:;_LL10D:({void*_tmp136[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp137="anal_Rexp: bad function type";
_tag_arr(_tmp137,sizeof(char),_get_zero_arr_size(_tmp137,29));}),_tag_arr(
_tmp136,sizeof(void*),0));});_LL109:;}goto _LL104;_LL107:;_LL108:({void*_tmp138[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp139="anal_Rexp: bad function type";_tag_arr(_tmp139,sizeof(char),
_get_zero_arr_size(_tmp139,29));}),_tag_arr(_tmp138,sizeof(void*),0));});_LL104:;}{
int i=1;for(0;_tmp12B != 0;(((_tmp12B=_tmp12B->tl,_tmpBE=((struct Cyc_List_List*)
_check_null(_tmpBE))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(
init_params,i)){_tmp12C=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpBE))->hd)->loc,_tmp12C,(void*)_tmp12B->hd);
continue;}{void*_tmp13A=_tmp12A;struct Cyc_Dict_Dict*_tmp13B;_LL114: if((int)
_tmp13A != 0)goto _LL116;_LL115: goto _LL113;_LL116: if(_tmp13A <= (void*)1  || *((int*)
_tmp13A)!= 0)goto _LL113;_tmp13B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp13A)->f1;_LL117: if(Cyc_CfFlowInfo_initlevel(_tmp13B,(void*)_tmp12B->hd)== (
void*)0)({void*_tmp13C[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpBE))->hd)->loc,({const char*_tmp13D="expression may not be initialized";
_tag_arr(_tmp13D,sizeof(char),_get_zero_arr_size(_tmp13D,34));}),_tag_arr(
_tmp13C,sizeof(void*),0));});{void*_tmp13E=_tmp12C;struct Cyc_Dict_Dict*_tmp13F;
struct Cyc_List_List*_tmp140;_LL119: if((int)_tmp13E != 0)goto _LL11B;_LL11A: goto
_LL118;_LL11B: if(_tmp13E <= (void*)1  || *((int*)_tmp13E)!= 0)goto _LL118;_tmp13F=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp13E)->f1;_tmp140=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp13E)->f2;_LL11C: {struct Cyc_Dict_Dict*_tmp141=Cyc_CfFlowInfo_escape_deref(
_tmp13F,env->all_changed,(void*)_tmp12B->hd);{void*_tmp142=(void*)_tmp12B->hd;
struct Cyc_CfFlowInfo_Place*_tmp143;_LL11E: if(_tmp142 <= (void*)3  || *((int*)
_tmp142)!= 2)goto _LL120;_tmp143=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp142)->f1;
_LL11F:{void*_tmp144=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpBE))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp145;void*_tmp146;_LL123: if(_tmp144 <= (void*)3  || *((
int*)_tmp144)!= 4)goto _LL125;_tmp145=((struct Cyc_Absyn_PointerType_struct*)
_tmp144)->f1;_tmp146=(void*)_tmp145.elt_typ;_LL124: _tmp141=Cyc_CfFlowInfo_assign_place(((
struct Cyc_Absyn_Exp*)_tmpBE->hd)->loc,_tmp141,env->all_changed,_tmp143,Cyc_CfFlowInfo_typ_to_absrval(
_tmp146,Cyc_CfFlowInfo_esc_all));goto _LL122;_LL125:;_LL126:({void*_tmp147[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp148="anal_Rexp: bad type for initialized arg";_tag_arr(_tmp148,sizeof(char),
_get_zero_arr_size(_tmp148,40));}),_tag_arr(_tmp147,sizeof(void*),0));});_LL122:;}
goto _LL11D;_LL120:;_LL121: goto _LL11D;_LL11D:;}_tmp12C=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp14A;_tmp14A.tag=0;_tmp14A.f1=_tmp141;_tmp14A.f2=_tmp140;_tmp14A;});_tmp149;});
goto _LL118;}_LL118:;}goto _LL113;_LL113:;}}}if(Cyc_Tcutil_is_noreturn((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpBD->topt))->v))return({struct _tuple0 _tmp14B;
_tmp14B.f1=(void*)0;_tmp14B.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp14B;});else{return({
struct _tuple0 _tmp14C;_tmp14C.f1=_tmp12C;_tmp14C.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp14C;});}}}}_LLC0: if(*((int*)_tmp9E)!= 35)goto _LLC2;_tmpBF=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp9E)->f1;_tmpC0=_tmpBF.is_calloc;_tmpC1=_tmpBF.rgn;_tmpC2=_tmpBF.elt_type;
_tmpC3=_tmpBF.num_elts;_tmpC4=_tmpBF.fat_result;_LLC1: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp15E;_tmp15E.tag=1;_tmp15E.f1=_tmpC3;
_tmp15E.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp15E;});_tmp15D;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->root=(void*)root;_tmp15C->fields=0;
_tmp15C;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp15A=
_cycalloc(sizeof(*_tmp15A));_tmp15A[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp15B;_tmp15B.tag=2;_tmp15B.f1=place;_tmp15B;});_tmp15A;});void*place_val=
_tmpC4?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(*((void**)_check_null(_tmpC2)),Cyc_CfFlowInfo_unknown_none);
void*outflow;if(env->all_changed != 0)*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((
struct Cyc_Set_Set**)_check_null(env->all_changed)),place);if(_tmpC1 != 0){void*
_tmp14F;struct Cyc_List_List*_tmp150;struct _tuple5 _tmp14E=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp14D[2];_tmp14D[1]=_tmpC3;_tmp14D[0]=(struct
Cyc_Absyn_Exp*)_tmpC1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp14D,sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp14F=_tmp14E.f1;_tmp150=
_tmp14E.f2;outflow=_tmp14F;}else{void*_tmp152;struct _tuple0 _tmp151=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpC3);_tmp152=_tmp151.f1;outflow=_tmp152;}{void*_tmp153=outflow;
struct Cyc_Dict_Dict*_tmp154;struct Cyc_List_List*_tmp155;_LL128: if((int)_tmp153 != 
0)goto _LL12A;_LL129: return({struct _tuple0 _tmp156;_tmp156.f1=outflow;_tmp156.f2=
rval;_tmp156;});_LL12A: if(_tmp153 <= (void*)1  || *((int*)_tmp153)!= 0)goto _LL127;
_tmp154=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp153)->f1;_tmp155=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp153)->f2;_LL12B: return({struct _tuple0
_tmp157;_tmp157.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp158=
_cycalloc(sizeof(*_tmp158));_tmp158[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp159;_tmp159.tag=0;_tmp159.f1=Cyc_Dict_insert(_tmp154,root,place_val);_tmp159.f2=
_tmp155;_tmp159;});_tmp158;});_tmp157.f2=rval;_tmp157;});_LL127:;}}_LLC2: if(*((
int*)_tmp9E)!= 17)goto _LLC4;_tmpC5=((struct Cyc_Absyn_New_e_struct*)_tmp9E)->f1;
_tmpC6=((struct Cyc_Absyn_New_e_struct*)_tmp9E)->f2;_LLC3: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp171;_tmp171.tag=1;_tmp171.f1=_tmpC6;
_tmp171.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp171;});_tmp170;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->root=(void*)root;_tmp16F->fields=0;
_tmp16F;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp16D=
_cycalloc(sizeof(*_tmp16D));_tmp16D[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp16E;_tmp16E.tag=2;_tmp16E.f1=place;_tmp16E;});_tmp16D;});if(env->all_changed
!= 0)*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(env->all_changed)),place);{void*outflow;void*place_val;if(_tmpC5 != 0){
void*_tmp161;struct Cyc_List_List*_tmp162;struct _tuple5 _tmp160=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp15F[2];_tmp15F[1]=_tmpC6;_tmp15F[0]=(struct
Cyc_Absyn_Exp*)_tmpC5;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp15F,sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp161=_tmp160.f1;_tmp162=
_tmp160.f2;outflow=_tmp161;place_val=(void*)((struct Cyc_List_List*)_check_null(((
struct Cyc_List_List*)_check_null(_tmp162))->tl))->hd;}else{void*_tmp164;void*
_tmp165;struct _tuple0 _tmp163=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpC6);
_tmp164=_tmp163.f1;_tmp165=_tmp163.f2;outflow=_tmp164;place_val=_tmp165;}{void*
_tmp166=outflow;struct Cyc_Dict_Dict*_tmp167;struct Cyc_List_List*_tmp168;_LL12D:
if((int)_tmp166 != 0)goto _LL12F;_LL12E: return({struct _tuple0 _tmp169;_tmp169.f1=
outflow;_tmp169.f2=rval;_tmp169;});_LL12F: if(_tmp166 <= (void*)1  || *((int*)
_tmp166)!= 0)goto _LL12C;_tmp167=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp166)->f1;_tmp168=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp166)->f2;
_LL130: return({struct _tuple0 _tmp16A;_tmp16A.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp16C;_tmp16C.tag=0;_tmp16C.f1=Cyc_Dict_insert(_tmp167,root,place_val);_tmp16C.f2=
_tmp168;_tmp16C;});_tmp16B;});_tmp16A.f2=rval;_tmp16A;});_LL12C:;}}}_LLC4: if(*((
int*)_tmp9E)!= 16)goto _LLC6;_tmpC7=((struct Cyc_Absyn_Address_e_struct*)_tmp9E)->f1;
_LLC5: {void*_tmp173;void*_tmp174;struct _tuple0 _tmp172=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmpC7);_tmp173=_tmp172.f1;_tmp174=_tmp172.f2;{void*_tmp175=_tmp174;
struct Cyc_CfFlowInfo_Place*_tmp176;_LL132: if((int)_tmp175 != 0)goto _LL134;_LL133:
return({struct _tuple0 _tmp177;_tmp177.f1=_tmp173;_tmp177.f2=(void*)1;_tmp177;});
_LL134: if(_tmp175 <= (void*)1  || *((int*)_tmp175)!= 0)goto _LL131;_tmp176=((struct
Cyc_CfFlowInfo_PlaceL_struct*)_tmp175)->f1;_LL135: return({struct _tuple0 _tmp178;
_tmp178.f1=_tmp173;_tmp178.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp17A;_tmp17A.tag=2;_tmp17A.f1=_tmp176;_tmp17A;});_tmp179;});_tmp178;});_LL131:;}}
_LLC6: if(*((int*)_tmp9E)!= 22)goto _LLC8;_tmpC8=((struct Cyc_Absyn_Deref_e_struct*)
_tmp9E)->f1;_LLC7: {void*_tmp17C;void*_tmp17D;struct _tuple0 _tmp17B=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpC8);_tmp17C=_tmp17B.f1;_tmp17D=_tmp17B.f2;return Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp17C,_tmpC8,_tmp17D);}_LLC8: if(*((int*)_tmp9E)!= 23)goto _LLCA;
_tmpC9=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp9E)->f1;_tmpCA=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp9E)->f2;_LLC9: {void*_tmp17F;void*_tmp180;struct _tuple0 _tmp17E=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpC9);_tmp17F=_tmp17E.f1;_tmp180=_tmp17E.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpC9->topt))->v))return({struct _tuple0
_tmp181;_tmp181.f1=_tmp17F;_tmp181.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp181;});{
void*_tmp182=_tmp180;struct Cyc_Dict_Dict*_tmp183;_LL137: if(_tmp182 <= (void*)3
 || *((int*)_tmp182)!= 4)goto _LL139;_tmp183=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp182)->f1;_LL138: return({struct _tuple0 _tmp184;_tmp184.f1=_tmp17F;_tmp184.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp183,
_tmpCA);_tmp184;});_LL139:;_LL13A:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185[0]=({struct Cyc_Core_Impossible_struct
_tmp186;_tmp186.tag=Cyc_Core_Impossible;_tmp186.f1=({const char*_tmp187="anal_Rexp: AggrMember";
_tag_arr(_tmp187,sizeof(char),_get_zero_arr_size(_tmp187,22));});_tmp186;});
_tmp185;}));_LL136:;}}_LLCA: if(*((int*)_tmp9E)!= 24)goto _LLCC;_tmpCB=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp9E)->f1;_tmpCC=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9E)->f2;_LLCB: {
void*_tmp189;void*_tmp18A;struct _tuple0 _tmp188=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpCB);_tmp189=_tmp188.f1;_tmp18A=_tmp188.f2;{void*_tmp18C;void*_tmp18D;
struct _tuple0 _tmp18B=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp189,_tmpCB,
_tmp18A);_tmp18C=_tmp18B.f1;_tmp18D=_tmp18B.f2;{void*_tmp18E=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpCB->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp18F;void*_tmp190;_LL13C: if(_tmp18E <= (void*)3  || *((int*)_tmp18E)!= 4)goto
_LL13E;_tmp18F=((struct Cyc_Absyn_PointerType_struct*)_tmp18E)->f1;_tmp190=(void*)
_tmp18F.elt_typ;_LL13D: if(Cyc_Absyn_is_union_type(_tmp190))return({struct _tuple0
_tmp191;_tmp191.f1=_tmp18C;_tmp191.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp191;});
goto _LL13B;_LL13E:;_LL13F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192[0]=({struct Cyc_Core_Impossible_struct
_tmp193;_tmp193.tag=Cyc_Core_Impossible;_tmp193.f1=({const char*_tmp194="anal_Rexp: AggrArrow ptr";
_tag_arr(_tmp194,sizeof(char),_get_zero_arr_size(_tmp194,25));});_tmp193;});
_tmp192;}));_LL13B:;}{void*_tmp195=_tmp18D;struct Cyc_Dict_Dict*_tmp196;_LL141:
if(_tmp195 <= (void*)3  || *((int*)_tmp195)!= 4)goto _LL143;_tmp196=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp195)->f1;_LL142: return({struct _tuple0 _tmp197;_tmp197.f1=_tmp18C;_tmp197.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp196,
_tmpCC);_tmp197;});_LL143:;_LL144:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp198=_cycalloc(sizeof(*_tmp198));_tmp198[0]=({struct Cyc_Core_Impossible_struct
_tmp199;_tmp199.tag=Cyc_Core_Impossible;_tmp199.f1=({const char*_tmp19A="anal_Rexp: AggrArrow";
_tag_arr(_tmp19A,sizeof(char),_get_zero_arr_size(_tmp19A,21));});_tmp199;});
_tmp198;}));_LL140:;}}}_LLCC: if(*((int*)_tmp9E)!= 6)goto _LLCE;_tmpCD=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp9E)->f1;_tmpCE=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9E)->f2;_tmpCF=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp9E)->f3;_LLCD: {void*_tmp19C;void*
_tmp19D;struct _tuple0 _tmp19B=Cyc_NewControlFlow_anal_test(env,inflow,_tmpCD);
_tmp19C=_tmp19B.f1;_tmp19D=_tmp19B.f2;{struct _tuple0 _tmp19E=Cyc_NewControlFlow_anal_Rexp(
env,_tmp19C,_tmpCE);struct _tuple0 _tmp19F=Cyc_NewControlFlow_anal_Rexp(env,
_tmp19D,_tmpCF);return Cyc_CfFlowInfo_join_flow_and_rval(env->all_changed,_tmp19E,
_tmp19F);}}_LLCE: if(*((int*)_tmp9E)!= 7)goto _LLD0;_tmpD0=((struct Cyc_Absyn_And_e_struct*)
_tmp9E)->f1;_tmpD1=((struct Cyc_Absyn_And_e_struct*)_tmp9E)->f2;_LLCF: {void*
_tmp1A1;void*_tmp1A2;struct _tuple0 _tmp1A0=Cyc_NewControlFlow_anal_test(env,
inflow,_tmpD0);_tmp1A1=_tmp1A0.f1;_tmp1A2=_tmp1A0.f2;{struct _tuple0 _tmp1A3=Cyc_NewControlFlow_anal_Rexp(
env,_tmp1A1,_tmpD1);struct _tuple0 _tmp1A4=({struct _tuple0 _tmp1A5;_tmp1A5.f1=
_tmp1A2;_tmp1A5.f2=(void*)((void*)0);_tmp1A5;});return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp1A3,_tmp1A4);}}_LLD0: if(*((int*)_tmp9E)!= 8)goto _LLD2;
_tmpD2=((struct Cyc_Absyn_Or_e_struct*)_tmp9E)->f1;_tmpD3=((struct Cyc_Absyn_Or_e_struct*)
_tmp9E)->f2;_LLD1: {void*_tmp1A7;void*_tmp1A8;struct _tuple0 _tmp1A6=Cyc_NewControlFlow_anal_test(
env,inflow,_tmpD2);_tmp1A7=_tmp1A6.f1;_tmp1A8=_tmp1A6.f2;{struct _tuple0 _tmp1A9=
Cyc_NewControlFlow_anal_Rexp(env,_tmp1A8,_tmpD3);struct _tuple0 _tmp1AA=({struct
_tuple0 _tmp1AB;_tmp1AB.f1=_tmp1A7;_tmp1AB.f2=(void*)((void*)1);_tmp1AB;});return
Cyc_CfFlowInfo_join_flow_and_rval(env->all_changed,_tmp1A9,_tmp1AA);}}_LLD2: if(*((
int*)_tmp9E)!= 25)goto _LLD4;_tmpD4=((struct Cyc_Absyn_Subscript_e_struct*)_tmp9E)->f1;
_tmpD5=((struct Cyc_Absyn_Subscript_e_struct*)_tmp9E)->f2;_LLD3: {void*_tmp1AE;
struct Cyc_List_List*_tmp1AF;struct _tuple5 _tmp1AD=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp1AC[2];_tmp1AC[1]=_tmpD5;_tmp1AC[0]=_tmpD4;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1AC,
sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp1AE=_tmp1AD.f1;_tmp1AF=_tmp1AD.f2;{void*
_tmp1B0=_tmp1AE;{void*_tmp1B1=_tmp1AE;struct Cyc_Dict_Dict*_tmp1B2;struct Cyc_List_List*
_tmp1B3;_LL146: if(_tmp1B1 <= (void*)1  || *((int*)_tmp1B1)!= 0)goto _LL148;_tmp1B2=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1B1)->f1;_tmp1B3=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1B1)->f2;_LL147: if(Cyc_CfFlowInfo_initlevel(_tmp1B2,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp1AF))->tl))->hd)== (void*)0)({
void*_tmp1B4[0]={};Cyc_Tcutil_terr(_tmpD5->loc,({const char*_tmp1B5="expression may not be initialized";
_tag_arr(_tmp1B5,sizeof(char),_get_zero_arr_size(_tmp1B5,34));}),_tag_arr(
_tmp1B4,sizeof(void*),0));});{struct Cyc_List_List*_tmp1B6=Cyc_NewControlFlow_add_subscript_reln(
_tmp1B3,_tmpD4,_tmpD5);if(_tmp1B3 != _tmp1B6)_tmp1B0=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp1B8;_tmp1B8.tag=0;_tmp1B8.f1=_tmp1B2;_tmp1B8.f2=_tmp1B6;_tmp1B8;});_tmp1B7;});
goto _LL145;}_LL148:;_LL149: goto _LL145;_LL145:;}{void*_tmp1B9=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD4->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1BA;struct Cyc_Absyn_PtrAtts _tmp1BB;struct Cyc_Absyn_Conref*_tmp1BC;_LL14B: if(
_tmp1B9 <= (void*)3  || *((int*)_tmp1B9)!= 9)goto _LL14D;_LL14C: {void*_tmp1BD=(
void*)((struct Cyc_List_List*)_check_null(_tmp1AF))->hd;struct Cyc_Dict_Dict*
_tmp1BE;_LL152: if(_tmp1BD <= (void*)3  || *((int*)_tmp1BD)!= 4)goto _LL154;_tmp1BE=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1BD)->f1;_LL153: return({struct _tuple0
_tmp1BF;_tmp1BF.f1=_tmp1B0;_tmp1BF.f2=((void*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_lookup)(_tmp1BE,Cyc_Absyn_fieldname((int)(Cyc_Evexp_eval_const_uint_exp(
_tmpD5)).f1));_tmp1BF;});_LL154:;_LL155:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0[0]=({struct Cyc_Core_Impossible_struct
_tmp1C1;_tmp1C1.tag=Cyc_Core_Impossible;_tmp1C1.f1=({const char*_tmp1C2="anal_Rexp: Subscript";
_tag_arr(_tmp1C2,sizeof(char),_get_zero_arr_size(_tmp1C2,21));});_tmp1C1;});
_tmp1C0;}));_LL151:;}_LL14D: if(_tmp1B9 <= (void*)3  || *((int*)_tmp1B9)!= 4)goto
_LL14F;_tmp1BA=((struct Cyc_Absyn_PointerType_struct*)_tmp1B9)->f1;_tmp1BB=
_tmp1BA.ptr_atts;_tmp1BC=_tmp1BB.bounds;_LL14E:{void*_tmp1C3=(void*)(Cyc_Absyn_compress_conref(
_tmp1BC))->v;void*_tmp1C4;_LL157: if(_tmp1C3 <= (void*)1  || *((int*)_tmp1C3)!= 0)
goto _LL159;_tmp1C4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp1C3)->f1;if(
_tmp1C4 <= (void*)1  || *((int*)_tmp1C4)!= 1)goto _LL159;_LL158:{void*_tmp1C5=(void*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp1AF))->tl))->hd;
struct Cyc_List_List*_tmp1C6;_LL15C: if(_tmp1C5 <= (void*)3  || *((int*)_tmp1C5)!= 3)
goto _LL15E;_tmp1C6=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1C5)->f1;_LL15D:(
void*)(_tmpD5->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp1C8;_tmp1C8.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp1C8.f1=_tmp1C6;_tmp1C8;});
_tmp1C7;})));goto _LL15B;_LL15E:;_LL15F: goto _LL15B;_LL15B:;}goto _LL156;_LL159:;
_LL15A: goto _LL156;_LL156:;}{void*_tmp1CA;void*_tmp1CB;struct _tuple0 _tmp1C9=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp1AE,_tmpD4,(void*)((struct Cyc_List_List*)_check_null(_tmp1AF))->hd);
_tmp1CA=_tmp1C9.f1;_tmp1CB=_tmp1C9.f2;return _tmp1CA == (void*)0?({struct _tuple0
_tmp1CC;_tmp1CC.f1=_tmp1CA;_tmp1CC.f2=_tmp1CB;_tmp1CC;}):({struct _tuple0 _tmp1CD;
_tmp1CD.f1=_tmp1B0;_tmp1CD.f2=_tmp1CB;_tmp1CD;});}_LL14F:;_LL150:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));
_tmp1CE[0]=({struct Cyc_Core_Impossible_struct _tmp1CF;_tmp1CF.tag=Cyc_Core_Impossible;
_tmp1CF.f1=({const char*_tmp1D0="anal_Rexp: Subscript -- bad type";_tag_arr(
_tmp1D0,sizeof(char),_get_zero_arr_size(_tmp1D0,33));});_tmp1CF;});_tmp1CE;}));
_LL14A:;}}}_LLD4: if(*((int*)_tmp9E)!= 32)goto _LLD6;_tmpD6=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp9E)->f1;_LLD5: _tmpD7=_tmpD6;goto _LLD7;_LLD6: if(*((int*)_tmp9E)!= 26)goto
_LLD8;_tmpD7=((struct Cyc_Absyn_Tuple_e_struct*)_tmp9E)->f1;_LLD7: {void*_tmp1D2;
struct Cyc_List_List*_tmp1D3;struct _tuple5 _tmp1D1=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmpD7);_tmp1D2=_tmp1D1.f1;_tmp1D3=_tmp1D1.f2;{struct Cyc_Dict_Dict*
aggrdict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{int i=0;for(0;_tmp1D3 != 0;(_tmp1D3=
_tmp1D3->tl,++ i)){aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,Cyc_Absyn_fieldname(i),(
void*)_tmp1D3->hd);}}return({struct _tuple0 _tmp1D4;_tmp1D4.f1=_tmp1D2;_tmp1D4.f2=(
void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));
_tmp1D5[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp1D6;_tmp1D6.tag=4;_tmp1D6.f1=
aggrdict;_tmp1D6;});_tmp1D5;});_tmp1D4;});}}_LLD8: if(*((int*)_tmp9E)!= 31)goto
_LLDA;_tmpD8=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp9E)->f2;_LLD9: _tmpD9=
_tmpD8;goto _LLDB;_LLDA: if(*((int*)_tmp9E)!= 30)goto _LLDC;_tmpD9=((struct Cyc_Absyn_Struct_e_struct*)
_tmp9E)->f3;_LLDB: {void*_tmp1D8;struct Cyc_List_List*_tmp1D9;struct _tuple5
_tmp1D7=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple6*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple6*))Cyc_Core_snd,_tmpD9));_tmp1D8=_tmp1D7.f1;
_tmp1D9=_tmp1D7.f2;{struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{int
i=0;for(0;_tmp1D9 != 0;(((_tmp1D9=_tmp1D9->tl,_tmpD9=_tmpD9->tl)),++ i)){struct Cyc_List_List*
ds=(*((struct _tuple6*)((struct Cyc_List_List*)_check_null(_tmpD9))->hd)).f1;for(0;
ds != 0;ds=ds->tl){void*_tmp1DA=(void*)ds->hd;struct _tagged_arr*_tmp1DB;_LL161:
if(*((int*)_tmp1DA)!= 0)goto _LL163;_LL162:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC[0]=({struct Cyc_Core_Impossible_struct
_tmp1DD;_tmp1DD.tag=Cyc_Core_Impossible;_tmp1DD.f1=({const char*_tmp1DE="anal_Rexp:Struct_e";
_tag_arr(_tmp1DE,sizeof(char),_get_zero_arr_size(_tmp1DE,19));});_tmp1DD;});
_tmp1DC;}));_LL163: if(*((int*)_tmp1DA)!= 1)goto _LL160;_tmp1DB=((struct Cyc_Absyn_FieldName_struct*)
_tmp1DA)->f1;_LL164: aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,_tmp1DB,(void*)_tmp1D9->hd);
_LL160:;}}}return({struct _tuple0 _tmp1DF;_tmp1DF.f1=_tmp1D8;_tmp1DF.f2=(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));
_tmp1E0[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp1E1;_tmp1E1.tag=4;_tmp1E1.f1=
aggrdict;_tmp1E1;});_tmp1E0;});_tmp1DF;});}}_LLDC: if(*((int*)_tmp9E)!= 28)goto
_LLDE;_tmpDA=((struct Cyc_Absyn_Array_e_struct*)_tmp9E)->f1;_LLDD: {struct Cyc_List_List*
_tmp1E2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple6*),
struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple6*))
Cyc_Core_snd,_tmpDA);void*_tmp1E4;struct Cyc_List_List*_tmp1E5;struct _tuple5
_tmp1E3=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp1E2);_tmp1E4=
_tmp1E3.f1;_tmp1E5=_tmp1E3.f2;for(0;_tmp1E5 != 0;(_tmp1E5=_tmp1E5->tl,_tmp1E2=
_tmp1E2->tl)){_tmp1E4=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1E2))->hd)->loc,_tmp1E4,(void*)_tmp1E5->hd);}
return({struct _tuple0 _tmp1E6;_tmp1E6.f1=_tmp1E4;_tmp1E6.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp1E6;});}_LLDE: if(*((int*)_tmp9E)!= 29)goto _LLE0;_tmpDB=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp9E)->f1;_tmpDC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp9E)->f2;_tmpDD=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp9E)->f3;_tmpDE=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp9E)->f4;_LLDF: {void*_tmp1E8;void*_tmp1E9;struct _tuple0 _tmp1E7=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpDC);_tmp1E8=_tmp1E7.f1;_tmp1E9=_tmp1E7.f2;{void*_tmp1EA=_tmp1E8;
struct Cyc_Dict_Dict*_tmp1EB;struct Cyc_List_List*_tmp1EC;_LL166: if((int)_tmp1EA != 
0)goto _LL168;_LL167: return({struct _tuple0 _tmp1ED;_tmp1ED.f1=_tmp1E8;_tmp1ED.f2=
Cyc_CfFlowInfo_unknown_all;_tmp1ED;});_LL168: if(_tmp1EA <= (void*)1  || *((int*)
_tmp1EA)!= 0)goto _LL165;_tmp1EB=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1EA)->f1;_tmp1EC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1EA)->f2;
_LL169: if(Cyc_CfFlowInfo_initlevel(_tmp1EB,_tmp1E9)== (void*)0)({void*_tmp1EE[0]={};
Cyc_Tcutil_terr(_tmpDC->loc,({const char*_tmp1EF="expression may not be initialized";
_tag_arr(_tmp1EF,sizeof(char),_get_zero_arr_size(_tmp1EF,34));}),_tag_arr(
_tmp1EE,sizeof(void*),0));});{struct Cyc_List_List*new_relns=_tmp1EC;comp_loop: {
void*_tmp1F0=(void*)_tmpDC->r;struct Cyc_Absyn_Exp*_tmp1F1;void*_tmp1F2;struct Cyc_Absyn_Vardecl*
_tmp1F3;void*_tmp1F4;struct Cyc_Absyn_Vardecl*_tmp1F5;void*_tmp1F6;struct Cyc_Absyn_Vardecl*
_tmp1F7;void*_tmp1F8;struct Cyc_Absyn_Vardecl*_tmp1F9;void*_tmp1FA;int _tmp1FB;
void*_tmp1FC;struct Cyc_List_List*_tmp1FD;struct Cyc_List_List _tmp1FE;struct Cyc_Absyn_Exp*
_tmp1FF;_LL16B: if(*((int*)_tmp1F0)!= 15)goto _LL16D;_tmp1F1=((struct Cyc_Absyn_Cast_e_struct*)
_tmp1F0)->f2;_LL16C: _tmpDC=_tmp1F1;goto comp_loop;_LL16D: if(*((int*)_tmp1F0)!= 1)
goto _LL16F;_tmp1F2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F0)->f2;if(
_tmp1F2 <= (void*)1  || *((int*)_tmp1F2)!= 0)goto _LL16F;_tmp1F3=((struct Cyc_Absyn_Global_b_struct*)
_tmp1F2)->f1;if(!(!_tmp1F3->escapes))goto _LL16F;_LL16E: _tmp1F5=_tmp1F3;goto
_LL170;_LL16F: if(*((int*)_tmp1F0)!= 1)goto _LL171;_tmp1F4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F0)->f2;if(_tmp1F4 <= (void*)1  || *((int*)_tmp1F4)!= 3)goto _LL171;_tmp1F5=((
struct Cyc_Absyn_Local_b_struct*)_tmp1F4)->f1;if(!(!_tmp1F5->escapes))goto _LL171;
_LL170: _tmp1F7=_tmp1F5;goto _LL172;_LL171: if(*((int*)_tmp1F0)!= 1)goto _LL173;
_tmp1F6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F0)->f2;if(_tmp1F6 <= (void*)
1  || *((int*)_tmp1F6)!= 4)goto _LL173;_tmp1F7=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1F6)->f1;if(!(!_tmp1F7->escapes))goto _LL173;_LL172: _tmp1F9=_tmp1F7;goto
_LL174;_LL173: if(*((int*)_tmp1F0)!= 1)goto _LL175;_tmp1F8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F0)->f2;if(_tmp1F8 <= (void*)1  || *((int*)_tmp1F8)!= 2)goto _LL175;_tmp1F9=((
struct Cyc_Absyn_Param_b_struct*)_tmp1F8)->f1;if(!(!_tmp1F9->escapes))goto _LL175;
_LL174: new_relns=({struct Cyc_List_List*_tmp200=_cycalloc(sizeof(*_tmp200));
_tmp200->hd=({struct Cyc_CfFlowInfo_Reln*_tmp201=_cycalloc(sizeof(*_tmp201));
_tmp201->vd=_tmpDB;_tmp201->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp203;_tmp203.tag=1;_tmp203.f1=_tmp1F9;_tmp203;});_tmp202;}));_tmp201;});
_tmp200->tl=_tmp1EC;_tmp200;});goto _LL16A;_LL175: if(*((int*)_tmp1F0)!= 0)goto
_LL177;_tmp1FA=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp1F0)->f1;if(_tmp1FA
<= (void*)1  || *((int*)_tmp1FA)!= 2)goto _LL177;_tmp1FB=((struct Cyc_Absyn_Int_c_struct*)
_tmp1FA)->f2;_LL176: new_relns=({struct Cyc_List_List*_tmp204=_cycalloc(sizeof(*
_tmp204));_tmp204->hd=({struct Cyc_CfFlowInfo_Reln*_tmp205=_cycalloc(sizeof(*
_tmp205));_tmp205->vd=_tmpDB;_tmp205->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp206=_cycalloc_atomic(sizeof(*_tmp206));_tmp206[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp207;_tmp207.tag=3;_tmp207.f1=(unsigned int)_tmp1FB;_tmp207;});_tmp206;}));
_tmp205;});_tmp204->tl=_tmp1EC;_tmp204;});goto _LL16A;_LL177: if(*((int*)_tmp1F0)
!= 3)goto _LL179;_tmp1FC=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1F0)->f1;
if((int)_tmp1FC != 19)goto _LL179;_tmp1FD=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1F0)->f2;if(_tmp1FD == 0)goto _LL179;_tmp1FE=*_tmp1FD;_tmp1FF=(struct Cyc_Absyn_Exp*)
_tmp1FE.hd;_LL178:{void*_tmp208=(void*)_tmp1FF->r;void*_tmp209;struct Cyc_Absyn_Vardecl*
_tmp20A;void*_tmp20B;struct Cyc_Absyn_Vardecl*_tmp20C;void*_tmp20D;struct Cyc_Absyn_Vardecl*
_tmp20E;void*_tmp20F;struct Cyc_Absyn_Vardecl*_tmp210;_LL17C: if(*((int*)_tmp208)
!= 1)goto _LL17E;_tmp209=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp208)->f2;if(
_tmp209 <= (void*)1  || *((int*)_tmp209)!= 0)goto _LL17E;_tmp20A=((struct Cyc_Absyn_Global_b_struct*)
_tmp209)->f1;if(!(!_tmp20A->escapes))goto _LL17E;_LL17D: _tmp20C=_tmp20A;goto
_LL17F;_LL17E: if(*((int*)_tmp208)!= 1)goto _LL180;_tmp20B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp208)->f2;if(_tmp20B <= (void*)1  || *((int*)_tmp20B)!= 3)goto _LL180;_tmp20C=((
struct Cyc_Absyn_Local_b_struct*)_tmp20B)->f1;if(!(!_tmp20C->escapes))goto _LL180;
_LL17F: _tmp20E=_tmp20C;goto _LL181;_LL180: if(*((int*)_tmp208)!= 1)goto _LL182;
_tmp20D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp208)->f2;if(_tmp20D <= (void*)
1  || *((int*)_tmp20D)!= 4)goto _LL182;_tmp20E=((struct Cyc_Absyn_Pat_b_struct*)
_tmp20D)->f1;if(!(!_tmp20E->escapes))goto _LL182;_LL181: _tmp210=_tmp20E;goto
_LL183;_LL182: if(*((int*)_tmp208)!= 1)goto _LL184;_tmp20F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp208)->f2;if(_tmp20F <= (void*)1  || *((int*)_tmp20F)!= 2)goto _LL184;_tmp210=((
struct Cyc_Absyn_Param_b_struct*)_tmp20F)->f1;if(!(!_tmp210->escapes))goto _LL184;
_LL183: new_relns=({struct Cyc_List_List*_tmp211=_cycalloc(sizeof(*_tmp211));
_tmp211->hd=({struct Cyc_CfFlowInfo_Reln*_tmp212=_cycalloc(sizeof(*_tmp212));
_tmp212->vd=_tmpDB;_tmp212->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp214;_tmp214.tag=2;_tmp214.f1=_tmp210;_tmp214;});_tmp213;}));_tmp212;});
_tmp211->tl=_tmp1EC;_tmp211;});goto _LL17B;_LL184:;_LL185: goto _LL17B;_LL17B:;}
goto _LL16A;_LL179:;_LL17A: goto _LL16A;_LL16A:;}if(_tmp1EC != new_relns)_tmp1E8=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp215=_cycalloc(sizeof(*
_tmp215));_tmp215[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp216;_tmp216.tag=
0;_tmp216.f1=_tmp1EB;_tmp216.f2=new_relns;_tmp216;});_tmp215;});{void*_tmp217=
_tmp1E9;_LL187: if((int)_tmp217 != 0)goto _LL189;_LL188: return({struct _tuple0
_tmp218;_tmp218.f1=_tmp1E8;_tmp218.f2=Cyc_CfFlowInfo_unknown_all;_tmp218;});
_LL189: if((int)_tmp217 != 2)goto _LL18B;_LL18A: goto _LL18C;_LL18B: if((int)_tmp217 != 
1)goto _LL18D;_LL18C: goto _LL18E;_LL18D: if(_tmp217 <= (void*)3  || *((int*)_tmp217)
!= 2)goto _LL18F;_LL18E: {struct Cyc_List_List _tmp219=({struct Cyc_List_List _tmp223;
_tmp223.hd=_tmpDB;_tmp223.tl=0;_tmp223;});_tmp1E8=Cyc_NewControlFlow_add_vars(
_tmp1E8,(struct Cyc_List_List*)& _tmp219,Cyc_CfFlowInfo_unknown_all);{void*_tmp21B;
void*_tmp21C;struct _tuple0 _tmp21A=Cyc_NewControlFlow_anal_Rexp(env,_tmp1E8,
_tmpDD);_tmp21B=_tmp21A.f1;_tmp21C=_tmp21A.f2;{void*_tmp21D=_tmp21B;struct Cyc_Dict_Dict*
_tmp21E;_LL192: if((int)_tmp21D != 0)goto _LL194;_LL193: return({struct _tuple0
_tmp21F;_tmp21F.f1=_tmp21B;_tmp21F.f2=Cyc_CfFlowInfo_unknown_all;_tmp21F;});
_LL194: if(_tmp21D <= (void*)1  || *((int*)_tmp21D)!= 0)goto _LL191;_tmp21E=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp21D)->f1;_LL195: if(Cyc_CfFlowInfo_initlevel(
_tmp21E,_tmp21C)!= (void*)2){({void*_tmp220[0]={};Cyc_Tcutil_terr(_tmpDC->loc,({
const char*_tmp221="expression may not be initialized";_tag_arr(_tmp221,sizeof(
char),_get_zero_arr_size(_tmp221,34));}),_tag_arr(_tmp220,sizeof(void*),0));});
return({struct _tuple0 _tmp222;_tmp222.f1=(void*)0;_tmp222.f2=Cyc_CfFlowInfo_unknown_all;
_tmp222;});}_LL191:;}_tmp1E8=_tmp21B;goto _LL190;}}_LL18F:;_LL190: while(1){struct
Cyc_List_List _tmp224=({struct Cyc_List_List _tmp22E;_tmp22E.hd=_tmpDB;_tmp22E.tl=0;
_tmp22E;});_tmp1E8=Cyc_NewControlFlow_add_vars(_tmp1E8,(struct Cyc_List_List*)&
_tmp224,Cyc_CfFlowInfo_unknown_all);{void*_tmp226;void*_tmp227;struct _tuple0
_tmp225=Cyc_NewControlFlow_anal_Rexp(env,_tmp1E8,_tmpDD);_tmp226=_tmp225.f1;
_tmp227=_tmp225.f2;{void*_tmp228=_tmp226;struct Cyc_Dict_Dict*_tmp229;_LL197: if((
int)_tmp228 != 0)goto _LL199;_LL198: goto _LL196;_LL199: if(_tmp228 <= (void*)1  || *((
int*)_tmp228)!= 0)goto _LL196;_tmp229=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp228)->f1;_LL19A: if(Cyc_CfFlowInfo_initlevel(_tmp229,_tmp227)!= (void*)2){({
void*_tmp22A[0]={};Cyc_Tcutil_terr(_tmpDC->loc,({const char*_tmp22B="expression may not be initialized";
_tag_arr(_tmp22B,sizeof(char),_get_zero_arr_size(_tmp22B,34));}),_tag_arr(
_tmp22A,sizeof(void*),0));});return({struct _tuple0 _tmp22C;_tmp22C.f1=(void*)0;
_tmp22C.f2=Cyc_CfFlowInfo_unknown_all;_tmp22C;});}_LL196:;}{void*_tmp22D=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp1E8,_tmp226);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp22D,
_tmp1E8))break;_tmp1E8=_tmp22D;}}}return({struct _tuple0 _tmp22F;_tmp22F.f1=
_tmp1E8;_tmp22F.f2=Cyc_CfFlowInfo_unknown_all;_tmp22F;});_LL186:;}}_LL165:;}}
_LLE0: if(*((int*)_tmp9E)!= 37)goto _LLE2;_tmpDF=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp9E)->f1;_LLE1: while(1){inflow=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
inflow,_tmpDF))->flow;{void*_tmp230=(void*)_tmpDF->r;struct Cyc_Absyn_Stmt*
_tmp231;struct Cyc_Absyn_Stmt*_tmp232;struct Cyc_Absyn_Decl*_tmp233;struct Cyc_Absyn_Stmt*
_tmp234;struct Cyc_Absyn_Exp*_tmp235;_LL19C: if(_tmp230 <= (void*)1  || *((int*)
_tmp230)!= 1)goto _LL19E;_tmp231=((struct Cyc_Absyn_Seq_s_struct*)_tmp230)->f1;
_tmp232=((struct Cyc_Absyn_Seq_s_struct*)_tmp230)->f2;_LL19D: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp231);_tmpDF=_tmp232;goto _LL19B;_LL19E: if(_tmp230 <= (void*)1  || *((
int*)_tmp230)!= 12)goto _LL1A0;_tmp233=((struct Cyc_Absyn_Decl_s_struct*)_tmp230)->f1;
_tmp234=((struct Cyc_Absyn_Decl_s_struct*)_tmp230)->f2;_LL19F: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp233);_tmpDF=_tmp234;goto _LL19B;_LL1A0: if(_tmp230 <= (void*)1  || *((
int*)_tmp230)!= 0)goto _LL1A2;_tmp235=((struct Cyc_Absyn_Exp_s_struct*)_tmp230)->f1;
_LL1A1: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp235);_LL1A2:;_LL1A3:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp236=_cycalloc(sizeof(*
_tmp236));_tmp236[0]=({struct Cyc_Core_Impossible_struct _tmp237;_tmp237.tag=Cyc_Core_Impossible;
_tmp237.f1=({const char*_tmp238="analyze_Rexp: ill-formed StmtExp";_tag_arr(
_tmp238,sizeof(char),_get_zero_arr_size(_tmp238,33));});_tmp237;});_tmp236;}));
_LL19B:;}}_LLE2: if(*((int*)_tmp9E)!= 1)goto _LLE4;_tmpE0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp9E)->f2;if((int)_tmpE0 != 0)goto _LLE4;_LLE3: goto _LLE5;_LLE4: if(*((int*)_tmp9E)
!= 2)goto _LLE6;_LLE5: goto _LLE7;_LLE6: if(*((int*)_tmp9E)!= 10)goto _LLE8;_LLE7:
goto _LLE9;_LLE8: if(*((int*)_tmp9E)!= 36)goto _LLEA;_LLE9: goto _LLEB;_LLEA: if(*((
int*)_tmp9E)!= 27)goto _LLEC;_LLEB: goto _LLED;_LLEC: if(*((int*)_tmp9E)!= 38)goto
_LLEE;_LLED: goto _LLEF;_LLEE: if(*((int*)_tmp9E)!= 39)goto _LL89;_LLEF:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp239=_cycalloc(sizeof(*_tmp239));
_tmp239[0]=({struct Cyc_Core_Impossible_struct _tmp23A;_tmp23A.tag=Cyc_Core_Impossible;
_tmp23A.f1=({const char*_tmp23B="anal_Rexp, unexpected exp form";_tag_arr(_tmp23B,
sizeof(char),_get_zero_arr_size(_tmp23B,31));});_tmp23A;});_tmp239;}));_LL89:;}}
static struct _tuple0 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){
void*_tmp23C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp23D;void*_tmp23E;struct Cyc_Absyn_PtrAtts _tmp23F;
struct Cyc_Absyn_Conref*_tmp240;struct Cyc_Absyn_Conref*_tmp241;_LL1A5: if(_tmp23C
<= (void*)3  || *((int*)_tmp23C)!= 4)goto _LL1A7;_tmp23D=((struct Cyc_Absyn_PointerType_struct*)
_tmp23C)->f1;_tmp23E=(void*)_tmp23D.elt_typ;_tmp23F=_tmp23D.ptr_atts;_tmp240=
_tmp23F.bounds;_tmp241=_tmp23F.zero_term;_LL1A6: {void*_tmp242=f;struct Cyc_Dict_Dict*
_tmp243;struct Cyc_List_List*_tmp244;_LL1AA: if((int)_tmp242 != 0)goto _LL1AC;_LL1AB:
return({struct _tuple0 _tmp245;_tmp245.f1=f;_tmp245.f2=(void*)0;_tmp245;});_LL1AC:
if(_tmp242 <= (void*)1  || *((int*)_tmp242)!= 0)goto _LL1A9;_tmp243=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp242)->f1;_tmp244=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp242)->f2;
_LL1AD: if(Cyc_Tcutil_is_bound_one(_tmp240)){void*_tmp246=r;struct Cyc_CfFlowInfo_Place*
_tmp247;struct Cyc_CfFlowInfo_Place _tmp248;void*_tmp249;struct Cyc_List_List*
_tmp24A;void*_tmp24B;_LL1AF: if((int)_tmp246 != 1)goto _LL1B1;_LL1B0: goto _LL1B2;
_LL1B1: if((int)_tmp246 != 2)goto _LL1B3;_LL1B2:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_NotZero_struct*_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C[
0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp24D;_tmp24D.tag=Cyc_CfFlowInfo_NotZero;
_tmp24D.f1=_tmp244;_tmp24D;});_tmp24C;})));goto _LL1AE;_LL1B3: if(_tmp246 <= (void*)
3  || *((int*)_tmp246)!= 2)goto _LL1B5;_tmp247=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp246)->f1;_tmp248=*_tmp247;_tmp249=(void*)_tmp248.root;_tmp24A=_tmp248.fields;
_LL1B4:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp24E=_cycalloc(sizeof(*_tmp24E));_tmp24E[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp24F;_tmp24F.tag=Cyc_CfFlowInfo_NotZero;_tmp24F.f1=_tmp244;_tmp24F;});_tmp24E;})));
return({struct _tuple0 _tmp250;_tmp250.f1=f;_tmp250.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp251=_cycalloc(sizeof(*_tmp251));_tmp251[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp252;_tmp252.tag=0;_tmp252.f1=({struct Cyc_CfFlowInfo_Place*_tmp253=_cycalloc(
sizeof(*_tmp253));_tmp253->root=(void*)_tmp249;_tmp253->fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp24A,flds);
_tmp253;});_tmp252;});_tmp251;});_tmp250;});_LL1B5: if((int)_tmp246 != 0)goto
_LL1B7;_LL1B6:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));return({
struct _tuple0 _tmp254;_tmp254.f1=(void*)0;_tmp254.f2=(void*)0;_tmp254;});_LL1B7:
if(_tmp246 <= (void*)3  || *((int*)_tmp246)!= 0)goto _LL1B9;_tmp24B=(void*)((struct
Cyc_CfFlowInfo_UnknownR_struct*)_tmp246)->f1;_LL1B8: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp24B);goto _LL1BA;_LL1B9:;_LL1BA:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp255=_cycalloc(sizeof(*_tmp255));_tmp255[
0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp256;_tmp256.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp256.f1=_tmp244;_tmp256;});_tmp255;})));_LL1AE:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp257=_cycalloc(sizeof(*_tmp257));
_tmp257[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp258;_tmp258.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp258.f1=_tmp244;_tmp258;});_tmp257;})));}if(Cyc_CfFlowInfo_initlevel(_tmp243,
r)== (void*)0)({void*_tmp259[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp25A="dereference of possibly uninitialized pointer";
_tag_arr(_tmp25A,sizeof(char),_get_zero_arr_size(_tmp25A,46));}),_tag_arr(
_tmp259,sizeof(void*),0));});return({struct _tuple0 _tmp25B;_tmp25B.f1=f;_tmp25B.f2=(
void*)0;_tmp25B;});_LL1A9:;}_LL1A7:;_LL1A8:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp25C=_cycalloc(sizeof(*_tmp25C));_tmp25C[0]=({struct Cyc_Core_Impossible_struct
_tmp25D;_tmp25D.tag=Cyc_Core_Impossible;_tmp25D.f1=({const char*_tmp25E="left deref of non-pointer-type";
_tag_arr(_tmp25E,sizeof(char),_get_zero_arr_size(_tmp25E,31));});_tmp25D;});
_tmp25C;}));_LL1A4:;}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(struct
Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict*d;{void*_tmp25F=inflow;struct Cyc_Dict_Dict*_tmp260;
struct Cyc_List_List*_tmp261;_LL1BC: if((int)_tmp25F != 0)goto _LL1BE;_LL1BD: return({
struct _tuple0 _tmp262;_tmp262.f1=(void*)0;_tmp262.f2=(void*)0;_tmp262;});_LL1BE:
if(_tmp25F <= (void*)1  || *((int*)_tmp25F)!= 0)goto _LL1BB;_tmp260=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp25F)->f1;_tmp261=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp25F)->f2;
_LL1BF: d=_tmp260;_LL1BB:;}{void*_tmp263=(void*)e->r;struct Cyc_Absyn_Exp*_tmp264;
struct Cyc_Absyn_Exp*_tmp265;struct Cyc_Absyn_Exp*_tmp266;void*_tmp267;struct Cyc_Absyn_Vardecl*
_tmp268;void*_tmp269;struct Cyc_Absyn_Vardecl*_tmp26A;void*_tmp26B;struct Cyc_Absyn_Vardecl*
_tmp26C;void*_tmp26D;struct Cyc_Absyn_Vardecl*_tmp26E;struct Cyc_Absyn_Exp*_tmp26F;
struct _tagged_arr*_tmp270;struct Cyc_Absyn_Exp*_tmp271;struct Cyc_Absyn_Exp*
_tmp272;struct Cyc_Absyn_Exp*_tmp273;struct Cyc_Absyn_Exp*_tmp274;struct
_tagged_arr*_tmp275;_LL1C1: if(*((int*)_tmp263)!= 15)goto _LL1C3;_tmp264=((struct
Cyc_Absyn_Cast_e_struct*)_tmp263)->f2;_LL1C2: _tmp265=_tmp264;goto _LL1C4;_LL1C3:
if(*((int*)_tmp263)!= 13)goto _LL1C5;_tmp265=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp263)->f1;_LL1C4: _tmp266=_tmp265;goto _LL1C6;_LL1C5: if(*((int*)_tmp263)!= 14)
goto _LL1C7;_tmp266=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp263)->f1;_LL1C6:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp266,flds);_LL1C7: if(*((int*)
_tmp263)!= 1)goto _LL1C9;_tmp267=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp263)->f2;
if(_tmp267 <= (void*)1  || *((int*)_tmp267)!= 2)goto _LL1C9;_tmp268=((struct Cyc_Absyn_Param_b_struct*)
_tmp267)->f1;_LL1C8: _tmp26A=_tmp268;goto _LL1CA;_LL1C9: if(*((int*)_tmp263)!= 1)
goto _LL1CB;_tmp269=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp263)->f2;if(
_tmp269 <= (void*)1  || *((int*)_tmp269)!= 3)goto _LL1CB;_tmp26A=((struct Cyc_Absyn_Local_b_struct*)
_tmp269)->f1;_LL1CA: _tmp26C=_tmp26A;goto _LL1CC;_LL1CB: if(*((int*)_tmp263)!= 1)
goto _LL1CD;_tmp26B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp263)->f2;if(
_tmp26B <= (void*)1  || *((int*)_tmp26B)!= 4)goto _LL1CD;_tmp26C=((struct Cyc_Absyn_Pat_b_struct*)
_tmp26B)->f1;_LL1CC: return({struct _tuple0 _tmp276;_tmp276.f1=inflow;_tmp276.f2=(
void*)({struct Cyc_CfFlowInfo_PlaceL_struct*_tmp277=_cycalloc(sizeof(*_tmp277));
_tmp277[0]=({struct Cyc_CfFlowInfo_PlaceL_struct _tmp278;_tmp278.tag=0;_tmp278.f1=({
struct Cyc_CfFlowInfo_Place*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp27A=_cycalloc(sizeof(*
_tmp27A));_tmp27A[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp27B;_tmp27B.tag=0;
_tmp27B.f1=_tmp26C;_tmp27B;});_tmp27A;}));_tmp279->fields=flds;_tmp279;});
_tmp278;});_tmp277;});_tmp276;});_LL1CD: if(*((int*)_tmp263)!= 1)goto _LL1CF;
_tmp26D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp263)->f2;if(_tmp26D <= (void*)
1  || *((int*)_tmp26D)!= 0)goto _LL1CF;_tmp26E=((struct Cyc_Absyn_Global_b_struct*)
_tmp26D)->f1;_LL1CE: return({struct _tuple0 _tmp27C;_tmp27C.f1=inflow;_tmp27C.f2=(
void*)0;_tmp27C;});_LL1CF: if(*((int*)_tmp263)!= 24)goto _LL1D1;_tmp26F=((struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp263)->f1;_tmp270=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp263)->f2;_LL1D0:{void*_tmp27D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp26F->topt))->v);struct Cyc_Absyn_PtrInfo _tmp27E;void*_tmp27F;
_LL1DA: if(_tmp27D <= (void*)3  || *((int*)_tmp27D)!= 4)goto _LL1DC;_tmp27E=((struct
Cyc_Absyn_PointerType_struct*)_tmp27D)->f1;_tmp27F=(void*)_tmp27E.elt_typ;_LL1DB:
if(!Cyc_Absyn_is_union_type(_tmp27F))flds=({struct Cyc_List_List*_tmp280=
_cycalloc(sizeof(*_tmp280));_tmp280->hd=_tmp270;_tmp280->tl=flds;_tmp280;});goto
_LL1D9;_LL1DC:;_LL1DD:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281[0]=({struct Cyc_Core_Impossible_struct
_tmp282;_tmp282.tag=Cyc_Core_Impossible;_tmp282.f1=({const char*_tmp283="anal_Rexp: AggrArrow ptr";
_tag_arr(_tmp283,sizeof(char),_get_zero_arr_size(_tmp283,25));});_tmp282;});
_tmp281;}));_LL1D9:;}_tmp271=_tmp26F;goto _LL1D2;_LL1D1: if(*((int*)_tmp263)!= 22)
goto _LL1D3;_tmp271=((struct Cyc_Absyn_Deref_e_struct*)_tmp263)->f1;_LL1D2: {void*
_tmp285;void*_tmp286;struct _tuple0 _tmp284=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp271);_tmp285=_tmp284.f1;_tmp286=_tmp284.f2;return Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp285,_tmp271,_tmp286,flds);}_LL1D3: if(*((int*)_tmp263)!= 25)goto
_LL1D5;_tmp272=((struct Cyc_Absyn_Subscript_e_struct*)_tmp263)->f1;_tmp273=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp263)->f2;_LL1D4: {void*_tmp287=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp272->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp288;struct Cyc_Absyn_PtrAtts _tmp289;struct Cyc_Absyn_Conref*_tmp28A;_LL1DF: if(
_tmp287 <= (void*)3  || *((int*)_tmp287)!= 9)goto _LL1E1;_LL1E0: {struct _tagged_arr*
_tmp28B=Cyc_Absyn_fieldname((int)(Cyc_Evexp_eval_const_uint_exp(_tmp273)).f1);
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp272,({struct Cyc_List_List*
_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->hd=_tmp28B;_tmp28C->tl=flds;_tmp28C;}));}
_LL1E1: if(_tmp287 <= (void*)3  || *((int*)_tmp287)!= 4)goto _LL1E3;_tmp288=((struct
Cyc_Absyn_PointerType_struct*)_tmp287)->f1;_tmp289=_tmp288.ptr_atts;_tmp28A=
_tmp289.bounds;_LL1E2: {void*_tmp28F;struct Cyc_List_List*_tmp290;struct _tuple5
_tmp28E=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({struct Cyc_Absyn_Exp*
_tmp28D[2];_tmp28D[1]=_tmp273;_tmp28D[0]=_tmp272;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp28D,sizeof(struct Cyc_Absyn_Exp*),
2));}));_tmp28F=_tmp28E.f1;_tmp290=_tmp28E.f2;{void*_tmp291=_tmp28F;{void*
_tmp292=_tmp28F;struct Cyc_Dict_Dict*_tmp293;struct Cyc_List_List*_tmp294;_LL1E6:
if(_tmp292 <= (void*)1  || *((int*)_tmp292)!= 0)goto _LL1E8;_tmp293=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp292)->f1;_tmp294=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp292)->f2;
_LL1E7: if(Cyc_CfFlowInfo_initlevel(_tmp293,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp290))->tl))->hd)== (void*)0)({
void*_tmp295[0]={};Cyc_Tcutil_terr(_tmp273->loc,({const char*_tmp296="expression may not be initialized";
_tag_arr(_tmp296,sizeof(char),_get_zero_arr_size(_tmp296,34));}),_tag_arr(
_tmp295,sizeof(void*),0));});{struct Cyc_List_List*_tmp297=Cyc_NewControlFlow_add_subscript_reln(
_tmp294,_tmp272,_tmp273);if(_tmp294 != _tmp297)_tmp291=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp299;_tmp299.tag=0;_tmp299.f1=_tmp293;_tmp299.f2=_tmp297;_tmp299;});_tmp298;});
goto _LL1E5;}_LL1E8:;_LL1E9: goto _LL1E5;_LL1E5:;}{void*_tmp29A=(void*)(Cyc_Absyn_compress_conref(
_tmp28A))->v;void*_tmp29B;_LL1EB: if(_tmp29A <= (void*)1  || *((int*)_tmp29A)!= 0)
goto _LL1ED;_tmp29B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp29A)->f1;if(
_tmp29B <= (void*)1  || *((int*)_tmp29B)!= 1)goto _LL1ED;_LL1EC:{void*_tmp29C=(void*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp290))->tl))->hd;
struct Cyc_List_List*_tmp29D;_LL1F0: if(_tmp29C <= (void*)3  || *((int*)_tmp29C)!= 3)
goto _LL1F2;_tmp29D=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp29C)->f1;_LL1F1:(
void*)(_tmp273->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp29E=_cycalloc(sizeof(*_tmp29E));_tmp29E[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp29F;_tmp29F.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp29F.f1=_tmp29D;_tmp29F;});
_tmp29E;})));goto _LL1EF;_LL1F2:;_LL1F3: goto _LL1EF;_LL1EF:;}goto _LL1EA;_LL1ED:;
_LL1EE: goto _LL1EA;_LL1EA:;}{void*_tmp2A1;void*_tmp2A2;struct _tuple0 _tmp2A0=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp28F,_tmp272,(void*)((struct Cyc_List_List*)_check_null(_tmp290))->hd,
flds);_tmp2A1=_tmp2A0.f1;_tmp2A2=_tmp2A0.f2;return _tmp2A1 == (void*)0?({struct
_tuple0 _tmp2A3;_tmp2A3.f1=_tmp2A1;_tmp2A3.f2=_tmp2A2;_tmp2A3;}):({struct _tuple0
_tmp2A4;_tmp2A4.f1=_tmp291;_tmp2A4.f2=_tmp2A2;_tmp2A4;});}}}_LL1E3:;_LL1E4:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2A5=_cycalloc(sizeof(*
_tmp2A5));_tmp2A5[0]=({struct Cyc_Core_Impossible_struct _tmp2A6;_tmp2A6.tag=Cyc_Core_Impossible;
_tmp2A6.f1=({const char*_tmp2A7="anal_Lexp: Subscript -- bad type";_tag_arr(
_tmp2A7,sizeof(char),_get_zero_arr_size(_tmp2A7,33));});_tmp2A6;});_tmp2A5;}));
_LL1DE:;}_LL1D5: if(*((int*)_tmp263)!= 23)goto _LL1D7;_tmp274=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp263)->f1;_tmp275=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp263)->f2;_LL1D6:
if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp274->topt))->v))
return({struct _tuple0 _tmp2A8;_tmp2A8.f1=inflow;_tmp2A8.f2=(void*)0;_tmp2A8;});
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp274,({struct Cyc_List_List*
_tmp2A9=_cycalloc(sizeof(*_tmp2A9));_tmp2A9->hd=_tmp275;_tmp2A9->tl=flds;_tmp2A9;}));
_LL1D7:;_LL1D8: return({struct _tuple0 _tmp2AA;_tmp2AA.f1=(void*)0;_tmp2AA.f2=(void*)
0;_tmp2AA;});_LL1C0:;}}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);}static struct _tuple0 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){{void*_tmp2AB=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Absyn_Exp*
_tmp2AF;struct Cyc_Absyn_Exp*_tmp2B0;struct Cyc_Absyn_Exp*_tmp2B1;struct Cyc_Absyn_Exp*
_tmp2B2;struct Cyc_Absyn_Exp*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B4;void*_tmp2B5;
struct Cyc_List_List*_tmp2B6;struct Cyc_List_List _tmp2B7;struct Cyc_Absyn_Exp*
_tmp2B8;struct Cyc_List_List*_tmp2B9;void*_tmp2BA;struct Cyc_List_List*_tmp2BB;
_LL1F5: if(*((int*)_tmp2AB)!= 6)goto _LL1F7;_tmp2AC=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp2AB)->f1;_tmp2AD=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2AB)->f2;
_tmp2AE=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2AB)->f3;_LL1F6: {void*
_tmp2BD;void*_tmp2BE;struct _tuple0 _tmp2BC=Cyc_NewControlFlow_anal_test(env,
inflow,_tmp2AC);_tmp2BD=_tmp2BC.f1;_tmp2BE=_tmp2BC.f2;{void*_tmp2C0;void*_tmp2C1;
struct _tuple0 _tmp2BF=Cyc_NewControlFlow_anal_test(env,_tmp2BD,_tmp2AD);_tmp2C0=
_tmp2BF.f1;_tmp2C1=_tmp2BF.f2;{void*_tmp2C3;void*_tmp2C4;struct _tuple0 _tmp2C2=
Cyc_NewControlFlow_anal_test(env,_tmp2BE,_tmp2AE);_tmp2C3=_tmp2C2.f1;_tmp2C4=
_tmp2C2.f2;return({struct _tuple0 _tmp2C5;_tmp2C5.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,
_tmp2C0,_tmp2C3);_tmp2C5.f2=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp2C1,
_tmp2C4);_tmp2C5;});}}}_LL1F7: if(*((int*)_tmp2AB)!= 7)goto _LL1F9;_tmp2AF=((
struct Cyc_Absyn_And_e_struct*)_tmp2AB)->f1;_tmp2B0=((struct Cyc_Absyn_And_e_struct*)
_tmp2AB)->f2;_LL1F8: {void*_tmp2C7;void*_tmp2C8;struct _tuple0 _tmp2C6=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp2AF);_tmp2C7=_tmp2C6.f1;_tmp2C8=_tmp2C6.f2;{void*_tmp2CA;void*
_tmp2CB;struct _tuple0 _tmp2C9=Cyc_NewControlFlow_anal_test(env,_tmp2C7,_tmp2B0);
_tmp2CA=_tmp2C9.f1;_tmp2CB=_tmp2C9.f2;return({struct _tuple0 _tmp2CC;_tmp2CC.f1=
_tmp2CA;_tmp2CC.f2=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp2C8,_tmp2CB);
_tmp2CC;});}}_LL1F9: if(*((int*)_tmp2AB)!= 8)goto _LL1FB;_tmp2B1=((struct Cyc_Absyn_Or_e_struct*)
_tmp2AB)->f1;_tmp2B2=((struct Cyc_Absyn_Or_e_struct*)_tmp2AB)->f2;_LL1FA: {void*
_tmp2CE;void*_tmp2CF;struct _tuple0 _tmp2CD=Cyc_NewControlFlow_anal_test(env,
inflow,_tmp2B1);_tmp2CE=_tmp2CD.f1;_tmp2CF=_tmp2CD.f2;{void*_tmp2D1;void*_tmp2D2;
struct _tuple0 _tmp2D0=Cyc_NewControlFlow_anal_test(env,_tmp2CF,_tmp2B2);_tmp2D1=
_tmp2D0.f1;_tmp2D2=_tmp2D0.f2;return({struct _tuple0 _tmp2D3;_tmp2D3.f1=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp2CE,_tmp2D1);_tmp2D3.f2=_tmp2D2;_tmp2D3;});}}_LL1FB: if(*((
int*)_tmp2AB)!= 9)goto _LL1FD;_tmp2B3=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp2AB)->f1;
_tmp2B4=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp2AB)->f2;_LL1FC: return Cyc_NewControlFlow_anal_test(
env,(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp2B3)).f1,_tmp2B4);_LL1FD: if(*((
int*)_tmp2AB)!= 3)goto _LL1FF;_tmp2B5=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2AB)->f1;if((int)_tmp2B5 != 11)goto _LL1FF;_tmp2B6=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2AB)->f2;if(_tmp2B6 == 0)goto _LL1FF;_tmp2B7=*_tmp2B6;_tmp2B8=(struct Cyc_Absyn_Exp*)
_tmp2B7.hd;_tmp2B9=_tmp2B7.tl;if(_tmp2B9 != 0)goto _LL1FF;_LL1FE: {void*_tmp2D5;
void*_tmp2D6;struct _tuple0 _tmp2D4=Cyc_NewControlFlow_anal_test(env,inflow,
_tmp2B8);_tmp2D5=_tmp2D4.f1;_tmp2D6=_tmp2D4.f2;return({struct _tuple0 _tmp2D7;
_tmp2D7.f1=_tmp2D6;_tmp2D7.f2=_tmp2D5;_tmp2D7;});}_LL1FF: if(*((int*)_tmp2AB)!= 3)
goto _LL201;_tmp2BA=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2AB)->f1;
_tmp2BB=((struct Cyc_Absyn_Primop_e_struct*)_tmp2AB)->f2;_LL200: {void*_tmp2D9;
struct Cyc_List_List*_tmp2DA;struct _tuple5 _tmp2D8=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp2BB);_tmp2D9=_tmp2D8.f1;_tmp2DA=_tmp2D8.f2;{void*_tmp2DB=_tmp2D9;
struct Cyc_Dict_Dict*_tmp2DC;struct Cyc_List_List*_tmp2DD;_LL204: if((int)_tmp2DB != 
0)goto _LL206;_LL205: return({struct _tuple0 _tmp2DE;_tmp2DE.f1=_tmp2D9;_tmp2DE.f2=
_tmp2D9;_tmp2DE;});_LL206: if(_tmp2DB <= (void*)1  || *((int*)_tmp2DB)!= 0)goto
_LL203;_tmp2DC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2DB)->f1;_tmp2DD=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2DB)->f2;_LL207: {struct Cyc_Absyn_Exp*
_tmp2DF=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2BB))->hd;
struct Cyc_Absyn_Exp*_tmp2E0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2BB->tl))->hd;void*_tmp2E1=(void*)((struct Cyc_List_List*)
_check_null(_tmp2DA))->hd;void*_tmp2E2=(void*)((struct Cyc_List_List*)_check_null(
_tmp2DA->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp2DC,_tmp2E1)== (void*)0)({void*
_tmp2E3[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp2BB->hd)->loc,({const
char*_tmp2E4="expression may not be initialized";_tag_arr(_tmp2E4,sizeof(char),
_get_zero_arr_size(_tmp2E4,34));}),_tag_arr(_tmp2E3,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(
_tmp2DC,_tmp2E2)== (void*)0)({void*_tmp2E5[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2BB->tl))->hd)->loc,({const char*_tmp2E6="expression may not be initialized";
_tag_arr(_tmp2E6,sizeof(char),_get_zero_arr_size(_tmp2E6,34));}),_tag_arr(
_tmp2E5,sizeof(void*),0));});if(_tmp2BA == (void*)5  || _tmp2BA == (void*)6){struct
_tuple0 _tmp2E8=({struct _tuple0 _tmp2E7;_tmp2E7.f1=_tmp2E1;_tmp2E7.f2=_tmp2E2;
_tmp2E7;});void*_tmp2E9;void*_tmp2EA;void*_tmp2EB;void*_tmp2EC;void*_tmp2ED;void*
_tmp2EE;void*_tmp2EF;void*_tmp2F0;void*_tmp2F1;void*_tmp2F2;void*_tmp2F3;void*
_tmp2F4;void*_tmp2F5;void*_tmp2F6;void*_tmp2F7;void*_tmp2F8;void*_tmp2F9;void*
_tmp2FA;void*_tmp2FB;void*_tmp2FC;_LL209: _tmp2E9=_tmp2E8.f1;if(_tmp2E9 <= (void*)
3  || *((int*)_tmp2E9)!= 0)goto _LL20B;_tmp2EA=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp2E9)->f1;_tmp2EB=_tmp2E8.f2;if((int)_tmp2EB != 0)goto _LL20B;_LL20A: {void*
_tmp2FE;void*_tmp2FF;struct _tuple0 _tmp2FD=Cyc_NewControlFlow_splitzero(env,
_tmp2D9,_tmp2D9,_tmp2DF,_tmp2EA);_tmp2FE=_tmp2FD.f1;_tmp2FF=_tmp2FD.f2;{void*
_tmp300=_tmp2BA;_LL21E: if((int)_tmp300 != 5)goto _LL220;_LL21F: return({struct
_tuple0 _tmp301;_tmp301.f1=_tmp2FF;_tmp301.f2=_tmp2FE;_tmp301;});_LL220: if((int)
_tmp300 != 6)goto _LL222;_LL221: return({struct _tuple0 _tmp302;_tmp302.f1=_tmp2FE;
_tmp302.f2=_tmp2FF;_tmp302;});_LL222:;_LL223:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303[0]=({struct Cyc_Core_Impossible_struct
_tmp304;_tmp304.tag=Cyc_Core_Impossible;_tmp304.f1=({const char*_tmp305="anal_test, zero-split";
_tag_arr(_tmp305,sizeof(char),_get_zero_arr_size(_tmp305,22));});_tmp304;});
_tmp303;}));_LL21D:;}}_LL20B: _tmp2EC=_tmp2E8.f1;if((int)_tmp2EC != 0)goto _LL20D;
_tmp2ED=_tmp2E8.f2;if(_tmp2ED <= (void*)3  || *((int*)_tmp2ED)!= 0)goto _LL20D;
_tmp2EE=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp2ED)->f1;_LL20C: {
void*_tmp307;void*_tmp308;struct _tuple0 _tmp306=Cyc_NewControlFlow_splitzero(env,
_tmp2D9,_tmp2D9,_tmp2E0,_tmp2EE);_tmp307=_tmp306.f1;_tmp308=_tmp306.f2;{void*
_tmp309=_tmp2BA;_LL225: if((int)_tmp309 != 5)goto _LL227;_LL226: return({struct
_tuple0 _tmp30A;_tmp30A.f1=_tmp308;_tmp30A.f2=_tmp307;_tmp30A;});_LL227: if((int)
_tmp309 != 6)goto _LL229;_LL228: return({struct _tuple0 _tmp30B;_tmp30B.f1=_tmp307;
_tmp30B.f2=_tmp308;_tmp30B;});_LL229:;_LL22A:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp30C=_cycalloc(sizeof(*_tmp30C));_tmp30C[0]=({struct Cyc_Core_Impossible_struct
_tmp30D;_tmp30D.tag=Cyc_Core_Impossible;_tmp30D.f1=({const char*_tmp30E="anal_test, zero-split";
_tag_arr(_tmp30E,sizeof(char),_get_zero_arr_size(_tmp30E,22));});_tmp30D;});
_tmp30C;}));_LL224:;}}_LL20D: _tmp2EF=_tmp2E8.f1;if((int)_tmp2EF != 0)goto _LL20F;
_tmp2F0=_tmp2E8.f2;if((int)_tmp2F0 != 0)goto _LL20F;_LL20E: if(_tmp2BA == (void*)5)
return({struct _tuple0 _tmp30F;_tmp30F.f1=_tmp2D9;_tmp30F.f2=(void*)0;_tmp30F;});
else{return({struct _tuple0 _tmp310;_tmp310.f1=(void*)0;_tmp310.f2=_tmp2D9;_tmp310;});}
_LL20F: _tmp2F1=_tmp2E8.f1;if((int)_tmp2F1 != 0)goto _LL211;_tmp2F2=_tmp2E8.f2;if((
int)_tmp2F2 != 1)goto _LL211;_LL210: goto _LL212;_LL211: _tmp2F3=_tmp2E8.f1;if((int)
_tmp2F3 != 0)goto _LL213;_tmp2F4=_tmp2E8.f2;if((int)_tmp2F4 != 2)goto _LL213;_LL212:
goto _LL214;_LL213: _tmp2F5=_tmp2E8.f1;if((int)_tmp2F5 != 0)goto _LL215;_tmp2F6=
_tmp2E8.f2;if(_tmp2F6 <= (void*)3  || *((int*)_tmp2F6)!= 2)goto _LL215;_LL214: goto
_LL216;_LL215: _tmp2F7=_tmp2E8.f1;if((int)_tmp2F7 != 1)goto _LL217;_tmp2F8=_tmp2E8.f2;
if((int)_tmp2F8 != 0)goto _LL217;_LL216: goto _LL218;_LL217: _tmp2F9=_tmp2E8.f1;if((
int)_tmp2F9 != 2)goto _LL219;_tmp2FA=_tmp2E8.f2;if((int)_tmp2FA != 0)goto _LL219;
_LL218: goto _LL21A;_LL219: _tmp2FB=_tmp2E8.f1;if(_tmp2FB <= (void*)3  || *((int*)
_tmp2FB)!= 2)goto _LL21B;_tmp2FC=_tmp2E8.f2;if((int)_tmp2FC != 0)goto _LL21B;_LL21A:
if(_tmp2BA == (void*)6)return({struct _tuple0 _tmp311;_tmp311.f1=_tmp2D9;_tmp311.f2=(
void*)0;_tmp311;});else{return({struct _tuple0 _tmp312;_tmp312.f1=(void*)0;_tmp312.f2=
_tmp2D9;_tmp312;});}_LL21B:;_LL21C: goto _LL208;_LL208:;}{struct _tuple0 _tmp314=({
struct _tuple0 _tmp313;_tmp313.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2DF->topt))->v);_tmp313.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp2E0->topt))->v);_tmp313;});void*_tmp315;void*
_tmp316;void*_tmp317;void*_tmp318;void*_tmp319;void*_tmp31A;_LL22C: _tmp315=
_tmp314.f1;if(_tmp315 <= (void*)3  || *((int*)_tmp315)!= 5)goto _LL22E;_tmp316=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp315)->f1;if((int)_tmp316 != 1)goto
_LL22E;_LL22D: goto _LL22F;_LL22E: _tmp317=_tmp314.f2;if(_tmp317 <= (void*)3  || *((
int*)_tmp317)!= 5)goto _LL230;_tmp318=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp317)->f1;if((int)_tmp318 != 1)goto _LL230;_LL22F: goto _LL231;_LL230: _tmp319=
_tmp314.f1;if(_tmp319 <= (void*)3  || *((int*)_tmp319)!= 17)goto _LL232;_LL231: goto
_LL233;_LL232: _tmp31A=_tmp314.f2;if(_tmp31A <= (void*)3  || *((int*)_tmp31A)!= 17)
goto _LL234;_LL233: goto _LL22B;_LL234:;_LL235: return({struct _tuple0 _tmp31B;_tmp31B.f1=
_tmp2D9;_tmp31B.f2=_tmp2D9;_tmp31B;});_LL22B:;}{void*_tmp31C=_tmp2BA;_LL237: if((
int)_tmp31C != 5)goto _LL239;_LL238: {void*_tmp31D=Cyc_NewControlFlow_if_tagcmp(
env,_tmp2D9,_tmp2DF,_tmp2E1,(void*)5,_tmp2E2);_tmp31D=Cyc_NewControlFlow_if_tagcmp(
env,_tmp31D,_tmp2E0,_tmp2E2,(void*)5,_tmp2E1);return({struct _tuple0 _tmp31E;
_tmp31E.f1=_tmp31D;_tmp31E.f2=_tmp2D9;_tmp31E;});}_LL239: if((int)_tmp31C != 6)
goto _LL23B;_LL23A: {void*_tmp31F=Cyc_NewControlFlow_if_tagcmp(env,_tmp2D9,
_tmp2DF,_tmp2E1,(void*)5,_tmp2E2);_tmp31F=Cyc_NewControlFlow_if_tagcmp(env,
_tmp31F,_tmp2E0,_tmp2E2,(void*)5,_tmp2E1);return({struct _tuple0 _tmp320;_tmp320.f1=
_tmp2D9;_tmp320.f2=_tmp31F;_tmp320;});}_LL23B: if((int)_tmp31C != 7)goto _LL23D;
_LL23C: {void*_tmp321=Cyc_NewControlFlow_if_tagcmp(env,_tmp2D9,_tmp2E0,_tmp2E2,(
void*)8,_tmp2E1);void*_tmp322=Cyc_NewControlFlow_if_tagcmp(env,_tmp2D9,_tmp2DF,
_tmp2E1,(void*)10,_tmp2E2);return({struct _tuple0 _tmp323;_tmp323.f1=_tmp321;
_tmp323.f2=_tmp322;_tmp323;});}_LL23D: if((int)_tmp31C != 9)goto _LL23F;_LL23E: {
void*_tmp324=Cyc_NewControlFlow_if_tagcmp(env,_tmp2D9,_tmp2E0,_tmp2E2,(void*)10,
_tmp2E1);void*_tmp325=Cyc_NewControlFlow_if_tagcmp(env,_tmp2D9,_tmp2DF,_tmp2E1,(
void*)8,_tmp2E2);return({struct _tuple0 _tmp326;_tmp326.f1=_tmp324;_tmp326.f2=
_tmp325;_tmp326;});}_LL23F: if((int)_tmp31C != 8)goto _LL241;_LL240: {void*_tmp327=
Cyc_NewControlFlow_if_tagcmp(env,_tmp2D9,_tmp2DF,_tmp2E1,(void*)8,_tmp2E2);void*
_tmp328=Cyc_NewControlFlow_if_tagcmp(env,_tmp2D9,_tmp2E0,_tmp2E2,(void*)10,
_tmp2E1);{void*_tmp329=_tmp327;struct Cyc_Dict_Dict*_tmp32A;_LL246: if((int)
_tmp329 != 0)goto _LL248;_LL247:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B[0]=({struct Cyc_Core_Impossible_struct
_tmp32C;_tmp32C.tag=Cyc_Core_Impossible;_tmp32C.f1=({const char*_tmp32D="anal_test, Lt";
_tag_arr(_tmp32D,sizeof(char),_get_zero_arr_size(_tmp32D,14));});_tmp32C;});
_tmp32B;}));_LL248: if(_tmp329 <= (void*)1  || *((int*)_tmp329)!= 0)goto _LL245;
_tmp32A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp329)->f1;_LL249: _tmp2DC=
_tmp32A;_LL245:;}{void*_tmp32E=(void*)_tmp2DF->r;void*_tmp32F;struct Cyc_Absyn_Vardecl*
_tmp330;void*_tmp331;struct Cyc_Absyn_Vardecl*_tmp332;void*_tmp333;struct Cyc_Absyn_Vardecl*
_tmp334;void*_tmp335;struct Cyc_Absyn_Vardecl*_tmp336;_LL24B: if(*((int*)_tmp32E)
!= 1)goto _LL24D;_tmp32F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp32E)->f2;if(
_tmp32F <= (void*)1  || *((int*)_tmp32F)!= 0)goto _LL24D;_tmp330=((struct Cyc_Absyn_Global_b_struct*)
_tmp32F)->f1;if(!(!_tmp330->escapes))goto _LL24D;_LL24C: _tmp332=_tmp330;goto
_LL24E;_LL24D: if(*((int*)_tmp32E)!= 1)goto _LL24F;_tmp331=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp32E)->f2;if(_tmp331 <= (void*)1  || *((int*)_tmp331)!= 3)goto _LL24F;_tmp332=((
struct Cyc_Absyn_Local_b_struct*)_tmp331)->f1;if(!(!_tmp332->escapes))goto _LL24F;
_LL24E: _tmp334=_tmp332;goto _LL250;_LL24F: if(*((int*)_tmp32E)!= 1)goto _LL251;
_tmp333=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp32E)->f2;if(_tmp333 <= (void*)
1  || *((int*)_tmp333)!= 4)goto _LL251;_tmp334=((struct Cyc_Absyn_Pat_b_struct*)
_tmp333)->f1;if(!(!_tmp334->escapes))goto _LL251;_LL250: _tmp336=_tmp334;goto
_LL252;_LL251: if(*((int*)_tmp32E)!= 1)goto _LL253;_tmp335=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp32E)->f2;if(_tmp335 <= (void*)1  || *((int*)_tmp335)!= 2)goto _LL253;_tmp336=((
struct Cyc_Absyn_Param_b_struct*)_tmp335)->f1;if(!(!_tmp336->escapes))goto _LL253;
_LL252: {void*_tmp337=(void*)_tmp2E0->r;void*_tmp338;struct Cyc_Absyn_Vardecl*
_tmp339;void*_tmp33A;struct Cyc_Absyn_Vardecl*_tmp33B;void*_tmp33C;struct Cyc_Absyn_Vardecl*
_tmp33D;void*_tmp33E;struct Cyc_Absyn_Vardecl*_tmp33F;void*_tmp340;int _tmp341;
void*_tmp342;struct Cyc_List_List*_tmp343;struct Cyc_List_List _tmp344;struct Cyc_Absyn_Exp*
_tmp345;_LL256: if(*((int*)_tmp337)!= 1)goto _LL258;_tmp338=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp337)->f2;if(_tmp338 <= (void*)1  || *((int*)_tmp338)!= 0)goto _LL258;_tmp339=((
struct Cyc_Absyn_Global_b_struct*)_tmp338)->f1;if(!(!_tmp339->escapes))goto _LL258;
_LL257: _tmp33B=_tmp339;goto _LL259;_LL258: if(*((int*)_tmp337)!= 1)goto _LL25A;
_tmp33A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp337)->f2;if(_tmp33A <= (void*)
1  || *((int*)_tmp33A)!= 3)goto _LL25A;_tmp33B=((struct Cyc_Absyn_Local_b_struct*)
_tmp33A)->f1;if(!(!_tmp33B->escapes))goto _LL25A;_LL259: _tmp33D=_tmp33B;goto
_LL25B;_LL25A: if(*((int*)_tmp337)!= 1)goto _LL25C;_tmp33C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp337)->f2;if(_tmp33C <= (void*)1  || *((int*)_tmp33C)!= 4)goto _LL25C;_tmp33D=((
struct Cyc_Absyn_Pat_b_struct*)_tmp33C)->f1;if(!(!_tmp33D->escapes))goto _LL25C;
_LL25B: _tmp33F=_tmp33D;goto _LL25D;_LL25C: if(*((int*)_tmp337)!= 1)goto _LL25E;
_tmp33E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp337)->f2;if(_tmp33E <= (void*)
1  || *((int*)_tmp33E)!= 2)goto _LL25E;_tmp33F=((struct Cyc_Absyn_Param_b_struct*)
_tmp33E)->f1;if(!(!_tmp33F->escapes))goto _LL25E;_LL25D: {struct Cyc_List_List*
_tmp346=({struct Cyc_List_List*_tmp34A=_cycalloc(sizeof(*_tmp34A));_tmp34A->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B->vd=
_tmp336;_tmp34B->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp34D;_tmp34D.tag=1;_tmp34D.f1=_tmp33F;_tmp34D;});_tmp34C;}));_tmp34B;});
_tmp34A->tl=_tmp2DD;_tmp34A;});return({struct _tuple0 _tmp347;_tmp347.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp348=_cycalloc(sizeof(*_tmp348));
_tmp348[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp349;_tmp349.tag=0;
_tmp349.f1=_tmp2DC;_tmp349.f2=_tmp346;_tmp349;});_tmp348;});_tmp347.f2=_tmp328;
_tmp347;});}_LL25E: if(*((int*)_tmp337)!= 0)goto _LL260;_tmp340=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp337)->f1;if(_tmp340 <= (void*)1  || *((int*)_tmp340)!= 2)goto _LL260;_tmp341=((
struct Cyc_Absyn_Int_c_struct*)_tmp340)->f2;_LL25F: {struct Cyc_List_List*_tmp34E=({
struct Cyc_List_List*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353->vd=_tmp336;_tmp353->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessConst_struct*_tmp354=_cycalloc_atomic(sizeof(*
_tmp354));_tmp354[0]=({struct Cyc_CfFlowInfo_LessConst_struct _tmp355;_tmp355.tag=
3;_tmp355.f1=(unsigned int)_tmp341;_tmp355;});_tmp354;}));_tmp353;});_tmp352->tl=
_tmp2DD;_tmp352;});return({struct _tuple0 _tmp34F;_tmp34F.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp351;_tmp351.tag=0;_tmp351.f1=_tmp2DC;_tmp351.f2=_tmp34E;_tmp351;});_tmp350;});
_tmp34F.f2=_tmp328;_tmp34F;});}_LL260: if(*((int*)_tmp337)!= 3)goto _LL262;_tmp342=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp337)->f1;if((int)_tmp342 != 19)goto
_LL262;_tmp343=((struct Cyc_Absyn_Primop_e_struct*)_tmp337)->f2;if(_tmp343 == 0)
goto _LL262;_tmp344=*_tmp343;_tmp345=(struct Cyc_Absyn_Exp*)_tmp344.hd;_LL261: {
void*_tmp356=(void*)_tmp345->r;void*_tmp357;struct Cyc_Absyn_Vardecl*_tmp358;void*
_tmp359;struct Cyc_Absyn_Vardecl*_tmp35A;void*_tmp35B;struct Cyc_Absyn_Vardecl*
_tmp35C;void*_tmp35D;struct Cyc_Absyn_Vardecl*_tmp35E;_LL265: if(*((int*)_tmp356)
!= 1)goto _LL267;_tmp357=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp356)->f2;if(
_tmp357 <= (void*)1  || *((int*)_tmp357)!= 0)goto _LL267;_tmp358=((struct Cyc_Absyn_Global_b_struct*)
_tmp357)->f1;if(!(!_tmp358->escapes))goto _LL267;_LL266: _tmp35A=_tmp358;goto
_LL268;_LL267: if(*((int*)_tmp356)!= 1)goto _LL269;_tmp359=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp356)->f2;if(_tmp359 <= (void*)1  || *((int*)_tmp359)!= 3)goto _LL269;_tmp35A=((
struct Cyc_Absyn_Local_b_struct*)_tmp359)->f1;if(!(!_tmp35A->escapes))goto _LL269;
_LL268: _tmp35C=_tmp35A;goto _LL26A;_LL269: if(*((int*)_tmp356)!= 1)goto _LL26B;
_tmp35B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp356)->f2;if(_tmp35B <= (void*)
1  || *((int*)_tmp35B)!= 4)goto _LL26B;_tmp35C=((struct Cyc_Absyn_Pat_b_struct*)
_tmp35B)->f1;if(!(!_tmp35C->escapes))goto _LL26B;_LL26A: _tmp35E=_tmp35C;goto
_LL26C;_LL26B: if(*((int*)_tmp356)!= 1)goto _LL26D;_tmp35D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp356)->f2;if(_tmp35D <= (void*)1  || *((int*)_tmp35D)!= 2)goto _LL26D;_tmp35E=((
struct Cyc_Absyn_Param_b_struct*)_tmp35D)->f1;if(!(!_tmp35E->escapes))goto _LL26D;
_LL26C: {struct Cyc_List_List*_tmp35F=({struct Cyc_List_List*_tmp363=_cycalloc(
sizeof(*_tmp363));_tmp363->hd=({struct Cyc_CfFlowInfo_Reln*_tmp364=_cycalloc(
sizeof(*_tmp364));_tmp364->vd=_tmp336;_tmp364->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp365=_cycalloc(sizeof(*_tmp365));_tmp365[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp366;_tmp366.tag=2;_tmp366.f1=_tmp35E;_tmp366;});_tmp365;}));_tmp364;});
_tmp363->tl=_tmp2DD;_tmp363;});return({struct _tuple0 _tmp360;_tmp360.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp361=_cycalloc(sizeof(*_tmp361));
_tmp361[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp362;_tmp362.tag=0;
_tmp362.f1=_tmp2DC;_tmp362.f2=_tmp35F;_tmp362;});_tmp361;});_tmp360.f2=_tmp328;
_tmp360;});}_LL26D:;_LL26E: return({struct _tuple0 _tmp367;_tmp367.f1=_tmp327;
_tmp367.f2=_tmp328;_tmp367;});_LL264:;}_LL262:;_LL263: return({struct _tuple0
_tmp368;_tmp368.f1=_tmp327;_tmp368.f2=_tmp328;_tmp368;});_LL255:;}_LL253:;_LL254:
return({struct _tuple0 _tmp369;_tmp369.f1=_tmp327;_tmp369.f2=_tmp328;_tmp369;});
_LL24A:;}}_LL241: if((int)_tmp31C != 10)goto _LL243;_LL242: {void*_tmp36A=Cyc_NewControlFlow_if_tagcmp(
env,_tmp2D9,_tmp2DF,_tmp2E1,(void*)10,_tmp2E2);void*_tmp36B=Cyc_NewControlFlow_if_tagcmp(
env,_tmp2D9,_tmp2E0,_tmp2E1,(void*)8,_tmp2E1);{void*_tmp36C=_tmp36A;struct Cyc_Dict_Dict*
_tmp36D;_LL270: if((int)_tmp36C != 0)goto _LL272;_LL271:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E[0]=({
struct Cyc_Core_Impossible_struct _tmp36F;_tmp36F.tag=Cyc_Core_Impossible;_tmp36F.f1=({
const char*_tmp370="anal_test, Lte";_tag_arr(_tmp370,sizeof(char),
_get_zero_arr_size(_tmp370,15));});_tmp36F;});_tmp36E;}));_LL272: if(_tmp36C <= (
void*)1  || *((int*)_tmp36C)!= 0)goto _LL26F;_tmp36D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp36C)->f1;_LL273: _tmp2DC=_tmp36D;_LL26F:;}{void*_tmp371=(void*)_tmp2DF->r;
void*_tmp372;struct Cyc_Absyn_Vardecl*_tmp373;void*_tmp374;struct Cyc_Absyn_Vardecl*
_tmp375;void*_tmp376;struct Cyc_Absyn_Vardecl*_tmp377;void*_tmp378;struct Cyc_Absyn_Vardecl*
_tmp379;_LL275: if(*((int*)_tmp371)!= 1)goto _LL277;_tmp372=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp371)->f2;if(_tmp372 <= (void*)1  || *((int*)_tmp372)!= 0)goto _LL277;_tmp373=((
struct Cyc_Absyn_Global_b_struct*)_tmp372)->f1;if(!(!_tmp373->escapes))goto _LL277;
_LL276: _tmp375=_tmp373;goto _LL278;_LL277: if(*((int*)_tmp371)!= 1)goto _LL279;
_tmp374=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp371)->f2;if(_tmp374 <= (void*)
1  || *((int*)_tmp374)!= 3)goto _LL279;_tmp375=((struct Cyc_Absyn_Local_b_struct*)
_tmp374)->f1;if(!(!_tmp375->escapes))goto _LL279;_LL278: _tmp377=_tmp375;goto
_LL27A;_LL279: if(*((int*)_tmp371)!= 1)goto _LL27B;_tmp376=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp371)->f2;if(_tmp376 <= (void*)1  || *((int*)_tmp376)!= 4)goto _LL27B;_tmp377=((
struct Cyc_Absyn_Pat_b_struct*)_tmp376)->f1;if(!(!_tmp377->escapes))goto _LL27B;
_LL27A: _tmp379=_tmp377;goto _LL27C;_LL27B: if(*((int*)_tmp371)!= 1)goto _LL27D;
_tmp378=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp371)->f2;if(_tmp378 <= (void*)
1  || *((int*)_tmp378)!= 2)goto _LL27D;_tmp379=((struct Cyc_Absyn_Param_b_struct*)
_tmp378)->f1;if(!(!_tmp379->escapes))goto _LL27D;_LL27C: {void*_tmp37A=(void*)
_tmp2E0->r;void*_tmp37B;struct Cyc_List_List*_tmp37C;struct Cyc_List_List _tmp37D;
struct Cyc_Absyn_Exp*_tmp37E;_LL280: if(*((int*)_tmp37A)!= 3)goto _LL282;_tmp37B=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp37A)->f1;if((int)_tmp37B != 19)goto
_LL282;_tmp37C=((struct Cyc_Absyn_Primop_e_struct*)_tmp37A)->f2;if(_tmp37C == 0)
goto _LL282;_tmp37D=*_tmp37C;_tmp37E=(struct Cyc_Absyn_Exp*)_tmp37D.hd;_LL281: {
void*_tmp37F=(void*)_tmp37E->r;void*_tmp380;struct Cyc_Absyn_Vardecl*_tmp381;void*
_tmp382;struct Cyc_Absyn_Vardecl*_tmp383;void*_tmp384;struct Cyc_Absyn_Vardecl*
_tmp385;void*_tmp386;struct Cyc_Absyn_Vardecl*_tmp387;_LL285: if(*((int*)_tmp37F)
!= 1)goto _LL287;_tmp380=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp37F)->f2;if(
_tmp380 <= (void*)1  || *((int*)_tmp380)!= 0)goto _LL287;_tmp381=((struct Cyc_Absyn_Global_b_struct*)
_tmp380)->f1;if(!(!_tmp381->escapes))goto _LL287;_LL286: _tmp383=_tmp381;goto
_LL288;_LL287: if(*((int*)_tmp37F)!= 1)goto _LL289;_tmp382=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp37F)->f2;if(_tmp382 <= (void*)1  || *((int*)_tmp382)!= 3)goto _LL289;_tmp383=((
struct Cyc_Absyn_Local_b_struct*)_tmp382)->f1;if(!(!_tmp383->escapes))goto _LL289;
_LL288: _tmp385=_tmp383;goto _LL28A;_LL289: if(*((int*)_tmp37F)!= 1)goto _LL28B;
_tmp384=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp37F)->f2;if(_tmp384 <= (void*)
1  || *((int*)_tmp384)!= 4)goto _LL28B;_tmp385=((struct Cyc_Absyn_Pat_b_struct*)
_tmp384)->f1;if(!(!_tmp385->escapes))goto _LL28B;_LL28A: _tmp387=_tmp385;goto
_LL28C;_LL28B: if(*((int*)_tmp37F)!= 1)goto _LL28D;_tmp386=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp37F)->f2;if(_tmp386 <= (void*)1  || *((int*)_tmp386)!= 2)goto _LL28D;_tmp387=((
struct Cyc_Absyn_Param_b_struct*)_tmp386)->f1;if(!(!_tmp387->escapes))goto _LL28D;
_LL28C: {struct Cyc_List_List*_tmp388=({struct Cyc_List_List*_tmp38C=_cycalloc(
sizeof(*_tmp38C));_tmp38C->hd=({struct Cyc_CfFlowInfo_Reln*_tmp38D=_cycalloc(
sizeof(*_tmp38D));_tmp38D->vd=_tmp379;_tmp38D->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp38F;_tmp38F.tag=4;_tmp38F.f1=_tmp387;_tmp38F;});_tmp38E;}));_tmp38D;});
_tmp38C->tl=_tmp2DD;_tmp38C;});return({struct _tuple0 _tmp389;_tmp389.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp38A=_cycalloc(sizeof(*_tmp38A));
_tmp38A[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp38B;_tmp38B.tag=0;
_tmp38B.f1=_tmp2DC;_tmp38B.f2=_tmp388;_tmp38B;});_tmp38A;});_tmp389.f2=_tmp36B;
_tmp389;});}_LL28D:;_LL28E: return({struct _tuple0 _tmp390;_tmp390.f1=_tmp36A;
_tmp390.f2=_tmp36B;_tmp390;});_LL284:;}_LL282:;_LL283: return({struct _tuple0
_tmp391;_tmp391.f1=_tmp36A;_tmp391.f2=_tmp36B;_tmp391;});_LL27F:;}_LL27D:;_LL27E:
return({struct _tuple0 _tmp392;_tmp392.f1=_tmp36A;_tmp392.f2=_tmp36B;_tmp392;});
_LL274:;}}_LL243:;_LL244: return({struct _tuple0 _tmp393;_tmp393.f1=_tmp2D9;_tmp393.f2=
_tmp2D9;_tmp393;});_LL236:;}}_LL203:;}}_LL201:;_LL202: goto _LL1F4;_LL1F4:;}{void*
_tmp395;void*_tmp396;struct _tuple0 _tmp394=Cyc_NewControlFlow_anal_Rexp(env,
inflow,e);_tmp395=_tmp394.f1;_tmp396=_tmp394.f2;{void*_tmp397=_tmp395;struct Cyc_Dict_Dict*
_tmp398;struct Cyc_List_List*_tmp399;_LL290: if((int)_tmp397 != 0)goto _LL292;_LL291:
return({struct _tuple0 _tmp39A;_tmp39A.f1=_tmp395;_tmp39A.f2=_tmp395;_tmp39A;});
_LL292: if(_tmp397 <= (void*)1  || *((int*)_tmp397)!= 0)goto _LL28F;_tmp398=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp397)->f1;_tmp399=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp397)->f2;_LL293: {void*_tmp39B=_tmp396;void*_tmp39C;void*_tmp39D;void*
_tmp39E;_LL295: if((int)_tmp39B != 0)goto _LL297;_LL296: return({struct _tuple0
_tmp39F;_tmp39F.f1=(void*)0;_tmp39F.f2=_tmp395;_tmp39F;});_LL297: if((int)_tmp39B
!= 2)goto _LL299;_LL298: goto _LL29A;_LL299: if((int)_tmp39B != 1)goto _LL29B;_LL29A:
goto _LL29C;_LL29B: if(_tmp39B <= (void*)3  || *((int*)_tmp39B)!= 2)goto _LL29D;
_LL29C: return({struct _tuple0 _tmp3A0;_tmp3A0.f1=_tmp395;_tmp3A0.f2=(void*)0;
_tmp3A0;});_LL29D: if(_tmp39B <= (void*)3  || *((int*)_tmp39B)!= 0)goto _LL29F;
_tmp39C=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp39B)->f1;if((int)
_tmp39C != 0)goto _LL29F;_LL29E: goto _LL2A0;_LL29F: if(_tmp39B <= (void*)3  || *((int*)
_tmp39B)!= 1)goto _LL2A1;_tmp39D=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp39B)->f1;if((int)_tmp39D != 0)goto _LL2A1;_LL2A0:({void*_tmp3A1[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp3A2="expression may not be initialized";_tag_arr(_tmp3A2,
sizeof(char),_get_zero_arr_size(_tmp3A2,34));}),_tag_arr(_tmp3A1,sizeof(void*),0));});
return({struct _tuple0 _tmp3A3;_tmp3A3.f1=(void*)0;_tmp3A3.f2=(void*)0;_tmp3A3;});
_LL2A1: if(_tmp39B <= (void*)3  || *((int*)_tmp39B)!= 0)goto _LL2A3;_tmp39E=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp39B)->f1;_LL2A2: return Cyc_NewControlFlow_splitzero(
env,inflow,_tmp395,e,_tmp39E);_LL2A3: if(_tmp39B <= (void*)3  || *((int*)_tmp39B)!= 
1)goto _LL2A5;_LL2A4: return({struct _tuple0 _tmp3A4;_tmp3A4.f1=_tmp395;_tmp3A4.f2=
_tmp395;_tmp3A4;});_LL2A5: if(_tmp39B <= (void*)3  || *((int*)_tmp39B)!= 3)goto
_LL2A7;_LL2A6: return({struct _tuple0 _tmp3A5;_tmp3A5.f1=_tmp395;_tmp3A5.f2=_tmp395;
_tmp3A5;});_LL2A7: if(_tmp39B <= (void*)3  || *((int*)_tmp39B)!= 4)goto _LL294;
_LL2A8:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3A6=_cycalloc(
sizeof(*_tmp3A6));_tmp3A6[0]=({struct Cyc_Core_Impossible_struct _tmp3A7;_tmp3A7.tag=
Cyc_Core_Impossible;_tmp3A7.f1=({const char*_tmp3A8="anal_test";_tag_arr(_tmp3A8,
sizeof(char),_get_zero_arr_size(_tmp3A8,10));});_tmp3A7;});_tmp3A6;}));_LL294:;}
_LL28F:;}}}static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*
loc,struct Cyc_NewControlFlow_AnalEnv*env,void*flow){void*_tmp3A9=flow;struct Cyc_Dict_Dict*
_tmp3AA;_LL2AA: if((int)_tmp3A9 != 0)goto _LL2AC;_LL2AB: return;_LL2AC: if(_tmp3A9 <= (
void*)1  || *((int*)_tmp3A9)!= 0)goto _LL2A9;_tmp3AA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp3A9)->f1;_LL2AD:{struct Cyc_List_List*_tmp3AB=env->param_roots;for(0;_tmp3AB
!= 0;_tmp3AB=_tmp3AB->tl){if(Cyc_CfFlowInfo_initlevel(_tmp3AA,Cyc_CfFlowInfo_lookup_place(
_tmp3AA,(struct Cyc_CfFlowInfo_Place*)_tmp3AB->hd))!= (void*)2)({void*_tmp3AC[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp3AD="function may not initialize all the parameters with attribute 'initializes'";
_tag_arr(_tmp3AD,sizeof(char),_get_zero_arr_size(_tmp3AD,76));}),_tag_arr(
_tmp3AC,sizeof(void*),0));});}}return;_LL2A9:;}static void*Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_List_List*scs){for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp3AF;struct Cyc_Core_Opt*
_tmp3B0;struct Cyc_Absyn_Exp*_tmp3B1;struct Cyc_Absyn_Stmt*_tmp3B2;struct Cyc_Absyn_Switch_clause*
_tmp3AE=(struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp3AF=*_tmp3AE;_tmp3B0=_tmp3AF.pat_vars;
_tmp3B1=_tmp3AF.where_clause;_tmp3B2=_tmp3AF.body;{void*clause_inflow=Cyc_NewControlFlow_add_vars(
inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3B0))->v,Cyc_CfFlowInfo_unknown_all);
void*clause_outflow;if(_tmp3B1 != 0){void*_tmp3B4;void*_tmp3B5;struct _tuple0
_tmp3B3=Cyc_NewControlFlow_anal_test(env,clause_inflow,(struct Cyc_Absyn_Exp*)
_tmp3B1);_tmp3B4=_tmp3B3.f1;_tmp3B5=_tmp3B3.f2;inflow=_tmp3B5;clause_outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp3B4,_tmp3B2);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp3B2);}{void*_tmp3B6=clause_outflow;_LL2AF: if((int)_tmp3B6 != 0)
goto _LL2B1;_LL2B0: goto _LL2AE;_LL2B1:;_LL2B2: if(scs->tl == 0)return clause_outflow;
else{if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*
_tmp3B7[0]={};Cyc_Tcutil_terr(_tmp3B2->loc,({const char*_tmp3B8="switch clause may implicitly fallthru";
_tag_arr(_tmp3B8,sizeof(char),_get_zero_arr_size(_tmp3B8,38));}),_tag_arr(
_tmp3B7,sizeof(void*),0));});else{({void*_tmp3B9[0]={};Cyc_Tcutil_warn(_tmp3B2->loc,({
const char*_tmp3BA="switch clause may implicitly fallthru";_tag_arr(_tmp3BA,
sizeof(char),_get_zero_arr_size(_tmp3BA,38));}),_tag_arr(_tmp3B9,sizeof(void*),0));});}
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->body,clause_outflow);}goto _LL2AE;_LL2AE:;}}}return(
void*)0;}static void*Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp3BB=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);inflow=(void*)_tmp3BB->flow;{
void*_tmp3BC=(void*)s->r;struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_Absyn_Exp*_tmp3BE;
struct Cyc_Absyn_Exp*_tmp3BF;struct Cyc_Absyn_Stmt*_tmp3C0;struct Cyc_Absyn_Stmt*
_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_Absyn_Stmt*_tmp3C3;struct Cyc_Absyn_Stmt*
_tmp3C4;struct _tuple3 _tmp3C5;struct Cyc_Absyn_Exp*_tmp3C6;struct Cyc_Absyn_Stmt*
_tmp3C7;struct Cyc_Absyn_Stmt*_tmp3C8;struct Cyc_Absyn_Stmt*_tmp3C9;struct _tuple3
_tmp3CA;struct Cyc_Absyn_Exp*_tmp3CB;struct Cyc_Absyn_Stmt*_tmp3CC;struct Cyc_Absyn_Exp*
_tmp3CD;struct _tuple3 _tmp3CE;struct Cyc_Absyn_Exp*_tmp3CF;struct Cyc_Absyn_Stmt*
_tmp3D0;struct _tuple3 _tmp3D1;struct Cyc_Absyn_Exp*_tmp3D2;struct Cyc_Absyn_Stmt*
_tmp3D3;struct Cyc_Absyn_Stmt*_tmp3D4;struct Cyc_Absyn_Stmt*_tmp3D5;struct Cyc_List_List*
_tmp3D6;struct Cyc_Absyn_Switch_clause**_tmp3D7;struct Cyc_Absyn_Switch_clause*
_tmp3D8;struct Cyc_Absyn_Stmt*_tmp3D9;struct Cyc_Absyn_Stmt*_tmp3DA;struct Cyc_Absyn_Stmt*
_tmp3DB;struct Cyc_Absyn_Exp*_tmp3DC;struct Cyc_List_List*_tmp3DD;struct Cyc_Absyn_Stmt*
_tmp3DE;struct Cyc_List_List*_tmp3DF;struct Cyc_Absyn_Decl*_tmp3E0;struct Cyc_Absyn_Stmt*
_tmp3E1;struct Cyc_Absyn_Stmt*_tmp3E2;struct Cyc_Absyn_Tvar*_tmp3E3;struct Cyc_Absyn_Vardecl*
_tmp3E4;int _tmp3E5;struct Cyc_Absyn_Stmt*_tmp3E6;struct Cyc_Absyn_Exp*_tmp3E7;
_LL2B4: if((int)_tmp3BC != 0)goto _LL2B6;_LL2B5: return inflow;_LL2B6: if(_tmp3BC <= (
void*)1  || *((int*)_tmp3BC)!= 2)goto _LL2B8;_tmp3BD=((struct Cyc_Absyn_Return_s_struct*)
_tmp3BC)->f1;if(_tmp3BD != 0)goto _LL2B8;_LL2B7: if(env->noreturn)({void*_tmp3E8[0]={};
Cyc_Tcutil_terr(s->loc,({const char*_tmp3E9="`noreturn' function might return";
_tag_arr(_tmp3E9,sizeof(char),_get_zero_arr_size(_tmp3E9,33));}),_tag_arr(
_tmp3E8,sizeof(void*),0));});Cyc_NewControlFlow_check_init_params(s->loc,env,
inflow);return(void*)0;_LL2B8: if(_tmp3BC <= (void*)1  || *((int*)_tmp3BC)!= 2)goto
_LL2BA;_tmp3BE=((struct Cyc_Absyn_Return_s_struct*)_tmp3BC)->f1;_LL2B9: if(env->noreturn)({
void*_tmp3EA[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp3EB="`noreturn' function might return";
_tag_arr(_tmp3EB,sizeof(char),_get_zero_arr_size(_tmp3EB,33));}),_tag_arr(
_tmp3EA,sizeof(void*),0));});{void*_tmp3ED;void*_tmp3EE;struct _tuple0 _tmp3EC=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp3BE));_tmp3ED=_tmp3EC.f1;
_tmp3EE=_tmp3EC.f2;_tmp3ED=Cyc_NewControlFlow_use_Rval(env,_tmp3BE->loc,_tmp3ED,
_tmp3EE);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp3ED);return(void*)0;}
_LL2BA: if(_tmp3BC <= (void*)1  || *((int*)_tmp3BC)!= 0)goto _LL2BC;_tmp3BF=((struct
Cyc_Absyn_Exp_s_struct*)_tmp3BC)->f1;_LL2BB: return(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp3BF)).f1;_LL2BC: if(_tmp3BC <= (void*)1  || *((int*)_tmp3BC)!= 1)
goto _LL2BE;_tmp3C0=((struct Cyc_Absyn_Seq_s_struct*)_tmp3BC)->f1;_tmp3C1=((struct
Cyc_Absyn_Seq_s_struct*)_tmp3BC)->f2;_LL2BD: return Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp3C0),_tmp3C1);_LL2BE: if(_tmp3BC
<= (void*)1  || *((int*)_tmp3BC)!= 3)goto _LL2C0;_tmp3C2=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp3BC)->f1;_tmp3C3=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3BC)->f2;_tmp3C4=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3BC)->f3;_LL2BF: {void*_tmp3F0;void*
_tmp3F1;struct _tuple0 _tmp3EF=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3C2);
_tmp3F0=_tmp3EF.f1;_tmp3F1=_tmp3EF.f2;return Cyc_CfFlowInfo_join_flow(env->all_changed,
Cyc_NewControlFlow_anal_stmt(env,_tmp3F0,_tmp3C3),Cyc_NewControlFlow_anal_stmt(
env,_tmp3F1,_tmp3C4));}_LL2C0: if(_tmp3BC <= (void*)1  || *((int*)_tmp3BC)!= 4)goto
_LL2C2;_tmp3C5=((struct Cyc_Absyn_While_s_struct*)_tmp3BC)->f1;_tmp3C6=_tmp3C5.f1;
_tmp3C7=_tmp3C5.f2;_tmp3C8=((struct Cyc_Absyn_While_s_struct*)_tmp3BC)->f2;_LL2C1: {
void*_tmp3F2=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp3C7))->flow;
void*_tmp3F4;void*_tmp3F5;struct _tuple0 _tmp3F3=Cyc_NewControlFlow_anal_test(env,
_tmp3F2,_tmp3C6);_tmp3F4=_tmp3F3.f1;_tmp3F5=_tmp3F3.f2;{void*_tmp3F6=Cyc_NewControlFlow_anal_stmt(
env,_tmp3F4,_tmp3C8);Cyc_NewControlFlow_update_flow(env,_tmp3C7,_tmp3F6);return
_tmp3F5;}}_LL2C2: if(_tmp3BC <= (void*)1  || *((int*)_tmp3BC)!= 16)goto _LL2C4;
_tmp3C9=((struct Cyc_Absyn_Do_s_struct*)_tmp3BC)->f1;_tmp3CA=((struct Cyc_Absyn_Do_s_struct*)
_tmp3BC)->f2;_tmp3CB=_tmp3CA.f1;_tmp3CC=_tmp3CA.f2;_LL2C3: {void*_tmp3F7=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp3C9);void*_tmp3F8=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp3F7,_tmp3CC))->flow;void*_tmp3FA;void*_tmp3FB;struct _tuple0 _tmp3F9=Cyc_NewControlFlow_anal_test(
env,_tmp3F8,_tmp3CB);_tmp3FA=_tmp3F9.f1;_tmp3FB=_tmp3F9.f2;Cyc_NewControlFlow_update_flow(
env,_tmp3C9,_tmp3FA);return _tmp3FB;}_LL2C4: if(_tmp3BC <= (void*)1  || *((int*)
_tmp3BC)!= 8)goto _LL2C6;_tmp3CD=((struct Cyc_Absyn_For_s_struct*)_tmp3BC)->f1;
_tmp3CE=((struct Cyc_Absyn_For_s_struct*)_tmp3BC)->f2;_tmp3CF=_tmp3CE.f1;_tmp3D0=
_tmp3CE.f2;_tmp3D1=((struct Cyc_Absyn_For_s_struct*)_tmp3BC)->f3;_tmp3D2=_tmp3D1.f1;
_tmp3D3=_tmp3D1.f2;_tmp3D4=((struct Cyc_Absyn_For_s_struct*)_tmp3BC)->f4;_LL2C5: {
void*_tmp3FC=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp3CD)).f1;void*_tmp3FD=(
void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp3FC,_tmp3D0))->flow;void*
_tmp3FF;void*_tmp400;struct _tuple0 _tmp3FE=Cyc_NewControlFlow_anal_test(env,
_tmp3FD,_tmp3CF);_tmp3FF=_tmp3FE.f1;_tmp400=_tmp3FE.f2;{void*_tmp401=Cyc_NewControlFlow_anal_stmt(
env,_tmp3FF,_tmp3D4);void*_tmp402=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp401,_tmp3D3))->flow;void*_tmp403=(Cyc_NewControlFlow_anal_Rexp(env,_tmp402,
_tmp3D2)).f1;Cyc_NewControlFlow_update_flow(env,_tmp3D0,_tmp403);return _tmp400;}}
_LL2C6: if(_tmp3BC <= (void*)1  || *((int*)_tmp3BC)!= 5)goto _LL2C8;_tmp3D5=((struct
Cyc_Absyn_Break_s_struct*)_tmp3BC)->f1;if(_tmp3D5 != 0)goto _LL2C8;_LL2C7: return(
void*)0;_LL2C8: if(_tmp3BC <= (void*)1  || *((int*)_tmp3BC)!= 11)goto _LL2CA;_tmp3D6=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp3BC)->f1;_tmp3D7=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp3BC)->f2;if(_tmp3D7 == 0)goto _LL2CA;_tmp3D8=*_tmp3D7;_LL2C9: {void*_tmp405;
struct Cyc_List_List*_tmp406;struct _tuple5 _tmp404=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp3D6);_tmp405=_tmp404.f1;_tmp406=_tmp404.f2;for(0;_tmp406 != 0;(
_tmp406=_tmp406->tl,_tmp3D6=_tmp3D6->tl)){_tmp405=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3D6))->hd)->loc,
_tmp405,(void*)_tmp406->hd);}_tmp405=Cyc_NewControlFlow_add_vars(_tmp405,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D8->pat_vars))->v,Cyc_CfFlowInfo_unknown_all);
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp3D8->body,_tmp405);
return(void*)0;}_LL2CA: if(_tmp3BC <= (void*)1  || *((int*)_tmp3BC)!= 5)goto _LL2CC;
_tmp3D9=((struct Cyc_Absyn_Break_s_struct*)_tmp3BC)->f1;_LL2CB: _tmp3DA=_tmp3D9;
goto _LL2CD;_LL2CC: if(_tmp3BC <= (void*)1  || *((int*)_tmp3BC)!= 6)goto _LL2CE;
_tmp3DA=((struct Cyc_Absyn_Continue_s_struct*)_tmp3BC)->f1;_LL2CD: _tmp3DB=_tmp3DA;
goto _LL2CF;_LL2CE: if(_tmp3BC <= (void*)1  || *((int*)_tmp3BC)!= 7)goto _LL2D0;
_tmp3DB=((struct Cyc_Absyn_Goto_s_struct*)_tmp3BC)->f2;_LL2CF: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp407=_tmp3BB->encloser;struct Cyc_Absyn_Stmt*
_tmp408=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp3DB)))->encloser;while(_tmp408 != _tmp407){struct Cyc_Absyn_Stmt*_tmp409=(Cyc_NewControlFlow_get_stmt_annot(
_tmp407))->encloser;if(_tmp409 == _tmp407){({void*_tmp40A[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp40B="goto enters local scope or exception handler";_tag_arr(
_tmp40B,sizeof(char),_get_zero_arr_size(_tmp40B,45));}),_tag_arr(_tmp40A,sizeof(
void*),0));});break;}_tmp407=_tmp409;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp3DB),inflow);return(void*)0;_LL2D0: if(
_tmp3BC <= (void*)1  || *((int*)_tmp3BC)!= 9)goto _LL2D2;_tmp3DC=((struct Cyc_Absyn_Switch_s_struct*)
_tmp3BC)->f1;_tmp3DD=((struct Cyc_Absyn_Switch_s_struct*)_tmp3BC)->f2;_LL2D1: {
void*_tmp40D;void*_tmp40E;struct _tuple0 _tmp40C=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp3DC);_tmp40D=_tmp40C.f1;_tmp40E=_tmp40C.f2;_tmp40D=Cyc_NewControlFlow_use_Rval(
env,_tmp3DC->loc,_tmp40D,_tmp40E);return Cyc_NewControlFlow_anal_scs(env,_tmp40D,
_tmp3DD);}_LL2D2: if(_tmp3BC <= (void*)1  || *((int*)_tmp3BC)!= 17)goto _LL2D4;
_tmp3DE=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp3BC)->f1;_tmp3DF=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp3BC)->f2;_LL2D3: {int old_in_try=env->in_try;void*old_tryflow=(void*)env->tryflow;
env->in_try=1;(void*)(env->tryflow=(void*)inflow);{void*s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp3DE);void*scs_inflow=(void*)env->tryflow;env->in_try=old_in_try;(
void*)(env->tryflow=(void*)old_tryflow);Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{void*scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp3DF);{
void*_tmp40F=scs_outflow;_LL2DF: if((int)_tmp40F != 0)goto _LL2E1;_LL2E0: goto _LL2DE;
_LL2E1:;_LL2E2:({void*_tmp410[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp411="last catch clause may implicitly fallthru";
_tag_arr(_tmp411,sizeof(char),_get_zero_arr_size(_tmp411,42));}),_tag_arr(
_tmp410,sizeof(void*),0));});_LL2DE:;}return s1_outflow;}}}_LL2D4: if(_tmp3BC <= (
void*)1  || *((int*)_tmp3BC)!= 12)goto _LL2D6;_tmp3E0=((struct Cyc_Absyn_Decl_s_struct*)
_tmp3BC)->f1;_tmp3E1=((struct Cyc_Absyn_Decl_s_struct*)_tmp3BC)->f2;_LL2D5: return
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp3E0),
_tmp3E1);_LL2D6: if(_tmp3BC <= (void*)1  || *((int*)_tmp3BC)!= 15)goto _LL2D8;
_tmp3E2=((struct Cyc_Absyn_Label_s_struct*)_tmp3BC)->f2;_LL2D7: return Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp3E2);_LL2D8: if(_tmp3BC <= (void*)1  || *((int*)_tmp3BC)!= 18)goto
_LL2DA;_tmp3E3=((struct Cyc_Absyn_Region_s_struct*)_tmp3BC)->f1;_tmp3E4=((struct
Cyc_Absyn_Region_s_struct*)_tmp3BC)->f2;_tmp3E5=((struct Cyc_Absyn_Region_s_struct*)
_tmp3BC)->f3;_tmp3E6=((struct Cyc_Absyn_Region_s_struct*)_tmp3BC)->f4;_LL2D9: {
struct Cyc_List_List _tmp412=({struct Cyc_List_List _tmp413;_tmp413.hd=_tmp3E4;
_tmp413.tl=0;_tmp413;});inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)&
_tmp412,Cyc_CfFlowInfo_unknown_all);return Cyc_NewControlFlow_anal_stmt(env,
inflow,_tmp3E6);}_LL2DA: if(_tmp3BC <= (void*)1  || *((int*)_tmp3BC)!= 20)goto
_LL2DC;_tmp3E7=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp3BC)->f1;_LL2DB: {
void*_tmp415;void*_tmp416;struct _tuple0 _tmp414=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp3E7);_tmp415=_tmp414.f1;_tmp416=_tmp414.f2;{void*_tmp417=Cyc_NewControlFlow_use_Rval(
env,_tmp3E7->loc,_tmp415,_tmp416);void*_tmp418=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3E7->topt))->v);void*_tmp419;_LL2E4: if(
_tmp418 <= (void*)3  || *((int*)_tmp418)!= 15)goto _LL2E6;_tmp419=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp418)->f1;_LL2E5: return Cyc_CfFlowInfo_kill_flow_region(
_tmp415,_tmp419);_LL2E6:;_LL2E7:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A[0]=({struct Cyc_Core_Impossible_struct
_tmp41B;_tmp41B.tag=Cyc_Core_Impossible;_tmp41B.f1=({const char*_tmp41C="anal_stmt -- reset_region";
_tag_arr(_tmp41C,sizeof(char),_get_zero_arr_size(_tmp41C,26));});_tmp41B;});
_tmp41A;}));_LL2E3:;}}_LL2DC:;_LL2DD:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D[0]=({struct Cyc_Core_Impossible_struct
_tmp41E;_tmp41E.tag=Cyc_Core_Impossible;_tmp41E.f1=({const char*_tmp41F="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_arr(_tmp41F,sizeof(char),_get_zero_arr_size(_tmp41F,56));});_tmp41E;});
_tmp41D;}));_LL2B3:;}}static void*Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Decl*decl){void*_tmp420=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp421;struct Cyc_Core_Opt*_tmp422;struct Cyc_Core_Opt _tmp423;struct Cyc_List_List*
_tmp424;struct Cyc_Absyn_Exp*_tmp425;struct Cyc_List_List*_tmp426;_LL2E9: if(*((int*)
_tmp420)!= 0)goto _LL2EB;_tmp421=((struct Cyc_Absyn_Var_d_struct*)_tmp420)->f1;
_LL2EA: {struct Cyc_List_List _tmp427=({struct Cyc_List_List _tmp435;_tmp435.hd=
_tmp421;_tmp435.tl=0;_tmp435;});inflow=Cyc_NewControlFlow_add_vars(inflow,(
struct Cyc_List_List*)& _tmp427,Cyc_CfFlowInfo_unknown_none);{struct Cyc_Absyn_Exp*
_tmp428=_tmp421->initializer;if(_tmp428 == 0)return inflow;{void*_tmp42A;void*
_tmp42B;struct _tuple0 _tmp429=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
_tmp428);_tmp42A=_tmp429.f1;_tmp42B=_tmp429.f2;{void*_tmp42C=_tmp42A;struct Cyc_Dict_Dict*
_tmp42D;struct Cyc_List_List*_tmp42E;_LL2F2: if((int)_tmp42C != 0)goto _LL2F4;_LL2F3:
return(void*)0;_LL2F4: if(_tmp42C <= (void*)1  || *((int*)_tmp42C)!= 0)goto _LL2F1;
_tmp42D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp42C)->f1;_tmp42E=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp42C)->f2;_LL2F5: _tmp42D=Cyc_CfFlowInfo_assign_place(
decl->loc,_tmp42D,env->all_changed,({struct Cyc_CfFlowInfo_Place*_tmp42F=
_cycalloc(sizeof(*_tmp42F));_tmp42F->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp430=_cycalloc(sizeof(*_tmp430));_tmp430[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp431;_tmp431.tag=0;_tmp431.f1=_tmp421;_tmp431;});_tmp430;}));_tmp42F->fields=
0;_tmp42F;}),_tmp42B);_tmp42E=Cyc_CfFlowInfo_reln_assign_var(_tmp42E,_tmp421,(
struct Cyc_Absyn_Exp*)_check_null(_tmp421->initializer));{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp432=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp433=_cycalloc(sizeof(*
_tmp433));_tmp433[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp434;_tmp434.tag=
0;_tmp434.f1=_tmp42D;_tmp434.f2=_tmp42E;_tmp434;});_tmp433;});Cyc_NewControlFlow_update_tryflow(
env,(void*)_tmp432);return(void*)_tmp432;}_LL2F1:;}}}}_LL2EB: if(*((int*)_tmp420)
!= 2)goto _LL2ED;_tmp422=((struct Cyc_Absyn_Let_d_struct*)_tmp420)->f2;if(_tmp422
== 0)goto _LL2ED;_tmp423=*_tmp422;_tmp424=(struct Cyc_List_List*)_tmp423.v;_tmp425=((
struct Cyc_Absyn_Let_d_struct*)_tmp420)->f3;_LL2EC: {void*_tmp437;void*_tmp438;
struct _tuple0 _tmp436=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp425);_tmp437=
_tmp436.f1;_tmp438=_tmp436.f2;_tmp437=Cyc_NewControlFlow_use_Rval(env,_tmp425->loc,
_tmp437,_tmp438);return Cyc_NewControlFlow_add_vars(_tmp437,_tmp424,Cyc_CfFlowInfo_unknown_all);}
_LL2ED: if(*((int*)_tmp420)!= 3)goto _LL2EF;_tmp426=((struct Cyc_Absyn_Letv_d_struct*)
_tmp420)->f1;_LL2EE: return Cyc_NewControlFlow_add_vars(inflow,_tmp426,Cyc_CfFlowInfo_unknown_none);
_LL2EF:;_LL2F0:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp439=
_cycalloc(sizeof(*_tmp439));_tmp439[0]=({struct Cyc_Core_Impossible_struct _tmp43A;
_tmp43A.tag=Cyc_Core_Impossible;_tmp43A.f1=({const char*_tmp43B="anal_decl: unexpected decl variant";
_tag_arr(_tmp43B,sizeof(char),_get_zero_arr_size(_tmp43B,35));});_tmp43A;});
_tmp439;}));_LL2E8:;}static void Cyc_NewControlFlow_postpass(struct Cyc_Absyn_Stmt*
s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{void*_tmp43C=(void*)s->r;
struct _tuple3 _tmp43D;struct Cyc_Absyn_Stmt*_tmp43E;struct _tuple3 _tmp43F;struct Cyc_Absyn_Stmt*
_tmp440;struct Cyc_Absyn_Stmt*_tmp441;struct Cyc_Absyn_Stmt*_tmp442;struct Cyc_Absyn_Stmt*
_tmp443;struct _tuple3 _tmp444;struct Cyc_Absyn_Stmt*_tmp445;struct Cyc_Absyn_Stmt*
_tmp446;struct Cyc_Absyn_Stmt*_tmp447;struct _tuple3 _tmp448;struct Cyc_Absyn_Stmt*
_tmp449;struct Cyc_Absyn_Stmt*_tmp44A;struct Cyc_Absyn_Stmt*_tmp44B;struct Cyc_Absyn_Stmt*
_tmp44C;struct Cyc_Absyn_Stmt*_tmp44D;struct Cyc_Absyn_Stmt*_tmp44E;struct Cyc_Absyn_Stmt*
_tmp44F;struct Cyc_List_List*_tmp450;struct Cyc_List_List*_tmp451;_LL2F7: if(
_tmp43C <= (void*)1  || *((int*)_tmp43C)!= 8)goto _LL2F9;_tmp43D=((struct Cyc_Absyn_For_s_struct*)
_tmp43C)->f2;_tmp43E=_tmp43D.f2;_tmp43F=((struct Cyc_Absyn_For_s_struct*)_tmp43C)->f3;
_tmp440=_tmp43F.f2;_tmp441=((struct Cyc_Absyn_For_s_struct*)_tmp43C)->f4;_LL2F8:
Cyc_NewControlFlow_postpass(_tmp440);_tmp442=_tmp441;_tmp443=_tmp43E;goto _LL2FA;
_LL2F9: if(_tmp43C <= (void*)1  || *((int*)_tmp43C)!= 1)goto _LL2FB;_tmp442=((struct
Cyc_Absyn_Seq_s_struct*)_tmp43C)->f1;_tmp443=((struct Cyc_Absyn_Seq_s_struct*)
_tmp43C)->f2;_LL2FA: _tmp445=_tmp442;_tmp446=_tmp443;goto _LL2FC;_LL2FB: if(_tmp43C
<= (void*)1  || *((int*)_tmp43C)!= 4)goto _LL2FD;_tmp444=((struct Cyc_Absyn_While_s_struct*)
_tmp43C)->f1;_tmp445=_tmp444.f2;_tmp446=((struct Cyc_Absyn_While_s_struct*)
_tmp43C)->f2;_LL2FC: _tmp447=_tmp446;_tmp449=_tmp445;goto _LL2FE;_LL2FD: if(_tmp43C
<= (void*)1  || *((int*)_tmp43C)!= 16)goto _LL2FF;_tmp447=((struct Cyc_Absyn_Do_s_struct*)
_tmp43C)->f1;_tmp448=((struct Cyc_Absyn_Do_s_struct*)_tmp43C)->f2;_tmp449=_tmp448.f2;
_LL2FE: _tmp44A=_tmp447;_tmp44B=_tmp449;goto _LL300;_LL2FF: if(_tmp43C <= (void*)1
 || *((int*)_tmp43C)!= 3)goto _LL301;_tmp44A=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp43C)->f2;_tmp44B=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp43C)->f3;_LL300:
Cyc_NewControlFlow_postpass(_tmp44A);_tmp44C=_tmp44B;goto _LL302;_LL301: if(
_tmp43C <= (void*)1  || *((int*)_tmp43C)!= 18)goto _LL303;_tmp44C=((struct Cyc_Absyn_Region_s_struct*)
_tmp43C)->f4;_LL302: _tmp44D=_tmp44C;goto _LL304;_LL303: if(_tmp43C <= (void*)1  || *((
int*)_tmp43C)!= 15)goto _LL305;_tmp44D=((struct Cyc_Absyn_Label_s_struct*)_tmp43C)->f2;
_LL304: _tmp44E=_tmp44D;goto _LL306;_LL305: if(_tmp43C <= (void*)1  || *((int*)
_tmp43C)!= 12)goto _LL307;_tmp44E=((struct Cyc_Absyn_Decl_s_struct*)_tmp43C)->f2;
_LL306: Cyc_NewControlFlow_postpass(_tmp44E);return;_LL307: if(_tmp43C <= (void*)1
 || *((int*)_tmp43C)!= 17)goto _LL309;_tmp44F=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp43C)->f1;_tmp450=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp43C)->f2;_LL308:
Cyc_NewControlFlow_postpass(_tmp44F);_tmp451=_tmp450;goto _LL30A;_LL309: if(
_tmp43C <= (void*)1  || *((int*)_tmp43C)!= 9)goto _LL30B;_tmp451=((struct Cyc_Absyn_Switch_s_struct*)
_tmp43C)->f2;_LL30A: for(0;_tmp451 != 0;_tmp451=_tmp451->tl){Cyc_NewControlFlow_postpass(((
struct Cyc_Absyn_Switch_clause*)_tmp451->hd)->body);}return;_LL30B:;_LL30C:
return;_LL2F6:;}}static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct Cyc_Position_Segment*_tmp452=(fd->body)->loc;void*inflow=Cyc_NewControlFlow_add_vars((
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp487=_cycalloc(sizeof(*
_tmp487));_tmp487[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp488;_tmp488.tag=
0;_tmp488.f1=Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp488.f2=0;_tmp488;});
_tmp487;}),(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,
Cyc_CfFlowInfo_unknown_all);struct Cyc_List_List*_tmp453=0;{void*_tmp454=inflow;
struct Cyc_Dict_Dict*_tmp455;struct Cyc_List_List*_tmp456;_LL30E: if((int)_tmp454 != 
0)goto _LL310;_LL30F:({void*_tmp457[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp458="check_fun";_tag_arr(
_tmp458,sizeof(char),_get_zero_arr_size(_tmp458,10));}),_tag_arr(_tmp457,sizeof(
void*),0));});_LL310: if(_tmp454 <= (void*)1  || *((int*)_tmp454)!= 0)goto _LL30D;
_tmp455=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp454)->f1;_tmp456=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp454)->f2;_LL311: {struct Cyc_List_List*
atts;{void*_tmp459=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp45A;struct Cyc_List_List*_tmp45B;
_LL313: if(_tmp459 <= (void*)3  || *((int*)_tmp459)!= 8)goto _LL315;_tmp45A=((struct
Cyc_Absyn_FnType_struct*)_tmp459)->f1;_tmp45B=_tmp45A.attributes;_LL314: atts=
_tmp45B;goto _LL312;_LL315:;_LL316:({void*_tmp45C[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp45D="check_fun: non-function type cached with fndecl_t";
_tag_arr(_tmp45D,sizeof(char),_get_zero_arr_size(_tmp45D,50));}),_tag_arr(
_tmp45C,sizeof(void*),0));});_LL312:;}for(0;atts != 0;atts=atts->tl){void*_tmp45E=(
void*)atts->hd;int _tmp45F;_LL318: if(_tmp45E <= (void*)17  || *((int*)_tmp45E)!= 4)
goto _LL31A;_tmp45F=((struct Cyc_Absyn_Initializes_att_struct*)_tmp45E)->f1;_LL319: {
unsigned int j=(unsigned int)_tmp45F;if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp460[0]={};Cyc_Tcutil_terr(_tmp452,({const char*_tmp461="initializes attribute exceeds number of parameters";
_tag_arr(_tmp461,sizeof(char),_get_zero_arr_size(_tmp461,51));}),_tag_arr(
_tmp460,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*_tmp462=((struct
Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*
_tmp463=Cyc_Tcutil_compress((void*)_tmp462->type);struct Cyc_Absyn_PtrInfo _tmp464;
void*_tmp465;struct Cyc_Absyn_PtrAtts _tmp466;struct Cyc_Absyn_Conref*_tmp467;
struct Cyc_Absyn_Conref*_tmp468;struct Cyc_Absyn_Conref*_tmp469;_LL31D: if(_tmp463
<= (void*)3  || *((int*)_tmp463)!= 4)goto _LL31F;_tmp464=((struct Cyc_Absyn_PointerType_struct*)
_tmp463)->f1;_tmp465=(void*)_tmp464.elt_typ;_tmp466=_tmp464.ptr_atts;_tmp467=
_tmp466.nullable;_tmp468=_tmp466.bounds;_tmp469=_tmp466.zero_term;_LL31E: if(((
int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp467))({void*_tmp46A[0]={};
Cyc_Tcutil_terr(_tmp452,({const char*_tmp46B="initializes attribute not allowed on nullable pointers";
_tag_arr(_tmp46B,sizeof(char),_get_zero_arr_size(_tmp46B,55));}),_tag_arr(
_tmp46A,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp468))({void*_tmp46C[
0]={};Cyc_Tcutil_terr(_tmp452,({const char*_tmp46D="initializes attribute allowed only on pointers of size 1";
_tag_arr(_tmp46D,sizeof(char),_get_zero_arr_size(_tmp46D,57));}),_tag_arr(
_tmp46C,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp469))({void*_tmp46E[0]={};Cyc_Tcutil_terr(_tmp452,({const char*_tmp46F="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_arr(_tmp46F,sizeof(char),_get_zero_arr_size(_tmp46F,77));}),_tag_arr(
_tmp46E,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*_tmp470=({
struct Cyc_CfFlowInfo_InitParam_struct*_tmp478=_cycalloc(sizeof(*_tmp478));
_tmp478[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp479;_tmp479.tag=2;_tmp479.f1=(
int)j;_tmp479.f2=(void*)_tmp465;_tmp479;});_tmp478;});struct Cyc_CfFlowInfo_Place*
_tmp471=({struct Cyc_CfFlowInfo_Place*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->root=(
void*)((void*)_tmp470);_tmp477->fields=0;_tmp477;});_tmp455=Cyc_Dict_insert(
_tmp455,(void*)_tmp470,Cyc_CfFlowInfo_typ_to_absrval(_tmp465,Cyc_CfFlowInfo_esc_none));
_tmp455=Cyc_Dict_insert(_tmp455,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp473;_tmp473.tag=0;_tmp473.f1=_tmp462;_tmp473;});_tmp472;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp474=_cycalloc(sizeof(*_tmp474));_tmp474[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp475;_tmp475.tag=2;_tmp475.f1=_tmp471;
_tmp475;});_tmp474;}));_tmp453=({struct Cyc_List_List*_tmp476=_cycalloc(sizeof(*
_tmp476));_tmp476->hd=_tmp471;_tmp476->tl=_tmp453;_tmp476;});goto _LL31C;}_LL31F:;
_LL320:({void*_tmp47A[0]={};Cyc_Tcutil_terr(_tmp452,({const char*_tmp47B="initializes attribute on non-pointer";
_tag_arr(_tmp47B,sizeof(char),_get_zero_arr_size(_tmp47B,37));}),_tag_arr(
_tmp47A,sizeof(void*),0));});_LL31C:;}goto _LL317;}}_LL31A:;_LL31B: goto _LL317;
_LL317:;}inflow=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp47C=
_cycalloc(sizeof(*_tmp47C));_tmp47C[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp47D;_tmp47D.tag=0;_tmp47D.f1=_tmp455;_tmp47D.f2=_tmp456;_tmp47D;});_tmp47C;});}
_LL30D:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));struct
Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp486=
_cycalloc(sizeof(*_tmp486));_tmp486->iterate_again=1;_tmp486->iteration_num=0;
_tmp486->in_try=0;_tmp486->tryflow=(void*)inflow;_tmp486->all_changed=0;_tmp486->noreturn=
noreturn;_tmp486->param_roots=_tmp453;_tmp486;});void*outflow=inflow;while(env->iterate_again){
++ env->iteration_num;env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,fd->body);}{void*_tmp47E=outflow;_LL322: if((int)_tmp47E != 0)goto
_LL324;_LL323: goto _LL321;_LL324:;_LL325: Cyc_NewControlFlow_check_init_params(
_tmp452,env,outflow);if(noreturn)({void*_tmp47F[0]={};Cyc_Tcutil_terr(_tmp452,({
const char*_tmp480="`noreturn' function might (implicitly) return";_tag_arr(
_tmp480,sizeof(char),_get_zero_arr_size(_tmp480,46));}),_tag_arr(_tmp47F,sizeof(
void*),0));});else{void*_tmp481=Cyc_Tcutil_compress((void*)fd->ret_type);_LL327:
if((int)_tmp481 != 0)goto _LL329;_LL328: goto _LL326;_LL329: if(_tmp481 <= (void*)3
 || *((int*)_tmp481)!= 6)goto _LL32B;_LL32A: goto _LL32C;_LL32B: if((int)_tmp481 != 1)
goto _LL32D;_LL32C: goto _LL32E;_LL32D: if(_tmp481 <= (void*)3  || *((int*)_tmp481)!= 
5)goto _LL32F;_LL32E:({void*_tmp482[0]={};Cyc_Tcutil_warn(_tmp452,({const char*
_tmp483="function may not return a value";_tag_arr(_tmp483,sizeof(char),
_get_zero_arr_size(_tmp483,32));}),_tag_arr(_tmp482,sizeof(void*),0));});goto
_LL326;_LL32F:;_LL330:({void*_tmp484[0]={};Cyc_Tcutil_terr(_tmp452,({const char*
_tmp485="function may not return a value";_tag_arr(_tmp485,sizeof(char),
_get_zero_arr_size(_tmp485,32));}),_tag_arr(_tmp484,sizeof(void*),0));});goto
_LL326;_LL326:;}goto _LL321;_LL321:;}Cyc_NewControlFlow_postpass(fd->body);}}void
Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*
_tmp489=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp48A;
struct Cyc_List_List*_tmp48B;struct Cyc_List_List*_tmp48C;struct Cyc_List_List*
_tmp48D;_LL332: if(*((int*)_tmp489)!= 1)goto _LL334;_tmp48A=((struct Cyc_Absyn_Fn_d_struct*)
_tmp489)->f1;_LL333: Cyc_NewControlFlow_check_fun(_tmp48A);goto _LL331;_LL334: if(*((
int*)_tmp489)!= 10)goto _LL336;_tmp48B=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp489)->f1;_LL335: _tmp48C=_tmp48B;goto _LL337;_LL336: if(*((int*)_tmp489)!= 9)
goto _LL338;_tmp48C=((struct Cyc_Absyn_Using_d_struct*)_tmp489)->f2;_LL337: _tmp48D=
_tmp48C;goto _LL339;_LL338: if(*((int*)_tmp489)!= 8)goto _LL33A;_tmp48D=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp489)->f2;_LL339: Cyc_NewControlFlow_cf_check(_tmp48D);goto _LL331;_LL33A:;
_LL33B: goto _LL331;_LL331:;}}
