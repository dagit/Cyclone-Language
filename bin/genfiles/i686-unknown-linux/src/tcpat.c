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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(
void*,int),void*env);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);
};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{int __pos;
Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;
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
char*tag;struct _tagged_arr f1;};struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*
r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict*d);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct
_RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct
_RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(struct Cyc_Set_Set*
s);int Cyc_Set_is_empty(struct Cyc_Set_Set*s);int Cyc_Set_member(struct Cyc_Set_Set*
s,void*elt);extern char Cyc_Set_Absent[11];void*Cyc_Set_choose(struct Cyc_Set_Set*s);
int Cyc_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);int Cyc_strptrcmp(struct
_tagged_arr*s1,struct _tagged_arr*s2);struct _tagged_arr Cyc_strconcat_l(struct Cyc_List_List*);
struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
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
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_RgnHandleType_struct{
int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*f1;
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
tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*f1;void*f2;};
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
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple3{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple3 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple3 f2;struct _tuple3 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple3 f2;
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
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
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;void*
Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct
Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_float_typ;
extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct
_tagged_arr Cyc_Absynpp_kind2string(void*);struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*);struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,
struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*
rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*
po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**
Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_List_List*);void Cyc_Tcenv_check_rgn_accessible(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);int Cyc_Tcutil_kind_leq(void*k1,void*k2);
void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerceable(void*);extern struct Cyc_Core_Opt
Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);struct _tuple4{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*
Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr err_msg);
struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct
_tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{
struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);void
Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);void
Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,struct Cyc_Absyn_Pat*
p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,struct Cyc_List_List*);
struct _tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p){void*_tmp0=(void*)p->r;struct _tuple1*_tmp1;struct _tuple1*_tmp2;struct Cyc_List_List*
_tmp3;int _tmp4;struct Cyc_Absyn_AggrInfo _tmp5;void*_tmp6;void*_tmp7;struct _tuple1*
_tmp8;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmpA;struct Cyc_List_List*
_tmpB;int _tmpC;struct Cyc_Absyn_Exp*_tmpD;_LL1: if(_tmp0 <= (void*)2  || *((int*)
_tmp0)!= 12)goto _LL3;_tmp1=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;_LL2:{
struct _handler_cons _tmpE;_push_handler(& _tmpE);{int _tmp10=0;if(setjmp(_tmpE.handler))
_tmp10=1;if(!_tmp10){{void*_tmp11=Cyc_Tcenv_lookup_ordinary(te,p->loc,_tmp1);
struct Cyc_Absyn_Tuniondecl*_tmp12;struct Cyc_Absyn_Tunionfield*_tmp13;struct Cyc_Absyn_Enumdecl*
_tmp14;struct Cyc_Absyn_Enumfield*_tmp15;void*_tmp16;struct Cyc_Absyn_Enumfield*
_tmp17;_LLC: if(*((int*)_tmp11)!= 1)goto _LLE;_LLD:(void*)(p->r=(void*)((void*)0));({
void*_tmp18[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp19="struct tag used without arguments in pattern";
_tag_arr(_tmp19,sizeof(char),_get_zero_arr_size(_tmp19,45));}),_tag_arr(_tmp18,
sizeof(void*),0));});_npop_handler(0);return;_LLE: if(*((int*)_tmp11)!= 2)goto
_LL10;_tmp12=((struct Cyc_Tcenv_TunionRes_struct*)_tmp11)->f1;_tmp13=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp11)->f2;_LLF:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*
_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A[0]=({struct Cyc_Absyn_Tunion_p_struct
_tmp1B;_tmp1B.tag=6;_tmp1B.f1=_tmp12;_tmp1B.f2=_tmp13;_tmp1B.f3=0;_tmp1B.f4=0;
_tmp1B;});_tmp1A;})));_npop_handler(0);return;_LL10: if(*((int*)_tmp11)!= 3)goto
_LL12;_tmp14=((struct Cyc_Tcenv_EnumRes_struct*)_tmp11)->f1;_tmp15=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp11)->f2;_LL11:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Enum_p_struct*
_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=({struct Cyc_Absyn_Enum_p_struct
_tmp1D;_tmp1D.tag=10;_tmp1D.f1=_tmp14;_tmp1D.f2=_tmp15;_tmp1D;});_tmp1C;})));
_npop_handler(0);return;_LL12: if(*((int*)_tmp11)!= 4)goto _LL14;_tmp16=(void*)((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp11)->f1;_tmp17=((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp11)->f2;_LL13:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_AnonEnum_p_struct*
_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=({struct Cyc_Absyn_AnonEnum_p_struct
_tmp1F;_tmp1F.tag=11;_tmp1F.f1=(void*)_tmp16;_tmp1F.f2=_tmp17;_tmp1F;});_tmp1E;})));
_npop_handler(0);return;_LL14: if(*((int*)_tmp11)!= 0)goto _LLB;_LL15: goto _LLB;
_LLB:;};_pop_handler();}else{void*_tmpF=(void*)_exn_thrown;void*_tmp21=_tmpF;
_LL17: if(_tmp21 != Cyc_Dict_Absent)goto _LL19;_LL18: goto _LL16;_LL19:;_LL1A:(void)
_throw(_tmp21);_LL16:;}}}{void*_tmp22=(*_tmp1).f1;struct Cyc_List_List*_tmp23;
_LL1C: if((int)_tmp22 != 0)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(_tmp22 <= (void*)1
 || *((int*)_tmp22)!= 0)goto _LL20;_tmp23=((struct Cyc_Absyn_Rel_n_struct*)_tmp22)->f1;
if(_tmp23 != 0)goto _LL20;_LL1F:(*_tmp1).f1=(void*)0;(void*)(p->r=(void*)((void*)({
struct Cyc_Absyn_Var_p_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24[0]=({
struct Cyc_Absyn_Var_p_struct _tmp25;_tmp25.tag=0;_tmp25.f1=Cyc_Absyn_new_vardecl(
_tmp1,(void*)0,0);_tmp25;});_tmp24;})));return;_LL20:;_LL21:({void*_tmp26[0]={};
Cyc_Tcutil_terr(p->loc,({const char*_tmp27="qualified variable in pattern";
_tag_arr(_tmp27,sizeof(char),_get_zero_arr_size(_tmp27,30));}),_tag_arr(_tmp26,
sizeof(void*),0));});return;_LL1B:;}_LL3: if(_tmp0 <= (void*)2  || *((int*)_tmp0)!= 
13)goto _LL5;_tmp2=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f1;_tmp3=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f2;_tmp4=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0)->f3;_LL4:{struct _handler_cons _tmp28;_push_handler(& _tmp28);{int _tmp2A=0;
if(setjmp(_tmp28.handler))_tmp2A=1;if(!_tmp2A){{void*_tmp2B=Cyc_Tcenv_lookup_ordinary(
te,p->loc,_tmp2);struct Cyc_Absyn_Aggrdecl*_tmp2C;struct Cyc_Absyn_Tuniondecl*
_tmp2D;struct Cyc_Absyn_Tunionfield*_tmp2E;_LL23: if(*((int*)_tmp2B)!= 1)goto _LL25;
_tmp2C=((struct Cyc_Tcenv_AggrRes_struct*)_tmp2B)->f1;_LL24: if((void*)_tmp2C->kind
== (void*)1){(void*)(p->r=(void*)((void*)0));({void*_tmp2F[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp30="cannot pattern-match a union";_tag_arr(_tmp30,sizeof(
char),_get_zero_arr_size(_tmp30,29));}),_tag_arr(_tmp2F,sizeof(void*),0));});
_npop_handler(0);return;}{struct Cyc_List_List*_tmp31=0;for(0;_tmp3 != 0;_tmp3=
_tmp3->tl){_tmp31=({struct Cyc_List_List*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->hd=({
struct _tuple8*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->f1=0;_tmp33->f2=(struct
Cyc_Absyn_Pat*)_tmp3->hd;_tmp33;});_tmp32->tl=_tmp31;_tmp32;});}(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp34=_cycalloc(sizeof(*_tmp34));
_tmp34[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp35;_tmp35.tag=5;_tmp35.f1=({struct
Cyc_Absyn_AggrInfo _tmp36;_tmp36.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*
_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37[0]=({struct Cyc_Absyn_KnownAggr_struct
_tmp38;_tmp38.tag=1;_tmp38.f1=({struct Cyc_Absyn_Aggrdecl**_tmp39=_cycalloc(
sizeof(*_tmp39));_tmp39[0]=_tmp2C;_tmp39;});_tmp38;});_tmp37;}));_tmp36.targs=0;
_tmp36;});_tmp35.f2=0;_tmp35.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp31);_tmp35.f4=_tmp4;_tmp35;});_tmp34;})));_npop_handler(0);
return;}_LL25: if(*((int*)_tmp2B)!= 2)goto _LL27;_tmp2D=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp2B)->f1;_tmp2E=((struct Cyc_Tcenv_TunionRes_struct*)_tmp2B)->f2;_LL26:(void*)(
p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*_tmp3A=_cycalloc(sizeof(*
_tmp3A));_tmp3A[0]=({struct Cyc_Absyn_Tunion_p_struct _tmp3B;_tmp3B.tag=6;_tmp3B.f1=
_tmp2D;_tmp3B.f2=_tmp2E;_tmp3B.f3=_tmp3;_tmp3B.f4=_tmp4;_tmp3B;});_tmp3A;})));
_npop_handler(0);return;_LL27: if(*((int*)_tmp2B)!= 3)goto _LL29;_LL28: goto _LL2A;
_LL29: if(*((int*)_tmp2B)!= 4)goto _LL2B;_LL2A:({void*_tmp3C[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp3D="enum tag used with arguments in pattern";_tag_arr(
_tmp3D,sizeof(char),_get_zero_arr_size(_tmp3D,40));}),_tag_arr(_tmp3C,sizeof(
void*),0));});(void*)(p->r=(void*)((void*)0));_npop_handler(0);return;_LL2B: if(*((
int*)_tmp2B)!= 0)goto _LL22;_LL2C: goto _LL22;_LL22:;};_pop_handler();}else{void*
_tmp29=(void*)_exn_thrown;void*_tmp3F=_tmp29;_LL2E: if(_tmp3F != Cyc_Dict_Absent)
goto _LL30;_LL2F: goto _LL2D;_LL30:;_LL31:(void)_throw(_tmp3F);_LL2D:;}}}({struct
Cyc_String_pa_struct _tmp42;_tmp42.tag=0;_tmp42.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp2));{void*_tmp40[1]={& _tmp42};Cyc_Tcutil_terr(
p->loc,({const char*_tmp41="%s is not a constructor in pattern";_tag_arr(_tmp41,
sizeof(char),_get_zero_arr_size(_tmp41,35));}),_tag_arr(_tmp40,sizeof(void*),1));}});(
void*)(p->r=(void*)((void*)0));return;_LL5: if(_tmp0 <= (void*)2  || *((int*)_tmp0)
!= 5)goto _LL7;_tmp5=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f1;_tmp6=(void*)
_tmp5.aggr_info;if(*((int*)_tmp6)!= 0)goto _LL7;_tmp7=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp6)->f1;_tmp8=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp6)->f2;_tmp9=_tmp5.targs;
_tmpA=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;_tmpB=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f3;_tmpC=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f4;_LL6:{struct
_handler_cons _tmp43;_push_handler(& _tmp43);{int _tmp45=0;if(setjmp(_tmp43.handler))
_tmp45=1;if(!_tmp45){{struct Cyc_Absyn_Aggrdecl**_tmp46=Cyc_Tcenv_lookup_aggrdecl(
te,p->loc,_tmp8);struct Cyc_Absyn_Aggrdecl*_tmp47=*_tmp46;if((void*)_tmp47->kind
== (void*)1){({void*_tmp48[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp49="cannot pattern-match a union";
_tag_arr(_tmp49,sizeof(char),_get_zero_arr_size(_tmp49,29));}),_tag_arr(_tmp48,
sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));_npop_handler(0);return;}
if(_tmp47->impl == 0){({void*_tmp4A[0]={};Cyc_Tcutil_terr(p->loc,({const char*
_tmp4B="can't destructure an abstract struct";_tag_arr(_tmp4B,sizeof(char),
_get_zero_arr_size(_tmp4B,37));}),_tag_arr(_tmp4A,sizeof(void*),0));});(void*)(p->r=(
void*)((void*)0));_npop_handler(0);return;}{int more_exists=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp47->impl))->exist_vars)
- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA);if(more_exists != 0)({
void*_tmp4C[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp4D="wrong number of existentially bound type variables";
_tag_arr(_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,51));}),_tag_arr(_tmp4C,
sizeof(void*),0));});(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*
_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({struct Cyc_Absyn_Aggr_p_struct
_tmp4F;_tmp4F.tag=5;_tmp4F.f1=({struct Cyc_Absyn_AggrInfo _tmp50;_tmp50.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp51=_cycalloc(sizeof(*_tmp51));
_tmp51[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp52;_tmp52.tag=1;_tmp52.f1=_tmp46;
_tmp52;});_tmp51;}));_tmp50.targs=_tmp9;_tmp50;});_tmp4F.f2=_tmpA;_tmp4F.f3=
_tmpB;_tmp4F.f4=_tmpC;_tmp4F;});_tmp4E;})));}};_pop_handler();}else{void*_tmp44=(
void*)_exn_thrown;void*_tmp54=_tmp44;_LL33: if(_tmp54 != Cyc_Dict_Absent)goto _LL35;
_LL34:({void*_tmp55[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp56="Non-struct name has designator patterns";
_tag_arr(_tmp56,sizeof(char),_get_zero_arr_size(_tmp56,40));}),_tag_arr(_tmp55,
sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));goto _LL32;_LL35:;_LL36:(
void)_throw(_tmp54);_LL32:;}}}return;_LL7: if(_tmp0 <= (void*)2  || *((int*)_tmp0)
!= 14)goto _LL9;_tmpD=((struct Cyc_Absyn_Exp_p_struct*)_tmp0)->f1;_LL8: Cyc_Tcexp_tcExp(
te,0,_tmpD);if(!Cyc_Tcutil_is_const_exp(te,_tmpD)){({void*_tmp57[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp58="non-constant expression in case pattern";_tag_arr(
_tmp58,sizeof(char),_get_zero_arr_size(_tmp58,40));}),_tag_arr(_tmp57,sizeof(
void*),0));});(void*)(p->r=(void*)((void*)0));}{unsigned int _tmp5A;int _tmp5B;
struct _tuple7 _tmp59=Cyc_Evexp_eval_const_uint_exp(_tmpD);_tmp5A=_tmp59.f1;_tmp5B=
_tmp59.f2;(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Int_p_struct*_tmp5C=
_cycalloc(sizeof(*_tmp5C));_tmp5C[0]=({struct Cyc_Absyn_Int_p_struct _tmp5D;_tmp5D.tag=
7;_tmp5D.f1=(void*)((void*)2);_tmp5D.f2=(int)_tmp5A;_tmp5D;});_tmp5C;})));
return;}_LL9:;_LLA: return;_LL0:;}static struct _tagged_arr*Cyc_Tcpat_get_name(
struct Cyc_Absyn_Vardecl*vd){return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(
struct Cyc_List_List*s,void**topt){if(topt != 0)return*topt;return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp5E=_cycalloc(sizeof(*
_tmp5E));_tmp5E->v=s;_tmp5E;}));}static void*Cyc_Tcpat_num_type(void**topt,void*
numt){if(topt != 0  && Cyc_Tcutil_coerceable(*topt))return*topt;{void*_tmp5F=Cyc_Tcutil_compress(
numt);_LL38: if(_tmp5F <= (void*)4  || *((int*)_tmp5F)!= 12)goto _LL3A;_LL39: if(topt
!= 0)return*topt;goto _LL37;_LL3A:;_LL3B: goto _LL37;_LL37:;}return numt;}static void
Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**v_result_ptr,
void*t){(void*)(vd->type=(void*)t);vd->tq=Cyc_Absyn_empty_tqual(0);*v_result_ptr=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp60=_cycalloc(sizeof(*_tmp60));
_tmp60->hd=vd;_tmp60->tl=*v_result_ptr;_tmp60;});}static struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult
res2){struct _tuple6*_tmp62;struct Cyc_List_List*_tmp63;struct Cyc_Tcpat_TcPatResult
_tmp61=res1;_tmp62=_tmp61.tvars_and_bounds_opt;_tmp63=_tmp61.patvars;{struct
_tuple6*_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_Tcpat_TcPatResult _tmp64=res2;
_tmp65=_tmp64.tvars_and_bounds_opt;_tmp66=_tmp64.patvars;if(_tmp62 != 0  || _tmp65
!= 0){if(_tmp62 == 0)_tmp62=({struct _tuple6*_tmp67=_cycalloc(sizeof(*_tmp67));
_tmp67->f1=0;_tmp67->f2=0;_tmp67;});if(_tmp65 == 0)_tmp65=({struct _tuple6*_tmp68=
_cycalloc(sizeof(*_tmp68));_tmp68->f1=0;_tmp68->f2=0;_tmp68;});return({struct Cyc_Tcpat_TcPatResult
_tmp69;_tmp69.tvars_and_bounds_opt=({struct _tuple6*_tmp6A=_cycalloc(sizeof(*
_tmp6A));_tmp6A->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)((*((struct _tuple6*)_check_null(_tmp62))).f1,(*((struct
_tuple6*)_check_null(_tmp65))).f1);_tmp6A->f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple6*)
_check_null(_tmp62))).f2,(*((struct _tuple6*)_check_null(_tmp65))).f2);_tmp6A;});
_tmp69.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp63,_tmp66);_tmp69;});}return({struct Cyc_Tcpat_TcPatResult
_tmp6B;_tmp6B.tvars_and_bounds_opt=0;_tmp6B.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp63,_tmp66);
_tmp6B;});}}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Pat*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->loc=
loc;_tmp6C->topt=0;_tmp6C->r=(void*)((void*)0);_tmp6C;});}struct _tuple9{struct
Cyc_Absyn_Tqual f1;void*f2;};struct _tuple10{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p,void**topt,void**rgn_opt){Cyc_Tcpat_resolve_pat(te,p);{
void*t;struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp13B;
_tmp13B.tvars_and_bounds_opt=0;_tmp13B.patvars=0;_tmp13B;});{void*_tmp6D=(void*)
p->r;struct Cyc_Absyn_Vardecl*_tmp6E;struct Cyc_Absyn_Vardecl*_tmp6F;struct Cyc_Absyn_Tvar*
_tmp70;struct Cyc_Absyn_Vardecl*_tmp71;void*_tmp72;void*_tmp73;void*_tmp74;struct
Cyc_Absyn_Enumdecl*_tmp75;void*_tmp76;struct Cyc_Absyn_Pat*_tmp77;struct Cyc_List_List*
_tmp78;struct Cyc_List_List**_tmp79;int _tmp7A;struct Cyc_Absyn_AggrInfo _tmp7B;void*
_tmp7C;struct Cyc_Absyn_Aggrdecl**_tmp7D;struct Cyc_Absyn_Aggrdecl*_tmp7E;struct
Cyc_List_List*_tmp7F;struct Cyc_List_List**_tmp80;struct Cyc_List_List*_tmp81;
struct Cyc_List_List*_tmp82;struct Cyc_List_List**_tmp83;int _tmp84;struct Cyc_Absyn_Tuniondecl*
_tmp85;struct Cyc_Absyn_Tunionfield*_tmp86;struct Cyc_List_List*_tmp87;struct Cyc_List_List**
_tmp88;int _tmp89;struct Cyc_Absyn_AggrInfo _tmp8A;void*_tmp8B;_LL3D: if((int)_tmp6D
!= 0)goto _LL3F;_LL3E: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);
goto _LL3C;_LL3F: if(_tmp6D <= (void*)2  || *((int*)_tmp6D)!= 0)goto _LL41;_tmp6E=((
struct Cyc_Absyn_Var_p_struct*)_tmp6D)->f1;_LL40: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);Cyc_Tcpat_set_vd(_tmp6E,& res.patvars,t);goto _LL3C;_LL41: if(_tmp6D <= (
void*)2  || *((int*)_tmp6D)!= 1)goto _LL43;_tmp6F=((struct Cyc_Absyn_Reference_p_struct*)
_tmp6D)->f1;_LL42: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);if(
rgn_opt == 0){({void*_tmp8C[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp8D="* pattern would point into an unknown/unallowed region";
_tag_arr(_tmp8D,sizeof(char),_get_zero_arr_size(_tmp8D,55));}),_tag_arr(_tmp8C,
sizeof(void*),0));});goto _LL3C;}Cyc_Tcpat_set_vd(_tmp6F,& res.patvars,(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E[0]=({
struct Cyc_Absyn_PointerType_struct _tmp8F;_tmp8F.tag=4;_tmp8F.f1=({struct Cyc_Absyn_PtrInfo
_tmp90;_tmp90.elt_typ=(void*)t;_tmp90.elt_tq=Cyc_Absyn_empty_tqual(0);_tmp90.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp91;_tmp91.rgn=(void*)*rgn_opt;_tmp91.nullable=Cyc_Absyn_false_conref;
_tmp91.bounds=Cyc_Absyn_empty_conref();_tmp91.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp91.ptrloc=0;_tmp91;});_tmp90;});_tmp8F;});_tmp8E;}));
goto _LL3C;_LL43: if(_tmp6D <= (void*)2  || *((int*)_tmp6D)!= 2)goto _LL45;_tmp70=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp6D)->f1;_tmp71=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp6D)->f2;_LL44: Cyc_Tcpat_set_vd(_tmp71,& res.patvars,(void*)_tmp71->type);Cyc_Tcenv_add_type_vars(
p->loc,te,({struct Cyc_Absyn_Tvar*_tmp92[1];_tmp92[0]=_tmp70;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp92,sizeof(struct Cyc_Absyn_Tvar*),
1));}));if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({struct _tuple6*
_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->f1=0;_tmp93->f2=0;_tmp93;});(*res.tvars_and_bounds_opt).f1=({
struct Cyc_List_List*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->hd=_tmp70;_tmp94->tl=(*
res.tvars_and_bounds_opt).f1;_tmp94;});t=Cyc_Absyn_uint_typ;goto _LL3C;_LL45: if(
_tmp6D <= (void*)2  || *((int*)_tmp6D)!= 7)goto _LL47;_tmp72=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp6D)->f1;if((int)_tmp72 != 1)goto _LL47;_LL46: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);
goto _LL3C;_LL47: if(_tmp6D <= (void*)2  || *((int*)_tmp6D)!= 7)goto _LL49;_tmp73=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp6D)->f1;if((int)_tmp73 != 2)goto _LL49;
_LL48: goto _LL4A;_LL49: if(_tmp6D <= (void*)2  || *((int*)_tmp6D)!= 7)goto _LL4B;
_tmp74=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp6D)->f1;if((int)_tmp74 != 0)
goto _LL4B;_LL4A: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL3C;_LL4B:
if(_tmp6D <= (void*)2  || *((int*)_tmp6D)!= 8)goto _LL4D;_LL4C: t=Cyc_Tcpat_num_type(
topt,Cyc_Absyn_char_typ);goto _LL3C;_LL4D: if(_tmp6D <= (void*)2  || *((int*)_tmp6D)
!= 9)goto _LL4F;_LL4E: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ);goto _LL3C;
_LL4F: if(_tmp6D <= (void*)2  || *((int*)_tmp6D)!= 10)goto _LL51;_tmp75=((struct Cyc_Absyn_Enum_p_struct*)
_tmp6D)->f1;_LL50: t=Cyc_Tcpat_num_type(topt,(void*)({struct Cyc_Absyn_EnumType_struct*
_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95[0]=({struct Cyc_Absyn_EnumType_struct
_tmp96;_tmp96.tag=12;_tmp96.f1=_tmp75->name;_tmp96.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp75;_tmp96;});_tmp95;}));goto _LL3C;_LL51: if(_tmp6D <= (void*)2  || *((int*)
_tmp6D)!= 11)goto _LL53;_tmp76=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp6D)->f1;
_LL52: t=Cyc_Tcpat_num_type(topt,_tmp76);goto _LL3C;_LL53: if((int)_tmp6D != 1)goto
_LL55;_LL54: if(topt != 0){void*_tmp97=Cyc_Tcutil_compress(*topt);_LL66: if(_tmp97
<= (void*)4  || *((int*)_tmp97)!= 4)goto _LL68;_LL67: t=*topt;goto tcpat_end;_LL68:;
_LL69: goto _LL65;_LL65:;}{struct Cyc_List_List*_tmp98=Cyc_Tcenv_lookup_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp99=_cycalloc(sizeof(*
_tmp99));_tmp99[0]=({struct Cyc_Absyn_PointerType_struct _tmp9A;_tmp9A.tag=4;
_tmp9A.f1=({struct Cyc_Absyn_PtrInfo _tmp9B;_tmp9B.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp9E=_cycalloc(sizeof(*
_tmp9E));_tmp9E->v=_tmp98;_tmp9E;}));_tmp9B.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp9B.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp9C;_tmp9C.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp9D=_cycalloc(sizeof(*
_tmp9D));_tmp9D->v=_tmp98;_tmp9D;}));_tmp9C.nullable=Cyc_Absyn_true_conref;
_tmp9C.bounds=Cyc_Absyn_empty_conref();_tmp9C.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp9C.ptrloc=0;_tmp9C;});_tmp9B;});_tmp9A;});_tmp99;});
goto _LL3C;}_LL55: if(_tmp6D <= (void*)2  || *((int*)_tmp6D)!= 4)goto _LL57;_tmp77=((
struct Cyc_Absyn_Pointer_p_struct*)_tmp6D)->f1;_LL56: {void*inner_typ=(void*)0;
void**_tmp9F=0;if(topt != 0){void*_tmpA0=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmpA1;void*_tmpA2;_LL6B: if(_tmpA0 <= (void*)4  || *((int*)_tmpA0)!= 4)goto _LL6D;
_tmpA1=((struct Cyc_Absyn_PointerType_struct*)_tmpA0)->f1;_tmpA2=(void*)_tmpA1.elt_typ;
_LL6C: inner_typ=_tmpA2;_tmp9F=(void**)& inner_typ;goto _LL6A;_LL6D:;_LL6E: goto
_LL6A;_LL6A:;}{void*ptr_rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpB7;}));res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp77,
_tmp9F,(void**)& ptr_rgn));if(_tmp9F == 0){void*_tmpA3=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp77->topt))->v);struct Cyc_Absyn_TunionFieldInfo
_tmpA4;void*_tmpA5;struct Cyc_Absyn_Tuniondecl*_tmpA6;struct Cyc_Absyn_Tunionfield*
_tmpA7;struct Cyc_List_List*_tmpA8;_LL70: if(_tmpA3 <= (void*)4  || *((int*)_tmpA3)
!= 3)goto _LL72;_tmpA4=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpA3)->f1;
_tmpA5=(void*)_tmpA4.field_info;if(*((int*)_tmpA5)!= 1)goto _LL72;_tmpA6=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmpA5)->f1;_tmpA7=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpA5)->f2;_tmpA8=_tmpA4.targs;_LL71: t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_TunionType_struct
_tmpAA;_tmpAA.tag=2;_tmpAA.f1=({struct Cyc_Absyn_TunionInfo _tmpAB;_tmpAB.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmpAC=_cycalloc(sizeof(*
_tmpAC));_tmpAC[0]=({struct Cyc_Absyn_KnownTunion_struct _tmpAD;_tmpAD.tag=1;
_tmpAD.f1=({struct Cyc_Absyn_Tuniondecl**_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE[
0]=_tmpA6;_tmpAE;});_tmpAD;});_tmpAC;}));_tmpAB.targs=_tmpA8;_tmpAB.rgn=(void*)
ptr_rgn;_tmpAB;});_tmpAA;});_tmpA9;});goto _LL6F;_LL72:;_LL73: t=(void*)({struct
Cyc_Absyn_PointerType_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({
struct Cyc_Absyn_PointerType_struct _tmpB0;_tmpB0.tag=4;_tmpB0.f1=({struct Cyc_Absyn_PtrInfo
_tmpB1;_tmpB1.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp77->topt))->v);
_tmpB1.elt_tq=Cyc_Absyn_empty_tqual(0);_tmpB1.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpB2;_tmpB2.rgn=(void*)ptr_rgn;_tmpB2.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpB2.bounds=Cyc_Absyn_empty_conref();_tmpB2.zero_term=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpB2.ptrloc=0;_tmpB2;});
_tmpB1;});_tmpB0;});_tmpAF;});goto _LL6F;_LL6F:;}else{t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_PointerType_struct
_tmpB4;_tmpB4.tag=4;_tmpB4.f1=({struct Cyc_Absyn_PtrInfo _tmpB5;_tmpB5.elt_typ=(
void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp77->topt))->v);_tmpB5.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmpB5.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpB6;_tmpB6.rgn=(
void*)ptr_rgn;_tmpB6.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmpB6.bounds=Cyc_Absyn_empty_conref();_tmpB6.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpB6.ptrloc=0;_tmpB6;});_tmpB5;});_tmpB4;});_tmpB3;});}
goto _LL3C;}}_LL57: if(_tmp6D <= (void*)2  || *((int*)_tmp6D)!= 3)goto _LL59;_tmp78=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp6D)->f1;_tmp79=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Tuple_p_struct*)_tmp6D)->f1;_tmp7A=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp6D)->f2;_LL58: {struct Cyc_List_List*_tmpB8=*_tmp79;struct Cyc_List_List*
pat_ts=0;struct Cyc_List_List*topt_ts=0;if(topt != 0){void*_tmpB9=Cyc_Tcutil_compress(*
topt);struct Cyc_List_List*_tmpBA;_LL75: if(_tmpB9 <= (void*)4  || *((int*)_tmpB9)!= 
9)goto _LL77;_tmpBA=((struct Cyc_Absyn_TupleType_struct*)_tmpB9)->f1;_LL76: topt_ts=
_tmpBA;if(_tmp7A){int _tmpBB=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpB8);int _tmpBC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBA);if(
_tmpBB < _tmpBC){struct Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmpBC - _tmpBB;i
++){wild_ps=({struct Cyc_List_List*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->hd=
Cyc_Tcpat_wild_pat(p->loc);_tmpBD->tl=wild_ps;_tmpBD;});}}*_tmp79=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpB8,wild_ps);
_tmpB8=*_tmp79;}else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB8)
== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBA))({void*_tmpBE[0]={};
Cyc_Tcutil_warn(p->loc,({const char*_tmpBF="unnecessary ... in tuple pattern";
_tag_arr(_tmpBF,sizeof(char),_get_zero_arr_size(_tmpBF,33));}),_tag_arr(_tmpBE,
sizeof(void*),0));});}}goto _LL74;_LL77:;_LL78: goto _LL74;_LL74:;}else{if(_tmp7A)({
void*_tmpC0[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmpC1="cannot determine missing fields for ... in tuple pattern";
_tag_arr(_tmpC1,sizeof(char),_get_zero_arr_size(_tmpC1,57));}),_tag_arr(_tmpC0,
sizeof(void*),0));});}for(0;_tmpB8 != 0;_tmpB8=_tmpB8->tl){void**_tmpC2=0;if(
topt_ts != 0){_tmpC2=(void**)&(*((struct _tuple9*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)
_tmpB8->hd,_tmpC2,rgn_opt));pat_ts=({struct Cyc_List_List*_tmpC3=_cycalloc(
sizeof(*_tmpC3));_tmpC3->hd=({struct _tuple9*_tmpC4=_cycalloc(sizeof(*_tmpC4));
_tmpC4->f1=Cyc_Absyn_empty_tqual(0);_tmpC4->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Pat*)_tmpB8->hd)->topt))->v;_tmpC4;});_tmpC3->tl=
pat_ts;_tmpC3;});}t=(void*)({struct Cyc_Absyn_TupleType_struct*_tmpC5=_cycalloc(
sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_TupleType_struct _tmpC6;_tmpC6.tag=9;
_tmpC6.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
pat_ts);_tmpC6;});_tmpC5;});goto _LL3C;}_LL59: if(_tmp6D <= (void*)2  || *((int*)
_tmp6D)!= 5)goto _LL5B;_tmp7B=((struct Cyc_Absyn_Aggr_p_struct*)_tmp6D)->f1;_tmp7C=(
void*)_tmp7B.aggr_info;if(*((int*)_tmp7C)!= 1)goto _LL5B;_tmp7D=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp7C)->f1;_tmp7E=*_tmp7D;_tmp7F=_tmp7B.targs;_tmp80=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_Aggr_p_struct*)_tmp6D)->f1).targs;_tmp81=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp6D)->f2;_tmp82=((struct Cyc_Absyn_Aggr_p_struct*)_tmp6D)->f3;_tmp83=(struct
Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_struct*)_tmp6D)->f3;_tmp84=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp6D)->f4;_LL5A: {struct Cyc_List_List*_tmpC7=*_tmp83;if(_tmp7E->impl == 0){({
void*_tmpC8[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmpC9="can't destructure an abstract struct";
_tag_arr(_tmpC9,sizeof(char),_get_zero_arr_size(_tmpC9,37));}),_tag_arr(_tmpC8,
sizeof(void*),0));});t=Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpCA=_cycalloc(
sizeof(*_tmpCA));_tmpCA->v=Cyc_Tcenv_lookup_type_vars(te);_tmpCA;}));goto _LL3C;}{
struct _RegionHandle _tmpCB=_new_region("rgn");struct _RegionHandle*rgn=& _tmpCB;
_push_region(rgn);{struct Cyc_List_List*_tmpCC=0;struct Cyc_List_List*
outlives_constraints=0;struct Cyc_List_List*_tmpCD=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7E->impl))->exist_vars;{struct Cyc_List_List*t=_tmp81;for(0;t != 0;
t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;struct Cyc_Absyn_Tvar*
uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpCD))->hd;_tmpCD=
_tmpCD->tl;{void*_tmpCE=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpCF=Cyc_Absyn_compress_kb((
void*)uv->kind);int error=0;void*k2;{void*_tmpD0=_tmpCF;void*_tmpD1;void*_tmpD2;
_LL7A: if(*((int*)_tmpD0)!= 2)goto _LL7C;_tmpD1=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpD0)->f2;_LL7B: _tmpD2=_tmpD1;goto _LL7D;_LL7C: if(*((int*)_tmpD0)!= 0)goto _LL7E;
_tmpD2=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpD0)->f1;_LL7D: k2=_tmpD2;goto
_LL79;_LL7E:;_LL7F:({void*_tmpD3[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpD4="unconstrained existential type variable in struct";
_tag_arr(_tmpD4,sizeof(char),_get_zero_arr_size(_tmpD4,50));}),_tag_arr(_tmpD3,
sizeof(void*),0));});_LL79:;}{void*_tmpD5=_tmpCE;void*_tmpD6;struct Cyc_Core_Opt*
_tmpD7;struct Cyc_Core_Opt**_tmpD8;void*_tmpD9;struct Cyc_Core_Opt*_tmpDA;struct
Cyc_Core_Opt**_tmpDB;_LL81: if(*((int*)_tmpD5)!= 0)goto _LL83;_tmpD6=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmpD5)->f1;_LL82: if(!Cyc_Tcutil_kind_leq(k2,
_tmpD6))error=1;goto _LL80;_LL83: if(*((int*)_tmpD5)!= 2)goto _LL85;_tmpD7=((struct
Cyc_Absyn_Less_kb_struct*)_tmpD5)->f1;_tmpD8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpD5)->f1;_tmpD9=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpD5)->f2;_LL84:
_tmpDB=_tmpD8;goto _LL86;_LL85: if(*((int*)_tmpD5)!= 1)goto _LL80;_tmpDA=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmpD5)->f1;_tmpDB=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Unknown_kb_struct*)_tmpD5)->f1;_LL86:*_tmpDB=({struct Cyc_Core_Opt*
_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->v=(void*)_tmpCF;_tmpDC;});goto _LL80;
_LL80:;}if(error)({struct Cyc_String_pa_struct _tmpE1;_tmpE1.tag=0;_tmpE1.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(k2));{struct Cyc_String_pa_struct
_tmpE0;_tmpE0.tag=0;_tmpE0.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kindbound2string(
_tmpCE));{struct Cyc_String_pa_struct _tmpDF;_tmpDF.tag=0;_tmpDF.f1=(struct
_tagged_arr)((struct _tagged_arr)*tv->name);{void*_tmpDD[3]={& _tmpDF,& _tmpE0,&
_tmpE1};Cyc_Tcutil_terr(p->loc,({const char*_tmpDE="type variable %s has kind %s but must have at least kind %s";
_tag_arr(_tmpDE,sizeof(char),_get_zero_arr_size(_tmpDE,60));}),_tag_arr(_tmpDD,
sizeof(void*),3));}}}});{void*vartype=(void*)({struct Cyc_Absyn_VarType_struct*
_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5[0]=({struct Cyc_Absyn_VarType_struct
_tmpE6;_tmpE6.tag=1;_tmpE6.f1=tv;_tmpE6;});_tmpE5;});_tmpCC=({struct Cyc_List_List*
_tmpE2=_region_malloc(rgn,sizeof(*_tmpE2));_tmpE2->hd=(void*)((void*)vartype);
_tmpE2->tl=_tmpCC;_tmpE2;});if(k2 == (void*)3)outlives_constraints=({struct Cyc_List_List*
_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->hd=({struct _tuple0*_tmpE4=_cycalloc(
sizeof(*_tmpE4));_tmpE4->f1=Cyc_Absyn_empty_effect;_tmpE4->f2=vartype;_tmpE4;});
_tmpE3->tl=outlives_constraints;_tmpE3;});}}}}_tmpCC=Cyc_List_imp_rev(_tmpCC);{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,_tmp81);struct Cyc_List_List*
_tmpE7=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple4 _tmpE8=({struct _tuple4
_tmp109;_tmp109.f1=_tmpE7;_tmp109.f2=rgn;_tmp109;});struct Cyc_List_List*_tmpE9=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmpE8,_tmp7E->tvs);struct Cyc_List_List*_tmpEA=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7E->impl))->exist_vars,_tmpCC);struct Cyc_List_List*_tmpEB=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple5*))Cyc_Core_snd,_tmpE9);struct Cyc_List_List*_tmpEC=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple5*))Cyc_Core_snd,_tmpEA);struct Cyc_List_List*_tmpED=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(rgn,_tmpE9,_tmpEA);if(_tmp81 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7E->impl))->rgn_po != 0){if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({
struct _tuple6*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->f1=0;_tmpEE->f2=0;_tmpEE;});(*
res.tvars_and_bounds_opt).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,_tmp81);(*
res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmpEF=0;{struct Cyc_List_List*_tmpF0=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7E->impl))->rgn_po;for(0;_tmpF0 != 
0;_tmpF0=_tmpF0->tl){_tmpEF=({struct Cyc_List_List*_tmpF1=_cycalloc(sizeof(*
_tmpF1));_tmpF1->hd=({struct _tuple0*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->f1=
Cyc_Tcutil_rsubstitute(rgn,_tmpED,(*((struct _tuple0*)_tmpF0->hd)).f1);_tmpF2->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmpED,(*((struct _tuple0*)_tmpF0->hd)).f2);_tmpF2;});
_tmpF1->tl=_tmpEF;_tmpF1;});}}_tmpEF=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmpEF);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
_tmpEF);}}*_tmp80=_tmpEB;t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmpF3=
_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({struct Cyc_Absyn_AggrType_struct _tmpF4;
_tmpF4.tag=10;_tmpF4.f1=({struct Cyc_Absyn_AggrInfo _tmpF5;_tmpF5.aggr_info=(void*)((
void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));
_tmpF6[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpF7;_tmpF7.tag=1;_tmpF7.f1=({
struct Cyc_Absyn_Aggrdecl**_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8[0]=_tmp7E;
_tmpF8;});_tmpF7;});_tmpF6;}));_tmpF5.targs=*_tmp80;_tmpF5;});_tmpF4;});_tmpF3;});
if(_tmp84){int _tmpF9=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC7);
int _tmpFA=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7E->impl))->fields);if(_tmpF9 < _tmpFA){struct Cyc_List_List*
wild_dps=0;{int i=0;for(0;i < _tmpFA - _tmpF9;i ++){wild_dps=({struct Cyc_List_List*
_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->hd=({struct _tuple8*_tmpFC=_cycalloc(
sizeof(*_tmpFC));_tmpFC->f1=0;_tmpFC->f2=Cyc_Tcpat_wild_pat(p->loc);_tmpFC;});
_tmpFB->tl=wild_dps;_tmpFB;});}}*_tmp83=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC7,wild_dps);_tmpC7=*_tmp83;}
else{if(_tmpF9 == _tmpFA)({void*_tmpFD[0]={};Cyc_Tcutil_warn(p->loc,({const char*
_tmpFE="unnecessary ... in struct pattern";_tag_arr(_tmpFE,sizeof(char),
_get_zero_arr_size(_tmpFE,34));}),_tag_arr(_tmpFD,sizeof(void*),0));});}}{struct
Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,p->loc,_tmpC7,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7E->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp100;struct Cyc_Absyn_Aggrfield*
_tmp101;struct Cyc_Absyn_Pat*_tmp102;struct _tuple10*_tmpFF=(struct _tuple10*)
fields->hd;_tmp100=*_tmpFF;_tmp101=_tmp100.f1;_tmp102=_tmp100.f2;{void*_tmp103=
Cyc_Tcutil_rsubstitute(rgn,_tmpED,(void*)_tmp101->type);res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te2,_tmp102,(void**)& _tmp103,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp102->topt))->v,_tmp103))({struct Cyc_String_pa_struct
_tmp108;_tmp108.tag=0;_tmp108.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp102->topt))->v));{struct Cyc_String_pa_struct
_tmp107;_tmp107.tag=0;_tmp107.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp103));{struct Cyc_String_pa_struct _tmp106;_tmp106.tag=0;_tmp106.f1=(struct
_tagged_arr)((struct _tagged_arr)*_tmp101->name);{void*_tmp104[3]={& _tmp106,&
_tmp107,& _tmp108};Cyc_Tcutil_terr(p->loc,({const char*_tmp105="field %s of struct pattern expects type %s != %s";
_tag_arr(_tmp105,sizeof(char),_get_zero_arr_size(_tmp105,49));}),_tag_arr(
_tmp104,sizeof(void*),3));}}}});}}}}};_pop_region(rgn);}goto _LL3C;}_LL5B: if(
_tmp6D <= (void*)2  || *((int*)_tmp6D)!= 6)goto _LL5D;_tmp85=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp6D)->f1;_tmp86=((struct Cyc_Absyn_Tunion_p_struct*)_tmp6D)->f2;_tmp87=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp6D)->f3;_tmp88=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Tunion_p_struct*)_tmp6D)->f3;_tmp89=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp6D)->f4;_LL5C: {struct Cyc_List_List*_tmp10A=*_tmp88;{struct _RegionHandle
_tmp10B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp10B;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmp86->typs;struct Cyc_List_List*_tmp10C=Cyc_Tcenv_lookup_type_vars(
te);struct _tuple4 _tmp10D=({struct _tuple4 _tmp138;_tmp138.f1=_tmp10C;_tmp138.f2=
rgn;_tmp138;});struct Cyc_List_List*_tmp10E=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp10D,_tmp85->tvs);struct Cyc_List_List*_tmp10F=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp10E);if(tqts == 0)t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110[0]=({struct Cyc_Absyn_TunionType_struct
_tmp111;_tmp111.tag=2;_tmp111.f1=({struct Cyc_Absyn_TunionInfo _tmp112;_tmp112.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmp114=_cycalloc(sizeof(*
_tmp114));_tmp114[0]=({struct Cyc_Absyn_KnownTunion_struct _tmp115;_tmp115.tag=1;
_tmp115.f1=({struct Cyc_Absyn_Tuniondecl**_tmp116=_cycalloc(sizeof(*_tmp116));
_tmp116[0]=_tmp85;_tmp116;});_tmp115;});_tmp114;}));_tmp112.targs=_tmp10F;
_tmp112.rgn=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->v=_tmp10C;
_tmp113;}));_tmp112;});_tmp111;});_tmp110;});else{t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp118;_tmp118.tag=3;_tmp118.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp119;
_tmp119.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp11B;_tmp11B.tag=1;_tmp11B.f1=_tmp85;_tmp11B.f2=_tmp86;_tmp11B;});_tmp11A;}));
_tmp119.targs=_tmp10F;_tmp119;});_tmp118;});_tmp117;});}if(topt != 0  && tqts == 0){
void*_tmp11C=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo _tmp11D;struct
Cyc_List_List*_tmp11E;_LL88: if(_tmp11C <= (void*)4  || *((int*)_tmp11C)!= 3)goto
_LL8A;_LL89: t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp11F=_cycalloc(
sizeof(*_tmp11F));_tmp11F[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp120;
_tmp120.tag=3;_tmp120.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp121;_tmp121.field_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmp122=_cycalloc(
sizeof(*_tmp122));_tmp122[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmp123;
_tmp123.tag=1;_tmp123.f1=_tmp85;_tmp123.f2=_tmp86;_tmp123;});_tmp122;}));_tmp121.targs=
_tmp10F;_tmp121;});_tmp120;});_tmp11F;});goto _LL87;_LL8A: if(_tmp11C <= (void*)4
 || *((int*)_tmp11C)!= 2)goto _LL8C;_tmp11D=((struct Cyc_Absyn_TunionType_struct*)
_tmp11C)->f1;_tmp11E=_tmp11D.targs;_LL8B: {struct Cyc_List_List*_tmp124=_tmp10F;
for(0;_tmp124 != 0  && _tmp11E != 0;(_tmp124=_tmp124->tl,_tmp11E=_tmp11E->tl)){Cyc_Tcutil_unify((
void*)_tmp124->hd,(void*)_tmp11E->hd);}goto _LL87;}_LL8C:;_LL8D: goto _LL87;_LL87:;}
if(_tmp89){int _tmp125=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp10A);
int _tmp126=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);if(_tmp125 < 
_tmp126){struct Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmp126 - _tmp125;i ++){
wild_ps=({struct Cyc_List_List*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->hd=
Cyc_Tcpat_wild_pat(p->loc);_tmp127->tl=wild_ps;_tmp127;});}}*_tmp88=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp10A,
wild_ps);_tmp10A=*_tmp88;}else{if(_tmp125 == _tmp126)({struct Cyc_String_pa_struct
_tmp12A;_tmp12A.tag=0;_tmp12A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp85->name));{void*_tmp128[1]={& _tmp12A};Cyc_Tcutil_warn(p->loc,({const char*
_tmp129="unnecessary ... in tunion field %s";_tag_arr(_tmp129,sizeof(char),
_get_zero_arr_size(_tmp129,35));}),_tag_arr(_tmp128,sizeof(void*),1));}});}}for(
0;_tmp10A != 0  && tqts != 0;(_tmp10A=_tmp10A->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*
_tmp12B=(struct Cyc_Absyn_Pat*)_tmp10A->hd;void*_tmp12C=Cyc_Tcutil_rsubstitute(
rgn,_tmp10E,(*((struct _tuple9*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(res,
Cyc_Tcpat_tcPatRec(te,_tmp12B,(void**)& _tmp12C,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp12B->topt))->v,_tmp12C))({struct Cyc_String_pa_struct
_tmp131;_tmp131.tag=0;_tmp131.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp12B->topt))->v));{struct Cyc_String_pa_struct
_tmp130;_tmp130.tag=0;_tmp130.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp12C));{struct Cyc_String_pa_struct _tmp12F;_tmp12F.tag=0;_tmp12F.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp86->name));{void*
_tmp12D[3]={& _tmp12F,& _tmp130,& _tmp131};Cyc_Tcutil_terr(_tmp12B->loc,({const char*
_tmp12E="%s expects argument type %s, not %s";_tag_arr(_tmp12E,sizeof(char),
_get_zero_arr_size(_tmp12E,36));}),_tag_arr(_tmp12D,sizeof(void*),3));}}}});}if(
_tmp10A != 0)({struct Cyc_String_pa_struct _tmp134;_tmp134.tag=0;_tmp134.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp86->name));{void*
_tmp132[1]={& _tmp134};Cyc_Tcutil_terr(p->loc,({const char*_tmp133="too many arguments for tunion constructor %s";
_tag_arr(_tmp133,sizeof(char),_get_zero_arr_size(_tmp133,45));}),_tag_arr(
_tmp132,sizeof(void*),1));}});if(tqts != 0)({struct Cyc_String_pa_struct _tmp137;
_tmp137.tag=0;_tmp137.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp86->name));{void*_tmp135[1]={& _tmp137};Cyc_Tcutil_terr(p->loc,({const char*
_tmp136="too few arguments for tunion constructor %s";_tag_arr(_tmp136,sizeof(
char),_get_zero_arr_size(_tmp136,44));}),_tag_arr(_tmp135,sizeof(void*),1));}});};
_pop_region(rgn);}goto _LL3C;}_LL5D: if(_tmp6D <= (void*)2  || *((int*)_tmp6D)!= 5)
goto _LL5F;_tmp8A=((struct Cyc_Absyn_Aggr_p_struct*)_tmp6D)->f1;_tmp8B=(void*)
_tmp8A.aggr_info;if(*((int*)_tmp8B)!= 0)goto _LL5F;_LL5E: goto _LL60;_LL5F: if(
_tmp6D <= (void*)2  || *((int*)_tmp6D)!= 12)goto _LL61;_LL60: goto _LL62;_LL61: if(
_tmp6D <= (void*)2  || *((int*)_tmp6D)!= 14)goto _LL63;_LL62: goto _LL64;_LL63: if(
_tmp6D <= (void*)2  || *((int*)_tmp6D)!= 13)goto _LL3C;_LL64: t=Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp139;}));goto _LL3C;_LL3C:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmp13A=
_cycalloc(sizeof(*_tmp13A));_tmp13A->v=(void*)t;_tmp13A;});return res;}}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp13C=Cyc_Tcpat_tcPatRec(te,p,topt,0);{struct
_RegionHandle _tmp13D=_new_region("r");struct _RegionHandle*r=& _tmp13D;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tagged_arr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp13C.patvars),p->loc,({const char*
_tmp13E="pattern contains a repeated variable";_tag_arr(_tmp13E,sizeof(char),
_get_zero_arr_size(_tmp13E,37));}));;_pop_region(r);}return _tmp13C;}void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*_tmp13F=(void*)p->r;struct
Cyc_Absyn_Pat*_tmp140;struct Cyc_List_List*_tmp141;struct Cyc_List_List*_tmp142;
struct Cyc_List_List*_tmp143;_LL8F: if(_tmp13F <= (void*)2  || *((int*)_tmp13F)!= 4)
goto _LL91;_tmp140=((struct Cyc_Absyn_Pointer_p_struct*)_tmp13F)->f1;_LL90: Cyc_Tcpat_check_pat_regions(
te,_tmp140);{void*_tmp144=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_PtrInfo _tmp145;struct Cyc_Absyn_PtrAtts _tmp146;void*_tmp147;
struct Cyc_Absyn_TunionInfo _tmp148;void*_tmp149;_LL9A: if(_tmp144 <= (void*)4  || *((
int*)_tmp144)!= 4)goto _LL9C;_tmp145=((struct Cyc_Absyn_PointerType_struct*)
_tmp144)->f1;_tmp146=_tmp145.ptr_atts;_tmp147=(void*)_tmp146.rgn;_LL9B: _tmp149=
_tmp147;goto _LL9D;_LL9C: if(_tmp144 <= (void*)4  || *((int*)_tmp144)!= 2)goto _LL9E;
_tmp148=((struct Cyc_Absyn_TunionType_struct*)_tmp144)->f1;_tmp149=(void*)_tmp148.rgn;
_LL9D: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp149);return;_LL9E:;_LL9F:({
void*_tmp14A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp14B="check_pat_regions: bad pointer type";_tag_arr(_tmp14B,sizeof(
char),_get_zero_arr_size(_tmp14B,36));}),_tag_arr(_tmp14A,sizeof(void*),0));});
_LL99:;}_LL91: if(_tmp13F <= (void*)2  || *((int*)_tmp13F)!= 6)goto _LL93;_tmp141=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp13F)->f3;_LL92: for(0;_tmp141 != 0;_tmp141=
_tmp141->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)_tmp141->hd);}{
void*_tmp14C=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;struct Cyc_Absyn_TunionInfo
_tmp14D;void*_tmp14E;_LLA1: if(_tmp14C <= (void*)4  || *((int*)_tmp14C)!= 2)goto
_LLA3;_tmp14D=((struct Cyc_Absyn_TunionType_struct*)_tmp14C)->f1;_tmp14E=(void*)
_tmp14D.rgn;_LLA2: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp14E);return;_LLA3:
if(_tmp14C <= (void*)4  || *((int*)_tmp14C)!= 3)goto _LLA5;_LLA4: return;_LLA5:;
_LLA6:({void*_tmp14F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp150="check_pat_regions: bad tunion type";
_tag_arr(_tmp150,sizeof(char),_get_zero_arr_size(_tmp150,35));}),_tag_arr(
_tmp14F,sizeof(void*),0));});_LLA0:;}_LL93: if(_tmp13F <= (void*)2  || *((int*)
_tmp13F)!= 5)goto _LL95;_tmp142=((struct Cyc_Absyn_Aggr_p_struct*)_tmp13F)->f3;
_LL94: for(0;_tmp142 != 0;_tmp142=_tmp142->tl){Cyc_Tcpat_check_pat_regions(te,(*((
struct _tuple8*)_tmp142->hd)).f2);}return;_LL95: if(_tmp13F <= (void*)2  || *((int*)
_tmp13F)!= 3)goto _LL97;_tmp143=((struct Cyc_Absyn_Tuple_p_struct*)_tmp13F)->f1;
_LL96: for(0;_tmp143 != 0;_tmp143=_tmp143->tl){Cyc_Tcpat_check_pat_regions(te,(
struct Cyc_Absyn_Pat*)_tmp143->hd);}return;_LL97:;_LL98: return;_LL8E:;}struct Cyc_Tcpat_Name_v_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};struct
Cyc_Tcpat_Con_s{void*name;int arity;struct Cyc_Core_Opt*span;struct Cyc_Absyn_Pat*
orig_pat;};struct Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*
f2;};static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*
c2){void*_tmp151=(void*)c1->name;struct _tagged_arr _tmp152;int _tmp153;_LLA8: if(*((
int*)_tmp151)!= 0)goto _LLAA;_tmp152=((struct Cyc_Tcpat_Name_v_struct*)_tmp151)->f1;
_LLA9: {void*_tmp154=(void*)c2->name;struct _tagged_arr _tmp155;_LLAD: if(*((int*)
_tmp154)!= 0)goto _LLAF;_tmp155=((struct Cyc_Tcpat_Name_v_struct*)_tmp154)->f1;
_LLAE: return Cyc_strcmp((struct _tagged_arr)_tmp152,(struct _tagged_arr)_tmp155);
_LLAF: if(*((int*)_tmp154)!= 1)goto _LLAC;_LLB0: return - 1;_LLAC:;}_LLAA: if(*((int*)
_tmp151)!= 1)goto _LLA7;_tmp153=((struct Cyc_Tcpat_Int_v_struct*)_tmp151)->f1;
_LLAB: {void*_tmp156=(void*)c2->name;int _tmp157;_LLB2: if(*((int*)_tmp156)!= 0)
goto _LLB4;_LLB3: return 1;_LLB4: if(*((int*)_tmp156)!= 1)goto _LLB1;_tmp157=((struct
Cyc_Tcpat_Int_v_struct*)_tmp156)->f1;_LLB5: return _tmp153 - _tmp157;_LLB1:;}_LLA7:;}
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){return((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,
struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}static struct
Cyc_Core_Opt Cyc_Tcpat_one_opt={(void*)1};static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={(
void*)2};static struct Cyc_Core_Opt*Cyc_Tcpat_one_opt_ptr=(struct Cyc_Core_Opt*)&
Cyc_Tcpat_one_opt;static char _tmp159[5]="NULL";static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_null_name_value={0,{_tmp159,_tmp159,_tmp159 + 5}};static char _tmp15B[2]="&";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={0,{_tmp15B,_tmp15B,
_tmp15B + 2}};static char _tmp15D[2]="$";static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={
0,{_tmp15D,_tmp15D,_tmp15D + 2}};static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(
struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp15E=_cycalloc(sizeof(*
_tmp15E));_tmp15E->name=(void*)((void*)& Cyc_Tcpat_null_name_value);_tmp15E->arity=
0;_tmp15E->span=(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt;_tmp15E->orig_pat=(
struct Cyc_Absyn_Pat*)p;_tmp15E;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(
struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp15F=_cycalloc(sizeof(*
_tmp15F));_tmp15F->name=(void*)((void*)& Cyc_Tcpat_ptr_name_value);_tmp15F->arity=
1;_tmp15F->span=(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt;_tmp15F->orig_pat=(
struct Cyc_Absyn_Pat*)p;_tmp15F;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(
struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp160=_cycalloc(sizeof(*
_tmp160));_tmp160->name=(void*)((void*)& Cyc_Tcpat_ptr_name_value);_tmp160->arity=
1;_tmp160->span=(struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;_tmp160->orig_pat=(
struct Cyc_Absyn_Pat*)p;_tmp160;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(
int i,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp161=_cycalloc(
sizeof(*_tmp161));_tmp161->name=(void*)((void*)({struct Cyc_Tcpat_Int_v_struct*
_tmp162=_cycalloc_atomic(sizeof(*_tmp162));_tmp162[0]=({struct Cyc_Tcpat_Int_v_struct
_tmp163;_tmp163.tag=1;_tmp163.f1=i;_tmp163;});_tmp162;}));_tmp161->arity=0;
_tmp161->span=0;_tmp161->orig_pat=p;_tmp161;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(
struct _tagged_arr f,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp164=
_cycalloc(sizeof(*_tmp164));_tmp164->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*
_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165[0]=({struct Cyc_Tcpat_Name_v_struct
_tmp166;_tmp166.tag=0;_tmp166.f1=f;_tmp166;});_tmp165;}));_tmp164->arity=0;
_tmp164->span=0;_tmp164->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp164;});}static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){return({
struct Cyc_Tcpat_Con_s*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167->name=(void*)((
void*)({struct Cyc_Tcpat_Int_v_struct*_tmp169=_cycalloc_atomic(sizeof(*_tmp169));
_tmp169[0]=({struct Cyc_Tcpat_Int_v_struct _tmp16A;_tmp16A.tag=1;_tmp16A.f1=(int)c;
_tmp16A;});_tmp169;}));_tmp167->arity=0;_tmp167->span=({struct Cyc_Core_Opt*
_tmp168=_cycalloc_atomic(sizeof(*_tmp168));_tmp168->v=(void*)256;_tmp168;});
_tmp167->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp167;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con(int i,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->name=(void*)((void*)& Cyc_Tcpat_tuple_name_value);
_tmp16B->arity=i;_tmp16B->span=Cyc_Tcpat_one_opt_ptr;_tmp16B->orig_pat=p;_tmp16B;});}
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C[0]=({struct Cyc_Tcpat_Con_struct
_tmp16D;_tmp16D.tag=0;_tmp16D.f1=Cyc_Tcpat_null_con(p);_tmp16D.f2=0;_tmp16D;});
_tmp16C;});}static void*Cyc_Tcpat_int_pat(int i,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E[0]=({
struct Cyc_Tcpat_Con_struct _tmp16F;_tmp16F.tag=0;_tmp16F.f1=Cyc_Tcpat_int_con(i,p);
_tmp16F.f2=0;_tmp16F;});_tmp16E;});}static void*Cyc_Tcpat_char_pat(char c,struct
Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp170=_cycalloc(
sizeof(*_tmp170));_tmp170[0]=({struct Cyc_Tcpat_Con_struct _tmp171;_tmp171.tag=0;
_tmp171.f1=Cyc_Tcpat_char_con(c,p);_tmp171.f2=0;_tmp171;});_tmp170;});}static
void*Cyc_Tcpat_float_pat(struct _tagged_arr f,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172[0]=({
struct Cyc_Tcpat_Con_struct _tmp173;_tmp173.tag=0;_tmp173.f1=Cyc_Tcpat_float_con(f,
p);_tmp173.f2=0;_tmp173;});_tmp172;});}static void*Cyc_Tcpat_null_ptr_pat(void*p,
struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp174=
_cycalloc(sizeof(*_tmp174));_tmp174[0]=({struct Cyc_Tcpat_Con_struct _tmp175;
_tmp175.tag=0;_tmp175.f1=Cyc_Tcpat_null_ptr_con(p0);_tmp175.f2=({struct Cyc_List_List*
_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->hd=(void*)p;_tmp176->tl=0;_tmp176;});
_tmp175;});_tmp174;});}static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*
p0){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp177=_cycalloc(sizeof(*_tmp177));
_tmp177[0]=({struct Cyc_Tcpat_Con_struct _tmp178;_tmp178.tag=0;_tmp178.f1=Cyc_Tcpat_ptr_con(
p0);_tmp178.f2=({struct Cyc_List_List*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->hd=(
void*)p;_tmp179->tl=0;_tmp179;});_tmp178;});_tmp177;});}static void*Cyc_Tcpat_tuple_pat(
struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A[0]=({struct Cyc_Tcpat_Con_struct
_tmp17B;_tmp17B.tag=0;_tmp17B.f1=Cyc_Tcpat_tuple_con(Cyc_List_length(ss),p);
_tmp17B.f2=ss;_tmp17B;});_tmp17A;});}static void*Cyc_Tcpat_con_pat(struct
_tagged_arr con_name,struct Cyc_Core_Opt*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*
p){struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp17E=_cycalloc(sizeof(*
_tmp17E));_tmp17E->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*_tmp17F=
_cycalloc(sizeof(*_tmp17F));_tmp17F[0]=({struct Cyc_Tcpat_Name_v_struct _tmp180;
_tmp180.tag=0;_tmp180.f1=con_name;_tmp180;});_tmp17F;}));_tmp17E->arity=Cyc_List_length(
ps);_tmp17E->span=span;_tmp17E->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp17E;});
return(void*)({struct Cyc_Tcpat_Con_struct*_tmp17C=_cycalloc(sizeof(*_tmp17C));
_tmp17C[0]=({struct Cyc_Tcpat_Con_struct _tmp17D;_tmp17D.tag=0;_tmp17D.f1=c;
_tmp17D.f2=ps;_tmp17D;});_tmp17C;});}static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*
p){void*s;{void*_tmp181=(void*)p->r;void*_tmp182;int _tmp183;char _tmp184;struct
_tagged_arr _tmp185;struct Cyc_Absyn_Pat*_tmp186;struct Cyc_Absyn_Tuniondecl*
_tmp187;struct Cyc_Absyn_Tunionfield*_tmp188;struct Cyc_List_List*_tmp189;struct
Cyc_List_List*_tmp18A;struct Cyc_List_List*_tmp18B;struct Cyc_Absyn_AggrInfo
_tmp18C;void*_tmp18D;struct Cyc_Absyn_Aggrdecl**_tmp18E;struct Cyc_Absyn_Aggrdecl*
_tmp18F;struct Cyc_List_List*_tmp190;struct Cyc_Absyn_Enumdecl*_tmp191;struct Cyc_Absyn_Enumfield*
_tmp192;void*_tmp193;struct Cyc_Absyn_Enumfield*_tmp194;_LLB7: if((int)_tmp181 != 0)
goto _LLB9;_LLB8: goto _LLBA;_LLB9: if(_tmp181 <= (void*)2  || *((int*)_tmp181)!= 0)
goto _LLBB;_LLBA: goto _LLBC;_LLBB: if(_tmp181 <= (void*)2  || *((int*)_tmp181)!= 2)
goto _LLBD;_LLBC: s=(void*)0;goto _LLB6;_LLBD: if((int)_tmp181 != 1)goto _LLBF;_LLBE: s=
Cyc_Tcpat_null_pat(p);goto _LLB6;_LLBF: if(_tmp181 <= (void*)2  || *((int*)_tmp181)
!= 7)goto _LLC1;_tmp182=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp181)->f1;
_tmp183=((struct Cyc_Absyn_Int_p_struct*)_tmp181)->f2;_LLC0: s=Cyc_Tcpat_int_pat(
_tmp183,(struct Cyc_Absyn_Pat*)p);goto _LLB6;_LLC1: if(_tmp181 <= (void*)2  || *((int*)
_tmp181)!= 8)goto _LLC3;_tmp184=((struct Cyc_Absyn_Char_p_struct*)_tmp181)->f1;
_LLC2: s=Cyc_Tcpat_char_pat(_tmp184,p);goto _LLB6;_LLC3: if(_tmp181 <= (void*)2  || *((
int*)_tmp181)!= 9)goto _LLC5;_tmp185=((struct Cyc_Absyn_Float_p_struct*)_tmp181)->f1;
_LLC4: s=Cyc_Tcpat_float_pat(_tmp185,p);goto _LLB6;_LLC5: if(_tmp181 <= (void*)2  || *((
int*)_tmp181)!= 1)goto _LLC7;_LLC6: s=(void*)0;goto _LLB6;_LLC7: if(_tmp181 <= (void*)
2  || *((int*)_tmp181)!= 4)goto _LLC9;_tmp186=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp181)->f1;_LLC8:{void*_tmp195=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo _tmp196;struct Cyc_Absyn_PtrAtts
_tmp197;struct Cyc_Absyn_Conref*_tmp198;_LLDE: if(_tmp195 <= (void*)4  || *((int*)
_tmp195)!= 4)goto _LLE0;_tmp196=((struct Cyc_Absyn_PointerType_struct*)_tmp195)->f1;
_tmp197=_tmp196.ptr_atts;_tmp198=_tmp197.nullable;_LLDF: {int is_nullable=0;int
still_working=1;while(still_working){void*_tmp199=(void*)_tmp198->v;struct Cyc_Absyn_Conref*
_tmp19A;int _tmp19B;_LLE5: if(_tmp199 <= (void*)1  || *((int*)_tmp199)!= 1)goto _LLE7;
_tmp19A=((struct Cyc_Absyn_Forward_constr_struct*)_tmp199)->f1;_LLE6:(void*)(
_tmp198->v=(void*)((void*)_tmp19A->v));continue;_LLE7: if((int)_tmp199 != 0)goto
_LLE9;_LLE8:(void*)(_tmp198->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp19D;_tmp19D.tag=0;_tmp19D.f1=(void*)0;_tmp19D;});_tmp19C;})));is_nullable=0;
still_working=0;goto _LLE4;_LLE9: if(_tmp199 <= (void*)1  || *((int*)_tmp199)!= 0)
goto _LLE4;_tmp19B=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp199)->f1;_LLEA:
is_nullable=(int)_tmp19B;still_working=0;goto _LLE4;_LLE4:;}{void*ss=Cyc_Tcpat_compile_pat(
_tmp186);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(ss,p);else{s=Cyc_Tcpat_ptr_pat(
ss,p);}goto _LLDD;}}_LLE0: if(_tmp195 <= (void*)4  || *((int*)_tmp195)!= 2)goto _LLE2;
_LLE1:{void*_tmp19E=(void*)_tmp186->r;struct Cyc_Absyn_Tuniondecl*_tmp19F;struct
Cyc_Absyn_Tunionfield*_tmp1A0;struct Cyc_List_List*_tmp1A1;_LLEC: if(_tmp19E <= (
void*)2  || *((int*)_tmp19E)!= 6)goto _LLEE;_tmp19F=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp19E)->f1;_tmp1A0=((struct Cyc_Absyn_Tunion_p_struct*)_tmp19E)->f2;_tmp1A1=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp19E)->f3;_LLED: {struct Cyc_Core_Opt*span;
if(_tmp19F->is_xtunion)span=0;else{span=({struct Cyc_Core_Opt*_tmp1A2=
_cycalloc_atomic(sizeof(*_tmp1A2));_tmp1A2->v=(void*)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp19F->fields))->v);_tmp1A2;});}s=Cyc_Tcpat_con_pat(*(*_tmp1A0->name).f2,span,((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp1A1),p);goto _LLEB;}_LLEE:;_LLEF:({void*
_tmp1A3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp1A4="non-[x]tunion pattern has tunion type";_tag_arr(_tmp1A4,
sizeof(char),_get_zero_arr_size(_tmp1A4,38));}),_tag_arr(_tmp1A3,sizeof(void*),0));});
_LLEB:;}goto _LLDD;_LLE2:;_LLE3:({void*_tmp1A5[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1A6="pointer pattern does not have pointer type";
_tag_arr(_tmp1A6,sizeof(char),_get_zero_arr_size(_tmp1A6,43));}),_tag_arr(
_tmp1A5,sizeof(void*),0));});_LLDD:;}goto _LLB6;_LLC9: if(_tmp181 <= (void*)2  || *((
int*)_tmp181)!= 6)goto _LLCB;_tmp187=((struct Cyc_Absyn_Tunion_p_struct*)_tmp181)->f1;
_tmp188=((struct Cyc_Absyn_Tunion_p_struct*)_tmp181)->f2;_tmp189=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp181)->f3;if(_tmp189 != 0)goto _LLCB;_LLCA: {struct Cyc_Core_Opt*span;{void*
_tmp1A7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);
_LLF1: if(_tmp1A7 <= (void*)4  || *((int*)_tmp1A7)!= 2)goto _LLF3;_LLF2: if(_tmp187->is_xtunion)
span=0;else{span=({struct Cyc_Core_Opt*_tmp1A8=_cycalloc_atomic(sizeof(*_tmp1A8));
_tmp1A8->v=(void*)((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp187->fields))->v);_tmp1A8;});}goto _LLF0;
_LLF3: if(_tmp1A7 <= (void*)4  || *((int*)_tmp1A7)!= 3)goto _LLF5;_LLF4: span=({
struct Cyc_Core_Opt*_tmp1A9=_cycalloc_atomic(sizeof(*_tmp1A9));_tmp1A9->v=(void*)
1;_tmp1A9;});goto _LLF0;_LLF5:;_LLF6: span=({void*_tmp1AA[0]={};((struct Cyc_Core_Opt*(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1AB="void tunion pattern has bad type";
_tag_arr(_tmp1AB,sizeof(char),_get_zero_arr_size(_tmp1AB,33));}),_tag_arr(
_tmp1AA,sizeof(void*),0));});goto _LLF0;_LLF0:;}s=Cyc_Tcpat_con_pat(*(*_tmp188->name).f2,
span,0,p);goto _LLB6;}_LLCB: if(_tmp181 <= (void*)2  || *((int*)_tmp181)!= 6)goto
_LLCD;_tmp18A=((struct Cyc_Absyn_Tunion_p_struct*)_tmp181)->f3;_LLCC: _tmp18B=
_tmp18A;goto _LLCE;_LLCD: if(_tmp181 <= (void*)2  || *((int*)_tmp181)!= 3)goto _LLCF;
_tmp18B=((struct Cyc_Absyn_Tuple_p_struct*)_tmp181)->f1;_LLCE: s=Cyc_Tcpat_tuple_pat(((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp18B),(struct Cyc_Absyn_Pat*)p);goto _LLB6;
_LLCF: if(_tmp181 <= (void*)2  || *((int*)_tmp181)!= 5)goto _LLD1;_tmp18C=((struct
Cyc_Absyn_Aggr_p_struct*)_tmp181)->f1;_tmp18D=(void*)_tmp18C.aggr_info;if(*((int*)
_tmp18D)!= 1)goto _LLD1;_tmp18E=((struct Cyc_Absyn_KnownAggr_struct*)_tmp18D)->f1;
_tmp18F=*_tmp18E;_tmp190=((struct Cyc_Absyn_Aggr_p_struct*)_tmp181)->f3;_LLD0: {
struct Cyc_List_List*ps=0;{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp18F->impl))->fields;for(0;fields != 0;fields=fields->tl){int found=
Cyc_strcmp((struct _tagged_arr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,({
const char*_tmp1BA="";_tag_arr(_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,1));}))
== 0;{struct Cyc_List_List*dlps0=_tmp190;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple8 _tmp1AD;struct Cyc_List_List*_tmp1AE;struct Cyc_Absyn_Pat*_tmp1AF;
struct _tuple8*_tmp1AC=(struct _tuple8*)dlps0->hd;_tmp1AD=*_tmp1AC;_tmp1AE=_tmp1AD.f1;
_tmp1AF=_tmp1AD.f2;{struct Cyc_List_List*_tmp1B0=_tmp1AE;struct Cyc_List_List
_tmp1B1;void*_tmp1B2;struct _tagged_arr*_tmp1B3;struct Cyc_List_List*_tmp1B4;_LLF8:
if(_tmp1B0 == 0)goto _LLFA;_tmp1B1=*_tmp1B0;_tmp1B2=(void*)_tmp1B1.hd;if(*((int*)
_tmp1B2)!= 1)goto _LLFA;_tmp1B3=((struct Cyc_Absyn_FieldName_struct*)_tmp1B2)->f1;
_tmp1B4=_tmp1B1.tl;if(_tmp1B4 != 0)goto _LLFA;_LLF9: if(Cyc_strptrcmp(_tmp1B3,((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){ps=({struct Cyc_List_List*
_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5->hd=(void*)Cyc_Tcpat_compile_pat(
_tmp1AF);_tmp1B5->tl=ps;_tmp1B5;});found=1;}goto _LLF7;_LLFA:;_LLFB:({void*
_tmp1B6[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp1B7="bad designator(s)";_tag_arr(_tmp1B7,sizeof(char),
_get_zero_arr_size(_tmp1B7,18));}),_tag_arr(_tmp1B6,sizeof(void*),0));});_LLF7:;}}}
if(!found)({void*_tmp1B8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp1B9="bad designator";_tag_arr(_tmp1B9,sizeof(
char),_get_zero_arr_size(_tmp1B9,15));}),_tag_arr(_tmp1B8,sizeof(void*),0));});}}
s=Cyc_Tcpat_tuple_pat(ps,(struct Cyc_Absyn_Pat*)p);goto _LLB6;}_LLD1: if(_tmp181 <= (
void*)2  || *((int*)_tmp181)!= 10)goto _LLD3;_tmp191=((struct Cyc_Absyn_Enum_p_struct*)
_tmp181)->f1;_tmp192=((struct Cyc_Absyn_Enum_p_struct*)_tmp181)->f2;_LLD2: {int
span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp191->fields))->v);s=Cyc_Tcpat_con_pat(*(*
_tmp192->name).f2,({struct Cyc_Core_Opt*_tmp1BB=_cycalloc_atomic(sizeof(*_tmp1BB));
_tmp1BB->v=(void*)span;_tmp1BB;}),0,p);goto _LLB6;}_LLD3: if(_tmp181 <= (void*)2
 || *((int*)_tmp181)!= 11)goto _LLD5;_tmp193=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp181)->f1;_tmp194=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp181)->f2;_LLD4: {
struct Cyc_List_List*fields;{void*_tmp1BC=Cyc_Tcutil_compress(_tmp193);struct Cyc_List_List*
_tmp1BD;_LLFD: if(_tmp1BC <= (void*)4  || *((int*)_tmp1BC)!= 13)goto _LLFF;_tmp1BD=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp1BC)->f1;_LLFE: fields=_tmp1BD;goto _LLFC;
_LLFF:;_LL100:({void*_tmp1BE[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1BF="bad type in AnonEnum_p";
_tag_arr(_tmp1BF,sizeof(char),_get_zero_arr_size(_tmp1BF,23));}),_tag_arr(
_tmp1BE,sizeof(void*),0));});_LLFC:;}{int span=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(fields);s=Cyc_Tcpat_con_pat(*(*_tmp194->name).f2,({struct Cyc_Core_Opt*
_tmp1C0=_cycalloc_atomic(sizeof(*_tmp1C0));_tmp1C0->v=(void*)span;_tmp1C0;}),0,p);
goto _LLB6;}}_LLD5: if(_tmp181 <= (void*)2  || *((int*)_tmp181)!= 12)goto _LLD7;_LLD6:
goto _LLD8;_LLD7: if(_tmp181 <= (void*)2  || *((int*)_tmp181)!= 13)goto _LLD9;_LLD8:
goto _LLDA;_LLD9: if(_tmp181 <= (void*)2  || *((int*)_tmp181)!= 5)goto _LLDB;_LLDA:
goto _LLDC;_LLDB: if(_tmp181 <= (void*)2  || *((int*)_tmp181)!= 14)goto _LLB6;_LLDC: s=(
void*)0;_LLB6:;}return s;}struct Cyc_Tcpat_Pos_struct{int tag;struct Cyc_Tcpat_Con_s*
f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{int tag;struct Cyc_Set_Set*
f1;};struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_struct{
int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct _tuple11{struct Cyc_List_List*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};static void*Cyc_Tcpat_add_neg(
struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){void*_tmp1C1=td;struct Cyc_Set_Set*
_tmp1C2;_LL102: if(*((int*)_tmp1C1)!= 1)goto _LL104;_tmp1C2=((struct Cyc_Tcpat_Neg_struct*)
_tmp1C1)->f1;_LL103: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp1C2,c))({void*_tmp1C3[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1C4="add_neg called when constructor already in set";
_tag_arr(_tmp1C4,sizeof(char),_get_zero_arr_size(_tmp1C4,47));}),_tag_arr(
_tmp1C3,sizeof(void*),0));});if(c->span != 0  && ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp1C2)+ 1 >= (int)((struct Cyc_Core_Opt*)_check_null(c->span))->v)({void*_tmp1C5[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp1C6="add_neg called when |cs U {c}| >= span(c)";_tag_arr(_tmp1C6,
sizeof(char),_get_zero_arr_size(_tmp1C6,42));}),_tag_arr(_tmp1C5,sizeof(void*),0));});
return(void*)({struct Cyc_Tcpat_Neg_struct*_tmp1C7=_region_malloc(r,sizeof(*
_tmp1C7));_tmp1C7[0]=({struct Cyc_Tcpat_Neg_struct _tmp1C8;_tmp1C8.tag=1;_tmp1C8.f1=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_rinsert)(r,_tmp1C2,c);_tmp1C8;});_tmp1C7;});_LL104: if(*((int*)
_tmp1C1)!= 0)goto _LL101;_LL105:({void*_tmp1C9[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1CA="add_neg called when td is Positive";
_tag_arr(_tmp1CA,sizeof(char),_get_zero_arr_size(_tmp1CA,35));}),_tag_arr(
_tmp1C9,sizeof(void*),0));});_LL101:;}static void*Cyc_Tcpat_static_match(struct
Cyc_Tcpat_Con_s*c,void*td){void*_tmp1CB=td;struct Cyc_Tcpat_Con_s*_tmp1CC;struct
Cyc_Set_Set*_tmp1CD;_LL107: if(*((int*)_tmp1CB)!= 0)goto _LL109;_tmp1CC=((struct
Cyc_Tcpat_Pos_struct*)_tmp1CB)->f1;_LL108: if(Cyc_Tcpat_compare_con(c,_tmp1CC)== 
0)return(void*)0;else{return(void*)1;}_LL109: if(*((int*)_tmp1CB)!= 1)goto _LL106;
_tmp1CD=((struct Cyc_Tcpat_Neg_struct*)_tmp1CB)->f1;_LL10A: if(((int(*)(struct Cyc_Set_Set*
s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp1CD,c))return(void*)1;else{if(c->span
!= 0  && (int)((struct Cyc_Core_Opt*)_check_null(c->span))->v == ((int(*)(struct Cyc_Set_Set*
s))Cyc_Set_cardinality)(_tmp1CD)+ 1)return(void*)0;else{return(void*)2;}}_LL106:;}
struct _tuple12{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*
Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp1CE=ctxt;struct Cyc_List_List _tmp1CF;struct _tuple12*
_tmp1D0;struct _tuple12 _tmp1D1;struct Cyc_Tcpat_Con_s*_tmp1D2;struct Cyc_List_List*
_tmp1D3;struct Cyc_List_List*_tmp1D4;_LL10C: if(_tmp1CE != 0)goto _LL10E;_LL10D:
return 0;_LL10E: if(_tmp1CE == 0)goto _LL10B;_tmp1CF=*_tmp1CE;_tmp1D0=(struct
_tuple12*)_tmp1CF.hd;_tmp1D1=*_tmp1D0;_tmp1D2=_tmp1D1.f1;_tmp1D3=_tmp1D1.f2;
_tmp1D4=_tmp1CF.tl;_LL10F: return({struct Cyc_List_List*_tmp1D5=_region_malloc(r,
sizeof(*_tmp1D5));_tmp1D5->hd=({struct _tuple12*_tmp1D6=_region_malloc(r,sizeof(*
_tmp1D6));_tmp1D6->f1=_tmp1D2;_tmp1D6->f2=(struct Cyc_List_List*)({struct Cyc_List_List*
_tmp1D7=_region_malloc(r,sizeof(*_tmp1D7));_tmp1D7->hd=(void*)dsc;_tmp1D7->tl=
_tmp1D3;_tmp1D7;});_tmp1D6;});_tmp1D5->tl=_tmp1D4;_tmp1D5;});_LL10B:;}static
struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct Cyc_List_List*
ctxt){struct Cyc_List_List*_tmp1D8=ctxt;struct Cyc_List_List _tmp1D9;struct _tuple12*
_tmp1DA;struct _tuple12 _tmp1DB;struct Cyc_Tcpat_Con_s*_tmp1DC;struct Cyc_List_List*
_tmp1DD;struct Cyc_List_List*_tmp1DE;_LL111: if(_tmp1D8 != 0)goto _LL113;_LL112:({
void*_tmp1DF[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp1E0="norm_context: empty context";_tag_arr(_tmp1E0,sizeof(char),
_get_zero_arr_size(_tmp1E0,28));}),_tag_arr(_tmp1DF,sizeof(void*),0));});_LL113:
if(_tmp1D8 == 0)goto _LL110;_tmp1D9=*_tmp1D8;_tmp1DA=(struct _tuple12*)_tmp1D9.hd;
_tmp1DB=*_tmp1DA;_tmp1DC=_tmp1DB.f1;_tmp1DD=_tmp1DB.f2;_tmp1DE=_tmp1D9.tl;_LL114:
return Cyc_Tcpat_augment(r,_tmp1DE,(void*)({struct Cyc_Tcpat_Pos_struct*_tmp1E1=
_region_malloc(r,sizeof(*_tmp1E1));_tmp1E1[0]=({struct Cyc_Tcpat_Pos_struct
_tmp1E2;_tmp1E2.tag=0;_tmp1E2.f1=_tmp1DC;_tmp1E2.f2=Cyc_List_rrev(r,_tmp1DD);
_tmp1E2;});_tmp1E1;}));_LL110:;}static void*Cyc_Tcpat_build_desc(struct
_RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
struct _tuple6 _tmp1E4=({struct _tuple6 _tmp1E3;_tmp1E3.f1=ctxt;_tmp1E3.f2=work;
_tmp1E3;});struct Cyc_List_List*_tmp1E5;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List*
_tmp1E7;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1E9;struct Cyc_List_List
_tmp1EA;struct _tuple12*_tmp1EB;struct _tuple12 _tmp1EC;struct Cyc_Tcpat_Con_s*
_tmp1ED;struct Cyc_List_List*_tmp1EE;struct Cyc_List_List*_tmp1EF;struct Cyc_List_List*
_tmp1F0;struct Cyc_List_List _tmp1F1;struct _tuple11*_tmp1F2;struct _tuple11 _tmp1F3;
struct Cyc_List_List*_tmp1F4;struct Cyc_List_List*_tmp1F5;_LL116: _tmp1E5=_tmp1E4.f1;
if(_tmp1E5 != 0)goto _LL118;_tmp1E6=_tmp1E4.f2;if(_tmp1E6 != 0)goto _LL118;_LL117:
return dsc;_LL118: _tmp1E7=_tmp1E4.f1;if(_tmp1E7 != 0)goto _LL11A;_LL119: goto _LL11B;
_LL11A: _tmp1E8=_tmp1E4.f2;if(_tmp1E8 != 0)goto _LL11C;_LL11B:({void*_tmp1F6[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp1F7="build_desc: ctxt and work don't match";_tag_arr(_tmp1F7,sizeof(char),
_get_zero_arr_size(_tmp1F7,38));}),_tag_arr(_tmp1F6,sizeof(void*),0));});_LL11C:
_tmp1E9=_tmp1E4.f1;if(_tmp1E9 == 0)goto _LL115;_tmp1EA=*_tmp1E9;_tmp1EB=(struct
_tuple12*)_tmp1EA.hd;_tmp1EC=*_tmp1EB;_tmp1ED=_tmp1EC.f1;_tmp1EE=_tmp1EC.f2;
_tmp1EF=_tmp1EA.tl;_tmp1F0=_tmp1E4.f2;if(_tmp1F0 == 0)goto _LL115;_tmp1F1=*_tmp1F0;
_tmp1F2=(struct _tuple11*)_tmp1F1.hd;_tmp1F3=*_tmp1F2;_tmp1F4=_tmp1F3.f3;_tmp1F5=
_tmp1F1.tl;_LL11D: {struct Cyc_Tcpat_Pos_struct*_tmp1F8=({struct Cyc_Tcpat_Pos_struct*
_tmp1F9=_region_malloc(r,sizeof(*_tmp1F9));_tmp1F9[0]=({struct Cyc_Tcpat_Pos_struct
_tmp1FA;_tmp1FA.tag=0;_tmp1FA.f1=_tmp1ED;_tmp1FA.f2=Cyc_List_rappend(r,Cyc_List_rrev(
r,_tmp1EE),({struct Cyc_List_List*_tmp1FB=_region_malloc(r,sizeof(*_tmp1FB));
_tmp1FB->hd=(void*)dsc;_tmp1FB->tl=_tmp1F4;_tmp1FB;}));_tmp1FA;});_tmp1F9;});
return Cyc_Tcpat_build_desc(r,_tmp1EF,(void*)_tmp1F8,_tmp1F5);}_LL115:;}static
void*Cyc_Tcpat_match(struct _RegionHandle*,void*,struct Cyc_List_List*,void*,
struct Cyc_List_List*,struct Cyc_List_List*,void*,struct Cyc_List_List*);static void*
Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp1FC=allmrules;struct Cyc_List_List _tmp1FD;struct _tuple0*
_tmp1FE;struct _tuple0 _tmp1FF;void*_tmp200;void*_tmp201;struct Cyc_List_List*
_tmp202;_LL11F: if(_tmp1FC != 0)goto _LL121;_LL120: return(void*)({struct Cyc_Tcpat_Failure_struct*
_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203[0]=({struct Cyc_Tcpat_Failure_struct
_tmp204;_tmp204.tag=0;_tmp204.f1=(void*)dsc;_tmp204;});_tmp203;});_LL121: if(
_tmp1FC == 0)goto _LL11E;_tmp1FD=*_tmp1FC;_tmp1FE=(struct _tuple0*)_tmp1FD.hd;
_tmp1FF=*_tmp1FE;_tmp200=_tmp1FF.f1;_tmp201=_tmp1FF.f2;_tmp202=_tmp1FD.tl;_LL122:
return Cyc_Tcpat_match(r,_tmp200,0,dsc,0,0,_tmp201,_tmp202);_LL11E:;}static void*
Cyc_Tcpat_match_compile(struct _RegionHandle*r,struct Cyc_List_List*allmrules){
return Cyc_Tcpat_or_match(r,(void*)({struct Cyc_Tcpat_Neg_struct*_tmp205=
_region_malloc(r,sizeof(*_tmp205));_tmp205[0]=({struct Cyc_Tcpat_Neg_struct
_tmp206;_tmp206.tag=1;_tmp206.f1=Cyc_Tcpat_empty_con_set(r);_tmp206;});_tmp205;}),
allmrules);}static void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*
ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
struct Cyc_List_List*_tmp207=work;struct Cyc_List_List _tmp208;struct _tuple11*
_tmp209;struct _tuple11 _tmp20A;struct Cyc_List_List*_tmp20B;struct Cyc_List_List*
_tmp20C;struct Cyc_List_List*_tmp20D;struct Cyc_List_List*_tmp20E;struct Cyc_List_List
_tmp20F;struct _tuple11*_tmp210;struct _tuple11 _tmp211;struct Cyc_List_List*_tmp212;
struct Cyc_List_List*_tmp213;struct Cyc_List_List*_tmp214;struct Cyc_List_List*
_tmp215;_LL124: if(_tmp207 != 0)goto _LL126;_LL125: return(void*)({struct Cyc_Tcpat_Success_struct*
_tmp216=_region_malloc(r,sizeof(*_tmp216));_tmp216[0]=({struct Cyc_Tcpat_Success_struct
_tmp217;_tmp217.tag=1;_tmp217.f1=(void*)right_hand_side;_tmp217;});_tmp216;});
_LL126: if(_tmp207 == 0)goto _LL128;_tmp208=*_tmp207;_tmp209=(struct _tuple11*)
_tmp208.hd;_tmp20A=*_tmp209;_tmp20B=_tmp20A.f1;if(_tmp20B != 0)goto _LL128;_tmp20C=
_tmp20A.f2;if(_tmp20C != 0)goto _LL128;_tmp20D=_tmp20A.f3;if(_tmp20D != 0)goto
_LL128;_tmp20E=_tmp208.tl;_LL127: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp20E,right_hand_side,rules);_LL128: if(_tmp207 == 0)goto _LL123;_tmp20F=*
_tmp207;_tmp210=(struct _tuple11*)_tmp20F.hd;_tmp211=*_tmp210;_tmp212=_tmp211.f1;
_tmp213=_tmp211.f2;_tmp214=_tmp211.f3;_tmp215=_tmp20F.tl;_LL129: if((_tmp212 == 0
 || _tmp213 == 0) || _tmp214 == 0)({void*_tmp218[0]={};Cyc_Tcutil_impos(({const
char*_tmp219="tcpat:and_match: malformed work frame";_tag_arr(_tmp219,sizeof(
char),_get_zero_arr_size(_tmp219,38));}),_tag_arr(_tmp218,sizeof(void*),0));});{
struct Cyc_List_List _tmp21B;void*_tmp21C;struct Cyc_List_List*_tmp21D;struct Cyc_List_List*
_tmp21A=(struct Cyc_List_List*)_tmp212;_tmp21B=*_tmp21A;_tmp21C=(void*)_tmp21B.hd;
_tmp21D=_tmp21B.tl;{struct Cyc_List_List _tmp21F;struct Cyc_List_List*_tmp220;
struct Cyc_List_List*_tmp221;struct Cyc_List_List*_tmp21E=(struct Cyc_List_List*)
_tmp213;_tmp21F=*_tmp21E;_tmp220=(struct Cyc_List_List*)_tmp21F.hd;_tmp221=
_tmp21F.tl;{struct Cyc_List_List _tmp223;void*_tmp224;struct Cyc_List_List*_tmp225;
struct Cyc_List_List*_tmp222=(struct Cyc_List_List*)_tmp214;_tmp223=*_tmp222;
_tmp224=(void*)_tmp223.hd;_tmp225=_tmp223.tl;{struct _tuple11*_tmp226=({struct
_tuple11*_tmp228=_region_malloc(r,sizeof(*_tmp228));_tmp228->f1=_tmp21D;_tmp228->f2=
_tmp221;_tmp228->f3=_tmp225;_tmp228;});return Cyc_Tcpat_match(r,_tmp21C,_tmp220,
_tmp224,ctx,({struct Cyc_List_List*_tmp227=_region_malloc(r,sizeof(*_tmp227));
_tmp227->hd=_tmp226;_tmp227->tl=_tmp215;_tmp227;}),right_hand_side,rules);}}}}
_LL123:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,
struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp229=dsc;struct Cyc_Set_Set*_tmp22A;
struct Cyc_List_List*_tmp22B;_LL12B: if(*((int*)_tmp229)!= 1)goto _LL12D;_tmp22A=((
struct Cyc_Tcpat_Neg_struct*)_tmp229)->f1;_LL12C: {void*any=(void*)({struct Cyc_Tcpat_Neg_struct*
_tmp22E=_region_malloc(r,sizeof(*_tmp22E));_tmp22E[0]=({struct Cyc_Tcpat_Neg_struct
_tmp22F;_tmp22F.tag=1;_tmp22F.f1=Cyc_Tcpat_empty_con_set(r);_tmp22F;});_tmp22E;});
struct Cyc_List_List*_tmp22C=0;{int i=0;for(0;i < pcon->arity;++ i){_tmp22C=({struct
Cyc_List_List*_tmp22D=_region_malloc(r,sizeof(*_tmp22D));_tmp22D->hd=(void*)any;
_tmp22D->tl=_tmp22C;_tmp22D;});}}return _tmp22C;}_LL12D: if(*((int*)_tmp229)!= 0)
goto _LL12A;_tmp22B=((struct Cyc_Tcpat_Pos_struct*)_tmp229)->f2;_LL12E: return
_tmp22B;_LL12A:;}struct _tuple13{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple13*env,int i){struct
_tuple13 _tmp231;struct _RegionHandle*_tmp232;struct Cyc_List_List*_tmp233;struct
_tuple13*_tmp230=env;_tmp231=*_tmp230;_tmp232=_tmp231.f1;_tmp233=_tmp231.f2;
return({struct Cyc_List_List*_tmp234=_region_malloc(_tmp232,sizeof(*_tmp234));
_tmp234->hd=(void*)(i + 1);_tmp234->tl=_tmp233;_tmp234;});}static struct Cyc_List_List*
Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*
obj){struct _tuple13 _tmp235=({struct _tuple13 _tmp236;_tmp236.f1=r;_tmp236.f2=obj;
_tmp236;});return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple13*,int),struct _tuple13*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp235);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp237=p;struct Cyc_Tcpat_Con_s*
_tmp238;struct Cyc_List_List*_tmp239;_LL130: if((int)_tmp237 != 0)goto _LL132;_LL131:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL132: if(_tmp237 <= (void*)1  || *((int*)_tmp237)!= 0)goto _LL12F;_tmp238=((
struct Cyc_Tcpat_Con_struct*)_tmp237)->f1;_tmp239=((struct Cyc_Tcpat_Con_struct*)
_tmp237)->f2;_LL133: {void*_tmp23A=Cyc_Tcpat_static_match(_tmp238,dsc);_LL135:
if((int)_tmp23A != 0)goto _LL137;_LL136: {struct Cyc_List_List*_tmp23B=({struct Cyc_List_List*
_tmp240=_region_malloc(r,sizeof(*_tmp240));_tmp240->hd=({struct _tuple12*_tmp241=
_region_malloc(r,sizeof(*_tmp241));_tmp241->f1=_tmp238;_tmp241->f2=0;_tmp241;});
_tmp240->tl=ctx;_tmp240;});struct _tuple11*_tmp23C=({struct _tuple11*_tmp23F=
_region_malloc(r,sizeof(*_tmp23F));_tmp23F->f1=_tmp239;_tmp23F->f2=Cyc_Tcpat_getoargs(
r,_tmp238,obj);_tmp23F->f3=Cyc_Tcpat_getdargs(r,_tmp238,dsc);_tmp23F;});struct
Cyc_List_List*_tmp23D=({struct Cyc_List_List*_tmp23E=_region_malloc(r,sizeof(*
_tmp23E));_tmp23E->hd=_tmp23C;_tmp23E->tl=work;_tmp23E;});return Cyc_Tcpat_and_match(
r,_tmp23B,_tmp23D,right_hand_side,rules);}_LL137: if((int)_tmp23A != 1)goto _LL139;
_LL138: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);
_LL139: if((int)_tmp23A != 2)goto _LL134;_LL13A: {struct Cyc_List_List*_tmp242=({
struct Cyc_List_List*_tmp24B=_region_malloc(r,sizeof(*_tmp24B));_tmp24B->hd=({
struct _tuple12*_tmp24C=_region_malloc(r,sizeof(*_tmp24C));_tmp24C->f1=_tmp238;
_tmp24C->f2=0;_tmp24C;});_tmp24B->tl=ctx;_tmp24B;});struct _tuple11*_tmp243=({
struct _tuple11*_tmp24A=_region_malloc(r,sizeof(*_tmp24A));_tmp24A->f1=_tmp239;
_tmp24A->f2=Cyc_Tcpat_getoargs(r,_tmp238,obj);_tmp24A->f3=Cyc_Tcpat_getdargs(r,
_tmp238,dsc);_tmp24A;});struct Cyc_List_List*_tmp244=({struct Cyc_List_List*
_tmp249=_region_malloc(r,sizeof(*_tmp249));_tmp249->hd=_tmp243;_tmp249->tl=work;
_tmp249;});void*_tmp245=Cyc_Tcpat_and_match(r,_tmp242,_tmp244,right_hand_side,
rules);void*_tmp246=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(
r,dsc,_tmp238),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*_tmp247=
_region_malloc(r,sizeof(*_tmp247));_tmp247[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp248;_tmp248.tag=2;_tmp248.f1=obj;_tmp248.f2=_tmp238;_tmp248.f3=(void*)
_tmp245;_tmp248.f4=(void*)_tmp246;_tmp248;});_tmp247;});}_LL134:;}_LL12F:;}
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),
void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){void*
_tmp24D=d;void*_tmp24E;void*_tmp24F;void*_tmp250;void*_tmp251;_LL13C: if(*((int*)
_tmp24D)!= 0)goto _LL13E;_tmp24E=(void*)((struct Cyc_Tcpat_Failure_struct*)_tmp24D)->f1;
_LL13D: if(!(*exhaust_done)){not_exhaust(env1,_tmp24E);*exhaust_done=1;}goto
_LL13B;_LL13E: if(*((int*)_tmp24D)!= 1)goto _LL140;_tmp24F=(void*)((struct Cyc_Tcpat_Success_struct*)
_tmp24D)->f1;_LL13F: rhs_appears(env2,_tmp24F);goto _LL13B;_LL140: if(*((int*)
_tmp24D)!= 2)goto _LL13B;_tmp250=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp24D)->f3;
_tmp251=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp24D)->f4;_LL141: Cyc_Tcpat_check_exhaust_overlap(
_tmp250,not_exhaust,env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(
_tmp251,not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL13B;_LL13B:;}
struct _tuple14{int f1;struct Cyc_Position_Segment*f2;};struct _tuple15{void*f1;
struct _tuple14*f2;};struct _tuple16{struct _RegionHandle*f1;int*f2;};static struct
_tuple15*Cyc_Tcpat_get_match(struct _tuple16*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple16 _tmp253;struct _RegionHandle*_tmp254;int*_tmp255;struct
_tuple16*_tmp252=env;_tmp253=*_tmp252;_tmp254=_tmp253.f1;_tmp255=_tmp253.f2;{
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);struct _tuple14*rhs=({struct _tuple14*
_tmp25B=_region_malloc(_tmp254,sizeof(*_tmp25B));_tmp25B->f1=0;_tmp25B->f2=(swc->pattern)->loc;
_tmp25B;});void*sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*
_tmp256=_cycalloc(sizeof(*_tmp256));_tmp256->hd=(void*)sp0;_tmp256->tl=({struct
Cyc_List_List*_tmp257=_cycalloc(sizeof(*_tmp257));_tmp257->hd=(void*)Cyc_Tcpat_int_pat(*
_tmp255,0);_tmp257->tl=0;_tmp257;});_tmp256;}),0);*_tmp255=*_tmp255 + 1;}else{sp=
Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*_tmp258=_cycalloc(sizeof(*_tmp258));
_tmp258->hd=(void*)sp0;_tmp258->tl=({struct Cyc_List_List*_tmp259=_cycalloc(
sizeof(*_tmp259));_tmp259->hd=(void*)((void*)0);_tmp259->tl=0;_tmp259;});_tmp258;}),
0);}return({struct _tuple15*_tmp25A=_region_malloc(_tmp254,sizeof(*_tmp25A));
_tmp25A->f1=sp;_tmp25A->f2=rhs;_tmp25A;});}}void Cyc_Tcpat_add_con_to_list(struct
Cyc_Tcpat_Con_s*c,struct Cyc_List_List**cs){*cs=({struct Cyc_List_List*_tmp25C=
_cycalloc(sizeof(*_tmp25C));_tmp25C->hd=c;_tmp25C->tl=*cs;_tmp25C;});}char Cyc_Tcpat_Desc2string[
16]="\000\000\000\000Desc2string\000";static struct _tagged_arr Cyc_Tcpat_descs2string(
struct Cyc_List_List*);static struct _tagged_arr Cyc_Tcpat_desc2string(void*d){void*
_tmp25D=d;struct Cyc_Tcpat_Con_s*_tmp25E;struct Cyc_List_List*_tmp25F;struct Cyc_Set_Set*
_tmp260;_LL143: if(*((int*)_tmp25D)!= 0)goto _LL145;_tmp25E=((struct Cyc_Tcpat_Pos_struct*)
_tmp25D)->f1;_tmp25F=((struct Cyc_Tcpat_Pos_struct*)_tmp25D)->f2;_LL144: {void*
_tmp261=(void*)_tmp25E->name;struct Cyc_Absyn_Pat*_tmp262=_tmp25E->orig_pat;if(
_tmp262 == 0)return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)
_check_null(_tmp25F))->hd);{void*_tmp263=(void*)_tmp262->r;struct Cyc_Absyn_Vardecl*
_tmp264;struct Cyc_Absyn_Vardecl*_tmp265;struct Cyc_Absyn_Tvar*_tmp266;struct Cyc_Absyn_Vardecl*
_tmp267;struct Cyc_Absyn_Pat*_tmp268;struct Cyc_Absyn_Pat _tmp269;void*_tmp26A;
struct Cyc_Absyn_Tunionfield*_tmp26B;struct Cyc_Absyn_AggrInfo _tmp26C;void*_tmp26D;
struct Cyc_Absyn_Aggrdecl**_tmp26E;struct Cyc_Absyn_Aggrdecl*_tmp26F;struct Cyc_Absyn_Tunionfield*
_tmp270;int _tmp271;char _tmp272;struct _tagged_arr _tmp273;struct Cyc_Absyn_Enumfield*
_tmp274;struct Cyc_Absyn_Enumfield*_tmp275;struct Cyc_Absyn_Exp*_tmp276;_LL148: if((
int)_tmp263 != 0)goto _LL14A;_LL149: return({const char*_tmp277="_";_tag_arr(_tmp277,
sizeof(char),_get_zero_arr_size(_tmp277,2));});_LL14A: if(_tmp263 <= (void*)2  || *((
int*)_tmp263)!= 0)goto _LL14C;_tmp264=((struct Cyc_Absyn_Var_p_struct*)_tmp263)->f1;
_LL14B: return Cyc_Absynpp_qvar2string(_tmp264->name);_LL14C: if(_tmp263 <= (void*)2
 || *((int*)_tmp263)!= 1)goto _LL14E;_tmp265=((struct Cyc_Absyn_Reference_p_struct*)
_tmp263)->f1;_LL14D: return(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp27A;
_tmp27A.tag=0;_tmp27A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp265->name));{void*_tmp278[1]={& _tmp27A};Cyc_aprintf(({const char*_tmp279="*%s";
_tag_arr(_tmp279,sizeof(char),_get_zero_arr_size(_tmp279,4));}),_tag_arr(_tmp278,
sizeof(void*),1));}});_LL14E: if(_tmp263 <= (void*)2  || *((int*)_tmp263)!= 2)goto
_LL150;_tmp266=((struct Cyc_Absyn_TagInt_p_struct*)_tmp263)->f1;_tmp267=((struct
Cyc_Absyn_TagInt_p_struct*)_tmp263)->f2;_LL14F: return(struct _tagged_arr)({struct
Cyc_String_pa_struct _tmp27E;_tmp27E.tag=0;_tmp27E.f1=(struct _tagged_arr)((struct
_tagged_arr)*_tmp266->name);{struct Cyc_String_pa_struct _tmp27D;_tmp27D.tag=0;
_tmp27D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp267->name));{void*_tmp27B[2]={& _tmp27D,& _tmp27E};Cyc_aprintf(({const char*
_tmp27C="%s<`%s>";_tag_arr(_tmp27C,sizeof(char),_get_zero_arr_size(_tmp27C,8));}),
_tag_arr(_tmp27B,sizeof(void*),2));}}});_LL150: if(_tmp263 <= (void*)2  || *((int*)
_tmp263)!= 3)goto _LL152;_LL151: return(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp281;_tmp281.tag=0;_tmp281.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcpat_descs2string(
_tmp25F));{void*_tmp27F[1]={& _tmp281};Cyc_aprintf(({const char*_tmp280="$(%s)";
_tag_arr(_tmp280,sizeof(char),_get_zero_arr_size(_tmp280,6));}),_tag_arr(_tmp27F,
sizeof(void*),1));}});_LL152: if(_tmp263 <= (void*)2  || *((int*)_tmp263)!= 4)goto
_LL154;_tmp268=((struct Cyc_Absyn_Pointer_p_struct*)_tmp263)->f1;_tmp269=*_tmp268;
_tmp26A=(void*)_tmp269.r;if(_tmp26A <= (void*)2  || *((int*)_tmp26A)!= 6)goto
_LL154;_tmp26B=((struct Cyc_Absyn_Tunion_p_struct*)_tmp26A)->f2;_LL153: return(
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp285;_tmp285.tag=0;_tmp285.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Tcpat_descs2string(_tmp25F));{struct
Cyc_String_pa_struct _tmp284;_tmp284.tag=0;_tmp284.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp26B->name));{void*_tmp282[2]={& _tmp284,&
_tmp285};Cyc_aprintf(({const char*_tmp283="&%s(%s)";_tag_arr(_tmp283,sizeof(char),
_get_zero_arr_size(_tmp283,8));}),_tag_arr(_tmp282,sizeof(void*),2));}}});_LL154:
if(_tmp263 <= (void*)2  || *((int*)_tmp263)!= 4)goto _LL156;_LL155: return(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp288;_tmp288.tag=0;_tmp288.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Tcpat_descs2string(_tmp25F));{void*_tmp286[1]={&
_tmp288};Cyc_aprintf(({const char*_tmp287="&%s";_tag_arr(_tmp287,sizeof(char),
_get_zero_arr_size(_tmp287,4));}),_tag_arr(_tmp286,sizeof(void*),1));}});_LL156:
if(_tmp263 <= (void*)2  || *((int*)_tmp263)!= 5)goto _LL158;_tmp26C=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp263)->f1;_tmp26D=(void*)_tmp26C.aggr_info;if(*((int*)_tmp26D)!= 1)goto _LL158;
_tmp26E=((struct Cyc_Absyn_KnownAggr_struct*)_tmp26D)->f1;_tmp26F=*_tmp26E;_LL157:
return(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp28C;_tmp28C.tag=0;
_tmp28C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcpat_descs2string(_tmp25F));{
struct Cyc_String_pa_struct _tmp28B;_tmp28B.tag=0;_tmp28B.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp26F->name));{void*_tmp289[2]={&
_tmp28B,& _tmp28C};Cyc_aprintf(({const char*_tmp28A="%s{%s}";_tag_arr(_tmp28A,
sizeof(char),_get_zero_arr_size(_tmp28A,7));}),_tag_arr(_tmp289,sizeof(void*),2));}}});
_LL158: if(_tmp263 <= (void*)2  || *((int*)_tmp263)!= 6)goto _LL15A;_tmp270=((struct
Cyc_Absyn_Tunion_p_struct*)_tmp263)->f2;_LL159: return(struct _tagged_arr)({struct
Cyc_String_pa_struct _tmp28F;_tmp28F.tag=0;_tmp28F.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp270->name));{void*_tmp28D[1]={& _tmp28F};
Cyc_aprintf(({const char*_tmp28E="%s";_tag_arr(_tmp28E,sizeof(char),
_get_zero_arr_size(_tmp28E,3));}),_tag_arr(_tmp28D,sizeof(void*),1));}});_LL15A:
if((int)_tmp263 != 1)goto _LL15C;_LL15B: return({const char*_tmp290="NULL";_tag_arr(
_tmp290,sizeof(char),_get_zero_arr_size(_tmp290,5));});_LL15C: if(_tmp263 <= (void*)
2  || *((int*)_tmp263)!= 7)goto _LL15E;_tmp271=((struct Cyc_Absyn_Int_p_struct*)
_tmp263)->f2;_LL15D: return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp293;
_tmp293.tag=1;_tmp293.f1=(unsigned int)_tmp271;{void*_tmp291[1]={& _tmp293};Cyc_aprintf(({
const char*_tmp292="%d";_tag_arr(_tmp292,sizeof(char),_get_zero_arr_size(_tmp292,
3));}),_tag_arr(_tmp291,sizeof(void*),1));}});_LL15E: if(_tmp263 <= (void*)2  || *((
int*)_tmp263)!= 8)goto _LL160;_tmp272=((struct Cyc_Absyn_Char_p_struct*)_tmp263)->f1;
_LL15F: return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp296;_tmp296.tag=1;
_tmp296.f1=(unsigned int)((int)_tmp272);{void*_tmp294[1]={& _tmp296};Cyc_aprintf(({
const char*_tmp295="%d";_tag_arr(_tmp295,sizeof(char),_get_zero_arr_size(_tmp295,
3));}),_tag_arr(_tmp294,sizeof(void*),1));}});_LL160: if(_tmp263 <= (void*)2  || *((
int*)_tmp263)!= 9)goto _LL162;_tmp273=((struct Cyc_Absyn_Float_p_struct*)_tmp263)->f1;
_LL161: return _tmp273;_LL162: if(_tmp263 <= (void*)2  || *((int*)_tmp263)!= 10)goto
_LL164;_tmp274=((struct Cyc_Absyn_Enum_p_struct*)_tmp263)->f2;_LL163: _tmp275=
_tmp274;goto _LL165;_LL164: if(_tmp263 <= (void*)2  || *((int*)_tmp263)!= 11)goto
_LL166;_tmp275=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp263)->f2;_LL165: return
Cyc_Absynpp_qvar2string(_tmp275->name);_LL166: if(_tmp263 <= (void*)2  || *((int*)
_tmp263)!= 14)goto _LL168;_tmp276=((struct Cyc_Absyn_Exp_p_struct*)_tmp263)->f1;
_LL167: return Cyc_Absynpp_exp2string(_tmp276);_LL168:;_LL169:(int)_throw((void*)
Cyc_Tcpat_Desc2string);_LL147:;}}_LL145: if(*((int*)_tmp25D)!= 1)goto _LL142;
_tmp260=((struct Cyc_Tcpat_Neg_struct*)_tmp25D)->f1;_LL146: if(((int(*)(struct Cyc_Set_Set*
s))Cyc_Set_is_empty)(_tmp260))return({const char*_tmp297="_";_tag_arr(_tmp297,
sizeof(char),_get_zero_arr_size(_tmp297,2));});{struct Cyc_Tcpat_Con_s*_tmp298=((
struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp260);struct
Cyc_Absyn_Pat*_tmp299=_tmp298->orig_pat;if(_tmp299 == 0)(int)_throw((void*)Cyc_Tcpat_Desc2string);{
void*_tmp29A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp299->topt))->v);void*_tmp29B;struct Cyc_Absyn_PtrInfo _tmp29C;struct Cyc_Absyn_PtrAtts
_tmp29D;struct Cyc_Absyn_TunionInfo _tmp29E;void*_tmp29F;struct Cyc_Absyn_Tuniondecl**
_tmp2A0;struct Cyc_Absyn_Tuniondecl*_tmp2A1;_LL16B: if(_tmp29A <= (void*)4  || *((
int*)_tmp29A)!= 5)goto _LL16D;_tmp29B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp29A)->f2;if((int)_tmp29B != 0)goto _LL16D;_LL16C:{int i=0;for(0;i < 256;i ++){
struct Cyc_Tcpat_Con_s*_tmp2A2=Cyc_Tcpat_char_con((char)i,(struct Cyc_Absyn_Pat*)
_tmp299);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp260,_tmp2A2))return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp2A5;
_tmp2A5.tag=1;_tmp2A5.f1=(unsigned int)i;{void*_tmp2A3[1]={& _tmp2A5};Cyc_aprintf(({
const char*_tmp2A4="%d";_tag_arr(_tmp2A4,sizeof(char),_get_zero_arr_size(_tmp2A4,
3));}),_tag_arr(_tmp2A3,sizeof(void*),1));}});}}(int)_throw((void*)Cyc_Tcpat_Desc2string);
_LL16D: if(_tmp29A <= (void*)4  || *((int*)_tmp29A)!= 5)goto _LL16F;_LL16E:{
unsigned int i=0;for(0;i < 0 - 1;i ++){struct Cyc_Tcpat_Con_s*_tmp2A6=Cyc_Tcpat_int_con((
int)i,_tmp299);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp260,_tmp2A6))return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp2A9;
_tmp2A9.tag=1;_tmp2A9.f1=(unsigned int)((int)i);{void*_tmp2A7[1]={& _tmp2A9};Cyc_aprintf(({
const char*_tmp2A8="%d";_tag_arr(_tmp2A8,sizeof(char),_get_zero_arr_size(_tmp2A8,
3));}),_tag_arr(_tmp2A7,sizeof(void*),1));}});}}(int)_throw((void*)Cyc_Tcpat_Desc2string);
_LL16F: if(_tmp29A <= (void*)4  || *((int*)_tmp29A)!= 4)goto _LL171;_tmp29C=((struct
Cyc_Absyn_PointerType_struct*)_tmp29A)->f1;_tmp29D=_tmp29C.ptr_atts;_LL170: {
struct Cyc_Absyn_Conref*_tmp2AA=_tmp29D.nullable;int is_nullable=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2AA);if(is_nullable){if(!((
int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp260,
Cyc_Tcpat_null_con((struct Cyc_Absyn_Pat*)_tmp299)))return({const char*_tmp2AB="NULL";
_tag_arr(_tmp2AB,sizeof(char),_get_zero_arr_size(_tmp2AB,5));});}return({const
char*_tmp2AC="&_";_tag_arr(_tmp2AC,sizeof(char),_get_zero_arr_size(_tmp2AC,3));});}
_LL171: if(_tmp29A <= (void*)4  || *((int*)_tmp29A)!= 2)goto _LL173;_tmp29E=((struct
Cyc_Absyn_TunionType_struct*)_tmp29A)->f1;_tmp29F=(void*)_tmp29E.tunion_info;if(*((
int*)_tmp29F)!= 1)goto _LL173;_tmp2A0=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp29F)->f1;_tmp2A1=*_tmp2A0;_LL172: if(_tmp2A1->is_xtunion)(int)_throw((void*)
Cyc_Tcpat_Desc2string);{struct Cyc_List_List*_tmp2AD=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp2A1->fields))->v;int span=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp2AD);for(0;(unsigned int)_tmp2AD;_tmp2AD=_tmp2AD->tl){
struct _tagged_arr n=*(*((struct Cyc_Absyn_Tunionfield*)_tmp2AD->hd)->name).f2;
struct Cyc_List_List*_tmp2AE=((struct Cyc_Absyn_Tunionfield*)_tmp2AD->hd)->typs;
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp260,({struct Cyc_Tcpat_Con_s*_tmp2AF=_cycalloc(sizeof(*_tmp2AF));_tmp2AF->name=(
void*)((void*)({struct Cyc_Tcpat_Name_v_struct*_tmp2B0=_cycalloc(sizeof(*_tmp2B0));
_tmp2B0[0]=({struct Cyc_Tcpat_Name_v_struct _tmp2B1;_tmp2B1.tag=0;_tmp2B1.f1=n;
_tmp2B1;});_tmp2B0;}));_tmp2AF->arity=0;_tmp2AF->span=0;_tmp2AF->orig_pat=
_tmp299;_tmp2AF;}))){if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2AE)
== 0)return n;else{return(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp2B4;
_tmp2B4.tag=0;_tmp2B4.f1=(struct _tagged_arr)((struct _tagged_arr)n);{void*_tmp2B2[
1]={& _tmp2B4};Cyc_aprintf(({const char*_tmp2B3="&%s(...)";_tag_arr(_tmp2B3,
sizeof(char),_get_zero_arr_size(_tmp2B3,9));}),_tag_arr(_tmp2B2,sizeof(void*),1));}});}}}(
int)_throw((void*)Cyc_Tcpat_Desc2string);}_LL173:;_LL174:(int)_throw((void*)Cyc_Tcpat_Desc2string);
_LL16A:;}}_LL142:;}static struct _tagged_arr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _tagged_arr*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));_tmp2B5[0]=Cyc_Tcpat_desc2string(
d);_tmp2B5;});}static struct _tagged_arr Cyc_Tcpat_descs2string(struct Cyc_List_List*
ds){struct Cyc_List_List*_tmp2B6=((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);struct
_tagged_arr*comma=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),",",sizeof(
char),2);{struct Cyc_List_List*_tmp2B7=_tmp2B6;for(0;_tmp2B7 != 0;_tmp2B7=((struct
Cyc_List_List*)_check_null(_tmp2B7))->tl){if(_tmp2B7->tl != 0){_tmp2B7->tl=({
struct Cyc_List_List*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8->hd=comma;
_tmp2B8->tl=_tmp2B7->tl;_tmp2B8;});_tmp2B7=_tmp2B7->tl;}}}return(struct
_tagged_arr)Cyc_strconcat_l(_tmp2B6);}static void Cyc_Tcpat_not_exhaust_err(struct
Cyc_Position_Segment*loc,void*desc){struct _handler_cons _tmp2BA;_push_handler(&
_tmp2BA);{int _tmp2BC=0;if(setjmp(_tmp2BA.handler))_tmp2BC=1;if(!_tmp2BC){{struct
_tagged_arr _tmp2BD=Cyc_Tcpat_desc2string(desc);({struct Cyc_String_pa_struct
_tmp2C0;_tmp2C0.tag=0;_tmp2C0.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp2BD);{
void*_tmp2BE[1]={& _tmp2C0};Cyc_Tcutil_terr(loc,({const char*_tmp2BF="patterns may not be exhaustive.\n\tmissing case for %s";
_tag_arr(_tmp2BF,sizeof(char),_get_zero_arr_size(_tmp2BF,53));}),_tag_arr(
_tmp2BE,sizeof(void*),1));}});};_pop_handler();}else{void*_tmp2BB=(void*)
_exn_thrown;void*_tmp2C2=_tmp2BB;_LL176: if(_tmp2C2 != Cyc_Tcpat_Desc2string)goto
_LL178;_LL177:({void*_tmp2C3[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp2C4="patterns may not be exhaustive.";
_tag_arr(_tmp2C4,sizeof(char),_get_zero_arr_size(_tmp2C4,32));}),_tag_arr(
_tmp2C3,sizeof(void*),0));});goto _LL175;_LL178:;_LL179:(void)_throw(_tmp2C2);
_LL175:;}}}static void Cyc_Tcpat_rule_occurs(int dummy,struct _tuple14*rhs){(*rhs).f1=
1;}void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*loc,struct
Cyc_List_List*swcs){struct _RegionHandle _tmp2C5=_new_region("r");struct
_RegionHandle*r=& _tmp2C5;_push_region(r);{int _tmp2C6=0;struct _tuple16 _tmp2C7=({
struct _tuple16 _tmp2D3;_tmp2D3.f1=r;_tmp2D3.f2=& _tmp2C6;_tmp2D3;});struct Cyc_List_List*
_tmp2C8=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple15*(*f)(
struct _tuple16*,struct Cyc_Absyn_Switch_clause*),struct _tuple16*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp2C7,swcs);void*_tmp2C9=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp2C8);int _tmp2CA=0;((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp2C9,Cyc_Tcpat_not_exhaust_err,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp2CA);for(0;_tmp2C8 != 0;_tmp2C8=_tmp2C8->tl){
struct _tuple15 _tmp2CC;struct _tuple14*_tmp2CD;struct _tuple14 _tmp2CE;int _tmp2CF;
struct Cyc_Position_Segment*_tmp2D0;struct _tuple15*_tmp2CB=(struct _tuple15*)
_tmp2C8->hd;_tmp2CC=*_tmp2CB;_tmp2CD=_tmp2CC.f2;_tmp2CE=*_tmp2CD;_tmp2CF=_tmp2CE.f1;
_tmp2D0=_tmp2CE.f2;if(!_tmp2CF)({void*_tmp2D1[0]={};Cyc_Tcutil_terr(_tmp2D0,({
const char*_tmp2D2="redundant pattern";_tag_arr(_tmp2D2,sizeof(char),
_get_zero_arr_size(_tmp2D2,18));}),_tag_arr(_tmp2D1,sizeof(void*),0));});}};
_pop_region(r);}struct _tuple17{struct Cyc_Position_Segment*f1;int f2;};static void
Cyc_Tcpat_not_exhaust_warn(struct _tuple17*pr,void*desc){(*pr).f2=0;{struct
_handler_cons _tmp2D4;_push_handler(& _tmp2D4);{int _tmp2D6=0;if(setjmp(_tmp2D4.handler))
_tmp2D6=1;if(!_tmp2D6){{struct _tagged_arr _tmp2D7=Cyc_Tcpat_desc2string(desc);({
struct Cyc_String_pa_struct _tmp2DA;_tmp2DA.tag=0;_tmp2DA.f1=(struct _tagged_arr)((
struct _tagged_arr)_tmp2D7);{void*_tmp2D8[1]={& _tmp2DA};Cyc_Tcutil_warn((*pr).f1,({
const char*_tmp2D9="pattern not exhaustive.\n\tmissing case for %s";_tag_arr(
_tmp2D9,sizeof(char),_get_zero_arr_size(_tmp2D9,45));}),_tag_arr(_tmp2D8,sizeof(
void*),1));}});};_pop_handler();}else{void*_tmp2D5=(void*)_exn_thrown;void*
_tmp2DC=_tmp2D5;_LL17B: if(_tmp2DC != Cyc_Tcpat_Desc2string)goto _LL17D;_LL17C:({
void*_tmp2DD[0]={};Cyc_Tcutil_warn((*pr).f1,({const char*_tmp2DE="pattern not exhaustive.";
_tag_arr(_tmp2DE,sizeof(char),_get_zero_arr_size(_tmp2DE,24));}),_tag_arr(
_tmp2DD,sizeof(void*),0));});goto _LL17A;_LL17D:;_LL17E:(void)_throw(_tmp2DC);
_LL17A:;}}}}static void Cyc_Tcpat_dummy_fn(int i,int j){return;}struct _tuple18{void*
f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Pat*p){struct _RegionHandle _tmp2DF=_new_region("r");struct
_RegionHandle*r=& _tmp2DF;_push_region(r);{struct Cyc_List_List*_tmp2E0=({struct
Cyc_List_List*_tmp2E6=_region_malloc(r,sizeof(*_tmp2E6));_tmp2E6->hd=({struct
_tuple18*_tmp2E7=_region_malloc(r,sizeof(*_tmp2E7));_tmp2E7->f1=Cyc_Tcpat_compile_pat(
p);_tmp2E7->f2=0;_tmp2E7;});_tmp2E6->tl=0;_tmp2E6;});void*_tmp2E1=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp2E0);struct _tuple17 _tmp2E2=({struct _tuple17 _tmp2E5;_tmp2E5.f1=loc;_tmp2E5.f2=
1;_tmp2E5;});int _tmp2E3=0;((void(*)(void*d,void(*not_exhaust)(struct _tuple17*,
void*),struct _tuple17*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))
Cyc_Tcpat_check_exhaust_overlap)(_tmp2E1,Cyc_Tcpat_not_exhaust_warn,& _tmp2E2,Cyc_Tcpat_dummy_fn,
0,& _tmp2E3);{int _tmp2E4=_tmp2E2.f2;_npop_handler(0);return _tmp2E4;}};_pop_region(
r);}static struct _tuple15*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);struct _tuple14*rhs=({struct _tuple14*
_tmp2E9=_cycalloc(sizeof(*_tmp2E9));_tmp2E9->f1=0;_tmp2E9->f2=(swc->pattern)->loc;
_tmp2E9;});return({struct _tuple15*_tmp2E8=_cycalloc(sizeof(*_tmp2E8));_tmp2E8->f1=
sp0;_tmp2E8->f2=rhs;_tmp2E8;});}static void Cyc_Tcpat_not_exhaust_err2(struct Cyc_Position_Segment*
loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*swcs){struct Cyc_List_List*_tmp2EA=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcpat_get_match2,swcs);void*_tmp2EB=((void*(*)(struct _RegionHandle*r,struct
Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(Cyc_Core_heap_region,_tmp2EA);
int _tmp2EC=0;((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp2EB,Cyc_Tcpat_not_exhaust_err2,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp2EC);for(0;_tmp2EA != 0;_tmp2EA=_tmp2EA->tl){
struct _tuple15 _tmp2EE;struct _tuple14*_tmp2EF;struct _tuple14 _tmp2F0;int _tmp2F1;
struct Cyc_Position_Segment*_tmp2F2;struct _tuple15*_tmp2ED=(struct _tuple15*)
_tmp2EA->hd;_tmp2EE=*_tmp2ED;_tmp2EF=_tmp2EE.f2;_tmp2F0=*_tmp2EF;_tmp2F1=_tmp2F0.f1;
_tmp2F2=_tmp2F0.f2;if(!_tmp2F1)({void*_tmp2F3[0]={};Cyc_Tcutil_terr(_tmp2F2,({
const char*_tmp2F4="redundant pattern";_tag_arr(_tmp2F4,sizeof(char),
_get_zero_arr_size(_tmp2F4,18));}),_tag_arr(_tmp2F3,sizeof(void*),0));});}}
