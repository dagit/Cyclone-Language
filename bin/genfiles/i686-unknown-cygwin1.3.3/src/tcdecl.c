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
  if (!_cks_index >= _cks_bound) _throw_arraybounds(); \
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
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*
Cyc_Std_stderr;struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_Buffer_pa_struct{int tag;struct _tagged_arr f1;};
struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(
struct _tagged_arr,struct _tagged_arr);int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,
struct _tagged_arr,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct
Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;}
;struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_Std_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct _tagged_arr Cyc_Std_vrprintf(struct _RegionHandle*,struct _tagged_arr,struct
_tagged_arr);extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _tagged_arr);extern char Cyc_Position_Nocontext[14];void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple0{
void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};
struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*
name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_PtrInfo{void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*
nullable;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
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
struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeInt_struct{int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{int tag;
void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_NonNullable_ps_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_Nullable_ps_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_TaggedArray_ps_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Regparm_att_struct{int tag;int
f1;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Carray_mod_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;void*f1;void*f2;struct Cyc_Absyn_Tqual f3;};struct Cyc_Absyn_Function_mod_struct{
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
tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*f1;void*f2;};
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
Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Address_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*f1;void*f2;
};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
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
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;
int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct
Cyc_Absyn_AggrInfo f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Tunion_p_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*
f3;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Pat{void*r;
struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*cnst_exp;struct Cyc_Absyn_Stmt*body;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
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
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _tagged_arr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{
void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(struct
_tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();void*Cyc_Absyn_force_kb(
void*kb);struct _tagged_arr Cyc_Absyn_attribute2string(void*);extern char Cyc_Tcdecl_Incompatible[
17];struct Cyc_Tcdecl_Xtunionfielddecl{struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*
field;};void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*loc,struct _tagged_arr*
msg1,struct _tagged_arr fmt,struct _tagged_arr ap);struct _tuple3{void*f1;int f2;};
struct _tuple3 Cyc_Tcdecl_merge_scope(void*s0,void*s1,struct _tagged_arr t,struct
_tagged_arr v,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Absyn_Aggrdecl*
Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Absyn_Tuniondecl*
Cyc_Tcdecl_merge_tuniondecl(struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Absyn_Enumdecl*
Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Absyn_Typedefdecl*
Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);void*Cyc_Tcdecl_merge_binding(
void*d0,void*d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Tcdecl_Xtunionfielddecl*
Cyc_Tcdecl_merge_xtunionfielddecl(struct Cyc_Tcdecl_Xtunionfielddecl*d0,struct Cyc_Tcdecl_Xtunionfielddecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_List_List*Cyc_Tcdecl_sort_xtunion_fields(
struct Cyc_List_List*f,int*res,struct _tagged_arr*v,struct Cyc_Position_Segment*loc,
struct _tagged_arr*msg);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_Dict;extern
char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct _tuple4{void*f1;
void*f2;};struct _tuple4*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d);struct _tuple4*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*s2);struct
_tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
ae;struct Cyc_Core_Opt*le;};void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(
void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_same_atts(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct _tagged_arr Cyc_Absynpp_kind2string(void*);struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct _tagged_arr Cyc_Absynpp_scope2string(void*sc);char Cyc_Tcdecl_Incompatible[
17]="\000\000\000\000Incompatible\000";void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg1,struct _tagged_arr fmt,struct _tagged_arr ap){if(msg1 == 0)(
int)_throw((void*)Cyc_Tcdecl_Incompatible);{struct _tagged_arr fmt2=(struct
_tagged_arr)Cyc_Std_strconcat(({const char*_tmp6="%s ";_tag_arr(_tmp6,sizeof(char),
_get_zero_arr_size(_tmp6,4));}),(struct _tagged_arr)fmt);struct _tagged_arr ap2=({
unsigned int _tmp0=_get_arr_size(ap,sizeof(void*))+ 1;void**_tmp1=(void**)
_cycalloc(_check_times(sizeof(void*),_tmp0));struct _tagged_arr _tmp5=_tag_arr(
_tmp1,sizeof(void*),_tmp0);{unsigned int _tmp2=_tmp0;unsigned int i;for(i=0;i < 
_tmp2;i ++){_tmp1[i]=i == 0?(void*)({struct Cyc_Std_String_pa_struct*_tmp3=
_cycalloc(sizeof(*_tmp3));_tmp3[0]=({struct Cyc_Std_String_pa_struct _tmp4;_tmp4.tag=
0;_tmp4.f1=(struct _tagged_arr)*((struct _tagged_arr*)msg1);_tmp4;});_tmp3;}):*((
void**)_check_unknown_subscript(ap,sizeof(void*),(int)(i - 1)));}}_tmp5;});Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,
fmt2,ap2)));}}static void Cyc_Tcdecl_merge_scope_err(void*s0,void*s1,struct
_tagged_arr t,struct _tagged_arr v,struct Cyc_Position_Segment*loc,struct _tagged_arr*
msg){({struct Cyc_Std_String_pa_struct _tmpC;_tmpC.tag=0;_tmpC.f1=(struct
_tagged_arr)Cyc_Absynpp_scope2string(s0);{struct Cyc_Std_String_pa_struct _tmpB;
_tmpB.tag=0;_tmpB.f1=(struct _tagged_arr)Cyc_Absynpp_scope2string(s1);{struct Cyc_Std_String_pa_struct
_tmpA;_tmpA.tag=0;_tmpA.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmp9;_tmp9.tag=0;_tmp9.f1=(struct _tagged_arr)t;{void*_tmp7[4]={& _tmp9,& _tmpA,&
_tmpB,& _tmpC};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp8="%s %s is %s whereas expected scope is %s";
_tag_arr(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,41));}),_tag_arr(_tmp7,
sizeof(void*),4));}}}}});}struct _tuple3 Cyc_Tcdecl_merge_scope(void*s0,void*s1,
struct _tagged_arr t,struct _tagged_arr v,struct Cyc_Position_Segment*loc,struct
_tagged_arr*msg){{struct _tuple4 _tmpE=({struct _tuple4 _tmpD;_tmpD.f1=s0;_tmpD.f2=
s1;_tmpD;});void*_tmpF;void*_tmp10;void*_tmp11;void*_tmp12;void*_tmp13;void*
_tmp14;void*_tmp15;void*_tmp16;void*_tmp17;void*_tmp18;void*_tmp19;void*_tmp1A;
void*_tmp1B;void*_tmp1C;_LL1: _tmpF=_tmpE.f1;if((int)_tmpF != 4)goto _LL3;_tmp10=
_tmpE.f2;if((int)_tmp10 != 4)goto _LL3;_LL2: goto _LL0;_LL3: _tmp11=_tmpE.f1;if((int)
_tmp11 != 4)goto _LL5;_LL4: goto _LL6;_LL5: _tmp12=_tmpE.f2;if((int)_tmp12 != 4)goto
_LL7;_LL6: Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple3
_tmp1D;_tmp1D.f1=s1;_tmp1D.f2=0;_tmp1D;});_LL7: _tmp13=_tmpE.f2;if((int)_tmp13 != 
3)goto _LL9;_LL8: s1=s0;goto _LL0;_LL9: _tmp14=_tmpE.f1;if((int)_tmp14 != 3)goto _LLB;
_LLA: goto _LL0;_LLB: _tmp15=_tmpE.f1;if((int)_tmp15 != 0)goto _LLD;_tmp16=_tmpE.f2;
if((int)_tmp16 != 0)goto _LLD;_LLC: goto _LLE;_LLD: _tmp17=_tmpE.f1;if((int)_tmp17 != 
2)goto _LLF;_tmp18=_tmpE.f2;if((int)_tmp18 != 2)goto _LLF;_LLE: goto _LL10;_LLF:
_tmp19=_tmpE.f1;if((int)_tmp19 != 1)goto _LL11;_tmp1A=_tmpE.f2;if((int)_tmp1A != 1)
goto _LL11;_LL10: goto _LL0;_LL11: _tmp1B=_tmpE.f1;if((int)_tmp1B != 5)goto _LL13;
_tmp1C=_tmpE.f2;if((int)_tmp1C != 5)goto _LL13;_LL12: goto _LL0;_LL13:;_LL14: Cyc_Tcdecl_merge_scope_err(
s0,s1,t,v,loc,msg);return({struct _tuple3 _tmp1E;_tmp1E.f1=s1;_tmp1E.f2=0;_tmp1E;});
_LL0:;}return({struct _tuple3 _tmp1F;_tmp1F.f1=s1;_tmp1F.f2=1;_tmp1F;});}static int
Cyc_Tcdecl_check_type(void*t0,void*t1){return Cyc_Tcutil_unify(t0,t1);}static
unsigned int Cyc_Tcdecl_get_uint_const_value(struct Cyc_Absyn_Exp*e){void*_tmp20=(
void*)e->r;void*_tmp21;int _tmp22;_LL16: if(*((int*)_tmp20)!= 0)goto _LL18;_tmp21=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp20)->f1;if(_tmp21 <= (void*)1?1:*((
int*)_tmp21)!= 2)goto _LL18;_tmp22=((struct Cyc_Absyn_Int_c_struct*)_tmp21)->f2;
_LL17: return(unsigned int)_tmp22;_LL18:;_LL19:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp24;_tmp24.tag=Cyc_Core_Invalid_argument;_tmp24.f1=({const char*_tmp25="Tcdecl::get_uint_const_value";
_tag_arr(_tmp25,sizeof(char),_get_zero_arr_size(_tmp25,29));});_tmp24;});_tmp23;}));
_LL15:;}inline static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*
tvs1,struct _tagged_arr t,struct _tagged_arr v,struct Cyc_Position_Segment*loc,struct
_tagged_arr*msg){if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs0)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1)){({struct Cyc_Std_String_pa_struct
_tmp29;_tmp29.tag=0;_tmp29.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmp28;_tmp28.tag=0;_tmp28.f1=(struct _tagged_arr)t;{void*_tmp26[2]={& _tmp28,&
_tmp29};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp27="%s %s has a different number of type parameters";
_tag_arr(_tmp27,sizeof(char),_get_zero_arr_size(_tmp27,48));}),_tag_arr(_tmp26,
sizeof(void*),2));}}});return 0;}{struct Cyc_List_List*_tmp2A=tvs0;struct Cyc_List_List*
_tmp2B=tvs1;for(0;_tmp2A != 0;(_tmp2A=_tmp2A->tl,_tmp2B=_tmp2B->tl)){void*_tmp2C=
Cyc_Absyn_force_kb((void*)((struct Cyc_Absyn_Tvar*)_tmp2A->hd)->kind);void*_tmp2D=
Cyc_Absyn_force_kb((void*)((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(_tmp2B))->hd)->kind);if(_tmp2C == _tmp2D)continue;({struct Cyc_Std_String_pa_struct
_tmp34;_tmp34.tag=0;_tmp34.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp2D);{
struct Cyc_Std_String_pa_struct _tmp33;_tmp33.tag=0;_tmp33.f1=(struct _tagged_arr)*((
struct _tagged_arr*)((struct Cyc_Absyn_Tvar*)_tmp2A->hd)->name);{struct Cyc_Std_String_pa_struct
_tmp32;_tmp32.tag=0;_tmp32.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp2C);{
struct Cyc_Std_String_pa_struct _tmp31;_tmp31.tag=0;_tmp31.f1=(struct _tagged_arr)v;{
struct Cyc_Std_String_pa_struct _tmp30;_tmp30.tag=0;_tmp30.f1=(struct _tagged_arr)t;{
void*_tmp2E[5]={& _tmp30,& _tmp31,& _tmp32,& _tmp33,& _tmp34};Cyc_Tcdecl_merr(loc,msg,({
const char*_tmp2F="%s %s has a different kind (%s) for type parameter %s (%s)";
_tag_arr(_tmp2F,sizeof(char),_get_zero_arr_size(_tmp2F,59));}),_tag_arr(_tmp2E,
sizeof(void*),5));}}}}}});return 0;}return 1;}}static int Cyc_Tcdecl_check_atts(
struct Cyc_List_List*atts0,struct Cyc_List_List*atts1,struct _tagged_arr t,struct
_tagged_arr v,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){if(!Cyc_Tcutil_same_atts(
atts0,atts1)){({struct Cyc_Std_String_pa_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(
struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct _tmp37;_tmp37.tag=0;_tmp37.f1=(
struct _tagged_arr)t;{void*_tmp35[2]={& _tmp37,& _tmp38};Cyc_Tcdecl_merr(loc,msg,({
const char*_tmp36="%s %s has different attributes";_tag_arr(_tmp36,sizeof(char),
_get_zero_arr_size(_tmp36,31));}),_tag_arr(_tmp35,sizeof(void*),2));}}});return 0;}
return 1;}struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};static struct Cyc_List_List*
Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){
struct Cyc_List_List*inst=0;for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){inst=({
struct Cyc_List_List*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->hd=({struct _tuple5*
_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(tvs1))->hd;_tmp3A->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp3B=
_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({struct Cyc_Absyn_VarType_struct _tmp3C;
_tmp3C.tag=1;_tmp3C.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd;_tmp3C;});_tmp3B;});
_tmp3A;});_tmp39->tl=inst;_tmp39;});}return inst;}struct _tuple6{struct Cyc_Absyn_AggrdeclImpl*
f1;struct Cyc_Absyn_AggrdeclImpl*f2;};struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(
struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg){struct _tagged_arr _tmp3D=Cyc_Absynpp_qvar2string(d0->name);
int _tmp3E=1;if(!((void*)d0->kind == (void*)d1->kind))return 0;if(!Cyc_Tcdecl_check_tvs(
d0->tvs,d1->tvs,({const char*_tmp3F="type";_tag_arr(_tmp3F,sizeof(char),
_get_zero_arr_size(_tmp3F,5));}),_tmp3D,loc,msg))return 0;{void*_tmp42;int _tmp43;
struct _tuple3 _tmp41=Cyc_Tcdecl_merge_scope((void*)d0->sc,(void*)d1->sc,({const
char*_tmp40="type";_tag_arr(_tmp40,sizeof(char),_get_zero_arr_size(_tmp40,5));}),
_tmp3D,loc,msg);_tmp42=_tmp41.f1;_tmp43=_tmp41.f2;if(!_tmp43)_tmp3E=0;if(!Cyc_Tcdecl_check_atts(
d0->attributes,d1->attributes,({const char*_tmp44="type";_tag_arr(_tmp44,sizeof(
char),_get_zero_arr_size(_tmp44,5));}),_tmp3D,loc,msg))_tmp3E=0;{struct Cyc_Absyn_Aggrdecl*
d2;{struct _tuple6 _tmp46=({struct _tuple6 _tmp45;_tmp45.f1=d0->impl;_tmp45.f2=d1->impl;
_tmp45;});struct Cyc_Absyn_AggrdeclImpl*_tmp47;struct Cyc_Absyn_AggrdeclImpl*
_tmp48;struct Cyc_Absyn_AggrdeclImpl*_tmp49;struct Cyc_Absyn_AggrdeclImpl _tmp4A;
struct Cyc_List_List*_tmp4B;struct Cyc_List_List*_tmp4C;struct Cyc_List_List*_tmp4D;
struct Cyc_Absyn_AggrdeclImpl*_tmp4E;struct Cyc_Absyn_AggrdeclImpl _tmp4F;struct Cyc_List_List*
_tmp50;struct Cyc_List_List*_tmp51;struct Cyc_List_List*_tmp52;_LL1B: _tmp47=_tmp46.f2;
if(_tmp47 != 0)goto _LL1D;_LL1C: d2=d0;goto _LL1A;_LL1D: _tmp48=_tmp46.f1;if(_tmp48 != 
0)goto _LL1F;_LL1E: d2=d1;goto _LL1A;_LL1F: _tmp49=_tmp46.f1;if(_tmp49 == 0)goto _LL1A;
_tmp4A=*_tmp49;_tmp4B=_tmp4A.exist_vars;_tmp4C=_tmp4A.rgn_po;_tmp4D=_tmp4A.fields;
_tmp4E=_tmp46.f2;if(_tmp4E == 0)goto _LL1A;_tmp4F=*_tmp4E;_tmp50=_tmp4F.exist_vars;
_tmp51=_tmp4F.rgn_po;_tmp52=_tmp4F.fields;_LL20: if(!Cyc_Tcdecl_check_tvs(_tmp4B,
_tmp50,({const char*_tmp53="type";_tag_arr(_tmp53,sizeof(char),_get_zero_arr_size(
_tmp53,5));}),_tmp3D,loc,msg))return 0;{struct Cyc_List_List*_tmp54=Cyc_Tcdecl_build_tvs_map(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
d0->tvs,_tmp4B),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(d1->tvs,_tmp50));for(0;_tmp4C != 0?_tmp51 != 0: 0;(_tmp4C=
_tmp4C->tl,_tmp51=_tmp51->tl)){Cyc_Tcdecl_check_type((*((struct _tuple4*)((struct
_tuple4*)_tmp4C->hd))).f1,(*((struct _tuple4*)((struct _tuple4*)_tmp51->hd))).f1);
Cyc_Tcdecl_check_type((*((struct _tuple4*)((struct _tuple4*)_tmp4C->hd))).f2,(*((
struct _tuple4*)((struct _tuple4*)_tmp51->hd))).f2);}for(0;_tmp4D != 0?_tmp52 != 0: 0;(
_tmp4D=_tmp4D->tl,_tmp52=_tmp52->tl)){struct Cyc_Absyn_Aggrfield _tmp56;struct
_tagged_arr*_tmp57;struct Cyc_Absyn_Tqual _tmp58;void*_tmp59;struct Cyc_Absyn_Exp*
_tmp5A;struct Cyc_List_List*_tmp5B;struct Cyc_Absyn_Aggrfield*_tmp55=(struct Cyc_Absyn_Aggrfield*)
_tmp4D->hd;_tmp56=*_tmp55;_tmp57=_tmp56.name;_tmp58=_tmp56.tq;_tmp59=(void*)
_tmp56.type;_tmp5A=_tmp56.width;_tmp5B=_tmp56.attributes;{struct Cyc_Absyn_Aggrfield
_tmp5D;struct _tagged_arr*_tmp5E;struct Cyc_Absyn_Tqual _tmp5F;void*_tmp60;struct
Cyc_Absyn_Exp*_tmp61;struct Cyc_List_List*_tmp62;struct Cyc_Absyn_Aggrfield*_tmp5C=(
struct Cyc_Absyn_Aggrfield*)_tmp52->hd;_tmp5D=*_tmp5C;_tmp5E=_tmp5D.name;_tmp5F=
_tmp5D.tq;_tmp60=(void*)_tmp5D.type;_tmp61=_tmp5D.width;_tmp62=_tmp5D.attributes;
if(Cyc_Std_strptrcmp(_tmp57,_tmp5E)!= 0){({struct Cyc_Std_String_pa_struct _tmp68;
_tmp68.tag=0;_tmp68.f1=(struct _tagged_arr)*((struct _tagged_arr*)_tmp5E);{struct
Cyc_Std_String_pa_struct _tmp67;_tmp67.tag=0;_tmp67.f1=(struct _tagged_arr)*((
struct _tagged_arr*)_tmp57);{struct Cyc_Std_String_pa_struct _tmp66;_tmp66.tag=0;
_tmp66.f1=(struct _tagged_arr)_tmp3D;{struct Cyc_Std_String_pa_struct _tmp65;_tmp65.tag=
0;_tmp65.f1=({const char*_tmp69="type";_tag_arr(_tmp69,sizeof(char),
_get_zero_arr_size(_tmp69,5));});{void*_tmp63[4]={& _tmp65,& _tmp66,& _tmp67,&
_tmp68};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp64="%s %s : field name mismatch %s != %s";
_tag_arr(_tmp64,sizeof(char),_get_zero_arr_size(_tmp64,37));}),_tag_arr(_tmp63,
sizeof(void*),4));}}}}});return 0;}if(!Cyc_Tcutil_same_atts(_tmp5B,_tmp62)){({
struct Cyc_Std_String_pa_struct _tmp6E;_tmp6E.tag=0;_tmp6E.f1=(struct _tagged_arr)*((
struct _tagged_arr*)_tmp57);{struct Cyc_Std_String_pa_struct _tmp6D;_tmp6D.tag=0;
_tmp6D.f1=(struct _tagged_arr)_tmp3D;{struct Cyc_Std_String_pa_struct _tmp6C;_tmp6C.tag=
0;_tmp6C.f1=({const char*_tmp6F="type";_tag_arr(_tmp6F,sizeof(char),
_get_zero_arr_size(_tmp6F,5));});{void*_tmp6A[3]={& _tmp6C,& _tmp6D,& _tmp6E};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp6B="%s %s : attribute mismatch on field %s";_tag_arr(
_tmp6B,sizeof(char),_get_zero_arr_size(_tmp6B,39));}),_tag_arr(_tmp6A,sizeof(
void*),3));}}}});_tmp3E=0;}if(!Cyc_Tcutil_equal_tqual(_tmp58,_tmp5F)){({struct
Cyc_Std_String_pa_struct _tmp74;_tmp74.tag=0;_tmp74.f1=(struct _tagged_arr)*((
struct _tagged_arr*)_tmp57);{struct Cyc_Std_String_pa_struct _tmp73;_tmp73.tag=0;
_tmp73.f1=(struct _tagged_arr)_tmp3D;{struct Cyc_Std_String_pa_struct _tmp72;_tmp72.tag=
0;_tmp72.f1=({const char*_tmp75="type";_tag_arr(_tmp75,sizeof(char),
_get_zero_arr_size(_tmp75,5));});{void*_tmp70[3]={& _tmp72,& _tmp73,& _tmp74};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp71="%s %s : qualifier mismatch on field %s";_tag_arr(
_tmp71,sizeof(char),_get_zero_arr_size(_tmp71,39));}),_tag_arr(_tmp70,sizeof(
void*),3));}}}});_tmp3E=0;}if((((_tmp5A != 0?_tmp61 != 0: 0)?Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)_check_null(_tmp5A))!= Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)_check_null(_tmp61)): 0)?1:(_tmp5A == 0?_tmp61 != 0: 0))?1:(
_tmp5A != 0?_tmp61 == 0: 0)){({struct Cyc_Std_String_pa_struct _tmp7A;_tmp7A.tag=0;
_tmp7A.f1=(struct _tagged_arr)*((struct _tagged_arr*)_tmp57);{struct Cyc_Std_String_pa_struct
_tmp79;_tmp79.tag=0;_tmp79.f1=(struct _tagged_arr)_tmp3D;{struct Cyc_Std_String_pa_struct
_tmp78;_tmp78.tag=0;_tmp78.f1=({const char*_tmp7B="type";_tag_arr(_tmp7B,sizeof(
char),_get_zero_arr_size(_tmp7B,5));});{void*_tmp76[3]={& _tmp78,& _tmp79,& _tmp7A};
Cyc_Tcdecl_merr(loc,msg,({const char*_tmp77="%s %s : bitfield mismatch on field %s";
_tag_arr(_tmp77,sizeof(char),_get_zero_arr_size(_tmp77,38));}),_tag_arr(_tmp76,
sizeof(void*),3));}}}});_tmp3E=0;}{void*subst_t1=Cyc_Tcutil_substitute(_tmp54,
_tmp60);if(!Cyc_Tcdecl_check_type(_tmp59,subst_t1)){({struct Cyc_Std_String_pa_struct
_tmp81;_tmp81.tag=0;_tmp81.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(subst_t1);{
struct Cyc_Std_String_pa_struct _tmp80;_tmp80.tag=0;_tmp80.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(_tmp59);{struct Cyc_Std_String_pa_struct _tmp7F;_tmp7F.tag=
0;_tmp7F.f1=(struct _tagged_arr)*((struct _tagged_arr*)_tmp57);{struct Cyc_Std_String_pa_struct
_tmp7E;_tmp7E.tag=0;_tmp7E.f1=(struct _tagged_arr)_tmp3D;{void*_tmp7C[4]={& _tmp7E,&
_tmp7F,& _tmp80,& _tmp81};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp7D="type %s : type mismatch on field %s: %s != %s";
_tag_arr(_tmp7D,sizeof(char),_get_zero_arr_size(_tmp7D,46));}),_tag_arr(_tmp7C,
sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();_tmp3E=0;}}}}if(_tmp4D != 0){({
struct Cyc_Std_String_pa_struct _tmp85;_tmp85.tag=0;_tmp85.f1=(struct _tagged_arr)*((
struct _tagged_arr*)((struct Cyc_Absyn_Aggrfield*)_tmp4D->hd)->name);{struct Cyc_Std_String_pa_struct
_tmp84;_tmp84.tag=0;_tmp84.f1=(struct _tagged_arr)_tmp3D;{void*_tmp82[2]={& _tmp84,&
_tmp85};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp83="type %s is missing field %s";
_tag_arr(_tmp83,sizeof(char),_get_zero_arr_size(_tmp83,28));}),_tag_arr(_tmp82,
sizeof(void*),2));}}});_tmp3E=0;}if(_tmp52 != 0){({struct Cyc_Std_String_pa_struct
_tmp89;_tmp89.tag=0;_tmp89.f1=(struct _tagged_arr)*((struct _tagged_arr*)((struct
Cyc_Absyn_Aggrfield*)_tmp52->hd)->name);{struct Cyc_Std_String_pa_struct _tmp88;
_tmp88.tag=0;_tmp88.f1=(struct _tagged_arr)_tmp3D;{void*_tmp86[2]={& _tmp88,&
_tmp89};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp87="type %s has extra field %s";
_tag_arr(_tmp87,sizeof(char),_get_zero_arr_size(_tmp87,27));}),_tag_arr(_tmp86,
sizeof(void*),2));}}});_tmp3E=0;}d2=d0;goto _LL1A;}_LL1A:;}if(!_tmp3E)return 0;if(
_tmp42 == (void*)d2->sc)return(struct Cyc_Absyn_Aggrdecl*)d2;else{d2=({struct Cyc_Absyn_Aggrdecl*
_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A[0]=*((struct Cyc_Absyn_Aggrdecl*)d2);
_tmp8A;});(void*)(d2->sc=(void*)_tmp42);return(struct Cyc_Absyn_Aggrdecl*)d2;}}}}
inline static struct _tagged_arr Cyc_Tcdecl_is_x2string(int is_x){return is_x?({const
char*_tmp8B="xtunion";_tag_arr(_tmp8B,sizeof(char),_get_zero_arr_size(_tmp8B,8));}):({
const char*_tmp8C="tunion";_tag_arr(_tmp8C,sizeof(char),_get_zero_arr_size(_tmp8C,
7));});}struct _tuple7{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_Absyn_Tunionfield*
Cyc_Tcdecl_merge_tunionfield(struct Cyc_Absyn_Tunionfield*f0,struct Cyc_Absyn_Tunionfield*
f1,struct Cyc_List_List*inst,struct _tagged_arr t,struct _tagged_arr v,struct
_tagged_arr*msg){struct Cyc_Position_Segment*loc=f1->loc;if(Cyc_Std_strptrcmp((*((
struct _tuple0*)f0->name)).f2,(*((struct _tuple0*)f1->name)).f2)!= 0){({struct Cyc_Std_String_pa_struct
_tmp92;_tmp92.tag=0;_tmp92.f1=(struct _tagged_arr)*((struct _tagged_arr*)(*((
struct _tuple0*)f0->name)).f2);{struct Cyc_Std_String_pa_struct _tmp91;_tmp91.tag=0;
_tmp91.f1=(struct _tagged_arr)*((struct _tagged_arr*)(*((struct _tuple0*)f1->name)).f2);{
struct Cyc_Std_String_pa_struct _tmp90;_tmp90.tag=0;_tmp90.f1=(struct _tagged_arr)v;{
struct Cyc_Std_String_pa_struct _tmp8F;_tmp8F.tag=0;_tmp8F.f1=(struct _tagged_arr)t;{
void*_tmp8D[4]={& _tmp8F,& _tmp90,& _tmp91,& _tmp92};Cyc_Tcdecl_merr(loc,msg,({const
char*_tmp8E="%s %s: field name mismatch %s != %s";_tag_arr(_tmp8E,sizeof(char),
_get_zero_arr_size(_tmp8E,36));}),_tag_arr(_tmp8D,sizeof(void*),4));}}}}});
return 0;}{struct _tagged_arr _tmp93=*((struct _tagged_arr*)(*((struct _tuple0*)f0->name)).f2);
void*_tmp99;int _tmp9A;struct _tuple3 _tmp98=Cyc_Tcdecl_merge_scope((void*)f0->sc,(
void*)f1->sc,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp97;_tmp97.tag=
0;_tmp97.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct _tmp96;_tmp96.tag=
0;_tmp96.f1=(struct _tagged_arr)t;{void*_tmp94[2]={& _tmp96,& _tmp97};Cyc_Std_aprintf(({
const char*_tmp95="in %s %s, field";_tag_arr(_tmp95,sizeof(char),
_get_zero_arr_size(_tmp95,16));}),_tag_arr(_tmp94,sizeof(void*),2));}}}),_tmp93,
loc,msg);_tmp99=_tmp98.f1;_tmp9A=_tmp98.f2;{struct Cyc_List_List*_tmp9B=f0->typs;
struct Cyc_List_List*_tmp9C=f1->typs;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp9B)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9C)){({struct Cyc_Std_String_pa_struct
_tmpA1;_tmpA1.tag=0;_tmpA1.f1=(struct _tagged_arr)_tmp93;{struct Cyc_Std_String_pa_struct
_tmpA0;_tmpA0.tag=0;_tmpA0.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmp9F;_tmp9F.tag=0;_tmp9F.f1=(struct _tagged_arr)t;{void*_tmp9D[3]={& _tmp9F,&
_tmpA0,& _tmpA1};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp9E="%s %s, field %s: parameter number mismatch";
_tag_arr(_tmp9E,sizeof(char),_get_zero_arr_size(_tmp9E,43));}),_tag_arr(_tmp9D,
sizeof(void*),3));}}}});_tmp9A=0;}for(0;_tmp9B != 0;(_tmp9B=_tmp9B->tl,_tmp9C=
_tmp9C->tl)){if(!Cyc_Tcutil_equal_tqual((*((struct _tuple7*)((struct _tuple7*)
_tmp9B->hd))).f1,(*((struct _tuple7*)((struct _tuple7*)((struct Cyc_List_List*)
_check_null(_tmp9C))->hd))).f1)){({struct Cyc_Std_String_pa_struct _tmpA6;_tmpA6.tag=
0;_tmpA6.f1=(struct _tagged_arr)_tmp93;{struct Cyc_Std_String_pa_struct _tmpA5;
_tmpA5.tag=0;_tmpA5.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmpA4;_tmpA4.tag=0;_tmpA4.f1=(struct _tagged_arr)t;{void*_tmpA2[3]={& _tmpA4,&
_tmpA5,& _tmpA6};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpA3="%s %s, field %s: parameter qualifier";
_tag_arr(_tmpA3,sizeof(char),_get_zero_arr_size(_tmpA3,37));}),_tag_arr(_tmpA2,
sizeof(void*),3));}}}});_tmp9A=0;}{void*subst_t1=Cyc_Tcutil_substitute(inst,(*((
struct _tuple7*)((struct _tuple7*)_tmp9C->hd))).f2);if(!Cyc_Tcdecl_check_type((*((
struct _tuple7*)((struct _tuple7*)_tmp9B->hd))).f2,subst_t1)){({struct Cyc_Std_String_pa_struct
_tmpAD;_tmpAD.tag=0;_tmpAD.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(subst_t1);{
struct Cyc_Std_String_pa_struct _tmpAC;_tmpAC.tag=0;_tmpAC.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string((*((struct _tuple7*)((struct _tuple7*)_tmp9B->hd))).f2);{
struct Cyc_Std_String_pa_struct _tmpAB;_tmpAB.tag=0;_tmpAB.f1=(struct _tagged_arr)
_tmp93;{struct Cyc_Std_String_pa_struct _tmpAA;_tmpAA.tag=0;_tmpAA.f1=(struct
_tagged_arr)v;{struct Cyc_Std_String_pa_struct _tmpA9;_tmpA9.tag=0;_tmpA9.f1=(
struct _tagged_arr)t;{void*_tmpA7[5]={& _tmpA9,& _tmpAA,& _tmpAB,& _tmpAC,& _tmpAD};
Cyc_Tcdecl_merr(loc,msg,({const char*_tmpA8="%s %s, field %s: parameter type mismatch %s != %s";
_tag_arr(_tmpA8,sizeof(char),_get_zero_arr_size(_tmpA8,50));}),_tag_arr(_tmpA7,
sizeof(void*),5));}}}}}});Cyc_Tcutil_explain_failure();_tmp9A=0;}}}if(!_tmp9A)
return 0;if((void*)f0->sc != _tmp99){struct Cyc_Absyn_Tunionfield*_tmpAE=({struct
Cyc_Absyn_Tunionfield*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=*((struct Cyc_Absyn_Tunionfield*)
f0);_tmpAF;});(void*)(((struct Cyc_Absyn_Tunionfield*)_check_null(_tmpAE))->sc=(
void*)_tmp99);return _tmpAE;}else{return(struct Cyc_Absyn_Tunionfield*)f0;}}}}
static struct _tuple7*Cyc_Tcdecl_substitute_tunionfield_f2(struct Cyc_List_List*
inst,struct _tuple7*x){struct _tuple7 _tmpB1;struct Cyc_Absyn_Tqual _tmpB2;void*
_tmpB3;struct _tuple7*_tmpB0=x;_tmpB1=*_tmpB0;_tmpB2=_tmpB1.f1;_tmpB3=_tmpB1.f2;
return({struct _tuple7*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->f1=_tmpB2;_tmpB4->f2=
Cyc_Tcutil_substitute(inst,_tmpB3);_tmpB4;});}static struct Cyc_Absyn_Tunionfield*
Cyc_Tcdecl_substitute_tunionfield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Tunionfield*
f1){struct Cyc_Absyn_Tunionfield*_tmpB5=({struct Cyc_Absyn_Tunionfield*_tmpB6=
_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=*((struct Cyc_Absyn_Tunionfield*)f1);_tmpB6;});
_tmpB5->typs=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct Cyc_List_List*,
struct _tuple7*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcdecl_substitute_tunionfield_f2,inst1,f1->typs);return _tmpB5;}static struct
Cyc_List_List*Cyc_Tcdecl_merge_xtunion_fields(struct Cyc_List_List*f0s,struct Cyc_List_List*
f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,
int*res,int*incl,struct _tagged_arr t,struct _tagged_arr v,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg){struct Cyc_List_List**f2sp=({struct Cyc_List_List**
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=0;_tmpBF;});struct Cyc_List_List**
_tmpB7=f2sp;int cmp=- 1;for(0;f0s != 0?f1s != 0: 0;f1s=f1s->tl){while(f0s != 0?(cmp=
Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Tunionfield*)f0s->hd)->name,((struct Cyc_Absyn_Tunionfield*)
f1s->hd)->name))< 0: 0){struct Cyc_List_List*_tmpB8=({struct Cyc_List_List*_tmpB9=
_cycalloc(sizeof(*_tmpB9));_tmpB9->hd=(struct Cyc_Absyn_Tunionfield*)f0s->hd;
_tmpB9->tl=0;_tmpB9;});*((struct Cyc_List_List**)_tmpB7)=_tmpB8;_tmpB7=&((struct
Cyc_List_List*)_check_null(_tmpB8))->tl;f0s=f0s->tl;}if(f0s == 0?1: cmp > 0){*((int*)
incl)=0;{struct Cyc_List_List*_tmpBA=({struct Cyc_List_List*_tmpBB=_cycalloc(
sizeof(*_tmpBB));_tmpBB->hd=Cyc_Tcdecl_substitute_tunionfield(inst,(struct Cyc_Absyn_Tunionfield*)
f1s->hd);_tmpBB->tl=0;_tmpBB;});*((struct Cyc_List_List**)_tmpB7)=_tmpBA;_tmpB7=&((
struct Cyc_List_List*)_check_null(_tmpBA))->tl;}}else{struct Cyc_Absyn_Tunionfield*
_tmpBC=Cyc_Tcdecl_merge_tunionfield((struct Cyc_Absyn_Tunionfield*)f0s->hd,(
struct Cyc_Absyn_Tunionfield*)f1s->hd,inst,t,v,msg);if(_tmpBC != 0){if(_tmpBC != (
struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)f0s->hd))*((int*)
incl)=0;{struct Cyc_List_List*_tmpBD=({struct Cyc_List_List*_tmpBE=_cycalloc(
sizeof(*_tmpBE));_tmpBE->hd=(struct Cyc_Absyn_Tunionfield*)_check_null(_tmpBC);
_tmpBE->tl=0;_tmpBE;});*((struct Cyc_List_List**)_tmpB7)=_tmpBD;_tmpB7=&((struct
Cyc_List_List*)_check_null(_tmpBD))->tl;}}else{*((int*)res)=0;}f0s=f0s->tl;}}if(
f1s != 0){*((int*)incl)=0;*((struct Cyc_List_List**)_tmpB7)=f1s;}else{*((struct Cyc_List_List**)
_tmpB7)=f0s;}return*((struct Cyc_List_List**)f2sp);}struct _tuple8{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};static struct _tuple8 Cyc_Tcdecl_split(struct Cyc_List_List*
f){if(f == 0)return({struct _tuple8 _tmpC0;_tmpC0.f1=0;_tmpC0.f2=0;_tmpC0;});if(f->tl
== 0)return({struct _tuple8 _tmpC1;_tmpC1.f1=f;_tmpC1.f2=0;_tmpC1;});{struct Cyc_List_List*
_tmpC3;struct Cyc_List_List*_tmpC4;struct _tuple8 _tmpC2=Cyc_Tcdecl_split(((struct
Cyc_List_List*)_check_null(f->tl))->tl);_tmpC3=_tmpC2.f1;_tmpC4=_tmpC2.f2;return({
struct _tuple8 _tmpC5;_tmpC5.f1=({struct Cyc_List_List*_tmpC7=_cycalloc(sizeof(*
_tmpC7));_tmpC7->hd=(void*)((void*)f->hd);_tmpC7->tl=_tmpC3;_tmpC7;});_tmpC5.f2=({
struct Cyc_List_List*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->hd=(void*)((void*)((
struct Cyc_List_List*)_check_null(f->tl))->hd);_tmpC6->tl=_tmpC4;_tmpC6;});_tmpC5;});}}
struct Cyc_List_List*Cyc_Tcdecl_sort_xtunion_fields(struct Cyc_List_List*f,int*res,
struct _tagged_arr*v,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){struct
Cyc_List_List*_tmpC9;struct Cyc_List_List*_tmpCA;struct _tuple8 _tmpC8=((struct
_tuple8(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(f);_tmpC9=_tmpC8.f1;_tmpCA=
_tmpC8.f2;if(_tmpC9 != 0?_tmpC9->tl != 0: 0)_tmpC9=Cyc_Tcdecl_sort_xtunion_fields(
_tmpC9,res,v,loc,msg);if(_tmpCA != 0?_tmpCA->tl != 0: 0)_tmpCA=Cyc_Tcdecl_sort_xtunion_fields(
_tmpCA,res,v,loc,msg);return Cyc_Tcdecl_merge_xtunion_fields(_tmpC9,_tmpCA,0,0,0,
res,({int*_tmpCB=_cycalloc_atomic(sizeof(*_tmpCB));_tmpCB[0]=1;_tmpCB;}),({const
char*_tmpCC="xtunion";_tag_arr(_tmpCC,sizeof(char),_get_zero_arr_size(_tmpCC,8));}),*((
struct _tagged_arr*)v),loc,msg);}struct _tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;};struct Cyc_Absyn_Tuniondecl*Cyc_Tcdecl_merge_tuniondecl(struct Cyc_Absyn_Tuniondecl*
d0,struct Cyc_Absyn_Tuniondecl*d1,struct Cyc_Position_Segment*loc,struct
_tagged_arr*msg){struct _tagged_arr _tmpCD=Cyc_Absynpp_qvar2string(d0->name);
struct _tagged_arr t=({const char*_tmpF1="[x]tunion";_tag_arr(_tmpF1,sizeof(char),
_get_zero_arr_size(_tmpF1,10));});int _tmpCE=1;if(d0->is_xtunion != d1->is_xtunion){({
struct Cyc_Std_String_pa_struct _tmpD3;_tmpD3.tag=0;_tmpD3.f1=(struct _tagged_arr)
Cyc_Tcdecl_is_x2string(d1->is_xtunion);{struct Cyc_Std_String_pa_struct _tmpD2;
_tmpD2.tag=0;_tmpD2.f1=(struct _tagged_arr)Cyc_Tcdecl_is_x2string(d0->is_xtunion);{
struct Cyc_Std_String_pa_struct _tmpD1;_tmpD1.tag=0;_tmpD1.f1=(struct _tagged_arr)
_tmpCD;{void*_tmpCF[3]={& _tmpD1,& _tmpD2,& _tmpD3};Cyc_Tcdecl_merr(loc,msg,({const
char*_tmpD0="expected %s to be a %s instead of a %s";_tag_arr(_tmpD0,sizeof(char),
_get_zero_arr_size(_tmpD0,39));}),_tag_arr(_tmpCF,sizeof(void*),3));}}}});_tmpCE=
0;}else{t=Cyc_Tcdecl_is_x2string(d0->is_xtunion);}if(!Cyc_Tcdecl_check_tvs(d0->tvs,
d1->tvs,t,_tmpCD,loc,msg))return 0;{void*_tmpD5;int _tmpD6;struct _tuple3 _tmpD4=Cyc_Tcdecl_merge_scope((
void*)d0->sc,(void*)d1->sc,t,_tmpCD,loc,msg);_tmpD5=_tmpD4.f1;_tmpD6=_tmpD4.f2;
if(!_tmpD6)_tmpCE=0;{struct Cyc_Absyn_Tuniondecl*d2;{struct _tuple9 _tmpD8=({struct
_tuple9 _tmpD7;_tmpD7.f1=d0->fields;_tmpD7.f2=d1->fields;_tmpD7;});struct Cyc_Core_Opt*
_tmpD9;struct Cyc_Core_Opt*_tmpDA;struct Cyc_Core_Opt*_tmpDB;struct Cyc_Core_Opt
_tmpDC;struct Cyc_List_List*_tmpDD;struct Cyc_Core_Opt*_tmpDE;struct Cyc_Core_Opt
_tmpDF;struct Cyc_List_List*_tmpE0;_LL22: _tmpD9=_tmpD8.f2;if(_tmpD9 != 0)goto _LL24;
_LL23: d2=d0;goto _LL21;_LL24: _tmpDA=_tmpD8.f1;if(_tmpDA != 0)goto _LL26;_LL25: d2=d1;
goto _LL21;_LL26: _tmpDB=_tmpD8.f1;if(_tmpDB == 0)goto _LL21;_tmpDC=*_tmpDB;_tmpDD=(
struct Cyc_List_List*)_tmpDC.v;_tmpDE=_tmpD8.f2;if(_tmpDE == 0)goto _LL21;_tmpDF=*
_tmpDE;_tmpE0=(struct Cyc_List_List*)_tmpDF.v;_LL27: {struct Cyc_List_List*_tmpE1=
Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);if(d0->is_xtunion){int _tmpE2=1;struct
Cyc_List_List*_tmpE3=Cyc_Tcdecl_merge_xtunion_fields(_tmpDD,_tmpE0,_tmpE1,d0->tvs,
d1->tvs,& _tmpCE,& _tmpE2,t,_tmpCD,loc,msg);if(_tmpE2)d2=d0;else{d2=({struct Cyc_Absyn_Tuniondecl*
_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=*((struct Cyc_Absyn_Tuniondecl*)d0);
_tmpE4;});(void*)(d2->sc=(void*)_tmpD5);d2->fields=({struct Cyc_Core_Opt*_tmpE5=
_cycalloc(sizeof(*_tmpE5));_tmpE5->v=_tmpE3;_tmpE5;});}}else{for(0;_tmpDD != 0?
_tmpE0 != 0: 0;(_tmpDD=_tmpDD->tl,_tmpE0=_tmpE0->tl)){Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)_tmpDD->hd,(struct Cyc_Absyn_Tunionfield*)_tmpE0->hd,
_tmpE1,t,_tmpCD,msg);}if(_tmpDD != 0){({struct Cyc_Std_String_pa_struct _tmpEA;
_tmpEA.tag=0;_tmpEA.f1=(struct _tagged_arr)*((struct _tagged_arr*)(*((struct
_tuple0*)((struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(
_tmpE0))->hd)->name)).f2);{struct Cyc_Std_String_pa_struct _tmpE9;_tmpE9.tag=0;
_tmpE9.f1=(struct _tagged_arr)_tmpCD;{struct Cyc_Std_String_pa_struct _tmpE8;_tmpE8.tag=
0;_tmpE8.f1=(struct _tagged_arr)t;{void*_tmpE6[3]={& _tmpE8,& _tmpE9,& _tmpEA};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmpE7="%s %s has extra field %s";_tag_arr(_tmpE7,sizeof(
char),_get_zero_arr_size(_tmpE7,25));}),_tag_arr(_tmpE6,sizeof(void*),3));}}}});
_tmpCE=0;}if(_tmpE0 != 0){({struct Cyc_Std_String_pa_struct _tmpEF;_tmpEF.tag=0;
_tmpEF.f1=(struct _tagged_arr)*((struct _tagged_arr*)(*((struct _tuple0*)((struct
Cyc_Absyn_Tunionfield*)_tmpE0->hd)->name)).f2);{struct Cyc_Std_String_pa_struct
_tmpEE;_tmpEE.tag=0;_tmpEE.f1=(struct _tagged_arr)_tmpCD;{struct Cyc_Std_String_pa_struct
_tmpED;_tmpED.tag=0;_tmpED.f1=(struct _tagged_arr)t;{void*_tmpEB[3]={& _tmpED,&
_tmpEE,& _tmpEF};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpEC="%s %s is missing field %s";
_tag_arr(_tmpEC,sizeof(char),_get_zero_arr_size(_tmpEC,26));}),_tag_arr(_tmpEB,
sizeof(void*),3));}}}});_tmpCE=0;}d2=d0;}goto _LL21;}_LL21:;}if(!_tmpCE)return 0;
if(_tmpD5 == (void*)d2->sc)return(struct Cyc_Absyn_Tuniondecl*)d2;else{d2=({struct
Cyc_Absyn_Tuniondecl*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=*((struct Cyc_Absyn_Tuniondecl*)
d2);_tmpF0;});(void*)(d2->sc=(void*)_tmpD5);return(struct Cyc_Absyn_Tuniondecl*)
d2;}}}}struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*
d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*
msg){struct _tagged_arr _tmpF2=Cyc_Absynpp_qvar2string(d0->name);int _tmpF3=1;void*
_tmpF6;int _tmpF7;struct _tuple3 _tmpF5=Cyc_Tcdecl_merge_scope((void*)d0->sc,(void*)
d1->sc,({const char*_tmpF4="enum";_tag_arr(_tmpF4,sizeof(char),_get_zero_arr_size(
_tmpF4,5));}),_tmpF2,loc,msg);_tmpF6=_tmpF5.f1;_tmpF7=_tmpF5.f2;if(!_tmpF7)
_tmpF3=0;{struct Cyc_Absyn_Enumdecl*d2;{struct _tuple9 _tmpF9=({struct _tuple9 _tmpF8;
_tmpF8.f1=d0->fields;_tmpF8.f2=d1->fields;_tmpF8;});struct Cyc_Core_Opt*_tmpFA;
struct Cyc_Core_Opt*_tmpFB;struct Cyc_Core_Opt*_tmpFC;struct Cyc_Core_Opt _tmpFD;
struct Cyc_List_List*_tmpFE;struct Cyc_Core_Opt*_tmpFF;struct Cyc_Core_Opt _tmp100;
struct Cyc_List_List*_tmp101;_LL29: _tmpFA=_tmpF9.f2;if(_tmpFA != 0)goto _LL2B;_LL2A:
d2=d0;goto _LL28;_LL2B: _tmpFB=_tmpF9.f1;if(_tmpFB != 0)goto _LL2D;_LL2C: d2=d1;goto
_LL28;_LL2D: _tmpFC=_tmpF9.f1;if(_tmpFC == 0)goto _LL28;_tmpFD=*_tmpFC;_tmpFE=(
struct Cyc_List_List*)_tmpFD.v;_tmpFF=_tmpF9.f2;if(_tmpFF == 0)goto _LL28;_tmp100=*
_tmpFF;_tmp101=(struct Cyc_List_List*)_tmp100.v;_LL2E: for(0;_tmpFE != 0?_tmp101 != 
0: 0;(_tmpFE=_tmpFE->tl,_tmp101=_tmp101->tl)){struct Cyc_Absyn_Enumfield _tmp103;
struct _tuple0*_tmp104;struct _tuple0 _tmp105;struct _tagged_arr*_tmp106;struct Cyc_Absyn_Exp*
_tmp107;struct Cyc_Position_Segment*_tmp108;struct Cyc_Absyn_Enumfield*_tmp102=(
struct Cyc_Absyn_Enumfield*)_tmpFE->hd;_tmp103=*_tmp102;_tmp104=_tmp103.name;
_tmp105=*_tmp104;_tmp106=_tmp105.f2;_tmp107=_tmp103.tag;_tmp108=_tmp103.loc;{
struct Cyc_Absyn_Enumfield _tmp10A;struct _tuple0*_tmp10B;struct _tuple0 _tmp10C;
struct _tagged_arr*_tmp10D;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Position_Segment*
_tmp10F;struct Cyc_Absyn_Enumfield*_tmp109=(struct Cyc_Absyn_Enumfield*)_tmp101->hd;
_tmp10A=*_tmp109;_tmp10B=_tmp10A.name;_tmp10C=*_tmp10B;_tmp10D=_tmp10C.f2;
_tmp10E=_tmp10A.tag;_tmp10F=_tmp10A.loc;if(Cyc_Std_strptrcmp(_tmp106,_tmp10D)!= 
0){({struct Cyc_Std_String_pa_struct _tmp114;_tmp114.tag=0;_tmp114.f1=(struct
_tagged_arr)*((struct _tagged_arr*)_tmp10D);{struct Cyc_Std_String_pa_struct
_tmp113;_tmp113.tag=0;_tmp113.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp106);{struct Cyc_Std_String_pa_struct _tmp112;_tmp112.tag=0;_tmp112.f1=(struct
_tagged_arr)_tmpF2;{void*_tmp110[3]={& _tmp112,& _tmp113,& _tmp114};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp111="enum %s: field name mismatch %s != %s";_tag_arr(
_tmp111,sizeof(char),_get_zero_arr_size(_tmp111,38));}),_tag_arr(_tmp110,sizeof(
void*),3));}}}});_tmpF3=0;}if(Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)
_check_null(_tmp107))!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)
_check_null(_tmp10E))){({struct Cyc_Std_String_pa_struct _tmp118;_tmp118.tag=0;
_tmp118.f1=(struct _tagged_arr)*((struct _tagged_arr*)_tmp10D);{struct Cyc_Std_String_pa_struct
_tmp117;_tmp117.tag=0;_tmp117.f1=(struct _tagged_arr)_tmpF2;{void*_tmp115[2]={&
_tmp117,& _tmp118};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp116="enum %s, field %s, value mismatch";
_tag_arr(_tmp116,sizeof(char),_get_zero_arr_size(_tmp116,34));}),_tag_arr(
_tmp115,sizeof(void*),2));}}});_tmpF3=0;}}}d2=d0;goto _LL28;_LL28:;}if(!_tmpF3)
return 0;if((void*)d2->sc == _tmpF6)return(struct Cyc_Absyn_Enumdecl*)d2;else{d2=({
struct Cyc_Absyn_Enumdecl*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119[0]=*((
struct Cyc_Absyn_Enumdecl*)d2);_tmp119;});(void*)(d2->sc=(void*)_tmpF6);return(
struct Cyc_Absyn_Enumdecl*)d2;}}}static struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl(
void*sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,void*sc1,
void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _tagged_arr t,
struct _tagged_arr v,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){int
_tmp11A=1;void*_tmp11C;int _tmp11D;struct _tuple3 _tmp11B=Cyc_Tcdecl_merge_scope(
sc0,sc1,t,v,loc,msg);_tmp11C=_tmp11B.f1;_tmp11D=_tmp11B.f2;if(!_tmp11D)_tmp11A=0;
if(!Cyc_Tcdecl_check_type(t0,t1)){({struct Cyc_Std_String_pa_struct _tmp123;
_tmp123.tag=0;_tmp123.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t0);{struct
Cyc_Std_String_pa_struct _tmp122;_tmp122.tag=0;_tmp122.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t1);{struct Cyc_Std_String_pa_struct _tmp121;_tmp121.tag=0;_tmp121.f1=(struct
_tagged_arr)v;{struct Cyc_Std_String_pa_struct _tmp120;_tmp120.tag=0;_tmp120.f1=(
struct _tagged_arr)t;{void*_tmp11E[4]={& _tmp120,& _tmp121,& _tmp122,& _tmp123};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp11F="%s %s has type \n%s\n instead of \n%s\n";_tag_arr(
_tmp11F,sizeof(char),_get_zero_arr_size(_tmp11F,36));}),_tag_arr(_tmp11E,sizeof(
void*),4));}}}}});Cyc_Tcutil_explain_failure();_tmp11A=0;}if(!Cyc_Tcutil_equal_tqual(
tq0,tq1)){({struct Cyc_Std_String_pa_struct _tmp127;_tmp127.tag=0;_tmp127.f1=(
struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct _tmp126;_tmp126.tag=0;_tmp126.f1=(
struct _tagged_arr)t;{void*_tmp124[2]={& _tmp126,& _tmp127};Cyc_Tcdecl_merr(loc,msg,({
const char*_tmp125="%s %s has different type qualifiers";_tag_arr(_tmp125,sizeof(
char),_get_zero_arr_size(_tmp125,36));}),_tag_arr(_tmp124,sizeof(void*),2));}}});
_tmp11A=0;}if(!Cyc_Tcutil_same_atts(atts0,atts1)){({struct Cyc_Std_String_pa_struct
_tmp12B;_tmp12B.tag=0;_tmp12B.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmp12A;_tmp12A.tag=0;_tmp12A.f1=(struct _tagged_arr)t;{void*_tmp128[2]={& _tmp12A,&
_tmp12B};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp129="%s %s has different attributes";
_tag_arr(_tmp129,sizeof(char),_get_zero_arr_size(_tmp129,31));}),_tag_arr(
_tmp128,sizeof(void*),2));}}});({void*_tmp12C[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp12D="previous attributes: ";_tag_arr(_tmp12D,sizeof(char),
_get_zero_arr_size(_tmp12D,22));}),_tag_arr(_tmp12C,sizeof(void*),0));});for(0;
atts0 != 0;atts0=atts0->tl){({struct Cyc_Std_String_pa_struct _tmp130;_tmp130.tag=0;
_tmp130.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts0->hd);{void*
_tmp12E[1]={& _tmp130};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp12F="%s ";
_tag_arr(_tmp12F,sizeof(char),_get_zero_arr_size(_tmp12F,4));}),_tag_arr(_tmp12E,
sizeof(void*),1));}});}({void*_tmp131[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp132="\ncurrent attributes: ";_tag_arr(_tmp132,sizeof(char),
_get_zero_arr_size(_tmp132,22));}),_tag_arr(_tmp131,sizeof(void*),0));});for(0;
atts1 != 0;atts1=atts1->tl){({struct Cyc_Std_String_pa_struct _tmp135;_tmp135.tag=0;
_tmp135.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts1->hd);{void*
_tmp133[1]={& _tmp135};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp134="%s ";
_tag_arr(_tmp134,sizeof(char),_get_zero_arr_size(_tmp134,4));}),_tag_arr(_tmp133,
sizeof(void*),1));}});}({void*_tmp136[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp137="\n";_tag_arr(_tmp137,sizeof(char),_get_zero_arr_size(_tmp137,
2));}),_tag_arr(_tmp136,sizeof(void*),0));});_tmp11A=0;}return({struct _tuple3
_tmp138;_tmp138.f1=_tmp11C;_tmp138.f2=_tmp11A;_tmp138;});}struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){struct _tagged_arr _tmp139=
Cyc_Absynpp_qvar2string(d0->name);void*_tmp13C;int _tmp13D;struct _tuple3 _tmp13B=
Cyc_Tcdecl_check_var_or_fn_decl((void*)d0->sc,(void*)d0->type,d0->tq,d0->attributes,(
void*)d1->sc,(void*)d1->type,d1->tq,d1->attributes,({const char*_tmp13A="variable";
_tag_arr(_tmp13A,sizeof(char),_get_zero_arr_size(_tmp13A,9));}),_tmp139,loc,msg);
_tmp13C=_tmp13B.f1;_tmp13D=_tmp13B.f2;if(!_tmp13D)return 0;if((void*)d0->sc == 
_tmp13C)return(struct Cyc_Absyn_Vardecl*)d0;else{struct Cyc_Absyn_Vardecl*_tmp13E=({
struct Cyc_Absyn_Vardecl*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F[0]=*((struct
Cyc_Absyn_Vardecl*)d0);_tmp13F;});(void*)(((struct Cyc_Absyn_Vardecl*)_check_null(
_tmp13E))->sc=(void*)_tmp13C);return _tmp13E;}}struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg){struct _tagged_arr _tmp140=Cyc_Absynpp_qvar2string(d0->name);
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,({const char*_tmp141="typedef";_tag_arr(
_tmp141,sizeof(char),_get_zero_arr_size(_tmp141,8));}),_tmp140,loc,msg))return 0;{
struct Cyc_List_List*_tmp142=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);if(d0->defn
!= 0?d1->defn != 0: 0){void*subst_defn1=Cyc_Tcutil_substitute(_tmp142,(void*)((
struct Cyc_Core_Opt*)_check_null(d1->defn))->v);if(!Cyc_Tcdecl_check_type((void*)((
struct Cyc_Core_Opt*)_check_null(d0->defn))->v,subst_defn1)){({struct Cyc_Std_String_pa_struct
_tmp147;_tmp147.tag=0;_tmp147.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(d0->defn))->v);{struct Cyc_Std_String_pa_struct
_tmp146;_tmp146.tag=0;_tmp146.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
subst_defn1);{struct Cyc_Std_String_pa_struct _tmp145;_tmp145.tag=0;_tmp145.f1=(
struct _tagged_arr)_tmp140;{void*_tmp143[3]={& _tmp145,& _tmp146,& _tmp147};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp144="typedef %s does not refer to the same type: %s != %s";
_tag_arr(_tmp144,sizeof(char),_get_zero_arr_size(_tmp144,53));}),_tag_arr(
_tmp143,sizeof(void*),3));}}}});return 0;}}return(struct Cyc_Absyn_Typedefdecl*)d0;}}
void*Cyc_Tcdecl_merge_binding(void*b0,void*b1,struct Cyc_Position_Segment*loc,
struct _tagged_arr*msg){struct _tuple4 _tmp149=({struct _tuple4 _tmp148;_tmp148.f1=b0;
_tmp148.f2=b1;_tmp148;});void*_tmp14A;void*_tmp14B;void*_tmp14C;struct Cyc_Absyn_Vardecl*
_tmp14D;void*_tmp14E;struct Cyc_Absyn_Vardecl*_tmp14F;void*_tmp150;struct Cyc_Absyn_Vardecl*
_tmp151;void*_tmp152;struct Cyc_Absyn_Fndecl*_tmp153;void*_tmp154;void*_tmp155;
struct Cyc_Absyn_Fndecl*_tmp156;void*_tmp157;struct Cyc_Absyn_Fndecl*_tmp158;void*
_tmp159;struct Cyc_Absyn_Vardecl*_tmp15A;_LL30: _tmp14A=_tmp149.f1;if((int)_tmp14A
!= 0)goto _LL32;_tmp14B=_tmp149.f2;if((int)_tmp14B != 0)goto _LL32;_LL31: return(
void*)0;_LL32: _tmp14C=_tmp149.f1;if(_tmp14C <= (void*)1?1:*((int*)_tmp14C)!= 0)
goto _LL34;_tmp14D=((struct Cyc_Absyn_Global_b_struct*)_tmp14C)->f1;_tmp14E=
_tmp149.f2;if(_tmp14E <= (void*)1?1:*((int*)_tmp14E)!= 0)goto _LL34;_tmp14F=((
struct Cyc_Absyn_Global_b_struct*)_tmp14E)->f1;_LL33: {struct Cyc_Absyn_Vardecl*
_tmp15B=Cyc_Tcdecl_merge_vardecl(_tmp14D,_tmp14F,loc,msg);if(_tmp15B == 0)return(
void*)0;if(_tmp15B == (struct Cyc_Absyn_Vardecl*)_tmp14D)return b0;if(_tmp15B == (
struct Cyc_Absyn_Vardecl*)_tmp14F)return b1;return(void*)({struct Cyc_Absyn_Global_b_struct*
_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C[0]=({struct Cyc_Absyn_Global_b_struct
_tmp15D;_tmp15D.tag=0;_tmp15D.f1=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp15B);
_tmp15D;});_tmp15C;});}_LL34: _tmp150=_tmp149.f1;if(_tmp150 <= (void*)1?1:*((int*)
_tmp150)!= 0)goto _LL36;_tmp151=((struct Cyc_Absyn_Global_b_struct*)_tmp150)->f1;
_tmp152=_tmp149.f2;if(_tmp152 <= (void*)1?1:*((int*)_tmp152)!= 1)goto _LL36;
_tmp153=((struct Cyc_Absyn_Funname_b_struct*)_tmp152)->f1;_LL35: {int _tmp160;
struct _tuple3 _tmp15F=Cyc_Tcdecl_check_var_or_fn_decl((void*)_tmp151->sc,(void*)
_tmp151->type,_tmp151->tq,_tmp151->attributes,(void*)_tmp153->sc,(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp153->cached_typ))->v,Cyc_Absyn_empty_tqual(),
_tmp153->attributes,({const char*_tmp15E="function";_tag_arr(_tmp15E,sizeof(char),
_get_zero_arr_size(_tmp15E,9));}),Cyc_Absynpp_qvar2string(_tmp151->name),loc,msg);
_tmp160=_tmp15F.f2;return _tmp160?b1:(void*)0;}_LL36: _tmp154=_tmp149.f1;if(
_tmp154 <= (void*)1?1:*((int*)_tmp154)!= 1)goto _LL38;_tmp155=_tmp149.f2;if(
_tmp155 <= (void*)1?1:*((int*)_tmp155)!= 1)goto _LL38;_tmp156=((struct Cyc_Absyn_Funname_b_struct*)
_tmp155)->f1;_LL37:({struct Cyc_Std_String_pa_struct _tmp163;_tmp163.tag=0;_tmp163.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp156->name);{void*_tmp161[1]={&
_tmp163};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp162="redefinition of function %s";
_tag_arr(_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,28));}),_tag_arr(
_tmp161,sizeof(void*),1));}});return(void*)0;_LL38: _tmp157=_tmp149.f1;if(_tmp157
<= (void*)1?1:*((int*)_tmp157)!= 1)goto _LL3A;_tmp158=((struct Cyc_Absyn_Funname_b_struct*)
_tmp157)->f1;_tmp159=_tmp149.f2;if(_tmp159 <= (void*)1?1:*((int*)_tmp159)!= 0)
goto _LL3A;_tmp15A=((struct Cyc_Absyn_Global_b_struct*)_tmp159)->f1;_LL39: {int
_tmp166;struct _tuple3 _tmp165=Cyc_Tcdecl_check_var_or_fn_decl((void*)_tmp158->sc,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp158->cached_typ))->v,Cyc_Absyn_empty_tqual(),
_tmp158->attributes,(void*)_tmp15A->sc,(void*)_tmp15A->type,_tmp15A->tq,_tmp15A->attributes,({
const char*_tmp164="variable";_tag_arr(_tmp164,sizeof(char),_get_zero_arr_size(
_tmp164,9));}),Cyc_Absynpp_qvar2string(_tmp158->name),loc,msg);_tmp166=_tmp165.f2;
return _tmp166?b0:(void*)0;}_LL3A:;_LL3B:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp168;_tmp168.tag=Cyc_Core_Invalid_argument;_tmp168.f1=({const char*_tmp169="Tcdecl::merge_binding";
_tag_arr(_tmp169,sizeof(char),_get_zero_arr_size(_tmp169,22));});_tmp168;});
_tmp167;}));_LL2F:;}struct Cyc_Tcdecl_Xtunionfielddecl*Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl*d0,struct Cyc_Tcdecl_Xtunionfielddecl*d1,struct
Cyc_Position_Segment*loc,struct _tagged_arr*msg){struct Cyc_Tcdecl_Xtunionfielddecl
_tmp16B;struct Cyc_Absyn_Tuniondecl*_tmp16C;struct Cyc_Absyn_Tunionfield*_tmp16D;
struct Cyc_Tcdecl_Xtunionfielddecl*_tmp16A=d0;_tmp16B=*_tmp16A;_tmp16C=_tmp16B.base;
_tmp16D=_tmp16B.field;{struct Cyc_Tcdecl_Xtunionfielddecl _tmp16F;struct Cyc_Absyn_Tuniondecl*
_tmp170;struct Cyc_Absyn_Tunionfield*_tmp171;struct Cyc_Tcdecl_Xtunionfielddecl*
_tmp16E=d1;_tmp16F=*_tmp16E;_tmp170=_tmp16F.base;_tmp171=_tmp16F.field;{struct
_tagged_arr _tmp172=Cyc_Absynpp_qvar2string(_tmp16D->name);if(Cyc_Absyn_qvar_cmp(
_tmp16C->name,_tmp170->name)!= 0){({struct Cyc_Std_String_pa_struct _tmp177;
_tmp177.tag=0;_tmp177.f1=(struct _tagged_arr)_tmp172;{struct Cyc_Std_String_pa_struct
_tmp176;_tmp176.tag=0;_tmp176.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp170->name);{struct Cyc_Std_String_pa_struct _tmp175;_tmp175.tag=0;_tmp175.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp16C->name);{void*_tmp173[3]={&
_tmp175,& _tmp176,& _tmp177};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp174="xtunions %s and %s have both a field named %s";
_tag_arr(_tmp174,sizeof(char),_get_zero_arr_size(_tmp174,46));}),_tag_arr(
_tmp173,sizeof(void*),3));}}}});return 0;}if(!Cyc_Tcdecl_check_tvs(_tmp16C->tvs,
_tmp170->tvs,({const char*_tmp178="xtunion";_tag_arr(_tmp178,sizeof(char),
_get_zero_arr_size(_tmp178,8));}),Cyc_Absynpp_qvar2string(_tmp16C->name),loc,msg))
return 0;{struct Cyc_List_List*_tmp179=Cyc_Tcdecl_build_tvs_map(_tmp16C->tvs,
_tmp170->tvs);struct Cyc_Absyn_Tunionfield*_tmp17A=Cyc_Tcdecl_merge_tunionfield(
_tmp16D,_tmp171,_tmp179,({const char*_tmp17C="xtunionfield";_tag_arr(_tmp17C,
sizeof(char),_get_zero_arr_size(_tmp17C,13));}),_tmp172,msg);if(_tmp17A == 0)
return 0;if(_tmp17A == (struct Cyc_Absyn_Tunionfield*)_tmp16D)return(struct Cyc_Tcdecl_Xtunionfielddecl*)
d0;return({struct Cyc_Tcdecl_Xtunionfielddecl*_tmp17B=_cycalloc(sizeof(*_tmp17B));
_tmp17B->base=_tmp16C;_tmp17B->field=(struct Cyc_Absyn_Tunionfield*)_check_null(
_tmp17A);_tmp17B;});}}}}
