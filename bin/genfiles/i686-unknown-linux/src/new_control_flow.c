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
_tuple1*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*
tunion_name;struct _tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
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
struct Cyc_List_List*f2;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*
cnst_exp;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
void*sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct
_tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*
exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple1*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _tagged_arr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{
void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);struct _tagged_arr*Cyc_Absyn_fieldname(
int);int Cyc_Absyn_is_union_type(void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
_LL6: if((int)_tmpC != 0)goto _LL8;_LL7: return(void*)0;_LL8: if(_tmpC <= (void*)1?1:*((
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
void*)0;_LLD: if(_tmp14 <= (void*)1?1:*((int*)_tmp14)!= 0)goto _LLA;_tmp15=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp14)->f1;_tmp16=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
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
struct Cyc_List_List*_tmp34;_LL10: if(_tmp32 <= (void*)1?1:*((int*)_tmp32)!= 0)goto
_LL12;_tmp33=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp32)->f1;_tmp34=((
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
return outflow;_LL17: if(_tmp38 <= (void*)1?1:*((int*)_tmp38)!= 0)goto _LL14;_tmp39=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp38)->f1;_tmp3A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp38)->f2;_LL18: {void*_tmp3B=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;
struct Cyc_CfFlowInfo_Place*_tmp3C;_LL1A: if((int)_tmp3B != 0)goto _LL1C;_LL1B:
return outflow;_LL1C: if(_tmp3B <= (void*)1?1:*((int*)_tmp3B)!= 0)goto _LL19;_tmp3C=((
struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp3B)->f1;_LL1D: {void*nzval=il == (void*)2?(
void*)1:(void*)2;return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3D=
_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp3E;_tmp3E.tag=0;_tmp3E.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp39,env->all_changed,
_tmp3C,nzval);_tmp3E.f2=_tmp3A;_tmp3E;});_tmp3D;});}_LL19:;}_LL14:;}static struct
_tuple0 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,void*outflow,struct Cyc_Absyn_Exp*e,void*il){void*_tmp3F=outflow;struct Cyc_Dict_Dict*
_tmp40;struct Cyc_List_List*_tmp41;_LL1F: if((int)_tmp3F != 0)goto _LL21;_LL20:
return({struct _tuple0 _tmp42;_tmp42.f1=outflow;_tmp42.f2=outflow;_tmp42;});_LL21:
if(_tmp3F <= (void*)1?1:*((int*)_tmp3F)!= 0)goto _LL1E;_tmp40=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp3F)->f1;_tmp41=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3F)->f2;_LL22: {
void*_tmp43=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*
_tmp44;_LL24: if((int)_tmp43 != 0)goto _LL26;_LL25: return({struct _tuple0 _tmp45;
_tmp45.f1=outflow;_tmp45.f2=outflow;_tmp45;});_LL26: if(_tmp43 <= (void*)1?1:*((
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
_LL2B;_LL2A: return flow;_LL2B: if(_tmp4B <= (void*)1?1:*((int*)_tmp4B)!= 0)goto
_LL28;_tmp4C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4B)->f1;_tmp4D=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4B)->f2;_LL2C: {void*_tmp4E=r2;
struct Cyc_List_List*_tmp4F;_LL2E: if(_tmp4E <= (void*)3?1:*((int*)_tmp4E)!= 3)goto
_LL30;_tmp4F=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp4E)->f1;_LL2F: {void*
_tmp50=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*
_tmp51;_LL33: if((int)_tmp50 != 0)goto _LL35;_LL34: return flow;_LL35: if(_tmp50 <= (
void*)1?1:*((int*)_tmp50)!= 0)goto _LL32;_tmp51=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp50)->f1;_LL36: {struct Cyc_List_List*new_cl;{void*_tmp52=r1;struct Cyc_List_List*
_tmp53;void*_tmp54;_LL38: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 3)goto _LL3A;
_tmp53=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp52)->f1;_LL39: new_cl=_tmp53;
goto _LL37;_LL3A: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 0)goto _LL3C;_tmp54=(
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
_tmp61 <= (void*)3?1:*((int*)_tmp61)!= 4)goto _LL4E;_tmp62=((struct Cyc_Absyn_PointerType_struct*)
_tmp61)->f1;_tmp63=(void*)_tmp62.elt_typ;_tmp64=_tmp62.ptr_atts;_tmp65=_tmp64.bounds;
_tmp66=_tmp64.zero_term;_LL4D: {void*_tmp67=f;struct Cyc_Dict_Dict*_tmp68;struct
Cyc_List_List*_tmp69;_LL51: if((int)_tmp67 != 0)goto _LL53;_LL52: return({struct
_tuple0 _tmp6A;_tmp6A.f1=f;_tmp6A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp63,Cyc_CfFlowInfo_unknown_all);
_tmp6A;});_LL53: if(_tmp67 <= (void*)1?1:*((int*)_tmp67)!= 0)goto _LL50;_tmp68=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp67)->f1;_tmp69=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp67)->f2;_LL54: if(Cyc_Tcutil_is_bound_one(_tmp65)){void*_tmp6B=r;struct Cyc_CfFlowInfo_Place*
_tmp6C;void*_tmp6D;_LL56: if((int)_tmp6B != 1)goto _LL58;_LL57: goto _LL59;_LL58: if((
int)_tmp6B != 2)goto _LL5A;_LL59:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp6F;_tmp6F.tag=Cyc_CfFlowInfo_NotZero;_tmp6F.f1=_tmp69;_tmp6F;});_tmp6E;})));
goto _LL55;_LL5A: if(_tmp6B <= (void*)3?1:*((int*)_tmp6B)!= 2)goto _LL5C;_tmp6C=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp6B)->f1;_LL5B:(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp70=_cycalloc(sizeof(*_tmp70));
_tmp70[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp71;_tmp71.tag=Cyc_CfFlowInfo_NotZero;
_tmp71.f1=_tmp69;_tmp71;});_tmp70;})));return({struct _tuple0 _tmp72;_tmp72.f1=f;
_tmp72.f2=Cyc_CfFlowInfo_lookup_place(_tmp68,_tmp6C);_tmp72;});_LL5C: if((int)
_tmp6B != 0)goto _LL5E;_LL5D:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple0 _tmp73;_tmp73.f1=(void*)0;_tmp73.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp63,Cyc_CfFlowInfo_unknown_all);_tmp73;});_LL5E: if(_tmp6B <= (void*)3?1:*((int*)
_tmp6B)!= 0)goto _LL60;_tmp6D=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
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
_tmp80)->f2;if(_tmp81 <= (void*)1?1:*((int*)_tmp81)!= 4)goto _LL6C;_tmp82=((struct
Cyc_Absyn_Pat_b_struct*)_tmp81)->f1;_LL6B: _tmp84=_tmp82;goto _LL6D;_LL6C: if(*((
int*)_tmp80)!= 1)goto _LL6E;_tmp83=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp80)->f2;
if(_tmp83 <= (void*)1?1:*((int*)_tmp83)!= 3)goto _LL6E;_tmp84=((struct Cyc_Absyn_Local_b_struct*)
_tmp83)->f1;_LL6D: _tmp86=_tmp84;goto _LL6F;_LL6E: if(*((int*)_tmp80)!= 1)goto _LL70;
_tmp85=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp80)->f2;if(_tmp85 <= (void*)1?1:*((
int*)_tmp85)!= 2)goto _LL70;_tmp86=((struct Cyc_Absyn_Param_b_struct*)_tmp85)->f1;
_LL6F: _tmp88=_tmp86;goto _LL71;_LL70: if(*((int*)_tmp80)!= 1)goto _LL72;_tmp87=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp80)->f2;if(_tmp87 <= (void*)1?1:*((int*)
_tmp87)!= 0)goto _LL72;_tmp88=((struct Cyc_Absyn_Global_b_struct*)_tmp87)->f1;
_LL71: if(!_tmp88->escapes){void*_tmp89=(void*)e2->r;void*_tmp8A;struct Cyc_Absyn_Vardecl*
_tmp8B;void*_tmp8C;struct Cyc_Absyn_Vardecl*_tmp8D;void*_tmp8E;struct Cyc_Absyn_Vardecl*
_tmp8F;void*_tmp90;struct Cyc_Absyn_Vardecl*_tmp91;_LL75: if(*((int*)_tmp89)!= 1)
goto _LL77;_tmp8A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp8A <= (
void*)1?1:*((int*)_tmp8A)!= 4)goto _LL77;_tmp8B=((struct Cyc_Absyn_Pat_b_struct*)
_tmp8A)->f1;_LL76: _tmp8D=_tmp8B;goto _LL78;_LL77: if(*((int*)_tmp89)!= 1)goto _LL79;
_tmp8C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp8C <= (void*)1?1:*((
int*)_tmp8C)!= 3)goto _LL79;_tmp8D=((struct Cyc_Absyn_Local_b_struct*)_tmp8C)->f1;
_LL78: _tmp8F=_tmp8D;goto _LL7A;_LL79: if(*((int*)_tmp89)!= 1)goto _LL7B;_tmp8E=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp8E <= (void*)1?1:*((int*)
_tmp8E)!= 2)goto _LL7B;_tmp8F=((struct Cyc_Absyn_Param_b_struct*)_tmp8E)->f1;_LL7A:
_tmp91=_tmp8F;goto _LL7C;_LL7B: if(*((int*)_tmp89)!= 1)goto _LL7D;_tmp90=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp90 <= (void*)1?1:*((int*)_tmp90)
!= 0)goto _LL7D;_tmp91=((struct Cyc_Absyn_Global_b_struct*)_tmp90)->f1;_LL7C: if(!
_tmp91->escapes){int found=0;{struct Cyc_List_List*_tmp92=relns;for(0;_tmp92 != 0;
_tmp92=_tmp92->tl){struct Cyc_CfFlowInfo_Reln*_tmp93=(struct Cyc_CfFlowInfo_Reln*)
_tmp92->hd;if(_tmp93->vd == _tmp91){void*_tmp94=(void*)_tmp93->rop;struct Cyc_Absyn_Vardecl*
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
1?1:*((int*)_tmp9A)!= 0)goto _LL84;_tmp9B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
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
_tmpD0;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_List_List*_tmpD2;struct Cyc_List_List*
_tmpD3;struct Cyc_List_List*_tmpD4;struct Cyc_List_List*_tmpD5;struct Cyc_List_List*
_tmpD6;struct Cyc_Absyn_Vardecl*_tmpD7;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*
_tmpD9;int _tmpDA;struct Cyc_Absyn_Stmt*_tmpDB;void*_tmpDC;_LL8A: if(*((int*)_tmp9E)
!= 13)goto _LL8C;_tmp9F=((struct Cyc_Absyn_Cast_e_struct*)_tmp9E)->f2;_tmpA0=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp9E)->f4;if((int)_tmpA0 != 2)goto _LL8C;_LL8B: {
void*_tmpDE;void*_tmpDF;struct _tuple0 _tmpDD=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp9F);_tmpDE=_tmpDD.f1;_tmpDF=_tmpDD.f2;{void*_tmpE1;void*_tmpE2;struct
_tuple0 _tmpE0=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmpDE,_tmp9F,_tmpDF);
_tmpE1=_tmpE0.f1;_tmpE2=_tmpE0.f2;return({struct _tuple0 _tmpE3;_tmpE3.f1=_tmpE1;
_tmpE3.f2=_tmpDF;_tmpE3;});}}_LL8C: if(*((int*)_tmp9E)!= 13)goto _LL8E;_tmpA1=((
struct Cyc_Absyn_Cast_e_struct*)_tmp9E)->f2;_LL8D: _tmpA2=_tmpA1;goto _LL8F;_LL8E:
if(*((int*)_tmp9E)!= 11)goto _LL90;_tmpA2=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp9E)->f1;_LL8F: _tmpA3=_tmpA2;goto _LL91;_LL90: if(*((int*)_tmp9E)!= 12)goto
_LL92;_tmpA3=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp9E)->f1;_LL91: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpA3);_LL92: if(*((int*)_tmp9E)!= 0)goto _LL94;_tmpA4=(void*)((struct
Cyc_Absyn_Const_e_struct*)_tmp9E)->f1;if((int)_tmpA4 != 0)goto _LL94;_LL93: goto
_LL95;_LL94: if(*((int*)_tmp9E)!= 0)goto _LL96;_tmpA5=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp9E)->f1;if(_tmpA5 <= (void*)1?1:*((int*)_tmpA5)!= 2)goto _LL96;_tmpA6=((struct
Cyc_Absyn_Int_c_struct*)_tmpA5)->f2;if(_tmpA6 != 0)goto _LL96;_LL95: return({struct
_tuple0 _tmpE4;_tmpE4.f1=inflow;_tmpE4.f2=(void*)0;_tmpE4;});_LL96: if(*((int*)
_tmp9E)!= 0)goto _LL98;_tmpA7=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp9E)->f1;
if(_tmpA7 <= (void*)1?1:*((int*)_tmpA7)!= 2)goto _LL98;_LL97: goto _LL99;_LL98: if(*((
int*)_tmp9E)!= 1)goto _LL9A;_tmpA8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9E)->f2;
if(_tmpA8 <= (void*)1?1:*((int*)_tmpA8)!= 1)goto _LL9A;_LL99: return({struct _tuple0
_tmpE5;_tmpE5.f1=inflow;_tmpE5.f2=(void*)1;_tmpE5;});_LL9A: if(*((int*)_tmp9E)!= 
30)goto _LL9C;_tmpA9=((struct Cyc_Absyn_Tunion_e_struct*)_tmp9E)->f1;if(_tmpA9 != 0)
goto _LL9C;_LL9B: goto _LL9D;_LL9C: if(*((int*)_tmp9E)!= 0)goto _LL9E;_LL9D: goto _LL9F;
_LL9E: if(*((int*)_tmp9E)!= 17)goto _LLA0;_LL9F: goto _LLA1;_LLA0: if(*((int*)_tmp9E)
!= 16)goto _LLA2;_LLA1: goto _LLA3;_LLA2: if(*((int*)_tmp9E)!= 18)goto _LLA4;_LLA3:
goto _LLA5;_LLA4: if(*((int*)_tmp9E)!= 19)goto _LLA6;_LLA5: goto _LLA7;_LLA6: if(*((
int*)_tmp9E)!= 32)goto _LLA8;_LLA7: goto _LLA9;_LLA8: if(*((int*)_tmp9E)!= 31)goto
_LLAA;_LLA9: return({struct _tuple0 _tmpE6;_tmpE6.f1=inflow;_tmpE6.f2=Cyc_CfFlowInfo_unknown_all;
_tmpE6;});_LLAA: if(*((int*)_tmp9E)!= 1)goto _LLAC;_tmpAA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp9E)->f2;if(_tmpAA <= (void*)1?1:*((int*)_tmpAA)!= 0)goto _LLAC;_LLAB: return({
struct _tuple0 _tmpE7;_tmpE7.f1=inflow;_tmpE7.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmpE7;});_LLAC: if(*((int*)_tmp9E)!= 1)goto _LLAE;_tmpAB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp9E)->f2;if(_tmpAB <= (void*)1?1:*((int*)_tmpAB)!= 2)goto _LLAE;_tmpAC=((struct
Cyc_Absyn_Param_b_struct*)_tmpAB)->f1;_LLAD: _tmpAE=_tmpAC;goto _LLAF;_LLAE: if(*((
int*)_tmp9E)!= 1)goto _LLB0;_tmpAD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9E)->f2;
if(_tmpAD <= (void*)1?1:*((int*)_tmpAD)!= 3)goto _LLB0;_tmpAE=((struct Cyc_Absyn_Local_b_struct*)
_tmpAD)->f1;_LLAF: _tmpB0=_tmpAE;goto _LLB1;_LLB0: if(*((int*)_tmp9E)!= 1)goto _LLB2;
_tmpAF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9E)->f2;if(_tmpAF <= (void*)1?1:*((
int*)_tmpAF)!= 4)goto _LLB2;_tmpB0=((struct Cyc_Absyn_Pat_b_struct*)_tmpAF)->f1;
_LLB1: return({struct _tuple0 _tmpE8;_tmpE8.f1=inflow;_tmpE8.f2=Cyc_Dict_lookup(d,(
void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmpE9=_cycalloc(sizeof(*_tmpE9));
_tmpE9[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmpEA;_tmpEA.tag=0;_tmpEA.f1=
_tmpB0;_tmpEA;});_tmpE9;}));_tmpE8;});_LLB2: if(*((int*)_tmp9E)!= 3)goto _LLB4;
_tmpB1=((struct Cyc_Absyn_Primop_e_struct*)_tmp9E)->f2;_LLB3: return Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmpB1);_LLB4: if(*((int*)_tmp9E)!= 5)goto _LLB6;_tmpB2=((struct Cyc_Absyn_Increment_e_struct*)
_tmp9E)->f1;_LLB5: {void*_tmpED;struct _tuple0 _tmpEC=Cyc_NewControlFlow_anal_use_ints(
env,inflow,({struct Cyc_Absyn_Exp*_tmpEB[1];_tmpEB[0]=_tmpB2;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpEB,sizeof(struct Cyc_Absyn_Exp*),1));}));
_tmpED=_tmpEC.f1;{void*_tmpEF;struct _tuple0 _tmpEE=Cyc_NewControlFlow_anal_Lexp(
env,_tmpED,_tmpB2);_tmpEF=_tmpEE.f2;{struct _tuple0 _tmpF1=({struct _tuple0 _tmpF0;
_tmpF0.f1=_tmpEF;_tmpF0.f2=_tmpED;_tmpF0;});void*_tmpF2;struct Cyc_CfFlowInfo_Place*
_tmpF3;void*_tmpF4;struct Cyc_Dict_Dict*_tmpF5;struct Cyc_List_List*_tmpF6;_LLED:
_tmpF2=_tmpF1.f1;if(_tmpF2 <= (void*)1?1:*((int*)_tmpF2)!= 0)goto _LLEF;_tmpF3=((
struct Cyc_CfFlowInfo_PlaceL_struct*)_tmpF2)->f1;_tmpF4=_tmpF1.f2;if(_tmpF4 <= (
void*)1?1:*((int*)_tmpF4)!= 0)goto _LLEF;_tmpF5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpF4)->f1;_tmpF6=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpF4)->f2;_LLEE:
_tmpF6=Cyc_CfFlowInfo_reln_kill_exp(_tmpF6,_tmpB2);_tmpED=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmpF8;_tmpF8.tag=0;_tmpF8.f1=Cyc_CfFlowInfo_assign_place(_tmpB2->loc,_tmpF5,env->all_changed,
_tmpF3,Cyc_CfFlowInfo_unknown_all);_tmpF8.f2=_tmpF6;_tmpF8;});_tmpF7;});goto
_LLEC;_LLEF:;_LLF0: goto _LLEC;_LLEC:;}return({struct _tuple0 _tmpF9;_tmpF9.f1=
_tmpED;_tmpF9.f2=Cyc_CfFlowInfo_unknown_all;_tmpF9;});}}_LLB6: if(*((int*)_tmp9E)
!= 4)goto _LLB8;_tmpB3=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f1;_tmpB4=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f2;if(_tmpB4 == 0)goto _LLB8;_tmpB5=*
_tmpB4;_tmpB6=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f3;_LLB7: {void*
_tmpFC;struct _tuple0 _tmpFB=Cyc_NewControlFlow_anal_use_ints(env,inflow,({struct
Cyc_Absyn_Exp*_tmpFA[2];_tmpFA[1]=_tmpB6;_tmpFA[0]=_tmpB3;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpFA,sizeof(struct Cyc_Absyn_Exp*),2));}));
_tmpFC=_tmpFB.f1;{void*_tmpFE;struct _tuple0 _tmpFD=Cyc_NewControlFlow_anal_Lexp(
env,_tmpFC,e);_tmpFE=_tmpFD.f2;{struct _tuple0 _tmp100=({struct _tuple0 _tmpFF;
_tmpFF.f1=_tmpFE;_tmpFF.f2=_tmpFC;_tmpFF;});void*_tmp101;struct Cyc_CfFlowInfo_Place*
_tmp102;void*_tmp103;struct Cyc_Dict_Dict*_tmp104;struct Cyc_List_List*_tmp105;
_LLF2: _tmp101=_tmp100.f1;if(_tmp101 <= (void*)1?1:*((int*)_tmp101)!= 0)goto _LLF4;
_tmp102=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp101)->f1;_tmp103=_tmp100.f2;
if(_tmp103 <= (void*)1?1:*((int*)_tmp103)!= 0)goto _LLF4;_tmp104=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp103)->f1;_tmp105=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp103)->f2;
_LLF3: _tmp105=Cyc_CfFlowInfo_reln_kill_exp(_tmp105,_tmpB3);_tmpFC=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp106=_cycalloc(sizeof(*_tmp106));
_tmp106[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp107;_tmp107.tag=0;
_tmp107.f1=Cyc_CfFlowInfo_assign_place(_tmpB3->loc,_tmp104,env->all_changed,
_tmp102,Cyc_CfFlowInfo_unknown_all);_tmp107.f2=_tmp105;_tmp107;});_tmp106;});
goto _LLF1;_LLF4:;_LLF5: goto _LLF1;_LLF1:;}return({struct _tuple0 _tmp108;_tmp108.f1=
_tmpFC;_tmp108.f2=Cyc_CfFlowInfo_unknown_all;_tmp108;});}}_LLB8: if(*((int*)
_tmp9E)!= 4)goto _LLBA;_tmpB7=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f1;
_tmpB8=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f2;if(_tmpB8 != 0)goto _LLBA;
_tmpB9=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f3;_LLB9: {struct Cyc_Set_Set**
_tmp109=env->all_changed;while(1){env->all_changed=({struct Cyc_Set_Set**_tmp10A=
_cycalloc(sizeof(*_tmp10A));_tmp10A[0]=Cyc_CfFlowInfo_mt_place_set();_tmp10A;});{
void*_tmp10C;void*_tmp10D;struct _tuple0 _tmp10B=Cyc_NewControlFlow_anal_Lexp(env,
inflow,_tmpB7);_tmp10C=_tmp10B.f1;_tmp10D=_tmp10B.f2;{struct Cyc_Set_Set*_tmp10E=*((
struct Cyc_Set_Set**)_check_null(env->all_changed));env->all_changed=({struct Cyc_Set_Set**
_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp10F;});{void*_tmp111;void*_tmp112;struct _tuple0 _tmp110=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpB9);_tmp111=_tmp110.f1;_tmp112=_tmp110.f2;{struct Cyc_Set_Set*
_tmp113=*((struct Cyc_Set_Set**)_check_null(env->all_changed));void*_tmp114=Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& _tmp10E,_tmp10C,_tmp111,_tmp10E,_tmp113);void*_tmp115=Cyc_CfFlowInfo_join_flow(
_tmp109,inflow,_tmp114);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp115,inflow)){
if(_tmp109 == 0)env->all_changed=0;else{*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*
_tmp109,((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp10E,_tmp113));}{void*_tmp116=_tmp114;struct Cyc_Dict_Dict*_tmp117;struct Cyc_List_List*
_tmp118;_LLF7: if((int)_tmp116 != 0)goto _LLF9;_LLF8: return({struct _tuple0 _tmp119;
_tmp119.f1=(void*)0;_tmp119.f2=_tmp112;_tmp119;});_LLF9: if(_tmp116 <= (void*)1?1:*((
int*)_tmp116)!= 0)goto _LLF6;_tmp117=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp116)->f1;_tmp118=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp116)->f2;
_LLFA: {void*_tmp11A=_tmp10D;struct Cyc_CfFlowInfo_Place*_tmp11B;_LLFC: if(_tmp11A
<= (void*)1?1:*((int*)_tmp11A)!= 0)goto _LLFE;_tmp11B=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp11A)->f1;_LLFD: _tmp117=Cyc_CfFlowInfo_assign_place(e->loc,_tmp117,env->all_changed,
_tmp11B,_tmp112);_tmp118=Cyc_CfFlowInfo_reln_assign_exp(_tmp118,_tmpB7,_tmpB9);
_tmp114=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp11C=_cycalloc(
sizeof(*_tmp11C));_tmp11C[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp11D;
_tmp11D.tag=0;_tmp11D.f1=_tmp117;_tmp11D.f2=_tmp118;_tmp11D;});_tmp11C;});Cyc_NewControlFlow_update_tryflow(
env,_tmp114);return({struct _tuple0 _tmp11E;_tmp11E.f1=_tmp114;_tmp11E.f2=_tmp112;
_tmp11E;});_LLFE: if((int)_tmp11A != 0)goto _LLFB;_LLFF: return({struct _tuple0
_tmp11F;_tmp11F.f1=Cyc_NewControlFlow_use_Rval(env,_tmpB9->loc,_tmp114,_tmp112);
_tmp11F.f2=_tmp112;_tmp11F;});_LLFB:;}_LLF6:;}}inflow=_tmp115;}}}}}}_LLBA: if(*((
int*)_tmp9E)!= 7)goto _LLBC;_tmpBA=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp9E)->f1;
_tmpBB=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp9E)->f2;_LLBB: return Cyc_NewControlFlow_anal_Rexp(
env,(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpBA)).f1,_tmpBB);_LLBC: if(*((int*)
_tmp9E)!= 10)goto _LLBE;_tmpBC=((struct Cyc_Absyn_Throw_e_struct*)_tmp9E)->f1;
_LLBD: {void*_tmp121;void*_tmp122;struct _tuple0 _tmp120=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpBC);_tmp121=_tmp120.f1;_tmp122=_tmp120.f2;Cyc_NewControlFlow_use_Rval(
env,_tmpBC->loc,_tmp121,_tmp122);return({struct _tuple0 _tmp123;_tmp123.f1=(void*)
0;_tmp123.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp123;});}_LLBE: if(*((int*)
_tmp9E)!= 9)goto _LLC0;_tmpBD=((struct Cyc_Absyn_FnCall_e_struct*)_tmp9E)->f1;
_tmpBE=((struct Cyc_Absyn_FnCall_e_struct*)_tmp9E)->f2;_LLBF: {void*_tmp126;
struct Cyc_List_List*_tmp127;struct _tuple5 _tmp125=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_List_List*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->hd=
_tmpBD;_tmp124->tl=_tmpBE;_tmp124;}));_tmp126=_tmp125.f1;_tmp127=_tmp125.f2;{
void*_tmp128=Cyc_NewControlFlow_use_Rval(env,_tmpBD->loc,_tmp126,(void*)((struct
Cyc_List_List*)_check_null(_tmp127))->hd);_tmp127=_tmp127->tl;{struct Cyc_List_List*
init_params=0;{void*_tmp129=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpBD->topt))->v);struct Cyc_Absyn_PtrInfo _tmp12A;void*_tmp12B;
_LL101: if(_tmp129 <= (void*)3?1:*((int*)_tmp129)!= 4)goto _LL103;_tmp12A=((struct
Cyc_Absyn_PointerType_struct*)_tmp129)->f1;_tmp12B=(void*)_tmp12A.elt_typ;_LL102:{
void*_tmp12C=Cyc_Tcutil_compress(_tmp12B);struct Cyc_Absyn_FnInfo _tmp12D;struct
Cyc_List_List*_tmp12E;_LL106: if(_tmp12C <= (void*)3?1:*((int*)_tmp12C)!= 8)goto
_LL108;_tmp12D=((struct Cyc_Absyn_FnType_struct*)_tmp12C)->f1;_tmp12E=_tmp12D.attributes;
_LL107: for(0;_tmp12E != 0;_tmp12E=_tmp12E->tl){void*_tmp12F=(void*)_tmp12E->hd;
int _tmp130;_LL10B: if(_tmp12F <= (void*)17?1:*((int*)_tmp12F)!= 4)goto _LL10D;
_tmp130=((struct Cyc_Absyn_Initializes_att_struct*)_tmp12F)->f1;_LL10C:
init_params=({struct Cyc_List_List*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->hd=(
void*)_tmp130;_tmp131->tl=init_params;_tmp131;});goto _LL10A;_LL10D:;_LL10E: goto
_LL10A;_LL10A:;}goto _LL105;_LL108:;_LL109:({void*_tmp132[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp133="anal_Rexp: bad function type";
_tag_arr(_tmp133,sizeof(char),_get_zero_arr_size(_tmp133,29));}),_tag_arr(
_tmp132,sizeof(void*),0));});_LL105:;}goto _LL100;_LL103:;_LL104:({void*_tmp134[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp135="anal_Rexp: bad function type";_tag_arr(_tmp135,sizeof(char),
_get_zero_arr_size(_tmp135,29));}),_tag_arr(_tmp134,sizeof(void*),0));});_LL100:;}{
int i=1;for(0;_tmp127 != 0;(((_tmp127=_tmp127->tl,_tmpBE=((struct Cyc_List_List*)
_check_null(_tmpBE))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(
init_params,i)){_tmp128=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpBE))->hd)->loc,_tmp128,(void*)_tmp127->hd);
continue;}{void*_tmp136=_tmp126;struct Cyc_Dict_Dict*_tmp137;_LL110: if((int)
_tmp136 != 0)goto _LL112;_LL111: goto _LL10F;_LL112: if(_tmp136 <= (void*)1?1:*((int*)
_tmp136)!= 0)goto _LL10F;_tmp137=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp136)->f1;_LL113: if(Cyc_CfFlowInfo_initlevel(_tmp137,(void*)_tmp127->hd)== (
void*)0)({void*_tmp138[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpBE))->hd)->loc,({const char*_tmp139="expression may not be initialized";
_tag_arr(_tmp139,sizeof(char),_get_zero_arr_size(_tmp139,34));}),_tag_arr(
_tmp138,sizeof(void*),0));});{void*_tmp13A=_tmp128;struct Cyc_Dict_Dict*_tmp13B;
struct Cyc_List_List*_tmp13C;_LL115: if((int)_tmp13A != 0)goto _LL117;_LL116: goto
_LL114;_LL117: if(_tmp13A <= (void*)1?1:*((int*)_tmp13A)!= 0)goto _LL114;_tmp13B=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp13A)->f1;_tmp13C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp13A)->f2;_LL118: {struct Cyc_Dict_Dict*_tmp13D=Cyc_CfFlowInfo_escape_deref(
_tmp13B,env->all_changed,(void*)_tmp127->hd);{void*_tmp13E=(void*)_tmp127->hd;
struct Cyc_CfFlowInfo_Place*_tmp13F;_LL11A: if(_tmp13E <= (void*)3?1:*((int*)
_tmp13E)!= 2)goto _LL11C;_tmp13F=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp13E)->f1;
_LL11B:{void*_tmp140=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpBE))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp141;void*_tmp142;_LL11F: if(_tmp140 <= (void*)3?1:*((
int*)_tmp140)!= 4)goto _LL121;_tmp141=((struct Cyc_Absyn_PointerType_struct*)
_tmp140)->f1;_tmp142=(void*)_tmp141.elt_typ;_LL120: _tmp13D=Cyc_CfFlowInfo_assign_place(((
struct Cyc_Absyn_Exp*)_tmpBE->hd)->loc,_tmp13D,env->all_changed,_tmp13F,Cyc_CfFlowInfo_typ_to_absrval(
_tmp142,Cyc_CfFlowInfo_esc_all));goto _LL11E;_LL121:;_LL122:({void*_tmp143[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp144="anal_Rexp: bad type for initialized arg";_tag_arr(_tmp144,sizeof(char),
_get_zero_arr_size(_tmp144,40));}),_tag_arr(_tmp143,sizeof(void*),0));});_LL11E:;}
goto _LL119;_LL11C:;_LL11D: goto _LL119;_LL119:;}_tmp128=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp146;_tmp146.tag=0;_tmp146.f1=_tmp13D;_tmp146.f2=_tmp13C;_tmp146;});_tmp145;});
goto _LL114;}_LL114:;}goto _LL10F;_LL10F:;}}}if(Cyc_Tcutil_is_noreturn((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpBD->topt))->v))return({struct _tuple0 _tmp147;
_tmp147.f1=(void*)0;_tmp147.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp147;});else{return({
struct _tuple0 _tmp148;_tmp148.f1=_tmp128;_tmp148.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp148;});}}}}_LLC0: if(*((int*)_tmp9E)!= 33)goto _LLC2;_tmpBF=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp9E)->f1;_tmpC0=_tmpBF.is_calloc;_tmpC1=_tmpBF.rgn;_tmpC2=_tmpBF.elt_type;
_tmpC3=_tmpBF.num_elts;_tmpC4=_tmpBF.fat_result;_LLC1: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp15A;_tmp15A.tag=1;_tmp15A.f1=_tmpC3;
_tmp15A.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp15A;});_tmp159;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158->root=(void*)root;_tmp158->fields=0;
_tmp158;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp156=
_cycalloc(sizeof(*_tmp156));_tmp156[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp157;_tmp157.tag=2;_tmp157.f1=place;_tmp157;});_tmp156;});void*place_val=
_tmpC4?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(*((void**)_check_null(_tmpC2)),Cyc_CfFlowInfo_unknown_none);
void*outflow;if(env->all_changed != 0)*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((
struct Cyc_Set_Set**)_check_null(env->all_changed)),place);if(_tmpC1 != 0){void*
_tmp14B;struct Cyc_List_List*_tmp14C;struct _tuple5 _tmp14A=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp149[2];_tmp149[1]=_tmpC3;_tmp149[0]=(struct
Cyc_Absyn_Exp*)_tmpC1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp149,sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp14B=_tmp14A.f1;_tmp14C=
_tmp14A.f2;outflow=_tmp14B;}else{void*_tmp14E;struct _tuple0 _tmp14D=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpC3);_tmp14E=_tmp14D.f1;outflow=_tmp14E;}{void*_tmp14F=outflow;
struct Cyc_Dict_Dict*_tmp150;struct Cyc_List_List*_tmp151;_LL124: if((int)_tmp14F != 
0)goto _LL126;_LL125: return({struct _tuple0 _tmp152;_tmp152.f1=outflow;_tmp152.f2=
rval;_tmp152;});_LL126: if(_tmp14F <= (void*)1?1:*((int*)_tmp14F)!= 0)goto _LL123;
_tmp150=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp14F)->f1;_tmp151=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp14F)->f2;_LL127: return({struct _tuple0
_tmp153;_tmp153.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp154=
_cycalloc(sizeof(*_tmp154));_tmp154[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp155;_tmp155.tag=0;_tmp155.f1=Cyc_Dict_insert(_tmp150,root,place_val);_tmp155.f2=
_tmp151;_tmp155;});_tmp154;});_tmp153.f2=rval;_tmp153;});_LL123:;}}_LLC2: if(*((
int*)_tmp9E)!= 15)goto _LLC4;_tmpC5=((struct Cyc_Absyn_New_e_struct*)_tmp9E)->f1;
_tmpC6=((struct Cyc_Absyn_New_e_struct*)_tmp9E)->f2;_LLC3: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp16D;_tmp16D.tag=1;_tmp16D.f1=_tmpC6;
_tmp16D.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp16D;});_tmp16C;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->root=(void*)root;_tmp16B->fields=0;
_tmp16B;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp169=
_cycalloc(sizeof(*_tmp169));_tmp169[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp16A;_tmp16A.tag=2;_tmp16A.f1=place;_tmp16A;});_tmp169;});if(env->all_changed
!= 0)*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(env->all_changed)),place);{void*outflow;void*place_val;if(_tmpC5 != 0){
void*_tmp15D;struct Cyc_List_List*_tmp15E;struct _tuple5 _tmp15C=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp15B[2];_tmp15B[1]=_tmpC6;_tmp15B[0]=(struct
Cyc_Absyn_Exp*)_tmpC5;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp15B,sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp15D=_tmp15C.f1;_tmp15E=
_tmp15C.f2;outflow=_tmp15D;place_val=(void*)((struct Cyc_List_List*)_check_null(((
struct Cyc_List_List*)_check_null(_tmp15E))->tl))->hd;}else{void*_tmp160;void*
_tmp161;struct _tuple0 _tmp15F=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpC6);
_tmp160=_tmp15F.f1;_tmp161=_tmp15F.f2;outflow=_tmp160;place_val=_tmp161;}{void*
_tmp162=outflow;struct Cyc_Dict_Dict*_tmp163;struct Cyc_List_List*_tmp164;_LL129:
if((int)_tmp162 != 0)goto _LL12B;_LL12A: return({struct _tuple0 _tmp165;_tmp165.f1=
outflow;_tmp165.f2=rval;_tmp165;});_LL12B: if(_tmp162 <= (void*)1?1:*((int*)
_tmp162)!= 0)goto _LL128;_tmp163=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp162)->f1;_tmp164=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp162)->f2;
_LL12C: return({struct _tuple0 _tmp166;_tmp166.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp168;_tmp168.tag=0;_tmp168.f1=Cyc_Dict_insert(_tmp163,root,place_val);_tmp168.f2=
_tmp164;_tmp168;});_tmp167;});_tmp166.f2=rval;_tmp166;});_LL128:;}}}_LLC4: if(*((
int*)_tmp9E)!= 14)goto _LLC6;_tmpC7=((struct Cyc_Absyn_Address_e_struct*)_tmp9E)->f1;
_LLC5: {void*_tmp16F;void*_tmp170;struct _tuple0 _tmp16E=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmpC7);_tmp16F=_tmp16E.f1;_tmp170=_tmp16E.f2;{void*_tmp171=_tmp170;
struct Cyc_CfFlowInfo_Place*_tmp172;_LL12E: if((int)_tmp171 != 0)goto _LL130;_LL12F:
return({struct _tuple0 _tmp173;_tmp173.f1=_tmp16F;_tmp173.f2=(void*)1;_tmp173;});
_LL130: if(_tmp171 <= (void*)1?1:*((int*)_tmp171)!= 0)goto _LL12D;_tmp172=((struct
Cyc_CfFlowInfo_PlaceL_struct*)_tmp171)->f1;_LL131: return({struct _tuple0 _tmp174;
_tmp174.f1=_tmp16F;_tmp174.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp176;_tmp176.tag=2;_tmp176.f1=_tmp172;_tmp176;});_tmp175;});_tmp174;});_LL12D:;}}
_LLC6: if(*((int*)_tmp9E)!= 20)goto _LLC8;_tmpC8=((struct Cyc_Absyn_Deref_e_struct*)
_tmp9E)->f1;_LLC7: {void*_tmp178;void*_tmp179;struct _tuple0 _tmp177=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpC8);_tmp178=_tmp177.f1;_tmp179=_tmp177.f2;return Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp178,_tmpC8,_tmp179);}_LLC8: if(*((int*)_tmp9E)!= 21)goto _LLCA;
_tmpC9=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp9E)->f1;_tmpCA=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp9E)->f2;_LLC9: {void*_tmp17B;void*_tmp17C;struct _tuple0 _tmp17A=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpC9);_tmp17B=_tmp17A.f1;_tmp17C=_tmp17A.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpC9->topt))->v))return({struct _tuple0
_tmp17D;_tmp17D.f1=_tmp17B;_tmp17D.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp17D;});{
void*_tmp17E=_tmp17C;struct Cyc_Dict_Dict*_tmp17F;_LL133: if(_tmp17E <= (void*)3?1:*((
int*)_tmp17E)!= 4)goto _LL135;_tmp17F=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp17E)->f1;_LL134: return({struct _tuple0 _tmp180;_tmp180.f1=_tmp17B;_tmp180.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp17F,
_tmpCA);_tmp180;});_LL135:;_LL136:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181[0]=({struct Cyc_Core_Impossible_struct
_tmp182;_tmp182.tag=Cyc_Core_Impossible;_tmp182.f1=({const char*_tmp183="anal_Rexp: AggrMember";
_tag_arr(_tmp183,sizeof(char),_get_zero_arr_size(_tmp183,22));});_tmp182;});
_tmp181;}));_LL132:;}}_LLCA: if(*((int*)_tmp9E)!= 22)goto _LLCC;_tmpCB=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp9E)->f1;_tmpCC=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9E)->f2;_LLCB: {
void*_tmp185;void*_tmp186;struct _tuple0 _tmp184=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpCB);_tmp185=_tmp184.f1;_tmp186=_tmp184.f2;{void*_tmp188;void*_tmp189;
struct _tuple0 _tmp187=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp185,_tmpCB,
_tmp186);_tmp188=_tmp187.f1;_tmp189=_tmp187.f2;{void*_tmp18A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpCB->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp18B;void*_tmp18C;_LL138: if(_tmp18A <= (void*)3?1:*((int*)_tmp18A)!= 4)goto
_LL13A;_tmp18B=((struct Cyc_Absyn_PointerType_struct*)_tmp18A)->f1;_tmp18C=(void*)
_tmp18B.elt_typ;_LL139: if(Cyc_Absyn_is_union_type(_tmp18C))return({struct _tuple0
_tmp18D;_tmp18D.f1=_tmp188;_tmp18D.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp18D;});
goto _LL137;_LL13A:;_LL13B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E[0]=({struct Cyc_Core_Impossible_struct
_tmp18F;_tmp18F.tag=Cyc_Core_Impossible;_tmp18F.f1=({const char*_tmp190="anal_Rexp: AggrArrow ptr";
_tag_arr(_tmp190,sizeof(char),_get_zero_arr_size(_tmp190,25));});_tmp18F;});
_tmp18E;}));_LL137:;}{void*_tmp191=_tmp189;struct Cyc_Dict_Dict*_tmp192;_LL13D:
if(_tmp191 <= (void*)3?1:*((int*)_tmp191)!= 4)goto _LL13F;_tmp192=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp191)->f1;_LL13E: return({struct _tuple0 _tmp193;_tmp193.f1=_tmp188;_tmp193.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp192,
_tmpCC);_tmp193;});_LL13F:;_LL140:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194[0]=({struct Cyc_Core_Impossible_struct
_tmp195;_tmp195.tag=Cyc_Core_Impossible;_tmp195.f1=({const char*_tmp196="anal_Rexp: AggrArrow";
_tag_arr(_tmp196,sizeof(char),_get_zero_arr_size(_tmp196,21));});_tmp195;});
_tmp194;}));_LL13C:;}}}_LLCC: if(*((int*)_tmp9E)!= 6)goto _LLCE;_tmpCD=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp9E)->f1;_tmpCE=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9E)->f2;_tmpCF=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp9E)->f3;_LLCD: {void*_tmp198;void*
_tmp199;struct _tuple0 _tmp197=Cyc_NewControlFlow_anal_test(env,inflow,_tmpCD);
_tmp198=_tmp197.f1;_tmp199=_tmp197.f2;{struct _tuple0 _tmp19A=Cyc_NewControlFlow_anal_Rexp(
env,_tmp198,_tmpCE);struct _tuple0 _tmp19B=Cyc_NewControlFlow_anal_Rexp(env,
_tmp199,_tmpCF);return Cyc_CfFlowInfo_join_flow_and_rval(env->all_changed,_tmp19A,
_tmp19B);}}_LLCE: if(*((int*)_tmp9E)!= 23)goto _LLD0;_tmpD0=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp9E)->f1;_tmpD1=((struct Cyc_Absyn_Subscript_e_struct*)_tmp9E)->f2;_LLCF: {
void*_tmp19E;struct Cyc_List_List*_tmp19F;struct _tuple5 _tmp19D=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp19C[2];_tmp19C[1]=_tmpD1;_tmp19C[0]=_tmpD0;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp19C,
sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp19E=_tmp19D.f1;_tmp19F=_tmp19D.f2;{void*
_tmp1A0=_tmp19E;{void*_tmp1A1=_tmp19E;struct Cyc_Dict_Dict*_tmp1A2;struct Cyc_List_List*
_tmp1A3;_LL142: if(_tmp1A1 <= (void*)1?1:*((int*)_tmp1A1)!= 0)goto _LL144;_tmp1A2=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1A1)->f1;_tmp1A3=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1A1)->f2;_LL143: if(Cyc_CfFlowInfo_initlevel(_tmp1A2,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp19F))->tl))->hd)== (void*)0)({
void*_tmp1A4[0]={};Cyc_Tcutil_terr(_tmpD1->loc,({const char*_tmp1A5="expression may not be initialized";
_tag_arr(_tmp1A5,sizeof(char),_get_zero_arr_size(_tmp1A5,34));}),_tag_arr(
_tmp1A4,sizeof(void*),0));});{struct Cyc_List_List*_tmp1A6=Cyc_NewControlFlow_add_subscript_reln(
_tmp1A3,_tmpD0,_tmpD1);if(_tmp1A3 != _tmp1A6)_tmp1A0=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp1A8;_tmp1A8.tag=0;_tmp1A8.f1=_tmp1A2;_tmp1A8.f2=_tmp1A6;_tmp1A8;});_tmp1A7;});
goto _LL141;}_LL144:;_LL145: goto _LL141;_LL141:;}{void*_tmp1A9=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD0->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1AA;struct Cyc_Absyn_PtrAtts _tmp1AB;struct Cyc_Absyn_Conref*_tmp1AC;_LL147: if(
_tmp1A9 <= (void*)3?1:*((int*)_tmp1A9)!= 9)goto _LL149;_LL148: {void*_tmp1AD=(void*)((
struct Cyc_List_List*)_check_null(_tmp19F))->hd;struct Cyc_Dict_Dict*_tmp1AE;
_LL14E: if(_tmp1AD <= (void*)3?1:*((int*)_tmp1AD)!= 4)goto _LL150;_tmp1AE=((struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmp1AD)->f1;_LL14F: return({struct _tuple0
_tmp1AF;_tmp1AF.f1=_tmp1A0;_tmp1AF.f2=((void*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_lookup)(_tmp1AE,Cyc_Absyn_fieldname((int)(Cyc_Evexp_eval_const_uint_exp(
_tmpD1)).f1));_tmp1AF;});_LL150:;_LL151:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0[0]=({struct Cyc_Core_Impossible_struct
_tmp1B1;_tmp1B1.tag=Cyc_Core_Impossible;_tmp1B1.f1=({const char*_tmp1B2="anal_Rexp: Subscript";
_tag_arr(_tmp1B2,sizeof(char),_get_zero_arr_size(_tmp1B2,21));});_tmp1B1;});
_tmp1B0;}));_LL14D:;}_LL149: if(_tmp1A9 <= (void*)3?1:*((int*)_tmp1A9)!= 4)goto
_LL14B;_tmp1AA=((struct Cyc_Absyn_PointerType_struct*)_tmp1A9)->f1;_tmp1AB=
_tmp1AA.ptr_atts;_tmp1AC=_tmp1AB.bounds;_LL14A:{void*_tmp1B3=(void*)(Cyc_Absyn_compress_conref(
_tmp1AC))->v;void*_tmp1B4;_LL153: if(_tmp1B3 <= (void*)1?1:*((int*)_tmp1B3)!= 0)
goto _LL155;_tmp1B4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp1B3)->f1;if(
_tmp1B4 <= (void*)1?1:*((int*)_tmp1B4)!= 1)goto _LL155;_LL154:{void*_tmp1B5=(void*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp19F))->tl))->hd;
struct Cyc_List_List*_tmp1B6;_LL158: if(_tmp1B5 <= (void*)3?1:*((int*)_tmp1B5)!= 3)
goto _LL15A;_tmp1B6=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1B5)->f1;_LL159:(
void*)(_tmpD1->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp1B8;_tmp1B8.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp1B8.f1=_tmp1B6;_tmp1B8;});
_tmp1B7;})));goto _LL157;_LL15A:;_LL15B: goto _LL157;_LL157:;}goto _LL152;_LL155:;
_LL156: goto _LL152;_LL152:;}{void*_tmp1BA;void*_tmp1BB;struct _tuple0 _tmp1B9=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp19E,_tmpD0,(void*)((struct Cyc_List_List*)_check_null(_tmp19F))->hd);
_tmp1BA=_tmp1B9.f1;_tmp1BB=_tmp1B9.f2;return _tmp1BA == (void*)0?({struct _tuple0
_tmp1BC;_tmp1BC.f1=_tmp1BA;_tmp1BC.f2=_tmp1BB;_tmp1BC;}):({struct _tuple0 _tmp1BD;
_tmp1BD.f1=_tmp1A0;_tmp1BD.f2=_tmp1BB;_tmp1BD;});}_LL14B:;_LL14C:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));
_tmp1BE[0]=({struct Cyc_Core_Impossible_struct _tmp1BF;_tmp1BF.tag=Cyc_Core_Impossible;
_tmp1BF.f1=({const char*_tmp1C0="anal_Rexp: Subscript -- bad type";_tag_arr(
_tmp1C0,sizeof(char),_get_zero_arr_size(_tmp1C0,33));});_tmp1BF;});_tmp1BE;}));
_LL146:;}}}_LLD0: if(*((int*)_tmp9E)!= 30)goto _LLD2;_tmpD2=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp9E)->f1;_LLD1: _tmpD3=_tmpD2;goto _LLD3;_LLD2: if(*((int*)_tmp9E)!= 24)goto
_LLD4;_tmpD3=((struct Cyc_Absyn_Tuple_e_struct*)_tmp9E)->f1;_LLD3: {void*_tmp1C2;
struct Cyc_List_List*_tmp1C3;struct _tuple5 _tmp1C1=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmpD3);_tmp1C2=_tmp1C1.f1;_tmp1C3=_tmp1C1.f2;{struct Cyc_Dict_Dict*
aggrdict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{int i=0;for(0;_tmp1C3 != 0;(_tmp1C3=
_tmp1C3->tl,++ i)){aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,Cyc_Absyn_fieldname(i),(
void*)_tmp1C3->hd);}}return({struct _tuple0 _tmp1C4;_tmp1C4.f1=_tmp1C2;_tmp1C4.f2=(
void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));
_tmp1C5[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp1C6;_tmp1C6.tag=4;_tmp1C6.f1=
aggrdict;_tmp1C6;});_tmp1C5;});_tmp1C4;});}}_LLD4: if(*((int*)_tmp9E)!= 29)goto
_LLD6;_tmpD4=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp9E)->f2;_LLD5: _tmpD5=
_tmpD4;goto _LLD7;_LLD6: if(*((int*)_tmp9E)!= 28)goto _LLD8;_tmpD5=((struct Cyc_Absyn_Struct_e_struct*)
_tmp9E)->f3;_LLD7: {void*_tmp1C8;struct Cyc_List_List*_tmp1C9;struct _tuple5
_tmp1C7=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple6*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple6*))Cyc_Core_snd,_tmpD5));_tmp1C8=_tmp1C7.f1;
_tmp1C9=_tmp1C7.f2;{struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{int
i=0;for(0;_tmp1C9 != 0;(((_tmp1C9=_tmp1C9->tl,_tmpD5=_tmpD5->tl)),++ i)){struct Cyc_List_List*
ds=(*((struct _tuple6*)((struct Cyc_List_List*)_check_null(_tmpD5))->hd)).f1;for(0;
ds != 0;ds=ds->tl){void*_tmp1CA=(void*)ds->hd;struct _tagged_arr*_tmp1CB;_LL15D:
if(*((int*)_tmp1CA)!= 0)goto _LL15F;_LL15E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC[0]=({struct Cyc_Core_Impossible_struct
_tmp1CD;_tmp1CD.tag=Cyc_Core_Impossible;_tmp1CD.f1=({const char*_tmp1CE="anal_Rexp:Struct_e";
_tag_arr(_tmp1CE,sizeof(char),_get_zero_arr_size(_tmp1CE,19));});_tmp1CD;});
_tmp1CC;}));_LL15F: if(*((int*)_tmp1CA)!= 1)goto _LL15C;_tmp1CB=((struct Cyc_Absyn_FieldName_struct*)
_tmp1CA)->f1;_LL160: aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,_tmp1CB,(void*)_tmp1C9->hd);
_LL15C:;}}}return({struct _tuple0 _tmp1CF;_tmp1CF.f1=_tmp1C8;_tmp1CF.f2=(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));
_tmp1D0[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp1D1;_tmp1D1.tag=4;_tmp1D1.f1=
aggrdict;_tmp1D1;});_tmp1D0;});_tmp1CF;});}}_LLD8: if(*((int*)_tmp9E)!= 26)goto
_LLDA;_tmpD6=((struct Cyc_Absyn_Array_e_struct*)_tmp9E)->f1;_LLD9: {struct Cyc_List_List*
_tmp1D2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple6*),
struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple6*))
Cyc_Core_snd,_tmpD6);void*_tmp1D4;struct Cyc_List_List*_tmp1D5;struct _tuple5
_tmp1D3=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp1D2);_tmp1D4=
_tmp1D3.f1;_tmp1D5=_tmp1D3.f2;for(0;_tmp1D5 != 0;(_tmp1D5=_tmp1D5->tl,_tmp1D2=
_tmp1D2->tl)){_tmp1D4=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1D2))->hd)->loc,_tmp1D4,(void*)_tmp1D5->hd);}
return({struct _tuple0 _tmp1D6;_tmp1D6.f1=_tmp1D4;_tmp1D6.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp1D6;});}_LLDA: if(*((int*)_tmp9E)!= 27)goto _LLDC;_tmpD7=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp9E)->f1;_tmpD8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp9E)->f2;_tmpD9=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp9E)->f3;_tmpDA=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp9E)->f4;_LLDB: {void*_tmp1D8;void*_tmp1D9;struct _tuple0 _tmp1D7=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpD8);_tmp1D8=_tmp1D7.f1;_tmp1D9=_tmp1D7.f2;{void*_tmp1DA=_tmp1D8;
struct Cyc_Dict_Dict*_tmp1DB;struct Cyc_List_List*_tmp1DC;_LL162: if((int)_tmp1DA != 
0)goto _LL164;_LL163: return({struct _tuple0 _tmp1DD;_tmp1DD.f1=_tmp1D8;_tmp1DD.f2=
Cyc_CfFlowInfo_unknown_all;_tmp1DD;});_LL164: if(_tmp1DA <= (void*)1?1:*((int*)
_tmp1DA)!= 0)goto _LL161;_tmp1DB=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1DA)->f1;_tmp1DC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1DA)->f2;
_LL165: if(Cyc_CfFlowInfo_initlevel(_tmp1DB,_tmp1D9)== (void*)0)({void*_tmp1DE[0]={};
Cyc_Tcutil_terr(_tmpD8->loc,({const char*_tmp1DF="expression may not be initialized";
_tag_arr(_tmp1DF,sizeof(char),_get_zero_arr_size(_tmp1DF,34));}),_tag_arr(
_tmp1DE,sizeof(void*),0));});{struct Cyc_List_List*new_relns=_tmp1DC;comp_loop: {
void*_tmp1E0=(void*)_tmpD8->r;struct Cyc_Absyn_Exp*_tmp1E1;void*_tmp1E2;struct Cyc_Absyn_Vardecl*
_tmp1E3;void*_tmp1E4;struct Cyc_Absyn_Vardecl*_tmp1E5;void*_tmp1E6;struct Cyc_Absyn_Vardecl*
_tmp1E7;void*_tmp1E8;struct Cyc_Absyn_Vardecl*_tmp1E9;void*_tmp1EA;int _tmp1EB;
void*_tmp1EC;struct Cyc_List_List*_tmp1ED;struct Cyc_List_List _tmp1EE;struct Cyc_Absyn_Exp*
_tmp1EF;_LL167: if(*((int*)_tmp1E0)!= 13)goto _LL169;_tmp1E1=((struct Cyc_Absyn_Cast_e_struct*)
_tmp1E0)->f2;_LL168: _tmpD8=_tmp1E1;goto comp_loop;_LL169: if(*((int*)_tmp1E0)!= 1)
goto _LL16B;_tmp1E2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1E0)->f2;if(
_tmp1E2 <= (void*)1?1:*((int*)_tmp1E2)!= 0)goto _LL16B;_tmp1E3=((struct Cyc_Absyn_Global_b_struct*)
_tmp1E2)->f1;if(!(!_tmp1E3->escapes))goto _LL16B;_LL16A: _tmp1E5=_tmp1E3;goto
_LL16C;_LL16B: if(*((int*)_tmp1E0)!= 1)goto _LL16D;_tmp1E4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1E0)->f2;if(_tmp1E4 <= (void*)1?1:*((int*)_tmp1E4)!= 3)goto _LL16D;_tmp1E5=((
struct Cyc_Absyn_Local_b_struct*)_tmp1E4)->f1;if(!(!_tmp1E5->escapes))goto _LL16D;
_LL16C: _tmp1E7=_tmp1E5;goto _LL16E;_LL16D: if(*((int*)_tmp1E0)!= 1)goto _LL16F;
_tmp1E6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1E0)->f2;if(_tmp1E6 <= (void*)
1?1:*((int*)_tmp1E6)!= 4)goto _LL16F;_tmp1E7=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1E6)->f1;if(!(!_tmp1E7->escapes))goto _LL16F;_LL16E: _tmp1E9=_tmp1E7;goto
_LL170;_LL16F: if(*((int*)_tmp1E0)!= 1)goto _LL171;_tmp1E8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1E0)->f2;if(_tmp1E8 <= (void*)1?1:*((int*)_tmp1E8)!= 2)goto _LL171;_tmp1E9=((
struct Cyc_Absyn_Param_b_struct*)_tmp1E8)->f1;if(!(!_tmp1E9->escapes))goto _LL171;
_LL170: new_relns=({struct Cyc_List_List*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));
_tmp1F0->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));
_tmp1F1->vd=_tmpD7;_tmp1F1->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp1F3;_tmp1F3.tag=1;_tmp1F3.f1=_tmp1E9;_tmp1F3;});_tmp1F2;}));_tmp1F1;});
_tmp1F0->tl=_tmp1DC;_tmp1F0;});goto _LL166;_LL171: if(*((int*)_tmp1E0)!= 0)goto
_LL173;_tmp1EA=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp1E0)->f1;if(_tmp1EA
<= (void*)1?1:*((int*)_tmp1EA)!= 2)goto _LL173;_tmp1EB=((struct Cyc_Absyn_Int_c_struct*)
_tmp1EA)->f2;_LL172: new_relns=({struct Cyc_List_List*_tmp1F4=_cycalloc(sizeof(*
_tmp1F4));_tmp1F4->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1F5=_cycalloc(sizeof(*
_tmp1F5));_tmp1F5->vd=_tmpD7;_tmp1F5->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp1F6=_cycalloc_atomic(sizeof(*_tmp1F6));_tmp1F6[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp1F7;_tmp1F7.tag=3;_tmp1F7.f1=(unsigned int)_tmp1EB;_tmp1F7;});_tmp1F6;}));
_tmp1F5;});_tmp1F4->tl=_tmp1DC;_tmp1F4;});goto _LL166;_LL173: if(*((int*)_tmp1E0)
!= 3)goto _LL175;_tmp1EC=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1E0)->f1;
if((int)_tmp1EC != 19)goto _LL175;_tmp1ED=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1E0)->f2;if(_tmp1ED == 0)goto _LL175;_tmp1EE=*_tmp1ED;_tmp1EF=(struct Cyc_Absyn_Exp*)
_tmp1EE.hd;_LL174:{void*_tmp1F8=(void*)_tmp1EF->r;void*_tmp1F9;struct Cyc_Absyn_Vardecl*
_tmp1FA;void*_tmp1FB;struct Cyc_Absyn_Vardecl*_tmp1FC;void*_tmp1FD;struct Cyc_Absyn_Vardecl*
_tmp1FE;void*_tmp1FF;struct Cyc_Absyn_Vardecl*_tmp200;_LL178: if(*((int*)_tmp1F8)
!= 1)goto _LL17A;_tmp1F9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F8)->f2;if(
_tmp1F9 <= (void*)1?1:*((int*)_tmp1F9)!= 0)goto _LL17A;_tmp1FA=((struct Cyc_Absyn_Global_b_struct*)
_tmp1F9)->f1;if(!(!_tmp1FA->escapes))goto _LL17A;_LL179: _tmp1FC=_tmp1FA;goto
_LL17B;_LL17A: if(*((int*)_tmp1F8)!= 1)goto _LL17C;_tmp1FB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F8)->f2;if(_tmp1FB <= (void*)1?1:*((int*)_tmp1FB)!= 3)goto _LL17C;_tmp1FC=((
struct Cyc_Absyn_Local_b_struct*)_tmp1FB)->f1;if(!(!_tmp1FC->escapes))goto _LL17C;
_LL17B: _tmp1FE=_tmp1FC;goto _LL17D;_LL17C: if(*((int*)_tmp1F8)!= 1)goto _LL17E;
_tmp1FD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F8)->f2;if(_tmp1FD <= (void*)
1?1:*((int*)_tmp1FD)!= 4)goto _LL17E;_tmp1FE=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1FD)->f1;if(!(!_tmp1FE->escapes))goto _LL17E;_LL17D: _tmp200=_tmp1FE;goto
_LL17F;_LL17E: if(*((int*)_tmp1F8)!= 1)goto _LL180;_tmp1FF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F8)->f2;if(_tmp1FF <= (void*)1?1:*((int*)_tmp1FF)!= 2)goto _LL180;_tmp200=((
struct Cyc_Absyn_Param_b_struct*)_tmp1FF)->f1;if(!(!_tmp200->escapes))goto _LL180;
_LL17F: new_relns=({struct Cyc_List_List*_tmp201=_cycalloc(sizeof(*_tmp201));
_tmp201->hd=({struct Cyc_CfFlowInfo_Reln*_tmp202=_cycalloc(sizeof(*_tmp202));
_tmp202->vd=_tmpD7;_tmp202->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp204;_tmp204.tag=2;_tmp204.f1=_tmp200;_tmp204;});_tmp203;}));_tmp202;});
_tmp201->tl=_tmp1DC;_tmp201;});goto _LL177;_LL180:;_LL181: goto _LL177;_LL177:;}
goto _LL166;_LL175:;_LL176: goto _LL166;_LL166:;}if(_tmp1DC != new_relns)_tmp1D8=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp205=_cycalloc(sizeof(*
_tmp205));_tmp205[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp206;_tmp206.tag=
0;_tmp206.f1=_tmp1DB;_tmp206.f2=new_relns;_tmp206;});_tmp205;});{void*_tmp207=
_tmp1D9;_LL183: if((int)_tmp207 != 0)goto _LL185;_LL184: return({struct _tuple0
_tmp208;_tmp208.f1=_tmp1D8;_tmp208.f2=Cyc_CfFlowInfo_unknown_all;_tmp208;});
_LL185: if((int)_tmp207 != 2)goto _LL187;_LL186: goto _LL188;_LL187: if((int)_tmp207 != 
1)goto _LL189;_LL188: goto _LL18A;_LL189: if(_tmp207 <= (void*)3?1:*((int*)_tmp207)!= 
2)goto _LL18B;_LL18A: {struct Cyc_List_List _tmp209=({struct Cyc_List_List _tmp213;
_tmp213.hd=_tmpD7;_tmp213.tl=0;_tmp213;});_tmp1D8=Cyc_NewControlFlow_add_vars(
_tmp1D8,(struct Cyc_List_List*)& _tmp209,Cyc_CfFlowInfo_unknown_all);{void*_tmp20B;
void*_tmp20C;struct _tuple0 _tmp20A=Cyc_NewControlFlow_anal_Rexp(env,_tmp1D8,
_tmpD9);_tmp20B=_tmp20A.f1;_tmp20C=_tmp20A.f2;{void*_tmp20D=_tmp20B;struct Cyc_Dict_Dict*
_tmp20E;_LL18E: if((int)_tmp20D != 0)goto _LL190;_LL18F: return({struct _tuple0
_tmp20F;_tmp20F.f1=_tmp20B;_tmp20F.f2=Cyc_CfFlowInfo_unknown_all;_tmp20F;});
_LL190: if(_tmp20D <= (void*)1?1:*((int*)_tmp20D)!= 0)goto _LL18D;_tmp20E=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp20D)->f1;_LL191: if(Cyc_CfFlowInfo_initlevel(
_tmp20E,_tmp20C)!= (void*)2){({void*_tmp210[0]={};Cyc_Tcutil_terr(_tmpD8->loc,({
const char*_tmp211="expression may not be initialized";_tag_arr(_tmp211,sizeof(
char),_get_zero_arr_size(_tmp211,34));}),_tag_arr(_tmp210,sizeof(void*),0));});
return({struct _tuple0 _tmp212;_tmp212.f1=(void*)0;_tmp212.f2=Cyc_CfFlowInfo_unknown_all;
_tmp212;});}_LL18D:;}_tmp1D8=_tmp20B;goto _LL18C;}}_LL18B:;_LL18C: while(1){struct
Cyc_List_List _tmp214=({struct Cyc_List_List _tmp21E;_tmp21E.hd=_tmpD7;_tmp21E.tl=0;
_tmp21E;});_tmp1D8=Cyc_NewControlFlow_add_vars(_tmp1D8,(struct Cyc_List_List*)&
_tmp214,Cyc_CfFlowInfo_unknown_all);{void*_tmp216;void*_tmp217;struct _tuple0
_tmp215=Cyc_NewControlFlow_anal_Rexp(env,_tmp1D8,_tmpD9);_tmp216=_tmp215.f1;
_tmp217=_tmp215.f2;{void*_tmp218=_tmp216;struct Cyc_Dict_Dict*_tmp219;_LL193: if((
int)_tmp218 != 0)goto _LL195;_LL194: goto _LL192;_LL195: if(_tmp218 <= (void*)1?1:*((
int*)_tmp218)!= 0)goto _LL192;_tmp219=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp218)->f1;_LL196: if(Cyc_CfFlowInfo_initlevel(_tmp219,_tmp217)!= (void*)2){({
void*_tmp21A[0]={};Cyc_Tcutil_terr(_tmpD8->loc,({const char*_tmp21B="expression may not be initialized";
_tag_arr(_tmp21B,sizeof(char),_get_zero_arr_size(_tmp21B,34));}),_tag_arr(
_tmp21A,sizeof(void*),0));});return({struct _tuple0 _tmp21C;_tmp21C.f1=(void*)0;
_tmp21C.f2=Cyc_CfFlowInfo_unknown_all;_tmp21C;});}_LL192:;}{void*_tmp21D=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp1D8,_tmp216);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp21D,
_tmp1D8))break;_tmp1D8=_tmp21D;}}}return({struct _tuple0 _tmp21F;_tmp21F.f1=
_tmp1D8;_tmp21F.f2=Cyc_CfFlowInfo_unknown_all;_tmp21F;});_LL182:;}}_LL161:;}}
_LLDC: if(*((int*)_tmp9E)!= 35)goto _LLDE;_tmpDB=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp9E)->f1;_LLDD: while(1){inflow=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
inflow,_tmpDB))->flow;{void*_tmp220=(void*)_tmpDB->r;struct Cyc_Absyn_Stmt*
_tmp221;struct Cyc_Absyn_Stmt*_tmp222;struct Cyc_Absyn_Decl*_tmp223;struct Cyc_Absyn_Stmt*
_tmp224;struct Cyc_Absyn_Exp*_tmp225;_LL198: if(_tmp220 <= (void*)1?1:*((int*)
_tmp220)!= 1)goto _LL19A;_tmp221=((struct Cyc_Absyn_Seq_s_struct*)_tmp220)->f1;
_tmp222=((struct Cyc_Absyn_Seq_s_struct*)_tmp220)->f2;_LL199: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp221);_tmpDB=_tmp222;goto _LL197;_LL19A: if(_tmp220 <= (void*)1?1:*((
int*)_tmp220)!= 12)goto _LL19C;_tmp223=((struct Cyc_Absyn_Decl_s_struct*)_tmp220)->f1;
_tmp224=((struct Cyc_Absyn_Decl_s_struct*)_tmp220)->f2;_LL19B: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp223);_tmpDB=_tmp224;goto _LL197;_LL19C: if(_tmp220 <= (void*)1?1:*((
int*)_tmp220)!= 0)goto _LL19E;_tmp225=((struct Cyc_Absyn_Exp_s_struct*)_tmp220)->f1;
_LL19D: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp225);_LL19E:;_LL19F:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp226=_cycalloc(sizeof(*
_tmp226));_tmp226[0]=({struct Cyc_Core_Impossible_struct _tmp227;_tmp227.tag=Cyc_Core_Impossible;
_tmp227.f1=({const char*_tmp228="analyze_Rexp: ill-formed StmtExp";_tag_arr(
_tmp228,sizeof(char),_get_zero_arr_size(_tmp228,33));});_tmp227;});_tmp226;}));
_LL197:;}}_LLDE: if(*((int*)_tmp9E)!= 1)goto _LLE0;_tmpDC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp9E)->f2;if((int)_tmpDC != 0)goto _LLE0;_LLDF: goto _LLE1;_LLE0: if(*((int*)_tmp9E)
!= 2)goto _LLE2;_LLE1: goto _LLE3;_LLE2: if(*((int*)_tmp9E)!= 8)goto _LLE4;_LLE3: goto
_LLE5;_LLE4: if(*((int*)_tmp9E)!= 34)goto _LLE6;_LLE5: goto _LLE7;_LLE6: if(*((int*)
_tmp9E)!= 25)goto _LLE8;_LLE7: goto _LLE9;_LLE8: if(*((int*)_tmp9E)!= 36)goto _LLEA;
_LLE9: goto _LLEB;_LLEA: if(*((int*)_tmp9E)!= 37)goto _LL89;_LLEB:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229[0]=({
struct Cyc_Core_Impossible_struct _tmp22A;_tmp22A.tag=Cyc_Core_Impossible;_tmp22A.f1=({
const char*_tmp22B="anal_Rexp, unexpected exp form";_tag_arr(_tmp22B,sizeof(char),
_get_zero_arr_size(_tmp22B,31));});_tmp22A;});_tmp229;}));_LL89:;}}static struct
_tuple0 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,void*f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){void*
_tmp22C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp22D;void*_tmp22E;struct Cyc_Absyn_PtrAtts _tmp22F;
struct Cyc_Absyn_Conref*_tmp230;struct Cyc_Absyn_Conref*_tmp231;_LL1A1: if(_tmp22C
<= (void*)3?1:*((int*)_tmp22C)!= 4)goto _LL1A3;_tmp22D=((struct Cyc_Absyn_PointerType_struct*)
_tmp22C)->f1;_tmp22E=(void*)_tmp22D.elt_typ;_tmp22F=_tmp22D.ptr_atts;_tmp230=
_tmp22F.bounds;_tmp231=_tmp22F.zero_term;_LL1A2: {void*_tmp232=f;struct Cyc_Dict_Dict*
_tmp233;struct Cyc_List_List*_tmp234;_LL1A6: if((int)_tmp232 != 0)goto _LL1A8;_LL1A7:
return({struct _tuple0 _tmp235;_tmp235.f1=f;_tmp235.f2=(void*)0;_tmp235;});_LL1A8:
if(_tmp232 <= (void*)1?1:*((int*)_tmp232)!= 0)goto _LL1A5;_tmp233=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp232)->f1;_tmp234=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp232)->f2;
_LL1A9: if(Cyc_Tcutil_is_bound_one(_tmp230)){void*_tmp236=r;struct Cyc_CfFlowInfo_Place*
_tmp237;struct Cyc_CfFlowInfo_Place _tmp238;void*_tmp239;struct Cyc_List_List*
_tmp23A;void*_tmp23B;_LL1AB: if((int)_tmp236 != 1)goto _LL1AD;_LL1AC: goto _LL1AE;
_LL1AD: if((int)_tmp236 != 2)goto _LL1AF;_LL1AE:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_NotZero_struct*_tmp23C=_cycalloc(sizeof(*_tmp23C));_tmp23C[
0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp23D;_tmp23D.tag=Cyc_CfFlowInfo_NotZero;
_tmp23D.f1=_tmp234;_tmp23D;});_tmp23C;})));goto _LL1AA;_LL1AF: if(_tmp236 <= (void*)
3?1:*((int*)_tmp236)!= 2)goto _LL1B1;_tmp237=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp236)->f1;_tmp238=*_tmp237;_tmp239=(void*)_tmp238.root;_tmp23A=_tmp238.fields;
_LL1B0:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp23F;_tmp23F.tag=Cyc_CfFlowInfo_NotZero;_tmp23F.f1=_tmp234;_tmp23F;});_tmp23E;})));
return({struct _tuple0 _tmp240;_tmp240.f1=f;_tmp240.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp241=_cycalloc(sizeof(*_tmp241));_tmp241[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp242;_tmp242.tag=0;_tmp242.f1=({struct Cyc_CfFlowInfo_Place*_tmp243=_cycalloc(
sizeof(*_tmp243));_tmp243->root=(void*)_tmp239;_tmp243->fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp23A,flds);
_tmp243;});_tmp242;});_tmp241;});_tmp240;});_LL1B1: if((int)_tmp236 != 0)goto
_LL1B3;_LL1B2:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));return({
struct _tuple0 _tmp244;_tmp244.f1=(void*)0;_tmp244.f2=(void*)0;_tmp244;});_LL1B3:
if(_tmp236 <= (void*)3?1:*((int*)_tmp236)!= 0)goto _LL1B5;_tmp23B=(void*)((struct
Cyc_CfFlowInfo_UnknownR_struct*)_tmp236)->f1;_LL1B4: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp23B);goto _LL1B6;_LL1B5:;_LL1B6:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245[
0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp246;_tmp246.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp246.f1=_tmp234;_tmp246;});_tmp245;})));_LL1AA:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp247=_cycalloc(sizeof(*_tmp247));
_tmp247[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp248;_tmp248.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp248.f1=_tmp234;_tmp248;});_tmp247;})));}if(Cyc_CfFlowInfo_initlevel(_tmp233,
r)== (void*)0)({void*_tmp249[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp24A="dereference of possibly uninitialized pointer";
_tag_arr(_tmp24A,sizeof(char),_get_zero_arr_size(_tmp24A,46));}),_tag_arr(
_tmp249,sizeof(void*),0));});return({struct _tuple0 _tmp24B;_tmp24B.f1=f;_tmp24B.f2=(
void*)0;_tmp24B;});_LL1A5:;}_LL1A3:;_LL1A4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C[0]=({struct Cyc_Core_Impossible_struct
_tmp24D;_tmp24D.tag=Cyc_Core_Impossible;_tmp24D.f1=({const char*_tmp24E="left deref of non-pointer-type";
_tag_arr(_tmp24E,sizeof(char),_get_zero_arr_size(_tmp24E,31));});_tmp24D;});
_tmp24C;}));_LL1A0:;}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(struct
Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict*d;{void*_tmp24F=inflow;struct Cyc_Dict_Dict*_tmp250;
struct Cyc_List_List*_tmp251;_LL1B8: if((int)_tmp24F != 0)goto _LL1BA;_LL1B9: return({
struct _tuple0 _tmp252;_tmp252.f1=(void*)0;_tmp252.f2=(void*)0;_tmp252;});_LL1BA:
if(_tmp24F <= (void*)1?1:*((int*)_tmp24F)!= 0)goto _LL1B7;_tmp250=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp24F)->f1;_tmp251=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp24F)->f2;
_LL1BB: d=_tmp250;_LL1B7:;}{void*_tmp253=(void*)e->r;struct Cyc_Absyn_Exp*_tmp254;
struct Cyc_Absyn_Exp*_tmp255;struct Cyc_Absyn_Exp*_tmp256;void*_tmp257;struct Cyc_Absyn_Vardecl*
_tmp258;void*_tmp259;struct Cyc_Absyn_Vardecl*_tmp25A;void*_tmp25B;struct Cyc_Absyn_Vardecl*
_tmp25C;void*_tmp25D;struct Cyc_Absyn_Vardecl*_tmp25E;struct Cyc_Absyn_Exp*_tmp25F;
struct _tagged_arr*_tmp260;struct Cyc_Absyn_Exp*_tmp261;struct Cyc_Absyn_Exp*
_tmp262;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Exp*_tmp264;struct
_tagged_arr*_tmp265;_LL1BD: if(*((int*)_tmp253)!= 13)goto _LL1BF;_tmp254=((struct
Cyc_Absyn_Cast_e_struct*)_tmp253)->f2;_LL1BE: _tmp255=_tmp254;goto _LL1C0;_LL1BF:
if(*((int*)_tmp253)!= 11)goto _LL1C1;_tmp255=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp253)->f1;_LL1C0: _tmp256=_tmp255;goto _LL1C2;_LL1C1: if(*((int*)_tmp253)!= 12)
goto _LL1C3;_tmp256=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp253)->f1;_LL1C2:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp256,flds);_LL1C3: if(*((int*)
_tmp253)!= 1)goto _LL1C5;_tmp257=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp253)->f2;
if(_tmp257 <= (void*)1?1:*((int*)_tmp257)!= 2)goto _LL1C5;_tmp258=((struct Cyc_Absyn_Param_b_struct*)
_tmp257)->f1;_LL1C4: _tmp25A=_tmp258;goto _LL1C6;_LL1C5: if(*((int*)_tmp253)!= 1)
goto _LL1C7;_tmp259=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp253)->f2;if(
_tmp259 <= (void*)1?1:*((int*)_tmp259)!= 3)goto _LL1C7;_tmp25A=((struct Cyc_Absyn_Local_b_struct*)
_tmp259)->f1;_LL1C6: _tmp25C=_tmp25A;goto _LL1C8;_LL1C7: if(*((int*)_tmp253)!= 1)
goto _LL1C9;_tmp25B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp253)->f2;if(
_tmp25B <= (void*)1?1:*((int*)_tmp25B)!= 4)goto _LL1C9;_tmp25C=((struct Cyc_Absyn_Pat_b_struct*)
_tmp25B)->f1;_LL1C8: return({struct _tuple0 _tmp266;_tmp266.f1=inflow;_tmp266.f2=(
void*)({struct Cyc_CfFlowInfo_PlaceL_struct*_tmp267=_cycalloc(sizeof(*_tmp267));
_tmp267[0]=({struct Cyc_CfFlowInfo_PlaceL_struct _tmp268;_tmp268.tag=0;_tmp268.f1=({
struct Cyc_CfFlowInfo_Place*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp26A=_cycalloc(sizeof(*
_tmp26A));_tmp26A[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp26B;_tmp26B.tag=0;
_tmp26B.f1=_tmp25C;_tmp26B;});_tmp26A;}));_tmp269->fields=flds;_tmp269;});
_tmp268;});_tmp267;});_tmp266;});_LL1C9: if(*((int*)_tmp253)!= 1)goto _LL1CB;
_tmp25D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp253)->f2;if(_tmp25D <= (void*)
1?1:*((int*)_tmp25D)!= 0)goto _LL1CB;_tmp25E=((struct Cyc_Absyn_Global_b_struct*)
_tmp25D)->f1;_LL1CA: return({struct _tuple0 _tmp26C;_tmp26C.f1=inflow;_tmp26C.f2=(
void*)0;_tmp26C;});_LL1CB: if(*((int*)_tmp253)!= 22)goto _LL1CD;_tmp25F=((struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp253)->f1;_tmp260=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp253)->f2;_LL1CC:{void*_tmp26D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp25F->topt))->v);struct Cyc_Absyn_PtrInfo _tmp26E;void*_tmp26F;
_LL1D6: if(_tmp26D <= (void*)3?1:*((int*)_tmp26D)!= 4)goto _LL1D8;_tmp26E=((struct
Cyc_Absyn_PointerType_struct*)_tmp26D)->f1;_tmp26F=(void*)_tmp26E.elt_typ;_LL1D7:
if(!Cyc_Absyn_is_union_type(_tmp26F))flds=({struct Cyc_List_List*_tmp270=
_cycalloc(sizeof(*_tmp270));_tmp270->hd=_tmp260;_tmp270->tl=flds;_tmp270;});goto
_LL1D5;_LL1D8:;_LL1D9:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp271=_cycalloc(sizeof(*_tmp271));_tmp271[0]=({struct Cyc_Core_Impossible_struct
_tmp272;_tmp272.tag=Cyc_Core_Impossible;_tmp272.f1=({const char*_tmp273="anal_Rexp: AggrArrow ptr";
_tag_arr(_tmp273,sizeof(char),_get_zero_arr_size(_tmp273,25));});_tmp272;});
_tmp271;}));_LL1D5:;}_tmp261=_tmp25F;goto _LL1CE;_LL1CD: if(*((int*)_tmp253)!= 20)
goto _LL1CF;_tmp261=((struct Cyc_Absyn_Deref_e_struct*)_tmp253)->f1;_LL1CE: {void*
_tmp275;void*_tmp276;struct _tuple0 _tmp274=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp261);_tmp275=_tmp274.f1;_tmp276=_tmp274.f2;return Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp275,_tmp261,_tmp276,flds);}_LL1CF: if(*((int*)_tmp253)!= 23)goto
_LL1D1;_tmp262=((struct Cyc_Absyn_Subscript_e_struct*)_tmp253)->f1;_tmp263=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp253)->f2;_LL1D0: {void*_tmp277=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp262->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp278;struct Cyc_Absyn_PtrAtts _tmp279;struct Cyc_Absyn_Conref*_tmp27A;_LL1DB: if(
_tmp277 <= (void*)3?1:*((int*)_tmp277)!= 9)goto _LL1DD;_LL1DC: {struct _tagged_arr*
_tmp27B=Cyc_Absyn_fieldname((int)(Cyc_Evexp_eval_const_uint_exp(_tmp263)).f1);
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp262,({struct Cyc_List_List*
_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->hd=_tmp27B;_tmp27C->tl=flds;_tmp27C;}));}
_LL1DD: if(_tmp277 <= (void*)3?1:*((int*)_tmp277)!= 4)goto _LL1DF;_tmp278=((struct
Cyc_Absyn_PointerType_struct*)_tmp277)->f1;_tmp279=_tmp278.ptr_atts;_tmp27A=
_tmp279.bounds;_LL1DE: {void*_tmp27F;struct Cyc_List_List*_tmp280;struct _tuple5
_tmp27E=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({struct Cyc_Absyn_Exp*
_tmp27D[2];_tmp27D[1]=_tmp263;_tmp27D[0]=_tmp262;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp27D,sizeof(struct Cyc_Absyn_Exp*),
2));}));_tmp27F=_tmp27E.f1;_tmp280=_tmp27E.f2;{void*_tmp281=_tmp27F;{void*
_tmp282=_tmp27F;struct Cyc_Dict_Dict*_tmp283;struct Cyc_List_List*_tmp284;_LL1E2:
if(_tmp282 <= (void*)1?1:*((int*)_tmp282)!= 0)goto _LL1E4;_tmp283=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp282)->f1;_tmp284=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp282)->f2;
_LL1E3: if(Cyc_CfFlowInfo_initlevel(_tmp283,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp280))->tl))->hd)== (void*)0)({
void*_tmp285[0]={};Cyc_Tcutil_terr(_tmp263->loc,({const char*_tmp286="expression may not be initialized";
_tag_arr(_tmp286,sizeof(char),_get_zero_arr_size(_tmp286,34));}),_tag_arr(
_tmp285,sizeof(void*),0));});{struct Cyc_List_List*_tmp287=Cyc_NewControlFlow_add_subscript_reln(
_tmp284,_tmp262,_tmp263);if(_tmp284 != _tmp287)_tmp281=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp288=_cycalloc(sizeof(*_tmp288));_tmp288[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp289;_tmp289.tag=0;_tmp289.f1=_tmp283;_tmp289.f2=_tmp287;_tmp289;});_tmp288;});
goto _LL1E1;}_LL1E4:;_LL1E5: goto _LL1E1;_LL1E1:;}{void*_tmp28A=(void*)(Cyc_Absyn_compress_conref(
_tmp27A))->v;void*_tmp28B;_LL1E7: if(_tmp28A <= (void*)1?1:*((int*)_tmp28A)!= 0)
goto _LL1E9;_tmp28B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp28A)->f1;if(
_tmp28B <= (void*)1?1:*((int*)_tmp28B)!= 1)goto _LL1E9;_LL1E8:{void*_tmp28C=(void*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp280))->tl))->hd;
struct Cyc_List_List*_tmp28D;_LL1EC: if(_tmp28C <= (void*)3?1:*((int*)_tmp28C)!= 3)
goto _LL1EE;_tmp28D=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp28C)->f1;_LL1ED:(
void*)(_tmp263->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp28E=_cycalloc(sizeof(*_tmp28E));_tmp28E[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp28F;_tmp28F.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp28F.f1=_tmp28D;_tmp28F;});
_tmp28E;})));goto _LL1EB;_LL1EE:;_LL1EF: goto _LL1EB;_LL1EB:;}goto _LL1E6;_LL1E9:;
_LL1EA: goto _LL1E6;_LL1E6:;}{void*_tmp291;void*_tmp292;struct _tuple0 _tmp290=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp27F,_tmp262,(void*)((struct Cyc_List_List*)_check_null(_tmp280))->hd,
flds);_tmp291=_tmp290.f1;_tmp292=_tmp290.f2;return _tmp291 == (void*)0?({struct
_tuple0 _tmp293;_tmp293.f1=_tmp291;_tmp293.f2=_tmp292;_tmp293;}):({struct _tuple0
_tmp294;_tmp294.f1=_tmp281;_tmp294.f2=_tmp292;_tmp294;});}}}_LL1DF:;_LL1E0:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp295=_cycalloc(sizeof(*
_tmp295));_tmp295[0]=({struct Cyc_Core_Impossible_struct _tmp296;_tmp296.tag=Cyc_Core_Impossible;
_tmp296.f1=({const char*_tmp297="anal_Lexp: Subscript -- bad type";_tag_arr(
_tmp297,sizeof(char),_get_zero_arr_size(_tmp297,33));});_tmp296;});_tmp295;}));
_LL1DA:;}_LL1D1: if(*((int*)_tmp253)!= 21)goto _LL1D3;_tmp264=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp253)->f1;_tmp265=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp253)->f2;_LL1D2:
if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp264->topt))->v))
return({struct _tuple0 _tmp298;_tmp298.f1=inflow;_tmp298.f2=(void*)0;_tmp298;});
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp264,({struct Cyc_List_List*
_tmp299=_cycalloc(sizeof(*_tmp299));_tmp299->hd=_tmp265;_tmp299->tl=flds;_tmp299;}));
_LL1D3:;_LL1D4: return({struct _tuple0 _tmp29A;_tmp29A.f1=(void*)0;_tmp29A.f2=(void*)
0;_tmp29A;});_LL1BC:;}}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);}static struct _tuple0 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){{void*_tmp29B=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp29C;struct Cyc_Absyn_Exp*_tmp29D;struct Cyc_Absyn_Exp*_tmp29E;struct Cyc_Absyn_Exp*
_tmp29F;struct Cyc_Absyn_Exp*_tmp2A0;void*_tmp2A1;struct Cyc_List_List*_tmp2A2;
struct Cyc_List_List _tmp2A3;struct Cyc_Absyn_Exp*_tmp2A4;struct Cyc_List_List*
_tmp2A5;void*_tmp2A6;struct Cyc_List_List*_tmp2A7;_LL1F1: if(*((int*)_tmp29B)!= 6)
goto _LL1F3;_tmp29C=((struct Cyc_Absyn_Conditional_e_struct*)_tmp29B)->f1;_tmp29D=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp29B)->f2;_tmp29E=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp29B)->f3;_LL1F2: {void*_tmp2A9;void*_tmp2AA;struct _tuple0 _tmp2A8=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp29C);_tmp2A9=_tmp2A8.f1;_tmp2AA=_tmp2A8.f2;{void*_tmp2AC;void*
_tmp2AD;struct _tuple0 _tmp2AB=Cyc_NewControlFlow_anal_test(env,_tmp2A9,_tmp29D);
_tmp2AC=_tmp2AB.f1;_tmp2AD=_tmp2AB.f2;{void*_tmp2AF;void*_tmp2B0;struct _tuple0
_tmp2AE=Cyc_NewControlFlow_anal_test(env,_tmp2AA,_tmp29E);_tmp2AF=_tmp2AE.f1;
_tmp2B0=_tmp2AE.f2;return({struct _tuple0 _tmp2B1;_tmp2B1.f1=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp2AC,_tmp2AF);_tmp2B1.f2=Cyc_CfFlowInfo_join_flow(env->all_changed,
_tmp2AD,_tmp2B0);_tmp2B1;});}}}_LL1F3: if(*((int*)_tmp29B)!= 7)goto _LL1F5;_tmp29F=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp29B)->f1;_tmp2A0=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp29B)->f2;_LL1F4: return Cyc_NewControlFlow_anal_test(env,(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp29F)).f1,_tmp2A0);_LL1F5: if(*((int*)_tmp29B)!= 3)goto _LL1F7;
_tmp2A1=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp29B)->f1;if((int)_tmp2A1 != 
11)goto _LL1F7;_tmp2A2=((struct Cyc_Absyn_Primop_e_struct*)_tmp29B)->f2;if(_tmp2A2
== 0)goto _LL1F7;_tmp2A3=*_tmp2A2;_tmp2A4=(struct Cyc_Absyn_Exp*)_tmp2A3.hd;
_tmp2A5=_tmp2A3.tl;if(_tmp2A5 != 0)goto _LL1F7;_LL1F6: {void*_tmp2B3;void*_tmp2B4;
struct _tuple0 _tmp2B2=Cyc_NewControlFlow_anal_test(env,inflow,_tmp2A4);_tmp2B3=
_tmp2B2.f1;_tmp2B4=_tmp2B2.f2;return({struct _tuple0 _tmp2B5;_tmp2B5.f1=_tmp2B4;
_tmp2B5.f2=_tmp2B3;_tmp2B5;});}_LL1F7: if(*((int*)_tmp29B)!= 3)goto _LL1F9;_tmp2A6=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp29B)->f1;_tmp2A7=((struct Cyc_Absyn_Primop_e_struct*)
_tmp29B)->f2;_LL1F8: {void*_tmp2B7;struct Cyc_List_List*_tmp2B8;struct _tuple5
_tmp2B6=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp2A7);_tmp2B7=
_tmp2B6.f1;_tmp2B8=_tmp2B6.f2;{void*_tmp2B9=_tmp2B7;struct Cyc_Dict_Dict*_tmp2BA;
struct Cyc_List_List*_tmp2BB;_LL1FC: if((int)_tmp2B9 != 0)goto _LL1FE;_LL1FD: return({
struct _tuple0 _tmp2BC;_tmp2BC.f1=_tmp2B7;_tmp2BC.f2=_tmp2B7;_tmp2BC;});_LL1FE: if(
_tmp2B9 <= (void*)1?1:*((int*)_tmp2B9)!= 0)goto _LL1FB;_tmp2BA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2B9)->f1;_tmp2BB=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2B9)->f2;
_LL1FF: {struct Cyc_Absyn_Exp*_tmp2BD=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2A7))->hd;struct Cyc_Absyn_Exp*_tmp2BE=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2A7->tl))->hd;void*_tmp2BF=(void*)((struct
Cyc_List_List*)_check_null(_tmp2B8))->hd;void*_tmp2C0=(void*)((struct Cyc_List_List*)
_check_null(_tmp2B8->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp2BA,_tmp2BF)== (
void*)0)({void*_tmp2C1[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp2A7->hd)->loc,({
const char*_tmp2C2="expression may not be initialized";_tag_arr(_tmp2C2,sizeof(
char),_get_zero_arr_size(_tmp2C2,34));}),_tag_arr(_tmp2C1,sizeof(void*),0));});
if(Cyc_CfFlowInfo_initlevel(_tmp2BA,_tmp2C0)== (void*)0)({void*_tmp2C3[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2A7->tl))->hd)->loc,({
const char*_tmp2C4="expression may not be initialized";_tag_arr(_tmp2C4,sizeof(
char),_get_zero_arr_size(_tmp2C4,34));}),_tag_arr(_tmp2C3,sizeof(void*),0));});
if(_tmp2A6 == (void*)5?1: _tmp2A6 == (void*)6){struct _tuple0 _tmp2C6=({struct _tuple0
_tmp2C5;_tmp2C5.f1=_tmp2BF;_tmp2C5.f2=_tmp2C0;_tmp2C5;});void*_tmp2C7;void*
_tmp2C8;void*_tmp2C9;void*_tmp2CA;void*_tmp2CB;void*_tmp2CC;void*_tmp2CD;void*
_tmp2CE;void*_tmp2CF;void*_tmp2D0;void*_tmp2D1;void*_tmp2D2;void*_tmp2D3;void*
_tmp2D4;void*_tmp2D5;void*_tmp2D6;void*_tmp2D7;void*_tmp2D8;void*_tmp2D9;void*
_tmp2DA;_LL201: _tmp2C7=_tmp2C6.f1;if(_tmp2C7 <= (void*)3?1:*((int*)_tmp2C7)!= 0)
goto _LL203;_tmp2C8=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp2C7)->f1;
_tmp2C9=_tmp2C6.f2;if((int)_tmp2C9 != 0)goto _LL203;_LL202: {void*_tmp2DC;void*
_tmp2DD;struct _tuple0 _tmp2DB=Cyc_NewControlFlow_splitzero(env,_tmp2B7,_tmp2B7,
_tmp2BD,_tmp2C8);_tmp2DC=_tmp2DB.f1;_tmp2DD=_tmp2DB.f2;{void*_tmp2DE=_tmp2A6;
_LL216: if((int)_tmp2DE != 5)goto _LL218;_LL217: return({struct _tuple0 _tmp2DF;
_tmp2DF.f1=_tmp2DD;_tmp2DF.f2=_tmp2DC;_tmp2DF;});_LL218: if((int)_tmp2DE != 6)goto
_LL21A;_LL219: return({struct _tuple0 _tmp2E0;_tmp2E0.f1=_tmp2DC;_tmp2E0.f2=_tmp2DD;
_tmp2E0;});_LL21A:;_LL21B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2E1=_cycalloc(sizeof(*_tmp2E1));_tmp2E1[0]=({struct Cyc_Core_Impossible_struct
_tmp2E2;_tmp2E2.tag=Cyc_Core_Impossible;_tmp2E2.f1=({const char*_tmp2E3="anal_test, zero-split";
_tag_arr(_tmp2E3,sizeof(char),_get_zero_arr_size(_tmp2E3,22));});_tmp2E2;});
_tmp2E1;}));_LL215:;}}_LL203: _tmp2CA=_tmp2C6.f1;if((int)_tmp2CA != 0)goto _LL205;
_tmp2CB=_tmp2C6.f2;if(_tmp2CB <= (void*)3?1:*((int*)_tmp2CB)!= 0)goto _LL205;
_tmp2CC=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp2CB)->f1;_LL204: {
void*_tmp2E5;void*_tmp2E6;struct _tuple0 _tmp2E4=Cyc_NewControlFlow_splitzero(env,
_tmp2B7,_tmp2B7,_tmp2BE,_tmp2CC);_tmp2E5=_tmp2E4.f1;_tmp2E6=_tmp2E4.f2;{void*
_tmp2E7=_tmp2A6;_LL21D: if((int)_tmp2E7 != 5)goto _LL21F;_LL21E: return({struct
_tuple0 _tmp2E8;_tmp2E8.f1=_tmp2E6;_tmp2E8.f2=_tmp2E5;_tmp2E8;});_LL21F: if((int)
_tmp2E7 != 6)goto _LL221;_LL220: return({struct _tuple0 _tmp2E9;_tmp2E9.f1=_tmp2E5;
_tmp2E9.f2=_tmp2E6;_tmp2E9;});_LL221:;_LL222:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA[0]=({struct Cyc_Core_Impossible_struct
_tmp2EB;_tmp2EB.tag=Cyc_Core_Impossible;_tmp2EB.f1=({const char*_tmp2EC="anal_test, zero-split";
_tag_arr(_tmp2EC,sizeof(char),_get_zero_arr_size(_tmp2EC,22));});_tmp2EB;});
_tmp2EA;}));_LL21C:;}}_LL205: _tmp2CD=_tmp2C6.f1;if((int)_tmp2CD != 0)goto _LL207;
_tmp2CE=_tmp2C6.f2;if((int)_tmp2CE != 0)goto _LL207;_LL206: if(_tmp2A6 == (void*)5)
return({struct _tuple0 _tmp2ED;_tmp2ED.f1=_tmp2B7;_tmp2ED.f2=(void*)0;_tmp2ED;});
else{return({struct _tuple0 _tmp2EE;_tmp2EE.f1=(void*)0;_tmp2EE.f2=_tmp2B7;_tmp2EE;});}
_LL207: _tmp2CF=_tmp2C6.f1;if((int)_tmp2CF != 0)goto _LL209;_tmp2D0=_tmp2C6.f2;if((
int)_tmp2D0 != 1)goto _LL209;_LL208: goto _LL20A;_LL209: _tmp2D1=_tmp2C6.f1;if((int)
_tmp2D1 != 0)goto _LL20B;_tmp2D2=_tmp2C6.f2;if((int)_tmp2D2 != 2)goto _LL20B;_LL20A:
goto _LL20C;_LL20B: _tmp2D3=_tmp2C6.f1;if((int)_tmp2D3 != 0)goto _LL20D;_tmp2D4=
_tmp2C6.f2;if(_tmp2D4 <= (void*)3?1:*((int*)_tmp2D4)!= 2)goto _LL20D;_LL20C: goto
_LL20E;_LL20D: _tmp2D5=_tmp2C6.f1;if((int)_tmp2D5 != 1)goto _LL20F;_tmp2D6=_tmp2C6.f2;
if((int)_tmp2D6 != 0)goto _LL20F;_LL20E: goto _LL210;_LL20F: _tmp2D7=_tmp2C6.f1;if((
int)_tmp2D7 != 2)goto _LL211;_tmp2D8=_tmp2C6.f2;if((int)_tmp2D8 != 0)goto _LL211;
_LL210: goto _LL212;_LL211: _tmp2D9=_tmp2C6.f1;if(_tmp2D9 <= (void*)3?1:*((int*)
_tmp2D9)!= 2)goto _LL213;_tmp2DA=_tmp2C6.f2;if((int)_tmp2DA != 0)goto _LL213;_LL212:
if(_tmp2A6 == (void*)6)return({struct _tuple0 _tmp2EF;_tmp2EF.f1=_tmp2B7;_tmp2EF.f2=(
void*)0;_tmp2EF;});else{return({struct _tuple0 _tmp2F0;_tmp2F0.f1=(void*)0;_tmp2F0.f2=
_tmp2B7;_tmp2F0;});}_LL213:;_LL214: goto _LL200;_LL200:;}{struct _tuple0 _tmp2F2=({
struct _tuple0 _tmp2F1;_tmp2F1.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2BD->topt))->v);_tmp2F1.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp2BE->topt))->v);_tmp2F1;});void*_tmp2F3;void*
_tmp2F4;void*_tmp2F5;void*_tmp2F6;void*_tmp2F7;void*_tmp2F8;_LL224: _tmp2F3=
_tmp2F2.f1;if(_tmp2F3 <= (void*)3?1:*((int*)_tmp2F3)!= 5)goto _LL226;_tmp2F4=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp2F3)->f1;if((int)_tmp2F4 != 1)goto _LL226;
_LL225: goto _LL227;_LL226: _tmp2F5=_tmp2F2.f2;if(_tmp2F5 <= (void*)3?1:*((int*)
_tmp2F5)!= 5)goto _LL228;_tmp2F6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2F5)->f1;
if((int)_tmp2F6 != 1)goto _LL228;_LL227: goto _LL229;_LL228: _tmp2F7=_tmp2F2.f1;if(
_tmp2F7 <= (void*)3?1:*((int*)_tmp2F7)!= 17)goto _LL22A;_LL229: goto _LL22B;_LL22A:
_tmp2F8=_tmp2F2.f2;if(_tmp2F8 <= (void*)3?1:*((int*)_tmp2F8)!= 17)goto _LL22C;
_LL22B: goto _LL223;_LL22C:;_LL22D: return({struct _tuple0 _tmp2F9;_tmp2F9.f1=_tmp2B7;
_tmp2F9.f2=_tmp2B7;_tmp2F9;});_LL223:;}{void*_tmp2FA=_tmp2A6;_LL22F: if((int)
_tmp2FA != 5)goto _LL231;_LL230: {void*_tmp2FB=Cyc_NewControlFlow_if_tagcmp(env,
_tmp2B7,_tmp2BD,_tmp2BF,(void*)5,_tmp2C0);_tmp2FB=Cyc_NewControlFlow_if_tagcmp(
env,_tmp2FB,_tmp2BE,_tmp2C0,(void*)5,_tmp2BF);return({struct _tuple0 _tmp2FC;
_tmp2FC.f1=_tmp2FB;_tmp2FC.f2=_tmp2B7;_tmp2FC;});}_LL231: if((int)_tmp2FA != 6)
goto _LL233;_LL232: {void*_tmp2FD=Cyc_NewControlFlow_if_tagcmp(env,_tmp2B7,
_tmp2BD,_tmp2BF,(void*)5,_tmp2C0);_tmp2FD=Cyc_NewControlFlow_if_tagcmp(env,
_tmp2FD,_tmp2BE,_tmp2C0,(void*)5,_tmp2BF);return({struct _tuple0 _tmp2FE;_tmp2FE.f1=
_tmp2B7;_tmp2FE.f2=_tmp2FD;_tmp2FE;});}_LL233: if((int)_tmp2FA != 7)goto _LL235;
_LL234: {void*_tmp2FF=Cyc_NewControlFlow_if_tagcmp(env,_tmp2B7,_tmp2BE,_tmp2C0,(
void*)8,_tmp2BF);void*_tmp300=Cyc_NewControlFlow_if_tagcmp(env,_tmp2B7,_tmp2BD,
_tmp2BF,(void*)10,_tmp2C0);return({struct _tuple0 _tmp301;_tmp301.f1=_tmp2FF;
_tmp301.f2=_tmp300;_tmp301;});}_LL235: if((int)_tmp2FA != 9)goto _LL237;_LL236: {
void*_tmp302=Cyc_NewControlFlow_if_tagcmp(env,_tmp2B7,_tmp2BE,_tmp2C0,(void*)10,
_tmp2BF);void*_tmp303=Cyc_NewControlFlow_if_tagcmp(env,_tmp2B7,_tmp2BD,_tmp2BF,(
void*)8,_tmp2C0);return({struct _tuple0 _tmp304;_tmp304.f1=_tmp302;_tmp304.f2=
_tmp303;_tmp304;});}_LL237: if((int)_tmp2FA != 8)goto _LL239;_LL238: {void*_tmp305=
Cyc_NewControlFlow_if_tagcmp(env,_tmp2B7,_tmp2BD,_tmp2BF,(void*)8,_tmp2C0);void*
_tmp306=Cyc_NewControlFlow_if_tagcmp(env,_tmp2B7,_tmp2BE,_tmp2C0,(void*)10,
_tmp2BF);{void*_tmp307=_tmp305;struct Cyc_Dict_Dict*_tmp308;_LL23E: if((int)
_tmp307 != 0)goto _LL240;_LL23F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp309=_cycalloc(sizeof(*_tmp309));_tmp309[0]=({struct Cyc_Core_Impossible_struct
_tmp30A;_tmp30A.tag=Cyc_Core_Impossible;_tmp30A.f1=({const char*_tmp30B="anal_test, Lt";
_tag_arr(_tmp30B,sizeof(char),_get_zero_arr_size(_tmp30B,14));});_tmp30A;});
_tmp309;}));_LL240: if(_tmp307 <= (void*)1?1:*((int*)_tmp307)!= 0)goto _LL23D;
_tmp308=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp307)->f1;_LL241: _tmp2BA=
_tmp308;_LL23D:;}{void*_tmp30C=(void*)_tmp2BD->r;void*_tmp30D;struct Cyc_Absyn_Vardecl*
_tmp30E;void*_tmp30F;struct Cyc_Absyn_Vardecl*_tmp310;void*_tmp311;struct Cyc_Absyn_Vardecl*
_tmp312;void*_tmp313;struct Cyc_Absyn_Vardecl*_tmp314;_LL243: if(*((int*)_tmp30C)
!= 1)goto _LL245;_tmp30D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp30C)->f2;if(
_tmp30D <= (void*)1?1:*((int*)_tmp30D)!= 0)goto _LL245;_tmp30E=((struct Cyc_Absyn_Global_b_struct*)
_tmp30D)->f1;if(!(!_tmp30E->escapes))goto _LL245;_LL244: _tmp310=_tmp30E;goto
_LL246;_LL245: if(*((int*)_tmp30C)!= 1)goto _LL247;_tmp30F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp30C)->f2;if(_tmp30F <= (void*)1?1:*((int*)_tmp30F)!= 3)goto _LL247;_tmp310=((
struct Cyc_Absyn_Local_b_struct*)_tmp30F)->f1;if(!(!_tmp310->escapes))goto _LL247;
_LL246: _tmp312=_tmp310;goto _LL248;_LL247: if(*((int*)_tmp30C)!= 1)goto _LL249;
_tmp311=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp30C)->f2;if(_tmp311 <= (void*)
1?1:*((int*)_tmp311)!= 4)goto _LL249;_tmp312=((struct Cyc_Absyn_Pat_b_struct*)
_tmp311)->f1;if(!(!_tmp312->escapes))goto _LL249;_LL248: _tmp314=_tmp312;goto
_LL24A;_LL249: if(*((int*)_tmp30C)!= 1)goto _LL24B;_tmp313=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp30C)->f2;if(_tmp313 <= (void*)1?1:*((int*)_tmp313)!= 2)goto _LL24B;_tmp314=((
struct Cyc_Absyn_Param_b_struct*)_tmp313)->f1;if(!(!_tmp314->escapes))goto _LL24B;
_LL24A: {void*_tmp315=(void*)_tmp2BE->r;void*_tmp316;struct Cyc_Absyn_Vardecl*
_tmp317;void*_tmp318;struct Cyc_Absyn_Vardecl*_tmp319;void*_tmp31A;struct Cyc_Absyn_Vardecl*
_tmp31B;void*_tmp31C;struct Cyc_Absyn_Vardecl*_tmp31D;void*_tmp31E;int _tmp31F;
void*_tmp320;struct Cyc_List_List*_tmp321;struct Cyc_List_List _tmp322;struct Cyc_Absyn_Exp*
_tmp323;_LL24E: if(*((int*)_tmp315)!= 1)goto _LL250;_tmp316=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp315)->f2;if(_tmp316 <= (void*)1?1:*((int*)_tmp316)!= 0)goto _LL250;_tmp317=((
struct Cyc_Absyn_Global_b_struct*)_tmp316)->f1;if(!(!_tmp317->escapes))goto _LL250;
_LL24F: _tmp319=_tmp317;goto _LL251;_LL250: if(*((int*)_tmp315)!= 1)goto _LL252;
_tmp318=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp315)->f2;if(_tmp318 <= (void*)
1?1:*((int*)_tmp318)!= 3)goto _LL252;_tmp319=((struct Cyc_Absyn_Local_b_struct*)
_tmp318)->f1;if(!(!_tmp319->escapes))goto _LL252;_LL251: _tmp31B=_tmp319;goto
_LL253;_LL252: if(*((int*)_tmp315)!= 1)goto _LL254;_tmp31A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp315)->f2;if(_tmp31A <= (void*)1?1:*((int*)_tmp31A)!= 4)goto _LL254;_tmp31B=((
struct Cyc_Absyn_Pat_b_struct*)_tmp31A)->f1;if(!(!_tmp31B->escapes))goto _LL254;
_LL253: _tmp31D=_tmp31B;goto _LL255;_LL254: if(*((int*)_tmp315)!= 1)goto _LL256;
_tmp31C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp315)->f2;if(_tmp31C <= (void*)
1?1:*((int*)_tmp31C)!= 2)goto _LL256;_tmp31D=((struct Cyc_Absyn_Param_b_struct*)
_tmp31C)->f1;if(!(!_tmp31D->escapes))goto _LL256;_LL255: {struct Cyc_List_List*
_tmp324=({struct Cyc_List_List*_tmp328=_cycalloc(sizeof(*_tmp328));_tmp328->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp329=_cycalloc(sizeof(*_tmp329));_tmp329->vd=
_tmp314;_tmp329->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp32A=_cycalloc(sizeof(*_tmp32A));_tmp32A[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp32B;_tmp32B.tag=1;_tmp32B.f1=_tmp31D;_tmp32B;});_tmp32A;}));_tmp329;});
_tmp328->tl=_tmp2BB;_tmp328;});return({struct _tuple0 _tmp325;_tmp325.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp326=_cycalloc(sizeof(*_tmp326));
_tmp326[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp327;_tmp327.tag=0;
_tmp327.f1=_tmp2BA;_tmp327.f2=_tmp324;_tmp327;});_tmp326;});_tmp325.f2=_tmp306;
_tmp325;});}_LL256: if(*((int*)_tmp315)!= 0)goto _LL258;_tmp31E=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp315)->f1;if(_tmp31E <= (void*)1?1:*((int*)_tmp31E)!= 2)goto _LL258;_tmp31F=((
struct Cyc_Absyn_Int_c_struct*)_tmp31E)->f2;_LL257: {struct Cyc_List_List*_tmp32C=({
struct Cyc_List_List*_tmp330=_cycalloc(sizeof(*_tmp330));_tmp330->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp331=_cycalloc(sizeof(*_tmp331));_tmp331->vd=_tmp314;_tmp331->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessConst_struct*_tmp332=_cycalloc_atomic(sizeof(*
_tmp332));_tmp332[0]=({struct Cyc_CfFlowInfo_LessConst_struct _tmp333;_tmp333.tag=
3;_tmp333.f1=(unsigned int)_tmp31F;_tmp333;});_tmp332;}));_tmp331;});_tmp330->tl=
_tmp2BB;_tmp330;});return({struct _tuple0 _tmp32D;_tmp32D.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp32F;_tmp32F.tag=0;_tmp32F.f1=_tmp2BA;_tmp32F.f2=_tmp32C;_tmp32F;});_tmp32E;});
_tmp32D.f2=_tmp306;_tmp32D;});}_LL258: if(*((int*)_tmp315)!= 3)goto _LL25A;_tmp320=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp315)->f1;if((int)_tmp320 != 19)goto
_LL25A;_tmp321=((struct Cyc_Absyn_Primop_e_struct*)_tmp315)->f2;if(_tmp321 == 0)
goto _LL25A;_tmp322=*_tmp321;_tmp323=(struct Cyc_Absyn_Exp*)_tmp322.hd;_LL259: {
void*_tmp334=(void*)_tmp323->r;void*_tmp335;struct Cyc_Absyn_Vardecl*_tmp336;void*
_tmp337;struct Cyc_Absyn_Vardecl*_tmp338;void*_tmp339;struct Cyc_Absyn_Vardecl*
_tmp33A;void*_tmp33B;struct Cyc_Absyn_Vardecl*_tmp33C;_LL25D: if(*((int*)_tmp334)
!= 1)goto _LL25F;_tmp335=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp334)->f2;if(
_tmp335 <= (void*)1?1:*((int*)_tmp335)!= 0)goto _LL25F;_tmp336=((struct Cyc_Absyn_Global_b_struct*)
_tmp335)->f1;if(!(!_tmp336->escapes))goto _LL25F;_LL25E: _tmp338=_tmp336;goto
_LL260;_LL25F: if(*((int*)_tmp334)!= 1)goto _LL261;_tmp337=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp334)->f2;if(_tmp337 <= (void*)1?1:*((int*)_tmp337)!= 3)goto _LL261;_tmp338=((
struct Cyc_Absyn_Local_b_struct*)_tmp337)->f1;if(!(!_tmp338->escapes))goto _LL261;
_LL260: _tmp33A=_tmp338;goto _LL262;_LL261: if(*((int*)_tmp334)!= 1)goto _LL263;
_tmp339=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp334)->f2;if(_tmp339 <= (void*)
1?1:*((int*)_tmp339)!= 4)goto _LL263;_tmp33A=((struct Cyc_Absyn_Pat_b_struct*)
_tmp339)->f1;if(!(!_tmp33A->escapes))goto _LL263;_LL262: _tmp33C=_tmp33A;goto
_LL264;_LL263: if(*((int*)_tmp334)!= 1)goto _LL265;_tmp33B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp334)->f2;if(_tmp33B <= (void*)1?1:*((int*)_tmp33B)!= 2)goto _LL265;_tmp33C=((
struct Cyc_Absyn_Param_b_struct*)_tmp33B)->f1;if(!(!_tmp33C->escapes))goto _LL265;
_LL264: {struct Cyc_List_List*_tmp33D=({struct Cyc_List_List*_tmp341=_cycalloc(
sizeof(*_tmp341));_tmp341->hd=({struct Cyc_CfFlowInfo_Reln*_tmp342=_cycalloc(
sizeof(*_tmp342));_tmp342->vd=_tmp314;_tmp342->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp343=_cycalloc(sizeof(*_tmp343));_tmp343[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp344;_tmp344.tag=2;_tmp344.f1=_tmp33C;_tmp344;});_tmp343;}));_tmp342;});
_tmp341->tl=_tmp2BB;_tmp341;});return({struct _tuple0 _tmp33E;_tmp33E.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp33F=_cycalloc(sizeof(*_tmp33F));
_tmp33F[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp340;_tmp340.tag=0;
_tmp340.f1=_tmp2BA;_tmp340.f2=_tmp33D;_tmp340;});_tmp33F;});_tmp33E.f2=_tmp306;
_tmp33E;});}_LL265:;_LL266: return({struct _tuple0 _tmp345;_tmp345.f1=_tmp305;
_tmp345.f2=_tmp306;_tmp345;});_LL25C:;}_LL25A:;_LL25B: return({struct _tuple0
_tmp346;_tmp346.f1=_tmp305;_tmp346.f2=_tmp306;_tmp346;});_LL24D:;}_LL24B:;_LL24C:
return({struct _tuple0 _tmp347;_tmp347.f1=_tmp305;_tmp347.f2=_tmp306;_tmp347;});
_LL242:;}}_LL239: if((int)_tmp2FA != 10)goto _LL23B;_LL23A: {void*_tmp348=Cyc_NewControlFlow_if_tagcmp(
env,_tmp2B7,_tmp2BD,_tmp2BF,(void*)10,_tmp2C0);void*_tmp349=Cyc_NewControlFlow_if_tagcmp(
env,_tmp2B7,_tmp2BE,_tmp2BF,(void*)8,_tmp2BF);{void*_tmp34A=_tmp348;struct Cyc_Dict_Dict*
_tmp34B;_LL268: if((int)_tmp34A != 0)goto _LL26A;_LL269:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C[0]=({
struct Cyc_Core_Impossible_struct _tmp34D;_tmp34D.tag=Cyc_Core_Impossible;_tmp34D.f1=({
const char*_tmp34E="anal_test, Lte";_tag_arr(_tmp34E,sizeof(char),
_get_zero_arr_size(_tmp34E,15));});_tmp34D;});_tmp34C;}));_LL26A: if(_tmp34A <= (
void*)1?1:*((int*)_tmp34A)!= 0)goto _LL267;_tmp34B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp34A)->f1;_LL26B: _tmp2BA=_tmp34B;_LL267:;}{void*_tmp34F=(void*)_tmp2BD->r;
void*_tmp350;struct Cyc_Absyn_Vardecl*_tmp351;void*_tmp352;struct Cyc_Absyn_Vardecl*
_tmp353;void*_tmp354;struct Cyc_Absyn_Vardecl*_tmp355;void*_tmp356;struct Cyc_Absyn_Vardecl*
_tmp357;_LL26D: if(*((int*)_tmp34F)!= 1)goto _LL26F;_tmp350=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp34F)->f2;if(_tmp350 <= (void*)1?1:*((int*)_tmp350)!= 0)goto _LL26F;_tmp351=((
struct Cyc_Absyn_Global_b_struct*)_tmp350)->f1;if(!(!_tmp351->escapes))goto _LL26F;
_LL26E: _tmp353=_tmp351;goto _LL270;_LL26F: if(*((int*)_tmp34F)!= 1)goto _LL271;
_tmp352=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp34F)->f2;if(_tmp352 <= (void*)
1?1:*((int*)_tmp352)!= 3)goto _LL271;_tmp353=((struct Cyc_Absyn_Local_b_struct*)
_tmp352)->f1;if(!(!_tmp353->escapes))goto _LL271;_LL270: _tmp355=_tmp353;goto
_LL272;_LL271: if(*((int*)_tmp34F)!= 1)goto _LL273;_tmp354=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp34F)->f2;if(_tmp354 <= (void*)1?1:*((int*)_tmp354)!= 4)goto _LL273;_tmp355=((
struct Cyc_Absyn_Pat_b_struct*)_tmp354)->f1;if(!(!_tmp355->escapes))goto _LL273;
_LL272: _tmp357=_tmp355;goto _LL274;_LL273: if(*((int*)_tmp34F)!= 1)goto _LL275;
_tmp356=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp34F)->f2;if(_tmp356 <= (void*)
1?1:*((int*)_tmp356)!= 2)goto _LL275;_tmp357=((struct Cyc_Absyn_Param_b_struct*)
_tmp356)->f1;if(!(!_tmp357->escapes))goto _LL275;_LL274: {void*_tmp358=(void*)
_tmp2BE->r;void*_tmp359;struct Cyc_List_List*_tmp35A;struct Cyc_List_List _tmp35B;
struct Cyc_Absyn_Exp*_tmp35C;_LL278: if(*((int*)_tmp358)!= 3)goto _LL27A;_tmp359=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp358)->f1;if((int)_tmp359 != 19)goto
_LL27A;_tmp35A=((struct Cyc_Absyn_Primop_e_struct*)_tmp358)->f2;if(_tmp35A == 0)
goto _LL27A;_tmp35B=*_tmp35A;_tmp35C=(struct Cyc_Absyn_Exp*)_tmp35B.hd;_LL279: {
void*_tmp35D=(void*)_tmp35C->r;void*_tmp35E;struct Cyc_Absyn_Vardecl*_tmp35F;void*
_tmp360;struct Cyc_Absyn_Vardecl*_tmp361;void*_tmp362;struct Cyc_Absyn_Vardecl*
_tmp363;void*_tmp364;struct Cyc_Absyn_Vardecl*_tmp365;_LL27D: if(*((int*)_tmp35D)
!= 1)goto _LL27F;_tmp35E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp35D)->f2;if(
_tmp35E <= (void*)1?1:*((int*)_tmp35E)!= 0)goto _LL27F;_tmp35F=((struct Cyc_Absyn_Global_b_struct*)
_tmp35E)->f1;if(!(!_tmp35F->escapes))goto _LL27F;_LL27E: _tmp361=_tmp35F;goto
_LL280;_LL27F: if(*((int*)_tmp35D)!= 1)goto _LL281;_tmp360=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp35D)->f2;if(_tmp360 <= (void*)1?1:*((int*)_tmp360)!= 3)goto _LL281;_tmp361=((
struct Cyc_Absyn_Local_b_struct*)_tmp360)->f1;if(!(!_tmp361->escapes))goto _LL281;
_LL280: _tmp363=_tmp361;goto _LL282;_LL281: if(*((int*)_tmp35D)!= 1)goto _LL283;
_tmp362=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp35D)->f2;if(_tmp362 <= (void*)
1?1:*((int*)_tmp362)!= 4)goto _LL283;_tmp363=((struct Cyc_Absyn_Pat_b_struct*)
_tmp362)->f1;if(!(!_tmp363->escapes))goto _LL283;_LL282: _tmp365=_tmp363;goto
_LL284;_LL283: if(*((int*)_tmp35D)!= 1)goto _LL285;_tmp364=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp35D)->f2;if(_tmp364 <= (void*)1?1:*((int*)_tmp364)!= 2)goto _LL285;_tmp365=((
struct Cyc_Absyn_Param_b_struct*)_tmp364)->f1;if(!(!_tmp365->escapes))goto _LL285;
_LL284: {struct Cyc_List_List*_tmp366=({struct Cyc_List_List*_tmp36A=_cycalloc(
sizeof(*_tmp36A));_tmp36A->hd=({struct Cyc_CfFlowInfo_Reln*_tmp36B=_cycalloc(
sizeof(*_tmp36B));_tmp36B->vd=_tmp357;_tmp36B->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp36D;_tmp36D.tag=4;_tmp36D.f1=_tmp365;_tmp36D;});_tmp36C;}));_tmp36B;});
_tmp36A->tl=_tmp2BB;_tmp36A;});return({struct _tuple0 _tmp367;_tmp367.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp368=_cycalloc(sizeof(*_tmp368));
_tmp368[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp369;_tmp369.tag=0;
_tmp369.f1=_tmp2BA;_tmp369.f2=_tmp366;_tmp369;});_tmp368;});_tmp367.f2=_tmp349;
_tmp367;});}_LL285:;_LL286: return({struct _tuple0 _tmp36E;_tmp36E.f1=_tmp348;
_tmp36E.f2=_tmp349;_tmp36E;});_LL27C:;}_LL27A:;_LL27B: return({struct _tuple0
_tmp36F;_tmp36F.f1=_tmp348;_tmp36F.f2=_tmp349;_tmp36F;});_LL277:;}_LL275:;_LL276:
return({struct _tuple0 _tmp370;_tmp370.f1=_tmp348;_tmp370.f2=_tmp349;_tmp370;});
_LL26C:;}}_LL23B:;_LL23C: return({struct _tuple0 _tmp371;_tmp371.f1=_tmp2B7;_tmp371.f2=
_tmp2B7;_tmp371;});_LL22E:;}}_LL1FB:;}}_LL1F9:;_LL1FA: goto _LL1F0;_LL1F0:;}{void*
_tmp373;void*_tmp374;struct _tuple0 _tmp372=Cyc_NewControlFlow_anal_Rexp(env,
inflow,e);_tmp373=_tmp372.f1;_tmp374=_tmp372.f2;{void*_tmp375=_tmp373;struct Cyc_Dict_Dict*
_tmp376;struct Cyc_List_List*_tmp377;_LL288: if((int)_tmp375 != 0)goto _LL28A;_LL289:
return({struct _tuple0 _tmp378;_tmp378.f1=_tmp373;_tmp378.f2=_tmp373;_tmp378;});
_LL28A: if(_tmp375 <= (void*)1?1:*((int*)_tmp375)!= 0)goto _LL287;_tmp376=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp375)->f1;_tmp377=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp375)->f2;_LL28B: {void*_tmp379=_tmp374;void*_tmp37A;void*_tmp37B;void*
_tmp37C;_LL28D: if((int)_tmp379 != 0)goto _LL28F;_LL28E: return({struct _tuple0
_tmp37D;_tmp37D.f1=(void*)0;_tmp37D.f2=_tmp373;_tmp37D;});_LL28F: if((int)_tmp379
!= 2)goto _LL291;_LL290: goto _LL292;_LL291: if((int)_tmp379 != 1)goto _LL293;_LL292:
goto _LL294;_LL293: if(_tmp379 <= (void*)3?1:*((int*)_tmp379)!= 2)goto _LL295;_LL294:
return({struct _tuple0 _tmp37E;_tmp37E.f1=_tmp373;_tmp37E.f2=(void*)0;_tmp37E;});
_LL295: if(_tmp379 <= (void*)3?1:*((int*)_tmp379)!= 0)goto _LL297;_tmp37A=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp379)->f1;if((int)_tmp37A != 0)goto
_LL297;_LL296: goto _LL298;_LL297: if(_tmp379 <= (void*)3?1:*((int*)_tmp379)!= 1)
goto _LL299;_tmp37B=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp379)->f1;if((
int)_tmp37B != 0)goto _LL299;_LL298:({void*_tmp37F[0]={};Cyc_Tcutil_terr(e->loc,({
const char*_tmp380="expression may not be initialized";_tag_arr(_tmp380,sizeof(
char),_get_zero_arr_size(_tmp380,34));}),_tag_arr(_tmp37F,sizeof(void*),0));});
return({struct _tuple0 _tmp381;_tmp381.f1=(void*)0;_tmp381.f2=(void*)0;_tmp381;});
_LL299: if(_tmp379 <= (void*)3?1:*((int*)_tmp379)!= 0)goto _LL29B;_tmp37C=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp379)->f1;_LL29A: return Cyc_NewControlFlow_splitzero(
env,inflow,_tmp373,e,_tmp37C);_LL29B: if(_tmp379 <= (void*)3?1:*((int*)_tmp379)!= 
1)goto _LL29D;_LL29C: return({struct _tuple0 _tmp382;_tmp382.f1=_tmp373;_tmp382.f2=
_tmp373;_tmp382;});_LL29D: if(_tmp379 <= (void*)3?1:*((int*)_tmp379)!= 3)goto
_LL29F;_LL29E: return({struct _tuple0 _tmp383;_tmp383.f1=_tmp373;_tmp383.f2=_tmp373;
_tmp383;});_LL29F: if(_tmp379 <= (void*)3?1:*((int*)_tmp379)!= 4)goto _LL28C;_LL2A0:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp384=_cycalloc(sizeof(*
_tmp384));_tmp384[0]=({struct Cyc_Core_Impossible_struct _tmp385;_tmp385.tag=Cyc_Core_Impossible;
_tmp385.f1=({const char*_tmp386="anal_test";_tag_arr(_tmp386,sizeof(char),
_get_zero_arr_size(_tmp386,10));});_tmp385;});_tmp384;}));_LL28C:;}_LL287:;}}}
static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*loc,
struct Cyc_NewControlFlow_AnalEnv*env,void*flow){void*_tmp387=flow;struct Cyc_Dict_Dict*
_tmp388;_LL2A2: if((int)_tmp387 != 0)goto _LL2A4;_LL2A3: return;_LL2A4: if(_tmp387 <= (
void*)1?1:*((int*)_tmp387)!= 0)goto _LL2A1;_tmp388=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp387)->f1;_LL2A5:{struct Cyc_List_List*_tmp389=env->param_roots;for(0;_tmp389
!= 0;_tmp389=_tmp389->tl){if(Cyc_CfFlowInfo_initlevel(_tmp388,Cyc_CfFlowInfo_lookup_place(
_tmp388,(struct Cyc_CfFlowInfo_Place*)_tmp389->hd))!= (void*)2)({void*_tmp38A[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp38B="function may not initialize all the parameters with attribute 'initializes'";
_tag_arr(_tmp38B,sizeof(char),_get_zero_arr_size(_tmp38B,76));}),_tag_arr(
_tmp38A,sizeof(void*),0));});}}return;_LL2A1:;}static void*Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_List_List*scs){for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp38D;struct Cyc_Core_Opt*
_tmp38E;struct Cyc_Absyn_Exp*_tmp38F;struct Cyc_Absyn_Stmt*_tmp390;struct Cyc_Absyn_Switch_clause*
_tmp38C=(struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp38D=*_tmp38C;_tmp38E=_tmp38D.pat_vars;
_tmp38F=_tmp38D.where_clause;_tmp390=_tmp38D.body;{void*clause_inflow=Cyc_NewControlFlow_add_vars(
inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp38E))->v,Cyc_CfFlowInfo_unknown_all);
void*clause_outflow;if(_tmp38F != 0){void*_tmp392;void*_tmp393;struct _tuple0
_tmp391=Cyc_NewControlFlow_anal_test(env,clause_inflow,(struct Cyc_Absyn_Exp*)
_tmp38F);_tmp392=_tmp391.f1;_tmp393=_tmp391.f2;inflow=_tmp393;clause_outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp392,_tmp390);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp390);}{void*_tmp394=clause_outflow;_LL2A7: if((int)_tmp394 != 0)
goto _LL2A9;_LL2A8: goto _LL2A6;_LL2A9:;_LL2AA: if(scs->tl == 0)return clause_outflow;
else{if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*
_tmp395[0]={};Cyc_Tcutil_terr(_tmp390->loc,({const char*_tmp396="switch clause may implicitly fallthru";
_tag_arr(_tmp396,sizeof(char),_get_zero_arr_size(_tmp396,38));}),_tag_arr(
_tmp395,sizeof(void*),0));});else{({void*_tmp397[0]={};Cyc_Tcutil_warn(_tmp390->loc,({
const char*_tmp398="switch clause may implicitly fallthru";_tag_arr(_tmp398,
sizeof(char),_get_zero_arr_size(_tmp398,38));}),_tag_arr(_tmp397,sizeof(void*),0));});}
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->body,clause_outflow);}goto _LL2A6;_LL2A6:;}}}return(
void*)0;}static void*Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp399=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);inflow=(void*)_tmp399->flow;{
void*_tmp39A=(void*)s->r;struct Cyc_Absyn_Exp*_tmp39B;struct Cyc_Absyn_Exp*_tmp39C;
struct Cyc_Absyn_Exp*_tmp39D;struct Cyc_Absyn_Stmt*_tmp39E;struct Cyc_Absyn_Stmt*
_tmp39F;struct Cyc_Absyn_Exp*_tmp3A0;struct Cyc_Absyn_Stmt*_tmp3A1;struct Cyc_Absyn_Stmt*
_tmp3A2;struct _tuple3 _tmp3A3;struct Cyc_Absyn_Exp*_tmp3A4;struct Cyc_Absyn_Stmt*
_tmp3A5;struct Cyc_Absyn_Stmt*_tmp3A6;struct Cyc_Absyn_Stmt*_tmp3A7;struct _tuple3
_tmp3A8;struct Cyc_Absyn_Exp*_tmp3A9;struct Cyc_Absyn_Stmt*_tmp3AA;struct Cyc_Absyn_Exp*
_tmp3AB;struct _tuple3 _tmp3AC;struct Cyc_Absyn_Exp*_tmp3AD;struct Cyc_Absyn_Stmt*
_tmp3AE;struct _tuple3 _tmp3AF;struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_Absyn_Stmt*
_tmp3B1;struct Cyc_Absyn_Stmt*_tmp3B2;struct Cyc_Absyn_Stmt*_tmp3B3;struct Cyc_List_List*
_tmp3B4;struct Cyc_Absyn_Switch_clause**_tmp3B5;struct Cyc_Absyn_Switch_clause*
_tmp3B6;struct Cyc_Absyn_Stmt*_tmp3B7;struct Cyc_Absyn_Stmt*_tmp3B8;struct Cyc_Absyn_Stmt*
_tmp3B9;struct Cyc_Absyn_Exp*_tmp3BA;struct Cyc_List_List*_tmp3BB;struct Cyc_Absyn_Stmt*
_tmp3BC;struct Cyc_List_List*_tmp3BD;struct Cyc_Absyn_Decl*_tmp3BE;struct Cyc_Absyn_Stmt*
_tmp3BF;struct Cyc_Absyn_Stmt*_tmp3C0;struct Cyc_Absyn_Tvar*_tmp3C1;struct Cyc_Absyn_Vardecl*
_tmp3C2;int _tmp3C3;struct Cyc_Absyn_Stmt*_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C5;
_LL2AC: if((int)_tmp39A != 0)goto _LL2AE;_LL2AD: return inflow;_LL2AE: if(_tmp39A <= (
void*)1?1:*((int*)_tmp39A)!= 2)goto _LL2B0;_tmp39B=((struct Cyc_Absyn_Return_s_struct*)
_tmp39A)->f1;if(_tmp39B != 0)goto _LL2B0;_LL2AF: if(env->noreturn)({void*_tmp3C6[0]={};
Cyc_Tcutil_terr(s->loc,({const char*_tmp3C7="`noreturn' function might return";
_tag_arr(_tmp3C7,sizeof(char),_get_zero_arr_size(_tmp3C7,33));}),_tag_arr(
_tmp3C6,sizeof(void*),0));});Cyc_NewControlFlow_check_init_params(s->loc,env,
inflow);return(void*)0;_LL2B0: if(_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 2)goto
_LL2B2;_tmp39C=((struct Cyc_Absyn_Return_s_struct*)_tmp39A)->f1;_LL2B1: if(env->noreturn)({
void*_tmp3C8[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp3C9="`noreturn' function might return";
_tag_arr(_tmp3C9,sizeof(char),_get_zero_arr_size(_tmp3C9,33));}),_tag_arr(
_tmp3C8,sizeof(void*),0));});{void*_tmp3CB;void*_tmp3CC;struct _tuple0 _tmp3CA=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp39C));_tmp3CB=_tmp3CA.f1;
_tmp3CC=_tmp3CA.f2;_tmp3CB=Cyc_NewControlFlow_use_Rval(env,_tmp39C->loc,_tmp3CB,
_tmp3CC);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp3CB);return(void*)0;}
_LL2B2: if(_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 0)goto _LL2B4;_tmp39D=((struct
Cyc_Absyn_Exp_s_struct*)_tmp39A)->f1;_LL2B3: return(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp39D)).f1;_LL2B4: if(_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 1)goto
_LL2B6;_tmp39E=((struct Cyc_Absyn_Seq_s_struct*)_tmp39A)->f1;_tmp39F=((struct Cyc_Absyn_Seq_s_struct*)
_tmp39A)->f2;_LL2B5: return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp39E),_tmp39F);_LL2B6: if(_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 3)
goto _LL2B8;_tmp3A0=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp39A)->f1;_tmp3A1=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp39A)->f2;_tmp3A2=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp39A)->f3;_LL2B7: {void*_tmp3CE;void*_tmp3CF;struct _tuple0 _tmp3CD=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3A0);_tmp3CE=_tmp3CD.f1;_tmp3CF=_tmp3CD.f2;return Cyc_CfFlowInfo_join_flow(
env->all_changed,Cyc_NewControlFlow_anal_stmt(env,_tmp3CE,_tmp3A1),Cyc_NewControlFlow_anal_stmt(
env,_tmp3CF,_tmp3A2));}_LL2B8: if(_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 4)goto
_LL2BA;_tmp3A3=((struct Cyc_Absyn_While_s_struct*)_tmp39A)->f1;_tmp3A4=_tmp3A3.f1;
_tmp3A5=_tmp3A3.f2;_tmp3A6=((struct Cyc_Absyn_While_s_struct*)_tmp39A)->f2;_LL2B9: {
void*_tmp3D0=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp3A5))->flow;
void*_tmp3D2;void*_tmp3D3;struct _tuple0 _tmp3D1=Cyc_NewControlFlow_anal_test(env,
_tmp3D0,_tmp3A4);_tmp3D2=_tmp3D1.f1;_tmp3D3=_tmp3D1.f2;{void*_tmp3D4=Cyc_NewControlFlow_anal_stmt(
env,_tmp3D2,_tmp3A6);Cyc_NewControlFlow_update_flow(env,_tmp3A5,_tmp3D4);return
_tmp3D3;}}_LL2BA: if(_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 16)goto _LL2BC;
_tmp3A7=((struct Cyc_Absyn_Do_s_struct*)_tmp39A)->f1;_tmp3A8=((struct Cyc_Absyn_Do_s_struct*)
_tmp39A)->f2;_tmp3A9=_tmp3A8.f1;_tmp3AA=_tmp3A8.f2;_LL2BB: {void*_tmp3D5=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp3A7);void*_tmp3D6=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp3D5,_tmp3AA))->flow;void*_tmp3D8;void*_tmp3D9;struct _tuple0 _tmp3D7=Cyc_NewControlFlow_anal_test(
env,_tmp3D6,_tmp3A9);_tmp3D8=_tmp3D7.f1;_tmp3D9=_tmp3D7.f2;Cyc_NewControlFlow_update_flow(
env,_tmp3A7,_tmp3D8);return _tmp3D9;}_LL2BC: if(_tmp39A <= (void*)1?1:*((int*)
_tmp39A)!= 8)goto _LL2BE;_tmp3AB=((struct Cyc_Absyn_For_s_struct*)_tmp39A)->f1;
_tmp3AC=((struct Cyc_Absyn_For_s_struct*)_tmp39A)->f2;_tmp3AD=_tmp3AC.f1;_tmp3AE=
_tmp3AC.f2;_tmp3AF=((struct Cyc_Absyn_For_s_struct*)_tmp39A)->f3;_tmp3B0=_tmp3AF.f1;
_tmp3B1=_tmp3AF.f2;_tmp3B2=((struct Cyc_Absyn_For_s_struct*)_tmp39A)->f4;_LL2BD: {
void*_tmp3DA=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp3AB)).f1;void*_tmp3DB=(
void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp3DA,_tmp3AE))->flow;void*
_tmp3DD;void*_tmp3DE;struct _tuple0 _tmp3DC=Cyc_NewControlFlow_anal_test(env,
_tmp3DB,_tmp3AD);_tmp3DD=_tmp3DC.f1;_tmp3DE=_tmp3DC.f2;{void*_tmp3DF=Cyc_NewControlFlow_anal_stmt(
env,_tmp3DD,_tmp3B2);void*_tmp3E0=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp3DF,_tmp3B1))->flow;void*_tmp3E1=(Cyc_NewControlFlow_anal_Rexp(env,_tmp3E0,
_tmp3B0)).f1;Cyc_NewControlFlow_update_flow(env,_tmp3AE,_tmp3E1);return _tmp3DE;}}
_LL2BE: if(_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 5)goto _LL2C0;_tmp3B3=((struct
Cyc_Absyn_Break_s_struct*)_tmp39A)->f1;if(_tmp3B3 != 0)goto _LL2C0;_LL2BF: return(
void*)0;_LL2C0: if(_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 11)goto _LL2C2;_tmp3B4=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp39A)->f1;_tmp3B5=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp39A)->f2;if(_tmp3B5 == 0)goto _LL2C2;_tmp3B6=*_tmp3B5;_LL2C1: {void*_tmp3E3;
struct Cyc_List_List*_tmp3E4;struct _tuple5 _tmp3E2=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp3B4);_tmp3E3=_tmp3E2.f1;_tmp3E4=_tmp3E2.f2;for(0;_tmp3E4 != 0;(
_tmp3E4=_tmp3E4->tl,_tmp3B4=_tmp3B4->tl)){_tmp3E3=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3B4))->hd)->loc,
_tmp3E3,(void*)_tmp3E4->hd);}_tmp3E3=Cyc_NewControlFlow_add_vars(_tmp3E3,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3B6->pat_vars))->v,Cyc_CfFlowInfo_unknown_all);
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp3B6->body,_tmp3E3);
return(void*)0;}_LL2C2: if(_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 5)goto _LL2C4;
_tmp3B7=((struct Cyc_Absyn_Break_s_struct*)_tmp39A)->f1;_LL2C3: _tmp3B8=_tmp3B7;
goto _LL2C5;_LL2C4: if(_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 6)goto _LL2C6;
_tmp3B8=((struct Cyc_Absyn_Continue_s_struct*)_tmp39A)->f1;_LL2C5: _tmp3B9=_tmp3B8;
goto _LL2C7;_LL2C6: if(_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 7)goto _LL2C8;
_tmp3B9=((struct Cyc_Absyn_Goto_s_struct*)_tmp39A)->f2;_LL2C7: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp3E5=_tmp399->encloser;struct Cyc_Absyn_Stmt*
_tmp3E6=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp3B9)))->encloser;while(_tmp3E6 != _tmp3E5){struct Cyc_Absyn_Stmt*_tmp3E7=(Cyc_NewControlFlow_get_stmt_annot(
_tmp3E5))->encloser;if(_tmp3E7 == _tmp3E5){({void*_tmp3E8[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp3E9="goto enters local scope or exception handler";_tag_arr(
_tmp3E9,sizeof(char),_get_zero_arr_size(_tmp3E9,45));}),_tag_arr(_tmp3E8,sizeof(
void*),0));});break;}_tmp3E5=_tmp3E7;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp3B9),inflow);return(void*)0;_LL2C8: if(
_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 9)goto _LL2CA;_tmp3BA=((struct Cyc_Absyn_Switch_s_struct*)
_tmp39A)->f1;_tmp3BB=((struct Cyc_Absyn_Switch_s_struct*)_tmp39A)->f2;_LL2C9: {
void*_tmp3EB;void*_tmp3EC;struct _tuple0 _tmp3EA=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp3BA);_tmp3EB=_tmp3EA.f1;_tmp3EC=_tmp3EA.f2;_tmp3EB=Cyc_NewControlFlow_use_Rval(
env,_tmp3BA->loc,_tmp3EB,_tmp3EC);return Cyc_NewControlFlow_anal_scs(env,_tmp3EB,
_tmp3BB);}_LL2CA: if(_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 17)goto _LL2CC;
_tmp3BC=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp39A)->f1;_tmp3BD=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp39A)->f2;_LL2CB: {int old_in_try=env->in_try;void*old_tryflow=(void*)env->tryflow;
env->in_try=1;(void*)(env->tryflow=(void*)inflow);{void*s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp3BC);void*scs_inflow=(void*)env->tryflow;env->in_try=old_in_try;(
void*)(env->tryflow=(void*)old_tryflow);Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{void*scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp3BD);{
void*_tmp3ED=scs_outflow;_LL2D7: if((int)_tmp3ED != 0)goto _LL2D9;_LL2D8: goto _LL2D6;
_LL2D9:;_LL2DA:({void*_tmp3EE[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp3EF="last catch clause may implicitly fallthru";
_tag_arr(_tmp3EF,sizeof(char),_get_zero_arr_size(_tmp3EF,42));}),_tag_arr(
_tmp3EE,sizeof(void*),0));});_LL2D6:;}return s1_outflow;}}}_LL2CC: if(_tmp39A <= (
void*)1?1:*((int*)_tmp39A)!= 12)goto _LL2CE;_tmp3BE=((struct Cyc_Absyn_Decl_s_struct*)
_tmp39A)->f1;_tmp3BF=((struct Cyc_Absyn_Decl_s_struct*)_tmp39A)->f2;_LL2CD: return
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp3BE),
_tmp3BF);_LL2CE: if(_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 15)goto _LL2D0;_tmp3C0=((
struct Cyc_Absyn_Label_s_struct*)_tmp39A)->f2;_LL2CF: return Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp3C0);_LL2D0: if(_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 18)goto
_LL2D2;_tmp3C1=((struct Cyc_Absyn_Region_s_struct*)_tmp39A)->f1;_tmp3C2=((struct
Cyc_Absyn_Region_s_struct*)_tmp39A)->f2;_tmp3C3=((struct Cyc_Absyn_Region_s_struct*)
_tmp39A)->f3;_tmp3C4=((struct Cyc_Absyn_Region_s_struct*)_tmp39A)->f4;_LL2D1: {
struct Cyc_List_List _tmp3F0=({struct Cyc_List_List _tmp3F1;_tmp3F1.hd=_tmp3C2;
_tmp3F1.tl=0;_tmp3F1;});inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)&
_tmp3F0,Cyc_CfFlowInfo_unknown_all);return Cyc_NewControlFlow_anal_stmt(env,
inflow,_tmp3C4);}_LL2D2: if(_tmp39A <= (void*)1?1:*((int*)_tmp39A)!= 20)goto _LL2D4;
_tmp3C5=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp39A)->f1;_LL2D3: {void*
_tmp3F3;void*_tmp3F4;struct _tuple0 _tmp3F2=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp3C5);_tmp3F3=_tmp3F2.f1;_tmp3F4=_tmp3F2.f2;{void*_tmp3F5=Cyc_NewControlFlow_use_Rval(
env,_tmp3C5->loc,_tmp3F3,_tmp3F4);void*_tmp3F6=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3C5->topt))->v);void*_tmp3F7;_LL2DC: if(
_tmp3F6 <= (void*)3?1:*((int*)_tmp3F6)!= 15)goto _LL2DE;_tmp3F7=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp3F6)->f1;_LL2DD: return Cyc_CfFlowInfo_kill_flow_region(_tmp3F3,_tmp3F7);
_LL2DE:;_LL2DF:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3F8=
_cycalloc(sizeof(*_tmp3F8));_tmp3F8[0]=({struct Cyc_Core_Impossible_struct _tmp3F9;
_tmp3F9.tag=Cyc_Core_Impossible;_tmp3F9.f1=({const char*_tmp3FA="anal_stmt -- reset_region";
_tag_arr(_tmp3FA,sizeof(char),_get_zero_arr_size(_tmp3FA,26));});_tmp3F9;});
_tmp3F8;}));_LL2DB:;}}_LL2D4:;_LL2D5:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB[0]=({struct Cyc_Core_Impossible_struct
_tmp3FC;_tmp3FC.tag=Cyc_Core_Impossible;_tmp3FC.f1=({const char*_tmp3FD="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_arr(_tmp3FD,sizeof(char),_get_zero_arr_size(_tmp3FD,56));});_tmp3FC;});
_tmp3FB;}));_LL2AB:;}}static void*Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Decl*decl){void*_tmp3FE=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp3FF;struct Cyc_Core_Opt*_tmp400;struct Cyc_Core_Opt _tmp401;struct Cyc_List_List*
_tmp402;struct Cyc_Absyn_Exp*_tmp403;struct Cyc_List_List*_tmp404;_LL2E1: if(*((int*)
_tmp3FE)!= 0)goto _LL2E3;_tmp3FF=((struct Cyc_Absyn_Var_d_struct*)_tmp3FE)->f1;
_LL2E2: {struct Cyc_List_List _tmp405=({struct Cyc_List_List _tmp413;_tmp413.hd=
_tmp3FF;_tmp413.tl=0;_tmp413;});inflow=Cyc_NewControlFlow_add_vars(inflow,(
struct Cyc_List_List*)& _tmp405,Cyc_CfFlowInfo_unknown_none);{struct Cyc_Absyn_Exp*
_tmp406=_tmp3FF->initializer;if(_tmp406 == 0)return inflow;{void*_tmp408;void*
_tmp409;struct _tuple0 _tmp407=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
_tmp406);_tmp408=_tmp407.f1;_tmp409=_tmp407.f2;{void*_tmp40A=_tmp408;struct Cyc_Dict_Dict*
_tmp40B;struct Cyc_List_List*_tmp40C;_LL2EA: if((int)_tmp40A != 0)goto _LL2EC;_LL2EB:
return(void*)0;_LL2EC: if(_tmp40A <= (void*)1?1:*((int*)_tmp40A)!= 0)goto _LL2E9;
_tmp40B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp40A)->f1;_tmp40C=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp40A)->f2;_LL2ED: _tmp40B=Cyc_CfFlowInfo_assign_place(
decl->loc,_tmp40B,env->all_changed,({struct Cyc_CfFlowInfo_Place*_tmp40D=
_cycalloc(sizeof(*_tmp40D));_tmp40D->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp40F;_tmp40F.tag=0;_tmp40F.f1=_tmp3FF;_tmp40F;});_tmp40E;}));_tmp40D->fields=
0;_tmp40D;}),_tmp409);_tmp40C=Cyc_CfFlowInfo_reln_assign_var(_tmp40C,_tmp3FF,(
struct Cyc_Absyn_Exp*)_check_null(_tmp3FF->initializer));{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp410=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp411=_cycalloc(sizeof(*
_tmp411));_tmp411[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp412;_tmp412.tag=
0;_tmp412.f1=_tmp40B;_tmp412.f2=_tmp40C;_tmp412;});_tmp411;});Cyc_NewControlFlow_update_tryflow(
env,(void*)_tmp410);return(void*)_tmp410;}_LL2E9:;}}}}_LL2E3: if(*((int*)_tmp3FE)
!= 2)goto _LL2E5;_tmp400=((struct Cyc_Absyn_Let_d_struct*)_tmp3FE)->f2;if(_tmp400
== 0)goto _LL2E5;_tmp401=*_tmp400;_tmp402=(struct Cyc_List_List*)_tmp401.v;_tmp403=((
struct Cyc_Absyn_Let_d_struct*)_tmp3FE)->f3;_LL2E4: {void*_tmp415;void*_tmp416;
struct _tuple0 _tmp414=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp403);_tmp415=
_tmp414.f1;_tmp416=_tmp414.f2;_tmp415=Cyc_NewControlFlow_use_Rval(env,_tmp403->loc,
_tmp415,_tmp416);return Cyc_NewControlFlow_add_vars(_tmp415,_tmp402,Cyc_CfFlowInfo_unknown_all);}
_LL2E5: if(*((int*)_tmp3FE)!= 3)goto _LL2E7;_tmp404=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3FE)->f1;_LL2E6: return Cyc_NewControlFlow_add_vars(inflow,_tmp404,Cyc_CfFlowInfo_unknown_none);
_LL2E7:;_LL2E8:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp417=
_cycalloc(sizeof(*_tmp417));_tmp417[0]=({struct Cyc_Core_Impossible_struct _tmp418;
_tmp418.tag=Cyc_Core_Impossible;_tmp418.f1=({const char*_tmp419="anal_decl: unexpected decl variant";
_tag_arr(_tmp419,sizeof(char),_get_zero_arr_size(_tmp419,35));});_tmp418;});
_tmp417;}));_LL2E0:;}static void Cyc_NewControlFlow_postpass(struct Cyc_Absyn_Stmt*
s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{void*_tmp41A=(void*)s->r;
struct _tuple3 _tmp41B;struct Cyc_Absyn_Stmt*_tmp41C;struct _tuple3 _tmp41D;struct Cyc_Absyn_Stmt*
_tmp41E;struct Cyc_Absyn_Stmt*_tmp41F;struct Cyc_Absyn_Stmt*_tmp420;struct Cyc_Absyn_Stmt*
_tmp421;struct _tuple3 _tmp422;struct Cyc_Absyn_Stmt*_tmp423;struct Cyc_Absyn_Stmt*
_tmp424;struct Cyc_Absyn_Stmt*_tmp425;struct _tuple3 _tmp426;struct Cyc_Absyn_Stmt*
_tmp427;struct Cyc_Absyn_Stmt*_tmp428;struct Cyc_Absyn_Stmt*_tmp429;struct Cyc_Absyn_Stmt*
_tmp42A;struct Cyc_Absyn_Stmt*_tmp42B;struct Cyc_Absyn_Stmt*_tmp42C;struct Cyc_Absyn_Stmt*
_tmp42D;struct Cyc_List_List*_tmp42E;struct Cyc_List_List*_tmp42F;_LL2EF: if(
_tmp41A <= (void*)1?1:*((int*)_tmp41A)!= 8)goto _LL2F1;_tmp41B=((struct Cyc_Absyn_For_s_struct*)
_tmp41A)->f2;_tmp41C=_tmp41B.f2;_tmp41D=((struct Cyc_Absyn_For_s_struct*)_tmp41A)->f3;
_tmp41E=_tmp41D.f2;_tmp41F=((struct Cyc_Absyn_For_s_struct*)_tmp41A)->f4;_LL2F0:
Cyc_NewControlFlow_postpass(_tmp41E);_tmp420=_tmp41F;_tmp421=_tmp41C;goto _LL2F2;
_LL2F1: if(_tmp41A <= (void*)1?1:*((int*)_tmp41A)!= 1)goto _LL2F3;_tmp420=((struct
Cyc_Absyn_Seq_s_struct*)_tmp41A)->f1;_tmp421=((struct Cyc_Absyn_Seq_s_struct*)
_tmp41A)->f2;_LL2F2: _tmp423=_tmp420;_tmp424=_tmp421;goto _LL2F4;_LL2F3: if(_tmp41A
<= (void*)1?1:*((int*)_tmp41A)!= 4)goto _LL2F5;_tmp422=((struct Cyc_Absyn_While_s_struct*)
_tmp41A)->f1;_tmp423=_tmp422.f2;_tmp424=((struct Cyc_Absyn_While_s_struct*)
_tmp41A)->f2;_LL2F4: _tmp425=_tmp424;_tmp427=_tmp423;goto _LL2F6;_LL2F5: if(_tmp41A
<= (void*)1?1:*((int*)_tmp41A)!= 16)goto _LL2F7;_tmp425=((struct Cyc_Absyn_Do_s_struct*)
_tmp41A)->f1;_tmp426=((struct Cyc_Absyn_Do_s_struct*)_tmp41A)->f2;_tmp427=_tmp426.f2;
_LL2F6: _tmp428=_tmp425;_tmp429=_tmp427;goto _LL2F8;_LL2F7: if(_tmp41A <= (void*)1?1:*((
int*)_tmp41A)!= 3)goto _LL2F9;_tmp428=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp41A)->f2;_tmp429=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp41A)->f3;_LL2F8:
Cyc_NewControlFlow_postpass(_tmp428);_tmp42A=_tmp429;goto _LL2FA;_LL2F9: if(
_tmp41A <= (void*)1?1:*((int*)_tmp41A)!= 18)goto _LL2FB;_tmp42A=((struct Cyc_Absyn_Region_s_struct*)
_tmp41A)->f4;_LL2FA: _tmp42B=_tmp42A;goto _LL2FC;_LL2FB: if(_tmp41A <= (void*)1?1:*((
int*)_tmp41A)!= 15)goto _LL2FD;_tmp42B=((struct Cyc_Absyn_Label_s_struct*)_tmp41A)->f2;
_LL2FC: _tmp42C=_tmp42B;goto _LL2FE;_LL2FD: if(_tmp41A <= (void*)1?1:*((int*)_tmp41A)
!= 12)goto _LL2FF;_tmp42C=((struct Cyc_Absyn_Decl_s_struct*)_tmp41A)->f2;_LL2FE:
Cyc_NewControlFlow_postpass(_tmp42C);return;_LL2FF: if(_tmp41A <= (void*)1?1:*((
int*)_tmp41A)!= 17)goto _LL301;_tmp42D=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp41A)->f1;_tmp42E=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp41A)->f2;_LL300:
Cyc_NewControlFlow_postpass(_tmp42D);_tmp42F=_tmp42E;goto _LL302;_LL301: if(
_tmp41A <= (void*)1?1:*((int*)_tmp41A)!= 9)goto _LL303;_tmp42F=((struct Cyc_Absyn_Switch_s_struct*)
_tmp41A)->f2;_LL302: for(0;_tmp42F != 0;_tmp42F=_tmp42F->tl){Cyc_NewControlFlow_postpass(((
struct Cyc_Absyn_Switch_clause*)_tmp42F->hd)->body);}return;_LL303:;_LL304:
return;_LL2EE:;}}static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct Cyc_Position_Segment*_tmp430=(fd->body)->loc;void*inflow=Cyc_NewControlFlow_add_vars((
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp465=_cycalloc(sizeof(*
_tmp465));_tmp465[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp466;_tmp466.tag=
0;_tmp466.f1=Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp466.f2=0;_tmp466;});
_tmp465;}),(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,
Cyc_CfFlowInfo_unknown_all);struct Cyc_List_List*_tmp431=0;{void*_tmp432=inflow;
struct Cyc_Dict_Dict*_tmp433;struct Cyc_List_List*_tmp434;_LL306: if((int)_tmp432 != 
0)goto _LL308;_LL307:({void*_tmp435[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp436="check_fun";_tag_arr(
_tmp436,sizeof(char),_get_zero_arr_size(_tmp436,10));}),_tag_arr(_tmp435,sizeof(
void*),0));});_LL308: if(_tmp432 <= (void*)1?1:*((int*)_tmp432)!= 0)goto _LL305;
_tmp433=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp432)->f1;_tmp434=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp432)->f2;_LL309: {struct Cyc_List_List*
atts;{void*_tmp437=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp438;struct Cyc_List_List*_tmp439;
_LL30B: if(_tmp437 <= (void*)3?1:*((int*)_tmp437)!= 8)goto _LL30D;_tmp438=((struct
Cyc_Absyn_FnType_struct*)_tmp437)->f1;_tmp439=_tmp438.attributes;_LL30C: atts=
_tmp439;goto _LL30A;_LL30D:;_LL30E:({void*_tmp43A[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp43B="check_fun: non-function type cached with fndecl_t";
_tag_arr(_tmp43B,sizeof(char),_get_zero_arr_size(_tmp43B,50));}),_tag_arr(
_tmp43A,sizeof(void*),0));});_LL30A:;}for(0;atts != 0;atts=atts->tl){void*_tmp43C=(
void*)atts->hd;int _tmp43D;_LL310: if(_tmp43C <= (void*)17?1:*((int*)_tmp43C)!= 4)
goto _LL312;_tmp43D=((struct Cyc_Absyn_Initializes_att_struct*)_tmp43C)->f1;_LL311: {
unsigned int j=(unsigned int)_tmp43D;if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp43E[0]={};Cyc_Tcutil_terr(_tmp430,({const char*_tmp43F="initializes attribute exceeds number of parameters";
_tag_arr(_tmp43F,sizeof(char),_get_zero_arr_size(_tmp43F,51));}),_tag_arr(
_tmp43E,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*_tmp440=((struct
Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*
_tmp441=Cyc_Tcutil_compress((void*)_tmp440->type);struct Cyc_Absyn_PtrInfo _tmp442;
void*_tmp443;struct Cyc_Absyn_PtrAtts _tmp444;struct Cyc_Absyn_Conref*_tmp445;
struct Cyc_Absyn_Conref*_tmp446;struct Cyc_Absyn_Conref*_tmp447;_LL315: if(_tmp441
<= (void*)3?1:*((int*)_tmp441)!= 4)goto _LL317;_tmp442=((struct Cyc_Absyn_PointerType_struct*)
_tmp441)->f1;_tmp443=(void*)_tmp442.elt_typ;_tmp444=_tmp442.ptr_atts;_tmp445=
_tmp444.nullable;_tmp446=_tmp444.bounds;_tmp447=_tmp444.zero_term;_LL316: if(((
int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp445))({void*_tmp448[0]={};
Cyc_Tcutil_terr(_tmp430,({const char*_tmp449="initializes attribute not allowed on nullable pointers";
_tag_arr(_tmp449,sizeof(char),_get_zero_arr_size(_tmp449,55));}),_tag_arr(
_tmp448,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp446))({void*_tmp44A[
0]={};Cyc_Tcutil_terr(_tmp430,({const char*_tmp44B="initializes attribute allowed only on pointers of size 1";
_tag_arr(_tmp44B,sizeof(char),_get_zero_arr_size(_tmp44B,57));}),_tag_arr(
_tmp44A,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp447))({void*_tmp44C[0]={};Cyc_Tcutil_terr(_tmp430,({const char*_tmp44D="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_arr(_tmp44D,sizeof(char),_get_zero_arr_size(_tmp44D,77));}),_tag_arr(
_tmp44C,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*_tmp44E=({
struct Cyc_CfFlowInfo_InitParam_struct*_tmp456=_cycalloc(sizeof(*_tmp456));
_tmp456[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp457;_tmp457.tag=2;_tmp457.f1=(
int)j;_tmp457.f2=(void*)_tmp443;_tmp457;});_tmp456;});struct Cyc_CfFlowInfo_Place*
_tmp44F=({struct Cyc_CfFlowInfo_Place*_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455->root=(
void*)((void*)_tmp44E);_tmp455->fields=0;_tmp455;});_tmp433=Cyc_Dict_insert(
_tmp433,(void*)_tmp44E,Cyc_CfFlowInfo_typ_to_absrval(_tmp443,Cyc_CfFlowInfo_esc_none));
_tmp433=Cyc_Dict_insert(_tmp433,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp451;_tmp451.tag=0;_tmp451.f1=_tmp440;_tmp451;});_tmp450;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp452=_cycalloc(sizeof(*_tmp452));_tmp452[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp453;_tmp453.tag=2;_tmp453.f1=_tmp44F;
_tmp453;});_tmp452;}));_tmp431=({struct Cyc_List_List*_tmp454=_cycalloc(sizeof(*
_tmp454));_tmp454->hd=_tmp44F;_tmp454->tl=_tmp431;_tmp454;});goto _LL314;}_LL317:;
_LL318:({void*_tmp458[0]={};Cyc_Tcutil_terr(_tmp430,({const char*_tmp459="initializes attribute on non-pointer";
_tag_arr(_tmp459,sizeof(char),_get_zero_arr_size(_tmp459,37));}),_tag_arr(
_tmp458,sizeof(void*),0));});_LL314:;}goto _LL30F;}}_LL312:;_LL313: goto _LL30F;
_LL30F:;}inflow=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp45A=
_cycalloc(sizeof(*_tmp45A));_tmp45A[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp45B;_tmp45B.tag=0;_tmp45B.f1=_tmp433;_tmp45B.f2=_tmp434;_tmp45B;});_tmp45A;});}
_LL305:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));struct
Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp464=
_cycalloc(sizeof(*_tmp464));_tmp464->iterate_again=1;_tmp464->iteration_num=0;
_tmp464->in_try=0;_tmp464->tryflow=(void*)inflow;_tmp464->all_changed=0;_tmp464->noreturn=
noreturn;_tmp464->param_roots=_tmp431;_tmp464;});void*outflow=inflow;while(env->iterate_again){
++ env->iteration_num;env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,fd->body);}{void*_tmp45C=outflow;_LL31A: if((int)_tmp45C != 0)goto
_LL31C;_LL31B: goto _LL319;_LL31C:;_LL31D: Cyc_NewControlFlow_check_init_params(
_tmp430,env,outflow);if(noreturn)({void*_tmp45D[0]={};Cyc_Tcutil_terr(_tmp430,({
const char*_tmp45E="`noreturn' function might (implicitly) return";_tag_arr(
_tmp45E,sizeof(char),_get_zero_arr_size(_tmp45E,46));}),_tag_arr(_tmp45D,sizeof(
void*),0));});else{void*_tmp45F=Cyc_Tcutil_compress((void*)fd->ret_type);_LL31F:
if((int)_tmp45F != 0)goto _LL321;_LL320: goto _LL31E;_LL321: if(_tmp45F <= (void*)3?1:*((
int*)_tmp45F)!= 6)goto _LL323;_LL322: goto _LL324;_LL323: if((int)_tmp45F != 1)goto
_LL325;_LL324: goto _LL326;_LL325: if(_tmp45F <= (void*)3?1:*((int*)_tmp45F)!= 5)
goto _LL327;_LL326:({void*_tmp460[0]={};Cyc_Tcutil_warn(_tmp430,({const char*
_tmp461="function may not return a value";_tag_arr(_tmp461,sizeof(char),
_get_zero_arr_size(_tmp461,32));}),_tag_arr(_tmp460,sizeof(void*),0));});goto
_LL31E;_LL327:;_LL328:({void*_tmp462[0]={};Cyc_Tcutil_terr(_tmp430,({const char*
_tmp463="function may not return a value";_tag_arr(_tmp463,sizeof(char),
_get_zero_arr_size(_tmp463,32));}),_tag_arr(_tmp462,sizeof(void*),0));});goto
_LL31E;_LL31E:;}goto _LL319;_LL319:;}Cyc_NewControlFlow_postpass(fd->body);}}void
Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*
_tmp467=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp468;
struct Cyc_List_List*_tmp469;struct Cyc_List_List*_tmp46A;struct Cyc_List_List*
_tmp46B;_LL32A: if(*((int*)_tmp467)!= 1)goto _LL32C;_tmp468=((struct Cyc_Absyn_Fn_d_struct*)
_tmp467)->f1;_LL32B: Cyc_NewControlFlow_check_fun(_tmp468);goto _LL329;_LL32C: if(*((
int*)_tmp467)!= 10)goto _LL32E;_tmp469=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp467)->f1;_LL32D: _tmp46A=_tmp469;goto _LL32F;_LL32E: if(*((int*)_tmp467)!= 9)
goto _LL330;_tmp46A=((struct Cyc_Absyn_Using_d_struct*)_tmp467)->f2;_LL32F: _tmp46B=
_tmp46A;goto _LL331;_LL330: if(*((int*)_tmp467)!= 8)goto _LL332;_tmp46B=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp467)->f2;_LL331: Cyc_NewControlFlow_cf_check(_tmp46B);goto _LL329;_LL332:;
_LL333: goto _LL329;_LL329:;}}
