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
 struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;extern
unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_vrprintf(struct
_RegionHandle*r1,struct _tagged_arr fmt,struct _tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};extern unsigned char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{void(*
refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};extern struct
Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc_Std___cycFILE*);struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern int Cyc_List_length(struct Cyc_List_List*x);
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct
Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[18];extern void Cyc_List_iter(
void(*f)(void*),struct Cyc_List_List*x);extern void*Cyc_List_fold_right(void*(*f)(
void*,void*),struct Cyc_List_List*x,void*accum);extern struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*
x);extern unsigned char Cyc_List_Nth[8];extern struct Cyc_List_List*Cyc_List_zip(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};extern struct _tuple0 Cyc_List_split(struct Cyc_List_List*
x);extern unsigned int Cyc_Std_strlen(struct _tagged_arr s);extern int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);extern int Cyc_Std_zstrcmp(struct
_tagged_arr,struct _tagged_arr);extern int Cyc_Std_zstrptrcmp(struct _tagged_arr*,
struct _tagged_arr*);extern struct _tagged_arr Cyc_Std_strcat(struct _tagged_arr dest,
struct _tagged_arr src);extern struct _tagged_arr Cyc_Std_strcpy(struct _tagged_arr
dest,struct _tagged_arr src);extern struct _tagged_arr Cyc_Std_substring(struct
_tagged_arr,int ofs,unsigned int n);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;extern struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);extern struct Cyc_Position_Segment*Cyc_Position_segment_join(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);extern struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern struct Cyc_Position_Error*
Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*,struct _tagged_arr);extern
unsigned char Cyc_Position_Nocontext[14];extern void Cyc_Position_post_error(struct
Cyc_Position_Error*);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple1{
void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};
struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
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
_tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};
struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Core_Opt*f3;};struct Cyc_Absyn_AccessEff_struct{
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
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Aggrdecl*f3;};
struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};
struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
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
struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple1*f1;
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
_tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Aggrfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
exist_vars;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};
struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{
int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int
tag;struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[
15];extern void*Cyc_Absyn_rel_ns_null;extern int Cyc_Absyn_is_qvar_qualified(struct
_tuple1*);extern struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual
x,struct Cyc_Absyn_Tqual y);extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern void*Cyc_Absyn_compress_kb(void*);extern void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_uchar_t;extern void*Cyc_Absyn_uint_t;extern void*Cyc_Absyn_sint_t;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_double_typ(int);extern struct
_tuple1*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
extern void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b);
extern void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b);
extern void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct
Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(unsigned char c,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _tagged_arr f,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _tagged_arr s,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*,
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct
Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,
void*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_tagged_arr*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(
struct Cyc_List_List*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);extern
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct
Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _tagged_arr*lab,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_forarray_stmt(struct Cyc_List_List*,
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);
extern struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct
Cyc_Core_Opt*t_opt,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);extern
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,
struct Cyc_Absyn_Exp*init);extern struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,
void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*exist_ts,struct
Cyc_Core_Opt*fs,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);extern
struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple1*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_xtunion,struct Cyc_Position_Segment*loc);extern
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,
void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*);extern struct
_tagged_arr Cyc_Absyn_attribute2string(void*);extern int Cyc_Absyn_fntype_att(void*
a);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];
struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
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
ae;struct Cyc_Core_Opt*le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void
Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct
_tagged_arr ap);extern void*Cyc_Tcutil_copy_type(void*t);extern void*Cyc_Tcutil_compress(
void*t);extern void Cyc_Lex_register_typedef(struct _tuple1*s);extern void Cyc_Lex_enter_namespace(
struct _tagged_arr*);extern void Cyc_Lex_leave_namespace();extern void Cyc_Lex_enter_using(
struct _tuple1*);extern void Cyc_Lex_leave_using();struct Cyc_Parse_Signed_spec_struct{
int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Unsigned_spec_struct{int
tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Short_spec_struct{int tag;
struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Long_spec_struct{int tag;struct
Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{int tag;void*f1;struct
Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{int tag;struct Cyc_Absyn_Decl*
f1;};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*sc;struct Cyc_Absyn_Tqual
tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*attributes;};
struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;};struct Cyc_Parse_Abstractdeclarator{
struct Cyc_List_List*tms;};struct _tuple4{void*f1;struct Cyc_Core_Opt*f2;};static
struct _tuple4 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc);struct _tuple5{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct
Cyc_List_List*f4;};static struct _tuple5 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,
void*,struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Core_Opt*Cyc_Parse_lbuf=
0;static struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(
struct _tagged_arr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(Cyc_Position_mk_err_parse(
sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap){Cyc_Parse_err((struct _tagged_arr)Cyc_Std_vrprintf(
Cyc_Core_heap_region,fmt,ap),sg);(int)_throw((void*)Cyc_Position_Exit);}static
void Cyc_Parse_unimp(struct _tagged_arr msg,struct Cyc_Position_Segment*sg){({struct
Cyc_Std_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(struct _tagged_arr)msg;{
struct Cyc_Std_String_pa_struct _tmp1;_tmp1.tag=0;_tmp1.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(
sg);{void*_tmp0[2]={& _tmp1,& _tmp2};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: Warning: Cyclone does not yet support %s\n",
sizeof(unsigned char),46),_tag_arr(_tmp0,sizeof(void*),2));}}});return;}static
int Cyc_Parse_enum_counter=0;struct _tuple1*Cyc_Parse_gensym_enum(){return({struct
_tuple1*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->f1=(void*)({struct Cyc_Absyn_Rel_n_struct*
_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=({struct Cyc_Absyn_Rel_n_struct _tmp8;
_tmp8.tag=0;_tmp8.f1=0;_tmp8;});_tmp7;});_tmp3->f2=({struct _tagged_arr*_tmp4=
_cycalloc(sizeof(*_tmp4));_tmp4[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp6;_tmp6.tag=1;_tmp6.f1=(int)((unsigned int)Cyc_Parse_enum_counter ++);{void*
_tmp5[1]={& _tmp6};Cyc_Std_aprintf(_tag_arr("__anonymous_enum_%d__",sizeof(
unsigned char),22),_tag_arr(_tmp5,sizeof(void*),1));}});_tmp4;});_tmp3;});}
struct _tuple6{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*
f4;struct Cyc_List_List*f5;};struct _tuple7{struct _tuple6*f1;struct Cyc_Absyn_Exp*
f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*
loc,struct _tuple7*field_info){struct _tuple7 _tmpA;struct Cyc_Absyn_Exp*_tmpB;
struct _tuple6*_tmpC;struct _tuple6 _tmpD;struct Cyc_List_List*_tmpE;struct Cyc_List_List*
_tmpF;void*_tmp10;struct Cyc_Absyn_Tqual _tmp11;struct _tuple1*_tmp12;struct _tuple7*
_tmp9=field_info;_tmpA=*_tmp9;_LL2: _tmpC=_tmpA.f1;_tmpD=*_tmpC;_LL7: _tmp12=_tmpD.f1;
goto _LL6;_LL6: _tmp11=_tmpD.f2;goto _LL5;_LL5: _tmp10=_tmpD.f3;goto _LL4;_LL4: _tmpF=
_tmpD.f4;goto _LL3;_LL3: _tmpE=_tmpD.f5;goto _LL1;_LL1: _tmpB=_tmpA.f2;goto _LL0;_LL0:
if(_tmpF != 0){Cyc_Parse_err(_tag_arr("bad type params in struct field",sizeof(
unsigned char),32),loc);}if(Cyc_Absyn_is_qvar_qualified(_tmp12)){Cyc_Parse_err(
_tag_arr("struct or union field cannot be qualified with a namespace",sizeof(
unsigned char),59),loc);}return({struct Cyc_Absyn_Aggrfield*_tmp13=_cycalloc(
sizeof(*_tmp13));_tmp13->name=(*_tmp12).f2;_tmp13->tq=_tmp11;_tmp13->type=(void*)
_tmp10;_tmp13->width=_tmpB;_tmp13->attributes=_tmpE;_tmp13;});}static void*Cyc_Parse_type_spec(
void*t,struct Cyc_Position_Segment*loc){return(void*)({struct Cyc_Parse_Type_spec_struct*
_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14[0]=({struct Cyc_Parse_Type_spec_struct
_tmp15;_tmp15.tag=4;_tmp15.f1=(void*)t;_tmp15.f2=loc;_tmp15;});_tmp14;});}static
void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp16=t;struct Cyc_Absyn_Exp*
_tmp17;struct Cyc_Absyn_Tqual _tmp18;void*_tmp19;_LL9: if((unsigned int)_tmp16 > 3?*((
int*)_tmp16)== 7: 0){_LLF: _tmp19=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp16)->f1;goto _LLE;_LLE: _tmp18=((struct Cyc_Absyn_ArrayType_struct*)_tmp16)->f2;
goto _LLD;_LLD: _tmp17=((struct Cyc_Absyn_ArrayType_struct*)_tmp16)->f3;goto _LLA;}
else{goto _LLB;}_LLB: goto _LLC;_LLA: return Cyc_Absyn_starb_typ(_tmp19,argposn? Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->v=(void*)((void*)3);
_tmp1A;}),0):(void*)2,_tmp18,_tmp17 == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp17);_tmp1C;});
_tmp1B;}));_LLC: return t;_LL8:;}static void Cyc_Parse_arg_array2ptr(struct _tuple2*x){(*
x).f3=Cyc_Parse_array2ptr((*x).f3,1);}struct _tuple8{struct Cyc_Absyn_Tqual f1;void*
f2;};static struct _tuple8*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,
struct _tuple2*t){return({struct _tuple8*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->f1=(*
t).f2;_tmp1D->f2=(*t).f3;_tmp1D;});}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(
struct Cyc_Absyn_Decl*d){void*_tmp1E=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp1F;
_LL11: if(*((int*)_tmp1E)== 0){_LL15: _tmp1F=((struct Cyc_Absyn_Var_d_struct*)
_tmp1E)->f1;goto _LL12;}else{goto _LL13;}_LL13: goto _LL14;_LL12: return _tmp1F;_LL14:({
void*_tmp20[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Parse_abort)(d->loc,_tag_arr("bad declaration in `forarray' statement",
sizeof(unsigned char),40),_tag_arr(_tmp20,sizeof(void*),0));});_LL10:;}static int
Cyc_Parse_is_typeparam(void*tm){void*_tmp21=tm;_LL17: if((unsigned int)_tmp21 > 1?*((
int*)_tmp21)== 3: 0){goto _LL18;}else{goto _LL19;}_LL19: goto _LL1A;_LL18: return 1;
_LL1A: return 0;_LL16:;}static void*Cyc_Parse_id2type(struct _tagged_arr s,void*k){
if(Cyc_Std_zstrcmp(s,_tag_arr("`H",sizeof(unsigned char),3))== 0){return(void*)2;}
else{return(void*)({struct Cyc_Absyn_VarType_struct*_tmp22=_cycalloc(sizeof(*
_tmp22));_tmp22[0]=({struct Cyc_Absyn_VarType_struct _tmp23;_tmp23.tag=1;_tmp23.f1=({
struct Cyc_Absyn_Tvar*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->name=({struct
_tagged_arr*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25[0]=s;_tmp25;});_tmp24->identity=
0;_tmp24->kind=(void*)k;_tmp24;});_tmp23;});_tmp22;});}}static struct Cyc_Absyn_Tvar*
Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){void*k;{void*_tmp26=Cyc_Absyn_compress_kb((
void*)t->kind);void*_tmp27;void*_tmp28;_LL1C: if(*((int*)_tmp26)== 0){_LL22:
_tmp27=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp26)->f1;goto _LL1D;}else{goto
_LL1E;}_LL1E: if(*((int*)_tmp26)== 1){goto _LL1F;}else{goto _LL20;}_LL20: if(*((int*)
_tmp26)== 2){_LL23: _tmp28=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp26)->f2;
goto _LL21;}else{goto _LL1B;}_LL1D: k=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp29=
_cycalloc(sizeof(*_tmp29));_tmp29[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp2A;_tmp2A.tag=
0;_tmp2A.f1=(void*)_tmp27;_tmp2A;});_tmp29;});goto _LL1B;_LL1F: k=(void*)({struct
Cyc_Absyn_Unknown_kb_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B[0]=({struct
Cyc_Absyn_Unknown_kb_struct _tmp2C;_tmp2C.tag=1;_tmp2C.f1=0;_tmp2C;});_tmp2B;});
goto _LL1B;_LL21: k=(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp2D=_cycalloc(
sizeof(*_tmp2D));_tmp2D[0]=({struct Cyc_Absyn_Less_kb_struct _tmp2E;_tmp2E.tag=2;
_tmp2E.f1=0;_tmp2E.f2=(void*)_tmp28;_tmp2E;});_tmp2D;});goto _LL1B;_LL1B:;}return({
struct Cyc_Absyn_Tvar*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->name=t->name;
_tmp2F->identity=0;_tmp2F->kind=(void*)k;_tmp2F;});}static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*loc,void*t){void*_tmp30=t;struct
Cyc_Absyn_Tvar*_tmp31;_LL25: if((unsigned int)_tmp30 > 3?*((int*)_tmp30)== 1: 0){
_LL29: _tmp31=((struct Cyc_Absyn_VarType_struct*)_tmp30)->f1;goto _LL26;}else{goto
_LL27;}_LL27: goto _LL28;_LL26: return _tmp31;_LL28:({void*_tmp32[0]={};((int(*)(
struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(
loc,_tag_arr("expecting a list of type variables, not types",sizeof(
unsigned char),46),_tag_arr(_tmp32,sizeof(void*),0));});_LL24:;}static void*Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar*pr){return(void*)({struct Cyc_Absyn_VarType_struct*_tmp33=
_cycalloc(sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Absyn_VarType_struct _tmp34;
_tmp34.tag=1;_tmp34.f1=pr;_tmp34;});_tmp33;});}static void Cyc_Parse_set_vartyp_kind(
void*t,void*k){void*_tmp35=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp36;
struct Cyc_Absyn_Tvar _tmp37;void*_tmp38;void**_tmp39;_LL2B: if((unsigned int)
_tmp35 > 3?*((int*)_tmp35)== 1: 0){_LL2F: _tmp36=((struct Cyc_Absyn_VarType_struct*)
_tmp35)->f1;_tmp37=*_tmp36;_LL30: _tmp38=(void*)_tmp37.kind;_tmp39=(void**)&(*((
struct Cyc_Absyn_VarType_struct*)_tmp35)->f1).kind;goto _LL2C;}else{goto _LL2D;}
_LL2D: goto _LL2E;_LL2C: {void*_tmp3A=Cyc_Absyn_compress_kb(*_tmp39);_LL32: if(*((
int*)_tmp3A)== 1){goto _LL33;}else{goto _LL34;}_LL34: goto _LL35;_LL33:*_tmp39=(void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp3C;_tmp3C.tag=0;_tmp3C.f1=(void*)k;_tmp3C;});
_tmp3B;});return;_LL35: return;_LL31:;}_LL2E: return;_LL2A:;}static struct Cyc_List_List*
Cyc_Parse_oldstyle2newstyle(struct Cyc_List_List*tms,struct Cyc_List_List*tds,
struct Cyc_Position_Segment*loc){if(tds == 0){return tms;}if(tms == 0){return 0;}{
void*_tmp3D=(void*)tms->hd;void*_tmp3E;_LL37: if((unsigned int)_tmp3D > 1?*((int*)
_tmp3D)== 2: 0){_LL3B: _tmp3E=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp3D)->f1;
goto _LL38;}else{goto _LL39;}_LL39: goto _LL3A;_LL38: if(tms->tl == 0? 1:(Cyc_Parse_is_typeparam((
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)?((struct Cyc_List_List*)
_check_null(tms->tl))->tl == 0: 0)){void*_tmp3F=_tmp3E;struct Cyc_List_List*_tmp40;
_LL3D: if(*((int*)_tmp3F)== 1){goto _LL3E;}else{goto _LL3F;}_LL3F: if(*((int*)_tmp3F)
== 0){_LL41: _tmp40=((struct Cyc_Absyn_NoTypes_struct*)_tmp3F)->f1;goto _LL40;}
else{goto _LL3C;}_LL3E:({void*_tmp41[0]={};Cyc_Tcutil_warn(loc,_tag_arr("function declaration with both new- and old-style parameter declarations; ignoring old-style",
sizeof(unsigned char),93),_tag_arr(_tmp41,sizeof(void*),0));});return tms;_LL40:
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp40)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(tds)){({void*_tmp42[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,_tag_arr("wrong number of parameter declarations in old-style function declaration",
sizeof(unsigned char),73),_tag_arr(_tmp42,sizeof(void*),0));});}{struct Cyc_List_List*
_tmp43=0;for(0;_tmp40 != 0;_tmp40=_tmp40->tl){struct Cyc_List_List*_tmp44=tds;for(
0;_tmp44 != 0;_tmp44=_tmp44->tl){struct Cyc_Absyn_Decl*_tmp45=(struct Cyc_Absyn_Decl*)
_tmp44->hd;void*_tmp46=(void*)_tmp45->r;struct Cyc_Absyn_Vardecl*_tmp47;_LL43: if(*((
int*)_tmp46)== 0){_LL47: _tmp47=((struct Cyc_Absyn_Var_d_struct*)_tmp46)->f1;goto
_LL44;}else{goto _LL45;}_LL45: goto _LL46;_LL44: if(Cyc_Std_zstrptrcmp((*_tmp47->name).f2,(
struct _tagged_arr*)_tmp40->hd)!= 0){continue;}if(_tmp47->initializer != 0){({void*
_tmp48[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Parse_abort)(_tmp45->loc,_tag_arr("initializer found in parameter declaration",
sizeof(unsigned char),43),_tag_arr(_tmp48,sizeof(void*),0));});}if(Cyc_Absyn_is_qvar_qualified(
_tmp47->name)){({void*_tmp49[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp45->loc,_tag_arr("namespaces forbidden in parameter declarations",
sizeof(unsigned char),47),_tag_arr(_tmp49,sizeof(void*),0));});}_tmp43=({struct
Cyc_List_List*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->hd=({struct _tuple2*
_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->f1=({struct Cyc_Core_Opt*_tmp4C=
_cycalloc(sizeof(*_tmp4C));_tmp4C->v=(*_tmp47->name).f2;_tmp4C;});_tmp4B->f2=
_tmp47->tq;_tmp4B->f3=(void*)_tmp47->type;_tmp4B;});_tmp4A->tl=_tmp43;_tmp4A;});
goto L;_LL46:({void*_tmp4D[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp45->loc,_tag_arr("nonvariable declaration in parameter type",
sizeof(unsigned char),42),_tag_arr(_tmp4D,sizeof(void*),0));});_LL42:;}L: if(
_tmp44 == 0){({struct Cyc_Std_String_pa_struct _tmp4F;_tmp4F.tag=0;_tmp4F.f1=(
struct _tagged_arr)*((struct _tagged_arr*)_tmp40->hd);{void*_tmp4E[1]={& _tmp4F};((
int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Parse_abort)(loc,_tag_arr("%s is not given a type",sizeof(unsigned char),23),
_tag_arr(_tmp4E,sizeof(void*),1));}});}}return({struct Cyc_List_List*_tmp50=
_cycalloc(sizeof(*_tmp50));_tmp50->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp52;_tmp52.tag=2;_tmp52.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp54;_tmp54.tag=1;_tmp54.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp43);_tmp54.f2=0;_tmp54.f3=0;_tmp54.f4=0;_tmp54.f5=0;_tmp54;});
_tmp53;}));_tmp52;});_tmp51;}));_tmp50->tl=0;_tmp50;});}_LL3C:;}goto _LL3A;_LL3A:
return({struct Cyc_List_List*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->hd=(void*)((
void*)tms->hd);_tmp55->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);_tmp55;});
_LL36:;}}struct _tuple9{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,
struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,
struct Cyc_Position_Segment*loc){if(tds != 0){d=({struct Cyc_Parse_Declarator*
_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->id=d->id;_tmp56->tms=Cyc_Parse_oldstyle2newstyle(
d->tms,tds,loc);_tmp56;});}{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual();int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp57=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL49: if((int)_tmp57 == 1){goto _LL4A;}
else{goto _LL4B;}_LL4B: if((int)_tmp57 == 3){goto _LL4C;}else{goto _LL4D;}_LL4D: goto
_LL4E;_LL4A: sc=(void*)3;goto _LL48;_LL4C: sc=(void*)0;goto _LL48;_LL4E: Cyc_Parse_err(
_tag_arr("bad storage class on function",sizeof(unsigned char),30),loc);goto
_LL48;_LL48:;}}{struct Cyc_Core_Opt*_tmp59;void*_tmp5A;struct _tuple4 _tmp58=Cyc_Parse_collapse_type_specifiers(
tss,loc);_LL51: _tmp5A=_tmp58.f1;goto _LL50;_LL50: _tmp59=_tmp58.f2;goto _LL4F;_LL4F: {
struct Cyc_List_List*_tmp5C;struct Cyc_List_List*_tmp5D;void*_tmp5E;struct Cyc_Absyn_Tqual
_tmp5F;struct _tuple5 _tmp5B=Cyc_Parse_apply_tms(tq,_tmp5A,atts,d->tms);_LL56:
_tmp5F=_tmp5B.f1;goto _LL55;_LL55: _tmp5E=_tmp5B.f2;goto _LL54;_LL54: _tmp5D=_tmp5B.f3;
goto _LL53;_LL53: _tmp5C=_tmp5B.f4;goto _LL52;_LL52: if(_tmp59 != 0){({void*_tmp60[0]={};
Cyc_Tcutil_warn(loc,_tag_arr("nested type declaration within function prototype",
sizeof(unsigned char),50),_tag_arr(_tmp60,sizeof(void*),0));});}if(_tmp5D != 0){({
void*_tmp61[0]={};Cyc_Tcutil_warn(loc,_tag_arr("bad type params, ignoring",
sizeof(unsigned char),26),_tag_arr(_tmp61,sizeof(void*),0));});}{void*_tmp62=
_tmp5E;struct Cyc_Absyn_FnInfo _tmp63;struct Cyc_List_List*_tmp64;struct Cyc_List_List*
_tmp65;struct Cyc_Absyn_VarargInfo*_tmp66;int _tmp67;struct Cyc_List_List*_tmp68;
void*_tmp69;struct Cyc_Core_Opt*_tmp6A;struct Cyc_List_List*_tmp6B;_LL58: if((
unsigned int)_tmp62 > 3?*((int*)_tmp62)== 8: 0){_LL5C: _tmp63=((struct Cyc_Absyn_FnType_struct*)
_tmp62)->f1;_LL64: _tmp6B=_tmp63.tvars;goto _LL63;_LL63: _tmp6A=_tmp63.effect;goto
_LL62;_LL62: _tmp69=(void*)_tmp63.ret_typ;goto _LL61;_LL61: _tmp68=_tmp63.args;goto
_LL60;_LL60: _tmp67=_tmp63.c_varargs;goto _LL5F;_LL5F: _tmp66=_tmp63.cyc_varargs;
goto _LL5E;_LL5E: _tmp65=_tmp63.rgn_po;goto _LL5D;_LL5D: _tmp64=_tmp63.attributes;
goto _LL59;}else{goto _LL5A;}_LL5A: goto _LL5B;_LL59: {struct Cyc_List_List*_tmp6C=0;{
struct Cyc_List_List*_tmp6D=_tmp68;for(0;_tmp6D != 0;_tmp6D=_tmp6D->tl){struct
_tuple2 _tmp6F;void*_tmp70;struct Cyc_Absyn_Tqual _tmp71;struct Cyc_Core_Opt*_tmp72;
struct _tuple2*_tmp6E=(struct _tuple2*)_tmp6D->hd;_tmp6F=*_tmp6E;_LL68: _tmp72=
_tmp6F.f1;goto _LL67;_LL67: _tmp71=_tmp6F.f2;goto _LL66;_LL66: _tmp70=_tmp6F.f3;goto
_LL65;_LL65: if(_tmp72 == 0){Cyc_Parse_err(_tag_arr("missing argument variable in function prototype",
sizeof(unsigned char),48),loc);_tmp6C=({struct Cyc_List_List*_tmp73=_cycalloc(
sizeof(*_tmp73));_tmp73->hd=({struct _tuple9*_tmp74=_cycalloc(sizeof(*_tmp74));
_tmp74->f1=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"?",sizeof(
unsigned char),2);_tmp74->f2=_tmp71;_tmp74->f3=_tmp70;_tmp74;});_tmp73->tl=
_tmp6C;_tmp73;});}else{_tmp6C=({struct Cyc_List_List*_tmp76=_cycalloc(sizeof(*
_tmp76));_tmp76->hd=({struct _tuple9*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->f1=(
struct _tagged_arr*)_tmp72->v;_tmp77->f2=_tmp71;_tmp77->f3=_tmp70;_tmp77;});
_tmp76->tl=_tmp6C;_tmp76;});}}}return({struct Cyc_Absyn_Fndecl*_tmp78=_cycalloc(
sizeof(*_tmp78));_tmp78->sc=(void*)sc;_tmp78->name=d->id;_tmp78->tvs=_tmp6B;
_tmp78->is_inline=is_inline;_tmp78->effect=_tmp6A;_tmp78->ret_type=(void*)_tmp69;
_tmp78->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp6C);_tmp78->c_varargs=_tmp67;_tmp78->cyc_varargs=_tmp66;_tmp78->rgn_po=
_tmp65;_tmp78->body=body;_tmp78->cached_typ=0;_tmp78->param_vardecls=0;_tmp78->attributes=
Cyc_List_append(_tmp64,_tmp5C);_tmp78;});}_LL5B:({void*_tmp79[0]={};((int(*)(
struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(
loc,_tag_arr("declarator is not a function prototype",sizeof(unsigned char),39),
_tag_arr(_tmp79,sizeof(void*),0));});_LL57:;}}}}}static unsigned char _tmp7A[52]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={_tmp7A,_tmp7A,_tmp7A + 52};static
unsigned char _tmp7B[63]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={_tmp7B,_tmp7B,_tmp7B + 63};static
unsigned char _tmp7C[50]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={_tmp7C,_tmp7C,_tmp7C + 50};static
unsigned char _tmp7D[60]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={_tmp7D,_tmp7D,_tmp7D + 60};static struct
_tuple4 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;
void*t=(void*)0;void*sz=(void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*
last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp7E=(void*)ts->hd;struct Cyc_Position_Segment*
_tmp7F;void*_tmp80;struct Cyc_Position_Segment*_tmp81;struct Cyc_Position_Segment*
_tmp82;struct Cyc_Position_Segment*_tmp83;struct Cyc_Position_Segment*_tmp84;
struct Cyc_Absyn_Decl*_tmp85;_LL6A: if(*((int*)_tmp7E)== 4){_LL77: _tmp80=(void*)((
struct Cyc_Parse_Type_spec_struct*)_tmp7E)->f1;goto _LL76;_LL76: _tmp7F=((struct Cyc_Parse_Type_spec_struct*)
_tmp7E)->f2;goto _LL6B;}else{goto _LL6C;}_LL6C: if(*((int*)_tmp7E)== 0){_LL78:
_tmp81=((struct Cyc_Parse_Signed_spec_struct*)_tmp7E)->f1;goto _LL6D;}else{goto
_LL6E;}_LL6E: if(*((int*)_tmp7E)== 1){_LL79: _tmp82=((struct Cyc_Parse_Unsigned_spec_struct*)
_tmp7E)->f1;goto _LL6F;}else{goto _LL70;}_LL70: if(*((int*)_tmp7E)== 2){_LL7A:
_tmp83=((struct Cyc_Parse_Short_spec_struct*)_tmp7E)->f1;goto _LL71;}else{goto
_LL72;}_LL72: if(*((int*)_tmp7E)== 3){_LL7B: _tmp84=((struct Cyc_Parse_Long_spec_struct*)
_tmp7E)->f1;goto _LL73;}else{goto _LL74;}_LL74: if(*((int*)_tmp7E)== 5){_LL7C:
_tmp85=((struct Cyc_Parse_Decl_spec_struct*)_tmp7E)->f1;goto _LL75;}else{goto _LL69;}
_LL6B: if(seen_type){Cyc_Parse_err(Cyc_Parse_msg1,_tmp7F);}last_loc=_tmp7F;
seen_type=1;t=_tmp80;goto _LL69;_LL6D: if(seen_sign){Cyc_Parse_err(Cyc_Parse_msg4,
_tmp81);}if(seen_type){Cyc_Parse_err(_tag_arr("signed qualifier must come before type",
sizeof(unsigned char),39),_tmp81);}last_loc=_tmp81;seen_sign=1;sgn=(void*)0;goto
_LL69;_LL6F: if(seen_sign){Cyc_Parse_err(Cyc_Parse_msg4,_tmp82);}if(seen_type){
Cyc_Parse_err(_tag_arr("signed qualifier must come before type",sizeof(
unsigned char),39),_tmp82);}last_loc=_tmp82;seen_sign=1;sgn=(void*)1;goto _LL69;
_LL71: if(seen_size){Cyc_Parse_err(_tag_arr("integral size may appear only once within a type specifier",
sizeof(unsigned char),59),_tmp83);}if(seen_type){Cyc_Parse_err(_tag_arr("short modifier must come before base type",
sizeof(unsigned char),42),_tmp83);}last_loc=_tmp83;seen_size=1;sz=(void*)1;goto
_LL69;_LL73: if(seen_type){Cyc_Parse_err(_tag_arr("long modifier must come before base type",
sizeof(unsigned char),41),_tmp84);}if(seen_size){void*_tmp86=sz;_LL7E: if((int)
_tmp86 == 2){goto _LL7F;}else{goto _LL80;}_LL80: goto _LL81;_LL7F: sz=(void*)3;goto
_LL7D;_LL81: Cyc_Parse_err(_tag_arr("extra long in type specifier",sizeof(
unsigned char),29),_tmp84);goto _LL7D;_LL7D:;}last_loc=_tmp84;seen_size=1;goto
_LL69;_LL75: last_loc=_tmp85->loc;if(declopt == 0? ! seen_type: 0){seen_type=1;{void*
_tmp87=(void*)_tmp85->r;struct Cyc_Absyn_Aggrdecl*_tmp88;struct Cyc_Absyn_Tuniondecl*
_tmp89;struct Cyc_Absyn_Enumdecl*_tmp8A;_LL83: if(*((int*)_tmp87)== 4){_LL8B:
_tmp88=((struct Cyc_Absyn_Aggr_d_struct*)_tmp87)->f1;goto _LL84;}else{goto _LL85;}
_LL85: if(*((int*)_tmp87)== 5){_LL8C: _tmp89=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp87)->f1;goto _LL86;}else{goto _LL87;}_LL87: if(*((int*)_tmp87)== 6){_LL8D:
_tmp8A=((struct Cyc_Absyn_Enum_d_struct*)_tmp87)->f1;goto _LL88;}else{goto _LL89;}
_LL89: goto _LL8A;_LL84: {struct Cyc_List_List*_tmp8B=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmp88->tvs));t=(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({
struct Cyc_Absyn_AggrType_struct _tmp8D;_tmp8D.tag=10;_tmp8D.f1=({struct Cyc_Absyn_AggrInfo
_tmp8E;_tmp8E.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp90;_tmp90.tag=0;_tmp90.f1=(void*)((void*)_tmp88->kind);_tmp90.f2=_tmp88->name;
_tmp90;});_tmp8F;}));_tmp8E.targs=_tmp8B;_tmp8E;});_tmp8D;});_tmp8C;});if(_tmp88->fields
!= 0){declopt=({struct Cyc_Core_Opt*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->v=
_tmp85;_tmp91;});}goto _LL82;}_LL86: {struct Cyc_List_List*_tmp92=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmp89->tvs));t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[0]=({
struct Cyc_Absyn_TunionType_struct _tmp94;_tmp94.tag=2;_tmp94.f1=({struct Cyc_Absyn_TunionInfo
_tmp95;_tmp95.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp97;_tmp97.tag=1;_tmp97.f1=({struct Cyc_Absyn_Tuniondecl**_tmp98=_cycalloc(
sizeof(*_tmp98));_tmp98[0]=_tmp89;_tmp98;});_tmp97;});_tmp96;}));_tmp95.targs=
_tmp92;_tmp95.rgn=(void*)((void*)2);_tmp95;});_tmp94;});_tmp93;});if(_tmp89->fields
!= 0){declopt=({struct Cyc_Core_Opt*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->v=
_tmp85;_tmp99;});}goto _LL82;}_LL88: t=(void*)({struct Cyc_Absyn_EnumType_struct*
_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_Absyn_EnumType_struct
_tmp9B;_tmp9B.tag=12;_tmp9B.f1=_tmp8A->name;_tmp9B.f2=0;_tmp9B;});_tmp9A;});
declopt=({struct Cyc_Core_Opt*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->v=_tmp85;
_tmp9C;});goto _LL82;_LL8A:({void*_tmp9D[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp85->loc,
_tag_arr("bad declaration within type specifier",sizeof(unsigned char),38),
_tag_arr(_tmp9D,sizeof(void*),0));});_LL82:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,
_tmp85->loc);}goto _LL69;_LL69:;}if(! seen_type){if(! seen_sign? ! seen_size: 0){({
void*_tmp9E[0]={};Cyc_Tcutil_warn(last_loc,_tag_arr("missing type within specifier",
sizeof(unsigned char),30),_tag_arr(_tmp9E,sizeof(void*),0));});}t=(void*)({
struct Cyc_Absyn_IntType_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({
struct Cyc_Absyn_IntType_struct _tmpA0;_tmpA0.tag=5;_tmpA0.f1=(void*)sgn;_tmpA0.f2=(
void*)sz;_tmpA0;});_tmp9F;});}else{if(seen_sign){void*_tmpA1=t;void*_tmpA2;_LL8F:
if((unsigned int)_tmpA1 > 3?*((int*)_tmpA1)== 5: 0){_LL93: _tmpA2=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmpA1)->f2;goto _LL90;}else{goto _LL91;}_LL91: goto _LL92;
_LL90: t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));
_tmpA3[0]=({struct Cyc_Absyn_IntType_struct _tmpA4;_tmpA4.tag=5;_tmpA4.f1=(void*)
sgn;_tmpA4.f2=(void*)_tmpA2;_tmpA4;});_tmpA3;});goto _LL8E;_LL92: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type",sizeof(unsigned char),40),
last_loc);goto _LL8E;_LL8E:;}if(seen_size){void*_tmpA5=t;void*_tmpA6;_LL95: if((
unsigned int)_tmpA5 > 3?*((int*)_tmpA5)== 5: 0){_LL9B: _tmpA6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpA5)->f1;goto _LL96;}else{goto _LL97;}_LL97: if((unsigned int)_tmpA5 > 3?*((int*)
_tmpA5)== 6: 0){goto _LL98;}else{goto _LL99;}_LL99: goto _LL9A;_LL96: t=(void*)({
struct Cyc_Absyn_IntType_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7[0]=({
struct Cyc_Absyn_IntType_struct _tmpA8;_tmpA8.tag=5;_tmpA8.f1=(void*)_tmpA6;_tmpA8.f2=(
void*)sz;_tmpA8;});_tmpA7;});goto _LL94;_LL98: t=(void*)({struct Cyc_Absyn_DoubleType_struct*
_tmpA9=_cycalloc_atomic(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpAA;_tmpAA.tag=6;_tmpAA.f1=1;_tmpAA;});_tmpA9;});goto _LL94;_LL9A: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type",sizeof(unsigned char),36),
last_loc);goto _LL94;_LL94:;}}return({struct _tuple4 _tmpAB;_tmpAB.f1=t;_tmpAB.f2=
declopt;_tmpAB;});}static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0){
return 0;}{struct Cyc_Parse_Declarator*_tmpAC=(struct Cyc_Parse_Declarator*)ds->hd;
struct _tuple1*_tmpAD=_tmpAC->id;struct Cyc_List_List*_tmpAF;struct Cyc_List_List*
_tmpB0;void*_tmpB1;struct Cyc_Absyn_Tqual _tmpB2;struct _tuple5 _tmpAE=Cyc_Parse_apply_tms(
tq,t,shared_atts,_tmpAC->tms);_LLA0: _tmpB2=_tmpAE.f1;goto _LL9F;_LL9F: _tmpB1=
_tmpAE.f2;goto _LL9E;_LL9E: _tmpB0=_tmpAE.f3;goto _LL9D;_LL9D: _tmpAF=_tmpAE.f4;goto
_LL9C;_LL9C: return({struct Cyc_List_List*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->hd=({
struct _tuple6*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->f1=_tmpAD;_tmpB4->f2=
_tmpB2;_tmpB4->f3=_tmpB1;_tmpB4->f4=_tmpB0;_tmpB4->f5=_tmpAF;_tmpB4;});_tmpB3->tl=
Cyc_Parse_apply_tmss(_tmpB2,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts);_tmpB3;});}}
static struct _tuple5 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0){return({struct _tuple5 _tmpB5;_tmpB5.f1=
tq;_tmpB5.f2=t;_tmpB5.f3=0;_tmpB5.f4=atts;_tmpB5;});}{void*_tmpB6=(void*)tms->hd;
struct Cyc_Absyn_Exp*_tmpB7;void*_tmpB8;struct Cyc_Position_Segment*_tmpB9;struct
Cyc_List_List*_tmpBA;struct Cyc_Absyn_Tqual _tmpBB;void*_tmpBC;void*_tmpBD;struct
Cyc_List_List*_tmpBE;struct Cyc_Position_Segment*_tmpBF;_LLA2: if((int)_tmpB6 == 0){
goto _LLA3;}else{goto _LLA4;}_LLA4: if((unsigned int)_tmpB6 > 1?*((int*)_tmpB6)== 0:
0){_LLAE: _tmpB7=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpB6)->f1;goto _LLA5;}
else{goto _LLA6;}_LLA6: if((unsigned int)_tmpB6 > 1?*((int*)_tmpB6)== 2: 0){_LLAF:
_tmpB8=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmpB6)->f1;goto _LLA7;}
else{goto _LLA8;}_LLA8: if((unsigned int)_tmpB6 > 1?*((int*)_tmpB6)== 3: 0){_LLB1:
_tmpBA=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpB6)->f1;goto _LLB0;_LLB0:
_tmpB9=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpB6)->f2;goto _LLA9;}else{goto
_LLAA;}_LLAA: if((unsigned int)_tmpB6 > 1?*((int*)_tmpB6)== 1: 0){_LLB4: _tmpBD=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpB6)->f1;goto _LLB3;_LLB3: _tmpBC=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpB6)->f2;goto _LLB2;_LLB2: _tmpBB=((
struct Cyc_Absyn_Pointer_mod_struct*)_tmpB6)->f3;goto _LLAB;}else{goto _LLAC;}_LLAC:
if((unsigned int)_tmpB6 > 1?*((int*)_tmpB6)== 4: 0){_LLB6: _tmpBF=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpB6)->f1;goto _LLB5;_LLB5: _tmpBE=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpB6)->f2;goto _LLAD;}else{goto _LLA1;}_LLA3: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),(
void*)({struct Cyc_Absyn_ArrayType_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));
_tmpC0[0]=({struct Cyc_Absyn_ArrayType_struct _tmpC1;_tmpC1.tag=7;_tmpC1.f1=(void*)
t;_tmpC1.f2=tq;_tmpC1.f3=0;_tmpC1;});_tmpC0;}),atts,tms->tl);_LLA5: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),(void*)({struct Cyc_Absyn_ArrayType_struct*_tmpC2=
_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_ArrayType_struct _tmpC3;
_tmpC3.tag=7;_tmpC3.f1=(void*)t;_tmpC3.f2=tq;_tmpC3.f3=(struct Cyc_Absyn_Exp*)
_tmpB7;_tmpC3;});_tmpC2;}),atts,tms->tl);_LLA7: {void*_tmpC4=_tmpB8;struct Cyc_List_List*
_tmpC5;struct Cyc_Core_Opt*_tmpC6;struct Cyc_Absyn_VarargInfo*_tmpC7;int _tmpC8;
struct Cyc_List_List*_tmpC9;struct Cyc_Position_Segment*_tmpCA;_LLB8: if(*((int*)
_tmpC4)== 1){_LLC0: _tmpC9=((struct Cyc_Absyn_WithTypes_struct*)_tmpC4)->f1;goto
_LLBF;_LLBF: _tmpC8=((struct Cyc_Absyn_WithTypes_struct*)_tmpC4)->f2;goto _LLBE;
_LLBE: _tmpC7=((struct Cyc_Absyn_WithTypes_struct*)_tmpC4)->f3;goto _LLBD;_LLBD:
_tmpC6=((struct Cyc_Absyn_WithTypes_struct*)_tmpC4)->f4;goto _LLBC;_LLBC: _tmpC5=((
struct Cyc_Absyn_WithTypes_struct*)_tmpC4)->f5;goto _LLB9;}else{goto _LLBA;}_LLBA:
if(*((int*)_tmpC4)== 0){_LLC1: _tmpCA=((struct Cyc_Absyn_NoTypes_struct*)_tmpC4)->f2;
goto _LLBB;}else{goto _LLB7;}_LLB9: {struct Cyc_List_List*typvars=0;struct Cyc_List_List*
fn_atts=0;struct Cyc_List_List*new_atts=0;{struct Cyc_List_List*as=atts;for(0;as != 
0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd)){fn_atts=({struct Cyc_List_List*
_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->hd=(void*)((void*)as->hd);_tmpCB->tl=
fn_atts;_tmpCB;});}else{new_atts=({struct Cyc_List_List*_tmpCC=_cycalloc(sizeof(*
_tmpCC));_tmpCC->hd=(void*)((void*)as->hd);_tmpCC->tl=new_atts;_tmpCC;});}}}if(
tms->tl != 0){void*_tmpCD=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;
struct Cyc_List_List*_tmpCE;_LLC3: if((unsigned int)_tmpCD > 1?*((int*)_tmpCD)== 3:
0){_LLC7: _tmpCE=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCD)->f1;goto _LLC4;}
else{goto _LLC5;}_LLC5: goto _LLC6;_LLC4: typvars=_tmpCE;tms=tms->tl;goto _LLC2;_LLC6:
goto _LLC2;_LLC2:;}if(((((! _tmpC8? _tmpC7 == 0: 0)? _tmpC9 != 0: 0)? _tmpC9->tl == 0: 0)?(*((
struct _tuple2*)_tmpC9->hd)).f1 == 0: 0)?(*((struct _tuple2*)_tmpC9->hd)).f3 == (void*)
0: 0){_tmpC9=0;}t=Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpC9);return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),Cyc_Absyn_function_typ(typvars,_tmpC6,t,_tmpC9,_tmpC8,
_tmpC7,_tmpC5,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}
_LLBB:({void*_tmpCF[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmpCA,_tag_arr("function declaration without parameter types",
sizeof(unsigned char),45),_tag_arr(_tmpCF,sizeof(void*),0));});_LLB7:;}_LLA9: if(
tms->tl == 0){return({struct _tuple5 _tmpD0;_tmpD0.f1=tq;_tmpD0.f2=t;_tmpD0.f3=
_tmpBA;_tmpD0.f4=atts;_tmpD0;});}({void*_tmpD1[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmpB9,_tag_arr("type parameters must appear before function arguments in declarator",
sizeof(unsigned char),68),_tag_arr(_tmpD1,sizeof(void*),0));});_LLAB: {void*
_tmpD2=_tmpBD;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD4;_LLC9: if((
unsigned int)_tmpD2 > 1?*((int*)_tmpD2)== 0: 0){_LLCF: _tmpD3=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmpD2)->f1;goto _LLCA;}else{goto _LLCB;}_LLCB: if((unsigned int)_tmpD2 > 1?*((int*)
_tmpD2)== 1: 0){_LLD0: _tmpD4=((struct Cyc_Absyn_Nullable_ps_struct*)_tmpD2)->f1;
goto _LLCC;}else{goto _LLCD;}_LLCD: if((int)_tmpD2 == 0){goto _LLCE;}else{goto _LLC8;}
_LLCA: return Cyc_Parse_apply_tms(_tmpBB,Cyc_Absyn_atb_typ(t,_tmpBC,tq,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({
struct Cyc_Absyn_Upper_b_struct _tmpD6;_tmpD6.tag=0;_tmpD6.f1=_tmpD3;_tmpD6;});
_tmpD5;})),atts,tms->tl);_LLCC: return Cyc_Parse_apply_tms(_tmpBB,Cyc_Absyn_starb_typ(
t,_tmpBC,tq,(void*)({struct Cyc_Absyn_Upper_b_struct*_tmpD7=_cycalloc(sizeof(*
_tmpD7));_tmpD7[0]=({struct Cyc_Absyn_Upper_b_struct _tmpD8;_tmpD8.tag=0;_tmpD8.f1=
_tmpD4;_tmpD8;});_tmpD7;})),atts,tms->tl);_LLCE: return Cyc_Parse_apply_tms(_tmpBB,
Cyc_Absyn_tagged_typ(t,_tmpBC,tq),atts,tms->tl);_LLC8:;}_LLAD: return Cyc_Parse_apply_tms(
tq,t,Cyc_List_append(atts,_tmpBE),tms->tl);_LLA1:;}}void*Cyc_Parse_speclist2typ(
struct Cyc_List_List*tss,struct Cyc_Position_Segment*loc){struct Cyc_Core_Opt*
_tmpDA;void*_tmpDB;struct _tuple4 _tmpD9=Cyc_Parse_collapse_type_specifiers(tss,
loc);_LLD3: _tmpDB=_tmpD9.f1;goto _LLD2;_LLD2: _tmpDA=_tmpD9.f2;goto _LLD1;_LLD1: if(
_tmpDA != 0){({void*_tmpDC[0]={};Cyc_Tcutil_warn(loc,_tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof(unsigned char),54),_tag_arr(_tmpDC,sizeof(void*),0));});}return _tmpDB;}
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple6*t){struct _tuple6 _tmpDE;struct Cyc_List_List*_tmpDF;struct Cyc_List_List*
_tmpE0;void*_tmpE1;struct Cyc_Absyn_Tqual _tmpE2;struct _tuple1*_tmpE3;struct
_tuple6*_tmpDD=t;_tmpDE=*_tmpDD;_LLD9: _tmpE3=_tmpDE.f1;goto _LLD8;_LLD8: _tmpE2=
_tmpDE.f2;goto _LLD7;_LLD7: _tmpE1=_tmpDE.f3;goto _LLD6;_LLD6: _tmpE0=_tmpDE.f4;goto
_LLD5;_LLD5: _tmpDF=_tmpDE.f5;goto _LLD4;_LLD4: Cyc_Lex_register_typedef(_tmpE3);
if(_tmpDF != 0){Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmpE5;_tmpE5.tag=0;_tmpE5.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmpDF->hd);{void*_tmpE4[1]={& _tmpE5};Cyc_Std_aprintf(_tag_arr("bad attribute %s within typedef",
sizeof(unsigned char),32),_tag_arr(_tmpE4,sizeof(void*),1));}}),loc);}return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Typedef_d_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));
_tmpE6[0]=({struct Cyc_Absyn_Typedef_d_struct _tmpE7;_tmpE7.tag=7;_tmpE7.f1=({
struct Cyc_Absyn_Typedefdecl*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->name=
_tmpE3;_tmpE8->tvs=_tmpE0;_tmpE8->defn=(void*)_tmpE1;_tmpE8;});_tmpE7;});_tmpE6;}),
loc);}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,
struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpEA;_tmpEA.tag=12;_tmpEA.f1=d;_tmpEA.f2=s;_tmpEA;});_tmpE9;}),Cyc_Position_segment_join(
d->loc,s->loc));}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*loc){struct Cyc_Parse_Declaration_spec
_tmpEC;struct Cyc_List_List*_tmpED;struct Cyc_List_List*_tmpEE;struct Cyc_Absyn_Tqual
_tmpEF;struct Cyc_Parse_Declaration_spec*_tmpEB=ds;_tmpEC=*_tmpEB;_LLDD: _tmpEF=
_tmpEC.tq;goto _LLDC;_LLDC: _tmpEE=_tmpEC.type_specs;goto _LLDB;_LLDB: _tmpED=_tmpEC.attributes;
goto _LLDA;_LLDA: if(ds->is_inline){Cyc_Parse_err(_tag_arr("inline is allowed only on function definitions",
sizeof(unsigned char),47),loc);}if(_tmpEE == 0){Cyc_Parse_err(_tag_arr("missing type specifiers in declaration",
sizeof(unsigned char),39),loc);return 0;}{void*s=(void*)2;int istypedef=0;if(ds->sc
!= 0){void*_tmpF0=(void*)((struct Cyc_Core_Opt*)_check_null(ds->sc))->v;_LLDF: if((
int)_tmpF0 == 0){goto _LLE0;}else{goto _LLE1;}_LLE1: if((int)_tmpF0 == 1){goto _LLE2;}
else{goto _LLE3;}_LLE3: if((int)_tmpF0 == 2){goto _LLE4;}else{goto _LLE5;}_LLE5: if((
int)_tmpF0 == 3){goto _LLE6;}else{goto _LLE7;}_LLE7: if((int)_tmpF0 == 4){goto _LLE8;}
else{goto _LLE9;}_LLE9: if((int)_tmpF0 == 5){goto _LLEA;}else{goto _LLEB;}_LLEB: if((
int)_tmpF0 == 6){goto _LLEC;}else{goto _LLDE;}_LLE0: istypedef=1;goto _LLDE;_LLE2: s=(
void*)3;goto _LLDE;_LLE4: s=(void*)4;goto _LLDE;_LLE6: s=(void*)0;goto _LLDE;_LLE8: s=(
void*)2;goto _LLDE;_LLEA: s=(void*)2;goto _LLDE;_LLEC: s=(void*)1;goto _LLDE;_LLDE:;}{
struct Cyc_List_List*_tmpF2;struct Cyc_List_List*_tmpF3;struct _tuple0 _tmpF1=((
struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(ids);_LLEF: _tmpF3=_tmpF1.f1;
goto _LLEE;_LLEE: _tmpF2=_tmpF1.f2;goto _LLED;_LLED: {int exps_empty=1;{struct Cyc_List_List*
es=_tmpF2;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;break;}}}{struct _tuple4 _tmpF4=Cyc_Parse_collapse_type_specifiers(
_tmpEE,loc);if(_tmpF3 == 0){struct Cyc_Core_Opt*_tmpF6;void*_tmpF7;struct _tuple4
_tmpF5=_tmpF4;_LLF2: _tmpF7=_tmpF5.f1;goto _LLF1;_LLF1: _tmpF6=_tmpF5.f2;goto _LLF0;
_LLF0: if(_tmpF6 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmpF6->v;{
void*_tmpF8=(void*)d->r;struct Cyc_Absyn_Enumdecl*_tmpF9;struct Cyc_Absyn_Aggrdecl*
_tmpFA;struct Cyc_Absyn_Tuniondecl*_tmpFB;_LLF4: if(*((int*)_tmpF8)== 6){_LLFC:
_tmpF9=((struct Cyc_Absyn_Enum_d_struct*)_tmpF8)->f1;goto _LLF5;}else{goto _LLF6;}
_LLF6: if(*((int*)_tmpF8)== 4){_LLFD: _tmpFA=((struct Cyc_Absyn_Aggr_d_struct*)
_tmpF8)->f1;goto _LLF7;}else{goto _LLF8;}_LLF8: if(*((int*)_tmpF8)== 5){_LLFE:
_tmpFB=((struct Cyc_Absyn_Tunion_d_struct*)_tmpF8)->f1;goto _LLF9;}else{goto _LLFA;}
_LLFA: goto _LLFB;_LLF5:(void*)(_tmpF9->sc=(void*)s);if(_tmpED != 0){Cyc_Parse_err(
_tag_arr("bad attributes on enum",sizeof(unsigned char),23),loc);}goto _LLF3;
_LLF7:(void*)(_tmpFA->sc=(void*)s);_tmpFA->attributes=_tmpED;goto _LLF3;_LLF9:(
void*)(_tmpFB->sc=(void*)s);if(_tmpED != 0){Cyc_Parse_err(_tag_arr("bad attributes on tunion",
sizeof(unsigned char),25),loc);}goto _LLF3;_LLFB: Cyc_Parse_err(_tag_arr("bad declaration",
sizeof(unsigned char),16),loc);return 0;_LLF3:;}return({struct Cyc_List_List*
_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->hd=d;_tmpFC->tl=0;_tmpFC;});}else{void*
_tmpFD=_tmpF7;struct Cyc_Absyn_AggrInfo _tmpFE;struct Cyc_List_List*_tmpFF;void*
_tmp100;struct _tuple1*_tmp101;void*_tmp102;struct Cyc_Absyn_TunionInfo _tmp103;
void*_tmp104;struct Cyc_Absyn_Tuniondecl**_tmp105;struct Cyc_Absyn_TunionInfo
_tmp106;struct Cyc_List_List*_tmp107;void*_tmp108;struct Cyc_Absyn_UnknownTunionInfo
_tmp109;int _tmp10A;struct _tuple1*_tmp10B;struct _tuple1*_tmp10C;struct Cyc_List_List*
_tmp10D;_LL100: if((unsigned int)_tmpFD > 3?*((int*)_tmpFD)== 10: 0){_LL10C: _tmpFE=((
struct Cyc_Absyn_AggrType_struct*)_tmpFD)->f1;_LL10E: _tmp100=(void*)_tmpFE.aggr_info;
if(*((int*)_tmp100)== 0){_LL110: _tmp102=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp100)->f1;goto _LL10F;_LL10F: _tmp101=((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp100)->f2;goto _LL10D;}else{goto _LL102;}_LL10D: _tmpFF=_tmpFE.targs;goto _LL101;}
else{goto _LL102;}_LL102: if((unsigned int)_tmpFD > 3?*((int*)_tmpFD)== 2: 0){_LL111:
_tmp103=((struct Cyc_Absyn_TunionType_struct*)_tmpFD)->f1;_LL112: _tmp104=(void*)
_tmp103.tunion_info;if(*((int*)_tmp104)== 1){_LL113: _tmp105=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp104)->f1;goto _LL103;}else{goto _LL104;}}else{goto _LL104;}_LL104: if((
unsigned int)_tmpFD > 3?*((int*)_tmpFD)== 2: 0){_LL114: _tmp106=((struct Cyc_Absyn_TunionType_struct*)
_tmpFD)->f1;_LL116: _tmp108=(void*)_tmp106.tunion_info;if(*((int*)_tmp108)== 0){
_LL117: _tmp109=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp108)->f1;_LL119:
_tmp10B=_tmp109.name;goto _LL118;_LL118: _tmp10A=_tmp109.is_xtunion;goto _LL115;}
else{goto _LL106;}_LL115: _tmp107=_tmp106.targs;goto _LL105;}else{goto _LL106;}
_LL106: if((unsigned int)_tmpFD > 3?*((int*)_tmpFD)== 12: 0){_LL11A: _tmp10C=((
struct Cyc_Absyn_EnumType_struct*)_tmpFD)->f1;goto _LL107;}else{goto _LL108;}_LL108:
if((unsigned int)_tmpFD > 3?*((int*)_tmpFD)== 13: 0){_LL11B: _tmp10D=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmpFD)->f1;goto _LL109;}else{goto _LL10A;}_LL10A: goto _LL10B;_LL101: {struct Cyc_List_List*
_tmp10E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmpFF);struct Cyc_Absyn_Aggrdecl*_tmp10F=({struct Cyc_Absyn_Aggrdecl*_tmp113=
_cycalloc(sizeof(*_tmp113));_tmp113->kind=(void*)_tmp102;_tmp113->sc=(void*)s;
_tmp113->name=_tmp101;_tmp113->tvs=_tmp10E;_tmp113->exist_vars=0;_tmp113->fields=
0;_tmp113->attributes=0;_tmp113;});if(_tmpED != 0){Cyc_Parse_err(_tag_arr("bad attributes on type declaration",
sizeof(unsigned char),35),loc);}return({struct Cyc_List_List*_tmp110=_cycalloc(
sizeof(*_tmp110));_tmp110->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp112;_tmp112.tag=4;_tmp112.f1=_tmp10F;_tmp112;});_tmp111;}),loc);_tmp110->tl=
0;_tmp110;});}_LL103: if(_tmpED != 0){Cyc_Parse_err(_tag_arr("bad attributes on tunion",
sizeof(unsigned char),25),loc);}return({struct Cyc_List_List*_tmp114=_cycalloc(
sizeof(*_tmp114));_tmp114->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp116;_tmp116.tag=5;_tmp116.f1=*_tmp105;_tmp116;});_tmp115;}),loc);_tmp114->tl=
0;_tmp114;});_LL105: {struct Cyc_List_List*_tmp117=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp107);
struct Cyc_Absyn_Decl*_tmp118=Cyc_Absyn_tunion_decl(s,_tmp10B,_tmp117,0,_tmp10A,
loc);if(_tmpED != 0){Cyc_Parse_err(_tag_arr("bad attributes on tunion",sizeof(
unsigned char),25),loc);}return({struct Cyc_List_List*_tmp119=_cycalloc(sizeof(*
_tmp119));_tmp119->hd=_tmp118;_tmp119->tl=0;_tmp119;});}_LL107: {struct Cyc_Absyn_Enumdecl*
_tmp11A=({struct Cyc_Absyn_Enumdecl*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->sc=(
void*)s;_tmp11F->name=_tmp10C;_tmp11F->fields=0;_tmp11F;});if(_tmpED != 0){Cyc_Parse_err(
_tag_arr("bad attributes on enum",sizeof(unsigned char),23),loc);}return({struct
Cyc_List_List*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->hd=({struct Cyc_Absyn_Decl*
_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp11E;_tmp11E.tag=6;_tmp11E.f1=_tmp11A;_tmp11E;});_tmp11D;}));_tmp11C->loc=loc;
_tmp11C;});_tmp11B->tl=0;_tmp11B;});}_LL109: {struct Cyc_Absyn_Enumdecl*_tmp120=({
struct Cyc_Absyn_Enumdecl*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->sc=(void*)
s;_tmp125->name=Cyc_Parse_gensym_enum();_tmp125->fields=({struct Cyc_Core_Opt*
_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->v=_tmp10D;_tmp126;});_tmp125;});if(
_tmpED != 0){Cyc_Parse_err(_tag_arr("bad attributes on enum",sizeof(unsigned char),
23),loc);}return({struct Cyc_List_List*_tmp121=_cycalloc(sizeof(*_tmp121));
_tmp121->hd=({struct Cyc_Absyn_Decl*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->r=(
void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp123=_cycalloc(sizeof(*_tmp123));
_tmp123[0]=({struct Cyc_Absyn_Enum_d_struct _tmp124;_tmp124.tag=6;_tmp124.f1=
_tmp120;_tmp124;});_tmp123;}));_tmp122->loc=loc;_tmp122;});_tmp121->tl=0;_tmp121;});}
_LL10B: Cyc_Parse_err(_tag_arr("missing declarator",sizeof(unsigned char),19),loc);
return 0;_LLFF:;}}else{void*t=_tmpF4.f1;struct Cyc_List_List*_tmp127=Cyc_Parse_apply_tmss(
_tmpEF,t,_tmpF3,_tmpED);if(istypedef){if(! exps_empty){Cyc_Parse_err(_tag_arr("initializer in typedef declaration",
sizeof(unsigned char),35),loc);}{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp127);if(_tmpF4.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(_tmpF4.f2)->v;{
void*_tmp128=(void*)d->r;struct Cyc_Absyn_Aggrdecl*_tmp129;struct Cyc_Absyn_Tuniondecl*
_tmp12A;struct Cyc_Absyn_Enumdecl*_tmp12B;_LL11D: if(*((int*)_tmp128)== 4){_LL125:
_tmp129=((struct Cyc_Absyn_Aggr_d_struct*)_tmp128)->f1;goto _LL11E;}else{goto
_LL11F;}_LL11F: if(*((int*)_tmp128)== 5){_LL126: _tmp12A=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp128)->f1;goto _LL120;}else{goto _LL121;}_LL121: if(*((int*)_tmp128)== 6){_LL127:
_tmp12B=((struct Cyc_Absyn_Enum_d_struct*)_tmp128)->f1;goto _LL122;}else{goto
_LL123;}_LL123: goto _LL124;_LL11E:(void*)(_tmp129->sc=(void*)s);_tmp129->attributes=
_tmpED;_tmpED=0;goto _LL11C;_LL120:(void*)(_tmp12A->sc=(void*)s);goto _LL11C;
_LL122:(void*)(_tmp12B->sc=(void*)s);goto _LL11C;_LL124: Cyc_Parse_err(_tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof(unsigned char),70),loc);goto _LL11C;_LL11C:;}decls=({struct Cyc_List_List*
_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->hd=d;_tmp12C->tl=decls;_tmp12C;});}
if(_tmpED != 0){Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp12E;_tmp12E.tag=0;_tmp12E.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmpED->hd);{void*_tmp12D[1]={& _tmp12E};Cyc_Std_aprintf(_tag_arr("bad attribute %s in typedef",
sizeof(unsigned char),28),_tag_arr(_tmp12D,sizeof(void*),1));}}),loc);}return
decls;}}else{if(_tmpF4.f2 != 0){Cyc_Parse_unimp(_tag_arr("nested type declaration within declarator",
sizeof(unsigned char),42),loc);}{struct Cyc_List_List*decls=0;{struct Cyc_List_List*
_tmp12F=_tmp127;for(0;_tmp12F != 0;(_tmp12F=_tmp12F->tl,_tmpF2=_tmpF2->tl)){
struct _tuple6 _tmp131;struct Cyc_List_List*_tmp132;struct Cyc_List_List*_tmp133;
void*_tmp134;struct Cyc_Absyn_Tqual _tmp135;struct _tuple1*_tmp136;struct _tuple6*
_tmp130=(struct _tuple6*)_tmp12F->hd;_tmp131=*_tmp130;_LL12D: _tmp136=_tmp131.f1;
goto _LL12C;_LL12C: _tmp135=_tmp131.f2;goto _LL12B;_LL12B: _tmp134=_tmp131.f3;goto
_LL12A;_LL12A: _tmp133=_tmp131.f4;goto _LL129;_LL129: _tmp132=_tmp131.f5;goto _LL128;
_LL128: if(_tmp133 != 0){({void*_tmp137[0]={};Cyc_Tcutil_warn(loc,_tag_arr("bad type params, ignoring",
sizeof(unsigned char),26),_tag_arr(_tmp137,sizeof(void*),0));});}if(_tmpF2 == 0){({
void*_tmp138[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Parse_abort)(loc,_tag_arr("unexpected NULL in parse!",
sizeof(unsigned char),26),_tag_arr(_tmp138,sizeof(void*),0));});}{struct Cyc_Absyn_Exp*
_tmp139=(struct Cyc_Absyn_Exp*)_tmpF2->hd;struct Cyc_Absyn_Vardecl*_tmp13A=Cyc_Absyn_new_vardecl(
_tmp136,_tmp134,_tmp139);_tmp13A->tq=_tmp135;(void*)(_tmp13A->sc=(void*)s);
_tmp13A->attributes=_tmp132;{struct Cyc_Absyn_Decl*_tmp13B=({struct Cyc_Absyn_Decl*
_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E[0]=({struct Cyc_Absyn_Var_d_struct
_tmp13F;_tmp13F.tag=0;_tmp13F.f1=_tmp13A;_tmp13F;});_tmp13E;}));_tmp13D->loc=loc;
_tmp13D;});decls=({struct Cyc_List_List*_tmp13C=_cycalloc(sizeof(*_tmp13C));
_tmp13C->hd=_tmp13B;_tmp13C->tl=decls;_tmp13C;});}}}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);}}}}}}}}static void*Cyc_Parse_id_to_kind(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){if(Cyc_Std_strlen(s)== 1){
switch(*((const unsigned char*)_check_unknown_subscript(s,sizeof(unsigned char),0))){
case 'A': _LL12E: return(void*)0;case 'M': _LL12F: return(void*)1;case 'B': _LL130:
return(void*)2;case 'R': _LL131: return(void*)3;case 'E': _LL132: return(void*)4;
default: _LL133: break;}}Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp141;_tmp141.tag=0;_tmp141.f1=(struct _tagged_arr)s;{void*_tmp140[1]={& _tmp141};
Cyc_Std_aprintf(_tag_arr("bad kind: %s",sizeof(unsigned char),13),_tag_arr(
_tmp140,sizeof(void*),1));}}),loc);return(void*)2;}static struct Cyc_List_List*Cyc_Parse_attopt_to_tms(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
if(atts == 0){return tms;}else{return({struct Cyc_List_List*_tmp142=_cycalloc(
sizeof(*_tmp142));_tmp142->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp144;_tmp144.tag=4;_tmp144.f1=loc;_tmp144.f2=atts;_tmp144;});_tmp143;}));
_tmp142->tl=tms;_tmp142;});}}unsigned char Cyc_AbstractDeclarator_tok[27]="\000\000\000\000AbstractDeclarator_tok";
struct Cyc_AbstractDeclarator_tok_struct{unsigned char*tag;struct Cyc_Parse_Abstractdeclarator*
f1;};unsigned char Cyc_AggrFieldDeclListList_tok[30]="\000\000\000\000AggrFieldDeclListList_tok";
struct Cyc_AggrFieldDeclListList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};unsigned char Cyc_AggrFieldDeclList_tok[26]="\000\000\000\000AggrFieldDeclList_tok";
struct Cyc_AggrFieldDeclList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;
};unsigned char Cyc_AttributeList_tok[22]="\000\000\000\000AttributeList_tok";
struct Cyc_AttributeList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};
unsigned char Cyc_Attribute_tok[18]="\000\000\000\000Attribute_tok";struct Cyc_Attribute_tok_struct{
unsigned char*tag;void*f1;};unsigned char Cyc_Bool_tok[13]="\000\000\000\000Bool_tok";
struct Cyc_Bool_tok_struct{unsigned char*tag;int f1;};unsigned char Cyc_Char_tok[13]="\000\000\000\000Char_tok";
struct Cyc_Char_tok_struct{unsigned char*tag;unsigned char f1;};unsigned char Cyc_DeclList_tok[
17]="\000\000\000\000DeclList_tok";struct Cyc_DeclList_tok_struct{unsigned char*
tag;struct Cyc_List_List*f1;};unsigned char Cyc_DeclSpec_tok[17]="\000\000\000\000DeclSpec_tok";
struct Cyc_DeclSpec_tok_struct{unsigned char*tag;struct Cyc_Parse_Declaration_spec*
f1;};unsigned char Cyc_Declarator_tok[19]="\000\000\000\000Declarator_tok";struct
Cyc_Declarator_tok_struct{unsigned char*tag;struct Cyc_Parse_Declarator*f1;};
unsigned char Cyc_DesignatorList_tok[23]="\000\000\000\000DesignatorList_tok";
struct Cyc_DesignatorList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};
unsigned char Cyc_Designator_tok[19]="\000\000\000\000Designator_tok";struct Cyc_Designator_tok_struct{
unsigned char*tag;void*f1;};unsigned char Cyc_EnumfieldList_tok[22]="\000\000\000\000EnumfieldList_tok";
struct Cyc_EnumfieldList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};
unsigned char Cyc_Enumfield_tok[18]="\000\000\000\000Enumfield_tok";struct Cyc_Enumfield_tok_struct{
unsigned char*tag;struct Cyc_Absyn_Enumfield*f1;};unsigned char Cyc_ExpList_tok[16]="\000\000\000\000ExpList_tok";
struct Cyc_ExpList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};
unsigned char Cyc_Exp_tok[12]="\000\000\000\000Exp_tok";struct Cyc_Exp_tok_struct{
unsigned char*tag;struct Cyc_Absyn_Exp*f1;};unsigned char Cyc_FieldPatternList_tok[
25]="\000\000\000\000FieldPatternList_tok";struct Cyc_FieldPatternList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};unsigned char Cyc_FieldPattern_tok[21]="\000\000\000\000FieldPattern_tok";
struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
unsigned char*tag;struct _tuple10*f1;};unsigned char Cyc_FnDecl_tok[15]="\000\000\000\000FnDecl_tok";
struct Cyc_FnDecl_tok_struct{unsigned char*tag;struct Cyc_Absyn_Fndecl*f1;};
unsigned char Cyc_IdList_tok[15]="\000\000\000\000IdList_tok";struct Cyc_IdList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};unsigned char Cyc_InitDeclList_tok[21]="\000\000\000\000InitDeclList_tok";
struct Cyc_InitDeclList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};
unsigned char Cyc_InitDecl_tok[17]="\000\000\000\000InitDecl_tok";struct _tuple11{
struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
unsigned char*tag;struct _tuple11*f1;};unsigned char Cyc_InitializerList_tok[24]="\000\000\000\000InitializerList_tok";
struct Cyc_InitializerList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};
unsigned char Cyc_Int_tok[12]="\000\000\000\000Int_tok";struct _tuple12{void*f1;
int f2;};struct Cyc_Int_tok_struct{unsigned char*tag;struct _tuple12*f1;};
unsigned char Cyc_Kind_tok[13]="\000\000\000\000Kind_tok";struct Cyc_Kind_tok_struct{
unsigned char*tag;void*f1;};unsigned char Cyc_ParamDeclListBool_tok[26]="\000\000\000\000ParamDeclListBool_tok";
struct _tuple13{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct
Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_ParamDeclListBool_tok_struct{
unsigned char*tag;struct _tuple13*f1;};unsigned char Cyc_ParamDeclList_tok[22]="\000\000\000\000ParamDeclList_tok";
struct Cyc_ParamDeclList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};
unsigned char Cyc_ParamDecl_tok[18]="\000\000\000\000ParamDecl_tok";struct Cyc_ParamDecl_tok_struct{
unsigned char*tag;struct _tuple2*f1;};unsigned char Cyc_PatternList_tok[20]="\000\000\000\000PatternList_tok";
struct Cyc_PatternList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};
unsigned char Cyc_Pattern_tok[16]="\000\000\000\000Pattern_tok";struct Cyc_Pattern_tok_struct{
unsigned char*tag;struct Cyc_Absyn_Pat*f1;};unsigned char Cyc_Pointer_Sort_tok[21]="\000\000\000\000Pointer_Sort_tok";
struct Cyc_Pointer_Sort_tok_struct{unsigned char*tag;void*f1;};unsigned char Cyc_Primop_tok[
15]="\000\000\000\000Primop_tok";struct Cyc_Primop_tok_struct{unsigned char*tag;
void*f1;};unsigned char Cyc_Primopopt_tok[18]="\000\000\000\000Primopopt_tok";
struct Cyc_Primopopt_tok_struct{unsigned char*tag;struct Cyc_Core_Opt*f1;};
unsigned char Cyc_QualId_tok[15]="\000\000\000\000QualId_tok";struct Cyc_QualId_tok_struct{
unsigned char*tag;struct _tuple1*f1;};unsigned char Cyc_QualSpecList_tok[21]="\000\000\000\000QualSpecList_tok";
struct _tuple14{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct Cyc_QualSpecList_tok_struct{unsigned char*tag;struct _tuple14*f1;};
unsigned char Cyc_Rgnorder_tok[17]="\000\000\000\000Rgnorder_tok";struct Cyc_Rgnorder_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};unsigned char Cyc_Scope_tok[14]="\000\000\000\000Scope_tok";
struct Cyc_Scope_tok_struct{unsigned char*tag;void*f1;};unsigned char Cyc_Short_tok[
14]="\000\000\000\000Short_tok";struct Cyc_Short_tok_struct{unsigned char*tag;
short f1;};unsigned char Cyc_Stmt_tok[13]="\000\000\000\000Stmt_tok";struct Cyc_Stmt_tok_struct{
unsigned char*tag;struct Cyc_Absyn_Stmt*f1;};unsigned char Cyc_StorageClass_tok[21]="\000\000\000\000StorageClass_tok";
struct Cyc_StorageClass_tok_struct{unsigned char*tag;void*f1;};unsigned char Cyc_String_tok[
15]="\000\000\000\000String_tok";struct Cyc_String_tok_struct{unsigned char*tag;
struct _tagged_arr f1;};unsigned char Cyc_Stringopt_tok[18]="\000\000\000\000Stringopt_tok";
struct Cyc_Stringopt_tok_struct{unsigned char*tag;struct Cyc_Core_Opt*f1;};
unsigned char Cyc_StructOrUnion_tok[22]="\000\000\000\000StructOrUnion_tok";
struct Cyc_StructOrUnion_tok_struct{unsigned char*tag;void*f1;};unsigned char Cyc_SwitchCClauseList_tok[
26]="\000\000\000\000SwitchCClauseList_tok";struct Cyc_SwitchCClauseList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};unsigned char Cyc_SwitchClauseList_tok[
25]="\000\000\000\000SwitchClauseList_tok";struct Cyc_SwitchClauseList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};unsigned char Cyc_TunionFieldList_tok[
24]="\000\000\000\000TunionFieldList_tok";struct Cyc_TunionFieldList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};unsigned char Cyc_TunionField_tok[20]="\000\000\000\000TunionField_tok";
struct Cyc_TunionField_tok_struct{unsigned char*tag;struct Cyc_Absyn_Tunionfield*
f1;};unsigned char Cyc_TypeList_tok[17]="\000\000\000\000TypeList_tok";struct Cyc_TypeList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};unsigned char Cyc_TypeModifierList_tok[
25]="\000\000\000\000TypeModifierList_tok";struct Cyc_TypeModifierList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};unsigned char Cyc_TypeOpt_tok[16]="\000\000\000\000TypeOpt_tok";
struct Cyc_TypeOpt_tok_struct{unsigned char*tag;struct Cyc_Core_Opt*f1;};
unsigned char Cyc_TypeQual_tok[17]="\000\000\000\000TypeQual_tok";struct Cyc_TypeQual_tok_struct{
unsigned char*tag;struct Cyc_Absyn_Tqual f1;};unsigned char Cyc_TypeSpecifier_tok[
22]="\000\000\000\000TypeSpecifier_tok";struct Cyc_TypeSpecifier_tok_struct{
unsigned char*tag;void*f1;};unsigned char Cyc_Type_tok[13]="\000\000\000\000Type_tok";
struct Cyc_Type_tok_struct{unsigned char*tag;void*f1;};unsigned char Cyc_YY1[8]="\000\000\000\000YY1";
struct Cyc_YY1_struct{unsigned char*tag;struct _tuple13*f1;};unsigned char Cyc_YYINITIALSVAL[
18]="\000\000\000\000YYINITIALSVAL";unsigned char Cyc_Okay_tok[13]="\000\000\000\000Okay_tok";
static unsigned char _tmp146[15]="$(sign_t,int)@";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp146,_tmp146,_tmp146 + 15}};struct
_tuple12*Cyc_yyget_Int_tok(void*yy1){struct _tuple12*yyzzz;{void*_tmp147=yy1;
struct _tuple12*_tmp148;_LL136: if(*((void**)_tmp147)== Cyc_Int_tok){_LL13A:
_tmp148=((struct Cyc_Int_tok_struct*)_tmp147)->f1;goto _LL137;}else{goto _LL138;}
_LL138: goto _LL139;_LL137: yyzzz=_tmp148;goto _LL135;_LL139:(int)_throw((void*)& Cyc_yyfail_Int_tok);
_LL135:;}return yyzzz;}static unsigned char _tmp14A[5]="char";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp14A,_tmp14A,_tmp14A + 5}};
unsigned char Cyc_yyget_Char_tok(void*yy1){unsigned char yyzzz;{void*_tmp14B=yy1;
unsigned char _tmp14C;_LL13C: if(*((void**)_tmp14B)== Cyc_Char_tok){_LL140: _tmp14C=((
struct Cyc_Char_tok_struct*)_tmp14B)->f1;goto _LL13D;}else{goto _LL13E;}_LL13E: goto
_LL13F;_LL13D: yyzzz=_tmp14C;goto _LL13B;_LL13F:(int)_throw((void*)& Cyc_yyfail_Char_tok);
_LL13B:;}return yyzzz;}static unsigned char _tmp14E[9]="string_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp14E,_tmp14E,_tmp14E + 9}};struct
_tagged_arr Cyc_yyget_String_tok(void*yy1){struct _tagged_arr yyzzz;{void*_tmp14F=
yy1;struct _tagged_arr _tmp150;_LL142: if(*((void**)_tmp14F)== Cyc_String_tok){
_LL146: _tmp150=((struct Cyc_String_tok_struct*)_tmp14F)->f1;goto _LL143;}else{goto
_LL144;}_LL144: goto _LL145;_LL143: yyzzz=_tmp150;goto _LL141;_LL145:(int)_throw((
void*)& Cyc_yyfail_String_tok);_LL141:;}return yyzzz;}static unsigned char _tmp152[
20]="tunion Pointer_Sort";static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={
Cyc_Core_Failure,{_tmp152,_tmp152,_tmp152 + 20}};void*Cyc_yyget_Pointer_Sort_tok(
void*yy1){void*yyzzz;{void*_tmp153=yy1;void*_tmp154;_LL148: if(*((void**)_tmp153)
== Cyc_Pointer_Sort_tok){_LL14C: _tmp154=(void*)((struct Cyc_Pointer_Sort_tok_struct*)
_tmp153)->f1;goto _LL149;}else{goto _LL14A;}_LL14A: goto _LL14B;_LL149: yyzzz=_tmp154;
goto _LL147;_LL14B:(int)_throw((void*)& Cyc_yyfail_Pointer_Sort_tok);_LL147:;}
return yyzzz;}static unsigned char _tmp156[6]="exp_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp156,_tmp156,_tmp156 + 6}};struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok(void*yy1){struct Cyc_Absyn_Exp*yyzzz;{void*_tmp157=yy1;struct
Cyc_Absyn_Exp*_tmp158;_LL14E: if(*((void**)_tmp157)== Cyc_Exp_tok){_LL152: _tmp158=((
struct Cyc_Exp_tok_struct*)_tmp157)->f1;goto _LL14F;}else{goto _LL150;}_LL150: goto
_LL151;_LL14F: yyzzz=_tmp158;goto _LL14D;_LL151:(int)_throw((void*)& Cyc_yyfail_Exp_tok);
_LL14D:;}return yyzzz;}static unsigned char _tmp15A[14]="list_t<exp_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={Cyc_Core_Failure,{_tmp15A,
_tmp15A,_tmp15A + 14}};struct Cyc_List_List*Cyc_yyget_ExpList_tok(void*yy1){struct
Cyc_List_List*yyzzz;{void*_tmp15B=yy1;struct Cyc_List_List*_tmp15C;_LL154: if(*((
void**)_tmp15B)== Cyc_ExpList_tok){_LL158: _tmp15C=((struct Cyc_ExpList_tok_struct*)
_tmp15B)->f1;goto _LL155;}else{goto _LL156;}_LL156: goto _LL157;_LL155: yyzzz=_tmp15C;
goto _LL153;_LL157:(int)_throw((void*)& Cyc_yyfail_ExpList_tok);_LL153:;}return
yyzzz;}static unsigned char _tmp15E[39]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{
_tmp15E,_tmp15E,_tmp15E + 39}};struct Cyc_List_List*Cyc_yyget_InitializerList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp15F=yy1;struct Cyc_List_List*
_tmp160;_LL15A: if(*((void**)_tmp15F)== Cyc_InitializerList_tok){_LL15E: _tmp160=((
struct Cyc_InitializerList_tok_struct*)_tmp15F)->f1;goto _LL15B;}else{goto _LL15C;}
_LL15C: goto _LL15D;_LL15B: yyzzz=_tmp160;goto _LL159;_LL15D:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);
_LL159:;}return yyzzz;}static unsigned char _tmp162[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={Cyc_Core_Failure,{_tmp162,_tmp162,_tmp162 + 9}};void*Cyc_yyget_Primop_tok(
void*yy1){void*yyzzz;{void*_tmp163=yy1;void*_tmp164;_LL160: if(*((void**)_tmp163)
== Cyc_Primop_tok){_LL164: _tmp164=(void*)((struct Cyc_Primop_tok_struct*)_tmp163)->f1;
goto _LL161;}else{goto _LL162;}_LL162: goto _LL163;_LL161: yyzzz=_tmp164;goto _LL15F;
_LL163:(int)_throw((void*)& Cyc_yyfail_Primop_tok);_LL15F:;}return yyzzz;}static
unsigned char _tmp166[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp166,_tmp166,_tmp166 + 16}};struct
Cyc_Core_Opt*Cyc_yyget_Primopopt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp167=yy1;struct Cyc_Core_Opt*_tmp168;_LL166: if(*((void**)_tmp167)== Cyc_Primopopt_tok){
_LL16A: _tmp168=((struct Cyc_Primopopt_tok_struct*)_tmp167)->f1;goto _LL167;}else{
goto _LL168;}_LL168: goto _LL169;_LL167: yyzzz=_tmp168;goto _LL165;_LL169:(int)_throw((
void*)& Cyc_yyfail_Primopopt_tok);_LL165:;}return yyzzz;}static unsigned char
_tmp16A[7]="qvar_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{_tmp16A,_tmp16A,_tmp16A + 7}};struct _tuple1*Cyc_yyget_QualId_tok(
void*yy1){struct _tuple1*yyzzz;{void*_tmp16B=yy1;struct _tuple1*_tmp16C;_LL16C: if(*((
void**)_tmp16B)== Cyc_QualId_tok){_LL170: _tmp16C=((struct Cyc_QualId_tok_struct*)
_tmp16B)->f1;goto _LL16D;}else{goto _LL16E;}_LL16E: goto _LL16F;_LL16D: yyzzz=_tmp16C;
goto _LL16B;_LL16F:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL16B:;}return
yyzzz;}static unsigned char _tmp16E[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp16E,_tmp16E,_tmp16E + 7}};struct Cyc_Absyn_Stmt*
Cyc_yyget_Stmt_tok(void*yy1){struct Cyc_Absyn_Stmt*yyzzz;{void*_tmp16F=yy1;struct
Cyc_Absyn_Stmt*_tmp170;_LL172: if(*((void**)_tmp16F)== Cyc_Stmt_tok){_LL176:
_tmp170=((struct Cyc_Stmt_tok_struct*)_tmp16F)->f1;goto _LL173;}else{goto _LL174;}
_LL174: goto _LL175;_LL173: yyzzz=_tmp170;goto _LL171;_LL175:(int)_throw((void*)& Cyc_yyfail_Stmt_tok);
_LL171:;}return yyzzz;}static unsigned char _tmp172[24]="list_t<switch_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={Cyc_Core_Failure,{
_tmp172,_tmp172,_tmp172 + 24}};struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp173=yy1;struct Cyc_List_List*
_tmp174;_LL178: if(*((void**)_tmp173)== Cyc_SwitchClauseList_tok){_LL17C: _tmp174=((
struct Cyc_SwitchClauseList_tok_struct*)_tmp173)->f1;goto _LL179;}else{goto _LL17A;}
_LL17A: goto _LL17B;_LL179: yyzzz=_tmp174;goto _LL177;_LL17B:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL177:;}return yyzzz;}static unsigned char _tmp176[25]="list_t<switchC_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={Cyc_Core_Failure,{
_tmp176,_tmp176,_tmp176 + 25}};struct Cyc_List_List*Cyc_yyget_SwitchCClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp177=yy1;struct Cyc_List_List*
_tmp178;_LL17E: if(*((void**)_tmp177)== Cyc_SwitchCClauseList_tok){_LL182: _tmp178=((
struct Cyc_SwitchCClauseList_tok_struct*)_tmp177)->f1;goto _LL17F;}else{goto _LL180;}
_LL180: goto _LL181;_LL17F: yyzzz=_tmp178;goto _LL17D;_LL181:(int)_throw((void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL17D:;}return yyzzz;}static unsigned char _tmp17A[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp17A,_tmp17A,_tmp17A + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp17B=yy1;
struct Cyc_Absyn_Pat*_tmp17C;_LL184: if(*((void**)_tmp17B)== Cyc_Pattern_tok){
_LL188: _tmp17C=((struct Cyc_Pattern_tok_struct*)_tmp17B)->f1;goto _LL185;}else{
goto _LL186;}_LL186: goto _LL187;_LL185: yyzzz=_tmp17C;goto _LL183;_LL187:(int)_throw((
void*)& Cyc_yyfail_Pattern_tok);_LL183:;}return yyzzz;}static unsigned char _tmp17E[
14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure,{_tmp17E,_tmp17E,_tmp17E + 14}};struct Cyc_List_List*Cyc_yyget_PatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp17F=yy1;struct Cyc_List_List*
_tmp180;_LL18A: if(*((void**)_tmp17F)== Cyc_PatternList_tok){_LL18E: _tmp180=((
struct Cyc_PatternList_tok_struct*)_tmp17F)->f1;goto _LL18B;}else{goto _LL18C;}
_LL18C: goto _LL18D;_LL18B: yyzzz=_tmp180;goto _LL189;_LL18D:(int)_throw((void*)& Cyc_yyfail_PatternList_tok);
_LL189:;}return yyzzz;}static unsigned char _tmp182[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{
_tmp182,_tmp182,_tmp182 + 31}};struct _tuple10*Cyc_yyget_FieldPattern_tok(void*yy1){
struct _tuple10*yyzzz;{void*_tmp183=yy1;struct _tuple10*_tmp184;_LL190: if(*((void**)
_tmp183)== Cyc_FieldPattern_tok){_LL194: _tmp184=((struct Cyc_FieldPattern_tok_struct*)
_tmp183)->f1;goto _LL191;}else{goto _LL192;}_LL192: goto _LL193;_LL191: yyzzz=_tmp184;
goto _LL18F;_LL193:(int)_throw((void*)& Cyc_yyfail_FieldPattern_tok);_LL18F:;}
return yyzzz;}static unsigned char _tmp186[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp186,_tmp186,_tmp186 + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp187=yy1;struct Cyc_List_List*
_tmp188;_LL196: if(*((void**)_tmp187)== Cyc_FieldPatternList_tok){_LL19A: _tmp188=((
struct Cyc_FieldPatternList_tok_struct*)_tmp187)->f1;goto _LL197;}else{goto _LL198;}
_LL198: goto _LL199;_LL197: yyzzz=_tmp188;goto _LL195;_LL199:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL195:;}return yyzzz;}static unsigned char _tmp18A[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={Cyc_Core_Failure,{_tmp18A,_tmp18A,_tmp18A + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok(void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp18B=yy1;
struct Cyc_Absyn_Fndecl*_tmp18C;_LL19C: if(*((void**)_tmp18B)== Cyc_FnDecl_tok){
_LL1A0: _tmp18C=((struct Cyc_FnDecl_tok_struct*)_tmp18B)->f1;goto _LL19D;}else{goto
_LL19E;}_LL19E: goto _LL19F;_LL19D: yyzzz=_tmp18C;goto _LL19B;_LL19F:(int)_throw((
void*)& Cyc_yyfail_FnDecl_tok);_LL19B:;}return yyzzz;}static unsigned char _tmp18E[
15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={
Cyc_Core_Failure,{_tmp18E,_tmp18E,_tmp18E + 15}};struct Cyc_List_List*Cyc_yyget_DeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp18F=yy1;struct Cyc_List_List*
_tmp190;_LL1A2: if(*((void**)_tmp18F)== Cyc_DeclList_tok){_LL1A6: _tmp190=((struct
Cyc_DeclList_tok_struct*)_tmp18F)->f1;goto _LL1A3;}else{goto _LL1A4;}_LL1A4: goto
_LL1A5;_LL1A3: yyzzz=_tmp190;goto _LL1A1;_LL1A5:(int)_throw((void*)& Cyc_yyfail_DeclList_tok);
_LL1A1:;}return yyzzz;}static unsigned char _tmp192[12]="decl_spec_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={Cyc_Core_Failure,{_tmp192,
_tmp192,_tmp192 + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(
void*yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{void*_tmp193=yy1;struct Cyc_Parse_Declaration_spec*
_tmp194;_LL1A8: if(*((void**)_tmp193)== Cyc_DeclSpec_tok){_LL1AC: _tmp194=((struct
Cyc_DeclSpec_tok_struct*)_tmp193)->f1;goto _LL1A9;}else{goto _LL1AA;}_LL1AA: goto
_LL1AB;_LL1A9: yyzzz=_tmp194;goto _LL1A7;_LL1AB:(int)_throw((void*)& Cyc_yyfail_DeclSpec_tok);
_LL1A7:;}return yyzzz;}static unsigned char _tmp196[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{
_tmp196,_tmp196,_tmp196 + 27}};struct _tuple11*Cyc_yyget_InitDecl_tok(void*yy1){
struct _tuple11*yyzzz;{void*_tmp197=yy1;struct _tuple11*_tmp198;_LL1AE: if(*((void**)
_tmp197)== Cyc_InitDecl_tok){_LL1B2: _tmp198=((struct Cyc_InitDecl_tok_struct*)
_tmp197)->f1;goto _LL1AF;}else{goto _LL1B0;}_LL1B0: goto _LL1B1;_LL1AF: yyzzz=_tmp198;
goto _LL1AD;_LL1B1:(int)_throw((void*)& Cyc_yyfail_InitDecl_tok);_LL1AD:;}return
yyzzz;}static unsigned char _tmp19A[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp19A,_tmp19A,_tmp19A + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp19B=yy1;struct Cyc_List_List*
_tmp19C;_LL1B4: if(*((void**)_tmp19B)== Cyc_InitDeclList_tok){_LL1B8: _tmp19C=((
struct Cyc_InitDeclList_tok_struct*)_tmp19B)->f1;goto _LL1B5;}else{goto _LL1B6;}
_LL1B6: goto _LL1B7;_LL1B5: yyzzz=_tmp19C;goto _LL1B3;_LL1B7:(int)_throw((void*)& Cyc_yyfail_InitDeclList_tok);
_LL1B3:;}return yyzzz;}static unsigned char _tmp19E[16]="storage_class_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{
_tmp19E,_tmp19E,_tmp19E + 16}};void*Cyc_yyget_StorageClass_tok(void*yy1){void*
yyzzz;{void*_tmp19F=yy1;void*_tmp1A0;_LL1BA: if(*((void**)_tmp19F)== Cyc_StorageClass_tok){
_LL1BE: _tmp1A0=(void*)((struct Cyc_StorageClass_tok_struct*)_tmp19F)->f1;goto
_LL1BB;}else{goto _LL1BC;}_LL1BC: goto _LL1BD;_LL1BB: yyzzz=_tmp1A0;goto _LL1B9;
_LL1BD:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);_LL1B9:;}return yyzzz;}
static unsigned char _tmp1A2[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{_tmp1A2,_tmp1A2,_tmp1A2 + 17}};
void*Cyc_yyget_TypeSpecifier_tok(void*yy1){void*yyzzz;{void*_tmp1A3=yy1;void*
_tmp1A4;_LL1C0: if(*((void**)_tmp1A3)== Cyc_TypeSpecifier_tok){_LL1C4: _tmp1A4=(
void*)((struct Cyc_TypeSpecifier_tok_struct*)_tmp1A3)->f1;goto _LL1C1;}else{goto
_LL1C2;}_LL1C2: goto _LL1C3;_LL1C1: yyzzz=_tmp1A4;goto _LL1BF;_LL1C3:(int)_throw((
void*)& Cyc_yyfail_TypeSpecifier_tok);_LL1BF:;}return yyzzz;}static unsigned char
_tmp1A6[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={
Cyc_Core_Failure,{_tmp1A6,_tmp1A6,_tmp1A6 + 12}};void*Cyc_yyget_StructOrUnion_tok(
void*yy1){void*yyzzz;{void*_tmp1A7=yy1;void*_tmp1A8;_LL1C6: if(*((void**)_tmp1A7)
== Cyc_StructOrUnion_tok){_LL1CA: _tmp1A8=(void*)((struct Cyc_StructOrUnion_tok_struct*)
_tmp1A7)->f1;goto _LL1C7;}else{goto _LL1C8;}_LL1C8: goto _LL1C9;_LL1C7: yyzzz=_tmp1A8;
goto _LL1C5;_LL1C9:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);_LL1C5:;}
return yyzzz;}static unsigned char _tmp1AA[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp1AA,_tmp1AA,_tmp1AA + 8}};struct
Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*
_tmp1AB=yy1;struct Cyc_Absyn_Tqual _tmp1AC;_LL1CC: if(*((void**)_tmp1AB)== Cyc_TypeQual_tok){
_LL1D0: _tmp1AC=((struct Cyc_TypeQual_tok_struct*)_tmp1AB)->f1;goto _LL1CD;}else{
goto _LL1CE;}_LL1CE: goto _LL1CF;_LL1CD: yyzzz=_tmp1AC;goto _LL1CB;_LL1CF:(int)_throw((
void*)& Cyc_yyfail_TypeQual_tok);_LL1CB:;}return yyzzz;}static unsigned char _tmp1AE[
20]="list_t<aggrfield_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclList_tok={
Cyc_Core_Failure,{_tmp1AE,_tmp1AE,_tmp1AE + 20}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1AF=yy1;struct Cyc_List_List*
_tmp1B0;_LL1D2: if(*((void**)_tmp1AF)== Cyc_AggrFieldDeclList_tok){_LL1D6: _tmp1B0=((
struct Cyc_AggrFieldDeclList_tok_struct*)_tmp1AF)->f1;goto _LL1D3;}else{goto _LL1D4;}
_LL1D4: goto _LL1D5;_LL1D3: yyzzz=_tmp1B0;goto _LL1D1;_LL1D5:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclList_tok);
_LL1D1:;}return yyzzz;}static unsigned char _tmp1B2[28]="list_t<list_t<aggrfield_t>>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclListList_tok={Cyc_Core_Failure,{
_tmp1B2,_tmp1B2,_tmp1B2 + 28}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclListList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B3=yy1;struct Cyc_List_List*
_tmp1B4;_LL1D8: if(*((void**)_tmp1B3)== Cyc_AggrFieldDeclListList_tok){_LL1DC:
_tmp1B4=((struct Cyc_AggrFieldDeclListList_tok_struct*)_tmp1B3)->f1;goto _LL1D9;}
else{goto _LL1DA;}_LL1DA: goto _LL1DB;_LL1D9: yyzzz=_tmp1B4;goto _LL1D7;_LL1DB:(int)
_throw((void*)& Cyc_yyfail_AggrFieldDeclListList_tok);_LL1D7:;}return yyzzz;}
static unsigned char _tmp1B6[24]="list_t<type_modifier_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{_tmp1B6,_tmp1B6,_tmp1B6 + 24}};
struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1B7=yy1;struct Cyc_List_List*_tmp1B8;_LL1DE: if(*((void**)_tmp1B7)
== Cyc_TypeModifierList_tok){_LL1E2: _tmp1B8=((struct Cyc_TypeModifierList_tok_struct*)
_tmp1B7)->f1;goto _LL1DF;}else{goto _LL1E0;}_LL1E0: goto _LL1E1;_LL1DF: yyzzz=_tmp1B8;
goto _LL1DD;_LL1E1:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);_LL1DD:;}
return yyzzz;}static unsigned char _tmp1BA[13]="declarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp1BA,_tmp1BA,_tmp1BA + 13}};
struct Cyc_Parse_Declarator*Cyc_yyget_Declarator_tok(void*yy1){struct Cyc_Parse_Declarator*
yyzzz;{void*_tmp1BB=yy1;struct Cyc_Parse_Declarator*_tmp1BC;_LL1E4: if(*((void**)
_tmp1BB)== Cyc_Declarator_tok){_LL1E8: _tmp1BC=((struct Cyc_Declarator_tok_struct*)
_tmp1BB)->f1;goto _LL1E5;}else{goto _LL1E6;}_LL1E6: goto _LL1E7;_LL1E5: yyzzz=_tmp1BC;
goto _LL1E3;_LL1E7:(int)_throw((void*)& Cyc_yyfail_Declarator_tok);_LL1E3:;}return
yyzzz;}static unsigned char _tmp1BE[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{_tmp1BE,_tmp1BE,_tmp1BE + 21}};
struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(void*yy1){
struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp1BF=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp1C0;_LL1EA: if(*((void**)_tmp1BF)== Cyc_AbstractDeclarator_tok){_LL1EE:
_tmp1C0=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp1BF)->f1;goto _LL1EB;}
else{goto _LL1EC;}_LL1EC: goto _LL1ED;_LL1EB: yyzzz=_tmp1C0;goto _LL1E9;_LL1ED:(int)
_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);_LL1E9:;}return yyzzz;}static
unsigned char _tmp1C2[5]="bool";static struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={
Cyc_Core_Failure,{_tmp1C2,_tmp1C2,_tmp1C2 + 5}};int Cyc_yyget_Bool_tok(void*yy1){
int yyzzz;{void*_tmp1C3=yy1;int _tmp1C4;_LL1F0: if(*((void**)_tmp1C3)== Cyc_Bool_tok){
_LL1F4: _tmp1C4=((struct Cyc_Bool_tok_struct*)_tmp1C3)->f1;goto _LL1F1;}else{goto
_LL1F2;}_LL1F2: goto _LL1F3;_LL1F1: yyzzz=_tmp1C4;goto _LL1EF;_LL1F3:(int)_throw((
void*)& Cyc_yyfail_Bool_tok);_LL1EF:;}return yyzzz;}static unsigned char _tmp1C6[8]="scope_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Scope_tok={Cyc_Core_Failure,{
_tmp1C6,_tmp1C6,_tmp1C6 + 8}};void*Cyc_yyget_Scope_tok(void*yy1){void*yyzzz;{void*
_tmp1C7=yy1;void*_tmp1C8;_LL1F6: if(*((void**)_tmp1C7)== Cyc_Scope_tok){_LL1FA:
_tmp1C8=(void*)((struct Cyc_Scope_tok_struct*)_tmp1C7)->f1;goto _LL1F7;}else{goto
_LL1F8;}_LL1F8: goto _LL1F9;_LL1F7: yyzzz=_tmp1C8;goto _LL1F5;_LL1F9:(int)_throw((
void*)& Cyc_yyfail_Scope_tok);_LL1F5:;}return yyzzz;}static unsigned char _tmp1CA[14]="tunionfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={Cyc_Core_Failure,{
_tmp1CA,_tmp1CA,_tmp1CA + 14}};struct Cyc_Absyn_Tunionfield*Cyc_yyget_TunionField_tok(
void*yy1){struct Cyc_Absyn_Tunionfield*yyzzz;{void*_tmp1CB=yy1;struct Cyc_Absyn_Tunionfield*
_tmp1CC;_LL1FC: if(*((void**)_tmp1CB)== Cyc_TunionField_tok){_LL200: _tmp1CC=((
struct Cyc_TunionField_tok_struct*)_tmp1CB)->f1;goto _LL1FD;}else{goto _LL1FE;}
_LL1FE: goto _LL1FF;_LL1FD: yyzzz=_tmp1CC;goto _LL1FB;_LL1FF:(int)_throw((void*)& Cyc_yyfail_TunionField_tok);
_LL1FB:;}return yyzzz;}static unsigned char _tmp1CE[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{
_tmp1CE,_tmp1CE,_tmp1CE + 22}};struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1CF=yy1;struct Cyc_List_List*
_tmp1D0;_LL202: if(*((void**)_tmp1CF)== Cyc_TunionFieldList_tok){_LL206: _tmp1D0=((
struct Cyc_TunionFieldList_tok_struct*)_tmp1CF)->f1;goto _LL203;}else{goto _LL204;}
_LL204: goto _LL205;_LL203: yyzzz=_tmp1D0;goto _LL201;_LL205:(int)_throw((void*)& Cyc_yyfail_TunionFieldList_tok);
_LL201:;}return yyzzz;}static unsigned char _tmp1D2[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{
_tmp1D2,_tmp1D2,_tmp1D2 + 50}};struct _tuple14*Cyc_yyget_QualSpecList_tok(void*yy1){
struct _tuple14*yyzzz;{void*_tmp1D3=yy1;struct _tuple14*_tmp1D4;_LL208: if(*((void**)
_tmp1D3)== Cyc_QualSpecList_tok){_LL20C: _tmp1D4=((struct Cyc_QualSpecList_tok_struct*)
_tmp1D3)->f1;goto _LL209;}else{goto _LL20A;}_LL20A: goto _LL20B;_LL209: yyzzz=_tmp1D4;
goto _LL207;_LL20B:(int)_throw((void*)& Cyc_yyfail_QualSpecList_tok);_LL207:;}
return yyzzz;}static unsigned char _tmp1D6[14]="list_t<var_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={Cyc_Core_Failure,{_tmp1D6,_tmp1D6,_tmp1D6 + 14}};struct Cyc_List_List*
Cyc_yyget_IdList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1D7=yy1;
struct Cyc_List_List*_tmp1D8;_LL20E: if(*((void**)_tmp1D7)== Cyc_IdList_tok){
_LL212: _tmp1D8=((struct Cyc_IdList_tok_struct*)_tmp1D7)->f1;goto _LL20F;}else{goto
_LL210;}_LL210: goto _LL211;_LL20F: yyzzz=_tmp1D8;goto _LL20D;_LL211:(int)_throw((
void*)& Cyc_yyfail_IdList_tok);_LL20D:;}return yyzzz;}static unsigned char _tmp1DA[
32]="$(opt_t<var_t>,tqual_t,type_t)@";static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={
Cyc_Core_Failure,{_tmp1DA,_tmp1DA,_tmp1DA + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(
void*yy1){struct _tuple2*yyzzz;{void*_tmp1DB=yy1;struct _tuple2*_tmp1DC;_LL214: if(*((
void**)_tmp1DB)== Cyc_ParamDecl_tok){_LL218: _tmp1DC=((struct Cyc_ParamDecl_tok_struct*)
_tmp1DB)->f1;goto _LL215;}else{goto _LL216;}_LL216: goto _LL217;_LL215: yyzzz=_tmp1DC;
goto _LL213;_LL217:(int)_throw((void*)& Cyc_yyfail_ParamDecl_tok);_LL213:;}return
yyzzz;}static unsigned char _tmp1DE[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp1DE,_tmp1DE,_tmp1DE + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1DF=yy1;struct Cyc_List_List*
_tmp1E0;_LL21A: if(*((void**)_tmp1DF)== Cyc_ParamDeclList_tok){_LL21E: _tmp1E0=((
struct Cyc_ParamDeclList_tok_struct*)_tmp1DF)->f1;goto _LL21B;}else{goto _LL21C;}
_LL21C: goto _LL21D;_LL21B: yyzzz=_tmp1E0;goto _LL219;_LL21D:(int)_throw((void*)& Cyc_yyfail_ParamDeclList_tok);
_LL219:;}return yyzzz;}static unsigned char _tmp1E2[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1E2,
_tmp1E2,_tmp1E2 + 107}};struct _tuple13*Cyc_yyget_YY1(void*yy1){struct _tuple13*
yyzzz;{void*_tmp1E3=yy1;struct _tuple13*_tmp1E4;_LL220: if(*((void**)_tmp1E3)== 
Cyc_YY1){_LL224: _tmp1E4=((struct Cyc_YY1_struct*)_tmp1E3)->f1;goto _LL221;}else{
goto _LL222;}_LL222: goto _LL223;_LL221: yyzzz=_tmp1E4;goto _LL21F;_LL223:(int)_throw((
void*)& Cyc_yyfail_YY1);_LL21F:;}return yyzzz;}static unsigned char _tmp1E6[15]="list_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={Cyc_Core_Failure,{
_tmp1E6,_tmp1E6,_tmp1E6 + 15}};struct Cyc_List_List*Cyc_yyget_TypeList_tok(void*
yy1){struct Cyc_List_List*yyzzz;{void*_tmp1E7=yy1;struct Cyc_List_List*_tmp1E8;
_LL226: if(*((void**)_tmp1E7)== Cyc_TypeList_tok){_LL22A: _tmp1E8=((struct Cyc_TypeList_tok_struct*)
_tmp1E7)->f1;goto _LL227;}else{goto _LL228;}_LL228: goto _LL229;_LL227: yyzzz=_tmp1E8;
goto _LL225;_LL229:(int)_throw((void*)& Cyc_yyfail_TypeList_tok);_LL225:;}return
yyzzz;}static unsigned char _tmp1EA[21]="list_t<designator_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{_tmp1EA,_tmp1EA,_tmp1EA + 21}};
struct Cyc_List_List*Cyc_yyget_DesignatorList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1EB=yy1;struct Cyc_List_List*_tmp1EC;_LL22C: if(*((void**)_tmp1EB)
== Cyc_DesignatorList_tok){_LL230: _tmp1EC=((struct Cyc_DesignatorList_tok_struct*)
_tmp1EB)->f1;goto _LL22D;}else{goto _LL22E;}_LL22E: goto _LL22F;_LL22D: yyzzz=_tmp1EC;
goto _LL22B;_LL22F:(int)_throw((void*)& Cyc_yyfail_DesignatorList_tok);_LL22B:;}
return yyzzz;}static unsigned char _tmp1EE[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={Cyc_Core_Failure,{_tmp1EE,_tmp1EE,_tmp1EE + 13}};void*
Cyc_yyget_Designator_tok(void*yy1){void*yyzzz;{void*_tmp1EF=yy1;void*_tmp1F0;
_LL232: if(*((void**)_tmp1EF)== Cyc_Designator_tok){_LL236: _tmp1F0=(void*)((
struct Cyc_Designator_tok_struct*)_tmp1EF)->f1;goto _LL233;}else{goto _LL234;}
_LL234: goto _LL235;_LL233: yyzzz=_tmp1F0;goto _LL231;_LL235:(int)_throw((void*)& Cyc_yyfail_Designator_tok);
_LL231:;}return yyzzz;}static unsigned char _tmp1F2[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{_tmp1F2,_tmp1F2,_tmp1F2 + 7}};void*Cyc_yyget_Kind_tok(
void*yy1){void*yyzzz;{void*_tmp1F3=yy1;void*_tmp1F4;_LL238: if(*((void**)_tmp1F3)
== Cyc_Kind_tok){_LL23C: _tmp1F4=(void*)((struct Cyc_Kind_tok_struct*)_tmp1F3)->f1;
goto _LL239;}else{goto _LL23A;}_LL23A: goto _LL23B;_LL239: yyzzz=_tmp1F4;goto _LL237;
_LL23B:(int)_throw((void*)& Cyc_yyfail_Kind_tok);_LL237:;}return yyzzz;}static
unsigned char _tmp1F6[7]="type_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={
Cyc_Core_Failure,{_tmp1F6,_tmp1F6,_tmp1F6 + 7}};void*Cyc_yyget_Type_tok(void*yy1){
void*yyzzz;{void*_tmp1F7=yy1;void*_tmp1F8;_LL23E: if(*((void**)_tmp1F7)== Cyc_Type_tok){
_LL242: _tmp1F8=(void*)((struct Cyc_Type_tok_struct*)_tmp1F7)->f1;goto _LL23F;}
else{goto _LL240;}_LL240: goto _LL241;_LL23F: yyzzz=_tmp1F8;goto _LL23D;_LL241:(int)
_throw((void*)& Cyc_yyfail_Type_tok);_LL23D:;}return yyzzz;}static unsigned char
_tmp1FA[20]="list_t<attribute_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={
Cyc_Core_Failure,{_tmp1FA,_tmp1FA,_tmp1FA + 20}};struct Cyc_List_List*Cyc_yyget_AttributeList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1FB=yy1;struct Cyc_List_List*
_tmp1FC;_LL244: if(*((void**)_tmp1FB)== Cyc_AttributeList_tok){_LL248: _tmp1FC=((
struct Cyc_AttributeList_tok_struct*)_tmp1FB)->f1;goto _LL245;}else{goto _LL246;}
_LL246: goto _LL247;_LL245: yyzzz=_tmp1FC;goto _LL243;_LL247:(int)_throw((void*)& Cyc_yyfail_AttributeList_tok);
_LL243:;}return yyzzz;}static unsigned char _tmp1FE[12]="attribute_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{_tmp1FE,
_tmp1FE,_tmp1FE + 12}};void*Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{void*
_tmp1FF=yy1;void*_tmp200;_LL24A: if(*((void**)_tmp1FF)== Cyc_Attribute_tok){
_LL24E: _tmp200=(void*)((struct Cyc_Attribute_tok_struct*)_tmp1FF)->f1;goto _LL24B;}
else{goto _LL24C;}_LL24C: goto _LL24D;_LL24B: yyzzz=_tmp200;goto _LL249;_LL24D:(int)
_throw((void*)& Cyc_yyfail_Attribute_tok);_LL249:;}return yyzzz;}static
unsigned char _tmp202[12]="enumfield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={
Cyc_Core_Failure,{_tmp202,_tmp202,_tmp202 + 12}};struct Cyc_Absyn_Enumfield*Cyc_yyget_Enumfield_tok(
void*yy1){struct Cyc_Absyn_Enumfield*yyzzz;{void*_tmp203=yy1;struct Cyc_Absyn_Enumfield*
_tmp204;_LL250: if(*((void**)_tmp203)== Cyc_Enumfield_tok){_LL254: _tmp204=((
struct Cyc_Enumfield_tok_struct*)_tmp203)->f1;goto _LL251;}else{goto _LL252;}_LL252:
goto _LL253;_LL251: yyzzz=_tmp204;goto _LL24F;_LL253:(int)_throw((void*)& Cyc_yyfail_Enumfield_tok);
_LL24F:;}return yyzzz;}static unsigned char _tmp206[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{
_tmp206,_tmp206,_tmp206 + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp207=yy1;struct Cyc_List_List*
_tmp208;_LL256: if(*((void**)_tmp207)== Cyc_EnumfieldList_tok){_LL25A: _tmp208=((
struct Cyc_EnumfieldList_tok_struct*)_tmp207)->f1;goto _LL257;}else{goto _LL258;}
_LL258: goto _LL259;_LL257: yyzzz=_tmp208;goto _LL255;_LL259:(int)_throw((void*)& Cyc_yyfail_EnumfieldList_tok);
_LL255:;}return yyzzz;}static unsigned char _tmp20A[14]="opt_t<type_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeOpt_tok={Cyc_Core_Failure,{_tmp20A,
_tmp20A,_tmp20A + 14}};struct Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(void*yy1){struct
Cyc_Core_Opt*yyzzz;{void*_tmp20B=yy1;struct Cyc_Core_Opt*_tmp20C;_LL25C: if(*((
void**)_tmp20B)== Cyc_TypeOpt_tok){_LL260: _tmp20C=((struct Cyc_TypeOpt_tok_struct*)
_tmp20B)->f1;goto _LL25D;}else{goto _LL25E;}_LL25E: goto _LL25F;_LL25D: yyzzz=_tmp20C;
goto _LL25B;_LL25F:(int)_throw((void*)& Cyc_yyfail_TypeOpt_tok);_LL25B:;}return
yyzzz;}static unsigned char _tmp20E[26]="list_t<$(type_t,type_t)@>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{_tmp20E,
_tmp20E,_tmp20E + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp20F=yy1;struct Cyc_List_List*_tmp210;_LL262:
if(*((void**)_tmp20F)== Cyc_Rgnorder_tok){_LL266: _tmp210=((struct Cyc_Rgnorder_tok_struct*)
_tmp20F)->f1;goto _LL263;}else{goto _LL264;}_LL264: goto _LL265;_LL263: yyzzz=_tmp210;
goto _LL261;_LL265:(int)_throw((void*)& Cyc_yyfail_Rgnorder_tok);_LL261:;}return
yyzzz;}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;
int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){return({struct Cyc_Yyltype
_tmp211;_tmp211.timestamp=0;_tmp211.first_line=0;_tmp211.first_column=0;_tmp211.last_line=
0;_tmp211.last_column=0;_tmp211;});}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
static short Cyc_yytranslate[354]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,125,2,2,107,123,119,2,103,104,114,117,105,118,111,122,2,2,2,2,
2,2,2,2,2,2,110,99,108,102,109,116,115,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,112,2,113,121,106,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,100,120,101,124,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,
46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,
73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98};
static unsigned char _tmp212[2]="$";static unsigned char _tmp213[6]="error";static
unsigned char _tmp214[12]="$undefined.";static unsigned char _tmp215[5]="AUTO";
static unsigned char _tmp216[9]="REGISTER";static unsigned char _tmp217[7]="STATIC";
static unsigned char _tmp218[7]="EXTERN";static unsigned char _tmp219[8]="TYPEDEF";
static unsigned char _tmp21A[5]="VOID";static unsigned char _tmp21B[5]="CHAR";static
unsigned char _tmp21C[6]="SHORT";static unsigned char _tmp21D[4]="INT";static
unsigned char _tmp21E[5]="LONG";static unsigned char _tmp21F[6]="FLOAT";static
unsigned char _tmp220[7]="DOUBLE";static unsigned char _tmp221[7]="SIGNED";static
unsigned char _tmp222[9]="UNSIGNED";static unsigned char _tmp223[6]="CONST";static
unsigned char _tmp224[9]="VOLATILE";static unsigned char _tmp225[9]="RESTRICT";
static unsigned char _tmp226[7]="STRUCT";static unsigned char _tmp227[6]="UNION";
static unsigned char _tmp228[5]="CASE";static unsigned char _tmp229[8]="DEFAULT";
static unsigned char _tmp22A[7]="INLINE";static unsigned char _tmp22B[7]="SIZEOF";
static unsigned char _tmp22C[9]="OFFSETOF";static unsigned char _tmp22D[3]="IF";
static unsigned char _tmp22E[5]="ELSE";static unsigned char _tmp22F[7]="SWITCH";
static unsigned char _tmp230[6]="WHILE";static unsigned char _tmp231[3]="DO";static
unsigned char _tmp232[4]="FOR";static unsigned char _tmp233[5]="GOTO";static
unsigned char _tmp234[9]="CONTINUE";static unsigned char _tmp235[6]="BREAK";static
unsigned char _tmp236[7]="RETURN";static unsigned char _tmp237[5]="ENUM";static
unsigned char _tmp238[8]="NULL_kw";static unsigned char _tmp239[4]="LET";static
unsigned char _tmp23A[6]="THROW";static unsigned char _tmp23B[4]="TRY";static
unsigned char _tmp23C[6]="CATCH";static unsigned char _tmp23D[4]="NEW";static
unsigned char _tmp23E[9]="ABSTRACT";static unsigned char _tmp23F[9]="FALLTHRU";
static unsigned char _tmp240[6]="USING";static unsigned char _tmp241[10]="NAMESPACE";
static unsigned char _tmp242[7]="TUNION";static unsigned char _tmp243[8]="XTUNION";
static unsigned char _tmp244[9]="FORARRAY";static unsigned char _tmp245[5]="FILL";
static unsigned char _tmp246[8]="CODEGEN";static unsigned char _tmp247[4]="CUT";
static unsigned char _tmp248[7]="SPLICE";static unsigned char _tmp249[7]="MALLOC";
static unsigned char _tmp24A[8]="RMALLOC";static unsigned char _tmp24B[7]="CALLOC";
static unsigned char _tmp24C[8]="RCALLOC";static unsigned char _tmp24D[9]="REGION_T";
static unsigned char _tmp24E[9]="SIZEOF_T";static unsigned char _tmp24F[7]="REGION";
static unsigned char _tmp250[5]="RNEW";static unsigned char _tmp251[8]="REGIONS";
static unsigned char _tmp252[4]="GEN";static unsigned char _tmp253[7]="PTR_OP";
static unsigned char _tmp254[7]="INC_OP";static unsigned char _tmp255[7]="DEC_OP";
static unsigned char _tmp256[8]="LEFT_OP";static unsigned char _tmp257[9]="RIGHT_OP";
static unsigned char _tmp258[6]="LE_OP";static unsigned char _tmp259[6]="GE_OP";
static unsigned char _tmp25A[6]="EQ_OP";static unsigned char _tmp25B[6]="NE_OP";
static unsigned char _tmp25C[7]="AND_OP";static unsigned char _tmp25D[6]="OR_OP";
static unsigned char _tmp25E[11]="MUL_ASSIGN";static unsigned char _tmp25F[11]="DIV_ASSIGN";
static unsigned char _tmp260[11]="MOD_ASSIGN";static unsigned char _tmp261[11]="ADD_ASSIGN";
static unsigned char _tmp262[11]="SUB_ASSIGN";static unsigned char _tmp263[12]="LEFT_ASSIGN";
static unsigned char _tmp264[13]="RIGHT_ASSIGN";static unsigned char _tmp265[11]="AND_ASSIGN";
static unsigned char _tmp266[11]="XOR_ASSIGN";static unsigned char _tmp267[10]="OR_ASSIGN";
static unsigned char _tmp268[9]="ELLIPSIS";static unsigned char _tmp269[11]="LEFT_RIGHT";
static unsigned char _tmp26A[12]="COLON_COLON";static unsigned char _tmp26B[11]="IDENTIFIER";
static unsigned char _tmp26C[17]="INTEGER_CONSTANT";static unsigned char _tmp26D[7]="STRING";
static unsigned char _tmp26E[19]="CHARACTER_CONSTANT";static unsigned char _tmp26F[
18]="FLOATING_CONSTANT";static unsigned char _tmp270[9]="TYPE_VAR";static
unsigned char _tmp271[13]="TYPEDEF_NAME";static unsigned char _tmp272[16]="QUAL_IDENTIFIER";
static unsigned char _tmp273[18]="QUAL_TYPEDEF_NAME";static unsigned char _tmp274[10]="ATTRIBUTE";
static unsigned char _tmp275[4]="';'";static unsigned char _tmp276[4]="'{'";static
unsigned char _tmp277[4]="'}'";static unsigned char _tmp278[4]="'='";static
unsigned char _tmp279[4]="'('";static unsigned char _tmp27A[4]="')'";static
unsigned char _tmp27B[4]="','";static unsigned char _tmp27C[4]="'_'";static
unsigned char _tmp27D[4]="'$'";static unsigned char _tmp27E[4]="'<'";static
unsigned char _tmp27F[4]="'>'";static unsigned char _tmp280[4]="':'";static
unsigned char _tmp281[4]="'.'";static unsigned char _tmp282[4]="'['";static
unsigned char _tmp283[4]="']'";static unsigned char _tmp284[4]="'*'";static
unsigned char _tmp285[4]="'@'";static unsigned char _tmp286[4]="'?'";static
unsigned char _tmp287[4]="'+'";static unsigned char _tmp288[4]="'-'";static
unsigned char _tmp289[4]="'&'";static unsigned char _tmp28A[4]="'|'";static
unsigned char _tmp28B[4]="'^'";static unsigned char _tmp28C[4]="'/'";static
unsigned char _tmp28D[4]="'%'";static unsigned char _tmp28E[4]="'~'";static
unsigned char _tmp28F[4]="'!'";static unsigned char _tmp290[5]="prog";static
unsigned char _tmp291[17]="translation_unit";static unsigned char _tmp292[21]="external_declaration";
static unsigned char _tmp293[20]="function_definition";static unsigned char _tmp294[
21]="function_definition2";static unsigned char _tmp295[13]="using_action";static
unsigned char _tmp296[15]="unusing_action";static unsigned char _tmp297[17]="namespace_action";
static unsigned char _tmp298[19]="unnamespace_action";static unsigned char _tmp299[
12]="declaration";static unsigned char _tmp29A[17]="declaration_list";static
unsigned char _tmp29B[23]="declaration_specifiers";static unsigned char _tmp29C[24]="storage_class_specifier";
static unsigned char _tmp29D[15]="attributes_opt";static unsigned char _tmp29E[11]="attributes";
static unsigned char _tmp29F[15]="attribute_list";static unsigned char _tmp2A0[10]="attribute";
static unsigned char _tmp2A1[15]="type_specifier";static unsigned char _tmp2A2[5]="kind";
static unsigned char _tmp2A3[15]="type_qualifier";static unsigned char _tmp2A4[15]="enum_specifier";
static unsigned char _tmp2A5[11]="enum_field";static unsigned char _tmp2A6[22]="enum_declaration_list";
static unsigned char _tmp2A7[26]="struct_or_union_specifier";static unsigned char
_tmp2A8[15]="exist_vars_opt";static unsigned char _tmp2A9[16]="type_params_opt";
static unsigned char _tmp2AA[16]="struct_or_union";static unsigned char _tmp2AB[24]="struct_declaration_list";
static unsigned char _tmp2AC[25]="struct_declaration_list0";static unsigned char
_tmp2AD[21]="init_declarator_list";static unsigned char _tmp2AE[22]="init_declarator_list0";
static unsigned char _tmp2AF[16]="init_declarator";static unsigned char _tmp2B0[19]="struct_declaration";
static unsigned char _tmp2B1[25]="specifier_qualifier_list";static unsigned char
_tmp2B2[23]="struct_declarator_list";static unsigned char _tmp2B3[24]="struct_declarator_list0";
static unsigned char _tmp2B4[18]="struct_declarator";static unsigned char _tmp2B5[17]="tunion_specifier";
static unsigned char _tmp2B6[18]="tunion_or_xtunion";static unsigned char _tmp2B7[17]="tunionfield_list";
static unsigned char _tmp2B8[18]="tunionfield_scope";static unsigned char _tmp2B9[12]="tunionfield";
static unsigned char _tmp2BA[11]="declarator";static unsigned char _tmp2BB[18]="direct_declarator";
static unsigned char _tmp2BC[8]="pointer";static unsigned char _tmp2BD[13]="pointer_char";
static unsigned char _tmp2BE[8]="rgn_opt";static unsigned char _tmp2BF[4]="rgn";
static unsigned char _tmp2C0[20]="type_qualifier_list";static unsigned char _tmp2C1[
20]="parameter_type_list";static unsigned char _tmp2C2[9]="type_var";static
unsigned char _tmp2C3[16]="optional_effect";static unsigned char _tmp2C4[19]="optional_rgn_order";
static unsigned char _tmp2C5[10]="rgn_order";static unsigned char _tmp2C6[16]="optional_inject";
static unsigned char _tmp2C7[11]="effect_set";static unsigned char _tmp2C8[14]="atomic_effect";
static unsigned char _tmp2C9[11]="region_set";static unsigned char _tmp2CA[15]="parameter_list";
static unsigned char _tmp2CB[22]="parameter_declaration";static unsigned char
_tmp2CC[16]="identifier_list";static unsigned char _tmp2CD[17]="identifier_list0";
static unsigned char _tmp2CE[12]="initializer";static unsigned char _tmp2CF[18]="array_initializer";
static unsigned char _tmp2D0[17]="initializer_list";static unsigned char _tmp2D1[12]="designation";
static unsigned char _tmp2D2[16]="designator_list";static unsigned char _tmp2D3[11]="designator";
static unsigned char _tmp2D4[10]="type_name";static unsigned char _tmp2D5[14]="any_type_name";
static unsigned char _tmp2D6[15]="type_name_list";static unsigned char _tmp2D7[20]="abstract_declarator";
static unsigned char _tmp2D8[27]="direct_abstract_declarator";static unsigned char
_tmp2D9[10]="statement";static unsigned char _tmp2DA[18]="labeled_statement";
static unsigned char _tmp2DB[21]="expression_statement";static unsigned char _tmp2DC[
19]="compound_statement";static unsigned char _tmp2DD[16]="block_item_list";static
unsigned char _tmp2DE[20]="selection_statement";static unsigned char _tmp2DF[15]="switch_clauses";
static unsigned char _tmp2E0[16]="switchC_clauses";static unsigned char _tmp2E1[20]="iteration_statement";
static unsigned char _tmp2E2[15]="jump_statement";static unsigned char _tmp2E3[8]="pattern";
static unsigned char _tmp2E4[19]="tuple_pattern_list";static unsigned char _tmp2E5[
14]="field_pattern";static unsigned char _tmp2E6[19]="field_pattern_list";static
unsigned char _tmp2E7[11]="expression";static unsigned char _tmp2E8[22]="assignment_expression";
static unsigned char _tmp2E9[20]="assignment_operator";static unsigned char _tmp2EA[
23]="conditional_expression";static unsigned char _tmp2EB[20]="constant_expression";
static unsigned char _tmp2EC[22]="logical_or_expression";static unsigned char
_tmp2ED[23]="logical_and_expression";static unsigned char _tmp2EE[24]="inclusive_or_expression";
static unsigned char _tmp2EF[24]="exclusive_or_expression";static unsigned char
_tmp2F0[15]="and_expression";static unsigned char _tmp2F1[20]="equality_expression";
static unsigned char _tmp2F2[22]="relational_expression";static unsigned char
_tmp2F3[17]="shift_expression";static unsigned char _tmp2F4[20]="additive_expression";
static unsigned char _tmp2F5[26]="multiplicative_expression";static unsigned char
_tmp2F6[16]="cast_expression";static unsigned char _tmp2F7[17]="unary_expression";
static unsigned char _tmp2F8[15]="unary_operator";static unsigned char _tmp2F9[19]="postfix_expression";
static unsigned char _tmp2FA[19]="primary_expression";static unsigned char _tmp2FB[
25]="argument_expression_list";static unsigned char _tmp2FC[26]="argument_expression_list0";
static unsigned char _tmp2FD[9]="constant";static unsigned char _tmp2FE[20]="qual_opt_identifier";
static unsigned char _tmp2FF[17]="qual_opt_typedef";static unsigned char _tmp300[18]="struct_union_name";
static unsigned char _tmp301[11]="field_name";static unsigned char _tmp302[12]="right_angle";
static struct _tagged_arr Cyc_yytname[241]={{_tmp212,_tmp212,_tmp212 + 2},{_tmp213,
_tmp213,_tmp213 + 6},{_tmp214,_tmp214,_tmp214 + 12},{_tmp215,_tmp215,_tmp215 + 5},{
_tmp216,_tmp216,_tmp216 + 9},{_tmp217,_tmp217,_tmp217 + 7},{_tmp218,_tmp218,
_tmp218 + 7},{_tmp219,_tmp219,_tmp219 + 8},{_tmp21A,_tmp21A,_tmp21A + 5},{_tmp21B,
_tmp21B,_tmp21B + 5},{_tmp21C,_tmp21C,_tmp21C + 6},{_tmp21D,_tmp21D,_tmp21D + 4},{
_tmp21E,_tmp21E,_tmp21E + 5},{_tmp21F,_tmp21F,_tmp21F + 6},{_tmp220,_tmp220,
_tmp220 + 7},{_tmp221,_tmp221,_tmp221 + 7},{_tmp222,_tmp222,_tmp222 + 9},{_tmp223,
_tmp223,_tmp223 + 6},{_tmp224,_tmp224,_tmp224 + 9},{_tmp225,_tmp225,_tmp225 + 9},{
_tmp226,_tmp226,_tmp226 + 7},{_tmp227,_tmp227,_tmp227 + 6},{_tmp228,_tmp228,
_tmp228 + 5},{_tmp229,_tmp229,_tmp229 + 8},{_tmp22A,_tmp22A,_tmp22A + 7},{_tmp22B,
_tmp22B,_tmp22B + 7},{_tmp22C,_tmp22C,_tmp22C + 9},{_tmp22D,_tmp22D,_tmp22D + 3},{
_tmp22E,_tmp22E,_tmp22E + 5},{_tmp22F,_tmp22F,_tmp22F + 7},{_tmp230,_tmp230,
_tmp230 + 6},{_tmp231,_tmp231,_tmp231 + 3},{_tmp232,_tmp232,_tmp232 + 4},{_tmp233,
_tmp233,_tmp233 + 5},{_tmp234,_tmp234,_tmp234 + 9},{_tmp235,_tmp235,_tmp235 + 6},{
_tmp236,_tmp236,_tmp236 + 7},{_tmp237,_tmp237,_tmp237 + 5},{_tmp238,_tmp238,
_tmp238 + 8},{_tmp239,_tmp239,_tmp239 + 4},{_tmp23A,_tmp23A,_tmp23A + 6},{_tmp23B,
_tmp23B,_tmp23B + 4},{_tmp23C,_tmp23C,_tmp23C + 6},{_tmp23D,_tmp23D,_tmp23D + 4},{
_tmp23E,_tmp23E,_tmp23E + 9},{_tmp23F,_tmp23F,_tmp23F + 9},{_tmp240,_tmp240,
_tmp240 + 6},{_tmp241,_tmp241,_tmp241 + 10},{_tmp242,_tmp242,_tmp242 + 7},{_tmp243,
_tmp243,_tmp243 + 8},{_tmp244,_tmp244,_tmp244 + 9},{_tmp245,_tmp245,_tmp245 + 5},{
_tmp246,_tmp246,_tmp246 + 8},{_tmp247,_tmp247,_tmp247 + 4},{_tmp248,_tmp248,
_tmp248 + 7},{_tmp249,_tmp249,_tmp249 + 7},{_tmp24A,_tmp24A,_tmp24A + 8},{_tmp24B,
_tmp24B,_tmp24B + 7},{_tmp24C,_tmp24C,_tmp24C + 8},{_tmp24D,_tmp24D,_tmp24D + 9},{
_tmp24E,_tmp24E,_tmp24E + 9},{_tmp24F,_tmp24F,_tmp24F + 7},{_tmp250,_tmp250,
_tmp250 + 5},{_tmp251,_tmp251,_tmp251 + 8},{_tmp252,_tmp252,_tmp252 + 4},{_tmp253,
_tmp253,_tmp253 + 7},{_tmp254,_tmp254,_tmp254 + 7},{_tmp255,_tmp255,_tmp255 + 7},{
_tmp256,_tmp256,_tmp256 + 8},{_tmp257,_tmp257,_tmp257 + 9},{_tmp258,_tmp258,
_tmp258 + 6},{_tmp259,_tmp259,_tmp259 + 6},{_tmp25A,_tmp25A,_tmp25A + 6},{_tmp25B,
_tmp25B,_tmp25B + 6},{_tmp25C,_tmp25C,_tmp25C + 7},{_tmp25D,_tmp25D,_tmp25D + 6},{
_tmp25E,_tmp25E,_tmp25E + 11},{_tmp25F,_tmp25F,_tmp25F + 11},{_tmp260,_tmp260,
_tmp260 + 11},{_tmp261,_tmp261,_tmp261 + 11},{_tmp262,_tmp262,_tmp262 + 11},{
_tmp263,_tmp263,_tmp263 + 12},{_tmp264,_tmp264,_tmp264 + 13},{_tmp265,_tmp265,
_tmp265 + 11},{_tmp266,_tmp266,_tmp266 + 11},{_tmp267,_tmp267,_tmp267 + 10},{
_tmp268,_tmp268,_tmp268 + 9},{_tmp269,_tmp269,_tmp269 + 11},{_tmp26A,_tmp26A,
_tmp26A + 12},{_tmp26B,_tmp26B,_tmp26B + 11},{_tmp26C,_tmp26C,_tmp26C + 17},{
_tmp26D,_tmp26D,_tmp26D + 7},{_tmp26E,_tmp26E,_tmp26E + 19},{_tmp26F,_tmp26F,
_tmp26F + 18},{_tmp270,_tmp270,_tmp270 + 9},{_tmp271,_tmp271,_tmp271 + 13},{_tmp272,
_tmp272,_tmp272 + 16},{_tmp273,_tmp273,_tmp273 + 18},{_tmp274,_tmp274,_tmp274 + 10},{
_tmp275,_tmp275,_tmp275 + 4},{_tmp276,_tmp276,_tmp276 + 4},{_tmp277,_tmp277,
_tmp277 + 4},{_tmp278,_tmp278,_tmp278 + 4},{_tmp279,_tmp279,_tmp279 + 4},{_tmp27A,
_tmp27A,_tmp27A + 4},{_tmp27B,_tmp27B,_tmp27B + 4},{_tmp27C,_tmp27C,_tmp27C + 4},{
_tmp27D,_tmp27D,_tmp27D + 4},{_tmp27E,_tmp27E,_tmp27E + 4},{_tmp27F,_tmp27F,
_tmp27F + 4},{_tmp280,_tmp280,_tmp280 + 4},{_tmp281,_tmp281,_tmp281 + 4},{_tmp282,
_tmp282,_tmp282 + 4},{_tmp283,_tmp283,_tmp283 + 4},{_tmp284,_tmp284,_tmp284 + 4},{
_tmp285,_tmp285,_tmp285 + 4},{_tmp286,_tmp286,_tmp286 + 4},{_tmp287,_tmp287,
_tmp287 + 4},{_tmp288,_tmp288,_tmp288 + 4},{_tmp289,_tmp289,_tmp289 + 4},{_tmp28A,
_tmp28A,_tmp28A + 4},{_tmp28B,_tmp28B,_tmp28B + 4},{_tmp28C,_tmp28C,_tmp28C + 4},{
_tmp28D,_tmp28D,_tmp28D + 4},{_tmp28E,_tmp28E,_tmp28E + 4},{_tmp28F,_tmp28F,
_tmp28F + 4},{_tmp290,_tmp290,_tmp290 + 5},{_tmp291,_tmp291,_tmp291 + 17},{_tmp292,
_tmp292,_tmp292 + 21},{_tmp293,_tmp293,_tmp293 + 20},{_tmp294,_tmp294,_tmp294 + 21},{
_tmp295,_tmp295,_tmp295 + 13},{_tmp296,_tmp296,_tmp296 + 15},{_tmp297,_tmp297,
_tmp297 + 17},{_tmp298,_tmp298,_tmp298 + 19},{_tmp299,_tmp299,_tmp299 + 12},{
_tmp29A,_tmp29A,_tmp29A + 17},{_tmp29B,_tmp29B,_tmp29B + 23},{_tmp29C,_tmp29C,
_tmp29C + 24},{_tmp29D,_tmp29D,_tmp29D + 15},{_tmp29E,_tmp29E,_tmp29E + 11},{
_tmp29F,_tmp29F,_tmp29F + 15},{_tmp2A0,_tmp2A0,_tmp2A0 + 10},{_tmp2A1,_tmp2A1,
_tmp2A1 + 15},{_tmp2A2,_tmp2A2,_tmp2A2 + 5},{_tmp2A3,_tmp2A3,_tmp2A3 + 15},{_tmp2A4,
_tmp2A4,_tmp2A4 + 15},{_tmp2A5,_tmp2A5,_tmp2A5 + 11},{_tmp2A6,_tmp2A6,_tmp2A6 + 22},{
_tmp2A7,_tmp2A7,_tmp2A7 + 26},{_tmp2A8,_tmp2A8,_tmp2A8 + 15},{_tmp2A9,_tmp2A9,
_tmp2A9 + 16},{_tmp2AA,_tmp2AA,_tmp2AA + 16},{_tmp2AB,_tmp2AB,_tmp2AB + 24},{
_tmp2AC,_tmp2AC,_tmp2AC + 25},{_tmp2AD,_tmp2AD,_tmp2AD + 21},{_tmp2AE,_tmp2AE,
_tmp2AE + 22},{_tmp2AF,_tmp2AF,_tmp2AF + 16},{_tmp2B0,_tmp2B0,_tmp2B0 + 19},{
_tmp2B1,_tmp2B1,_tmp2B1 + 25},{_tmp2B2,_tmp2B2,_tmp2B2 + 23},{_tmp2B3,_tmp2B3,
_tmp2B3 + 24},{_tmp2B4,_tmp2B4,_tmp2B4 + 18},{_tmp2B5,_tmp2B5,_tmp2B5 + 17},{
_tmp2B6,_tmp2B6,_tmp2B6 + 18},{_tmp2B7,_tmp2B7,_tmp2B7 + 17},{_tmp2B8,_tmp2B8,
_tmp2B8 + 18},{_tmp2B9,_tmp2B9,_tmp2B9 + 12},{_tmp2BA,_tmp2BA,_tmp2BA + 11},{
_tmp2BB,_tmp2BB,_tmp2BB + 18},{_tmp2BC,_tmp2BC,_tmp2BC + 8},{_tmp2BD,_tmp2BD,
_tmp2BD + 13},{_tmp2BE,_tmp2BE,_tmp2BE + 8},{_tmp2BF,_tmp2BF,_tmp2BF + 4},{_tmp2C0,
_tmp2C0,_tmp2C0 + 20},{_tmp2C1,_tmp2C1,_tmp2C1 + 20},{_tmp2C2,_tmp2C2,_tmp2C2 + 9},{
_tmp2C3,_tmp2C3,_tmp2C3 + 16},{_tmp2C4,_tmp2C4,_tmp2C4 + 19},{_tmp2C5,_tmp2C5,
_tmp2C5 + 10},{_tmp2C6,_tmp2C6,_tmp2C6 + 16},{_tmp2C7,_tmp2C7,_tmp2C7 + 11},{
_tmp2C8,_tmp2C8,_tmp2C8 + 14},{_tmp2C9,_tmp2C9,_tmp2C9 + 11},{_tmp2CA,_tmp2CA,
_tmp2CA + 15},{_tmp2CB,_tmp2CB,_tmp2CB + 22},{_tmp2CC,_tmp2CC,_tmp2CC + 16},{
_tmp2CD,_tmp2CD,_tmp2CD + 17},{_tmp2CE,_tmp2CE,_tmp2CE + 12},{_tmp2CF,_tmp2CF,
_tmp2CF + 18},{_tmp2D0,_tmp2D0,_tmp2D0 + 17},{_tmp2D1,_tmp2D1,_tmp2D1 + 12},{
_tmp2D2,_tmp2D2,_tmp2D2 + 16},{_tmp2D3,_tmp2D3,_tmp2D3 + 11},{_tmp2D4,_tmp2D4,
_tmp2D4 + 10},{_tmp2D5,_tmp2D5,_tmp2D5 + 14},{_tmp2D6,_tmp2D6,_tmp2D6 + 15},{
_tmp2D7,_tmp2D7,_tmp2D7 + 20},{_tmp2D8,_tmp2D8,_tmp2D8 + 27},{_tmp2D9,_tmp2D9,
_tmp2D9 + 10},{_tmp2DA,_tmp2DA,_tmp2DA + 18},{_tmp2DB,_tmp2DB,_tmp2DB + 21},{
_tmp2DC,_tmp2DC,_tmp2DC + 19},{_tmp2DD,_tmp2DD,_tmp2DD + 16},{_tmp2DE,_tmp2DE,
_tmp2DE + 20},{_tmp2DF,_tmp2DF,_tmp2DF + 15},{_tmp2E0,_tmp2E0,_tmp2E0 + 16},{
_tmp2E1,_tmp2E1,_tmp2E1 + 20},{_tmp2E2,_tmp2E2,_tmp2E2 + 15},{_tmp2E3,_tmp2E3,
_tmp2E3 + 8},{_tmp2E4,_tmp2E4,_tmp2E4 + 19},{_tmp2E5,_tmp2E5,_tmp2E5 + 14},{_tmp2E6,
_tmp2E6,_tmp2E6 + 19},{_tmp2E7,_tmp2E7,_tmp2E7 + 11},{_tmp2E8,_tmp2E8,_tmp2E8 + 22},{
_tmp2E9,_tmp2E9,_tmp2E9 + 20},{_tmp2EA,_tmp2EA,_tmp2EA + 23},{_tmp2EB,_tmp2EB,
_tmp2EB + 20},{_tmp2EC,_tmp2EC,_tmp2EC + 22},{_tmp2ED,_tmp2ED,_tmp2ED + 23},{
_tmp2EE,_tmp2EE,_tmp2EE + 24},{_tmp2EF,_tmp2EF,_tmp2EF + 24},{_tmp2F0,_tmp2F0,
_tmp2F0 + 15},{_tmp2F1,_tmp2F1,_tmp2F1 + 20},{_tmp2F2,_tmp2F2,_tmp2F2 + 22},{
_tmp2F3,_tmp2F3,_tmp2F3 + 17},{_tmp2F4,_tmp2F4,_tmp2F4 + 20},{_tmp2F5,_tmp2F5,
_tmp2F5 + 26},{_tmp2F6,_tmp2F6,_tmp2F6 + 16},{_tmp2F7,_tmp2F7,_tmp2F7 + 17},{
_tmp2F8,_tmp2F8,_tmp2F8 + 15},{_tmp2F9,_tmp2F9,_tmp2F9 + 19},{_tmp2FA,_tmp2FA,
_tmp2FA + 19},{_tmp2FB,_tmp2FB,_tmp2FB + 25},{_tmp2FC,_tmp2FC,_tmp2FC + 26},{
_tmp2FD,_tmp2FD,_tmp2FD + 9},{_tmp2FE,_tmp2FE,_tmp2FE + 20},{_tmp2FF,_tmp2FF,
_tmp2FF + 17},{_tmp300,_tmp300,_tmp300 + 18},{_tmp301,_tmp301,_tmp301 + 11},{
_tmp302,_tmp302,_tmp302 + 12}};static short Cyc_yyr1[408]={0,126,127,127,127,127,
127,127,127,128,128,129,129,129,129,130,130,131,132,133,134,135,135,135,135,136,
136,137,137,137,137,137,137,137,137,138,138,138,138,138,138,138,139,139,140,141,
141,142,142,142,142,142,143,143,143,143,143,143,143,143,143,143,143,143,143,143,
143,143,143,143,144,145,145,145,146,146,146,147,147,148,148,149,149,149,150,150,
151,151,152,152,153,154,154,155,156,156,157,157,158,159,159,159,159,160,161,161,
162,162,162,163,163,163,163,164,164,165,165,165,165,166,166,166,167,167,168,168,
169,169,169,169,169,169,169,169,169,169,169,170,170,170,170,171,171,171,171,171,
172,172,173,173,174,174,175,175,175,175,176,176,177,177,178,178,179,179,180,180,
181,181,182,182,182,182,183,183,184,184,185,185,185,186,187,187,188,188,189,189,
189,189,190,190,190,190,191,192,192,193,193,194,194,195,195,195,195,195,196,196,
197,197,197,198,198,198,198,198,198,198,198,198,198,198,199,199,199,199,199,199,
199,199,199,199,200,201,201,202,202,203,203,203,203,203,203,204,204,204,204,204,
205,205,205,205,205,205,206,206,206,206,207,207,207,207,207,207,207,207,207,207,
207,207,207,207,207,208,208,208,208,208,208,208,208,209,209,209,209,209,209,209,
209,209,209,209,209,209,210,210,211,211,212,212,213,213,214,214,215,215,215,215,
215,215,215,215,215,215,215,216,216,216,216,216,216,216,217,218,218,219,219,220,
220,221,221,222,222,223,223,223,224,224,224,224,224,225,225,225,226,226,226,227,
227,227,227,228,228,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,
229,230,230,230,231,231,231,231,231,231,231,231,231,231,231,231,232,232,232,232,
232,232,232,232,232,233,234,234,235,235,235,235,236,236,237,237,238,238,239,239,
240,240};static short Cyc_yyr2[408]={0,1,2,3,5,3,5,6,0,1,1,2,3,3,4,3,4,2,1,2,1,2,3,
5,3,1,2,2,3,2,3,2,3,2,3,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,8,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,2,4,4,4,1,1,1,1,5,2,4,1,3,1,3,4,7,3,0,3,0,3,1,1,1,1,2,1,1,3,1,3,3,2,3,2,3,
1,1,3,1,2,3,6,4,3,5,1,1,1,2,3,3,0,1,1,2,6,1,2,1,3,3,4,4,5,4,4,2,2,1,3,4,4,5,1,1,
4,4,1,0,1,1,1,1,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,
1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,2,3,3,4,4,3,5,4,4,2,1,1,1,
1,1,1,6,3,2,2,3,1,2,2,3,1,2,1,2,1,2,5,7,7,8,6,0,3,4,5,6,7,0,3,4,5,5,7,6,7,7,8,7,
8,8,9,6,7,7,8,9,3,2,2,2,3,2,4,5,1,3,1,2,1,1,1,1,4,5,5,2,2,1,3,1,2,1,3,1,3,1,3,1,
1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,
3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,1,1,1,1,4,3,4,3,3,2,2,6,7,4,4,1,
1,1,3,2,5,4,4,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[830]={
0,136,35,36,37,38,40,52,54,55,56,57,58,59,60,61,71,72,73,88,89,42,0,0,41,0,0,113,
114,0,0,398,156,400,399,401,0,53,0,141,142,145,1,0,9,0,0,10,0,42,42,42,62,63,0,
64,0,0,124,0,146,65,126,86,39,0,33,43,0,75,285,398,281,284,283,0,279,0,0,0,0,0,
179,0,286,17,19,0,0,0,0,0,0,0,2,0,0,0,0,21,0,93,94,96,27,29,31,0,402,403,86,149,
0,148,86,38,0,25,0,0,11,158,0,0,134,125,42,147,135,0,66,0,0,34,79,0,77,0,0,0,291,
282,290,24,0,0,84,0,0,0,42,42,197,199,0,0,404,405,157,70,127,0,0,174,0,0,397,0,0,
0,0,0,0,0,0,0,86,0,0,394,384,395,396,0,0,0,0,369,0,367,368,0,300,313,321,323,325,
327,329,331,334,339,342,345,349,0,351,370,383,382,0,3,0,5,0,22,0,0,0,12,28,30,32,
0,90,91,0,83,86,0,111,39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,398,231,233,0,239,235,0,
237,220,221,222,0,223,224,225,0,298,26,13,96,164,180,0,0,160,158,0,204,0,128,0,
137,0,0,0,0,76,0,0,280,292,0,181,0,0,0,0,0,200,172,0,99,101,158,0,206,198,207,
407,406,0,68,69,0,176,0,178,67,0,0,359,0,315,349,0,316,317,0,0,0,0,0,0,0,0,0,352,
353,0,0,0,0,355,356,354,143,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,303,304,305,
306,307,308,309,310,311,312,302,0,357,0,376,377,0,0,0,386,0,0,144,18,0,20,0,95,
97,183,182,14,81,92,0,0,103,104,106,84,110,86,119,0,0,0,0,0,0,0,272,273,274,0,0,
276,0,0,228,229,0,0,0,0,240,236,96,238,234,232,0,165,0,0,0,171,159,166,130,0,0,0,
160,132,0,133,129,150,139,138,87,0,48,47,0,45,80,320,78,74,287,0,23,0,0,0,0,0,
193,294,296,0,0,0,0,201,100,102,0,160,0,210,0,208,158,0,0,219,203,175,0,0,0,184,
188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,385,392,0,391,322,0,324,326,328,330,332,333,337,
338,335,336,340,341,343,344,346,347,348,301,375,372,0,374,0,0,0,4,6,107,98,0,0,0,
112,121,120,0,0,115,0,0,0,0,0,0,0,0,271,275,0,0,0,0,227,0,230,0,15,299,158,0,168,
0,0,0,161,131,164,152,205,151,140,7,0,0,0,293,85,196,0,295,192,194,289,0,288,202,
173,215,0,209,212,0,160,0,211,0,358,0,0,185,0,189,380,381,0,363,0,0,0,0,0,0,0,0,
350,388,0,0,373,371,389,0,0,105,108,0,109,122,119,119,0,0,0,0,0,0,0,0,0,0,246,
277,0,0,0,16,160,0,169,167,0,160,0,0,0,0,44,46,195,297,214,217,0,218,213,0,0,0,
186,190,0,0,0,0,318,319,362,390,0,393,314,387,82,0,118,117,241,0,246,256,0,0,0,0,
0,0,0,0,0,0,0,0,278,0,0,154,170,162,153,158,0,49,50,216,361,360,0,191,364,0,0,
378,0,0,0,252,0,0,258,0,0,0,266,0,0,0,0,0,0,0,0,245,0,226,0,160,0,0,0,0,379,0,
242,0,0,0,243,257,259,260,0,268,267,0,262,0,0,0,0,246,247,0,163,155,0,0,0,0,123,
0,0,244,261,269,263,264,0,0,246,248,0,0,187,365,0,252,253,265,246,249,270,51,0,
252,254,246,250,366,255,251,0,0,0};static short Cyc_yydefgoto[115]={827,42,43,44,
251,45,386,46,388,47,218,48,49,66,67,456,457,50,158,51,52,134,135,53,292,130,54,
223,224,100,101,102,225,152,397,398,399,55,56,552,553,554,57,58,59,60,126,112,
451,480,61,481,442,581,434,438,439,297,272,163,81,82,496,391,497,498,469,470,153,
274,275,482,304,254,255,256,257,258,259,718,772,260,261,287,288,472,473,262,263,
373,192,460,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,513,514,
208,209,63,110,159,308};static short Cyc_yypact[830]={2025,- -32768,- -32768,- -32768,
- -32768,- 36,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 14,191,847,- -32768,91,78,
- -32768,- -32768,74,99,- -32768,133,- -32768,- -32768,- -32768,482,- -32768,171,138,
184,- -32768,- -32768,1908,- -32768,19,216,- -32768,422,- 14,- 14,- 14,- -32768,- -32768,
308,- -32768,146,2377,205,39,- 74,- -32768,91,187,198,202,2502,- -32768,91,228,-
-32768,103,- -32768,- -32768,- -32768,2438,- -32768,255,304,312,2438,314,306,348,159,
- -32768,- -32768,4699,4699,207,349,4870,4328,4328,- -32768,1908,2141,1908,2141,-
-32768,356,355,- -32768,672,2502,2502,2502,4870,- -32768,- -32768,187,- -32768,91,-
-32768,35,367,1300,- -32768,2377,422,- -32768,1844,4699,3000,- -32768,205,- 14,-
-32768,- -32768,4699,- -32768,2141,362,- -32768,373,387,397,91,401,2438,- -32768,-
-32768,- -32768,- -32768,421,4328,412,430,436,150,- 14,- 14,101,- -32768,61,61,-
-32768,- -32768,- -32768,- -32768,- -32768,388,392,- -32768,4486,441,- -32768,4328,775,
443,452,454,456,462,472,474,187,4561,4561,- -32768,- -32768,- -32768,- -32768,1664,
476,4636,4636,- -32768,4636,- -32768,- -32768,480,- -32768,- 17,509,466,478,502,450,
83,463,423,131,- -32768,1029,4636,256,- -32768,- -32768,96,492,- -32768,494,- -32768,
500,- -32768,482,3083,2377,- -32768,- -32768,- -32768,- -32768,533,4870,- -32768,526,
503,187,91,528,- -32768,534,34,536,2585,546,562,557,563,3166,2585,273,571,2585,
2585,13,556,- -32768,- -32768,596,1423,1423,422,1423,- -32768,- -32768,- -32768,569,-
-32768,- -32768,- -32768,121,- -32768,- -32768,- -32768,601,616,- -32768,50,604,600,
264,615,611,63,- -32768,626,141,63,640,22,91,- -32768,4328,644,- -32768,- -32768,447,
- -32768,274,4699,2316,2438,4699,- -32768,641,646,4870,4870,909,3249,120,- -32768,
226,- -32768,- -32768,50,- -32768,- -32768,2263,- -32768,549,- -32768,- -32768,4870,
1664,- -32768,4870,- -32768,- -32768,2668,- -32768,673,4328,2202,4328,4328,4328,4328,
4328,647,1664,- -32768,- -32768,1423,645,458,4328,- -32768,- -32768,- -32768,- -32768,
4636,4328,4636,4636,4636,4636,4636,4636,4636,4636,4636,4636,4636,4636,4636,4636,
4636,4636,4636,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,4328,- -32768,207,- -32768,- -32768,3332,207,4328,- -32768,
2917,648,- -32768,- -32768,1908,- -32768,1908,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,4328,654,652,- -32768,651,412,- -32768,187,567,4328,660,
4328,4328,734,1546,671,- -32768,- -32768,- -32768,293,729,- -32768,3415,2441,- -32768,
- -32768,2585,681,2585,1782,- -32768,- -32768,672,- -32768,- -32768,- -32768,4328,-
-32768,4870,674,185,- -32768,- -32768,659,- -32768,686,677,4770,600,- -32768,4699,-
-32768,- -32768,- -32768,- -32768,141,- -32768,1908,- -32768,679,680,678,- -32768,-
-32768,- -32768,- -32768,- -32768,2438,- -32768,322,207,4328,2438,73,- -32768,- -32768,
- -32768,334,484,- 83,691,- -32768,- -32768,- -32768,682,600,683,- -32768,676,226,4755,
4699,3498,- -32768,- -32768,- -32768,687,685,703,- -32768,- -32768,340,3083,506,689,
482,508,692,697,698,512,4870,690,695,4403,- -32768,- -32768,700,702,509,220,466,
478,502,450,83,83,463,463,463,463,423,423,131,131,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,701,- -32768,206,384,4699,- -32768,- -32768,- -32768,- -32768,
526,4328,4870,- -32768,- -32768,- -32768,713,91,302,515,4328,520,522,715,3581,3664,
307,- -32768,- -32768,720,722,719,725,- -32768,726,- -32768,2377,- -32768,- -32768,730,
4699,- -32768,727,50,732,- -32768,- -32768,52,- -32768,611,- -32768,- -32768,- -32768,
353,733,22,- -32768,- -32768,- -32768,721,- -32768,- -32768,- -32768,- -32768,2316,-
-32768,- -32768,- -32768,- -32768,739,- -32768,- -32768,740,600,63,- -32768,735,738,
252,741,- -32768,2751,- -32768,- -32768,- -32768,2377,- -32768,4328,821,4328,775,743,
738,748,2917,- -32768,- -32768,4328,4328,- -32768,- -32768,- -32768,2917,63,- -32768,-
-32768,752,- -32768,243,72,567,2585,527,756,2585,4328,3747,311,3830,315,3913,621,
- -32768,759,4328,770,- -32768,600,102,- -32768,- -32768,766,600,4870,757,765,768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,772,- -32768,- -32768,773,776,4328,
- -32768,- -32768,3083,542,758,774,- -32768,673,- -32768,- -32768,411,- -32768,- -32768,
- -32768,- -32768,760,- -32768,- -32768,842,783,621,- -32768,560,2585,564,3996,2585,
568,4079,4162,316,2438,778,789,- -32768,327,2585,- -32768,- -32768,786,- -32768,730,
805,- -32768,- -32768,- -32768,- -32768,- -32768,245,- -32768,- -32768,4870,871,- -32768,
2834,4870,2585,675,796,799,- -32768,2585,2585,590,- -32768,2585,2585,597,2585,608,
4245,66,1423,- -32768,4328,- -32768,686,600,798,4328,801,806,- -32768,610,- -32768,
4328,802,807,- -32768,- -32768,- -32768,- -32768,2585,- -32768,- -32768,2585,- -32768,
2585,2585,613,4328,1033,- -32768,618,- -32768,- -32768,823,429,810,4870,- -32768,280,
1423,- -32768,- -32768,- -32768,- -32768,- -32768,2585,324,621,- -32768,2585,812,-
-32768,- -32768,827,1177,- -32768,- -32768,1033,- -32768,- -32768,- -32768,828,675,-
-32768,621,- -32768,- -32768,- -32768,- -32768,933,935,- -32768};static short Cyc_yypgoto[
115]={- -32768,- 2,- -32768,617,- -32768,- -32768,- -32768,- -32768,- -32768,- 53,- 40,- 57,
- -32768,398,- 15,347,- -32768,- 6,- -32768,- 69,- -32768,- -32768,- 101,- -32768,540,- 42,
- -32768,396,- -32768,- -32768,- -32768,731,724,- 34,- -32768,- -32768,399,- -32768,-
-32768,81,- -32768,- -32768,44,- 43,- 107,- -32768,- -32768,889,- -32768,- 106,30,- 108,-
332,190,369,376,649,- 367,- 90,- 286,838,- -32768,- 192,- 158,- 355,- 257,- -32768,493,-
180,- 82,- 117,87,- 102,104,- -32768,- -32768,- 29,- 228,- -32768,- 572,- 480,- -32768,-
-32768,- 9,704,370,- -32768,250,135,- -32768,- 160,- 359,- 157,624,627,625,628,650,386,
253,382,385,- 88,631,- -32768,- -32768,- -32768,- 287,- -32768,- -32768,8,919,- -32768,-
300,- 153};static short Cyc_yytable[4978]={119,162,309,336,117,154,155,319,62,133,
322,323,279,271,83,270,125,118,151,151,32,602,151,426,427,390,429,540,120,491,69,
84,111,85,307,468,285,544,151,454,1,94,147,124,62,302,119,220,221,222,117,62,151,
151,312,64,62,161,343,253,151,119,108,252,114,264,138,62,227,578,128,142,230,226,
219,535,136,550,551,538,90,150,150,84,65,150,113,161,84,265,113,537,103,211,212,
213,214,339,340,344,341,150,422,62,62,62,62,509,595,603,124,455,584,435,151,150,
150,374,95,96,228,423,447,150,459,406,452,62,31,280,305,567,305,743,331,34,492,
407,493,62,785,433,36,129,32,136,229,84,575,605,436,269,508,351,352,151,- 158,491,
16,17,18,119,- 158,266,444,264,594,86,446,62,306,450,306,- 116,465,597,786,150,307,
296,31,458,87,381,466,467,402,34,641,393,226,353,354,302,253,253,382,253,252,252,
485,252,- 180,312,300,311,723,88,- 180,449,485,383,475,301,806,39,40,41,150,307,
431,89,151,300,62,151,432,191,210,151,151,151,301,816,62,31,459,403,92,303,32,
151,34,823,32,359,151,151,313,151,826,295,111,360,361,39,40,41,277,146,266,62,-
86,151,478,479,161,129,501,125,400,531,532,533,91,695,161,679,253,32,31,161,252,
471,93,150,577,34,150,489,136,68,150,150,150,129,156,428,131,437,84,84,157,65,
150,132,618,459,121,150,150,432,150,122,683,97,98,123,62,636,62,375,376,377,65,
432,150,627,137,486,634,722,821,62,487,162,725,437,488,409,825,156,682,468,587,
416,- 86,157,420,421,432,129,392,119,90,764,151,561,139,378,687,549,119,269,585,
151,568,379,380,443,610,119,417,464,151,117,418,151,609,432,608,687,586,726,542,
432,543,459,572,1,812,161,564,140,62,290,62,31,432,573,161,645,141,33,34,35,656,
646,107,161,710,144,432,143,713,755,432,151,151,150,432,432,631,1,639,686,759,
446,150,432,790,593,432,337,815,599,484,150,151,600,150,616,670,671,672,617,686,
104,105,106,145,588,161,160,592,215,392,680,231,596,216,62,503,504,505,281,296,
663,691,692,489,84,151,512,697,668,84,31,282,151,150,150,687,1,34,637,698,- 177,
283,638,415,310,- 177,- 177,664,734,314,315,- 177,284,301,150,39,40,41,286,296,151,
534,62,289,31,738,512,226,119,739,392,34,264,291,99,349,350,278,36,569,1,571,787,
809,355,356,293,432,150,39,40,41,294,357,358,150,662,318,621,324,686,298,299,1,
462,463,512,62,325,765,326,805,327,512,644,511,432,119,328,337,574,117,813,150,
31,550,551,499,329,502,330,34,338,506,342,337,345,820,36,346,822,601,463,400,471,
219,384,516,385,39,40,41,347,151,387,700,401,523,524,525,526,84,437,619,432,622,
432,811,31,626,432,762,647,432,348,34,612,649,432,650,432,404,36,539,704,432,392,
394,161,396,405,31,408,39,40,41,716,717,34,735,432,- 206,410,768,411,310,- 206,-
206,555,412,557,558,- 206,562,301,413,150,744,432,424,151,746,432,430,151,750,432,
419,2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,777,432,21,770,771,425,
253,780,432,217,252,433,161,756,440,22,441,23,782,432,795,315,24,803,432,445,27,
28,807,432,84,151,701,702,307,253,150,29,30,252,150,521,522,527,528,448,253,453,
529,530,252,461,476,477,343,510,507,703,392,545,706,253,541,546,253,252,690,547,
252,556,559,392,32,33,696,35,563,565,116,392,217,570,579,576,37,38,580,582,589,
591,590,32,604,606,150,607,614,613,615,620,628,317,629,623,320,320,164,165,624,
625,632,635,648,633,333,334,653,655,745,166,643,749,320,320,651,320,657,658,392,
659,660,760,169,170,665,269,171,172,173,174,675,661,320,673,630,176,667,177,178,
677,678,769,689,693,681,684,775,776,694,699,778,779,705,781,719,721,724,736,727,
740,31,179,180,181,182,728,741,34,729,688,392,321,730,731,183,737,732,799,184,
742,800,70,801,802,757,185,758,761,186,187,188,763,766,773,774,189,190,707,709,
791,712,793,715,814,798,794,720,817,797,808,810,320,818,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,819,824,828,733,829,71,72,674,73,74,548,500,34,642,640,22,389,395,
127,75,789,669,76,77,666,490,27,28,273,748,78,598,752,754,79,80,515,29,30,676,
518,517,109,320,519,320,320,320,320,320,320,320,320,320,320,320,320,320,320,320,
320,320,0,0,267,0,474,520,0,0,0,0,32,33,784,35,0,269,788,0,0,300,0,792,37,38,0,0,
0,796,301,0,39,40,41,0,320,0,0,0,0,0,0,0,804,2,3,4,115,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,716,717,21,164,165,232,0,233,234,235,236,237,238,239,240,22,166,
23,167,241,0,168,24,242,0,0,27,28,243,169,170,244,245,171,172,173,174,29,30,246,
175,0,176,320,177,178,0,0,0,0,362,363,364,365,366,367,368,369,370,371,0,0,0,0,0,
0,0,247,179,180,181,182,32,33,34,35,372,248,116,0,0,183,0,0,37,250,320,0,0,0,0,0,
185,0,0,186,187,188,0,0,0,0,189,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,320,0,
2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,770,771,21,164,165,232,0,233,
234,235,236,237,238,239,240,22,166,23,167,241,0,168,24,242,0,0,27,28,243,169,170,
244,245,171,172,173,174,29,30,246,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,
320,0,0,0,0,0,0,0,320,247,179,180,181,182,32,33,34,35,0,248,116,0,0,183,0,0,37,
250,0,0,0,0,0,0,185,0,0,186,187,188,0,0,0,0,189,190,2,3,4,115,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,164,165,232,0,233,234,235,236,237,238,239,240,22,
166,23,167,241,0,168,24,242,0,0,27,28,243,169,170,244,245,171,172,173,174,29,30,
246,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,247,179,180,
181,182,32,33,34,35,0,248,116,249,0,183,0,0,37,250,0,0,0,0,0,0,185,0,0,186,187,
188,0,0,0,0,189,190,2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,
164,165,232,0,233,234,235,236,237,238,239,240,22,166,23,167,241,0,168,24,242,0,0,
27,28,243,169,170,244,245,171,172,173,174,29,30,246,175,0,176,0,177,178,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,247,179,180,181,182,32,33,34,35,0,248,116,0,0,
183,0,0,37,250,0,0,0,0,0,0,185,0,0,186,187,188,0,0,0,0,189,190,2,3,4,115,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,164,165,0,0,0,0,0,0,0,0,0,0,22,166,23,
167,0,0,168,24,0,0,0,27,28,0,169,170,0,0,171,172,173,174,29,30,0,175,0,176,0,177,
178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,32,33,34,35,0,
560,0,0,0,183,0,0,37,250,0,0,0,0,0,0,185,0,0,186,187,188,0,0,0,0,189,190,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,0,164,165,0,0,0,0,0,0,0,0,0,0,22,166,0,167,
0,0,168,0,0,0,0,27,28,0,169,170,0,0,171,172,173,174,29,30,0,175,0,176,0,177,178,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,32,33,34,35,0,0,335,
0,0,183,0,0,37,250,0,0,0,0,0,0,185,0,0,186,187,188,0,0,0,0,189,190,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,0,164,165,0,0,0,0,0,0,0,0,0,0,22,166,0,167,0,0,
168,0,0,0,0,27,28,0,169,170,0,0,171,172,173,174,29,30,0,175,0,176,0,177,178,0,0,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,31,179,180,181,182,32,33,34,35,
0,22,0,0,0,183,0,0,37,250,0,0,27,28,0,0,185,0,0,186,187,188,0,29,30,0,189,190,- 8,
1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,267,0,21,268,0,0,0,0,32,33,
0,35,0,269,0,22,0,23,0,0,37,38,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,32,33,34,35,0,0,0,- 8,
0,36,0,0,37,38,0,0,0,0,0,0,39,40,41,- 8,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,
0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,
0,0,0,32,33,34,35,0,0,0,0,0,36,0,0,37,38,0,0,0,0,0,0,39,40,41,1,0,2,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,
0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,0,1,0,2,3,4,115,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,21,0,0,0,31,0,0,0,0,32,33,34,35,22,0,0,- 8,0,36,0,24,37,
38,0,27,28,0,0,0,39,40,41,0,0,0,29,30,0,1,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,0,0,0,31,0,0,0,0,32,33,34,35,22,0,0,0,0,36,0,0,37,38,0,27,28,0,
0,0,39,40,41,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,267,0,
0,31,0,70,0,0,32,33,34,35,0,269,0,0,0,310,- 158,0,37,38,0,0,- 158,0,301,0,39,40,41,
2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,31,72,0,73,74,0,
0,34,0,22,0,23,0,0,75,0,24,76,77,0,27,28,466,467,0,78,0,0,0,79,80,29,30,0,0,0,0,
0,0,2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,32,33,0,
35,0,70,116,22,0,23,0,0,37,38,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,2,3,4,
115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,31,72,0,73,74,0,0,34,32,33,0,
35,22,0,75,0,0,76,77,24,37,38,0,27,28,78,0,0,0,79,80,0,0,0,29,30,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,33,0,35,0,0,0,0,0,0,0,0,37,
38,164,165,232,0,233,234,235,236,237,238,239,240,0,166,0,167,241,0,168,0,242,0,0,
0,0,243,169,170,244,245,171,172,173,174,0,0,246,175,0,176,0,177,178,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,247,179,180,181,182,0,0,34,0,0,248,116,0,0,183,0,0,
0,184,164,165,0,0,0,0,185,494,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,
0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,321,495,0,183,0,0,0,184,164,
165,0,466,467,0,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,
170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,321,685,0,183,0,0,0,184,164,165,0,
466,467,0,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,
171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,31,179,180,181,182,0,0,34,0,0,0,321,767,0,183,0,0,0,184,164,165,0,466,467,0,
185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,
173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,
179,180,181,182,0,0,34,0,0,0,321,0,0,183,0,0,0,184,164,165,0,466,467,0,185,0,0,
186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,
0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,
182,0,0,34,0,0,0,0,0,0,183,0,0,0,184,164,165,0,0,0,276,185,0,0,186,187,188,0,166,
0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,
177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,
0,321,0,0,183,0,0,0,184,164,165,0,0,0,0,185,0,0,186,187,188,0,166,0,167,189,190,
168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,414,0,0,0,183,0,
0,0,184,164,165,0,0,0,0,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,
0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,0,0,0,183,0,0,0,184,164,165,0,
0,0,483,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,
171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,31,179,180,181,182,0,0,34,0,0,0,0,0,0,183,536,0,0,184,164,165,0,0,0,0,185,0,
0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,
0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,
181,182,0,0,34,0,0,0,0,0,0,183,566,0,0,184,164,165,0,0,0,0,185,0,0,186,187,188,0,
166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,
0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,
0,0,0,0,0,183,0,0,0,184,164,165,0,0,0,611,185,0,0,186,187,188,0,166,0,167,189,
190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,652,0,0,0,
183,0,0,0,184,164,165,0,0,0,0,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,
0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,654,0,0,0,183,0,0,0,184,
164,165,0,0,0,0,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,
170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,0,0,0,183,708,0,0,184,164,165,0,0,0,
0,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,
173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,
179,180,181,182,0,0,34,0,0,0,0,0,0,183,711,0,0,184,164,165,0,0,0,0,185,0,0,186,
187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,
175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,
0,0,34,0,0,714,0,0,0,183,0,0,0,184,164,165,0,0,0,0,185,0,0,186,187,188,0,166,0,
167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,
178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,0,
0,0,183,747,0,0,184,164,165,0,0,0,0,185,0,0,186,187,188,0,166,0,167,189,190,168,
0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,0,0,0,183,751,0,0,
184,164,165,0,0,0,0,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,
169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,0,0,0,183,753,0,0,184,164,165,0,
0,0,0,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,
172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
31,179,180,181,182,0,0,34,0,0,0,0,0,0,183,783,0,0,184,164,165,0,0,0,0,185,0,0,
186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,
0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,
182,0,0,34,0,0,0,164,165,0,183,0,0,0,184,0,0,0,0,0,166,185,0,0,186,187,188,0,0,0,
0,189,190,169,170,0,0,171,172,173,174,0,0,0,0,0,176,0,177,178,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,630,0,0,183,0,0,0,184,
164,165,0,0,0,0,185,0,0,186,187,188,0,166,0,0,189,190,0,0,0,0,0,0,0,0,169,170,0,
0,171,172,173,174,0,0,0,0,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,31,179,180,181,182,0,0,34,0,0,0,164,165,0,316,0,0,0,184,0,0,0,0,0,166,185,0,
0,186,187,188,0,0,0,0,189,190,169,170,0,0,171,172,173,174,0,0,0,0,0,176,0,177,
178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,
164,165,0,332,0,0,0,184,0,0,0,0,0,166,185,0,0,186,187,188,0,0,0,0,189,190,169,
170,0,0,171,172,173,174,0,0,0,0,0,176,0,177,178,0,0,0,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,22,0,0,183,0,0,0,184,0,0,0,
27,28,0,185,0,0,186,187,188,0,0,29,30,189,190,148,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,32,33,0,35,0,0,149,0,0,0,27,
28,37,38,22,0,0,0,0,0,0,29,30,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,0,0,
0,0,267,0,0,0,0,0,0,0,32,33,0,35,0,269,0,583,0,0,0,0,37,38,0,32,33,0,35,0,0,0,0,
0,0,0,0,37,38,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,33,0,35,0,0,0,0,0,0,0,0,37,38};static
short Cyc_yycheck[4978]={57,91,155,183,57,87,88,167,0,66,168,168,129,121,23,121,
59,57,87,88,94,104,91,251,252,217,254,382,57,315,22,23,106,25,117,292,137,396,
107,17,1,43,84,58,36,152,103,104,105,106,103,43,121,122,161,91,48,91,75,116,129,
118,54,116,56,118,75,59,110,436,62,80,114,107,103,375,68,5,6,379,36,87,88,75,98,
91,56,121,80,118,60,378,48,95,96,97,98,185,186,116,188,107,89,95,96,97,98,335,
467,476,125,89,444,63,183,121,122,205,99,100,112,108,275,129,284,91,279,119,89,
131,69,418,69,705,176,96,316,103,318,131,74,89,103,108,94,137,111,139,434,481,
100,99,332,70,71,224,104,443,17,18,19,218,110,119,272,218,466,89,105,161,109,278,
109,101,291,102,110,183,117,149,89,282,108,87,111,112,228,96,547,218,224,108,109,
300,251,252,100,254,251,252,302,254,99,310,103,161,104,108,105,278,312,115,294,
112,786,114,115,116,224,117,99,88,291,103,216,294,105,92,93,298,299,300,112,805,
226,89,396,229,100,152,94,310,96,815,94,114,315,316,161,318,822,101,106,122,123,
114,115,116,123,100,216,253,103,332,298,299,300,108,325,312,226,359,360,361,103,
630,310,609,335,94,89,315,335,292,100,291,101,96,294,304,282,100,298,299,300,108,
89,253,100,269,292,293,95,98,310,103,498,467,103,315,316,105,318,108,614,99,100,
112,310,113,312,65,66,67,98,105,332,507,100,103,110,663,812,325,108,425,668,307,
112,235,820,89,90,600,451,241,103,95,244,245,105,108,217,410,310,110,425,410,103,
103,617,403,419,99,446,434,419,111,112,105,487,428,99,99,443,428,103,446,486,105,
486,638,451,669,386,105,388,547,428,1,110,425,99,89,386,145,388,89,105,428,434,
99,90,95,96,97,99,105,100,443,99,105,105,99,99,99,105,486,487,425,105,105,510,1,
541,617,99,105,434,105,762,109,105,183,110,101,301,443,507,105,446,101,89,90,91,
105,638,49,50,51,102,453,486,104,463,99,321,610,91,468,105,453,327,328,329,103,
436,575,626,626,485,463,541,338,634,583,468,89,105,548,486,487,739,1,96,101,639,
99,101,105,240,103,104,105,576,687,104,105,110,102,112,507,114,115,116,104,476,
576,373,501,89,89,101,378,548,572,105,382,96,572,108,99,72,73,126,103,422,1,424,
757,101,68,69,103,105,541,114,115,116,103,117,118,548,572,103,501,103,739,150,
151,1,104,105,418,546,103,736,103,786,103,425,553,104,105,621,103,316,432,621,
797,576,89,5,6,324,103,326,103,96,103,330,101,332,74,812,103,120,815,104,105,546,
600,621,101,344,101,114,115,116,121,669,101,644,100,351,352,353,354,600,579,104,
105,104,105,794,89,104,105,726,104,105,119,96,488,104,105,104,105,100,103,380,
104,105,498,101,669,110,103,89,103,114,115,116,22,23,96,104,105,99,103,740,89,
103,104,105,405,99,407,408,110,410,112,99,669,104,105,110,736,104,105,101,740,
104,105,103,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,104,105,24,22,23,
103,757,104,105,102,757,89,740,716,104,37,110,39,104,105,104,105,44,104,105,104,
48,49,104,105,716,794,645,646,117,786,736,59,60,786,740,349,350,355,356,113,797,
101,357,358,797,101,105,101,75,104,103,647,617,99,650,812,108,105,815,812,625,
110,815,103,30,630,94,95,633,97,99,42,100,638,102,94,117,103,106,107,94,104,103,
105,104,94,104,104,794,113,105,104,89,104,104,164,101,105,167,168,25,26,105,105,
104,104,556,105,177,178,560,561,708,38,101,711,185,186,103,188,100,99,687,104,99,
721,51,52,101,99,55,56,57,58,113,109,205,104,100,64,108,66,67,104,104,741,25,104,
113,108,746,747,104,101,750,751,100,753,99,89,94,103,105,103,89,90,91,92,93,104,
28,96,104,623,739,100,104,104,103,105,104,777,107,100,780,38,782,783,110,114,101,
105,117,118,119,90,25,101,99,124,125,651,652,105,654,104,656,803,101,103,660,807,
110,90,104,284,104,8,9,10,11,12,13,14,15,16,17,18,19,20,21,104,104,0,684,0,89,90,
591,92,93,401,325,96,548,546,37,216,224,60,103,761,583,106,107,579,307,48,49,121,
710,114,469,713,714,118,119,343,59,60,600,346,345,54,343,347,345,346,347,348,349,
350,351,352,353,354,355,356,357,358,359,360,361,- 1,- 1,86,- 1,293,348,- 1,- 1,- 1,- 1,
94,95,755,97,- 1,99,759,- 1,- 1,103,- 1,764,106,107,- 1,- 1,- 1,770,112,- 1,114,115,116,
- 1,396,- 1,- 1,- 1,- 1,- 1,- 1,- 1,785,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,
48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,- 1,64,467,66,67,- 1,- 1,- 1,- 1,76,77,
78,79,80,81,82,83,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,97,102,99,
100,- 1,- 1,103,- 1,- 1,106,107,510,- 1,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,- 1,- 1,- 1,
124,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,547,- 1,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,
35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,626,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
634,89,90,91,92,93,94,95,96,97,- 1,99,100,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,
- 1,114,- 1,- 1,117,118,119,- 1,- 1,- 1,- 1,124,125,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,
44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,- 1,64,- 1,66,67,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,97,
- 1,99,100,101,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,- 1,
- 1,- 1,124,125,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,
27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,
54,55,56,57,58,59,60,61,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,97,- 1,99,100,- 1,- 1,103,- 1,- 1,106,
107,- 1,- 1,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,- 1,- 1,- 1,124,125,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
38,39,40,- 1,- 1,43,44,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,- 1,62,- 1,64,
- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,
92,93,94,95,96,97,- 1,99,- 1,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,114,- 1,- 1,
117,118,119,- 1,- 1,- 1,- 1,124,125,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,
25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,
52,- 1,- 1,55,56,57,58,59,60,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,97,- 1,- 1,100,- 1,- 1,103,- 1,
- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,- 1,- 1,- 1,124,125,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,
- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,- 1,62,- 1,64,- 1,
66,67,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,89,90,91,92,
93,94,95,96,97,- 1,37,- 1,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,48,49,- 1,- 1,114,- 1,- 1,117,
118,119,- 1,59,60,- 1,124,125,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,86,- 1,24,89,- 1,- 1,- 1,- 1,94,95,- 1,97,- 1,99,- 1,37,- 1,39,- 1,- 1,106,107,44,- 1,
46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,96,97,- 1,- 1,
- 1,101,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,0,1,- 1,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,- 1,
- 1,- 1,- 1,94,95,96,97,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,
116,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,59,60,- 1,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,96,97,37,- 1,- 1,101,- 1,103,- 1,44,106,107,- 1,48,
49,- 1,- 1,- 1,114,115,116,- 1,- 1,- 1,59,60,- 1,1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,96,97,37,- 1,- 1,- 1,- 1,
103,- 1,- 1,106,107,- 1,48,49,- 1,- 1,- 1,114,115,116,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,- 1,38,- 1,- 1,
94,95,96,97,- 1,99,- 1,- 1,- 1,103,104,- 1,106,107,- 1,- 1,110,- 1,112,- 1,114,115,116,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,89,90,- 1,92,93,
- 1,- 1,96,- 1,37,- 1,39,- 1,- 1,103,- 1,44,106,107,- 1,48,49,111,112,- 1,114,- 1,- 1,- 1,
118,119,59,60,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,94,95,- 1,97,- 1,38,100,37,- 1,39,- 1,- 1,106,107,44,- 1,- 1,
- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,89,90,- 1,92,93,- 1,- 1,96,94,95,- 1,97,37,- 1,103,- 1,-
1,106,107,44,106,107,- 1,48,49,114,- 1,- 1,- 1,118,119,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,94,95,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,25,26,27,- 1,29,30,31,32,33,34,35,
36,- 1,38,- 1,40,41,- 1,43,- 1,45,- 1,- 1,- 1,- 1,50,51,52,53,54,55,56,57,58,- 1,- 1,61,62,
- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,
90,91,92,93,- 1,- 1,96,- 1,- 1,99,100,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,
32,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,
57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,101,- 1,103,- 1,- 1,- 1,107,25,
26,- 1,111,112,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,101,- 1,
103,- 1,- 1,- 1,107,25,26,- 1,111,112,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,
43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,
- 1,- 1,- 1,100,101,- 1,103,- 1,- 1,- 1,107,25,26,- 1,111,112,- 1,114,- 1,- 1,117,118,119,-
1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,-
1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,
90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,111,112,- 1,114,
- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,
57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,
- 1,- 1,- 1,113,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,- 1,- 1,103,
- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,-
1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,
99,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,
124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,
67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,
- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,113,114,- 1,- 1,117,118,
119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,
114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,
56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,
26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,-
1,- 1,- 1,107,25,26,- 1,- 1,- 1,113,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,-
1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,
99,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,
124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,
67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,
- 1,- 1,96,- 1,- 1,99,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,
119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,
114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,
56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,
26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,99,- 1,- 1,- 1,103,-
1,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,
- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,
124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,
67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,
- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,
119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,
114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,
56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,
26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,25,26,- 1,103,-
1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,38,114,- 1,- 1,117,118,119,- 1,- 1,- 1,- 1,124,125,51,52,- 1,
- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,- 1,- 1,103,- 1,- 1,- 1,
107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,- 1,124,125,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,25,26,
- 1,103,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,38,114,- 1,- 1,117,118,119,- 1,- 1,- 1,- 1,124,125,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,25,26,- 1,103,-
1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,38,114,- 1,- 1,117,118,119,- 1,- 1,- 1,- 1,124,125,51,52,- 1,
- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,37,- 1,- 1,103,- 1,- 1,- 1,
107,- 1,- 1,- 1,48,49,- 1,114,- 1,- 1,117,118,119,- 1,- 1,59,60,124,125,63,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,94,95,-
1,97,- 1,- 1,100,- 1,- 1,- 1,48,49,106,107,37,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,48,49,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,94,95,- 1,97,- 1,99,- 1,86,- 1,- 1,- 1,- 1,106,107,- 1,94,95,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,106,107,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107};
unsigned char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible";unsigned char
Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow";extern void Cyc_yyerror(
struct _tagged_arr);extern int Cyc_yylex();static int Cyc_yychar=(int)'\000';void*Cyc_yylval=(
void*)Cyc_YYINITIALSVAL;static int Cyc_yynerrs=0;struct _tuple15{struct _tagged_arr
f1;void*f2;};static unsigned char _tmp38A[8]="stdcall";static unsigned char _tmp38B[
6]="cdecl";static unsigned char _tmp38C[9]="fastcall";static unsigned char _tmp38D[9]="noreturn";
static unsigned char _tmp38E[6]="const";static unsigned char _tmp38F[8]="aligned";
static unsigned char _tmp390[7]="packed";static unsigned char _tmp391[7]="shared";
static unsigned char _tmp392[7]="unused";static unsigned char _tmp393[5]="weak";
static unsigned char _tmp394[10]="dllimport";static unsigned char _tmp395[10]="dllexport";
static unsigned char _tmp396[23]="no_instrument_function";static unsigned char
_tmp397[12]="constructor";static unsigned char _tmp398[11]="destructor";static
unsigned char _tmp399[22]="no_check_memory_usage";struct _tuple16{void*f1;void*f2;
};struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(){
int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int yyssp_offset;short*yyss=(
short*)({unsigned int _tmp840=10000;short*_tmp841=(short*)_cycalloc_atomic(
_check_times(sizeof(short),_tmp840));{unsigned int _tmp842=_tmp840;unsigned int i;
for(i=0;i < _tmp842;i ++){_tmp841[i]=(short)0;}};_tmp841;});int yyvsp_offset;void**
yyvs=(void**)({unsigned int _tmp83D=10000;void**_tmp83E=(void**)_cycalloc(
_check_times(sizeof(void*),_tmp83D));{unsigned int _tmp83F=_tmp83D;unsigned int i;
for(i=0;i < _tmp83F;i ++){_tmp83E[i]=Cyc_yylval;}};_tmp83E;});int yylsp_offset;
struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmp83A=10000;struct Cyc_Yyltype*
_tmp83B=(struct Cyc_Yyltype*)_cycalloc_atomic(_check_times(sizeof(struct Cyc_Yyltype),
_tmp83A));{unsigned int _tmp83C=_tmp83A;unsigned int i;for(i=0;i < _tmp83C;i ++){
_tmp83B[i]=Cyc_yynewloc();}};_tmp83B;});int yystacksize=10000;void*yyval=Cyc_yylval;
int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;
yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(
10000,++ yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(
yystacksize >= 10000){Cyc_yyerror(_tag_arr("parser stack overflow",sizeof(
unsigned char),22));(int)_throw((void*)Cyc_Yystack_overflow);}}goto yybackup;
yybackup: yyn=(int)Cyc_yypact[yystate];if(yyn == - 32768){goto yydefault;}if(Cyc_yychar
== - 2){Cyc_yychar=Cyc_yylex();}if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{
yychar1=(Cyc_yychar > 0? Cyc_yychar <= 353: 0)?(int)Cyc_yytranslate[
_check_known_subscript_notnull(354,Cyc_yychar)]: 241;}yyn +=yychar1;if((yyn < 0? 1:
yyn > 4977)? 1: Cyc_yycheck[yyn]!= yychar1){goto yydefault;}yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){if(yyn == - 32768){goto yyerrlab;}yyn=- yyn;goto yyreduce;}else{if(yyn == 0){
goto yyerrlab;}}if(yyn == 829){return 0;}if(Cyc_yychar != 0){Cyc_yychar=- 2;}yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0){yyerrstatus --;}yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
yystate];if(yyn == 0){goto yyerrlab;}yyreduce: yylen=(int)Cyc_yyr2[yyn];if(yylen > 0){
yyval=yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];}
switch(yyn){case 1: _LL267: yyval=yyvs[yyvsp_offset];Cyc_Parse_parse_result=Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]);break;case 2: _LL268: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303[0]=({struct Cyc_DeclList_tok_struct
_tmp304;_tmp304.tag=Cyc_DeclList_tok;_tmp304.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]));_tmp304;});_tmp303;});break;case 3: _LL269: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp305=_cycalloc(sizeof(*_tmp305));_tmp305[0]=({
struct Cyc_DeclList_tok_struct _tmp306;_tmp306.tag=Cyc_DeclList_tok;_tmp306.f1=({
struct Cyc_List_List*_tmp307=_cycalloc(sizeof(*_tmp307));_tmp307->hd=({struct Cyc_Absyn_Decl*
_tmp308=_cycalloc(sizeof(*_tmp308));_tmp308->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp309=_cycalloc(sizeof(*_tmp309));_tmp309[0]=({struct Cyc_Absyn_Using_d_struct
_tmp30A;_tmp30A.tag=9;_tmp30A.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp30A.f2=Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]);_tmp30A;});_tmp309;}));_tmp308->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp308;});_tmp307->tl=0;_tmp307;});_tmp306;});_tmp305;});
Cyc_Lex_leave_using();break;case 4: _LL26A: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B[0]=({struct Cyc_DeclList_tok_struct
_tmp30C;_tmp30C.tag=Cyc_DeclList_tok;_tmp30C.f1=({struct Cyc_List_List*_tmp30D=
_cycalloc(sizeof(*_tmp30D));_tmp30D->hd=({struct Cyc_Absyn_Decl*_tmp30E=_cycalloc(
sizeof(*_tmp30E));_tmp30E->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F[0]=({struct Cyc_Absyn_Using_d_struct
_tmp310;_tmp310.tag=9;_tmp310.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp310.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp310;});_tmp30F;}));
_tmp30E->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp30E;});_tmp30D->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp30D;});_tmp30C;});_tmp30B;});break;case 5: _LL26B: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp311=_cycalloc(sizeof(*_tmp311));_tmp311[0]=({
struct Cyc_DeclList_tok_struct _tmp312;_tmp312.tag=Cyc_DeclList_tok;_tmp312.f1=({
struct Cyc_List_List*_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313->hd=({struct Cyc_Absyn_Decl*
_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp315=_cycalloc(sizeof(*_tmp315));_tmp315[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp316;_tmp316.tag=8;_tmp316.f1=({struct _tagged_arr*_tmp317=_cycalloc(sizeof(*
_tmp317));_tmp317[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp317;});_tmp316.f2=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp316;});_tmp315;}));_tmp314->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp314;});_tmp313->tl=0;_tmp313;});_tmp312;});_tmp311;});
Cyc_Lex_leave_namespace();break;case 6: _LL26C: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp318=_cycalloc(sizeof(*_tmp318));_tmp318[0]=({struct Cyc_DeclList_tok_struct
_tmp319;_tmp319.tag=Cyc_DeclList_tok;_tmp319.f1=({struct Cyc_List_List*_tmp31A=
_cycalloc(sizeof(*_tmp31A));_tmp31A->hd=({struct Cyc_Absyn_Decl*_tmp31B=_cycalloc(
sizeof(*_tmp31B));_tmp31B->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp31D;_tmp31D.tag=8;_tmp31D.f1=({struct _tagged_arr*_tmp31E=_cycalloc(sizeof(*
_tmp31E));_tmp31E[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp31E;});_tmp31D.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp31D;});_tmp31C;}));
_tmp31B->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp31B;});_tmp31A->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp31A;});_tmp319;});_tmp318;});break;case 7: _LL26D: if(Cyc_Std_strcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tag_arr("C",sizeof(unsigned char),2))!= 0){Cyc_Parse_err(_tag_arr("only extern \"C\" { ... } is allowed",
sizeof(unsigned char),35),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));}yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp31F=_cycalloc(sizeof(*_tmp31F));_tmp31F[0]=({
struct Cyc_DeclList_tok_struct _tmp320;_tmp320.tag=Cyc_DeclList_tok;_tmp320.f1=({
struct Cyc_List_List*_tmp321=_cycalloc(sizeof(*_tmp321));_tmp321->hd=({struct Cyc_Absyn_Decl*
_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp323=_cycalloc(sizeof(*_tmp323));_tmp323[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp324;_tmp324.tag=10;_tmp324.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp324;});_tmp323;}));
_tmp322->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp322;});_tmp321->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp321;});_tmp320;});_tmp31F;});break;case 8: _LL26E: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp325=_cycalloc(sizeof(*_tmp325));_tmp325[0]=({
struct Cyc_DeclList_tok_struct _tmp326;_tmp326.tag=Cyc_DeclList_tok;_tmp326.f1=0;
_tmp326;});_tmp325;});break;case 9: _LL26F: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp327=_cycalloc(sizeof(*_tmp327));_tmp327[0]=({struct Cyc_DeclList_tok_struct
_tmp328;_tmp328.tag=Cyc_DeclList_tok;_tmp328.f1=({struct Cyc_List_List*_tmp329=
_cycalloc(sizeof(*_tmp329));_tmp329->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp32A=_cycalloc(sizeof(*_tmp32A));_tmp32A[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp32B;_tmp32B.tag=1;_tmp32B.f1=Cyc_yyget_FnDecl_tok(yyvs[yyvsp_offset]);
_tmp32B;});_tmp32A;}),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp329->tl=0;_tmp329;});_tmp328;});_tmp327;});
break;case 10: _LL270: yyval=yyvs[yyvsp_offset];break;case 11: _LL271: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp32C=_cycalloc(sizeof(*_tmp32C));_tmp32C[0]=({
struct Cyc_FnDecl_tok_struct _tmp32D;_tmp32D.tag=Cyc_FnDecl_tok;_tmp32D.f1=Cyc_Parse_make_function(
0,Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp32D;});_tmp32C;});break;case 12: _LL272: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E[0]=({
struct Cyc_FnDecl_tok_struct _tmp32F;_tmp32F.tag=Cyc_FnDecl_tok;_tmp32F.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp330=_cycalloc(sizeof(*_tmp330));_tmp330->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp330;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp32F;});_tmp32E;});break;case 13: _LL273: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp331=_cycalloc(sizeof(*_tmp331));_tmp331[0]=({
struct Cyc_FnDecl_tok_struct _tmp332;_tmp332.tag=Cyc_FnDecl_tok;_tmp332.f1=Cyc_Parse_make_function(
0,Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp332;});_tmp331;});break;case 14: _LL274: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp333=_cycalloc(sizeof(*_tmp333));_tmp333[0]=({
struct Cyc_FnDecl_tok_struct _tmp334;_tmp334.tag=Cyc_FnDecl_tok;_tmp334.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp335=_cycalloc(sizeof(*_tmp335));_tmp335->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp335;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp334;});_tmp333;});break;case 15: _LL275: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp336=_cycalloc(sizeof(*_tmp336));_tmp336[0]=({
struct Cyc_FnDecl_tok_struct _tmp337;_tmp337.tag=Cyc_FnDecl_tok;_tmp337.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp338=_cycalloc(sizeof(*_tmp338));_tmp338->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp338;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp337;});_tmp336;});break;case 16: _LL276: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339[0]=({
struct Cyc_FnDecl_tok_struct _tmp33A;_tmp33A.tag=Cyc_FnDecl_tok;_tmp33A.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp33B=_cycalloc(sizeof(*_tmp33B));_tmp33B->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp33B;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp33A;});_tmp339;});break;case 17: _LL277: Cyc_Lex_enter_using(
Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]));yyval=yyvs[yyvsp_offset];break;case 18:
_LL278: Cyc_Lex_leave_using();break;case 19: _LL279: Cyc_Lex_enter_namespace(({
struct _tagged_arr*_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp33C;}));yyval=yyvs[yyvsp_offset];break;case 20: _LL27A: Cyc_Lex_leave_namespace();
break;case 21: _LL27B: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp33D=
_cycalloc(sizeof(*_tmp33D));_tmp33D[0]=({struct Cyc_DeclList_tok_struct _tmp33E;
_tmp33E.tag=Cyc_DeclList_tok;_tmp33E.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp33E;});
_tmp33D;});break;case 22: _LL27C: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp33F=_cycalloc(sizeof(*_tmp33F));_tmp33F[0]=({struct Cyc_DeclList_tok_struct
_tmp340;_tmp340.tag=Cyc_DeclList_tok;_tmp340.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp340;});_tmp33F;});break;case 23: _LL27D: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp341=_cycalloc(sizeof(*_tmp341));_tmp341[0]=({
struct Cyc_DeclList_tok_struct _tmp342;_tmp342.tag=Cyc_DeclList_tok;_tmp342.f1=({
struct Cyc_List_List*_tmp343=_cycalloc(sizeof(*_tmp343));_tmp343->hd=Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
0,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp343->tl=0;
_tmp343;});_tmp342;});_tmp341;});break;case 24: _LL27E: {struct Cyc_List_List*
_tmp344=0;{struct Cyc_List_List*_tmp345=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp345 != 0;
_tmp345=_tmp345->tl){struct _tagged_arr*_tmp346=(struct _tagged_arr*)_tmp345->hd;
struct _tuple1*_tmp347=({struct _tuple1*_tmp34A=_cycalloc(sizeof(*_tmp34A));
_tmp34A->f1=Cyc_Absyn_rel_ns_null;_tmp34A->f2=_tmp346;_tmp34A;});struct Cyc_Absyn_Vardecl*
_tmp348=Cyc_Absyn_new_vardecl(_tmp347,Cyc_Absyn_wildtyp(0),0);_tmp344=({struct
Cyc_List_List*_tmp349=_cycalloc(sizeof(*_tmp349));_tmp349->hd=_tmp348;_tmp349->tl=
_tmp344;_tmp349;});}}_tmp344=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp344);yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp34B=
_cycalloc(sizeof(*_tmp34B));_tmp34B[0]=({struct Cyc_DeclList_tok_struct _tmp34C;
_tmp34C.tag=Cyc_DeclList_tok;_tmp34C.f1=({struct Cyc_List_List*_tmp34D=_cycalloc(
sizeof(*_tmp34D));_tmp34D->hd=Cyc_Absyn_letv_decl(_tmp344,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp34D->tl=0;_tmp34D;});_tmp34C;});_tmp34B;});break;}
case 25: _LL27F: yyval=yyvs[yyvsp_offset];break;case 26: _LL280: yyval=(void*)({struct
Cyc_DeclList_tok_struct*_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E[0]=({struct
Cyc_DeclList_tok_struct _tmp34F;_tmp34F.tag=Cyc_DeclList_tok;_tmp34F.f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_yyget_DeclList_tok(yyvs[yyvsp_offset]));_tmp34F;});_tmp34E;});break;case 27:
_LL281: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp350=_cycalloc(sizeof(*
_tmp350));_tmp350[0]=({struct Cyc_DeclSpec_tok_struct _tmp351;_tmp351.tag=Cyc_DeclSpec_tok;
_tmp351.f1=({struct Cyc_Parse_Declaration_spec*_tmp352=_cycalloc(sizeof(*_tmp352));
_tmp352->sc=({struct Cyc_Core_Opt*_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353->v=(
void*)Cyc_yyget_StorageClass_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp353;});_tmp352->tq=Cyc_Absyn_empty_tqual();_tmp352->type_specs=
0;_tmp352->is_inline=0;_tmp352->attributes=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp352;});_tmp351;});_tmp350;});break;case 28: _LL282: if((Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->sc != 0){({void*_tmp354[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tag_arr("Only one storage class is allowed in a declaration",
sizeof(unsigned char),51),_tag_arr(_tmp354,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355[0]=({
struct Cyc_DeclSpec_tok_struct _tmp356;_tmp356.tag=Cyc_DeclSpec_tok;_tmp356.f1=({
struct Cyc_Parse_Declaration_spec*_tmp357=_cycalloc(sizeof(*_tmp357));_tmp357->sc=({
struct Cyc_Core_Opt*_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358->v=(void*)Cyc_yyget_StorageClass_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp358;});_tmp357->tq=(
Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq;_tmp357->type_specs=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp357->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->is_inline;_tmp357->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp357;});_tmp356;});_tmp355;});break;case 29:
_LL283: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp359=_cycalloc(sizeof(*
_tmp359));_tmp359[0]=({struct Cyc_DeclSpec_tok_struct _tmp35A;_tmp35A.tag=Cyc_DeclSpec_tok;
_tmp35A.f1=({struct Cyc_Parse_Declaration_spec*_tmp35B=_cycalloc(sizeof(*_tmp35B));
_tmp35B->sc=0;_tmp35B->tq=Cyc_Absyn_empty_tqual();_tmp35B->type_specs=({struct
Cyc_List_List*_tmp35C=_cycalloc(sizeof(*_tmp35C));_tmp35C->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp35C->tl=0;
_tmp35C;});_tmp35B->is_inline=0;_tmp35B->attributes=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp35B;});_tmp35A;});_tmp359;});break;case 30: _LL284: yyval=(
void*)({struct Cyc_DeclSpec_tok_struct*_tmp35D=_cycalloc(sizeof(*_tmp35D));
_tmp35D[0]=({struct Cyc_DeclSpec_tok_struct _tmp35E;_tmp35E.tag=Cyc_DeclSpec_tok;
_tmp35E.f1=({struct Cyc_Parse_Declaration_spec*_tmp35F=_cycalloc(sizeof(*_tmp35F));
_tmp35F->sc=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->sc;_tmp35F->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->tq;_tmp35F->type_specs=({struct Cyc_List_List*_tmp360=
_cycalloc(sizeof(*_tmp360));_tmp360->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp360->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp360;});_tmp35F->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->is_inline;_tmp35F->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp35F;});_tmp35E;});_tmp35D;});break;case 31:
_LL285: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp361=_cycalloc(sizeof(*
_tmp361));_tmp361[0]=({struct Cyc_DeclSpec_tok_struct _tmp362;_tmp362.tag=Cyc_DeclSpec_tok;
_tmp362.f1=({struct Cyc_Parse_Declaration_spec*_tmp363=_cycalloc(sizeof(*_tmp363));
_tmp363->sc=0;_tmp363->tq=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp363->type_specs=0;
_tmp363->is_inline=0;_tmp363->attributes=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp363;});_tmp362;});_tmp361;});break;case 32: _LL286: yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp364=_cycalloc(sizeof(*_tmp364));_tmp364[0]=({
struct Cyc_DeclSpec_tok_struct _tmp365;_tmp365.tag=Cyc_DeclSpec_tok;_tmp365.f1=({
struct Cyc_Parse_Declaration_spec*_tmp366=_cycalloc(sizeof(*_tmp366));_tmp366->sc=(
Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->sc;_tmp366->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq);_tmp366->type_specs=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp366->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->is_inline;_tmp366->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp366;});_tmp365;});_tmp364;});break;case 33:
_LL287: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp367=_cycalloc(sizeof(*
_tmp367));_tmp367[0]=({struct Cyc_DeclSpec_tok_struct _tmp368;_tmp368.tag=Cyc_DeclSpec_tok;
_tmp368.f1=({struct Cyc_Parse_Declaration_spec*_tmp369=_cycalloc(sizeof(*_tmp369));
_tmp369->sc=0;_tmp369->tq=Cyc_Absyn_empty_tqual();_tmp369->type_specs=0;_tmp369->is_inline=
1;_tmp369->attributes=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp369;});
_tmp368;});_tmp367;});break;case 34: _LL288: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A[0]=({struct Cyc_DeclSpec_tok_struct
_tmp36B;_tmp36B.tag=Cyc_DeclSpec_tok;_tmp36B.f1=({struct Cyc_Parse_Declaration_spec*
_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->sc;_tmp36C->tq=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq;
_tmp36C->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->type_specs;
_tmp36C->is_inline=1;_tmp36C->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp36C;});_tmp36B;});_tmp36A;});break;case 35:
_LL289: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp36D=_cycalloc(
sizeof(*_tmp36D));_tmp36D[0]=({struct Cyc_StorageClass_tok_struct _tmp36E;_tmp36E.tag=
Cyc_StorageClass_tok;_tmp36E.f1=(void*)((void*)4);_tmp36E;});_tmp36D;});break;
case 36: _LL28A: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp36F=_cycalloc(
sizeof(*_tmp36F));_tmp36F[0]=({struct Cyc_StorageClass_tok_struct _tmp370;_tmp370.tag=
Cyc_StorageClass_tok;_tmp370.f1=(void*)((void*)5);_tmp370;});_tmp36F;});break;
case 37: _LL28B: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp371=_cycalloc(
sizeof(*_tmp371));_tmp371[0]=({struct Cyc_StorageClass_tok_struct _tmp372;_tmp372.tag=
Cyc_StorageClass_tok;_tmp372.f1=(void*)((void*)3);_tmp372;});_tmp371;});break;
case 38: _LL28C: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp373=_cycalloc(
sizeof(*_tmp373));_tmp373[0]=({struct Cyc_StorageClass_tok_struct _tmp374;_tmp374.tag=
Cyc_StorageClass_tok;_tmp374.f1=(void*)((void*)1);_tmp374;});_tmp373;});break;
case 39: _LL28D: if(Cyc_Std_strcmp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),
_tag_arr("C",sizeof(unsigned char),2))!= 0){Cyc_Parse_err(_tag_arr("only extern or extern \"C\" is allowed",
sizeof(unsigned char),37),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp375=_cycalloc(sizeof(*_tmp375));_tmp375[0]=({struct Cyc_StorageClass_tok_struct
_tmp376;_tmp376.tag=Cyc_StorageClass_tok;_tmp376.f1=(void*)((void*)2);_tmp376;});
_tmp375;});break;case 40: _LL28E: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp377=_cycalloc(sizeof(*_tmp377));_tmp377[0]=({struct Cyc_StorageClass_tok_struct
_tmp378;_tmp378.tag=Cyc_StorageClass_tok;_tmp378.f1=(void*)((void*)0);_tmp378;});
_tmp377;});break;case 41: _LL28F: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379[0]=({struct Cyc_StorageClass_tok_struct
_tmp37A;_tmp37A.tag=Cyc_StorageClass_tok;_tmp37A.f1=(void*)((void*)6);_tmp37A;});
_tmp379;});break;case 42: _LL290: yyval=(void*)({struct Cyc_AttributeList_tok_struct*
_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B[0]=({struct Cyc_AttributeList_tok_struct
_tmp37C;_tmp37C.tag=Cyc_AttributeList_tok;_tmp37C.f1=0;_tmp37C;});_tmp37B;});
break;case 43: _LL291: yyval=yyvs[yyvsp_offset];break;case 44: _LL292: yyval=(void*)({
struct Cyc_AttributeList_tok_struct*_tmp37D=_cycalloc(sizeof(*_tmp37D));_tmp37D[0]=({
struct Cyc_AttributeList_tok_struct _tmp37E;_tmp37E.tag=Cyc_AttributeList_tok;
_tmp37E.f1=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp37E;});_tmp37D;});break;case 45: _LL293: yyval=(void*)({
struct Cyc_AttributeList_tok_struct*_tmp37F=_cycalloc(sizeof(*_tmp37F));_tmp37F[0]=({
struct Cyc_AttributeList_tok_struct _tmp380;_tmp380.tag=Cyc_AttributeList_tok;
_tmp380.f1=({struct Cyc_List_List*_tmp381=_cycalloc(sizeof(*_tmp381));_tmp381->hd=(
void*)Cyc_yyget_Attribute_tok(yyvs[yyvsp_offset]);_tmp381->tl=0;_tmp381;});
_tmp380;});_tmp37F;});break;case 46: _LL294: yyval=(void*)({struct Cyc_AttributeList_tok_struct*
_tmp382=_cycalloc(sizeof(*_tmp382));_tmp382[0]=({struct Cyc_AttributeList_tok_struct
_tmp383;_tmp383.tag=Cyc_AttributeList_tok;_tmp383.f1=({struct Cyc_List_List*
_tmp384=_cycalloc(sizeof(*_tmp384));_tmp384->hd=(void*)Cyc_yyget_Attribute_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp384->tl=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp384;});_tmp383;});_tmp382;});break;case 47: _LL295: {
static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};static struct _tuple15
att_map[16]={{{_tmp38A,_tmp38A,_tmp38A + 8},(void*)0},{{_tmp38B,_tmp38B,_tmp38B + 
6},(void*)1},{{_tmp38C,_tmp38C,_tmp38C + 9},(void*)2},{{_tmp38D,_tmp38D,_tmp38D + 
9},(void*)3},{{_tmp38E,_tmp38E,_tmp38E + 6},(void*)4},{{_tmp38F,_tmp38F,_tmp38F + 
8},(void*)& att_aligned},{{_tmp390,_tmp390,_tmp390 + 7},(void*)5},{{_tmp391,
_tmp391,_tmp391 + 7},(void*)7},{{_tmp392,_tmp392,_tmp392 + 7},(void*)8},{{_tmp393,
_tmp393,_tmp393 + 5},(void*)9},{{_tmp394,_tmp394,_tmp394 + 10},(void*)10},{{
_tmp395,_tmp395,_tmp395 + 10},(void*)11},{{_tmp396,_tmp396,_tmp396 + 23},(void*)12},{{
_tmp397,_tmp397,_tmp397 + 12},(void*)13},{{_tmp398,_tmp398,_tmp398 + 11},(void*)14},{{
_tmp399,_tmp399,_tmp399 + 22},(void*)15}};struct _tagged_arr _tmp385=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);if((((_get_arr_size(_tmp385,sizeof(unsigned char))> 4?*((
const unsigned char*)_check_unknown_subscript(_tmp385,sizeof(unsigned char),0))== '_':
0)?*((const unsigned char*)_check_unknown_subscript(_tmp385,sizeof(unsigned char),
1))== '_': 0)?*((const unsigned char*)_check_unknown_subscript(_tmp385,sizeof(
unsigned char),(int)(_get_arr_size(_tmp385,sizeof(unsigned char))- 2)))== '_': 0)?*((
const unsigned char*)_check_unknown_subscript(_tmp385,sizeof(unsigned char),(int)(
_get_arr_size(_tmp385,sizeof(unsigned char))- 3)))== '_': 0){_tmp385=(struct
_tagged_arr)Cyc_Std_substring(_tmp385,2,_get_arr_size(_tmp385,sizeof(
unsigned char))- 5);}{int i=0;for(0;i < 16;++ i){if(Cyc_Std_strcmp(_tmp385,(att_map[
i]).f1)== 0){yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp386=_cycalloc(
sizeof(*_tmp386));_tmp386[0]=({struct Cyc_Attribute_tok_struct _tmp387;_tmp387.tag=
Cyc_Attribute_tok;_tmp387.f1=(void*)(att_map[i]).f2;_tmp387;});_tmp386;});break;}}
if(i == 16){Cyc_Parse_err(_tag_arr("unrecognized attribute",sizeof(unsigned char),
23),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp388=
_cycalloc(sizeof(*_tmp388));_tmp388[0]=({struct Cyc_Attribute_tok_struct _tmp389;
_tmp389.tag=Cyc_Attribute_tok;_tmp389.f1=(void*)((void*)1);_tmp389;});_tmp388;});}
break;}}case 48: _LL296: yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp39B=
_cycalloc(sizeof(*_tmp39B));_tmp39B[0]=({struct Cyc_Attribute_tok_struct _tmp39C;
_tmp39C.tag=Cyc_Attribute_tok;_tmp39C.f1=(void*)((void*)4);_tmp39C;});_tmp39B;});
break;case 49: _LL297: {struct _tagged_arr _tmp39D=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct _tuple12 _tmp39F;
int _tmp3A0;struct _tuple12*_tmp39E=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp39F=*_tmp39E;_LL29A:
_tmp3A0=_tmp39F.f2;goto _LL299;_LL299: {void*a;if(Cyc_Std_zstrcmp(_tmp39D,
_tag_arr("regparm",sizeof(unsigned char),8))== 0? 1: Cyc_Std_zstrcmp(_tmp39D,
_tag_arr("__regparm__",sizeof(unsigned char),12))== 0){if(_tmp3A0 < 0? 1: _tmp3A0 > 
3){Cyc_Parse_err(_tag_arr("regparm requires value between 0 and 3",sizeof(
unsigned char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp3A1=_cycalloc_atomic(sizeof(*_tmp3A1));
_tmp3A1[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp3A2;_tmp3A2.tag=0;_tmp3A2.f1=
_tmp3A0;_tmp3A2;});_tmp3A1;});}else{if(Cyc_Std_zstrcmp(_tmp39D,_tag_arr("aligned",
sizeof(unsigned char),8))== 0? 1: Cyc_Std_zstrcmp(_tmp39D,_tag_arr("__aligned__",
sizeof(unsigned char),12))== 0){if(_tmp3A0 < 0){Cyc_Parse_err(_tag_arr("aligned requires positive power of two",
sizeof(unsigned char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp3A0;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
Cyc_Parse_err(_tag_arr("aligned requires positive power of two",sizeof(
unsigned char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp3A3=_cycalloc_atomic(sizeof(*_tmp3A3));
_tmp3A3[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp3A4;_tmp3A4.tag=1;_tmp3A4.f1=
_tmp3A0;_tmp3A4;});_tmp3A3;});}}else{Cyc_Parse_err(_tag_arr("unrecognized attribute",
sizeof(unsigned char),23),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));
_tmp3A5[0]=({struct Cyc_Attribute_tok_struct _tmp3A6;_tmp3A6.tag=Cyc_Attribute_tok;
_tmp3A6.f1=(void*)a;_tmp3A6;});_tmp3A5;});break;}}case 50: _LL298: {struct
_tagged_arr _tmp3A7=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp3A8=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_Std_zstrcmp(
_tmp3A7,_tag_arr("section",sizeof(unsigned char),8))== 0? 1: Cyc_Std_zstrcmp(
_tmp3A7,_tag_arr("__section__",sizeof(unsigned char),12))== 0){a=(void*)({struct
Cyc_Absyn_Section_att_struct*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9[0]=({
struct Cyc_Absyn_Section_att_struct _tmp3AA;_tmp3AA.tag=2;_tmp3AA.f1=_tmp3A8;
_tmp3AA;});_tmp3A9;});}else{Cyc_Parse_err(_tag_arr("unrecognized attribute",
sizeof(unsigned char),23),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));
_tmp3AB[0]=({struct Cyc_Attribute_tok_struct _tmp3AC;_tmp3AC.tag=Cyc_Attribute_tok;
_tmp3AC.f1=(void*)a;_tmp3AC;});_tmp3AB;});break;}case 51: _LL29B: {struct
_tagged_arr _tmp3AD=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp3AE=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp3B0;struct
_tuple12 _tmp3AF=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_LL29E: _tmp3B0=_tmp3AF.f2;goto _LL29D;_LL29D: {int _tmp3B2;
struct _tuple12 _tmp3B1=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_LL2A0: _tmp3B2=_tmp3B1.f2;goto _LL29F;_LL29F: {void*a=(
void*)1;if(Cyc_Std_zstrcmp(_tmp3AD,_tag_arr("format",sizeof(unsigned char),7))== 
0? 1: Cyc_Std_zstrcmp(_tmp3AD,_tag_arr("__format__",sizeof(unsigned char),11))== 0){
if(Cyc_Std_zstrcmp(_tmp3AE,_tag_arr("printf",sizeof(unsigned char),7))== 0){a=(
void*)({struct Cyc_Absyn_Format_att_struct*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));
_tmp3B3[0]=({struct Cyc_Absyn_Format_att_struct _tmp3B4;_tmp3B4.tag=3;_tmp3B4.f1=(
void*)((void*)0);_tmp3B4.f2=_tmp3B0;_tmp3B4.f3=_tmp3B2;_tmp3B4;});_tmp3B3;});}
else{if(Cyc_Std_zstrcmp(_tmp3AE,_tag_arr("scanf",sizeof(unsigned char),6))== 0){
a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));
_tmp3B5[0]=({struct Cyc_Absyn_Format_att_struct _tmp3B6;_tmp3B6.tag=3;_tmp3B6.f1=(
void*)((void*)1);_tmp3B6.f2=_tmp3B0;_tmp3B6.f3=_tmp3B2;_tmp3B6;});_tmp3B5;});}
else{Cyc_Parse_err(_tag_arr("unrecognized format type",sizeof(unsigned char),25),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(_tag_arr("unrecognized attribute",
sizeof(unsigned char),23),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Attribute_tok_struct*
_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7[0]=({struct Cyc_Attribute_tok_struct
_tmp3B8;_tmp3B8.tag=Cyc_Attribute_tok;_tmp3B8.f1=(void*)a;_tmp3B8;});_tmp3B7;});
break;}}}case 52: _LL29C: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3B9=
_cycalloc(sizeof(*_tmp3B9));_tmp3B9[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3BA;_tmp3BA.tag=Cyc_TypeSpecifier_tok;_tmp3BA.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3BA;});_tmp3B9;});break;case 53: _LL2A1: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3BC;_tmp3BC.tag=Cyc_TypeSpecifier_tok;
_tmp3BC.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3BC;});
_tmp3BB;});break;case 54: _LL2A2: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3BE;_tmp3BE.tag=Cyc_TypeSpecifier_tok;_tmp3BE.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_uchar_t,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3BE;});_tmp3BD;});break;case 55: _LL2A3: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));
_tmp3BF[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3C0;_tmp3C0.tag=Cyc_TypeSpecifier_tok;
_tmp3C0.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*_tmp3C1=_cycalloc(
sizeof(*_tmp3C1));_tmp3C1[0]=({struct Cyc_Parse_Short_spec_struct _tmp3C2;_tmp3C2.tag=
2;_tmp3C2.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line);_tmp3C2;});_tmp3C1;}));_tmp3C0;});_tmp3BF;});break;
case 56: _LL2A4: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3C3=
_cycalloc(sizeof(*_tmp3C3));_tmp3C3[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3C4;_tmp3C4.tag=Cyc_TypeSpecifier_tok;_tmp3C4.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_t,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3C4;});_tmp3C3;});break;case 57: _LL2A5: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));
_tmp3C5[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3C6;_tmp3C6.tag=Cyc_TypeSpecifier_tok;
_tmp3C6.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*_tmp3C7=_cycalloc(
sizeof(*_tmp3C7));_tmp3C7[0]=({struct Cyc_Parse_Long_spec_struct _tmp3C8;_tmp3C8.tag=
3;_tmp3C8.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line);_tmp3C8;});_tmp3C7;}));_tmp3C6;});_tmp3C5;});break;
case 58: _LL2A6: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3C9=
_cycalloc(sizeof(*_tmp3C9));_tmp3C9[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3CA;_tmp3CA.tag=Cyc_TypeSpecifier_tok;_tmp3CA.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3CA;});_tmp3C9;});break;case 59: _LL2A7: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));
_tmp3CB[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3CC;_tmp3CC.tag=Cyc_TypeSpecifier_tok;
_tmp3CC.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3CC;});
_tmp3CB;});break;case 60: _LL2A8: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3CE;_tmp3CE.tag=Cyc_TypeSpecifier_tok;_tmp3CE.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*
_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp3D0;_tmp3D0.tag=0;_tmp3D0.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp3D0;});_tmp3CF;}));_tmp3CE;});_tmp3CD;});
break;case 61: _LL2A9: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3D1=
_cycalloc(sizeof(*_tmp3D1));_tmp3D1[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3D2;_tmp3D2.tag=Cyc_TypeSpecifier_tok;_tmp3D2.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp3D4;_tmp3D4.tag=1;_tmp3D4.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp3D4;});_tmp3D3;}));_tmp3D2;});_tmp3D1;});
break;case 62: _LL2AA: yyval=yyvs[yyvsp_offset];break;case 63: _LL2AB: yyval=yyvs[
yyvsp_offset];break;case 64: _LL2AC: yyval=yyvs[yyvsp_offset];break;case 65: _LL2AD:
yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3D5=_cycalloc(sizeof(*
_tmp3D5));_tmp3D5[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3D6;_tmp3D6.tag=Cyc_TypeSpecifier_tok;
_tmp3D6.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3D6;});
_tmp3D5;});break;case 66: _LL2AE: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3D8;_tmp3D8.tag=Cyc_TypeSpecifier_tok;_tmp3D8.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypedefType_struct*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));
_tmp3D9[0]=({struct Cyc_Absyn_TypedefType_struct _tmp3DA;_tmp3DA.tag=16;_tmp3DA.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3DA.f2=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp3DA.f3=0;_tmp3DA;});
_tmp3D9;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3D8;});
_tmp3D7;});break;case 67: _LL2AF: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3DC;_tmp3DC.tag=Cyc_TypeSpecifier_tok;_tmp3DC.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TupleType_struct*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));
_tmp3DD[0]=({struct Cyc_Absyn_TupleType_struct _tmp3DE;_tmp3DE.tag=9;_tmp3DE.f1=((
struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp3DE;});_tmp3DD;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3DC;});
_tmp3DB;});break;case 68: _LL2B0: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3E0;_tmp3E0.tag=Cyc_TypeSpecifier_tok;_tmp3E0.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));
_tmp3E1[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp3E2;_tmp3E2.tag=15;_tmp3E2.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp3E2;});_tmp3E1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3E0;});_tmp3DF;});break;case 69: _LL2B1: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3E3=_cycalloc(sizeof(*_tmp3E3));_tmp3E3[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3E4;_tmp3E4.tag=Cyc_TypeSpecifier_tok;
_tmp3E4.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp3E6;_tmp3E6.tag=14;_tmp3E6.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3E6;});_tmp3E5;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3E4;});
_tmp3E3;});break;case 70: _LL2B2: yyval=(void*)({struct Cyc_Kind_tok_struct*_tmp3E7=
_cycalloc(sizeof(*_tmp3E7));_tmp3E7[0]=({struct Cyc_Kind_tok_struct _tmp3E8;
_tmp3E8.tag=Cyc_Kind_tok;_tmp3E8.f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3E8;});_tmp3E7;});break;case 71: _LL2B3: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp3E9=_cycalloc_atomic(sizeof(*_tmp3E9));
_tmp3E9[0]=({struct Cyc_TypeQual_tok_struct _tmp3EA;_tmp3EA.tag=Cyc_TypeQual_tok;
_tmp3EA.f1=({struct Cyc_Absyn_Tqual _tmp3EB;_tmp3EB.q_const=1;_tmp3EB.q_volatile=0;
_tmp3EB.q_restrict=0;_tmp3EB;});_tmp3EA;});_tmp3E9;});break;case 72: _LL2B4: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp3EC=_cycalloc_atomic(sizeof(*_tmp3EC));
_tmp3EC[0]=({struct Cyc_TypeQual_tok_struct _tmp3ED;_tmp3ED.tag=Cyc_TypeQual_tok;
_tmp3ED.f1=({struct Cyc_Absyn_Tqual _tmp3EE;_tmp3EE.q_const=0;_tmp3EE.q_volatile=1;
_tmp3EE.q_restrict=0;_tmp3EE;});_tmp3ED;});_tmp3EC;});break;case 73: _LL2B5: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp3EF=_cycalloc_atomic(sizeof(*_tmp3EF));
_tmp3EF[0]=({struct Cyc_TypeQual_tok_struct _tmp3F0;_tmp3F0.tag=Cyc_TypeQual_tok;
_tmp3F0.f1=({struct Cyc_Absyn_Tqual _tmp3F1;_tmp3F1.q_const=0;_tmp3F1.q_volatile=0;
_tmp3F1.q_restrict=1;_tmp3F1;});_tmp3F0;});_tmp3EF;});break;case 74: _LL2B6: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));
_tmp3F2[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3F3;_tmp3F3.tag=Cyc_TypeSpecifier_tok;
_tmp3F3.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp3F4=_cycalloc(
sizeof(*_tmp3F4));_tmp3F4[0]=({struct Cyc_Parse_Decl_spec_struct _tmp3F5;_tmp3F5.tag=
5;_tmp3F5.f1=({struct Cyc_Absyn_Decl*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));_tmp3F6->r=(
void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));
_tmp3F7[0]=({struct Cyc_Absyn_Enum_d_struct _tmp3F8;_tmp3F8.tag=6;_tmp3F8.f1=({
struct Cyc_Absyn_Enumdecl*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));_tmp3F9->sc=(void*)((
void*)2);_tmp3F9->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp3F9->fields=({struct Cyc_Core_Opt*_tmp3FA=_cycalloc(
sizeof(*_tmp3FA));_tmp3FA->v=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3FA;});_tmp3F9;});
_tmp3F8;});_tmp3F7;}));_tmp3F6->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp3F6;});_tmp3F5;});_tmp3F4;}));_tmp3F3;});_tmp3F2;});
break;case 75: _LL2B7: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3FB=
_cycalloc(sizeof(*_tmp3FB));_tmp3FB[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3FC;_tmp3FC.tag=Cyc_TypeSpecifier_tok;_tmp3FC.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));
_tmp3FD[0]=({struct Cyc_Absyn_EnumType_struct _tmp3FE;_tmp3FE.tag=12;_tmp3FE.f1=
Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);_tmp3FE.f2=0;_tmp3FE;});_tmp3FD;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3FC;});_tmp3FB;});break;case 76: _LL2B8: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp400;_tmp400.tag=Cyc_TypeSpecifier_tok;
_tmp400.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp401=_cycalloc(
sizeof(*_tmp401));_tmp401[0]=({struct Cyc_Parse_Type_spec_struct _tmp402;_tmp402.tag=
4;_tmp402.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp403=
_cycalloc(sizeof(*_tmp403));_tmp403[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp404;_tmp404.tag=13;_tmp404.f1=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp404;});_tmp403;}));
_tmp402.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line);_tmp402;});
_tmp401;}));_tmp400;});_tmp3FF;});break;case 77: _LL2B9: yyval=(void*)({struct Cyc_Enumfield_tok_struct*
_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405[0]=({struct Cyc_Enumfield_tok_struct
_tmp406;_tmp406.tag=Cyc_Enumfield_tok;_tmp406.f1=({struct Cyc_Absyn_Enumfield*
_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->name=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp407->tag=0;_tmp407->loc=Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);_tmp407;});_tmp406;});
_tmp405;});break;case 78: _LL2BA: yyval=(void*)({struct Cyc_Enumfield_tok_struct*
_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408[0]=({struct Cyc_Enumfield_tok_struct
_tmp409;_tmp409.tag=Cyc_Enumfield_tok;_tmp409.f1=({struct Cyc_Absyn_Enumfield*
_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp40A->tag=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp40A->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp40A;});_tmp409;});_tmp408;});break;case 79: _LL2BB:
yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*_tmp40B=_cycalloc(sizeof(*
_tmp40B));_tmp40B[0]=({struct Cyc_EnumfieldList_tok_struct _tmp40C;_tmp40C.tag=Cyc_EnumfieldList_tok;
_tmp40C.f1=({struct Cyc_List_List*_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D->hd=
Cyc_yyget_Enumfield_tok(yyvs[yyvsp_offset]);_tmp40D->tl=0;_tmp40D;});_tmp40C;});
_tmp40B;});break;case 80: _LL2BC: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp40F;_tmp40F.tag=Cyc_EnumfieldList_tok;_tmp40F.f1=({struct Cyc_List_List*
_tmp410=_cycalloc(sizeof(*_tmp410));_tmp410->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp410->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[yyvsp_offset]);_tmp410;});_tmp40F;});_tmp40E;});break;case 81: _LL2BD: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp411=_cycalloc(sizeof(*_tmp411));
_tmp411[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp412;_tmp412.tag=Cyc_TypeSpecifier_tok;
_tmp412.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmp413=_cycalloc(sizeof(*_tmp413));_tmp413[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp414;_tmp414.tag=11;_tmp414.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp414.f2=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp414;});_tmp413;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp412;});
_tmp411;});break;case 82: _LL2BE: {struct Cyc_List_List*_tmp415=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));struct Cyc_List_List*
_tmp416=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp418;_tmp418.tag=Cyc_TypeSpecifier_tok;
_tmp418.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp419=_cycalloc(
sizeof(*_tmp419));_tmp419[0]=({struct Cyc_Parse_Decl_spec_struct _tmp41A;_tmp41A.tag=
5;_tmp41A.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmp415,({struct Cyc_Core_Opt*
_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B->v=_tmp416;_tmp41B;}),({struct Cyc_Core_Opt*
_tmp41C=_cycalloc(sizeof(*_tmp41C));_tmp41C->v=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp41C;}),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp41A;});_tmp419;}));_tmp418;});_tmp417;});break;}
case 83: _LL2BF: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp41D=
_cycalloc(sizeof(*_tmp41D));_tmp41D[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp41E;_tmp41E.tag=Cyc_TypeSpecifier_tok;_tmp41E.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp41F=_cycalloc(sizeof(*_tmp41F));
_tmp41F[0]=({struct Cyc_Absyn_AggrType_struct _tmp420;_tmp420.tag=10;_tmp420.f1=({
struct Cyc_Absyn_AggrInfo _tmp421;_tmp421.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp423;_tmp423.tag=0;_tmp423.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp423.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp423;});_tmp422;}));
_tmp421.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp421;});_tmp420;});
_tmp41F;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp41E;});
_tmp41D;});break;case 84: _LL2C0: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp424=_cycalloc(sizeof(*_tmp424));_tmp424[0]=({struct Cyc_TypeList_tok_struct
_tmp425;_tmp425.tag=Cyc_TypeList_tok;_tmp425.f1=0;_tmp425;});_tmp424;});break;
case 85: _LL2C1: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp426=_cycalloc(
sizeof(*_tmp426));_tmp426[0]=({struct Cyc_TypeList_tok_struct _tmp427;_tmp427.tag=
Cyc_TypeList_tok;_tmp427.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp427;});_tmp426;});
break;case 86: _LL2C2: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp428=
_cycalloc(sizeof(*_tmp428));_tmp428[0]=({struct Cyc_TypeList_tok_struct _tmp429;
_tmp429.tag=Cyc_TypeList_tok;_tmp429.f1=0;_tmp429;});_tmp428;});break;case 87:
_LL2C3: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp42A=_cycalloc(sizeof(*
_tmp42A));_tmp42A[0]=({struct Cyc_TypeList_tok_struct _tmp42B;_tmp42B.tag=Cyc_TypeList_tok;
_tmp42B.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp42B;});_tmp42A;});
break;case 88: _LL2C4: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp42C=
_cycalloc(sizeof(*_tmp42C));_tmp42C[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp42D;_tmp42D.tag=Cyc_StructOrUnion_tok;_tmp42D.f1=(void*)((void*)0);_tmp42D;});
_tmp42C;});break;case 89: _LL2C5: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*
_tmp42E=_cycalloc(sizeof(*_tmp42E));_tmp42E[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp42F;_tmp42F.tag=Cyc_StructOrUnion_tok;_tmp42F.f1=(void*)((void*)1);_tmp42F;});
_tmp42E;});break;case 90: _LL2C6: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp430=_cycalloc(sizeof(*_tmp430));_tmp430[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp431;_tmp431.tag=Cyc_AggrFieldDeclList_tok;_tmp431.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_AggrFieldDeclListList_tok(yyvs[yyvsp_offset])));
_tmp431;});_tmp430;});break;case 91: _LL2C7: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp433;_tmp433.tag=Cyc_AggrFieldDeclListList_tok;_tmp433.f1=({struct Cyc_List_List*
_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[yyvsp_offset]);_tmp434->tl=0;_tmp434;});_tmp433;});_tmp432;});break;case 92:
_LL2C8: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*_tmp435=
_cycalloc(sizeof(*_tmp435));_tmp435[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp436;_tmp436.tag=Cyc_AggrFieldDeclListList_tok;_tmp436.f1=({struct Cyc_List_List*
_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[yyvsp_offset]);_tmp437->tl=Cyc_yyget_AggrFieldDeclListList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp437;});_tmp436;});
_tmp435;});break;case 93: _LL2C9: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438[0]=({struct Cyc_InitDeclList_tok_struct
_tmp439;_tmp439.tag=Cyc_InitDeclList_tok;_tmp439.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
yyvsp_offset]));_tmp439;});_tmp438;});break;case 94: _LL2CA: yyval=(void*)({struct
Cyc_InitDeclList_tok_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A[0]=({
struct Cyc_InitDeclList_tok_struct _tmp43B;_tmp43B.tag=Cyc_InitDeclList_tok;
_tmp43B.f1=({struct Cyc_List_List*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->hd=
Cyc_yyget_InitDecl_tok(yyvs[yyvsp_offset]);_tmp43C->tl=0;_tmp43C;});_tmp43B;});
_tmp43A;});break;case 95: _LL2CB: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D[0]=({struct Cyc_InitDeclList_tok_struct
_tmp43E;_tmp43E.tag=Cyc_InitDeclList_tok;_tmp43E.f1=({struct Cyc_List_List*
_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->hd=Cyc_yyget_InitDecl_tok(yyvs[
yyvsp_offset]);_tmp43F->tl=Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp43F;});_tmp43E;});
_tmp43D;});break;case 96: _LL2CC: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440[0]=({struct Cyc_InitDecl_tok_struct
_tmp441;_tmp441.tag=Cyc_InitDecl_tok;_tmp441.f1=({struct _tuple11*_tmp442=
_cycalloc(sizeof(*_tmp442));_tmp442->f1=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp442->f2=0;_tmp442;});_tmp441;});_tmp440;});break;case 97:
_LL2CD: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp443=_cycalloc(sizeof(*
_tmp443));_tmp443[0]=({struct Cyc_InitDecl_tok_struct _tmp444;_tmp444.tag=Cyc_InitDecl_tok;
_tmp444.f1=({struct _tuple11*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->f1=Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp445->f2=(struct
Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp445;});_tmp444;});
_tmp443;});break;case 98: _LL2CE: {struct _tuple14 _tmp447;struct Cyc_List_List*
_tmp448;struct Cyc_List_List*_tmp449;struct Cyc_Absyn_Tqual _tmp44A;struct _tuple14*
_tmp446=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp447=*_tmp446;_LL2D3: _tmp44A=_tmp447.f1;goto _LL2D2;_LL2D2:
_tmp449=_tmp447.f2;goto _LL2D1;_LL2D1: _tmp448=_tmp447.f3;goto _LL2D0;_LL2D0: {
struct Cyc_List_List*_tmp44C;struct Cyc_List_List*_tmp44D;struct _tuple0 _tmp44B=((
struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_LL2D6: _tmp44D=
_tmp44B.f1;goto _LL2D5;_LL2D5: _tmp44C=_tmp44B.f2;goto _LL2D4;_LL2D4: {void*_tmp44E=
Cyc_Parse_speclist2typ(_tmp449,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp44F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(Cyc_Parse_apply_tmss(_tmp44A,_tmp44E,_tmp44D,_tmp448),_tmp44C);
yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*_tmp450=_cycalloc(sizeof(*
_tmp450));_tmp450[0]=({struct Cyc_AggrFieldDeclList_tok_struct _tmp451;_tmp451.tag=
Cyc_AggrFieldDeclList_tok;_tmp451.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Position_Segment*,struct _tuple7*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp44F);
_tmp451;});_tmp450;});break;}}}case 99: _LL2CF: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp452=_cycalloc(sizeof(*_tmp452));_tmp452[0]=({struct Cyc_QualSpecList_tok_struct
_tmp453;_tmp453.tag=Cyc_QualSpecList_tok;_tmp453.f1=({struct _tuple14*_tmp454=
_cycalloc(sizeof(*_tmp454));_tmp454->f1=Cyc_Absyn_empty_tqual();_tmp454->f2=({
struct Cyc_List_List*_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp455->tl=0;
_tmp455;});_tmp454->f3=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp454;});
_tmp453;});_tmp452;});break;case 100: _LL2D7: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp456=_cycalloc(sizeof(*_tmp456));_tmp456[0]=({struct Cyc_QualSpecList_tok_struct
_tmp457;_tmp457.tag=Cyc_QualSpecList_tok;_tmp457.f1=({struct _tuple14*_tmp458=
_cycalloc(sizeof(*_tmp458));_tmp458->f1=(*Cyc_yyget_QualSpecList_tok(yyvs[
yyvsp_offset])).f1;_tmp458->f2=({struct Cyc_List_List*_tmp459=_cycalloc(sizeof(*
_tmp459));_tmp459->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp459->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f2;_tmp459;});_tmp458->f3=Cyc_List_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f3);_tmp458;});_tmp457;});_tmp456;});break;case 101: _LL2D8:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp45A=_cycalloc(sizeof(*
_tmp45A));_tmp45A[0]=({struct Cyc_QualSpecList_tok_struct _tmp45B;_tmp45B.tag=Cyc_QualSpecList_tok;
_tmp45B.f1=({struct _tuple14*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp45C->f2=0;
_tmp45C->f3=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp45C;});_tmp45B;});
_tmp45A;});break;case 102: _LL2D9: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D[0]=({struct Cyc_QualSpecList_tok_struct
_tmp45E;_tmp45E.tag=Cyc_QualSpecList_tok;_tmp45E.f1=({struct _tuple14*_tmp45F=
_cycalloc(sizeof(*_tmp45F));_tmp45F->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f1);_tmp45F->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
yyvsp_offset])).f2;_tmp45F->f3=Cyc_List_append(Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f3);_tmp45F;});_tmp45E;});_tmp45D;});break;case 103: _LL2DA:
yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp460=_cycalloc(sizeof(*
_tmp460));_tmp460[0]=({struct Cyc_InitDeclList_tok_struct _tmp461;_tmp461.tag=Cyc_InitDeclList_tok;
_tmp461.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok(yyvs[yyvsp_offset]));_tmp461;});_tmp460;});break;case
104: _LL2DB: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp462=_cycalloc(
sizeof(*_tmp462));_tmp462[0]=({struct Cyc_InitDeclList_tok_struct _tmp463;_tmp463.tag=
Cyc_InitDeclList_tok;_tmp463.f1=({struct Cyc_List_List*_tmp464=_cycalloc(sizeof(*
_tmp464));_tmp464->hd=Cyc_yyget_InitDecl_tok(yyvs[yyvsp_offset]);_tmp464->tl=0;
_tmp464;});_tmp463;});_tmp462;});break;case 105: _LL2DC: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465[0]=({struct Cyc_InitDeclList_tok_struct
_tmp466;_tmp466.tag=Cyc_InitDeclList_tok;_tmp466.f1=({struct Cyc_List_List*
_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467->hd=Cyc_yyget_InitDecl_tok(yyvs[
yyvsp_offset]);_tmp467->tl=Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp467;});_tmp466;});
_tmp465;});break;case 106: _LL2DD: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468[0]=({struct Cyc_InitDecl_tok_struct
_tmp469;_tmp469.tag=Cyc_InitDecl_tok;_tmp469.f1=({struct _tuple11*_tmp46A=
_cycalloc(sizeof(*_tmp46A));_tmp46A->f1=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp46A->f2=0;_tmp46A;});_tmp469;});_tmp468;});break;case 107:
_LL2DE: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp46B=_cycalloc(sizeof(*
_tmp46B));_tmp46B[0]=({struct Cyc_InitDecl_tok_struct _tmp46C;_tmp46C.tag=Cyc_InitDecl_tok;
_tmp46C.f1=({struct _tuple11*_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D->f1=({
struct Cyc_Parse_Declarator*_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E->id=({
struct _tuple1*_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F->f1=Cyc_Absyn_rel_ns_null;
_tmp46F->f2=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(
unsigned char),1);_tmp46F;});_tmp46E->tms=0;_tmp46E;});_tmp46D->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp46D;});_tmp46C;});_tmp46B;});break;
case 108: _LL2DF: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp471=_cycalloc(
sizeof(*_tmp471));_tmp471[0]=({struct Cyc_InitDecl_tok_struct _tmp472;_tmp472.tag=
Cyc_InitDecl_tok;_tmp472.f1=({struct _tuple11*_tmp473=_cycalloc(sizeof(*_tmp473));
_tmp473->f1=Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp473->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]);_tmp473;});_tmp472;});_tmp471;});break;case 109: _LL2E0: {struct Cyc_List_List*
_tmp474=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp476;_tmp476.tag=Cyc_TypeSpecifier_tok;
_tmp476.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp477=_cycalloc(
sizeof(*_tmp477));_tmp477[0]=({struct Cyc_Parse_Decl_spec_struct _tmp478;_tmp478.tag=
5;_tmp478.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp474,({struct Cyc_Core_Opt*
_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp479;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp478;});_tmp477;}));_tmp476;});_tmp475;});break;}
case 110: _LL2E1: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp47A=
_cycalloc(sizeof(*_tmp47A));_tmp47A[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp47B;_tmp47B.tag=Cyc_TypeSpecifier_tok;_tmp47B.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp47C=_cycalloc(sizeof(*_tmp47C));
_tmp47C[0]=({struct Cyc_Absyn_TunionType_struct _tmp47D;_tmp47D.tag=2;_tmp47D.f1=({
struct Cyc_Absyn_TunionInfo _tmp47E;_tmp47E.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp47F=_cycalloc(sizeof(*_tmp47F));_tmp47F[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp480;_tmp480.tag=0;_tmp480.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp481;
_tmp481.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp481.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp481;});_tmp480;});
_tmp47F;}));_tmp47E.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp47E.rgn=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp47E;});_tmp47D;});_tmp47C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp47B;});_tmp47A;});break;case 111: _LL2E2: {void*
_tmp482=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp48B=_cycalloc(sizeof(*
_tmp48B));_tmp48B->v=(void*)((void*)3);_tmp48B;}),0);yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp484;_tmp484.tag=Cyc_TypeSpecifier_tok;_tmp484.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp485=_cycalloc(sizeof(*_tmp485));
_tmp485[0]=({struct Cyc_Absyn_TunionType_struct _tmp486;_tmp486.tag=2;_tmp486.f1=({
struct Cyc_Absyn_TunionInfo _tmp487;_tmp487.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp488=_cycalloc(sizeof(*_tmp488));_tmp488[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp489;_tmp489.tag=0;_tmp489.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp48A;
_tmp48A.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp48A.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp48A;});_tmp489;});
_tmp488;}));_tmp487.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp487.rgn=(
void*)_tmp482;_tmp487;});_tmp486;});_tmp485;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp484;});_tmp483;});break;}case 112: _LL2E3: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp48C=_cycalloc(sizeof(*_tmp48C));
_tmp48C[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp48D;_tmp48D.tag=Cyc_TypeSpecifier_tok;
_tmp48D.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp48E=_cycalloc(sizeof(*_tmp48E));_tmp48E[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp48F;_tmp48F.tag=3;_tmp48F.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp490;
_tmp490.field_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*
_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp492;_tmp492.tag=0;_tmp492.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp493;
_tmp493.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp493.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp493.is_xtunion=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp493;});_tmp492;});
_tmp491;}));_tmp490.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp490;});
_tmp48F;});_tmp48E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp48D;});_tmp48C;});break;case 113: _LL2E4: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp494=_cycalloc_atomic(sizeof(*_tmp494));
_tmp494[0]=({struct Cyc_Bool_tok_struct _tmp495;_tmp495.tag=Cyc_Bool_tok;_tmp495.f1=
0;_tmp495;});_tmp494;});break;case 114: _LL2E5: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp496=_cycalloc_atomic(sizeof(*_tmp496));_tmp496[0]=({struct Cyc_Bool_tok_struct
_tmp497;_tmp497.tag=Cyc_Bool_tok;_tmp497.f1=1;_tmp497;});_tmp496;});break;case
115: _LL2E6: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp498=_cycalloc(
sizeof(*_tmp498));_tmp498[0]=({struct Cyc_TunionFieldList_tok_struct _tmp499;
_tmp499.tag=Cyc_TunionFieldList_tok;_tmp499.f1=({struct Cyc_List_List*_tmp49A=
_cycalloc(sizeof(*_tmp49A));_tmp49A->hd=Cyc_yyget_TunionField_tok(yyvs[
yyvsp_offset]);_tmp49A->tl=0;_tmp49A;});_tmp499;});_tmp498;});break;case 116:
_LL2E7: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp49B=_cycalloc(
sizeof(*_tmp49B));_tmp49B[0]=({struct Cyc_TunionFieldList_tok_struct _tmp49C;
_tmp49C.tag=Cyc_TunionFieldList_tok;_tmp49C.f1=({struct Cyc_List_List*_tmp49D=
_cycalloc(sizeof(*_tmp49D));_tmp49D->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp49D->tl=0;_tmp49D;});
_tmp49C;});_tmp49B;});break;case 117: _LL2E8: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp49F;_tmp49F.tag=Cyc_TunionFieldList_tok;_tmp49F.f1=({struct Cyc_List_List*
_tmp4A0=_cycalloc(sizeof(*_tmp4A0));_tmp4A0->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4A0->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[yyvsp_offset]);_tmp4A0;});_tmp49F;});_tmp49E;});break;case 118: _LL2E9: yyval=(
void*)({struct Cyc_TunionFieldList_tok_struct*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));
_tmp4A1[0]=({struct Cyc_TunionFieldList_tok_struct _tmp4A2;_tmp4A2.tag=Cyc_TunionFieldList_tok;
_tmp4A2.f1=({struct Cyc_List_List*_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3->hd=
Cyc_yyget_TunionField_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]);_tmp4A3->tl=Cyc_yyget_TunionFieldList_tok(yyvs[yyvsp_offset]);_tmp4A3;});
_tmp4A2;});_tmp4A1;});break;case 119: _LL2EA: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4[0]=({struct Cyc_Scope_tok_struct
_tmp4A5;_tmp4A5.tag=Cyc_Scope_tok;_tmp4A5.f1=(void*)((void*)2);_tmp4A5;});
_tmp4A4;});break;case 120: _LL2EB: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6[0]=({struct Cyc_Scope_tok_struct
_tmp4A7;_tmp4A7.tag=Cyc_Scope_tok;_tmp4A7.f1=(void*)((void*)3);_tmp4A7;});
_tmp4A6;});break;case 121: _LL2EC: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4A8=_cycalloc(sizeof(*_tmp4A8));_tmp4A8[0]=({struct Cyc_Scope_tok_struct
_tmp4A9;_tmp4A9.tag=Cyc_Scope_tok;_tmp4A9.f1=(void*)((void*)0);_tmp4A9;});
_tmp4A8;});break;case 122: _LL2ED: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA[0]=({struct Cyc_TunionField_tok_struct
_tmp4AB;_tmp4AB.tag=Cyc_TunionField_tok;_tmp4AB.f1=({struct Cyc_Absyn_Tunionfield*
_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC->name=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp4AC->tvs=0;_tmp4AC->typs=0;_tmp4AC->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4AC->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4AC;});_tmp4AB;});
_tmp4AA;});break;case 123: _LL2EE: {struct Cyc_List_List*_tmp4AD=((struct Cyc_List_List*(*)(
struct _tuple8*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));struct Cyc_List_List*
_tmp4AE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));_tmp4AF[0]=({
struct Cyc_TunionField_tok_struct _tmp4B0;_tmp4B0.tag=Cyc_TunionField_tok;_tmp4B0.f1=({
struct Cyc_Absyn_Tunionfield*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp4B1->tvs=_tmp4AE;_tmp4B1->typs=_tmp4AD;_tmp4B1->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4B1->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp4B1;});_tmp4B0;});
_tmp4AF;});break;}case 124: _LL2EF: yyval=yyvs[yyvsp_offset];break;case 125: _LL2F0:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));
_tmp4B2[0]=({struct Cyc_Declarator_tok_struct _tmp4B3;_tmp4B3.tag=Cyc_Declarator_tok;
_tmp4B3.f1=({struct Cyc_Parse_Declarator*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));
_tmp4B4->id=(Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]))->id;_tmp4B4->tms=Cyc_List_imp_append(
Cyc_yyget_TypeModifierList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]))->tms);_tmp4B4;});
_tmp4B3;});_tmp4B2;});break;case 126: _LL2F1: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5[0]=({struct Cyc_Declarator_tok_struct
_tmp4B6;_tmp4B6.tag=Cyc_Declarator_tok;_tmp4B6.f1=({struct Cyc_Parse_Declarator*
_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7->id=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp4B7->tms=0;_tmp4B7;});_tmp4B6;});_tmp4B5;});break;case 127:
_LL2F2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 128: _LL2F3: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4B8=_cycalloc(
sizeof(*_tmp4B8));_tmp4B8[0]=({struct Cyc_Declarator_tok_struct _tmp4B9;_tmp4B9.tag=
Cyc_Declarator_tok;_tmp4B9.f1=({struct Cyc_Parse_Declarator*_tmp4BA=_cycalloc(
sizeof(*_tmp4BA));_tmp4BA->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->id;_tmp4BA->tms=({
struct Cyc_List_List*_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB->hd=(void*)((
void*)0);_tmp4BB->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->tms;_tmp4BB;});_tmp4BA;});
_tmp4B9;});_tmp4B8;});break;case 129: _LL2F4: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4BC=_cycalloc(sizeof(*_tmp4BC));_tmp4BC[0]=({struct Cyc_Declarator_tok_struct
_tmp4BD;_tmp4BD.tag=Cyc_Declarator_tok;_tmp4BD.f1=({struct Cyc_Parse_Declarator*
_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4BE->tms=({
struct Cyc_List_List*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));
_tmp4C0[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp4C1;_tmp4C1.tag=0;_tmp4C1.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4C1;});_tmp4C0;}));_tmp4BF->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp4BF;});_tmp4BE;});
_tmp4BD;});_tmp4BC;});break;case 130: _LL2F5: {struct _tuple13 _tmp4C3;struct Cyc_List_List*
_tmp4C4;struct Cyc_Core_Opt*_tmp4C5;struct Cyc_Absyn_VarargInfo*_tmp4C6;int _tmp4C7;
struct Cyc_List_List*_tmp4C8;struct _tuple13*_tmp4C2=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4C3=*_tmp4C2;_LL2FC:
_tmp4C8=_tmp4C3.f1;goto _LL2FB;_LL2FB: _tmp4C7=_tmp4C3.f2;goto _LL2FA;_LL2FA:
_tmp4C6=_tmp4C3.f3;goto _LL2F9;_LL2F9: _tmp4C5=_tmp4C3.f4;goto _LL2F8;_LL2F8:
_tmp4C4=_tmp4C3.f5;goto _LL2F7;_LL2F7: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9[0]=({struct Cyc_Declarator_tok_struct
_tmp4CA;_tmp4CA.tag=Cyc_Declarator_tok;_tmp4CA.f1=({struct Cyc_Parse_Declarator*
_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4CB->tms=({
struct Cyc_List_List*_tmp4CC=_cycalloc(sizeof(*_tmp4CC));_tmp4CC->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4CD=_cycalloc(sizeof(*_tmp4CD));
_tmp4CD[0]=({struct Cyc_Absyn_Function_mod_struct _tmp4CE;_tmp4CE.tag=2;_tmp4CE.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp4CF=_cycalloc(sizeof(*
_tmp4CF));_tmp4CF[0]=({struct Cyc_Absyn_WithTypes_struct _tmp4D0;_tmp4D0.tag=1;
_tmp4D0.f1=_tmp4C8;_tmp4D0.f2=_tmp4C7;_tmp4D0.f3=_tmp4C6;_tmp4D0.f4=_tmp4C5;
_tmp4D0.f5=_tmp4C4;_tmp4D0;});_tmp4CF;}));_tmp4CE;});_tmp4CD;}));_tmp4CC->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp4CC;});_tmp4CB;});_tmp4CA;});_tmp4C9;});break;}case 131: _LL2F6:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4D1=_cycalloc(sizeof(*_tmp4D1));
_tmp4D1[0]=({struct Cyc_Declarator_tok_struct _tmp4D2;_tmp4D2.tag=Cyc_Declarator_tok;
_tmp4D2.f1=({struct Cyc_Parse_Declarator*_tmp4D3=_cycalloc(sizeof(*_tmp4D3));
_tmp4D3->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp4D3->tms=({struct Cyc_List_List*_tmp4D4=_cycalloc(
sizeof(*_tmp4D4));_tmp4D4->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp4D6;_tmp4D6.tag=2;_tmp4D6.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp4D7=_cycalloc(sizeof(*_tmp4D7));_tmp4D7[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp4D8;_tmp4D8.tag=1;_tmp4D8.f1=0;_tmp4D8.f2=0;_tmp4D8.f3=0;_tmp4D8.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D8.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4D8;});_tmp4D7;}));
_tmp4D6;});_tmp4D5;}));_tmp4D4->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp4D4;});_tmp4D3;});
_tmp4D2;});_tmp4D1;});break;case 132: _LL2FD: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9[0]=({struct Cyc_Declarator_tok_struct
_tmp4DA;_tmp4DA.tag=Cyc_Declarator_tok;_tmp4DA.f1=({struct Cyc_Parse_Declarator*
_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4DB->tms=({
struct Cyc_List_List*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));
_tmp4DD[0]=({struct Cyc_Absyn_Function_mod_struct _tmp4DE;_tmp4DE.tag=2;_tmp4DE.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));
_tmp4DF[0]=({struct Cyc_Absyn_NoTypes_struct _tmp4E0;_tmp4E0.tag=0;_tmp4E0.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E0.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4E0;});_tmp4DF;}));_tmp4DE;});_tmp4DD;}));_tmp4DC->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp4DC;});_tmp4DB;});_tmp4DA;});_tmp4D9;});break;case 133: _LL2FE: {
struct Cyc_List_List*_tmp4E1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));_tmp4E2[0]=({
struct Cyc_Declarator_tok_struct _tmp4E3;_tmp4E3.tag=Cyc_Declarator_tok;_tmp4E3.f1=({
struct Cyc_Parse_Declarator*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4E4->tms=({
struct Cyc_List_List*_tmp4E5=_cycalloc(sizeof(*_tmp4E5));_tmp4E5->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));
_tmp4E6[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp4E7;_tmp4E7.tag=3;_tmp4E7.f1=
_tmp4E1;_tmp4E7.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4E7.f3=0;_tmp4E7;});_tmp4E6;}));_tmp4E5->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp4E5;});
_tmp4E4;});_tmp4E3;});_tmp4E2;});break;}case 134: _LL2FF: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4E8=_cycalloc(sizeof(*_tmp4E8));_tmp4E8[0]=({struct Cyc_Declarator_tok_struct
_tmp4E9;_tmp4E9.tag=Cyc_Declarator_tok;_tmp4E9.f1=({struct Cyc_Parse_Declarator*
_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp4EA->tms=({
struct Cyc_List_List*_tmp4EB=_cycalloc(sizeof(*_tmp4EB));_tmp4EB->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp4EC=_cycalloc(sizeof(*_tmp4EC));
_tmp4EC[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp4ED;_tmp4ED.tag=4;_tmp4ED.f1=
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);
_tmp4ED.f2=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp4ED;});_tmp4EC;}));
_tmp4EB->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp4EB;});_tmp4EA;});_tmp4E9;});_tmp4E8;});break;case
135: _LL300: Cyc_Parse_err(_tag_arr("identifier has not been declared as a typedef",
sizeof(unsigned char),46),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(void*)({
struct Cyc_Declarator_tok_struct*_tmp4EE=_cycalloc(sizeof(*_tmp4EE));_tmp4EE[0]=({
struct Cyc_Declarator_tok_struct _tmp4EF;_tmp4EF.tag=Cyc_Declarator_tok;_tmp4EF.f1=({
struct Cyc_Parse_Declarator*_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0->id=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp4F0->tms=0;_tmp4F0;});_tmp4EF;});_tmp4EE;});break;case
136: _LL301: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4F1=_cycalloc(
sizeof(*_tmp4F1));_tmp4F1[0]=({struct Cyc_Declarator_tok_struct _tmp4F2;_tmp4F2.tag=
Cyc_Declarator_tok;_tmp4F2.f1=({struct Cyc_Parse_Declarator*_tmp4F3=_cycalloc(
sizeof(*_tmp4F3));_tmp4F3->id=Cyc_Absyn_exn_name;_tmp4F3->tms=0;_tmp4F3;});
_tmp4F2;});_tmp4F1;});break;case 137: _LL302: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp4F5;_tmp4F5.tag=Cyc_TypeModifierList_tok;_tmp4F5.f1=({struct Cyc_List_List*
_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp4F8;_tmp4F8.tag=1;_tmp4F8.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4F8.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4F8.f3=Cyc_Absyn_empty_tqual();
_tmp4F8;});_tmp4F7;}));_tmp4F6->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]),0);_tmp4F6;});_tmp4F5;});_tmp4F4;});break;case 138: _LL303:
yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*_tmp4F9=_cycalloc(sizeof(*
_tmp4F9));_tmp4F9[0]=({struct Cyc_TypeModifierList_tok_struct _tmp4FA;_tmp4FA.tag=
Cyc_TypeModifierList_tok;_tmp4FA.f1=({struct Cyc_List_List*_tmp4FB=_cycalloc(
sizeof(*_tmp4FB));_tmp4FB->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp4FD;_tmp4FD.tag=1;_tmp4FD.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4FD.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4FD.f3=Cyc_yyget_TypeQual_tok(
yyvs[yyvsp_offset]);_tmp4FD;});_tmp4FC;}));_tmp4FB->tl=Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0);_tmp4FB;});_tmp4FA;});
_tmp4F9;});break;case 139: _LL304: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp4FF;_tmp4FF.tag=Cyc_TypeModifierList_tok;_tmp4FF.f1=({struct Cyc_List_List*
_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp502;_tmp502.tag=1;_tmp502.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp502.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp502.f3=Cyc_Absyn_empty_tqual();
_tmp502;});_tmp501;}));_tmp500->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp500;});_tmp4FF;});_tmp4FE;});break;case 140: _LL305: yyval=(
void*)({struct Cyc_TypeModifierList_tok_struct*_tmp503=_cycalloc(sizeof(*_tmp503));
_tmp503[0]=({struct Cyc_TypeModifierList_tok_struct _tmp504;_tmp504.tag=Cyc_TypeModifierList_tok;
_tmp504.f1=({struct Cyc_List_List*_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505->hd=(
void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp506=_cycalloc(sizeof(*
_tmp506));_tmp506[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp507;_tmp507.tag=1;
_tmp507.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp507.f2=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp507.f3=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp507;});_tmp506;}));
_tmp505->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp505;});_tmp504;});_tmp503;});break;case 141: _LL306: yyval=(
void*)({struct Cyc_Pointer_Sort_tok_struct*_tmp508=_cycalloc(sizeof(*_tmp508));
_tmp508[0]=({struct Cyc_Pointer_Sort_tok_struct _tmp509;_tmp509.tag=Cyc_Pointer_Sort_tok;
_tmp509.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*_tmp50A=_cycalloc(
sizeof(*_tmp50A));_tmp50A[0]=({struct Cyc_Absyn_Nullable_ps_struct _tmp50B;_tmp50B.tag=
1;_tmp50B.f1=Cyc_Absyn_exp_unsigned_one;_tmp50B;});_tmp50A;}));_tmp509;});
_tmp508;});break;case 142: _LL307: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp50D;_tmp50D.tag=Cyc_Pointer_Sort_tok;_tmp50D.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp50F;_tmp50F.tag=0;_tmp50F.f1=Cyc_Absyn_exp_unsigned_one;_tmp50F;});_tmp50E;}));
_tmp50D;});_tmp50C;});break;case 143: _LL308: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp511;_tmp511.tag=Cyc_Pointer_Sort_tok;_tmp511.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*
_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp513;_tmp513.tag=1;_tmp513.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp513;});_tmp512;}));
_tmp511;});_tmp510;});break;case 144: _LL309: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp515;_tmp515.tag=Cyc_Pointer_Sort_tok;_tmp515.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp517;_tmp517.tag=0;_tmp517.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp517;});_tmp516;}));
_tmp515;});_tmp514;});break;case 145: _LL30A: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp518=_cycalloc(sizeof(*_tmp518));_tmp518[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp519;_tmp519.tag=Cyc_Pointer_Sort_tok;_tmp519.f1=(void*)((void*)0);_tmp519;});
_tmp518;});break;case 146: _LL30B: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp51A=
_cycalloc(sizeof(*_tmp51A));_tmp51A[0]=({struct Cyc_Type_tok_struct _tmp51B;
_tmp51B.tag=Cyc_Type_tok;_tmp51B.f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp51C=_cycalloc(sizeof(*_tmp51C));_tmp51C->v=(void*)((void*)3);_tmp51C;}),0);
_tmp51B;});_tmp51A;});break;case 147: _LL30C: yyval=yyvs[yyvsp_offset];break;case
148: _LL30D: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(
void*)3);yyval=yyvs[yyvsp_offset];break;case 149: _LL30E: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D[0]=({struct Cyc_Type_tok_struct
_tmp51E;_tmp51E.tag=Cyc_Type_tok;_tmp51E.f1=(void*)Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp51F=_cycalloc(sizeof(*_tmp51F));_tmp51F->v=(void*)((void*)3);
_tmp51F;}),0);_tmp51E;});_tmp51D;});break;case 150: _LL30F: yyval=yyvs[yyvsp_offset];
break;case 151: _LL310: yyval=(void*)({struct Cyc_TypeQual_tok_struct*_tmp520=
_cycalloc_atomic(sizeof(*_tmp520));_tmp520[0]=({struct Cyc_TypeQual_tok_struct
_tmp521;_tmp521.tag=Cyc_TypeQual_tok;_tmp521.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[yyvsp_offset]));_tmp521;});_tmp520;});break;case 152: _LL311: yyval=(void*)({
struct Cyc_YY1_struct*_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522[0]=({struct Cyc_YY1_struct
_tmp523;_tmp523.tag=Cyc_YY1;_tmp523.f1=({struct _tuple13*_tmp524=_cycalloc(
sizeof(*_tmp524));_tmp524->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp524->f2=0;_tmp524->f3=
0;_tmp524->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp524->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp524;});_tmp523;});_tmp522;});break;case 153: _LL312: yyval=(void*)({struct Cyc_YY1_struct*
_tmp525=_cycalloc(sizeof(*_tmp525));_tmp525[0]=({struct Cyc_YY1_struct _tmp526;
_tmp526.tag=Cyc_YY1;_tmp526.f1=({struct _tuple13*_tmp527=_cycalloc(sizeof(*
_tmp527));_tmp527->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]));_tmp527->f2=1;_tmp527->f3=0;_tmp527->f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp527->f5=Cyc_yyget_Rgnorder_tok(
yyvs[yyvsp_offset]);_tmp527;});_tmp526;});_tmp525;});break;case 154: _LL313: {
struct _tuple2 _tmp529;void*_tmp52A;struct Cyc_Absyn_Tqual _tmp52B;struct Cyc_Core_Opt*
_tmp52C;struct _tuple2*_tmp528=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp529=*_tmp528;_LL318:
_tmp52C=_tmp529.f1;goto _LL317;_LL317: _tmp52B=_tmp529.f2;goto _LL316;_LL316:
_tmp52A=_tmp529.f3;goto _LL315;_LL315: {struct Cyc_Absyn_VarargInfo*_tmp52D=({
struct Cyc_Absyn_VarargInfo*_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531->name=
_tmp52C;_tmp531->tq=_tmp52B;_tmp531->type=(void*)_tmp52A;_tmp531->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp531;});yyval=(
void*)({struct Cyc_YY1_struct*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E[0]=({
struct Cyc_YY1_struct _tmp52F;_tmp52F.tag=Cyc_YY1;_tmp52F.f1=({struct _tuple13*
_tmp530=_cycalloc(sizeof(*_tmp530));_tmp530->f1=0;_tmp530->f2=0;_tmp530->f3=
_tmp52D;_tmp530->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp530->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp530;});_tmp52F;});_tmp52E;});break;}}case 155: _LL314: {struct _tuple2 _tmp533;
void*_tmp534;struct Cyc_Absyn_Tqual _tmp535;struct Cyc_Core_Opt*_tmp536;struct
_tuple2*_tmp532=Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp533=*_tmp532;_LL31D: _tmp536=_tmp533.f1;goto _LL31C;
_LL31C: _tmp535=_tmp533.f2;goto _LL31B;_LL31B: _tmp534=_tmp533.f3;goto _LL31A;_LL31A: {
struct Cyc_Absyn_VarargInfo*_tmp537=({struct Cyc_Absyn_VarargInfo*_tmp53B=
_cycalloc(sizeof(*_tmp53B));_tmp53B->name=_tmp536;_tmp53B->tq=_tmp535;_tmp53B->type=(
void*)_tmp534;_tmp53B->inject=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp53B;});yyval=(void*)({
struct Cyc_YY1_struct*_tmp538=_cycalloc(sizeof(*_tmp538));_tmp538[0]=({struct Cyc_YY1_struct
_tmp539;_tmp539.tag=Cyc_YY1;_tmp539.f1=({struct _tuple13*_tmp53A=_cycalloc(
sizeof(*_tmp53A));_tmp53A->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp53A->f2=0;_tmp53A->f3=
_tmp537;_tmp53A->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp53A->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp53A;});_tmp539;});_tmp538;});break;}}case 156: _LL319: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp53C=_cycalloc(sizeof(*_tmp53C));_tmp53C[0]=({struct Cyc_Type_tok_struct
_tmp53D;_tmp53D.tag=Cyc_Type_tok;_tmp53D.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp53E=_cycalloc(
sizeof(*_tmp53E));_tmp53E[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp53F;_tmp53F.tag=
1;_tmp53F.f1=0;_tmp53F;});_tmp53E;}));_tmp53D;});_tmp53C;});break;case 157: _LL31E:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp540=_cycalloc(sizeof(*_tmp540));
_tmp540[0]=({struct Cyc_Type_tok_struct _tmp541;_tmp541.tag=Cyc_Type_tok;_tmp541.f1=(
void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp542=_cycalloc(
sizeof(*_tmp542));_tmp542[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp543;_tmp543.tag=0;
_tmp543.f1=(void*)Cyc_yyget_Kind_tok(yyvs[yyvsp_offset]);_tmp543;});_tmp542;}));
_tmp541;});_tmp540;});break;case 158: _LL31F: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp544=_cycalloc(sizeof(*_tmp544));_tmp544[0]=({struct Cyc_TypeOpt_tok_struct
_tmp545;_tmp545.tag=Cyc_TypeOpt_tok;_tmp545.f1=0;_tmp545;});_tmp544;});break;
case 159: _LL320: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp546=_cycalloc(
sizeof(*_tmp546));_tmp546[0]=({struct Cyc_TypeOpt_tok_struct _tmp547;_tmp547.tag=
Cyc_TypeOpt_tok;_tmp547.f1=({struct Cyc_Core_Opt*_tmp548=_cycalloc(sizeof(*
_tmp548));_tmp548->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp549=
_cycalloc(sizeof(*_tmp549));_tmp549[0]=({struct Cyc_Absyn_JoinEff_struct _tmp54A;
_tmp54A.tag=18;_tmp54A.f1=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp54A;});
_tmp549;}));_tmp548;});_tmp547;});_tmp546;});break;case 160: _LL321: yyval=(void*)({
struct Cyc_Rgnorder_tok_struct*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B[0]=({
struct Cyc_Rgnorder_tok_struct _tmp54C;_tmp54C.tag=Cyc_Rgnorder_tok;_tmp54C.f1=0;
_tmp54C;});_tmp54B;});break;case 161: _LL322: yyval=yyvs[yyvsp_offset];break;case
162: _LL323: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp54D=_cycalloc(
sizeof(*_tmp54D));_tmp54D[0]=({struct Cyc_Rgnorder_tok_struct _tmp54E;_tmp54E.tag=
Cyc_Rgnorder_tok;_tmp54E.f1=({struct Cyc_List_List*_tmp54F=_cycalloc(sizeof(*
_tmp54F));_tmp54F->hd=({struct _tuple16*_tmp550=_cycalloc(sizeof(*_tmp550));
_tmp550->f1=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp554;_tmp554.tag=0;_tmp554.f1=(void*)((void*)3);_tmp554;});_tmp553;}));
_tmp550->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),(void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp552;_tmp552.tag=0;_tmp552.f1=(void*)((void*)3);
_tmp552;});_tmp551;}));_tmp550;});_tmp54F->tl=0;_tmp54F;});_tmp54E;});_tmp54D;});
break;case 163: _LL324: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp555=
_cycalloc(sizeof(*_tmp555));_tmp555[0]=({struct Cyc_Rgnorder_tok_struct _tmp556;
_tmp556.tag=Cyc_Rgnorder_tok;_tmp556.f1=({struct Cyc_List_List*_tmp557=_cycalloc(
sizeof(*_tmp557));_tmp557->hd=({struct _tuple16*_tmp558=_cycalloc(sizeof(*_tmp558));
_tmp558->f1=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp55C;_tmp55C.tag=0;_tmp55C.f1=(void*)((void*)3);_tmp55C;});_tmp55B;}));
_tmp558->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp55A;_tmp55A.tag=0;_tmp55A.f1=(void*)((void*)3);_tmp55A;});_tmp559;}));
_tmp558;});_tmp557->tl=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);_tmp557;});
_tmp556;});_tmp555;});break;case 164: _LL325: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp55D=_cycalloc_atomic(sizeof(*_tmp55D));_tmp55D[0]=({struct Cyc_Bool_tok_struct
_tmp55E;_tmp55E.tag=Cyc_Bool_tok;_tmp55E.f1=0;_tmp55E;});_tmp55D;});break;case
165: _LL326: if(Cyc_Std_zstrcmp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),_tag_arr("inject",
sizeof(unsigned char),7))!= 0){Cyc_Parse_err(_tag_arr("missing type in function declaration",
sizeof(unsigned char),37),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp55F=_cycalloc_atomic(sizeof(*_tmp55F));_tmp55F[0]=({struct Cyc_Bool_tok_struct
_tmp560;_tmp560.tag=Cyc_Bool_tok;_tmp560.f1=1;_tmp560;});_tmp55F;});break;case
166: _LL327: yyval=yyvs[yyvsp_offset];break;case 167: _LL328: yyval=(void*)({struct
Cyc_TypeList_tok_struct*_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561[0]=({struct
Cyc_TypeList_tok_struct _tmp562;_tmp562.tag=Cyc_TypeList_tok;_tmp562.f1=Cyc_List_imp_append(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]));_tmp562;});_tmp561;});break;case 168:
_LL329: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp563=_cycalloc(sizeof(*
_tmp563));_tmp563[0]=({struct Cyc_TypeList_tok_struct _tmp564;_tmp564.tag=Cyc_TypeList_tok;
_tmp564.f1=0;_tmp564;});_tmp563;});break;case 169: _LL32A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 170: _LL32B:
yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp565=_cycalloc(sizeof(*_tmp565));
_tmp565[0]=({struct Cyc_TypeList_tok_struct _tmp566;_tmp566.tag=Cyc_TypeList_tok;
_tmp566.f1=({struct Cyc_List_List*_tmp567=_cycalloc(sizeof(*_tmp567));_tmp567->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp568=_cycalloc(sizeof(*_tmp568));
_tmp568[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp569;_tmp569.tag=19;_tmp569.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp569;});_tmp568;}));_tmp567->tl=0;_tmp567;});_tmp566;});_tmp565;});
break;case 171: _LL32C: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
yyvsp_offset]),(void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp56A=
_cycalloc(sizeof(*_tmp56A));_tmp56A[0]=({struct Cyc_TypeList_tok_struct _tmp56B;
_tmp56B.tag=Cyc_TypeList_tok;_tmp56B.f1=({struct Cyc_List_List*_tmp56C=_cycalloc(
sizeof(*_tmp56C));_tmp56C->hd=(void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);
_tmp56C->tl=0;_tmp56C;});_tmp56B;});_tmp56A;});break;case 172: _LL32D: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D[0]=({struct Cyc_TypeList_tok_struct
_tmp56E;_tmp56E.tag=Cyc_TypeList_tok;_tmp56E.f1=({struct Cyc_List_List*_tmp56F=
_cycalloc(sizeof(*_tmp56F));_tmp56F->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp571;_tmp571.tag=17;_tmp571.f1=(void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);
_tmp571;});_tmp570;}));_tmp56F->tl=0;_tmp56F;});_tmp56E;});_tmp56D;});break;case
173: _LL32E: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(void*)({
struct Cyc_TypeList_tok_struct*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572[0]=({
struct Cyc_TypeList_tok_struct _tmp573;_tmp573.tag=Cyc_TypeList_tok;_tmp573.f1=({
struct Cyc_List_List*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp575=_cycalloc(sizeof(*_tmp575));
_tmp575[0]=({struct Cyc_Absyn_AccessEff_struct _tmp576;_tmp576.tag=17;_tmp576.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp576;});_tmp575;}));_tmp574->tl=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);
_tmp574;});_tmp573;});_tmp572;});break;case 174: _LL32F: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp578;_tmp578.tag=Cyc_ParamDeclList_tok;_tmp578.f1=({struct Cyc_List_List*
_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->hd=Cyc_yyget_ParamDecl_tok(yyvs[
yyvsp_offset]);_tmp579->tl=0;_tmp579;});_tmp578;});_tmp577;});break;case 175:
_LL330: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*_tmp57A=_cycalloc(
sizeof(*_tmp57A));_tmp57A[0]=({struct Cyc_ParamDeclList_tok_struct _tmp57B;_tmp57B.tag=
Cyc_ParamDeclList_tok;_tmp57B.f1=({struct Cyc_List_List*_tmp57C=_cycalloc(sizeof(*
_tmp57C));_tmp57C->hd=Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset]);_tmp57C->tl=
Cyc_yyget_ParamDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp57C;});_tmp57B;});_tmp57A;});break;case 176: _LL331: {
struct _tuple14 _tmp57E;struct Cyc_List_List*_tmp57F;struct Cyc_List_List*_tmp580;
struct Cyc_Absyn_Tqual _tmp581;struct _tuple14*_tmp57D=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp57E=*_tmp57D;
_LL336: _tmp581=_tmp57E.f1;goto _LL335;_LL335: _tmp580=_tmp57E.f2;goto _LL334;_LL334:
_tmp57F=_tmp57E.f3;goto _LL333;_LL333: {struct Cyc_Parse_Declarator _tmp583;struct
Cyc_List_List*_tmp584;struct _tuple1*_tmp585;struct Cyc_Parse_Declarator*_tmp582=
Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]);_tmp583=*_tmp582;_LL339: _tmp585=
_tmp583.id;goto _LL338;_LL338: _tmp584=_tmp583.tms;goto _LL337;_LL337: {void*
_tmp586=Cyc_Parse_speclist2typ(_tmp580,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp588;struct Cyc_List_List*_tmp589;void*_tmp58A;struct Cyc_Absyn_Tqual _tmp58B;
struct _tuple5 _tmp587=Cyc_Parse_apply_tms(_tmp581,_tmp586,_tmp57F,_tmp584);_LL33E:
_tmp58B=_tmp587.f1;goto _LL33D;_LL33D: _tmp58A=_tmp587.f2;goto _LL33C;_LL33C:
_tmp589=_tmp587.f3;goto _LL33B;_LL33B: _tmp588=_tmp587.f4;goto _LL33A;_LL33A: if(
_tmp589 != 0){Cyc_Parse_err(_tag_arr("parameter with bad type params",sizeof(
unsigned char),31),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp585)){Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a namespace",sizeof(unsigned char),
47),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*_tmp58C=(struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt*_tmp591=_cycalloc(sizeof(*_tmp591));_tmp591->v=(*_tmp585).f2;
_tmp591;});if(_tmp588 != 0){({void*_tmp58D[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("extra attributes on parameter, ignoring",
sizeof(unsigned char),40),_tag_arr(_tmp58D,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp58E=_cycalloc(sizeof(*_tmp58E));_tmp58E[0]=({
struct Cyc_ParamDecl_tok_struct _tmp58F;_tmp58F.tag=Cyc_ParamDecl_tok;_tmp58F.f1=({
struct _tuple2*_tmp590=_cycalloc(sizeof(*_tmp590));_tmp590->f1=_tmp58C;_tmp590->f2=
_tmp58B;_tmp590->f3=_tmp58A;_tmp590;});_tmp58F;});_tmp58E;});break;}}}}case 177:
_LL332: {struct _tuple14 _tmp593;struct Cyc_List_List*_tmp594;struct Cyc_List_List*
_tmp595;struct Cyc_Absyn_Tqual _tmp596;struct _tuple14*_tmp592=Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset]);_tmp593=*_tmp592;_LL343: _tmp596=_tmp593.f1;goto _LL342;_LL342:
_tmp595=_tmp593.f2;goto _LL341;_LL341: _tmp594=_tmp593.f3;goto _LL340;_LL340: {void*
_tmp597=Cyc_Parse_speclist2typ(_tmp595,Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));if(_tmp594 != 0){({
void*_tmp598[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(unsigned char),38),_tag_arr(_tmp598,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599[0]=({
struct Cyc_ParamDecl_tok_struct _tmp59A;_tmp59A.tag=Cyc_ParamDecl_tok;_tmp59A.f1=({
struct _tuple2*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->f1=0;_tmp59B->f2=
_tmp596;_tmp59B->f3=_tmp597;_tmp59B;});_tmp59A;});_tmp599;});break;}}case 178:
_LL33F: {struct _tuple14 _tmp59D;struct Cyc_List_List*_tmp59E;struct Cyc_List_List*
_tmp59F;struct Cyc_Absyn_Tqual _tmp5A0;struct _tuple14*_tmp59C=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp59D=*_tmp59C;
_LL348: _tmp5A0=_tmp59D.f1;goto _LL347;_LL347: _tmp59F=_tmp59D.f2;goto _LL346;_LL346:
_tmp59E=_tmp59D.f3;goto _LL345;_LL345: {void*_tmp5A1=Cyc_Parse_speclist2typ(
_tmp59F,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp5A2=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[yyvsp_offset]))->tms;struct Cyc_List_List*_tmp5A4;struct Cyc_List_List*
_tmp5A5;void*_tmp5A6;struct Cyc_Absyn_Tqual _tmp5A7;struct _tuple5 _tmp5A3=Cyc_Parse_apply_tms(
_tmp5A0,_tmp5A1,_tmp59E,_tmp5A2);_LL34D: _tmp5A7=_tmp5A3.f1;goto _LL34C;_LL34C:
_tmp5A6=_tmp5A3.f2;goto _LL34B;_LL34B: _tmp5A5=_tmp5A3.f3;goto _LL34A;_LL34A:
_tmp5A4=_tmp5A3.f4;goto _LL349;_LL349: if(_tmp5A5 != 0){({void*_tmp5A8[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad type parameters on formal argument, ignoring",
sizeof(unsigned char),49),_tag_arr(_tmp5A8,sizeof(void*),0));});}if(_tmp5A4 != 0){({
void*_tmp5A9[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(unsigned char),38),_tag_arr(_tmp5A9,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));_tmp5AA[0]=({
struct Cyc_ParamDecl_tok_struct _tmp5AB;_tmp5AB.tag=Cyc_ParamDecl_tok;_tmp5AB.f1=({
struct _tuple2*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));_tmp5AC->f1=0;_tmp5AC->f2=
_tmp5A7;_tmp5AC->f3=_tmp5A6;_tmp5AC;});_tmp5AB;});_tmp5AA;});break;}}case 179:
_LL344: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5AD=_cycalloc(sizeof(*
_tmp5AD));_tmp5AD[0]=({struct Cyc_IdList_tok_struct _tmp5AE;_tmp5AE.tag=Cyc_IdList_tok;
_tmp5AE.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(yyvs[yyvsp_offset]));_tmp5AE;});_tmp5AD;});break;case 180:
_LL34E: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5AF=_cycalloc(sizeof(*
_tmp5AF));_tmp5AF[0]=({struct Cyc_IdList_tok_struct _tmp5B0;_tmp5B0.tag=Cyc_IdList_tok;
_tmp5B0.f1=({struct Cyc_List_List*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1->hd=({
struct _tagged_arr*_tmp5B2=_cycalloc(sizeof(*_tmp5B2));_tmp5B2[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp5B2;});_tmp5B1->tl=0;_tmp5B1;});_tmp5B0;});_tmp5AF;});
break;case 181: _LL34F: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5B3=
_cycalloc(sizeof(*_tmp5B3));_tmp5B3[0]=({struct Cyc_IdList_tok_struct _tmp5B4;
_tmp5B4.tag=Cyc_IdList_tok;_tmp5B4.f1=({struct Cyc_List_List*_tmp5B5=_cycalloc(
sizeof(*_tmp5B5));_tmp5B5->hd=({struct _tagged_arr*_tmp5B6=_cycalloc(sizeof(*
_tmp5B6));_tmp5B6[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp5B6;});_tmp5B5->tl=
Cyc_yyget_IdList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5B5;});_tmp5B4;});_tmp5B3;});break;case 182: _LL350: yyval=yyvs[yyvsp_offset];
break;case 183: _LL351: yyval=yyvs[yyvsp_offset];break;case 184: _LL352: yyval=(void*)({
struct Cyc_Exp_tok_struct*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7[0]=({struct
Cyc_Exp_tok_struct _tmp5B8;_tmp5B8.tag=Cyc_Exp_tok;_tmp5B8.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));
_tmp5B9[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5BA;_tmp5BA.tag=34;
_tmp5BA.f1=0;_tmp5BA.f2=0;_tmp5BA;});_tmp5B9;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5B8;});_tmp5B7;});break;case 185: _LL353: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB[0]=({
struct Cyc_Exp_tok_struct _tmp5BC;_tmp5BC.tag=Cyc_Exp_tok;_tmp5BC.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));
_tmp5BD[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5BE;_tmp5BE.tag=34;
_tmp5BE.f1=0;_tmp5BE.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp5BE;});_tmp5BD;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5BC;});_tmp5BB;});break;case 186: _LL354: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF[0]=({
struct Cyc_Exp_tok_struct _tmp5C0;_tmp5C0.tag=Cyc_Exp_tok;_tmp5C0.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));
_tmp5C1[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5C2;_tmp5C2.tag=34;
_tmp5C2.f1=0;_tmp5C2.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp5C2;});_tmp5C1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5C0;});_tmp5BF;});break;case 187: _LL355: {struct
Cyc_Absyn_Vardecl*_tmp5C3=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp5C9=
_cycalloc(sizeof(*_tmp5C9));_tmp5C9->f1=(void*)0;_tmp5C9->f2=({struct _tagged_arr*
_tmp5CA=_cycalloc(sizeof(*_tmp5CA));_tmp5CA[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp5CA;});_tmp5C9;}),
Cyc_Absyn_uint_t,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));_tmp5C3->tq=({
struct Cyc_Absyn_Tqual _tmp5C4;_tmp5C4.q_const=1;_tmp5C4.q_volatile=0;_tmp5C4.q_restrict=
1;_tmp5C4;});yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp5C5=_cycalloc(sizeof(*
_tmp5C5));_tmp5C5[0]=({struct Cyc_Exp_tok_struct _tmp5C6;_tmp5C6.tag=Cyc_Exp_tok;
_tmp5C6.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp5C7=_cycalloc(sizeof(*_tmp5C7));_tmp5C7[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp5C8;_tmp5C8.tag=27;_tmp5C8.f1=_tmp5C3;_tmp5C8.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5C8.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5C8;});_tmp5C7;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmp5C6;});
_tmp5C5;});break;}case 188: _LL356: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp5CB=_cycalloc(sizeof(*_tmp5CB));_tmp5CB[0]=({struct Cyc_InitializerList_tok_struct
_tmp5CC;_tmp5CC.tag=Cyc_InitializerList_tok;_tmp5CC.f1=({struct Cyc_List_List*
_tmp5CD=_cycalloc(sizeof(*_tmp5CD));_tmp5CD->hd=({struct _tuple17*_tmp5CE=
_cycalloc(sizeof(*_tmp5CE));_tmp5CE->f1=0;_tmp5CE->f2=Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]);_tmp5CE;});_tmp5CD->tl=0;_tmp5CD;});_tmp5CC;});_tmp5CB;});break;
case 189: _LL357: yyval=(void*)({struct Cyc_InitializerList_tok_struct*_tmp5CF=
_cycalloc(sizeof(*_tmp5CF));_tmp5CF[0]=({struct Cyc_InitializerList_tok_struct
_tmp5D0;_tmp5D0.tag=Cyc_InitializerList_tok;_tmp5D0.f1=({struct Cyc_List_List*
_tmp5D1=_cycalloc(sizeof(*_tmp5D1));_tmp5D1->hd=({struct _tuple17*_tmp5D2=
_cycalloc(sizeof(*_tmp5D2));_tmp5D2->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D2->f2=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp5D2;});_tmp5D1->tl=0;_tmp5D1;});_tmp5D0;});_tmp5CF;});
break;case 190: _LL358: yyval=(void*)({struct Cyc_InitializerList_tok_struct*_tmp5D3=
_cycalloc(sizeof(*_tmp5D3));_tmp5D3[0]=({struct Cyc_InitializerList_tok_struct
_tmp5D4;_tmp5D4.tag=Cyc_InitializerList_tok;_tmp5D4.f1=({struct Cyc_List_List*
_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->hd=({struct _tuple17*_tmp5D6=
_cycalloc(sizeof(*_tmp5D6));_tmp5D6->f1=0;_tmp5D6->f2=Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]);_tmp5D6;});_tmp5D5->tl=Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D5;});_tmp5D4;});
_tmp5D3;});break;case 191: _LL359: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp5D7=_cycalloc(sizeof(*_tmp5D7));_tmp5D7[0]=({struct Cyc_InitializerList_tok_struct
_tmp5D8;_tmp5D8.tag=Cyc_InitializerList_tok;_tmp5D8.f1=({struct Cyc_List_List*
_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->hd=({struct _tuple17*_tmp5DA=
_cycalloc(sizeof(*_tmp5DA));_tmp5DA->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5DA->f2=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp5DA;});_tmp5D9->tl=Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5D9;});_tmp5D8;});
_tmp5D7;});break;case 192: _LL35A: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB[0]=({struct Cyc_DesignatorList_tok_struct
_tmp5DC;_tmp5DC.tag=Cyc_DesignatorList_tok;_tmp5DC.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp5DC;});_tmp5DB;});
break;case 193: _LL35B: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp5DD=
_cycalloc(sizeof(*_tmp5DD));_tmp5DD[0]=({struct Cyc_DesignatorList_tok_struct
_tmp5DE;_tmp5DE.tag=Cyc_DesignatorList_tok;_tmp5DE.f1=({struct Cyc_List_List*
_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[yyvsp_offset]);_tmp5DF->tl=0;_tmp5DF;});_tmp5DE;});_tmp5DD;});break;case 194:
_LL35C: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp5E0=_cycalloc(
sizeof(*_tmp5E0));_tmp5E0[0]=({struct Cyc_DesignatorList_tok_struct _tmp5E1;
_tmp5E1.tag=Cyc_DesignatorList_tok;_tmp5E1.f1=({struct Cyc_List_List*_tmp5E2=
_cycalloc(sizeof(*_tmp5E2));_tmp5E2->hd=(void*)Cyc_yyget_Designator_tok(yyvs[
yyvsp_offset]);_tmp5E2->tl=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E2;});_tmp5E1;});
_tmp5E0;});break;case 195: _LL35D: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp5E3=_cycalloc(sizeof(*_tmp5E3));_tmp5E3[0]=({struct Cyc_Designator_tok_struct
_tmp5E4;_tmp5E4.tag=Cyc_Designator_tok;_tmp5E4.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp5E5=_cycalloc(sizeof(*_tmp5E5));_tmp5E5[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp5E6;_tmp5E6.tag=0;_tmp5E6.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E6;});_tmp5E5;}));
_tmp5E4;});_tmp5E3;});break;case 196: _LL35E: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7[0]=({struct Cyc_Designator_tok_struct
_tmp5E8;_tmp5E8.tag=Cyc_Designator_tok;_tmp5E8.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9[0]=({struct Cyc_Absyn_FieldName_struct
_tmp5EA;_tmp5EA.tag=1;_tmp5EA.f1=({struct _tagged_arr*_tmp5EB=_cycalloc(sizeof(*
_tmp5EB));_tmp5EB[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp5EB;});_tmp5EA;});
_tmp5E9;}));_tmp5E8;});_tmp5E7;});break;case 197: _LL35F: {void*_tmp5EC=Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));struct Cyc_List_List*
_tmp5ED=(*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f3;if(_tmp5ED != 0){({
void*_tmp5EE[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("ignoring attributes in type",
sizeof(unsigned char),28),_tag_arr(_tmp5EE,sizeof(void*),0));});}{struct Cyc_Absyn_Tqual
_tmp5EF=(*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f1;yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0[0]=({
struct Cyc_ParamDecl_tok_struct _tmp5F1;_tmp5F1.tag=Cyc_ParamDecl_tok;_tmp5F1.f1=({
struct _tuple2*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->f1=0;_tmp5F2->f2=
_tmp5EF;_tmp5F2->f3=_tmp5EC;_tmp5F2;});_tmp5F1;});_tmp5F0;});break;}}case 198:
_LL360: {void*_tmp5F3=Cyc_Parse_speclist2typ((*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp5F4=(*Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f3;struct Cyc_Absyn_Tqual _tmp5F5=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f1;struct Cyc_List_List*
_tmp5F6=(Cyc_yyget_AbstractDeclarator_tok(yyvs[yyvsp_offset]))->tms;struct
_tuple5 _tmp5F7=Cyc_Parse_apply_tms(_tmp5F5,_tmp5F3,_tmp5F4,_tmp5F6);if(_tmp5F7.f3
!= 0){({void*_tmp5F8[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad type params, ignoring",
sizeof(unsigned char),26),_tag_arr(_tmp5F8,sizeof(void*),0));});}if(_tmp5F7.f4 != 
0){({void*_tmp5F9[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad specifiers, ignoring",
sizeof(unsigned char),25),_tag_arr(_tmp5F9,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA[0]=({
struct Cyc_ParamDecl_tok_struct _tmp5FB;_tmp5FB.tag=Cyc_ParamDecl_tok;_tmp5FB.f1=({
struct _tuple2*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->f1=0;_tmp5FC->f2=
_tmp5F7.f1;_tmp5FC->f3=_tmp5F7.f2;_tmp5FC;});_tmp5FB;});_tmp5FA;});break;}case
199: _LL361: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5FD=_cycalloc(sizeof(*
_tmp5FD));_tmp5FD[0]=({struct Cyc_Type_tok_struct _tmp5FE;_tmp5FE.tag=Cyc_Type_tok;
_tmp5FE.f1=(void*)(*Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset])).f3;_tmp5FE;});
_tmp5FD;});break;case 200: _LL362: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5FF=
_cycalloc(sizeof(*_tmp5FF));_tmp5FF[0]=({struct Cyc_Type_tok_struct _tmp600;
_tmp600.tag=Cyc_Type_tok;_tmp600.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp602;_tmp602.tag=18;_tmp602.f1=0;_tmp602;});_tmp601;}));_tmp600;});_tmp5FF;});
break;case 201: _LL363: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp603=_cycalloc(
sizeof(*_tmp603));_tmp603[0]=({struct Cyc_Type_tok_struct _tmp604;_tmp604.tag=Cyc_Type_tok;
_tmp604.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp605=_cycalloc(
sizeof(*_tmp605));_tmp605[0]=({struct Cyc_Absyn_JoinEff_struct _tmp606;_tmp606.tag=
18;_tmp606.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp606;});_tmp605;}));_tmp604;});_tmp603;});break;case 202:
_LL364: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp607=_cycalloc(sizeof(*
_tmp607));_tmp607[0]=({struct Cyc_Type_tok_struct _tmp608;_tmp608.tag=Cyc_Type_tok;
_tmp608.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp609=_cycalloc(
sizeof(*_tmp609));_tmp609[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp60A;_tmp60A.tag=
19;_tmp60A.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp60A;});_tmp609;}));_tmp608;});_tmp607;});break;case
203: _LL365: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp60B=_cycalloc(sizeof(*
_tmp60B));_tmp60B[0]=({struct Cyc_Type_tok_struct _tmp60C;_tmp60C.tag=Cyc_Type_tok;
_tmp60C.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp60D=_cycalloc(
sizeof(*_tmp60D));_tmp60D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp60E;_tmp60E.tag=
18;_tmp60E.f1=({struct Cyc_List_List*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp60F->tl=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp60F;});_tmp60E;});
_tmp60D;}));_tmp60C;});_tmp60B;});break;case 204: _LL366: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610[0]=({struct Cyc_TypeList_tok_struct
_tmp611;_tmp611.tag=Cyc_TypeList_tok;_tmp611.f1=({struct Cyc_List_List*_tmp612=
_cycalloc(sizeof(*_tmp612));_tmp612->hd=(void*)Cyc_yyget_Type_tok(yyvs[
yyvsp_offset]);_tmp612->tl=0;_tmp612;});_tmp611;});_tmp610;});break;case 205:
_LL367: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp613=_cycalloc(sizeof(*
_tmp613));_tmp613[0]=({struct Cyc_TypeList_tok_struct _tmp614;_tmp614.tag=Cyc_TypeList_tok;
_tmp614.f1=({struct Cyc_List_List*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->hd=(
void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);_tmp615->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp615;});_tmp614;});
_tmp613;});break;case 206: _LL368: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp617;_tmp617.tag=Cyc_AbstractDeclarator_tok;_tmp617.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]);_tmp618;});_tmp617;});_tmp616;});break;case 207: _LL369: yyval=
yyvs[yyvsp_offset];break;case 208: _LL36A: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp61A;_tmp61A.tag=Cyc_AbstractDeclarator_tok;_tmp61A.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[yyvsp_offset]))->tms);_tmp61B;});_tmp61A;});_tmp619;});break;case 209: _LL36B:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 210:
_LL36C: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp61C=_cycalloc(
sizeof(*_tmp61C));_tmp61C[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp61D;
_tmp61D.tag=Cyc_AbstractDeclarator_tok;_tmp61D.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->tms=({struct Cyc_List_List*_tmp61F=
_cycalloc(sizeof(*_tmp61F));_tmp61F->hd=(void*)((void*)0);_tmp61F->tl=0;_tmp61F;});
_tmp61E;});_tmp61D;});_tmp61C;});break;case 211: _LL36D: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp621;_tmp621.tag=Cyc_AbstractDeclarator_tok;_tmp621.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622->tms=({struct Cyc_List_List*_tmp623=
_cycalloc(sizeof(*_tmp623));_tmp623->hd=(void*)((void*)0);_tmp623->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->tms;_tmp623;});
_tmp622;});_tmp621;});_tmp620;});break;case 212: _LL36E: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp625;_tmp625.tag=Cyc_AbstractDeclarator_tok;_tmp625.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->tms=({struct Cyc_List_List*_tmp627=
_cycalloc(sizeof(*_tmp627));_tmp627->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp629;_tmp629.tag=0;_tmp629.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp629;});_tmp628;}));
_tmp627->tl=0;_tmp627;});_tmp626;});_tmp625;});_tmp624;});break;case 213: _LL36F:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp62A=_cycalloc(sizeof(*
_tmp62A));_tmp62A[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp62B;_tmp62B.tag=
Cyc_AbstractDeclarator_tok;_tmp62B.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->tms=({struct Cyc_List_List*_tmp62D=
_cycalloc(sizeof(*_tmp62D));_tmp62D->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp62F;_tmp62F.tag=0;_tmp62F.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp62F;});_tmp62E;}));
_tmp62D->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp62D;});_tmp62C;});
_tmp62B;});_tmp62A;});break;case 214: _LL370: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp631;_tmp631.tag=Cyc_AbstractDeclarator_tok;_tmp631.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->tms=({struct Cyc_List_List*_tmp633=
_cycalloc(sizeof(*_tmp633));_tmp633->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp635;_tmp635.tag=2;_tmp635.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp637;_tmp637.tag=1;_tmp637.f1=0;_tmp637.f2=0;_tmp637.f3=0;_tmp637.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp637.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp637;});_tmp636;}));
_tmp635;});_tmp634;}));_tmp633->tl=0;_tmp633;});_tmp632;});_tmp631;});_tmp630;});
break;case 215: _LL371: {struct _tuple13 _tmp639;struct Cyc_List_List*_tmp63A;struct
Cyc_Core_Opt*_tmp63B;struct Cyc_Absyn_VarargInfo*_tmp63C;int _tmp63D;struct Cyc_List_List*
_tmp63E;struct _tuple13*_tmp638=Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp639=*_tmp638;_LL378: _tmp63E=_tmp639.f1;goto _LL377;
_LL377: _tmp63D=_tmp639.f2;goto _LL376;_LL376: _tmp63C=_tmp639.f3;goto _LL375;_LL375:
_tmp63B=_tmp639.f4;goto _LL374;_LL374: _tmp63A=_tmp639.f5;goto _LL373;_LL373: yyval=(
void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp63F=_cycalloc(sizeof(*
_tmp63F));_tmp63F[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp640;_tmp640.tag=
Cyc_AbstractDeclarator_tok;_tmp640.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->tms=({struct Cyc_List_List*_tmp642=
_cycalloc(sizeof(*_tmp642));_tmp642->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp643=_cycalloc(sizeof(*_tmp643));_tmp643[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp644;_tmp644.tag=2;_tmp644.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp646;_tmp646.tag=1;_tmp646.f1=_tmp63E;_tmp646.f2=_tmp63D;_tmp646.f3=_tmp63C;
_tmp646.f4=_tmp63B;_tmp646.f5=_tmp63A;_tmp646;});_tmp645;}));_tmp644;});_tmp643;}));
_tmp642->tl=0;_tmp642;});_tmp641;});_tmp640;});_tmp63F;});break;}case 216: _LL372:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp647=_cycalloc(sizeof(*
_tmp647));_tmp647[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp648;_tmp648.tag=
Cyc_AbstractDeclarator_tok;_tmp648.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->tms=({struct Cyc_List_List*_tmp64A=
_cycalloc(sizeof(*_tmp64A));_tmp64A->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp64C;_tmp64C.tag=2;_tmp64C.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp64E;_tmp64E.tag=1;_tmp64E.f1=0;_tmp64E.f2=0;_tmp64E.f3=0;_tmp64E.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp64E.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp64E;});_tmp64D;}));
_tmp64C;});_tmp64B;}));_tmp64A->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp64A;});_tmp649;});
_tmp648;});_tmp647;});break;case 217: _LL379: {struct _tuple13 _tmp650;struct Cyc_List_List*
_tmp651;struct Cyc_Core_Opt*_tmp652;struct Cyc_Absyn_VarargInfo*_tmp653;int _tmp654;
struct Cyc_List_List*_tmp655;struct _tuple13*_tmp64F=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp650=*_tmp64F;_LL380:
_tmp655=_tmp650.f1;goto _LL37F;_LL37F: _tmp654=_tmp650.f2;goto _LL37E;_LL37E:
_tmp653=_tmp650.f3;goto _LL37D;_LL37D: _tmp652=_tmp650.f4;goto _LL37C;_LL37C:
_tmp651=_tmp650.f5;goto _LL37B;_LL37B: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp657;_tmp657.tag=Cyc_AbstractDeclarator_tok;_tmp657.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->tms=({struct Cyc_List_List*_tmp659=
_cycalloc(sizeof(*_tmp659));_tmp659->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp65B;_tmp65B.tag=2;_tmp65B.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp65D;_tmp65D.tag=1;_tmp65D.f1=_tmp655;_tmp65D.f2=_tmp654;_tmp65D.f3=_tmp653;
_tmp65D.f4=_tmp652;_tmp65D.f5=_tmp651;_tmp65D;});_tmp65C;}));_tmp65B;});_tmp65A;}));
_tmp659->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp659;});_tmp658;});
_tmp657;});_tmp656;});break;}case 218: _LL37A: {struct Cyc_List_List*_tmp65E=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp65F=_cycalloc(sizeof(*
_tmp65F));_tmp65F[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp660;_tmp660.tag=
Cyc_AbstractDeclarator_tok;_tmp660.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->tms=({struct Cyc_List_List*_tmp662=
_cycalloc(sizeof(*_tmp662));_tmp662->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp664;_tmp664.tag=3;_tmp664.f1=_tmp65E;_tmp664.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp664.f3=0;_tmp664;});_tmp663;}));_tmp662->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp662;});
_tmp661;});_tmp660;});_tmp65F;});break;}case 219: _LL381: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp666;_tmp666.tag=Cyc_AbstractDeclarator_tok;_tmp666.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->tms=({struct Cyc_List_List*_tmp668=
_cycalloc(sizeof(*_tmp668));_tmp668->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp66A;_tmp66A.tag=4;_tmp66A.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp66A.f2=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp66A;});_tmp669;}));_tmp668->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp668;});
_tmp667;});_tmp666;});_tmp665;});break;case 220: _LL382: yyval=yyvs[yyvsp_offset];
break;case 221: _LL383: yyval=yyvs[yyvsp_offset];break;case 222: _LL384: yyval=yyvs[
yyvsp_offset];break;case 223: _LL385: yyval=yyvs[yyvsp_offset];break;case 224: _LL386:
yyval=yyvs[yyvsp_offset];break;case 225: _LL387: yyval=yyvs[yyvsp_offset];break;
case 226: _LL388: if(Cyc_Std_zstrcmp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tag_arr("`H",sizeof(
unsigned char),3))== 0){Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(unsigned char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675->name=({
struct _tagged_arr*_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp678;});_tmp675->identity=
0;_tmp675->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp676=_cycalloc(
sizeof(*_tmp676));_tmp676[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp677;_tmp677.tag=0;
_tmp677.f1=(void*)((void*)3);_tmp677;});_tmp676;}));_tmp675;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673[0]=({
struct Cyc_Absyn_VarType_struct _tmp674;_tmp674.tag=1;_tmp674.f1=tv;_tmp674;});
_tmp673;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp66B=_cycalloc(sizeof(*
_tmp66B));_tmp66B[0]=({struct Cyc_Stmt_tok_struct _tmp66C;_tmp66C.tag=Cyc_Stmt_tok;
_tmp66C.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp66D=
_cycalloc(sizeof(*_tmp66D));_tmp66D[0]=({struct Cyc_Absyn_Region_s_struct _tmp66E;
_tmp66E.tag=18;_tmp66E.f1=tv;_tmp66E.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->f1=(void*)0;_tmp66F->f2=({struct
_tagged_arr*_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp670;});_tmp66F;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp671=_cycalloc(sizeof(*_tmp671));
_tmp671[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp672;_tmp672.tag=15;_tmp672.f1=(
void*)t;_tmp672;});_tmp671;}),0);_tmp66E.f3=Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);
_tmp66E;});_tmp66D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp66C;});_tmp66B;});break;}case 227: _LL389: if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tag_arr("H",sizeof(unsigned char),2))== 0){Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(unsigned char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683->name=({
struct _tagged_arr*_tmp686=_cycalloc(sizeof(*_tmp686));_tmp686[0]=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp688;_tmp688.tag=0;_tmp688.f1=(
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);{void*_tmp687[1]={& _tmp688};Cyc_Std_aprintf(_tag_arr("`%s",
sizeof(unsigned char),4),_tag_arr(_tmp687,sizeof(void*),1));}});_tmp686;});
_tmp683->identity=0;_tmp683->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp685;_tmp685.tag=0;_tmp685.f1=(void*)((void*)3);_tmp685;});_tmp684;}));
_tmp683;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp681=_cycalloc(
sizeof(*_tmp681));_tmp681[0]=({struct Cyc_Absyn_VarType_struct _tmp682;_tmp682.tag=
1;_tmp682.f1=tv;_tmp682;});_tmp681;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679[0]=({struct Cyc_Stmt_tok_struct
_tmp67A;_tmp67A.tag=Cyc_Stmt_tok;_tmp67A.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B[0]=({
struct Cyc_Absyn_Region_s_struct _tmp67C;_tmp67C.tag=18;_tmp67C.f1=tv;_tmp67C.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp67D=_cycalloc(sizeof(*_tmp67D));
_tmp67D->f1=(void*)0;_tmp67D->f2=({struct _tagged_arr*_tmp67E=_cycalloc(sizeof(*
_tmp67E));_tmp67E[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp67E;});_tmp67D;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp680;_tmp680.tag=15;_tmp680.f1=(void*)t;_tmp680;});_tmp67F;}),0);_tmp67C.f3=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp67C;});_tmp67B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp67A;});_tmp679;});break;}case 228: _LL38A: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp689=_cycalloc(sizeof(*_tmp689));_tmp689[0]=({
struct Cyc_Stmt_tok_struct _tmp68A;_tmp68A.tag=Cyc_Stmt_tok;_tmp68A.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Cut_s_struct*_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B[
0]=({struct Cyc_Absyn_Cut_s_struct _tmp68C;_tmp68C.tag=13;_tmp68C.f1=Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]);_tmp68C;});_tmp68B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp68A;});_tmp689;});break;case 229: _LL38B: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D[0]=({
struct Cyc_Stmt_tok_struct _tmp68E;_tmp68E.tag=Cyc_Stmt_tok;_tmp68E.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmp68F=_cycalloc(sizeof(*_tmp68F));
_tmp68F[0]=({struct Cyc_Absyn_Splice_s_struct _tmp690;_tmp690.tag=14;_tmp690.f1=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp690;});_tmp68F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp68E;});_tmp68D;});break;case 230: _LL38C: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691[0]=({
struct Cyc_Stmt_tok_struct _tmp692;_tmp692.tag=Cyc_Stmt_tok;_tmp692.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp693=_cycalloc(sizeof(*_tmp693));
_tmp693[0]=({struct Cyc_Absyn_Label_s_struct _tmp694;_tmp694.tag=15;_tmp694.f1=({
struct _tagged_arr*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp695;});_tmp694.f2=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp694;});_tmp693;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp692;});_tmp691;});break;case 231: _LL38D: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696[0]=({
struct Cyc_Stmt_tok_struct _tmp697;_tmp697.tag=Cyc_Stmt_tok;_tmp697.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmp697;});_tmp696;});break;case 232: _LL38E: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698[0]=({struct Cyc_Stmt_tok_struct
_tmp699;_tmp699.tag=Cyc_Stmt_tok;_tmp699.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp699;});_tmp698;});break;case 233: _LL38F: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A[0]=({
struct Cyc_Stmt_tok_struct _tmp69B;_tmp69B.tag=Cyc_Stmt_tok;_tmp69B.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp69B;});
_tmp69A;});break;case 234: _LL390: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 235: _LL391: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C[0]=({struct Cyc_Stmt_tok_struct
_tmp69D;_tmp69D.tag=Cyc_Stmt_tok;_tmp69D.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(0));_tmp69D;});_tmp69C;});break;case 236:
_LL392: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp69E=_cycalloc(sizeof(*
_tmp69E));_tmp69E[0]=({struct Cyc_Stmt_tok_struct _tmp69F;_tmp69F.tag=Cyc_Stmt_tok;
_tmp69F.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]));_tmp69F;});_tmp69E;});break;case 237: _LL393: yyval=yyvs[
yyvsp_offset];break;case 238: _LL394: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0[0]=({struct Cyc_Stmt_tok_struct
_tmp6A1;_tmp6A1.tag=Cyc_Stmt_tok;_tmp6A1.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6A1;});_tmp6A0;});break;case 239: _LL395: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));_tmp6A2[0]=({
struct Cyc_Stmt_tok_struct _tmp6A3;_tmp6A3.tag=Cyc_Stmt_tok;_tmp6A3.f1=Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*_tmp6A4=_cycalloc(
sizeof(*_tmp6A4));_tmp6A4[0]=({struct Cyc_Absyn_Fn_d_struct _tmp6A5;_tmp6A5.tag=1;
_tmp6A5.f1=Cyc_yyget_FnDecl_tok(yyvs[yyvsp_offset]);_tmp6A5;});_tmp6A4;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line)),Cyc_Absyn_skip_stmt(
0));_tmp6A3;});_tmp6A2;});break;case 240: _LL396: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6[0]=({struct Cyc_Stmt_tok_struct
_tmp6A7;_tmp6A7.tag=Cyc_Stmt_tok;_tmp6A7.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp6A9;_tmp6A9.tag=1;_tmp6A9.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6A9;});_tmp6A8;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]));_tmp6A7;});
_tmp6A6;});break;case 241: _LL397: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6AA=
_cycalloc(sizeof(*_tmp6AA));_tmp6AA[0]=({struct Cyc_Stmt_tok_struct _tmp6AB;
_tmp6AB.tag=Cyc_Stmt_tok;_tmp6AB.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6AB;});_tmp6AA;});break;case 242: _LL398: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));_tmp6AC[0]=({
struct Cyc_Stmt_tok_struct _tmp6AD;_tmp6AD.tag=Cyc_Stmt_tok;_tmp6AD.f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6AD;});_tmp6AC;});break;case 243: _LL399: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));_tmp6AE[0]=({
struct Cyc_Stmt_tok_struct _tmp6AF;_tmp6AF.tag=Cyc_Stmt_tok;_tmp6AF.f1=Cyc_Absyn_switch_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6AF;});_tmp6AE;});break;case 244: _LL39A: if(Cyc_Std_strcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tag_arr("C",sizeof(unsigned char),2))!= 0){Cyc_Parse_err(_tag_arr("only switch \"C\" { ... } is allowed",
sizeof(unsigned char),35),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6B0=
_cycalloc(sizeof(*_tmp6B0));_tmp6B0[0]=({struct Cyc_Stmt_tok_struct _tmp6B1;
_tmp6B1.tag=Cyc_Stmt_tok;_tmp6B1.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_SwitchC_s_struct*
_tmp6B2=_cycalloc(sizeof(*_tmp6B2));_tmp6B2[0]=({struct Cyc_Absyn_SwitchC_s_struct
_tmp6B3;_tmp6B3.tag=10;_tmp6B3.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp6B3.f2=Cyc_yyget_SwitchCClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6B3;});_tmp6B2;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6B1;});
_tmp6B0;});break;case 245: _LL39B: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6B4=
_cycalloc(sizeof(*_tmp6B4));_tmp6B4[0]=({struct Cyc_Stmt_tok_struct _tmp6B5;
_tmp6B5.tag=Cyc_Stmt_tok;_tmp6B5.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6B5;});_tmp6B4;});break;case 246: _LL39C: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp6B6=_cycalloc(sizeof(*_tmp6B6));
_tmp6B6[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp6B7;_tmp6B7.tag=Cyc_SwitchClauseList_tok;
_tmp6B7.f1=0;_tmp6B7;});_tmp6B6;});break;case 247: _LL39D: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6B9;_tmp6B9.tag=Cyc_SwitchClauseList_tok;_tmp6B9.f1=({struct Cyc_List_List*
_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp6BB->pat_vars=0;_tmp6BB->where_clause=0;
_tmp6BB->body=Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp6BB->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6BB;});_tmp6BA->tl=0;_tmp6BA;});_tmp6B9;});_tmp6B8;});
break;case 248: _LL39E: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6BD;_tmp6BD.tag=Cyc_SwitchClauseList_tok;_tmp6BD.f1=({struct Cyc_List_List*
_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6BF->pat_vars=0;
_tmp6BF->where_clause=0;_tmp6BF->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp6BF->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line);_tmp6BF;});_tmp6BE->tl=
Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmp6BE;});_tmp6BD;});_tmp6BC;});
break;case 249: _LL39F: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6C1;_tmp6C1.tag=Cyc_SwitchClauseList_tok;_tmp6C1.f1=({struct Cyc_List_List*
_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6C3->pat_vars=0;
_tmp6C3->where_clause=0;_tmp6C3->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6C3->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp6C3;});
_tmp6C2->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmp6C2;});
_tmp6C1;});_tmp6C0;});break;case 250: _LL3A0: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6C4=_cycalloc(sizeof(*_tmp6C4));_tmp6C4[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6C5;_tmp6C5.tag=Cyc_SwitchClauseList_tok;_tmp6C5.f1=({struct Cyc_List_List*
_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp6C7->pat_vars=0;
_tmp6C7->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6C7->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp6C7->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6C7;});_tmp6C6->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmp6C6;});_tmp6C5;});_tmp6C4;});break;case 251: _LL3A1: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));
_tmp6C8[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp6C9;_tmp6C9.tag=Cyc_SwitchClauseList_tok;
_tmp6C9.f1=({struct Cyc_List_List*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA->hd=({
struct Cyc_Absyn_Switch_clause*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp6CB->pat_vars=0;_tmp6CB->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6CB->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6CB->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6CB;});_tmp6CA->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmp6CA;});_tmp6C9;});_tmp6C8;});break;case 252: _LL3A2: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));
_tmp6CC[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp6CD;_tmp6CD.tag=Cyc_SwitchCClauseList_tok;
_tmp6CD.f1=0;_tmp6CD;});_tmp6CC;});break;case 253: _LL3A3: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmp6CE=_cycalloc(sizeof(*_tmp6CE));_tmp6CE[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmp6CF;_tmp6CF.tag=Cyc_SwitchCClauseList_tok;_tmp6CF.f1=({struct Cyc_List_List*
_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0->hd=({struct Cyc_Absyn_SwitchC_clause*
_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1->cnst_exp=0;_tmp6D1->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Absyn_break_stmt(0),0);_tmp6D1->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line);_tmp6D1;});_tmp6D0->tl=
0;_tmp6D0;});_tmp6CF;});_tmp6CE;});break;case 254: _LL3A4: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmp6D3;_tmp6D3.tag=Cyc_SwitchCClauseList_tok;_tmp6D3.f1=({struct Cyc_List_List*
_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4->hd=({struct Cyc_Absyn_SwitchC_clause*
_tmp6D5=_cycalloc(sizeof(*_tmp6D5));_tmp6D5->cnst_exp=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6D5->body=Cyc_Absyn_fallthru_stmt(
0,0);_tmp6D5->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6D5;});_tmp6D4->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmp6D4;});_tmp6D3;});_tmp6D2;});break;case 255: _LL3A5: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));
_tmp6D6[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp6D7;_tmp6D7.tag=Cyc_SwitchCClauseList_tok;
_tmp6D7.f1=({struct Cyc_List_List*_tmp6D8=_cycalloc(sizeof(*_tmp6D8));_tmp6D8->hd=({
struct Cyc_Absyn_SwitchC_clause*_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9->cnst_exp=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp6D9->body=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Absyn_fallthru_stmt(
0,0),0);_tmp6D9->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6D9;});_tmp6D8->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmp6D8;});_tmp6D7;});_tmp6D6;});break;case 256: _LL3A6: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));_tmp6DA[0]=({
struct Cyc_Stmt_tok_struct _tmp6DB;_tmp6DB.tag=Cyc_Stmt_tok;_tmp6DB.f1=Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6DB;});_tmp6DA;});break;case 257: _LL3A7: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6DC=_cycalloc(sizeof(*_tmp6DC));_tmp6DC[0]=({
struct Cyc_Stmt_tok_struct _tmp6DD;_tmp6DD.tag=Cyc_Stmt_tok;_tmp6DD.f1=Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6DD;});
_tmp6DC;});break;case 258: _LL3A8: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6DE=
_cycalloc(sizeof(*_tmp6DE));_tmp6DE[0]=({struct Cyc_Stmt_tok_struct _tmp6DF;
_tmp6DF.tag=Cyc_Stmt_tok;_tmp6DF.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6DF;});
_tmp6DE;});break;case 259: _LL3A9: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6E0=
_cycalloc(sizeof(*_tmp6E0));_tmp6E0[0]=({struct Cyc_Stmt_tok_struct _tmp6E1;
_tmp6E1.tag=Cyc_Stmt_tok;_tmp6E1.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6E1;});_tmp6E0;});break;case 260: _LL3AA: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2[0]=({
struct Cyc_Stmt_tok_struct _tmp6E3;_tmp6E3.tag=Cyc_Stmt_tok;_tmp6E3.f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6E3;});
_tmp6E2;});break;case 261: _LL3AB: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6E4=
_cycalloc(sizeof(*_tmp6E4));_tmp6E4[0]=({struct Cyc_Stmt_tok_struct _tmp6E5;
_tmp6E5.tag=Cyc_Stmt_tok;_tmp6E5.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp6E5;});
_tmp6E4;});break;case 262: _LL3AC: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6E6=
_cycalloc(sizeof(*_tmp6E6));_tmp6E6[0]=({struct Cyc_Stmt_tok_struct _tmp6E7;
_tmp6E7.tag=Cyc_Stmt_tok;_tmp6E7.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6E7;});_tmp6E6;});break;case 263: _LL3AD: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8[0]=({
struct Cyc_Stmt_tok_struct _tmp6E9;_tmp6E9.tag=Cyc_Stmt_tok;_tmp6E9.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6E9;});_tmp6E8;});break;case 264: _LL3AE: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6EA=_cycalloc(sizeof(*_tmp6EA));_tmp6EA[0]=({
struct Cyc_Stmt_tok_struct _tmp6EB;_tmp6EB.tag=Cyc_Stmt_tok;_tmp6EB.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6EB;});_tmp6EA;});break;case 265: _LL3AF: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6EC=_cycalloc(sizeof(*_tmp6EC));_tmp6EC[0]=({
struct Cyc_Stmt_tok_struct _tmp6ED;_tmp6ED.tag=Cyc_Stmt_tok;_tmp6ED.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6ED;});_tmp6EC;});break;case 266: _LL3B0: {struct
Cyc_List_List*_tmp6EE=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*_tmp6EF=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp6F0=_cycalloc(sizeof(*_tmp6F0));_tmp6F0[0]=({
struct Cyc_Stmt_tok_struct _tmp6F1;_tmp6F1.tag=Cyc_Stmt_tok;_tmp6F1.f1=Cyc_Parse_flatten_declarations(
_tmp6EE,_tmp6EF);_tmp6F1;});_tmp6F0;});break;}case 267: _LL3B1: {struct Cyc_List_List*
_tmp6F2=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp6F3=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6F4=
_cycalloc(sizeof(*_tmp6F4));_tmp6F4[0]=({struct Cyc_Stmt_tok_struct _tmp6F5;
_tmp6F5.tag=Cyc_Stmt_tok;_tmp6F5.f1=Cyc_Parse_flatten_declarations(_tmp6F2,
_tmp6F3);_tmp6F5;});_tmp6F4;});break;}case 268: _LL3B2: {struct Cyc_List_List*
_tmp6F6=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp6F7=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6F8=
_cycalloc(sizeof(*_tmp6F8));_tmp6F8[0]=({struct Cyc_Stmt_tok_struct _tmp6F9;
_tmp6F9.tag=Cyc_Stmt_tok;_tmp6F9.f1=Cyc_Parse_flatten_declarations(_tmp6F6,
_tmp6F7);_tmp6F9;});_tmp6F8;});break;}case 269: _LL3B3: {struct Cyc_List_List*
_tmp6FA=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp6FB=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6FC=
_cycalloc(sizeof(*_tmp6FC));_tmp6FC[0]=({struct Cyc_Stmt_tok_struct _tmp6FD;
_tmp6FD.tag=Cyc_Stmt_tok;_tmp6FD.f1=Cyc_Parse_flatten_declarations(_tmp6FA,
_tmp6FB);_tmp6FD;});_tmp6FC;});break;}case 270: _LL3B4: {struct Cyc_List_List*
_tmp6FE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_decl2vardecl,Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF[0]=({
struct Cyc_Stmt_tok_struct _tmp700;_tmp700.tag=Cyc_Stmt_tok;_tmp700.f1=Cyc_Absyn_forarray_stmt(
_tmp6FE,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp700;});_tmp6FF;});break;}case 271: _LL3B5: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701[0]=({
struct Cyc_Stmt_tok_struct _tmp702;_tmp702.tag=Cyc_Stmt_tok;_tmp702.f1=Cyc_Absyn_goto_stmt(({
struct _tagged_arr*_tmp703=_cycalloc(sizeof(*_tmp703));_tmp703[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp703;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp702;});
_tmp701;});break;case 272: _LL3B6: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp704=
_cycalloc(sizeof(*_tmp704));_tmp704[0]=({struct Cyc_Stmt_tok_struct _tmp705;
_tmp705.tag=Cyc_Stmt_tok;_tmp705.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp705;});
_tmp704;});break;case 273: _LL3B7: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp706=
_cycalloc(sizeof(*_tmp706));_tmp706[0]=({struct Cyc_Stmt_tok_struct _tmp707;
_tmp707.tag=Cyc_Stmt_tok;_tmp707.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp707;});
_tmp706;});break;case 274: _LL3B8: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp708=
_cycalloc(sizeof(*_tmp708));_tmp708[0]=({struct Cyc_Stmt_tok_struct _tmp709;
_tmp709.tag=Cyc_Stmt_tok;_tmp709.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp709;});
_tmp708;});break;case 275: _LL3B9: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp70A=
_cycalloc(sizeof(*_tmp70A));_tmp70A[0]=({struct Cyc_Stmt_tok_struct _tmp70B;
_tmp70B.tag=Cyc_Stmt_tok;_tmp70B.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp70B;});_tmp70A;});break;case 276: _LL3BA: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp70C=_cycalloc(sizeof(*_tmp70C));_tmp70C[0]=({
struct Cyc_Stmt_tok_struct _tmp70D;_tmp70D.tag=Cyc_Stmt_tok;_tmp70D.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp70D;});_tmp70C;});break;case 277: _LL3BB: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp70E=_cycalloc(sizeof(*_tmp70E));_tmp70E[0]=({
struct Cyc_Stmt_tok_struct _tmp70F;_tmp70F.tag=Cyc_Stmt_tok;_tmp70F.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp70F;});_tmp70E;});break;case 278: _LL3BC: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710[0]=({
struct Cyc_Stmt_tok_struct _tmp711;_tmp711.tag=Cyc_Stmt_tok;_tmp711.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp711;});_tmp710;});break;case 279: _LL3BD: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp712=_cycalloc(sizeof(*_tmp712));_tmp712[
0]=({struct Cyc_Pattern_tok_struct _tmp713;_tmp713.tag=Cyc_Pattern_tok;_tmp713.f1=
Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp713;});_tmp712;});break;case 280: _LL3BE: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 281: _LL3BF:
yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp714=_cycalloc(sizeof(*_tmp714));
_tmp714[0]=({struct Cyc_Pattern_tok_struct _tmp715;_tmp715.tag=Cyc_Pattern_tok;
_tmp715.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp716=
_cycalloc(sizeof(*_tmp716));_tmp716[0]=({struct Cyc_Absyn_Int_p_struct _tmp717;
_tmp717.tag=1;_tmp717.f1=(void*)(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f1;
_tmp717.f2=(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2;_tmp717;});_tmp716;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp715;});
_tmp714;});break;case 282: _LL3C0: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp718=_cycalloc(sizeof(*_tmp718));_tmp718[0]=({struct Cyc_Pattern_tok_struct
_tmp719;_tmp719.tag=Cyc_Pattern_tok;_tmp719.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Int_p_struct*_tmp71A=_cycalloc(sizeof(*_tmp71A));_tmp71A[0]=({struct
Cyc_Absyn_Int_p_struct _tmp71B;_tmp71B.tag=1;_tmp71B.f1=(void*)((void*)0);_tmp71B.f2=
-(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2;_tmp71B;});_tmp71A;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp719;});_tmp718;});break;case 283: _LL3C1: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C[
0]=({struct Cyc_Pattern_tok_struct _tmp71D;_tmp71D.tag=Cyc_Pattern_tok;_tmp71D.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*_tmp71E=_cycalloc(
sizeof(*_tmp71E));_tmp71E[0]=({struct Cyc_Absyn_Float_p_struct _tmp71F;_tmp71F.tag=
3;_tmp71F.f1=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp71F;});_tmp71E;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp71D;});
_tmp71C;});break;case 284: _LL3C2: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp720=_cycalloc(sizeof(*_tmp720));_tmp720[0]=({struct Cyc_Pattern_tok_struct
_tmp721;_tmp721.tag=Cyc_Pattern_tok;_tmp721.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Char_p_struct*_tmp722=_cycalloc_atomic(sizeof(*_tmp722));_tmp722[0]=({
struct Cyc_Absyn_Char_p_struct _tmp723;_tmp723.tag=2;_tmp723.f1=Cyc_yyget_Char_tok(
yyvs[yyvsp_offset]);_tmp723;});_tmp722;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp721;});_tmp720;});
break;case 285: _LL3C3: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp724=
_cycalloc(sizeof(*_tmp724));_tmp724[0]=({struct Cyc_Pattern_tok_struct _tmp725;
_tmp725.tag=Cyc_Pattern_tok;_tmp725.f1=Cyc_Absyn_new_pat((void*)1,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp725;});
_tmp724;});break;case 286: _LL3C4: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp726=_cycalloc(sizeof(*_tmp726));_tmp726[0]=({struct Cyc_Pattern_tok_struct
_tmp727;_tmp727.tag=Cyc_Pattern_tok;_tmp727.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownId_p_struct*_tmp728=_cycalloc(sizeof(*_tmp728));_tmp728[0]=({
struct Cyc_Absyn_UnknownId_p_struct _tmp729;_tmp729.tag=11;_tmp729.f1=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp729;});_tmp728;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp727;});_tmp726;});
break;case 287: _LL3C5: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp72A=
_cycalloc(sizeof(*_tmp72A));_tmp72A[0]=({struct Cyc_Pattern_tok_struct _tmp72B;
_tmp72B.tag=Cyc_Pattern_tok;_tmp72B.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_struct*
_tmp72C=_cycalloc(sizeof(*_tmp72C));_tmp72C[0]=({struct Cyc_Absyn_Tuple_p_struct
_tmp72D;_tmp72D.tag=4;_tmp72D.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]));_tmp72D;});_tmp72C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp72B;});_tmp72A;});break;case 288: _LL3C6: {struct
Cyc_List_List*_tmp72E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp72F=_cycalloc(sizeof(*_tmp72F));_tmp72F[0]=({
struct Cyc_Pattern_tok_struct _tmp730;_tmp730.tag=Cyc_Pattern_tok;_tmp730.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp731=_cycalloc(sizeof(*_tmp731));
_tmp731[0]=({struct Cyc_Absyn_UnknownCall_p_struct _tmp732;_tmp732.tag=12;_tmp732.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp732.f2=_tmp72E;_tmp732.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]));_tmp732;});_tmp731;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp730;});_tmp72F;});break;}case 289: _LL3C7: {struct
Cyc_List_List*_tmp733=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp734=_cycalloc(sizeof(*_tmp734));_tmp734[0]=({
struct Cyc_Pattern_tok_struct _tmp735;_tmp735.tag=Cyc_Pattern_tok;_tmp735.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp736=_cycalloc(sizeof(*_tmp736));
_tmp736[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp737;_tmp737.tag=7;_tmp737.f1=({
struct Cyc_Absyn_AggrInfo _tmp738;_tmp738.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp739=_cycalloc(sizeof(*_tmp739));_tmp739[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp73A;_tmp73A.tag=0;_tmp73A.f1=(void*)((void*)0);_tmp73A.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp73A;});_tmp739;}));
_tmp738.targs=0;_tmp738;});_tmp737.f2=_tmp733;_tmp737.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp737;});_tmp736;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp735;});
_tmp734;});break;}case 290: _LL3C8: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp73B=_cycalloc(sizeof(*_tmp73B));_tmp73B[0]=({struct Cyc_Pattern_tok_struct
_tmp73C;_tmp73C.tag=Cyc_Pattern_tok;_tmp73C.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Pointer_p_struct*_tmp73D=_cycalloc(sizeof(*_tmp73D));_tmp73D[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp73E;_tmp73E.tag=5;_tmp73E.f1=Cyc_yyget_Pattern_tok(
yyvs[yyvsp_offset]);_tmp73E;});_tmp73D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp73C;});_tmp73B;});break;case 291: _LL3C9: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp73F=_cycalloc(sizeof(*_tmp73F));_tmp73F[
0]=({struct Cyc_Pattern_tok_struct _tmp740;_tmp740.tag=Cyc_Pattern_tok;_tmp740.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp741=_cycalloc(
sizeof(*_tmp741));_tmp741[0]=({struct Cyc_Absyn_Reference_p_struct _tmp742;_tmp742.tag=
6;_tmp742.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp743=_cycalloc(sizeof(*
_tmp743));_tmp743->f1=(void*)0;_tmp743->f2=({struct _tagged_arr*_tmp744=_cycalloc(
sizeof(*_tmp744));_tmp744[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp744;});
_tmp743;}),(void*)0,0);_tmp742;});_tmp741;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp740;});_tmp73F;});break;case 292: _LL3CA: yyval=(
void*)({struct Cyc_PatternList_tok_struct*_tmp745=_cycalloc(sizeof(*_tmp745));
_tmp745[0]=({struct Cyc_PatternList_tok_struct _tmp746;_tmp746.tag=Cyc_PatternList_tok;
_tmp746.f1=({struct Cyc_List_List*_tmp747=_cycalloc(sizeof(*_tmp747));_tmp747->hd=
Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmp747->tl=0;_tmp747;});_tmp746;});
_tmp745;});break;case 293: _LL3CB: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp748=_cycalloc(sizeof(*_tmp748));_tmp748[0]=({struct Cyc_PatternList_tok_struct
_tmp749;_tmp749.tag=Cyc_PatternList_tok;_tmp749.f1=({struct Cyc_List_List*_tmp74A=
_cycalloc(sizeof(*_tmp74A));_tmp74A->hd=Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);
_tmp74A->tl=Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp74A;});_tmp749;});_tmp748;});break;case 294: _LL3CC: yyval=(
void*)({struct Cyc_FieldPattern_tok_struct*_tmp74B=_cycalloc(sizeof(*_tmp74B));
_tmp74B[0]=({struct Cyc_FieldPattern_tok_struct _tmp74C;_tmp74C.tag=Cyc_FieldPattern_tok;
_tmp74C.f1=({struct _tuple10*_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D->f1=0;
_tmp74D->f2=Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmp74D;});_tmp74C;});
_tmp74B;});break;case 295: _LL3CD: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp74E=_cycalloc(sizeof(*_tmp74E));_tmp74E[0]=({struct Cyc_FieldPattern_tok_struct
_tmp74F;_tmp74F.tag=Cyc_FieldPattern_tok;_tmp74F.f1=({struct _tuple10*_tmp750=
_cycalloc(sizeof(*_tmp750));_tmp750->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp750->f2=Cyc_yyget_Pattern_tok(
yyvs[yyvsp_offset]);_tmp750;});_tmp74F;});_tmp74E;});break;case 296: _LL3CE: yyval=(
void*)({struct Cyc_FieldPatternList_tok_struct*_tmp751=_cycalloc(sizeof(*_tmp751));
_tmp751[0]=({struct Cyc_FieldPatternList_tok_struct _tmp752;_tmp752.tag=Cyc_FieldPatternList_tok;
_tmp752.f1=({struct Cyc_List_List*_tmp753=_cycalloc(sizeof(*_tmp753));_tmp753->hd=
Cyc_yyget_FieldPattern_tok(yyvs[yyvsp_offset]);_tmp753->tl=0;_tmp753;});_tmp752;});
_tmp751;});break;case 297: _LL3CF: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp754=_cycalloc(sizeof(*_tmp754));_tmp754[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp755;_tmp755.tag=Cyc_FieldPatternList_tok;_tmp755.f1=({struct Cyc_List_List*
_tmp756=_cycalloc(sizeof(*_tmp756));_tmp756->hd=Cyc_yyget_FieldPattern_tok(yyvs[
yyvsp_offset]);_tmp756->tl=Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp756;});_tmp755;});
_tmp754;});break;case 298: _LL3D0: yyval=yyvs[yyvsp_offset];break;case 299: _LL3D1:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp757=_cycalloc(sizeof(*_tmp757));
_tmp757[0]=({struct Cyc_Exp_tok_struct _tmp758;_tmp758.tag=Cyc_Exp_tok;_tmp758.f1=
Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp758;});_tmp757;});break;case 300: _LL3D2: yyval=yyvs[
yyvsp_offset];break;case 301: _LL3D3: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp759=_cycalloc(sizeof(*_tmp759));_tmp759[0]=({struct Cyc_Exp_tok_struct _tmp75A;
_tmp75A.tag=Cyc_Exp_tok;_tmp75A.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp75A;});_tmp759;});break;case 302: _LL3D4: yyval=(
void*)({struct Cyc_Primopopt_tok_struct*_tmp75B=_cycalloc(sizeof(*_tmp75B));
_tmp75B[0]=({struct Cyc_Primopopt_tok_struct _tmp75C;_tmp75C.tag=Cyc_Primopopt_tok;
_tmp75C.f1=0;_tmp75C;});_tmp75B;});break;case 303: _LL3D5: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D[0]=({struct Cyc_Primopopt_tok_struct
_tmp75E;_tmp75E.tag=Cyc_Primopopt_tok;_tmp75E.f1=({struct Cyc_Core_Opt*_tmp75F=
_cycalloc(sizeof(*_tmp75F));_tmp75F->v=(void*)((void*)1);_tmp75F;});_tmp75E;});
_tmp75D;});break;case 304: _LL3D6: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp760=_cycalloc(sizeof(*_tmp760));_tmp760[0]=({struct Cyc_Primopopt_tok_struct
_tmp761;_tmp761.tag=Cyc_Primopopt_tok;_tmp761.f1=({struct Cyc_Core_Opt*_tmp762=
_cycalloc(sizeof(*_tmp762));_tmp762->v=(void*)((void*)3);_tmp762;});_tmp761;});
_tmp760;});break;case 305: _LL3D7: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp763=_cycalloc(sizeof(*_tmp763));_tmp763[0]=({struct Cyc_Primopopt_tok_struct
_tmp764;_tmp764.tag=Cyc_Primopopt_tok;_tmp764.f1=({struct Cyc_Core_Opt*_tmp765=
_cycalloc(sizeof(*_tmp765));_tmp765->v=(void*)((void*)4);_tmp765;});_tmp764;});
_tmp763;});break;case 306: _LL3D8: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp766=_cycalloc(sizeof(*_tmp766));_tmp766[0]=({struct Cyc_Primopopt_tok_struct
_tmp767;_tmp767.tag=Cyc_Primopopt_tok;_tmp767.f1=({struct Cyc_Core_Opt*_tmp768=
_cycalloc(sizeof(*_tmp768));_tmp768->v=(void*)((void*)0);_tmp768;});_tmp767;});
_tmp766;});break;case 307: _LL3D9: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp769=_cycalloc(sizeof(*_tmp769));_tmp769[0]=({struct Cyc_Primopopt_tok_struct
_tmp76A;_tmp76A.tag=Cyc_Primopopt_tok;_tmp76A.f1=({struct Cyc_Core_Opt*_tmp76B=
_cycalloc(sizeof(*_tmp76B));_tmp76B->v=(void*)((void*)2);_tmp76B;});_tmp76A;});
_tmp769;});break;case 308: _LL3DA: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp76C=_cycalloc(sizeof(*_tmp76C));_tmp76C[0]=({struct Cyc_Primopopt_tok_struct
_tmp76D;_tmp76D.tag=Cyc_Primopopt_tok;_tmp76D.f1=({struct Cyc_Core_Opt*_tmp76E=
_cycalloc(sizeof(*_tmp76E));_tmp76E->v=(void*)((void*)16);_tmp76E;});_tmp76D;});
_tmp76C;});break;case 309: _LL3DB: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp76F=_cycalloc(sizeof(*_tmp76F));_tmp76F[0]=({struct Cyc_Primopopt_tok_struct
_tmp770;_tmp770.tag=Cyc_Primopopt_tok;_tmp770.f1=({struct Cyc_Core_Opt*_tmp771=
_cycalloc(sizeof(*_tmp771));_tmp771->v=(void*)((void*)17);_tmp771;});_tmp770;});
_tmp76F;});break;case 310: _LL3DC: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp772=_cycalloc(sizeof(*_tmp772));_tmp772[0]=({struct Cyc_Primopopt_tok_struct
_tmp773;_tmp773.tag=Cyc_Primopopt_tok;_tmp773.f1=({struct Cyc_Core_Opt*_tmp774=
_cycalloc(sizeof(*_tmp774));_tmp774->v=(void*)((void*)13);_tmp774;});_tmp773;});
_tmp772;});break;case 311: _LL3DD: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp775=_cycalloc(sizeof(*_tmp775));_tmp775[0]=({struct Cyc_Primopopt_tok_struct
_tmp776;_tmp776.tag=Cyc_Primopopt_tok;_tmp776.f1=({struct Cyc_Core_Opt*_tmp777=
_cycalloc(sizeof(*_tmp777));_tmp777->v=(void*)((void*)15);_tmp777;});_tmp776;});
_tmp775;});break;case 312: _LL3DE: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp778=_cycalloc(sizeof(*_tmp778));_tmp778[0]=({struct Cyc_Primopopt_tok_struct
_tmp779;_tmp779.tag=Cyc_Primopopt_tok;_tmp779.f1=({struct Cyc_Core_Opt*_tmp77A=
_cycalloc(sizeof(*_tmp77A));_tmp77A->v=(void*)((void*)14);_tmp77A;});_tmp779;});
_tmp778;});break;case 313: _LL3DF: yyval=yyvs[yyvsp_offset];break;case 314: _LL3E0:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp77B=_cycalloc(sizeof(*_tmp77B));
_tmp77B[0]=({struct Cyc_Exp_tok_struct _tmp77C;_tmp77C.tag=Cyc_Exp_tok;_tmp77C.f1=
Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp77C;});_tmp77B;});break;case 315: _LL3E1: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp77D=_cycalloc(sizeof(*_tmp77D));_tmp77D[0]=({
struct Cyc_Exp_tok_struct _tmp77E;_tmp77E.tag=Cyc_Exp_tok;_tmp77E.f1=Cyc_Absyn_throw_exp(
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp77E;});_tmp77D;});break;case 316: _LL3E2: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp77F=_cycalloc(sizeof(*_tmp77F));_tmp77F[0]=({
struct Cyc_Exp_tok_struct _tmp780;_tmp780.tag=Cyc_Exp_tok;_tmp780.f1=Cyc_Absyn_New_exp(
0,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line));_tmp780;});
_tmp77F;});break;case 317: _LL3E3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp781=
_cycalloc(sizeof(*_tmp781));_tmp781[0]=({struct Cyc_Exp_tok_struct _tmp782;_tmp782.tag=
Cyc_Exp_tok;_tmp782.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).last_line));_tmp782;});_tmp781;});break;case 318: _LL3E4: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp783=_cycalloc(sizeof(*_tmp783));_tmp783[0]=({
struct Cyc_Exp_tok_struct _tmp784;_tmp784.tag=Cyc_Exp_tok;_tmp784.f1=Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp784;});_tmp783;});break;case 319: _LL3E5: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp785=_cycalloc(sizeof(*_tmp785));_tmp785[0]=({
struct Cyc_Exp_tok_struct _tmp786;_tmp786.tag=Cyc_Exp_tok;_tmp786.f1=Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp786;});_tmp785;});break;case 320: _LL3E6: yyval=yyvs[
yyvsp_offset];break;case 321: _LL3E7: yyval=yyvs[yyvsp_offset];break;case 322: _LL3E8:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp787=_cycalloc(sizeof(*_tmp787));
_tmp787[0]=({struct Cyc_Exp_tok_struct _tmp788;_tmp788.tag=Cyc_Exp_tok;_tmp788.f1=
Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp788;});_tmp787;});break;case 323: _LL3E9: yyval=yyvs[
yyvsp_offset];break;case 324: _LL3EA: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp789=_cycalloc(sizeof(*_tmp789));_tmp789[0]=({struct Cyc_Exp_tok_struct _tmp78A;
_tmp78A.tag=Cyc_Exp_tok;_tmp78A.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp78A;});
_tmp789;});break;case 325: _LL3EB: yyval=yyvs[yyvsp_offset];break;case 326: _LL3EC:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp78B=_cycalloc(sizeof(*_tmp78B));
_tmp78B[0]=({struct Cyc_Exp_tok_struct _tmp78C;_tmp78C.tag=Cyc_Exp_tok;_tmp78C.f1=
Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp78C;});
_tmp78B;});break;case 327: _LL3ED: yyval=yyvs[yyvsp_offset];break;case 328: _LL3EE:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp78D=_cycalloc(sizeof(*_tmp78D));
_tmp78D[0]=({struct Cyc_Exp_tok_struct _tmp78E;_tmp78E.tag=Cyc_Exp_tok;_tmp78E.f1=
Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp78E;});
_tmp78D;});break;case 329: _LL3EF: yyval=yyvs[yyvsp_offset];break;case 330: _LL3F0:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp78F=_cycalloc(sizeof(*_tmp78F));
_tmp78F[0]=({struct Cyc_Exp_tok_struct _tmp790;_tmp790.tag=Cyc_Exp_tok;_tmp790.f1=
Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp790;});
_tmp78F;});break;case 331: _LL3F1: yyval=yyvs[yyvsp_offset];break;case 332: _LL3F2:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp791=_cycalloc(sizeof(*_tmp791));
_tmp791[0]=({struct Cyc_Exp_tok_struct _tmp792;_tmp792.tag=Cyc_Exp_tok;_tmp792.f1=
Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp792;});_tmp791;});break;case 333: _LL3F3: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp793=_cycalloc(sizeof(*_tmp793));_tmp793[0]=({
struct Cyc_Exp_tok_struct _tmp794;_tmp794.tag=Cyc_Exp_tok;_tmp794.f1=Cyc_Absyn_neq_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp794;});_tmp793;});break;case 334: _LL3F4: yyval=yyvs[
yyvsp_offset];break;case 335: _LL3F5: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp795=_cycalloc(sizeof(*_tmp795));_tmp795[0]=({struct Cyc_Exp_tok_struct _tmp796;
_tmp796.tag=Cyc_Exp_tok;_tmp796.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp796;});
_tmp795;});break;case 336: _LL3F6: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp797=
_cycalloc(sizeof(*_tmp797));_tmp797[0]=({struct Cyc_Exp_tok_struct _tmp798;_tmp798.tag=
Cyc_Exp_tok;_tmp798.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp798;});
_tmp797;});break;case 337: _LL3F7: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp799=
_cycalloc(sizeof(*_tmp799));_tmp799[0]=({struct Cyc_Exp_tok_struct _tmp79A;_tmp79A.tag=
Cyc_Exp_tok;_tmp79A.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp79A;});
_tmp799;});break;case 338: _LL3F8: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp79B=
_cycalloc(sizeof(*_tmp79B));_tmp79B[0]=({struct Cyc_Exp_tok_struct _tmp79C;_tmp79C.tag=
Cyc_Exp_tok;_tmp79C.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp79C;});
_tmp79B;});break;case 339: _LL3F9: yyval=yyvs[yyvsp_offset];break;case 340: _LL3FA:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp79D=_cycalloc(sizeof(*_tmp79D));
_tmp79D[0]=({struct Cyc_Exp_tok_struct _tmp79E;_tmp79E.tag=Cyc_Exp_tok;_tmp79E.f1=
Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp79E;});
_tmp79D;});break;case 341: _LL3FB: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp79F=
_cycalloc(sizeof(*_tmp79F));_tmp79F[0]=({struct Cyc_Exp_tok_struct _tmp7A0;_tmp7A0.tag=
Cyc_Exp_tok;_tmp7A0.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A0;});
_tmp79F;});break;case 342: _LL3FC: yyval=yyvs[yyvsp_offset];break;case 343: _LL3FD:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A1=_cycalloc(sizeof(*_tmp7A1));
_tmp7A1[0]=({struct Cyc_Exp_tok_struct _tmp7A2;_tmp7A2.tag=Cyc_Exp_tok;_tmp7A2.f1=
Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A2;});
_tmp7A1;});break;case 344: _LL3FE: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A3=
_cycalloc(sizeof(*_tmp7A3));_tmp7A3[0]=({struct Cyc_Exp_tok_struct _tmp7A4;_tmp7A4.tag=
Cyc_Exp_tok;_tmp7A4.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A4;});
_tmp7A3;});break;case 345: _LL3FF: yyval=yyvs[yyvsp_offset];break;case 346: _LL400:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A5=_cycalloc(sizeof(*_tmp7A5));
_tmp7A5[0]=({struct Cyc_Exp_tok_struct _tmp7A6;_tmp7A6.tag=Cyc_Exp_tok;_tmp7A6.f1=
Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A6;});
_tmp7A5;});break;case 347: _LL401: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A7=
_cycalloc(sizeof(*_tmp7A7));_tmp7A7[0]=({struct Cyc_Exp_tok_struct _tmp7A8;_tmp7A8.tag=
Cyc_Exp_tok;_tmp7A8.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A8;});
_tmp7A7;});break;case 348: _LL402: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A9=
_cycalloc(sizeof(*_tmp7A9));_tmp7A9[0]=({struct Cyc_Exp_tok_struct _tmp7AA;_tmp7AA.tag=
Cyc_Exp_tok;_tmp7AA.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7AA;});
_tmp7A9;});break;case 349: _LL403: yyval=yyvs[yyvsp_offset];break;case 350: _LL404:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7AB=_cycalloc(sizeof(*_tmp7AB));
_tmp7AB[0]=({struct Cyc_Exp_tok_struct _tmp7AC;_tmp7AC.tag=Cyc_Exp_tok;_tmp7AC.f1=
Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7AC;});_tmp7AB;});break;case 351: _LL405: yyval=yyvs[
yyvsp_offset];break;case 352: _LL406: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7AD=_cycalloc(sizeof(*_tmp7AD));_tmp7AD[0]=({struct Cyc_Exp_tok_struct _tmp7AE;
_tmp7AE.tag=Cyc_Exp_tok;_tmp7AE.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7AE;});
_tmp7AD;});break;case 353: _LL407: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7AF=
_cycalloc(sizeof(*_tmp7AF));_tmp7AF[0]=({struct Cyc_Exp_tok_struct _tmp7B0;_tmp7B0.tag=
Cyc_Exp_tok;_tmp7B0.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B0;});
_tmp7AF;});break;case 354: _LL408: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B1=
_cycalloc(sizeof(*_tmp7B1));_tmp7B1[0]=({struct Cyc_Exp_tok_struct _tmp7B2;_tmp7B2.tag=
Cyc_Exp_tok;_tmp7B2.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B2;});
_tmp7B1;});break;case 355: _LL409: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B3=
_cycalloc(sizeof(*_tmp7B3));_tmp7B3[0]=({struct Cyc_Exp_tok_struct _tmp7B4;_tmp7B4.tag=
Cyc_Exp_tok;_tmp7B4.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B4;});
_tmp7B3;});break;case 356: _LL40A: yyval=yyvs[yyvsp_offset];break;case 357: _LL40B:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B5=_cycalloc(sizeof(*_tmp7B5));
_tmp7B5[0]=({struct Cyc_Exp_tok_struct _tmp7B6;_tmp7B6.tag=Cyc_Exp_tok;_tmp7B6.f1=
Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7B6;});_tmp7B5;});break;case 358: _LL40C: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));_tmp7B7[0]=({
struct Cyc_Exp_tok_struct _tmp7B8;_tmp7B8.tag=Cyc_Exp_tok;_tmp7B8.f1=Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)])).f3,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B8;});
_tmp7B7;});break;case 359: _LL40D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B9=
_cycalloc(sizeof(*_tmp7B9));_tmp7B9[0]=({struct Cyc_Exp_tok_struct _tmp7BA;_tmp7BA.tag=
Cyc_Exp_tok;_tmp7BA.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7BA;});
_tmp7B9;});break;case 360: _LL40E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7BB=
_cycalloc(sizeof(*_tmp7BB));_tmp7BB[0]=({struct Cyc_Exp_tok_struct _tmp7BC;_tmp7BC.tag=
Cyc_Exp_tok;_tmp7BC.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp7BD=_cycalloc(sizeof(*_tmp7BD));_tmp7BD[0]=({struct Cyc_Absyn_StructField_struct
_tmp7BE;_tmp7BE.tag=0;_tmp7BE.f1=({struct _tagged_arr*_tmp7BF=_cycalloc(sizeof(*
_tmp7BF));_tmp7BF[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp7BF;});_tmp7BE;});_tmp7BD;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7BC;});_tmp7BB;});break;case 361: _LL40F: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7C0=_cycalloc(sizeof(*_tmp7C0));_tmp7C0[0]=({
struct Cyc_Exp_tok_struct _tmp7C1;_tmp7C1.tag=Cyc_Exp_tok;_tmp7C1.f1=Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp7C2=_cycalloc_atomic(
sizeof(*_tmp7C2));_tmp7C2[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp7C3;_tmp7C3.tag=
1;_tmp7C3.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp7C3;});_tmp7C2;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7C1;});
_tmp7C0;});break;case 362: _LL410: {struct Cyc_Position_Segment*_tmp7C4=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);struct Cyc_List_List*_tmp7C5=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp7C4,Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_Exp_tok_struct*_tmp7C6=_cycalloc(sizeof(*_tmp7C6));_tmp7C6[0]=({struct
Cyc_Exp_tok_struct _tmp7C7;_tmp7C7.tag=Cyc_Exp_tok;_tmp7C7.f1=Cyc_Absyn_gentyp_exp(
_tmp7C5,(*Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C7;});_tmp7C6;});break;}case 363: _LL411: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7C8=_cycalloc(sizeof(*_tmp7C8));_tmp7C8[0]=({
struct Cyc_Exp_tok_struct _tmp7C9;_tmp7C9.tag=Cyc_Exp_tok;_tmp7C9.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7CA=_cycalloc(sizeof(*_tmp7CA));
_tmp7CA[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7CB;_tmp7CB.tag=33;_tmp7CB.f1=({
struct Cyc_Absyn_MallocInfo _tmp7CC;_tmp7CC.is_calloc=0;_tmp7CC.rgn=0;_tmp7CC.elt_type=
0;_tmp7CC.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp7CC.fat_result=0;_tmp7CC;});_tmp7CB;});_tmp7CA;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C9;});_tmp7C8;});break;case 364: _LL412: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7CD=_cycalloc(sizeof(*_tmp7CD));_tmp7CD[0]=({
struct Cyc_Exp_tok_struct _tmp7CE;_tmp7CE.tag=Cyc_Exp_tok;_tmp7CE.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7CF=_cycalloc(sizeof(*_tmp7CF));
_tmp7CF[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7D0;_tmp7D0.tag=33;_tmp7D0.f1=({
struct Cyc_Absyn_MallocInfo _tmp7D1;_tmp7D1.is_calloc=0;_tmp7D1.rgn=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp7D1.elt_type=0;_tmp7D1.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7D1.fat_result=0;
_tmp7D1;});_tmp7D0;});_tmp7CF;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7CE;});_tmp7CD;});break;case 365: _LL413: {void*
_tmp7D3;struct _tuple2 _tmp7D2=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_LL416: _tmp7D3=_tmp7D2.f3;
goto _LL415;_LL415: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7D4=_cycalloc(
sizeof(*_tmp7D4));_tmp7D4[0]=({struct Cyc_Exp_tok_struct _tmp7D5;_tmp7D5.tag=Cyc_Exp_tok;
_tmp7D5.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7D6=
_cycalloc(sizeof(*_tmp7D6));_tmp7D6[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7D7;
_tmp7D7.tag=33;_tmp7D7.f1=({struct Cyc_Absyn_MallocInfo _tmp7D8;_tmp7D8.is_calloc=
1;_tmp7D8.rgn=0;_tmp7D8.elt_type=({void**_tmp7D9=_cycalloc(sizeof(*_tmp7D9));
_tmp7D9[0]=_tmp7D3;_tmp7D9;});_tmp7D8.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp7D8.fat_result=0;
_tmp7D8;});_tmp7D7;});_tmp7D6;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7D5;});_tmp7D4;});break;}case 366: _LL414: {void*
_tmp7DB;struct _tuple2 _tmp7DA=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_LL419: _tmp7DB=_tmp7DA.f3;
goto _LL418;_LL418: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7DC=_cycalloc(
sizeof(*_tmp7DC));_tmp7DC[0]=({struct Cyc_Exp_tok_struct _tmp7DD;_tmp7DD.tag=Cyc_Exp_tok;
_tmp7DD.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7DE=
_cycalloc(sizeof(*_tmp7DE));_tmp7DE[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7DF;
_tmp7DF.tag=33;_tmp7DF.f1=({struct Cyc_Absyn_MallocInfo _tmp7E0;_tmp7E0.is_calloc=
1;_tmp7E0.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp7E0.elt_type=({void**
_tmp7E1=_cycalloc(sizeof(*_tmp7E1));_tmp7E1[0]=_tmp7DB;_tmp7E1;});_tmp7E0.num_elts=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp7E0.fat_result=0;_tmp7E0;});_tmp7DF;});_tmp7DE;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7DD;});_tmp7DC;});break;}case 367: _LL417: yyval=(
void*)({struct Cyc_Primop_tok_struct*_tmp7E2=_cycalloc(sizeof(*_tmp7E2));_tmp7E2[
0]=({struct Cyc_Primop_tok_struct _tmp7E3;_tmp7E3.tag=Cyc_Primop_tok;_tmp7E3.f1=(
void*)((void*)12);_tmp7E3;});_tmp7E2;});break;case 368: _LL41A: yyval=(void*)({
struct Cyc_Primop_tok_struct*_tmp7E4=_cycalloc(sizeof(*_tmp7E4));_tmp7E4[0]=({
struct Cyc_Primop_tok_struct _tmp7E5;_tmp7E5.tag=Cyc_Primop_tok;_tmp7E5.f1=(void*)((
void*)11);_tmp7E5;});_tmp7E4;});break;case 369: _LL41B: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp7E6=_cycalloc(sizeof(*_tmp7E6));_tmp7E6[0]=({struct Cyc_Primop_tok_struct
_tmp7E7;_tmp7E7.tag=Cyc_Primop_tok;_tmp7E7.f1=(void*)((void*)2);_tmp7E7;});
_tmp7E6;});break;case 370: _LL41C: yyval=yyvs[yyvsp_offset];break;case 371: _LL41D:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7E8=_cycalloc(sizeof(*_tmp7E8));
_tmp7E8[0]=({struct Cyc_Exp_tok_struct _tmp7E9;_tmp7E9.tag=Cyc_Exp_tok;_tmp7E9.f1=
Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7E9;});_tmp7E8;});break;case 372: _LL41E: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7EA=_cycalloc(sizeof(*_tmp7EA));_tmp7EA[0]=({
struct Cyc_Exp_tok_struct _tmp7EB;_tmp7EB.tag=Cyc_Exp_tok;_tmp7EB.f1=Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7EB;});
_tmp7EA;});break;case 373: _LL41F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7EC=
_cycalloc(sizeof(*_tmp7EC));_tmp7EC[0]=({struct Cyc_Exp_tok_struct _tmp7ED;_tmp7ED.tag=
Cyc_Exp_tok;_tmp7ED.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7ED;});_tmp7EC;});break;case 374: _LL420: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7EE=_cycalloc(sizeof(*_tmp7EE));_tmp7EE[0]=({
struct Cyc_Exp_tok_struct _tmp7EF;_tmp7EF.tag=Cyc_Exp_tok;_tmp7EF.f1=Cyc_Absyn_aggrmember_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
struct _tagged_arr*_tmp7F0=_cycalloc(sizeof(*_tmp7F0));_tmp7F0[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp7F0;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7EF;});_tmp7EE;});break;case 375: _LL421: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1[0]=({
struct Cyc_Exp_tok_struct _tmp7F2;_tmp7F2.tag=Cyc_Exp_tok;_tmp7F2.f1=Cyc_Absyn_aggrarrow_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
struct _tagged_arr*_tmp7F3=_cycalloc(sizeof(*_tmp7F3));_tmp7F3[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp7F3;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F2;});_tmp7F1;});break;case 376: _LL422: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7F4=_cycalloc(sizeof(*_tmp7F4));_tmp7F4[0]=({
struct Cyc_Exp_tok_struct _tmp7F5;_tmp7F5.tag=Cyc_Exp_tok;_tmp7F5.f1=Cyc_Absyn_post_inc_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7F5;});
_tmp7F4;});break;case 377: _LL423: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7F6=
_cycalloc(sizeof(*_tmp7F6));_tmp7F6[0]=({struct Cyc_Exp_tok_struct _tmp7F7;_tmp7F7.tag=
Cyc_Exp_tok;_tmp7F7.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F7;});_tmp7F6;});break;case 378: _LL424: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7F8=_cycalloc(sizeof(*_tmp7F8));_tmp7F8[0]=({
struct Cyc_Exp_tok_struct _tmp7F9;_tmp7F9.tag=Cyc_Exp_tok;_tmp7F9.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp7FA=_cycalloc(sizeof(*_tmp7FA));
_tmp7FA[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp7FB;_tmp7FB.tag=25;_tmp7FB.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
4)]);_tmp7FB.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp7FB;});_tmp7FA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F9;});_tmp7F8;});break;case 379: _LL425: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7FC=_cycalloc(sizeof(*_tmp7FC));_tmp7FC[0]=({
struct Cyc_Exp_tok_struct _tmp7FD;_tmp7FD.tag=Cyc_Exp_tok;_tmp7FD.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp7FE=_cycalloc(sizeof(*_tmp7FE));
_tmp7FE[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp7FF;_tmp7FF.tag=25;_tmp7FF.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmp7FF.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp7FF;});_tmp7FE;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7FD;});_tmp7FC;});break;case 380: _LL426: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp800=_cycalloc(sizeof(*_tmp800));_tmp800[0]=({
struct Cyc_Exp_tok_struct _tmp801;_tmp801.tag=Cyc_Exp_tok;_tmp801.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Fill_e_struct*_tmp802=_cycalloc(sizeof(*_tmp802));
_tmp802[0]=({struct Cyc_Absyn_Fill_e_struct _tmp803;_tmp803.tag=37;_tmp803.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp803;});_tmp802;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp801;});
_tmp800;});break;case 381: _LL427: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp804=
_cycalloc(sizeof(*_tmp804));_tmp804[0]=({struct Cyc_Exp_tok_struct _tmp805;_tmp805.tag=
Cyc_Exp_tok;_tmp805.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Codegen_e_struct*
_tmp806=_cycalloc(sizeof(*_tmp806));_tmp806[0]=({struct Cyc_Absyn_Codegen_e_struct
_tmp807;_tmp807.tag=36;_tmp807.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp807;});_tmp806;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp805;});
_tmp804;});break;case 382: _LL428: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp808=
_cycalloc(sizeof(*_tmp808));_tmp808[0]=({struct Cyc_Exp_tok_struct _tmp809;_tmp809.tag=
Cyc_Exp_tok;_tmp809.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp80A=_cycalloc(sizeof(*_tmp80A));_tmp80A[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp80B;_tmp80B.tag=2;_tmp80B.f1=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);
_tmp80B;});_tmp80A;}),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp809;});_tmp808;});break;case 383: _LL429: yyval=
yyvs[yyvsp_offset];break;case 384: _LL42A: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp80C=_cycalloc(sizeof(*_tmp80C));_tmp80C[0]=({struct Cyc_Exp_tok_struct _tmp80D;
_tmp80D.tag=Cyc_Exp_tok;_tmp80D.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp80D;});_tmp80C;});break;case 385: _LL42B: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 386: _LL42C:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp80E=_cycalloc(sizeof(*_tmp80E));
_tmp80E[0]=({struct Cyc_Exp_tok_struct _tmp80F;_tmp80F.tag=Cyc_Exp_tok;_tmp80F.f1=
Cyc_Absyn_noinstantiate_exp(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp810=_cycalloc(sizeof(*_tmp810));_tmp810[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp811;_tmp811.tag=2;_tmp811.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp811;});_tmp810;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp80F;});_tmp80E;});break;case 387: _LL42D: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp812=_cycalloc(sizeof(*_tmp812));_tmp812[0]=({
struct Cyc_Exp_tok_struct _tmp813;_tmp813.tag=Cyc_Exp_tok;_tmp813.f1=Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp814=_cycalloc(
sizeof(*_tmp814));_tmp814[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp815;_tmp815.tag=
2;_tmp815.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp815;});_tmp814;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp813;});
_tmp812;});break;case 388: _LL42E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp816=
_cycalloc(sizeof(*_tmp816));_tmp816[0]=({struct Cyc_Exp_tok_struct _tmp817;_tmp817.tag=
Cyc_Exp_tok;_tmp817.f1=Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp817;});_tmp816;});break;case 389: _LL42F: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp818=_cycalloc(sizeof(*_tmp818));_tmp818[0]=({
struct Cyc_Exp_tok_struct _tmp819;_tmp819.tag=Cyc_Exp_tok;_tmp819.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Struct_e_struct*_tmp81A=_cycalloc(sizeof(*_tmp81A));
_tmp81A[0]=({struct Cyc_Absyn_Struct_e_struct _tmp81B;_tmp81B.tag=28;_tmp81B.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp81B.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp81B.f3=0;_tmp81B;});_tmp81A;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp819;});_tmp818;});break;case 390: _LL430: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C[0]=({
struct Cyc_Exp_tok_struct _tmp81D;_tmp81D.tag=Cyc_Exp_tok;_tmp81D.f1=Cyc_Absyn_stmt_exp(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp81D;});
_tmp81C;});break;case 391: _LL431: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E[0]=({struct Cyc_ExpList_tok_struct
_tmp81F;_tmp81F.tag=Cyc_ExpList_tok;_tmp81F.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(yyvs[yyvsp_offset]));
_tmp81F;});_tmp81E;});break;case 392: _LL432: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp820=_cycalloc(sizeof(*_tmp820));_tmp820[0]=({struct Cyc_ExpList_tok_struct
_tmp821;_tmp821.tag=Cyc_ExpList_tok;_tmp821.f1=({struct Cyc_List_List*_tmp822=
_cycalloc(sizeof(*_tmp822));_tmp822->hd=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);
_tmp822->tl=0;_tmp822;});_tmp821;});_tmp820;});break;case 393: _LL433: yyval=(void*)({
struct Cyc_ExpList_tok_struct*_tmp823=_cycalloc(sizeof(*_tmp823));_tmp823[0]=({
struct Cyc_ExpList_tok_struct _tmp824;_tmp824.tag=Cyc_ExpList_tok;_tmp824.f1=({
struct Cyc_List_List*_tmp825=_cycalloc(sizeof(*_tmp825));_tmp825->hd=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp825->tl=Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp825;});_tmp824;});
_tmp823;});break;case 394: _LL434: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp826=
_cycalloc(sizeof(*_tmp826));_tmp826[0]=({struct Cyc_Exp_tok_struct _tmp827;_tmp827.tag=
Cyc_Exp_tok;_tmp827.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f1,(*
Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp827;});_tmp826;});
break;case 395: _LL435: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp828=_cycalloc(
sizeof(*_tmp828));_tmp828[0]=({struct Cyc_Exp_tok_struct _tmp829;_tmp829.tag=Cyc_Exp_tok;
_tmp829.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp829;});
_tmp828;});break;case 396: _LL436: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp82A=
_cycalloc(sizeof(*_tmp82A));_tmp82A[0]=({struct Cyc_Exp_tok_struct _tmp82B;_tmp82B.tag=
Cyc_Exp_tok;_tmp82B.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));_tmp82B;});_tmp82A;});break;case 397: _LL437: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp82C=_cycalloc(sizeof(*_tmp82C));_tmp82C[0]=({
struct Cyc_Exp_tok_struct _tmp82D;_tmp82D.tag=Cyc_Exp_tok;_tmp82D.f1=Cyc_Absyn_null_exp(
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmp82D;});_tmp82C;});break;case 398: _LL438: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp82E=_cycalloc(sizeof(*_tmp82E));_tmp82E[0]=({struct Cyc_QualId_tok_struct
_tmp82F;_tmp82F.tag=Cyc_QualId_tok;_tmp82F.f1=({struct _tuple1*_tmp830=_cycalloc(
sizeof(*_tmp830));_tmp830->f1=Cyc_Absyn_rel_ns_null;_tmp830->f2=({struct
_tagged_arr*_tmp831=_cycalloc(sizeof(*_tmp831));_tmp831[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp831;});_tmp830;});_tmp82F;});_tmp82E;});break;case 399:
_LL439: yyval=yyvs[yyvsp_offset];break;case 400: _LL43A: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp832=_cycalloc(sizeof(*_tmp832));_tmp832[0]=({struct Cyc_QualId_tok_struct
_tmp833;_tmp833.tag=Cyc_QualId_tok;_tmp833.f1=({struct _tuple1*_tmp834=_cycalloc(
sizeof(*_tmp834));_tmp834->f1=Cyc_Absyn_rel_ns_null;_tmp834->f2=({struct
_tagged_arr*_tmp835=_cycalloc(sizeof(*_tmp835));_tmp835[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp835;});_tmp834;});_tmp833;});_tmp832;});break;case 401:
_LL43B: yyval=yyvs[yyvsp_offset];break;case 402: _LL43C: yyval=yyvs[yyvsp_offset];
break;case 403: _LL43D: yyval=yyvs[yyvsp_offset];break;case 404: _LL43E: yyval=yyvs[
yyvsp_offset];break;case 405: _LL43F: yyval=yyvs[yyvsp_offset];break;case 406: _LL440:
break;case 407: _LL441:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;break;default: _LL442: break;}
yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;yylsp_offset ++;if(
yylen == 0){(yyls[yylsp_offset]).first_line=Cyc_yylloc.first_line;(yyls[
yylsp_offset]).first_column=Cyc_yylloc.first_column;(yyls[yylsp_offset]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
yylsp_offset]).last_column=(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_column;}else{(yyls[yylsp_offset]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
yylsp_offset]).last_column=(yyls[_check_known_subscript_notnull(10000,(
yylsp_offset + yylen)- 1)]).last_column;}yyn=(int)Cyc_yyr1[yyn];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(115,yyn - 126)]+ yyss[yyssp_offset];if((yystate >= 
0? yystate <= 4977: 0)? Cyc_yycheck[yystate]== yyss[yyssp_offset]: 0){yystate=(int)
Cyc_yytable[yystate];}else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(115,yyn - 126)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[yystate];if(yyn > - 32768? yyn < 
4977: 0){int sze=0;struct _tagged_arr msg;int x;int count;count=0;for(x=yyn < 0? - yyn: 0;
x < 241 / sizeof(unsigned char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(4978,x + yyn)]== x){(sze +=Cyc_Std_strlen(Cyc_yytname[
x])+ 15,count ++);}}msg=({unsigned int _tmp836=(unsigned int)(sze + 15);
unsigned char*_tmp837=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmp836));struct _tagged_arr _tmp839=_tag_arr(_tmp837,sizeof(
unsigned char),(unsigned int)(sze + 15));{unsigned int _tmp838=_tmp836;
unsigned int i;for(i=0;i < _tmp838;i ++){_tmp837[i]='\000';}};_tmp839;});Cyc_Std_strcpy(
msg,_tag_arr("parse error",sizeof(unsigned char),12));if(count < 5){count=0;for(x=
yyn < 0? - yyn: 0;x < 241 / sizeof(unsigned char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(4978,x + yyn)]== x){Cyc_Std_strcat(msg,count == 0?
_tag_arr(", expecting `",sizeof(unsigned char),14): _tag_arr(" or `",sizeof(
unsigned char),6));Cyc_Std_strcat(msg,Cyc_yytname[x]);Cyc_Std_strcat(msg,
_tag_arr("'",sizeof(unsigned char),2));count ++;}}}Cyc_yyerror((struct _tagged_arr)
msg);}else{Cyc_yyerror(_tag_arr("parse error",sizeof(unsigned char),12));}}goto
yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){return 1;}Cyc_yychar=-
2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0){
return 1;}yyvsp_offset --;yystate=(int)yyss[_check_known_subscript_notnull(10000,
-- yyssp_offset)];yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[yystate];if(yyn
== - 32768){goto yyerrdefault;}yyn +=1;if((yyn < 0? 1: yyn > 4977)? 1: Cyc_yycheck[yyn]!= 
1){goto yyerrdefault;}yyn=(int)Cyc_yytable[yyn];if(yyn < 0){if(yyn == - 32768){goto
yyerrpop;}yyn=- yyn;goto yyreduce;}else{if(yyn == 0){goto yyerrpop;}}if(yyn == 829){
return 0;}yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;
yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto
yynewstate;}void Cyc_yyprint(int i,void*v){void*_tmp843=v;struct _tuple12*_tmp844;
struct _tuple12 _tmp845;int _tmp846;unsigned char _tmp847;short _tmp848;struct
_tagged_arr _tmp849;struct Cyc_Core_Opt*_tmp84A;struct Cyc_Core_Opt*_tmp84B;struct
Cyc_Core_Opt _tmp84C;struct _tagged_arr*_tmp84D;struct _tuple1*_tmp84E;struct
_tuple1 _tmp84F;struct _tagged_arr*_tmp850;void*_tmp851;_LL445: if(*((void**)
_tmp843)== Cyc_Int_tok){_LL455: _tmp844=((struct Cyc_Int_tok_struct*)_tmp843)->f1;
_tmp845=*_tmp844;_LL456: _tmp846=_tmp845.f2;goto _LL446;}else{goto _LL447;}_LL447:
if(*((void**)_tmp843)== Cyc_Char_tok){_LL457: _tmp847=((struct Cyc_Char_tok_struct*)
_tmp843)->f1;goto _LL448;}else{goto _LL449;}_LL449: if(*((void**)_tmp843)== Cyc_Short_tok){
_LL458: _tmp848=((struct Cyc_Short_tok_struct*)_tmp843)->f1;goto _LL44A;}else{goto
_LL44B;}_LL44B: if(*((void**)_tmp843)== Cyc_String_tok){_LL459: _tmp849=((struct
Cyc_String_tok_struct*)_tmp843)->f1;goto _LL44C;}else{goto _LL44D;}_LL44D: if(*((
void**)_tmp843)== Cyc_Stringopt_tok){_LL45A: _tmp84A=((struct Cyc_Stringopt_tok_struct*)
_tmp843)->f1;if(_tmp84A == 0){goto _LL44E;}else{goto _LL44F;}}else{goto _LL44F;}
_LL44F: if(*((void**)_tmp843)== Cyc_Stringopt_tok){_LL45B: _tmp84B=((struct Cyc_Stringopt_tok_struct*)
_tmp843)->f1;if(_tmp84B == 0){goto _LL451;}else{_tmp84C=*_tmp84B;_LL45C: _tmp84D=(
struct _tagged_arr*)_tmp84C.v;goto _LL450;}}else{goto _LL451;}_LL451: if(*((void**)
_tmp843)== Cyc_QualId_tok){_LL45D: _tmp84E=((struct Cyc_QualId_tok_struct*)_tmp843)->f1;
_tmp84F=*_tmp84E;_LL45F: _tmp851=_tmp84F.f1;goto _LL45E;_LL45E: _tmp850=_tmp84F.f2;
goto _LL452;}else{goto _LL453;}_LL453: goto _LL454;_LL446:({struct Cyc_Std_Int_pa_struct
_tmp853;_tmp853.tag=1;_tmp853.f1=(int)((unsigned int)_tmp846);{void*_tmp852[1]={&
_tmp853};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%d",sizeof(unsigned char),3),
_tag_arr(_tmp852,sizeof(void*),1));}});goto _LL444;_LL448:({struct Cyc_Std_Int_pa_struct
_tmp855;_tmp855.tag=1;_tmp855.f1=(int)((unsigned int)((int)_tmp847));{void*
_tmp854[1]={& _tmp855};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%c",sizeof(
unsigned char),3),_tag_arr(_tmp854,sizeof(void*),1));}});goto _LL444;_LL44A:({
struct Cyc_Std_Int_pa_struct _tmp857;_tmp857.tag=1;_tmp857.f1=(int)((unsigned int)((
int)_tmp848));{void*_tmp856[1]={& _tmp857};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%ds",sizeof(unsigned char),4),_tag_arr(_tmp856,sizeof(void*),1));}});
goto _LL444;_LL44C:({struct Cyc_Std_String_pa_struct _tmp859;_tmp859.tag=0;_tmp859.f1=(
struct _tagged_arr)_tmp849;{void*_tmp858[1]={& _tmp859};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\"%s\"",sizeof(unsigned char),5),_tag_arr(_tmp858,sizeof(void*),1));}});
goto _LL444;_LL44E:({void*_tmp85A[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("NULL",
sizeof(unsigned char),5),_tag_arr(_tmp85A,sizeof(void*),0));});goto _LL444;_LL450:({
struct Cyc_Std_String_pa_struct _tmp85C;_tmp85C.tag=0;_tmp85C.f1=(struct
_tagged_arr)*_tmp84D;{void*_tmp85B[1]={& _tmp85C};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\"%s\"",sizeof(unsigned char),5),_tag_arr(_tmp85B,sizeof(void*),1));}});
goto _LL444;_LL452: {struct Cyc_List_List*_tmp85D=0;{void*_tmp85E=_tmp851;struct
Cyc_List_List*_tmp85F;struct Cyc_List_List*_tmp860;_LL461: if((unsigned int)
_tmp85E > 1?*((int*)_tmp85E)== 0: 0){_LL467: _tmp85F=((struct Cyc_Absyn_Rel_n_struct*)
_tmp85E)->f1;goto _LL462;}else{goto _LL463;}_LL463: if((unsigned int)_tmp85E > 1?*((
int*)_tmp85E)== 1: 0){_LL468: _tmp860=((struct Cyc_Absyn_Abs_n_struct*)_tmp85E)->f1;
goto _LL464;}else{goto _LL465;}_LL465: if((int)_tmp85E == 0){goto _LL466;}else{goto
_LL460;}_LL462: _tmp85D=_tmp85F;goto _LL460;_LL464: _tmp85D=_tmp860;goto _LL460;
_LL466: goto _LL460;_LL460:;}for(0;_tmp85D != 0;_tmp85D=_tmp85D->tl){({struct Cyc_Std_String_pa_struct
_tmp862;_tmp862.tag=0;_tmp862.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp85D->hd);{void*_tmp861[1]={& _tmp862};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",
sizeof(unsigned char),5),_tag_arr(_tmp861,sizeof(void*),1));}});}({struct Cyc_Std_String_pa_struct
_tmp864;_tmp864.tag=0;_tmp864.f1=(struct _tagged_arr)*_tmp850;{void*_tmp863[1]={&
_tmp864};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",sizeof(unsigned char),5),
_tag_arr(_tmp863,sizeof(void*),1));}});goto _LL444;}_LL454:({void*_tmp865[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("?",sizeof(unsigned char),2),_tag_arr(
_tmp865,sizeof(void*),0));});goto _LL444;_LL444:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc_Std___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*
_tmp866=_cycalloc(sizeof(*_tmp866));_tmp866->v=Cyc_Lexing_from_file(f);_tmp866;});
Cyc_yyparse();return Cyc_Parse_parse_result;}
