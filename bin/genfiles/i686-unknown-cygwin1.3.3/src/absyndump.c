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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{char*
tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stdout;int Cyc_Std_fputc(
int __c,struct Cyc_Std___cycFILE*__stream);extern char Cyc_Std_FileCloseError[19];
extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*tag;
struct _tagged_arr f1;};int Cyc_Std_file_string_write(struct Cyc_Std___cycFILE*fd,
struct _tagged_arr src,int src_offset,int max_count);struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};int Cyc_Std_fprintf(
struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr);struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];int Cyc_List_exists(int(*pred)(
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
struct Cyc_Absyn_Format_att_struct{int tag;void*f1;int f2;int f3;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pointer_mod_struct{int tag;void*
f1;void*f2;struct Cyc_Absyn_Tqual f3;};struct Cyc_Absyn_Function_mod_struct{int tag;
void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int
tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{
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
f3;};struct _tuple5 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t);
unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern
char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct _tuple6{void*f1;
void*f2;};struct _tuple6*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d);struct _tuple6*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
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
sizeof(char),11));return;_LL20: if((int)_tmp6 != 1)goto _LL17;_LL21: Cyc_Absyndump_dump(
_tag_arr("abstract",sizeof(char),9));return;_LL17:;}void Cyc_Absyndump_dumpkind(
void*k){void*_tmp7=k;_LL23: if((int)_tmp7 != 0)goto _LL25;_LL24: Cyc_Absyndump_dump(
_tag_arr("A",sizeof(char),2));return;_LL25: if((int)_tmp7 != 1)goto _LL27;_LL26: Cyc_Absyndump_dump(
_tag_arr("M",sizeof(char),2));return;_LL27: if((int)_tmp7 != 2)goto _LL29;_LL28: Cyc_Absyndump_dump(
_tag_arr("B",sizeof(char),2));return;_LL29: if((int)_tmp7 != 3)goto _LL2B;_LL2A: Cyc_Absyndump_dump(
_tag_arr("R",sizeof(char),2));return;_LL2B: if((int)_tmp7 != 4)goto _LL2D;_LL2C: Cyc_Absyndump_dump(
_tag_arr("E",sizeof(char),2));return;_LL2D: if((int)_tmp7 != 5)goto _LL22;_LL2E: Cyc_Absyndump_dump(
_tag_arr("I",sizeof(char),2));return;_LL22:;}void Cyc_Absyndump_dumpaggr_kind(
void*k){void*_tmp8=k;_LL30: if((int)_tmp8 != 0)goto _LL32;_LL31: Cyc_Absyndump_dump(
_tag_arr("struct",sizeof(char),7));return;_LL32: if((int)_tmp8 != 1)goto _LL2F;
_LL33: Cyc_Absyndump_dump(_tag_arr("union",sizeof(char),6));return;_LL2F:;}void
Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){Cyc_Absyndump_egroup(Cyc_Absyndump_dumptyp,
ts,_tag_arr("<",sizeof(char),2),_tag_arr(">",sizeof(char),2),_tag_arr(",",
sizeof(char),2));}void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);}void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);{void*_tmp9=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpA;void*
_tmpB;_LL35: if(*((int*)_tmp9)!= 0)goto _LL37;_tmpA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp9)->f1;if((int)_tmpA != 2)goto _LL37;_LL36: goto _LL38;_LL37: if(*((int*)_tmp9)!= 
2)goto _LL39;_LL38: goto _LL3A;_LL39: if(*((int*)_tmp9)!= 1)goto _LL3B;_LL3A: Cyc_Absyndump_dump(
_tag_arr("::?",sizeof(char),4));goto _LL34;_LL3B: if(*((int*)_tmp9)!= 0)goto _LL34;
_tmpB=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp9)->f1;_LL3C: Cyc_Absyndump_dump(
_tag_arr("::",sizeof(char),3));Cyc_Absyndump_dumpkind(_tmpB);goto _LL34;_LL34:;}}
void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){((void(*)(void(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr
end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,tvs,
_tag_arr("<",sizeof(char),2),_tag_arr(">",sizeof(char),2),_tag_arr(",",sizeof(
char),2));}void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){((void(*)(
void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,
tvs,_tag_arr("<",sizeof(char),2),_tag_arr(">",sizeof(char),2),_tag_arr(",",
sizeof(char),2));}struct _tuple7{struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(
struct _tuple7*pr){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(
struct Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple7*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumparg,ts,_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(char),
2),_tag_arr(",",sizeof(char),2));}void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*
atts){for(0;atts != 0;atts=atts->tl){void*_tmpC=(void*)atts->hd;_LL3E: if((int)
_tmpC != 0)goto _LL40;_LL3F: Cyc_Absyndump_dump(_tag_arr("_stdcall",sizeof(char),9));
return;_LL40: if((int)_tmpC != 1)goto _LL42;_LL41: Cyc_Absyndump_dump(_tag_arr("_cdecl",
sizeof(char),7));return;_LL42: if((int)_tmpC != 2)goto _LL44;_LL43: Cyc_Absyndump_dump(
_tag_arr("_fastcall",sizeof(char),10));return;_LL44:;_LL45: goto _LL3D;_LL3D:;}}
void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){int hasatt=0;{struct
Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmpD=(void*)((
struct Cyc_List_List*)_check_null(atts))->hd;_LL47: if((int)_tmpD != 0)goto _LL49;
_LL48: goto _LL4A;_LL49: if((int)_tmpD != 1)goto _LL4B;_LL4A: goto _LL4C;_LL4B: if((int)
_tmpD != 2)goto _LL4D;_LL4C: goto _LL46;_LL4D:;_LL4E: hasatt=1;goto _LL46;_LL46:;}}if(
!hasatt)return;Cyc_Absyndump_dump(_tag_arr("__declspec(",sizeof(char),12));for(0;
atts != 0;atts=atts->tl){void*_tmpE=(void*)atts->hd;_LL50: if((int)_tmpE != 0)goto
_LL52;_LL51: goto _LL53;_LL52: if((int)_tmpE != 1)goto _LL54;_LL53: goto _LL55;_LL54:
if((int)_tmpE != 2)goto _LL56;_LL55: goto _LL4F;_LL56:;_LL57: Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((
void*)atts->hd));goto _LL4F;_LL4F:;}Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpatts(
struct Cyc_List_List*atts){if(atts == 0)return;if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dump_noncallconv(
atts);return;}Cyc_Absyndump_dump(_tag_arr(" __attribute__((",sizeof(char),17));
for(0;atts != 0;atts=atts->tl){Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((
void*)atts->hd));if(atts->tl != 0)Cyc_Absyndump_dump(_tag_arr(",",sizeof(char),2));}
Cyc_Absyndump_dump(_tag_arr(")) ",sizeof(char),4));}int Cyc_Absyndump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmpF=(void*)tms->hd;_LL59: if(
_tmpF <= (void*)1?1:*((int*)_tmpF)!= 1)goto _LL5B;_LL5A: return 1;_LL5B:;_LL5C:
return 0;_LL58:;}}static void Cyc_Absyndump_dumprgn(void*t){void*_tmp10=Cyc_Tcutil_compress(
t);_LL5E: if((int)_tmp10 != 2)goto _LL60;_LL5F: Cyc_Absyndump_dump(_tag_arr("`H",
sizeof(char),3));goto _LL5D;_LL60:;_LL61: Cyc_Absyndump_dumpntyp(t);goto _LL5D;
_LL5D:;}struct _tuple8{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static
struct _tuple8 Cyc_Absyndump_effects_split(void*t){struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;{void*_tmp11=Cyc_Tcutil_compress(t);void*_tmp12;
struct Cyc_List_List*_tmp13;_LL63: if(_tmp11 <= (void*)3?1:*((int*)_tmp11)!= 19)
goto _LL65;_tmp12=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp11)->f1;_LL64:
rgions=({struct Cyc_List_List*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->hd=(void*)
_tmp12;_tmp14->tl=rgions;_tmp14;});goto _LL62;_LL65: if(_tmp11 <= (void*)3?1:*((int*)
_tmp11)!= 20)goto _LL67;_tmp13=((struct Cyc_Absyn_JoinEff_struct*)_tmp11)->f1;
_LL66: for(0;_tmp13 != 0;_tmp13=_tmp13->tl){struct Cyc_List_List*_tmp16;struct Cyc_List_List*
_tmp17;struct _tuple8 _tmp15=Cyc_Absyndump_effects_split((void*)_tmp13->hd);_tmp16=
_tmp15.f1;_tmp17=_tmp15.f2;rgions=Cyc_List_imp_append(_tmp16,rgions);effects=Cyc_List_imp_append(
_tmp17,effects);}goto _LL62;_LL67:;_LL68: effects=({struct Cyc_List_List*_tmp18=
_cycalloc(sizeof(*_tmp18));_tmp18->hd=(void*)t;_tmp18->tl=effects;_tmp18;});goto
_LL62;_LL62:;}return({struct _tuple8 _tmp19;_tmp19.f1=rgions;_tmp19.f2=effects;
_tmp19;});}static void Cyc_Absyndump_dumpeff(void*t){struct Cyc_List_List*_tmp1B;
struct Cyc_List_List*_tmp1C;struct _tuple8 _tmp1A=Cyc_Absyndump_effects_split(t);
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
void*_tmp41;int _tmp42;struct Cyc_List_List*_tmp43;struct Cyc_Absyn_AggrInfo _tmp44;
void*_tmp45;struct Cyc_List_List*_tmp46;void*_tmp47;struct Cyc_List_List*_tmp48;
struct _tuple0*_tmp49;struct Cyc_List_List*_tmp4A;struct _tuple0*_tmp4B;struct Cyc_List_List*
_tmp4C;struct Cyc_Core_Opt*_tmp4D;void*_tmp4E;void*_tmp4F;int _tmp50;void*_tmp51;
_LL6A: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 7)goto _LL6C;_LL6B: goto _LL6D;_LL6C:
if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 8)goto _LL6E;_LL6D: goto _LL6F;_LL6E: if(
_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 4)goto _LL70;_LL6F: return;_LL70: if((int)
_tmp1D != 0)goto _LL72;_LL71: Cyc_Absyndump_dump(_tag_arr("void",sizeof(char),5));
return;_LL72: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 1)goto _LL74;_tmp1E=((
struct Cyc_Absyn_VarType_struct*)_tmp1D)->f1;_LL73: Cyc_Absyndump_dump_str(_tmp1E->name);
return;_LL74: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 0)goto _LL76;_tmp1F=((
struct Cyc_Absyn_Evar_struct*)_tmp1D)->f1;_tmp20=((struct Cyc_Absyn_Evar_struct*)
_tmp1D)->f2;if(_tmp20 != 0)goto _LL76;_tmp21=((struct Cyc_Absyn_Evar_struct*)_tmp1D)->f3;
_LL75: Cyc_Absyndump_dump(_tag_arr("%",sizeof(char),2));if(_tmp1F == 0)Cyc_Absyndump_dump(
_tag_arr("?",sizeof(char),2));else{Cyc_Absyndump_dumpkind((void*)_tmp1F->v);}Cyc_Absyndump_dump((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp53;_tmp53.tag=1;_tmp53.f1=(
int)((unsigned int)_tmp21);{void*_tmp52[1]={& _tmp53};Cyc_Std_aprintf(_tag_arr("(%d)",
sizeof(char),5),_tag_arr(_tmp52,sizeof(void*),1));}}));return;_LL76: if(_tmp1D <= (
void*)3?1:*((int*)_tmp1D)!= 0)goto _LL78;_tmp22=((struct Cyc_Absyn_Evar_struct*)
_tmp1D)->f1;_tmp23=((struct Cyc_Absyn_Evar_struct*)_tmp1D)->f2;if(_tmp23 == 0)goto
_LL78;_tmp24=*_tmp23;_tmp25=(void*)_tmp24.v;_tmp26=((struct Cyc_Absyn_Evar_struct*)
_tmp1D)->f3;_LL77: Cyc_Absyndump_dumpntyp(_tmp25);return;_LL78: if(_tmp1D <= (void*)
3?1:*((int*)_tmp1D)!= 2)goto _LL7A;_tmp27=((struct Cyc_Absyn_TunionType_struct*)
_tmp1D)->f1;_tmp28=(void*)_tmp27.tunion_info;_tmp29=_tmp27.targs;_tmp2A=(void*)
_tmp27.rgn;_LL79:{void*_tmp54=_tmp28;struct Cyc_Absyn_UnknownTunionInfo _tmp55;
struct _tuple0*_tmp56;int _tmp57;struct Cyc_Absyn_Tuniondecl**_tmp58;struct Cyc_Absyn_Tuniondecl*
_tmp59;struct Cyc_Absyn_Tuniondecl _tmp5A;struct _tuple0*_tmp5B;int _tmp5C;_LLB1: if(*((
int*)_tmp54)!= 0)goto _LLB3;_tmp55=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp54)->f1;
_tmp56=_tmp55.name;_tmp57=_tmp55.is_xtunion;_LLB2: _tmp5B=_tmp56;_tmp5C=_tmp57;
goto _LLB4;_LLB3: if(*((int*)_tmp54)!= 1)goto _LLB0;_tmp58=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp54)->f1;_tmp59=*_tmp58;_tmp5A=*_tmp59;_tmp5B=_tmp5A.name;_tmp5C=_tmp5A.is_xtunion;
_LLB4: if(_tmp5C)Cyc_Absyndump_dump(_tag_arr("xtunion ",sizeof(char),9));else{Cyc_Absyndump_dump(
_tag_arr("tunion ",sizeof(char),8));}{void*_tmp5D=Cyc_Tcutil_compress(_tmp2A);
_LLB6: if((int)_tmp5D != 2)goto _LLB8;_LLB7: goto _LLB5;_LLB8:;_LLB9: Cyc_Absyndump_dumptyp(
_tmp2A);Cyc_Absyndump_dump(_tag_arr(" ",sizeof(char),2));goto _LLB5;_LLB5:;}Cyc_Absyndump_dumpqvar(
_tmp5B);Cyc_Absyndump_dumptps(_tmp29);goto _LLB0;_LLB0:;}goto _LL69;_LL7A: if(
_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 3)goto _LL7C;_tmp2B=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp1D)->f1;_tmp2C=(void*)_tmp2B.field_info;_tmp2D=_tmp2B.targs;_LL7B:{void*
_tmp5E=_tmp2C;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp5F;struct _tuple0*_tmp60;
struct _tuple0*_tmp61;int _tmp62;struct Cyc_Absyn_Tuniondecl*_tmp63;struct Cyc_Absyn_Tuniondecl
_tmp64;struct _tuple0*_tmp65;int _tmp66;struct Cyc_Absyn_Tunionfield*_tmp67;struct
Cyc_Absyn_Tunionfield _tmp68;struct _tuple0*_tmp69;_LLBB: if(*((int*)_tmp5E)!= 0)
goto _LLBD;_tmp5F=((struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp5E)->f1;_tmp60=
_tmp5F.tunion_name;_tmp61=_tmp5F.field_name;_tmp62=_tmp5F.is_xtunion;_LLBC:
_tmp65=_tmp60;_tmp66=_tmp62;_tmp69=_tmp61;goto _LLBE;_LLBD: if(*((int*)_tmp5E)!= 1)
goto _LLBA;_tmp63=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp5E)->f1;_tmp64=*
_tmp63;_tmp65=_tmp64.name;_tmp66=_tmp64.is_xtunion;_tmp67=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp5E)->f2;_tmp68=*_tmp67;_tmp69=_tmp68.name;_LLBE: if(_tmp66)Cyc_Absyndump_dump(
_tag_arr("xtunion ",sizeof(char),9));else{Cyc_Absyndump_dump(_tag_arr("tunion ",
sizeof(char),8));}Cyc_Absyndump_dumpqvar(_tmp65);Cyc_Absyndump_dump(_tag_arr(".",
sizeof(char),2));Cyc_Absyndump_dumpqvar(_tmp69);Cyc_Absyndump_dumptps(_tmp2D);
goto _LLBA;_LLBA:;}goto _LL69;_LL7C: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)
goto _LL7E;_tmp2E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)
_tmp2E != 0)goto _LL7E;_tmp2F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;
if((int)_tmp2F != 2)goto _LL7E;_LL7D: Cyc_Absyndump_dump(_tag_arr("int",sizeof(char),
4));return;_LL7E: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto _LL80;_tmp30=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp30 != 0)goto _LL80;
_tmp31=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((int)_tmp31 != 0)
goto _LL80;_LL7F: Cyc_Absyndump_dump(_tag_arr("signed char",sizeof(char),12));
return;_LL80: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto _LL82;_tmp32=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp32 != 1)goto _LL82;_tmp33=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((int)_tmp33 != 0)goto _LL82;
_LL81: Cyc_Absyndump_dump(_tag_arr("char",sizeof(char),5));return;_LL82: if(_tmp1D
<= (void*)3?1:*((int*)_tmp1D)!= 5)goto _LL84;_tmp34=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f1;if((int)_tmp34 != 0)goto _LL84;_tmp35=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp35 != 1)goto _LL84;_LL83: Cyc_Absyndump_dump(_tag_arr("short",
sizeof(char),6));return;_LL84: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto
_LL86;_tmp36=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp36
!= 1)goto _LL86;_tmp37=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((
int)_tmp37 != 1)goto _LL86;_LL85: Cyc_Absyndump_dump(_tag_arr("unsigned short",
sizeof(char),15));return;_LL86: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto
_LL88;_tmp38=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp38
!= 1)goto _LL88;_tmp39=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((
int)_tmp39 != 2)goto _LL88;_LL87: Cyc_Absyndump_dump(_tag_arr("unsigned int",
sizeof(char),13));return;_LL88: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto
_LL8A;_tmp3A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp3A
!= 0)goto _LL8A;_tmp3B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((
int)_tmp3B != 3)goto _LL8A;if(!Cyc_Absyndump_to_VC)goto _LL8A;_LL89: Cyc_Absyndump_dump(
_tag_arr("__int64",sizeof(char),8));return;_LL8A: if(_tmp1D <= (void*)3?1:*((int*)
_tmp1D)!= 5)goto _LL8C;_tmp3C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;
if((int)_tmp3C != 0)goto _LL8C;_tmp3D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp3D != 3)goto _LL8C;_LL8B: Cyc_Absyndump_dump(_tag_arr("long long",
sizeof(char),10));return;_LL8C: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto
_LL8E;_tmp3E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp3E
!= 1)goto _LL8E;_tmp3F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((
int)_tmp3F != 3)goto _LL8E;if(!Cyc_Absyndump_to_VC)goto _LL8E;_LL8D: Cyc_Absyndump_dump(
_tag_arr("unsigned __int64",sizeof(char),17));return;_LL8E: if(_tmp1D <= (void*)3?
1:*((int*)_tmp1D)!= 5)goto _LL90;_tmp40=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f1;if((int)_tmp40 != 1)goto _LL90;_tmp41=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp41 != 3)goto _LL90;_LL8F: Cyc_Absyndump_dump(_tag_arr("unsigned long long",
sizeof(char),19));return;_LL90: if((int)_tmp1D != 1)goto _LL92;_LL91: Cyc_Absyndump_dump(
_tag_arr("float",sizeof(char),6));return;_LL92: if(_tmp1D <= (void*)3?1:*((int*)
_tmp1D)!= 6)goto _LL94;_tmp42=((struct Cyc_Absyn_DoubleType_struct*)_tmp1D)->f1;
_LL93: if(_tmp42)Cyc_Absyndump_dump(_tag_arr("long double",sizeof(char),12));
else{Cyc_Absyndump_dump(_tag_arr("double",sizeof(char),7));}return;_LL94: if(
_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 9)goto _LL96;_tmp43=((struct Cyc_Absyn_TupleType_struct*)
_tmp1D)->f1;_LL95: Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(
_tmp43);return;_LL96: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 10)goto _LL98;
_tmp44=((struct Cyc_Absyn_AggrType_struct*)_tmp1D)->f1;_tmp45=(void*)_tmp44.aggr_info;
_tmp46=_tmp44.targs;_LL97: {void*_tmp6B;struct _tuple0*_tmp6C;struct _tuple3 _tmp6A=
Cyc_Absyn_aggr_kinded_name(_tmp45);_tmp6B=_tmp6A.f1;_tmp6C=_tmp6A.f2;Cyc_Absyndump_dumpaggr_kind(
_tmp6B);Cyc_Absyndump_dumpqvar(_tmp6C);Cyc_Absyndump_dumptps(_tmp46);return;}
_LL98: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 11)goto _LL9A;_tmp47=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp1D)->f1;_tmp48=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1D)->f2;_LL99: Cyc_Absyndump_dumpaggr_kind(_tmp47);Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpaggrfields(_tmp48);Cyc_Absyndump_dump_char((int)'}');
return;_LL9A: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 12)goto _LL9C;_tmp49=((
struct Cyc_Absyn_EnumType_struct*)_tmp1D)->f1;_LL9B: Cyc_Absyndump_dump(_tag_arr("enum ",
sizeof(char),6));Cyc_Absyndump_dumpqvar(_tmp49);return;_LL9C: if(_tmp1D <= (void*)
3?1:*((int*)_tmp1D)!= 13)goto _LL9E;_tmp4A=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1D)->f1;_LL9D: Cyc_Absyndump_dump(_tag_arr("enum {",sizeof(char),7));Cyc_Absyndump_dumpenumfields(
_tmp4A);Cyc_Absyndump_dump(_tag_arr("}",sizeof(char),2));return;_LL9E: if(_tmp1D
<= (void*)3?1:*((int*)_tmp1D)!= 16)goto _LLA0;_tmp4B=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1D)->f1;_tmp4C=((struct Cyc_Absyn_TypedefType_struct*)_tmp1D)->f2;_tmp4D=((
struct Cyc_Absyn_TypedefType_struct*)_tmp1D)->f4;_LL9F:(Cyc_Absyndump_dumpqvar(
_tmp4B),Cyc_Absyndump_dumptps(_tmp4C));return;_LLA0: if(_tmp1D <= (void*)3?1:*((
int*)_tmp1D)!= 14)goto _LLA2;_tmp4E=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp1D)->f1;_LLA1: Cyc_Absyndump_dump(_tag_arr("sizeof_t<",sizeof(char),10));Cyc_Absyndump_dumpntyp(
_tmp4E);Cyc_Absyndump_dump(_tag_arr(">",sizeof(char),2));return;_LLA2: if(_tmp1D
<= (void*)3?1:*((int*)_tmp1D)!= 15)goto _LLA4;_tmp4F=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp1D)->f1;_LLA3: Cyc_Absyndump_dumprgn(_tmp4F);return;_LLA4: if(_tmp1D <= (void*)
3?1:*((int*)_tmp1D)!= 18)goto _LLA6;_tmp50=((struct Cyc_Absyn_TypeInt_struct*)
_tmp1D)->f1;_LLA5: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp6E;_tmp6E.tag=1;_tmp6E.f1=(int)((unsigned int)_tmp50);{void*_tmp6D[1]={&
_tmp6E};Cyc_Std_aprintf(_tag_arr("`%d",sizeof(char),4),_tag_arr(_tmp6D,sizeof(
void*),1));}}));return;_LLA6: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 17)goto
_LLA8;_tmp51=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp1D)->f1;_LLA7: Cyc_Absyndump_dump(
_tag_arr("tag_t<",sizeof(char),7));Cyc_Absyndump_dumpntyp(_tmp51);Cyc_Absyndump_dump(
_tag_arr(">",sizeof(char),2));return;_LLA8: if((int)_tmp1D != 2)goto _LLAA;_LLA9:
goto _LLAB;_LLAA: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 19)goto _LLAC;_LLAB: goto
_LLAD;_LLAC: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 21)goto _LLAE;_LLAD: goto
_LLAF;_LLAE: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 20)goto _LL69;_LLAF: return;
_LL69:;}void Cyc_Absyndump_dumpvaropt(struct Cyc_Core_Opt*vo){if(vo != 0)Cyc_Absyndump_dump_str((
struct _tagged_arr*)vo->v);}void Cyc_Absyndump_dumpfunarg(struct _tuple1*t){((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),struct Cyc_Core_Opt*))
Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}void
Cyc_Absyndump_dump_rgncmp(struct _tuple6*cmp){struct _tuple6 _tmp70;void*_tmp71;
void*_tmp72;struct _tuple6*_tmp6F=cmp;_tmp70=*_tmp6F;_tmp71=_tmp70.f1;_tmp72=
_tmp70.f2;Cyc_Absyndump_dumpeff(_tmp71);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(
_tmp72);}void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){((void(*)(void(*
f)(struct _tuple6*),struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dump_rgncmp,rgn_po,_tag_arr(",",sizeof(char),2));}void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((
int)'(');for(0;args != 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple1*)
args->hd);if((args->tl != 0?1: c_varargs)?1: cyc_varargs != 0)Cyc_Absyndump_dump_char((
int)',');}if(c_varargs)Cyc_Absyndump_dump(_tag_arr("...",sizeof(char),4));else{
if(cyc_varargs != 0){struct _tuple1*_tmp73=({struct _tuple1*_tmp74=_cycalloc(
sizeof(*_tmp74));_tmp74->f1=cyc_varargs->name;_tmp74->f2=cyc_varargs->tq;_tmp74->f3=(
void*)cyc_varargs->type;_tmp74;});Cyc_Absyndump_dump(_tag_arr("...",sizeof(char),
4));if(cyc_varargs->inject)Cyc_Absyndump_dump(_tag_arr(" inject ",sizeof(char),9));
Cyc_Absyndump_dumpfunarg(_tmp73);}}if(effopt != 0){Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpeff((
void*)effopt->v);}if(rgn_po != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(
rgn_po);}Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumptyp(void*t){((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _tmp75;_tmp75.q_const=0;_tmp75.q_volatile=0;_tmp75.q_restrict=
0;_tmp75;}),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpdesignator(
void*d){void*_tmp76=d;struct Cyc_Absyn_Exp*_tmp77;struct _tagged_arr*_tmp78;_LLC0:
if(*((int*)_tmp76)!= 0)goto _LLC2;_tmp77=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp76)->f1;_LLC1: Cyc_Absyndump_dump(_tag_arr(".[",sizeof(char),3));Cyc_Absyndump_dumpexp(
_tmp77);Cyc_Absyndump_dump_char((int)']');goto _LLBF;_LLC2: if(*((int*)_tmp76)!= 1)
goto _LLBF;_tmp78=((struct Cyc_Absyn_FieldName_struct*)_tmp76)->f1;_LLC3: Cyc_Absyndump_dump_char((
int)'.');Cyc_Absyndump_dump_nospace(*_tmp78);goto _LLBF;_LLBF:;}struct _tuple9{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};void Cyc_Absyndump_dumpde(struct
_tuple9*de){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*de).f1,_tag_arr("",
sizeof(char),1),_tag_arr("=",sizeof(char),2),_tag_arr("=",sizeof(char),2));Cyc_Absyndump_dumpexp((*
de).f2);}void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){((
void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec,inprec,es,_tag_arr("",sizeof(char),1),_tag_arr("",
sizeof(char),1),_tag_arr(",",sizeof(char),2));}void Cyc_Absyndump_dumpexp_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= 
myprec)Cyc_Absyndump_dump_nospace(_tag_arr("(",sizeof(char),2));{void*_tmp79=(
void*)e->r;void*_tmp7A;void*_tmp7B;char _tmp7C;void*_tmp7D;void*_tmp7E;short
_tmp7F;void*_tmp80;void*_tmp81;int _tmp82;void*_tmp83;void*_tmp84;int _tmp85;void*
_tmp86;void*_tmp87;long long _tmp88;void*_tmp89;struct _tagged_arr _tmp8A;void*
_tmp8B;void*_tmp8C;struct _tagged_arr _tmp8D;struct _tuple0*_tmp8E;struct _tuple0*
_tmp8F;void*_tmp90;struct Cyc_List_List*_tmp91;struct Cyc_Absyn_Exp*_tmp92;struct
Cyc_Core_Opt*_tmp93;struct Cyc_Absyn_Exp*_tmp94;struct Cyc_Absyn_Exp*_tmp95;void*
_tmp96;struct Cyc_Absyn_Exp*_tmp97;void*_tmp98;struct Cyc_Absyn_Exp*_tmp99;void*
_tmp9A;struct Cyc_Absyn_Exp*_tmp9B;void*_tmp9C;struct Cyc_Absyn_Exp*_tmp9D;struct
Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*_tmpA0;
struct Cyc_Absyn_Exp*_tmpA1;struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_List_List*_tmpA3;
struct Cyc_Absyn_Exp*_tmpA4;struct Cyc_List_List*_tmpA5;struct Cyc_Absyn_Exp*_tmpA6;
struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_Absyn_Exp*_tmpA8;void*_tmpA9;struct Cyc_Absyn_Exp*
_tmpAA;struct Cyc_Absyn_Exp*_tmpAB;struct Cyc_Absyn_Exp*_tmpAC;struct Cyc_Absyn_Exp*
_tmpAD;void*_tmpAE;struct Cyc_Absyn_Exp*_tmpAF;void*_tmpB0;void*_tmpB1;struct
_tagged_arr*_tmpB2;void*_tmpB3;void*_tmpB4;unsigned int _tmpB5;struct Cyc_List_List*
_tmpB6;void*_tmpB7;struct Cyc_Absyn_Exp*_tmpB8;struct Cyc_Absyn_Exp*_tmpB9;struct
_tagged_arr*_tmpBA;struct Cyc_Absyn_Exp*_tmpBB;struct _tagged_arr*_tmpBC;struct Cyc_Absyn_Exp*
_tmpBD;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_List_List*_tmpBF;struct _tuple1*
_tmpC0;struct Cyc_List_List*_tmpC1;struct Cyc_List_List*_tmpC2;struct Cyc_Absyn_Vardecl*
_tmpC3;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Exp*_tmpC5;struct _tuple0*
_tmpC6;struct Cyc_List_List*_tmpC7;struct Cyc_List_List*_tmpC8;struct Cyc_List_List*
_tmpC9;struct Cyc_List_List*_tmpCA;struct Cyc_Absyn_Tunionfield*_tmpCB;struct
_tuple0*_tmpCC;struct _tuple0*_tmpCD;struct Cyc_Absyn_MallocInfo _tmpCE;int _tmpCF;
struct Cyc_Absyn_Exp*_tmpD0;void**_tmpD1;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Core_Opt*
_tmpD3;struct Cyc_List_List*_tmpD4;struct Cyc_Absyn_Stmt*_tmpD5;struct Cyc_Absyn_Fndecl*
_tmpD6;struct Cyc_Absyn_Exp*_tmpD7;_LLC5: if(*((int*)_tmp79)!= 0)goto _LLC7;_tmp7A=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp79)->f1;if(_tmp7A <= (void*)1?1:*((
int*)_tmp7A)!= 0)goto _LLC7;_tmp7B=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp7A)->f1;
_tmp7C=((struct Cyc_Absyn_Char_c_struct*)_tmp7A)->f2;_LLC6: Cyc_Absyndump_dump_char((
int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp7C));Cyc_Absyndump_dump_char((
int)'\'');goto _LLC4;_LLC7: if(*((int*)_tmp79)!= 0)goto _LLC9;_tmp7D=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp79)->f1;if(_tmp7D <= (void*)1?1:*((int*)
_tmp7D)!= 1)goto _LLC9;_tmp7E=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp7D)->f1;
_tmp7F=((struct Cyc_Absyn_Short_c_struct*)_tmp7D)->f2;_LLC8: Cyc_Absyndump_dump((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpD9;_tmpD9.tag=1;_tmpD9.f1=(
int)((unsigned int)((int)_tmp7F));{void*_tmpD8[1]={& _tmpD9};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(char),3),_tag_arr(_tmpD8,sizeof(void*),1));}}));goto _LLC4;
_LLC9: if(*((int*)_tmp79)!= 0)goto _LLCB;_tmp80=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp79)->f1;if(_tmp80 <= (void*)1?1:*((int*)_tmp80)!= 2)goto _LLCB;_tmp81=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp80)->f1;if((int)_tmp81 != 0)goto _LLCB;_tmp82=((
struct Cyc_Absyn_Int_c_struct*)_tmp80)->f2;_LLCA: Cyc_Absyndump_dump((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpDB;_tmpDB.tag=1;_tmpDB.f1=(int)((
unsigned int)_tmp82);{void*_tmpDA[1]={& _tmpDB};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(char),3),_tag_arr(_tmpDA,sizeof(void*),1));}}));goto _LLC4;_LLCB: if(*((int*)
_tmp79)!= 0)goto _LLCD;_tmp83=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp79)->f1;
if(_tmp83 <= (void*)1?1:*((int*)_tmp83)!= 2)goto _LLCD;_tmp84=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp83)->f1;if((int)_tmp84 != 1)goto _LLCD;_tmp85=((struct Cyc_Absyn_Int_c_struct*)
_tmp83)->f2;_LLCC: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmpDD;_tmpDD.tag=1;_tmpDD.f1=(unsigned int)_tmp85;{void*_tmpDC[1]={& _tmpDD};Cyc_Std_aprintf(
_tag_arr("%u",sizeof(char),3),_tag_arr(_tmpDC,sizeof(void*),1));}}));goto _LLC4;
_LLCD: if(*((int*)_tmp79)!= 0)goto _LLCF;_tmp86=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp79)->f1;if(_tmp86 <= (void*)1?1:*((int*)_tmp86)!= 3)goto _LLCF;_tmp87=(void*)((
struct Cyc_Absyn_LongLong_c_struct*)_tmp86)->f1;_tmp88=((struct Cyc_Absyn_LongLong_c_struct*)
_tmp86)->f2;_LLCE: Cyc_Absyndump_dump(_tag_arr("<<FIX LONG LONG CONSTANT>>",
sizeof(char),27));goto _LLC4;_LLCF: if(*((int*)_tmp79)!= 0)goto _LLD1;_tmp89=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp79)->f1;if(_tmp89 <= (void*)1?1:*((int*)
_tmp89)!= 4)goto _LLD1;_tmp8A=((struct Cyc_Absyn_Float_c_struct*)_tmp89)->f1;_LLD0:
Cyc_Absyndump_dump(_tmp8A);goto _LLC4;_LLD1: if(*((int*)_tmp79)!= 0)goto _LLD3;
_tmp8B=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp79)->f1;if((int)_tmp8B != 0)
goto _LLD3;_LLD2: Cyc_Absyndump_dump(_tag_arr("NULL",sizeof(char),5));goto _LLC4;
_LLD3: if(*((int*)_tmp79)!= 0)goto _LLD5;_tmp8C=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp79)->f1;if(_tmp8C <= (void*)1?1:*((int*)_tmp8C)!= 5)goto _LLD5;_tmp8D=((struct
Cyc_Absyn_String_c_struct*)_tmp8C)->f1;_LLD4: Cyc_Absyndump_dump_char((int)'"');
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp8D));Cyc_Absyndump_dump_char((
int)'"');goto _LLC4;_LLD5: if(*((int*)_tmp79)!= 2)goto _LLD7;_tmp8E=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp79)->f1;_LLD6: _tmp8F=_tmp8E;goto _LLD8;_LLD7: if(*((int*)_tmp79)!= 1)goto _LLD9;
_tmp8F=((struct Cyc_Absyn_Var_e_struct*)_tmp79)->f1;_LLD8: Cyc_Absyndump_dumpqvar(
_tmp8F);goto _LLC4;_LLD9: if(*((int*)_tmp79)!= 3)goto _LLDB;_tmp90=(void*)((struct
Cyc_Absyn_Primop_e_struct*)_tmp79)->f1;_tmp91=((struct Cyc_Absyn_Primop_e_struct*)
_tmp79)->f2;_LLDA: {struct _tagged_arr _tmpDE=Cyc_Absynpp_prim2str(_tmp90);switch(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp91)){case 1: _LL127: if(_tmp90 == (
void*)19){Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp91))->hd);Cyc_Absyndump_dump(_tag_arr(".size",sizeof(char),6));}
else{Cyc_Absyndump_dump(_tmpDE);Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp91))->hd);}break;case 2: _LL128: Cyc_Absyndump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp91))->hd);
Cyc_Absyndump_dump(_tmpDE);Cyc_Absyndump_dump_char((int)' ');Cyc_Absyndump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp91->tl))->hd);
break;default: _LL129:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpDF=
_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=({struct Cyc_Core_Failure_struct _tmpE0;
_tmpE0.tag=Cyc_Core_Failure;_tmpE0.f1=_tag_arr("Absyndump -- Bad number of arguments to primop",
sizeof(char),47);_tmpE0;});_tmpDF;}));}goto _LLC4;}_LLDB: if(*((int*)_tmp79)!= 4)
goto _LLDD;_tmp92=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp79)->f1;_tmp93=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp79)->f2;_tmp94=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp79)->f3;_LLDC: Cyc_Absyndump_dumpexp_prec(myprec,_tmp92);if(_tmp93 != 0)Cyc_Absyndump_dump(
Cyc_Absynpp_prim2str((void*)_tmp93->v));Cyc_Absyndump_dump_nospace(_tag_arr("=",
sizeof(char),2));Cyc_Absyndump_dumpexp_prec(myprec,_tmp94);goto _LLC4;_LLDD: if(*((
int*)_tmp79)!= 5)goto _LLDF;_tmp95=((struct Cyc_Absyn_Increment_e_struct*)_tmp79)->f1;
_tmp96=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp79)->f2;if((int)_tmp96 != 
0)goto _LLDF;_LLDE: Cyc_Absyndump_dump(_tag_arr("++",sizeof(char),3));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp95);goto _LLC4;_LLDF: if(*((int*)_tmp79)!= 5)goto _LLE1;_tmp97=((struct
Cyc_Absyn_Increment_e_struct*)_tmp79)->f1;_tmp98=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp79)->f2;if((int)_tmp98 != 2)goto _LLE1;_LLE0: Cyc_Absyndump_dump(_tag_arr("--",
sizeof(char),3));Cyc_Absyndump_dumpexp_prec(myprec,_tmp97);goto _LLC4;_LLE1: if(*((
int*)_tmp79)!= 5)goto _LLE3;_tmp99=((struct Cyc_Absyn_Increment_e_struct*)_tmp79)->f1;
_tmp9A=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp79)->f2;if((int)_tmp9A != 
1)goto _LLE3;_LLE2: Cyc_Absyndump_dumpexp_prec(myprec,_tmp99);Cyc_Absyndump_dump(
_tag_arr("++",sizeof(char),3));goto _LLC4;_LLE3: if(*((int*)_tmp79)!= 5)goto _LLE5;
_tmp9B=((struct Cyc_Absyn_Increment_e_struct*)_tmp79)->f1;_tmp9C=(void*)((struct
Cyc_Absyn_Increment_e_struct*)_tmp79)->f2;if((int)_tmp9C != 3)goto _LLE5;_LLE4: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp9B);Cyc_Absyndump_dump(_tag_arr("--",sizeof(char),3));goto _LLC4;_LLE5:
if(*((int*)_tmp79)!= 6)goto _LLE7;_tmp9D=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp79)->f1;_tmp9E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp79)->f2;_tmp9F=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp79)->f3;_LLE6: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp9D);Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,
_tmp9E);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,
_tmp9F);goto _LLC4;_LLE7: if(*((int*)_tmp79)!= 7)goto _LLE9;_tmpA0=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp79)->f1;_tmpA1=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp79)->f2;_LLE8: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumpexp_prec(myprec,_tmpA0);Cyc_Absyndump_dump_char((int)',');
Cyc_Absyndump_dumpexp_prec(myprec,_tmpA1);Cyc_Absyndump_dump_char((int)')');goto
_LLC4;_LLE9: if(*((int*)_tmp79)!= 8)goto _LLEB;_tmpA2=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp79)->f1;_tmpA3=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp79)->f2;_LLEA:
_tmpA4=_tmpA2;_tmpA5=_tmpA3;goto _LLEC;_LLEB: if(*((int*)_tmp79)!= 9)goto _LLED;
_tmpA4=((struct Cyc_Absyn_FnCall_e_struct*)_tmp79)->f1;_tmpA5=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp79)->f2;_LLEC: Cyc_Absyndump_dumpexp_prec(myprec,_tmpA4);Cyc_Absyndump_dump_nospace(
_tag_arr("(",sizeof(char),2));Cyc_Absyndump_dumpexps_prec(20,_tmpA5);Cyc_Absyndump_dump_nospace(
_tag_arr(")",sizeof(char),2));goto _LLC4;_LLED: if(*((int*)_tmp79)!= 10)goto _LLEF;
_tmpA6=((struct Cyc_Absyn_Throw_e_struct*)_tmp79)->f1;_LLEE: Cyc_Absyndump_dump(
_tag_arr("throw",sizeof(char),6));Cyc_Absyndump_dumpexp_prec(myprec,_tmpA6);goto
_LLC4;_LLEF: if(*((int*)_tmp79)!= 11)goto _LLF1;_tmpA7=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp79)->f1;_LLF0: _tmpA8=_tmpA7;goto _LLF2;_LLF1: if(*((int*)_tmp79)!= 12)goto
_LLF3;_tmpA8=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp79)->f1;_LLF2: Cyc_Absyndump_dumpexp_prec(
inprec,_tmpA8);goto _LLC4;_LLF3: if(*((int*)_tmp79)!= 13)goto _LLF5;_tmpA9=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp79)->f1;_tmpAA=((struct Cyc_Absyn_Cast_e_struct*)
_tmp79)->f2;_LLF4: Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmpA9);
Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmpAA);goto
_LLC4;_LLF5: if(*((int*)_tmp79)!= 14)goto _LLF7;_tmpAB=((struct Cyc_Absyn_Address_e_struct*)
_tmp79)->f1;_LLF6: Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(
myprec,_tmpAB);goto _LLC4;_LLF7: if(*((int*)_tmp79)!= 15)goto _LLF9;_tmpAC=((struct
Cyc_Absyn_New_e_struct*)_tmp79)->f1;_tmpAD=((struct Cyc_Absyn_New_e_struct*)
_tmp79)->f2;_LLF8: Cyc_Absyndump_dump(_tag_arr("new ",sizeof(char),5));Cyc_Absyndump_dumpexp_prec(
myprec,_tmpAD);goto _LLC4;_LLF9: if(*((int*)_tmp79)!= 16)goto _LLFB;_tmpAE=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp79)->f1;_LLFA: Cyc_Absyndump_dump(
_tag_arr("sizeof(",sizeof(char),8));Cyc_Absyndump_dumptyp(_tmpAE);Cyc_Absyndump_dump_char((
int)')');goto _LLC4;_LLFB: if(*((int*)_tmp79)!= 17)goto _LLFD;_tmpAF=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp79)->f1;_LLFC: Cyc_Absyndump_dump(_tag_arr("sizeof(",sizeof(char),8));Cyc_Absyndump_dumpexp(
_tmpAF);Cyc_Absyndump_dump_char((int)')');goto _LLC4;_LLFD: if(*((int*)_tmp79)!= 
18)goto _LLFF;_tmpB0=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp79)->f1;
_tmpB1=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp79)->f2;if(*((int*)_tmpB1)
!= 0)goto _LLFF;_tmpB2=((struct Cyc_Absyn_StructField_struct*)_tmpB1)->f1;_LLFE:
Cyc_Absyndump_dump(_tag_arr("offsetof(",sizeof(char),10));Cyc_Absyndump_dumptyp(
_tmpB0);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmpB2);
Cyc_Absyndump_dump_char((int)')');goto _LLC4;_LLFF: if(*((int*)_tmp79)!= 18)goto
_LL101;_tmpB3=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp79)->f1;_tmpB4=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp79)->f2;if(*((int*)_tmpB4)!= 1)
goto _LL101;_tmpB5=((struct Cyc_Absyn_TupleIndex_struct*)_tmpB4)->f1;_LL100: Cyc_Absyndump_dump(
_tag_arr("offsetof(",sizeof(char),10));Cyc_Absyndump_dumptyp(_tmpB3);Cyc_Absyndump_dump_char((
int)',');Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmpE2;_tmpE2.tag=1;_tmpE2.f1=(int)_tmpB5;{void*_tmpE1[1]={& _tmpE2};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(char),3),_tag_arr(_tmpE1,sizeof(void*),1));}}));Cyc_Absyndump_dump_char((
int)')');goto _LLC4;_LL101: if(*((int*)_tmp79)!= 19)goto _LL103;_tmpB6=((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp79)->f1;_tmpB7=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp79)->f2;_LL102:
Cyc_Absyndump_dump(_tag_arr("__gen(",sizeof(char),7));Cyc_Absyndump_dumptvars(
_tmpB6);Cyc_Absyndump_dumptyp(_tmpB7);Cyc_Absyndump_dump_char((int)')');goto
_LLC4;_LL103: if(*((int*)_tmp79)!= 20)goto _LL105;_tmpB8=((struct Cyc_Absyn_Deref_e_struct*)
_tmp79)->f1;_LL104: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(
myprec,_tmpB8);goto _LLC4;_LL105: if(*((int*)_tmp79)!= 21)goto _LL107;_tmpB9=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp79)->f1;_tmpBA=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp79)->f2;_LL106: Cyc_Absyndump_dumpexp_prec(myprec,_tmpB9);Cyc_Absyndump_dump_char((
int)'.');Cyc_Absyndump_dump_nospace(*_tmpBA);goto _LLC4;_LL107: if(*((int*)_tmp79)
!= 22)goto _LL109;_tmpBB=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp79)->f1;_tmpBC=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp79)->f2;_LL108: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpBB);Cyc_Absyndump_dump_nospace(_tag_arr("->",sizeof(char),3));Cyc_Absyndump_dump_nospace(*
_tmpBC);goto _LLC4;_LL109: if(*((int*)_tmp79)!= 23)goto _LL10B;_tmpBD=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp79)->f1;_tmpBE=((struct Cyc_Absyn_Subscript_e_struct*)_tmp79)->f2;_LL10A: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpBD);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmpBE);
Cyc_Absyndump_dump_char((int)']');goto _LLC4;_LL10B: if(*((int*)_tmp79)!= 24)goto
_LL10D;_tmpBF=((struct Cyc_Absyn_Tuple_e_struct*)_tmp79)->f1;_LL10C: Cyc_Absyndump_dump(
_tag_arr("$(",sizeof(char),3));Cyc_Absyndump_dumpexps_prec(20,_tmpBF);Cyc_Absyndump_dump_char((
int)')');goto _LLC4;_LL10D: if(*((int*)_tmp79)!= 25)goto _LL10F;_tmpC0=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp79)->f1;_tmpC1=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp79)->f2;_LL10E:
Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp((*_tmpC0).f3);Cyc_Absyndump_dump_char((
int)')');((void(*)(void(*f)(struct _tuple9*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmpC1,_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(
char),2),_tag_arr(",",sizeof(char),2));goto _LLC4;_LL10F: if(*((int*)_tmp79)!= 26)
goto _LL111;_tmpC2=((struct Cyc_Absyn_Array_e_struct*)_tmp79)->f1;_LL110:((void(*)(
void(*f)(struct _tuple9*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmpC2,_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(char),2),_tag_arr(",",
sizeof(char),2));goto _LLC4;_LL111: if(*((int*)_tmp79)!= 27)goto _LL113;_tmpC3=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp79)->f1;_tmpC4=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp79)->f2;_tmpC5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp79)->f3;_LL112:
Cyc_Absyndump_dump(_tag_arr("new {for",sizeof(char),9));Cyc_Absyndump_dump_str((*
_tmpC3->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmpC4);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmpC5);Cyc_Absyndump_dump_char((
int)'}');goto _LLC4;_LL113: if(*((int*)_tmp79)!= 28)goto _LL115;_tmpC6=((struct Cyc_Absyn_Struct_e_struct*)
_tmp79)->f1;_tmpC7=((struct Cyc_Absyn_Struct_e_struct*)_tmp79)->f2;_tmpC8=((
struct Cyc_Absyn_Struct_e_struct*)_tmp79)->f3;_LL114: Cyc_Absyndump_dumpqvar(
_tmpC6);Cyc_Absyndump_dump_char((int)'{');if(_tmpC7 != 0)Cyc_Absyndump_dumptps(
_tmpC7);((void(*)(void(*f)(struct _tuple9*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmpC8,_tag_arr("",sizeof(char),1),_tag_arr("}",sizeof(char),
2),_tag_arr(",",sizeof(char),2));goto _LLC4;_LL115: if(*((int*)_tmp79)!= 29)goto
_LL117;_tmpC9=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp79)->f2;_LL116:((void(*)(
void(*f)(struct _tuple9*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmpC9,_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(char),2),_tag_arr(",",
sizeof(char),2));goto _LLC4;_LL117: if(*((int*)_tmp79)!= 30)goto _LL119;_tmpCA=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp79)->f1;_tmpCB=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp79)->f3;_LL118: Cyc_Absyndump_dumpqvar(_tmpCB->name);if(_tmpCA != 0)((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,
_tmpCA,_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(char),2),_tag_arr(",",
sizeof(char),2));goto _LLC4;_LL119: if(*((int*)_tmp79)!= 31)goto _LL11B;_tmpCC=((
struct Cyc_Absyn_Enum_e_struct*)_tmp79)->f1;_LL11A: Cyc_Absyndump_dumpqvar(_tmpCC);
goto _LLC4;_LL11B: if(*((int*)_tmp79)!= 32)goto _LL11D;_tmpCD=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp79)->f1;_LL11C: Cyc_Absyndump_dumpqvar(_tmpCD);goto _LLC4;_LL11D: if(*((int*)
_tmp79)!= 33)goto _LL11F;_tmpCE=((struct Cyc_Absyn_Malloc_e_struct*)_tmp79)->f1;
_tmpCF=_tmpCE.is_calloc;_tmpD0=_tmpCE.rgn;_tmpD1=_tmpCE.elt_type;_tmpD2=_tmpCE.num_elts;
_LL11E: if(_tmpCF){if(_tmpD0 != 0){Cyc_Absyndump_dump(_tag_arr("rcalloc(",sizeof(
char),9));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmpD0));Cyc_Absyndump_dump(
_tag_arr(",",sizeof(char),2));}else{Cyc_Absyndump_dump(_tag_arr("calloc",sizeof(
char),7));}Cyc_Absyndump_dumpexp(_tmpD2);Cyc_Absyndump_dump(_tag_arr(",",sizeof(
char),2));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmpD1)),0));Cyc_Absyndump_dump(_tag_arr(")",sizeof(char),2));}else{if(_tmpD0 != 
0){Cyc_Absyndump_dump(_tag_arr("rmalloc(",sizeof(char),9));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_check_null(_tmpD0));Cyc_Absyndump_dump(_tag_arr(",",
sizeof(char),2));}else{Cyc_Absyndump_dump(_tag_arr("malloc(",sizeof(char),8));}
if(_tmpD1 != 0)Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*
_tmpD1,0),_tmpD2,0));else{Cyc_Absyndump_dumpexp(_tmpD2);}Cyc_Absyndump_dump(
_tag_arr(")",sizeof(char),2));}goto _LLC4;_LL11F: if(*((int*)_tmp79)!= 34)goto
_LL121;_tmpD3=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp79)->f1;_tmpD4=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp79)->f2;_LL120:((void(*)(void(*f)(
struct _tuple9*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr
end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmpD4,
_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(char),2),_tag_arr(",",sizeof(
char),2));goto _LLC4;_LL121: if(*((int*)_tmp79)!= 35)goto _LL123;_tmpD5=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp79)->f1;_LL122: Cyc_Absyndump_dump_nospace(_tag_arr("({",sizeof(char),3));Cyc_Absyndump_dumpstmt(
_tmpD5);Cyc_Absyndump_dump_nospace(_tag_arr("})",sizeof(char),3));goto _LLC4;
_LL123: if(*((int*)_tmp79)!= 36)goto _LL125;_tmpD6=((struct Cyc_Absyn_Codegen_e_struct*)
_tmp79)->f1;_LL124: Cyc_Absyndump_dump(_tag_arr("codegen(",sizeof(char),9));Cyc_Absyndump_dumpdecl(({
struct Cyc_Absyn_Decl*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->r=(void*)((void*)({
struct Cyc_Absyn_Fn_d_struct*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct
Cyc_Absyn_Fn_d_struct _tmpE5;_tmpE5.tag=1;_tmpE5.f1=_tmpD6;_tmpE5;});_tmpE4;}));
_tmpE3->loc=e->loc;_tmpE3;}));Cyc_Absyndump_dump(_tag_arr(")",sizeof(char),2));
goto _LLC4;_LL125: if(*((int*)_tmp79)!= 37)goto _LLC4;_tmpD7=((struct Cyc_Absyn_Fill_e_struct*)
_tmp79)->f1;_LL126: Cyc_Absyndump_dump(_tag_arr("fill(",sizeof(char),6));Cyc_Absyndump_dumpexp(
_tmpD7);Cyc_Absyndump_dump(_tag_arr(")",sizeof(char),2));goto _LLC4;_LLC4:;}if(
inprec >= myprec)Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*e){Cyc_Absyndump_dumpexp_prec(0,e);}void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmpE6=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmpE6->where_clause == 0?(void*)(
_tmpE6->pattern)->r == (void*)0: 0)Cyc_Absyndump_dump(_tag_arr("default:",sizeof(
char),9));else{Cyc_Absyndump_dump(_tag_arr("case",sizeof(char),5));Cyc_Absyndump_dumppat(
_tmpE6->pattern);if(_tmpE6->where_clause != 0){Cyc_Absyndump_dump(_tag_arr("&&",
sizeof(char),3));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmpE6->where_clause));}
Cyc_Absyndump_dump_nospace(_tag_arr(":",sizeof(char),2));}Cyc_Absyndump_dumpstmt(
_tmpE6->body);}}void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s){void*_tmpE7=(
void*)s->r;struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_Absyn_Stmt*_tmpE9;struct Cyc_Absyn_Stmt*
_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*
_tmpED;struct Cyc_Absyn_Stmt*_tmpEE;struct Cyc_Absyn_Stmt*_tmpEF;struct _tuple2
_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Stmt*_tmpF2;struct _tagged_arr*
_tmpF3;struct Cyc_Absyn_Exp*_tmpF4;struct _tuple2 _tmpF5;struct Cyc_Absyn_Exp*_tmpF6;
struct _tuple2 _tmpF7;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_Absyn_Stmt*_tmpF9;
struct Cyc_Absyn_ForArrayInfo _tmpFA;struct Cyc_List_List*_tmpFB;struct _tuple2
_tmpFC;struct Cyc_Absyn_Exp*_tmpFD;struct _tuple2 _tmpFE;struct Cyc_Absyn_Exp*_tmpFF;
struct Cyc_Absyn_Stmt*_tmp100;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_List_List*
_tmp102;struct Cyc_Absyn_Decl*_tmp103;struct Cyc_Absyn_Stmt*_tmp104;struct
_tagged_arr*_tmp105;struct Cyc_Absyn_Stmt*_tmp106;struct Cyc_Absyn_Stmt*_tmp107;
struct _tuple2 _tmp108;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp10A;
struct Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10C;struct Cyc_List_List*
_tmp10D;struct Cyc_Absyn_Stmt*_tmp10E;struct Cyc_List_List*_tmp10F;struct Cyc_Absyn_Tvar*
_tmp110;struct Cyc_Absyn_Vardecl*_tmp111;int _tmp112;struct Cyc_Absyn_Stmt*_tmp113;
struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Stmt*_tmp115;struct Cyc_Absyn_Stmt*
_tmp116;_LL12C: if((int)_tmpE7 != 0)goto _LL12E;_LL12D: Cyc_Absyndump_dump_semi();
goto _LL12B;_LL12E: if(_tmpE7 <= (void*)1?1:*((int*)_tmpE7)!= 0)goto _LL130;_tmpE8=((
struct Cyc_Absyn_Exp_s_struct*)_tmpE7)->f1;_LL12F: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dumpexp(_tmpE8);Cyc_Absyndump_dump_semi();goto _LL12B;_LL130: if(
_tmpE7 <= (void*)1?1:*((int*)_tmpE7)!= 1)goto _LL132;_tmpE9=((struct Cyc_Absyn_Seq_s_struct*)
_tmpE7)->f1;_tmpEA=((struct Cyc_Absyn_Seq_s_struct*)_tmpE7)->f2;_LL131: if(Cyc_Absynpp_is_declaration(
_tmpE9)){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmpE9);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dumpstmt(_tmpE9);}if(Cyc_Absynpp_is_declaration(
_tmpEA)){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmpEA);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dumpstmt(_tmpEA);}goto _LL12B;_LL132: if(_tmpE7 <= (
void*)1?1:*((int*)_tmpE7)!= 2)goto _LL134;_tmpEB=((struct Cyc_Absyn_Return_s_struct*)
_tmpE7)->f1;if(_tmpEB != 0)goto _LL134;_LL133: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("return;",sizeof(char),8));goto _LL12B;_LL134: if(_tmpE7 <= (void*)1?1:*((
int*)_tmpE7)!= 2)goto _LL136;_tmpEC=((struct Cyc_Absyn_Return_s_struct*)_tmpE7)->f1;
_LL135: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("return",
sizeof(char),7));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmpEC));
Cyc_Absyndump_dump_semi();goto _LL12B;_LL136: if(_tmpE7 <= (void*)1?1:*((int*)
_tmpE7)!= 3)goto _LL138;_tmpED=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpE7)->f1;
_tmpEE=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpE7)->f2;_tmpEF=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpE7)->f3;_LL137: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("if(",
sizeof(char),4));Cyc_Absyndump_dumpexp(_tmpED);{void*_tmp117=(void*)_tmpEE->r;
_LL15D: if(_tmp117 <= (void*)1?1:*((int*)_tmp117)!= 1)goto _LL15F;_LL15E: goto _LL160;
_LL15F: if(_tmp117 <= (void*)1?1:*((int*)_tmp117)!= 12)goto _LL161;_LL160: goto
_LL162;_LL161: if(_tmp117 <= (void*)1?1:*((int*)_tmp117)!= 3)goto _LL163;_LL162:
goto _LL164;_LL163: if(_tmp117 <= (void*)1?1:*((int*)_tmp117)!= 15)goto _LL165;
_LL164: Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(char),3));Cyc_Absyndump_dumpstmt(
_tmpEE);Cyc_Absyndump_dump_char((int)'}');goto _LL15C;_LL165:;_LL166: Cyc_Absyndump_dump_char((
int)')');Cyc_Absyndump_dumpstmt(_tmpEE);_LL15C:;}{void*_tmp118=(void*)_tmpEF->r;
_LL168: if((int)_tmp118 != 0)goto _LL16A;_LL169: goto _LL167;_LL16A:;_LL16B: Cyc_Absyndump_dump(
_tag_arr("else{",sizeof(char),6));Cyc_Absyndump_dumpstmt(_tmpEF);Cyc_Absyndump_dump_char((
int)'}');goto _LL167;_LL167:;}goto _LL12B;_LL138: if(_tmpE7 <= (void*)1?1:*((int*)
_tmpE7)!= 4)goto _LL13A;_tmpF0=((struct Cyc_Absyn_While_s_struct*)_tmpE7)->f1;
_tmpF1=_tmpF0.f1;_tmpF2=((struct Cyc_Absyn_While_s_struct*)_tmpE7)->f2;_LL139: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("while(",sizeof(char),7));Cyc_Absyndump_dumpexp(
_tmpF1);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(char),3));Cyc_Absyndump_dumpstmt(
_tmpF2);Cyc_Absyndump_dump_char((int)'}');goto _LL12B;_LL13A: if(_tmpE7 <= (void*)1?
1:*((int*)_tmpE7)!= 5)goto _LL13C;_LL13B: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("break;",sizeof(char),7));goto _LL12B;_LL13C: if(_tmpE7 <= (void*)1?1:*((
int*)_tmpE7)!= 6)goto _LL13E;_LL13D: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("continue;",sizeof(char),10));goto _LL12B;_LL13E: if(_tmpE7 <= (void*)1?1:*((
int*)_tmpE7)!= 7)goto _LL140;_tmpF3=((struct Cyc_Absyn_Goto_s_struct*)_tmpE7)->f1;
_LL13F: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("goto",sizeof(
char),5));Cyc_Absyndump_dump_str(_tmpF3);Cyc_Absyndump_dump_semi();goto _LL12B;
_LL140: if(_tmpE7 <= (void*)1?1:*((int*)_tmpE7)!= 8)goto _LL142;_tmpF4=((struct Cyc_Absyn_For_s_struct*)
_tmpE7)->f1;_tmpF5=((struct Cyc_Absyn_For_s_struct*)_tmpE7)->f2;_tmpF6=_tmpF5.f1;
_tmpF7=((struct Cyc_Absyn_For_s_struct*)_tmpE7)->f3;_tmpF8=_tmpF7.f1;_tmpF9=((
struct Cyc_Absyn_For_s_struct*)_tmpE7)->f4;_LL141: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(_tag_arr("for(",sizeof(char),5));Cyc_Absyndump_dumpexp(_tmpF4);
Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmpF6);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmpF8);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(
char),3));Cyc_Absyndump_dumpstmt(_tmpF9);Cyc_Absyndump_dump_char((int)'}');goto
_LL12B;_LL142: if(_tmpE7 <= (void*)1?1:*((int*)_tmpE7)!= 19)goto _LL144;_tmpFA=((
struct Cyc_Absyn_ForArray_s_struct*)_tmpE7)->f1;_tmpFB=_tmpFA.defns;_tmpFC=_tmpFA.condition;
_tmpFD=_tmpFC.f1;_tmpFE=_tmpFA.delta;_tmpFF=_tmpFE.f1;_tmp100=_tmpFA.body;_LL143:
Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("forarray(",sizeof(
char),10));for(0;_tmpFB != 0;_tmpFB=_tmpFB->tl){Cyc_Absyndump_dumpvardecl((struct
Cyc_Absyn_Vardecl*)_tmpFB->hd,s->loc);}Cyc_Absyndump_dumpexp(_tmpFD);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmpFF);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(
char),3));Cyc_Absyndump_dumpstmt(_tmp100);Cyc_Absyndump_dump_char((int)'}');goto
_LL12B;_LL144: if(_tmpE7 <= (void*)1?1:*((int*)_tmpE7)!= 9)goto _LL146;_tmp101=((
struct Cyc_Absyn_Switch_s_struct*)_tmpE7)->f1;_tmp102=((struct Cyc_Absyn_Switch_s_struct*)
_tmpE7)->f2;_LL145: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("switch(",
sizeof(char),8));Cyc_Absyndump_dumpexp(_tmp101);Cyc_Absyndump_dump_nospace(
_tag_arr("){",sizeof(char),3));Cyc_Absyndump_dumpswitchclauses(_tmp102);Cyc_Absyndump_dump_char((
int)'}');goto _LL12B;_LL146: if(_tmpE7 <= (void*)1?1:*((int*)_tmpE7)!= 12)goto
_LL148;_tmp103=((struct Cyc_Absyn_Decl_s_struct*)_tmpE7)->f1;_tmp104=((struct Cyc_Absyn_Decl_s_struct*)
_tmpE7)->f2;_LL147: Cyc_Absyndump_dumpdecl(_tmp103);Cyc_Absyndump_dumpstmt(
_tmp104);goto _LL12B;_LL148: if(_tmpE7 <= (void*)1?1:*((int*)_tmpE7)!= 15)goto
_LL14A;_tmp105=((struct Cyc_Absyn_Label_s_struct*)_tmpE7)->f1;_tmp106=((struct Cyc_Absyn_Label_s_struct*)
_tmpE7)->f2;_LL149: if(Cyc_Absynpp_is_declaration(_tmp106)){Cyc_Absyndump_dump_str(
_tmp105);Cyc_Absyndump_dump_nospace(_tag_arr(": {",sizeof(char),4));Cyc_Absyndump_dumpstmt(
_tmp106);Cyc_Absyndump_dump_char((int)'}');}else{Cyc_Absyndump_dump_str(_tmp105);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp106);}goto _LL12B;
_LL14A: if(_tmpE7 <= (void*)1?1:*((int*)_tmpE7)!= 16)goto _LL14C;_tmp107=((struct
Cyc_Absyn_Do_s_struct*)_tmpE7)->f1;_tmp108=((struct Cyc_Absyn_Do_s_struct*)_tmpE7)->f2;
_tmp109=_tmp108.f1;_LL14B: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("do{",sizeof(char),4));Cyc_Absyndump_dumpstmt(_tmp107);Cyc_Absyndump_dump_nospace(
_tag_arr("}while(",sizeof(char),8));Cyc_Absyndump_dumpexp(_tmp109);Cyc_Absyndump_dump_nospace(
_tag_arr(");",sizeof(char),3));goto _LL12B;_LL14C: if(_tmpE7 <= (void*)1?1:*((int*)
_tmpE7)!= 10)goto _LL14E;_tmp10A=((struct Cyc_Absyn_SwitchC_s_struct*)_tmpE7)->f1;
_tmp10B=((struct Cyc_Absyn_SwitchC_s_struct*)_tmpE7)->f2;_LL14D: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("switch \"C\"(",sizeof(char),12));Cyc_Absyndump_dumpexp(
_tmp10A);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(char),3));for(0;_tmp10B
!= 0;_tmp10B=_tmp10B->tl){struct Cyc_Absyn_SwitchC_clause _tmp11A;struct Cyc_Absyn_Exp*
_tmp11B;struct Cyc_Absyn_Stmt*_tmp11C;struct Cyc_Absyn_SwitchC_clause*_tmp119=(
struct Cyc_Absyn_SwitchC_clause*)_tmp10B->hd;_tmp11A=*_tmp119;_tmp11B=_tmp11A.cnst_exp;
_tmp11C=_tmp11A.body;if(_tmp11B == 0)Cyc_Absyndump_dump(_tag_arr("default:",
sizeof(char),9));else{Cyc_Absyndump_dump(_tag_arr("case",sizeof(char),5));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_check_null(_tmp11B));Cyc_Absyndump_dump_char((int)':');}
Cyc_Absyndump_dumpstmt(_tmp11C);}Cyc_Absyndump_dump_char((int)'}');goto _LL12B;
_LL14E: if(_tmpE7 <= (void*)1?1:*((int*)_tmpE7)!= 11)goto _LL150;_tmp10C=((struct
Cyc_Absyn_Fallthru_s_struct*)_tmpE7)->f1;if(_tmp10C != 0)goto _LL150;_LL14F: Cyc_Absyndump_dump(
_tag_arr("fallthru;",sizeof(char),10));goto _LL12B;_LL150: if(_tmpE7 <= (void*)1?1:*((
int*)_tmpE7)!= 11)goto _LL152;_tmp10D=((struct Cyc_Absyn_Fallthru_s_struct*)_tmpE7)->f1;
_LL151: Cyc_Absyndump_dump(_tag_arr("fallthru(",sizeof(char),10));Cyc_Absyndump_dumpexps_prec(
20,_tmp10D);Cyc_Absyndump_dump_nospace(_tag_arr(");",sizeof(char),3));goto _LL12B;
_LL152: if(_tmpE7 <= (void*)1?1:*((int*)_tmpE7)!= 17)goto _LL154;_tmp10E=((struct
Cyc_Absyn_TryCatch_s_struct*)_tmpE7)->f1;_tmp10F=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmpE7)->f2;_LL153: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("try",
sizeof(char),4));Cyc_Absyndump_dumpstmt(_tmp10E);Cyc_Absyndump_dump(_tag_arr("catch{",
sizeof(char),7));Cyc_Absyndump_dumpswitchclauses(_tmp10F);Cyc_Absyndump_dump_char((
int)'}');goto _LL12B;_LL154: if(_tmpE7 <= (void*)1?1:*((int*)_tmpE7)!= 18)goto
_LL156;_tmp110=((struct Cyc_Absyn_Region_s_struct*)_tmpE7)->f1;_tmp111=((struct
Cyc_Absyn_Region_s_struct*)_tmpE7)->f2;_tmp112=((struct Cyc_Absyn_Region_s_struct*)
_tmpE7)->f3;_tmp113=((struct Cyc_Absyn_Region_s_struct*)_tmpE7)->f4;_LL155: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("region",sizeof(char),7));if(_tmp112)Cyc_Absyndump_dump(
_tag_arr("[resetable]",sizeof(char),12));Cyc_Absyndump_dump(_tag_arr("<",sizeof(
char),2));Cyc_Absyndump_dumptvar(_tmp110);Cyc_Absyndump_dump(_tag_arr("> ",
sizeof(char),3));Cyc_Absyndump_dumpqvar(_tmp111->name);Cyc_Absyndump_dump(
_tag_arr("{",sizeof(char),2));Cyc_Absyndump_dumpstmt(_tmp113);Cyc_Absyndump_dump(
_tag_arr("}",sizeof(char),2));goto _LL12B;_LL156: if(_tmpE7 <= (void*)1?1:*((int*)
_tmpE7)!= 20)goto _LL158;_tmp114=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmpE7)->f1;
_LL157: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("reset_region(",
sizeof(char),14));Cyc_Absyndump_dumpexp(_tmp114);Cyc_Absyndump_dump(_tag_arr(");",
sizeof(char),3));goto _LL12B;_LL158: if(_tmpE7 <= (void*)1?1:*((int*)_tmpE7)!= 13)
goto _LL15A;_tmp115=((struct Cyc_Absyn_Cut_s_struct*)_tmpE7)->f1;_LL159: Cyc_Absyndump_dump(
_tag_arr("cut",sizeof(char),4));Cyc_Absyndump_dumpstmt(_tmp115);goto _LL12B;
_LL15A: if(_tmpE7 <= (void*)1?1:*((int*)_tmpE7)!= 14)goto _LL12B;_tmp116=((struct
Cyc_Absyn_Splice_s_struct*)_tmpE7)->f1;_LL15B: Cyc_Absyndump_dump(_tag_arr("splice",
sizeof(char),7));Cyc_Absyndump_dumpstmt(_tmp116);goto _LL12B;_LL12B:;}struct
_tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(
struct _tuple10*dp){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*dp).f1,
_tag_arr("",sizeof(char),1),_tag_arr("=",sizeof(char),2),_tag_arr("=",sizeof(
char),2));Cyc_Absyndump_dumppat((*dp).f2);}void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*
p){void*_tmp11D=(void*)p->r;void*_tmp11E;int _tmp11F;void*_tmp120;int _tmp121;char
_tmp122;struct _tagged_arr _tmp123;struct Cyc_Absyn_Vardecl*_tmp124;struct Cyc_List_List*
_tmp125;struct Cyc_Absyn_Pat*_tmp126;struct Cyc_Absyn_Vardecl*_tmp127;struct
_tuple0*_tmp128;struct _tuple0*_tmp129;struct Cyc_List_List*_tmp12A;struct Cyc_Absyn_AggrInfo
_tmp12B;void*_tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_List_List*_tmp12E;
struct Cyc_Absyn_Tunionfield*_tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_Absyn_Enumfield*
_tmp131;struct Cyc_Absyn_Enumfield*_tmp132;_LL16D: if((int)_tmp11D != 0)goto _LL16F;
_LL16E: Cyc_Absyndump_dump_char((int)'_');goto _LL16C;_LL16F: if((int)_tmp11D != 1)
goto _LL171;_LL170: Cyc_Absyndump_dump(_tag_arr("NULL",sizeof(char),5));goto _LL16C;
_LL171: if(_tmp11D <= (void*)2?1:*((int*)_tmp11D)!= 1)goto _LL173;_tmp11E=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp11D)->f1;if((int)_tmp11E != 0)goto _LL173;
_tmp11F=((struct Cyc_Absyn_Int_p_struct*)_tmp11D)->f2;_LL172: Cyc_Absyndump_dump((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp134;_tmp134.tag=1;_tmp134.f1=(
int)((unsigned int)_tmp11F);{void*_tmp133[1]={& _tmp134};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(char),3),_tag_arr(_tmp133,sizeof(void*),1));}}));goto _LL16C;_LL173: if(
_tmp11D <= (void*)2?1:*((int*)_tmp11D)!= 1)goto _LL175;_tmp120=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp11D)->f1;if((int)_tmp120 != 1)goto _LL175;_tmp121=((struct Cyc_Absyn_Int_p_struct*)
_tmp11D)->f2;_LL174: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp136;_tmp136.tag=1;_tmp136.f1=(unsigned int)_tmp121;{void*_tmp135[1]={&
_tmp136};Cyc_Std_aprintf(_tag_arr("%u",sizeof(char),3),_tag_arr(_tmp135,sizeof(
void*),1));}}));goto _LL16C;_LL175: if(_tmp11D <= (void*)2?1:*((int*)_tmp11D)!= 2)
goto _LL177;_tmp122=((struct Cyc_Absyn_Char_p_struct*)_tmp11D)->f1;_LL176: Cyc_Absyndump_dump(
_tag_arr("'",sizeof(char),2));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(
_tmp122));Cyc_Absyndump_dump_nospace(_tag_arr("'",sizeof(char),2));goto _LL16C;
_LL177: if(_tmp11D <= (void*)2?1:*((int*)_tmp11D)!= 3)goto _LL179;_tmp123=((struct
Cyc_Absyn_Float_p_struct*)_tmp11D)->f1;_LL178: Cyc_Absyndump_dump(_tmp123);goto
_LL16C;_LL179: if(_tmp11D <= (void*)2?1:*((int*)_tmp11D)!= 0)goto _LL17B;_tmp124=((
struct Cyc_Absyn_Var_p_struct*)_tmp11D)->f1;_LL17A: Cyc_Absyndump_dumpqvar(_tmp124->name);
goto _LL16C;_LL17B: if(_tmp11D <= (void*)2?1:*((int*)_tmp11D)!= 4)goto _LL17D;
_tmp125=((struct Cyc_Absyn_Tuple_p_struct*)_tmp11D)->f1;_LL17C:((void(*)(void(*f)(
struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
_tmp125,_tag_arr("$(",sizeof(char),3),_tag_arr(")",sizeof(char),2),_tag_arr(",",
sizeof(char),2));goto _LL16C;_LL17D: if(_tmp11D <= (void*)2?1:*((int*)_tmp11D)!= 5)
goto _LL17F;_tmp126=((struct Cyc_Absyn_Pointer_p_struct*)_tmp11D)->f1;_LL17E: Cyc_Absyndump_dump(
_tag_arr("&",sizeof(char),2));Cyc_Absyndump_dumppat(_tmp126);goto _LL16C;_LL17F:
if(_tmp11D <= (void*)2?1:*((int*)_tmp11D)!= 6)goto _LL181;_tmp127=((struct Cyc_Absyn_Reference_p_struct*)
_tmp11D)->f1;_LL180: Cyc_Absyndump_dump(_tag_arr("*",sizeof(char),2));Cyc_Absyndump_dumpqvar(
_tmp127->name);goto _LL16C;_LL181: if(_tmp11D <= (void*)2?1:*((int*)_tmp11D)!= 11)
goto _LL183;_tmp128=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp11D)->f1;_LL182: Cyc_Absyndump_dumpqvar(
_tmp128);goto _LL16C;_LL183: if(_tmp11D <= (void*)2?1:*((int*)_tmp11D)!= 12)goto
_LL185;_tmp129=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp11D)->f1;_tmp12A=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp11D)->f2;_LL184: Cyc_Absyndump_dumpqvar(
_tmp129);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp12A,_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(
char),2),_tag_arr(",",sizeof(char),2));goto _LL16C;_LL185: if(_tmp11D <= (void*)2?1:*((
int*)_tmp11D)!= 7)goto _LL187;_tmp12B=((struct Cyc_Absyn_Aggr_p_struct*)_tmp11D)->f1;
_tmp12C=(void*)_tmp12B.aggr_info;_tmp12D=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp11D)->f2;_tmp12E=((struct Cyc_Absyn_Aggr_p_struct*)_tmp11D)->f3;_LL186: {
struct _tuple0*_tmp138;struct _tuple3 _tmp137=Cyc_Absyn_aggr_kinded_name(_tmp12C);
_tmp138=_tmp137.f2;Cyc_Absyndump_dumpqvar(_tmp138);Cyc_Absyndump_dump_char((int)'{');((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _tagged_arr
start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp12D,_tag_arr("[",sizeof(char),2),_tag_arr("]",sizeof(char),2),_tag_arr(",",
sizeof(char),2));((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp,_tmp12E,_tag_arr("",sizeof(char),1),_tag_arr("}",sizeof(
char),2),_tag_arr(",",sizeof(char),2));goto _LL16C;}_LL187: if(_tmp11D <= (void*)2?
1:*((int*)_tmp11D)!= 8)goto _LL189;_tmp12F=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp11D)->f2;_tmp130=((struct Cyc_Absyn_Tunion_p_struct*)_tmp11D)->f3;_LL188: Cyc_Absyndump_dumpqvar(
_tmp12F->name);if(_tmp130 != 0)((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp130,_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(
char),2),_tag_arr(",",sizeof(char),2));goto _LL16C;_LL189: if(_tmp11D <= (void*)2?1:*((
int*)_tmp11D)!= 9)goto _LL18B;_tmp131=((struct Cyc_Absyn_Enum_p_struct*)_tmp11D)->f2;
_LL18A: _tmp132=_tmp131;goto _LL18C;_LL18B: if(_tmp11D <= (void*)2?1:*((int*)_tmp11D)
!= 10)goto _LL16C;_tmp132=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp11D)->f2;
_LL18C: Cyc_Absyndump_dumpqvar(_tmp132->name);goto _LL16C;_LL16C:;}void Cyc_Absyndump_dumptunionfield(
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
_tmp13A;struct _tagged_arr*_tmp13B;struct Cyc_Absyn_Tqual _tmp13C;void*_tmp13D;
struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_List_List*_tmp13F;struct Cyc_Absyn_Aggrfield*
_tmp139=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp13A=*_tmp139;_tmp13B=_tmp13A.name;
_tmp13C=_tmp13A.tq;_tmp13D=(void*)_tmp13A.type;_tmp13E=_tmp13A.width;_tmp13F=
_tmp13A.attributes;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct
_tagged_arr*),struct _tagged_arr*))Cyc_Absyndump_dumptqtd)(_tmp13C,_tmp13D,Cyc_Absyndump_dump_str,
_tmp13B);Cyc_Absyndump_dumpatts(_tmp13F);if(_tmp13E != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp13E));}Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(
td->name);Cyc_Absyndump_dumptvars(td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(
struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(fd->attributes);Cyc_Absyndump_dumpqvar(
fd->name);}static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple3*pr){{void*
_tmp140=(*pr).f1;_LL18E: if((int)_tmp140 != 8)goto _LL190;_LL18F: goto _LL18D;_LL190:
if((int)_tmp140 != 0)goto _LL192;_LL191: Cyc_Absyndump_dump(_tag_arr("_stdcall",
sizeof(char),9));goto _LL18D;_LL192: if((int)_tmp140 != 1)goto _LL194;_LL193: Cyc_Absyndump_dump(
_tag_arr("_cdecl",sizeof(char),7));goto _LL18D;_LL194: if((int)_tmp140 != 2)goto
_LL196;_LL195: Cyc_Absyndump_dump(_tag_arr("_fastcall",sizeof(char),10));goto
_LL18D;_LL196:;_LL197: goto _LL18D;_LL18D:;}Cyc_Absyndump_dumpqvar((*pr).f2);}
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dump_callconv(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}static void Cyc_Absyndump_dumpids(
struct Cyc_List_List*vds){for(0;vds != 0;vds=vds->tl){Cyc_Absyndump_dumpqvar(((
struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl != 0)Cyc_Absyndump_dump_char((
int)',');}}void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Vardecl _tmp142;void*_tmp143;struct _tuple0*_tmp144;struct Cyc_Absyn_Tqual
_tmp145;void*_tmp146;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_List_List*_tmp148;
struct Cyc_Absyn_Vardecl*_tmp141=vd;_tmp142=*_tmp141;_tmp143=(void*)_tmp142.sc;
_tmp144=_tmp142.name;_tmp145=_tmp142.tq;_tmp146=(void*)_tmp142.type;_tmp147=
_tmp142.initializer;_tmp148=_tmp142.attributes;Cyc_Absyndump_dumploc(loc);if(Cyc_Absyndump_to_VC){
Cyc_Absyndump_dumpatts(_tmp148);Cyc_Absyndump_dumpscope(_tmp143);{struct Cyc_Absyn_Tqual
_tmp14A;void*_tmp14B;struct Cyc_List_List*_tmp14C;struct _tuple5 _tmp149=Cyc_Absynpp_to_tms(
_tmp145,_tmp146);_tmp14A=_tmp149.f1;_tmp14B=_tmp149.f2;_tmp14C=_tmp149.f3;{void*
call_conv=(void*)8;{struct Cyc_List_List*tms2=_tmp14C;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp14D=(void*)tms2->hd;struct Cyc_List_List*_tmp14E;_LL199: if(_tmp14D <= (
void*)1?1:*((int*)_tmp14D)!= 4)goto _LL19B;_tmp14E=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmp14D)->f2;_LL19A: for(0;_tmp14E != 0;_tmp14E=_tmp14E->tl){void*_tmp14F=(void*)
_tmp14E->hd;_LL19E: if((int)_tmp14F != 0)goto _LL1A0;_LL19F: call_conv=(void*)0;goto
_LL19D;_LL1A0: if((int)_tmp14F != 1)goto _LL1A2;_LL1A1: call_conv=(void*)1;goto
_LL19D;_LL1A2: if((int)_tmp14F != 2)goto _LL1A4;_LL1A3: call_conv=(void*)2;goto
_LL19D;_LL1A4:;_LL1A5: goto _LL19D;_LL19D:;}goto _LL198;_LL19B:;_LL19C: goto _LL198;
_LL198:;}}Cyc_Absyndump_dumptq(_tmp14A);Cyc_Absyndump_dumpntyp(_tmp14B);{struct
_tuple3 _tmp150=({struct _tuple3 _tmp151;_tmp151.f1=call_conv;_tmp151.f2=_tmp144;
_tmp151;});((void(*)(struct Cyc_List_List*tms,void(*f)(struct _tuple3*),struct
_tuple3*a))Cyc_Absyndump_dumptms)(Cyc_List_imp_rev(_tmp14C),Cyc_Absyndump_dump_callconv_qvar,&
_tmp150);}}}}else{if(_tmp143 == (void*)3?Cyc_Absyndump_qvar_to_Cids: 0){void*
_tmp152=Cyc_Tcutil_compress(_tmp146);_LL1A7: if(_tmp152 <= (void*)3?1:*((int*)
_tmp152)!= 8)goto _LL1A9;_LL1A8: goto _LL1A6;_LL1A9:;_LL1AA: Cyc_Absyndump_dumpscope(
_tmp143);_LL1A6:;}else{Cyc_Absyndump_dumpscope(_tmp143);}((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp145,
_tmp146,Cyc_Absyndump_dumpqvar,_tmp144);Cyc_Absyndump_dumpatts(_tmp148);}if(
_tmp147 != 0){Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmp147));}Cyc_Absyndump_dump_semi();}void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*d){void*_tmp153=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp154;
struct Cyc_Absyn_Fndecl*_tmp155;struct Cyc_Absyn_Aggrdecl*_tmp156;struct Cyc_Absyn_Tuniondecl*
_tmp157;struct Cyc_Absyn_Tuniondecl _tmp158;void*_tmp159;struct _tuple0*_tmp15A;
struct Cyc_List_List*_tmp15B;struct Cyc_Core_Opt*_tmp15C;int _tmp15D;struct Cyc_Absyn_Enumdecl*
_tmp15E;struct Cyc_Absyn_Enumdecl _tmp15F;void*_tmp160;struct _tuple0*_tmp161;
struct Cyc_Core_Opt*_tmp162;struct Cyc_Absyn_Pat*_tmp163;struct Cyc_Absyn_Exp*
_tmp164;struct Cyc_List_List*_tmp165;struct Cyc_Absyn_Typedefdecl*_tmp166;struct
_tagged_arr*_tmp167;struct Cyc_List_List*_tmp168;struct _tuple0*_tmp169;struct Cyc_List_List*
_tmp16A;struct Cyc_List_List*_tmp16B;_LL1AC: if(*((int*)_tmp153)!= 0)goto _LL1AE;
_tmp154=((struct Cyc_Absyn_Var_d_struct*)_tmp153)->f1;_LL1AD: Cyc_Absyndump_dumpvardecl(
_tmp154,d->loc);goto _LL1AB;_LL1AE: if(*((int*)_tmp153)!= 1)goto _LL1B0;_tmp155=((
struct Cyc_Absyn_Fn_d_struct*)_tmp153)->f1;_LL1AF: Cyc_Absyndump_dumploc(d->loc);
if(Cyc_Absyndump_to_VC)Cyc_Absyndump_dumpatts(_tmp155->attributes);if(_tmp155->is_inline){
if(Cyc_Absyndump_to_VC)Cyc_Absyndump_dump(_tag_arr("__inline",sizeof(char),9));
else{Cyc_Absyndump_dump(_tag_arr("inline",sizeof(char),7));}}Cyc_Absyndump_dumpscope((
void*)_tmp155->sc);{void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp16D=
_cycalloc(sizeof(*_tmp16D));_tmp16D[0]=({struct Cyc_Absyn_FnType_struct _tmp16E;
_tmp16E.tag=8;_tmp16E.f1=({struct Cyc_Absyn_FnInfo _tmp16F;_tmp16F.tvars=_tmp155->tvs;
_tmp16F.effect=_tmp155->effect;_tmp16F.ret_typ=(void*)((void*)_tmp155->ret_type);
_tmp16F.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple4*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp155->args);_tmp16F.c_varargs=
_tmp155->c_varargs;_tmp16F.cyc_varargs=_tmp155->cyc_varargs;_tmp16F.rgn_po=
_tmp155->rgn_po;_tmp16F.attributes=0;_tmp16F;});_tmp16E;});_tmp16D;});((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))
Cyc_Absyndump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp16C;_tmp16C.q_const=0;_tmp16C.q_volatile=
0;_tmp16C.q_restrict=0;_tmp16C;}),t,Cyc_Absyndump_to_VC?Cyc_Absyndump_dump_callconv_fdqvar:
Cyc_Absyndump_dump_atts_qvar,_tmp155);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(
_tmp155->body);Cyc_Absyndump_dump_char((int)'}');goto _LL1AB;}_LL1B0: if(*((int*)
_tmp153)!= 4)goto _LL1B2;_tmp156=((struct Cyc_Absyn_Aggr_d_struct*)_tmp153)->f1;
_LL1B1: Cyc_Absyndump_dumpscope((void*)_tmp156->sc);Cyc_Absyndump_dumpaggr_kind((
void*)_tmp156->kind);Cyc_Absyndump_dumpqvar(_tmp156->name);Cyc_Absyndump_dumptvars(
_tmp156->tvs);if(_tmp156->impl == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp156->impl))->exist_vars
!= 0)((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp156->impl))->exist_vars,
_tag_arr("<",sizeof(char),2),_tag_arr(">",sizeof(char),2),_tag_arr(",",sizeof(
char),2));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp156->impl))->rgn_po
!= 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp156->impl))->rgn_po);}Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp156->impl))->fields);Cyc_Absyndump_dump(_tag_arr("}",sizeof(char),
2));Cyc_Absyndump_dumpatts(_tmp156->attributes);Cyc_Absyndump_dump(_tag_arr(";",
sizeof(char),2));}goto _LL1AB;_LL1B2: if(*((int*)_tmp153)!= 5)goto _LL1B4;_tmp157=((
struct Cyc_Absyn_Tunion_d_struct*)_tmp153)->f1;_tmp158=*_tmp157;_tmp159=(void*)
_tmp158.sc;_tmp15A=_tmp158.name;_tmp15B=_tmp158.tvs;_tmp15C=_tmp158.fields;
_tmp15D=_tmp158.is_xtunion;_LL1B3: Cyc_Absyndump_dumpscope(_tmp159);Cyc_Absyndump_dump(
_tmp15D?_tag_arr("xtunion",sizeof(char),8): _tag_arr("tunion",sizeof(char),7));
Cyc_Absyndump_dumpqvar(_tmp15A);Cyc_Absyndump_dumptvars(_tmp15B);if(_tmp15C == 0)
Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumptunionfields((
struct Cyc_List_List*)_tmp15C->v);Cyc_Absyndump_dump_nospace(_tag_arr("};",
sizeof(char),3));}goto _LL1AB;_LL1B4: if(*((int*)_tmp153)!= 6)goto _LL1B6;_tmp15E=((
struct Cyc_Absyn_Enum_d_struct*)_tmp153)->f1;_tmp15F=*_tmp15E;_tmp160=(void*)
_tmp15F.sc;_tmp161=_tmp15F.name;_tmp162=_tmp15F.fields;_LL1B5: Cyc_Absyndump_dumpscope(
_tmp160);Cyc_Absyndump_dump(_tag_arr("enum ",sizeof(char),6));Cyc_Absyndump_dumpqvar(
_tmp161);if(_tmp162 == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp162->v);Cyc_Absyndump_dump_nospace(
_tag_arr("};",sizeof(char),3));}return;_LL1B6: if(*((int*)_tmp153)!= 2)goto _LL1B8;
_tmp163=((struct Cyc_Absyn_Let_d_struct*)_tmp153)->f1;_tmp164=((struct Cyc_Absyn_Let_d_struct*)
_tmp153)->f3;_LL1B7: Cyc_Absyndump_dump(_tag_arr("let",sizeof(char),4));Cyc_Absyndump_dumppat(
_tmp163);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(_tmp164);Cyc_Absyndump_dump_semi();
goto _LL1AB;_LL1B8: if(*((int*)_tmp153)!= 3)goto _LL1BA;_tmp165=((struct Cyc_Absyn_Letv_d_struct*)
_tmp153)->f1;_LL1B9: Cyc_Absyndump_dump(_tag_arr("let ",sizeof(char),5));Cyc_Absyndump_dumpids(
_tmp165);Cyc_Absyndump_dump_semi();goto _LL1AB;_LL1BA: if(*((int*)_tmp153)!= 7)
goto _LL1BC;_tmp166=((struct Cyc_Absyn_Typedef_d_struct*)_tmp153)->f1;_LL1BB: if(!
Cyc_Absyndump_expand_typedefs?1:(_tmp166->defn != 0?Cyc_Absynpp_is_anon_aggrtype((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp166->defn))->v): 0)){Cyc_Absyndump_dump(
_tag_arr("typedef",sizeof(char),8));{void*t;if(_tmp166->defn == 0)t=Cyc_Absyn_new_evar(
_tmp166->kind,0);else{t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp166->defn))->v;}((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(({struct Cyc_Absyn_Tqual
_tmp170;_tmp170.q_const=0;_tmp170.q_volatile=0;_tmp170.q_restrict=0;_tmp170;}),t,
Cyc_Absyndump_dumptypedefname,_tmp166);Cyc_Absyndump_dump_semi();}}goto _LL1AB;
_LL1BC: if(*((int*)_tmp153)!= 8)goto _LL1BE;_tmp167=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp153)->f1;_tmp168=((struct Cyc_Absyn_Namespace_d_struct*)_tmp153)->f2;_LL1BD:
Cyc_Absyndump_dump(_tag_arr("namespace",sizeof(char),10));Cyc_Absyndump_dump_str(
_tmp167);Cyc_Absyndump_dump_char((int)'{');for(0;_tmp168 != 0;_tmp168=_tmp168->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp168->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1AB;_LL1BE: if(*((int*)_tmp153)!= 9)goto _LL1C0;_tmp169=((struct
Cyc_Absyn_Using_d_struct*)_tmp153)->f1;_tmp16A=((struct Cyc_Absyn_Using_d_struct*)
_tmp153)->f2;_LL1BF: Cyc_Absyndump_dump(_tag_arr("using",sizeof(char),6));Cyc_Absyndump_dumpqvar(
_tmp169);Cyc_Absyndump_dump_char((int)'{');for(0;_tmp16A != 0;_tmp16A=_tmp16A->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp16A->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1AB;_LL1C0: if(*((int*)_tmp153)!= 10)goto _LL1AB;_tmp16B=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp153)->f1;_LL1C1: Cyc_Absyndump_dump(_tag_arr("extern \"C\" {",
sizeof(char),13));for(0;_tmp16B != 0;_tmp16B=_tmp16B->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)_tmp16B->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1AB;
_LL1AB:;}static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
unsigned int i=Cyc_Evexp_eval_const_uint_exp(e);if(i != 1){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}void Cyc_Absyndump_dumptms(
struct Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){f(a);return;}{void*
_tmp171=(void*)tms->hd;void*_tmp172;struct Cyc_Absyn_Exp*_tmp173;void*_tmp174;
struct Cyc_Absyn_Tqual _tmp175;void*_tmp176;struct Cyc_Absyn_Exp*_tmp177;void*
_tmp178;struct Cyc_Absyn_Tqual _tmp179;void*_tmp17A;void*_tmp17B;struct Cyc_Absyn_Tqual
_tmp17C;void*_tmp17D;struct Cyc_Absyn_Exp*_tmp17E;void*_tmp17F;struct Cyc_Absyn_Tvar*
_tmp180;struct Cyc_Absyn_Tqual _tmp181;void*_tmp182;struct Cyc_Absyn_Exp*_tmp183;
void*_tmp184;struct Cyc_Absyn_Tvar*_tmp185;struct Cyc_Absyn_Tqual _tmp186;void*
_tmp187;void*_tmp188;struct Cyc_Absyn_Tvar*_tmp189;struct Cyc_Absyn_Tqual _tmp18A;
_LL1C3: if(_tmp171 <= (void*)1?1:*((int*)_tmp171)!= 1)goto _LL1C5;_tmp172=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp171)->f1;if(_tmp172 <= (void*)1?1:*((int*)
_tmp172)!= 1)goto _LL1C5;_tmp173=((struct Cyc_Absyn_Nullable_ps_struct*)_tmp172)->f1;
_tmp174=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp171)->f2;if((int)
_tmp174 != 2)goto _LL1C5;_tmp175=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp171)->f3;
_LL1C4: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dump_upperbound(_tmp173);
Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1C5: if(_tmp171 <= (void*)1?1:*((int*)
_tmp171)!= 1)goto _LL1C7;_tmp176=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp171)->f1;if(_tmp176 <= (void*)1?1:*((int*)_tmp176)!= 0)goto _LL1C7;_tmp177=((
struct Cyc_Absyn_NonNullable_ps_struct*)_tmp176)->f1;_tmp178=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp171)->f2;if((int)_tmp178 != 2)goto _LL1C7;_tmp179=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp171)->f3;_LL1C6: Cyc_Absyndump_dump_char((int)'@');Cyc_Absyndump_dump_upperbound(
_tmp177);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1C7: if(_tmp171 <= (void*)1?
1:*((int*)_tmp171)!= 1)goto _LL1C9;_tmp17A=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp171)->f1;if((int)_tmp17A != 0)goto _LL1C9;_tmp17B=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp171)->f2;if((int)_tmp17B != 2)goto _LL1C9;_tmp17C=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp171)->f3;_LL1C8: Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumptms(tms->tl,
f,a);return;_LL1C9: if(_tmp171 <= (void*)1?1:*((int*)_tmp171)!= 1)goto _LL1CB;
_tmp17D=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp171)->f1;if(_tmp17D <= (
void*)1?1:*((int*)_tmp17D)!= 1)goto _LL1CB;_tmp17E=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmp17D)->f1;_tmp17F=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp171)->f2;
if(_tmp17F <= (void*)3?1:*((int*)_tmp17F)!= 1)goto _LL1CB;_tmp180=((struct Cyc_Absyn_VarType_struct*)
_tmp17F)->f1;_tmp181=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp171)->f3;_LL1CA:
Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dump_upperbound(_tmp17E);Cyc_Absyndump_dump_str(
_tmp180->name);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1CB: if(_tmp171 <= (
void*)1?1:*((int*)_tmp171)!= 1)goto _LL1CD;_tmp182=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp171)->f1;if(_tmp182 <= (void*)1?1:*((int*)_tmp182)!= 0)goto _LL1CD;_tmp183=((
struct Cyc_Absyn_NonNullable_ps_struct*)_tmp182)->f1;_tmp184=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp171)->f2;if(_tmp184 <= (void*)3?1:*((int*)_tmp184)!= 1)goto _LL1CD;_tmp185=((
struct Cyc_Absyn_VarType_struct*)_tmp184)->f1;_tmp186=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp171)->f3;_LL1CC: Cyc_Absyndump_dump_char((int)'@');Cyc_Absyndump_dump_upperbound(
_tmp183);Cyc_Absyndump_dump_str(_tmp185->name);Cyc_Absyndump_dumptms(tms->tl,f,a);
return;_LL1CD: if(_tmp171 <= (void*)1?1:*((int*)_tmp171)!= 1)goto _LL1CF;_tmp187=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp171)->f1;if((int)_tmp187 != 0)
goto _LL1CF;_tmp188=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp171)->f2;if(
_tmp188 <= (void*)3?1:*((int*)_tmp188)!= 1)goto _LL1CF;_tmp189=((struct Cyc_Absyn_VarType_struct*)
_tmp188)->f1;_tmp18A=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp171)->f3;_LL1CE:
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dump_str(_tmp189->name);Cyc_Absyndump_dumptms(
tms->tl,f,a);return;_LL1CF: if(_tmp171 <= (void*)1?1:*((int*)_tmp171)!= 1)goto
_LL1D1;_LL1D0:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp18B=
_cycalloc(sizeof(*_tmp18B));_tmp18B[0]=({struct Cyc_Core_Impossible_struct _tmp18C;
_tmp18C.tag=Cyc_Core_Impossible;_tmp18C.f1=_tag_arr("dumptms: bad Pointer_mod",
sizeof(char),25);_tmp18C;});_tmp18B;}));_LL1D1:;_LL1D2: {int next_is_pointer=0;
if(tms->tl != 0){void*_tmp18D=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;
_LL1D4: if(_tmp18D <= (void*)1?1:*((int*)_tmp18D)!= 1)goto _LL1D6;_LL1D5:
next_is_pointer=1;goto _LL1D3;_LL1D6:;_LL1D7: goto _LL1D3;_LL1D3:;}if(
next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(tms->tl,
f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp18E=(void*)
tms->hd;struct Cyc_Absyn_Exp*_tmp18F;void*_tmp190;struct Cyc_List_List*_tmp191;int
_tmp192;struct Cyc_Absyn_VarargInfo*_tmp193;struct Cyc_Core_Opt*_tmp194;struct Cyc_List_List*
_tmp195;void*_tmp196;struct Cyc_List_List*_tmp197;struct Cyc_Position_Segment*
_tmp198;struct Cyc_List_List*_tmp199;struct Cyc_Position_Segment*_tmp19A;int
_tmp19B;struct Cyc_List_List*_tmp19C;void*_tmp19D;void*_tmp19E;_LL1D9: if((int)
_tmp18E != 0)goto _LL1DB;_LL1DA: Cyc_Absyndump_dump(_tag_arr("[]",sizeof(char),3));
goto _LL1D8;_LL1DB: if(_tmp18E <= (void*)1?1:*((int*)_tmp18E)!= 0)goto _LL1DD;
_tmp18F=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmp18E)->f1;_LL1DC: Cyc_Absyndump_dump_char((
int)'[');Cyc_Absyndump_dumpexp(_tmp18F);Cyc_Absyndump_dump_char((int)']');goto
_LL1D8;_LL1DD: if(_tmp18E <= (void*)1?1:*((int*)_tmp18E)!= 2)goto _LL1DF;_tmp190=(
void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp18E)->f1;if(*((int*)_tmp190)!= 
1)goto _LL1DF;_tmp191=((struct Cyc_Absyn_WithTypes_struct*)_tmp190)->f1;_tmp192=((
struct Cyc_Absyn_WithTypes_struct*)_tmp190)->f2;_tmp193=((struct Cyc_Absyn_WithTypes_struct*)
_tmp190)->f3;_tmp194=((struct Cyc_Absyn_WithTypes_struct*)_tmp190)->f4;_tmp195=((
struct Cyc_Absyn_WithTypes_struct*)_tmp190)->f5;_LL1DE: Cyc_Absyndump_dumpfunargs(
_tmp191,_tmp192,_tmp193,_tmp194,_tmp195);goto _LL1D8;_LL1DF: if(_tmp18E <= (void*)1?
1:*((int*)_tmp18E)!= 2)goto _LL1E1;_tmp196=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp18E)->f1;if(*((int*)_tmp196)!= 0)goto _LL1E1;_tmp197=((struct Cyc_Absyn_NoTypes_struct*)
_tmp196)->f1;_tmp198=((struct Cyc_Absyn_NoTypes_struct*)_tmp196)->f2;_LL1E0:((
void(*)(void(*f)(struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr
start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,
_tmp197,_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(char),2),_tag_arr(",",
sizeof(char),2));goto _LL1D8;_LL1E1: if(_tmp18E <= (void*)1?1:*((int*)_tmp18E)!= 3)
goto _LL1E3;_tmp199=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp18E)->f1;_tmp19A=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmp18E)->f2;_tmp19B=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp18E)->f3;_LL1E2: if(_tmp19B)Cyc_Absyndump_dumpkindedtvars(_tmp199);else{Cyc_Absyndump_dumptvars(
_tmp199);}goto _LL1D8;_LL1E3: if(_tmp18E <= (void*)1?1:*((int*)_tmp18E)!= 4)goto
_LL1E5;_tmp19C=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp18E)->f2;_LL1E4: Cyc_Absyndump_dumpatts(
_tmp19C);goto _LL1D8;_LL1E5: if(_tmp18E <= (void*)1?1:*((int*)_tmp18E)!= 1)goto
_LL1D8;_tmp19D=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp18E)->f1;_tmp19E=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp18E)->f2;_LL1E6:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp19F=_cycalloc(sizeof(*_tmp19F));
_tmp19F[0]=({struct Cyc_Core_Impossible_struct _tmp1A0;_tmp1A0.tag=Cyc_Core_Impossible;
_tmp1A0.f1=_tag_arr("dumptms",sizeof(char),8);_tmp1A0;});_tmp19F;}));_LL1D8:;}
return;}_LL1C2:;}}void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*
f)(void*),void*a){struct Cyc_Absyn_Tqual _tmp1A2;void*_tmp1A3;struct Cyc_List_List*
_tmp1A4;struct _tuple5 _tmp1A1=Cyc_Absynpp_to_tms(tq,t);_tmp1A2=_tmp1A1.f1;_tmp1A3=
_tmp1A1.f2;_tmp1A4=_tmp1A1.f3;Cyc_Absyndump_dumptq(_tmp1A2);Cyc_Absyndump_dumpntyp(
_tmp1A3);Cyc_Absyndump_dumptms(Cyc_List_imp_rev(_tmp1A4),f,a);}void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=
f;for(0;tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}({
void*_tmp1A5[0]={};Cyc_Std_fprintf(f,_tag_arr("\n",sizeof(char),2),_tag_arr(
_tmp1A5,sizeof(void*),0));});}
