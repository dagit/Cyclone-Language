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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Std___cycFILE;
extern struct Cyc_Std___cycFILE*Cyc_Std_stdout;struct Cyc_Std_Cstdio___abstractFILE;
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct _tagged_arr,
struct _tagged_arr);int Cyc_Std_fputc(int,struct Cyc_Std___cycFILE*);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};int Cyc_Std_file_string_write(
struct Cyc_Std___cycFILE*,struct _tagged_arr src,int src_offset,int max_count);struct
Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern char Cyc_List_Nth[8];int Cyc_List_exists(int(*pred)(
void*),struct Cyc_List_List*x);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;
struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct
Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*
seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];
struct _tagged_arr Cyc_Position_get_line_directive(struct Cyc_Position_Segment*loc);
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
tq;struct Cyc_Absyn_Conref*bounds;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*
name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct
Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
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
targs;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_TunionType_struct{int tag;struct Cyc_Absyn_TunionInfo f1;};
struct Cyc_Absyn_TunionFieldType_struct{int tag;struct Cyc_Absyn_TunionFieldInfo f1;
};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct
Cyc_Absyn_IntType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;void*f1;struct Cyc_Absyn_Tqual
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo
f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{
int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;
void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};
struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeInt_struct{int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{int tag;
void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_NonNullable_ps_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Nullable_ps_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};struct Cyc_Absyn_Section_att_struct{int tag;struct _tagged_arr f1;};
struct Cyc_Absyn_Format_att_struct{int tag;void*f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{
int tag;int f1;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pointer_mod_struct{int tag;void*f1;void*f2;struct Cyc_Absyn_Tqual
f3;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{
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
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,
struct Cyc_Position_Segment*);struct _tagged_arr Cyc_Absyn_attribute2string(void*);
struct _tuple3{void*f1;struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
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
struct Cyc_Position_Segment*);struct Cyc_Std___cycFILE**Cyc_Absyndump_dump_file=&
Cyc_Std_stdout;void Cyc_Absyndump_ignore(void*x){return;}static unsigned int Cyc_Absyndump_pos=
0;static char Cyc_Absyndump_prev_char='x';int Cyc_Absyndump_need_space_before(){
switch(Cyc_Absyndump_prev_char){case '{': _LL0: goto _LL1;case '}': _LL1: goto _LL2;case
'(': _LL2: goto _LL3;case ')': _LL3: goto _LL4;case '[': _LL4: goto _LL5;case ']': _LL5: goto
_LL6;case ';': _LL6: goto _LL7;case ',': _LL7: goto _LL8;case '=': _LL8: goto _LL9;case '?':
_LL9: goto _LLA;case '!': _LLA: goto _LLB;case ' ': _LLB: goto _LLC;case '\n': _LLC: goto _LLD;
case '*': _LLD: return 0;default: _LLE: return 1;}}void Cyc_Absyndump_dump(struct
_tagged_arr s){int sz=(int)_get_arr_size(s,sizeof(char));if(!((int)*((const char*)
_check_unknown_subscript(s,sizeof(char),sz - 1))))-- sz;Cyc_Absyndump_pos +=sz;if(
Cyc_Absyndump_pos > 80){Cyc_Absyndump_pos=(unsigned int)sz;Cyc_Std_fputc((int)'\n',*
Cyc_Absyndump_dump_file);}else{if(Cyc_Absyndump_need_space_before())Cyc_Std_fputc((
int)' ',*Cyc_Absyndump_dump_file);}Cyc_Std_file_string_write(*Cyc_Absyndump_dump_file,
s,0,sz);if(sz >= 1)Cyc_Absyndump_prev_char=*((const char*)_check_unknown_subscript(
s,sizeof(char),sz - 1));}void Cyc_Absyndump_dump_nospace(struct _tagged_arr s){int sz=(
int)_get_arr_size(s,sizeof(char));if(!((int)*((const char*)
_check_unknown_subscript(s,sizeof(char),sz - 1))))-- sz;Cyc_Absyndump_pos +=sz;Cyc_Std_file_string_write(*
Cyc_Absyndump_dump_file,s,0,sz);if(sz >= 1)Cyc_Absyndump_prev_char=*((const char*)
_check_unknown_subscript(s,sizeof(char),sz - 1));}void Cyc_Absyndump_dump_char(int
c){++ Cyc_Absyndump_pos;Cyc_Std_fputc(c,*Cyc_Absyndump_dump_file);Cyc_Absyndump_prev_char=(
char)c;}void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*loc){if(loc == 0)
return;if(!Cyc_Absyndump_generate_line_directives)return;{struct _tagged_arr _tmp0=
Cyc_Position_get_line_directive(loc);Cyc_Absyndump_dump(_tmp0);}}void Cyc_Absyndump_dump_str(
struct _tagged_arr*s){Cyc_Absyndump_dump(*s);}void Cyc_Absyndump_dump_semi(){Cyc_Absyndump_dump_char((
int)';');}void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,
struct _tagged_arr sep){if(l == 0)return;for(0;l->tl != 0;l=l->tl){f((void*)l->hd);
Cyc_Absyndump_dump_nospace(sep);}f((void*)l->hd);}void Cyc_Absyndump_dump_sep_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _tagged_arr sep){if(l
== 0)return;for(0;l->tl != 0;l=l->tl){f(env,(void*)l->hd);Cyc_Absyndump_dump_nospace(
sep);}f(env,(void*)l->hd);}void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep){Cyc_Absyndump_dump_nospace(
start);Cyc_Absyndump_dump_sep(f,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_group_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep){Cyc_Absyndump_dump_nospace(start);Cyc_Absyndump_dump_sep_c(
f,env,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_egroup(void(*f)(
void*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep){if(l != 0)Cyc_Absyndump_group(f,l,start,end,sep);}void Cyc_Absyndump_dumpqvar(
struct _tuple0*v){struct Cyc_List_List*_tmp1=0;{void*_tmp2=(*v).f1;struct Cyc_List_List*
_tmp3;struct Cyc_List_List*_tmp4;_LL11: if((int)_tmp2 != 0)goto _LL13;_LL12: _tmp3=0;
goto _LL14;_LL13: if(_tmp2 <= (void*)1?1:*((int*)_tmp2)!= 0)goto _LL15;_tmp3=((
struct Cyc_Absyn_Rel_n_struct*)_tmp2)->f1;_LL14: _tmp1=_tmp3;goto _LL10;_LL15: if(
_tmp2 <= (void*)1?1:*((int*)_tmp2)!= 1)goto _LL10;_tmp4=((struct Cyc_Absyn_Abs_n_struct*)
_tmp2)->f1;_LL16: _tmp1=(Cyc_Absyndump_qvar_to_Cids?Cyc_Absyndump_add_cyc_prefix:
0)?({struct Cyc_List_List*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->hd=Cyc_Absynpp_cyc_stringptr;
_tmp5->tl=_tmp4;_tmp5;}): _tmp4;goto _LL10;_LL10:;}if(_tmp1 != 0){Cyc_Absyndump_dump_str((
struct _tagged_arr*)_tmp1->hd);for(_tmp1=_tmp1->tl;_tmp1 != 0;_tmp1=_tmp1->tl){if(
Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{Cyc_Absyndump_dump_nospace(
_tag_arr("::",sizeof(char),3));}Cyc_Absyndump_dump_nospace(*((struct _tagged_arr*)
_tmp1->hd));}if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_nospace(_tag_arr("_",
sizeof(char),2));else{Cyc_Absyndump_dump_nospace(_tag_arr("::",sizeof(char),3));}
Cyc_Absyndump_dump_nospace(*(*v).f2);}else{Cyc_Absyndump_dump_str((*v).f2);}}
void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict)Cyc_Absyndump_dump(
_tag_arr("restrict",sizeof(char),9));if(tq.q_volatile)Cyc_Absyndump_dump(
_tag_arr("volatile",sizeof(char),9));if(tq.q_const)Cyc_Absyndump_dump(_tag_arr("const",
sizeof(char),6));}void Cyc_Absyndump_dumpscope(void*sc){void*_tmp6=sc;_LL18: if((
int)_tmp6 != 0)goto _LL1A;_LL19: Cyc_Absyndump_dump(_tag_arr("static",sizeof(char),
7));return;_LL1A: if((int)_tmp6 != 2)goto _LL1C;_LL1B: return;_LL1C: if((int)_tmp6 != 
3)goto _LL1E;_LL1D: Cyc_Absyndump_dump(_tag_arr("extern",sizeof(char),7));return;
_LL1E: if((int)_tmp6 != 4)goto _LL20;_LL1F: Cyc_Absyndump_dump(_tag_arr("extern \"C\"",
sizeof(char),11));return;_LL20: if((int)_tmp6 != 1)goto _LL22;_LL21: Cyc_Absyndump_dump(
_tag_arr("abstract",sizeof(char),9));return;_LL22: if((int)_tmp6 != 5)goto _LL17;
_LL23: Cyc_Absyndump_dump(_tag_arr("register",sizeof(char),9));return;_LL17:;}
void Cyc_Absyndump_dumpkind(void*k){void*_tmp7=k;_LL25: if((int)_tmp7 != 0)goto
_LL27;_LL26: Cyc_Absyndump_dump(_tag_arr("A",sizeof(char),2));return;_LL27: if((
int)_tmp7 != 1)goto _LL29;_LL28: Cyc_Absyndump_dump(_tag_arr("M",sizeof(char),2));
return;_LL29: if((int)_tmp7 != 2)goto _LL2B;_LL2A: Cyc_Absyndump_dump(_tag_arr("B",
sizeof(char),2));return;_LL2B: if((int)_tmp7 != 3)goto _LL2D;_LL2C: Cyc_Absyndump_dump(
_tag_arr("R",sizeof(char),2));return;_LL2D: if((int)_tmp7 != 4)goto _LL2F;_LL2E: Cyc_Absyndump_dump(
_tag_arr("E",sizeof(char),2));return;_LL2F: if((int)_tmp7 != 5)goto _LL24;_LL30: Cyc_Absyndump_dump(
_tag_arr("I",sizeof(char),2));return;_LL24:;}void Cyc_Absyndump_dumpaggr_kind(
void*k){void*_tmp8=k;_LL32: if((int)_tmp8 != 0)goto _LL34;_LL33: Cyc_Absyndump_dump(
_tag_arr("struct",sizeof(char),7));return;_LL34: if((int)_tmp8 != 1)goto _LL31;
_LL35: Cyc_Absyndump_dump(_tag_arr("union",sizeof(char),6));return;_LL31:;}void
Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){Cyc_Absyndump_egroup(Cyc_Absyndump_dumptyp,
ts,_tag_arr("<",sizeof(char),2),_tag_arr(">",sizeof(char),2),_tag_arr(",",
sizeof(char),2));}void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);}void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);{void*_tmp9=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpA;void*
_tmpB;_LL37: if(*((int*)_tmp9)!= 0)goto _LL39;_tmpA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp9)->f1;if((int)_tmpA != 2)goto _LL39;_LL38: goto _LL3A;_LL39: if(*((int*)_tmp9)!= 
2)goto _LL3B;_LL3A: goto _LL3C;_LL3B: if(*((int*)_tmp9)!= 1)goto _LL3D;_LL3C: Cyc_Absyndump_dump(
_tag_arr("::?",sizeof(char),4));goto _LL36;_LL3D: if(*((int*)_tmp9)!= 0)goto _LL36;
_tmpB=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp9)->f1;_LL3E: Cyc_Absyndump_dump(
_tag_arr("::",sizeof(char),3));Cyc_Absyndump_dumpkind(_tmpB);goto _LL36;_LL36:;}}
void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){((void(*)(void(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr
end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,tvs,
_tag_arr("<",sizeof(char),2),_tag_arr(">",sizeof(char),2),_tag_arr(",",sizeof(
char),2));}void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){((void(*)(
void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,
tvs,_tag_arr("<",sizeof(char),2),_tag_arr(">",sizeof(char),2),_tag_arr(",",
sizeof(char),2));}struct _tuple8{struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(
struct _tuple8*pr){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(
struct Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumparg,ts,_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(char),
2),_tag_arr(",",sizeof(char),2));}void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*
atts){for(0;atts != 0;atts=atts->tl){void*_tmpC=(void*)atts->hd;_LL40: if((int)
_tmpC != 0)goto _LL42;_LL41: Cyc_Absyndump_dump(_tag_arr("_stdcall",sizeof(char),9));
return;_LL42: if((int)_tmpC != 1)goto _LL44;_LL43: Cyc_Absyndump_dump(_tag_arr("_cdecl",
sizeof(char),7));return;_LL44: if((int)_tmpC != 2)goto _LL46;_LL45: Cyc_Absyndump_dump(
_tag_arr("_fastcall",sizeof(char),10));return;_LL46:;_LL47: goto _LL3F;_LL3F:;}}
void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){int hasatt=0;{struct
Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmpD=(void*)((
struct Cyc_List_List*)_check_null(atts))->hd;_LL49: if((int)_tmpD != 0)goto _LL4B;
_LL4A: goto _LL4C;_LL4B: if((int)_tmpD != 1)goto _LL4D;_LL4C: goto _LL4E;_LL4D: if((int)
_tmpD != 2)goto _LL4F;_LL4E: goto _LL48;_LL4F:;_LL50: hasatt=1;goto _LL48;_LL48:;}}if(
!hasatt)return;Cyc_Absyndump_dump(_tag_arr("__declspec(",sizeof(char),12));for(0;
atts != 0;atts=atts->tl){void*_tmpE=(void*)atts->hd;_LL52: if((int)_tmpE != 0)goto
_LL54;_LL53: goto _LL55;_LL54: if((int)_tmpE != 1)goto _LL56;_LL55: goto _LL57;_LL56:
if((int)_tmpE != 2)goto _LL58;_LL57: goto _LL51;_LL58:;_LL59: Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((
void*)atts->hd));goto _LL51;_LL51:;}Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpatts(
struct Cyc_List_List*atts){if(atts == 0)return;if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dump_noncallconv(
atts);return;}Cyc_Absyndump_dump(_tag_arr(" __attribute__((",sizeof(char),17));
for(0;atts != 0;atts=atts->tl){Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((
void*)atts->hd));if(atts->tl != 0)Cyc_Absyndump_dump(_tag_arr(",",sizeof(char),2));}
Cyc_Absyndump_dump(_tag_arr(")) ",sizeof(char),4));}int Cyc_Absyndump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmpF=(void*)tms->hd;_LL5B: if(
_tmpF <= (void*)1?1:*((int*)_tmpF)!= 1)goto _LL5D;_LL5C: return 1;_LL5D:;_LL5E:
return 0;_LL5A:;}}static void Cyc_Absyndump_dumprgn(void*t){void*_tmp10=Cyc_Tcutil_compress(
t);_LL60: if((int)_tmp10 != 2)goto _LL62;_LL61: Cyc_Absyndump_dump(_tag_arr("`H",
sizeof(char),3));goto _LL5F;_LL62:;_LL63: Cyc_Absyndump_dumpntyp(t);goto _LL5F;
_LL5F:;}struct _tuple9{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static
struct _tuple9 Cyc_Absyndump_effects_split(void*t){struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;{void*_tmp11=Cyc_Tcutil_compress(t);void*_tmp12;
struct Cyc_List_List*_tmp13;_LL65: if(_tmp11 <= (void*)3?1:*((int*)_tmp11)!= 19)
goto _LL67;_tmp12=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp11)->f1;_LL66:
rgions=({struct Cyc_List_List*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->hd=(void*)
_tmp12;_tmp14->tl=rgions;_tmp14;});goto _LL64;_LL67: if(_tmp11 <= (void*)3?1:*((int*)
_tmp11)!= 20)goto _LL69;_tmp13=((struct Cyc_Absyn_JoinEff_struct*)_tmp11)->f1;
_LL68: for(0;_tmp13 != 0;_tmp13=_tmp13->tl){struct Cyc_List_List*_tmp16;struct Cyc_List_List*
_tmp17;struct _tuple9 _tmp15=Cyc_Absyndump_effects_split((void*)_tmp13->hd);_tmp16=
_tmp15.f1;_tmp17=_tmp15.f2;rgions=Cyc_List_imp_append(_tmp16,rgions);effects=Cyc_List_imp_append(
_tmp17,effects);}goto _LL64;_LL69:;_LL6A: effects=({struct Cyc_List_List*_tmp18=
_cycalloc(sizeof(*_tmp18));_tmp18->hd=(void*)t;_tmp18->tl=effects;_tmp18;});goto
_LL64;_LL64:;}return({struct _tuple9 _tmp19;_tmp19.f1=rgions;_tmp19.f2=effects;
_tmp19;});}static void Cyc_Absyndump_dumpeff(void*t){struct Cyc_List_List*_tmp1B;
struct Cyc_List_List*_tmp1C;struct _tuple9 _tmp1A=Cyc_Absyndump_effects_split(t);
_tmp1B=_tmp1A.f1;_tmp1C=_tmp1A.f2;_tmp1B=Cyc_List_imp_rev(_tmp1B);_tmp1C=Cyc_List_imp_rev(
_tmp1C);for(0;_tmp1C != 0;_tmp1C=_tmp1C->tl){Cyc_Absyndump_dumpntyp((void*)_tmp1C->hd);
Cyc_Absyndump_dump_char((int)'+');}Cyc_Absyndump_dump_char((int)'{');for(0;
_tmp1B != 0;_tmp1B=_tmp1B->tl){Cyc_Absyndump_dumprgn((void*)_tmp1B->hd);if(_tmp1B->tl
!= 0)Cyc_Absyndump_dump_char((int)',');}Cyc_Absyndump_dump_char((int)'}');}void
Cyc_Absyndump_dumpntyp(void*t){void*_tmp1D=t;struct Cyc_Absyn_Tvar*_tmp1E;struct
Cyc_Core_Opt*_tmp1F;struct Cyc_Core_Opt*_tmp20;int _tmp21;struct Cyc_Core_Opt*
_tmp22;struct Cyc_Core_Opt*_tmp23;struct Cyc_Core_Opt _tmp24;void*_tmp25;int _tmp26;
struct Cyc_Absyn_TunionInfo _tmp27;void*_tmp28;struct Cyc_List_List*_tmp29;void*
_tmp2A;struct Cyc_Absyn_TunionFieldInfo _tmp2B;void*_tmp2C;struct Cyc_List_List*
_tmp2D;void*_tmp2E;void*_tmp2F;void*_tmp30;void*_tmp31;void*_tmp32;void*_tmp33;
void*_tmp34;void*_tmp35;void*_tmp36;void*_tmp37;void*_tmp38;void*_tmp39;void*
_tmp3A;void*_tmp3B;void*_tmp3C;void*_tmp3D;void*_tmp3E;void*_tmp3F;void*_tmp40;
void*_tmp41;void*_tmp42;void*_tmp43;void*_tmp44;void*_tmp45;void*_tmp46;void*
_tmp47;void*_tmp48;void*_tmp49;void*_tmp4A;void*_tmp4B;int _tmp4C;struct Cyc_List_List*
_tmp4D;struct Cyc_Absyn_AggrInfo _tmp4E;void*_tmp4F;struct Cyc_List_List*_tmp50;
void*_tmp51;struct Cyc_List_List*_tmp52;struct _tuple0*_tmp53;struct Cyc_List_List*
_tmp54;struct _tuple0*_tmp55;struct Cyc_List_List*_tmp56;struct Cyc_Core_Opt*_tmp57;
void*_tmp58;void*_tmp59;int _tmp5A;void*_tmp5B;_LL6C: if(_tmp1D <= (void*)3?1:*((
int*)_tmp1D)!= 7)goto _LL6E;_LL6D: goto _LL6F;_LL6E: if(_tmp1D <= (void*)3?1:*((int*)
_tmp1D)!= 8)goto _LL70;_LL6F: goto _LL71;_LL70: if(_tmp1D <= (void*)3?1:*((int*)
_tmp1D)!= 4)goto _LL72;_LL71: return;_LL72: if((int)_tmp1D != 0)goto _LL74;_LL73: Cyc_Absyndump_dump(
_tag_arr("void",sizeof(char),5));return;_LL74: if(_tmp1D <= (void*)3?1:*((int*)
_tmp1D)!= 1)goto _LL76;_tmp1E=((struct Cyc_Absyn_VarType_struct*)_tmp1D)->f1;_LL75:
Cyc_Absyndump_dump_str(_tmp1E->name);return;_LL76: if(_tmp1D <= (void*)3?1:*((int*)
_tmp1D)!= 0)goto _LL78;_tmp1F=((struct Cyc_Absyn_Evar_struct*)_tmp1D)->f1;_tmp20=((
struct Cyc_Absyn_Evar_struct*)_tmp1D)->f2;if(_tmp20 != 0)goto _LL78;_tmp21=((struct
Cyc_Absyn_Evar_struct*)_tmp1D)->f3;_LL77: Cyc_Absyndump_dump(_tag_arr("%",sizeof(
char),2));if(_tmp1F == 0)Cyc_Absyndump_dump(_tag_arr("?",sizeof(char),2));else{
Cyc_Absyndump_dumpkind((void*)_tmp1F->v);}Cyc_Absyndump_dump((struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp5D;_tmp5D.tag=1;_tmp5D.f1=(int)((unsigned int)
_tmp21);{void*_tmp5C[1]={& _tmp5D};Cyc_Std_aprintf(_tag_arr("(%d)",sizeof(char),5),
_tag_arr(_tmp5C,sizeof(void*),1));}}));return;_LL78: if(_tmp1D <= (void*)3?1:*((
int*)_tmp1D)!= 0)goto _LL7A;_tmp22=((struct Cyc_Absyn_Evar_struct*)_tmp1D)->f1;
_tmp23=((struct Cyc_Absyn_Evar_struct*)_tmp1D)->f2;if(_tmp23 == 0)goto _LL7A;_tmp24=*
_tmp23;_tmp25=(void*)_tmp24.v;_tmp26=((struct Cyc_Absyn_Evar_struct*)_tmp1D)->f3;
_LL79: Cyc_Absyndump_dumpntyp(_tmp25);return;_LL7A: if(_tmp1D <= (void*)3?1:*((int*)
_tmp1D)!= 2)goto _LL7C;_tmp27=((struct Cyc_Absyn_TunionType_struct*)_tmp1D)->f1;
_tmp28=(void*)_tmp27.tunion_info;_tmp29=_tmp27.targs;_tmp2A=(void*)_tmp27.rgn;
_LL7B:{void*_tmp5E=_tmp28;struct Cyc_Absyn_UnknownTunionInfo _tmp5F;struct _tuple0*
_tmp60;int _tmp61;struct Cyc_Absyn_Tuniondecl**_tmp62;struct Cyc_Absyn_Tuniondecl*
_tmp63;struct Cyc_Absyn_Tuniondecl _tmp64;struct _tuple0*_tmp65;int _tmp66;_LLBD: if(*((
int*)_tmp5E)!= 0)goto _LLBF;_tmp5F=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp5E)->f1;
_tmp60=_tmp5F.name;_tmp61=_tmp5F.is_xtunion;_LLBE: _tmp65=_tmp60;_tmp66=_tmp61;
goto _LLC0;_LLBF: if(*((int*)_tmp5E)!= 1)goto _LLBC;_tmp62=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp5E)->f1;_tmp63=*_tmp62;_tmp64=*_tmp63;_tmp65=_tmp64.name;_tmp66=_tmp64.is_xtunion;
_LLC0: if(_tmp66)Cyc_Absyndump_dump(_tag_arr("xtunion ",sizeof(char),9));else{Cyc_Absyndump_dump(
_tag_arr("tunion ",sizeof(char),8));}{void*_tmp67=Cyc_Tcutil_compress(_tmp2A);
_LLC2: if((int)_tmp67 != 2)goto _LLC4;_LLC3: goto _LLC1;_LLC4:;_LLC5: Cyc_Absyndump_dumptyp(
_tmp2A);Cyc_Absyndump_dump(_tag_arr(" ",sizeof(char),2));goto _LLC1;_LLC1:;}Cyc_Absyndump_dumpqvar(
_tmp65);Cyc_Absyndump_dumptps(_tmp29);goto _LLBC;_LLBC:;}goto _LL6B;_LL7C: if(
_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 3)goto _LL7E;_tmp2B=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp1D)->f1;_tmp2C=(void*)_tmp2B.field_info;_tmp2D=_tmp2B.targs;_LL7D:{void*
_tmp68=_tmp2C;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp69;struct _tuple0*_tmp6A;
struct _tuple0*_tmp6B;int _tmp6C;struct Cyc_Absyn_Tuniondecl*_tmp6D;struct Cyc_Absyn_Tuniondecl
_tmp6E;struct _tuple0*_tmp6F;int _tmp70;struct Cyc_Absyn_Tunionfield*_tmp71;struct
Cyc_Absyn_Tunionfield _tmp72;struct _tuple0*_tmp73;_LLC7: if(*((int*)_tmp68)!= 0)
goto _LLC9;_tmp69=((struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp68)->f1;_tmp6A=
_tmp69.tunion_name;_tmp6B=_tmp69.field_name;_tmp6C=_tmp69.is_xtunion;_LLC8:
_tmp6F=_tmp6A;_tmp70=_tmp6C;_tmp73=_tmp6B;goto _LLCA;_LLC9: if(*((int*)_tmp68)!= 1)
goto _LLC6;_tmp6D=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp68)->f1;_tmp6E=*
_tmp6D;_tmp6F=_tmp6E.name;_tmp70=_tmp6E.is_xtunion;_tmp71=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp68)->f2;_tmp72=*_tmp71;_tmp73=_tmp72.name;_LLCA: if(_tmp70)Cyc_Absyndump_dump(
_tag_arr("xtunion ",sizeof(char),9));else{Cyc_Absyndump_dump(_tag_arr("tunion ",
sizeof(char),8));}Cyc_Absyndump_dumpqvar(_tmp6F);Cyc_Absyndump_dump(_tag_arr(".",
sizeof(char),2));Cyc_Absyndump_dumpqvar(_tmp73);Cyc_Absyndump_dumptps(_tmp2D);
goto _LLC6;_LLC6:;}goto _LL6B;_LL7E: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)
goto _LL80;_tmp2E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)
_tmp2E != 2)goto _LL80;_tmp2F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;
if((int)_tmp2F != 2)goto _LL80;_LL7F: goto _LL81;_LL80: if(_tmp1D <= (void*)3?1:*((int*)
_tmp1D)!= 5)goto _LL82;_tmp30=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;
if((int)_tmp30 != 0)goto _LL82;_tmp31=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp31 != 2)goto _LL82;_LL81: Cyc_Absyndump_dump(_tag_arr("int",
sizeof(char),4));return;_LL82: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto
_LL84;_tmp32=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp32
!= 2)goto _LL84;_tmp33=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((
int)_tmp33 != 0)goto _LL84;_LL83: Cyc_Absyndump_dump(_tag_arr("char",sizeof(char),5));
return;_LL84: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto _LL86;_tmp34=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp34 != 0)goto _LL86;_tmp35=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((int)_tmp35 != 0)goto _LL86;
_LL85: Cyc_Absyndump_dump(_tag_arr("signed char",sizeof(char),12));return;_LL86:
if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto _LL88;_tmp36=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f1;if((int)_tmp36 != 1)goto _LL88;_tmp37=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp37 != 0)goto _LL88;_LL87: Cyc_Absyndump_dump(_tag_arr("unsigned char",
sizeof(char),14));return;_LL88: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto
_LL8A;_tmp38=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp38
!= 2)goto _LL8A;_tmp39=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((
int)_tmp39 != 1)goto _LL8A;_LL89: goto _LL8B;_LL8A: if(_tmp1D <= (void*)3?1:*((int*)
_tmp1D)!= 5)goto _LL8C;_tmp3A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;
if((int)_tmp3A != 0)goto _LL8C;_tmp3B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp3B != 1)goto _LL8C;_LL8B: Cyc_Absyndump_dump(_tag_arr("short",
sizeof(char),6));return;_LL8C: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto
_LL8E;_tmp3C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp3C
!= 1)goto _LL8E;_tmp3D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((
int)_tmp3D != 1)goto _LL8E;_LL8D: Cyc_Absyndump_dump(_tag_arr("unsigned short",
sizeof(char),15));return;_LL8E: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto
_LL90;_tmp3E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp3E
!= 1)goto _LL90;_tmp3F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((
int)_tmp3F != 2)goto _LL90;_LL8F: Cyc_Absyndump_dump(_tag_arr("unsigned int",
sizeof(char),13));return;_LL90: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto
_LL92;_tmp40=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp40
!= 2)goto _LL92;_tmp41=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((
int)_tmp41 != 3)goto _LL92;if(!Cyc_Absyndump_to_VC)goto _LL92;_LL91: goto _LL93;_LL92:
if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto _LL94;_tmp42=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f1;if((int)_tmp42 != 0)goto _LL94;_tmp43=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp43 != 3)goto _LL94;if(!Cyc_Absyndump_to_VC)goto _LL94;_LL93:
Cyc_Absyndump_dump(_tag_arr("__int64",sizeof(char),8));return;_LL94: if(_tmp1D <= (
void*)3?1:*((int*)_tmp1D)!= 5)goto _LL96;_tmp44=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f1;if((int)_tmp44 != 2)goto _LL96;_tmp45=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp45 != 3)goto _LL96;_LL95: goto _LL97;_LL96: if(_tmp1D <= (void*)
3?1:*((int*)_tmp1D)!= 5)goto _LL98;_tmp46=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f1;if((int)_tmp46 != 0)goto _LL98;_tmp47=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp47 != 3)goto _LL98;_LL97: Cyc_Absyndump_dump(_tag_arr("long long",
sizeof(char),10));return;_LL98: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto
_LL9A;_tmp48=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp48
!= 1)goto _LL9A;_tmp49=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((
int)_tmp49 != 3)goto _LL9A;if(!Cyc_Absyndump_to_VC)goto _LL9A;_LL99: Cyc_Absyndump_dump(
_tag_arr("unsigned __int64",sizeof(char),17));return;_LL9A: if(_tmp1D <= (void*)3?
1:*((int*)_tmp1D)!= 5)goto _LL9C;_tmp4A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f1;if((int)_tmp4A != 1)goto _LL9C;_tmp4B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp4B != 3)goto _LL9C;_LL9B: Cyc_Absyndump_dump(_tag_arr("unsigned long long",
sizeof(char),19));return;_LL9C: if((int)_tmp1D != 1)goto _LL9E;_LL9D: Cyc_Absyndump_dump(
_tag_arr("float",sizeof(char),6));return;_LL9E: if(_tmp1D <= (void*)3?1:*((int*)
_tmp1D)!= 6)goto _LLA0;_tmp4C=((struct Cyc_Absyn_DoubleType_struct*)_tmp1D)->f1;
_LL9F: if(_tmp4C)Cyc_Absyndump_dump(_tag_arr("long double",sizeof(char),12));
else{Cyc_Absyndump_dump(_tag_arr("double",sizeof(char),7));}return;_LLA0: if(
_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 9)goto _LLA2;_tmp4D=((struct Cyc_Absyn_TupleType_struct*)
_tmp1D)->f1;_LLA1: Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(
_tmp4D);return;_LLA2: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 10)goto _LLA4;
_tmp4E=((struct Cyc_Absyn_AggrType_struct*)_tmp1D)->f1;_tmp4F=(void*)_tmp4E.aggr_info;
_tmp50=_tmp4E.targs;_LLA3: {void*_tmp75;struct _tuple0*_tmp76;struct _tuple3 _tmp74=
Cyc_Absyn_aggr_kinded_name(_tmp4F);_tmp75=_tmp74.f1;_tmp76=_tmp74.f2;Cyc_Absyndump_dumpaggr_kind(
_tmp75);Cyc_Absyndump_dumpqvar(_tmp76);Cyc_Absyndump_dumptps(_tmp50);return;}
_LLA4: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 11)goto _LLA6;_tmp51=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp1D)->f1;_tmp52=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1D)->f2;_LLA5: Cyc_Absyndump_dumpaggr_kind(_tmp51);Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpaggrfields(_tmp52);Cyc_Absyndump_dump_char((int)'}');
return;_LLA6: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 12)goto _LLA8;_tmp53=((
struct Cyc_Absyn_EnumType_struct*)_tmp1D)->f1;_LLA7: Cyc_Absyndump_dump(_tag_arr("enum ",
sizeof(char),6));Cyc_Absyndump_dumpqvar(_tmp53);return;_LLA8: if(_tmp1D <= (void*)
3?1:*((int*)_tmp1D)!= 13)goto _LLAA;_tmp54=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1D)->f1;_LLA9: Cyc_Absyndump_dump(_tag_arr("enum {",sizeof(char),7));Cyc_Absyndump_dumpenumfields(
_tmp54);Cyc_Absyndump_dump(_tag_arr("}",sizeof(char),2));return;_LLAA: if(_tmp1D
<= (void*)3?1:*((int*)_tmp1D)!= 16)goto _LLAC;_tmp55=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1D)->f1;_tmp56=((struct Cyc_Absyn_TypedefType_struct*)_tmp1D)->f2;_tmp57=((
struct Cyc_Absyn_TypedefType_struct*)_tmp1D)->f4;_LLAB:(Cyc_Absyndump_dumpqvar(
_tmp55),Cyc_Absyndump_dumptps(_tmp56));return;_LLAC: if(_tmp1D <= (void*)3?1:*((
int*)_tmp1D)!= 14)goto _LLAE;_tmp58=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp1D)->f1;_LLAD: Cyc_Absyndump_dump(_tag_arr("sizeof_t<",sizeof(char),10));Cyc_Absyndump_dumpntyp(
_tmp58);Cyc_Absyndump_dump(_tag_arr(">",sizeof(char),2));return;_LLAE: if(_tmp1D
<= (void*)3?1:*((int*)_tmp1D)!= 15)goto _LLB0;_tmp59=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp1D)->f1;_LLAF: Cyc_Absyndump_dumprgn(_tmp59);return;_LLB0: if(_tmp1D <= (void*)
3?1:*((int*)_tmp1D)!= 18)goto _LLB2;_tmp5A=((struct Cyc_Absyn_TypeInt_struct*)
_tmp1D)->f1;_LLB1: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp78;_tmp78.tag=1;_tmp78.f1=(int)((unsigned int)_tmp5A);{void*_tmp77[1]={&
_tmp78};Cyc_Std_aprintf(_tag_arr("`%d",sizeof(char),4),_tag_arr(_tmp77,sizeof(
void*),1));}}));return;_LLB2: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 17)goto
_LLB4;_tmp5B=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp1D)->f1;_LLB3: Cyc_Absyndump_dump(
_tag_arr("tag_t<",sizeof(char),7));Cyc_Absyndump_dumpntyp(_tmp5B);Cyc_Absyndump_dump(
_tag_arr(">",sizeof(char),2));return;_LLB4: if((int)_tmp1D != 2)goto _LLB6;_LLB5:
goto _LLB7;_LLB6: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 19)goto _LLB8;_LLB7: goto
_LLB9;_LLB8: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 21)goto _LLBA;_LLB9: goto
_LLBB;_LLBA: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 20)goto _LL6B;_LLBB: return;
_LL6B:;}void Cyc_Absyndump_dumpvaropt(struct Cyc_Core_Opt*vo){if(vo != 0)Cyc_Absyndump_dump_str((
struct _tagged_arr*)vo->v);}void Cyc_Absyndump_dumpfunarg(struct _tuple1*t){((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),struct Cyc_Core_Opt*))
Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}void
Cyc_Absyndump_dump_rgncmp(struct _tuple7*cmp){struct _tuple7 _tmp7A;void*_tmp7B;
void*_tmp7C;struct _tuple7*_tmp79=cmp;_tmp7A=*_tmp79;_tmp7B=_tmp7A.f1;_tmp7C=
_tmp7A.f2;Cyc_Absyndump_dumpeff(_tmp7B);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(
_tmp7C);}void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){((void(*)(void(*
f)(struct _tuple7*),struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dump_rgncmp,rgn_po,_tag_arr(",",sizeof(char),2));}void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((
int)'(');for(0;args != 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple1*)
args->hd);if((args->tl != 0?1: c_varargs)?1: cyc_varargs != 0)Cyc_Absyndump_dump_char((
int)',');}if(c_varargs)Cyc_Absyndump_dump(_tag_arr("...",sizeof(char),4));else{
if(cyc_varargs != 0){struct _tuple1*_tmp7D=({struct _tuple1*_tmp7E=_cycalloc(
sizeof(*_tmp7E));_tmp7E->f1=cyc_varargs->name;_tmp7E->f2=cyc_varargs->tq;_tmp7E->f3=(
void*)cyc_varargs->type;_tmp7E;});Cyc_Absyndump_dump(_tag_arr("...",sizeof(char),
4));if(cyc_varargs->inject)Cyc_Absyndump_dump(_tag_arr(" inject ",sizeof(char),9));
Cyc_Absyndump_dumpfunarg(_tmp7D);}}if(effopt != 0){Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpeff((
void*)effopt->v);}if(rgn_po != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(
rgn_po);}Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumptyp(void*t){((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _tmp7F;_tmp7F.q_const=0;_tmp7F.q_volatile=0;_tmp7F.q_restrict=
0;_tmp7F;}),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpdesignator(
void*d){void*_tmp80=d;struct Cyc_Absyn_Exp*_tmp81;struct _tagged_arr*_tmp82;_LLCC:
if(*((int*)_tmp80)!= 0)goto _LLCE;_tmp81=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp80)->f1;_LLCD: Cyc_Absyndump_dump(_tag_arr(".[",sizeof(char),3));Cyc_Absyndump_dumpexp(
_tmp81);Cyc_Absyndump_dump_char((int)']');goto _LLCB;_LLCE: if(*((int*)_tmp80)!= 1)
goto _LLCB;_tmp82=((struct Cyc_Absyn_FieldName_struct*)_tmp80)->f1;_LLCF: Cyc_Absyndump_dump_char((
int)'.');Cyc_Absyndump_dump_nospace(*_tmp82);goto _LLCB;_LLCB:;}struct _tuple10{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};void Cyc_Absyndump_dumpde(struct
_tuple10*de){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*de).f1,_tag_arr("",
sizeof(char),1),_tag_arr("=",sizeof(char),2),_tag_arr("=",sizeof(char),2));Cyc_Absyndump_dumpexp((*
de).f2);}void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){((
void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec,inprec,es,_tag_arr("",sizeof(char),1),_tag_arr("",
sizeof(char),1),_tag_arr(",",sizeof(char),2));}void Cyc_Absyndump_dumpexp_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= 
myprec)Cyc_Absyndump_dump_nospace(_tag_arr("(",sizeof(char),2));{void*_tmp83=(
void*)e->r;void*_tmp84;void*_tmp85;char _tmp86;void*_tmp87;void*_tmp88;short
_tmp89;void*_tmp8A;void*_tmp8B;int _tmp8C;void*_tmp8D;void*_tmp8E;int _tmp8F;void*
_tmp90;void*_tmp91;int _tmp92;void*_tmp93;void*_tmp94;long long _tmp95;void*_tmp96;
struct _tagged_arr _tmp97;void*_tmp98;void*_tmp99;struct _tagged_arr _tmp9A;struct
_tuple0*_tmp9B;struct _tuple0*_tmp9C;void*_tmp9D;struct Cyc_List_List*_tmp9E;
struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Core_Opt*_tmpA0;struct Cyc_Absyn_Exp*_tmpA1;
struct Cyc_Absyn_Exp*_tmpA2;void*_tmpA3;struct Cyc_Absyn_Exp*_tmpA4;void*_tmpA5;
struct Cyc_Absyn_Exp*_tmpA6;void*_tmpA7;struct Cyc_Absyn_Exp*_tmpA8;void*_tmpA9;
struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Exp*_tmpAB;struct Cyc_Absyn_Exp*_tmpAC;
struct Cyc_Absyn_Exp*_tmpAD;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_Absyn_Exp*_tmpAF;
struct Cyc_List_List*_tmpB0;struct Cyc_Absyn_Exp*_tmpB1;struct Cyc_List_List*_tmpB2;
struct Cyc_Absyn_Exp*_tmpB3;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;
void*_tmpB6;struct Cyc_Absyn_Exp*_tmpB7;struct Cyc_Absyn_Exp*_tmpB8;struct Cyc_Absyn_Exp*
_tmpB9;struct Cyc_Absyn_Exp*_tmpBA;void*_tmpBB;struct Cyc_Absyn_Exp*_tmpBC;void*
_tmpBD;void*_tmpBE;struct _tagged_arr*_tmpBF;void*_tmpC0;void*_tmpC1;unsigned int
_tmpC2;struct Cyc_List_List*_tmpC3;void*_tmpC4;struct Cyc_Absyn_Exp*_tmpC5;struct
Cyc_Absyn_Exp*_tmpC6;struct _tagged_arr*_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;struct
_tagged_arr*_tmpC9;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_Absyn_Exp*_tmpCB;struct
Cyc_List_List*_tmpCC;struct _tuple1*_tmpCD;struct Cyc_List_List*_tmpCE;struct Cyc_List_List*
_tmpCF;struct Cyc_Absyn_Vardecl*_tmpD0;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_Absyn_Exp*
_tmpD2;struct _tuple0*_tmpD3;struct Cyc_List_List*_tmpD4;struct Cyc_List_List*
_tmpD5;struct Cyc_List_List*_tmpD6;struct Cyc_List_List*_tmpD7;struct Cyc_Absyn_Tunionfield*
_tmpD8;struct _tuple0*_tmpD9;struct _tuple0*_tmpDA;struct Cyc_Absyn_MallocInfo
_tmpDB;int _tmpDC;struct Cyc_Absyn_Exp*_tmpDD;void**_tmpDE;struct Cyc_Absyn_Exp*
_tmpDF;struct Cyc_Core_Opt*_tmpE0;struct Cyc_List_List*_tmpE1;struct Cyc_Absyn_Stmt*
_tmpE2;struct Cyc_Absyn_Fndecl*_tmpE3;struct Cyc_Absyn_Exp*_tmpE4;_LLD1: if(*((int*)
_tmp83)!= 0)goto _LLD3;_tmp84=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp83)->f1;
if(_tmp84 <= (void*)1?1:*((int*)_tmp84)!= 0)goto _LLD3;_tmp85=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp84)->f1;_tmp86=((struct Cyc_Absyn_Char_c_struct*)_tmp84)->f2;_LLD2: Cyc_Absyndump_dump_char((
int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp86));Cyc_Absyndump_dump_char((
int)'\'');goto _LLD0;_LLD3: if(*((int*)_tmp83)!= 0)goto _LLD5;_tmp87=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp83)->f1;if(_tmp87 <= (void*)1?1:*((int*)
_tmp87)!= 1)goto _LLD5;_tmp88=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp87)->f1;
_tmp89=((struct Cyc_Absyn_Short_c_struct*)_tmp87)->f2;_LLD4: Cyc_Absyndump_dump((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpE6;_tmpE6.tag=1;_tmpE6.f1=(
int)((unsigned int)((int)_tmp89));{void*_tmpE5[1]={& _tmpE6};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(char),3),_tag_arr(_tmpE5,sizeof(void*),1));}}));goto _LLD0;
_LLD5: if(*((int*)_tmp83)!= 0)goto _LLD7;_tmp8A=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp83)->f1;if(_tmp8A <= (void*)1?1:*((int*)_tmp8A)!= 2)goto _LLD7;_tmp8B=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp8A)->f1;if((int)_tmp8B != 2)goto _LLD7;_tmp8C=((
struct Cyc_Absyn_Int_c_struct*)_tmp8A)->f2;_LLD6: _tmp8F=_tmp8C;goto _LLD8;_LLD7:
if(*((int*)_tmp83)!= 0)goto _LLD9;_tmp8D=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp83)->f1;if(_tmp8D <= (void*)1?1:*((int*)_tmp8D)!= 2)goto _LLD9;_tmp8E=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp8D)->f1;if((int)_tmp8E != 0)goto _LLD9;_tmp8F=((
struct Cyc_Absyn_Int_c_struct*)_tmp8D)->f2;_LLD8: Cyc_Absyndump_dump((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpE8;_tmpE8.tag=1;_tmpE8.f1=(int)((
unsigned int)_tmp8F);{void*_tmpE7[1]={& _tmpE8};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(char),3),_tag_arr(_tmpE7,sizeof(void*),1));}}));goto _LLD0;_LLD9: if(*((int*)
_tmp83)!= 0)goto _LLDB;_tmp90=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp83)->f1;
if(_tmp90 <= (void*)1?1:*((int*)_tmp90)!= 2)goto _LLDB;_tmp91=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp90)->f1;if((int)_tmp91 != 1)goto _LLDB;_tmp92=((struct Cyc_Absyn_Int_c_struct*)
_tmp90)->f2;_LLDA: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmpEA;_tmpEA.tag=1;_tmpEA.f1=(unsigned int)_tmp92;{void*_tmpE9[1]={& _tmpEA};Cyc_Std_aprintf(
_tag_arr("%u",sizeof(char),3),_tag_arr(_tmpE9,sizeof(void*),1));}}));goto _LLD0;
_LLDB: if(*((int*)_tmp83)!= 0)goto _LLDD;_tmp93=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp83)->f1;if(_tmp93 <= (void*)1?1:*((int*)_tmp93)!= 3)goto _LLDD;_tmp94=(void*)((
struct Cyc_Absyn_LongLong_c_struct*)_tmp93)->f1;_tmp95=((struct Cyc_Absyn_LongLong_c_struct*)
_tmp93)->f2;_LLDC: Cyc_Absyndump_dump(_tag_arr("<<FIX LONG LONG CONSTANT>>",
sizeof(char),27));goto _LLD0;_LLDD: if(*((int*)_tmp83)!= 0)goto _LLDF;_tmp96=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp83)->f1;if(_tmp96 <= (void*)1?1:*((int*)
_tmp96)!= 4)goto _LLDF;_tmp97=((struct Cyc_Absyn_Float_c_struct*)_tmp96)->f1;_LLDE:
Cyc_Absyndump_dump(_tmp97);goto _LLD0;_LLDF: if(*((int*)_tmp83)!= 0)goto _LLE1;
_tmp98=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp83)->f1;if((int)_tmp98 != 0)
goto _LLE1;_LLE0: Cyc_Absyndump_dump(_tag_arr("NULL",sizeof(char),5));goto _LLD0;
_LLE1: if(*((int*)_tmp83)!= 0)goto _LLE3;_tmp99=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp83)->f1;if(_tmp99 <= (void*)1?1:*((int*)_tmp99)!= 5)goto _LLE3;_tmp9A=((struct
Cyc_Absyn_String_c_struct*)_tmp99)->f1;_LLE2: Cyc_Absyndump_dump_char((int)'"');
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp9A));Cyc_Absyndump_dump_char((
int)'"');goto _LLD0;_LLE3: if(*((int*)_tmp83)!= 2)goto _LLE5;_tmp9B=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp83)->f1;_LLE4: _tmp9C=_tmp9B;goto _LLE6;_LLE5: if(*((int*)_tmp83)!= 1)goto _LLE7;
_tmp9C=((struct Cyc_Absyn_Var_e_struct*)_tmp83)->f1;_LLE6: Cyc_Absyndump_dumpqvar(
_tmp9C);goto _LLD0;_LLE7: if(*((int*)_tmp83)!= 3)goto _LLE9;_tmp9D=(void*)((struct
Cyc_Absyn_Primop_e_struct*)_tmp83)->f1;_tmp9E=((struct Cyc_Absyn_Primop_e_struct*)
_tmp83)->f2;_LLE8: {struct _tagged_arr _tmpEB=Cyc_Absynpp_prim2str(_tmp9D);switch(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9E)){case 1: _LL135: if(_tmp9D == (
void*)19){Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp9E))->hd);Cyc_Absyndump_dump(_tag_arr(".size",sizeof(char),6));}
else{Cyc_Absyndump_dump(_tmpEB);Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp9E))->hd);}break;case 2: _LL136: Cyc_Absyndump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9E))->hd);
Cyc_Absyndump_dump(_tmpEB);Cyc_Absyndump_dump_char((int)' ');Cyc_Absyndump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9E->tl))->hd);
break;default: _LL137:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpEC=
_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=({struct Cyc_Core_Failure_struct _tmpED;
_tmpED.tag=Cyc_Core_Failure;_tmpED.f1=_tag_arr("Absyndump -- Bad number of arguments to primop",
sizeof(char),47);_tmpED;});_tmpEC;}));}goto _LLD0;}_LLE9: if(*((int*)_tmp83)!= 4)
goto _LLEB;_tmp9F=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f1;_tmpA0=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f2;_tmpA1=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp83)->f3;_LLEA: Cyc_Absyndump_dumpexp_prec(myprec,_tmp9F);if(_tmpA0 != 0)Cyc_Absyndump_dump(
Cyc_Absynpp_prim2str((void*)_tmpA0->v));Cyc_Absyndump_dump_nospace(_tag_arr("=",
sizeof(char),2));Cyc_Absyndump_dumpexp_prec(myprec,_tmpA1);goto _LLD0;_LLEB: if(*((
int*)_tmp83)!= 5)goto _LLED;_tmpA2=((struct Cyc_Absyn_Increment_e_struct*)_tmp83)->f1;
_tmpA3=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp83)->f2;if((int)_tmpA3 != 
0)goto _LLED;_LLEC: Cyc_Absyndump_dump(_tag_arr("++",sizeof(char),3));Cyc_Absyndump_dumpexp_prec(
myprec,_tmpA2);goto _LLD0;_LLED: if(*((int*)_tmp83)!= 5)goto _LLEF;_tmpA4=((struct
Cyc_Absyn_Increment_e_struct*)_tmp83)->f1;_tmpA5=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp83)->f2;if((int)_tmpA5 != 2)goto _LLEF;_LLEE: Cyc_Absyndump_dump(_tag_arr("--",
sizeof(char),3));Cyc_Absyndump_dumpexp_prec(myprec,_tmpA4);goto _LLD0;_LLEF: if(*((
int*)_tmp83)!= 5)goto _LLF1;_tmpA6=((struct Cyc_Absyn_Increment_e_struct*)_tmp83)->f1;
_tmpA7=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp83)->f2;if((int)_tmpA7 != 
1)goto _LLF1;_LLF0: Cyc_Absyndump_dumpexp_prec(myprec,_tmpA6);Cyc_Absyndump_dump(
_tag_arr("++",sizeof(char),3));goto _LLD0;_LLF1: if(*((int*)_tmp83)!= 5)goto _LLF3;
_tmpA8=((struct Cyc_Absyn_Increment_e_struct*)_tmp83)->f1;_tmpA9=(void*)((struct
Cyc_Absyn_Increment_e_struct*)_tmp83)->f2;if((int)_tmpA9 != 3)goto _LLF3;_LLF2: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpA8);Cyc_Absyndump_dump(_tag_arr("--",sizeof(char),3));goto _LLD0;_LLF3:
if(*((int*)_tmp83)!= 6)goto _LLF5;_tmpAA=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp83)->f1;_tmpAB=((struct Cyc_Absyn_Conditional_e_struct*)_tmp83)->f2;_tmpAC=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp83)->f3;_LLF4: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpAA);Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,
_tmpAB);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,
_tmpAC);goto _LLD0;_LLF5: if(*((int*)_tmp83)!= 7)goto _LLF7;_tmpAD=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp83)->f1;_tmpAE=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp83)->f2;_LLF6: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumpexp_prec(myprec,_tmpAD);Cyc_Absyndump_dump_char((int)',');
Cyc_Absyndump_dumpexp_prec(myprec,_tmpAE);Cyc_Absyndump_dump_char((int)')');goto
_LLD0;_LLF7: if(*((int*)_tmp83)!= 8)goto _LLF9;_tmpAF=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp83)->f1;_tmpB0=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp83)->f2;_LLF8:
_tmpB1=_tmpAF;_tmpB2=_tmpB0;goto _LLFA;_LLF9: if(*((int*)_tmp83)!= 9)goto _LLFB;
_tmpB1=((struct Cyc_Absyn_FnCall_e_struct*)_tmp83)->f1;_tmpB2=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp83)->f2;_LLFA: Cyc_Absyndump_dumpexp_prec(myprec,_tmpB1);Cyc_Absyndump_dump_nospace(
_tag_arr("(",sizeof(char),2));Cyc_Absyndump_dumpexps_prec(20,_tmpB2);Cyc_Absyndump_dump_nospace(
_tag_arr(")",sizeof(char),2));goto _LLD0;_LLFB: if(*((int*)_tmp83)!= 10)goto _LLFD;
_tmpB3=((struct Cyc_Absyn_Throw_e_struct*)_tmp83)->f1;_LLFC: Cyc_Absyndump_dump(
_tag_arr("throw",sizeof(char),6));Cyc_Absyndump_dumpexp_prec(myprec,_tmpB3);goto
_LLD0;_LLFD: if(*((int*)_tmp83)!= 11)goto _LLFF;_tmpB4=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp83)->f1;_LLFE: _tmpB5=_tmpB4;goto _LL100;_LLFF: if(*((int*)_tmp83)!= 12)goto
_LL101;_tmpB5=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp83)->f1;_LL100: Cyc_Absyndump_dumpexp_prec(
inprec,_tmpB5);goto _LLD0;_LL101: if(*((int*)_tmp83)!= 13)goto _LL103;_tmpB6=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp83)->f1;_tmpB7=((struct Cyc_Absyn_Cast_e_struct*)
_tmp83)->f2;_LL102: Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(
_tmpB6);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,
_tmpB7);goto _LLD0;_LL103: if(*((int*)_tmp83)!= 14)goto _LL105;_tmpB8=((struct Cyc_Absyn_Address_e_struct*)
_tmp83)->f1;_LL104: Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(
myprec,_tmpB8);goto _LLD0;_LL105: if(*((int*)_tmp83)!= 15)goto _LL107;_tmpB9=((
struct Cyc_Absyn_New_e_struct*)_tmp83)->f1;_tmpBA=((struct Cyc_Absyn_New_e_struct*)
_tmp83)->f2;_LL106: Cyc_Absyndump_dump(_tag_arr("new ",sizeof(char),5));Cyc_Absyndump_dumpexp_prec(
myprec,_tmpBA);goto _LLD0;_LL107: if(*((int*)_tmp83)!= 16)goto _LL109;_tmpBB=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp83)->f1;_LL108: Cyc_Absyndump_dump(
_tag_arr("sizeof(",sizeof(char),8));Cyc_Absyndump_dumptyp(_tmpBB);Cyc_Absyndump_dump_char((
int)')');goto _LLD0;_LL109: if(*((int*)_tmp83)!= 17)goto _LL10B;_tmpBC=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp83)->f1;_LL10A: Cyc_Absyndump_dump(_tag_arr("sizeof(",sizeof(char),8));Cyc_Absyndump_dumpexp(
_tmpBC);Cyc_Absyndump_dump_char((int)')');goto _LLD0;_LL10B: if(*((int*)_tmp83)!= 
18)goto _LL10D;_tmpBD=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp83)->f1;
_tmpBE=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp83)->f2;if(*((int*)_tmpBE)
!= 0)goto _LL10D;_tmpBF=((struct Cyc_Absyn_StructField_struct*)_tmpBE)->f1;_LL10C:
Cyc_Absyndump_dump(_tag_arr("offsetof(",sizeof(char),10));Cyc_Absyndump_dumptyp(
_tmpBD);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmpBF);
Cyc_Absyndump_dump_char((int)')');goto _LLD0;_LL10D: if(*((int*)_tmp83)!= 18)goto
_LL10F;_tmpC0=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp83)->f1;_tmpC1=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp83)->f2;if(*((int*)_tmpC1)!= 1)
goto _LL10F;_tmpC2=((struct Cyc_Absyn_TupleIndex_struct*)_tmpC1)->f1;_LL10E: Cyc_Absyndump_dump(
_tag_arr("offsetof(",sizeof(char),10));Cyc_Absyndump_dumptyp(_tmpC0);Cyc_Absyndump_dump_char((
int)',');Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmpEF;_tmpEF.tag=1;_tmpEF.f1=(int)_tmpC2;{void*_tmpEE[1]={& _tmpEF};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(char),3),_tag_arr(_tmpEE,sizeof(void*),1));}}));Cyc_Absyndump_dump_char((
int)')');goto _LLD0;_LL10F: if(*((int*)_tmp83)!= 19)goto _LL111;_tmpC3=((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp83)->f1;_tmpC4=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp83)->f2;_LL110:
Cyc_Absyndump_dump(_tag_arr("__gen(",sizeof(char),7));Cyc_Absyndump_dumptvars(
_tmpC3);Cyc_Absyndump_dumptyp(_tmpC4);Cyc_Absyndump_dump_char((int)')');goto
_LLD0;_LL111: if(*((int*)_tmp83)!= 20)goto _LL113;_tmpC5=((struct Cyc_Absyn_Deref_e_struct*)
_tmp83)->f1;_LL112: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(
myprec,_tmpC5);goto _LLD0;_LL113: if(*((int*)_tmp83)!= 21)goto _LL115;_tmpC6=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp83)->f1;_tmpC7=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp83)->f2;_LL114: Cyc_Absyndump_dumpexp_prec(myprec,_tmpC6);Cyc_Absyndump_dump_char((
int)'.');Cyc_Absyndump_dump_nospace(*_tmpC7);goto _LLD0;_LL115: if(*((int*)_tmp83)
!= 22)goto _LL117;_tmpC8=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp83)->f1;_tmpC9=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp83)->f2;_LL116: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpC8);Cyc_Absyndump_dump_nospace(_tag_arr("->",sizeof(char),3));Cyc_Absyndump_dump_nospace(*
_tmpC9);goto _LLD0;_LL117: if(*((int*)_tmp83)!= 23)goto _LL119;_tmpCA=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp83)->f1;_tmpCB=((struct Cyc_Absyn_Subscript_e_struct*)_tmp83)->f2;_LL118: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpCA);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmpCB);
Cyc_Absyndump_dump_char((int)']');goto _LLD0;_LL119: if(*((int*)_tmp83)!= 24)goto
_LL11B;_tmpCC=((struct Cyc_Absyn_Tuple_e_struct*)_tmp83)->f1;_LL11A: Cyc_Absyndump_dump(
_tag_arr("$(",sizeof(char),3));Cyc_Absyndump_dumpexps_prec(20,_tmpCC);Cyc_Absyndump_dump_char((
int)')');goto _LLD0;_LL11B: if(*((int*)_tmp83)!= 25)goto _LL11D;_tmpCD=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp83)->f1;_tmpCE=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp83)->f2;_LL11C:
Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp((*_tmpCD).f3);Cyc_Absyndump_dump_char((
int)')');((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmpCE,_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(
char),2),_tag_arr(",",sizeof(char),2));goto _LLD0;_LL11D: if(*((int*)_tmp83)!= 26)
goto _LL11F;_tmpCF=((struct Cyc_Absyn_Array_e_struct*)_tmp83)->f1;_LL11E:((void(*)(
void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmpCF,_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(char),2),_tag_arr(",",
sizeof(char),2));goto _LLD0;_LL11F: if(*((int*)_tmp83)!= 27)goto _LL121;_tmpD0=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp83)->f1;_tmpD1=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp83)->f2;_tmpD2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp83)->f3;_LL120:
Cyc_Absyndump_dump(_tag_arr("new {for",sizeof(char),9));Cyc_Absyndump_dump_str((*
_tmpD0->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmpD1);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmpD2);Cyc_Absyndump_dump_char((
int)'}');goto _LLD0;_LL121: if(*((int*)_tmp83)!= 28)goto _LL123;_tmpD3=((struct Cyc_Absyn_Struct_e_struct*)
_tmp83)->f1;_tmpD4=((struct Cyc_Absyn_Struct_e_struct*)_tmp83)->f2;_tmpD5=((
struct Cyc_Absyn_Struct_e_struct*)_tmp83)->f3;_LL122: Cyc_Absyndump_dumpqvar(
_tmpD3);Cyc_Absyndump_dump_char((int)'{');if(_tmpD4 != 0)Cyc_Absyndump_dumptps(
_tmpD4);((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmpD5,_tag_arr("",sizeof(char),1),_tag_arr("}",sizeof(char),
2),_tag_arr(",",sizeof(char),2));goto _LLD0;_LL123: if(*((int*)_tmp83)!= 29)goto
_LL125;_tmpD6=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp83)->f2;_LL124:((void(*)(
void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmpD6,_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(char),2),_tag_arr(",",
sizeof(char),2));goto _LLD0;_LL125: if(*((int*)_tmp83)!= 30)goto _LL127;_tmpD7=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp83)->f1;_tmpD8=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp83)->f3;_LL126: Cyc_Absyndump_dumpqvar(_tmpD8->name);if(_tmpD7 != 0)((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,
_tmpD7,_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(char),2),_tag_arr(",",
sizeof(char),2));goto _LLD0;_LL127: if(*((int*)_tmp83)!= 31)goto _LL129;_tmpD9=((
struct Cyc_Absyn_Enum_e_struct*)_tmp83)->f1;_LL128: Cyc_Absyndump_dumpqvar(_tmpD9);
goto _LLD0;_LL129: if(*((int*)_tmp83)!= 32)goto _LL12B;_tmpDA=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp83)->f1;_LL12A: Cyc_Absyndump_dumpqvar(_tmpDA);goto _LLD0;_LL12B: if(*((int*)
_tmp83)!= 33)goto _LL12D;_tmpDB=((struct Cyc_Absyn_Malloc_e_struct*)_tmp83)->f1;
_tmpDC=_tmpDB.is_calloc;_tmpDD=_tmpDB.rgn;_tmpDE=_tmpDB.elt_type;_tmpDF=_tmpDB.num_elts;
_LL12C: if(_tmpDC){if(_tmpDD != 0){Cyc_Absyndump_dump(_tag_arr("rcalloc(",sizeof(
char),9));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmpDD));Cyc_Absyndump_dump(
_tag_arr(",",sizeof(char),2));}else{Cyc_Absyndump_dump(_tag_arr("calloc",sizeof(
char),7));}Cyc_Absyndump_dumpexp(_tmpDF);Cyc_Absyndump_dump(_tag_arr(",",sizeof(
char),2));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmpDE)),0));Cyc_Absyndump_dump(_tag_arr(")",sizeof(char),2));}else{if(_tmpDD != 
0){Cyc_Absyndump_dump(_tag_arr("rmalloc(",sizeof(char),9));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_check_null(_tmpDD));Cyc_Absyndump_dump(_tag_arr(",",
sizeof(char),2));}else{Cyc_Absyndump_dump(_tag_arr("malloc(",sizeof(char),8));}
if(_tmpDE != 0)Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*
_tmpDE,0),_tmpDF,0));else{Cyc_Absyndump_dumpexp(_tmpDF);}Cyc_Absyndump_dump(
_tag_arr(")",sizeof(char),2));}goto _LLD0;_LL12D: if(*((int*)_tmp83)!= 34)goto
_LL12F;_tmpE0=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp83)->f1;_tmpE1=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp83)->f2;_LL12E:((void(*)(void(*f)(
struct _tuple10*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr
end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmpE1,
_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(char),2),_tag_arr(",",sizeof(
char),2));goto _LLD0;_LL12F: if(*((int*)_tmp83)!= 35)goto _LL131;_tmpE2=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp83)->f1;_LL130: Cyc_Absyndump_dump_nospace(_tag_arr("({",sizeof(char),3));Cyc_Absyndump_dumpstmt(
_tmpE2);Cyc_Absyndump_dump_nospace(_tag_arr("})",sizeof(char),3));goto _LLD0;
_LL131: if(*((int*)_tmp83)!= 36)goto _LL133;_tmpE3=((struct Cyc_Absyn_Codegen_e_struct*)
_tmp83)->f1;_LL132: Cyc_Absyndump_dump(_tag_arr("codegen(",sizeof(char),9));Cyc_Absyndump_dumpdecl(({
struct Cyc_Absyn_Decl*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0->r=(void*)((void*)({
struct Cyc_Absyn_Fn_d_struct*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1[0]=({struct
Cyc_Absyn_Fn_d_struct _tmpF2;_tmpF2.tag=1;_tmpF2.f1=_tmpE3;_tmpF2;});_tmpF1;}));
_tmpF0->loc=e->loc;_tmpF0;}));Cyc_Absyndump_dump(_tag_arr(")",sizeof(char),2));
goto _LLD0;_LL133: if(*((int*)_tmp83)!= 37)goto _LLD0;_tmpE4=((struct Cyc_Absyn_Fill_e_struct*)
_tmp83)->f1;_LL134: Cyc_Absyndump_dump(_tag_arr("fill(",sizeof(char),6));Cyc_Absyndump_dumpexp(
_tmpE4);Cyc_Absyndump_dump(_tag_arr(")",sizeof(char),2));goto _LLD0;_LLD0:;}if(
inprec >= myprec)Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*e){Cyc_Absyndump_dumpexp_prec(0,e);}void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmpF3=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmpF3->where_clause == 0?(void*)(
_tmpF3->pattern)->r == (void*)0: 0)Cyc_Absyndump_dump(_tag_arr("default:",sizeof(
char),9));else{Cyc_Absyndump_dump(_tag_arr("case",sizeof(char),5));Cyc_Absyndump_dumppat(
_tmpF3->pattern);if(_tmpF3->where_clause != 0){Cyc_Absyndump_dump(_tag_arr("&&",
sizeof(char),3));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmpF3->where_clause));}
Cyc_Absyndump_dump_nospace(_tag_arr(":",sizeof(char),2));}Cyc_Absyndump_dumpstmt(
_tmpF3->body);}}void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s){void*_tmpF4=(
void*)s->r;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Stmt*_tmpF6;struct Cyc_Absyn_Stmt*
_tmpF7;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*
_tmpFA;struct Cyc_Absyn_Stmt*_tmpFB;struct Cyc_Absyn_Stmt*_tmpFC;struct _tuple2
_tmpFD;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Stmt*_tmpFF;struct _tagged_arr*
_tmp100;struct Cyc_Absyn_Exp*_tmp101;struct _tuple2 _tmp102;struct Cyc_Absyn_Exp*
_tmp103;struct _tuple2 _tmp104;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Stmt*
_tmp106;struct Cyc_Absyn_ForArrayInfo _tmp107;struct Cyc_List_List*_tmp108;struct
_tuple2 _tmp109;struct Cyc_Absyn_Exp*_tmp10A;struct _tuple2 _tmp10B;struct Cyc_Absyn_Exp*
_tmp10C;struct Cyc_Absyn_Stmt*_tmp10D;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_List_List*
_tmp10F;struct Cyc_Absyn_Decl*_tmp110;struct Cyc_Absyn_Stmt*_tmp111;struct
_tagged_arr*_tmp112;struct Cyc_Absyn_Stmt*_tmp113;struct Cyc_Absyn_Stmt*_tmp114;
struct _tuple2 _tmp115;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp117;
struct Cyc_List_List*_tmp118;struct Cyc_List_List*_tmp119;struct Cyc_List_List*
_tmp11A;struct Cyc_Absyn_Stmt*_tmp11B;struct Cyc_List_List*_tmp11C;struct Cyc_Absyn_Tvar*
_tmp11D;struct Cyc_Absyn_Vardecl*_tmp11E;int _tmp11F;struct Cyc_Absyn_Stmt*_tmp120;
struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Stmt*_tmp122;struct Cyc_Absyn_Stmt*
_tmp123;_LL13A: if((int)_tmpF4 != 0)goto _LL13C;_LL13B: Cyc_Absyndump_dump_semi();
goto _LL139;_LL13C: if(_tmpF4 <= (void*)1?1:*((int*)_tmpF4)!= 0)goto _LL13E;_tmpF5=((
struct Cyc_Absyn_Exp_s_struct*)_tmpF4)->f1;_LL13D: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dumpexp(_tmpF5);Cyc_Absyndump_dump_semi();goto _LL139;_LL13E: if(
_tmpF4 <= (void*)1?1:*((int*)_tmpF4)!= 1)goto _LL140;_tmpF6=((struct Cyc_Absyn_Seq_s_struct*)
_tmpF4)->f1;_tmpF7=((struct Cyc_Absyn_Seq_s_struct*)_tmpF4)->f2;_LL13F: if(Cyc_Absynpp_is_declaration(
_tmpF6)){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmpF6);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dumpstmt(_tmpF6);}if(Cyc_Absynpp_is_declaration(
_tmpF7)){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmpF7);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dumpstmt(_tmpF7);}goto _LL139;_LL140: if(_tmpF4 <= (
void*)1?1:*((int*)_tmpF4)!= 2)goto _LL142;_tmpF8=((struct Cyc_Absyn_Return_s_struct*)
_tmpF4)->f1;if(_tmpF8 != 0)goto _LL142;_LL141: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("return;",sizeof(char),8));goto _LL139;_LL142: if(_tmpF4 <= (void*)1?1:*((
int*)_tmpF4)!= 2)goto _LL144;_tmpF9=((struct Cyc_Absyn_Return_s_struct*)_tmpF4)->f1;
_LL143: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("return",
sizeof(char),7));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmpF9));
Cyc_Absyndump_dump_semi();goto _LL139;_LL144: if(_tmpF4 <= (void*)1?1:*((int*)
_tmpF4)!= 3)goto _LL146;_tmpFA=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpF4)->f1;
_tmpFB=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpF4)->f2;_tmpFC=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpF4)->f3;_LL145: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("if(",
sizeof(char),4));Cyc_Absyndump_dumpexp(_tmpFA);{void*_tmp124=(void*)_tmpFB->r;
_LL16B: if(_tmp124 <= (void*)1?1:*((int*)_tmp124)!= 1)goto _LL16D;_LL16C: goto _LL16E;
_LL16D: if(_tmp124 <= (void*)1?1:*((int*)_tmp124)!= 12)goto _LL16F;_LL16E: goto
_LL170;_LL16F: if(_tmp124 <= (void*)1?1:*((int*)_tmp124)!= 3)goto _LL171;_LL170:
goto _LL172;_LL171: if(_tmp124 <= (void*)1?1:*((int*)_tmp124)!= 15)goto _LL173;
_LL172: Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(char),3));Cyc_Absyndump_dumpstmt(
_tmpFB);Cyc_Absyndump_dump_char((int)'}');goto _LL16A;_LL173:;_LL174: Cyc_Absyndump_dump_char((
int)')');Cyc_Absyndump_dumpstmt(_tmpFB);_LL16A:;}{void*_tmp125=(void*)_tmpFC->r;
_LL176: if((int)_tmp125 != 0)goto _LL178;_LL177: goto _LL175;_LL178:;_LL179: Cyc_Absyndump_dump(
_tag_arr("else{",sizeof(char),6));Cyc_Absyndump_dumpstmt(_tmpFC);Cyc_Absyndump_dump_char((
int)'}');goto _LL175;_LL175:;}goto _LL139;_LL146: if(_tmpF4 <= (void*)1?1:*((int*)
_tmpF4)!= 4)goto _LL148;_tmpFD=((struct Cyc_Absyn_While_s_struct*)_tmpF4)->f1;
_tmpFE=_tmpFD.f1;_tmpFF=((struct Cyc_Absyn_While_s_struct*)_tmpF4)->f2;_LL147: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("while(",sizeof(char),7));Cyc_Absyndump_dumpexp(
_tmpFE);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(char),3));Cyc_Absyndump_dumpstmt(
_tmpFF);Cyc_Absyndump_dump_char((int)'}');goto _LL139;_LL148: if(_tmpF4 <= (void*)1?
1:*((int*)_tmpF4)!= 5)goto _LL14A;_LL149: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("break;",sizeof(char),7));goto _LL139;_LL14A: if(_tmpF4 <= (void*)1?1:*((
int*)_tmpF4)!= 6)goto _LL14C;_LL14B: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("continue;",sizeof(char),10));goto _LL139;_LL14C: if(_tmpF4 <= (void*)1?1:*((
int*)_tmpF4)!= 7)goto _LL14E;_tmp100=((struct Cyc_Absyn_Goto_s_struct*)_tmpF4)->f1;
_LL14D: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("goto",sizeof(
char),5));Cyc_Absyndump_dump_str(_tmp100);Cyc_Absyndump_dump_semi();goto _LL139;
_LL14E: if(_tmpF4 <= (void*)1?1:*((int*)_tmpF4)!= 8)goto _LL150;_tmp101=((struct Cyc_Absyn_For_s_struct*)
_tmpF4)->f1;_tmp102=((struct Cyc_Absyn_For_s_struct*)_tmpF4)->f2;_tmp103=_tmp102.f1;
_tmp104=((struct Cyc_Absyn_For_s_struct*)_tmpF4)->f3;_tmp105=_tmp104.f1;_tmp106=((
struct Cyc_Absyn_For_s_struct*)_tmpF4)->f4;_LL14F: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(_tag_arr("for(",sizeof(char),5));Cyc_Absyndump_dumpexp(
_tmp101);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp103);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmp105);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(
char),3));Cyc_Absyndump_dumpstmt(_tmp106);Cyc_Absyndump_dump_char((int)'}');goto
_LL139;_LL150: if(_tmpF4 <= (void*)1?1:*((int*)_tmpF4)!= 19)goto _LL152;_tmp107=((
struct Cyc_Absyn_ForArray_s_struct*)_tmpF4)->f1;_tmp108=_tmp107.defns;_tmp109=
_tmp107.condition;_tmp10A=_tmp109.f1;_tmp10B=_tmp107.delta;_tmp10C=_tmp10B.f1;
_tmp10D=_tmp107.body;_LL151: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("forarray(",sizeof(char),10));for(0;_tmp108 != 0;_tmp108=_tmp108->tl){
Cyc_Absyndump_dumpvardecl((struct Cyc_Absyn_Vardecl*)_tmp108->hd,s->loc);}Cyc_Absyndump_dumpexp(
_tmp10A);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp10C);Cyc_Absyndump_dump_nospace(
_tag_arr("){",sizeof(char),3));Cyc_Absyndump_dumpstmt(_tmp10D);Cyc_Absyndump_dump_char((
int)'}');goto _LL139;_LL152: if(_tmpF4 <= (void*)1?1:*((int*)_tmpF4)!= 9)goto _LL154;
_tmp10E=((struct Cyc_Absyn_Switch_s_struct*)_tmpF4)->f1;_tmp10F=((struct Cyc_Absyn_Switch_s_struct*)
_tmpF4)->f2;_LL153: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("switch(",
sizeof(char),8));Cyc_Absyndump_dumpexp(_tmp10E);Cyc_Absyndump_dump_nospace(
_tag_arr("){",sizeof(char),3));Cyc_Absyndump_dumpswitchclauses(_tmp10F);Cyc_Absyndump_dump_char((
int)'}');goto _LL139;_LL154: if(_tmpF4 <= (void*)1?1:*((int*)_tmpF4)!= 12)goto
_LL156;_tmp110=((struct Cyc_Absyn_Decl_s_struct*)_tmpF4)->f1;_tmp111=((struct Cyc_Absyn_Decl_s_struct*)
_tmpF4)->f2;_LL155: Cyc_Absyndump_dumpdecl(_tmp110);Cyc_Absyndump_dumpstmt(
_tmp111);goto _LL139;_LL156: if(_tmpF4 <= (void*)1?1:*((int*)_tmpF4)!= 15)goto
_LL158;_tmp112=((struct Cyc_Absyn_Label_s_struct*)_tmpF4)->f1;_tmp113=((struct Cyc_Absyn_Label_s_struct*)
_tmpF4)->f2;_LL157: if(Cyc_Absynpp_is_declaration(_tmp113)){Cyc_Absyndump_dump_str(
_tmp112);Cyc_Absyndump_dump_nospace(_tag_arr(": {",sizeof(char),4));Cyc_Absyndump_dumpstmt(
_tmp113);Cyc_Absyndump_dump_char((int)'}');}else{Cyc_Absyndump_dump_str(_tmp112);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp113);}goto _LL139;
_LL158: if(_tmpF4 <= (void*)1?1:*((int*)_tmpF4)!= 16)goto _LL15A;_tmp114=((struct
Cyc_Absyn_Do_s_struct*)_tmpF4)->f1;_tmp115=((struct Cyc_Absyn_Do_s_struct*)_tmpF4)->f2;
_tmp116=_tmp115.f1;_LL159: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("do{",sizeof(char),4));Cyc_Absyndump_dumpstmt(_tmp114);Cyc_Absyndump_dump_nospace(
_tag_arr("}while(",sizeof(char),8));Cyc_Absyndump_dumpexp(_tmp116);Cyc_Absyndump_dump_nospace(
_tag_arr(");",sizeof(char),3));goto _LL139;_LL15A: if(_tmpF4 <= (void*)1?1:*((int*)
_tmpF4)!= 10)goto _LL15C;_tmp117=((struct Cyc_Absyn_SwitchC_s_struct*)_tmpF4)->f1;
_tmp118=((struct Cyc_Absyn_SwitchC_s_struct*)_tmpF4)->f2;_LL15B: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("switch \"C\"(",sizeof(char),12));Cyc_Absyndump_dumpexp(
_tmp117);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(char),3));for(0;_tmp118
!= 0;_tmp118=_tmp118->tl){struct Cyc_Absyn_SwitchC_clause _tmp127;struct Cyc_Absyn_Exp*
_tmp128;struct Cyc_Absyn_Stmt*_tmp129;struct Cyc_Absyn_SwitchC_clause*_tmp126=(
struct Cyc_Absyn_SwitchC_clause*)_tmp118->hd;_tmp127=*_tmp126;_tmp128=_tmp127.cnst_exp;
_tmp129=_tmp127.body;if(_tmp128 == 0)Cyc_Absyndump_dump(_tag_arr("default:",
sizeof(char),9));else{Cyc_Absyndump_dump(_tag_arr("case",sizeof(char),5));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_check_null(_tmp128));Cyc_Absyndump_dump_char((int)':');}
Cyc_Absyndump_dumpstmt(_tmp129);}Cyc_Absyndump_dump_char((int)'}');goto _LL139;
_LL15C: if(_tmpF4 <= (void*)1?1:*((int*)_tmpF4)!= 11)goto _LL15E;_tmp119=((struct
Cyc_Absyn_Fallthru_s_struct*)_tmpF4)->f1;if(_tmp119 != 0)goto _LL15E;_LL15D: Cyc_Absyndump_dump(
_tag_arr("fallthru;",sizeof(char),10));goto _LL139;_LL15E: if(_tmpF4 <= (void*)1?1:*((
int*)_tmpF4)!= 11)goto _LL160;_tmp11A=((struct Cyc_Absyn_Fallthru_s_struct*)_tmpF4)->f1;
_LL15F: Cyc_Absyndump_dump(_tag_arr("fallthru(",sizeof(char),10));Cyc_Absyndump_dumpexps_prec(
20,_tmp11A);Cyc_Absyndump_dump_nospace(_tag_arr(");",sizeof(char),3));goto _LL139;
_LL160: if(_tmpF4 <= (void*)1?1:*((int*)_tmpF4)!= 17)goto _LL162;_tmp11B=((struct
Cyc_Absyn_TryCatch_s_struct*)_tmpF4)->f1;_tmp11C=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmpF4)->f2;_LL161: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("try",
sizeof(char),4));Cyc_Absyndump_dumpstmt(_tmp11B);Cyc_Absyndump_dump(_tag_arr("catch{",
sizeof(char),7));Cyc_Absyndump_dumpswitchclauses(_tmp11C);Cyc_Absyndump_dump_char((
int)'}');goto _LL139;_LL162: if(_tmpF4 <= (void*)1?1:*((int*)_tmpF4)!= 18)goto
_LL164;_tmp11D=((struct Cyc_Absyn_Region_s_struct*)_tmpF4)->f1;_tmp11E=((struct
Cyc_Absyn_Region_s_struct*)_tmpF4)->f2;_tmp11F=((struct Cyc_Absyn_Region_s_struct*)
_tmpF4)->f3;_tmp120=((struct Cyc_Absyn_Region_s_struct*)_tmpF4)->f4;_LL163: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("region",sizeof(char),7));if(_tmp11F)Cyc_Absyndump_dump(
_tag_arr("[resetable]",sizeof(char),12));Cyc_Absyndump_dump(_tag_arr("<",sizeof(
char),2));Cyc_Absyndump_dumptvar(_tmp11D);Cyc_Absyndump_dump(_tag_arr("> ",
sizeof(char),3));Cyc_Absyndump_dumpqvar(_tmp11E->name);Cyc_Absyndump_dump(
_tag_arr("{",sizeof(char),2));Cyc_Absyndump_dumpstmt(_tmp120);Cyc_Absyndump_dump(
_tag_arr("}",sizeof(char),2));goto _LL139;_LL164: if(_tmpF4 <= (void*)1?1:*((int*)
_tmpF4)!= 20)goto _LL166;_tmp121=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmpF4)->f1;
_LL165: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("reset_region(",
sizeof(char),14));Cyc_Absyndump_dumpexp(_tmp121);Cyc_Absyndump_dump(_tag_arr(");",
sizeof(char),3));goto _LL139;_LL166: if(_tmpF4 <= (void*)1?1:*((int*)_tmpF4)!= 13)
goto _LL168;_tmp122=((struct Cyc_Absyn_Cut_s_struct*)_tmpF4)->f1;_LL167: Cyc_Absyndump_dump(
_tag_arr("cut",sizeof(char),4));Cyc_Absyndump_dumpstmt(_tmp122);goto _LL139;
_LL168: if(_tmpF4 <= (void*)1?1:*((int*)_tmpF4)!= 14)goto _LL139;_tmp123=((struct
Cyc_Absyn_Splice_s_struct*)_tmpF4)->f1;_LL169: Cyc_Absyndump_dump(_tag_arr("splice",
sizeof(char),7));Cyc_Absyndump_dumpstmt(_tmp123);goto _LL139;_LL139:;}struct
_tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(
struct _tuple11*dp){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*dp).f1,
_tag_arr("",sizeof(char),1),_tag_arr("=",sizeof(char),2),_tag_arr("=",sizeof(
char),2));Cyc_Absyndump_dumppat((*dp).f2);}void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*
p){void*_tmp12A=(void*)p->r;void*_tmp12B;int _tmp12C;void*_tmp12D;int _tmp12E;void*
_tmp12F;int _tmp130;char _tmp131;struct _tagged_arr _tmp132;struct Cyc_Absyn_Vardecl*
_tmp133;struct Cyc_List_List*_tmp134;struct Cyc_Absyn_Pat*_tmp135;struct Cyc_Absyn_Vardecl*
_tmp136;struct _tuple0*_tmp137;struct _tuple0*_tmp138;struct Cyc_List_List*_tmp139;
struct Cyc_Absyn_AggrInfo _tmp13A;void*_tmp13B;struct Cyc_List_List*_tmp13C;struct
Cyc_List_List*_tmp13D;struct Cyc_Absyn_Tunionfield*_tmp13E;struct Cyc_List_List*
_tmp13F;struct Cyc_Absyn_Enumfield*_tmp140;struct Cyc_Absyn_Enumfield*_tmp141;
_LL17B: if((int)_tmp12A != 0)goto _LL17D;_LL17C: Cyc_Absyndump_dump_char((int)'_');
goto _LL17A;_LL17D: if((int)_tmp12A != 1)goto _LL17F;_LL17E: Cyc_Absyndump_dump(
_tag_arr("NULL",sizeof(char),5));goto _LL17A;_LL17F: if(_tmp12A <= (void*)2?1:*((
int*)_tmp12A)!= 1)goto _LL181;_tmp12B=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp12A)->f1;if((int)_tmp12B != 2)goto _LL181;_tmp12C=((struct Cyc_Absyn_Int_p_struct*)
_tmp12A)->f2;_LL180: _tmp12E=_tmp12C;goto _LL182;_LL181: if(_tmp12A <= (void*)2?1:*((
int*)_tmp12A)!= 1)goto _LL183;_tmp12D=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp12A)->f1;if((int)_tmp12D != 0)goto _LL183;_tmp12E=((struct Cyc_Absyn_Int_p_struct*)
_tmp12A)->f2;_LL182: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp143;_tmp143.tag=1;_tmp143.f1=(int)((unsigned int)_tmp12E);{void*_tmp142[1]={&
_tmp143};Cyc_Std_aprintf(_tag_arr("%d",sizeof(char),3),_tag_arr(_tmp142,sizeof(
void*),1));}}));goto _LL17A;_LL183: if(_tmp12A <= (void*)2?1:*((int*)_tmp12A)!= 1)
goto _LL185;_tmp12F=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp12A)->f1;if((int)
_tmp12F != 1)goto _LL185;_tmp130=((struct Cyc_Absyn_Int_p_struct*)_tmp12A)->f2;
_LL184: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp145;_tmp145.tag=1;_tmp145.f1=(unsigned int)_tmp130;{void*_tmp144[1]={&
_tmp145};Cyc_Std_aprintf(_tag_arr("%u",sizeof(char),3),_tag_arr(_tmp144,sizeof(
void*),1));}}));goto _LL17A;_LL185: if(_tmp12A <= (void*)2?1:*((int*)_tmp12A)!= 2)
goto _LL187;_tmp131=((struct Cyc_Absyn_Char_p_struct*)_tmp12A)->f1;_LL186: Cyc_Absyndump_dump(
_tag_arr("'",sizeof(char),2));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(
_tmp131));Cyc_Absyndump_dump_nospace(_tag_arr("'",sizeof(char),2));goto _LL17A;
_LL187: if(_tmp12A <= (void*)2?1:*((int*)_tmp12A)!= 3)goto _LL189;_tmp132=((struct
Cyc_Absyn_Float_p_struct*)_tmp12A)->f1;_LL188: Cyc_Absyndump_dump(_tmp132);goto
_LL17A;_LL189: if(_tmp12A <= (void*)2?1:*((int*)_tmp12A)!= 0)goto _LL18B;_tmp133=((
struct Cyc_Absyn_Var_p_struct*)_tmp12A)->f1;_LL18A: Cyc_Absyndump_dumpqvar(_tmp133->name);
goto _LL17A;_LL18B: if(_tmp12A <= (void*)2?1:*((int*)_tmp12A)!= 4)goto _LL18D;
_tmp134=((struct Cyc_Absyn_Tuple_p_struct*)_tmp12A)->f1;_LL18C:((void(*)(void(*f)(
struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
_tmp134,_tag_arr("$(",sizeof(char),3),_tag_arr(")",sizeof(char),2),_tag_arr(",",
sizeof(char),2));goto _LL17A;_LL18D: if(_tmp12A <= (void*)2?1:*((int*)_tmp12A)!= 5)
goto _LL18F;_tmp135=((struct Cyc_Absyn_Pointer_p_struct*)_tmp12A)->f1;_LL18E: Cyc_Absyndump_dump(
_tag_arr("&",sizeof(char),2));Cyc_Absyndump_dumppat(_tmp135);goto _LL17A;_LL18F:
if(_tmp12A <= (void*)2?1:*((int*)_tmp12A)!= 6)goto _LL191;_tmp136=((struct Cyc_Absyn_Reference_p_struct*)
_tmp12A)->f1;_LL190: Cyc_Absyndump_dump(_tag_arr("*",sizeof(char),2));Cyc_Absyndump_dumpqvar(
_tmp136->name);goto _LL17A;_LL191: if(_tmp12A <= (void*)2?1:*((int*)_tmp12A)!= 11)
goto _LL193;_tmp137=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp12A)->f1;_LL192: Cyc_Absyndump_dumpqvar(
_tmp137);goto _LL17A;_LL193: if(_tmp12A <= (void*)2?1:*((int*)_tmp12A)!= 12)goto
_LL195;_tmp138=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp12A)->f1;_tmp139=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp12A)->f2;_LL194: Cyc_Absyndump_dumpqvar(
_tmp138);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp139,_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(
char),2),_tag_arr(",",sizeof(char),2));goto _LL17A;_LL195: if(_tmp12A <= (void*)2?1:*((
int*)_tmp12A)!= 7)goto _LL197;_tmp13A=((struct Cyc_Absyn_Aggr_p_struct*)_tmp12A)->f1;
_tmp13B=(void*)_tmp13A.aggr_info;_tmp13C=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp12A)->f2;_tmp13D=((struct Cyc_Absyn_Aggr_p_struct*)_tmp12A)->f3;_LL196: {
struct _tuple0*_tmp147;struct _tuple3 _tmp146=Cyc_Absyn_aggr_kinded_name(_tmp13B);
_tmp147=_tmp146.f2;Cyc_Absyndump_dumpqvar(_tmp147);Cyc_Absyndump_dump_char((int)'{');((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _tagged_arr
start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp13C,_tag_arr("[",sizeof(char),2),_tag_arr("]",sizeof(char),2),_tag_arr(",",
sizeof(char),2));((void(*)(void(*f)(struct _tuple11*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp,_tmp13D,_tag_arr("",sizeof(char),1),_tag_arr("}",sizeof(
char),2),_tag_arr(",",sizeof(char),2));goto _LL17A;}_LL197: if(_tmp12A <= (void*)2?
1:*((int*)_tmp12A)!= 8)goto _LL199;_tmp13E=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp12A)->f2;_tmp13F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp12A)->f3;_LL198: Cyc_Absyndump_dumpqvar(
_tmp13E->name);if(_tmp13F != 0)((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp13F,_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(
char),2),_tag_arr(",",sizeof(char),2));goto _LL17A;_LL199: if(_tmp12A <= (void*)2?1:*((
int*)_tmp12A)!= 9)goto _LL19B;_tmp140=((struct Cyc_Absyn_Enum_p_struct*)_tmp12A)->f2;
_LL19A: _tmp141=_tmp140;goto _LL19C;_LL19B: if(_tmp12A <= (void*)2?1:*((int*)_tmp12A)
!= 10)goto _LL17A;_tmp141=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp12A)->f2;
_LL19C: Cyc_Absyndump_dumpqvar(_tmp141->name);goto _LL17A;_LL17A:;}void Cyc_Absyndump_dumptunionfield(
struct Cyc_Absyn_Tunionfield*ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}void Cyc_Absyndump_dumptunionfields(struct Cyc_List_List*
fields){((void(*)(void(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*l,
struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumptunionfield,
fields,_tag_arr(",",sizeof(char),2));}void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*
ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->tag != 0){Cyc_Absyndump_dump(_tag_arr(" = ",
sizeof(char),4));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){((void(*)(void(*f)(
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumpenumfield,fields,_tag_arr(",",sizeof(char),2));}void Cyc_Absyndump_dumpaggrfields(
struct Cyc_List_List*fields){for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Aggrfield
_tmp149;struct _tagged_arr*_tmp14A;struct Cyc_Absyn_Tqual _tmp14B;void*_tmp14C;
struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_List_List*_tmp14E;struct Cyc_Absyn_Aggrfield*
_tmp148=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp149=*_tmp148;_tmp14A=_tmp149.name;
_tmp14B=_tmp149.tq;_tmp14C=(void*)_tmp149.type;_tmp14D=_tmp149.width;_tmp14E=
_tmp149.attributes;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct
_tagged_arr*),struct _tagged_arr*))Cyc_Absyndump_dumptqtd)(_tmp14B,_tmp14C,Cyc_Absyndump_dump_str,
_tmp14A);Cyc_Absyndump_dumpatts(_tmp14E);if(_tmp14D != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp14D));}Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(
td->name);Cyc_Absyndump_dumptvars(td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(
struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(fd->attributes);Cyc_Absyndump_dumpqvar(
fd->name);}static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple3*pr){{void*
_tmp14F=(*pr).f1;_LL19E: if((int)_tmp14F != 8)goto _LL1A0;_LL19F: goto _LL19D;_LL1A0:
if((int)_tmp14F != 0)goto _LL1A2;_LL1A1: Cyc_Absyndump_dump(_tag_arr("_stdcall",
sizeof(char),9));goto _LL19D;_LL1A2: if((int)_tmp14F != 1)goto _LL1A4;_LL1A3: Cyc_Absyndump_dump(
_tag_arr("_cdecl",sizeof(char),7));goto _LL19D;_LL1A4: if((int)_tmp14F != 2)goto
_LL1A6;_LL1A5: Cyc_Absyndump_dump(_tag_arr("_fastcall",sizeof(char),10));goto
_LL19D;_LL1A6:;_LL1A7: goto _LL19D;_LL19D:;}Cyc_Absyndump_dumpqvar((*pr).f2);}
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dump_callconv(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}static void Cyc_Absyndump_dumpids(
struct Cyc_List_List*vds){for(0;vds != 0;vds=vds->tl){Cyc_Absyndump_dumpqvar(((
struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl != 0)Cyc_Absyndump_dump_char((
int)',');}}void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Vardecl _tmp151;void*_tmp152;struct _tuple0*_tmp153;struct Cyc_Absyn_Tqual
_tmp154;void*_tmp155;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_List_List*_tmp157;
struct Cyc_Absyn_Vardecl*_tmp150=vd;_tmp151=*_tmp150;_tmp152=(void*)_tmp151.sc;
_tmp153=_tmp151.name;_tmp154=_tmp151.tq;_tmp155=(void*)_tmp151.type;_tmp156=
_tmp151.initializer;_tmp157=_tmp151.attributes;Cyc_Absyndump_dumploc(loc);if(Cyc_Absyndump_to_VC){
Cyc_Absyndump_dumpatts(_tmp157);Cyc_Absyndump_dumpscope(_tmp152);{struct Cyc_Absyn_Tqual
_tmp159;void*_tmp15A;struct Cyc_List_List*_tmp15B;struct _tuple5 _tmp158=Cyc_Absynpp_to_tms(
_tmp154,_tmp155);_tmp159=_tmp158.f1;_tmp15A=_tmp158.f2;_tmp15B=_tmp158.f3;{void*
call_conv=(void*)8;{struct Cyc_List_List*tms2=_tmp15B;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp15C=(void*)tms2->hd;struct Cyc_List_List*_tmp15D;_LL1A9: if(_tmp15C <= (
void*)1?1:*((int*)_tmp15C)!= 4)goto _LL1AB;_tmp15D=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmp15C)->f2;_LL1AA: for(0;_tmp15D != 0;_tmp15D=_tmp15D->tl){void*_tmp15E=(void*)
_tmp15D->hd;_LL1AE: if((int)_tmp15E != 0)goto _LL1B0;_LL1AF: call_conv=(void*)0;goto
_LL1AD;_LL1B0: if((int)_tmp15E != 1)goto _LL1B2;_LL1B1: call_conv=(void*)1;goto
_LL1AD;_LL1B2: if((int)_tmp15E != 2)goto _LL1B4;_LL1B3: call_conv=(void*)2;goto
_LL1AD;_LL1B4:;_LL1B5: goto _LL1AD;_LL1AD:;}goto _LL1A8;_LL1AB:;_LL1AC: goto _LL1A8;
_LL1A8:;}}Cyc_Absyndump_dumptq(_tmp159);Cyc_Absyndump_dumpntyp(_tmp15A);{struct
_tuple3 _tmp15F=({struct _tuple3 _tmp160;_tmp160.f1=call_conv;_tmp160.f2=_tmp153;
_tmp160;});((void(*)(struct Cyc_List_List*tms,void(*f)(struct _tuple3*),struct
_tuple3*a))Cyc_Absyndump_dumptms)(Cyc_List_imp_rev(_tmp15B),Cyc_Absyndump_dump_callconv_qvar,&
_tmp15F);}}}}else{if(_tmp152 == (void*)3?Cyc_Absyndump_qvar_to_Cids: 0){void*
_tmp161=Cyc_Tcutil_compress(_tmp155);_LL1B7: if(_tmp161 <= (void*)3?1:*((int*)
_tmp161)!= 8)goto _LL1B9;_LL1B8: goto _LL1B6;_LL1B9:;_LL1BA: Cyc_Absyndump_dumpscope(
_tmp152);_LL1B6:;}else{Cyc_Absyndump_dumpscope(_tmp152);}((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp154,
_tmp155,Cyc_Absyndump_dumpqvar,_tmp153);Cyc_Absyndump_dumpatts(_tmp157);}if(
_tmp156 != 0){Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmp156));}Cyc_Absyndump_dump_semi();}void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*d){void*_tmp162=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp163;
struct Cyc_Absyn_Fndecl*_tmp164;struct Cyc_Absyn_Aggrdecl*_tmp165;struct Cyc_Absyn_Tuniondecl*
_tmp166;struct Cyc_Absyn_Tuniondecl _tmp167;void*_tmp168;struct _tuple0*_tmp169;
struct Cyc_List_List*_tmp16A;struct Cyc_Core_Opt*_tmp16B;int _tmp16C;struct Cyc_Absyn_Enumdecl*
_tmp16D;struct Cyc_Absyn_Enumdecl _tmp16E;void*_tmp16F;struct _tuple0*_tmp170;
struct Cyc_Core_Opt*_tmp171;struct Cyc_Absyn_Pat*_tmp172;struct Cyc_Absyn_Exp*
_tmp173;struct Cyc_List_List*_tmp174;struct Cyc_Absyn_Typedefdecl*_tmp175;struct
_tagged_arr*_tmp176;struct Cyc_List_List*_tmp177;struct _tuple0*_tmp178;struct Cyc_List_List*
_tmp179;struct Cyc_List_List*_tmp17A;_LL1BC: if(*((int*)_tmp162)!= 0)goto _LL1BE;
_tmp163=((struct Cyc_Absyn_Var_d_struct*)_tmp162)->f1;_LL1BD: Cyc_Absyndump_dumpvardecl(
_tmp163,d->loc);goto _LL1BB;_LL1BE: if(*((int*)_tmp162)!= 1)goto _LL1C0;_tmp164=((
struct Cyc_Absyn_Fn_d_struct*)_tmp162)->f1;_LL1BF: Cyc_Absyndump_dumploc(d->loc);
if(Cyc_Absyndump_to_VC)Cyc_Absyndump_dumpatts(_tmp164->attributes);if(_tmp164->is_inline){
if(Cyc_Absyndump_to_VC)Cyc_Absyndump_dump(_tag_arr("__inline",sizeof(char),9));
else{Cyc_Absyndump_dump(_tag_arr("inline",sizeof(char),7));}}Cyc_Absyndump_dumpscope((
void*)_tmp164->sc);{void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp17C=
_cycalloc(sizeof(*_tmp17C));_tmp17C[0]=({struct Cyc_Absyn_FnType_struct _tmp17D;
_tmp17D.tag=8;_tmp17D.f1=({struct Cyc_Absyn_FnInfo _tmp17E;_tmp17E.tvars=_tmp164->tvs;
_tmp17E.effect=_tmp164->effect;_tmp17E.ret_typ=(void*)((void*)_tmp164->ret_type);
_tmp17E.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple4*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp164->args);_tmp17E.c_varargs=
_tmp164->c_varargs;_tmp17E.cyc_varargs=_tmp164->cyc_varargs;_tmp17E.rgn_po=
_tmp164->rgn_po;_tmp17E.attributes=0;_tmp17E;});_tmp17D;});_tmp17C;});((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))
Cyc_Absyndump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp17B;_tmp17B.q_const=0;_tmp17B.q_volatile=
0;_tmp17B.q_restrict=0;_tmp17B;}),t,Cyc_Absyndump_to_VC?Cyc_Absyndump_dump_callconv_fdqvar:
Cyc_Absyndump_dump_atts_qvar,_tmp164);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(
_tmp164->body);Cyc_Absyndump_dump_char((int)'}');goto _LL1BB;}_LL1C0: if(*((int*)
_tmp162)!= 4)goto _LL1C2;_tmp165=((struct Cyc_Absyn_Aggr_d_struct*)_tmp162)->f1;
_LL1C1: Cyc_Absyndump_dumpscope((void*)_tmp165->sc);Cyc_Absyndump_dumpaggr_kind((
void*)_tmp165->kind);Cyc_Absyndump_dumpqvar(_tmp165->name);Cyc_Absyndump_dumptvars(
_tmp165->tvs);if(_tmp165->impl == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp165->impl))->exist_vars
!= 0)((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp165->impl))->exist_vars,
_tag_arr("<",sizeof(char),2),_tag_arr(">",sizeof(char),2),_tag_arr(",",sizeof(
char),2));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp165->impl))->rgn_po
!= 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp165->impl))->rgn_po);}Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp165->impl))->fields);Cyc_Absyndump_dump(_tag_arr("}",sizeof(char),
2));Cyc_Absyndump_dumpatts(_tmp165->attributes);Cyc_Absyndump_dump(_tag_arr(";",
sizeof(char),2));}goto _LL1BB;_LL1C2: if(*((int*)_tmp162)!= 5)goto _LL1C4;_tmp166=((
struct Cyc_Absyn_Tunion_d_struct*)_tmp162)->f1;_tmp167=*_tmp166;_tmp168=(void*)
_tmp167.sc;_tmp169=_tmp167.name;_tmp16A=_tmp167.tvs;_tmp16B=_tmp167.fields;
_tmp16C=_tmp167.is_xtunion;_LL1C3: Cyc_Absyndump_dumpscope(_tmp168);Cyc_Absyndump_dump(
_tmp16C?_tag_arr("xtunion",sizeof(char),8): _tag_arr("tunion",sizeof(char),7));
Cyc_Absyndump_dumpqvar(_tmp169);Cyc_Absyndump_dumptvars(_tmp16A);if(_tmp16B == 0)
Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumptunionfields((
struct Cyc_List_List*)_tmp16B->v);Cyc_Absyndump_dump_nospace(_tag_arr("};",
sizeof(char),3));}goto _LL1BB;_LL1C4: if(*((int*)_tmp162)!= 6)goto _LL1C6;_tmp16D=((
struct Cyc_Absyn_Enum_d_struct*)_tmp162)->f1;_tmp16E=*_tmp16D;_tmp16F=(void*)
_tmp16E.sc;_tmp170=_tmp16E.name;_tmp171=_tmp16E.fields;_LL1C5: Cyc_Absyndump_dumpscope(
_tmp16F);Cyc_Absyndump_dump(_tag_arr("enum ",sizeof(char),6));Cyc_Absyndump_dumpqvar(
_tmp170);if(_tmp171 == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp171->v);Cyc_Absyndump_dump_nospace(
_tag_arr("};",sizeof(char),3));}return;_LL1C6: if(*((int*)_tmp162)!= 2)goto _LL1C8;
_tmp172=((struct Cyc_Absyn_Let_d_struct*)_tmp162)->f1;_tmp173=((struct Cyc_Absyn_Let_d_struct*)
_tmp162)->f3;_LL1C7: Cyc_Absyndump_dump(_tag_arr("let",sizeof(char),4));Cyc_Absyndump_dumppat(
_tmp172);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(_tmp173);Cyc_Absyndump_dump_semi();
goto _LL1BB;_LL1C8: if(*((int*)_tmp162)!= 3)goto _LL1CA;_tmp174=((struct Cyc_Absyn_Letv_d_struct*)
_tmp162)->f1;_LL1C9: Cyc_Absyndump_dump(_tag_arr("let ",sizeof(char),5));Cyc_Absyndump_dumpids(
_tmp174);Cyc_Absyndump_dump_semi();goto _LL1BB;_LL1CA: if(*((int*)_tmp162)!= 7)
goto _LL1CC;_tmp175=((struct Cyc_Absyn_Typedef_d_struct*)_tmp162)->f1;_LL1CB: if(!
Cyc_Absyndump_expand_typedefs?1:(_tmp175->defn != 0?Cyc_Absynpp_is_anon_aggrtype((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp175->defn))->v): 0)){Cyc_Absyndump_dump(
_tag_arr("typedef",sizeof(char),8));{void*t;if(_tmp175->defn == 0)t=Cyc_Absyn_new_evar(
_tmp175->kind,0);else{t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp175->defn))->v;}((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(({struct Cyc_Absyn_Tqual
_tmp17F;_tmp17F.q_const=0;_tmp17F.q_volatile=0;_tmp17F.q_restrict=0;_tmp17F;}),t,
Cyc_Absyndump_dumptypedefname,_tmp175);Cyc_Absyndump_dump_semi();}}goto _LL1BB;
_LL1CC: if(*((int*)_tmp162)!= 8)goto _LL1CE;_tmp176=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp162)->f1;_tmp177=((struct Cyc_Absyn_Namespace_d_struct*)_tmp162)->f2;_LL1CD:
Cyc_Absyndump_dump(_tag_arr("namespace",sizeof(char),10));Cyc_Absyndump_dump_str(
_tmp176);Cyc_Absyndump_dump_char((int)'{');for(0;_tmp177 != 0;_tmp177=_tmp177->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp177->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1BB;_LL1CE: if(*((int*)_tmp162)!= 9)goto _LL1D0;_tmp178=((struct
Cyc_Absyn_Using_d_struct*)_tmp162)->f1;_tmp179=((struct Cyc_Absyn_Using_d_struct*)
_tmp162)->f2;_LL1CF: Cyc_Absyndump_dump(_tag_arr("using",sizeof(char),6));Cyc_Absyndump_dumpqvar(
_tmp178);Cyc_Absyndump_dump_char((int)'{');for(0;_tmp179 != 0;_tmp179=_tmp179->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp179->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1BB;_LL1D0: if(*((int*)_tmp162)!= 10)goto _LL1BB;_tmp17A=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp162)->f1;_LL1D1: Cyc_Absyndump_dump(_tag_arr("extern \"C\" {",
sizeof(char),13));for(0;_tmp17A != 0;_tmp17A=_tmp17A->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)_tmp17A->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1BB;
_LL1BB:;}static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){struct
_tuple6 pr=Cyc_Evexp_eval_const_uint_exp(e);if(pr.f1 != 1?1: !pr.f2){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}void Cyc_Absyndump_dumptms(
struct Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){f(a);return;}{void*
_tmp180=(void*)tms->hd;void*_tmp181;struct Cyc_Absyn_Exp*_tmp182;void*_tmp183;
struct Cyc_Absyn_Tqual _tmp184;void*_tmp185;struct Cyc_Absyn_Exp*_tmp186;void*
_tmp187;struct Cyc_Absyn_Tqual _tmp188;void*_tmp189;void*_tmp18A;struct Cyc_Absyn_Tqual
_tmp18B;void*_tmp18C;struct Cyc_Absyn_Exp*_tmp18D;void*_tmp18E;struct Cyc_Absyn_Tvar*
_tmp18F;struct Cyc_Absyn_Tqual _tmp190;void*_tmp191;struct Cyc_Absyn_Exp*_tmp192;
void*_tmp193;struct Cyc_Absyn_Tvar*_tmp194;struct Cyc_Absyn_Tqual _tmp195;void*
_tmp196;void*_tmp197;struct Cyc_Absyn_Tvar*_tmp198;struct Cyc_Absyn_Tqual _tmp199;
_LL1D3: if(_tmp180 <= (void*)1?1:*((int*)_tmp180)!= 1)goto _LL1D5;_tmp181=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp180)->f1;if(_tmp181 <= (void*)1?1:*((int*)
_tmp181)!= 1)goto _LL1D5;_tmp182=((struct Cyc_Absyn_Nullable_ps_struct*)_tmp181)->f1;
_tmp183=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp180)->f2;if((int)
_tmp183 != 2)goto _LL1D5;_tmp184=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp180)->f3;
_LL1D4: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dump_upperbound(_tmp182);
Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1D5: if(_tmp180 <= (void*)1?1:*((int*)
_tmp180)!= 1)goto _LL1D7;_tmp185=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp180)->f1;if(_tmp185 <= (void*)1?1:*((int*)_tmp185)!= 0)goto _LL1D7;_tmp186=((
struct Cyc_Absyn_NonNullable_ps_struct*)_tmp185)->f1;_tmp187=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp180)->f2;if((int)_tmp187 != 2)goto _LL1D7;_tmp188=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp180)->f3;_LL1D6: Cyc_Absyndump_dump_char((int)'@');Cyc_Absyndump_dump_upperbound(
_tmp186);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1D7: if(_tmp180 <= (void*)1?
1:*((int*)_tmp180)!= 1)goto _LL1D9;_tmp189=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp180)->f1;if((int)_tmp189 != 0)goto _LL1D9;_tmp18A=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp180)->f2;if((int)_tmp18A != 2)goto _LL1D9;_tmp18B=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp180)->f3;_LL1D8: Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumptms(tms->tl,
f,a);return;_LL1D9: if(_tmp180 <= (void*)1?1:*((int*)_tmp180)!= 1)goto _LL1DB;
_tmp18C=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp180)->f1;if(_tmp18C <= (
void*)1?1:*((int*)_tmp18C)!= 1)goto _LL1DB;_tmp18D=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmp18C)->f1;_tmp18E=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp180)->f2;
if(_tmp18E <= (void*)3?1:*((int*)_tmp18E)!= 1)goto _LL1DB;_tmp18F=((struct Cyc_Absyn_VarType_struct*)
_tmp18E)->f1;_tmp190=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp180)->f3;_LL1DA:
Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dump_upperbound(_tmp18D);Cyc_Absyndump_dump_str(
_tmp18F->name);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1DB: if(_tmp180 <= (
void*)1?1:*((int*)_tmp180)!= 1)goto _LL1DD;_tmp191=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp180)->f1;if(_tmp191 <= (void*)1?1:*((int*)_tmp191)!= 0)goto _LL1DD;_tmp192=((
struct Cyc_Absyn_NonNullable_ps_struct*)_tmp191)->f1;_tmp193=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp180)->f2;if(_tmp193 <= (void*)3?1:*((int*)_tmp193)!= 1)goto _LL1DD;_tmp194=((
struct Cyc_Absyn_VarType_struct*)_tmp193)->f1;_tmp195=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp180)->f3;_LL1DC: Cyc_Absyndump_dump_char((int)'@');Cyc_Absyndump_dump_upperbound(
_tmp192);Cyc_Absyndump_dump_str(_tmp194->name);Cyc_Absyndump_dumptms(tms->tl,f,a);
return;_LL1DD: if(_tmp180 <= (void*)1?1:*((int*)_tmp180)!= 1)goto _LL1DF;_tmp196=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp180)->f1;if((int)_tmp196 != 0)
goto _LL1DF;_tmp197=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp180)->f2;if(
_tmp197 <= (void*)3?1:*((int*)_tmp197)!= 1)goto _LL1DF;_tmp198=((struct Cyc_Absyn_VarType_struct*)
_tmp197)->f1;_tmp199=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp180)->f3;_LL1DE:
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dump_str(_tmp198->name);Cyc_Absyndump_dumptms(
tms->tl,f,a);return;_LL1DF: if(_tmp180 <= (void*)1?1:*((int*)_tmp180)!= 1)goto
_LL1E1;_LL1E0:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp19A=
_cycalloc(sizeof(*_tmp19A));_tmp19A[0]=({struct Cyc_Core_Impossible_struct _tmp19B;
_tmp19B.tag=Cyc_Core_Impossible;_tmp19B.f1=_tag_arr("dumptms: bad Pointer_mod",
sizeof(char),25);_tmp19B;});_tmp19A;}));_LL1E1:;_LL1E2: {int next_is_pointer=0;
if(tms->tl != 0){void*_tmp19C=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;
_LL1E4: if(_tmp19C <= (void*)1?1:*((int*)_tmp19C)!= 1)goto _LL1E6;_LL1E5:
next_is_pointer=1;goto _LL1E3;_LL1E6:;_LL1E7: goto _LL1E3;_LL1E3:;}if(
next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(tms->tl,
f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp19D=(void*)
tms->hd;struct Cyc_Absyn_Exp*_tmp19E;void*_tmp19F;struct Cyc_List_List*_tmp1A0;int
_tmp1A1;struct Cyc_Absyn_VarargInfo*_tmp1A2;struct Cyc_Core_Opt*_tmp1A3;struct Cyc_List_List*
_tmp1A4;void*_tmp1A5;struct Cyc_List_List*_tmp1A6;struct Cyc_Position_Segment*
_tmp1A7;struct Cyc_List_List*_tmp1A8;struct Cyc_Position_Segment*_tmp1A9;int
_tmp1AA;struct Cyc_List_List*_tmp1AB;void*_tmp1AC;void*_tmp1AD;_LL1E9: if((int)
_tmp19D != 0)goto _LL1EB;_LL1EA: Cyc_Absyndump_dump(_tag_arr("[]",sizeof(char),3));
goto _LL1E8;_LL1EB: if(_tmp19D <= (void*)1?1:*((int*)_tmp19D)!= 0)goto _LL1ED;
_tmp19E=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmp19D)->f1;_LL1EC: Cyc_Absyndump_dump_char((
int)'[');Cyc_Absyndump_dumpexp(_tmp19E);Cyc_Absyndump_dump_char((int)']');goto
_LL1E8;_LL1ED: if(_tmp19D <= (void*)1?1:*((int*)_tmp19D)!= 2)goto _LL1EF;_tmp19F=(
void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp19D)->f1;if(*((int*)_tmp19F)!= 
1)goto _LL1EF;_tmp1A0=((struct Cyc_Absyn_WithTypes_struct*)_tmp19F)->f1;_tmp1A1=((
struct Cyc_Absyn_WithTypes_struct*)_tmp19F)->f2;_tmp1A2=((struct Cyc_Absyn_WithTypes_struct*)
_tmp19F)->f3;_tmp1A3=((struct Cyc_Absyn_WithTypes_struct*)_tmp19F)->f4;_tmp1A4=((
struct Cyc_Absyn_WithTypes_struct*)_tmp19F)->f5;_LL1EE: Cyc_Absyndump_dumpfunargs(
_tmp1A0,_tmp1A1,_tmp1A2,_tmp1A3,_tmp1A4);goto _LL1E8;_LL1EF: if(_tmp19D <= (void*)1?
1:*((int*)_tmp19D)!= 2)goto _LL1F1;_tmp1A5=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp19D)->f1;if(*((int*)_tmp1A5)!= 0)goto _LL1F1;_tmp1A6=((struct Cyc_Absyn_NoTypes_struct*)
_tmp1A5)->f1;_tmp1A7=((struct Cyc_Absyn_NoTypes_struct*)_tmp1A5)->f2;_LL1F0:((
void(*)(void(*f)(struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr
start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,
_tmp1A6,_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(char),2),_tag_arr(",",
sizeof(char),2));goto _LL1E8;_LL1F1: if(_tmp19D <= (void*)1?1:*((int*)_tmp19D)!= 3)
goto _LL1F3;_tmp1A8=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp19D)->f1;_tmp1A9=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmp19D)->f2;_tmp1AA=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp19D)->f3;_LL1F2: if(_tmp1AA)Cyc_Absyndump_dumpkindedtvars(_tmp1A8);else{Cyc_Absyndump_dumptvars(
_tmp1A8);}goto _LL1E8;_LL1F3: if(_tmp19D <= (void*)1?1:*((int*)_tmp19D)!= 4)goto
_LL1F5;_tmp1AB=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp19D)->f2;_LL1F4: Cyc_Absyndump_dumpatts(
_tmp1AB);goto _LL1E8;_LL1F5: if(_tmp19D <= (void*)1?1:*((int*)_tmp19D)!= 1)goto
_LL1E8;_tmp1AC=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp19D)->f1;_tmp1AD=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp19D)->f2;_LL1F6:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));
_tmp1AE[0]=({struct Cyc_Core_Impossible_struct _tmp1AF;_tmp1AF.tag=Cyc_Core_Impossible;
_tmp1AF.f1=_tag_arr("dumptms",sizeof(char),8);_tmp1AF;});_tmp1AE;}));_LL1E8:;}
return;}_LL1D2:;}}void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*
f)(void*),void*a){struct Cyc_Absyn_Tqual _tmp1B1;void*_tmp1B2;struct Cyc_List_List*
_tmp1B3;struct _tuple5 _tmp1B0=Cyc_Absynpp_to_tms(tq,t);_tmp1B1=_tmp1B0.f1;_tmp1B2=
_tmp1B0.f2;_tmp1B3=_tmp1B0.f3;Cyc_Absyndump_dumptq(_tmp1B1);Cyc_Absyndump_dumpntyp(
_tmp1B2);Cyc_Absyndump_dumptms(Cyc_List_imp_rev(_tmp1B3),f,a);}void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=
f;for(0;tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}({
void*_tmp1B4[0]={};Cyc_Std_fprintf(f,_tag_arr("\n",sizeof(char),2),_tag_arr(
_tmp1B4,sizeof(void*),0));});}
