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
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc___cycFILE;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
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
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_impos(struct
_tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
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
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict*consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ReachableFL_struct{
int tag;struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo
f3;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
int Cyc_Toc_warn_bounds_checks=0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=
0;unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
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
_tmp65;_LL1A: if(_tmp62 <= (void*)4  || *((int*)_tmp62)!= 4)goto _LL1C;_tmp63=((
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
_tmp9B;struct Cyc_Core_Opt _tmp9C;void*_tmp9D;_LL1F: if(_tmp94 <= (void*)4  || *((int*)
_tmp94)!= 7)goto _LL21;_tmp95=((struct Cyc_Absyn_ArrayType_struct*)_tmp94)->f1;
_tmp96=(void*)_tmp95.elt_type;_tmp97=_tmp95.tq;_tmp98=_tmp95.num_elts;_tmp99=
_tmp95.zero_term;_tmp9A=_tmp95.zt_loc;_LL20: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp96),_tmp97,_tmp98,Cyc_Absyn_false_conref,_tmp9A);_LL21: if(_tmp94 <= (void*)4
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
_LL28: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 0)goto _LL2A;_tmpA0=((struct Cyc_Absyn_Evar_struct*)
_tmp9F)->f2;if(_tmpA0 != 0)goto _LL2A;_LL29: return Cyc_Absyn_sint_typ;_LL2A: if(
_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 0)goto _LL2C;_tmpA1=((struct Cyc_Absyn_Evar_struct*)
_tmp9F)->f2;if(_tmpA1 == 0)goto _LL2C;_tmpA2=*_tmpA1;_tmpA3=(void*)_tmpA2.v;_LL2B:
return Cyc_Toc_typ_to_c(_tmpA3);_LL2C: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 1)
goto _LL2E;_tmpA4=((struct Cyc_Absyn_VarType_struct*)_tmp9F)->f1;_LL2D: if(Cyc_Tcutil_tvar_kind(
_tmpA4)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}_LL2E:
if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 2)goto _LL30;_tmpA5=((struct Cyc_Absyn_TunionType_struct*)
_tmp9F)->f1;_tmpA6=(void*)_tmpA5.tunion_info;if(*((int*)_tmpA6)!= 1)goto _LL30;
_LL2F: return Cyc_Absyn_void_star_typ();_LL30: if(_tmp9F <= (void*)4  || *((int*)
_tmp9F)!= 2)goto _LL32;_LL31:({void*_tmpC8[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmpC9="unresolved TunionType";
_tag_arr(_tmpC9,sizeof(char),_get_zero_arr_size(_tmpC9,22));}),_tag_arr(_tmpC8,
sizeof(void*),0));});_LL32: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 3)goto _LL34;
_tmpA7=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp9F)->f1;_tmpA8=(void*)
_tmpA7.field_info;if(*((int*)_tmpA8)!= 1)goto _LL34;_tmpA9=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpA8)->f1;_tmpAA=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpA8)->f2;_LL33:
if(_tmpAA->typs == 0){if(_tmpA9->is_xtunion)return Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);else{return Cyc_Absyn_uint_typ;}}
else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmpAA->name,({const char*
_tmpCA="_struct";_tag_arr(_tmpCA,sizeof(char),_get_zero_arr_size(_tmpCA,8));})));}
_LL34: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 3)goto _LL36;_LL35:({void*_tmpCB[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpCC="unresolved TunionFieldType";_tag_arr(_tmpCC,sizeof(char),
_get_zero_arr_size(_tmpCC,27));}),_tag_arr(_tmpCB,sizeof(void*),0));});_LL36: if(
_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 4)goto _LL38;_tmpAB=((struct Cyc_Absyn_PointerType_struct*)
_tmp9F)->f1;_tmpAC=(void*)_tmpAB.elt_typ;_tmpAD=_tmpAB.elt_tq;_tmpAE=_tmpAB.ptr_atts;
_tmpAF=_tmpAE.bounds;_LL37: _tmpAC=Cyc_Toc_typ_to_c_array(_tmpAC);{void*_tmpCD=(
void*)(Cyc_Absyn_compress_conref(_tmpAF))->v;void*_tmpCE;_LL61: if((int)_tmpCD != 
0)goto _LL63;_LL62: goto _LL64;_LL63: if(_tmpCD <= (void*)1  || *((int*)_tmpCD)!= 0)
goto _LL65;_tmpCE=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpCD)->f1;if((int)
_tmpCE != 0)goto _LL65;_LL64: return Cyc_Toc_tagged_arr_typ;_LL65:;_LL66: return Cyc_Absyn_star_typ(
_tmpAC,(void*)2,_tmpAD,Cyc_Absyn_false_conref);_LL60:;}_LL38: if(_tmp9F <= (void*)
4  || *((int*)_tmp9F)!= 5)goto _LL3A;_LL39: goto _LL3B;_LL3A: if((int)_tmp9F != 1)goto
_LL3C;_LL3B: goto _LL3D;_LL3C: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 6)goto
_LL3E;_LL3D: return t;_LL3E: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 7)goto _LL40;
_tmpB0=((struct Cyc_Absyn_ArrayType_struct*)_tmp9F)->f1;_tmpB1=(void*)_tmpB0.elt_type;
_tmpB2=_tmpB0.tq;_tmpB3=_tmpB0.num_elts;_tmpB4=_tmpB0.zt_loc;_LL3F: return Cyc_Absyn_array_typ(
Cyc_Toc_typ_to_c_array(_tmpB1),_tmpB2,_tmpB3,Cyc_Absyn_false_conref,_tmpB4);
_LL40: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 8)goto _LL42;_tmpB5=((struct Cyc_Absyn_FnType_struct*)
_tmp9F)->f1;_tmpB6=(void*)_tmpB5.ret_typ;_tmpB7=_tmpB5.args;_tmpB8=_tmpB5.c_varargs;
_tmpB9=_tmpB5.cyc_varargs;_tmpBA=_tmpB5.attributes;_LL41: {struct Cyc_List_List*
_tmpCF=0;for(0;_tmpBA != 0;_tmpBA=_tmpBA->tl){void*_tmpD0=(void*)_tmpBA->hd;_LL68:
if((int)_tmpD0 != 3)goto _LL6A;_LL69: goto _LL6B;_LL6A: if((int)_tmpD0 != 4)goto _LL6C;
_LL6B: goto _LL6D;_LL6C: if(_tmpD0 <= (void*)17  || *((int*)_tmpD0)!= 3)goto _LL6E;
_LL6D: continue;_LL6E: if(_tmpD0 <= (void*)17  || *((int*)_tmpD0)!= 4)goto _LL70;
_LL6F: continue;_LL70:;_LL71: _tmpCF=({struct Cyc_List_List*_tmpD1=_cycalloc(
sizeof(*_tmpD1));_tmpD1->hd=(void*)((void*)_tmpBA->hd);_tmpD1->tl=_tmpCF;_tmpD1;});
goto _LL67;_LL67:;}{struct Cyc_List_List*_tmpD2=((struct Cyc_List_List*(*)(struct
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
if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 9)goto _LL44;_tmpBB=((struct Cyc_Absyn_TupleType_struct*)
_tmp9F)->f1;_LL43: _tmpBB=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmpBB);{
struct _tagged_arr*_tmpDA=Cyc_Toc_add_tuple_type(_tmpBB);return Cyc_Absyn_strct(
_tmpDA);}_LL44: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 11)goto _LL46;_tmpBC=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp9F)->f1;_tmpBD=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp9F)->f2;_LL45: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpDB=
_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpDC;
_tmpDC.tag=11;_tmpDC.f1=(void*)_tmpBC;_tmpDC.f2=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_aggrfield_to_c,_tmpBD);_tmpDC;});_tmpDB;});_LL46: if(_tmp9F <= (void*)4
 || *((int*)_tmp9F)!= 10)goto _LL48;_tmpBE=((struct Cyc_Absyn_AggrType_struct*)
_tmp9F)->f1;_tmpBF=(void*)_tmpBE.aggr_info;_tmpC0=_tmpBE.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*
_tmpDD=Cyc_Absyn_get_known_aggrdecl(_tmpBF);if((void*)_tmpDD->kind == (void*)1)
return Cyc_Absyn_unionq_typ(_tmpDD->name);return Cyc_Absyn_strctq(_tmpDD->name);}
_LL48: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 12)goto _LL4A;_tmpC1=((struct Cyc_Absyn_EnumType_struct*)
_tmp9F)->f1;_LL49: return t;_LL4A: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 13)
goto _LL4C;_tmpC2=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp9F)->f1;_LL4B: Cyc_Toc_enumfields_to_c(
_tmpC2);return t;_LL4C: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 16)goto _LL4E;
_tmpC3=((struct Cyc_Absyn_TypedefType_struct*)_tmp9F)->f1;_tmpC4=((struct Cyc_Absyn_TypedefType_struct*)
_tmp9F)->f2;_tmpC5=((struct Cyc_Absyn_TypedefType_struct*)_tmp9F)->f3;_tmpC6=((
struct Cyc_Absyn_TypedefType_struct*)_tmp9F)->f4;_LL4D: if(_tmpC6 == 0)return(void*)({
struct Cyc_Absyn_TypedefType_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=({
struct Cyc_Absyn_TypedefType_struct _tmpDF;_tmpDF.tag=16;_tmpDF.f1=_tmpC3;_tmpDF.f2=
0;_tmpDF.f3=_tmpC5;_tmpDF.f4=0;_tmpDF;});_tmpDE;});else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_TypedefType_struct
_tmpE1;_tmpE1.tag=16;_tmpE1.f1=_tmpC3;_tmpE1.f2=0;_tmpE1.f3=_tmpC5;_tmpE1.f4=({
void**_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2[0]=Cyc_Toc_typ_to_c_array(*_tmpC6);
_tmpE2;});_tmpE1;});_tmpE0;});}_LL4E: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 
14)goto _LL50;_LL4F: goto _LL51;_LL50: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 17)
goto _LL52;_LL51: return Cyc_Absyn_uint_typ;_LL52: if(_tmp9F <= (void*)4  || *((int*)
_tmp9F)!= 15)goto _LL54;_tmpC7=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp9F)->f1;_LL53: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL54: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 18)goto _LL56;
_LL55:({void*_tmpE3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpE4="Toc::typ_to_c: type translation passed a type integer";
_tag_arr(_tmpE4,sizeof(char),_get_zero_arr_size(_tmpE4,54));}),_tag_arr(_tmpE3,
sizeof(void*),0));});_LL56: if((int)_tmp9F != 2)goto _LL58;_LL57:({void*_tmpE5[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmpE6="Toc::typ_to_c: type translation passed the heap region";_tag_arr(_tmpE6,
sizeof(char),_get_zero_arr_size(_tmpE6,55));}),_tag_arr(_tmpE5,sizeof(void*),0));});
_LL58: if((int)_tmp9F != 3)goto _LL5A;_LL59:({void*_tmpE7[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmpE8="Toc::typ_to_c: type translation passed the unique region";
_tag_arr(_tmpE8,sizeof(char),_get_zero_arr_size(_tmpE8,57));}),_tag_arr(_tmpE7,
sizeof(void*),0));});_LL5A: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 19)goto
_LL5C;_LL5B: goto _LL5D;_LL5C: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 20)goto
_LL5E;_LL5D: goto _LL5F;_LL5E: if(_tmp9F <= (void*)4  || *((int*)_tmp9F)!= 21)goto
_LL25;_LL5F:({void*_tmpE9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Toc_toc_impos)(({const char*_tmpEA="Toc::typ_to_c: type translation passed an effect";
_tag_arr(_tmpEA,sizeof(char),_get_zero_arr_size(_tmpEA,49));}),_tag_arr(_tmpE9,
sizeof(void*),0));});_LL25:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmpEB=t;struct
Cyc_Absyn_ArrayInfo _tmpEC;void*_tmpED;struct Cyc_Absyn_Tqual _tmpEE;_LL73: if(
_tmpEB <= (void*)4  || *((int*)_tmpEB)!= 7)goto _LL75;_tmpEC=((struct Cyc_Absyn_ArrayType_struct*)
_tmpEB)->f1;_tmpED=(void*)_tmpEC.elt_type;_tmpEE=_tmpEC.tq;_LL74: return Cyc_Toc_cast_it(
Cyc_Absyn_star_typ(_tmpED,(void*)2,_tmpEE,Cyc_Absyn_false_conref),e);_LL75:;
_LL76: return Cyc_Toc_cast_it(t,e);_LL72:;}static int Cyc_Toc_atomic_typ(void*t){
void*_tmpEF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpF0;void*_tmpF1;
struct Cyc_Absyn_AggrInfo _tmpF2;void*_tmpF3;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_TunionFieldInfo
_tmpF5;void*_tmpF6;struct Cyc_Absyn_Tuniondecl*_tmpF7;struct Cyc_Absyn_Tunionfield*
_tmpF8;struct Cyc_List_List*_tmpF9;_LL78: if((int)_tmpEF != 0)goto _LL7A;_LL79:
return 1;_LL7A: if(_tmpEF <= (void*)4  || *((int*)_tmpEF)!= 1)goto _LL7C;_LL7B: return
0;_LL7C: if(_tmpEF <= (void*)4  || *((int*)_tmpEF)!= 5)goto _LL7E;_LL7D: goto _LL7F;
_LL7E: if(_tmpEF <= (void*)4  || *((int*)_tmpEF)!= 12)goto _LL80;_LL7F: goto _LL81;
_LL80: if(_tmpEF <= (void*)4  || *((int*)_tmpEF)!= 13)goto _LL82;_LL81: goto _LL83;
_LL82: if((int)_tmpEF != 1)goto _LL84;_LL83: goto _LL85;_LL84: if(_tmpEF <= (void*)4
 || *((int*)_tmpEF)!= 6)goto _LL86;_LL85: goto _LL87;_LL86: if(_tmpEF <= (void*)4  || *((
int*)_tmpEF)!= 8)goto _LL88;_LL87: goto _LL89;_LL88: if(_tmpEF <= (void*)4  || *((int*)
_tmpEF)!= 17)goto _LL8A;_LL89: goto _LL8B;_LL8A: if(_tmpEF <= (void*)4  || *((int*)
_tmpEF)!= 14)goto _LL8C;_LL8B: return 1;_LL8C: if(_tmpEF <= (void*)4  || *((int*)
_tmpEF)!= 7)goto _LL8E;_tmpF0=((struct Cyc_Absyn_ArrayType_struct*)_tmpEF)->f1;
_tmpF1=(void*)_tmpF0.elt_type;_LL8D: return Cyc_Toc_atomic_typ(_tmpF1);_LL8E: if(
_tmpEF <= (void*)4  || *((int*)_tmpEF)!= 10)goto _LL90;_tmpF2=((struct Cyc_Absyn_AggrType_struct*)
_tmpEF)->f1;_tmpF3=(void*)_tmpF2.aggr_info;_LL8F:{void*_tmpFA=_tmpF3;_LL9F: if(*((
int*)_tmpFA)!= 0)goto _LLA1;_LLA0: return 0;_LLA1:;_LLA2: goto _LL9E;_LL9E:;}{struct
Cyc_Absyn_Aggrdecl*_tmpFB=Cyc_Absyn_get_known_aggrdecl(_tmpF3);if(_tmpFB->impl == 
0)return 0;{struct Cyc_List_List*_tmpFC=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpFB->impl))->fields;for(0;_tmpFC != 0;_tmpFC=_tmpFC->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpFC->hd)->type))return 0;}}return 1;}_LL90:
if(_tmpEF <= (void*)4  || *((int*)_tmpEF)!= 11)goto _LL92;_tmpF4=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpEF)->f2;_LL91: for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpF4->hd)->type))return 0;}return 1;_LL92: if(
_tmpEF <= (void*)4  || *((int*)_tmpEF)!= 3)goto _LL94;_tmpF5=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpEF)->f1;_tmpF6=(void*)_tmpF5.field_info;if(*((int*)_tmpF6)!= 1)goto _LL94;
_tmpF7=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpF6)->f1;_tmpF8=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpF6)->f2;_LL93: _tmpF9=_tmpF8->typs;goto _LL95;_LL94: if(_tmpEF <= (void*)4  || *((
int*)_tmpEF)!= 9)goto _LL96;_tmpF9=((struct Cyc_Absyn_TupleType_struct*)_tmpEF)->f1;
_LL95: for(0;_tmpF9 != 0;_tmpF9=_tmpF9->tl){if(!Cyc_Toc_atomic_typ((*((struct
_tuple4*)_tmpF9->hd)).f2))return 0;}return 1;_LL96: if(_tmpEF <= (void*)4  || *((int*)
_tmpEF)!= 2)goto _LL98;_LL97: goto _LL99;_LL98: if(_tmpEF <= (void*)4  || *((int*)
_tmpEF)!= 4)goto _LL9A;_LL99: goto _LL9B;_LL9A: if(_tmpEF <= (void*)4  || *((int*)
_tmpEF)!= 15)goto _LL9C;_LL9B: return 0;_LL9C:;_LL9D:({struct Cyc_String_pa_struct
_tmpFF;_tmpFF.tag=0;_tmpFF.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmpFD[1]={& _tmpFF};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmpFE="atomic_typ:  bad type %s";_tag_arr(_tmpFE,
sizeof(char),_get_zero_arr_size(_tmpFE,25));}),_tag_arr(_tmpFD,sizeof(void*),1));}});
_LL77:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp100=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp101;void*_tmp102;_LLA4: if(_tmp100 <= (void*)4  || *((
int*)_tmp100)!= 4)goto _LLA6;_tmp101=((struct Cyc_Absyn_PointerType_struct*)
_tmp100)->f1;_tmp102=(void*)_tmp101.elt_typ;_LLA5: {void*_tmp103=Cyc_Tcutil_compress(
_tmp102);_LLA9: if((int)_tmp103 != 0)goto _LLAB;_LLAA: return 1;_LLAB:;_LLAC: return 0;
_LLA8:;}_LLA6:;_LLA7: return 0;_LLA3:;}static int Cyc_Toc_is_poly_field(void*t,
struct _tagged_arr*f){void*_tmp104=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp105;void*_tmp106;struct Cyc_List_List*_tmp107;_LLAE: if(_tmp104 <= (void*)4  || *((
int*)_tmp104)!= 10)goto _LLB0;_tmp105=((struct Cyc_Absyn_AggrType_struct*)_tmp104)->f1;
_tmp106=(void*)_tmp105.aggr_info;_LLAF: {struct Cyc_Absyn_Aggrdecl*_tmp108=Cyc_Absyn_get_known_aggrdecl(
_tmp106);if(_tmp108->impl == 0)({void*_tmp109[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp10A="is_poly_field: type missing fields";
_tag_arr(_tmp10A,sizeof(char),_get_zero_arr_size(_tmp10A,35));}),_tag_arr(
_tmp109,sizeof(void*),0));});_tmp107=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp108->impl))->fields;goto _LLB1;}_LLB0: if(_tmp104 <= (void*)4  || *((int*)
_tmp104)!= 11)goto _LLB2;_tmp107=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp104)->f2;
_LLB1: {struct Cyc_Absyn_Aggrfield*_tmp10B=Cyc_Absyn_lookup_field(_tmp107,f);if(
_tmp10B == 0)({struct Cyc_String_pa_struct _tmp10E;_tmp10E.tag=0;_tmp10E.f1=(struct
_tagged_arr)((struct _tagged_arr)*f);{void*_tmp10C[1]={& _tmp10E};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp10D="is_poly_field: bad field %s";
_tag_arr(_tmp10D,sizeof(char),_get_zero_arr_size(_tmp10D,28));}),_tag_arr(
_tmp10C,sizeof(void*),1));}});return Cyc_Toc_is_void_star((void*)_tmp10B->type);}
_LLB2:;_LLB3:({struct Cyc_String_pa_struct _tmp111;_tmp111.tag=0;_tmp111.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp10F[1]={&
_tmp111};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp110="is_poly_field: bad type %s";_tag_arr(_tmp110,sizeof(char),
_get_zero_arr_size(_tmp110,27));}),_tag_arr(_tmp10F,sizeof(void*),1));}});_LLAD:;}
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp112=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp113;struct _tagged_arr*_tmp114;struct Cyc_Absyn_Exp*
_tmp115;struct _tagged_arr*_tmp116;_LLB5: if(*((int*)_tmp112)!= 23)goto _LLB7;
_tmp113=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp112)->f1;_tmp114=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp112)->f2;_LLB6: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp113->topt))->v,_tmp114);_LLB7: if(*((int*)_tmp112)!= 24)goto _LLB9;
_tmp115=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp112)->f1;_tmp116=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp112)->f2;_LLB8: {void*_tmp117=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp115->topt))->v);struct Cyc_Absyn_PtrInfo _tmp118;void*_tmp119;
_LLBC: if(_tmp117 <= (void*)4  || *((int*)_tmp117)!= 4)goto _LLBE;_tmp118=((struct
Cyc_Absyn_PointerType_struct*)_tmp117)->f1;_tmp119=(void*)_tmp118.elt_typ;_LLBD:
return Cyc_Toc_is_poly_field(_tmp119,_tmp116);_LLBE:;_LLBF:({struct Cyc_String_pa_struct
_tmp11C;_tmp11C.tag=0;_tmp11C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp115->topt))->v));{void*_tmp11A[1]={&
_tmp11C};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp11B="is_poly_project:  bad type %s";_tag_arr(_tmp11B,sizeof(char),
_get_zero_arr_size(_tmp11B,30));}),_tag_arr(_tmp11A,sizeof(void*),1));}});_LLBB:;}
_LLB9:;_LLBA: return 0;_LLB4:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct
Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*
_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->hd=s;_tmp11D->tl=0;_tmp11D;}),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmp11E=_cycalloc(sizeof(*
_tmp11E));_tmp11E->hd=s;_tmp11E->tl=0;_tmp11E;}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return
Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*
Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*_tmp11F[2];_tmp11F[1]=s;_tmp11F[0]=
rgn;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp11F,
sizeof(struct Cyc_Absyn_Exp*),2));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(
void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(
elt_type))return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*
_tmp120[2];_tmp120[1]=n;_tmp120[0]=s;((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp120,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp121[2];
_tmp121[1]=n;_tmp121[0]=s;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp121,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*
_tmp122[3];_tmp122[2]=n;_tmp122[1]=s;_tmp122[0]=rgn;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp122,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*_tmp123=
_cycalloc(sizeof(*_tmp123));_tmp123->hd=e;_tmp123->tl=0;_tmp123;}),0);}static
struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_tagged_arr(void*t,struct Cyc_Absyn_Exp*
sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*_tmp124=(void*)e->r;void*_tmp125;
_LLC1: if(*((int*)_tmp124)!= 0)goto _LLC3;_tmp125=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp124)->f1;if(_tmp125 <= (void*)1  || *((int*)_tmp125)!= 5)goto _LLC3;_LLC2:
is_string=1;goto _LLC0;_LLC3:;_LLC4: goto _LLC0;_LLC0:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127[0]=({
struct Cyc_Absyn_Var_d_struct _tmp128;_tmp128.tag=0;_tmp128.f1=vd;_tmp128;});
_tmp127;}),0);_tmp126->tl=Cyc_Toc_result_decls;_tmp126;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple6*
_tmp129[3];_tmp129[2]=({struct _tuple6*_tmp12C=_cycalloc(sizeof(*_tmp12C));
_tmp12C->f1=0;_tmp12C->f2=xplussz;_tmp12C;});_tmp129[1]=({struct _tuple6*_tmp12B=
_cycalloc(sizeof(*_tmp12B));_tmp12B->f1=0;_tmp12B->f2=xexp;_tmp12B;});_tmp129[0]=({
struct _tuple6*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->f1=0;_tmp12A->f2=xexp;
_tmp12A;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp129,sizeof(struct _tuple6*),3));}),0);return urm_exp;}}}struct Cyc_Toc_Env{
struct Cyc_Core_Opt*break_lab;struct Cyc_Core_Opt*continue_lab;struct Cyc_Core_Opt*
fallthru_info;struct Cyc_Dict_Dict*varmap;int toplevel;};static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(){return({struct Cyc_Toc_Env*_tmp12D=_cycalloc(sizeof(*_tmp12D));
_tmp12D->break_lab=0;_tmp12D->continue_lab=0;_tmp12D->fallthru_info=0;_tmp12D->varmap=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp);_tmp12D->toplevel=1;_tmp12D;});}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp12E=_cycalloc(
sizeof(*_tmp12E));_tmp12E[0]=*e;_tmp12E;});ans->toplevel=0;return ans;}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct Cyc_Toc_Env*e,struct _tuple1*x,struct
Cyc_Absyn_Exp*y){{void*_tmp12F=(*x).f1;_LLC6: if(_tmp12F <= (void*)1  || *((int*)
_tmp12F)!= 0)goto _LLC8;_LLC7:({struct Cyc_String_pa_struct _tmp132;_tmp132.tag=0;
_tmp132.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(x));{
void*_tmp130[1]={& _tmp132};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp131="Toc::add_varmap on Rel_n: %s\n";_tag_arr(
_tmp131,sizeof(char),_get_zero_arr_size(_tmp131,30));}),_tag_arr(_tmp130,sizeof(
void*),1));}});_LLC8:;_LLC9: goto _LLC5;_LLC5:;}{struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133[0]=*e;_tmp133;});ans->varmap=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,struct Cyc_Absyn_Exp*
v))Cyc_Dict_insert)(ans->varmap,x,y);return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp134=_cycalloc(
sizeof(*_tmp134));_tmp134[0]=*e;_tmp134;});ans->break_lab=0;ans->continue_lab=0;
return ans;}struct _tuple8{struct _tagged_arr*f1;struct Cyc_List_List*f2;struct Cyc_Dict_Dict*
f3;};static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct Cyc_Toc_Env*
e,struct _tagged_arr*break_l,struct _tagged_arr*fallthru_l,struct Cyc_List_List*
fallthru_binders,struct Cyc_Toc_Env*next_case_env){struct Cyc_List_List*_tmp135=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){_tmp135=({
struct Cyc_List_List*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp136->tl=_tmp135;_tmp136;});}_tmp135=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp135);{struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A[0]=*e;_tmp13A;});
ans->break_lab=({struct Cyc_Core_Opt*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->v=
break_l;_tmp137;});ans->fallthru_info=({struct Cyc_Core_Opt*_tmp138=_cycalloc(
sizeof(*_tmp138));_tmp138->v=({struct _tuple8*_tmp139=_cycalloc(sizeof(*_tmp139));
_tmp139->f1=fallthru_l;_tmp139->f2=_tmp135;_tmp139->f3=next_case_env->varmap;
_tmp139;});_tmp138;});return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*break_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C[0]=*e;_tmp13C;});ans->break_lab=({
struct Cyc_Core_Opt*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->v=break_l;
_tmp13B;});ans->fallthru_info=0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*next_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F[0]=*e;_tmp13F;});ans->break_lab=0;
ans->fallthru_info=({struct Cyc_Core_Opt*_tmp13D=_cycalloc(sizeof(*_tmp13D));
_tmp13D->v=({struct _tuple8*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->f1=
next_l;_tmp13E->f2=0;_tmp13E->f3=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp13E;});_tmp13D;});
return ans;}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static
int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp140=(void*)e->annot;
_LLCB: if(*((void**)_tmp140)!= Cyc_CfFlowInfo_UnknownZ)goto _LLCD;_LLCC: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLCD: if(*((void**)_tmp140)
!= Cyc_CfFlowInfo_NotZero)goto _LLCF;_LLCE: return 0;_LLCF: if(_tmp140 != Cyc_CfFlowInfo_IsZero)
goto _LLD1;_LLD0:({void*_tmp141[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp142="dereference of NULL pointer";
_tag_arr(_tmp142,sizeof(char),_get_zero_arr_size(_tmp142,28));}),_tag_arr(
_tmp141,sizeof(void*),0));});return 0;_LLD1: if(_tmp140 != Cyc_Absyn_EmptyAnnot)
goto _LLD3;_LLD2: return 0;_LLD3: if(*((void**)_tmp140)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LLD5;_LLD4:({void*_tmp143[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp144="compiler oddity: pointer compared to tag type";
_tag_arr(_tmp144,sizeof(char),_get_zero_arr_size(_tmp144,46));}),_tag_arr(
_tmp143,sizeof(void*),0));});return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLD5:;_LLD6:({void*_tmp145[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp146="need_null_check";
_tag_arr(_tmp146,sizeof(char),_get_zero_arr_size(_tmp146,16));}),_tag_arr(
_tmp145,sizeof(void*),0));});_LLCA:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e){void*_tmp147=(void*)e->annot;struct Cyc_List_List*_tmp148;
struct Cyc_List_List*_tmp149;_LLD8: if(*((void**)_tmp147)!= Cyc_CfFlowInfo_UnknownZ)
goto _LLDA;_tmp148=((struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp147)->f1;_LLD9:
return _tmp148;_LLDA: if(*((void**)_tmp147)!= Cyc_CfFlowInfo_NotZero)goto _LLDC;
_tmp149=((struct Cyc_CfFlowInfo_NotZero_struct*)_tmp147)->f1;_LLDB: return _tmp149;
_LLDC: if(_tmp147 != Cyc_CfFlowInfo_IsZero)goto _LLDE;_LLDD:({void*_tmp14A[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp14B="dereference of NULL pointer";
_tag_arr(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,28));}),_tag_arr(
_tmp14A,sizeof(void*),0));});return 0;_LLDE: if(*((void**)_tmp147)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LLE0;_LLDF: goto _LLE1;_LLE0: if(_tmp147 != Cyc_Absyn_EmptyAnnot)goto _LLE2;
_LLE1: return 0;_LLE2:;_LLE3:({void*_tmp14C[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp14D="get_relns";_tag_arr(
_tmp14D,sizeof(char),_get_zero_arr_size(_tmp14D,10));}),_tag_arr(_tmp14C,sizeof(
void*),0));});_LLD7:;}static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp14E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp14F;struct Cyc_Absyn_PtrAtts
_tmp150;struct Cyc_Absyn_Conref*_tmp151;struct Cyc_Absyn_Conref*_tmp152;struct Cyc_Absyn_ArrayInfo
_tmp153;struct Cyc_Absyn_Exp*_tmp154;_LLE5: if(_tmp14E <= (void*)4  || *((int*)
_tmp14E)!= 4)goto _LLE7;_tmp14F=((struct Cyc_Absyn_PointerType_struct*)_tmp14E)->f1;
_tmp150=_tmp14F.ptr_atts;_tmp151=_tmp150.bounds;_tmp152=_tmp150.zero_term;_LLE6: {
void*_tmp155=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp151);struct Cyc_Absyn_Exp*
_tmp156;_LLEC: if((int)_tmp155 != 0)goto _LLEE;_LLED: return 0;_LLEE: if(_tmp155 <= (
void*)1  || *((int*)_tmp155)!= 0)goto _LLF0;_tmp156=((struct Cyc_Absyn_Upper_b_struct*)
_tmp155)->f1;_LLEF: {unsigned int _tmp158;int _tmp159;struct _tuple5 _tmp157=Cyc_Evexp_eval_const_uint_exp(
_tmp156);_tmp158=_tmp157.f1;_tmp159=_tmp157.f2;return _tmp159  && i <= _tmp158;}
_LLF0: if(_tmp155 <= (void*)1  || *((int*)_tmp155)!= 1)goto _LLEB;_LLF1:({void*
_tmp15A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp15B="check_const_array: AbsUpper_b";_tag_arr(_tmp15B,sizeof(char),
_get_zero_arr_size(_tmp15B,30));}),_tag_arr(_tmp15A,sizeof(void*),0));});_LLEB:;}
_LLE7: if(_tmp14E <= (void*)4  || *((int*)_tmp14E)!= 7)goto _LLE9;_tmp153=((struct
Cyc_Absyn_ArrayType_struct*)_tmp14E)->f1;_tmp154=_tmp153.num_elts;_LLE8: if(
_tmp154 == 0)return 0;{unsigned int _tmp15D;int _tmp15E;struct _tuple5 _tmp15C=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp154);_tmp15D=_tmp15C.f1;_tmp15E=_tmp15C.f2;return
_tmp15E  && i <= _tmp15D;}_LLE9:;_LLEA: return 0;_LLE4:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp15F=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp15F->vd != y)continue;{void*_tmp160=(void*)_tmp15F->rop;struct
Cyc_Absyn_Vardecl*_tmp161;struct Cyc_Absyn_Vardecl*_tmp162;_LLF3: if(*((int*)
_tmp160)!= 2)goto _LLF5;_tmp161=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp160)->f1;
_LLF4: _tmp162=_tmp161;goto _LLF6;_LLF5: if(*((int*)_tmp160)!= 4)goto _LLF7;_tmp162=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp160)->f1;_LLF6: if(_tmp162 == v)return
1;else{goto _LLF2;}_LLF7:;_LLF8: continue;_LLF2:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*
_tmp163=(void*)e->r;void*_tmp164;struct Cyc_Absyn_Vardecl*_tmp165;void*_tmp166;
struct Cyc_Absyn_Vardecl*_tmp167;void*_tmp168;struct Cyc_Absyn_Vardecl*_tmp169;
void*_tmp16A;struct Cyc_Absyn_Vardecl*_tmp16B;void*_tmp16C;struct Cyc_List_List*
_tmp16D;struct Cyc_List_List _tmp16E;struct Cyc_Absyn_Exp*_tmp16F;_LLFA: if(*((int*)
_tmp163)!= 1)goto _LLFC;_tmp164=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp163)->f2;
if(_tmp164 <= (void*)1  || *((int*)_tmp164)!= 4)goto _LLFC;_tmp165=((struct Cyc_Absyn_Pat_b_struct*)
_tmp164)->f1;_LLFB: _tmp167=_tmp165;goto _LLFD;_LLFC: if(*((int*)_tmp163)!= 1)goto
_LLFE;_tmp166=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp163)->f2;if(_tmp166 <= (
void*)1  || *((int*)_tmp166)!= 3)goto _LLFE;_tmp167=((struct Cyc_Absyn_Local_b_struct*)
_tmp166)->f1;_LLFD: _tmp169=_tmp167;goto _LLFF;_LLFE: if(*((int*)_tmp163)!= 1)goto
_LL100;_tmp168=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp163)->f2;if(_tmp168 <= (
void*)1  || *((int*)_tmp168)!= 0)goto _LL100;_tmp169=((struct Cyc_Absyn_Global_b_struct*)
_tmp168)->f1;_LLFF: _tmp16B=_tmp169;goto _LL101;_LL100: if(*((int*)_tmp163)!= 1)
goto _LL102;_tmp16A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp163)->f2;if(
_tmp16A <= (void*)1  || *((int*)_tmp16A)!= 2)goto _LL102;_tmp16B=((struct Cyc_Absyn_Param_b_struct*)
_tmp16A)->f1;_LL101: if(_tmp16B->escapes)return 0;if(Cyc_Toc_check_leq_size_var(
relns,v,_tmp16B))return 1;goto _LLF9;_LL102: if(*((int*)_tmp163)!= 3)goto _LL104;
_tmp16C=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp163)->f1;if((int)_tmp16C != 
19)goto _LL104;_tmp16D=((struct Cyc_Absyn_Primop_e_struct*)_tmp163)->f2;if(_tmp16D
== 0)goto _LL104;_tmp16E=*_tmp16D;_tmp16F=(struct Cyc_Absyn_Exp*)_tmp16E.hd;_LL103:{
void*_tmp170=(void*)_tmp16F->r;void*_tmp171;struct Cyc_Absyn_Vardecl*_tmp172;void*
_tmp173;struct Cyc_Absyn_Vardecl*_tmp174;void*_tmp175;struct Cyc_Absyn_Vardecl*
_tmp176;void*_tmp177;struct Cyc_Absyn_Vardecl*_tmp178;_LL107: if(*((int*)_tmp170)
!= 1)goto _LL109;_tmp171=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp170)->f2;if(
_tmp171 <= (void*)1  || *((int*)_tmp171)!= 4)goto _LL109;_tmp172=((struct Cyc_Absyn_Pat_b_struct*)
_tmp171)->f1;_LL108: _tmp174=_tmp172;goto _LL10A;_LL109: if(*((int*)_tmp170)!= 1)
goto _LL10B;_tmp173=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp170)->f2;if(
_tmp173 <= (void*)1  || *((int*)_tmp173)!= 3)goto _LL10B;_tmp174=((struct Cyc_Absyn_Local_b_struct*)
_tmp173)->f1;_LL10A: _tmp176=_tmp174;goto _LL10C;_LL10B: if(*((int*)_tmp170)!= 1)
goto _LL10D;_tmp175=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp170)->f2;if(
_tmp175 <= (void*)1  || *((int*)_tmp175)!= 0)goto _LL10D;_tmp176=((struct Cyc_Absyn_Global_b_struct*)
_tmp175)->f1;_LL10C: _tmp178=_tmp176;goto _LL10E;_LL10D: if(*((int*)_tmp170)!= 1)
goto _LL10F;_tmp177=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp170)->f2;if(
_tmp177 <= (void*)1  || *((int*)_tmp177)!= 2)goto _LL10F;_tmp178=((struct Cyc_Absyn_Param_b_struct*)
_tmp177)->f1;_LL10E: return _tmp178 == v;_LL10F:;_LL110: goto _LL106;_LL106:;}goto
_LLF9;_LL104:;_LL105: goto _LLF9;_LLF9:;}return 0;}static int Cyc_Toc_check_bounds(
struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*
_tmp179=(void*)a->r;void*_tmp17A;struct Cyc_Absyn_Vardecl*_tmp17B;void*_tmp17C;
struct Cyc_Absyn_Vardecl*_tmp17D;void*_tmp17E;struct Cyc_Absyn_Vardecl*_tmp17F;
void*_tmp180;struct Cyc_Absyn_Vardecl*_tmp181;_LL112: if(*((int*)_tmp179)!= 1)goto
_LL114;_tmp17A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp179)->f2;if(_tmp17A <= (
void*)1  || *((int*)_tmp17A)!= 4)goto _LL114;_tmp17B=((struct Cyc_Absyn_Pat_b_struct*)
_tmp17A)->f1;_LL113: _tmp17D=_tmp17B;goto _LL115;_LL114: if(*((int*)_tmp179)!= 1)
goto _LL116;_tmp17C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp179)->f2;if(
_tmp17C <= (void*)1  || *((int*)_tmp17C)!= 3)goto _LL116;_tmp17D=((struct Cyc_Absyn_Local_b_struct*)
_tmp17C)->f1;_LL115: _tmp17F=_tmp17D;goto _LL117;_LL116: if(*((int*)_tmp179)!= 1)
goto _LL118;_tmp17E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp179)->f2;if(
_tmp17E <= (void*)1  || *((int*)_tmp17E)!= 0)goto _LL118;_tmp17F=((struct Cyc_Absyn_Global_b_struct*)
_tmp17E)->f1;_LL117: _tmp181=_tmp17F;goto _LL119;_LL118: if(*((int*)_tmp179)!= 1)
goto _LL11A;_tmp180=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp179)->f2;if(
_tmp180 <= (void*)1  || *((int*)_tmp180)!= 2)goto _LL11A;_tmp181=((struct Cyc_Absyn_Param_b_struct*)
_tmp180)->f1;_LL119: if(_tmp181->escapes)return 0;inner_loop: {void*_tmp182=(void*)
i->r;void*_tmp183;struct Cyc_Absyn_Exp*_tmp184;void*_tmp185;void*_tmp186;int
_tmp187;void*_tmp188;void*_tmp189;int _tmp18A;void*_tmp18B;void*_tmp18C;int
_tmp18D;void*_tmp18E;struct Cyc_List_List*_tmp18F;struct Cyc_List_List _tmp190;
struct Cyc_Absyn_Exp*_tmp191;struct Cyc_List_List*_tmp192;struct Cyc_List_List
_tmp193;struct Cyc_Absyn_Exp*_tmp194;void*_tmp195;struct Cyc_Absyn_Vardecl*_tmp196;
void*_tmp197;struct Cyc_Absyn_Vardecl*_tmp198;void*_tmp199;struct Cyc_Absyn_Vardecl*
_tmp19A;void*_tmp19B;struct Cyc_Absyn_Vardecl*_tmp19C;_LL11D: if(*((int*)_tmp182)
!= 15)goto _LL11F;_tmp183=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp182)->f1;
_tmp184=((struct Cyc_Absyn_Cast_e_struct*)_tmp182)->f2;_LL11E: i=_tmp184;goto
inner_loop;_LL11F: if(*((int*)_tmp182)!= 0)goto _LL121;_tmp185=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp182)->f1;if(_tmp185 <= (void*)1  || *((int*)_tmp185)!= 2)goto _LL121;_tmp186=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp185)->f1;if((int)_tmp186 != 2)goto
_LL121;_tmp187=((struct Cyc_Absyn_Int_c_struct*)_tmp185)->f2;_LL120: _tmp18A=
_tmp187;goto _LL122;_LL121: if(*((int*)_tmp182)!= 0)goto _LL123;_tmp188=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp182)->f1;if(_tmp188 <= (void*)1  || *((int*)
_tmp188)!= 2)goto _LL123;_tmp189=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp188)->f1;
if((int)_tmp189 != 0)goto _LL123;_tmp18A=((struct Cyc_Absyn_Int_c_struct*)_tmp188)->f2;
_LL122: return _tmp18A >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp18A + 1),(
void*)_tmp181->type);_LL123: if(*((int*)_tmp182)!= 0)goto _LL125;_tmp18B=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp182)->f1;if(_tmp18B <= (void*)1  || *((int*)
_tmp18B)!= 2)goto _LL125;_tmp18C=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp18B)->f1;
if((int)_tmp18C != 1)goto _LL125;_tmp18D=((struct Cyc_Absyn_Int_c_struct*)_tmp18B)->f2;
_LL124: return Cyc_Toc_check_const_array((unsigned int)(_tmp18D + 1),(void*)_tmp181->type);
_LL125: if(*((int*)_tmp182)!= 3)goto _LL127;_tmp18E=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp182)->f1;if((int)_tmp18E != 4)goto _LL127;_tmp18F=((struct Cyc_Absyn_Primop_e_struct*)
_tmp182)->f2;if(_tmp18F == 0)goto _LL127;_tmp190=*_tmp18F;_tmp191=(struct Cyc_Absyn_Exp*)
_tmp190.hd;_tmp192=_tmp190.tl;if(_tmp192 == 0)goto _LL127;_tmp193=*_tmp192;_tmp194=(
struct Cyc_Absyn_Exp*)_tmp193.hd;_LL126: return Cyc_Toc_check_leq_size(relns,
_tmp181,_tmp194);_LL127: if(*((int*)_tmp182)!= 1)goto _LL129;_tmp195=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp182)->f2;if(_tmp195 <= (void*)1  || *((int*)
_tmp195)!= 4)goto _LL129;_tmp196=((struct Cyc_Absyn_Pat_b_struct*)_tmp195)->f1;
_LL128: _tmp198=_tmp196;goto _LL12A;_LL129: if(*((int*)_tmp182)!= 1)goto _LL12B;
_tmp197=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp182)->f2;if(_tmp197 <= (void*)
1  || *((int*)_tmp197)!= 3)goto _LL12B;_tmp198=((struct Cyc_Absyn_Local_b_struct*)
_tmp197)->f1;_LL12A: _tmp19A=_tmp198;goto _LL12C;_LL12B: if(*((int*)_tmp182)!= 1)
goto _LL12D;_tmp199=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp182)->f2;if(
_tmp199 <= (void*)1  || *((int*)_tmp199)!= 0)goto _LL12D;_tmp19A=((struct Cyc_Absyn_Global_b_struct*)
_tmp199)->f1;_LL12C: _tmp19C=_tmp19A;goto _LL12E;_LL12D: if(*((int*)_tmp182)!= 1)
goto _LL12F;_tmp19B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp182)->f2;if(
_tmp19B <= (void*)1  || *((int*)_tmp19B)!= 2)goto _LL12F;_tmp19C=((struct Cyc_Absyn_Param_b_struct*)
_tmp19B)->f1;_LL12E: if(_tmp19C->escapes)return 0;{struct Cyc_List_List*_tmp19D=
relns;for(0;_tmp19D != 0;_tmp19D=_tmp19D->tl){struct Cyc_CfFlowInfo_Reln*_tmp19E=(
struct Cyc_CfFlowInfo_Reln*)_tmp19D->hd;if(_tmp19E->vd == _tmp19C){void*_tmp19F=(
void*)_tmp19E->rop;struct Cyc_Absyn_Vardecl*_tmp1A0;struct Cyc_Absyn_Vardecl*
_tmp1A1;unsigned int _tmp1A2;_LL132: if(*((int*)_tmp19F)!= 2)goto _LL134;_tmp1A0=((
struct Cyc_CfFlowInfo_LessSize_struct*)_tmp19F)->f1;_LL133: if(_tmp181 == _tmp1A0)
return 1;else{goto _LL131;}_LL134: if(*((int*)_tmp19F)!= 1)goto _LL136;_tmp1A1=((
struct Cyc_CfFlowInfo_LessVar_struct*)_tmp19F)->f1;_LL135:{struct Cyc_List_List*
_tmp1A3=relns;for(0;_tmp1A3 != 0;_tmp1A3=_tmp1A3->tl){struct Cyc_CfFlowInfo_Reln*
_tmp1A4=(struct Cyc_CfFlowInfo_Reln*)_tmp1A3->hd;if(_tmp1A4->vd == _tmp1A1){void*
_tmp1A5=(void*)_tmp1A4->rop;struct Cyc_Absyn_Vardecl*_tmp1A6;struct Cyc_Absyn_Vardecl*
_tmp1A7;unsigned int _tmp1A8;struct Cyc_Absyn_Vardecl*_tmp1A9;_LL13B: if(*((int*)
_tmp1A5)!= 4)goto _LL13D;_tmp1A6=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp1A5)->f1;_LL13C: _tmp1A7=_tmp1A6;goto _LL13E;_LL13D: if(*((int*)_tmp1A5)!= 2)
goto _LL13F;_tmp1A7=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1A5)->f1;_LL13E:
if(_tmp181 == _tmp1A7)return 1;goto _LL13A;_LL13F: if(*((int*)_tmp1A5)!= 0)goto
_LL141;_tmp1A8=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp1A5)->f1;_LL140:
return Cyc_Toc_check_const_array(_tmp1A8,(void*)_tmp181->type);_LL141: if(*((int*)
_tmp1A5)!= 1)goto _LL143;_tmp1A9=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1A5)->f1;
_LL142: if(Cyc_Toc_check_leq_size_var(relns,_tmp181,_tmp1A9))return 1;goto _LL13A;
_LL143:;_LL144: goto _LL13A;_LL13A:;}}}goto _LL131;_LL136: if(*((int*)_tmp19F)!= 3)
goto _LL138;_tmp1A2=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp19F)->f1;_LL137:
return Cyc_Toc_check_const_array(_tmp1A2,(void*)_tmp181->type);_LL138:;_LL139:
goto _LL131;_LL131:;}}}goto _LL11C;_LL12F:;_LL130: goto _LL11C;_LL11C:;}goto _LL111;
_LL11A:;_LL11B: goto _LL111;_LL111:;}return 0;}static void*Cyc_Toc_get_c_typ(struct
Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp1AA[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1AB="Missing type in primop ";
_tag_arr(_tmp1AB,sizeof(char),_get_zero_arr_size(_tmp1AB,24));}),_tag_arr(
_tmp1AA,sizeof(void*),0));});return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp1AC[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1AD="Missing type in primop ";
_tag_arr(_tmp1AD,sizeof(char),_get_zero_arr_size(_tmp1AD,24));}),_tag_arr(
_tmp1AC,sizeof(void*),0));});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static struct _tuple4*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct
_tuple4*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));_tmp1AE->f1=Cyc_Toc_mt_tq;_tmp1AE->f2=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp1AE;});}
static struct _tuple6*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){Cyc_Toc_exp_to_c(nv,e);return({struct _tuple6*_tmp1AF=_cycalloc(sizeof(*
_tmp1AF));_tmp1AF->f1=0;_tmp1AF->f2=e;_tmp1AF;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp1B0=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp1B0):
Cyc_Toc_malloc_ptr(_tmp1B0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp1B0);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp1B1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp1B1 != 0;_tmp1B1=_tmp1B1->tl){struct _tuple6 _tmp1B3;struct Cyc_List_List*
_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B5;struct _tuple6*_tmp1B2=(struct _tuple6*)
_tmp1B1->hd;_tmp1B3=*_tmp1B2;_tmp1B4=_tmp1B3.f1;_tmp1B5=_tmp1B3.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp1B4 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp1B4->tl != 0)({void*_tmp1B6[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp1B7="multiple designators in array";
_tag_arr(_tmp1B7,sizeof(char),_get_zero_arr_size(_tmp1B7,30));}),_tag_arr(
_tmp1B6,sizeof(void*),0));});{void*_tmp1B8=(void*)_tmp1B4->hd;void*_tmp1B9=
_tmp1B8;struct Cyc_Absyn_Exp*_tmp1BA;_LL146: if(*((int*)_tmp1B9)!= 0)goto _LL148;
_tmp1BA=((struct Cyc_Absyn_ArrayElement_struct*)_tmp1B9)->f1;_LL147: Cyc_Toc_exp_to_c(
nv,_tmp1BA);e_index=_tmp1BA;goto _LL145;_LL148: if(*((int*)_tmp1B9)!= 1)goto _LL145;
_LL149:({void*_tmp1BB[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_unimp)(({const char*_tmp1BC="field name designators in array";_tag_arr(
_tmp1BC,sizeof(char),_get_zero_arr_size(_tmp1BC,32));}),_tag_arr(_tmp1BB,sizeof(
void*),0));});_LL145:;}}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,
e_index,0);void*_tmp1BD=(void*)_tmp1B5->r;struct Cyc_List_List*_tmp1BE;struct Cyc_Absyn_Vardecl*
_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1C1;int _tmp1C2;void*
_tmp1C3;struct Cyc_List_List*_tmp1C4;_LL14B: if(*((int*)_tmp1BD)!= 28)goto _LL14D;
_tmp1BE=((struct Cyc_Absyn_Array_e_struct*)_tmp1BD)->f1;_LL14C: s=Cyc_Toc_init_array(
nv,lval,_tmp1BE,s);goto _LL14A;_LL14D: if(*((int*)_tmp1BD)!= 29)goto _LL14F;_tmp1BF=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1BD)->f1;_tmp1C0=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1BD)->f2;_tmp1C1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1BD)->f3;
_tmp1C2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1BD)->f4;_LL14E: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp1BF,_tmp1C0,_tmp1C1,_tmp1C2,s,0);goto _LL14A;_LL14F: if(*((int*)
_tmp1BD)!= 31)goto _LL151;_tmp1C3=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1BD)->f1;_tmp1C4=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1BD)->f2;_LL150:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp1C3,_tmp1C4,s);goto _LL14A;_LL151:;_LL152:
Cyc_Toc_exp_to_c(nv,_tmp1B5);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp1B5,0),s,0);goto _LL14A;_LL14A:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp1C5=vd->name;
void*_tmp1C6=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
nv,_tmp1C5,Cyc_Absyn_varb_exp(_tmp1C5,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF[0]=({struct Cyc_Absyn_Local_b_struct
_tmp1D0;_tmp1D0.tag=3;_tmp1D0.f1=vd;_tmp1D0;});_tmp1CF;}),0));struct _tuple1*max=
Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(
_tmp1C5,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp(_tmp1C5,0),Cyc_Absyn_var_exp(max,0),0);struct Cyc_Absyn_Exp*ec=
Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp1C5,0),0);struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp1C5,0),0);struct Cyc_Absyn_Stmt*
body;{void*_tmp1C7=(void*)e2->r;struct Cyc_List_List*_tmp1C8;struct Cyc_Absyn_Vardecl*
_tmp1C9;struct Cyc_Absyn_Exp*_tmp1CA;struct Cyc_Absyn_Exp*_tmp1CB;int _tmp1CC;void*
_tmp1CD;struct Cyc_List_List*_tmp1CE;_LL154: if(*((int*)_tmp1C7)!= 28)goto _LL156;
_tmp1C8=((struct Cyc_Absyn_Array_e_struct*)_tmp1C7)->f1;_LL155: body=Cyc_Toc_init_array(
nv2,lval,_tmp1C8,Cyc_Toc_skip_stmt_dl());goto _LL153;_LL156: if(*((int*)_tmp1C7)!= 
29)goto _LL158;_tmp1C9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1C7)->f1;
_tmp1CA=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1C7)->f2;_tmp1CB=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1C7)->f3;_tmp1CC=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1C7)->f4;_LL157: body=Cyc_Toc_init_comprehension(nv2,lval,_tmp1C9,_tmp1CA,
_tmp1CB,_tmp1CC,Cyc_Toc_skip_stmt_dl(),0);goto _LL153;_LL158: if(*((int*)_tmp1C7)
!= 31)goto _LL15A;_tmp1CD=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1C7)->f1;
_tmp1CE=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1C7)->f2;_LL159: body=Cyc_Toc_init_anon_struct(
nv,lval,_tmp1CD,_tmp1CE,Cyc_Toc_skip_stmt_dl());goto _LL153;_LL15A:;_LL15B: Cyc_Toc_exp_to_c(
nv2,e2);body=Cyc_Absyn_assign_stmt(lval,e2,0);goto _LL153;_LL153:;}{struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp1C6,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp1C5,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp1D1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp1D1 != 0;_tmp1D1=_tmp1D1->tl){struct _tuple6 _tmp1D3;
struct Cyc_List_List*_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D5;struct _tuple6*_tmp1D2=(
struct _tuple6*)_tmp1D1->hd;_tmp1D3=*_tmp1D2;_tmp1D4=_tmp1D3.f1;_tmp1D5=_tmp1D3.f2;
if(_tmp1D4 == 0)({void*_tmp1D6[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1D7="empty designator list";
_tag_arr(_tmp1D7,sizeof(char),_get_zero_arr_size(_tmp1D7,22));}),_tag_arr(
_tmp1D6,sizeof(void*),0));});if(_tmp1D4->tl != 0)({void*_tmp1D8[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1D9="too many designators in anonymous struct";
_tag_arr(_tmp1D9,sizeof(char),_get_zero_arr_size(_tmp1D9,41));}),_tag_arr(
_tmp1D8,sizeof(void*),0));});{void*_tmp1DA=(void*)_tmp1D4->hd;struct _tagged_arr*
_tmp1DB;_LL15D: if(*((int*)_tmp1DA)!= 1)goto _LL15F;_tmp1DB=((struct Cyc_Absyn_FieldName_struct*)
_tmp1DA)->f1;_LL15E: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,
_tmp1DB,0);{void*_tmp1DC=(void*)_tmp1D5->r;struct Cyc_List_List*_tmp1DD;struct Cyc_Absyn_Vardecl*
_tmp1DE;struct Cyc_Absyn_Exp*_tmp1DF;struct Cyc_Absyn_Exp*_tmp1E0;int _tmp1E1;void*
_tmp1E2;struct Cyc_List_List*_tmp1E3;_LL162: if(*((int*)_tmp1DC)!= 28)goto _LL164;
_tmp1DD=((struct Cyc_Absyn_Array_e_struct*)_tmp1DC)->f1;_LL163: s=Cyc_Toc_init_array(
nv,lval,_tmp1DD,s);goto _LL161;_LL164: if(*((int*)_tmp1DC)!= 29)goto _LL166;_tmp1DE=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DC)->f1;_tmp1DF=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1DC)->f2;_tmp1E0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DC)->f3;
_tmp1E1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DC)->f4;_LL165: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp1DE,_tmp1DF,_tmp1E0,_tmp1E1,s,0);goto _LL161;_LL166: if(*((int*)
_tmp1DC)!= 31)goto _LL168;_tmp1E2=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1DC)->f1;_tmp1E3=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1DC)->f2;_LL167:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp1E2,_tmp1E3,s);goto _LL161;_LL168:;_LL169:
Cyc_Toc_exp_to_c(nv,_tmp1D5);if(Cyc_Toc_is_poly_field(struct_type,_tmp1DB))
_tmp1D5=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp1D5);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp1D5,0),0),s,0);goto _LL161;
_LL161:;}goto _LL15C;}_LL15F:;_LL160:({void*_tmp1E4[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1E5="array designator in struct";
_tag_arr(_tmp1E5,sizeof(char),_get_zero_arr_size(_tmp1E5,27));}),_tag_arr(
_tmp1E4,sizeof(void*),0));});_LL15C:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(
struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
es){struct _RegionHandle _tmp1E6=_new_region("r");struct _RegionHandle*r=& _tmp1E6;
_push_region(r);{struct Cyc_List_List*_tmp1E7=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);struct _tagged_arr*_tmp1E8=Cyc_Toc_add_tuple_type(
_tmp1E7);struct _tuple1*_tmp1E9=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1EA=
Cyc_Absyn_var_exp(_tmp1E9,0);struct Cyc_Absyn_Stmt*_tmp1EB=Cyc_Absyn_exp_stmt(
_tmp1EA,0);struct Cyc_Absyn_Exp*(*_tmp1EC)(struct Cyc_Absyn_Exp*,struct _tagged_arr*,
struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp1ED=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp1ED);for(0;_tmp1ED != 0;(_tmp1ED=_tmp1ED->tl,
-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp1ED->hd;struct Cyc_Absyn_Exp*
lval=_tmp1EC(_tmp1EA,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic  && Cyc_Toc_atomic_typ((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1EE=(void*)e->r;
struct Cyc_List_List*_tmp1EF;struct Cyc_Absyn_Vardecl*_tmp1F0;struct Cyc_Absyn_Exp*
_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;int _tmp1F3;_LL16B: if(*((int*)_tmp1EE)!= 28)
goto _LL16D;_tmp1EF=((struct Cyc_Absyn_Array_e_struct*)_tmp1EE)->f1;_LL16C: _tmp1EB=
Cyc_Toc_init_array(nv,lval,_tmp1EF,_tmp1EB);goto _LL16A;_LL16D: if(*((int*)_tmp1EE)
!= 29)goto _LL16F;_tmp1F0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1EE)->f1;
_tmp1F1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1EE)->f2;_tmp1F2=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1EE)->f3;_tmp1F3=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1EE)->f4;_LL16E: _tmp1EB=Cyc_Toc_init_comprehension(nv,lval,_tmp1F0,_tmp1F1,
_tmp1F2,_tmp1F3,_tmp1EB,0);goto _LL16A;_LL16F:;_LL170: Cyc_Toc_exp_to_c(nv,e);
_tmp1EB=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp1EC(
_tmp1EA,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp1EB,0);goto _LL16A;_LL16A:;}}}{
struct Cyc_Absyn_Exp*_tmp1F4=Cyc_Toc_make_struct(nv,_tmp1E9,Cyc_Absyn_strct(
_tmp1E8),_tmp1EB,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp1F4;}};
_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct
Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp1F5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp1F6=Cyc_Absyn_var_exp(_tmp1F5,0);struct Cyc_Absyn_Stmt*
_tmp1F7=Cyc_Absyn_exp_stmt(_tmp1F6,0);struct Cyc_Absyn_Exp*(*_tmp1F8)(struct Cyc_Absyn_Exp*,
struct _tagged_arr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp1F9=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp1FA;void*
_tmp1FB;_LL172: if(_tmp1F9 <= (void*)4  || *((int*)_tmp1F9)!= 10)goto _LL174;_tmp1FA=((
struct Cyc_Absyn_AggrType_struct*)_tmp1F9)->f1;_tmp1FB=(void*)_tmp1FA.aggr_info;
_LL173: ad=Cyc_Absyn_get_known_aggrdecl(_tmp1FB);goto _LL171;_LL174:;_LL175:({void*
_tmp1FC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1FD="init_struct: bad struct type";_tag_arr(_tmp1FD,sizeof(char),
_get_zero_arr_size(_tmp1FD,29));}),_tag_arr(_tmp1FC,sizeof(void*),0));});_LL171:;}{
struct _RegionHandle _tmp1FE=_new_region("r");struct _RegionHandle*r=& _tmp1FE;
_push_region(r);{struct Cyc_List_List*_tmp1FF=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp1FF != 0;
_tmp1FF=_tmp1FF->tl){struct _tuple6 _tmp201;struct Cyc_List_List*_tmp202;struct Cyc_Absyn_Exp*
_tmp203;struct _tuple6*_tmp200=(struct _tuple6*)_tmp1FF->hd;_tmp201=*_tmp200;
_tmp202=_tmp201.f1;_tmp203=_tmp201.f2;is_atomic=is_atomic  && Cyc_Toc_atomic_typ((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp203->topt))->v);if(_tmp202 == 0)({
void*_tmp204[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp205="empty designator list";_tag_arr(_tmp205,sizeof(char),
_get_zero_arr_size(_tmp205,22));}),_tag_arr(_tmp204,sizeof(void*),0));});if(
_tmp202->tl != 0){struct _tuple1*_tmp206=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp207=Cyc_Absyn_var_exp(_tmp206,0);for(0;_tmp202 != 0;_tmp202=_tmp202->tl){void*
_tmp208=(void*)_tmp202->hd;struct _tagged_arr*_tmp209;_LL177: if(*((int*)_tmp208)
!= 1)goto _LL179;_tmp209=((struct Cyc_Absyn_FieldName_struct*)_tmp208)->f1;_LL178:
if(Cyc_Toc_is_poly_field(struct_type,_tmp209))_tmp207=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp207);_tmp1F7=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp1F8(_tmp1F6,_tmp209,0),_tmp207,0),0),_tmp1F7,0);goto _LL176;_LL179:;_LL17A:({
void*_tmp20A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp20B="array designator in struct";_tag_arr(_tmp20B,sizeof(char),
_get_zero_arr_size(_tmp20B,27));}),_tag_arr(_tmp20A,sizeof(void*),0));});_LL176:;}
Cyc_Toc_exp_to_c(nv,_tmp203);_tmp1F7=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp207,_tmp203,0),0),_tmp1F7,0);}else{void*_tmp20C=(void*)_tmp202->hd;struct
_tagged_arr*_tmp20D;_LL17C: if(*((int*)_tmp20C)!= 1)goto _LL17E;_tmp20D=((struct
Cyc_Absyn_FieldName_struct*)_tmp20C)->f1;_LL17D: {struct Cyc_Absyn_Exp*lval=
_tmp1F8(_tmp1F6,_tmp20D,0);{void*_tmp20E=(void*)_tmp203->r;struct Cyc_List_List*
_tmp20F;struct Cyc_Absyn_Vardecl*_tmp210;struct Cyc_Absyn_Exp*_tmp211;struct Cyc_Absyn_Exp*
_tmp212;int _tmp213;void*_tmp214;struct Cyc_List_List*_tmp215;_LL181: if(*((int*)
_tmp20E)!= 28)goto _LL183;_tmp20F=((struct Cyc_Absyn_Array_e_struct*)_tmp20E)->f1;
_LL182: _tmp1F7=Cyc_Toc_init_array(nv,lval,_tmp20F,_tmp1F7);goto _LL180;_LL183: if(*((
int*)_tmp20E)!= 29)goto _LL185;_tmp210=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp20E)->f1;_tmp211=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp20E)->f2;
_tmp212=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp20E)->f3;_tmp213=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp20E)->f4;_LL184: _tmp1F7=Cyc_Toc_init_comprehension(
nv,lval,_tmp210,_tmp211,_tmp212,_tmp213,_tmp1F7,0);goto _LL180;_LL185: if(*((int*)
_tmp20E)!= 31)goto _LL187;_tmp214=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp20E)->f1;_tmp215=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp20E)->f2;_LL186:
_tmp1F7=Cyc_Toc_init_anon_struct(nv,lval,_tmp214,_tmp215,_tmp1F7);goto _LL180;
_LL187:;_LL188: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp203->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp203);if(Cyc_Toc_is_poly_field(
struct_type,_tmp20D) && !was_ptr_type)_tmp203=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp203);if(has_exists)_tmp203=Cyc_Toc_cast_it((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(Cyc_Absyn_lookup_decl_field(ad,_tmp20D)))->type,_tmp203);_tmp1F7=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp203,0),0),_tmp1F7,0);goto _LL180;}
_LL180:;}goto _LL17B;}_LL17E:;_LL17F:({void*_tmp216[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp217="array designator in struct";
_tag_arr(_tmp217,sizeof(char),_get_zero_arr_size(_tmp217,27));}),_tag_arr(
_tmp216,sizeof(void*),0));});_LL17B:;}}}{struct Cyc_Absyn_Exp*_tmp218=Cyc_Toc_make_struct(
nv,_tmp1F5,Cyc_Absyn_strctq(tdn),_tmp1F7,pointer,rgnopt,is_atomic);_npop_handler(
0);return _tmp218;};_pop_region(r);}}struct _tuple9{struct Cyc_Core_Opt*f1;struct
Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*
el,struct _tuple9*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int
ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmp219=_cycalloc(sizeof(*_tmp219));_tmp219[0]=({struct Cyc_Absyn_Increment_e_struct
_tmp21A;_tmp21A.tag=5;_tmp21A.f1=e1;_tmp21A.f2=(void*)incr;_tmp21A;});_tmp219;}),
0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp21B=(void*)e1->r;struct Cyc_Absyn_Stmt*
_tmp21C;void*_tmp21D;struct Cyc_Absyn_Exp*_tmp21E;struct Cyc_Absyn_Exp*_tmp21F;
struct _tagged_arr*_tmp220;_LL18A: if(*((int*)_tmp21B)!= 37)goto _LL18C;_tmp21C=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp21B)->f1;_LL18B: Cyc_Toc_lvalue_assign_stmt(
_tmp21C,fs,f,f_env);goto _LL189;_LL18C: if(*((int*)_tmp21B)!= 15)goto _LL18E;
_tmp21D=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp21B)->f1;_tmp21E=((struct Cyc_Absyn_Cast_e_struct*)
_tmp21B)->f2;_LL18D: Cyc_Toc_lvalue_assign(_tmp21E,fs,f,f_env);goto _LL189;_LL18E:
if(*((int*)_tmp21B)!= 23)goto _LL190;_tmp21F=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp21B)->f1;_tmp220=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp21B)->f2;_LL18F:(
void*)(e1->r=(void*)((void*)_tmp21F->r));Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp221=_cycalloc(sizeof(*_tmp221));_tmp221->hd=_tmp220;
_tmp221->tl=fs;_tmp221;}),f,f_env);goto _LL189;_LL190:;_LL191: {struct Cyc_Absyn_Exp*
e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(
e1_copy,(struct _tagged_arr*)fs->hd,e1_copy->loc);}(void*)(e1->r=(void*)((void*)(
f(e1_copy,f_env))->r));goto _LL189;}_LL189:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp222=(void*)s->r;struct Cyc_Absyn_Exp*_tmp223;struct
Cyc_Absyn_Decl*_tmp224;struct Cyc_Absyn_Stmt*_tmp225;struct Cyc_Absyn_Stmt*_tmp226;
_LL193: if(_tmp222 <= (void*)1  || *((int*)_tmp222)!= 0)goto _LL195;_tmp223=((struct
Cyc_Absyn_Exp_s_struct*)_tmp222)->f1;_LL194: Cyc_Toc_lvalue_assign(_tmp223,fs,f,
f_env);goto _LL192;_LL195: if(_tmp222 <= (void*)1  || *((int*)_tmp222)!= 11)goto
_LL197;_tmp224=((struct Cyc_Absyn_Decl_s_struct*)_tmp222)->f1;_tmp225=((struct Cyc_Absyn_Decl_s_struct*)
_tmp222)->f2;_LL196: Cyc_Toc_lvalue_assign_stmt(_tmp225,fs,f,f_env);goto _LL192;
_LL197: if(_tmp222 <= (void*)1  || *((int*)_tmp222)!= 1)goto _LL199;_tmp226=((struct
Cyc_Absyn_Seq_s_struct*)_tmp222)->f2;_LL198: Cyc_Toc_lvalue_assign_stmt(_tmp226,
fs,f,f_env);goto _LL192;_LL199:;_LL19A:({struct Cyc_String_pa_struct _tmp229;
_tmp229.tag=0;_tmp229.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_stmt2string(
s));{void*_tmp227[1]={& _tmp229};Cyc_Toc_toc_impos(({const char*_tmp228="lvalue_assign_stmt: %s";
_tag_arr(_tmp228,sizeof(char),_get_zero_arr_size(_tmp228,23));}),_tag_arr(
_tmp227,sizeof(void*),1));}});_LL192:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(
struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({
struct Cyc_List_List*_tmp22A=_region_malloc(r2,sizeof(*_tmp22A));_tmp22A->hd=(
void*)f((void*)x->hd,env);_tmp22A->tl=0;_tmp22A;});prev=result;for(x=x->tl;x != 0;
x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp22B=_region_malloc(r2,sizeof(*_tmp22B));_tmp22B->hd=(void*)f((void*)x->hd,
env);_tmp22B->tl=0;_tmp22B;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,
x);}static struct _tuple6*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({struct
_tuple6*_tmp22C=_cycalloc(sizeof(*_tmp22C));_tmp22C->f1=0;_tmp22C->f2=e;_tmp22C;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp22D=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp22E;_LL19C: if(_tmp22D <= (void*)4  || *((int*)
_tmp22D)!= 4)goto _LL19E;_tmp22E=((struct Cyc_Absyn_PointerType_struct*)_tmp22D)->f1;
_LL19D: return _tmp22E;_LL19E:;_LL19F:({void*_tmp22F[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp230="get_ptr_typ: not a pointer!";
_tag_arr(_tmp230,sizeof(char),_get_zero_arr_size(_tmp230,28));}),_tag_arr(
_tmp22F,sizeof(void*),0));});_LL19B:;}static int Cyc_Toc_is_zero_ptr_type(void*t,
void**ptr_type,int*is_fat,void**elt_type){void*_tmp231=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp232;void*_tmp233;struct Cyc_Absyn_PtrAtts _tmp234;
struct Cyc_Absyn_Conref*_tmp235;struct Cyc_Absyn_Conref*_tmp236;struct Cyc_Absyn_ArrayInfo
_tmp237;void*_tmp238;struct Cyc_Absyn_Tqual _tmp239;struct Cyc_Absyn_Exp*_tmp23A;
struct Cyc_Absyn_Conref*_tmp23B;_LL1A1: if(_tmp231 <= (void*)4  || *((int*)_tmp231)
!= 4)goto _LL1A3;_tmp232=((struct Cyc_Absyn_PointerType_struct*)_tmp231)->f1;
_tmp233=(void*)_tmp232.elt_typ;_tmp234=_tmp232.ptr_atts;_tmp235=_tmp234.bounds;
_tmp236=_tmp234.zero_term;_LL1A2: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp236)){*ptr_type=t;*elt_type=_tmp233;{void*_tmp23C=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp235);_LL1A8: if((int)_tmp23C != 0)goto _LL1AA;_LL1A9:*is_fat=1;goto _LL1A7;
_LL1AA:;_LL1AB:*is_fat=0;goto _LL1A7;_LL1A7:;}return 1;}else{return 0;}_LL1A3: if(
_tmp231 <= (void*)4  || *((int*)_tmp231)!= 7)goto _LL1A5;_tmp237=((struct Cyc_Absyn_ArrayType_struct*)
_tmp231)->f1;_tmp238=(void*)_tmp237.elt_type;_tmp239=_tmp237.tq;_tmp23A=_tmp237.num_elts;
_tmp23B=_tmp237.zero_term;_LL1A4: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp23B)){*elt_type=_tmp238;*is_fat=0;*ptr_type=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp23D=_cycalloc(sizeof(*_tmp23D));_tmp23D[0]=({struct Cyc_Absyn_PointerType_struct
_tmp23E;_tmp23E.tag=4;_tmp23E.f1=({struct Cyc_Absyn_PtrInfo _tmp23F;_tmp23F.elt_typ=(
void*)_tmp238;_tmp23F.elt_tq=_tmp239;_tmp23F.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp240;_tmp240.rgn=(void*)((void*)2);_tmp240.nullable=Cyc_Absyn_false_conref;
_tmp240.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp241=_cycalloc(sizeof(*_tmp241));_tmp241[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp242;_tmp242.tag=0;_tmp242.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp23A);
_tmp242;});_tmp241;}));_tmp240.zero_term=_tmp23B;_tmp240.ptrloc=0;_tmp240;});
_tmp23F;});_tmp23E;});_tmp23D;});return 1;}else{return 0;}_LL1A5:;_LL1A6: return 0;
_LL1A0:;}static int Cyc_Toc_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_fat,void**elt_type){void*_tmp243=(void*)e1->r;struct Cyc_Absyn_Exp*
_tmp244;struct Cyc_Absyn_Exp*_tmp245;struct Cyc_Absyn_Exp*_tmp246;struct Cyc_Absyn_Exp*
_tmp247;struct Cyc_Absyn_Exp*_tmp248;struct Cyc_Absyn_Exp*_tmp249;_LL1AD: if(*((int*)
_tmp243)!= 15)goto _LL1AF;_LL1AE:({struct Cyc_String_pa_struct _tmp24C;_tmp24C.tag=
0;_tmp24C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e1));{
void*_tmp24A[1]={& _tmp24C};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp24B="we have a cast in a lhs:  %s";_tag_arr(
_tmp24B,sizeof(char),_get_zero_arr_size(_tmp24B,29));}),_tag_arr(_tmp24A,sizeof(
void*),1));}});_LL1AF: if(*((int*)_tmp243)!= 22)goto _LL1B1;_tmp244=((struct Cyc_Absyn_Deref_e_struct*)
_tmp243)->f1;_LL1B0: _tmp245=_tmp244;goto _LL1B2;_LL1B1: if(*((int*)_tmp243)!= 25)
goto _LL1B3;_tmp245=((struct Cyc_Absyn_Subscript_e_struct*)_tmp243)->f1;_LL1B2:
return Cyc_Toc_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp245->topt))->v,
ptr_type,is_fat,elt_type);_LL1B3: if(*((int*)_tmp243)!= 24)goto _LL1B5;_tmp246=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp243)->f1;_LL1B4: _tmp247=_tmp246;goto
_LL1B6;_LL1B5: if(*((int*)_tmp243)!= 23)goto _LL1B7;_tmp247=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp243)->f1;_LL1B6: if(Cyc_Toc_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp247->topt))->v,ptr_type,is_fat,elt_type))({struct Cyc_String_pa_struct
_tmp24F;_tmp24F.tag=0;_tmp24F.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(
e1));{void*_tmp24D[1]={& _tmp24F};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Toc_toc_impos)(({const char*_tmp24E="found zero pointer aggregate member assignment: %s";
_tag_arr(_tmp24E,sizeof(char),_get_zero_arr_size(_tmp24E,51));}),_tag_arr(
_tmp24D,sizeof(void*),1));}});return 0;_LL1B7: if(*((int*)_tmp243)!= 14)goto _LL1B9;
_tmp248=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp243)->f1;_LL1B8: _tmp249=
_tmp248;goto _LL1BA;_LL1B9: if(*((int*)_tmp243)!= 13)goto _LL1BB;_tmp249=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp243)->f1;_LL1BA: if(Cyc_Toc_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp249->topt))->v,ptr_type,is_fat,
elt_type))({struct Cyc_String_pa_struct _tmp252;_tmp252.tag=0;_tmp252.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e1));{void*_tmp250[1]={&
_tmp252};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp251="found zero pointer instantiate/noinstantiate: %s";_tag_arr(
_tmp251,sizeof(char),_get_zero_arr_size(_tmp251,49));}),_tag_arr(_tmp250,sizeof(
void*),1));}});return 0;_LL1BB: if(*((int*)_tmp243)!= 1)goto _LL1BD;_LL1BC: return 0;
_LL1BD:;_LL1BE:({struct Cyc_String_pa_struct _tmp255;_tmp255.tag=0;_tmp255.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e1));{void*_tmp253[
1]={& _tmp255};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp254="found bad lhs in is_zero_ptr_deref: %s";_tag_arr(_tmp254,
sizeof(char),_get_zero_arr_size(_tmp254,39));}),_tag_arr(_tmp253,sizeof(void*),1));}});
_LL1AC:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp256=Cyc_Tcutil_compress(t);void*_tmp257;void*_tmp258;void*_tmp259;
void*_tmp25A;void*_tmp25B;void*_tmp25C;void*_tmp25D;void*_tmp25E;_LL1C0: if(
_tmp256 <= (void*)4  || *((int*)_tmp256)!= 4)goto _LL1C2;_LL1C1: res=Cyc_Absyn_null_exp(
0);goto _LL1BF;_LL1C2: if(_tmp256 <= (void*)4  || *((int*)_tmp256)!= 5)goto _LL1C4;
_tmp257=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp256)->f1;_tmp258=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp256)->f2;if((int)_tmp258 != 0)goto _LL1C4;
_LL1C3: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Char_c_struct*_tmp25F=
_cycalloc(sizeof(*_tmp25F));_tmp25F[0]=({struct Cyc_Absyn_Char_c_struct _tmp260;
_tmp260.tag=0;_tmp260.f1=(void*)_tmp257;_tmp260.f2='\000';_tmp260;});_tmp25F;}),
0);goto _LL1BF;_LL1C4: if(_tmp256 <= (void*)4  || *((int*)_tmp256)!= 5)goto _LL1C6;
_tmp259=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp256)->f1;_tmp25A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp256)->f2;if((int)_tmp25A != 1)goto _LL1C6;
_LL1C5: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Short_c_struct*_tmp261=
_cycalloc(sizeof(*_tmp261));_tmp261[0]=({struct Cyc_Absyn_Short_c_struct _tmp262;
_tmp262.tag=1;_tmp262.f1=(void*)_tmp259;_tmp262.f2=0;_tmp262;});_tmp261;}),0);
goto _LL1BF;_LL1C6: if(_tmp256 <= (void*)4  || *((int*)_tmp256)!= 12)goto _LL1C8;
_LL1C7: goto _LL1C9;_LL1C8: if(_tmp256 <= (void*)4  || *((int*)_tmp256)!= 13)goto
_LL1CA;_LL1C9: _tmp25B=(void*)1;goto _LL1CB;_LL1CA: if(_tmp256 <= (void*)4  || *((int*)
_tmp256)!= 5)goto _LL1CC;_tmp25B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp256)->f1;
_tmp25C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp256)->f2;if((int)_tmp25C != 
2)goto _LL1CC;_LL1CB: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*
_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263[0]=({struct Cyc_Absyn_Int_c_struct
_tmp264;_tmp264.tag=2;_tmp264.f1=(void*)_tmp25B;_tmp264.f2=0;_tmp264;});_tmp263;}),
0);goto _LL1BF;_LL1CC: if(_tmp256 <= (void*)4  || *((int*)_tmp256)!= 5)goto _LL1CE;
_tmp25D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp256)->f1;_tmp25E=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp256)->f2;if((int)_tmp25E != 3)goto _LL1CE;
_LL1CD: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_LongLong_c_struct*
_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265[0]=({struct Cyc_Absyn_LongLong_c_struct
_tmp266;_tmp266.tag=3;_tmp266.f1=(void*)_tmp25D;_tmp266.f2=(long long)0;_tmp266;});
_tmp265;}),0);goto _LL1BF;_LL1CE: if((int)_tmp256 != 1)goto _LL1D0;_LL1CF: goto _LL1D1;
_LL1D0: if(_tmp256 <= (void*)4  || *((int*)_tmp256)!= 6)goto _LL1D2;_LL1D1: res=Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Float_c_struct*_tmp267=_cycalloc(sizeof(*_tmp267));
_tmp267[0]=({struct Cyc_Absyn_Float_c_struct _tmp268;_tmp268.tag=4;_tmp268.f1=({
const char*_tmp269="0.0";_tag_arr(_tmp269,sizeof(char),_get_zero_arr_size(_tmp269,
4));});_tmp268;});_tmp267;}),0);goto _LL1BF;_LL1D2:;_LL1D3:({struct Cyc_String_pa_struct
_tmp26C;_tmp26C.tag=0;_tmp26C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp26A[1]={& _tmp26C};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Toc_toc_impos)(({const char*_tmp26B="found non-zero type %s in generate_zero";
_tag_arr(_tmp26B,sizeof(char),_get_zero_arr_size(_tmp26B,40));}),_tag_arr(
_tmp26A,sizeof(void*),1));}});_LL1BF:;}res->topt=({struct Cyc_Core_Opt*_tmp26D=
_cycalloc(sizeof(*_tmp26D));_tmp26D->v=(void*)t;_tmp26D;});return res;}static void
Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,
int is_fat,void*elt_type){void*_tmp26E=Cyc_Absyn_tagged_typ(elt_type,(void*)2,Cyc_Toc_mt_tq,
Cyc_Absyn_true_conref);void*_tmp26F=Cyc_Toc_typ_to_c(elt_type);void*_tmp270=Cyc_Toc_typ_to_c(
_tmp26E);void*_tmp271=Cyc_Absyn_cstar_typ(_tmp26F,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmp272=({struct Cyc_Core_Opt*_tmp2AC=_cycalloc(sizeof(*_tmp2AC));_tmp2AC->v=(
void*)_tmp271;_tmp2AC;});struct Cyc_Absyn_Exp*xinit;{void*_tmp273=(void*)e1->r;
struct Cyc_Absyn_Exp*_tmp274;struct Cyc_Absyn_Exp*_tmp275;struct Cyc_Absyn_Exp*
_tmp276;_LL1D5: if(*((int*)_tmp273)!= 22)goto _LL1D7;_tmp274=((struct Cyc_Absyn_Deref_e_struct*)
_tmp273)->f1;_LL1D6: if(!is_fat){_tmp274=Cyc_Toc_cast_it(_tmp26E,_tmp274);_tmp274->topt=({
struct Cyc_Core_Opt*_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277->v=(void*)_tmp26E;
_tmp277;});}Cyc_Toc_exp_to_c(nv,_tmp274);xinit=_tmp274;goto _LL1D4;_LL1D7: if(*((
int*)_tmp273)!= 25)goto _LL1D9;_tmp275=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp273)->f1;_tmp276=((struct Cyc_Absyn_Subscript_e_struct*)_tmp273)->f2;_LL1D8:
if(!is_fat){_tmp275=Cyc_Toc_cast_it(_tmp26E,_tmp275);_tmp275->topt=({struct Cyc_Core_Opt*
_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278->v=(void*)_tmp26E;_tmp278;});}Cyc_Toc_exp_to_c(
nv,_tmp275);Cyc_Toc_exp_to_c(nv,_tmp276);xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp279[3];_tmp279[2]=_tmp276;_tmp279[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0);_tmp279[0]=_tmp275;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp279,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);goto _LL1D4;_LL1D9:;_LL1DA:({void*_tmp27A[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp27B="found bad lhs for zero-terminated pointer assignment";
_tag_arr(_tmp27B,sizeof(char),_get_zero_arr_size(_tmp27B,53));}),_tag_arr(
_tmp27A,sizeof(void*),0));});_LL1D4:;}{struct _tuple1*_tmp27C=Cyc_Toc_temp_var();
nv=Cyc_Toc_add_varmap(nv,_tmp27C,Cyc_Absyn_var_exp(_tmp27C,0));{struct Cyc_Absyn_Vardecl*
_tmp27D=({struct Cyc_Absyn_Vardecl*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB->sc=(
void*)((void*)2);_tmp2AB->name=_tmp27C;_tmp2AB->tq=Cyc_Toc_mt_tq;_tmp2AB->type=(
void*)_tmp270;_tmp2AB->initializer=(struct Cyc_Absyn_Exp*)xinit;_tmp2AB->rgn=0;
_tmp2AB->attributes=0;_tmp2AB->escapes=0;_tmp2AB;});struct Cyc_Absyn_Local_b_struct*
_tmp27E=({struct Cyc_Absyn_Local_b_struct*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));
_tmp2A9[0]=({struct Cyc_Absyn_Local_b_struct _tmp2AA;_tmp2AA.tag=3;_tmp2AA.f1=
_tmp27D;_tmp2AA;});_tmp2A9;});struct Cyc_Absyn_Exp*_tmp27F=Cyc_Absyn_varb_exp(
_tmp27C,(void*)_tmp27E,0);_tmp27F->topt=({struct Cyc_Core_Opt*_tmp280=_cycalloc(
sizeof(*_tmp280));_tmp280->v=(void*)_tmp26E;_tmp280;});{struct Cyc_Absyn_Exp*
_tmp281=Cyc_Absyn_deref_exp(_tmp27F,0);_tmp281->topt=({struct Cyc_Core_Opt*
_tmp282=_cycalloc(sizeof(*_tmp282));_tmp282->v=(void*)elt_type;_tmp282;});Cyc_Toc_exp_to_c(
nv,_tmp281);{struct _tuple1*_tmp283=Cyc_Toc_temp_var();nv=Cyc_Toc_add_varmap(nv,
_tmp283,Cyc_Absyn_var_exp(_tmp283,0));{struct Cyc_Absyn_Vardecl*_tmp284=({struct
Cyc_Absyn_Vardecl*_tmp2A8=_cycalloc(sizeof(*_tmp2A8));_tmp2A8->sc=(void*)((void*)
2);_tmp2A8->name=_tmp283;_tmp2A8->tq=Cyc_Toc_mt_tq;_tmp2A8->type=(void*)_tmp26F;
_tmp2A8->initializer=(struct Cyc_Absyn_Exp*)_tmp281;_tmp2A8->rgn=0;_tmp2A8->attributes=
0;_tmp2A8->escapes=0;_tmp2A8;});struct Cyc_Absyn_Local_b_struct*_tmp285=({struct
Cyc_Absyn_Local_b_struct*_tmp2A6=_cycalloc(sizeof(*_tmp2A6));_tmp2A6[0]=({struct
Cyc_Absyn_Local_b_struct _tmp2A7;_tmp2A7.tag=3;_tmp2A7.f1=_tmp284;_tmp2A7;});
_tmp2A6;});struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*
_tmp286=Cyc_Absyn_varb_exp(_tmp283,(void*)_tmp285,0);_tmp286->topt=_tmp281->topt;
z_init=Cyc_Absyn_prim2_exp((void*)popt->v,_tmp286,e2,0);z_init->topt=_tmp286->topt;}
Cyc_Toc_exp_to_c(nv,z_init);{struct _tuple1*_tmp287=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp288=({struct Cyc_Absyn_Vardecl*_tmp2A5=_cycalloc(sizeof(*_tmp2A5));_tmp2A5->sc=(
void*)((void*)2);_tmp2A5->name=_tmp287;_tmp2A5->tq=Cyc_Toc_mt_tq;_tmp2A5->type=(
void*)_tmp26F;_tmp2A5->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp2A5->rgn=0;
_tmp2A5->attributes=0;_tmp2A5->escapes=0;_tmp2A5;});struct Cyc_Absyn_Local_b_struct*
_tmp289=({struct Cyc_Absyn_Local_b_struct*_tmp2A3=_cycalloc(sizeof(*_tmp2A3));
_tmp2A3[0]=({struct Cyc_Absyn_Local_b_struct _tmp2A4;_tmp2A4.tag=3;_tmp2A4.f1=
_tmp288;_tmp2A4;});_tmp2A3;});nv=Cyc_Toc_add_varmap(nv,_tmp287,Cyc_Absyn_var_exp(
_tmp287,0));{struct Cyc_Absyn_Exp*_tmp28A=Cyc_Absyn_varb_exp(_tmp283,(void*)
_tmp285,0);_tmp28A->topt=_tmp281->topt;{struct Cyc_Absyn_Exp*_tmp28B=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp28C=Cyc_Absyn_prim2_exp((void*)5,_tmp28A,
_tmp28B,0);_tmp28C->topt=({struct Cyc_Core_Opt*_tmp28D=_cycalloc(sizeof(*_tmp28D));
_tmp28D->v=(void*)Cyc_Absyn_sint_typ;_tmp28D;});Cyc_Toc_exp_to_c(nv,_tmp28C);{
struct Cyc_Absyn_Exp*_tmp28E=Cyc_Absyn_varb_exp(_tmp287,(void*)_tmp289,0);_tmp28E->topt=
_tmp281->topt;{struct Cyc_Absyn_Exp*_tmp28F=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp290=Cyc_Absyn_prim2_exp((void*)6,_tmp28E,_tmp28F,0);
_tmp290->topt=({struct Cyc_Core_Opt*_tmp291=_cycalloc(sizeof(*_tmp291));_tmp291->v=(
void*)Cyc_Absyn_sint_typ;_tmp291;});Cyc_Toc_exp_to_c(nv,_tmp290);{struct Cyc_Absyn_Exp*
_tmp292=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp2A2[2];_tmp2A2[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp2A2[0]=Cyc_Absyn_varb_exp(_tmp27C,(void*)_tmp27E,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp2A2,sizeof(struct Cyc_Absyn_Exp*),
2));}),0),Cyc_Absyn_uint_exp(1,0),0);struct Cyc_Absyn_Exp*_tmp293=Cyc_Absyn_and_exp(
_tmp292,Cyc_Absyn_and_exp(_tmp28C,_tmp290,0),0);struct Cyc_Absyn_Stmt*_tmp294=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*
_tmp295=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_varb_exp(_tmp27C,(void*)_tmp27E,0),
Cyc_Toc_curr_sp,0);_tmp295=Cyc_Toc_cast_it(_tmp271,_tmp295);{struct Cyc_Absyn_Exp*
_tmp296=Cyc_Absyn_deref_exp(_tmp295,0);struct Cyc_Absyn_Exp*_tmp297=Cyc_Absyn_assign_exp(
_tmp296,Cyc_Absyn_var_exp(_tmp287,0),0);struct Cyc_Absyn_Stmt*_tmp298=Cyc_Absyn_exp_stmt(
_tmp297,0);_tmp298=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp293,_tmp294,
Cyc_Absyn_skip_stmt(0),0),_tmp298,0);_tmp298=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*
_tmp299=_cycalloc(sizeof(*_tmp299));_tmp299->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp29A=_cycalloc(sizeof(*_tmp29A));_tmp29A[0]=({struct Cyc_Absyn_Var_d_struct
_tmp29B;_tmp29B.tag=0;_tmp29B.f1=_tmp288;_tmp29B;});_tmp29A;}));_tmp299->loc=0;
_tmp299;}),_tmp298,0);_tmp298=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp29C=
_cycalloc(sizeof(*_tmp29C));_tmp29C->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D[0]=({struct Cyc_Absyn_Var_d_struct
_tmp29E;_tmp29E.tag=0;_tmp29E.f1=_tmp284;_tmp29E;});_tmp29D;}));_tmp29C->loc=0;
_tmp29C;}),_tmp298,0);_tmp298=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp29F=
_cycalloc(sizeof(*_tmp29F));_tmp29F->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0[0]=({struct Cyc_Absyn_Var_d_struct
_tmp2A1;_tmp2A1.tag=0;_tmp2A1.f1=_tmp27D;_tmp2A1;});_tmp2A0;}));_tmp29F->loc=0;
_tmp29F;}),_tmp298,0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp298,0))->r));}}}}}}}}}}}}}
struct _tuple10{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple11{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp2AD=(void*)e->r;if(e->topt
== 0)({struct Cyc_String_pa_struct _tmp2B0;_tmp2B0.tag=0;_tmp2B0.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e));{void*_tmp2AE[1]={&
_tmp2B0};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp2AF="exp_to_c: no type for %s";_tag_arr(_tmp2AF,sizeof(char),
_get_zero_arr_size(_tmp2AF,25));}),_tag_arr(_tmp2AE,sizeof(void*),1));}});{void*
old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp2B1=
_tmp2AD;void*_tmp2B2;struct _tuple1*_tmp2B3;void*_tmp2B4;struct _tuple1*_tmp2B5;
void*_tmp2B6;struct Cyc_List_List*_tmp2B7;struct Cyc_Absyn_Exp*_tmp2B8;void*
_tmp2B9;struct Cyc_Absyn_Exp*_tmp2BA;struct Cyc_Core_Opt*_tmp2BB;struct Cyc_Absyn_Exp*
_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_Absyn_Exp*
_tmp2BF;struct Cyc_Absyn_Exp*_tmp2C0;struct Cyc_Absyn_Exp*_tmp2C1;struct Cyc_Absyn_Exp*
_tmp2C2;struct Cyc_Absyn_Exp*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C4;struct Cyc_Absyn_Exp*
_tmp2C5;struct Cyc_Absyn_Exp*_tmp2C6;struct Cyc_List_List*_tmp2C7;struct Cyc_Absyn_Exp*
_tmp2C8;struct Cyc_List_List*_tmp2C9;struct Cyc_Absyn_VarargCallInfo*_tmp2CA;
struct Cyc_Absyn_Exp*_tmp2CB;struct Cyc_List_List*_tmp2CC;struct Cyc_Absyn_VarargCallInfo*
_tmp2CD;struct Cyc_Absyn_VarargCallInfo _tmp2CE;int _tmp2CF;struct Cyc_List_List*
_tmp2D0;struct Cyc_Absyn_VarargInfo*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D2;struct Cyc_Absyn_Exp*
_tmp2D3;struct Cyc_Absyn_Exp*_tmp2D4;struct Cyc_List_List*_tmp2D5;void*_tmp2D6;
void**_tmp2D7;struct Cyc_Absyn_Exp*_tmp2D8;int _tmp2D9;void*_tmp2DA;struct Cyc_Absyn_Exp*
_tmp2DB;struct Cyc_Absyn_Exp*_tmp2DC;struct Cyc_Absyn_Exp*_tmp2DD;struct Cyc_Absyn_Exp*
_tmp2DE;void*_tmp2DF;void*_tmp2E0;void*_tmp2E1;struct _tagged_arr*_tmp2E2;void*
_tmp2E3;void*_tmp2E4;unsigned int _tmp2E5;struct Cyc_Absyn_Exp*_tmp2E6;struct Cyc_Absyn_Exp*
_tmp2E7;struct _tagged_arr*_tmp2E8;struct Cyc_Absyn_Exp*_tmp2E9;struct _tagged_arr*
_tmp2EA;struct Cyc_Absyn_Exp*_tmp2EB;struct Cyc_Absyn_Exp*_tmp2EC;struct Cyc_List_List*
_tmp2ED;struct Cyc_List_List*_tmp2EE;struct Cyc_Absyn_Vardecl*_tmp2EF;struct Cyc_Absyn_Exp*
_tmp2F0;struct Cyc_Absyn_Exp*_tmp2F1;int _tmp2F2;struct _tuple1*_tmp2F3;struct Cyc_List_List*
_tmp2F4;struct Cyc_List_List*_tmp2F5;struct Cyc_Absyn_Aggrdecl*_tmp2F6;void*
_tmp2F7;struct Cyc_List_List*_tmp2F8;struct Cyc_List_List*_tmp2F9;struct Cyc_Absyn_Tuniondecl*
_tmp2FA;struct Cyc_Absyn_Tunionfield*_tmp2FB;struct Cyc_List_List*_tmp2FC;struct
Cyc_Absyn_Tuniondecl*_tmp2FD;struct Cyc_Absyn_Tunionfield*_tmp2FE;struct Cyc_Absyn_MallocInfo
_tmp2FF;int _tmp300;struct Cyc_Absyn_Exp*_tmp301;void**_tmp302;struct Cyc_Absyn_Exp*
_tmp303;int _tmp304;struct Cyc_Absyn_Stmt*_tmp305;_LL1DC: if(*((int*)_tmp2B1)!= 0)
goto _LL1DE;_tmp2B2=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp2B1)->f1;if((int)
_tmp2B2 != 0)goto _LL1DE;_LL1DD: {struct Cyc_Absyn_Exp*_tmp306=Cyc_Absyn_uint_exp(0,
0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){if(nv->toplevel)(void*)(e->r=(
void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_typ,_tmp306,_tmp306))->r));
else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp307[3];_tmp307[2]=_tmp306;_tmp307[1]=_tmp306;_tmp307[0]=
_tmp306;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp307,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}else{(void*)(e->r=(void*)((
void*)(Cyc_Absyn_signed_int_exp(0,0))->r));}goto _LL1DB;}_LL1DE: if(*((int*)
_tmp2B1)!= 0)goto _LL1E0;_LL1DF: goto _LL1DB;_LL1E0: if(*((int*)_tmp2B1)!= 1)goto
_LL1E2;_tmp2B3=((struct Cyc_Absyn_Var_e_struct*)_tmp2B1)->f1;_tmp2B4=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2B1)->f2;_LL1E1:{struct _handler_cons _tmp308;
_push_handler(& _tmp308);{int _tmp30A=0;if(setjmp(_tmp308.handler))_tmp30A=1;if(!
_tmp30A){(void*)(e->r=(void*)((void*)(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*
d,struct _tuple1*k))Cyc_Dict_lookup)(nv->varmap,_tmp2B3))->r));;_pop_handler();}
else{void*_tmp309=(void*)_exn_thrown;void*_tmp30C=_tmp309;_LL231: if(_tmp30C != 
Cyc_Dict_Absent)goto _LL233;_LL232:({struct Cyc_String_pa_struct _tmp30F;_tmp30F.tag=
0;_tmp30F.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp2B3));{void*_tmp30D[1]={& _tmp30F};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp30E="Can't find %s in exp_to_c, Var\n";
_tag_arr(_tmp30E,sizeof(char),_get_zero_arr_size(_tmp30E,32));}),_tag_arr(
_tmp30D,sizeof(void*),1));}});_LL233:;_LL234:(void)_throw(_tmp30C);_LL230:;}}}
goto _LL1DB;_LL1E2: if(*((int*)_tmp2B1)!= 2)goto _LL1E4;_tmp2B5=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp2B1)->f1;_LL1E3:({void*_tmp310[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp311="unknownid";_tag_arr(
_tmp311,sizeof(char),_get_zero_arr_size(_tmp311,10));}),_tag_arr(_tmp310,sizeof(
void*),0));});_LL1E4: if(*((int*)_tmp2B1)!= 3)goto _LL1E6;_tmp2B6=(void*)((struct
Cyc_Absyn_Primop_e_struct*)_tmp2B1)->f1;_tmp2B7=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2B1)->f2;_LL1E5: {struct Cyc_List_List*_tmp312=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,
_tmp2B7);((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp2B7);{void*
_tmp313=_tmp2B6;_LL236: if((int)_tmp313 != 19)goto _LL238;_LL237: {struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2B7))->hd;{void*
_tmp314=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);
struct Cyc_Absyn_ArrayInfo _tmp315;struct Cyc_Absyn_Exp*_tmp316;struct Cyc_Absyn_PtrInfo
_tmp317;void*_tmp318;struct Cyc_Absyn_PtrAtts _tmp319;struct Cyc_Absyn_Conref*
_tmp31A;struct Cyc_Absyn_Conref*_tmp31B;struct Cyc_Absyn_Conref*_tmp31C;_LL24B: if(
_tmp314 <= (void*)4  || *((int*)_tmp314)!= 7)goto _LL24D;_tmp315=((struct Cyc_Absyn_ArrayType_struct*)
_tmp314)->f1;_tmp316=_tmp315.num_elts;_LL24C:(void*)(e->r=(void*)((void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp316))->r));goto _LL24A;_LL24D: if(_tmp314 <= (
void*)4  || *((int*)_tmp314)!= 4)goto _LL24F;_tmp317=((struct Cyc_Absyn_PointerType_struct*)
_tmp314)->f1;_tmp318=(void*)_tmp317.elt_typ;_tmp319=_tmp317.ptr_atts;_tmp31A=
_tmp319.nullable;_tmp31B=_tmp319.bounds;_tmp31C=_tmp319.zero_term;_LL24E:{void*
_tmp31D=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp31B);struct Cyc_Absyn_Exp*
_tmp31E;_LL252: if((int)_tmp31D != 0)goto _LL254;_LL253:(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({struct Cyc_Absyn_Exp*_tmp31F[2];
_tmp31F[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp318),0);_tmp31F[0]=(
struct Cyc_Absyn_Exp*)_tmp2B7->hd;((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp31F,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));
goto _LL251;_LL254: if(_tmp31D <= (void*)1  || *((int*)_tmp31D)!= 0)goto _LL256;
_tmp31E=((struct Cyc_Absyn_Upper_b_struct*)_tmp31D)->f1;_LL255: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp31C))(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*
_tmp320[2];_tmp320[1]=_tmp31E;_tmp320[0]=(struct Cyc_Absyn_Exp*)_tmp2B7->hd;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp320,
sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));else{if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp31A))(void*)(e->r=(void*)((void*)(Cyc_Absyn_conditional_exp(
arg,_tmp31E,Cyc_Absyn_uint_exp(0,0),0))->r));else{(void*)(e->r=(void*)((void*)
_tmp31E->r));goto _LL251;}}goto _LL251;_LL256: if(_tmp31D <= (void*)1  || *((int*)
_tmp31D)!= 1)goto _LL251;_LL257:({void*_tmp321[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp322="toc: size of AbsUpper_b";
_tag_arr(_tmp322,sizeof(char),_get_zero_arr_size(_tmp322,24));}),_tag_arr(
_tmp321,sizeof(void*),0));});_LL251:;}goto _LL24A;_LL24F:;_LL250:({struct Cyc_String_pa_struct
_tmp326;_tmp326.tag=0;_tmp326.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{struct Cyc_String_pa_struct
_tmp325;_tmp325.tag=0;_tmp325.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{void*_tmp323[2]={&
_tmp325,& _tmp326};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp324="size primop applied to non-array %s (%s)";_tag_arr(_tmp324,
sizeof(char),_get_zero_arr_size(_tmp324,41));}),_tag_arr(_tmp323,sizeof(void*),2));}}});
_LL24A:;}goto _LL235;}_LL238: if((int)_tmp313 != 0)goto _LL23A;_LL239:{void*_tmp327=
Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp312))->hd);
struct Cyc_Absyn_PtrInfo _tmp328;void*_tmp329;struct Cyc_Absyn_PtrAtts _tmp32A;
struct Cyc_Absyn_Conref*_tmp32B;struct Cyc_Absyn_Conref*_tmp32C;_LL259: if(_tmp327
<= (void*)4  || *((int*)_tmp327)!= 4)goto _LL25B;_tmp328=((struct Cyc_Absyn_PointerType_struct*)
_tmp327)->f1;_tmp329=(void*)_tmp328.elt_typ;_tmp32A=_tmp328.ptr_atts;_tmp32B=
_tmp32A.bounds;_tmp32C=_tmp32A.zero_term;_LL25A:{void*_tmp32D=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp32B);struct Cyc_Absyn_Exp*_tmp32E;_LL25E: if((int)_tmp32D
!= 0)goto _LL260;_LL25F: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp2B7))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2B7->tl))->hd;(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp32F[3];
_tmp32F[2]=e2;_tmp32F[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp329),0);
_tmp32F[0]=e1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp32F,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));goto _LL25D;}_LL260:
if(_tmp32D <= (void*)1  || *((int*)_tmp32D)!= 0)goto _LL262;_tmp32E=((struct Cyc_Absyn_Upper_b_struct*)
_tmp32D)->f1;_LL261: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp32C)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2B7))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp2B7->tl))->hd;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__zero_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp330[3];_tmp330[2]=e2;_tmp330[
1]=_tmp32E;_tmp330[0]=e1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp330,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}goto _LL25D;_LL262:
if(_tmp32D <= (void*)1  || *((int*)_tmp32D)!= 1)goto _LL25D;_LL263:({void*_tmp331[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*
_tmp332="toc: plus on AbsUpper_b";_tag_arr(_tmp332,sizeof(char),
_get_zero_arr_size(_tmp332,24));}),_tag_arr(_tmp331,sizeof(void*),0));});_LL25D:;}
goto _LL258;_LL25B:;_LL25C: goto _LL258;_LL258:;}goto _LL235;_LL23A: if((int)_tmp313
!= 2)goto _LL23C;_LL23B: {void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)((struct Cyc_List_List*)_check_null(_tmp312))->hd,& elt_typ)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2B7))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp2B7->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)
_check_null(_tmp312->tl))->hd)){(void*)(e1->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e2->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,
0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r));}else{(void*)(e->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*
_tmp333[3];_tmp333[2]=Cyc_Absyn_prim1_exp((void*)2,e2,0);_tmp333[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp333[0]=e1;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp333,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));}}goto _LL235;}_LL23C: if((int)_tmp313 != 5)goto _LL23E;_LL23D: goto _LL23F;
_LL23E: if((int)_tmp313 != 6)goto _LL240;_LL23F: goto _LL241;_LL240: if((int)_tmp313 != 
7)goto _LL242;_LL241: goto _LL243;_LL242: if((int)_tmp313 != 9)goto _LL244;_LL243: goto
_LL245;_LL244: if((int)_tmp313 != 8)goto _LL246;_LL245: goto _LL247;_LL246: if((int)
_tmp313 != 10)goto _LL248;_LL247: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2B7))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2B7->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp312))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp312->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(t1))(void*)(e1->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,
0))->r));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))(void*)(e2->r=(void*)((void*)(
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));
goto _LL235;}_LL248:;_LL249: goto _LL235;_LL235:;}goto _LL1DB;}_LL1E6: if(*((int*)
_tmp2B1)!= 5)goto _LL1E8;_tmp2B8=((struct Cyc_Absyn_Increment_e_struct*)_tmp2B1)->f1;
_tmp2B9=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp2B1)->f2;_LL1E7: {void*
e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2B8->topt))->v;void*
ptr_type=(void*)0;void*elt_type=(void*)0;int fat_ptr=0;struct _tagged_arr incr_str=({
const char*_tmp33F="increment";_tag_arr(_tmp33F,sizeof(char),_get_zero_arr_size(
_tmp33F,10));});if(_tmp2B9 == (void*)2  || _tmp2B9 == (void*)3)incr_str=({const char*
_tmp334="decrement";_tag_arr(_tmp334,sizeof(char),_get_zero_arr_size(_tmp334,10));});
if(Cyc_Toc_is_zero_ptr_deref(_tmp2B8,& ptr_type,& fat_ptr,& elt_type)){({struct Cyc_String_pa_struct
_tmp337;_tmp337.tag=0;_tmp337.f1=(struct _tagged_arr)((struct _tagged_arr)incr_str);{
void*_tmp335[1]={& _tmp337};Cyc_Tcutil_terr(e->loc,({const char*_tmp336="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_arr(_tmp336,sizeof(char),_get_zero_arr_size(_tmp336,74));}),_tag_arr(
_tmp335,sizeof(void*),1));}});({void*_tmp338[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp339="in-place inc on zero-term";
_tag_arr(_tmp339,sizeof(char),_get_zero_arr_size(_tmp339,26));}),_tag_arr(
_tmp338,sizeof(void*),0));});}Cyc_Toc_exp_to_c(nv,_tmp2B8);{void*elt_typ=(void*)
0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
_tmp33A=(_tmp2B9 == (void*)1  || _tmp2B9 == (void*)3)?Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e;int _tmp33B=(_tmp2B9 == (void*)0  || _tmp2B9 == (
void*)1)?1: - 1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(_tmp33A,({struct
Cyc_Absyn_Exp*_tmp33C[3];_tmp33C[2]=Cyc_Absyn_signed_int_exp(_tmp33B,0);_tmp33C[
1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp33C[0]=Cyc_Absyn_address_exp(
_tmp2B8,0);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp33C,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*_tmp33D=_tmp2B9 == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_e:
Cyc_Toc__zero_arr_inplace_plus_e;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
_tmp33D,({struct Cyc_Absyn_Exp*_tmp33E[2];_tmp33E[1]=Cyc_Absyn_signed_int_exp(1,0);
_tmp33E[0]=_tmp2B8;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp33E,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}else{if(elt_typ == (
void*)0  && !Cyc_Absyn_is_lvalue(_tmp2B8)){Cyc_Toc_lvalue_assign(_tmp2B8,0,Cyc_Toc_incr_lvalue,
_tmp2B9);(void*)(e->r=(void*)((void*)_tmp2B8->r));}}}goto _LL1DB;}}_LL1E8: if(*((
int*)_tmp2B1)!= 4)goto _LL1EA;_tmp2BA=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp2B1)->f1;_tmp2BB=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp2B1)->f2;_tmp2BC=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp2B1)->f3;_LL1E9: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_fat=0;if(Cyc_Toc_is_zero_ptr_deref(_tmp2BA,&
ptr_type,& is_fat,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp2BA,_tmp2BB,
_tmp2BC,ptr_type,is_fat,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp2BA);void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2BA->topt))->v;
void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2BC->topt))->v;Cyc_Toc_exp_to_c(
nv,_tmp2BA);Cyc_Toc_exp_to_c(nv,_tmp2BC);{int done=0;if(_tmp2BB != 0){void*elt_typ=(
void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;{void*_tmp340=(void*)_tmp2BB->v;_LL265: if((int)_tmp340 != 0)goto _LL267;
_LL266: change=_tmp2BC;goto _LL264;_LL267: if((int)_tmp340 != 2)goto _LL269;_LL268:
change=Cyc_Absyn_prim1_exp((void*)2,_tmp2BC,0);goto _LL264;_LL269:;_LL26A:({void*
_tmp341[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp342="bad t ? pointer arithmetic";_tag_arr(_tmp342,sizeof(char),
_get_zero_arr_size(_tmp342,27));}),_tag_arr(_tmp341,sizeof(void*),0));});_LL264:;}
done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp343[3];_tmp343[2]=change;_tmp343[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp343[0]=Cyc_Absyn_address_exp(_tmp2BA,0);((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp343,
sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp344=(void*)_tmp2BB->v;_LL26C: if((int)_tmp344 != 0)
goto _LL26E;_LL26D: done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp345[2];_tmp345[1]=_tmp2BC;_tmp345[0]=_tmp2BA;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp345,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL26B;_LL26E:;_LL26F:({void*_tmp346[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp347="bad zero-terminated pointer arithmetic";
_tag_arr(_tmp347,sizeof(char),_get_zero_arr_size(_tmp347,39));}),_tag_arr(
_tmp346,sizeof(void*),0));});_LL26B:;}}}if(!done){if(e1_poly)(void*)(_tmp2BC->r=(
void*)((void*)(Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)
_tmp2BC->r,0)))->r));if(!Cyc_Absyn_is_lvalue(_tmp2BA)){((void(*)(struct Cyc_Absyn_Exp*
e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct
_tuple9*),struct _tuple9*f_env))Cyc_Toc_lvalue_assign)(_tmp2BA,0,Cyc_Toc_assignop_lvalue,({
struct _tuple9*_tmp348=_cycalloc(sizeof(struct _tuple9)* 1);_tmp348[0]=({struct
_tuple9 _tmp349;_tmp349.f1=_tmp2BB;_tmp349.f2=_tmp2BC;_tmp349;});_tmp348;}));(
void*)(e->r=(void*)((void*)_tmp2BA->r));}}goto _LL1DB;}}}_LL1EA: if(*((int*)
_tmp2B1)!= 6)goto _LL1EC;_tmp2BD=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2B1)->f1;
_tmp2BE=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2B1)->f2;_tmp2BF=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp2B1)->f3;_LL1EB: Cyc_Toc_exp_to_c(nv,_tmp2BD);Cyc_Toc_exp_to_c(nv,_tmp2BE);
Cyc_Toc_exp_to_c(nv,_tmp2BF);goto _LL1DB;_LL1EC: if(*((int*)_tmp2B1)!= 7)goto
_LL1EE;_tmp2C0=((struct Cyc_Absyn_And_e_struct*)_tmp2B1)->f1;_tmp2C1=((struct Cyc_Absyn_And_e_struct*)
_tmp2B1)->f2;_LL1ED: Cyc_Toc_exp_to_c(nv,_tmp2C0);Cyc_Toc_exp_to_c(nv,_tmp2C1);
goto _LL1DB;_LL1EE: if(*((int*)_tmp2B1)!= 8)goto _LL1F0;_tmp2C2=((struct Cyc_Absyn_Or_e_struct*)
_tmp2B1)->f1;_tmp2C3=((struct Cyc_Absyn_Or_e_struct*)_tmp2B1)->f2;_LL1EF: Cyc_Toc_exp_to_c(
nv,_tmp2C2);Cyc_Toc_exp_to_c(nv,_tmp2C3);goto _LL1DB;_LL1F0: if(*((int*)_tmp2B1)!= 
9)goto _LL1F2;_tmp2C4=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp2B1)->f1;_tmp2C5=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp2B1)->f2;_LL1F1: Cyc_Toc_exp_to_c(nv,_tmp2C4);
Cyc_Toc_exp_to_c(nv,_tmp2C5);goto _LL1DB;_LL1F2: if(*((int*)_tmp2B1)!= 10)goto
_LL1F4;_tmp2C6=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp2B1)->f1;_tmp2C7=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp2B1)->f2;_LL1F3: _tmp2C8=_tmp2C6;
_tmp2C9=_tmp2C7;goto _LL1F5;_LL1F4: if(*((int*)_tmp2B1)!= 11)goto _LL1F6;_tmp2C8=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp2B1)->f1;_tmp2C9=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp2B1)->f2;_tmp2CA=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2B1)->f3;if(_tmp2CA
!= 0)goto _LL1F6;_LL1F5: Cyc_Toc_exp_to_c(nv,_tmp2C8);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp2C9);goto _LL1DB;_LL1F6: if(*((int*)_tmp2B1)!= 11)goto
_LL1F8;_tmp2CB=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2B1)->f1;_tmp2CC=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp2B1)->f2;_tmp2CD=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp2B1)->f3;if(_tmp2CD == 0)goto _LL1F8;_tmp2CE=*_tmp2CD;_tmp2CF=_tmp2CE.num_varargs;
_tmp2D0=_tmp2CE.injectors;_tmp2D1=_tmp2CE.vai;_LL1F7:{struct _RegionHandle _tmp34A=
_new_region("r");struct _RegionHandle*r=& _tmp34A;_push_region(r);{struct _tuple1*
argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp2CF,0);
void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp2D1->type);void*arr_type=Cyc_Absyn_array_typ(
cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)num_varargs_exp,Cyc_Absyn_false_conref,
0);int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2CC);int
num_normargs=num_args - _tmp2CF;struct Cyc_List_List*new_args=0;{int i=0;for(0;i < 
num_normargs;(++ i,_tmp2CC=_tmp2CC->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2CC))->hd);new_args=({struct Cyc_List_List*
_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B->hd=(struct Cyc_Absyn_Exp*)_tmp2CC->hd;
_tmp34B->tl=new_args;_tmp34B;});}}new_args=({struct Cyc_List_List*_tmp34C=
_cycalloc(sizeof(*_tmp34C));_tmp34C->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp34D[3];_tmp34D[2]=num_varargs_exp;_tmp34D[1]=Cyc_Absyn_sizeoftyp_exp(
cva_type,0);_tmp34D[0]=argvexp;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp34D,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp34C->tl=new_args;
_tmp34C;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_args);Cyc_Toc_exp_to_c(nv,_tmp2CB);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(_tmp2CB,new_args,0),0);if(_tmp2D1->inject){struct Cyc_Absyn_Tuniondecl*
tud;{void*_tmp34E=Cyc_Tcutil_compress((void*)_tmp2D1->type);struct Cyc_Absyn_TunionInfo
_tmp34F;void*_tmp350;struct Cyc_Absyn_Tuniondecl**_tmp351;struct Cyc_Absyn_Tuniondecl*
_tmp352;_LL271: if(_tmp34E <= (void*)4  || *((int*)_tmp34E)!= 2)goto _LL273;_tmp34F=((
struct Cyc_Absyn_TunionType_struct*)_tmp34E)->f1;_tmp350=(void*)_tmp34F.tunion_info;
if(*((int*)_tmp350)!= 1)goto _LL273;_tmp351=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp350)->f1;_tmp352=*_tmp351;_LL272: tud=_tmp352;goto _LL270;_LL273:;_LL274:({
void*_tmp353[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp354="toc: unknown tunion in vararg with inject";_tag_arr(_tmp354,
sizeof(char),_get_zero_arr_size(_tmp354,42));}),_tag_arr(_tmp353,sizeof(void*),0));});
_LL270:;}{struct _tagged_arr vs=({unsigned int _tmp35C=(unsigned int)_tmp2CF;struct
_tuple1**_tmp35D=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct
_tuple1*),_tmp35C));struct _tagged_arr _tmp35F=_tag_arr(_tmp35D,sizeof(struct
_tuple1*),_tmp35C);{unsigned int _tmp35E=_tmp35C;unsigned int i;for(i=0;i < _tmp35E;
i ++){_tmp35D[i]=Cyc_Toc_temp_var();}}_tmp35F;});struct Cyc_List_List*_tmp355=0;{
int i=_tmp2CF - 1;for(0;i >= 0;-- i){_tmp355=({struct Cyc_List_List*_tmp356=_cycalloc(
sizeof(*_tmp356));_tmp356->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((
struct _tuple1**)_check_unknown_subscript(vs,sizeof(struct _tuple1*),i)),0),0));
_tmp356->tl=_tmp355;_tmp356;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct
Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp355,0),s,0);{int i=0;for(0;
_tmp2CC != 0;(((_tmp2CC=_tmp2CC->tl,_tmp2D0=_tmp2D0->tl)),++ i)){struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)_tmp2CC->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_unknown_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp358;struct _tuple1*
_tmp359;struct Cyc_List_List*_tmp35A;struct Cyc_Absyn_Tunionfield*_tmp357=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp2D0))->hd;_tmp358=*
_tmp357;_tmp359=_tmp358.name;_tmp35A=_tmp358.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp35A))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(
1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp359,1),0),s,0);s=Cyc_Absyn_declare_stmt(
var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp359,({const char*_tmp35B="_struct";
_tag_arr(_tmp35B,sizeof(char),_get_zero_arr_size(_tmp35B,8));}))),0,s,0);}}}}}
else{{int i=0;for(0;_tmp2CC != 0;(_tmp2CC=_tmp2CC->tl,++ i)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp2CC->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp2CC->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL1DB;_LL1F8: if(*((
int*)_tmp2B1)!= 12)goto _LL1FA;_tmp2D2=((struct Cyc_Absyn_Throw_e_struct*)_tmp2B1)->f1;
_LL1F9: Cyc_Toc_exp_to_c(nv,_tmp2D2);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp2D2),0))->r));goto _LL1DB;
_LL1FA: if(*((int*)_tmp2B1)!= 13)goto _LL1FC;_tmp2D3=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp2B1)->f1;_LL1FB: Cyc_Toc_exp_to_c(nv,_tmp2D3);goto _LL1DB;_LL1FC: if(*((int*)
_tmp2B1)!= 14)goto _LL1FE;_tmp2D4=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2B1)->f1;
_tmp2D5=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2B1)->f2;_LL1FD: Cyc_Toc_exp_to_c(
nv,_tmp2D4);for(0;_tmp2D5 != 0;_tmp2D5=_tmp2D5->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp2D5->hd);if(k != (void*)4  && k != (void*)3){{void*_tmp360=Cyc_Tcutil_compress((
void*)_tmp2D5->hd);_LL276: if(_tmp360 <= (void*)4  || *((int*)_tmp360)!= 1)goto
_LL278;_LL277: goto _LL279;_LL278: if(_tmp360 <= (void*)4  || *((int*)_tmp360)!= 2)
goto _LL27A;_LL279: continue;_LL27A:;_LL27B:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp2D4,
0))->r));goto _LL275;_LL275:;}break;}}goto _LL1DB;_LL1FE: if(*((int*)_tmp2B1)!= 15)
goto _LL200;_tmp2D6=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2B1)->f1;_tmp2D7=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2B1)->f1);_tmp2D8=((struct
Cyc_Absyn_Cast_e_struct*)_tmp2B1)->f2;_tmp2D9=((struct Cyc_Absyn_Cast_e_struct*)
_tmp2B1)->f3;_tmp2DA=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2B1)->f4;_LL1FF: {
void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2D8->topt))->v;void*
new_typ=*_tmp2D7;*_tmp2D7=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp2D8);{
struct _tuple0 _tmp362=({struct _tuple0 _tmp361;_tmp361.f1=Cyc_Tcutil_compress(
old_t2);_tmp361.f2=Cyc_Tcutil_compress(new_typ);_tmp361;});void*_tmp363;struct
Cyc_Absyn_PtrInfo _tmp364;void*_tmp365;struct Cyc_Absyn_PtrInfo _tmp366;void*
_tmp367;struct Cyc_Absyn_PtrInfo _tmp368;void*_tmp369;_LL27D: _tmp363=_tmp362.f1;
if(_tmp363 <= (void*)4  || *((int*)_tmp363)!= 4)goto _LL27F;_tmp364=((struct Cyc_Absyn_PointerType_struct*)
_tmp363)->f1;_tmp365=_tmp362.f2;if(_tmp365 <= (void*)4  || *((int*)_tmp365)!= 4)
goto _LL27F;_tmp366=((struct Cyc_Absyn_PointerType_struct*)_tmp365)->f1;_LL27E: {
int _tmp36A=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(
_tmp364.ptr_atts).nullable);int _tmp36B=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp366.ptr_atts).nullable);void*_tmp36C=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp364.ptr_atts).bounds);void*_tmp36D=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp366.ptr_atts).bounds);int _tmp36E=((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp364.ptr_atts).zero_term);int
_tmp36F=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp366.ptr_atts).zero_term);{
struct _tuple0 _tmp371=({struct _tuple0 _tmp370;_tmp370.f1=_tmp36C;_tmp370.f2=
_tmp36D;_tmp370;});void*_tmp372;struct Cyc_Absyn_Exp*_tmp373;void*_tmp374;struct
Cyc_Absyn_Exp*_tmp375;void*_tmp376;struct Cyc_Absyn_Exp*_tmp377;void*_tmp378;void*
_tmp379;void*_tmp37A;struct Cyc_Absyn_Exp*_tmp37B;void*_tmp37C;void*_tmp37D;void*
_tmp37E;void*_tmp37F;void*_tmp380;void*_tmp381;void*_tmp382;void*_tmp383;_LL284:
_tmp372=_tmp371.f1;if(_tmp372 <= (void*)1  || *((int*)_tmp372)!= 0)goto _LL286;
_tmp373=((struct Cyc_Absyn_Upper_b_struct*)_tmp372)->f1;_tmp374=_tmp371.f2;if(
_tmp374 <= (void*)1  || *((int*)_tmp374)!= 0)goto _LL286;_tmp375=((struct Cyc_Absyn_Upper_b_struct*)
_tmp374)->f1;_LL285: if(_tmp36A  && !_tmp36B){if(nv->toplevel)({void*_tmp384[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*
_tmp385="can't do NULL-check conversion at top-level";_tag_arr(_tmp385,sizeof(
char),_get_zero_arr_size(_tmp385,44));}),_tag_arr(_tmp384,sizeof(void*),0));});
if(_tmp2DA != (void*)2)({struct Cyc_String_pa_struct _tmp388;_tmp388.tag=0;_tmp388.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e));{void*_tmp386[1]={&
_tmp388};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp387="null-check conversion mis-classified: %s";_tag_arr(_tmp387,
sizeof(char),_get_zero_arr_size(_tmp387,41));}),_tag_arr(_tmp386,sizeof(void*),1));}});{
int do_null_check=Cyc_Toc_need_null_check(_tmp2D8);if(do_null_check){if(!_tmp2D9)({
void*_tmp389[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp38A="inserted null check due to implicit cast from * to @ type";
_tag_arr(_tmp38A,sizeof(char),_get_zero_arr_size(_tmp38A,58));}),_tag_arr(
_tmp389,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(*
_tmp2D7,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp38B=
_cycalloc(sizeof(*_tmp38B));_tmp38B->hd=_tmp2D8;_tmp38B->tl=0;_tmp38B;}),0)))->r));}}}
goto _LL283;_LL286: _tmp376=_tmp371.f1;if(_tmp376 <= (void*)1  || *((int*)_tmp376)!= 
0)goto _LL288;_tmp377=((struct Cyc_Absyn_Upper_b_struct*)_tmp376)->f1;_tmp378=
_tmp371.f2;if((int)_tmp378 != 0)goto _LL288;_LL287: if(_tmp2DA == (void*)2)({struct
Cyc_String_pa_struct _tmp38E;_tmp38E.tag=0;_tmp38E.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_exp2string(e));{void*_tmp38C[1]={& _tmp38E};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp38D="conversion mis-classified as null-check: %s";
_tag_arr(_tmp38D,sizeof(char),_get_zero_arr_size(_tmp38D,44));}),_tag_arr(
_tmp38C,sizeof(void*),1));}});if(nv->toplevel){if((_tmp36E  && !(_tmp366.elt_tq).real_const)
 && !_tmp36F)_tmp377=Cyc_Absyn_prim2_exp((void*)2,_tmp377,Cyc_Absyn_uint_exp(1,0),
0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_t2,_tmp377,
_tmp2D8))->r));}else{if(_tmp36E){struct _tuple1*_tmp38F=Cyc_Toc_temp_var();struct
Cyc_Absyn_Exp*_tmp390=Cyc_Absyn_var_exp(_tmp38F,0);struct Cyc_Absyn_Exp*_tmp391=
Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*_tmp396[
2];_tmp396[1]=_tmp377;_tmp396[0]=_tmp390;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp396,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);if(!_tmp36F  && !(_tmp366.elt_tq).real_const)_tmp391=Cyc_Absyn_prim2_exp((void*)
2,_tmp391,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp392=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp366.elt_typ),0);struct Cyc_Absyn_Exp*_tmp393=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp395[3];_tmp395[2]=_tmp391;_tmp395[1]=
_tmp392;_tmp395[0]=_tmp390;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp395,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp394=Cyc_Absyn_exp_stmt(_tmp393,0);_tmp394=Cyc_Absyn_declare_stmt(_tmp38F,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp2D8,_tmp394,0);(void*)(e->r=(void*)((void*)(
Cyc_Absyn_stmt_exp(_tmp394,0))->r));}}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp397[3];_tmp397[2]=_tmp377;_tmp397[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)_tmp366.elt_typ),0);_tmp397[0]=
_tmp2D8;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp397,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}goto _LL283;_LL288: _tmp379=
_tmp371.f1;if((int)_tmp379 != 0)goto _LL28A;_tmp37A=_tmp371.f2;if(_tmp37A <= (void*)
1  || *((int*)_tmp37A)!= 0)goto _LL28A;_tmp37B=((struct Cyc_Absyn_Upper_b_struct*)
_tmp37A)->f1;_LL289: if(nv->toplevel)({void*_tmp398[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp399="can't coerce t? to t* or t@ at the top-level";
_tag_arr(_tmp399,sizeof(char),_get_zero_arr_size(_tmp399,45));}),_tag_arr(
_tmp398,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp39A=_tmp37B;if(_tmp36E  && 
!_tmp36F)_tmp39A=Cyc_Absyn_add_exp(_tmp37B,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp39B=Cyc_Absyn_fncall_exp(Cyc_Toc__untag_arr_e,({struct Cyc_Absyn_Exp*_tmp39D[
3];_tmp39D[2]=_tmp39A;_tmp39D[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)
_tmp364.elt_typ),0);_tmp39D[0]=_tmp2D8;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp39D,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);if(_tmp36B)(void*)(_tmp39B->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->hd=Cyc_Absyn_copy_exp(
_tmp39B);_tmp39C->tl=0;_tmp39C;}),0))->r));(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(*
_tmp2D7,_tmp39B))->r));goto _LL283;}}_LL28A: _tmp37C=_tmp371.f1;if((int)_tmp37C != 
0)goto _LL28C;_tmp37D=_tmp371.f2;if((int)_tmp37D != 0)goto _LL28C;_LL28B: if((
_tmp36E  && !_tmp36F) && !(_tmp366.elt_tq).real_const){if(nv->toplevel)({void*
_tmp39E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp39F="can't coerce a ?ZEROTERM to a non-const ?NOZEROTERM at toplevel";
_tag_arr(_tmp39F,sizeof(char),_get_zero_arr_size(_tmp39F,64));}),_tag_arr(
_tmp39E,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_ptr_decrease_size_e,({struct Cyc_Absyn_Exp*_tmp3A0[3];_tmp3A0[2]=
Cyc_Absyn_uint_exp(1,0);_tmp3A0[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp364.elt_typ),0);_tmp3A0[0]=_tmp2D8;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp3A0,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));}goto _LL283;_LL28C: _tmp37E=_tmp371.f1;if(_tmp37E <= (void*)1  || *((int*)
_tmp37E)!= 1)goto _LL28E;_tmp37F=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp37E)->f1;_tmp380=_tmp371.f2;if(_tmp380 <= (void*)1  || *((int*)_tmp380)!= 1)
goto _LL28E;_tmp381=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp380)->f1;
_LL28D: if(_tmp36A  && !_tmp36B){if(nv->toplevel)({void*_tmp3A1[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp3A2="can't do NULL-check conversion at top-level";
_tag_arr(_tmp3A2,sizeof(char),_get_zero_arr_size(_tmp3A2,44));}),_tag_arr(
_tmp3A1,sizeof(void*),0));});if(_tmp2DA != (void*)2)({struct Cyc_String_pa_struct
_tmp3A5;_tmp3A5.tag=0;_tmp3A5.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(
e));{void*_tmp3A3[1]={& _tmp3A5};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(({const char*_tmp3A4="null-check conversion mis-classified: %s";
_tag_arr(_tmp3A4,sizeof(char),_get_zero_arr_size(_tmp3A4,41));}),_tag_arr(
_tmp3A3,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(_tmp2D8);
if(do_null_check){if(!_tmp2D9)({void*_tmp3A6[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp3A7="inserted null check due to implicit cast from * to @ type";
_tag_arr(_tmp3A7,sizeof(char),_get_zero_arr_size(_tmp3A7,58));}),_tag_arr(
_tmp3A6,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(*
_tmp2D7,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp3A8=
_cycalloc(sizeof(*_tmp3A8));_tmp3A8->hd=_tmp2D8;_tmp3A8->tl=0;_tmp3A8;}),0)))->r));}}}
goto _LL283;_LL28E: _tmp382=_tmp371.f1;if(_tmp382 <= (void*)1  || *((int*)_tmp382)!= 
1)goto _LL290;_LL28F:({struct Cyc_String_pa_struct _tmp3AB;_tmp3AB.tag=0;_tmp3AB.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Position_string_of_segment(_tmp2D8->loc));{
void*_tmp3A9[1]={& _tmp3AB};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_unimp)(({const char*_tmp3AA="%s: toc, cast from AbsUpper_b";_tag_arr(
_tmp3AA,sizeof(char),_get_zero_arr_size(_tmp3AA,30));}),_tag_arr(_tmp3A9,sizeof(
void*),1));}});_LL290: _tmp383=_tmp371.f2;if(_tmp383 <= (void*)1  || *((int*)
_tmp383)!= 1)goto _LL283;_LL291: goto _LL283;_LL283:;}goto _LL27C;}_LL27F: _tmp367=
_tmp362.f1;if(_tmp367 <= (void*)4  || *((int*)_tmp367)!= 4)goto _LL281;_tmp368=((
struct Cyc_Absyn_PointerType_struct*)_tmp367)->f1;_tmp369=_tmp362.f2;if(_tmp369 <= (
void*)4  || *((int*)_tmp369)!= 5)goto _LL281;_LL280:{void*_tmp3AC=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp368.ptr_atts).bounds);_LL293: if((int)_tmp3AC != 0)goto
_LL295;_LL294:(void*)(_tmp2D8->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((
void*)_tmp2D8->r,_tmp2D8->loc),Cyc_Toc_curr_sp,0))->r));goto _LL292;_LL295:;
_LL296: goto _LL292;_LL292:;}goto _LL27C;_LL281:;_LL282: goto _LL27C;_LL27C:;}goto
_LL1DB;}_LL200: if(*((int*)_tmp2B1)!= 16)goto _LL202;_tmp2DB=((struct Cyc_Absyn_Address_e_struct*)
_tmp2B1)->f1;_LL201:{void*_tmp3AD=(void*)_tmp2DB->r;struct _tuple1*_tmp3AE;struct
Cyc_List_List*_tmp3AF;struct Cyc_List_List*_tmp3B0;struct Cyc_List_List*_tmp3B1;
_LL298: if(*((int*)_tmp3AD)!= 30)goto _LL29A;_tmp3AE=((struct Cyc_Absyn_Struct_e_struct*)
_tmp3AD)->f1;_tmp3AF=((struct Cyc_Absyn_Struct_e_struct*)_tmp3AD)->f2;_tmp3B0=((
struct Cyc_Absyn_Struct_e_struct*)_tmp3AD)->f3;_LL299: if(nv->toplevel)({struct Cyc_String_pa_struct
_tmp3B4;_tmp3B4.tag=0;_tmp3B4.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp2DB->loc));{void*_tmp3B2[1]={& _tmp3B4};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp3B3="%s: & on non-identifiers at the top-level";
_tag_arr(_tmp3B3,sizeof(char),_get_zero_arr_size(_tmp3B3,42));}),_tag_arr(
_tmp3B2,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2DB->topt))->v,_tmp3AF != 0,1,0,
_tmp3B0,_tmp3AE))->r));goto _LL297;_LL29A: if(*((int*)_tmp3AD)!= 26)goto _LL29C;
_tmp3B1=((struct Cyc_Absyn_Tuple_e_struct*)_tmp3AD)->f1;_LL29B: if(nv->toplevel)({
struct Cyc_String_pa_struct _tmp3B7;_tmp3B7.tag=0;_tmp3B7.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Position_string_of_segment(_tmp2DB->loc));{void*_tmp3B5[1]={&
_tmp3B7};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp3B6="%s: & on non-identifiers at the top-level";_tag_arr(_tmp3B6,
sizeof(char),_get_zero_arr_size(_tmp3B6,42));}),_tag_arr(_tmp3B5,sizeof(void*),1));}});(
void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,0,_tmp3B1))->r));goto _LL297;
_LL29C:;_LL29D: Cyc_Toc_exp_to_c(nv,_tmp2DB);if(!Cyc_Absyn_is_lvalue(_tmp2DB)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp2DB,0,Cyc_Toc_address_lvalue,
1);(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp2DB))->r));}goto _LL297;_LL297:;}goto
_LL1DB;_LL202: if(*((int*)_tmp2B1)!= 17)goto _LL204;_tmp2DC=((struct Cyc_Absyn_New_e_struct*)
_tmp2B1)->f1;_tmp2DD=((struct Cyc_Absyn_New_e_struct*)_tmp2B1)->f2;_LL203: if(nv->toplevel)({
struct Cyc_String_pa_struct _tmp3BA;_tmp3BA.tag=0;_tmp3BA.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Position_string_of_segment(_tmp2DD->loc));{void*_tmp3B8[1]={&
_tmp3BA};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp3B9="%s: new at top-level";_tag_arr(_tmp3B9,sizeof(char),
_get_zero_arr_size(_tmp3B9,21));}),_tag_arr(_tmp3B8,sizeof(void*),1));}});{void*
_tmp3BB=(void*)_tmp2DD->r;struct Cyc_List_List*_tmp3BC;struct Cyc_Absyn_Vardecl*
_tmp3BD;struct Cyc_Absyn_Exp*_tmp3BE;struct Cyc_Absyn_Exp*_tmp3BF;int _tmp3C0;
struct _tuple1*_tmp3C1;struct Cyc_List_List*_tmp3C2;struct Cyc_List_List*_tmp3C3;
struct Cyc_List_List*_tmp3C4;_LL29F: if(*((int*)_tmp3BB)!= 28)goto _LL2A1;_tmp3BC=((
struct Cyc_Absyn_Array_e_struct*)_tmp3BB)->f1;_LL2A0: {struct _tuple1*_tmp3C5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3C6=Cyc_Absyn_var_exp(_tmp3C5,0);struct Cyc_Absyn_Stmt*
_tmp3C7=Cyc_Toc_init_array(nv,_tmp3C6,_tmp3BC,Cyc_Absyn_exp_stmt(_tmp3C6,0));
void*old_elt_typ;{void*_tmp3C8=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp3C9;void*_tmp3CA;struct Cyc_Absyn_Tqual _tmp3CB;struct Cyc_Absyn_PtrAtts _tmp3CC;
struct Cyc_Absyn_Conref*_tmp3CD;_LL2AA: if(_tmp3C8 <= (void*)4  || *((int*)_tmp3C8)
!= 4)goto _LL2AC;_tmp3C9=((struct Cyc_Absyn_PointerType_struct*)_tmp3C8)->f1;
_tmp3CA=(void*)_tmp3C9.elt_typ;_tmp3CB=_tmp3C9.elt_tq;_tmp3CC=_tmp3C9.ptr_atts;
_tmp3CD=_tmp3CC.zero_term;_LL2AB: old_elt_typ=_tmp3CA;goto _LL2A9;_LL2AC:;_LL2AD:
old_elt_typ=({void*_tmp3CE[0]={};Cyc_Toc_toc_impos(({const char*_tmp3CF="exp_to_c:new array expression doesn't have ptr type";
_tag_arr(_tmp3CF,sizeof(char),_get_zero_arr_size(_tmp3CF,52));}),_tag_arr(
_tmp3CE,sizeof(void*),0));});_LL2A9:;}{void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp3D0=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp3D1=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),Cyc_Absyn_signed_int_exp(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3BC),0),0);struct Cyc_Absyn_Exp*
e1;if(_tmp2DC == 0)e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp3D1);else{struct Cyc_Absyn_Exp*
r=(struct Cyc_Absyn_Exp*)_tmp2DC;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,
_tmp3D1);}(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
_tmp3C5,_tmp3D0,(struct Cyc_Absyn_Exp*)e1,_tmp3C7,0),0))->r));goto _LL29E;}}_LL2A1:
if(*((int*)_tmp3BB)!= 29)goto _LL2A3;_tmp3BD=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3BB)->f1;_tmp3BE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3BB)->f2;
_tmp3BF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3BB)->f3;_tmp3C0=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp3BB)->f4;_LL2A2: {int is_tagged_ptr=0;{void*
_tmp3D2=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp3D3;void*_tmp3D4;
struct Cyc_Absyn_Tqual _tmp3D5;struct Cyc_Absyn_PtrAtts _tmp3D6;struct Cyc_Absyn_Conref*
_tmp3D7;struct Cyc_Absyn_Conref*_tmp3D8;_LL2AF: if(_tmp3D2 <= (void*)4  || *((int*)
_tmp3D2)!= 4)goto _LL2B1;_tmp3D3=((struct Cyc_Absyn_PointerType_struct*)_tmp3D2)->f1;
_tmp3D4=(void*)_tmp3D3.elt_typ;_tmp3D5=_tmp3D3.elt_tq;_tmp3D6=_tmp3D3.ptr_atts;
_tmp3D7=_tmp3D6.bounds;_tmp3D8=_tmp3D6.zero_term;_LL2B0: is_tagged_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp3D7)== (void*)0;goto _LL2AE;_LL2B1:;_LL2B2:({void*
_tmp3D9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3DA="exp_to_c: comprehension not an array type";_tag_arr(_tmp3DA,
sizeof(char),_get_zero_arr_size(_tmp3DA,42));}),_tag_arr(_tmp3D9,sizeof(void*),0));});
_LL2AE:;}{struct _tuple1*max=Cyc_Toc_temp_var();struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3BF->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp3BE);{struct Cyc_Absyn_Exp*_tmp3DB=
Cyc_Absyn_var_exp(max,0);if(_tmp3C0)_tmp3DB=Cyc_Absyn_add_exp(_tmp3DB,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp3BD,Cyc_Absyn_var_exp(max,0),_tmp3BF,_tmp3C0,Cyc_Toc_skip_stmt_dl(),1);{
struct _RegionHandle _tmp3DC=_new_region("r");struct _RegionHandle*r=& _tmp3DC;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp3EC=
_region_malloc(r,sizeof(*_tmp3EC));_tmp3EC->hd=({struct _tuple10*_tmp3ED=
_region_malloc(r,sizeof(*_tmp3ED));_tmp3ED->f1=max;_tmp3ED->f2=Cyc_Absyn_uint_typ;
_tmp3ED->f3=(struct Cyc_Absyn_Exp*)_tmp3BE;_tmp3ED;});_tmp3EC->tl=0;_tmp3EC;});
struct Cyc_Absyn_Exp*ai;if(_tmp2DC == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp3DD[2];_tmp3DD[1]=_tmp3DB;
_tmp3DD[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp3DD,sizeof(struct Cyc_Absyn_Exp*),2));}),
0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp2DC;Cyc_Toc_exp_to_c(nv,
r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct
Cyc_Absyn_Exp*_tmp3DE[2];_tmp3DE[1]=_tmp3DB;_tmp3DE[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp3DE,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(
ptr_typ,ai);decls=({struct Cyc_List_List*_tmp3DF=_region_malloc(r,sizeof(*_tmp3DF));
_tmp3DF->hd=({struct _tuple10*_tmp3E0=_region_malloc(r,sizeof(*_tmp3E0));_tmp3E0->f1=
a;_tmp3E0->f2=ptr_typ;_tmp3E0->f3=(struct Cyc_Absyn_Exp*)ainit;_tmp3E0;});_tmp3DF->tl=
decls;_tmp3DF;});if(is_tagged_ptr){struct _tuple1*_tmp3E1=Cyc_Toc_temp_var();void*
_tmp3E2=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp3E3=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp3E6[3];_tmp3E6[2]=_tmp3DB;_tmp3E6[1]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp3E6[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp3E6,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);decls=({struct Cyc_List_List*_tmp3E4=_region_malloc(r,sizeof(*_tmp3E4));
_tmp3E4->hd=({struct _tuple10*_tmp3E5=_region_malloc(r,sizeof(*_tmp3E5));_tmp3E5->f1=
_tmp3E1;_tmp3E5->f2=_tmp3E2;_tmp3E5->f3=(struct Cyc_Absyn_Exp*)_tmp3E3;_tmp3E5;});
_tmp3E4->tl=decls;_tmp3E4;});s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
_tmp3E1,0),0),0);}else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
a,0),0),0);}{struct Cyc_List_List*_tmp3E7=decls;for(0;_tmp3E7 != 0;_tmp3E7=_tmp3E7->tl){
struct _tuple1*_tmp3E9;void*_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EB;struct _tuple10
_tmp3E8=*((struct _tuple10*)_tmp3E7->hd);_tmp3E9=_tmp3E8.f1;_tmp3EA=_tmp3E8.f2;
_tmp3EB=_tmp3E8.f3;s=Cyc_Absyn_declare_stmt(_tmp3E9,_tmp3EA,_tmp3EB,s,0);}}(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL29E;}}}}
_LL2A3: if(*((int*)_tmp3BB)!= 30)goto _LL2A5;_tmp3C1=((struct Cyc_Absyn_Struct_e_struct*)
_tmp3BB)->f1;_tmp3C2=((struct Cyc_Absyn_Struct_e_struct*)_tmp3BB)->f2;_tmp3C3=((
struct Cyc_Absyn_Struct_e_struct*)_tmp3BB)->f3;_LL2A4:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2DD->topt))->v,
_tmp3C2 != 0,1,_tmp2DC,_tmp3C3,_tmp3C1))->r));goto _LL29E;_LL2A5: if(*((int*)
_tmp3BB)!= 26)goto _LL2A7;_tmp3C4=((struct Cyc_Absyn_Tuple_e_struct*)_tmp3BB)->f1;
_LL2A6:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,_tmp2DC,_tmp3C4))->r));
goto _LL29E;_LL2A7:;_LL2A8: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2DD->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct
_tuple1*_tmp3EE=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3EF=Cyc_Absyn_var_exp(
_tmp3EE,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(
_tmp3EF,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp2DC == 0)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp2DC;Cyc_Toc_exp_to_c(
nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp3F0=(void*)_tmp2DD->r;
void*_tmp3F1;struct Cyc_Absyn_Exp*_tmp3F2;_LL2B4: if(*((int*)_tmp3F0)!= 15)goto
_LL2B6;_tmp3F1=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp3F0)->f1;_tmp3F2=((
struct Cyc_Absyn_Cast_e_struct*)_tmp3F0)->f2;_LL2B5:{struct _tuple0 _tmp3F4=({
struct _tuple0 _tmp3F3;_tmp3F3.f1=Cyc_Tcutil_compress(_tmp3F1);_tmp3F3.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F2->topt))->v);_tmp3F3;});void*
_tmp3F5;struct Cyc_Absyn_PtrInfo _tmp3F6;void*_tmp3F7;struct Cyc_Absyn_PtrAtts
_tmp3F8;struct Cyc_Absyn_Conref*_tmp3F9;void*_tmp3FA;struct Cyc_Absyn_PtrInfo
_tmp3FB;struct Cyc_Absyn_PtrAtts _tmp3FC;struct Cyc_Absyn_Conref*_tmp3FD;_LL2B9:
_tmp3F5=_tmp3F4.f1;if(_tmp3F5 <= (void*)4  || *((int*)_tmp3F5)!= 4)goto _LL2BB;
_tmp3F6=((struct Cyc_Absyn_PointerType_struct*)_tmp3F5)->f1;_tmp3F7=(void*)
_tmp3F6.elt_typ;_tmp3F8=_tmp3F6.ptr_atts;_tmp3F9=_tmp3F8.bounds;_tmp3FA=_tmp3F4.f2;
if(_tmp3FA <= (void*)4  || *((int*)_tmp3FA)!= 4)goto _LL2BB;_tmp3FB=((struct Cyc_Absyn_PointerType_struct*)
_tmp3FA)->f1;_tmp3FC=_tmp3FB.ptr_atts;_tmp3FD=_tmp3FC.bounds;_LL2BA:{struct
_tuple0 _tmp3FF=({struct _tuple0 _tmp3FE;_tmp3FE.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp3F9);_tmp3FE.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp3FD);_tmp3FE;});
void*_tmp400;void*_tmp401;struct Cyc_Absyn_Exp*_tmp402;_LL2BE: _tmp400=_tmp3FF.f1;
if((int)_tmp400 != 0)goto _LL2C0;_tmp401=_tmp3FF.f2;if(_tmp401 <= (void*)1  || *((
int*)_tmp401)!= 0)goto _LL2C0;_tmp402=((struct Cyc_Absyn_Upper_b_struct*)_tmp401)->f1;
_LL2BF: Cyc_Toc_exp_to_c(nv,_tmp3F2);(void*)(inner_mexp->r=(void*)((void*)(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0))->r));done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__init_tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp403[4];_tmp403[3]=_tmp402;_tmp403[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp3F7),0);_tmp403[1]=_tmp3F2;_tmp403[0]=mexp;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp403,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))->r));goto _LL2BD;_LL2C0:;_LL2C1: goto _LL2BD;_LL2BD:;}goto _LL2B8;_LL2BB:;
_LL2BC: goto _LL2B8;_LL2B8:;}goto _LL2B3;_LL2B6:;_LL2B7: goto _LL2B3;_LL2B3:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp404=Cyc_Absyn_exp_stmt(_tmp3EF,0);struct Cyc_Absyn_Exp*
_tmp405=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp2DD);_tmp404=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp3EF,_tmp405,0),_tmp2DD,0),
_tmp404,0);{void*_tmp406=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp3EE,_tmp406,(struct
Cyc_Absyn_Exp*)mexp,_tmp404,0),0))->r));}}goto _LL29E;}}_LL29E:;}goto _LL1DB;
_LL204: if(*((int*)_tmp2B1)!= 19)goto _LL206;_tmp2DE=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp2B1)->f1;_LL205: Cyc_Toc_exp_to_c(nv,_tmp2DE);goto _LL1DB;_LL206: if(*((int*)
_tmp2B1)!= 18)goto _LL208;_tmp2DF=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp2B1)->f1;_LL207:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp408;_tmp408.tag=18;_tmp408.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2DF);_tmp408;});
_tmp407;})));goto _LL1DB;_LL208: if(*((int*)_tmp2B1)!= 21)goto _LL20A;_LL209:({void*
_tmp409[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp40A="__gen() in code generator";_tag_arr(_tmp40A,sizeof(char),
_get_zero_arr_size(_tmp40A,26));}),_tag_arr(_tmp409,sizeof(void*),0));});_LL20A:
if(*((int*)_tmp2B1)!= 20)goto _LL20C;_tmp2E0=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp2B1)->f1;_tmp2E1=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp2B1)->f2;
if(*((int*)_tmp2E1)!= 0)goto _LL20C;_tmp2E2=((struct Cyc_Absyn_StructField_struct*)
_tmp2E1)->f1;_LL20B:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp40C;_tmp40C.tag=20;_tmp40C.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2E0);_tmp40C.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp40D=_cycalloc(sizeof(*
_tmp40D));_tmp40D[0]=({struct Cyc_Absyn_StructField_struct _tmp40E;_tmp40E.tag=0;
_tmp40E.f1=_tmp2E2;_tmp40E;});_tmp40D;}));_tmp40C;});_tmp40B;})));goto _LL1DB;
_LL20C: if(*((int*)_tmp2B1)!= 20)goto _LL20E;_tmp2E3=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp2B1)->f1;_tmp2E4=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp2B1)->f2;
if(*((int*)_tmp2E4)!= 1)goto _LL20E;_tmp2E5=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp2E4)->f1;_LL20D:{void*_tmp40F=Cyc_Tcutil_compress(_tmp2E3);struct Cyc_Absyn_AggrInfo
_tmp410;void*_tmp411;struct Cyc_List_List*_tmp412;_LL2C3: if(_tmp40F <= (void*)4
 || *((int*)_tmp40F)!= 10)goto _LL2C5;_tmp410=((struct Cyc_Absyn_AggrType_struct*)
_tmp40F)->f1;_tmp411=(void*)_tmp410.aggr_info;_LL2C4: {struct Cyc_Absyn_Aggrdecl*
_tmp413=Cyc_Absyn_get_known_aggrdecl(_tmp411);if(_tmp413->impl == 0)({void*
_tmp414[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp415="struct fields must be known";_tag_arr(_tmp415,sizeof(char),
_get_zero_arr_size(_tmp415,28));}),_tag_arr(_tmp414,sizeof(void*),0));});_tmp412=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp413->impl))->fields;goto _LL2C6;}
_LL2C5: if(_tmp40F <= (void*)4  || *((int*)_tmp40F)!= 11)goto _LL2C7;_tmp412=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp40F)->f2;_LL2C6: {struct Cyc_Absyn_Aggrfield*
_tmp416=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmp412,(int)_tmp2E5);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp418;_tmp418.tag=20;_tmp418.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2E3);_tmp418.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp419=_cycalloc(sizeof(*
_tmp419));_tmp419[0]=({struct Cyc_Absyn_StructField_struct _tmp41A;_tmp41A.tag=0;
_tmp41A.f1=_tmp416->name;_tmp41A;});_tmp419;}));_tmp418;});_tmp417;})));goto
_LL2C2;}_LL2C7: if(_tmp40F <= (void*)4  || *((int*)_tmp40F)!= 9)goto _LL2C9;_LL2C8:(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp41B=_cycalloc(
sizeof(*_tmp41B));_tmp41B[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp41C;_tmp41C.tag=
20;_tmp41C.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2E3);_tmp41C.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D[0]=({
struct Cyc_Absyn_StructField_struct _tmp41E;_tmp41E.tag=0;_tmp41E.f1=Cyc_Absyn_fieldname((
int)(_tmp2E5 + 1));_tmp41E;});_tmp41D;}));_tmp41C;});_tmp41B;})));goto _LL2C2;
_LL2C9: if(_tmp40F <= (void*)4  || *((int*)_tmp40F)!= 3)goto _LL2CB;_LL2CA: if(
_tmp2E5 == 0)(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp420;_tmp420.tag=20;_tmp420.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2E3);_tmp420.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp421=_cycalloc(sizeof(*
_tmp421));_tmp421[0]=({struct Cyc_Absyn_StructField_struct _tmp422;_tmp422.tag=0;
_tmp422.f1=Cyc_Toc_tag_sp;_tmp422;});_tmp421;}));_tmp420;});_tmp41F;})));else{(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp423=_cycalloc(
sizeof(*_tmp423));_tmp423[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp424;_tmp424.tag=
20;_tmp424.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2E3);_tmp424.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425[0]=({
struct Cyc_Absyn_StructField_struct _tmp426;_tmp426.tag=0;_tmp426.f1=Cyc_Absyn_fieldname((
int)_tmp2E5);_tmp426;});_tmp425;}));_tmp424;});_tmp423;})));}goto _LL2C2;_LL2CB:;
_LL2CC:({void*_tmp427[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp428="impossible type for offsetof tuple index";
_tag_arr(_tmp428,sizeof(char),_get_zero_arr_size(_tmp428,41));}),_tag_arr(
_tmp427,sizeof(void*),0));});_LL2C2:;}goto _LL1DB;_LL20E: if(*((int*)_tmp2B1)!= 22)
goto _LL210;_tmp2E6=((struct Cyc_Absyn_Deref_e_struct*)_tmp2B1)->f1;_LL20F: {void*
_tmp429=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2E6->topt))->v);{
void*_tmp42A=_tmp429;struct Cyc_Absyn_PtrInfo _tmp42B;void*_tmp42C;struct Cyc_Absyn_Tqual
_tmp42D;struct Cyc_Absyn_PtrAtts _tmp42E;void*_tmp42F;struct Cyc_Absyn_Conref*
_tmp430;struct Cyc_Absyn_Conref*_tmp431;struct Cyc_Absyn_Conref*_tmp432;_LL2CE: if(
_tmp42A <= (void*)4  || *((int*)_tmp42A)!= 4)goto _LL2D0;_tmp42B=((struct Cyc_Absyn_PointerType_struct*)
_tmp42A)->f1;_tmp42C=(void*)_tmp42B.elt_typ;_tmp42D=_tmp42B.elt_tq;_tmp42E=
_tmp42B.ptr_atts;_tmp42F=(void*)_tmp42E.rgn;_tmp430=_tmp42E.nullable;_tmp431=
_tmp42E.bounds;_tmp432=_tmp42E.zero_term;_LL2CF:{void*_tmp433=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp431);_LL2D3: if(_tmp433 <= (void*)1  || *((int*)_tmp433)!= 
0)goto _LL2D5;_LL2D4: {int do_null_check=Cyc_Toc_need_null_check(_tmp2E6);Cyc_Toc_exp_to_c(
nv,_tmp2E6);if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp434[0]={};
Cyc_Tcutil_warn(e->loc,({const char*_tmp435="inserted null check due to dereference";
_tag_arr(_tmp435,sizeof(char),_get_zero_arr_size(_tmp435,39));}),_tag_arr(
_tmp434,sizeof(void*),0));});(void*)(_tmp2E6->r=(void*)((void*)(Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(_tmp429),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436->hd=Cyc_Absyn_copy_exp(_tmp2E6);
_tmp436->tl=0;_tmp436;}),0)))->r));}goto _LL2D2;}_LL2D5: if((int)_tmp433 != 0)goto
_LL2D7;_LL2D6: {struct Cyc_Absyn_Exp*_tmp437=Cyc_Absyn_uint_exp(0,0);_tmp437->topt=({
struct Cyc_Core_Opt*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->v=(void*)Cyc_Absyn_uint_typ;
_tmp438;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(_tmp2E6,_tmp437,
0))->r));Cyc_Toc_exp_to_c(nv,e);goto _LL2D2;}_LL2D7: if(_tmp433 <= (void*)1  || *((
int*)_tmp433)!= 1)goto _LL2D2;_LL2D8:({void*_tmp439[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp43A="exp_to_c: deref w/ AbsUpper_b";
_tag_arr(_tmp43A,sizeof(char),_get_zero_arr_size(_tmp43A,30));}),_tag_arr(
_tmp439,sizeof(void*),0));});_LL2D2:;}goto _LL2CD;_LL2D0:;_LL2D1:({void*_tmp43B[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmp43C="exp_to_c: Deref: non-pointer";_tag_arr(_tmp43C,sizeof(char),
_get_zero_arr_size(_tmp43C,29));}),_tag_arr(_tmp43B,sizeof(void*),0));});_LL2CD:;}
goto _LL1DB;}_LL210: if(*((int*)_tmp2B1)!= 23)goto _LL212;_tmp2E7=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2B1)->f1;_tmp2E8=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp2B1)->f2;_LL211:
Cyc_Toc_exp_to_c(nv,_tmp2E7);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1DB;
_LL212: if(*((int*)_tmp2B1)!= 24)goto _LL214;_tmp2E9=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp2B1)->f1;_tmp2EA=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2B1)->f2;_LL213: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2E9->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp2E9);Cyc_Toc_exp_to_c(nv,_tmp2E9);{
int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp43E;struct Cyc_Absyn_Tqual _tmp43F;
struct Cyc_Absyn_PtrAtts _tmp440;void*_tmp441;struct Cyc_Absyn_Conref*_tmp442;
struct Cyc_Absyn_Conref*_tmp443;struct Cyc_Absyn_Conref*_tmp444;struct Cyc_Absyn_PtrInfo
_tmp43D=Cyc_Toc_get_ptr_type(e1typ);_tmp43E=(void*)_tmp43D.elt_typ;_tmp43F=
_tmp43D.elt_tq;_tmp440=_tmp43D.ptr_atts;_tmp441=(void*)_tmp440.rgn;_tmp442=
_tmp440.nullable;_tmp443=_tmp440.bounds;_tmp444=_tmp440.zero_term;{void*_tmp445=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp443);struct Cyc_Absyn_Exp*_tmp446;
_LL2DA: if(_tmp445 <= (void*)1  || *((int*)_tmp445)!= 0)goto _LL2DC;_tmp446=((struct
Cyc_Absyn_Upper_b_struct*)_tmp445)->f1;_LL2DB: {unsigned int _tmp448;int _tmp449;
struct _tuple5 _tmp447=Cyc_Evexp_eval_const_uint_exp(_tmp446);_tmp448=_tmp447.f1;
_tmp449=_tmp447.f2;if(_tmp449){if(_tmp448 < 1)({void*_tmp44A[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp44B="exp_to_c:  AggrArrow_e on pointer of size 0";
_tag_arr(_tmp44B,sizeof(char),_get_zero_arr_size(_tmp44B,44));}),_tag_arr(
_tmp44A,sizeof(void*),0));});if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({
void*_tmp44C[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp44D="inserted null check due to dereference";
_tag_arr(_tmp44D,sizeof(char),_get_zero_arr_size(_tmp44D,39));}),_tag_arr(
_tmp44C,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2E9->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp44E=_cycalloc(sizeof(*_tmp44E));_tmp44E->hd=_tmp2E9;_tmp44E->tl=0;_tmp44E;}),
0)),_tmp2EA,0))->r));}}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2E9->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp44F[4];_tmp44F[3]=Cyc_Absyn_uint_exp(0,0);_tmp44F[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp43E,0);_tmp44F[1]=_tmp446;_tmp44F[0]=_tmp2E9;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp44F,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)),_tmp2EA,0))->r));}goto _LL2D9;}_LL2DC: if((int)_tmp445 != 0)goto _LL2DE;
_LL2DD: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp43E);(void*)(_tmp2E9->r=(void*)((
void*)(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp43F),Cyc_Absyn_fncall_exp(Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_Absyn_Exp*_tmp450[3];_tmp450[2]=Cyc_Absyn_uint_exp(0,0);_tmp450[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp450[0]=Cyc_Absyn_copy_exp(_tmp2E9);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp450,sizeof(struct Cyc_Absyn_Exp*),3));}),
0)))->r));goto _LL2D9;}_LL2DE: if(_tmp445 <= (void*)1  || *((int*)_tmp445)!= 1)goto
_LL2D9;_LL2DF:({void*_tmp451[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp452="exp_to_c: AggrArrow w/ AbsUpper_b";
_tag_arr(_tmp452,sizeof(char),_get_zero_arr_size(_tmp452,34));}),_tag_arr(
_tmp451,sizeof(void*),0));});_LL2D9:;}if(is_poly)(void*)(e->r=(void*)((void*)(
Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1DB;}}
_LL214: if(*((int*)_tmp2B1)!= 25)goto _LL216;_tmp2EB=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp2B1)->f1;_tmp2EC=((struct Cyc_Absyn_Subscript_e_struct*)_tmp2B1)->f2;_LL215: {
void*_tmp453=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2EB->topt))->v);{void*_tmp454=_tmp453;struct Cyc_List_List*_tmp455;struct Cyc_Absyn_PtrInfo
_tmp456;void*_tmp457;struct Cyc_Absyn_Tqual _tmp458;struct Cyc_Absyn_PtrAtts _tmp459;
void*_tmp45A;struct Cyc_Absyn_Conref*_tmp45B;struct Cyc_Absyn_Conref*_tmp45C;
struct Cyc_Absyn_Conref*_tmp45D;_LL2E1: if(_tmp454 <= (void*)4  || *((int*)_tmp454)
!= 9)goto _LL2E3;_tmp455=((struct Cyc_Absyn_TupleType_struct*)_tmp454)->f1;_LL2E2:
Cyc_Toc_exp_to_c(nv,_tmp2EB);Cyc_Toc_exp_to_c(nv,_tmp2EC);{unsigned int _tmp45F;
int _tmp460;struct _tuple5 _tmp45E=Cyc_Evexp_eval_const_uint_exp(_tmp2EC);_tmp45F=
_tmp45E.f1;_tmp460=_tmp45E.f2;if(!_tmp460)({void*_tmp461[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp462="unknown tuple subscript in translation to C";
_tag_arr(_tmp462,sizeof(char),_get_zero_arr_size(_tmp462,44));}),_tag_arr(
_tmp461,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
_tmp2EB,Cyc_Absyn_fieldname((int)(_tmp45F + 1)),0))->r));goto _LL2E0;}_LL2E3: if(
_tmp454 <= (void*)4  || *((int*)_tmp454)!= 4)goto _LL2E5;_tmp456=((struct Cyc_Absyn_PointerType_struct*)
_tmp454)->f1;_tmp457=(void*)_tmp456.elt_typ;_tmp458=_tmp456.elt_tq;_tmp459=
_tmp456.ptr_atts;_tmp45A=(void*)_tmp459.rgn;_tmp45B=_tmp459.nullable;_tmp45C=
_tmp459.bounds;_tmp45D=_tmp459.zero_term;_LL2E4: {struct Cyc_List_List*_tmp463=
Cyc_Toc_get_relns(_tmp2EB);int in_bnds=0;{void*_tmp464=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp45C);_LL2E8: if(_tmp464 <= (void*)1  || *((int*)_tmp464)!= 1)goto _LL2EA;_LL2E9:
goto _LL2E7;_LL2EA:;_LL2EB: in_bnds=Cyc_Toc_check_bounds(_tmp463,_tmp2EB,_tmp2EC);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)({struct Cyc_String_pa_struct _tmp467;
_tmp467.tag=0;_tmp467.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(
e));{void*_tmp465[1]={& _tmp467};Cyc_Tcutil_warn(e->loc,({const char*_tmp466="bounds check necessary for %s";
_tag_arr(_tmp466,sizeof(char),_get_zero_arr_size(_tmp466,30));}),_tag_arr(
_tmp465,sizeof(void*),1));}});_LL2E7:;}Cyc_Toc_exp_to_c(nv,_tmp2EB);Cyc_Toc_exp_to_c(
nv,_tmp2EC);Cyc_Toc_total_bounds_checks ++;{void*_tmp468=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp45C);struct Cyc_Absyn_Exp*_tmp469;void*_tmp46A;_LL2ED: if(_tmp468 <= (void*)1
 || *((int*)_tmp468)!= 0)goto _LL2EF;_tmp469=((struct Cyc_Absyn_Upper_b_struct*)
_tmp468)->f1;_LL2EE: {int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp45B);void*ta1=Cyc_Toc_typ_to_c(_tmp457);void*ta2=Cyc_Absyn_cstar_typ(
ta1,_tmp458);if(in_bnds)Cyc_Toc_bounds_checks_eliminated ++;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp45D))(void*)(e->r=(void*)((
void*)(Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp46B[3];_tmp46B[2]=_tmp2EC;_tmp46B[1]=_tmp469;_tmp46B[0]=
_tmp2EB;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp46B,sizeof(struct Cyc_Absyn_Exp*),3));}),0)),0))->r));else{if(possibly_null){
if(Cyc_Toc_warn_all_null_deref)({void*_tmp46C[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp46D="inserted null check due to dereference";_tag_arr(_tmp46D,
sizeof(char),_get_zero_arr_size(_tmp46D,39));}),_tag_arr(_tmp46C,sizeof(void*),0));});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp46E[4];_tmp46E[3]=
_tmp2EC;_tmp46E[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp46E[1]=_tmp469;_tmp46E[0]=
_tmp2EB;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp46E,sizeof(struct Cyc_Absyn_Exp*),4));}),0)),0))->r));}else{(void*)(_tmp2EC->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp46F[2];_tmp46F[1]=Cyc_Absyn_copy_exp(_tmp2EC);_tmp46F[0]=
_tmp469;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp46F,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}}}goto _LL2EC;}_LL2EF: if((
int)_tmp468 != 0)goto _LL2F1;_LL2F0: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp457);if(
in_bnds){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp458),Cyc_Absyn_aggrmember_exp(
_tmp2EB,Cyc_Toc_curr_sp,0)),_tmp2EC,0))->r));}else{(void*)(e->r=(void*)((void*)(
Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp458),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*_tmp470[3];_tmp470[2]=
_tmp2EC;_tmp470[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp470[0]=_tmp2EB;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp470,sizeof(struct Cyc_Absyn_Exp*),
3));}),0)),0))->r));}goto _LL2EC;}_LL2F1: if(_tmp468 <= (void*)1  || *((int*)_tmp468)
!= 1)goto _LL2EC;_tmp46A=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp468)->f1;
_LL2F2:{void*_tmp471=(void*)_tmp2EC->annot;struct Cyc_List_List*_tmp472;_LL2F4:
if(*((void**)_tmp471)!= Cyc_CfFlowInfo_HasTagCmps)goto _LL2F6;_tmp472=((struct Cyc_CfFlowInfo_HasTagCmps_struct*)
_tmp471)->f1;_LL2F5: for(0;_tmp472 != 0;_tmp472=_tmp472->tl){if((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp472->hd)->cmp == (void*)8  && Cyc_Tcutil_typecmp((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp472->hd)->bd,_tmp46A)== 0){in_bnds=1;break;}}goto _LL2F3;_LL2F6:;_LL2F7: goto
_LL2F3;_LL2F3:;}if(!in_bnds)({void*_tmp473[0]={};Cyc_Tcutil_terr(e->loc,({const
char*_tmp474="cannot determine that subscript is in bounds";_tag_arr(_tmp474,
sizeof(char),_get_zero_arr_size(_tmp474,45));}),_tag_arr(_tmp473,sizeof(void*),0));});
goto _LL2EC;_LL2EC:;}goto _LL2E0;}_LL2E5:;_LL2E6:({void*_tmp475[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp476="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_arr(_tmp476,sizeof(char),_get_zero_arr_size(_tmp476,49));}),_tag_arr(
_tmp475,sizeof(void*),0));});_LL2E0:;}goto _LL1DB;}_LL216: if(*((int*)_tmp2B1)!= 
26)goto _LL218;_tmp2ED=((struct Cyc_Absyn_Tuple_e_struct*)_tmp2B1)->f1;_LL217: if(!
nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,0,0,_tmp2ED))->r));
else{struct Cyc_List_List*_tmp477=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,
_tmp2ED);struct _tagged_arr*_tmp478=Cyc_Toc_add_tuple_type(_tmp477);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp2ED != 0;(_tmp2ED=_tmp2ED->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp2ED->hd);dles=({struct Cyc_List_List*_tmp479=_cycalloc(
sizeof(*_tmp479));_tmp479->hd=({struct _tuple6*_tmp47A=_cycalloc(sizeof(*_tmp47A));
_tmp47A->f1=0;_tmp47A->f2=(struct Cyc_Absyn_Exp*)_tmp2ED->hd;_tmp47A;});_tmp479->tl=
dles;_tmp479;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,dles,0))->r));}
goto _LL1DB;_LL218: if(*((int*)_tmp2B1)!= 28)goto _LL21A;_tmp2EE=((struct Cyc_Absyn_Array_e_struct*)
_tmp2B1)->f1;_LL219:(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,
_tmp2EE,0))->r));{struct Cyc_List_List*_tmp47B=_tmp2EE;for(0;_tmp47B != 0;_tmp47B=
_tmp47B->tl){struct _tuple6 _tmp47D;struct Cyc_Absyn_Exp*_tmp47E;struct _tuple6*
_tmp47C=(struct _tuple6*)_tmp47B->hd;_tmp47D=*_tmp47C;_tmp47E=_tmp47D.f2;Cyc_Toc_exp_to_c(
nv,_tmp47E);}}goto _LL1DB;_LL21A: if(*((int*)_tmp2B1)!= 29)goto _LL21C;_tmp2EF=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp2B1)->f1;_tmp2F0=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2B1)->f2;_tmp2F1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2B1)->f3;
_tmp2F2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2B1)->f4;_LL21B: {
unsigned int _tmp480;int _tmp481;struct _tuple5 _tmp47F=Cyc_Evexp_eval_const_uint_exp(
_tmp2F0);_tmp480=_tmp47F.f1;_tmp481=_tmp47F.f2;{void*_tmp482=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp2F1->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp2F1);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp2F1)){if(!_tmp481)({
void*_tmp483[0]={};Cyc_Tcutil_terr(_tmp2F0->loc,({const char*_tmp484="cannot determine value of constant";
_tag_arr(_tmp484,sizeof(char),_get_zero_arr_size(_tmp484,35));}),_tag_arr(
_tmp483,sizeof(void*),0));});{unsigned int i=0;for(0;i < _tmp480;i ++){es=({struct
Cyc_List_List*_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485->hd=({struct _tuple6*
_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486->f1=0;_tmp486->f2=_tmp2F1;_tmp486;});
_tmp485->tl=es;_tmp485;});}}if(_tmp2F2){struct Cyc_Absyn_Exp*_tmp487=Cyc_Toc_cast_it(
_tmp482,Cyc_Absyn_uint_exp(0,0));es=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp488=
_cycalloc(sizeof(*_tmp488));_tmp488->hd=({struct _tuple6*_tmp489=_cycalloc(
sizeof(*_tmp489));_tmp489->f1=0;_tmp489->f2=_tmp487;_tmp489;});_tmp488->tl=0;
_tmp488;}));}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,es,0))->r));
goto _LL1DB;}}}_LL21C: if(*((int*)_tmp2B1)!= 30)goto _LL21E;_tmp2F3=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2B1)->f1;_tmp2F4=((struct Cyc_Absyn_Struct_e_struct*)_tmp2B1)->f2;_tmp2F5=((
struct Cyc_Absyn_Struct_e_struct*)_tmp2B1)->f3;_tmp2F6=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2B1)->f4;_LL21D: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp2F4 != 0,0,0,_tmp2F5,_tmp2F3))->r));else{if(_tmp2F6 == 0)({void*
_tmp48A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp48B="Struct_e: missing aggrdecl pointer";_tag_arr(_tmp48B,sizeof(
char),_get_zero_arr_size(_tmp48B,35));}),_tag_arr(_tmp48A,sizeof(void*),0));});{
struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp2F6;struct
_RegionHandle _tmp48C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp48C;
_push_region(rgn);{struct Cyc_List_List*_tmp48D=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp2F5,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);struct Cyc_List_List*
_tmp48E=0;struct Cyc_List_List*_tmp48F=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp48F != 0;_tmp48F=_tmp48F->tl){struct Cyc_List_List*
_tmp490=_tmp48D;for(0;_tmp490 != 0;_tmp490=_tmp490->tl){if((*((struct _tuple11*)
_tmp490->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp48F->hd){struct _tuple11
_tmp492;struct Cyc_Absyn_Aggrfield*_tmp493;struct Cyc_Absyn_Exp*_tmp494;struct
_tuple11*_tmp491=(struct _tuple11*)_tmp490->hd;_tmp492=*_tmp491;_tmp493=_tmp492.f1;
_tmp494=_tmp492.f2;{void*_tmp495=(void*)_tmp493->type;Cyc_Toc_exp_to_c(nv,
_tmp494);if(Cyc_Toc_is_void_star(_tmp495))(void*)(_tmp494->r=(void*)((void*)(Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp494->r,0)))->r));_tmp48E=({
struct Cyc_List_List*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496->hd=({struct
_tuple6*_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->f1=0;_tmp497->f2=_tmp494;
_tmp497;});_tmp496->tl=_tmp48E;_tmp496;});break;}}}}(void*)(e->r=(void*)((void*)(
Cyc_Absyn_unresolvedmem_exp(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp48E),0))->r));};_pop_region(rgn);}}goto _LL1DB;_LL21E: if(*((
int*)_tmp2B1)!= 31)goto _LL220;_tmp2F7=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2B1)->f1;_tmp2F8=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2B1)->f2;_LL21F: {
struct Cyc_List_List*fs;{void*_tmp498=Cyc_Tcutil_compress(_tmp2F7);struct Cyc_List_List*
_tmp499;_LL2F9: if(_tmp498 <= (void*)4  || *((int*)_tmp498)!= 11)goto _LL2FB;_tmp499=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp498)->f2;_LL2FA: fs=_tmp499;goto _LL2F8;
_LL2FB:;_LL2FC:({struct Cyc_String_pa_struct _tmp49C;_tmp49C.tag=0;_tmp49C.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp2F7));{void*
_tmp49A[1]={& _tmp49C};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp49B="anon struct has type %s";_tag_arr(_tmp49B,sizeof(char),
_get_zero_arr_size(_tmp49B,24));}),_tag_arr(_tmp49A,sizeof(void*),1));}});_LL2F8:;}{
struct _RegionHandle _tmp49D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp49D;
_push_region(rgn);{struct Cyc_List_List*_tmp49E=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp2F8,
fs);for(0;_tmp49E != 0;_tmp49E=_tmp49E->tl){struct _tuple11 _tmp4A0;struct Cyc_Absyn_Aggrfield*
_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;struct _tuple11*_tmp49F=(struct _tuple11*)
_tmp49E->hd;_tmp4A0=*_tmp49F;_tmp4A1=_tmp4A0.f1;_tmp4A2=_tmp4A0.f2;{void*_tmp4A3=(
void*)_tmp4A1->type;Cyc_Toc_exp_to_c(nv,_tmp4A2);if(Cyc_Toc_is_void_star(_tmp4A3))(
void*)(_tmp4A2->r=(void*)((void*)(Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((
void*)_tmp4A2->r,0)))->r));}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(
0,_tmp2F8,0))->r));};_pop_region(rgn);}goto _LL1DB;}_LL220: if(*((int*)_tmp2B1)!= 
32)goto _LL222;_tmp2F9=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2B1)->f1;if(_tmp2F9
!= 0)goto _LL222;_tmp2FA=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2B1)->f2;_tmp2FB=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp2B1)->f3;_LL221: {struct _tuple1*qv=_tmp2FB->name;
struct Cyc_Absyn_Exp*tag_exp=_tmp2FA->is_xtunion?Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(
_tmp2FA,qv,0);(void*)(e->r=(void*)((void*)tag_exp->r));goto _LL1DB;}_LL222: if(*((
int*)_tmp2B1)!= 32)goto _LL224;_tmp2FC=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2B1)->f1;
_tmp2FD=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2B1)->f2;_tmp2FE=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp2B1)->f3;_LL223: {struct _tuple1*_tmp4A4=_tmp2FE->name;struct Cyc_List_List*
_tmp4A5=_tmp2FE->typs;struct _tuple1*_tmp4A6=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp4A7=Cyc_Absyn_var_exp(_tmp4A6,0);void*_tmp4A8=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp4A4,({const char*_tmp4B6="_struct";_tag_arr(_tmp4B6,sizeof(char),
_get_zero_arr_size(_tmp4B6,8));})));struct Cyc_Absyn_Exp*tag_exp=_tmp2FD->is_xtunion?
Cyc_Absyn_var_exp(_tmp4A4,0): Cyc_Toc_tunion_tag(_tmp2FD,_tmp4A4,1);if(nv->toplevel){
struct Cyc_List_List*dles=0;for(0;_tmp2FC != 0;(_tmp2FC=_tmp2FC->tl,_tmp4A5=
_tmp4A5->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp2FC->hd;void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)_check_null(
_tmp4A5))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);dles=({struct Cyc_List_List*_tmp4A9=
_cycalloc(sizeof(*_tmp4A9));_tmp4A9->hd=({struct _tuple6*_tmp4AA=_cycalloc(
sizeof(*_tmp4AA));_tmp4AA->f1=0;_tmp4AA->f2=cur_e;_tmp4AA;});_tmp4A9->tl=dles;
_tmp4A9;});}dles=({struct Cyc_List_List*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));
_tmp4AB->hd=({struct _tuple6*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC->f1=0;
_tmp4AC->f2=tag_exp;_tmp4AC;});_tmp4AB->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(dles);_tmp4AB;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD->v=Cyc_Toc_collapse_qvar_tag(
_tmp4A4,({const char*_tmp4AE="_struct";_tag_arr(_tmp4AE,sizeof(char),
_get_zero_arr_size(_tmp4AE,8));}));_tmp4AD;}),dles,0))->r));}else{struct Cyc_List_List*
_tmp4AF=({struct Cyc_List_List*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5->hd=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp4A7,Cyc_Toc_tag_sp,0),tag_exp,
0);_tmp4B5->tl=0;_tmp4B5;});{int i=1;for(0;_tmp2FC != 0;(((_tmp2FC=_tmp2FC->tl,i ++)),
_tmp4A5=_tmp4A5->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp2FC->hd;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp4A5))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp4B0=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp4A7,Cyc_Absyn_fieldname(i),0),
cur_e,0);_tmp4AF=({struct Cyc_List_List*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));
_tmp4B1->hd=_tmp4B0;_tmp4B1->tl=_tmp4AF;_tmp4B1;});}}}{struct Cyc_Absyn_Stmt*
_tmp4B2=Cyc_Absyn_exp_stmt(_tmp4A7,0);struct Cyc_Absyn_Stmt*_tmp4B3=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4->hd=_tmp4B2;_tmp4B4->tl=_tmp4AF;
_tmp4B4;})),0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
_tmp4A6,_tmp4A8,0,_tmp4B3,0),0))->r));}}goto _LL1DB;}_LL224: if(*((int*)_tmp2B1)!= 
33)goto _LL226;_LL225: goto _LL227;_LL226: if(*((int*)_tmp2B1)!= 34)goto _LL228;
_LL227: goto _LL1DB;_LL228: if(*((int*)_tmp2B1)!= 35)goto _LL22A;_tmp2FF=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp2B1)->f1;_tmp300=_tmp2FF.is_calloc;_tmp301=_tmp2FF.rgn;_tmp302=_tmp2FF.elt_type;
_tmp303=_tmp2FF.num_elts;_tmp304=_tmp2FF.fat_result;_LL229: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp302)));Cyc_Toc_exp_to_c(nv,_tmp303);if(_tmp304){struct
_tuple1*_tmp4B7=Cyc_Toc_temp_var();struct _tuple1*_tmp4B8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp300){xexp=_tmp303;if(_tmp301 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp301;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp4B7,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp302,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp4B7,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp4B9[3];_tmp4B9[2]=Cyc_Absyn_var_exp(_tmp4B7,0);_tmp4B9[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp4B9[0]=Cyc_Absyn_var_exp(_tmp4B8,0);((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp4B9,sizeof(
struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp303,0);if(_tmp301 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp301;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp4B7,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp302,Cyc_Absyn_var_exp(_tmp4B7,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp4BA[3];
_tmp4BA[2]=Cyc_Absyn_var_exp(_tmp4B7,0);_tmp4BA[1]=Cyc_Absyn_uint_exp(1,0);
_tmp4BA[0]=Cyc_Absyn_var_exp(_tmp4B8,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp4BA,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);}{struct Cyc_Absyn_Stmt*_tmp4BB=Cyc_Absyn_declare_stmt(_tmp4B7,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp4B8,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp4BB,0))->r));}}else{if(_tmp301 != 0){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp301;Cyc_Toc_exp_to_c(nv,rgn);(
void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,
0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp302,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL1DB;}_LL22A: if(*((int*)_tmp2B1)!= 37)goto _LL22C;_tmp305=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp2B1)->f1;_LL22B: Cyc_Toc_stmt_to_c(nv,
_tmp305);goto _LL1DB;_LL22C: if(*((int*)_tmp2B1)!= 36)goto _LL22E;_LL22D:({void*
_tmp4BC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4BD="UnresolvedMem";_tag_arr(_tmp4BD,sizeof(char),
_get_zero_arr_size(_tmp4BD,14));}),_tag_arr(_tmp4BC,sizeof(void*),0));});_LL22E:
if(*((int*)_tmp2B1)!= 27)goto _LL1DB;_LL22F:({void*_tmp4BE[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp4BF="compoundlit";
_tag_arr(_tmp4BF,sizeof(char),_get_zero_arr_size(_tmp4BF,12));}),_tag_arr(
_tmp4BE,sizeof(void*),0));});_LL1DB:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0);}struct _tuple12{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*
f3;};struct _tuple13{struct _tuple1*f1;void*f2;};struct _tuple14{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple12 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*
nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp4C0=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp4C1;
struct Cyc_Absyn_Vardecl _tmp4C2;struct _tuple1*_tmp4C3;struct Cyc_Absyn_Vardecl*
_tmp4C4;struct Cyc_Absyn_Vardecl _tmp4C5;struct _tuple1*_tmp4C6;struct Cyc_Absyn_Vardecl*
_tmp4C7;void*_tmp4C8;int _tmp4C9;char _tmp4CA;struct _tagged_arr _tmp4CB;struct Cyc_Absyn_Enumdecl*
_tmp4CC;struct Cyc_Absyn_Enumfield*_tmp4CD;void*_tmp4CE;struct Cyc_Absyn_Enumfield*
_tmp4CF;struct Cyc_Absyn_Tuniondecl*_tmp4D0;struct Cyc_Absyn_Tunionfield*_tmp4D1;
struct Cyc_List_List*_tmp4D2;struct Cyc_Absyn_Pat*_tmp4D3;struct Cyc_Absyn_Pat
_tmp4D4;void*_tmp4D5;struct Cyc_Absyn_Tuniondecl*_tmp4D6;struct Cyc_Absyn_Tunionfield*
_tmp4D7;struct Cyc_List_List*_tmp4D8;struct Cyc_List_List*_tmp4D9;struct Cyc_List_List*
_tmp4DA;struct Cyc_Absyn_AggrInfo _tmp4DB;void*_tmp4DC;struct Cyc_List_List*_tmp4DD;
struct Cyc_Absyn_Pat*_tmp4DE;_LL2FE: if(_tmp4C0 <= (void*)2  || *((int*)_tmp4C0)!= 0)
goto _LL300;_tmp4C1=((struct Cyc_Absyn_Var_p_struct*)_tmp4C0)->f1;_tmp4C2=*_tmp4C1;
_tmp4C3=_tmp4C2.name;_LL2FF: _tmp4C6=_tmp4C3;goto _LL301;_LL300: if(_tmp4C0 <= (void*)
2  || *((int*)_tmp4C0)!= 2)goto _LL302;_tmp4C4=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp4C0)->f2;_tmp4C5=*_tmp4C4;_tmp4C6=_tmp4C5.name;_LL301: nv=Cyc_Toc_add_varmap(
nv,_tmp4C6,r);goto _LL303;_LL302: if((int)_tmp4C0 != 0)goto _LL304;_LL303: s=Cyc_Toc_skip_stmt_dl();
goto _LL2FD;_LL304: if(_tmp4C0 <= (void*)2  || *((int*)_tmp4C0)!= 1)goto _LL306;
_tmp4C7=((struct Cyc_Absyn_Reference_p_struct*)_tmp4C0)->f1;_LL305: {struct
_tuple1*_tmp4DF=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*_tmp4E0=
_region_malloc(rgn,sizeof(*_tmp4E0));_tmp4E0->hd=({struct _tuple13*_tmp4E1=
_region_malloc(rgn,sizeof(*_tmp4E1));_tmp4E1->f1=_tmp4DF;_tmp4E1->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp4E1;});_tmp4E0->tl=decls;_tmp4E0;});nv=
Cyc_Toc_add_varmap(nv,_tmp4C7->name,Cyc_Absyn_var_exp(_tmp4DF,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp4DF,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(path,0)),0);goto _LL2FD;}_LL306: if((int)
_tmp4C0 != 1)goto _LL308;_LL307: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,
0),fail_stmt);goto _LL2FD;_LL308: if(_tmp4C0 <= (void*)2  || *((int*)_tmp4C0)!= 7)
goto _LL30A;_tmp4C8=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp4C0)->f1;_tmp4C9=((
struct Cyc_Absyn_Int_p_struct*)_tmp4C0)->f2;_LL309: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp4C8,_tmp4C9,0),fail_stmt);goto _LL2FD;_LL30A: if(_tmp4C0 <= (void*)2  || *((int*)
_tmp4C0)!= 8)goto _LL30C;_tmp4CA=((struct Cyc_Absyn_Char_p_struct*)_tmp4C0)->f1;
_LL30B: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp4CA,0),fail_stmt);goto
_LL2FD;_LL30C: if(_tmp4C0 <= (void*)2  || *((int*)_tmp4C0)!= 9)goto _LL30E;_tmp4CB=((
struct Cyc_Absyn_Float_p_struct*)_tmp4C0)->f1;_LL30D: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp4CB,0),fail_stmt);goto _LL2FD;_LL30E: if(_tmp4C0 <= (void*)2  || *((int*)_tmp4C0)
!= 10)goto _LL310;_tmp4CC=((struct Cyc_Absyn_Enum_p_struct*)_tmp4C0)->f1;_tmp4CD=((
struct Cyc_Absyn_Enum_p_struct*)_tmp4C0)->f2;_LL30F: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Enum_e_struct*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));
_tmp4E2[0]=({struct Cyc_Absyn_Enum_e_struct _tmp4E3;_tmp4E3.tag=33;_tmp4E3.f1=
_tmp4CD->name;_tmp4E3.f2=(struct Cyc_Absyn_Enumdecl*)_tmp4CC;_tmp4E3.f3=(struct
Cyc_Absyn_Enumfield*)_tmp4CD;_tmp4E3;});_tmp4E2;}),0),fail_stmt);goto _LL2FD;
_LL310: if(_tmp4C0 <= (void*)2  || *((int*)_tmp4C0)!= 11)goto _LL312;_tmp4CE=(void*)((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp4C0)->f1;_tmp4CF=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp4C0)->f2;_LL311: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*
_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4[0]=({struct Cyc_Absyn_AnonEnum_e_struct
_tmp4E5;_tmp4E5.tag=34;_tmp4E5.f1=_tmp4CF->name;_tmp4E5.f2=(void*)_tmp4CE;
_tmp4E5.f3=(struct Cyc_Absyn_Enumfield*)_tmp4CF;_tmp4E5;});_tmp4E4;}),0),
fail_stmt);goto _LL2FD;_LL312: if(_tmp4C0 <= (void*)2  || *((int*)_tmp4C0)!= 6)goto
_LL314;_tmp4D0=((struct Cyc_Absyn_Tunion_p_struct*)_tmp4C0)->f1;_tmp4D1=((struct
Cyc_Absyn_Tunion_p_struct*)_tmp4C0)->f2;_tmp4D2=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp4C0)->f3;if(_tmp4D2 != 0)goto _LL314;_LL313: {struct Cyc_Absyn_Exp*cmp_exp;if(
_tmp4D0->is_xtunion)cmp_exp=Cyc_Absyn_var_exp(_tmp4D1->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(
_tmp4D0,_tmp4D1->name,0);r=Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(
r,cmp_exp,fail_stmt);goto _LL2FD;}_LL314: if(_tmp4C0 <= (void*)2  || *((int*)_tmp4C0)
!= 4)goto _LL316;_tmp4D3=((struct Cyc_Absyn_Pointer_p_struct*)_tmp4C0)->f1;_tmp4D4=*
_tmp4D3;_tmp4D5=(void*)_tmp4D4.r;if(_tmp4D5 <= (void*)2  || *((int*)_tmp4D5)!= 6)
goto _LL316;_tmp4D6=((struct Cyc_Absyn_Tunion_p_struct*)_tmp4D5)->f1;_tmp4D7=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp4D5)->f2;_tmp4D8=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp4D5)->f3;if(!(_tmp4D8 != 0))goto _LL316;_LL315: s=Cyc_Toc_skip_stmt_dl();{int
cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp4D7->name,({const char*
_tmp4F2="_struct";_tag_arr(_tmp4F2,sizeof(char),_get_zero_arr_size(_tmp4F2,8));}));
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(
tufstrct),Cyc_Toc_mt_tq),r);struct Cyc_List_List*_tmp4E6=_tmp4D7->typs;for(0;
_tmp4D8 != 0;(((_tmp4D8=_tmp4D8->tl,_tmp4E6=((struct Cyc_List_List*)_check_null(
_tmp4E6))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp4E7=(struct Cyc_Absyn_Pat*)
_tmp4D8->hd;if((void*)_tmp4E7->r == (void*)0)continue;{void*_tmp4E8=(*((struct
_tuple4*)((struct Cyc_List_List*)_check_null(_tmp4E6))->hd)).f2;struct _tuple1*
_tmp4E9=Cyc_Toc_temp_var();void*_tmp4EA=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4E7->topt))->v;void*_tmp4EB=Cyc_Toc_typ_to_c(_tmp4EA);struct Cyc_Absyn_Exp*
_tmp4EC=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp4E8)))_tmp4EC=Cyc_Toc_cast_it(_tmp4EB,_tmp4EC);decls=({
struct Cyc_List_List*_tmp4ED=_region_malloc(rgn,sizeof(*_tmp4ED));_tmp4ED->hd=({
struct _tuple13*_tmp4EE=_region_malloc(rgn,sizeof(*_tmp4EE));_tmp4EE->f1=_tmp4E9;
_tmp4EE->f2=_tmp4EB;_tmp4EE;});_tmp4ED->tl=decls;_tmp4ED;});{struct _tuple12
_tmp4EF=Cyc_Toc_xlate_pat(nv,rgn,_tmp4EA,Cyc_Absyn_var_exp(_tmp4E9,0),_tmp4EC,
_tmp4E7,fail_stmt,decls);nv=_tmp4EF.f1;decls=_tmp4EF.f2;{struct Cyc_Absyn_Stmt*
_tmp4F0=_tmp4EF.f3;struct Cyc_Absyn_Stmt*_tmp4F1=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp4E9,0),_tmp4EC,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp4F1,_tmp4F0,
0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(_tmp4D6->is_xtunion){struct Cyc_Absyn_Exp*
e2=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),
r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(
_tmp4D7->name,0);test_exp=Cyc_Absyn_neq_exp(e1,e,0);}else{int max_tag=Cyc_Toc_num_void_tags(
_tmp4D6);struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)
max_tag,0);struct Cyc_Absyn_Exp*e3=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,
Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e5=Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),max_tag_exp),0);
struct Cyc_Absyn_Exp*e=Cyc_Toc_tunion_tag(_tmp4D6,_tmp4D7->name,1);test_exp=
max_tag > 0?Cyc_Absyn_or_exp(e5,Cyc_Absyn_neq_exp(e1,e,0),0): Cyc_Absyn_neq_exp(e1,
e,0);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);goto _LL2FD;}}_LL316: if(_tmp4C0 <= (void*)2  || *((int*)_tmp4C0)!= 6)goto
_LL318;_tmp4D9=((struct Cyc_Absyn_Tunion_p_struct*)_tmp4C0)->f3;_LL317: _tmp4DA=
_tmp4D9;goto _LL319;_LL318: if(_tmp4C0 <= (void*)2  || *((int*)_tmp4C0)!= 3)goto
_LL31A;_tmp4DA=((struct Cyc_Absyn_Tuple_p_struct*)_tmp4C0)->f1;_LL319: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;for(0;_tmp4DA != 0;(_tmp4DA=_tmp4DA->tl,++ cnt)){struct Cyc_Absyn_Pat*
_tmp4F3=(struct Cyc_Absyn_Pat*)_tmp4DA->hd;if((void*)_tmp4F3->r == (void*)0)
continue;{struct _tuple1*_tmp4F4=Cyc_Toc_temp_var();void*_tmp4F5=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4F3->topt))->v;decls=({struct Cyc_List_List*_tmp4F6=
_region_malloc(rgn,sizeof(*_tmp4F6));_tmp4F6->hd=({struct _tuple13*_tmp4F7=
_region_malloc(rgn,sizeof(*_tmp4F7));_tmp4F7->f1=_tmp4F4;_tmp4F7->f2=Cyc_Toc_typ_to_c(
_tmp4F5);_tmp4F7;});_tmp4F6->tl=decls;_tmp4F6;});{struct _tuple12 _tmp4F8=Cyc_Toc_xlate_pat(
nv,rgn,_tmp4F5,Cyc_Absyn_var_exp(_tmp4F4,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp4F3,fail_stmt,decls);nv=_tmp4F8.f1;decls=_tmp4F8.f2;{struct Cyc_Absyn_Stmt*
_tmp4F9=_tmp4F8.f3;struct Cyc_Absyn_Stmt*_tmp4FA=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp4F4,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp4FA,_tmp4F9,0),0);}}}}goto _LL2FD;}_LL31A: if(_tmp4C0 <= (
void*)2  || *((int*)_tmp4C0)!= 5)goto _LL31C;_tmp4DB=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp4C0)->f1;_tmp4DC=(void*)_tmp4DB.aggr_info;_tmp4DD=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp4C0)->f3;_LL31B: {struct Cyc_Absyn_Aggrdecl*_tmp4FB=Cyc_Absyn_get_known_aggrdecl(
_tmp4DC);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp4DD != 0;_tmp4DD=_tmp4DD->tl){struct
_tuple14*_tmp4FC=(struct _tuple14*)_tmp4DD->hd;struct Cyc_Absyn_Pat*_tmp4FD=(*
_tmp4FC).f2;if((void*)_tmp4FD->r == (void*)0)continue;{struct _tagged_arr*f;{void*
_tmp4FE=(void*)((struct Cyc_List_List*)_check_null((*_tmp4FC).f1))->hd;struct
_tagged_arr*_tmp4FF;_LL325: if(*((int*)_tmp4FE)!= 1)goto _LL327;_tmp4FF=((struct
Cyc_Absyn_FieldName_struct*)_tmp4FE)->f1;_LL326: f=_tmp4FF;goto _LL324;_LL327:;
_LL328:(int)_throw((void*)Cyc_Toc_Match_error);_LL324:;}{struct _tuple1*_tmp500=
Cyc_Toc_temp_var();void*_tmp501=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4FD->topt))->v;
void*_tmp502=Cyc_Toc_typ_to_c(_tmp501);decls=({struct Cyc_List_List*_tmp503=
_region_malloc(rgn,sizeof(*_tmp503));_tmp503->hd=({struct _tuple13*_tmp504=
_region_malloc(rgn,sizeof(*_tmp504));_tmp504->f1=_tmp500;_tmp504->f2=_tmp502;
_tmp504;});_tmp503->tl=decls;_tmp503;});{struct _tuple12 _tmp505=Cyc_Toc_xlate_pat(
nv,rgn,_tmp501,Cyc_Absyn_var_exp(_tmp500,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp4FD,fail_stmt,decls);nv=_tmp505.f1;decls=_tmp505.f2;{struct Cyc_Absyn_Exp*
_tmp506=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((void*)((struct
Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4FB->impl))->fields,f)))->type))_tmp506=Cyc_Toc_cast_it(_tmp502,
_tmp506);{struct Cyc_Absyn_Stmt*_tmp507=_tmp505.f3;struct Cyc_Absyn_Stmt*_tmp508=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp500,0),_tmp506,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp508,_tmp507,0),0);}}}}}}goto _LL2FD;}_LL31C: if(_tmp4C0 <= (
void*)2  || *((int*)_tmp4C0)!= 4)goto _LL31E;_tmp4DE=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp4C0)->f1;_LL31D: {struct _tuple1*_tmp509=Cyc_Toc_temp_var();void*_tmp50A=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4DE->topt))->v;decls=({struct Cyc_List_List*
_tmp50B=_region_malloc(rgn,sizeof(*_tmp50B));_tmp50B->hd=({struct _tuple13*
_tmp50C=_region_malloc(rgn,sizeof(*_tmp50C));_tmp50C->f1=_tmp509;_tmp50C->f2=Cyc_Toc_typ_to_c(
_tmp50A);_tmp50C;});_tmp50B->tl=decls;_tmp50B;});{struct _tuple12 _tmp50D=Cyc_Toc_xlate_pat(
nv,rgn,_tmp50A,Cyc_Absyn_var_exp(_tmp509,0),Cyc_Absyn_deref_exp(path,0),_tmp4DE,
fail_stmt,decls);nv=_tmp50D.f1;decls=_tmp50D.f2;{struct Cyc_Absyn_Stmt*_tmp50E=
_tmp50D.f3;struct Cyc_Absyn_Stmt*_tmp50F=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp509,0),Cyc_Absyn_deref_exp(r,0),0),_tmp50E,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp50F,0);else{s=_tmp50F;}goto
_LL2FD;}}}_LL31E: if(_tmp4C0 <= (void*)2  || *((int*)_tmp4C0)!= 12)goto _LL320;
_LL31F:({void*_tmp510[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp511="unknownid pat";_tag_arr(_tmp511,sizeof(
char),_get_zero_arr_size(_tmp511,14));}),_tag_arr(_tmp510,sizeof(void*),0));});
_LL320: if(_tmp4C0 <= (void*)2  || *((int*)_tmp4C0)!= 13)goto _LL322;_LL321:({void*
_tmp512[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp513="unknowncall pat";_tag_arr(_tmp513,sizeof(char),
_get_zero_arr_size(_tmp513,16));}),_tag_arr(_tmp512,sizeof(void*),0));});_LL322:
if(_tmp4C0 <= (void*)2  || *((int*)_tmp4C0)!= 14)goto _LL2FD;_LL323:({void*_tmp514[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp515="exp pat";_tag_arr(_tmp515,sizeof(char),_get_zero_arr_size(
_tmp515,8));}),_tag_arr(_tmp514,sizeof(void*),0));});_LL2FD:;}return({struct
_tuple12 _tmp516;_tmp516.f1=nv;_tmp516.f2=decls;_tmp516.f3=s;_tmp516;});}struct
_tuple15{struct _tagged_arr*f1;struct _tagged_arr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple15*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple15*_tmp517=_region_malloc(r,sizeof(*_tmp517));_tmp517->f1=
Cyc_Toc_fresh_label();_tmp517->f2=Cyc_Toc_fresh_label();_tmp517->f3=sc;_tmp517;});}
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*
_tmp518=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int leave_as_switch;{
void*_tmp519=Cyc_Tcutil_compress(_tmp518);_LL32A: if(_tmp519 <= (void*)4  || *((int*)
_tmp519)!= 5)goto _LL32C;_LL32B: goto _LL32D;_LL32C: if(_tmp519 <= (void*)4  || *((int*)
_tmp519)!= 12)goto _LL32E;_LL32D: leave_as_switch=1;goto _LL329;_LL32E:;_LL32F:
leave_as_switch=0;goto _LL329;_LL329:;}{struct Cyc_List_List*_tmp51A=scs;for(0;
_tmp51A != 0;_tmp51A=_tmp51A->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp51A->hd)->pat_vars))->v != 0  || ((
struct Cyc_Absyn_Switch_clause*)_tmp51A->hd)->where_clause != 0){leave_as_switch=0;
break;}}}if(leave_as_switch){struct _tagged_arr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp51B=scs;for(0;_tmp51B != 0;_tmp51B=_tmp51B->tl){struct Cyc_Absyn_Stmt*
_tmp51C=((struct Cyc_Absyn_Switch_clause*)_tmp51B->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp51B->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp51C,0);next_l=Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(nv,next_l),_tmp51C);}}return;}{
struct _tuple1*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct _tagged_arr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp51D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp51D;
_push_region(rgn);{struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple15*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,
rgn,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*
decls=0;{struct Cyc_List_List*_tmp51E=lscs;for(0;_tmp51E != 0;_tmp51E=_tmp51E->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple15*)_tmp51E->hd)).f3;struct
_tagged_arr*fail_lab=_tmp51E->tl == 0?end_l:(*((struct _tuple15*)((struct Cyc_List_List*)
_check_null(_tmp51E->tl))->hd)).f1;struct Cyc_Toc_Env*_tmp520;struct Cyc_List_List*
_tmp521;struct Cyc_Absyn_Stmt*_tmp522;struct _tuple12 _tmp51F=Cyc_Toc_xlate_pat(nv,
rgn,_tmp518,r,path,sc->pattern,Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp520=
_tmp51F.f1;_tmp521=_tmp51F.f2;_tmp522=_tmp51F.f3;if(sc->where_clause != 0){struct
Cyc_Absyn_Exp*_tmp523=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(
_tmp520,_tmp523);_tmp522=Cyc_Absyn_seq_stmt(_tmp522,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp523,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp521;nvs=({struct Cyc_List_List*_tmp524=_region_malloc(rgn,
sizeof(*_tmp524));_tmp524->hd=_tmp520;_tmp524->tl=nvs;_tmp524;});test_stmts=({
struct Cyc_List_List*_tmp525=_region_malloc(rgn,sizeof(*_tmp525));_tmp525->hd=
_tmp522;_tmp525->tl=test_stmts;_tmp525;});}}nvs=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple15 _tmp527;struct _tagged_arr*_tmp528;struct _tagged_arr*_tmp529;struct
Cyc_Absyn_Switch_clause*_tmp52A;struct _tuple15*_tmp526=(struct _tuple15*)lscs->hd;
_tmp527=*_tmp526;_tmp528=_tmp527.f1;_tmp529=_tmp527.f2;_tmp52A=_tmp527.f3;{
struct Cyc_Toc_Env*_tmp52B=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp52A->body;if(lscs->tl != 0){struct _tuple15
_tmp52D;struct _tagged_arr*_tmp52E;struct Cyc_Absyn_Switch_clause*_tmp52F;struct
_tuple15*_tmp52C=(struct _tuple15*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;
_tmp52D=*_tmp52C;_tmp52E=_tmp52D.f2;_tmp52F=_tmp52D.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
_tmp52B,end_l,_tmp52E,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp52F->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp52B,
end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp528,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp529,s,0),0);stmts=({struct Cyc_List_List*_tmp530=_region_malloc(rgn,sizeof(*
_tmp530));_tmp530->hd=s;_tmp530->tl=stmts;_tmp530;});}}{struct Cyc_Absyn_Stmt*res=
Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),
0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple13 _tmp532;struct _tuple1*
_tmp533;void*_tmp534;struct _tuple13*_tmp531=(struct _tuple13*)decls->hd;_tmp532=*
_tmp531;_tmp533=_tmp532.f1;_tmp534=_tmp532.f2;res=Cyc_Absyn_declare_stmt(_tmp533,
_tmp534,0,res,0);}(void*)(whole_s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(v,
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct
Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(rgn);}}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp535=
_cycalloc(sizeof(*_tmp535));_tmp535->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),
0);_tmp535->tl=0;_tmp535;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp536=(void*)s->r;
struct Cyc_Absyn_Exp*_tmp537;struct Cyc_Absyn_Stmt*_tmp538;struct Cyc_Absyn_Stmt*
_tmp539;struct Cyc_Absyn_Exp*_tmp53A;struct Cyc_Absyn_Exp*_tmp53B;struct Cyc_Absyn_Stmt*
_tmp53C;struct Cyc_Absyn_Stmt*_tmp53D;struct _tuple3 _tmp53E;struct Cyc_Absyn_Exp*
_tmp53F;struct Cyc_Absyn_Stmt*_tmp540;struct Cyc_Absyn_Stmt*_tmp541;struct Cyc_Absyn_Stmt*
_tmp542;struct Cyc_Absyn_Stmt*_tmp543;struct Cyc_Absyn_Exp*_tmp544;struct _tuple3
_tmp545;struct Cyc_Absyn_Exp*_tmp546;struct _tuple3 _tmp547;struct Cyc_Absyn_Exp*
_tmp548;struct Cyc_Absyn_Stmt*_tmp549;struct Cyc_Absyn_Exp*_tmp54A;struct Cyc_List_List*
_tmp54B;struct Cyc_List_List*_tmp54C;struct Cyc_Absyn_Switch_clause**_tmp54D;
struct Cyc_Absyn_Decl*_tmp54E;struct Cyc_Absyn_Stmt*_tmp54F;struct _tagged_arr*
_tmp550;struct Cyc_Absyn_Stmt*_tmp551;struct Cyc_Absyn_Stmt*_tmp552;struct _tuple3
_tmp553;struct Cyc_Absyn_Exp*_tmp554;struct Cyc_Absyn_Stmt*_tmp555;struct Cyc_List_List*
_tmp556;struct Cyc_Absyn_Tvar*_tmp557;struct Cyc_Absyn_Vardecl*_tmp558;int _tmp559;
struct Cyc_Absyn_Stmt*_tmp55A;struct Cyc_Absyn_Exp*_tmp55B;_LL331: if((int)_tmp536
!= 0)goto _LL333;_LL332: return;_LL333: if(_tmp536 <= (void*)1  || *((int*)_tmp536)!= 
0)goto _LL335;_tmp537=((struct Cyc_Absyn_Exp_s_struct*)_tmp536)->f1;_LL334: Cyc_Toc_exp_to_c(
nv,_tmp537);return;_LL335: if(_tmp536 <= (void*)1  || *((int*)_tmp536)!= 1)goto
_LL337;_tmp538=((struct Cyc_Absyn_Seq_s_struct*)_tmp536)->f1;_tmp539=((struct Cyc_Absyn_Seq_s_struct*)
_tmp536)->f2;_LL336: Cyc_Toc_stmt_to_c(nv,_tmp538);s=_tmp539;continue;_LL337: if(
_tmp536 <= (void*)1  || *((int*)_tmp536)!= 2)goto _LL339;_tmp53A=((struct Cyc_Absyn_Return_s_struct*)
_tmp536)->f1;_LL338: {struct Cyc_Core_Opt*topt=0;if(_tmp53A != 0){topt=({struct Cyc_Core_Opt*
_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp53A->topt))->v);_tmp55C;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp53A);}if(s->try_depth > 0){if(topt != 0){struct
_tuple1*_tmp55D=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp55E=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp55D,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp55D,(void*)topt->v,_tmp53A,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp55E,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL339: if(_tmp536 <= (void*)1  || *((int*)_tmp536)!= 3)goto _LL33B;_tmp53B=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp536)->f1;_tmp53C=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp536)->f2;_tmp53D=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp536)->f3;_LL33A:
Cyc_Toc_exp_to_c(nv,_tmp53B);Cyc_Toc_stmt_to_c(nv,_tmp53C);s=_tmp53D;continue;
_LL33B: if(_tmp536 <= (void*)1  || *((int*)_tmp536)!= 4)goto _LL33D;_tmp53E=((struct
Cyc_Absyn_While_s_struct*)_tmp536)->f1;_tmp53F=_tmp53E.f1;_tmp540=((struct Cyc_Absyn_While_s_struct*)
_tmp536)->f2;_LL33C: Cyc_Toc_exp_to_c(nv,_tmp53F);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
nv),_tmp540);return;_LL33D: if(_tmp536 <= (void*)1  || *((int*)_tmp536)!= 5)goto
_LL33F;_tmp541=((struct Cyc_Absyn_Break_s_struct*)_tmp536)->f1;_LL33E: if(nv->break_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->break_lab))->v,0))->r));{int dest_depth=
_tmp541 == 0?0: _tmp541->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,
s);return;}_LL33F: if(_tmp536 <= (void*)1  || *((int*)_tmp536)!= 6)goto _LL341;
_tmp542=((struct Cyc_Absyn_Continue_s_struct*)_tmp536)->f1;_LL340: if(nv->continue_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->continue_lab))->v,0))->r));_tmp543=_tmp542;
goto _LL342;_LL341: if(_tmp536 <= (void*)1  || *((int*)_tmp536)!= 7)goto _LL343;
_tmp543=((struct Cyc_Absyn_Goto_s_struct*)_tmp536)->f2;_LL342: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp543))->try_depth,s);
return;_LL343: if(_tmp536 <= (void*)1  || *((int*)_tmp536)!= 8)goto _LL345;_tmp544=((
struct Cyc_Absyn_For_s_struct*)_tmp536)->f1;_tmp545=((struct Cyc_Absyn_For_s_struct*)
_tmp536)->f2;_tmp546=_tmp545.f1;_tmp547=((struct Cyc_Absyn_For_s_struct*)_tmp536)->f3;
_tmp548=_tmp547.f1;_tmp549=((struct Cyc_Absyn_For_s_struct*)_tmp536)->f4;_LL344:
Cyc_Toc_exp_to_c(nv,_tmp544);Cyc_Toc_exp_to_c(nv,_tmp546);Cyc_Toc_exp_to_c(nv,
_tmp548);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp549);return;_LL345: if(
_tmp536 <= (void*)1  || *((int*)_tmp536)!= 9)goto _LL347;_tmp54A=((struct Cyc_Absyn_Switch_s_struct*)
_tmp536)->f1;_tmp54B=((struct Cyc_Absyn_Switch_s_struct*)_tmp536)->f2;_LL346: Cyc_Toc_xlate_switch(
nv,s,_tmp54A,_tmp54B);return;_LL347: if(_tmp536 <= (void*)1  || *((int*)_tmp536)!= 
10)goto _LL349;_tmp54C=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp536)->f1;_tmp54D=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp536)->f2;_LL348: if(nv->fallthru_info == 0)({
void*_tmp55F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp560="fallthru in unexpected place";_tag_arr(_tmp560,sizeof(char),
_get_zero_arr_size(_tmp560,29));}),_tag_arr(_tmp55F,sizeof(void*),0));});{struct
_tuple8 _tmp562;struct _tagged_arr*_tmp563;struct Cyc_List_List*_tmp564;struct Cyc_Dict_Dict*
_tmp565;struct _tuple8*_tmp561=(struct _tuple8*)((struct Cyc_Core_Opt*)_check_null(
nv->fallthru_info))->v;_tmp562=*_tmp561;_tmp563=_tmp562.f1;_tmp564=_tmp562.f2;
_tmp565=_tmp562.f3;{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp563,0);Cyc_Toc_do_npop_before(
s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp54D)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp566=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp564);struct Cyc_List_List*_tmp567=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp54C);for(0;_tmp566 != 0;(_tmp566=_tmp566->tl,
_tmp567=_tmp567->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp567))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup)(
_tmp565,(struct _tuple1*)_tmp566->hd),(struct Cyc_Absyn_Exp*)_tmp567->hd,0),s2,0);}(
void*)(s->r=(void*)((void*)s2->r));return;}}}_LL349: if(_tmp536 <= (void*)1  || *((
int*)_tmp536)!= 11)goto _LL34B;_tmp54E=((struct Cyc_Absyn_Decl_s_struct*)_tmp536)->f1;
_tmp54F=((struct Cyc_Absyn_Decl_s_struct*)_tmp536)->f2;_LL34A:{void*_tmp568=(void*)
_tmp54E->r;struct Cyc_Absyn_Vardecl*_tmp569;struct Cyc_Absyn_Pat*_tmp56A;struct Cyc_Absyn_Exp*
_tmp56B;struct Cyc_List_List*_tmp56C;_LL356: if(_tmp568 <= (void*)2  || *((int*)
_tmp568)!= 0)goto _LL358;_tmp569=((struct Cyc_Absyn_Var_d_struct*)_tmp568)->f1;
_LL357: {struct Cyc_Toc_Env*_tmp56D=Cyc_Toc_add_varmap(nv,_tmp569->name,Cyc_Absyn_varb_exp(
_tmp569->name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp56E=_cycalloc(sizeof(*
_tmp56E));_tmp56E[0]=({struct Cyc_Absyn_Local_b_struct _tmp56F;_tmp56F.tag=3;
_tmp56F.f1=_tmp569;_tmp56F;});_tmp56E;}),0));Cyc_Toc_local_decl_to_c(_tmp56D,
_tmp56D,_tmp569,_tmp54F);goto _LL355;}_LL358: if(_tmp568 <= (void*)2  || *((int*)
_tmp568)!= 2)goto _LL35A;_tmp56A=((struct Cyc_Absyn_Let_d_struct*)_tmp568)->f1;
_tmp56B=((struct Cyc_Absyn_Let_d_struct*)_tmp568)->f3;_LL359:{void*_tmp570=(void*)
_tmp56A->r;struct Cyc_Absyn_Vardecl*_tmp571;_LL35F: if(_tmp570 <= (void*)2  || *((
int*)_tmp570)!= 0)goto _LL361;_tmp571=((struct Cyc_Absyn_Var_p_struct*)_tmp570)->f1;
_LL360: {struct _tuple1*old_name=_tmp571->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp571->name=new_name;_tmp571->initializer=(struct Cyc_Absyn_Exp*)_tmp56B;(void*)(
_tmp54E->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp572=_cycalloc(
sizeof(*_tmp572));_tmp572[0]=({struct Cyc_Absyn_Var_d_struct _tmp573;_tmp573.tag=0;
_tmp573.f1=_tmp571;_tmp573;});_tmp572;})));{struct Cyc_Toc_Env*_tmp574=Cyc_Toc_add_varmap(
nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575[0]=({struct Cyc_Absyn_Local_b_struct
_tmp576;_tmp576.tag=3;_tmp576.f1=_tmp571;_tmp576;});_tmp575;}),0));Cyc_Toc_local_decl_to_c(
_tmp574,nv,_tmp571,_tmp54F);goto _LL35E;}}_LL361:;_LL362:(void*)(s->r=(void*)((
void*)(Cyc_Toc_letdecl_to_c(nv,_tmp56A,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp56B->topt))->v,_tmp56B,_tmp54F))->r));goto _LL35E;_LL35E:;}goto _LL355;_LL35A:
if(_tmp568 <= (void*)2  || *((int*)_tmp568)!= 3)goto _LL35C;_tmp56C=((struct Cyc_Absyn_Letv_d_struct*)
_tmp568)->f1;_LL35B: {struct Cyc_List_List*_tmp577=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp56C);if(_tmp577 == 0)({void*_tmp578[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp579="empty Letv_d";_tag_arr(_tmp579,sizeof(char),_get_zero_arr_size(_tmp579,
13));}),_tag_arr(_tmp578,sizeof(void*),0));});(void*)(_tmp54E->r=(void*)((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A[0]=({
struct Cyc_Absyn_Var_d_struct _tmp57B;_tmp57B.tag=0;_tmp57B.f1=(struct Cyc_Absyn_Vardecl*)
_tmp577->hd;_tmp57B;});_tmp57A;})));_tmp577=_tmp577->tl;for(0;_tmp577 != 0;
_tmp577=_tmp577->tl){struct Cyc_Absyn_Decl*_tmp57C=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D[0]=({
struct Cyc_Absyn_Var_d_struct _tmp57E;_tmp57E.tag=0;_tmp57E.f1=(struct Cyc_Absyn_Vardecl*)
_tmp577->hd;_tmp57E;});_tmp57D;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp57C,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto
_LL355;}_LL35C:;_LL35D:({void*_tmp57F[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp580="bad nested declaration within function";
_tag_arr(_tmp580,sizeof(char),_get_zero_arr_size(_tmp580,39));}),_tag_arr(
_tmp57F,sizeof(void*),0));});_LL355:;}return;_LL34B: if(_tmp536 <= (void*)1  || *((
int*)_tmp536)!= 12)goto _LL34D;_tmp550=((struct Cyc_Absyn_Label_s_struct*)_tmp536)->f1;
_tmp551=((struct Cyc_Absyn_Label_s_struct*)_tmp536)->f2;_LL34C: s=_tmp551;
continue;_LL34D: if(_tmp536 <= (void*)1  || *((int*)_tmp536)!= 13)goto _LL34F;
_tmp552=((struct Cyc_Absyn_Do_s_struct*)_tmp536)->f1;_tmp553=((struct Cyc_Absyn_Do_s_struct*)
_tmp536)->f2;_tmp554=_tmp553.f1;_LL34E: Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),
_tmp552);Cyc_Toc_exp_to_c(nv,_tmp554);return;_LL34F: if(_tmp536 <= (void*)1  || *((
int*)_tmp536)!= 14)goto _LL351;_tmp555=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp536)->f1;_tmp556=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp536)->f2;_LL350: {
struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=({struct Cyc_Core_Opt*
_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581->v=(void*)e_typ;_tmp581;});nv=Cyc_Toc_add_varmap(
nv,e_var,e_exp);Cyc_Toc_stmt_to_c(nv,_tmp555);{struct Cyc_Absyn_Stmt*_tmp582=Cyc_Absyn_seq_stmt(
_tmp555,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
struct _tuple1*_tmp583=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp584=Cyc_Absyn_var_exp(
_tmp583,0);struct Cyc_Absyn_Vardecl*_tmp585=Cyc_Absyn_new_vardecl(_tmp583,Cyc_Absyn_exn_typ,
0);_tmp584->topt=({struct Cyc_Core_Opt*_tmp586=_cycalloc(sizeof(*_tmp586));
_tmp586->v=(void*)Cyc_Absyn_exn_typ;_tmp586;});{struct Cyc_Absyn_Pat*_tmp587=({
struct Cyc_Absyn_Pat*_tmp597=_cycalloc(sizeof(*_tmp597));_tmp597->r=(void*)((void*)({
struct Cyc_Absyn_Var_p_struct*_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599[0]=({
struct Cyc_Absyn_Var_p_struct _tmp59A;_tmp59A.tag=0;_tmp59A.f1=_tmp585;_tmp59A;});
_tmp599;}));_tmp597->topt=({struct Cyc_Core_Opt*_tmp598=_cycalloc(sizeof(*_tmp598));
_tmp598->v=(void*)Cyc_Absyn_exn_typ;_tmp598;});_tmp597->loc=0;_tmp597;});struct
Cyc_Absyn_Exp*_tmp588=Cyc_Absyn_throw_exp(_tmp584,0);_tmp588->topt=({struct Cyc_Core_Opt*
_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589->v=(void*)((void*)0);_tmp589;});{
struct Cyc_Absyn_Stmt*_tmp58A=Cyc_Absyn_exp_stmt(_tmp588,0);struct Cyc_Absyn_Switch_clause*
_tmp58B=({struct Cyc_Absyn_Switch_clause*_tmp594=_cycalloc(sizeof(*_tmp594));
_tmp594->pattern=_tmp587;_tmp594->pat_vars=({struct Cyc_Core_Opt*_tmp595=
_cycalloc(sizeof(*_tmp595));_tmp595->v=({struct Cyc_List_List*_tmp596=_cycalloc(
sizeof(*_tmp596));_tmp596->hd=_tmp585;_tmp596->tl=0;_tmp596;});_tmp595;});
_tmp594->where_clause=0;_tmp594->body=_tmp58A;_tmp594->loc=0;_tmp594;});struct
Cyc_Absyn_Stmt*_tmp58C=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp556,({struct
Cyc_List_List*_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593->hd=_tmp58B;_tmp593->tl=
0;_tmp593;})),0);Cyc_Toc_stmt_to_c(nv,_tmp58C);{struct Cyc_Absyn_Exp*_tmp58D=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp592=_cycalloc(sizeof(*_tmp592));
_tmp592->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp592->tl=0;
_tmp592;}),0);struct Cyc_Absyn_Stmt*_tmp58E=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp591=_cycalloc(sizeof(*_tmp591));
_tmp591->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp591->tl=0;_tmp591;}),0),0);struct
Cyc_Absyn_Exp*_tmp58F=Cyc_Absyn_int_exp((void*)0,0,0);struct Cyc_Absyn_Exp*
_tmp590=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp58E,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp58F,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp58D,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp590,0),Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),
_tmp582,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Toc_cast_it(
e_typ,Cyc_Toc__exn_thrown_e),_tmp58C,0),0),0),0),0),0))->r));return;}}}}}_LL351:
if(_tmp536 <= (void*)1  || *((int*)_tmp536)!= 15)goto _LL353;_tmp557=((struct Cyc_Absyn_Region_s_struct*)
_tmp536)->f1;_tmp558=((struct Cyc_Absyn_Region_s_struct*)_tmp536)->f2;_tmp559=((
struct Cyc_Absyn_Region_s_struct*)_tmp536)->f3;_tmp55A=((struct Cyc_Absyn_Region_s_struct*)
_tmp536)->f4;_LL352: {void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct
_tuple1*rh_var=Cyc_Toc_temp_var();struct _tuple1*x_var=_tmp558->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(nv,x_var,x_exp),_tmp55A);(void*)(s->r=(
void*)((void*)(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp59B=
_cycalloc(sizeof(*_tmp59B));_tmp59B->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0);_tmp59B->tl=0;_tmp59B;}),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*
_tmp59C[1];_tmp59C[0]=x_exp;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp59C,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),Cyc_Absyn_seq_stmt(
_tmp55A,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct
Cyc_Absyn_Exp*_tmp59D[1];_tmp59D[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp59D,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),0),0),0),0))->r));return;}_LL353: if(_tmp536 <= (void*)1  || *((int*)_tmp536)
!= 16)goto _LL330;_tmp55B=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp536)->f1;
_LL354: Cyc_Toc_exp_to_c(nv,_tmp55B);(void*)(s->r=(void*)((void*)(Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp59E=
_cycalloc(sizeof(*_tmp59E));_tmp59E->hd=_tmp55B;_tmp59E->tl=0;_tmp59E;}),0),0))->r));
return;_LL330:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);struct
_tuple16{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){f->tvs=0;f->effect=0;
f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((void*)f->ret_type));{
struct Cyc_List_List*_tmp59F=f->args;for(0;_tmp59F != 0;_tmp59F=_tmp59F->tl){(*((
struct _tuple16*)_tmp59F->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple16*)_tmp59F->hd)).f3);{
struct _tuple1*_tmp5A0=({struct _tuple1*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));
_tmp5A1->f1=(void*)((void*)0);_tmp5A1->f2=(*((struct _tuple16*)_tmp59F->hd)).f1;
_tmp5A1;});nv=Cyc_Toc_add_varmap(nv,_tmp5A0,Cyc_Absyn_var_exp(_tmp5A0,0));}}}if(
cinclude){Cyc_Toc_stmttypes_to_c(f->body);return;}if((unsigned int)f->cyc_varargs
 && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){struct
Cyc_Core_Opt*_tmp5A3;struct Cyc_Absyn_Tqual _tmp5A4;void*_tmp5A5;int _tmp5A6;struct
Cyc_Absyn_VarargInfo _tmp5A2=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));
_tmp5A3=_tmp5A2.name;_tmp5A4=_tmp5A2.tq;_tmp5A5=(void*)_tmp5A2.type;_tmp5A6=
_tmp5A2.inject;{void*_tmp5A7=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ(_tmp5A5,(void*)
2,_tmp5A4,Cyc_Absyn_false_conref));struct _tuple1*_tmp5A8=({struct _tuple1*_tmp5AB=
_cycalloc(sizeof(*_tmp5AB));_tmp5AB->f1=(void*)((void*)0);_tmp5AB->f2=(struct
_tagged_arr*)((struct Cyc_Core_Opt*)_check_null(_tmp5A3))->v;_tmp5AB;});f->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
f->args,({struct Cyc_List_List*_tmp5A9=_cycalloc(sizeof(*_tmp5A9));_tmp5A9->hd=({
struct _tuple16*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));_tmp5AA->f1=(struct
_tagged_arr*)_tmp5A3->v;_tmp5AA->f2=_tmp5A4;_tmp5AA->f3=_tmp5A7;_tmp5AA;});
_tmp5A9->tl=0;_tmp5A9;}));nv=Cyc_Toc_add_varmap(nv,_tmp5A8,Cyc_Absyn_var_exp(
_tmp5A8,0));f->cyc_varargs=0;}}{struct Cyc_List_List*_tmp5AC=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp5AC != 0;_tmp5AC=
_tmp5AC->tl){(void*)(((struct Cyc_Absyn_Vardecl*)_tmp5AC->hd)->type=(void*)Cyc_Toc_typ_to_c((
void*)((struct Cyc_Absyn_Vardecl*)_tmp5AC->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(
nv),f->body);}static void*Cyc_Toc_scope_to_c(void*s){void*_tmp5AD=s;_LL364: if((
int)_tmp5AD != 1)goto _LL366;_LL365: return(void*)2;_LL366: if((int)_tmp5AD != 4)goto
_LL368;_LL367: return(void*)3;_LL368:;_LL369: return s;_LL363:;}struct Cyc_Core_Opt*
Cyc_Toc_aggrs_so_far=0;static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad){struct _tuple1*_tmp5AE=ad->name;if(Cyc_Toc_aggrs_so_far == 0)Cyc_Toc_aggrs_so_far=({
struct Cyc_Core_Opt*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp5AF;});{int seen_defn_before;struct Cyc_Core_Opt*_tmp5B0=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp5AE);if(_tmp5B0 == 
0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(
Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp5AE,ad));}else{if(((
struct Cyc_Absyn_Aggrdecl*)_tmp5B0->v)->impl == 0){(struct Cyc_Dict_Dict*)(((struct
Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,
_tmp5AE,ad));seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(ad->sc=(void*)((
void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po=
0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*_tmp5B1=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp5B1 != 0;_tmp5B1=_tmp5B1->tl){(void*)(((
struct Cyc_Absyn_Aggrfield*)_tmp5B1->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp5B1->hd)->type));}}}}}static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far=0;static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
tud){if(Cyc_Toc_tunions_so_far == 0)Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*
_tmp5B2=_cycalloc(sizeof(*_tmp5B2));_tmp5B2->v=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);_tmp5B2;});{
struct _tuple1*_tmp5B3=tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*
s,struct _tuple1*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_tunions_so_far))->v,_tmp5B3))return;(struct Cyc_Set_Set*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(void*)((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_insert)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp5B3));{struct Cyc_List_List*
_tmp5B4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;
for(0;_tmp5B4 != 0;_tmp5B4=_tmp5B4->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp5B4->hd;if(f->typs != 0){struct Cyc_List_List*_tmp5B5=0;int i=1;{struct Cyc_List_List*
_tmp5B6=f->typs;for(0;_tmp5B6 != 0;(_tmp5B6=_tmp5B6->tl,i ++)){struct _tagged_arr*
_tmp5B7=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*_tmp5B8=({struct Cyc_Absyn_Aggrfield*
_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->name=_tmp5B7;_tmp5BA->tq=(*((struct
_tuple4*)_tmp5B6->hd)).f1;_tmp5BA->type=(void*)Cyc_Toc_typ_to_c_array((*((struct
_tuple4*)_tmp5B6->hd)).f2);_tmp5BA->width=0;_tmp5BA->attributes=0;_tmp5BA;});
_tmp5B5=({struct Cyc_List_List*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9->hd=
_tmp5B8;_tmp5B9->tl=_tmp5B5;_tmp5B9;});}}_tmp5B5=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5B5);_tmp5B5=({struct Cyc_List_List*
_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->hd=({struct Cyc_Absyn_Aggrfield*
_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC->name=Cyc_Toc_tag_sp;_tmp5BC->tq=Cyc_Toc_mt_tq;
_tmp5BC->type=(void*)Cyc_Absyn_sint_typ;_tmp5BC->width=0;_tmp5BC->attributes=0;
_tmp5BC;});_tmp5BB->tl=_tmp5B5;_tmp5BB;});{struct Cyc_Absyn_Aggrdecl*_tmp5BD=({
struct Cyc_Absyn_Aggrdecl*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1->kind=(void*)((
void*)0);_tmp5C1->sc=(void*)((void*)2);_tmp5C1->name=Cyc_Toc_collapse_qvar_tag(f->name,({
const char*_tmp5C3="_struct";_tag_arr(_tmp5C3,sizeof(char),_get_zero_arr_size(
_tmp5C3,8));}));_tmp5C1->tvs=0;_tmp5C1->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2->exist_vars=0;_tmp5C2->rgn_po=0;
_tmp5C2->fields=_tmp5B5;_tmp5C2;});_tmp5C1->attributes=0;_tmp5C1;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));
_tmp5BF[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp5C0;_tmp5C0.tag=4;_tmp5C0.f1=
_tmp5BD;_tmp5C0;});_tmp5BF;}),0);_tmp5BE->tl=Cyc_Toc_result_decls;_tmp5BE;});}}}}}}
static struct Cyc_Core_Opt*Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far == 0)Cyc_Toc_xtunion_fields_so_far=({
struct Cyc_Core_Opt*_tmp5C4=_cycalloc(sizeof(*_tmp5C4));_tmp5C4->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp5C4;});if(xd->fields == 0)return;{struct _tuple1*_tmp5C5=xd->name;struct Cyc_List_List*
_tmp5C6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp5C6 != 0;_tmp5C6=_tmp5C6->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp5C6->hd;struct _tagged_arr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp5C7=Cyc_Absyn_uint_exp(
_get_arr_size(*fn,sizeof(char))+ 4,0);void*_tmp5C8=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)_tmp5C7,Cyc_Absyn_false_conref,0);struct Cyc_Core_Opt*
_tmp5C9=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name);struct Cyc_Core_Opt _tmp5CA;int _tmp5CB;_LL36B: if(_tmp5C9 != 0)goto _LL36D;
_LL36C: {struct Cyc_Absyn_Exp*initopt=0;if((void*)f->sc != (void*)3){char zero='\000';
initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((struct _tagged_arr)({struct
Cyc_String_pa_struct _tmp5D2;_tmp5D2.tag=0;_tmp5D2.f1=(struct _tagged_arr)((struct
_tagged_arr)*fn);{struct Cyc_Int_pa_struct _tmp5D1;_tmp5D1.tag=1;_tmp5D1.f1=(
unsigned int)((int)zero);{struct Cyc_Int_pa_struct _tmp5D0;_tmp5D0.tag=1;_tmp5D0.f1=(
unsigned int)((int)zero);{struct Cyc_Int_pa_struct _tmp5CF;_tmp5CF.tag=1;_tmp5CF.f1=(
unsigned int)((int)zero);{struct Cyc_Int_pa_struct _tmp5CE;_tmp5CE.tag=1;_tmp5CE.f1=(
unsigned int)((int)zero);{void*_tmp5CC[5]={& _tmp5CE,& _tmp5CF,& _tmp5D0,& _tmp5D1,&
_tmp5D2};Cyc_aprintf(({const char*_tmp5CD="%c%c%c%c%s";_tag_arr(_tmp5CD,sizeof(
char),_get_zero_arr_size(_tmp5CD,11));}),_tag_arr(_tmp5CC,sizeof(void*),5));}}}}}}),
0);}{struct Cyc_Absyn_Vardecl*_tmp5D3=Cyc_Absyn_new_vardecl(f->name,_tmp5C8,
initopt);(void*)(_tmp5D3->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5[
0]=({struct Cyc_Absyn_Var_d_struct _tmp5D6;_tmp5D6.tag=0;_tmp5D6.f1=_tmp5D3;
_tmp5D6;});_tmp5D5;}),0);_tmp5D4->tl=Cyc_Toc_result_decls;_tmp5D4;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,(void*)f->sc != (void*)3));if(f->typs != 0){struct Cyc_List_List*fields=0;
int i=1;{struct Cyc_List_List*_tmp5D7=f->typs;for(0;_tmp5D7 != 0;(_tmp5D7=_tmp5D7->tl,
i ++)){struct _tagged_arr*_tmp5D8=({struct _tagged_arr*_tmp5DC=_cycalloc(sizeof(*
_tmp5DC));_tmp5DC[0]=(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp5DF;_tmp5DF.tag=
1;_tmp5DF.f1=(unsigned int)i;{void*_tmp5DD[1]={& _tmp5DF};Cyc_aprintf(({const char*
_tmp5DE="f%d";_tag_arr(_tmp5DE,sizeof(char),_get_zero_arr_size(_tmp5DE,4));}),
_tag_arr(_tmp5DD,sizeof(void*),1));}});_tmp5DC;});struct Cyc_Absyn_Aggrfield*
_tmp5D9=({struct Cyc_Absyn_Aggrfield*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB->name=
_tmp5D8;_tmp5DB->tq=(*((struct _tuple4*)_tmp5D7->hd)).f1;_tmp5DB->type=(void*)Cyc_Toc_typ_to_c_array((*((
struct _tuple4*)_tmp5D7->hd)).f2);_tmp5DB->width=0;_tmp5DB->attributes=0;_tmp5DB;});
fields=({struct Cyc_List_List*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->hd=
_tmp5D9;_tmp5DA->tl=fields;_tmp5DA;});}}fields=({struct Cyc_List_List*_tmp5E0=
_cycalloc(sizeof(*_tmp5E0));_tmp5E0->hd=({struct Cyc_Absyn_Aggrfield*_tmp5E1=
_cycalloc(sizeof(*_tmp5E1));_tmp5E1->name=Cyc_Toc_tag_sp;_tmp5E1->tq=Cyc_Toc_mt_tq;
_tmp5E1->type=(void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);
_tmp5E1->width=0;_tmp5E1->attributes=0;_tmp5E1;});_tmp5E0->tl=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp5E0;});{struct Cyc_Absyn_Aggrdecl*
_tmp5E2=({struct Cyc_Absyn_Aggrdecl*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6->kind=(
void*)((void*)0);_tmp5E6->sc=(void*)((void*)2);_tmp5E6->name=Cyc_Toc_collapse_qvar_tag(
f->name,({const char*_tmp5E8="_struct";_tag_arr(_tmp5E8,sizeof(char),
_get_zero_arr_size(_tmp5E8,8));}));_tmp5E6->tvs=0;_tmp5E6->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7->exist_vars=0;_tmp5E7->rgn_po=0;
_tmp5E7->fields=fields;_tmp5E7;});_tmp5E6->attributes=0;_tmp5E6;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));_tmp5E3->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));
_tmp5E4[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp5E5;_tmp5E5.tag=4;_tmp5E5.f1=
_tmp5E2;_tmp5E5;});_tmp5E4;}),0);_tmp5E3->tl=Cyc_Toc_result_decls;_tmp5E3;});}}
goto _LL36A;}}_LL36D: if(_tmp5C9 == 0)goto _LL36F;_tmp5CA=*_tmp5C9;_tmp5CB=(int)
_tmp5CA.v;if(_tmp5CB != 0)goto _LL36F;_LL36E: if((void*)f->sc != (void*)3){char zero='\000';
struct Cyc_Absyn_Exp*_tmp5E9=Cyc_Absyn_string_exp((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp5F4;_tmp5F4.tag=0;_tmp5F4.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
struct Cyc_Int_pa_struct _tmp5F3;_tmp5F3.tag=1;_tmp5F3.f1=(unsigned int)((int)zero);{
struct Cyc_Int_pa_struct _tmp5F2;_tmp5F2.tag=1;_tmp5F2.f1=(unsigned int)((int)zero);{
struct Cyc_Int_pa_struct _tmp5F1;_tmp5F1.tag=1;_tmp5F1.f1=(unsigned int)((int)zero);{
struct Cyc_Int_pa_struct _tmp5F0;_tmp5F0.tag=1;_tmp5F0.f1=(unsigned int)((int)zero);{
void*_tmp5EE[5]={& _tmp5F0,& _tmp5F1,& _tmp5F2,& _tmp5F3,& _tmp5F4};Cyc_aprintf(({
const char*_tmp5EF="%c%c%c%c%s";_tag_arr(_tmp5EF,sizeof(char),_get_zero_arr_size(
_tmp5EF,11));}),_tag_arr(_tmp5EE,sizeof(void*),5));}}}}}}),0);struct Cyc_Absyn_Vardecl*
_tmp5EA=Cyc_Absyn_new_vardecl(f->name,_tmp5C8,(struct Cyc_Absyn_Exp*)_tmp5E9);(
void*)(_tmp5EA->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp5EB=_cycalloc(sizeof(*_tmp5EB));_tmp5EB->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC[0]=({
struct Cyc_Absyn_Var_d_struct _tmp5ED;_tmp5ED.tag=0;_tmp5ED.f1=_tmp5EA;_tmp5ED;});
_tmp5EC;}),0);_tmp5EB->tl=Cyc_Toc_result_decls;_tmp5EB;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,1));}goto _LL36A;_LL36F:;_LL370: goto _LL36A;_LL36A:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));if((void*)vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp5F5=(void*)init->r;struct Cyc_Absyn_Vardecl*_tmp5F6;struct Cyc_Absyn_Exp*
_tmp5F7;struct Cyc_Absyn_Exp*_tmp5F8;int _tmp5F9;_LL372: if(*((int*)_tmp5F5)!= 29)
goto _LL374;_tmp5F6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp5F5)->f1;
_tmp5F7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp5F5)->f2;_tmp5F8=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp5F5)->f3;_tmp5F9=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp5F5)->f4;_LL373: vd->initializer=0;(void*)(s->r=(void*)((void*)(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp5F6,_tmp5F7,_tmp5F8,_tmp5F9,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));goto _LL371;_LL374:;_LL375: if((void*)vd->sc == (void*)0){
struct Cyc_Toc_Env*_tmp5FA=({struct Cyc_Toc_Env*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));
_tmp5FB[0]=*init_nv;_tmp5FB;});_tmp5FA->toplevel=1;Cyc_Toc_exp_to_c(_tmp5FA,init);}
else{Cyc_Toc_exp_to_c(init_nv,init);}goto _LL371;_LL371:;}else{void*_tmp5FC=Cyc_Tcutil_compress(
old_typ);struct Cyc_Absyn_ArrayInfo _tmp5FD;void*_tmp5FE;struct Cyc_Absyn_Exp*
_tmp5FF;struct Cyc_Absyn_Conref*_tmp600;_LL377: if(_tmp5FC <= (void*)4  || *((int*)
_tmp5FC)!= 7)goto _LL379;_tmp5FD=((struct Cyc_Absyn_ArrayType_struct*)_tmp5FC)->f1;
_tmp5FE=(void*)_tmp5FD.elt_type;_tmp5FF=_tmp5FD.num_elts;_tmp600=_tmp5FD.zero_term;
if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp600))goto
_LL379;_LL378: if(_tmp5FF == 0)({void*_tmp601[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp602="can't initialize zero-terminated array -- size unknown";
_tag_arr(_tmp602,sizeof(char),_get_zero_arr_size(_tmp602,55));}),_tag_arr(
_tmp601,sizeof(void*),0));});{struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)
_tmp5FF;struct Cyc_Absyn_Exp*_tmp603=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,
0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*
_tmp604=Cyc_Absyn_signed_int_exp(0,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp603,_tmp604,0),0),Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));goto _LL376;}_LL379:;_LL37A: goto _LL376;_LL376:;}}static
struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=0;static struct Cyc_Absyn_Stmt*
Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt == 0)Cyc_Toc_throw_match_stmt_opt=({
struct Cyc_Absyn_Stmt**_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605[0]=Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(0)),0);_tmp605;});return*((struct
Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{struct _tuple1*x=Cyc_Toc_temp_var();{
struct _RegionHandle _tmp606=_new_region("prgn");struct _RegionHandle*prgn=& _tmp606;
_push_region(prgn);{struct Cyc_Toc_Env*_tmp608;struct Cyc_List_List*_tmp609;struct
Cyc_Absyn_Stmt*_tmp60A;struct _tuple12 _tmp607=Cyc_Toc_xlate_pat(nv,prgn,t,Cyc_Absyn_var_exp(
x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),0);_tmp608=_tmp607.f1;
_tmp609=_tmp607.f2;_tmp60A=_tmp607.f3;Cyc_Toc_stmt_to_c(_tmp608,s);s=Cyc_Absyn_declare_stmt(
x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp60A,s,0),0);
for(0;_tmp609 != 0;_tmp609=_tmp609->tl){struct _tuple13 _tmp60C;struct _tuple1*
_tmp60D;void*_tmp60E;struct _tuple13*_tmp60B=(struct _tuple13*)_tmp609->hd;_tmp60C=*
_tmp60B;_tmp60D=_tmp60C.f1;_tmp60E=_tmp60C.f2;s=Cyc_Absyn_declare_stmt(_tmp60D,
_tmp60E,0,s,0);}};_pop_region(prgn);}return s;}}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e){void*_tmp60F=(void*)e->r;struct Cyc_Absyn_Exp*_tmp610;
struct Cyc_Absyn_Exp*_tmp611;struct Cyc_Absyn_Exp*_tmp612;struct Cyc_Absyn_Exp*
_tmp613;struct Cyc_Absyn_Exp*_tmp614;struct Cyc_Absyn_Exp*_tmp615;struct Cyc_Absyn_Exp*
_tmp616;struct Cyc_Absyn_Exp*_tmp617;struct Cyc_List_List*_tmp618;struct Cyc_Absyn_Exp*
_tmp619;struct Cyc_Absyn_Exp*_tmp61A;struct Cyc_Absyn_Exp*_tmp61B;struct Cyc_Absyn_Exp*
_tmp61C;struct Cyc_Absyn_Exp*_tmp61D;struct Cyc_Absyn_Exp*_tmp61E;struct Cyc_Absyn_Exp*
_tmp61F;struct Cyc_Absyn_Exp*_tmp620;struct Cyc_Absyn_Exp*_tmp621;struct Cyc_Absyn_Exp*
_tmp622;struct Cyc_Absyn_Exp*_tmp623;struct Cyc_Absyn_Exp*_tmp624;struct Cyc_Absyn_Exp*
_tmp625;struct Cyc_Absyn_Exp*_tmp626;struct Cyc_List_List*_tmp627;struct Cyc_Absyn_Exp*
_tmp628;struct Cyc_List_List*_tmp629;void*_tmp62A;void**_tmp62B;struct Cyc_Absyn_Exp*
_tmp62C;struct _tuple2*_tmp62D;struct _tuple2 _tmp62E;void*_tmp62F;void**_tmp630;
struct Cyc_List_List*_tmp631;struct Cyc_List_List*_tmp632;struct Cyc_List_List*
_tmp633;void*_tmp634;void**_tmp635;void*_tmp636;void**_tmp637;struct Cyc_Absyn_Stmt*
_tmp638;struct Cyc_Absyn_MallocInfo _tmp639;struct Cyc_Absyn_MallocInfo*_tmp63A;
_LL37C: if(*((int*)_tmp60F)!= 22)goto _LL37E;_tmp610=((struct Cyc_Absyn_Deref_e_struct*)
_tmp60F)->f1;_LL37D: _tmp611=_tmp610;goto _LL37F;_LL37E: if(*((int*)_tmp60F)!= 23)
goto _LL380;_tmp611=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp60F)->f1;_LL37F:
_tmp612=_tmp611;goto _LL381;_LL380: if(*((int*)_tmp60F)!= 24)goto _LL382;_tmp612=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp60F)->f1;_LL381: _tmp613=_tmp612;goto
_LL383;_LL382: if(*((int*)_tmp60F)!= 16)goto _LL384;_tmp613=((struct Cyc_Absyn_Address_e_struct*)
_tmp60F)->f1;_LL383: _tmp614=_tmp613;goto _LL385;_LL384: if(*((int*)_tmp60F)!= 12)
goto _LL386;_tmp614=((struct Cyc_Absyn_Throw_e_struct*)_tmp60F)->f1;_LL385: _tmp615=
_tmp614;goto _LL387;_LL386: if(*((int*)_tmp60F)!= 13)goto _LL388;_tmp615=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp60F)->f1;_LL387: _tmp616=_tmp615;goto _LL389;
_LL388: if(*((int*)_tmp60F)!= 19)goto _LL38A;_tmp616=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp60F)->f1;_LL389: _tmp617=_tmp616;goto _LL38B;_LL38A: if(*((int*)_tmp60F)!= 5)
goto _LL38C;_tmp617=((struct Cyc_Absyn_Increment_e_struct*)_tmp60F)->f1;_LL38B: Cyc_Toc_exptypes_to_c(
_tmp617);goto _LL37B;_LL38C: if(*((int*)_tmp60F)!= 3)goto _LL38E;_tmp618=((struct
Cyc_Absyn_Primop_e_struct*)_tmp60F)->f2;_LL38D:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp618);goto _LL37B;
_LL38E: if(*((int*)_tmp60F)!= 7)goto _LL390;_tmp619=((struct Cyc_Absyn_And_e_struct*)
_tmp60F)->f1;_tmp61A=((struct Cyc_Absyn_And_e_struct*)_tmp60F)->f2;_LL38F: _tmp61B=
_tmp619;_tmp61C=_tmp61A;goto _LL391;_LL390: if(*((int*)_tmp60F)!= 8)goto _LL392;
_tmp61B=((struct Cyc_Absyn_Or_e_struct*)_tmp60F)->f1;_tmp61C=((struct Cyc_Absyn_Or_e_struct*)
_tmp60F)->f2;_LL391: _tmp61D=_tmp61B;_tmp61E=_tmp61C;goto _LL393;_LL392: if(*((int*)
_tmp60F)!= 9)goto _LL394;_tmp61D=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp60F)->f1;
_tmp61E=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp60F)->f2;_LL393: _tmp61F=_tmp61D;
_tmp620=_tmp61E;goto _LL395;_LL394: if(*((int*)_tmp60F)!= 25)goto _LL396;_tmp61F=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp60F)->f1;_tmp620=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp60F)->f2;_LL395: _tmp621=_tmp61F;_tmp622=_tmp620;goto _LL397;_LL396: if(*((int*)
_tmp60F)!= 4)goto _LL398;_tmp621=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp60F)->f1;
_tmp622=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp60F)->f3;_LL397: Cyc_Toc_exptypes_to_c(
_tmp621);Cyc_Toc_exptypes_to_c(_tmp622);goto _LL37B;_LL398: if(*((int*)_tmp60F)!= 
6)goto _LL39A;_tmp623=((struct Cyc_Absyn_Conditional_e_struct*)_tmp60F)->f1;
_tmp624=((struct Cyc_Absyn_Conditional_e_struct*)_tmp60F)->f2;_tmp625=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp60F)->f3;_LL399: Cyc_Toc_exptypes_to_c(_tmp623);Cyc_Toc_exptypes_to_c(_tmp624);
Cyc_Toc_exptypes_to_c(_tmp625);goto _LL37B;_LL39A: if(*((int*)_tmp60F)!= 11)goto
_LL39C;_tmp626=((struct Cyc_Absyn_FnCall_e_struct*)_tmp60F)->f1;_tmp627=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp60F)->f2;_LL39B: _tmp628=_tmp626;_tmp629=_tmp627;
goto _LL39D;_LL39C: if(*((int*)_tmp60F)!= 10)goto _LL39E;_tmp628=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp60F)->f1;_tmp629=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp60F)->f2;_LL39D:
Cyc_Toc_exptypes_to_c(_tmp628);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp629);goto _LL37B;_LL39E:
if(*((int*)_tmp60F)!= 15)goto _LL3A0;_tmp62A=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp60F)->f1;_tmp62B=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp60F)->f1);
_tmp62C=((struct Cyc_Absyn_Cast_e_struct*)_tmp60F)->f2;_LL39F:*_tmp62B=Cyc_Toc_typ_to_c(*
_tmp62B);Cyc_Toc_exptypes_to_c(_tmp62C);goto _LL37B;_LL3A0: if(*((int*)_tmp60F)!= 
27)goto _LL3A2;_tmp62D=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp60F)->f1;
_tmp62E=*_tmp62D;_tmp62F=_tmp62E.f3;_tmp630=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp60F)->f1).f3;_tmp631=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp60F)->f2;
_LL3A1:*_tmp630=Cyc_Toc_typ_to_c(*_tmp630);_tmp632=_tmp631;goto _LL3A3;_LL3A2: if(*((
int*)_tmp60F)!= 36)goto _LL3A4;_tmp632=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp60F)->f2;_LL3A3: _tmp633=_tmp632;goto _LL3A5;_LL3A4: if(*((int*)_tmp60F)!= 28)
goto _LL3A6;_tmp633=((struct Cyc_Absyn_Array_e_struct*)_tmp60F)->f1;_LL3A5: for(0;
_tmp633 != 0;_tmp633=_tmp633->tl){struct Cyc_Absyn_Exp*_tmp63C;struct _tuple6
_tmp63B=*((struct _tuple6*)_tmp633->hd);_tmp63C=_tmp63B.f2;Cyc_Toc_exptypes_to_c(
_tmp63C);}goto _LL37B;_LL3A6: if(*((int*)_tmp60F)!= 20)goto _LL3A8;_tmp634=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp60F)->f1;_tmp635=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp60F)->f1);_LL3A7: _tmp637=_tmp635;goto
_LL3A9;_LL3A8: if(*((int*)_tmp60F)!= 18)goto _LL3AA;_tmp636=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp60F)->f1;_tmp637=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp60F)->f1);_LL3A9:*_tmp637=Cyc_Toc_typ_to_c(*_tmp637);goto _LL37B;_LL3AA: if(*((
int*)_tmp60F)!= 37)goto _LL3AC;_tmp638=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp60F)->f1;_LL3AB: Cyc_Toc_stmttypes_to_c(_tmp638);goto _LL37B;_LL3AC: if(*((int*)
_tmp60F)!= 35)goto _LL3AE;_tmp639=((struct Cyc_Absyn_Malloc_e_struct*)_tmp60F)->f1;
_tmp63A=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp60F)->f1;
_LL3AD: if(_tmp63A->elt_type != 0)_tmp63A->elt_type=({void**_tmp63D=_cycalloc(
sizeof(*_tmp63D));_tmp63D[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp63A->elt_type)));
_tmp63D;});Cyc_Toc_exptypes_to_c(_tmp63A->num_elts);goto _LL37B;_LL3AE: if(*((int*)
_tmp60F)!= 0)goto _LL3B0;_LL3AF: goto _LL3B1;_LL3B0: if(*((int*)_tmp60F)!= 1)goto
_LL3B2;_LL3B1: goto _LL3B3;_LL3B2: if(*((int*)_tmp60F)!= 2)goto _LL3B4;_LL3B3: goto
_LL3B5;_LL3B4: if(*((int*)_tmp60F)!= 33)goto _LL3B6;_LL3B5: goto _LL3B7;_LL3B6: if(*((
int*)_tmp60F)!= 34)goto _LL3B8;_LL3B7: goto _LL37B;_LL3B8: if(*((int*)_tmp60F)!= 31)
goto _LL3BA;_LL3B9: goto _LL3BB;_LL3BA: if(*((int*)_tmp60F)!= 32)goto _LL3BC;_LL3BB:
goto _LL3BD;_LL3BC: if(*((int*)_tmp60F)!= 30)goto _LL3BE;_LL3BD: goto _LL3BF;_LL3BE:
if(*((int*)_tmp60F)!= 29)goto _LL3C0;_LL3BF: goto _LL3C1;_LL3C0: if(*((int*)_tmp60F)
!= 26)goto _LL3C2;_LL3C1: goto _LL3C3;_LL3C2: if(*((int*)_tmp60F)!= 14)goto _LL3C4;
_LL3C3: goto _LL3C5;_LL3C4: if(*((int*)_tmp60F)!= 17)goto _LL3C6;_LL3C5: goto _LL3C7;
_LL3C6: if(*((int*)_tmp60F)!= 21)goto _LL37B;_LL3C7:({void*_tmp63E[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp63F="Cyclone expression within C code";_tag_arr(_tmp63F,
sizeof(char),_get_zero_arr_size(_tmp63F,33));}),_tag_arr(_tmp63E,sizeof(void*),0));});
goto _LL37B;_LL37B:;}static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp640=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp641;struct Cyc_Absyn_Fndecl*
_tmp642;struct Cyc_Absyn_Aggrdecl*_tmp643;struct Cyc_Absyn_Enumdecl*_tmp644;struct
Cyc_Absyn_Typedefdecl*_tmp645;_LL3C9: if(_tmp640 <= (void*)2  || *((int*)_tmp640)!= 
0)goto _LL3CB;_tmp641=((struct Cyc_Absyn_Var_d_struct*)_tmp640)->f1;_LL3CA:(void*)(
_tmp641->type=(void*)Cyc_Toc_typ_to_c((void*)_tmp641->type));if(_tmp641->initializer
!= 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp641->initializer));
goto _LL3C8;_LL3CB: if(_tmp640 <= (void*)2  || *((int*)_tmp640)!= 1)goto _LL3CD;
_tmp642=((struct Cyc_Absyn_Fn_d_struct*)_tmp640)->f1;_LL3CC:(void*)(_tmp642->ret_type=(
void*)Cyc_Toc_typ_to_c((void*)_tmp642->ret_type));{struct Cyc_List_List*_tmp646=
_tmp642->args;for(0;_tmp646 != 0;_tmp646=_tmp646->tl){(*((struct _tuple16*)_tmp646->hd)).f3=
Cyc_Toc_typ_to_c((*((struct _tuple16*)_tmp646->hd)).f3);}}goto _LL3C8;_LL3CD: if(
_tmp640 <= (void*)2  || *((int*)_tmp640)!= 4)goto _LL3CF;_tmp643=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp640)->f1;_LL3CE: Cyc_Toc_aggrdecl_to_c(_tmp643);goto _LL3C8;_LL3CF: if(_tmp640
<= (void*)2  || *((int*)_tmp640)!= 6)goto _LL3D1;_tmp644=((struct Cyc_Absyn_Enum_d_struct*)
_tmp640)->f1;_LL3D0: if(_tmp644->fields != 0){struct Cyc_List_List*_tmp647=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp644->fields))->v;for(0;
_tmp647 != 0;_tmp647=_tmp647->tl){struct Cyc_Absyn_Enumfield*_tmp648=(struct Cyc_Absyn_Enumfield*)
_tmp647->hd;if(_tmp648->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp648->tag));}}goto _LL3C8;_LL3D1: if(_tmp640 <= (void*)2  || *((int*)
_tmp640)!= 7)goto _LL3D3;_tmp645=((struct Cyc_Absyn_Typedef_d_struct*)_tmp640)->f1;
_LL3D2: _tmp645->defn=({struct Cyc_Core_Opt*_tmp649=_cycalloc(sizeof(*_tmp649));
_tmp649->v=(void*)Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp645->defn))->v);_tmp649;});goto _LL3C8;_LL3D3: if(_tmp640 <= (void*)
2  || *((int*)_tmp640)!= 2)goto _LL3D5;_LL3D4: goto _LL3D6;_LL3D5: if(_tmp640 <= (void*)
2  || *((int*)_tmp640)!= 3)goto _LL3D7;_LL3D6: goto _LL3D8;_LL3D7: if(_tmp640 <= (void*)
2  || *((int*)_tmp640)!= 5)goto _LL3D9;_LL3D8: goto _LL3DA;_LL3D9: if(_tmp640 <= (void*)
2  || *((int*)_tmp640)!= 8)goto _LL3DB;_LL3DA: goto _LL3DC;_LL3DB: if(_tmp640 <= (void*)
2  || *((int*)_tmp640)!= 9)goto _LL3DD;_LL3DC: goto _LL3DE;_LL3DD: if(_tmp640 <= (void*)
2  || *((int*)_tmp640)!= 10)goto _LL3DF;_LL3DE: goto _LL3E0;_LL3DF: if(_tmp640 <= (
void*)2  || *((int*)_tmp640)!= 11)goto _LL3E1;_LL3E0:({void*_tmp64A[0]={};Cyc_Tcutil_terr(
d->loc,({const char*_tmp64B="Cyclone declaration within C code";_tag_arr(_tmp64B,
sizeof(char),_get_zero_arr_size(_tmp64B,34));}),_tag_arr(_tmp64A,sizeof(void*),0));});
goto _LL3C8;_LL3E1: if((int)_tmp640 != 0)goto _LL3E3;_LL3E2: goto _LL3E4;_LL3E3: if((
int)_tmp640 != 1)goto _LL3C8;_LL3E4: goto _LL3C8;_LL3C8:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s){void*_tmp64C=(void*)s->r;struct Cyc_Absyn_Exp*_tmp64D;
struct Cyc_Absyn_Stmt*_tmp64E;struct Cyc_Absyn_Stmt*_tmp64F;struct Cyc_Absyn_Exp*
_tmp650;struct Cyc_Absyn_Exp*_tmp651;struct Cyc_Absyn_Stmt*_tmp652;struct Cyc_Absyn_Stmt*
_tmp653;struct _tuple3 _tmp654;struct Cyc_Absyn_Exp*_tmp655;struct Cyc_Absyn_Stmt*
_tmp656;struct Cyc_Absyn_Exp*_tmp657;struct _tuple3 _tmp658;struct Cyc_Absyn_Exp*
_tmp659;struct _tuple3 _tmp65A;struct Cyc_Absyn_Exp*_tmp65B;struct Cyc_Absyn_Stmt*
_tmp65C;struct Cyc_Absyn_Exp*_tmp65D;struct Cyc_List_List*_tmp65E;struct Cyc_Absyn_Decl*
_tmp65F;struct Cyc_Absyn_Stmt*_tmp660;struct Cyc_Absyn_Stmt*_tmp661;struct _tuple3
_tmp662;struct Cyc_Absyn_Exp*_tmp663;_LL3E6: if(_tmp64C <= (void*)1  || *((int*)
_tmp64C)!= 0)goto _LL3E8;_tmp64D=((struct Cyc_Absyn_Exp_s_struct*)_tmp64C)->f1;
_LL3E7: Cyc_Toc_exptypes_to_c(_tmp64D);goto _LL3E5;_LL3E8: if(_tmp64C <= (void*)1
 || *((int*)_tmp64C)!= 1)goto _LL3EA;_tmp64E=((struct Cyc_Absyn_Seq_s_struct*)
_tmp64C)->f1;_tmp64F=((struct Cyc_Absyn_Seq_s_struct*)_tmp64C)->f2;_LL3E9: Cyc_Toc_stmttypes_to_c(
_tmp64E);Cyc_Toc_stmttypes_to_c(_tmp64F);goto _LL3E5;_LL3EA: if(_tmp64C <= (void*)1
 || *((int*)_tmp64C)!= 2)goto _LL3EC;_tmp650=((struct Cyc_Absyn_Return_s_struct*)
_tmp64C)->f1;_LL3EB: if(_tmp650 != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_tmp650);goto _LL3E5;_LL3EC: if(_tmp64C <= (void*)1  || *((int*)_tmp64C)!= 3)goto
_LL3EE;_tmp651=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp64C)->f1;_tmp652=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp64C)->f2;_tmp653=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp64C)->f3;_LL3ED: Cyc_Toc_exptypes_to_c(_tmp651);Cyc_Toc_stmttypes_to_c(
_tmp652);Cyc_Toc_stmttypes_to_c(_tmp653);goto _LL3E5;_LL3EE: if(_tmp64C <= (void*)1
 || *((int*)_tmp64C)!= 4)goto _LL3F0;_tmp654=((struct Cyc_Absyn_While_s_struct*)
_tmp64C)->f1;_tmp655=_tmp654.f1;_tmp656=((struct Cyc_Absyn_While_s_struct*)
_tmp64C)->f2;_LL3EF: Cyc_Toc_exptypes_to_c(_tmp655);Cyc_Toc_stmttypes_to_c(
_tmp656);goto _LL3E5;_LL3F0: if(_tmp64C <= (void*)1  || *((int*)_tmp64C)!= 8)goto
_LL3F2;_tmp657=((struct Cyc_Absyn_For_s_struct*)_tmp64C)->f1;_tmp658=((struct Cyc_Absyn_For_s_struct*)
_tmp64C)->f2;_tmp659=_tmp658.f1;_tmp65A=((struct Cyc_Absyn_For_s_struct*)_tmp64C)->f3;
_tmp65B=_tmp65A.f1;_tmp65C=((struct Cyc_Absyn_For_s_struct*)_tmp64C)->f4;_LL3F1:
Cyc_Toc_exptypes_to_c(_tmp657);Cyc_Toc_exptypes_to_c(_tmp659);Cyc_Toc_exptypes_to_c(
_tmp65B);Cyc_Toc_stmttypes_to_c(_tmp65C);goto _LL3E5;_LL3F2: if(_tmp64C <= (void*)1
 || *((int*)_tmp64C)!= 9)goto _LL3F4;_tmp65D=((struct Cyc_Absyn_Switch_s_struct*)
_tmp64C)->f1;_tmp65E=((struct Cyc_Absyn_Switch_s_struct*)_tmp64C)->f2;_LL3F3: Cyc_Toc_exptypes_to_c(
_tmp65D);for(0;_tmp65E != 0;_tmp65E=_tmp65E->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp65E->hd)->body);}goto _LL3E5;_LL3F4: if(_tmp64C <= (
void*)1  || *((int*)_tmp64C)!= 11)goto _LL3F6;_tmp65F=((struct Cyc_Absyn_Decl_s_struct*)
_tmp64C)->f1;_tmp660=((struct Cyc_Absyn_Decl_s_struct*)_tmp64C)->f2;_LL3F5: Cyc_Toc_decltypes_to_c(
_tmp65F);Cyc_Toc_stmttypes_to_c(_tmp660);goto _LL3E5;_LL3F6: if(_tmp64C <= (void*)1
 || *((int*)_tmp64C)!= 13)goto _LL3F8;_tmp661=((struct Cyc_Absyn_Do_s_struct*)
_tmp64C)->f1;_tmp662=((struct Cyc_Absyn_Do_s_struct*)_tmp64C)->f2;_tmp663=_tmp662.f1;
_LL3F7: Cyc_Toc_stmttypes_to_c(_tmp661);Cyc_Toc_exptypes_to_c(_tmp663);goto _LL3E5;
_LL3F8: if((int)_tmp64C != 0)goto _LL3FA;_LL3F9: goto _LL3FB;_LL3FA: if(_tmp64C <= (
void*)1  || *((int*)_tmp64C)!= 5)goto _LL3FC;_LL3FB: goto _LL3FD;_LL3FC: if(_tmp64C <= (
void*)1  || *((int*)_tmp64C)!= 6)goto _LL3FE;_LL3FD: goto _LL3FF;_LL3FE: if(_tmp64C <= (
void*)1  || *((int*)_tmp64C)!= 7)goto _LL400;_LL3FF: goto _LL3E5;_LL400: if(_tmp64C <= (
void*)1  || *((int*)_tmp64C)!= 10)goto _LL402;_LL401: goto _LL403;_LL402: if(_tmp64C
<= (void*)1  || *((int*)_tmp64C)!= 12)goto _LL404;_LL403: goto _LL405;_LL404: if(
_tmp64C <= (void*)1  || *((int*)_tmp64C)!= 14)goto _LL406;_LL405: goto _LL407;_LL406:
if(_tmp64C <= (void*)1  || *((int*)_tmp64C)!= 15)goto _LL408;_LL407: goto _LL409;
_LL408: if(_tmp64C <= (void*)1  || *((int*)_tmp64C)!= 16)goto _LL3E5;_LL409:({void*
_tmp664[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp665="Cyclone statement in C code";
_tag_arr(_tmp665,sizeof(char),_get_zero_arr_size(_tmp665,28));}),_tag_arr(
_tmp664,sizeof(void*),0));});goto _LL3E5;_LL3E5:;}static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){for(0;ds != 0;ds=
ds->tl){if(!nv->toplevel)({void*_tmp666[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp667="decls_to_c: not at toplevel!";
_tag_arr(_tmp667,sizeof(char),_get_zero_arr_size(_tmp667,29));}),_tag_arr(
_tmp666,sizeof(void*),0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp668=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp669;struct Cyc_Absyn_Fndecl*
_tmp66A;struct Cyc_Absyn_Aggrdecl*_tmp66B;struct Cyc_Absyn_Tuniondecl*_tmp66C;
struct Cyc_Absyn_Enumdecl*_tmp66D;struct Cyc_Absyn_Typedefdecl*_tmp66E;struct Cyc_List_List*
_tmp66F;struct Cyc_List_List*_tmp670;struct Cyc_List_List*_tmp671;struct Cyc_List_List*
_tmp672;_LL40B: if(_tmp668 <= (void*)2  || *((int*)_tmp668)!= 0)goto _LL40D;_tmp669=((
struct Cyc_Absyn_Var_d_struct*)_tmp668)->f1;_LL40C: {struct _tuple1*_tmp673=
_tmp669->name;if((void*)_tmp669->sc == (void*)4)_tmp673=({struct _tuple1*_tmp674=
_cycalloc(sizeof(*_tmp674));_tmp674->f1=Cyc_Absyn_rel_ns_null;_tmp674->f2=(*
_tmp673).f2;_tmp674;});if(_tmp669->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp669->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmp669->initializer));}}nv=Cyc_Toc_add_varmap(
nv,_tmp669->name,Cyc_Absyn_varb_exp(_tmp673,(void*)({struct Cyc_Absyn_Global_b_struct*
_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675[0]=({struct Cyc_Absyn_Global_b_struct
_tmp676;_tmp676.tag=0;_tmp676.f1=_tmp669;_tmp676;});_tmp675;}),0));_tmp669->name=
_tmp673;(void*)(_tmp669->sc=(void*)Cyc_Toc_scope_to_c((void*)_tmp669->sc));(void*)(
_tmp669->type=(void*)Cyc_Toc_typ_to_c_array((void*)_tmp669->type));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677->hd=d;_tmp677->tl=
Cyc_Toc_result_decls;_tmp677;});goto _LL40A;}_LL40D: if(_tmp668 <= (void*)2  || *((
int*)_tmp668)!= 1)goto _LL40F;_tmp66A=((struct Cyc_Absyn_Fn_d_struct*)_tmp668)->f1;
_LL40E: {struct _tuple1*_tmp678=_tmp66A->name;if((void*)_tmp66A->sc == (void*)4){
_tmp678=({struct _tuple1*_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->f1=Cyc_Absyn_rel_ns_null;
_tmp679->f2=(*_tmp678).f2;_tmp679;});(void*)(_tmp66A->sc=(void*)((void*)2));}nv=
Cyc_Toc_add_varmap(nv,_tmp66A->name,Cyc_Absyn_var_exp(_tmp678,0));_tmp66A->name=
_tmp678;Cyc_Toc_fndecl_to_c(nv,_tmp66A,cinclude);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A->hd=d;_tmp67A->tl=Cyc_Toc_result_decls;
_tmp67A;});goto _LL40A;}_LL40F: if(_tmp668 <= (void*)2  || *((int*)_tmp668)!= 2)goto
_LL411;_LL410: goto _LL412;_LL411: if(_tmp668 <= (void*)2  || *((int*)_tmp668)!= 3)
goto _LL413;_LL412:({void*_tmp67B[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp67C="letdecl at toplevel";
_tag_arr(_tmp67C,sizeof(char),_get_zero_arr_size(_tmp67C,20));}),_tag_arr(
_tmp67B,sizeof(void*),0));});_LL413: if(_tmp668 <= (void*)2  || *((int*)_tmp668)!= 
4)goto _LL415;_tmp66B=((struct Cyc_Absyn_Aggr_d_struct*)_tmp668)->f1;_LL414: Cyc_Toc_aggrdecl_to_c(
_tmp66B);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp67D=_cycalloc(sizeof(*
_tmp67D));_tmp67D->hd=d;_tmp67D->tl=Cyc_Toc_result_decls;_tmp67D;});goto _LL40A;
_LL415: if(_tmp668 <= (void*)2  || *((int*)_tmp668)!= 5)goto _LL417;_tmp66C=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp668)->f1;_LL416: if(_tmp66C->is_xtunion)Cyc_Toc_xtuniondecl_to_c(
_tmp66C);else{Cyc_Toc_tuniondecl_to_c(_tmp66C);}goto _LL40A;_LL417: if(_tmp668 <= (
void*)2  || *((int*)_tmp668)!= 6)goto _LL419;_tmp66D=((struct Cyc_Absyn_Enum_d_struct*)
_tmp668)->f1;_LL418: Cyc_Toc_enumdecl_to_c(nv,_tmp66D);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E->hd=d;_tmp67E->tl=
Cyc_Toc_result_decls;_tmp67E;});goto _LL40A;_LL419: if(_tmp668 <= (void*)2  || *((
int*)_tmp668)!= 7)goto _LL41B;_tmp66E=((struct Cyc_Absyn_Typedef_d_struct*)_tmp668)->f1;
_LL41A: _tmp66E->name=_tmp66E->name;_tmp66E->tvs=0;if(_tmp66E->defn != 0)_tmp66E->defn=({
struct Cyc_Core_Opt*_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp66E->defn))->v);_tmp67F;});else{void*
_tmp680=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp66E->kind))->v;_LL428: if((
int)_tmp680 != 2)goto _LL42A;_LL429: _tmp66E->defn=({struct Cyc_Core_Opt*_tmp681=
_cycalloc(sizeof(*_tmp681));_tmp681->v=(void*)Cyc_Absyn_void_star_typ();_tmp681;});
goto _LL427;_LL42A:;_LL42B: _tmp66E->defn=({struct Cyc_Core_Opt*_tmp682=_cycalloc(
sizeof(*_tmp682));_tmp682->v=(void*)((void*)0);_tmp682;});goto _LL427;_LL427:;}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp683=_cycalloc(sizeof(*_tmp683));
_tmp683->hd=d;_tmp683->tl=Cyc_Toc_result_decls;_tmp683;});goto _LL40A;_LL41B: if((
int)_tmp668 != 0)goto _LL41D;_LL41C: goto _LL41E;_LL41D: if((int)_tmp668 != 1)goto
_LL41F;_LL41E: goto _LL40A;_LL41F: if(_tmp668 <= (void*)2  || *((int*)_tmp668)!= 8)
goto _LL421;_tmp66F=((struct Cyc_Absyn_Namespace_d_struct*)_tmp668)->f2;_LL420:
_tmp670=_tmp66F;goto _LL422;_LL421: if(_tmp668 <= (void*)2  || *((int*)_tmp668)!= 9)
goto _LL423;_tmp670=((struct Cyc_Absyn_Using_d_struct*)_tmp668)->f2;_LL422: _tmp671=
_tmp670;goto _LL424;_LL423: if(_tmp668 <= (void*)2  || *((int*)_tmp668)!= 10)goto
_LL425;_tmp671=((struct Cyc_Absyn_ExternC_d_struct*)_tmp668)->f1;_LL424: nv=Cyc_Toc_decls_to_c(
nv,_tmp671,top,cinclude);goto _LL40A;_LL425: if(_tmp668 <= (void*)2  || *((int*)
_tmp668)!= 11)goto _LL40A;_tmp672=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp668)->f1;_LL426: nv=Cyc_Toc_decls_to_c(nv,_tmp672,top,1);goto _LL40A;_LL40A:;}}
return nv;}static void Cyc_Toc_init(){Cyc_Toc_result_decls=0;Cyc_Toc_tuple_types=0;
Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_aggrs_so_far=0;Cyc_Toc_tunions_so_far=0;Cyc_Toc_xtunion_fields_so_far=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();Cyc_Toc_decls_to_c(Cyc_Toc_empty_env(),
ds,1,0);return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}
