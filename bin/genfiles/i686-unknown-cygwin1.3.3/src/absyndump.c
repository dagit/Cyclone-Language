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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;
short*f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;};struct _tagged_arr
Cyc_aprintf(struct _tagged_arr,struct _tagged_arr);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _tagged_arr,struct _tagged_arr);int Cyc_fputc(int,struct Cyc___cycFILE*);
struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int
tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
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
struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int
q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*
f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*name;int*identity;void*kind;
};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrInfo{
void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Tqual
tq;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*zero_term;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple0*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*
tunion_name;struct _tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];void*Cyc_Absyn_conref_def(
void*,struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct _tagged_arr Cyc_Absyn_attribute2string(
void*);struct _tuple3{void*f1;struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);int Cyc_Absynpp_is_anon_aggrtype(
void*t);extern struct _tagged_arr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*);struct _tagged_arr Cyc_Absynpp_char_escape(char);struct
_tagged_arr Cyc_Absynpp_string_escape(struct _tagged_arr);struct _tagged_arr Cyc_Absynpp_prim2str(
void*p);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple4{
struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(
struct _tuple4*arg);struct _tuple5{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*
f3;};struct _tuple5 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t);struct
_tuple6{unsigned int f1;int f2;};struct _tuple6 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[
11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[
11];struct _tuple7{void*f1;void*f2;};struct _tuple7*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple7*Cyc_Dict_rchoose(struct
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_compress(void*t);static int Cyc_Absyndump_expand_typedefs;
static int Cyc_Absyndump_qvar_to_Cids;static int Cyc_Absyndump_add_cyc_prefix;static
int Cyc_Absyndump_generate_line_directives;static int Cyc_Absyndump_to_VC;void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs){Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_set_params(fs);}void Cyc_Absyndump_dumptyp(void*);void Cyc_Absyndump_dumpntyp(
void*t);void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumpexp_prec(
int,struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);void
Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*);void Cyc_Absyndump_dumpvardecl(
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
if(Cyc_Absyndump_pos > 80){Cyc_Absyndump_pos=(unsigned int)sz;Cyc_fputc((int)'\n',*((
struct Cyc___cycFILE**)Cyc_Absyndump_dump_file));}else{if(Cyc_Absyndump_need_space_before())
Cyc_fputc((int)' ',*((struct Cyc___cycFILE**)Cyc_Absyndump_dump_file));}if(sz >= 1){
Cyc_Absyndump_prev_char=*((const char*)_check_unknown_subscript(s,sizeof(char),sz
- 1));Cyc_file_string_write(*((struct Cyc___cycFILE**)Cyc_Absyndump_dump_file),s,
0,sz);}}void Cyc_Absyndump_dump_nospace(struct _tagged_arr s){int sz=(int)Cyc_strlen((
struct _tagged_arr)s);Cyc_Absyndump_pos +=sz;if(sz >= 1){Cyc_file_string_write(*((
struct Cyc___cycFILE**)Cyc_Absyndump_dump_file),s,0,sz);Cyc_Absyndump_prev_char=*((
const char*)_check_unknown_subscript(s,sizeof(char),sz - 1));}}void Cyc_Absyndump_dump_char(
int c){++ Cyc_Absyndump_pos;Cyc_fputc(c,*((struct Cyc___cycFILE**)Cyc_Absyndump_dump_file));
Cyc_Absyndump_prev_char=(char)c;}void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*
loc){if(loc == 0)return;if(!Cyc_Absyndump_generate_line_directives)return;{struct
_tagged_arr _tmp0=Cyc_Position_get_line_directive(loc);Cyc_Absyndump_dump(_tmp0);}}
void Cyc_Absyndump_dump_str(struct _tagged_arr*s){Cyc_Absyndump_dump(*((struct
_tagged_arr*)s));}void Cyc_Absyndump_dump_semi(){Cyc_Absyndump_dump_char((int)';');}
void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,struct
_tagged_arr sep){if(l == 0)return;for(0;l->tl != 0;l=l->tl){f((void*)l->hd);Cyc_Absyndump_dump_nospace(
sep);}f((void*)l->hd);}void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*
env,struct Cyc_List_List*l,struct _tagged_arr sep){if(l == 0)return;for(0;l->tl != 0;
l=l->tl){f(env,(void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f(env,(void*)l->hd);}
void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _tagged_arr
start,struct _tagged_arr end,struct _tagged_arr sep){Cyc_Absyndump_dump_nospace(
start);Cyc_Absyndump_dump_sep(f,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_group_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep){Cyc_Absyndump_dump_nospace(start);Cyc_Absyndump_dump_sep_c(
f,env,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_egroup(void(*f)(
void*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep){if(l != 0)Cyc_Absyndump_group(f,l,start,end,sep);}void Cyc_Absyndump_dumpqvar(
struct _tuple0*v){struct Cyc_List_List*_tmp1=0;{void*_tmp2=(*((struct _tuple0*)v)).f1;
struct Cyc_List_List*_tmp3;struct Cyc_List_List*_tmp4;_LL11: if((int)_tmp2 != 0)goto
_LL13;_LL12: _tmp3=0;goto _LL14;_LL13: if(_tmp2 <= (void*)1?1:*((int*)_tmp2)!= 0)
goto _LL15;_tmp3=((struct Cyc_Absyn_Rel_n_struct*)_tmp2)->f1;_LL14: _tmp1=_tmp3;
goto _LL10;_LL15: if(_tmp2 <= (void*)1?1:*((int*)_tmp2)!= 1)goto _LL10;_tmp4=((
struct Cyc_Absyn_Abs_n_struct*)_tmp2)->f1;_LL16: _tmp1=(Cyc_Absyndump_qvar_to_Cids?
Cyc_Absyndump_add_cyc_prefix: 0)?({struct Cyc_List_List*_tmp5=_cycalloc(sizeof(*
_tmp5));_tmp5->hd=Cyc_Absynpp_cyc_stringptr;_tmp5->tl=_tmp4;_tmp5;}): _tmp4;goto
_LL10;_LL10:;}if(_tmp1 != 0){Cyc_Absyndump_dump_str((struct _tagged_arr*)_tmp1->hd);
for(_tmp1=_tmp1->tl;_tmp1 != 0;_tmp1=_tmp1->tl){if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((
int)'_');else{Cyc_Absyndump_dump_nospace(({const char*_tmp6="::";_tag_arr(_tmp6,
sizeof(char),_get_zero_arr_size(_tmp6,3));}));}Cyc_Absyndump_dump_nospace(*((
struct _tagged_arr*)((struct _tagged_arr*)_tmp1->hd)));}if(Cyc_Absyndump_qvar_to_Cids)
Cyc_Absyndump_dump_nospace(({const char*_tmp7="_";_tag_arr(_tmp7,sizeof(char),
_get_zero_arr_size(_tmp7,2));}));else{Cyc_Absyndump_dump_nospace(({const char*
_tmp8="::";_tag_arr(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,3));}));}Cyc_Absyndump_dump_nospace(*((
struct _tagged_arr*)(*((struct _tuple0*)v)).f2));}else{Cyc_Absyndump_dump_str((*((
struct _tuple0*)v)).f2);}}void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict)
Cyc_Absyndump_dump(({const char*_tmp9="restrict";_tag_arr(_tmp9,sizeof(char),
_get_zero_arr_size(_tmp9,9));}));if(tq.q_volatile)Cyc_Absyndump_dump(({const char*
_tmpA="volatile";_tag_arr(_tmpA,sizeof(char),_get_zero_arr_size(_tmpA,9));}));
if(tq.q_const)Cyc_Absyndump_dump(({const char*_tmpB="const";_tag_arr(_tmpB,
sizeof(char),_get_zero_arr_size(_tmpB,6));}));}void Cyc_Absyndump_dumpscope(void*
sc){void*_tmpC=sc;_LL18: if((int)_tmpC != 0)goto _LL1A;_LL19: Cyc_Absyndump_dump(({
const char*_tmpD="static";_tag_arr(_tmpD,sizeof(char),_get_zero_arr_size(_tmpD,7));}));
return;_LL1A: if((int)_tmpC != 2)goto _LL1C;_LL1B: return;_LL1C: if((int)_tmpC != 3)
goto _LL1E;_LL1D: Cyc_Absyndump_dump(({const char*_tmpE="extern";_tag_arr(_tmpE,
sizeof(char),_get_zero_arr_size(_tmpE,7));}));return;_LL1E: if((int)_tmpC != 4)
goto _LL20;_LL1F: Cyc_Absyndump_dump(({const char*_tmpF="extern \"C\"";_tag_arr(
_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,11));}));return;_LL20: if((int)_tmpC
!= 1)goto _LL22;_LL21: Cyc_Absyndump_dump(({const char*_tmp10="abstract";_tag_arr(
_tmp10,sizeof(char),_get_zero_arr_size(_tmp10,9));}));return;_LL22: if((int)_tmpC
!= 5)goto _LL17;_LL23: Cyc_Absyndump_dump(({const char*_tmp11="register";_tag_arr(
_tmp11,sizeof(char),_get_zero_arr_size(_tmp11,9));}));return;_LL17:;}void Cyc_Absyndump_dumpkind(
void*k){void*_tmp12=k;_LL25: if((int)_tmp12 != 0)goto _LL27;_LL26: Cyc_Absyndump_dump(({
const char*_tmp13="A";_tag_arr(_tmp13,sizeof(char),_get_zero_arr_size(_tmp13,2));}));
return;_LL27: if((int)_tmp12 != 1)goto _LL29;_LL28: Cyc_Absyndump_dump(({const char*
_tmp14="M";_tag_arr(_tmp14,sizeof(char),_get_zero_arr_size(_tmp14,2));}));
return;_LL29: if((int)_tmp12 != 2)goto _LL2B;_LL2A: Cyc_Absyndump_dump(({const char*
_tmp15="B";_tag_arr(_tmp15,sizeof(char),_get_zero_arr_size(_tmp15,2));}));
return;_LL2B: if((int)_tmp12 != 3)goto _LL2D;_LL2C: Cyc_Absyndump_dump(({const char*
_tmp16="R";_tag_arr(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,2));}));
return;_LL2D: if((int)_tmp12 != 4)goto _LL2F;_LL2E: Cyc_Absyndump_dump(({const char*
_tmp17="E";_tag_arr(_tmp17,sizeof(char),_get_zero_arr_size(_tmp17,2));}));
return;_LL2F: if((int)_tmp12 != 5)goto _LL24;_LL30: Cyc_Absyndump_dump(({const char*
_tmp18="I";_tag_arr(_tmp18,sizeof(char),_get_zero_arr_size(_tmp18,2));}));
return;_LL24:;}void Cyc_Absyndump_dumpaggr_kind(void*k){void*_tmp19=k;_LL32: if((
int)_tmp19 != 0)goto _LL34;_LL33: Cyc_Absyndump_dump(({const char*_tmp1A="struct";
_tag_arr(_tmp1A,sizeof(char),_get_zero_arr_size(_tmp1A,7));}));return;_LL34: if((
int)_tmp19 != 1)goto _LL31;_LL35: Cyc_Absyndump_dump(({const char*_tmp1B="union";
_tag_arr(_tmp1B,sizeof(char),_get_zero_arr_size(_tmp1B,6));}));return;_LL31:;}
void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){Cyc_Absyndump_egroup(Cyc_Absyndump_dumptyp,
ts,({const char*_tmp1C="<";_tag_arr(_tmp1C,sizeof(char),_get_zero_arr_size(_tmp1C,
2));}),({const char*_tmp1D=">";_tag_arr(_tmp1D,sizeof(char),_get_zero_arr_size(
_tmp1D,2));}),({const char*_tmp1E=",";_tag_arr(_tmp1E,sizeof(char),
_get_zero_arr_size(_tmp1E,2));}));}void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*
tv){Cyc_Absyndump_dump_str(tv->name);}void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*
tv){Cyc_Absyndump_dump_str(tv->name);{void*_tmp1F=Cyc_Absyn_compress_kb((void*)
tv->kind);void*_tmp20;void*_tmp21;_LL37: if(*((int*)_tmp1F)!= 0)goto _LL39;_tmp20=(
void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp1F)->f1;if((int)_tmp20 != 2)goto _LL39;
_LL38: goto _LL3A;_LL39: if(*((int*)_tmp1F)!= 2)goto _LL3B;_LL3A: goto _LL3C;_LL3B: if(*((
int*)_tmp1F)!= 1)goto _LL3D;_LL3C: Cyc_Absyndump_dump(({const char*_tmp22="::?";
_tag_arr(_tmp22,sizeof(char),_get_zero_arr_size(_tmp22,4));}));goto _LL36;_LL3D:
if(*((int*)_tmp1F)!= 0)goto _LL36;_tmp21=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
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
void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*((struct _tuple8*)pr)).f1,(*((
struct _tuple8*)pr)).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(
struct Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumparg,ts,({const char*_tmp2A="(";_tag_arr(_tmp2A,sizeof(char),
_get_zero_arr_size(_tmp2A,2));}),({const char*_tmp2B=")";_tag_arr(_tmp2B,sizeof(
char),_get_zero_arr_size(_tmp2B,2));}),({const char*_tmp2C=",";_tag_arr(_tmp2C,
sizeof(char),_get_zero_arr_size(_tmp2C,2));}));}void Cyc_Absyndump_dump_callconv(
struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmp2D=(void*)atts->hd;
_LL40: if((int)_tmp2D != 0)goto _LL42;_LL41: Cyc_Absyndump_dump(({const char*_tmp2E="_stdcall";
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
t);void*_tmp3B;struct Cyc_List_List*_tmp3C;_LL65: if(_tmp3A <= (void*)3?1:*((int*)
_tmp3A)!= 19)goto _LL67;_tmp3B=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp3A)->f1;
_LL66: rgions=({struct Cyc_List_List*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->hd=(
void*)_tmp3B;_tmp3D->tl=rgions;_tmp3D;});goto _LL64;_LL67: if(_tmp3A <= (void*)3?1:*((
int*)_tmp3A)!= 20)goto _LL69;_tmp3C=((struct Cyc_Absyn_JoinEff_struct*)_tmp3A)->f1;
_LL68: for(0;_tmp3C != 0;_tmp3C=_tmp3C->tl){struct Cyc_List_List*_tmp3F;struct Cyc_List_List*
_tmp40;struct _tuple9 _tmp3E=Cyc_Absyndump_effects_split((void*)_tmp3C->hd);_tmp3F=
_tmp3E.f1;_tmp40=_tmp3E.f2;rgions=Cyc_List_imp_append(_tmp3F,rgions);effects=Cyc_List_imp_append(
_tmp40,effects);}goto _LL64;_LL69:;_LL6A: effects=({struct Cyc_List_List*_tmp41=
_cycalloc(sizeof(*_tmp41));_tmp41->hd=(void*)t;_tmp41->tl=effects;_tmp41;});goto
_LL64;_LL64:;}return({struct _tuple9 _tmp42;_tmp42.f1=rgions;_tmp42.f2=effects;
_tmp42;});}static void Cyc_Absyndump_dumpeff(void*t){struct Cyc_List_List*_tmp44;
struct Cyc_List_List*_tmp45;struct _tuple9 _tmp43=Cyc_Absyndump_effects_split(t);
_tmp44=_tmp43.f1;_tmp45=_tmp43.f2;_tmp44=Cyc_List_imp_rev(_tmp44);_tmp45=Cyc_List_imp_rev(
_tmp45);for(0;_tmp45 != 0;_tmp45=_tmp45->tl){Cyc_Absyndump_dumpntyp((void*)_tmp45->hd);
Cyc_Absyndump_dump_char((int)'+');}Cyc_Absyndump_dump_char((int)'{');for(0;
_tmp44 != 0;_tmp44=_tmp44->tl){Cyc_Absyndump_dumprgn((void*)_tmp44->hd);if(_tmp44->tl
!= 0)Cyc_Absyndump_dump_char((int)',');}Cyc_Absyndump_dump_char((int)'}');}void
Cyc_Absyndump_dumpntyp(void*t){void*_tmp46=t;struct Cyc_Absyn_Tvar*_tmp47;struct
Cyc_Core_Opt*_tmp48;struct Cyc_Core_Opt*_tmp49;int _tmp4A;struct Cyc_Core_Opt*
_tmp4B;struct Cyc_Core_Opt*_tmp4C;struct Cyc_Core_Opt _tmp4D;void*_tmp4E;int _tmp4F;
struct Cyc_Absyn_TunionInfo _tmp50;void*_tmp51;struct Cyc_List_List*_tmp52;void*
_tmp53;struct Cyc_Absyn_TunionFieldInfo _tmp54;void*_tmp55;struct Cyc_List_List*
_tmp56;void*_tmp57;void*_tmp58;void*_tmp59;void*_tmp5A;void*_tmp5B;void*_tmp5C;
void*_tmp5D;void*_tmp5E;void*_tmp5F;void*_tmp60;void*_tmp61;void*_tmp62;void*
_tmp63;void*_tmp64;void*_tmp65;void*_tmp66;void*_tmp67;void*_tmp68;void*_tmp69;
void*_tmp6A;void*_tmp6B;void*_tmp6C;void*_tmp6D;void*_tmp6E;void*_tmp6F;void*
_tmp70;void*_tmp71;void*_tmp72;void*_tmp73;void*_tmp74;int _tmp75;struct Cyc_List_List*
_tmp76;struct Cyc_Absyn_AggrInfo _tmp77;void*_tmp78;struct Cyc_List_List*_tmp79;
void*_tmp7A;struct Cyc_List_List*_tmp7B;struct _tuple0*_tmp7C;struct Cyc_List_List*
_tmp7D;struct _tuple0*_tmp7E;struct Cyc_List_List*_tmp7F;struct Cyc_Core_Opt*_tmp80;
void*_tmp81;void*_tmp82;int _tmp83;void*_tmp84;_LL6C: if(_tmp46 <= (void*)3?1:*((
int*)_tmp46)!= 7)goto _LL6E;_LL6D: goto _LL6F;_LL6E: if(_tmp46 <= (void*)3?1:*((int*)
_tmp46)!= 8)goto _LL70;_LL6F: goto _LL71;_LL70: if(_tmp46 <= (void*)3?1:*((int*)
_tmp46)!= 4)goto _LL72;_LL71: return;_LL72: if((int)_tmp46 != 0)goto _LL74;_LL73: Cyc_Absyndump_dump(({
const char*_tmp85="void";_tag_arr(_tmp85,sizeof(char),_get_zero_arr_size(_tmp85,5));}));
return;_LL74: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 1)goto _LL76;_tmp47=((
struct Cyc_Absyn_VarType_struct*)_tmp46)->f1;_LL75: Cyc_Absyndump_dump_str(_tmp47->name);
return;_LL76: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 0)goto _LL78;_tmp48=((
struct Cyc_Absyn_Evar_struct*)_tmp46)->f1;_tmp49=((struct Cyc_Absyn_Evar_struct*)
_tmp46)->f2;if(_tmp49 != 0)goto _LL78;_tmp4A=((struct Cyc_Absyn_Evar_struct*)_tmp46)->f3;
_LL77: Cyc_Absyndump_dump(({const char*_tmp86="%";_tag_arr(_tmp86,sizeof(char),
_get_zero_arr_size(_tmp86,2));}));if(_tmp48 == 0)Cyc_Absyndump_dump(({const char*
_tmp87="?";_tag_arr(_tmp87,sizeof(char),_get_zero_arr_size(_tmp87,2));}));else{
Cyc_Absyndump_dumpkind((void*)_tmp48->v);}Cyc_Absyndump_dump((struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp8A;_tmp8A.tag=1;_tmp8A.f1=(unsigned int)_tmp4A;{void*
_tmp88[1]={& _tmp8A};Cyc_aprintf(({const char*_tmp89="(%d)";_tag_arr(_tmp89,
sizeof(char),_get_zero_arr_size(_tmp89,5));}),_tag_arr(_tmp88,sizeof(void*),1));}}));
return;_LL78: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 0)goto _LL7A;_tmp4B=((
struct Cyc_Absyn_Evar_struct*)_tmp46)->f1;_tmp4C=((struct Cyc_Absyn_Evar_struct*)
_tmp46)->f2;if(_tmp4C == 0)goto _LL7A;_tmp4D=*_tmp4C;_tmp4E=(void*)_tmp4D.v;_tmp4F=((
struct Cyc_Absyn_Evar_struct*)_tmp46)->f3;_LL79: Cyc_Absyndump_dumpntyp(_tmp4E);
return;_LL7A: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 2)goto _LL7C;_tmp50=((
struct Cyc_Absyn_TunionType_struct*)_tmp46)->f1;_tmp51=(void*)_tmp50.tunion_info;
_tmp52=_tmp50.targs;_tmp53=(void*)_tmp50.rgn;_LL7B:{void*_tmp8B=_tmp51;struct Cyc_Absyn_UnknownTunionInfo
_tmp8C;struct _tuple0*_tmp8D;int _tmp8E;struct Cyc_Absyn_Tuniondecl**_tmp8F;struct
Cyc_Absyn_Tuniondecl*_tmp90;struct Cyc_Absyn_Tuniondecl _tmp91;struct _tuple0*
_tmp92;int _tmp93;_LLBD: if(*((int*)_tmp8B)!= 0)goto _LLBF;_tmp8C=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp8B)->f1;_tmp8D=_tmp8C.name;_tmp8E=_tmp8C.is_xtunion;_LLBE: _tmp92=_tmp8D;
_tmp93=_tmp8E;goto _LLC0;_LLBF: if(*((int*)_tmp8B)!= 1)goto _LLBC;_tmp8F=((struct
Cyc_Absyn_KnownTunion_struct*)_tmp8B)->f1;_tmp90=*_tmp8F;_tmp91=*_tmp90;_tmp92=
_tmp91.name;_tmp93=_tmp91.is_xtunion;_LLC0: if(_tmp93)Cyc_Absyndump_dump(({const
char*_tmp94="xtunion ";_tag_arr(_tmp94,sizeof(char),_get_zero_arr_size(_tmp94,9));}));
else{Cyc_Absyndump_dump(({const char*_tmp95="tunion ";_tag_arr(_tmp95,sizeof(char),
_get_zero_arr_size(_tmp95,8));}));}{void*_tmp96=Cyc_Tcutil_compress(_tmp53);
_LLC2: if((int)_tmp96 != 2)goto _LLC4;_LLC3: goto _LLC1;_LLC4:;_LLC5: Cyc_Absyndump_dumptyp(
_tmp53);Cyc_Absyndump_dump(({const char*_tmp97=" ";_tag_arr(_tmp97,sizeof(char),
_get_zero_arr_size(_tmp97,2));}));goto _LLC1;_LLC1:;}Cyc_Absyndump_dumpqvar(
_tmp92);Cyc_Absyndump_dumptps(_tmp52);goto _LLBC;_LLBC:;}goto _LL6B;_LL7C: if(
_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 3)goto _LL7E;_tmp54=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp46)->f1;_tmp55=(void*)_tmp54.field_info;_tmp56=_tmp54.targs;_LL7D:{void*
_tmp98=_tmp55;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp99;struct _tuple0*_tmp9A;
struct _tuple0*_tmp9B;int _tmp9C;struct Cyc_Absyn_Tuniondecl*_tmp9D;struct Cyc_Absyn_Tuniondecl
_tmp9E;struct _tuple0*_tmp9F;int _tmpA0;struct Cyc_Absyn_Tunionfield*_tmpA1;struct
Cyc_Absyn_Tunionfield _tmpA2;struct _tuple0*_tmpA3;_LLC7: if(*((int*)_tmp98)!= 0)
goto _LLC9;_tmp99=((struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp98)->f1;_tmp9A=
_tmp99.tunion_name;_tmp9B=_tmp99.field_name;_tmp9C=_tmp99.is_xtunion;_LLC8:
_tmp9F=_tmp9A;_tmpA0=_tmp9C;_tmpA3=_tmp9B;goto _LLCA;_LLC9: if(*((int*)_tmp98)!= 1)
goto _LLC6;_tmp9D=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp98)->f1;_tmp9E=*
_tmp9D;_tmp9F=_tmp9E.name;_tmpA0=_tmp9E.is_xtunion;_tmpA1=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp98)->f2;_tmpA2=*_tmpA1;_tmpA3=_tmpA2.name;_LLCA: if(_tmpA0)Cyc_Absyndump_dump(({
const char*_tmpA4="xtunion ";_tag_arr(_tmpA4,sizeof(char),_get_zero_arr_size(
_tmpA4,9));}));else{Cyc_Absyndump_dump(({const char*_tmpA5="tunion ";_tag_arr(
_tmpA5,sizeof(char),_get_zero_arr_size(_tmpA5,8));}));}Cyc_Absyndump_dumpqvar(
_tmp9F);Cyc_Absyndump_dump(({const char*_tmpA6=".";_tag_arr(_tmpA6,sizeof(char),
_get_zero_arr_size(_tmpA6,2));}));Cyc_Absyndump_dumpqvar(_tmpA3);Cyc_Absyndump_dumptps(
_tmp56);goto _LLC6;_LLC6:;}goto _LL6B;_LL7E: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)
!= 5)goto _LL80;_tmp57=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((
int)_tmp57 != 2)goto _LL80;_tmp58=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;
if((int)_tmp58 != 2)goto _LL80;_LL7F: goto _LL81;_LL80: if(_tmp46 <= (void*)3?1:*((int*)
_tmp46)!= 5)goto _LL82;_tmp59=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;
if((int)_tmp59 != 0)goto _LL82;_tmp5A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp5A != 2)goto _LL82;_LL81: Cyc_Absyndump_dump(({const char*
_tmpA7="int";_tag_arr(_tmpA7,sizeof(char),_get_zero_arr_size(_tmpA7,4));}));
return;_LL82: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 5)goto _LL84;_tmp5B=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp5B != 2)goto _LL84;_tmp5C=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)_tmp5C != 0)goto _LL84;
_LL83: Cyc_Absyndump_dump(({const char*_tmpA8="char";_tag_arr(_tmpA8,sizeof(char),
_get_zero_arr_size(_tmpA8,5));}));return;_LL84: if(_tmp46 <= (void*)3?1:*((int*)
_tmp46)!= 5)goto _LL86;_tmp5D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;
if((int)_tmp5D != 0)goto _LL86;_tmp5E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp5E != 0)goto _LL86;_LL85: Cyc_Absyndump_dump(({const char*
_tmpA9="signed char";_tag_arr(_tmpA9,sizeof(char),_get_zero_arr_size(_tmpA9,12));}));
return;_LL86: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 5)goto _LL88;_tmp5F=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp5F != 1)goto _LL88;_tmp60=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)_tmp60 != 0)goto _LL88;
_LL87: Cyc_Absyndump_dump(({const char*_tmpAA="unsigned char";_tag_arr(_tmpAA,
sizeof(char),_get_zero_arr_size(_tmpAA,14));}));return;_LL88: if(_tmp46 <= (void*)
3?1:*((int*)_tmp46)!= 5)goto _LL8A;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp61 != 2)goto _LL8A;_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp62 != 1)goto _LL8A;_LL89: goto _LL8B;_LL8A: if(_tmp46 <= (void*)
3?1:*((int*)_tmp46)!= 5)goto _LL8C;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp63 != 0)goto _LL8C;_tmp64=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp64 != 1)goto _LL8C;_LL8B: Cyc_Absyndump_dump(({const char*
_tmpAB="short";_tag_arr(_tmpAB,sizeof(char),_get_zero_arr_size(_tmpAB,6));}));
return;_LL8C: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 5)goto _LL8E;_tmp65=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp65 != 1)goto _LL8E;_tmp66=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)_tmp66 != 1)goto _LL8E;
_LL8D: Cyc_Absyndump_dump(({const char*_tmpAC="unsigned short";_tag_arr(_tmpAC,
sizeof(char),_get_zero_arr_size(_tmpAC,15));}));return;_LL8E: if(_tmp46 <= (void*)
3?1:*((int*)_tmp46)!= 5)goto _LL90;_tmp67=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp67 != 1)goto _LL90;_tmp68=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp68 != 2)goto _LL90;_LL8F: Cyc_Absyndump_dump(({const char*
_tmpAD="unsigned int";_tag_arr(_tmpAD,sizeof(char),_get_zero_arr_size(_tmpAD,13));}));
return;_LL90: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 5)goto _LL92;_tmp69=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp69 != 2)goto _LL92;_tmp6A=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)_tmp6A != 3)goto _LL92;
if(!Cyc_Absyndump_to_VC)goto _LL92;_LL91: goto _LL93;_LL92: if(_tmp46 <= (void*)3?1:*((
int*)_tmp46)!= 5)goto _LL94;_tmp6B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp6B != 0)goto _LL94;_tmp6C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp6C != 3)goto _LL94;if(!Cyc_Absyndump_to_VC)goto _LL94;_LL93:
Cyc_Absyndump_dump(({const char*_tmpAE="__int64";_tag_arr(_tmpAE,sizeof(char),
_get_zero_arr_size(_tmpAE,8));}));return;_LL94: if(_tmp46 <= (void*)3?1:*((int*)
_tmp46)!= 5)goto _LL96;_tmp6D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;
if((int)_tmp6D != 2)goto _LL96;_tmp6E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp6E != 3)goto _LL96;_LL95: goto _LL97;_LL96: if(_tmp46 <= (void*)
3?1:*((int*)_tmp46)!= 5)goto _LL98;_tmp6F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp6F != 0)goto _LL98;_tmp70=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp70 != 3)goto _LL98;_LL97: Cyc_Absyndump_dump(({const char*
_tmpAF="long long";_tag_arr(_tmpAF,sizeof(char),_get_zero_arr_size(_tmpAF,10));}));
return;_LL98: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 5)goto _LL9A;_tmp71=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp71 != 1)goto _LL9A;_tmp72=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)_tmp72 != 3)goto _LL9A;
if(!Cyc_Absyndump_to_VC)goto _LL9A;_LL99: Cyc_Absyndump_dump(({const char*_tmpB0="unsigned __int64";
_tag_arr(_tmpB0,sizeof(char),_get_zero_arr_size(_tmpB0,17));}));return;_LL9A: if(
_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 5)goto _LL9C;_tmp73=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp73 != 1)goto _LL9C;_tmp74=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp74 != 3)goto _LL9C;_LL9B: Cyc_Absyndump_dump(({const char*
_tmpB1="unsigned long long";_tag_arr(_tmpB1,sizeof(char),_get_zero_arr_size(
_tmpB1,19));}));return;_LL9C: if((int)_tmp46 != 1)goto _LL9E;_LL9D: Cyc_Absyndump_dump(({
const char*_tmpB2="float";_tag_arr(_tmpB2,sizeof(char),_get_zero_arr_size(_tmpB2,
6));}));return;_LL9E: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 6)goto _LLA0;_tmp75=((
struct Cyc_Absyn_DoubleType_struct*)_tmp46)->f1;_LL9F: if(_tmp75)Cyc_Absyndump_dump(({
const char*_tmpB3="long double";_tag_arr(_tmpB3,sizeof(char),_get_zero_arr_size(
_tmpB3,12));}));else{Cyc_Absyndump_dump(({const char*_tmpB4="double";_tag_arr(
_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,7));}));}return;_LLA0: if(_tmp46 <= (
void*)3?1:*((int*)_tmp46)!= 9)goto _LLA2;_tmp76=((struct Cyc_Absyn_TupleType_struct*)
_tmp46)->f1;_LLA1: Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(
_tmp76);return;_LLA2: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 10)goto _LLA4;
_tmp77=((struct Cyc_Absyn_AggrType_struct*)_tmp46)->f1;_tmp78=(void*)_tmp77.aggr_info;
_tmp79=_tmp77.targs;_LLA3: {void*_tmpB6;struct _tuple0*_tmpB7;struct _tuple3 _tmpB5=
Cyc_Absyn_aggr_kinded_name(_tmp78);_tmpB6=_tmpB5.f1;_tmpB7=_tmpB5.f2;Cyc_Absyndump_dumpaggr_kind(
_tmpB6);Cyc_Absyndump_dumpqvar(_tmpB7);Cyc_Absyndump_dumptps(_tmp79);return;}
_LLA4: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 11)goto _LLA6;_tmp7A=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp46)->f1;_tmp7B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp46)->f2;_LLA5: Cyc_Absyndump_dumpaggr_kind(_tmp7A);Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpaggrfields(_tmp7B);Cyc_Absyndump_dump_char((int)'}');
return;_LLA6: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 12)goto _LLA8;_tmp7C=((
struct Cyc_Absyn_EnumType_struct*)_tmp46)->f1;_LLA7: Cyc_Absyndump_dump(({const
char*_tmpB8="enum ";_tag_arr(_tmpB8,sizeof(char),_get_zero_arr_size(_tmpB8,6));}));
Cyc_Absyndump_dumpqvar(_tmp7C);return;_LLA8: if(_tmp46 <= (void*)3?1:*((int*)
_tmp46)!= 13)goto _LLAA;_tmp7D=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp46)->f1;
_LLA9: Cyc_Absyndump_dump(({const char*_tmpB9="enum {";_tag_arr(_tmpB9,sizeof(char),
_get_zero_arr_size(_tmpB9,7));}));Cyc_Absyndump_dumpenumfields(_tmp7D);Cyc_Absyndump_dump(({
const char*_tmpBA="}";_tag_arr(_tmpBA,sizeof(char),_get_zero_arr_size(_tmpBA,2));}));
return;_LLAA: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 16)goto _LLAC;_tmp7E=((
struct Cyc_Absyn_TypedefType_struct*)_tmp46)->f1;_tmp7F=((struct Cyc_Absyn_TypedefType_struct*)
_tmp46)->f2;_tmp80=((struct Cyc_Absyn_TypedefType_struct*)_tmp46)->f4;_LLAB:(Cyc_Absyndump_dumpqvar(
_tmp7E),Cyc_Absyndump_dumptps(_tmp7F));return;_LLAC: if(_tmp46 <= (void*)3?1:*((
int*)_tmp46)!= 14)goto _LLAE;_tmp81=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp46)->f1;_LLAD: Cyc_Absyndump_dump(({const char*_tmpBB="sizeof_t<";_tag_arr(
_tmpBB,sizeof(char),_get_zero_arr_size(_tmpBB,10));}));Cyc_Absyndump_dumpntyp(
_tmp81);Cyc_Absyndump_dump(({const char*_tmpBC=">";_tag_arr(_tmpBC,sizeof(char),
_get_zero_arr_size(_tmpBC,2));}));return;_LLAE: if(_tmp46 <= (void*)3?1:*((int*)
_tmp46)!= 15)goto _LLB0;_tmp82=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp46)->f1;_LLAF: Cyc_Absyndump_dumprgn(_tmp82);return;_LLB0: if(_tmp46 <= (void*)
3?1:*((int*)_tmp46)!= 18)goto _LLB2;_tmp83=((struct Cyc_Absyn_TypeInt_struct*)
_tmp46)->f1;_LLB1: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmpBF;_tmpBF.tag=1;_tmpBF.f1=(unsigned int)_tmp83;{void*_tmpBD[1]={& _tmpBF};Cyc_aprintf(({
const char*_tmpBE="`%d";_tag_arr(_tmpBE,sizeof(char),_get_zero_arr_size(_tmpBE,4));}),
_tag_arr(_tmpBD,sizeof(void*),1));}}));return;_LLB2: if(_tmp46 <= (void*)3?1:*((
int*)_tmp46)!= 17)goto _LLB4;_tmp84=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp46)->f1;_LLB3: Cyc_Absyndump_dump(({const char*_tmpC0="tag_t<";_tag_arr(_tmpC0,
sizeof(char),_get_zero_arr_size(_tmpC0,7));}));Cyc_Absyndump_dumpntyp(_tmp84);
Cyc_Absyndump_dump(({const char*_tmpC1=">";_tag_arr(_tmpC1,sizeof(char),
_get_zero_arr_size(_tmpC1,2));}));return;_LLB4: if((int)_tmp46 != 2)goto _LLB6;
_LLB5: goto _LLB7;_LLB6: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 19)goto _LLB8;
_LLB7: goto _LLB9;_LLB8: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 21)goto _LLBA;
_LLB9: goto _LLBB;_LLBA: if(_tmp46 <= (void*)3?1:*((int*)_tmp46)!= 20)goto _LL6B;
_LLBB: return;_LL6B:;}void Cyc_Absyndump_dumpvaropt(struct Cyc_Core_Opt*vo){if(vo != 
0)Cyc_Absyndump_dump_str((struct _tagged_arr*)vo->v);}void Cyc_Absyndump_dumpfunarg(
struct _tuple1*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),
struct Cyc_Core_Opt*))Cyc_Absyndump_dumptqtd)((*((struct _tuple1*)t)).f2,(*((
struct _tuple1*)t)).f3,Cyc_Absyndump_dumpvaropt,(*((struct _tuple1*)t)).f1);}void
Cyc_Absyndump_dump_rgncmp(struct _tuple7*cmp){struct _tuple7 _tmpC3;void*_tmpC4;
void*_tmpC5;struct _tuple7*_tmpC2=cmp;_tmpC3=*_tmpC2;_tmpC4=_tmpC3.f1;_tmpC5=
_tmpC3.f2;Cyc_Absyndump_dumpeff(_tmpC4);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(
_tmpC5);}void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){((void(*)(void(*
f)(struct _tuple7*),struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dump_rgncmp,rgn_po,({const char*_tmpC6=",";_tag_arr(_tmpC6,sizeof(
char),_get_zero_arr_size(_tmpC6,2));}));}void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((int)'(');for(0;args
!= 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple1*)args->hd);if((args->tl
!= 0?1: c_varargs)?1: cyc_varargs != 0)Cyc_Absyndump_dump_char((int)',');}if(
c_varargs)Cyc_Absyndump_dump(({const char*_tmpC7="...";_tag_arr(_tmpC7,sizeof(
char),_get_zero_arr_size(_tmpC7,4));}));else{if(cyc_varargs != 0){struct _tuple1*
_tmpC8=({struct _tuple1*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->f1=cyc_varargs->name;
_tmpCB->f2=cyc_varargs->tq;_tmpCB->f3=(void*)cyc_varargs->type;_tmpCB;});Cyc_Absyndump_dump(({
const char*_tmpC9="...";_tag_arr(_tmpC9,sizeof(char),_get_zero_arr_size(_tmpC9,4));}));
if(cyc_varargs->inject)Cyc_Absyndump_dump(({const char*_tmpCA=" inject ";_tag_arr(
_tmpCA,sizeof(char),_get_zero_arr_size(_tmpCA,9));}));Cyc_Absyndump_dumpfunarg(
_tmpC8);}}if(effopt != 0){Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpeff((void*)
effopt->v);}if(rgn_po != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(
rgn_po);}Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumptyp(void*t){((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _tmpCC;_tmpCC.q_const=0;_tmpCC.q_volatile=0;_tmpCC.q_restrict=
0;_tmpCC;}),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpdesignator(
void*d){void*_tmpCD=d;struct Cyc_Absyn_Exp*_tmpCE;struct _tagged_arr*_tmpCF;_LLCC:
if(*((int*)_tmpCD)!= 0)goto _LLCE;_tmpCE=((struct Cyc_Absyn_ArrayElement_struct*)
_tmpCD)->f1;_LLCD: Cyc_Absyndump_dump(({const char*_tmpD0=".[";_tag_arr(_tmpD0,
sizeof(char),_get_zero_arr_size(_tmpD0,3));}));Cyc_Absyndump_dumpexp(_tmpCE);Cyc_Absyndump_dump_char((
int)']');goto _LLCB;_LLCE: if(*((int*)_tmpCD)!= 1)goto _LLCB;_tmpCF=((struct Cyc_Absyn_FieldName_struct*)
_tmpCD)->f1;_LLCF: Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*((
struct _tagged_arr*)_tmpCF));goto _LLCB;_LLCB:;}struct _tuple10{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};void Cyc_Absyndump_dumpde(struct _tuple10*de){Cyc_Absyndump_egroup(
Cyc_Absyndump_dumpdesignator,(*((struct _tuple10*)de)).f1,({const char*_tmpD1="";
_tag_arr(_tmpD1,sizeof(char),_get_zero_arr_size(_tmpD1,1));}),({const char*_tmpD2="=";
_tag_arr(_tmpD2,sizeof(char),_get_zero_arr_size(_tmpD2,2));}),({const char*_tmpD3="=";
_tag_arr(_tmpD3,sizeof(char),_get_zero_arr_size(_tmpD3,2));}));Cyc_Absyndump_dumpexp((*((
struct _tuple10*)de)).f2);}void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*
es){((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec,inprec,es,({const char*_tmpD4="";_tag_arr(_tmpD4,
sizeof(char),_get_zero_arr_size(_tmpD4,1));}),({const char*_tmpD5="";_tag_arr(
_tmpD5,sizeof(char),_get_zero_arr_size(_tmpD5,1));}),({const char*_tmpD6=",";
_tag_arr(_tmpD6,sizeof(char),_get_zero_arr_size(_tmpD6,2));}));}void Cyc_Absyndump_dumpexp_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= 
myprec)Cyc_Absyndump_dump_nospace(({const char*_tmpD7="(";_tag_arr(_tmpD7,sizeof(
char),_get_zero_arr_size(_tmpD7,2));}));{void*_tmpD8=(void*)e->r;void*_tmpD9;
void*_tmpDA;char _tmpDB;void*_tmpDC;void*_tmpDD;short _tmpDE;void*_tmpDF;void*
_tmpE0;int _tmpE1;void*_tmpE2;void*_tmpE3;int _tmpE4;void*_tmpE5;void*_tmpE6;int
_tmpE7;void*_tmpE8;void*_tmpE9;long long _tmpEA;void*_tmpEB;struct _tagged_arr
_tmpEC;void*_tmpED;void*_tmpEE;struct _tagged_arr _tmpEF;struct _tuple0*_tmpF0;
struct _tuple0*_tmpF1;void*_tmpF2;struct Cyc_List_List*_tmpF3;struct Cyc_Absyn_Exp*
_tmpF4;struct Cyc_Core_Opt*_tmpF5;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*
_tmpF7;void*_tmpF8;struct Cyc_Absyn_Exp*_tmpF9;void*_tmpFA;struct Cyc_Absyn_Exp*
_tmpFB;void*_tmpFC;struct Cyc_Absyn_Exp*_tmpFD;void*_tmpFE;struct Cyc_Absyn_Exp*
_tmpFF;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*
_tmp102;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_List_List*
_tmp105;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_Absyn_Exp*
_tmp108;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp10A;void*_tmp10B;
struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*
_tmp10E;struct Cyc_Absyn_Exp*_tmp10F;void*_tmp110;struct Cyc_Absyn_Exp*_tmp111;
void*_tmp112;void*_tmp113;struct _tagged_arr*_tmp114;void*_tmp115;void*_tmp116;
unsigned int _tmp117;struct Cyc_List_List*_tmp118;void*_tmp119;struct Cyc_Absyn_Exp*
_tmp11A;struct Cyc_Absyn_Exp*_tmp11B;struct _tagged_arr*_tmp11C;struct Cyc_Absyn_Exp*
_tmp11D;struct _tagged_arr*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*
_tmp120;struct Cyc_List_List*_tmp121;struct _tuple1*_tmp122;struct Cyc_List_List*
_tmp123;struct Cyc_List_List*_tmp124;struct Cyc_Absyn_Vardecl*_tmp125;struct Cyc_Absyn_Exp*
_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct _tuple0*_tmp128;struct Cyc_List_List*
_tmp129;struct Cyc_List_List*_tmp12A;struct Cyc_List_List*_tmp12B;struct Cyc_List_List*
_tmp12C;struct Cyc_Absyn_Tunionfield*_tmp12D;struct _tuple0*_tmp12E;struct _tuple0*
_tmp12F;struct Cyc_Absyn_MallocInfo _tmp130;int _tmp131;struct Cyc_Absyn_Exp*_tmp132;
void**_tmp133;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Core_Opt*_tmp135;struct Cyc_List_List*
_tmp136;struct Cyc_Absyn_Stmt*_tmp137;struct Cyc_Absyn_Fndecl*_tmp138;struct Cyc_Absyn_Exp*
_tmp139;_LLD1: if(*((int*)_tmpD8)!= 0)goto _LLD3;_tmpD9=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpD8)->f1;if(_tmpD9 <= (void*)1?1:*((int*)_tmpD9)!= 0)goto _LLD3;_tmpDA=(void*)((
struct Cyc_Absyn_Char_c_struct*)_tmpD9)->f1;_tmpDB=((struct Cyc_Absyn_Char_c_struct*)
_tmpD9)->f2;_LLD2: Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape(_tmpDB));Cyc_Absyndump_dump_char((int)'\'');goto _LLD0;
_LLD3: if(*((int*)_tmpD8)!= 0)goto _LLD5;_tmpDC=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpD8)->f1;if(_tmpDC <= (void*)1?1:*((int*)_tmpDC)!= 1)goto _LLD5;_tmpDD=(void*)((
struct Cyc_Absyn_Short_c_struct*)_tmpDC)->f1;_tmpDE=((struct Cyc_Absyn_Short_c_struct*)
_tmpDC)->f2;_LLD4: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp13C;_tmp13C.tag=1;_tmp13C.f1=(unsigned int)((int)_tmpDE);{void*_tmp13A[1]={&
_tmp13C};Cyc_aprintf(({const char*_tmp13B="%d";_tag_arr(_tmp13B,sizeof(char),
_get_zero_arr_size(_tmp13B,3));}),_tag_arr(_tmp13A,sizeof(void*),1));}}));goto
_LLD0;_LLD5: if(*((int*)_tmpD8)!= 0)goto _LLD7;_tmpDF=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpD8)->f1;if(_tmpDF <= (void*)1?1:*((int*)_tmpDF)!= 2)goto _LLD7;_tmpE0=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmpDF)->f1;if((int)_tmpE0 != 2)goto _LLD7;_tmpE1=((
struct Cyc_Absyn_Int_c_struct*)_tmpDF)->f2;_LLD6: _tmpE4=_tmpE1;goto _LLD8;_LLD7:
if(*((int*)_tmpD8)!= 0)goto _LLD9;_tmpE2=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpD8)->f1;if(_tmpE2 <= (void*)1?1:*((int*)_tmpE2)!= 2)goto _LLD9;_tmpE3=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmpE2)->f1;if((int)_tmpE3 != 0)goto _LLD9;_tmpE4=((
struct Cyc_Absyn_Int_c_struct*)_tmpE2)->f2;_LLD8: Cyc_Absyndump_dump((struct
_tagged_arr)({struct Cyc_Int_pa_struct _tmp13F;_tmp13F.tag=1;_tmp13F.f1=(
unsigned int)_tmpE4;{void*_tmp13D[1]={& _tmp13F};Cyc_aprintf(({const char*_tmp13E="%d";
_tag_arr(_tmp13E,sizeof(char),_get_zero_arr_size(_tmp13E,3));}),_tag_arr(_tmp13D,
sizeof(void*),1));}}));goto _LLD0;_LLD9: if(*((int*)_tmpD8)!= 0)goto _LLDB;_tmpE5=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmpD8)->f1;if(_tmpE5 <= (void*)1?1:*((
int*)_tmpE5)!= 2)goto _LLDB;_tmpE6=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmpE5)->f1;
if((int)_tmpE6 != 1)goto _LLDB;_tmpE7=((struct Cyc_Absyn_Int_c_struct*)_tmpE5)->f2;
_LLDA: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp142;
_tmp142.tag=1;_tmp142.f1=(unsigned int)_tmpE7;{void*_tmp140[1]={& _tmp142};Cyc_aprintf(({
const char*_tmp141="%u";_tag_arr(_tmp141,sizeof(char),_get_zero_arr_size(_tmp141,
3));}),_tag_arr(_tmp140,sizeof(void*),1));}}));goto _LLD0;_LLDB: if(*((int*)_tmpD8)
!= 0)goto _LLDD;_tmpE8=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpD8)->f1;if(
_tmpE8 <= (void*)1?1:*((int*)_tmpE8)!= 3)goto _LLDD;_tmpE9=(void*)((struct Cyc_Absyn_LongLong_c_struct*)
_tmpE8)->f1;_tmpEA=((struct Cyc_Absyn_LongLong_c_struct*)_tmpE8)->f2;_LLDC: Cyc_Absyndump_dump(({
const char*_tmp143="<<FIX LONG LONG CONSTANT>>";_tag_arr(_tmp143,sizeof(char),
_get_zero_arr_size(_tmp143,27));}));goto _LLD0;_LLDD: if(*((int*)_tmpD8)!= 0)goto
_LLDF;_tmpEB=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpD8)->f1;if(_tmpEB <= (
void*)1?1:*((int*)_tmpEB)!= 4)goto _LLDF;_tmpEC=((struct Cyc_Absyn_Float_c_struct*)
_tmpEB)->f1;_LLDE: Cyc_Absyndump_dump(_tmpEC);goto _LLD0;_LLDF: if(*((int*)_tmpD8)
!= 0)goto _LLE1;_tmpED=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpD8)->f1;if((
int)_tmpED != 0)goto _LLE1;_LLE0: Cyc_Absyndump_dump(({const char*_tmp144="NULL";
_tag_arr(_tmp144,sizeof(char),_get_zero_arr_size(_tmp144,5));}));goto _LLD0;_LLE1:
if(*((int*)_tmpD8)!= 0)goto _LLE3;_tmpEE=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpD8)->f1;if(_tmpEE <= (void*)1?1:*((int*)_tmpEE)!= 5)goto _LLE3;_tmpEF=((struct
Cyc_Absyn_String_c_struct*)_tmpEE)->f1;_LLE2: Cyc_Absyndump_dump_char((int)'"');
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmpEF));Cyc_Absyndump_dump_char((
int)'"');goto _LLD0;_LLE3: if(*((int*)_tmpD8)!= 2)goto _LLE5;_tmpF0=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmpD8)->f1;_LLE4: _tmpF1=_tmpF0;goto _LLE6;_LLE5: if(*((int*)_tmpD8)!= 1)goto _LLE7;
_tmpF1=((struct Cyc_Absyn_Var_e_struct*)_tmpD8)->f1;_LLE6: Cyc_Absyndump_dumpqvar(
_tmpF1);goto _LLD0;_LLE7: if(*((int*)_tmpD8)!= 3)goto _LLE9;_tmpF2=(void*)((struct
Cyc_Absyn_Primop_e_struct*)_tmpD8)->f1;_tmpF3=((struct Cyc_Absyn_Primop_e_struct*)
_tmpD8)->f2;_LLE8: {struct _tagged_arr _tmp145=Cyc_Absynpp_prim2str(_tmpF2);
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF3)){case 1: _LL135:
if(_tmpF2 == (void*)19){Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpF3))->hd);Cyc_Absyndump_dump(({const char*
_tmp146=".size";_tag_arr(_tmp146,sizeof(char),_get_zero_arr_size(_tmp146,6));}));}
else{Cyc_Absyndump_dump(_tmp145);Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpF3))->hd);}break;case 2: _LL136: Cyc_Absyndump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpF3))->hd);
Cyc_Absyndump_dump(_tmp145);Cyc_Absyndump_dump_char((int)' ');Cyc_Absyndump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpF3->tl))->hd);
break;default: _LL137:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp147=
_cycalloc(sizeof(*_tmp147));_tmp147[0]=({struct Cyc_Core_Failure_struct _tmp148;
_tmp148.tag=Cyc_Core_Failure;_tmp148.f1=({const char*_tmp149="Absyndump -- Bad number of arguments to primop";
_tag_arr(_tmp149,sizeof(char),_get_zero_arr_size(_tmp149,47));});_tmp148;});
_tmp147;}));}goto _LLD0;}_LLE9: if(*((int*)_tmpD8)!= 4)goto _LLEB;_tmpF4=((struct
Cyc_Absyn_AssignOp_e_struct*)_tmpD8)->f1;_tmpF5=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpD8)->f2;_tmpF6=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpD8)->f3;_LLEA: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpF4);if(_tmpF5 != 0)Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((void*)
_tmpF5->v));Cyc_Absyndump_dump_nospace(({const char*_tmp14A="=";_tag_arr(_tmp14A,
sizeof(char),_get_zero_arr_size(_tmp14A,2));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmpF6);goto _LLD0;_LLEB: if(*((int*)_tmpD8)!= 5)goto _LLED;_tmpF7=((struct
Cyc_Absyn_Increment_e_struct*)_tmpD8)->f1;_tmpF8=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmpD8)->f2;if((int)_tmpF8 != 0)goto _LLED;_LLEC: Cyc_Absyndump_dump(({const char*
_tmp14B="++";_tag_arr(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,3));}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmpF7);goto _LLD0;_LLED: if(*((int*)_tmpD8)!= 5)
goto _LLEF;_tmpF9=((struct Cyc_Absyn_Increment_e_struct*)_tmpD8)->f1;_tmpFA=(void*)((
struct Cyc_Absyn_Increment_e_struct*)_tmpD8)->f2;if((int)_tmpFA != 2)goto _LLEF;
_LLEE: Cyc_Absyndump_dump(({const char*_tmp14C="--";_tag_arr(_tmp14C,sizeof(char),
_get_zero_arr_size(_tmp14C,3));}));Cyc_Absyndump_dumpexp_prec(myprec,_tmpF9);
goto _LLD0;_LLEF: if(*((int*)_tmpD8)!= 5)goto _LLF1;_tmpFB=((struct Cyc_Absyn_Increment_e_struct*)
_tmpD8)->f1;_tmpFC=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpD8)->f2;if((
int)_tmpFC != 1)goto _LLF1;_LLF0: Cyc_Absyndump_dumpexp_prec(myprec,_tmpFB);Cyc_Absyndump_dump(({
const char*_tmp14D="++";_tag_arr(_tmp14D,sizeof(char),_get_zero_arr_size(_tmp14D,
3));}));goto _LLD0;_LLF1: if(*((int*)_tmpD8)!= 5)goto _LLF3;_tmpFD=((struct Cyc_Absyn_Increment_e_struct*)
_tmpD8)->f1;_tmpFE=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpD8)->f2;if((
int)_tmpFE != 3)goto _LLF3;_LLF2: Cyc_Absyndump_dumpexp_prec(myprec,_tmpFD);Cyc_Absyndump_dump(({
const char*_tmp14E="--";_tag_arr(_tmp14E,sizeof(char),_get_zero_arr_size(_tmp14E,
3));}));goto _LLD0;_LLF3: if(*((int*)_tmpD8)!= 6)goto _LLF5;_tmpFF=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpD8)->f1;_tmp100=((struct Cyc_Absyn_Conditional_e_struct*)_tmpD8)->f2;_tmp101=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpD8)->f3;_LLF4: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpFF);Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,
_tmp100);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,
_tmp101);goto _LLD0;_LLF5: if(*((int*)_tmpD8)!= 7)goto _LLF7;_tmp102=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpD8)->f1;_tmp103=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpD8)->f2;_LLF6: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumpexp_prec(myprec,_tmp102);Cyc_Absyndump_dump_char((int)',');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp103);Cyc_Absyndump_dump_char((int)')');
goto _LLD0;_LLF7: if(*((int*)_tmpD8)!= 8)goto _LLF9;_tmp104=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmpD8)->f1;_tmp105=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmpD8)->f2;_LLF8:
_tmp106=_tmp104;_tmp107=_tmp105;goto _LLFA;_LLF9: if(*((int*)_tmpD8)!= 9)goto _LLFB;
_tmp106=((struct Cyc_Absyn_FnCall_e_struct*)_tmpD8)->f1;_tmp107=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpD8)->f2;_LLFA: Cyc_Absyndump_dumpexp_prec(myprec,_tmp106);Cyc_Absyndump_dump_nospace(({
const char*_tmp14F="(";_tag_arr(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,2));}));
Cyc_Absyndump_dumpexps_prec(20,_tmp107);Cyc_Absyndump_dump_nospace(({const char*
_tmp150=")";_tag_arr(_tmp150,sizeof(char),_get_zero_arr_size(_tmp150,2));}));
goto _LLD0;_LLFB: if(*((int*)_tmpD8)!= 10)goto _LLFD;_tmp108=((struct Cyc_Absyn_Throw_e_struct*)
_tmpD8)->f1;_LLFC: Cyc_Absyndump_dump(({const char*_tmp151="throw";_tag_arr(
_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,6));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp108);goto _LLD0;_LLFD: if(*((int*)_tmpD8)!= 11)goto _LLFF;_tmp109=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpD8)->f1;_LLFE: _tmp10A=_tmp109;goto
_LL100;_LLFF: if(*((int*)_tmpD8)!= 12)goto _LL101;_tmp10A=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpD8)->f1;_LL100: Cyc_Absyndump_dumpexp_prec(inprec,_tmp10A);goto _LLD0;_LL101:
if(*((int*)_tmpD8)!= 13)goto _LL103;_tmp10B=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpD8)->f1;_tmp10C=((struct Cyc_Absyn_Cast_e_struct*)_tmpD8)->f2;_LL102: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumptyp(_tmp10B);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp10C);goto _LLD0;_LL103: if(*((int*)_tmpD8)!= 14)goto _LL105;_tmp10D=((
struct Cyc_Absyn_Address_e_struct*)_tmpD8)->f1;_LL104: Cyc_Absyndump_dump_char((
int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp10D);goto _LLD0;_LL105: if(*((int*)
_tmpD8)!= 15)goto _LL107;_tmp10E=((struct Cyc_Absyn_New_e_struct*)_tmpD8)->f1;
_tmp10F=((struct Cyc_Absyn_New_e_struct*)_tmpD8)->f2;_LL106: Cyc_Absyndump_dump(({
const char*_tmp152="new ";_tag_arr(_tmp152,sizeof(char),_get_zero_arr_size(
_tmp152,5));}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp10F);goto _LLD0;_LL107: if(*((
int*)_tmpD8)!= 16)goto _LL109;_tmp110=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpD8)->f1;_LL108: Cyc_Absyndump_dump(({const char*_tmp153="sizeof(";_tag_arr(
_tmp153,sizeof(char),_get_zero_arr_size(_tmp153,8));}));Cyc_Absyndump_dumptyp(
_tmp110);Cyc_Absyndump_dump_char((int)')');goto _LLD0;_LL109: if(*((int*)_tmpD8)!= 
17)goto _LL10B;_tmp111=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpD8)->f1;_LL10A:
Cyc_Absyndump_dump(({const char*_tmp154="sizeof(";_tag_arr(_tmp154,sizeof(char),
_get_zero_arr_size(_tmp154,8));}));Cyc_Absyndump_dumpexp(_tmp111);Cyc_Absyndump_dump_char((
int)')');goto _LLD0;_LL10B: if(*((int*)_tmpD8)!= 18)goto _LL10D;_tmp112=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmpD8)->f1;_tmp113=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpD8)->f2;if(*((int*)_tmp113)!= 0)goto _LL10D;_tmp114=((struct Cyc_Absyn_StructField_struct*)
_tmp113)->f1;_LL10C: Cyc_Absyndump_dump(({const char*_tmp155="offsetof(";_tag_arr(
_tmp155,sizeof(char),_get_zero_arr_size(_tmp155,10));}));Cyc_Absyndump_dumptyp(
_tmp112);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*((struct
_tagged_arr*)_tmp114));Cyc_Absyndump_dump_char((int)')');goto _LLD0;_LL10D: if(*((
int*)_tmpD8)!= 18)goto _LL10F;_tmp115=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpD8)->f1;_tmp116=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpD8)->f2;if(*((
int*)_tmp116)!= 1)goto _LL10F;_tmp117=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp116)->f1;_LL10E: Cyc_Absyndump_dump(({const char*_tmp156="offsetof(";_tag_arr(
_tmp156,sizeof(char),_get_zero_arr_size(_tmp156,10));}));Cyc_Absyndump_dumptyp(
_tmp115);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump((struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp159;_tmp159.tag=1;_tmp159.f1=(unsigned int)((int)
_tmp117);{void*_tmp157[1]={& _tmp159};Cyc_aprintf(({const char*_tmp158="%d";
_tag_arr(_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,3));}),_tag_arr(_tmp157,
sizeof(void*),1));}}));Cyc_Absyndump_dump_char((int)')');goto _LLD0;_LL10F: if(*((
int*)_tmpD8)!= 19)goto _LL111;_tmp118=((struct Cyc_Absyn_Gentyp_e_struct*)_tmpD8)->f1;
_tmp119=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmpD8)->f2;_LL110: Cyc_Absyndump_dump(({
const char*_tmp15A="__gen(";_tag_arr(_tmp15A,sizeof(char),_get_zero_arr_size(
_tmp15A,7));}));Cyc_Absyndump_dumptvars(_tmp118);Cyc_Absyndump_dumptyp(_tmp119);
Cyc_Absyndump_dump_char((int)')');goto _LLD0;_LL111: if(*((int*)_tmpD8)!= 20)goto
_LL113;_tmp11A=((struct Cyc_Absyn_Deref_e_struct*)_tmpD8)->f1;_LL112: Cyc_Absyndump_dump_char((
int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp11A);goto _LLD0;_LL113: if(*((int*)
_tmpD8)!= 21)goto _LL115;_tmp11B=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpD8)->f1;
_tmp11C=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpD8)->f2;_LL114: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp11B);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*((
struct _tagged_arr*)_tmp11C));goto _LLD0;_LL115: if(*((int*)_tmpD8)!= 22)goto _LL117;
_tmp11D=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpD8)->f1;_tmp11E=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpD8)->f2;_LL116: Cyc_Absyndump_dumpexp_prec(myprec,_tmp11D);Cyc_Absyndump_dump_nospace(({
const char*_tmp15B="->";_tag_arr(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,
3));}));Cyc_Absyndump_dump_nospace(*((struct _tagged_arr*)_tmp11E));goto _LLD0;
_LL117: if(*((int*)_tmpD8)!= 23)goto _LL119;_tmp11F=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpD8)->f1;_tmp120=((struct Cyc_Absyn_Subscript_e_struct*)_tmpD8)->f2;_LL118: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp11F);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp120);
Cyc_Absyndump_dump_char((int)']');goto _LLD0;_LL119: if(*((int*)_tmpD8)!= 24)goto
_LL11B;_tmp121=((struct Cyc_Absyn_Tuple_e_struct*)_tmpD8)->f1;_LL11A: Cyc_Absyndump_dump(({
const char*_tmp15C="$(";_tag_arr(_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,
3));}));Cyc_Absyndump_dumpexps_prec(20,_tmp121);Cyc_Absyndump_dump_char((int)')');
goto _LLD0;_LL11B: if(*((int*)_tmpD8)!= 25)goto _LL11D;_tmp122=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpD8)->f1;_tmp123=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpD8)->f2;_LL11C:
Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp((*((struct _tuple1*)
_tmp122)).f3);Cyc_Absyndump_dump_char((int)')');((void(*)(void(*f)(struct
_tuple10*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,
struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp123,({const
char*_tmp15D="{";_tag_arr(_tmp15D,sizeof(char),_get_zero_arr_size(_tmp15D,2));}),({
const char*_tmp15E="}";_tag_arr(_tmp15E,sizeof(char),_get_zero_arr_size(_tmp15E,2));}),({
const char*_tmp15F=",";_tag_arr(_tmp15F,sizeof(char),_get_zero_arr_size(_tmp15F,2));}));
goto _LLD0;_LL11D: if(*((int*)_tmpD8)!= 26)goto _LL11F;_tmp124=((struct Cyc_Absyn_Array_e_struct*)
_tmpD8)->f1;_LL11E:((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp124,({const char*_tmp160="{";_tag_arr(_tmp160,sizeof(
char),_get_zero_arr_size(_tmp160,2));}),({const char*_tmp161="}";_tag_arr(_tmp161,
sizeof(char),_get_zero_arr_size(_tmp161,2));}),({const char*_tmp162=",";_tag_arr(
_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,2));}));goto _LLD0;_LL11F: if(*((
int*)_tmpD8)!= 27)goto _LL121;_tmp125=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpD8)->f1;_tmp126=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpD8)->f2;
_tmp127=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpD8)->f3;_LL120: Cyc_Absyndump_dump(({
const char*_tmp163="new {for";_tag_arr(_tmp163,sizeof(char),_get_zero_arr_size(
_tmp163,9));}));Cyc_Absyndump_dump_str((*((struct _tuple0*)_tmp125->name)).f2);
Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp126);Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dumpexp(_tmp127);Cyc_Absyndump_dump_char((int)'}');goto
_LLD0;_LL121: if(*((int*)_tmpD8)!= 28)goto _LL123;_tmp128=((struct Cyc_Absyn_Struct_e_struct*)
_tmpD8)->f1;_tmp129=((struct Cyc_Absyn_Struct_e_struct*)_tmpD8)->f2;_tmp12A=((
struct Cyc_Absyn_Struct_e_struct*)_tmpD8)->f3;_LL122: Cyc_Absyndump_dumpqvar(
_tmp128);Cyc_Absyndump_dump_char((int)'{');if(_tmp129 != 0)Cyc_Absyndump_dumptps(
_tmp129);((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp12A,({const char*_tmp164="";_tag_arr(_tmp164,sizeof(char),
_get_zero_arr_size(_tmp164,1));}),({const char*_tmp165="}";_tag_arr(_tmp165,
sizeof(char),_get_zero_arr_size(_tmp165,2));}),({const char*_tmp166=",";_tag_arr(
_tmp166,sizeof(char),_get_zero_arr_size(_tmp166,2));}));goto _LLD0;_LL123: if(*((
int*)_tmpD8)!= 29)goto _LL125;_tmp12B=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpD8)->f2;_LL124:((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp12B,({const char*_tmp167="{";_tag_arr(_tmp167,sizeof(
char),_get_zero_arr_size(_tmp167,2));}),({const char*_tmp168="}";_tag_arr(_tmp168,
sizeof(char),_get_zero_arr_size(_tmp168,2));}),({const char*_tmp169=",";_tag_arr(
_tmp169,sizeof(char),_get_zero_arr_size(_tmp169,2));}));goto _LLD0;_LL125: if(*((
int*)_tmpD8)!= 30)goto _LL127;_tmp12C=((struct Cyc_Absyn_Tunion_e_struct*)_tmpD8)->f1;
_tmp12D=((struct Cyc_Absyn_Tunion_e_struct*)_tmpD8)->f3;_LL126: Cyc_Absyndump_dumpqvar(
_tmp12D->name);if(_tmp12C != 0)((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp,_tmp12C,({const char*_tmp16A="(";_tag_arr(_tmp16A,sizeof(
char),_get_zero_arr_size(_tmp16A,2));}),({const char*_tmp16B=")";_tag_arr(_tmp16B,
sizeof(char),_get_zero_arr_size(_tmp16B,2));}),({const char*_tmp16C=",";_tag_arr(
_tmp16C,sizeof(char),_get_zero_arr_size(_tmp16C,2));}));goto _LLD0;_LL127: if(*((
int*)_tmpD8)!= 31)goto _LL129;_tmp12E=((struct Cyc_Absyn_Enum_e_struct*)_tmpD8)->f1;
_LL128: Cyc_Absyndump_dumpqvar(_tmp12E);goto _LLD0;_LL129: if(*((int*)_tmpD8)!= 32)
goto _LL12B;_tmp12F=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmpD8)->f1;_LL12A: Cyc_Absyndump_dumpqvar(
_tmp12F);goto _LLD0;_LL12B: if(*((int*)_tmpD8)!= 33)goto _LL12D;_tmp130=((struct Cyc_Absyn_Malloc_e_struct*)
_tmpD8)->f1;_tmp131=_tmp130.is_calloc;_tmp132=_tmp130.rgn;_tmp133=_tmp130.elt_type;
_tmp134=_tmp130.num_elts;_LL12C: if(_tmp131){if(_tmp132 != 0){Cyc_Absyndump_dump(({
const char*_tmp16D="rcalloc(";_tag_arr(_tmp16D,sizeof(char),_get_zero_arr_size(
_tmp16D,9));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp132));
Cyc_Absyndump_dump(({const char*_tmp16E=",";_tag_arr(_tmp16E,sizeof(char),
_get_zero_arr_size(_tmp16E,2));}));}else{Cyc_Absyndump_dump(({const char*_tmp16F="calloc";
_tag_arr(_tmp16F,sizeof(char),_get_zero_arr_size(_tmp16F,7));}));}Cyc_Absyndump_dumpexp(
_tmp134);Cyc_Absyndump_dump(({const char*_tmp170=",";_tag_arr(_tmp170,sizeof(char),
_get_zero_arr_size(_tmp170,2));}));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmp133)),0));Cyc_Absyndump_dump(({const char*_tmp171=")";
_tag_arr(_tmp171,sizeof(char),_get_zero_arr_size(_tmp171,2));}));}else{if(
_tmp132 != 0){Cyc_Absyndump_dump(({const char*_tmp172="rmalloc(";_tag_arr(_tmp172,
sizeof(char),_get_zero_arr_size(_tmp172,9));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmp132));Cyc_Absyndump_dump(({const char*_tmp173=",";_tag_arr(
_tmp173,sizeof(char),_get_zero_arr_size(_tmp173,2));}));}else{Cyc_Absyndump_dump(({
const char*_tmp174="malloc(";_tag_arr(_tmp174,sizeof(char),_get_zero_arr_size(
_tmp174,8));}));}if(_tmp133 != 0)Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*((
void**)_tmp133),0),_tmp134,0));else{Cyc_Absyndump_dumpexp(_tmp134);}Cyc_Absyndump_dump(({
const char*_tmp175=")";_tag_arr(_tmp175,sizeof(char),_get_zero_arr_size(_tmp175,2));}));}
goto _LLD0;_LL12D: if(*((int*)_tmpD8)!= 34)goto _LL12F;_tmp135=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpD8)->f1;_tmp136=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpD8)->f2;_LL12E:((
void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmp136,({const char*_tmp176="{";_tag_arr(_tmp176,sizeof(char),_get_zero_arr_size(
_tmp176,2));}),({const char*_tmp177="}";_tag_arr(_tmp177,sizeof(char),
_get_zero_arr_size(_tmp177,2));}),({const char*_tmp178=",";_tag_arr(_tmp178,
sizeof(char),_get_zero_arr_size(_tmp178,2));}));goto _LLD0;_LL12F: if(*((int*)
_tmpD8)!= 35)goto _LL131;_tmp137=((struct Cyc_Absyn_StmtExp_e_struct*)_tmpD8)->f1;
_LL130: Cyc_Absyndump_dump_nospace(({const char*_tmp179="({";_tag_arr(_tmp179,
sizeof(char),_get_zero_arr_size(_tmp179,3));}));Cyc_Absyndump_dumpstmt(_tmp137);
Cyc_Absyndump_dump_nospace(({const char*_tmp17A="})";_tag_arr(_tmp17A,sizeof(char),
_get_zero_arr_size(_tmp17A,3));}));goto _LLD0;_LL131: if(*((int*)_tmpD8)!= 36)goto
_LL133;_tmp138=((struct Cyc_Absyn_Codegen_e_struct*)_tmpD8)->f1;_LL132: Cyc_Absyndump_dump(({
const char*_tmp17B="codegen(";_tag_arr(_tmp17B,sizeof(char),_get_zero_arr_size(
_tmp17B,9));}));Cyc_Absyndump_dumpdecl(({struct Cyc_Absyn_Decl*_tmp17C=_cycalloc(
sizeof(*_tmp17C));_tmp17C->r=(void*)((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp17E;_tmp17E.tag=1;_tmp17E.f1=_tmp138;_tmp17E;});_tmp17D;}));_tmp17C->loc=e->loc;
_tmp17C;}));Cyc_Absyndump_dump(({const char*_tmp17F=")";_tag_arr(_tmp17F,sizeof(
char),_get_zero_arr_size(_tmp17F,2));}));goto _LLD0;_LL133: if(*((int*)_tmpD8)!= 
37)goto _LLD0;_tmp139=((struct Cyc_Absyn_Fill_e_struct*)_tmpD8)->f1;_LL134: Cyc_Absyndump_dump(({
const char*_tmp180="fill(";_tag_arr(_tmp180,sizeof(char),_get_zero_arr_size(
_tmp180,6));}));Cyc_Absyndump_dumpexp(_tmp139);Cyc_Absyndump_dump(({const char*
_tmp181=")";_tag_arr(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,2));}));
goto _LLD0;_LLD0:;}if(inprec >= myprec)Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*e){Cyc_Absyndump_dumpexp_prec(0,e);}void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmp182=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmp182->where_clause == 0?(
void*)(_tmp182->pattern)->r == (void*)0: 0)Cyc_Absyndump_dump(({const char*_tmp183="default:";
_tag_arr(_tmp183,sizeof(char),_get_zero_arr_size(_tmp183,9));}));else{Cyc_Absyndump_dump(({
const char*_tmp184="case";_tag_arr(_tmp184,sizeof(char),_get_zero_arr_size(
_tmp184,5));}));Cyc_Absyndump_dumppat(_tmp182->pattern);if(_tmp182->where_clause
!= 0){Cyc_Absyndump_dump(({const char*_tmp185="&&";_tag_arr(_tmp185,sizeof(char),
_get_zero_arr_size(_tmp185,3));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmp182->where_clause));}Cyc_Absyndump_dump_nospace(({const char*
_tmp186=":";_tag_arr(_tmp186,sizeof(char),_get_zero_arr_size(_tmp186,2));}));}
Cyc_Absyndump_dumpstmt(_tmp182->body);}}void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*
s){void*_tmp187=(void*)s->r;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Stmt*
_tmp189;struct Cyc_Absyn_Stmt*_tmp18A;struct Cyc_Absyn_Exp*_tmp18B;struct Cyc_Absyn_Exp*
_tmp18C;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Stmt*_tmp18E;struct Cyc_Absyn_Stmt*
_tmp18F;struct _tuple2 _tmp190;struct Cyc_Absyn_Exp*_tmp191;struct Cyc_Absyn_Stmt*
_tmp192;struct _tagged_arr*_tmp193;struct Cyc_Absyn_Exp*_tmp194;struct _tuple2
_tmp195;struct Cyc_Absyn_Exp*_tmp196;struct _tuple2 _tmp197;struct Cyc_Absyn_Exp*
_tmp198;struct Cyc_Absyn_Stmt*_tmp199;struct Cyc_Absyn_ForArrayInfo _tmp19A;struct
Cyc_List_List*_tmp19B;struct _tuple2 _tmp19C;struct Cyc_Absyn_Exp*_tmp19D;struct
_tuple2 _tmp19E;struct Cyc_Absyn_Exp*_tmp19F;struct Cyc_Absyn_Stmt*_tmp1A0;struct
Cyc_Absyn_Exp*_tmp1A1;struct Cyc_List_List*_tmp1A2;struct Cyc_Absyn_Decl*_tmp1A3;
struct Cyc_Absyn_Stmt*_tmp1A4;struct _tagged_arr*_tmp1A5;struct Cyc_Absyn_Stmt*
_tmp1A6;struct Cyc_Absyn_Stmt*_tmp1A7;struct _tuple2 _tmp1A8;struct Cyc_Absyn_Exp*
_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_List_List*_tmp1AB;struct Cyc_List_List*
_tmp1AC;struct Cyc_List_List*_tmp1AD;struct Cyc_Absyn_Stmt*_tmp1AE;struct Cyc_List_List*
_tmp1AF;struct Cyc_Absyn_Tvar*_tmp1B0;struct Cyc_Absyn_Vardecl*_tmp1B1;int _tmp1B2;
struct Cyc_Absyn_Stmt*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_Absyn_Stmt*
_tmp1B5;struct Cyc_Absyn_Stmt*_tmp1B6;_LL13A: if((int)_tmp187 != 0)goto _LL13C;
_LL13B: Cyc_Absyndump_dump_semi();goto _LL139;_LL13C: if(_tmp187 <= (void*)1?1:*((
int*)_tmp187)!= 0)goto _LL13E;_tmp188=((struct Cyc_Absyn_Exp_s_struct*)_tmp187)->f1;
_LL13D: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dumpexp(_tmp188);Cyc_Absyndump_dump_semi();
goto _LL139;_LL13E: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 1)goto _LL140;
_tmp189=((struct Cyc_Absyn_Seq_s_struct*)_tmp187)->f1;_tmp18A=((struct Cyc_Absyn_Seq_s_struct*)
_tmp187)->f2;_LL13F: if(Cyc_Absynpp_is_declaration(_tmp189)){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstmt(_tmp189);Cyc_Absyndump_dump_char((int)'}');}
else{Cyc_Absyndump_dumpstmt(_tmp189);}if(Cyc_Absynpp_is_declaration(_tmp18A)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp18A);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dumpstmt(_tmp18A);}goto _LL139;_LL140: if(_tmp187 <= (
void*)1?1:*((int*)_tmp187)!= 2)goto _LL142;_tmp18B=((struct Cyc_Absyn_Return_s_struct*)
_tmp187)->f1;if(_tmp18B != 0)goto _LL142;_LL141: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({
const char*_tmp1B7="return;";_tag_arr(_tmp1B7,sizeof(char),_get_zero_arr_size(
_tmp1B7,8));}));goto _LL139;_LL142: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 2)
goto _LL144;_tmp18C=((struct Cyc_Absyn_Return_s_struct*)_tmp187)->f1;_LL143: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1B8="return";_tag_arr(_tmp1B8,sizeof(
char),_get_zero_arr_size(_tmp1B8,7));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmp18C));Cyc_Absyndump_dump_semi();goto _LL139;_LL144: if(_tmp187 <= (
void*)1?1:*((int*)_tmp187)!= 3)goto _LL146;_tmp18D=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp187)->f1;_tmp18E=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp187)->f2;_tmp18F=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp187)->f3;_LL145: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(({const char*_tmp1B9="if(";_tag_arr(_tmp1B9,sizeof(char),
_get_zero_arr_size(_tmp1B9,4));}));Cyc_Absyndump_dumpexp(_tmp18D);{void*_tmp1BA=(
void*)_tmp18E->r;_LL16B: if(_tmp1BA <= (void*)1?1:*((int*)_tmp1BA)!= 1)goto _LL16D;
_LL16C: goto _LL16E;_LL16D: if(_tmp1BA <= (void*)1?1:*((int*)_tmp1BA)!= 12)goto
_LL16F;_LL16E: goto _LL170;_LL16F: if(_tmp1BA <= (void*)1?1:*((int*)_tmp1BA)!= 3)
goto _LL171;_LL170: goto _LL172;_LL171: if(_tmp1BA <= (void*)1?1:*((int*)_tmp1BA)!= 
15)goto _LL173;_LL172: Cyc_Absyndump_dump_nospace(({const char*_tmp1BB="){";
_tag_arr(_tmp1BB,sizeof(char),_get_zero_arr_size(_tmp1BB,3));}));Cyc_Absyndump_dumpstmt(
_tmp18E);Cyc_Absyndump_dump_char((int)'}');goto _LL16A;_LL173:;_LL174: Cyc_Absyndump_dump_char((
int)')');Cyc_Absyndump_dumpstmt(_tmp18E);_LL16A:;}{void*_tmp1BC=(void*)_tmp18F->r;
_LL176: if((int)_tmp1BC != 0)goto _LL178;_LL177: goto _LL175;_LL178:;_LL179: Cyc_Absyndump_dump(({
const char*_tmp1BD="else{";_tag_arr(_tmp1BD,sizeof(char),_get_zero_arr_size(
_tmp1BD,6));}));Cyc_Absyndump_dumpstmt(_tmp18F);Cyc_Absyndump_dump_char((int)'}');
goto _LL175;_LL175:;}goto _LL139;_LL146: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 
4)goto _LL148;_tmp190=((struct Cyc_Absyn_While_s_struct*)_tmp187)->f1;_tmp191=
_tmp190.f1;_tmp192=((struct Cyc_Absyn_While_s_struct*)_tmp187)->f2;_LL147: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1BE="while(";_tag_arr(_tmp1BE,sizeof(
char),_get_zero_arr_size(_tmp1BE,7));}));Cyc_Absyndump_dumpexp(_tmp191);Cyc_Absyndump_dump_nospace(({
const char*_tmp1BF="){";_tag_arr(_tmp1BF,sizeof(char),_get_zero_arr_size(_tmp1BF,
3));}));Cyc_Absyndump_dumpstmt(_tmp192);Cyc_Absyndump_dump_char((int)'}');goto
_LL139;_LL148: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 5)goto _LL14A;_LL149: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1C0="break;";_tag_arr(_tmp1C0,sizeof(
char),_get_zero_arr_size(_tmp1C0,7));}));goto _LL139;_LL14A: if(_tmp187 <= (void*)1?
1:*((int*)_tmp187)!= 6)goto _LL14C;_LL14B: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({
const char*_tmp1C1="continue;";_tag_arr(_tmp1C1,sizeof(char),_get_zero_arr_size(
_tmp1C1,10));}));goto _LL139;_LL14C: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 7)
goto _LL14E;_tmp193=((struct Cyc_Absyn_Goto_s_struct*)_tmp187)->f1;_LL14D: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1C2="goto";_tag_arr(_tmp1C2,sizeof(
char),_get_zero_arr_size(_tmp1C2,5));}));Cyc_Absyndump_dump_str(_tmp193);Cyc_Absyndump_dump_semi();
goto _LL139;_LL14E: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 8)goto _LL150;
_tmp194=((struct Cyc_Absyn_For_s_struct*)_tmp187)->f1;_tmp195=((struct Cyc_Absyn_For_s_struct*)
_tmp187)->f2;_tmp196=_tmp195.f1;_tmp197=((struct Cyc_Absyn_For_s_struct*)_tmp187)->f3;
_tmp198=_tmp197.f1;_tmp199=((struct Cyc_Absyn_For_s_struct*)_tmp187)->f4;_LL14F:
Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*_tmp1C3="for(";
_tag_arr(_tmp1C3,sizeof(char),_get_zero_arr_size(_tmp1C3,5));}));Cyc_Absyndump_dumpexp(
_tmp194);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp196);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmp198);Cyc_Absyndump_dump_nospace(({const char*_tmp1C4="){";
_tag_arr(_tmp1C4,sizeof(char),_get_zero_arr_size(_tmp1C4,3));}));Cyc_Absyndump_dumpstmt(
_tmp199);Cyc_Absyndump_dump_char((int)'}');goto _LL139;_LL150: if(_tmp187 <= (void*)
1?1:*((int*)_tmp187)!= 19)goto _LL152;_tmp19A=((struct Cyc_Absyn_ForArray_s_struct*)
_tmp187)->f1;_tmp19B=_tmp19A.defns;_tmp19C=_tmp19A.condition;_tmp19D=_tmp19C.f1;
_tmp19E=_tmp19A.delta;_tmp19F=_tmp19E.f1;_tmp1A0=_tmp19A.body;_LL151: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1C5="forarray(";_tag_arr(_tmp1C5,
sizeof(char),_get_zero_arr_size(_tmp1C5,10));}));for(0;_tmp19B != 0;_tmp19B=
_tmp19B->tl){Cyc_Absyndump_dumpvardecl((struct Cyc_Absyn_Vardecl*)_tmp19B->hd,s->loc);}
Cyc_Absyndump_dumpexp(_tmp19D);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(
_tmp19F);Cyc_Absyndump_dump_nospace(({const char*_tmp1C6="){";_tag_arr(_tmp1C6,
sizeof(char),_get_zero_arr_size(_tmp1C6,3));}));Cyc_Absyndump_dumpstmt(_tmp1A0);
Cyc_Absyndump_dump_char((int)'}');goto _LL139;_LL152: if(_tmp187 <= (void*)1?1:*((
int*)_tmp187)!= 9)goto _LL154;_tmp1A1=((struct Cyc_Absyn_Switch_s_struct*)_tmp187)->f1;
_tmp1A2=((struct Cyc_Absyn_Switch_s_struct*)_tmp187)->f2;_LL153: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1C7="switch(";_tag_arr(_tmp1C7,
sizeof(char),_get_zero_arr_size(_tmp1C7,8));}));Cyc_Absyndump_dumpexp(_tmp1A1);
Cyc_Absyndump_dump_nospace(({const char*_tmp1C8="){";_tag_arr(_tmp1C8,sizeof(char),
_get_zero_arr_size(_tmp1C8,3));}));Cyc_Absyndump_dumpswitchclauses(_tmp1A2);Cyc_Absyndump_dump_char((
int)'}');goto _LL139;_LL154: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 12)goto
_LL156;_tmp1A3=((struct Cyc_Absyn_Decl_s_struct*)_tmp187)->f1;_tmp1A4=((struct Cyc_Absyn_Decl_s_struct*)
_tmp187)->f2;_LL155: Cyc_Absyndump_dumpdecl(_tmp1A3);Cyc_Absyndump_dumpstmt(
_tmp1A4);goto _LL139;_LL156: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 15)goto
_LL158;_tmp1A5=((struct Cyc_Absyn_Label_s_struct*)_tmp187)->f1;_tmp1A6=((struct
Cyc_Absyn_Label_s_struct*)_tmp187)->f2;_LL157: if(Cyc_Absynpp_is_declaration(
_tmp1A6)){Cyc_Absyndump_dump_str(_tmp1A5);Cyc_Absyndump_dump_nospace(({const char*
_tmp1C9=": {";_tag_arr(_tmp1C9,sizeof(char),_get_zero_arr_size(_tmp1C9,4));}));
Cyc_Absyndump_dumpstmt(_tmp1A6);Cyc_Absyndump_dump_char((int)'}');}else{Cyc_Absyndump_dump_str(
_tmp1A5);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp1A6);}goto
_LL139;_LL158: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 16)goto _LL15A;_tmp1A7=((
struct Cyc_Absyn_Do_s_struct*)_tmp187)->f1;_tmp1A8=((struct Cyc_Absyn_Do_s_struct*)
_tmp187)->f2;_tmp1A9=_tmp1A8.f1;_LL159: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({
const char*_tmp1CA="do{";_tag_arr(_tmp1CA,sizeof(char),_get_zero_arr_size(_tmp1CA,
4));}));Cyc_Absyndump_dumpstmt(_tmp1A7);Cyc_Absyndump_dump_nospace(({const char*
_tmp1CB="}while(";_tag_arr(_tmp1CB,sizeof(char),_get_zero_arr_size(_tmp1CB,8));}));
Cyc_Absyndump_dumpexp(_tmp1A9);Cyc_Absyndump_dump_nospace(({const char*_tmp1CC=");";
_tag_arr(_tmp1CC,sizeof(char),_get_zero_arr_size(_tmp1CC,3));}));goto _LL139;
_LL15A: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 10)goto _LL15C;_tmp1AA=((struct
Cyc_Absyn_SwitchC_s_struct*)_tmp187)->f1;_tmp1AB=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmp187)->f2;_LL15B: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1CD="switch \"C\"(";_tag_arr(_tmp1CD,sizeof(char),_get_zero_arr_size(_tmp1CD,
12));}));Cyc_Absyndump_dumpexp(_tmp1AA);Cyc_Absyndump_dump_nospace(({const char*
_tmp1CE="){";_tag_arr(_tmp1CE,sizeof(char),_get_zero_arr_size(_tmp1CE,3));}));
for(0;_tmp1AB != 0;_tmp1AB=_tmp1AB->tl){struct Cyc_Absyn_SwitchC_clause _tmp1D0;
struct Cyc_Absyn_Exp*_tmp1D1;struct Cyc_Absyn_Stmt*_tmp1D2;struct Cyc_Absyn_SwitchC_clause*
_tmp1CF=(struct Cyc_Absyn_SwitchC_clause*)_tmp1AB->hd;_tmp1D0=*_tmp1CF;_tmp1D1=
_tmp1D0.cnst_exp;_tmp1D2=_tmp1D0.body;if(_tmp1D1 == 0)Cyc_Absyndump_dump(({const
char*_tmp1D3="default:";_tag_arr(_tmp1D3,sizeof(char),_get_zero_arr_size(_tmp1D3,
9));}));else{Cyc_Absyndump_dump(({const char*_tmp1D4="case";_tag_arr(_tmp1D4,
sizeof(char),_get_zero_arr_size(_tmp1D4,5));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmp1D1));Cyc_Absyndump_dump_char((int)':');}Cyc_Absyndump_dumpstmt(
_tmp1D2);}Cyc_Absyndump_dump_char((int)'}');goto _LL139;_LL15C: if(_tmp187 <= (void*)
1?1:*((int*)_tmp187)!= 11)goto _LL15E;_tmp1AC=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp187)->f1;if(_tmp1AC != 0)goto _LL15E;_LL15D: Cyc_Absyndump_dump(({const char*
_tmp1D5="fallthru;";_tag_arr(_tmp1D5,sizeof(char),_get_zero_arr_size(_tmp1D5,10));}));
goto _LL139;_LL15E: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 11)goto _LL160;
_tmp1AD=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp187)->f1;_LL15F: Cyc_Absyndump_dump(({
const char*_tmp1D6="fallthru(";_tag_arr(_tmp1D6,sizeof(char),_get_zero_arr_size(
_tmp1D6,10));}));Cyc_Absyndump_dumpexps_prec(20,_tmp1AD);Cyc_Absyndump_dump_nospace(({
const char*_tmp1D7=");";_tag_arr(_tmp1D7,sizeof(char),_get_zero_arr_size(_tmp1D7,
3));}));goto _LL139;_LL160: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 17)goto
_LL162;_tmp1AE=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp187)->f1;_tmp1AF=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp187)->f2;_LL161: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(({const char*_tmp1D8="try";_tag_arr(_tmp1D8,sizeof(char),
_get_zero_arr_size(_tmp1D8,4));}));Cyc_Absyndump_dumpstmt(_tmp1AE);Cyc_Absyndump_dump(({
const char*_tmp1D9="catch{";_tag_arr(_tmp1D9,sizeof(char),_get_zero_arr_size(
_tmp1D9,7));}));Cyc_Absyndump_dumpswitchclauses(_tmp1AF);Cyc_Absyndump_dump_char((
int)'}');goto _LL139;_LL162: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 18)goto
_LL164;_tmp1B0=((struct Cyc_Absyn_Region_s_struct*)_tmp187)->f1;_tmp1B1=((struct
Cyc_Absyn_Region_s_struct*)_tmp187)->f2;_tmp1B2=((struct Cyc_Absyn_Region_s_struct*)
_tmp187)->f3;_tmp1B3=((struct Cyc_Absyn_Region_s_struct*)_tmp187)->f4;_LL163: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1DA="region";_tag_arr(_tmp1DA,sizeof(
char),_get_zero_arr_size(_tmp1DA,7));}));if(_tmp1B2)Cyc_Absyndump_dump(({const
char*_tmp1DB="[resetable]";_tag_arr(_tmp1DB,sizeof(char),_get_zero_arr_size(
_tmp1DB,12));}));Cyc_Absyndump_dump(({const char*_tmp1DC="<";_tag_arr(_tmp1DC,
sizeof(char),_get_zero_arr_size(_tmp1DC,2));}));Cyc_Absyndump_dumptvar(_tmp1B0);
Cyc_Absyndump_dump(({const char*_tmp1DD="> ";_tag_arr(_tmp1DD,sizeof(char),
_get_zero_arr_size(_tmp1DD,3));}));Cyc_Absyndump_dumpqvar(_tmp1B1->name);Cyc_Absyndump_dump(({
const char*_tmp1DE="{";_tag_arr(_tmp1DE,sizeof(char),_get_zero_arr_size(_tmp1DE,2));}));
Cyc_Absyndump_dumpstmt(_tmp1B3);Cyc_Absyndump_dump(({const char*_tmp1DF="}";
_tag_arr(_tmp1DF,sizeof(char),_get_zero_arr_size(_tmp1DF,2));}));goto _LL139;
_LL164: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 20)goto _LL166;_tmp1B4=((struct
Cyc_Absyn_ResetRegion_s_struct*)_tmp187)->f1;_LL165: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(({const char*_tmp1E0="reset_region(";_tag_arr(_tmp1E0,sizeof(
char),_get_zero_arr_size(_tmp1E0,14));}));Cyc_Absyndump_dumpexp(_tmp1B4);Cyc_Absyndump_dump(({
const char*_tmp1E1=");";_tag_arr(_tmp1E1,sizeof(char),_get_zero_arr_size(_tmp1E1,
3));}));goto _LL139;_LL166: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 13)goto
_LL168;_tmp1B5=((struct Cyc_Absyn_Cut_s_struct*)_tmp187)->f1;_LL167: Cyc_Absyndump_dump(({
const char*_tmp1E2="cut";_tag_arr(_tmp1E2,sizeof(char),_get_zero_arr_size(_tmp1E2,
4));}));Cyc_Absyndump_dumpstmt(_tmp1B5);goto _LL139;_LL168: if(_tmp187 <= (void*)1?
1:*((int*)_tmp187)!= 14)goto _LL139;_tmp1B6=((struct Cyc_Absyn_Splice_s_struct*)
_tmp187)->f1;_LL169: Cyc_Absyndump_dump(({const char*_tmp1E3="splice";_tag_arr(
_tmp1E3,sizeof(char),_get_zero_arr_size(_tmp1E3,7));}));Cyc_Absyndump_dumpstmt(
_tmp1B6);goto _LL139;_LL139:;}struct _tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};void Cyc_Absyndump_dumpdp(struct _tuple11*dp){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*((
struct _tuple11*)dp)).f1,({const char*_tmp1E4="";_tag_arr(_tmp1E4,sizeof(char),
_get_zero_arr_size(_tmp1E4,1));}),({const char*_tmp1E5="=";_tag_arr(_tmp1E5,
sizeof(char),_get_zero_arr_size(_tmp1E5,2));}),({const char*_tmp1E6="=";_tag_arr(
_tmp1E6,sizeof(char),_get_zero_arr_size(_tmp1E6,2));}));Cyc_Absyndump_dumppat((*((
struct _tuple11*)dp)).f2);}void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){void*
_tmp1E7=(void*)p->r;void*_tmp1E8;int _tmp1E9;void*_tmp1EA;int _tmp1EB;void*_tmp1EC;
int _tmp1ED;char _tmp1EE;struct _tagged_arr _tmp1EF;struct Cyc_Absyn_Vardecl*_tmp1F0;
struct Cyc_List_List*_tmp1F1;struct Cyc_Absyn_Pat*_tmp1F2;struct Cyc_Absyn_Vardecl*
_tmp1F3;struct _tuple0*_tmp1F4;struct _tuple0*_tmp1F5;struct Cyc_List_List*_tmp1F6;
struct Cyc_Absyn_AggrInfo _tmp1F7;void*_tmp1F8;struct Cyc_List_List*_tmp1F9;struct
Cyc_List_List*_tmp1FA;struct Cyc_Absyn_Tunionfield*_tmp1FB;struct Cyc_List_List*
_tmp1FC;struct Cyc_Absyn_Enumfield*_tmp1FD;struct Cyc_Absyn_Enumfield*_tmp1FE;
_LL17B: if((int)_tmp1E7 != 0)goto _LL17D;_LL17C: Cyc_Absyndump_dump_char((int)'_');
goto _LL17A;_LL17D: if((int)_tmp1E7 != 1)goto _LL17F;_LL17E: Cyc_Absyndump_dump(({
const char*_tmp1FF="NULL";_tag_arr(_tmp1FF,sizeof(char),_get_zero_arr_size(
_tmp1FF,5));}));goto _LL17A;_LL17F: if(_tmp1E7 <= (void*)2?1:*((int*)_tmp1E7)!= 1)
goto _LL181;_tmp1E8=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp1E7)->f1;if((int)
_tmp1E8 != 2)goto _LL181;_tmp1E9=((struct Cyc_Absyn_Int_p_struct*)_tmp1E7)->f2;
_LL180: _tmp1EB=_tmp1E9;goto _LL182;_LL181: if(_tmp1E7 <= (void*)2?1:*((int*)_tmp1E7)
!= 1)goto _LL183;_tmp1EA=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp1E7)->f1;if((
int)_tmp1EA != 0)goto _LL183;_tmp1EB=((struct Cyc_Absyn_Int_p_struct*)_tmp1E7)->f2;
_LL182: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp202;
_tmp202.tag=1;_tmp202.f1=(unsigned int)_tmp1EB;{void*_tmp200[1]={& _tmp202};Cyc_aprintf(({
const char*_tmp201="%d";_tag_arr(_tmp201,sizeof(char),_get_zero_arr_size(_tmp201,
3));}),_tag_arr(_tmp200,sizeof(void*),1));}}));goto _LL17A;_LL183: if(_tmp1E7 <= (
void*)2?1:*((int*)_tmp1E7)!= 1)goto _LL185;_tmp1EC=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp1E7)->f1;if((int)_tmp1EC != 1)goto _LL185;_tmp1ED=((struct Cyc_Absyn_Int_p_struct*)
_tmp1E7)->f2;_LL184: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp205;_tmp205.tag=1;_tmp205.f1=(unsigned int)_tmp1ED;{void*_tmp203[1]={&
_tmp205};Cyc_aprintf(({const char*_tmp204="%u";_tag_arr(_tmp204,sizeof(char),
_get_zero_arr_size(_tmp204,3));}),_tag_arr(_tmp203,sizeof(void*),1));}}));goto
_LL17A;_LL185: if(_tmp1E7 <= (void*)2?1:*((int*)_tmp1E7)!= 2)goto _LL187;_tmp1EE=((
struct Cyc_Absyn_Char_p_struct*)_tmp1E7)->f1;_LL186: Cyc_Absyndump_dump(({const
char*_tmp206="'";_tag_arr(_tmp206,sizeof(char),_get_zero_arr_size(_tmp206,2));}));
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp1EE));Cyc_Absyndump_dump_nospace(({
const char*_tmp207="'";_tag_arr(_tmp207,sizeof(char),_get_zero_arr_size(_tmp207,2));}));
goto _LL17A;_LL187: if(_tmp1E7 <= (void*)2?1:*((int*)_tmp1E7)!= 3)goto _LL189;
_tmp1EF=((struct Cyc_Absyn_Float_p_struct*)_tmp1E7)->f1;_LL188: Cyc_Absyndump_dump(
_tmp1EF);goto _LL17A;_LL189: if(_tmp1E7 <= (void*)2?1:*((int*)_tmp1E7)!= 0)goto
_LL18B;_tmp1F0=((struct Cyc_Absyn_Var_p_struct*)_tmp1E7)->f1;_LL18A: Cyc_Absyndump_dumpqvar(
_tmp1F0->name);goto _LL17A;_LL18B: if(_tmp1E7 <= (void*)2?1:*((int*)_tmp1E7)!= 4)
goto _LL18D;_tmp1F1=((struct Cyc_Absyn_Tuple_p_struct*)_tmp1E7)->f1;_LL18C:((void(*)(
void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
_tmp1F1,({const char*_tmp208="$(";_tag_arr(_tmp208,sizeof(char),
_get_zero_arr_size(_tmp208,3));}),({const char*_tmp209=")";_tag_arr(_tmp209,
sizeof(char),_get_zero_arr_size(_tmp209,2));}),({const char*_tmp20A=",";_tag_arr(
_tmp20A,sizeof(char),_get_zero_arr_size(_tmp20A,2));}));goto _LL17A;_LL18D: if(
_tmp1E7 <= (void*)2?1:*((int*)_tmp1E7)!= 5)goto _LL18F;_tmp1F2=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp1E7)->f1;_LL18E: Cyc_Absyndump_dump(({const char*_tmp20B="&";_tag_arr(_tmp20B,
sizeof(char),_get_zero_arr_size(_tmp20B,2));}));Cyc_Absyndump_dumppat(_tmp1F2);
goto _LL17A;_LL18F: if(_tmp1E7 <= (void*)2?1:*((int*)_tmp1E7)!= 6)goto _LL191;
_tmp1F3=((struct Cyc_Absyn_Reference_p_struct*)_tmp1E7)->f1;_LL190: Cyc_Absyndump_dump(({
const char*_tmp20C="*";_tag_arr(_tmp20C,sizeof(char),_get_zero_arr_size(_tmp20C,2));}));
Cyc_Absyndump_dumpqvar(_tmp1F3->name);goto _LL17A;_LL191: if(_tmp1E7 <= (void*)2?1:*((
int*)_tmp1E7)!= 11)goto _LL193;_tmp1F4=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp1E7)->f1;_LL192: Cyc_Absyndump_dumpqvar(_tmp1F4);goto _LL17A;_LL193: if(_tmp1E7
<= (void*)2?1:*((int*)_tmp1E7)!= 12)goto _LL195;_tmp1F5=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp1E7)->f1;_tmp1F6=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1E7)->f2;_LL194:
Cyc_Absyndump_dumpqvar(_tmp1F5);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct
Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp1F6,({const char*_tmp20D="(";
_tag_arr(_tmp20D,sizeof(char),_get_zero_arr_size(_tmp20D,2));}),({const char*
_tmp20E=")";_tag_arr(_tmp20E,sizeof(char),_get_zero_arr_size(_tmp20E,2));}),({
const char*_tmp20F=",";_tag_arr(_tmp20F,sizeof(char),_get_zero_arr_size(_tmp20F,2));}));
goto _LL17A;_LL195: if(_tmp1E7 <= (void*)2?1:*((int*)_tmp1E7)!= 7)goto _LL197;
_tmp1F7=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1E7)->f1;_tmp1F8=(void*)_tmp1F7.aggr_info;
_tmp1F9=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1E7)->f2;_tmp1FA=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp1E7)->f3;_LL196: {struct _tuple0*_tmp211;struct _tuple3 _tmp210=Cyc_Absyn_aggr_kinded_name(
_tmp1F8);_tmp211=_tmp210.f2;Cyc_Absyndump_dumpqvar(_tmp211);Cyc_Absyndump_dump_char((
int)'{');((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,_tmp1F9,({const char*_tmp212="[";_tag_arr(_tmp212,sizeof(
char),_get_zero_arr_size(_tmp212,2));}),({const char*_tmp213="]";_tag_arr(_tmp213,
sizeof(char),_get_zero_arr_size(_tmp213,2));}),({const char*_tmp214=",";_tag_arr(
_tmp214,sizeof(char),_get_zero_arr_size(_tmp214,2));}));((void(*)(void(*f)(
struct _tuple11*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr
end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp1FA,({
const char*_tmp215="";_tag_arr(_tmp215,sizeof(char),_get_zero_arr_size(_tmp215,1));}),({
const char*_tmp216="}";_tag_arr(_tmp216,sizeof(char),_get_zero_arr_size(_tmp216,2));}),({
const char*_tmp217=",";_tag_arr(_tmp217,sizeof(char),_get_zero_arr_size(_tmp217,2));}));
goto _LL17A;}_LL197: if(_tmp1E7 <= (void*)2?1:*((int*)_tmp1E7)!= 8)goto _LL199;
_tmp1FB=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1E7)->f2;_tmp1FC=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1E7)->f3;_LL198: Cyc_Absyndump_dumpqvar(_tmp1FB->name);if(_tmp1FC != 0)((void(*)(
void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
_tmp1FC,({const char*_tmp218="(";_tag_arr(_tmp218,sizeof(char),_get_zero_arr_size(
_tmp218,2));}),({const char*_tmp219=")";_tag_arr(_tmp219,sizeof(char),
_get_zero_arr_size(_tmp219,2));}),({const char*_tmp21A=",";_tag_arr(_tmp21A,
sizeof(char),_get_zero_arr_size(_tmp21A,2));}));goto _LL17A;_LL199: if(_tmp1E7 <= (
void*)2?1:*((int*)_tmp1E7)!= 9)goto _LL19B;_tmp1FD=((struct Cyc_Absyn_Enum_p_struct*)
_tmp1E7)->f2;_LL19A: _tmp1FE=_tmp1FD;goto _LL19C;_LL19B: if(_tmp1E7 <= (void*)2?1:*((
int*)_tmp1E7)!= 10)goto _LL17A;_tmp1FE=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp1E7)->f2;_LL19C: Cyc_Absyndump_dumpqvar(_tmp1FE->name);goto _LL17A;_LL17A:;}
void Cyc_Absyndump_dumptunionfield(struct Cyc_Absyn_Tunionfield*ef){Cyc_Absyndump_dumpqvar(
ef->name);if(ef->typs != 0)Cyc_Absyndump_dumpargs(ef->typs);}void Cyc_Absyndump_dumptunionfields(
struct Cyc_List_List*fields){((void(*)(void(*f)(struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumptunionfield,
fields,({const char*_tmp21B=",";_tag_arr(_tmp21B,sizeof(char),_get_zero_arr_size(
_tmp21B,2));}));}void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);if(ef->tag != 0){Cyc_Absyndump_dump(({const char*
_tmp21C=" = ";_tag_arr(_tmp21C,sizeof(char),_get_zero_arr_size(_tmp21C,4));}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List*fields){((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,
fields,({const char*_tmp21D=",";_tag_arr(_tmp21D,sizeof(char),_get_zero_arr_size(
_tmp21D,2));}));}void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Aggrfield _tmp21F;struct
_tagged_arr*_tmp220;struct Cyc_Absyn_Tqual _tmp221;void*_tmp222;struct Cyc_Absyn_Exp*
_tmp223;struct Cyc_List_List*_tmp224;struct Cyc_Absyn_Aggrfield*_tmp21E=(struct Cyc_Absyn_Aggrfield*)
fields->hd;_tmp21F=*_tmp21E;_tmp220=_tmp21F.name;_tmp221=_tmp21F.tq;_tmp222=(
void*)_tmp21F.type;_tmp223=_tmp21F.width;_tmp224=_tmp21F.attributes;((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tagged_arr*),struct _tagged_arr*))Cyc_Absyndump_dumptqtd)(
_tmp221,_tmp222,Cyc_Absyndump_dump_str,_tmp220);Cyc_Absyndump_dumpatts(_tmp224);
if(_tmp223 != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp((struct
Cyc_Absyn_Exp*)_check_null(_tmp223));}Cyc_Absyndump_dump_semi();}}void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(td->name);Cyc_Absyndump_dumptvars(
td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple3*pr){{void*_tmp225=(*((struct _tuple3*)pr)).f1;_LL19E: if((int)
_tmp225 != 8)goto _LL1A0;_LL19F: goto _LL19D;_LL1A0: if((int)_tmp225 != 0)goto _LL1A2;
_LL1A1: Cyc_Absyndump_dump(({const char*_tmp226="_stdcall";_tag_arr(_tmp226,
sizeof(char),_get_zero_arr_size(_tmp226,9));}));goto _LL19D;_LL1A2: if((int)
_tmp225 != 1)goto _LL1A4;_LL1A3: Cyc_Absyndump_dump(({const char*_tmp227="_cdecl";
_tag_arr(_tmp227,sizeof(char),_get_zero_arr_size(_tmp227,7));}));goto _LL19D;
_LL1A4: if((int)_tmp225 != 2)goto _LL1A6;_LL1A5: Cyc_Absyndump_dump(({const char*
_tmp228="_fastcall";_tag_arr(_tmp228,sizeof(char),_get_zero_arr_size(_tmp228,10));}));
goto _LL19D;_LL1A6:;_LL1A7: goto _LL19D;_LL19D:;}Cyc_Absyndump_dumpqvar((*((struct
_tuple3*)pr)).f2);}static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*
fd){Cyc_Absyndump_dump_callconv(fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}
static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){for(0;vds != 0;vds=vds->tl){
Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl != 0)
Cyc_Absyndump_dump_char((int)',');}}void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Vardecl _tmp22A;void*_tmp22B;
struct _tuple0*_tmp22C;struct Cyc_Absyn_Tqual _tmp22D;void*_tmp22E;struct Cyc_Absyn_Exp*
_tmp22F;struct Cyc_List_List*_tmp230;struct Cyc_Absyn_Vardecl*_tmp229=vd;_tmp22A=*
_tmp229;_tmp22B=(void*)_tmp22A.sc;_tmp22C=_tmp22A.name;_tmp22D=_tmp22A.tq;
_tmp22E=(void*)_tmp22A.type;_tmp22F=_tmp22A.initializer;_tmp230=_tmp22A.attributes;
Cyc_Absyndump_dumploc(loc);if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dumpatts(
_tmp230);Cyc_Absyndump_dumpscope(_tmp22B);{struct Cyc_Absyn_Tqual _tmp232;void*
_tmp233;struct Cyc_List_List*_tmp234;struct _tuple5 _tmp231=Cyc_Absynpp_to_tms(
_tmp22D,_tmp22E);_tmp232=_tmp231.f1;_tmp233=_tmp231.f2;_tmp234=_tmp231.f3;{void*
call_conv=(void*)8;{struct Cyc_List_List*tms2=_tmp234;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp235=(void*)tms2->hd;struct Cyc_List_List*_tmp236;_LL1A9: if(*((int*)
_tmp235)!= 5)goto _LL1AB;_tmp236=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp235)->f2;
_LL1AA: for(0;_tmp236 != 0;_tmp236=_tmp236->tl){void*_tmp237=(void*)_tmp236->hd;
_LL1AE: if((int)_tmp237 != 0)goto _LL1B0;_LL1AF: call_conv=(void*)0;goto _LL1AD;
_LL1B0: if((int)_tmp237 != 1)goto _LL1B2;_LL1B1: call_conv=(void*)1;goto _LL1AD;
_LL1B2: if((int)_tmp237 != 2)goto _LL1B4;_LL1B3: call_conv=(void*)2;goto _LL1AD;
_LL1B4:;_LL1B5: goto _LL1AD;_LL1AD:;}goto _LL1A8;_LL1AB:;_LL1AC: goto _LL1A8;_LL1A8:;}}
Cyc_Absyndump_dumptq(_tmp232);Cyc_Absyndump_dumpntyp(_tmp233);{struct _tuple3
_tmp238=({struct _tuple3 _tmp239;_tmp239.f1=call_conv;_tmp239.f2=_tmp22C;_tmp239;});((
void(*)(struct Cyc_List_List*tms,void(*f)(struct _tuple3*),struct _tuple3*a))Cyc_Absyndump_dumptms)(
Cyc_List_imp_rev(_tmp234),Cyc_Absyndump_dump_callconv_qvar,& _tmp238);}}}}else{
if(_tmp22B == (void*)3?Cyc_Absyndump_qvar_to_Cids: 0){void*_tmp23A=Cyc_Tcutil_compress(
_tmp22E);_LL1B7: if(_tmp23A <= (void*)3?1:*((int*)_tmp23A)!= 8)goto _LL1B9;_LL1B8:
goto _LL1B6;_LL1B9:;_LL1BA: Cyc_Absyndump_dumpscope(_tmp22B);_LL1B6:;}else{Cyc_Absyndump_dumpscope(
_tmp22B);}((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct
_tuple0*))Cyc_Absyndump_dumptqtd)(_tmp22D,_tmp22E,Cyc_Absyndump_dumpqvar,_tmp22C);
Cyc_Absyndump_dumpatts(_tmp230);}if(_tmp22F != 0){Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp22F));}Cyc_Absyndump_dump_semi();}
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){void*_tmp23B=(void*)d->r;
struct Cyc_Absyn_Vardecl*_tmp23C;struct Cyc_Absyn_Fndecl*_tmp23D;struct Cyc_Absyn_Aggrdecl*
_tmp23E;struct Cyc_Absyn_Tuniondecl*_tmp23F;struct Cyc_Absyn_Tuniondecl _tmp240;
void*_tmp241;struct _tuple0*_tmp242;struct Cyc_List_List*_tmp243;struct Cyc_Core_Opt*
_tmp244;int _tmp245;struct Cyc_Absyn_Enumdecl*_tmp246;struct Cyc_Absyn_Enumdecl
_tmp247;void*_tmp248;struct _tuple0*_tmp249;struct Cyc_Core_Opt*_tmp24A;struct Cyc_Absyn_Pat*
_tmp24B;struct Cyc_Absyn_Exp*_tmp24C;struct Cyc_List_List*_tmp24D;struct Cyc_Absyn_Typedefdecl*
_tmp24E;struct _tagged_arr*_tmp24F;struct Cyc_List_List*_tmp250;struct _tuple0*
_tmp251;struct Cyc_List_List*_tmp252;struct Cyc_List_List*_tmp253;_LL1BC: if(*((int*)
_tmp23B)!= 0)goto _LL1BE;_tmp23C=((struct Cyc_Absyn_Var_d_struct*)_tmp23B)->f1;
_LL1BD: Cyc_Absyndump_dumpvardecl(_tmp23C,d->loc);goto _LL1BB;_LL1BE: if(*((int*)
_tmp23B)!= 1)goto _LL1C0;_tmp23D=((struct Cyc_Absyn_Fn_d_struct*)_tmp23B)->f1;
_LL1BF: Cyc_Absyndump_dumploc(d->loc);if(Cyc_Absyndump_to_VC)Cyc_Absyndump_dumpatts(
_tmp23D->attributes);if(_tmp23D->is_inline){if(Cyc_Absyndump_to_VC)Cyc_Absyndump_dump(({
const char*_tmp254="__inline";_tag_arr(_tmp254,sizeof(char),_get_zero_arr_size(
_tmp254,9));}));else{Cyc_Absyndump_dump(({const char*_tmp255="inline";_tag_arr(
_tmp255,sizeof(char),_get_zero_arr_size(_tmp255,7));}));}}Cyc_Absyndump_dumpscope((
void*)_tmp23D->sc);{void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp257=
_cycalloc(sizeof(*_tmp257));_tmp257[0]=({struct Cyc_Absyn_FnType_struct _tmp258;
_tmp258.tag=8;_tmp258.f1=({struct Cyc_Absyn_FnInfo _tmp259;_tmp259.tvars=_tmp23D->tvs;
_tmp259.effect=_tmp23D->effect;_tmp259.ret_typ=(void*)((void*)_tmp23D->ret_type);
_tmp259.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple4*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp23D->args);_tmp259.c_varargs=
_tmp23D->c_varargs;_tmp259.cyc_varargs=_tmp23D->cyc_varargs;_tmp259.rgn_po=
_tmp23D->rgn_po;_tmp259.attributes=0;_tmp259;});_tmp258;});_tmp257;});((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))
Cyc_Absyndump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp256;_tmp256.q_const=0;_tmp256.q_volatile=
0;_tmp256.q_restrict=0;_tmp256;}),t,Cyc_Absyndump_to_VC?Cyc_Absyndump_dump_callconv_fdqvar:
Cyc_Absyndump_dump_atts_qvar,_tmp23D);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(
_tmp23D->body);Cyc_Absyndump_dump_char((int)'}');goto _LL1BB;}_LL1C0: if(*((int*)
_tmp23B)!= 4)goto _LL1C2;_tmp23E=((struct Cyc_Absyn_Aggr_d_struct*)_tmp23B)->f1;
_LL1C1: Cyc_Absyndump_dumpscope((void*)_tmp23E->sc);Cyc_Absyndump_dumpaggr_kind((
void*)_tmp23E->kind);Cyc_Absyndump_dumpqvar(_tmp23E->name);Cyc_Absyndump_dumptvars(
_tmp23E->tvs);if(_tmp23E->impl == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23E->impl))->exist_vars
!= 0)((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23E->impl))->exist_vars,({
const char*_tmp25A="<";_tag_arr(_tmp25A,sizeof(char),_get_zero_arr_size(_tmp25A,2));}),({
const char*_tmp25B=">";_tag_arr(_tmp25B,sizeof(char),_get_zero_arr_size(_tmp25B,2));}),({
const char*_tmp25C=",";_tag_arr(_tmp25C,sizeof(char),_get_zero_arr_size(_tmp25C,2));}));
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23E->impl))->rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp23E->impl))->rgn_po);}Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp23E->impl))->fields);Cyc_Absyndump_dump(({const char*_tmp25D="}";
_tag_arr(_tmp25D,sizeof(char),_get_zero_arr_size(_tmp25D,2));}));Cyc_Absyndump_dumpatts(
_tmp23E->attributes);Cyc_Absyndump_dump(({const char*_tmp25E=";";_tag_arr(_tmp25E,
sizeof(char),_get_zero_arr_size(_tmp25E,2));}));}goto _LL1BB;_LL1C2: if(*((int*)
_tmp23B)!= 5)goto _LL1C4;_tmp23F=((struct Cyc_Absyn_Tunion_d_struct*)_tmp23B)->f1;
_tmp240=*_tmp23F;_tmp241=(void*)_tmp240.sc;_tmp242=_tmp240.name;_tmp243=_tmp240.tvs;
_tmp244=_tmp240.fields;_tmp245=_tmp240.is_xtunion;_LL1C3: Cyc_Absyndump_dumpscope(
_tmp241);Cyc_Absyndump_dump(_tmp245?({const char*_tmp25F="xtunion";_tag_arr(
_tmp25F,sizeof(char),_get_zero_arr_size(_tmp25F,8));}):({const char*_tmp260="tunion";
_tag_arr(_tmp260,sizeof(char),_get_zero_arr_size(_tmp260,7));}));Cyc_Absyndump_dumpqvar(
_tmp242);Cyc_Absyndump_dumptvars(_tmp243);if(_tmp244 == 0)Cyc_Absyndump_dump_semi();
else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumptunionfields((struct Cyc_List_List*)
_tmp244->v);Cyc_Absyndump_dump_nospace(({const char*_tmp261="};";_tag_arr(_tmp261,
sizeof(char),_get_zero_arr_size(_tmp261,3));}));}goto _LL1BB;_LL1C4: if(*((int*)
_tmp23B)!= 6)goto _LL1C6;_tmp246=((struct Cyc_Absyn_Enum_d_struct*)_tmp23B)->f1;
_tmp247=*_tmp246;_tmp248=(void*)_tmp247.sc;_tmp249=_tmp247.name;_tmp24A=_tmp247.fields;
_LL1C5: Cyc_Absyndump_dumpscope(_tmp248);Cyc_Absyndump_dump(({const char*_tmp262="enum ";
_tag_arr(_tmp262,sizeof(char),_get_zero_arr_size(_tmp262,6));}));Cyc_Absyndump_dumpqvar(
_tmp249);if(_tmp24A == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp24A->v);Cyc_Absyndump_dump_nospace(({
const char*_tmp263="};";_tag_arr(_tmp263,sizeof(char),_get_zero_arr_size(_tmp263,
3));}));}return;_LL1C6: if(*((int*)_tmp23B)!= 2)goto _LL1C8;_tmp24B=((struct Cyc_Absyn_Let_d_struct*)
_tmp23B)->f1;_tmp24C=((struct Cyc_Absyn_Let_d_struct*)_tmp23B)->f3;_LL1C7: Cyc_Absyndump_dump(({
const char*_tmp264="let";_tag_arr(_tmp264,sizeof(char),_get_zero_arr_size(_tmp264,
4));}));Cyc_Absyndump_dumppat(_tmp24B);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(
_tmp24C);Cyc_Absyndump_dump_semi();goto _LL1BB;_LL1C8: if(*((int*)_tmp23B)!= 3)
goto _LL1CA;_tmp24D=((struct Cyc_Absyn_Letv_d_struct*)_tmp23B)->f1;_LL1C9: Cyc_Absyndump_dump(({
const char*_tmp265="let ";_tag_arr(_tmp265,sizeof(char),_get_zero_arr_size(
_tmp265,5));}));Cyc_Absyndump_dumpids(_tmp24D);Cyc_Absyndump_dump_semi();goto
_LL1BB;_LL1CA: if(*((int*)_tmp23B)!= 7)goto _LL1CC;_tmp24E=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp23B)->f1;_LL1CB: if(!Cyc_Absyndump_expand_typedefs?1:(_tmp24E->defn != 0?Cyc_Absynpp_is_anon_aggrtype((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp24E->defn))->v): 0)){Cyc_Absyndump_dump(({
const char*_tmp266="typedef";_tag_arr(_tmp266,sizeof(char),_get_zero_arr_size(
_tmp266,8));}));{void*t;if(_tmp24E->defn == 0)t=Cyc_Absyn_new_evar(_tmp24E->kind,
0);else{t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp24E->defn))->v;}((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))
Cyc_Absyndump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp267;_tmp267.q_const=0;_tmp267.q_volatile=
0;_tmp267.q_restrict=0;_tmp267;}),t,Cyc_Absyndump_dumptypedefname,_tmp24E);Cyc_Absyndump_dump_semi();}}
goto _LL1BB;_LL1CC: if(*((int*)_tmp23B)!= 8)goto _LL1CE;_tmp24F=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp23B)->f1;_tmp250=((struct Cyc_Absyn_Namespace_d_struct*)_tmp23B)->f2;_LL1CD:
Cyc_Absyndump_dump(({const char*_tmp268="namespace";_tag_arr(_tmp268,sizeof(char),
_get_zero_arr_size(_tmp268,10));}));Cyc_Absyndump_dump_str(_tmp24F);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp250 != 0;_tmp250=_tmp250->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp250->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1BB;
_LL1CE: if(*((int*)_tmp23B)!= 9)goto _LL1D0;_tmp251=((struct Cyc_Absyn_Using_d_struct*)
_tmp23B)->f1;_tmp252=((struct Cyc_Absyn_Using_d_struct*)_tmp23B)->f2;_LL1CF: Cyc_Absyndump_dump(({
const char*_tmp269="using";_tag_arr(_tmp269,sizeof(char),_get_zero_arr_size(
_tmp269,6));}));Cyc_Absyndump_dumpqvar(_tmp251);Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp252 != 0;_tmp252=_tmp252->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)
_tmp252->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1BB;_LL1D0: if(*((int*)
_tmp23B)!= 10)goto _LL1BB;_tmp253=((struct Cyc_Absyn_ExternC_d_struct*)_tmp23B)->f1;
_LL1D1: Cyc_Absyndump_dump(({const char*_tmp26A="extern \"C\" {";_tag_arr(_tmp26A,
sizeof(char),_get_zero_arr_size(_tmp26A,13));}));for(0;_tmp253 != 0;_tmp253=
_tmp253->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp253->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1BB;_LL1BB:;}static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*
e){struct _tuple6 pr=Cyc_Evexp_eval_const_uint_exp(e);if(pr.f1 != 1?1: !pr.f2){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}void Cyc_Absyndump_dumptms(
struct Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){f(a);return;}{void*
_tmp26B=(void*)tms->hd;void*_tmp26C;struct Cyc_Absyn_Exp*_tmp26D;struct Cyc_Absyn_Conref*
_tmp26E;void*_tmp26F;struct Cyc_Absyn_Tqual _tmp270;void*_tmp271;struct Cyc_Absyn_Exp*
_tmp272;struct Cyc_Absyn_Conref*_tmp273;void*_tmp274;struct Cyc_Absyn_Tqual _tmp275;
void*_tmp276;struct Cyc_Absyn_Conref*_tmp277;void*_tmp278;struct Cyc_Absyn_Tqual
_tmp279;void*_tmp27A;struct Cyc_Absyn_Exp*_tmp27B;struct Cyc_Absyn_Conref*_tmp27C;
void*_tmp27D;struct Cyc_Absyn_Tvar*_tmp27E;struct Cyc_Absyn_Tqual _tmp27F;void*
_tmp280;struct Cyc_Absyn_Exp*_tmp281;struct Cyc_Absyn_Conref*_tmp282;void*_tmp283;
struct Cyc_Absyn_Tvar*_tmp284;struct Cyc_Absyn_Tqual _tmp285;void*_tmp286;struct Cyc_Absyn_Conref*
_tmp287;void*_tmp288;struct Cyc_Absyn_Tvar*_tmp289;struct Cyc_Absyn_Tqual _tmp28A;
_LL1D3: if(*((int*)_tmp26B)!= 2)goto _LL1D5;_tmp26C=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp26B)->f1;if(*((int*)_tmp26C)!= 1)goto _LL1D5;_tmp26D=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmp26C)->f1;_tmp26E=((struct Cyc_Absyn_Nullable_ps_struct*)_tmp26C)->f2;_tmp26F=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp26B)->f2;if((int)_tmp26F != 2)
goto _LL1D5;_tmp270=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp26B)->f3;_LL1D4: Cyc_Absyndump_dump_char((
int)'*');Cyc_Absyndump_dump_upperbound(_tmp26D);if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp26E))Cyc_Absyndump_dump(({const char*_tmp28B="ZEROTERM";
_tag_arr(_tmp28B,sizeof(char),_get_zero_arr_size(_tmp28B,9));}));Cyc_Absyndump_dumptms(
tms->tl,f,a);return;_LL1D5: if(*((int*)_tmp26B)!= 2)goto _LL1D7;_tmp271=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp26B)->f1;if(*((int*)_tmp271)!= 0)goto
_LL1D7;_tmp272=((struct Cyc_Absyn_NonNullable_ps_struct*)_tmp271)->f1;_tmp273=((
struct Cyc_Absyn_NonNullable_ps_struct*)_tmp271)->f2;_tmp274=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp26B)->f2;if((int)_tmp274 != 2)goto _LL1D7;_tmp275=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp26B)->f3;_LL1D6: Cyc_Absyndump_dump_char((int)'@');Cyc_Absyndump_dump_upperbound(
_tmp272);if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp273))Cyc_Absyndump_dump(({const char*_tmp28C="ZEROTERM";_tag_arr(_tmp28C,
sizeof(char),_get_zero_arr_size(_tmp28C,9));}));Cyc_Absyndump_dumptms(tms->tl,f,
a);return;_LL1D7: if(*((int*)_tmp26B)!= 2)goto _LL1D9;_tmp276=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp26B)->f1;if(*((int*)_tmp276)!= 2)goto _LL1D9;_tmp277=((struct Cyc_Absyn_TaggedArray_ps_struct*)
_tmp276)->f1;_tmp278=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp26B)->f2;
if((int)_tmp278 != 2)goto _LL1D9;_tmp279=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp26B)->f3;_LL1D8: Cyc_Absyndump_dump_char((int)'?');if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp277))Cyc_Absyndump_dump(({const char*_tmp28D="ZEROTERM";
_tag_arr(_tmp28D,sizeof(char),_get_zero_arr_size(_tmp28D,9));}));Cyc_Absyndump_dumptms(
tms->tl,f,a);return;_LL1D9: if(*((int*)_tmp26B)!= 2)goto _LL1DB;_tmp27A=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp26B)->f1;if(*((int*)_tmp27A)!= 1)goto
_LL1DB;_tmp27B=((struct Cyc_Absyn_Nullable_ps_struct*)_tmp27A)->f1;_tmp27C=((
struct Cyc_Absyn_Nullable_ps_struct*)_tmp27A)->f2;_tmp27D=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp26B)->f2;if(_tmp27D <= (void*)3?1:*((int*)_tmp27D)!= 1)goto _LL1DB;_tmp27E=((
struct Cyc_Absyn_VarType_struct*)_tmp27D)->f1;_tmp27F=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp26B)->f3;_LL1DA: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dump_upperbound(
_tmp27B);if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp27C))Cyc_Absyndump_dump(({const char*_tmp28E="ZEROTERM";_tag_arr(_tmp28E,
sizeof(char),_get_zero_arr_size(_tmp28E,9));}));Cyc_Absyndump_dump_str(_tmp27E->name);
Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1DB: if(*((int*)_tmp26B)!= 2)goto
_LL1DD;_tmp280=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp26B)->f1;if(*((
int*)_tmp280)!= 0)goto _LL1DD;_tmp281=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmp280)->f1;_tmp282=((struct Cyc_Absyn_NonNullable_ps_struct*)_tmp280)->f2;
_tmp283=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp26B)->f2;if(_tmp283 <= (
void*)3?1:*((int*)_tmp283)!= 1)goto _LL1DD;_tmp284=((struct Cyc_Absyn_VarType_struct*)
_tmp283)->f1;_tmp285=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp26B)->f3;_LL1DC:
Cyc_Absyndump_dump_char((int)'@');Cyc_Absyndump_dump_upperbound(_tmp281);if(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp282))Cyc_Absyndump_dump(({
const char*_tmp28F="ZEROTERM";_tag_arr(_tmp28F,sizeof(char),_get_zero_arr_size(
_tmp28F,9));}));Cyc_Absyndump_dump_str(_tmp284->name);Cyc_Absyndump_dumptms(tms->tl,
f,a);return;_LL1DD: if(*((int*)_tmp26B)!= 2)goto _LL1DF;_tmp286=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp26B)->f1;if(*((int*)_tmp286)!= 2)goto _LL1DF;_tmp287=((struct Cyc_Absyn_TaggedArray_ps_struct*)
_tmp286)->f1;_tmp288=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp26B)->f2;
if(_tmp288 <= (void*)3?1:*((int*)_tmp288)!= 1)goto _LL1DF;_tmp289=((struct Cyc_Absyn_VarType_struct*)
_tmp288)->f1;_tmp28A=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp26B)->f3;_LL1DE:
Cyc_Absyndump_dump_char((int)'?');if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp287))Cyc_Absyndump_dump(({const char*_tmp290="ZEROTERM";_tag_arr(_tmp290,
sizeof(char),_get_zero_arr_size(_tmp290,9));}));Cyc_Absyndump_dump_str(_tmp289->name);
Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1DF: if(*((int*)_tmp26B)!= 2)goto
_LL1E1;_LL1E0:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp291=
_cycalloc(sizeof(*_tmp291));_tmp291[0]=({struct Cyc_Core_Impossible_struct _tmp292;
_tmp292.tag=Cyc_Core_Impossible;_tmp292.f1=({const char*_tmp293="dumptms: bad Pointer_mod";
_tag_arr(_tmp293,sizeof(char),_get_zero_arr_size(_tmp293,25));});_tmp292;});
_tmp291;}));_LL1E1:;_LL1E2: {int next_is_pointer=0;if(tms->tl != 0){void*_tmp294=(
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_LL1E4: if(*((int*)_tmp294)
!= 2)goto _LL1E6;_LL1E5: next_is_pointer=1;goto _LL1E3;_LL1E6:;_LL1E7: goto _LL1E3;
_LL1E3:;}if(next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(
tms->tl,f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp295=(
void*)tms->hd;struct Cyc_Absyn_Conref*_tmp296;struct Cyc_Absyn_Exp*_tmp297;struct
Cyc_Absyn_Conref*_tmp298;void*_tmp299;struct Cyc_List_List*_tmp29A;int _tmp29B;
struct Cyc_Absyn_VarargInfo*_tmp29C;struct Cyc_Core_Opt*_tmp29D;struct Cyc_List_List*
_tmp29E;void*_tmp29F;struct Cyc_List_List*_tmp2A0;struct Cyc_Position_Segment*
_tmp2A1;struct Cyc_List_List*_tmp2A2;struct Cyc_Position_Segment*_tmp2A3;int
_tmp2A4;struct Cyc_List_List*_tmp2A5;void*_tmp2A6;void*_tmp2A7;_LL1E9: if(*((int*)
_tmp295)!= 0)goto _LL1EB;_tmp296=((struct Cyc_Absyn_Carray_mod_struct*)_tmp295)->f1;
_LL1EA: Cyc_Absyndump_dump(({const char*_tmp2A8="[]";_tag_arr(_tmp2A8,sizeof(char),
_get_zero_arr_size(_tmp2A8,3));}));if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp296))Cyc_Absyndump_dump(({const char*_tmp2A9="ZEROTERM";_tag_arr(_tmp2A9,
sizeof(char),_get_zero_arr_size(_tmp2A9,9));}));goto _LL1E8;_LL1EB: if(*((int*)
_tmp295)!= 1)goto _LL1ED;_tmp297=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmp295)->f1;
_tmp298=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmp295)->f2;_LL1EC: Cyc_Absyndump_dump_char((
int)'[');Cyc_Absyndump_dumpexp(_tmp297);Cyc_Absyndump_dump_char((int)']');if(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp298))Cyc_Absyndump_dump(({
const char*_tmp2AA="ZEROTERM";_tag_arr(_tmp2AA,sizeof(char),_get_zero_arr_size(
_tmp2AA,9));}));goto _LL1E8;_LL1ED: if(*((int*)_tmp295)!= 3)goto _LL1EF;_tmp299=(
void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp295)->f1;if(*((int*)_tmp299)!= 
1)goto _LL1EF;_tmp29A=((struct Cyc_Absyn_WithTypes_struct*)_tmp299)->f1;_tmp29B=((
struct Cyc_Absyn_WithTypes_struct*)_tmp299)->f2;_tmp29C=((struct Cyc_Absyn_WithTypes_struct*)
_tmp299)->f3;_tmp29D=((struct Cyc_Absyn_WithTypes_struct*)_tmp299)->f4;_tmp29E=((
struct Cyc_Absyn_WithTypes_struct*)_tmp299)->f5;_LL1EE: Cyc_Absyndump_dumpfunargs(
_tmp29A,_tmp29B,_tmp29C,_tmp29D,_tmp29E);goto _LL1E8;_LL1EF: if(*((int*)_tmp295)!= 
3)goto _LL1F1;_tmp29F=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp295)->f1;
if(*((int*)_tmp29F)!= 0)goto _LL1F1;_tmp2A0=((struct Cyc_Absyn_NoTypes_struct*)
_tmp29F)->f1;_tmp2A1=((struct Cyc_Absyn_NoTypes_struct*)_tmp29F)->f2;_LL1F0:((
void(*)(void(*f)(struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr
start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,
_tmp2A0,({const char*_tmp2AB="(";_tag_arr(_tmp2AB,sizeof(char),_get_zero_arr_size(
_tmp2AB,2));}),({const char*_tmp2AC=")";_tag_arr(_tmp2AC,sizeof(char),
_get_zero_arr_size(_tmp2AC,2));}),({const char*_tmp2AD=",";_tag_arr(_tmp2AD,
sizeof(char),_get_zero_arr_size(_tmp2AD,2));}));goto _LL1E8;_LL1F1: if(*((int*)
_tmp295)!= 4)goto _LL1F3;_tmp2A2=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp295)->f1;
_tmp2A3=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp295)->f2;_tmp2A4=((struct
Cyc_Absyn_TypeParams_mod_struct*)_tmp295)->f3;_LL1F2: if(_tmp2A4)Cyc_Absyndump_dumpkindedtvars(
_tmp2A2);else{Cyc_Absyndump_dumptvars(_tmp2A2);}goto _LL1E8;_LL1F3: if(*((int*)
_tmp295)!= 5)goto _LL1F5;_tmp2A5=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp295)->f2;
_LL1F4: Cyc_Absyndump_dumpatts(_tmp2A5);goto _LL1E8;_LL1F5: if(*((int*)_tmp295)!= 2)
goto _LL1E8;_tmp2A6=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp295)->f1;
_tmp2A7=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp295)->f2;_LL1F6:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2AE=_cycalloc(sizeof(*
_tmp2AE));_tmp2AE[0]=({struct Cyc_Core_Impossible_struct _tmp2AF;_tmp2AF.tag=Cyc_Core_Impossible;
_tmp2AF.f1=({const char*_tmp2B0="dumptms";_tag_arr(_tmp2B0,sizeof(char),
_get_zero_arr_size(_tmp2B0,8));});_tmp2AF;});_tmp2AE;}));_LL1E8:;}return;}_LL1D2:;}}
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
struct Cyc_Absyn_Tqual _tmp2B2;void*_tmp2B3;struct Cyc_List_List*_tmp2B4;struct
_tuple5 _tmp2B1=Cyc_Absynpp_to_tms(tq,t);_tmp2B2=_tmp2B1.f1;_tmp2B3=_tmp2B1.f2;
_tmp2B4=_tmp2B1.f3;Cyc_Absyndump_dumptq(_tmp2B2);Cyc_Absyndump_dumpntyp(_tmp2B3);
Cyc_Absyndump_dumptms(Cyc_List_imp_rev(_tmp2B4),f,a);}void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){Cyc_Absyndump_pos=0;*((struct Cyc___cycFILE**)
Cyc_Absyndump_dump_file)=f;for(0;tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)tdl->hd);}({void*_tmp2B5[0]={};Cyc_fprintf(f,({const char*
_tmp2B6="\n";_tag_arr(_tmp2B6,sizeof(char),_get_zero_arr_size(_tmp2B6,2));}),
_tag_arr(_tmp2B5,sizeof(void*),0));});}
