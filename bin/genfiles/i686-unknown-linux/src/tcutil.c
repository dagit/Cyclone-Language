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
 struct Cyc_Core_Opt{void*v;};int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
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
tag;struct _tagged_arr f1;};int Cyc_printf(struct _tagged_arr,struct _tagged_arr);
struct _tagged_arr Cyc_vrprintf(struct _RegionHandle*,struct _tagged_arr,struct
_tagged_arr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];
struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_map2(void*(*f)(
void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);void Cyc_List_iter(void(*
f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_revappend(struct
Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct
_tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_List_List*Cyc_Position_strings_of_segments(
struct Cyc_List_List*);struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*
seg;void*kind;struct _tagged_arr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*,struct _tagged_arr);extern char Cyc_Position_Nocontext[
14];extern int Cyc_Position_num_errors;extern int Cyc_Position_max_errors;void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple1{
void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;
void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*f1;
};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(struct
_tuple1*,struct _tuple1*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple1*Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple1*Cyc_Absyn_tunion_scanf_arg_qvar;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct
Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct
_tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,
struct _tagged_arr*);struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};struct
_tuple4*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);struct _tagged_arr
Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(void*a);struct _tuple5{
void*f1;struct _tuple1*f2;};struct _tuple5 Cyc_Absyn_aggr_kinded_name(void*);struct
Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct _tagged_arr Cyc_Absynpp_kind2string(void*);struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*);struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern
char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);struct _tuple6{void*f1;void*f2;};struct _tuple6*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple6*Cyc_Dict_rchoose(struct
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
struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**
Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);unsigned int Cyc_strlen(struct _tagged_arr s);
int Cyc_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);int Cyc_strptrcmp(struct
_tagged_arr*s1,struct _tagged_arr*s2);struct _tagged_arr Cyc_strconcat(struct
_tagged_arr,struct _tagged_arr);struct _tuple7{unsigned int f1;int f2;};struct
_tuple7 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr
fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_flush_warnings();extern struct
Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*t);int Cyc_Tcutil_kind_leq(
void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(
void*t);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*,void*);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,
struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int
Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);void*
Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();int
Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple8{struct Cyc_Absyn_Tvar*
f1;void*f2;};struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct
Cyc_Absyn_Tvar*);struct _tuple9{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _tagged_arr*fn);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*,int allow_unique);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void
Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct
Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr err_msg);
void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,struct Cyc_List_List*);
void Cyc_Tcutil_check_no_unique_region(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*t);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,struct
Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,
unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*
b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,void*t,struct
Cyc_Absyn_Exp*e);struct _tuple10{int f1;void*f2;};struct _tuple10 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_normalize_effect(
void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int*Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_snd_tqt(struct _tuple4*);int Cyc_Tcutil_supports_default(
void*);int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);void*Cyc_Tcexp_tcExp(struct
Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Enumdecl*);char Cyc_Tcutil_Unify[
10]="\000\000\000\000Unify\000";void Cyc_Tcutil_unify_it(void*t1,void*t2);void*
Cyc_Tcutil_t1_failure=(void*)0;int Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(
void*)0;int Cyc_Tcutil_tq2_const=0;struct _tagged_arr Cyc_Tcutil_failure_reason=(
struct _tagged_arr){(void*)0,(void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{struct _tagged_arr s1=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp22;_tmp22.tag=0;_tmp22.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure));{struct Cyc_String_pa_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(
struct _tagged_arr)(Cyc_Tcutil_tq1_const?({const char*_tmp23="const ";_tag_arr(
_tmp23,sizeof(char),_get_zero_arr_size(_tmp23,7));}):({const char*_tmp24="";
_tag_arr(_tmp24,sizeof(char),_get_zero_arr_size(_tmp24,1));}));{void*_tmp1F[2]={&
_tmp21,& _tmp22};Cyc_aprintf(({const char*_tmp20="%s%s";_tag_arr(_tmp20,sizeof(
char),_get_zero_arr_size(_tmp20,5));}),_tag_arr(_tmp1F,sizeof(void*),2));}}});
struct _tagged_arr s2=(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp1C;_tmp1C.tag=
0;_tmp1C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure));{
struct Cyc_String_pa_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _tagged_arr)(Cyc_Tcutil_tq2_const?({
const char*_tmp1D="const ";_tag_arr(_tmp1D,sizeof(char),_get_zero_arr_size(_tmp1D,
7));}):({const char*_tmp1E="";_tag_arr(_tmp1E,sizeof(char),_get_zero_arr_size(
_tmp1E,1));}));{void*_tmp19[2]={& _tmp1B,& _tmp1C};Cyc_aprintf(({const char*_tmp1A="%s%s";
_tag_arr(_tmp1A,sizeof(char),_get_zero_arr_size(_tmp1A,5));}),_tag_arr(_tmp19,
sizeof(void*),2));}}});int pos=2;({struct Cyc_String_pa_struct _tmp2;_tmp2.tag=0;
_tmp2.f1=(struct _tagged_arr)((struct _tagged_arr)s1);{void*_tmp0[1]={& _tmp2};Cyc_fprintf(
Cyc_stderr,({const char*_tmp1="  %s";_tag_arr(_tmp1,sizeof(char),
_get_zero_arr_size(_tmp1,5));}),_tag_arr(_tmp0,sizeof(void*),1));}});pos +=
_get_arr_size(s1,sizeof(char));if(pos + 5 >= 80){({void*_tmp3[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp4="\n\t";_tag_arr(_tmp4,sizeof(char),
_get_zero_arr_size(_tmp4,3));}),_tag_arr(_tmp3,sizeof(void*),0));});pos=8;}else{({
void*_tmp5[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp6=" ";_tag_arr(_tmp6,
sizeof(char),_get_zero_arr_size(_tmp6,2));}),_tag_arr(_tmp5,sizeof(void*),0));});
pos ++;}({void*_tmp7[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp8="and ";
_tag_arr(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,5));}),_tag_arr(_tmp7,
sizeof(void*),0));});pos +=4;if(pos + _get_arr_size(s2,sizeof(char))>= 80){({void*
_tmp9[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpA="\n\t";_tag_arr(_tmpA,
sizeof(char),_get_zero_arr_size(_tmpA,3));}),_tag_arr(_tmp9,sizeof(void*),0));});
pos=8;}({struct Cyc_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(struct _tagged_arr)((
struct _tagged_arr)s2);{void*_tmpB[1]={& _tmpD};Cyc_fprintf(Cyc_stderr,({const char*
_tmpC="%s ";_tag_arr(_tmpC,sizeof(char),_get_zero_arr_size(_tmpC,4));}),_tag_arr(
_tmpB,sizeof(void*),1));}});pos +=_get_arr_size(s2,sizeof(char))+ 1;if(pos + 17 >= 
80){({void*_tmpE[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpF="\n\t";_tag_arr(
_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,3));}),_tag_arr(_tmpE,sizeof(void*),
0));});pos=8;}({void*_tmp10[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp11="are not compatible. ";
_tag_arr(_tmp11,sizeof(char),_get_zero_arr_size(_tmp11,21));}),_tag_arr(_tmp10,
sizeof(void*),0));});pos +=17;if(Cyc_Tcutil_failure_reason.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr){if(pos + Cyc_strlen((struct _tagged_arr)Cyc_Tcutil_failure_reason)
>= 80)({void*_tmp12[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp13="\n\t";
_tag_arr(_tmp13,sizeof(char),_get_zero_arr_size(_tmp13,3));}),_tag_arr(_tmp12,
sizeof(void*),0));});({struct Cyc_String_pa_struct _tmp16;_tmp16.tag=0;_tmp16.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_failure_reason);{void*_tmp14[1]={&
_tmp16};Cyc_fprintf(Cyc_stderr,({const char*_tmp15="%s";_tag_arr(_tmp15,sizeof(
char),_get_zero_arr_size(_tmp15,3));}),_tag_arr(_tmp14,sizeof(void*),1));}});}({
void*_tmp17[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp18="\n";_tag_arr(_tmp18,
sizeof(char),_get_zero_arr_size(_tmp18,2));}),_tag_arr(_tmp17,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap){struct _tagged_arr msg=(struct
_tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);({struct Cyc_String_pa_struct
_tmp27;_tmp27.tag=0;_tmp27.f1=(struct _tagged_arr)((struct _tagged_arr)msg);{void*
_tmp25[1]={& _tmp27};Cyc_fprintf(Cyc_stderr,({const char*_tmp26="Compiler Error (Tcutil::impos): %s\n";
_tag_arr(_tmp26,sizeof(char),_get_zero_arr_size(_tmp26,36));}),_tag_arr(_tmp25,
sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp28=_cycalloc(sizeof(*_tmp28));
_tmp28[0]=({struct Cyc_Core_Impossible_struct _tmp29;_tmp29.tag=Cyc_Core_Impossible;
_tmp29.f1=msg;_tmp29;});_tmp28;}));}static struct _tagged_arr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){({struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;
_tmp2D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kindbound2string((
void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind));{struct Cyc_String_pa_struct _tmp2C;
_tmp2C.tag=0;_tmp2C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)tvs->hd));{void*_tmp2A[2]={& _tmp2C,& _tmp2D};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2B="%s::%s ";_tag_arr(_tmp2B,sizeof(char),
_get_zero_arr_size(_tmp2B,8));}),_tag_arr(_tmp2A,sizeof(void*),2));}}});}({void*
_tmp2E[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2F="\n";_tag_arr(_tmp2F,
sizeof(char),_get_zero_arr_size(_tmp2F,2));}),_tag_arr(_tmp2E,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=
0;static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap){struct
_tagged_arr msg=(struct _tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);Cyc_Tcutil_warning_segs=({
struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->hd=sg;_tmp30->tl=
Cyc_Tcutil_warning_segs;_tmp30;});Cyc_Tcutil_warning_msgs=({struct Cyc_List_List*
_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->hd=({struct _tagged_arr*_tmp32=
_cycalloc(sizeof(*_tmp32));_tmp32[0]=msg;_tmp32;});_tmp31->tl=Cyc_Tcutil_warning_msgs;
_tmp31;});}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)
return;({void*_tmp33[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp34="***Warnings***\n";
_tag_arr(_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,16));}),_tag_arr(_tmp33,
sizeof(void*),0));});{struct Cyc_List_List*_tmp35=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){({struct Cyc_String_pa_struct _tmp39;_tmp39.tag=
0;_tmp39.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd));{struct Cyc_String_pa_struct
_tmp38;_tmp38.tag=0;_tmp38.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct
_tagged_arr*)((struct Cyc_List_List*)_check_null(_tmp35))->hd));{void*_tmp36[2]={&
_tmp38,& _tmp39};Cyc_fprintf(Cyc_stderr,({const char*_tmp37="%s: %s\n";_tag_arr(
_tmp37,sizeof(char),_get_zero_arr_size(_tmp37,8));}),_tag_arr(_tmp36,sizeof(void*),
2));}}});_tmp35=_tmp35->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}({void*_tmp3A[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp3B="**************\n";_tag_arr(_tmp3B,sizeof(char),
_get_zero_arr_size(_tmp3B,16));}),_tag_arr(_tmp3A,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){return*((int*)_check_null(tv1->identity))- *((int*)_check_null(tv2->identity));}
void*Cyc_Tcutil_compress(void*t){void*_tmp3C=t;struct Cyc_Core_Opt*_tmp3D;void**
_tmp3E;void**_tmp3F;void***_tmp40;struct Cyc_Core_Opt*_tmp41;struct Cyc_Core_Opt**
_tmp42;_LL1: if(_tmp3C <= (void*)4  || *((int*)_tmp3C)!= 0)goto _LL3;_tmp3D=((struct
Cyc_Absyn_Evar_struct*)_tmp3C)->f2;if(_tmp3D != 0)goto _LL3;_LL2: goto _LL4;_LL3: if(
_tmp3C <= (void*)4  || *((int*)_tmp3C)!= 16)goto _LL5;_tmp3E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp3C)->f4;if(_tmp3E != 0)goto _LL5;_LL4: return t;_LL5: if(_tmp3C <= (void*)4  || *((
int*)_tmp3C)!= 16)goto _LL7;_tmp3F=((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;
_tmp40=(void***)&((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;_LL6: {void*
t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp40)));if(t2 != *((void**)
_check_null(*_tmp40)))*_tmp40=({void**_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=
t2;_tmp43;});return t2;}_LL7: if(_tmp3C <= (void*)4  || *((int*)_tmp3C)!= 0)goto _LL9;
_tmp41=((struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_tmp42=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_LL8: {void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(*_tmp42))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp42))->v)*_tmp42=({struct Cyc_Core_Opt*_tmp44=_cycalloc(sizeof(*
_tmp44));_tmp44->v=(void*)t2;_tmp44;});return t2;}_LL9:;_LLA: return t;_LL0:;}void*
Cyc_Tcutil_copy_type(void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(
struct Cyc_List_List*ts){return Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static
struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(struct Cyc_Absyn_Conref*c){void*
_tmp45=(void*)c->v;void*_tmp46;struct Cyc_Absyn_Conref*_tmp47;_LLC: if((int)_tmp45
!= 0)goto _LLE;_LLD: return Cyc_Absyn_empty_conref();_LLE: if(_tmp45 <= (void*)1  || *((
int*)_tmp45)!= 0)goto _LL10;_tmp46=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp45)->f1;_LLF: return Cyc_Absyn_new_conref(_tmp46);_LL10: if(_tmp45 <= (void*)1
 || *((int*)_tmp45)!= 1)goto _LLB;_tmp47=((struct Cyc_Absyn_Forward_constr_struct*)
_tmp45)->f1;_LL11: return Cyc_Tcutil_copy_conref(_tmp47);_LLB:;}static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp48=Cyc_Absyn_compress_kb(kb);void*_tmp49;void*_tmp4A;_LL13: if(*((
int*)_tmp48)!= 0)goto _LL15;_tmp49=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp48)->f1;
_LL14: return(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp4B=_cycalloc(sizeof(*
_tmp4B));_tmp4B[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp4C;_tmp4C.tag=0;_tmp4C.f1=(
void*)_tmp49;_tmp4C;});_tmp4B;});_LL15: if(*((int*)_tmp48)!= 1)goto _LL17;_LL16:
return(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));
_tmp4D[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp4E;_tmp4E.tag=1;_tmp4E.f1=0;
_tmp4E;});_tmp4D;});_LL17: if(*((int*)_tmp48)!= 2)goto _LL12;_tmp4A=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp48)->f2;_LL18: return(void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp50;_tmp50.tag=2;_tmp50.f1=0;_tmp50.f2=(void*)_tmp4A;_tmp50;});_tmp4F;});
_LL12:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->name=tv->name;
_tmp51->identity=0;_tmp51->kind=(void*)Cyc_Tcutil_copy_kindbound((void*)tv->kind);
_tmp51;});}static struct _tuple2*Cyc_Tcutil_copy_arg(struct _tuple2*arg){struct
_tuple2 _tmp53;struct Cyc_Core_Opt*_tmp54;struct Cyc_Absyn_Tqual _tmp55;void*_tmp56;
struct _tuple2*_tmp52=arg;_tmp53=*_tmp52;_tmp54=_tmp53.f1;_tmp55=_tmp53.f2;_tmp56=
_tmp53.f3;return({struct _tuple2*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=
_tmp54;_tmp57->f2=_tmp55;_tmp57->f3=Cyc_Tcutil_copy_type(_tmp56);_tmp57;});}
static struct _tuple4*Cyc_Tcutil_copy_tqt(struct _tuple4*arg){struct _tuple4 _tmp59;
struct Cyc_Absyn_Tqual _tmp5A;void*_tmp5B;struct _tuple4*_tmp58=arg;_tmp59=*_tmp58;
_tmp5A=_tmp59.f1;_tmp5B=_tmp59.f2;return({struct _tuple4*_tmp5C=_cycalloc(sizeof(*
_tmp5C));_tmp5C->f1=_tmp5A;_tmp5C->f2=Cyc_Tcutil_copy_type(_tmp5B);_tmp5C;});}
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*
f){return({struct Cyc_Absyn_Aggrfield*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->name=
f->name;_tmp5D->tq=f->tq;_tmp5D->type=(void*)Cyc_Tcutil_copy_type((void*)f->type);
_tmp5D->width=f->width;_tmp5D->attributes=f->attributes;_tmp5D;});}static struct
_tuple6*Cyc_Tcutil_copy_rgncmp(struct _tuple6*x){struct _tuple6 _tmp5F;void*_tmp60;
void*_tmp61;struct _tuple6*_tmp5E=x;_tmp5F=*_tmp5E;_tmp60=_tmp5F.f1;_tmp61=_tmp5F.f2;
return({struct _tuple6*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=Cyc_Tcutil_copy_type(
_tmp60);_tmp62->f2=Cyc_Tcutil_copy_type(_tmp61);_tmp62;});}static struct Cyc_Absyn_Enumfield*
Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){return({struct Cyc_Absyn_Enumfield*
_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->name=f->name;_tmp63->tag=f->tag;_tmp63->loc=
f->loc;_tmp63;});}void*Cyc_Tcutil_copy_type(void*t){void*_tmp64=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp65;struct Cyc_Absyn_TunionInfo _tmp66;void*_tmp67;
struct Cyc_List_List*_tmp68;void*_tmp69;struct Cyc_Absyn_TunionFieldInfo _tmp6A;
void*_tmp6B;struct Cyc_List_List*_tmp6C;struct Cyc_Absyn_PtrInfo _tmp6D;void*_tmp6E;
struct Cyc_Absyn_Tqual _tmp6F;struct Cyc_Absyn_PtrAtts _tmp70;void*_tmp71;struct Cyc_Absyn_Conref*
_tmp72;struct Cyc_Absyn_Conref*_tmp73;struct Cyc_Absyn_Conref*_tmp74;struct Cyc_Absyn_PtrLoc*
_tmp75;void*_tmp76;void*_tmp77;int _tmp78;struct Cyc_Absyn_ArrayInfo _tmp79;void*
_tmp7A;struct Cyc_Absyn_Tqual _tmp7B;struct Cyc_Absyn_Exp*_tmp7C;struct Cyc_Absyn_Conref*
_tmp7D;struct Cyc_Position_Segment*_tmp7E;struct Cyc_Absyn_FnInfo _tmp7F;struct Cyc_List_List*
_tmp80;struct Cyc_Core_Opt*_tmp81;void*_tmp82;struct Cyc_List_List*_tmp83;int
_tmp84;struct Cyc_Absyn_VarargInfo*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_List_List*
_tmp87;struct Cyc_List_List*_tmp88;struct Cyc_Absyn_AggrInfo _tmp89;void*_tmp8A;
void*_tmp8B;struct _tuple1*_tmp8C;struct Cyc_List_List*_tmp8D;struct Cyc_Absyn_AggrInfo
_tmp8E;void*_tmp8F;struct Cyc_Absyn_Aggrdecl**_tmp90;struct Cyc_List_List*_tmp91;
void*_tmp92;struct Cyc_List_List*_tmp93;struct _tuple1*_tmp94;struct Cyc_Absyn_Enumdecl*
_tmp95;struct Cyc_List_List*_tmp96;void*_tmp97;int _tmp98;void*_tmp99;void*_tmp9A;
struct _tuple1*_tmp9B;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Typedefdecl*
_tmp9D;void*_tmp9E;struct Cyc_List_List*_tmp9F;void*_tmpA0;_LL1A: if((int)_tmp64 != 
0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp64 <= (void*)4  || *((int*)_tmp64)!= 0)
goto _LL1E;_LL1D: return t;_LL1E: if(_tmp64 <= (void*)4  || *((int*)_tmp64)!= 1)goto
_LL20;_tmp65=((struct Cyc_Absyn_VarType_struct*)_tmp64)->f1;_LL1F: return(void*)({
struct Cyc_Absyn_VarType_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1[0]=({
struct Cyc_Absyn_VarType_struct _tmpA2;_tmpA2.tag=1;_tmpA2.f1=Cyc_Tcutil_copy_tvar(
_tmp65);_tmpA2;});_tmpA1;});_LL20: if(_tmp64 <= (void*)4  || *((int*)_tmp64)!= 2)
goto _LL22;_tmp66=((struct Cyc_Absyn_TunionType_struct*)_tmp64)->f1;_tmp67=(void*)
_tmp66.tunion_info;_tmp68=_tmp66.targs;_tmp69=(void*)_tmp66.rgn;_LL21: return(
void*)({struct Cyc_Absyn_TunionType_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));
_tmpA3[0]=({struct Cyc_Absyn_TunionType_struct _tmpA4;_tmpA4.tag=2;_tmpA4.f1=({
struct Cyc_Absyn_TunionInfo _tmpA5;_tmpA5.tunion_info=(void*)_tmp67;_tmpA5.targs=
Cyc_Tcutil_copy_types(_tmp68);_tmpA5.rgn=(void*)Cyc_Tcutil_copy_type(_tmp69);
_tmpA5;});_tmpA4;});_tmpA3;});_LL22: if(_tmp64 <= (void*)4  || *((int*)_tmp64)!= 3)
goto _LL24;_tmp6A=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp64)->f1;_tmp6B=(
void*)_tmp6A.field_info;_tmp6C=_tmp6A.targs;_LL23: return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpA7;_tmpA7.tag=3;_tmpA7.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpA8;_tmpA8.field_info=(
void*)_tmp6B;_tmpA8.targs=Cyc_Tcutil_copy_types(_tmp6C);_tmpA8;});_tmpA7;});
_tmpA6;});_LL24: if(_tmp64 <= (void*)4  || *((int*)_tmp64)!= 4)goto _LL26;_tmp6D=((
struct Cyc_Absyn_PointerType_struct*)_tmp64)->f1;_tmp6E=(void*)_tmp6D.elt_typ;
_tmp6F=_tmp6D.elt_tq;_tmp70=_tmp6D.ptr_atts;_tmp71=(void*)_tmp70.rgn;_tmp72=
_tmp70.nullable;_tmp73=_tmp70.bounds;_tmp74=_tmp70.zero_term;_tmp75=_tmp70.ptrloc;
_LL25: {void*_tmpA9=Cyc_Tcutil_copy_type(_tmp6E);void*_tmpAA=Cyc_Tcutil_copy_type(
_tmp71);struct Cyc_Absyn_Conref*_tmpAB=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp72);struct Cyc_Absyn_Tqual _tmpAC=_tmp6F;struct Cyc_Absyn_Conref*
_tmpAD=Cyc_Tcutil_copy_conref(_tmp73);struct Cyc_Absyn_Conref*_tmpAE=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp74);return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({
struct Cyc_Absyn_PointerType_struct _tmpB0;_tmpB0.tag=4;_tmpB0.f1=({struct Cyc_Absyn_PtrInfo
_tmpB1;_tmpB1.elt_typ=(void*)_tmpA9;_tmpB1.elt_tq=_tmpAC;_tmpB1.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpB2;_tmpB2.rgn=(void*)_tmpAA;_tmpB2.nullable=_tmpAB;
_tmpB2.bounds=_tmpAD;_tmpB2.zero_term=_tmpAE;_tmpB2.ptrloc=_tmp75;_tmpB2;});
_tmpB1;});_tmpB0;});_tmpAF;});}_LL26: if(_tmp64 <= (void*)4  || *((int*)_tmp64)!= 5)
goto _LL28;_tmp76=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp64)->f1;_tmp77=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp64)->f2;_LL27: return(void*)({struct
Cyc_Absyn_IntType_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_IntType_struct
_tmpB4;_tmpB4.tag=5;_tmpB4.f1=(void*)_tmp76;_tmpB4.f2=(void*)_tmp77;_tmpB4;});
_tmpB3;});_LL28: if((int)_tmp64 != 1)goto _LL2A;_LL29: return t;_LL2A: if(_tmp64 <= (
void*)4  || *((int*)_tmp64)!= 6)goto _LL2C;_tmp78=((struct Cyc_Absyn_DoubleType_struct*)
_tmp64)->f1;_LL2B: return(void*)({struct Cyc_Absyn_DoubleType_struct*_tmpB5=
_cycalloc_atomic(sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpB6;_tmpB6.tag=6;_tmpB6.f1=_tmp78;_tmpB6;});_tmpB5;});_LL2C: if(_tmp64 <= (void*)
4  || *((int*)_tmp64)!= 7)goto _LL2E;_tmp79=((struct Cyc_Absyn_ArrayType_struct*)
_tmp64)->f1;_tmp7A=(void*)_tmp79.elt_type;_tmp7B=_tmp79.tq;_tmp7C=_tmp79.num_elts;
_tmp7D=_tmp79.zero_term;_tmp7E=_tmp79.zt_loc;_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpB8;_tmpB8.tag=7;_tmpB8.f1=({struct Cyc_Absyn_ArrayInfo _tmpB9;_tmpB9.elt_type=(
void*)Cyc_Tcutil_copy_type(_tmp7A);_tmpB9.tq=_tmp7B;_tmpB9.num_elts=_tmp7C;
_tmpB9.zero_term=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(
_tmp7D);_tmpB9.zt_loc=_tmp7E;_tmpB9;});_tmpB8;});_tmpB7;});_LL2E: if(_tmp64 <= (
void*)4  || *((int*)_tmp64)!= 8)goto _LL30;_tmp7F=((struct Cyc_Absyn_FnType_struct*)
_tmp64)->f1;_tmp80=_tmp7F.tvars;_tmp81=_tmp7F.effect;_tmp82=(void*)_tmp7F.ret_typ;
_tmp83=_tmp7F.args;_tmp84=_tmp7F.c_varargs;_tmp85=_tmp7F.cyc_varargs;_tmp86=
_tmp7F.rgn_po;_tmp87=_tmp7F.attributes;_LL2F: {struct Cyc_List_List*_tmpBA=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp80);struct Cyc_Core_Opt*
_tmpBB=_tmp81 == 0?0:({struct Cyc_Core_Opt*_tmpC5=_cycalloc(sizeof(*_tmpC5));
_tmpC5->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp81->v);_tmpC5;});void*_tmpBC=
Cyc_Tcutil_copy_type(_tmp82);struct Cyc_List_List*_tmpBD=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp83);int _tmpBE=_tmp84;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp85 != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp85;cyc_varargs2=({
struct Cyc_Absyn_VarargInfo*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->name=cv->name;
_tmpBF->tq=cv->tq;_tmpBF->type=(void*)Cyc_Tcutil_copy_type((void*)cv->type);
_tmpBF->inject=cv->inject;_tmpBF;});}{struct Cyc_List_List*_tmpC0=((struct Cyc_List_List*(*)(
struct _tuple6*(*f)(struct _tuple6*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,
_tmp86);struct Cyc_List_List*_tmpC1=_tmp87;return(void*)({struct Cyc_Absyn_FnType_struct*
_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_FnType_struct
_tmpC3;_tmpC3.tag=8;_tmpC3.f1=({struct Cyc_Absyn_FnInfo _tmpC4;_tmpC4.tvars=_tmpBA;
_tmpC4.effect=_tmpBB;_tmpC4.ret_typ=(void*)_tmpBC;_tmpC4.args=_tmpBD;_tmpC4.c_varargs=
_tmpBE;_tmpC4.cyc_varargs=cyc_varargs2;_tmpC4.rgn_po=_tmpC0;_tmpC4.attributes=
_tmpC1;_tmpC4;});_tmpC3;});_tmpC2;});}}_LL30: if(_tmp64 <= (void*)4  || *((int*)
_tmp64)!= 9)goto _LL32;_tmp88=((struct Cyc_Absyn_TupleType_struct*)_tmp64)->f1;
_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*_tmpC6=_cycalloc(sizeof(*
_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_TupleType_struct _tmpC7;_tmpC7.tag=9;_tmpC7.f1=((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp88);_tmpC7;});_tmpC6;});_LL32: if(_tmp64 <= (
void*)4  || *((int*)_tmp64)!= 10)goto _LL34;_tmp89=((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1;_tmp8A=(void*)_tmp89.aggr_info;if(*((int*)_tmp8A)!= 0)goto _LL34;
_tmp8B=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp8A)->f1;_tmp8C=((struct
Cyc_Absyn_UnknownAggr_struct*)_tmp8A)->f2;_tmp8D=_tmp89.targs;_LL33: return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({
struct Cyc_Absyn_AggrType_struct _tmpC9;_tmpC9.tag=10;_tmpC9.f1=({struct Cyc_Absyn_AggrInfo
_tmpCA;_tmpCA.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmpCC;_tmpCC.tag=0;_tmpCC.f1=(void*)_tmp8B;_tmpCC.f2=_tmp8C;_tmpCC;});_tmpCB;}));
_tmpCA.targs=Cyc_Tcutil_copy_types(_tmp8D);_tmpCA;});_tmpC9;});_tmpC8;});_LL34:
if(_tmp64 <= (void*)4  || *((int*)_tmp64)!= 10)goto _LL36;_tmp8E=((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1;_tmp8F=(void*)_tmp8E.aggr_info;if(*((int*)_tmp8F)!= 1)goto _LL36;
_tmp90=((struct Cyc_Absyn_KnownAggr_struct*)_tmp8F)->f1;_tmp91=_tmp8E.targs;_LL35:
return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));
_tmpCD[0]=({struct Cyc_Absyn_AggrType_struct _tmpCE;_tmpCE.tag=10;_tmpCE.f1=({
struct Cyc_Absyn_AggrInfo _tmpCF;_tmpCF.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*
_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_KnownAggr_struct
_tmpD1;_tmpD1.tag=1;_tmpD1.f1=_tmp90;_tmpD1;});_tmpD0;}));_tmpCF.targs=Cyc_Tcutil_copy_types(
_tmp91);_tmpCF;});_tmpCE;});_tmpCD;});_LL36: if(_tmp64 <= (void*)4  || *((int*)
_tmp64)!= 11)goto _LL38;_tmp92=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp64)->f1;_tmp93=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp64)->f2;_LL37:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpD2=_cycalloc(sizeof(*
_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpD3;_tmpD3.tag=11;
_tmpD3.f1=(void*)_tmp92;_tmpD3.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmp93);_tmpD3;});_tmpD2;});_LL38: if(_tmp64 <= (void*)4  || *((int*)_tmp64)!= 12)
goto _LL3A;_tmp94=((struct Cyc_Absyn_EnumType_struct*)_tmp64)->f1;_tmp95=((struct
Cyc_Absyn_EnumType_struct*)_tmp64)->f2;_LL39: return(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_Absyn_EnumType_struct
_tmpD5;_tmpD5.tag=12;_tmpD5.f1=_tmp94;_tmpD5.f2=_tmp95;_tmpD5;});_tmpD4;});_LL3A:
if(_tmp64 <= (void*)4  || *((int*)_tmp64)!= 13)goto _LL3C;_tmp96=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp64)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmpD6=
_cycalloc(sizeof(*_tmpD6));_tmpD6[0]=({struct Cyc_Absyn_AnonEnumType_struct _tmpD7;
_tmpD7.tag=13;_tmpD7.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp96);_tmpD7;});_tmpD6;});_LL3C: if(_tmp64 <= (void*)4  || *((int*)_tmp64)!= 14)
goto _LL3E;_tmp97=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp64)->f1;_LL3D:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));
_tmpD8[0]=({struct Cyc_Absyn_SizeofType_struct _tmpD9;_tmpD9.tag=14;_tmpD9.f1=(
void*)Cyc_Tcutil_copy_type(_tmp97);_tmpD9;});_tmpD8;});_LL3E: if(_tmp64 <= (void*)
4  || *((int*)_tmp64)!= 18)goto _LL40;_tmp98=((struct Cyc_Absyn_TypeInt_struct*)
_tmp64)->f1;_LL3F: return(void*)({struct Cyc_Absyn_TypeInt_struct*_tmpDA=
_cycalloc_atomic(sizeof(*_tmpDA));_tmpDA[0]=({struct Cyc_Absyn_TypeInt_struct
_tmpDB;_tmpDB.tag=18;_tmpDB.f1=_tmp98;_tmpDB;});_tmpDA;});_LL40: if(_tmp64 <= (
void*)4  || *((int*)_tmp64)!= 17)goto _LL42;_tmp99=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp64)->f1;_LL41: return(void*)({struct Cyc_Absyn_TagType_struct*_tmpDC=_cycalloc(
sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_TagType_struct _tmpDD;_tmpDD.tag=17;
_tmpDD.f1=(void*)Cyc_Tcutil_copy_type(_tmp99);_tmpDD;});_tmpDC;});_LL42: if(
_tmp64 <= (void*)4  || *((int*)_tmp64)!= 15)goto _LL44;_tmp9A=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp64)->f1;_LL43: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpDE=
_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmpDF;_tmpDF.tag=15;_tmpDF.f1=(void*)Cyc_Tcutil_copy_type(_tmp9A);_tmpDF;});
_tmpDE;});_LL44: if(_tmp64 <= (void*)4  || *((int*)_tmp64)!= 16)goto _LL46;_tmp9B=((
struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f1;_tmp9C=((struct Cyc_Absyn_TypedefType_struct*)
_tmp64)->f2;_tmp9D=((struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f3;_LL45:
return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpE0=_cycalloc(sizeof(*
_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_TypedefType_struct _tmpE1;_tmpE1.tag=16;
_tmpE1.f1=_tmp9B;_tmpE1.f2=Cyc_Tcutil_copy_types(_tmp9C);_tmpE1.f3=_tmp9D;_tmpE1.f4=
0;_tmpE1;});_tmpE0;});_LL46: if((int)_tmp64 != 3)goto _LL48;_LL47: goto _LL49;_LL48:
if((int)_tmp64 != 2)goto _LL4A;_LL49: return t;_LL4A: if(_tmp64 <= (void*)4  || *((int*)
_tmp64)!= 19)goto _LL4C;_tmp9E=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp64)->f1;
_LL4B: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmpE2=_cycalloc(sizeof(*
_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_AccessEff_struct _tmpE3;_tmpE3.tag=19;_tmpE3.f1=(
void*)Cyc_Tcutil_copy_type(_tmp9E);_tmpE3;});_tmpE2;});_LL4C: if(_tmp64 <= (void*)
4  || *((int*)_tmp64)!= 20)goto _LL4E;_tmp9F=((struct Cyc_Absyn_JoinEff_struct*)
_tmp64)->f1;_LL4D: return(void*)({struct Cyc_Absyn_JoinEff_struct*_tmpE4=_cycalloc(
sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_JoinEff_struct _tmpE5;_tmpE5.tag=20;
_tmpE5.f1=Cyc_Tcutil_copy_types(_tmp9F);_tmpE5;});_tmpE4;});_LL4E: if(_tmp64 <= (
void*)4  || *((int*)_tmp64)!= 21)goto _LL19;_tmpA0=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp64)->f1;_LL4F: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmpE6=_cycalloc(
sizeof(*_tmpE6));_tmpE6[0]=({struct Cyc_Absyn_RgnsEff_struct _tmpE7;_tmpE7.tag=21;
_tmpE7.f1=(void*)Cyc_Tcutil_copy_type(_tmpA0);_tmpE7;});_tmpE6;});_LL19:;}int Cyc_Tcutil_kind_leq(
void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple6 _tmpE9=({struct _tuple6 _tmpE8;
_tmpE8.f1=k1;_tmpE8.f2=k2;_tmpE8;});void*_tmpEA;void*_tmpEB;void*_tmpEC;void*
_tmpED;void*_tmpEE;void*_tmpEF;_LL51: _tmpEA=_tmpE9.f1;if((int)_tmpEA != 2)goto
_LL53;_tmpEB=_tmpE9.f2;if((int)_tmpEB != 1)goto _LL53;_LL52: goto _LL54;_LL53: _tmpEC=
_tmpE9.f1;if((int)_tmpEC != 2)goto _LL55;_tmpED=_tmpE9.f2;if((int)_tmpED != 0)goto
_LL55;_LL54: goto _LL56;_LL55: _tmpEE=_tmpE9.f1;if((int)_tmpEE != 1)goto _LL57;_tmpEF=
_tmpE9.f2;if((int)_tmpEF != 0)goto _LL57;_LL56: return 1;_LL57:;_LL58: return 0;_LL50:;}}
void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpF0=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpF1;void*_tmpF2;_LL5A: if(*((int*)_tmpF0)!= 0)goto _LL5C;
_tmpF1=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpF0)->f1;_LL5B: return _tmpF1;
_LL5C: if(*((int*)_tmpF0)!= 2)goto _LL5E;_tmpF2=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpF0)->f2;_LL5D: return _tmpF2;_LL5E:;_LL5F:({void*_tmpF3[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpF4="kind not suitably constrained!";
_tag_arr(_tmpF4,sizeof(char),_get_zero_arr_size(_tmpF4,31));}),_tag_arr(_tmpF3,
sizeof(void*),0));});_LL59:;}void*Cyc_Tcutil_typ_kind(void*t){void*_tmpF5=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmpF6;struct Cyc_Core_Opt*_tmpF7;struct Cyc_Absyn_Tvar*
_tmpF8;void*_tmpF9;struct Cyc_Absyn_TunionFieldInfo _tmpFA;void*_tmpFB;struct Cyc_Absyn_Tunionfield*
_tmpFC;struct Cyc_Absyn_TunionFieldInfo _tmpFD;void*_tmpFE;struct Cyc_Absyn_Enumdecl*
_tmpFF;struct Cyc_Absyn_AggrInfo _tmp100;void*_tmp101;struct Cyc_Absyn_AggrInfo
_tmp102;void*_tmp103;struct Cyc_Absyn_Aggrdecl**_tmp104;struct Cyc_Absyn_Aggrdecl*
_tmp105;struct Cyc_Absyn_Aggrdecl _tmp106;struct Cyc_Absyn_AggrdeclImpl*_tmp107;
struct Cyc_Absyn_Enumdecl*_tmp108;struct Cyc_Absyn_PtrInfo _tmp109;struct Cyc_Absyn_Typedefdecl*
_tmp10A;_LL61: if(_tmpF5 <= (void*)4  || *((int*)_tmpF5)!= 0)goto _LL63;_tmpF6=((
struct Cyc_Absyn_Evar_struct*)_tmpF5)->f1;_tmpF7=((struct Cyc_Absyn_Evar_struct*)
_tmpF5)->f2;_LL62: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF6))->v;
_LL63: if(_tmpF5 <= (void*)4  || *((int*)_tmpF5)!= 1)goto _LL65;_tmpF8=((struct Cyc_Absyn_VarType_struct*)
_tmpF5)->f1;_LL64: return Cyc_Tcutil_tvar_kind(_tmpF8);_LL65: if((int)_tmpF5 != 0)
goto _LL67;_LL66: return(void*)1;_LL67: if(_tmpF5 <= (void*)4  || *((int*)_tmpF5)!= 5)
goto _LL69;_tmpF9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpF5)->f2;_LL68:
return _tmpF9 == (void*)((void*)2)?(void*)2:(void*)1;_LL69: if((int)_tmpF5 != 1)goto
_LL6B;_LL6A: goto _LL6C;_LL6B: if(_tmpF5 <= (void*)4  || *((int*)_tmpF5)!= 6)goto
_LL6D;_LL6C: goto _LL6E;_LL6D: if(_tmpF5 <= (void*)4  || *((int*)_tmpF5)!= 8)goto
_LL6F;_LL6E: return(void*)1;_LL6F: if(_tmpF5 <= (void*)4  || *((int*)_tmpF5)!= 15)
goto _LL71;_LL70: return(void*)2;_LL71: if((int)_tmpF5 != 3)goto _LL73;_LL72: goto
_LL74;_LL73: if((int)_tmpF5 != 2)goto _LL75;_LL74: return(void*)3;_LL75: if(_tmpF5 <= (
void*)4  || *((int*)_tmpF5)!= 2)goto _LL77;_LL76: return(void*)2;_LL77: if(_tmpF5 <= (
void*)4  || *((int*)_tmpF5)!= 3)goto _LL79;_tmpFA=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF5)->f1;_tmpFB=(void*)_tmpFA.field_info;if(*((int*)_tmpFB)!= 1)goto _LL79;
_tmpFC=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpFB)->f2;_LL78: if(_tmpFC->typs
== 0)return(void*)2;else{return(void*)1;}_LL79: if(_tmpF5 <= (void*)4  || *((int*)
_tmpF5)!= 3)goto _LL7B;_tmpFD=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpF5)->f1;
_tmpFE=(void*)_tmpFD.field_info;if(*((int*)_tmpFE)!= 0)goto _LL7B;_LL7A:({void*
_tmp10B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp10C="typ_kind: Unresolved TunionFieldType";_tag_arr(_tmp10C,
sizeof(char),_get_zero_arr_size(_tmp10C,37));}),_tag_arr(_tmp10B,sizeof(void*),0));});
_LL7B: if(_tmpF5 <= (void*)4  || *((int*)_tmpF5)!= 12)goto _LL7D;_tmpFF=((struct Cyc_Absyn_EnumType_struct*)
_tmpF5)->f2;if(_tmpFF != 0)goto _LL7D;_LL7C: goto _LL7E;_LL7D: if(_tmpF5 <= (void*)4
 || *((int*)_tmpF5)!= 10)goto _LL7F;_tmp100=((struct Cyc_Absyn_AggrType_struct*)
_tmpF5)->f1;_tmp101=(void*)_tmp100.aggr_info;if(*((int*)_tmp101)!= 0)goto _LL7F;
_LL7E: return(void*)0;_LL7F: if(_tmpF5 <= (void*)4  || *((int*)_tmpF5)!= 10)goto
_LL81;_tmp102=((struct Cyc_Absyn_AggrType_struct*)_tmpF5)->f1;_tmp103=(void*)
_tmp102.aggr_info;if(*((int*)_tmp103)!= 1)goto _LL81;_tmp104=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp103)->f1;_tmp105=*_tmp104;_tmp106=*_tmp105;_tmp107=_tmp106.impl;_LL80: return
_tmp107 == 0?(void*)0:(void*)1;_LL81: if(_tmpF5 <= (void*)4  || *((int*)_tmpF5)!= 11)
goto _LL83;_LL82: goto _LL84;_LL83: if(_tmpF5 <= (void*)4  || *((int*)_tmpF5)!= 13)
goto _LL85;_LL84: return(void*)1;_LL85: if(_tmpF5 <= (void*)4  || *((int*)_tmpF5)!= 
12)goto _LL87;_tmp108=((struct Cyc_Absyn_EnumType_struct*)_tmpF5)->f2;_LL86: if(
_tmp108->fields == 0)return(void*)0;else{return(void*)1;}_LL87: if(_tmpF5 <= (void*)
4  || *((int*)_tmpF5)!= 4)goto _LL89;_tmp109=((struct Cyc_Absyn_PointerType_struct*)
_tmpF5)->f1;_LL88: {void*_tmp10D=(void*)(Cyc_Absyn_compress_conref((_tmp109.ptr_atts).bounds))->v;
void*_tmp10E;void*_tmp10F;void*_tmp110;_LL9C: if((int)_tmp10D != 0)goto _LL9E;_LL9D:
goto _LL9F;_LL9E: if(_tmp10D <= (void*)1  || *((int*)_tmp10D)!= 0)goto _LLA0;_tmp10E=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10D)->f1;if((int)_tmp10E != 0)goto
_LLA0;_LL9F: return(void*)1;_LLA0: if(_tmp10D <= (void*)1  || *((int*)_tmp10D)!= 0)
goto _LLA2;_tmp10F=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10D)->f1;if(
_tmp10F <= (void*)1  || *((int*)_tmp10F)!= 0)goto _LLA2;_LLA1: goto _LLA3;_LLA2: if(
_tmp10D <= (void*)1  || *((int*)_tmp10D)!= 0)goto _LLA4;_tmp110=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp10D)->f1;if(_tmp110 <= (void*)1  || *((int*)_tmp110)!= 1)goto _LLA4;_LLA3:
return(void*)2;_LLA4: if(_tmp10D <= (void*)1  || *((int*)_tmp10D)!= 1)goto _LL9B;
_LLA5:({void*_tmp111[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp112="typ_kind: forward constr in ptr bounds";
_tag_arr(_tmp112,sizeof(char),_get_zero_arr_size(_tmp112,39));}),_tag_arr(
_tmp111,sizeof(void*),0));});_LL9B:;}_LL89: if(_tmpF5 <= (void*)4  || *((int*)
_tmpF5)!= 14)goto _LL8B;_LL8A: return(void*)2;_LL8B: if(_tmpF5 <= (void*)4  || *((int*)
_tmpF5)!= 18)goto _LL8D;_LL8C: return(void*)5;_LL8D: if(_tmpF5 <= (void*)4  || *((int*)
_tmpF5)!= 17)goto _LL8F;_LL8E: return(void*)2;_LL8F: if(_tmpF5 <= (void*)4  || *((int*)
_tmpF5)!= 7)goto _LL91;_LL90: goto _LL92;_LL91: if(_tmpF5 <= (void*)4  || *((int*)
_tmpF5)!= 9)goto _LL93;_LL92: return(void*)1;_LL93: if(_tmpF5 <= (void*)4  || *((int*)
_tmpF5)!= 16)goto _LL95;_tmp10A=((struct Cyc_Absyn_TypedefType_struct*)_tmpF5)->f3;
_LL94: if(_tmp10A == 0  || _tmp10A->kind == 0)({struct Cyc_String_pa_struct _tmp115;
_tmp115.tag=0;_tmp115.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp113[1]={& _tmp115};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(({const char*_tmp114="typ_kind: typedef found: %s";_tag_arr(
_tmp114,sizeof(char),_get_zero_arr_size(_tmp114,28));}),_tag_arr(_tmp113,sizeof(
void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp10A->kind))->v;
_LL95: if(_tmpF5 <= (void*)4  || *((int*)_tmpF5)!= 19)goto _LL97;_LL96: goto _LL98;
_LL97: if(_tmpF5 <= (void*)4  || *((int*)_tmpF5)!= 20)goto _LL99;_LL98: goto _LL9A;
_LL99: if(_tmpF5 <= (void*)4  || *((int*)_tmpF5)!= 21)goto _LL60;_LL9A: return(void*)
4;_LL60:;}int Cyc_Tcutil_unify(void*t1,void*t2){struct _handler_cons _tmp116;
_push_handler(& _tmp116);{int _tmp118=0;if(setjmp(_tmp116.handler))_tmp118=1;if(!
_tmp118){Cyc_Tcutil_unify_it(t1,t2);{int _tmp119=1;_npop_handler(0);return _tmp119;};
_pop_handler();}else{void*_tmp117=(void*)_exn_thrown;void*_tmp11B=_tmp117;_LLA7:
if(_tmp11B != Cyc_Tcutil_Unify)goto _LLA9;_LLA8: return 0;_LLA9:;_LLAA:(void)_throw(
_tmp11B);_LLA6:;}}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*
r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(
t);{void*_tmp11C=t;struct Cyc_Absyn_Tvar*_tmp11D;struct Cyc_Core_Opt*_tmp11E;
struct Cyc_Core_Opt*_tmp11F;struct Cyc_Core_Opt**_tmp120;struct Cyc_Absyn_PtrInfo
_tmp121;struct Cyc_Absyn_ArrayInfo _tmp122;void*_tmp123;struct Cyc_Absyn_FnInfo
_tmp124;struct Cyc_List_List*_tmp125;struct Cyc_Core_Opt*_tmp126;void*_tmp127;
struct Cyc_List_List*_tmp128;int _tmp129;struct Cyc_Absyn_VarargInfo*_tmp12A;struct
Cyc_List_List*_tmp12B;struct Cyc_List_List*_tmp12C;struct Cyc_List_List*_tmp12D;
struct Cyc_Absyn_TunionInfo _tmp12E;struct Cyc_List_List*_tmp12F;void*_tmp130;
struct Cyc_List_List*_tmp131;struct Cyc_Absyn_TunionFieldInfo _tmp132;struct Cyc_List_List*
_tmp133;struct Cyc_Absyn_AggrInfo _tmp134;struct Cyc_List_List*_tmp135;struct Cyc_List_List*
_tmp136;void*_tmp137;void*_tmp138;void*_tmp139;void*_tmp13A;void*_tmp13B;struct
Cyc_List_List*_tmp13C;_LLAC: if(_tmp11C <= (void*)4  || *((int*)_tmp11C)!= 1)goto
_LLAE;_tmp11D=((struct Cyc_Absyn_VarType_struct*)_tmp11C)->f1;_LLAD: if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp11D)){Cyc_Tcutil_failure_reason=({
const char*_tmp13D="(type variable would escape scope)";_tag_arr(_tmp13D,sizeof(
char),_get_zero_arr_size(_tmp13D,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}
goto _LLAB;_LLAE: if(_tmp11C <= (void*)4  || *((int*)_tmp11C)!= 0)goto _LLB0;_tmp11E=((
struct Cyc_Absyn_Evar_struct*)_tmp11C)->f2;_tmp11F=((struct Cyc_Absyn_Evar_struct*)
_tmp11C)->f4;_tmp120=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp11C)->f4;_LLAF: if(t == evar){Cyc_Tcutil_failure_reason=({const char*_tmp13E="(occurs check)";
_tag_arr(_tmp13E,sizeof(char),_get_zero_arr_size(_tmp13E,15));});(int)_throw((
void*)Cyc_Tcutil_Unify);}else{if(_tmp11E != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)
_tmp11E->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp120))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp13F=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp120))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd))_tmp13F=({struct Cyc_List_List*_tmp140=
_cycalloc(sizeof(*_tmp140));_tmp140->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp140->tl=
_tmp13F;_tmp140;});}}*_tmp120=({struct Cyc_Core_Opt*_tmp141=_cycalloc(sizeof(*
_tmp141));_tmp141->v=_tmp13F;_tmp141;});}}}goto _LLAB;_LLB0: if(_tmp11C <= (void*)4
 || *((int*)_tmp11C)!= 4)goto _LLB2;_tmp121=((struct Cyc_Absyn_PointerType_struct*)
_tmp11C)->f1;_LLB1: Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp121.elt_typ);Cyc_Tcutil_occurs(
evar,r,env,(void*)(_tmp121.ptr_atts).rgn);{void*_tmp142=(void*)(Cyc_Absyn_compress_conref((
_tmp121.ptr_atts).bounds))->v;void*_tmp143;void*_tmp144;_LLD1: if(_tmp142 <= (void*)
1  || *((int*)_tmp142)!= 0)goto _LLD3;_tmp143=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp142)->f1;if(_tmp143 <= (void*)1  || *((int*)_tmp143)!= 1)goto _LLD3;_tmp144=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp143)->f1;_LLD2: Cyc_Tcutil_occurs(
evar,r,env,_tmp144);goto _LLD0;_LLD3:;_LLD4: goto _LLD0;_LLD0:;}goto _LLAB;_LLB2: if(
_tmp11C <= (void*)4  || *((int*)_tmp11C)!= 7)goto _LLB4;_tmp122=((struct Cyc_Absyn_ArrayType_struct*)
_tmp11C)->f1;_tmp123=(void*)_tmp122.elt_type;_LLB3: Cyc_Tcutil_occurs(evar,r,env,
_tmp123);goto _LLAB;_LLB4: if(_tmp11C <= (void*)4  || *((int*)_tmp11C)!= 8)goto _LLB6;
_tmp124=((struct Cyc_Absyn_FnType_struct*)_tmp11C)->f1;_tmp125=_tmp124.tvars;
_tmp126=_tmp124.effect;_tmp127=(void*)_tmp124.ret_typ;_tmp128=_tmp124.args;
_tmp129=_tmp124.c_varargs;_tmp12A=_tmp124.cyc_varargs;_tmp12B=_tmp124.rgn_po;
_tmp12C=_tmp124.attributes;_LLB5: env=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,
_tmp125,env);if(_tmp126 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp126->v);Cyc_Tcutil_occurs(
evar,r,env,_tmp127);for(0;_tmp128 != 0;_tmp128=_tmp128->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple2*)_tmp128->hd)).f3);}if(_tmp12A != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp12A->type);for(0;_tmp12B != 0;_tmp12B=_tmp12B->tl){struct
_tuple6 _tmp146;void*_tmp147;void*_tmp148;struct _tuple6*_tmp145=(struct _tuple6*)
_tmp12B->hd;_tmp146=*_tmp145;_tmp147=_tmp146.f1;_tmp148=_tmp146.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp147);Cyc_Tcutil_occurs(evar,r,env,_tmp148);}goto _LLAB;_LLB6: if(
_tmp11C <= (void*)4  || *((int*)_tmp11C)!= 9)goto _LLB8;_tmp12D=((struct Cyc_Absyn_TupleType_struct*)
_tmp11C)->f1;_LLB7: for(0;_tmp12D != 0;_tmp12D=_tmp12D->tl){Cyc_Tcutil_occurs(evar,
r,env,(*((struct _tuple4*)_tmp12D->hd)).f2);}goto _LLAB;_LLB8: if(_tmp11C <= (void*)
4  || *((int*)_tmp11C)!= 2)goto _LLBA;_tmp12E=((struct Cyc_Absyn_TunionType_struct*)
_tmp11C)->f1;_tmp12F=_tmp12E.targs;_tmp130=(void*)_tmp12E.rgn;_LLB9: Cyc_Tcutil_occurs(
evar,r,env,_tmp130);Cyc_Tcutil_occurslist(evar,r,env,_tmp12F);goto _LLAB;_LLBA:
if(_tmp11C <= (void*)4  || *((int*)_tmp11C)!= 16)goto _LLBC;_tmp131=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11C)->f2;_LLBB: _tmp133=_tmp131;goto _LLBD;_LLBC: if(_tmp11C <= (void*)4  || *((
int*)_tmp11C)!= 3)goto _LLBE;_tmp132=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp11C)->f1;_tmp133=_tmp132.targs;_LLBD: _tmp135=_tmp133;goto _LLBF;_LLBE: if(
_tmp11C <= (void*)4  || *((int*)_tmp11C)!= 10)goto _LLC0;_tmp134=((struct Cyc_Absyn_AggrType_struct*)
_tmp11C)->f1;_tmp135=_tmp134.targs;_LLBF: Cyc_Tcutil_occurslist(evar,r,env,
_tmp135);goto _LLAB;_LLC0: if(_tmp11C <= (void*)4  || *((int*)_tmp11C)!= 11)goto
_LLC2;_tmp136=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11C)->f2;_LLC1: for(0;
_tmp136 != 0;_tmp136=_tmp136->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmp136->hd)->type);}goto _LLAB;_LLC2: if(_tmp11C <= (void*)4  || *((int*)_tmp11C)!= 
17)goto _LLC4;_tmp137=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp11C)->f1;_LLC3:
_tmp138=_tmp137;goto _LLC5;_LLC4: if(_tmp11C <= (void*)4  || *((int*)_tmp11C)!= 14)
goto _LLC6;_tmp138=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp11C)->f1;_LLC5:
_tmp139=_tmp138;goto _LLC7;_LLC6: if(_tmp11C <= (void*)4  || *((int*)_tmp11C)!= 19)
goto _LLC8;_tmp139=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp11C)->f1;_LLC7:
_tmp13A=_tmp139;goto _LLC9;_LLC8: if(_tmp11C <= (void*)4  || *((int*)_tmp11C)!= 15)
goto _LLCA;_tmp13A=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp11C)->f1;
_LLC9: _tmp13B=_tmp13A;goto _LLCB;_LLCA: if(_tmp11C <= (void*)4  || *((int*)_tmp11C)
!= 21)goto _LLCC;_tmp13B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp11C)->f1;
_LLCB: Cyc_Tcutil_occurs(evar,r,env,_tmp13B);goto _LLAB;_LLCC: if(_tmp11C <= (void*)
4  || *((int*)_tmp11C)!= 20)goto _LLCE;_tmp13C=((struct Cyc_Absyn_JoinEff_struct*)
_tmp11C)->f1;_LLCD: Cyc_Tcutil_occurslist(evar,r,env,_tmp13C);goto _LLAB;_LLCE:;
_LLCF: goto _LLAB;_LLAB:;}}static void Cyc_Tcutil_occurslist(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){for(0;ts != 0;ts=
ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(
struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,
t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}if(t1 != 0  || t2 != 
0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(struct
Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const)({void*_tmp149[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp14A="tq1 real_const not set.";
_tag_arr(_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,24));}),_tag_arr(
_tmp149,sizeof(void*),0));});if(tq2.print_const  && !tq2.real_const)({void*
_tmp14B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp14C="tq2 real_const not set.";_tag_arr(_tmp14C,sizeof(char),
_get_zero_arr_size(_tmp14C,24));}),_tag_arr(_tmp14B,sizeof(void*),0));});if((tq1.real_const
!= tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;Cyc_Tcutil_failure_reason=({const char*
_tmp14D="(qualifiers don't match)";_tag_arr(_tmp14D,sizeof(char),
_get_zero_arr_size(_tmp14D,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_tq1_const=
0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{void*_tmp14E=(void*)x->v;void*_tmp14F;_LLD6: if((int)_tmp14E != 0)
goto _LLD8;_LLD7:(void*)(x->v=(void*)((void*)({struct Cyc_Absyn_Forward_constr_struct*
_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[0]=({struct Cyc_Absyn_Forward_constr_struct
_tmp151;_tmp151.tag=1;_tmp151.f1=y;_tmp151;});_tmp150;})));return;_LLD8: if(
_tmp14E <= (void*)1  || *((int*)_tmp14E)!= 0)goto _LLDA;_tmp14F=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp14E)->f1;_LLD9: {void*_tmp152=(void*)y->v;void*_tmp153;_LLDD: if((int)_tmp152
!= 0)goto _LLDF;_LLDE:(void*)(y->v=(void*)((void*)x->v));return;_LLDF: if(_tmp152
<= (void*)1  || *((int*)_tmp152)!= 0)goto _LLE1;_tmp153=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp152)->f1;_LLE0: if(cmp(_tmp14F,_tmp153)!= 0){Cyc_Tcutil_failure_reason=reason;(
int)_throw((void*)Cyc_Tcutil_Unify);}return;_LLE1: if(_tmp152 <= (void*)1  || *((
int*)_tmp152)!= 1)goto _LLDC;_LLE2:({void*_tmp154[0]={};Cyc_Tcutil_impos(({const
char*_tmp155="unify_conref: forward after compress(2)";_tag_arr(_tmp155,sizeof(
char),_get_zero_arr_size(_tmp155,40));}),_tag_arr(_tmp154,sizeof(void*),0));});
_LLDC:;}_LLDA: if(_tmp14E <= (void*)1  || *((int*)_tmp14E)!= 1)goto _LLD5;_LLDB:({
void*_tmp156[0]={};Cyc_Tcutil_impos(({const char*_tmp157="unify_conref: forward after compress";
_tag_arr(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,37));}),_tag_arr(
_tmp156,sizeof(void*),0));});_LLD5:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(
void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp158;_push_handler(& _tmp158);{int _tmp15A=0;if(setjmp(_tmp158.handler))
_tmp15A=1;if(!_tmp15A){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _tagged_arr)
_tag_arr(0,0,0));{int _tmp15B=1;_npop_handler(0);return _tmp15B;};_pop_handler();}
else{void*_tmp159=(void*)_exn_thrown;void*_tmp15D=_tmp159;_LLE4: if(_tmp15D != Cyc_Tcutil_Unify)
goto _LLE6;_LLE5: return 0;_LLE6:;_LLE7:(void)_throw(_tmp15D);_LLE3:;}}}static int
Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple6 _tmp15F=({struct _tuple6
_tmp15E;_tmp15E.f1=b1;_tmp15E.f2=b2;_tmp15E;});void*_tmp160;void*_tmp161;void*
_tmp162;void*_tmp163;void*_tmp164;struct Cyc_Absyn_Exp*_tmp165;void*_tmp166;
struct Cyc_Absyn_Exp*_tmp167;void*_tmp168;void*_tmp169;void*_tmp16A;void*_tmp16B;
void*_tmp16C;void*_tmp16D;void*_tmp16E;void*_tmp16F;_LLE9: _tmp160=_tmp15F.f1;if((
int)_tmp160 != 0)goto _LLEB;_tmp161=_tmp15F.f2;if((int)_tmp161 != 0)goto _LLEB;_LLEA:
return 0;_LLEB: _tmp162=_tmp15F.f1;if((int)_tmp162 != 0)goto _LLED;_LLEC: return - 1;
_LLED: _tmp163=_tmp15F.f2;if((int)_tmp163 != 0)goto _LLEF;_LLEE: return 1;_LLEF:
_tmp164=_tmp15F.f1;if(_tmp164 <= (void*)1  || *((int*)_tmp164)!= 0)goto _LLF1;
_tmp165=((struct Cyc_Absyn_Upper_b_struct*)_tmp164)->f1;_tmp166=_tmp15F.f2;if(
_tmp166 <= (void*)1  || *((int*)_tmp166)!= 0)goto _LLF1;_tmp167=((struct Cyc_Absyn_Upper_b_struct*)
_tmp166)->f1;_LLF0: return Cyc_Evexp_const_exp_cmp(_tmp165,_tmp167);_LLF1: _tmp168=
_tmp15F.f1;if(_tmp168 <= (void*)1  || *((int*)_tmp168)!= 0)goto _LLF3;_tmp169=
_tmp15F.f2;if(_tmp169 <= (void*)1  || *((int*)_tmp169)!= 1)goto _LLF3;_LLF2: return -
1;_LLF3: _tmp16A=_tmp15F.f1;if(_tmp16A <= (void*)1  || *((int*)_tmp16A)!= 1)goto
_LLF5;_tmp16B=_tmp15F.f2;if(_tmp16B <= (void*)1  || *((int*)_tmp16B)!= 0)goto _LLF5;
_LLF4: return 1;_LLF5: _tmp16C=_tmp15F.f1;if(_tmp16C <= (void*)1  || *((int*)_tmp16C)
!= 1)goto _LLE8;_tmp16D=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp16C)->f1;
_tmp16E=_tmp15F.f2;if(_tmp16E <= (void*)1  || *((int*)_tmp16E)!= 1)goto _LLE8;
_tmp16F=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp16E)->f1;_LLF6: return Cyc_Tcutil_typecmp(
_tmp16D,_tmp16F);_LLE8:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple6 _tmp171=({struct _tuple6 _tmp170;_tmp170.f1=b1;_tmp170.f2=b2;_tmp170;});
void*_tmp172;void*_tmp173;void*_tmp174;void*_tmp175;void*_tmp176;struct Cyc_Absyn_Exp*
_tmp177;void*_tmp178;struct Cyc_Absyn_Exp*_tmp179;void*_tmp17A;struct Cyc_Absyn_Exp*
_tmp17B;void*_tmp17C;void*_tmp17D;void*_tmp17E;void*_tmp17F;void*_tmp180;struct
Cyc_Absyn_Exp*_tmp181;void*_tmp182;void*_tmp183;void*_tmp184;void*_tmp185;_LLF8:
_tmp172=_tmp171.f1;if((int)_tmp172 != 0)goto _LLFA;_tmp173=_tmp171.f2;if((int)
_tmp173 != 0)goto _LLFA;_LLF9: return 0;_LLFA: _tmp174=_tmp171.f1;if((int)_tmp174 != 0)
goto _LLFC;_LLFB: return - 1;_LLFC: _tmp175=_tmp171.f2;if((int)_tmp175 != 0)goto _LLFE;
_LLFD: return 1;_LLFE: _tmp176=_tmp171.f1;if(_tmp176 <= (void*)1  || *((int*)_tmp176)
!= 0)goto _LL100;_tmp177=((struct Cyc_Absyn_Upper_b_struct*)_tmp176)->f1;_tmp178=
_tmp171.f2;if(_tmp178 <= (void*)1  || *((int*)_tmp178)!= 0)goto _LL100;_tmp179=((
struct Cyc_Absyn_Upper_b_struct*)_tmp178)->f1;_LLFF: return Cyc_Evexp_const_exp_cmp(
_tmp177,_tmp179);_LL100: _tmp17A=_tmp171.f1;if(_tmp17A <= (void*)1  || *((int*)
_tmp17A)!= 0)goto _LL102;_tmp17B=((struct Cyc_Absyn_Upper_b_struct*)_tmp17A)->f1;
_tmp17C=_tmp171.f2;if(_tmp17C <= (void*)1  || *((int*)_tmp17C)!= 1)goto _LL102;
_tmp17D=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp17C)->f1;_LL101: _tmp17F=
_tmp17D;_tmp181=_tmp17B;goto _LL103;_LL102: _tmp17E=_tmp171.f1;if(_tmp17E <= (void*)
1  || *((int*)_tmp17E)!= 1)goto _LL104;_tmp17F=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp17E)->f1;_tmp180=_tmp171.f2;if(_tmp180 <= (void*)1  || *((int*)_tmp180)!= 0)
goto _LL104;_tmp181=((struct Cyc_Absyn_Upper_b_struct*)_tmp180)->f1;_LL103: {
unsigned int _tmp187;int _tmp188;struct _tuple7 _tmp186=Cyc_Evexp_eval_const_uint_exp(
_tmp181);_tmp187=_tmp186.f1;_tmp188=_tmp186.f2;if(!_tmp188)return 1;_tmp183=
_tmp17F;_tmp185=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp189=_cycalloc_atomic(
sizeof(*_tmp189));_tmp189[0]=({struct Cyc_Absyn_TypeInt_struct _tmp18A;_tmp18A.tag=
18;_tmp18A.f1=(int)_tmp187;_tmp18A;});_tmp189;});goto _LL105;}_LL104: _tmp182=
_tmp171.f1;if(_tmp182 <= (void*)1  || *((int*)_tmp182)!= 1)goto _LLF7;_tmp183=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmp182)->f1;_tmp184=_tmp171.f2;if(_tmp184 <= (
void*)1  || *((int*)_tmp184)!= 1)goto _LLF7;_tmp185=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp184)->f1;_LL105: Cyc_Tcutil_unify_it(_tmp183,_tmp185);return 0;_LLF7:;}static
int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp18B=att;_LL107: if(_tmp18B
<= (void*)17  || *((int*)_tmp18B)!= 0)goto _LL109;_LL108: return 0;_LL109: if((int)
_tmp18B != 0)goto _LL10B;_LL10A: return 1;_LL10B: if((int)_tmp18B != 1)goto _LL10D;
_LL10C: return 2;_LL10D: if((int)_tmp18B != 2)goto _LL10F;_LL10E: return 3;_LL10F: if((
int)_tmp18B != 3)goto _LL111;_LL110: return 4;_LL111: if((int)_tmp18B != 4)goto _LL113;
_LL112: return 5;_LL113: if(_tmp18B <= (void*)17  || *((int*)_tmp18B)!= 1)goto _LL115;
_LL114: return 6;_LL115: if((int)_tmp18B != 5)goto _LL117;_LL116: return 7;_LL117: if(
_tmp18B <= (void*)17  || *((int*)_tmp18B)!= 2)goto _LL119;_LL118: return 8;_LL119: if((
int)_tmp18B != 6)goto _LL11B;_LL11A: return 9;_LL11B: if((int)_tmp18B != 7)goto _LL11D;
_LL11C: return 10;_LL11D: if((int)_tmp18B != 8)goto _LL11F;_LL11E: return 11;_LL11F: if((
int)_tmp18B != 9)goto _LL121;_LL120: return 12;_LL121: if((int)_tmp18B != 10)goto
_LL123;_LL122: return 13;_LL123: if((int)_tmp18B != 11)goto _LL125;_LL124: return 14;
_LL125: if((int)_tmp18B != 12)goto _LL127;_LL126: return 15;_LL127: if((int)_tmp18B != 
13)goto _LL129;_LL128: return 16;_LL129: if((int)_tmp18B != 14)goto _LL12B;_LL12A:
return 17;_LL12B: if((int)_tmp18B != 15)goto _LL12D;_LL12C: return 18;_LL12D: if(
_tmp18B <= (void*)17  || *((int*)_tmp18B)!= 3)goto _LL12F;_LL12E: return 19;_LL12F:
if(_tmp18B <= (void*)17  || *((int*)_tmp18B)!= 4)goto _LL131;_LL130: return 20;_LL131:;
_LL132: return 21;_LL106:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple6 _tmp18D=({struct _tuple6 _tmp18C;_tmp18C.f1=att1;_tmp18C.f2=att2;
_tmp18C;});void*_tmp18E;int _tmp18F;void*_tmp190;int _tmp191;void*_tmp192;int
_tmp193;void*_tmp194;int _tmp195;void*_tmp196;int _tmp197;void*_tmp198;int _tmp199;
void*_tmp19A;struct _tagged_arr _tmp19B;void*_tmp19C;struct _tagged_arr _tmp19D;void*
_tmp19E;void*_tmp19F;int _tmp1A0;int _tmp1A1;void*_tmp1A2;void*_tmp1A3;int _tmp1A4;
int _tmp1A5;_LL134: _tmp18E=_tmp18D.f1;if(_tmp18E <= (void*)17  || *((int*)_tmp18E)
!= 0)goto _LL136;_tmp18F=((struct Cyc_Absyn_Regparm_att_struct*)_tmp18E)->f1;
_tmp190=_tmp18D.f2;if(_tmp190 <= (void*)17  || *((int*)_tmp190)!= 0)goto _LL136;
_tmp191=((struct Cyc_Absyn_Regparm_att_struct*)_tmp190)->f1;_LL135: _tmp193=
_tmp18F;_tmp195=_tmp191;goto _LL137;_LL136: _tmp192=_tmp18D.f1;if(_tmp192 <= (void*)
17  || *((int*)_tmp192)!= 4)goto _LL138;_tmp193=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp192)->f1;_tmp194=_tmp18D.f2;if(_tmp194 <= (void*)17  || *((int*)_tmp194)!= 4)
goto _LL138;_tmp195=((struct Cyc_Absyn_Initializes_att_struct*)_tmp194)->f1;_LL137:
_tmp197=_tmp193;_tmp199=_tmp195;goto _LL139;_LL138: _tmp196=_tmp18D.f1;if(_tmp196
<= (void*)17  || *((int*)_tmp196)!= 1)goto _LL13A;_tmp197=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp196)->f1;_tmp198=_tmp18D.f2;if(_tmp198 <= (void*)17  || *((int*)_tmp198)!= 1)
goto _LL13A;_tmp199=((struct Cyc_Absyn_Aligned_att_struct*)_tmp198)->f1;_LL139:
return Cyc_Core_intcmp(_tmp197,_tmp199);_LL13A: _tmp19A=_tmp18D.f1;if(_tmp19A <= (
void*)17  || *((int*)_tmp19A)!= 2)goto _LL13C;_tmp19B=((struct Cyc_Absyn_Section_att_struct*)
_tmp19A)->f1;_tmp19C=_tmp18D.f2;if(_tmp19C <= (void*)17  || *((int*)_tmp19C)!= 2)
goto _LL13C;_tmp19D=((struct Cyc_Absyn_Section_att_struct*)_tmp19C)->f1;_LL13B:
return Cyc_strcmp((struct _tagged_arr)_tmp19B,(struct _tagged_arr)_tmp19D);_LL13C:
_tmp19E=_tmp18D.f1;if(_tmp19E <= (void*)17  || *((int*)_tmp19E)!= 3)goto _LL13E;
_tmp19F=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp19E)->f1;_tmp1A0=((
struct Cyc_Absyn_Format_att_struct*)_tmp19E)->f2;_tmp1A1=((struct Cyc_Absyn_Format_att_struct*)
_tmp19E)->f3;_tmp1A2=_tmp18D.f2;if(_tmp1A2 <= (void*)17  || *((int*)_tmp1A2)!= 3)
goto _LL13E;_tmp1A3=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp1A2)->f1;
_tmp1A4=((struct Cyc_Absyn_Format_att_struct*)_tmp1A2)->f2;_tmp1A5=((struct Cyc_Absyn_Format_att_struct*)
_tmp1A2)->f3;_LL13D: {int _tmp1A6=Cyc_Core_intcmp((int)((unsigned int)_tmp19F),(
int)((unsigned int)_tmp1A3));if(_tmp1A6 != 0)return _tmp1A6;{int _tmp1A7=Cyc_Core_intcmp(
_tmp1A0,_tmp1A4);if(_tmp1A7 != 0)return _tmp1A7;return Cyc_Core_intcmp(_tmp1A1,
_tmp1A5);}}_LL13E:;_LL13F: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL133:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return
0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static
void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of((
void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*
tv){void*t;{void*_tmp1A8=Cyc_Tcutil_tvar_kind(tv);_LL141: if((int)_tmp1A8 != 3)
goto _LL143;_LL142: t=(void*)2;goto _LL140;_LL143: if((int)_tmp1A8 != 4)goto _LL145;
_LL144: t=Cyc_Absyn_empty_effect;goto _LL140;_LL145: if((int)_tmp1A8 != 5)goto _LL147;
_LL146: t=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp1A9=_cycalloc_atomic(
sizeof(*_tmp1A9));_tmp1A9[0]=({struct Cyc_Absyn_TypeInt_struct _tmp1AA;_tmp1AA.tag=
18;_tmp1AA.f1=0;_tmp1AA;});_tmp1A9;});goto _LL140;_LL147:;_LL148: t=Cyc_Absyn_sint_typ;
goto _LL140;_LL140:;}return({struct _tuple8*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));
_tmp1AB->f1=tv;_tmp1AB->f2=t;_tmp1AB;});}static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp1AC=Cyc_Tcutil_compress(t);void*_tmp1AD;struct Cyc_Absyn_TunionInfo
_tmp1AE;struct Cyc_List_List*_tmp1AF;void*_tmp1B0;struct Cyc_Absyn_PtrInfo _tmp1B1;
void*_tmp1B2;struct Cyc_Absyn_PtrAtts _tmp1B3;void*_tmp1B4;struct Cyc_Absyn_ArrayInfo
_tmp1B5;void*_tmp1B6;struct Cyc_List_List*_tmp1B7;struct Cyc_Absyn_TunionFieldInfo
_tmp1B8;struct Cyc_List_List*_tmp1B9;struct Cyc_Absyn_AggrInfo _tmp1BA;struct Cyc_List_List*
_tmp1BB;struct Cyc_List_List*_tmp1BC;void*_tmp1BD;struct Cyc_Absyn_FnInfo _tmp1BE;
struct Cyc_List_List*_tmp1BF;struct Cyc_Core_Opt*_tmp1C0;void*_tmp1C1;struct Cyc_List_List*
_tmp1C2;struct Cyc_Absyn_VarargInfo*_tmp1C3;struct Cyc_List_List*_tmp1C4;void*
_tmp1C5;struct Cyc_List_List*_tmp1C6;_LL14A: if((int)_tmp1AC != 0)goto _LL14C;_LL14B:
goto _LL14D;_LL14C: if((int)_tmp1AC != 1)goto _LL14E;_LL14D: goto _LL14F;_LL14E: if(
_tmp1AC <= (void*)4  || *((int*)_tmp1AC)!= 6)goto _LL150;_LL14F: goto _LL151;_LL150:
if(_tmp1AC <= (void*)4  || *((int*)_tmp1AC)!= 12)goto _LL152;_LL151: goto _LL153;
_LL152: if(_tmp1AC <= (void*)4  || *((int*)_tmp1AC)!= 13)goto _LL154;_LL153: goto
_LL155;_LL154: if(_tmp1AC <= (void*)4  || *((int*)_tmp1AC)!= 18)goto _LL156;_LL155:
goto _LL157;_LL156: if(_tmp1AC <= (void*)4  || *((int*)_tmp1AC)!= 5)goto _LL158;
_LL157: return Cyc_Absyn_empty_effect;_LL158: if(_tmp1AC <= (void*)4  || *((int*)
_tmp1AC)!= 0)goto _LL15A;_LL159: goto _LL15B;_LL15A: if(_tmp1AC <= (void*)4  || *((int*)
_tmp1AC)!= 1)goto _LL15C;_LL15B: {void*_tmp1C7=Cyc_Tcutil_typ_kind(t);_LL17F: if((
int)_tmp1C7 != 3)goto _LL181;_LL180: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1C8=_cycalloc(sizeof(*_tmp1C8));_tmp1C8[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1C9;_tmp1C9.tag=19;_tmp1C9.f1=(void*)t;_tmp1C9;});_tmp1C8;});_LL181: if((int)
_tmp1C7 != 4)goto _LL183;_LL182: return t;_LL183: if((int)_tmp1C7 != 5)goto _LL185;
_LL184: return Cyc_Absyn_empty_effect;_LL185:;_LL186: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1CB;_tmp1CB.tag=21;_tmp1CB.f1=(void*)t;_tmp1CB;});_tmp1CA;});_LL17E:;}_LL15C:
if(_tmp1AC <= (void*)4  || *((int*)_tmp1AC)!= 15)goto _LL15E;_tmp1AD=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp1AC)->f1;_LL15D: return(void*)({struct
Cyc_Absyn_AccessEff_struct*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp1CD;_tmp1CD.tag=19;_tmp1CD.f1=(void*)_tmp1AD;
_tmp1CD;});_tmp1CC;});_LL15E: if(_tmp1AC <= (void*)4  || *((int*)_tmp1AC)!= 2)goto
_LL160;_tmp1AE=((struct Cyc_Absyn_TunionType_struct*)_tmp1AC)->f1;_tmp1AF=_tmp1AE.targs;
_tmp1B0=(void*)_tmp1AE.rgn;_LL15F: {struct Cyc_List_List*ts=({struct Cyc_List_List*
_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1D2;_tmp1D2.tag=19;_tmp1D2.f1=(void*)_tmp1B0;_tmp1D2;});_tmp1D1;}));_tmp1D0->tl=
Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1AF);_tmp1D0;});return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));
_tmp1CE[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1CF;_tmp1CF.tag=20;_tmp1CF.f1=ts;
_tmp1CF;});_tmp1CE;}));}_LL160: if(_tmp1AC <= (void*)4  || *((int*)_tmp1AC)!= 4)
goto _LL162;_tmp1B1=((struct Cyc_Absyn_PointerType_struct*)_tmp1AC)->f1;_tmp1B2=(
void*)_tmp1B1.elt_typ;_tmp1B3=_tmp1B1.ptr_atts;_tmp1B4=(void*)_tmp1B3.rgn;_LL161:
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1D4;_tmp1D4.tag=20;_tmp1D4.f1=({void*_tmp1D5[2];_tmp1D5[1]=Cyc_Tcutil_rgns_of(
_tmp1B2);_tmp1D5[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1D6=_cycalloc(
sizeof(*_tmp1D6));_tmp1D6[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1D7;_tmp1D7.tag=
19;_tmp1D7.f1=(void*)_tmp1B4;_tmp1D7;});_tmp1D6;});Cyc_List_list(_tag_arr(
_tmp1D5,sizeof(void*),2));});_tmp1D4;});_tmp1D3;}));_LL162: if(_tmp1AC <= (void*)4
 || *((int*)_tmp1AC)!= 7)goto _LL164;_tmp1B5=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1AC)->f1;_tmp1B6=(void*)_tmp1B5.elt_type;_LL163: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp1B6));_LL164: if(_tmp1AC <= (void*)4  || *((int*)_tmp1AC)!= 
9)goto _LL166;_tmp1B7=((struct Cyc_Absyn_TupleType_struct*)_tmp1AC)->f1;_LL165: {
struct Cyc_List_List*_tmp1D8=0;for(0;_tmp1B7 != 0;_tmp1B7=_tmp1B7->tl){_tmp1D8=({
struct Cyc_List_List*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->hd=(void*)(*((
struct _tuple4*)_tmp1B7->hd)).f2;_tmp1D9->tl=_tmp1D8;_tmp1D9;});}_tmp1B9=_tmp1D8;
goto _LL167;}_LL166: if(_tmp1AC <= (void*)4  || *((int*)_tmp1AC)!= 3)goto _LL168;
_tmp1B8=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1AC)->f1;_tmp1B9=_tmp1B8.targs;
_LL167: _tmp1BB=_tmp1B9;goto _LL169;_LL168: if(_tmp1AC <= (void*)4  || *((int*)
_tmp1AC)!= 10)goto _LL16A;_tmp1BA=((struct Cyc_Absyn_AggrType_struct*)_tmp1AC)->f1;
_tmp1BB=_tmp1BA.targs;_LL169: return Cyc_Tcutil_normalize_effect((void*)({struct
Cyc_Absyn_JoinEff_struct*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA[0]=({struct
Cyc_Absyn_JoinEff_struct _tmp1DB;_tmp1DB.tag=20;_tmp1DB.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1BB);_tmp1DB;});_tmp1DA;}));_LL16A: if(_tmp1AC <= (void*)4  || *((int*)_tmp1AC)
!= 11)goto _LL16C;_tmp1BC=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1AC)->f2;
_LL16B: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1DD;_tmp1DD.tag=20;_tmp1DD.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1BC);_tmp1DD;});
_tmp1DC;}));_LL16C: if(_tmp1AC <= (void*)4  || *((int*)_tmp1AC)!= 14)goto _LL16E;
_tmp1BD=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp1AC)->f1;_LL16D: return
Cyc_Tcutil_rgns_of(_tmp1BD);_LL16E: if(_tmp1AC <= (void*)4  || *((int*)_tmp1AC)!= 
17)goto _LL170;_LL16F: return Cyc_Tcutil_rgns_of(t);_LL170: if(_tmp1AC <= (void*)4
 || *((int*)_tmp1AC)!= 8)goto _LL172;_tmp1BE=((struct Cyc_Absyn_FnType_struct*)
_tmp1AC)->f1;_tmp1BF=_tmp1BE.tvars;_tmp1C0=_tmp1BE.effect;_tmp1C1=(void*)_tmp1BE.ret_typ;
_tmp1C2=_tmp1BE.args;_tmp1C3=_tmp1BE.cyc_varargs;_tmp1C4=_tmp1BE.rgn_po;_LL171: {
void*_tmp1DE=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1BF),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1C0))->v);return Cyc_Tcutil_normalize_effect(
_tmp1DE);}_LL172: if((int)_tmp1AC != 3)goto _LL174;_LL173: goto _LL175;_LL174: if((int)
_tmp1AC != 2)goto _LL176;_LL175: return Cyc_Absyn_empty_effect;_LL176: if(_tmp1AC <= (
void*)4  || *((int*)_tmp1AC)!= 19)goto _LL178;_LL177: goto _LL179;_LL178: if(_tmp1AC
<= (void*)4  || *((int*)_tmp1AC)!= 20)goto _LL17A;_LL179: return t;_LL17A: if(_tmp1AC
<= (void*)4  || *((int*)_tmp1AC)!= 21)goto _LL17C;_tmp1C5=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1AC)->f1;_LL17B: return Cyc_Tcutil_rgns_of(_tmp1C5);_LL17C: if(_tmp1AC <= (void*)
4  || *((int*)_tmp1AC)!= 16)goto _LL149;_tmp1C6=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1AC)->f2;_LL17D: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1E0;_tmp1E0.tag=20;_tmp1E0.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1C6);
_tmp1E0;});_tmp1DF;}));_LL149:;}void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp1E1=e;struct Cyc_List_List*_tmp1E2;struct Cyc_List_List**_tmp1E3;void*
_tmp1E4;_LL188: if(_tmp1E1 <= (void*)4  || *((int*)_tmp1E1)!= 20)goto _LL18A;_tmp1E2=((
struct Cyc_Absyn_JoinEff_struct*)_tmp1E1)->f1;_tmp1E3=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp1E1)->f1;_LL189: {int redo_join=0;{struct Cyc_List_List*
effs=*_tmp1E3;for(0;effs != 0;effs=effs->tl){void*_tmp1E5=(void*)effs->hd;(void*)(
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp1E5)));{void*
_tmp1E6=(void*)effs->hd;void*_tmp1E7;_LL18F: if(_tmp1E6 <= (void*)4  || *((int*)
_tmp1E6)!= 20)goto _LL191;_LL190: goto _LL192;_LL191: if(_tmp1E6 <= (void*)4  || *((
int*)_tmp1E6)!= 19)goto _LL193;_tmp1E7=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1E6)->f1;if((int)_tmp1E7 != 2)goto _LL193;_LL192: redo_join=1;goto _LL18E;_LL193:;
_LL194: goto _LL18E;_LL18E:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=
0;{struct Cyc_List_List*effs=*_tmp1E3;for(0;effs != 0;effs=effs->tl){void*_tmp1E8=
Cyc_Tcutil_compress((void*)effs->hd);struct Cyc_List_List*_tmp1E9;void*_tmp1EA;
_LL196: if(_tmp1E8 <= (void*)4  || *((int*)_tmp1E8)!= 20)goto _LL198;_tmp1E9=((
struct Cyc_Absyn_JoinEff_struct*)_tmp1E8)->f1;_LL197: effects=Cyc_List_revappend(
_tmp1E9,effects);goto _LL195;_LL198: if(_tmp1E8 <= (void*)4  || *((int*)_tmp1E8)!= 
19)goto _LL19A;_tmp1EA=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1E8)->f1;
if((int)_tmp1EA != 2)goto _LL19A;_LL199: goto _LL195;_LL19A:;_LL19B: effects=({struct
Cyc_List_List*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB->hd=(void*)_tmp1E8;
_tmp1EB->tl=effects;_tmp1EB;});goto _LL195;_LL195:;}}*_tmp1E3=Cyc_List_imp_rev(
effects);return e;}}_LL18A: if(_tmp1E1 <= (void*)4  || *((int*)_tmp1E1)!= 21)goto
_LL18C;_tmp1E4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1E1)->f1;_LL18B: {
void*_tmp1EC=Cyc_Tcutil_compress(_tmp1E4);_LL19D: if(_tmp1EC <= (void*)4  || *((int*)
_tmp1EC)!= 0)goto _LL19F;_LL19E: goto _LL1A0;_LL19F: if(_tmp1EC <= (void*)4  || *((int*)
_tmp1EC)!= 1)goto _LL1A1;_LL1A0: return e;_LL1A1:;_LL1A2: return Cyc_Tcutil_rgns_of(
_tmp1E4);_LL19C:;}_LL18C:;_LL18D: return e;_LL187:;}}static struct Cyc_Core_Opt Cyc_Tcutil_ek={(
void*)((void*)4)};static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct*
_tmp1ED=({struct Cyc_Absyn_FnType_struct*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));
_tmp1EE[0]=({struct Cyc_Absyn_FnType_struct _tmp1EF;_tmp1EF.tag=8;_tmp1EF.f1=({
struct Cyc_Absyn_FnInfo _tmp1F0;_tmp1F0.tvars=0;_tmp1F0.effect=({struct Cyc_Core_Opt*
_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1->v=(void*)eff;_tmp1F1;});_tmp1F0.ret_typ=(
void*)((void*)0);_tmp1F0.args=0;_tmp1F0.c_varargs=0;_tmp1F0.cyc_varargs=0;
_tmp1F0.rgn_po=0;_tmp1F0.attributes=0;_tmp1F0;});_tmp1EF;});_tmp1EE;});return Cyc_Absyn_atb_typ((
void*)_tmp1ED,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){r=Cyc_Tcutil_compress(
r);if(r == (void*)2)return 1;{void*_tmp1F2=Cyc_Tcutil_compress(e);void*_tmp1F3;
struct Cyc_List_List*_tmp1F4;void*_tmp1F5;struct Cyc_Core_Opt*_tmp1F6;struct Cyc_Core_Opt*
_tmp1F7;struct Cyc_Core_Opt**_tmp1F8;struct Cyc_Core_Opt*_tmp1F9;_LL1A4: if(_tmp1F2
<= (void*)4  || *((int*)_tmp1F2)!= 19)goto _LL1A6;_tmp1F3=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1F2)->f1;_LL1A5: if(constrain)return Cyc_Tcutil_unify(r,_tmp1F3);_tmp1F3=Cyc_Tcutil_compress(
_tmp1F3);if(r == _tmp1F3)return 1;{struct _tuple6 _tmp1FB=({struct _tuple6 _tmp1FA;
_tmp1FA.f1=r;_tmp1FA.f2=_tmp1F3;_tmp1FA;});void*_tmp1FC;struct Cyc_Absyn_Tvar*
_tmp1FD;void*_tmp1FE;struct Cyc_Absyn_Tvar*_tmp1FF;_LL1AF: _tmp1FC=_tmp1FB.f1;if(
_tmp1FC <= (void*)4  || *((int*)_tmp1FC)!= 1)goto _LL1B1;_tmp1FD=((struct Cyc_Absyn_VarType_struct*)
_tmp1FC)->f1;_tmp1FE=_tmp1FB.f2;if(_tmp1FE <= (void*)4  || *((int*)_tmp1FE)!= 1)
goto _LL1B1;_tmp1FF=((struct Cyc_Absyn_VarType_struct*)_tmp1FE)->f1;_LL1B0: return
Cyc_Absyn_tvar_cmp(_tmp1FD,_tmp1FF)== 0;_LL1B1:;_LL1B2: return 0;_LL1AE:;}_LL1A6:
if(_tmp1F2 <= (void*)4  || *((int*)_tmp1F2)!= 20)goto _LL1A8;_tmp1F4=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1F2)->f1;_LL1A7: for(0;_tmp1F4 != 0;_tmp1F4=_tmp1F4->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp1F4->hd))return 1;}return 0;_LL1A8: if(_tmp1F2 <= (void*)4
 || *((int*)_tmp1F2)!= 21)goto _LL1AA;_tmp1F5=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1F2)->f1;_LL1A9: {void*_tmp200=Cyc_Tcutil_rgns_of(_tmp1F5);void*_tmp201;
_LL1B4: if(_tmp200 <= (void*)4  || *((int*)_tmp200)!= 21)goto _LL1B6;_tmp201=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp200)->f1;_LL1B5: if(!constrain)return 0;{void*
_tmp202=Cyc_Tcutil_compress(_tmp201);struct Cyc_Core_Opt*_tmp203;struct Cyc_Core_Opt*
_tmp204;struct Cyc_Core_Opt**_tmp205;struct Cyc_Core_Opt*_tmp206;_LL1B9: if(_tmp202
<= (void*)4  || *((int*)_tmp202)!= 0)goto _LL1BB;_tmp203=((struct Cyc_Absyn_Evar_struct*)
_tmp202)->f1;_tmp204=((struct Cyc_Absyn_Evar_struct*)_tmp202)->f2;_tmp205=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp202)->f2;_tmp206=((struct Cyc_Absyn_Evar_struct*)
_tmp202)->f4;_LL1BA: {void*_tmp207=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp206);Cyc_Tcutil_occurs(_tmp207,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp206))->v,r);{void*_tmp208=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp20A=_cycalloc(sizeof(*_tmp20A));
_tmp20A[0]=({struct Cyc_Absyn_JoinEff_struct _tmp20B;_tmp20B.tag=20;_tmp20B.f1=({
void*_tmp20C[2];_tmp20C[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp20D=
_cycalloc(sizeof(*_tmp20D));_tmp20D[0]=({struct Cyc_Absyn_AccessEff_struct _tmp20E;
_tmp20E.tag=19;_tmp20E.f1=(void*)r;_tmp20E;});_tmp20D;});_tmp20C[0]=_tmp207;Cyc_List_list(
_tag_arr(_tmp20C,sizeof(void*),2));});_tmp20B;});_tmp20A;}));*_tmp205=({struct
Cyc_Core_Opt*_tmp209=_cycalloc(sizeof(*_tmp209));_tmp209->v=(void*)_tmp208;
_tmp209;});return 1;}}_LL1BB:;_LL1BC: return 0;_LL1B8:;}_LL1B6:;_LL1B7: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp200);_LL1B3:;}_LL1AA: if(_tmp1F2 <= (void*)4  || *((int*)_tmp1F2)!= 
0)goto _LL1AC;_tmp1F6=((struct Cyc_Absyn_Evar_struct*)_tmp1F2)->f1;_tmp1F7=((
struct Cyc_Absyn_Evar_struct*)_tmp1F2)->f2;_tmp1F8=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1F2)->f2;_tmp1F9=((struct Cyc_Absyn_Evar_struct*)
_tmp1F2)->f4;_LL1AB: if(_tmp1F6 == 0  || (void*)_tmp1F6->v != (void*)4)({void*
_tmp20F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp210="effect evar has wrong kind";_tag_arr(_tmp210,sizeof(char),
_get_zero_arr_size(_tmp210,27));}),_tag_arr(_tmp20F,sizeof(void*),0));});if(!
constrain)return 0;{void*_tmp211=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1F9);Cyc_Tcutil_occurs(_tmp211,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1F9))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp212=({struct Cyc_Absyn_JoinEff_struct*_tmp214=_cycalloc(sizeof(*_tmp214));
_tmp214[0]=({struct Cyc_Absyn_JoinEff_struct _tmp215;_tmp215.tag=20;_tmp215.f1=({
struct Cyc_List_List*_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216->hd=(void*)
_tmp211;_tmp216->tl=({struct Cyc_List_List*_tmp217=_cycalloc(sizeof(*_tmp217));
_tmp217->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp218=_cycalloc(
sizeof(*_tmp218));_tmp218[0]=({struct Cyc_Absyn_AccessEff_struct _tmp219;_tmp219.tag=
19;_tmp219.f1=(void*)r;_tmp219;});_tmp218;}));_tmp217->tl=0;_tmp217;});_tmp216;});
_tmp215;});_tmp214;});*_tmp1F8=({struct Cyc_Core_Opt*_tmp213=_cycalloc(sizeof(*
_tmp213));_tmp213->v=(void*)((void*)_tmp212);_tmp213;});return 1;}}_LL1AC:;_LL1AD:
return 0;_LL1A3:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp21A=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp21B;void*_tmp21C;struct Cyc_Core_Opt*_tmp21D;struct
Cyc_Core_Opt*_tmp21E;struct Cyc_Core_Opt**_tmp21F;struct Cyc_Core_Opt*_tmp220;
_LL1BE: if(_tmp21A <= (void*)4  || *((int*)_tmp21A)!= 19)goto _LL1C0;_LL1BF: return 0;
_LL1C0: if(_tmp21A <= (void*)4  || *((int*)_tmp21A)!= 20)goto _LL1C2;_tmp21B=((
struct Cyc_Absyn_JoinEff_struct*)_tmp21A)->f1;_LL1C1: for(0;_tmp21B != 0;_tmp21B=
_tmp21B->tl){if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp21B->hd))
return 1;}return 0;_LL1C2: if(_tmp21A <= (void*)4  || *((int*)_tmp21A)!= 21)goto
_LL1C4;_tmp21C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp21A)->f1;_LL1C3:
_tmp21C=Cyc_Tcutil_compress(_tmp21C);if(t == _tmp21C)return 1;if(
may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp21C);{void*_tmp221=Cyc_Tcutil_rgns_of(
t);void*_tmp222;_LL1C9: if(_tmp221 <= (void*)4  || *((int*)_tmp221)!= 21)goto _LL1CB;
_tmp222=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp221)->f1;_LL1CA: {struct
_tuple6 _tmp224=({struct _tuple6 _tmp223;_tmp223.f1=Cyc_Tcutil_compress(_tmp222);
_tmp223.f2=_tmp21C;_tmp223;});void*_tmp225;struct Cyc_Absyn_Tvar*_tmp226;void*
_tmp227;struct Cyc_Absyn_Tvar*_tmp228;_LL1CE: _tmp225=_tmp224.f1;if(_tmp225 <= (
void*)4  || *((int*)_tmp225)!= 1)goto _LL1D0;_tmp226=((struct Cyc_Absyn_VarType_struct*)
_tmp225)->f1;_tmp227=_tmp224.f2;if(_tmp227 <= (void*)4  || *((int*)_tmp227)!= 1)
goto _LL1D0;_tmp228=((struct Cyc_Absyn_VarType_struct*)_tmp227)->f1;_LL1CF: return
Cyc_Tcutil_unify(t,_tmp21C);_LL1D0:;_LL1D1: return _tmp222 == _tmp21C;_LL1CD:;}
_LL1CB:;_LL1CC: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp221);
_LL1C8:;}_LL1C4: if(_tmp21A <= (void*)4  || *((int*)_tmp21A)!= 0)goto _LL1C6;_tmp21D=((
struct Cyc_Absyn_Evar_struct*)_tmp21A)->f1;_tmp21E=((struct Cyc_Absyn_Evar_struct*)
_tmp21A)->f2;_tmp21F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp21A)->f2;_tmp220=((struct Cyc_Absyn_Evar_struct*)_tmp21A)->f4;_LL1C5: if(
_tmp21D == 0  || (void*)_tmp21D->v != (void*)4)({void*_tmp229[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp22A="effect evar has wrong kind";
_tag_arr(_tmp22A,sizeof(char),_get_zero_arr_size(_tmp22A,27));}),_tag_arr(
_tmp229,sizeof(void*),0));});if(!may_constrain_evars)return 0;{void*_tmp22B=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp220);Cyc_Tcutil_occurs(_tmp22B,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp220))->v,t);{struct
Cyc_Absyn_JoinEff_struct*_tmp22C=({struct Cyc_Absyn_JoinEff_struct*_tmp22E=
_cycalloc(sizeof(*_tmp22E));_tmp22E[0]=({struct Cyc_Absyn_JoinEff_struct _tmp22F;
_tmp22F.tag=20;_tmp22F.f1=({struct Cyc_List_List*_tmp230=_cycalloc(sizeof(*
_tmp230));_tmp230->hd=(void*)_tmp22B;_tmp230->tl=({struct Cyc_List_List*_tmp231=
_cycalloc(sizeof(*_tmp231));_tmp231->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp233;_tmp233.tag=21;_tmp233.f1=(void*)t;_tmp233;});_tmp232;}));_tmp231->tl=0;
_tmp231;});_tmp230;});_tmp22F;});_tmp22E;});*_tmp21F=({struct Cyc_Core_Opt*
_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->v=(void*)((void*)_tmp22C);_tmp22D;});
return 1;}}_LL1C6:;_LL1C7: return 0;_LL1BD:;}}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp234=e;struct Cyc_Absyn_Tvar*_tmp235;struct Cyc_List_List*_tmp236;void*
_tmp237;struct Cyc_Core_Opt*_tmp238;struct Cyc_Core_Opt*_tmp239;struct Cyc_Core_Opt**
_tmp23A;struct Cyc_Core_Opt*_tmp23B;_LL1D3: if(_tmp234 <= (void*)4  || *((int*)
_tmp234)!= 1)goto _LL1D5;_tmp235=((struct Cyc_Absyn_VarType_struct*)_tmp234)->f1;
_LL1D4: return Cyc_Absyn_tvar_cmp(v,_tmp235)== 0;_LL1D5: if(_tmp234 <= (void*)4  || *((
int*)_tmp234)!= 20)goto _LL1D7;_tmp236=((struct Cyc_Absyn_JoinEff_struct*)_tmp234)->f1;
_LL1D6: for(0;_tmp236 != 0;_tmp236=_tmp236->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp236->hd))return 1;}return 0;_LL1D7: if(_tmp234 <= (
void*)4  || *((int*)_tmp234)!= 21)goto _LL1D9;_tmp237=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp234)->f1;_LL1D8: {void*_tmp23C=Cyc_Tcutil_rgns_of(_tmp237);void*_tmp23D;
_LL1DE: if(_tmp23C <= (void*)4  || *((int*)_tmp23C)!= 21)goto _LL1E0;_tmp23D=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp23C)->f1;_LL1DF: if(!may_constrain_evars)
return 0;{void*_tmp23E=Cyc_Tcutil_compress(_tmp23D);struct Cyc_Core_Opt*_tmp23F;
struct Cyc_Core_Opt*_tmp240;struct Cyc_Core_Opt**_tmp241;struct Cyc_Core_Opt*
_tmp242;_LL1E3: if(_tmp23E <= (void*)4  || *((int*)_tmp23E)!= 0)goto _LL1E5;_tmp23F=((
struct Cyc_Absyn_Evar_struct*)_tmp23E)->f1;_tmp240=((struct Cyc_Absyn_Evar_struct*)
_tmp23E)->f2;_tmp241=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp23E)->f2;_tmp242=((struct Cyc_Absyn_Evar_struct*)_tmp23E)->f4;_LL1E4: {void*
_tmp243=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp242);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp242))->v,v))return 0;{void*_tmp244=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp246=_cycalloc(sizeof(*_tmp246));
_tmp246[0]=({struct Cyc_Absyn_JoinEff_struct _tmp247;_tmp247.tag=20;_tmp247.f1=({
void*_tmp248[2];_tmp248[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp249=
_cycalloc(sizeof(*_tmp249));_tmp249[0]=({struct Cyc_Absyn_VarType_struct _tmp24A;
_tmp24A.tag=1;_tmp24A.f1=v;_tmp24A;});_tmp249;});_tmp248[0]=_tmp243;Cyc_List_list(
_tag_arr(_tmp248,sizeof(void*),2));});_tmp247;});_tmp246;}));*_tmp241=({struct
Cyc_Core_Opt*_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245->v=(void*)_tmp244;
_tmp245;});return 1;}}_LL1E5:;_LL1E6: return 0;_LL1E2:;}_LL1E0:;_LL1E1: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp23C);_LL1DD:;}_LL1D9: if(_tmp234 <= (void*)4  || *((int*)
_tmp234)!= 0)goto _LL1DB;_tmp238=((struct Cyc_Absyn_Evar_struct*)_tmp234)->f1;
_tmp239=((struct Cyc_Absyn_Evar_struct*)_tmp234)->f2;_tmp23A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp234)->f2;_tmp23B=((struct Cyc_Absyn_Evar_struct*)
_tmp234)->f4;_LL1DA: if(_tmp238 == 0  || (void*)_tmp238->v != (void*)4)({void*
_tmp24B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp24C="effect evar has wrong kind";_tag_arr(_tmp24C,sizeof(char),
_get_zero_arr_size(_tmp24C,27));}),_tag_arr(_tmp24B,sizeof(void*),0));});{void*
_tmp24D=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp23B);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp23B))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*
_tmp24E=({struct Cyc_Absyn_JoinEff_struct*_tmp250=_cycalloc(sizeof(*_tmp250));
_tmp250[0]=({struct Cyc_Absyn_JoinEff_struct _tmp251;_tmp251.tag=20;_tmp251.f1=({
struct Cyc_List_List*_tmp252=_cycalloc(sizeof(*_tmp252));_tmp252->hd=(void*)
_tmp24D;_tmp252->tl=({struct Cyc_List_List*_tmp253=_cycalloc(sizeof(*_tmp253));
_tmp253->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp254=_cycalloc(
sizeof(*_tmp254));_tmp254[0]=({struct Cyc_Absyn_VarType_struct _tmp255;_tmp255.tag=
1;_tmp255.f1=v;_tmp255;});_tmp254;}));_tmp253->tl=0;_tmp253;});_tmp252;});
_tmp251;});_tmp250;});*_tmp23A=({struct Cyc_Core_Opt*_tmp24F=_cycalloc(sizeof(*
_tmp24F));_tmp24F->v=(void*)((void*)_tmp24E);_tmp24F;});return 1;}}_LL1DB:;_LL1DC:
return 0;_LL1D2:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp256=e;struct Cyc_List_List*_tmp257;void*_tmp258;_LL1E8: if(_tmp256 <= (
void*)4  || *((int*)_tmp256)!= 20)goto _LL1EA;_tmp257=((struct Cyc_Absyn_JoinEff_struct*)
_tmp256)->f1;_LL1E9: for(0;_tmp257 != 0;_tmp257=_tmp257->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp257->hd))return 1;}return 0;_LL1EA: if(_tmp256 <= (void*)4  || *((int*)
_tmp256)!= 21)goto _LL1EC;_tmp258=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp256)->f1;_LL1EB: {void*_tmp259=Cyc_Tcutil_rgns_of(_tmp258);void*_tmp25A;
_LL1F1: if(_tmp259 <= (void*)4  || *((int*)_tmp259)!= 21)goto _LL1F3;_tmp25A=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp259)->f1;_LL1F2: return 0;_LL1F3:;_LL1F4:
return Cyc_Tcutil_evar_in_effect(evar,_tmp259);_LL1F0:;}_LL1EC: if(_tmp256 <= (void*)
4  || *((int*)_tmp256)!= 0)goto _LL1EE;_LL1ED: return evar == e;_LL1EE:;_LL1EF: return
0;_LL1E7:;}}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
void*_tmp25B=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp25C;void*_tmp25D;
struct Cyc_Absyn_Tvar*_tmp25E;void*_tmp25F;struct Cyc_Core_Opt*_tmp260;struct Cyc_Core_Opt**
_tmp261;struct Cyc_Core_Opt*_tmp262;_LL1F6: if(_tmp25B <= (void*)4  || *((int*)
_tmp25B)!= 20)goto _LL1F8;_tmp25C=((struct Cyc_Absyn_JoinEff_struct*)_tmp25B)->f1;
_LL1F7: for(0;_tmp25C != 0;_tmp25C=_tmp25C->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp25C->hd,e2))return 0;}return 1;_LL1F8: if(_tmp25B <= (
void*)4  || *((int*)_tmp25B)!= 19)goto _LL1FA;_tmp25D=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp25B)->f1;_LL1F9: return Cyc_Tcutil_region_in_effect(0,_tmp25D,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp25D,(void*)2);_LL1FA: if(_tmp25B <= (
void*)4  || *((int*)_tmp25B)!= 1)goto _LL1FC;_tmp25E=((struct Cyc_Absyn_VarType_struct*)
_tmp25B)->f1;_LL1FB: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,
_tmp25E,e2);_LL1FC: if(_tmp25B <= (void*)4  || *((int*)_tmp25B)!= 21)goto _LL1FE;
_tmp25F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp25B)->f1;_LL1FD: {void*
_tmp263=Cyc_Tcutil_rgns_of(_tmp25F);void*_tmp264;_LL203: if(_tmp263 <= (void*)4
 || *((int*)_tmp263)!= 21)goto _LL205;_tmp264=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp263)->f1;_LL204: return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp264,
e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp264,Cyc_Absyn_sint_typ);
_LL205:;_LL206: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp263,e2);
_LL202:;}_LL1FE: if(_tmp25B <= (void*)4  || *((int*)_tmp25B)!= 0)goto _LL200;_tmp260=((
struct Cyc_Absyn_Evar_struct*)_tmp25B)->f2;_tmp261=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp25B)->f2;_tmp262=((struct Cyc_Absyn_Evar_struct*)
_tmp25B)->f4;_LL1FF: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp261=({struct Cyc_Core_Opt*
_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265->v=(void*)Cyc_Absyn_empty_effect;
_tmp265;});return 1;_LL200:;_LL201:({struct Cyc_String_pa_struct _tmp268;_tmp268.tag=
0;_tmp268.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(e1));{
void*_tmp266[1]={& _tmp268};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp267="subset_effect: bad effect: %s";_tag_arr(
_tmp267,sizeof(char),_get_zero_arr_size(_tmp267,30));}),_tag_arr(_tmp266,sizeof(
void*),1));}});_LL1F5:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && 
Cyc_Tcutil_subset_effect(0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)
 && Cyc_Tcutil_subset_effect(1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple6 _tmp26A;void*_tmp26B;void*_tmp26C;struct
_tuple6*_tmp269=(struct _tuple6*)r1->hd;_tmp26A=*_tmp269;_tmp26B=_tmp26A.f1;
_tmp26C=_tmp26A.f2;{int found=_tmp26B == (void*)2;{struct Cyc_List_List*r2=rpo2;
for(0;r2 != 0  && !found;r2=r2->tl){struct _tuple6 _tmp26E;void*_tmp26F;void*_tmp270;
struct _tuple6*_tmp26D=(struct _tuple6*)r2->hd;_tmp26E=*_tmp26D;_tmp26F=_tmp26E.f1;
_tmp270=_tmp26E.f2;if(Cyc_Tcutil_unify(_tmp26B,_tmp26F) && Cyc_Tcutil_unify(
_tmp26C,_tmp270)){found=1;break;}}}if(!found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple11{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _tagged_arr)
_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)
return;{void*_tmp271=t1;struct Cyc_Core_Opt*_tmp272;struct Cyc_Core_Opt*_tmp273;
struct Cyc_Core_Opt**_tmp274;struct Cyc_Core_Opt*_tmp275;_LL208: if(_tmp271 <= (void*)
4  || *((int*)_tmp271)!= 0)goto _LL20A;_tmp272=((struct Cyc_Absyn_Evar_struct*)
_tmp271)->f1;_tmp273=((struct Cyc_Absyn_Evar_struct*)_tmp271)->f2;_tmp274=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp271)->f2;_tmp275=((struct Cyc_Absyn_Evar_struct*)
_tmp271)->f4;_LL209: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp275))->v,t2);{void*_tmp276=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp276,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp272))->v)){*_tmp274=({struct Cyc_Core_Opt*_tmp277=_cycalloc(sizeof(*_tmp277));
_tmp277->v=(void*)t2;_tmp277;});return;}else{{void*_tmp278=t2;struct Cyc_Core_Opt*
_tmp279;struct Cyc_Core_Opt**_tmp27A;struct Cyc_Core_Opt*_tmp27B;struct Cyc_Absyn_PtrInfo
_tmp27C;_LL20D: if(_tmp278 <= (void*)4  || *((int*)_tmp278)!= 0)goto _LL20F;_tmp279=((
struct Cyc_Absyn_Evar_struct*)_tmp278)->f2;_tmp27A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp278)->f2;_tmp27B=((struct Cyc_Absyn_Evar_struct*)
_tmp278)->f4;_LL20E: {struct Cyc_List_List*_tmp27D=(struct Cyc_List_List*)_tmp275->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp27B))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp27D,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp27E="(type variable would escape scope)";_tag_arr(_tmp27E,sizeof(
char),_get_zero_arr_size(_tmp27E,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}}}
if(Cyc_Tcutil_kind_leq((void*)_tmp272->v,_tmp276)){*_tmp27A=({struct Cyc_Core_Opt*
_tmp27F=_cycalloc(sizeof(*_tmp27F));_tmp27F->v=(void*)t1;_tmp27F;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp280="(kinds are incompatible)";_tag_arr(_tmp280,sizeof(char),
_get_zero_arr_size(_tmp280,25));});goto _LL20C;}_LL20F: if(_tmp278 <= (void*)4  || *((
int*)_tmp278)!= 4)goto _LL211;_tmp27C=((struct Cyc_Absyn_PointerType_struct*)
_tmp278)->f1;if(!((void*)_tmp272->v == (void*)2))goto _LL211;_LL210: {struct Cyc_Absyn_Conref*
_tmp281=Cyc_Absyn_compress_conref((_tmp27C.ptr_atts).bounds);{void*_tmp282=(void*)
_tmp281->v;_LL214: if((int)_tmp282 != 0)goto _LL216;_LL215:(void*)(_tmp281->v=(void*)((
void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp283=_cycalloc(sizeof(*_tmp283));
_tmp283[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp284;_tmp284.tag=0;_tmp284.f1=(
void*)Cyc_Absyn_bounds_one;_tmp284;});_tmp283;})));*_tmp274=({struct Cyc_Core_Opt*
_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285->v=(void*)t2;_tmp285;});return;
_LL216:;_LL217: goto _LL213;_LL213:;}goto _LL20C;}_LL211:;_LL212: goto _LL20C;_LL20C:;}
Cyc_Tcutil_failure_reason=({const char*_tmp286="(kinds are incompatible)";
_tag_arr(_tmp286,sizeof(char),_get_zero_arr_size(_tmp286,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}_LL20A:;_LL20B: goto _LL207;_LL207:;}{struct _tuple6
_tmp288=({struct _tuple6 _tmp287;_tmp287.f1=t2;_tmp287.f2=t1;_tmp287;});void*
_tmp289;void*_tmp28A;void*_tmp28B;void*_tmp28C;struct Cyc_Absyn_Tvar*_tmp28D;void*
_tmp28E;struct Cyc_Absyn_Tvar*_tmp28F;void*_tmp290;struct Cyc_Absyn_AggrInfo
_tmp291;void*_tmp292;struct Cyc_List_List*_tmp293;void*_tmp294;struct Cyc_Absyn_AggrInfo
_tmp295;void*_tmp296;struct Cyc_List_List*_tmp297;void*_tmp298;struct _tuple1*
_tmp299;void*_tmp29A;struct _tuple1*_tmp29B;void*_tmp29C;struct Cyc_List_List*
_tmp29D;void*_tmp29E;struct Cyc_List_List*_tmp29F;void*_tmp2A0;struct Cyc_Absyn_TunionInfo
_tmp2A1;void*_tmp2A2;struct Cyc_Absyn_Tuniondecl**_tmp2A3;struct Cyc_Absyn_Tuniondecl*
_tmp2A4;struct Cyc_List_List*_tmp2A5;void*_tmp2A6;void*_tmp2A7;struct Cyc_Absyn_TunionInfo
_tmp2A8;void*_tmp2A9;struct Cyc_Absyn_Tuniondecl**_tmp2AA;struct Cyc_Absyn_Tuniondecl*
_tmp2AB;struct Cyc_List_List*_tmp2AC;void*_tmp2AD;void*_tmp2AE;struct Cyc_Absyn_TunionFieldInfo
_tmp2AF;void*_tmp2B0;struct Cyc_Absyn_Tuniondecl*_tmp2B1;struct Cyc_Absyn_Tunionfield*
_tmp2B2;struct Cyc_List_List*_tmp2B3;void*_tmp2B4;struct Cyc_Absyn_TunionFieldInfo
_tmp2B5;void*_tmp2B6;struct Cyc_Absyn_Tuniondecl*_tmp2B7;struct Cyc_Absyn_Tunionfield*
_tmp2B8;struct Cyc_List_List*_tmp2B9;void*_tmp2BA;struct Cyc_Absyn_PtrInfo _tmp2BB;
void*_tmp2BC;struct Cyc_Absyn_Tqual _tmp2BD;struct Cyc_Absyn_PtrAtts _tmp2BE;void*
_tmp2BF;struct Cyc_Absyn_Conref*_tmp2C0;struct Cyc_Absyn_Conref*_tmp2C1;struct Cyc_Absyn_Conref*
_tmp2C2;void*_tmp2C3;struct Cyc_Absyn_PtrInfo _tmp2C4;void*_tmp2C5;struct Cyc_Absyn_Tqual
_tmp2C6;struct Cyc_Absyn_PtrAtts _tmp2C7;void*_tmp2C8;struct Cyc_Absyn_Conref*
_tmp2C9;struct Cyc_Absyn_Conref*_tmp2CA;struct Cyc_Absyn_Conref*_tmp2CB;void*
_tmp2CC;void*_tmp2CD;void*_tmp2CE;void*_tmp2CF;void*_tmp2D0;void*_tmp2D1;void*
_tmp2D2;void*_tmp2D3;void*_tmp2D4;int _tmp2D5;void*_tmp2D6;int _tmp2D7;void*
_tmp2D8;void*_tmp2D9;void*_tmp2DA;void*_tmp2DB;void*_tmp2DC;int _tmp2DD;void*
_tmp2DE;int _tmp2DF;void*_tmp2E0;void*_tmp2E1;void*_tmp2E2;void*_tmp2E3;void*
_tmp2E4;struct Cyc_Absyn_ArrayInfo _tmp2E5;void*_tmp2E6;struct Cyc_Absyn_Tqual
_tmp2E7;struct Cyc_Absyn_Exp*_tmp2E8;struct Cyc_Absyn_Conref*_tmp2E9;void*_tmp2EA;
struct Cyc_Absyn_ArrayInfo _tmp2EB;void*_tmp2EC;struct Cyc_Absyn_Tqual _tmp2ED;
struct Cyc_Absyn_Exp*_tmp2EE;struct Cyc_Absyn_Conref*_tmp2EF;void*_tmp2F0;struct
Cyc_Absyn_FnInfo _tmp2F1;struct Cyc_List_List*_tmp2F2;struct Cyc_Core_Opt*_tmp2F3;
void*_tmp2F4;struct Cyc_List_List*_tmp2F5;int _tmp2F6;struct Cyc_Absyn_VarargInfo*
_tmp2F7;struct Cyc_List_List*_tmp2F8;struct Cyc_List_List*_tmp2F9;void*_tmp2FA;
struct Cyc_Absyn_FnInfo _tmp2FB;struct Cyc_List_List*_tmp2FC;struct Cyc_Core_Opt*
_tmp2FD;void*_tmp2FE;struct Cyc_List_List*_tmp2FF;int _tmp300;struct Cyc_Absyn_VarargInfo*
_tmp301;struct Cyc_List_List*_tmp302;struct Cyc_List_List*_tmp303;void*_tmp304;
struct Cyc_List_List*_tmp305;void*_tmp306;struct Cyc_List_List*_tmp307;void*
_tmp308;void*_tmp309;struct Cyc_List_List*_tmp30A;void*_tmp30B;void*_tmp30C;
struct Cyc_List_List*_tmp30D;void*_tmp30E;void*_tmp30F;void*_tmp310;void*_tmp311;
void*_tmp312;void*_tmp313;void*_tmp314;void*_tmp315;void*_tmp316;void*_tmp317;
void*_tmp318;void*_tmp319;void*_tmp31A;void*_tmp31B;_LL219: _tmp289=_tmp288.f1;
if(_tmp289 <= (void*)4  || *((int*)_tmp289)!= 0)goto _LL21B;_LL21A: Cyc_Tcutil_unify_it(
t2,t1);return;_LL21B: _tmp28A=_tmp288.f1;if((int)_tmp28A != 0)goto _LL21D;_tmp28B=
_tmp288.f2;if((int)_tmp28B != 0)goto _LL21D;_LL21C: return;_LL21D: _tmp28C=_tmp288.f1;
if(_tmp28C <= (void*)4  || *((int*)_tmp28C)!= 1)goto _LL21F;_tmp28D=((struct Cyc_Absyn_VarType_struct*)
_tmp28C)->f1;_tmp28E=_tmp288.f2;if(_tmp28E <= (void*)4  || *((int*)_tmp28E)!= 1)
goto _LL21F;_tmp28F=((struct Cyc_Absyn_VarType_struct*)_tmp28E)->f1;_LL21E: {
struct _tagged_arr*_tmp31C=_tmp28D->name;struct _tagged_arr*_tmp31D=_tmp28F->name;
int _tmp31E=*((int*)_check_null(_tmp28D->identity));int _tmp31F=*((int*)
_check_null(_tmp28F->identity));void*_tmp320=Cyc_Tcutil_tvar_kind(_tmp28D);void*
_tmp321=Cyc_Tcutil_tvar_kind(_tmp28F);if(_tmp31F == _tmp31E  && Cyc_strptrcmp(
_tmp31C,_tmp31D)== 0){if(_tmp320 != _tmp321)({struct Cyc_String_pa_struct _tmp326;
_tmp326.tag=0;_tmp326.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp321));{struct Cyc_String_pa_struct _tmp325;_tmp325.tag=0;_tmp325.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp320));{struct Cyc_String_pa_struct
_tmp324;_tmp324.tag=0;_tmp324.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp31C);{
void*_tmp322[3]={& _tmp324,& _tmp325,& _tmp326};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp323="same type variable %s has kinds %s and %s";
_tag_arr(_tmp323,sizeof(char),_get_zero_arr_size(_tmp323,42));}),_tag_arr(
_tmp322,sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=({const char*
_tmp327="(variable types are not the same)";_tag_arr(_tmp327,sizeof(char),
_get_zero_arr_size(_tmp327,34));});goto _LL218;}_LL21F: _tmp290=_tmp288.f1;if(
_tmp290 <= (void*)4  || *((int*)_tmp290)!= 10)goto _LL221;_tmp291=((struct Cyc_Absyn_AggrType_struct*)
_tmp290)->f1;_tmp292=(void*)_tmp291.aggr_info;_tmp293=_tmp291.targs;_tmp294=
_tmp288.f2;if(_tmp294 <= (void*)4  || *((int*)_tmp294)!= 10)goto _LL221;_tmp295=((
struct Cyc_Absyn_AggrType_struct*)_tmp294)->f1;_tmp296=(void*)_tmp295.aggr_info;
_tmp297=_tmp295.targs;_LL220: {void*_tmp329;struct _tuple1*_tmp32A;struct _tuple5
_tmp328=Cyc_Absyn_aggr_kinded_name(_tmp296);_tmp329=_tmp328.f1;_tmp32A=_tmp328.f2;{
void*_tmp32C;struct _tuple1*_tmp32D;struct _tuple5 _tmp32B=Cyc_Absyn_aggr_kinded_name(
_tmp292);_tmp32C=_tmp32B.f1;_tmp32D=_tmp32B.f2;if(_tmp329 != _tmp32C){Cyc_Tcutil_failure_reason=({
const char*_tmp32E="(struct and union type)";_tag_arr(_tmp32E,sizeof(char),
_get_zero_arr_size(_tmp32E,24));});goto _LL218;}if(Cyc_Absyn_qvar_cmp(_tmp32A,
_tmp32D)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp32F="(different type name)";
_tag_arr(_tmp32F,sizeof(char),_get_zero_arr_size(_tmp32F,22));});goto _LL218;}Cyc_Tcutil_unify_list(
_tmp297,_tmp293);return;}}_LL221: _tmp298=_tmp288.f1;if(_tmp298 <= (void*)4  || *((
int*)_tmp298)!= 12)goto _LL223;_tmp299=((struct Cyc_Absyn_EnumType_struct*)_tmp298)->f1;
_tmp29A=_tmp288.f2;if(_tmp29A <= (void*)4  || *((int*)_tmp29A)!= 12)goto _LL223;
_tmp29B=((struct Cyc_Absyn_EnumType_struct*)_tmp29A)->f1;_LL222: if(Cyc_Absyn_qvar_cmp(
_tmp299,_tmp29B)== 0)return;Cyc_Tcutil_failure_reason=({const char*_tmp330="(different enum types)";
_tag_arr(_tmp330,sizeof(char),_get_zero_arr_size(_tmp330,23));});goto _LL218;
_LL223: _tmp29C=_tmp288.f1;if(_tmp29C <= (void*)4  || *((int*)_tmp29C)!= 13)goto
_LL225;_tmp29D=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp29C)->f1;_tmp29E=
_tmp288.f2;if(_tmp29E <= (void*)4  || *((int*)_tmp29E)!= 13)goto _LL225;_tmp29F=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp29E)->f1;_LL224: {int bad=0;for(0;
_tmp29D != 0  && _tmp29F != 0;(_tmp29D=_tmp29D->tl,_tmp29F=_tmp29F->tl)){struct Cyc_Absyn_Enumfield*
_tmp331=(struct Cyc_Absyn_Enumfield*)_tmp29D->hd;struct Cyc_Absyn_Enumfield*
_tmp332=(struct Cyc_Absyn_Enumfield*)_tmp29F->hd;if(Cyc_Absyn_qvar_cmp(_tmp331->name,
_tmp332->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp333="(different names for enum fields)";
_tag_arr(_tmp333,sizeof(char),_get_zero_arr_size(_tmp333,34));});bad=1;break;}
if(_tmp331->tag == _tmp332->tag)continue;if(_tmp331->tag == 0  || _tmp332->tag == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp334="(different tag values for enum fields)";
_tag_arr(_tmp334,sizeof(char),_get_zero_arr_size(_tmp334,39));});bad=1;break;}
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp331->tag),(
struct Cyc_Absyn_Exp*)_check_null(_tmp332->tag))){Cyc_Tcutil_failure_reason=({
const char*_tmp335="(different tag values for enum fields)";_tag_arr(_tmp335,
sizeof(char),_get_zero_arr_size(_tmp335,39));});bad=1;break;}}if(bad)goto _LL218;
if(_tmp29D == 0  && _tmp29F == 0)return;Cyc_Tcutil_failure_reason=({const char*
_tmp336="(different number of fields for enums)";_tag_arr(_tmp336,sizeof(char),
_get_zero_arr_size(_tmp336,39));});goto _LL218;}_LL225: _tmp2A0=_tmp288.f1;if(
_tmp2A0 <= (void*)4  || *((int*)_tmp2A0)!= 2)goto _LL227;_tmp2A1=((struct Cyc_Absyn_TunionType_struct*)
_tmp2A0)->f1;_tmp2A2=(void*)_tmp2A1.tunion_info;if(*((int*)_tmp2A2)!= 1)goto
_LL227;_tmp2A3=((struct Cyc_Absyn_KnownTunion_struct*)_tmp2A2)->f1;_tmp2A4=*
_tmp2A3;_tmp2A5=_tmp2A1.targs;_tmp2A6=(void*)_tmp2A1.rgn;_tmp2A7=_tmp288.f2;if(
_tmp2A7 <= (void*)4  || *((int*)_tmp2A7)!= 2)goto _LL227;_tmp2A8=((struct Cyc_Absyn_TunionType_struct*)
_tmp2A7)->f1;_tmp2A9=(void*)_tmp2A8.tunion_info;if(*((int*)_tmp2A9)!= 1)goto
_LL227;_tmp2AA=((struct Cyc_Absyn_KnownTunion_struct*)_tmp2A9)->f1;_tmp2AB=*
_tmp2AA;_tmp2AC=_tmp2A8.targs;_tmp2AD=(void*)_tmp2A8.rgn;_LL226: if(_tmp2A4 == 
_tmp2AB  || Cyc_Absyn_qvar_cmp(_tmp2A4->name,_tmp2AB->name)== 0){Cyc_Tcutil_unify_it(
_tmp2AD,_tmp2A6);Cyc_Tcutil_unify_list(_tmp2AC,_tmp2A5);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp337="(different tunion types)";_tag_arr(_tmp337,sizeof(char),
_get_zero_arr_size(_tmp337,25));});goto _LL218;_LL227: _tmp2AE=_tmp288.f1;if(
_tmp2AE <= (void*)4  || *((int*)_tmp2AE)!= 3)goto _LL229;_tmp2AF=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2AE)->f1;_tmp2B0=(void*)_tmp2AF.field_info;if(*((int*)_tmp2B0)!= 1)goto
_LL229;_tmp2B1=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2B0)->f1;_tmp2B2=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2B0)->f2;_tmp2B3=_tmp2AF.targs;
_tmp2B4=_tmp288.f2;if(_tmp2B4 <= (void*)4  || *((int*)_tmp2B4)!= 3)goto _LL229;
_tmp2B5=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp2B4)->f1;_tmp2B6=(void*)
_tmp2B5.field_info;if(*((int*)_tmp2B6)!= 1)goto _LL229;_tmp2B7=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2B6)->f2;
_tmp2B9=_tmp2B5.targs;_LL228: if((_tmp2B1 == _tmp2B7  || Cyc_Absyn_qvar_cmp(_tmp2B1->name,
_tmp2B7->name)== 0) && (_tmp2B2 == _tmp2B8  || Cyc_Absyn_qvar_cmp(_tmp2B2->name,
_tmp2B8->name)== 0)){Cyc_Tcutil_unify_list(_tmp2B9,_tmp2B3);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp338="(different tunion field types)";_tag_arr(_tmp338,sizeof(char),
_get_zero_arr_size(_tmp338,31));});goto _LL218;_LL229: _tmp2BA=_tmp288.f1;if(
_tmp2BA <= (void*)4  || *((int*)_tmp2BA)!= 4)goto _LL22B;_tmp2BB=((struct Cyc_Absyn_PointerType_struct*)
_tmp2BA)->f1;_tmp2BC=(void*)_tmp2BB.elt_typ;_tmp2BD=_tmp2BB.elt_tq;_tmp2BE=
_tmp2BB.ptr_atts;_tmp2BF=(void*)_tmp2BE.rgn;_tmp2C0=_tmp2BE.nullable;_tmp2C1=
_tmp2BE.bounds;_tmp2C2=_tmp2BE.zero_term;_tmp2C3=_tmp288.f2;if(_tmp2C3 <= (void*)
4  || *((int*)_tmp2C3)!= 4)goto _LL22B;_tmp2C4=((struct Cyc_Absyn_PointerType_struct*)
_tmp2C3)->f1;_tmp2C5=(void*)_tmp2C4.elt_typ;_tmp2C6=_tmp2C4.elt_tq;_tmp2C7=
_tmp2C4.ptr_atts;_tmp2C8=(void*)_tmp2C7.rgn;_tmp2C9=_tmp2C7.nullable;_tmp2CA=
_tmp2C7.bounds;_tmp2CB=_tmp2C7.zero_term;_LL22A: Cyc_Tcutil_unify_it(_tmp2C5,
_tmp2BC);Cyc_Tcutil_unify_it(_tmp2BF,_tmp2C8);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2CB,
_tmp2C2,({const char*_tmp339="(not both zero terminated)";_tag_arr(_tmp339,
sizeof(char),_get_zero_arr_size(_tmp339,27));}));Cyc_Tcutil_unify_tqual(_tmp2C6,
_tmp2C5,_tmp2BD,_tmp2BC);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp2CA,_tmp2C1,({const char*_tmp33A="(different pointer bounds)";_tag_arr(
_tmp33A,sizeof(char),_get_zero_arr_size(_tmp33A,27));}));{void*_tmp33B=(void*)(
Cyc_Absyn_compress_conref(_tmp2CA))->v;void*_tmp33C;_LL254: if(_tmp33B <= (void*)1
 || *((int*)_tmp33B)!= 0)goto _LL256;_tmp33C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp33B)->f1;if((int)_tmp33C != 0)goto _LL256;_LL255: return;_LL256:;_LL257: goto
_LL253;_LL253:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2C9,
_tmp2C0,({const char*_tmp33D="(incompatible pointer types)";_tag_arr(_tmp33D,
sizeof(char),_get_zero_arr_size(_tmp33D,29));}));return;_LL22B: _tmp2CC=_tmp288.f1;
if(_tmp2CC <= (void*)4  || *((int*)_tmp2CC)!= 5)goto _LL22D;_tmp2CD=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp2CC)->f1;_tmp2CE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2CC)->f2;_tmp2CF=_tmp288.f2;if(_tmp2CF <= (void*)4  || *((int*)_tmp2CF)!= 5)
goto _LL22D;_tmp2D0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2CF)->f1;_tmp2D1=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2CF)->f2;_LL22C: if(_tmp2D0 == _tmp2CD
 && _tmp2D1 == _tmp2CE)return;Cyc_Tcutil_failure_reason=({const char*_tmp33E="(different integral types)";
_tag_arr(_tmp33E,sizeof(char),_get_zero_arr_size(_tmp33E,27));});goto _LL218;
_LL22D: _tmp2D2=_tmp288.f1;if((int)_tmp2D2 != 1)goto _LL22F;_tmp2D3=_tmp288.f2;if((
int)_tmp2D3 != 1)goto _LL22F;_LL22E: return;_LL22F: _tmp2D4=_tmp288.f1;if(_tmp2D4 <= (
void*)4  || *((int*)_tmp2D4)!= 6)goto _LL231;_tmp2D5=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2D4)->f1;_tmp2D6=_tmp288.f2;if(_tmp2D6 <= (void*)4  || *((int*)_tmp2D6)!= 6)
goto _LL231;_tmp2D7=((struct Cyc_Absyn_DoubleType_struct*)_tmp2D6)->f1;_LL230: if(
_tmp2D5 == _tmp2D7)return;goto _LL218;_LL231: _tmp2D8=_tmp288.f1;if(_tmp2D8 <= (void*)
4  || *((int*)_tmp2D8)!= 14)goto _LL233;_tmp2D9=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp2D8)->f1;_tmp2DA=_tmp288.f2;if(_tmp2DA <= (void*)4  || *((int*)_tmp2DA)!= 14)
goto _LL233;_tmp2DB=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2DA)->f1;
_LL232: Cyc_Tcutil_unify_it(_tmp2D9,_tmp2DB);return;_LL233: _tmp2DC=_tmp288.f1;if(
_tmp2DC <= (void*)4  || *((int*)_tmp2DC)!= 18)goto _LL235;_tmp2DD=((struct Cyc_Absyn_TypeInt_struct*)
_tmp2DC)->f1;_tmp2DE=_tmp288.f2;if(_tmp2DE <= (void*)4  || *((int*)_tmp2DE)!= 18)
goto _LL235;_tmp2DF=((struct Cyc_Absyn_TypeInt_struct*)_tmp2DE)->f1;_LL234: if(
_tmp2DD == _tmp2DF)return;Cyc_Tcutil_failure_reason=({const char*_tmp33F="(different type integers)";
_tag_arr(_tmp33F,sizeof(char),_get_zero_arr_size(_tmp33F,26));});goto _LL218;
_LL235: _tmp2E0=_tmp288.f1;if(_tmp2E0 <= (void*)4  || *((int*)_tmp2E0)!= 17)goto
_LL237;_tmp2E1=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp2E0)->f1;_tmp2E2=
_tmp288.f2;if(_tmp2E2 <= (void*)4  || *((int*)_tmp2E2)!= 17)goto _LL237;_tmp2E3=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp2E2)->f1;_LL236: Cyc_Tcutil_unify_it(
_tmp2E1,_tmp2E3);return;_LL237: _tmp2E4=_tmp288.f1;if(_tmp2E4 <= (void*)4  || *((
int*)_tmp2E4)!= 7)goto _LL239;_tmp2E5=((struct Cyc_Absyn_ArrayType_struct*)_tmp2E4)->f1;
_tmp2E6=(void*)_tmp2E5.elt_type;_tmp2E7=_tmp2E5.tq;_tmp2E8=_tmp2E5.num_elts;
_tmp2E9=_tmp2E5.zero_term;_tmp2EA=_tmp288.f2;if(_tmp2EA <= (void*)4  || *((int*)
_tmp2EA)!= 7)goto _LL239;_tmp2EB=((struct Cyc_Absyn_ArrayType_struct*)_tmp2EA)->f1;
_tmp2EC=(void*)_tmp2EB.elt_type;_tmp2ED=_tmp2EB.tq;_tmp2EE=_tmp2EB.num_elts;
_tmp2EF=_tmp2EB.zero_term;_LL238: Cyc_Tcutil_unify_it(_tmp2EC,_tmp2E6);Cyc_Tcutil_unify_tqual(
_tmp2ED,_tmp2EC,_tmp2E7,_tmp2E6);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2E9,
_tmp2EF,({const char*_tmp340="(not both zero terminated)";_tag_arr(_tmp340,
sizeof(char),_get_zero_arr_size(_tmp340,27));}));if(_tmp2E8 == _tmp2EE)return;if(
_tmp2E8 == 0  || _tmp2EE == 0)goto _LL218;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_tmp2E8,(struct Cyc_Absyn_Exp*)_tmp2EE))return;Cyc_Tcutil_failure_reason=({const
char*_tmp341="(different array sizes)";_tag_arr(_tmp341,sizeof(char),
_get_zero_arr_size(_tmp341,24));});goto _LL218;_LL239: _tmp2F0=_tmp288.f1;if(
_tmp2F0 <= (void*)4  || *((int*)_tmp2F0)!= 8)goto _LL23B;_tmp2F1=((struct Cyc_Absyn_FnType_struct*)
_tmp2F0)->f1;_tmp2F2=_tmp2F1.tvars;_tmp2F3=_tmp2F1.effect;_tmp2F4=(void*)_tmp2F1.ret_typ;
_tmp2F5=_tmp2F1.args;_tmp2F6=_tmp2F1.c_varargs;_tmp2F7=_tmp2F1.cyc_varargs;
_tmp2F8=_tmp2F1.rgn_po;_tmp2F9=_tmp2F1.attributes;_tmp2FA=_tmp288.f2;if(_tmp2FA
<= (void*)4  || *((int*)_tmp2FA)!= 8)goto _LL23B;_tmp2FB=((struct Cyc_Absyn_FnType_struct*)
_tmp2FA)->f1;_tmp2FC=_tmp2FB.tvars;_tmp2FD=_tmp2FB.effect;_tmp2FE=(void*)_tmp2FB.ret_typ;
_tmp2FF=_tmp2FB.args;_tmp300=_tmp2FB.c_varargs;_tmp301=_tmp2FB.cyc_varargs;
_tmp302=_tmp2FB.rgn_po;_tmp303=_tmp2FB.attributes;_LL23A: {int done=0;{struct
_RegionHandle _tmp342=_new_region("rgn");struct _RegionHandle*rgn=& _tmp342;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp2FC != 0){if(_tmp2F2 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp343="(second function type has too few type variables)";
_tag_arr(_tmp343,sizeof(char),_get_zero_arr_size(_tmp343,50));});(int)_throw((
void*)Cyc_Tcutil_Unify);}{void*_tmp344=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp2FC->hd);void*_tmp345=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2F2->hd);
if(_tmp344 != _tmp345){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp34A;_tmp34A.tag=0;_tmp34A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp345));{struct Cyc_String_pa_struct _tmp349;_tmp349.tag=0;_tmp349.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp344));{struct Cyc_String_pa_struct
_tmp348;_tmp348.tag=0;_tmp348.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp2FC->hd));{void*_tmp346[3]={& _tmp348,& _tmp349,& _tmp34A};
Cyc_aprintf(({const char*_tmp347="(type var %s has different kinds %s and %s)";
_tag_arr(_tmp347,sizeof(char),_get_zero_arr_size(_tmp347,44));}),_tag_arr(
_tmp346,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}inst=({
struct Cyc_List_List*_tmp34B=_region_malloc(rgn,sizeof(*_tmp34B));_tmp34B->hd=({
struct _tuple8*_tmp34C=_region_malloc(rgn,sizeof(*_tmp34C));_tmp34C->f1=(struct
Cyc_Absyn_Tvar*)_tmp2F2->hd;_tmp34C->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp34D=_cycalloc(sizeof(*_tmp34D));_tmp34D[0]=({struct Cyc_Absyn_VarType_struct
_tmp34E;_tmp34E.tag=1;_tmp34E.f1=(struct Cyc_Absyn_Tvar*)_tmp2FC->hd;_tmp34E;});
_tmp34D;});_tmp34C;});_tmp34B->tl=inst;_tmp34B;});_tmp2FC=_tmp2FC->tl;_tmp2F2=
_tmp2F2->tl;}}if(_tmp2F2 != 0){Cyc_Tcutil_failure_reason=({const char*_tmp34F="(second function type has too many type variables)";
_tag_arr(_tmp34F,sizeof(char),_get_zero_arr_size(_tmp34F,51));});_npop_handler(0);
goto _LL218;}if(inst != 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*
_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350[0]=({struct Cyc_Absyn_FnType_struct
_tmp351;_tmp351.tag=8;_tmp351.f1=({struct Cyc_Absyn_FnInfo _tmp352;_tmp352.tvars=0;
_tmp352.effect=_tmp2FD;_tmp352.ret_typ=(void*)_tmp2FE;_tmp352.args=_tmp2FF;
_tmp352.c_varargs=_tmp300;_tmp352.cyc_varargs=_tmp301;_tmp352.rgn_po=_tmp302;
_tmp352.attributes=_tmp303;_tmp352;});_tmp351;});_tmp350;}),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*_tmp353=_cycalloc(sizeof(*
_tmp353));_tmp353[0]=({struct Cyc_Absyn_FnType_struct _tmp354;_tmp354.tag=8;
_tmp354.f1=({struct Cyc_Absyn_FnInfo _tmp355;_tmp355.tvars=0;_tmp355.effect=
_tmp2F3;_tmp355.ret_typ=(void*)_tmp2F4;_tmp355.args=_tmp2F5;_tmp355.c_varargs=
_tmp2F6;_tmp355.cyc_varargs=_tmp2F7;_tmp355.rgn_po=_tmp2F8;_tmp355.attributes=
_tmp2F9;_tmp355;});_tmp354;});_tmp353;})));done=1;}};_pop_region(rgn);}if(done)
return;Cyc_Tcutil_unify_it(_tmp2FE,_tmp2F4);for(0;_tmp2FF != 0  && _tmp2F5 != 0;(
_tmp2FF=_tmp2FF->tl,_tmp2F5=_tmp2F5->tl)){struct Cyc_Absyn_Tqual _tmp357;void*
_tmp358;struct _tuple2 _tmp356=*((struct _tuple2*)_tmp2FF->hd);_tmp357=_tmp356.f2;
_tmp358=_tmp356.f3;{struct Cyc_Absyn_Tqual _tmp35A;void*_tmp35B;struct _tuple2
_tmp359=*((struct _tuple2*)_tmp2F5->hd);_tmp35A=_tmp359.f2;_tmp35B=_tmp359.f3;Cyc_Tcutil_unify_it(
_tmp358,_tmp35B);Cyc_Tcutil_unify_tqual(_tmp357,_tmp358,_tmp35A,_tmp35B);}}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp2FF != 0  || _tmp2F5 != 0){Cyc_Tcutil_failure_reason=({
const char*_tmp35C="(function types have different number of arguments)";_tag_arr(
_tmp35C,sizeof(char),_get_zero_arr_size(_tmp35C,52));});goto _LL218;}if(_tmp300 != 
_tmp2F6){Cyc_Tcutil_failure_reason=({const char*_tmp35D="(only one function type takes C varargs)";
_tag_arr(_tmp35D,sizeof(char),_get_zero_arr_size(_tmp35D,41));});goto _LL218;}{
int bad_cyc_vararg=0;{struct _tuple11 _tmp35F=({struct _tuple11 _tmp35E;_tmp35E.f1=
_tmp301;_tmp35E.f2=_tmp2F7;_tmp35E;});struct Cyc_Absyn_VarargInfo*_tmp360;struct
Cyc_Absyn_VarargInfo*_tmp361;struct Cyc_Absyn_VarargInfo*_tmp362;struct Cyc_Absyn_VarargInfo*
_tmp363;struct Cyc_Absyn_VarargInfo*_tmp364;struct Cyc_Absyn_VarargInfo _tmp365;
struct Cyc_Core_Opt*_tmp366;struct Cyc_Absyn_Tqual _tmp367;void*_tmp368;int _tmp369;
struct Cyc_Absyn_VarargInfo*_tmp36A;struct Cyc_Absyn_VarargInfo _tmp36B;struct Cyc_Core_Opt*
_tmp36C;struct Cyc_Absyn_Tqual _tmp36D;void*_tmp36E;int _tmp36F;_LL259: _tmp360=
_tmp35F.f1;if(_tmp360 != 0)goto _LL25B;_tmp361=_tmp35F.f2;if(_tmp361 != 0)goto
_LL25B;_LL25A: goto _LL258;_LL25B: _tmp362=_tmp35F.f1;if(_tmp362 != 0)goto _LL25D;
_LL25C: goto _LL25E;_LL25D: _tmp363=_tmp35F.f2;if(_tmp363 != 0)goto _LL25F;_LL25E:
bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp370="(only one function type takes varargs)";
_tag_arr(_tmp370,sizeof(char),_get_zero_arr_size(_tmp370,39));});goto _LL258;
_LL25F: _tmp364=_tmp35F.f1;if(_tmp364 == 0)goto _LL258;_tmp365=*_tmp364;_tmp366=
_tmp365.name;_tmp367=_tmp365.tq;_tmp368=(void*)_tmp365.type;_tmp369=_tmp365.inject;
_tmp36A=_tmp35F.f2;if(_tmp36A == 0)goto _LL258;_tmp36B=*_tmp36A;_tmp36C=_tmp36B.name;
_tmp36D=_tmp36B.tq;_tmp36E=(void*)_tmp36B.type;_tmp36F=_tmp36B.inject;_LL260: Cyc_Tcutil_unify_it(
_tmp368,_tmp36E);Cyc_Tcutil_unify_tqual(_tmp367,_tmp368,_tmp36D,_tmp36E);if(
_tmp369 != _tmp36F){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*
_tmp371="(only one function type injects varargs)";_tag_arr(_tmp371,sizeof(char),
_get_zero_arr_size(_tmp371,41));});}goto _LL258;_LL258:;}if(bad_cyc_vararg)goto
_LL218;{int bad_effect=0;{struct _tuple12 _tmp373=({struct _tuple12 _tmp372;_tmp372.f1=
_tmp2FD;_tmp372.f2=_tmp2F3;_tmp372;});struct Cyc_Core_Opt*_tmp374;struct Cyc_Core_Opt*
_tmp375;struct Cyc_Core_Opt*_tmp376;struct Cyc_Core_Opt*_tmp377;_LL262: _tmp374=
_tmp373.f1;if(_tmp374 != 0)goto _LL264;_tmp375=_tmp373.f2;if(_tmp375 != 0)goto
_LL264;_LL263: goto _LL261;_LL264: _tmp376=_tmp373.f1;if(_tmp376 != 0)goto _LL266;
_LL265: goto _LL267;_LL266: _tmp377=_tmp373.f2;if(_tmp377 != 0)goto _LL268;_LL267:
bad_effect=1;goto _LL261;_LL268:;_LL269: bad_effect=!Cyc_Tcutil_unify_effect((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp2FD))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2F3))->v);goto _LL261;_LL261:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=({const char*_tmp378="(function type effects do not unify)";
_tag_arr(_tmp378,sizeof(char),_get_zero_arr_size(_tmp378,37));});goto _LL218;}if(
!Cyc_Tcutil_same_atts(_tmp2F9,_tmp303)){Cyc_Tcutil_failure_reason=({const char*
_tmp379="(function types have different attributes)";_tag_arr(_tmp379,sizeof(
char),_get_zero_arr_size(_tmp379,43));});goto _LL218;}if(!Cyc_Tcutil_same_rgn_po(
_tmp2F8,_tmp302)){Cyc_Tcutil_failure_reason=({const char*_tmp37A="(function types have different region lifetime orderings)";
_tag_arr(_tmp37A,sizeof(char),_get_zero_arr_size(_tmp37A,58));});goto _LL218;}
return;}}}_LL23B: _tmp304=_tmp288.f1;if(_tmp304 <= (void*)4  || *((int*)_tmp304)!= 
9)goto _LL23D;_tmp305=((struct Cyc_Absyn_TupleType_struct*)_tmp304)->f1;_tmp306=
_tmp288.f2;if(_tmp306 <= (void*)4  || *((int*)_tmp306)!= 9)goto _LL23D;_tmp307=((
struct Cyc_Absyn_TupleType_struct*)_tmp306)->f1;_LL23C: for(0;_tmp307 != 0  && 
_tmp305 != 0;(_tmp307=_tmp307->tl,_tmp305=_tmp305->tl)){struct Cyc_Absyn_Tqual
_tmp37C;void*_tmp37D;struct _tuple4 _tmp37B=*((struct _tuple4*)_tmp307->hd);_tmp37C=
_tmp37B.f1;_tmp37D=_tmp37B.f2;{struct Cyc_Absyn_Tqual _tmp37F;void*_tmp380;struct
_tuple4 _tmp37E=*((struct _tuple4*)_tmp305->hd);_tmp37F=_tmp37E.f1;_tmp380=_tmp37E.f2;
Cyc_Tcutil_unify_it(_tmp37D,_tmp380);Cyc_Tcutil_unify_tqual(_tmp37C,_tmp37D,
_tmp37F,_tmp380);}}if(_tmp307 == 0  && _tmp305 == 0)return;Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp381="(tuple types have different numbers of components)";
_tag_arr(_tmp381,sizeof(char),_get_zero_arr_size(_tmp381,51));});goto _LL218;
_LL23D: _tmp308=_tmp288.f1;if(_tmp308 <= (void*)4  || *((int*)_tmp308)!= 11)goto
_LL23F;_tmp309=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp308)->f1;
_tmp30A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp308)->f2;_tmp30B=_tmp288.f2;
if(_tmp30B <= (void*)4  || *((int*)_tmp30B)!= 11)goto _LL23F;_tmp30C=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp30B)->f1;_tmp30D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp30B)->f2;_LL23E: if(_tmp30C != _tmp309){Cyc_Tcutil_failure_reason=({const char*
_tmp382="(struct and union type)";_tag_arr(_tmp382,sizeof(char),
_get_zero_arr_size(_tmp382,24));});goto _LL218;}for(0;_tmp30D != 0  && _tmp30A != 0;(
_tmp30D=_tmp30D->tl,_tmp30A=_tmp30A->tl)){struct Cyc_Absyn_Aggrfield*_tmp383=(
struct Cyc_Absyn_Aggrfield*)_tmp30D->hd;struct Cyc_Absyn_Aggrfield*_tmp384=(struct
Cyc_Absyn_Aggrfield*)_tmp30A->hd;if(Cyc_strptrcmp(_tmp383->name,_tmp384->name)!= 
0){Cyc_Tcutil_failure_reason=({const char*_tmp385="(different member names)";
_tag_arr(_tmp385,sizeof(char),_get_zero_arr_size(_tmp385,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_it((void*)_tmp383->type,(void*)_tmp384->type);
Cyc_Tcutil_unify_tqual(_tmp383->tq,(void*)_tmp383->type,_tmp384->tq,(void*)
_tmp384->type);if(!Cyc_Tcutil_same_atts(_tmp383->attributes,_tmp384->attributes)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp386="(different attributes on member)";_tag_arr(_tmp386,sizeof(
char),_get_zero_arr_size(_tmp386,33));});(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp383->width != 0  && _tmp384->width == 0  || _tmp384->width != 0  && _tmp383->width
== 0) || (_tmp383->width != 0  && _tmp384->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp383->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp384->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp387="(different bitfield widths on member)";
_tag_arr(_tmp387,sizeof(char),_get_zero_arr_size(_tmp387,38));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}if(_tmp30D == 0  && _tmp30A == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp388="(different number of members)";
_tag_arr(_tmp388,sizeof(char),_get_zero_arr_size(_tmp388,30));});goto _LL218;
_LL23F: _tmp30E=_tmp288.f1;if((int)_tmp30E != 2)goto _LL241;_tmp30F=_tmp288.f2;if((
int)_tmp30F != 2)goto _LL241;_LL240: return;_LL241: _tmp310=_tmp288.f1;if((int)
_tmp310 != 3)goto _LL243;_tmp311=_tmp288.f2;if((int)_tmp311 != 3)goto _LL243;_LL242:
return;_LL243: _tmp312=_tmp288.f1;if(_tmp312 <= (void*)4  || *((int*)_tmp312)!= 15)
goto _LL245;_tmp313=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp312)->f1;
_tmp314=_tmp288.f2;if(_tmp314 <= (void*)4  || *((int*)_tmp314)!= 15)goto _LL245;
_tmp315=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp314)->f1;_LL244: Cyc_Tcutil_unify_it(
_tmp313,_tmp315);return;_LL245: _tmp316=_tmp288.f1;if(_tmp316 <= (void*)4  || *((
int*)_tmp316)!= 20)goto _LL247;_LL246: goto _LL248;_LL247: _tmp317=_tmp288.f2;if(
_tmp317 <= (void*)4  || *((int*)_tmp317)!= 20)goto _LL249;_LL248: goto _LL24A;_LL249:
_tmp318=_tmp288.f1;if(_tmp318 <= (void*)4  || *((int*)_tmp318)!= 19)goto _LL24B;
_LL24A: goto _LL24C;_LL24B: _tmp319=_tmp288.f1;if(_tmp319 <= (void*)4  || *((int*)
_tmp319)!= 21)goto _LL24D;_LL24C: goto _LL24E;_LL24D: _tmp31A=_tmp288.f2;if(_tmp31A
<= (void*)4  || *((int*)_tmp31A)!= 21)goto _LL24F;_LL24E: goto _LL250;_LL24F: _tmp31B=
_tmp288.f2;if(_tmp31B <= (void*)4  || *((int*)_tmp31B)!= 19)goto _LL251;_LL250: if(
Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=({const char*
_tmp389="(effects don't unify)";_tag_arr(_tmp389,sizeof(char),_get_zero_arr_size(
_tmp389,22));});goto _LL218;_LL251:;_LL252: goto _LL218;_LL218:;}(int)_throw((void*)
Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int _tmp38A=(tq1.real_const + (
tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp38B=(tq2.real_const + (tq2.q_volatile
<< 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(_tmp38A,_tmp38B);}static int
Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);if(x == y)return
0;{void*_tmp38C=(void*)x->v;void*_tmp38D;_LL26B: if((int)_tmp38C != 0)goto _LL26D;
_LL26C: return - 1;_LL26D: if(_tmp38C <= (void*)1  || *((int*)_tmp38C)!= 0)goto _LL26F;
_tmp38D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp38C)->f1;_LL26E: {void*
_tmp38E=(void*)y->v;void*_tmp38F;_LL272: if((int)_tmp38E != 0)goto _LL274;_LL273:
return 1;_LL274: if(_tmp38E <= (void*)1  || *((int*)_tmp38E)!= 0)goto _LL276;_tmp38F=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp38E)->f1;_LL275: return cmp(_tmp38D,
_tmp38F);_LL276: if(_tmp38E <= (void*)1  || *((int*)_tmp38E)!= 1)goto _LL271;_LL277:({
void*_tmp390[0]={};Cyc_Tcutil_impos(({const char*_tmp391="unify_conref: forward after compress(2)";
_tag_arr(_tmp391,sizeof(char),_get_zero_arr_size(_tmp391,40));}),_tag_arr(
_tmp390,sizeof(void*),0));});_LL271:;}_LL26F: if(_tmp38C <= (void*)1  || *((int*)
_tmp38C)!= 1)goto _LL26A;_LL270:({void*_tmp392[0]={};Cyc_Tcutil_impos(({const char*
_tmp393="unify_conref: forward after compress";_tag_arr(_tmp393,sizeof(char),
_get_zero_arr_size(_tmp393,37));}),_tag_arr(_tmp392,sizeof(void*),0));});_LL26A:;}}
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple4*tqt1,struct _tuple4*tqt2){struct
_tuple4 _tmp395;struct Cyc_Absyn_Tqual _tmp396;void*_tmp397;struct _tuple4*_tmp394=
tqt1;_tmp395=*_tmp394;_tmp396=_tmp395.f1;_tmp397=_tmp395.f2;{struct _tuple4
_tmp399;struct Cyc_Absyn_Tqual _tmp39A;void*_tmp39B;struct _tuple4*_tmp398=tqt2;
_tmp399=*_tmp398;_tmp39A=_tmp399.f1;_tmp39B=_tmp399.f2;{int _tmp39C=Cyc_Tcutil_tqual_cmp(
_tmp396,_tmp39A);if(_tmp39C != 0)return _tmp39C;return Cyc_Tcutil_typecmp(_tmp397,
_tmp39B);}}}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,
struct Cyc_Absyn_Aggrfield*f2){int _tmp39D=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp39D != 0)return _tmp39D;{int _tmp39E=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp39E != 0)return _tmp39E;{int _tmp39F=Cyc_Tcutil_typecmp((void*)f1->type,(void*)
f2->type);if(_tmp39F != 0)return _tmp39F;{int _tmp3A0=Cyc_List_list_cmp(Cyc_Tcutil_attribute_cmp,
f1->attributes,f2->attributes);if(_tmp3A0 != 0)return _tmp3A0;return((int(*)(int(*
cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);}}}}static
int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2){int _tmp3A1=Cyc_Absyn_qvar_cmp(e1->name,e2->name);if(_tmp3A1 != 0)return
_tmp3A1;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(void*t){void*_tmp3A2=t;
_LL279: if((int)_tmp3A2 != 0)goto _LL27B;_LL27A: return 0;_LL27B: if(_tmp3A2 <= (void*)
4  || *((int*)_tmp3A2)!= 0)goto _LL27D;_LL27C: return 1;_LL27D: if(_tmp3A2 <= (void*)4
 || *((int*)_tmp3A2)!= 1)goto _LL27F;_LL27E: return 2;_LL27F: if(_tmp3A2 <= (void*)4
 || *((int*)_tmp3A2)!= 2)goto _LL281;_LL280: return 3;_LL281: if(_tmp3A2 <= (void*)4
 || *((int*)_tmp3A2)!= 3)goto _LL283;_LL282: return 4;_LL283: if(_tmp3A2 <= (void*)4
 || *((int*)_tmp3A2)!= 4)goto _LL285;_LL284: return 5;_LL285: if(_tmp3A2 <= (void*)4
 || *((int*)_tmp3A2)!= 5)goto _LL287;_LL286: return 6;_LL287: if((int)_tmp3A2 != 1)
goto _LL289;_LL288: return 7;_LL289: if(_tmp3A2 <= (void*)4  || *((int*)_tmp3A2)!= 6)
goto _LL28B;_LL28A: return 8;_LL28B: if(_tmp3A2 <= (void*)4  || *((int*)_tmp3A2)!= 7)
goto _LL28D;_LL28C: return 9;_LL28D: if(_tmp3A2 <= (void*)4  || *((int*)_tmp3A2)!= 8)
goto _LL28F;_LL28E: return 10;_LL28F: if(_tmp3A2 <= (void*)4  || *((int*)_tmp3A2)!= 9)
goto _LL291;_LL290: return 11;_LL291: if(_tmp3A2 <= (void*)4  || *((int*)_tmp3A2)!= 10)
goto _LL293;_LL292: return 12;_LL293: if(_tmp3A2 <= (void*)4  || *((int*)_tmp3A2)!= 11)
goto _LL295;_LL294: return 14;_LL295: if(_tmp3A2 <= (void*)4  || *((int*)_tmp3A2)!= 12)
goto _LL297;_LL296: return 16;_LL297: if(_tmp3A2 <= (void*)4  || *((int*)_tmp3A2)!= 13)
goto _LL299;_LL298: return 17;_LL299: if(_tmp3A2 <= (void*)4  || *((int*)_tmp3A2)!= 15)
goto _LL29B;_LL29A: return 18;_LL29B: if(_tmp3A2 <= (void*)4  || *((int*)_tmp3A2)!= 16)
goto _LL29D;_LL29C: return 19;_LL29D: if((int)_tmp3A2 != 3)goto _LL29F;_LL29E: return 20;
_LL29F: if((int)_tmp3A2 != 2)goto _LL2A1;_LL2A0: return 21;_LL2A1: if(_tmp3A2 <= (void*)
4  || *((int*)_tmp3A2)!= 19)goto _LL2A3;_LL2A2: return 22;_LL2A3: if(_tmp3A2 <= (void*)
4  || *((int*)_tmp3A2)!= 20)goto _LL2A5;_LL2A4: return 23;_LL2A5: if(_tmp3A2 <= (void*)
4  || *((int*)_tmp3A2)!= 21)goto _LL2A7;_LL2A6: return 24;_LL2A7: if(_tmp3A2 <= (void*)
4  || *((int*)_tmp3A2)!= 14)goto _LL2A9;_LL2A8: return 25;_LL2A9: if(_tmp3A2 <= (void*)
4  || *((int*)_tmp3A2)!= 18)goto _LL2AB;_LL2AA: return 26;_LL2AB: if(_tmp3A2 <= (void*)
4  || *((int*)_tmp3A2)!= 17)goto _LL278;_LL2AC: return 27;_LL278:;}int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp3A3=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp3A3 != 0)return _tmp3A3;{struct _tuple6 _tmp3A5=({struct _tuple6 _tmp3A4;
_tmp3A4.f1=t2;_tmp3A4.f2=t1;_tmp3A4;});void*_tmp3A6;void*_tmp3A7;void*_tmp3A8;
struct Cyc_Absyn_Tvar*_tmp3A9;void*_tmp3AA;struct Cyc_Absyn_Tvar*_tmp3AB;void*
_tmp3AC;struct Cyc_Absyn_AggrInfo _tmp3AD;void*_tmp3AE;struct Cyc_List_List*_tmp3AF;
void*_tmp3B0;struct Cyc_Absyn_AggrInfo _tmp3B1;void*_tmp3B2;struct Cyc_List_List*
_tmp3B3;void*_tmp3B4;struct _tuple1*_tmp3B5;void*_tmp3B6;struct _tuple1*_tmp3B7;
void*_tmp3B8;struct Cyc_List_List*_tmp3B9;void*_tmp3BA;struct Cyc_List_List*
_tmp3BB;void*_tmp3BC;struct Cyc_Absyn_TunionInfo _tmp3BD;void*_tmp3BE;struct Cyc_Absyn_Tuniondecl**
_tmp3BF;struct Cyc_Absyn_Tuniondecl*_tmp3C0;struct Cyc_List_List*_tmp3C1;void*
_tmp3C2;void*_tmp3C3;struct Cyc_Absyn_TunionInfo _tmp3C4;void*_tmp3C5;struct Cyc_Absyn_Tuniondecl**
_tmp3C6;struct Cyc_Absyn_Tuniondecl*_tmp3C7;struct Cyc_List_List*_tmp3C8;void*
_tmp3C9;void*_tmp3CA;struct Cyc_Absyn_TunionFieldInfo _tmp3CB;void*_tmp3CC;struct
Cyc_Absyn_Tuniondecl*_tmp3CD;struct Cyc_Absyn_Tunionfield*_tmp3CE;struct Cyc_List_List*
_tmp3CF;void*_tmp3D0;struct Cyc_Absyn_TunionFieldInfo _tmp3D1;void*_tmp3D2;struct
Cyc_Absyn_Tuniondecl*_tmp3D3;struct Cyc_Absyn_Tunionfield*_tmp3D4;struct Cyc_List_List*
_tmp3D5;void*_tmp3D6;struct Cyc_Absyn_PtrInfo _tmp3D7;void*_tmp3D8;struct Cyc_Absyn_Tqual
_tmp3D9;struct Cyc_Absyn_PtrAtts _tmp3DA;void*_tmp3DB;struct Cyc_Absyn_Conref*
_tmp3DC;struct Cyc_Absyn_Conref*_tmp3DD;struct Cyc_Absyn_Conref*_tmp3DE;void*
_tmp3DF;struct Cyc_Absyn_PtrInfo _tmp3E0;void*_tmp3E1;struct Cyc_Absyn_Tqual _tmp3E2;
struct Cyc_Absyn_PtrAtts _tmp3E3;void*_tmp3E4;struct Cyc_Absyn_Conref*_tmp3E5;
struct Cyc_Absyn_Conref*_tmp3E6;struct Cyc_Absyn_Conref*_tmp3E7;void*_tmp3E8;void*
_tmp3E9;void*_tmp3EA;void*_tmp3EB;void*_tmp3EC;void*_tmp3ED;void*_tmp3EE;void*
_tmp3EF;void*_tmp3F0;int _tmp3F1;void*_tmp3F2;int _tmp3F3;void*_tmp3F4;struct Cyc_Absyn_ArrayInfo
_tmp3F5;void*_tmp3F6;struct Cyc_Absyn_Tqual _tmp3F7;struct Cyc_Absyn_Exp*_tmp3F8;
struct Cyc_Absyn_Conref*_tmp3F9;void*_tmp3FA;struct Cyc_Absyn_ArrayInfo _tmp3FB;
void*_tmp3FC;struct Cyc_Absyn_Tqual _tmp3FD;struct Cyc_Absyn_Exp*_tmp3FE;struct Cyc_Absyn_Conref*
_tmp3FF;void*_tmp400;struct Cyc_Absyn_FnInfo _tmp401;struct Cyc_List_List*_tmp402;
struct Cyc_Core_Opt*_tmp403;void*_tmp404;struct Cyc_List_List*_tmp405;int _tmp406;
struct Cyc_Absyn_VarargInfo*_tmp407;struct Cyc_List_List*_tmp408;struct Cyc_List_List*
_tmp409;void*_tmp40A;struct Cyc_Absyn_FnInfo _tmp40B;struct Cyc_List_List*_tmp40C;
struct Cyc_Core_Opt*_tmp40D;void*_tmp40E;struct Cyc_List_List*_tmp40F;int _tmp410;
struct Cyc_Absyn_VarargInfo*_tmp411;struct Cyc_List_List*_tmp412;struct Cyc_List_List*
_tmp413;void*_tmp414;struct Cyc_List_List*_tmp415;void*_tmp416;struct Cyc_List_List*
_tmp417;void*_tmp418;void*_tmp419;struct Cyc_List_List*_tmp41A;void*_tmp41B;void*
_tmp41C;struct Cyc_List_List*_tmp41D;void*_tmp41E;void*_tmp41F;void*_tmp420;void*
_tmp421;void*_tmp422;void*_tmp423;void*_tmp424;void*_tmp425;void*_tmp426;void*
_tmp427;void*_tmp428;void*_tmp429;void*_tmp42A;int _tmp42B;void*_tmp42C;int
_tmp42D;void*_tmp42E;void*_tmp42F;void*_tmp430;void*_tmp431;void*_tmp432;void*
_tmp433;_LL2AE: _tmp3A6=_tmp3A5.f1;if(_tmp3A6 <= (void*)4  || *((int*)_tmp3A6)!= 0)
goto _LL2B0;_tmp3A7=_tmp3A5.f2;if(_tmp3A7 <= (void*)4  || *((int*)_tmp3A7)!= 0)goto
_LL2B0;_LL2AF:({void*_tmp434[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp435="typecmp: can only compare closed types";
_tag_arr(_tmp435,sizeof(char),_get_zero_arr_size(_tmp435,39));}),_tag_arr(
_tmp434,sizeof(void*),0));});_LL2B0: _tmp3A8=_tmp3A5.f1;if(_tmp3A8 <= (void*)4  || *((
int*)_tmp3A8)!= 1)goto _LL2B2;_tmp3A9=((struct Cyc_Absyn_VarType_struct*)_tmp3A8)->f1;
_tmp3AA=_tmp3A5.f2;if(_tmp3AA <= (void*)4  || *((int*)_tmp3AA)!= 1)goto _LL2B2;
_tmp3AB=((struct Cyc_Absyn_VarType_struct*)_tmp3AA)->f1;_LL2B1: return Cyc_Core_intcmp(*((
int*)_check_null(_tmp3AB->identity)),*((int*)_check_null(_tmp3A9->identity)));
_LL2B2: _tmp3AC=_tmp3A5.f1;if(_tmp3AC <= (void*)4  || *((int*)_tmp3AC)!= 10)goto
_LL2B4;_tmp3AD=((struct Cyc_Absyn_AggrType_struct*)_tmp3AC)->f1;_tmp3AE=(void*)
_tmp3AD.aggr_info;_tmp3AF=_tmp3AD.targs;_tmp3B0=_tmp3A5.f2;if(_tmp3B0 <= (void*)4
 || *((int*)_tmp3B0)!= 10)goto _LL2B4;_tmp3B1=((struct Cyc_Absyn_AggrType_struct*)
_tmp3B0)->f1;_tmp3B2=(void*)_tmp3B1.aggr_info;_tmp3B3=_tmp3B1.targs;_LL2B3: {
struct _tuple1*_tmp437;struct _tuple5 _tmp436=Cyc_Absyn_aggr_kinded_name(_tmp3AE);
_tmp437=_tmp436.f2;{struct _tuple1*_tmp439;struct _tuple5 _tmp438=Cyc_Absyn_aggr_kinded_name(
_tmp3B2);_tmp439=_tmp438.f2;{int _tmp43A=Cyc_Absyn_qvar_cmp(_tmp437,_tmp439);if(
_tmp43A != 0)return _tmp43A;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3AF,_tmp3B3);}}}}_LL2B4: _tmp3B4=_tmp3A5.f1;if(_tmp3B4 <= (void*)4  || *((int*)
_tmp3B4)!= 12)goto _LL2B6;_tmp3B5=((struct Cyc_Absyn_EnumType_struct*)_tmp3B4)->f1;
_tmp3B6=_tmp3A5.f2;if(_tmp3B6 <= (void*)4  || *((int*)_tmp3B6)!= 12)goto _LL2B6;
_tmp3B7=((struct Cyc_Absyn_EnumType_struct*)_tmp3B6)->f1;_LL2B5: return Cyc_Absyn_qvar_cmp(
_tmp3B5,_tmp3B7);_LL2B6: _tmp3B8=_tmp3A5.f1;if(_tmp3B8 <= (void*)4  || *((int*)
_tmp3B8)!= 13)goto _LL2B8;_tmp3B9=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp3B8)->f1;
_tmp3BA=_tmp3A5.f2;if(_tmp3BA <= (void*)4  || *((int*)_tmp3BA)!= 13)goto _LL2B8;
_tmp3BB=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp3BA)->f1;_LL2B7: return((int(*)(
int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp3B9,
_tmp3BB);_LL2B8: _tmp3BC=_tmp3A5.f1;if(_tmp3BC <= (void*)4  || *((int*)_tmp3BC)!= 2)
goto _LL2BA;_tmp3BD=((struct Cyc_Absyn_TunionType_struct*)_tmp3BC)->f1;_tmp3BE=(
void*)_tmp3BD.tunion_info;if(*((int*)_tmp3BE)!= 1)goto _LL2BA;_tmp3BF=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3BE)->f1;_tmp3C0=*_tmp3BF;_tmp3C1=_tmp3BD.targs;_tmp3C2=(void*)_tmp3BD.rgn;
_tmp3C3=_tmp3A5.f2;if(_tmp3C3 <= (void*)4  || *((int*)_tmp3C3)!= 2)goto _LL2BA;
_tmp3C4=((struct Cyc_Absyn_TunionType_struct*)_tmp3C3)->f1;_tmp3C5=(void*)_tmp3C4.tunion_info;
if(*((int*)_tmp3C5)!= 1)goto _LL2BA;_tmp3C6=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3C5)->f1;_tmp3C7=*_tmp3C6;_tmp3C8=_tmp3C4.targs;_tmp3C9=(void*)_tmp3C4.rgn;
_LL2B9: if(_tmp3C7 == _tmp3C0)return 0;{int _tmp43B=Cyc_Absyn_qvar_cmp(_tmp3C7->name,
_tmp3C0->name);if(_tmp43B != 0)return _tmp43B;{int _tmp43C=Cyc_Tcutil_typecmp(
_tmp3C9,_tmp3C2);if(_tmp43C != 0)return _tmp43C;return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3C8,_tmp3C1);}}_LL2BA: _tmp3CA=_tmp3A5.f1;if(_tmp3CA <= (void*)4  || *((int*)
_tmp3CA)!= 3)goto _LL2BC;_tmp3CB=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3CA)->f1;_tmp3CC=(void*)_tmp3CB.field_info;if(*((int*)_tmp3CC)!= 1)goto
_LL2BC;_tmp3CD=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3CC)->f1;_tmp3CE=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3CC)->f2;_tmp3CF=_tmp3CB.targs;
_tmp3D0=_tmp3A5.f2;if(_tmp3D0 <= (void*)4  || *((int*)_tmp3D0)!= 3)goto _LL2BC;
_tmp3D1=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3D0)->f1;_tmp3D2=(void*)
_tmp3D1.field_info;if(*((int*)_tmp3D2)!= 1)goto _LL2BC;_tmp3D3=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp3D2)->f1;_tmp3D4=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3D2)->f2;
_tmp3D5=_tmp3D1.targs;_LL2BB: if(_tmp3D3 == _tmp3CD)return 0;{int _tmp43D=Cyc_Absyn_qvar_cmp(
_tmp3CD->name,_tmp3D3->name);if(_tmp43D != 0)return _tmp43D;{int _tmp43E=Cyc_Absyn_qvar_cmp(
_tmp3CE->name,_tmp3D4->name);if(_tmp43E != 0)return _tmp43E;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3D5,_tmp3CF);}}_LL2BC: _tmp3D6=_tmp3A5.f1;if(_tmp3D6 <= (
void*)4  || *((int*)_tmp3D6)!= 4)goto _LL2BE;_tmp3D7=((struct Cyc_Absyn_PointerType_struct*)
_tmp3D6)->f1;_tmp3D8=(void*)_tmp3D7.elt_typ;_tmp3D9=_tmp3D7.elt_tq;_tmp3DA=
_tmp3D7.ptr_atts;_tmp3DB=(void*)_tmp3DA.rgn;_tmp3DC=_tmp3DA.nullable;_tmp3DD=
_tmp3DA.bounds;_tmp3DE=_tmp3DA.zero_term;_tmp3DF=_tmp3A5.f2;if(_tmp3DF <= (void*)
4  || *((int*)_tmp3DF)!= 4)goto _LL2BE;_tmp3E0=((struct Cyc_Absyn_PointerType_struct*)
_tmp3DF)->f1;_tmp3E1=(void*)_tmp3E0.elt_typ;_tmp3E2=_tmp3E0.elt_tq;_tmp3E3=
_tmp3E0.ptr_atts;_tmp3E4=(void*)_tmp3E3.rgn;_tmp3E5=_tmp3E3.nullable;_tmp3E6=
_tmp3E3.bounds;_tmp3E7=_tmp3E3.zero_term;_LL2BD: {int _tmp43F=Cyc_Tcutil_typecmp(
_tmp3E1,_tmp3D8);if(_tmp43F != 0)return _tmp43F;{int _tmp440=Cyc_Tcutil_typecmp(
_tmp3E4,_tmp3DB);if(_tmp440 != 0)return _tmp440;{int _tmp441=Cyc_Tcutil_tqual_cmp(
_tmp3E2,_tmp3D9);if(_tmp441 != 0)return _tmp441;{int _tmp442=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3E6,_tmp3DD);if(_tmp442 != 0)return _tmp442;{int _tmp443=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3E7,_tmp3DE);if(_tmp443 != 0)return _tmp443;{void*_tmp444=(
void*)(Cyc_Absyn_compress_conref(_tmp3E6))->v;void*_tmp445;_LL2E3: if(_tmp444 <= (
void*)1  || *((int*)_tmp444)!= 0)goto _LL2E5;_tmp445=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp444)->f1;if((int)_tmp445 != 0)goto _LL2E5;_LL2E4: return 0;_LL2E5:;_LL2E6: goto
_LL2E2;_LL2E2:;}return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3E5,
_tmp3DC);}}}}}_LL2BE: _tmp3E8=_tmp3A5.f1;if(_tmp3E8 <= (void*)4  || *((int*)_tmp3E8)
!= 5)goto _LL2C0;_tmp3E9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E8)->f1;
_tmp3EA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E8)->f2;_tmp3EB=_tmp3A5.f2;
if(_tmp3EB <= (void*)4  || *((int*)_tmp3EB)!= 5)goto _LL2C0;_tmp3EC=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp3EB)->f1;_tmp3ED=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3EB)->f2;_LL2BF: if(_tmp3EC != _tmp3E9)return Cyc_Core_intcmp((int)((
unsigned int)_tmp3EC),(int)((unsigned int)_tmp3E9));if(_tmp3ED != _tmp3EA)return
Cyc_Core_intcmp((int)((unsigned int)_tmp3ED),(int)((unsigned int)_tmp3EA));
return 0;_LL2C0: _tmp3EE=_tmp3A5.f1;if((int)_tmp3EE != 1)goto _LL2C2;_tmp3EF=_tmp3A5.f2;
if((int)_tmp3EF != 1)goto _LL2C2;_LL2C1: return 0;_LL2C2: _tmp3F0=_tmp3A5.f1;if(
_tmp3F0 <= (void*)4  || *((int*)_tmp3F0)!= 6)goto _LL2C4;_tmp3F1=((struct Cyc_Absyn_DoubleType_struct*)
_tmp3F0)->f1;_tmp3F2=_tmp3A5.f2;if(_tmp3F2 <= (void*)4  || *((int*)_tmp3F2)!= 6)
goto _LL2C4;_tmp3F3=((struct Cyc_Absyn_DoubleType_struct*)_tmp3F2)->f1;_LL2C3: if(
_tmp3F1 == _tmp3F3)return 0;else{if(_tmp3F1)return - 1;else{return 1;}}_LL2C4: _tmp3F4=
_tmp3A5.f1;if(_tmp3F4 <= (void*)4  || *((int*)_tmp3F4)!= 7)goto _LL2C6;_tmp3F5=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F4)->f1;_tmp3F6=(void*)_tmp3F5.elt_type;
_tmp3F7=_tmp3F5.tq;_tmp3F8=_tmp3F5.num_elts;_tmp3F9=_tmp3F5.zero_term;_tmp3FA=
_tmp3A5.f2;if(_tmp3FA <= (void*)4  || *((int*)_tmp3FA)!= 7)goto _LL2C6;_tmp3FB=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3FA)->f1;_tmp3FC=(void*)_tmp3FB.elt_type;
_tmp3FD=_tmp3FB.tq;_tmp3FE=_tmp3FB.num_elts;_tmp3FF=_tmp3FB.zero_term;_LL2C5: {
int _tmp446=Cyc_Tcutil_tqual_cmp(_tmp3FD,_tmp3F7);if(_tmp446 != 0)return _tmp446;{
int _tmp447=Cyc_Tcutil_typecmp(_tmp3FC,_tmp3F6);if(_tmp447 != 0)return _tmp447;{int
_tmp448=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3F9,_tmp3FF);if(_tmp448 != 0)
return _tmp448;if(_tmp3F8 == _tmp3FE)return 0;if(_tmp3F8 == 0  || _tmp3FE == 0)({void*
_tmp449[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp44A="missing expression in array index";_tag_arr(_tmp44A,sizeof(
char),_get_zero_arr_size(_tmp44A,34));}),_tag_arr(_tmp449,sizeof(void*),0));});
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*
a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3F8,
_tmp3FE);}}}_LL2C6: _tmp400=_tmp3A5.f1;if(_tmp400 <= (void*)4  || *((int*)_tmp400)
!= 8)goto _LL2C8;_tmp401=((struct Cyc_Absyn_FnType_struct*)_tmp400)->f1;_tmp402=
_tmp401.tvars;_tmp403=_tmp401.effect;_tmp404=(void*)_tmp401.ret_typ;_tmp405=
_tmp401.args;_tmp406=_tmp401.c_varargs;_tmp407=_tmp401.cyc_varargs;_tmp408=
_tmp401.rgn_po;_tmp409=_tmp401.attributes;_tmp40A=_tmp3A5.f2;if(_tmp40A <= (void*)
4  || *((int*)_tmp40A)!= 8)goto _LL2C8;_tmp40B=((struct Cyc_Absyn_FnType_struct*)
_tmp40A)->f1;_tmp40C=_tmp40B.tvars;_tmp40D=_tmp40B.effect;_tmp40E=(void*)_tmp40B.ret_typ;
_tmp40F=_tmp40B.args;_tmp410=_tmp40B.c_varargs;_tmp411=_tmp40B.cyc_varargs;
_tmp412=_tmp40B.rgn_po;_tmp413=_tmp40B.attributes;_LL2C7:({void*_tmp44B[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp44C="typecmp: function types not handled";_tag_arr(_tmp44C,sizeof(char),
_get_zero_arr_size(_tmp44C,36));}),_tag_arr(_tmp44B,sizeof(void*),0));});_LL2C8:
_tmp414=_tmp3A5.f1;if(_tmp414 <= (void*)4  || *((int*)_tmp414)!= 9)goto _LL2CA;
_tmp415=((struct Cyc_Absyn_TupleType_struct*)_tmp414)->f1;_tmp416=_tmp3A5.f2;if(
_tmp416 <= (void*)4  || *((int*)_tmp416)!= 9)goto _LL2CA;_tmp417=((struct Cyc_Absyn_TupleType_struct*)
_tmp416)->f1;_LL2C9: return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,
_tmp417,_tmp415);_LL2CA: _tmp418=_tmp3A5.f1;if(_tmp418 <= (void*)4  || *((int*)
_tmp418)!= 11)goto _LL2CC;_tmp419=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp418)->f1;_tmp41A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp418)->f2;_tmp41B=
_tmp3A5.f2;if(_tmp41B <= (void*)4  || *((int*)_tmp41B)!= 11)goto _LL2CC;_tmp41C=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp41B)->f1;_tmp41D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp41B)->f2;_LL2CB: if(_tmp41C != _tmp419){if(_tmp41C == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp41D,_tmp41A);_LL2CC: _tmp41E=_tmp3A5.f1;if(_tmp41E <= (void*)4  || *((int*)
_tmp41E)!= 15)goto _LL2CE;_tmp41F=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp41E)->f1;_tmp420=_tmp3A5.f2;if(_tmp420 <= (void*)4  || *((int*)_tmp420)!= 15)
goto _LL2CE;_tmp421=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp420)->f1;
_LL2CD: return Cyc_Tcutil_typecmp(_tmp41F,_tmp421);_LL2CE: _tmp422=_tmp3A5.f1;if(
_tmp422 <= (void*)4  || *((int*)_tmp422)!= 14)goto _LL2D0;_tmp423=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp422)->f1;_tmp424=_tmp3A5.f2;if(_tmp424 <= (void*)
4  || *((int*)_tmp424)!= 14)goto _LL2D0;_tmp425=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp424)->f1;_LL2CF: return Cyc_Tcutil_typecmp(_tmp423,_tmp425);_LL2D0: _tmp426=
_tmp3A5.f1;if(_tmp426 <= (void*)4  || *((int*)_tmp426)!= 17)goto _LL2D2;_tmp427=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp426)->f1;_tmp428=_tmp3A5.f2;if(
_tmp428 <= (void*)4  || *((int*)_tmp428)!= 17)goto _LL2D2;_tmp429=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp428)->f1;_LL2D1: return Cyc_Tcutil_typecmp(_tmp427,
_tmp429);_LL2D2: _tmp42A=_tmp3A5.f1;if(_tmp42A <= (void*)4  || *((int*)_tmp42A)!= 
18)goto _LL2D4;_tmp42B=((struct Cyc_Absyn_TypeInt_struct*)_tmp42A)->f1;_tmp42C=
_tmp3A5.f2;if(_tmp42C <= (void*)4  || *((int*)_tmp42C)!= 18)goto _LL2D4;_tmp42D=((
struct Cyc_Absyn_TypeInt_struct*)_tmp42C)->f1;_LL2D3: return Cyc_Core_intcmp(
_tmp42B,_tmp42D);_LL2D4: _tmp42E=_tmp3A5.f1;if(_tmp42E <= (void*)4  || *((int*)
_tmp42E)!= 20)goto _LL2D6;_LL2D5: goto _LL2D7;_LL2D6: _tmp42F=_tmp3A5.f2;if(_tmp42F
<= (void*)4  || *((int*)_tmp42F)!= 20)goto _LL2D8;_LL2D7: goto _LL2D9;_LL2D8: _tmp430=
_tmp3A5.f1;if(_tmp430 <= (void*)4  || *((int*)_tmp430)!= 19)goto _LL2DA;_LL2D9: goto
_LL2DB;_LL2DA: _tmp431=_tmp3A5.f1;if(_tmp431 <= (void*)4  || *((int*)_tmp431)!= 21)
goto _LL2DC;_LL2DB: goto _LL2DD;_LL2DC: _tmp432=_tmp3A5.f2;if(_tmp432 <= (void*)4  || *((
int*)_tmp432)!= 21)goto _LL2DE;_LL2DD: goto _LL2DF;_LL2DE: _tmp433=_tmp3A5.f2;if(
_tmp433 <= (void*)4  || *((int*)_tmp433)!= 19)goto _LL2E0;_LL2DF:({void*_tmp44D[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp44E="typecmp: effects not handled";_tag_arr(_tmp44E,sizeof(char),
_get_zero_arr_size(_tmp44E,29));}),_tag_arr(_tmp44D,sizeof(void*),0));});_LL2E0:;
_LL2E1:({void*_tmp44F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp450="Unmatched case in typecmp";_tag_arr(
_tmp450,sizeof(char),_get_zero_arr_size(_tmp450,26));}),_tag_arr(_tmp44F,sizeof(
void*),0));});_LL2AD:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp451=
Cyc_Tcutil_compress(t);_LL2E8: if(_tmp451 <= (void*)4  || *((int*)_tmp451)!= 5)goto
_LL2EA;_LL2E9: goto _LL2EB;_LL2EA: if((int)_tmp451 != 1)goto _LL2EC;_LL2EB: goto _LL2ED;
_LL2EC: if(_tmp451 <= (void*)4  || *((int*)_tmp451)!= 6)goto _LL2EE;_LL2ED: goto
_LL2EF;_LL2EE: if(_tmp451 <= (void*)4  || *((int*)_tmp451)!= 12)goto _LL2F0;_LL2EF:
goto _LL2F1;_LL2F0: if(_tmp451 <= (void*)4  || *((int*)_tmp451)!= 13)goto _LL2F2;
_LL2F1: return 1;_LL2F2:;_LL2F3: return 0;_LL2E7:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){struct _tuple6 _tmp453=({struct _tuple6 _tmp452;_tmp452.f1=Cyc_Tcutil_compress(
t1);_tmp452.f2=Cyc_Tcutil_compress(t2);_tmp452;});void*_tmp454;int _tmp455;void*
_tmp456;int _tmp457;void*_tmp458;void*_tmp459;void*_tmp45A;void*_tmp45B;void*
_tmp45C;void*_tmp45D;void*_tmp45E;void*_tmp45F;void*_tmp460;void*_tmp461;void*
_tmp462;void*_tmp463;void*_tmp464;void*_tmp465;void*_tmp466;void*_tmp467;void*
_tmp468;void*_tmp469;void*_tmp46A;void*_tmp46B;void*_tmp46C;void*_tmp46D;void*
_tmp46E;void*_tmp46F;void*_tmp470;void*_tmp471;void*_tmp472;void*_tmp473;void*
_tmp474;void*_tmp475;void*_tmp476;void*_tmp477;void*_tmp478;void*_tmp479;void*
_tmp47A;void*_tmp47B;void*_tmp47C;void*_tmp47D;void*_tmp47E;void*_tmp47F;void*
_tmp480;void*_tmp481;void*_tmp482;void*_tmp483;void*_tmp484;void*_tmp485;void*
_tmp486;_LL2F5: _tmp454=_tmp453.f1;if(_tmp454 <= (void*)4  || *((int*)_tmp454)!= 6)
goto _LL2F7;_tmp455=((struct Cyc_Absyn_DoubleType_struct*)_tmp454)->f1;_tmp456=
_tmp453.f2;if(_tmp456 <= (void*)4  || *((int*)_tmp456)!= 6)goto _LL2F7;_tmp457=((
struct Cyc_Absyn_DoubleType_struct*)_tmp456)->f1;_LL2F6: return !_tmp457  && _tmp455;
_LL2F7: _tmp458=_tmp453.f1;if(_tmp458 <= (void*)4  || *((int*)_tmp458)!= 6)goto
_LL2F9;_tmp459=_tmp453.f2;if((int)_tmp459 != 1)goto _LL2F9;_LL2F8: goto _LL2FA;
_LL2F9: _tmp45A=_tmp453.f1;if(_tmp45A <= (void*)4  || *((int*)_tmp45A)!= 6)goto
_LL2FB;_tmp45B=_tmp453.f2;if(_tmp45B <= (void*)4  || *((int*)_tmp45B)!= 5)goto
_LL2FB;_LL2FA: goto _LL2FC;_LL2FB: _tmp45C=_tmp453.f1;if(_tmp45C <= (void*)4  || *((
int*)_tmp45C)!= 6)goto _LL2FD;_tmp45D=_tmp453.f2;if(_tmp45D <= (void*)4  || *((int*)
_tmp45D)!= 14)goto _LL2FD;_LL2FC: goto _LL2FE;_LL2FD: _tmp45E=_tmp453.f1;if((int)
_tmp45E != 1)goto _LL2FF;_tmp45F=_tmp453.f2;if(_tmp45F <= (void*)4  || *((int*)
_tmp45F)!= 14)goto _LL2FF;_LL2FE: goto _LL300;_LL2FF: _tmp460=_tmp453.f1;if(_tmp460
<= (void*)4  || *((int*)_tmp460)!= 6)goto _LL301;_tmp461=_tmp453.f2;if(_tmp461 <= (
void*)4  || *((int*)_tmp461)!= 17)goto _LL301;_LL300: goto _LL302;_LL301: _tmp462=
_tmp453.f1;if((int)_tmp462 != 1)goto _LL303;_tmp463=_tmp453.f2;if(_tmp463 <= (void*)
4  || *((int*)_tmp463)!= 17)goto _LL303;_LL302: goto _LL304;_LL303: _tmp464=_tmp453.f1;
if((int)_tmp464 != 1)goto _LL305;_tmp465=_tmp453.f2;if(_tmp465 <= (void*)4  || *((
int*)_tmp465)!= 5)goto _LL305;_LL304: return 1;_LL305: _tmp466=_tmp453.f1;if(_tmp466
<= (void*)4  || *((int*)_tmp466)!= 5)goto _LL307;_tmp467=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp466)->f2;if((int)_tmp467 != 3)goto _LL307;_tmp468=_tmp453.f2;if(_tmp468 <= (
void*)4  || *((int*)_tmp468)!= 5)goto _LL307;_tmp469=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp468)->f2;if((int)_tmp469 != 3)goto _LL307;_LL306: return 0;_LL307: _tmp46A=
_tmp453.f1;if(_tmp46A <= (void*)4  || *((int*)_tmp46A)!= 5)goto _LL309;_tmp46B=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46A)->f2;if((int)_tmp46B != 3)goto
_LL309;_LL308: goto _LL30A;_LL309: _tmp46C=_tmp453.f1;if(_tmp46C <= (void*)4  || *((
int*)_tmp46C)!= 5)goto _LL30B;_tmp46D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46C)->f2;if((int)_tmp46D != 2)goto _LL30B;_tmp46E=_tmp453.f2;if((int)_tmp46E != 
1)goto _LL30B;_LL30A: goto _LL30C;_LL30B: _tmp46F=_tmp453.f1;if(_tmp46F <= (void*)4
 || *((int*)_tmp46F)!= 5)goto _LL30D;_tmp470=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46F)->f2;if((int)_tmp470 != 2)goto _LL30D;_tmp471=_tmp453.f2;if(_tmp471 <= (
void*)4  || *((int*)_tmp471)!= 5)goto _LL30D;_tmp472=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp471)->f2;if((int)_tmp472 != 1)goto _LL30D;_LL30C: goto _LL30E;_LL30D: _tmp473=
_tmp453.f1;if(_tmp473 <= (void*)4  || *((int*)_tmp473)!= 5)goto _LL30F;_tmp474=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp473)->f2;if((int)_tmp474 != 2)goto
_LL30F;_tmp475=_tmp453.f2;if(_tmp475 <= (void*)4  || *((int*)_tmp475)!= 5)goto
_LL30F;_tmp476=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp475)->f2;if((int)
_tmp476 != 0)goto _LL30F;_LL30E: goto _LL310;_LL30F: _tmp477=_tmp453.f1;if(_tmp477 <= (
void*)4  || *((int*)_tmp477)!= 5)goto _LL311;_tmp478=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp477)->f2;if((int)_tmp478 != 1)goto _LL311;_tmp479=_tmp453.f2;if(_tmp479 <= (
void*)4  || *((int*)_tmp479)!= 5)goto _LL311;_tmp47A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp479)->f2;if((int)_tmp47A != 0)goto _LL311;_LL310: goto _LL312;_LL311: _tmp47B=
_tmp453.f1;if(_tmp47B <= (void*)4  || *((int*)_tmp47B)!= 17)goto _LL313;_tmp47C=
_tmp453.f2;if(_tmp47C <= (void*)4  || *((int*)_tmp47C)!= 5)goto _LL313;_tmp47D=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp47C)->f2;if((int)_tmp47D != 1)goto
_LL313;_LL312: goto _LL314;_LL313: _tmp47E=_tmp453.f1;if(_tmp47E <= (void*)4  || *((
int*)_tmp47E)!= 17)goto _LL315;_tmp47F=_tmp453.f2;if(_tmp47F <= (void*)4  || *((int*)
_tmp47F)!= 5)goto _LL315;_tmp480=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47F)->f2;
if((int)_tmp480 != 0)goto _LL315;_LL314: goto _LL316;_LL315: _tmp481=_tmp453.f1;if(
_tmp481 <= (void*)4  || *((int*)_tmp481)!= 14)goto _LL317;_tmp482=_tmp453.f2;if(
_tmp482 <= (void*)4  || *((int*)_tmp482)!= 5)goto _LL317;_tmp483=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp482)->f2;if((int)_tmp483 != 1)goto _LL317;_LL316: goto _LL318;_LL317: _tmp484=
_tmp453.f1;if(_tmp484 <= (void*)4  || *((int*)_tmp484)!= 14)goto _LL319;_tmp485=
_tmp453.f2;if(_tmp485 <= (void*)4  || *((int*)_tmp485)!= 5)goto _LL319;_tmp486=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp485)->f2;if((int)_tmp486 != 0)goto
_LL319;_LL318: return 1;_LL319:;_LL31A: return 0;_LL2F4:;}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct Cyc_Core_Opt*
max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);
if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(
t1,(void*)max_arith_type->v))max_arith_type=({struct Cyc_Core_Opt*_tmp487=
_cycalloc(sizeof(*_tmp487));_tmp487->v=(void*)t1;_tmp487;});}}}if(max_arith_type
!= 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_String_pa_struct _tmp48B;_tmp48B.tag=0;_tmp48B.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{struct Cyc_String_pa_struct
_tmp48A;_tmp48A.tag=0;_tmp48A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp488[2]={& _tmp48A,& _tmp48B};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,({const char*_tmp489="type mismatch: expecting %s but found %s";
_tag_arr(_tmp489,sizeof(char),_get_zero_arr_size(_tmp489,41));}),_tag_arr(
_tmp488,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp48C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL31C: if(_tmp48C <= (void*)4  || *((int*)_tmp48C)!= 4)goto _LL31E;_LL31D: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL31B;_LL31E:;_LL31F: return 0;_LL31B:;}
return 1;}int Cyc_Tcutil_is_integral_type(void*t){void*_tmp48D=Cyc_Tcutil_compress(
t);_LL321: if(_tmp48D <= (void*)4  || *((int*)_tmp48D)!= 5)goto _LL323;_LL322: goto
_LL324;_LL323: if(_tmp48D <= (void*)4  || *((int*)_tmp48D)!= 14)goto _LL325;_LL324:
goto _LL326;_LL325: if(_tmp48D <= (void*)4  || *((int*)_tmp48D)!= 17)goto _LL327;
_LL326: goto _LL328;_LL327: if(_tmp48D <= (void*)4  || *((int*)_tmp48D)!= 12)goto
_LL329;_LL328: goto _LL32A;_LL329: if(_tmp48D <= (void*)4  || *((int*)_tmp48D)!= 13)
goto _LL32B;_LL32A: return 1;_LL32B:;_LL32C: return 0;_LL320:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*
_tmp48E[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp48F="integral size mismatch; conversion supplied";
_tag_arr(_tmp48F,sizeof(char),_get_zero_arr_size(_tmp48F,44));}),_tag_arr(
_tmp48E,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp490[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp491="integral size mismatch; conversion supplied";
_tag_arr(_tmp491,sizeof(char),_get_zero_arr_size(_tmp491,44));}),_tag_arr(
_tmp490,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp493=({struct _tuple6 _tmp492;_tmp492.f1=
t1;_tmp492.f2=t2;_tmp492;});void*_tmp494;struct Cyc_Absyn_PtrInfo _tmp495;void*
_tmp496;struct Cyc_Absyn_PtrInfo _tmp497;void*_tmp498;struct Cyc_Absyn_ArrayInfo
_tmp499;void*_tmp49A;struct Cyc_Absyn_Tqual _tmp49B;struct Cyc_Absyn_Exp*_tmp49C;
struct Cyc_Absyn_Conref*_tmp49D;void*_tmp49E;struct Cyc_Absyn_ArrayInfo _tmp49F;
void*_tmp4A0;struct Cyc_Absyn_Tqual _tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Conref*
_tmp4A3;void*_tmp4A4;struct Cyc_Absyn_TunionFieldInfo _tmp4A5;void*_tmp4A6;struct
Cyc_Absyn_Tuniondecl*_tmp4A7;struct Cyc_Absyn_Tunionfield*_tmp4A8;struct Cyc_List_List*
_tmp4A9;void*_tmp4AA;struct Cyc_Absyn_TunionInfo _tmp4AB;void*_tmp4AC;struct Cyc_Absyn_Tuniondecl**
_tmp4AD;struct Cyc_Absyn_Tuniondecl*_tmp4AE;struct Cyc_List_List*_tmp4AF;void*
_tmp4B0;struct Cyc_Absyn_PtrInfo _tmp4B1;void*_tmp4B2;struct Cyc_Absyn_Tqual _tmp4B3;
struct Cyc_Absyn_PtrAtts _tmp4B4;void*_tmp4B5;struct Cyc_Absyn_Conref*_tmp4B6;
struct Cyc_Absyn_Conref*_tmp4B7;struct Cyc_Absyn_Conref*_tmp4B8;void*_tmp4B9;
struct Cyc_Absyn_TunionInfo _tmp4BA;void*_tmp4BB;struct Cyc_Absyn_Tuniondecl**
_tmp4BC;struct Cyc_Absyn_Tuniondecl*_tmp4BD;struct Cyc_List_List*_tmp4BE;void*
_tmp4BF;void*_tmp4C0;void*_tmp4C1;void*_tmp4C2;void*_tmp4C3;void*_tmp4C4;_LL32E:
_tmp494=_tmp493.f1;if(_tmp494 <= (void*)4  || *((int*)_tmp494)!= 4)goto _LL330;
_tmp495=((struct Cyc_Absyn_PointerType_struct*)_tmp494)->f1;_tmp496=_tmp493.f2;
if(_tmp496 <= (void*)4  || *((int*)_tmp496)!= 4)goto _LL330;_tmp497=((struct Cyc_Absyn_PointerType_struct*)
_tmp496)->f1;_LL32F: {int okay=1;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp495.ptr_atts).nullable,(
_tmp497.ptr_atts).nullable)){void*_tmp4C5=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp495.ptr_atts).nullable))->v;
int _tmp4C6;_LL33D: if(_tmp4C5 <= (void*)1  || *((int*)_tmp4C5)!= 0)goto _LL33F;
_tmp4C6=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4C5)->f1;_LL33E: okay=!
_tmp4C6;goto _LL33C;_LL33F:;_LL340:({void*_tmp4C7[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp4C8="silent_castable conref not eq";
_tag_arr(_tmp4C8,sizeof(char),_get_zero_arr_size(_tmp4C8,30));}),_tag_arr(
_tmp4C7,sizeof(void*),0));});_LL33C:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(
_tmp495.ptr_atts).bounds,(_tmp497.ptr_atts).bounds)){struct _tuple6 _tmp4CA=({
struct _tuple6 _tmp4C9;_tmp4C9.f1=(void*)(Cyc_Absyn_compress_conref((_tmp495.ptr_atts).bounds))->v;
_tmp4C9.f2=(void*)(Cyc_Absyn_compress_conref((_tmp497.ptr_atts).bounds))->v;
_tmp4C9;});void*_tmp4CB;void*_tmp4CC;void*_tmp4CD;void*_tmp4CE;void*_tmp4CF;
_LL342: _tmp4CB=_tmp4CA.f1;if(_tmp4CB <= (void*)1  || *((int*)_tmp4CB)!= 0)goto
_LL344;_tmp4CC=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4CB)->f1;_tmp4CD=
_tmp4CA.f2;if(_tmp4CD <= (void*)1  || *((int*)_tmp4CD)!= 0)goto _LL344;_tmp4CE=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4CD)->f1;_LL343:{struct _tuple6
_tmp4D1=({struct _tuple6 _tmp4D0;_tmp4D0.f1=_tmp4CC;_tmp4D0.f2=_tmp4CE;_tmp4D0;});
void*_tmp4D2;void*_tmp4D3;void*_tmp4D4;void*_tmp4D5;void*_tmp4D6;struct Cyc_Absyn_Exp*
_tmp4D7;void*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D9;void*_tmp4DA;void*_tmp4DB;
struct Cyc_Absyn_Exp*_tmp4DC;void*_tmp4DD;void*_tmp4DE;void*_tmp4DF;void*_tmp4E0;
void*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E2;void*_tmp4E3;void*_tmp4E4;void*_tmp4E5;
void*_tmp4E6;_LL349: _tmp4D2=_tmp4D1.f1;if(_tmp4D2 <= (void*)1  || *((int*)_tmp4D2)
!= 0)goto _LL34B;_tmp4D3=_tmp4D1.f2;if((int)_tmp4D3 != 0)goto _LL34B;_LL34A: goto
_LL34C;_LL34B: _tmp4D4=_tmp4D1.f1;if((int)_tmp4D4 != 0)goto _LL34D;_tmp4D5=_tmp4D1.f2;
if((int)_tmp4D5 != 0)goto _LL34D;_LL34C: okay=1;goto _LL348;_LL34D: _tmp4D6=_tmp4D1.f1;
if(_tmp4D6 <= (void*)1  || *((int*)_tmp4D6)!= 0)goto _LL34F;_tmp4D7=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4D6)->f1;_tmp4D8=_tmp4D1.f2;if(_tmp4D8 <= (void*)1  || *((int*)_tmp4D8)!= 0)
goto _LL34F;_tmp4D9=((struct Cyc_Absyn_Upper_b_struct*)_tmp4D8)->f1;_LL34E: okay=
okay  && Cyc_Evexp_lte_const_exp(_tmp4D9,_tmp4D7);if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,(_tmp497.ptr_atts).zero_term))({void*_tmp4E7[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp4E8="implicit cast to shorter array";
_tag_arr(_tmp4E8,sizeof(char),_get_zero_arr_size(_tmp4E8,31));}),_tag_arr(
_tmp4E7,sizeof(void*),0));});goto _LL348;_LL34F: _tmp4DA=_tmp4D1.f1;if((int)
_tmp4DA != 0)goto _LL351;_tmp4DB=_tmp4D1.f2;if(_tmp4DB <= (void*)1  || *((int*)
_tmp4DB)!= 0)goto _LL351;_tmp4DC=((struct Cyc_Absyn_Upper_b_struct*)_tmp4DB)->f1;
_LL350: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(
_tmp495.ptr_atts).zero_term) && Cyc_Tcutil_is_bound_one((_tmp497.ptr_atts).bounds))
goto _LL348;okay=0;goto _LL348;_LL351: _tmp4DD=_tmp4D1.f1;if(_tmp4DD <= (void*)1  || *((
int*)_tmp4DD)!= 1)goto _LL353;_tmp4DE=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp4DD)->f1;_tmp4DF=_tmp4D1.f2;if(_tmp4DF <= (void*)1  || *((int*)_tmp4DF)!= 1)
goto _LL353;_tmp4E0=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4DF)->f1;
_LL352: if(!Cyc_Tcutil_unify(_tmp4DE,_tmp4E0)){struct _tuple6 _tmp4EA=({struct
_tuple6 _tmp4E9;_tmp4E9.f1=Cyc_Tcutil_compress(_tmp4DE);_tmp4E9.f2=Cyc_Tcutil_compress(
_tmp4E0);_tmp4E9;});void*_tmp4EB;int _tmp4EC;void*_tmp4ED;int _tmp4EE;_LL35A:
_tmp4EB=_tmp4EA.f1;if(_tmp4EB <= (void*)4  || *((int*)_tmp4EB)!= 18)goto _LL35C;
_tmp4EC=((struct Cyc_Absyn_TypeInt_struct*)_tmp4EB)->f1;_tmp4ED=_tmp4EA.f2;if(
_tmp4ED <= (void*)4  || *((int*)_tmp4ED)!= 18)goto _LL35C;_tmp4EE=((struct Cyc_Absyn_TypeInt_struct*)
_tmp4ED)->f1;_LL35B: if(_tmp4EC < _tmp4EE)okay=0;goto _LL359;_LL35C:;_LL35D: okay=0;
goto _LL359;_LL359:;}goto _LL348;_LL353: _tmp4E1=_tmp4D1.f1;if(_tmp4E1 <= (void*)1
 || *((int*)_tmp4E1)!= 0)goto _LL355;_tmp4E2=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4E1)->f1;_tmp4E3=_tmp4D1.f2;if(_tmp4E3 <= (void*)1  || *((int*)_tmp4E3)!= 1)
goto _LL355;_tmp4E4=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4E3)->f1;
_LL354: {unsigned int _tmp4F0;int _tmp4F1;struct _tuple7 _tmp4EF=Cyc_Evexp_eval_const_uint_exp(
_tmp4E2);_tmp4F0=_tmp4EF.f1;_tmp4F1=_tmp4EF.f2;if(!_tmp4F1){okay=0;goto _LL348;}{
void*_tmp4F2=Cyc_Tcutil_compress(_tmp4E4);int _tmp4F3;_LL35F: if(_tmp4F2 <= (void*)
4  || *((int*)_tmp4F2)!= 18)goto _LL361;_tmp4F3=((struct Cyc_Absyn_TypeInt_struct*)
_tmp4F2)->f1;_LL360: if(_tmp4F0 < _tmp4F3)okay=0;goto _LL35E;_LL361:;_LL362: okay=0;
goto _LL35E;_LL35E:;}goto _LL348;}_LL355: _tmp4E5=_tmp4D1.f1;if(_tmp4E5 <= (void*)1
 || *((int*)_tmp4E5)!= 1)goto _LL357;_LL356: goto _LL358;_LL357: _tmp4E6=_tmp4D1.f2;
if(_tmp4E6 <= (void*)1  || *((int*)_tmp4E6)!= 1)goto _LL348;_LL358: okay=0;goto
_LL348;_LL348:;}goto _LL341;_LL344: _tmp4CF=_tmp4CA.f1;if((int)_tmp4CF != 0)goto
_LL346;_LL345: okay=0;goto _LL341;_LL346:;_LL347:({void*_tmp4F4[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp4F5="silent_castable conrefs didn't unify";
_tag_arr(_tmp4F5,sizeof(char),_get_zero_arr_size(_tmp4F5,37));}),_tag_arr(
_tmp4F4,sizeof(void*),0));});_LL341:;}okay=okay  && Cyc_Tcutil_unify((void*)
_tmp495.elt_typ,(void*)_tmp497.elt_typ);okay=okay  && (Cyc_Tcutil_unify((void*)(
_tmp495.ptr_atts).rgn,(void*)(_tmp497.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(
te,(void*)(_tmp495.ptr_atts).rgn,(void*)(_tmp497.ptr_atts).rgn));okay=okay  && (!(
_tmp495.elt_tq).real_const  || (_tmp497.elt_tq).real_const);okay=okay  && (((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp495.ptr_atts).zero_term,(_tmp497.ptr_atts).zero_term) || ((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,(_tmp495.ptr_atts).zero_term)
 && (_tmp497.elt_tq).real_const);return okay;}_LL330: _tmp498=_tmp493.f1;if(
_tmp498 <= (void*)4  || *((int*)_tmp498)!= 7)goto _LL332;_tmp499=((struct Cyc_Absyn_ArrayType_struct*)
_tmp498)->f1;_tmp49A=(void*)_tmp499.elt_type;_tmp49B=_tmp499.tq;_tmp49C=_tmp499.num_elts;
_tmp49D=_tmp499.zero_term;_tmp49E=_tmp493.f2;if(_tmp49E <= (void*)4  || *((int*)
_tmp49E)!= 7)goto _LL332;_tmp49F=((struct Cyc_Absyn_ArrayType_struct*)_tmp49E)->f1;
_tmp4A0=(void*)_tmp49F.elt_type;_tmp4A1=_tmp49F.tq;_tmp4A2=_tmp49F.num_elts;
_tmp4A3=_tmp49F.zero_term;_LL331: {int okay;okay=((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp49D,_tmp4A3) && ((_tmp49C != 0  && _tmp4A2 != 0) && Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp49C,(struct Cyc_Absyn_Exp*)_tmp4A2));return(okay  && Cyc_Tcutil_unify(
_tmp49A,_tmp4A0)) && (!_tmp49B.real_const  || _tmp4A1.real_const);}_LL332: _tmp4A4=
_tmp493.f1;if(_tmp4A4 <= (void*)4  || *((int*)_tmp4A4)!= 3)goto _LL334;_tmp4A5=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4A4)->f1;_tmp4A6=(void*)_tmp4A5.field_info;
if(*((int*)_tmp4A6)!= 1)goto _LL334;_tmp4A7=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp4A6)->f1;_tmp4A8=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4A6)->f2;
_tmp4A9=_tmp4A5.targs;_tmp4AA=_tmp493.f2;if(_tmp4AA <= (void*)4  || *((int*)
_tmp4AA)!= 2)goto _LL334;_tmp4AB=((struct Cyc_Absyn_TunionType_struct*)_tmp4AA)->f1;
_tmp4AC=(void*)_tmp4AB.tunion_info;if(*((int*)_tmp4AC)!= 1)goto _LL334;_tmp4AD=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp4AC)->f1;_tmp4AE=*_tmp4AD;_tmp4AF=
_tmp4AB.targs;_LL333: if((_tmp4A7 == _tmp4AE  || Cyc_Absyn_qvar_cmp(_tmp4A7->name,
_tmp4AE->name)== 0) && _tmp4A8->typs == 0){for(0;_tmp4A9 != 0  && _tmp4AF != 0;(
_tmp4A9=_tmp4A9->tl,_tmp4AF=_tmp4AF->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4A9->hd,(
void*)_tmp4AF->hd))break;}if(_tmp4A9 == 0  && _tmp4AF == 0)return 1;}return 0;_LL334:
_tmp4B0=_tmp493.f1;if(_tmp4B0 <= (void*)4  || *((int*)_tmp4B0)!= 4)goto _LL336;
_tmp4B1=((struct Cyc_Absyn_PointerType_struct*)_tmp4B0)->f1;_tmp4B2=(void*)
_tmp4B1.elt_typ;_tmp4B3=_tmp4B1.elt_tq;_tmp4B4=_tmp4B1.ptr_atts;_tmp4B5=(void*)
_tmp4B4.rgn;_tmp4B6=_tmp4B4.nullable;_tmp4B7=_tmp4B4.bounds;_tmp4B8=_tmp4B4.zero_term;
_tmp4B9=_tmp493.f2;if(_tmp4B9 <= (void*)4  || *((int*)_tmp4B9)!= 2)goto _LL336;
_tmp4BA=((struct Cyc_Absyn_TunionType_struct*)_tmp4B9)->f1;_tmp4BB=(void*)_tmp4BA.tunion_info;
if(*((int*)_tmp4BB)!= 1)goto _LL336;_tmp4BC=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp4BB)->f1;_tmp4BD=*_tmp4BC;_tmp4BE=_tmp4BA.targs;_tmp4BF=(void*)_tmp4BA.rgn;
_LL335:{void*_tmp4F6=Cyc_Tcutil_compress(_tmp4B2);struct Cyc_Absyn_TunionFieldInfo
_tmp4F7;void*_tmp4F8;struct Cyc_Absyn_Tuniondecl*_tmp4F9;struct Cyc_Absyn_Tunionfield*
_tmp4FA;struct Cyc_List_List*_tmp4FB;_LL364: if(_tmp4F6 <= (void*)4  || *((int*)
_tmp4F6)!= 3)goto _LL366;_tmp4F7=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp4F6)->f1;_tmp4F8=(void*)_tmp4F7.field_info;if(*((int*)_tmp4F8)!= 1)goto
_LL366;_tmp4F9=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4F8)->f1;_tmp4FA=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4F8)->f2;_tmp4FB=_tmp4F7.targs;
_LL365: if(!Cyc_Tcutil_unify(_tmp4B5,_tmp4BF) && !Cyc_Tcenv_region_outlives(te,
_tmp4B5,_tmp4BF))return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4B6,
Cyc_Absyn_false_conref))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp4B7,Cyc_Absyn_bounds_one_conref))return 0;if(!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp4B8,Cyc_Absyn_false_conref))return 0;if(Cyc_Absyn_qvar_cmp(_tmp4BD->name,
_tmp4F9->name)== 0  && _tmp4FA->typs != 0){int okay=1;for(0;_tmp4FB != 0  && _tmp4BE
!= 0;(_tmp4FB=_tmp4FB->tl,_tmp4BE=_tmp4BE->tl)){if(!Cyc_Tcutil_unify((void*)
_tmp4FB->hd,(void*)_tmp4BE->hd)){okay=0;break;}}if((!okay  || _tmp4FB != 0) || 
_tmp4BE != 0)return 0;return 1;}goto _LL363;_LL366:;_LL367: goto _LL363;_LL363:;}
return 0;_LL336: _tmp4C0=_tmp493.f1;if(_tmp4C0 <= (void*)4  || *((int*)_tmp4C0)!= 14)
goto _LL338;_tmp4C1=_tmp493.f2;if(_tmp4C1 <= (void*)4  || *((int*)_tmp4C1)!= 5)goto
_LL338;_tmp4C2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4C1)->f2;if((int)
_tmp4C2 != 2)goto _LL338;_LL337: goto _LL339;_LL338: _tmp4C3=_tmp493.f1;if(_tmp4C3 <= (
void*)4  || *((int*)_tmp4C3)!= 17)goto _LL33A;_tmp4C4=_tmp493.f2;if(_tmp4C4 <= (
void*)4  || *((int*)_tmp4C4)!= 5)goto _LL33A;_LL339: return 1;_LL33A:;_LL33B: return
Cyc_Tcutil_unify(t1,t2);_LL32D:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*
_tmp4FC=Cyc_Tcutil_compress(t);_LL369: if(_tmp4FC <= (void*)4  || *((int*)_tmp4FC)
!= 4)goto _LL36B;_LL36A: return 1;_LL36B:;_LL36C: return 0;_LL368:;}int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp4FD=(void*)e->r;void*_tmp4FE;int _tmp4FF;void*
_tmp500;char _tmp501;void*_tmp502;short _tmp503;void*_tmp504;long long _tmp505;void*
_tmp506;struct Cyc_Absyn_Exp*_tmp507;_LL36E: if(*((int*)_tmp4FD)!= 0)goto _LL370;
_tmp4FE=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4FD)->f1;if(_tmp4FE <= (void*)
1  || *((int*)_tmp4FE)!= 2)goto _LL370;_tmp4FF=((struct Cyc_Absyn_Int_c_struct*)
_tmp4FE)->f2;if(_tmp4FF != 0)goto _LL370;_LL36F: goto _LL371;_LL370: if(*((int*)
_tmp4FD)!= 0)goto _LL372;_tmp500=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4FD)->f1;
if(_tmp500 <= (void*)1  || *((int*)_tmp500)!= 0)goto _LL372;_tmp501=((struct Cyc_Absyn_Char_c_struct*)
_tmp500)->f2;if(_tmp501 != 0)goto _LL372;_LL371: goto _LL373;_LL372: if(*((int*)
_tmp4FD)!= 0)goto _LL374;_tmp502=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4FD)->f1;
if(_tmp502 <= (void*)1  || *((int*)_tmp502)!= 1)goto _LL374;_tmp503=((struct Cyc_Absyn_Short_c_struct*)
_tmp502)->f2;if(_tmp503 != 0)goto _LL374;_LL373: goto _LL375;_LL374: if(*((int*)
_tmp4FD)!= 0)goto _LL376;_tmp504=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4FD)->f1;
if(_tmp504 <= (void*)1  || *((int*)_tmp504)!= 3)goto _LL376;_tmp505=((struct Cyc_Absyn_LongLong_c_struct*)
_tmp504)->f2;if(_tmp505 != 0)goto _LL376;_LL375: return 1;_LL376: if(*((int*)_tmp4FD)
!= 15)goto _LL378;_tmp506=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp4FD)->f1;
_tmp507=((struct Cyc_Absyn_Cast_e_struct*)_tmp4FD)->f2;_LL377: return Cyc_Tcutil_is_zero(
_tmp507) && Cyc_Tcutil_admits_zero(_tmp506);_LL378:;_LL379: return 0;_LL36D:;}
struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(
void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt
Cyc_Tcutil_mk={(void*)((void*)1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*
te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(
e1)){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp508=
_cycalloc(sizeof(*_tmp508));_tmp508[0]=({struct Cyc_Absyn_Const_e_struct _tmp509;
_tmp509.tag=0;_tmp509.f1=(void*)((void*)0);_tmp509;});_tmp508;})));{struct Cyc_List_List*
_tmp50A=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp50B=({struct Cyc_Absyn_PointerType_struct*_tmp50C=_cycalloc(sizeof(*_tmp50C));
_tmp50C[0]=({struct Cyc_Absyn_PointerType_struct _tmp50D;_tmp50D.tag=4;_tmp50D.f1=({
struct Cyc_Absyn_PtrInfo _tmp50E;_tmp50E.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp511=_cycalloc(sizeof(*
_tmp511));_tmp511->v=_tmp50A;_tmp511;}));_tmp50E.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp50E.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp50F;_tmp50F.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp510=_cycalloc(
sizeof(*_tmp510));_tmp510->v=_tmp50A;_tmp510;}));_tmp50F.nullable=Cyc_Absyn_true_conref;
_tmp50F.bounds=Cyc_Absyn_empty_conref();_tmp50F.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp50F.ptrloc=0;_tmp50F;});_tmp50E;});_tmp50D;});
_tmp50C;});(void*)(((struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)
_tmp50B));return Cyc_Tcutil_coerce_arg(te,e1,t2);}}return 0;}struct _tagged_arr Cyc_Tcutil_coercion2string(
void*c){void*_tmp512=c;_LL37B: if((int)_tmp512 != 0)goto _LL37D;_LL37C: return({
const char*_tmp513="unknown";_tag_arr(_tmp513,sizeof(char),_get_zero_arr_size(
_tmp513,8));});_LL37D: if((int)_tmp512 != 1)goto _LL37F;_LL37E: return({const char*
_tmp514="no coercion";_tag_arr(_tmp514,sizeof(char),_get_zero_arr_size(_tmp514,
12));});_LL37F: if((int)_tmp512 != 2)goto _LL381;_LL380: return({const char*_tmp515="null check";
_tag_arr(_tmp515,sizeof(char),_get_zero_arr_size(_tmp515,11));});_LL381: if((int)
_tmp512 != 3)goto _LL37A;_LL382: return({const char*_tmp516="other coercion";
_tag_arr(_tmp516,sizeof(char),_get_zero_arr_size(_tmp516,15));});_LL37A:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(
t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(
t1)){if(Cyc_Tcutil_will_lose_precision(t1,t2))({struct Cyc_String_pa_struct
_tmp51A;_tmp51A.tag=0;_tmp51A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp519;_tmp519.tag=0;_tmp519.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp517[2]={&
_tmp519,& _tmp51A};Cyc_Tcutil_warn(e->loc,({const char*_tmp518="integral size mismatch; %s -> %s conversion supplied";
_tag_arr(_tmp518,sizeof(char),_get_zero_arr_size(_tmp518,53));}),_tag_arr(
_tmp517,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)1);
return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,(void*)3);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;
else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(
te,e,t2,c);if(c != (void*)2)({struct Cyc_String_pa_struct _tmp51E;_tmp51E.tag=0;
_tmp51E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp51D;_tmp51D.tag=0;_tmp51D.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp51B[2]={& _tmp51D,&
_tmp51E};Cyc_Tcutil_warn(e->loc,({const char*_tmp51C="implicit cast from %s to %s";
_tag_arr(_tmp51C,sizeof(char),_get_zero_arr_size(_tmp51C,28));}),_tag_arr(
_tmp51B,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp51F=Cyc_Tcutil_compress(t);
_LL384: if(_tmp51F <= (void*)4  || *((int*)_tmp51F)!= 5)goto _LL386;_LL385: goto
_LL387;_LL386: if((int)_tmp51F != 1)goto _LL388;_LL387: goto _LL389;_LL388: if(_tmp51F
<= (void*)4  || *((int*)_tmp51F)!= 6)goto _LL38A;_LL389: return 1;_LL38A:;_LL38B:
return 0;_LL383:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*
inst,struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*_tmp520=_cycalloc(
sizeof(*_tmp520));_tmp520->f1=x->tq;_tmp520->f2=Cyc_Tcutil_substitute(inst,(void*)
x->type);_tmp520;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*
te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp521=t1;struct Cyc_List_List*
_tmp522;struct Cyc_Absyn_AggrInfo _tmp523;void*_tmp524;struct Cyc_Absyn_Aggrdecl**
_tmp525;struct Cyc_Absyn_Aggrdecl*_tmp526;struct Cyc_List_List*_tmp527;void*
_tmp528;struct Cyc_List_List*_tmp529;struct Cyc_Absyn_FnInfo _tmp52A;_LL38D: if((int)
_tmp521 != 0)goto _LL38F;_LL38E: return 0;_LL38F: if(_tmp521 <= (void*)4  || *((int*)
_tmp521)!= 9)goto _LL391;_tmp522=((struct Cyc_Absyn_TupleType_struct*)_tmp521)->f1;
_LL390: return _tmp522;_LL391: if(_tmp521 <= (void*)4  || *((int*)_tmp521)!= 10)goto
_LL393;_tmp523=((struct Cyc_Absyn_AggrType_struct*)_tmp521)->f1;_tmp524=(void*)
_tmp523.aggr_info;if(*((int*)_tmp524)!= 1)goto _LL393;_tmp525=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp524)->f1;_tmp526=*_tmp525;_tmp527=_tmp523.targs;_LL392: if((((void*)_tmp526->kind
== (void*)1  || _tmp526->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp526->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp526->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp52B=_cycalloc(
sizeof(*_tmp52B));_tmp52B->hd=({struct _tuple4*_tmp52C=_cycalloc(sizeof(*_tmp52C));
_tmp52C->f1=Cyc_Absyn_empty_tqual(0);_tmp52C->f2=t1;_tmp52C;});_tmp52B->tl=0;
_tmp52B;});{struct Cyc_List_List*_tmp52D=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp526->tvs,_tmp527);return((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp52D,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp526->impl))->fields);}_LL393: if(
_tmp521 <= (void*)4  || *((int*)_tmp521)!= 11)goto _LL395;_tmp528=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp521)->f1;if((int)_tmp528 != 0)goto _LL395;
_tmp529=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp521)->f2;_LL394: return((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,
0,_tmp529);_LL395: if(_tmp521 <= (void*)4  || *((int*)_tmp521)!= 8)goto _LL397;
_tmp52A=((struct Cyc_Absyn_FnType_struct*)_tmp521)->f1;_LL396: return({struct Cyc_List_List*
_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->hd=({struct _tuple4*_tmp52F=
_cycalloc(sizeof(*_tmp52F));_tmp52F->f1=Cyc_Absyn_const_tqual(0);_tmp52F->f2=t1;
_tmp52F;});_tmp52E->tl=0;_tmp52E;});_LL397:;_LL398: return({struct Cyc_List_List*
_tmp530=_cycalloc(sizeof(*_tmp530));_tmp530->hd=({struct _tuple4*_tmp531=
_cycalloc(sizeof(*_tmp531));_tmp531->f1=Cyc_Absyn_empty_tqual(0);_tmp531->f2=t1;
_tmp531;});_tmp530->tl=0;_tmp530;});_LL38C:;}}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp532=(void*)t->hd;_LL39A: if((int)_tmp532 != 16)goto _LL39C;
_LL39B: goto _LL39D;_LL39C: if((int)_tmp532 != 3)goto _LL39E;_LL39D: goto _LL39F;_LL39E:
if(_tmp532 <= (void*)17  || *((int*)_tmp532)!= 4)goto _LL3A0;_LL39F: continue;_LL3A0:;
_LL3A1: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;
_LL399:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp534=({struct _tuple6 _tmp533;_tmp533.f1=t1;_tmp533.f2=t2;_tmp533;});void*
_tmp535;struct Cyc_Absyn_PtrInfo _tmp536;void*_tmp537;struct Cyc_Absyn_Tqual _tmp538;
struct Cyc_Absyn_PtrAtts _tmp539;void*_tmp53A;struct Cyc_Absyn_Conref*_tmp53B;
struct Cyc_Absyn_Conref*_tmp53C;struct Cyc_Absyn_Conref*_tmp53D;void*_tmp53E;
struct Cyc_Absyn_PtrInfo _tmp53F;void*_tmp540;struct Cyc_Absyn_Tqual _tmp541;struct
Cyc_Absyn_PtrAtts _tmp542;void*_tmp543;struct Cyc_Absyn_Conref*_tmp544;struct Cyc_Absyn_Conref*
_tmp545;struct Cyc_Absyn_Conref*_tmp546;void*_tmp547;struct Cyc_Absyn_FnInfo
_tmp548;void*_tmp549;struct Cyc_Absyn_FnInfo _tmp54A;_LL3A3: _tmp535=_tmp534.f1;if(
_tmp535 <= (void*)4  || *((int*)_tmp535)!= 4)goto _LL3A5;_tmp536=((struct Cyc_Absyn_PointerType_struct*)
_tmp535)->f1;_tmp537=(void*)_tmp536.elt_typ;_tmp538=_tmp536.elt_tq;_tmp539=
_tmp536.ptr_atts;_tmp53A=(void*)_tmp539.rgn;_tmp53B=_tmp539.nullable;_tmp53C=
_tmp539.bounds;_tmp53D=_tmp539.zero_term;_tmp53E=_tmp534.f2;if(_tmp53E <= (void*)
4  || *((int*)_tmp53E)!= 4)goto _LL3A5;_tmp53F=((struct Cyc_Absyn_PointerType_struct*)
_tmp53E)->f1;_tmp540=(void*)_tmp53F.elt_typ;_tmp541=_tmp53F.elt_tq;_tmp542=
_tmp53F.ptr_atts;_tmp543=(void*)_tmp542.rgn;_tmp544=_tmp542.nullable;_tmp545=
_tmp542.bounds;_tmp546=_tmp542.zero_term;_LL3A4: if(_tmp538.real_const  && !
_tmp541.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp53B,
_tmp544) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp53B)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp544))return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp53D,
_tmp546) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp53D)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp546))return 0;if(!Cyc_Tcutil_unify(_tmp53A,_tmp543) && !Cyc_Tcenv_region_outlives(
te,_tmp53A,_tmp543))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp53C,_tmp545)){struct _tuple6 _tmp54C=({struct _tuple6 _tmp54B;_tmp54B.f1=Cyc_Absyn_conref_val(
_tmp53C);_tmp54B.f2=Cyc_Absyn_conref_val(_tmp545);_tmp54B;});void*_tmp54D;void*
_tmp54E;void*_tmp54F;struct Cyc_Absyn_Exp*_tmp550;void*_tmp551;struct Cyc_Absyn_Exp*
_tmp552;_LL3AA: _tmp54D=_tmp54C.f1;if(_tmp54D <= (void*)1  || *((int*)_tmp54D)!= 0)
goto _LL3AC;_tmp54E=_tmp54C.f2;if((int)_tmp54E != 0)goto _LL3AC;_LL3AB: goto _LL3A9;
_LL3AC: _tmp54F=_tmp54C.f1;if(_tmp54F <= (void*)1  || *((int*)_tmp54F)!= 0)goto
_LL3AE;_tmp550=((struct Cyc_Absyn_Upper_b_struct*)_tmp54F)->f1;_tmp551=_tmp54C.f2;
if(_tmp551 <= (void*)1  || *((int*)_tmp551)!= 0)goto _LL3AE;_tmp552=((struct Cyc_Absyn_Upper_b_struct*)
_tmp551)->f1;_LL3AD: if(!Cyc_Evexp_lte_const_exp(_tmp552,_tmp550))return 0;goto
_LL3A9;_LL3AE:;_LL3AF: return 0;_LL3A9:;}return Cyc_Tcutil_ptrsubtype(te,({struct
Cyc_List_List*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->hd=({struct _tuple6*
_tmp554=_cycalloc(sizeof(*_tmp554));_tmp554->f1=t1;_tmp554->f2=t2;_tmp554;});
_tmp553->tl=assume;_tmp553;}),_tmp537,_tmp540);_LL3A5: _tmp547=_tmp534.f1;if(
_tmp547 <= (void*)4  || *((int*)_tmp547)!= 8)goto _LL3A7;_tmp548=((struct Cyc_Absyn_FnType_struct*)
_tmp547)->f1;_tmp549=_tmp534.f2;if(_tmp549 <= (void*)4  || *((int*)_tmp549)!= 8)
goto _LL3A7;_tmp54A=((struct Cyc_Absyn_FnType_struct*)_tmp549)->f1;_LL3A6: if(
_tmp548.tvars != 0  || _tmp54A.tvars != 0){struct Cyc_List_List*_tmp555=_tmp548.tvars;
struct Cyc_List_List*_tmp556=_tmp54A.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp555)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp556))return 0;{
struct _RegionHandle _tmp557=_new_region("r");struct _RegionHandle*r=& _tmp557;
_push_region(r);{struct Cyc_List_List*inst=0;while(_tmp555 != 0){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp555->hd)!= Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp556))->hd)){int _tmp558=0;_npop_handler(0);
return _tmp558;}inst=({struct Cyc_List_List*_tmp559=_region_malloc(r,sizeof(*
_tmp559));_tmp559->hd=({struct _tuple8*_tmp55A=_region_malloc(r,sizeof(*_tmp55A));
_tmp55A->f1=(struct Cyc_Absyn_Tvar*)_tmp556->hd;_tmp55A->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B[0]=({struct Cyc_Absyn_VarType_struct
_tmp55C;_tmp55C.tag=1;_tmp55C.f1=(struct Cyc_Absyn_Tvar*)_tmp555->hd;_tmp55C;});
_tmp55B;});_tmp55A;});_tmp559->tl=inst;_tmp559;});_tmp555=_tmp555->tl;_tmp556=
_tmp556->tl;}if(inst != 0){_tmp548.tvars=0;_tmp54A.tvars=0;{int _tmp561=Cyc_Tcutil_subtype(
te,assume,(void*)({struct Cyc_Absyn_FnType_struct*_tmp55D=_cycalloc(sizeof(*
_tmp55D));_tmp55D[0]=({struct Cyc_Absyn_FnType_struct _tmp55E;_tmp55E.tag=8;
_tmp55E.f1=_tmp548;_tmp55E;});_tmp55D;}),(void*)({struct Cyc_Absyn_FnType_struct*
_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F[0]=({struct Cyc_Absyn_FnType_struct
_tmp560;_tmp560.tag=8;_tmp560.f1=_tmp54A;_tmp560;});_tmp55F;}));_npop_handler(0);
return _tmp561;}}};_pop_region(r);}}if(!Cyc_Tcutil_subtype(te,assume,(void*)
_tmp548.ret_typ,(void*)_tmp54A.ret_typ))return 0;{struct Cyc_List_List*_tmp562=
_tmp548.args;struct Cyc_List_List*_tmp563=_tmp54A.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp562)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp563))return 0;for(0;_tmp562 != 0;(_tmp562=_tmp562->tl,_tmp563=_tmp563->tl)){
struct Cyc_Absyn_Tqual _tmp565;void*_tmp566;struct _tuple2 _tmp564=*((struct _tuple2*)
_tmp562->hd);_tmp565=_tmp564.f2;_tmp566=_tmp564.f3;{struct Cyc_Absyn_Tqual _tmp568;
void*_tmp569;struct _tuple2 _tmp567=*((struct _tuple2*)((struct Cyc_List_List*)
_check_null(_tmp563))->hd);_tmp568=_tmp567.f2;_tmp569=_tmp567.f3;if(_tmp568.real_const
 && !_tmp565.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp569,_tmp566))return
0;}}if(_tmp548.c_varargs != _tmp54A.c_varargs)return 0;if(_tmp548.cyc_varargs != 0
 && _tmp54A.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp56A=*_tmp548.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp56B=*_tmp54A.cyc_varargs;if((_tmp56B.tq).real_const
 && !(_tmp56A.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp56B.type,(
void*)_tmp56A.type))return 0;}else{if(_tmp548.cyc_varargs != 0  || _tmp54A.cyc_varargs
!= 0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp548.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp54A.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp548.rgn_po,_tmp54A.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp548.attributes,_tmp54A.attributes))return 0;return 1;}_LL3A7:;_LL3A8: return 0;
_LL3A2:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp56D=({
struct _tuple6 _tmp56C;_tmp56C.f1=Cyc_Tcutil_compress(t1);_tmp56C.f2=Cyc_Tcutil_compress(
t2);_tmp56C;});void*_tmp56E;void*_tmp56F;void*_tmp570;void*_tmp571;_LL3B1:
_tmp56E=_tmp56D.f1;if(_tmp56E <= (void*)4  || *((int*)_tmp56E)!= 5)goto _LL3B3;
_tmp56F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp56E)->f2;_tmp570=_tmp56D.f2;
if(_tmp570 <= (void*)4  || *((int*)_tmp570)!= 5)goto _LL3B3;_tmp571=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp570)->f2;_LL3B2: return _tmp56F == _tmp571;_LL3B3:;
_LL3B4: return 0;_LL3B0:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple4 _tmp573;struct
Cyc_Absyn_Tqual _tmp574;void*_tmp575;struct _tuple4*_tmp572=(struct _tuple4*)tqs1->hd;
_tmp573=*_tmp572;_tmp574=_tmp573.f1;_tmp575=_tmp573.f2;{struct _tuple4 _tmp577;
struct Cyc_Absyn_Tqual _tmp578;void*_tmp579;struct _tuple4*_tmp576=(struct _tuple4*)
tqs2->hd;_tmp577=*_tmp576;_tmp578=_tmp577.f1;_tmp579=_tmp577.f2;if(_tmp578.real_const
 && Cyc_Tcutil_subtype(te,assume,_tmp575,_tmp579))continue;else{if(Cyc_Tcutil_unify(
_tmp575,_tmp579))continue;else{if(Cyc_Tcutil_isomorphic(_tmp575,_tmp579))
continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp57A=Cyc_Tcutil_compress(t);void*_tmp57B;_LL3B6: if(_tmp57A <= (void*)4
 || *((int*)_tmp57A)!= 5)goto _LL3B8;_tmp57B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp57A)->f2;if((int)_tmp57B != 0)goto _LL3B8;_LL3B7: return 1;_LL3B8:;_LL3B9: return
0;_LL3B5:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp57C=t2;
void*_tmp57D;_LL3BB: if(_tmp57C <= (void*)4  || *((int*)_tmp57C)!= 5)goto _LL3BD;
_tmp57D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp57C)->f2;if((int)_tmp57D != 
2)goto _LL3BD;_LL3BC: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)1;goto
_LL3BA;_LL3BD:;_LL3BE: goto _LL3BA;_LL3BA:;}{void*_tmp57E=t1;struct Cyc_Absyn_PtrInfo
_tmp57F;void*_tmp580;struct Cyc_Absyn_Tqual _tmp581;struct Cyc_Absyn_PtrAtts _tmp582;
void*_tmp583;struct Cyc_Absyn_Conref*_tmp584;struct Cyc_Absyn_Conref*_tmp585;
struct Cyc_Absyn_Conref*_tmp586;struct Cyc_Absyn_ArrayInfo _tmp587;void*_tmp588;
struct Cyc_Absyn_Tqual _tmp589;struct Cyc_Absyn_Exp*_tmp58A;struct Cyc_Absyn_Conref*
_tmp58B;struct Cyc_Absyn_Enumdecl*_tmp58C;_LL3C0: if(_tmp57E <= (void*)4  || *((int*)
_tmp57E)!= 4)goto _LL3C2;_tmp57F=((struct Cyc_Absyn_PointerType_struct*)_tmp57E)->f1;
_tmp580=(void*)_tmp57F.elt_typ;_tmp581=_tmp57F.elt_tq;_tmp582=_tmp57F.ptr_atts;
_tmp583=(void*)_tmp582.rgn;_tmp584=_tmp582.nullable;_tmp585=_tmp582.bounds;
_tmp586=_tmp582.zero_term;_LL3C1:{void*_tmp58D=t2;struct Cyc_Absyn_PtrInfo _tmp58E;
void*_tmp58F;struct Cyc_Absyn_Tqual _tmp590;struct Cyc_Absyn_PtrAtts _tmp591;void*
_tmp592;struct Cyc_Absyn_Conref*_tmp593;struct Cyc_Absyn_Conref*_tmp594;struct Cyc_Absyn_Conref*
_tmp595;_LL3CF: if(_tmp58D <= (void*)4  || *((int*)_tmp58D)!= 4)goto _LL3D1;_tmp58E=((
struct Cyc_Absyn_PointerType_struct*)_tmp58D)->f1;_tmp58F=(void*)_tmp58E.elt_typ;
_tmp590=_tmp58E.elt_tq;_tmp591=_tmp58E.ptr_atts;_tmp592=(void*)_tmp591.rgn;
_tmp593=_tmp591.nullable;_tmp594=_tmp591.bounds;_tmp595=_tmp591.zero_term;_LL3D0: {
void*coercion=(void*)3;struct Cyc_List_List*_tmp596=({struct Cyc_List_List*_tmp5A5=
_cycalloc(sizeof(*_tmp5A5));_tmp5A5->hd=({struct _tuple6*_tmp5A6=_cycalloc(
sizeof(*_tmp5A6));_tmp5A6->f1=t1;_tmp5A6->f2=t2;_tmp5A6;});_tmp5A5->tl=0;_tmp5A5;});
int _tmp597=Cyc_Tcutil_ptrsubtype(te,_tmp596,_tmp580,_tmp58F) && (!_tmp581.real_const
 || _tmp590.real_const);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int
zeroterm_ok=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp586,_tmp595) || !((int(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp595);int _tmp598=_tmp597?0:((
Cyc_Tcutil_bits_only(_tmp580) && Cyc_Tcutil_is_char_type(_tmp58F)) && !((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp595)) && (_tmp590.real_const
 || !_tmp581.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp585,_tmp594);if(!bounds_ok  && !_tmp598){struct _tuple6 _tmp59A=({struct _tuple6
_tmp599;_tmp599.f1=Cyc_Absyn_conref_val(_tmp585);_tmp599.f2=Cyc_Absyn_conref_val(
_tmp594);_tmp599;});void*_tmp59B;struct Cyc_Absyn_Exp*_tmp59C;void*_tmp59D;struct
Cyc_Absyn_Exp*_tmp59E;void*_tmp59F;void*_tmp5A0;void*_tmp5A1;void*_tmp5A2;void*
_tmp5A3;void*_tmp5A4;_LL3D4: _tmp59B=_tmp59A.f1;if(_tmp59B <= (void*)1  || *((int*)
_tmp59B)!= 0)goto _LL3D6;_tmp59C=((struct Cyc_Absyn_Upper_b_struct*)_tmp59B)->f1;
_tmp59D=_tmp59A.f2;if(_tmp59D <= (void*)1  || *((int*)_tmp59D)!= 0)goto _LL3D6;
_tmp59E=((struct Cyc_Absyn_Upper_b_struct*)_tmp59D)->f1;_LL3D5: if(Cyc_Evexp_lte_const_exp(
_tmp59E,_tmp59C))bounds_ok=1;goto _LL3D3;_LL3D6: _tmp59F=_tmp59A.f1;if(_tmp59F <= (
void*)1  || *((int*)_tmp59F)!= 1)goto _LL3D8;_tmp5A0=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp59F)->f1;_tmp5A1=_tmp59A.f2;if(_tmp5A1 <= (void*)1  || *((int*)_tmp5A1)!= 1)
goto _LL3D8;_tmp5A2=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp5A1)->f1;
_LL3D7: bounds_ok=Cyc_Tcutil_unify(_tmp5A0,_tmp5A2);goto _LL3D3;_LL3D8: _tmp5A3=
_tmp59A.f1;if(_tmp5A3 <= (void*)1  || *((int*)_tmp5A3)!= 1)goto _LL3DA;_LL3D9: goto
_LL3DB;_LL3DA: _tmp5A4=_tmp59A.f2;if(_tmp5A4 <= (void*)1  || *((int*)_tmp5A4)!= 1)
goto _LL3DC;_LL3DB: bounds_ok=0;goto _LL3D3;_LL3DC:;_LL3DD: bounds_ok=1;goto _LL3D3;
_LL3D3:;}if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp584) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp593))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok) && (_tmp597  || _tmp598))
 && (Cyc_Tcutil_unify(_tmp583,_tmp592) || Cyc_Tcenv_region_outlives(te,_tmp583,
_tmp592)))return coercion;else{return(void*)0;}}}_LL3D1:;_LL3D2: goto _LL3CE;_LL3CE:;}
return(void*)0;_LL3C2: if(_tmp57E <= (void*)4  || *((int*)_tmp57E)!= 7)goto _LL3C4;
_tmp587=((struct Cyc_Absyn_ArrayType_struct*)_tmp57E)->f1;_tmp588=(void*)_tmp587.elt_type;
_tmp589=_tmp587.tq;_tmp58A=_tmp587.num_elts;_tmp58B=_tmp587.zero_term;_LL3C3:{
void*_tmp5A7=t2;struct Cyc_Absyn_ArrayInfo _tmp5A8;void*_tmp5A9;struct Cyc_Absyn_Tqual
_tmp5AA;struct Cyc_Absyn_Exp*_tmp5AB;struct Cyc_Absyn_Conref*_tmp5AC;_LL3DF: if(
_tmp5A7 <= (void*)4  || *((int*)_tmp5A7)!= 7)goto _LL3E1;_tmp5A8=((struct Cyc_Absyn_ArrayType_struct*)
_tmp5A7)->f1;_tmp5A9=(void*)_tmp5A8.elt_type;_tmp5AA=_tmp5A8.tq;_tmp5AB=_tmp5A8.num_elts;
_tmp5AC=_tmp5A8.zero_term;_LL3E0: {int okay;okay=((_tmp58A != 0  && _tmp5AB != 0)
 && ((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp58B,_tmp5AC)) && Cyc_Evexp_lte_const_exp((
struct Cyc_Absyn_Exp*)_tmp5AB,(struct Cyc_Absyn_Exp*)_tmp58A);return(okay  && Cyc_Tcutil_unify(
_tmp588,_tmp5A9)) && (!_tmp589.real_const  || _tmp5AA.real_const)?(void*)3:(void*)
0;}_LL3E1:;_LL3E2: return(void*)0;_LL3DE:;}return(void*)0;_LL3C4: if(_tmp57E <= (
void*)4  || *((int*)_tmp57E)!= 12)goto _LL3C6;_tmp58C=((struct Cyc_Absyn_EnumType_struct*)
_tmp57E)->f2;_LL3C5:{void*_tmp5AD=t2;struct Cyc_Absyn_Enumdecl*_tmp5AE;_LL3E4: if(
_tmp5AD <= (void*)4  || *((int*)_tmp5AD)!= 12)goto _LL3E6;_tmp5AE=((struct Cyc_Absyn_EnumType_struct*)
_tmp5AD)->f2;_LL3E5: if((_tmp58C->fields != 0  && _tmp5AE->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp58C->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5AE->fields))->v))
return(void*)1;goto _LL3E3;_LL3E6:;_LL3E7: goto _LL3E3;_LL3E3:;}goto _LL3C7;_LL3C6:
if(_tmp57E <= (void*)4  || *((int*)_tmp57E)!= 5)goto _LL3C8;_LL3C7: goto _LL3C9;
_LL3C8: if((int)_tmp57E != 1)goto _LL3CA;_LL3C9: goto _LL3CB;_LL3CA: if(_tmp57E <= (
void*)4  || *((int*)_tmp57E)!= 6)goto _LL3CC;_LL3CB: return Cyc_Tcutil_coerceable(t2)?(
void*)1:(void*)0;_LL3CC:;_LL3CD: return(void*)0;_LL3BF:;}}void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*
_tmp5AF=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*
_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0[0]=({struct Cyc_Absyn_Cast_e_struct
_tmp5B1;_tmp5B1.tag=15;_tmp5B1.f1=(void*)t;_tmp5B1.f2=_tmp5AF;_tmp5B1.f3=0;
_tmp5B1.f4=(void*)c;_tmp5B1;});_tmp5B0;})));e->topt=({struct Cyc_Core_Opt*_tmp5B2=
_cycalloc(sizeof(*_tmp5B2));_tmp5B2->v=(void*)t;_tmp5B2;});}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmp5B3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL3E9: if(_tmp5B3 <= (void*)4  || *((int*)_tmp5B3)!= 5)
goto _LL3EB;_LL3EA: goto _LL3EC;_LL3EB: if(_tmp5B3 <= (void*)4  || *((int*)_tmp5B3)!= 
12)goto _LL3ED;_LL3EC: goto _LL3EE;_LL3ED: if(_tmp5B3 <= (void*)4  || *((int*)_tmp5B3)
!= 13)goto _LL3EF;_LL3EE: goto _LL3F0;_LL3EF: if(_tmp5B3 <= (void*)4  || *((int*)
_tmp5B3)!= 17)goto _LL3F1;_LL3F0: goto _LL3F2;_LL3F1: if(_tmp5B3 <= (void*)4  || *((
int*)_tmp5B3)!= 14)goto _LL3F3;_LL3F2: return 1;_LL3F3: if(_tmp5B3 <= (void*)4  || *((
int*)_tmp5B3)!= 0)goto _LL3F5;_LL3F4: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL3F5:;_LL3F6: return 0;_LL3E8:;}int
Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return
1;{void*_tmp5B4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL3F8: if((int)_tmp5B4 != 1)goto _LL3FA;_LL3F9: goto _LL3FB;_LL3FA: if(_tmp5B4 <= (
void*)4  || *((int*)_tmp5B4)!= 6)goto _LL3FC;_LL3FB: return 1;_LL3FC:;_LL3FD: return 0;
_LL3F7:;}}int Cyc_Tcutil_is_function_type(void*t){void*_tmp5B5=Cyc_Tcutil_compress(
t);_LL3FF: if(_tmp5B5 <= (void*)4  || *((int*)_tmp5B5)!= 8)goto _LL401;_LL400: return
1;_LL401:;_LL402: return 0;_LL3FE:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,
void*t2){struct _tuple6 _tmp5B7=({struct _tuple6 _tmp5B6;_tmp5B6.f1=t1;_tmp5B6.f2=t2;
_tmp5B6;});void*_tmp5B8;int _tmp5B9;void*_tmp5BA;int _tmp5BB;void*_tmp5BC;void*
_tmp5BD;void*_tmp5BE;void*_tmp5BF;void*_tmp5C0;void*_tmp5C1;void*_tmp5C2;void*
_tmp5C3;void*_tmp5C4;void*_tmp5C5;void*_tmp5C6;void*_tmp5C7;void*_tmp5C8;void*
_tmp5C9;void*_tmp5CA;void*_tmp5CB;void*_tmp5CC;void*_tmp5CD;void*_tmp5CE;void*
_tmp5CF;void*_tmp5D0;void*_tmp5D1;void*_tmp5D2;void*_tmp5D3;void*_tmp5D4;void*
_tmp5D5;void*_tmp5D6;void*_tmp5D7;_LL404: _tmp5B8=_tmp5B7.f1;if(_tmp5B8 <= (void*)
4  || *((int*)_tmp5B8)!= 6)goto _LL406;_tmp5B9=((struct Cyc_Absyn_DoubleType_struct*)
_tmp5B8)->f1;_tmp5BA=_tmp5B7.f2;if(_tmp5BA <= (void*)4  || *((int*)_tmp5BA)!= 6)
goto _LL406;_tmp5BB=((struct Cyc_Absyn_DoubleType_struct*)_tmp5BA)->f1;_LL405: if(
_tmp5B9)return t1;else{return t2;}_LL406: _tmp5BC=_tmp5B7.f1;if(_tmp5BC <= (void*)4
 || *((int*)_tmp5BC)!= 6)goto _LL408;_LL407: return t1;_LL408: _tmp5BD=_tmp5B7.f2;
if(_tmp5BD <= (void*)4  || *((int*)_tmp5BD)!= 6)goto _LL40A;_LL409: return t2;_LL40A:
_tmp5BE=_tmp5B7.f1;if((int)_tmp5BE != 1)goto _LL40C;_LL40B: goto _LL40D;_LL40C:
_tmp5BF=_tmp5B7.f2;if((int)_tmp5BF != 1)goto _LL40E;_LL40D: return(void*)1;_LL40E:
_tmp5C0=_tmp5B7.f1;if(_tmp5C0 <= (void*)4  || *((int*)_tmp5C0)!= 5)goto _LL410;
_tmp5C1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C0)->f1;if((int)_tmp5C1 != 
1)goto _LL410;_tmp5C2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C0)->f2;if((
int)_tmp5C2 != 3)goto _LL410;_LL40F: goto _LL411;_LL410: _tmp5C3=_tmp5B7.f2;if(
_tmp5C3 <= (void*)4  || *((int*)_tmp5C3)!= 5)goto _LL412;_tmp5C4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C3)->f1;if((int)_tmp5C4 != 1)goto _LL412;_tmp5C5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C3)->f2;if((int)_tmp5C5 != 3)goto _LL412;_LL411: return Cyc_Absyn_ulonglong_typ;
_LL412: _tmp5C6=_tmp5B7.f1;if(_tmp5C6 <= (void*)4  || *((int*)_tmp5C6)!= 5)goto
_LL414;_tmp5C7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C6)->f1;if((int)
_tmp5C7 != 2)goto _LL414;_tmp5C8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C6)->f2;
if((int)_tmp5C8 != 3)goto _LL414;_LL413: goto _LL415;_LL414: _tmp5C9=_tmp5B7.f2;if(
_tmp5C9 <= (void*)4  || *((int*)_tmp5C9)!= 5)goto _LL416;_tmp5CA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C9)->f1;if((int)_tmp5CA != 2)goto _LL416;_tmp5CB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C9)->f2;if((int)_tmp5CB != 3)goto _LL416;_LL415: goto _LL417;_LL416: _tmp5CC=
_tmp5B7.f1;if(_tmp5CC <= (void*)4  || *((int*)_tmp5CC)!= 5)goto _LL418;_tmp5CD=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp5CC)->f1;if((int)_tmp5CD != 0)goto
_LL418;_tmp5CE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5CC)->f2;if((int)
_tmp5CE != 3)goto _LL418;_LL417: goto _LL419;_LL418: _tmp5CF=_tmp5B7.f2;if(_tmp5CF <= (
void*)4  || *((int*)_tmp5CF)!= 5)goto _LL41A;_tmp5D0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5CF)->f1;if((int)_tmp5D0 != 0)goto _LL41A;_tmp5D1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5CF)->f2;if((int)_tmp5D1 != 3)goto _LL41A;_LL419: return Cyc_Absyn_slonglong_typ;
_LL41A: _tmp5D2=_tmp5B7.f1;if(_tmp5D2 <= (void*)4  || *((int*)_tmp5D2)!= 5)goto
_LL41C;_tmp5D3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5D2)->f1;if((int)
_tmp5D3 != 1)goto _LL41C;_tmp5D4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5D2)->f2;
if((int)_tmp5D4 != 2)goto _LL41C;_LL41B: goto _LL41D;_LL41C: _tmp5D5=_tmp5B7.f2;if(
_tmp5D5 <= (void*)4  || *((int*)_tmp5D5)!= 5)goto _LL41E;_tmp5D6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5D5)->f1;if((int)_tmp5D6 != 1)goto _LL41E;_tmp5D7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5D5)->f2;if((int)_tmp5D7 != 2)goto _LL41E;_LL41D: return Cyc_Absyn_uint_typ;
_LL41E:;_LL41F: return Cyc_Absyn_sint_typ;_LL403:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp5D8=(void*)e->r;struct Cyc_Core_Opt*_tmp5D9;
_LL421: if(*((int*)_tmp5D8)!= 4)goto _LL423;_tmp5D9=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp5D8)->f2;if(_tmp5D9 != 0)goto _LL423;_LL422:({void*_tmp5DA[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp5DB="assignment in test";_tag_arr(_tmp5DB,sizeof(char),
_get_zero_arr_size(_tmp5DB,19));}),_tag_arr(_tmp5DA,sizeof(void*),0));});goto
_LL420;_LL423:;_LL424: goto _LL420;_LL420:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp5DD=({struct _tuple6 _tmp5DC;_tmp5DC.f1=c1;_tmp5DC.f2=c2;_tmp5DC;});
void*_tmp5DE;void*_tmp5DF;void*_tmp5E0;void*_tmp5E1;void*_tmp5E2;struct Cyc_Core_Opt*
_tmp5E3;struct Cyc_Core_Opt**_tmp5E4;void*_tmp5E5;struct Cyc_Core_Opt*_tmp5E6;
struct Cyc_Core_Opt**_tmp5E7;void*_tmp5E8;struct Cyc_Core_Opt*_tmp5E9;struct Cyc_Core_Opt**
_tmp5EA;void*_tmp5EB;void*_tmp5EC;void*_tmp5ED;void*_tmp5EE;void*_tmp5EF;void*
_tmp5F0;struct Cyc_Core_Opt*_tmp5F1;struct Cyc_Core_Opt**_tmp5F2;void*_tmp5F3;void*
_tmp5F4;struct Cyc_Core_Opt*_tmp5F5;struct Cyc_Core_Opt**_tmp5F6;void*_tmp5F7;void*
_tmp5F8;struct Cyc_Core_Opt*_tmp5F9;struct Cyc_Core_Opt**_tmp5FA;void*_tmp5FB;
_LL426: _tmp5DE=_tmp5DD.f1;if(*((int*)_tmp5DE)!= 0)goto _LL428;_tmp5DF=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp5DE)->f1;_tmp5E0=_tmp5DD.f2;if(*((int*)_tmp5E0)
!= 0)goto _LL428;_tmp5E1=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5E0)->f1;
_LL427: return _tmp5DF == _tmp5E1;_LL428: _tmp5E2=_tmp5DD.f2;if(*((int*)_tmp5E2)!= 1)
goto _LL42A;_tmp5E3=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E2)->f1;_tmp5E4=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E2)->f1;_LL429:*
_tmp5E4=({struct Cyc_Core_Opt*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->v=(
void*)c1;_tmp5FC;});return 1;_LL42A: _tmp5E5=_tmp5DD.f1;if(*((int*)_tmp5E5)!= 1)
goto _LL42C;_tmp5E6=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E5)->f1;_tmp5E7=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E5)->f1;_LL42B:*
_tmp5E7=({struct Cyc_Core_Opt*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD->v=(
void*)c2;_tmp5FD;});return 1;_LL42C: _tmp5E8=_tmp5DD.f1;if(*((int*)_tmp5E8)!= 2)
goto _LL42E;_tmp5E9=((struct Cyc_Absyn_Less_kb_struct*)_tmp5E8)->f1;_tmp5EA=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp5E8)->f1;_tmp5EB=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp5E8)->f2;_tmp5EC=_tmp5DD.f2;if(*((
int*)_tmp5EC)!= 0)goto _LL42E;_tmp5ED=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp5EC)->f1;_LL42D: if(Cyc_Tcutil_kind_leq(_tmp5ED,_tmp5EB)){*_tmp5EA=({struct
Cyc_Core_Opt*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE->v=(void*)c2;_tmp5FE;});
return 1;}else{return 0;}_LL42E: _tmp5EE=_tmp5DD.f1;if(*((int*)_tmp5EE)!= 0)goto
_LL430;_tmp5EF=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5EE)->f1;_tmp5F0=
_tmp5DD.f2;if(*((int*)_tmp5F0)!= 2)goto _LL430;_tmp5F1=((struct Cyc_Absyn_Less_kb_struct*)
_tmp5F0)->f1;_tmp5F2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp5F0)->f1;_tmp5F3=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp5F0)->f2;
_LL42F: if(Cyc_Tcutil_kind_leq(_tmp5EF,_tmp5F3)){*_tmp5F2=({struct Cyc_Core_Opt*
_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF->v=(void*)c1;_tmp5FF;});return 1;}
else{return 0;}_LL430: _tmp5F4=_tmp5DD.f1;if(*((int*)_tmp5F4)!= 2)goto _LL425;
_tmp5F5=((struct Cyc_Absyn_Less_kb_struct*)_tmp5F4)->f1;_tmp5F6=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp5F4)->f1;_tmp5F7=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp5F4)->f2;_tmp5F8=_tmp5DD.f2;if(*((int*)_tmp5F8)!= 2)goto _LL425;_tmp5F9=((
struct Cyc_Absyn_Less_kb_struct*)_tmp5F8)->f1;_tmp5FA=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp5F8)->f1;_tmp5FB=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp5F8)->f2;_LL431: if(Cyc_Tcutil_kind_leq(_tmp5F7,_tmp5FB)){*_tmp5FA=({struct
Cyc_Core_Opt*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->v=(void*)c1;_tmp600;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp5FB,_tmp5F7)){*_tmp5F6=({struct Cyc_Core_Opt*
_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->v=(void*)c2;_tmp601;});return 1;}
else{return 0;}}_LL425:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp602=_cycalloc_atomic(sizeof(*_tmp602));_tmp602[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp602;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp607;_tmp607.tag=1;_tmp607.f1=(unsigned int)i;{void*
_tmp605[1]={& _tmp607};Cyc_aprintf(({const char*_tmp606="#%d";_tag_arr(_tmp606,
sizeof(char),_get_zero_arr_size(_tmp606,4));}),_tag_arr(_tmp605,sizeof(void*),1));}});
return({struct Cyc_Absyn_Tvar*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->name=({
struct _tagged_arr*_tmp604=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp604[0]=s;
_tmp604;});_tmp603->identity=0;_tmp603->kind=(void*)k;_tmp603;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _tagged_arr _tmp608=*t->name;return*((const char*)
_check_unknown_subscript(_tmp608,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp60B;_tmp60B.tag=0;
_tmp60B.f1=(struct _tagged_arr)((struct _tagged_arr)*t->name);{void*_tmp609[1]={&
_tmp60B};Cyc_printf(({const char*_tmp60A="%s";_tag_arr(_tmp60A,sizeof(char),
_get_zero_arr_size(_tmp60A,3));}),_tag_arr(_tmp609,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(
t))return;{struct _tagged_arr _tmp60C=Cyc_strconcat(({const char*_tmp611="`";
_tag_arr(_tmp611,sizeof(char),_get_zero_arr_size(_tmp611,2));}),(struct
_tagged_arr)*t->name);({struct _tagged_arr _tmp60D=_tagged_arr_plus(_tmp60C,
sizeof(char),1);char _tmp60E=*((char*)_check_unknown_subscript(_tmp60D,sizeof(
char),0));char _tmp60F='t';if(_get_arr_size(_tmp60D,sizeof(char))== 1  && (_tmp60E
== '\000'  && _tmp60F != '\000'))_throw_arraybounds();*((char*)_tmp60D.curr)=
_tmp60F;});t->name=({struct _tagged_arr*_tmp610=_cycalloc(sizeof(struct
_tagged_arr)* 1);_tmp610[0]=(struct _tagged_arr)_tmp60C;_tmp610;});}}struct
_tuple13{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct
_tuple2*Cyc_Tcutil_fndecl2typ_f(struct _tuple13*x){return({struct _tuple2*_tmp612=
_cycalloc(sizeof(*_tmp612));_tmp612->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*
_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613->v=(*x).f1;_tmp613;});_tmp612->f2=(*
x).f2;_tmp612->f3=(*x).f3;_tmp612;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*
fd){if(fd->cached_typ == 0){struct Cyc_List_List*_tmp614=0;{struct Cyc_List_List*
atts=fd->attributes;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)
atts->hd))_tmp614=({struct Cyc_List_List*_tmp615=_cycalloc(sizeof(*_tmp615));
_tmp615->hd=(void*)((void*)atts->hd);_tmp615->tl=_tmp614;_tmp615;});}}return(
void*)({struct Cyc_Absyn_FnType_struct*_tmp616=_cycalloc(sizeof(*_tmp616));
_tmp616[0]=({struct Cyc_Absyn_FnType_struct _tmp617;_tmp617.tag=8;_tmp617.f1=({
struct Cyc_Absyn_FnInfo _tmp618;_tmp618.tvars=fd->tvs;_tmp618.effect=fd->effect;
_tmp618.ret_typ=(void*)((void*)fd->ret_type);_tmp618.args=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args);_tmp618.c_varargs=fd->c_varargs;_tmp618.cyc_varargs=fd->cyc_varargs;
_tmp618.rgn_po=fd->rgn_po;_tmp618.attributes=_tmp614;_tmp618;});_tmp617;});
_tmp616;});}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple14{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple14*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*
t).f2;}static struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({
struct _tuple4*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->f1=(*pr).f1;_tmp619->f2=
t;_tmp619;});}struct _tuple15{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};
struct _tuple16{struct _tuple15*f1;void*f2;};static struct _tuple16*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple2*y){return({struct _tuple16*_tmp61A=
_region_malloc(rgn,sizeof(*_tmp61A));_tmp61A->f1=({struct _tuple15*_tmp61B=
_region_malloc(rgn,sizeof(*_tmp61B));_tmp61B->f1=(*y).f1;_tmp61B->f2=(*y).f2;
_tmp61B;});_tmp61A->f2=(*y).f3;_tmp61A;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(
struct _tuple16*w){struct _tuple15*_tmp61D;void*_tmp61E;struct _tuple16 _tmp61C=*w;
_tmp61D=_tmp61C.f1;_tmp61E=_tmp61C.f2;{struct Cyc_Core_Opt*_tmp620;struct Cyc_Absyn_Tqual
_tmp621;struct _tuple15 _tmp61F=*_tmp61D;_tmp620=_tmp61F.f1;_tmp621=_tmp61F.f2;
return({struct _tuple2*_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622->f1=_tmp620;
_tmp622->f2=_tmp621;_tmp622->f3=_tmp61E;_tmp622;});}}static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*
_tmp623=_cycalloc(sizeof(*_tmp623));_tmp623->name=f->name;_tmp623->tq=f->tq;
_tmp623->type=(void*)t;_tmp623->width=f->width;_tmp623->attributes=f->attributes;
_tmp623;});}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){void*_tmp624=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp625;struct Cyc_Absyn_AggrInfo _tmp626;void*_tmp627;
struct Cyc_List_List*_tmp628;struct Cyc_Absyn_TunionInfo _tmp629;void*_tmp62A;
struct Cyc_List_List*_tmp62B;void*_tmp62C;struct Cyc_Absyn_TunionFieldInfo _tmp62D;
void*_tmp62E;struct Cyc_List_List*_tmp62F;struct _tuple1*_tmp630;struct Cyc_List_List*
_tmp631;struct Cyc_Absyn_Typedefdecl*_tmp632;void**_tmp633;struct Cyc_Absyn_ArrayInfo
_tmp634;void*_tmp635;struct Cyc_Absyn_Tqual _tmp636;struct Cyc_Absyn_Exp*_tmp637;
struct Cyc_Absyn_Conref*_tmp638;struct Cyc_Position_Segment*_tmp639;struct Cyc_Absyn_PtrInfo
_tmp63A;void*_tmp63B;struct Cyc_Absyn_Tqual _tmp63C;struct Cyc_Absyn_PtrAtts _tmp63D;
void*_tmp63E;struct Cyc_Absyn_Conref*_tmp63F;struct Cyc_Absyn_Conref*_tmp640;
struct Cyc_Absyn_Conref*_tmp641;struct Cyc_Absyn_FnInfo _tmp642;struct Cyc_List_List*
_tmp643;struct Cyc_Core_Opt*_tmp644;void*_tmp645;struct Cyc_List_List*_tmp646;int
_tmp647;struct Cyc_Absyn_VarargInfo*_tmp648;struct Cyc_List_List*_tmp649;struct Cyc_List_List*
_tmp64A;struct Cyc_List_List*_tmp64B;void*_tmp64C;struct Cyc_List_List*_tmp64D;
struct Cyc_Core_Opt*_tmp64E;void*_tmp64F;void*_tmp650;void*_tmp651;void*_tmp652;
void*_tmp653;struct Cyc_List_List*_tmp654;_LL433: if(_tmp624 <= (void*)4  || *((int*)
_tmp624)!= 1)goto _LL435;_tmp625=((struct Cyc_Absyn_VarType_struct*)_tmp624)->f1;
_LL434: {struct _handler_cons _tmp655;_push_handler(& _tmp655);{int _tmp657=0;if(
setjmp(_tmp655.handler))_tmp657=1;if(!_tmp657){{void*_tmp658=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp625);_npop_handler(0);return
_tmp658;};_pop_handler();}else{void*_tmp656=(void*)_exn_thrown;void*_tmp65A=
_tmp656;_LL468: if(_tmp65A != Cyc_Core_Not_found)goto _LL46A;_LL469: return t;_LL46A:;
_LL46B:(void)_throw(_tmp65A);_LL467:;}}}_LL435: if(_tmp624 <= (void*)4  || *((int*)
_tmp624)!= 10)goto _LL437;_tmp626=((struct Cyc_Absyn_AggrType_struct*)_tmp624)->f1;
_tmp627=(void*)_tmp626.aggr_info;_tmp628=_tmp626.targs;_LL436: {struct Cyc_List_List*
_tmp65B=Cyc_Tcutil_substs(rgn,inst,_tmp628);return _tmp65B == _tmp628?t:(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C[0]=({
struct Cyc_Absyn_AggrType_struct _tmp65D;_tmp65D.tag=10;_tmp65D.f1=({struct Cyc_Absyn_AggrInfo
_tmp65E;_tmp65E.aggr_info=(void*)_tmp627;_tmp65E.targs=_tmp65B;_tmp65E;});
_tmp65D;});_tmp65C;});}_LL437: if(_tmp624 <= (void*)4  || *((int*)_tmp624)!= 2)goto
_LL439;_tmp629=((struct Cyc_Absyn_TunionType_struct*)_tmp624)->f1;_tmp62A=(void*)
_tmp629.tunion_info;_tmp62B=_tmp629.targs;_tmp62C=(void*)_tmp629.rgn;_LL438: {
struct Cyc_List_List*_tmp65F=Cyc_Tcutil_substs(rgn,inst,_tmp62B);void*_tmp660=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp62C);return _tmp65F == _tmp62B  && _tmp660 == _tmp62C?t:(void*)({struct
Cyc_Absyn_TunionType_struct*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661[0]=({
struct Cyc_Absyn_TunionType_struct _tmp662;_tmp662.tag=2;_tmp662.f1=({struct Cyc_Absyn_TunionInfo
_tmp663;_tmp663.tunion_info=(void*)_tmp62A;_tmp663.targs=_tmp65F;_tmp663.rgn=(
void*)_tmp660;_tmp663;});_tmp662;});_tmp661;});}_LL439: if(_tmp624 <= (void*)4  || *((
int*)_tmp624)!= 3)goto _LL43B;_tmp62D=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp624)->f1;_tmp62E=(void*)_tmp62D.field_info;_tmp62F=_tmp62D.targs;_LL43A: {
struct Cyc_List_List*_tmp664=Cyc_Tcutil_substs(rgn,inst,_tmp62F);return _tmp664 == 
_tmp62F?t:(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp665=_cycalloc(
sizeof(*_tmp665));_tmp665[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp666;
_tmp666.tag=3;_tmp666.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp667;_tmp667.field_info=(
void*)_tmp62E;_tmp667.targs=_tmp664;_tmp667;});_tmp666;});_tmp665;});}_LL43B: if(
_tmp624 <= (void*)4  || *((int*)_tmp624)!= 16)goto _LL43D;_tmp630=((struct Cyc_Absyn_TypedefType_struct*)
_tmp624)->f1;_tmp631=((struct Cyc_Absyn_TypedefType_struct*)_tmp624)->f2;_tmp632=((
struct Cyc_Absyn_TypedefType_struct*)_tmp624)->f3;_tmp633=((struct Cyc_Absyn_TypedefType_struct*)
_tmp624)->f4;_LL43C: {struct Cyc_List_List*_tmp668=Cyc_Tcutil_substs(rgn,inst,
_tmp631);return _tmp668 == _tmp631?t:(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp66A;_tmp66A.tag=16;_tmp66A.f1=_tmp630;_tmp66A.f2=_tmp668;_tmp66A.f3=_tmp632;
_tmp66A.f4=_tmp633;_tmp66A;});_tmp669;});}_LL43D: if(_tmp624 <= (void*)4  || *((int*)
_tmp624)!= 7)goto _LL43F;_tmp634=((struct Cyc_Absyn_ArrayType_struct*)_tmp624)->f1;
_tmp635=(void*)_tmp634.elt_type;_tmp636=_tmp634.tq;_tmp637=_tmp634.num_elts;
_tmp638=_tmp634.zero_term;_tmp639=_tmp634.zt_loc;_LL43E: {void*_tmp66B=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp635);return _tmp66B == _tmp635?t:(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp66D;_tmp66D.tag=7;_tmp66D.f1=({struct Cyc_Absyn_ArrayInfo _tmp66E;_tmp66E.elt_type=(
void*)_tmp66B;_tmp66E.tq=_tmp636;_tmp66E.num_elts=_tmp637;_tmp66E.zero_term=
_tmp638;_tmp66E.zt_loc=_tmp639;_tmp66E;});_tmp66D;});_tmp66C;});}_LL43F: if(
_tmp624 <= (void*)4  || *((int*)_tmp624)!= 4)goto _LL441;_tmp63A=((struct Cyc_Absyn_PointerType_struct*)
_tmp624)->f1;_tmp63B=(void*)_tmp63A.elt_typ;_tmp63C=_tmp63A.elt_tq;_tmp63D=
_tmp63A.ptr_atts;_tmp63E=(void*)_tmp63D.rgn;_tmp63F=_tmp63D.nullable;_tmp640=
_tmp63D.bounds;_tmp641=_tmp63D.zero_term;_LL440: {void*_tmp66F=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp63B);void*_tmp670=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp63E);struct
Cyc_Absyn_Conref*_tmp671=_tmp640;{void*_tmp672=(void*)(Cyc_Absyn_compress_conref(
_tmp640))->v;void*_tmp673;void*_tmp674;_LL46D: if(_tmp672 <= (void*)1  || *((int*)
_tmp672)!= 0)goto _LL46F;_tmp673=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp672)->f1;if(_tmp673 <= (void*)1  || *((int*)_tmp673)!= 1)goto _LL46F;_tmp674=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp673)->f1;_LL46E: {void*_tmp675=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp674);if(_tmp674 != _tmp675)_tmp671=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp676=_cycalloc(sizeof(*_tmp676));
_tmp676[0]=({struct Cyc_Absyn_AbsUpper_b_struct _tmp677;_tmp677.tag=1;_tmp677.f1=(
void*)_tmp675;_tmp677;});_tmp676;}));goto _LL46C;}_LL46F:;_LL470: goto _LL46C;
_LL46C:;}if((_tmp66F == _tmp63B  && _tmp670 == _tmp63E) && _tmp671 == _tmp640)return
t;return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp678=_cycalloc(sizeof(*
_tmp678));_tmp678[0]=({struct Cyc_Absyn_PointerType_struct _tmp679;_tmp679.tag=4;
_tmp679.f1=({struct Cyc_Absyn_PtrInfo _tmp67A;_tmp67A.elt_typ=(void*)_tmp66F;
_tmp67A.elt_tq=_tmp63C;_tmp67A.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp67B;_tmp67B.rgn=(
void*)_tmp670;_tmp67B.nullable=_tmp63F;_tmp67B.bounds=_tmp671;_tmp67B.zero_term=
_tmp641;_tmp67B.ptrloc=0;_tmp67B;});_tmp67A;});_tmp679;});_tmp678;});}_LL441: if(
_tmp624 <= (void*)4  || *((int*)_tmp624)!= 8)goto _LL443;_tmp642=((struct Cyc_Absyn_FnType_struct*)
_tmp624)->f1;_tmp643=_tmp642.tvars;_tmp644=_tmp642.effect;_tmp645=(void*)_tmp642.ret_typ;
_tmp646=_tmp642.args;_tmp647=_tmp642.c_varargs;_tmp648=_tmp642.cyc_varargs;
_tmp649=_tmp642.rgn_po;_tmp64A=_tmp642.attributes;_LL442:{struct Cyc_List_List*
_tmp67C=_tmp643;for(0;_tmp67C != 0;_tmp67C=_tmp67C->tl){inst=({struct Cyc_List_List*
_tmp67D=_region_malloc(rgn,sizeof(*_tmp67D));_tmp67D->hd=({struct _tuple8*_tmp67E=
_region_malloc(rgn,sizeof(*_tmp67E));_tmp67E->f1=(struct Cyc_Absyn_Tvar*)_tmp67C->hd;
_tmp67E->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp67F=_cycalloc(sizeof(*
_tmp67F));_tmp67F[0]=({struct Cyc_Absyn_VarType_struct _tmp680;_tmp680.tag=1;
_tmp680.f1=(struct Cyc_Absyn_Tvar*)_tmp67C->hd;_tmp680;});_tmp67F;});_tmp67E;});
_tmp67D->tl=inst;_tmp67D;});}}{struct Cyc_List_List*_tmp682;struct Cyc_List_List*
_tmp683;struct _tuple0 _tmp681=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple16*(*f)(struct _RegionHandle*,struct _tuple2*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp646));_tmp682=_tmp681.f1;_tmp683=_tmp681.f2;{struct Cyc_List_List*_tmp684=
Cyc_Tcutil_substs(rgn,inst,_tmp683);struct Cyc_List_List*_tmp685=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp682,_tmp684));struct Cyc_Core_Opt*
eff2;if(_tmp644 == 0)eff2=0;else{void*_tmp686=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp644->v);if(_tmp686 == (void*)_tmp644->v)eff2=_tmp644;else{eff2=({struct
Cyc_Core_Opt*_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687->v=(void*)_tmp686;
_tmp687;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp648 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp689;struct Cyc_Absyn_Tqual _tmp68A;void*
_tmp68B;int _tmp68C;struct Cyc_Absyn_VarargInfo _tmp688=*_tmp648;_tmp689=_tmp688.name;
_tmp68A=_tmp688.tq;_tmp68B=(void*)_tmp688.type;_tmp68C=_tmp688.inject;{void*
_tmp68D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp68B);if(_tmp68D == _tmp68B)
cyc_varargs2=_tmp648;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp68E=
_cycalloc(sizeof(*_tmp68E));_tmp68E->name=_tmp689;_tmp68E->tq=_tmp68A;_tmp68E->type=(
void*)_tmp68D;_tmp68E->inject=_tmp68C;_tmp68E;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp690;struct Cyc_List_List*_tmp691;struct _tuple0 _tmp68F=Cyc_List_rsplit(
rgn,rgn,_tmp649);_tmp690=_tmp68F.f1;_tmp691=_tmp68F.f2;{struct Cyc_List_List*
_tmp692=Cyc_Tcutil_substs(rgn,inst,_tmp690);struct Cyc_List_List*_tmp693=Cyc_Tcutil_substs(
rgn,inst,_tmp691);if(_tmp692 == _tmp690  && _tmp693 == _tmp691)rgn_po2=_tmp649;
else{rgn_po2=Cyc_List_zip(_tmp692,_tmp693);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp694=_cycalloc(sizeof(*_tmp694));_tmp694[0]=({struct Cyc_Absyn_FnType_struct
_tmp695;_tmp695.tag=8;_tmp695.f1=({struct Cyc_Absyn_FnInfo _tmp696;_tmp696.tvars=
_tmp643;_tmp696.effect=eff2;_tmp696.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp645);_tmp696.args=_tmp685;_tmp696.c_varargs=_tmp647;_tmp696.cyc_varargs=
cyc_varargs2;_tmp696.rgn_po=rgn_po2;_tmp696.attributes=_tmp64A;_tmp696;});
_tmp695;});_tmp694;});}}}}}_LL443: if(_tmp624 <= (void*)4  || *((int*)_tmp624)!= 9)
goto _LL445;_tmp64B=((struct Cyc_Absyn_TupleType_struct*)_tmp624)->f1;_LL444: {
struct Cyc_List_List*_tmp697=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp64B);struct Cyc_List_List*_tmp698=Cyc_Tcutil_substs(rgn,inst,_tmp697);if(
_tmp698 == _tmp697)return t;{struct Cyc_List_List*_tmp699=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp64B,_tmp698);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A[0]=({struct Cyc_Absyn_TupleType_struct
_tmp69B;_tmp69B.tag=9;_tmp69B.f1=_tmp699;_tmp69B;});_tmp69A;});}}_LL445: if(
_tmp624 <= (void*)4  || *((int*)_tmp624)!= 11)goto _LL447;_tmp64C=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp624)->f1;_tmp64D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp624)->f2;_LL446: {struct Cyc_List_List*_tmp69C=((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp64D);struct Cyc_List_List*_tmp69D=
Cyc_Tcutil_substs(rgn,inst,_tmp69C);if(_tmp69D == _tmp69C)return t;{struct Cyc_List_List*
_tmp69E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,
_tmp64D,_tmp69D);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp69F=
_cycalloc(sizeof(*_tmp69F));_tmp69F[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp6A0;_tmp6A0.tag=11;_tmp6A0.f1=(void*)_tmp64C;_tmp6A0.f2=_tmp69E;_tmp6A0;});
_tmp69F;});}}_LL447: if(_tmp624 <= (void*)4  || *((int*)_tmp624)!= 0)goto _LL449;
_tmp64E=((struct Cyc_Absyn_Evar_struct*)_tmp624)->f2;_LL448: if(_tmp64E != 0)return
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmp64E->v);else{return t;}_LL449: if(
_tmp624 <= (void*)4  || *((int*)_tmp624)!= 15)goto _LL44B;_tmp64F=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp624)->f1;_LL44A: {void*_tmp6A1=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp64F);return _tmp6A1 == _tmp64F?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp6A2=_cycalloc(sizeof(*_tmp6A2));_tmp6A2[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp6A3;_tmp6A3.tag=15;_tmp6A3.f1=(void*)_tmp6A1;_tmp6A3;});_tmp6A2;});}_LL44B:
if(_tmp624 <= (void*)4  || *((int*)_tmp624)!= 14)goto _LL44D;_tmp650=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp624)->f1;_LL44C: {void*_tmp6A4=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp650);return _tmp6A4 == _tmp650?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp6A5=_cycalloc(sizeof(*_tmp6A5));_tmp6A5[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp6A6;_tmp6A6.tag=14;_tmp6A6.f1=(void*)_tmp6A4;_tmp6A6;});_tmp6A5;});}_LL44D:
if(_tmp624 <= (void*)4  || *((int*)_tmp624)!= 17)goto _LL44F;_tmp651=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp624)->f1;_LL44E: {void*_tmp6A7=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp651);return _tmp6A7 == _tmp651?t:(void*)({struct Cyc_Absyn_TagType_struct*
_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8[0]=({struct Cyc_Absyn_TagType_struct
_tmp6A9;_tmp6A9.tag=17;_tmp6A9.f1=(void*)_tmp6A7;_tmp6A9;});_tmp6A8;});}_LL44F:
if(_tmp624 <= (void*)4  || *((int*)_tmp624)!= 18)goto _LL451;_LL450: goto _LL452;
_LL451: if(_tmp624 <= (void*)4  || *((int*)_tmp624)!= 12)goto _LL453;_LL452: goto
_LL454;_LL453: if(_tmp624 <= (void*)4  || *((int*)_tmp624)!= 13)goto _LL455;_LL454:
goto _LL456;_LL455: if((int)_tmp624 != 0)goto _LL457;_LL456: goto _LL458;_LL457: if(
_tmp624 <= (void*)4  || *((int*)_tmp624)!= 5)goto _LL459;_LL458: goto _LL45A;_LL459:
if((int)_tmp624 != 1)goto _LL45B;_LL45A: goto _LL45C;_LL45B: if(_tmp624 <= (void*)4
 || *((int*)_tmp624)!= 6)goto _LL45D;_LL45C: goto _LL45E;_LL45D: if((int)_tmp624 != 3)
goto _LL45F;_LL45E: goto _LL460;_LL45F: if((int)_tmp624 != 2)goto _LL461;_LL460: return
t;_LL461: if(_tmp624 <= (void*)4  || *((int*)_tmp624)!= 21)goto _LL463;_tmp652=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp624)->f1;_LL462: {void*_tmp6AA=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp652);return _tmp6AA == _tmp652?t:(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp6AB=_cycalloc(sizeof(*_tmp6AB));_tmp6AB[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp6AC;_tmp6AC.tag=21;_tmp6AC.f1=(void*)_tmp6AA;_tmp6AC;});_tmp6AB;});}_LL463:
if(_tmp624 <= (void*)4  || *((int*)_tmp624)!= 19)goto _LL465;_tmp653=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp624)->f1;_LL464: {void*_tmp6AD=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp653);return _tmp6AD == _tmp653?t:(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp6AE=_cycalloc(sizeof(*_tmp6AE));_tmp6AE[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp6AF;_tmp6AF.tag=19;_tmp6AF.f1=(void*)_tmp6AD;_tmp6AF;});_tmp6AE;});}_LL465:
if(_tmp624 <= (void*)4  || *((int*)_tmp624)!= 20)goto _LL432;_tmp654=((struct Cyc_Absyn_JoinEff_struct*)
_tmp624)->f1;_LL466: {struct Cyc_List_List*_tmp6B0=Cyc_Tcutil_substs(rgn,inst,
_tmp654);return _tmp6B0 == _tmp654?t:(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6B1=_cycalloc(sizeof(*_tmp6B1));_tmp6B1[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6B2;_tmp6B2.tag=20;_tmp6B2.f1=_tmp6B0;_tmp6B2;});_tmp6B1;});}_LL432:;}static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp6B3=(void*)ts->hd;
struct Cyc_List_List*_tmp6B4=ts->tl;void*_tmp6B5=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp6B3);struct Cyc_List_List*_tmp6B6=Cyc_Tcutil_substs(rgn,inst,_tmp6B4);if(
_tmp6B3 == _tmp6B5  && _tmp6B4 == _tmp6B6)return ts;return(struct Cyc_List_List*)((
struct Cyc_List_List*)({struct Cyc_List_List*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));
_tmp6B7->hd=(void*)_tmp6B5;_tmp6B7->tl=_tmp6B6;_tmp6B7;}));}}extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp6B8=_cycalloc(
sizeof(*_tmp6B8));_tmp6B8->f1=tv;_tmp6B8->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->v=(void*)k;_tmp6B9;}),({struct Cyc_Core_Opt*
_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA->v=s;_tmp6BA;}));_tmp6B8;});}struct
_tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple9 _tmp6BC;struct Cyc_List_List*_tmp6BD;struct _RegionHandle*_tmp6BE;
struct _tuple9*_tmp6BB=env;_tmp6BC=*_tmp6BB;_tmp6BD=_tmp6BC.f1;_tmp6BE=_tmp6BC.f2;{
void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp6BF=_region_malloc(
_tmp6BE,sizeof(*_tmp6BF));_tmp6BF->f1=tv;_tmp6BF->f2=Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0->v=(void*)k;_tmp6C0;}),({
struct Cyc_Core_Opt*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1->v=_tmp6BD;
_tmp6C1;}));_tmp6BF;});}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2))({
struct Cyc_String_pa_struct _tmp6C6;_tmp6C6.tag=0;_tmp6C6.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_kindbound2string(k2));{struct Cyc_String_pa_struct
_tmp6C5;_tmp6C5.tag=0;_tmp6C5.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kindbound2string(
k1));{struct Cyc_String_pa_struct _tmp6C4;_tmp6C4.tag=0;_tmp6C4.f1=(struct
_tagged_arr)((struct _tagged_arr)*tv->name);{void*_tmp6C2[3]={& _tmp6C4,& _tmp6C5,&
_tmp6C6};Cyc_Tcutil_terr(loc,({const char*_tmp6C3="type variable %s is used with inconsistent kinds %s and %s";
_tag_arr(_tmp6C3,sizeof(char),_get_zero_arr_size(_tmp6C3,59));}),_tag_arr(
_tmp6C2,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))!= *((int*)
_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*_tmp6C7[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp6C8="same type variable has different identity!";_tag_arr(_tmp6C8,sizeof(
char),_get_zero_arr_size(_tmp6C8,43));}),_tag_arr(_tmp6C7,sizeof(void*),0));});}
return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();return({struct Cyc_List_List*
_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9->hd=tv;_tmp6C9->tl=tvs;_tmp6C9;});}
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*_tmp6CA[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp6CB="fast_add_free_tvar: bad identity in tv";
_tag_arr(_tmp6CB,sizeof(char),_get_zero_arr_size(_tmp6CB,39));}),_tag_arr(
_tmp6CA,sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=
tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp6CC[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp6CD="fast_add_free_tvar: bad identity in tvs2";_tag_arr(_tmp6CD,sizeof(char),
_get_zero_arr_size(_tmp6CD,41));}),_tag_arr(_tmp6CC,sizeof(void*),0));});if(*((
int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))== *((int*)
_check_null(tv->identity)))return tvs;}}return({struct Cyc_List_List*_tmp6CE=
_cycalloc(sizeof(*_tmp6CE));_tmp6CE->hd=tv;_tmp6CE->tl=tvs;_tmp6CE;});}static
struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*
tv){if(tv->identity == 0)({struct Cyc_String_pa_struct _tmp6D1;_tmp6D1.tag=0;
_tmp6D1.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string(tv));{
void*_tmp6CF[1]={& _tmp6D1};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp6D0="bound tvar id for %s is NULL";_tag_arr(
_tmp6D0,sizeof(char),_get_zero_arr_size(_tmp6D0,29));}),_tag_arr(_tmp6CF,sizeof(
void*),1));}});return({struct Cyc_List_List*_tmp6D2=_cycalloc(sizeof(*_tmp6D2));
_tmp6D2->hd=tv;_tmp6D2->tl=tvs;_tmp6D2;});}static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(
struct Cyc_List_List*es,void*e){void*_tmp6D3=Cyc_Tcutil_compress(e);int _tmp6D4;
_LL472: if(_tmp6D3 <= (void*)4  || *((int*)_tmp6D3)!= 0)goto _LL474;_tmp6D4=((struct
Cyc_Absyn_Evar_struct*)_tmp6D3)->f3;_LL473:{struct Cyc_List_List*es2=es;for(0;es2
!= 0;es2=es2->tl){void*_tmp6D5=Cyc_Tcutil_compress((void*)es2->hd);int _tmp6D6;
_LL477: if(_tmp6D5 <= (void*)4  || *((int*)_tmp6D5)!= 0)goto _LL479;_tmp6D6=((struct
Cyc_Absyn_Evar_struct*)_tmp6D5)->f3;_LL478: if(_tmp6D4 == _tmp6D6)return es;goto
_LL476;_LL479:;_LL47A: goto _LL476;_LL476:;}}return({struct Cyc_List_List*_tmp6D7=
_cycalloc(sizeof(*_tmp6D7));_tmp6D7->hd=(void*)e;_tmp6D7->tl=es;_tmp6D7;});
_LL474:;_LL475: return es;_LL471:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;
tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))== *((int*)
_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(!
present)r=({struct Cyc_List_List*_tmp6D8=_cycalloc(sizeof(*_tmp6D8));_tmp6D8->hd=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6D8->tl=r;_tmp6D8;});}r=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)width))({struct Cyc_String_pa_struct _tmp6DB;_tmp6DB.tag=
0;_tmp6DB.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{void*_tmp6D9[1]={&
_tmp6DB};Cyc_Tcutil_terr(loc,({const char*_tmp6DA="bitfield %s does not have constant width";
_tag_arr(_tmp6DA,sizeof(char),_get_zero_arr_size(_tmp6DA,41));}),_tag_arr(
_tmp6D9,sizeof(void*),1));}});else{unsigned int _tmp6DD;int _tmp6DE;struct _tuple7
_tmp6DC=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)width);_tmp6DD=
_tmp6DC.f1;_tmp6DE=_tmp6DC.f2;if(!_tmp6DE)({void*_tmp6DF[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp6E0="bitfield width cannot use sizeof or offsetof";_tag_arr(
_tmp6E0,sizeof(char),_get_zero_arr_size(_tmp6E0,45));}),_tag_arr(_tmp6DF,sizeof(
void*),0));});w=_tmp6DD;}{void*_tmp6E1=Cyc_Tcutil_compress(field_typ);void*
_tmp6E2;_LL47C: if(_tmp6E1 <= (void*)4  || *((int*)_tmp6E1)!= 5)goto _LL47E;_tmp6E2=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp6E1)->f2;_LL47D:{void*_tmp6E3=
_tmp6E2;_LL481: if((int)_tmp6E3 != 0)goto _LL483;_LL482: if(w > 8)({void*_tmp6E4[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp6E5="bitfield larger than type";_tag_arr(
_tmp6E5,sizeof(char),_get_zero_arr_size(_tmp6E5,26));}),_tag_arr(_tmp6E4,sizeof(
void*),0));});goto _LL480;_LL483: if((int)_tmp6E3 != 1)goto _LL485;_LL484: if(w > 16)({
void*_tmp6E6[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6E7="bitfield larger than type";
_tag_arr(_tmp6E7,sizeof(char),_get_zero_arr_size(_tmp6E7,26));}),_tag_arr(
_tmp6E6,sizeof(void*),0));});goto _LL480;_LL485: if((int)_tmp6E3 != 2)goto _LL487;
_LL486: if(w > 32)({void*_tmp6E8[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6E9="bitfield larger than type";
_tag_arr(_tmp6E9,sizeof(char),_get_zero_arr_size(_tmp6E9,26));}),_tag_arr(
_tmp6E8,sizeof(void*),0));});goto _LL480;_LL487: if((int)_tmp6E3 != 3)goto _LL480;
_LL488: if(w > 64)({void*_tmp6EA[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6EB="bitfield larger than type";
_tag_arr(_tmp6EB,sizeof(char),_get_zero_arr_size(_tmp6EB,26));}),_tag_arr(
_tmp6EA,sizeof(void*),0));});goto _LL480;_LL480:;}goto _LL47B;_LL47E:;_LL47F:({
struct Cyc_String_pa_struct _tmp6EF;_tmp6EF.tag=0;_tmp6EF.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(field_typ));{struct Cyc_String_pa_struct
_tmp6EE;_tmp6EE.tag=0;_tmp6EE.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
void*_tmp6EC[2]={& _tmp6EE,& _tmp6EF};Cyc_Tcutil_terr(loc,({const char*_tmp6ED="bitfield %s must have integral type but has type %s";
_tag_arr(_tmp6ED,sizeof(char),_get_zero_arr_size(_tmp6ED,52));}),_tag_arr(
_tmp6EC,sizeof(void*),2));}}});goto _LL47B;_LL47B:;}}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _tagged_arr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp6F0=(void*)atts->hd;_LL48A: if((int)
_tmp6F0 != 5)goto _LL48C;_LL48B: continue;_LL48C: if(_tmp6F0 <= (void*)17  || *((int*)
_tmp6F0)!= 1)goto _LL48E;_LL48D: continue;_LL48E:;_LL48F:({struct Cyc_String_pa_struct
_tmp6F4;_tmp6F4.tag=0;_tmp6F4.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
struct Cyc_String_pa_struct _tmp6F3;_tmp6F3.tag=0;_tmp6F3.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp6F1[2]={&
_tmp6F3,& _tmp6F4};Cyc_Tcutil_terr(loc,({const char*_tmp6F2="bad attribute %s on member %s";
_tag_arr(_tmp6F2,sizeof(char),_get_zero_arr_size(_tmp6F2,30));}),_tag_arr(
_tmp6F1,sizeof(void*),2));}}});_LL489:;}}struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(struct
Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp6F5=t;struct Cyc_Absyn_Typedefdecl*
_tmp6F6;void**_tmp6F7;_LL491: if(_tmp6F5 <= (void*)4  || *((int*)_tmp6F5)!= 16)goto
_LL493;_tmp6F6=((struct Cyc_Absyn_TypedefType_struct*)_tmp6F5)->f3;_tmp6F7=((
struct Cyc_Absyn_TypedefType_struct*)_tmp6F5)->f4;_LL492: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmp6F6))->tq).real_const  || (_tmp6F6->tq).print_const){if(
declared_const)({void*_tmp6F8[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp6F9="extra const";
_tag_arr(_tmp6F9,sizeof(char),_get_zero_arr_size(_tmp6F9,12));}),_tag_arr(
_tmp6F8,sizeof(void*),0));});return 1;}if((unsigned int)_tmp6F7)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp6F7);else{return declared_const;}_LL493:;_LL494: return
declared_const;_LL490:;}struct _tuple17{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t,int allow_unique){{
void*_tmp6FA=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp6FB;struct Cyc_Core_Opt**
_tmp6FC;struct Cyc_Core_Opt*_tmp6FD;struct Cyc_Core_Opt**_tmp6FE;struct Cyc_Absyn_Tvar*
_tmp6FF;struct Cyc_List_List*_tmp700;struct _tuple1*_tmp701;struct Cyc_Absyn_Enumdecl*
_tmp702;struct Cyc_Absyn_Enumdecl**_tmp703;struct Cyc_Absyn_TunionInfo _tmp704;void*
_tmp705;void**_tmp706;struct Cyc_List_List*_tmp707;struct Cyc_List_List**_tmp708;
void*_tmp709;struct Cyc_Absyn_TunionFieldInfo _tmp70A;void*_tmp70B;void**_tmp70C;
struct Cyc_List_List*_tmp70D;struct Cyc_Absyn_PtrInfo _tmp70E;void*_tmp70F;struct
Cyc_Absyn_Tqual _tmp710;struct Cyc_Absyn_Tqual*_tmp711;struct Cyc_Absyn_PtrAtts
_tmp712;void*_tmp713;struct Cyc_Absyn_Conref*_tmp714;struct Cyc_Absyn_Conref*
_tmp715;struct Cyc_Absyn_Conref*_tmp716;void*_tmp717;void*_tmp718;struct Cyc_Absyn_ArrayInfo
_tmp719;void*_tmp71A;struct Cyc_Absyn_Tqual _tmp71B;struct Cyc_Absyn_Tqual*_tmp71C;
struct Cyc_Absyn_Exp*_tmp71D;struct Cyc_Absyn_Exp**_tmp71E;struct Cyc_Absyn_Conref*
_tmp71F;struct Cyc_Position_Segment*_tmp720;struct Cyc_Absyn_FnInfo _tmp721;struct
Cyc_List_List*_tmp722;struct Cyc_List_List**_tmp723;struct Cyc_Core_Opt*_tmp724;
struct Cyc_Core_Opt**_tmp725;void*_tmp726;struct Cyc_List_List*_tmp727;int _tmp728;
struct Cyc_Absyn_VarargInfo*_tmp729;struct Cyc_List_List*_tmp72A;struct Cyc_List_List*
_tmp72B;struct Cyc_List_List*_tmp72C;void*_tmp72D;struct Cyc_List_List*_tmp72E;
struct Cyc_Absyn_AggrInfo _tmp72F;void*_tmp730;void**_tmp731;struct Cyc_List_List*
_tmp732;struct Cyc_List_List**_tmp733;struct _tuple1*_tmp734;struct Cyc_List_List*
_tmp735;struct Cyc_List_List**_tmp736;struct Cyc_Absyn_Typedefdecl*_tmp737;struct
Cyc_Absyn_Typedefdecl**_tmp738;void**_tmp739;void***_tmp73A;void*_tmp73B;void*
_tmp73C;void*_tmp73D;struct Cyc_List_List*_tmp73E;_LL496: if((int)_tmp6FA != 0)goto
_LL498;_LL497: goto _LL495;_LL498: if(_tmp6FA <= (void*)4  || *((int*)_tmp6FA)!= 0)
goto _LL49A;_tmp6FB=((struct Cyc_Absyn_Evar_struct*)_tmp6FA)->f1;_tmp6FC=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp6FA)->f1;_tmp6FD=((struct Cyc_Absyn_Evar_struct*)
_tmp6FA)->f2;_tmp6FE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp6FA)->f2;_LL499: if(*_tmp6FC == 0)*_tmp6FC=({struct Cyc_Core_Opt*_tmp73F=
_cycalloc(sizeof(*_tmp73F));_tmp73F->v=(void*)expected_kind;_tmp73F;});if((
cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind == (void*)3)*
_tmp6FE=({struct Cyc_Core_Opt*_tmp740=_cycalloc(sizeof(*_tmp740));_tmp740->v=(
void*)((void*)2);_tmp740;});else{if(cvtenv.generalize_evars){struct Cyc_Absyn_Tvar*
_tmp741=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp745=
_cycalloc(sizeof(*_tmp745));_tmp745[0]=({struct Cyc_Absyn_Less_kb_struct _tmp746;
_tmp746.tag=2;_tmp746.f1=0;_tmp746.f2=(void*)expected_kind;_tmp746;});_tmp745;}));*
_tmp6FE=({struct Cyc_Core_Opt*_tmp742=_cycalloc(sizeof(*_tmp742));_tmp742->v=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp743=_cycalloc(sizeof(*_tmp743));
_tmp743[0]=({struct Cyc_Absyn_VarType_struct _tmp744;_tmp744.tag=1;_tmp744.f1=
_tmp741;_tmp744;});_tmp743;}));_tmp742;});_tmp6FF=_tmp741;goto _LL49B;}else{
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}goto _LL495;
_LL49A: if(_tmp6FA <= (void*)4  || *((int*)_tmp6FA)!= 1)goto _LL49C;_tmp6FF=((struct
Cyc_Absyn_VarType_struct*)_tmp6FA)->f1;_LL49B:{void*_tmp747=Cyc_Absyn_compress_kb((
void*)_tmp6FF->kind);struct Cyc_Core_Opt*_tmp748;struct Cyc_Core_Opt**_tmp749;
_LL4CB: if(*((int*)_tmp747)!= 1)goto _LL4CD;_tmp748=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp747)->f1;_tmp749=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp747)->f1;_LL4CC:*_tmp749=({struct Cyc_Core_Opt*_tmp74A=_cycalloc(sizeof(*
_tmp74A));_tmp74A->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp74B=
_cycalloc(sizeof(*_tmp74B));_tmp74B[0]=({struct Cyc_Absyn_Less_kb_struct _tmp74C;
_tmp74C.tag=2;_tmp74C.f1=0;_tmp74C.f2=(void*)expected_kind;_tmp74C;});_tmp74B;}));
_tmp74A;});goto _LL4CA;_LL4CD:;_LL4CE: goto _LL4CA;_LL4CA:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp6FF);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp6FF);goto _LL495;_LL49C: if(_tmp6FA <= (void*)4  || *((int*)
_tmp6FA)!= 13)goto _LL49E;_tmp700=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp6FA)->f1;
_LL49D: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp74D=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp74D;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp700 != 0;_tmp700=_tmp700->tl){struct Cyc_Absyn_Enumfield*_tmp74E=(
struct Cyc_Absyn_Enumfield*)_tmp700->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_strptrcmp,prev_fields,(*_tmp74E->name).f2))({struct Cyc_String_pa_struct
_tmp751;_tmp751.tag=0;_tmp751.f1=(struct _tagged_arr)((struct _tagged_arr)*(*
_tmp74E->name).f2);{void*_tmp74F[1]={& _tmp751};Cyc_Tcutil_terr(_tmp74E->loc,({
const char*_tmp750="duplicate enum field name %s";_tag_arr(_tmp750,sizeof(char),
_get_zero_arr_size(_tmp750,29));}),_tag_arr(_tmp74F,sizeof(void*),1));}});else{
prev_fields=({struct Cyc_List_List*_tmp752=_region_malloc(uprev_rgn,sizeof(*
_tmp752));_tmp752->hd=(*_tmp74E->name).f2;_tmp752->tl=prev_fields;_tmp752;});}
if(_tmp74E->tag == 0)_tmp74E->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp74E->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp74E->tag)))({struct Cyc_String_pa_struct _tmp755;_tmp755.tag=0;
_tmp755.f1=(struct _tagged_arr)((struct _tagged_arr)*(*_tmp74E->name).f2);{void*
_tmp753[1]={& _tmp755};Cyc_Tcutil_terr(loc,({const char*_tmp754="enum field %s: expression is not constant";
_tag_arr(_tmp754,sizeof(char),_get_zero_arr_size(_tmp754,42));}),_tag_arr(
_tmp753,sizeof(void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp74E->tag))).f1;tag_count=t1 + 1;(*_tmp74E->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp756=_cycalloc(sizeof(*_tmp756));_tmp756[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp757;_tmp757.tag=1;_tmp757.f1=te->ns;_tmp757;});
_tmp756;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple17*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp74E->name).f2,({
struct _tuple17*_tmp758=_cycalloc(sizeof(*_tmp758));_tmp758->f1=(void*)({struct
Cyc_Tcenv_AnonEnumRes_struct*_tmp759=_cycalloc(sizeof(*_tmp759));_tmp759[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmp75A;_tmp75A.tag=4;_tmp75A.f1=(void*)t;
_tmp75A.f2=_tmp74E;_tmp75A;});_tmp759;});_tmp758->f2=1;_tmp758;}));}}};
_pop_region(uprev_rgn);}goto _LL495;}_LL49E: if(_tmp6FA <= (void*)4  || *((int*)
_tmp6FA)!= 12)goto _LL4A0;_tmp701=((struct Cyc_Absyn_EnumType_struct*)_tmp6FA)->f1;
_tmp702=((struct Cyc_Absyn_EnumType_struct*)_tmp6FA)->f2;_tmp703=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp6FA)->f2;_LL49F: if(*_tmp703 == 0  || ((struct
Cyc_Absyn_Enumdecl*)_check_null(*_tmp703))->fields == 0){struct _handler_cons
_tmp75B;_push_handler(& _tmp75B);{int _tmp75D=0;if(setjmp(_tmp75B.handler))_tmp75D=
1;if(!_tmp75D){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,
_tmp701);*_tmp703=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{void*
_tmp75C=(void*)_exn_thrown;void*_tmp75F=_tmp75C;_LL4D0: if(_tmp75F != Cyc_Dict_Absent)
goto _LL4D2;_LL4D1: {struct Cyc_Tcenv_Genv*_tmp760=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp761=({struct Cyc_Absyn_Enumdecl*_tmp762=_cycalloc(
sizeof(*_tmp762));_tmp762->sc=(void*)((void*)3);_tmp762->name=_tmp701;_tmp762->fields=
0;_tmp762;});Cyc_Tc_tcEnumdecl(te,_tmp760,loc,_tmp761);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp701);*_tmp703=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL4CF;}}_LL4D2:;_LL4D3:(void)_throw(_tmp75F);_LL4CF:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp703);*_tmp701=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL495;}_LL4A0: if(_tmp6FA <= (void*)4
 || *((int*)_tmp6FA)!= 2)goto _LL4A2;_tmp704=((struct Cyc_Absyn_TunionType_struct*)
_tmp6FA)->f1;_tmp705=(void*)_tmp704.tunion_info;_tmp706=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp6FA)->f1).tunion_info;_tmp707=_tmp704.targs;_tmp708=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp6FA)->f1).targs;_tmp709=(void*)_tmp704.rgn;
_LL4A1: {struct Cyc_List_List*_tmp763=*_tmp708;{void*_tmp764=*_tmp706;struct Cyc_Absyn_UnknownTunionInfo
_tmp765;struct _tuple1*_tmp766;int _tmp767;struct Cyc_Absyn_Tuniondecl**_tmp768;
struct Cyc_Absyn_Tuniondecl*_tmp769;_LL4D5: if(*((int*)_tmp764)!= 0)goto _LL4D7;
_tmp765=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp764)->f1;_tmp766=_tmp765.name;
_tmp767=_tmp765.is_xtunion;_LL4D6: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp76A;_push_handler(& _tmp76A);{int _tmp76C=0;if(setjmp(_tmp76A.handler))
_tmp76C=1;if(!_tmp76C){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp766);;
_pop_handler();}else{void*_tmp76B=(void*)_exn_thrown;void*_tmp76E=_tmp76B;_LL4DA:
if(_tmp76E != Cyc_Dict_Absent)goto _LL4DC;_LL4DB: {struct Cyc_Tcenv_Genv*_tmp76F=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp770=({struct Cyc_Absyn_Tuniondecl*
_tmp777=_cycalloc(sizeof(*_tmp777));_tmp777->sc=(void*)((void*)3);_tmp777->name=
_tmp766;_tmp777->tvs=0;_tmp777->fields=0;_tmp777->is_xtunion=_tmp767;_tmp777;});
Cyc_Tc_tcTuniondecl(te,_tmp76F,loc,_tmp770);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp766);if(_tmp763 != 0){({struct Cyc_String_pa_struct _tmp774;_tmp774.tag=0;
_tmp774.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp766));{struct Cyc_String_pa_struct _tmp773;_tmp773.tag=0;_tmp773.f1=(struct
_tagged_arr)(_tmp767?(struct _tagged_arr)((struct _tagged_arr)({const char*_tmp775="xtunion";
_tag_arr(_tmp775,sizeof(char),_get_zero_arr_size(_tmp775,8));})):(struct
_tagged_arr)({const char*_tmp776="tunion";_tag_arr(_tmp776,sizeof(char),
_get_zero_arr_size(_tmp776,7));}));{void*_tmp771[2]={& _tmp773,& _tmp774};Cyc_Tcutil_terr(
loc,({const char*_tmp772="declare parameterized %s %s before using";_tag_arr(
_tmp772,sizeof(char),_get_zero_arr_size(_tmp772,41));}),_tag_arr(_tmp771,sizeof(
void*),2));}}});return cvtenv;}goto _LL4D9;}_LL4DC:;_LL4DD:(void)_throw(_tmp76E);
_LL4D9:;}}}if((*tudp)->is_xtunion != _tmp767)({struct Cyc_String_pa_struct _tmp77A;
_tmp77A.tag=0;_tmp77A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp766));{void*_tmp778[1]={& _tmp77A};Cyc_Tcutil_terr(loc,({const char*_tmp779="[x]tunion is different from type declaration %s";
_tag_arr(_tmp779,sizeof(char),_get_zero_arr_size(_tmp779,48));}),_tag_arr(
_tmp778,sizeof(void*),1));}});*_tmp706=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp77B=_cycalloc(sizeof(*_tmp77B));_tmp77B[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp77C;_tmp77C.tag=1;_tmp77C.f1=tudp;_tmp77C;});_tmp77B;});_tmp769=*tudp;goto
_LL4D8;}_LL4D7: if(*((int*)_tmp764)!= 1)goto _LL4D4;_tmp768=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp764)->f1;_tmp769=*_tmp768;_LL4D8: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp709,allow_unique);{struct Cyc_List_List*tvs=_tmp769->tvs;for(
0;_tmp763 != 0  && tvs != 0;(_tmp763=_tmp763->tl,tvs=tvs->tl)){void*t1=(void*)
_tmp763->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1,0);}if(_tmp763 != 0)({struct Cyc_String_pa_struct
_tmp77F;_tmp77F.tag=0;_tmp77F.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp769->name));{void*_tmp77D[1]={& _tmp77F};Cyc_Tcutil_terr(loc,({const char*
_tmp77E="too many type arguments for tunion %s";_tag_arr(_tmp77E,sizeof(char),
_get_zero_arr_size(_tmp77E,38));}),_tag_arr(_tmp77D,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp780=_cycalloc(sizeof(*_tmp780));_tmp780->hd=(void*)e;_tmp780->tl=
hidden_ts;_tmp780;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,0);}*
_tmp708=Cyc_List_imp_append(*_tmp708,Cyc_List_imp_rev(hidden_ts));}goto _LL4D4;}
_LL4D4:;}goto _LL495;}_LL4A2: if(_tmp6FA <= (void*)4  || *((int*)_tmp6FA)!= 3)goto
_LL4A4;_tmp70A=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp6FA)->f1;_tmp70B=(
void*)_tmp70A.field_info;_tmp70C=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp6FA)->f1).field_info;_tmp70D=_tmp70A.targs;_LL4A3:{void*_tmp781=*_tmp70C;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp782;struct _tuple1*_tmp783;struct
_tuple1*_tmp784;int _tmp785;struct Cyc_Absyn_Tuniondecl*_tmp786;struct Cyc_Absyn_Tunionfield*
_tmp787;_LL4DF: if(*((int*)_tmp781)!= 0)goto _LL4E1;_tmp782=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp781)->f1;_tmp783=_tmp782.tunion_name;_tmp784=_tmp782.field_name;_tmp785=
_tmp782.is_xtunion;_LL4E0: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp788;_push_handler(& _tmp788);{int _tmp78A=0;if(setjmp(
_tmp788.handler))_tmp78A=1;if(!_tmp78A){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp783);;_pop_handler();}else{void*_tmp789=(void*)_exn_thrown;void*_tmp78C=
_tmp789;_LL4E4: if(_tmp78C != Cyc_Dict_Absent)goto _LL4E6;_LL4E5:({struct Cyc_String_pa_struct
_tmp78F;_tmp78F.tag=0;_tmp78F.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp783));{void*_tmp78D[1]={& _tmp78F};Cyc_Tcutil_terr(loc,({const char*_tmp78E="unbound type tunion %s";
_tag_arr(_tmp78E,sizeof(char),_get_zero_arr_size(_tmp78E,23));}),_tag_arr(
_tmp78D,sizeof(void*),1));}});return cvtenv;_LL4E6:;_LL4E7:(void)_throw(_tmp78C);
_LL4E3:;}}}{struct _handler_cons _tmp790;_push_handler(& _tmp790);{int _tmp792=0;if(
setjmp(_tmp790.handler))_tmp792=1;if(!_tmp792){{void*_tmp793=Cyc_Tcenv_lookup_ordinary(
te,loc,_tmp784);struct Cyc_Absyn_Tuniondecl*_tmp794;struct Cyc_Absyn_Tunionfield*
_tmp795;_LL4E9: if(*((int*)_tmp793)!= 2)goto _LL4EB;_tmp794=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp793)->f1;_tmp795=((struct Cyc_Tcenv_TunionRes_struct*)_tmp793)->f2;_LL4EA: tuf=
_tmp795;tud=_tmp794;if(tud->is_xtunion != _tmp785)({struct Cyc_String_pa_struct
_tmp798;_tmp798.tag=0;_tmp798.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp783));{void*_tmp796[1]={& _tmp798};Cyc_Tcutil_terr(loc,({const char*_tmp797="[x]tunion is different from type declaration %s";
_tag_arr(_tmp797,sizeof(char),_get_zero_arr_size(_tmp797,48));}),_tag_arr(
_tmp796,sizeof(void*),1));}});goto _LL4E8;_LL4EB:;_LL4EC:({struct Cyc_String_pa_struct
_tmp79C;_tmp79C.tag=0;_tmp79C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp783));{struct Cyc_String_pa_struct _tmp79B;_tmp79B.tag=0;_tmp79B.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp784));{void*_tmp799[
2]={& _tmp79B,& _tmp79C};Cyc_Tcutil_terr(loc,({const char*_tmp79A="unbound field %s in type tunion %s";
_tag_arr(_tmp79A,sizeof(char),_get_zero_arr_size(_tmp79A,35));}),_tag_arr(
_tmp799,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv _tmp79D=cvtenv;
_npop_handler(0);return _tmp79D;}_LL4E8:;};_pop_handler();}else{void*_tmp791=(
void*)_exn_thrown;void*_tmp79F=_tmp791;_LL4EE: if(_tmp79F != Cyc_Dict_Absent)goto
_LL4F0;_LL4EF:({struct Cyc_String_pa_struct _tmp7A3;_tmp7A3.tag=0;_tmp7A3.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp783));{struct
Cyc_String_pa_struct _tmp7A2;_tmp7A2.tag=0;_tmp7A2.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp784));{void*_tmp7A0[2]={& _tmp7A2,&
_tmp7A3};Cyc_Tcutil_terr(loc,({const char*_tmp7A1="unbound field %s in type tunion %s";
_tag_arr(_tmp7A1,sizeof(char),_get_zero_arr_size(_tmp7A1,35));}),_tag_arr(
_tmp7A0,sizeof(void*),2));}}});return cvtenv;_LL4F0:;_LL4F1:(void)_throw(_tmp79F);
_LL4ED:;}}}*_tmp70C=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmp7A4=
_cycalloc(sizeof(*_tmp7A4));_tmp7A4[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp7A5;_tmp7A5.tag=1;_tmp7A5.f1=tud;_tmp7A5.f2=tuf;_tmp7A5;});_tmp7A4;});
_tmp786=tud;_tmp787=tuf;goto _LL4E2;}_LL4E1: if(*((int*)_tmp781)!= 1)goto _LL4DE;
_tmp786=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp781)->f1;_tmp787=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp781)->f2;_LL4E2: {struct Cyc_List_List*tvs=
_tmp786->tvs;for(0;_tmp70D != 0  && tvs != 0;(_tmp70D=_tmp70D->tl,tvs=tvs->tl)){
void*t1=(void*)_tmp70D->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1,0);}if(_tmp70D
!= 0)({struct Cyc_String_pa_struct _tmp7A9;_tmp7A9.tag=0;_tmp7A9.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp787->name));{struct
Cyc_String_pa_struct _tmp7A8;_tmp7A8.tag=0;_tmp7A8.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp786->name));{void*_tmp7A6[2]={& _tmp7A8,&
_tmp7A9};Cyc_Tcutil_terr(loc,({const char*_tmp7A7="too many type arguments for tunion %s.%s";
_tag_arr(_tmp7A7,sizeof(char),_get_zero_arr_size(_tmp7A7,41));}),_tag_arr(
_tmp7A6,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_String_pa_struct _tmp7AD;
_tmp7AD.tag=0;_tmp7AD.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp787->name));{struct Cyc_String_pa_struct _tmp7AC;_tmp7AC.tag=0;_tmp7AC.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp786->name));{
void*_tmp7AA[2]={& _tmp7AC,& _tmp7AD};Cyc_Tcutil_terr(loc,({const char*_tmp7AB="too few type arguments for tunion %s.%s";
_tag_arr(_tmp7AB,sizeof(char),_get_zero_arr_size(_tmp7AB,40));}),_tag_arr(
_tmp7AA,sizeof(void*),2));}}});goto _LL4DE;}_LL4DE:;}goto _LL495;_LL4A4: if(_tmp6FA
<= (void*)4  || *((int*)_tmp6FA)!= 4)goto _LL4A6;_tmp70E=((struct Cyc_Absyn_PointerType_struct*)
_tmp6FA)->f1;_tmp70F=(void*)_tmp70E.elt_typ;_tmp710=_tmp70E.elt_tq;_tmp711=(
struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_struct*)_tmp6FA)->f1).elt_tq;
_tmp712=_tmp70E.ptr_atts;_tmp713=(void*)_tmp712.rgn;_tmp714=_tmp712.nullable;
_tmp715=_tmp712.bounds;_tmp716=_tmp712.zero_term;_LL4A5: {int is_zero_terminated;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp70F,0);_tmp711->real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,_tmp711->print_const,_tmp70F);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp713,allow_unique);{void*
_tmp7AE=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp716))->v;int _tmp7AF;_LL4F3: if((int)_tmp7AE != 0)goto _LL4F5;_LL4F4:{void*
_tmp7B0=Cyc_Tcutil_compress(_tmp70F);void*_tmp7B1;_LL4FA: if(_tmp7B0 <= (void*)4
 || *((int*)_tmp7B0)!= 5)goto _LL4FC;_tmp7B1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp7B0)->f2;if((int)_tmp7B1 != 0)goto _LL4FC;_LL4FB:((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp716,Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL4F9;_LL4FC:;_LL4FD:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp716,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL4F9;
_LL4F9:;}goto _LL4F2;_LL4F5: if(_tmp7AE <= (void*)1  || *((int*)_tmp7AE)!= 0)goto
_LL4F7;_tmp7AF=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7AE)->f1;if(_tmp7AF
!= 1)goto _LL4F7;_LL4F6: if(!Cyc_Tcutil_admits_zero(_tmp70F))({struct Cyc_String_pa_struct
_tmp7B4;_tmp7B4.tag=0;_tmp7B4.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp70F));{void*_tmp7B2[1]={& _tmp7B4};Cyc_Tcutil_terr(loc,({const char*_tmp7B3="cannot have a pointer to zero-terminated %s elements";
_tag_arr(_tmp7B3,sizeof(char),_get_zero_arr_size(_tmp7B3,53));}),_tag_arr(
_tmp7B2,sizeof(void*),1));}});is_zero_terminated=1;goto _LL4F2;_LL4F7:;_LL4F8:
is_zero_terminated=0;goto _LL4F2;_LL4F2:;}{void*_tmp7B5=(void*)(Cyc_Absyn_compress_conref(
_tmp715))->v;void*_tmp7B6;void*_tmp7B7;struct Cyc_Absyn_Exp*_tmp7B8;void*_tmp7B9;
void*_tmp7BA;_LL4FF: if((int)_tmp7B5 != 0)goto _LL501;_LL500: goto _LL502;_LL501: if(
_tmp7B5 <= (void*)1  || *((int*)_tmp7B5)!= 0)goto _LL503;_tmp7B6=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7B5)->f1;if((int)_tmp7B6 != 0)goto _LL503;_LL502: goto _LL4FE;_LL503: if(_tmp7B5
<= (void*)1  || *((int*)_tmp7B5)!= 0)goto _LL505;_tmp7B7=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7B5)->f1;if(_tmp7B7 <= (void*)1  || *((int*)_tmp7B7)!= 0)goto _LL505;_tmp7B8=((
struct Cyc_Absyn_Upper_b_struct*)_tmp7B7)->f1;_LL504: Cyc_Tcexp_tcExp(te,0,_tmp7B8);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7B8))({void*_tmp7BB[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp7BC="pointer bounds expression is not an unsigned int";
_tag_arr(_tmp7BC,sizeof(char),_get_zero_arr_size(_tmp7BC,49));}),_tag_arr(
_tmp7BB,sizeof(void*),0));});{unsigned int _tmp7BE;int _tmp7BF;struct _tuple7
_tmp7BD=Cyc_Evexp_eval_const_uint_exp(_tmp7B8);_tmp7BE=_tmp7BD.f1;_tmp7BF=
_tmp7BD.f2;if(is_zero_terminated  && (!_tmp7BF  || _tmp7BE < 1))({void*_tmp7C0[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp7C1="zero-terminated pointer cannot point to empty sequence";
_tag_arr(_tmp7C1,sizeof(char),_get_zero_arr_size(_tmp7C1,55));}),_tag_arr(
_tmp7C0,sizeof(void*),0));});goto _LL4FE;}_LL505: if(_tmp7B5 <= (void*)1  || *((int*)
_tmp7B5)!= 0)goto _LL507;_tmp7B9=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7B5)->f1;if(_tmp7B9 <= (void*)1  || *((int*)_tmp7B9)!= 1)goto _LL507;_tmp7BA=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp7B9)->f1;_LL506: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)5,_tmp7BA,0);goto _LL4FE;_LL507: if(_tmp7B5 <= (void*)1  || *((
int*)_tmp7B5)!= 1)goto _LL4FE;_LL508:({void*_tmp7C2[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp7C3="forward constraint";
_tag_arr(_tmp7C3,sizeof(char),_get_zero_arr_size(_tmp7C3,19));}),_tag_arr(
_tmp7C2,sizeof(void*),0));});_LL4FE:;}goto _LL495;}_LL4A6: if(_tmp6FA <= (void*)4
 || *((int*)_tmp6FA)!= 17)goto _LL4A8;_tmp717=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp6FA)->f1;_LL4A7: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp717,allow_unique);goto _LL495;_LL4A8: if(_tmp6FA <= (void*)4  || *((int*)_tmp6FA)
!= 14)goto _LL4AA;_tmp718=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp6FA)->f1;
_LL4A9: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp718,
allow_unique);goto _LL495;_LL4AA: if(_tmp6FA <= (void*)4  || *((int*)_tmp6FA)!= 5)
goto _LL4AC;_LL4AB: goto _LL4AD;_LL4AC: if((int)_tmp6FA != 1)goto _LL4AE;_LL4AD: goto
_LL4AF;_LL4AE: if(_tmp6FA <= (void*)4  || *((int*)_tmp6FA)!= 6)goto _LL4B0;_LL4AF:
goto _LL495;_LL4B0: if(_tmp6FA <= (void*)4  || *((int*)_tmp6FA)!= 7)goto _LL4B2;
_tmp719=((struct Cyc_Absyn_ArrayType_struct*)_tmp6FA)->f1;_tmp71A=(void*)_tmp719.elt_type;
_tmp71B=_tmp719.tq;_tmp71C=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp6FA)->f1).tq;_tmp71D=_tmp719.num_elts;_tmp71E=(struct Cyc_Absyn_Exp**)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp6FA)->f1).num_elts;_tmp71F=_tmp719.zero_term;
_tmp720=_tmp719.zt_loc;_LL4B1: {struct Cyc_Absyn_Exp*_tmp7C4=*_tmp71E;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp71A,0);_tmp71C->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp71C->print_const,_tmp71A);{int is_zero_terminated;{void*_tmp7C5=(void*)(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp71F))->v;int _tmp7C6;_LL50A: if((int)_tmp7C5 != 0)goto _LL50C;_LL50B:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp71F,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL509;
_LL50C: if(_tmp7C5 <= (void*)1  || *((int*)_tmp7C5)!= 0)goto _LL50E;_tmp7C6=(int)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp7C5)->f1;if(_tmp7C6 != 1)goto _LL50E;_LL50D:
if(!Cyc_Tcutil_admits_zero(_tmp71A))({struct Cyc_String_pa_struct _tmp7C9;_tmp7C9.tag=
0;_tmp7C9.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp71A));{void*_tmp7C7[1]={& _tmp7C9};Cyc_Tcutil_terr(loc,({const char*_tmp7C8="cannot have a zero-terminated array of %s elements";
_tag_arr(_tmp7C8,sizeof(char),_get_zero_arr_size(_tmp7C8,51));}),_tag_arr(
_tmp7C7,sizeof(void*),1));}});is_zero_terminated=1;goto _LL509;_LL50E:;_LL50F:
is_zero_terminated=0;goto _LL509;_LL509:;}if(_tmp7C4 == 0){if(is_zero_terminated)*
_tmp71E=(_tmp7C4=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{({void*
_tmp7CA[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp7CB="array bound defaults to 0 here";
_tag_arr(_tmp7CB,sizeof(char),_get_zero_arr_size(_tmp7CB,31));}),_tag_arr(
_tmp7CA,sizeof(void*),0));});*_tmp71E=(_tmp7C4=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
0,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp7C4);if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_tmp7C4))({void*_tmp7CC[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp7CD="array bounds expression is not constant";_tag_arr(_tmp7CD,
sizeof(char),_get_zero_arr_size(_tmp7CD,40));}),_tag_arr(_tmp7CC,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmp7C4))({void*_tmp7CE[
0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7CF="array bounds expression is not an unsigned int";
_tag_arr(_tmp7CF,sizeof(char),_get_zero_arr_size(_tmp7CF,47));}),_tag_arr(
_tmp7CE,sizeof(void*),0));});{unsigned int _tmp7D1;int _tmp7D2;struct _tuple7
_tmp7D0=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp7C4);_tmp7D1=
_tmp7D0.f1;_tmp7D2=_tmp7D0.f2;if((is_zero_terminated  && _tmp7D2) && _tmp7D1 < 1)({
void*_tmp7D3[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp7D4="zero terminated array cannot have zero elements";
_tag_arr(_tmp7D4,sizeof(char),_get_zero_arr_size(_tmp7D4,48));}),_tag_arr(
_tmp7D3,sizeof(void*),0));});goto _LL495;}}}_LL4B2: if(_tmp6FA <= (void*)4  || *((
int*)_tmp6FA)!= 8)goto _LL4B4;_tmp721=((struct Cyc_Absyn_FnType_struct*)_tmp6FA)->f1;
_tmp722=_tmp721.tvars;_tmp723=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp6FA)->f1).tvars;_tmp724=_tmp721.effect;_tmp725=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp6FA)->f1).effect;_tmp726=(void*)_tmp721.ret_typ;
_tmp727=_tmp721.args;_tmp728=_tmp721.c_varargs;_tmp729=_tmp721.cyc_varargs;
_tmp72A=_tmp721.rgn_po;_tmp72B=_tmp721.attributes;_LL4B3: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp72B != 0;_tmp72B=_tmp72B->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp72B->hd))({struct Cyc_String_pa_struct _tmp7D7;
_tmp7D7.tag=0;_tmp7D7.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp72B->hd));{void*_tmp7D5[1]={& _tmp7D7};Cyc_Tcutil_terr(loc,({const char*
_tmp7D6="bad function type attribute %s";_tag_arr(_tmp7D6,sizeof(char),
_get_zero_arr_size(_tmp7D6,31));}),_tag_arr(_tmp7D5,sizeof(void*),1));}});{void*
_tmp7D8=(void*)_tmp72B->hd;void*_tmp7D9;int _tmp7DA;int _tmp7DB;_LL511: if((int)
_tmp7D8 != 0)goto _LL513;_LL512: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto
_LL510;_LL513: if((int)_tmp7D8 != 1)goto _LL515;_LL514: if(!seen_cdecl){seen_cdecl=1;
++ num_convs;}goto _LL510;_LL515: if((int)_tmp7D8 != 2)goto _LL517;_LL516: if(!
seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL510;_LL517: if(_tmp7D8 <= (void*)
17  || *((int*)_tmp7D8)!= 3)goto _LL519;_tmp7D9=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp7D8)->f1;_tmp7DA=((struct Cyc_Absyn_Format_att_struct*)_tmp7D8)->f2;_tmp7DB=((
struct Cyc_Absyn_Format_att_struct*)_tmp7D8)->f3;_LL518: if(!seen_format){
seen_format=1;ft=_tmp7D9;fmt_desc_arg=_tmp7DA;fmt_arg_start=_tmp7DB;}else{({void*
_tmp7DC[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7DD="function can't have multiple format attributes";
_tag_arr(_tmp7DD,sizeof(char),_get_zero_arr_size(_tmp7DD,47));}),_tag_arr(
_tmp7DC,sizeof(void*),0));});}goto _LL510;_LL519:;_LL51A: goto _LL510;_LL510:;}}if(
num_convs > 1)({void*_tmp7DE[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7DF="function can't have multiple calling conventions";
_tag_arr(_tmp7DF,sizeof(char),_get_zero_arr_size(_tmp7DF,49));}),_tag_arr(
_tmp7DE,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*_tmp723);{struct
Cyc_List_List*b=*_tmp723;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=
Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp7E0=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp7E1;_LL51C: if(*((int*)_tmp7E0)!= 0)goto
_LL51E;_tmp7E1=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7E0)->f1;if((int)
_tmp7E1 != 1)goto _LL51E;_LL51D:({struct Cyc_String_pa_struct _tmp7E4;_tmp7E4.tag=0;
_tmp7E4.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);{
void*_tmp7E2[1]={& _tmp7E4};Cyc_Tcutil_terr(loc,({const char*_tmp7E3="function attempts to abstract Mem type variable %s";
_tag_arr(_tmp7E3,sizeof(char),_get_zero_arr_size(_tmp7E3,51));}),_tag_arr(
_tmp7E2,sizeof(void*),1));}});goto _LL51B;_LL51E:;_LL51F: goto _LL51B;_LL51B:;}}}{
struct Cyc_Tcutil_CVTEnv _tmp7E5=({struct Cyc_Tcutil_CVTEnv _tmp861;_tmp861.kind_env=
cvtenv.kind_env;_tmp861.free_vars=0;_tmp861.free_evars=0;_tmp861.generalize_evars=
cvtenv.generalize_evars;_tmp861.fn_result=1;_tmp861;});_tmp7E5=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7E5,(void*)1,_tmp726,1);_tmp7E5.fn_result=0;{struct Cyc_List_List*a=
_tmp727;for(0;a != 0;a=a->tl){struct _tuple2*_tmp7E6=(struct _tuple2*)a->hd;void*
_tmp7E7=(*_tmp7E6).f3;_tmp7E5=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7E5,(void*)
1,_tmp7E7,1);((*_tmp7E6).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp7E6).f2).print_const,_tmp7E7);}}if(_tmp729 != 0){if(_tmp728)({void*
_tmp7E8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp7E9="both c_vararg and cyc_vararg";_tag_arr(_tmp7E9,sizeof(char),
_get_zero_arr_size(_tmp7E9,29));}),_tag_arr(_tmp7E8,sizeof(void*),0));});{void*
_tmp7EB;int _tmp7EC;struct Cyc_Absyn_VarargInfo _tmp7EA=*_tmp729;_tmp7EB=(void*)
_tmp7EA.type;_tmp7EC=_tmp7EA.inject;_tmp7E5=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp7E5,(void*)1,_tmp7EB,1);(_tmp729->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp729->tq).print_const,_tmp7EB);if(_tmp7EC){void*_tmp7ED=Cyc_Tcutil_compress(
_tmp7EB);struct Cyc_Absyn_TunionInfo _tmp7EE;void*_tmp7EF;_LL521: if(_tmp7ED <= (
void*)4  || *((int*)_tmp7ED)!= 2)goto _LL523;_tmp7EE=((struct Cyc_Absyn_TunionType_struct*)
_tmp7ED)->f1;_tmp7EF=(void*)_tmp7EE.tunion_info;if(*((int*)_tmp7EF)!= 1)goto
_LL523;_LL522: goto _LL520;_LL523:;_LL524:({void*_tmp7F0[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp7F1="can't inject a non-[x]tunion type";_tag_arr(_tmp7F1,sizeof(
char),_get_zero_arr_size(_tmp7F1,34));}),_tag_arr(_tmp7F0,sizeof(void*),0));});
goto _LL520;_LL520:;}}}if(seen_format){int _tmp7F2=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp727);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp7F2) || 
fmt_arg_start < 0) || _tmp729 == 0  && fmt_arg_start != 0) || _tmp729 != 0  && 
fmt_arg_start != _tmp7F2 + 1)({void*_tmp7F3[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp7F4="bad format descriptor";_tag_arr(_tmp7F4,sizeof(char),_get_zero_arr_size(
_tmp7F4,22));}),_tag_arr(_tmp7F3,sizeof(void*),0));});else{void*_tmp7F6;struct
_tuple2 _tmp7F5=*((struct _tuple2*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmp727,fmt_desc_arg - 1);_tmp7F6=_tmp7F5.f3;{void*_tmp7F7=Cyc_Tcutil_compress(
_tmp7F6);struct Cyc_Absyn_PtrInfo _tmp7F8;void*_tmp7F9;struct Cyc_Absyn_PtrAtts
_tmp7FA;struct Cyc_Absyn_Conref*_tmp7FB;struct Cyc_Absyn_Conref*_tmp7FC;_LL526: if(
_tmp7F7 <= (void*)4  || *((int*)_tmp7F7)!= 4)goto _LL528;_tmp7F8=((struct Cyc_Absyn_PointerType_struct*)
_tmp7F7)->f1;_tmp7F9=(void*)_tmp7F8.elt_typ;_tmp7FA=_tmp7F8.ptr_atts;_tmp7FB=
_tmp7FA.bounds;_tmp7FC=_tmp7FA.zero_term;_LL527:{struct _tuple6 _tmp7FE=({struct
_tuple6 _tmp7FD;_tmp7FD.f1=Cyc_Tcutil_compress(_tmp7F9);_tmp7FD.f2=Cyc_Absyn_conref_def((
void*)((void*)0),_tmp7FB);_tmp7FD;});void*_tmp7FF;void*_tmp800;void*_tmp801;void*
_tmp802;_LL52B: _tmp7FF=_tmp7FE.f1;if(_tmp7FF <= (void*)4  || *((int*)_tmp7FF)!= 5)
goto _LL52D;_tmp800=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7FF)->f1;if((int)
_tmp800 != 2)goto _LL52D;_tmp801=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7FF)->f2;
if((int)_tmp801 != 0)goto _LL52D;_tmp802=_tmp7FE.f2;if((int)_tmp802 != 0)goto _LL52D;
_LL52C: goto _LL52A;_LL52D:;_LL52E:({void*_tmp803[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp804="format descriptor is not a char ? type";_tag_arr(_tmp804,
sizeof(char),_get_zero_arr_size(_tmp804,39));}),_tag_arr(_tmp803,sizeof(void*),0));});
goto _LL52A;_LL52A:;}goto _LL525;_LL528:;_LL529:({void*_tmp805[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp806="format descriptor is not a char ? type";_tag_arr(
_tmp806,sizeof(char),_get_zero_arr_size(_tmp806,39));}),_tag_arr(_tmp805,sizeof(
void*),0));});goto _LL525;_LL525:;}if(fmt_arg_start != 0){int problem;{struct
_tuple6 _tmp808=({struct _tuple6 _tmp807;_tmp807.f1=ft;_tmp807.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp729))->type);_tmp807;});void*
_tmp809;void*_tmp80A;struct Cyc_Absyn_TunionInfo _tmp80B;void*_tmp80C;struct Cyc_Absyn_Tuniondecl**
_tmp80D;struct Cyc_Absyn_Tuniondecl*_tmp80E;void*_tmp80F;void*_tmp810;struct Cyc_Absyn_TunionInfo
_tmp811;void*_tmp812;struct Cyc_Absyn_Tuniondecl**_tmp813;struct Cyc_Absyn_Tuniondecl*
_tmp814;_LL530: _tmp809=_tmp808.f1;if((int)_tmp809 != 0)goto _LL532;_tmp80A=_tmp808.f2;
if(_tmp80A <= (void*)4  || *((int*)_tmp80A)!= 2)goto _LL532;_tmp80B=((struct Cyc_Absyn_TunionType_struct*)
_tmp80A)->f1;_tmp80C=(void*)_tmp80B.tunion_info;if(*((int*)_tmp80C)!= 1)goto
_LL532;_tmp80D=((struct Cyc_Absyn_KnownTunion_struct*)_tmp80C)->f1;_tmp80E=*
_tmp80D;_LL531: problem=Cyc_Absyn_qvar_cmp(_tmp80E->name,Cyc_Absyn_tunion_print_arg_qvar)
!= 0;goto _LL52F;_LL532: _tmp80F=_tmp808.f1;if((int)_tmp80F != 1)goto _LL534;_tmp810=
_tmp808.f2;if(_tmp810 <= (void*)4  || *((int*)_tmp810)!= 2)goto _LL534;_tmp811=((
struct Cyc_Absyn_TunionType_struct*)_tmp810)->f1;_tmp812=(void*)_tmp811.tunion_info;
if(*((int*)_tmp812)!= 1)goto _LL534;_tmp813=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp812)->f1;_tmp814=*_tmp813;_LL533: problem=Cyc_Absyn_qvar_cmp(_tmp814->name,
Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL52F;_LL534:;_LL535: problem=1;goto
_LL52F;_LL52F:;}if(problem)({void*_tmp815[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp816="format attribute and vararg types don't match";_tag_arr(_tmp816,sizeof(
char),_get_zero_arr_size(_tmp816,46));}),_tag_arr(_tmp815,sizeof(void*),0));});}}}{
struct Cyc_List_List*rpo=_tmp72A;for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp818;
void*_tmp819;void*_tmp81A;struct _tuple6*_tmp817=(struct _tuple6*)rpo->hd;_tmp818=*
_tmp817;_tmp819=_tmp818.f1;_tmp81A=_tmp818.f2;_tmp7E5=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7E5,(void*)4,_tmp819,0);_tmp7E5=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp7E5,(void*)3,_tmp81A,0);}}if(*_tmp725 != 0)_tmp7E5=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7E5,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp725))->v,0);
else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp7E5.free_vars;
for(0;tvs != 0;tvs=tvs->tl){void*_tmp81B=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmp81C;struct Cyc_Core_Opt**_tmp81D;void*
_tmp81E;void*_tmp81F;void*_tmp820;void*_tmp821;struct Cyc_Core_Opt*_tmp822;struct
Cyc_Core_Opt**_tmp823;void*_tmp824;void*_tmp825;struct Cyc_Core_Opt*_tmp826;
struct Cyc_Core_Opt**_tmp827;_LL537: if(*((int*)_tmp81B)!= 2)goto _LL539;_tmp81C=((
struct Cyc_Absyn_Less_kb_struct*)_tmp81B)->f1;_tmp81D=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp81B)->f1;_tmp81E=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp81B)->f2;if((int)_tmp81E != 3)goto _LL539;_LL538:*_tmp81D=({struct Cyc_Core_Opt*
_tmp828=_cycalloc(sizeof(*_tmp828));_tmp828->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp829=_cycalloc(sizeof(*_tmp829));_tmp829[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp82A;_tmp82A.tag=0;_tmp82A.f1=(void*)((void*)3);_tmp82A;});_tmp829;}));
_tmp828;});goto _LL53A;_LL539: if(*((int*)_tmp81B)!= 0)goto _LL53B;_tmp81F=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp81B)->f1;if((int)_tmp81F != 3)goto _LL53B;_LL53A:
effect=({struct Cyc_List_List*_tmp82B=_cycalloc(sizeof(*_tmp82B));_tmp82B->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp82C=_cycalloc(sizeof(*
_tmp82C));_tmp82C[0]=({struct Cyc_Absyn_AccessEff_struct _tmp82D;_tmp82D.tag=19;
_tmp82D.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp82E=_cycalloc(
sizeof(*_tmp82E));_tmp82E[0]=({struct Cyc_Absyn_VarType_struct _tmp82F;_tmp82F.tag=
1;_tmp82F.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp82F;});_tmp82E;}));_tmp82D;});
_tmp82C;}));_tmp82B->tl=effect;_tmp82B;});goto _LL536;_LL53B: if(*((int*)_tmp81B)
!= 2)goto _LL53D;_tmp820=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp81B)->f2;
if((int)_tmp820 != 5)goto _LL53D;_LL53C: goto _LL53E;_LL53D: if(*((int*)_tmp81B)!= 0)
goto _LL53F;_tmp821=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp81B)->f1;if((int)
_tmp821 != 5)goto _LL53F;_LL53E: goto _LL536;_LL53F: if(*((int*)_tmp81B)!= 2)goto
_LL541;_tmp822=((struct Cyc_Absyn_Less_kb_struct*)_tmp81B)->f1;_tmp823=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp81B)->f1;_tmp824=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp81B)->f2;if((int)_tmp824 != 4)goto _LL541;_LL540:*_tmp823=({struct Cyc_Core_Opt*
_tmp830=_cycalloc(sizeof(*_tmp830));_tmp830->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp831=_cycalloc(sizeof(*_tmp831));_tmp831[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp832;_tmp832.tag=0;_tmp832.f1=(void*)((void*)4);_tmp832;});_tmp831;}));
_tmp830;});goto _LL542;_LL541: if(*((int*)_tmp81B)!= 0)goto _LL543;_tmp825=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp81B)->f1;if((int)_tmp825 != 4)goto _LL543;_LL542:
effect=({struct Cyc_List_List*_tmp833=_cycalloc(sizeof(*_tmp833));_tmp833->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp834=_cycalloc(sizeof(*_tmp834));
_tmp834[0]=({struct Cyc_Absyn_VarType_struct _tmp835;_tmp835.tag=1;_tmp835.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp835;});_tmp834;}));_tmp833->tl=effect;_tmp833;});
goto _LL536;_LL543: if(*((int*)_tmp81B)!= 1)goto _LL545;_tmp826=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp81B)->f1;_tmp827=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp81B)->f1;_LL544:*_tmp827=({struct Cyc_Core_Opt*_tmp836=_cycalloc(sizeof(*
_tmp836));_tmp836->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp837=
_cycalloc(sizeof(*_tmp837));_tmp837[0]=({struct Cyc_Absyn_Less_kb_struct _tmp838;
_tmp838.tag=2;_tmp838.f1=0;_tmp838.f2=(void*)((void*)0);_tmp838;});_tmp837;}));
_tmp836;});goto _LL546;_LL545:;_LL546: effect=({struct Cyc_List_List*_tmp839=
_cycalloc(sizeof(*_tmp839));_tmp839->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp83A=_cycalloc(sizeof(*_tmp83A));_tmp83A[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp83B;_tmp83B.tag=21;_tmp83B.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp83C=_cycalloc(sizeof(*_tmp83C));_tmp83C[0]=({struct Cyc_Absyn_VarType_struct
_tmp83D;_tmp83D.tag=1;_tmp83D.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp83D;});
_tmp83C;}));_tmp83B;});_tmp83A;}));_tmp839->tl=effect;_tmp839;});goto _LL536;
_LL536:;}}{struct Cyc_List_List*ts=_tmp7E5.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmp83E=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL548: if((int)_tmp83E != 3)goto
_LL54A;_LL549: effect=({struct Cyc_List_List*_tmp83F=_cycalloc(sizeof(*_tmp83F));
_tmp83F->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp840=_cycalloc(
sizeof(*_tmp840));_tmp840[0]=({struct Cyc_Absyn_AccessEff_struct _tmp841;_tmp841.tag=
19;_tmp841.f1=(void*)((void*)ts->hd);_tmp841;});_tmp840;}));_tmp83F->tl=effect;
_tmp83F;});goto _LL547;_LL54A: if((int)_tmp83E != 4)goto _LL54C;_LL54B: effect=({
struct Cyc_List_List*_tmp842=_cycalloc(sizeof(*_tmp842));_tmp842->hd=(void*)((
void*)ts->hd);_tmp842->tl=effect;_tmp842;});goto _LL547;_LL54C: if((int)_tmp83E != 
5)goto _LL54E;_LL54D: goto _LL547;_LL54E:;_LL54F: effect=({struct Cyc_List_List*
_tmp843=_cycalloc(sizeof(*_tmp843));_tmp843->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp844=_cycalloc(sizeof(*_tmp844));_tmp844[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp845;_tmp845.tag=21;_tmp845.f1=(void*)((void*)ts->hd);_tmp845;});_tmp844;}));
_tmp843->tl=effect;_tmp843;});goto _LL547;_LL547:;}}*_tmp725=({struct Cyc_Core_Opt*
_tmp846=_cycalloc(sizeof(*_tmp846));_tmp846->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp847=_cycalloc(sizeof(*_tmp847));_tmp847[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp848;_tmp848.tag=20;_tmp848.f1=effect;_tmp848;});_tmp847;}));_tmp846;});}if(*
_tmp723 != 0){struct Cyc_List_List*bs=*_tmp723;for(0;bs != 0;bs=bs->tl){void*
_tmp849=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp84A;struct Cyc_Core_Opt**_tmp84B;struct Cyc_Core_Opt*
_tmp84C;struct Cyc_Core_Opt**_tmp84D;void*_tmp84E;struct Cyc_Core_Opt*_tmp84F;
struct Cyc_Core_Opt**_tmp850;void*_tmp851;struct Cyc_Core_Opt*_tmp852;struct Cyc_Core_Opt**
_tmp853;void*_tmp854;void*_tmp855;_LL551: if(*((int*)_tmp849)!= 1)goto _LL553;
_tmp84A=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp849)->f1;_tmp84B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp849)->f1;_LL552:({struct Cyc_String_pa_struct
_tmp858;_tmp858.tag=0;_tmp858.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)bs->hd)->name);{void*_tmp856[1]={& _tmp858};Cyc_Tcutil_warn(
loc,({const char*_tmp857="Type variable %s unconstrained, assuming boxed";
_tag_arr(_tmp857,sizeof(char),_get_zero_arr_size(_tmp857,47));}),_tag_arr(
_tmp856,sizeof(void*),1));}});_tmp84D=_tmp84B;goto _LL554;_LL553: if(*((int*)
_tmp849)!= 2)goto _LL555;_tmp84C=((struct Cyc_Absyn_Less_kb_struct*)_tmp849)->f1;
_tmp84D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp849)->f1;
_tmp84E=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp849)->f2;if((int)_tmp84E != 
1)goto _LL555;_LL554: _tmp850=_tmp84D;goto _LL556;_LL555: if(*((int*)_tmp849)!= 2)
goto _LL557;_tmp84F=((struct Cyc_Absyn_Less_kb_struct*)_tmp849)->f1;_tmp850=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp849)->f1;_tmp851=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp849)->f2;if((int)_tmp851 != 0)goto
_LL557;_LL556:*_tmp850=({struct Cyc_Core_Opt*_tmp859=_cycalloc(sizeof(*_tmp859));
_tmp859->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp85A=_cycalloc(
sizeof(*_tmp85A));_tmp85A[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp85B;_tmp85B.tag=0;
_tmp85B.f1=(void*)((void*)2);_tmp85B;});_tmp85A;}));_tmp859;});goto _LL550;_LL557:
if(*((int*)_tmp849)!= 2)goto _LL559;_tmp852=((struct Cyc_Absyn_Less_kb_struct*)
_tmp849)->f1;_tmp853=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp849)->f1;_tmp854=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp849)->f2;
_LL558:*_tmp853=({struct Cyc_Core_Opt*_tmp85C=_cycalloc(sizeof(*_tmp85C));_tmp85C->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp85D=_cycalloc(sizeof(*_tmp85D));
_tmp85D[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp85E;_tmp85E.tag=0;_tmp85E.f1=(void*)
_tmp854;_tmp85E;});_tmp85D;}));_tmp85C;});goto _LL550;_LL559: if(*((int*)_tmp849)
!= 0)goto _LL55B;_tmp855=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp849)->f1;if((
int)_tmp855 != 1)goto _LL55B;_LL55A:({void*_tmp85F[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp860="functions can't abstract M types";_tag_arr(_tmp860,sizeof(
char),_get_zero_arr_size(_tmp860,33));}),_tag_arr(_tmp85F,sizeof(void*),0));});
goto _LL550;_LL55B:;_LL55C: goto _LL550;_LL550:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmp7E5.kind_env,*_tmp723);_tmp7E5.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp7E5.free_vars,*_tmp723);{struct Cyc_List_List*tvs=_tmp7E5.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp7E5.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL495;}}_LL4B4: if(_tmp6FA <= (void*)4  || *((int*)_tmp6FA)!= 
9)goto _LL4B6;_tmp72C=((struct Cyc_Absyn_TupleType_struct*)_tmp6FA)->f1;_LL4B5:
for(0;_tmp72C != 0;_tmp72C=_tmp72C->tl){struct _tuple4*_tmp862=(struct _tuple4*)
_tmp72C->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,(*
_tmp862).f2,0);((*_tmp862).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp862).f1).print_const,(*_tmp862).f2);}goto _LL495;_LL4B6: if(_tmp6FA <= (
void*)4  || *((int*)_tmp6FA)!= 11)goto _LL4B8;_tmp72D=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6FA)->f1;_tmp72E=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp6FA)->f2;_LL4B7:{
struct _RegionHandle _tmp863=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp863;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp72E != 0;_tmp72E=_tmp72E->tl){struct Cyc_Absyn_Aggrfield _tmp865;struct
_tagged_arr*_tmp866;struct Cyc_Absyn_Tqual _tmp867;struct Cyc_Absyn_Tqual*_tmp868;
void*_tmp869;struct Cyc_Absyn_Exp*_tmp86A;struct Cyc_List_List*_tmp86B;struct Cyc_Absyn_Aggrfield*
_tmp864=(struct Cyc_Absyn_Aggrfield*)_tmp72E->hd;_tmp865=*_tmp864;_tmp866=_tmp865.name;
_tmp867=_tmp865.tq;_tmp868=(struct Cyc_Absyn_Tqual*)&(*_tmp864).tq;_tmp869=(void*)
_tmp865.type;_tmp86A=_tmp865.width;_tmp86B=_tmp865.attributes;if(((int(*)(int(*
compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct
_tagged_arr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp866))({struct Cyc_String_pa_struct
_tmp86E;_tmp86E.tag=0;_tmp86E.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp866);{
void*_tmp86C[1]={& _tmp86E};Cyc_Tcutil_terr(loc,({const char*_tmp86D="duplicate field %s";
_tag_arr(_tmp86D,sizeof(char),_get_zero_arr_size(_tmp86D,19));}),_tag_arr(
_tmp86C,sizeof(void*),1));}});if(Cyc_strcmp((struct _tagged_arr)*_tmp866,({const
char*_tmp86F="";_tag_arr(_tmp86F,sizeof(char),_get_zero_arr_size(_tmp86F,1));}))
!= 0)prev_fields=({struct Cyc_List_List*_tmp870=_region_malloc(aprev_rgn,sizeof(*
_tmp870));_tmp870->hd=_tmp866;_tmp870->tl=prev_fields;_tmp870;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp869,0);_tmp868->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp868->print_const,_tmp869);if(_tmp72D == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp869))({struct Cyc_String_pa_struct _tmp873;_tmp873.tag=0;_tmp873.f1=(struct
_tagged_arr)((struct _tagged_arr)*_tmp866);{void*_tmp871[1]={& _tmp873};Cyc_Tcutil_warn(
loc,({const char*_tmp872="union member %s is not `bits-only' so it can only be written and not read";
_tag_arr(_tmp872,sizeof(char),_get_zero_arr_size(_tmp872,74));}),_tag_arr(
_tmp871,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(loc,te,_tmp869,_tmp86A,
_tmp866);Cyc_Tcutil_check_field_atts(loc,_tmp866,_tmp86B);}};_pop_region(
aprev_rgn);}goto _LL495;_LL4B8: if(_tmp6FA <= (void*)4  || *((int*)_tmp6FA)!= 10)
goto _LL4BA;_tmp72F=((struct Cyc_Absyn_AggrType_struct*)_tmp6FA)->f1;_tmp730=(void*)
_tmp72F.aggr_info;_tmp731=(void**)&(((struct Cyc_Absyn_AggrType_struct*)_tmp6FA)->f1).aggr_info;
_tmp732=_tmp72F.targs;_tmp733=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp6FA)->f1).targs;_LL4B9:{void*_tmp874=*_tmp731;void*_tmp875;struct _tuple1*
_tmp876;struct Cyc_Absyn_Aggrdecl**_tmp877;struct Cyc_Absyn_Aggrdecl*_tmp878;
_LL55E: if(*((int*)_tmp874)!= 0)goto _LL560;_tmp875=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp874)->f1;_tmp876=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp874)->f2;_LL55F: {
struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp879;_push_handler(& _tmp879);{
int _tmp87B=0;if(setjmp(_tmp879.handler))_tmp87B=1;if(!_tmp87B){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp876);*_tmp731=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp87C=
_cycalloc(sizeof(*_tmp87C));_tmp87C[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp87D;
_tmp87D.tag=1;_tmp87D.f1=adp;_tmp87D;});_tmp87C;});;_pop_handler();}else{void*
_tmp87A=(void*)_exn_thrown;void*_tmp87F=_tmp87A;_LL563: if(_tmp87F != Cyc_Dict_Absent)
goto _LL565;_LL564: {struct Cyc_Tcenv_Genv*_tmp880=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp881=({struct Cyc_Absyn_Aggrdecl*_tmp887=_cycalloc(
sizeof(*_tmp887));_tmp887->kind=(void*)_tmp875;_tmp887->sc=(void*)((void*)3);
_tmp887->name=_tmp876;_tmp887->tvs=0;_tmp887->impl=0;_tmp887->attributes=0;
_tmp887;});Cyc_Tc_tcAggrdecl(te,_tmp880,loc,_tmp881);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp876);*_tmp731=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp882=
_cycalloc(sizeof(*_tmp882));_tmp882[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp883;
_tmp883.tag=1;_tmp883.f1=adp;_tmp883;});_tmp882;});if(*_tmp733 != 0){({struct Cyc_String_pa_struct
_tmp886;_tmp886.tag=0;_tmp886.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp876));{void*_tmp884[1]={& _tmp886};Cyc_Tcutil_terr(loc,({const char*_tmp885="declare parameterized type %s before using";
_tag_arr(_tmp885,sizeof(char),_get_zero_arr_size(_tmp885,43));}),_tag_arr(
_tmp884,sizeof(void*),1));}});return cvtenv;}goto _LL562;}_LL565:;_LL566:(void)
_throw(_tmp87F);_LL562:;}}}_tmp878=*adp;goto _LL561;}_LL560: if(*((int*)_tmp874)!= 
1)goto _LL55D;_tmp877=((struct Cyc_Absyn_KnownAggr_struct*)_tmp874)->f1;_tmp878=*
_tmp877;_LL561: {struct Cyc_List_List*tvs=_tmp878->tvs;struct Cyc_List_List*ts=*
_tmp733;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,0);}if(ts != 0)({struct Cyc_String_pa_struct _tmp88A;_tmp88A.tag=
0;_tmp88A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp878->name));{void*_tmp888[1]={& _tmp88A};Cyc_Tcutil_terr(loc,({const char*
_tmp889="too many parameters for type %s";_tag_arr(_tmp889,sizeof(char),
_get_zero_arr_size(_tmp889,32));}),_tag_arr(_tmp888,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp88B=_cycalloc(sizeof(*_tmp88B));_tmp88B->hd=(void*)e;_tmp88B->tl=
hidden_ts;_tmp88B;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,0);}*
_tmp733=Cyc_List_imp_append(*_tmp733,Cyc_List_imp_rev(hidden_ts));}}_LL55D:;}
goto _LL495;_LL4BA: if(_tmp6FA <= (void*)4  || *((int*)_tmp6FA)!= 16)goto _LL4BC;
_tmp734=((struct Cyc_Absyn_TypedefType_struct*)_tmp6FA)->f1;_tmp735=((struct Cyc_Absyn_TypedefType_struct*)
_tmp6FA)->f2;_tmp736=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp6FA)->f2;_tmp737=((struct Cyc_Absyn_TypedefType_struct*)_tmp6FA)->f3;_tmp738=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp6FA)->f3;
_tmp739=((struct Cyc_Absyn_TypedefType_struct*)_tmp6FA)->f4;_tmp73A=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp6FA)->f4;_LL4BB: {struct Cyc_List_List*
targs=*_tmp736;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp88C;
_push_handler(& _tmp88C);{int _tmp88E=0;if(setjmp(_tmp88C.handler))_tmp88E=1;if(!
_tmp88E){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp734);;_pop_handler();}else{
void*_tmp88D=(void*)_exn_thrown;void*_tmp890=_tmp88D;_LL568: if(_tmp890 != Cyc_Dict_Absent)
goto _LL56A;_LL569:({struct Cyc_String_pa_struct _tmp893;_tmp893.tag=0;_tmp893.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp734));{void*
_tmp891[1]={& _tmp893};Cyc_Tcutil_terr(loc,({const char*_tmp892="unbound typedef name %s";
_tag_arr(_tmp892,sizeof(char),_get_zero_arr_size(_tmp892,24));}),_tag_arr(
_tmp891,sizeof(void*),1));}});return cvtenv;_LL56A:;_LL56B:(void)_throw(_tmp890);
_LL567:;}}}*_tmp738=(struct Cyc_Absyn_Typedefdecl*)td;_tmp734[0]=(td->name)[
_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*
ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,0);inst=({struct Cyc_List_List*_tmp894=_cycalloc(
sizeof(*_tmp894));_tmp894->hd=({struct _tuple8*_tmp895=_cycalloc(sizeof(*_tmp895));
_tmp895->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp895->f2=(void*)ts->hd;_tmp895;});
_tmp894->tl=inst;_tmp894;});}if(ts != 0)({struct Cyc_String_pa_struct _tmp898;
_tmp898.tag=0;_tmp898.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp734));{void*_tmp896[1]={& _tmp898};Cyc_Tcutil_terr(loc,({const char*_tmp897="too many parameters for typedef %s";
_tag_arr(_tmp897,sizeof(char),_get_zero_arr_size(_tmp897,35));}),_tag_arr(
_tmp896,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;
tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp899=_cycalloc(
sizeof(*_tmp899));_tmp899->hd=(void*)e;_tmp899->tl=hidden_ts;_tmp899;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,0);inst=({struct Cyc_List_List*
_tmp89A=_cycalloc(sizeof(*_tmp89A));_tmp89A->hd=({struct _tuple8*_tmp89B=
_cycalloc(sizeof(*_tmp89B));_tmp89B->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp89B->f2=
e;_tmp89B;});_tmp89A->tl=inst;_tmp89A;});}*_tmp736=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp73A=({void**_tmp89C=
_cycalloc(sizeof(*_tmp89C));_tmp89C[0]=new_typ;_tmp89C;});}goto _LL495;}}_LL4BC:
if((int)_tmp6FA != 3)goto _LL4BE;_LL4BD: if(!allow_unique)({void*_tmp89D[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp89E="`U only allowed in outermost pointers";_tag_arr(_tmp89E,
sizeof(char),_get_zero_arr_size(_tmp89E,38));}),_tag_arr(_tmp89D,sizeof(void*),0));});
goto _LL495;_LL4BE: if(_tmp6FA <= (void*)4  || *((int*)_tmp6FA)!= 18)goto _LL4C0;
_LL4BF: goto _LL4C1;_LL4C0: if((int)_tmp6FA != 2)goto _LL4C2;_LL4C1: goto _LL495;_LL4C2:
if(_tmp6FA <= (void*)4  || *((int*)_tmp6FA)!= 15)goto _LL4C4;_tmp73B=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp6FA)->f1;_LL4C3: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp73B,1);goto _LL495;_LL4C4: if(_tmp6FA <= (void*)4  || *((
int*)_tmp6FA)!= 19)goto _LL4C6;_tmp73C=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp6FA)->f1;_LL4C5: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp73C,0);goto _LL495;_LL4C6: if(_tmp6FA <= (void*)4  || *((int*)_tmp6FA)!= 21)goto
_LL4C8;_tmp73D=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp6FA)->f1;_LL4C7:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp73D,0);goto
_LL495;_LL4C8: if(_tmp6FA <= (void*)4  || *((int*)_tmp6FA)!= 20)goto _LL495;_tmp73E=((
struct Cyc_Absyn_JoinEff_struct*)_tmp6FA)->f1;_LL4C9: for(0;_tmp73E != 0;_tmp73E=
_tmp73E->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)4,(void*)
_tmp73E->hd,0);}goto _LL495;_LL495:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t),expected_kind))({struct Cyc_String_pa_struct _tmp8A3;_tmp8A3.tag=0;_tmp8A3.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(expected_kind));{
struct Cyc_String_pa_struct _tmp8A2;_tmp8A2.tag=0;_tmp8A2.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));{struct Cyc_String_pa_struct
_tmp8A1;_tmp8A1.tag=0;_tmp8A1.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp89F[3]={& _tmp8A1,& _tmp8A2,& _tmp8A3};Cyc_Tcutil_terr(loc,({const
char*_tmp8A0="type %s has kind %s but as used here needs kind %s";_tag_arr(
_tmp8A0,sizeof(char),_get_zero_arr_size(_tmp8A0,51));}),_tag_arr(_tmp89F,sizeof(
void*),3));}}}});return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
kind_env,void*expected_kind,int generalize_evars,void*t,int allow_unique){struct
Cyc_Tcutil_CVTEnv _tmp8A4=Cyc_Tcutil_i_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv
_tmp8AD;_tmp8AD.kind_env=kind_env;_tmp8AD.free_vars=0;_tmp8AD.free_evars=0;
_tmp8AD.generalize_evars=generalize_evars;_tmp8AD.fn_result=0;_tmp8AD;}),
expected_kind,t,allow_unique);{struct Cyc_List_List*vs=_tmp8A4.free_vars;for(0;vs
!= 0;vs=vs->tl){_tmp8A4.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(struct
Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp8A4.free_evars;for(0;evs
!= 0;evs=evs->tl){void*_tmp8A5=Cyc_Tcutil_compress((void*)evs->hd);struct Cyc_Core_Opt*
_tmp8A6;struct Cyc_Core_Opt**_tmp8A7;_LL56D: if(_tmp8A5 <= (void*)4  || *((int*)
_tmp8A5)!= 0)goto _LL56F;_tmp8A6=((struct Cyc_Absyn_Evar_struct*)_tmp8A5)->f4;
_tmp8A7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp8A5)->f4;
_LL56E: if(*_tmp8A7 == 0)*_tmp8A7=({struct Cyc_Core_Opt*_tmp8A8=_cycalloc(sizeof(*
_tmp8A8));_tmp8A8->v=kind_env;_tmp8A8;});else{struct Cyc_List_List*_tmp8A9=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp8A7))->v;struct Cyc_List_List*
_tmp8AA=0;for(0;_tmp8A9 != 0;_tmp8A9=_tmp8A9->tl){if(((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)
_tmp8A9->hd))_tmp8AA=({struct Cyc_List_List*_tmp8AB=_cycalloc(sizeof(*_tmp8AB));
_tmp8AB->hd=(struct Cyc_Absyn_Tvar*)_tmp8A9->hd;_tmp8AB->tl=_tmp8AA;_tmp8AB;});}*
_tmp8A7=({struct Cyc_Core_Opt*_tmp8AC=_cycalloc(sizeof(*_tmp8AC));_tmp8AC->v=
_tmp8AA;_tmp8AC;});}goto _LL56C;_LL56F:;_LL570: goto _LL56C;_LL56C:;}}return _tmp8A4;}
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Tenv*te,void*t,int allow_unique){int generalize_evars=Cyc_Tcutil_is_function_type(
t);struct Cyc_Tcutil_CVTEnv _tmp8AE=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t,allow_unique);struct Cyc_List_List*_tmp8AF=_tmp8AE.free_vars;
struct Cyc_List_List*_tmp8B0=_tmp8AE.free_evars;{struct Cyc_List_List*x=_tmp8AF;
for(0;x != 0;x=x->tl){void*_tmp8B1=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
x->hd)->kind);struct Cyc_Core_Opt*_tmp8B2;struct Cyc_Core_Opt**_tmp8B3;struct Cyc_Core_Opt*
_tmp8B4;struct Cyc_Core_Opt**_tmp8B5;void*_tmp8B6;struct Cyc_Core_Opt*_tmp8B7;
struct Cyc_Core_Opt**_tmp8B8;void*_tmp8B9;struct Cyc_Core_Opt*_tmp8BA;struct Cyc_Core_Opt**
_tmp8BB;void*_tmp8BC;void*_tmp8BD;_LL572: if(*((int*)_tmp8B1)!= 1)goto _LL574;
_tmp8B2=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B1)->f1;_tmp8B3=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B1)->f1;_LL573: _tmp8B5=_tmp8B3;goto
_LL575;_LL574: if(*((int*)_tmp8B1)!= 2)goto _LL576;_tmp8B4=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8B1)->f1;_tmp8B5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8B1)->f1;_tmp8B6=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B1)->f2;if((
int)_tmp8B6 != 1)goto _LL576;_LL575: _tmp8B8=_tmp8B5;goto _LL577;_LL576: if(*((int*)
_tmp8B1)!= 2)goto _LL578;_tmp8B7=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B1)->f1;
_tmp8B8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8B1)->f1;
_tmp8B9=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B1)->f2;if((int)_tmp8B9 != 
0)goto _LL578;_LL577:*_tmp8B8=({struct Cyc_Core_Opt*_tmp8BE=_cycalloc(sizeof(*
_tmp8BE));_tmp8BE->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8BF=
_cycalloc(sizeof(*_tmp8BF));_tmp8BF[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8C0;
_tmp8C0.tag=0;_tmp8C0.f1=(void*)((void*)2);_tmp8C0;});_tmp8BF;}));_tmp8BE;});
goto _LL571;_LL578: if(*((int*)_tmp8B1)!= 2)goto _LL57A;_tmp8BA=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8B1)->f1;_tmp8BB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8B1)->f1;_tmp8BC=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B1)->f2;
_LL579:*_tmp8BB=({struct Cyc_Core_Opt*_tmp8C1=_cycalloc(sizeof(*_tmp8C1));_tmp8C1->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8C2=_cycalloc(sizeof(*_tmp8C2));
_tmp8C2[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8C3;_tmp8C3.tag=0;_tmp8C3.f1=(void*)
_tmp8BC;_tmp8C3;});_tmp8C2;}));_tmp8C1;});goto _LL571;_LL57A: if(*((int*)_tmp8B1)
!= 0)goto _LL57C;_tmp8BD=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8B1)->f1;if((
int)_tmp8BD != 1)goto _LL57C;_LL57B:({struct Cyc_String_pa_struct _tmp8C6;_tmp8C6.tag=
0;_tmp8C6.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp8C4[1]={& _tmp8C6};Cyc_Tcutil_terr(loc,({
const char*_tmp8C5="type variable %s cannot have kind M";_tag_arr(_tmp8C5,sizeof(
char),_get_zero_arr_size(_tmp8C5,36));}),_tag_arr(_tmp8C4,sizeof(void*),1));}});
goto _LL571;_LL57C:;_LL57D: goto _LL571;_LL571:;}}if(_tmp8AF != 0  || _tmp8B0 != 0){{
void*_tmp8C7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp8C8;struct Cyc_List_List*
_tmp8C9;struct Cyc_List_List**_tmp8CA;struct Cyc_Core_Opt*_tmp8CB;void*_tmp8CC;
struct Cyc_List_List*_tmp8CD;int _tmp8CE;struct Cyc_Absyn_VarargInfo*_tmp8CF;struct
Cyc_List_List*_tmp8D0;struct Cyc_List_List*_tmp8D1;_LL57F: if(_tmp8C7 <= (void*)4
 || *((int*)_tmp8C7)!= 8)goto _LL581;_tmp8C8=((struct Cyc_Absyn_FnType_struct*)
_tmp8C7)->f1;_tmp8C9=_tmp8C8.tvars;_tmp8CA=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp8C7)->f1).tvars;_tmp8CB=_tmp8C8.effect;_tmp8CC=(void*)_tmp8C8.ret_typ;
_tmp8CD=_tmp8C8.args;_tmp8CE=_tmp8C8.c_varargs;_tmp8CF=_tmp8C8.cyc_varargs;
_tmp8D0=_tmp8C8.rgn_po;_tmp8D1=_tmp8C8.attributes;_LL580: if(*_tmp8CA == 0){*
_tmp8CA=_tmp8AF;_tmp8AF=0;}goto _LL57E;_LL581:;_LL582: goto _LL57E;_LL57E:;}if(
_tmp8AF != 0)({struct Cyc_String_pa_struct _tmp8D4;_tmp8D4.tag=0;_tmp8D4.f1=(struct
_tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)_tmp8AF->hd)->name);{
void*_tmp8D2[1]={& _tmp8D4};Cyc_Tcutil_terr(loc,({const char*_tmp8D3="unbound type variable %s";
_tag_arr(_tmp8D3,sizeof(char),_get_zero_arr_size(_tmp8D3,25));}),_tag_arr(
_tmp8D2,sizeof(void*),1));}});if(_tmp8B0 != 0)for(0;_tmp8B0 != 0;_tmp8B0=_tmp8B0->tl){
void*e=(void*)_tmp8B0->hd;void*_tmp8D5=Cyc_Tcutil_typ_kind(e);_LL584: if((int)
_tmp8D5 != 3)goto _LL586;_LL585: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp8D6[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp8D7="can't unify evar with heap!";_tag_arr(_tmp8D7,sizeof(char),
_get_zero_arr_size(_tmp8D7,28));}),_tag_arr(_tmp8D6,sizeof(void*),0));});goto
_LL583;_LL586: if((int)_tmp8D5 != 4)goto _LL588;_LL587: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({
void*_tmp8D8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp8D9="can't unify evar with {}!";_tag_arr(_tmp8D9,sizeof(char),
_get_zero_arr_size(_tmp8D9,26));}),_tag_arr(_tmp8D8,sizeof(void*),0));});goto
_LL583;_LL588:;_LL589:({struct Cyc_String_pa_struct _tmp8DD;_tmp8DD.tag=0;_tmp8DD.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp8DC;_tmp8DC.tag=0;_tmp8DC.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
e));{void*_tmp8DA[2]={& _tmp8DC,& _tmp8DD};Cyc_Tcutil_terr(loc,({const char*_tmp8DB="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp8DB,sizeof(char),_get_zero_arr_size(_tmp8DB,52));}),_tag_arr(
_tmp8DA,sizeof(void*),2));}}});goto _LL583;_LL583:;}}}void Cyc_Tcutil_check_no_unique_region(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp8DE;_tmp8DE.kind_env=0;_tmp8DE.free_vars=0;
_tmp8DE.free_evars=0;_tmp8DE.generalize_evars=0;_tmp8DE.fn_result=0;_tmp8DE;}),(
void*)1,t,0);}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t,1);{void*_tmp8DF=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp8E0;struct Cyc_List_List*_tmp8E1;struct Cyc_Core_Opt*
_tmp8E2;_LL58B: if(_tmp8DF <= (void*)4  || *((int*)_tmp8DF)!= 8)goto _LL58D;_tmp8E0=((
struct Cyc_Absyn_FnType_struct*)_tmp8DF)->f1;_tmp8E1=_tmp8E0.tvars;_tmp8E2=
_tmp8E0.effect;_LL58C: fd->tvs=_tmp8E1;fd->effect=_tmp8E2;goto _LL58A;_LL58D:;
_LL58E:({void*_tmp8E3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp8E4="check_fndecl_valid_type: not a FnType";
_tag_arr(_tmp8E4,sizeof(char),_get_zero_arr_size(_tmp8E4,38));}),_tag_arr(
_tmp8E3,sizeof(void*),0));});_LL58A:;}{struct _RegionHandle _tmp8E5=_new_region("r");
struct _RegionHandle*r=& _tmp8E5;_push_region(r);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tagged_arr*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct
_tuple13*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp8E6="function declaration has repeated parameter";
_tag_arr(_tmp8E6,sizeof(char),_get_zero_arr_size(_tmp8E6,44));}));;_pop_region(r);}
fd->cached_typ=({struct Cyc_Core_Opt*_tmp8E7=_cycalloc(sizeof(*_tmp8E7));_tmp8E7->v=(
void*)t;_tmp8E7;});}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,int
allow_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp8E8=Cyc_Tcutil_check_valid_type(
loc,te,bound_tvars,expected_kind,0,t,1);struct Cyc_List_List*_tmp8E9=Cyc_Tcutil_remove_bound_tvars(
_tmp8E8.free_vars,bound_tvars);struct Cyc_List_List*_tmp8EA=_tmp8E8.free_evars;{
struct Cyc_List_List*fs=_tmp8E9;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp8EB=((
struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct _tmp8EF;_tmp8EF.tag=
0;_tmp8EF.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
struct Cyc_String_pa_struct _tmp8EE;_tmp8EE.tag=0;_tmp8EE.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp8EB);{void*_tmp8EC[2]={& _tmp8EE,& _tmp8EF};Cyc_Tcutil_terr(
loc,({const char*_tmp8ED="unbound type variable %s in type %s";_tag_arr(_tmp8ED,
sizeof(char),_get_zero_arr_size(_tmp8ED,36));}),_tag_arr(_tmp8EC,sizeof(void*),2));}}});}}
if(!allow_evars  && _tmp8EA != 0)for(0;_tmp8EA != 0;_tmp8EA=_tmp8EA->tl){void*e=(
void*)_tmp8EA->hd;void*_tmp8F0=Cyc_Tcutil_typ_kind(e);_LL590: if((int)_tmp8F0 != 3)
goto _LL592;_LL591: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp8F1[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8F2="can't unify evar with heap!";
_tag_arr(_tmp8F2,sizeof(char),_get_zero_arr_size(_tmp8F2,28));}),_tag_arr(
_tmp8F1,sizeof(void*),0));});goto _LL58F;_LL592: if((int)_tmp8F0 != 4)goto _LL594;
_LL593: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp8F3[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8F4="can't unify evar with {}!";
_tag_arr(_tmp8F4,sizeof(char),_get_zero_arr_size(_tmp8F4,26));}),_tag_arr(
_tmp8F3,sizeof(void*),0));});goto _LL58F;_LL594:;_LL595:({struct Cyc_String_pa_struct
_tmp8F8;_tmp8F8.tag=0;_tmp8F8.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp8F7;_tmp8F7.tag=0;_tmp8F7.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(e));{void*_tmp8F5[2]={&
_tmp8F7,& _tmp8F8};Cyc_Tcutil_terr(loc,({const char*_tmp8F6="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp8F6,sizeof(char),_get_zero_arr_size(_tmp8F6,52));}),_tag_arr(
_tmp8F5,sizeof(void*),2));}}});goto _LL58F;_LL58F:;}}void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,
tvs);}static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct
Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr(*a2string)(
void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){struct Cyc_List_List*vs2=vs->tl;
for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(void*)vs2->hd)== 0)({struct Cyc_String_pa_struct
_tmp8FC;_tmp8FC.tag=0;_tmp8FC.f1=(struct _tagged_arr)((struct _tagged_arr)a2string((
void*)vs->hd));{struct Cyc_String_pa_struct _tmp8FB;_tmp8FB.tag=0;_tmp8FB.f1=(
struct _tagged_arr)((struct _tagged_arr)msg);{void*_tmp8F9[2]={& _tmp8FB,& _tmp8FC};
Cyc_Tcutil_terr(loc,({const char*_tmp8FA="%s: %s";_tag_arr(_tmp8FA,sizeof(char),
_get_zero_arr_size(_tmp8FA,7));}),_tag_arr(_tmp8F9,sizeof(void*),2));}}});}}}
static struct _tagged_arr Cyc_Tcutil_strptr2string(struct _tagged_arr*s){return*s;}
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr msg){((void(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct
_tagged_arr(*a2string)(struct _tagged_arr*),struct _tagged_arr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){((void(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr(*a2string)(struct Cyc_Absyn_Tvar*),struct _tagged_arr msg))
Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,({
const char*_tmp8FD="duplicate type variable";_tag_arr(_tmp8FD,sizeof(char),
_get_zero_arr_size(_tmp8FD,24));}));}struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;
int f2;};struct _tuple19{struct Cyc_List_List*f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*
f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){if(Cyc_strcmp((
struct _tagged_arr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,({const char*
_tmp8FE="";_tag_arr(_tmp8FE,sizeof(char),_get_zero_arr_size(_tmp8FE,1));}))!= 0)
fields=({struct Cyc_List_List*_tmp8FF=_cycalloc(sizeof(*_tmp8FF));_tmp8FF->hd=({
struct _tuple18*_tmp900=_cycalloc(sizeof(*_tmp900));_tmp900->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd;_tmp900->f2=0;_tmp900;});_tmp8FF->tl=fields;_tmp8FF;});}}fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct
Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple19 _tmp902;struct Cyc_List_List*
_tmp903;void*_tmp904;struct _tuple19*_tmp901=(struct _tuple19*)des->hd;_tmp902=*
_tmp901;_tmp903=_tmp902.f1;_tmp904=_tmp902.f2;if(_tmp903 == 0){struct Cyc_List_List*
_tmp905=fields;for(0;_tmp905 != 0;_tmp905=_tmp905->tl){if(!(*((struct _tuple18*)
_tmp905->hd)).f2){(*((struct _tuple18*)_tmp905->hd)).f2=1;(*((struct _tuple19*)des->hd)).f1=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp906=_cycalloc(sizeof(*_tmp906));
_tmp906->hd=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp907=_cycalloc(
sizeof(*_tmp907));_tmp907[0]=({struct Cyc_Absyn_FieldName_struct _tmp908;_tmp908.tag=
1;_tmp908.f1=((*((struct _tuple18*)_tmp905->hd)).f1)->name;_tmp908;});_tmp907;}));
_tmp906->tl=0;_tmp906;});ans=({struct Cyc_List_List*_tmp909=_region_malloc(rgn,
sizeof(*_tmp909));_tmp909->hd=({struct _tuple20*_tmp90A=_region_malloc(rgn,
sizeof(*_tmp90A));_tmp90A->f1=(*((struct _tuple18*)_tmp905->hd)).f1;_tmp90A->f2=
_tmp904;_tmp90A;});_tmp909->tl=ans;_tmp909;});break;}}if(_tmp905 == 0)({void*
_tmp90B[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp90C="too many arguments to struct";
_tag_arr(_tmp90C,sizeof(char),_get_zero_arr_size(_tmp90C,29));}),_tag_arr(
_tmp90B,sizeof(void*),0));});}else{if(_tmp903->tl != 0)({void*_tmp90D[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp90E="multiple designators are not supported";_tag_arr(
_tmp90E,sizeof(char),_get_zero_arr_size(_tmp90E,39));}),_tag_arr(_tmp90D,sizeof(
void*),0));});else{void*_tmp90F=(void*)_tmp903->hd;struct _tagged_arr*_tmp910;
_LL597: if(*((int*)_tmp90F)!= 0)goto _LL599;_LL598:({void*_tmp911[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp912="array designator used in argument to struct";_tag_arr(
_tmp912,sizeof(char),_get_zero_arr_size(_tmp912,44));}),_tag_arr(_tmp911,sizeof(
void*),0));});goto _LL596;_LL599: if(*((int*)_tmp90F)!= 1)goto _LL596;_tmp910=((
struct Cyc_Absyn_FieldName_struct*)_tmp90F)->f1;_LL59A: {struct Cyc_List_List*
_tmp913=fields;for(0;_tmp913 != 0;_tmp913=_tmp913->tl){if(Cyc_strptrcmp(_tmp910,((*((
struct _tuple18*)_tmp913->hd)).f1)->name)== 0){if((*((struct _tuple18*)_tmp913->hd)).f2)({
struct Cyc_String_pa_struct _tmp916;_tmp916.tag=0;_tmp916.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp910);{void*_tmp914[1]={& _tmp916};Cyc_Tcutil_terr(loc,({
const char*_tmp915="field %s has already been used as an argument";_tag_arr(
_tmp915,sizeof(char),_get_zero_arr_size(_tmp915,46));}),_tag_arr(_tmp914,sizeof(
void*),1));}});(*((struct _tuple18*)_tmp913->hd)).f2=1;ans=({struct Cyc_List_List*
_tmp917=_region_malloc(rgn,sizeof(*_tmp917));_tmp917->hd=({struct _tuple20*
_tmp918=_region_malloc(rgn,sizeof(*_tmp918));_tmp918->f1=(*((struct _tuple18*)
_tmp913->hd)).f1;_tmp918->f2=_tmp904;_tmp918;});_tmp917->tl=ans;_tmp917;});
break;}}if(_tmp913 == 0)({struct Cyc_String_pa_struct _tmp91B;_tmp91B.tag=0;_tmp91B.f1=(
struct _tagged_arr)((struct _tagged_arr)*_tmp910);{void*_tmp919[1]={& _tmp91B};Cyc_Tcutil_terr(
loc,({const char*_tmp91A="bad field designator %s";_tag_arr(_tmp91A,sizeof(char),
_get_zero_arr_size(_tmp91A,24));}),_tag_arr(_tmp919,sizeof(void*),1));}});goto
_LL596;}_LL596:;}}}for(0;fields != 0;fields=fields->tl){if(!(*((struct _tuple18*)
fields->hd)).f2){({void*_tmp91C[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp91D="too few arguments to struct";
_tag_arr(_tmp91D,sizeof(char),_get_zero_arr_size(_tmp91D,28));}),_tag_arr(
_tmp91C,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmp91E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp91F;
void*_tmp920;struct Cyc_Absyn_PtrAtts _tmp921;struct Cyc_Absyn_Conref*_tmp922;
_LL59C: if(_tmp91E <= (void*)4  || *((int*)_tmp91E)!= 4)goto _LL59E;_tmp91F=((struct
Cyc_Absyn_PointerType_struct*)_tmp91E)->f1;_tmp920=(void*)_tmp91F.elt_typ;
_tmp921=_tmp91F.ptr_atts;_tmp922=_tmp921.bounds;_LL59D: {struct Cyc_Absyn_Conref*
_tmp923=Cyc_Absyn_compress_conref(_tmp922);void*_tmp924=(void*)(Cyc_Absyn_compress_conref(
_tmp923))->v;void*_tmp925;_LL5A1: if(_tmp924 <= (void*)1  || *((int*)_tmp924)!= 0)
goto _LL5A3;_tmp925=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp924)->f1;if((
int)_tmp925 != 0)goto _LL5A3;_LL5A2:*elt_typ_dest=_tmp920;return 1;_LL5A3: if((int)
_tmp924 != 0)goto _LL5A5;_LL5A4:(void*)(_tmp923->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp926=_cycalloc(sizeof(*_tmp926));_tmp926[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp927;_tmp927.tag=0;_tmp927.f1=(void*)((void*)0);_tmp927;});_tmp926;})));*
elt_typ_dest=_tmp920;return 1;_LL5A5:;_LL5A6: return 0;_LL5A0:;}_LL59E:;_LL59F:
return 0;_LL59B:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp928=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp929;void*_tmp92A;
struct Cyc_Absyn_PtrAtts _tmp92B;struct Cyc_Absyn_Conref*_tmp92C;_LL5A8: if(_tmp928
<= (void*)4  || *((int*)_tmp928)!= 4)goto _LL5AA;_tmp929=((struct Cyc_Absyn_PointerType_struct*)
_tmp928)->f1;_tmp92A=(void*)_tmp929.elt_typ;_tmp92B=_tmp929.ptr_atts;_tmp92C=
_tmp92B.zero_term;_LL5A9:*elt_typ_dest=_tmp92A;return((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp92C);_LL5AA:;_LL5AB: return 0;_LL5A7:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmp970;_tmp970.f1=0;
_tmp970.f2=(void*)2;_tmp970;});void*_tmp92D=(void*)e->r;struct _tuple1*_tmp92E;
void*_tmp92F;struct Cyc_Absyn_Exp*_tmp930;struct _tagged_arr*_tmp931;struct Cyc_Absyn_Exp*
_tmp932;struct _tagged_arr*_tmp933;struct Cyc_Absyn_Exp*_tmp934;struct Cyc_Absyn_Exp*
_tmp935;struct Cyc_Absyn_Exp*_tmp936;_LL5AD: if(*((int*)_tmp92D)!= 1)goto _LL5AF;
_tmp92E=((struct Cyc_Absyn_Var_e_struct*)_tmp92D)->f1;_tmp92F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp92D)->f2;_LL5AE: {void*_tmp937=_tmp92F;struct Cyc_Absyn_Vardecl*_tmp938;
struct Cyc_Absyn_Vardecl*_tmp939;struct Cyc_Absyn_Vardecl*_tmp93A;struct Cyc_Absyn_Vardecl*
_tmp93B;_LL5BA: if((int)_tmp937 != 0)goto _LL5BC;_LL5BB: goto _LL5BD;_LL5BC: if(
_tmp937 <= (void*)1  || *((int*)_tmp937)!= 1)goto _LL5BE;_LL5BD: return bogus_ans;
_LL5BE: if(_tmp937 <= (void*)1  || *((int*)_tmp937)!= 0)goto _LL5C0;_tmp938=((struct
Cyc_Absyn_Global_b_struct*)_tmp937)->f1;_LL5BF: {void*_tmp93C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL5C7: if(_tmp93C <= (void*)
4  || *((int*)_tmp93C)!= 7)goto _LL5C9;_LL5C8: return({struct _tuple10 _tmp93D;
_tmp93D.f1=1;_tmp93D.f2=(void*)2;_tmp93D;});_LL5C9:;_LL5CA: return({struct
_tuple10 _tmp93E;_tmp93E.f1=(_tmp938->tq).real_const;_tmp93E.f2=(void*)2;_tmp93E;});
_LL5C6:;}_LL5C0: if(_tmp937 <= (void*)1  || *((int*)_tmp937)!= 3)goto _LL5C2;_tmp939=((
struct Cyc_Absyn_Local_b_struct*)_tmp937)->f1;_LL5C1: {void*_tmp93F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL5CC: if(_tmp93F <= (void*)
4  || *((int*)_tmp93F)!= 7)goto _LL5CE;_LL5CD: return({struct _tuple10 _tmp940;
_tmp940.f1=1;_tmp940.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp939->rgn))->v;
_tmp940;});_LL5CE:;_LL5CF: _tmp939->escapes=1;return({struct _tuple10 _tmp941;
_tmp941.f1=(_tmp939->tq).real_const;_tmp941.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp939->rgn))->v;_tmp941;});_LL5CB:;}_LL5C2: if(_tmp937 <= (void*)1
 || *((int*)_tmp937)!= 4)goto _LL5C4;_tmp93A=((struct Cyc_Absyn_Pat_b_struct*)
_tmp937)->f1;_LL5C3: _tmp93B=_tmp93A;goto _LL5C5;_LL5C4: if(_tmp937 <= (void*)1  || *((
int*)_tmp937)!= 2)goto _LL5B9;_tmp93B=((struct Cyc_Absyn_Param_b_struct*)_tmp937)->f1;
_LL5C5: _tmp93B->escapes=1;return({struct _tuple10 _tmp942;_tmp942.f1=(_tmp93B->tq).real_const;
_tmp942.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp93B->rgn))->v;_tmp942;});
_LL5B9:;}_LL5AF: if(*((int*)_tmp92D)!= 23)goto _LL5B1;_tmp930=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp92D)->f1;_tmp931=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp92D)->f2;_LL5B0: {
void*_tmp943=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp930->topt))->v);struct Cyc_List_List*_tmp944;struct Cyc_Absyn_AggrInfo _tmp945;
void*_tmp946;struct Cyc_Absyn_Aggrdecl**_tmp947;struct Cyc_Absyn_Aggrdecl*_tmp948;
_LL5D1: if(_tmp943 <= (void*)4  || *((int*)_tmp943)!= 11)goto _LL5D3;_tmp944=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp943)->f2;_LL5D2: {struct Cyc_Absyn_Aggrfield*
_tmp949=Cyc_Absyn_lookup_field(_tmp944,_tmp931);if(_tmp949 != 0  && _tmp949->width
!= 0)return({struct _tuple10 _tmp94A;_tmp94A.f1=(_tmp949->tq).real_const;_tmp94A.f2=(
Cyc_Tcutil_addressof_props(te,_tmp930)).f2;_tmp94A;});return bogus_ans;}_LL5D3:
if(_tmp943 <= (void*)4  || *((int*)_tmp943)!= 10)goto _LL5D5;_tmp945=((struct Cyc_Absyn_AggrType_struct*)
_tmp943)->f1;_tmp946=(void*)_tmp945.aggr_info;if(*((int*)_tmp946)!= 1)goto _LL5D5;
_tmp947=((struct Cyc_Absyn_KnownAggr_struct*)_tmp946)->f1;_tmp948=*_tmp947;_LL5D4: {
struct Cyc_Absyn_Aggrfield*_tmp94B=Cyc_Absyn_lookup_decl_field(_tmp948,_tmp931);
if(_tmp94B != 0  && _tmp94B->width != 0)return({struct _tuple10 _tmp94C;_tmp94C.f1=(
_tmp94B->tq).real_const;_tmp94C.f2=(Cyc_Tcutil_addressof_props(te,_tmp930)).f2;
_tmp94C;});return bogus_ans;}_LL5D5:;_LL5D6: return bogus_ans;_LL5D0:;}_LL5B1: if(*((
int*)_tmp92D)!= 24)goto _LL5B3;_tmp932=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp92D)->f1;_tmp933=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp92D)->f2;_LL5B2: {
void*_tmp94D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp932->topt))->v);struct Cyc_Absyn_PtrInfo _tmp94E;void*_tmp94F;struct Cyc_Absyn_PtrAtts
_tmp950;void*_tmp951;_LL5D8: if(_tmp94D <= (void*)4  || *((int*)_tmp94D)!= 4)goto
_LL5DA;_tmp94E=((struct Cyc_Absyn_PointerType_struct*)_tmp94D)->f1;_tmp94F=(void*)
_tmp94E.elt_typ;_tmp950=_tmp94E.ptr_atts;_tmp951=(void*)_tmp950.rgn;_LL5D9: {
struct Cyc_Absyn_Aggrfield*finfo;{void*_tmp952=Cyc_Tcutil_compress(_tmp94F);
struct Cyc_List_List*_tmp953;struct Cyc_Absyn_AggrInfo _tmp954;void*_tmp955;struct
Cyc_Absyn_Aggrdecl**_tmp956;struct Cyc_Absyn_Aggrdecl*_tmp957;_LL5DD: if(_tmp952 <= (
void*)4  || *((int*)_tmp952)!= 11)goto _LL5DF;_tmp953=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp952)->f2;_LL5DE: finfo=Cyc_Absyn_lookup_field(_tmp953,_tmp933);goto _LL5DC;
_LL5DF: if(_tmp952 <= (void*)4  || *((int*)_tmp952)!= 10)goto _LL5E1;_tmp954=((
struct Cyc_Absyn_AggrType_struct*)_tmp952)->f1;_tmp955=(void*)_tmp954.aggr_info;
if(*((int*)_tmp955)!= 1)goto _LL5E1;_tmp956=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp955)->f1;_tmp957=*_tmp956;_LL5E0: finfo=Cyc_Absyn_lookup_decl_field(_tmp957,
_tmp933);goto _LL5DC;_LL5E1:;_LL5E2: return bogus_ans;_LL5DC:;}if(finfo != 0  && 
finfo->width != 0)return({struct _tuple10 _tmp958;_tmp958.f1=(finfo->tq).real_const;
_tmp958.f2=_tmp951;_tmp958;});return bogus_ans;}_LL5DA:;_LL5DB: return bogus_ans;
_LL5D7:;}_LL5B3: if(*((int*)_tmp92D)!= 22)goto _LL5B5;_tmp934=((struct Cyc_Absyn_Deref_e_struct*)
_tmp92D)->f1;_LL5B4: {void*_tmp959=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp934->topt))->v);struct Cyc_Absyn_PtrInfo _tmp95A;struct Cyc_Absyn_Tqual
_tmp95B;struct Cyc_Absyn_PtrAtts _tmp95C;void*_tmp95D;_LL5E4: if(_tmp959 <= (void*)4
 || *((int*)_tmp959)!= 4)goto _LL5E6;_tmp95A=((struct Cyc_Absyn_PointerType_struct*)
_tmp959)->f1;_tmp95B=_tmp95A.elt_tq;_tmp95C=_tmp95A.ptr_atts;_tmp95D=(void*)
_tmp95C.rgn;_LL5E5: return({struct _tuple10 _tmp95E;_tmp95E.f1=_tmp95B.real_const;
_tmp95E.f2=_tmp95D;_tmp95E;});_LL5E6:;_LL5E7: return bogus_ans;_LL5E3:;}_LL5B5: if(*((
int*)_tmp92D)!= 25)goto _LL5B7;_tmp935=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp92D)->f1;_tmp936=((struct Cyc_Absyn_Subscript_e_struct*)_tmp92D)->f2;_LL5B6: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp935->topt))->v);
void*_tmp95F=t;struct Cyc_List_List*_tmp960;struct Cyc_Absyn_PtrInfo _tmp961;struct
Cyc_Absyn_Tqual _tmp962;struct Cyc_Absyn_PtrAtts _tmp963;void*_tmp964;struct Cyc_Absyn_ArrayInfo
_tmp965;struct Cyc_Absyn_Tqual _tmp966;_LL5E9: if(_tmp95F <= (void*)4  || *((int*)
_tmp95F)!= 9)goto _LL5EB;_tmp960=((struct Cyc_Absyn_TupleType_struct*)_tmp95F)->f1;
_LL5EA: {unsigned int _tmp968;int _tmp969;struct _tuple7 _tmp967=Cyc_Evexp_eval_const_uint_exp(
_tmp936);_tmp968=_tmp967.f1;_tmp969=_tmp967.f2;if(!_tmp969)return bogus_ans;{
struct _tuple4*_tmp96A=Cyc_Absyn_lookup_tuple_field(_tmp960,(int)_tmp968);if(
_tmp96A != 0)return({struct _tuple10 _tmp96B;_tmp96B.f1=((*_tmp96A).f1).real_const;
_tmp96B.f2=(Cyc_Tcutil_addressof_props(te,_tmp935)).f2;_tmp96B;});return
bogus_ans;}}_LL5EB: if(_tmp95F <= (void*)4  || *((int*)_tmp95F)!= 4)goto _LL5ED;
_tmp961=((struct Cyc_Absyn_PointerType_struct*)_tmp95F)->f1;_tmp962=_tmp961.elt_tq;
_tmp963=_tmp961.ptr_atts;_tmp964=(void*)_tmp963.rgn;_LL5EC: return({struct
_tuple10 _tmp96C;_tmp96C.f1=_tmp962.real_const;_tmp96C.f2=_tmp964;_tmp96C;});
_LL5ED: if(_tmp95F <= (void*)4  || *((int*)_tmp95F)!= 7)goto _LL5EF;_tmp965=((struct
Cyc_Absyn_ArrayType_struct*)_tmp95F)->f1;_tmp966=_tmp965.tq;_LL5EE: return({
struct _tuple10 _tmp96D;_tmp96D.f1=_tmp966.real_const;_tmp96D.f2=(Cyc_Tcutil_addressof_props(
te,_tmp935)).f2;_tmp96D;});_LL5EF:;_LL5F0: return bogus_ans;_LL5E8:;}_LL5B7:;
_LL5B8:({void*_tmp96E[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp96F="unary & applied to non-lvalue";
_tag_arr(_tmp96F,sizeof(char),_get_zero_arr_size(_tmp96F,30));}),_tag_arr(
_tmp96E,sizeof(void*),0));});return bogus_ans;_LL5AC:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmp971=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmp972;void*_tmp973;struct Cyc_Absyn_Tqual
_tmp974;struct Cyc_Absyn_Conref*_tmp975;_LL5F2: if(_tmp971 <= (void*)4  || *((int*)
_tmp971)!= 7)goto _LL5F4;_tmp972=((struct Cyc_Absyn_ArrayType_struct*)_tmp971)->f1;
_tmp973=(void*)_tmp972.elt_type;_tmp974=_tmp972.tq;_tmp975=_tmp972.zero_term;
_LL5F3: {void*_tmp977;struct _tuple10 _tmp976=Cyc_Tcutil_addressof_props(te,e);
_tmp977=_tmp976.f2;return Cyc_Absyn_atb_typ(_tmp973,_tmp977,_tmp974,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp978=_cycalloc(sizeof(*_tmp978));_tmp978[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp979;_tmp979.tag=0;_tmp979.f1=e;_tmp979;});
_tmp978;}),_tmp975);}_LL5F4:;_LL5F5: return e_typ;_LL5F1:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{void*_tmp97A=(void*)b->v;void*_tmp97B;void*_tmp97C;struct Cyc_Absyn_Exp*
_tmp97D;void*_tmp97E;_LL5F7: if(_tmp97A <= (void*)1  || *((int*)_tmp97A)!= 0)goto
_LL5F9;_tmp97B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp97A)->f1;if((int)
_tmp97B != 0)goto _LL5F9;_LL5F8: return;_LL5F9: if(_tmp97A <= (void*)1  || *((int*)
_tmp97A)!= 0)goto _LL5FB;_tmp97C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp97A)->f1;if(_tmp97C <= (void*)1  || *((int*)_tmp97C)!= 0)goto _LL5FB;_tmp97D=((
struct Cyc_Absyn_Upper_b_struct*)_tmp97C)->f1;_LL5FA: {unsigned int _tmp980;int
_tmp981;struct _tuple7 _tmp97F=Cyc_Evexp_eval_const_uint_exp(_tmp97D);_tmp980=
_tmp97F.f1;_tmp981=_tmp97F.f2;if(_tmp981  && _tmp980 <= i)({struct Cyc_Int_pa_struct
_tmp985;_tmp985.tag=1;_tmp985.f1=(unsigned int)((int)i);{struct Cyc_Int_pa_struct
_tmp984;_tmp984.tag=1;_tmp984.f1=(unsigned int)((int)_tmp980);{void*_tmp982[2]={&
_tmp984,& _tmp985};Cyc_Tcutil_terr(loc,({const char*_tmp983="dereference is out of bounds: %d <= %d";
_tag_arr(_tmp983,sizeof(char),_get_zero_arr_size(_tmp983,39));}),_tag_arr(
_tmp982,sizeof(void*),2));}}});return;}_LL5FB: if(_tmp97A <= (void*)1  || *((int*)
_tmp97A)!= 0)goto _LL5FD;_tmp97E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp97A)->f1;if(_tmp97E <= (void*)1  || *((int*)_tmp97E)!= 1)goto _LL5FD;_LL5FC:
return;_LL5FD: if((int)_tmp97A != 0)goto _LL5FF;_LL5FE:(void*)(b->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp986=_cycalloc(sizeof(*_tmp986));_tmp986[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp987;_tmp987.tag=0;_tmp987.f1=(void*)((void*)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp988=_cycalloc(sizeof(*_tmp988));
_tmp988[0]=({struct Cyc_Absyn_Upper_b_struct _tmp989;_tmp989.tag=0;_tmp989.f1=Cyc_Absyn_uint_exp(
i + 1,0);_tmp989;});_tmp988;})));_tmp987;});_tmp986;})));return;_LL5FF:;_LL600:({
void*_tmp98A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp98B="check_bound -- bad compressed conref";_tag_arr(_tmp98B,
sizeof(char),_get_zero_arr_size(_tmp98B,37));}),_tag_arr(_tmp98A,sizeof(void*),0));});
_LL5F6:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){void*_tmp98C=(void*)(Cyc_Absyn_compress_conref(b))->v;
void*_tmp98D;struct Cyc_Absyn_Exp*_tmp98E;_LL602: if(_tmp98C <= (void*)1  || *((int*)
_tmp98C)!= 0)goto _LL604;_tmp98D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp98C)->f1;if(_tmp98D <= (void*)1  || *((int*)_tmp98D)!= 0)goto _LL604;_tmp98E=((
struct Cyc_Absyn_Upper_b_struct*)_tmp98D)->f1;_LL603: {unsigned int _tmp990;int
_tmp991;struct _tuple7 _tmp98F=Cyc_Evexp_eval_const_uint_exp(_tmp98E);_tmp990=
_tmp98F.f1;_tmp991=_tmp98F.f2;return _tmp991  && _tmp990 == 1;}_LL604:;_LL605:
return 0;_LL601:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp992=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmp993;void*_tmp994;struct Cyc_Absyn_Conref*_tmp995;
struct Cyc_List_List*_tmp996;struct Cyc_Absyn_AggrInfo _tmp997;void*_tmp998;struct
Cyc_Absyn_AggrInfo _tmp999;void*_tmp99A;struct Cyc_Absyn_Aggrdecl**_tmp99B;struct
Cyc_Absyn_Aggrdecl*_tmp99C;struct Cyc_List_List*_tmp99D;struct Cyc_List_List*
_tmp99E;_LL607: if((int)_tmp992 != 0)goto _LL609;_LL608: goto _LL60A;_LL609: if(
_tmp992 <= (void*)4  || *((int*)_tmp992)!= 5)goto _LL60B;_LL60A: goto _LL60C;_LL60B:
if((int)_tmp992 != 1)goto _LL60D;_LL60C: goto _LL60E;_LL60D: if(_tmp992 <= (void*)4
 || *((int*)_tmp992)!= 6)goto _LL60F;_LL60E: return 1;_LL60F: if(_tmp992 <= (void*)4
 || *((int*)_tmp992)!= 12)goto _LL611;_LL610: goto _LL612;_LL611: if(_tmp992 <= (void*)
4  || *((int*)_tmp992)!= 13)goto _LL613;_LL612: return 0;_LL613: if(_tmp992 <= (void*)
4  || *((int*)_tmp992)!= 7)goto _LL615;_tmp993=((struct Cyc_Absyn_ArrayType_struct*)
_tmp992)->f1;_tmp994=(void*)_tmp993.elt_type;_tmp995=_tmp993.zero_term;_LL614:
return !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp995)
 && Cyc_Tcutil_bits_only(_tmp994);_LL615: if(_tmp992 <= (void*)4  || *((int*)
_tmp992)!= 9)goto _LL617;_tmp996=((struct Cyc_Absyn_TupleType_struct*)_tmp992)->f1;
_LL616: for(0;_tmp996 != 0;_tmp996=_tmp996->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple4*)_tmp996->hd)).f2))return 0;}return 1;_LL617: if(_tmp992 <= (void*)4
 || *((int*)_tmp992)!= 10)goto _LL619;_tmp997=((struct Cyc_Absyn_AggrType_struct*)
_tmp992)->f1;_tmp998=(void*)_tmp997.aggr_info;if(*((int*)_tmp998)!= 0)goto _LL619;
_LL618: return 0;_LL619: if(_tmp992 <= (void*)4  || *((int*)_tmp992)!= 10)goto _LL61B;
_tmp999=((struct Cyc_Absyn_AggrType_struct*)_tmp992)->f1;_tmp99A=(void*)_tmp999.aggr_info;
if(*((int*)_tmp99A)!= 1)goto _LL61B;_tmp99B=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp99A)->f1;_tmp99C=*_tmp99B;_tmp99D=_tmp999.targs;_LL61A: if(_tmp99C->impl == 0)
return 0;{struct _RegionHandle _tmp99F=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp99F;_push_region(rgn);{struct Cyc_List_List*_tmp9A0=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp99C->tvs,_tmp99D);{struct Cyc_List_List*fs=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp99C->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp9A0,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type))){int _tmp9A1=0;_npop_handler(0);return _tmp9A1;}}}{int _tmp9A2=1;
_npop_handler(0);return _tmp9A2;}};_pop_region(rgn);}_LL61B: if(_tmp992 <= (void*)4
 || *((int*)_tmp992)!= 11)goto _LL61D;_tmp99E=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp992)->f2;_LL61C: for(0;_tmp99E != 0;_tmp99E=_tmp99E->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp99E->hd)->type))return 0;}return 1;_LL61D:;
_LL61E: return 0;_LL606:;}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmp9A3=(void*)e->r;struct _tuple1*_tmp9A4;void*_tmp9A5;struct Cyc_Absyn_Exp*
_tmp9A6;struct Cyc_Absyn_Exp*_tmp9A7;struct Cyc_Absyn_Exp*_tmp9A8;struct Cyc_Absyn_Exp*
_tmp9A9;struct Cyc_Absyn_Exp*_tmp9AA;struct Cyc_Absyn_Exp*_tmp9AB;struct Cyc_Absyn_Exp*
_tmp9AC;void*_tmp9AD;struct Cyc_Absyn_Exp*_tmp9AE;void*_tmp9AF;void*_tmp9B0;
struct Cyc_Absyn_Exp*_tmp9B1;struct Cyc_Absyn_Exp*_tmp9B2;struct Cyc_Absyn_Exp*
_tmp9B3;struct Cyc_Absyn_Exp*_tmp9B4;struct Cyc_List_List*_tmp9B5;struct Cyc_List_List*
_tmp9B6;struct Cyc_List_List*_tmp9B7;void*_tmp9B8;struct Cyc_List_List*_tmp9B9;
struct Cyc_List_List*_tmp9BA;struct Cyc_List_List*_tmp9BB;_LL620: if(*((int*)
_tmp9A3)!= 0)goto _LL622;_LL621: goto _LL623;_LL622: if(*((int*)_tmp9A3)!= 18)goto
_LL624;_LL623: goto _LL625;_LL624: if(*((int*)_tmp9A3)!= 19)goto _LL626;_LL625: goto
_LL627;_LL626: if(*((int*)_tmp9A3)!= 20)goto _LL628;_LL627: goto _LL629;_LL628: if(*((
int*)_tmp9A3)!= 21)goto _LL62A;_LL629: goto _LL62B;_LL62A: if(*((int*)_tmp9A3)!= 33)
goto _LL62C;_LL62B: goto _LL62D;_LL62C: if(*((int*)_tmp9A3)!= 34)goto _LL62E;_LL62D:
return 1;_LL62E: if(*((int*)_tmp9A3)!= 1)goto _LL630;_tmp9A4=((struct Cyc_Absyn_Var_e_struct*)
_tmp9A3)->f1;_tmp9A5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9A3)->f2;_LL62F: {
void*_tmp9BC=_tmp9A5;struct Cyc_Absyn_Vardecl*_tmp9BD;_LL64F: if(_tmp9BC <= (void*)
1  || *((int*)_tmp9BC)!= 1)goto _LL651;_LL650: return 1;_LL651: if(_tmp9BC <= (void*)1
 || *((int*)_tmp9BC)!= 0)goto _LL653;_tmp9BD=((struct Cyc_Absyn_Global_b_struct*)
_tmp9BC)->f1;_LL652: {void*_tmp9BE=Cyc_Tcutil_compress((void*)_tmp9BD->type);
_LL658: if(_tmp9BE <= (void*)4  || *((int*)_tmp9BE)!= 7)goto _LL65A;_LL659: goto
_LL65B;_LL65A: if(_tmp9BE <= (void*)4  || *((int*)_tmp9BE)!= 8)goto _LL65C;_LL65B:
return 1;_LL65C:;_LL65D: return var_okay;_LL657:;}_LL653: if((int)_tmp9BC != 0)goto
_LL655;_LL654: return 0;_LL655:;_LL656: return var_okay;_LL64E:;}_LL630: if(*((int*)
_tmp9A3)!= 6)goto _LL632;_tmp9A6=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9A3)->f1;
_tmp9A7=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9A3)->f2;_tmp9A8=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp9A3)->f3;_LL631: return(Cyc_Tcutil_cnst_exp(te,0,_tmp9A6) && Cyc_Tcutil_cnst_exp(
te,0,_tmp9A7)) && Cyc_Tcutil_cnst_exp(te,0,_tmp9A8);_LL632: if(*((int*)_tmp9A3)!= 
9)goto _LL634;_tmp9A9=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp9A3)->f1;_tmp9AA=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp9A3)->f2;_LL633: return Cyc_Tcutil_cnst_exp(
te,0,_tmp9A9) && Cyc_Tcutil_cnst_exp(te,0,_tmp9AA);_LL634: if(*((int*)_tmp9A3)!= 
13)goto _LL636;_tmp9AB=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp9A3)->f1;
_LL635: _tmp9AC=_tmp9AB;goto _LL637;_LL636: if(*((int*)_tmp9A3)!= 14)goto _LL638;
_tmp9AC=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp9A3)->f1;_LL637: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp9AC);_LL638: if(*((int*)_tmp9A3)!= 15)goto _LL63A;_tmp9AD=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp9A3)->f1;_tmp9AE=((struct Cyc_Absyn_Cast_e_struct*)
_tmp9A3)->f2;_tmp9AF=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp9A3)->f4;if((
int)_tmp9AF != 1)goto _LL63A;_LL639: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp9AE);
_LL63A: if(*((int*)_tmp9A3)!= 15)goto _LL63C;_tmp9B0=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp9A3)->f1;_tmp9B1=((struct Cyc_Absyn_Cast_e_struct*)_tmp9A3)->f2;_LL63B: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmp9B1);_LL63C: if(*((int*)_tmp9A3)!= 16)goto
_LL63E;_tmp9B2=((struct Cyc_Absyn_Address_e_struct*)_tmp9A3)->f1;_LL63D: return Cyc_Tcutil_cnst_exp(
te,1,_tmp9B2);_LL63E: if(*((int*)_tmp9A3)!= 29)goto _LL640;_tmp9B3=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp9A3)->f2;_tmp9B4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp9A3)->f3;
_LL63F: return Cyc_Tcutil_cnst_exp(te,0,_tmp9B3) && Cyc_Tcutil_cnst_exp(te,0,
_tmp9B4);_LL640: if(*((int*)_tmp9A3)!= 28)goto _LL642;_tmp9B5=((struct Cyc_Absyn_Array_e_struct*)
_tmp9A3)->f1;_LL641: _tmp9B6=_tmp9B5;goto _LL643;_LL642: if(*((int*)_tmp9A3)!= 31)
goto _LL644;_tmp9B6=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp9A3)->f2;_LL643:
_tmp9B7=_tmp9B6;goto _LL645;_LL644: if(*((int*)_tmp9A3)!= 30)goto _LL646;_tmp9B7=((
struct Cyc_Absyn_Struct_e_struct*)_tmp9A3)->f3;_LL645: for(0;_tmp9B7 != 0;_tmp9B7=
_tmp9B7->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple21*)_tmp9B7->hd)).f2))
return 0;}return 1;_LL646: if(*((int*)_tmp9A3)!= 3)goto _LL648;_tmp9B8=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp9A3)->f1;_tmp9B9=((struct Cyc_Absyn_Primop_e_struct*)
_tmp9A3)->f2;_LL647: _tmp9BA=_tmp9B9;goto _LL649;_LL648: if(*((int*)_tmp9A3)!= 26)
goto _LL64A;_tmp9BA=((struct Cyc_Absyn_Tuple_e_struct*)_tmp9A3)->f1;_LL649: _tmp9BB=
_tmp9BA;goto _LL64B;_LL64A: if(*((int*)_tmp9A3)!= 32)goto _LL64C;_tmp9BB=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp9A3)->f1;_LL64B: for(0;_tmp9BB != 0;_tmp9BB=_tmp9BB->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp9BB->hd))return 0;}return 1;
_LL64C:;_LL64D: return 0;_LL61F:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp9BF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9C0;
struct Cyc_Absyn_PtrAtts _tmp9C1;struct Cyc_Absyn_Conref*_tmp9C2;struct Cyc_Absyn_Conref*
_tmp9C3;struct Cyc_Absyn_ArrayInfo _tmp9C4;void*_tmp9C5;struct Cyc_List_List*
_tmp9C6;struct Cyc_Absyn_AggrInfo _tmp9C7;void*_tmp9C8;struct Cyc_List_List*_tmp9C9;
struct Cyc_List_List*_tmp9CA;_LL65F: if((int)_tmp9BF != 0)goto _LL661;_LL660: goto
_LL662;_LL661: if(_tmp9BF <= (void*)4  || *((int*)_tmp9BF)!= 5)goto _LL663;_LL662:
goto _LL664;_LL663: if((int)_tmp9BF != 1)goto _LL665;_LL664: goto _LL666;_LL665: if(
_tmp9BF <= (void*)4  || *((int*)_tmp9BF)!= 6)goto _LL667;_LL666: return 1;_LL667: if(
_tmp9BF <= (void*)4  || *((int*)_tmp9BF)!= 4)goto _LL669;_tmp9C0=((struct Cyc_Absyn_PointerType_struct*)
_tmp9BF)->f1;_tmp9C1=_tmp9C0.ptr_atts;_tmp9C2=_tmp9C1.nullable;_tmp9C3=_tmp9C1.bounds;
_LL668: {void*_tmp9CB=Cyc_Absyn_conref_def((void*)((void*)0),_tmp9C3);_LL678: if((
int)_tmp9CB != 0)goto _LL67A;_LL679: return 1;_LL67A:;_LL67B: return((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp9C2);_LL677:;}_LL669: if(
_tmp9BF <= (void*)4  || *((int*)_tmp9BF)!= 7)goto _LL66B;_tmp9C4=((struct Cyc_Absyn_ArrayType_struct*)
_tmp9BF)->f1;_tmp9C5=(void*)_tmp9C4.elt_type;_LL66A: return Cyc_Tcutil_supports_default(
_tmp9C5);_LL66B: if(_tmp9BF <= (void*)4  || *((int*)_tmp9BF)!= 9)goto _LL66D;_tmp9C6=((
struct Cyc_Absyn_TupleType_struct*)_tmp9BF)->f1;_LL66C: for(0;_tmp9C6 != 0;_tmp9C6=
_tmp9C6->tl){if(!Cyc_Tcutil_supports_default((*((struct _tuple4*)_tmp9C6->hd)).f2))
return 0;}return 1;_LL66D: if(_tmp9BF <= (void*)4  || *((int*)_tmp9BF)!= 10)goto
_LL66F;_tmp9C7=((struct Cyc_Absyn_AggrType_struct*)_tmp9BF)->f1;_tmp9C8=(void*)
_tmp9C7.aggr_info;_tmp9C9=_tmp9C7.targs;_LL66E: {struct Cyc_Absyn_Aggrdecl*
_tmp9CC=Cyc_Absyn_get_known_aggrdecl(_tmp9C8);if(_tmp9CC->impl == 0)return 0;if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9CC->impl))->exist_vars != 0)return
0;return Cyc_Tcutil_fields_support_default(_tmp9CC->tvs,_tmp9C9,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp9CC->impl))->fields);}_LL66F: if(_tmp9BF <= (void*)4  || *((int*)
_tmp9BF)!= 11)goto _LL671;_tmp9CA=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp9BF)->f2;
_LL670: return Cyc_Tcutil_fields_support_default(0,0,_tmp9CA);_LL671: if(_tmp9BF <= (
void*)4  || *((int*)_tmp9BF)!= 13)goto _LL673;_LL672: goto _LL674;_LL673: if(_tmp9BF
<= (void*)4  || *((int*)_tmp9BF)!= 12)goto _LL675;_LL674: return 1;_LL675:;_LL676:
return 0;_LL65E:;}static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*
tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmp9CD=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp9CD;_push_region(rgn);{struct Cyc_List_List*
_tmp9CE=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(
0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp9CE,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type);if(!Cyc_Tcutil_supports_default(t)){int _tmp9CF=0;_npop_handler(0);
return _tmp9CF;}}};_pop_region(rgn);}return 1;}int Cyc_Tcutil_admits_zero(void*t){
void*_tmp9D0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9D1;struct Cyc_Absyn_PtrAtts
_tmp9D2;struct Cyc_Absyn_Conref*_tmp9D3;struct Cyc_Absyn_Conref*_tmp9D4;_LL67D: if(
_tmp9D0 <= (void*)4  || *((int*)_tmp9D0)!= 5)goto _LL67F;_LL67E: goto _LL680;_LL67F:
if((int)_tmp9D0 != 1)goto _LL681;_LL680: goto _LL682;_LL681: if(_tmp9D0 <= (void*)4
 || *((int*)_tmp9D0)!= 6)goto _LL683;_LL682: return 1;_LL683: if(_tmp9D0 <= (void*)4
 || *((int*)_tmp9D0)!= 4)goto _LL685;_tmp9D1=((struct Cyc_Absyn_PointerType_struct*)
_tmp9D0)->f1;_tmp9D2=_tmp9D1.ptr_atts;_tmp9D3=_tmp9D2.nullable;_tmp9D4=_tmp9D2.bounds;
_LL684: {void*_tmp9D5=(void*)(Cyc_Absyn_compress_conref(_tmp9D4))->v;void*
_tmp9D6;_LL688: if(_tmp9D5 <= (void*)1  || *((int*)_tmp9D5)!= 0)goto _LL68A;_tmp9D6=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9D5)->f1;if((int)_tmp9D6 != 0)goto
_LL68A;_LL689: return 0;_LL68A: if(_tmp9D5 <= (void*)1  || *((int*)_tmp9D5)!= 0)goto
_LL68C;_LL68B: {void*_tmp9D7=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmp9D3))->v;int _tmp9D8;_LL68F: if(_tmp9D7 <= (void*)
1  || *((int*)_tmp9D7)!= 0)goto _LL691;_tmp9D8=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp9D7)->f1;_LL690: return _tmp9D8;_LL691:;_LL692: return 0;_LL68E:;}_LL68C:;_LL68D:
return 0;_LL687:;}_LL685:;_LL686: return 0;_LL67C:;}int Cyc_Tcutil_is_noreturn(void*
t){{void*_tmp9D9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9DA;void*
_tmp9DB;struct Cyc_Absyn_FnInfo _tmp9DC;struct Cyc_List_List*_tmp9DD;_LL694: if(
_tmp9D9 <= (void*)4  || *((int*)_tmp9D9)!= 4)goto _LL696;_tmp9DA=((struct Cyc_Absyn_PointerType_struct*)
_tmp9D9)->f1;_tmp9DB=(void*)_tmp9DA.elt_typ;_LL695: return Cyc_Tcutil_is_noreturn(
_tmp9DB);_LL696: if(_tmp9D9 <= (void*)4  || *((int*)_tmp9D9)!= 8)goto _LL698;_tmp9DC=((
struct Cyc_Absyn_FnType_struct*)_tmp9D9)->f1;_tmp9DD=_tmp9DC.attributes;_LL697:
for(0;_tmp9DD != 0;_tmp9DD=_tmp9DD->tl){void*_tmp9DE=(void*)_tmp9DD->hd;_LL69B:
if((int)_tmp9DE != 3)goto _LL69D;_LL69C: return 1;_LL69D:;_LL69E: continue;_LL69A:;}
goto _LL693;_LL698:;_LL699: goto _LL693;_LL693:;}return 0;}
