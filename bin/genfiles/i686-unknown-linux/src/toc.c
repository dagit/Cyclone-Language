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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{int __pos;
Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _tagged_arr,struct _tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _tagged_arr f1;};int Cyc_vfprintf(struct Cyc___cycFILE*,struct _tagged_arr,
struct _tagged_arr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(
struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*
x,int n);int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);struct
_tagged_arr Cyc_strconcat(struct _tagged_arr,struct _tagged_arr);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*
Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*k);
struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[
14];struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};
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
struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct _tuple1*
name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct Cyc_Absyn_UnknownTunionInfo
f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};
struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*targs;void*rgn;}
;struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*tunion_name;struct _tuple1*
field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int tag;
struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(struct
_tuple1*,struct _tuple1*);extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value;
extern void*Cyc_Absyn_rel_ns_null;struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(
void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_tagged_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _tagged_arr*name);void*Cyc_Absyn_strctq(
struct _tuple1*name);void*Cyc_Absyn_unionq_typ(struct _tuple1*name);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _tagged_arr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _tagged_arr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple1*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple1*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _tagged_arr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _tagged_arr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*
r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct
Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct _tagged_arr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_tagged_arr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple1*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);
struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};void*Cyc_Tcutil_snd_tqt(
struct _tuple4*);struct _tuple5{unsigned int f1;int f2;};struct _tuple5 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
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
int tag;struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};struct Cyc_List_List*Cyc_Toc_toc(
struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();int Cyc_Toc_warn_bounds_checks=
0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;static struct Cyc_List_List*Cyc_Toc_tuple_types=0;static int Cyc_Toc_tuple_type_counter=
0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=0;
char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";char
Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*Cyc_Toc_unimp(
struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({void*
_tmp0[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="\n";_tag_arr(_tmp1,sizeof(
char),_get_zero_arr_size(_tmp1,2));}),_tag_arr(_tmp0,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Toc_Toc_Unimplemented);}
static void*Cyc_Toc_toc_impos(struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);({void*_tmp2[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp3="\n";
_tag_arr(_tmp3,sizeof(char),_get_zero_arr_size(_tmp3,2));}),_tag_arr(_tmp2,
sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((
void*)Cyc_Toc_Toc_Impossible);}char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error\000";
static char _tmp4[5]="curr";static struct _tagged_arr Cyc_Toc_curr_string={_tmp4,
_tmp4,_tmp4 + 5};static struct _tagged_arr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static char _tmp5[4]="tag";static struct _tagged_arr Cyc_Toc_tag_string={_tmp5,_tmp5,
_tmp5 + 4};static struct _tagged_arr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char
_tmp6[14]="_handler_cons";static struct _tagged_arr Cyc_Toc__handler_cons_string={
_tmp6,_tmp6,_tmp6 + 14};static struct _tagged_arr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp7[8]="handler";static struct _tagged_arr Cyc_Toc_handler_string={
_tmp7,_tmp7,_tmp7 + 8};static struct _tagged_arr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp8[14]="_RegionHandle";static struct _tagged_arr Cyc_Toc__RegionHandle_string={
_tmp8,_tmp8,_tmp8 + 14};static struct _tagged_arr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmp9[7]="_throw";static struct _tagged_arr Cyc_Toc__throw_str={_tmp9,
_tmp9,_tmp9 + 7};static struct _tuple1 Cyc_Toc__throw_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)((void*)&
Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmpB[7]="setjmp";static struct
_tagged_arr Cyc_Toc_setjmp_str={_tmpB,_tmpB,_tmpB + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc_setjmp_ev={0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmpD[14]="_push_handler";
static struct _tagged_arr Cyc_Toc__push_handler_str={_tmpD,_tmpD,_tmpD + 14};static
struct _tuple1 Cyc_Toc__push_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((
void*)& Cyc_Toc__push_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char
_tmpF[13]="_pop_handler";static struct _tagged_arr Cyc_Toc__pop_handler_str={_tmpF,
_tmpF,_tmpF + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmp11[12]="_exn_thrown";static struct _tagged_arr Cyc_Toc__exn_thrown_str={
_tmp11,_tmp11,_tmp11 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={
0,(void*)((void*)& Cyc_Toc__exn_thrown_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static
char _tmp13[14]="_npop_handler";static struct _tagged_arr Cyc_Toc__npop_handler_str={
_tmp13,_tmp13,_tmp13 + 14};static struct _tuple1 Cyc_Toc__npop_handler_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)((void*)& Cyc_Toc__npop_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev;static char _tmp15[12]="_check_null";static struct
_tagged_arr Cyc_Toc__check_null_str={_tmp15,_tmp15,_tmp15 + 12};static struct
_tuple1 Cyc_Toc__check_null_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((
void*)& Cyc_Toc__check_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char
_tmp17[28]="_check_known_subscript_null";static struct _tagged_arr Cyc_Toc__check_known_subscript_null_str={
_tmp17,_tmp17,_tmp17 + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp19[31]="_check_known_subscript_notnull";static struct _tagged_arr Cyc_Toc__check_known_subscript_notnull_str={
_tmp19,_tmp19,_tmp19 + 31};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp1B[25]="_check_unknown_subscript";
static struct _tagged_arr Cyc_Toc__check_unknown_subscript_str={_tmp1B,_tmp1B,
_tmp1B + 25};static struct _tuple1 Cyc_Toc__check_unknown_subscript_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_unknown_subscript_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1,& Cyc_Toc__check_unknown_subscript_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_unknown_subscript_ev={0,(void*)((void*)& Cyc_Toc__check_unknown_subscript_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_unknown_subscript_e=&
Cyc_Toc__check_unknown_subscript_ev;static char _tmp1D[12]="_tagged_arr";static
struct _tagged_arr Cyc_Toc__tagged_arr_str={_tmp1D,_tmp1D,_tmp1D + 12};static struct
_tuple1 Cyc_Toc__tagged_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_re={1,& Cyc_Toc__tagged_arr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={0,(void*)((
void*)& Cyc_Toc__tagged_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_e=& Cyc_Toc__tagged_arr_ev;static char
_tmp1F[9]="_tag_arr";static struct _tagged_arr Cyc_Toc__tag_arr_str={_tmp1F,_tmp1F,
_tmp1F + 9};static struct _tuple1 Cyc_Toc__tag_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_arr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_arr_re={1,&
Cyc_Toc__tag_arr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={
0,(void*)((void*)& Cyc_Toc__tag_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_arr_e=& Cyc_Toc__tag_arr_ev;static char
_tmp21[14]="_init_tag_arr";static struct _tagged_arr Cyc_Toc__init_tag_arr_str={
_tmp21,_tmp21,_tmp21 + 14};static struct _tuple1 Cyc_Toc__init_tag_arr_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__init_tag_arr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__init_tag_arr_re={1,& Cyc_Toc__init_tag_arr_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__init_tag_arr_ev={0,(void*)((void*)& Cyc_Toc__init_tag_arr_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__init_tag_arr_e=&
Cyc_Toc__init_tag_arr_ev;static char _tmp23[11]="_untag_arr";static struct
_tagged_arr Cyc_Toc__untag_arr_str={_tmp23,_tmp23,_tmp23 + 11};static struct _tuple1
Cyc_Toc__untag_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_arr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_arr_re={1,& Cyc_Toc__untag_arr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={0,(void*)((
void*)& Cyc_Toc__untag_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__untag_arr_e=& Cyc_Toc__untag_arr_ev;static char _tmp25[
14]="_get_arr_size";static struct _tagged_arr Cyc_Toc__get_arr_size_str={_tmp25,
_tmp25,_tmp25 + 14};static struct _tuple1 Cyc_Toc__get_arr_size_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__get_arr_size_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_arr_size_re={
1,& Cyc_Toc__get_arr_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_arr_size_ev={
0,(void*)((void*)& Cyc_Toc__get_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_arr_size_e=& Cyc_Toc__get_arr_size_ev;
static char _tmp27[19]="_get_zero_arr_size";static struct _tagged_arr Cyc_Toc__get_zero_arr_size_str={
_tmp27,_tmp27,_tmp27 + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__get_zero_arr_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp29[17]="_tagged_arr_plus";static struct _tagged_arr Cyc_Toc__tagged_arr_plus_str={
_tmp29,_tmp29,_tmp29 + 17};static struct _tuple1 Cyc_Toc__tagged_arr_plus_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tagged_arr_plus_re={1,& Cyc_Toc__tagged_arr_plus_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_plus_ev={0,(void*)((void*)& Cyc_Toc__tagged_arr_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_plus_e=&
Cyc_Toc__tagged_arr_plus_ev;static char _tmp2B[15]="_zero_arr_plus";static struct
_tagged_arr Cyc_Toc__zero_arr_plus_str={_tmp2B,_tmp2B,_tmp2B + 15};static struct
_tuple1 Cyc_Toc__zero_arr_plus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__zero_arr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static
char _tmp2D[25]="_tagged_arr_inplace_plus";static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_str={
_tmp2D,_tmp2D,_tmp2D + 25};static struct _tuple1 Cyc_Toc__tagged_arr_inplace_plus_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={1,& Cyc_Toc__tagged_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__tagged_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_inplace_plus_e=& Cyc_Toc__tagged_arr_inplace_plus_ev;
static char _tmp2F[23]="_zero_arr_inplace_plus";static struct _tagged_arr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp2F,_tmp2F,_tmp2F + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__zero_arr_inplace_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp31[30]="_tagged_arr_inplace_plus_post";static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={
_tmp31,_tmp31,_tmp31 + 30};static struct _tuple1 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={1,&
Cyc_Toc__tagged_arr_inplace_plus_post_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_inplace_plus_post_e=&
Cyc_Toc__tagged_arr_inplace_plus_post_ev;static char _tmp33[28]="_zero_arr_inplace_plus_post";
static struct _tagged_arr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp33,_tmp33,
_tmp33 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__zero_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp35[10]="_cycalloc";static struct _tagged_arr Cyc_Toc__cycalloc_str={
_tmp35,_tmp35,_tmp35 + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char
_tmp37[11]="_cyccalloc";static struct _tagged_arr Cyc_Toc__cyccalloc_str={_tmp37,
_tmp37,_tmp37 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={
1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char
_tmp39[17]="_cycalloc_atomic";static struct _tagged_arr Cyc_Toc__cycalloc_atomic_str={
_tmp39,_tmp39,_tmp39 + 17};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=&
Cyc_Toc__cycalloc_atomic_ev;static char _tmp3B[18]="_cyccalloc_atomic";static
struct _tagged_arr Cyc_Toc__cyccalloc_atomic_str={_tmp3B,_tmp3B,_tmp3B + 18};static
struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={
1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmp3D[15]="_region_malloc";static struct _tagged_arr Cyc_Toc__region_malloc_str={
_tmp3D,_tmp3D,_tmp3D + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((void*)& Cyc_Toc__region_malloc_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev;static char _tmp3F[15]="_region_calloc";static struct
_tagged_arr Cyc_Toc__region_calloc_str={_tmp3F,_tmp3F,_tmp3F + 15};static struct
_tuple1 Cyc_Toc__region_calloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_calloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)((
void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static
char _tmp41[13]="_check_times";static struct _tagged_arr Cyc_Toc__check_times_str={
_tmp41,_tmp41,_tmp41 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static char _tmp43[12]="_new_region";static struct
_tagged_arr Cyc_Toc__new_region_str={_tmp43,_tmp43,_tmp43 + 12};static struct
_tuple1 Cyc_Toc__new_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__new_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)((
void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char
_tmp45[13]="_push_region";static struct _tagged_arr Cyc_Toc__push_region_str={
_tmp45,_tmp45,_tmp45 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static char _tmp47[12]="_pop_region";static struct
_tagged_arr Cyc_Toc__pop_region_str={_tmp47,_tmp47,_tmp47 + 12};static struct
_tuple1 Cyc_Toc__pop_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)((
void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char
_tmp49[14]="_reset_region";static struct _tagged_arr Cyc_Toc__reset_region_str={
_tmp49,_tmp49,_tmp49 + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)((void*)& Cyc_Toc__reset_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=&
Cyc_Toc__reset_region_ev;static char _tmp4B[19]="_throw_arraybounds";static struct
_tagged_arr Cyc_Toc__throw_arraybounds_str={_tmp4B,_tmp4B,_tmp4B + 19};static
struct _tuple1 Cyc_Toc__throw_arraybounds_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={
1,& Cyc_Toc__throw_arraybounds_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={
0,(void*)((void*)& Cyc_Toc__throw_arraybounds_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmp4D[26]="_tagged_ptr_decrease_size";static struct _tagged_arr Cyc_Toc__tagged_ptr_decrease_size_str={
_tmp4D,_tmp4D,_tmp4D + 26};static struct _tuple1 Cyc_Toc__tagged_ptr_decrease_size_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_ptr_decrease_size_re={1,& Cyc_Toc__tagged_ptr_decrease_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_ptr_decrease_size_ev={
0,(void*)((void*)& Cyc_Toc__tagged_ptr_decrease_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_ptr_decrease_size_e=& Cyc_Toc__tagged_ptr_decrease_size_ev;
static struct Cyc_Absyn_UnknownAggr_struct Cyc_Toc_tagged_arr_typ_u={0,(void*)((
void*)0),& Cyc_Toc__tagged_arr_pr};static struct Cyc_Absyn_AggrType_struct Cyc_Toc_tagged_arr_typ_v={
10,{(void*)((void*)& Cyc_Toc_tagged_arr_typ_u),0}};static void*Cyc_Toc_tagged_arr_typ=(
void*)& Cyc_Toc_tagged_arr_typ_v;static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,
0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**
skip_stmt_opt=0;if(skip_stmt_opt == 0)skip_stmt_opt=({struct Cyc_Absyn_Stmt**
_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51[0]=Cyc_Absyn_skip_stmt(0);_tmp51;});
return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*
e){return Cyc_Absyn_cast_exp(t,e,0,(void*)1,0);}struct _tuple6{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*
_tmp52=(void*)e->r;void*_tmp53;char _tmp54;void*_tmp55;short _tmp56;void*_tmp57;
int _tmp58;void*_tmp59;long long _tmp5A;void*_tmp5B;struct Cyc_Absyn_Exp*_tmp5C;
struct Cyc_List_List*_tmp5D;struct Cyc_List_List*_tmp5E;struct Cyc_List_List*_tmp5F;
struct Cyc_List_List*_tmp60;struct Cyc_List_List*_tmp61;_LL1: if(*((int*)_tmp52)!= 
0)goto _LL3;_tmp53=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp52)->f1;if(_tmp53
<= (void*)1  || *((int*)_tmp53)!= 0)goto _LL3;_tmp54=((struct Cyc_Absyn_Char_c_struct*)
_tmp53)->f2;_LL2: return _tmp54 == '\000';_LL3: if(*((int*)_tmp52)!= 0)goto _LL5;
_tmp55=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp52)->f1;if(_tmp55 <= (void*)1
 || *((int*)_tmp55)!= 1)goto _LL5;_tmp56=((struct Cyc_Absyn_Short_c_struct*)_tmp55)->f2;
_LL4: return _tmp56 == 0;_LL5: if(*((int*)_tmp52)!= 0)goto _LL7;_tmp57=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp52)->f1;if(_tmp57 <= (void*)1  || *((int*)
_tmp57)!= 2)goto _LL7;_tmp58=((struct Cyc_Absyn_Int_c_struct*)_tmp57)->f2;_LL6:
return _tmp58 == 0;_LL7: if(*((int*)_tmp52)!= 0)goto _LL9;_tmp59=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp52)->f1;if(_tmp59 <= (void*)1  || *((int*)_tmp59)!= 3)goto _LL9;_tmp5A=((struct
Cyc_Absyn_LongLong_c_struct*)_tmp59)->f2;_LL8: return _tmp5A == 0;_LL9: if(*((int*)
_tmp52)!= 0)goto _LLB;_tmp5B=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp52)->f1;
if((int)_tmp5B != 0)goto _LLB;_LLA: return 1;_LLB: if(*((int*)_tmp52)!= 15)goto _LLD;
_tmp5C=((struct Cyc_Absyn_Cast_e_struct*)_tmp52)->f2;_LLC: return Cyc_Toc_is_zero(
_tmp5C);_LLD: if(*((int*)_tmp52)!= 26)goto _LLF;_tmp5D=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp52)->f1;_LLE: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp5D);_LLF: if(*((int*)_tmp52)!= 28)goto
_LL11;_tmp5E=((struct Cyc_Absyn_Array_e_struct*)_tmp52)->f1;_LL10: _tmp5F=_tmp5E;
goto _LL12;_LL11: if(*((int*)_tmp52)!= 30)goto _LL13;_tmp5F=((struct Cyc_Absyn_Struct_e_struct*)
_tmp52)->f3;_LL12: _tmp60=_tmp5F;goto _LL14;_LL13: if(*((int*)_tmp52)!= 27)goto
_LL15;_tmp60=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp52)->f2;_LL14: _tmp61=
_tmp60;goto _LL16;_LL15: if(*((int*)_tmp52)!= 36)goto _LL17;_tmp61=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp52)->f2;_LL16: for(0;_tmp61 != 0;_tmp61=_tmp61->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple6*)_tmp61->hd)).f2))return 0;}return 1;_LL17:;_LL18: return 0;_LL0:;}
static int Cyc_Toc_is_nullable(void*t){void*_tmp62=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmp63;struct Cyc_Absyn_PtrAtts _tmp64;struct Cyc_Absyn_Conref*
_tmp65;_LL1A: if(_tmp62 <= (void*)3  || *((int*)_tmp62)!= 4)goto _LL1C;_tmp63=((
struct Cyc_Absyn_PointerType_struct*)_tmp62)->f1;_tmp64=_tmp63.ptr_atts;_tmp65=
_tmp64.nullable;_LL1B: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp65);_LL1C:;_LL1D:({void*_tmp66[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp67="is_nullable";_tag_arr(
_tmp67,sizeof(char),_get_zero_arr_size(_tmp67,12));}),_tag_arr(_tmp66,sizeof(
void*),0));});_LL19:;}static struct _tuple1*Cyc_Toc_collapse_qvar_tag(struct
_tuple1*x,struct _tagged_arr tag){return({struct _tuple1*_tmp68=_cycalloc(sizeof(*
_tmp68));_tmp68->f1=(*x).f1;_tmp68->f2=({struct _tagged_arr*_tmp69=_cycalloc(
sizeof(*_tmp69));_tmp69[0]=(struct _tagged_arr)Cyc_strconcat((struct _tagged_arr)*(*
x).f2,(struct _tagged_arr)tag);_tmp69;});_tmp68;});}struct _tuple7{struct
_tagged_arr*f1;struct Cyc_List_List*f2;};static struct _tagged_arr*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){{struct Cyc_List_List*_tmp6A=Cyc_Toc_tuple_types;for(0;
_tmp6A != 0;_tmp6A=_tmp6A->tl){struct _tuple7 _tmp6C;struct _tagged_arr*_tmp6D;
struct Cyc_List_List*_tmp6E;struct _tuple7*_tmp6B=(struct _tuple7*)_tmp6A->hd;
_tmp6C=*_tmp6B;_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;{struct Cyc_List_List*_tmp6F=
tqs0;for(0;_tmp6F != 0  && _tmp6E != 0;(_tmp6F=_tmp6F->tl,_tmp6E=_tmp6E->tl)){if(!
Cyc_Tcutil_unify((*((struct _tuple4*)_tmp6F->hd)).f2,(void*)_tmp6E->hd))break;}
if(_tmp6F == 0  && _tmp6E == 0)return _tmp6D;}}}{struct _tagged_arr*x=({struct
_tagged_arr*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp7F;_tmp7F.tag=1;_tmp7F.f1=(unsigned int)Cyc_Toc_tuple_type_counter
++;{void*_tmp7D[1]={& _tmp7F};Cyc_aprintf(({const char*_tmp7E="_tuple%d";_tag_arr(
_tmp7E,sizeof(char),_get_zero_arr_size(_tmp7E,9));}),_tag_arr(_tmp7D,sizeof(void*),
1));}});_tmp7C;});struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmp70=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(
ts2=ts2->tl,i ++)){_tmp70=({struct Cyc_List_List*_tmp71=_cycalloc(sizeof(*_tmp71));
_tmp71->hd=({struct Cyc_Absyn_Aggrfield*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->name=
Cyc_Absyn_fieldname(i);_tmp72->tq=Cyc_Toc_mt_tq;_tmp72->type=(void*)((void*)ts2->hd);
_tmp72->width=0;_tmp72->attributes=0;_tmp72;});_tmp71->tl=_tmp70;_tmp71;});}}
_tmp70=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp70);{
struct Cyc_Absyn_Aggrdecl*_tmp73=({struct Cyc_Absyn_Aggrdecl*_tmp79=_cycalloc(
sizeof(*_tmp79));_tmp79->kind=(void*)((void*)0);_tmp79->sc=(void*)((void*)2);
_tmp79->name=({struct _tuple1*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->f1=Cyc_Absyn_rel_ns_null;
_tmp7B->f2=x;_tmp7B;});_tmp79->tvs=0;_tmp79->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->exist_vars=0;_tmp7A->rgn_po=0;_tmp7A->fields=
_tmp70;_tmp7A;});_tmp79->attributes=0;_tmp79;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->hd=Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Aggr_d_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp76;_tmp76.tag=4;_tmp76.f1=_tmp73;_tmp76;});_tmp75;}),0);_tmp74->tl=Cyc_Toc_result_decls;
_tmp74;});Cyc_Toc_tuple_types=({struct Cyc_List_List*_tmp77=_cycalloc(sizeof(*
_tmp77));_tmp77->hd=({struct _tuple7*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->f1=
x;_tmp78->f2=ts;_tmp78;});_tmp77->tl=Cyc_Toc_tuple_types;_tmp77;});return x;}}}
struct _tuple1*Cyc_Toc_temp_var(){return({struct _tuple1*_tmp80=_cycalloc(sizeof(*
_tmp80));_tmp80->f1=(void*)0;_tmp80->f2=({struct _tagged_arr*_tmp81=_cycalloc(
sizeof(*_tmp81));_tmp81[0]=(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp84;
_tmp84.tag=1;_tmp84.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;{void*_tmp82[1]={&
_tmp84};Cyc_aprintf(({const char*_tmp83="_tmp%X";_tag_arr(_tmp83,sizeof(char),
_get_zero_arr_size(_tmp83,7));}),_tag_arr(_tmp82,sizeof(void*),1));}});_tmp81;});
_tmp80;});}static struct _tagged_arr*Cyc_Toc_fresh_label(){return({struct
_tagged_arr*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp88;_tmp88.tag=1;_tmp88.f1=(unsigned int)Cyc_Toc_fresh_label_counter
++;{void*_tmp86[1]={& _tmp88};Cyc_aprintf(({const char*_tmp87="_LL%X";_tag_arr(
_tmp87,sizeof(char),_get_zero_arr_size(_tmp87,6));}),_tag_arr(_tmp86,sizeof(void*),
1));}});_tmp85;});}static struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*
td,struct _tuple1*name,int carries_value){int ans=0;struct Cyc_List_List*_tmp89=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(
name,((struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp89))->hd)->name)
!= 0){if(carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp89->hd)->typs != 0
 || !carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp89->hd)->typs == 0)++ ans;
_tmp89=_tmp89->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static int Cyc_Toc_num_void_tags(
struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*_tmp8A=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;_tmp8A != 0;_tmp8A=_tmp8A->tl){
if(((struct Cyc_Absyn_Tunionfield*)_tmp8A->hd)->typs == 0)++ ans;}}return ans;}
static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*Cyc_Toc_arg_to_c(struct
_tuple2*a){struct Cyc_Core_Opt*_tmp8C;struct Cyc_Absyn_Tqual _tmp8D;void*_tmp8E;
struct _tuple2 _tmp8B=*a;_tmp8C=_tmp8B.f1;_tmp8D=_tmp8B.f2;_tmp8E=_tmp8B.f3;return({
struct _tuple2*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->f1=_tmp8C;_tmp8F->f2=
_tmp8D;_tmp8F->f3=Cyc_Toc_typ_to_c(_tmp8E);_tmp8F;});}static struct _tuple4*Cyc_Toc_typ_to_c_f(
struct _tuple4*x){struct Cyc_Absyn_Tqual _tmp91;void*_tmp92;struct _tuple4 _tmp90=*x;
_tmp91=_tmp90.f1;_tmp92=_tmp90.f2;return({struct _tuple4*_tmp93=_cycalloc(sizeof(*
_tmp93));_tmp93->f1=_tmp91;_tmp93->f2=Cyc_Toc_typ_to_c(_tmp92);_tmp93;});}static
void*Cyc_Toc_typ_to_c_array(void*t){void*_tmp94=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo
_tmp95;void*_tmp96;struct Cyc_Absyn_Tqual _tmp97;struct Cyc_Absyn_Exp*_tmp98;struct
Cyc_Absyn_Conref*_tmp99;struct Cyc_Position_Segment*_tmp9A;struct Cyc_Core_Opt*
_tmp9B;struct Cyc_Core_Opt _tmp9C;void*_tmp9D;_LL1F: if(_tmp94 <= (void*)3  || *((int*)
_tmp94)!= 7)goto _LL21;_tmp95=((struct Cyc_Absyn_ArrayType_struct*)_tmp94)->f1;
_tmp96=(void*)_tmp95.elt_type;_tmp97=_tmp95.tq;_tmp98=_tmp95.num_elts;_tmp99=
_tmp95.zero_term;_tmp9A=_tmp95.zt_loc;_LL20: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp96),_tmp97,_tmp98,Cyc_Absyn_false_conref,_tmp9A);_LL21: if(_tmp94 <= (void*)3
 || *((int*)_tmp94)!= 0)goto _LL23;_tmp9B=((struct Cyc_Absyn_Evar_struct*)_tmp94)->f2;
if(_tmp9B == 0)goto _LL23;_tmp9C=*_tmp9B;_tmp9D=(void*)_tmp9C.v;_LL22: return Cyc_Toc_typ_to_c_array(
_tmp9D);_LL23:;_LL24: return Cyc_Toc_typ_to_c(t);_LL1E:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->name=f->name;_tmp9E->tq=f->tq;_tmp9E->type=(
void*)Cyc_Toc_typ_to_c((void*)f->type);_tmp9E->width=f->width;_tmp9E->attributes=
f->attributes;_tmp9E;});}static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*
fs){return;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp9F=t;struct Cyc_Core_Opt*
_tmpA0;struct Cyc_Core_Opt*_tmpA1;struct Cyc_Core_Opt _tmpA2;void*_tmpA3;struct Cyc_Absyn_Tvar*
_tmpA4;struct Cyc_Absyn_TunionInfo _tmpA5;void*_tmpA6;struct Cyc_Absyn_TunionFieldInfo
_tmpA7;void*_tmpA8;struct Cyc_Absyn_Tuniondecl*_tmpA9;struct Cyc_Absyn_Tunionfield*
_tmpAA;struct Cyc_Absyn_PtrInfo _tmpAB;void*_tmpAC;struct Cyc_Absyn_Tqual _tmpAD;
struct Cyc_Absyn_PtrAtts _tmpAE;struct Cyc_Absyn_Conref*_tmpAF;struct Cyc_Absyn_ArrayInfo
_tmpB0;void*_tmpB1;struct Cyc_Absyn_Tqual _tmpB2;struct Cyc_Absyn_Exp*_tmpB3;struct
Cyc_Position_Segment*_tmpB4;struct Cyc_Absyn_FnInfo _tmpB5;void*_tmpB6;struct Cyc_List_List*
_tmpB7;int _tmpB8;struct Cyc_Absyn_VarargInfo*_tmpB9;struct Cyc_List_List*_tmpBA;
struct Cyc_List_List*_tmpBB;void*_tmpBC;struct Cyc_List_List*_tmpBD;struct Cyc_Absyn_AggrInfo
_tmpBE;void*_tmpBF;struct Cyc_List_List*_tmpC0;struct _tuple1*_tmpC1;struct Cyc_List_List*
_tmpC2;struct _tuple1*_tmpC3;struct Cyc_List_List*_tmpC4;struct Cyc_Absyn_Typedefdecl*
_tmpC5;void**_tmpC6;void*_tmpC7;_LL26: if((int)_tmp9F != 0)goto _LL28;_LL27: return t;
_LL28: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 0)goto _LL2A;_tmpA0=((struct Cyc_Absyn_Evar_struct*)
_tmp9F)->f2;if(_tmpA0 != 0)goto _LL2A;_LL29: return Cyc_Absyn_sint_typ;_LL2A: if(
_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 0)goto _LL2C;_tmpA1=((struct Cyc_Absyn_Evar_struct*)
_tmp9F)->f2;if(_tmpA1 == 0)goto _LL2C;_tmpA2=*_tmpA1;_tmpA3=(void*)_tmpA2.v;_LL2B:
return Cyc_Toc_typ_to_c(_tmpA3);_LL2C: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 1)
goto _LL2E;_tmpA4=((struct Cyc_Absyn_VarType_struct*)_tmp9F)->f1;_LL2D: if(Cyc_Tcutil_tvar_kind(
_tmpA4)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}_LL2E:
if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 2)goto _LL30;_tmpA5=((struct Cyc_Absyn_TunionType_struct*)
_tmp9F)->f1;_tmpA6=(void*)_tmpA5.tunion_info;if(*((int*)_tmpA6)!= 1)goto _LL30;
_LL2F: return Cyc_Absyn_void_star_typ();_LL30: if(_tmp9F <= (void*)3  || *((int*)
_tmp9F)!= 2)goto _LL32;_LL31:({void*_tmpC8[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmpC9="unresolved TunionType";
_tag_arr(_tmpC9,sizeof(char),_get_zero_arr_size(_tmpC9,22));}),_tag_arr(_tmpC8,
sizeof(void*),0));});_LL32: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 3)goto _LL34;
_tmpA7=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp9F)->f1;_tmpA8=(void*)
_tmpA7.field_info;if(*((int*)_tmpA8)!= 1)goto _LL34;_tmpA9=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpA8)->f1;_tmpAA=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpA8)->f2;_LL33:
if(_tmpAA->typs == 0){if(_tmpA9->is_xtunion)return Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);else{return Cyc_Absyn_uint_typ;}}
else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmpAA->name,({const char*
_tmpCA="_struct";_tag_arr(_tmpCA,sizeof(char),_get_zero_arr_size(_tmpCA,8));})));}
_LL34: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 3)goto _LL36;_LL35:({void*_tmpCB[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpCC="unresolved TunionFieldType";_tag_arr(_tmpCC,sizeof(char),
_get_zero_arr_size(_tmpCC,27));}),_tag_arr(_tmpCB,sizeof(void*),0));});_LL36: if(
_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 4)goto _LL38;_tmpAB=((struct Cyc_Absyn_PointerType_struct*)
_tmp9F)->f1;_tmpAC=(void*)_tmpAB.elt_typ;_tmpAD=_tmpAB.elt_tq;_tmpAE=_tmpAB.ptr_atts;
_tmpAF=_tmpAE.bounds;_LL37: _tmpAC=Cyc_Toc_typ_to_c_array(_tmpAC);{void*_tmpCD=(
void*)(Cyc_Absyn_compress_conref(_tmpAF))->v;void*_tmpCE;_LL5F: if((int)_tmpCD != 
0)goto _LL61;_LL60: goto _LL62;_LL61: if(_tmpCD <= (void*)1  || *((int*)_tmpCD)!= 0)
goto _LL63;_tmpCE=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpCD)->f1;if((int)
_tmpCE != 0)goto _LL63;_LL62: return Cyc_Toc_tagged_arr_typ;_LL63:;_LL64: return Cyc_Absyn_star_typ(
_tmpAC,(void*)2,_tmpAD,Cyc_Absyn_false_conref);_LL5E:;}_LL38: if(_tmp9F <= (void*)
3  || *((int*)_tmp9F)!= 5)goto _LL3A;_LL39: goto _LL3B;_LL3A: if((int)_tmp9F != 1)goto
_LL3C;_LL3B: goto _LL3D;_LL3C: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 6)goto
_LL3E;_LL3D: return t;_LL3E: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 7)goto _LL40;
_tmpB0=((struct Cyc_Absyn_ArrayType_struct*)_tmp9F)->f1;_tmpB1=(void*)_tmpB0.elt_type;
_tmpB2=_tmpB0.tq;_tmpB3=_tmpB0.num_elts;_tmpB4=_tmpB0.zt_loc;_LL3F: return Cyc_Absyn_array_typ(
Cyc_Toc_typ_to_c_array(_tmpB1),_tmpB2,_tmpB3,Cyc_Absyn_false_conref,_tmpB4);
_LL40: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 8)goto _LL42;_tmpB5=((struct Cyc_Absyn_FnType_struct*)
_tmp9F)->f1;_tmpB6=(void*)_tmpB5.ret_typ;_tmpB7=_tmpB5.args;_tmpB8=_tmpB5.c_varargs;
_tmpB9=_tmpB5.cyc_varargs;_tmpBA=_tmpB5.attributes;_LL41: {struct Cyc_List_List*
_tmpCF=0;for(0;_tmpBA != 0;_tmpBA=_tmpBA->tl){void*_tmpD0=(void*)_tmpBA->hd;_LL66:
if((int)_tmpD0 != 3)goto _LL68;_LL67: goto _LL69;_LL68: if((int)_tmpD0 != 4)goto _LL6A;
_LL69: goto _LL6B;_LL6A: if(_tmpD0 <= (void*)17  || *((int*)_tmpD0)!= 3)goto _LL6C;
_LL6B: continue;_LL6C: if(_tmpD0 <= (void*)17  || *((int*)_tmpD0)!= 4)goto _LL6E;
_LL6D: continue;_LL6E:;_LL6F: _tmpCF=({struct Cyc_List_List*_tmpD1=_cycalloc(
sizeof(*_tmpD1));_tmpD1->hd=(void*)((void*)_tmpBA->hd);_tmpD1->tl=_tmpCF;_tmpD1;});
goto _LL65;_LL65:;}{struct Cyc_List_List*_tmpD2=((struct Cyc_List_List*(*)(struct
_tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmpB7);if(_tmpB9 != 0){void*_tmpD3=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ((void*)
_tmpB9->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct _tuple2*
_tmpD4=({struct _tuple2*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->f1=_tmpB9->name;
_tmpD6->f2=_tmpB9->tq;_tmpD6->f3=_tmpD3;_tmpD6;});_tmpD2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpD2,({
struct Cyc_List_List*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5->hd=_tmpD4;_tmpD5->tl=
0;_tmpD5;}));}return(void*)({struct Cyc_Absyn_FnType_struct*_tmpD7=_cycalloc(
sizeof(*_tmpD7));_tmpD7[0]=({struct Cyc_Absyn_FnType_struct _tmpD8;_tmpD8.tag=8;
_tmpD8.f1=({struct Cyc_Absyn_FnInfo _tmpD9;_tmpD9.tvars=0;_tmpD9.effect=0;_tmpD9.ret_typ=(
void*)Cyc_Toc_typ_to_c(_tmpB6);_tmpD9.args=_tmpD2;_tmpD9.c_varargs=_tmpB8;_tmpD9.cyc_varargs=
0;_tmpD9.rgn_po=0;_tmpD9.attributes=_tmpCF;_tmpD9;});_tmpD8;});_tmpD7;});}}_LL42:
if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 9)goto _LL44;_tmpBB=((struct Cyc_Absyn_TupleType_struct*)
_tmp9F)->f1;_LL43: _tmpBB=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmpBB);{
struct _tagged_arr*_tmpDA=Cyc_Toc_add_tuple_type(_tmpBB);return Cyc_Absyn_strct(
_tmpDA);}_LL44: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 11)goto _LL46;_tmpBC=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp9F)->f1;_tmpBD=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp9F)->f2;_LL45: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpDB=
_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpDC;
_tmpDC.tag=11;_tmpDC.f1=(void*)_tmpBC;_tmpDC.f2=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_aggrfield_to_c,_tmpBD);_tmpDC;});_tmpDB;});_LL46: if(_tmp9F <= (void*)3
 || *((int*)_tmp9F)!= 10)goto _LL48;_tmpBE=((struct Cyc_Absyn_AggrType_struct*)
_tmp9F)->f1;_tmpBF=(void*)_tmpBE.aggr_info;_tmpC0=_tmpBE.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*
_tmpDD=Cyc_Absyn_get_known_aggrdecl(_tmpBF);if((void*)_tmpDD->kind == (void*)1)
return Cyc_Absyn_unionq_typ(_tmpDD->name);return Cyc_Absyn_strctq(_tmpDD->name);}
_LL48: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 12)goto _LL4A;_tmpC1=((struct Cyc_Absyn_EnumType_struct*)
_tmp9F)->f1;_LL49: return t;_LL4A: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 13)
goto _LL4C;_tmpC2=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp9F)->f1;_LL4B: Cyc_Toc_enumfields_to_c(
_tmpC2);return t;_LL4C: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 16)goto _LL4E;
_tmpC3=((struct Cyc_Absyn_TypedefType_struct*)_tmp9F)->f1;_tmpC4=((struct Cyc_Absyn_TypedefType_struct*)
_tmp9F)->f2;_tmpC5=((struct Cyc_Absyn_TypedefType_struct*)_tmp9F)->f3;_tmpC6=((
struct Cyc_Absyn_TypedefType_struct*)_tmp9F)->f4;_LL4D: if(_tmpC6 == 0)return(void*)({
struct Cyc_Absyn_TypedefType_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=({
struct Cyc_Absyn_TypedefType_struct _tmpDF;_tmpDF.tag=16;_tmpDF.f1=_tmpC3;_tmpDF.f2=
0;_tmpDF.f3=_tmpC5;_tmpDF.f4=0;_tmpDF;});_tmpDE;});else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_TypedefType_struct
_tmpE1;_tmpE1.tag=16;_tmpE1.f1=_tmpC3;_tmpE1.f2=0;_tmpE1.f3=_tmpC5;_tmpE1.f4=({
void**_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2[0]=Cyc_Toc_typ_to_c_array(*_tmpC6);
_tmpE2;});_tmpE1;});_tmpE0;});}_LL4E: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 
14)goto _LL50;_LL4F: goto _LL51;_LL50: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 17)
goto _LL52;_LL51: return Cyc_Absyn_uint_typ;_LL52: if(_tmp9F <= (void*)3  || *((int*)
_tmp9F)!= 15)goto _LL54;_tmpC7=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp9F)->f1;_LL53: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL54: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 18)goto _LL56;
_LL55:({void*_tmpE3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpE4="Toc::typ_to_c: type translation passed a type integer";
_tag_arr(_tmpE4,sizeof(char),_get_zero_arr_size(_tmpE4,54));}),_tag_arr(_tmpE3,
sizeof(void*),0));});_LL56: if((int)_tmp9F != 2)goto _LL58;_LL57:({void*_tmpE5[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmpE6="Toc::typ_to_c: type translation passed the heap region";_tag_arr(_tmpE6,
sizeof(char),_get_zero_arr_size(_tmpE6,55));}),_tag_arr(_tmpE5,sizeof(void*),0));});
_LL58: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 19)goto _LL5A;_LL59: goto _LL5B;
_LL5A: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 20)goto _LL5C;_LL5B: goto _LL5D;
_LL5C: if(_tmp9F <= (void*)3  || *((int*)_tmp9F)!= 21)goto _LL25;_LL5D:({void*_tmpE7[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpE8="Toc::typ_to_c: type translation passed an effect";_tag_arr(
_tmpE8,sizeof(char),_get_zero_arr_size(_tmpE8,49));}),_tag_arr(_tmpE7,sizeof(
void*),0));});_LL25:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmpE9=t;struct Cyc_Absyn_ArrayInfo
_tmpEA;void*_tmpEB;struct Cyc_Absyn_Tqual _tmpEC;_LL71: if(_tmpE9 <= (void*)3  || *((
int*)_tmpE9)!= 7)goto _LL73;_tmpEA=((struct Cyc_Absyn_ArrayType_struct*)_tmpE9)->f1;
_tmpEB=(void*)_tmpEA.elt_type;_tmpEC=_tmpEA.tq;_LL72: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(
_tmpEB,(void*)2,_tmpEC,Cyc_Absyn_false_conref),e);_LL73:;_LL74: return Cyc_Toc_cast_it(
t,e);_LL70:;}static int Cyc_Toc_atomic_typ(void*t){void*_tmpED=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpEE;void*_tmpEF;struct Cyc_Absyn_AggrInfo _tmpF0;
void*_tmpF1;struct Cyc_List_List*_tmpF2;struct Cyc_Absyn_TunionFieldInfo _tmpF3;
void*_tmpF4;struct Cyc_Absyn_Tuniondecl*_tmpF5;struct Cyc_Absyn_Tunionfield*_tmpF6;
struct Cyc_List_List*_tmpF7;_LL76: if((int)_tmpED != 0)goto _LL78;_LL77: return 1;
_LL78: if(_tmpED <= (void*)3  || *((int*)_tmpED)!= 1)goto _LL7A;_LL79: return 0;_LL7A:
if(_tmpED <= (void*)3  || *((int*)_tmpED)!= 5)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if(
_tmpED <= (void*)3  || *((int*)_tmpED)!= 12)goto _LL7E;_LL7D: goto _LL7F;_LL7E: if(
_tmpED <= (void*)3  || *((int*)_tmpED)!= 13)goto _LL80;_LL7F: goto _LL81;_LL80: if((
int)_tmpED != 1)goto _LL82;_LL81: goto _LL83;_LL82: if(_tmpED <= (void*)3  || *((int*)
_tmpED)!= 6)goto _LL84;_LL83: goto _LL85;_LL84: if(_tmpED <= (void*)3  || *((int*)
_tmpED)!= 8)goto _LL86;_LL85: goto _LL87;_LL86: if(_tmpED <= (void*)3  || *((int*)
_tmpED)!= 17)goto _LL88;_LL87: goto _LL89;_LL88: if(_tmpED <= (void*)3  || *((int*)
_tmpED)!= 14)goto _LL8A;_LL89: return 1;_LL8A: if(_tmpED <= (void*)3  || *((int*)
_tmpED)!= 7)goto _LL8C;_tmpEE=((struct Cyc_Absyn_ArrayType_struct*)_tmpED)->f1;
_tmpEF=(void*)_tmpEE.elt_type;_LL8B: return Cyc_Toc_atomic_typ(_tmpEF);_LL8C: if(
_tmpED <= (void*)3  || *((int*)_tmpED)!= 10)goto _LL8E;_tmpF0=((struct Cyc_Absyn_AggrType_struct*)
_tmpED)->f1;_tmpF1=(void*)_tmpF0.aggr_info;_LL8D:{void*_tmpF8=_tmpF1;_LL9D: if(*((
int*)_tmpF8)!= 0)goto _LL9F;_LL9E: return 0;_LL9F:;_LLA0: goto _LL9C;_LL9C:;}{struct
Cyc_Absyn_Aggrdecl*_tmpF9=Cyc_Absyn_get_known_aggrdecl(_tmpF1);if(_tmpF9->impl == 
0)return 0;{struct Cyc_List_List*_tmpFA=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpF9->impl))->fields;for(0;_tmpFA != 0;_tmpFA=_tmpFA->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpFA->hd)->type))return 0;}}return 1;}_LL8E:
if(_tmpED <= (void*)3  || *((int*)_tmpED)!= 11)goto _LL90;_tmpF2=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpED)->f2;_LL8F: for(0;_tmpF2 != 0;_tmpF2=_tmpF2->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpF2->hd)->type))return 0;}return 1;_LL90: if(
_tmpED <= (void*)3  || *((int*)_tmpED)!= 3)goto _LL92;_tmpF3=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpED)->f1;_tmpF4=(void*)_tmpF3.field_info;if(*((int*)_tmpF4)!= 1)goto _LL92;
_tmpF5=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpF4)->f1;_tmpF6=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpF4)->f2;_LL91: _tmpF7=_tmpF6->typs;goto _LL93;_LL92: if(_tmpED <= (void*)3  || *((
int*)_tmpED)!= 9)goto _LL94;_tmpF7=((struct Cyc_Absyn_TupleType_struct*)_tmpED)->f1;
_LL93: for(0;_tmpF7 != 0;_tmpF7=_tmpF7->tl){if(!Cyc_Toc_atomic_typ((*((struct
_tuple4*)_tmpF7->hd)).f2))return 0;}return 1;_LL94: if(_tmpED <= (void*)3  || *((int*)
_tmpED)!= 2)goto _LL96;_LL95: goto _LL97;_LL96: if(_tmpED <= (void*)3  || *((int*)
_tmpED)!= 4)goto _LL98;_LL97: goto _LL99;_LL98: if(_tmpED <= (void*)3  || *((int*)
_tmpED)!= 15)goto _LL9A;_LL99: return 0;_LL9A:;_LL9B:({struct Cyc_String_pa_struct
_tmpFD;_tmpFD.tag=0;_tmpFD.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmpFB[1]={& _tmpFD};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmpFC="atomic_typ:  bad type %s";_tag_arr(_tmpFC,
sizeof(char),_get_zero_arr_size(_tmpFC,25));}),_tag_arr(_tmpFB,sizeof(void*),1));}});
_LL75:;}static int Cyc_Toc_is_void_star(void*t){void*_tmpFE=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpFF;void*_tmp100;_LLA2: if(_tmpFE <= (void*)3  || *((int*)
_tmpFE)!= 4)goto _LLA4;_tmpFF=((struct Cyc_Absyn_PointerType_struct*)_tmpFE)->f1;
_tmp100=(void*)_tmpFF.elt_typ;_LLA3: {void*_tmp101=Cyc_Tcutil_compress(_tmp100);
_LLA7: if((int)_tmp101 != 0)goto _LLA9;_LLA8: return 1;_LLA9:;_LLAA: return 0;_LLA6:;}
_LLA4:;_LLA5: return 0;_LLA1:;}static int Cyc_Toc_is_poly_field(void*t,struct
_tagged_arr*f){void*_tmp102=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp103;void*_tmp104;struct Cyc_List_List*_tmp105;_LLAC: if(_tmp102 <= (void*)3  || *((
int*)_tmp102)!= 10)goto _LLAE;_tmp103=((struct Cyc_Absyn_AggrType_struct*)_tmp102)->f1;
_tmp104=(void*)_tmp103.aggr_info;_LLAD: {struct Cyc_Absyn_Aggrdecl*_tmp106=Cyc_Absyn_get_known_aggrdecl(
_tmp104);if(_tmp106->impl == 0)({void*_tmp107[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp108="is_poly_field: type missing fields";
_tag_arr(_tmp108,sizeof(char),_get_zero_arr_size(_tmp108,35));}),_tag_arr(
_tmp107,sizeof(void*),0));});_tmp105=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp106->impl))->fields;goto _LLAF;}_LLAE: if(_tmp102 <= (void*)3  || *((int*)
_tmp102)!= 11)goto _LLB0;_tmp105=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp102)->f2;
_LLAF: {struct Cyc_Absyn_Aggrfield*_tmp109=Cyc_Absyn_lookup_field(_tmp105,f);if(
_tmp109 == 0)({struct Cyc_String_pa_struct _tmp10C;_tmp10C.tag=0;_tmp10C.f1=(struct
_tagged_arr)((struct _tagged_arr)*f);{void*_tmp10A[1]={& _tmp10C};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp10B="is_poly_field: bad field %s";
_tag_arr(_tmp10B,sizeof(char),_get_zero_arr_size(_tmp10B,28));}),_tag_arr(
_tmp10A,sizeof(void*),1));}});return Cyc_Toc_is_void_star((void*)_tmp109->type);}
_LLB0:;_LLB1:({struct Cyc_String_pa_struct _tmp10F;_tmp10F.tag=0;_tmp10F.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp10D[1]={&
_tmp10F};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp10E="is_poly_field: bad type %s";_tag_arr(_tmp10E,sizeof(char),
_get_zero_arr_size(_tmp10E,27));}),_tag_arr(_tmp10D,sizeof(void*),1));}});_LLAB:;}
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp110=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp111;struct _tagged_arr*_tmp112;struct Cyc_Absyn_Exp*
_tmp113;struct _tagged_arr*_tmp114;_LLB3: if(*((int*)_tmp110)!= 23)goto _LLB5;
_tmp111=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp110)->f1;_tmp112=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp110)->f2;_LLB4: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp111->topt))->v,_tmp112);_LLB5: if(*((int*)_tmp110)!= 24)goto _LLB7;
_tmp113=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp110)->f1;_tmp114=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp110)->f2;_LLB6: {void*_tmp115=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp113->topt))->v);struct Cyc_Absyn_PtrInfo _tmp116;void*_tmp117;
_LLBA: if(_tmp115 <= (void*)3  || *((int*)_tmp115)!= 4)goto _LLBC;_tmp116=((struct
Cyc_Absyn_PointerType_struct*)_tmp115)->f1;_tmp117=(void*)_tmp116.elt_typ;_LLBB:
return Cyc_Toc_is_poly_field(_tmp117,_tmp114);_LLBC:;_LLBD:({struct Cyc_String_pa_struct
_tmp11A;_tmp11A.tag=0;_tmp11A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp113->topt))->v));{void*_tmp118[1]={&
_tmp11A};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp119="is_poly_project:  bad type %s";_tag_arr(_tmp119,sizeof(char),
_get_zero_arr_size(_tmp119,30));}),_tag_arr(_tmp118,sizeof(void*),1));}});_LLB9:;}
_LLB7:;_LLB8: return 0;_LLB2:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct
Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*
_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->hd=s;_tmp11B->tl=0;_tmp11B;}),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmp11C=_cycalloc(sizeof(*
_tmp11C));_tmp11C->hd=s;_tmp11C->tl=0;_tmp11C;}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return
Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*
Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*_tmp11D[2];_tmp11D[1]=s;_tmp11D[0]=
rgn;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp11D,
sizeof(struct Cyc_Absyn_Exp*),2));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(
void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(
elt_type))return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*
_tmp11E[2];_tmp11E[1]=n;_tmp11E[0]=s;((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp11E,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp11F[2];
_tmp11F[1]=n;_tmp11F[0]=s;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp11F,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*
_tmp120[3];_tmp120[2]=n;_tmp120[1]=s;_tmp120[0]=rgn;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp120,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*_tmp121=
_cycalloc(sizeof(*_tmp121));_tmp121->hd=e;_tmp121->tl=0;_tmp121;}),0);}static
struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_tagged_arr(void*t,struct Cyc_Absyn_Exp*
sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*_tmp122=(void*)e->r;void*_tmp123;
_LLBF: if(*((int*)_tmp122)!= 0)goto _LLC1;_tmp123=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp122)->f1;if(_tmp123 <= (void*)1  || *((int*)_tmp123)!= 5)goto _LLC1;_LLC0:
is_string=1;goto _LLBE;_LLC1:;_LLC2: goto _LLBE;_LLBE:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125[0]=({
struct Cyc_Absyn_Var_d_struct _tmp126;_tmp126.tag=0;_tmp126.f1=vd;_tmp126;});
_tmp125;}),0);_tmp124->tl=Cyc_Toc_result_decls;_tmp124;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple6*
_tmp127[3];_tmp127[2]=({struct _tuple6*_tmp12A=_cycalloc(sizeof(*_tmp12A));
_tmp12A->f1=0;_tmp12A->f2=xplussz;_tmp12A;});_tmp127[1]=({struct _tuple6*_tmp129=
_cycalloc(sizeof(*_tmp129));_tmp129->f1=0;_tmp129->f2=xexp;_tmp129;});_tmp127[0]=({
struct _tuple6*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->f1=0;_tmp128->f2=xexp;
_tmp128;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp127,sizeof(struct _tuple6*),3));}),0);return urm_exp;}}}struct Cyc_Toc_Env{
struct Cyc_Core_Opt*break_lab;struct Cyc_Core_Opt*continue_lab;struct Cyc_Core_Opt*
fallthru_info;struct Cyc_Dict_Dict*varmap;int toplevel;};static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(){return({struct Cyc_Toc_Env*_tmp12B=_cycalloc(sizeof(*_tmp12B));
_tmp12B->break_lab=0;_tmp12B->continue_lab=0;_tmp12B->fallthru_info=0;_tmp12B->varmap=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp);_tmp12B->toplevel=1;_tmp12B;});}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp12C=_cycalloc(
sizeof(*_tmp12C));_tmp12C[0]=*e;_tmp12C;});ans->toplevel=0;return ans;}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct Cyc_Toc_Env*e,struct _tuple1*x,struct
Cyc_Absyn_Exp*y){{void*_tmp12D=(*x).f1;_LLC4: if(_tmp12D <= (void*)1  || *((int*)
_tmp12D)!= 0)goto _LLC6;_LLC5:({struct Cyc_String_pa_struct _tmp130;_tmp130.tag=0;
_tmp130.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(x));{
void*_tmp12E[1]={& _tmp130};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp12F="Toc::add_varmap on Rel_n: %s\n";_tag_arr(
_tmp12F,sizeof(char),_get_zero_arr_size(_tmp12F,30));}),_tag_arr(_tmp12E,sizeof(
void*),1));}});_LLC6:;_LLC7: goto _LLC3;_LLC3:;}{struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131[0]=*e;_tmp131;});ans->varmap=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,struct Cyc_Absyn_Exp*
v))Cyc_Dict_insert)(ans->varmap,x,y);return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp132=_cycalloc(
sizeof(*_tmp132));_tmp132[0]=*e;_tmp132;});ans->break_lab=0;ans->continue_lab=0;
return ans;}struct _tuple8{struct _tagged_arr*f1;struct Cyc_List_List*f2;struct Cyc_Dict_Dict*
f3;};static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct Cyc_Toc_Env*
e,struct _tagged_arr*break_l,struct _tagged_arr*fallthru_l,struct Cyc_List_List*
fallthru_binders,struct Cyc_Toc_Env*next_case_env){struct Cyc_List_List*_tmp133=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){_tmp133=({
struct Cyc_List_List*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp134->tl=_tmp133;_tmp134;});}_tmp133=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp133);{struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138[0]=*e;_tmp138;});
ans->break_lab=({struct Cyc_Core_Opt*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->v=
break_l;_tmp135;});ans->fallthru_info=({struct Cyc_Core_Opt*_tmp136=_cycalloc(
sizeof(*_tmp136));_tmp136->v=({struct _tuple8*_tmp137=_cycalloc(sizeof(*_tmp137));
_tmp137->f1=fallthru_l;_tmp137->f2=_tmp133;_tmp137->f3=next_case_env->varmap;
_tmp137;});_tmp136;});return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*break_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A[0]=*e;_tmp13A;});ans->break_lab=({
struct Cyc_Core_Opt*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->v=break_l;
_tmp139;});ans->fallthru_info=0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*next_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D[0]=*e;_tmp13D;});ans->break_lab=0;
ans->fallthru_info=({struct Cyc_Core_Opt*_tmp13B=_cycalloc(sizeof(*_tmp13B));
_tmp13B->v=({struct _tuple8*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->f1=
next_l;_tmp13C->f2=0;_tmp13C->f3=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp13C;});_tmp13B;});
return ans;}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static
int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp13E=(void*)e->annot;
_LLC9: if(*((void**)_tmp13E)!= Cyc_CfFlowInfo_UnknownZ)goto _LLCB;_LLCA: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLCB: if(*((void**)_tmp13E)
!= Cyc_CfFlowInfo_NotZero)goto _LLCD;_LLCC: return 0;_LLCD: if(_tmp13E != Cyc_CfFlowInfo_IsZero)
goto _LLCF;_LLCE:({void*_tmp13F[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp140="dereference of NULL pointer";
_tag_arr(_tmp140,sizeof(char),_get_zero_arr_size(_tmp140,28));}),_tag_arr(
_tmp13F,sizeof(void*),0));});return 0;_LLCF: if(_tmp13E != Cyc_Absyn_EmptyAnnot)
goto _LLD1;_LLD0: return 0;_LLD1: if(*((void**)_tmp13E)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LLD3;_LLD2:({void*_tmp141[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp142="compiler oddity: pointer compared to tag type";
_tag_arr(_tmp142,sizeof(char),_get_zero_arr_size(_tmp142,46));}),_tag_arr(
_tmp141,sizeof(void*),0));});return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLD3:;_LLD4:({void*_tmp143[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp144="need_null_check";
_tag_arr(_tmp144,sizeof(char),_get_zero_arr_size(_tmp144,16));}),_tag_arr(
_tmp143,sizeof(void*),0));});_LLC8:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e){void*_tmp145=(void*)e->annot;struct Cyc_List_List*_tmp146;
struct Cyc_List_List*_tmp147;_LLD6: if(*((void**)_tmp145)!= Cyc_CfFlowInfo_UnknownZ)
goto _LLD8;_tmp146=((struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp145)->f1;_LLD7:
return _tmp146;_LLD8: if(*((void**)_tmp145)!= Cyc_CfFlowInfo_NotZero)goto _LLDA;
_tmp147=((struct Cyc_CfFlowInfo_NotZero_struct*)_tmp145)->f1;_LLD9: return _tmp147;
_LLDA: if(_tmp145 != Cyc_CfFlowInfo_IsZero)goto _LLDC;_LLDB:({void*_tmp148[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp149="dereference of NULL pointer";
_tag_arr(_tmp149,sizeof(char),_get_zero_arr_size(_tmp149,28));}),_tag_arr(
_tmp148,sizeof(void*),0));});return 0;_LLDC: if(*((void**)_tmp145)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LLDE;_LLDD: goto _LLDF;_LLDE: if(_tmp145 != Cyc_Absyn_EmptyAnnot)goto _LLE0;
_LLDF: return 0;_LLE0:;_LLE1:({void*_tmp14A[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp14B="get_relns";_tag_arr(
_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,10));}),_tag_arr(_tmp14A,sizeof(
void*),0));});_LLD5:;}static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp14C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp14D;struct Cyc_Absyn_PtrAtts
_tmp14E;struct Cyc_Absyn_Conref*_tmp14F;struct Cyc_Absyn_Conref*_tmp150;struct Cyc_Absyn_ArrayInfo
_tmp151;struct Cyc_Absyn_Exp*_tmp152;_LLE3: if(_tmp14C <= (void*)3  || *((int*)
_tmp14C)!= 4)goto _LLE5;_tmp14D=((struct Cyc_Absyn_PointerType_struct*)_tmp14C)->f1;
_tmp14E=_tmp14D.ptr_atts;_tmp14F=_tmp14E.bounds;_tmp150=_tmp14E.zero_term;_LLE4: {
void*_tmp153=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp14F);struct Cyc_Absyn_Exp*
_tmp154;_LLEA: if((int)_tmp153 != 0)goto _LLEC;_LLEB: return 0;_LLEC: if(_tmp153 <= (
void*)1  || *((int*)_tmp153)!= 0)goto _LLEE;_tmp154=((struct Cyc_Absyn_Upper_b_struct*)
_tmp153)->f1;_LLED: {unsigned int _tmp156;int _tmp157;struct _tuple5 _tmp155=Cyc_Evexp_eval_const_uint_exp(
_tmp154);_tmp156=_tmp155.f1;_tmp157=_tmp155.f2;return _tmp157  && i <= _tmp156;}
_LLEE: if(_tmp153 <= (void*)1  || *((int*)_tmp153)!= 1)goto _LLE9;_LLEF:({void*
_tmp158[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp159="check_const_array: AbsUpper_b";_tag_arr(_tmp159,sizeof(char),
_get_zero_arr_size(_tmp159,30));}),_tag_arr(_tmp158,sizeof(void*),0));});_LLE9:;}
_LLE5: if(_tmp14C <= (void*)3  || *((int*)_tmp14C)!= 7)goto _LLE7;_tmp151=((struct
Cyc_Absyn_ArrayType_struct*)_tmp14C)->f1;_tmp152=_tmp151.num_elts;_LLE6: if(
_tmp152 == 0)return 0;{unsigned int _tmp15B;int _tmp15C;struct _tuple5 _tmp15A=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp152);_tmp15B=_tmp15A.f1;_tmp15C=_tmp15A.f2;return
_tmp15C  && i <= _tmp15B;}_LLE7:;_LLE8: return 0;_LLE2:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp15D=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp15D->vd != y)continue;{void*_tmp15E=(void*)_tmp15D->rop;struct
Cyc_Absyn_Vardecl*_tmp15F;struct Cyc_Absyn_Vardecl*_tmp160;_LLF1: if(*((int*)
_tmp15E)!= 2)goto _LLF3;_tmp15F=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp15E)->f1;
_LLF2: _tmp160=_tmp15F;goto _LLF4;_LLF3: if(*((int*)_tmp15E)!= 4)goto _LLF5;_tmp160=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp15E)->f1;_LLF4: if(_tmp160 == v)return
1;else{goto _LLF0;}_LLF5:;_LLF6: continue;_LLF0:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*
_tmp161=(void*)e->r;void*_tmp162;struct Cyc_Absyn_Vardecl*_tmp163;void*_tmp164;
struct Cyc_Absyn_Vardecl*_tmp165;void*_tmp166;struct Cyc_Absyn_Vardecl*_tmp167;
void*_tmp168;struct Cyc_Absyn_Vardecl*_tmp169;void*_tmp16A;struct Cyc_List_List*
_tmp16B;struct Cyc_List_List _tmp16C;struct Cyc_Absyn_Exp*_tmp16D;_LLF8: if(*((int*)
_tmp161)!= 1)goto _LLFA;_tmp162=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp161)->f2;
if(_tmp162 <= (void*)1  || *((int*)_tmp162)!= 4)goto _LLFA;_tmp163=((struct Cyc_Absyn_Pat_b_struct*)
_tmp162)->f1;_LLF9: _tmp165=_tmp163;goto _LLFB;_LLFA: if(*((int*)_tmp161)!= 1)goto
_LLFC;_tmp164=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp161)->f2;if(_tmp164 <= (
void*)1  || *((int*)_tmp164)!= 3)goto _LLFC;_tmp165=((struct Cyc_Absyn_Local_b_struct*)
_tmp164)->f1;_LLFB: _tmp167=_tmp165;goto _LLFD;_LLFC: if(*((int*)_tmp161)!= 1)goto
_LLFE;_tmp166=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp161)->f2;if(_tmp166 <= (
void*)1  || *((int*)_tmp166)!= 0)goto _LLFE;_tmp167=((struct Cyc_Absyn_Global_b_struct*)
_tmp166)->f1;_LLFD: _tmp169=_tmp167;goto _LLFF;_LLFE: if(*((int*)_tmp161)!= 1)goto
_LL100;_tmp168=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp161)->f2;if(_tmp168 <= (
void*)1  || *((int*)_tmp168)!= 2)goto _LL100;_tmp169=((struct Cyc_Absyn_Param_b_struct*)
_tmp168)->f1;_LLFF: if(_tmp169->escapes)return 0;if(Cyc_Toc_check_leq_size_var(
relns,v,_tmp169))return 1;goto _LLF7;_LL100: if(*((int*)_tmp161)!= 3)goto _LL102;
_tmp16A=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp161)->f1;if((int)_tmp16A != 
19)goto _LL102;_tmp16B=((struct Cyc_Absyn_Primop_e_struct*)_tmp161)->f2;if(_tmp16B
== 0)goto _LL102;_tmp16C=*_tmp16B;_tmp16D=(struct Cyc_Absyn_Exp*)_tmp16C.hd;_LL101:{
void*_tmp16E=(void*)_tmp16D->r;void*_tmp16F;struct Cyc_Absyn_Vardecl*_tmp170;void*
_tmp171;struct Cyc_Absyn_Vardecl*_tmp172;void*_tmp173;struct Cyc_Absyn_Vardecl*
_tmp174;void*_tmp175;struct Cyc_Absyn_Vardecl*_tmp176;_LL105: if(*((int*)_tmp16E)
!= 1)goto _LL107;_tmp16F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp16E)->f2;if(
_tmp16F <= (void*)1  || *((int*)_tmp16F)!= 4)goto _LL107;_tmp170=((struct Cyc_Absyn_Pat_b_struct*)
_tmp16F)->f1;_LL106: _tmp172=_tmp170;goto _LL108;_LL107: if(*((int*)_tmp16E)!= 1)
goto _LL109;_tmp171=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp16E)->f2;if(
_tmp171 <= (void*)1  || *((int*)_tmp171)!= 3)goto _LL109;_tmp172=((struct Cyc_Absyn_Local_b_struct*)
_tmp171)->f1;_LL108: _tmp174=_tmp172;goto _LL10A;_LL109: if(*((int*)_tmp16E)!= 1)
goto _LL10B;_tmp173=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp16E)->f2;if(
_tmp173 <= (void*)1  || *((int*)_tmp173)!= 0)goto _LL10B;_tmp174=((struct Cyc_Absyn_Global_b_struct*)
_tmp173)->f1;_LL10A: _tmp176=_tmp174;goto _LL10C;_LL10B: if(*((int*)_tmp16E)!= 1)
goto _LL10D;_tmp175=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp16E)->f2;if(
_tmp175 <= (void*)1  || *((int*)_tmp175)!= 2)goto _LL10D;_tmp176=((struct Cyc_Absyn_Param_b_struct*)
_tmp175)->f1;_LL10C: return _tmp176 == v;_LL10D:;_LL10E: goto _LL104;_LL104:;}goto
_LLF7;_LL102:;_LL103: goto _LLF7;_LLF7:;}return 0;}static int Cyc_Toc_check_bounds(
struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*
_tmp177=(void*)a->r;void*_tmp178;struct Cyc_Absyn_Vardecl*_tmp179;void*_tmp17A;
struct Cyc_Absyn_Vardecl*_tmp17B;void*_tmp17C;struct Cyc_Absyn_Vardecl*_tmp17D;
void*_tmp17E;struct Cyc_Absyn_Vardecl*_tmp17F;_LL110: if(*((int*)_tmp177)!= 1)goto
_LL112;_tmp178=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp177)->f2;if(_tmp178 <= (
void*)1  || *((int*)_tmp178)!= 4)goto _LL112;_tmp179=((struct Cyc_Absyn_Pat_b_struct*)
_tmp178)->f1;_LL111: _tmp17B=_tmp179;goto _LL113;_LL112: if(*((int*)_tmp177)!= 1)
goto _LL114;_tmp17A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp177)->f2;if(
_tmp17A <= (void*)1  || *((int*)_tmp17A)!= 3)goto _LL114;_tmp17B=((struct Cyc_Absyn_Local_b_struct*)
_tmp17A)->f1;_LL113: _tmp17D=_tmp17B;goto _LL115;_LL114: if(*((int*)_tmp177)!= 1)
goto _LL116;_tmp17C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp177)->f2;if(
_tmp17C <= (void*)1  || *((int*)_tmp17C)!= 0)goto _LL116;_tmp17D=((struct Cyc_Absyn_Global_b_struct*)
_tmp17C)->f1;_LL115: _tmp17F=_tmp17D;goto _LL117;_LL116: if(*((int*)_tmp177)!= 1)
goto _LL118;_tmp17E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp177)->f2;if(
_tmp17E <= (void*)1  || *((int*)_tmp17E)!= 2)goto _LL118;_tmp17F=((struct Cyc_Absyn_Param_b_struct*)
_tmp17E)->f1;_LL117: if(_tmp17F->escapes)return 0;inner_loop: {void*_tmp180=(void*)
i->r;void*_tmp181;struct Cyc_Absyn_Exp*_tmp182;void*_tmp183;void*_tmp184;int
_tmp185;void*_tmp186;void*_tmp187;int _tmp188;void*_tmp189;void*_tmp18A;int
_tmp18B;void*_tmp18C;struct Cyc_List_List*_tmp18D;struct Cyc_List_List _tmp18E;
struct Cyc_Absyn_Exp*_tmp18F;struct Cyc_List_List*_tmp190;struct Cyc_List_List
_tmp191;struct Cyc_Absyn_Exp*_tmp192;void*_tmp193;struct Cyc_Absyn_Vardecl*_tmp194;
void*_tmp195;struct Cyc_Absyn_Vardecl*_tmp196;void*_tmp197;struct Cyc_Absyn_Vardecl*
_tmp198;void*_tmp199;struct Cyc_Absyn_Vardecl*_tmp19A;_LL11B: if(*((int*)_tmp180)
!= 15)goto _LL11D;_tmp181=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp180)->f1;
_tmp182=((struct Cyc_Absyn_Cast_e_struct*)_tmp180)->f2;_LL11C: i=_tmp182;goto
inner_loop;_LL11D: if(*((int*)_tmp180)!= 0)goto _LL11F;_tmp183=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp180)->f1;if(_tmp183 <= (void*)1  || *((int*)_tmp183)!= 2)goto _LL11F;_tmp184=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp183)->f1;if((int)_tmp184 != 2)goto
_LL11F;_tmp185=((struct Cyc_Absyn_Int_c_struct*)_tmp183)->f2;_LL11E: _tmp188=
_tmp185;goto _LL120;_LL11F: if(*((int*)_tmp180)!= 0)goto _LL121;_tmp186=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp180)->f1;if(_tmp186 <= (void*)1  || *((int*)
_tmp186)!= 2)goto _LL121;_tmp187=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp186)->f1;
if((int)_tmp187 != 0)goto _LL121;_tmp188=((struct Cyc_Absyn_Int_c_struct*)_tmp186)->f2;
_LL120: return _tmp188 >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp188 + 1),(
void*)_tmp17F->type);_LL121: if(*((int*)_tmp180)!= 0)goto _LL123;_tmp189=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp180)->f1;if(_tmp189 <= (void*)1  || *((int*)
_tmp189)!= 2)goto _LL123;_tmp18A=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp189)->f1;
if((int)_tmp18A != 1)goto _LL123;_tmp18B=((struct Cyc_Absyn_Int_c_struct*)_tmp189)->f2;
_LL122: return Cyc_Toc_check_const_array((unsigned int)(_tmp18B + 1),(void*)_tmp17F->type);
_LL123: if(*((int*)_tmp180)!= 3)goto _LL125;_tmp18C=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp180)->f1;if((int)_tmp18C != 4)goto _LL125;_tmp18D=((struct Cyc_Absyn_Primop_e_struct*)
_tmp180)->f2;if(_tmp18D == 0)goto _LL125;_tmp18E=*_tmp18D;_tmp18F=(struct Cyc_Absyn_Exp*)
_tmp18E.hd;_tmp190=_tmp18E.tl;if(_tmp190 == 0)goto _LL125;_tmp191=*_tmp190;_tmp192=(
struct Cyc_Absyn_Exp*)_tmp191.hd;_LL124: return Cyc_Toc_check_leq_size(relns,
_tmp17F,_tmp192);_LL125: if(*((int*)_tmp180)!= 1)goto _LL127;_tmp193=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp180)->f2;if(_tmp193 <= (void*)1  || *((int*)
_tmp193)!= 4)goto _LL127;_tmp194=((struct Cyc_Absyn_Pat_b_struct*)_tmp193)->f1;
_LL126: _tmp196=_tmp194;goto _LL128;_LL127: if(*((int*)_tmp180)!= 1)goto _LL129;
_tmp195=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp180)->f2;if(_tmp195 <= (void*)
1  || *((int*)_tmp195)!= 3)goto _LL129;_tmp196=((struct Cyc_Absyn_Local_b_struct*)
_tmp195)->f1;_LL128: _tmp198=_tmp196;goto _LL12A;_LL129: if(*((int*)_tmp180)!= 1)
goto _LL12B;_tmp197=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp180)->f2;if(
_tmp197 <= (void*)1  || *((int*)_tmp197)!= 0)goto _LL12B;_tmp198=((struct Cyc_Absyn_Global_b_struct*)
_tmp197)->f1;_LL12A: _tmp19A=_tmp198;goto _LL12C;_LL12B: if(*((int*)_tmp180)!= 1)
goto _LL12D;_tmp199=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp180)->f2;if(
_tmp199 <= (void*)1  || *((int*)_tmp199)!= 2)goto _LL12D;_tmp19A=((struct Cyc_Absyn_Param_b_struct*)
_tmp199)->f1;_LL12C: if(_tmp19A->escapes)return 0;{struct Cyc_List_List*_tmp19B=
relns;for(0;_tmp19B != 0;_tmp19B=_tmp19B->tl){struct Cyc_CfFlowInfo_Reln*_tmp19C=(
struct Cyc_CfFlowInfo_Reln*)_tmp19B->hd;if(_tmp19C->vd == _tmp19A){void*_tmp19D=(
void*)_tmp19C->rop;struct Cyc_Absyn_Vardecl*_tmp19E;struct Cyc_Absyn_Vardecl*
_tmp19F;unsigned int _tmp1A0;_LL130: if(*((int*)_tmp19D)!= 2)goto _LL132;_tmp19E=((
struct Cyc_CfFlowInfo_LessSize_struct*)_tmp19D)->f1;_LL131: if(_tmp17F == _tmp19E)
return 1;else{goto _LL12F;}_LL132: if(*((int*)_tmp19D)!= 1)goto _LL134;_tmp19F=((
struct Cyc_CfFlowInfo_LessVar_struct*)_tmp19D)->f1;_LL133:{struct Cyc_List_List*
_tmp1A1=relns;for(0;_tmp1A1 != 0;_tmp1A1=_tmp1A1->tl){struct Cyc_CfFlowInfo_Reln*
_tmp1A2=(struct Cyc_CfFlowInfo_Reln*)_tmp1A1->hd;if(_tmp1A2->vd == _tmp19F){void*
_tmp1A3=(void*)_tmp1A2->rop;struct Cyc_Absyn_Vardecl*_tmp1A4;struct Cyc_Absyn_Vardecl*
_tmp1A5;unsigned int _tmp1A6;struct Cyc_Absyn_Vardecl*_tmp1A7;_LL139: if(*((int*)
_tmp1A3)!= 4)goto _LL13B;_tmp1A4=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp1A3)->f1;_LL13A: _tmp1A5=_tmp1A4;goto _LL13C;_LL13B: if(*((int*)_tmp1A3)!= 2)
goto _LL13D;_tmp1A5=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1A3)->f1;_LL13C:
if(_tmp17F == _tmp1A5)return 1;goto _LL138;_LL13D: if(*((int*)_tmp1A3)!= 0)goto
_LL13F;_tmp1A6=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp1A3)->f1;_LL13E:
return Cyc_Toc_check_const_array(_tmp1A6,(void*)_tmp17F->type);_LL13F: if(*((int*)
_tmp1A3)!= 1)goto _LL141;_tmp1A7=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1A3)->f1;
_LL140: if(Cyc_Toc_check_leq_size_var(relns,_tmp17F,_tmp1A7))return 1;goto _LL138;
_LL141:;_LL142: goto _LL138;_LL138:;}}}goto _LL12F;_LL134: if(*((int*)_tmp19D)!= 3)
goto _LL136;_tmp1A0=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp19D)->f1;_LL135:
return Cyc_Toc_check_const_array(_tmp1A0,(void*)_tmp17F->type);_LL136:;_LL137:
goto _LL12F;_LL12F:;}}}goto _LL11A;_LL12D:;_LL12E: goto _LL11A;_LL11A:;}goto _LL10F;
_LL118:;_LL119: goto _LL10F;_LL10F:;}return 0;}static void*Cyc_Toc_get_c_typ(struct
Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp1A8[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1A9="Missing type in primop ";
_tag_arr(_tmp1A9,sizeof(char),_get_zero_arr_size(_tmp1A9,24));}),_tag_arr(
_tmp1A8,sizeof(void*),0));});return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp1AA[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1AB="Missing type in primop ";
_tag_arr(_tmp1AB,sizeof(char),_get_zero_arr_size(_tmp1AB,24));}),_tag_arr(
_tmp1AA,sizeof(void*),0));});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static struct _tuple4*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct
_tuple4*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC->f1=Cyc_Toc_mt_tq;_tmp1AC->f2=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp1AC;});}
static struct _tuple6*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){Cyc_Toc_exp_to_c(nv,e);return({struct _tuple6*_tmp1AD=_cycalloc(sizeof(*
_tmp1AD));_tmp1AD->f1=0;_tmp1AD->f2=e;_tmp1AD;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp1AE=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp1AE):
Cyc_Toc_malloc_ptr(_tmp1AE));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp1AE);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp1AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp1AF != 0;_tmp1AF=_tmp1AF->tl){struct _tuple6 _tmp1B1;struct Cyc_List_List*
_tmp1B2;struct Cyc_Absyn_Exp*_tmp1B3;struct _tuple6*_tmp1B0=(struct _tuple6*)
_tmp1AF->hd;_tmp1B1=*_tmp1B0;_tmp1B2=_tmp1B1.f1;_tmp1B3=_tmp1B1.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp1B2 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp1B2->tl != 0)({void*_tmp1B4[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp1B5="multiple designators in array";
_tag_arr(_tmp1B5,sizeof(char),_get_zero_arr_size(_tmp1B5,30));}),_tag_arr(
_tmp1B4,sizeof(void*),0));});{void*_tmp1B6=(void*)_tmp1B2->hd;void*_tmp1B7=
_tmp1B6;struct Cyc_Absyn_Exp*_tmp1B8;_LL144: if(*((int*)_tmp1B7)!= 0)goto _LL146;
_tmp1B8=((struct Cyc_Absyn_ArrayElement_struct*)_tmp1B7)->f1;_LL145: Cyc_Toc_exp_to_c(
nv,_tmp1B8);e_index=_tmp1B8;goto _LL143;_LL146: if(*((int*)_tmp1B7)!= 1)goto _LL143;
_LL147:({void*_tmp1B9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_unimp)(({const char*_tmp1BA="field name designators in array";_tag_arr(
_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,32));}),_tag_arr(_tmp1B9,sizeof(
void*),0));});_LL143:;}}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,
e_index,0);void*_tmp1BB=(void*)_tmp1B3->r;struct Cyc_List_List*_tmp1BC;struct Cyc_Absyn_Vardecl*
_tmp1BD;struct Cyc_Absyn_Exp*_tmp1BE;struct Cyc_Absyn_Exp*_tmp1BF;int _tmp1C0;void*
_tmp1C1;struct Cyc_List_List*_tmp1C2;_LL149: if(*((int*)_tmp1BB)!= 28)goto _LL14B;
_tmp1BC=((struct Cyc_Absyn_Array_e_struct*)_tmp1BB)->f1;_LL14A: s=Cyc_Toc_init_array(
nv,lval,_tmp1BC,s);goto _LL148;_LL14B: if(*((int*)_tmp1BB)!= 29)goto _LL14D;_tmp1BD=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1BB)->f1;_tmp1BE=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1BB)->f2;_tmp1BF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1BB)->f3;
_tmp1C0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1BB)->f4;_LL14C: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp1BD,_tmp1BE,_tmp1BF,_tmp1C0,s,0);goto _LL148;_LL14D: if(*((int*)
_tmp1BB)!= 31)goto _LL14F;_tmp1C1=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1BB)->f1;_tmp1C2=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1BB)->f2;_LL14E:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp1C1,_tmp1C2,s);goto _LL148;_LL14F:;_LL150:
Cyc_Toc_exp_to_c(nv,_tmp1B3);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp1B3,0),s,0);goto _LL148;_LL148:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp1C3=vd->name;
void*_tmp1C4=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
nv,_tmp1C3,Cyc_Absyn_varb_exp(_tmp1C3,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD[0]=({struct Cyc_Absyn_Local_b_struct
_tmp1CE;_tmp1CE.tag=3;_tmp1CE.f1=vd;_tmp1CE;});_tmp1CD;}),0));struct _tuple1*max=
Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(
_tmp1C3,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp(_tmp1C3,0),Cyc_Absyn_var_exp(max,0),0);struct Cyc_Absyn_Exp*ec=
Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp1C3,0),0);struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp1C3,0),0);struct Cyc_Absyn_Stmt*
body;{void*_tmp1C5=(void*)e2->r;struct Cyc_List_List*_tmp1C6;struct Cyc_Absyn_Vardecl*
_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1C9;int _tmp1CA;void*
_tmp1CB;struct Cyc_List_List*_tmp1CC;_LL152: if(*((int*)_tmp1C5)!= 28)goto _LL154;
_tmp1C6=((struct Cyc_Absyn_Array_e_struct*)_tmp1C5)->f1;_LL153: body=Cyc_Toc_init_array(
nv2,lval,_tmp1C6,Cyc_Toc_skip_stmt_dl());goto _LL151;_LL154: if(*((int*)_tmp1C5)!= 
29)goto _LL156;_tmp1C7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1C5)->f1;
_tmp1C8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1C5)->f2;_tmp1C9=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1C5)->f3;_tmp1CA=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1C5)->f4;_LL155: body=Cyc_Toc_init_comprehension(nv2,lval,_tmp1C7,_tmp1C8,
_tmp1C9,_tmp1CA,Cyc_Toc_skip_stmt_dl(),0);goto _LL151;_LL156: if(*((int*)_tmp1C5)
!= 31)goto _LL158;_tmp1CB=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1C5)->f1;
_tmp1CC=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1C5)->f2;_LL157: body=Cyc_Toc_init_anon_struct(
nv,lval,_tmp1CB,_tmp1CC,Cyc_Toc_skip_stmt_dl());goto _LL151;_LL158:;_LL159: Cyc_Toc_exp_to_c(
nv2,e2);body=Cyc_Absyn_assign_stmt(lval,e2,0);goto _LL151;_LL151:;}{struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp1C4,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp1C3,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp1CF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp1CF != 0;_tmp1CF=_tmp1CF->tl){struct _tuple6 _tmp1D1;
struct Cyc_List_List*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D3;struct _tuple6*_tmp1D0=(
struct _tuple6*)_tmp1CF->hd;_tmp1D1=*_tmp1D0;_tmp1D2=_tmp1D1.f1;_tmp1D3=_tmp1D1.f2;
if(_tmp1D2 == 0)({void*_tmp1D4[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1D5="empty designator list";
_tag_arr(_tmp1D5,sizeof(char),_get_zero_arr_size(_tmp1D5,22));}),_tag_arr(
_tmp1D4,sizeof(void*),0));});if(_tmp1D2->tl != 0)({void*_tmp1D6[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1D7="too many designators in anonymous struct";
_tag_arr(_tmp1D7,sizeof(char),_get_zero_arr_size(_tmp1D7,41));}),_tag_arr(
_tmp1D6,sizeof(void*),0));});{void*_tmp1D8=(void*)_tmp1D2->hd;struct _tagged_arr*
_tmp1D9;_LL15B: if(*((int*)_tmp1D8)!= 1)goto _LL15D;_tmp1D9=((struct Cyc_Absyn_FieldName_struct*)
_tmp1D8)->f1;_LL15C: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,
_tmp1D9,0);{void*_tmp1DA=(void*)_tmp1D3->r;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Vardecl*
_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DD;struct Cyc_Absyn_Exp*_tmp1DE;int _tmp1DF;void*
_tmp1E0;struct Cyc_List_List*_tmp1E1;_LL160: if(*((int*)_tmp1DA)!= 28)goto _LL162;
_tmp1DB=((struct Cyc_Absyn_Array_e_struct*)_tmp1DA)->f1;_LL161: s=Cyc_Toc_init_array(
nv,lval,_tmp1DB,s);goto _LL15F;_LL162: if(*((int*)_tmp1DA)!= 29)goto _LL164;_tmp1DC=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DA)->f1;_tmp1DD=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1DA)->f2;_tmp1DE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DA)->f3;
_tmp1DF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DA)->f4;_LL163: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp1DC,_tmp1DD,_tmp1DE,_tmp1DF,s,0);goto _LL15F;_LL164: if(*((int*)
_tmp1DA)!= 31)goto _LL166;_tmp1E0=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1DA)->f1;_tmp1E1=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1DA)->f2;_LL165:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp1E0,_tmp1E1,s);goto _LL15F;_LL166:;_LL167:
Cyc_Toc_exp_to_c(nv,_tmp1D3);if(Cyc_Toc_is_poly_field(struct_type,_tmp1D9))
_tmp1D3=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp1D3);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp1D3,0),0),s,0);goto _LL15F;
_LL15F:;}goto _LL15A;}_LL15D:;_LL15E:({void*_tmp1E2[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1E3="array designator in struct";
_tag_arr(_tmp1E3,sizeof(char),_get_zero_arr_size(_tmp1E3,27));}),_tag_arr(
_tmp1E2,sizeof(void*),0));});_LL15A:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(
struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
es){struct _RegionHandle _tmp1E4=_new_region("r");struct _RegionHandle*r=& _tmp1E4;
_push_region(r);{struct Cyc_List_List*_tmp1E5=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);struct _tagged_arr*_tmp1E6=Cyc_Toc_add_tuple_type(
_tmp1E5);struct _tuple1*_tmp1E7=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1E8=
Cyc_Absyn_var_exp(_tmp1E7,0);struct Cyc_Absyn_Stmt*_tmp1E9=Cyc_Absyn_exp_stmt(
_tmp1E8,0);struct Cyc_Absyn_Exp*(*_tmp1EA)(struct Cyc_Absyn_Exp*,struct _tagged_arr*,
struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp1EB=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp1EB);for(0;_tmp1EB != 0;(_tmp1EB=_tmp1EB->tl,
-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp1EB->hd;struct Cyc_Absyn_Exp*
lval=_tmp1EA(_tmp1E8,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic  && Cyc_Toc_atomic_typ((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1EC=(void*)e->r;
struct Cyc_List_List*_tmp1ED;struct Cyc_Absyn_Vardecl*_tmp1EE;struct Cyc_Absyn_Exp*
_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F0;int _tmp1F1;_LL169: if(*((int*)_tmp1EC)!= 28)
goto _LL16B;_tmp1ED=((struct Cyc_Absyn_Array_e_struct*)_tmp1EC)->f1;_LL16A: _tmp1E9=
Cyc_Toc_init_array(nv,lval,_tmp1ED,_tmp1E9);goto _LL168;_LL16B: if(*((int*)_tmp1EC)
!= 29)goto _LL16D;_tmp1EE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1EC)->f1;
_tmp1EF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1EC)->f2;_tmp1F0=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1EC)->f3;_tmp1F1=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1EC)->f4;_LL16C: _tmp1E9=Cyc_Toc_init_comprehension(nv,lval,_tmp1EE,_tmp1EF,
_tmp1F0,_tmp1F1,_tmp1E9,0);goto _LL168;_LL16D:;_LL16E: Cyc_Toc_exp_to_c(nv,e);
_tmp1E9=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp1EA(
_tmp1E8,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp1E9,0);goto _LL168;_LL168:;}}}{
struct Cyc_Absyn_Exp*_tmp1F2=Cyc_Toc_make_struct(nv,_tmp1E7,Cyc_Absyn_strct(
_tmp1E6),_tmp1E9,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp1F2;}};
_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct
Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp1F3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp1F4=Cyc_Absyn_var_exp(_tmp1F3,0);struct Cyc_Absyn_Stmt*
_tmp1F5=Cyc_Absyn_exp_stmt(_tmp1F4,0);struct Cyc_Absyn_Exp*(*_tmp1F6)(struct Cyc_Absyn_Exp*,
struct _tagged_arr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp1F7=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp1F8;void*
_tmp1F9;_LL170: if(_tmp1F7 <= (void*)3  || *((int*)_tmp1F7)!= 10)goto _LL172;_tmp1F8=((
struct Cyc_Absyn_AggrType_struct*)_tmp1F7)->f1;_tmp1F9=(void*)_tmp1F8.aggr_info;
_LL171: ad=Cyc_Absyn_get_known_aggrdecl(_tmp1F9);goto _LL16F;_LL172:;_LL173:({void*
_tmp1FA[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1FB="init_struct: bad struct type";_tag_arr(_tmp1FB,sizeof(char),
_get_zero_arr_size(_tmp1FB,29));}),_tag_arr(_tmp1FA,sizeof(void*),0));});_LL16F:;}{
struct _RegionHandle _tmp1FC=_new_region("r");struct _RegionHandle*r=& _tmp1FC;
_push_region(r);{struct Cyc_List_List*_tmp1FD=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp1FD != 0;
_tmp1FD=_tmp1FD->tl){struct _tuple6 _tmp1FF;struct Cyc_List_List*_tmp200;struct Cyc_Absyn_Exp*
_tmp201;struct _tuple6*_tmp1FE=(struct _tuple6*)_tmp1FD->hd;_tmp1FF=*_tmp1FE;
_tmp200=_tmp1FF.f1;_tmp201=_tmp1FF.f2;is_atomic=is_atomic  && Cyc_Toc_atomic_typ((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp201->topt))->v);if(_tmp200 == 0)({
void*_tmp202[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp203="empty designator list";_tag_arr(_tmp203,sizeof(char),
_get_zero_arr_size(_tmp203,22));}),_tag_arr(_tmp202,sizeof(void*),0));});if(
_tmp200->tl != 0){struct _tuple1*_tmp204=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp205=Cyc_Absyn_var_exp(_tmp204,0);for(0;_tmp200 != 0;_tmp200=_tmp200->tl){void*
_tmp206=(void*)_tmp200->hd;struct _tagged_arr*_tmp207;_LL175: if(*((int*)_tmp206)
!= 1)goto _LL177;_tmp207=((struct Cyc_Absyn_FieldName_struct*)_tmp206)->f1;_LL176:
if(Cyc_Toc_is_poly_field(struct_type,_tmp207))_tmp205=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp205);_tmp1F5=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp1F6(_tmp1F4,_tmp207,0),_tmp205,0),0),_tmp1F5,0);goto _LL174;_LL177:;_LL178:({
void*_tmp208[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp209="array designator in struct";_tag_arr(_tmp209,sizeof(char),
_get_zero_arr_size(_tmp209,27));}),_tag_arr(_tmp208,sizeof(void*),0));});_LL174:;}
Cyc_Toc_exp_to_c(nv,_tmp201);_tmp1F5=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp205,_tmp201,0),0),_tmp1F5,0);}else{void*_tmp20A=(void*)_tmp200->hd;struct
_tagged_arr*_tmp20B;_LL17A: if(*((int*)_tmp20A)!= 1)goto _LL17C;_tmp20B=((struct
Cyc_Absyn_FieldName_struct*)_tmp20A)->f1;_LL17B: {struct Cyc_Absyn_Exp*lval=
_tmp1F6(_tmp1F4,_tmp20B,0);{void*_tmp20C=(void*)_tmp201->r;struct Cyc_List_List*
_tmp20D;struct Cyc_Absyn_Vardecl*_tmp20E;struct Cyc_Absyn_Exp*_tmp20F;struct Cyc_Absyn_Exp*
_tmp210;int _tmp211;void*_tmp212;struct Cyc_List_List*_tmp213;_LL17F: if(*((int*)
_tmp20C)!= 28)goto _LL181;_tmp20D=((struct Cyc_Absyn_Array_e_struct*)_tmp20C)->f1;
_LL180: _tmp1F5=Cyc_Toc_init_array(nv,lval,_tmp20D,_tmp1F5);goto _LL17E;_LL181: if(*((
int*)_tmp20C)!= 29)goto _LL183;_tmp20E=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp20C)->f1;_tmp20F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp20C)->f2;
_tmp210=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp20C)->f3;_tmp211=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp20C)->f4;_LL182: _tmp1F5=Cyc_Toc_init_comprehension(
nv,lval,_tmp20E,_tmp20F,_tmp210,_tmp211,_tmp1F5,0);goto _LL17E;_LL183: if(*((int*)
_tmp20C)!= 31)goto _LL185;_tmp212=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp20C)->f1;_tmp213=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp20C)->f2;_LL184:
_tmp1F5=Cyc_Toc_init_anon_struct(nv,lval,_tmp212,_tmp213,_tmp1F5);goto _LL17E;
_LL185:;_LL186: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp201->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp201);if(Cyc_Toc_is_poly_field(
struct_type,_tmp20B) && !was_ptr_type)_tmp201=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp201);if(has_exists)_tmp201=Cyc_Toc_cast_it((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(Cyc_Absyn_lookup_decl_field(ad,_tmp20B)))->type,_tmp201);_tmp1F5=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp201,0),0),_tmp1F5,0);goto _LL17E;}
_LL17E:;}goto _LL179;}_LL17C:;_LL17D:({void*_tmp214[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp215="array designator in struct";
_tag_arr(_tmp215,sizeof(char),_get_zero_arr_size(_tmp215,27));}),_tag_arr(
_tmp214,sizeof(void*),0));});_LL179:;}}}{struct Cyc_Absyn_Exp*_tmp216=Cyc_Toc_make_struct(
nv,_tmp1F3,Cyc_Absyn_strctq(tdn),_tmp1F5,pointer,rgnopt,is_atomic);_npop_handler(
0);return _tmp216;};_pop_region(r);}}struct _tuple9{struct Cyc_Core_Opt*f1;struct
Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*
el,struct _tuple9*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int
ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmp217=_cycalloc(sizeof(*_tmp217));_tmp217[0]=({struct Cyc_Absyn_Increment_e_struct
_tmp218;_tmp218.tag=5;_tmp218.f1=e1;_tmp218.f2=(void*)incr;_tmp218;});_tmp217;}),
0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp219=(void*)e1->r;struct Cyc_Absyn_Stmt*
_tmp21A;void*_tmp21B;struct Cyc_Absyn_Exp*_tmp21C;struct Cyc_Absyn_Exp*_tmp21D;
struct _tagged_arr*_tmp21E;_LL188: if(*((int*)_tmp219)!= 37)goto _LL18A;_tmp21A=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp219)->f1;_LL189: Cyc_Toc_lvalue_assign_stmt(
_tmp21A,fs,f,f_env);goto _LL187;_LL18A: if(*((int*)_tmp219)!= 15)goto _LL18C;
_tmp21B=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp219)->f1;_tmp21C=((struct Cyc_Absyn_Cast_e_struct*)
_tmp219)->f2;_LL18B: Cyc_Toc_lvalue_assign(_tmp21C,fs,f,f_env);goto _LL187;_LL18C:
if(*((int*)_tmp219)!= 23)goto _LL18E;_tmp21D=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp219)->f1;_tmp21E=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp219)->f2;_LL18D:(
void*)(e1->r=(void*)((void*)_tmp21D->r));Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp21F=_cycalloc(sizeof(*_tmp21F));_tmp21F->hd=_tmp21E;
_tmp21F->tl=fs;_tmp21F;}),f,f_env);goto _LL187;_LL18E:;_LL18F: {struct Cyc_Absyn_Exp*
e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(
e1_copy,(struct _tagged_arr*)fs->hd,e1_copy->loc);}(void*)(e1->r=(void*)((void*)(
f(e1_copy,f_env))->r));goto _LL187;}_LL187:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp220=(void*)s->r;struct Cyc_Absyn_Exp*_tmp221;struct
Cyc_Absyn_Decl*_tmp222;struct Cyc_Absyn_Stmt*_tmp223;struct Cyc_Absyn_Stmt*_tmp224;
_LL191: if(_tmp220 <= (void*)1  || *((int*)_tmp220)!= 0)goto _LL193;_tmp221=((struct
Cyc_Absyn_Exp_s_struct*)_tmp220)->f1;_LL192: Cyc_Toc_lvalue_assign(_tmp221,fs,f,
f_env);goto _LL190;_LL193: if(_tmp220 <= (void*)1  || *((int*)_tmp220)!= 11)goto
_LL195;_tmp222=((struct Cyc_Absyn_Decl_s_struct*)_tmp220)->f1;_tmp223=((struct Cyc_Absyn_Decl_s_struct*)
_tmp220)->f2;_LL194: Cyc_Toc_lvalue_assign_stmt(_tmp223,fs,f,f_env);goto _LL190;
_LL195: if(_tmp220 <= (void*)1  || *((int*)_tmp220)!= 1)goto _LL197;_tmp224=((struct
Cyc_Absyn_Seq_s_struct*)_tmp220)->f2;_LL196: Cyc_Toc_lvalue_assign_stmt(_tmp224,
fs,f,f_env);goto _LL190;_LL197:;_LL198:({struct Cyc_String_pa_struct _tmp227;
_tmp227.tag=0;_tmp227.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_stmt2string(
s));{void*_tmp225[1]={& _tmp227};Cyc_Toc_toc_impos(({const char*_tmp226="lvalue_assign_stmt: %s";
_tag_arr(_tmp226,sizeof(char),_get_zero_arr_size(_tmp226,23));}),_tag_arr(
_tmp225,sizeof(void*),1));}});_LL190:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(
struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({
struct Cyc_List_List*_tmp228=_region_malloc(r2,sizeof(*_tmp228));_tmp228->hd=(
void*)f((void*)x->hd,env);_tmp228->tl=0;_tmp228;});prev=result;for(x=x->tl;x != 0;
x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp229=_region_malloc(r2,sizeof(*_tmp229));_tmp229->hd=(void*)f((void*)x->hd,
env);_tmp229->tl=0;_tmp229;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,
x);}static struct _tuple6*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({struct
_tuple6*_tmp22A=_cycalloc(sizeof(*_tmp22A));_tmp22A->f1=0;_tmp22A->f2=e;_tmp22A;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp22B=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp22C;_LL19A: if(_tmp22B <= (void*)3  || *((int*)
_tmp22B)!= 4)goto _LL19C;_tmp22C=((struct Cyc_Absyn_PointerType_struct*)_tmp22B)->f1;
_LL19B: return _tmp22C;_LL19C:;_LL19D:({void*_tmp22D[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp22E="get_ptr_typ: not a pointer!";
_tag_arr(_tmp22E,sizeof(char),_get_zero_arr_size(_tmp22E,28));}),_tag_arr(
_tmp22D,sizeof(void*),0));});_LL199:;}static int Cyc_Toc_is_zero_ptr_type(void*t,
void**ptr_type,int*is_fat,void**elt_type){void*_tmp22F=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp230;void*_tmp231;struct Cyc_Absyn_PtrAtts _tmp232;
struct Cyc_Absyn_Conref*_tmp233;struct Cyc_Absyn_Conref*_tmp234;struct Cyc_Absyn_ArrayInfo
_tmp235;void*_tmp236;struct Cyc_Absyn_Tqual _tmp237;struct Cyc_Absyn_Exp*_tmp238;
struct Cyc_Absyn_Conref*_tmp239;_LL19F: if(_tmp22F <= (void*)3  || *((int*)_tmp22F)
!= 4)goto _LL1A1;_tmp230=((struct Cyc_Absyn_PointerType_struct*)_tmp22F)->f1;
_tmp231=(void*)_tmp230.elt_typ;_tmp232=_tmp230.ptr_atts;_tmp233=_tmp232.bounds;
_tmp234=_tmp232.zero_term;_LL1A0: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp234)){*ptr_type=t;*elt_type=_tmp231;{void*_tmp23A=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp233);_LL1A6: if((int)_tmp23A != 0)goto _LL1A8;_LL1A7:*is_fat=1;goto _LL1A5;
_LL1A8:;_LL1A9:*is_fat=0;goto _LL1A5;_LL1A5:;}return 1;}else{return 0;}_LL1A1: if(
_tmp22F <= (void*)3  || *((int*)_tmp22F)!= 7)goto _LL1A3;_tmp235=((struct Cyc_Absyn_ArrayType_struct*)
_tmp22F)->f1;_tmp236=(void*)_tmp235.elt_type;_tmp237=_tmp235.tq;_tmp238=_tmp235.num_elts;
_tmp239=_tmp235.zero_term;_LL1A2: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp239)){*elt_type=_tmp236;*is_fat=0;*ptr_type=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B[0]=({struct Cyc_Absyn_PointerType_struct
_tmp23C;_tmp23C.tag=4;_tmp23C.f1=({struct Cyc_Absyn_PtrInfo _tmp23D;_tmp23D.elt_typ=(
void*)_tmp236;_tmp23D.elt_tq=_tmp237;_tmp23D.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp23E;_tmp23E.rgn=(void*)((void*)2);_tmp23E.nullable=Cyc_Absyn_false_conref;
_tmp23E.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp23F=_cycalloc(sizeof(*_tmp23F));_tmp23F[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp240;_tmp240.tag=0;_tmp240.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp238);
_tmp240;});_tmp23F;}));_tmp23E.zero_term=_tmp239;_tmp23E.ptrloc=0;_tmp23E;});
_tmp23D;});_tmp23C;});_tmp23B;});return 1;}else{return 0;}_LL1A3:;_LL1A4: return 0;
_LL19E:;}static int Cyc_Toc_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_fat,void**elt_type){void*_tmp241=(void*)e1->r;struct Cyc_Absyn_Exp*
_tmp242;struct Cyc_Absyn_Exp*_tmp243;struct Cyc_Absyn_Exp*_tmp244;struct Cyc_Absyn_Exp*
_tmp245;struct Cyc_Absyn_Exp*_tmp246;struct Cyc_Absyn_Exp*_tmp247;_LL1AB: if(*((int*)
_tmp241)!= 15)goto _LL1AD;_LL1AC:({struct Cyc_String_pa_struct _tmp24A;_tmp24A.tag=
0;_tmp24A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e1));{
void*_tmp248[1]={& _tmp24A};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp249="we have a cast in a lhs:  %s";_tag_arr(
_tmp249,sizeof(char),_get_zero_arr_size(_tmp249,29));}),_tag_arr(_tmp248,sizeof(
void*),1));}});_LL1AD: if(*((int*)_tmp241)!= 22)goto _LL1AF;_tmp242=((struct Cyc_Absyn_Deref_e_struct*)
_tmp241)->f1;_LL1AE: _tmp243=_tmp242;goto _LL1B0;_LL1AF: if(*((int*)_tmp241)!= 25)
goto _LL1B1;_tmp243=((struct Cyc_Absyn_Subscript_e_struct*)_tmp241)->f1;_LL1B0:
return Cyc_Toc_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp243->topt))->v,
ptr_type,is_fat,elt_type);_LL1B1: if(*((int*)_tmp241)!= 24)goto _LL1B3;_tmp244=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp241)->f1;_LL1B2: _tmp245=_tmp244;goto
_LL1B4;_LL1B3: if(*((int*)_tmp241)!= 23)goto _LL1B5;_tmp245=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp241)->f1;_LL1B4: if(Cyc_Toc_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp245->topt))->v,ptr_type,is_fat,elt_type))({struct Cyc_String_pa_struct
_tmp24D;_tmp24D.tag=0;_tmp24D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(
e1));{void*_tmp24B[1]={& _tmp24D};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Toc_toc_impos)(({const char*_tmp24C="found zero pointer aggregate member assignment: %s";
_tag_arr(_tmp24C,sizeof(char),_get_zero_arr_size(_tmp24C,51));}),_tag_arr(
_tmp24B,sizeof(void*),1));}});return 0;_LL1B5: if(*((int*)_tmp241)!= 14)goto _LL1B7;
_tmp246=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp241)->f1;_LL1B6: _tmp247=
_tmp246;goto _LL1B8;_LL1B7: if(*((int*)_tmp241)!= 13)goto _LL1B9;_tmp247=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp241)->f1;_LL1B8: if(Cyc_Toc_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp247->topt))->v,ptr_type,is_fat,
elt_type))({struct Cyc_String_pa_struct _tmp250;_tmp250.tag=0;_tmp250.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e1));{void*_tmp24E[1]={&
_tmp250};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp24F="found zero pointer instantiate/noinstantiate: %s";_tag_arr(
_tmp24F,sizeof(char),_get_zero_arr_size(_tmp24F,49));}),_tag_arr(_tmp24E,sizeof(
void*),1));}});return 0;_LL1B9: if(*((int*)_tmp241)!= 1)goto _LL1BB;_LL1BA: return 0;
_LL1BB:;_LL1BC:({struct Cyc_String_pa_struct _tmp253;_tmp253.tag=0;_tmp253.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e1));{void*_tmp251[
1]={& _tmp253};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp252="found bad lhs in is_zero_ptr_deref: %s";_tag_arr(_tmp252,
sizeof(char),_get_zero_arr_size(_tmp252,39));}),_tag_arr(_tmp251,sizeof(void*),1));}});
_LL1AA:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp254=Cyc_Tcutil_compress(t);void*_tmp255;void*_tmp256;void*_tmp257;
void*_tmp258;void*_tmp259;void*_tmp25A;void*_tmp25B;void*_tmp25C;_LL1BE: if(
_tmp254 <= (void*)3  || *((int*)_tmp254)!= 4)goto _LL1C0;_LL1BF: res=Cyc_Absyn_null_exp(
0);goto _LL1BD;_LL1C0: if(_tmp254 <= (void*)3  || *((int*)_tmp254)!= 5)goto _LL1C2;
_tmp255=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp254)->f1;_tmp256=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp254)->f2;if((int)_tmp256 != 0)goto _LL1C2;
_LL1C1: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Char_c_struct*_tmp25D=
_cycalloc(sizeof(*_tmp25D));_tmp25D[0]=({struct Cyc_Absyn_Char_c_struct _tmp25E;
_tmp25E.tag=0;_tmp25E.f1=(void*)_tmp255;_tmp25E.f2='\000';_tmp25E;});_tmp25D;}),
0);goto _LL1BD;_LL1C2: if(_tmp254 <= (void*)3  || *((int*)_tmp254)!= 5)goto _LL1C4;
_tmp257=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp254)->f1;_tmp258=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp254)->f2;if((int)_tmp258 != 1)goto _LL1C4;
_LL1C3: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Short_c_struct*_tmp25F=
_cycalloc(sizeof(*_tmp25F));_tmp25F[0]=({struct Cyc_Absyn_Short_c_struct _tmp260;
_tmp260.tag=1;_tmp260.f1=(void*)_tmp257;_tmp260.f2=0;_tmp260;});_tmp25F;}),0);
goto _LL1BD;_LL1C4: if(_tmp254 <= (void*)3  || *((int*)_tmp254)!= 12)goto _LL1C6;
_LL1C5: goto _LL1C7;_LL1C6: if(_tmp254 <= (void*)3  || *((int*)_tmp254)!= 13)goto
_LL1C8;_LL1C7: _tmp259=(void*)1;goto _LL1C9;_LL1C8: if(_tmp254 <= (void*)3  || *((int*)
_tmp254)!= 5)goto _LL1CA;_tmp259=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp254)->f1;
_tmp25A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp254)->f2;if((int)_tmp25A != 
2)goto _LL1CA;_LL1C9: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*
_tmp261=_cycalloc(sizeof(*_tmp261));_tmp261[0]=({struct Cyc_Absyn_Int_c_struct
_tmp262;_tmp262.tag=2;_tmp262.f1=(void*)_tmp259;_tmp262.f2=0;_tmp262;});_tmp261;}),
0);goto _LL1BD;_LL1CA: if(_tmp254 <= (void*)3  || *((int*)_tmp254)!= 5)goto _LL1CC;
_tmp25B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp254)->f1;_tmp25C=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp254)->f2;if((int)_tmp25C != 3)goto _LL1CC;
_LL1CB: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_LongLong_c_struct*
_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263[0]=({struct Cyc_Absyn_LongLong_c_struct
_tmp264;_tmp264.tag=3;_tmp264.f1=(void*)_tmp25B;_tmp264.f2=(long long)0;_tmp264;});
_tmp263;}),0);goto _LL1BD;_LL1CC: if((int)_tmp254 != 1)goto _LL1CE;_LL1CD: goto _LL1CF;
_LL1CE: if(_tmp254 <= (void*)3  || *((int*)_tmp254)!= 6)goto _LL1D0;_LL1CF: res=Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Float_c_struct*_tmp265=_cycalloc(sizeof(*_tmp265));
_tmp265[0]=({struct Cyc_Absyn_Float_c_struct _tmp266;_tmp266.tag=4;_tmp266.f1=({
const char*_tmp267="0.0";_tag_arr(_tmp267,sizeof(char),_get_zero_arr_size(_tmp267,
4));});_tmp266;});_tmp265;}),0);goto _LL1BD;_LL1D0:;_LL1D1:({struct Cyc_String_pa_struct
_tmp26A;_tmp26A.tag=0;_tmp26A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp268[1]={& _tmp26A};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Toc_toc_impos)(({const char*_tmp269="found non-zero type %s in generate_zero";
_tag_arr(_tmp269,sizeof(char),_get_zero_arr_size(_tmp269,40));}),_tag_arr(
_tmp268,sizeof(void*),1));}});_LL1BD:;}res->topt=({struct Cyc_Core_Opt*_tmp26B=
_cycalloc(sizeof(*_tmp26B));_tmp26B->v=(void*)t;_tmp26B;});return res;}static void
Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,
int is_fat,void*elt_type){void*_tmp26C=Cyc_Absyn_tagged_typ(elt_type,(void*)2,Cyc_Toc_mt_tq,
Cyc_Absyn_true_conref);void*_tmp26D=Cyc_Toc_typ_to_c(elt_type);void*_tmp26E=Cyc_Toc_typ_to_c(
_tmp26C);void*_tmp26F=Cyc_Absyn_cstar_typ(_tmp26D,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmp270=({struct Cyc_Core_Opt*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA->v=(
void*)_tmp26F;_tmp2AA;});struct Cyc_Absyn_Exp*xinit;{void*_tmp271=(void*)e1->r;
struct Cyc_Absyn_Exp*_tmp272;struct Cyc_Absyn_Exp*_tmp273;struct Cyc_Absyn_Exp*
_tmp274;_LL1D3: if(*((int*)_tmp271)!= 22)goto _LL1D5;_tmp272=((struct Cyc_Absyn_Deref_e_struct*)
_tmp271)->f1;_LL1D4: if(!is_fat){_tmp272=Cyc_Toc_cast_it(_tmp26C,_tmp272);_tmp272->topt=({
struct Cyc_Core_Opt*_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275->v=(void*)_tmp26C;
_tmp275;});}Cyc_Toc_exp_to_c(nv,_tmp272);xinit=_tmp272;goto _LL1D2;_LL1D5: if(*((
int*)_tmp271)!= 25)goto _LL1D7;_tmp273=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp271)->f1;_tmp274=((struct Cyc_Absyn_Subscript_e_struct*)_tmp271)->f2;_LL1D6:
if(!is_fat){_tmp273=Cyc_Toc_cast_it(_tmp26C,_tmp273);_tmp273->topt=({struct Cyc_Core_Opt*
_tmp276=_cycalloc(sizeof(*_tmp276));_tmp276->v=(void*)_tmp26C;_tmp276;});}Cyc_Toc_exp_to_c(
nv,_tmp273);Cyc_Toc_exp_to_c(nv,_tmp274);xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp277[3];_tmp277[2]=_tmp274;_tmp277[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0);_tmp277[0]=_tmp273;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp277,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);goto _LL1D2;_LL1D7:;_LL1D8:({void*_tmp278[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp279="found bad lhs for zero-terminated pointer assignment";
_tag_arr(_tmp279,sizeof(char),_get_zero_arr_size(_tmp279,53));}),_tag_arr(
_tmp278,sizeof(void*),0));});_LL1D2:;}{struct _tuple1*_tmp27A=Cyc_Toc_temp_var();
nv=Cyc_Toc_add_varmap(nv,_tmp27A,Cyc_Absyn_var_exp(_tmp27A,0));{struct Cyc_Absyn_Vardecl*
_tmp27B=({struct Cyc_Absyn_Vardecl*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));_tmp2A9->sc=(
void*)((void*)2);_tmp2A9->name=_tmp27A;_tmp2A9->tq=Cyc_Toc_mt_tq;_tmp2A9->type=(
void*)_tmp26E;_tmp2A9->initializer=(struct Cyc_Absyn_Exp*)xinit;_tmp2A9->rgn=0;
_tmp2A9->attributes=0;_tmp2A9->escapes=0;_tmp2A9;});struct Cyc_Absyn_Local_b_struct*
_tmp27C=({struct Cyc_Absyn_Local_b_struct*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));
_tmp2A7[0]=({struct Cyc_Absyn_Local_b_struct _tmp2A8;_tmp2A8.tag=3;_tmp2A8.f1=
_tmp27B;_tmp2A8;});_tmp2A7;});struct Cyc_Absyn_Exp*_tmp27D=Cyc_Absyn_varb_exp(
_tmp27A,(void*)_tmp27C,0);_tmp27D->topt=({struct Cyc_Core_Opt*_tmp27E=_cycalloc(
sizeof(*_tmp27E));_tmp27E->v=(void*)_tmp26C;_tmp27E;});{struct Cyc_Absyn_Exp*
_tmp27F=Cyc_Absyn_deref_exp(_tmp27D,0);_tmp27F->topt=({struct Cyc_Core_Opt*
_tmp280=_cycalloc(sizeof(*_tmp280));_tmp280->v=(void*)elt_type;_tmp280;});Cyc_Toc_exp_to_c(
nv,_tmp27F);{struct _tuple1*_tmp281=Cyc_Toc_temp_var();nv=Cyc_Toc_add_varmap(nv,
_tmp281,Cyc_Absyn_var_exp(_tmp281,0));{struct Cyc_Absyn_Vardecl*_tmp282=({struct
Cyc_Absyn_Vardecl*_tmp2A6=_cycalloc(sizeof(*_tmp2A6));_tmp2A6->sc=(void*)((void*)
2);_tmp2A6->name=_tmp281;_tmp2A6->tq=Cyc_Toc_mt_tq;_tmp2A6->type=(void*)_tmp26D;
_tmp2A6->initializer=(struct Cyc_Absyn_Exp*)_tmp27F;_tmp2A6->rgn=0;_tmp2A6->attributes=
0;_tmp2A6->escapes=0;_tmp2A6;});struct Cyc_Absyn_Local_b_struct*_tmp283=({struct
Cyc_Absyn_Local_b_struct*_tmp2A4=_cycalloc(sizeof(*_tmp2A4));_tmp2A4[0]=({struct
Cyc_Absyn_Local_b_struct _tmp2A5;_tmp2A5.tag=3;_tmp2A5.f1=_tmp282;_tmp2A5;});
_tmp2A4;});struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*
_tmp284=Cyc_Absyn_varb_exp(_tmp281,(void*)_tmp283,0);_tmp284->topt=_tmp27F->topt;
z_init=Cyc_Absyn_prim2_exp((void*)popt->v,_tmp284,e2,0);z_init->topt=_tmp284->topt;}
Cyc_Toc_exp_to_c(nv,z_init);{struct _tuple1*_tmp285=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp286=({struct Cyc_Absyn_Vardecl*_tmp2A3=_cycalloc(sizeof(*_tmp2A3));_tmp2A3->sc=(
void*)((void*)2);_tmp2A3->name=_tmp285;_tmp2A3->tq=Cyc_Toc_mt_tq;_tmp2A3->type=(
void*)_tmp26D;_tmp2A3->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp2A3->rgn=0;
_tmp2A3->attributes=0;_tmp2A3->escapes=0;_tmp2A3;});struct Cyc_Absyn_Local_b_struct*
_tmp287=({struct Cyc_Absyn_Local_b_struct*_tmp2A1=_cycalloc(sizeof(*_tmp2A1));
_tmp2A1[0]=({struct Cyc_Absyn_Local_b_struct _tmp2A2;_tmp2A2.tag=3;_tmp2A2.f1=
_tmp286;_tmp2A2;});_tmp2A1;});nv=Cyc_Toc_add_varmap(nv,_tmp285,Cyc_Absyn_var_exp(
_tmp285,0));{struct Cyc_Absyn_Exp*_tmp288=Cyc_Absyn_varb_exp(_tmp281,(void*)
_tmp283,0);_tmp288->topt=_tmp27F->topt;{struct Cyc_Absyn_Exp*_tmp289=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp28A=Cyc_Absyn_prim2_exp((void*)5,_tmp288,
_tmp289,0);_tmp28A->topt=({struct Cyc_Core_Opt*_tmp28B=_cycalloc(sizeof(*_tmp28B));
_tmp28B->v=(void*)Cyc_Absyn_sint_typ;_tmp28B;});Cyc_Toc_exp_to_c(nv,_tmp28A);{
struct Cyc_Absyn_Exp*_tmp28C=Cyc_Absyn_varb_exp(_tmp285,(void*)_tmp287,0);_tmp28C->topt=
_tmp27F->topt;{struct Cyc_Absyn_Exp*_tmp28D=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp28E=Cyc_Absyn_prim2_exp((void*)6,_tmp28C,_tmp28D,0);
_tmp28E->topt=({struct Cyc_Core_Opt*_tmp28F=_cycalloc(sizeof(*_tmp28F));_tmp28F->v=(
void*)Cyc_Absyn_sint_typ;_tmp28F;});Cyc_Toc_exp_to_c(nv,_tmp28E);{struct Cyc_Absyn_Exp*
_tmp290=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp2A0[2];_tmp2A0[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp2A0[0]=Cyc_Absyn_varb_exp(_tmp27A,(void*)_tmp27C,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp2A0,sizeof(struct Cyc_Absyn_Exp*),
2));}),0),Cyc_Absyn_uint_exp(1,0),0);struct Cyc_Absyn_Exp*_tmp291=Cyc_Absyn_and_exp(
_tmp290,Cyc_Absyn_and_exp(_tmp28A,_tmp28E,0),0);struct Cyc_Absyn_Stmt*_tmp292=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*
_tmp293=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_varb_exp(_tmp27A,(void*)_tmp27C,0),
Cyc_Toc_curr_sp,0);_tmp293=Cyc_Toc_cast_it(_tmp26F,_tmp293);{struct Cyc_Absyn_Exp*
_tmp294=Cyc_Absyn_deref_exp(_tmp293,0);struct Cyc_Absyn_Exp*_tmp295=Cyc_Absyn_assign_exp(
_tmp294,Cyc_Absyn_var_exp(_tmp285,0),0);struct Cyc_Absyn_Stmt*_tmp296=Cyc_Absyn_exp_stmt(
_tmp295,0);_tmp296=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp291,_tmp292,
Cyc_Absyn_skip_stmt(0),0),_tmp296,0);_tmp296=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*
_tmp297=_cycalloc(sizeof(*_tmp297));_tmp297->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298[0]=({struct Cyc_Absyn_Var_d_struct
_tmp299;_tmp299.tag=0;_tmp299.f1=_tmp286;_tmp299;});_tmp298;}));_tmp297->loc=0;
_tmp297;}),_tmp296,0);_tmp296=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp29A=
_cycalloc(sizeof(*_tmp29A));_tmp29A->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B[0]=({struct Cyc_Absyn_Var_d_struct
_tmp29C;_tmp29C.tag=0;_tmp29C.f1=_tmp282;_tmp29C;});_tmp29B;}));_tmp29A->loc=0;
_tmp29A;}),_tmp296,0);_tmp296=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp29D=
_cycalloc(sizeof(*_tmp29D));_tmp29D->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp29E=_cycalloc(sizeof(*_tmp29E));_tmp29E[0]=({struct Cyc_Absyn_Var_d_struct
_tmp29F;_tmp29F.tag=0;_tmp29F.f1=_tmp27B;_tmp29F;});_tmp29E;}));_tmp29D->loc=0;
_tmp29D;}),_tmp296,0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp296,0))->r));}}}}}}}}}}}}}
struct _tuple10{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple11{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp2AB=(void*)e->r;if(e->topt
== 0)({struct Cyc_String_pa_struct _tmp2AE;_tmp2AE.tag=0;_tmp2AE.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e));{void*_tmp2AC[1]={&
_tmp2AE};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp2AD="exp_to_c: no type for %s";_tag_arr(_tmp2AD,sizeof(char),
_get_zero_arr_size(_tmp2AD,25));}),_tag_arr(_tmp2AC,sizeof(void*),1));}});{void*
old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp2AF=
_tmp2AB;void*_tmp2B0;struct _tuple1*_tmp2B1;void*_tmp2B2;struct _tuple1*_tmp2B3;
void*_tmp2B4;struct Cyc_List_List*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B6;void*
_tmp2B7;struct Cyc_Absyn_Exp*_tmp2B8;struct Cyc_Core_Opt*_tmp2B9;struct Cyc_Absyn_Exp*
_tmp2BA;struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BC;struct Cyc_Absyn_Exp*
_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BF;struct Cyc_Absyn_Exp*
_tmp2C0;struct Cyc_Absyn_Exp*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_Absyn_Exp*
_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C4;struct Cyc_List_List*_tmp2C5;struct Cyc_Absyn_Exp*
_tmp2C6;struct Cyc_List_List*_tmp2C7;struct Cyc_Absyn_VarargCallInfo*_tmp2C8;
struct Cyc_Absyn_Exp*_tmp2C9;struct Cyc_List_List*_tmp2CA;struct Cyc_Absyn_VarargCallInfo*
_tmp2CB;struct Cyc_Absyn_VarargCallInfo _tmp2CC;int _tmp2CD;struct Cyc_List_List*
_tmp2CE;struct Cyc_Absyn_VarargInfo*_tmp2CF;struct Cyc_Absyn_Exp*_tmp2D0;struct Cyc_Absyn_Exp*
_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D2;struct Cyc_List_List*_tmp2D3;void*_tmp2D4;
void**_tmp2D5;struct Cyc_Absyn_Exp*_tmp2D6;int _tmp2D7;void*_tmp2D8;struct Cyc_Absyn_Exp*
_tmp2D9;struct Cyc_Absyn_Exp*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2DB;struct Cyc_Absyn_Exp*
_tmp2DC;void*_tmp2DD;void*_tmp2DE;void*_tmp2DF;struct _tagged_arr*_tmp2E0;void*
_tmp2E1;void*_tmp2E2;unsigned int _tmp2E3;struct Cyc_Absyn_Exp*_tmp2E4;struct Cyc_Absyn_Exp*
_tmp2E5;struct _tagged_arr*_tmp2E6;struct Cyc_Absyn_Exp*_tmp2E7;struct _tagged_arr*
_tmp2E8;struct Cyc_Absyn_Exp*_tmp2E9;struct Cyc_Absyn_Exp*_tmp2EA;struct Cyc_List_List*
_tmp2EB;struct Cyc_List_List*_tmp2EC;struct Cyc_Absyn_Vardecl*_tmp2ED;struct Cyc_Absyn_Exp*
_tmp2EE;struct Cyc_Absyn_Exp*_tmp2EF;int _tmp2F0;struct _tuple1*_tmp2F1;struct Cyc_List_List*
_tmp2F2;struct Cyc_List_List*_tmp2F3;struct Cyc_Absyn_Aggrdecl*_tmp2F4;void*
_tmp2F5;struct Cyc_List_List*_tmp2F6;struct Cyc_List_List*_tmp2F7;struct Cyc_Absyn_Tuniondecl*
_tmp2F8;struct Cyc_Absyn_Tunionfield*_tmp2F9;struct Cyc_List_List*_tmp2FA;struct
Cyc_Absyn_Tuniondecl*_tmp2FB;struct Cyc_Absyn_Tunionfield*_tmp2FC;struct Cyc_Absyn_MallocInfo
_tmp2FD;int _tmp2FE;struct Cyc_Absyn_Exp*_tmp2FF;void**_tmp300;struct Cyc_Absyn_Exp*
_tmp301;int _tmp302;struct Cyc_Absyn_Stmt*_tmp303;_LL1DA: if(*((int*)_tmp2AF)!= 0)
goto _LL1DC;_tmp2B0=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp2AF)->f1;if((int)
_tmp2B0 != 0)goto _LL1DC;_LL1DB: {struct Cyc_Absyn_Exp*_tmp304=Cyc_Absyn_uint_exp(0,
0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){if(nv->toplevel)(void*)(e->r=(
void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_typ,_tmp304,_tmp304))->r));
else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp305[3];_tmp305[2]=_tmp304;_tmp305[1]=_tmp304;_tmp305[0]=
_tmp304;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp305,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}else{(void*)(e->r=(void*)((
void*)(Cyc_Absyn_signed_int_exp(0,0))->r));}goto _LL1D9;}_LL1DC: if(*((int*)
_tmp2AF)!= 0)goto _LL1DE;_LL1DD: goto _LL1D9;_LL1DE: if(*((int*)_tmp2AF)!= 1)goto
_LL1E0;_tmp2B1=((struct Cyc_Absyn_Var_e_struct*)_tmp2AF)->f1;_tmp2B2=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2AF)->f2;_LL1DF:{struct _handler_cons _tmp306;
_push_handler(& _tmp306);{int _tmp308=0;if(setjmp(_tmp306.handler))_tmp308=1;if(!
_tmp308){(void*)(e->r=(void*)((void*)(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*
d,struct _tuple1*k))Cyc_Dict_lookup)(nv->varmap,_tmp2B1))->r));;_pop_handler();}
else{void*_tmp307=(void*)_exn_thrown;void*_tmp30A=_tmp307;_LL22F: if(_tmp30A != 
Cyc_Dict_Absent)goto _LL231;_LL230:({struct Cyc_String_pa_struct _tmp30D;_tmp30D.tag=
0;_tmp30D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp2B1));{void*_tmp30B[1]={& _tmp30D};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp30C="Can't find %s in exp_to_c, Var\n";
_tag_arr(_tmp30C,sizeof(char),_get_zero_arr_size(_tmp30C,32));}),_tag_arr(
_tmp30B,sizeof(void*),1));}});_LL231:;_LL232:(void)_throw(_tmp30A);_LL22E:;}}}
goto _LL1D9;_LL1E0: if(*((int*)_tmp2AF)!= 2)goto _LL1E2;_tmp2B3=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp2AF)->f1;_LL1E1:({void*_tmp30E[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp30F="unknownid";_tag_arr(
_tmp30F,sizeof(char),_get_zero_arr_size(_tmp30F,10));}),_tag_arr(_tmp30E,sizeof(
void*),0));});_LL1E2: if(*((int*)_tmp2AF)!= 3)goto _LL1E4;_tmp2B4=(void*)((struct
Cyc_Absyn_Primop_e_struct*)_tmp2AF)->f1;_tmp2B5=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2AF)->f2;_LL1E3: {struct Cyc_List_List*_tmp310=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,
_tmp2B5);((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp2B5);{void*
_tmp311=_tmp2B4;_LL234: if((int)_tmp311 != 19)goto _LL236;_LL235: {struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2B5))->hd;{void*
_tmp312=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);
struct Cyc_Absyn_ArrayInfo _tmp313;struct Cyc_Absyn_Exp*_tmp314;struct Cyc_Absyn_PtrInfo
_tmp315;void*_tmp316;struct Cyc_Absyn_PtrAtts _tmp317;struct Cyc_Absyn_Conref*
_tmp318;struct Cyc_Absyn_Conref*_tmp319;struct Cyc_Absyn_Conref*_tmp31A;_LL249: if(
_tmp312 <= (void*)3  || *((int*)_tmp312)!= 7)goto _LL24B;_tmp313=((struct Cyc_Absyn_ArrayType_struct*)
_tmp312)->f1;_tmp314=_tmp313.num_elts;_LL24A:(void*)(e->r=(void*)((void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp314))->r));goto _LL248;_LL24B: if(_tmp312 <= (
void*)3  || *((int*)_tmp312)!= 4)goto _LL24D;_tmp315=((struct Cyc_Absyn_PointerType_struct*)
_tmp312)->f1;_tmp316=(void*)_tmp315.elt_typ;_tmp317=_tmp315.ptr_atts;_tmp318=
_tmp317.nullable;_tmp319=_tmp317.bounds;_tmp31A=_tmp317.zero_term;_LL24C:{void*
_tmp31B=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp319);struct Cyc_Absyn_Exp*
_tmp31C;_LL250: if((int)_tmp31B != 0)goto _LL252;_LL251:(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({struct Cyc_Absyn_Exp*_tmp31D[2];
_tmp31D[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp316),0);_tmp31D[0]=(
struct Cyc_Absyn_Exp*)_tmp2B5->hd;((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp31D,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));
goto _LL24F;_LL252: if(_tmp31B <= (void*)1  || *((int*)_tmp31B)!= 0)goto _LL254;
_tmp31C=((struct Cyc_Absyn_Upper_b_struct*)_tmp31B)->f1;_LL253: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp31A))(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*
_tmp31E[2];_tmp31E[1]=_tmp31C;_tmp31E[0]=(struct Cyc_Absyn_Exp*)_tmp2B5->hd;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp31E,
sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));else{if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp318))(void*)(e->r=(void*)((void*)(Cyc_Absyn_conditional_exp(
arg,_tmp31C,Cyc_Absyn_uint_exp(0,0),0))->r));else{(void*)(e->r=(void*)((void*)
_tmp31C->r));goto _LL24F;}}goto _LL24F;_LL254: if(_tmp31B <= (void*)1  || *((int*)
_tmp31B)!= 1)goto _LL24F;_LL255:({void*_tmp31F[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp320="toc: size of AbsUpper_b";
_tag_arr(_tmp320,sizeof(char),_get_zero_arr_size(_tmp320,24));}),_tag_arr(
_tmp31F,sizeof(void*),0));});_LL24F:;}goto _LL248;_LL24D:;_LL24E:({struct Cyc_String_pa_struct
_tmp324;_tmp324.tag=0;_tmp324.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{struct Cyc_String_pa_struct
_tmp323;_tmp323.tag=0;_tmp323.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{void*_tmp321[2]={&
_tmp323,& _tmp324};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp322="size primop applied to non-array %s (%s)";_tag_arr(_tmp322,
sizeof(char),_get_zero_arr_size(_tmp322,41));}),_tag_arr(_tmp321,sizeof(void*),2));}}});
_LL248:;}goto _LL233;}_LL236: if((int)_tmp311 != 0)goto _LL238;_LL237:{void*_tmp325=
Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp310))->hd);
struct Cyc_Absyn_PtrInfo _tmp326;void*_tmp327;struct Cyc_Absyn_PtrAtts _tmp328;
struct Cyc_Absyn_Conref*_tmp329;struct Cyc_Absyn_Conref*_tmp32A;_LL257: if(_tmp325
<= (void*)3  || *((int*)_tmp325)!= 4)goto _LL259;_tmp326=((struct Cyc_Absyn_PointerType_struct*)
_tmp325)->f1;_tmp327=(void*)_tmp326.elt_typ;_tmp328=_tmp326.ptr_atts;_tmp329=
_tmp328.bounds;_tmp32A=_tmp328.zero_term;_LL258:{void*_tmp32B=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp329);struct Cyc_Absyn_Exp*_tmp32C;_LL25C: if((int)_tmp32B
!= 0)goto _LL25E;_LL25D: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp2B5))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2B5->tl))->hd;(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp32D[3];
_tmp32D[2]=e2;_tmp32D[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp327),0);
_tmp32D[0]=e1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp32D,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));goto _LL25B;}_LL25E:
if(_tmp32B <= (void*)1  || *((int*)_tmp32B)!= 0)goto _LL260;_tmp32C=((struct Cyc_Absyn_Upper_b_struct*)
_tmp32B)->f1;_LL25F: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp32A)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2B5))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp2B5->tl))->hd;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__zero_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp32E[3];_tmp32E[2]=e2;_tmp32E[
1]=_tmp32C;_tmp32E[0]=e1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp32E,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}goto _LL25B;_LL260:
if(_tmp32B <= (void*)1  || *((int*)_tmp32B)!= 1)goto _LL25B;_LL261:({void*_tmp32F[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*
_tmp330="toc: plus on AbsUpper_b";_tag_arr(_tmp330,sizeof(char),
_get_zero_arr_size(_tmp330,24));}),_tag_arr(_tmp32F,sizeof(void*),0));});_LL25B:;}
goto _LL256;_LL259:;_LL25A: goto _LL256;_LL256:;}goto _LL233;_LL238: if((int)_tmp311
!= 2)goto _LL23A;_LL239: {void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)((struct Cyc_List_List*)_check_null(_tmp310))->hd,& elt_typ)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2B5))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp2B5->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)
_check_null(_tmp310->tl))->hd)){(void*)(e1->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e2->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,
0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r));}else{(void*)(e->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*
_tmp331[3];_tmp331[2]=Cyc_Absyn_prim1_exp((void*)2,e2,0);_tmp331[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp331[0]=e1;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp331,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));}}goto _LL233;}_LL23A: if((int)_tmp311 != 5)goto _LL23C;_LL23B: goto _LL23D;
_LL23C: if((int)_tmp311 != 6)goto _LL23E;_LL23D: goto _LL23F;_LL23E: if((int)_tmp311 != 
7)goto _LL240;_LL23F: goto _LL241;_LL240: if((int)_tmp311 != 9)goto _LL242;_LL241: goto
_LL243;_LL242: if((int)_tmp311 != 8)goto _LL244;_LL243: goto _LL245;_LL244: if((int)
_tmp311 != 10)goto _LL246;_LL245: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2B5))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2B5->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp310))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp310->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(t1))(void*)(e1->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,
0))->r));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))(void*)(e2->r=(void*)((void*)(
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));
goto _LL233;}_LL246:;_LL247: goto _LL233;_LL233:;}goto _LL1D9;}_LL1E4: if(*((int*)
_tmp2AF)!= 5)goto _LL1E6;_tmp2B6=((struct Cyc_Absyn_Increment_e_struct*)_tmp2AF)->f1;
_tmp2B7=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp2AF)->f2;_LL1E5: {void*
e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2B6->topt))->v;void*
ptr_type=(void*)0;void*elt_type=(void*)0;int fat_ptr=0;struct _tagged_arr incr_str=({
const char*_tmp33D="increment";_tag_arr(_tmp33D,sizeof(char),_get_zero_arr_size(
_tmp33D,10));});if(_tmp2B7 == (void*)2  || _tmp2B7 == (void*)3)incr_str=({const char*
_tmp332="decrement";_tag_arr(_tmp332,sizeof(char),_get_zero_arr_size(_tmp332,10));});
if(Cyc_Toc_is_zero_ptr_deref(_tmp2B6,& ptr_type,& fat_ptr,& elt_type)){({struct Cyc_String_pa_struct
_tmp335;_tmp335.tag=0;_tmp335.f1=(struct _tagged_arr)((struct _tagged_arr)incr_str);{
void*_tmp333[1]={& _tmp335};Cyc_Tcutil_terr(e->loc,({const char*_tmp334="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_arr(_tmp334,sizeof(char),_get_zero_arr_size(_tmp334,74));}),_tag_arr(
_tmp333,sizeof(void*),1));}});({void*_tmp336[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp337="in-place inc on zero-term";
_tag_arr(_tmp337,sizeof(char),_get_zero_arr_size(_tmp337,26));}),_tag_arr(
_tmp336,sizeof(void*),0));});}Cyc_Toc_exp_to_c(nv,_tmp2B6);{void*elt_typ=(void*)
0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
_tmp338=(_tmp2B7 == (void*)1  || _tmp2B7 == (void*)3)?Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e;int _tmp339=(_tmp2B7 == (void*)0  || _tmp2B7 == (
void*)1)?1: - 1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(_tmp338,({struct
Cyc_Absyn_Exp*_tmp33A[3];_tmp33A[2]=Cyc_Absyn_signed_int_exp(_tmp339,0);_tmp33A[
1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp33A[0]=Cyc_Absyn_address_exp(
_tmp2B6,0);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp33A,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*_tmp33B=_tmp2B7 == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_e:
Cyc_Toc__zero_arr_inplace_plus_e;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
_tmp33B,({struct Cyc_Absyn_Exp*_tmp33C[2];_tmp33C[1]=Cyc_Absyn_signed_int_exp(1,0);
_tmp33C[0]=_tmp2B6;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp33C,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}else{if(elt_typ == (
void*)0  && !Cyc_Absyn_is_lvalue(_tmp2B6)){Cyc_Toc_lvalue_assign(_tmp2B6,0,Cyc_Toc_incr_lvalue,
_tmp2B7);(void*)(e->r=(void*)((void*)_tmp2B6->r));}}}goto _LL1D9;}}_LL1E6: if(*((
int*)_tmp2AF)!= 4)goto _LL1E8;_tmp2B8=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp2AF)->f1;_tmp2B9=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp2AF)->f2;_tmp2BA=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp2AF)->f3;_LL1E7: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_fat=0;if(Cyc_Toc_is_zero_ptr_deref(_tmp2B8,&
ptr_type,& is_fat,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp2B8,_tmp2B9,
_tmp2BA,ptr_type,is_fat,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp2B8);void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2B8->topt))->v;
void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2BA->topt))->v;Cyc_Toc_exp_to_c(
nv,_tmp2B8);Cyc_Toc_exp_to_c(nv,_tmp2BA);{int done=0;if(_tmp2B9 != 0){void*elt_typ=(
void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;{void*_tmp33E=(void*)_tmp2B9->v;_LL263: if((int)_tmp33E != 0)goto _LL265;
_LL264: change=_tmp2BA;goto _LL262;_LL265: if((int)_tmp33E != 2)goto _LL267;_LL266:
change=Cyc_Absyn_prim1_exp((void*)2,_tmp2BA,0);goto _LL262;_LL267:;_LL268:({void*
_tmp33F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp340="bad t ? pointer arithmetic";_tag_arr(_tmp340,sizeof(char),
_get_zero_arr_size(_tmp340,27));}),_tag_arr(_tmp33F,sizeof(void*),0));});_LL262:;}
done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp341[3];_tmp341[2]=change;_tmp341[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp341[0]=Cyc_Absyn_address_exp(_tmp2B8,0);((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp341,
sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp342=(void*)_tmp2B9->v;_LL26A: if((int)_tmp342 != 0)
goto _LL26C;_LL26B: done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp343[2];_tmp343[1]=_tmp2BA;_tmp343[0]=_tmp2B8;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp343,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL269;_LL26C:;_LL26D:({void*_tmp344[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp345="bad zero-terminated pointer arithmetic";
_tag_arr(_tmp345,sizeof(char),_get_zero_arr_size(_tmp345,39));}),_tag_arr(
_tmp344,sizeof(void*),0));});_LL269:;}}}if(!done){if(e1_poly)(void*)(_tmp2BA->r=(
void*)((void*)(Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)
_tmp2BA->r,0)))->r));if(!Cyc_Absyn_is_lvalue(_tmp2B8)){((void(*)(struct Cyc_Absyn_Exp*
e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct
_tuple9*),struct _tuple9*f_env))Cyc_Toc_lvalue_assign)(_tmp2B8,0,Cyc_Toc_assignop_lvalue,({
struct _tuple9*_tmp346=_cycalloc(sizeof(struct _tuple9)* 1);_tmp346[0]=({struct
_tuple9 _tmp347;_tmp347.f1=_tmp2B9;_tmp347.f2=_tmp2BA;_tmp347;});_tmp346;}));(
void*)(e->r=(void*)((void*)_tmp2B8->r));}}goto _LL1D9;}}}_LL1E8: if(*((int*)
_tmp2AF)!= 6)goto _LL1EA;_tmp2BB=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2AF)->f1;
_tmp2BC=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2AF)->f2;_tmp2BD=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp2AF)->f3;_LL1E9: Cyc_Toc_exp_to_c(nv,_tmp2BB);Cyc_Toc_exp_to_c(nv,_tmp2BC);
Cyc_Toc_exp_to_c(nv,_tmp2BD);goto _LL1D9;_LL1EA: if(*((int*)_tmp2AF)!= 7)goto
_LL1EC;_tmp2BE=((struct Cyc_Absyn_And_e_struct*)_tmp2AF)->f1;_tmp2BF=((struct Cyc_Absyn_And_e_struct*)
_tmp2AF)->f2;_LL1EB: Cyc_Toc_exp_to_c(nv,_tmp2BE);Cyc_Toc_exp_to_c(nv,_tmp2BF);
goto _LL1D9;_LL1EC: if(*((int*)_tmp2AF)!= 8)goto _LL1EE;_tmp2C0=((struct Cyc_Absyn_Or_e_struct*)
_tmp2AF)->f1;_tmp2C1=((struct Cyc_Absyn_Or_e_struct*)_tmp2AF)->f2;_LL1ED: Cyc_Toc_exp_to_c(
nv,_tmp2C0);Cyc_Toc_exp_to_c(nv,_tmp2C1);goto _LL1D9;_LL1EE: if(*((int*)_tmp2AF)!= 
9)goto _LL1F0;_tmp2C2=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp2AF)->f1;_tmp2C3=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp2AF)->f2;_LL1EF: Cyc_Toc_exp_to_c(nv,_tmp2C2);
Cyc_Toc_exp_to_c(nv,_tmp2C3);goto _LL1D9;_LL1F0: if(*((int*)_tmp2AF)!= 10)goto
_LL1F2;_tmp2C4=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp2AF)->f1;_tmp2C5=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp2AF)->f2;_LL1F1: _tmp2C6=_tmp2C4;
_tmp2C7=_tmp2C5;goto _LL1F3;_LL1F2: if(*((int*)_tmp2AF)!= 11)goto _LL1F4;_tmp2C6=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp2AF)->f1;_tmp2C7=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp2AF)->f2;_tmp2C8=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2AF)->f3;if(_tmp2C8
!= 0)goto _LL1F4;_LL1F3: Cyc_Toc_exp_to_c(nv,_tmp2C6);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp2C7);goto _LL1D9;_LL1F4: if(*((int*)_tmp2AF)!= 11)goto
_LL1F6;_tmp2C9=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2AF)->f1;_tmp2CA=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp2AF)->f2;_tmp2CB=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp2AF)->f3;if(_tmp2CB == 0)goto _LL1F6;_tmp2CC=*_tmp2CB;_tmp2CD=_tmp2CC.num_varargs;
_tmp2CE=_tmp2CC.injectors;_tmp2CF=_tmp2CC.vai;_LL1F5:{struct _RegionHandle _tmp348=
_new_region("r");struct _RegionHandle*r=& _tmp348;_push_region(r);{struct _tuple1*
argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp2CD,0);
void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp2CF->type);void*arr_type=Cyc_Absyn_array_typ(
cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)num_varargs_exp,Cyc_Absyn_false_conref,
0);int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2CA);int
num_normargs=num_args - _tmp2CD;struct Cyc_List_List*new_args=0;{int i=0;for(0;i < 
num_normargs;(++ i,_tmp2CA=_tmp2CA->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2CA))->hd);new_args=({struct Cyc_List_List*
_tmp349=_cycalloc(sizeof(*_tmp349));_tmp349->hd=(struct Cyc_Absyn_Exp*)_tmp2CA->hd;
_tmp349->tl=new_args;_tmp349;});}}new_args=({struct Cyc_List_List*_tmp34A=
_cycalloc(sizeof(*_tmp34A));_tmp34A->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp34B[3];_tmp34B[2]=num_varargs_exp;_tmp34B[1]=Cyc_Absyn_sizeoftyp_exp(
cva_type,0);_tmp34B[0]=argvexp;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp34B,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp34A->tl=new_args;
_tmp34A;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_args);Cyc_Toc_exp_to_c(nv,_tmp2C9);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(_tmp2C9,new_args,0),0);if(_tmp2CF->inject){struct Cyc_Absyn_Tuniondecl*
tud;{void*_tmp34C=Cyc_Tcutil_compress((void*)_tmp2CF->type);struct Cyc_Absyn_TunionInfo
_tmp34D;void*_tmp34E;struct Cyc_Absyn_Tuniondecl**_tmp34F;struct Cyc_Absyn_Tuniondecl*
_tmp350;_LL26F: if(_tmp34C <= (void*)3  || *((int*)_tmp34C)!= 2)goto _LL271;_tmp34D=((
struct Cyc_Absyn_TunionType_struct*)_tmp34C)->f1;_tmp34E=(void*)_tmp34D.tunion_info;
if(*((int*)_tmp34E)!= 1)goto _LL271;_tmp34F=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp34E)->f1;_tmp350=*_tmp34F;_LL270: tud=_tmp350;goto _LL26E;_LL271:;_LL272:({
void*_tmp351[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp352="toc: unknown tunion in vararg with inject";_tag_arr(_tmp352,
sizeof(char),_get_zero_arr_size(_tmp352,42));}),_tag_arr(_tmp351,sizeof(void*),0));});
_LL26E:;}{struct _tagged_arr vs=({unsigned int _tmp35A=(unsigned int)_tmp2CD;struct
_tuple1**_tmp35B=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct
_tuple1*),_tmp35A));struct _tagged_arr _tmp35D=_tag_arr(_tmp35B,sizeof(struct
_tuple1*),_tmp35A);{unsigned int _tmp35C=_tmp35A;unsigned int i;for(i=0;i < _tmp35C;
i ++){_tmp35B[i]=Cyc_Toc_temp_var();}}_tmp35D;});struct Cyc_List_List*_tmp353=0;{
int i=_tmp2CD - 1;for(0;i >= 0;-- i){_tmp353=({struct Cyc_List_List*_tmp354=_cycalloc(
sizeof(*_tmp354));_tmp354->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((
struct _tuple1**)_check_unknown_subscript(vs,sizeof(struct _tuple1*),i)),0),0));
_tmp354->tl=_tmp353;_tmp354;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct
Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp353,0),s,0);{int i=0;for(0;
_tmp2CA != 0;(((_tmp2CA=_tmp2CA->tl,_tmp2CE=_tmp2CE->tl)),++ i)){struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)_tmp2CA->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_unknown_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp356;struct _tuple1*
_tmp357;struct Cyc_List_List*_tmp358;struct Cyc_Absyn_Tunionfield*_tmp355=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp2CE))->hd;_tmp356=*
_tmp355;_tmp357=_tmp356.name;_tmp358=_tmp356.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp358))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(
1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp357,1),0),s,0);s=Cyc_Absyn_declare_stmt(
var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp357,({const char*_tmp359="_struct";
_tag_arr(_tmp359,sizeof(char),_get_zero_arr_size(_tmp359,8));}))),0,s,0);}}}}}
else{{int i=0;for(0;_tmp2CA != 0;(_tmp2CA=_tmp2CA->tl,++ i)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp2CA->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp2CA->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL1D9;_LL1F6: if(*((
int*)_tmp2AF)!= 12)goto _LL1F8;_tmp2D0=((struct Cyc_Absyn_Throw_e_struct*)_tmp2AF)->f1;
_LL1F7: Cyc_Toc_exp_to_c(nv,_tmp2D0);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp2D0),0))->r));goto _LL1D9;
_LL1F8: if(*((int*)_tmp2AF)!= 13)goto _LL1FA;_tmp2D1=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp2AF)->f1;_LL1F9: Cyc_Toc_exp_to_c(nv,_tmp2D1);goto _LL1D9;_LL1FA: if(*((int*)
_tmp2AF)!= 14)goto _LL1FC;_tmp2D2=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2AF)->f1;
_tmp2D3=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2AF)->f2;_LL1FB: Cyc_Toc_exp_to_c(
nv,_tmp2D2);for(0;_tmp2D3 != 0;_tmp2D3=_tmp2D3->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp2D3->hd);if(k != (void*)4  && k != (void*)3){{void*_tmp35E=Cyc_Tcutil_compress((
void*)_tmp2D3->hd);_LL274: if(_tmp35E <= (void*)3  || *((int*)_tmp35E)!= 1)goto
_LL276;_LL275: goto _LL277;_LL276: if(_tmp35E <= (void*)3  || *((int*)_tmp35E)!= 2)
goto _LL278;_LL277: continue;_LL278:;_LL279:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp2D2,
0))->r));goto _LL273;_LL273:;}break;}}goto _LL1D9;_LL1FC: if(*((int*)_tmp2AF)!= 15)
goto _LL1FE;_tmp2D4=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2AF)->f1;_tmp2D5=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2AF)->f1);_tmp2D6=((struct
Cyc_Absyn_Cast_e_struct*)_tmp2AF)->f2;_tmp2D7=((struct Cyc_Absyn_Cast_e_struct*)
_tmp2AF)->f3;_tmp2D8=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2AF)->f4;_LL1FD: {
void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2D6->topt))->v;void*
new_typ=*_tmp2D5;*_tmp2D5=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp2D6);{
struct _tuple0 _tmp360=({struct _tuple0 _tmp35F;_tmp35F.f1=Cyc_Tcutil_compress(
old_t2);_tmp35F.f2=Cyc_Tcutil_compress(new_typ);_tmp35F;});void*_tmp361;struct
Cyc_Absyn_PtrInfo _tmp362;void*_tmp363;struct Cyc_Absyn_PtrInfo _tmp364;void*
_tmp365;struct Cyc_Absyn_PtrInfo _tmp366;void*_tmp367;_LL27B: _tmp361=_tmp360.f1;
if(_tmp361 <= (void*)3  || *((int*)_tmp361)!= 4)goto _LL27D;_tmp362=((struct Cyc_Absyn_PointerType_struct*)
_tmp361)->f1;_tmp363=_tmp360.f2;if(_tmp363 <= (void*)3  || *((int*)_tmp363)!= 4)
goto _LL27D;_tmp364=((struct Cyc_Absyn_PointerType_struct*)_tmp363)->f1;_LL27C: {
int _tmp368=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(
_tmp362.ptr_atts).nullable);int _tmp369=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp364.ptr_atts).nullable);void*_tmp36A=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp362.ptr_atts).bounds);void*_tmp36B=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp364.ptr_atts).bounds);int _tmp36C=((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp362.ptr_atts).zero_term);int
_tmp36D=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp364.ptr_atts).zero_term);{
struct _tuple0 _tmp36F=({struct _tuple0 _tmp36E;_tmp36E.f1=_tmp36A;_tmp36E.f2=
_tmp36B;_tmp36E;});void*_tmp370;struct Cyc_Absyn_Exp*_tmp371;void*_tmp372;struct
Cyc_Absyn_Exp*_tmp373;void*_tmp374;struct Cyc_Absyn_Exp*_tmp375;void*_tmp376;void*
_tmp377;void*_tmp378;struct Cyc_Absyn_Exp*_tmp379;void*_tmp37A;void*_tmp37B;void*
_tmp37C;void*_tmp37D;void*_tmp37E;void*_tmp37F;void*_tmp380;void*_tmp381;_LL282:
_tmp370=_tmp36F.f1;if(_tmp370 <= (void*)1  || *((int*)_tmp370)!= 0)goto _LL284;
_tmp371=((struct Cyc_Absyn_Upper_b_struct*)_tmp370)->f1;_tmp372=_tmp36F.f2;if(
_tmp372 <= (void*)1  || *((int*)_tmp372)!= 0)goto _LL284;_tmp373=((struct Cyc_Absyn_Upper_b_struct*)
_tmp372)->f1;_LL283: if(_tmp368  && !_tmp369){if(nv->toplevel)({void*_tmp382[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*
_tmp383="can't do NULL-check conversion at top-level";_tag_arr(_tmp383,sizeof(
char),_get_zero_arr_size(_tmp383,44));}),_tag_arr(_tmp382,sizeof(void*),0));});
if(_tmp2D8 != (void*)2)({struct Cyc_String_pa_struct _tmp386;_tmp386.tag=0;_tmp386.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e));{void*_tmp384[1]={&
_tmp386};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp385="null-check conversion mis-classified: %s";_tag_arr(_tmp385,
sizeof(char),_get_zero_arr_size(_tmp385,41));}),_tag_arr(_tmp384,sizeof(void*),1));}});{
int do_null_check=Cyc_Toc_need_null_check(_tmp2D6);if(do_null_check){if(!_tmp2D7)({
void*_tmp387[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp388="inserted null check due to implicit cast from * to @ type";
_tag_arr(_tmp388,sizeof(char),_get_zero_arr_size(_tmp388,58));}),_tag_arr(
_tmp387,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(*
_tmp2D5,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp389=
_cycalloc(sizeof(*_tmp389));_tmp389->hd=_tmp2D6;_tmp389->tl=0;_tmp389;}),0)))->r));}}}
goto _LL281;_LL284: _tmp374=_tmp36F.f1;if(_tmp374 <= (void*)1  || *((int*)_tmp374)!= 
0)goto _LL286;_tmp375=((struct Cyc_Absyn_Upper_b_struct*)_tmp374)->f1;_tmp376=
_tmp36F.f2;if((int)_tmp376 != 0)goto _LL286;_LL285: if(_tmp2D8 == (void*)2)({struct
Cyc_String_pa_struct _tmp38C;_tmp38C.tag=0;_tmp38C.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_exp2string(e));{void*_tmp38A[1]={& _tmp38C};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp38B="conversion mis-classified as null-check: %s";
_tag_arr(_tmp38B,sizeof(char),_get_zero_arr_size(_tmp38B,44));}),_tag_arr(
_tmp38A,sizeof(void*),1));}});if(nv->toplevel){if((_tmp36C  && !(_tmp364.elt_tq).real_const)
 && !_tmp36D)_tmp375=Cyc_Absyn_prim2_exp((void*)2,_tmp375,Cyc_Absyn_uint_exp(1,0),
0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_t2,_tmp375,
_tmp2D6))->r));}else{if(_tmp36C){struct _tuple1*_tmp38D=Cyc_Toc_temp_var();struct
Cyc_Absyn_Exp*_tmp38E=Cyc_Absyn_var_exp(_tmp38D,0);struct Cyc_Absyn_Exp*_tmp38F=
Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*_tmp394[
2];_tmp394[1]=_tmp375;_tmp394[0]=_tmp38E;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp394,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);if(!_tmp36D  && !(_tmp364.elt_tq).real_const)_tmp38F=Cyc_Absyn_prim2_exp((void*)
2,_tmp38F,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp390=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp364.elt_typ),0);struct Cyc_Absyn_Exp*_tmp391=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp393[3];_tmp393[2]=_tmp38F;_tmp393[1]=
_tmp390;_tmp393[0]=_tmp38E;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp393,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp392=Cyc_Absyn_exp_stmt(_tmp391,0);_tmp392=Cyc_Absyn_declare_stmt(_tmp38D,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp2D6,_tmp392,0);(void*)(e->r=(void*)((void*)(
Cyc_Absyn_stmt_exp(_tmp392,0))->r));}}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp395[3];_tmp395[2]=_tmp375;_tmp395[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)_tmp364.elt_typ),0);_tmp395[0]=
_tmp2D6;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp395,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}goto _LL281;_LL286: _tmp377=
_tmp36F.f1;if((int)_tmp377 != 0)goto _LL288;_tmp378=_tmp36F.f2;if(_tmp378 <= (void*)
1  || *((int*)_tmp378)!= 0)goto _LL288;_tmp379=((struct Cyc_Absyn_Upper_b_struct*)
_tmp378)->f1;_LL287: if(nv->toplevel)({void*_tmp396[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp397="can't coerce t? to t* or t@ at the top-level";
_tag_arr(_tmp397,sizeof(char),_get_zero_arr_size(_tmp397,45));}),_tag_arr(
_tmp396,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp398=_tmp379;if(_tmp36C  && 
!_tmp36D)_tmp398=Cyc_Absyn_add_exp(_tmp379,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp399=Cyc_Absyn_fncall_exp(Cyc_Toc__untag_arr_e,({struct Cyc_Absyn_Exp*_tmp39B[
3];_tmp39B[2]=_tmp398;_tmp39B[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)
_tmp362.elt_typ),0);_tmp39B[0]=_tmp2D6;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp39B,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);if(_tmp369)(void*)(_tmp399->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->hd=Cyc_Absyn_copy_exp(
_tmp399);_tmp39A->tl=0;_tmp39A;}),0))->r));(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(*
_tmp2D5,_tmp399))->r));goto _LL281;}}_LL288: _tmp37A=_tmp36F.f1;if((int)_tmp37A != 
0)goto _LL28A;_tmp37B=_tmp36F.f2;if((int)_tmp37B != 0)goto _LL28A;_LL289: if((
_tmp36C  && !_tmp36D) && !(_tmp364.elt_tq).real_const){if(nv->toplevel)({void*
_tmp39C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp39D="can't coerce a ?ZEROTERM to a non-const ?NOZEROTERM at toplevel";
_tag_arr(_tmp39D,sizeof(char),_get_zero_arr_size(_tmp39D,64));}),_tag_arr(
_tmp39C,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_ptr_decrease_size_e,({struct Cyc_Absyn_Exp*_tmp39E[3];_tmp39E[2]=
Cyc_Absyn_uint_exp(1,0);_tmp39E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp362.elt_typ),0);_tmp39E[0]=_tmp2D6;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp39E,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));}goto _LL281;_LL28A: _tmp37C=_tmp36F.f1;if(_tmp37C <= (void*)1  || *((int*)
_tmp37C)!= 1)goto _LL28C;_tmp37D=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp37C)->f1;_tmp37E=_tmp36F.f2;if(_tmp37E <= (void*)1  || *((int*)_tmp37E)!= 1)
goto _LL28C;_tmp37F=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp37E)->f1;
_LL28B: if(_tmp368  && !_tmp369){if(nv->toplevel)({void*_tmp39F[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp3A0="can't do NULL-check conversion at top-level";
_tag_arr(_tmp3A0,sizeof(char),_get_zero_arr_size(_tmp3A0,44));}),_tag_arr(
_tmp39F,sizeof(void*),0));});if(_tmp2D8 != (void*)2)({struct Cyc_String_pa_struct
_tmp3A3;_tmp3A3.tag=0;_tmp3A3.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(
e));{void*_tmp3A1[1]={& _tmp3A3};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(({const char*_tmp3A2="null-check conversion mis-classified: %s";
_tag_arr(_tmp3A2,sizeof(char),_get_zero_arr_size(_tmp3A2,41));}),_tag_arr(
_tmp3A1,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(_tmp2D6);
if(do_null_check){if(!_tmp2D7)({void*_tmp3A4[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp3A5="inserted null check due to implicit cast from * to @ type";
_tag_arr(_tmp3A5,sizeof(char),_get_zero_arr_size(_tmp3A5,58));}),_tag_arr(
_tmp3A4,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(*
_tmp2D5,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp3A6=
_cycalloc(sizeof(*_tmp3A6));_tmp3A6->hd=_tmp2D6;_tmp3A6->tl=0;_tmp3A6;}),0)))->r));}}}
goto _LL281;_LL28C: _tmp380=_tmp36F.f1;if(_tmp380 <= (void*)1  || *((int*)_tmp380)!= 
1)goto _LL28E;_LL28D:({struct Cyc_String_pa_struct _tmp3A9;_tmp3A9.tag=0;_tmp3A9.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Position_string_of_segment(_tmp2D6->loc));{
void*_tmp3A7[1]={& _tmp3A9};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_unimp)(({const char*_tmp3A8="%s: toc, cast from AbsUpper_b";_tag_arr(
_tmp3A8,sizeof(char),_get_zero_arr_size(_tmp3A8,30));}),_tag_arr(_tmp3A7,sizeof(
void*),1));}});_LL28E: _tmp381=_tmp36F.f2;if(_tmp381 <= (void*)1  || *((int*)
_tmp381)!= 1)goto _LL281;_LL28F: goto _LL281;_LL281:;}goto _LL27A;}_LL27D: _tmp365=
_tmp360.f1;if(_tmp365 <= (void*)3  || *((int*)_tmp365)!= 4)goto _LL27F;_tmp366=((
struct Cyc_Absyn_PointerType_struct*)_tmp365)->f1;_tmp367=_tmp360.f2;if(_tmp367 <= (
void*)3  || *((int*)_tmp367)!= 5)goto _LL27F;_LL27E:{void*_tmp3AA=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp366.ptr_atts).bounds);_LL291: if((int)_tmp3AA != 0)goto
_LL293;_LL292:(void*)(_tmp2D6->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((
void*)_tmp2D6->r,_tmp2D6->loc),Cyc_Toc_curr_sp,0))->r));goto _LL290;_LL293:;
_LL294: goto _LL290;_LL290:;}goto _LL27A;_LL27F:;_LL280: goto _LL27A;_LL27A:;}goto
_LL1D9;}_LL1FE: if(*((int*)_tmp2AF)!= 16)goto _LL200;_tmp2D9=((struct Cyc_Absyn_Address_e_struct*)
_tmp2AF)->f1;_LL1FF:{void*_tmp3AB=(void*)_tmp2D9->r;struct _tuple1*_tmp3AC;struct
Cyc_List_List*_tmp3AD;struct Cyc_List_List*_tmp3AE;struct Cyc_List_List*_tmp3AF;
_LL296: if(*((int*)_tmp3AB)!= 30)goto _LL298;_tmp3AC=((struct Cyc_Absyn_Struct_e_struct*)
_tmp3AB)->f1;_tmp3AD=((struct Cyc_Absyn_Struct_e_struct*)_tmp3AB)->f2;_tmp3AE=((
struct Cyc_Absyn_Struct_e_struct*)_tmp3AB)->f3;_LL297: if(nv->toplevel)({struct Cyc_String_pa_struct
_tmp3B2;_tmp3B2.tag=0;_tmp3B2.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp2D9->loc));{void*_tmp3B0[1]={& _tmp3B2};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp3B1="%s: & on non-identifiers at the top-level";
_tag_arr(_tmp3B1,sizeof(char),_get_zero_arr_size(_tmp3B1,42));}),_tag_arr(
_tmp3B0,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2D9->topt))->v,_tmp3AD != 0,1,0,
_tmp3AE,_tmp3AC))->r));goto _LL295;_LL298: if(*((int*)_tmp3AB)!= 26)goto _LL29A;
_tmp3AF=((struct Cyc_Absyn_Tuple_e_struct*)_tmp3AB)->f1;_LL299: if(nv->toplevel)({
struct Cyc_String_pa_struct _tmp3B5;_tmp3B5.tag=0;_tmp3B5.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Position_string_of_segment(_tmp2D9->loc));{void*_tmp3B3[1]={&
_tmp3B5};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp3B4="%s: & on non-identifiers at the top-level";_tag_arr(_tmp3B4,
sizeof(char),_get_zero_arr_size(_tmp3B4,42));}),_tag_arr(_tmp3B3,sizeof(void*),1));}});(
void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,0,_tmp3AF))->r));goto _LL295;
_LL29A:;_LL29B: Cyc_Toc_exp_to_c(nv,_tmp2D9);if(!Cyc_Absyn_is_lvalue(_tmp2D9)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp2D9,0,Cyc_Toc_address_lvalue,
1);(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp2D9))->r));}goto _LL295;_LL295:;}goto
_LL1D9;_LL200: if(*((int*)_tmp2AF)!= 17)goto _LL202;_tmp2DA=((struct Cyc_Absyn_New_e_struct*)
_tmp2AF)->f1;_tmp2DB=((struct Cyc_Absyn_New_e_struct*)_tmp2AF)->f2;_LL201: if(nv->toplevel)({
struct Cyc_String_pa_struct _tmp3B8;_tmp3B8.tag=0;_tmp3B8.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Position_string_of_segment(_tmp2DB->loc));{void*_tmp3B6[1]={&
_tmp3B8};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp3B7="%s: new at top-level";_tag_arr(_tmp3B7,sizeof(char),
_get_zero_arr_size(_tmp3B7,21));}),_tag_arr(_tmp3B6,sizeof(void*),1));}});{void*
_tmp3B9=(void*)_tmp2DB->r;struct Cyc_List_List*_tmp3BA;struct Cyc_Absyn_Vardecl*
_tmp3BB;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BD;int _tmp3BE;
struct _tuple1*_tmp3BF;struct Cyc_List_List*_tmp3C0;struct Cyc_List_List*_tmp3C1;
struct Cyc_List_List*_tmp3C2;_LL29D: if(*((int*)_tmp3B9)!= 28)goto _LL29F;_tmp3BA=((
struct Cyc_Absyn_Array_e_struct*)_tmp3B9)->f1;_LL29E: {struct _tuple1*_tmp3C3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3C4=Cyc_Absyn_var_exp(_tmp3C3,0);struct Cyc_Absyn_Stmt*
_tmp3C5=Cyc_Toc_init_array(nv,_tmp3C4,_tmp3BA,Cyc_Absyn_exp_stmt(_tmp3C4,0));
void*old_elt_typ;{void*_tmp3C6=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp3C7;void*_tmp3C8;struct Cyc_Absyn_Tqual _tmp3C9;struct Cyc_Absyn_PtrAtts _tmp3CA;
struct Cyc_Absyn_Conref*_tmp3CB;_LL2A8: if(_tmp3C6 <= (void*)3  || *((int*)_tmp3C6)
!= 4)goto _LL2AA;_tmp3C7=((struct Cyc_Absyn_PointerType_struct*)_tmp3C6)->f1;
_tmp3C8=(void*)_tmp3C7.elt_typ;_tmp3C9=_tmp3C7.elt_tq;_tmp3CA=_tmp3C7.ptr_atts;
_tmp3CB=_tmp3CA.zero_term;_LL2A9: old_elt_typ=_tmp3C8;goto _LL2A7;_LL2AA:;_LL2AB:
old_elt_typ=({void*_tmp3CC[0]={};Cyc_Toc_toc_impos(({const char*_tmp3CD="exp_to_c:new array expression doesn't have ptr type";
_tag_arr(_tmp3CD,sizeof(char),_get_zero_arr_size(_tmp3CD,52));}),_tag_arr(
_tmp3CC,sizeof(void*),0));});_LL2A7:;}{void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp3CE=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp3CF=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),Cyc_Absyn_signed_int_exp(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3BA),0),0);struct Cyc_Absyn_Exp*
e1;if(_tmp2DA == 0)e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp3CF);else{struct Cyc_Absyn_Exp*
r=(struct Cyc_Absyn_Exp*)_tmp2DA;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,
_tmp3CF);}(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
_tmp3C3,_tmp3CE,(struct Cyc_Absyn_Exp*)e1,_tmp3C5,0),0))->r));goto _LL29C;}}_LL29F:
if(*((int*)_tmp3B9)!= 29)goto _LL2A1;_tmp3BB=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3B9)->f1;_tmp3BC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3B9)->f2;
_tmp3BD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3B9)->f3;_tmp3BE=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp3B9)->f4;_LL2A0: {int is_tagged_ptr=0;{void*
_tmp3D0=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp3D1;void*_tmp3D2;
struct Cyc_Absyn_Tqual _tmp3D3;struct Cyc_Absyn_PtrAtts _tmp3D4;struct Cyc_Absyn_Conref*
_tmp3D5;struct Cyc_Absyn_Conref*_tmp3D6;_LL2AD: if(_tmp3D0 <= (void*)3  || *((int*)
_tmp3D0)!= 4)goto _LL2AF;_tmp3D1=((struct Cyc_Absyn_PointerType_struct*)_tmp3D0)->f1;
_tmp3D2=(void*)_tmp3D1.elt_typ;_tmp3D3=_tmp3D1.elt_tq;_tmp3D4=_tmp3D1.ptr_atts;
_tmp3D5=_tmp3D4.bounds;_tmp3D6=_tmp3D4.zero_term;_LL2AE: is_tagged_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp3D5)== (void*)0;goto _LL2AC;_LL2AF:;_LL2B0:({void*
_tmp3D7[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3D8="exp_to_c: comprehension not an array type";_tag_arr(_tmp3D8,
sizeof(char),_get_zero_arr_size(_tmp3D8,42));}),_tag_arr(_tmp3D7,sizeof(void*),0));});
_LL2AC:;}{struct _tuple1*max=Cyc_Toc_temp_var();struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3BD->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp3BC);{struct Cyc_Absyn_Exp*_tmp3D9=
Cyc_Absyn_var_exp(max,0);if(_tmp3BE)_tmp3D9=Cyc_Absyn_add_exp(_tmp3D9,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp3BB,Cyc_Absyn_var_exp(max,0),_tmp3BD,_tmp3BE,Cyc_Toc_skip_stmt_dl(),1);{
struct _RegionHandle _tmp3DA=_new_region("r");struct _RegionHandle*r=& _tmp3DA;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp3EA=
_region_malloc(r,sizeof(*_tmp3EA));_tmp3EA->hd=({struct _tuple10*_tmp3EB=
_region_malloc(r,sizeof(*_tmp3EB));_tmp3EB->f1=max;_tmp3EB->f2=Cyc_Absyn_uint_typ;
_tmp3EB->f3=(struct Cyc_Absyn_Exp*)_tmp3BC;_tmp3EB;});_tmp3EA->tl=0;_tmp3EA;});
struct Cyc_Absyn_Exp*ai;if(_tmp2DA == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp3DB[2];_tmp3DB[1]=_tmp3D9;
_tmp3DB[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp3DB,sizeof(struct Cyc_Absyn_Exp*),2));}),
0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp2DA;Cyc_Toc_exp_to_c(nv,
r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct
Cyc_Absyn_Exp*_tmp3DC[2];_tmp3DC[1]=_tmp3D9;_tmp3DC[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp3DC,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(
ptr_typ,ai);decls=({struct Cyc_List_List*_tmp3DD=_region_malloc(r,sizeof(*_tmp3DD));
_tmp3DD->hd=({struct _tuple10*_tmp3DE=_region_malloc(r,sizeof(*_tmp3DE));_tmp3DE->f1=
a;_tmp3DE->f2=ptr_typ;_tmp3DE->f3=(struct Cyc_Absyn_Exp*)ainit;_tmp3DE;});_tmp3DD->tl=
decls;_tmp3DD;});if(is_tagged_ptr){struct _tuple1*_tmp3DF=Cyc_Toc_temp_var();void*
_tmp3E0=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp3E1=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp3E4[3];_tmp3E4[2]=_tmp3D9;_tmp3E4[1]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp3E4[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp3E4,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);decls=({struct Cyc_List_List*_tmp3E2=_region_malloc(r,sizeof(*_tmp3E2));
_tmp3E2->hd=({struct _tuple10*_tmp3E3=_region_malloc(r,sizeof(*_tmp3E3));_tmp3E3->f1=
_tmp3DF;_tmp3E3->f2=_tmp3E0;_tmp3E3->f3=(struct Cyc_Absyn_Exp*)_tmp3E1;_tmp3E3;});
_tmp3E2->tl=decls;_tmp3E2;});s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
_tmp3DF,0),0),0);}else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
a,0),0),0);}{struct Cyc_List_List*_tmp3E5=decls;for(0;_tmp3E5 != 0;_tmp3E5=_tmp3E5->tl){
struct _tuple1*_tmp3E7;void*_tmp3E8;struct Cyc_Absyn_Exp*_tmp3E9;struct _tuple10
_tmp3E6=*((struct _tuple10*)_tmp3E5->hd);_tmp3E7=_tmp3E6.f1;_tmp3E8=_tmp3E6.f2;
_tmp3E9=_tmp3E6.f3;s=Cyc_Absyn_declare_stmt(_tmp3E7,_tmp3E8,_tmp3E9,s,0);}}(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL29C;}}}}
_LL2A1: if(*((int*)_tmp3B9)!= 30)goto _LL2A3;_tmp3BF=((struct Cyc_Absyn_Struct_e_struct*)
_tmp3B9)->f1;_tmp3C0=((struct Cyc_Absyn_Struct_e_struct*)_tmp3B9)->f2;_tmp3C1=((
struct Cyc_Absyn_Struct_e_struct*)_tmp3B9)->f3;_LL2A2:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2DB->topt))->v,
_tmp3C0 != 0,1,_tmp2DA,_tmp3C1,_tmp3BF))->r));goto _LL29C;_LL2A3: if(*((int*)
_tmp3B9)!= 26)goto _LL2A5;_tmp3C2=((struct Cyc_Absyn_Tuple_e_struct*)_tmp3B9)->f1;
_LL2A4:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,_tmp2DA,_tmp3C2))->r));
goto _LL29C;_LL2A5:;_LL2A6: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2DB->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct
_tuple1*_tmp3EC=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3ED=Cyc_Absyn_var_exp(
_tmp3EC,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(
_tmp3ED,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp2DA == 0)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp2DA;Cyc_Toc_exp_to_c(
nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp3EE=(void*)_tmp2DB->r;
void*_tmp3EF;struct Cyc_Absyn_Exp*_tmp3F0;_LL2B2: if(*((int*)_tmp3EE)!= 15)goto
_LL2B4;_tmp3EF=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp3EE)->f1;_tmp3F0=((
struct Cyc_Absyn_Cast_e_struct*)_tmp3EE)->f2;_LL2B3:{struct _tuple0 _tmp3F2=({
struct _tuple0 _tmp3F1;_tmp3F1.f1=Cyc_Tcutil_compress(_tmp3EF);_tmp3F1.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F0->topt))->v);_tmp3F1;});void*
_tmp3F3;struct Cyc_Absyn_PtrInfo _tmp3F4;void*_tmp3F5;struct Cyc_Absyn_PtrAtts
_tmp3F6;struct Cyc_Absyn_Conref*_tmp3F7;void*_tmp3F8;struct Cyc_Absyn_PtrInfo
_tmp3F9;struct Cyc_Absyn_PtrAtts _tmp3FA;struct Cyc_Absyn_Conref*_tmp3FB;_LL2B7:
_tmp3F3=_tmp3F2.f1;if(_tmp3F3 <= (void*)3  || *((int*)_tmp3F3)!= 4)goto _LL2B9;
_tmp3F4=((struct Cyc_Absyn_PointerType_struct*)_tmp3F3)->f1;_tmp3F5=(void*)
_tmp3F4.elt_typ;_tmp3F6=_tmp3F4.ptr_atts;_tmp3F7=_tmp3F6.bounds;_tmp3F8=_tmp3F2.f2;
if(_tmp3F8 <= (void*)3  || *((int*)_tmp3F8)!= 4)goto _LL2B9;_tmp3F9=((struct Cyc_Absyn_PointerType_struct*)
_tmp3F8)->f1;_tmp3FA=_tmp3F9.ptr_atts;_tmp3FB=_tmp3FA.bounds;_LL2B8:{struct
_tuple0 _tmp3FD=({struct _tuple0 _tmp3FC;_tmp3FC.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp3F7);_tmp3FC.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp3FB);_tmp3FC;});
void*_tmp3FE;void*_tmp3FF;struct Cyc_Absyn_Exp*_tmp400;_LL2BC: _tmp3FE=_tmp3FD.f1;
if((int)_tmp3FE != 0)goto _LL2BE;_tmp3FF=_tmp3FD.f2;if(_tmp3FF <= (void*)1  || *((
int*)_tmp3FF)!= 0)goto _LL2BE;_tmp400=((struct Cyc_Absyn_Upper_b_struct*)_tmp3FF)->f1;
_LL2BD: Cyc_Toc_exp_to_c(nv,_tmp3F0);(void*)(inner_mexp->r=(void*)((void*)(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0))->r));done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__init_tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp401[4];_tmp401[3]=_tmp400;_tmp401[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp3F5),0);_tmp401[1]=_tmp3F0;_tmp401[0]=mexp;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp401,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))->r));goto _LL2BB;_LL2BE:;_LL2BF: goto _LL2BB;_LL2BB:;}goto _LL2B6;_LL2B9:;
_LL2BA: goto _LL2B6;_LL2B6:;}goto _LL2B1;_LL2B4:;_LL2B5: goto _LL2B1;_LL2B1:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp402=Cyc_Absyn_exp_stmt(_tmp3ED,0);struct Cyc_Absyn_Exp*
_tmp403=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp2DB);_tmp402=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp3ED,_tmp403,0),_tmp2DB,0),
_tmp402,0);{void*_tmp404=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp3EC,_tmp404,(struct
Cyc_Absyn_Exp*)mexp,_tmp402,0),0))->r));}}goto _LL29C;}}_LL29C:;}goto _LL1D9;
_LL202: if(*((int*)_tmp2AF)!= 19)goto _LL204;_tmp2DC=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp2AF)->f1;_LL203: Cyc_Toc_exp_to_c(nv,_tmp2DC);goto _LL1D9;_LL204: if(*((int*)
_tmp2AF)!= 18)goto _LL206;_tmp2DD=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp2AF)->f1;_LL205:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp406;_tmp406.tag=18;_tmp406.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2DD);_tmp406;});
_tmp405;})));goto _LL1D9;_LL206: if(*((int*)_tmp2AF)!= 21)goto _LL208;_LL207:({void*
_tmp407[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp408="__gen() in code generator";_tag_arr(_tmp408,sizeof(char),
_get_zero_arr_size(_tmp408,26));}),_tag_arr(_tmp407,sizeof(void*),0));});_LL208:
if(*((int*)_tmp2AF)!= 20)goto _LL20A;_tmp2DE=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp2AF)->f1;_tmp2DF=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp2AF)->f2;
if(*((int*)_tmp2DF)!= 0)goto _LL20A;_tmp2E0=((struct Cyc_Absyn_StructField_struct*)
_tmp2DF)->f1;_LL209:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp409=_cycalloc(sizeof(*_tmp409));_tmp409[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp40A;_tmp40A.tag=20;_tmp40A.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2DE);_tmp40A.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp40B=_cycalloc(sizeof(*
_tmp40B));_tmp40B[0]=({struct Cyc_Absyn_StructField_struct _tmp40C;_tmp40C.tag=0;
_tmp40C.f1=_tmp2E0;_tmp40C;});_tmp40B;}));_tmp40A;});_tmp409;})));goto _LL1D9;
_LL20A: if(*((int*)_tmp2AF)!= 20)goto _LL20C;_tmp2E1=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp2AF)->f1;_tmp2E2=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp2AF)->f2;
if(*((int*)_tmp2E2)!= 1)goto _LL20C;_tmp2E3=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp2E2)->f1;_LL20B:{void*_tmp40D=Cyc_Tcutil_compress(_tmp2E1);struct Cyc_Absyn_AggrInfo
_tmp40E;void*_tmp40F;struct Cyc_List_List*_tmp410;_LL2C1: if(_tmp40D <= (void*)3
 || *((int*)_tmp40D)!= 10)goto _LL2C3;_tmp40E=((struct Cyc_Absyn_AggrType_struct*)
_tmp40D)->f1;_tmp40F=(void*)_tmp40E.aggr_info;_LL2C2: {struct Cyc_Absyn_Aggrdecl*
_tmp411=Cyc_Absyn_get_known_aggrdecl(_tmp40F);if(_tmp411->impl == 0)({void*
_tmp412[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp413="struct fields must be known";_tag_arr(_tmp413,sizeof(char),
_get_zero_arr_size(_tmp413,28));}),_tag_arr(_tmp412,sizeof(void*),0));});_tmp410=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp411->impl))->fields;goto _LL2C4;}
_LL2C3: if(_tmp40D <= (void*)3  || *((int*)_tmp40D)!= 11)goto _LL2C5;_tmp410=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp40D)->f2;_LL2C4: {struct Cyc_Absyn_Aggrfield*
_tmp414=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmp410,(int)_tmp2E3);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp416;_tmp416.tag=20;_tmp416.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2E1);_tmp416.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp417=_cycalloc(sizeof(*
_tmp417));_tmp417[0]=({struct Cyc_Absyn_StructField_struct _tmp418;_tmp418.tag=0;
_tmp418.f1=_tmp414->name;_tmp418;});_tmp417;}));_tmp416;});_tmp415;})));goto
_LL2C0;}_LL2C5: if(_tmp40D <= (void*)3  || *((int*)_tmp40D)!= 9)goto _LL2C7;_LL2C6:(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp419=_cycalloc(
sizeof(*_tmp419));_tmp419[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp41A;_tmp41A.tag=
20;_tmp41A.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2E1);_tmp41A.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B[0]=({
struct Cyc_Absyn_StructField_struct _tmp41C;_tmp41C.tag=0;_tmp41C.f1=Cyc_Absyn_fieldname((
int)(_tmp2E3 + 1));_tmp41C;});_tmp41B;}));_tmp41A;});_tmp419;})));goto _LL2C0;
_LL2C7: if(_tmp40D <= (void*)3  || *((int*)_tmp40D)!= 3)goto _LL2C9;_LL2C8: if(
_tmp2E3 == 0)(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp41E;_tmp41E.tag=20;_tmp41E.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2E1);_tmp41E.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp41F=_cycalloc(sizeof(*
_tmp41F));_tmp41F[0]=({struct Cyc_Absyn_StructField_struct _tmp420;_tmp420.tag=0;
_tmp420.f1=Cyc_Toc_tag_sp;_tmp420;});_tmp41F;}));_tmp41E;});_tmp41D;})));else{(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp421=_cycalloc(
sizeof(*_tmp421));_tmp421[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp422;_tmp422.tag=
20;_tmp422.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2E1);_tmp422.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423[0]=({
struct Cyc_Absyn_StructField_struct _tmp424;_tmp424.tag=0;_tmp424.f1=Cyc_Absyn_fieldname((
int)_tmp2E3);_tmp424;});_tmp423;}));_tmp422;});_tmp421;})));}goto _LL2C0;_LL2C9:;
_LL2CA:({void*_tmp425[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp426="impossible type for offsetof tuple index";
_tag_arr(_tmp426,sizeof(char),_get_zero_arr_size(_tmp426,41));}),_tag_arr(
_tmp425,sizeof(void*),0));});_LL2C0:;}goto _LL1D9;_LL20C: if(*((int*)_tmp2AF)!= 22)
goto _LL20E;_tmp2E4=((struct Cyc_Absyn_Deref_e_struct*)_tmp2AF)->f1;_LL20D: {void*
_tmp427=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2E4->topt))->v);{
void*_tmp428=_tmp427;struct Cyc_Absyn_PtrInfo _tmp429;void*_tmp42A;struct Cyc_Absyn_Tqual
_tmp42B;struct Cyc_Absyn_PtrAtts _tmp42C;void*_tmp42D;struct Cyc_Absyn_Conref*
_tmp42E;struct Cyc_Absyn_Conref*_tmp42F;struct Cyc_Absyn_Conref*_tmp430;_LL2CC: if(
_tmp428 <= (void*)3  || *((int*)_tmp428)!= 4)goto _LL2CE;_tmp429=((struct Cyc_Absyn_PointerType_struct*)
_tmp428)->f1;_tmp42A=(void*)_tmp429.elt_typ;_tmp42B=_tmp429.elt_tq;_tmp42C=
_tmp429.ptr_atts;_tmp42D=(void*)_tmp42C.rgn;_tmp42E=_tmp42C.nullable;_tmp42F=
_tmp42C.bounds;_tmp430=_tmp42C.zero_term;_LL2CD:{void*_tmp431=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp42F);_LL2D1: if(_tmp431 <= (void*)1  || *((int*)_tmp431)!= 
0)goto _LL2D3;_LL2D2: {int do_null_check=Cyc_Toc_need_null_check(_tmp2E4);Cyc_Toc_exp_to_c(
nv,_tmp2E4);if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp432[0]={};
Cyc_Tcutil_warn(e->loc,({const char*_tmp433="inserted null check due to dereference";
_tag_arr(_tmp433,sizeof(char),_get_zero_arr_size(_tmp433,39));}),_tag_arr(
_tmp432,sizeof(void*),0));});(void*)(_tmp2E4->r=(void*)((void*)(Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(_tmp427),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->hd=Cyc_Absyn_copy_exp(_tmp2E4);
_tmp434->tl=0;_tmp434;}),0)))->r));}goto _LL2D0;}_LL2D3: if((int)_tmp431 != 0)goto
_LL2D5;_LL2D4: {struct Cyc_Absyn_Exp*_tmp435=Cyc_Absyn_uint_exp(0,0);_tmp435->topt=({
struct Cyc_Core_Opt*_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436->v=(void*)Cyc_Absyn_uint_typ;
_tmp436;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(_tmp2E4,_tmp435,
0))->r));Cyc_Toc_exp_to_c(nv,e);goto _LL2D0;}_LL2D5: if(_tmp431 <= (void*)1  || *((
int*)_tmp431)!= 1)goto _LL2D0;_LL2D6:({void*_tmp437[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp438="exp_to_c: deref w/ AbsUpper_b";
_tag_arr(_tmp438,sizeof(char),_get_zero_arr_size(_tmp438,30));}),_tag_arr(
_tmp437,sizeof(void*),0));});_LL2D0:;}goto _LL2CB;_LL2CE:;_LL2CF:({void*_tmp439[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmp43A="exp_to_c: Deref: non-pointer";_tag_arr(_tmp43A,sizeof(char),
_get_zero_arr_size(_tmp43A,29));}),_tag_arr(_tmp439,sizeof(void*),0));});_LL2CB:;}
goto _LL1D9;}_LL20E: if(*((int*)_tmp2AF)!= 23)goto _LL210;_tmp2E5=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2AF)->f1;_tmp2E6=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp2AF)->f2;_LL20F:
Cyc_Toc_exp_to_c(nv,_tmp2E5);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1D9;
_LL210: if(*((int*)_tmp2AF)!= 24)goto _LL212;_tmp2E7=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp2AF)->f1;_tmp2E8=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2AF)->f2;_LL211: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2E7->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp2E7);Cyc_Toc_exp_to_c(nv,_tmp2E7);{
int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp43C;struct Cyc_Absyn_Tqual _tmp43D;
struct Cyc_Absyn_PtrAtts _tmp43E;void*_tmp43F;struct Cyc_Absyn_Conref*_tmp440;
struct Cyc_Absyn_Conref*_tmp441;struct Cyc_Absyn_Conref*_tmp442;struct Cyc_Absyn_PtrInfo
_tmp43B=Cyc_Toc_get_ptr_type(e1typ);_tmp43C=(void*)_tmp43B.elt_typ;_tmp43D=
_tmp43B.elt_tq;_tmp43E=_tmp43B.ptr_atts;_tmp43F=(void*)_tmp43E.rgn;_tmp440=
_tmp43E.nullable;_tmp441=_tmp43E.bounds;_tmp442=_tmp43E.zero_term;{void*_tmp443=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp441);struct Cyc_Absyn_Exp*_tmp444;
_LL2D8: if(_tmp443 <= (void*)1  || *((int*)_tmp443)!= 0)goto _LL2DA;_tmp444=((struct
Cyc_Absyn_Upper_b_struct*)_tmp443)->f1;_LL2D9: {unsigned int _tmp446;int _tmp447;
struct _tuple5 _tmp445=Cyc_Evexp_eval_const_uint_exp(_tmp444);_tmp446=_tmp445.f1;
_tmp447=_tmp445.f2;if(_tmp447){if(_tmp446 < 1)({void*_tmp448[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp449="exp_to_c:  AggrArrow_e on pointer of size 0";
_tag_arr(_tmp449,sizeof(char),_get_zero_arr_size(_tmp449,44));}),_tag_arr(
_tmp448,sizeof(void*),0));});if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({
void*_tmp44A[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp44B="inserted null check due to dereference";
_tag_arr(_tmp44B,sizeof(char),_get_zero_arr_size(_tmp44B,39));}),_tag_arr(
_tmp44A,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2E7->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp44C=_cycalloc(sizeof(*_tmp44C));_tmp44C->hd=_tmp2E7;_tmp44C->tl=0;_tmp44C;}),
0)),_tmp2E8,0))->r));}}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2E7->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp44D[4];_tmp44D[3]=Cyc_Absyn_uint_exp(0,0);_tmp44D[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp43C,0);_tmp44D[1]=_tmp444;_tmp44D[0]=_tmp2E7;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp44D,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)),_tmp2E8,0))->r));}goto _LL2D7;}_LL2DA: if((int)_tmp443 != 0)goto _LL2DC;
_LL2DB: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp43C);(void*)(_tmp2E7->r=(void*)((
void*)(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp43D),Cyc_Absyn_fncall_exp(Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_Absyn_Exp*_tmp44E[3];_tmp44E[2]=Cyc_Absyn_uint_exp(0,0);_tmp44E[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp44E[0]=Cyc_Absyn_copy_exp(_tmp2E7);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp44E,sizeof(struct Cyc_Absyn_Exp*),3));}),
0)))->r));goto _LL2D7;}_LL2DC: if(_tmp443 <= (void*)1  || *((int*)_tmp443)!= 1)goto
_LL2D7;_LL2DD:({void*_tmp44F[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp450="exp_to_c: AggrArrow w/ AbsUpper_b";
_tag_arr(_tmp450,sizeof(char),_get_zero_arr_size(_tmp450,34));}),_tag_arr(
_tmp44F,sizeof(void*),0));});_LL2D7:;}if(is_poly)(void*)(e->r=(void*)((void*)(
Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1D9;}}
_LL212: if(*((int*)_tmp2AF)!= 25)goto _LL214;_tmp2E9=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp2AF)->f1;_tmp2EA=((struct Cyc_Absyn_Subscript_e_struct*)_tmp2AF)->f2;_LL213: {
void*_tmp451=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2E9->topt))->v);{void*_tmp452=_tmp451;struct Cyc_List_List*_tmp453;struct Cyc_Absyn_PtrInfo
_tmp454;void*_tmp455;struct Cyc_Absyn_Tqual _tmp456;struct Cyc_Absyn_PtrAtts _tmp457;
void*_tmp458;struct Cyc_Absyn_Conref*_tmp459;struct Cyc_Absyn_Conref*_tmp45A;
struct Cyc_Absyn_Conref*_tmp45B;_LL2DF: if(_tmp452 <= (void*)3  || *((int*)_tmp452)
!= 9)goto _LL2E1;_tmp453=((struct Cyc_Absyn_TupleType_struct*)_tmp452)->f1;_LL2E0:
Cyc_Toc_exp_to_c(nv,_tmp2E9);Cyc_Toc_exp_to_c(nv,_tmp2EA);{unsigned int _tmp45D;
int _tmp45E;struct _tuple5 _tmp45C=Cyc_Evexp_eval_const_uint_exp(_tmp2EA);_tmp45D=
_tmp45C.f1;_tmp45E=_tmp45C.f2;if(!_tmp45E)({void*_tmp45F[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp460="unknown tuple subscript in translation to C";
_tag_arr(_tmp460,sizeof(char),_get_zero_arr_size(_tmp460,44));}),_tag_arr(
_tmp45F,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
_tmp2E9,Cyc_Absyn_fieldname((int)(_tmp45D + 1)),0))->r));goto _LL2DE;}_LL2E1: if(
_tmp452 <= (void*)3  || *((int*)_tmp452)!= 4)goto _LL2E3;_tmp454=((struct Cyc_Absyn_PointerType_struct*)
_tmp452)->f1;_tmp455=(void*)_tmp454.elt_typ;_tmp456=_tmp454.elt_tq;_tmp457=
_tmp454.ptr_atts;_tmp458=(void*)_tmp457.rgn;_tmp459=_tmp457.nullable;_tmp45A=
_tmp457.bounds;_tmp45B=_tmp457.zero_term;_LL2E2: {struct Cyc_List_List*_tmp461=
Cyc_Toc_get_relns(_tmp2E9);int in_bnds=0;{void*_tmp462=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp45A);_LL2E6: if(_tmp462 <= (void*)1  || *((int*)_tmp462)!= 1)goto _LL2E8;_LL2E7:
goto _LL2E5;_LL2E8:;_LL2E9: in_bnds=Cyc_Toc_check_bounds(_tmp461,_tmp2E9,_tmp2EA);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)({struct Cyc_String_pa_struct _tmp465;
_tmp465.tag=0;_tmp465.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(
e));{void*_tmp463[1]={& _tmp465};Cyc_Tcutil_warn(e->loc,({const char*_tmp464="bounds check necessary for %s";
_tag_arr(_tmp464,sizeof(char),_get_zero_arr_size(_tmp464,30));}),_tag_arr(
_tmp463,sizeof(void*),1));}});_LL2E5:;}Cyc_Toc_exp_to_c(nv,_tmp2E9);Cyc_Toc_exp_to_c(
nv,_tmp2EA);Cyc_Toc_total_bounds_checks ++;{void*_tmp466=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp45A);struct Cyc_Absyn_Exp*_tmp467;void*_tmp468;_LL2EB: if(_tmp466 <= (void*)1
 || *((int*)_tmp466)!= 0)goto _LL2ED;_tmp467=((struct Cyc_Absyn_Upper_b_struct*)
_tmp466)->f1;_LL2EC: {int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp459);void*ta1=Cyc_Toc_typ_to_c(_tmp455);void*ta2=Cyc_Absyn_cstar_typ(
ta1,_tmp456);if(in_bnds)Cyc_Toc_bounds_checks_eliminated ++;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp45B))(void*)(e->r=(void*)((
void*)(Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp469[3];_tmp469[2]=_tmp2EA;_tmp469[1]=_tmp467;_tmp469[0]=
_tmp2E9;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp469,sizeof(struct Cyc_Absyn_Exp*),3));}),0)),0))->r));else{if(possibly_null){
if(Cyc_Toc_warn_all_null_deref)({void*_tmp46A[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp46B="inserted null check due to dereference";_tag_arr(_tmp46B,
sizeof(char),_get_zero_arr_size(_tmp46B,39));}),_tag_arr(_tmp46A,sizeof(void*),0));});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp46C[4];_tmp46C[3]=
_tmp2EA;_tmp46C[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp46C[1]=_tmp467;_tmp46C[0]=
_tmp2E9;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp46C,sizeof(struct Cyc_Absyn_Exp*),4));}),0)),0))->r));}else{(void*)(_tmp2EA->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp46D[2];_tmp46D[1]=Cyc_Absyn_copy_exp(_tmp2EA);_tmp46D[0]=
_tmp467;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp46D,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}}}goto _LL2EA;}_LL2ED: if((
int)_tmp466 != 0)goto _LL2EF;_LL2EE: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp455);if(
in_bnds){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp456),Cyc_Absyn_aggrmember_exp(
_tmp2E9,Cyc_Toc_curr_sp,0)),_tmp2EA,0))->r));}else{(void*)(e->r=(void*)((void*)(
Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp456),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*_tmp46E[3];_tmp46E[2]=
_tmp2EA;_tmp46E[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp46E[0]=_tmp2E9;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp46E,sizeof(struct Cyc_Absyn_Exp*),
3));}),0)),0))->r));}goto _LL2EA;}_LL2EF: if(_tmp466 <= (void*)1  || *((int*)_tmp466)
!= 1)goto _LL2EA;_tmp468=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp466)->f1;
_LL2F0:{void*_tmp46F=(void*)_tmp2EA->annot;struct Cyc_List_List*_tmp470;_LL2F2:
if(*((void**)_tmp46F)!= Cyc_CfFlowInfo_HasTagCmps)goto _LL2F4;_tmp470=((struct Cyc_CfFlowInfo_HasTagCmps_struct*)
_tmp46F)->f1;_LL2F3: for(0;_tmp470 != 0;_tmp470=_tmp470->tl){if((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp470->hd)->cmp == (void*)8  && Cyc_Tcutil_typecmp((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp470->hd)->bd,_tmp468)== 0){in_bnds=1;break;}}goto _LL2F1;_LL2F4:;_LL2F5: goto
_LL2F1;_LL2F1:;}if(!in_bnds)({void*_tmp471[0]={};Cyc_Tcutil_terr(e->loc,({const
char*_tmp472="cannot determine that subscript is in bounds";_tag_arr(_tmp472,
sizeof(char),_get_zero_arr_size(_tmp472,45));}),_tag_arr(_tmp471,sizeof(void*),0));});
goto _LL2EA;_LL2EA:;}goto _LL2DE;}_LL2E3:;_LL2E4:({void*_tmp473[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp474="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_arr(_tmp474,sizeof(char),_get_zero_arr_size(_tmp474,49));}),_tag_arr(
_tmp473,sizeof(void*),0));});_LL2DE:;}goto _LL1D9;}_LL214: if(*((int*)_tmp2AF)!= 
26)goto _LL216;_tmp2EB=((struct Cyc_Absyn_Tuple_e_struct*)_tmp2AF)->f1;_LL215: if(!
nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,0,0,_tmp2EB))->r));
else{struct Cyc_List_List*_tmp475=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,
_tmp2EB);struct _tagged_arr*_tmp476=Cyc_Toc_add_tuple_type(_tmp475);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp2EB != 0;(_tmp2EB=_tmp2EB->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp2EB->hd);dles=({struct Cyc_List_List*_tmp477=_cycalloc(
sizeof(*_tmp477));_tmp477->hd=({struct _tuple6*_tmp478=_cycalloc(sizeof(*_tmp478));
_tmp478->f1=0;_tmp478->f2=(struct Cyc_Absyn_Exp*)_tmp2EB->hd;_tmp478;});_tmp477->tl=
dles;_tmp477;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,dles,0))->r));}
goto _LL1D9;_LL216: if(*((int*)_tmp2AF)!= 28)goto _LL218;_tmp2EC=((struct Cyc_Absyn_Array_e_struct*)
_tmp2AF)->f1;_LL217:(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,
_tmp2EC,0))->r));{struct Cyc_List_List*_tmp479=_tmp2EC;for(0;_tmp479 != 0;_tmp479=
_tmp479->tl){struct _tuple6 _tmp47B;struct Cyc_Absyn_Exp*_tmp47C;struct _tuple6*
_tmp47A=(struct _tuple6*)_tmp479->hd;_tmp47B=*_tmp47A;_tmp47C=_tmp47B.f2;Cyc_Toc_exp_to_c(
nv,_tmp47C);}}goto _LL1D9;_LL218: if(*((int*)_tmp2AF)!= 29)goto _LL21A;_tmp2ED=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp2AF)->f1;_tmp2EE=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2AF)->f2;_tmp2EF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2AF)->f3;
_tmp2F0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2AF)->f4;_LL219: {
unsigned int _tmp47E;int _tmp47F;struct _tuple5 _tmp47D=Cyc_Evexp_eval_const_uint_exp(
_tmp2EE);_tmp47E=_tmp47D.f1;_tmp47F=_tmp47D.f2;{void*_tmp480=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp2EF->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp2EF);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp2EF)){if(!_tmp47F)({
void*_tmp481[0]={};Cyc_Tcutil_terr(_tmp2EE->loc,({const char*_tmp482="cannot determine value of constant";
_tag_arr(_tmp482,sizeof(char),_get_zero_arr_size(_tmp482,35));}),_tag_arr(
_tmp481,sizeof(void*),0));});{unsigned int i=0;for(0;i < _tmp47E;i ++){es=({struct
Cyc_List_List*_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483->hd=({struct _tuple6*
_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484->f1=0;_tmp484->f2=_tmp2EF;_tmp484;});
_tmp483->tl=es;_tmp483;});}}if(_tmp2F0){struct Cyc_Absyn_Exp*_tmp485=Cyc_Toc_cast_it(
_tmp480,Cyc_Absyn_uint_exp(0,0));es=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp486=
_cycalloc(sizeof(*_tmp486));_tmp486->hd=({struct _tuple6*_tmp487=_cycalloc(
sizeof(*_tmp487));_tmp487->f1=0;_tmp487->f2=_tmp485;_tmp487;});_tmp486->tl=0;
_tmp486;}));}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,es,0))->r));
goto _LL1D9;}}}_LL21A: if(*((int*)_tmp2AF)!= 30)goto _LL21C;_tmp2F1=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2AF)->f1;_tmp2F2=((struct Cyc_Absyn_Struct_e_struct*)_tmp2AF)->f2;_tmp2F3=((
struct Cyc_Absyn_Struct_e_struct*)_tmp2AF)->f3;_tmp2F4=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2AF)->f4;_LL21B: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp2F2 != 0,0,0,_tmp2F3,_tmp2F1))->r));else{if(_tmp2F4 == 0)({void*
_tmp488[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp489="Struct_e: missing aggrdecl pointer";_tag_arr(_tmp489,sizeof(
char),_get_zero_arr_size(_tmp489,35));}),_tag_arr(_tmp488,sizeof(void*),0));});{
struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp2F4;struct
_RegionHandle _tmp48A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp48A;
_push_region(rgn);{struct Cyc_List_List*_tmp48B=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp2F3,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);struct Cyc_List_List*
_tmp48C=0;struct Cyc_List_List*_tmp48D=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp48D != 0;_tmp48D=_tmp48D->tl){struct Cyc_List_List*
_tmp48E=_tmp48B;for(0;_tmp48E != 0;_tmp48E=_tmp48E->tl){if((*((struct _tuple11*)
_tmp48E->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp48D->hd){struct _tuple11
_tmp490;struct Cyc_Absyn_Aggrfield*_tmp491;struct Cyc_Absyn_Exp*_tmp492;struct
_tuple11*_tmp48F=(struct _tuple11*)_tmp48E->hd;_tmp490=*_tmp48F;_tmp491=_tmp490.f1;
_tmp492=_tmp490.f2;{void*_tmp493=(void*)_tmp491->type;Cyc_Toc_exp_to_c(nv,
_tmp492);if(Cyc_Toc_is_void_star(_tmp493))(void*)(_tmp492->r=(void*)((void*)(Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp492->r,0)))->r));_tmp48C=({
struct Cyc_List_List*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->hd=({struct
_tuple6*_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495->f1=0;_tmp495->f2=_tmp492;
_tmp495;});_tmp494->tl=_tmp48C;_tmp494;});break;}}}}(void*)(e->r=(void*)((void*)(
Cyc_Absyn_unresolvedmem_exp(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp48C),0))->r));};_pop_region(rgn);}}goto _LL1D9;_LL21C: if(*((
int*)_tmp2AF)!= 31)goto _LL21E;_tmp2F5=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2AF)->f1;_tmp2F6=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2AF)->f2;_LL21D: {
struct Cyc_List_List*fs;{void*_tmp496=Cyc_Tcutil_compress(_tmp2F5);struct Cyc_List_List*
_tmp497;_LL2F7: if(_tmp496 <= (void*)3  || *((int*)_tmp496)!= 11)goto _LL2F9;_tmp497=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp496)->f2;_LL2F8: fs=_tmp497;goto _LL2F6;
_LL2F9:;_LL2FA:({struct Cyc_String_pa_struct _tmp49A;_tmp49A.tag=0;_tmp49A.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp2F5));{void*
_tmp498[1]={& _tmp49A};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp499="anon struct has type %s";_tag_arr(_tmp499,sizeof(char),
_get_zero_arr_size(_tmp499,24));}),_tag_arr(_tmp498,sizeof(void*),1));}});_LL2F6:;}{
struct _RegionHandle _tmp49B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp49B;
_push_region(rgn);{struct Cyc_List_List*_tmp49C=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp2F6,
fs);for(0;_tmp49C != 0;_tmp49C=_tmp49C->tl){struct _tuple11 _tmp49E;struct Cyc_Absyn_Aggrfield*
_tmp49F;struct Cyc_Absyn_Exp*_tmp4A0;struct _tuple11*_tmp49D=(struct _tuple11*)
_tmp49C->hd;_tmp49E=*_tmp49D;_tmp49F=_tmp49E.f1;_tmp4A0=_tmp49E.f2;{void*_tmp4A1=(
void*)_tmp49F->type;Cyc_Toc_exp_to_c(nv,_tmp4A0);if(Cyc_Toc_is_void_star(_tmp4A1))(
void*)(_tmp4A0->r=(void*)((void*)(Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((
void*)_tmp4A0->r,0)))->r));}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(
0,_tmp2F6,0))->r));};_pop_region(rgn);}goto _LL1D9;}_LL21E: if(*((int*)_tmp2AF)!= 
32)goto _LL220;_tmp2F7=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2AF)->f1;if(_tmp2F7
!= 0)goto _LL220;_tmp2F8=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2AF)->f2;_tmp2F9=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp2AF)->f3;_LL21F: {struct _tuple1*qv=_tmp2F9->name;
struct Cyc_Absyn_Exp*tag_exp=_tmp2F8->is_xtunion?Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(
_tmp2F8,qv,0);(void*)(e->r=(void*)((void*)tag_exp->r));goto _LL1D9;}_LL220: if(*((
int*)_tmp2AF)!= 32)goto _LL222;_tmp2FA=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2AF)->f1;
_tmp2FB=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2AF)->f2;_tmp2FC=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp2AF)->f3;_LL221: {struct _tuple1*_tmp4A2=_tmp2FC->name;struct Cyc_List_List*
_tmp4A3=_tmp2FC->typs;struct _tuple1*_tmp4A4=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp4A5=Cyc_Absyn_var_exp(_tmp4A4,0);void*_tmp4A6=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp4A2,({const char*_tmp4B4="_struct";_tag_arr(_tmp4B4,sizeof(char),
_get_zero_arr_size(_tmp4B4,8));})));struct Cyc_Absyn_Exp*tag_exp=_tmp2FB->is_xtunion?
Cyc_Absyn_var_exp(_tmp4A2,0): Cyc_Toc_tunion_tag(_tmp2FB,_tmp4A2,1);if(nv->toplevel){
struct Cyc_List_List*dles=0;for(0;_tmp2FA != 0;(_tmp2FA=_tmp2FA->tl,_tmp4A3=
_tmp4A3->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp2FA->hd;void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)_check_null(
_tmp4A3))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);dles=({struct Cyc_List_List*_tmp4A7=
_cycalloc(sizeof(*_tmp4A7));_tmp4A7->hd=({struct _tuple6*_tmp4A8=_cycalloc(
sizeof(*_tmp4A8));_tmp4A8->f1=0;_tmp4A8->f2=cur_e;_tmp4A8;});_tmp4A7->tl=dles;
_tmp4A7;});}dles=({struct Cyc_List_List*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));
_tmp4A9->hd=({struct _tuple6*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA->f1=0;
_tmp4AA->f2=tag_exp;_tmp4AA;});_tmp4A9->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(dles);_tmp4A9;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB->v=Cyc_Toc_collapse_qvar_tag(
_tmp4A2,({const char*_tmp4AC="_struct";_tag_arr(_tmp4AC,sizeof(char),
_get_zero_arr_size(_tmp4AC,8));}));_tmp4AB;}),dles,0))->r));}else{struct Cyc_List_List*
_tmp4AD=({struct Cyc_List_List*_tmp4B3=_cycalloc(sizeof(*_tmp4B3));_tmp4B3->hd=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp4A5,Cyc_Toc_tag_sp,0),tag_exp,
0);_tmp4B3->tl=0;_tmp4B3;});{int i=1;for(0;_tmp2FA != 0;(((_tmp2FA=_tmp2FA->tl,i ++)),
_tmp4A3=_tmp4A3->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp2FA->hd;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp4A3))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp4AE=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp4A5,Cyc_Absyn_fieldname(i),0),
cur_e,0);_tmp4AD=({struct Cyc_List_List*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));
_tmp4AF->hd=_tmp4AE;_tmp4AF->tl=_tmp4AD;_tmp4AF;});}}}{struct Cyc_Absyn_Stmt*
_tmp4B0=Cyc_Absyn_exp_stmt(_tmp4A5,0);struct Cyc_Absyn_Stmt*_tmp4B1=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp4B2=_cycalloc(sizeof(*_tmp4B2));_tmp4B2->hd=_tmp4B0;_tmp4B2->tl=_tmp4AD;
_tmp4B2;})),0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
_tmp4A4,_tmp4A6,0,_tmp4B1,0),0))->r));}}goto _LL1D9;}_LL222: if(*((int*)_tmp2AF)!= 
33)goto _LL224;_LL223: goto _LL225;_LL224: if(*((int*)_tmp2AF)!= 34)goto _LL226;
_LL225: goto _LL1D9;_LL226: if(*((int*)_tmp2AF)!= 35)goto _LL228;_tmp2FD=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp2AF)->f1;_tmp2FE=_tmp2FD.is_calloc;_tmp2FF=_tmp2FD.rgn;_tmp300=_tmp2FD.elt_type;
_tmp301=_tmp2FD.num_elts;_tmp302=_tmp2FD.fat_result;_LL227: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp300)));Cyc_Toc_exp_to_c(nv,_tmp301);if(_tmp302){struct
_tuple1*_tmp4B5=Cyc_Toc_temp_var();struct _tuple1*_tmp4B6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp2FE){xexp=_tmp301;if(_tmp2FF != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp2FF;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp4B5,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp300,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp4B5,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp4B7[3];_tmp4B7[2]=Cyc_Absyn_var_exp(_tmp4B5,0);_tmp4B7[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp4B7[0]=Cyc_Absyn_var_exp(_tmp4B6,0);((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp4B7,sizeof(
struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp301,0);if(_tmp2FF != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp2FF;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp4B5,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp300,Cyc_Absyn_var_exp(_tmp4B5,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp4B8[3];
_tmp4B8[2]=Cyc_Absyn_var_exp(_tmp4B5,0);_tmp4B8[1]=Cyc_Absyn_uint_exp(1,0);
_tmp4B8[0]=Cyc_Absyn_var_exp(_tmp4B6,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp4B8,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);}{struct Cyc_Absyn_Stmt*_tmp4B9=Cyc_Absyn_declare_stmt(_tmp4B5,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp4B6,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp4B9,0))->r));}}else{if(_tmp2FF != 0){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp2FF;Cyc_Toc_exp_to_c(nv,rgn);(
void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,
0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp300,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL1D9;}_LL228: if(*((int*)_tmp2AF)!= 37)goto _LL22A;_tmp303=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp2AF)->f1;_LL229: Cyc_Toc_stmt_to_c(nv,
_tmp303);goto _LL1D9;_LL22A: if(*((int*)_tmp2AF)!= 36)goto _LL22C;_LL22B:({void*
_tmp4BA[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4BB="UnresolvedMem";_tag_arr(_tmp4BB,sizeof(char),
_get_zero_arr_size(_tmp4BB,14));}),_tag_arr(_tmp4BA,sizeof(void*),0));});_LL22C:
if(*((int*)_tmp2AF)!= 27)goto _LL1D9;_LL22D:({void*_tmp4BC[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp4BD="compoundlit";
_tag_arr(_tmp4BD,sizeof(char),_get_zero_arr_size(_tmp4BD,12));}),_tag_arr(
_tmp4BC,sizeof(void*),0));});_LL1D9:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0);}struct _tuple12{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*
f3;};struct _tuple13{struct _tuple1*f1;void*f2;};struct _tuple14{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple12 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*
nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp4BE=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp4BF;
struct Cyc_Absyn_Vardecl _tmp4C0;struct _tuple1*_tmp4C1;struct Cyc_Absyn_Vardecl*
_tmp4C2;struct Cyc_Absyn_Vardecl _tmp4C3;struct _tuple1*_tmp4C4;struct Cyc_Absyn_Vardecl*
_tmp4C5;void*_tmp4C6;int _tmp4C7;char _tmp4C8;struct _tagged_arr _tmp4C9;struct Cyc_Absyn_Enumdecl*
_tmp4CA;struct Cyc_Absyn_Enumfield*_tmp4CB;void*_tmp4CC;struct Cyc_Absyn_Enumfield*
_tmp4CD;struct Cyc_Absyn_Tuniondecl*_tmp4CE;struct Cyc_Absyn_Tunionfield*_tmp4CF;
struct Cyc_List_List*_tmp4D0;struct Cyc_Absyn_Pat*_tmp4D1;struct Cyc_Absyn_Pat
_tmp4D2;void*_tmp4D3;struct Cyc_Absyn_Tuniondecl*_tmp4D4;struct Cyc_Absyn_Tunionfield*
_tmp4D5;struct Cyc_List_List*_tmp4D6;struct Cyc_List_List*_tmp4D7;struct Cyc_List_List*
_tmp4D8;struct Cyc_Absyn_AggrInfo _tmp4D9;void*_tmp4DA;struct Cyc_List_List*_tmp4DB;
struct Cyc_Absyn_Pat*_tmp4DC;_LL2FC: if(_tmp4BE <= (void*)2  || *((int*)_tmp4BE)!= 0)
goto _LL2FE;_tmp4BF=((struct Cyc_Absyn_Var_p_struct*)_tmp4BE)->f1;_tmp4C0=*_tmp4BF;
_tmp4C1=_tmp4C0.name;_LL2FD: _tmp4C4=_tmp4C1;goto _LL2FF;_LL2FE: if(_tmp4BE <= (void*)
2  || *((int*)_tmp4BE)!= 2)goto _LL300;_tmp4C2=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp4BE)->f2;_tmp4C3=*_tmp4C2;_tmp4C4=_tmp4C3.name;_LL2FF: nv=Cyc_Toc_add_varmap(
nv,_tmp4C4,r);goto _LL301;_LL300: if((int)_tmp4BE != 0)goto _LL302;_LL301: s=Cyc_Toc_skip_stmt_dl();
goto _LL2FB;_LL302: if(_tmp4BE <= (void*)2  || *((int*)_tmp4BE)!= 1)goto _LL304;
_tmp4C5=((struct Cyc_Absyn_Reference_p_struct*)_tmp4BE)->f1;_LL303: {struct
_tuple1*_tmp4DD=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*_tmp4DE=
_region_malloc(rgn,sizeof(*_tmp4DE));_tmp4DE->hd=({struct _tuple13*_tmp4DF=
_region_malloc(rgn,sizeof(*_tmp4DF));_tmp4DF->f1=_tmp4DD;_tmp4DF->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp4DF;});_tmp4DE->tl=decls;_tmp4DE;});nv=
Cyc_Toc_add_varmap(nv,_tmp4C5->name,Cyc_Absyn_var_exp(_tmp4DD,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp4DD,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(path,0)),0);goto _LL2FB;}_LL304: if((int)
_tmp4BE != 1)goto _LL306;_LL305: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,
0),fail_stmt);goto _LL2FB;_LL306: if(_tmp4BE <= (void*)2  || *((int*)_tmp4BE)!= 7)
goto _LL308;_tmp4C6=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp4BE)->f1;_tmp4C7=((
struct Cyc_Absyn_Int_p_struct*)_tmp4BE)->f2;_LL307: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp4C6,_tmp4C7,0),fail_stmt);goto _LL2FB;_LL308: if(_tmp4BE <= (void*)2  || *((int*)
_tmp4BE)!= 8)goto _LL30A;_tmp4C8=((struct Cyc_Absyn_Char_p_struct*)_tmp4BE)->f1;
_LL309: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp4C8,0),fail_stmt);goto
_LL2FB;_LL30A: if(_tmp4BE <= (void*)2  || *((int*)_tmp4BE)!= 9)goto _LL30C;_tmp4C9=((
struct Cyc_Absyn_Float_p_struct*)_tmp4BE)->f1;_LL30B: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp4C9,0),fail_stmt);goto _LL2FB;_LL30C: if(_tmp4BE <= (void*)2  || *((int*)_tmp4BE)
!= 10)goto _LL30E;_tmp4CA=((struct Cyc_Absyn_Enum_p_struct*)_tmp4BE)->f1;_tmp4CB=((
struct Cyc_Absyn_Enum_p_struct*)_tmp4BE)->f2;_LL30D: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Enum_e_struct*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));
_tmp4E0[0]=({struct Cyc_Absyn_Enum_e_struct _tmp4E1;_tmp4E1.tag=33;_tmp4E1.f1=
_tmp4CB->name;_tmp4E1.f2=(struct Cyc_Absyn_Enumdecl*)_tmp4CA;_tmp4E1.f3=(struct
Cyc_Absyn_Enumfield*)_tmp4CB;_tmp4E1;});_tmp4E0;}),0),fail_stmt);goto _LL2FB;
_LL30E: if(_tmp4BE <= (void*)2  || *((int*)_tmp4BE)!= 11)goto _LL310;_tmp4CC=(void*)((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp4BE)->f1;_tmp4CD=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp4BE)->f2;_LL30F: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*
_tmp4E2=_cycalloc(sizeof(*_tmp4E2));_tmp4E2[0]=({struct Cyc_Absyn_AnonEnum_e_struct
_tmp4E3;_tmp4E3.tag=34;_tmp4E3.f1=_tmp4CD->name;_tmp4E3.f2=(void*)_tmp4CC;
_tmp4E3.f3=(struct Cyc_Absyn_Enumfield*)_tmp4CD;_tmp4E3;});_tmp4E2;}),0),
fail_stmt);goto _LL2FB;_LL310: if(_tmp4BE <= (void*)2  || *((int*)_tmp4BE)!= 6)goto
_LL312;_tmp4CE=((struct Cyc_Absyn_Tunion_p_struct*)_tmp4BE)->f1;_tmp4CF=((struct
Cyc_Absyn_Tunion_p_struct*)_tmp4BE)->f2;_tmp4D0=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp4BE)->f3;if(_tmp4D0 != 0)goto _LL312;_LL311: {struct Cyc_Absyn_Exp*cmp_exp;if(
_tmp4CE->is_xtunion)cmp_exp=Cyc_Absyn_var_exp(_tmp4CF->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(
_tmp4CE,_tmp4CF->name,0);r=Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(
r,cmp_exp,fail_stmt);goto _LL2FB;}_LL312: if(_tmp4BE <= (void*)2  || *((int*)_tmp4BE)
!= 4)goto _LL314;_tmp4D1=((struct Cyc_Absyn_Pointer_p_struct*)_tmp4BE)->f1;_tmp4D2=*
_tmp4D1;_tmp4D3=(void*)_tmp4D2.r;if(_tmp4D3 <= (void*)2  || *((int*)_tmp4D3)!= 6)
goto _LL314;_tmp4D4=((struct Cyc_Absyn_Tunion_p_struct*)_tmp4D3)->f1;_tmp4D5=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp4D3)->f2;_tmp4D6=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp4D3)->f3;if(!(_tmp4D6 != 0))goto _LL314;_LL313: s=Cyc_Toc_skip_stmt_dl();{int
cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp4D5->name,({const char*
_tmp4F0="_struct";_tag_arr(_tmp4F0,sizeof(char),_get_zero_arr_size(_tmp4F0,8));}));
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(
tufstrct),Cyc_Toc_mt_tq),r);struct Cyc_List_List*_tmp4E4=_tmp4D5->typs;for(0;
_tmp4D6 != 0;(((_tmp4D6=_tmp4D6->tl,_tmp4E4=((struct Cyc_List_List*)_check_null(
_tmp4E4))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp4E5=(struct Cyc_Absyn_Pat*)
_tmp4D6->hd;if((void*)_tmp4E5->r == (void*)0)continue;{void*_tmp4E6=(*((struct
_tuple4*)((struct Cyc_List_List*)_check_null(_tmp4E4))->hd)).f2;struct _tuple1*
_tmp4E7=Cyc_Toc_temp_var();void*_tmp4E8=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4E5->topt))->v;void*_tmp4E9=Cyc_Toc_typ_to_c(_tmp4E8);struct Cyc_Absyn_Exp*
_tmp4EA=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp4E6)))_tmp4EA=Cyc_Toc_cast_it(_tmp4E9,_tmp4EA);decls=({
struct Cyc_List_List*_tmp4EB=_region_malloc(rgn,sizeof(*_tmp4EB));_tmp4EB->hd=({
struct _tuple13*_tmp4EC=_region_malloc(rgn,sizeof(*_tmp4EC));_tmp4EC->f1=_tmp4E7;
_tmp4EC->f2=_tmp4E9;_tmp4EC;});_tmp4EB->tl=decls;_tmp4EB;});{struct _tuple12
_tmp4ED=Cyc_Toc_xlate_pat(nv,rgn,_tmp4E8,Cyc_Absyn_var_exp(_tmp4E7,0),_tmp4EA,
_tmp4E5,fail_stmt,decls);nv=_tmp4ED.f1;decls=_tmp4ED.f2;{struct Cyc_Absyn_Stmt*
_tmp4EE=_tmp4ED.f3;struct Cyc_Absyn_Stmt*_tmp4EF=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp4E7,0),_tmp4EA,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp4EF,_tmp4EE,
0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(_tmp4D4->is_xtunion){struct Cyc_Absyn_Exp*
e2=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),
r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(
_tmp4D5->name,0);test_exp=Cyc_Absyn_neq_exp(e1,e,0);}else{int max_tag=Cyc_Toc_num_void_tags(
_tmp4D4);struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)
max_tag,0);struct Cyc_Absyn_Exp*e3=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,
Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e5=Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),max_tag_exp),0);
struct Cyc_Absyn_Exp*e=Cyc_Toc_tunion_tag(_tmp4D4,_tmp4D5->name,1);test_exp=
max_tag > 0?Cyc_Absyn_or_exp(e5,Cyc_Absyn_neq_exp(e1,e,0),0): Cyc_Absyn_neq_exp(e1,
e,0);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);goto _LL2FB;}}_LL314: if(_tmp4BE <= (void*)2  || *((int*)_tmp4BE)!= 6)goto
_LL316;_tmp4D7=((struct Cyc_Absyn_Tunion_p_struct*)_tmp4BE)->f3;_LL315: _tmp4D8=
_tmp4D7;goto _LL317;_LL316: if(_tmp4BE <= (void*)2  || *((int*)_tmp4BE)!= 3)goto
_LL318;_tmp4D8=((struct Cyc_Absyn_Tuple_p_struct*)_tmp4BE)->f1;_LL317: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;for(0;_tmp4D8 != 0;(_tmp4D8=_tmp4D8->tl,++ cnt)){struct Cyc_Absyn_Pat*
_tmp4F1=(struct Cyc_Absyn_Pat*)_tmp4D8->hd;if((void*)_tmp4F1->r == (void*)0)
continue;{struct _tuple1*_tmp4F2=Cyc_Toc_temp_var();void*_tmp4F3=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4F1->topt))->v;decls=({struct Cyc_List_List*_tmp4F4=
_region_malloc(rgn,sizeof(*_tmp4F4));_tmp4F4->hd=({struct _tuple13*_tmp4F5=
_region_malloc(rgn,sizeof(*_tmp4F5));_tmp4F5->f1=_tmp4F2;_tmp4F5->f2=Cyc_Toc_typ_to_c(
_tmp4F3);_tmp4F5;});_tmp4F4->tl=decls;_tmp4F4;});{struct _tuple12 _tmp4F6=Cyc_Toc_xlate_pat(
nv,rgn,_tmp4F3,Cyc_Absyn_var_exp(_tmp4F2,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp4F1,fail_stmt,decls);nv=_tmp4F6.f1;decls=_tmp4F6.f2;{struct Cyc_Absyn_Stmt*
_tmp4F7=_tmp4F6.f3;struct Cyc_Absyn_Stmt*_tmp4F8=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp4F2,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp4F8,_tmp4F7,0),0);}}}}goto _LL2FB;}_LL318: if(_tmp4BE <= (
void*)2  || *((int*)_tmp4BE)!= 5)goto _LL31A;_tmp4D9=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp4BE)->f1;_tmp4DA=(void*)_tmp4D9.aggr_info;_tmp4DB=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp4BE)->f3;_LL319: {struct Cyc_Absyn_Aggrdecl*_tmp4F9=Cyc_Absyn_get_known_aggrdecl(
_tmp4DA);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp4DB != 0;_tmp4DB=_tmp4DB->tl){struct
_tuple14*_tmp4FA=(struct _tuple14*)_tmp4DB->hd;struct Cyc_Absyn_Pat*_tmp4FB=(*
_tmp4FA).f2;if((void*)_tmp4FB->r == (void*)0)continue;{struct _tagged_arr*f;{void*
_tmp4FC=(void*)((struct Cyc_List_List*)_check_null((*_tmp4FA).f1))->hd;struct
_tagged_arr*_tmp4FD;_LL323: if(*((int*)_tmp4FC)!= 1)goto _LL325;_tmp4FD=((struct
Cyc_Absyn_FieldName_struct*)_tmp4FC)->f1;_LL324: f=_tmp4FD;goto _LL322;_LL325:;
_LL326:(int)_throw((void*)Cyc_Toc_Match_error);_LL322:;}{struct _tuple1*_tmp4FE=
Cyc_Toc_temp_var();void*_tmp4FF=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4FB->topt))->v;
void*_tmp500=Cyc_Toc_typ_to_c(_tmp4FF);decls=({struct Cyc_List_List*_tmp501=
_region_malloc(rgn,sizeof(*_tmp501));_tmp501->hd=({struct _tuple13*_tmp502=
_region_malloc(rgn,sizeof(*_tmp502));_tmp502->f1=_tmp4FE;_tmp502->f2=_tmp500;
_tmp502;});_tmp501->tl=decls;_tmp501;});{struct _tuple12 _tmp503=Cyc_Toc_xlate_pat(
nv,rgn,_tmp4FF,Cyc_Absyn_var_exp(_tmp4FE,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp4FB,fail_stmt,decls);nv=_tmp503.f1;decls=_tmp503.f2;{struct Cyc_Absyn_Exp*
_tmp504=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((void*)((struct
Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4F9->impl))->fields,f)))->type))_tmp504=Cyc_Toc_cast_it(_tmp500,
_tmp504);{struct Cyc_Absyn_Stmt*_tmp505=_tmp503.f3;struct Cyc_Absyn_Stmt*_tmp506=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp4FE,0),_tmp504,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp506,_tmp505,0),0);}}}}}}goto _LL2FB;}_LL31A: if(_tmp4BE <= (
void*)2  || *((int*)_tmp4BE)!= 4)goto _LL31C;_tmp4DC=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp4BE)->f1;_LL31B: {struct _tuple1*_tmp507=Cyc_Toc_temp_var();void*_tmp508=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4DC->topt))->v;decls=({struct Cyc_List_List*
_tmp509=_region_malloc(rgn,sizeof(*_tmp509));_tmp509->hd=({struct _tuple13*
_tmp50A=_region_malloc(rgn,sizeof(*_tmp50A));_tmp50A->f1=_tmp507;_tmp50A->f2=Cyc_Toc_typ_to_c(
_tmp508);_tmp50A;});_tmp509->tl=decls;_tmp509;});{struct _tuple12 _tmp50B=Cyc_Toc_xlate_pat(
nv,rgn,_tmp508,Cyc_Absyn_var_exp(_tmp507,0),Cyc_Absyn_deref_exp(path,0),_tmp4DC,
fail_stmt,decls);nv=_tmp50B.f1;decls=_tmp50B.f2;{struct Cyc_Absyn_Stmt*_tmp50C=
_tmp50B.f3;struct Cyc_Absyn_Stmt*_tmp50D=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp507,0),Cyc_Absyn_deref_exp(r,0),0),_tmp50C,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp50D,0);else{s=_tmp50D;}goto
_LL2FB;}}}_LL31C: if(_tmp4BE <= (void*)2  || *((int*)_tmp4BE)!= 12)goto _LL31E;
_LL31D:({void*_tmp50E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp50F="unknownid pat";_tag_arr(_tmp50F,sizeof(
char),_get_zero_arr_size(_tmp50F,14));}),_tag_arr(_tmp50E,sizeof(void*),0));});
_LL31E: if(_tmp4BE <= (void*)2  || *((int*)_tmp4BE)!= 13)goto _LL320;_LL31F:({void*
_tmp510[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp511="unknowncall pat";_tag_arr(_tmp511,sizeof(char),
_get_zero_arr_size(_tmp511,16));}),_tag_arr(_tmp510,sizeof(void*),0));});_LL320:
if(_tmp4BE <= (void*)2  || *((int*)_tmp4BE)!= 14)goto _LL2FB;_LL321:({void*_tmp512[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp513="exp pat";_tag_arr(_tmp513,sizeof(char),_get_zero_arr_size(
_tmp513,8));}),_tag_arr(_tmp512,sizeof(void*),0));});_LL2FB:;}return({struct
_tuple12 _tmp514;_tmp514.f1=nv;_tmp514.f2=decls;_tmp514.f3=s;_tmp514;});}struct
_tuple15{struct _tagged_arr*f1;struct _tagged_arr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple15*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple15*_tmp515=_region_malloc(r,sizeof(*_tmp515));_tmp515->f1=
Cyc_Toc_fresh_label();_tmp515->f2=Cyc_Toc_fresh_label();_tmp515->f3=sc;_tmp515;});}
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*
_tmp516=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int leave_as_switch;{
void*_tmp517=Cyc_Tcutil_compress(_tmp516);_LL328: if(_tmp517 <= (void*)3  || *((int*)
_tmp517)!= 5)goto _LL32A;_LL329: goto _LL32B;_LL32A: if(_tmp517 <= (void*)3  || *((int*)
_tmp517)!= 12)goto _LL32C;_LL32B: leave_as_switch=1;goto _LL327;_LL32C:;_LL32D:
leave_as_switch=0;goto _LL327;_LL327:;}{struct Cyc_List_List*_tmp518=scs;for(0;
_tmp518 != 0;_tmp518=_tmp518->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp518->hd)->pat_vars))->v != 0  || ((
struct Cyc_Absyn_Switch_clause*)_tmp518->hd)->where_clause != 0){leave_as_switch=0;
break;}}}if(leave_as_switch){struct _tagged_arr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp519=scs;for(0;_tmp519 != 0;_tmp519=_tmp519->tl){struct Cyc_Absyn_Stmt*
_tmp51A=((struct Cyc_Absyn_Switch_clause*)_tmp519->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp519->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp51A,0);next_l=Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(nv,next_l),_tmp51A);}}return;}{
struct _tuple1*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct _tagged_arr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp51B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp51B;
_push_region(rgn);{struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple15*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,
rgn,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*
decls=0;{struct Cyc_List_List*_tmp51C=lscs;for(0;_tmp51C != 0;_tmp51C=_tmp51C->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple15*)_tmp51C->hd)).f3;struct
_tagged_arr*fail_lab=_tmp51C->tl == 0?end_l:(*((struct _tuple15*)((struct Cyc_List_List*)
_check_null(_tmp51C->tl))->hd)).f1;struct Cyc_Toc_Env*_tmp51E;struct Cyc_List_List*
_tmp51F;struct Cyc_Absyn_Stmt*_tmp520;struct _tuple12 _tmp51D=Cyc_Toc_xlate_pat(nv,
rgn,_tmp516,r,path,sc->pattern,Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp51E=
_tmp51D.f1;_tmp51F=_tmp51D.f2;_tmp520=_tmp51D.f3;if(sc->where_clause != 0){struct
Cyc_Absyn_Exp*_tmp521=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(
_tmp51E,_tmp521);_tmp520=Cyc_Absyn_seq_stmt(_tmp520,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp521,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp51F;nvs=({struct Cyc_List_List*_tmp522=_region_malloc(rgn,
sizeof(*_tmp522));_tmp522->hd=_tmp51E;_tmp522->tl=nvs;_tmp522;});test_stmts=({
struct Cyc_List_List*_tmp523=_region_malloc(rgn,sizeof(*_tmp523));_tmp523->hd=
_tmp520;_tmp523->tl=test_stmts;_tmp523;});}}nvs=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple15 _tmp525;struct _tagged_arr*_tmp526;struct _tagged_arr*_tmp527;struct
Cyc_Absyn_Switch_clause*_tmp528;struct _tuple15*_tmp524=(struct _tuple15*)lscs->hd;
_tmp525=*_tmp524;_tmp526=_tmp525.f1;_tmp527=_tmp525.f2;_tmp528=_tmp525.f3;{
struct Cyc_Toc_Env*_tmp529=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp528->body;if(lscs->tl != 0){struct _tuple15
_tmp52B;struct _tagged_arr*_tmp52C;struct Cyc_Absyn_Switch_clause*_tmp52D;struct
_tuple15*_tmp52A=(struct _tuple15*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;
_tmp52B=*_tmp52A;_tmp52C=_tmp52B.f2;_tmp52D=_tmp52B.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
_tmp529,end_l,_tmp52C,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp52D->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp529,
end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp526,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp527,s,0),0);stmts=({struct Cyc_List_List*_tmp52E=_region_malloc(rgn,sizeof(*
_tmp52E));_tmp52E->hd=s;_tmp52E->tl=stmts;_tmp52E;});}}{struct Cyc_Absyn_Stmt*res=
Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),
0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple13 _tmp530;struct _tuple1*
_tmp531;void*_tmp532;struct _tuple13*_tmp52F=(struct _tuple13*)decls->hd;_tmp530=*
_tmp52F;_tmp531=_tmp530.f1;_tmp532=_tmp530.f2;res=Cyc_Absyn_declare_stmt(_tmp531,
_tmp532,0,res,0);}(void*)(whole_s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(v,
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct
Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(rgn);}}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp533=
_cycalloc(sizeof(*_tmp533));_tmp533->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),
0);_tmp533->tl=0;_tmp533;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp534=(void*)s->r;
struct Cyc_Absyn_Exp*_tmp535;struct Cyc_Absyn_Stmt*_tmp536;struct Cyc_Absyn_Stmt*
_tmp537;struct Cyc_Absyn_Exp*_tmp538;struct Cyc_Absyn_Exp*_tmp539;struct Cyc_Absyn_Stmt*
_tmp53A;struct Cyc_Absyn_Stmt*_tmp53B;struct _tuple3 _tmp53C;struct Cyc_Absyn_Exp*
_tmp53D;struct Cyc_Absyn_Stmt*_tmp53E;struct Cyc_Absyn_Stmt*_tmp53F;struct Cyc_Absyn_Stmt*
_tmp540;struct Cyc_Absyn_Stmt*_tmp541;struct Cyc_Absyn_Exp*_tmp542;struct _tuple3
_tmp543;struct Cyc_Absyn_Exp*_tmp544;struct _tuple3 _tmp545;struct Cyc_Absyn_Exp*
_tmp546;struct Cyc_Absyn_Stmt*_tmp547;struct Cyc_Absyn_Exp*_tmp548;struct Cyc_List_List*
_tmp549;struct Cyc_List_List*_tmp54A;struct Cyc_Absyn_Switch_clause**_tmp54B;
struct Cyc_Absyn_Decl*_tmp54C;struct Cyc_Absyn_Stmt*_tmp54D;struct _tagged_arr*
_tmp54E;struct Cyc_Absyn_Stmt*_tmp54F;struct Cyc_Absyn_Stmt*_tmp550;struct _tuple3
_tmp551;struct Cyc_Absyn_Exp*_tmp552;struct Cyc_Absyn_Stmt*_tmp553;struct Cyc_List_List*
_tmp554;struct Cyc_Absyn_Tvar*_tmp555;struct Cyc_Absyn_Vardecl*_tmp556;int _tmp557;
struct Cyc_Absyn_Stmt*_tmp558;struct Cyc_Absyn_Exp*_tmp559;_LL32F: if((int)_tmp534
!= 0)goto _LL331;_LL330: return;_LL331: if(_tmp534 <= (void*)1  || *((int*)_tmp534)!= 
0)goto _LL333;_tmp535=((struct Cyc_Absyn_Exp_s_struct*)_tmp534)->f1;_LL332: Cyc_Toc_exp_to_c(
nv,_tmp535);return;_LL333: if(_tmp534 <= (void*)1  || *((int*)_tmp534)!= 1)goto
_LL335;_tmp536=((struct Cyc_Absyn_Seq_s_struct*)_tmp534)->f1;_tmp537=((struct Cyc_Absyn_Seq_s_struct*)
_tmp534)->f2;_LL334: Cyc_Toc_stmt_to_c(nv,_tmp536);s=_tmp537;continue;_LL335: if(
_tmp534 <= (void*)1  || *((int*)_tmp534)!= 2)goto _LL337;_tmp538=((struct Cyc_Absyn_Return_s_struct*)
_tmp534)->f1;_LL336: {struct Cyc_Core_Opt*topt=0;if(_tmp538 != 0){topt=({struct Cyc_Core_Opt*
_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp538->topt))->v);_tmp55A;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp538);}if(s->try_depth > 0){if(topt != 0){struct
_tuple1*_tmp55B=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp55C=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp55B,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp55B,(void*)topt->v,_tmp538,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp55C,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL337: if(_tmp534 <= (void*)1  || *((int*)_tmp534)!= 3)goto _LL339;_tmp539=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp534)->f1;_tmp53A=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp534)->f2;_tmp53B=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp534)->f3;_LL338:
Cyc_Toc_exp_to_c(nv,_tmp539);Cyc_Toc_stmt_to_c(nv,_tmp53A);s=_tmp53B;continue;
_LL339: if(_tmp534 <= (void*)1  || *((int*)_tmp534)!= 4)goto _LL33B;_tmp53C=((struct
Cyc_Absyn_While_s_struct*)_tmp534)->f1;_tmp53D=_tmp53C.f1;_tmp53E=((struct Cyc_Absyn_While_s_struct*)
_tmp534)->f2;_LL33A: Cyc_Toc_exp_to_c(nv,_tmp53D);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
nv),_tmp53E);return;_LL33B: if(_tmp534 <= (void*)1  || *((int*)_tmp534)!= 5)goto
_LL33D;_tmp53F=((struct Cyc_Absyn_Break_s_struct*)_tmp534)->f1;_LL33C: if(nv->break_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->break_lab))->v,0))->r));{int dest_depth=
_tmp53F == 0?0: _tmp53F->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,
s);return;}_LL33D: if(_tmp534 <= (void*)1  || *((int*)_tmp534)!= 6)goto _LL33F;
_tmp540=((struct Cyc_Absyn_Continue_s_struct*)_tmp534)->f1;_LL33E: if(nv->continue_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->continue_lab))->v,0))->r));_tmp541=_tmp540;
goto _LL340;_LL33F: if(_tmp534 <= (void*)1  || *((int*)_tmp534)!= 7)goto _LL341;
_tmp541=((struct Cyc_Absyn_Goto_s_struct*)_tmp534)->f2;_LL340: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp541))->try_depth,s);
return;_LL341: if(_tmp534 <= (void*)1  || *((int*)_tmp534)!= 8)goto _LL343;_tmp542=((
struct Cyc_Absyn_For_s_struct*)_tmp534)->f1;_tmp543=((struct Cyc_Absyn_For_s_struct*)
_tmp534)->f2;_tmp544=_tmp543.f1;_tmp545=((struct Cyc_Absyn_For_s_struct*)_tmp534)->f3;
_tmp546=_tmp545.f1;_tmp547=((struct Cyc_Absyn_For_s_struct*)_tmp534)->f4;_LL342:
Cyc_Toc_exp_to_c(nv,_tmp542);Cyc_Toc_exp_to_c(nv,_tmp544);Cyc_Toc_exp_to_c(nv,
_tmp546);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp547);return;_LL343: if(
_tmp534 <= (void*)1  || *((int*)_tmp534)!= 9)goto _LL345;_tmp548=((struct Cyc_Absyn_Switch_s_struct*)
_tmp534)->f1;_tmp549=((struct Cyc_Absyn_Switch_s_struct*)_tmp534)->f2;_LL344: Cyc_Toc_xlate_switch(
nv,s,_tmp548,_tmp549);return;_LL345: if(_tmp534 <= (void*)1  || *((int*)_tmp534)!= 
10)goto _LL347;_tmp54A=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp534)->f1;_tmp54B=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp534)->f2;_LL346: if(nv->fallthru_info == 0)({
void*_tmp55D[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp55E="fallthru in unexpected place";_tag_arr(_tmp55E,sizeof(char),
_get_zero_arr_size(_tmp55E,29));}),_tag_arr(_tmp55D,sizeof(void*),0));});{struct
_tuple8 _tmp560;struct _tagged_arr*_tmp561;struct Cyc_List_List*_tmp562;struct Cyc_Dict_Dict*
_tmp563;struct _tuple8*_tmp55F=(struct _tuple8*)((struct Cyc_Core_Opt*)_check_null(
nv->fallthru_info))->v;_tmp560=*_tmp55F;_tmp561=_tmp560.f1;_tmp562=_tmp560.f2;
_tmp563=_tmp560.f3;{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp561,0);Cyc_Toc_do_npop_before(
s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp54B)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp564=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp562);struct Cyc_List_List*_tmp565=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp54A);for(0;_tmp564 != 0;(_tmp564=_tmp564->tl,
_tmp565=_tmp565->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp565))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup)(
_tmp563,(struct _tuple1*)_tmp564->hd),(struct Cyc_Absyn_Exp*)_tmp565->hd,0),s2,0);}(
void*)(s->r=(void*)((void*)s2->r));return;}}}_LL347: if(_tmp534 <= (void*)1  || *((
int*)_tmp534)!= 11)goto _LL349;_tmp54C=((struct Cyc_Absyn_Decl_s_struct*)_tmp534)->f1;
_tmp54D=((struct Cyc_Absyn_Decl_s_struct*)_tmp534)->f2;_LL348:{void*_tmp566=(void*)
_tmp54C->r;struct Cyc_Absyn_Vardecl*_tmp567;struct Cyc_Absyn_Pat*_tmp568;struct Cyc_Absyn_Exp*
_tmp569;struct Cyc_List_List*_tmp56A;_LL354: if(_tmp566 <= (void*)2  || *((int*)
_tmp566)!= 0)goto _LL356;_tmp567=((struct Cyc_Absyn_Var_d_struct*)_tmp566)->f1;
_LL355: {struct Cyc_Toc_Env*_tmp56B=Cyc_Toc_add_varmap(nv,_tmp567->name,Cyc_Absyn_varb_exp(
_tmp567->name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp56C=_cycalloc(sizeof(*
_tmp56C));_tmp56C[0]=({struct Cyc_Absyn_Local_b_struct _tmp56D;_tmp56D.tag=3;
_tmp56D.f1=_tmp567;_tmp56D;});_tmp56C;}),0));Cyc_Toc_local_decl_to_c(_tmp56B,
_tmp56B,_tmp567,_tmp54D);goto _LL353;}_LL356: if(_tmp566 <= (void*)2  || *((int*)
_tmp566)!= 2)goto _LL358;_tmp568=((struct Cyc_Absyn_Let_d_struct*)_tmp566)->f1;
_tmp569=((struct Cyc_Absyn_Let_d_struct*)_tmp566)->f3;_LL357:{void*_tmp56E=(void*)
_tmp568->r;struct Cyc_Absyn_Vardecl*_tmp56F;_LL35D: if(_tmp56E <= (void*)2  || *((
int*)_tmp56E)!= 0)goto _LL35F;_tmp56F=((struct Cyc_Absyn_Var_p_struct*)_tmp56E)->f1;
_LL35E: {struct _tuple1*old_name=_tmp56F->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp56F->name=new_name;_tmp56F->initializer=(struct Cyc_Absyn_Exp*)_tmp569;(void*)(
_tmp54C->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp570=_cycalloc(
sizeof(*_tmp570));_tmp570[0]=({struct Cyc_Absyn_Var_d_struct _tmp571;_tmp571.tag=0;
_tmp571.f1=_tmp56F;_tmp571;});_tmp570;})));{struct Cyc_Toc_Env*_tmp572=Cyc_Toc_add_varmap(
nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573[0]=({struct Cyc_Absyn_Local_b_struct
_tmp574;_tmp574.tag=3;_tmp574.f1=_tmp56F;_tmp574;});_tmp573;}),0));Cyc_Toc_local_decl_to_c(
_tmp572,nv,_tmp56F,_tmp54D);goto _LL35C;}}_LL35F:;_LL360:(void*)(s->r=(void*)((
void*)(Cyc_Toc_letdecl_to_c(nv,_tmp568,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp569->topt))->v,_tmp569,_tmp54D))->r));goto _LL35C;_LL35C:;}goto _LL353;_LL358:
if(_tmp566 <= (void*)2  || *((int*)_tmp566)!= 3)goto _LL35A;_tmp56A=((struct Cyc_Absyn_Letv_d_struct*)
_tmp566)->f1;_LL359: {struct Cyc_List_List*_tmp575=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp56A);if(_tmp575 == 0)({void*_tmp576[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp577="empty Letv_d";_tag_arr(_tmp577,sizeof(char),_get_zero_arr_size(_tmp577,
13));}),_tag_arr(_tmp576,sizeof(void*),0));});(void*)(_tmp54C->r=(void*)((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578[0]=({
struct Cyc_Absyn_Var_d_struct _tmp579;_tmp579.tag=0;_tmp579.f1=(struct Cyc_Absyn_Vardecl*)
_tmp575->hd;_tmp579;});_tmp578;})));_tmp575=_tmp575->tl;for(0;_tmp575 != 0;
_tmp575=_tmp575->tl){struct Cyc_Absyn_Decl*_tmp57A=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B[0]=({
struct Cyc_Absyn_Var_d_struct _tmp57C;_tmp57C.tag=0;_tmp57C.f1=(struct Cyc_Absyn_Vardecl*)
_tmp575->hd;_tmp57C;});_tmp57B;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp57A,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto
_LL353;}_LL35A:;_LL35B:({void*_tmp57D[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp57E="bad nested declaration within function";
_tag_arr(_tmp57E,sizeof(char),_get_zero_arr_size(_tmp57E,39));}),_tag_arr(
_tmp57D,sizeof(void*),0));});_LL353:;}return;_LL349: if(_tmp534 <= (void*)1  || *((
int*)_tmp534)!= 12)goto _LL34B;_tmp54E=((struct Cyc_Absyn_Label_s_struct*)_tmp534)->f1;
_tmp54F=((struct Cyc_Absyn_Label_s_struct*)_tmp534)->f2;_LL34A: s=_tmp54F;
continue;_LL34B: if(_tmp534 <= (void*)1  || *((int*)_tmp534)!= 13)goto _LL34D;
_tmp550=((struct Cyc_Absyn_Do_s_struct*)_tmp534)->f1;_tmp551=((struct Cyc_Absyn_Do_s_struct*)
_tmp534)->f2;_tmp552=_tmp551.f1;_LL34C: Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),
_tmp550);Cyc_Toc_exp_to_c(nv,_tmp552);return;_LL34D: if(_tmp534 <= (void*)1  || *((
int*)_tmp534)!= 14)goto _LL34F;_tmp553=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp534)->f1;_tmp554=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp534)->f2;_LL34E: {
struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=({struct Cyc_Core_Opt*
_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->v=(void*)e_typ;_tmp57F;});nv=Cyc_Toc_add_varmap(
nv,e_var,e_exp);Cyc_Toc_stmt_to_c(nv,_tmp553);{struct Cyc_Absyn_Stmt*_tmp580=Cyc_Absyn_seq_stmt(
_tmp553,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
struct _tuple1*_tmp581=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp582=Cyc_Absyn_var_exp(
_tmp581,0);struct Cyc_Absyn_Vardecl*_tmp583=Cyc_Absyn_new_vardecl(_tmp581,Cyc_Absyn_exn_typ,
0);_tmp582->topt=({struct Cyc_Core_Opt*_tmp584=_cycalloc(sizeof(*_tmp584));
_tmp584->v=(void*)Cyc_Absyn_exn_typ;_tmp584;});{struct Cyc_Absyn_Pat*_tmp585=({
struct Cyc_Absyn_Pat*_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595->r=(void*)((void*)({
struct Cyc_Absyn_Var_p_struct*_tmp597=_cycalloc(sizeof(*_tmp597));_tmp597[0]=({
struct Cyc_Absyn_Var_p_struct _tmp598;_tmp598.tag=0;_tmp598.f1=_tmp583;_tmp598;});
_tmp597;}));_tmp595->topt=({struct Cyc_Core_Opt*_tmp596=_cycalloc(sizeof(*_tmp596));
_tmp596->v=(void*)Cyc_Absyn_exn_typ;_tmp596;});_tmp595->loc=0;_tmp595;});struct
Cyc_Absyn_Exp*_tmp586=Cyc_Absyn_throw_exp(_tmp582,0);_tmp586->topt=({struct Cyc_Core_Opt*
_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587->v=(void*)((void*)0);_tmp587;});{
struct Cyc_Absyn_Stmt*_tmp588=Cyc_Absyn_exp_stmt(_tmp586,0);struct Cyc_Absyn_Switch_clause*
_tmp589=({struct Cyc_Absyn_Switch_clause*_tmp592=_cycalloc(sizeof(*_tmp592));
_tmp592->pattern=_tmp585;_tmp592->pat_vars=({struct Cyc_Core_Opt*_tmp593=
_cycalloc(sizeof(*_tmp593));_tmp593->v=({struct Cyc_List_List*_tmp594=_cycalloc(
sizeof(*_tmp594));_tmp594->hd=_tmp583;_tmp594->tl=0;_tmp594;});_tmp593;});
_tmp592->where_clause=0;_tmp592->body=_tmp588;_tmp592->loc=0;_tmp592;});struct
Cyc_Absyn_Stmt*_tmp58A=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp554,({struct
Cyc_List_List*_tmp591=_cycalloc(sizeof(*_tmp591));_tmp591->hd=_tmp589;_tmp591->tl=
0;_tmp591;})),0);Cyc_Toc_stmt_to_c(nv,_tmp58A);{struct Cyc_Absyn_Exp*_tmp58B=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp590=_cycalloc(sizeof(*_tmp590));
_tmp590->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp590->tl=0;
_tmp590;}),0);struct Cyc_Absyn_Stmt*_tmp58C=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp58F=_cycalloc(sizeof(*_tmp58F));
_tmp58F->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp58F->tl=0;_tmp58F;}),0),0);struct
Cyc_Absyn_Exp*_tmp58D=Cyc_Absyn_int_exp((void*)0,0,0);struct Cyc_Absyn_Exp*
_tmp58E=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp58C,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp58D,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp58B,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp58E,0),Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),
_tmp580,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Toc_cast_it(
e_typ,Cyc_Toc__exn_thrown_e),_tmp58A,0),0),0),0),0),0))->r));return;}}}}}_LL34F:
if(_tmp534 <= (void*)1  || *((int*)_tmp534)!= 15)goto _LL351;_tmp555=((struct Cyc_Absyn_Region_s_struct*)
_tmp534)->f1;_tmp556=((struct Cyc_Absyn_Region_s_struct*)_tmp534)->f2;_tmp557=((
struct Cyc_Absyn_Region_s_struct*)_tmp534)->f3;_tmp558=((struct Cyc_Absyn_Region_s_struct*)
_tmp534)->f4;_LL350: {void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct
_tuple1*rh_var=Cyc_Toc_temp_var();struct _tuple1*x_var=_tmp556->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(nv,x_var,x_exp),_tmp558);(void*)(s->r=(
void*)((void*)(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp599=
_cycalloc(sizeof(*_tmp599));_tmp599->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0);_tmp599->tl=0;_tmp599;}),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*
_tmp59A[1];_tmp59A[0]=x_exp;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp59A,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),Cyc_Absyn_seq_stmt(
_tmp558,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct
Cyc_Absyn_Exp*_tmp59B[1];_tmp59B[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp59B,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),0),0),0),0))->r));return;}_LL351: if(_tmp534 <= (void*)1  || *((int*)_tmp534)
!= 16)goto _LL32E;_tmp559=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp534)->f1;
_LL352: Cyc_Toc_exp_to_c(nv,_tmp559);(void*)(s->r=(void*)((void*)(Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp59C=
_cycalloc(sizeof(*_tmp59C));_tmp59C->hd=_tmp559;_tmp59C->tl=0;_tmp59C;}),0),0))->r));
return;_LL32E:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);struct
_tuple16{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){f->tvs=0;f->effect=0;
f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((void*)f->ret_type));{
struct Cyc_List_List*_tmp59D=f->args;for(0;_tmp59D != 0;_tmp59D=_tmp59D->tl){(*((
struct _tuple16*)_tmp59D->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple16*)_tmp59D->hd)).f3);{
struct _tuple1*_tmp59E=({struct _tuple1*_tmp59F=_cycalloc(sizeof(*_tmp59F));
_tmp59F->f1=(void*)((void*)0);_tmp59F->f2=(*((struct _tuple16*)_tmp59D->hd)).f1;
_tmp59F;});nv=Cyc_Toc_add_varmap(nv,_tmp59E,Cyc_Absyn_var_exp(_tmp59E,0));}}}if(
cinclude){Cyc_Toc_stmttypes_to_c(f->body);return;}if((unsigned int)f->cyc_varargs
 && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){struct
Cyc_Core_Opt*_tmp5A1;struct Cyc_Absyn_Tqual _tmp5A2;void*_tmp5A3;int _tmp5A4;struct
Cyc_Absyn_VarargInfo _tmp5A0=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));
_tmp5A1=_tmp5A0.name;_tmp5A2=_tmp5A0.tq;_tmp5A3=(void*)_tmp5A0.type;_tmp5A4=
_tmp5A0.inject;{void*_tmp5A5=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ(_tmp5A3,(void*)
2,_tmp5A2,Cyc_Absyn_false_conref));struct _tuple1*_tmp5A6=({struct _tuple1*_tmp5A9=
_cycalloc(sizeof(*_tmp5A9));_tmp5A9->f1=(void*)((void*)0);_tmp5A9->f2=(struct
_tagged_arr*)((struct Cyc_Core_Opt*)_check_null(_tmp5A1))->v;_tmp5A9;});f->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
f->args,({struct Cyc_List_List*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7->hd=({
struct _tuple16*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8->f1=(struct
_tagged_arr*)_tmp5A1->v;_tmp5A8->f2=_tmp5A2;_tmp5A8->f3=_tmp5A5;_tmp5A8;});
_tmp5A7->tl=0;_tmp5A7;}));nv=Cyc_Toc_add_varmap(nv,_tmp5A6,Cyc_Absyn_var_exp(
_tmp5A6,0));f->cyc_varargs=0;}}{struct Cyc_List_List*_tmp5AA=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp5AA != 0;_tmp5AA=
_tmp5AA->tl){(void*)(((struct Cyc_Absyn_Vardecl*)_tmp5AA->hd)->type=(void*)Cyc_Toc_typ_to_c((
void*)((struct Cyc_Absyn_Vardecl*)_tmp5AA->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(
nv),f->body);}static void*Cyc_Toc_scope_to_c(void*s){void*_tmp5AB=s;_LL362: if((
int)_tmp5AB != 1)goto _LL364;_LL363: return(void*)2;_LL364: if((int)_tmp5AB != 4)goto
_LL366;_LL365: return(void*)3;_LL366:;_LL367: return s;_LL361:;}struct Cyc_Core_Opt*
Cyc_Toc_aggrs_so_far=0;static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad){struct _tuple1*_tmp5AC=ad->name;if(Cyc_Toc_aggrs_so_far == 0)Cyc_Toc_aggrs_so_far=({
struct Cyc_Core_Opt*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));_tmp5AD->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp5AD;});{int seen_defn_before;struct Cyc_Core_Opt*_tmp5AE=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp5AC);if(_tmp5AE == 
0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(
Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp5AC,ad));}else{if(((
struct Cyc_Absyn_Aggrdecl*)_tmp5AE->v)->impl == 0){(struct Cyc_Dict_Dict*)(((struct
Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,
_tmp5AC,ad));seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(ad->sc=(void*)((
void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po=
0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*_tmp5AF=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp5AF != 0;_tmp5AF=_tmp5AF->tl){(void*)(((
struct Cyc_Absyn_Aggrfield*)_tmp5AF->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp5AF->hd)->type));}}}}}static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far=0;static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
tud){if(Cyc_Toc_tunions_so_far == 0)Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*
_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->v=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);_tmp5B0;});{
struct _tuple1*_tmp5B1=tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*
s,struct _tuple1*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_tunions_so_far))->v,_tmp5B1))return;(struct Cyc_Set_Set*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(void*)((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_insert)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp5B1));{struct Cyc_List_List*
_tmp5B2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;
for(0;_tmp5B2 != 0;_tmp5B2=_tmp5B2->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp5B2->hd;if(f->typs != 0){struct Cyc_List_List*_tmp5B3=0;int i=1;{struct Cyc_List_List*
_tmp5B4=f->typs;for(0;_tmp5B4 != 0;(_tmp5B4=_tmp5B4->tl,i ++)){struct _tagged_arr*
_tmp5B5=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*_tmp5B6=({struct Cyc_Absyn_Aggrfield*
_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->name=_tmp5B5;_tmp5B8->tq=(*((struct
_tuple4*)_tmp5B4->hd)).f1;_tmp5B8->type=(void*)Cyc_Toc_typ_to_c_array((*((struct
_tuple4*)_tmp5B4->hd)).f2);_tmp5B8->width=0;_tmp5B8->attributes=0;_tmp5B8;});
_tmp5B3=({struct Cyc_List_List*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->hd=
_tmp5B6;_tmp5B7->tl=_tmp5B3;_tmp5B7;});}}_tmp5B3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5B3);_tmp5B3=({struct Cyc_List_List*
_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9->hd=({struct Cyc_Absyn_Aggrfield*
_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->name=Cyc_Toc_tag_sp;_tmp5BA->tq=Cyc_Toc_mt_tq;
_tmp5BA->type=(void*)Cyc_Absyn_sint_typ;_tmp5BA->width=0;_tmp5BA->attributes=0;
_tmp5BA;});_tmp5B9->tl=_tmp5B3;_tmp5B9;});{struct Cyc_Absyn_Aggrdecl*_tmp5BB=({
struct Cyc_Absyn_Aggrdecl*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF->kind=(void*)((
void*)0);_tmp5BF->sc=(void*)((void*)2);_tmp5BF->name=Cyc_Toc_collapse_qvar_tag(f->name,({
const char*_tmp5C1="_struct";_tag_arr(_tmp5C1,sizeof(char),_get_zero_arr_size(
_tmp5C1,8));}));_tmp5BF->tvs=0;_tmp5BF->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0->exist_vars=0;_tmp5C0->rgn_po=0;
_tmp5C0->fields=_tmp5B3;_tmp5C0;});_tmp5BF->attributes=0;_tmp5BF;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));
_tmp5BD[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp5BE;_tmp5BE.tag=4;_tmp5BE.f1=
_tmp5BB;_tmp5BE;});_tmp5BD;}),0);_tmp5BC->tl=Cyc_Toc_result_decls;_tmp5BC;});}}}}}}
static struct Cyc_Core_Opt*Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far == 0)Cyc_Toc_xtunion_fields_so_far=({
struct Cyc_Core_Opt*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp5C2;});if(xd->fields == 0)return;{struct _tuple1*_tmp5C3=xd->name;struct Cyc_List_List*
_tmp5C4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp5C4 != 0;_tmp5C4=_tmp5C4->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp5C4->hd;struct _tagged_arr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp5C5=Cyc_Absyn_uint_exp(
_get_arr_size(*fn,sizeof(char))+ 4,0);void*_tmp5C6=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)_tmp5C5,Cyc_Absyn_false_conref,0);struct Cyc_Core_Opt*
_tmp5C7=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name);struct Cyc_Core_Opt _tmp5C8;int _tmp5C9;_LL369: if(_tmp5C7 != 0)goto _LL36B;
_LL36A: {struct Cyc_Absyn_Exp*initopt=0;if((void*)f->sc != (void*)3){char zero='\000';
initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((struct _tagged_arr)({struct
Cyc_String_pa_struct _tmp5D0;_tmp5D0.tag=0;_tmp5D0.f1=(struct _tagged_arr)((struct
_tagged_arr)*fn);{struct Cyc_Int_pa_struct _tmp5CF;_tmp5CF.tag=1;_tmp5CF.f1=(
unsigned int)((int)zero);{struct Cyc_Int_pa_struct _tmp5CE;_tmp5CE.tag=1;_tmp5CE.f1=(
unsigned int)((int)zero);{struct Cyc_Int_pa_struct _tmp5CD;_tmp5CD.tag=1;_tmp5CD.f1=(
unsigned int)((int)zero);{struct Cyc_Int_pa_struct _tmp5CC;_tmp5CC.tag=1;_tmp5CC.f1=(
unsigned int)((int)zero);{void*_tmp5CA[5]={& _tmp5CC,& _tmp5CD,& _tmp5CE,& _tmp5CF,&
_tmp5D0};Cyc_aprintf(({const char*_tmp5CB="%c%c%c%c%s";_tag_arr(_tmp5CB,sizeof(
char),_get_zero_arr_size(_tmp5CB,11));}),_tag_arr(_tmp5CA,sizeof(void*),5));}}}}}}),
0);}{struct Cyc_Absyn_Vardecl*_tmp5D1=Cyc_Absyn_new_vardecl(f->name,_tmp5C6,
initopt);(void*)(_tmp5D1->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3[
0]=({struct Cyc_Absyn_Var_d_struct _tmp5D4;_tmp5D4.tag=0;_tmp5D4.f1=_tmp5D1;
_tmp5D4;});_tmp5D3;}),0);_tmp5D2->tl=Cyc_Toc_result_decls;_tmp5D2;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,(void*)f->sc != (void*)3));if(f->typs != 0){struct Cyc_List_List*fields=0;
int i=1;{struct Cyc_List_List*_tmp5D5=f->typs;for(0;_tmp5D5 != 0;(_tmp5D5=_tmp5D5->tl,
i ++)){struct _tagged_arr*_tmp5D6=({struct _tagged_arr*_tmp5DA=_cycalloc(sizeof(*
_tmp5DA));_tmp5DA[0]=(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp5DD;_tmp5DD.tag=
1;_tmp5DD.f1=(unsigned int)i;{void*_tmp5DB[1]={& _tmp5DD};Cyc_aprintf(({const char*
_tmp5DC="f%d";_tag_arr(_tmp5DC,sizeof(char),_get_zero_arr_size(_tmp5DC,4));}),
_tag_arr(_tmp5DB,sizeof(void*),1));}});_tmp5DA;});struct Cyc_Absyn_Aggrfield*
_tmp5D7=({struct Cyc_Absyn_Aggrfield*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->name=
_tmp5D6;_tmp5D9->tq=(*((struct _tuple4*)_tmp5D5->hd)).f1;_tmp5D9->type=(void*)Cyc_Toc_typ_to_c_array((*((
struct _tuple4*)_tmp5D5->hd)).f2);_tmp5D9->width=0;_tmp5D9->attributes=0;_tmp5D9;});
fields=({struct Cyc_List_List*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->hd=
_tmp5D7;_tmp5D8->tl=fields;_tmp5D8;});}}fields=({struct Cyc_List_List*_tmp5DE=
_cycalloc(sizeof(*_tmp5DE));_tmp5DE->hd=({struct Cyc_Absyn_Aggrfield*_tmp5DF=
_cycalloc(sizeof(*_tmp5DF));_tmp5DF->name=Cyc_Toc_tag_sp;_tmp5DF->tq=Cyc_Toc_mt_tq;
_tmp5DF->type=(void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);
_tmp5DF->width=0;_tmp5DF->attributes=0;_tmp5DF;});_tmp5DE->tl=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp5DE;});{struct Cyc_Absyn_Aggrdecl*
_tmp5E0=({struct Cyc_Absyn_Aggrdecl*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->kind=(
void*)((void*)0);_tmp5E4->sc=(void*)((void*)2);_tmp5E4->name=Cyc_Toc_collapse_qvar_tag(
f->name,({const char*_tmp5E6="_struct";_tag_arr(_tmp5E6,sizeof(char),
_get_zero_arr_size(_tmp5E6,8));}));_tmp5E4->tvs=0;_tmp5E4->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp5E5=_cycalloc(sizeof(*_tmp5E5));_tmp5E5->exist_vars=0;_tmp5E5->rgn_po=0;
_tmp5E5->fields=fields;_tmp5E5;});_tmp5E4->attributes=0;_tmp5E4;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));
_tmp5E2[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp5E3;_tmp5E3.tag=4;_tmp5E3.f1=
_tmp5E0;_tmp5E3;});_tmp5E2;}),0);_tmp5E1->tl=Cyc_Toc_result_decls;_tmp5E1;});}}
goto _LL368;}}_LL36B: if(_tmp5C7 == 0)goto _LL36D;_tmp5C8=*_tmp5C7;_tmp5C9=(int)
_tmp5C8.v;if(_tmp5C9 != 0)goto _LL36D;_LL36C: if((void*)f->sc != (void*)3){char zero='\000';
struct Cyc_Absyn_Exp*_tmp5E7=Cyc_Absyn_string_exp((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp5F2;_tmp5F2.tag=0;_tmp5F2.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
struct Cyc_Int_pa_struct _tmp5F1;_tmp5F1.tag=1;_tmp5F1.f1=(unsigned int)((int)zero);{
struct Cyc_Int_pa_struct _tmp5F0;_tmp5F0.tag=1;_tmp5F0.f1=(unsigned int)((int)zero);{
struct Cyc_Int_pa_struct _tmp5EF;_tmp5EF.tag=1;_tmp5EF.f1=(unsigned int)((int)zero);{
struct Cyc_Int_pa_struct _tmp5EE;_tmp5EE.tag=1;_tmp5EE.f1=(unsigned int)((int)zero);{
void*_tmp5EC[5]={& _tmp5EE,& _tmp5EF,& _tmp5F0,& _tmp5F1,& _tmp5F2};Cyc_aprintf(({
const char*_tmp5ED="%c%c%c%c%s";_tag_arr(_tmp5ED,sizeof(char),_get_zero_arr_size(
_tmp5ED,11));}),_tag_arr(_tmp5EC,sizeof(void*),5));}}}}}}),0);struct Cyc_Absyn_Vardecl*
_tmp5E8=Cyc_Absyn_new_vardecl(f->name,_tmp5C6,(struct Cyc_Absyn_Exp*)_tmp5E7);(
void*)(_tmp5E8->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA[0]=({
struct Cyc_Absyn_Var_d_struct _tmp5EB;_tmp5EB.tag=0;_tmp5EB.f1=_tmp5E8;_tmp5EB;});
_tmp5EA;}),0);_tmp5E9->tl=Cyc_Toc_result_decls;_tmp5E9;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,1));}goto _LL368;_LL36D:;_LL36E: goto _LL368;_LL368:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));if((void*)vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp5F3=(void*)init->r;struct Cyc_Absyn_Vardecl*_tmp5F4;struct Cyc_Absyn_Exp*
_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F6;int _tmp5F7;_LL370: if(*((int*)_tmp5F3)!= 29)
goto _LL372;_tmp5F4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp5F3)->f1;
_tmp5F5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp5F3)->f2;_tmp5F6=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp5F3)->f3;_tmp5F7=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp5F3)->f4;_LL371: vd->initializer=0;(void*)(s->r=(void*)((void*)(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp5F4,_tmp5F5,_tmp5F6,_tmp5F7,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));goto _LL36F;_LL372:;_LL373: if((void*)vd->sc == (void*)0){
struct Cyc_Toc_Env*_tmp5F8=({struct Cyc_Toc_Env*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));
_tmp5F9[0]=*init_nv;_tmp5F9;});_tmp5F8->toplevel=1;Cyc_Toc_exp_to_c(_tmp5F8,init);}
else{Cyc_Toc_exp_to_c(init_nv,init);}goto _LL36F;_LL36F:;}else{void*_tmp5FA=Cyc_Tcutil_compress(
old_typ);struct Cyc_Absyn_ArrayInfo _tmp5FB;void*_tmp5FC;struct Cyc_Absyn_Exp*
_tmp5FD;struct Cyc_Absyn_Conref*_tmp5FE;_LL375: if(_tmp5FA <= (void*)3  || *((int*)
_tmp5FA)!= 7)goto _LL377;_tmp5FB=((struct Cyc_Absyn_ArrayType_struct*)_tmp5FA)->f1;
_tmp5FC=(void*)_tmp5FB.elt_type;_tmp5FD=_tmp5FB.num_elts;_tmp5FE=_tmp5FB.zero_term;
if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp5FE))goto
_LL377;_LL376: if(_tmp5FD == 0)({void*_tmp5FF[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp600="can't initialize zero-terminated array -- size unknown";
_tag_arr(_tmp600,sizeof(char),_get_zero_arr_size(_tmp600,55));}),_tag_arr(
_tmp5FF,sizeof(void*),0));});{struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)
_tmp5FD;struct Cyc_Absyn_Exp*_tmp601=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,
0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*
_tmp602=Cyc_Absyn_signed_int_exp(0,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp601,_tmp602,0),0),Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));goto _LL374;}_LL377:;_LL378: goto _LL374;_LL374:;}}static
struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=0;static struct Cyc_Absyn_Stmt*
Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt == 0)Cyc_Toc_throw_match_stmt_opt=({
struct Cyc_Absyn_Stmt**_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603[0]=Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(0)),0);_tmp603;});return*((struct
Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{struct _tuple1*x=Cyc_Toc_temp_var();{
struct _RegionHandle _tmp604=_new_region("prgn");struct _RegionHandle*prgn=& _tmp604;
_push_region(prgn);{struct Cyc_Toc_Env*_tmp606;struct Cyc_List_List*_tmp607;struct
Cyc_Absyn_Stmt*_tmp608;struct _tuple12 _tmp605=Cyc_Toc_xlate_pat(nv,prgn,t,Cyc_Absyn_var_exp(
x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),0);_tmp606=_tmp605.f1;
_tmp607=_tmp605.f2;_tmp608=_tmp605.f3;Cyc_Toc_stmt_to_c(_tmp606,s);s=Cyc_Absyn_declare_stmt(
x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp608,s,0),0);
for(0;_tmp607 != 0;_tmp607=_tmp607->tl){struct _tuple13 _tmp60A;struct _tuple1*
_tmp60B;void*_tmp60C;struct _tuple13*_tmp609=(struct _tuple13*)_tmp607->hd;_tmp60A=*
_tmp609;_tmp60B=_tmp60A.f1;_tmp60C=_tmp60A.f2;s=Cyc_Absyn_declare_stmt(_tmp60B,
_tmp60C,0,s,0);}};_pop_region(prgn);}return s;}}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e){void*_tmp60D=(void*)e->r;struct Cyc_Absyn_Exp*_tmp60E;
struct Cyc_Absyn_Exp*_tmp60F;struct Cyc_Absyn_Exp*_tmp610;struct Cyc_Absyn_Exp*
_tmp611;struct Cyc_Absyn_Exp*_tmp612;struct Cyc_Absyn_Exp*_tmp613;struct Cyc_Absyn_Exp*
_tmp614;struct Cyc_Absyn_Exp*_tmp615;struct Cyc_List_List*_tmp616;struct Cyc_Absyn_Exp*
_tmp617;struct Cyc_Absyn_Exp*_tmp618;struct Cyc_Absyn_Exp*_tmp619;struct Cyc_Absyn_Exp*
_tmp61A;struct Cyc_Absyn_Exp*_tmp61B;struct Cyc_Absyn_Exp*_tmp61C;struct Cyc_Absyn_Exp*
_tmp61D;struct Cyc_Absyn_Exp*_tmp61E;struct Cyc_Absyn_Exp*_tmp61F;struct Cyc_Absyn_Exp*
_tmp620;struct Cyc_Absyn_Exp*_tmp621;struct Cyc_Absyn_Exp*_tmp622;struct Cyc_Absyn_Exp*
_tmp623;struct Cyc_Absyn_Exp*_tmp624;struct Cyc_List_List*_tmp625;struct Cyc_Absyn_Exp*
_tmp626;struct Cyc_List_List*_tmp627;void*_tmp628;void**_tmp629;struct Cyc_Absyn_Exp*
_tmp62A;struct _tuple2*_tmp62B;struct _tuple2 _tmp62C;void*_tmp62D;void**_tmp62E;
struct Cyc_List_List*_tmp62F;struct Cyc_List_List*_tmp630;struct Cyc_List_List*
_tmp631;void*_tmp632;void**_tmp633;void*_tmp634;void**_tmp635;struct Cyc_Absyn_Stmt*
_tmp636;struct Cyc_Absyn_MallocInfo _tmp637;struct Cyc_Absyn_MallocInfo*_tmp638;
_LL37A: if(*((int*)_tmp60D)!= 22)goto _LL37C;_tmp60E=((struct Cyc_Absyn_Deref_e_struct*)
_tmp60D)->f1;_LL37B: _tmp60F=_tmp60E;goto _LL37D;_LL37C: if(*((int*)_tmp60D)!= 23)
goto _LL37E;_tmp60F=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp60D)->f1;_LL37D:
_tmp610=_tmp60F;goto _LL37F;_LL37E: if(*((int*)_tmp60D)!= 24)goto _LL380;_tmp610=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp60D)->f1;_LL37F: _tmp611=_tmp610;goto
_LL381;_LL380: if(*((int*)_tmp60D)!= 16)goto _LL382;_tmp611=((struct Cyc_Absyn_Address_e_struct*)
_tmp60D)->f1;_LL381: _tmp612=_tmp611;goto _LL383;_LL382: if(*((int*)_tmp60D)!= 12)
goto _LL384;_tmp612=((struct Cyc_Absyn_Throw_e_struct*)_tmp60D)->f1;_LL383: _tmp613=
_tmp612;goto _LL385;_LL384: if(*((int*)_tmp60D)!= 13)goto _LL386;_tmp613=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp60D)->f1;_LL385: _tmp614=_tmp613;goto _LL387;
_LL386: if(*((int*)_tmp60D)!= 19)goto _LL388;_tmp614=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp60D)->f1;_LL387: _tmp615=_tmp614;goto _LL389;_LL388: if(*((int*)_tmp60D)!= 5)
goto _LL38A;_tmp615=((struct Cyc_Absyn_Increment_e_struct*)_tmp60D)->f1;_LL389: Cyc_Toc_exptypes_to_c(
_tmp615);goto _LL379;_LL38A: if(*((int*)_tmp60D)!= 3)goto _LL38C;_tmp616=((struct
Cyc_Absyn_Primop_e_struct*)_tmp60D)->f2;_LL38B:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp616);goto _LL379;
_LL38C: if(*((int*)_tmp60D)!= 7)goto _LL38E;_tmp617=((struct Cyc_Absyn_And_e_struct*)
_tmp60D)->f1;_tmp618=((struct Cyc_Absyn_And_e_struct*)_tmp60D)->f2;_LL38D: _tmp619=
_tmp617;_tmp61A=_tmp618;goto _LL38F;_LL38E: if(*((int*)_tmp60D)!= 8)goto _LL390;
_tmp619=((struct Cyc_Absyn_Or_e_struct*)_tmp60D)->f1;_tmp61A=((struct Cyc_Absyn_Or_e_struct*)
_tmp60D)->f2;_LL38F: _tmp61B=_tmp619;_tmp61C=_tmp61A;goto _LL391;_LL390: if(*((int*)
_tmp60D)!= 9)goto _LL392;_tmp61B=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp60D)->f1;
_tmp61C=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp60D)->f2;_LL391: _tmp61D=_tmp61B;
_tmp61E=_tmp61C;goto _LL393;_LL392: if(*((int*)_tmp60D)!= 25)goto _LL394;_tmp61D=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp60D)->f1;_tmp61E=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp60D)->f2;_LL393: _tmp61F=_tmp61D;_tmp620=_tmp61E;goto _LL395;_LL394: if(*((int*)
_tmp60D)!= 4)goto _LL396;_tmp61F=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp60D)->f1;
_tmp620=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp60D)->f3;_LL395: Cyc_Toc_exptypes_to_c(
_tmp61F);Cyc_Toc_exptypes_to_c(_tmp620);goto _LL379;_LL396: if(*((int*)_tmp60D)!= 
6)goto _LL398;_tmp621=((struct Cyc_Absyn_Conditional_e_struct*)_tmp60D)->f1;
_tmp622=((struct Cyc_Absyn_Conditional_e_struct*)_tmp60D)->f2;_tmp623=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp60D)->f3;_LL397: Cyc_Toc_exptypes_to_c(_tmp621);Cyc_Toc_exptypes_to_c(_tmp622);
Cyc_Toc_exptypes_to_c(_tmp623);goto _LL379;_LL398: if(*((int*)_tmp60D)!= 11)goto
_LL39A;_tmp624=((struct Cyc_Absyn_FnCall_e_struct*)_tmp60D)->f1;_tmp625=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp60D)->f2;_LL399: _tmp626=_tmp624;_tmp627=_tmp625;
goto _LL39B;_LL39A: if(*((int*)_tmp60D)!= 10)goto _LL39C;_tmp626=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp60D)->f1;_tmp627=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp60D)->f2;_LL39B:
Cyc_Toc_exptypes_to_c(_tmp626);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp627);goto _LL379;_LL39C:
if(*((int*)_tmp60D)!= 15)goto _LL39E;_tmp628=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp60D)->f1;_tmp629=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp60D)->f1);
_tmp62A=((struct Cyc_Absyn_Cast_e_struct*)_tmp60D)->f2;_LL39D:*_tmp629=Cyc_Toc_typ_to_c(*
_tmp629);Cyc_Toc_exptypes_to_c(_tmp62A);goto _LL379;_LL39E: if(*((int*)_tmp60D)!= 
27)goto _LL3A0;_tmp62B=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp60D)->f1;
_tmp62C=*_tmp62B;_tmp62D=_tmp62C.f3;_tmp62E=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp60D)->f1).f3;_tmp62F=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp60D)->f2;
_LL39F:*_tmp62E=Cyc_Toc_typ_to_c(*_tmp62E);_tmp630=_tmp62F;goto _LL3A1;_LL3A0: if(*((
int*)_tmp60D)!= 36)goto _LL3A2;_tmp630=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp60D)->f2;_LL3A1: _tmp631=_tmp630;goto _LL3A3;_LL3A2: if(*((int*)_tmp60D)!= 28)
goto _LL3A4;_tmp631=((struct Cyc_Absyn_Array_e_struct*)_tmp60D)->f1;_LL3A3: for(0;
_tmp631 != 0;_tmp631=_tmp631->tl){struct Cyc_Absyn_Exp*_tmp63A;struct _tuple6
_tmp639=*((struct _tuple6*)_tmp631->hd);_tmp63A=_tmp639.f2;Cyc_Toc_exptypes_to_c(
_tmp63A);}goto _LL379;_LL3A4: if(*((int*)_tmp60D)!= 20)goto _LL3A6;_tmp632=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp60D)->f1;_tmp633=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp60D)->f1);_LL3A5: _tmp635=_tmp633;goto
_LL3A7;_LL3A6: if(*((int*)_tmp60D)!= 18)goto _LL3A8;_tmp634=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp60D)->f1;_tmp635=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp60D)->f1);_LL3A7:*_tmp635=Cyc_Toc_typ_to_c(*_tmp635);goto _LL379;_LL3A8: if(*((
int*)_tmp60D)!= 37)goto _LL3AA;_tmp636=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp60D)->f1;_LL3A9: Cyc_Toc_stmttypes_to_c(_tmp636);goto _LL379;_LL3AA: if(*((int*)
_tmp60D)!= 35)goto _LL3AC;_tmp637=((struct Cyc_Absyn_Malloc_e_struct*)_tmp60D)->f1;
_tmp638=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp60D)->f1;
_LL3AB: if(_tmp638->elt_type != 0)_tmp638->elt_type=({void**_tmp63B=_cycalloc(
sizeof(*_tmp63B));_tmp63B[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp638->elt_type)));
_tmp63B;});Cyc_Toc_exptypes_to_c(_tmp638->num_elts);goto _LL379;_LL3AC: if(*((int*)
_tmp60D)!= 0)goto _LL3AE;_LL3AD: goto _LL3AF;_LL3AE: if(*((int*)_tmp60D)!= 1)goto
_LL3B0;_LL3AF: goto _LL3B1;_LL3B0: if(*((int*)_tmp60D)!= 2)goto _LL3B2;_LL3B1: goto
_LL3B3;_LL3B2: if(*((int*)_tmp60D)!= 33)goto _LL3B4;_LL3B3: goto _LL3B5;_LL3B4: if(*((
int*)_tmp60D)!= 34)goto _LL3B6;_LL3B5: goto _LL379;_LL3B6: if(*((int*)_tmp60D)!= 31)
goto _LL3B8;_LL3B7: goto _LL3B9;_LL3B8: if(*((int*)_tmp60D)!= 32)goto _LL3BA;_LL3B9:
goto _LL3BB;_LL3BA: if(*((int*)_tmp60D)!= 30)goto _LL3BC;_LL3BB: goto _LL3BD;_LL3BC:
if(*((int*)_tmp60D)!= 29)goto _LL3BE;_LL3BD: goto _LL3BF;_LL3BE: if(*((int*)_tmp60D)
!= 26)goto _LL3C0;_LL3BF: goto _LL3C1;_LL3C0: if(*((int*)_tmp60D)!= 14)goto _LL3C2;
_LL3C1: goto _LL3C3;_LL3C2: if(*((int*)_tmp60D)!= 17)goto _LL3C4;_LL3C3: goto _LL3C5;
_LL3C4: if(*((int*)_tmp60D)!= 21)goto _LL379;_LL3C5:({void*_tmp63C[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp63D="Cyclone expression within C code";_tag_arr(_tmp63D,
sizeof(char),_get_zero_arr_size(_tmp63D,33));}),_tag_arr(_tmp63C,sizeof(void*),0));});
goto _LL379;_LL379:;}static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp63E=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp63F;struct Cyc_Absyn_Fndecl*
_tmp640;struct Cyc_Absyn_Aggrdecl*_tmp641;struct Cyc_Absyn_Enumdecl*_tmp642;struct
Cyc_Absyn_Typedefdecl*_tmp643;_LL3C7: if(_tmp63E <= (void*)2  || *((int*)_tmp63E)!= 
0)goto _LL3C9;_tmp63F=((struct Cyc_Absyn_Var_d_struct*)_tmp63E)->f1;_LL3C8:(void*)(
_tmp63F->type=(void*)Cyc_Toc_typ_to_c((void*)_tmp63F->type));if(_tmp63F->initializer
!= 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp63F->initializer));
goto _LL3C6;_LL3C9: if(_tmp63E <= (void*)2  || *((int*)_tmp63E)!= 1)goto _LL3CB;
_tmp640=((struct Cyc_Absyn_Fn_d_struct*)_tmp63E)->f1;_LL3CA:(void*)(_tmp640->ret_type=(
void*)Cyc_Toc_typ_to_c((void*)_tmp640->ret_type));{struct Cyc_List_List*_tmp644=
_tmp640->args;for(0;_tmp644 != 0;_tmp644=_tmp644->tl){(*((struct _tuple16*)_tmp644->hd)).f3=
Cyc_Toc_typ_to_c((*((struct _tuple16*)_tmp644->hd)).f3);}}goto _LL3C6;_LL3CB: if(
_tmp63E <= (void*)2  || *((int*)_tmp63E)!= 4)goto _LL3CD;_tmp641=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp63E)->f1;_LL3CC: Cyc_Toc_aggrdecl_to_c(_tmp641);goto _LL3C6;_LL3CD: if(_tmp63E
<= (void*)2  || *((int*)_tmp63E)!= 6)goto _LL3CF;_tmp642=((struct Cyc_Absyn_Enum_d_struct*)
_tmp63E)->f1;_LL3CE: if(_tmp642->fields != 0){struct Cyc_List_List*_tmp645=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp642->fields))->v;for(0;
_tmp645 != 0;_tmp645=_tmp645->tl){struct Cyc_Absyn_Enumfield*_tmp646=(struct Cyc_Absyn_Enumfield*)
_tmp645->hd;if(_tmp646->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp646->tag));}}goto _LL3C6;_LL3CF: if(_tmp63E <= (void*)2  || *((int*)
_tmp63E)!= 7)goto _LL3D1;_tmp643=((struct Cyc_Absyn_Typedef_d_struct*)_tmp63E)->f1;
_LL3D0: _tmp643->defn=({struct Cyc_Core_Opt*_tmp647=_cycalloc(sizeof(*_tmp647));
_tmp647->v=(void*)Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp643->defn))->v);_tmp647;});goto _LL3C6;_LL3D1: if(_tmp63E <= (void*)
2  || *((int*)_tmp63E)!= 2)goto _LL3D3;_LL3D2: goto _LL3D4;_LL3D3: if(_tmp63E <= (void*)
2  || *((int*)_tmp63E)!= 3)goto _LL3D5;_LL3D4: goto _LL3D6;_LL3D5: if(_tmp63E <= (void*)
2  || *((int*)_tmp63E)!= 5)goto _LL3D7;_LL3D6: goto _LL3D8;_LL3D7: if(_tmp63E <= (void*)
2  || *((int*)_tmp63E)!= 8)goto _LL3D9;_LL3D8: goto _LL3DA;_LL3D9: if(_tmp63E <= (void*)
2  || *((int*)_tmp63E)!= 9)goto _LL3DB;_LL3DA: goto _LL3DC;_LL3DB: if(_tmp63E <= (void*)
2  || *((int*)_tmp63E)!= 10)goto _LL3DD;_LL3DC: goto _LL3DE;_LL3DD: if(_tmp63E <= (
void*)2  || *((int*)_tmp63E)!= 11)goto _LL3DF;_LL3DE:({void*_tmp648[0]={};Cyc_Tcutil_terr(
d->loc,({const char*_tmp649="Cyclone declaration within C code";_tag_arr(_tmp649,
sizeof(char),_get_zero_arr_size(_tmp649,34));}),_tag_arr(_tmp648,sizeof(void*),0));});
goto _LL3C6;_LL3DF: if((int)_tmp63E != 0)goto _LL3E1;_LL3E0: goto _LL3E2;_LL3E1: if((
int)_tmp63E != 1)goto _LL3C6;_LL3E2: goto _LL3C6;_LL3C6:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s){void*_tmp64A=(void*)s->r;struct Cyc_Absyn_Exp*_tmp64B;
struct Cyc_Absyn_Stmt*_tmp64C;struct Cyc_Absyn_Stmt*_tmp64D;struct Cyc_Absyn_Exp*
_tmp64E;struct Cyc_Absyn_Exp*_tmp64F;struct Cyc_Absyn_Stmt*_tmp650;struct Cyc_Absyn_Stmt*
_tmp651;struct _tuple3 _tmp652;struct Cyc_Absyn_Exp*_tmp653;struct Cyc_Absyn_Stmt*
_tmp654;struct Cyc_Absyn_Exp*_tmp655;struct _tuple3 _tmp656;struct Cyc_Absyn_Exp*
_tmp657;struct _tuple3 _tmp658;struct Cyc_Absyn_Exp*_tmp659;struct Cyc_Absyn_Stmt*
_tmp65A;struct Cyc_Absyn_Exp*_tmp65B;struct Cyc_List_List*_tmp65C;struct Cyc_Absyn_Decl*
_tmp65D;struct Cyc_Absyn_Stmt*_tmp65E;struct Cyc_Absyn_Stmt*_tmp65F;struct _tuple3
_tmp660;struct Cyc_Absyn_Exp*_tmp661;_LL3E4: if(_tmp64A <= (void*)1  || *((int*)
_tmp64A)!= 0)goto _LL3E6;_tmp64B=((struct Cyc_Absyn_Exp_s_struct*)_tmp64A)->f1;
_LL3E5: Cyc_Toc_exptypes_to_c(_tmp64B);goto _LL3E3;_LL3E6: if(_tmp64A <= (void*)1
 || *((int*)_tmp64A)!= 1)goto _LL3E8;_tmp64C=((struct Cyc_Absyn_Seq_s_struct*)
_tmp64A)->f1;_tmp64D=((struct Cyc_Absyn_Seq_s_struct*)_tmp64A)->f2;_LL3E7: Cyc_Toc_stmttypes_to_c(
_tmp64C);Cyc_Toc_stmttypes_to_c(_tmp64D);goto _LL3E3;_LL3E8: if(_tmp64A <= (void*)1
 || *((int*)_tmp64A)!= 2)goto _LL3EA;_tmp64E=((struct Cyc_Absyn_Return_s_struct*)
_tmp64A)->f1;_LL3E9: if(_tmp64E != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_tmp64E);goto _LL3E3;_LL3EA: if(_tmp64A <= (void*)1  || *((int*)_tmp64A)!= 3)goto
_LL3EC;_tmp64F=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp64A)->f1;_tmp650=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp64A)->f2;_tmp651=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp64A)->f3;_LL3EB: Cyc_Toc_exptypes_to_c(_tmp64F);Cyc_Toc_stmttypes_to_c(
_tmp650);Cyc_Toc_stmttypes_to_c(_tmp651);goto _LL3E3;_LL3EC: if(_tmp64A <= (void*)1
 || *((int*)_tmp64A)!= 4)goto _LL3EE;_tmp652=((struct Cyc_Absyn_While_s_struct*)
_tmp64A)->f1;_tmp653=_tmp652.f1;_tmp654=((struct Cyc_Absyn_While_s_struct*)
_tmp64A)->f2;_LL3ED: Cyc_Toc_exptypes_to_c(_tmp653);Cyc_Toc_stmttypes_to_c(
_tmp654);goto _LL3E3;_LL3EE: if(_tmp64A <= (void*)1  || *((int*)_tmp64A)!= 8)goto
_LL3F0;_tmp655=((struct Cyc_Absyn_For_s_struct*)_tmp64A)->f1;_tmp656=((struct Cyc_Absyn_For_s_struct*)
_tmp64A)->f2;_tmp657=_tmp656.f1;_tmp658=((struct Cyc_Absyn_For_s_struct*)_tmp64A)->f3;
_tmp659=_tmp658.f1;_tmp65A=((struct Cyc_Absyn_For_s_struct*)_tmp64A)->f4;_LL3EF:
Cyc_Toc_exptypes_to_c(_tmp655);Cyc_Toc_exptypes_to_c(_tmp657);Cyc_Toc_exptypes_to_c(
_tmp659);Cyc_Toc_stmttypes_to_c(_tmp65A);goto _LL3E3;_LL3F0: if(_tmp64A <= (void*)1
 || *((int*)_tmp64A)!= 9)goto _LL3F2;_tmp65B=((struct Cyc_Absyn_Switch_s_struct*)
_tmp64A)->f1;_tmp65C=((struct Cyc_Absyn_Switch_s_struct*)_tmp64A)->f2;_LL3F1: Cyc_Toc_exptypes_to_c(
_tmp65B);for(0;_tmp65C != 0;_tmp65C=_tmp65C->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp65C->hd)->body);}goto _LL3E3;_LL3F2: if(_tmp64A <= (
void*)1  || *((int*)_tmp64A)!= 11)goto _LL3F4;_tmp65D=((struct Cyc_Absyn_Decl_s_struct*)
_tmp64A)->f1;_tmp65E=((struct Cyc_Absyn_Decl_s_struct*)_tmp64A)->f2;_LL3F3: Cyc_Toc_decltypes_to_c(
_tmp65D);Cyc_Toc_stmttypes_to_c(_tmp65E);goto _LL3E3;_LL3F4: if(_tmp64A <= (void*)1
 || *((int*)_tmp64A)!= 13)goto _LL3F6;_tmp65F=((struct Cyc_Absyn_Do_s_struct*)
_tmp64A)->f1;_tmp660=((struct Cyc_Absyn_Do_s_struct*)_tmp64A)->f2;_tmp661=_tmp660.f1;
_LL3F5: Cyc_Toc_stmttypes_to_c(_tmp65F);Cyc_Toc_exptypes_to_c(_tmp661);goto _LL3E3;
_LL3F6: if((int)_tmp64A != 0)goto _LL3F8;_LL3F7: goto _LL3F9;_LL3F8: if(_tmp64A <= (
void*)1  || *((int*)_tmp64A)!= 5)goto _LL3FA;_LL3F9: goto _LL3FB;_LL3FA: if(_tmp64A <= (
void*)1  || *((int*)_tmp64A)!= 6)goto _LL3FC;_LL3FB: goto _LL3FD;_LL3FC: if(_tmp64A <= (
void*)1  || *((int*)_tmp64A)!= 7)goto _LL3FE;_LL3FD: goto _LL3E3;_LL3FE: if(_tmp64A <= (
void*)1  || *((int*)_tmp64A)!= 10)goto _LL400;_LL3FF: goto _LL401;_LL400: if(_tmp64A
<= (void*)1  || *((int*)_tmp64A)!= 12)goto _LL402;_LL401: goto _LL403;_LL402: if(
_tmp64A <= (void*)1  || *((int*)_tmp64A)!= 14)goto _LL404;_LL403: goto _LL405;_LL404:
if(_tmp64A <= (void*)1  || *((int*)_tmp64A)!= 15)goto _LL406;_LL405: goto _LL407;
_LL406: if(_tmp64A <= (void*)1  || *((int*)_tmp64A)!= 16)goto _LL3E3;_LL407:({void*
_tmp662[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp663="Cyclone statement in C code";
_tag_arr(_tmp663,sizeof(char),_get_zero_arr_size(_tmp663,28));}),_tag_arr(
_tmp662,sizeof(void*),0));});goto _LL3E3;_LL3E3:;}static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){for(0;ds != 0;ds=
ds->tl){if(!nv->toplevel)({void*_tmp664[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp665="decls_to_c: not at toplevel!";
_tag_arr(_tmp665,sizeof(char),_get_zero_arr_size(_tmp665,29));}),_tag_arr(
_tmp664,sizeof(void*),0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp666=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp667;struct Cyc_Absyn_Fndecl*
_tmp668;struct Cyc_Absyn_Aggrdecl*_tmp669;struct Cyc_Absyn_Tuniondecl*_tmp66A;
struct Cyc_Absyn_Enumdecl*_tmp66B;struct Cyc_Absyn_Typedefdecl*_tmp66C;struct Cyc_List_List*
_tmp66D;struct Cyc_List_List*_tmp66E;struct Cyc_List_List*_tmp66F;struct Cyc_List_List*
_tmp670;_LL409: if(_tmp666 <= (void*)2  || *((int*)_tmp666)!= 0)goto _LL40B;_tmp667=((
struct Cyc_Absyn_Var_d_struct*)_tmp666)->f1;_LL40A: {struct _tuple1*_tmp671=
_tmp667->name;if((void*)_tmp667->sc == (void*)4)_tmp671=({struct _tuple1*_tmp672=
_cycalloc(sizeof(*_tmp672));_tmp672->f1=Cyc_Absyn_rel_ns_null;_tmp672->f2=(*
_tmp671).f2;_tmp672;});if(_tmp667->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp667->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmp667->initializer));}}nv=Cyc_Toc_add_varmap(
nv,_tmp667->name,Cyc_Absyn_varb_exp(_tmp671,(void*)({struct Cyc_Absyn_Global_b_struct*
_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673[0]=({struct Cyc_Absyn_Global_b_struct
_tmp674;_tmp674.tag=0;_tmp674.f1=_tmp667;_tmp674;});_tmp673;}),0));_tmp667->name=
_tmp671;(void*)(_tmp667->sc=(void*)Cyc_Toc_scope_to_c((void*)_tmp667->sc));(void*)(
_tmp667->type=(void*)Cyc_Toc_typ_to_c_array((void*)_tmp667->type));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675->hd=d;_tmp675->tl=
Cyc_Toc_result_decls;_tmp675;});goto _LL408;}_LL40B: if(_tmp666 <= (void*)2  || *((
int*)_tmp666)!= 1)goto _LL40D;_tmp668=((struct Cyc_Absyn_Fn_d_struct*)_tmp666)->f1;
_LL40C: {struct _tuple1*_tmp676=_tmp668->name;if((void*)_tmp668->sc == (void*)4){
_tmp676=({struct _tuple1*_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677->f1=Cyc_Absyn_rel_ns_null;
_tmp677->f2=(*_tmp676).f2;_tmp677;});(void*)(_tmp668->sc=(void*)((void*)2));}nv=
Cyc_Toc_add_varmap(nv,_tmp668->name,Cyc_Absyn_var_exp(_tmp676,0));_tmp668->name=
_tmp676;Cyc_Toc_fndecl_to_c(nv,_tmp668,cinclude);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678->hd=d;_tmp678->tl=Cyc_Toc_result_decls;
_tmp678;});goto _LL408;}_LL40D: if(_tmp666 <= (void*)2  || *((int*)_tmp666)!= 2)goto
_LL40F;_LL40E: goto _LL410;_LL40F: if(_tmp666 <= (void*)2  || *((int*)_tmp666)!= 3)
goto _LL411;_LL410:({void*_tmp679[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp67A="letdecl at toplevel";
_tag_arr(_tmp67A,sizeof(char),_get_zero_arr_size(_tmp67A,20));}),_tag_arr(
_tmp679,sizeof(void*),0));});_LL411: if(_tmp666 <= (void*)2  || *((int*)_tmp666)!= 
4)goto _LL413;_tmp669=((struct Cyc_Absyn_Aggr_d_struct*)_tmp666)->f1;_LL412: Cyc_Toc_aggrdecl_to_c(
_tmp669);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp67B=_cycalloc(sizeof(*
_tmp67B));_tmp67B->hd=d;_tmp67B->tl=Cyc_Toc_result_decls;_tmp67B;});goto _LL408;
_LL413: if(_tmp666 <= (void*)2  || *((int*)_tmp666)!= 5)goto _LL415;_tmp66A=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp666)->f1;_LL414: if(_tmp66A->is_xtunion)Cyc_Toc_xtuniondecl_to_c(
_tmp66A);else{Cyc_Toc_tuniondecl_to_c(_tmp66A);}goto _LL408;_LL415: if(_tmp666 <= (
void*)2  || *((int*)_tmp666)!= 6)goto _LL417;_tmp66B=((struct Cyc_Absyn_Enum_d_struct*)
_tmp666)->f1;_LL416: Cyc_Toc_enumdecl_to_c(nv,_tmp66B);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C->hd=d;_tmp67C->tl=
Cyc_Toc_result_decls;_tmp67C;});goto _LL408;_LL417: if(_tmp666 <= (void*)2  || *((
int*)_tmp666)!= 7)goto _LL419;_tmp66C=((struct Cyc_Absyn_Typedef_d_struct*)_tmp666)->f1;
_LL418: _tmp66C->name=_tmp66C->name;_tmp66C->tvs=0;if(_tmp66C->defn != 0)_tmp66C->defn=({
struct Cyc_Core_Opt*_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp66C->defn))->v);_tmp67D;});else{void*
_tmp67E=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp66C->kind))->v;_LL426: if((
int)_tmp67E != 2)goto _LL428;_LL427: _tmp66C->defn=({struct Cyc_Core_Opt*_tmp67F=
_cycalloc(sizeof(*_tmp67F));_tmp67F->v=(void*)Cyc_Absyn_void_star_typ();_tmp67F;});
goto _LL425;_LL428:;_LL429: _tmp66C->defn=({struct Cyc_Core_Opt*_tmp680=_cycalloc(
sizeof(*_tmp680));_tmp680->v=(void*)((void*)0);_tmp680;});goto _LL425;_LL425:;}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp681=_cycalloc(sizeof(*_tmp681));
_tmp681->hd=d;_tmp681->tl=Cyc_Toc_result_decls;_tmp681;});goto _LL408;_LL419: if((
int)_tmp666 != 0)goto _LL41B;_LL41A: goto _LL41C;_LL41B: if((int)_tmp666 != 1)goto
_LL41D;_LL41C: goto _LL408;_LL41D: if(_tmp666 <= (void*)2  || *((int*)_tmp666)!= 8)
goto _LL41F;_tmp66D=((struct Cyc_Absyn_Namespace_d_struct*)_tmp666)->f2;_LL41E:
_tmp66E=_tmp66D;goto _LL420;_LL41F: if(_tmp666 <= (void*)2  || *((int*)_tmp666)!= 9)
goto _LL421;_tmp66E=((struct Cyc_Absyn_Using_d_struct*)_tmp666)->f2;_LL420: _tmp66F=
_tmp66E;goto _LL422;_LL421: if(_tmp666 <= (void*)2  || *((int*)_tmp666)!= 10)goto
_LL423;_tmp66F=((struct Cyc_Absyn_ExternC_d_struct*)_tmp666)->f1;_LL422: nv=Cyc_Toc_decls_to_c(
nv,_tmp66F,top,cinclude);goto _LL408;_LL423: if(_tmp666 <= (void*)2  || *((int*)
_tmp666)!= 11)goto _LL408;_tmp670=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp666)->f1;_LL424: nv=Cyc_Toc_decls_to_c(nv,_tmp670,top,1);goto _LL408;_LL408:;}}
return nv;}static void Cyc_Toc_init(){Cyc_Toc_result_decls=0;Cyc_Toc_tuple_types=0;
Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_aggrs_so_far=0;Cyc_Toc_tunions_so_far=0;Cyc_Toc_xtunion_fields_so_far=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();Cyc_Toc_decls_to_c(Cyc_Toc_empty_env(),
ds,1,0);return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}
