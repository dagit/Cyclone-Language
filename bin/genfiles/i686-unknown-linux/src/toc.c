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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
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

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
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

#define _tag_arr(tcurr,elt_sz,num_elts) ({ \
  struct _tagged_arr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_arr(arr,elt_sz,num_elts) ((arr).curr)
#else
#define _untag_arr(arr,elt_sz,num_elts) ({ \
  struct _tagged_arr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif

#define _get_arr_size(arr,elt_sz) \
  ({struct _tagged_arr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})

#define _tagged_arr_plus(arr,elt_sz,change) ({ \
  struct _tagged_arr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })

#define _tagged_arr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })

#define _tagged_arr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  struct _tagged_arr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })

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
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;
short*f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;};struct _tagged_arr
Cyc_aprintf(struct _tagged_arr,struct _tagged_arr);int Cyc_fflush(struct Cyc___cycFILE*);
int Cyc_fprintf(struct Cyc___cycFILE*,struct _tagged_arr,struct _tagged_arr);struct
Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;
unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};int
Cyc_vfprintf(struct Cyc___cycFILE*,struct _tagged_arr,struct _tagged_arr);extern
char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];void Cyc_List_iter_c(void(*f)(void*,void*),void*env,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);int Cyc_List_forall(int(*pred)(
void*),struct Cyc_List_List*x);struct _tagged_arr Cyc_strconcat(struct _tagged_arr,
struct _tagged_arr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);}
;int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*
s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[
11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[
11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*
d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[
14];struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};
struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int
q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
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
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Carray_mod_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
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
struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Address_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*f1;void*f2;
};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
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
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;
int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct
Cyc_Absyn_AggrInfo f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Tunion_p_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*
f3;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Pat{void*r;
struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*cnst_exp;struct Cyc_Absyn_Stmt*body;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
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
struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
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
zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _tagged_arr
f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct
_tagged_arr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
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
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
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
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct
_tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _tagged_arr Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*);struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);
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
0;unsigned int Cyc_Toc_total_bounds_checks=0;unsigned int Cyc_Toc_bounds_checks_eliminated=
0;static struct Cyc_List_List*Cyc_Toc_result_decls=0;static struct Cyc_List_List*Cyc_Toc_tuple_types=
0;static int Cyc_Toc_tuple_type_counter=0;static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";
char Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*
Cyc_Toc_unimp(struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_vfprintf(Cyc_stderr,
fmt,ap);({void*_tmp0[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="\n";
_tag_arr(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,2));}),_tag_arr(_tmp0,
sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((
void*)Cyc_Toc_Toc_Unimplemented);}static void*Cyc_Toc_toc_impos(struct _tagged_arr
fmt,struct _tagged_arr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({void*_tmp2[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp3="\n";_tag_arr(_tmp3,sizeof(char),_get_zero_arr_size(
_tmp3,2));}),_tag_arr(_tmp2,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);(int)_throw((void*)Cyc_Toc_Toc_Impossible);}char Cyc_Toc_Match_error[
16]="\000\000\000\000Match_error\000";static char _tmp4[5]="curr";static struct
_tagged_arr Cyc_Toc_curr_string={_tmp4,_tmp4,_tmp4 + 5};static struct _tagged_arr*
Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp5[4]="tag";static struct
_tagged_arr Cyc_Toc_tag_string={_tmp5,_tmp5,_tmp5 + 4};static struct _tagged_arr*Cyc_Toc_tag_sp=&
Cyc_Toc_tag_string;static char _tmp6[14]="_handler_cons";static struct _tagged_arr
Cyc_Toc__handler_cons_string={_tmp6,_tmp6,_tmp6 + 14};static struct _tagged_arr*Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string;static char _tmp7[8]="handler";static struct
_tagged_arr Cyc_Toc_handler_string={_tmp7,_tmp7,_tmp7 + 8};static struct _tagged_arr*
Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp8[14]="_RegionHandle";
static struct _tagged_arr Cyc_Toc__RegionHandle_string={_tmp8,_tmp8,_tmp8 + 14};
static struct _tagged_arr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
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
void*)& Cyc_Toc_tagged_arr_typ_v;static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0};
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**
skip_stmt_opt=0;if(skip_stmt_opt == 0)skip_stmt_opt=({struct Cyc_Absyn_Stmt**
_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51[0]=Cyc_Absyn_skip_stmt(0);_tmp51;});
return*((struct Cyc_Absyn_Stmt**)_check_null(skip_stmt_opt));}struct _tuple6{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct
Cyc_Absyn_Exp*e){void*_tmp52=(void*)e->r;void*_tmp53;char _tmp54;void*_tmp55;
short _tmp56;void*_tmp57;int _tmp58;void*_tmp59;long long _tmp5A;void*_tmp5B;struct
Cyc_Absyn_Exp*_tmp5C;struct Cyc_List_List*_tmp5D;struct Cyc_List_List*_tmp5E;
struct Cyc_List_List*_tmp5F;struct Cyc_List_List*_tmp60;struct Cyc_List_List*_tmp61;
_LL1: if(*((int*)_tmp52)!= 0)goto _LL3;_tmp53=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp52)->f1;if(_tmp53 <= (void*)1?1:*((int*)_tmp53)!= 0)goto _LL3;_tmp54=((struct
Cyc_Absyn_Char_c_struct*)_tmp53)->f2;_LL2: return _tmp54 == '\000';_LL3: if(*((int*)
_tmp52)!= 0)goto _LL5;_tmp55=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp52)->f1;
if(_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 1)goto _LL5;_tmp56=((struct Cyc_Absyn_Short_c_struct*)
_tmp55)->f2;_LL4: return _tmp56 == 0;_LL5: if(*((int*)_tmp52)!= 0)goto _LL7;_tmp57=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp52)->f1;if(_tmp57 <= (void*)1?1:*((
int*)_tmp57)!= 2)goto _LL7;_tmp58=((struct Cyc_Absyn_Int_c_struct*)_tmp57)->f2;
_LL6: return _tmp58 == 0;_LL7: if(*((int*)_tmp52)!= 0)goto _LL9;_tmp59=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp52)->f1;if(_tmp59 <= (void*)1?1:*((int*)
_tmp59)!= 3)goto _LL9;_tmp5A=((struct Cyc_Absyn_LongLong_c_struct*)_tmp59)->f2;
_LL8: return _tmp5A == 0;_LL9: if(*((int*)_tmp52)!= 0)goto _LLB;_tmp5B=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp52)->f1;if((int)_tmp5B != 0)goto _LLB;_LLA:
return 1;_LLB: if(*((int*)_tmp52)!= 13)goto _LLD;_tmp5C=((struct Cyc_Absyn_Cast_e_struct*)
_tmp52)->f2;_LLC: return Cyc_Toc_is_zero(_tmp5C);_LLD: if(*((int*)_tmp52)!= 24)goto
_LLF;_tmp5D=((struct Cyc_Absyn_Tuple_e_struct*)_tmp52)->f1;_LLE: return((int(*)(
int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,
_tmp5D);_LLF: if(*((int*)_tmp52)!= 26)goto _LL11;_tmp5E=((struct Cyc_Absyn_Array_e_struct*)
_tmp52)->f1;_LL10: _tmp5F=_tmp5E;goto _LL12;_LL11: if(*((int*)_tmp52)!= 28)goto
_LL13;_tmp5F=((struct Cyc_Absyn_Struct_e_struct*)_tmp52)->f3;_LL12: _tmp60=_tmp5F;
goto _LL14;_LL13: if(*((int*)_tmp52)!= 25)goto _LL15;_tmp60=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp52)->f2;_LL14: _tmp61=_tmp60;goto _LL16;_LL15: if(*((int*)_tmp52)!= 34)goto
_LL17;_tmp61=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp52)->f2;_LL16: for(0;
_tmp61 != 0;_tmp61=_tmp61->tl){if(!Cyc_Toc_is_zero((*((struct _tuple6*)_tmp61->hd)).f2))
return 0;}return 1;_LL17:;_LL18: return 0;_LL0:;}static int Cyc_Toc_is_nullable(void*t){
void*_tmp62=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp63;struct Cyc_Absyn_PtrAtts
_tmp64;struct Cyc_Absyn_Conref*_tmp65;_LL1A: if(_tmp62 <= (void*)3?1:*((int*)_tmp62)
!= 4)goto _LL1C;_tmp63=((struct Cyc_Absyn_PointerType_struct*)_tmp62)->f1;_tmp64=
_tmp63.ptr_atts;_tmp65=_tmp64.nullable;_LL1B: return((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp65);_LL1C:;_LL1D:({void*_tmp66[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp67="is_nullable";
_tag_arr(_tmp67,sizeof(char),_get_zero_arr_size(_tmp67,12));}),_tag_arr(_tmp66,
sizeof(void*),0));});_LL19:;}static struct _tuple1*Cyc_Toc_collapse_qvar_tag(
struct _tuple1*x,struct _tagged_arr tag){return({struct _tuple1*_tmp68=_cycalloc(
sizeof(*_tmp68));_tmp68->f1=(*x).f1;_tmp68->f2=({struct _tagged_arr*_tmp69=
_cycalloc(sizeof(*_tmp69));_tmp69[0]=(struct _tagged_arr)Cyc_strconcat((struct
_tagged_arr)*(*x).f2,(struct _tagged_arr)tag);_tmp69;});_tmp68;});}struct _tuple7{
struct _tagged_arr*f1;struct Cyc_List_List*f2;};static struct _tagged_arr*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){{struct Cyc_List_List*_tmp6A=Cyc_Toc_tuple_types;for(0;
_tmp6A != 0;_tmp6A=_tmp6A->tl){struct _tuple7 _tmp6C;struct _tagged_arr*_tmp6D;
struct Cyc_List_List*_tmp6E;struct _tuple7*_tmp6B=(struct _tuple7*)_tmp6A->hd;
_tmp6C=*_tmp6B;_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;{struct Cyc_List_List*_tmp6F=
tqs0;for(0;_tmp6F != 0?_tmp6E != 0: 0;(_tmp6F=_tmp6F->tl,_tmp6E=_tmp6E->tl)){if(!
Cyc_Tcutil_unify((*((struct _tuple4*)_tmp6F->hd)).f2,(void*)_tmp6E->hd))break;}
if(_tmp6F == 0?_tmp6E == 0: 0)return _tmp6D;}}}{struct _tagged_arr*x=({struct
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
!= 0){if((carries_value?((struct Cyc_Absyn_Tunionfield*)_tmp89->hd)->typs != 0: 0)?
1:(!carries_value?((struct Cyc_Absyn_Tunionfield*)_tmp89->hd)->typs == 0: 0))++ ans;
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
Cyc_Absyn_Conref*_tmp99;struct Cyc_Core_Opt*_tmp9A;struct Cyc_Core_Opt _tmp9B;void*
_tmp9C;_LL1F: if(_tmp94 <= (void*)3?1:*((int*)_tmp94)!= 7)goto _LL21;_tmp95=((
struct Cyc_Absyn_ArrayType_struct*)_tmp94)->f1;_tmp96=(void*)_tmp95.elt_type;
_tmp97=_tmp95.tq;_tmp98=_tmp95.num_elts;_tmp99=_tmp95.zero_term;_LL20: return Cyc_Absyn_array_typ(
Cyc_Toc_typ_to_c_array(_tmp96),_tmp97,_tmp98,Cyc_Absyn_false_conref);_LL21: if(
_tmp94 <= (void*)3?1:*((int*)_tmp94)!= 0)goto _LL23;_tmp9A=((struct Cyc_Absyn_Evar_struct*)
_tmp94)->f2;if(_tmp9A == 0)goto _LL23;_tmp9B=*_tmp9A;_tmp9C=(void*)_tmp9B.v;_LL22:
return Cyc_Toc_typ_to_c_array(_tmp9C);_LL23:;_LL24: return Cyc_Toc_typ_to_c(t);
_LL1E:;}static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*
f){return({struct Cyc_Absyn_Aggrfield*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->name=
f->name;_tmp9D->tq=f->tq;_tmp9D->type=(void*)Cyc_Toc_typ_to_c((void*)f->type);
_tmp9D->width=f->width;_tmp9D->attributes=f->attributes;_tmp9D;});}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp9E=
t;struct Cyc_Core_Opt*_tmp9F;struct Cyc_Core_Opt*_tmpA0;struct Cyc_Core_Opt _tmpA1;
void*_tmpA2;struct Cyc_Absyn_Tvar*_tmpA3;struct Cyc_Absyn_TunionInfo _tmpA4;void*
_tmpA5;struct Cyc_Absyn_TunionFieldInfo _tmpA6;void*_tmpA7;struct Cyc_Absyn_Tuniondecl*
_tmpA8;struct Cyc_Absyn_Tunionfield*_tmpA9;struct Cyc_Absyn_PtrInfo _tmpAA;void*
_tmpAB;struct Cyc_Absyn_Tqual _tmpAC;struct Cyc_Absyn_PtrAtts _tmpAD;struct Cyc_Absyn_Conref*
_tmpAE;struct Cyc_Absyn_ArrayInfo _tmpAF;void*_tmpB0;struct Cyc_Absyn_Tqual _tmpB1;
struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_FnInfo _tmpB3;void*_tmpB4;struct Cyc_List_List*
_tmpB5;int _tmpB6;struct Cyc_Absyn_VarargInfo*_tmpB7;struct Cyc_List_List*_tmpB8;
struct Cyc_List_List*_tmpB9;void*_tmpBA;struct Cyc_List_List*_tmpBB;struct Cyc_Absyn_AggrInfo
_tmpBC;void*_tmpBD;struct Cyc_List_List*_tmpBE;struct _tuple1*_tmpBF;struct Cyc_List_List*
_tmpC0;struct _tuple1*_tmpC1;struct Cyc_List_List*_tmpC2;struct Cyc_Absyn_Typedefdecl*
_tmpC3;void**_tmpC4;void*_tmpC5;_LL26: if((int)_tmp9E != 0)goto _LL28;_LL27: return t;
_LL28: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 0)goto _LL2A;_tmp9F=((struct Cyc_Absyn_Evar_struct*)
_tmp9E)->f2;if(_tmp9F != 0)goto _LL2A;_LL29: return Cyc_Absyn_sint_typ;_LL2A: if(
_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 0)goto _LL2C;_tmpA0=((struct Cyc_Absyn_Evar_struct*)
_tmp9E)->f2;if(_tmpA0 == 0)goto _LL2C;_tmpA1=*_tmpA0;_tmpA2=(void*)_tmpA1.v;_LL2B:
return Cyc_Toc_typ_to_c(_tmpA2);_LL2C: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 1)
goto _LL2E;_tmpA3=((struct Cyc_Absyn_VarType_struct*)_tmp9E)->f1;_LL2D: if(Cyc_Tcutil_tvar_kind(
_tmpA3)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}_LL2E:
if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 2)goto _LL30;_tmpA4=((struct Cyc_Absyn_TunionType_struct*)
_tmp9E)->f1;_tmpA5=(void*)_tmpA4.tunion_info;if(*((int*)_tmpA5)!= 1)goto _LL30;
_LL2F: return Cyc_Absyn_void_star_typ();_LL30: if(_tmp9E <= (void*)3?1:*((int*)
_tmp9E)!= 2)goto _LL32;_LL31:({void*_tmpC6[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmpC7="unresolved TunionType";
_tag_arr(_tmpC7,sizeof(char),_get_zero_arr_size(_tmpC7,22));}),_tag_arr(_tmpC6,
sizeof(void*),0));});_LL32: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 3)goto _LL34;
_tmpA6=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp9E)->f1;_tmpA7=(void*)
_tmpA6.field_info;if(*((int*)_tmpA7)!= 1)goto _LL34;_tmpA8=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpA7)->f1;_tmpA9=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpA7)->f2;_LL33:
if(_tmpA9->typs == 0){if(_tmpA8->is_xtunion)return Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);else{return Cyc_Absyn_uint_typ;}}
else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmpA9->name,({const char*
_tmpC8="_struct";_tag_arr(_tmpC8,sizeof(char),_get_zero_arr_size(_tmpC8,8));})));}
_LL34: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 3)goto _LL36;_LL35:({void*_tmpC9[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmpCA="unresolved TunionFieldType";_tag_arr(_tmpCA,sizeof(char),
_get_zero_arr_size(_tmpCA,27));}),_tag_arr(_tmpC9,sizeof(void*),0));});_LL36: if(
_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 4)goto _LL38;_tmpAA=((struct Cyc_Absyn_PointerType_struct*)
_tmp9E)->f1;_tmpAB=(void*)_tmpAA.elt_typ;_tmpAC=_tmpAA.elt_tq;_tmpAD=_tmpAA.ptr_atts;
_tmpAE=_tmpAD.bounds;_LL37: _tmpAB=Cyc_Toc_typ_to_c_array(_tmpAB);{void*_tmpCB=(
void*)(Cyc_Absyn_compress_conref(_tmpAE))->v;void*_tmpCC;_LL5F: if(_tmpCB <= (void*)
1?1:*((int*)_tmpCB)!= 0)goto _LL61;_tmpCC=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpCB)->f1;if((int)_tmpCC != 0)goto _LL61;_LL60: return Cyc_Toc_tagged_arr_typ;
_LL61:;_LL62: return Cyc_Absyn_star_typ(_tmpAB,(void*)2,_tmpAC,Cyc_Absyn_false_conref);
_LL5E:;}_LL38: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 5)goto _LL3A;_LL39: goto
_LL3B;_LL3A: if((int)_tmp9E != 1)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if(_tmp9E <= (void*)
3?1:*((int*)_tmp9E)!= 6)goto _LL3E;_LL3D: return t;_LL3E: if(_tmp9E <= (void*)3?1:*((
int*)_tmp9E)!= 7)goto _LL40;_tmpAF=((struct Cyc_Absyn_ArrayType_struct*)_tmp9E)->f1;
_tmpB0=(void*)_tmpAF.elt_type;_tmpB1=_tmpAF.tq;_tmpB2=_tmpAF.num_elts;_LL3F:
return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmpB0),_tmpB1,_tmpB2,Cyc_Absyn_false_conref);
_LL40: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 8)goto _LL42;_tmpB3=((struct Cyc_Absyn_FnType_struct*)
_tmp9E)->f1;_tmpB4=(void*)_tmpB3.ret_typ;_tmpB5=_tmpB3.args;_tmpB6=_tmpB3.c_varargs;
_tmpB7=_tmpB3.cyc_varargs;_tmpB8=_tmpB3.attributes;_LL41: {struct Cyc_List_List*
_tmpCD=0;for(0;_tmpB8 != 0;_tmpB8=_tmpB8->tl){void*_tmpCE=(void*)_tmpB8->hd;_LL64:
if((int)_tmpCE != 3)goto _LL66;_LL65: goto _LL67;_LL66: if((int)_tmpCE != 4)goto _LL68;
_LL67: goto _LL69;_LL68: if(_tmpCE <= (void*)17?1:*((int*)_tmpCE)!= 3)goto _LL6A;
_LL69: continue;_LL6A: if(_tmpCE <= (void*)17?1:*((int*)_tmpCE)!= 4)goto _LL6C;_LL6B:
continue;_LL6C:;_LL6D: _tmpCD=({struct Cyc_List_List*_tmpCF=_cycalloc(sizeof(*
_tmpCF));_tmpCF->hd=(void*)((void*)_tmpB8->hd);_tmpCF->tl=_tmpCD;_tmpCF;});goto
_LL63;_LL63:;}{struct Cyc_List_List*_tmpD0=((struct Cyc_List_List*(*)(struct
_tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmpB5);if(_tmpB7 != 0){void*_tmpD1=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ((void*)
_tmpB7->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct _tuple2*
_tmpD2=({struct _tuple2*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->f1=_tmpB7->name;
_tmpD4->f2=_tmpB7->tq;_tmpD4->f3=_tmpD1;_tmpD4;});_tmpD0=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpD0,({
struct Cyc_List_List*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->hd=_tmpD2;_tmpD3->tl=
0;_tmpD3;}));}return(void*)({struct Cyc_Absyn_FnType_struct*_tmpD5=_cycalloc(
sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Absyn_FnType_struct _tmpD6;_tmpD6.tag=8;
_tmpD6.f1=({struct Cyc_Absyn_FnInfo _tmpD7;_tmpD7.tvars=0;_tmpD7.effect=0;_tmpD7.ret_typ=(
void*)Cyc_Toc_typ_to_c(_tmpB4);_tmpD7.args=_tmpD0;_tmpD7.c_varargs=_tmpB6;_tmpD7.cyc_varargs=
0;_tmpD7.rgn_po=0;_tmpD7.attributes=_tmpCD;_tmpD7;});_tmpD6;});_tmpD5;});}}_LL42:
if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 9)goto _LL44;_tmpB9=((struct Cyc_Absyn_TupleType_struct*)
_tmp9E)->f1;_LL43: _tmpB9=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmpB9);{
struct _tagged_arr*_tmpD8=Cyc_Toc_add_tuple_type(_tmpB9);return Cyc_Absyn_strct(
_tmpD8);}_LL44: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 11)goto _LL46;_tmpBA=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp9E)->f1;_tmpBB=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp9E)->f2;_LL45: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpD9=
_cycalloc(sizeof(*_tmpD9));_tmpD9[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpDA;
_tmpDA.tag=11;_tmpDA.f1=(void*)_tmpBA;_tmpDA.f2=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_aggrfield_to_c,_tmpBB);_tmpDA;});_tmpD9;});_LL46: if(_tmp9E <= (void*)3?1:*((
int*)_tmp9E)!= 10)goto _LL48;_tmpBC=((struct Cyc_Absyn_AggrType_struct*)_tmp9E)->f1;
_tmpBD=(void*)_tmpBC.aggr_info;_tmpBE=_tmpBC.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*
_tmpDB=Cyc_Absyn_get_known_aggrdecl(_tmpBD);if((void*)_tmpDB->kind == (void*)1)
return Cyc_Absyn_unionq_typ(_tmpDB->name);return Cyc_Absyn_strctq(_tmpDB->name);}
_LL48: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 12)goto _LL4A;_tmpBF=((struct Cyc_Absyn_EnumType_struct*)
_tmp9E)->f1;_LL49: return t;_LL4A: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 13)goto
_LL4C;_tmpC0=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp9E)->f1;_LL4B: Cyc_Toc_enumfields_to_c(
_tmpC0);return t;_LL4C: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 16)goto _LL4E;
_tmpC1=((struct Cyc_Absyn_TypedefType_struct*)_tmp9E)->f1;_tmpC2=((struct Cyc_Absyn_TypedefType_struct*)
_tmp9E)->f2;_tmpC3=((struct Cyc_Absyn_TypedefType_struct*)_tmp9E)->f3;_tmpC4=((
struct Cyc_Absyn_TypedefType_struct*)_tmp9E)->f4;_LL4D: if(_tmpC4 == 0)return(void*)({
struct Cyc_Absyn_TypedefType_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({
struct Cyc_Absyn_TypedefType_struct _tmpDD;_tmpDD.tag=16;_tmpDD.f1=_tmpC1;_tmpDD.f2=
0;_tmpDD.f3=_tmpC3;_tmpDD.f4=0;_tmpDD;});_tmpDC;});else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_TypedefType_struct
_tmpDF;_tmpDF.tag=16;_tmpDF.f1=_tmpC1;_tmpDF.f2=0;_tmpDF.f3=_tmpC3;_tmpDF.f4=({
void**_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=Cyc_Toc_typ_to_c_array(*_tmpC4);
_tmpE0;});_tmpDF;});_tmpDE;});}_LL4E: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 14)
goto _LL50;_LL4F: goto _LL51;_LL50: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 17)goto
_LL52;_LL51: return Cyc_Absyn_uint_typ;_LL52: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)
!= 15)goto _LL54;_tmpC5=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp9E)->f1;
_LL53: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq);
_LL54: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 18)goto _LL56;_LL55:({void*_tmpE1[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpE2="Toc::typ_to_c: type translation passed a type integer";
_tag_arr(_tmpE2,sizeof(char),_get_zero_arr_size(_tmpE2,54));}),_tag_arr(_tmpE1,
sizeof(void*),0));});_LL56: if((int)_tmp9E != 2)goto _LL58;_LL57:({void*_tmpE3[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmpE4="Toc::typ_to_c: type translation passed the heap region";_tag_arr(_tmpE4,
sizeof(char),_get_zero_arr_size(_tmpE4,55));}),_tag_arr(_tmpE3,sizeof(void*),0));});
_LL58: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 19)goto _LL5A;_LL59: goto _LL5B;
_LL5A: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 20)goto _LL5C;_LL5B: goto _LL5D;
_LL5C: if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 21)goto _LL25;_LL5D:({void*_tmpE5[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpE6="Toc::typ_to_c: type translation passed an effect";_tag_arr(
_tmpE6,sizeof(char),_get_zero_arr_size(_tmpE6,49));}),_tag_arr(_tmpE5,sizeof(
void*),0));});_LL25:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmpE7=t;struct Cyc_Absyn_ArrayInfo
_tmpE8;void*_tmpE9;struct Cyc_Absyn_Tqual _tmpEA;_LL6F: if(_tmpE7 <= (void*)3?1:*((
int*)_tmpE7)!= 7)goto _LL71;_tmpE8=((struct Cyc_Absyn_ArrayType_struct*)_tmpE7)->f1;
_tmpE9=(void*)_tmpE8.elt_type;_tmpEA=_tmpE8.tq;_LL70: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ(_tmpE9,(void*)2,_tmpEA,Cyc_Absyn_false_conref),e,l);_LL71:;
_LL72: return Cyc_Absyn_cast_exp(t,e,l);_LL6E:;}static int Cyc_Toc_atomic_typ(void*t){
void*_tmpEB=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpEC;void*_tmpED;
struct Cyc_Absyn_AggrInfo _tmpEE;void*_tmpEF;struct Cyc_List_List*_tmpF0;struct Cyc_Absyn_TunionFieldInfo
_tmpF1;void*_tmpF2;struct Cyc_Absyn_Tuniondecl*_tmpF3;struct Cyc_Absyn_Tunionfield*
_tmpF4;struct Cyc_List_List*_tmpF5;_LL74: if((int)_tmpEB != 0)goto _LL76;_LL75:
return 1;_LL76: if(_tmpEB <= (void*)3?1:*((int*)_tmpEB)!= 1)goto _LL78;_LL77: return 0;
_LL78: if(_tmpEB <= (void*)3?1:*((int*)_tmpEB)!= 5)goto _LL7A;_LL79: goto _LL7B;_LL7A:
if(_tmpEB <= (void*)3?1:*((int*)_tmpEB)!= 12)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if(
_tmpEB <= (void*)3?1:*((int*)_tmpEB)!= 13)goto _LL7E;_LL7D: goto _LL7F;_LL7E: if((int)
_tmpEB != 1)goto _LL80;_LL7F: goto _LL81;_LL80: if(_tmpEB <= (void*)3?1:*((int*)_tmpEB)
!= 6)goto _LL82;_LL81: goto _LL83;_LL82: if(_tmpEB <= (void*)3?1:*((int*)_tmpEB)!= 8)
goto _LL84;_LL83: goto _LL85;_LL84: if(_tmpEB <= (void*)3?1:*((int*)_tmpEB)!= 17)goto
_LL86;_LL85: goto _LL87;_LL86: if(_tmpEB <= (void*)3?1:*((int*)_tmpEB)!= 14)goto
_LL88;_LL87: return 1;_LL88: if(_tmpEB <= (void*)3?1:*((int*)_tmpEB)!= 7)goto _LL8A;
_tmpEC=((struct Cyc_Absyn_ArrayType_struct*)_tmpEB)->f1;_tmpED=(void*)_tmpEC.elt_type;
_LL89: return Cyc_Toc_atomic_typ(_tmpED);_LL8A: if(_tmpEB <= (void*)3?1:*((int*)
_tmpEB)!= 10)goto _LL8C;_tmpEE=((struct Cyc_Absyn_AggrType_struct*)_tmpEB)->f1;
_tmpEF=(void*)_tmpEE.aggr_info;_LL8B:{void*_tmpF6=_tmpEF;_LL9B: if(*((int*)_tmpF6)
!= 0)goto _LL9D;_LL9C: return 0;_LL9D:;_LL9E: goto _LL9A;_LL9A:;}{struct Cyc_Absyn_Aggrdecl*
_tmpF7=Cyc_Absyn_get_known_aggrdecl(_tmpEF);if(_tmpF7->impl == 0)return 0;{struct
Cyc_List_List*_tmpF8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF7->impl))->fields;
for(0;_tmpF8 != 0;_tmpF8=_tmpF8->tl){if(!Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)
_tmpF8->hd)->type))return 0;}}return 1;}_LL8C: if(_tmpEB <= (void*)3?1:*((int*)
_tmpEB)!= 11)goto _LL8E;_tmpF0=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpEB)->f2;
_LL8D: for(0;_tmpF0 != 0;_tmpF0=_tmpF0->tl){if(!Cyc_Toc_atomic_typ((void*)((struct
Cyc_Absyn_Aggrfield*)_tmpF0->hd)->type))return 0;}return 1;_LL8E: if(_tmpEB <= (void*)
3?1:*((int*)_tmpEB)!= 3)goto _LL90;_tmpF1=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpEB)->f1;_tmpF2=(void*)_tmpF1.field_info;if(*((int*)_tmpF2)!= 1)goto _LL90;
_tmpF3=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpF2)->f1;_tmpF4=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpF2)->f2;_LL8F: _tmpF5=_tmpF4->typs;goto _LL91;_LL90: if(_tmpEB <= (void*)3?1:*((
int*)_tmpEB)!= 9)goto _LL92;_tmpF5=((struct Cyc_Absyn_TupleType_struct*)_tmpEB)->f1;
_LL91: for(0;_tmpF5 != 0;_tmpF5=_tmpF5->tl){if(!Cyc_Toc_atomic_typ((*((struct
_tuple4*)_tmpF5->hd)).f2))return 0;}return 1;_LL92: if(_tmpEB <= (void*)3?1:*((int*)
_tmpEB)!= 2)goto _LL94;_LL93: goto _LL95;_LL94: if(_tmpEB <= (void*)3?1:*((int*)
_tmpEB)!= 4)goto _LL96;_LL95: goto _LL97;_LL96: if(_tmpEB <= (void*)3?1:*((int*)
_tmpEB)!= 15)goto _LL98;_LL97: return 0;_LL98:;_LL99:({struct Cyc_String_pa_struct
_tmpFB;_tmpFB.tag=0;_tmpFB.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmpF9[1]={& _tmpFB};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmpFA="atomic_typ:  bad type %s";_tag_arr(_tmpFA,
sizeof(char),_get_zero_arr_size(_tmpFA,25));}),_tag_arr(_tmpF9,sizeof(void*),1));}});
_LL73:;}static int Cyc_Toc_is_void_star(void*t){void*_tmpFC=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpFD;void*_tmpFE;_LLA0: if(_tmpFC <= (void*)3?1:*((int*)
_tmpFC)!= 4)goto _LLA2;_tmpFD=((struct Cyc_Absyn_PointerType_struct*)_tmpFC)->f1;
_tmpFE=(void*)_tmpFD.elt_typ;_LLA1: {void*_tmpFF=Cyc_Tcutil_compress(_tmpFE);
_LLA5: if((int)_tmpFF != 0)goto _LLA7;_LLA6: return 1;_LLA7:;_LLA8: return 0;_LLA4:;}
_LLA2:;_LLA3: return 0;_LL9F:;}static int Cyc_Toc_is_poly_field(void*t,struct
_tagged_arr*f){void*_tmp100=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp101;void*_tmp102;struct Cyc_List_List*_tmp103;_LLAA: if(_tmp100 <= (void*)3?1:*((
int*)_tmp100)!= 10)goto _LLAC;_tmp101=((struct Cyc_Absyn_AggrType_struct*)_tmp100)->f1;
_tmp102=(void*)_tmp101.aggr_info;_LLAB: {struct Cyc_Absyn_Aggrdecl*_tmp104=Cyc_Absyn_get_known_aggrdecl(
_tmp102);if(_tmp104->impl == 0)({void*_tmp105[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp106="is_poly_field: type missing fields";
_tag_arr(_tmp106,sizeof(char),_get_zero_arr_size(_tmp106,35));}),_tag_arr(
_tmp105,sizeof(void*),0));});_tmp103=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp104->impl))->fields;goto _LLAD;}_LLAC: if(_tmp100 <= (void*)3?1:*((int*)_tmp100)
!= 11)goto _LLAE;_tmp103=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp100)->f2;
_LLAD: {struct Cyc_Absyn_Aggrfield*_tmp107=Cyc_Absyn_lookup_field(_tmp103,f);if(
_tmp107 == 0)({struct Cyc_String_pa_struct _tmp10A;_tmp10A.tag=0;_tmp10A.f1=(struct
_tagged_arr)((struct _tagged_arr)*f);{void*_tmp108[1]={& _tmp10A};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp109="is_poly_field: bad field %s";
_tag_arr(_tmp109,sizeof(char),_get_zero_arr_size(_tmp109,28));}),_tag_arr(
_tmp108,sizeof(void*),1));}});return Cyc_Toc_is_void_star((void*)_tmp107->type);}
_LLAE:;_LLAF:({struct Cyc_String_pa_struct _tmp10D;_tmp10D.tag=0;_tmp10D.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp10B[1]={&
_tmp10D};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp10C="is_poly_field: bad type %s";_tag_arr(_tmp10C,sizeof(char),
_get_zero_arr_size(_tmp10C,27));}),_tag_arr(_tmp10B,sizeof(void*),1));}});_LLA9:;}
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp10E=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp10F;struct _tagged_arr*_tmp110;struct Cyc_Absyn_Exp*
_tmp111;struct _tagged_arr*_tmp112;_LLB1: if(*((int*)_tmp10E)!= 21)goto _LLB3;
_tmp10F=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp10E)->f1;_tmp110=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp10E)->f2;_LLB2: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp10F->topt))->v,_tmp110);_LLB3: if(*((int*)_tmp10E)!= 22)goto _LLB5;
_tmp111=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp10E)->f1;_tmp112=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp10E)->f2;_LLB4: {void*_tmp113=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp111->topt))->v);struct Cyc_Absyn_PtrInfo _tmp114;void*_tmp115;
_LLB8: if(_tmp113 <= (void*)3?1:*((int*)_tmp113)!= 4)goto _LLBA;_tmp114=((struct Cyc_Absyn_PointerType_struct*)
_tmp113)->f1;_tmp115=(void*)_tmp114.elt_typ;_LLB9: return Cyc_Toc_is_poly_field(
_tmp115,_tmp112);_LLBA:;_LLBB:({struct Cyc_String_pa_struct _tmp118;_tmp118.tag=0;
_tmp118.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp111->topt))->v));{void*_tmp116[1]={& _tmp118};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmp117="is_poly_project:  bad type %s";_tag_arr(_tmp117,sizeof(char),
_get_zero_arr_size(_tmp117,30));}),_tag_arr(_tmp116,sizeof(void*),1));}});_LLB7:;}
_LLB5:;_LLB6: return 0;_LLB0:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct
Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*
_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->hd=s;_tmp119->tl=0;_tmp119;}),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmp11A=_cycalloc(sizeof(*
_tmp11A));_tmp11A->hd=s;_tmp11A->tl=0;_tmp11A;}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return
Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*
Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*_tmp11B[2];_tmp11B[1]=s;_tmp11B[0]=
rgn;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp11B,
sizeof(struct Cyc_Absyn_Exp*),2));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(
void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(
elt_type))return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*
_tmp11C[2];_tmp11C[1]=n;_tmp11C[0]=s;((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp11C,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp11D[2];
_tmp11D[1]=n;_tmp11D[0]=s;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp11D,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*
_tmp11E[3];_tmp11E[2]=n;_tmp11E[1]=s;_tmp11E[0]=rgn;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp11E,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*_tmp11F=
_cycalloc(sizeof(*_tmp11F));_tmp11F->hd=e;_tmp11F->tl=0;_tmp11F;}),0);}static
struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_tagged_arr(void*t,struct Cyc_Absyn_Exp*
sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*_tmp120=(void*)e->r;void*_tmp121;
_LLBD: if(*((int*)_tmp120)!= 0)goto _LLBF;_tmp121=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp120)->f1;if(_tmp121 <= (void*)1?1:*((int*)_tmp121)!= 5)goto _LLBF;_LLBE:
is_string=1;goto _LLBC;_LLBF:;_LLC0: goto _LLBC;_LLBC:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,
vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123[0]=({
struct Cyc_Absyn_Var_d_struct _tmp124;_tmp124.tag=0;_tmp124.f1=vd;_tmp124;});
_tmp123;}),0);_tmp122->tl=Cyc_Toc_result_decls;_tmp122;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
e,0);xplussz=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,
0),0);}{struct Cyc_Absyn_Exp*urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct
_tuple6*_tmp125[3];_tmp125[2]=({struct _tuple6*_tmp128=_cycalloc(sizeof(*_tmp128));
_tmp128->f1=0;_tmp128->f2=xplussz;_tmp128;});_tmp125[1]=({struct _tuple6*_tmp127=
_cycalloc(sizeof(*_tmp127));_tmp127->f1=0;_tmp127->f2=xexp;_tmp127;});_tmp125[0]=({
struct _tuple6*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->f1=0;_tmp126->f2=xexp;
_tmp126;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp125,sizeof(struct _tuple6*),3));}),0);return urm_exp;}}}struct Cyc_Toc_Env{
struct Cyc_Core_Opt*break_lab;struct Cyc_Core_Opt*continue_lab;struct Cyc_Core_Opt*
fallthru_info;struct Cyc_Dict_Dict*varmap;int toplevel;};static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(){return({struct Cyc_Toc_Env*_tmp129=_cycalloc(sizeof(*_tmp129));
_tmp129->break_lab=0;_tmp129->continue_lab=0;_tmp129->fallthru_info=0;_tmp129->varmap=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp);_tmp129->toplevel=1;_tmp129;});}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp12A=_cycalloc(
sizeof(*_tmp12A));_tmp12A[0]=*e;_tmp12A;});ans->toplevel=0;return ans;}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct Cyc_Toc_Env*e,struct _tuple1*x,struct
Cyc_Absyn_Exp*y){{void*_tmp12B=(*x).f1;_LLC2: if(_tmp12B <= (void*)1?1:*((int*)
_tmp12B)!= 0)goto _LLC4;_LLC3:({struct Cyc_String_pa_struct _tmp12E;_tmp12E.tag=0;
_tmp12E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(x));{
void*_tmp12C[1]={& _tmp12E};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp12D="Toc::add_varmap on Rel_n: %s\n";_tag_arr(
_tmp12D,sizeof(char),_get_zero_arr_size(_tmp12D,30));}),_tag_arr(_tmp12C,sizeof(
void*),1));}});_LLC4:;_LLC5: goto _LLC1;_LLC1:;}{struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F[0]=*e;_tmp12F;});ans->varmap=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,struct Cyc_Absyn_Exp*
v))Cyc_Dict_insert)(ans->varmap,x,y);return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp130=_cycalloc(
sizeof(*_tmp130));_tmp130[0]=*e;_tmp130;});ans->break_lab=0;ans->continue_lab=0;
return ans;}struct _tuple8{struct _tagged_arr*f1;struct Cyc_List_List*f2;struct Cyc_Dict_Dict*
f3;};static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct Cyc_Toc_Env*
e,struct _tagged_arr*break_l,struct _tagged_arr*fallthru_l,struct Cyc_List_List*
fallthru_binders,struct Cyc_Toc_Env*next_case_env){struct Cyc_List_List*_tmp131=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){_tmp131=({
struct Cyc_List_List*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp132->tl=_tmp131;_tmp132;});}_tmp131=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp131);{struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136[0]=*e;_tmp136;});
ans->break_lab=({struct Cyc_Core_Opt*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->v=
break_l;_tmp133;});ans->fallthru_info=({struct Cyc_Core_Opt*_tmp134=_cycalloc(
sizeof(*_tmp134));_tmp134->v=({struct _tuple8*_tmp135=_cycalloc(sizeof(*_tmp135));
_tmp135->f1=fallthru_l;_tmp135->f2=_tmp131;_tmp135->f3=next_case_env->varmap;
_tmp135;});_tmp134;});return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*break_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138[0]=*e;_tmp138;});ans->break_lab=({
struct Cyc_Core_Opt*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->v=break_l;
_tmp137;});ans->fallthru_info=0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*next_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B[0]=*e;_tmp13B;});ans->break_lab=0;
ans->fallthru_info=({struct Cyc_Core_Opt*_tmp139=_cycalloc(sizeof(*_tmp139));
_tmp139->v=({struct _tuple8*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->f1=
next_l;_tmp13A->f2=0;_tmp13A->f3=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp13A;});_tmp139;});
return ans;}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static
int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp13C=(void*)e->annot;
_LLC7: if(*((void**)_tmp13C)!= Cyc_CfFlowInfo_UnknownZ)goto _LLC9;_LLC8: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLC9: if(*((void**)_tmp13C)
!= Cyc_CfFlowInfo_NotZero)goto _LLCB;_LLCA: return 0;_LLCB: if(_tmp13C != Cyc_CfFlowInfo_IsZero)
goto _LLCD;_LLCC:({void*_tmp13D[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp13E="dereference of NULL pointer";
_tag_arr(_tmp13E,sizeof(char),_get_zero_arr_size(_tmp13E,28));}),_tag_arr(
_tmp13D,sizeof(void*),0));});return 0;_LLCD: if(_tmp13C != Cyc_Absyn_EmptyAnnot)
goto _LLCF;_LLCE: return 0;_LLCF: if(*((void**)_tmp13C)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LLD1;_LLD0:({void*_tmp13F[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp140="compiler oddity: pointer compared to tag type";
_tag_arr(_tmp140,sizeof(char),_get_zero_arr_size(_tmp140,46));}),_tag_arr(
_tmp13F,sizeof(void*),0));});return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLD1:;_LLD2:({void*_tmp141[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp142="need_null_check";
_tag_arr(_tmp142,sizeof(char),_get_zero_arr_size(_tmp142,16));}),_tag_arr(
_tmp141,sizeof(void*),0));});_LLC6:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e){void*_tmp143=(void*)e->annot;struct Cyc_List_List*_tmp144;
struct Cyc_List_List*_tmp145;_LLD4: if(*((void**)_tmp143)!= Cyc_CfFlowInfo_UnknownZ)
goto _LLD6;_tmp144=((struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp143)->f1;_LLD5:
return _tmp144;_LLD6: if(*((void**)_tmp143)!= Cyc_CfFlowInfo_NotZero)goto _LLD8;
_tmp145=((struct Cyc_CfFlowInfo_NotZero_struct*)_tmp143)->f1;_LLD7: return _tmp145;
_LLD8: if(_tmp143 != Cyc_CfFlowInfo_IsZero)goto _LLDA;_LLD9:({void*_tmp146[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp147="dereference of NULL pointer";
_tag_arr(_tmp147,sizeof(char),_get_zero_arr_size(_tmp147,28));}),_tag_arr(
_tmp146,sizeof(void*),0));});return 0;_LLDA: if(*((void**)_tmp143)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LLDC;_LLDB: goto _LLDD;_LLDC: if(_tmp143 != Cyc_Absyn_EmptyAnnot)goto _LLDE;
_LLDD: return 0;_LLDE:;_LLDF:({void*_tmp148[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp149="get_relns";_tag_arr(
_tmp149,sizeof(char),_get_zero_arr_size(_tmp149,10));}),_tag_arr(_tmp148,sizeof(
void*),0));});_LLD3:;}static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp14A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp14B;struct Cyc_Absyn_PtrAtts
_tmp14C;struct Cyc_Absyn_Conref*_tmp14D;struct Cyc_Absyn_Conref*_tmp14E;struct Cyc_Absyn_ArrayInfo
_tmp14F;struct Cyc_Absyn_Exp*_tmp150;_LLE1: if(_tmp14A <= (void*)3?1:*((int*)
_tmp14A)!= 4)goto _LLE3;_tmp14B=((struct Cyc_Absyn_PointerType_struct*)_tmp14A)->f1;
_tmp14C=_tmp14B.ptr_atts;_tmp14D=_tmp14C.bounds;_tmp14E=_tmp14C.zero_term;_LLE2: {
void*_tmp151=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp14D);struct Cyc_Absyn_Exp*
_tmp152;_LLE8: if((int)_tmp151 != 0)goto _LLEA;_LLE9: return 0;_LLEA: if(_tmp151 <= (
void*)1?1:*((int*)_tmp151)!= 0)goto _LLEC;_tmp152=((struct Cyc_Absyn_Upper_b_struct*)
_tmp151)->f1;_LLEB: {unsigned int _tmp154;int _tmp155;struct _tuple5 _tmp153=Cyc_Evexp_eval_const_uint_exp(
_tmp152);_tmp154=_tmp153.f1;_tmp155=_tmp153.f2;return _tmp155?i <= _tmp154: 0;}
_LLEC: if(_tmp151 <= (void*)1?1:*((int*)_tmp151)!= 1)goto _LLE7;_LLED:({void*
_tmp156[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp157="check_const_array: AbsUpper_b";_tag_arr(_tmp157,sizeof(char),
_get_zero_arr_size(_tmp157,30));}),_tag_arr(_tmp156,sizeof(void*),0));});_LLE7:;}
_LLE3: if(_tmp14A <= (void*)3?1:*((int*)_tmp14A)!= 7)goto _LLE5;_tmp14F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp14A)->f1;_tmp150=_tmp14F.num_elts;_LLE4: if(_tmp150 == 0)return 0;{unsigned int
_tmp159;int _tmp15A;struct _tuple5 _tmp158=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp150));_tmp159=_tmp158.f1;_tmp15A=_tmp158.f2;return _tmp15A?i <= 
_tmp159: 0;}_LLE5:;_LLE6: return 0;_LLE0:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp15B=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp15B->vd != y)continue;{void*_tmp15C=(void*)_tmp15B->rop;struct
Cyc_Absyn_Vardecl*_tmp15D;struct Cyc_Absyn_Vardecl*_tmp15E;_LLEF: if(*((int*)
_tmp15C)!= 2)goto _LLF1;_tmp15D=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp15C)->f1;
_LLF0: _tmp15E=_tmp15D;goto _LLF2;_LLF1: if(*((int*)_tmp15C)!= 4)goto _LLF3;_tmp15E=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp15C)->f1;_LLF2: if(_tmp15E == v)return
1;else{goto _LLEE;}_LLF3:;_LLF4: continue;_LLEE:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*
_tmp15F=(void*)e->r;void*_tmp160;struct Cyc_Absyn_Vardecl*_tmp161;void*_tmp162;
struct Cyc_Absyn_Vardecl*_tmp163;void*_tmp164;struct Cyc_Absyn_Vardecl*_tmp165;
void*_tmp166;struct Cyc_Absyn_Vardecl*_tmp167;void*_tmp168;struct Cyc_List_List*
_tmp169;struct Cyc_List_List _tmp16A;struct Cyc_Absyn_Exp*_tmp16B;_LLF6: if(*((int*)
_tmp15F)!= 1)goto _LLF8;_tmp160=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp15F)->f2;
if(_tmp160 <= (void*)1?1:*((int*)_tmp160)!= 4)goto _LLF8;_tmp161=((struct Cyc_Absyn_Pat_b_struct*)
_tmp160)->f1;_LLF7: _tmp163=_tmp161;goto _LLF9;_LLF8: if(*((int*)_tmp15F)!= 1)goto
_LLFA;_tmp162=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp15F)->f2;if(_tmp162 <= (
void*)1?1:*((int*)_tmp162)!= 3)goto _LLFA;_tmp163=((struct Cyc_Absyn_Local_b_struct*)
_tmp162)->f1;_LLF9: _tmp165=_tmp163;goto _LLFB;_LLFA: if(*((int*)_tmp15F)!= 1)goto
_LLFC;_tmp164=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp15F)->f2;if(_tmp164 <= (
void*)1?1:*((int*)_tmp164)!= 0)goto _LLFC;_tmp165=((struct Cyc_Absyn_Global_b_struct*)
_tmp164)->f1;_LLFB: _tmp167=_tmp165;goto _LLFD;_LLFC: if(*((int*)_tmp15F)!= 1)goto
_LLFE;_tmp166=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp15F)->f2;if(_tmp166 <= (
void*)1?1:*((int*)_tmp166)!= 2)goto _LLFE;_tmp167=((struct Cyc_Absyn_Param_b_struct*)
_tmp166)->f1;_LLFD: if(_tmp167->escapes)return 0;if(Cyc_Toc_check_leq_size_var(
relns,v,_tmp167))return 1;goto _LLF5;_LLFE: if(*((int*)_tmp15F)!= 3)goto _LL100;
_tmp168=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp15F)->f1;if((int)_tmp168 != 
19)goto _LL100;_tmp169=((struct Cyc_Absyn_Primop_e_struct*)_tmp15F)->f2;if(_tmp169
== 0)goto _LL100;_tmp16A=*_tmp169;_tmp16B=(struct Cyc_Absyn_Exp*)_tmp16A.hd;_LLFF:{
void*_tmp16C=(void*)_tmp16B->r;void*_tmp16D;struct Cyc_Absyn_Vardecl*_tmp16E;void*
_tmp16F;struct Cyc_Absyn_Vardecl*_tmp170;void*_tmp171;struct Cyc_Absyn_Vardecl*
_tmp172;void*_tmp173;struct Cyc_Absyn_Vardecl*_tmp174;_LL103: if(*((int*)_tmp16C)
!= 1)goto _LL105;_tmp16D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp16C)->f2;if(
_tmp16D <= (void*)1?1:*((int*)_tmp16D)!= 4)goto _LL105;_tmp16E=((struct Cyc_Absyn_Pat_b_struct*)
_tmp16D)->f1;_LL104: _tmp170=_tmp16E;goto _LL106;_LL105: if(*((int*)_tmp16C)!= 1)
goto _LL107;_tmp16F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp16C)->f2;if(
_tmp16F <= (void*)1?1:*((int*)_tmp16F)!= 3)goto _LL107;_tmp170=((struct Cyc_Absyn_Local_b_struct*)
_tmp16F)->f1;_LL106: _tmp172=_tmp170;goto _LL108;_LL107: if(*((int*)_tmp16C)!= 1)
goto _LL109;_tmp171=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp16C)->f2;if(
_tmp171 <= (void*)1?1:*((int*)_tmp171)!= 0)goto _LL109;_tmp172=((struct Cyc_Absyn_Global_b_struct*)
_tmp171)->f1;_LL108: _tmp174=_tmp172;goto _LL10A;_LL109: if(*((int*)_tmp16C)!= 1)
goto _LL10B;_tmp173=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp16C)->f2;if(
_tmp173 <= (void*)1?1:*((int*)_tmp173)!= 2)goto _LL10B;_tmp174=((struct Cyc_Absyn_Param_b_struct*)
_tmp173)->f1;_LL10A: return _tmp174 == v;_LL10B:;_LL10C: goto _LL102;_LL102:;}goto
_LLF5;_LL100:;_LL101: goto _LLF5;_LLF5:;}return 0;}static int Cyc_Toc_check_bounds(
struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*
_tmp175=(void*)a->r;void*_tmp176;struct Cyc_Absyn_Vardecl*_tmp177;void*_tmp178;
struct Cyc_Absyn_Vardecl*_tmp179;void*_tmp17A;struct Cyc_Absyn_Vardecl*_tmp17B;
void*_tmp17C;struct Cyc_Absyn_Vardecl*_tmp17D;_LL10E: if(*((int*)_tmp175)!= 1)goto
_LL110;_tmp176=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp175)->f2;if(_tmp176 <= (
void*)1?1:*((int*)_tmp176)!= 4)goto _LL110;_tmp177=((struct Cyc_Absyn_Pat_b_struct*)
_tmp176)->f1;_LL10F: _tmp179=_tmp177;goto _LL111;_LL110: if(*((int*)_tmp175)!= 1)
goto _LL112;_tmp178=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp175)->f2;if(
_tmp178 <= (void*)1?1:*((int*)_tmp178)!= 3)goto _LL112;_tmp179=((struct Cyc_Absyn_Local_b_struct*)
_tmp178)->f1;_LL111: _tmp17B=_tmp179;goto _LL113;_LL112: if(*((int*)_tmp175)!= 1)
goto _LL114;_tmp17A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp175)->f2;if(
_tmp17A <= (void*)1?1:*((int*)_tmp17A)!= 0)goto _LL114;_tmp17B=((struct Cyc_Absyn_Global_b_struct*)
_tmp17A)->f1;_LL113: _tmp17D=_tmp17B;goto _LL115;_LL114: if(*((int*)_tmp175)!= 1)
goto _LL116;_tmp17C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp175)->f2;if(
_tmp17C <= (void*)1?1:*((int*)_tmp17C)!= 2)goto _LL116;_tmp17D=((struct Cyc_Absyn_Param_b_struct*)
_tmp17C)->f1;_LL115: if(_tmp17D->escapes)return 0;inner_loop: {void*_tmp17E=(void*)
i->r;void*_tmp17F;struct Cyc_Absyn_Exp*_tmp180;void*_tmp181;void*_tmp182;int
_tmp183;void*_tmp184;void*_tmp185;int _tmp186;void*_tmp187;void*_tmp188;int
_tmp189;void*_tmp18A;struct Cyc_List_List*_tmp18B;struct Cyc_List_List _tmp18C;
struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_List_List
_tmp18F;struct Cyc_Absyn_Exp*_tmp190;void*_tmp191;struct Cyc_Absyn_Vardecl*_tmp192;
void*_tmp193;struct Cyc_Absyn_Vardecl*_tmp194;void*_tmp195;struct Cyc_Absyn_Vardecl*
_tmp196;void*_tmp197;struct Cyc_Absyn_Vardecl*_tmp198;_LL119: if(*((int*)_tmp17E)
!= 13)goto _LL11B;_tmp17F=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp17E)->f1;
_tmp180=((struct Cyc_Absyn_Cast_e_struct*)_tmp17E)->f2;_LL11A: i=_tmp180;goto
inner_loop;_LL11B: if(*((int*)_tmp17E)!= 0)goto _LL11D;_tmp181=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp17E)->f1;if(_tmp181 <= (void*)1?1:*((int*)_tmp181)!= 2)goto _LL11D;_tmp182=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp181)->f1;if((int)_tmp182 != 2)goto
_LL11D;_tmp183=((struct Cyc_Absyn_Int_c_struct*)_tmp181)->f2;_LL11C: _tmp186=
_tmp183;goto _LL11E;_LL11D: if(*((int*)_tmp17E)!= 0)goto _LL11F;_tmp184=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp17E)->f1;if(_tmp184 <= (void*)1?1:*((int*)
_tmp184)!= 2)goto _LL11F;_tmp185=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp184)->f1;
if((int)_tmp185 != 0)goto _LL11F;_tmp186=((struct Cyc_Absyn_Int_c_struct*)_tmp184)->f2;
_LL11E: return _tmp186 >= 0?Cyc_Toc_check_const_array((unsigned int)(_tmp186 + 1),(
void*)_tmp17D->type): 0;_LL11F: if(*((int*)_tmp17E)!= 0)goto _LL121;_tmp187=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp17E)->f1;if(_tmp187 <= (void*)1?1:*((int*)
_tmp187)!= 2)goto _LL121;_tmp188=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp187)->f1;
if((int)_tmp188 != 1)goto _LL121;_tmp189=((struct Cyc_Absyn_Int_c_struct*)_tmp187)->f2;
_LL120: return Cyc_Toc_check_const_array((unsigned int)(_tmp189 + 1),(void*)_tmp17D->type);
_LL121: if(*((int*)_tmp17E)!= 3)goto _LL123;_tmp18A=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp17E)->f1;if((int)_tmp18A != 4)goto _LL123;_tmp18B=((struct Cyc_Absyn_Primop_e_struct*)
_tmp17E)->f2;if(_tmp18B == 0)goto _LL123;_tmp18C=*_tmp18B;_tmp18D=(struct Cyc_Absyn_Exp*)
_tmp18C.hd;_tmp18E=_tmp18C.tl;if(_tmp18E == 0)goto _LL123;_tmp18F=*_tmp18E;_tmp190=(
struct Cyc_Absyn_Exp*)_tmp18F.hd;_LL122: return Cyc_Toc_check_leq_size(relns,
_tmp17D,_tmp190);_LL123: if(*((int*)_tmp17E)!= 1)goto _LL125;_tmp191=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp17E)->f2;if(_tmp191 <= (void*)1?1:*((int*)
_tmp191)!= 4)goto _LL125;_tmp192=((struct Cyc_Absyn_Pat_b_struct*)_tmp191)->f1;
_LL124: _tmp194=_tmp192;goto _LL126;_LL125: if(*((int*)_tmp17E)!= 1)goto _LL127;
_tmp193=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp17E)->f2;if(_tmp193 <= (void*)
1?1:*((int*)_tmp193)!= 3)goto _LL127;_tmp194=((struct Cyc_Absyn_Local_b_struct*)
_tmp193)->f1;_LL126: _tmp196=_tmp194;goto _LL128;_LL127: if(*((int*)_tmp17E)!= 1)
goto _LL129;_tmp195=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp17E)->f2;if(
_tmp195 <= (void*)1?1:*((int*)_tmp195)!= 0)goto _LL129;_tmp196=((struct Cyc_Absyn_Global_b_struct*)
_tmp195)->f1;_LL128: _tmp198=_tmp196;goto _LL12A;_LL129: if(*((int*)_tmp17E)!= 1)
goto _LL12B;_tmp197=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp17E)->f2;if(
_tmp197 <= (void*)1?1:*((int*)_tmp197)!= 2)goto _LL12B;_tmp198=((struct Cyc_Absyn_Param_b_struct*)
_tmp197)->f1;_LL12A: if(_tmp198->escapes)return 0;{struct Cyc_List_List*_tmp199=
relns;for(0;_tmp199 != 0;_tmp199=_tmp199->tl){struct Cyc_CfFlowInfo_Reln*_tmp19A=(
struct Cyc_CfFlowInfo_Reln*)_tmp199->hd;if(_tmp19A->vd == _tmp198){void*_tmp19B=(
void*)_tmp19A->rop;struct Cyc_Absyn_Vardecl*_tmp19C;struct Cyc_Absyn_Vardecl*
_tmp19D;unsigned int _tmp19E;_LL12E: if(*((int*)_tmp19B)!= 2)goto _LL130;_tmp19C=((
struct Cyc_CfFlowInfo_LessSize_struct*)_tmp19B)->f1;_LL12F: if(_tmp17D == _tmp19C)
return 1;else{goto _LL12D;}_LL130: if(*((int*)_tmp19B)!= 1)goto _LL132;_tmp19D=((
struct Cyc_CfFlowInfo_LessVar_struct*)_tmp19B)->f1;_LL131:{struct Cyc_List_List*
_tmp19F=relns;for(0;_tmp19F != 0;_tmp19F=_tmp19F->tl){struct Cyc_CfFlowInfo_Reln*
_tmp1A0=(struct Cyc_CfFlowInfo_Reln*)_tmp19F->hd;if(_tmp1A0->vd == _tmp19D){void*
_tmp1A1=(void*)_tmp1A0->rop;struct Cyc_Absyn_Vardecl*_tmp1A2;struct Cyc_Absyn_Vardecl*
_tmp1A3;unsigned int _tmp1A4;struct Cyc_Absyn_Vardecl*_tmp1A5;_LL137: if(*((int*)
_tmp1A1)!= 4)goto _LL139;_tmp1A2=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp1A1)->f1;_LL138: _tmp1A3=_tmp1A2;goto _LL13A;_LL139: if(*((int*)_tmp1A1)!= 2)
goto _LL13B;_tmp1A3=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1A1)->f1;_LL13A:
if(_tmp17D == _tmp1A3)return 1;goto _LL136;_LL13B: if(*((int*)_tmp1A1)!= 0)goto
_LL13D;_tmp1A4=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp1A1)->f1;_LL13C:
return Cyc_Toc_check_const_array(_tmp1A4,(void*)_tmp17D->type);_LL13D: if(*((int*)
_tmp1A1)!= 1)goto _LL13F;_tmp1A5=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1A1)->f1;
_LL13E: if(Cyc_Toc_check_leq_size_var(relns,_tmp17D,_tmp1A5))return 1;goto _LL136;
_LL13F:;_LL140: goto _LL136;_LL136:;}}}goto _LL12D;_LL132: if(*((int*)_tmp19B)!= 3)
goto _LL134;_tmp19E=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp19B)->f1;_LL133:
return Cyc_Toc_check_const_array(_tmp19E,(void*)_tmp17D->type);_LL134:;_LL135:
goto _LL12D;_LL12D:;}}}goto _LL118;_LL12B:;_LL12C: goto _LL118;_LL118:;}goto _LL10D;
_LL116:;_LL117: goto _LL10D;_LL10D:;}return 0;}static void*Cyc_Toc_get_c_typ(struct
Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp1A6[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1A7="Missing type in primop ";
_tag_arr(_tmp1A7,sizeof(char),_get_zero_arr_size(_tmp1A7,24));}),_tag_arr(
_tmp1A6,sizeof(void*),0));});return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp1A8[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1A9="Missing type in primop ";
_tag_arr(_tmp1A9,sizeof(char),_get_zero_arr_size(_tmp1A9,24));}),_tag_arr(
_tmp1A8,sizeof(void*),0));});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static struct _tuple4*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct
_tuple4*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA->f1=Cyc_Toc_mt_tq;_tmp1AA->f2=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp1AA;});}
static struct _tuple6*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){Cyc_Toc_exp_to_c(nv,e);return({struct _tuple6*_tmp1AB=_cycalloc(sizeof(*
_tmp1AB));_tmp1AB->f1=0;_tmp1AB->f2=e;_tmp1AB;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp1AC=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp1AC):
Cyc_Toc_malloc_ptr(_tmp1AC));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_check_null(rgnopt);Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(
r,_tmp1AC);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp1AD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp1AD != 0;_tmp1AD=_tmp1AD->tl){struct _tuple6 _tmp1AF;struct Cyc_List_List*
_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B1;struct _tuple6*_tmp1AE=(struct _tuple6*)
_tmp1AD->hd;_tmp1AF=*_tmp1AE;_tmp1B0=_tmp1AF.f1;_tmp1B1=_tmp1AF.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp1B0 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp1B0->tl != 0)({void*_tmp1B2[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp1B3="multiple designators in array";
_tag_arr(_tmp1B3,sizeof(char),_get_zero_arr_size(_tmp1B3,30));}),_tag_arr(
_tmp1B2,sizeof(void*),0));});{void*_tmp1B4=(void*)_tmp1B0->hd;void*_tmp1B5=
_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B6;_LL142: if(*((int*)_tmp1B5)!= 0)goto _LL144;
_tmp1B6=((struct Cyc_Absyn_ArrayElement_struct*)_tmp1B5)->f1;_LL143: Cyc_Toc_exp_to_c(
nv,_tmp1B6);e_index=_tmp1B6;goto _LL141;_LL144: if(*((int*)_tmp1B5)!= 1)goto _LL141;
_LL145:({void*_tmp1B7[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_unimp)(({const char*_tmp1B8="field name designators in array";_tag_arr(
_tmp1B8,sizeof(char),_get_zero_arr_size(_tmp1B8,32));}),_tag_arr(_tmp1B7,sizeof(
void*),0));});_LL141:;}}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,
e_index,0);void*_tmp1B9=(void*)_tmp1B1->r;struct Cyc_List_List*_tmp1BA;struct Cyc_Absyn_Vardecl*
_tmp1BB;struct Cyc_Absyn_Exp*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BD;int _tmp1BE;void*
_tmp1BF;struct Cyc_List_List*_tmp1C0;_LL147: if(*((int*)_tmp1B9)!= 26)goto _LL149;
_tmp1BA=((struct Cyc_Absyn_Array_e_struct*)_tmp1B9)->f1;_LL148: s=Cyc_Toc_init_array(
nv,lval,_tmp1BA,s);goto _LL146;_LL149: if(*((int*)_tmp1B9)!= 27)goto _LL14B;_tmp1BB=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1B9)->f1;_tmp1BC=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1B9)->f2;_tmp1BD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1B9)->f3;
_tmp1BE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1B9)->f4;_LL14A: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp1BB,_tmp1BC,_tmp1BD,_tmp1BE,s,0);goto _LL146;_LL14B: if(*((int*)
_tmp1B9)!= 29)goto _LL14D;_tmp1BF=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1B9)->f1;_tmp1C0=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1B9)->f2;_LL14C:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp1BF,_tmp1C0,s);goto _LL146;_LL14D:;_LL14E:
Cyc_Toc_exp_to_c(nv,_tmp1B1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp1B1,0),s,0);goto _LL146;_LL146:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp1C1=vd->name;
void*_tmp1C2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
nv,_tmp1C1,Cyc_Absyn_varb_exp(_tmp1C1,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB[0]=({struct Cyc_Absyn_Local_b_struct
_tmp1CC;_tmp1CC.tag=3;_tmp1CC.f1=vd;_tmp1CC;});_tmp1CB;}),0));struct _tuple1*max=
Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(
_tmp1C1,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp(_tmp1C1,0),Cyc_Absyn_var_exp(max,0),0);struct Cyc_Absyn_Exp*ec=
Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp1C1,0),0);struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp1C1,0),0);struct Cyc_Absyn_Stmt*
body;{void*_tmp1C3=(void*)e2->r;struct Cyc_List_List*_tmp1C4;struct Cyc_Absyn_Vardecl*
_tmp1C5;struct Cyc_Absyn_Exp*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C7;int _tmp1C8;void*
_tmp1C9;struct Cyc_List_List*_tmp1CA;_LL150: if(*((int*)_tmp1C3)!= 26)goto _LL152;
_tmp1C4=((struct Cyc_Absyn_Array_e_struct*)_tmp1C3)->f1;_LL151: body=Cyc_Toc_init_array(
nv2,lval,_tmp1C4,Cyc_Toc_skip_stmt_dl());goto _LL14F;_LL152: if(*((int*)_tmp1C3)!= 
27)goto _LL154;_tmp1C5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1C3)->f1;
_tmp1C6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1C3)->f2;_tmp1C7=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1C3)->f3;_tmp1C8=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1C3)->f4;_LL153: body=Cyc_Toc_init_comprehension(nv2,lval,_tmp1C5,_tmp1C6,
_tmp1C7,_tmp1C8,Cyc_Toc_skip_stmt_dl(),0);goto _LL14F;_LL154: if(*((int*)_tmp1C3)
!= 29)goto _LL156;_tmp1C9=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1C3)->f1;
_tmp1CA=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1C3)->f2;_LL155: body=Cyc_Toc_init_anon_struct(
nv,lval,_tmp1C9,_tmp1CA,Cyc_Toc_skip_stmt_dl());goto _LL14F;_LL156:;_LL157: Cyc_Toc_exp_to_c(
nv2,e2);body=Cyc_Absyn_assign_stmt(lval,e2,0);goto _LL14F;_LL14F:;}{struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(max,0),0),Cyc_Absyn_cast_exp(
_tmp1C2,Cyc_Absyn_uint_exp(0,0),0),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(
ex,0),0);}return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp1C1,Cyc_Absyn_uint_typ,0,s2,0),
0),s,0);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp1CD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp1CD != 0;_tmp1CD=_tmp1CD->tl){struct _tuple6 _tmp1CF;
struct Cyc_List_List*_tmp1D0;struct Cyc_Absyn_Exp*_tmp1D1;struct _tuple6*_tmp1CE=(
struct _tuple6*)_tmp1CD->hd;_tmp1CF=*_tmp1CE;_tmp1D0=_tmp1CF.f1;_tmp1D1=_tmp1CF.f2;
if(_tmp1D0 == 0)({void*_tmp1D2[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1D3="empty designator list";
_tag_arr(_tmp1D3,sizeof(char),_get_zero_arr_size(_tmp1D3,22));}),_tag_arr(
_tmp1D2,sizeof(void*),0));});if(_tmp1D0->tl != 0)({void*_tmp1D4[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1D5="too many designators in anonymous struct";
_tag_arr(_tmp1D5,sizeof(char),_get_zero_arr_size(_tmp1D5,41));}),_tag_arr(
_tmp1D4,sizeof(void*),0));});{void*_tmp1D6=(void*)_tmp1D0->hd;struct _tagged_arr*
_tmp1D7;_LL159: if(*((int*)_tmp1D6)!= 1)goto _LL15B;_tmp1D7=((struct Cyc_Absyn_FieldName_struct*)
_tmp1D6)->f1;_LL15A: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,
_tmp1D7,0);{void*_tmp1D8=(void*)_tmp1D1->r;struct Cyc_List_List*_tmp1D9;struct Cyc_Absyn_Vardecl*
_tmp1DA;struct Cyc_Absyn_Exp*_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DC;int _tmp1DD;void*
_tmp1DE;struct Cyc_List_List*_tmp1DF;_LL15E: if(*((int*)_tmp1D8)!= 26)goto _LL160;
_tmp1D9=((struct Cyc_Absyn_Array_e_struct*)_tmp1D8)->f1;_LL15F: s=Cyc_Toc_init_array(
nv,lval,_tmp1D9,s);goto _LL15D;_LL160: if(*((int*)_tmp1D8)!= 27)goto _LL162;_tmp1DA=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D8)->f1;_tmp1DB=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1D8)->f2;_tmp1DC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D8)->f3;
_tmp1DD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D8)->f4;_LL161: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp1DA,_tmp1DB,_tmp1DC,_tmp1DD,s,0);goto _LL15D;_LL162: if(*((int*)
_tmp1D8)!= 29)goto _LL164;_tmp1DE=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1D8)->f1;_tmp1DF=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1D8)->f2;_LL163:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp1DE,_tmp1DF,s);goto _LL15D;_LL164:;_LL165:
Cyc_Toc_exp_to_c(nv,_tmp1D1);if(Cyc_Toc_is_poly_field(struct_type,_tmp1D7))
_tmp1D1=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),_tmp1D1,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp1D1,0),0),s,0);goto _LL15D;
_LL15D:;}goto _LL158;}_LL15B:;_LL15C:({void*_tmp1E0[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1E1="array designator in struct";
_tag_arr(_tmp1E1,sizeof(char),_get_zero_arr_size(_tmp1E1,27));}),_tag_arr(
_tmp1E0,sizeof(void*),0));});_LL158:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(
struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
es){struct _RegionHandle _tmp1E2=_new_region("r");struct _RegionHandle*r=& _tmp1E2;
_push_region(r);{struct Cyc_List_List*_tmp1E3=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);struct _tagged_arr*_tmp1E4=Cyc_Toc_add_tuple_type(
_tmp1E3);struct _tuple1*_tmp1E5=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1E6=
Cyc_Absyn_var_exp(_tmp1E5,0);struct Cyc_Absyn_Stmt*_tmp1E7=Cyc_Absyn_exp_stmt(
_tmp1E6,0);struct Cyc_Absyn_Exp*(*_tmp1E8)(struct Cyc_Absyn_Exp*,struct _tagged_arr*,
struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp1E9=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp1E9);for(0;_tmp1E9 != 0;(_tmp1E9=_tmp1E9->tl,
-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp1E9->hd;struct Cyc_Absyn_Exp*
lval=_tmp1E8(_tmp1E6,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic?Cyc_Toc_atomic_typ((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v): 0;{void*_tmp1EA=(void*)e->r;
struct Cyc_List_List*_tmp1EB;struct Cyc_Absyn_Vardecl*_tmp1EC;struct Cyc_Absyn_Exp*
_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EE;int _tmp1EF;_LL167: if(*((int*)_tmp1EA)!= 26)
goto _LL169;_tmp1EB=((struct Cyc_Absyn_Array_e_struct*)_tmp1EA)->f1;_LL168: _tmp1E7=
Cyc_Toc_init_array(nv,lval,_tmp1EB,_tmp1E7);goto _LL166;_LL169: if(*((int*)_tmp1EA)
!= 27)goto _LL16B;_tmp1EC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1EA)->f1;
_tmp1ED=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1EA)->f2;_tmp1EE=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1EA)->f3;_tmp1EF=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1EA)->f4;_LL16A: _tmp1E7=Cyc_Toc_init_comprehension(nv,lval,_tmp1EC,_tmp1ED,
_tmp1EE,_tmp1EF,_tmp1E7,0);goto _LL166;_LL16B:;_LL16C: Cyc_Toc_exp_to_c(nv,e);
_tmp1E7=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp1E8(
_tmp1E6,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp1E7,0);goto _LL166;_LL166:;}}}{
struct Cyc_Absyn_Exp*_tmp1F0=Cyc_Toc_make_struct(nv,_tmp1E5,Cyc_Absyn_strct(
_tmp1E4),_tmp1E7,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp1F0;}};
_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct
Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp1F1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp1F2=Cyc_Absyn_var_exp(_tmp1F1,0);struct Cyc_Absyn_Stmt*
_tmp1F3=Cyc_Absyn_exp_stmt(_tmp1F2,0);struct Cyc_Absyn_Exp*(*_tmp1F4)(struct Cyc_Absyn_Exp*,
struct _tagged_arr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp1F5=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp1F6;void*
_tmp1F7;_LL16E: if(_tmp1F5 <= (void*)3?1:*((int*)_tmp1F5)!= 10)goto _LL170;_tmp1F6=((
struct Cyc_Absyn_AggrType_struct*)_tmp1F5)->f1;_tmp1F7=(void*)_tmp1F6.aggr_info;
_LL16F: ad=Cyc_Absyn_get_known_aggrdecl(_tmp1F7);goto _LL16D;_LL170:;_LL171:({void*
_tmp1F8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1F9="init_struct: bad struct type";_tag_arr(_tmp1F9,sizeof(char),
_get_zero_arr_size(_tmp1F9,29));}),_tag_arr(_tmp1F8,sizeof(void*),0));});_LL16D:;}{
struct _RegionHandle _tmp1FA=_new_region("r");struct _RegionHandle*r=& _tmp1FA;
_push_region(r);{struct Cyc_List_List*_tmp1FB=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp1FB != 0;
_tmp1FB=_tmp1FB->tl){struct _tuple6 _tmp1FD;struct Cyc_List_List*_tmp1FE;struct Cyc_Absyn_Exp*
_tmp1FF;struct _tuple6*_tmp1FC=(struct _tuple6*)_tmp1FB->hd;_tmp1FD=*_tmp1FC;
_tmp1FE=_tmp1FD.f1;_tmp1FF=_tmp1FD.f2;is_atomic=is_atomic?Cyc_Toc_atomic_typ((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1FF->topt))->v): 0;if(_tmp1FE == 0)({
void*_tmp200[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp201="empty designator list";_tag_arr(_tmp201,sizeof(char),
_get_zero_arr_size(_tmp201,22));}),_tag_arr(_tmp200,sizeof(void*),0));});if(
_tmp1FE->tl != 0){struct _tuple1*_tmp202=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp203=Cyc_Absyn_var_exp(_tmp202,0);for(0;_tmp1FE != 0;_tmp1FE=_tmp1FE->tl){void*
_tmp204=(void*)_tmp1FE->hd;struct _tagged_arr*_tmp205;_LL173: if(*((int*)_tmp204)
!= 1)goto _LL175;_tmp205=((struct Cyc_Absyn_FieldName_struct*)_tmp204)->f1;_LL174:
if(Cyc_Toc_is_poly_field(struct_type,_tmp205))_tmp203=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp203,0);_tmp1F3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp1F4(_tmp1F2,_tmp205,0),_tmp203,0),0),_tmp1F3,0);goto _LL172;_LL175:;_LL176:({
void*_tmp206[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp207="array designator in struct";_tag_arr(_tmp207,sizeof(char),
_get_zero_arr_size(_tmp207,27));}),_tag_arr(_tmp206,sizeof(void*),0));});_LL172:;}
Cyc_Toc_exp_to_c(nv,_tmp1FF);_tmp1F3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp203,_tmp1FF,0),0),_tmp1F3,0);}else{void*_tmp208=(void*)_tmp1FE->hd;struct
_tagged_arr*_tmp209;_LL178: if(*((int*)_tmp208)!= 1)goto _LL17A;_tmp209=((struct
Cyc_Absyn_FieldName_struct*)_tmp208)->f1;_LL179: {struct Cyc_Absyn_Exp*lval=
_tmp1F4(_tmp1F2,_tmp209,0);{void*_tmp20A=(void*)_tmp1FF->r;struct Cyc_List_List*
_tmp20B;struct Cyc_Absyn_Vardecl*_tmp20C;struct Cyc_Absyn_Exp*_tmp20D;struct Cyc_Absyn_Exp*
_tmp20E;int _tmp20F;void*_tmp210;struct Cyc_List_List*_tmp211;_LL17D: if(*((int*)
_tmp20A)!= 26)goto _LL17F;_tmp20B=((struct Cyc_Absyn_Array_e_struct*)_tmp20A)->f1;
_LL17E: _tmp1F3=Cyc_Toc_init_array(nv,lval,_tmp20B,_tmp1F3);goto _LL17C;_LL17F: if(*((
int*)_tmp20A)!= 27)goto _LL181;_tmp20C=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp20A)->f1;_tmp20D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp20A)->f2;
_tmp20E=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp20A)->f3;_tmp20F=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp20A)->f4;_LL180: _tmp1F3=Cyc_Toc_init_comprehension(
nv,lval,_tmp20C,_tmp20D,_tmp20E,_tmp20F,_tmp1F3,0);goto _LL17C;_LL181: if(*((int*)
_tmp20A)!= 29)goto _LL183;_tmp210=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp20A)->f1;_tmp211=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp20A)->f2;_LL182:
_tmp1F3=Cyc_Toc_init_anon_struct(nv,lval,_tmp210,_tmp211,_tmp1F3);goto _LL17C;
_LL183:;_LL184: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1FF->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp1FF);if(Cyc_Toc_is_poly_field(
struct_type,_tmp209)?!was_ptr_type: 0)_tmp1FF=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp1FF,0);if(has_exists)_tmp1FF=Cyc_Absyn_cast_exp((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(Cyc_Absyn_lookup_decl_field(ad,_tmp209)))->type,_tmp1FF,0);_tmp1F3=
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp1FF,0),0),
_tmp1F3,0);goto _LL17C;}_LL17C:;}goto _LL177;}_LL17A:;_LL17B:({void*_tmp212[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmp213="array designator in struct";_tag_arr(_tmp213,sizeof(char),
_get_zero_arr_size(_tmp213,27));}),_tag_arr(_tmp212,sizeof(void*),0));});_LL177:;}}}{
struct Cyc_Absyn_Exp*_tmp214=Cyc_Toc_make_struct(nv,_tmp1F1,Cyc_Absyn_strctq(tdn),
_tmp1F3,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp214;};_pop_region(r);}}
struct _tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple9*pr){return Cyc_Absyn_assignop_exp(
el,(*pr).f1,(*pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct
Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp215=_cycalloc(sizeof(*_tmp215));
_tmp215[0]=({struct Cyc_Absyn_Increment_e_struct _tmp216;_tmp216.tag=5;_tmp216.f1=
e1;_tmp216.f2=(void*)incr;_tmp216;});_tmp215;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp217=(void*)e1->r;struct Cyc_Absyn_Stmt*_tmp218;void*_tmp219;struct Cyc_Absyn_Exp*
_tmp21A;struct Cyc_Absyn_Exp*_tmp21B;struct _tagged_arr*_tmp21C;_LL186: if(*((int*)
_tmp217)!= 35)goto _LL188;_tmp218=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp217)->f1;
_LL187: Cyc_Toc_lvalue_assign_stmt(_tmp218,fs,f,f_env);goto _LL185;_LL188: if(*((
int*)_tmp217)!= 13)goto _LL18A;_tmp219=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp217)->f1;_tmp21A=((struct Cyc_Absyn_Cast_e_struct*)_tmp217)->f2;_LL189: Cyc_Toc_lvalue_assign(
_tmp21A,fs,f,f_env);goto _LL185;_LL18A: if(*((int*)_tmp217)!= 21)goto _LL18C;
_tmp21B=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp217)->f1;_tmp21C=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp217)->f2;_LL18B:(void*)(e1->r=(void*)((void*)_tmp21B->r));Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp21D=_cycalloc(sizeof(*_tmp21D));
_tmp21D->hd=_tmp21C;_tmp21D->tl=fs;_tmp21D;}),f,f_env);goto _LL185;_LL18C:;_LL18D: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _tagged_arr*)fs->hd,e1_copy->loc);}(
void*)(e1->r=(void*)((void*)(f(e1_copy,f_env))->r));goto _LL185;}_LL185:;}static
void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp21E=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp21F;struct Cyc_Absyn_Decl*_tmp220;struct Cyc_Absyn_Stmt*
_tmp221;struct Cyc_Absyn_Stmt*_tmp222;_LL18F: if(_tmp21E <= (void*)1?1:*((int*)
_tmp21E)!= 0)goto _LL191;_tmp21F=((struct Cyc_Absyn_Exp_s_struct*)_tmp21E)->f1;
_LL190: Cyc_Toc_lvalue_assign(_tmp21F,fs,f,f_env);goto _LL18E;_LL191: if(_tmp21E <= (
void*)1?1:*((int*)_tmp21E)!= 12)goto _LL193;_tmp220=((struct Cyc_Absyn_Decl_s_struct*)
_tmp21E)->f1;_tmp221=((struct Cyc_Absyn_Decl_s_struct*)_tmp21E)->f2;_LL192: Cyc_Toc_lvalue_assign_stmt(
_tmp221,fs,f,f_env);goto _LL18E;_LL193: if(_tmp21E <= (void*)1?1:*((int*)_tmp21E)!= 
1)goto _LL195;_tmp222=((struct Cyc_Absyn_Seq_s_struct*)_tmp21E)->f2;_LL194: Cyc_Toc_lvalue_assign_stmt(
_tmp222,fs,f,f_env);goto _LL18E;_LL195:;_LL196:({struct Cyc_String_pa_struct
_tmp225;_tmp225.tag=0;_tmp225.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_stmt2string(
s));{void*_tmp223[1]={& _tmp225};Cyc_Toc_toc_impos(({const char*_tmp224="lvalue_assign_stmt: %s";
_tag_arr(_tmp224,sizeof(char),_get_zero_arr_size(_tmp224,23));}),_tag_arr(
_tmp223,sizeof(void*),1));}});_LL18E:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(
struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({
struct Cyc_List_List*_tmp226=_region_malloc(r2,sizeof(*_tmp226));_tmp226->hd=(
void*)f((void*)x->hd,env);_tmp226->tl=0;_tmp226;});prev=result;for(x=x->tl;x != 0;
x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp227=_region_malloc(r2,sizeof(*_tmp227));_tmp227->hd=(void*)f((void*)x->hd,
env);_tmp227->tl=0;_tmp227;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,
x);}static struct _tuple6*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({struct
_tuple6*_tmp228=_cycalloc(sizeof(*_tmp228));_tmp228->f1=0;_tmp228->f2=e;_tmp228;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp229=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp22A;_LL198: if(_tmp229 <= (void*)3?1:*((int*)_tmp229)
!= 4)goto _LL19A;_tmp22A=((struct Cyc_Absyn_PointerType_struct*)_tmp229)->f1;
_LL199: return _tmp22A;_LL19A:;_LL19B:({void*_tmp22B[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp22C="get_ptr_typ: not a pointer!";
_tag_arr(_tmp22C,sizeof(char),_get_zero_arr_size(_tmp22C,28));}),_tag_arr(
_tmp22B,sizeof(void*),0));});_LL197:;}static int Cyc_Toc_is_zero_ptr_type(void*t,
void**ptr_type,int*is_fat,void**elt_type){void*_tmp22D=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp22E;void*_tmp22F;struct Cyc_Absyn_PtrAtts _tmp230;
struct Cyc_Absyn_Conref*_tmp231;struct Cyc_Absyn_Conref*_tmp232;struct Cyc_Absyn_ArrayInfo
_tmp233;void*_tmp234;struct Cyc_Absyn_Tqual _tmp235;struct Cyc_Absyn_Exp*_tmp236;
struct Cyc_Absyn_Conref*_tmp237;_LL19D: if(_tmp22D <= (void*)3?1:*((int*)_tmp22D)!= 
4)goto _LL19F;_tmp22E=((struct Cyc_Absyn_PointerType_struct*)_tmp22D)->f1;_tmp22F=(
void*)_tmp22E.elt_typ;_tmp230=_tmp22E.ptr_atts;_tmp231=_tmp230.bounds;_tmp232=
_tmp230.zero_term;_LL19E: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp232)){*ptr_type=t;*elt_type=_tmp22F;{void*_tmp238=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp231);_LL1A4: if((int)_tmp238 != 0)goto _LL1A6;_LL1A5:*is_fat=1;goto _LL1A3;
_LL1A6:;_LL1A7:*is_fat=0;goto _LL1A3;_LL1A3:;}return 1;}else{return 0;}_LL19F: if(
_tmp22D <= (void*)3?1:*((int*)_tmp22D)!= 7)goto _LL1A1;_tmp233=((struct Cyc_Absyn_ArrayType_struct*)
_tmp22D)->f1;_tmp234=(void*)_tmp233.elt_type;_tmp235=_tmp233.tq;_tmp236=_tmp233.num_elts;
_tmp237=_tmp233.zero_term;_LL1A0: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp237)){*elt_type=_tmp234;*is_fat=0;*ptr_type=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp239=_cycalloc(sizeof(*_tmp239));_tmp239[0]=({struct Cyc_Absyn_PointerType_struct
_tmp23A;_tmp23A.tag=4;_tmp23A.f1=({struct Cyc_Absyn_PtrInfo _tmp23B;_tmp23B.elt_typ=(
void*)_tmp234;_tmp23B.elt_tq=_tmp235;_tmp23B.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp23C;_tmp23C.rgn=(void*)((void*)2);_tmp23C.nullable=Cyc_Absyn_false_conref;
_tmp23C.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp23D=_cycalloc(sizeof(*_tmp23D));_tmp23D[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp23E;_tmp23E.tag=0;_tmp23E.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp236);
_tmp23E;});_tmp23D;}));_tmp23C.zero_term=_tmp237;_tmp23C;});_tmp23B;});_tmp23A;});
_tmp239;});return 1;}else{return 0;}_LL1A1:;_LL1A2: return 0;_LL19C:;}static int Cyc_Toc_is_zero_ptr_deref(
struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){void*_tmp23F=(
void*)e1->r;struct Cyc_Absyn_Exp*_tmp240;struct Cyc_Absyn_Exp*_tmp241;struct Cyc_Absyn_Exp*
_tmp242;struct Cyc_Absyn_Exp*_tmp243;struct Cyc_Absyn_Exp*_tmp244;struct Cyc_Absyn_Exp*
_tmp245;_LL1A9: if(*((int*)_tmp23F)!= 13)goto _LL1AB;_LL1AA:({struct Cyc_String_pa_struct
_tmp248;_tmp248.tag=0;_tmp248.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(
e1));{void*_tmp246[1]={& _tmp248};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Toc_toc_impos)(({const char*_tmp247="we have a cast in a lhs:  %s";
_tag_arr(_tmp247,sizeof(char),_get_zero_arr_size(_tmp247,29));}),_tag_arr(
_tmp246,sizeof(void*),1));}});_LL1AB: if(*((int*)_tmp23F)!= 20)goto _LL1AD;_tmp240=((
struct Cyc_Absyn_Deref_e_struct*)_tmp23F)->f1;_LL1AC: _tmp241=_tmp240;goto _LL1AE;
_LL1AD: if(*((int*)_tmp23F)!= 23)goto _LL1AF;_tmp241=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp23F)->f1;_LL1AE: return Cyc_Toc_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp241->topt))->v,ptr_type,is_fat,elt_type);_LL1AF: if(*((int*)
_tmp23F)!= 22)goto _LL1B1;_tmp242=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp23F)->f1;
_LL1B0: _tmp243=_tmp242;goto _LL1B2;_LL1B1: if(*((int*)_tmp23F)!= 21)goto _LL1B3;
_tmp243=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp23F)->f1;_LL1B2: if(Cyc_Toc_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp243->topt))->v,ptr_type,is_fat,
elt_type))({struct Cyc_String_pa_struct _tmp24B;_tmp24B.tag=0;_tmp24B.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e1));{void*_tmp249[1]={&
_tmp24B};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp24A="found zero pointer aggregate member assignment: %s";_tag_arr(
_tmp24A,sizeof(char),_get_zero_arr_size(_tmp24A,51));}),_tag_arr(_tmp249,sizeof(
void*),1));}});return 0;_LL1B3: if(*((int*)_tmp23F)!= 12)goto _LL1B5;_tmp244=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp23F)->f1;_LL1B4: _tmp245=_tmp244;goto
_LL1B6;_LL1B5: if(*((int*)_tmp23F)!= 11)goto _LL1B7;_tmp245=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp23F)->f1;_LL1B6: if(Cyc_Toc_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp245->topt))->v,ptr_type,is_fat,elt_type))({struct Cyc_String_pa_struct
_tmp24E;_tmp24E.tag=0;_tmp24E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(
e1));{void*_tmp24C[1]={& _tmp24E};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Toc_toc_impos)(({const char*_tmp24D="found zero pointer instantiate/noinstantiate: %s";
_tag_arr(_tmp24D,sizeof(char),_get_zero_arr_size(_tmp24D,49));}),_tag_arr(
_tmp24C,sizeof(void*),1));}});return 0;_LL1B7: if(*((int*)_tmp23F)!= 1)goto _LL1B9;
_LL1B8: return 0;_LL1B9:;_LL1BA:({struct Cyc_String_pa_struct _tmp251;_tmp251.tag=0;
_tmp251.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e1));{
void*_tmp24F[1]={& _tmp251};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp250="found bad lhs in is_zero_ptr_deref: %s";
_tag_arr(_tmp250,sizeof(char),_get_zero_arr_size(_tmp250,39));}),_tag_arr(
_tmp24F,sizeof(void*),1));}});_LL1A8:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(
void*t){struct Cyc_Absyn_Exp*res;{void*_tmp252=Cyc_Tcutil_compress(t);void*
_tmp253;void*_tmp254;void*_tmp255;void*_tmp256;void*_tmp257;void*_tmp258;void*
_tmp259;void*_tmp25A;_LL1BC: if(_tmp252 <= (void*)3?1:*((int*)_tmp252)!= 4)goto
_LL1BE;_LL1BD: res=Cyc_Absyn_null_exp(0);goto _LL1BB;_LL1BE: if(_tmp252 <= (void*)3?
1:*((int*)_tmp252)!= 5)goto _LL1C0;_tmp253=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp252)->f1;_tmp254=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp252)->f2;if((
int)_tmp254 != 0)goto _LL1C0;_LL1BF: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Char_c_struct*
_tmp25B=_cycalloc(sizeof(*_tmp25B));_tmp25B[0]=({struct Cyc_Absyn_Char_c_struct
_tmp25C;_tmp25C.tag=0;_tmp25C.f1=(void*)_tmp253;_tmp25C.f2='\000';_tmp25C;});
_tmp25B;}),0);goto _LL1BB;_LL1C0: if(_tmp252 <= (void*)3?1:*((int*)_tmp252)!= 5)
goto _LL1C2;_tmp255=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp252)->f1;_tmp256=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp252)->f2;if((int)_tmp256 != 1)goto
_LL1C2;_LL1C1: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Short_c_struct*
_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D[0]=({struct Cyc_Absyn_Short_c_struct
_tmp25E;_tmp25E.tag=1;_tmp25E.f1=(void*)_tmp255;_tmp25E.f2=0;_tmp25E;});_tmp25D;}),
0);goto _LL1BB;_LL1C2: if(_tmp252 <= (void*)3?1:*((int*)_tmp252)!= 12)goto _LL1C4;
_LL1C3: goto _LL1C5;_LL1C4: if(_tmp252 <= (void*)3?1:*((int*)_tmp252)!= 13)goto
_LL1C6;_LL1C5: _tmp257=(void*)1;goto _LL1C7;_LL1C6: if(_tmp252 <= (void*)3?1:*((int*)
_tmp252)!= 5)goto _LL1C8;_tmp257=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp252)->f1;
_tmp258=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp252)->f2;if((int)_tmp258 != 
2)goto _LL1C8;_LL1C7: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*
_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F[0]=({struct Cyc_Absyn_Int_c_struct
_tmp260;_tmp260.tag=2;_tmp260.f1=(void*)_tmp257;_tmp260.f2=0;_tmp260;});_tmp25F;}),
0);goto _LL1BB;_LL1C8: if(_tmp252 <= (void*)3?1:*((int*)_tmp252)!= 5)goto _LL1CA;
_tmp259=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp252)->f1;_tmp25A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp252)->f2;if((int)_tmp25A != 3)goto _LL1CA;
_LL1C9: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_LongLong_c_struct*
_tmp261=_cycalloc(sizeof(*_tmp261));_tmp261[0]=({struct Cyc_Absyn_LongLong_c_struct
_tmp262;_tmp262.tag=3;_tmp262.f1=(void*)_tmp259;_tmp262.f2=(long long)0;_tmp262;});
_tmp261;}),0);goto _LL1BB;_LL1CA: if((int)_tmp252 != 1)goto _LL1CC;_LL1CB: goto _LL1CD;
_LL1CC: if(_tmp252 <= (void*)3?1:*((int*)_tmp252)!= 6)goto _LL1CE;_LL1CD: res=Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Float_c_struct*_tmp263=_cycalloc(sizeof(*_tmp263));
_tmp263[0]=({struct Cyc_Absyn_Float_c_struct _tmp264;_tmp264.tag=4;_tmp264.f1=({
const char*_tmp265="0.0";_tag_arr(_tmp265,sizeof(char),_get_zero_arr_size(_tmp265,
4));});_tmp264;});_tmp263;}),0);goto _LL1BB;_LL1CE:;_LL1CF:({struct Cyc_String_pa_struct
_tmp268;_tmp268.tag=0;_tmp268.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp266[1]={& _tmp268};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Toc_toc_impos)(({const char*_tmp267="found non-zero type %s in generate_zero";
_tag_arr(_tmp267,sizeof(char),_get_zero_arr_size(_tmp267,40));}),_tag_arr(
_tmp266,sizeof(void*),1));}});_LL1BB:;}res->topt=({struct Cyc_Core_Opt*_tmp269=
_cycalloc(sizeof(*_tmp269));_tmp269->v=(void*)t;_tmp269;});return res;}static void
Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,
int is_fat,void*elt_type){void*_tmp26A=Cyc_Absyn_tagged_typ(elt_type,(void*)2,Cyc_Toc_mt_tq,
Cyc_Absyn_true_conref);void*_tmp26B=Cyc_Toc_typ_to_c(elt_type);void*_tmp26C=Cyc_Toc_typ_to_c(
_tmp26A);void*_tmp26D=Cyc_Absyn_cstar_typ(_tmp26B,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmp26E=({struct Cyc_Core_Opt*_tmp2A8=_cycalloc(sizeof(*_tmp2A8));_tmp2A8->v=(
void*)_tmp26D;_tmp2A8;});struct Cyc_Absyn_Exp*xinit;{void*_tmp26F=(void*)e1->r;
struct Cyc_Absyn_Exp*_tmp270;struct Cyc_Absyn_Exp*_tmp271;struct Cyc_Absyn_Exp*
_tmp272;_LL1D1: if(*((int*)_tmp26F)!= 20)goto _LL1D3;_tmp270=((struct Cyc_Absyn_Deref_e_struct*)
_tmp26F)->f1;_LL1D2: if(!is_fat)_tmp270=Cyc_Absyn_cast_exp(_tmp26A,_tmp270,0);
_tmp270->topt=({struct Cyc_Core_Opt*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->v=(
void*)_tmp26A;_tmp273;});Cyc_Toc_exp_to_c(nv,_tmp270);xinit=_tmp270;goto _LL1D0;
_LL1D3: if(*((int*)_tmp26F)!= 23)goto _LL1D5;_tmp271=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp26F)->f1;_tmp272=((struct Cyc_Absyn_Subscript_e_struct*)_tmp26F)->f2;_LL1D4:
if(!is_fat)_tmp271=Cyc_Absyn_cast_exp(_tmp26A,_tmp271,0);_tmp271->topt=({struct
Cyc_Core_Opt*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274->v=(void*)_tmp26A;
_tmp274;});Cyc_Toc_exp_to_c(nv,_tmp271);Cyc_Toc_exp_to_c(nv,_tmp272);xinit=Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp275[3];_tmp275[2]=_tmp272;
_tmp275[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp275[0]=
_tmp271;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp275,sizeof(struct Cyc_Absyn_Exp*),3));}),0);goto _LL1D0;_LL1D5:;_LL1D6:({void*
_tmp276[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp277="found bad lhs for zero-terminated pointer assignment";
_tag_arr(_tmp277,sizeof(char),_get_zero_arr_size(_tmp277,53));}),_tag_arr(
_tmp276,sizeof(void*),0));});_LL1D0:;}{struct _tuple1*_tmp278=Cyc_Toc_temp_var();
nv=Cyc_Toc_add_varmap(nv,_tmp278,Cyc_Absyn_var_exp(_tmp278,0));{struct Cyc_Absyn_Vardecl*
_tmp279=({struct Cyc_Absyn_Vardecl*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7->sc=(
void*)((void*)2);_tmp2A7->name=_tmp278;_tmp2A7->tq=Cyc_Toc_mt_tq;_tmp2A7->type=(
void*)_tmp26C;_tmp2A7->initializer=(struct Cyc_Absyn_Exp*)xinit;_tmp2A7->rgn=0;
_tmp2A7->attributes=0;_tmp2A7->escapes=0;_tmp2A7;});struct Cyc_Absyn_Local_b_struct*
_tmp27A=({struct Cyc_Absyn_Local_b_struct*_tmp2A5=_cycalloc(sizeof(*_tmp2A5));
_tmp2A5[0]=({struct Cyc_Absyn_Local_b_struct _tmp2A6;_tmp2A6.tag=3;_tmp2A6.f1=
_tmp279;_tmp2A6;});_tmp2A5;});struct Cyc_Absyn_Exp*_tmp27B=Cyc_Absyn_varb_exp(
_tmp278,(void*)_tmp27A,0);_tmp27B->topt=({struct Cyc_Core_Opt*_tmp27C=_cycalloc(
sizeof(*_tmp27C));_tmp27C->v=(void*)_tmp26A;_tmp27C;});{struct Cyc_Absyn_Exp*
_tmp27D=Cyc_Absyn_deref_exp(_tmp27B,0);_tmp27D->topt=({struct Cyc_Core_Opt*
_tmp27E=_cycalloc(sizeof(*_tmp27E));_tmp27E->v=(void*)elt_type;_tmp27E;});Cyc_Toc_exp_to_c(
nv,_tmp27D);{struct _tuple1*_tmp27F=Cyc_Toc_temp_var();nv=Cyc_Toc_add_varmap(nv,
_tmp27F,Cyc_Absyn_var_exp(_tmp27F,0));{struct Cyc_Absyn_Vardecl*_tmp280=({struct
Cyc_Absyn_Vardecl*_tmp2A4=_cycalloc(sizeof(*_tmp2A4));_tmp2A4->sc=(void*)((void*)
2);_tmp2A4->name=_tmp27F;_tmp2A4->tq=Cyc_Toc_mt_tq;_tmp2A4->type=(void*)_tmp26B;
_tmp2A4->initializer=(struct Cyc_Absyn_Exp*)_tmp27D;_tmp2A4->rgn=0;_tmp2A4->attributes=
0;_tmp2A4->escapes=0;_tmp2A4;});struct Cyc_Absyn_Local_b_struct*_tmp281=({struct
Cyc_Absyn_Local_b_struct*_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2[0]=({struct
Cyc_Absyn_Local_b_struct _tmp2A3;_tmp2A3.tag=3;_tmp2A3.f1=_tmp280;_tmp2A3;});
_tmp2A2;});struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*
_tmp282=Cyc_Absyn_varb_exp(_tmp27F,(void*)_tmp281,0);_tmp282->topt=_tmp27D->topt;
z_init=Cyc_Absyn_prim2_exp((void*)popt->v,_tmp282,e2,0);z_init->topt=_tmp282->topt;}
Cyc_Toc_exp_to_c(nv,z_init);{struct _tuple1*_tmp283=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp284=({struct Cyc_Absyn_Vardecl*_tmp2A1=_cycalloc(sizeof(*_tmp2A1));_tmp2A1->sc=(
void*)((void*)2);_tmp2A1->name=_tmp283;_tmp2A1->tq=Cyc_Toc_mt_tq;_tmp2A1->type=(
void*)_tmp26B;_tmp2A1->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp2A1->rgn=0;
_tmp2A1->attributes=0;_tmp2A1->escapes=0;_tmp2A1;});struct Cyc_Absyn_Local_b_struct*
_tmp285=({struct Cyc_Absyn_Local_b_struct*_tmp29F=_cycalloc(sizeof(*_tmp29F));
_tmp29F[0]=({struct Cyc_Absyn_Local_b_struct _tmp2A0;_tmp2A0.tag=3;_tmp2A0.f1=
_tmp284;_tmp2A0;});_tmp29F;});nv=Cyc_Toc_add_varmap(nv,_tmp283,Cyc_Absyn_var_exp(
_tmp283,0));{struct Cyc_Absyn_Exp*_tmp286=Cyc_Absyn_varb_exp(_tmp27F,(void*)
_tmp281,0);_tmp286->topt=_tmp27D->topt;{struct Cyc_Absyn_Exp*_tmp287=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp288=Cyc_Absyn_prim2_exp((void*)5,_tmp286,
_tmp287,0);_tmp288->topt=({struct Cyc_Core_Opt*_tmp289=_cycalloc(sizeof(*_tmp289));
_tmp289->v=(void*)Cyc_Absyn_sint_typ;_tmp289;});Cyc_Toc_exp_to_c(nv,_tmp288);{
struct Cyc_Absyn_Exp*_tmp28A=Cyc_Absyn_varb_exp(_tmp283,(void*)_tmp285,0);_tmp28A->topt=
_tmp27D->topt;{struct Cyc_Absyn_Exp*_tmp28B=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp28C=Cyc_Absyn_prim2_exp((void*)6,_tmp28A,_tmp28B,0);
_tmp28C->topt=({struct Cyc_Core_Opt*_tmp28D=_cycalloc(sizeof(*_tmp28D));_tmp28D->v=(
void*)Cyc_Absyn_sint_typ;_tmp28D;});Cyc_Toc_exp_to_c(nv,_tmp28C);{struct Cyc_Absyn_Exp*
_tmp28E=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp29E[2];_tmp29E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp29E[0]=Cyc_Absyn_varb_exp(_tmp278,(void*)_tmp27A,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp29E,sizeof(struct Cyc_Absyn_Exp*),
2));}),0),Cyc_Absyn_uint_exp(1,0),0);struct Cyc_Absyn_Exp*_tmp28F=Cyc_Absyn_and_exp(
_tmp28E,Cyc_Absyn_and_exp(_tmp288,_tmp28C,0),0);struct Cyc_Absyn_Stmt*_tmp290=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*
_tmp291=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_varb_exp(_tmp278,(void*)_tmp27A,0),
Cyc_Toc_curr_sp,0);_tmp291=Cyc_Absyn_cast_exp(_tmp26D,_tmp291,0);{struct Cyc_Absyn_Exp*
_tmp292=Cyc_Absyn_deref_exp(_tmp291,0);struct Cyc_Absyn_Exp*_tmp293=Cyc_Absyn_assign_exp(
_tmp292,Cyc_Absyn_var_exp(_tmp283,0),0);struct Cyc_Absyn_Stmt*_tmp294=Cyc_Absyn_exp_stmt(
_tmp293,0);_tmp294=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp28F,_tmp290,
Cyc_Absyn_skip_stmt(0),0),_tmp294,0);_tmp294=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*
_tmp295=_cycalloc(sizeof(*_tmp295));_tmp295->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296[0]=({struct Cyc_Absyn_Var_d_struct
_tmp297;_tmp297.tag=0;_tmp297.f1=_tmp284;_tmp297;});_tmp296;}));_tmp295->loc=0;
_tmp295;}),_tmp294,0);_tmp294=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp298=
_cycalloc(sizeof(*_tmp298));_tmp298->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp299=_cycalloc(sizeof(*_tmp299));_tmp299[0]=({struct Cyc_Absyn_Var_d_struct
_tmp29A;_tmp29A.tag=0;_tmp29A.f1=_tmp280;_tmp29A;});_tmp299;}));_tmp298->loc=0;
_tmp298;}),_tmp294,0);_tmp294=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp29B=
_cycalloc(sizeof(*_tmp29B));_tmp29B->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C[0]=({struct Cyc_Absyn_Var_d_struct
_tmp29D;_tmp29D.tag=0;_tmp29D.f1=_tmp279;_tmp29D;});_tmp29C;}));_tmp29B->loc=0;
_tmp29B;}),_tmp294,0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp294,0))->r));}}}}}}}}}}}}}
struct _tuple10{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple11{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp2A9=(void*)e->r;if(e->topt
== 0)({struct Cyc_String_pa_struct _tmp2AC;_tmp2AC.tag=0;_tmp2AC.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e));{void*_tmp2AA[1]={&
_tmp2AC};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp2AB="exp_to_c: no type for %s";_tag_arr(_tmp2AB,sizeof(char),
_get_zero_arr_size(_tmp2AB,25));}),_tag_arr(_tmp2AA,sizeof(void*),1));}});{void*
old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp2AD=
_tmp2A9;void*_tmp2AE;struct _tuple1*_tmp2AF;void*_tmp2B0;struct _tuple1*_tmp2B1;
void*_tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B4;void*
_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B6;struct Cyc_Core_Opt*_tmp2B7;struct Cyc_Absyn_Exp*
_tmp2B8;struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_Absyn_Exp*_tmp2BA;struct Cyc_Absyn_Exp*
_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BD;struct Cyc_Absyn_Exp*
_tmp2BE;struct Cyc_List_List*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2C0;struct Cyc_List_List*
_tmp2C1;struct Cyc_Absyn_VarargCallInfo*_tmp2C2;struct Cyc_Absyn_Exp*_tmp2C3;
struct Cyc_List_List*_tmp2C4;struct Cyc_Absyn_VarargCallInfo*_tmp2C5;struct Cyc_Absyn_VarargCallInfo
_tmp2C6;int _tmp2C7;struct Cyc_List_List*_tmp2C8;struct Cyc_Absyn_VarargInfo*
_tmp2C9;struct Cyc_Absyn_Exp*_tmp2CA;struct Cyc_Absyn_Exp*_tmp2CB;struct Cyc_Absyn_Exp*
_tmp2CC;struct Cyc_List_List*_tmp2CD;void*_tmp2CE;void**_tmp2CF;struct Cyc_Absyn_Exp*
_tmp2D0;struct Cyc_Absyn_Exp*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D2;struct Cyc_Absyn_Exp*
_tmp2D3;struct Cyc_Absyn_Exp*_tmp2D4;void*_tmp2D5;void*_tmp2D6;void*_tmp2D7;
struct _tagged_arr*_tmp2D8;void*_tmp2D9;void*_tmp2DA;unsigned int _tmp2DB;struct
Cyc_Absyn_Exp*_tmp2DC;struct Cyc_Absyn_Exp*_tmp2DD;struct _tagged_arr*_tmp2DE;
struct Cyc_Absyn_Exp*_tmp2DF;struct _tagged_arr*_tmp2E0;struct Cyc_Absyn_Exp*
_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E2;struct Cyc_List_List*_tmp2E3;struct Cyc_List_List*
_tmp2E4;struct Cyc_Absyn_Vardecl*_tmp2E5;struct Cyc_Absyn_Exp*_tmp2E6;struct Cyc_Absyn_Exp*
_tmp2E7;int _tmp2E8;struct _tuple1*_tmp2E9;struct Cyc_List_List*_tmp2EA;struct Cyc_List_List*
_tmp2EB;struct Cyc_Absyn_Aggrdecl*_tmp2EC;void*_tmp2ED;struct Cyc_List_List*
_tmp2EE;struct Cyc_List_List*_tmp2EF;struct Cyc_Absyn_Tuniondecl*_tmp2F0;struct Cyc_Absyn_Tunionfield*
_tmp2F1;struct Cyc_List_List*_tmp2F2;struct Cyc_Absyn_Tuniondecl*_tmp2F3;struct Cyc_Absyn_Tunionfield*
_tmp2F4;struct Cyc_Absyn_MallocInfo _tmp2F5;int _tmp2F6;struct Cyc_Absyn_Exp*_tmp2F7;
void**_tmp2F8;struct Cyc_Absyn_Exp*_tmp2F9;int _tmp2FA;struct Cyc_Absyn_Stmt*
_tmp2FB;struct Cyc_Absyn_Fndecl*_tmp2FC;_LL1D8: if(*((int*)_tmp2AD)!= 0)goto _LL1DA;
_tmp2AE=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp2AD)->f1;if((int)_tmp2AE != 
0)goto _LL1DA;_LL1D9: {struct Cyc_Absyn_Exp*_tmp2FD=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){if(nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(
old_typ,_tmp2FD,_tmp2FD))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp2FE[3];_tmp2FE[2]=_tmp2FD;_tmp2FE[1]=
_tmp2FD;_tmp2FE[0]=_tmp2FD;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp2FE,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}else{(void*)(e->r=(
void*)((void*)(Cyc_Absyn_signed_int_exp(0,0))->r));}goto _LL1D7;}_LL1DA: if(*((int*)
_tmp2AD)!= 0)goto _LL1DC;_LL1DB: goto _LL1D7;_LL1DC: if(*((int*)_tmp2AD)!= 1)goto
_LL1DE;_tmp2AF=((struct Cyc_Absyn_Var_e_struct*)_tmp2AD)->f1;_tmp2B0=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2AD)->f2;_LL1DD:{struct _handler_cons _tmp2FF;
_push_handler(& _tmp2FF);{int _tmp301=0;if(setjmp(_tmp2FF.handler))_tmp301=1;if(!
_tmp301){(void*)(e->r=(void*)((void*)(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*
d,struct _tuple1*k))Cyc_Dict_lookup)(nv->varmap,_tmp2AF))->r));;_pop_handler();}
else{void*_tmp300=(void*)_exn_thrown;void*_tmp303=_tmp300;_LL22D: if(_tmp303 != 
Cyc_Dict_Absent)goto _LL22F;_LL22E:({struct Cyc_String_pa_struct _tmp306;_tmp306.tag=
0;_tmp306.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp2AF));{void*_tmp304[1]={& _tmp306};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp305="Can't find %s in exp_to_c, Var\n";
_tag_arr(_tmp305,sizeof(char),_get_zero_arr_size(_tmp305,32));}),_tag_arr(
_tmp304,sizeof(void*),1));}});_LL22F:;_LL230:(void)_throw(_tmp303);_LL22C:;}}}
goto _LL1D7;_LL1DE: if(*((int*)_tmp2AD)!= 2)goto _LL1E0;_tmp2B1=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp2AD)->f1;_LL1DF:({void*_tmp307[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp308="unknownid";_tag_arr(
_tmp308,sizeof(char),_get_zero_arr_size(_tmp308,10));}),_tag_arr(_tmp307,sizeof(
void*),0));});_LL1E0: if(*((int*)_tmp2AD)!= 3)goto _LL1E2;_tmp2B2=(void*)((struct
Cyc_Absyn_Primop_e_struct*)_tmp2AD)->f1;_tmp2B3=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2AD)->f2;_LL1E1: {struct Cyc_List_List*_tmp309=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,
_tmp2B3);((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp2B3);{void*
_tmp30A=_tmp2B2;_LL232: if((int)_tmp30A != 19)goto _LL234;_LL233: {struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2B3))->hd;{void*
_tmp30B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);
struct Cyc_Absyn_ArrayInfo _tmp30C;struct Cyc_Absyn_Exp*_tmp30D;struct Cyc_Absyn_PtrInfo
_tmp30E;void*_tmp30F;struct Cyc_Absyn_PtrAtts _tmp310;struct Cyc_Absyn_Conref*
_tmp311;struct Cyc_Absyn_Conref*_tmp312;struct Cyc_Absyn_Conref*_tmp313;_LL247: if(
_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 7)goto _LL249;_tmp30C=((struct Cyc_Absyn_ArrayType_struct*)
_tmp30B)->f1;_tmp30D=_tmp30C.num_elts;_LL248:(void*)(e->r=(void*)((void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp30D))->r));goto _LL246;_LL249: if(_tmp30B <= (
void*)3?1:*((int*)_tmp30B)!= 4)goto _LL24B;_tmp30E=((struct Cyc_Absyn_PointerType_struct*)
_tmp30B)->f1;_tmp30F=(void*)_tmp30E.elt_typ;_tmp310=_tmp30E.ptr_atts;_tmp311=
_tmp310.nullable;_tmp312=_tmp310.bounds;_tmp313=_tmp310.zero_term;_LL24A:{void*
_tmp314=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp312);struct Cyc_Absyn_Exp*
_tmp315;_LL24E: if((int)_tmp314 != 0)goto _LL250;_LL24F:(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({struct Cyc_Absyn_Exp*_tmp316[2];
_tmp316[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp30F),0);_tmp316[0]=(
struct Cyc_Absyn_Exp*)_tmp2B3->hd;((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp316,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));
goto _LL24D;_LL250: if(_tmp314 <= (void*)1?1:*((int*)_tmp314)!= 0)goto _LL252;
_tmp315=((struct Cyc_Absyn_Upper_b_struct*)_tmp314)->f1;_LL251: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp313))(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*
_tmp317[2];_tmp317[1]=_tmp315;_tmp317[0]=(struct Cyc_Absyn_Exp*)_tmp2B3->hd;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp317,
sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));else{if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp311))(void*)(e->r=(void*)((void*)(Cyc_Absyn_conditional_exp(
arg,_tmp315,Cyc_Absyn_uint_exp(0,0),0))->r));else{(void*)(e->r=(void*)((void*)
_tmp315->r));goto _LL24D;}}goto _LL24D;_LL252: if(_tmp314 <= (void*)1?1:*((int*)
_tmp314)!= 1)goto _LL24D;_LL253:({void*_tmp318[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp319="toc: size of AbsUpper_b";
_tag_arr(_tmp319,sizeof(char),_get_zero_arr_size(_tmp319,24));}),_tag_arr(
_tmp318,sizeof(void*),0));});_LL24D:;}goto _LL246;_LL24B:;_LL24C:({struct Cyc_String_pa_struct
_tmp31D;_tmp31D.tag=0;_tmp31D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{struct Cyc_String_pa_struct
_tmp31C;_tmp31C.tag=0;_tmp31C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{void*_tmp31A[2]={&
_tmp31C,& _tmp31D};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp31B="size primop applied to non-array %s (%s)";_tag_arr(_tmp31B,
sizeof(char),_get_zero_arr_size(_tmp31B,41));}),_tag_arr(_tmp31A,sizeof(void*),2));}}});
_LL246:;}goto _LL231;}_LL234: if((int)_tmp30A != 0)goto _LL236;_LL235:{void*_tmp31E=
Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp309))->hd);
struct Cyc_Absyn_PtrInfo _tmp31F;void*_tmp320;struct Cyc_Absyn_PtrAtts _tmp321;
struct Cyc_Absyn_Conref*_tmp322;struct Cyc_Absyn_Conref*_tmp323;_LL255: if(_tmp31E
<= (void*)3?1:*((int*)_tmp31E)!= 4)goto _LL257;_tmp31F=((struct Cyc_Absyn_PointerType_struct*)
_tmp31E)->f1;_tmp320=(void*)_tmp31F.elt_typ;_tmp321=_tmp31F.ptr_atts;_tmp322=
_tmp321.bounds;_tmp323=_tmp321.zero_term;_LL256:{void*_tmp324=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp322);struct Cyc_Absyn_Exp*_tmp325;_LL25A: if((int)_tmp324
!= 0)goto _LL25C;_LL25B: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp2B3))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2B3->tl))->hd;(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp326[3];
_tmp326[2]=e2;_tmp326[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp320),0);
_tmp326[0]=e1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp326,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));goto _LL259;}_LL25C:
if(_tmp324 <= (void*)1?1:*((int*)_tmp324)!= 0)goto _LL25E;_tmp325=((struct Cyc_Absyn_Upper_b_struct*)
_tmp324)->f1;_LL25D: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp323)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2B3))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp2B3->tl))->hd;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__zero_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp327[3];_tmp327[2]=e2;_tmp327[
1]=_tmp325;_tmp327[0]=e1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp327,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}goto _LL259;_LL25E:
if(_tmp324 <= (void*)1?1:*((int*)_tmp324)!= 1)goto _LL259;_LL25F:({void*_tmp328[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*
_tmp329="toc: plus on AbsUpper_b";_tag_arr(_tmp329,sizeof(char),
_get_zero_arr_size(_tmp329,24));}),_tag_arr(_tmp328,sizeof(void*),0));});_LL259:;}
goto _LL254;_LL257:;_LL258: goto _LL254;_LL254:;}goto _LL231;_LL236: if((int)_tmp30A
!= 2)goto _LL238;_LL237: {void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)((struct Cyc_List_List*)_check_null(_tmp309))->hd,& elt_typ)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2B3))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp2B3->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)
_check_null(_tmp309->tl))->hd)){(void*)(e1->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e2->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,
0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r));}else{(void*)(e->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*
_tmp32A[3];_tmp32A[2]=Cyc_Absyn_prim1_exp((void*)2,e2,0);_tmp32A[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp32A[0]=e1;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp32A,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));}}goto _LL231;}_LL238: if((int)_tmp30A != 5)goto _LL23A;_LL239: goto _LL23B;
_LL23A: if((int)_tmp30A != 6)goto _LL23C;_LL23B: goto _LL23D;_LL23C: if((int)_tmp30A != 
7)goto _LL23E;_LL23D: goto _LL23F;_LL23E: if((int)_tmp30A != 9)goto _LL240;_LL23F: goto
_LL241;_LL240: if((int)_tmp30A != 8)goto _LL242;_LL241: goto _LL243;_LL242: if((int)
_tmp30A != 10)goto _LL244;_LL243: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2B3))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2B3->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp309))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp309->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(t1))(void*)(e1->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,
0))->r));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))(void*)(e2->r=(void*)((void*)(
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));
goto _LL231;}_LL244:;_LL245: goto _LL231;_LL231:;}goto _LL1D7;}_LL1E2: if(*((int*)
_tmp2AD)!= 5)goto _LL1E4;_tmp2B4=((struct Cyc_Absyn_Increment_e_struct*)_tmp2AD)->f1;
_tmp2B5=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp2AD)->f2;_LL1E3: {void*
e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2B4->topt))->v;void*
ptr_type=(void*)0;void*elt_type=(void*)0;int fat_ptr=0;struct _tagged_arr incr_str=({
const char*_tmp336="increment";_tag_arr(_tmp336,sizeof(char),_get_zero_arr_size(
_tmp336,10));});if(_tmp2B5 == (void*)2?1: _tmp2B5 == (void*)3)incr_str=({const char*
_tmp32B="decrement";_tag_arr(_tmp32B,sizeof(char),_get_zero_arr_size(_tmp32B,10));});
if(Cyc_Toc_is_zero_ptr_deref(_tmp2B4,& ptr_type,& fat_ptr,& elt_type)){({struct Cyc_String_pa_struct
_tmp32E;_tmp32E.tag=0;_tmp32E.f1=(struct _tagged_arr)((struct _tagged_arr)incr_str);{
void*_tmp32C[1]={& _tmp32E};Cyc_Tcutil_terr(e->loc,({const char*_tmp32D="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_arr(_tmp32D,sizeof(char),_get_zero_arr_size(_tmp32D,74));}),_tag_arr(
_tmp32C,sizeof(void*),1));}});({void*_tmp32F[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp330="in-place inc on zero-term";
_tag_arr(_tmp330,sizeof(char),_get_zero_arr_size(_tmp330,26));}),_tag_arr(
_tmp32F,sizeof(void*),0));});}Cyc_Toc_exp_to_c(nv,_tmp2B4);{void*elt_typ=(void*)
0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
_tmp331=(_tmp2B5 == (void*)1?1: _tmp2B5 == (void*)3)?Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e;int _tmp332=(_tmp2B5 == (void*)0?1: _tmp2B5 == (
void*)1)?1: - 1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(_tmp331,({struct
Cyc_Absyn_Exp*_tmp333[3];_tmp333[2]=Cyc_Absyn_signed_int_exp(_tmp332,0);_tmp333[
1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp333[0]=Cyc_Absyn_address_exp(
_tmp2B4,0);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp333,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*_tmp334=_tmp2B5 == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_e:
Cyc_Toc__zero_arr_inplace_plus_e;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
_tmp334,({struct Cyc_Absyn_Exp*_tmp335[2];_tmp335[1]=Cyc_Absyn_signed_int_exp(1,0);
_tmp335[0]=_tmp2B4;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp335,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}else{if(elt_typ == (
void*)0?!Cyc_Absyn_is_lvalue(_tmp2B4): 0){Cyc_Toc_lvalue_assign(_tmp2B4,0,Cyc_Toc_incr_lvalue,
_tmp2B5);(void*)(e->r=(void*)((void*)_tmp2B4->r));}}}goto _LL1D7;}}_LL1E4: if(*((
int*)_tmp2AD)!= 4)goto _LL1E6;_tmp2B6=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp2AD)->f1;_tmp2B7=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp2AD)->f2;_tmp2B8=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp2AD)->f3;_LL1E5: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_fat=0;if(Cyc_Toc_is_zero_ptr_deref(_tmp2B6,&
ptr_type,& is_fat,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp2B6,_tmp2B7,
_tmp2B8,ptr_type,is_fat,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp2B6);void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2B6->topt))->v;
void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2B8->topt))->v;Cyc_Toc_exp_to_c(
nv,_tmp2B6);Cyc_Toc_exp_to_c(nv,_tmp2B8);{int done=0;if(_tmp2B7 != 0){void*elt_typ=(
void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;{void*_tmp337=(void*)_tmp2B7->v;_LL261: if((int)_tmp337 != 0)goto _LL263;
_LL262: change=_tmp2B8;goto _LL260;_LL263: if((int)_tmp337 != 2)goto _LL265;_LL264:
change=Cyc_Absyn_prim1_exp((void*)2,_tmp2B8,0);goto _LL260;_LL265:;_LL266:({void*
_tmp338[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp339="bad t ? pointer arithmetic";_tag_arr(_tmp339,sizeof(char),
_get_zero_arr_size(_tmp339,27));}),_tag_arr(_tmp338,sizeof(void*),0));});_LL260:;}
done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp33A[3];_tmp33A[2]=change;_tmp33A[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp33A[0]=Cyc_Absyn_address_exp(_tmp2B6,0);((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp33A,
sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp33B=(void*)_tmp2B7->v;_LL268: if((int)_tmp33B != 0)
goto _LL26A;_LL269: done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp33C[2];_tmp33C[1]=_tmp2B8;_tmp33C[0]=_tmp2B6;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp33C,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL267;_LL26A:;_LL26B:({void*_tmp33D[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp33E="bad zero-terminated pointer arithmetic";
_tag_arr(_tmp33E,sizeof(char),_get_zero_arr_size(_tmp33E,39));}),_tag_arr(
_tmp33D,sizeof(void*),0));});_LL267:;}}}if(!done){if(e1_poly)(void*)(_tmp2B8->r=(
void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((
void*)_tmp2B8->r,0),0))->r));if(!Cyc_Absyn_is_lvalue(_tmp2B6)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
struct _tuple9*),struct _tuple9*f_env))Cyc_Toc_lvalue_assign)(_tmp2B6,0,Cyc_Toc_assignop_lvalue,({
struct _tuple9*_tmp33F=_cycalloc(sizeof(struct _tuple9)* 1);_tmp33F[0]=({struct
_tuple9 _tmp340;_tmp340.f1=_tmp2B7;_tmp340.f2=_tmp2B8;_tmp340;});_tmp33F;}));(
void*)(e->r=(void*)((void*)_tmp2B6->r));}}goto _LL1D7;}}}_LL1E6: if(*((int*)
_tmp2AD)!= 6)goto _LL1E8;_tmp2B9=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2AD)->f1;
_tmp2BA=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2AD)->f2;_tmp2BB=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp2AD)->f3;_LL1E7: Cyc_Toc_exp_to_c(nv,_tmp2B9);Cyc_Toc_exp_to_c(nv,_tmp2BA);
Cyc_Toc_exp_to_c(nv,_tmp2BB);goto _LL1D7;_LL1E8: if(*((int*)_tmp2AD)!= 7)goto
_LL1EA;_tmp2BC=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp2AD)->f1;_tmp2BD=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp2AD)->f2;_LL1E9: Cyc_Toc_exp_to_c(nv,_tmp2BC);Cyc_Toc_exp_to_c(
nv,_tmp2BD);goto _LL1D7;_LL1EA: if(*((int*)_tmp2AD)!= 8)goto _LL1EC;_tmp2BE=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp2AD)->f1;_tmp2BF=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp2AD)->f2;_LL1EB: _tmp2C0=_tmp2BE;_tmp2C1=_tmp2BF;goto _LL1ED;_LL1EC: if(*((int*)
_tmp2AD)!= 9)goto _LL1EE;_tmp2C0=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2AD)->f1;
_tmp2C1=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2AD)->f2;_tmp2C2=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp2AD)->f3;if(_tmp2C2 != 0)goto _LL1EE;_LL1ED: Cyc_Toc_exp_to_c(nv,_tmp2C0);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp2C1);goto _LL1D7;
_LL1EE: if(*((int*)_tmp2AD)!= 9)goto _LL1F0;_tmp2C3=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp2AD)->f1;_tmp2C4=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2AD)->f2;_tmp2C5=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp2AD)->f3;if(_tmp2C5 == 0)goto _LL1F0;_tmp2C6=*
_tmp2C5;_tmp2C7=_tmp2C6.num_varargs;_tmp2C8=_tmp2C6.injectors;_tmp2C9=_tmp2C6.vai;
_LL1EF:{struct _RegionHandle _tmp341=_new_region("r");struct _RegionHandle*r=&
_tmp341;_push_region(r);{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp2C7,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp2C9->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp2C4);int num_normargs=num_args - _tmp2C7;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp2C4=_tmp2C4->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2C4))->hd);
new_args=({struct Cyc_List_List*_tmp342=_cycalloc(sizeof(*_tmp342));_tmp342->hd=(
struct Cyc_Absyn_Exp*)_tmp2C4->hd;_tmp342->tl=new_args;_tmp342;});}}new_args=({
struct Cyc_List_List*_tmp343=_cycalloc(sizeof(*_tmp343));_tmp343->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp344[3];_tmp344[2]=num_varargs_exp;
_tmp344[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp344[0]=argvexp;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp344,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);_tmp343->tl=new_args;_tmp343;});new_args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp2C3);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp2C3,new_args,
0),0);if(_tmp2C9->inject){struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp345=Cyc_Tcutil_compress((
void*)_tmp2C9->type);struct Cyc_Absyn_TunionInfo _tmp346;void*_tmp347;struct Cyc_Absyn_Tuniondecl**
_tmp348;struct Cyc_Absyn_Tuniondecl*_tmp349;_LL26D: if(_tmp345 <= (void*)3?1:*((int*)
_tmp345)!= 2)goto _LL26F;_tmp346=((struct Cyc_Absyn_TunionType_struct*)_tmp345)->f1;
_tmp347=(void*)_tmp346.tunion_info;if(*((int*)_tmp347)!= 1)goto _LL26F;_tmp348=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp347)->f1;_tmp349=*_tmp348;_LL26E: tud=
_tmp349;goto _LL26C;_LL26F:;_LL270:({void*_tmp34A[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp34B="toc: unknown tunion in vararg with inject";
_tag_arr(_tmp34B,sizeof(char),_get_zero_arr_size(_tmp34B,42));}),_tag_arr(
_tmp34A,sizeof(void*),0));});_LL26C:;}{struct _tagged_arr vs=({unsigned int _tmp353=(
unsigned int)_tmp2C7;struct _tuple1**_tmp354=(struct _tuple1**)_region_malloc(r,
_check_times(sizeof(struct _tuple1*),_tmp353));struct _tagged_arr _tmp356=_tag_arr(
_tmp354,sizeof(struct _tuple1*),_tmp353);{unsigned int _tmp355=_tmp353;
unsigned int i;for(i=0;i < _tmp355;i ++){_tmp354[i]=Cyc_Toc_temp_var();}}_tmp356;});
struct Cyc_List_List*_tmp34C=0;{int i=_tmp2C7 - 1;for(0;i >= 0;-- i){_tmp34C=({struct
Cyc_List_List*_tmp34D=_cycalloc(sizeof(*_tmp34D));_tmp34D->hd=Cyc_Toc_make_dle(
Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)
_check_unknown_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp34D->tl=
_tmp34C;_tmp34D;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp(0,_tmp34C,0),s,0);{int i=0;for(0;_tmp2C4 != 0;(((
_tmp2C4=_tmp2C4->tl,_tmp2C8=_tmp2C8->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct
Cyc_Absyn_Exp*)_tmp2C4->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_unknown_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp34F;struct _tuple1*
_tmp350;struct Cyc_List_List*_tmp351;struct Cyc_Absyn_Tunionfield*_tmp34E=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp2C8))->hd;_tmp34F=*
_tmp34E;_tmp350=_tmp34F.name;_tmp351=_tmp34F.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp351))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Absyn_cast_exp(field_typ,arg,
0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,
Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp350,
1),0),s,0);s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp350,({const char*_tmp352="_struct";_tag_arr(_tmp352,sizeof(char),
_get_zero_arr_size(_tmp352,8));}))),0,s,0);}}}}}else{{int i=0;for(0;_tmp2C4 != 0;(
_tmp2C4=_tmp2C4->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp2C4->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((
unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp2C4->hd,0),s,0);}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,0,s,0);}(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};
_pop_region(r);}goto _LL1D7;_LL1F0: if(*((int*)_tmp2AD)!= 10)goto _LL1F2;_tmp2CA=((
struct Cyc_Absyn_Throw_e_struct*)_tmp2AD)->f1;_LL1F1: Cyc_Toc_exp_to_c(nv,_tmp2CA);(
void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),
Cyc_Toc_newthrow_exp(_tmp2CA),0))->r));goto _LL1D7;_LL1F2: if(*((int*)_tmp2AD)!= 
11)goto _LL1F4;_tmp2CB=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp2AD)->f1;
_LL1F3: Cyc_Toc_exp_to_c(nv,_tmp2CB);goto _LL1D7;_LL1F4: if(*((int*)_tmp2AD)!= 12)
goto _LL1F6;_tmp2CC=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2AD)->f1;_tmp2CD=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp2AD)->f2;_LL1F5: Cyc_Toc_exp_to_c(nv,
_tmp2CC);for(0;_tmp2CD != 0;_tmp2CD=_tmp2CD->tl){void*k=Cyc_Tcutil_typ_kind((void*)
_tmp2CD->hd);if(k != (void*)4?k != (void*)3: 0){{void*_tmp357=Cyc_Tcutil_compress((
void*)_tmp2CD->hd);_LL272: if(_tmp357 <= (void*)3?1:*((int*)_tmp357)!= 1)goto
_LL274;_LL273: goto _LL275;_LL274: if(_tmp357 <= (void*)3?1:*((int*)_tmp357)!= 2)
goto _LL276;_LL275: continue;_LL276:;_LL277:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp2CC,
0))->r));goto _LL271;_LL271:;}break;}}goto _LL1D7;_LL1F6: if(*((int*)_tmp2AD)!= 13)
goto _LL1F8;_tmp2CE=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2AD)->f1;_tmp2CF=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2AD)->f1);_tmp2D0=((struct
Cyc_Absyn_Cast_e_struct*)_tmp2AD)->f2;_LL1F7: {void*old_t2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2D0->topt))->v;void*new_typ=*_tmp2CF;*_tmp2CF=Cyc_Toc_typ_to_c(
new_typ);Cyc_Toc_exp_to_c(nv,_tmp2D0);{struct _tuple0 _tmp359=({struct _tuple0
_tmp358;_tmp358.f1=Cyc_Tcutil_compress(old_t2);_tmp358.f2=Cyc_Tcutil_compress(
new_typ);_tmp358;});void*_tmp35A;struct Cyc_Absyn_PtrInfo _tmp35B;void*_tmp35C;
struct Cyc_Absyn_PtrInfo _tmp35D;void*_tmp35E;struct Cyc_Absyn_PtrInfo _tmp35F;void*
_tmp360;_LL279: _tmp35A=_tmp359.f1;if(_tmp35A <= (void*)3?1:*((int*)_tmp35A)!= 4)
goto _LL27B;_tmp35B=((struct Cyc_Absyn_PointerType_struct*)_tmp35A)->f1;_tmp35C=
_tmp359.f2;if(_tmp35C <= (void*)3?1:*((int*)_tmp35C)!= 4)goto _LL27B;_tmp35D=((
struct Cyc_Absyn_PointerType_struct*)_tmp35C)->f1;_LL27A: {int _tmp361=((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp35B.ptr_atts).nullable);
int _tmp362=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(
_tmp35D.ptr_atts).nullable);void*_tmp363=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp35B.ptr_atts).bounds);void*_tmp364=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp35D.ptr_atts).bounds);int _tmp365=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp35B.ptr_atts).zero_term);int _tmp366=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp35D.ptr_atts).zero_term);{struct _tuple0 _tmp368=({
struct _tuple0 _tmp367;_tmp367.f1=_tmp363;_tmp367.f2=_tmp364;_tmp367;});void*
_tmp369;struct Cyc_Absyn_Exp*_tmp36A;void*_tmp36B;struct Cyc_Absyn_Exp*_tmp36C;
void*_tmp36D;struct Cyc_Absyn_Exp*_tmp36E;void*_tmp36F;void*_tmp370;void*_tmp371;
struct Cyc_Absyn_Exp*_tmp372;void*_tmp373;void*_tmp374;void*_tmp375;void*_tmp376;
_LL280: _tmp369=_tmp368.f1;if(_tmp369 <= (void*)1?1:*((int*)_tmp369)!= 0)goto
_LL282;_tmp36A=((struct Cyc_Absyn_Upper_b_struct*)_tmp369)->f1;_tmp36B=_tmp368.f2;
if(_tmp36B <= (void*)1?1:*((int*)_tmp36B)!= 0)goto _LL282;_tmp36C=((struct Cyc_Absyn_Upper_b_struct*)
_tmp36B)->f1;_LL281: if(_tmp361?!_tmp362: 0){if(nv->toplevel)({void*_tmp377[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*
_tmp378="can't do NULL-check conversion at top-level";_tag_arr(_tmp378,sizeof(
char),_get_zero_arr_size(_tmp378,44));}),_tag_arr(_tmp377,sizeof(void*),0));});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(*_tmp2CF,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379->hd=_tmp2D0;
_tmp379->tl=0;_tmp379;}),0),0))->r));}goto _LL27F;_LL282: _tmp36D=_tmp368.f1;if(
_tmp36D <= (void*)1?1:*((int*)_tmp36D)!= 0)goto _LL284;_tmp36E=((struct Cyc_Absyn_Upper_b_struct*)
_tmp36D)->f1;_tmp36F=_tmp368.f2;if((int)_tmp36F != 0)goto _LL284;_LL283: if(nv->toplevel){
if((_tmp365?!(_tmp35D.elt_tq).q_const: 0)?!_tmp366: 0)_tmp36E=Cyc_Absyn_prim2_exp((
void*)2,_tmp36E,Cyc_Absyn_uint_exp(1,0),0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(
old_t2,_tmp36E,_tmp2D0))->r));}else{if(_tmp365){struct _tuple1*_tmp37A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp37B=Cyc_Absyn_var_exp(_tmp37A,0);struct Cyc_Absyn_Exp*
_tmp37C=Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*
_tmp381[2];_tmp381[1]=_tmp36E;_tmp381[0]=_tmp37B;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp381,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);if(!_tmp366?!(_tmp35D.elt_tq).q_const: 0)_tmp37C=Cyc_Absyn_prim2_exp((
void*)2,_tmp37C,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp37D=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp35D.elt_typ),0);struct Cyc_Absyn_Exp*_tmp37E=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp380[3];_tmp380[2]=_tmp37C;_tmp380[1]=
_tmp37D;_tmp380[0]=_tmp37B;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp380,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp37F=Cyc_Absyn_exp_stmt(_tmp37E,0);_tmp37F=Cyc_Absyn_declare_stmt(_tmp37A,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp2D0,_tmp37F,0);(void*)(e->r=(void*)((void*)(
Cyc_Absyn_stmt_exp(_tmp37F,0))->r));}}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp382[3];_tmp382[2]=_tmp36E;_tmp382[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)_tmp35D.elt_typ),0);_tmp382[0]=
_tmp2D0;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp382,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}goto _LL27F;_LL284: _tmp370=
_tmp368.f1;if((int)_tmp370 != 0)goto _LL286;_tmp371=_tmp368.f2;if(_tmp371 <= (void*)
1?1:*((int*)_tmp371)!= 0)goto _LL286;_tmp372=((struct Cyc_Absyn_Upper_b_struct*)
_tmp371)->f1;_LL285: if(nv->toplevel)({void*_tmp383[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp384="can't coerce t? to t* or t@ at the top-level";
_tag_arr(_tmp384,sizeof(char),_get_zero_arr_size(_tmp384,45));}),_tag_arr(
_tmp383,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp385=_tmp372;if(_tmp365?!
_tmp366: 0)_tmp385=Cyc_Absyn_add_exp(_tmp372,Cyc_Absyn_uint_exp(1,0),0);{struct
Cyc_Absyn_Exp*_tmp386=Cyc_Absyn_fncall_exp(Cyc_Toc__untag_arr_e,({struct Cyc_Absyn_Exp*
_tmp388[3];_tmp388[2]=_tmp385;_tmp388[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp35B.elt_typ),0);_tmp388[0]=_tmp2D0;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp388,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);if(_tmp362)(void*)(_tmp386->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387->hd=Cyc_Absyn_copy_exp(
_tmp386);_tmp387->tl=0;_tmp387;}),0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(*
_tmp2CF,_tmp386,0))->r));goto _LL27F;}}_LL286: _tmp373=_tmp368.f1;if((int)_tmp373
!= 0)goto _LL288;_tmp374=_tmp368.f2;if((int)_tmp374 != 0)goto _LL288;_LL287: if((
_tmp365?!_tmp366: 0)?!(_tmp35D.elt_tq).q_const: 0){if(nv->toplevel)({void*_tmp389[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const
char*_tmp38A="can't coerce a ?ZEROTERM to a non-const ?NOZEROTERM at toplevel";
_tag_arr(_tmp38A,sizeof(char),_get_zero_arr_size(_tmp38A,64));}),_tag_arr(
_tmp389,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_ptr_decrease_size_e,({struct Cyc_Absyn_Exp*_tmp38B[3];_tmp38B[2]=
Cyc_Absyn_uint_exp(1,0);_tmp38B[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp35B.elt_typ),0);_tmp38B[0]=_tmp2D0;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp38B,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));}goto _LL27F;_LL288: _tmp375=_tmp368.f1;if(_tmp375 <= (void*)1?1:*((int*)
_tmp375)!= 1)goto _LL28A;_LL289:({void*_tmp38C[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp38D="toc, cast from AbsUpper_b";
_tag_arr(_tmp38D,sizeof(char),_get_zero_arr_size(_tmp38D,26));}),_tag_arr(
_tmp38C,sizeof(void*),0));});_LL28A: _tmp376=_tmp368.f2;if(_tmp376 <= (void*)1?1:*((
int*)_tmp376)!= 1)goto _LL27F;_LL28B: goto _LL27F;_LL27F:;}goto _LL278;}_LL27B:
_tmp35E=_tmp359.f1;if(_tmp35E <= (void*)3?1:*((int*)_tmp35E)!= 4)goto _LL27D;
_tmp35F=((struct Cyc_Absyn_PointerType_struct*)_tmp35E)->f1;_tmp360=_tmp359.f2;
if(_tmp360 <= (void*)3?1:*((int*)_tmp360)!= 5)goto _LL27D;_LL27C:{void*_tmp38E=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp35F.ptr_atts).bounds);_LL28D: if((int)_tmp38E != 0)goto
_LL28F;_LL28E:(void*)(_tmp2D0->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((
void*)_tmp2D0->r,_tmp2D0->loc),Cyc_Toc_curr_sp,0))->r));goto _LL28C;_LL28F:;
_LL290: goto _LL28C;_LL28C:;}goto _LL278;_LL27D:;_LL27E: goto _LL278;_LL278:;}goto
_LL1D7;}_LL1F8: if(*((int*)_tmp2AD)!= 14)goto _LL1FA;_tmp2D1=((struct Cyc_Absyn_Address_e_struct*)
_tmp2AD)->f1;_LL1F9:{void*_tmp38F=(void*)_tmp2D1->r;struct _tuple1*_tmp390;struct
Cyc_List_List*_tmp391;struct Cyc_List_List*_tmp392;struct Cyc_List_List*_tmp393;
_LL292: if(*((int*)_tmp38F)!= 28)goto _LL294;_tmp390=((struct Cyc_Absyn_Struct_e_struct*)
_tmp38F)->f1;_tmp391=((struct Cyc_Absyn_Struct_e_struct*)_tmp38F)->f2;_tmp392=((
struct Cyc_Absyn_Struct_e_struct*)_tmp38F)->f3;_LL293: if(nv->toplevel)({struct Cyc_String_pa_struct
_tmp396;_tmp396.tag=0;_tmp396.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp2D1->loc));{void*_tmp394[1]={& _tmp396};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp395="%s: & on non-identifiers at the top-level";
_tag_arr(_tmp395,sizeof(char),_get_zero_arr_size(_tmp395,42));}),_tag_arr(
_tmp394,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2D1->topt))->v,_tmp391 != 0,1,0,
_tmp392,_tmp390))->r));goto _LL291;_LL294: if(*((int*)_tmp38F)!= 24)goto _LL296;
_tmp393=((struct Cyc_Absyn_Tuple_e_struct*)_tmp38F)->f1;_LL295: if(nv->toplevel)({
struct Cyc_String_pa_struct _tmp399;_tmp399.tag=0;_tmp399.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Position_string_of_segment(_tmp2D1->loc));{void*_tmp397[1]={&
_tmp399};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp398="%s: & on non-identifiers at the top-level";_tag_arr(_tmp398,
sizeof(char),_get_zero_arr_size(_tmp398,42));}),_tag_arr(_tmp397,sizeof(void*),1));}});(
void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,0,_tmp393))->r));goto _LL291;
_LL296:;_LL297: Cyc_Toc_exp_to_c(nv,_tmp2D1);if(!Cyc_Absyn_is_lvalue(_tmp2D1)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp2D1,0,Cyc_Toc_address_lvalue,
1);(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp2D1,0))->r));}goto _LL291;
_LL291:;}goto _LL1D7;_LL1FA: if(*((int*)_tmp2AD)!= 15)goto _LL1FC;_tmp2D2=((struct
Cyc_Absyn_New_e_struct*)_tmp2AD)->f1;_tmp2D3=((struct Cyc_Absyn_New_e_struct*)
_tmp2AD)->f2;_LL1FB: if(nv->toplevel)({struct Cyc_String_pa_struct _tmp39C;_tmp39C.tag=
0;_tmp39C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp2D3->loc));{void*_tmp39A[1]={& _tmp39C};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp39B="%s: new at top-level";
_tag_arr(_tmp39B,sizeof(char),_get_zero_arr_size(_tmp39B,21));}),_tag_arr(
_tmp39A,sizeof(void*),1));}});{void*_tmp39D=(void*)_tmp2D3->r;struct Cyc_List_List*
_tmp39E;struct Cyc_Absyn_Vardecl*_tmp39F;struct Cyc_Absyn_Exp*_tmp3A0;struct Cyc_Absyn_Exp*
_tmp3A1;int _tmp3A2;struct _tuple1*_tmp3A3;struct Cyc_List_List*_tmp3A4;struct Cyc_List_List*
_tmp3A5;struct Cyc_List_List*_tmp3A6;_LL299: if(*((int*)_tmp39D)!= 26)goto _LL29B;
_tmp39E=((struct Cyc_Absyn_Array_e_struct*)_tmp39D)->f1;_LL29A: {struct _tuple1*
_tmp3A7=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3A8=Cyc_Absyn_var_exp(_tmp3A7,
0);struct Cyc_Absyn_Stmt*_tmp3A9=Cyc_Toc_init_array(nv,_tmp3A8,_tmp39E,Cyc_Absyn_exp_stmt(
_tmp3A8,0));void*old_elt_typ;{void*_tmp3AA=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_PtrInfo _tmp3AB;void*_tmp3AC;struct Cyc_Absyn_Tqual _tmp3AD;struct Cyc_Absyn_PtrAtts
_tmp3AE;struct Cyc_Absyn_Conref*_tmp3AF;_LL2A4: if(_tmp3AA <= (void*)3?1:*((int*)
_tmp3AA)!= 4)goto _LL2A6;_tmp3AB=((struct Cyc_Absyn_PointerType_struct*)_tmp3AA)->f1;
_tmp3AC=(void*)_tmp3AB.elt_typ;_tmp3AD=_tmp3AB.elt_tq;_tmp3AE=_tmp3AB.ptr_atts;
_tmp3AF=_tmp3AE.zero_term;_LL2A5: old_elt_typ=_tmp3AC;goto _LL2A3;_LL2A6:;_LL2A7:
old_elt_typ=({void*_tmp3B0[0]={};Cyc_Toc_toc_impos(({const char*_tmp3B1="exp_to_c:new array expression doesn't have ptr type";
_tag_arr(_tmp3B1,sizeof(char),_get_zero_arr_size(_tmp3B1,52));}),_tag_arr(
_tmp3B0,sizeof(void*),0));});_LL2A3:;}{void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp3B2=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp3B3=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),Cyc_Absyn_signed_int_exp(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp39E),0),0);struct Cyc_Absyn_Exp*
e1;if(_tmp2D2 == 0)e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp3B3);else{struct Cyc_Absyn_Exp*
r=(struct Cyc_Absyn_Exp*)_check_null(_tmp2D2);Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(
r,_tmp3B3);}(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
_tmp3A7,_tmp3B2,(struct Cyc_Absyn_Exp*)e1,_tmp3A9,0),0))->r));goto _LL298;}}_LL29B:
if(*((int*)_tmp39D)!= 27)goto _LL29D;_tmp39F=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp39D)->f1;_tmp3A0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp39D)->f2;
_tmp3A1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp39D)->f3;_tmp3A2=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp39D)->f4;_LL29C: {int is_tagged_ptr=0;{void*
_tmp3B4=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp3B5;void*_tmp3B6;
struct Cyc_Absyn_Tqual _tmp3B7;struct Cyc_Absyn_PtrAtts _tmp3B8;struct Cyc_Absyn_Conref*
_tmp3B9;struct Cyc_Absyn_Conref*_tmp3BA;_LL2A9: if(_tmp3B4 <= (void*)3?1:*((int*)
_tmp3B4)!= 4)goto _LL2AB;_tmp3B5=((struct Cyc_Absyn_PointerType_struct*)_tmp3B4)->f1;
_tmp3B6=(void*)_tmp3B5.elt_typ;_tmp3B7=_tmp3B5.elt_tq;_tmp3B8=_tmp3B5.ptr_atts;
_tmp3B9=_tmp3B8.bounds;_tmp3BA=_tmp3B8.zero_term;_LL2AA: is_tagged_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp3B9)== (void*)0;goto _LL2A8;_LL2AB:;_LL2AC:({void*
_tmp3BB[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3BC="exp_to_c: comprehension not an array type";_tag_arr(_tmp3BC,
sizeof(char),_get_zero_arr_size(_tmp3BC,42));}),_tag_arr(_tmp3BB,sizeof(void*),0));});
_LL2A8:;}{struct _tuple1*max=Cyc_Toc_temp_var();struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3A1->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp3A0);{struct Cyc_Absyn_Exp*_tmp3BD=
Cyc_Absyn_var_exp(max,0);if(_tmp3A2)_tmp3BD=Cyc_Absyn_add_exp(_tmp3BD,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp39F,Cyc_Absyn_var_exp(max,0),_tmp3A1,_tmp3A2,Cyc_Toc_skip_stmt_dl(),1);{
struct _RegionHandle _tmp3BE=_new_region("r");struct _RegionHandle*r=& _tmp3BE;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp3CE=
_region_malloc(r,sizeof(*_tmp3CE));_tmp3CE->hd=({struct _tuple10*_tmp3CF=
_region_malloc(r,sizeof(*_tmp3CF));_tmp3CF->f1=max;_tmp3CF->f2=Cyc_Absyn_uint_typ;
_tmp3CF->f3=(struct Cyc_Absyn_Exp*)_tmp3A0;_tmp3CF;});_tmp3CE->tl=0;_tmp3CE;});
struct Cyc_Absyn_Exp*ai;if(_tmp2D2 == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp3BF[2];_tmp3BF[1]=_tmp3BD;
_tmp3BF[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp3BF,sizeof(struct Cyc_Absyn_Exp*),2));}),
0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(_tmp2D2);Cyc_Toc_exp_to_c(
nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp3C0[2];_tmp3C0[1]=_tmp3BD;_tmp3C0[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp3C0,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct Cyc_Absyn_Exp*ainit=Cyc_Absyn_cast_exp(
ptr_typ,ai,0);decls=({struct Cyc_List_List*_tmp3C1=_region_malloc(r,sizeof(*
_tmp3C1));_tmp3C1->hd=({struct _tuple10*_tmp3C2=_region_malloc(r,sizeof(*_tmp3C2));
_tmp3C2->f1=a;_tmp3C2->f2=ptr_typ;_tmp3C2->f3=(struct Cyc_Absyn_Exp*)ainit;
_tmp3C2;});_tmp3C1->tl=decls;_tmp3C1;});if(is_tagged_ptr){struct _tuple1*_tmp3C3=
Cyc_Toc_temp_var();void*_tmp3C4=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*
_tmp3C5=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp3C8[3];
_tmp3C8[2]=_tmp3BD;_tmp3C8[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp3C8[0]=Cyc_Absyn_var_exp(
a,0);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp3C8,sizeof(struct Cyc_Absyn_Exp*),3));}),0);decls=({struct Cyc_List_List*
_tmp3C6=_region_malloc(r,sizeof(*_tmp3C6));_tmp3C6->hd=({struct _tuple10*_tmp3C7=
_region_malloc(r,sizeof(*_tmp3C7));_tmp3C7->f1=_tmp3C3;_tmp3C7->f2=_tmp3C4;
_tmp3C7->f3=(struct Cyc_Absyn_Exp*)_tmp3C5;_tmp3C7;});_tmp3C6->tl=decls;_tmp3C6;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp3C3,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp3C9=decls;for(0;_tmp3C9 != 0;_tmp3C9=_tmp3C9->tl){struct
_tuple1*_tmp3CB;void*_tmp3CC;struct Cyc_Absyn_Exp*_tmp3CD;struct _tuple10 _tmp3CA=*((
struct _tuple10*)_tmp3C9->hd);_tmp3CB=_tmp3CA.f1;_tmp3CC=_tmp3CA.f2;_tmp3CD=
_tmp3CA.f3;s=Cyc_Absyn_declare_stmt(_tmp3CB,_tmp3CC,_tmp3CD,s,0);}}(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL298;}}}}
_LL29D: if(*((int*)_tmp39D)!= 28)goto _LL29F;_tmp3A3=((struct Cyc_Absyn_Struct_e_struct*)
_tmp39D)->f1;_tmp3A4=((struct Cyc_Absyn_Struct_e_struct*)_tmp39D)->f2;_tmp3A5=((
struct Cyc_Absyn_Struct_e_struct*)_tmp39D)->f3;_LL29E:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2D3->topt))->v,
_tmp3A4 != 0,1,_tmp2D2,_tmp3A5,_tmp3A3))->r));goto _LL298;_LL29F: if(*((int*)
_tmp39D)!= 24)goto _LL2A1;_tmp3A6=((struct Cyc_Absyn_Tuple_e_struct*)_tmp39D)->f1;
_LL2A0:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,_tmp2D2,_tmp3A6))->r));
goto _LL298;_LL2A1:;_LL2A2: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2D3->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct
_tuple1*_tmp3D0=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3D1=Cyc_Absyn_var_exp(
_tmp3D0,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(
_tmp3D1,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp2D2 == 0)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(
_tmp2D2);Cyc_Toc_exp_to_c(nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{
void*_tmp3D2=(void*)_tmp2D3->r;void*_tmp3D3;struct Cyc_Absyn_Exp*_tmp3D4;_LL2AE:
if(*((int*)_tmp3D2)!= 13)goto _LL2B0;_tmp3D3=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp3D2)->f1;_tmp3D4=((struct Cyc_Absyn_Cast_e_struct*)_tmp3D2)->f2;_LL2AF:{
struct _tuple0 _tmp3D6=({struct _tuple0 _tmp3D5;_tmp3D5.f1=Cyc_Tcutil_compress(
_tmp3D3);_tmp3D5.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3D4->topt))->v);_tmp3D5;});void*_tmp3D7;struct Cyc_Absyn_PtrInfo _tmp3D8;void*
_tmp3D9;struct Cyc_Absyn_PtrAtts _tmp3DA;struct Cyc_Absyn_Conref*_tmp3DB;void*
_tmp3DC;struct Cyc_Absyn_PtrInfo _tmp3DD;struct Cyc_Absyn_PtrAtts _tmp3DE;struct Cyc_Absyn_Conref*
_tmp3DF;_LL2B3: _tmp3D7=_tmp3D6.f1;if(_tmp3D7 <= (void*)3?1:*((int*)_tmp3D7)!= 4)
goto _LL2B5;_tmp3D8=((struct Cyc_Absyn_PointerType_struct*)_tmp3D7)->f1;_tmp3D9=(
void*)_tmp3D8.elt_typ;_tmp3DA=_tmp3D8.ptr_atts;_tmp3DB=_tmp3DA.bounds;_tmp3DC=
_tmp3D6.f2;if(_tmp3DC <= (void*)3?1:*((int*)_tmp3DC)!= 4)goto _LL2B5;_tmp3DD=((
struct Cyc_Absyn_PointerType_struct*)_tmp3DC)->f1;_tmp3DE=_tmp3DD.ptr_atts;
_tmp3DF=_tmp3DE.bounds;_LL2B4:{struct _tuple0 _tmp3E1=({struct _tuple0 _tmp3E0;
_tmp3E0.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp3DB);_tmp3E0.f2=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp3DF);_tmp3E0;});void*_tmp3E2;void*_tmp3E3;struct Cyc_Absyn_Exp*
_tmp3E4;_LL2B8: _tmp3E2=_tmp3E1.f1;if((int)_tmp3E2 != 0)goto _LL2BA;_tmp3E3=_tmp3E1.f2;
if(_tmp3E3 <= (void*)1?1:*((int*)_tmp3E3)!= 0)goto _LL2BA;_tmp3E4=((struct Cyc_Absyn_Upper_b_struct*)
_tmp3E3)->f1;_LL2B9: Cyc_Toc_exp_to_c(nv,_tmp3D4);(void*)(inner_mexp->r=(void*)((
void*)(Cyc_Absyn_sizeoftyp_exp(elt_typ,0))->r));done=1;(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__init_tag_arr_e,({struct Cyc_Absyn_Exp*
_tmp3E5[4];_tmp3E5[3]=_tmp3E4;_tmp3E5[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp3D9),0);_tmp3E5[1]=_tmp3D4;_tmp3E5[0]=mexp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp3E5,sizeof(struct Cyc_Absyn_Exp*),4));}),
0))->r));goto _LL2B7;_LL2BA:;_LL2BB: goto _LL2B7;_LL2B7:;}goto _LL2B2;_LL2B5:;_LL2B6:
goto _LL2B2;_LL2B2:;}goto _LL2AD;_LL2B0:;_LL2B1: goto _LL2AD;_LL2AD:;}if(!done){
struct Cyc_Absyn_Stmt*_tmp3E6=Cyc_Absyn_exp_stmt(_tmp3D1,0);struct Cyc_Absyn_Exp*
_tmp3E7=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp2D3);_tmp3E6=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp3D1,_tmp3E7,0),_tmp2D3,0),
_tmp3E6,0);{void*_tmp3E8=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp3D0,_tmp3E8,(struct
Cyc_Absyn_Exp*)mexp,_tmp3E6,0),0))->r));}}goto _LL298;}}_LL298:;}goto _LL1D7;
_LL1FC: if(*((int*)_tmp2AD)!= 17)goto _LL1FE;_tmp2D4=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp2AD)->f1;_LL1FD: Cyc_Toc_exp_to_c(nv,_tmp2D4);goto _LL1D7;_LL1FE: if(*((int*)
_tmp2AD)!= 16)goto _LL200;_tmp2D5=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp2AD)->f1;_LL1FF:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp3E9=_cycalloc(sizeof(*_tmp3E9));_tmp3E9[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp3EA;_tmp3EA.tag=16;_tmp3EA.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2D5);_tmp3EA;});
_tmp3E9;})));goto _LL1D7;_LL200: if(*((int*)_tmp2AD)!= 19)goto _LL202;_LL201:({void*
_tmp3EB[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3EC="__gen() in code generator";_tag_arr(_tmp3EC,sizeof(char),
_get_zero_arr_size(_tmp3EC,26));}),_tag_arr(_tmp3EB,sizeof(void*),0));});_LL202:
if(*((int*)_tmp2AD)!= 18)goto _LL204;_tmp2D6=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp2AD)->f1;_tmp2D7=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp2AD)->f2;
if(*((int*)_tmp2D7)!= 0)goto _LL204;_tmp2D8=((struct Cyc_Absyn_StructField_struct*)
_tmp2D7)->f1;_LL203:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp3EE;_tmp3EE.tag=18;_tmp3EE.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2D6);_tmp3EE.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp3EF=_cycalloc(sizeof(*
_tmp3EF));_tmp3EF[0]=({struct Cyc_Absyn_StructField_struct _tmp3F0;_tmp3F0.tag=0;
_tmp3F0.f1=_tmp2D8;_tmp3F0;});_tmp3EF;}));_tmp3EE;});_tmp3ED;})));goto _LL1D7;
_LL204: if(*((int*)_tmp2AD)!= 18)goto _LL206;_tmp2D9=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp2AD)->f1;_tmp2DA=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp2AD)->f2;
if(*((int*)_tmp2DA)!= 1)goto _LL206;_tmp2DB=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp2DA)->f1;_LL205:{void*_tmp3F1=Cyc_Tcutil_compress(_tmp2D9);struct Cyc_Absyn_AggrInfo
_tmp3F2;void*_tmp3F3;struct Cyc_List_List*_tmp3F4;_LL2BD: if(_tmp3F1 <= (void*)3?1:*((
int*)_tmp3F1)!= 10)goto _LL2BF;_tmp3F2=((struct Cyc_Absyn_AggrType_struct*)_tmp3F1)->f1;
_tmp3F3=(void*)_tmp3F2.aggr_info;_LL2BE: {struct Cyc_Absyn_Aggrdecl*_tmp3F5=Cyc_Absyn_get_known_aggrdecl(
_tmp3F3);if(_tmp3F5->impl == 0)({void*_tmp3F6[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp3F7="struct fields must be known";
_tag_arr(_tmp3F7,sizeof(char),_get_zero_arr_size(_tmp3F7,28));}),_tag_arr(
_tmp3F6,sizeof(void*),0));});_tmp3F4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3F5->impl))->fields;goto _LL2C0;}_LL2BF: if(_tmp3F1 <= (void*)3?1:*((int*)
_tmp3F1)!= 11)goto _LL2C1;_tmp3F4=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp3F1)->f2;
_LL2C0: {struct Cyc_Absyn_Aggrfield*_tmp3F8=((struct Cyc_Absyn_Aggrfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3F4,(int)_tmp2DB);(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp3F9=_cycalloc(sizeof(*
_tmp3F9));_tmp3F9[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp3FA;_tmp3FA.tag=18;
_tmp3FA.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2D9);_tmp3FA.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB[0]=({
struct Cyc_Absyn_StructField_struct _tmp3FC;_tmp3FC.tag=0;_tmp3FC.f1=_tmp3F8->name;
_tmp3FC;});_tmp3FB;}));_tmp3FA;});_tmp3F9;})));goto _LL2BC;}_LL2C1: if(_tmp3F1 <= (
void*)3?1:*((int*)_tmp3F1)!= 9)goto _LL2C3;_LL2C2:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp3FE;_tmp3FE.tag=18;_tmp3FE.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp2D9);_tmp3FE.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp3FF=
_cycalloc(sizeof(*_tmp3FF));_tmp3FF[0]=({struct Cyc_Absyn_StructField_struct
_tmp400;_tmp400.tag=0;_tmp400.f1=Cyc_Absyn_fieldname((int)(_tmp2DB + 1));_tmp400;});
_tmp3FF;}));_tmp3FE;});_tmp3FD;})));goto _LL2BC;_LL2C3: if(_tmp3F1 <= (void*)3?1:*((
int*)_tmp3F1)!= 3)goto _LL2C5;_LL2C4: if(_tmp2DB == 0)(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp402;_tmp402.tag=18;_tmp402.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp2D9);_tmp402.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp403=
_cycalloc(sizeof(*_tmp403));_tmp403[0]=({struct Cyc_Absyn_StructField_struct
_tmp404;_tmp404.tag=0;_tmp404.f1=Cyc_Toc_tag_sp;_tmp404;});_tmp403;}));_tmp402;});
_tmp401;})));else{(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp406;_tmp406.tag=18;_tmp406.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2D9);_tmp406.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp407=_cycalloc(sizeof(*
_tmp407));_tmp407[0]=({struct Cyc_Absyn_StructField_struct _tmp408;_tmp408.tag=0;
_tmp408.f1=Cyc_Absyn_fieldname((int)_tmp2DB);_tmp408;});_tmp407;}));_tmp406;});
_tmp405;})));}goto _LL2BC;_LL2C5:;_LL2C6:({void*_tmp409[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp40A="impossible type for offsetof tuple index";
_tag_arr(_tmp40A,sizeof(char),_get_zero_arr_size(_tmp40A,41));}),_tag_arr(
_tmp409,sizeof(void*),0));});_LL2BC:;}goto _LL1D7;_LL206: if(*((int*)_tmp2AD)!= 20)
goto _LL208;_tmp2DC=((struct Cyc_Absyn_Deref_e_struct*)_tmp2AD)->f1;_LL207: {void*
_tmp40B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2DC->topt))->v);{
void*_tmp40C=_tmp40B;struct Cyc_Absyn_PtrInfo _tmp40D;void*_tmp40E;struct Cyc_Absyn_Tqual
_tmp40F;struct Cyc_Absyn_PtrAtts _tmp410;void*_tmp411;struct Cyc_Absyn_Conref*
_tmp412;struct Cyc_Absyn_Conref*_tmp413;struct Cyc_Absyn_Conref*_tmp414;_LL2C8: if(
_tmp40C <= (void*)3?1:*((int*)_tmp40C)!= 4)goto _LL2CA;_tmp40D=((struct Cyc_Absyn_PointerType_struct*)
_tmp40C)->f1;_tmp40E=(void*)_tmp40D.elt_typ;_tmp40F=_tmp40D.elt_tq;_tmp410=
_tmp40D.ptr_atts;_tmp411=(void*)_tmp410.rgn;_tmp412=_tmp410.nullable;_tmp413=
_tmp410.bounds;_tmp414=_tmp410.zero_term;_LL2C9:{void*_tmp415=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp413);_LL2CD: if(_tmp415 <= (void*)1?1:*((int*)_tmp415)!= 
0)goto _LL2CF;_LL2CE: {int do_null_check=Cyc_Toc_need_null_check(_tmp2DC);Cyc_Toc_exp_to_c(
nv,_tmp2DC);if(do_null_check)(void*)(_tmp2DC->r=(void*)((void*)(Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(_tmp40B),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416->hd=Cyc_Absyn_copy_exp(_tmp2DC);
_tmp416->tl=0;_tmp416;}),0),0))->r));goto _LL2CC;}_LL2CF: if((int)_tmp415 != 0)goto
_LL2D1;_LL2D0: {struct Cyc_Absyn_Exp*_tmp417=Cyc_Absyn_uint_exp(0,0);_tmp417->topt=({
struct Cyc_Core_Opt*_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418->v=(void*)Cyc_Absyn_uint_typ;
_tmp418;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(_tmp2DC,_tmp417,
0))->r));Cyc_Toc_exp_to_c(nv,e);goto _LL2CC;}_LL2D1: if(_tmp415 <= (void*)1?1:*((
int*)_tmp415)!= 1)goto _LL2CC;_LL2D2:({void*_tmp419[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp41A="exp_to_c: deref w/ AbsUpper_b";
_tag_arr(_tmp41A,sizeof(char),_get_zero_arr_size(_tmp41A,30));}),_tag_arr(
_tmp419,sizeof(void*),0));});_LL2CC:;}goto _LL2C7;_LL2CA:;_LL2CB:({void*_tmp41B[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmp41C="exp_to_c: Deref: non-pointer";_tag_arr(_tmp41C,sizeof(char),
_get_zero_arr_size(_tmp41C,29));}),_tag_arr(_tmp41B,sizeof(void*),0));});_LL2C7:;}
goto _LL1D7;}_LL208: if(*((int*)_tmp2AD)!= 21)goto _LL20A;_tmp2DD=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2AD)->f1;_tmp2DE=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp2AD)->f2;_LL209:
Cyc_Toc_exp_to_c(nv,_tmp2DD);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1D7;
_LL20A: if(*((int*)_tmp2AD)!= 22)goto _LL20C;_tmp2DF=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp2AD)->f1;_tmp2E0=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2AD)->f2;_LL20B: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2DF->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp2DF);Cyc_Toc_exp_to_c(nv,_tmp2DF);{
int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp41E;struct Cyc_Absyn_Tqual _tmp41F;
struct Cyc_Absyn_PtrAtts _tmp420;void*_tmp421;struct Cyc_Absyn_Conref*_tmp422;
struct Cyc_Absyn_Conref*_tmp423;struct Cyc_Absyn_Conref*_tmp424;struct Cyc_Absyn_PtrInfo
_tmp41D=Cyc_Toc_get_ptr_type(e1typ);_tmp41E=(void*)_tmp41D.elt_typ;_tmp41F=
_tmp41D.elt_tq;_tmp420=_tmp41D.ptr_atts;_tmp421=(void*)_tmp420.rgn;_tmp422=
_tmp420.nullable;_tmp423=_tmp420.bounds;_tmp424=_tmp420.zero_term;{void*_tmp425=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp423);struct Cyc_Absyn_Exp*_tmp426;
_LL2D4: if(_tmp425 <= (void*)1?1:*((int*)_tmp425)!= 0)goto _LL2D6;_tmp426=((struct
Cyc_Absyn_Upper_b_struct*)_tmp425)->f1;_LL2D5: {unsigned int _tmp428;int _tmp429;
struct _tuple5 _tmp427=Cyc_Evexp_eval_const_uint_exp(_tmp426);_tmp428=_tmp427.f1;
_tmp429=_tmp427.f2;if(_tmp429){if(_tmp428 < 1)({void*_tmp42A[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp42B="exp_to_c:  AggrArrow_e on pointer of size 0";
_tag_arr(_tmp42B,sizeof(char),_get_zero_arr_size(_tmp42B,44));}),_tag_arr(
_tmp42A,sizeof(void*),0));});if(do_null_check)(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2DF->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp42C=_cycalloc(sizeof(*_tmp42C));_tmp42C->hd=_tmp2DF;_tmp42C->tl=0;_tmp42C;}),
0),0),_tmp2E0,0))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2DF->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp42D[4];_tmp42D[3]=Cyc_Absyn_uint_exp(0,0);_tmp42D[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp41E,0);_tmp42D[1]=_tmp426;_tmp42D[0]=_tmp2DF;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp42D,sizeof(struct Cyc_Absyn_Exp*),
4));}),0),0),_tmp2E0,0))->r));}goto _LL2D3;}_LL2D6: if((int)_tmp425 != 0)goto _LL2D8;
_LL2D7: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp41E);(void*)(_tmp2DF->r=(void*)((
void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp41F),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*_tmp42E[3];_tmp42E[2]=
Cyc_Absyn_uint_exp(0,0);_tmp42E[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp42E[0]=Cyc_Absyn_copy_exp(
_tmp2DF);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp42E,sizeof(struct Cyc_Absyn_Exp*),3));}),0),0))->r));goto _LL2D3;}_LL2D8: if(
_tmp425 <= (void*)1?1:*((int*)_tmp425)!= 1)goto _LL2D3;_LL2D9:({void*_tmp42F[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*
_tmp430="exp_to_c: AggrArrow w/ AbsUpper_b";_tag_arr(_tmp430,sizeof(char),
_get_zero_arr_size(_tmp430,34));}),_tag_arr(_tmp42F,sizeof(void*),0));});_LL2D3:;}
if(is_poly)(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,
0),0))->r));goto _LL1D7;}}_LL20C: if(*((int*)_tmp2AD)!= 23)goto _LL20E;_tmp2E1=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp2AD)->f1;_tmp2E2=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp2AD)->f2;_LL20D: {void*_tmp431=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2E1->topt))->v);{void*_tmp432=_tmp431;struct Cyc_List_List*
_tmp433;struct Cyc_Absyn_PtrInfo _tmp434;void*_tmp435;struct Cyc_Absyn_Tqual _tmp436;
struct Cyc_Absyn_PtrAtts _tmp437;void*_tmp438;struct Cyc_Absyn_Conref*_tmp439;
struct Cyc_Absyn_Conref*_tmp43A;struct Cyc_Absyn_Conref*_tmp43B;_LL2DB: if(_tmp432
<= (void*)3?1:*((int*)_tmp432)!= 9)goto _LL2DD;_tmp433=((struct Cyc_Absyn_TupleType_struct*)
_tmp432)->f1;_LL2DC: Cyc_Toc_exp_to_c(nv,_tmp2E1);Cyc_Toc_exp_to_c(nv,_tmp2E2);{
unsigned int _tmp43D;int _tmp43E;struct _tuple5 _tmp43C=Cyc_Evexp_eval_const_uint_exp(
_tmp2E2);_tmp43D=_tmp43C.f1;_tmp43E=_tmp43C.f2;if(!_tmp43E)({void*_tmp43F[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp440="unknown tuple subscript in translation to C";_tag_arr(_tmp440,sizeof(
char),_get_zero_arr_size(_tmp440,44));}),_tag_arr(_tmp43F,sizeof(void*),0));});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(_tmp2E1,Cyc_Absyn_fieldname((
int)(_tmp43D + 1)),0))->r));goto _LL2DA;}_LL2DD: if(_tmp432 <= (void*)3?1:*((int*)
_tmp432)!= 4)goto _LL2DF;_tmp434=((struct Cyc_Absyn_PointerType_struct*)_tmp432)->f1;
_tmp435=(void*)_tmp434.elt_typ;_tmp436=_tmp434.elt_tq;_tmp437=_tmp434.ptr_atts;
_tmp438=(void*)_tmp437.rgn;_tmp439=_tmp437.nullable;_tmp43A=_tmp437.bounds;
_tmp43B=_tmp437.zero_term;_LL2DE: {struct Cyc_List_List*_tmp441=Cyc_Toc_get_relns(
_tmp2E1);int _tmp442=Cyc_Toc_check_bounds(_tmp441,_tmp2E1,_tmp2E2);if(Cyc_Toc_warn_bounds_checks?
!_tmp442: 0)({struct Cyc_String_pa_struct _tmp445;_tmp445.tag=0;_tmp445.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e));{void*_tmp443[1]={&
_tmp445};Cyc_Tcutil_warn(e->loc,({const char*_tmp444="bounds check necessary for %s";
_tag_arr(_tmp444,sizeof(char),_get_zero_arr_size(_tmp444,30));}),_tag_arr(
_tmp443,sizeof(void*),1));}});Cyc_Toc_exp_to_c(nv,_tmp2E1);Cyc_Toc_exp_to_c(nv,
_tmp2E2);Cyc_Toc_total_bounds_checks ++;{void*_tmp446=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp43A);struct Cyc_Absyn_Exp*_tmp447;void*_tmp448;_LL2E2: if(_tmp446 <= (void*)1?1:*((
int*)_tmp446)!= 0)goto _LL2E4;_tmp447=((struct Cyc_Absyn_Upper_b_struct*)_tmp446)->f1;
_LL2E3: {int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp439);void*ta1=Cyc_Toc_typ_to_c(_tmp435);void*ta2=Cyc_Absyn_cstar_typ(ta1,
_tmp436);if(_tmp442)Cyc_Toc_bounds_checks_eliminated ++;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp43B))(void*)(e->r=(void*)((
void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp449[3];_tmp449[2]=_tmp2E2;_tmp449[1]=_tmp447;_tmp449[0]=
_tmp2E1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp449,sizeof(struct Cyc_Absyn_Exp*),3));}),0),0),0))->r));else{if(possibly_null)(
void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(ta2,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp44A[4];_tmp44A[3]=
_tmp2E2;_tmp44A[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp44A[1]=_tmp447;_tmp44A[0]=
_tmp2E1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp44A,sizeof(struct Cyc_Absyn_Exp*),4));}),0),0),0))->r));else{(void*)(_tmp2E2->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp44B[2];_tmp44B[1]=Cyc_Absyn_copy_exp(_tmp2E2);_tmp44B[0]=
_tmp447;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp44B,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}}}goto _LL2E1;}_LL2E4: if((
int)_tmp446 != 0)goto _LL2E6;_LL2E5: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp435);if(
_tmp442){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(
Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp436),Cyc_Absyn_aggrmember_exp(
_tmp2E1,Cyc_Toc_curr_sp,0),0),_tmp2E2,0))->r));}else{(void*)(e->r=(void*)((void*)(
Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp436),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*_tmp44C[3];_tmp44C[2]=
_tmp2E2;_tmp44C[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp44C[0]=_tmp2E1;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp44C,sizeof(struct Cyc_Absyn_Exp*),
3));}),0),0),0))->r));}goto _LL2E1;}_LL2E6: if(_tmp446 <= (void*)1?1:*((int*)
_tmp446)!= 1)goto _LL2E1;_tmp448=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp446)->f1;_LL2E7: _tmp442=0;{void*_tmp44D=(void*)_tmp2E2->annot;struct Cyc_List_List*
_tmp44E;_LL2E9: if(*((void**)_tmp44D)!= Cyc_CfFlowInfo_HasTagCmps)goto _LL2EB;
_tmp44E=((struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp44D)->f1;_LL2EA: for(0;
_tmp44E != 0;_tmp44E=_tmp44E->tl){if((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp44E->hd)->cmp == (void*)8?Cyc_Tcutil_typecmp((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp44E->hd)->bd,_tmp448)== 0: 0){_tmp442=1;break;}}goto _LL2E8;_LL2EB:;_LL2EC:
goto _LL2E8;_LL2E8:;}if(!_tmp442)({void*_tmp44F[0]={};Cyc_Tcutil_terr(e->loc,({
const char*_tmp450="cannot determine that subscript is in bounds";_tag_arr(
_tmp450,sizeof(char),_get_zero_arr_size(_tmp450,45));}),_tag_arr(_tmp44F,sizeof(
void*),0));});goto _LL2E1;_LL2E1:;}goto _LL2DA;}_LL2DF:;_LL2E0:({void*_tmp451[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmp452="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_arr(_tmp452,
sizeof(char),_get_zero_arr_size(_tmp452,49));}),_tag_arr(_tmp451,sizeof(void*),0));});
_LL2DA:;}goto _LL1D7;}_LL20E: if(*((int*)_tmp2AD)!= 24)goto _LL210;_tmp2E3=((struct
Cyc_Absyn_Tuple_e_struct*)_tmp2AD)->f1;_LL20F: if(!nv->toplevel)(void*)(e->r=(
void*)((void*)(Cyc_Toc_init_tuple(nv,0,0,_tmp2E3))->r));else{struct Cyc_List_List*
_tmp453=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp2E3);struct _tagged_arr*
_tmp454=Cyc_Toc_add_tuple_type(_tmp453);struct Cyc_List_List*dles=0;{int i=1;for(0;
_tmp2E3 != 0;(_tmp2E3=_tmp2E3->tl,i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp2E3->hd);dles=({struct Cyc_List_List*_tmp455=_cycalloc(sizeof(*_tmp455));
_tmp455->hd=({struct _tuple6*_tmp456=_cycalloc(sizeof(*_tmp456));_tmp456->f1=0;
_tmp456->f2=(struct Cyc_Absyn_Exp*)_tmp2E3->hd;_tmp456;});_tmp455->tl=dles;
_tmp455;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,dles,0))->r));}
goto _LL1D7;_LL210: if(*((int*)_tmp2AD)!= 26)goto _LL212;_tmp2E4=((struct Cyc_Absyn_Array_e_struct*)
_tmp2AD)->f1;_LL211:(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,
_tmp2E4,0))->r));{struct Cyc_List_List*_tmp457=_tmp2E4;for(0;_tmp457 != 0;_tmp457=
_tmp457->tl){struct _tuple6 _tmp459;struct Cyc_Absyn_Exp*_tmp45A;struct _tuple6*
_tmp458=(struct _tuple6*)_tmp457->hd;_tmp459=*_tmp458;_tmp45A=_tmp459.f2;Cyc_Toc_exp_to_c(
nv,_tmp45A);}}goto _LL1D7;_LL212: if(*((int*)_tmp2AD)!= 27)goto _LL214;_tmp2E5=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp2AD)->f1;_tmp2E6=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2AD)->f2;_tmp2E7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2AD)->f3;
_tmp2E8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2AD)->f4;_LL213: {
unsigned int _tmp45C;int _tmp45D;struct _tuple5 _tmp45B=Cyc_Evexp_eval_const_uint_exp(
_tmp2E6);_tmp45C=_tmp45B.f1;_tmp45D=_tmp45B.f2;{void*_tmp45E=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp2E7->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp2E7);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp2E7)){if(!_tmp45D)({
void*_tmp45F[0]={};Cyc_Tcutil_terr(_tmp2E6->loc,({const char*_tmp460="cannot determine value of constant";
_tag_arr(_tmp460,sizeof(char),_get_zero_arr_size(_tmp460,35));}),_tag_arr(
_tmp45F,sizeof(void*),0));});{unsigned int i=0;for(0;i < _tmp45C;i ++){es=({struct
Cyc_List_List*_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461->hd=({struct _tuple6*
_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462->f1=0;_tmp462->f2=_tmp2E7;_tmp462;});
_tmp461->tl=es;_tmp461;});}}if(_tmp2E8){struct Cyc_Absyn_Exp*_tmp463=Cyc_Absyn_cast_exp(
_tmp45E,Cyc_Absyn_uint_exp(0,0),0);es=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp464=
_cycalloc(sizeof(*_tmp464));_tmp464->hd=({struct _tuple6*_tmp465=_cycalloc(
sizeof(*_tmp465));_tmp465->f1=0;_tmp465->f2=_tmp463;_tmp465;});_tmp464->tl=0;
_tmp464;}));}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,es,0))->r));
goto _LL1D7;}}}_LL214: if(*((int*)_tmp2AD)!= 28)goto _LL216;_tmp2E9=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2AD)->f1;_tmp2EA=((struct Cyc_Absyn_Struct_e_struct*)_tmp2AD)->f2;_tmp2EB=((
struct Cyc_Absyn_Struct_e_struct*)_tmp2AD)->f3;_tmp2EC=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2AD)->f4;_LL215: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp2EA != 0,0,0,_tmp2EB,_tmp2E9))->r));else{if(_tmp2EC == 0)({void*
_tmp466[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp467="Struct_e: missing aggrdecl pointer";_tag_arr(_tmp467,sizeof(
char),_get_zero_arr_size(_tmp467,35));}),_tag_arr(_tmp466,sizeof(void*),0));});{
struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_check_null(_tmp2EC);
struct _RegionHandle _tmp468=_new_region("rgn");struct _RegionHandle*rgn=& _tmp468;
_push_region(rgn);{struct Cyc_List_List*_tmp469=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp2EB,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);struct Cyc_List_List*
_tmp46A=0;struct Cyc_List_List*_tmp46B=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp46B != 0;_tmp46B=_tmp46B->tl){struct Cyc_List_List*
_tmp46C=_tmp469;for(0;_tmp46C != 0;_tmp46C=_tmp46C->tl){if((*((struct _tuple11*)
_tmp46C->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp46B->hd){struct _tuple11
_tmp46E;struct Cyc_Absyn_Aggrfield*_tmp46F;struct Cyc_Absyn_Exp*_tmp470;struct
_tuple11*_tmp46D=(struct _tuple11*)_tmp46C->hd;_tmp46E=*_tmp46D;_tmp46F=_tmp46E.f1;
_tmp470=_tmp46E.f2;{void*_tmp471=(void*)_tmp46F->type;Cyc_Toc_exp_to_c(nv,
_tmp470);if(Cyc_Toc_is_void_star(_tmp471))(void*)(_tmp470->r=(void*)((void*)(Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp470->r,0),0))->r));
_tmp46A=({struct Cyc_List_List*_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->hd=({
struct _tuple6*_tmp473=_cycalloc(sizeof(*_tmp473));_tmp473->f1=0;_tmp473->f2=
_tmp470;_tmp473;});_tmp472->tl=_tmp46A;_tmp472;});break;}}}}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_unresolvedmem_exp(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp46A),0))->r));};_pop_region(rgn);}}goto _LL1D7;_LL216:
if(*((int*)_tmp2AD)!= 29)goto _LL218;_tmp2ED=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2AD)->f1;_tmp2EE=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2AD)->f2;_LL217: {
struct Cyc_List_List*fs;{void*_tmp474=Cyc_Tcutil_compress(_tmp2ED);struct Cyc_List_List*
_tmp475;_LL2EE: if(_tmp474 <= (void*)3?1:*((int*)_tmp474)!= 11)goto _LL2F0;_tmp475=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp474)->f2;_LL2EF: fs=_tmp475;goto _LL2ED;
_LL2F0:;_LL2F1:({struct Cyc_String_pa_struct _tmp478;_tmp478.tag=0;_tmp478.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp2ED));{void*
_tmp476[1]={& _tmp478};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp477="anon struct has type %s";_tag_arr(_tmp477,sizeof(char),
_get_zero_arr_size(_tmp477,24));}),_tag_arr(_tmp476,sizeof(void*),1));}});_LL2ED:;}{
struct _RegionHandle _tmp479=_new_region("rgn");struct _RegionHandle*rgn=& _tmp479;
_push_region(rgn);{struct Cyc_List_List*_tmp47A=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp2EE,
fs);for(0;_tmp47A != 0;_tmp47A=_tmp47A->tl){struct _tuple11 _tmp47C;struct Cyc_Absyn_Aggrfield*
_tmp47D;struct Cyc_Absyn_Exp*_tmp47E;struct _tuple11*_tmp47B=(struct _tuple11*)
_tmp47A->hd;_tmp47C=*_tmp47B;_tmp47D=_tmp47C.f1;_tmp47E=_tmp47C.f2;{void*_tmp47F=(
void*)_tmp47D->type;Cyc_Toc_exp_to_c(nv,_tmp47E);if(Cyc_Toc_is_void_star(_tmp47F))(
void*)(_tmp47E->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp47E->r,0),0))->r));}}(void*)(e->r=(void*)((void*)(
Cyc_Absyn_unresolvedmem_exp(0,_tmp2EE,0))->r));};_pop_region(rgn);}goto _LL1D7;}
_LL218: if(*((int*)_tmp2AD)!= 30)goto _LL21A;_tmp2EF=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp2AD)->f1;if(_tmp2EF != 0)goto _LL21A;_tmp2F0=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp2AD)->f2;_tmp2F1=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2AD)->f3;_LL219: {
struct _tuple1*qv=_tmp2F1->name;struct Cyc_Absyn_Exp*tag_exp=_tmp2F0->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp2F0,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL1D7;}_LL21A: if(*((int*)_tmp2AD)!= 30)goto _LL21C;
_tmp2F2=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2AD)->f1;_tmp2F3=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp2AD)->f2;_tmp2F4=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2AD)->f3;_LL21B: {
struct _tuple1*_tmp480=_tmp2F4->name;struct Cyc_List_List*_tmp481=_tmp2F4->typs;
struct _tuple1*_tmp482=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp483=Cyc_Absyn_var_exp(
_tmp482,0);void*_tmp484=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp480,({
const char*_tmp492="_struct";_tag_arr(_tmp492,sizeof(char),_get_zero_arr_size(
_tmp492,8));})));struct Cyc_Absyn_Exp*tag_exp=_tmp2F3->is_xtunion?Cyc_Absyn_var_exp(
_tmp480,0): Cyc_Toc_tunion_tag(_tmp2F3,_tmp480,1);if(nv->toplevel){struct Cyc_List_List*
dles=0;for(0;_tmp2F2 != 0;(_tmp2F2=_tmp2F2->tl,_tmp481=_tmp481->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp2F2->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp481))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Absyn_cast_exp(field_typ,
cur_e,0);dles=({struct Cyc_List_List*_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485->hd=({
struct _tuple6*_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486->f1=0;_tmp486->f2=
cur_e;_tmp486;});_tmp485->tl=dles;_tmp485;});}dles=({struct Cyc_List_List*_tmp487=
_cycalloc(sizeof(*_tmp487));_tmp487->hd=({struct _tuple6*_tmp488=_cycalloc(
sizeof(*_tmp488));_tmp488->f1=0;_tmp488->f2=tag_exp;_tmp488;});_tmp487->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp487;});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(({struct Cyc_Core_Opt*
_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489->v=Cyc_Toc_collapse_qvar_tag(_tmp480,({
const char*_tmp48A="_struct";_tag_arr(_tmp48A,sizeof(char),_get_zero_arr_size(
_tmp48A,8));}));_tmp489;}),dles,0))->r));}else{struct Cyc_List_List*_tmp48B=({
struct Cyc_List_List*_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491->hd=Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(_tmp483,Cyc_Toc_tag_sp,0),tag_exp,0);_tmp491->tl=0;
_tmp491;});{int i=1;for(0;_tmp2F2 != 0;(((_tmp2F2=_tmp2F2->tl,i ++)),_tmp481=
_tmp481->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp2F2->hd;void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)_check_null(
_tmp481))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))
cur_e=Cyc_Absyn_cast_exp(field_typ,cur_e,0);{struct Cyc_Absyn_Stmt*_tmp48C=Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(_tmp483,Cyc_Absyn_fieldname(i),0),cur_e,0);_tmp48B=({
struct Cyc_List_List*_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D->hd=_tmp48C;
_tmp48D->tl=_tmp48B;_tmp48D;});}}}{struct Cyc_Absyn_Stmt*_tmp48E=Cyc_Absyn_exp_stmt(
_tmp483,0);struct Cyc_Absyn_Stmt*_tmp48F=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp490=_cycalloc(
sizeof(*_tmp490));_tmp490->hd=_tmp48E;_tmp490->tl=_tmp48B;_tmp490;})),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp482,_tmp484,0,
_tmp48F,0),0))->r));}}goto _LL1D7;}_LL21C: if(*((int*)_tmp2AD)!= 31)goto _LL21E;
_LL21D: goto _LL21F;_LL21E: if(*((int*)_tmp2AD)!= 32)goto _LL220;_LL21F: goto _LL1D7;
_LL220: if(*((int*)_tmp2AD)!= 33)goto _LL222;_tmp2F5=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp2AD)->f1;_tmp2F6=_tmp2F5.is_calloc;_tmp2F7=_tmp2F5.rgn;_tmp2F8=_tmp2F5.elt_type;
_tmp2F9=_tmp2F5.num_elts;_tmp2FA=_tmp2F5.fat_result;_LL221: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp2F8)));Cyc_Toc_exp_to_c(nv,_tmp2F9);if(_tmp2FA){struct
_tuple1*_tmp493=Cyc_Toc_temp_var();struct _tuple1*_tmp494=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp2F6){xexp=_tmp2F9;if(_tmp2F7 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp2F7);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp493,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp2F8,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp493,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp495[3];_tmp495[2]=Cyc_Absyn_var_exp(_tmp493,0);_tmp495[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp495[0]=Cyc_Absyn_var_exp(_tmp494,0);((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp495,sizeof(
struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp2F9,0);if(_tmp2F7 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp2F7);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp493,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp2F8,Cyc_Absyn_var_exp(_tmp493,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp496[3];
_tmp496[2]=Cyc_Absyn_var_exp(_tmp493,0);_tmp496[1]=Cyc_Absyn_uint_exp(1,0);
_tmp496[0]=Cyc_Absyn_var_exp(_tmp494,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp496,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);}{struct Cyc_Absyn_Stmt*_tmp497=Cyc_Absyn_declare_stmt(_tmp493,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp494,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp497,0))->r));}}else{if(_tmp2F7 != 0){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_check_null(_tmp2F7);Cyc_Toc_exp_to_c(
nv,rgn);(void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp2F8,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL1D7;}_LL222: if(*((int*)_tmp2AD)!= 35)goto _LL224;_tmp2FB=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp2AD)->f1;_LL223: Cyc_Toc_stmt_to_c(nv,
_tmp2FB);goto _LL1D7;_LL224: if(*((int*)_tmp2AD)!= 34)goto _LL226;_LL225:({void*
_tmp498[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp499="UnresolvedMem";_tag_arr(_tmp499,sizeof(char),
_get_zero_arr_size(_tmp499,14));}),_tag_arr(_tmp498,sizeof(void*),0));});_LL226:
if(*((int*)_tmp2AD)!= 25)goto _LL228;_LL227:({void*_tmp49A[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp49B="compoundlit";
_tag_arr(_tmp49B,sizeof(char),_get_zero_arr_size(_tmp49B,12));}),_tag_arr(
_tmp49A,sizeof(void*),0));});_LL228: if(*((int*)_tmp2AD)!= 36)goto _LL22A;_tmp2FC=((
struct Cyc_Absyn_Codegen_e_struct*)_tmp2AD)->f1;_LL229:({void*_tmp49C[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp49D="codegen";
_tag_arr(_tmp49D,sizeof(char),_get_zero_arr_size(_tmp49D,8));}),_tag_arr(_tmp49C,
sizeof(void*),0));});_LL22A: if(*((int*)_tmp2AD)!= 37)goto _LL1D7;_LL22B:({void*
_tmp49E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp49F="fill";_tag_arr(_tmp49F,sizeof(char),_get_zero_arr_size(
_tmp49F,5));}),_tag_arr(_tmp49E,sizeof(void*),0));});_LL1D7:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple12{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple13{struct _tuple1*f1;void*f2;};struct
_tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple12 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct
Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{void*_tmp4A0=(void*)p->r;struct Cyc_Absyn_Vardecl*
_tmp4A1;struct Cyc_Absyn_Vardecl _tmp4A2;struct _tuple1*_tmp4A3;struct Cyc_Absyn_Vardecl*
_tmp4A4;void*_tmp4A5;int _tmp4A6;char _tmp4A7;struct _tagged_arr _tmp4A8;struct Cyc_Absyn_Enumdecl*
_tmp4A9;struct Cyc_Absyn_Enumfield*_tmp4AA;void*_tmp4AB;struct Cyc_Absyn_Enumfield*
_tmp4AC;struct Cyc_Absyn_Tuniondecl*_tmp4AD;struct Cyc_Absyn_Tunionfield*_tmp4AE;
struct Cyc_List_List*_tmp4AF;struct Cyc_Absyn_Pat*_tmp4B0;struct Cyc_Absyn_Pat
_tmp4B1;void*_tmp4B2;struct Cyc_Absyn_Tuniondecl*_tmp4B3;struct Cyc_Absyn_Tunionfield*
_tmp4B4;struct Cyc_List_List*_tmp4B5;struct Cyc_List_List*_tmp4B6;struct Cyc_List_List*
_tmp4B7;struct Cyc_Absyn_AggrInfo _tmp4B8;void*_tmp4B9;struct Cyc_List_List*_tmp4BA;
struct Cyc_Absyn_Pat*_tmp4BB;_LL2F3: if(_tmp4A0 <= (void*)2?1:*((int*)_tmp4A0)!= 0)
goto _LL2F5;_tmp4A1=((struct Cyc_Absyn_Var_p_struct*)_tmp4A0)->f1;_tmp4A2=*_tmp4A1;
_tmp4A3=_tmp4A2.name;_LL2F4: nv=Cyc_Toc_add_varmap(nv,_tmp4A3,r);goto _LL2F6;
_LL2F5: if((int)_tmp4A0 != 0)goto _LL2F7;_LL2F6: s=Cyc_Toc_skip_stmt_dl();goto _LL2F2;
_LL2F7: if(_tmp4A0 <= (void*)2?1:*((int*)_tmp4A0)!= 6)goto _LL2F9;_tmp4A4=((struct
Cyc_Absyn_Reference_p_struct*)_tmp4A0)->f1;_LL2F8: {struct _tuple1*_tmp4BC=Cyc_Toc_temp_var();
decls=({struct Cyc_List_List*_tmp4BD=_region_malloc(rgn,sizeof(*_tmp4BD));_tmp4BD->hd=({
struct _tuple13*_tmp4BE=_region_malloc(rgn,sizeof(*_tmp4BE));_tmp4BE->f1=_tmp4BC;
_tmp4BE->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp4BE;});
_tmp4BD->tl=decls;_tmp4BD;});nv=Cyc_Toc_add_varmap(nv,_tmp4A4->name,Cyc_Absyn_var_exp(
_tmp4BC,0));s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp4BC,0),Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(
path,0),0),0);goto _LL2F2;}_LL2F9: if((int)_tmp4A0 != 1)goto _LL2FB;_LL2FA: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL2F2;_LL2FB: if(_tmp4A0 <= (void*)
2?1:*((int*)_tmp4A0)!= 1)goto _LL2FD;_tmp4A5=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp4A0)->f1;_tmp4A6=((struct Cyc_Absyn_Int_p_struct*)_tmp4A0)->f2;_LL2FC: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_int_exp(_tmp4A5,_tmp4A6,0),fail_stmt);goto _LL2F2;_LL2FD: if(_tmp4A0 <= (
void*)2?1:*((int*)_tmp4A0)!= 2)goto _LL2FF;_tmp4A7=((struct Cyc_Absyn_Char_p_struct*)
_tmp4A0)->f1;_LL2FE: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp4A7,0),
fail_stmt);goto _LL2F2;_LL2FF: if(_tmp4A0 <= (void*)2?1:*((int*)_tmp4A0)!= 3)goto
_LL301;_tmp4A8=((struct Cyc_Absyn_Float_p_struct*)_tmp4A0)->f1;_LL300: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_float_exp(_tmp4A8,0),fail_stmt);goto _LL2F2;_LL301: if(_tmp4A0 <= (void*)
2?1:*((int*)_tmp4A0)!= 9)goto _LL303;_tmp4A9=((struct Cyc_Absyn_Enum_p_struct*)
_tmp4A0)->f1;_tmp4AA=((struct Cyc_Absyn_Enum_p_struct*)_tmp4A0)->f2;_LL302: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp4BF=_cycalloc(
sizeof(*_tmp4BF));_tmp4BF[0]=({struct Cyc_Absyn_Enum_e_struct _tmp4C0;_tmp4C0.tag=
31;_tmp4C0.f1=_tmp4AA->name;_tmp4C0.f2=(struct Cyc_Absyn_Enumdecl*)_tmp4A9;
_tmp4C0.f3=(struct Cyc_Absyn_Enumfield*)_tmp4AA;_tmp4C0;});_tmp4BF;}),0),
fail_stmt);goto _LL2F2;_LL303: if(_tmp4A0 <= (void*)2?1:*((int*)_tmp4A0)!= 10)goto
_LL305;_tmp4AB=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp4A0)->f1;_tmp4AC=((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp4A0)->f2;_LL304: s=Cyc_Toc_if_neq_stmt(r,
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp4C1=_cycalloc(
sizeof(*_tmp4C1));_tmp4C1[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp4C2;_tmp4C2.tag=
32;_tmp4C2.f1=_tmp4AC->name;_tmp4C2.f2=(void*)_tmp4AB;_tmp4C2.f3=(struct Cyc_Absyn_Enumfield*)
_tmp4AC;_tmp4C2;});_tmp4C1;}),0),fail_stmt);goto _LL2F2;_LL305: if(_tmp4A0 <= (void*)
2?1:*((int*)_tmp4A0)!= 8)goto _LL307;_tmp4AD=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp4A0)->f1;_tmp4AE=((struct Cyc_Absyn_Tunion_p_struct*)_tmp4A0)->f2;_tmp4AF=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp4A0)->f3;if(_tmp4AF != 0)goto _LL307;_LL306: {
struct Cyc_Absyn_Exp*cmp_exp;if(_tmp4AD->is_xtunion)cmp_exp=Cyc_Absyn_var_exp(
_tmp4AE->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(_tmp4AD,_tmp4AE->name,0);r=Cyc_Absyn_cast_exp(
Cyc_Absyn_sint_typ,r,0);}s=Cyc_Toc_if_neq_stmt(r,cmp_exp,fail_stmt);goto _LL2F2;}
_LL307: if(_tmp4A0 <= (void*)2?1:*((int*)_tmp4A0)!= 5)goto _LL309;_tmp4B0=((struct
Cyc_Absyn_Pointer_p_struct*)_tmp4A0)->f1;_tmp4B1=*_tmp4B0;_tmp4B2=(void*)_tmp4B1.r;
if(_tmp4B2 <= (void*)2?1:*((int*)_tmp4B2)!= 8)goto _LL309;_tmp4B3=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp4B2)->f1;_tmp4B4=((struct Cyc_Absyn_Tunion_p_struct*)_tmp4B2)->f2;_tmp4B5=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp4B2)->f3;if(!(_tmp4B5 != 0))goto _LL309;
_LL308: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(
_tmp4B4->name,({const char*_tmp4CF="_struct";_tag_arr(_tmp4CF,sizeof(char),
_get_zero_arr_size(_tmp4CF,8));}));struct Cyc_Absyn_Exp*rcast=Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r,0);struct Cyc_List_List*
_tmp4C3=_tmp4B4->typs;for(0;_tmp4B5 != 0;(((_tmp4B5=_tmp4B5->tl,_tmp4C3=((struct
Cyc_List_List*)_check_null(_tmp4C3))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp4C4=(
struct Cyc_Absyn_Pat*)_tmp4B5->hd;if((void*)_tmp4C4->r == (void*)0)continue;{void*
_tmp4C5=(*((struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp4C3))->hd)).f2;
struct _tuple1*_tmp4C6=Cyc_Toc_temp_var();void*_tmp4C7=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4C4->topt))->v;void*_tmp4C8=Cyc_Toc_typ_to_c(_tmp4C7);struct Cyc_Absyn_Exp*
_tmp4C9=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp4C5)))_tmp4C9=Cyc_Absyn_cast_exp(_tmp4C8,_tmp4C9,0);decls=({
struct Cyc_List_List*_tmp4CA=_region_malloc(rgn,sizeof(*_tmp4CA));_tmp4CA->hd=({
struct _tuple13*_tmp4CB=_region_malloc(rgn,sizeof(*_tmp4CB));_tmp4CB->f1=_tmp4C6;
_tmp4CB->f2=_tmp4C8;_tmp4CB;});_tmp4CA->tl=decls;_tmp4CA;});{struct _tuple12
_tmp4CC=Cyc_Toc_xlate_pat(nv,rgn,_tmp4C7,Cyc_Absyn_var_exp(_tmp4C6,0),_tmp4C9,
_tmp4C4,fail_stmt,decls);nv=_tmp4CC.f1;decls=_tmp4CC.f2;{struct Cyc_Absyn_Stmt*
_tmp4CD=_tmp4CC.f3;struct Cyc_Absyn_Stmt*_tmp4CE=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp4C6,0),_tmp4C9,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp4CE,_tmp4CD,
0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(_tmp4B3->is_xtunion){struct Cyc_Absyn_Exp*
e2=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),
r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(
_tmp4B4->name,0);test_exp=Cyc_Absyn_neq_exp(e1,e,0);}else{int max_tag=Cyc_Toc_num_void_tags(
_tmp4B3);struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)
max_tag,0);struct Cyc_Absyn_Exp*e3=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,
Cyc_Toc_mt_tq),r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e5=Cyc_Absyn_lte_exp(r,Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),max_tag_exp,
0),0);struct Cyc_Absyn_Exp*e=Cyc_Toc_tunion_tag(_tmp4B3,_tmp4B4->name,1);test_exp=
max_tag > 0?Cyc_Absyn_or_exp(e5,Cyc_Absyn_neq_exp(e1,e,0),0): Cyc_Absyn_neq_exp(e1,
e,0);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);goto _LL2F2;}}_LL309: if(_tmp4A0 <= (void*)2?1:*((int*)_tmp4A0)!= 8)goto
_LL30B;_tmp4B6=((struct Cyc_Absyn_Tunion_p_struct*)_tmp4A0)->f3;_LL30A: _tmp4B7=
_tmp4B6;goto _LL30C;_LL30B: if(_tmp4A0 <= (void*)2?1:*((int*)_tmp4A0)!= 4)goto
_LL30D;_tmp4B7=((struct Cyc_Absyn_Tuple_p_struct*)_tmp4A0)->f1;_LL30C: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;for(0;_tmp4B7 != 0;(_tmp4B7=_tmp4B7->tl,++ cnt)){struct Cyc_Absyn_Pat*
_tmp4D0=(struct Cyc_Absyn_Pat*)_tmp4B7->hd;if((void*)_tmp4D0->r == (void*)0)
continue;{struct _tuple1*_tmp4D1=Cyc_Toc_temp_var();void*_tmp4D2=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4D0->topt))->v;decls=({struct Cyc_List_List*_tmp4D3=
_region_malloc(rgn,sizeof(*_tmp4D3));_tmp4D3->hd=({struct _tuple13*_tmp4D4=
_region_malloc(rgn,sizeof(*_tmp4D4));_tmp4D4->f1=_tmp4D1;_tmp4D4->f2=Cyc_Toc_typ_to_c(
_tmp4D2);_tmp4D4;});_tmp4D3->tl=decls;_tmp4D3;});{struct _tuple12 _tmp4D5=Cyc_Toc_xlate_pat(
nv,rgn,_tmp4D2,Cyc_Absyn_var_exp(_tmp4D1,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp4D0,fail_stmt,decls);nv=_tmp4D5.f1;decls=_tmp4D5.f2;{struct Cyc_Absyn_Stmt*
_tmp4D6=_tmp4D5.f3;struct Cyc_Absyn_Stmt*_tmp4D7=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp4D1,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp4D7,_tmp4D6,0),0);}}}}goto _LL2F2;}_LL30D: if(_tmp4A0 <= (
void*)2?1:*((int*)_tmp4A0)!= 7)goto _LL30F;_tmp4B8=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp4A0)->f1;_tmp4B9=(void*)_tmp4B8.aggr_info;_tmp4BA=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp4A0)->f3;_LL30E: {struct Cyc_Absyn_Aggrdecl*_tmp4D8=Cyc_Absyn_get_known_aggrdecl(
_tmp4B9);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp4BA != 0;_tmp4BA=_tmp4BA->tl){struct
_tuple14*_tmp4D9=(struct _tuple14*)_tmp4BA->hd;struct Cyc_Absyn_Pat*_tmp4DA=(*
_tmp4D9).f2;if((void*)_tmp4DA->r == (void*)0)continue;{struct _tagged_arr*f;{void*
_tmp4DB=(void*)((struct Cyc_List_List*)_check_null((*_tmp4D9).f1))->hd;struct
_tagged_arr*_tmp4DC;_LL316: if(*((int*)_tmp4DB)!= 1)goto _LL318;_tmp4DC=((struct
Cyc_Absyn_FieldName_struct*)_tmp4DB)->f1;_LL317: f=_tmp4DC;goto _LL315;_LL318:;
_LL319:(int)_throw((void*)Cyc_Toc_Match_error);_LL315:;}{struct _tuple1*_tmp4DD=
Cyc_Toc_temp_var();void*_tmp4DE=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4DA->topt))->v;
void*_tmp4DF=Cyc_Toc_typ_to_c(_tmp4DE);decls=({struct Cyc_List_List*_tmp4E0=
_region_malloc(rgn,sizeof(*_tmp4E0));_tmp4E0->hd=({struct _tuple13*_tmp4E1=
_region_malloc(rgn,sizeof(*_tmp4E1));_tmp4E1->f1=_tmp4DD;_tmp4E1->f2=_tmp4DF;
_tmp4E1;});_tmp4E0->tl=decls;_tmp4E0;});{struct _tuple12 _tmp4E2=Cyc_Toc_xlate_pat(
nv,rgn,_tmp4DE,Cyc_Absyn_var_exp(_tmp4DD,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp4DA,fail_stmt,decls);nv=_tmp4E2.f1;decls=_tmp4E2.f2;{struct Cyc_Absyn_Exp*
_tmp4E3=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((void*)((struct
Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4D8->impl))->fields,f)))->type))_tmp4E3=Cyc_Absyn_cast_exp(
_tmp4DF,_tmp4E3,0);{struct Cyc_Absyn_Stmt*_tmp4E4=_tmp4E2.f3;struct Cyc_Absyn_Stmt*
_tmp4E5=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp4DD,0),_tmp4E3,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp4E5,_tmp4E4,0),0);}}}}}}goto _LL2F2;}_LL30F: if(_tmp4A0 <= (
void*)2?1:*((int*)_tmp4A0)!= 5)goto _LL311;_tmp4BB=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp4A0)->f1;_LL310: {struct _tuple1*_tmp4E6=Cyc_Toc_temp_var();void*_tmp4E7=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4BB->topt))->v;decls=({struct Cyc_List_List*
_tmp4E8=_region_malloc(rgn,sizeof(*_tmp4E8));_tmp4E8->hd=({struct _tuple13*
_tmp4E9=_region_malloc(rgn,sizeof(*_tmp4E9));_tmp4E9->f1=_tmp4E6;_tmp4E9->f2=Cyc_Toc_typ_to_c(
_tmp4E7);_tmp4E9;});_tmp4E8->tl=decls;_tmp4E8;});{struct _tuple12 _tmp4EA=Cyc_Toc_xlate_pat(
nv,rgn,_tmp4E7,Cyc_Absyn_var_exp(_tmp4E6,0),Cyc_Absyn_deref_exp(path,0),_tmp4BB,
fail_stmt,decls);nv=_tmp4EA.f1;decls=_tmp4EA.f2;{struct Cyc_Absyn_Stmt*_tmp4EB=
_tmp4EA.f3;struct Cyc_Absyn_Stmt*_tmp4EC=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp4E6,0),Cyc_Absyn_deref_exp(r,0),0),_tmp4EB,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp4EC,0);else{s=_tmp4EC;}goto
_LL2F2;}}}_LL311: if(_tmp4A0 <= (void*)2?1:*((int*)_tmp4A0)!= 11)goto _LL313;_LL312:({
void*_tmp4ED[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4EE="unknownid";_tag_arr(_tmp4EE,sizeof(char),_get_zero_arr_size(
_tmp4EE,10));}),_tag_arr(_tmp4ED,sizeof(void*),0));});_LL313: if(_tmp4A0 <= (void*)
2?1:*((int*)_tmp4A0)!= 12)goto _LL2F2;_LL314:({void*_tmp4EF[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp4F0="unknowncall";
_tag_arr(_tmp4F0,sizeof(char),_get_zero_arr_size(_tmp4F0,12));}),_tag_arr(
_tmp4EF,sizeof(void*),0));});_LL2F2:;}return({struct _tuple12 _tmp4F1;_tmp4F1.f1=
nv;_tmp4F1.f2=decls;_tmp4F1.f3=s;_tmp4F1;});}struct _tuple15{struct _tagged_arr*f1;
struct _tagged_arr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple15*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){return({struct _tuple15*
_tmp4F2=_region_malloc(r,sizeof(*_tmp4F2));_tmp4F2->f1=Cyc_Toc_fresh_label();
_tmp4F2->f2=Cyc_Toc_fresh_label();_tmp4F2->f3=sc;_tmp4F2;});}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp4F3=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp4F4=Cyc_Tcutil_compress(
_tmp4F3);_LL31B: if(_tmp4F4 <= (void*)3?1:*((int*)_tmp4F4)!= 5)goto _LL31D;_LL31C:
goto _LL31E;_LL31D: if(_tmp4F4 <= (void*)3?1:*((int*)_tmp4F4)!= 12)goto _LL31F;
_LL31E: leave_as_switch=1;goto _LL31A;_LL31F:;_LL320: leave_as_switch=0;goto _LL31A;
_LL31A:;}{struct Cyc_List_List*_tmp4F5=scs;for(0;_tmp4F5 != 0;_tmp4F5=_tmp4F5->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)
_tmp4F5->hd)->pat_vars))->v != 0?1:((struct Cyc_Absyn_Switch_clause*)_tmp4F5->hd)->where_clause
!= 0){leave_as_switch=0;break;}}}if(leave_as_switch){struct _tagged_arr*next_l=
Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp4F6=scs;for(0;_tmp4F6 != 0;_tmp4F6=
_tmp4F6->tl){struct Cyc_Absyn_Stmt*_tmp4F7=((struct Cyc_Absyn_Switch_clause*)
_tmp4F6->hd)->body;((struct Cyc_Absyn_Switch_clause*)_tmp4F6->hd)->body=Cyc_Absyn_label_stmt(
next_l,_tmp4F7,0);next_l=Cyc_Toc_fresh_label();Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
nv,next_l),_tmp4F7);}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct
_tagged_arr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle _tmp4F8=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp4F8;_push_region(rgn);{struct Cyc_List_List*lscs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple15*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;{struct Cyc_List_List*
_tmp4F9=lscs;for(0;_tmp4F9 != 0;_tmp4F9=_tmp4F9->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple15*)_tmp4F9->hd)).f3;struct _tagged_arr*fail_lab=_tmp4F9->tl == 
0?end_l:(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmp4F9->tl))->hd)).f1;
struct Cyc_Toc_Env*_tmp4FB;struct Cyc_List_List*_tmp4FC;struct Cyc_Absyn_Stmt*
_tmp4FD;struct _tuple12 _tmp4FA=Cyc_Toc_xlate_pat(nv,rgn,_tmp4F3,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp4FB=_tmp4FA.f1;_tmp4FC=_tmp4FA.f2;
_tmp4FD=_tmp4FA.f3;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp4FE=(struct
Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp4FB,_tmp4FE);
_tmp4FD=Cyc_Absyn_seq_stmt(_tmp4FD,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((
void*)11,_tmp4FE,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}
decls=_tmp4FC;nvs=({struct Cyc_List_List*_tmp4FF=_region_malloc(rgn,sizeof(*
_tmp4FF));_tmp4FF->hd=_tmp4FB;_tmp4FF->tl=nvs;_tmp4FF;});test_stmts=({struct Cyc_List_List*
_tmp500=_region_malloc(rgn,sizeof(*_tmp500));_tmp500->hd=_tmp4FD;_tmp500->tl=
test_stmts;_tmp500;});}}nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
nvs);test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
test_stmts);{struct Cyc_List_List*stmts=0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),
test_stmts=test_stmts->tl)){struct _tuple15 _tmp502;struct _tagged_arr*_tmp503;
struct _tagged_arr*_tmp504;struct Cyc_Absyn_Switch_clause*_tmp505;struct _tuple15*
_tmp501=(struct _tuple15*)lscs->hd;_tmp502=*_tmp501;_tmp503=_tmp502.f1;_tmp504=
_tmp502.f2;_tmp505=_tmp502.f3;{struct Cyc_Toc_Env*_tmp506=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp505->body;
if(lscs->tl != 0){struct _tuple15 _tmp508;struct _tagged_arr*_tmp509;struct Cyc_Absyn_Switch_clause*
_tmp50A;struct _tuple15*_tmp507=(struct _tuple15*)((struct Cyc_List_List*)
_check_null(lscs->tl))->hd;_tmp508=*_tmp507;_tmp509=_tmp508.f2;_tmp50A=_tmp508.f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp506,end_l,_tmp509,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp50A->pat_vars))->v,(
struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp506,end_l),s);}s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_label_stmt(_tmp503,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)
_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(_tmp504,s,0),0);stmts=({
struct Cyc_List_List*_tmp50B=_region_malloc(rgn,sizeof(*_tmp50B));_tmp50B->hd=s;
_tmp50B->tl=stmts;_tmp50B;});}}{struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(
end_l,Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct
_tuple13 _tmp50D;struct _tuple1*_tmp50E;void*_tmp50F;struct _tuple13*_tmp50C=(
struct _tuple13*)((struct Cyc_List_List*)_check_null(decls))->hd;_tmp50D=*_tmp50C;
_tmp50E=_tmp50D.f1;_tmp50F=_tmp50D.f2;res=Cyc_Absyn_declare_stmt(_tmp50E,_tmp50F,
0,res,0);}(void*)(whole_s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,
res,0))->r));}}};_pop_region(rgn);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp510=
_cycalloc(sizeof(*_tmp510));_tmp510->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),
0);_tmp510->tl=0;_tmp510;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp511=(void*)s->r;
struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Stmt*_tmp513;struct Cyc_Absyn_Stmt*
_tmp514;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Stmt*
_tmp517;struct Cyc_Absyn_Stmt*_tmp518;struct _tuple3 _tmp519;struct Cyc_Absyn_Exp*
_tmp51A;struct Cyc_Absyn_Stmt*_tmp51B;struct Cyc_Absyn_Stmt*_tmp51C;struct Cyc_Absyn_Stmt*
_tmp51D;struct Cyc_Absyn_Stmt*_tmp51E;struct Cyc_Absyn_Exp*_tmp51F;struct _tuple3
_tmp520;struct Cyc_Absyn_Exp*_tmp521;struct _tuple3 _tmp522;struct Cyc_Absyn_Exp*
_tmp523;struct Cyc_Absyn_Stmt*_tmp524;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_List_List*
_tmp526;struct Cyc_List_List*_tmp527;struct Cyc_Absyn_Switch_clause**_tmp528;
struct Cyc_Absyn_Decl*_tmp529;struct Cyc_Absyn_Stmt*_tmp52A;struct _tagged_arr*
_tmp52B;struct Cyc_Absyn_Stmt*_tmp52C;struct Cyc_Absyn_Stmt*_tmp52D;struct _tuple3
_tmp52E;struct Cyc_Absyn_Exp*_tmp52F;struct Cyc_Absyn_Stmt*_tmp530;struct Cyc_List_List*
_tmp531;struct Cyc_Absyn_Tvar*_tmp532;struct Cyc_Absyn_Vardecl*_tmp533;int _tmp534;
struct Cyc_Absyn_Stmt*_tmp535;struct Cyc_Absyn_Exp*_tmp536;_LL322: if((int)_tmp511
!= 0)goto _LL324;_LL323: return;_LL324: if(_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 
0)goto _LL326;_tmp512=((struct Cyc_Absyn_Exp_s_struct*)_tmp511)->f1;_LL325: Cyc_Toc_exp_to_c(
nv,_tmp512);return;_LL326: if(_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 1)goto
_LL328;_tmp513=((struct Cyc_Absyn_Seq_s_struct*)_tmp511)->f1;_tmp514=((struct Cyc_Absyn_Seq_s_struct*)
_tmp511)->f2;_LL327: Cyc_Toc_stmt_to_c(nv,_tmp513);s=_tmp514;continue;_LL328: if(
_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 2)goto _LL32A;_tmp515=((struct Cyc_Absyn_Return_s_struct*)
_tmp511)->f1;_LL329: {struct Cyc_Core_Opt*topt=0;if(_tmp515 != 0){topt=({struct Cyc_Core_Opt*
_tmp537=_cycalloc(sizeof(*_tmp537));_tmp537->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp515->topt))->v);_tmp537;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp515));}if(s->try_depth > 0){if(topt != 0){
struct _tuple1*_tmp538=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp539=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp538,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp538,(void*)topt->v,_tmp515,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp539,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL32A: if(_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 3)goto _LL32C;_tmp516=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp511)->f1;_tmp517=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp511)->f2;_tmp518=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp511)->f3;_LL32B:
Cyc_Toc_exp_to_c(nv,_tmp516);Cyc_Toc_stmt_to_c(nv,_tmp517);s=_tmp518;continue;
_LL32C: if(_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 4)goto _LL32E;_tmp519=((struct
Cyc_Absyn_While_s_struct*)_tmp511)->f1;_tmp51A=_tmp519.f1;_tmp51B=((struct Cyc_Absyn_While_s_struct*)
_tmp511)->f2;_LL32D: Cyc_Toc_exp_to_c(nv,_tmp51A);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
nv),_tmp51B);return;_LL32E: if(_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 5)goto
_LL330;_tmp51C=((struct Cyc_Absyn_Break_s_struct*)_tmp511)->f1;_LL32F: if(nv->break_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->break_lab))->v,0))->r));{int dest_depth=
_tmp51C == 0?0: _tmp51C->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,
s);return;}_LL330: if(_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 6)goto _LL332;
_tmp51D=((struct Cyc_Absyn_Continue_s_struct*)_tmp511)->f1;_LL331: if(nv->continue_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->continue_lab))->v,0))->r));_tmp51E=_tmp51D;
goto _LL333;_LL332: if(_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 7)goto _LL334;
_tmp51E=((struct Cyc_Absyn_Goto_s_struct*)_tmp511)->f2;_LL333: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp51E))->try_depth,s);
return;_LL334: if(_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 8)goto _LL336;_tmp51F=((
struct Cyc_Absyn_For_s_struct*)_tmp511)->f1;_tmp520=((struct Cyc_Absyn_For_s_struct*)
_tmp511)->f2;_tmp521=_tmp520.f1;_tmp522=((struct Cyc_Absyn_For_s_struct*)_tmp511)->f3;
_tmp523=_tmp522.f1;_tmp524=((struct Cyc_Absyn_For_s_struct*)_tmp511)->f4;_LL335:
Cyc_Toc_exp_to_c(nv,_tmp51F);Cyc_Toc_exp_to_c(nv,_tmp521);Cyc_Toc_exp_to_c(nv,
_tmp523);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp524);return;_LL336: if(
_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 9)goto _LL338;_tmp525=((struct Cyc_Absyn_Switch_s_struct*)
_tmp511)->f1;_tmp526=((struct Cyc_Absyn_Switch_s_struct*)_tmp511)->f2;_LL337: Cyc_Toc_xlate_switch(
nv,s,_tmp525,_tmp526);return;_LL338: if(_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 
11)goto _LL33A;_tmp527=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp511)->f1;_tmp528=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp511)->f2;_LL339: if(nv->fallthru_info == 0)({
void*_tmp53A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp53B="fallthru in unexpected place";_tag_arr(_tmp53B,sizeof(char),
_get_zero_arr_size(_tmp53B,29));}),_tag_arr(_tmp53A,sizeof(void*),0));});{struct
_tuple8 _tmp53D;struct _tagged_arr*_tmp53E;struct Cyc_List_List*_tmp53F;struct Cyc_Dict_Dict*
_tmp540;struct _tuple8*_tmp53C=(struct _tuple8*)((struct Cyc_Core_Opt*)_check_null(
nv->fallthru_info))->v;_tmp53D=*_tmp53C;_tmp53E=_tmp53D.f1;_tmp53F=_tmp53D.f2;
_tmp540=_tmp53D.f3;{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp53E,0);Cyc_Toc_do_npop_before(
s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp528)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp541=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp53F);struct Cyc_List_List*_tmp542=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp527);for(0;_tmp541 != 0;(_tmp541=_tmp541->tl,
_tmp542=_tmp542->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp542))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup)(
_tmp540,(struct _tuple1*)_tmp541->hd),(struct Cyc_Absyn_Exp*)_tmp542->hd,0),s2,0);}(
void*)(s->r=(void*)((void*)s2->r));return;}}}_LL33A: if(_tmp511 <= (void*)1?1:*((
int*)_tmp511)!= 12)goto _LL33C;_tmp529=((struct Cyc_Absyn_Decl_s_struct*)_tmp511)->f1;
_tmp52A=((struct Cyc_Absyn_Decl_s_struct*)_tmp511)->f2;_LL33B:{void*_tmp543=(void*)
_tmp529->r;struct Cyc_Absyn_Vardecl*_tmp544;struct Cyc_Absyn_Pat*_tmp545;struct Cyc_Absyn_Exp*
_tmp546;struct Cyc_List_List*_tmp547;_LL34F: if(*((int*)_tmp543)!= 0)goto _LL351;
_tmp544=((struct Cyc_Absyn_Var_d_struct*)_tmp543)->f1;_LL350: {struct Cyc_Toc_Env*
_tmp548=Cyc_Toc_add_varmap(nv,_tmp544->name,Cyc_Absyn_varb_exp(_tmp544->name,(
void*)({struct Cyc_Absyn_Local_b_struct*_tmp549=_cycalloc(sizeof(*_tmp549));
_tmp549[0]=({struct Cyc_Absyn_Local_b_struct _tmp54A;_tmp54A.tag=3;_tmp54A.f1=
_tmp544;_tmp54A;});_tmp549;}),0));Cyc_Toc_local_decl_to_c(_tmp548,_tmp548,
_tmp544,_tmp52A);goto _LL34E;}_LL351: if(*((int*)_tmp543)!= 2)goto _LL353;_tmp545=((
struct Cyc_Absyn_Let_d_struct*)_tmp543)->f1;_tmp546=((struct Cyc_Absyn_Let_d_struct*)
_tmp543)->f3;_LL352:{void*_tmp54B=(void*)_tmp545->r;struct Cyc_Absyn_Vardecl*
_tmp54C;_LL358: if(_tmp54B <= (void*)2?1:*((int*)_tmp54B)!= 0)goto _LL35A;_tmp54C=((
struct Cyc_Absyn_Var_p_struct*)_tmp54B)->f1;_LL359: {struct _tuple1*old_name=
_tmp54C->name;struct _tuple1*new_name=Cyc_Toc_temp_var();_tmp54C->name=new_name;
_tmp54C->initializer=(struct Cyc_Absyn_Exp*)_tmp546;(void*)(_tmp529->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D[
0]=({struct Cyc_Absyn_Var_d_struct _tmp54E;_tmp54E.tag=0;_tmp54E.f1=_tmp54C;
_tmp54E;});_tmp54D;})));{struct Cyc_Toc_Env*_tmp54F=Cyc_Toc_add_varmap(nv,
old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550[0]=({struct Cyc_Absyn_Local_b_struct
_tmp551;_tmp551.tag=3;_tmp551.f1=_tmp54C;_tmp551;});_tmp550;}),0));Cyc_Toc_local_decl_to_c(
_tmp54F,nv,_tmp54C,_tmp52A);goto _LL357;}}_LL35A:;_LL35B:(void*)(s->r=(void*)((
void*)(Cyc_Toc_letdecl_to_c(nv,_tmp545,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp546->topt))->v,_tmp546,_tmp52A))->r));goto _LL357;_LL357:;}goto _LL34E;_LL353:
if(*((int*)_tmp543)!= 3)goto _LL355;_tmp547=((struct Cyc_Absyn_Letv_d_struct*)
_tmp543)->f1;_LL354: {struct Cyc_List_List*_tmp552=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp547);if(_tmp552 == 0)({void*_tmp553[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp554="empty Letv_d";_tag_arr(_tmp554,sizeof(char),_get_zero_arr_size(_tmp554,
13));}),_tag_arr(_tmp553,sizeof(void*),0));});(void*)(_tmp529->r=(void*)((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555[0]=({
struct Cyc_Absyn_Var_d_struct _tmp556;_tmp556.tag=0;_tmp556.f1=(struct Cyc_Absyn_Vardecl*)
_tmp552->hd;_tmp556;});_tmp555;})));_tmp552=_tmp552->tl;for(0;_tmp552 != 0;
_tmp552=_tmp552->tl){struct Cyc_Absyn_Decl*_tmp557=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558[0]=({
struct Cyc_Absyn_Var_d_struct _tmp559;_tmp559.tag=0;_tmp559.f1=(struct Cyc_Absyn_Vardecl*)
_tmp552->hd;_tmp559;});_tmp558;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp557,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto
_LL34E;}_LL355:;_LL356:({void*_tmp55A[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp55B="bad nested declaration within function";
_tag_arr(_tmp55B,sizeof(char),_get_zero_arr_size(_tmp55B,39));}),_tag_arr(
_tmp55A,sizeof(void*),0));});_LL34E:;}return;_LL33C: if(_tmp511 <= (void*)1?1:*((
int*)_tmp511)!= 15)goto _LL33E;_tmp52B=((struct Cyc_Absyn_Label_s_struct*)_tmp511)->f1;
_tmp52C=((struct Cyc_Absyn_Label_s_struct*)_tmp511)->f2;_LL33D: s=_tmp52C;
continue;_LL33E: if(_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 16)goto _LL340;_tmp52D=((
struct Cyc_Absyn_Do_s_struct*)_tmp511)->f1;_tmp52E=((struct Cyc_Absyn_Do_s_struct*)
_tmp511)->f2;_tmp52F=_tmp52E.f1;_LL33F: Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),
_tmp52D);Cyc_Toc_exp_to_c(nv,_tmp52F);return;_LL340: if(_tmp511 <= (void*)1?1:*((
int*)_tmp511)!= 17)goto _LL342;_tmp530=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp511)->f1;_tmp531=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp511)->f2;_LL341: {
struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=({struct Cyc_Core_Opt*
_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->v=(void*)e_typ;_tmp55C;});nv=Cyc_Toc_add_varmap(
nv,e_var,e_exp);Cyc_Toc_stmt_to_c(nv,_tmp530);{struct Cyc_Absyn_Stmt*_tmp55D=Cyc_Absyn_seq_stmt(
_tmp530,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
struct _tuple1*_tmp55E=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp55F=Cyc_Absyn_var_exp(
_tmp55E,0);struct Cyc_Absyn_Vardecl*_tmp560=Cyc_Absyn_new_vardecl(_tmp55E,Cyc_Absyn_exn_typ,
0);_tmp55F->topt=({struct Cyc_Core_Opt*_tmp561=_cycalloc(sizeof(*_tmp561));
_tmp561->v=(void*)Cyc_Absyn_exn_typ;_tmp561;});{struct Cyc_Absyn_Pat*_tmp562=({
struct Cyc_Absyn_Pat*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572->r=(void*)((void*)({
struct Cyc_Absyn_Var_p_struct*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574[0]=({
struct Cyc_Absyn_Var_p_struct _tmp575;_tmp575.tag=0;_tmp575.f1=_tmp560;_tmp575;});
_tmp574;}));_tmp572->topt=({struct Cyc_Core_Opt*_tmp573=_cycalloc(sizeof(*_tmp573));
_tmp573->v=(void*)Cyc_Absyn_exn_typ;_tmp573;});_tmp572->loc=0;_tmp572;});struct
Cyc_Absyn_Exp*_tmp563=Cyc_Absyn_throw_exp(_tmp55F,0);_tmp563->topt=({struct Cyc_Core_Opt*
_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564->v=(void*)((void*)0);_tmp564;});{
struct Cyc_Absyn_Stmt*_tmp565=Cyc_Absyn_exp_stmt(_tmp563,0);struct Cyc_Absyn_Switch_clause*
_tmp566=({struct Cyc_Absyn_Switch_clause*_tmp56F=_cycalloc(sizeof(*_tmp56F));
_tmp56F->pattern=_tmp562;_tmp56F->pat_vars=({struct Cyc_Core_Opt*_tmp570=
_cycalloc(sizeof(*_tmp570));_tmp570->v=({struct Cyc_List_List*_tmp571=_cycalloc(
sizeof(*_tmp571));_tmp571->hd=_tmp560;_tmp571->tl=0;_tmp571;});_tmp570;});
_tmp56F->where_clause=0;_tmp56F->body=_tmp565;_tmp56F->loc=0;_tmp56F;});struct
Cyc_Absyn_Stmt*_tmp567=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp531,({struct
Cyc_List_List*_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E->hd=_tmp566;_tmp56E->tl=
0;_tmp56E;})),0);Cyc_Toc_stmt_to_c(nv,_tmp567);{struct Cyc_Absyn_Exp*_tmp568=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp56D=_cycalloc(sizeof(*_tmp56D));
_tmp56D->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp56D->tl=0;
_tmp56D;}),0);struct Cyc_Absyn_Stmt*_tmp569=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp56C=_cycalloc(sizeof(*_tmp56C));
_tmp56C->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp56C->tl=0;_tmp56C;}),0),0);struct
Cyc_Absyn_Exp*_tmp56A=Cyc_Absyn_int_exp((void*)0,0,0);struct Cyc_Absyn_Exp*
_tmp56B=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp569,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp56A,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp568,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp56B,0),Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),
_tmp55D,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_cast_exp(
e_typ,Cyc_Toc__exn_thrown_e,0),_tmp567,0),0),0),0),0),0))->r));return;}}}}}
_LL342: if(_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 18)goto _LL344;_tmp532=((struct
Cyc_Absyn_Region_s_struct*)_tmp511)->f1;_tmp533=((struct Cyc_Absyn_Region_s_struct*)
_tmp511)->f2;_tmp534=((struct Cyc_Absyn_Region_s_struct*)_tmp511)->f3;_tmp535=((
struct Cyc_Absyn_Region_s_struct*)_tmp511)->f4;_LL343: {void*rh_struct_typ=Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp);void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(
rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*rh_var=Cyc_Toc_temp_var();struct
_tuple1*x_var=_tmp533->name;struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,
0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
nv,x_var,x_exp),_tmp535);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp576->tl=0;_tmp576;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp577[1];_tmp577[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp577,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),Cyc_Absyn_seq_stmt(_tmp535,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({
struct Cyc_Absyn_Exp*_tmp578[1];_tmp578[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp578,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),0),0),0),0))->r));return;}_LL344: if(_tmp511 <= (void*)1?1:*((int*)_tmp511)
!= 20)goto _LL346;_tmp536=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp511)->f1;
_LL345: Cyc_Toc_exp_to_c(nv,_tmp536);(void*)(s->r=(void*)((void*)(Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp579=
_cycalloc(sizeof(*_tmp579));_tmp579->hd=_tmp536;_tmp579->tl=0;_tmp579;}),0),0))->r));
return;_LL346: if(_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 10)goto _LL348;_LL347:({
void*_tmp57A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp57B="switchC_s";_tag_arr(_tmp57B,sizeof(char),_get_zero_arr_size(
_tmp57B,10));}),_tag_arr(_tmp57A,sizeof(void*),0));});_LL348: if(_tmp511 <= (void*)
1?1:*((int*)_tmp511)!= 13)goto _LL34A;_LL349:({void*_tmp57C[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp57D="cut";
_tag_arr(_tmp57D,sizeof(char),_get_zero_arr_size(_tmp57D,4));}),_tag_arr(_tmp57C,
sizeof(void*),0));});_LL34A: if(_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 14)goto
_LL34C;_LL34B:({void*_tmp57E[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp57F="splice";_tag_arr(_tmp57F,
sizeof(char),_get_zero_arr_size(_tmp57F,7));}),_tag_arr(_tmp57E,sizeof(void*),0));});
_LL34C: if(_tmp511 <= (void*)1?1:*((int*)_tmp511)!= 19)goto _LL321;_LL34D:({void*
_tmp580[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp581="forarray";_tag_arr(_tmp581,sizeof(char),_get_zero_arr_size(
_tmp581,9));}),_tag_arr(_tmp580,sizeof(void*),0));});_LL321:;}}struct _tuple16{
struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f){f->tvs=0;f->effect=0;f->rgn_po=0;(
void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((void*)f->ret_type));{struct Cyc_List_List*
_tmp582=f->args;for(0;_tmp582 != 0;_tmp582=_tmp582->tl){(*((struct _tuple16*)
_tmp582->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple16*)_tmp582->hd)).f3);{struct
_tuple1*_tmp583=({struct _tuple1*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->f1=(
void*)0;_tmp584->f2=(*((struct _tuple16*)_tmp582->hd)).f1;_tmp584;});nv=Cyc_Toc_add_varmap(
nv,_tmp583,Cyc_Absyn_var_exp(_tmp583,0));}}}if((unsigned int)f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0: 0){struct Cyc_Core_Opt*
_tmp586;struct Cyc_Absyn_Tqual _tmp587;void*_tmp588;int _tmp589;struct Cyc_Absyn_VarargInfo
_tmp585=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp586=
_tmp585.name;_tmp587=_tmp585.tq;_tmp588=(void*)_tmp585.type;_tmp589=_tmp585.inject;{
void*_tmp58A=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ(_tmp588,(void*)2,_tmp587,Cyc_Absyn_false_conref));
struct _tuple1*_tmp58B=({struct _tuple1*_tmp58E=_cycalloc(sizeof(*_tmp58E));
_tmp58E->f1=(void*)0;_tmp58E->f2=(struct _tagged_arr*)((struct Cyc_Core_Opt*)
_check_null(_tmp586))->v;_tmp58E;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp58C=
_cycalloc(sizeof(*_tmp58C));_tmp58C->hd=({struct _tuple16*_tmp58D=_cycalloc(
sizeof(*_tmp58D));_tmp58D->f1=(struct _tagged_arr*)_tmp586->v;_tmp58D->f2=_tmp587;
_tmp58D->f3=_tmp58A;_tmp58D;});_tmp58C->tl=0;_tmp58C;}));nv=Cyc_Toc_add_varmap(
nv,_tmp58B,Cyc_Absyn_var_exp(_tmp58B,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp58F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp58F != 0;_tmp58F=_tmp58F->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp58F->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp58F->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(nv),f->body);}
static void*Cyc_Toc_scope_to_c(void*s){void*_tmp590=s;_LL35D: if((int)_tmp590 != 1)
goto _LL35F;_LL35E: return(void*)2;_LL35F: if((int)_tmp590 != 4)goto _LL361;_LL360:
return(void*)3;_LL361:;_LL362: return s;_LL35C:;}struct Cyc_Core_Opt*Cyc_Toc_aggrs_so_far=
0;static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*
_tmp591=ad->name;if(Cyc_Toc_aggrs_so_far == 0)Cyc_Toc_aggrs_so_far=({struct Cyc_Core_Opt*
_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592->v=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp592;});{
int seen_defn_before;struct Cyc_Core_Opt*_tmp593=((struct Cyc_Core_Opt*(*)(struct
Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp591);if(_tmp593 == 
0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(
Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp591,ad));}else{if(((
struct Cyc_Absyn_Aggrdecl*)_tmp593->v)->impl == 0){(struct Cyc_Dict_Dict*)(((struct
Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,
_tmp591,ad));seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(ad->sc=(void*)((
void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po=
0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*_tmp594=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp594 != 0;_tmp594=_tmp594->tl){(void*)(((
struct Cyc_Absyn_Aggrfield*)_tmp594->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp594->hd)->type));}}}}}static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far=0;static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
tud){if(Cyc_Toc_tunions_so_far == 0)Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*
_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595->v=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);_tmp595;});{
struct _tuple1*_tmp596=tud->name;if(tud->fields == 0?1:((int(*)(struct Cyc_Set_Set*
s,struct _tuple1*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_tunions_so_far))->v,_tmp596))return;(struct Cyc_Set_Set*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(void*)((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_insert)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp596));{struct Cyc_List_List*
_tmp597=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;
for(0;_tmp597 != 0;_tmp597=_tmp597->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp597->hd;if(f->typs != 0){struct Cyc_List_List*_tmp598=0;int i=1;{struct Cyc_List_List*
_tmp599=f->typs;for(0;_tmp599 != 0;(_tmp599=_tmp599->tl,i ++)){struct _tagged_arr*
_tmp59A=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*_tmp59B=({struct Cyc_Absyn_Aggrfield*
_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D->name=_tmp59A;_tmp59D->tq=(*((struct
_tuple4*)_tmp599->hd)).f1;_tmp59D->type=(void*)Cyc_Toc_typ_to_c_array((*((struct
_tuple4*)_tmp599->hd)).f2);_tmp59D->width=0;_tmp59D->attributes=0;_tmp59D;});
_tmp598=({struct Cyc_List_List*_tmp59C=_cycalloc(sizeof(*_tmp59C));_tmp59C->hd=
_tmp59B;_tmp59C->tl=_tmp598;_tmp59C;});}}_tmp598=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp598);_tmp598=({struct Cyc_List_List*
_tmp59E=_cycalloc(sizeof(*_tmp59E));_tmp59E->hd=({struct Cyc_Absyn_Aggrfield*
_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F->name=Cyc_Toc_tag_sp;_tmp59F->tq=Cyc_Toc_mt_tq;
_tmp59F->type=(void*)Cyc_Absyn_sint_typ;_tmp59F->width=0;_tmp59F->attributes=0;
_tmp59F;});_tmp59E->tl=_tmp598;_tmp59E;});{struct Cyc_Absyn_Aggrdecl*_tmp5A0=({
struct Cyc_Absyn_Aggrdecl*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4->kind=(void*)((
void*)0);_tmp5A4->sc=(void*)((void*)2);_tmp5A4->name=Cyc_Toc_collapse_qvar_tag(f->name,({
const char*_tmp5A6="_struct";_tag_arr(_tmp5A6,sizeof(char),_get_zero_arr_size(
_tmp5A6,8));}));_tmp5A4->tvs=0;_tmp5A4->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5->exist_vars=0;_tmp5A5->rgn_po=0;
_tmp5A5->fields=_tmp598;_tmp5A5;});_tmp5A4->attributes=0;_tmp5A4;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));
_tmp5A2[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp5A3;_tmp5A3.tag=4;_tmp5A3.f1=
_tmp5A0;_tmp5A3;});_tmp5A2;}),0);_tmp5A1->tl=Cyc_Toc_result_decls;_tmp5A1;});}}}}}}
static struct Cyc_Core_Opt*Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far == 0)Cyc_Toc_xtunion_fields_so_far=({
struct Cyc_Core_Opt*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp5A7;});if(xd->fields == 0)return;{struct _tuple1*_tmp5A8=xd->name;struct Cyc_List_List*
_tmp5A9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp5A9 != 0;_tmp5A9=_tmp5A9->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp5A9->hd;struct _tagged_arr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp5AA=Cyc_Absyn_uint_exp(
_get_arr_size(*fn,sizeof(char))+ 4,0);void*_tmp5AB=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)_tmp5AA,Cyc_Absyn_false_conref);struct Cyc_Core_Opt*
_tmp5AC=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name);struct Cyc_Core_Opt _tmp5AD;int _tmp5AE;_LL364: if(_tmp5AC != 0)goto _LL366;
_LL365: {struct Cyc_Absyn_Exp*initopt=0;if((void*)f->sc != (void*)3){char zero='\000';
initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((struct _tagged_arr)({struct
Cyc_String_pa_struct _tmp5B5;_tmp5B5.tag=0;_tmp5B5.f1=(struct _tagged_arr)((struct
_tagged_arr)*fn);{struct Cyc_Int_pa_struct _tmp5B4;_tmp5B4.tag=1;_tmp5B4.f1=(
unsigned int)((int)zero);{struct Cyc_Int_pa_struct _tmp5B3;_tmp5B3.tag=1;_tmp5B3.f1=(
unsigned int)((int)zero);{struct Cyc_Int_pa_struct _tmp5B2;_tmp5B2.tag=1;_tmp5B2.f1=(
unsigned int)((int)zero);{struct Cyc_Int_pa_struct _tmp5B1;_tmp5B1.tag=1;_tmp5B1.f1=(
unsigned int)((int)zero);{void*_tmp5AF[5]={& _tmp5B1,& _tmp5B2,& _tmp5B3,& _tmp5B4,&
_tmp5B5};Cyc_aprintf(({const char*_tmp5B0="%c%c%c%c%s";_tag_arr(_tmp5B0,sizeof(
char),_get_zero_arr_size(_tmp5B0,11));}),_tag_arr(_tmp5AF,sizeof(void*),5));}}}}}}),
0);}{struct Cyc_Absyn_Vardecl*_tmp5B6=Cyc_Absyn_new_vardecl(f->name,_tmp5AB,
initopt);(void*)(_tmp5B6->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8[
0]=({struct Cyc_Absyn_Var_d_struct _tmp5B9;_tmp5B9.tag=0;_tmp5B9.f1=_tmp5B6;
_tmp5B9;});_tmp5B8;}),0);_tmp5B7->tl=Cyc_Toc_result_decls;_tmp5B7;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,(void*)f->sc != (void*)3));if(f->typs != 0){struct Cyc_List_List*fields=0;
int i=1;{struct Cyc_List_List*_tmp5BA=f->typs;for(0;_tmp5BA != 0;(_tmp5BA=_tmp5BA->tl,
i ++)){struct _tagged_arr*_tmp5BB=({struct _tagged_arr*_tmp5BF=_cycalloc(sizeof(*
_tmp5BF));_tmp5BF[0]=(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp5C2;_tmp5C2.tag=
1;_tmp5C2.f1=(unsigned int)i;{void*_tmp5C0[1]={& _tmp5C2};Cyc_aprintf(({const char*
_tmp5C1="f%d";_tag_arr(_tmp5C1,sizeof(char),_get_zero_arr_size(_tmp5C1,4));}),
_tag_arr(_tmp5C0,sizeof(void*),1));}});_tmp5BF;});struct Cyc_Absyn_Aggrfield*
_tmp5BC=({struct Cyc_Absyn_Aggrfield*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->name=
_tmp5BB;_tmp5BE->tq=(*((struct _tuple4*)_tmp5BA->hd)).f1;_tmp5BE->type=(void*)Cyc_Toc_typ_to_c_array((*((
struct _tuple4*)_tmp5BA->hd)).f2);_tmp5BE->width=0;_tmp5BE->attributes=0;_tmp5BE;});
fields=({struct Cyc_List_List*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));_tmp5BD->hd=
_tmp5BC;_tmp5BD->tl=fields;_tmp5BD;});}}fields=({struct Cyc_List_List*_tmp5C3=
_cycalloc(sizeof(*_tmp5C3));_tmp5C3->hd=({struct Cyc_Absyn_Aggrfield*_tmp5C4=
_cycalloc(sizeof(*_tmp5C4));_tmp5C4->name=Cyc_Toc_tag_sp;_tmp5C4->tq=Cyc_Toc_mt_tq;
_tmp5C4->type=(void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);
_tmp5C4->width=0;_tmp5C4->attributes=0;_tmp5C4;});_tmp5C3->tl=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp5C3;});{struct Cyc_Absyn_Aggrdecl*
_tmp5C5=({struct Cyc_Absyn_Aggrdecl*_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9->kind=(
void*)((void*)0);_tmp5C9->sc=(void*)((void*)2);_tmp5C9->name=Cyc_Toc_collapse_qvar_tag(
f->name,({const char*_tmp5CB="_struct";_tag_arr(_tmp5CB,sizeof(char),
_get_zero_arr_size(_tmp5CB,8));}));_tmp5C9->tvs=0;_tmp5C9->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp5CA=_cycalloc(sizeof(*_tmp5CA));_tmp5CA->exist_vars=0;_tmp5CA->rgn_po=0;
_tmp5CA->fields=fields;_tmp5CA;});_tmp5C9->attributes=0;_tmp5C9;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));_tmp5C6->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));
_tmp5C7[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp5C8;_tmp5C8.tag=4;_tmp5C8.f1=
_tmp5C5;_tmp5C8;});_tmp5C7;}),0);_tmp5C6->tl=Cyc_Toc_result_decls;_tmp5C6;});}}
goto _LL363;}}_LL366: if(_tmp5AC == 0)goto _LL368;_tmp5AD=*_tmp5AC;_tmp5AE=(int)
_tmp5AD.v;if(_tmp5AE != 0)goto _LL368;_LL367: if((void*)f->sc != (void*)3){char zero='\000';
struct Cyc_Absyn_Exp*_tmp5CC=Cyc_Absyn_string_exp((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp5D7;_tmp5D7.tag=0;_tmp5D7.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
struct Cyc_Int_pa_struct _tmp5D6;_tmp5D6.tag=1;_tmp5D6.f1=(unsigned int)((int)zero);{
struct Cyc_Int_pa_struct _tmp5D5;_tmp5D5.tag=1;_tmp5D5.f1=(unsigned int)((int)zero);{
struct Cyc_Int_pa_struct _tmp5D4;_tmp5D4.tag=1;_tmp5D4.f1=(unsigned int)((int)zero);{
struct Cyc_Int_pa_struct _tmp5D3;_tmp5D3.tag=1;_tmp5D3.f1=(unsigned int)((int)zero);{
void*_tmp5D1[5]={& _tmp5D3,& _tmp5D4,& _tmp5D5,& _tmp5D6,& _tmp5D7};Cyc_aprintf(({
const char*_tmp5D2="%c%c%c%c%s";_tag_arr(_tmp5D2,sizeof(char),_get_zero_arr_size(
_tmp5D2,11));}),_tag_arr(_tmp5D1,sizeof(void*),5));}}}}}}),0);struct Cyc_Absyn_Vardecl*
_tmp5CD=Cyc_Absyn_new_vardecl(f->name,_tmp5AB,(struct Cyc_Absyn_Exp*)_tmp5CC);(
void*)(_tmp5CD->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF[0]=({
struct Cyc_Absyn_Var_d_struct _tmp5D0;_tmp5D0.tag=0;_tmp5D0.f1=_tmp5CD;_tmp5D0;});
_tmp5CF;}),0);_tmp5CE->tl=Cyc_Toc_result_decls;_tmp5CE;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,1));}goto _LL363;_LL368:;_LL369: goto _LL363;_LL363:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));if((void*)vd->sc == (void*)5?Cyc_Tcutil_is_tagged_pointer_typ(old_typ):
0)(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp5D8=(void*)init->r;struct Cyc_Absyn_Vardecl*_tmp5D9;struct Cyc_Absyn_Exp*
_tmp5DA;struct Cyc_Absyn_Exp*_tmp5DB;int _tmp5DC;_LL36B: if(*((int*)_tmp5D8)!= 27)
goto _LL36D;_tmp5D9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp5D8)->f1;
_tmp5DA=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp5D8)->f2;_tmp5DB=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp5D8)->f3;_tmp5DC=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp5D8)->f4;_LL36C: vd->initializer=0;(void*)(s->r=(void*)((void*)(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp5D9,_tmp5DA,_tmp5DB,_tmp5DC,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));goto _LL36A;_LL36D:;_LL36E: if((void*)vd->sc == (void*)0){
struct Cyc_Toc_Env*_tmp5DD=({struct Cyc_Toc_Env*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));
_tmp5DE[0]=*init_nv;_tmp5DE;});_tmp5DD->toplevel=1;Cyc_Toc_exp_to_c(_tmp5DD,init);}
else{Cyc_Toc_exp_to_c(init_nv,init);}goto _LL36A;_LL36A:;}else{void*_tmp5DF=Cyc_Tcutil_compress(
old_typ);struct Cyc_Absyn_ArrayInfo _tmp5E0;void*_tmp5E1;struct Cyc_Absyn_Exp*
_tmp5E2;struct Cyc_Absyn_Conref*_tmp5E3;_LL370: if(_tmp5DF <= (void*)3?1:*((int*)
_tmp5DF)!= 7)goto _LL372;_tmp5E0=((struct Cyc_Absyn_ArrayType_struct*)_tmp5DF)->f1;
_tmp5E1=(void*)_tmp5E0.elt_type;_tmp5E2=_tmp5E0.num_elts;_tmp5E3=_tmp5E0.zero_term;
if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp5E3))goto
_LL372;_LL371: if(_tmp5E2 == 0)({void*_tmp5E4[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp5E5="can't initialize zero-terminated array -- size unknown";
_tag_arr(_tmp5E5,sizeof(char),_get_zero_arr_size(_tmp5E5,55));}),_tag_arr(
_tmp5E4,sizeof(void*),0));});{struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)
_check_null(_tmp5E2);struct Cyc_Absyn_Exp*_tmp5E6=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(
vd->name,0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
struct Cyc_Absyn_Exp*_tmp5E7=Cyc_Absyn_signed_int_exp(0,0);(void*)(s->r=(void*)((
void*)(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp5E6,
_tmp5E7,0),0),Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));goto _LL36F;}_LL372:;
_LL373: goto _LL36F;_LL36F:;}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=
0;static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp5E8=_cycalloc(
sizeof(*_tmp5E8));_tmp5E8[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp5E8;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp5E9=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp5E9;_push_region(prgn);{struct Cyc_Toc_Env*_tmp5EB;
struct Cyc_List_List*_tmp5EC;struct Cyc_Absyn_Stmt*_tmp5ED;struct _tuple12 _tmp5EA=
Cyc_Toc_xlate_pat(nv,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp5EB=_tmp5EA.f1;_tmp5EC=_tmp5EA.f2;_tmp5ED=_tmp5EA.f3;Cyc_Toc_stmt_to_c(
_tmp5EB,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp5ED,s,0),0);for(0;_tmp5EC != 0;_tmp5EC=_tmp5EC->tl){
struct _tuple13 _tmp5EF;struct _tuple1*_tmp5F0;void*_tmp5F1;struct _tuple13*_tmp5EE=(
struct _tuple13*)_tmp5EC->hd;_tmp5EF=*_tmp5EE;_tmp5F0=_tmp5EF.f1;_tmp5F1=_tmp5EF.f2;
s=Cyc_Absyn_declare_stmt(_tmp5F0,_tmp5F1,0,s,0);}};_pop_region(prgn);}return s;}}
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top){for(0;ds != 0;ds=ds->tl){if(!nv->toplevel)({void*_tmp5F2[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp5F3="decls_to_c: not at toplevel!";
_tag_arr(_tmp5F3,sizeof(char),_get_zero_arr_size(_tmp5F3,29));}),_tag_arr(
_tmp5F2,sizeof(void*),0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp5F4=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp5F5;struct Cyc_Absyn_Fndecl*
_tmp5F6;struct Cyc_Absyn_Aggrdecl*_tmp5F7;struct Cyc_Absyn_Tuniondecl*_tmp5F8;
struct Cyc_Absyn_Enumdecl*_tmp5F9;struct Cyc_Absyn_Typedefdecl*_tmp5FA;struct Cyc_List_List*
_tmp5FB;struct Cyc_List_List*_tmp5FC;struct Cyc_List_List*_tmp5FD;_LL375: if(*((int*)
_tmp5F4)!= 0)goto _LL377;_tmp5F5=((struct Cyc_Absyn_Var_d_struct*)_tmp5F4)->f1;
_LL376: {struct _tuple1*_tmp5FE=_tmp5F5->name;if((void*)_tmp5F5->sc == (void*)4)
_tmp5FE=({struct _tuple1*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF->f1=Cyc_Absyn_rel_ns_null;
_tmp5FF->f2=(*_tmp5FE).f2;_tmp5FF;});if(_tmp5F5->initializer != 0)Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp5F5->initializer));nv=Cyc_Toc_add_varmap(
nv,_tmp5F5->name,Cyc_Absyn_varb_exp(_tmp5FE,(void*)({struct Cyc_Absyn_Global_b_struct*
_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600[0]=({struct Cyc_Absyn_Global_b_struct
_tmp601;_tmp601.tag=0;_tmp601.f1=_tmp5F5;_tmp601;});_tmp600;}),0));_tmp5F5->name=
_tmp5FE;(void*)(_tmp5F5->sc=(void*)Cyc_Toc_scope_to_c((void*)_tmp5F5->sc));(void*)(
_tmp5F5->type=(void*)Cyc_Toc_typ_to_c_array((void*)_tmp5F5->type));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602->hd=d;_tmp602->tl=
Cyc_Toc_result_decls;_tmp602;});goto _LL374;}_LL377: if(*((int*)_tmp5F4)!= 1)goto
_LL379;_tmp5F6=((struct Cyc_Absyn_Fn_d_struct*)_tmp5F4)->f1;_LL378: {struct
_tuple1*_tmp603=_tmp5F6->name;if((void*)_tmp5F6->sc == (void*)4)_tmp603=({struct
_tuple1*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604->f1=Cyc_Absyn_rel_ns_null;
_tmp604->f2=(*_tmp603).f2;_tmp604;});nv=Cyc_Toc_add_varmap(nv,_tmp5F6->name,Cyc_Absyn_var_exp(
_tmp603,0));_tmp5F6->name=_tmp603;Cyc_Toc_fndecl_to_c(nv,_tmp5F6);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605->hd=d;_tmp605->tl=
Cyc_Toc_result_decls;_tmp605;});goto _LL374;}_LL379: if(*((int*)_tmp5F4)!= 2)goto
_LL37B;_LL37A: goto _LL37C;_LL37B: if(*((int*)_tmp5F4)!= 3)goto _LL37D;_LL37C:({void*
_tmp606[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp607="letdecl at toplevel";_tag_arr(_tmp607,sizeof(char),
_get_zero_arr_size(_tmp607,20));}),_tag_arr(_tmp606,sizeof(void*),0));});_LL37D:
if(*((int*)_tmp5F4)!= 4)goto _LL37F;_tmp5F7=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp5F4)->f1;_LL37E: Cyc_Toc_aggrdecl_to_c(_tmp5F7);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608->hd=d;_tmp608->tl=Cyc_Toc_result_decls;
_tmp608;});goto _LL374;_LL37F: if(*((int*)_tmp5F4)!= 5)goto _LL381;_tmp5F8=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp5F4)->f1;_LL380: if(_tmp5F8->is_xtunion)Cyc_Toc_xtuniondecl_to_c(
_tmp5F8);else{Cyc_Toc_tuniondecl_to_c(_tmp5F8);}goto _LL374;_LL381: if(*((int*)
_tmp5F4)!= 6)goto _LL383;_tmp5F9=((struct Cyc_Absyn_Enum_d_struct*)_tmp5F4)->f1;
_LL382: Cyc_Toc_enumdecl_to_c(nv,_tmp5F9);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609->hd=d;_tmp609->tl=Cyc_Toc_result_decls;
_tmp609;});goto _LL374;_LL383: if(*((int*)_tmp5F4)!= 7)goto _LL385;_tmp5FA=((struct
Cyc_Absyn_Typedef_d_struct*)_tmp5F4)->f1;_LL384: _tmp5FA->name=_tmp5FA->name;
_tmp5FA->tvs=0;if(_tmp5FA->defn != 0)_tmp5FA->defn=({struct Cyc_Core_Opt*_tmp60A=
_cycalloc(sizeof(*_tmp60A));_tmp60A->v=(void*)Cyc_Toc_typ_to_c_array((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp5FA->defn))->v);_tmp60A;});else{void*_tmp60B=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp5FA->kind))->v;_LL38C: if((int)
_tmp60B != 2)goto _LL38E;_LL38D: _tmp5FA->defn=({struct Cyc_Core_Opt*_tmp60C=
_cycalloc(sizeof(*_tmp60C));_tmp60C->v=(void*)Cyc_Absyn_void_star_typ();_tmp60C;});
goto _LL38B;_LL38E:;_LL38F: _tmp5FA->defn=({struct Cyc_Core_Opt*_tmp60D=_cycalloc(
sizeof(*_tmp60D));_tmp60D->v=(void*)((void*)0);_tmp60D;});goto _LL38B;_LL38B:;}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp60E=_cycalloc(sizeof(*_tmp60E));
_tmp60E->hd=d;_tmp60E->tl=Cyc_Toc_result_decls;_tmp60E;});goto _LL374;_LL385: if(*((
int*)_tmp5F4)!= 8)goto _LL387;_tmp5FB=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp5F4)->f2;_LL386: _tmp5FC=_tmp5FB;goto _LL388;_LL387: if(*((int*)_tmp5F4)!= 9)
goto _LL389;_tmp5FC=((struct Cyc_Absyn_Using_d_struct*)_tmp5F4)->f2;_LL388: _tmp5FD=
_tmp5FC;goto _LL38A;_LL389: if(*((int*)_tmp5F4)!= 10)goto _LL374;_tmp5FD=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp5F4)->f1;_LL38A: nv=Cyc_Toc_decls_to_c(nv,_tmp5FD,
top);goto _LL374;_LL374:;}}return nv;}static void Cyc_Toc_init(){Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_types=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;Cyc_Toc_aggrs_so_far=0;Cyc_Toc_tunions_so_far=0;
Cyc_Toc_xtunion_fields_so_far=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(),ds,1);return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
