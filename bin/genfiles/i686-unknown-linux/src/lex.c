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
  unsigned int total_bytes;
  unsigned int free_bytes;
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
extern int _throw(void * e);
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
  ({ void *_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void *_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (!_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)cks_ptr) + cks_elt_sz*cks_index; })
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
  _tag_arr_ans.base = _tag_arr_ans.curr = (void *)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void * _itarr = (arr); \
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
extern void * GC_malloc(int);
extern void * GC_malloc_atomic(int);
extern void * GC_calloc(unsigned int,unsigned int);
extern void * GC_calloc_atomic(unsigned int,unsigned int);

static inline void * _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc_atomic(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned int x, unsigned int y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
  unsigned int word_ans = (unsigned int)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void * _profile_GC_malloc(int,char *file,int lineno);
extern void * _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void * _profile_region_malloc(struct _RegionHandle *, unsigned int,
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
 struct Cyc_Core_Opt{void*v;};extern struct _tagged_arr Cyc_Core_new_string(
unsigned int);extern unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Cstdio___abstractFILE;struct
Cyc_Std___cycFILE;extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern
struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);struct
Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};extern unsigned char Cyc_List_List_mismatch[18];extern
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern unsigned char
Cyc_List_Nth[8];extern unsigned char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{void(*
refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};extern struct
_tagged_arr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);extern unsigned char Cyc_Lexing_lexeme_char(
struct Cyc_Lexing_lexbuf*,int);extern int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_Set_Set;
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));extern struct Cyc_Set_Set*
Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);extern int Cyc_Set_member(struct Cyc_Set_Set*
s,void*elt);extern void Cyc_Set_iter(void(*f)(void*),struct Cyc_Set_Set*s);extern
unsigned char Cyc_Set_Absent[11];extern unsigned int Cyc_Std_strlen(struct
_tagged_arr s);extern int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);
extern struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,struct _tagged_arr);
extern struct _tagged_arr Cyc_Std_zstrncpy(struct _tagged_arr,struct _tagged_arr,
unsigned int);extern struct _tagged_arr Cyc_Std_substring(struct _tagged_arr,int ofs,
unsigned int n);struct Cyc_Xarray_Xarray{struct _tagged_arr elmts;int num_elmts;};
extern void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);extern struct Cyc_Xarray_Xarray*
Cyc_Xarray_create(int,void*);extern void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,
void*);extern int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;extern struct Cyc_Position_Segment*
Cyc_Position_segment_of_abs(int,int);struct Cyc_Position_Error{struct _tagged_arr
source;struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
struct Cyc_Position_Error*Cyc_Position_mk_err_lex(struct Cyc_Position_Segment*,
struct _tagged_arr);extern struct Cyc_Position_Error*Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*,struct _tagged_arr);extern unsigned char Cyc_Position_Nocontext[
14];extern void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{
void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*
name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_PtrInfo{void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*
nullable;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Conref*bounds;};struct Cyc_Absyn_VarargInfo{
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Core_Opt*f3;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
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
int tag;void*f1;unsigned char f2;};struct Cyc_Absyn_Short_c_struct{int tag;void*f1;
short f2;};struct Cyc_Absyn_Int_c_struct{int tag;void*f1;int f2;};struct Cyc_Absyn_LongLong_c_struct{
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Aggrdecl*f3;};
struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};
struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
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
f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_ForArray_s_struct{
int tag;struct Cyc_Absyn_ForArrayInfo f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;
int f2;};struct Cyc_Absyn_Char_p_struct{int tag;unsigned char f1;};struct Cyc_Absyn_Float_p_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct
Cyc_Absyn_AggrInfo f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Tunion_p_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;
struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{void*r;
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
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
exist_vars;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};
struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{
int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int
tag;struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[
15];extern int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value;extern struct Cyc_Core_Opt*
Cyc_Parse_lbuf;struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
extern int Cyc_yyparse();extern unsigned char Cyc_AbstractDeclarator_tok[27];struct
Cyc_AbstractDeclarator_tok_struct{unsigned char*tag;struct Cyc_Abstractdeclarator*
f1;};extern unsigned char Cyc_AggrFieldDeclListList_tok[30];struct Cyc_AggrFieldDeclListList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_AggrFieldDeclList_tok[
26];struct Cyc_AggrFieldDeclList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_AttributeList_tok[22];struct Cyc_AttributeList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Attribute_tok[
18];struct Cyc_Attribute_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_Bool_tok[13];struct Cyc_Bool_tok_struct{unsigned char*tag;int f1;
};extern unsigned char Cyc_Char_tok[13];struct Cyc_Char_tok_struct{unsigned char*
tag;unsigned char f1;};extern unsigned char Cyc_DeclList_tok[17];struct Cyc_DeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_DeclSpec_tok[
17];struct Cyc_DeclSpec_tok_struct{unsigned char*tag;struct Cyc_Declaration_spec*
f1;};extern unsigned char Cyc_Declarator_tok[19];struct Cyc_Declarator_tok_struct{
unsigned char*tag;struct Cyc_Declarator*f1;};extern unsigned char Cyc_DesignatorList_tok[
23];struct Cyc_DesignatorList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;
};extern unsigned char Cyc_Designator_tok[19];struct Cyc_Designator_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_EnumfieldList_tok[22];struct
Cyc_EnumfieldList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_Enumfield_tok[18];struct Cyc_Enumfield_tok_struct{unsigned char*
tag;struct Cyc_Absyn_Enumfield*f1;};extern unsigned char Cyc_ExpList_tok[16];struct
Cyc_ExpList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_Exp_tok[12];struct Cyc_Exp_tok_struct{unsigned char*tag;struct
Cyc_Absyn_Exp*f1;};extern unsigned char Cyc_FieldPatternList_tok[25];struct Cyc_FieldPatternList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_FieldPattern_tok[
21];struct _tuple3{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
unsigned char*tag;struct _tuple3*f1;};extern unsigned char Cyc_FnDecl_tok[15];
struct Cyc_FnDecl_tok_struct{unsigned char*tag;struct Cyc_Absyn_Fndecl*f1;};extern
unsigned char Cyc_IdList_tok[15];struct Cyc_IdList_tok_struct{unsigned char*tag;
struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDeclList_tok[21];struct Cyc_InitDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDecl_tok[
17];struct _tuple4{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
unsigned char*tag;struct _tuple4*f1;};extern unsigned char Cyc_InitializerList_tok[
24];struct Cyc_InitializerList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_Int_tok[12];struct _tuple5{void*f1;int f2;};struct Cyc_Int_tok_struct{
unsigned char*tag;struct _tuple5*f1;};extern unsigned char Cyc_Kind_tok[13];struct
Cyc_Kind_tok_struct{unsigned char*tag;void*f1;};extern unsigned char Cyc_Okay_tok[
13];extern unsigned char Cyc_ParamDeclListBool_tok[26];struct _tuple6{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct Cyc_ParamDeclListBool_tok_struct{unsigned char*tag;struct _tuple6*f1;}
;extern unsigned char Cyc_ParamDeclList_tok[22];struct Cyc_ParamDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_ParamDecl_tok[
18];struct Cyc_ParamDecl_tok_struct{unsigned char*tag;struct _tuple1*f1;};extern
unsigned char Cyc_PatternList_tok[20];struct Cyc_PatternList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Pattern_tok[16];
struct Cyc_Pattern_tok_struct{unsigned char*tag;struct Cyc_Absyn_Pat*f1;};extern
unsigned char Cyc_Pointer_Sort_tok[21];struct Cyc_Pointer_Sort_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Primop_tok[15];struct Cyc_Primop_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Primopopt_tok[18];struct Cyc_Primopopt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_QualId_tok[15];
struct Cyc_QualId_tok_struct{unsigned char*tag;struct _tuple0*f1;};extern
unsigned char Cyc_QualSpecList_tok[21];struct _tuple7{struct Cyc_Absyn_Tqual f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_QualSpecList_tok_struct{
unsigned char*tag;struct _tuple7*f1;};extern unsigned char Cyc_Rgnorder_tok[17];
struct Cyc_Rgnorder_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_Scope_tok[14];struct Cyc_Scope_tok_struct{unsigned char*tag;void*
f1;};extern unsigned char Cyc_Short_tok[14];struct Cyc_Short_tok_struct{
unsigned char*tag;short f1;};extern unsigned char Cyc_Stmt_tok[13];struct Cyc_Stmt_tok_struct{
unsigned char*tag;struct Cyc_Absyn_Stmt*f1;};extern unsigned char Cyc_StorageClass_tok[
21];struct Cyc_StorageClass_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_String_tok[15];struct Cyc_String_tok_struct{unsigned char*tag;
struct _tagged_arr f1;};extern unsigned char Cyc_Stringopt_tok[18];struct Cyc_Stringopt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_StructOrUnion_tok[
22];struct Cyc_StructOrUnion_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_SwitchCClauseList_tok[26];struct Cyc_SwitchCClauseList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_SwitchClauseList_tok[
25];struct Cyc_SwitchClauseList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_TunionFieldList_tok[24];struct Cyc_TunionFieldList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_TunionField_tok[
20];struct Cyc_TunionField_tok_struct{unsigned char*tag;struct Cyc_Absyn_Tunionfield*
f1;};extern unsigned char Cyc_TypeList_tok[17];struct Cyc_TypeList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_TypeModifierList_tok[
25];struct Cyc_TypeModifierList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_TypeOpt_tok[16];struct Cyc_TypeOpt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_TypeQual_tok[17];
struct Cyc_TypeQual_tok_struct{unsigned char*tag;struct Cyc_Absyn_Tqual f1;};extern
unsigned char Cyc_TypeSpecifier_tok[22];struct Cyc_TypeSpecifier_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Type_tok[13];struct Cyc_Type_tok_struct{
unsigned char*tag;void*f1;};extern int Cyc_yyparse();extern unsigned char Cyc_YY1[8];
struct Cyc_YY1_struct{unsigned char*tag;struct _tuple6*f1;};extern unsigned char Cyc_YYINITIALSVAL[
18];struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;
int last_column;};extern struct Cyc_Yyltype Cyc_yylloc;extern void*Cyc_yylval;struct
Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[
11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));extern
struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);extern
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);void Cyc_yyerror(struct
_tagged_arr s){Cyc_Position_post_error(Cyc_Position_mk_err_parse(Cyc_Position_segment_of_abs(
Cyc_yylloc.first_line,Cyc_yylloc.last_line),s));}struct Cyc_Lex_Trie;struct Cyc_Lex_One_struct{
int tag;int f1;struct Cyc_Lex_Trie*f2;};struct Cyc_Lex_Many_struct{int tag;struct Cyc_Lex_Trie**
f1;};struct Cyc_Lex_Trie{void*children;int shared_str;};static int Cyc_Lex_num_kws=0;
static struct _tagged_arr Cyc_Lex_kw_nums={(void*)0,(void*)0,(void*)(0 + 0)};static
struct Cyc_Xarray_Xarray*Cyc_Lex_symbols=0;static struct Cyc_Lex_Trie*Cyc_Lex_ids_trie=
0;static struct Cyc_Lex_Trie*Cyc_Lex_typedefs_trie=0;static int Cyc_Lex_comment_depth=
0;static struct _tuple5 Cyc_Lex_token_int_pair={(void*)0,0};static unsigned char
_tmp0[8]="*bogus*";static struct _tagged_arr Cyc_Lex_bogus_string={_tmp0,_tmp0,
_tmp0 + 8};static struct _tuple0 Cyc_Lex_token_id_pair={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Lex_bogus_string};static unsigned char Cyc_Lex_token_char='\000';static
unsigned char _tmp1[1]="";static struct _tagged_arr Cyc_Lex_token_string={_tmp1,
_tmp1,_tmp1 + 1};static struct _tuple5*Cyc_Lex_token_int=& Cyc_Lex_token_int_pair;
static struct _tuple0*Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair;static int Cyc_Lex_runaway_start=
0;static void Cyc_Lex_err(struct _tagged_arr msg,struct Cyc_Lexing_lexbuf*lb){struct
Cyc_Position_Segment*s=Cyc_Position_segment_of_abs(Cyc_Lexing_lexeme_start(lb),
Cyc_Lexing_lexeme_end(lb));Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,msg));}
static void Cyc_Lex_runaway_err(struct _tagged_arr msg,struct Cyc_Lexing_lexbuf*lb){
struct Cyc_Position_Segment*s=Cyc_Position_segment_of_abs(Cyc_Lex_runaway_start,
Cyc_Lexing_lexeme_start(lb));Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,
msg));}struct _tuple8{struct _tagged_arr f1;short f2;};static unsigned char _tmp2[14]="__attribute__";
static unsigned char _tmp3[9]="abstract";static unsigned char _tmp4[5]="auto";static
unsigned char _tmp5[6]="break";static unsigned char _tmp6[7]="calloc";static
unsigned char _tmp7[5]="case";static unsigned char _tmp8[6]="catch";static
unsigned char _tmp9[5]="char";static unsigned char _tmpA[8]="codegen";static
unsigned char _tmpB[6]="const";static unsigned char _tmpC[9]="continue";static
unsigned char _tmpD[4]="cut";static unsigned char _tmpE[8]="default";static
unsigned char _tmpF[3]="do";static unsigned char _tmp10[7]="double";static
unsigned char _tmp11[5]="else";static unsigned char _tmp12[5]="enum";static
unsigned char _tmp13[7]="extern";static unsigned char _tmp14[9]="fallthru";static
unsigned char _tmp15[5]="fill";static unsigned char _tmp16[6]="float";static
unsigned char _tmp17[4]="for";static unsigned char _tmp18[9]="forarray";static
unsigned char _tmp19[6]="__gen";static unsigned char _tmp1A[5]="goto";static
unsigned char _tmp1B[3]="if";static unsigned char _tmp1C[7]="inline";static
unsigned char _tmp1D[4]="int";static unsigned char _tmp1E[4]="let";static
unsigned char _tmp1F[5]="long";static unsigned char _tmp20[7]="malloc";static
unsigned char _tmp21[10]="namespace";static unsigned char _tmp22[4]="new";static
unsigned char _tmp23[5]="NULL";static unsigned char _tmp24[9]="offsetof";static
unsigned char _tmp25[8]="rcalloc";static unsigned char _tmp26[9]="region_t";static
unsigned char _tmp27[7]="region";static unsigned char _tmp28[8]="regions";static
unsigned char _tmp29[9]="register";static unsigned char _tmp2A[9]="restrict";static
unsigned char _tmp2B[7]="return";static unsigned char _tmp2C[8]="rmalloc";static
unsigned char _tmp2D[5]="rnew";static unsigned char _tmp2E[6]="short";static
unsigned char _tmp2F[7]="signed";static unsigned char _tmp30[7]="sizeof";static
unsigned char _tmp31[9]="sizeof_t";static unsigned char _tmp32[7]="splice";static
unsigned char _tmp33[7]="static";static unsigned char _tmp34[7]="struct";static
unsigned char _tmp35[7]="switch";static unsigned char _tmp36[6]="throw";static
unsigned char _tmp37[4]="try";static unsigned char _tmp38[7]="tunion";static
unsigned char _tmp39[8]="typedef";static unsigned char _tmp3A[6]="union";static
unsigned char _tmp3B[9]="unsigned";static unsigned char _tmp3C[6]="using";static
unsigned char _tmp3D[5]="void";static unsigned char _tmp3E[9]="volatile";static
unsigned char _tmp3F[6]="while";static unsigned char _tmp40[8]="xtunion";static
struct _tuple8 Cyc_Lex_rw_array[63]={{{_tmp2,_tmp2,_tmp2 + 14},353},{{_tmp3,_tmp3,
_tmp3 + 9},299},{{_tmp4,_tmp4,_tmp4 + 5},258},{{_tmp5,_tmp5,_tmp5 + 6},290},{{_tmp6,
_tmp6,_tmp6 + 7},312},{{_tmp7,_tmp7,_tmp7 + 5},277},{{_tmp8,_tmp8,_tmp8 + 6},297},{{
_tmp9,_tmp9,_tmp9 + 5},264},{{_tmpA,_tmpA,_tmpA + 8},307},{{_tmpB,_tmpB,_tmpB + 6},
272},{{_tmpC,_tmpC,_tmpC + 9},289},{{_tmpD,_tmpD,_tmpD + 4},308},{{_tmpE,_tmpE,
_tmpE + 8},278},{{_tmpF,_tmpF,_tmpF + 3},286},{{_tmp10,_tmp10,_tmp10 + 7},269},{{
_tmp11,_tmp11,_tmp11 + 5},283},{{_tmp12,_tmp12,_tmp12 + 5},292},{{_tmp13,_tmp13,
_tmp13 + 7},261},{{_tmp14,_tmp14,_tmp14 + 9},300},{{_tmp15,_tmp15,_tmp15 + 5},306},{{
_tmp16,_tmp16,_tmp16 + 6},268},{{_tmp17,_tmp17,_tmp17 + 4},287},{{_tmp18,_tmp18,
_tmp18 + 9},305},{{_tmp19,_tmp19,_tmp19 + 6},319},{{_tmp1A,_tmp1A,_tmp1A + 5},288},{{
_tmp1B,_tmp1B,_tmp1B + 3},282},{{_tmp1C,_tmp1C,_tmp1C + 7},279},{{_tmp1D,_tmp1D,
_tmp1D + 4},266},{{_tmp1E,_tmp1E,_tmp1E + 4},294},{{_tmp1F,_tmp1F,_tmp1F + 5},267},{{
_tmp20,_tmp20,_tmp20 + 7},310},{{_tmp21,_tmp21,_tmp21 + 10},302},{{_tmp22,_tmp22,
_tmp22 + 4},298},{{_tmp23,_tmp23,_tmp23 + 5},293},{{_tmp24,_tmp24,_tmp24 + 9},281},{{
_tmp25,_tmp25,_tmp25 + 8},313},{{_tmp26,_tmp26,_tmp26 + 9},314},{{_tmp27,_tmp27,
_tmp27 + 7},316},{{_tmp28,_tmp28,_tmp28 + 8},318},{{_tmp29,_tmp29,_tmp29 + 9},259},{{
_tmp2A,_tmp2A,_tmp2A + 9},274},{{_tmp2B,_tmp2B,_tmp2B + 7},291},{{_tmp2C,_tmp2C,
_tmp2C + 8},311},{{_tmp2D,_tmp2D,_tmp2D + 5},317},{{_tmp2E,_tmp2E,_tmp2E + 6},265},{{
_tmp2F,_tmp2F,_tmp2F + 7},270},{{_tmp30,_tmp30,_tmp30 + 7},280},{{_tmp31,_tmp31,
_tmp31 + 9},315},{{_tmp32,_tmp32,_tmp32 + 7},309},{{_tmp33,_tmp33,_tmp33 + 7},260},{{
_tmp34,_tmp34,_tmp34 + 7},275},{{_tmp35,_tmp35,_tmp35 + 7},284},{{_tmp36,_tmp36,
_tmp36 + 6},295},{{_tmp37,_tmp37,_tmp37 + 4},296},{{_tmp38,_tmp38,_tmp38 + 7},303},{{
_tmp39,_tmp39,_tmp39 + 8},262},{{_tmp3A,_tmp3A,_tmp3A + 6},276},{{_tmp3B,_tmp3B,
_tmp3B + 9},271},{{_tmp3C,_tmp3C,_tmp3C + 6},301},{{_tmp3D,_tmp3D,_tmp3D + 5},263},{{
_tmp3E,_tmp3E,_tmp3E + 9},273},{{_tmp3F,_tmp3F,_tmp3F + 6},285},{{_tmp40,_tmp40,
_tmp40 + 8},304}};static int Cyc_Lex_trie_char(int c){if(c >= 95){return c - 59;}else{
if(c > 64){return c - 55;}}return c - 48;}static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(
struct Cyc_Lex_Trie*t,struct _tagged_arr buff,int offset,int len){int i=offset;int last=(
offset + len)- 1;while(i <= last){void*_tmp41=(void*)((struct Cyc_Lex_Trie*)
_check_null(t))->children;struct Cyc_Lex_Trie**_tmp42;struct Cyc_Lex_Trie*_tmp43;
int _tmp44;_LL1: if((unsigned int)_tmp41 > 1?*((int*)_tmp41)== 1: 0){_LL7: _tmp42=((
struct Cyc_Lex_Many_struct*)_tmp41)->f1;goto _LL2;}else{goto _LL3;}_LL3: if((
unsigned int)_tmp41 > 1?*((int*)_tmp41)== 0: 0){_LL9: _tmp44=((struct Cyc_Lex_One_struct*)
_tmp41)->f1;goto _LL8;_LL8: _tmp43=((struct Cyc_Lex_One_struct*)_tmp41)->f2;goto
_LL4;}else{goto _LL5;}_LL5: if((int)_tmp41 == 0){goto _LL6;}else{goto _LL0;}_LL2: {
int ch=Cyc_Lex_trie_char((int)((const unsigned char*)buff.curr)[i]);if(_tmp42[ch]
== 0){_tmp42[ch]=({struct Cyc_Lex_Trie*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->children=(
void*)((void*)0);_tmp45->shared_str=0;_tmp45;});}t=_tmp42[ch];++ i;goto _LL0;}_LL4:
if(_tmp44 == ((const unsigned char*)buff.curr)[i]){t=_tmp43;}else{struct Cyc_Lex_Trie**
_tmp46=({unsigned int _tmp4B=64;struct Cyc_Lex_Trie**_tmp4C=(struct Cyc_Lex_Trie**)
_cycalloc(_check_times(sizeof(struct Cyc_Lex_Trie*),_tmp4B));{unsigned int _tmp4D=
_tmp4B;unsigned int j;for(j=0;j < _tmp4D;j ++){_tmp4C[j]=0;}};_tmp4C;});_tmp46[
_check_known_subscript_notnull(64,Cyc_Lex_trie_char(_tmp44))]=_tmp43;{int _tmp47=
Cyc_Lex_trie_char((int)((const unsigned char*)buff.curr)[i]);_tmp46[_tmp47]=({
struct Cyc_Lex_Trie*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->children=(void*)((
void*)0);_tmp48->shared_str=0;_tmp48;});(void*)(t->children=(void*)((void*)({
struct Cyc_Lex_Many_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49[0]=({struct
Cyc_Lex_Many_struct _tmp4A;_tmp4A.tag=1;_tmp4A.f1=_tmp46;_tmp4A;});_tmp49;})));t=
_tmp46[_tmp47];}}++ i;goto _LL0;_LL6: while(i <= last){struct Cyc_Lex_Trie*_tmp4E=({
struct Cyc_Lex_Trie*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->children=(void*)((
void*)0);_tmp51->shared_str=0;_tmp51;});(void*)(((struct Cyc_Lex_Trie*)
_check_null(t))->children=(void*)((void*)({struct Cyc_Lex_One_struct*_tmp4F=
_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Lex_One_struct _tmp50;_tmp50.tag=
0;_tmp50.f1=(int)*((const unsigned char*)_check_unknown_subscript(buff,sizeof(
unsigned char),i ++));_tmp50.f2=_tmp4E;_tmp50;});_tmp4F;})));t=_tmp4E;}return t;
_LL0:;}return t;}static int Cyc_Lex_str_index(struct _tagged_arr buff,int offset,int
len){struct Cyc_Lex_Trie*_tmp52=Cyc_Lex_trie_lookup(Cyc_Lex_ids_trie,buff,offset,
len);if(_tmp52->shared_str == 0){struct _tagged_arr _tmp53=Cyc_Core_new_string((
unsigned int)(len + 1));Cyc_Std_zstrncpy(_tmp53,_tagged_arr_plus(buff,sizeof(
unsigned char),offset),(unsigned int)len);{int ans=((int(*)(struct Cyc_Xarray_Xarray*,
struct _tagged_arr*))Cyc_Xarray_add_ind)((struct Cyc_Xarray_Xarray*)_check_null(
Cyc_Lex_symbols),({struct _tagged_arr*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=(
struct _tagged_arr)_tmp53;_tmp54;}));_tmp52->shared_str=ans;}}return _tmp52->shared_str;}
static int Cyc_Lex_str_index_lbuf(struct Cyc_Lexing_lexbuf*lbuf){return Cyc_Lex_str_index((
struct _tagged_arr)lbuf->lex_buffer,lbuf->lex_start_pos,lbuf->lex_curr_pos - lbuf->lex_start_pos);}
static void Cyc_Lex_insert_typedef(struct _tagged_arr*sptr){struct _tagged_arr _tmp55=*
sptr;struct Cyc_Lex_Trie*_tmp56=Cyc_Lex_trie_lookup(Cyc_Lex_typedefs_trie,_tmp55,
0,(int)(_get_arr_size(_tmp55,sizeof(unsigned char))- 1));((struct Cyc_Lex_Trie*)
_check_null(_tmp56))->shared_str=1;return;}static struct _tagged_arr*Cyc_Lex_get_symbol(
int symbol_num){return((struct _tagged_arr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)((
struct Cyc_Xarray_Xarray*)_check_null(Cyc_Lex_symbols),symbol_num);}static int Cyc_Lex_int_of_char(
unsigned char c){if('0' <= c? c <= '9': 0){return c - '0';}else{if('a' <= c? c <= 'f': 0){
return(10 + c)- 'a';}else{if('A' <= c? c <= 'F': 0){return(10 + c)- 'A';}else{(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp57=_cycalloc(sizeof(*
_tmp57));_tmp57[0]=({struct Cyc_Core_Invalid_argument_struct _tmp58;_tmp58.tag=Cyc_Core_Invalid_argument;
_tmp58.f1=_tag_arr("string to integer conversion",sizeof(unsigned char),29);
_tmp58;});_tmp57;}));}}}}static struct _tuple5*Cyc_Lex_intconst(struct Cyc_Lexing_lexbuf*
lbuf,int start,int end,int base){unsigned int n=0;int end2=lbuf->lex_curr_pos - end;
struct _tagged_arr buff=lbuf->lex_buffer;int i=start + lbuf->lex_start_pos;{int i=
start + lbuf->lex_start_pos;for(0;i < end2;++ i){unsigned char c=((unsigned char*)
buff.curr)[i];switch(c){case 'u': _LLA: goto _LLB;case 'U': _LLB: return({struct _tuple5*
_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->f1=(void*)1;_tmp59->f2=(int)n;_tmp59;});
case 'l': _LLC: break;case 'L': _LLD: break;default: _LLE: n=n * base + (unsigned int)Cyc_Lex_int_of_char(
c);break;}}}return({struct _tuple5*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->f1=(
void*)0;_tmp5A->f2=(int)n;_tmp5A;});}unsigned char Cyc_Lex_string_buffer_v[10]={'x','x','x','x','x','x','x','x','x','x'};
struct _tagged_arr Cyc_Lex_string_buffer={(void*)Cyc_Lex_string_buffer_v,(void*)
Cyc_Lex_string_buffer_v,(void*)(Cyc_Lex_string_buffer_v + 10)};int Cyc_Lex_string_pos=
0;void Cyc_Lex_store_string_char(unsigned char c){int sz=(int)_get_arr_size(Cyc_Lex_string_buffer,
sizeof(unsigned char));if(Cyc_Lex_string_pos >= sz){int newsz=sz;while(Cyc_Lex_string_pos
>= newsz){newsz=newsz * 2;}{struct _tagged_arr str=({unsigned int _tmp5B=(
unsigned int)newsz;unsigned char*_tmp5C=(unsigned char*)_cycalloc_atomic(
_check_times(sizeof(unsigned char),_tmp5B));struct _tagged_arr _tmp5E=_tag_arr(
_tmp5C,sizeof(unsigned char),(unsigned int)newsz);{unsigned int _tmp5D=_tmp5B;
unsigned int i;for(i=0;i < _tmp5D;i ++){_tmp5C[i]=i < sz?*((unsigned char*)
_check_unknown_subscript(Cyc_Lex_string_buffer,sizeof(unsigned char),(int)i)):'\000';}};
_tmp5E;});Cyc_Lex_string_buffer=str;}}*((unsigned char*)_check_unknown_subscript(
Cyc_Lex_string_buffer,sizeof(unsigned char),Cyc_Lex_string_pos))=c;++ Cyc_Lex_string_pos;}
struct _tagged_arr Cyc_Lex_get_stored_string(){struct _tagged_arr str=Cyc_Std_substring((
struct _tagged_arr)Cyc_Lex_string_buffer,0,(unsigned int)Cyc_Lex_string_pos);Cyc_Lex_string_pos=
0;return str;}struct Cyc_Lex_Ldecls{struct Cyc_Set_Set*typedefs;struct Cyc_Set_Set*
namespaces;};struct Cyc_Lex_Lvis{struct Cyc_List_List*current_namespace;struct Cyc_List_List*
imported_namespaces;};struct Cyc_Lex_Lstate{struct Cyc_List_List*lstack;struct Cyc_Dict_Dict*
decls;};static struct Cyc_Core_Opt*Cyc_Lex_lstate=0;static void Cyc_Lex_typedef_init(){
struct Cyc_Lex_Lvis*_tmp5F=({struct Cyc_Lex_Lvis*_tmp65=_cycalloc(sizeof(*_tmp65));
_tmp65->current_namespace=0;_tmp65->imported_namespaces=0;_tmp65;});struct Cyc_List_List*
_tmp60=({struct Cyc_List_List*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->hd=_tmp5F;
_tmp64->tl=0;_tmp64;});struct Cyc_Dict_Dict*init_decls=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))
Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),0,({struct Cyc_Lex_Ldecls*_tmp63=_cycalloc(
sizeof(*_tmp63));_tmp63->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_zstrptrcmp);_tmp63->namespaces=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_Std_zstrptrcmp);_tmp63;}));Cyc_Lex_lstate=({struct Cyc_Core_Opt*_tmp61=
_cycalloc(sizeof(*_tmp61));_tmp61->v=({struct Cyc_Lex_Lstate*_tmp62=_cycalloc(
sizeof(*_tmp62));_tmp62->lstack=_tmp60;_tmp62->decls=init_decls;_tmp62;});_tmp61;});}
static struct Cyc_List_List*Cyc_Lex_get_absolute_namespace(struct Cyc_List_List*ns){
struct _tagged_arr*n=(struct _tagged_arr*)ns->hd;{struct Cyc_List_List*ls=(struct
Cyc_List_List*)((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;
for(0;ls != 0;ls=ls->tl){struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;struct
Cyc_List_List*x=({struct Cyc_List_List*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->hd=
lv->current_namespace;_tmp66->tl=lv->imported_namespaces;_tmp66;});for(0;x != 0;x=((
struct Cyc_List_List*)_check_null(x))->tl){struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)((
struct Cyc_List_List*)_check_null(x))->hd);if(((int(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_member)(ld->namespaces,n)){return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((struct Cyc_List_List*)((
struct Cyc_List_List*)_check_null(x))->hd,(struct Cyc_List_List*)ns);}}}}Cyc_yyerror((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp68;_tmp68.tag=0;_tmp68.f1=(
struct _tagged_arr)Cyc_Std_str_sepstr((struct Cyc_List_List*)ns,_tag_arr("::",
sizeof(unsigned char),3));{void*_tmp67[1]={& _tmp68};Cyc_Std_aprintf(_tag_arr("undeclared namespace %s",
sizeof(unsigned char),24),_tag_arr(_tmp67,sizeof(void*),1));}}));return 0;}static
void Cyc_Lex_recompute_typedefs(){Cyc_Lex_typedefs_trie=({struct Cyc_Lex_Trie*
_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->children=(void*)((void*)0);_tmp69->shared_str=
0;_tmp69;});{struct Cyc_List_List*ls=(struct Cyc_List_List*)((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;for(0;ls != 0;ls=ls->tl){
struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;struct Cyc_List_List*x=({struct
Cyc_List_List*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->hd=lv->current_namespace;
_tmp6A->tl=lv->imported_namespaces;_tmp6A;});for(0;x != 0;x=((struct Cyc_List_List*)
_check_null(x))->tl){struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct
Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)((
struct Cyc_List_List*)_check_null(x))->hd);((void(*)(void(*f)(struct _tagged_arr*),
struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,ld->typedefs);}}}}
static int Cyc_Lex_is_typedef_in_namespace(struct Cyc_List_List*ns,struct
_tagged_arr*v){struct Cyc_List_List*ans=Cyc_Lex_get_absolute_namespace(ns);struct
_handler_cons _tmp6B;_push_handler(& _tmp6B);{int _tmp6D=0;if(setjmp(_tmp6B.handler)){
_tmp6D=1;}if(! _tmp6D){{struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct
Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ans);int _tmp6E=((int(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(ld->typedefs,v);
_npop_handler(0);return _tmp6E;};_pop_handler();}else{void*_tmp6C=(void*)
_exn_thrown;void*_tmp70=_tmp6C;_LL11: if(_tmp70 == Cyc_Dict_Absent){goto _LL12;}
else{goto _LL13;}_LL13: goto _LL14;_LL12: return 0;_LL14:(void)_throw(_tmp70);_LL10:;}}}
static int Cyc_Lex_is_typedef(struct Cyc_List_List*ns,struct _tagged_arr*v){if(ns != 
0){return Cyc_Lex_is_typedef_in_namespace((struct Cyc_List_List*)_check_null(ns),v);}{
struct _tagged_arr _tmp71=*v;int len=(int)(_get_arr_size(_tmp71,sizeof(
unsigned char))- 1);struct Cyc_Lex_Trie*t=Cyc_Lex_typedefs_trie;{int i=0;for(0;i < 
len;++ i){void*_tmp72=(void*)((struct Cyc_Lex_Trie*)_check_null(t))->children;
struct Cyc_Lex_Trie*_tmp73;int _tmp74;struct Cyc_Lex_Trie*_tmp75;struct Cyc_Lex_Trie**
_tmp76;_LL16: if((int)_tmp72 == 0){goto _LL17;}else{goto _LL18;}_LL18: if((
unsigned int)_tmp72 > 1?*((int*)_tmp72)== 0: 0){_LL20: _tmp74=((struct Cyc_Lex_One_struct*)
_tmp72)->f1;goto _LL1F;_LL1F: _tmp73=((struct Cyc_Lex_One_struct*)_tmp72)->f2;goto
_LL1E;}else{goto _LL1A;}_LL1E: if(_tmp74 != ((const unsigned char*)_tmp71.curr)[i]){
goto _LL19;}else{goto _LL1A;}_LL1A: if((unsigned int)_tmp72 > 1?*((int*)_tmp72)== 0:
0){_LL21: _tmp75=((struct Cyc_Lex_One_struct*)_tmp72)->f2;goto _LL1B;}else{goto
_LL1C;}_LL1C: if((unsigned int)_tmp72 > 1?*((int*)_tmp72)== 1: 0){_LL22: _tmp76=((
struct Cyc_Lex_Many_struct*)_tmp72)->f1;goto _LL1D;}else{goto _LL15;}_LL17: return 0;
_LL19: return 0;_LL1B: t=_tmp75;goto _LL15;_LL1D: {struct Cyc_Lex_Trie*_tmp77=_tmp76[
_check_known_subscript_notnull(64,Cyc_Lex_trie_char((int)((const unsigned char*)
_tmp71.curr)[i]))];if(_tmp77 == 0){return 0;}t=_tmp77;goto _LL15;}_LL15:;}}return((
struct Cyc_Lex_Trie*)_check_null(t))->shared_str;}}void Cyc_Lex_enter_namespace(
struct _tagged_arr*s){struct Cyc_List_List*ns=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_List_List*new_ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(ns,({struct Cyc_List_List*_tmp7C=
_cycalloc(sizeof(*_tmp7C));_tmp7C->hd=s;_tmp7C->tl=0;_tmp7C;}));((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=({struct Cyc_List_List*
_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->hd=({struct Cyc_Lex_Lvis*_tmp79=
_cycalloc(sizeof(*_tmp79));_tmp79->current_namespace=new_ns;_tmp79->imported_namespaces=
0;_tmp79;});_tmp78->tl=(struct Cyc_List_List*)((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->lstack;_tmp78;});{struct Cyc_Lex_Ldecls*ld=((
struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,
ns);if(!((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(ld->namespaces,
s)){((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*
v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->decls,ns,({struct Cyc_Lex_Ldecls*_tmp7A=_cycalloc(sizeof(*
_tmp7A));_tmp7A->typedefs=ld->typedefs;_tmp7A->namespaces=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(ld->namespaces,s);
_tmp7A;}));((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*
v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->decls,new_ns,({struct Cyc_Lex_Ldecls*_tmp7B=_cycalloc(
sizeof(*_tmp7B));_tmp7B->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_zstrptrcmp);_tmp7B->namespaces=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_Std_zstrptrcmp);_tmp7B;}));}((void(*)(void(*f)(struct _tagged_arr*),struct Cyc_Set_Set*
s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls,new_ns))->typedefs);}}void Cyc_Lex_leave_namespace(){((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=(
struct Cyc_List_List*)_check_null((((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->lstack)->tl);Cyc_Lex_recompute_typedefs();}
void Cyc_Lex_enter_using(struct _tuple0*q){struct Cyc_List_List*ns;{void*_tmp7D=(*q).f1;
struct Cyc_List_List*_tmp7E;struct Cyc_List_List*_tmp7F;_LL24: if((int)_tmp7D == 0){
goto _LL25;}else{goto _LL26;}_LL26: if((unsigned int)_tmp7D > 1?*((int*)_tmp7D)== 0:
0){_LL2A: _tmp7E=((struct Cyc_Absyn_Rel_n_struct*)_tmp7D)->f1;goto _LL27;}else{goto
_LL28;}_LL28: if((unsigned int)_tmp7D > 1?*((int*)_tmp7D)== 1: 0){_LL2B: _tmp7F=((
struct Cyc_Absyn_Abs_n_struct*)_tmp7D)->f1;goto _LL29;}else{goto _LL23;}_LL25: ns=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp80=_cycalloc(sizeof(*_tmp80));
_tmp80->hd=(*q).f2;_tmp80->tl=0;_tmp80;});goto _LL23;_LL27: _tmp7F=_tmp7E;goto
_LL29;_LL29: ns=(struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp7F,({struct Cyc_List_List*
_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->hd=(*q).f2;_tmp81->tl=0;_tmp81;})));
goto _LL23;_LL23:;}{struct Cyc_List_List*_tmp82=Cyc_Lex_get_absolute_namespace(ns);
struct Cyc_List_List*_tmp83=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((
struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=({struct Cyc_List_List*
_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->hd=_tmp82;_tmp84->tl=_tmp83;_tmp84;});((
void(*)(void(*f)(struct _tagged_arr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((
struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,
_tmp82))->typedefs);}}void Cyc_Lex_leave_using(){struct Cyc_List_List*_tmp85=((
struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((
struct Cyc_List_List*)_check_null(_tmp85))->tl;Cyc_Lex_recompute_typedefs();}void
Cyc_Lex_register_typedef(struct _tuple0*q){struct Cyc_List_List*_tmp86=((struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_Dict_Dict*_tmp87=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls;struct Cyc_Lex_Ldecls*_tmp88=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp87,_tmp86);
struct Cyc_Lex_Ldecls*_tmp89=({struct Cyc_Lex_Ldecls*_tmp8A=_cycalloc(sizeof(*
_tmp8A));_tmp8A->typedefs=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_insert)(_tmp88->typedefs,(*q).f2);_tmp8A->namespaces=
_tmp88->namespaces;_tmp8A;});((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(_tmp87,_tmp86,
_tmp89);Cyc_Lex_insert_typedef((*q).f2);}static short Cyc_Lex_process_id(struct Cyc_Lexing_lexbuf*
lbuf){int symbol_num=Cyc_Lex_str_index_lbuf(lbuf);if(symbol_num <= Cyc_Lex_num_kws){
return(short)*((int*)_check_unknown_subscript(Cyc_Lex_kw_nums,sizeof(int),
symbol_num - 1));}{struct _tagged_arr*_tmp8B=Cyc_Lex_get_symbol(symbol_num);Cyc_Lex_token_string=*
_tmp8B;if(Cyc_Lex_is_typedef(0,_tmp8B)){return 350;}return 344;}}static short Cyc_Lex_process_qual_id(
struct Cyc_Lexing_lexbuf*lbuf){int i=lbuf->lex_start_pos;int end=lbuf->lex_curr_pos;
struct _tagged_arr s=lbuf->lex_buffer;struct Cyc_List_List*rev_vs=0;while(i < end){
int start=i;for(0;i < end?((unsigned char*)s.curr)[i]!= ':': 0;i ++){;}if(start == i){(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp8C=_cycalloc(sizeof(*
_tmp8C));_tmp8C[0]=({struct Cyc_Core_Impossible_struct _tmp8D;_tmp8D.tag=Cyc_Core_Impossible;
_tmp8D.f1=_tag_arr("bad namespace",sizeof(unsigned char),14);_tmp8D;});_tmp8C;}));}{
int vlen=i - start;struct _tagged_arr*v=Cyc_Lex_get_symbol(Cyc_Lex_str_index((
struct _tagged_arr)s,start,vlen));rev_vs=({struct Cyc_List_List*_tmp8E=_cycalloc(
sizeof(*_tmp8E));_tmp8E->hd=v;_tmp8E->tl=rev_vs;_tmp8E;});i +=2;}}if(rev_vs == 0){(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp8F=_cycalloc(sizeof(*
_tmp8F));_tmp8F[0]=({struct Cyc_Core_Impossible_struct _tmp90;_tmp90.tag=Cyc_Core_Impossible;
_tmp90.f1=_tag_arr("bad namespace",sizeof(unsigned char),14);_tmp90;});_tmp8F;}));}{
struct _tagged_arr*v=(struct _tagged_arr*)((struct Cyc_List_List*)_check_null(
rev_vs))->hd;struct Cyc_List_List*vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(rev_vs->tl);Cyc_Lex_token_qvar=({struct _tuple0*_tmp91=
_cycalloc(sizeof(*_tmp91));_tmp91->f1=(void*)({struct Cyc_Absyn_Rel_n_struct*
_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92[0]=({struct Cyc_Absyn_Rel_n_struct _tmp93;
_tmp93.tag=0;_tmp93.f1=vs;_tmp93;});_tmp92;});_tmp91->f2=v;_tmp91;});if(Cyc_Lex_is_typedef(
vs,v)){return 352;}return 351;}}int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*);int Cyc_Lex_strng(
struct Cyc_Lexing_lexbuf*);int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*);int Cyc_yylex(){
int ans=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lex_token)((struct Cyc_Lexing_lexbuf*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Parse_lbuf))->v);Cyc_yylloc.first_line=((int(*)(
struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_start)((struct Cyc_Lexing_lexbuf*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Parse_lbuf))->v);Cyc_yylloc.last_line=((int(*)(
struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_end)((struct Cyc_Lexing_lexbuf*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Parse_lbuf))->v);switch(ans){case 344: _LL2C:
Cyc_yylval=(void*)({struct Cyc_String_tok_struct*_tmp94=_cycalloc(sizeof(*_tmp94));
_tmp94[0]=({struct Cyc_String_tok_struct _tmp95;_tmp95.tag=Cyc_String_tok;_tmp95.f1=
Cyc_Lex_token_string;_tmp95;});_tmp94;});break;case 350: _LL2D: Cyc_yylval=(void*)({
struct Cyc_String_tok_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[0]=({struct
Cyc_String_tok_struct _tmp97;_tmp97.tag=Cyc_String_tok;_tmp97.f1=Cyc_Lex_token_string;
_tmp97;});_tmp96;});break;case 351: _LL2E: Cyc_yylval=(void*)({struct Cyc_QualId_tok_struct*
_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98[0]=({struct Cyc_QualId_tok_struct _tmp99;
_tmp99.tag=Cyc_QualId_tok;_tmp99.f1=Cyc_Lex_token_qvar;_tmp99;});_tmp98;});
break;case 352: _LL2F: Cyc_yylval=(void*)({struct Cyc_QualId_tok_struct*_tmp9A=
_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_QualId_tok_struct _tmp9B;_tmp9B.tag=
Cyc_QualId_tok;_tmp9B.f1=Cyc_Lex_token_qvar;_tmp9B;});_tmp9A;});break;case 349:
_LL30: Cyc_yylval=(void*)({struct Cyc_String_tok_struct*_tmp9C=_cycalloc(sizeof(*
_tmp9C));_tmp9C[0]=({struct Cyc_String_tok_struct _tmp9D;_tmp9D.tag=Cyc_String_tok;
_tmp9D.f1=Cyc_Lex_token_string;_tmp9D;});_tmp9C;});break;case 345: _LL31: Cyc_yylval=(
void*)({struct Cyc_Int_tok_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({
struct Cyc_Int_tok_struct _tmp9F;_tmp9F.tag=Cyc_Int_tok;_tmp9F.f1=Cyc_Lex_token_int;
_tmp9F;});_tmp9E;});break;case 347: _LL32: Cyc_yylval=(void*)({struct Cyc_Char_tok_struct*
_tmpA0=_cycalloc_atomic(sizeof(*_tmpA0));_tmpA0[0]=({struct Cyc_Char_tok_struct
_tmpA1;_tmpA1.tag=Cyc_Char_tok;_tmpA1.f1=Cyc_Lex_token_char;_tmpA1;});_tmpA0;});
break;case 348: _LL33: Cyc_yylval=(void*)({struct Cyc_String_tok_struct*_tmpA2=
_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_String_tok_struct _tmpA3;_tmpA3.tag=
Cyc_String_tok;_tmpA3.f1=Cyc_Lex_token_string;_tmpA3;});_tmpA2;});break;case 346:
_LL34: Cyc_yylval=(void*)({struct Cyc_String_tok_struct*_tmpA4=_cycalloc(sizeof(*
_tmpA4));_tmpA4[0]=({struct Cyc_String_tok_struct _tmpA5;_tmpA5.tag=Cyc_String_tok;
_tmpA5.f1=Cyc_Lex_token_string;_tmpA5;});_tmpA4;});break;default: _LL35: break;}
return ans;}const int Cyc_Lex_lex_base[149]=(const int[149]){0,113,17,83,16,2,- 3,- 1,
- 2,- 17,- 18,4,118,119,- 19,5,- 13,- 12,85,- 14,- 11,- 4,- 5,- 6,- 7,- 8,- 9,- 10,192,215,111,
- 15,166,- 18,- 55,8,30,- 39,18,31,116,139,32,138,135,137,159,250,325,83,82,84,94,
395,85,470,545,90,- 54,- 22,- 28,620,695,90,753,828,99,- 23,903,123,- 26,134,- 31,- 25,
- 34,978,1007,363,117,128,292,1017,1047,1083,335,1108,1120,1190,133,143,1260,1298,
137,147,191,201,193,203,- 6,- 38,199,- 20,- 37,6,157,1230,- 33,- 16,- 32,- 17,2,1338,168,
169,279,171,174,176,185,186,187,188,189,191,1411,1495,- 52,- 46,- 45,- 44,- 43,- 42,-
41,- 40,- 47,- 50,- 51,311,193,- 48,- 49,- 53,- 27,- 24,- 21,310,- 35,12,425};const int Cyc_Lex_lex_backtrk[
149]=(const int[149]){- 1,- 1,- 1,5,3,4,- 1,- 1,- 1,- 1,- 1,16,1,19,- 1,2,- 1,- 1,14,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,14,- 1,- 1,- 1,- 1,35,54,- 1,54,54,54,54,54,54,54,54,54,5,7,
54,54,54,54,0,54,54,54,54,- 1,- 1,- 1,3,1,- 1,- 1,2,- 1,- 1,0,29,- 1,28,- 1,- 1,- 1,9,7,- 1,
7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,- 1,- 1,36,- 1,9,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,34,35};const int Cyc_Lex_lex_default[149]=(const int[
149]){34,9,3,3,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,0,- 1,0,0,- 1,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,0,
- 1,0,0,- 1,- 1,0,145,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,
0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,100,0,0,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,0,- 1,- 1,0,0,0,0,0,0,145,0,- 1,- 1};
const int Cyc_Lex_lex_trans[1752]=(const int[1752]){0,0,0,0,0,0,0,0,0,35,35,35,35,
35,33,6,102,148,148,148,148,148,146,0,0,0,0,0,146,0,0,147,35,36,37,38,0,39,40,41,
148,141,42,43,7,44,45,46,47,48,48,48,48,48,48,48,48,48,49,4,50,51,52,8,5,53,53,
53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,17,144,
14,54,55,56,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,
53,53,53,10,57,- 1,11,32,32,6,- 1,32,15,30,30,30,30,30,30,30,30,74,71,16,72,20,67,
12,64,- 1,32,59,7,16,142,19,69,64,17,31,31,31,31,31,31,31,31,18,18,18,18,18,18,18,
18,32,32,143,- 1,32,107,31,19,104,70,105,105,105,105,105,105,105,105,105,105,73,
33,108,32,109,7,99,25,106,25,13,100,140,139,102,135,20,103,134,60,133,21,22,23,
23,101,23,22,22,132,131,130,129,128,24,127,111,136,25,25,26,25,27,0,28,29,29,29,
29,29,29,29,29,29,29,23,23,0,0,22,22,58,29,29,29,29,29,29,29,29,29,29,29,29,29,
29,29,29,6,- 1,0,24,24,98,98,29,29,29,29,29,29,0,0,0,29,29,29,29,29,29,0,75,0,86,
86,86,86,86,86,86,86,87,87,24,24,98,98,29,29,29,29,29,29,136,77,146,0,0,147,0,0,
88,137,137,137,137,137,137,137,137,89,0,0,90,- 1,81,81,81,81,81,81,81,81,81,81,
136,77,0,0,0,0,0,0,88,138,138,138,138,138,138,138,138,89,0,14,90,75,0,76,76,76,
76,76,76,76,76,76,76,85,85,85,85,85,85,85,85,85,85,0,77,- 1,0,0,0,0,0,78,0,0,0,0,
80,0,80,0,79,81,81,81,81,81,81,81,81,81,81,0,0,0,0,0,77,0,0,0,0,0,0,78,148,148,
148,148,148,0,0,0,79,68,68,68,68,68,68,68,68,68,68,63,0,- 1,0,148,0,0,68,68,68,68,
68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,0,0,0,0,68,0,
68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,62,
62,62,62,62,62,62,62,62,62,63,0,0,0,0,0,0,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,62,62,62,62,62,62,62,62,62,62,0,0,0,0,62,- 1,62,62,62,62,62,62,62,62,62,
62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,61,61,61,61,61,61,61,61,61,61,
0,0,0,0,0,0,0,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,
61,61,61,61,0,0,0,0,61,0,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,
61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,0,0,0,0,0,0,0,61,61,61,61,
61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,0,0,0,0,61,0,
61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,62,
62,62,62,62,62,62,62,62,62,63,0,0,0,0,0,0,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,62,62,62,62,62,62,62,62,62,62,0,0,0,0,62,0,62,62,62,62,62,62,62,62,62,
62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,65,65,65,65,65,65,65,65,65,65,
65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,0,0,0,0,65,0,65,65,65,65,65,65,
65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,
65,65,65,66,0,0,0,0,0,0,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,
65,65,65,65,65,65,65,0,0,0,0,65,0,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,
65,65,65,65,65,65,65,65,65,65,65,68,68,68,68,68,68,68,68,68,68,63,0,0,0,0,0,0,68,
68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,0,0,0,
0,68,0,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
68,68,82,82,82,82,82,82,82,82,82,82,0,0,0,0,0,0,0,0,0,0,0,83,27,0,0,0,0,75,27,76,
76,76,76,76,76,76,76,76,76,81,81,81,81,81,81,81,81,81,81,0,77,0,0,83,27,0,0,78,0,
0,27,26,0,0,0,0,79,26,0,82,82,82,82,82,82,82,82,82,82,0,0,0,77,0,0,0,0,0,0,78,83,
27,0,26,0,0,0,27,79,26,84,0,84,0,0,85,85,85,85,85,85,85,85,85,85,0,0,0,0,0,0,0,
83,27,0,0,0,0,0,27,85,85,85,85,85,85,85,85,85,85,75,0,86,86,86,86,86,86,86,86,87,
87,27,0,0,0,0,0,27,0,0,0,0,77,0,0,0,0,0,0,96,0,0,0,0,0,0,0,0,97,0,0,0,0,27,0,0,0,
0,0,27,0,0,0,0,77,0,0,0,0,0,0,96,0,0,0,0,0,0,0,75,97,87,87,87,87,87,87,87,87,87,
87,0,0,0,0,0,0,0,0,0,0,0,77,0,0,0,0,0,0,94,0,0,0,0,0,0,0,0,95,0,0,105,105,105,
105,105,105,105,105,105,105,0,0,0,77,0,0,0,0,0,0,94,83,27,0,0,0,0,0,27,95,91,91,
91,91,91,91,91,91,91,91,0,0,0,0,0,0,0,91,91,91,91,91,91,83,27,0,0,0,0,0,27,0,0,0,
0,0,0,0,91,91,91,91,91,91,91,91,91,91,0,91,91,91,91,91,91,91,91,91,91,91,91,0,0,
0,112,0,92,0,0,113,0,0,0,0,0,93,0,0,114,114,114,114,114,114,114,114,0,91,91,91,
91,91,91,115,0,0,0,0,92,0,0,0,0,0,0,0,0,93,0,0,0,0,0,0,0,0,0,0,0,0,0,0,116,0,0,0,
0,117,118,0,0,0,119,0,0,0,0,0,0,0,120,0,0,0,121,0,122,0,123,0,124,125,125,125,
125,125,125,125,125,125,125,0,0,0,0,0,0,0,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,0,0,0,0,0,0,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,126,0,0,0,0,0,0,0,0,125,125,125,125,125,125,125,125,125,
125,0,0,0,0,0,0,0,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,0,0,0,0,0,0,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0};const int Cyc_Lex_lex_check[1752]=(const int[1752]){- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,11,15,103,35,35,35,35,35,147,- 1,- 1,- 1,- 1,- 1,38,
- 1,- 1,38,0,0,0,0,- 1,0,0,0,35,110,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,4,2,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,39,42,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,3,1,12,12,13,3,12,13,18,18,18,18,18,18,
18,18,49,50,50,50,51,54,1,63,41,12,57,12,13,40,52,52,66,13,30,30,30,30,30,30,30,
30,13,13,13,13,13,13,13,13,32,32,40,41,32,44,43,13,45,69,45,45,45,45,45,45,45,45,
45,45,71,44,44,32,43,32,46,78,104,79,1,46,112,113,100,115,13,100,116,57,117,13,
13,88,89,46,13,92,93,118,119,120,121,122,13,123,41,138,13,78,13,79,13,- 1,13,28,
28,28,28,28,28,28,28,28,28,88,89,- 1,- 1,92,93,0,28,28,28,28,28,28,29,29,29,29,29,
29,29,29,29,29,2,38,- 1,94,95,96,97,29,29,29,29,29,29,- 1,- 1,- 1,28,28,28,28,28,28,
- 1,47,- 1,47,47,47,47,47,47,47,47,47,47,94,95,96,97,29,29,29,29,29,29,114,47,145,
- 1,- 1,145,- 1,- 1,47,114,114,114,114,114,114,114,114,47,- 1,- 1,47,3,80,80,80,80,80,
80,80,80,80,80,137,47,- 1,- 1,- 1,- 1,- 1,- 1,47,137,137,137,137,137,137,137,137,47,- 1,
1,47,48,- 1,48,48,48,48,48,48,48,48,48,48,84,84,84,84,84,84,84,84,84,84,- 1,48,41,
- 1,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,77,- 1,77,- 1,48,77,77,77,77,77,77,77,77,77,77,- 1,- 1,
- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,- 1,48,148,148,148,148,148,- 1,- 1,- 1,48,53,53,53,53,53,
53,53,53,53,53,53,- 1,100,- 1,148,- 1,- 1,53,53,53,53,53,53,53,53,53,53,53,53,53,53,
53,53,53,53,53,53,53,53,53,53,53,53,- 1,- 1,- 1,- 1,53,- 1,53,53,53,53,53,53,53,53,53,
53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,55,55,55,55,55,55,55,55,55,55,
55,- 1,- 1,- 1,- 1,- 1,- 1,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,
55,55,55,55,55,55,- 1,- 1,- 1,- 1,55,145,55,55,55,55,55,55,55,55,55,55,55,55,55,55,
55,55,55,55,55,55,55,55,55,55,55,55,56,56,56,56,56,56,56,56,56,56,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,
56,- 1,- 1,- 1,- 1,56,- 1,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,
56,56,56,56,56,56,61,61,61,61,61,61,61,61,61,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,61,61,61,61,
61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,- 1,- 1,- 1,- 1,61,
- 1,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,
62,62,62,62,62,62,62,62,62,62,62,- 1,- 1,- 1,- 1,- 1,- 1,62,62,62,62,62,62,62,62,62,62,
62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,- 1,- 1,- 1,- 1,62,- 1,62,62,62,62,62,
62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,64,64,64,64,64,64,
64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,- 1,- 1,- 1,- 1,64,- 1,64,
64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,65,65,
65,65,65,65,65,65,65,65,65,- 1,- 1,- 1,- 1,- 1,- 1,65,65,65,65,65,65,65,65,65,65,65,65,
65,65,65,65,65,65,65,65,65,65,65,65,65,65,- 1,- 1,- 1,- 1,65,- 1,65,65,65,65,65,65,65,
65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,68,68,68,68,68,68,68,68,
68,68,68,- 1,- 1,- 1,- 1,- 1,- 1,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
68,68,68,68,68,68,68,68,- 1,- 1,- 1,- 1,68,- 1,68,68,68,68,68,68,68,68,68,68,68,68,68,
68,68,68,68,68,68,68,68,68,68,68,68,68,75,75,75,75,75,75,75,75,75,75,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,75,75,- 1,- 1,- 1,- 1,76,75,76,76,76,76,76,76,76,76,76,76,81,81,
81,81,81,81,81,81,81,81,- 1,76,- 1,- 1,75,75,- 1,- 1,76,- 1,- 1,75,81,- 1,- 1,- 1,- 1,76,81,
- 1,82,82,82,82,82,82,82,82,82,82,- 1,- 1,- 1,76,- 1,- 1,- 1,- 1,- 1,- 1,76,82,82,- 1,81,- 1,
- 1,- 1,82,76,81,83,- 1,83,- 1,- 1,83,83,83,83,83,83,83,83,83,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
82,82,- 1,- 1,- 1,- 1,- 1,82,85,85,85,85,85,85,85,85,85,85,86,- 1,86,86,86,86,86,86,86,
86,86,86,85,- 1,- 1,- 1,- 1,- 1,85,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,85,- 1,- 1,- 1,- 1,- 1,85,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,86,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,86,87,87,87,87,87,87,87,87,87,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,105,105,105,
105,105,105,105,105,105,105,- 1,- 1,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,87,105,105,- 1,- 1,- 1,- 1,
- 1,105,87,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,90,90,90,90,90,
105,105,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,91,91,91,91,91,91,91,91,91,- 1,
90,90,90,90,90,90,91,91,91,91,91,91,- 1,- 1,- 1,111,- 1,91,- 1,- 1,111,- 1,- 1,- 1,- 1,- 1,
91,- 1,- 1,111,111,111,111,111,111,111,111,- 1,91,91,91,91,91,91,111,- 1,- 1,- 1,- 1,91,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,-
1,- 1,111,111,- 1,- 1,- 1,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,- 1,111,- 1,111,- 1,111,- 1,
111,124,124,124,124,124,124,124,124,124,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,- 1,- 1,- 1,- 1,- 1,- 1,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
125,125,125,125,125,125,125,125,125,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,- 1,- 1,- 1,- 1,- 1,- 1,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};int Cyc_Lex_lex_engine(int
start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;state=
start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){base=Cyc_Lex_lex_base[
state];if(base < 0){return(- base)- 1;}backtrk=Cyc_Lex_lex_backtrk[state];if(
backtrk >= 0){lbuf->lex_last_pos=lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){if(! lbuf->lex_eof_reached){return(
- state)- 1;}else{c=256;}}else{c=(int)*((unsigned char*)_check_unknown_subscript(
lbuf->lex_buffer,sizeof(unsigned char),lbuf->lex_curr_pos ++));if(c == - 1){c=256;}}
if(Cyc_Lex_lex_check[_check_known_subscript_notnull(1752,base + c)]== state){
state=Cyc_Lex_lex_trans[_check_known_subscript_notnull(1752,base + c)];}else{
state=Cyc_Lex_lex_default[state];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1){(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Lexing_Error_struct
_tmpA7;_tmpA7.tag=Cyc_Lexing_Error;_tmpA7.f1=_tag_arr("empty token",sizeof(
unsigned char),12);_tmpA7;});_tmpA6;}));}else{return lbuf->lex_last_action;}}
else{if(c == 256){lbuf->lex_eof_reached=0;}}}}int Cyc_Lex_token_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL37: return(int)Cyc_Lex_process_id(lexbuf);case 1: _LL38: return(int)Cyc_Lex_process_id(
lexbuf);case 2: _LL39: return(int)Cyc_Lex_process_qual_id(lexbuf);case 3: _LL3A: Cyc_Lex_token_string=*
Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(lexbuf));return 349;case 4: _LL3B: Cyc_Lex_token_int=
Cyc_Lex_intconst(lexbuf,2,0,16);return 345;case 5: _LL3C: Cyc_Lex_token_int=Cyc_Lex_intconst(
lexbuf,0,0,8);return 345;case 6: _LL3D: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,
0,10);return 345;case 7: _LL3E: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 345;case 8: _LL3F: Cyc_Lex_token_string=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);return 348;case 9: _LL40: Cyc_Lex_token_string=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);return 348;case 10: _LL41: return 327;case 11: _LL42: return 328;case 12: _LL43:
return 325;case 13: _LL44: return 326;case 14: _LL45: return 321;case 15: _LL46: return 322;
case 16: _LL47: return 334;case 17: _LL48: return 335;case 18: _LL49: return 331;case 19:
_LL4A: return 332;case 20: _LL4B: return 333;case 21: _LL4C: return 340;case 22: _LL4D:
return 339;case 23: _LL4E: return 338;case 24: _LL4F: return 336;case 25: _LL50: return 337;
case 26: _LL51: return 329;case 27: _LL52: return 330;case 28: _LL53: return 323;case 29:
_LL54: return 324;case 30: _LL55: return 342;case 31: _LL56: return 320;case 32: _LL57:
return 341;case 33: _LL58: return 343;case 34: _LL59: return Cyc_Lex_token(lexbuf);case 35:
_LL5A: return Cyc_Lex_token(lexbuf);case 36: _LL5B: return Cyc_Lex_token(lexbuf);case
37: _LL5C: Cyc_Lex_comment_depth=1;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(
lexbuf);Cyc_Lex_comment(lexbuf);return Cyc_Lex_token(lexbuf);case 38: _LL5D: Cyc_Lex_string_pos=
0;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);while(Cyc_Lex_strng(
lexbuf)){;}Cyc_Lex_token_string=(struct _tagged_arr)Cyc_Lex_get_stored_string();
return 346;case 39: _LL5E: Cyc_Lex_token_char='\a';return 347;case 40: _LL5F: Cyc_Lex_token_char='\b';
return 347;case 41: _LL60: Cyc_Lex_token_char='\f';return 347;case 42: _LL61: Cyc_Lex_token_char='\n';
return 347;case 43: _LL62: Cyc_Lex_token_char='\r';return 347;case 44: _LL63: Cyc_Lex_token_char='\t';
return 347;case 45: _LL64: Cyc_Lex_token_char='\v';return 347;case 46: _LL65: Cyc_Lex_token_char='\\';
return 347;case 47: _LL66: Cyc_Lex_token_char='\'';return 347;case 48: _LL67: Cyc_Lex_token_char='"';
return 347;case 49: _LL68: Cyc_Lex_token_char='?';return 347;case 50: _LL69: Cyc_Lex_token_char=(
unsigned char)(*Cyc_Lex_intconst(lexbuf,2,1,8)).f2;return 347;case 51: _LL6A: Cyc_Lex_token_char=(
unsigned char)(*Cyc_Lex_intconst(lexbuf,3,1,16)).f2;return 347;case 52: _LL6B: Cyc_Lex_token_char=
Cyc_Lexing_lexeme_char(lexbuf,1);return 347;case 53: _LL6C: return - 1;case 54: _LL6D:
return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default: _LL6E:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_token_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_Lexing_Error_struct
_tmpA9;_tmpA9.tag=Cyc_Lexing_Error;_tmpA9.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmpA9;});_tmpA8;}));}int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_Lex_token_rec(lexbuf,0);}int Cyc_Lex_strng_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL70: return 1;case 1: _LL71: return 0;case 2: _LL72: return 1;case 3: _LL73: Cyc_Lex_store_string_char('\a');
return 1;case 4: _LL74: Cyc_Lex_store_string_char('\b');return 1;case 5: _LL75: Cyc_Lex_store_string_char('\f');
return 1;case 6: _LL76: Cyc_Lex_store_string_char('\n');return 1;case 7: _LL77: Cyc_Lex_store_string_char('\r');
return 1;case 8: _LL78: Cyc_Lex_store_string_char('\t');return 1;case 9: _LL79: Cyc_Lex_store_string_char('\v');
return 1;case 10: _LL7A: Cyc_Lex_store_string_char('\\');return 1;case 11: _LL7B: Cyc_Lex_store_string_char('\'');
return 1;case 12: _LL7C: Cyc_Lex_store_string_char('"');return 1;case 13: _LL7D: Cyc_Lex_store_string_char('?');
return 1;case 14: _LL7E: Cyc_Lex_store_string_char((unsigned char)(*Cyc_Lex_intconst(
lexbuf,1,0,8)).f2);return 1;case 15: _LL7F: Cyc_Lex_store_string_char((unsigned char)(*
Cyc_Lex_intconst(lexbuf,2,0,16)).f2);return 1;case 16: _LL80: Cyc_Lex_store_string_char(
Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 17: _LL81: Cyc_Lex_runaway_err(
_tag_arr("string ends in newline",sizeof(unsigned char),23),lexbuf);return 0;case
18: _LL82: Cyc_Lex_runaway_err(_tag_arr("unterminated string",sizeof(unsigned char),
20),lexbuf);return 0;case 19: _LL83: Cyc_Lex_err(_tag_arr("bad character following backslash in string",
sizeof(unsigned char),44),lexbuf);return 1;default: _LL84:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_strng_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_Lexing_Error_struct
_tmpAB;_tmpAB.tag=Cyc_Lexing_Error;_tmpAB.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmpAB;});_tmpAA;}));}int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_Lex_strng_rec(lexbuf,1);}int Cyc_Lex_comment_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL86: ++ Cyc_Lex_comment_depth;return Cyc_Lex_comment(lexbuf);case 1: _LL87: --
Cyc_Lex_comment_depth;if(Cyc_Lex_comment_depth > 0){return Cyc_Lex_comment(lexbuf);}
return 0;case 2: _LL88: Cyc_Lex_runaway_err(_tag_arr("unterminated comment",sizeof(
unsigned char),21),lexbuf);return 0;case 3: _LL89: return Cyc_Lex_comment(lexbuf);
case 4: _LL8A: return Cyc_Lex_comment(lexbuf);case 5: _LL8B: return Cyc_Lex_comment(
lexbuf);default: _LL8C:(lexbuf->refill_buff)(lexbuf);return Cyc_Lex_comment_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpAC=
_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({struct Cyc_Lexing_Error_struct _tmpAD;
_tmpAD.tag=Cyc_Lexing_Error;_tmpAD.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmpAD;});_tmpAC;}));}int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_Lex_comment_rec(lexbuf,2);}void Cyc_Lex_lex_init(){Cyc_Lex_ids_trie=({
struct Cyc_Lex_Trie*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->children=(void*)((
void*)0);_tmpAE->shared_str=0;_tmpAE;});Cyc_Lex_typedefs_trie=({struct Cyc_Lex_Trie*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->children=(void*)((void*)0);_tmpAF->shared_str=
0;_tmpAF;});Cyc_Lex_symbols=(struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(
int,struct _tagged_arr*))Cyc_Xarray_create)(101,_init_tag_arr(_cycalloc(sizeof(
struct _tagged_arr)),"",sizeof(unsigned char),1));((void(*)(struct Cyc_Xarray_Xarray*,
struct _tagged_arr*))Cyc_Xarray_add)((struct Cyc_Xarray_Xarray*)_check_null(Cyc_Lex_symbols),&
Cyc_Lex_bogus_string);Cyc_Lex_num_kws=(int)63;Cyc_Lex_kw_nums=({unsigned int
_tmpB1=(unsigned int)Cyc_Lex_num_kws;int*_tmpB2=(int*)_cycalloc_atomic(
_check_times(sizeof(int),_tmpB1));struct _tagged_arr _tmpB4=_tag_arr(_tmpB2,
sizeof(int),(unsigned int)Cyc_Lex_num_kws);{unsigned int _tmpB3=_tmpB1;
unsigned int i;for(i=0;i < _tmpB3;i ++){_tmpB2[i]=0;}};_tmpB4;});{int i=0;for(0;i < 
Cyc_Lex_num_kws;++ i){struct _tagged_arr _tmpB5=(Cyc_Lex_rw_array[i]).f1;Cyc_Lex_str_index(
_tmpB5,0,(int)Cyc_Std_strlen(_tmpB5));*((int*)_check_unknown_subscript(Cyc_Lex_kw_nums,
sizeof(int),i))=(int)(Cyc_Lex_rw_array[i]).f2;}}Cyc_Lex_typedef_init();Cyc_Lex_comment_depth=
0;}
