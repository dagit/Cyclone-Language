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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _tagged_arr,struct
_tagged_arr);int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};int Cyc_file_string_write(struct Cyc___cycFILE*,
struct _tagged_arr src,int src_offset,int max_count);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);unsigned int Cyc_strlen(
struct _tagged_arr s);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct
_tagged_arr Cyc_Position_get_line_directive(struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};
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
struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct _tuple0*
name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct Cyc_Absyn_UnknownTunionInfo
f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};
struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*targs;void*rgn;}
;struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*tunion_name;struct _tuple0*
field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int tag;
struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct _tagged_arr Cyc_Absyn_attribute2string(void*);struct _tuple3{void*f1;struct
_tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(void*);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);int Cyc_Absynpp_is_anon_aggrtype(void*t);extern
struct _tagged_arr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _tagged_arr Cyc_Absynpp_char_escape(char);struct _tagged_arr Cyc_Absynpp_string_escape(
struct _tagged_arr);struct _tagged_arr Cyc_Absynpp_prim2str(void*p);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);struct _tuple4{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple4*arg);struct
_tuple5{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple5
Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t);struct _tuple6{unsigned int f1;
int f2;};struct _tuple6 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct
Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct
Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct _tuple7{void*
f1;void*f2;};struct _tuple7*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d);struct _tuple7*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
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
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_impos(struct
_tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);static int
Cyc_Absyndump_expand_typedefs;static int Cyc_Absyndump_qvar_to_Cids;static int Cyc_Absyndump_add_cyc_prefix;
static int Cyc_Absyndump_generate_line_directives;static int Cyc_Absyndump_to_VC;
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs){Cyc_Absyndump_expand_typedefs=
fs->expand_typedefs;Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absyndump_add_cyc_prefix=
fs->add_cyc_prefix;Cyc_Absyndump_to_VC=fs->to_VC;Cyc_Absyndump_generate_line_directives=
fs->generate_line_directives;Cyc_Absynpp_set_params(fs);}void Cyc_Absyndump_dumptyp(
void*);void Cyc_Absyndump_dumpntyp(void*t);void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);
void Cyc_Absyndump_dumpexp_prec(int,struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumppat(
struct Cyc_Absyn_Pat*);void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*);void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*);void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(
void*),void*a);void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(
void*),void*);void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);void
Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);void Cyc_Absyndump_dumploc(
struct Cyc_Position_Segment*);struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
void Cyc_Absyndump_ignore(void*x){return;}static unsigned int Cyc_Absyndump_pos=0;
static char Cyc_Absyndump_prev_char='x';int Cyc_Absyndump_need_space_before(){
switch(Cyc_Absyndump_prev_char){case '{': _LL0: goto _LL1;case '}': _LL1: goto _LL2;case
'(': _LL2: goto _LL3;case ')': _LL3: goto _LL4;case '[': _LL4: goto _LL5;case ']': _LL5: goto
_LL6;case ';': _LL6: goto _LL7;case ',': _LL7: goto _LL8;case '=': _LL8: goto _LL9;case '?':
_LL9: goto _LLA;case '!': _LLA: goto _LLB;case ' ': _LLB: goto _LLC;case '\n': _LLC: goto _LLD;
case '*': _LLD: return 0;default: _LLE: return 1;}}void Cyc_Absyndump_dump(struct
_tagged_arr s){int sz=(int)Cyc_strlen((struct _tagged_arr)s);Cyc_Absyndump_pos +=sz;
if(Cyc_Absyndump_pos > 80){Cyc_Absyndump_pos=(unsigned int)sz;Cyc_fputc((int)'\n',*
Cyc_Absyndump_dump_file);}else{if(Cyc_Absyndump_need_space_before())Cyc_fputc((
int)' ',*Cyc_Absyndump_dump_file);}if(sz >= 1){Cyc_Absyndump_prev_char=*((const
char*)_check_unknown_subscript(s,sizeof(char),sz - 1));Cyc_file_string_write(*Cyc_Absyndump_dump_file,
s,0,sz);}}void Cyc_Absyndump_dump_nospace(struct _tagged_arr s){int sz=(int)Cyc_strlen((
struct _tagged_arr)s);Cyc_Absyndump_pos +=sz;if(sz >= 1){Cyc_file_string_write(*Cyc_Absyndump_dump_file,
s,0,sz);Cyc_Absyndump_prev_char=*((const char*)_check_unknown_subscript(s,sizeof(
char),sz - 1));}}void Cyc_Absyndump_dump_char(int c){++ Cyc_Absyndump_pos;Cyc_fputc(
c,*Cyc_Absyndump_dump_file);Cyc_Absyndump_prev_char=(char)c;}void Cyc_Absyndump_dumploc(
struct Cyc_Position_Segment*loc){if(loc == 0)return;if(!Cyc_Absyndump_generate_line_directives)
return;{struct _tagged_arr _tmp0=Cyc_Position_get_line_directive(loc);Cyc_Absyndump_dump(
_tmp0);}}void Cyc_Absyndump_dump_str(struct _tagged_arr*s){Cyc_Absyndump_dump(*s);}
void Cyc_Absyndump_dump_semi(){Cyc_Absyndump_dump_char((int)';');}void Cyc_Absyndump_dump_sep(
void(*f)(void*),struct Cyc_List_List*l,struct _tagged_arr sep){if(l == 0)return;for(
0;l->tl != 0;l=l->tl){f((void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f((void*)l->hd);}
void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*
l,struct _tagged_arr sep){if(l == 0)return;for(0;l->tl != 0;l=l->tl){f(env,(void*)l->hd);
Cyc_Absyndump_dump_nospace(sep);}f(env,(void*)l->hd);}void Cyc_Absyndump_group(
void(*f)(void*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr
end,struct _tagged_arr sep){Cyc_Absyndump_dump_nospace(start);Cyc_Absyndump_dump_sep(
f,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_group_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep){Cyc_Absyndump_dump_nospace(start);Cyc_Absyndump_dump_sep_c(
f,env,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_egroup(void(*f)(
void*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep){if(l != 0)Cyc_Absyndump_group(f,l,start,end,sep);}void Cyc_Absyndump_dumpqvar(
struct _tuple0*v){struct Cyc_List_List*_tmp1=0;{void*_tmp2=(*v).f1;struct Cyc_List_List*
_tmp3;struct Cyc_List_List*_tmp4;_LL11: if((int)_tmp2 != 0)goto _LL13;_LL12: _tmp3=0;
goto _LL14;_LL13: if(_tmp2 <= (void*)1  || *((int*)_tmp2)!= 0)goto _LL15;_tmp3=((
struct Cyc_Absyn_Rel_n_struct*)_tmp2)->f1;_LL14: _tmp1=_tmp3;goto _LL10;_LL15: if(
_tmp2 <= (void*)1  || *((int*)_tmp2)!= 1)goto _LL10;_tmp4=((struct Cyc_Absyn_Abs_n_struct*)
_tmp2)->f1;_LL16: _tmp1=Cyc_Absyndump_qvar_to_Cids  && Cyc_Absyndump_add_cyc_prefix?({
struct Cyc_List_List*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->hd=Cyc_Absynpp_cyc_stringptr;
_tmp5->tl=_tmp4;_tmp5;}): _tmp4;goto _LL10;_LL10:;}if(_tmp1 != 0){Cyc_Absyndump_dump_str((
struct _tagged_arr*)_tmp1->hd);for(_tmp1=_tmp1->tl;_tmp1 != 0;_tmp1=_tmp1->tl){if(
Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{Cyc_Absyndump_dump_nospace(({
const char*_tmp6="::";_tag_arr(_tmp6,sizeof(char),_get_zero_arr_size(_tmp6,3));}));}
Cyc_Absyndump_dump_nospace(*((struct _tagged_arr*)_tmp1->hd));}if(Cyc_Absyndump_qvar_to_Cids)
Cyc_Absyndump_dump_nospace(({const char*_tmp7="_";_tag_arr(_tmp7,sizeof(char),
_get_zero_arr_size(_tmp7,2));}));else{Cyc_Absyndump_dump_nospace(({const char*
_tmp8="::";_tag_arr(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,3));}));}Cyc_Absyndump_dump_nospace(*(*
v).f2);}else{Cyc_Absyndump_dump_str((*v).f2);}}void Cyc_Absyndump_dumptq(struct
Cyc_Absyn_Tqual tq){if(tq.q_restrict)Cyc_Absyndump_dump(({const char*_tmp9="restrict";
_tag_arr(_tmp9,sizeof(char),_get_zero_arr_size(_tmp9,9));}));if(tq.q_volatile)
Cyc_Absyndump_dump(({const char*_tmpA="volatile";_tag_arr(_tmpA,sizeof(char),
_get_zero_arr_size(_tmpA,9));}));if(tq.print_const)Cyc_Absyndump_dump(({const
char*_tmpB="const";_tag_arr(_tmpB,sizeof(char),_get_zero_arr_size(_tmpB,6));}));}
void Cyc_Absyndump_dumpscope(void*sc){void*_tmpC=sc;_LL18: if((int)_tmpC != 0)goto
_LL1A;_LL19: Cyc_Absyndump_dump(({const char*_tmpD="static";_tag_arr(_tmpD,sizeof(
char),_get_zero_arr_size(_tmpD,7));}));return;_LL1A: if((int)_tmpC != 2)goto _LL1C;
_LL1B: return;_LL1C: if((int)_tmpC != 3)goto _LL1E;_LL1D: Cyc_Absyndump_dump(({const
char*_tmpE="extern";_tag_arr(_tmpE,sizeof(char),_get_zero_arr_size(_tmpE,7));}));
return;_LL1E: if((int)_tmpC != 4)goto _LL20;_LL1F: Cyc_Absyndump_dump(({const char*
_tmpF="extern \"C\"";_tag_arr(_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,11));}));
return;_LL20: if((int)_tmpC != 1)goto _LL22;_LL21: Cyc_Absyndump_dump(({const char*
_tmp10="abstract";_tag_arr(_tmp10,sizeof(char),_get_zero_arr_size(_tmp10,9));}));
return;_LL22: if((int)_tmpC != 5)goto _LL17;_LL23: Cyc_Absyndump_dump(({const char*
_tmp11="register";_tag_arr(_tmp11,sizeof(char),_get_zero_arr_size(_tmp11,9));}));
return;_LL17:;}void Cyc_Absyndump_dumpkind(void*k){void*_tmp12=k;_LL25: if((int)
_tmp12 != 0)goto _LL27;_LL26: Cyc_Absyndump_dump(({const char*_tmp13="A";_tag_arr(
_tmp13,sizeof(char),_get_zero_arr_size(_tmp13,2));}));return;_LL27: if((int)
_tmp12 != 1)goto _LL29;_LL28: Cyc_Absyndump_dump(({const char*_tmp14="M";_tag_arr(
_tmp14,sizeof(char),_get_zero_arr_size(_tmp14,2));}));return;_LL29: if((int)
_tmp12 != 2)goto _LL2B;_LL2A: Cyc_Absyndump_dump(({const char*_tmp15="B";_tag_arr(
_tmp15,sizeof(char),_get_zero_arr_size(_tmp15,2));}));return;_LL2B: if((int)
_tmp12 != 3)goto _LL2D;_LL2C: Cyc_Absyndump_dump(({const char*_tmp16="R";_tag_arr(
_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,2));}));return;_LL2D: if((int)
_tmp12 != 4)goto _LL2F;_LL2E: Cyc_Absyndump_dump(({const char*_tmp17="E";_tag_arr(
_tmp17,sizeof(char),_get_zero_arr_size(_tmp17,2));}));return;_LL2F: if((int)
_tmp12 != 5)goto _LL24;_LL30: Cyc_Absyndump_dump(({const char*_tmp18="I";_tag_arr(
_tmp18,sizeof(char),_get_zero_arr_size(_tmp18,2));}));return;_LL24:;}void Cyc_Absyndump_dumpaggr_kind(
void*k){void*_tmp19=k;_LL32: if((int)_tmp19 != 0)goto _LL34;_LL33: Cyc_Absyndump_dump(({
const char*_tmp1A="struct";_tag_arr(_tmp1A,sizeof(char),_get_zero_arr_size(_tmp1A,
7));}));return;_LL34: if((int)_tmp19 != 1)goto _LL31;_LL35: Cyc_Absyndump_dump(({
const char*_tmp1B="union";_tag_arr(_tmp1B,sizeof(char),_get_zero_arr_size(_tmp1B,
6));}));return;_LL31:;}void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){Cyc_Absyndump_egroup(
Cyc_Absyndump_dumptyp,ts,({const char*_tmp1C="<";_tag_arr(_tmp1C,sizeof(char),
_get_zero_arr_size(_tmp1C,2));}),({const char*_tmp1D=">";_tag_arr(_tmp1D,sizeof(
char),_get_zero_arr_size(_tmp1D,2));}),({const char*_tmp1E=",";_tag_arr(_tmp1E,
sizeof(char),_get_zero_arr_size(_tmp1E,2));}));}void Cyc_Absyndump_dumptvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(tv->name);}void Cyc_Absyndump_dumpkindedtvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(tv->name);{void*_tmp1F=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmp20;void*_tmp21;_LL37: if(*((int*)_tmp1F)!= 0)goto _LL39;
_tmp20=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp1F)->f1;if((int)_tmp20 != 2)
goto _LL39;_LL38: goto _LL3A;_LL39: if(*((int*)_tmp1F)!= 2)goto _LL3B;_LL3A: goto _LL3C;
_LL3B: if(*((int*)_tmp1F)!= 1)goto _LL3D;_LL3C: Cyc_Absyndump_dump(({const char*
_tmp22="::?";_tag_arr(_tmp22,sizeof(char),_get_zero_arr_size(_tmp22,4));}));goto
_LL36;_LL3D: if(*((int*)_tmp1F)!= 0)goto _LL36;_tmp21=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp1F)->f1;_LL3E: Cyc_Absyndump_dump(({const char*_tmp23="::";_tag_arr(_tmp23,
sizeof(char),_get_zero_arr_size(_tmp23,3));}));Cyc_Absyndump_dumpkind(_tmp21);
goto _LL36;_LL36:;}}void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){((void(*)(
void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
tvs,({const char*_tmp24="<";_tag_arr(_tmp24,sizeof(char),_get_zero_arr_size(
_tmp24,2));}),({const char*_tmp25=">";_tag_arr(_tmp25,sizeof(char),
_get_zero_arr_size(_tmp25,2));}),({const char*_tmp26=",";_tag_arr(_tmp26,sizeof(
char),_get_zero_arr_size(_tmp26,2));}));}void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar,tvs,({const char*_tmp27="<";_tag_arr(_tmp27,sizeof(
char),_get_zero_arr_size(_tmp27,2));}),({const char*_tmp28=">";_tag_arr(_tmp28,
sizeof(char),_get_zero_arr_size(_tmp28,2));}),({const char*_tmp29=",";_tag_arr(
_tmp29,sizeof(char),_get_zero_arr_size(_tmp29,2));}));}struct _tuple8{struct Cyc_Absyn_Tqual
f1;void*f2;};void Cyc_Absyndump_dumparg(struct _tuple8*pr){((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))
Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){((
void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,
ts,({const char*_tmp2A="(";_tag_arr(_tmp2A,sizeof(char),_get_zero_arr_size(_tmp2A,
2));}),({const char*_tmp2B=")";_tag_arr(_tmp2B,sizeof(char),_get_zero_arr_size(
_tmp2B,2));}),({const char*_tmp2C=",";_tag_arr(_tmp2C,sizeof(char),
_get_zero_arr_size(_tmp2C,2));}));}void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*
atts){for(0;atts != 0;atts=atts->tl){void*_tmp2D=(void*)atts->hd;_LL40: if((int)
_tmp2D != 0)goto _LL42;_LL41: Cyc_Absyndump_dump(({const char*_tmp2E="_stdcall";
_tag_arr(_tmp2E,sizeof(char),_get_zero_arr_size(_tmp2E,9));}));return;_LL42: if((
int)_tmp2D != 1)goto _LL44;_LL43: Cyc_Absyndump_dump(({const char*_tmp2F="_cdecl";
_tag_arr(_tmp2F,sizeof(char),_get_zero_arr_size(_tmp2F,7));}));return;_LL44: if((
int)_tmp2D != 2)goto _LL46;_LL45: Cyc_Absyndump_dump(({const char*_tmp30="_fastcall";
_tag_arr(_tmp30,sizeof(char),_get_zero_arr_size(_tmp30,10));}));return;_LL46:;
_LL47: goto _LL3F;_LL3F:;}}void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*
atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp31=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;_LL49: if((int)
_tmp31 != 0)goto _LL4B;_LL4A: goto _LL4C;_LL4B: if((int)_tmp31 != 1)goto _LL4D;_LL4C:
goto _LL4E;_LL4D: if((int)_tmp31 != 2)goto _LL4F;_LL4E: goto _LL48;_LL4F:;_LL50: hasatt=
1;goto _LL48;_LL48:;}}if(!hasatt)return;Cyc_Absyndump_dump(({const char*_tmp32="__declspec(";
_tag_arr(_tmp32,sizeof(char),_get_zero_arr_size(_tmp32,12));}));for(0;atts != 0;
atts=atts->tl){void*_tmp33=(void*)atts->hd;_LL52: if((int)_tmp33 != 0)goto _LL54;
_LL53: goto _LL55;_LL54: if((int)_tmp33 != 1)goto _LL56;_LL55: goto _LL57;_LL56: if((int)
_tmp33 != 2)goto _LL58;_LL57: goto _LL51;_LL58:;_LL59: Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((
void*)atts->hd));goto _LL51;_LL51:;}Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpatts(
struct Cyc_List_List*atts){if(atts == 0)return;if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dump_noncallconv(
atts);return;}Cyc_Absyndump_dump(({const char*_tmp34=" __attribute__((";_tag_arr(
_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,17));}));for(0;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));if(atts->tl != 0)
Cyc_Absyndump_dump(({const char*_tmp35=",";_tag_arr(_tmp35,sizeof(char),
_get_zero_arr_size(_tmp35,2));}));}Cyc_Absyndump_dump(({const char*_tmp36=")) ";
_tag_arr(_tmp36,sizeof(char),_get_zero_arr_size(_tmp36,4));}));}int Cyc_Absyndump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmp37=(void*)tms->hd;_LL5B:
if(*((int*)_tmp37)!= 2)goto _LL5D;_LL5C: return 1;_LL5D:;_LL5E: return 0;_LL5A:;}}
static void Cyc_Absyndump_dumprgn(void*t){void*_tmp38=Cyc_Tcutil_compress(t);_LL60:
if((int)_tmp38 != 2)goto _LL62;_LL61: Cyc_Absyndump_dump(({const char*_tmp39="`H";
_tag_arr(_tmp39,sizeof(char),_get_zero_arr_size(_tmp39,3));}));goto _LL5F;_LL62:;
_LL63: Cyc_Absyndump_dumpntyp(t);goto _LL5F;_LL5F:;}struct _tuple9{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};static struct _tuple9 Cyc_Absyndump_effects_split(void*
t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;{void*_tmp3A=Cyc_Tcutil_compress(
t);void*_tmp3B;struct Cyc_List_List*_tmp3C;_LL65: if(_tmp3A <= (void*)4  || *((int*)
_tmp3A)!= 19)goto _LL67;_tmp3B=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp3A)->f1;
_LL66: rgions=({struct Cyc_List_List*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->hd=(
void*)_tmp3B;_tmp3D->tl=rgions;_tmp3D;});goto _LL64;_LL67: if(_tmp3A <= (void*)4
 || *((int*)_tmp3A)!= 20)goto _LL69;_tmp3C=((struct Cyc_Absyn_JoinEff_struct*)
_tmp3A)->f1;_LL68: for(0;_tmp3C != 0;_tmp3C=_tmp3C->tl){struct Cyc_List_List*_tmp3F;
struct Cyc_List_List*_tmp40;struct _tuple9 _tmp3E=Cyc_Absyndump_effects_split((void*)
_tmp3C->hd);_tmp3F=_tmp3E.f1;_tmp40=_tmp3E.f2;rgions=Cyc_List_imp_append(_tmp3F,
rgions);effects=Cyc_List_imp_append(_tmp40,effects);}goto _LL64;_LL69:;_LL6A:
effects=({struct Cyc_List_List*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->hd=(void*)
t;_tmp41->tl=effects;_tmp41;});goto _LL64;_LL64:;}return({struct _tuple9 _tmp42;
_tmp42.f1=rgions;_tmp42.f2=effects;_tmp42;});}static void Cyc_Absyndump_dumpeff(
void*t){struct Cyc_List_List*_tmp44;struct Cyc_List_List*_tmp45;struct _tuple9
_tmp43=Cyc_Absyndump_effects_split(t);_tmp44=_tmp43.f1;_tmp45=_tmp43.f2;_tmp44=
Cyc_List_imp_rev(_tmp44);_tmp45=Cyc_List_imp_rev(_tmp45);for(0;_tmp45 != 0;_tmp45=
_tmp45->tl){Cyc_Absyndump_dumpntyp((void*)_tmp45->hd);Cyc_Absyndump_dump_char((
int)'+');}Cyc_Absyndump_dump_char((int)'{');for(0;_tmp44 != 0;_tmp44=_tmp44->tl){
Cyc_Absyndump_dumprgn((void*)_tmp44->hd);if(_tmp44->tl != 0)Cyc_Absyndump_dump_char((
int)',');}Cyc_Absyndump_dump_char((int)'}');}void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp46=t;struct Cyc_Absyn_Tvar*_tmp47;struct Cyc_Core_Opt*_tmp48;struct Cyc_Core_Opt*
_tmp49;int _tmp4A;struct Cyc_Core_Opt*_tmp4B;struct Cyc_Core_Opt*_tmp4C;struct Cyc_Core_Opt
_tmp4D;void*_tmp4E;int _tmp4F;struct Cyc_Absyn_TunionInfo _tmp50;void*_tmp51;struct
Cyc_List_List*_tmp52;void*_tmp53;struct Cyc_Absyn_TunionFieldInfo _tmp54;void*
_tmp55;struct Cyc_List_List*_tmp56;void*_tmp57;void*_tmp58;void*_tmp59;void*
_tmp5A;void*_tmp5B;void*_tmp5C;void*_tmp5D;void*_tmp5E;void*_tmp5F;void*_tmp60;
void*_tmp61;void*_tmp62;void*_tmp63;void*_tmp64;void*_tmp65;void*_tmp66;void*
_tmp67;void*_tmp68;void*_tmp69;void*_tmp6A;void*_tmp6B;void*_tmp6C;void*_tmp6D;
void*_tmp6E;void*_tmp6F;void*_tmp70;void*_tmp71;void*_tmp72;void*_tmp73;void*
_tmp74;int _tmp75;struct Cyc_List_List*_tmp76;struct Cyc_Absyn_AggrInfo _tmp77;void*
_tmp78;struct Cyc_List_List*_tmp79;void*_tmp7A;struct Cyc_List_List*_tmp7B;struct
_tuple0*_tmp7C;struct Cyc_List_List*_tmp7D;struct _tuple0*_tmp7E;struct Cyc_List_List*
_tmp7F;void*_tmp80;void*_tmp81;int _tmp82;void*_tmp83;_LL6C: if(_tmp46 <= (void*)4
 || *((int*)_tmp46)!= 7)goto _LL6E;_LL6D: goto _LL6F;_LL6E: if(_tmp46 <= (void*)4  || *((
int*)_tmp46)!= 8)goto _LL70;_LL6F: goto _LL71;_LL70: if(_tmp46 <= (void*)4  || *((int*)
_tmp46)!= 4)goto _LL72;_LL71: return;_LL72: if((int)_tmp46 != 0)goto _LL74;_LL73: Cyc_Absyndump_dump(({
const char*_tmp84="void";_tag_arr(_tmp84,sizeof(char),_get_zero_arr_size(_tmp84,5));}));
return;_LL74: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 1)goto _LL76;_tmp47=((
struct Cyc_Absyn_VarType_struct*)_tmp46)->f1;_LL75: Cyc_Absyndump_dump_str(_tmp47->name);
return;_LL76: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 0)goto _LL78;_tmp48=((
struct Cyc_Absyn_Evar_struct*)_tmp46)->f1;_tmp49=((struct Cyc_Absyn_Evar_struct*)
_tmp46)->f2;if(_tmp49 != 0)goto _LL78;_tmp4A=((struct Cyc_Absyn_Evar_struct*)_tmp46)->f3;
_LL77: Cyc_Absyndump_dump(({const char*_tmp85="%";_tag_arr(_tmp85,sizeof(char),
_get_zero_arr_size(_tmp85,2));}));if(_tmp48 == 0)Cyc_Absyndump_dump(({const char*
_tmp86="?";_tag_arr(_tmp86,sizeof(char),_get_zero_arr_size(_tmp86,2));}));else{
Cyc_Absyndump_dumpkind((void*)_tmp48->v);}Cyc_Absyndump_dump((struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp89;_tmp89.tag=1;_tmp89.f1=(unsigned int)_tmp4A;{void*
_tmp87[1]={& _tmp89};Cyc_aprintf(({const char*_tmp88="(%d)";_tag_arr(_tmp88,
sizeof(char),_get_zero_arr_size(_tmp88,5));}),_tag_arr(_tmp87,sizeof(void*),1));}}));
return;_LL78: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 0)goto _LL7A;_tmp4B=((
struct Cyc_Absyn_Evar_struct*)_tmp46)->f1;_tmp4C=((struct Cyc_Absyn_Evar_struct*)
_tmp46)->f2;if(_tmp4C == 0)goto _LL7A;_tmp4D=*_tmp4C;_tmp4E=(void*)_tmp4D.v;_tmp4F=((
struct Cyc_Absyn_Evar_struct*)_tmp46)->f3;_LL79: Cyc_Absyndump_dumpntyp(_tmp4E);
return;_LL7A: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 2)goto _LL7C;_tmp50=((
struct Cyc_Absyn_TunionType_struct*)_tmp46)->f1;_tmp51=(void*)_tmp50.tunion_info;
_tmp52=_tmp50.targs;_tmp53=(void*)_tmp50.rgn;_LL7B:{void*_tmp8A=_tmp51;struct Cyc_Absyn_UnknownTunionInfo
_tmp8B;struct _tuple0*_tmp8C;int _tmp8D;struct Cyc_Absyn_Tuniondecl**_tmp8E;struct
Cyc_Absyn_Tuniondecl*_tmp8F;struct Cyc_Absyn_Tuniondecl _tmp90;struct _tuple0*
_tmp91;int _tmp92;_LLBF: if(*((int*)_tmp8A)!= 0)goto _LLC1;_tmp8B=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp8A)->f1;_tmp8C=_tmp8B.name;_tmp8D=_tmp8B.is_xtunion;_LLC0: _tmp91=_tmp8C;
_tmp92=_tmp8D;goto _LLC2;_LLC1: if(*((int*)_tmp8A)!= 1)goto _LLBE;_tmp8E=((struct
Cyc_Absyn_KnownTunion_struct*)_tmp8A)->f1;_tmp8F=*_tmp8E;_tmp90=*_tmp8F;_tmp91=
_tmp90.name;_tmp92=_tmp90.is_xtunion;_LLC2: if(_tmp92)Cyc_Absyndump_dump(({const
char*_tmp93="xtunion ";_tag_arr(_tmp93,sizeof(char),_get_zero_arr_size(_tmp93,9));}));
else{Cyc_Absyndump_dump(({const char*_tmp94="tunion ";_tag_arr(_tmp94,sizeof(char),
_get_zero_arr_size(_tmp94,8));}));}{void*_tmp95=Cyc_Tcutil_compress(_tmp53);
_LLC4: if((int)_tmp95 != 2)goto _LLC6;_LLC5: goto _LLC3;_LLC6:;_LLC7: Cyc_Absyndump_dumptyp(
_tmp53);Cyc_Absyndump_dump(({const char*_tmp96=" ";_tag_arr(_tmp96,sizeof(char),
_get_zero_arr_size(_tmp96,2));}));goto _LLC3;_LLC3:;}Cyc_Absyndump_dumpqvar(
_tmp91);Cyc_Absyndump_dumptps(_tmp52);goto _LLBE;_LLBE:;}goto _LL6B;_LL7C: if(
_tmp46 <= (void*)4  || *((int*)_tmp46)!= 3)goto _LL7E;_tmp54=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp46)->f1;_tmp55=(void*)_tmp54.field_info;_tmp56=_tmp54.targs;_LL7D:{void*
_tmp97=_tmp55;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp98;struct _tuple0*_tmp99;
struct _tuple0*_tmp9A;int _tmp9B;struct Cyc_Absyn_Tuniondecl*_tmp9C;struct Cyc_Absyn_Tuniondecl
_tmp9D;struct _tuple0*_tmp9E;int _tmp9F;struct Cyc_Absyn_Tunionfield*_tmpA0;struct
Cyc_Absyn_Tunionfield _tmpA1;struct _tuple0*_tmpA2;_LLC9: if(*((int*)_tmp97)!= 0)
goto _LLCB;_tmp98=((struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp97)->f1;_tmp99=
_tmp98.tunion_name;_tmp9A=_tmp98.field_name;_tmp9B=_tmp98.is_xtunion;_LLCA:
_tmp9E=_tmp99;_tmp9F=_tmp9B;_tmpA2=_tmp9A;goto _LLCC;_LLCB: if(*((int*)_tmp97)!= 1)
goto _LLC8;_tmp9C=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp97)->f1;_tmp9D=*
_tmp9C;_tmp9E=_tmp9D.name;_tmp9F=_tmp9D.is_xtunion;_tmpA0=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp97)->f2;_tmpA1=*_tmpA0;_tmpA2=_tmpA1.name;_LLCC: if(_tmp9F)Cyc_Absyndump_dump(({
const char*_tmpA3="xtunion ";_tag_arr(_tmpA3,sizeof(char),_get_zero_arr_size(
_tmpA3,9));}));else{Cyc_Absyndump_dump(({const char*_tmpA4="tunion ";_tag_arr(
_tmpA4,sizeof(char),_get_zero_arr_size(_tmpA4,8));}));}Cyc_Absyndump_dumpqvar(
_tmp9E);Cyc_Absyndump_dump(({const char*_tmpA5=".";_tag_arr(_tmpA5,sizeof(char),
_get_zero_arr_size(_tmpA5,2));}));Cyc_Absyndump_dumpqvar(_tmpA2);Cyc_Absyndump_dumptps(
_tmp56);goto _LLC8;_LLC8:;}goto _LL6B;_LL7E: if(_tmp46 <= (void*)4  || *((int*)_tmp46)
!= 5)goto _LL80;_tmp57=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((
int)_tmp57 != 2)goto _LL80;_tmp58=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;
if((int)_tmp58 != 2)goto _LL80;_LL7F: goto _LL81;_LL80: if(_tmp46 <= (void*)4  || *((
int*)_tmp46)!= 5)goto _LL82;_tmp59=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp59 != 0)goto _LL82;_tmp5A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp5A != 2)goto _LL82;_LL81: Cyc_Absyndump_dump(({const char*
_tmpA6="int";_tag_arr(_tmpA6,sizeof(char),_get_zero_arr_size(_tmpA6,4));}));
return;_LL82: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 5)goto _LL84;_tmp5B=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp5B != 2)goto _LL84;_tmp5C=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)_tmp5C != 0)goto _LL84;
_LL83: Cyc_Absyndump_dump(({const char*_tmpA7="char";_tag_arr(_tmpA7,sizeof(char),
_get_zero_arr_size(_tmpA7,5));}));return;_LL84: if(_tmp46 <= (void*)4  || *((int*)
_tmp46)!= 5)goto _LL86;_tmp5D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;
if((int)_tmp5D != 0)goto _LL86;_tmp5E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp5E != 0)goto _LL86;_LL85: Cyc_Absyndump_dump(({const char*
_tmpA8="signed char";_tag_arr(_tmpA8,sizeof(char),_get_zero_arr_size(_tmpA8,12));}));
return;_LL86: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 5)goto _LL88;_tmp5F=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp5F != 1)goto _LL88;_tmp60=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)_tmp60 != 0)goto _LL88;
_LL87: Cyc_Absyndump_dump(({const char*_tmpA9="unsigned char";_tag_arr(_tmpA9,
sizeof(char),_get_zero_arr_size(_tmpA9,14));}));return;_LL88: if(_tmp46 <= (void*)
4  || *((int*)_tmp46)!= 5)goto _LL8A;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp61 != 2)goto _LL8A;_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp62 != 1)goto _LL8A;_LL89: goto _LL8B;_LL8A: if(_tmp46 <= (void*)
4  || *((int*)_tmp46)!= 5)goto _LL8C;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp63 != 0)goto _LL8C;_tmp64=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp64 != 1)goto _LL8C;_LL8B: Cyc_Absyndump_dump(({const char*
_tmpAA="short";_tag_arr(_tmpAA,sizeof(char),_get_zero_arr_size(_tmpAA,6));}));
return;_LL8C: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 5)goto _LL8E;_tmp65=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp65 != 1)goto _LL8E;_tmp66=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)_tmp66 != 1)goto _LL8E;
_LL8D: Cyc_Absyndump_dump(({const char*_tmpAB="unsigned short";_tag_arr(_tmpAB,
sizeof(char),_get_zero_arr_size(_tmpAB,15));}));return;_LL8E: if(_tmp46 <= (void*)
4  || *((int*)_tmp46)!= 5)goto _LL90;_tmp67=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp67 != 1)goto _LL90;_tmp68=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp68 != 2)goto _LL90;_LL8F: Cyc_Absyndump_dump(({const char*
_tmpAC="unsigned int";_tag_arr(_tmpAC,sizeof(char),_get_zero_arr_size(_tmpAC,13));}));
return;_LL90: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 5)goto _LL92;_tmp69=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp69 != 2)goto _LL92;_tmp6A=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)_tmp6A != 3)goto _LL92;
if(!Cyc_Absyndump_to_VC)goto _LL92;_LL91: goto _LL93;_LL92: if(_tmp46 <= (void*)4  || *((
int*)_tmp46)!= 5)goto _LL94;_tmp6B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp6B != 0)goto _LL94;_tmp6C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp6C != 3)goto _LL94;if(!Cyc_Absyndump_to_VC)goto _LL94;_LL93:
Cyc_Absyndump_dump(({const char*_tmpAD="__int64";_tag_arr(_tmpAD,sizeof(char),
_get_zero_arr_size(_tmpAD,8));}));return;_LL94: if(_tmp46 <= (void*)4  || *((int*)
_tmp46)!= 5)goto _LL96;_tmp6D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;
if((int)_tmp6D != 2)goto _LL96;_tmp6E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp6E != 3)goto _LL96;_LL95: goto _LL97;_LL96: if(_tmp46 <= (void*)
4  || *((int*)_tmp46)!= 5)goto _LL98;_tmp6F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp6F != 0)goto _LL98;_tmp70=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp70 != 3)goto _LL98;_LL97: Cyc_Absyndump_dump(({const char*
_tmpAE="long long";_tag_arr(_tmpAE,sizeof(char),_get_zero_arr_size(_tmpAE,10));}));
return;_LL98: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 5)goto _LL9A;_tmp71=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp71 != 1)goto _LL9A;_tmp72=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)_tmp72 != 3)goto _LL9A;
if(!Cyc_Absyndump_to_VC)goto _LL9A;_LL99: Cyc_Absyndump_dump(({const char*_tmpAF="unsigned __int64";
_tag_arr(_tmpAF,sizeof(char),_get_zero_arr_size(_tmpAF,17));}));return;_LL9A: if(
_tmp46 <= (void*)4  || *((int*)_tmp46)!= 5)goto _LL9C;_tmp73=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp73 != 1)goto _LL9C;_tmp74=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp74 != 3)goto _LL9C;_LL9B: Cyc_Absyndump_dump(({const char*
_tmpB0="unsigned long long";_tag_arr(_tmpB0,sizeof(char),_get_zero_arr_size(
_tmpB0,19));}));return;_LL9C: if((int)_tmp46 != 1)goto _LL9E;_LL9D: Cyc_Absyndump_dump(({
const char*_tmpB1="float";_tag_arr(_tmpB1,sizeof(char),_get_zero_arr_size(_tmpB1,
6));}));return;_LL9E: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 6)goto _LLA0;
_tmp75=((struct Cyc_Absyn_DoubleType_struct*)_tmp46)->f1;_LL9F: if(_tmp75)Cyc_Absyndump_dump(({
const char*_tmpB2="long double";_tag_arr(_tmpB2,sizeof(char),_get_zero_arr_size(
_tmpB2,12));}));else{Cyc_Absyndump_dump(({const char*_tmpB3="double";_tag_arr(
_tmpB3,sizeof(char),_get_zero_arr_size(_tmpB3,7));}));}return;_LLA0: if(_tmp46 <= (
void*)4  || *((int*)_tmp46)!= 9)goto _LLA2;_tmp76=((struct Cyc_Absyn_TupleType_struct*)
_tmp46)->f1;_LLA1: Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(
_tmp76);return;_LLA2: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 10)goto _LLA4;
_tmp77=((struct Cyc_Absyn_AggrType_struct*)_tmp46)->f1;_tmp78=(void*)_tmp77.aggr_info;
_tmp79=_tmp77.targs;_LLA3: {void*_tmpB5;struct _tuple0*_tmpB6;struct _tuple3 _tmpB4=
Cyc_Absyn_aggr_kinded_name(_tmp78);_tmpB5=_tmpB4.f1;_tmpB6=_tmpB4.f2;Cyc_Absyndump_dumpaggr_kind(
_tmpB5);Cyc_Absyndump_dumpqvar(_tmpB6);Cyc_Absyndump_dumptps(_tmp79);return;}
_LLA4: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 11)goto _LLA6;_tmp7A=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp46)->f1;_tmp7B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp46)->f2;_LLA5: Cyc_Absyndump_dumpaggr_kind(_tmp7A);Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpaggrfields(_tmp7B);Cyc_Absyndump_dump_char((int)'}');
return;_LLA6: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 12)goto _LLA8;_tmp7C=((
struct Cyc_Absyn_EnumType_struct*)_tmp46)->f1;_LLA7: Cyc_Absyndump_dump(({const
char*_tmpB7="enum ";_tag_arr(_tmpB7,sizeof(char),_get_zero_arr_size(_tmpB7,6));}));
Cyc_Absyndump_dumpqvar(_tmp7C);return;_LLA8: if(_tmp46 <= (void*)4  || *((int*)
_tmp46)!= 13)goto _LLAA;_tmp7D=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp46)->f1;
_LLA9: Cyc_Absyndump_dump(({const char*_tmpB8="enum {";_tag_arr(_tmpB8,sizeof(char),
_get_zero_arr_size(_tmpB8,7));}));Cyc_Absyndump_dumpenumfields(_tmp7D);Cyc_Absyndump_dump(({
const char*_tmpB9="}";_tag_arr(_tmpB9,sizeof(char),_get_zero_arr_size(_tmpB9,2));}));
return;_LLAA: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 16)goto _LLAC;_tmp7E=((
struct Cyc_Absyn_TypedefType_struct*)_tmp46)->f1;_tmp7F=((struct Cyc_Absyn_TypedefType_struct*)
_tmp46)->f2;_LLAB:(Cyc_Absyndump_dumpqvar(_tmp7E),Cyc_Absyndump_dumptps(_tmp7F));
return;_LLAC: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 14)goto _LLAE;_tmp80=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp46)->f1;_LLAD: Cyc_Absyndump_dump(({const
char*_tmpBA="sizeof_t<";_tag_arr(_tmpBA,sizeof(char),_get_zero_arr_size(_tmpBA,
10));}));Cyc_Absyndump_dumpntyp(_tmp80);Cyc_Absyndump_dump(({const char*_tmpBB=">";
_tag_arr(_tmpBB,sizeof(char),_get_zero_arr_size(_tmpBB,2));}));return;_LLAE: if(
_tmp46 <= (void*)4  || *((int*)_tmp46)!= 15)goto _LLB0;_tmp81=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp46)->f1;_LLAF: Cyc_Absyndump_dumprgn(_tmp81);return;_LLB0: if(_tmp46 <= (void*)
4  || *((int*)_tmp46)!= 18)goto _LLB2;_tmp82=((struct Cyc_Absyn_TypeInt_struct*)
_tmp46)->f1;_LLB1: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmpBE;_tmpBE.tag=1;_tmpBE.f1=(unsigned int)_tmp82;{void*_tmpBC[1]={& _tmpBE};Cyc_aprintf(({
const char*_tmpBD="`%d";_tag_arr(_tmpBD,sizeof(char),_get_zero_arr_size(_tmpBD,4));}),
_tag_arr(_tmpBC,sizeof(void*),1));}}));return;_LLB2: if(_tmp46 <= (void*)4  || *((
int*)_tmp46)!= 17)goto _LLB4;_tmp83=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp46)->f1;_LLB3: Cyc_Absyndump_dump(({const char*_tmpBF="tag_t<";_tag_arr(_tmpBF,
sizeof(char),_get_zero_arr_size(_tmpBF,7));}));Cyc_Absyndump_dumpntyp(_tmp83);
Cyc_Absyndump_dump(({const char*_tmpC0=">";_tag_arr(_tmpC0,sizeof(char),
_get_zero_arr_size(_tmpC0,2));}));return;_LLB4: if((int)_tmp46 != 3)goto _LLB6;
_LLB5: Cyc_Absyndump_dump(({const char*_tmpC1="`U";_tag_arr(_tmpC1,sizeof(char),
_get_zero_arr_size(_tmpC1,3));}));goto _LL6B;_LLB6: if((int)_tmp46 != 2)goto _LLB8;
_LLB7: goto _LLB9;_LLB8: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 19)goto _LLBA;
_LLB9: goto _LLBB;_LLBA: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 21)goto _LLBC;
_LLBB: goto _LLBD;_LLBC: if(_tmp46 <= (void*)4  || *((int*)_tmp46)!= 20)goto _LL6B;
_LLBD: return;_LL6B:;}void Cyc_Absyndump_dumpvaropt(struct Cyc_Core_Opt*vo){if(vo != 
0)Cyc_Absyndump_dump_str((struct _tagged_arr*)vo->v);}void Cyc_Absyndump_dumpfunarg(
struct _tuple1*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),
struct Cyc_Core_Opt*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*
t).f1);}void Cyc_Absyndump_dump_rgncmp(struct _tuple7*cmp){struct _tuple7 _tmpC3;
void*_tmpC4;void*_tmpC5;struct _tuple7*_tmpC2=cmp;_tmpC3=*_tmpC2;_tmpC4=_tmpC3.f1;
_tmpC5=_tmpC3.f2;Cyc_Absyndump_dumpeff(_tmpC4);Cyc_Absyndump_dump_char((int)'>');
Cyc_Absyndump_dumprgn(_tmpC5);}void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*
rgn_po){((void(*)(void(*f)(struct _tuple7*),struct Cyc_List_List*l,struct
_tagged_arr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,rgn_po,({const
char*_tmpC6=",";_tag_arr(_tmpC6,sizeof(char),_get_zero_arr_size(_tmpC6,2));}));}
void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((
int)'(');for(0;args != 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple1*)
args->hd);if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((
int)',');}if(c_varargs)Cyc_Absyndump_dump(({const char*_tmpC7="...";_tag_arr(
_tmpC7,sizeof(char),_get_zero_arr_size(_tmpC7,4));}));else{if(cyc_varargs != 0){
struct _tuple1*_tmpC8=({struct _tuple1*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->f1=
cyc_varargs->name;_tmpCB->f2=cyc_varargs->tq;_tmpCB->f3=(void*)cyc_varargs->type;
_tmpCB;});Cyc_Absyndump_dump(({const char*_tmpC9="...";_tag_arr(_tmpC9,sizeof(
char),_get_zero_arr_size(_tmpC9,4));}));if(cyc_varargs->inject)Cyc_Absyndump_dump(({
const char*_tmpCA=" inject ";_tag_arr(_tmpCA,sizeof(char),_get_zero_arr_size(
_tmpCA,9));}));Cyc_Absyndump_dumpfunarg(_tmpC8);}}if(effopt != 0){Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff((void*)effopt->v);}if(rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}Cyc_Absyndump_dump_char((int)')');}
void Cyc_Absyndump_dumptyp(void*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,(void(*)(int x))Cyc_Absyndump_ignore,
0);}void Cyc_Absyndump_dumpdesignator(void*d){void*_tmpCC=d;struct Cyc_Absyn_Exp*
_tmpCD;struct _tagged_arr*_tmpCE;_LLCE: if(*((int*)_tmpCC)!= 0)goto _LLD0;_tmpCD=((
struct Cyc_Absyn_ArrayElement_struct*)_tmpCC)->f1;_LLCF: Cyc_Absyndump_dump(({
const char*_tmpCF=".[";_tag_arr(_tmpCF,sizeof(char),_get_zero_arr_size(_tmpCF,3));}));
Cyc_Absyndump_dumpexp(_tmpCD);Cyc_Absyndump_dump_char((int)']');goto _LLCD;_LLD0:
if(*((int*)_tmpCC)!= 1)goto _LLCD;_tmpCE=((struct Cyc_Absyn_FieldName_struct*)
_tmpCC)->f1;_LLD1: Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmpCE);goto _LLCD;_LLCD:;}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};void Cyc_Absyndump_dumpde(struct _tuple10*de){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*
de).f1,({const char*_tmpD0="";_tag_arr(_tmpD0,sizeof(char),_get_zero_arr_size(
_tmpD0,1));}),({const char*_tmpD1="=";_tag_arr(_tmpD1,sizeof(char),
_get_zero_arr_size(_tmpD1,2));}),({const char*_tmpD2="=";_tag_arr(_tmpD2,sizeof(
char),_get_zero_arr_size(_tmpD2,2));}));Cyc_Absyndump_dumpexp((*de).f2);}void Cyc_Absyndump_dumpexps_prec(
int inprec,struct Cyc_List_List*es){((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),
int env,struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,inprec,es,({
const char*_tmpD3="";_tag_arr(_tmpD3,sizeof(char),_get_zero_arr_size(_tmpD3,1));}),({
const char*_tmpD4="";_tag_arr(_tmpD4,sizeof(char),_get_zero_arr_size(_tmpD4,1));}),({
const char*_tmpD5=",";_tag_arr(_tmpD5,sizeof(char),_get_zero_arr_size(_tmpD5,2));}));}
void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(
e);if(inprec >= myprec)Cyc_Absyndump_dump_nospace(({const char*_tmpD6="(";_tag_arr(
_tmpD6,sizeof(char),_get_zero_arr_size(_tmpD6,2));}));{void*_tmpD7=(void*)e->r;
void*_tmpD8;void*_tmpD9;char _tmpDA;void*_tmpDB;void*_tmpDC;short _tmpDD;void*
_tmpDE;void*_tmpDF;int _tmpE0;void*_tmpE1;void*_tmpE2;int _tmpE3;void*_tmpE4;void*
_tmpE5;int _tmpE6;void*_tmpE7;void*_tmpE8;long long _tmpE9;void*_tmpEA;struct
_tagged_arr _tmpEB;void*_tmpEC;void*_tmpED;struct _tagged_arr _tmpEE;struct _tuple0*
_tmpEF;struct _tuple0*_tmpF0;void*_tmpF1;struct Cyc_List_List*_tmpF2;struct Cyc_Absyn_Exp*
_tmpF3;struct Cyc_Core_Opt*_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*
_tmpF6;void*_tmpF7;struct Cyc_Absyn_Exp*_tmpF8;void*_tmpF9;struct Cyc_Absyn_Exp*
_tmpFA;void*_tmpFB;struct Cyc_Absyn_Exp*_tmpFC;void*_tmpFD;struct Cyc_Absyn_Exp*
_tmpFE;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*
_tmp101;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*
_tmp104;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*
_tmp107;struct Cyc_List_List*_tmp108;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_List_List*
_tmp10A;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*
_tmp10D;void*_tmp10E;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp110;
struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp112;void*_tmp113;struct Cyc_Absyn_Exp*
_tmp114;void*_tmp115;void*_tmp116;struct _tagged_arr*_tmp117;void*_tmp118;void*
_tmp119;unsigned int _tmp11A;struct Cyc_List_List*_tmp11B;void*_tmp11C;struct Cyc_Absyn_Exp*
_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;struct _tagged_arr*_tmp11F;struct Cyc_Absyn_Exp*
_tmp120;struct _tagged_arr*_tmp121;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*
_tmp123;struct Cyc_List_List*_tmp124;struct _tuple1*_tmp125;struct Cyc_List_List*
_tmp126;struct Cyc_List_List*_tmp127;struct Cyc_Absyn_Vardecl*_tmp128;struct Cyc_Absyn_Exp*
_tmp129;struct Cyc_Absyn_Exp*_tmp12A;struct _tuple0*_tmp12B;struct Cyc_List_List*
_tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_List_List*_tmp12E;struct Cyc_List_List*
_tmp12F;struct Cyc_Absyn_Tunionfield*_tmp130;struct _tuple0*_tmp131;struct _tuple0*
_tmp132;struct Cyc_Absyn_MallocInfo _tmp133;int _tmp134;struct Cyc_Absyn_Exp*_tmp135;
void**_tmp136;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Core_Opt*_tmp138;struct Cyc_List_List*
_tmp139;struct Cyc_Absyn_Stmt*_tmp13A;_LLD3: if(*((int*)_tmpD7)!= 0)goto _LLD5;
_tmpD8=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpD7)->f1;if(_tmpD8 <= (void*)1
 || *((int*)_tmpD8)!= 0)goto _LLD5;_tmpD9=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmpD8)->f1;_tmpDA=((struct Cyc_Absyn_Char_c_struct*)_tmpD8)->f2;_LLD4: Cyc_Absyndump_dump_char((
int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmpDA));Cyc_Absyndump_dump_char((
int)'\'');goto _LLD2;_LLD5: if(*((int*)_tmpD7)!= 0)goto _LLD7;_tmpDB=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmpD7)->f1;if(_tmpDB <= (void*)1  || *((int*)
_tmpDB)!= 1)goto _LLD7;_tmpDC=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmpDB)->f1;
_tmpDD=((struct Cyc_Absyn_Short_c_struct*)_tmpDB)->f2;_LLD6: Cyc_Absyndump_dump((
struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp13D;_tmp13D.tag=1;_tmp13D.f1=(
unsigned int)((int)_tmpDD);{void*_tmp13B[1]={& _tmp13D};Cyc_aprintf(({const char*
_tmp13C="%d";_tag_arr(_tmp13C,sizeof(char),_get_zero_arr_size(_tmp13C,3));}),
_tag_arr(_tmp13B,sizeof(void*),1));}}));goto _LLD2;_LLD7: if(*((int*)_tmpD7)!= 0)
goto _LLD9;_tmpDE=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpD7)->f1;if(_tmpDE
<= (void*)1  || *((int*)_tmpDE)!= 2)goto _LLD9;_tmpDF=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmpDE)->f1;if((int)_tmpDF != 2)goto _LLD9;_tmpE0=((struct Cyc_Absyn_Int_c_struct*)
_tmpDE)->f2;_LLD8: _tmpE3=_tmpE0;goto _LLDA;_LLD9: if(*((int*)_tmpD7)!= 0)goto _LLDB;
_tmpE1=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpD7)->f1;if(_tmpE1 <= (void*)1
 || *((int*)_tmpE1)!= 2)goto _LLDB;_tmpE2=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmpE1)->f1;if((int)_tmpE2 != 0)goto _LLDB;_tmpE3=((struct Cyc_Absyn_Int_c_struct*)
_tmpE1)->f2;_LLDA: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp140;_tmp140.tag=1;_tmp140.f1=(unsigned int)_tmpE3;{void*_tmp13E[1]={& _tmp140};
Cyc_aprintf(({const char*_tmp13F="%d";_tag_arr(_tmp13F,sizeof(char),
_get_zero_arr_size(_tmp13F,3));}),_tag_arr(_tmp13E,sizeof(void*),1));}}));goto
_LLD2;_LLDB: if(*((int*)_tmpD7)!= 0)goto _LLDD;_tmpE4=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpD7)->f1;if(_tmpE4 <= (void*)1  || *((int*)_tmpE4)!= 2)goto _LLDD;_tmpE5=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmpE4)->f1;if((int)_tmpE5 != 1)goto _LLDD;_tmpE6=((
struct Cyc_Absyn_Int_c_struct*)_tmpE4)->f2;_LLDC: Cyc_Absyndump_dump((struct
_tagged_arr)({struct Cyc_Int_pa_struct _tmp143;_tmp143.tag=1;_tmp143.f1=(
unsigned int)_tmpE6;{void*_tmp141[1]={& _tmp143};Cyc_aprintf(({const char*_tmp142="%u";
_tag_arr(_tmp142,sizeof(char),_get_zero_arr_size(_tmp142,3));}),_tag_arr(_tmp141,
sizeof(void*),1));}}));goto _LLD2;_LLDD: if(*((int*)_tmpD7)!= 0)goto _LLDF;_tmpE7=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmpD7)->f1;if(_tmpE7 <= (void*)1  || *((
int*)_tmpE7)!= 3)goto _LLDF;_tmpE8=(void*)((struct Cyc_Absyn_LongLong_c_struct*)
_tmpE7)->f1;_tmpE9=((struct Cyc_Absyn_LongLong_c_struct*)_tmpE7)->f2;_LLDE: Cyc_Absyndump_dump(({
const char*_tmp144="<<FIX LONG LONG CONSTANT>>";_tag_arr(_tmp144,sizeof(char),
_get_zero_arr_size(_tmp144,27));}));goto _LLD2;_LLDF: if(*((int*)_tmpD7)!= 0)goto
_LLE1;_tmpEA=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpD7)->f1;if(_tmpEA <= (
void*)1  || *((int*)_tmpEA)!= 4)goto _LLE1;_tmpEB=((struct Cyc_Absyn_Float_c_struct*)
_tmpEA)->f1;_LLE0: Cyc_Absyndump_dump(_tmpEB);goto _LLD2;_LLE1: if(*((int*)_tmpD7)
!= 0)goto _LLE3;_tmpEC=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpD7)->f1;if((
int)_tmpEC != 0)goto _LLE3;_LLE2: Cyc_Absyndump_dump(({const char*_tmp145="NULL";
_tag_arr(_tmp145,sizeof(char),_get_zero_arr_size(_tmp145,5));}));goto _LLD2;_LLE3:
if(*((int*)_tmpD7)!= 0)goto _LLE5;_tmpED=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpD7)->f1;if(_tmpED <= (void*)1  || *((int*)_tmpED)!= 5)goto _LLE5;_tmpEE=((
struct Cyc_Absyn_String_c_struct*)_tmpED)->f1;_LLE4: Cyc_Absyndump_dump_char((int)'"');
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmpEE));Cyc_Absyndump_dump_char((
int)'"');goto _LLD2;_LLE5: if(*((int*)_tmpD7)!= 2)goto _LLE7;_tmpEF=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmpD7)->f1;_LLE6: _tmpF0=_tmpEF;goto _LLE8;_LLE7: if(*((int*)_tmpD7)!= 1)goto _LLE9;
_tmpF0=((struct Cyc_Absyn_Var_e_struct*)_tmpD7)->f1;_LLE8: Cyc_Absyndump_dumpqvar(
_tmpF0);goto _LLD2;_LLE9: if(*((int*)_tmpD7)!= 3)goto _LLEB;_tmpF1=(void*)((struct
Cyc_Absyn_Primop_e_struct*)_tmpD7)->f1;_tmpF2=((struct Cyc_Absyn_Primop_e_struct*)
_tmpD7)->f2;_LLEA: {struct _tagged_arr _tmp146=Cyc_Absynpp_prim2str(_tmpF1);
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF2)){case 1: _LL137:
if(_tmpF1 == (void*)((void*)19)){Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpF2))->hd);Cyc_Absyndump_dump(({const char*
_tmp147=".size";_tag_arr(_tmp147,sizeof(char),_get_zero_arr_size(_tmp147,6));}));}
else{Cyc_Absyndump_dump(_tmp146);Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpF2))->hd);}break;case 2: _LL138: Cyc_Absyndump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpF2))->hd);
Cyc_Absyndump_dump(_tmp146);Cyc_Absyndump_dump_char((int)' ');Cyc_Absyndump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpF2->tl))->hd);
break;default: _LL139:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp148=
_cycalloc(sizeof(*_tmp148));_tmp148[0]=({struct Cyc_Core_Failure_struct _tmp149;
_tmp149.tag=Cyc_Core_Failure;_tmp149.f1=({const char*_tmp14A="Absyndump -- Bad number of arguments to primop";
_tag_arr(_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,47));});_tmp149;});
_tmp148;}));}goto _LLD2;}_LLEB: if(*((int*)_tmpD7)!= 4)goto _LLED;_tmpF3=((struct
Cyc_Absyn_AssignOp_e_struct*)_tmpD7)->f1;_tmpF4=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpD7)->f2;_tmpF5=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpD7)->f3;_LLEC: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpF3);if(_tmpF4 != 0)Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((void*)
_tmpF4->v));Cyc_Absyndump_dump_nospace(({const char*_tmp14B="=";_tag_arr(_tmp14B,
sizeof(char),_get_zero_arr_size(_tmp14B,2));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmpF5);goto _LLD2;_LLED: if(*((int*)_tmpD7)!= 5)goto _LLEF;_tmpF6=((struct
Cyc_Absyn_Increment_e_struct*)_tmpD7)->f1;_tmpF7=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmpD7)->f2;if((int)_tmpF7 != 0)goto _LLEF;_LLEE: Cyc_Absyndump_dump(({const char*
_tmp14C="++";_tag_arr(_tmp14C,sizeof(char),_get_zero_arr_size(_tmp14C,3));}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmpF6);goto _LLD2;_LLEF: if(*((int*)_tmpD7)!= 5)
goto _LLF1;_tmpF8=((struct Cyc_Absyn_Increment_e_struct*)_tmpD7)->f1;_tmpF9=(void*)((
struct Cyc_Absyn_Increment_e_struct*)_tmpD7)->f2;if((int)_tmpF9 != 2)goto _LLF1;
_LLF0: Cyc_Absyndump_dump(({const char*_tmp14D="--";_tag_arr(_tmp14D,sizeof(char),
_get_zero_arr_size(_tmp14D,3));}));Cyc_Absyndump_dumpexp_prec(myprec,_tmpF8);
goto _LLD2;_LLF1: if(*((int*)_tmpD7)!= 5)goto _LLF3;_tmpFA=((struct Cyc_Absyn_Increment_e_struct*)
_tmpD7)->f1;_tmpFB=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpD7)->f2;if((
int)_tmpFB != 1)goto _LLF3;_LLF2: Cyc_Absyndump_dumpexp_prec(myprec,_tmpFA);Cyc_Absyndump_dump(({
const char*_tmp14E="++";_tag_arr(_tmp14E,sizeof(char),_get_zero_arr_size(_tmp14E,
3));}));goto _LLD2;_LLF3: if(*((int*)_tmpD7)!= 5)goto _LLF5;_tmpFC=((struct Cyc_Absyn_Increment_e_struct*)
_tmpD7)->f1;_tmpFD=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpD7)->f2;if((
int)_tmpFD != 3)goto _LLF5;_LLF4: Cyc_Absyndump_dumpexp_prec(myprec,_tmpFC);Cyc_Absyndump_dump(({
const char*_tmp14F="--";_tag_arr(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,
3));}));goto _LLD2;_LLF5: if(*((int*)_tmpD7)!= 6)goto _LLF7;_tmpFE=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpD7)->f1;_tmpFF=((struct Cyc_Absyn_Conditional_e_struct*)_tmpD7)->f2;_tmp100=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpD7)->f3;_LLF6: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpFE);Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,
_tmpFF);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,
_tmp100);goto _LLD2;_LLF7: if(*((int*)_tmpD7)!= 7)goto _LLF9;_tmp101=((struct Cyc_Absyn_And_e_struct*)
_tmpD7)->f1;_tmp102=((struct Cyc_Absyn_And_e_struct*)_tmpD7)->f2;_LLF8: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp101);Cyc_Absyndump_dump(({const char*_tmp150=" && ";_tag_arr(_tmp150,
sizeof(char),_get_zero_arr_size(_tmp150,5));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp102);goto _LLD2;_LLF9: if(*((int*)_tmpD7)!= 8)goto _LLFB;_tmp103=((
struct Cyc_Absyn_Or_e_struct*)_tmpD7)->f1;_tmp104=((struct Cyc_Absyn_Or_e_struct*)
_tmpD7)->f2;_LLFA: Cyc_Absyndump_dumpexp_prec(myprec,_tmp103);Cyc_Absyndump_dump(({
const char*_tmp151=" || ";_tag_arr(_tmp151,sizeof(char),_get_zero_arr_size(
_tmp151,5));}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp104);goto _LLD2;_LLFB: if(*((
int*)_tmpD7)!= 9)goto _LLFD;_tmp105=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpD7)->f1;
_tmp106=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpD7)->f2;_LLFC: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumpexp_prec(myprec,_tmp105);Cyc_Absyndump_dump_char((int)',');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp106);Cyc_Absyndump_dump_char((int)')');
goto _LLD2;_LLFD: if(*((int*)_tmpD7)!= 10)goto _LLFF;_tmp107=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmpD7)->f1;_tmp108=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmpD7)->f2;_LLFE:
_tmp109=_tmp107;_tmp10A=_tmp108;goto _LL100;_LLFF: if(*((int*)_tmpD7)!= 11)goto
_LL101;_tmp109=((struct Cyc_Absyn_FnCall_e_struct*)_tmpD7)->f1;_tmp10A=((struct
Cyc_Absyn_FnCall_e_struct*)_tmpD7)->f2;_LL100: Cyc_Absyndump_dumpexp_prec(myprec,
_tmp109);Cyc_Absyndump_dump_nospace(({const char*_tmp152="(";_tag_arr(_tmp152,
sizeof(char),_get_zero_arr_size(_tmp152,2));}));Cyc_Absyndump_dumpexps_prec(20,
_tmp10A);Cyc_Absyndump_dump_nospace(({const char*_tmp153=")";_tag_arr(_tmp153,
sizeof(char),_get_zero_arr_size(_tmp153,2));}));goto _LLD2;_LL101: if(*((int*)
_tmpD7)!= 12)goto _LL103;_tmp10B=((struct Cyc_Absyn_Throw_e_struct*)_tmpD7)->f1;
_LL102: Cyc_Absyndump_dump(({const char*_tmp154="throw";_tag_arr(_tmp154,sizeof(
char),_get_zero_arr_size(_tmp154,6));}));Cyc_Absyndump_dumpexp_prec(myprec,
_tmp10B);goto _LLD2;_LL103: if(*((int*)_tmpD7)!= 13)goto _LL105;_tmp10C=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpD7)->f1;_LL104: _tmp10D=_tmp10C;goto _LL106;_LL105: if(*((int*)_tmpD7)!= 14)
goto _LL107;_tmp10D=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpD7)->f1;_LL106:
Cyc_Absyndump_dumpexp_prec(inprec,_tmp10D);goto _LLD2;_LL107: if(*((int*)_tmpD7)!= 
15)goto _LL109;_tmp10E=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpD7)->f1;
_tmp10F=((struct Cyc_Absyn_Cast_e_struct*)_tmpD7)->f2;_LL108: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumptyp(_tmp10E);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp10F);goto _LLD2;_LL109: if(*((int*)_tmpD7)!= 16)goto _LL10B;_tmp110=((
struct Cyc_Absyn_Address_e_struct*)_tmpD7)->f1;_LL10A: Cyc_Absyndump_dump_char((
int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp110);goto _LLD2;_LL10B: if(*((int*)
_tmpD7)!= 17)goto _LL10D;_tmp111=((struct Cyc_Absyn_New_e_struct*)_tmpD7)->f1;
_tmp112=((struct Cyc_Absyn_New_e_struct*)_tmpD7)->f2;_LL10C: Cyc_Absyndump_dump(({
const char*_tmp155="new ";_tag_arr(_tmp155,sizeof(char),_get_zero_arr_size(
_tmp155,5));}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp112);goto _LLD2;_LL10D: if(*((
int*)_tmpD7)!= 18)goto _LL10F;_tmp113=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpD7)->f1;_LL10E: Cyc_Absyndump_dump(({const char*_tmp156="sizeof(";_tag_arr(
_tmp156,sizeof(char),_get_zero_arr_size(_tmp156,8));}));Cyc_Absyndump_dumptyp(
_tmp113);Cyc_Absyndump_dump_char((int)')');goto _LLD2;_LL10F: if(*((int*)_tmpD7)!= 
19)goto _LL111;_tmp114=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpD7)->f1;_LL110:
Cyc_Absyndump_dump(({const char*_tmp157="sizeof(";_tag_arr(_tmp157,sizeof(char),
_get_zero_arr_size(_tmp157,8));}));Cyc_Absyndump_dumpexp(_tmp114);Cyc_Absyndump_dump_char((
int)')');goto _LLD2;_LL111: if(*((int*)_tmpD7)!= 20)goto _LL113;_tmp115=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmpD7)->f1;_tmp116=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpD7)->f2;if(*((int*)_tmp116)!= 0)goto _LL113;_tmp117=((struct Cyc_Absyn_StructField_struct*)
_tmp116)->f1;_LL112: Cyc_Absyndump_dump(({const char*_tmp158="offsetof(";_tag_arr(
_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,10));}));Cyc_Absyndump_dumptyp(
_tmp115);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmp117);
Cyc_Absyndump_dump_char((int)')');goto _LLD2;_LL113: if(*((int*)_tmpD7)!= 20)goto
_LL115;_tmp118=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpD7)->f1;_tmp119=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpD7)->f2;if(*((int*)_tmp119)!= 1)
goto _LL115;_tmp11A=((struct Cyc_Absyn_TupleIndex_struct*)_tmp119)->f1;_LL114: Cyc_Absyndump_dump(({
const char*_tmp159="offsetof(";_tag_arr(_tmp159,sizeof(char),_get_zero_arr_size(
_tmp159,10));}));Cyc_Absyndump_dumptyp(_tmp118);Cyc_Absyndump_dump_char((int)',');
Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp15C;_tmp15C.tag=
1;_tmp15C.f1=(unsigned int)((int)_tmp11A);{void*_tmp15A[1]={& _tmp15C};Cyc_aprintf(({
const char*_tmp15B="%d";_tag_arr(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,
3));}),_tag_arr(_tmp15A,sizeof(void*),1));}}));Cyc_Absyndump_dump_char((int)')');
goto _LLD2;_LL115: if(*((int*)_tmpD7)!= 21)goto _LL117;_tmp11B=((struct Cyc_Absyn_Gentyp_e_struct*)
_tmpD7)->f1;_tmp11C=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmpD7)->f2;_LL116:
Cyc_Absyndump_dump(({const char*_tmp15D="__gen(";_tag_arr(_tmp15D,sizeof(char),
_get_zero_arr_size(_tmp15D,7));}));Cyc_Absyndump_dumptvars(_tmp11B);Cyc_Absyndump_dumptyp(
_tmp11C);Cyc_Absyndump_dump_char((int)')');goto _LLD2;_LL117: if(*((int*)_tmpD7)!= 
22)goto _LL119;_tmp11D=((struct Cyc_Absyn_Deref_e_struct*)_tmpD7)->f1;_LL118: Cyc_Absyndump_dump_char((
int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp11D);goto _LLD2;_LL119: if(*((int*)
_tmpD7)!= 23)goto _LL11B;_tmp11E=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpD7)->f1;
_tmp11F=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpD7)->f2;_LL11A: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp11E);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp11F);goto _LLD2;_LL11B: if(*((int*)_tmpD7)!= 24)goto _LL11D;_tmp120=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpD7)->f1;_tmp121=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpD7)->f2;_LL11C: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp120);Cyc_Absyndump_dump_nospace(({const char*_tmp15E="->";_tag_arr(
_tmp15E,sizeof(char),_get_zero_arr_size(_tmp15E,3));}));Cyc_Absyndump_dump_nospace(*
_tmp121);goto _LLD2;_LL11D: if(*((int*)_tmpD7)!= 25)goto _LL11F;_tmp122=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpD7)->f1;_tmp123=((struct Cyc_Absyn_Subscript_e_struct*)_tmpD7)->f2;_LL11E: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp122);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp123);
Cyc_Absyndump_dump_char((int)']');goto _LLD2;_LL11F: if(*((int*)_tmpD7)!= 26)goto
_LL121;_tmp124=((struct Cyc_Absyn_Tuple_e_struct*)_tmpD7)->f1;_LL120: Cyc_Absyndump_dump(({
const char*_tmp15F="$(";_tag_arr(_tmp15F,sizeof(char),_get_zero_arr_size(_tmp15F,
3));}));Cyc_Absyndump_dumpexps_prec(20,_tmp124);Cyc_Absyndump_dump_char((int)')');
goto _LLD2;_LL121: if(*((int*)_tmpD7)!= 27)goto _LL123;_tmp125=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpD7)->f1;_tmp126=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpD7)->f2;_LL122:
Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp((*_tmp125).f3);Cyc_Absyndump_dump_char((
int)')');((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp126,({const char*_tmp160="{";_tag_arr(_tmp160,sizeof(
char),_get_zero_arr_size(_tmp160,2));}),({const char*_tmp161="}";_tag_arr(_tmp161,
sizeof(char),_get_zero_arr_size(_tmp161,2));}),({const char*_tmp162=",";_tag_arr(
_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,2));}));goto _LLD2;_LL123: if(*((
int*)_tmpD7)!= 28)goto _LL125;_tmp127=((struct Cyc_Absyn_Array_e_struct*)_tmpD7)->f1;
_LL124:((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp127,({const char*_tmp163="{";_tag_arr(_tmp163,sizeof(
char),_get_zero_arr_size(_tmp163,2));}),({const char*_tmp164="}";_tag_arr(_tmp164,
sizeof(char),_get_zero_arr_size(_tmp164,2));}),({const char*_tmp165=",";_tag_arr(
_tmp165,sizeof(char),_get_zero_arr_size(_tmp165,2));}));goto _LLD2;_LL125: if(*((
int*)_tmpD7)!= 29)goto _LL127;_tmp128=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpD7)->f1;_tmp129=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpD7)->f2;
_tmp12A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpD7)->f3;_LL126: Cyc_Absyndump_dump(({
const char*_tmp166="new {for";_tag_arr(_tmp166,sizeof(char),_get_zero_arr_size(
_tmp166,9));}));Cyc_Absyndump_dump_str((*_tmp128->name).f2);Cyc_Absyndump_dump_char((
int)'<');Cyc_Absyndump_dumpexp(_tmp129);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(
_tmp12A);Cyc_Absyndump_dump_char((int)'}');goto _LLD2;_LL127: if(*((int*)_tmpD7)!= 
30)goto _LL129;_tmp12B=((struct Cyc_Absyn_Struct_e_struct*)_tmpD7)->f1;_tmp12C=((
struct Cyc_Absyn_Struct_e_struct*)_tmpD7)->f2;_tmp12D=((struct Cyc_Absyn_Struct_e_struct*)
_tmpD7)->f3;_LL128: Cyc_Absyndump_dumpqvar(_tmp12B);Cyc_Absyndump_dump_char((int)'{');
if(_tmp12C != 0)Cyc_Absyndump_dumptps(_tmp12C);((void(*)(void(*f)(struct _tuple10*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp12D,({const char*
_tmp167="";_tag_arr(_tmp167,sizeof(char),_get_zero_arr_size(_tmp167,1));}),({
const char*_tmp168="}";_tag_arr(_tmp168,sizeof(char),_get_zero_arr_size(_tmp168,2));}),({
const char*_tmp169=",";_tag_arr(_tmp169,sizeof(char),_get_zero_arr_size(_tmp169,2));}));
goto _LLD2;_LL129: if(*((int*)_tmpD7)!= 31)goto _LL12B;_tmp12E=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpD7)->f2;_LL12A:((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp12E,({const char*_tmp16A="{";_tag_arr(_tmp16A,sizeof(
char),_get_zero_arr_size(_tmp16A,2));}),({const char*_tmp16B="}";_tag_arr(_tmp16B,
sizeof(char),_get_zero_arr_size(_tmp16B,2));}),({const char*_tmp16C=",";_tag_arr(
_tmp16C,sizeof(char),_get_zero_arr_size(_tmp16C,2));}));goto _LLD2;_LL12B: if(*((
int*)_tmpD7)!= 32)goto _LL12D;_tmp12F=((struct Cyc_Absyn_Tunion_e_struct*)_tmpD7)->f1;
_tmp130=((struct Cyc_Absyn_Tunion_e_struct*)_tmpD7)->f3;_LL12C: Cyc_Absyndump_dumpqvar(
_tmp130->name);if(_tmp12F != 0)((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp,_tmp12F,({const char*_tmp16D="(";_tag_arr(_tmp16D,sizeof(
char),_get_zero_arr_size(_tmp16D,2));}),({const char*_tmp16E=")";_tag_arr(_tmp16E,
sizeof(char),_get_zero_arr_size(_tmp16E,2));}),({const char*_tmp16F=",";_tag_arr(
_tmp16F,sizeof(char),_get_zero_arr_size(_tmp16F,2));}));goto _LLD2;_LL12D: if(*((
int*)_tmpD7)!= 33)goto _LL12F;_tmp131=((struct Cyc_Absyn_Enum_e_struct*)_tmpD7)->f1;
_LL12E: Cyc_Absyndump_dumpqvar(_tmp131);goto _LLD2;_LL12F: if(*((int*)_tmpD7)!= 34)
goto _LL131;_tmp132=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmpD7)->f1;_LL130: Cyc_Absyndump_dumpqvar(
_tmp132);goto _LLD2;_LL131: if(*((int*)_tmpD7)!= 35)goto _LL133;_tmp133=((struct Cyc_Absyn_Malloc_e_struct*)
_tmpD7)->f1;_tmp134=_tmp133.is_calloc;_tmp135=_tmp133.rgn;_tmp136=_tmp133.elt_type;
_tmp137=_tmp133.num_elts;_LL132: if(_tmp134){if(_tmp135 != 0){Cyc_Absyndump_dump(({
const char*_tmp170="rcalloc(";_tag_arr(_tmp170,sizeof(char),_get_zero_arr_size(
_tmp170,9));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp135);Cyc_Absyndump_dump(({
const char*_tmp171=",";_tag_arr(_tmp171,sizeof(char),_get_zero_arr_size(_tmp171,2));}));}
else{Cyc_Absyndump_dump(({const char*_tmp172="calloc";_tag_arr(_tmp172,sizeof(
char),_get_zero_arr_size(_tmp172,7));}));}Cyc_Absyndump_dumpexp(_tmp137);Cyc_Absyndump_dump(({
const char*_tmp173=",";_tag_arr(_tmp173,sizeof(char),_get_zero_arr_size(_tmp173,2));}));
Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp136)),0));
Cyc_Absyndump_dump(({const char*_tmp174=")";_tag_arr(_tmp174,sizeof(char),
_get_zero_arr_size(_tmp174,2));}));}else{if(_tmp135 != 0){Cyc_Absyndump_dump(({
const char*_tmp175="rmalloc(";_tag_arr(_tmp175,sizeof(char),_get_zero_arr_size(
_tmp175,9));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp135);Cyc_Absyndump_dump(({
const char*_tmp176=",";_tag_arr(_tmp176,sizeof(char),_get_zero_arr_size(_tmp176,2));}));}
else{Cyc_Absyndump_dump(({const char*_tmp177="malloc(";_tag_arr(_tmp177,sizeof(
char),_get_zero_arr_size(_tmp177,8));}));}if(_tmp136 != 0)Cyc_Absyndump_dumpexp(
Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp136,0),_tmp137,0));else{Cyc_Absyndump_dumpexp(
_tmp137);}Cyc_Absyndump_dump(({const char*_tmp178=")";_tag_arr(_tmp178,sizeof(
char),_get_zero_arr_size(_tmp178,2));}));}goto _LLD2;_LL133: if(*((int*)_tmpD7)!= 
36)goto _LL135;_tmp138=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpD7)->f1;
_tmp139=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpD7)->f2;_LL134:((void(*)(
void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmp139,({const char*_tmp179="{";_tag_arr(_tmp179,sizeof(char),_get_zero_arr_size(
_tmp179,2));}),({const char*_tmp17A="}";_tag_arr(_tmp17A,sizeof(char),
_get_zero_arr_size(_tmp17A,2));}),({const char*_tmp17B=",";_tag_arr(_tmp17B,
sizeof(char),_get_zero_arr_size(_tmp17B,2));}));goto _LLD2;_LL135: if(*((int*)
_tmpD7)!= 37)goto _LLD2;_tmp13A=((struct Cyc_Absyn_StmtExp_e_struct*)_tmpD7)->f1;
_LL136: Cyc_Absyndump_dump_nospace(({const char*_tmp17C="({";_tag_arr(_tmp17C,
sizeof(char),_get_zero_arr_size(_tmp17C,3));}));Cyc_Absyndump_dumpstmt(_tmp13A);
Cyc_Absyndump_dump_nospace(({const char*_tmp17D="})";_tag_arr(_tmp17D,sizeof(char),
_get_zero_arr_size(_tmp17D,3));}));goto _LLD2;_LLD2:;}if(inprec >= myprec)Cyc_Absyndump_dump_char((
int)')');}void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){Cyc_Absyndump_dumpexp_prec(
0,e);}void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs){for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause*_tmp17E=(struct Cyc_Absyn_Switch_clause*)
scs->hd;if(_tmp17E->where_clause == 0  && (void*)(_tmp17E->pattern)->r == (void*)((
void*)0))Cyc_Absyndump_dump(({const char*_tmp17F="default:";_tag_arr(_tmp17F,
sizeof(char),_get_zero_arr_size(_tmp17F,9));}));else{Cyc_Absyndump_dump(({const
char*_tmp180="case";_tag_arr(_tmp180,sizeof(char),_get_zero_arr_size(_tmp180,5));}));
Cyc_Absyndump_dumppat(_tmp17E->pattern);if(_tmp17E->where_clause != 0){Cyc_Absyndump_dump(({
const char*_tmp181="&&";_tag_arr(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,
3));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp17E->where_clause));}
Cyc_Absyndump_dump_nospace(({const char*_tmp182=":";_tag_arr(_tmp182,sizeof(char),
_get_zero_arr_size(_tmp182,2));}));}Cyc_Absyndump_dumpstmt(_tmp17E->body);}}void
Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s){void*_tmp183=(void*)s->r;struct
Cyc_Absyn_Exp*_tmp184;struct Cyc_Absyn_Stmt*_tmp185;struct Cyc_Absyn_Stmt*_tmp186;
struct Cyc_Absyn_Exp*_tmp187;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*
_tmp189;struct Cyc_Absyn_Stmt*_tmp18A;struct Cyc_Absyn_Stmt*_tmp18B;struct _tuple2
_tmp18C;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Stmt*_tmp18E;struct
_tagged_arr*_tmp18F;struct Cyc_Absyn_Exp*_tmp190;struct _tuple2 _tmp191;struct Cyc_Absyn_Exp*
_tmp192;struct _tuple2 _tmp193;struct Cyc_Absyn_Exp*_tmp194;struct Cyc_Absyn_Stmt*
_tmp195;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_List_List*_tmp197;struct Cyc_Absyn_Decl*
_tmp198;struct Cyc_Absyn_Stmt*_tmp199;struct _tagged_arr*_tmp19A;struct Cyc_Absyn_Stmt*
_tmp19B;struct Cyc_Absyn_Stmt*_tmp19C;struct _tuple2 _tmp19D;struct Cyc_Absyn_Exp*
_tmp19E;struct Cyc_List_List*_tmp19F;struct Cyc_List_List*_tmp1A0;struct Cyc_Absyn_Stmt*
_tmp1A1;struct Cyc_List_List*_tmp1A2;struct Cyc_Absyn_Tvar*_tmp1A3;struct Cyc_Absyn_Vardecl*
_tmp1A4;int _tmp1A5;struct Cyc_Absyn_Stmt*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A7;
_LL13C: if((int)_tmp183 != 0)goto _LL13E;_LL13D: Cyc_Absyndump_dump_semi();goto
_LL13B;_LL13E: if(_tmp183 <= (void*)1  || *((int*)_tmp183)!= 0)goto _LL140;_tmp184=((
struct Cyc_Absyn_Exp_s_struct*)_tmp183)->f1;_LL13F: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dumpexp(_tmp184);Cyc_Absyndump_dump_semi();goto _LL13B;_LL140: if(
_tmp183 <= (void*)1  || *((int*)_tmp183)!= 1)goto _LL142;_tmp185=((struct Cyc_Absyn_Seq_s_struct*)
_tmp183)->f1;_tmp186=((struct Cyc_Absyn_Seq_s_struct*)_tmp183)->f2;_LL141: if(Cyc_Absynpp_is_declaration(
_tmp185)){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp185);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dumpstmt(_tmp185);}if(Cyc_Absynpp_is_declaration(
_tmp186)){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp186);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dumpstmt(_tmp186);}goto _LL13B;_LL142: if(_tmp183 <= (
void*)1  || *((int*)_tmp183)!= 2)goto _LL144;_tmp187=((struct Cyc_Absyn_Return_s_struct*)
_tmp183)->f1;if(_tmp187 != 0)goto _LL144;_LL143: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({
const char*_tmp1A8="return;";_tag_arr(_tmp1A8,sizeof(char),_get_zero_arr_size(
_tmp1A8,8));}));goto _LL13B;_LL144: if(_tmp183 <= (void*)1  || *((int*)_tmp183)!= 2)
goto _LL146;_tmp188=((struct Cyc_Absyn_Return_s_struct*)_tmp183)->f1;_LL145: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1A9="return";_tag_arr(_tmp1A9,sizeof(
char),_get_zero_arr_size(_tmp1A9,7));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmp188));Cyc_Absyndump_dump_semi();goto _LL13B;_LL146: if(_tmp183 <= (
void*)1  || *((int*)_tmp183)!= 3)goto _LL148;_tmp189=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp183)->f1;_tmp18A=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp183)->f2;_tmp18B=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp183)->f3;_LL147: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(({const char*_tmp1AA="if(";_tag_arr(_tmp1AA,sizeof(char),
_get_zero_arr_size(_tmp1AA,4));}));Cyc_Absyndump_dumpexp(_tmp189);{void*_tmp1AB=(
void*)_tmp18A->r;_LL165: if(_tmp1AB <= (void*)1  || *((int*)_tmp1AB)!= 1)goto _LL167;
_LL166: goto _LL168;_LL167: if(_tmp1AB <= (void*)1  || *((int*)_tmp1AB)!= 11)goto
_LL169;_LL168: goto _LL16A;_LL169: if(_tmp1AB <= (void*)1  || *((int*)_tmp1AB)!= 3)
goto _LL16B;_LL16A: goto _LL16C;_LL16B: if(_tmp1AB <= (void*)1  || *((int*)_tmp1AB)!= 
12)goto _LL16D;_LL16C: Cyc_Absyndump_dump_nospace(({const char*_tmp1AC="){";
_tag_arr(_tmp1AC,sizeof(char),_get_zero_arr_size(_tmp1AC,3));}));Cyc_Absyndump_dumpstmt(
_tmp18A);Cyc_Absyndump_dump_char((int)'}');goto _LL164;_LL16D:;_LL16E: Cyc_Absyndump_dump_char((
int)')');Cyc_Absyndump_dumpstmt(_tmp18A);_LL164:;}{void*_tmp1AD=(void*)_tmp18B->r;
_LL170: if((int)_tmp1AD != 0)goto _LL172;_LL171: goto _LL16F;_LL172:;_LL173: Cyc_Absyndump_dump(({
const char*_tmp1AE="else{";_tag_arr(_tmp1AE,sizeof(char),_get_zero_arr_size(
_tmp1AE,6));}));Cyc_Absyndump_dumpstmt(_tmp18B);Cyc_Absyndump_dump_char((int)'}');
goto _LL16F;_LL16F:;}goto _LL13B;_LL148: if(_tmp183 <= (void*)1  || *((int*)_tmp183)
!= 4)goto _LL14A;_tmp18C=((struct Cyc_Absyn_While_s_struct*)_tmp183)->f1;_tmp18D=
_tmp18C.f1;_tmp18E=((struct Cyc_Absyn_While_s_struct*)_tmp183)->f2;_LL149: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1AF="while(";_tag_arr(_tmp1AF,sizeof(
char),_get_zero_arr_size(_tmp1AF,7));}));Cyc_Absyndump_dumpexp(_tmp18D);Cyc_Absyndump_dump_nospace(({
const char*_tmp1B0="){";_tag_arr(_tmp1B0,sizeof(char),_get_zero_arr_size(_tmp1B0,
3));}));Cyc_Absyndump_dumpstmt(_tmp18E);Cyc_Absyndump_dump_char((int)'}');goto
_LL13B;_LL14A: if(_tmp183 <= (void*)1  || *((int*)_tmp183)!= 5)goto _LL14C;_LL14B:
Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*_tmp1B1="break;";
_tag_arr(_tmp1B1,sizeof(char),_get_zero_arr_size(_tmp1B1,7));}));goto _LL13B;
_LL14C: if(_tmp183 <= (void*)1  || *((int*)_tmp183)!= 6)goto _LL14E;_LL14D: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1B2="continue;";_tag_arr(_tmp1B2,
sizeof(char),_get_zero_arr_size(_tmp1B2,10));}));goto _LL13B;_LL14E: if(_tmp183 <= (
void*)1  || *((int*)_tmp183)!= 7)goto _LL150;_tmp18F=((struct Cyc_Absyn_Goto_s_struct*)
_tmp183)->f1;_LL14F: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1B3="goto";_tag_arr(_tmp1B3,sizeof(char),_get_zero_arr_size(_tmp1B3,5));}));
Cyc_Absyndump_dump_str(_tmp18F);Cyc_Absyndump_dump_semi();goto _LL13B;_LL150: if(
_tmp183 <= (void*)1  || *((int*)_tmp183)!= 8)goto _LL152;_tmp190=((struct Cyc_Absyn_For_s_struct*)
_tmp183)->f1;_tmp191=((struct Cyc_Absyn_For_s_struct*)_tmp183)->f2;_tmp192=
_tmp191.f1;_tmp193=((struct Cyc_Absyn_For_s_struct*)_tmp183)->f3;_tmp194=_tmp193.f1;
_tmp195=((struct Cyc_Absyn_For_s_struct*)_tmp183)->f4;_LL151: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1B4="for(";_tag_arr(_tmp1B4,sizeof(
char),_get_zero_arr_size(_tmp1B4,5));}));Cyc_Absyndump_dumpexp(_tmp190);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmp192);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(
_tmp194);Cyc_Absyndump_dump_nospace(({const char*_tmp1B5="){";_tag_arr(_tmp1B5,
sizeof(char),_get_zero_arr_size(_tmp1B5,3));}));Cyc_Absyndump_dumpstmt(_tmp195);
Cyc_Absyndump_dump_char((int)'}');goto _LL13B;_LL152: if(_tmp183 <= (void*)1  || *((
int*)_tmp183)!= 9)goto _LL154;_tmp196=((struct Cyc_Absyn_Switch_s_struct*)_tmp183)->f1;
_tmp197=((struct Cyc_Absyn_Switch_s_struct*)_tmp183)->f2;_LL153: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1B6="switch(";_tag_arr(_tmp1B6,
sizeof(char),_get_zero_arr_size(_tmp1B6,8));}));Cyc_Absyndump_dumpexp(_tmp196);
Cyc_Absyndump_dump_nospace(({const char*_tmp1B7="){";_tag_arr(_tmp1B7,sizeof(char),
_get_zero_arr_size(_tmp1B7,3));}));Cyc_Absyndump_dumpswitchclauses(_tmp197);Cyc_Absyndump_dump_char((
int)'}');goto _LL13B;_LL154: if(_tmp183 <= (void*)1  || *((int*)_tmp183)!= 11)goto
_LL156;_tmp198=((struct Cyc_Absyn_Decl_s_struct*)_tmp183)->f1;_tmp199=((struct Cyc_Absyn_Decl_s_struct*)
_tmp183)->f2;_LL155: Cyc_Absyndump_dumpdecl(_tmp198);Cyc_Absyndump_dumpstmt(
_tmp199);goto _LL13B;_LL156: if(_tmp183 <= (void*)1  || *((int*)_tmp183)!= 12)goto
_LL158;_tmp19A=((struct Cyc_Absyn_Label_s_struct*)_tmp183)->f1;_tmp19B=((struct
Cyc_Absyn_Label_s_struct*)_tmp183)->f2;_LL157: if(Cyc_Absynpp_is_declaration(
_tmp19B)){Cyc_Absyndump_dump_str(_tmp19A);Cyc_Absyndump_dump_nospace(({const char*
_tmp1B8=": {";_tag_arr(_tmp1B8,sizeof(char),_get_zero_arr_size(_tmp1B8,4));}));
Cyc_Absyndump_dumpstmt(_tmp19B);Cyc_Absyndump_dump_char((int)'}');}else{Cyc_Absyndump_dump_str(
_tmp19A);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp19B);}goto
_LL13B;_LL158: if(_tmp183 <= (void*)1  || *((int*)_tmp183)!= 13)goto _LL15A;_tmp19C=((
struct Cyc_Absyn_Do_s_struct*)_tmp183)->f1;_tmp19D=((struct Cyc_Absyn_Do_s_struct*)
_tmp183)->f2;_tmp19E=_tmp19D.f1;_LL159: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({
const char*_tmp1B9="do{";_tag_arr(_tmp1B9,sizeof(char),_get_zero_arr_size(_tmp1B9,
4));}));Cyc_Absyndump_dumpstmt(_tmp19C);Cyc_Absyndump_dump_nospace(({const char*
_tmp1BA="}while(";_tag_arr(_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,8));}));
Cyc_Absyndump_dumpexp(_tmp19E);Cyc_Absyndump_dump_nospace(({const char*_tmp1BB=");";
_tag_arr(_tmp1BB,sizeof(char),_get_zero_arr_size(_tmp1BB,3));}));goto _LL13B;
_LL15A: if(_tmp183 <= (void*)1  || *((int*)_tmp183)!= 10)goto _LL15C;_tmp19F=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp183)->f1;if(_tmp19F != 0)goto _LL15C;_LL15B:
Cyc_Absyndump_dump(({const char*_tmp1BC="fallthru;";_tag_arr(_tmp1BC,sizeof(char),
_get_zero_arr_size(_tmp1BC,10));}));goto _LL13B;_LL15C: if(_tmp183 <= (void*)1  || *((
int*)_tmp183)!= 10)goto _LL15E;_tmp1A0=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp183)->f1;_LL15D: Cyc_Absyndump_dump(({const char*_tmp1BD="fallthru(";_tag_arr(
_tmp1BD,sizeof(char),_get_zero_arr_size(_tmp1BD,10));}));Cyc_Absyndump_dumpexps_prec(
20,_tmp1A0);Cyc_Absyndump_dump_nospace(({const char*_tmp1BE=");";_tag_arr(_tmp1BE,
sizeof(char),_get_zero_arr_size(_tmp1BE,3));}));goto _LL13B;_LL15E: if(_tmp183 <= (
void*)1  || *((int*)_tmp183)!= 14)goto _LL160;_tmp1A1=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp183)->f1;_tmp1A2=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp183)->f2;_LL15F:
Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*_tmp1BF="try";
_tag_arr(_tmp1BF,sizeof(char),_get_zero_arr_size(_tmp1BF,4));}));Cyc_Absyndump_dumpstmt(
_tmp1A1);Cyc_Absyndump_dump(({const char*_tmp1C0="catch{";_tag_arr(_tmp1C0,
sizeof(char),_get_zero_arr_size(_tmp1C0,7));}));Cyc_Absyndump_dumpswitchclauses(
_tmp1A2);Cyc_Absyndump_dump_char((int)'}');goto _LL13B;_LL160: if(_tmp183 <= (void*)
1  || *((int*)_tmp183)!= 15)goto _LL162;_tmp1A3=((struct Cyc_Absyn_Region_s_struct*)
_tmp183)->f1;_tmp1A4=((struct Cyc_Absyn_Region_s_struct*)_tmp183)->f2;_tmp1A5=((
struct Cyc_Absyn_Region_s_struct*)_tmp183)->f3;_tmp1A6=((struct Cyc_Absyn_Region_s_struct*)
_tmp183)->f4;_LL161: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1C1="region";_tag_arr(_tmp1C1,sizeof(char),_get_zero_arr_size(_tmp1C1,7));}));
if(_tmp1A5)Cyc_Absyndump_dump(({const char*_tmp1C2="[resetable]";_tag_arr(_tmp1C2,
sizeof(char),_get_zero_arr_size(_tmp1C2,12));}));Cyc_Absyndump_dump(({const char*
_tmp1C3="<";_tag_arr(_tmp1C3,sizeof(char),_get_zero_arr_size(_tmp1C3,2));}));Cyc_Absyndump_dumptvar(
_tmp1A3);Cyc_Absyndump_dump(({const char*_tmp1C4="> ";_tag_arr(_tmp1C4,sizeof(
char),_get_zero_arr_size(_tmp1C4,3));}));Cyc_Absyndump_dumpqvar(_tmp1A4->name);
Cyc_Absyndump_dump(({const char*_tmp1C5="{";_tag_arr(_tmp1C5,sizeof(char),
_get_zero_arr_size(_tmp1C5,2));}));Cyc_Absyndump_dumpstmt(_tmp1A6);Cyc_Absyndump_dump(({
const char*_tmp1C6="}";_tag_arr(_tmp1C6,sizeof(char),_get_zero_arr_size(_tmp1C6,2));}));
goto _LL13B;_LL162: if(_tmp183 <= (void*)1  || *((int*)_tmp183)!= 16)goto _LL13B;
_tmp1A7=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp183)->f1;_LL163: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1C7="reset_region(";_tag_arr(_tmp1C7,
sizeof(char),_get_zero_arr_size(_tmp1C7,14));}));Cyc_Absyndump_dumpexp(_tmp1A7);
Cyc_Absyndump_dump(({const char*_tmp1C8=");";_tag_arr(_tmp1C8,sizeof(char),
_get_zero_arr_size(_tmp1C8,3));}));goto _LL13B;_LL13B:;}struct _tuple11{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(struct _tuple11*dp){Cyc_Absyndump_egroup(
Cyc_Absyndump_dumpdesignator,(*dp).f1,({const char*_tmp1C9="";_tag_arr(_tmp1C9,
sizeof(char),_get_zero_arr_size(_tmp1C9,1));}),({const char*_tmp1CA="=";_tag_arr(
_tmp1CA,sizeof(char),_get_zero_arr_size(_tmp1CA,2));}),({const char*_tmp1CB="=";
_tag_arr(_tmp1CB,sizeof(char),_get_zero_arr_size(_tmp1CB,2));}));Cyc_Absyndump_dumppat((*
dp).f2);}void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){void*_tmp1CC=(void*)p->r;
void*_tmp1CD;int _tmp1CE;void*_tmp1CF;int _tmp1D0;void*_tmp1D1;int _tmp1D2;char
_tmp1D3;struct _tagged_arr _tmp1D4;struct Cyc_Absyn_Vardecl*_tmp1D5;struct Cyc_List_List*
_tmp1D6;int _tmp1D7;struct Cyc_Absyn_Pat*_tmp1D8;struct Cyc_Absyn_Vardecl*_tmp1D9;
struct Cyc_Absyn_Tvar*_tmp1DA;struct Cyc_Absyn_Vardecl*_tmp1DB;struct _tuple0*
_tmp1DC;struct _tuple0*_tmp1DD;struct Cyc_List_List*_tmp1DE;int _tmp1DF;struct Cyc_Absyn_AggrInfo
_tmp1E0;void*_tmp1E1;struct Cyc_List_List*_tmp1E2;struct Cyc_List_List*_tmp1E3;int
_tmp1E4;struct Cyc_Absyn_Tunionfield*_tmp1E5;struct Cyc_List_List*_tmp1E6;int
_tmp1E7;struct Cyc_Absyn_Enumfield*_tmp1E8;struct Cyc_Absyn_Enumfield*_tmp1E9;
struct Cyc_Absyn_Exp*_tmp1EA;_LL175: if((int)_tmp1CC != 0)goto _LL177;_LL176: Cyc_Absyndump_dump_char((
int)'_');goto _LL174;_LL177: if((int)_tmp1CC != 1)goto _LL179;_LL178: Cyc_Absyndump_dump(({
const char*_tmp1EB="NULL";_tag_arr(_tmp1EB,sizeof(char),_get_zero_arr_size(
_tmp1EB,5));}));goto _LL174;_LL179: if(_tmp1CC <= (void*)2  || *((int*)_tmp1CC)!= 7)
goto _LL17B;_tmp1CD=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp1CC)->f1;if((int)
_tmp1CD != 2)goto _LL17B;_tmp1CE=((struct Cyc_Absyn_Int_p_struct*)_tmp1CC)->f2;
_LL17A: _tmp1D0=_tmp1CE;goto _LL17C;_LL17B: if(_tmp1CC <= (void*)2  || *((int*)
_tmp1CC)!= 7)goto _LL17D;_tmp1CF=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp1CC)->f1;
if((int)_tmp1CF != 0)goto _LL17D;_tmp1D0=((struct Cyc_Absyn_Int_p_struct*)_tmp1CC)->f2;
_LL17C: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp1EE;
_tmp1EE.tag=1;_tmp1EE.f1=(unsigned int)_tmp1D0;{void*_tmp1EC[1]={& _tmp1EE};Cyc_aprintf(({
const char*_tmp1ED="%d";_tag_arr(_tmp1ED,sizeof(char),_get_zero_arr_size(_tmp1ED,
3));}),_tag_arr(_tmp1EC,sizeof(void*),1));}}));goto _LL174;_LL17D: if(_tmp1CC <= (
void*)2  || *((int*)_tmp1CC)!= 7)goto _LL17F;_tmp1D1=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp1CC)->f1;if((int)_tmp1D1 != 1)goto _LL17F;_tmp1D2=((struct Cyc_Absyn_Int_p_struct*)
_tmp1CC)->f2;_LL17E: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp1F1;_tmp1F1.tag=1;_tmp1F1.f1=(unsigned int)_tmp1D2;{void*_tmp1EF[1]={&
_tmp1F1};Cyc_aprintf(({const char*_tmp1F0="%u";_tag_arr(_tmp1F0,sizeof(char),
_get_zero_arr_size(_tmp1F0,3));}),_tag_arr(_tmp1EF,sizeof(void*),1));}}));goto
_LL174;_LL17F: if(_tmp1CC <= (void*)2  || *((int*)_tmp1CC)!= 8)goto _LL181;_tmp1D3=((
struct Cyc_Absyn_Char_p_struct*)_tmp1CC)->f1;_LL180: Cyc_Absyndump_dump(({const
char*_tmp1F2="'";_tag_arr(_tmp1F2,sizeof(char),_get_zero_arr_size(_tmp1F2,2));}));
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp1D3));Cyc_Absyndump_dump_nospace(({
const char*_tmp1F3="'";_tag_arr(_tmp1F3,sizeof(char),_get_zero_arr_size(_tmp1F3,2));}));
goto _LL174;_LL181: if(_tmp1CC <= (void*)2  || *((int*)_tmp1CC)!= 9)goto _LL183;
_tmp1D4=((struct Cyc_Absyn_Float_p_struct*)_tmp1CC)->f1;_LL182: Cyc_Absyndump_dump(
_tmp1D4);goto _LL174;_LL183: if(_tmp1CC <= (void*)2  || *((int*)_tmp1CC)!= 0)goto
_LL185;_tmp1D5=((struct Cyc_Absyn_Var_p_struct*)_tmp1CC)->f1;_LL184: Cyc_Absyndump_dumpqvar(
_tmp1D5->name);goto _LL174;_LL185: if(_tmp1CC <= (void*)2  || *((int*)_tmp1CC)!= 3)
goto _LL187;_tmp1D6=((struct Cyc_Absyn_Tuple_p_struct*)_tmp1CC)->f1;_tmp1D7=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp1CC)->f2;_LL186: {struct _tagged_arr term=
_tmp1D7?({const char*_tmp1F6=", ...)";_tag_arr(_tmp1F6,sizeof(char),
_get_zero_arr_size(_tmp1F6,7));}):({const char*_tmp1F7=")";_tag_arr(_tmp1F7,
sizeof(char),_get_zero_arr_size(_tmp1F7,2));});((void(*)(void(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp1D6,({const char*
_tmp1F4="$(";_tag_arr(_tmp1F4,sizeof(char),_get_zero_arr_size(_tmp1F4,3));}),
term,({const char*_tmp1F5=",";_tag_arr(_tmp1F5,sizeof(char),_get_zero_arr_size(
_tmp1F5,2));}));goto _LL174;}_LL187: if(_tmp1CC <= (void*)2  || *((int*)_tmp1CC)!= 4)
goto _LL189;_tmp1D8=((struct Cyc_Absyn_Pointer_p_struct*)_tmp1CC)->f1;_LL188: Cyc_Absyndump_dump(({
const char*_tmp1F8="&";_tag_arr(_tmp1F8,sizeof(char),_get_zero_arr_size(_tmp1F8,2));}));
Cyc_Absyndump_dumppat(_tmp1D8);goto _LL174;_LL189: if(_tmp1CC <= (void*)2  || *((int*)
_tmp1CC)!= 1)goto _LL18B;_tmp1D9=((struct Cyc_Absyn_Reference_p_struct*)_tmp1CC)->f1;
_LL18A: Cyc_Absyndump_dump(({const char*_tmp1F9="*";_tag_arr(_tmp1F9,sizeof(char),
_get_zero_arr_size(_tmp1F9,2));}));Cyc_Absyndump_dumpqvar(_tmp1D9->name);goto
_LL174;_LL18B: if(_tmp1CC <= (void*)2  || *((int*)_tmp1CC)!= 2)goto _LL18D;_tmp1DA=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp1CC)->f1;_tmp1DB=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp1CC)->f2;_LL18C: Cyc_Absyndump_dumpqvar(_tmp1DB->name);Cyc_Absyndump_dump_char((
int)'<');Cyc_Absyndump_dumptvar(_tmp1DA);Cyc_Absyndump_dump_char((int)'>');goto
_LL174;_LL18D: if(_tmp1CC <= (void*)2  || *((int*)_tmp1CC)!= 12)goto _LL18F;_tmp1DC=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp1CC)->f1;_LL18E: Cyc_Absyndump_dumpqvar(
_tmp1DC);goto _LL174;_LL18F: if(_tmp1CC <= (void*)2  || *((int*)_tmp1CC)!= 13)goto
_LL191;_tmp1DD=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1CC)->f1;_tmp1DE=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1CC)->f2;_tmp1DF=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp1CC)->f3;_LL190: {struct _tagged_arr term=_tmp1DF?({const char*_tmp1FC=", ...)";
_tag_arr(_tmp1FC,sizeof(char),_get_zero_arr_size(_tmp1FC,7));}):({const char*
_tmp1FD=")";_tag_arr(_tmp1FD,sizeof(char),_get_zero_arr_size(_tmp1FD,2));});Cyc_Absyndump_dumpqvar(
_tmp1DD);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp1DE,({const char*_tmp1FA="(";_tag_arr(_tmp1FA,sizeof(
char),_get_zero_arr_size(_tmp1FA,2));}),term,({const char*_tmp1FB=",";_tag_arr(
_tmp1FB,sizeof(char),_get_zero_arr_size(_tmp1FB,2));}));goto _LL174;}_LL191: if(
_tmp1CC <= (void*)2  || *((int*)_tmp1CC)!= 5)goto _LL193;_tmp1E0=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp1CC)->f1;_tmp1E1=(void*)_tmp1E0.aggr_info;_tmp1E2=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp1CC)->f2;_tmp1E3=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1CC)->f3;_tmp1E4=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp1CC)->f4;_LL192: {struct _tuple0*_tmp1FF;
struct _tuple3 _tmp1FE=Cyc_Absyn_aggr_kinded_name(_tmp1E1);_tmp1FF=_tmp1FE.f2;{
struct _tagged_arr term=_tmp1E4?({const char*_tmp205=", ...)";_tag_arr(_tmp205,
sizeof(char),_get_zero_arr_size(_tmp205,7));}):({const char*_tmp206=")";_tag_arr(
_tmp206,sizeof(char),_get_zero_arr_size(_tmp206,2));});Cyc_Absyndump_dumpqvar(
_tmp1FF);Cyc_Absyndump_dump_char((int)'{');((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp1E2,({const char*
_tmp200="[";_tag_arr(_tmp200,sizeof(char),_get_zero_arr_size(_tmp200,2));}),({
const char*_tmp201="]";_tag_arr(_tmp201,sizeof(char),_get_zero_arr_size(_tmp201,2));}),({
const char*_tmp202=",";_tag_arr(_tmp202,sizeof(char),_get_zero_arr_size(_tmp202,2));}));((
void(*)(void(*f)(struct _tuple11*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp1E3,({const char*_tmp203="";_tag_arr(_tmp203,sizeof(char),_get_zero_arr_size(
_tmp203,1));}),term,({const char*_tmp204=",";_tag_arr(_tmp204,sizeof(char),
_get_zero_arr_size(_tmp204,2));}));goto _LL174;}}_LL193: if(_tmp1CC <= (void*)2  || *((
int*)_tmp1CC)!= 6)goto _LL195;_tmp1E5=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1CC)->f2;
_tmp1E6=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1CC)->f3;_tmp1E7=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1CC)->f4;_LL194: {struct _tagged_arr term=_tmp1E7?({const char*_tmp209=", ...)";
_tag_arr(_tmp209,sizeof(char),_get_zero_arr_size(_tmp209,7));}):({const char*
_tmp20A=")";_tag_arr(_tmp20A,sizeof(char),_get_zero_arr_size(_tmp20A,2));});Cyc_Absyndump_dumpqvar(
_tmp1E5->name);if(_tmp1E6 != 0)((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp1E6,({const char*_tmp207="(";_tag_arr(_tmp207,sizeof(
char),_get_zero_arr_size(_tmp207,2));}),term,({const char*_tmp208=",";_tag_arr(
_tmp208,sizeof(char),_get_zero_arr_size(_tmp208,2));}));goto _LL174;}_LL195: if(
_tmp1CC <= (void*)2  || *((int*)_tmp1CC)!= 10)goto _LL197;_tmp1E8=((struct Cyc_Absyn_Enum_p_struct*)
_tmp1CC)->f2;_LL196: _tmp1E9=_tmp1E8;goto _LL198;_LL197: if(_tmp1CC <= (void*)2  || *((
int*)_tmp1CC)!= 11)goto _LL199;_tmp1E9=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp1CC)->f2;_LL198: Cyc_Absyndump_dumpqvar(_tmp1E9->name);goto _LL174;_LL199: if(
_tmp1CC <= (void*)2  || *((int*)_tmp1CC)!= 14)goto _LL174;_tmp1EA=((struct Cyc_Absyn_Exp_p_struct*)
_tmp1CC)->f1;_LL19A: Cyc_Absyndump_dumpexp(_tmp1EA);goto _LL174;_LL174:;}void Cyc_Absyndump_dumptunionfield(
struct Cyc_Absyn_Tunionfield*ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}void Cyc_Absyndump_dumptunionfields(struct Cyc_List_List*
fields){((void(*)(void(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*l,
struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumptunionfield,
fields,({const char*_tmp20B=",";_tag_arr(_tmp20B,sizeof(char),_get_zero_arr_size(
_tmp20B,2));}));}void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);if(ef->tag != 0){Cyc_Absyndump_dump(({const char*
_tmp20C=" = ";_tag_arr(_tmp20C,sizeof(char),_get_zero_arr_size(_tmp20C,4));}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List*fields){((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,
fields,({const char*_tmp20D=",";_tag_arr(_tmp20D,sizeof(char),_get_zero_arr_size(
_tmp20D,2));}));}void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Aggrfield _tmp20F;struct
_tagged_arr*_tmp210;struct Cyc_Absyn_Tqual _tmp211;void*_tmp212;struct Cyc_Absyn_Exp*
_tmp213;struct Cyc_List_List*_tmp214;struct Cyc_Absyn_Aggrfield*_tmp20E=(struct Cyc_Absyn_Aggrfield*)
fields->hd;_tmp20F=*_tmp20E;_tmp210=_tmp20F.name;_tmp211=_tmp20F.tq;_tmp212=(
void*)_tmp20F.type;_tmp213=_tmp20F.width;_tmp214=_tmp20F.attributes;((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tagged_arr*),struct _tagged_arr*))Cyc_Absyndump_dumptqtd)(
_tmp211,_tmp212,Cyc_Absyndump_dump_str,_tmp210);Cyc_Absyndump_dumpatts(_tmp214);
if(_tmp213 != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp((struct
Cyc_Absyn_Exp*)_tmp213);}Cyc_Absyndump_dump_semi();}}void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(td->name);Cyc_Absyndump_dumptvars(
td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple3*pr){{void*_tmp215=(*pr).f1;_LL19C: if((int)_tmp215 != 8)goto _LL19E;
_LL19D: goto _LL19B;_LL19E: if((int)_tmp215 != 0)goto _LL1A0;_LL19F: Cyc_Absyndump_dump(({
const char*_tmp216="_stdcall";_tag_arr(_tmp216,sizeof(char),_get_zero_arr_size(
_tmp216,9));}));goto _LL19B;_LL1A0: if((int)_tmp215 != 1)goto _LL1A2;_LL1A1: Cyc_Absyndump_dump(({
const char*_tmp217="_cdecl";_tag_arr(_tmp217,sizeof(char),_get_zero_arr_size(
_tmp217,7));}));goto _LL19B;_LL1A2: if((int)_tmp215 != 2)goto _LL1A4;_LL1A3: Cyc_Absyndump_dump(({
const char*_tmp218="_fastcall";_tag_arr(_tmp218,sizeof(char),_get_zero_arr_size(
_tmp218,10));}));goto _LL19B;_LL1A4:;_LL1A5: goto _LL19B;_LL19B:;}Cyc_Absyndump_dumpqvar((*
pr).f2);}static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv(fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}
static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){for(0;vds != 0;vds=vds->tl){
Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl != 0)
Cyc_Absyndump_dump_char((int)',');}}void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Vardecl _tmp21A;void*_tmp21B;
struct _tuple0*_tmp21C;struct Cyc_Absyn_Tqual _tmp21D;void*_tmp21E;struct Cyc_Absyn_Exp*
_tmp21F;struct Cyc_List_List*_tmp220;struct Cyc_Absyn_Vardecl*_tmp219=vd;_tmp21A=*
_tmp219;_tmp21B=(void*)_tmp21A.sc;_tmp21C=_tmp21A.name;_tmp21D=_tmp21A.tq;
_tmp21E=(void*)_tmp21A.type;_tmp21F=_tmp21A.initializer;_tmp220=_tmp21A.attributes;
Cyc_Absyndump_dumploc(loc);if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dumpatts(
_tmp220);Cyc_Absyndump_dumpscope(_tmp21B);{struct Cyc_Absyn_Tqual _tmp222;void*
_tmp223;struct Cyc_List_List*_tmp224;struct _tuple5 _tmp221=Cyc_Absynpp_to_tms(
_tmp21D,_tmp21E);_tmp222=_tmp221.f1;_tmp223=_tmp221.f2;_tmp224=_tmp221.f3;{void*
call_conv=(void*)8;{struct Cyc_List_List*tms2=_tmp224;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp225=(void*)tms2->hd;struct Cyc_List_List*_tmp226;_LL1A7: if(*((int*)
_tmp225)!= 5)goto _LL1A9;_tmp226=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp225)->f2;
_LL1A8: for(0;_tmp226 != 0;_tmp226=_tmp226->tl){void*_tmp227=(void*)_tmp226->hd;
_LL1AC: if((int)_tmp227 != 0)goto _LL1AE;_LL1AD: call_conv=(void*)0;goto _LL1AB;
_LL1AE: if((int)_tmp227 != 1)goto _LL1B0;_LL1AF: call_conv=(void*)1;goto _LL1AB;
_LL1B0: if((int)_tmp227 != 2)goto _LL1B2;_LL1B1: call_conv=(void*)2;goto _LL1AB;
_LL1B2:;_LL1B3: goto _LL1AB;_LL1AB:;}goto _LL1A6;_LL1A9:;_LL1AA: goto _LL1A6;_LL1A6:;}}
Cyc_Absyndump_dumptq(_tmp222);Cyc_Absyndump_dumpntyp(_tmp223);{struct _tuple3
_tmp228=({struct _tuple3 _tmp229;_tmp229.f1=call_conv;_tmp229.f2=_tmp21C;_tmp229;});((
void(*)(struct Cyc_List_List*tms,void(*f)(struct _tuple3*),struct _tuple3*a))Cyc_Absyndump_dumptms)(
Cyc_List_imp_rev(_tmp224),Cyc_Absyndump_dump_callconv_qvar,& _tmp228);}}}}else{
if(_tmp21B == (void*)3  && Cyc_Absyndump_qvar_to_Cids){void*_tmp22A=Cyc_Tcutil_compress(
_tmp21E);_LL1B5: if(_tmp22A <= (void*)4  || *((int*)_tmp22A)!= 8)goto _LL1B7;_LL1B6:
goto _LL1B4;_LL1B7:;_LL1B8: Cyc_Absyndump_dumpscope(_tmp21B);_LL1B4:;}else{Cyc_Absyndump_dumpscope(
_tmp21B);}((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct
_tuple0*))Cyc_Absyndump_dumptqtd)(_tmp21D,_tmp21E,Cyc_Absyndump_dumpqvar,_tmp21C);
Cyc_Absyndump_dumpatts(_tmp220);}if(_tmp21F != 0){Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp21F);}Cyc_Absyndump_dump_semi();}
struct _tuple12{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*d){void*_tmp22B=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp22C;
struct Cyc_Absyn_Fndecl*_tmp22D;struct Cyc_Absyn_Aggrdecl*_tmp22E;struct Cyc_Absyn_Tuniondecl*
_tmp22F;struct Cyc_Absyn_Tuniondecl _tmp230;void*_tmp231;struct _tuple0*_tmp232;
struct Cyc_List_List*_tmp233;struct Cyc_Core_Opt*_tmp234;int _tmp235;struct Cyc_Absyn_Enumdecl*
_tmp236;struct Cyc_Absyn_Enumdecl _tmp237;void*_tmp238;struct _tuple0*_tmp239;
struct Cyc_Core_Opt*_tmp23A;struct Cyc_Absyn_Pat*_tmp23B;struct Cyc_Absyn_Exp*
_tmp23C;struct Cyc_List_List*_tmp23D;struct Cyc_Absyn_Typedefdecl*_tmp23E;struct
_tagged_arr*_tmp23F;struct Cyc_List_List*_tmp240;struct _tuple0*_tmp241;struct Cyc_List_List*
_tmp242;struct Cyc_List_List*_tmp243;struct Cyc_List_List*_tmp244;struct Cyc_List_List*
_tmp245;_LL1BA: if(_tmp22B <= (void*)2  || *((int*)_tmp22B)!= 0)goto _LL1BC;_tmp22C=((
struct Cyc_Absyn_Var_d_struct*)_tmp22B)->f1;_LL1BB: Cyc_Absyndump_dumpvardecl(
_tmp22C,d->loc);goto _LL1B9;_LL1BC: if(_tmp22B <= (void*)2  || *((int*)_tmp22B)!= 1)
goto _LL1BE;_tmp22D=((struct Cyc_Absyn_Fn_d_struct*)_tmp22B)->f1;_LL1BD: Cyc_Absyndump_dumploc(
d->loc);if(Cyc_Absyndump_to_VC)Cyc_Absyndump_dumpatts(_tmp22D->attributes);if(
_tmp22D->is_inline){if(Cyc_Absyndump_to_VC)Cyc_Absyndump_dump(({const char*
_tmp246="__inline";_tag_arr(_tmp246,sizeof(char),_get_zero_arr_size(_tmp246,9));}));
else{Cyc_Absyndump_dump(({const char*_tmp247="inline";_tag_arr(_tmp247,sizeof(
char),_get_zero_arr_size(_tmp247,7));}));}}Cyc_Absyndump_dumpscope((void*)
_tmp22D->sc);{void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp248=_cycalloc(
sizeof(*_tmp248));_tmp248[0]=({struct Cyc_Absyn_FnType_struct _tmp249;_tmp249.tag=
8;_tmp249.f1=({struct Cyc_Absyn_FnInfo _tmp24A;_tmp24A.tvars=_tmp22D->tvs;_tmp24A.effect=
_tmp22D->effect;_tmp24A.ret_typ=(void*)((void*)_tmp22D->ret_type);_tmp24A.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple4*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp22D->args);_tmp24A.c_varargs=_tmp22D->c_varargs;
_tmp24A.cyc_varargs=_tmp22D->cyc_varargs;_tmp24A.rgn_po=_tmp22D->rgn_po;_tmp24A.attributes=
0;_tmp24A;});_tmp249;});_tmp248;});((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(
0),t,Cyc_Absyndump_to_VC?Cyc_Absyndump_dump_callconv_fdqvar: Cyc_Absyndump_dump_atts_qvar,
_tmp22D);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp22D->body);
Cyc_Absyndump_dump_char((int)'}');goto _LL1B9;}_LL1BE: if(_tmp22B <= (void*)2  || *((
int*)_tmp22B)!= 4)goto _LL1C0;_tmp22E=((struct Cyc_Absyn_Aggr_d_struct*)_tmp22B)->f1;
_LL1BF: Cyc_Absyndump_dumpscope((void*)_tmp22E->sc);Cyc_Absyndump_dumpaggr_kind((
void*)_tmp22E->kind);Cyc_Absyndump_dumpqvar(_tmp22E->name);Cyc_Absyndump_dumptvars(
_tmp22E->tvs);if(_tmp22E->impl == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22E->impl))->exist_vars
!= 0)((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22E->impl))->exist_vars,({
const char*_tmp24B="<";_tag_arr(_tmp24B,sizeof(char),_get_zero_arr_size(_tmp24B,2));}),({
const char*_tmp24C=">";_tag_arr(_tmp24C,sizeof(char),_get_zero_arr_size(_tmp24C,2));}),({
const char*_tmp24D=",";_tag_arr(_tmp24D,sizeof(char),_get_zero_arr_size(_tmp24D,2));}));
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22E->impl))->rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp22E->impl))->rgn_po);}Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp22E->impl))->fields);Cyc_Absyndump_dump(({const char*_tmp24E="}";
_tag_arr(_tmp24E,sizeof(char),_get_zero_arr_size(_tmp24E,2));}));Cyc_Absyndump_dumpatts(
_tmp22E->attributes);Cyc_Absyndump_dump(({const char*_tmp24F=";";_tag_arr(_tmp24F,
sizeof(char),_get_zero_arr_size(_tmp24F,2));}));}goto _LL1B9;_LL1C0: if(_tmp22B <= (
void*)2  || *((int*)_tmp22B)!= 5)goto _LL1C2;_tmp22F=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp22B)->f1;_tmp230=*_tmp22F;_tmp231=(void*)_tmp230.sc;_tmp232=_tmp230.name;
_tmp233=_tmp230.tvs;_tmp234=_tmp230.fields;_tmp235=_tmp230.is_xtunion;_LL1C1: Cyc_Absyndump_dumpscope(
_tmp231);Cyc_Absyndump_dump(_tmp235?({const char*_tmp250="xtunion";_tag_arr(
_tmp250,sizeof(char),_get_zero_arr_size(_tmp250,8));}):({const char*_tmp251="tunion";
_tag_arr(_tmp251,sizeof(char),_get_zero_arr_size(_tmp251,7));}));Cyc_Absyndump_dumpqvar(
_tmp232);Cyc_Absyndump_dumptvars(_tmp233);if(_tmp234 == 0)Cyc_Absyndump_dump_semi();
else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumptunionfields((struct Cyc_List_List*)
_tmp234->v);Cyc_Absyndump_dump_nospace(({const char*_tmp252="};";_tag_arr(_tmp252,
sizeof(char),_get_zero_arr_size(_tmp252,3));}));}goto _LL1B9;_LL1C2: if(_tmp22B <= (
void*)2  || *((int*)_tmp22B)!= 6)goto _LL1C4;_tmp236=((struct Cyc_Absyn_Enum_d_struct*)
_tmp22B)->f1;_tmp237=*_tmp236;_tmp238=(void*)_tmp237.sc;_tmp239=_tmp237.name;
_tmp23A=_tmp237.fields;_LL1C3: Cyc_Absyndump_dumpscope(_tmp238);Cyc_Absyndump_dump(({
const char*_tmp253="enum ";_tag_arr(_tmp253,sizeof(char),_get_zero_arr_size(
_tmp253,6));}));Cyc_Absyndump_dumpqvar(_tmp239);if(_tmp23A == 0)Cyc_Absyndump_dump_semi();
else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)
_tmp23A->v);Cyc_Absyndump_dump_nospace(({const char*_tmp254="};";_tag_arr(_tmp254,
sizeof(char),_get_zero_arr_size(_tmp254,3));}));}return;_LL1C4: if(_tmp22B <= (
void*)2  || *((int*)_tmp22B)!= 2)goto _LL1C6;_tmp23B=((struct Cyc_Absyn_Let_d_struct*)
_tmp22B)->f1;_tmp23C=((struct Cyc_Absyn_Let_d_struct*)_tmp22B)->f3;_LL1C5: Cyc_Absyndump_dump(({
const char*_tmp255="let";_tag_arr(_tmp255,sizeof(char),_get_zero_arr_size(_tmp255,
4));}));Cyc_Absyndump_dumppat(_tmp23B);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(
_tmp23C);Cyc_Absyndump_dump_semi();goto _LL1B9;_LL1C6: if(_tmp22B <= (void*)2  || *((
int*)_tmp22B)!= 3)goto _LL1C8;_tmp23D=((struct Cyc_Absyn_Letv_d_struct*)_tmp22B)->f1;
_LL1C7: Cyc_Absyndump_dump(({const char*_tmp256="let ";_tag_arr(_tmp256,sizeof(
char),_get_zero_arr_size(_tmp256,5));}));Cyc_Absyndump_dumpids(_tmp23D);Cyc_Absyndump_dump_semi();
goto _LL1B9;_LL1C8: if(_tmp22B <= (void*)2  || *((int*)_tmp22B)!= 7)goto _LL1CA;
_tmp23E=((struct Cyc_Absyn_Typedef_d_struct*)_tmp22B)->f1;_LL1C9: if(!Cyc_Absyndump_expand_typedefs
 || _tmp23E->defn != 0  && Cyc_Absynpp_is_anon_aggrtype((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp23E->defn))->v)){Cyc_Absyndump_dump(({const char*_tmp257="typedef";
_tag_arr(_tmp257,sizeof(char),_get_zero_arr_size(_tmp257,8));}));{void*t;if(
_tmp23E->defn == 0)t=Cyc_Absyn_new_evar(_tmp23E->kind,0);else{t=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp23E->defn))->v;}((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(
_tmp23E->tq,t,Cyc_Absyndump_dumptypedefname,_tmp23E);Cyc_Absyndump_dumpatts(
_tmp23E->atts);Cyc_Absyndump_dump_semi();}}goto _LL1B9;_LL1CA: if(_tmp22B <= (void*)
2  || *((int*)_tmp22B)!= 8)goto _LL1CC;_tmp23F=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp22B)->f1;_tmp240=((struct Cyc_Absyn_Namespace_d_struct*)_tmp22B)->f2;_LL1CB:
Cyc_Absyndump_dump(({const char*_tmp258="namespace";_tag_arr(_tmp258,sizeof(char),
_get_zero_arr_size(_tmp258,10));}));Cyc_Absyndump_dump_str(_tmp23F);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp240 != 0;_tmp240=_tmp240->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp240->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1B9;
_LL1CC: if(_tmp22B <= (void*)2  || *((int*)_tmp22B)!= 9)goto _LL1CE;_tmp241=((struct
Cyc_Absyn_Using_d_struct*)_tmp22B)->f1;_tmp242=((struct Cyc_Absyn_Using_d_struct*)
_tmp22B)->f2;_LL1CD: Cyc_Absyndump_dump(({const char*_tmp259="using";_tag_arr(
_tmp259,sizeof(char),_get_zero_arr_size(_tmp259,6));}));Cyc_Absyndump_dumpqvar(
_tmp241);Cyc_Absyndump_dump_char((int)'{');for(0;_tmp242 != 0;_tmp242=_tmp242->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp242->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1B9;_LL1CE: if(_tmp22B <= (void*)2  || *((int*)_tmp22B)!= 10)goto
_LL1D0;_tmp243=((struct Cyc_Absyn_ExternC_d_struct*)_tmp22B)->f1;_LL1CF: Cyc_Absyndump_dump(({
const char*_tmp25A="extern \"C\" {";_tag_arr(_tmp25A,sizeof(char),
_get_zero_arr_size(_tmp25A,13));}));for(0;_tmp243 != 0;_tmp243=_tmp243->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)_tmp243->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1B9;
_LL1D0: if(_tmp22B <= (void*)2  || *((int*)_tmp22B)!= 11)goto _LL1D2;_tmp244=((
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp22B)->f1;_tmp245=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp22B)->f2;_LL1D1: Cyc_Absyndump_dump(({const char*_tmp25B="extern \"C include\" {";
_tag_arr(_tmp25B,sizeof(char),_get_zero_arr_size(_tmp25B,21));}));for(0;_tmp244
!= 0;_tmp244=_tmp244->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp244->hd);}
Cyc_Absyndump_dump_char((int)'}');if(_tmp245 != 0){Cyc_Absyndump_dump(({const char*
_tmp25C=" export {";_tag_arr(_tmp25C,sizeof(char),_get_zero_arr_size(_tmp25C,10));}));
for(0;_tmp245 != 0;_tmp245=_tmp245->tl){struct _tuple0*_tmp25E;struct _tuple12
_tmp25D=*((struct _tuple12*)_tmp245->hd);_tmp25E=_tmp25D.f2;Cyc_Absyndump_dumpqvar(
_tmp25E);if(_tmp245->tl != 0)Cyc_Absyndump_dump_char((int)',');}Cyc_Absyndump_dump(({
const char*_tmp25F="}";_tag_arr(_tmp25F,sizeof(char),_get_zero_arr_size(_tmp25F,2));}));}
goto _LL1B9;_LL1D2: if((int)_tmp22B != 0)goto _LL1D4;_LL1D3: Cyc_Absyndump_dump(({
const char*_tmp260=" __cyclone_port_on__; ";_tag_arr(_tmp260,sizeof(char),
_get_zero_arr_size(_tmp260,23));}));goto _LL1B9;_LL1D4: if((int)_tmp22B != 1)goto
_LL1B9;_LL1D5: Cyc_Absyndump_dump(({const char*_tmp261=" __cyclone_port_off__; ";
_tag_arr(_tmp261,sizeof(char),_get_zero_arr_size(_tmp261,24));}));goto _LL1B9;
_LL1B9:;}static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){struct
_tuple6 pr=Cyc_Evexp_eval_const_uint_exp(e);if(pr.f1 != 1  || !pr.f2){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}void Cyc_Absyndump_dumptms(
struct Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){f(a);return;}{void*
_tmp262=(void*)tms->hd;struct Cyc_Absyn_PtrAtts _tmp263;void*_tmp264;struct Cyc_Absyn_Conref*
_tmp265;struct Cyc_Absyn_Conref*_tmp266;struct Cyc_Absyn_Conref*_tmp267;struct Cyc_Absyn_Tqual
_tmp268;_LL1D7: if(*((int*)_tmp262)!= 2)goto _LL1D9;_tmp263=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp262)->f1;_tmp264=(void*)_tmp263.rgn;_tmp265=_tmp263.nullable;_tmp266=_tmp263.bounds;
_tmp267=_tmp263.zero_term;_tmp268=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp262)->f2;
_LL1D8:{void*_tmp269=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp266);struct
Cyc_Absyn_Exp*_tmp26A;void*_tmp26B;_LL1DC: if((int)_tmp269 != 0)goto _LL1DE;_LL1DD:
Cyc_Absyndump_dump_char((int)'?');goto _LL1DB;_LL1DE: if(_tmp269 <= (void*)1  || *((
int*)_tmp269)!= 0)goto _LL1E0;_tmp26A=((struct Cyc_Absyn_Upper_b_struct*)_tmp269)->f1;
_LL1DF: Cyc_Absyndump_dump_char((int)(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmp265)?'*':'@'));Cyc_Absyndump_dump_upperbound(_tmp26A);goto _LL1DB;_LL1E0:
if(_tmp269 <= (void*)1  || *((int*)_tmp269)!= 1)goto _LL1DB;_tmp26B=(void*)((struct
Cyc_Absyn_AbsUpper_b_struct*)_tmp269)->f1;_LL1E1: Cyc_Absyndump_dump_char((int)(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp265)?'*':'@'));
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumptyp(_tmp26B);Cyc_Absyndump_dump_char((
int)'}');_LL1DB:;}if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp267))Cyc_Absyndump_dump(({const char*_tmp26C="ZEROTERM";_tag_arr(_tmp26C,
sizeof(char),_get_zero_arr_size(_tmp26C,9));}));{void*_tmp26D=Cyc_Tcutil_compress(
_tmp264);struct Cyc_Absyn_Tvar*_tmp26E;_LL1E3: if((int)_tmp26D != 2)goto _LL1E5;
_LL1E4: goto _LL1E2;_LL1E5: if(_tmp26D <= (void*)4  || *((int*)_tmp26D)!= 1)goto
_LL1E7;_tmp26E=((struct Cyc_Absyn_VarType_struct*)_tmp26D)->f1;_LL1E6: Cyc_Absyndump_dump_str(
_tmp26E->name);goto _LL1E2;_LL1E7:;_LL1E8:({void*_tmp26F[0]={};Cyc_Tcutil_impos(({
const char*_tmp270="dumptms: bad rgn type in Pointer_mod";_tag_arr(_tmp270,
sizeof(char),_get_zero_arr_size(_tmp270,37));}),_tag_arr(_tmp26F,sizeof(void*),0));});
_LL1E2:;}Cyc_Absyndump_dumptq(_tmp268);Cyc_Absyndump_dumptms(tms->tl,f,a);
return;_LL1D9:;_LL1DA: {int next_is_pointer=0;if(tms->tl != 0){void*_tmp271=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;_LL1EA: if(*((int*)_tmp271)!= 2)
goto _LL1EC;_LL1EB: next_is_pointer=1;goto _LL1E9;_LL1EC:;_LL1ED: goto _LL1E9;_LL1E9:;}
if(next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(tms->tl,
f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp272=(void*)
tms->hd;struct Cyc_Absyn_Conref*_tmp273;struct Cyc_Absyn_Exp*_tmp274;struct Cyc_Absyn_Conref*
_tmp275;void*_tmp276;struct Cyc_List_List*_tmp277;int _tmp278;struct Cyc_Absyn_VarargInfo*
_tmp279;struct Cyc_Core_Opt*_tmp27A;struct Cyc_List_List*_tmp27B;void*_tmp27C;
struct Cyc_List_List*_tmp27D;struct Cyc_Position_Segment*_tmp27E;struct Cyc_List_List*
_tmp27F;struct Cyc_Position_Segment*_tmp280;int _tmp281;struct Cyc_List_List*
_tmp282;_LL1EF: if(*((int*)_tmp272)!= 0)goto _LL1F1;_tmp273=((struct Cyc_Absyn_Carray_mod_struct*)
_tmp272)->f1;_LL1F0: Cyc_Absyndump_dump(({const char*_tmp283="[]";_tag_arr(_tmp283,
sizeof(char),_get_zero_arr_size(_tmp283,3));}));if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp273))Cyc_Absyndump_dump(({const char*_tmp284="ZEROTERM";
_tag_arr(_tmp284,sizeof(char),_get_zero_arr_size(_tmp284,9));}));goto _LL1EE;
_LL1F1: if(*((int*)_tmp272)!= 1)goto _LL1F3;_tmp274=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp272)->f1;_tmp275=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmp272)->f2;
_LL1F2: Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp274);Cyc_Absyndump_dump_char((
int)']');if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp275))Cyc_Absyndump_dump(({const char*_tmp285="ZEROTERM";_tag_arr(_tmp285,
sizeof(char),_get_zero_arr_size(_tmp285,9));}));goto _LL1EE;_LL1F3: if(*((int*)
_tmp272)!= 3)goto _LL1F5;_tmp276=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp272)->f1;if(*((int*)_tmp276)!= 1)goto _LL1F5;_tmp277=((struct Cyc_Absyn_WithTypes_struct*)
_tmp276)->f1;_tmp278=((struct Cyc_Absyn_WithTypes_struct*)_tmp276)->f2;_tmp279=((
struct Cyc_Absyn_WithTypes_struct*)_tmp276)->f3;_tmp27A=((struct Cyc_Absyn_WithTypes_struct*)
_tmp276)->f4;_tmp27B=((struct Cyc_Absyn_WithTypes_struct*)_tmp276)->f5;_LL1F4: Cyc_Absyndump_dumpfunargs(
_tmp277,_tmp278,_tmp279,_tmp27A,_tmp27B);goto _LL1EE;_LL1F5: if(*((int*)_tmp272)!= 
3)goto _LL1F7;_tmp27C=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp272)->f1;
if(*((int*)_tmp27C)!= 0)goto _LL1F7;_tmp27D=((struct Cyc_Absyn_NoTypes_struct*)
_tmp27C)->f1;_tmp27E=((struct Cyc_Absyn_NoTypes_struct*)_tmp27C)->f2;_LL1F6:((
void(*)(void(*f)(struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr
start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,
_tmp27D,({const char*_tmp286="(";_tag_arr(_tmp286,sizeof(char),_get_zero_arr_size(
_tmp286,2));}),({const char*_tmp287=")";_tag_arr(_tmp287,sizeof(char),
_get_zero_arr_size(_tmp287,2));}),({const char*_tmp288=",";_tag_arr(_tmp288,
sizeof(char),_get_zero_arr_size(_tmp288,2));}));goto _LL1EE;_LL1F7: if(*((int*)
_tmp272)!= 4)goto _LL1F9;_tmp27F=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp272)->f1;
_tmp280=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp272)->f2;_tmp281=((struct
Cyc_Absyn_TypeParams_mod_struct*)_tmp272)->f3;_LL1F8: if(_tmp281)Cyc_Absyndump_dumpkindedtvars(
_tmp27F);else{Cyc_Absyndump_dumptvars(_tmp27F);}goto _LL1EE;_LL1F9: if(*((int*)
_tmp272)!= 5)goto _LL1FB;_tmp282=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp272)->f2;
_LL1FA: Cyc_Absyndump_dumpatts(_tmp282);goto _LL1EE;_LL1FB: if(*((int*)_tmp272)!= 2)
goto _LL1EE;_LL1FC:({void*_tmp289[0]={};Cyc_Tcutil_impos(({const char*_tmp28A="dumptms";
_tag_arr(_tmp28A,sizeof(char),_get_zero_arr_size(_tmp28A,8));}),_tag_arr(_tmp289,
sizeof(void*),0));});_LL1EE:;}return;}_LL1D6:;}}void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){struct Cyc_Absyn_Tqual
_tmp28C;void*_tmp28D;struct Cyc_List_List*_tmp28E;struct _tuple5 _tmp28B=Cyc_Absynpp_to_tms(
tq,t);_tmp28C=_tmp28B.f1;_tmp28D=_tmp28B.f2;_tmp28E=_tmp28B.f3;Cyc_Absyndump_dumptq(
_tmp28C);Cyc_Absyndump_dumpntyp(_tmp28D);Cyc_Absyndump_dumptms(Cyc_List_imp_rev(
_tmp28E),f,a);}void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=f;for(0;tdl
!= 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}({void*
_tmp28F[0]={};Cyc_fprintf(f,({const char*_tmp290="\n";_tag_arr(_tmp290,sizeof(
char),_get_zero_arr_size(_tmp290,2));}),_tag_arr(_tmp28F,sizeof(void*),0));});}
