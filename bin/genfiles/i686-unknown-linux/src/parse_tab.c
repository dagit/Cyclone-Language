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
extern void* _region_malloc(struct _RegionHandle *, unsigned);
extern void* _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void  _free_region(struct _RegionHandle *);

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
tag;unsigned int*f1;};int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr);struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);struct _tagged_arr Cyc_Std_vrprintf(struct
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
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file(struct Cyc_Std___cycFILE*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void*Cyc_List_fold_right(
void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*
x);extern unsigned char Cyc_List_Nth[8];struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);unsigned int Cyc_Std_strlen(
struct _tagged_arr s);int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);
int Cyc_Std_zstrcmp(struct _tagged_arr,struct _tagged_arr);int Cyc_Std_zstrptrcmp(
struct _tagged_arr*,struct _tagged_arr*);struct _tagged_arr Cyc_Std_strcat(struct
_tagged_arr dest,struct _tagged_arr src);struct _tagged_arr Cyc_Std_strcpy(struct
_tagged_arr dest,struct _tagged_arr src);struct _tagged_arr Cyc_Std_substring(struct
_tagged_arr,int ofs,unsigned int n);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct Cyc_Position_Segment*Cyc_Position_segment_join(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};struct Cyc_Position_Error*
Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*,struct _tagged_arr);extern
unsigned char Cyc_Position_Nocontext[14];void Cyc_Position_post_error(struct Cyc_Position_Error*);
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};
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
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
exist_vars;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
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
15];extern void*Cyc_Absyn_rel_ns_null;int Cyc_Absyn_is_qvar_qualified(struct
_tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,
struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_uchar_t;extern
void*Cyc_Absyn_uint_t;extern void*Cyc_Absyn_sint_t;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);extern struct _tuple1*Cyc_Absyn_exn_name;extern
struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;void*Cyc_Absyn_starb_typ(void*t,
void*rgn,struct Cyc_Absyn_Tqual tq,void*b);void*Cyc_Absyn_atb_typ(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,void*b);void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
unsigned char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _tagged_arr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _tagged_arr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(
void*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _tagged_arr*lab,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_forarray_stmt(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct
Cyc_Position_Segment*s);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Core_Opt*
t_opt,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_letv_decl(struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple1*n,
struct Cyc_List_List*ts,struct Cyc_Core_Opt*exist_ts,struct Cyc_Core_Opt*fs,struct
Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(
void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int
is_xtunion,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(struct Cyc_List_List*
tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,
struct Cyc_List_List*);struct _tagged_arr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
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
ae;struct Cyc_Core_Opt*le;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_copy_type(void*t);void*
Cyc_Tcutil_compress(void*t);void Cyc_Lex_register_typedef(struct _tuple1*s);void
Cyc_Lex_enter_namespace(struct _tagged_arr*);void Cyc_Lex_leave_namespace();void
Cyc_Lex_enter_using(struct _tuple1*);void Cyc_Lex_leave_using();struct Cyc_Parse_Signed_spec_struct{
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
loc,struct _tuple7*field_info){struct _tuple7 _tmpA;struct _tuple6*_tmpB;struct
_tuple6 _tmpC;struct _tuple1*_tmpD;struct Cyc_Absyn_Tqual _tmpE;void*_tmpF;struct Cyc_List_List*
_tmp10;struct Cyc_List_List*_tmp11;struct Cyc_Absyn_Exp*_tmp12;struct _tuple7*_tmp9=
field_info;_tmpA=*_tmp9;_tmpB=_tmpA.f1;_tmpC=*_tmpB;_tmpD=_tmpC.f1;_tmpE=_tmpC.f2;
_tmpF=_tmpC.f3;_tmp10=_tmpC.f4;_tmp11=_tmpC.f5;_tmp12=_tmpA.f2;if(_tmp10 != 0)Cyc_Parse_err(
_tag_arr("bad type params in struct field",sizeof(unsigned char),32),loc);if(Cyc_Absyn_is_qvar_qualified(
_tmpD))Cyc_Parse_err(_tag_arr("struct or union field cannot be qualified with a namespace",
sizeof(unsigned char),59),loc);return({struct Cyc_Absyn_Aggrfield*_tmp13=
_cycalloc(sizeof(*_tmp13));_tmp13->name=(*_tmpD).f2;_tmp13->tq=_tmpE;_tmp13->type=(
void*)_tmpF;_tmp13->width=_tmp12;_tmp13->attributes=_tmp11;_tmp13;});}static void*
Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*loc){return(void*)({struct
Cyc_Parse_Type_spec_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14[0]=({struct
Cyc_Parse_Type_spec_struct _tmp15;_tmp15.tag=4;_tmp15.f1=(void*)t;_tmp15.f2=loc;
_tmp15;});_tmp14;});}static void*Cyc_Parse_array2ptr(void*t,int argposn){void*
_tmp16=t;void*_tmp17;struct Cyc_Absyn_Tqual _tmp18;struct Cyc_Absyn_Exp*_tmp19;_LL1:
if(_tmp16 <= (void*)3?1:*((int*)_tmp16)!= 7)goto _LL3;_tmp17=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp16)->f1;_tmp18=((struct Cyc_Absyn_ArrayType_struct*)_tmp16)->f2;_tmp19=((
struct Cyc_Absyn_ArrayType_struct*)_tmp16)->f3;_LL2: return Cyc_Absyn_starb_typ(
_tmp17,argposn?Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp1A=_cycalloc(sizeof(*
_tmp1A));_tmp1A->v=(void*)((void*)3);_tmp1A;}),0):(void*)2,_tmp18,_tmp19 == 0?(
void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));
_tmp1B[0]=({struct Cyc_Absyn_Upper_b_struct _tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct
Cyc_Absyn_Exp*)_check_null(_tmp19);_tmp1C;});_tmp1B;}));_LL3:;_LL4: return t;_LL0:;}
static void Cyc_Parse_arg_array2ptr(struct _tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*
x).f3,1);}struct _tuple8{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple8*
Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple2*t){return({
struct _tuple8*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->f1=(*t).f2;_tmp1D->f2=(*
t).f3;_tmp1D;});}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*
d){void*_tmp1E=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp1F;_LL6: if(*((int*)_tmp1E)
!= 0)goto _LL8;_tmp1F=((struct Cyc_Absyn_Var_d_struct*)_tmp1E)->f1;_LL7: return
_tmp1F;_LL8:;_LL9:({void*_tmp20[0]={};((int(*)(struct Cyc_Position_Segment*sg,
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(d->loc,_tag_arr("bad declaration in `forarray' statement",
sizeof(unsigned char),40),_tag_arr(_tmp20,sizeof(void*),0));});_LL5:;}static int
Cyc_Parse_is_typeparam(void*tm){void*_tmp21=tm;_LLB: if(_tmp21 <= (void*)1?1:*((
int*)_tmp21)!= 3)goto _LLD;_LLC: return 1;_LLD:;_LLE: return 0;_LLA:;}static void*Cyc_Parse_id2type(
struct _tagged_arr s,void*k){if(Cyc_Std_zstrcmp(s,_tag_arr("`H",sizeof(
unsigned char),3))== 0)return(void*)2;else{return(void*)({struct Cyc_Absyn_VarType_struct*
_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22[0]=({struct Cyc_Absyn_VarType_struct
_tmp23;_tmp23.tag=1;_tmp23.f1=({struct Cyc_Absyn_Tvar*_tmp24=_cycalloc(sizeof(*
_tmp24));_tmp24->name=({struct _tagged_arr*_tmp25=_cycalloc(sizeof(*_tmp25));
_tmp25[0]=s;_tmp25;});_tmp24->identity=0;_tmp24->kind=(void*)k;_tmp24;});_tmp23;});
_tmp22;});}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*
t){void*k;{void*_tmp26=Cyc_Absyn_compress_kb((void*)t->kind);void*_tmp27;void*
_tmp28;_LL10: if(*((int*)_tmp26)!= 0)goto _LL12;_tmp27=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp26)->f1;_LL11: k=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp29=_cycalloc(
sizeof(*_tmp29));_tmp29[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp2A;_tmp2A.tag=0;
_tmp2A.f1=(void*)_tmp27;_tmp2A;});_tmp29;});goto _LLF;_LL12: if(*((int*)_tmp26)!= 
1)goto _LL14;_LL13: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp2B=_cycalloc(
sizeof(*_tmp2B));_tmp2B[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp2C;_tmp2C.tag=
1;_tmp2C.f1=0;_tmp2C;});_tmp2B;});goto _LLF;_LL14: if(*((int*)_tmp26)!= 2)goto _LLF;
_tmp28=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp26)->f2;_LL15: k=(void*)({
struct Cyc_Absyn_Less_kb_struct*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D[0]=({
struct Cyc_Absyn_Less_kb_struct _tmp2E;_tmp2E.tag=2;_tmp2E.f1=0;_tmp2E.f2=(void*)
_tmp28;_tmp2E;});_tmp2D;});goto _LLF;_LLF:;}return({struct Cyc_Absyn_Tvar*_tmp2F=
_cycalloc(sizeof(*_tmp2F));_tmp2F->name=t->name;_tmp2F->identity=0;_tmp2F->kind=(
void*)k;_tmp2F;});}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmp30=t;struct Cyc_Absyn_Tvar*_tmp31;_LL17: if(_tmp30 <= (void*)3?
1:*((int*)_tmp30)!= 1)goto _LL19;_tmp31=((struct Cyc_Absyn_VarType_struct*)_tmp30)->f1;
_LL18: return _tmp31;_LL19:;_LL1A:({void*_tmp32[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,_tag_arr("expecting a list of type variables, not types",
sizeof(unsigned char),46),_tag_arr(_tmp32,sizeof(void*),0));});_LL16:;}static
void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){return(void*)({struct Cyc_Absyn_VarType_struct*
_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Absyn_VarType_struct
_tmp34;_tmp34.tag=1;_tmp34.f1=pr;_tmp34;});_tmp33;});}static void Cyc_Parse_set_vartyp_kind(
void*t,void*k){void*_tmp35=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp36;
struct Cyc_Absyn_Tvar _tmp37;void*_tmp38;void**_tmp39;_LL1C: if(_tmp35 <= (void*)3?1:*((
int*)_tmp35)!= 1)goto _LL1E;_tmp36=((struct Cyc_Absyn_VarType_struct*)_tmp35)->f1;
_tmp37=*_tmp36;_tmp38=(void*)_tmp37.kind;_tmp39=(void**)&(*((struct Cyc_Absyn_VarType_struct*)
_tmp35)->f1).kind;_LL1D: {void*_tmp3A=Cyc_Absyn_compress_kb(*_tmp39);_LL21: if(*((
int*)_tmp3A)!= 1)goto _LL23;_LL22:*_tmp39=(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp3C;
_tmp3C.tag=0;_tmp3C.f1=(void*)k;_tmp3C;});_tmp3B;});return;_LL23:;_LL24: return;
_LL20:;}_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp3D=(void*)tms->hd;void*_tmp3E;
_LL26: if(_tmp3D <= (void*)1?1:*((int*)_tmp3D)!= 2)goto _LL28;_tmp3E=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmp3D)->f1;_LL27: if(tms->tl == 0?1:(Cyc_Parse_is_typeparam((
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)?((struct Cyc_List_List*)
_check_null(tms->tl))->tl == 0: 0)){void*_tmp3F=_tmp3E;struct Cyc_List_List*_tmp40;
_LL2B: if(*((int*)_tmp3F)!= 1)goto _LL2D;_LL2C:({void*_tmp41[0]={};Cyc_Tcutil_warn(
loc,_tag_arr("function declaration with both new- and old-style parameter declarations; ignoring old-style",
sizeof(unsigned char),93),_tag_arr(_tmp41,sizeof(void*),0));});return tms;_LL2D:
if(*((int*)_tmp3F)!= 0)goto _LL2A;_tmp40=((struct Cyc_Absyn_NoTypes_struct*)_tmp3F)->f1;
_LL2E: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp40)!= ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(tds))({void*_tmp42[0]={};((int(*)(struct
Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(
loc,_tag_arr("wrong number of parameter declarations in old-style function declaration",
sizeof(unsigned char),73),_tag_arr(_tmp42,sizeof(void*),0));});{struct Cyc_List_List*
_tmp43=0;for(0;_tmp40 != 0;_tmp40=_tmp40->tl){struct Cyc_List_List*_tmp44=tds;for(
0;_tmp44 != 0;_tmp44=_tmp44->tl){struct Cyc_Absyn_Decl*_tmp45=(struct Cyc_Absyn_Decl*)
_tmp44->hd;void*_tmp46=(void*)_tmp45->r;struct Cyc_Absyn_Vardecl*_tmp47;_LL30: if(*((
int*)_tmp46)!= 0)goto _LL32;_tmp47=((struct Cyc_Absyn_Var_d_struct*)_tmp46)->f1;
_LL31: if(Cyc_Std_zstrptrcmp((*_tmp47->name).f2,(struct _tagged_arr*)_tmp40->hd)!= 
0)continue;if(_tmp47->initializer != 0)({void*_tmp48[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp45->loc,
_tag_arr("initializer found in parameter declaration",sizeof(unsigned char),43),
_tag_arr(_tmp48,sizeof(void*),0));});if(Cyc_Absyn_is_qvar_qualified(_tmp47->name))({
void*_tmp49[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Parse_abort)(_tmp45->loc,_tag_arr("namespaces forbidden in parameter declarations",
sizeof(unsigned char),47),_tag_arr(_tmp49,sizeof(void*),0));});_tmp43=({struct
Cyc_List_List*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->hd=({struct _tuple2*
_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->f1=({struct Cyc_Core_Opt*_tmp4C=
_cycalloc(sizeof(*_tmp4C));_tmp4C->v=(*_tmp47->name).f2;_tmp4C;});_tmp4B->f2=
_tmp47->tq;_tmp4B->f3=(void*)_tmp47->type;_tmp4B;});_tmp4A->tl=_tmp43;_tmp4A;});
goto L;_LL32:;_LL33:({void*_tmp4D[0]={};((int(*)(struct Cyc_Position_Segment*sg,
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp45->loc,_tag_arr("nonvariable declaration in parameter type",
sizeof(unsigned char),42),_tag_arr(_tmp4D,sizeof(void*),0));});_LL2F:;}L: if(
_tmp44 == 0)({struct Cyc_Std_String_pa_struct _tmp4F;_tmp4F.tag=0;_tmp4F.f1=(struct
_tagged_arr)*((struct _tagged_arr*)_tmp40->hd);{void*_tmp4E[1]={& _tmp4F};((int(*)(
struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(
loc,_tag_arr("%s is not given a type",sizeof(unsigned char),23),_tag_arr(_tmp4E,
sizeof(void*),1));}});}return({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*
_tmp50));_tmp50->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp51=
_cycalloc(sizeof(*_tmp51));_tmp51[0]=({struct Cyc_Absyn_Function_mod_struct _tmp52;
_tmp52.tag=2;_tmp52.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp53=
_cycalloc(sizeof(*_tmp53));_tmp53[0]=({struct Cyc_Absyn_WithTypes_struct _tmp54;
_tmp54.tag=1;_tmp54.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp43);_tmp54.f2=0;_tmp54.f3=0;_tmp54.f4=0;_tmp54.f5=0;_tmp54;});_tmp53;}));
_tmp52;});_tmp51;}));_tmp50->tl=0;_tmp50;});}_LL2A:;}goto _LL29;_LL28:;_LL29:
return({struct Cyc_List_List*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->hd=(void*)((
void*)tms->hd);_tmp55->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);_tmp55;});
_LL25:;}}struct _tuple9{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,
struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,
struct Cyc_Position_Segment*loc){if(tds != 0)d=({struct Cyc_Parse_Declarator*_tmp56=
_cycalloc(sizeof(*_tmp56));_tmp56->id=d->id;_tmp56->tms=Cyc_Parse_oldstyle2newstyle(
d->tms,tds,loc);_tmp56;});{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual();int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp57=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL35: if((int)_tmp57 != 1)goto _LL37;
_LL36: sc=(void*)3;goto _LL34;_LL37: if((int)_tmp57 != 3)goto _LL39;_LL38: sc=(void*)0;
goto _LL34;_LL39:;_LL3A: Cyc_Parse_err(_tag_arr("bad storage class on function",
sizeof(unsigned char),30),loc);goto _LL34;_LL34:;}}{void*_tmp59;struct Cyc_Core_Opt*
_tmp5A;struct _tuple4 _tmp58=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmp59=
_tmp58.f1;_tmp5A=_tmp58.f2;{struct Cyc_Absyn_Tqual _tmp5C;void*_tmp5D;struct Cyc_List_List*
_tmp5E;struct Cyc_List_List*_tmp5F;struct _tuple5 _tmp5B=Cyc_Parse_apply_tms(tq,
_tmp59,atts,d->tms);_tmp5C=_tmp5B.f1;_tmp5D=_tmp5B.f2;_tmp5E=_tmp5B.f3;_tmp5F=
_tmp5B.f4;if(_tmp5A != 0)({void*_tmp60[0]={};Cyc_Tcutil_warn(loc,_tag_arr("nested type declaration within function prototype",
sizeof(unsigned char),50),_tag_arr(_tmp60,sizeof(void*),0));});if(_tmp5E != 0)({
void*_tmp61[0]={};Cyc_Tcutil_warn(loc,_tag_arr("bad type params, ignoring",
sizeof(unsigned char),26),_tag_arr(_tmp61,sizeof(void*),0));});{void*_tmp62=
_tmp5D;struct Cyc_Absyn_FnInfo _tmp63;struct Cyc_List_List*_tmp64;struct Cyc_Core_Opt*
_tmp65;void*_tmp66;struct Cyc_List_List*_tmp67;int _tmp68;struct Cyc_Absyn_VarargInfo*
_tmp69;struct Cyc_List_List*_tmp6A;struct Cyc_List_List*_tmp6B;_LL3C: if(_tmp62 <= (
void*)3?1:*((int*)_tmp62)!= 8)goto _LL3E;_tmp63=((struct Cyc_Absyn_FnType_struct*)
_tmp62)->f1;_tmp64=_tmp63.tvars;_tmp65=_tmp63.effect;_tmp66=(void*)_tmp63.ret_typ;
_tmp67=_tmp63.args;_tmp68=_tmp63.c_varargs;_tmp69=_tmp63.cyc_varargs;_tmp6A=
_tmp63.rgn_po;_tmp6B=_tmp63.attributes;_LL3D: {struct Cyc_List_List*_tmp6C=0;{
struct Cyc_List_List*_tmp6D=_tmp67;for(0;_tmp6D != 0;_tmp6D=_tmp6D->tl){struct
_tuple2 _tmp6F;struct Cyc_Core_Opt*_tmp70;struct Cyc_Absyn_Tqual _tmp71;void*_tmp72;
struct _tuple2*_tmp6E=(struct _tuple2*)_tmp6D->hd;_tmp6F=*_tmp6E;_tmp70=_tmp6F.f1;
_tmp71=_tmp6F.f2;_tmp72=_tmp6F.f3;if(_tmp70 == 0){Cyc_Parse_err(_tag_arr("missing argument variable in function prototype",
sizeof(unsigned char),48),loc);_tmp6C=({struct Cyc_List_List*_tmp73=_cycalloc(
sizeof(*_tmp73));_tmp73->hd=({struct _tuple9*_tmp74=_cycalloc(sizeof(*_tmp74));
_tmp74->f1=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"?",sizeof(
unsigned char),2);_tmp74->f2=_tmp71;_tmp74->f3=_tmp72;_tmp74;});_tmp73->tl=
_tmp6C;_tmp73;});}else{_tmp6C=({struct Cyc_List_List*_tmp76=_cycalloc(sizeof(*
_tmp76));_tmp76->hd=({struct _tuple9*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->f1=(
struct _tagged_arr*)_tmp70->v;_tmp77->f2=_tmp71;_tmp77->f3=_tmp72;_tmp77;});
_tmp76->tl=_tmp6C;_tmp76;});}}}return({struct Cyc_Absyn_Fndecl*_tmp78=_cycalloc(
sizeof(*_tmp78));_tmp78->sc=(void*)sc;_tmp78->name=d->id;_tmp78->tvs=_tmp64;
_tmp78->is_inline=is_inline;_tmp78->effect=_tmp65;_tmp78->ret_type=(void*)_tmp66;
_tmp78->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp6C);_tmp78->c_varargs=_tmp68;_tmp78->cyc_varargs=_tmp69;_tmp78->rgn_po=
_tmp6A;_tmp78->body=body;_tmp78->cached_typ=0;_tmp78->param_vardecls=0;_tmp78->attributes=
Cyc_List_append(_tmp6B,_tmp5F);_tmp78;});}_LL3E:;_LL3F:({void*_tmp79[0]={};((int(*)(
struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(
loc,_tag_arr("declarator is not a function prototype",sizeof(unsigned char),39),
_tag_arr(_tmp79,sizeof(void*),0));});_LL3B:;}}}}}static unsigned char _tmp7A[52]="at most one type may appear within a type specifier";
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
last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp7E=(void*)ts->hd;void*_tmp7F;
struct Cyc_Position_Segment*_tmp80;struct Cyc_Position_Segment*_tmp81;struct Cyc_Position_Segment*
_tmp82;struct Cyc_Position_Segment*_tmp83;struct Cyc_Position_Segment*_tmp84;
struct Cyc_Absyn_Decl*_tmp85;_LL41: if(*((int*)_tmp7E)!= 4)goto _LL43;_tmp7F=(void*)((
struct Cyc_Parse_Type_spec_struct*)_tmp7E)->f1;_tmp80=((struct Cyc_Parse_Type_spec_struct*)
_tmp7E)->f2;_LL42: if(seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp80);last_loc=
_tmp80;seen_type=1;t=_tmp7F;goto _LL40;_LL43: if(*((int*)_tmp7E)!= 0)goto _LL45;
_tmp81=((struct Cyc_Parse_Signed_spec_struct*)_tmp7E)->f1;_LL44: if(seen_sign)Cyc_Parse_err(
Cyc_Parse_msg4,_tmp81);if(seen_type)Cyc_Parse_err(_tag_arr("signed qualifier must come before type",
sizeof(unsigned char),39),_tmp81);last_loc=_tmp81;seen_sign=1;sgn=(void*)0;goto
_LL40;_LL45: if(*((int*)_tmp7E)!= 1)goto _LL47;_tmp82=((struct Cyc_Parse_Unsigned_spec_struct*)
_tmp7E)->f1;_LL46: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp82);if(seen_type)
Cyc_Parse_err(_tag_arr("signed qualifier must come before type",sizeof(
unsigned char),39),_tmp82);last_loc=_tmp82;seen_sign=1;sgn=(void*)1;goto _LL40;
_LL47: if(*((int*)_tmp7E)!= 2)goto _LL49;_tmp83=((struct Cyc_Parse_Short_spec_struct*)
_tmp7E)->f1;_LL48: if(seen_size)Cyc_Parse_err(_tag_arr("integral size may appear only once within a type specifier",
sizeof(unsigned char),59),_tmp83);if(seen_type)Cyc_Parse_err(_tag_arr("short modifier must come before base type",
sizeof(unsigned char),42),_tmp83);last_loc=_tmp83;seen_size=1;sz=(void*)1;goto
_LL40;_LL49: if(*((int*)_tmp7E)!= 3)goto _LL4B;_tmp84=((struct Cyc_Parse_Long_spec_struct*)
_tmp7E)->f1;_LL4A: if(seen_type)Cyc_Parse_err(_tag_arr("long modifier must come before base type",
sizeof(unsigned char),41),_tmp84);if(seen_size){void*_tmp86=sz;_LL4E: if((int)
_tmp86 != 2)goto _LL50;_LL4F: sz=(void*)3;goto _LL4D;_LL50:;_LL51: Cyc_Parse_err(
_tag_arr("extra long in type specifier",sizeof(unsigned char),29),_tmp84);goto
_LL4D;_LL4D:;}last_loc=_tmp84;seen_size=1;goto _LL40;_LL4B: if(*((int*)_tmp7E)!= 5)
goto _LL40;_tmp85=((struct Cyc_Parse_Decl_spec_struct*)_tmp7E)->f1;_LL4C: last_loc=
_tmp85->loc;if(declopt == 0?!seen_type: 0){seen_type=1;{void*_tmp87=(void*)_tmp85->r;
struct Cyc_Absyn_Aggrdecl*_tmp88;struct Cyc_Absyn_Tuniondecl*_tmp89;struct Cyc_Absyn_Enumdecl*
_tmp8A;_LL53: if(*((int*)_tmp87)!= 4)goto _LL55;_tmp88=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp87)->f1;_LL54: {struct Cyc_List_List*_tmp8B=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmp88->tvs));t=(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({
struct Cyc_Absyn_AggrType_struct _tmp8D;_tmp8D.tag=10;_tmp8D.f1=({struct Cyc_Absyn_AggrInfo
_tmp8E;_tmp8E.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp90;_tmp90.tag=0;_tmp90.f1=(void*)((void*)_tmp88->kind);_tmp90.f2=_tmp88->name;
_tmp90;});_tmp8F;}));_tmp8E.targs=_tmp8B;_tmp8E;});_tmp8D;});_tmp8C;});if(_tmp88->fields
!= 0)declopt=({struct Cyc_Core_Opt*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->v=
_tmp85;_tmp91;});goto _LL52;}_LL55: if(*((int*)_tmp87)!= 5)goto _LL57;_tmp89=((
struct Cyc_Absyn_Tunion_d_struct*)_tmp87)->f1;_LL56: {struct Cyc_List_List*_tmp92=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmp89->tvs));t=(void*)({struct Cyc_Absyn_TunionType_struct*_tmp93=_cycalloc(
sizeof(*_tmp93));_tmp93[0]=({struct Cyc_Absyn_TunionType_struct _tmp94;_tmp94.tag=
2;_tmp94.f1=({struct Cyc_Absyn_TunionInfo _tmp95;_tmp95.tunion_info=(void*)((void*)({
struct Cyc_Absyn_KnownTunion_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[0]=({
struct Cyc_Absyn_KnownTunion_struct _tmp97;_tmp97.tag=1;_tmp97.f1=({struct Cyc_Absyn_Tuniondecl**
_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98[0]=_tmp89;_tmp98;});_tmp97;});_tmp96;}));
_tmp95.targs=_tmp92;_tmp95.rgn=(void*)((void*)2);_tmp95;});_tmp94;});_tmp93;});
if(_tmp89->fields != 0)declopt=({struct Cyc_Core_Opt*_tmp99=_cycalloc(sizeof(*
_tmp99));_tmp99->v=_tmp85;_tmp99;});goto _LL52;}_LL57: if(*((int*)_tmp87)!= 6)goto
_LL59;_tmp8A=((struct Cyc_Absyn_Enum_d_struct*)_tmp87)->f1;_LL58: t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({
struct Cyc_Absyn_EnumType_struct _tmp9B;_tmp9B.tag=12;_tmp9B.f1=_tmp8A->name;
_tmp9B.f2=0;_tmp9B;});_tmp9A;});declopt=({struct Cyc_Core_Opt*_tmp9C=_cycalloc(
sizeof(*_tmp9C));_tmp9C->v=_tmp85;_tmp9C;});goto _LL52;_LL59:;_LL5A:({void*_tmp9D[
0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Parse_abort)(_tmp85->loc,_tag_arr("bad declaration within type specifier",
sizeof(unsigned char),38),_tag_arr(_tmp9D,sizeof(void*),0));});_LL52:;}}else{Cyc_Parse_err(
Cyc_Parse_msg3,_tmp85->loc);}goto _LL40;_LL40:;}if(!seen_type){if(!seen_sign?!
seen_size: 0)({void*_tmp9E[0]={};Cyc_Tcutil_warn(last_loc,_tag_arr("missing type within specifier",
sizeof(unsigned char),30),_tag_arr(_tmp9E,sizeof(void*),0));});t=(void*)({struct
Cyc_Absyn_IntType_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({struct Cyc_Absyn_IntType_struct
_tmpA0;_tmpA0.tag=5;_tmpA0.f1=(void*)sgn;_tmpA0.f2=(void*)sz;_tmpA0;});_tmp9F;});}
else{if(seen_sign){void*_tmpA1=t;void*_tmpA2;_LL5C: if(_tmpA1 <= (void*)3?1:*((int*)
_tmpA1)!= 5)goto _LL5E;_tmpA2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpA1)->f2;
_LL5D: t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));
_tmpA3[0]=({struct Cyc_Absyn_IntType_struct _tmpA4;_tmpA4.tag=5;_tmpA4.f1=(void*)
sgn;_tmpA4.f2=(void*)_tmpA2;_tmpA4;});_tmpA3;});goto _LL5B;_LL5E:;_LL5F: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type",sizeof(unsigned char),40),
last_loc);goto _LL5B;_LL5B:;}if(seen_size){void*_tmpA5=t;void*_tmpA6;_LL61: if(
_tmpA5 <= (void*)3?1:*((int*)_tmpA5)!= 5)goto _LL63;_tmpA6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpA5)->f1;_LL62: t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpA7=_cycalloc(
sizeof(*_tmpA7));_tmpA7[0]=({struct Cyc_Absyn_IntType_struct _tmpA8;_tmpA8.tag=5;
_tmpA8.f1=(void*)_tmpA6;_tmpA8.f2=(void*)sz;_tmpA8;});_tmpA7;});goto _LL60;_LL63:
if(_tmpA5 <= (void*)3?1:*((int*)_tmpA5)!= 6)goto _LL65;_LL64: t=(void*)({struct Cyc_Absyn_DoubleType_struct*
_tmpA9=_cycalloc_atomic(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpAA;_tmpAA.tag=6;_tmpAA.f1=1;_tmpAA;});_tmpA9;});goto _LL60;_LL65:;_LL66: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type",sizeof(unsigned char),36),
last_loc);goto _LL60;_LL60:;}}return({struct _tuple4 _tmpAB;_tmpAB.f1=t;_tmpAB.f2=
declopt;_tmpAB;});}static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)
return 0;{struct Cyc_Parse_Declarator*_tmpAC=(struct Cyc_Parse_Declarator*)ds->hd;
struct _tuple1*_tmpAD=_tmpAC->id;struct Cyc_Absyn_Tqual _tmpAF;void*_tmpB0;struct
Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB2;struct _tuple5 _tmpAE=Cyc_Parse_apply_tms(
tq,t,shared_atts,_tmpAC->tms);_tmpAF=_tmpAE.f1;_tmpB0=_tmpAE.f2;_tmpB1=_tmpAE.f3;
_tmpB2=_tmpAE.f4;return({struct Cyc_List_List*_tmpB3=_cycalloc(sizeof(*_tmpB3));
_tmpB3->hd=({struct _tuple6*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->f1=_tmpAD;
_tmpB4->f2=_tmpAF;_tmpB4->f3=_tmpB0;_tmpB4->f4=_tmpB1;_tmpB4->f5=_tmpB2;_tmpB4;});
_tmpB3->tl=Cyc_Parse_apply_tmss(_tmpAF,Cyc_Tcutil_copy_type(t),ds->tl,
shared_atts);_tmpB3;});}}static struct _tuple5 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){if(tms == 0)return({
struct _tuple5 _tmpB5;_tmpB5.f1=tq;_tmpB5.f2=t;_tmpB5.f3=0;_tmpB5.f4=atts;_tmpB5;});{
void*_tmpB6=(void*)tms->hd;struct Cyc_Absyn_Exp*_tmpB7;void*_tmpB8;struct Cyc_List_List*
_tmpB9;struct Cyc_Position_Segment*_tmpBA;void*_tmpBB;void*_tmpBC;struct Cyc_Absyn_Tqual
_tmpBD;struct Cyc_Position_Segment*_tmpBE;struct Cyc_List_List*_tmpBF;_LL68: if((
int)_tmpB6 != 0)goto _LL6A;_LL69: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),(
void*)({struct Cyc_Absyn_ArrayType_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));
_tmpC0[0]=({struct Cyc_Absyn_ArrayType_struct _tmpC1;_tmpC1.tag=7;_tmpC1.f1=(void*)
t;_tmpC1.f2=tq;_tmpC1.f3=0;_tmpC1;});_tmpC0;}),atts,tms->tl);_LL6A: if(_tmpB6 <= (
void*)1?1:*((int*)_tmpB6)!= 0)goto _LL6C;_tmpB7=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpB6)->f1;_LL6B: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({
struct Cyc_Absyn_ArrayType_struct _tmpC3;_tmpC3.tag=7;_tmpC3.f1=(void*)t;_tmpC3.f2=
tq;_tmpC3.f3=(struct Cyc_Absyn_Exp*)_tmpB7;_tmpC3;});_tmpC2;}),atts,tms->tl);
_LL6C: if(_tmpB6 <= (void*)1?1:*((int*)_tmpB6)!= 2)goto _LL6E;_tmpB8=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmpB6)->f1;_LL6D: {void*_tmpC4=_tmpB8;
struct Cyc_List_List*_tmpC5;int _tmpC6;struct Cyc_Absyn_VarargInfo*_tmpC7;struct Cyc_Core_Opt*
_tmpC8;struct Cyc_List_List*_tmpC9;struct Cyc_Position_Segment*_tmpCA;_LL75: if(*((
int*)_tmpC4)!= 1)goto _LL77;_tmpC5=((struct Cyc_Absyn_WithTypes_struct*)_tmpC4)->f1;
_tmpC6=((struct Cyc_Absyn_WithTypes_struct*)_tmpC4)->f2;_tmpC7=((struct Cyc_Absyn_WithTypes_struct*)
_tmpC4)->f3;_tmpC8=((struct Cyc_Absyn_WithTypes_struct*)_tmpC4)->f4;_tmpC9=((
struct Cyc_Absyn_WithTypes_struct*)_tmpC4)->f5;_LL76: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->hd=(void*)((
void*)as->hd);_tmpCB->tl=fn_atts;_tmpCB;});else{new_atts=({struct Cyc_List_List*
_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->hd=(void*)((void*)as->hd);_tmpCC->tl=
new_atts;_tmpCC;});}}}if(tms->tl != 0){void*_tmpCD=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpCE;_LL7A: if(_tmpCD <= (void*)1?
1:*((int*)_tmpCD)!= 3)goto _LL7C;_tmpCE=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCD)->f1;_LL7B: typvars=_tmpCE;tms=tms->tl;goto _LL79;_LL7C:;_LL7D: goto _LL79;
_LL79:;}if(((((!_tmpC6?_tmpC7 == 0: 0)?_tmpC5 != 0: 0)?_tmpC5->tl == 0: 0)?(*((struct
_tuple2*)_tmpC5->hd)).f1 == 0: 0)?(*((struct _tuple2*)_tmpC5->hd)).f3 == (void*)0: 0)
_tmpC5=0;t=Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpC5);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),
Cyc_Absyn_function_typ(typvars,_tmpC8,t,_tmpC5,_tmpC6,_tmpC7,_tmpC9,fn_atts),
new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL77: if(*((int*)_tmpC4)
!= 0)goto _LL74;_tmpCA=((struct Cyc_Absyn_NoTypes_struct*)_tmpC4)->f2;_LL78:({void*
_tmpCF[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Parse_abort)(_tmpCA,_tag_arr("function declaration without parameter types",
sizeof(unsigned char),45),_tag_arr(_tmpCF,sizeof(void*),0));});_LL74:;}_LL6E: if(
_tmpB6 <= (void*)1?1:*((int*)_tmpB6)!= 3)goto _LL70;_tmpB9=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpB6)->f1;_tmpBA=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpB6)->f2;_LL6F:
if(tms->tl == 0)return({struct _tuple5 _tmpD0;_tmpD0.f1=tq;_tmpD0.f2=t;_tmpD0.f3=
_tmpB9;_tmpD0.f4=atts;_tmpD0;});({void*_tmpD1[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmpBA,_tag_arr("type parameters must appear before function arguments in declarator",
sizeof(unsigned char),68),_tag_arr(_tmpD1,sizeof(void*),0));});_LL70: if(_tmpB6 <= (
void*)1?1:*((int*)_tmpB6)!= 1)goto _LL72;_tmpBB=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpB6)->f1;_tmpBC=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpB6)->f2;
_tmpBD=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpB6)->f3;_LL71: {void*_tmpD2=
_tmpBB;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD4;_LL7F: if(_tmpD2 <= (
void*)1?1:*((int*)_tmpD2)!= 0)goto _LL81;_tmpD3=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmpD2)->f1;_LL80: return Cyc_Parse_apply_tms(_tmpBD,Cyc_Absyn_atb_typ(t,_tmpBC,tq,(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[
0]=({struct Cyc_Absyn_Upper_b_struct _tmpD6;_tmpD6.tag=0;_tmpD6.f1=_tmpD3;_tmpD6;});
_tmpD5;})),atts,tms->tl);_LL81: if(_tmpD2 <= (void*)1?1:*((int*)_tmpD2)!= 1)goto
_LL83;_tmpD4=((struct Cyc_Absyn_Nullable_ps_struct*)_tmpD2)->f1;_LL82: return Cyc_Parse_apply_tms(
_tmpBD,Cyc_Absyn_starb_typ(t,_tmpBC,tq,(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7[0]=({struct Cyc_Absyn_Upper_b_struct
_tmpD8;_tmpD8.tag=0;_tmpD8.f1=_tmpD4;_tmpD8;});_tmpD7;})),atts,tms->tl);_LL83:
if((int)_tmpD2 != 0)goto _LL7E;_LL84: return Cyc_Parse_apply_tms(_tmpBD,Cyc_Absyn_tagged_typ(
t,_tmpBC,tq),atts,tms->tl);_LL7E:;}_LL72: if(_tmpB6 <= (void*)1?1:*((int*)_tmpB6)
!= 4)goto _LL67;_tmpBE=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpB6)->f1;
_tmpBF=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpB6)->f2;_LL73: return Cyc_Parse_apply_tms(
tq,t,Cyc_List_append(atts,_tmpBF),tms->tl);_LL67:;}}void*Cyc_Parse_speclist2typ(
struct Cyc_List_List*tss,struct Cyc_Position_Segment*loc){void*_tmpDA;struct Cyc_Core_Opt*
_tmpDB;struct _tuple4 _tmpD9=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmpDA=
_tmpD9.f1;_tmpDB=_tmpD9.f2;if(_tmpDB != 0)({void*_tmpDC[0]={};Cyc_Tcutil_warn(loc,
_tag_arr("ignoring nested type declaration(s) in specifier list",sizeof(
unsigned char),54),_tag_arr(_tmpDC,sizeof(void*),0));});return _tmpDA;}static
struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*loc,
struct _tuple6*t){struct _tuple6 _tmpDE;struct _tuple1*_tmpDF;struct Cyc_Absyn_Tqual
_tmpE0;void*_tmpE1;struct Cyc_List_List*_tmpE2;struct Cyc_List_List*_tmpE3;struct
_tuple6*_tmpDD=t;_tmpDE=*_tmpDD;_tmpDF=_tmpDE.f1;_tmpE0=_tmpDE.f2;_tmpE1=_tmpDE.f3;
_tmpE2=_tmpDE.f4;_tmpE3=_tmpDE.f5;Cyc_Lex_register_typedef(_tmpDF);if(_tmpE3 != 0)
Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpE5;_tmpE5.tag=
0;_tmpE5.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((void*)_tmpE3->hd);{
void*_tmpE4[1]={& _tmpE5};Cyc_Std_aprintf(_tag_arr("bad attribute %s within typedef",
sizeof(unsigned char),32),_tag_arr(_tmpE4,sizeof(void*),1));}}),loc);return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Typedef_d_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));
_tmpE6[0]=({struct Cyc_Absyn_Typedef_d_struct _tmpE7;_tmpE7.tag=7;_tmpE7.f1=({
struct Cyc_Absyn_Typedefdecl*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->name=
_tmpDF;_tmpE8->tvs=_tmpE2;_tmpE8->defn=(void*)_tmpE1;_tmpE8;});_tmpE7;});_tmpE6;}),
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
_tmpEC;struct Cyc_Absyn_Tqual _tmpED;struct Cyc_List_List*_tmpEE;struct Cyc_List_List*
_tmpEF;struct Cyc_Parse_Declaration_spec*_tmpEB=ds;_tmpEC=*_tmpEB;_tmpED=_tmpEC.tq;
_tmpEE=_tmpEC.type_specs;_tmpEF=_tmpEC.attributes;if(ds->is_inline)Cyc_Parse_err(
_tag_arr("inline is allowed only on function definitions",sizeof(unsigned char),
47),loc);if(_tmpEE == 0){Cyc_Parse_err(_tag_arr("missing type specifiers in declaration",
sizeof(unsigned char),39),loc);return 0;}{void*s=(void*)2;int istypedef=0;if(ds->sc
!= 0){void*_tmpF0=(void*)((struct Cyc_Core_Opt*)_check_null(ds->sc))->v;_LL86: if((
int)_tmpF0 != 0)goto _LL88;_LL87: istypedef=1;goto _LL85;_LL88: if((int)_tmpF0 != 1)
goto _LL8A;_LL89: s=(void*)3;goto _LL85;_LL8A: if((int)_tmpF0 != 2)goto _LL8C;_LL8B: s=(
void*)4;goto _LL85;_LL8C: if((int)_tmpF0 != 3)goto _LL8E;_LL8D: s=(void*)0;goto _LL85;
_LL8E: if((int)_tmpF0 != 4)goto _LL90;_LL8F: s=(void*)2;goto _LL85;_LL90: if((int)
_tmpF0 != 5)goto _LL92;_LL91: s=(void*)2;goto _LL85;_LL92: if((int)_tmpF0 != 6)goto
_LL85;_LL93: s=(void*)1;goto _LL85;_LL85:;}{struct Cyc_List_List*_tmpF2;struct Cyc_List_List*
_tmpF3;struct _tuple0 _tmpF1=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
ids);_tmpF2=_tmpF1.f1;_tmpF3=_tmpF1.f2;{int exps_empty=1;{struct Cyc_List_List*es=
_tmpF3;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){exps_empty=0;
break;}}}{struct _tuple4 _tmpF4=Cyc_Parse_collapse_type_specifiers(_tmpEE,loc);if(
_tmpF2 == 0){void*_tmpF6;struct Cyc_Core_Opt*_tmpF7;struct _tuple4 _tmpF5=_tmpF4;
_tmpF6=_tmpF5.f1;_tmpF7=_tmpF5.f2;if(_tmpF7 != 0){struct Cyc_Absyn_Decl*d=(struct
Cyc_Absyn_Decl*)_tmpF7->v;{void*_tmpF8=(void*)d->r;struct Cyc_Absyn_Enumdecl*
_tmpF9;struct Cyc_Absyn_Aggrdecl*_tmpFA;struct Cyc_Absyn_Tuniondecl*_tmpFB;_LL95:
if(*((int*)_tmpF8)!= 6)goto _LL97;_tmpF9=((struct Cyc_Absyn_Enum_d_struct*)_tmpF8)->f1;
_LL96:(void*)(_tmpF9->sc=(void*)s);if(_tmpEF != 0)Cyc_Parse_err(_tag_arr("bad attributes on enum",
sizeof(unsigned char),23),loc);goto _LL94;_LL97: if(*((int*)_tmpF8)!= 4)goto _LL99;
_tmpFA=((struct Cyc_Absyn_Aggr_d_struct*)_tmpF8)->f1;_LL98:(void*)(_tmpFA->sc=(
void*)s);_tmpFA->attributes=_tmpEF;goto _LL94;_LL99: if(*((int*)_tmpF8)!= 5)goto
_LL9B;_tmpFB=((struct Cyc_Absyn_Tunion_d_struct*)_tmpF8)->f1;_LL9A:(void*)(_tmpFB->sc=(
void*)s);if(_tmpEF != 0)Cyc_Parse_err(_tag_arr("bad attributes on tunion",sizeof(
unsigned char),25),loc);goto _LL94;_LL9B:;_LL9C: Cyc_Parse_err(_tag_arr("bad declaration",
sizeof(unsigned char),16),loc);return 0;_LL94:;}return({struct Cyc_List_List*
_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->hd=d;_tmpFC->tl=0;_tmpFC;});}else{void*
_tmpFD=_tmpF6;struct Cyc_Absyn_AggrInfo _tmpFE;void*_tmpFF;void*_tmp100;struct
_tuple1*_tmp101;struct Cyc_List_List*_tmp102;struct Cyc_Absyn_TunionInfo _tmp103;
void*_tmp104;struct Cyc_Absyn_Tuniondecl**_tmp105;struct Cyc_Absyn_TunionInfo
_tmp106;void*_tmp107;struct Cyc_Absyn_UnknownTunionInfo _tmp108;struct _tuple1*
_tmp109;int _tmp10A;struct Cyc_List_List*_tmp10B;struct _tuple1*_tmp10C;struct Cyc_List_List*
_tmp10D;_LL9E: if(_tmpFD <= (void*)3?1:*((int*)_tmpFD)!= 10)goto _LLA0;_tmpFE=((
struct Cyc_Absyn_AggrType_struct*)_tmpFD)->f1;_tmpFF=(void*)_tmpFE.aggr_info;if(*((
int*)_tmpFF)!= 0)goto _LLA0;_tmp100=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmpFF)->f1;_tmp101=((struct Cyc_Absyn_UnknownAggr_struct*)_tmpFF)->f2;_tmp102=
_tmpFE.targs;_LL9F: {struct Cyc_List_List*_tmp10E=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp102);
struct Cyc_Absyn_Aggrdecl*_tmp10F=({struct Cyc_Absyn_Aggrdecl*_tmp113=_cycalloc(
sizeof(*_tmp113));_tmp113->kind=(void*)_tmp100;_tmp113->sc=(void*)s;_tmp113->name=
_tmp101;_tmp113->tvs=_tmp10E;_tmp113->exist_vars=0;_tmp113->fields=0;_tmp113->attributes=
0;_tmp113;});if(_tmpEF != 0)Cyc_Parse_err(_tag_arr("bad attributes on type declaration",
sizeof(unsigned char),35),loc);return({struct Cyc_List_List*_tmp110=_cycalloc(
sizeof(*_tmp110));_tmp110->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp112;_tmp112.tag=4;_tmp112.f1=_tmp10F;_tmp112;});_tmp111;}),loc);_tmp110->tl=
0;_tmp110;});}_LLA0: if(_tmpFD <= (void*)3?1:*((int*)_tmpFD)!= 2)goto _LLA2;_tmp103=((
struct Cyc_Absyn_TunionType_struct*)_tmpFD)->f1;_tmp104=(void*)_tmp103.tunion_info;
if(*((int*)_tmp104)!= 1)goto _LLA2;_tmp105=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp104)->f1;_LLA1: if(_tmpEF != 0)Cyc_Parse_err(_tag_arr("bad attributes on tunion",
sizeof(unsigned char),25),loc);return({struct Cyc_List_List*_tmp114=_cycalloc(
sizeof(*_tmp114));_tmp114->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp116;_tmp116.tag=5;_tmp116.f1=*_tmp105;_tmp116;});_tmp115;}),loc);_tmp114->tl=
0;_tmp114;});_LLA2: if(_tmpFD <= (void*)3?1:*((int*)_tmpFD)!= 2)goto _LLA4;_tmp106=((
struct Cyc_Absyn_TunionType_struct*)_tmpFD)->f1;_tmp107=(void*)_tmp106.tunion_info;
if(*((int*)_tmp107)!= 0)goto _LLA4;_tmp108=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp107)->f1;_tmp109=_tmp108.name;_tmp10A=_tmp108.is_xtunion;_tmp10B=_tmp106.targs;
_LLA3: {struct Cyc_List_List*_tmp117=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp10B);struct Cyc_Absyn_Decl*_tmp118=
Cyc_Absyn_tunion_decl(s,_tmp109,_tmp117,0,_tmp10A,loc);if(_tmpEF != 0)Cyc_Parse_err(
_tag_arr("bad attributes on tunion",sizeof(unsigned char),25),loc);return({
struct Cyc_List_List*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->hd=_tmp118;
_tmp119->tl=0;_tmp119;});}_LLA4: if(_tmpFD <= (void*)3?1:*((int*)_tmpFD)!= 12)goto
_LLA6;_tmp10C=((struct Cyc_Absyn_EnumType_struct*)_tmpFD)->f1;_LLA5: {struct Cyc_Absyn_Enumdecl*
_tmp11A=({struct Cyc_Absyn_Enumdecl*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->sc=(
void*)s;_tmp11F->name=_tmp10C;_tmp11F->fields=0;_tmp11F;});if(_tmpEF != 0)Cyc_Parse_err(
_tag_arr("bad attributes on enum",sizeof(unsigned char),23),loc);return({struct
Cyc_List_List*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->hd=({struct Cyc_Absyn_Decl*
_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp11E;_tmp11E.tag=6;_tmp11E.f1=_tmp11A;_tmp11E;});_tmp11D;}));_tmp11C->loc=loc;
_tmp11C;});_tmp11B->tl=0;_tmp11B;});}_LLA6: if(_tmpFD <= (void*)3?1:*((int*)_tmpFD)
!= 13)goto _LLA8;_tmp10D=((struct Cyc_Absyn_AnonEnumType_struct*)_tmpFD)->f1;_LLA7: {
struct Cyc_Absyn_Enumdecl*_tmp120=({struct Cyc_Absyn_Enumdecl*_tmp125=_cycalloc(
sizeof(*_tmp125));_tmp125->sc=(void*)s;_tmp125->name=Cyc_Parse_gensym_enum();
_tmp125->fields=({struct Cyc_Core_Opt*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->v=
_tmp10D;_tmp126;});_tmp125;});if(_tmpEF != 0)Cyc_Parse_err(_tag_arr("bad attributes on enum",
sizeof(unsigned char),23),loc);return({struct Cyc_List_List*_tmp121=_cycalloc(
sizeof(*_tmp121));_tmp121->hd=({struct Cyc_Absyn_Decl*_tmp122=_cycalloc(sizeof(*
_tmp122));_tmp122->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp123=
_cycalloc(sizeof(*_tmp123));_tmp123[0]=({struct Cyc_Absyn_Enum_d_struct _tmp124;
_tmp124.tag=6;_tmp124.f1=_tmp120;_tmp124;});_tmp123;}));_tmp122->loc=loc;_tmp122;});
_tmp121->tl=0;_tmp121;});}_LLA8:;_LLA9: Cyc_Parse_err(_tag_arr("missing declarator",
sizeof(unsigned char),19),loc);return 0;_LL9D:;}}else{void*t=_tmpF4.f1;struct Cyc_List_List*
_tmp127=Cyc_Parse_apply_tmss(_tmpED,t,_tmpF2,_tmpEF);if(istypedef){if(!
exps_empty)Cyc_Parse_err(_tag_arr("initializer in typedef declaration",sizeof(
unsigned char),35),loc);{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp127);if(_tmpF4.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(_tmpF4.f2)->v;{
void*_tmp128=(void*)d->r;struct Cyc_Absyn_Aggrdecl*_tmp129;struct Cyc_Absyn_Tuniondecl*
_tmp12A;struct Cyc_Absyn_Enumdecl*_tmp12B;_LLAB: if(*((int*)_tmp128)!= 4)goto _LLAD;
_tmp129=((struct Cyc_Absyn_Aggr_d_struct*)_tmp128)->f1;_LLAC:(void*)(_tmp129->sc=(
void*)s);_tmp129->attributes=_tmpEF;_tmpEF=0;goto _LLAA;_LLAD: if(*((int*)_tmp128)
!= 5)goto _LLAF;_tmp12A=((struct Cyc_Absyn_Tunion_d_struct*)_tmp128)->f1;_LLAE:(
void*)(_tmp12A->sc=(void*)s);goto _LLAA;_LLAF: if(*((int*)_tmp128)!= 6)goto _LLB1;
_tmp12B=((struct Cyc_Absyn_Enum_d_struct*)_tmp128)->f1;_LLB0:(void*)(_tmp12B->sc=(
void*)s);goto _LLAA;_LLB1:;_LLB2: Cyc_Parse_err(_tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof(unsigned char),70),loc);goto _LLAA;_LLAA:;}decls=({struct Cyc_List_List*
_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->hd=d;_tmp12C->tl=decls;_tmp12C;});}
if(_tmpEF != 0)Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp12E;_tmp12E.tag=0;_tmp12E.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmpEF->hd);{void*_tmp12D[1]={& _tmp12E};Cyc_Std_aprintf(_tag_arr("bad attribute %s in typedef",
sizeof(unsigned char),28),_tag_arr(_tmp12D,sizeof(void*),1));}}),loc);return
decls;}}else{if(_tmpF4.f2 != 0)Cyc_Parse_unimp(_tag_arr("nested type declaration within declarator",
sizeof(unsigned char),42),loc);{struct Cyc_List_List*decls=0;{struct Cyc_List_List*
_tmp12F=_tmp127;for(0;_tmp12F != 0;(_tmp12F=_tmp12F->tl,_tmpF3=_tmpF3->tl)){
struct _tuple6 _tmp131;struct _tuple1*_tmp132;struct Cyc_Absyn_Tqual _tmp133;void*
_tmp134;struct Cyc_List_List*_tmp135;struct Cyc_List_List*_tmp136;struct _tuple6*
_tmp130=(struct _tuple6*)_tmp12F->hd;_tmp131=*_tmp130;_tmp132=_tmp131.f1;_tmp133=
_tmp131.f2;_tmp134=_tmp131.f3;_tmp135=_tmp131.f4;_tmp136=_tmp131.f5;if(_tmp135 != 
0)({void*_tmp137[0]={};Cyc_Tcutil_warn(loc,_tag_arr("bad type params, ignoring",
sizeof(unsigned char),26),_tag_arr(_tmp137,sizeof(void*),0));});if(_tmpF3 == 0)({
void*_tmp138[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Parse_abort)(loc,_tag_arr("unexpected NULL in parse!",
sizeof(unsigned char),26),_tag_arr(_tmp138,sizeof(void*),0));});{struct Cyc_Absyn_Exp*
_tmp139=(struct Cyc_Absyn_Exp*)_tmpF3->hd;struct Cyc_Absyn_Vardecl*_tmp13A=Cyc_Absyn_new_vardecl(
_tmp132,_tmp134,_tmp139);_tmp13A->tq=_tmp133;(void*)(_tmp13A->sc=(void*)s);
_tmp13A->attributes=_tmp136;{struct Cyc_Absyn_Decl*_tmp13B=({struct Cyc_Absyn_Decl*
_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E[0]=({struct Cyc_Absyn_Var_d_struct
_tmp13F;_tmp13F.tag=0;_tmp13F.f1=_tmp13A;_tmp13F;});_tmp13E;}));_tmp13D->loc=loc;
_tmp13D;});decls=({struct Cyc_List_List*_tmp13C=_cycalloc(sizeof(*_tmp13C));
_tmp13C->hd=_tmp13B;_tmp13C->tl=decls;_tmp13C;});}}}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);}}}}}}}}static void*Cyc_Parse_id_to_kind(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){if(Cyc_Std_strlen(s)== 1)
switch(*((const unsigned char*)_check_unknown_subscript(s,sizeof(unsigned char),0))){
case 'A': _LLB3: return(void*)0;case 'M': _LLB4: return(void*)1;case 'B': _LLB5: return(
void*)2;case 'R': _LLB6: return(void*)3;case 'E': _LLB7: return(void*)4;default: _LLB8:
break;}Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp141;
_tmp141.tag=0;_tmp141.f1=(struct _tagged_arr)s;{void*_tmp140[1]={& _tmp141};Cyc_Std_aprintf(
_tag_arr("bad kind: %s",sizeof(unsigned char),13),_tag_arr(_tmp140,sizeof(void*),
1));}}),loc);return(void*)2;}static struct Cyc_List_List*Cyc_Parse_attopt_to_tms(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
if(atts == 0)return tms;else{return({struct Cyc_List_List*_tmp142=_cycalloc(sizeof(*
_tmp142));_tmp142->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
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
struct _tuple12*_tmp148;_LLBB: if(*((void**)_tmp147)!= Cyc_Int_tok)goto _LLBD;
_tmp148=((struct Cyc_Int_tok_struct*)_tmp147)->f1;_LLBC: yyzzz=_tmp148;goto _LLBA;
_LLBD:;_LLBE:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLBA:;}return yyzzz;}static
unsigned char _tmp14A[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp14A,_tmp14A,_tmp14A + 5}};unsigned char Cyc_yyget_Char_tok(
void*yy1){unsigned char yyzzz;{void*_tmp14B=yy1;unsigned char _tmp14C;_LLC0: if(*((
void**)_tmp14B)!= Cyc_Char_tok)goto _LLC2;_tmp14C=((struct Cyc_Char_tok_struct*)
_tmp14B)->f1;_LLC1: yyzzz=_tmp14C;goto _LLBF;_LLC2:;_LLC3:(int)_throw((void*)& Cyc_yyfail_Char_tok);
_LLBF:;}return yyzzz;}static unsigned char _tmp14E[9]="string_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp14E,_tmp14E,_tmp14E + 9}};struct
_tagged_arr Cyc_yyget_String_tok(void*yy1){struct _tagged_arr yyzzz;{void*_tmp14F=
yy1;struct _tagged_arr _tmp150;_LLC5: if(*((void**)_tmp14F)!= Cyc_String_tok)goto
_LLC7;_tmp150=((struct Cyc_String_tok_struct*)_tmp14F)->f1;_LLC6: yyzzz=_tmp150;
goto _LLC4;_LLC7:;_LLC8:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLC4:;}return
yyzzz;}static unsigned char _tmp152[20]="tunion Pointer_Sort";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={Cyc_Core_Failure,{_tmp152,_tmp152,_tmp152 + 20}};
void*Cyc_yyget_Pointer_Sort_tok(void*yy1){void*yyzzz;{void*_tmp153=yy1;void*
_tmp154;_LLCA: if(*((void**)_tmp153)!= Cyc_Pointer_Sort_tok)goto _LLCC;_tmp154=(
void*)((struct Cyc_Pointer_Sort_tok_struct*)_tmp153)->f1;_LLCB: yyzzz=_tmp154;goto
_LLC9;_LLCC:;_LLCD:(int)_throw((void*)& Cyc_yyfail_Pointer_Sort_tok);_LLC9:;}
return yyzzz;}static unsigned char _tmp156[6]="exp_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp156,_tmp156,_tmp156 + 6}};struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok(void*yy1){struct Cyc_Absyn_Exp*yyzzz;{void*_tmp157=yy1;struct
Cyc_Absyn_Exp*_tmp158;_LLCF: if(*((void**)_tmp157)!= Cyc_Exp_tok)goto _LLD1;
_tmp158=((struct Cyc_Exp_tok_struct*)_tmp157)->f1;_LLD0: yyzzz=_tmp158;goto _LLCE;
_LLD1:;_LLD2:(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LLCE:;}return yyzzz;}static
unsigned char _tmp15A[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{_tmp15A,_tmp15A,_tmp15A + 14}};struct Cyc_List_List*Cyc_yyget_ExpList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp15B=yy1;struct Cyc_List_List*
_tmp15C;_LLD4: if(*((void**)_tmp15B)!= Cyc_ExpList_tok)goto _LLD6;_tmp15C=((struct
Cyc_ExpList_tok_struct*)_tmp15B)->f1;_LLD5: yyzzz=_tmp15C;goto _LLD3;_LLD6:;_LLD7:(
int)_throw((void*)& Cyc_yyfail_ExpList_tok);_LLD3:;}return yyzzz;}static
unsigned char _tmp15E[39]="list_t<$(list_t<designator_t>,exp_t)@>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{_tmp15E,
_tmp15E,_tmp15E + 39}};struct Cyc_List_List*Cyc_yyget_InitializerList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp15F=yy1;struct Cyc_List_List*_tmp160;_LLD9:
if(*((void**)_tmp15F)!= Cyc_InitializerList_tok)goto _LLDB;_tmp160=((struct Cyc_InitializerList_tok_struct*)
_tmp15F)->f1;_LLDA: yyzzz=_tmp160;goto _LLD8;_LLDB:;_LLDC:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);
_LLD8:;}return yyzzz;}static unsigned char _tmp162[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={Cyc_Core_Failure,{_tmp162,_tmp162,_tmp162 + 9}};void*Cyc_yyget_Primop_tok(
void*yy1){void*yyzzz;{void*_tmp163=yy1;void*_tmp164;_LLDE: if(*((void**)_tmp163)
!= Cyc_Primop_tok)goto _LLE0;_tmp164=(void*)((struct Cyc_Primop_tok_struct*)
_tmp163)->f1;_LLDF: yyzzz=_tmp164;goto _LLDD;_LLE0:;_LLE1:(int)_throw((void*)& Cyc_yyfail_Primop_tok);
_LLDD:;}return yyzzz;}static unsigned char _tmp166[16]="opt_t<primop_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp166,
_tmp166,_tmp166 + 16}};struct Cyc_Core_Opt*Cyc_yyget_Primopopt_tok(void*yy1){
struct Cyc_Core_Opt*yyzzz;{void*_tmp167=yy1;struct Cyc_Core_Opt*_tmp168;_LLE3: if(*((
void**)_tmp167)!= Cyc_Primopopt_tok)goto _LLE5;_tmp168=((struct Cyc_Primopopt_tok_struct*)
_tmp167)->f1;_LLE4: yyzzz=_tmp168;goto _LLE2;_LLE5:;_LLE6:(int)_throw((void*)& Cyc_yyfail_Primopopt_tok);
_LLE2:;}return yyzzz;}static unsigned char _tmp16A[7]="qvar_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp16A,_tmp16A,_tmp16A + 7}};struct
_tuple1*Cyc_yyget_QualId_tok(void*yy1){struct _tuple1*yyzzz;{void*_tmp16B=yy1;
struct _tuple1*_tmp16C;_LLE8: if(*((void**)_tmp16B)!= Cyc_QualId_tok)goto _LLEA;
_tmp16C=((struct Cyc_QualId_tok_struct*)_tmp16B)->f1;_LLE9: yyzzz=_tmp16C;goto
_LLE7;_LLEA:;_LLEB:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LLE7:;}return
yyzzz;}static unsigned char _tmp16E[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp16E,_tmp16E,_tmp16E + 7}};struct Cyc_Absyn_Stmt*
Cyc_yyget_Stmt_tok(void*yy1){struct Cyc_Absyn_Stmt*yyzzz;{void*_tmp16F=yy1;struct
Cyc_Absyn_Stmt*_tmp170;_LLED: if(*((void**)_tmp16F)!= Cyc_Stmt_tok)goto _LLEF;
_tmp170=((struct Cyc_Stmt_tok_struct*)_tmp16F)->f1;_LLEE: yyzzz=_tmp170;goto _LLEC;
_LLEF:;_LLF0:(int)_throw((void*)& Cyc_yyfail_Stmt_tok);_LLEC:;}return yyzzz;}
static unsigned char _tmp172[24]="list_t<switch_clause_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={Cyc_Core_Failure,{_tmp172,_tmp172,_tmp172 + 24}};
struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp173=yy1;struct Cyc_List_List*_tmp174;_LLF2: if(*((void**)_tmp173)
!= Cyc_SwitchClauseList_tok)goto _LLF4;_tmp174=((struct Cyc_SwitchClauseList_tok_struct*)
_tmp173)->f1;_LLF3: yyzzz=_tmp174;goto _LLF1;_LLF4:;_LLF5:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);
_LLF1:;}return yyzzz;}static unsigned char _tmp176[25]="list_t<switchC_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={Cyc_Core_Failure,{
_tmp176,_tmp176,_tmp176 + 25}};struct Cyc_List_List*Cyc_yyget_SwitchCClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp177=yy1;struct Cyc_List_List*
_tmp178;_LLF7: if(*((void**)_tmp177)!= Cyc_SwitchCClauseList_tok)goto _LLF9;
_tmp178=((struct Cyc_SwitchCClauseList_tok_struct*)_tmp177)->f1;_LLF8: yyzzz=
_tmp178;goto _LLF6;_LLF9:;_LLFA:(int)_throw((void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LLF6:;}return yyzzz;}static unsigned char _tmp17A[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp17A,_tmp17A,_tmp17A + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp17B=yy1;
struct Cyc_Absyn_Pat*_tmp17C;_LLFC: if(*((void**)_tmp17B)!= Cyc_Pattern_tok)goto
_LLFE;_tmp17C=((struct Cyc_Pattern_tok_struct*)_tmp17B)->f1;_LLFD: yyzzz=_tmp17C;
goto _LLFB;_LLFE:;_LLFF:(int)_throw((void*)& Cyc_yyfail_Pattern_tok);_LLFB:;}
return yyzzz;}static unsigned char _tmp17E[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={Cyc_Core_Failure,{_tmp17E,_tmp17E,_tmp17E + 14}};
struct Cyc_List_List*Cyc_yyget_PatternList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp17F=yy1;struct Cyc_List_List*_tmp180;_LL101: if(*((void**)_tmp17F)
!= Cyc_PatternList_tok)goto _LL103;_tmp180=((struct Cyc_PatternList_tok_struct*)
_tmp17F)->f1;_LL102: yyzzz=_tmp180;goto _LL100;_LL103:;_LL104:(int)_throw((void*)&
Cyc_yyfail_PatternList_tok);_LL100:;}return yyzzz;}static unsigned char _tmp182[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{
_tmp182,_tmp182,_tmp182 + 31}};struct _tuple10*Cyc_yyget_FieldPattern_tok(void*yy1){
struct _tuple10*yyzzz;{void*_tmp183=yy1;struct _tuple10*_tmp184;_LL106: if(*((void**)
_tmp183)!= Cyc_FieldPattern_tok)goto _LL108;_tmp184=((struct Cyc_FieldPattern_tok_struct*)
_tmp183)->f1;_LL107: yyzzz=_tmp184;goto _LL105;_LL108:;_LL109:(int)_throw((void*)&
Cyc_yyfail_FieldPattern_tok);_LL105:;}return yyzzz;}static unsigned char _tmp186[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp186,_tmp186,_tmp186 + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp187=yy1;struct Cyc_List_List*
_tmp188;_LL10B: if(*((void**)_tmp187)!= Cyc_FieldPatternList_tok)goto _LL10D;
_tmp188=((struct Cyc_FieldPatternList_tok_struct*)_tmp187)->f1;_LL10C: yyzzz=
_tmp188;goto _LL10A;_LL10D:;_LL10E:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL10A:;}return yyzzz;}static unsigned char _tmp18A[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={Cyc_Core_Failure,{_tmp18A,_tmp18A,_tmp18A + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok(void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp18B=yy1;
struct Cyc_Absyn_Fndecl*_tmp18C;_LL110: if(*((void**)_tmp18B)!= Cyc_FnDecl_tok)
goto _LL112;_tmp18C=((struct Cyc_FnDecl_tok_struct*)_tmp18B)->f1;_LL111: yyzzz=
_tmp18C;goto _LL10F;_LL112:;_LL113:(int)_throw((void*)& Cyc_yyfail_FnDecl_tok);
_LL10F:;}return yyzzz;}static unsigned char _tmp18E[15]="list_t<decl_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={Cyc_Core_Failure,{_tmp18E,
_tmp18E,_tmp18E + 15}};struct Cyc_List_List*Cyc_yyget_DeclList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp18F=yy1;struct Cyc_List_List*_tmp190;_LL115:
if(*((void**)_tmp18F)!= Cyc_DeclList_tok)goto _LL117;_tmp190=((struct Cyc_DeclList_tok_struct*)
_tmp18F)->f1;_LL116: yyzzz=_tmp190;goto _LL114;_LL117:;_LL118:(int)_throw((void*)&
Cyc_yyfail_DeclList_tok);_LL114:;}return yyzzz;}static unsigned char _tmp192[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={Cyc_Core_Failure,{
_tmp192,_tmp192,_tmp192 + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(
void*yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{void*_tmp193=yy1;struct Cyc_Parse_Declaration_spec*
_tmp194;_LL11A: if(*((void**)_tmp193)!= Cyc_DeclSpec_tok)goto _LL11C;_tmp194=((
struct Cyc_DeclSpec_tok_struct*)_tmp193)->f1;_LL11B: yyzzz=_tmp194;goto _LL119;
_LL11C:;_LL11D:(int)_throw((void*)& Cyc_yyfail_DeclSpec_tok);_LL119:;}return yyzzz;}
static unsigned char _tmp196[27]="$(declarator_t,exp_opt_t)@";static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{_tmp196,_tmp196,_tmp196 + 27}};struct
_tuple11*Cyc_yyget_InitDecl_tok(void*yy1){struct _tuple11*yyzzz;{void*_tmp197=yy1;
struct _tuple11*_tmp198;_LL11F: if(*((void**)_tmp197)!= Cyc_InitDecl_tok)goto
_LL121;_tmp198=((struct Cyc_InitDecl_tok_struct*)_tmp197)->f1;_LL120: yyzzz=
_tmp198;goto _LL11E;_LL121:;_LL122:(int)_throw((void*)& Cyc_yyfail_InitDecl_tok);
_LL11E:;}return yyzzz;}static unsigned char _tmp19A[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp19A,_tmp19A,_tmp19A + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp19B=yy1;struct Cyc_List_List*
_tmp19C;_LL124: if(*((void**)_tmp19B)!= Cyc_InitDeclList_tok)goto _LL126;_tmp19C=((
struct Cyc_InitDeclList_tok_struct*)_tmp19B)->f1;_LL125: yyzzz=_tmp19C;goto _LL123;
_LL126:;_LL127:(int)_throw((void*)& Cyc_yyfail_InitDeclList_tok);_LL123:;}return
yyzzz;}static unsigned char _tmp19E[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{_tmp19E,_tmp19E,_tmp19E + 16}};
void*Cyc_yyget_StorageClass_tok(void*yy1){void*yyzzz;{void*_tmp19F=yy1;void*
_tmp1A0;_LL129: if(*((void**)_tmp19F)!= Cyc_StorageClass_tok)goto _LL12B;_tmp1A0=(
void*)((struct Cyc_StorageClass_tok_struct*)_tmp19F)->f1;_LL12A: yyzzz=_tmp1A0;
goto _LL128;_LL12B:;_LL12C:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);
_LL128:;}return yyzzz;}static unsigned char _tmp1A2[17]="type_specifier_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{
_tmp1A2,_tmp1A2,_tmp1A2 + 17}};void*Cyc_yyget_TypeSpecifier_tok(void*yy1){void*
yyzzz;{void*_tmp1A3=yy1;void*_tmp1A4;_LL12E: if(*((void**)_tmp1A3)!= Cyc_TypeSpecifier_tok)
goto _LL130;_tmp1A4=(void*)((struct Cyc_TypeSpecifier_tok_struct*)_tmp1A3)->f1;
_LL12F: yyzzz=_tmp1A4;goto _LL12D;_LL130:;_LL131:(int)_throw((void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL12D:;}return yyzzz;}static unsigned char _tmp1A6[12]="aggr_kind_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={Cyc_Core_Failure,{_tmp1A6,
_tmp1A6,_tmp1A6 + 12}};void*Cyc_yyget_StructOrUnion_tok(void*yy1){void*yyzzz;{
void*_tmp1A7=yy1;void*_tmp1A8;_LL133: if(*((void**)_tmp1A7)!= Cyc_StructOrUnion_tok)
goto _LL135;_tmp1A8=(void*)((struct Cyc_StructOrUnion_tok_struct*)_tmp1A7)->f1;
_LL134: yyzzz=_tmp1A8;goto _LL132;_LL135:;_LL136:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);
_LL132:;}return yyzzz;}static unsigned char _tmp1AA[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp1AA,_tmp1AA,_tmp1AA + 8}};struct
Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*
_tmp1AB=yy1;struct Cyc_Absyn_Tqual _tmp1AC;_LL138: if(*((void**)_tmp1AB)!= Cyc_TypeQual_tok)
goto _LL13A;_tmp1AC=((struct Cyc_TypeQual_tok_struct*)_tmp1AB)->f1;_LL139: yyzzz=
_tmp1AC;goto _LL137;_LL13A:;_LL13B:(int)_throw((void*)& Cyc_yyfail_TypeQual_tok);
_LL137:;}return yyzzz;}static unsigned char _tmp1AE[20]="list_t<aggrfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclList_tok={Cyc_Core_Failure,{
_tmp1AE,_tmp1AE,_tmp1AE + 20}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1AF=yy1;struct Cyc_List_List*
_tmp1B0;_LL13D: if(*((void**)_tmp1AF)!= Cyc_AggrFieldDeclList_tok)goto _LL13F;
_tmp1B0=((struct Cyc_AggrFieldDeclList_tok_struct*)_tmp1AF)->f1;_LL13E: yyzzz=
_tmp1B0;goto _LL13C;_LL13F:;_LL140:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclList_tok);
_LL13C:;}return yyzzz;}static unsigned char _tmp1B2[28]="list_t<list_t<aggrfield_t>>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclListList_tok={Cyc_Core_Failure,{
_tmp1B2,_tmp1B2,_tmp1B2 + 28}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclListList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B3=yy1;struct Cyc_List_List*
_tmp1B4;_LL142: if(*((void**)_tmp1B3)!= Cyc_AggrFieldDeclListList_tok)goto _LL144;
_tmp1B4=((struct Cyc_AggrFieldDeclListList_tok_struct*)_tmp1B3)->f1;_LL143: yyzzz=
_tmp1B4;goto _LL141;_LL144:;_LL145:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclListList_tok);
_LL141:;}return yyzzz;}static unsigned char _tmp1B6[24]="list_t<type_modifier_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{
_tmp1B6,_tmp1B6,_tmp1B6 + 24}};struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B7=yy1;struct Cyc_List_List*
_tmp1B8;_LL147: if(*((void**)_tmp1B7)!= Cyc_TypeModifierList_tok)goto _LL149;
_tmp1B8=((struct Cyc_TypeModifierList_tok_struct*)_tmp1B7)->f1;_LL148: yyzzz=
_tmp1B8;goto _LL146;_LL149:;_LL14A:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);
_LL146:;}return yyzzz;}static unsigned char _tmp1BA[13]="declarator_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp1BA,
_tmp1BA,_tmp1BA + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_Declarator_tok(void*
yy1){struct Cyc_Parse_Declarator*yyzzz;{void*_tmp1BB=yy1;struct Cyc_Parse_Declarator*
_tmp1BC;_LL14C: if(*((void**)_tmp1BB)!= Cyc_Declarator_tok)goto _LL14E;_tmp1BC=((
struct Cyc_Declarator_tok_struct*)_tmp1BB)->f1;_LL14D: yyzzz=_tmp1BC;goto _LL14B;
_LL14E:;_LL14F:(int)_throw((void*)& Cyc_yyfail_Declarator_tok);_LL14B:;}return
yyzzz;}static unsigned char _tmp1BE[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{_tmp1BE,_tmp1BE,_tmp1BE + 21}};
struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(void*yy1){
struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp1BF=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp1C0;_LL151: if(*((void**)_tmp1BF)!= Cyc_AbstractDeclarator_tok)goto _LL153;
_tmp1C0=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp1BF)->f1;_LL152: yyzzz=
_tmp1C0;goto _LL150;_LL153:;_LL154:(int)_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL150:;}return yyzzz;}static unsigned char _tmp1C2[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={Cyc_Core_Failure,{_tmp1C2,_tmp1C2,_tmp1C2 + 5}};int Cyc_yyget_Bool_tok(
void*yy1){int yyzzz;{void*_tmp1C3=yy1;int _tmp1C4;_LL156: if(*((void**)_tmp1C3)!= 
Cyc_Bool_tok)goto _LL158;_tmp1C4=((struct Cyc_Bool_tok_struct*)_tmp1C3)->f1;_LL157:
yyzzz=_tmp1C4;goto _LL155;_LL158:;_LL159:(int)_throw((void*)& Cyc_yyfail_Bool_tok);
_LL155:;}return yyzzz;}static unsigned char _tmp1C6[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={Cyc_Core_Failure,{_tmp1C6,_tmp1C6,_tmp1C6 + 8}};void*Cyc_yyget_Scope_tok(
void*yy1){void*yyzzz;{void*_tmp1C7=yy1;void*_tmp1C8;_LL15B: if(*((void**)_tmp1C7)
!= Cyc_Scope_tok)goto _LL15D;_tmp1C8=(void*)((struct Cyc_Scope_tok_struct*)_tmp1C7)->f1;
_LL15C: yyzzz=_tmp1C8;goto _LL15A;_LL15D:;_LL15E:(int)_throw((void*)& Cyc_yyfail_Scope_tok);
_LL15A:;}return yyzzz;}static unsigned char _tmp1CA[14]="tunionfield_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={Cyc_Core_Failure,{
_tmp1CA,_tmp1CA,_tmp1CA + 14}};struct Cyc_Absyn_Tunionfield*Cyc_yyget_TunionField_tok(
void*yy1){struct Cyc_Absyn_Tunionfield*yyzzz;{void*_tmp1CB=yy1;struct Cyc_Absyn_Tunionfield*
_tmp1CC;_LL160: if(*((void**)_tmp1CB)!= Cyc_TunionField_tok)goto _LL162;_tmp1CC=((
struct Cyc_TunionField_tok_struct*)_tmp1CB)->f1;_LL161: yyzzz=_tmp1CC;goto _LL15F;
_LL162:;_LL163:(int)_throw((void*)& Cyc_yyfail_TunionField_tok);_LL15F:;}return
yyzzz;}static unsigned char _tmp1CE[22]="list_t<tunionfield_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{_tmp1CE,_tmp1CE,_tmp1CE + 22}};
struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1CF=yy1;struct Cyc_List_List*_tmp1D0;_LL165: if(*((void**)_tmp1CF)
!= Cyc_TunionFieldList_tok)goto _LL167;_tmp1D0=((struct Cyc_TunionFieldList_tok_struct*)
_tmp1CF)->f1;_LL166: yyzzz=_tmp1D0;goto _LL164;_LL167:;_LL168:(int)_throw((void*)&
Cyc_yyfail_TunionFieldList_tok);_LL164:;}return yyzzz;}static unsigned char _tmp1D2[
50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{_tmp1D2,_tmp1D2,_tmp1D2 + 50}};
struct _tuple14*Cyc_yyget_QualSpecList_tok(void*yy1){struct _tuple14*yyzzz;{void*
_tmp1D3=yy1;struct _tuple14*_tmp1D4;_LL16A: if(*((void**)_tmp1D3)!= Cyc_QualSpecList_tok)
goto _LL16C;_tmp1D4=((struct Cyc_QualSpecList_tok_struct*)_tmp1D3)->f1;_LL16B:
yyzzz=_tmp1D4;goto _LL169;_LL16C:;_LL16D:(int)_throw((void*)& Cyc_yyfail_QualSpecList_tok);
_LL169:;}return yyzzz;}static unsigned char _tmp1D6[14]="list_t<var_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={Cyc_Core_Failure,{_tmp1D6,
_tmp1D6,_tmp1D6 + 14}};struct Cyc_List_List*Cyc_yyget_IdList_tok(void*yy1){struct
Cyc_List_List*yyzzz;{void*_tmp1D7=yy1;struct Cyc_List_List*_tmp1D8;_LL16F: if(*((
void**)_tmp1D7)!= Cyc_IdList_tok)goto _LL171;_tmp1D8=((struct Cyc_IdList_tok_struct*)
_tmp1D7)->f1;_LL170: yyzzz=_tmp1D8;goto _LL16E;_LL171:;_LL172:(int)_throw((void*)&
Cyc_yyfail_IdList_tok);_LL16E:;}return yyzzz;}static unsigned char _tmp1DA[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={Cyc_Core_Failure,{
_tmp1DA,_tmp1DA,_tmp1DA + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(void*yy1){
struct _tuple2*yyzzz;{void*_tmp1DB=yy1;struct _tuple2*_tmp1DC;_LL174: if(*((void**)
_tmp1DB)!= Cyc_ParamDecl_tok)goto _LL176;_tmp1DC=((struct Cyc_ParamDecl_tok_struct*)
_tmp1DB)->f1;_LL175: yyzzz=_tmp1DC;goto _LL173;_LL176:;_LL177:(int)_throw((void*)&
Cyc_yyfail_ParamDecl_tok);_LL173:;}return yyzzz;}static unsigned char _tmp1DE[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp1DE,_tmp1DE,_tmp1DE + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1DF=yy1;struct Cyc_List_List*
_tmp1E0;_LL179: if(*((void**)_tmp1DF)!= Cyc_ParamDeclList_tok)goto _LL17B;_tmp1E0=((
struct Cyc_ParamDeclList_tok_struct*)_tmp1DF)->f1;_LL17A: yyzzz=_tmp1E0;goto _LL178;
_LL17B:;_LL17C:(int)_throw((void*)& Cyc_yyfail_ParamDeclList_tok);_LL178:;}return
yyzzz;}static unsigned char _tmp1E2[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1E2,
_tmp1E2,_tmp1E2 + 107}};struct _tuple13*Cyc_yyget_YY1(void*yy1){struct _tuple13*
yyzzz;{void*_tmp1E3=yy1;struct _tuple13*_tmp1E4;_LL17E: if(*((void**)_tmp1E3)!= 
Cyc_YY1)goto _LL180;_tmp1E4=((struct Cyc_YY1_struct*)_tmp1E3)->f1;_LL17F: yyzzz=
_tmp1E4;goto _LL17D;_LL180:;_LL181:(int)_throw((void*)& Cyc_yyfail_YY1);_LL17D:;}
return yyzzz;}static unsigned char _tmp1E6[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={Cyc_Core_Failure,{_tmp1E6,_tmp1E6,_tmp1E6 + 15}};struct
Cyc_List_List*Cyc_yyget_TypeList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp1E7=yy1;struct Cyc_List_List*_tmp1E8;_LL183: if(*((void**)_tmp1E7)!= Cyc_TypeList_tok)
goto _LL185;_tmp1E8=((struct Cyc_TypeList_tok_struct*)_tmp1E7)->f1;_LL184: yyzzz=
_tmp1E8;goto _LL182;_LL185:;_LL186:(int)_throw((void*)& Cyc_yyfail_TypeList_tok);
_LL182:;}return yyzzz;}static unsigned char _tmp1EA[21]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{
_tmp1EA,_tmp1EA,_tmp1EA + 21}};struct Cyc_List_List*Cyc_yyget_DesignatorList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1EB=yy1;struct Cyc_List_List*
_tmp1EC;_LL188: if(*((void**)_tmp1EB)!= Cyc_DesignatorList_tok)goto _LL18A;_tmp1EC=((
struct Cyc_DesignatorList_tok_struct*)_tmp1EB)->f1;_LL189: yyzzz=_tmp1EC;goto
_LL187;_LL18A:;_LL18B:(int)_throw((void*)& Cyc_yyfail_DesignatorList_tok);_LL187:;}
return yyzzz;}static unsigned char _tmp1EE[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={Cyc_Core_Failure,{_tmp1EE,_tmp1EE,_tmp1EE + 13}};void*
Cyc_yyget_Designator_tok(void*yy1){void*yyzzz;{void*_tmp1EF=yy1;void*_tmp1F0;
_LL18D: if(*((void**)_tmp1EF)!= Cyc_Designator_tok)goto _LL18F;_tmp1F0=(void*)((
struct Cyc_Designator_tok_struct*)_tmp1EF)->f1;_LL18E: yyzzz=_tmp1F0;goto _LL18C;
_LL18F:;_LL190:(int)_throw((void*)& Cyc_yyfail_Designator_tok);_LL18C:;}return
yyzzz;}static unsigned char _tmp1F2[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{_tmp1F2,_tmp1F2,_tmp1F2 + 7}};void*Cyc_yyget_Kind_tok(
void*yy1){void*yyzzz;{void*_tmp1F3=yy1;void*_tmp1F4;_LL192: if(*((void**)_tmp1F3)
!= Cyc_Kind_tok)goto _LL194;_tmp1F4=(void*)((struct Cyc_Kind_tok_struct*)_tmp1F3)->f1;
_LL193: yyzzz=_tmp1F4;goto _LL191;_LL194:;_LL195:(int)_throw((void*)& Cyc_yyfail_Kind_tok);
_LL191:;}return yyzzz;}static unsigned char _tmp1F6[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Type_tok={Cyc_Core_Failure,{_tmp1F6,_tmp1F6,_tmp1F6 + 7}};void*Cyc_yyget_Type_tok(
void*yy1){void*yyzzz;{void*_tmp1F7=yy1;void*_tmp1F8;_LL197: if(*((void**)_tmp1F7)
!= Cyc_Type_tok)goto _LL199;_tmp1F8=(void*)((struct Cyc_Type_tok_struct*)_tmp1F7)->f1;
_LL198: yyzzz=_tmp1F8;goto _LL196;_LL199:;_LL19A:(int)_throw((void*)& Cyc_yyfail_Type_tok);
_LL196:;}return yyzzz;}static unsigned char _tmp1FA[20]="list_t<attribute_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={Cyc_Core_Failure,{
_tmp1FA,_tmp1FA,_tmp1FA + 20}};struct Cyc_List_List*Cyc_yyget_AttributeList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1FB=yy1;struct Cyc_List_List*
_tmp1FC;_LL19C: if(*((void**)_tmp1FB)!= Cyc_AttributeList_tok)goto _LL19E;_tmp1FC=((
struct Cyc_AttributeList_tok_struct*)_tmp1FB)->f1;_LL19D: yyzzz=_tmp1FC;goto _LL19B;
_LL19E:;_LL19F:(int)_throw((void*)& Cyc_yyfail_AttributeList_tok);_LL19B:;}return
yyzzz;}static unsigned char _tmp1FE[12]="attribute_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{_tmp1FE,_tmp1FE,_tmp1FE + 12}};void*
Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{void*_tmp1FF=yy1;void*_tmp200;
_LL1A1: if(*((void**)_tmp1FF)!= Cyc_Attribute_tok)goto _LL1A3;_tmp200=(void*)((
struct Cyc_Attribute_tok_struct*)_tmp1FF)->f1;_LL1A2: yyzzz=_tmp200;goto _LL1A0;
_LL1A3:;_LL1A4:(int)_throw((void*)& Cyc_yyfail_Attribute_tok);_LL1A0:;}return
yyzzz;}static unsigned char _tmp202[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={Cyc_Core_Failure,{_tmp202,_tmp202,_tmp202 + 12}};struct
Cyc_Absyn_Enumfield*Cyc_yyget_Enumfield_tok(void*yy1){struct Cyc_Absyn_Enumfield*
yyzzz;{void*_tmp203=yy1;struct Cyc_Absyn_Enumfield*_tmp204;_LL1A6: if(*((void**)
_tmp203)!= Cyc_Enumfield_tok)goto _LL1A8;_tmp204=((struct Cyc_Enumfield_tok_struct*)
_tmp203)->f1;_LL1A7: yyzzz=_tmp204;goto _LL1A5;_LL1A8:;_LL1A9:(int)_throw((void*)&
Cyc_yyfail_Enumfield_tok);_LL1A5:;}return yyzzz;}static unsigned char _tmp206[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{
_tmp206,_tmp206,_tmp206 + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp207=yy1;struct Cyc_List_List*
_tmp208;_LL1AB: if(*((void**)_tmp207)!= Cyc_EnumfieldList_tok)goto _LL1AD;_tmp208=((
struct Cyc_EnumfieldList_tok_struct*)_tmp207)->f1;_LL1AC: yyzzz=_tmp208;goto _LL1AA;
_LL1AD:;_LL1AE:(int)_throw((void*)& Cyc_yyfail_EnumfieldList_tok);_LL1AA:;}return
yyzzz;}static unsigned char _tmp20A[14]="opt_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={Cyc_Core_Failure,{_tmp20A,_tmp20A,_tmp20A + 14}};struct
Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp20B=yy1;struct Cyc_Core_Opt*_tmp20C;_LL1B0: if(*((void**)_tmp20B)!= Cyc_TypeOpt_tok)
goto _LL1B2;_tmp20C=((struct Cyc_TypeOpt_tok_struct*)_tmp20B)->f1;_LL1B1: yyzzz=
_tmp20C;goto _LL1AF;_LL1B2:;_LL1B3:(int)_throw((void*)& Cyc_yyfail_TypeOpt_tok);
_LL1AF:;}return yyzzz;}static unsigned char _tmp20E[26]="list_t<$(type_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{
_tmp20E,_tmp20E,_tmp20E + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*
yy1){struct Cyc_List_List*yyzzz;{void*_tmp20F=yy1;struct Cyc_List_List*_tmp210;
_LL1B5: if(*((void**)_tmp20F)!= Cyc_Rgnorder_tok)goto _LL1B7;_tmp210=((struct Cyc_Rgnorder_tok_struct*)
_tmp20F)->f1;_LL1B6: yyzzz=_tmp210;goto _LL1B4;_LL1B7:;_LL1B8:(int)_throw((void*)&
Cyc_yyfail_Rgnorder_tok);_LL1B4:;}return yyzzz;}struct Cyc_Yyltype{int timestamp;
int first_line;int first_column;int last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp211;_tmp211.timestamp=0;_tmp211.first_line=0;
_tmp211.first_column=0;_tmp211.last_line=0;_tmp211.last_column=0;_tmp211;});}
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[354]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,125,2,2,107,123,119,2,
103,104,113,116,105,118,110,122,2,2,2,2,2,2,2,2,2,2,109,99,108,102,117,115,114,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,111,2,112,121,106,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,100,120,101,124,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,
59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,
86,87,88,89,90,91,92,93,94,95,96,97,98};static unsigned char _tmp212[2]="$";static
unsigned char _tmp213[6]="error";static unsigned char _tmp214[12]="$undefined.";
static unsigned char _tmp215[5]="AUTO";static unsigned char _tmp216[9]="REGISTER";
static unsigned char _tmp217[7]="STATIC";static unsigned char _tmp218[7]="EXTERN";
static unsigned char _tmp219[8]="TYPEDEF";static unsigned char _tmp21A[5]="VOID";
static unsigned char _tmp21B[5]="CHAR";static unsigned char _tmp21C[6]="SHORT";
static unsigned char _tmp21D[4]="INT";static unsigned char _tmp21E[5]="LONG";static
unsigned char _tmp21F[6]="FLOAT";static unsigned char _tmp220[7]="DOUBLE";static
unsigned char _tmp221[7]="SIGNED";static unsigned char _tmp222[9]="UNSIGNED";static
unsigned char _tmp223[6]="CONST";static unsigned char _tmp224[9]="VOLATILE";static
unsigned char _tmp225[9]="RESTRICT";static unsigned char _tmp226[7]="STRUCT";static
unsigned char _tmp227[6]="UNION";static unsigned char _tmp228[5]="CASE";static
unsigned char _tmp229[8]="DEFAULT";static unsigned char _tmp22A[7]="INLINE";static
unsigned char _tmp22B[7]="SIZEOF";static unsigned char _tmp22C[9]="OFFSETOF";static
unsigned char _tmp22D[3]="IF";static unsigned char _tmp22E[5]="ELSE";static
unsigned char _tmp22F[7]="SWITCH";static unsigned char _tmp230[6]="WHILE";static
unsigned char _tmp231[3]="DO";static unsigned char _tmp232[4]="FOR";static
unsigned char _tmp233[5]="GOTO";static unsigned char _tmp234[9]="CONTINUE";static
unsigned char _tmp235[6]="BREAK";static unsigned char _tmp236[7]="RETURN";static
unsigned char _tmp237[5]="ENUM";static unsigned char _tmp238[8]="NULL_kw";static
unsigned char _tmp239[4]="LET";static unsigned char _tmp23A[6]="THROW";static
unsigned char _tmp23B[4]="TRY";static unsigned char _tmp23C[6]="CATCH";static
unsigned char _tmp23D[4]="NEW";static unsigned char _tmp23E[9]="ABSTRACT";static
unsigned char _tmp23F[9]="FALLTHRU";static unsigned char _tmp240[6]="USING";static
unsigned char _tmp241[10]="NAMESPACE";static unsigned char _tmp242[7]="TUNION";
static unsigned char _tmp243[8]="XTUNION";static unsigned char _tmp244[9]="FORARRAY";
static unsigned char _tmp245[5]="FILL";static unsigned char _tmp246[8]="CODEGEN";
static unsigned char _tmp247[4]="CUT";static unsigned char _tmp248[7]="SPLICE";
static unsigned char _tmp249[7]="MALLOC";static unsigned char _tmp24A[8]="RMALLOC";
static unsigned char _tmp24B[7]="CALLOC";static unsigned char _tmp24C[8]="RCALLOC";
static unsigned char _tmp24D[9]="REGION_T";static unsigned char _tmp24E[9]="SIZEOF_T";
static unsigned char _tmp24F[7]="REGION";static unsigned char _tmp250[5]="RNEW";
static unsigned char _tmp251[8]="REGIONS";static unsigned char _tmp252[4]="GEN";
static unsigned char _tmp253[7]="PTR_OP";static unsigned char _tmp254[7]="INC_OP";
static unsigned char _tmp255[7]="DEC_OP";static unsigned char _tmp256[8]="LEFT_OP";
static unsigned char _tmp257[9]="RIGHT_OP";static unsigned char _tmp258[6]="LE_OP";
static unsigned char _tmp259[6]="GE_OP";static unsigned char _tmp25A[6]="EQ_OP";
static unsigned char _tmp25B[6]="NE_OP";static unsigned char _tmp25C[7]="AND_OP";
static unsigned char _tmp25D[6]="OR_OP";static unsigned char _tmp25E[11]="MUL_ASSIGN";
static unsigned char _tmp25F[11]="DIV_ASSIGN";static unsigned char _tmp260[11]="MOD_ASSIGN";
static unsigned char _tmp261[11]="ADD_ASSIGN";static unsigned char _tmp262[11]="SUB_ASSIGN";
static unsigned char _tmp263[12]="LEFT_ASSIGN";static unsigned char _tmp264[13]="RIGHT_ASSIGN";
static unsigned char _tmp265[11]="AND_ASSIGN";static unsigned char _tmp266[11]="XOR_ASSIGN";
static unsigned char _tmp267[10]="OR_ASSIGN";static unsigned char _tmp268[9]="ELLIPSIS";
static unsigned char _tmp269[11]="LEFT_RIGHT";static unsigned char _tmp26A[12]="COLON_COLON";
static unsigned char _tmp26B[11]="IDENTIFIER";static unsigned char _tmp26C[17]="INTEGER_CONSTANT";
static unsigned char _tmp26D[7]="STRING";static unsigned char _tmp26E[19]="CHARACTER_CONSTANT";
static unsigned char _tmp26F[18]="FLOATING_CONSTANT";static unsigned char _tmp270[9]="TYPE_VAR";
static unsigned char _tmp271[13]="TYPEDEF_NAME";static unsigned char _tmp272[16]="QUAL_IDENTIFIER";
static unsigned char _tmp273[18]="QUAL_TYPEDEF_NAME";static unsigned char _tmp274[10]="ATTRIBUTE";
static unsigned char _tmp275[4]="';'";static unsigned char _tmp276[4]="'{'";static
unsigned char _tmp277[4]="'}'";static unsigned char _tmp278[4]="'='";static
unsigned char _tmp279[4]="'('";static unsigned char _tmp27A[4]="')'";static
unsigned char _tmp27B[4]="','";static unsigned char _tmp27C[4]="'_'";static
unsigned char _tmp27D[4]="'$'";static unsigned char _tmp27E[4]="'<'";static
unsigned char _tmp27F[4]="':'";static unsigned char _tmp280[4]="'.'";static
unsigned char _tmp281[4]="'['";static unsigned char _tmp282[4]="']'";static
unsigned char _tmp283[4]="'*'";static unsigned char _tmp284[4]="'@'";static
unsigned char _tmp285[4]="'?'";static unsigned char _tmp286[4]="'+'";static
unsigned char _tmp287[4]="'>'";static unsigned char _tmp288[4]="'-'";static
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
_tmp2A8[16]="type_params_opt";static unsigned char _tmp2A9[16]="struct_or_union";
static unsigned char _tmp2AA[24]="struct_declaration_list";static unsigned char
_tmp2AB[25]="struct_declaration_list0";static unsigned char _tmp2AC[21]="init_declarator_list";
static unsigned char _tmp2AD[22]="init_declarator_list0";static unsigned char
_tmp2AE[16]="init_declarator";static unsigned char _tmp2AF[19]="struct_declaration";
static unsigned char _tmp2B0[25]="specifier_qualifier_list";static unsigned char
_tmp2B1[23]="struct_declarator_list";static unsigned char _tmp2B2[24]="struct_declarator_list0";
static unsigned char _tmp2B3[18]="struct_declarator";static unsigned char _tmp2B4[17]="tunion_specifier";
static unsigned char _tmp2B5[18]="tunion_or_xtunion";static unsigned char _tmp2B6[17]="tunionfield_list";
static unsigned char _tmp2B7[18]="tunionfield_scope";static unsigned char _tmp2B8[12]="tunionfield";
static unsigned char _tmp2B9[11]="declarator";static unsigned char _tmp2BA[18]="direct_declarator";
static unsigned char _tmp2BB[8]="pointer";static unsigned char _tmp2BC[13]="pointer_char";
static unsigned char _tmp2BD[8]="rgn_opt";static unsigned char _tmp2BE[4]="rgn";
static unsigned char _tmp2BF[20]="type_qualifier_list";static unsigned char _tmp2C0[
20]="parameter_type_list";static unsigned char _tmp2C1[9]="type_var";static
unsigned char _tmp2C2[16]="optional_effect";static unsigned char _tmp2C3[19]="optional_rgn_order";
static unsigned char _tmp2C4[10]="rgn_order";static unsigned char _tmp2C5[16]="optional_inject";
static unsigned char _tmp2C6[11]="effect_set";static unsigned char _tmp2C7[14]="atomic_effect";
static unsigned char _tmp2C8[11]="region_set";static unsigned char _tmp2C9[15]="parameter_list";
static unsigned char _tmp2CA[22]="parameter_declaration";static unsigned char
_tmp2CB[16]="identifier_list";static unsigned char _tmp2CC[17]="identifier_list0";
static unsigned char _tmp2CD[12]="initializer";static unsigned char _tmp2CE[18]="array_initializer";
static unsigned char _tmp2CF[17]="initializer_list";static unsigned char _tmp2D0[12]="designation";
static unsigned char _tmp2D1[16]="designator_list";static unsigned char _tmp2D2[11]="designator";
static unsigned char _tmp2D3[10]="type_name";static unsigned char _tmp2D4[14]="any_type_name";
static unsigned char _tmp2D5[15]="type_name_list";static unsigned char _tmp2D6[20]="abstract_declarator";
static unsigned char _tmp2D7[27]="direct_abstract_declarator";static unsigned char
_tmp2D8[10]="statement";static unsigned char _tmp2D9[18]="labeled_statement";
static unsigned char _tmp2DA[21]="expression_statement";static unsigned char _tmp2DB[
19]="compound_statement";static unsigned char _tmp2DC[16]="block_item_list";static
unsigned char _tmp2DD[20]="selection_statement";static unsigned char _tmp2DE[15]="switch_clauses";
static unsigned char _tmp2DF[16]="switchC_clauses";static unsigned char _tmp2E0[20]="iteration_statement";
static unsigned char _tmp2E1[15]="jump_statement";static unsigned char _tmp2E2[8]="pattern";
static unsigned char _tmp2E3[19]="tuple_pattern_list";static unsigned char _tmp2E4[
14]="field_pattern";static unsigned char _tmp2E5[19]="field_pattern_list";static
unsigned char _tmp2E6[11]="expression";static unsigned char _tmp2E7[22]="assignment_expression";
static unsigned char _tmp2E8[20]="assignment_operator";static unsigned char _tmp2E9[
23]="conditional_expression";static unsigned char _tmp2EA[20]="constant_expression";
static unsigned char _tmp2EB[22]="logical_or_expression";static unsigned char
_tmp2EC[23]="logical_and_expression";static unsigned char _tmp2ED[24]="inclusive_or_expression";
static unsigned char _tmp2EE[24]="exclusive_or_expression";static unsigned char
_tmp2EF[15]="and_expression";static unsigned char _tmp2F0[20]="equality_expression";
static unsigned char _tmp2F1[22]="relational_expression";static unsigned char
_tmp2F2[17]="shift_expression";static unsigned char _tmp2F3[20]="additive_expression";
static unsigned char _tmp2F4[26]="multiplicative_expression";static unsigned char
_tmp2F5[16]="cast_expression";static unsigned char _tmp2F6[17]="unary_expression";
static unsigned char _tmp2F7[15]="unary_operator";static unsigned char _tmp2F8[19]="postfix_expression";
static unsigned char _tmp2F9[19]="primary_expression";static unsigned char _tmp2FA[
25]="argument_expression_list";static unsigned char _tmp2FB[26]="argument_expression_list0";
static unsigned char _tmp2FC[9]="constant";static unsigned char _tmp2FD[20]="qual_opt_identifier";
static unsigned char _tmp2FE[17]="qual_opt_typedef";static unsigned char _tmp2FF[18]="struct_union_name";
static unsigned char _tmp300[11]="field_name";static unsigned char _tmp301[12]="right_angle";
static struct _tagged_arr Cyc_yytname[240]={{_tmp212,_tmp212,_tmp212 + 2},{_tmp213,
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
_tmp2A7,_tmp2A7,_tmp2A7 + 26},{_tmp2A8,_tmp2A8,_tmp2A8 + 16},{_tmp2A9,_tmp2A9,
_tmp2A9 + 16},{_tmp2AA,_tmp2AA,_tmp2AA + 24},{_tmp2AB,_tmp2AB,_tmp2AB + 25},{
_tmp2AC,_tmp2AC,_tmp2AC + 21},{_tmp2AD,_tmp2AD,_tmp2AD + 22},{_tmp2AE,_tmp2AE,
_tmp2AE + 16},{_tmp2AF,_tmp2AF,_tmp2AF + 19},{_tmp2B0,_tmp2B0,_tmp2B0 + 25},{
_tmp2B1,_tmp2B1,_tmp2B1 + 23},{_tmp2B2,_tmp2B2,_tmp2B2 + 24},{_tmp2B3,_tmp2B3,
_tmp2B3 + 18},{_tmp2B4,_tmp2B4,_tmp2B4 + 17},{_tmp2B5,_tmp2B5,_tmp2B5 + 18},{
_tmp2B6,_tmp2B6,_tmp2B6 + 17},{_tmp2B7,_tmp2B7,_tmp2B7 + 18},{_tmp2B8,_tmp2B8,
_tmp2B8 + 12},{_tmp2B9,_tmp2B9,_tmp2B9 + 11},{_tmp2BA,_tmp2BA,_tmp2BA + 18},{
_tmp2BB,_tmp2BB,_tmp2BB + 8},{_tmp2BC,_tmp2BC,_tmp2BC + 13},{_tmp2BD,_tmp2BD,
_tmp2BD + 8},{_tmp2BE,_tmp2BE,_tmp2BE + 4},{_tmp2BF,_tmp2BF,_tmp2BF + 20},{_tmp2C0,
_tmp2C0,_tmp2C0 + 20},{_tmp2C1,_tmp2C1,_tmp2C1 + 9},{_tmp2C2,_tmp2C2,_tmp2C2 + 16},{
_tmp2C3,_tmp2C3,_tmp2C3 + 19},{_tmp2C4,_tmp2C4,_tmp2C4 + 10},{_tmp2C5,_tmp2C5,
_tmp2C5 + 16},{_tmp2C6,_tmp2C6,_tmp2C6 + 11},{_tmp2C7,_tmp2C7,_tmp2C7 + 14},{
_tmp2C8,_tmp2C8,_tmp2C8 + 11},{_tmp2C9,_tmp2C9,_tmp2C9 + 15},{_tmp2CA,_tmp2CA,
_tmp2CA + 22},{_tmp2CB,_tmp2CB,_tmp2CB + 16},{_tmp2CC,_tmp2CC,_tmp2CC + 17},{
_tmp2CD,_tmp2CD,_tmp2CD + 12},{_tmp2CE,_tmp2CE,_tmp2CE + 18},{_tmp2CF,_tmp2CF,
_tmp2CF + 17},{_tmp2D0,_tmp2D0,_tmp2D0 + 12},{_tmp2D1,_tmp2D1,_tmp2D1 + 16},{
_tmp2D2,_tmp2D2,_tmp2D2 + 11},{_tmp2D3,_tmp2D3,_tmp2D3 + 10},{_tmp2D4,_tmp2D4,
_tmp2D4 + 14},{_tmp2D5,_tmp2D5,_tmp2D5 + 15},{_tmp2D6,_tmp2D6,_tmp2D6 + 20},{
_tmp2D7,_tmp2D7,_tmp2D7 + 27},{_tmp2D8,_tmp2D8,_tmp2D8 + 10},{_tmp2D9,_tmp2D9,
_tmp2D9 + 18},{_tmp2DA,_tmp2DA,_tmp2DA + 21},{_tmp2DB,_tmp2DB,_tmp2DB + 19},{
_tmp2DC,_tmp2DC,_tmp2DC + 16},{_tmp2DD,_tmp2DD,_tmp2DD + 20},{_tmp2DE,_tmp2DE,
_tmp2DE + 15},{_tmp2DF,_tmp2DF,_tmp2DF + 16},{_tmp2E0,_tmp2E0,_tmp2E0 + 20},{
_tmp2E1,_tmp2E1,_tmp2E1 + 15},{_tmp2E2,_tmp2E2,_tmp2E2 + 8},{_tmp2E3,_tmp2E3,
_tmp2E3 + 19},{_tmp2E4,_tmp2E4,_tmp2E4 + 14},{_tmp2E5,_tmp2E5,_tmp2E5 + 19},{
_tmp2E6,_tmp2E6,_tmp2E6 + 11},{_tmp2E7,_tmp2E7,_tmp2E7 + 22},{_tmp2E8,_tmp2E8,
_tmp2E8 + 20},{_tmp2E9,_tmp2E9,_tmp2E9 + 23},{_tmp2EA,_tmp2EA,_tmp2EA + 20},{
_tmp2EB,_tmp2EB,_tmp2EB + 22},{_tmp2EC,_tmp2EC,_tmp2EC + 23},{_tmp2ED,_tmp2ED,
_tmp2ED + 24},{_tmp2EE,_tmp2EE,_tmp2EE + 24},{_tmp2EF,_tmp2EF,_tmp2EF + 15},{
_tmp2F0,_tmp2F0,_tmp2F0 + 20},{_tmp2F1,_tmp2F1,_tmp2F1 + 22},{_tmp2F2,_tmp2F2,
_tmp2F2 + 17},{_tmp2F3,_tmp2F3,_tmp2F3 + 20},{_tmp2F4,_tmp2F4,_tmp2F4 + 26},{
_tmp2F5,_tmp2F5,_tmp2F5 + 16},{_tmp2F6,_tmp2F6,_tmp2F6 + 17},{_tmp2F7,_tmp2F7,
_tmp2F7 + 15},{_tmp2F8,_tmp2F8,_tmp2F8 + 19},{_tmp2F9,_tmp2F9,_tmp2F9 + 19},{
_tmp2FA,_tmp2FA,_tmp2FA + 25},{_tmp2FB,_tmp2FB,_tmp2FB + 26},{_tmp2FC,_tmp2FC,
_tmp2FC + 9},{_tmp2FD,_tmp2FD,_tmp2FD + 20},{_tmp2FE,_tmp2FE,_tmp2FE + 17},{_tmp2FF,
_tmp2FF,_tmp2FF + 18},{_tmp300,_tmp300,_tmp300 + 11},{_tmp301,_tmp301,_tmp301 + 12}};
static short Cyc_yyr1[406]={0,126,127,127,127,127,127,127,127,128,128,129,129,129,
129,130,130,131,132,133,134,135,135,135,135,136,136,137,137,137,137,137,137,137,
137,138,138,138,138,138,138,138,139,139,140,141,141,142,142,142,142,142,143,143,
143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,144,145,145,145,
146,146,146,147,147,148,148,149,149,149,150,150,151,151,152,153,153,154,155,155,
156,156,157,158,158,158,158,159,160,160,161,161,161,162,162,162,162,163,163,164,
164,164,164,165,165,165,166,166,167,167,168,168,168,168,168,168,168,168,168,168,
168,169,169,169,169,170,170,170,170,170,171,171,172,172,173,173,174,174,174,174,
175,175,176,176,177,177,178,178,179,179,180,180,181,181,181,181,182,182,183,183,
184,184,184,185,186,186,187,187,188,188,188,188,189,189,189,189,190,191,191,192,
192,193,193,194,194,194,194,194,195,195,196,196,196,197,197,197,197,197,197,197,
197,197,197,197,198,198,198,198,198,198,198,198,198,198,199,200,200,201,201,202,
202,202,202,202,202,203,203,203,203,203,204,204,204,204,204,204,205,205,205,205,
206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,207,207,207,207,207,
207,207,207,208,208,208,208,208,208,208,208,208,208,208,208,208,209,209,210,210,
211,211,212,212,213,213,214,214,214,214,214,214,214,214,214,214,214,215,215,215,
215,215,215,215,216,217,217,218,218,219,219,220,220,221,221,222,222,222,223,223,
223,223,223,224,224,224,225,225,225,226,226,226,226,227,227,228,228,228,228,228,
228,228,228,228,228,228,228,228,228,228,228,229,229,229,230,230,230,230,230,230,
230,230,230,230,230,230,231,231,231,231,231,231,231,231,231,232,233,233,234,234,
234,234,235,235,236,236,237,237,238,238,239,239};static short Cyc_yyr2[406]={0,1,2,
3,5,3,5,6,0,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,1,2,2,3,2,3,2,3,2,3,1,1,1,1,2,1,1,0,
1,6,1,3,1,1,4,4,8,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,4,4,4,1,1,1,1,5,2,4,1,3,1,3,4,7,
3,0,3,1,1,1,1,2,1,1,3,1,3,3,2,3,2,3,1,1,3,1,2,3,6,4,3,5,1,1,1,2,3,3,0,1,1,2,5,1,
2,1,3,3,4,4,5,4,4,2,2,1,3,4,4,5,1,1,4,4,1,0,1,1,1,1,2,3,5,5,7,1,3,0,2,0,2,3,5,0,
1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,
3,1,1,2,3,2,3,3,4,4,3,5,4,4,2,1,1,1,1,1,1,6,3,2,2,3,1,2,2,3,1,2,1,2,1,2,5,7,7,8,
6,0,3,4,5,6,7,0,3,4,5,5,7,6,7,7,8,7,8,8,9,6,7,7,8,9,3,2,2,2,3,2,4,5,1,3,1,2,1,1,
1,1,4,4,5,2,2,1,3,1,2,1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,
1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,
9,11,1,1,1,1,4,3,4,3,3,2,2,6,7,4,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,
1,1,1};static short Cyc_yydefact[826]={0,134,35,36,37,38,40,52,54,55,56,57,58,59,
60,61,71,72,73,86,87,42,0,0,41,0,0,111,112,0,0,396,154,398,397,399,0,53,0,139,
140,143,1,0,9,0,0,10,0,42,42,42,62,63,0,64,0,0,122,0,144,65,124,84,39,0,33,43,0,
75,283,396,279,282,281,0,277,0,0,0,0,0,177,0,284,17,19,0,0,0,0,0,0,0,2,0,0,0,0,
21,0,91,92,94,27,29,31,0,400,401,84,147,0,146,84,38,0,25,0,0,11,156,0,0,132,123,
42,145,133,0,66,0,0,34,79,0,77,0,0,0,289,280,288,24,0,0,84,0,0,0,42,42,195,197,0,
0,402,403,155,70,125,0,0,172,0,0,395,0,0,0,0,0,0,0,0,0,84,0,0,392,382,393,394,0,
0,0,0,367,0,365,366,0,298,311,319,321,323,325,327,329,332,337,340,343,347,0,349,
368,381,380,0,3,0,5,0,22,0,0,0,12,28,30,32,0,88,89,0,83,84,0,109,39,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,396,229,231,0,237,233,0,235,218,219,220,0,221,222,223,0,296,26,
13,94,162,178,0,0,158,156,0,202,0,126,0,135,0,0,0,0,76,0,0,278,290,0,179,0,0,0,0,
198,170,0,97,99,156,0,204,196,205,405,0,404,68,69,0,174,0,176,67,0,0,357,0,313,
347,0,314,315,0,0,0,0,0,0,0,0,0,350,351,0,0,0,0,353,354,352,141,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,301,302,303,304,305,306,307,308,309,310,300,0,355,0,374,
375,0,0,0,384,84,0,142,18,0,20,0,93,95,181,180,14,81,90,0,0,101,102,104,84,108,
84,117,0,0,0,0,0,0,0,270,271,272,0,0,274,0,0,226,227,0,0,0,0,238,234,94,236,232,
230,0,163,0,0,0,169,157,164,128,0,0,0,158,130,0,131,127,148,137,136,85,0,48,47,0,
45,80,318,78,74,285,0,23,0,0,0,0,191,292,294,0,286,0,0,199,98,100,0,158,0,208,0,
206,156,0,0,217,201,173,0,0,0,182,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,383,390,0,389,
320,0,322,324,326,328,330,331,335,336,333,334,338,339,341,342,344,345,346,299,
373,370,0,372,0,0,0,4,6,105,96,0,0,0,110,119,118,0,0,113,0,0,0,0,0,0,0,0,269,273,
0,0,0,0,225,0,228,0,15,297,156,0,166,0,0,0,159,129,162,150,203,149,138,7,0,0,0,
291,194,0,293,190,192,287,0,200,171,213,0,207,210,0,158,0,209,0,356,0,0,183,0,
187,378,379,0,361,0,0,0,0,0,0,0,0,348,386,0,0,371,369,0,0,103,106,0,107,120,117,
117,0,0,0,0,0,0,0,0,0,0,244,275,0,0,0,16,158,0,167,165,0,158,0,0,0,0,44,46,193,
295,212,215,0,216,211,0,0,0,184,188,0,0,0,0,316,317,360,388,0,391,312,387,0,385,
82,0,116,115,239,0,244,254,0,0,0,0,0,0,0,0,0,0,0,0,276,0,0,152,168,160,151,156,0,
49,50,214,359,358,0,189,362,0,0,376,0,0,0,250,0,0,256,0,0,0,264,0,0,0,0,0,0,0,0,
243,0,224,0,158,0,0,0,0,377,121,240,0,0,0,241,255,257,258,0,266,265,0,260,0,0,0,
0,244,245,0,161,153,0,0,0,0,0,0,242,259,267,261,262,0,0,244,246,0,0,185,363,0,
250,251,263,244,247,268,51,0,250,252,244,248,364,253,249,0,0,0};static short Cyc_yydefgoto[
114]={823,42,43,44,251,45,385,46,387,47,218,48,49,66,67,455,456,50,158,51,52,134,
135,53,130,54,223,224,100,101,102,225,152,396,397,398,55,56,550,551,552,57,58,59,
60,126,112,450,478,61,479,441,579,433,437,438,296,272,163,81,82,494,390,495,496,
467,468,153,274,275,480,303,254,255,256,257,258,259,715,769,260,261,287,288,470,
471,262,263,372,192,459,193,194,195,196,197,198,199,200,201,202,203,204,205,206,
207,511,512,208,209,63,110,159,307};static short Cyc_yypact[826]={1968,- -32768,-
-32768,- -32768,- -32768,- 20,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,25,158,
823,- -32768,137,43,- -32768,- -32768,35,51,- -32768,141,- -32768,- -32768,- -32768,531,
- -32768,88,138,146,- -32768,- -32768,1852,- -32768,105,149,- -32768,522,25,25,25,-
-32768,- -32768,258,- -32768,90,2330,180,28,7,- -32768,137,165,170,223,2455,- -32768,
137,182,- -32768,75,- -32768,- -32768,- -32768,2417,- -32768,232,214,274,2417,268,272,
278,- 4,- -32768,- -32768,4724,4724,132,330,4895,4281,4281,- -32768,1852,2083,1852,
2083,- -32768,298,312,- -32768,2266,2455,2455,2455,4895,- -32768,- -32768,165,-
-32768,137,- -32768,- 71,387,1244,- -32768,2330,522,- -32768,1788,4724,2953,- -32768,
180,25,- -32768,- -32768,4724,- -32768,2083,377,- -32768,343,381,389,137,385,2417,-
-32768,- -32768,- -32768,- -32768,404,4281,165,2417,394,178,25,25,230,- -32768,- 37,-
37,- -32768,- -32768,- -32768,- -32768,- -32768,553,207,- -32768,4509,396,- -32768,4281,
4357,398,400,403,418,425,428,440,165,4585,4585,- -32768,- -32768,- -32768,- -32768,
1608,446,4661,4661,- -32768,4661,- -32768,- -32768,450,- -32768,- 30,479,445,447,457,
221,20,281,- 50,97,- -32768,872,4661,348,- -32768,- -32768,0,483,- -32768,486,- -32768,
488,- -32768,531,3036,2330,- -32768,- -32768,- -32768,- -32768,495,4895,- -32768,614,
480,165,137,491,- -32768,498,- 10,501,2538,503,523,527,529,3119,2538,45,547,2538,
2538,13,521,- -32768,- -32768,552,1367,1367,522,1367,- -32768,- -32768,- -32768,560,-
-32768,- -32768,- -32768,103,- -32768,- -32768,- -32768,561,576,- -32768,66,565,574,
217,575,571,- 28,- -32768,582,225,- 28,597,23,137,- -32768,4281,598,- -32768,- -32768,
322,- -32768,252,746,350,4724,- -32768,595,601,4895,4895,2205,3202,46,- -32768,189,
- -32768,66,- -32768,- -32768,- -32768,884,- -32768,670,- -32768,- -32768,4895,1608,-
-32768,4895,- -32768,- -32768,2621,- -32768,629,4281,2144,4281,4281,4281,4281,4281,
602,1608,- -32768,- -32768,1367,603,359,4281,- -32768,- -32768,- -32768,- -32768,4661,
4281,4661,4661,4661,4661,4661,4661,4661,4661,4661,4661,4661,4661,4661,4661,4661,
4661,4661,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,4281,- -32768,132,- -32768,- -32768,3285,132,4281,- -32768,
165,600,- -32768,- -32768,1852,- -32768,1852,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,4281,610,608,- -32768,605,165,- -32768,165,490,4281,616,
4281,4281,692,1490,625,- -32768,- -32768,- -32768,261,683,- -32768,3368,2394,- -32768,
- -32768,2538,637,2538,1726,- -32768,- -32768,2266,- -32768,- -32768,- -32768,4281,-
-32768,4895,631,204,- -32768,- -32768,619,- -32768,638,639,4795,574,- -32768,4724,-
-32768,- -32768,- -32768,- -32768,225,- -32768,1852,- -32768,641,642,645,- -32768,-
-32768,- -32768,- -32768,- -32768,2417,- -32768,132,4281,2417,164,- -32768,- -32768,-
-32768,220,- -32768,42,658,- -32768,- -32768,- -32768,652,574,653,- -32768,648,189,
4780,4724,3451,- -32768,- -32768,- -32768,657,659,673,- -32768,- -32768,229,3036,407,
661,531,411,663,665,667,415,4895,672,662,4433,- -32768,- -32768,676,678,479,299,
445,447,457,221,20,20,281,281,281,281,- 50,- 50,97,97,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,681,- -32768,215,2870,4724,- -32768,- -32768,- -32768,- -32768,
614,4281,4895,- -32768,- -32768,- -32768,685,137,276,420,4281,435,437,674,3534,3617,
293,- -32768,- -32768,687,690,686,693,- -32768,677,- -32768,2330,- -32768,- -32768,694,
4724,- -32768,695,66,708,- -32768,- -32768,151,- -32768,571,- -32768,- -32768,- -32768,
395,713,23,- -32768,- -32768,679,- -32768,- -32768,- -32768,- -32768,746,- -32768,-
-32768,- -32768,715,- -32768,- -32768,716,574,- 28,- -32768,710,723,162,719,- -32768,
2704,- -32768,- -32768,- -32768,2330,- -32768,4281,799,4281,4357,725,723,727,2870,-
-32768,- -32768,4281,4281,- -32768,- -32768,304,- 28,- -32768,- -32768,731,- -32768,730,
67,490,2538,442,737,2538,4281,3700,302,3783,313,3866,540,- -32768,741,4281,752,-
-32768,574,77,- -32768,- -32768,749,574,4895,739,742,743,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,744,- -32768,- -32768,747,750,4281,- -32768,- -32768,3036,467,
757,740,- -32768,629,- -32768,- -32768,342,- -32768,- -32768,- -32768,2870,- -32768,-
-32768,4895,- -32768,- -32768,827,762,540,- -32768,469,2538,474,3949,2538,489,4032,
4115,325,2417,754,765,- -32768,336,2538,- -32768,- -32768,763,- -32768,694,777,-
-32768,- -32768,- -32768,- -32768,- -32768,340,- -32768,- -32768,4895,844,- -32768,2787,
494,2538,594,769,772,- -32768,2538,2538,534,- -32768,2538,2538,543,2538,549,4198,-
14,1367,- -32768,4281,- -32768,638,574,767,4281,770,774,- -32768,- -32768,- -32768,
4281,764,778,- -32768,- -32768,- -32768,- -32768,2538,- -32768,- -32768,2538,- -32768,
2538,2538,570,4281,998,- -32768,573,- -32768,- -32768,785,345,776,4895,357,1367,-
-32768,- -32768,- -32768,- -32768,- -32768,2538,364,540,- -32768,2538,779,- -32768,-
-32768,780,1121,- -32768,- -32768,998,- -32768,- -32768,- -32768,783,594,- -32768,540,
- -32768,- -32768,- -32768,- -32768,878,881,- -32768};static short Cyc_yypgoto[114]={-
-32768,92,- -32768,558,- -32768,- -32768,- -32768,- -32768,- -32768,- 53,- 39,- 57,-
-32768,113,- 41,317,- -32768,85,- -32768,- 13,- -32768,- -32768,- 111,- -32768,200,-
-32768,361,- -32768,- -32768,- -32768,675,664,124,- -32768,- -32768,365,- -32768,-
-32768,41,- -32768,- -32768,6,- 40,64,- -32768,- -32768,848,- -32768,- 94,26,- 116,- 422,
152,333,334,612,- 419,- 88,- 292,797,- -32768,- 207,- 155,- 515,- 284,- -32768,456,- 177,-
76,- 114,- 118,- 204,- 32,- -32768,- -32768,12,- 216,- -32768,- 583,- 631,- -32768,- -32768,
- 22,775,327,- -32768,179,59,- -32768,- 142,- 367,- 154,583,584,589,581,590,341,184,
338,339,- 52,455,- -32768,- -32768,- -32768,- 325,- -32768,- -32768,8,885,- -32768,- 354,
- 153};static short Cyc_yytable[5003]={119,83,308,162,117,271,335,466,62,133,389,
154,155,321,322,279,576,124,118,125,533,582,489,633,536,318,285,270,542,1,69,84,
304,85,302,425,426,129,428,229,453,304,90,312,62,342,119,220,221,222,117,62,535,
138,103,599,62,601,142,253,782,119,108,252,114,264,356,62,357,120,128,64,548,549,
151,151,136,445,151,305,306,405,113,84,124,343,113,380,84,306,350,351,565,406,
151,783,146,483,592,147,381,32,421,62,62,62,62,483,151,151,591,690,454,111,382,
219,151,31,507,740,228,422,446,65,34,266,451,62,352,434,265,36,86,338,339,94,340,
353,490,62,491,573,458,87,416,136,598,84,417,299,489,191,210,373,506,84,443,300,
305,88,32,119,104,105,106,264,435,310,- 114,62,151,457,150,150,- 178,295,150,817,
636,31,- 178,720,277,674,32,821,34,211,212,213,214,91,150,305,253,253,111,253,252,
252,802,252,430,408,95,96,150,150,431,415,358,151,419,420,150,161,301,473,812,
359,360,156,266,280,62,311,31,157,819,89,392,226,399,34,62,822,719,402,92,278,
432,722,16,17,18,161,93,31,97,98,269,156,677,458,34,- 156,678,157,68,427,- 156,62,
487,297,298,448,594,499,150,469,131,125,32,129,464,465,391,253,65,294,151,252,
137,121,151,151,151,65,122,614,136,123,484,348,349,436,151,485,32,84,486,151,151,
140,151,575,529,530,531,150,227,313,314,466,230,90,269,62,151,62,431,596,442,458,
290,597,132,632,623,682,612,436,62,299,613,139,162,787,39,40,41,300,449,39,40,41,
291,31,226,354,355,463,119,33,34,35,559,431,107,482,562,119,336,301,141,566,431,
143,605,583,119,606,723,311,117,640,330,144,150,391,145,641,150,150,150,501,502,
503,570,567,604,569,651,62,150,62,510,215,431,150,150,707,150,458,431,693,681,
431,630,694,682,151,710,374,375,376,150,216,431,414,151,476,477,161,752,634,461,
462,401,151,431,532,151,161,160,756,510,584,161,571,590,431,487,735,593,431,805,
736,282,761,431,377,682,675,472,462,627,658,378,379,62,295,431,509,431,663,808,
686,687,431,84,151,151,811,84,731,510,540,231,541,281,695,283,510,665,666,667,
681,692,286,572,284,151,289,336,548,549,293,659,317,295,323,497,324,500,617,325,
62,504,150,336,615,431,119,585,618,431,264,150,622,431,326,514,1,642,431,151,150,
327,681,150,328,1,151,521,522,523,524,784,644,431,645,431,329,586,608,701,431,
161,337,399,341,62,344,1,391,762,161,537,639,119,151,713,714,117,345,161,801,346,
150,150,732,431,741,431,469,347,809,743,431,400,538,657,553,383,555,556,384,560,
386,150,403,816,747,431,818,393,391,765,314,546,404,547,436,407,84,409,759,161,
737,700,31,410,703,807,1,767,768,34,316,31,99,319,319,150,36,411,34,412,219,423,
150,332,333,36,39,40,41,774,431,319,319,31,319,39,40,41,777,431,34,418,151,- 175,
779,431,424,309,- 175,- 175,150,319,429,- 175,217,300,432,39,40,41,439,226,1,391,
742,799,431,746,803,431,444,685,698,699,440,151,391,757,305,691,519,520,753,525,
526,447,527,528,253,452,460,474,252,475,31,342,505,766,508,539,543,34,772,773,
544,545,775,776,36,778,554,151,84,557,395,561,563,253,39,40,41,252,568,578,643,
574,577,253,648,650,319,252,391,795,580,587,796,588,797,798,150,589,253,32,391,
253,252,600,602,252,31,603,609,611,625,610,616,34,810,619,- 204,620,813,621,309,-
204,- 204,624,646,151,- 204,628,300,150,629,70,631,638,652,161,653,654,670,655,269,
656,391,660,319,683,319,319,319,319,319,319,319,319,319,319,319,319,319,319,319,
319,319,662,668,150,672,673,161,676,626,684,704,706,679,709,688,712,689,696,697,
717,31,72,702,73,74,716,718,34,721,724,734,725,726,727,75,319,728,76,77,729,738,
464,465,730,78,733,70,739,754,79,80,755,760,758,763,770,771,788,793,790,804,150,
791,824,794,806,825,498,814,815,1,745,820,394,749,751,388,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,669,637,127,635,786,661,71,72,664,73,74,488,273,34,319,22,292,
595,671,513,75,517,515,76,77,781,27,28,516,785,78,518,0,109,789,79,80,29,30,0,
792,0,361,362,363,364,365,366,367,368,369,370,0,0,0,800,0,319,0,0,0,0,0,0,267,0,
0,31,371,0,0,0,32,33,34,35,0,269,0,0,0,309,- 156,0,37,38,0,- 156,0,300,0,39,40,41,
319,2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,713,714,21,164,165,232,0,
233,234,235,236,237,238,239,240,22,166,23,167,241,0,168,24,242,0,0,27,28,243,169,
170,244,245,171,172,173,174,29,30,246,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,
319,0,0,0,0,0,0,0,319,0,247,179,180,181,182,32,33,34,35,0,248,116,0,0,183,0,0,37,
250,0,0,0,0,0,185,0,0,186,0,187,188,0,0,0,0,189,190,2,3,4,115,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,767,768,21,164,165,232,0,233,234,235,236,237,238,239,240,
22,166,23,167,241,0,168,24,242,0,0,27,28,243,169,170,244,245,171,172,173,174,29,
30,246,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,247,179,180,
181,182,32,33,34,35,0,248,116,0,0,183,0,0,37,250,0,0,0,0,0,185,0,0,186,0,187,188,
0,0,0,0,189,190,2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,164,
165,232,0,233,234,235,236,237,238,239,240,22,166,23,167,241,0,168,24,242,0,0,27,
28,243,169,170,244,245,171,172,173,174,29,30,246,175,0,176,0,177,178,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,247,179,180,181,182,32,33,34,35,0,248,116,249,0,
183,0,0,37,250,0,0,0,0,0,185,0,0,186,0,187,188,0,0,0,0,189,190,2,3,4,115,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,164,165,232,0,233,234,235,236,237,238,
239,240,22,166,23,167,241,0,168,24,242,0,0,27,28,243,169,170,244,245,171,172,173,
174,29,30,246,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,247,
179,180,181,182,32,33,34,35,0,248,116,0,0,183,0,0,37,250,0,0,0,0,0,185,0,0,186,0,
187,188,0,0,0,0,189,190,2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
21,164,165,0,0,0,0,0,0,0,0,0,0,22,166,23,167,0,0,168,24,0,0,0,27,28,0,169,170,0,
0,171,172,173,174,29,30,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,31,179,180,181,182,32,33,34,35,0,558,0,0,0,183,0,0,37,250,0,0,0,0,0,185,
0,0,186,0,187,188,0,0,0,0,189,190,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,
164,165,0,0,0,0,0,0,0,0,0,0,22,166,0,167,0,0,168,0,0,0,0,27,28,0,169,170,0,0,171,
172,173,174,29,30,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,31,179,180,181,182,32,33,34,35,0,0,334,0,0,183,0,0,37,250,0,0,0,0,0,185,0,0,
186,0,187,188,0,0,0,0,189,190,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,164,
165,0,0,0,0,0,0,0,0,0,0,22,166,0,167,0,0,168,0,0,0,0,27,28,0,169,170,0,0,171,172,
173,174,29,30,0,175,0,176,0,177,178,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,0,0,0,31,179,180,181,182,32,33,34,35,0,22,0,0,0,183,0,0,37,250,0,0,27,28,0,185,
0,0,186,0,187,188,0,29,30,0,189,190,- 8,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,267,0,21,268,0,0,0,0,32,33,0,35,0,269,0,22,0,23,0,0,37,38,24,0,25,26,
27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,31,0,0,0,0,32,33,34,35,0,0,0,- 8,0,36,0,0,37,38,0,0,0,0,0,39,40,41,- 8,1,0,2,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,
0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,32,33,34,35,0,0,0,0,0,36,0,0,37,38,0,0,
0,0,0,39,40,41,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,
0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,0,1,0,
2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,31,0,0,0,0,32,33,
34,35,22,0,0,- 8,0,36,0,24,37,38,0,27,28,0,0,39,40,41,0,0,0,0,29,30,0,0,0,0,0,0,0,
0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,31,0,0,0,0,32,33,34,35,22,0,
0,0,0,36,0,0,37,38,0,27,28,0,0,39,40,41,0,0,0,0,29,30,0,0,0,2,3,4,115,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,21,267,0,0,0,0,0,0,0,32,33,0,35,22,269,23,0,0,
299,0,24,37,38,0,27,28,300,0,39,40,41,0,0,0,0,29,30,0,0,0,0,0,0,2,3,4,115,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,32,33,0,35,0,0,116,22,217,23,
0,0,37,38,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,2,3,4,115,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,32,33,0,35,0,0,116,22,0,23,0,0,
37,38,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,70,0,0,2,3,4,115,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,32,33,0,35,22,0,0,0,0,0,0,24,37,
38,0,27,28,0,31,72,0,73,74,0,0,34,29,30,0,0,0,0,75,0,0,76,77,0,0,0,0,0,78,0,0,0,
0,79,80,0,0,0,0,0,0,0,0,0,0,0,0,32,33,0,35,0,0,0,0,0,0,0,0,37,38,164,165,232,0,
233,234,235,236,237,238,239,240,0,166,0,167,241,0,168,0,242,0,0,0,0,243,169,170,
244,245,171,172,173,174,0,0,246,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,247,179,180,181,182,0,0,34,0,0,248,116,0,0,183,0,0,0,184,164,165,0,
0,0,185,0,492,186,0,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,
171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,31,179,180,181,182,0,0,34,0,0,0,320,493,0,183,0,0,0,184,164,165,464,465,0,
185,0,0,186,0,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,
173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,
179,180,181,182,0,0,34,0,0,0,320,680,0,183,0,0,0,184,164,165,464,465,0,185,0,0,
186,0,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,
0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,
181,182,0,0,34,0,0,0,320,764,0,183,0,0,0,184,164,165,464,465,0,185,0,0,186,0,187,
188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,
0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,
34,0,0,0,320,0,0,183,0,0,0,184,164,165,464,465,0,185,0,0,186,0,187,188,0,166,0,
167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,
178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,0,
0,0,183,0,0,0,184,164,165,0,0,276,185,0,0,186,0,187,188,0,166,0,167,189,190,168,
0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,320,0,0,183,0,0,0,
184,164,165,0,0,0,185,0,0,186,0,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,
169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,413,0,0,0,183,0,0,0,184,164,165,0,
0,0,185,0,0,186,0,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,
172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
31,179,180,181,182,0,0,34,0,0,0,0,0,0,183,0,0,0,184,164,165,0,0,481,185,0,0,186,
0,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,
0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,
182,0,0,34,0,0,0,0,0,0,183,534,0,0,184,164,165,0,0,0,185,0,0,186,0,187,188,0,166,
0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,
177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,
0,0,0,0,183,564,0,0,184,164,165,0,0,0,185,0,0,186,0,187,188,0,166,0,167,189,190,
168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,0,0,0,183,0,0,
0,184,164,165,0,0,607,185,0,0,186,0,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,
0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,647,0,0,0,183,0,0,0,184,164,165,
0,0,0,185,0,0,186,0,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,
171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,31,179,180,181,182,0,0,34,0,0,649,0,0,0,183,0,0,0,184,164,165,0,0,0,185,0,0,
186,0,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,
0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,
181,182,0,0,34,0,0,0,0,0,0,183,705,0,0,184,164,165,0,0,0,185,0,0,186,0,187,188,0,
166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,
0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,
0,0,0,0,0,183,708,0,0,184,164,165,0,0,0,185,0,0,186,0,187,188,0,166,0,167,189,
190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,711,0,0,0,
183,0,0,0,184,164,165,0,0,0,185,0,0,186,0,187,188,0,166,0,167,189,190,168,0,0,0,
0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,0,0,0,183,744,0,0,184,
164,165,0,0,0,185,0,0,186,0,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,
170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,0,0,0,183,748,0,0,184,164,165,0,0,0,
185,0,0,186,0,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,
173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,
179,180,181,182,0,0,34,0,0,0,0,0,0,183,750,0,0,184,164,165,0,0,0,185,0,0,186,0,
187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,
175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,
0,0,34,0,0,0,0,0,0,183,780,0,0,184,164,165,0,0,0,185,0,0,186,0,187,188,0,166,0,
167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,
178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,0,
164,165,183,0,0,0,184,0,0,0,0,0,185,166,0,186,0,187,188,0,0,0,0,189,190,0,169,
170,0,0,171,172,173,174,0,0,0,0,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,320,164,165,183,0,0,0,184,0,0,0,0,0,
185,166,0,186,0,187,188,0,0,0,0,189,190,0,169,170,0,0,171,172,173,174,0,0,0,0,0,
176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,
34,0,0,0,626,164,165,183,0,0,0,184,0,0,0,0,0,185,166,0,186,0,187,188,0,0,0,0,189,
190,0,169,170,0,0,171,172,173,174,0,0,0,0,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,0,164,165,315,0,0,0,184,0,0,
0,0,0,185,166,0,186,0,187,188,0,0,0,0,189,190,0,169,170,0,0,171,172,173,174,0,0,
0,0,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,
0,0,34,0,0,0,0,164,165,331,0,0,0,184,0,0,0,0,0,185,166,0,186,0,187,188,0,0,0,0,
189,190,0,169,170,0,0,171,172,173,174,0,0,0,0,0,176,0,177,178,0,0,0,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,22,0,0,183,0,
0,0,184,0,0,0,27,28,185,0,0,186,0,187,188,0,0,29,30,189,190,148,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,32,33,0,35,0,
0,149,0,0,0,27,28,37,38,22,0,0,0,0,0,0,29,30,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,
0,0,0,0,0,0,0,0,0,267,0,0,0,0,0,0,0,32,33,0,35,0,269,0,581,0,0,0,0,37,38,0,32,33,
0,35,0,0,0,0,0,0,0,0,37,38,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,33,0,35,0,0,0,0,0,0,0,0,
37,38};static short Cyc_yycheck[5003]={57,23,155,91,57,121,183,291,0,66,217,87,88,
168,168,129,435,58,57,59,374,443,314,538,378,167,137,121,395,1,22,23,69,25,152,
251,252,108,254,110,17,69,36,161,36,75,103,104,105,106,103,43,377,75,48,474,48,
479,80,116,74,118,54,116,56,118,116,59,118,57,62,91,5,6,87,88,68,105,91,116,117,
91,56,75,125,115,60,87,80,117,70,71,417,103,107,109,100,301,465,103,100,94,89,95,
96,97,98,311,121,122,464,626,89,106,114,103,129,89,334,702,112,108,275,98,96,119,
279,119,108,63,118,103,89,185,186,43,188,117,315,131,317,433,284,108,99,137,104,
139,103,103,442,92,93,205,331,147,272,111,116,108,94,218,49,50,51,218,100,161,
101,161,183,282,87,88,99,149,91,808,545,89,105,104,123,605,94,816,96,95,96,97,98,
103,107,116,251,252,106,254,251,252,783,254,99,235,99,100,121,122,105,241,113,
224,244,245,129,91,152,293,801,122,123,89,216,131,216,161,89,95,811,88,218,107,
226,96,226,818,658,229,100,126,89,663,17,18,19,121,100,89,99,100,99,89,90,395,96,
104,610,95,100,253,109,253,303,150,151,278,102,324,183,291,100,311,94,108,110,
111,217,334,98,101,293,334,100,103,297,298,299,98,108,496,282,111,103,72,73,269,
309,108,94,291,111,314,315,89,317,101,358,359,360,224,110,104,105,597,114,309,99,
309,331,311,105,101,105,465,145,105,103,112,505,613,101,305,324,103,105,103,424,
759,113,114,115,111,278,113,114,115,146,89,224,68,69,99,409,95,96,97,409,105,100,
300,99,418,183,299,90,418,105,99,484,445,427,485,664,309,427,99,176,105,293,320,
102,105,297,298,299,326,327,328,427,421,484,423,99,385,309,387,337,99,105,314,
315,99,317,545,105,101,613,105,109,105,694,424,99,65,66,67,331,105,105,240,433,
297,298,299,99,539,104,105,228,442,105,372,445,309,104,99,377,450,314,427,462,
105,483,101,466,105,101,105,105,109,105,103,736,606,104,105,508,573,110,111,452,
435,105,104,105,581,109,622,622,105,462,484,485,109,466,682,417,385,91,387,103,
634,101,424,89,90,91,694,630,104,431,102,505,89,315,5,6,103,574,103,474,103,323,
103,325,499,103,499,329,424,331,104,105,570,450,104,105,570,433,104,105,103,343,
1,104,105,539,442,103,736,445,103,1,546,350,351,352,353,754,104,105,104,105,103,
452,486,104,105,424,103,544,101,544,74,1,496,733,433,379,551,617,574,22,23,617,
120,442,783,121,484,485,104,105,104,105,597,119,793,104,105,100,381,570,404,101,
406,407,101,409,101,505,100,808,104,105,811,101,538,104,105,400,103,402,577,103,
597,103,723,484,697,642,89,89,645,791,1,22,23,96,164,89,99,167,168,539,103,99,96,
99,617,109,546,177,178,103,113,114,115,104,105,185,186,89,188,113,114,115,104,
105,96,103,664,99,104,105,103,103,104,105,574,205,101,109,102,111,89,113,114,115,
104,546,1,613,705,104,105,708,104,105,104,621,640,641,109,697,626,718,116,629,
348,349,713,354,355,112,356,357,754,101,101,105,754,101,89,75,103,738,104,108,99,
96,743,744,105,109,747,748,103,750,103,733,713,30,109,99,42,783,113,114,115,783,
94,94,554,103,116,793,558,559,284,793,682,774,104,103,777,104,779,780,664,105,
808,94,694,811,808,104,104,811,89,112,104,89,101,105,104,96,799,105,99,105,803,
105,103,104,105,104,103,791,109,104,111,697,105,38,104,101,100,664,99,104,112,99,
99,117,736,101,342,619,344,345,346,347,348,349,350,351,352,353,354,355,356,357,
358,359,360,108,104,733,104,104,697,112,100,25,646,647,108,649,104,651,104,101,
103,655,89,90,100,92,93,99,89,96,94,105,105,104,104,104,103,395,104,106,107,104,
28,110,111,679,113,103,38,100,109,118,119,101,90,105,25,101,99,105,109,104,90,
791,103,0,101,104,0,324,104,104,1,707,104,224,710,711,216,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,589,546,60,544,758,577,89,90,581,92,93,305,121,96,465,37,147,
467,597,342,103,346,344,106,107,752,48,49,345,756,113,347,- 1,54,761,118,119,59,
60,- 1,767,- 1,76,77,78,79,80,81,82,83,84,85,- 1,- 1,- 1,782,- 1,508,- 1,- 1,- 1,- 1,- 1,- 1,
86,- 1,- 1,89,102,- 1,- 1,- 1,94,95,96,97,- 1,99,- 1,- 1,- 1,103,104,- 1,106,107,- 1,109,- 1,
111,- 1,113,114,115,545,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,
24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,
51,52,53,54,55,56,57,58,59,60,61,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,622,- 1,- 1,- 1,- 1,- 1,- 1,- 1,630,- 1,89,90,91,92,93,94,95,96,97,- 1,99,100,- 1,- 1,
103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,
33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,
60,61,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,89,90,91,92,93,94,95,96,97,- 1,99,100,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,-
1,113,- 1,- 1,116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,
43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,- 1,64,- 1,66,67,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,
97,- 1,99,100,101,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,
- 1,- 1,- 1,124,125,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,
26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,
53,54,55,56,57,58,59,60,61,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,97,- 1,99,100,- 1,- 1,103,- 1,- 1,
106,107,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,37,38,39,40,- 1,- 1,43,44,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,- 1,62,
- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,
90,91,92,93,94,95,96,97,- 1,99,- 1,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,113,- 1,-
1,116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,
- 1,51,52,- 1,- 1,55,56,57,58,59,60,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,97,- 1,- 1,100,- 1,- 1,
103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,- 1,62,
- 1,64,- 1,66,67,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,89,
90,91,92,93,94,95,96,97,- 1,37,- 1,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,48,49,- 1,113,- 1,-
1,116,- 1,118,119,- 1,59,60,- 1,124,125,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,86,- 1,24,89,- 1,- 1,- 1,- 1,94,95,- 1,97,- 1,99,- 1,37,- 1,39,- 1,- 1,106,
107,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,
96,97,- 1,- 1,- 1,101,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,113,114,115,0,1,- 1,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,89,- 1,- 1,- 1,- 1,94,95,96,97,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,
113,114,115,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,24,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,96,97,37,- 1,- 1,101,- 1,103,- 1,44,106,
107,- 1,48,49,- 1,- 1,113,114,115,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,96,97,37,
- 1,- 1,- 1,- 1,103,- 1,- 1,106,107,- 1,48,49,- 1,- 1,113,114,115,- 1,- 1,- 1,- 1,59,60,- 1,- 1,
- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,86,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,94,95,- 1,97,37,99,39,- 1,- 1,103,- 1,44,106,107,- 1,48,49,111,- 1,113,114,115,-
1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,94,95,- 1,97,- 1,- 1,100,37,102,39,- 1,- 1,106,107,44,-
1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,94,95,- 1,97,- 1,- 1,
100,37,- 1,39,- 1,- 1,106,107,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,38,
- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,94,95,- 1,97,37,- 1,- 1,- 1,- 1,- 1,- 1,44,106,107,- 1,48,49,- 1,89,90,- 1,92,93,
- 1,- 1,96,59,60,- 1,- 1,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,- 1,- 1,118,
119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,
107,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,43,- 1,45,- 1,- 1,- 1,- 1,
50,51,52,53,54,55,56,57,58,- 1,- 1,61,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,99,100,- 1,- 1,
103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,32,116,- 1,118,119,- 1,38,- 1,40,124,125,43,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,
- 1,- 1,100,101,- 1,103,- 1,- 1,- 1,107,25,26,110,111,- 1,113,- 1,- 1,116,- 1,118,119,- 1,
38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,
64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,
91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,101,- 1,103,- 1,- 1,- 1,107,25,26,110,111,- 1,113,- 1,-
1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,
57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,101,- 1,103,- 1,- 1,- 1,107,25,
26,110,111,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,- 1,- 1,
103,- 1,- 1,- 1,107,25,26,110,111,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,
43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,
- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,112,113,- 1,- 1,116,- 1,118,119,- 1,
38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,
64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,
91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,- 1,
116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,
58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,99,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,
- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,
- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,-
1,107,25,26,- 1,- 1,112,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,-
1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,
- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,
125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,
- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,
119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,112,113,-
1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,
56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,99,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,
26,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,99,- 1,- 1,- 1,103,-
1,- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,
- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,
124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,
67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,
- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,
119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,- 1,96,- 1,- 1,99,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,
- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,
57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,
- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,
52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,
- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,-
1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,
- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,
124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,
67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,
- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,
119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,25,26,103,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,113,38,
- 1,116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,
64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,
91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,25,26,103,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,113,38,- 1,
116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,
- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,
92,93,- 1,- 1,96,- 1,- 1,- 1,100,25,26,103,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,113,38,- 1,116,
- 1,118,119,- 1,- 1,- 1,- 1,124,125,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,
66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,
93,- 1,- 1,96,- 1,- 1,- 1,- 1,25,26,103,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,113,38,- 1,116,- 1,
118,119,- 1,- 1,- 1,- 1,124,125,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,
67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,
- 1,- 1,96,- 1,- 1,- 1,- 1,25,26,103,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,113,38,- 1,116,- 1,118,
119,- 1,- 1,- 1,- 1,124,125,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,
- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,
96,- 1,- 1,- 1,37,- 1,- 1,103,- 1,- 1,- 1,107,- 1,- 1,- 1,48,49,113,- 1,- 1,116,- 1,118,119,- 1,
- 1,59,60,124,125,63,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,37,94,95,- 1,97,- 1,- 1,100,- 1,- 1,- 1,48,49,106,107,37,- 1,- 1,
- 1,- 1,- 1,- 1,59,60,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,- 1,97,- 1,99,- 1,86,- 1,- 1,- 1,- 1,106,107,
- 1,94,95,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,- 1,97,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,106,107};unsigned char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible";
unsigned char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow";void Cyc_yyerror(
struct _tagged_arr);int Cyc_yylex();static int Cyc_yychar=(int)'\000';void*Cyc_yylval=(
void*)Cyc_YYINITIALSVAL;static int Cyc_yynerrs=0;struct _tuple15{struct _tagged_arr
f1;void*f2;};static unsigned char _tmp389[8]="stdcall";static unsigned char _tmp38A[
6]="cdecl";static unsigned char _tmp38B[9]="fastcall";static unsigned char _tmp38C[9]="noreturn";
static unsigned char _tmp38D[6]="const";static unsigned char _tmp38E[8]="aligned";
static unsigned char _tmp38F[7]="packed";static unsigned char _tmp390[7]="shared";
static unsigned char _tmp391[7]="unused";static unsigned char _tmp392[5]="weak";
static unsigned char _tmp393[10]="dllimport";static unsigned char _tmp394[10]="dllexport";
static unsigned char _tmp395[23]="no_instrument_function";static unsigned char
_tmp396[12]="constructor";static unsigned char _tmp397[11]="destructor";static
unsigned char _tmp398[22]="no_check_memory_usage";struct _tuple16{void*f1;void*f2;
};struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(){
int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int yyssp_offset;short*yyss=(
short*)({unsigned int _tmp839=10000;short*_tmp83A=(short*)_cycalloc_atomic(
_check_times(sizeof(short),_tmp839));{unsigned int _tmp83B=_tmp839;unsigned int i;
for(i=0;i < _tmp83B;i ++){_tmp83A[i]=(short)0;}}_tmp83A;});int yyvsp_offset;void**
yyvs=(void**)({unsigned int _tmp836=10000;void**_tmp837=(void**)_cycalloc(
_check_times(sizeof(void*),_tmp836));{unsigned int _tmp838=_tmp836;unsigned int i;
for(i=0;i < _tmp838;i ++){_tmp837[i]=Cyc_yylval;}}_tmp837;});int yylsp_offset;
struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmp833=10000;struct Cyc_Yyltype*
_tmp834=(struct Cyc_Yyltype*)_cycalloc_atomic(_check_times(sizeof(struct Cyc_Yyltype),
_tmp833));{unsigned int _tmp835=_tmp833;unsigned int i;for(i=0;i < _tmp835;i ++){
_tmp834[i]=Cyc_yynewloc();}}_tmp834;});int yystacksize=10000;void*yyval=Cyc_yylval;
int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;
yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(
10000,++ yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(
yystacksize >= 10000){Cyc_yyerror(_tag_arr("parser stack overflow",sizeof(
unsigned char),22));(int)_throw((void*)Cyc_Yystack_overflow);}}goto yybackup;
yybackup: yyn=(int)Cyc_yypact[yystate];if(yyn == - 32768)goto yydefault;if(Cyc_yychar
== - 2)Cyc_yychar=Cyc_yylex();if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{
yychar1=(Cyc_yychar > 0?Cyc_yychar <= 353: 0)?(int)Cyc_yytranslate[
_check_known_subscript_notnull(354,Cyc_yychar)]: 240;}yyn +=yychar1;if((yyn < 0?1:
yyn > 5002)?1: Cyc_yycheck[yyn]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){if(yyn == - 32768)goto yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)
goto yyerrlab;}if(yyn == 825)return 0;if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)yyerrstatus --;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
yystate];if(yyn == 0)goto yyerrlab;yyreduce: yylen=(int)Cyc_yyr2[yyn];if(yylen > 0)
yyval=yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];
switch(yyn){case 1: _LL1B9: yyval=yyvs[yyvsp_offset];Cyc_Parse_parse_result=Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]);break;case 2: _LL1BA: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp302=_cycalloc(sizeof(*_tmp302));_tmp302[0]=({struct Cyc_DeclList_tok_struct
_tmp303;_tmp303.tag=Cyc_DeclList_tok;_tmp303.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]));_tmp303;});_tmp302;});break;case 3: _LL1BB: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp304=_cycalloc(sizeof(*_tmp304));_tmp304[0]=({
struct Cyc_DeclList_tok_struct _tmp305;_tmp305.tag=Cyc_DeclList_tok;_tmp305.f1=({
struct Cyc_List_List*_tmp306=_cycalloc(sizeof(*_tmp306));_tmp306->hd=({struct Cyc_Absyn_Decl*
_tmp307=_cycalloc(sizeof(*_tmp307));_tmp307->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp308=_cycalloc(sizeof(*_tmp308));_tmp308[0]=({struct Cyc_Absyn_Using_d_struct
_tmp309;_tmp309.tag=9;_tmp309.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp309.f2=Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]);_tmp309;});_tmp308;}));_tmp307->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp307;});_tmp306->tl=0;_tmp306;});_tmp305;});_tmp304;});
Cyc_Lex_leave_using();break;case 4: _LL1BC: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp30A=_cycalloc(sizeof(*_tmp30A));_tmp30A[0]=({struct Cyc_DeclList_tok_struct
_tmp30B;_tmp30B.tag=Cyc_DeclList_tok;_tmp30B.f1=({struct Cyc_List_List*_tmp30C=
_cycalloc(sizeof(*_tmp30C));_tmp30C->hd=({struct Cyc_Absyn_Decl*_tmp30D=_cycalloc(
sizeof(*_tmp30D));_tmp30D->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp30E=_cycalloc(sizeof(*_tmp30E));_tmp30E[0]=({struct Cyc_Absyn_Using_d_struct
_tmp30F;_tmp30F.tag=9;_tmp30F.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp30F.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp30F;});_tmp30E;}));
_tmp30D->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp30D;});_tmp30C->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp30C;});_tmp30B;});_tmp30A;});break;case 5: _LL1BD: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp310=_cycalloc(sizeof(*_tmp310));_tmp310[0]=({
struct Cyc_DeclList_tok_struct _tmp311;_tmp311.tag=Cyc_DeclList_tok;_tmp311.f1=({
struct Cyc_List_List*_tmp312=_cycalloc(sizeof(*_tmp312));_tmp312->hd=({struct Cyc_Absyn_Decl*
_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp315;_tmp315.tag=8;_tmp315.f1=({struct _tagged_arr*_tmp316=_cycalloc(sizeof(*
_tmp316));_tmp316[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp316;});_tmp315.f2=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp315;});_tmp314;}));_tmp313->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp313;});_tmp312->tl=0;_tmp312;});_tmp311;});_tmp310;});
Cyc_Lex_leave_namespace();break;case 6: _LL1BE: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317[0]=({struct Cyc_DeclList_tok_struct
_tmp318;_tmp318.tag=Cyc_DeclList_tok;_tmp318.f1=({struct Cyc_List_List*_tmp319=
_cycalloc(sizeof(*_tmp319));_tmp319->hd=({struct Cyc_Absyn_Decl*_tmp31A=_cycalloc(
sizeof(*_tmp31A));_tmp31A->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp31B=_cycalloc(sizeof(*_tmp31B));_tmp31B[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp31C;_tmp31C.tag=8;_tmp31C.f1=({struct _tagged_arr*_tmp31D=_cycalloc(sizeof(*
_tmp31D));_tmp31D[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp31D;});_tmp31C.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp31C;});_tmp31B;}));
_tmp31A->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp31A;});_tmp319->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp319;});_tmp318;});_tmp317;});break;case 7: _LL1BF: if(Cyc_Std_strcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tag_arr("C",sizeof(unsigned char),2))!= 0)Cyc_Parse_err(_tag_arr("only extern \"C\" { ... } is allowed",
sizeof(unsigned char),35),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp31E=_cycalloc(sizeof(*_tmp31E));_tmp31E[0]=({
struct Cyc_DeclList_tok_struct _tmp31F;_tmp31F.tag=Cyc_DeclList_tok;_tmp31F.f1=({
struct Cyc_List_List*_tmp320=_cycalloc(sizeof(*_tmp320));_tmp320->hd=({struct Cyc_Absyn_Decl*
_tmp321=_cycalloc(sizeof(*_tmp321));_tmp321->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp323;_tmp323.tag=10;_tmp323.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp323;});_tmp322;}));
_tmp321->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp321;});_tmp320->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp320;});_tmp31F;});_tmp31E;});break;case 8: _LL1C0: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324[0]=({
struct Cyc_DeclList_tok_struct _tmp325;_tmp325.tag=Cyc_DeclList_tok;_tmp325.f1=0;
_tmp325;});_tmp324;});break;case 9: _LL1C1: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp326=_cycalloc(sizeof(*_tmp326));_tmp326[0]=({struct Cyc_DeclList_tok_struct
_tmp327;_tmp327.tag=Cyc_DeclList_tok;_tmp327.f1=({struct Cyc_List_List*_tmp328=
_cycalloc(sizeof(*_tmp328));_tmp328->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp329=_cycalloc(sizeof(*_tmp329));_tmp329[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp32A;_tmp32A.tag=1;_tmp32A.f1=Cyc_yyget_FnDecl_tok(yyvs[yyvsp_offset]);
_tmp32A;});_tmp329;}),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp328->tl=0;_tmp328;});_tmp327;});_tmp326;});
break;case 10: _LL1C2: yyval=yyvs[yyvsp_offset];break;case 11: _LL1C3: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B[0]=({
struct Cyc_FnDecl_tok_struct _tmp32C;_tmp32C.tag=Cyc_FnDecl_tok;_tmp32C.f1=Cyc_Parse_make_function(
0,Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp32C;});_tmp32B;});break;case 12: _LL1C4: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp32D=_cycalloc(sizeof(*_tmp32D));_tmp32D[0]=({
struct Cyc_FnDecl_tok_struct _tmp32E;_tmp32E.tag=Cyc_FnDecl_tok;_tmp32E.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp32F=_cycalloc(sizeof(*_tmp32F));_tmp32F->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp32F;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp32E;});_tmp32D;});break;case 13: _LL1C5: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp330=_cycalloc(sizeof(*_tmp330));_tmp330[0]=({
struct Cyc_FnDecl_tok_struct _tmp331;_tmp331.tag=Cyc_FnDecl_tok;_tmp331.f1=Cyc_Parse_make_function(
0,Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp331;});_tmp330;});break;case 14: _LL1C6: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp332=_cycalloc(sizeof(*_tmp332));_tmp332[0]=({
struct Cyc_FnDecl_tok_struct _tmp333;_tmp333.tag=Cyc_FnDecl_tok;_tmp333.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp334=_cycalloc(sizeof(*_tmp334));_tmp334->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp334;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp333;});_tmp332;});break;case 15: _LL1C7: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp335=_cycalloc(sizeof(*_tmp335));_tmp335[0]=({
struct Cyc_FnDecl_tok_struct _tmp336;_tmp336.tag=Cyc_FnDecl_tok;_tmp336.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp337=_cycalloc(sizeof(*_tmp337));_tmp337->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp337;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp336;});_tmp335;});break;case 16: _LL1C8: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp338=_cycalloc(sizeof(*_tmp338));_tmp338[0]=({
struct Cyc_FnDecl_tok_struct _tmp339;_tmp339.tag=Cyc_FnDecl_tok;_tmp339.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp33A=_cycalloc(sizeof(*_tmp33A));_tmp33A->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp33A;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp339;});_tmp338;});break;case 17: _LL1C9: Cyc_Lex_enter_using(
Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]));yyval=yyvs[yyvsp_offset];break;case 18:
_LL1CA: Cyc_Lex_leave_using();break;case 19: _LL1CB: Cyc_Lex_enter_namespace(({
struct _tagged_arr*_tmp33B=_cycalloc(sizeof(*_tmp33B));_tmp33B[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp33B;}));yyval=yyvs[yyvsp_offset];break;case 20: _LL1CC: Cyc_Lex_leave_namespace();
break;case 21: _LL1CD: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp33C=
_cycalloc(sizeof(*_tmp33C));_tmp33C[0]=({struct Cyc_DeclList_tok_struct _tmp33D;
_tmp33D.tag=Cyc_DeclList_tok;_tmp33D.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp33D;});
_tmp33C;});break;case 22: _LL1CE: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp33E=_cycalloc(sizeof(*_tmp33E));_tmp33E[0]=({struct Cyc_DeclList_tok_struct
_tmp33F;_tmp33F.tag=Cyc_DeclList_tok;_tmp33F.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp33F;});_tmp33E;});break;case 23: _LL1CF: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp340=_cycalloc(sizeof(*_tmp340));_tmp340[0]=({
struct Cyc_DeclList_tok_struct _tmp341;_tmp341.tag=Cyc_DeclList_tok;_tmp341.f1=({
struct Cyc_List_List*_tmp342=_cycalloc(sizeof(*_tmp342));_tmp342->hd=Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
0,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp342->tl=0;
_tmp342;});_tmp341;});_tmp340;});break;case 24: _LL1D0: {struct Cyc_List_List*
_tmp343=0;{struct Cyc_List_List*_tmp344=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp344 != 0;
_tmp344=_tmp344->tl){struct _tagged_arr*_tmp345=(struct _tagged_arr*)_tmp344->hd;
struct _tuple1*_tmp346=({struct _tuple1*_tmp349=_cycalloc(sizeof(*_tmp349));
_tmp349->f1=Cyc_Absyn_rel_ns_null;_tmp349->f2=_tmp345;_tmp349;});struct Cyc_Absyn_Vardecl*
_tmp347=Cyc_Absyn_new_vardecl(_tmp346,Cyc_Absyn_wildtyp(0),0);_tmp343=({struct
Cyc_List_List*_tmp348=_cycalloc(sizeof(*_tmp348));_tmp348->hd=_tmp347;_tmp348->tl=
_tmp343;_tmp348;});}}_tmp343=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp343);yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp34A=
_cycalloc(sizeof(*_tmp34A));_tmp34A[0]=({struct Cyc_DeclList_tok_struct _tmp34B;
_tmp34B.tag=Cyc_DeclList_tok;_tmp34B.f1=({struct Cyc_List_List*_tmp34C=_cycalloc(
sizeof(*_tmp34C));_tmp34C->hd=Cyc_Absyn_letv_decl(_tmp343,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp34C->tl=0;_tmp34C;});_tmp34B;});_tmp34A;});break;}
case 25: _LL1D1: yyval=yyvs[yyvsp_offset];break;case 26: _LL1D2: yyval=(void*)({struct
Cyc_DeclList_tok_struct*_tmp34D=_cycalloc(sizeof(*_tmp34D));_tmp34D[0]=({struct
Cyc_DeclList_tok_struct _tmp34E;_tmp34E.tag=Cyc_DeclList_tok;_tmp34E.f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_yyget_DeclList_tok(yyvs[yyvsp_offset]));_tmp34E;});_tmp34D;});break;case 27:
_LL1D3: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp34F=_cycalloc(sizeof(*
_tmp34F));_tmp34F[0]=({struct Cyc_DeclSpec_tok_struct _tmp350;_tmp350.tag=Cyc_DeclSpec_tok;
_tmp350.f1=({struct Cyc_Parse_Declaration_spec*_tmp351=_cycalloc(sizeof(*_tmp351));
_tmp351->sc=({struct Cyc_Core_Opt*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352->v=(
void*)Cyc_yyget_StorageClass_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp352;});_tmp351->tq=Cyc_Absyn_empty_tqual();_tmp351->type_specs=
0;_tmp351->is_inline=0;_tmp351->attributes=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp351;});_tmp350;});_tmp34F;});break;case 28: _LL1D4: if((Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->sc != 0)({void*_tmp353[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tag_arr("Only one storage class is allowed in a declaration",
sizeof(unsigned char),51),_tag_arr(_tmp353,sizeof(void*),0));});yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354[0]=({
struct Cyc_DeclSpec_tok_struct _tmp355;_tmp355.tag=Cyc_DeclSpec_tok;_tmp355.f1=({
struct Cyc_Parse_Declaration_spec*_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356->sc=({
struct Cyc_Core_Opt*_tmp357=_cycalloc(sizeof(*_tmp357));_tmp357->v=(void*)Cyc_yyget_StorageClass_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp357;});_tmp356->tq=(
Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq;_tmp356->type_specs=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp356->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->is_inline;_tmp356->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp356;});_tmp355;});_tmp354;});break;case 29:
_LL1D5: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp358=_cycalloc(sizeof(*
_tmp358));_tmp358[0]=({struct Cyc_DeclSpec_tok_struct _tmp359;_tmp359.tag=Cyc_DeclSpec_tok;
_tmp359.f1=({struct Cyc_Parse_Declaration_spec*_tmp35A=_cycalloc(sizeof(*_tmp35A));
_tmp35A->sc=0;_tmp35A->tq=Cyc_Absyn_empty_tqual();_tmp35A->type_specs=({struct
Cyc_List_List*_tmp35B=_cycalloc(sizeof(*_tmp35B));_tmp35B->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp35B->tl=0;
_tmp35B;});_tmp35A->is_inline=0;_tmp35A->attributes=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp35A;});_tmp359;});_tmp358;});break;case 30: _LL1D6: yyval=(
void*)({struct Cyc_DeclSpec_tok_struct*_tmp35C=_cycalloc(sizeof(*_tmp35C));
_tmp35C[0]=({struct Cyc_DeclSpec_tok_struct _tmp35D;_tmp35D.tag=Cyc_DeclSpec_tok;
_tmp35D.f1=({struct Cyc_Parse_Declaration_spec*_tmp35E=_cycalloc(sizeof(*_tmp35E));
_tmp35E->sc=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->sc;_tmp35E->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->tq;_tmp35E->type_specs=({struct Cyc_List_List*_tmp35F=
_cycalloc(sizeof(*_tmp35F));_tmp35F->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp35F->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp35F;});_tmp35E->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->is_inline;_tmp35E->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp35E;});_tmp35D;});_tmp35C;});break;case 31:
_LL1D7: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp360=_cycalloc(sizeof(*
_tmp360));_tmp360[0]=({struct Cyc_DeclSpec_tok_struct _tmp361;_tmp361.tag=Cyc_DeclSpec_tok;
_tmp361.f1=({struct Cyc_Parse_Declaration_spec*_tmp362=_cycalloc(sizeof(*_tmp362));
_tmp362->sc=0;_tmp362->tq=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp362->type_specs=0;
_tmp362->is_inline=0;_tmp362->attributes=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp362;});_tmp361;});_tmp360;});break;case 32: _LL1D8: yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp363=_cycalloc(sizeof(*_tmp363));_tmp363[0]=({
struct Cyc_DeclSpec_tok_struct _tmp364;_tmp364.tag=Cyc_DeclSpec_tok;_tmp364.f1=({
struct Cyc_Parse_Declaration_spec*_tmp365=_cycalloc(sizeof(*_tmp365));_tmp365->sc=(
Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->sc;_tmp365->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq);_tmp365->type_specs=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp365->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->is_inline;_tmp365->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp365;});_tmp364;});_tmp363;});break;case 33:
_LL1D9: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp366=_cycalloc(sizeof(*
_tmp366));_tmp366[0]=({struct Cyc_DeclSpec_tok_struct _tmp367;_tmp367.tag=Cyc_DeclSpec_tok;
_tmp367.f1=({struct Cyc_Parse_Declaration_spec*_tmp368=_cycalloc(sizeof(*_tmp368));
_tmp368->sc=0;_tmp368->tq=Cyc_Absyn_empty_tqual();_tmp368->type_specs=0;_tmp368->is_inline=
1;_tmp368->attributes=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp368;});
_tmp367;});_tmp366;});break;case 34: _LL1DA: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp369=_cycalloc(sizeof(*_tmp369));_tmp369[0]=({struct Cyc_DeclSpec_tok_struct
_tmp36A;_tmp36A.tag=Cyc_DeclSpec_tok;_tmp36A.f1=({struct Cyc_Parse_Declaration_spec*
_tmp36B=_cycalloc(sizeof(*_tmp36B));_tmp36B->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->sc;_tmp36B->tq=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq;
_tmp36B->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->type_specs;
_tmp36B->is_inline=1;_tmp36B->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp36B;});_tmp36A;});_tmp369;});break;case 35:
_LL1DB: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp36C=_cycalloc(
sizeof(*_tmp36C));_tmp36C[0]=({struct Cyc_StorageClass_tok_struct _tmp36D;_tmp36D.tag=
Cyc_StorageClass_tok;_tmp36D.f1=(void*)((void*)4);_tmp36D;});_tmp36C;});break;
case 36: _LL1DC: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp36E=_cycalloc(
sizeof(*_tmp36E));_tmp36E[0]=({struct Cyc_StorageClass_tok_struct _tmp36F;_tmp36F.tag=
Cyc_StorageClass_tok;_tmp36F.f1=(void*)((void*)5);_tmp36F;});_tmp36E;});break;
case 37: _LL1DD: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp370=_cycalloc(
sizeof(*_tmp370));_tmp370[0]=({struct Cyc_StorageClass_tok_struct _tmp371;_tmp371.tag=
Cyc_StorageClass_tok;_tmp371.f1=(void*)((void*)3);_tmp371;});_tmp370;});break;
case 38: _LL1DE: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp372=_cycalloc(
sizeof(*_tmp372));_tmp372[0]=({struct Cyc_StorageClass_tok_struct _tmp373;_tmp373.tag=
Cyc_StorageClass_tok;_tmp373.f1=(void*)((void*)1);_tmp373;});_tmp372;});break;
case 39: _LL1DF: if(Cyc_Std_strcmp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),
_tag_arr("C",sizeof(unsigned char),2))!= 0)Cyc_Parse_err(_tag_arr("only extern or extern \"C\" is allowed",
sizeof(unsigned char),37),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374[0]=({struct Cyc_StorageClass_tok_struct
_tmp375;_tmp375.tag=Cyc_StorageClass_tok;_tmp375.f1=(void*)((void*)2);_tmp375;});
_tmp374;});break;case 40: _LL1E0: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp376=_cycalloc(sizeof(*_tmp376));_tmp376[0]=({struct Cyc_StorageClass_tok_struct
_tmp377;_tmp377.tag=Cyc_StorageClass_tok;_tmp377.f1=(void*)((void*)0);_tmp377;});
_tmp376;});break;case 41: _LL1E1: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378[0]=({struct Cyc_StorageClass_tok_struct
_tmp379;_tmp379.tag=Cyc_StorageClass_tok;_tmp379.f1=(void*)((void*)6);_tmp379;});
_tmp378;});break;case 42: _LL1E2: yyval=(void*)({struct Cyc_AttributeList_tok_struct*
_tmp37A=_cycalloc(sizeof(*_tmp37A));_tmp37A[0]=({struct Cyc_AttributeList_tok_struct
_tmp37B;_tmp37B.tag=Cyc_AttributeList_tok;_tmp37B.f1=0;_tmp37B;});_tmp37A;});
break;case 43: _LL1E3: yyval=yyvs[yyvsp_offset];break;case 44: _LL1E4: yyval=(void*)({
struct Cyc_AttributeList_tok_struct*_tmp37C=_cycalloc(sizeof(*_tmp37C));_tmp37C[0]=({
struct Cyc_AttributeList_tok_struct _tmp37D;_tmp37D.tag=Cyc_AttributeList_tok;
_tmp37D.f1=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp37D;});_tmp37C;});break;case 45: _LL1E5: yyval=(void*)({
struct Cyc_AttributeList_tok_struct*_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E[0]=({
struct Cyc_AttributeList_tok_struct _tmp37F;_tmp37F.tag=Cyc_AttributeList_tok;
_tmp37F.f1=({struct Cyc_List_List*_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380->hd=(
void*)Cyc_yyget_Attribute_tok(yyvs[yyvsp_offset]);_tmp380->tl=0;_tmp380;});
_tmp37F;});_tmp37E;});break;case 46: _LL1E6: yyval=(void*)({struct Cyc_AttributeList_tok_struct*
_tmp381=_cycalloc(sizeof(*_tmp381));_tmp381[0]=({struct Cyc_AttributeList_tok_struct
_tmp382;_tmp382.tag=Cyc_AttributeList_tok;_tmp382.f1=({struct Cyc_List_List*
_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383->hd=(void*)Cyc_yyget_Attribute_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp383->tl=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp383;});_tmp382;});_tmp381;});break;case 47: _LL1E7: {
static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};static struct _tuple15
att_map[16]={{{_tmp389,_tmp389,_tmp389 + 8},(void*)0},{{_tmp38A,_tmp38A,_tmp38A + 
6},(void*)1},{{_tmp38B,_tmp38B,_tmp38B + 9},(void*)2},{{_tmp38C,_tmp38C,_tmp38C + 
9},(void*)3},{{_tmp38D,_tmp38D,_tmp38D + 6},(void*)4},{{_tmp38E,_tmp38E,_tmp38E + 
8},(void*)& att_aligned},{{_tmp38F,_tmp38F,_tmp38F + 7},(void*)5},{{_tmp390,
_tmp390,_tmp390 + 7},(void*)7},{{_tmp391,_tmp391,_tmp391 + 7},(void*)8},{{_tmp392,
_tmp392,_tmp392 + 5},(void*)9},{{_tmp393,_tmp393,_tmp393 + 10},(void*)10},{{
_tmp394,_tmp394,_tmp394 + 10},(void*)11},{{_tmp395,_tmp395,_tmp395 + 23},(void*)12},{{
_tmp396,_tmp396,_tmp396 + 12},(void*)13},{{_tmp397,_tmp397,_tmp397 + 11},(void*)14},{{
_tmp398,_tmp398,_tmp398 + 22},(void*)15}};struct _tagged_arr _tmp384=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);if((((_get_arr_size(_tmp384,sizeof(unsigned char))> 4?*((
const unsigned char*)_check_unknown_subscript(_tmp384,sizeof(unsigned char),0))== '_':
0)?*((const unsigned char*)_check_unknown_subscript(_tmp384,sizeof(unsigned char),
1))== '_': 0)?*((const unsigned char*)_check_unknown_subscript(_tmp384,sizeof(
unsigned char),(int)(_get_arr_size(_tmp384,sizeof(unsigned char))- 2)))== '_': 0)?*((
const unsigned char*)_check_unknown_subscript(_tmp384,sizeof(unsigned char),(int)(
_get_arr_size(_tmp384,sizeof(unsigned char))- 3)))== '_': 0)_tmp384=(struct
_tagged_arr)Cyc_Std_substring(_tmp384,2,_get_arr_size(_tmp384,sizeof(
unsigned char))- 5);{int i=0;for(0;i < 16;++ i){if(Cyc_Std_strcmp(_tmp384,(att_map[
i]).f1)== 0){yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp385=_cycalloc(
sizeof(*_tmp385));_tmp385[0]=({struct Cyc_Attribute_tok_struct _tmp386;_tmp386.tag=
Cyc_Attribute_tok;_tmp386.f1=(void*)(att_map[i]).f2;_tmp386;});_tmp385;});break;}}
if(i == 16){Cyc_Parse_err(_tag_arr("unrecognized attribute",sizeof(unsigned char),
23),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp387=
_cycalloc(sizeof(*_tmp387));_tmp387[0]=({struct Cyc_Attribute_tok_struct _tmp388;
_tmp388.tag=Cyc_Attribute_tok;_tmp388.f1=(void*)((void*)1);_tmp388;});_tmp387;});}
break;}}case 48: _LL1E8: yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp39A=
_cycalloc(sizeof(*_tmp39A));_tmp39A[0]=({struct Cyc_Attribute_tok_struct _tmp39B;
_tmp39B.tag=Cyc_Attribute_tok;_tmp39B.f1=(void*)((void*)4);_tmp39B;});_tmp39A;});
break;case 49: _LL1E9: {struct _tagged_arr _tmp39C=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct _tuple12 _tmp39E;
int _tmp39F;struct _tuple12*_tmp39D=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp39E=*_tmp39D;_tmp39F=
_tmp39E.f2;{void*a;if(Cyc_Std_zstrcmp(_tmp39C,_tag_arr("regparm",sizeof(
unsigned char),8))== 0?1: Cyc_Std_zstrcmp(_tmp39C,_tag_arr("__regparm__",sizeof(
unsigned char),12))== 0){if(_tmp39F < 0?1: _tmp39F > 3)Cyc_Parse_err(_tag_arr("regparm requires value between 0 and 3",
sizeof(unsigned char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp3A0=_cycalloc_atomic(sizeof(*_tmp3A0));
_tmp3A0[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp3A1;_tmp3A1.tag=0;_tmp3A1.f1=
_tmp39F;_tmp3A1;});_tmp3A0;});}else{if(Cyc_Std_zstrcmp(_tmp39C,_tag_arr("aligned",
sizeof(unsigned char),8))== 0?1: Cyc_Std_zstrcmp(_tmp39C,_tag_arr("__aligned__",
sizeof(unsigned char),12))== 0){if(_tmp39F < 0)Cyc_Parse_err(_tag_arr("aligned requires positive power of two",
sizeof(unsigned char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp39F;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(_tag_arr("aligned requires positive power of two",sizeof(
unsigned char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp3A2=_cycalloc_atomic(sizeof(*_tmp3A2));
_tmp3A2[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp3A3;_tmp3A3.tag=1;_tmp3A3.f1=
_tmp39F;_tmp3A3;});_tmp3A2;});}}else{Cyc_Parse_err(_tag_arr("unrecognized attribute",
sizeof(unsigned char),23),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));
_tmp3A4[0]=({struct Cyc_Attribute_tok_struct _tmp3A5;_tmp3A5.tag=Cyc_Attribute_tok;
_tmp3A5.f1=(void*)a;_tmp3A5;});_tmp3A4;});break;}}case 50: _LL1EA: {struct
_tagged_arr _tmp3A6=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp3A7=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_Std_zstrcmp(
_tmp3A6,_tag_arr("section",sizeof(unsigned char),8))== 0?1: Cyc_Std_zstrcmp(
_tmp3A6,_tag_arr("__section__",sizeof(unsigned char),12))== 0)a=(void*)({struct
Cyc_Absyn_Section_att_struct*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));_tmp3A8[0]=({
struct Cyc_Absyn_Section_att_struct _tmp3A9;_tmp3A9.tag=2;_tmp3A9.f1=_tmp3A7;
_tmp3A9;});_tmp3A8;});else{Cyc_Parse_err(_tag_arr("unrecognized attribute",
sizeof(unsigned char),23),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));
_tmp3AA[0]=({struct Cyc_Attribute_tok_struct _tmp3AB;_tmp3AB.tag=Cyc_Attribute_tok;
_tmp3AB.f1=(void*)a;_tmp3AB;});_tmp3AA;});break;}case 51: _LL1EB: {struct
_tagged_arr _tmp3AC=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp3AD=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp3AF;struct
_tuple12 _tmp3AE=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp3AF=_tmp3AE.f2;{int _tmp3B1;struct _tuple12 _tmp3B0=*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3B1=_tmp3B0.f2;{
void*a=(void*)1;if(Cyc_Std_zstrcmp(_tmp3AC,_tag_arr("format",sizeof(
unsigned char),7))== 0?1: Cyc_Std_zstrcmp(_tmp3AC,_tag_arr("__format__",sizeof(
unsigned char),11))== 0){if(Cyc_Std_zstrcmp(_tmp3AD,_tag_arr("printf",sizeof(
unsigned char),7))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp3B2=
_cycalloc(sizeof(*_tmp3B2));_tmp3B2[0]=({struct Cyc_Absyn_Format_att_struct
_tmp3B3;_tmp3B3.tag=3;_tmp3B3.f1=(void*)((void*)0);_tmp3B3.f2=_tmp3AF;_tmp3B3.f3=
_tmp3B1;_tmp3B3;});_tmp3B2;});else{if(Cyc_Std_zstrcmp(_tmp3AD,_tag_arr("scanf",
sizeof(unsigned char),6))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*
_tmp3B4=_cycalloc(sizeof(*_tmp3B4));_tmp3B4[0]=({struct Cyc_Absyn_Format_att_struct
_tmp3B5;_tmp3B5.tag=3;_tmp3B5.f1=(void*)((void*)1);_tmp3B5.f2=_tmp3AF;_tmp3B5.f3=
_tmp3B1;_tmp3B5;});_tmp3B4;});else{Cyc_Parse_err(_tag_arr("unrecognized format type",
sizeof(unsigned char),25),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(
_tag_arr("unrecognized attribute",sizeof(unsigned char),23),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Attribute_tok_struct*
_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6[0]=({struct Cyc_Attribute_tok_struct
_tmp3B7;_tmp3B7.tag=Cyc_Attribute_tok;_tmp3B7.f1=(void*)a;_tmp3B7;});_tmp3B6;});
break;}}}case 52: _LL1EC: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3B8=
_cycalloc(sizeof(*_tmp3B8));_tmp3B8[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3B9;_tmp3B9.tag=Cyc_TypeSpecifier_tok;_tmp3B9.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3B9;});_tmp3B8;});break;case 53: _LL1ED: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));_tmp3BA[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3BB;_tmp3BB.tag=Cyc_TypeSpecifier_tok;
_tmp3BB.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3BB;});
_tmp3BA;});break;case 54: _LL1EE: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3BD;_tmp3BD.tag=Cyc_TypeSpecifier_tok;_tmp3BD.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_uchar_t,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3BD;});_tmp3BC;});break;case 55: _LL1EF: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));
_tmp3BE[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3BF;_tmp3BF.tag=Cyc_TypeSpecifier_tok;
_tmp3BF.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*_tmp3C0=_cycalloc(
sizeof(*_tmp3C0));_tmp3C0[0]=({struct Cyc_Parse_Short_spec_struct _tmp3C1;_tmp3C1.tag=
2;_tmp3C1.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line);_tmp3C1;});_tmp3C0;}));_tmp3BF;});_tmp3BE;});break;
case 56: _LL1F0: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3C2=
_cycalloc(sizeof(*_tmp3C2));_tmp3C2[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3C3;_tmp3C3.tag=Cyc_TypeSpecifier_tok;_tmp3C3.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_t,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3C3;});_tmp3C2;});break;case 57: _LL1F1: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));
_tmp3C4[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3C5;_tmp3C5.tag=Cyc_TypeSpecifier_tok;
_tmp3C5.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*_tmp3C6=_cycalloc(
sizeof(*_tmp3C6));_tmp3C6[0]=({struct Cyc_Parse_Long_spec_struct _tmp3C7;_tmp3C7.tag=
3;_tmp3C7.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line);_tmp3C7;});_tmp3C6;}));_tmp3C5;});_tmp3C4;});break;
case 58: _LL1F2: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3C8=
_cycalloc(sizeof(*_tmp3C8));_tmp3C8[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3C9;_tmp3C9.tag=Cyc_TypeSpecifier_tok;_tmp3C9.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3C9;});_tmp3C8;});break;case 59: _LL1F3: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));
_tmp3CA[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3CB;_tmp3CB.tag=Cyc_TypeSpecifier_tok;
_tmp3CB.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3CB;});
_tmp3CA;});break;case 60: _LL1F4: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3CD;_tmp3CD.tag=Cyc_TypeSpecifier_tok;_tmp3CD.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*
_tmp3CE=_cycalloc(sizeof(*_tmp3CE));_tmp3CE[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp3CF;_tmp3CF.tag=0;_tmp3CF.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp3CF;});_tmp3CE;}));_tmp3CD;});_tmp3CC;});
break;case 61: _LL1F5: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3D0=
_cycalloc(sizeof(*_tmp3D0));_tmp3D0[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3D1;_tmp3D1.tag=Cyc_TypeSpecifier_tok;_tmp3D1.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp3D3;_tmp3D3.tag=1;_tmp3D3.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp3D3;});_tmp3D2;}));_tmp3D1;});_tmp3D0;});
break;case 62: _LL1F6: yyval=yyvs[yyvsp_offset];break;case 63: _LL1F7: yyval=yyvs[
yyvsp_offset];break;case 64: _LL1F8: yyval=yyvs[yyvsp_offset];break;case 65: _LL1F9:
yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3D4=_cycalloc(sizeof(*
_tmp3D4));_tmp3D4[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3D5;_tmp3D5.tag=Cyc_TypeSpecifier_tok;
_tmp3D5.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3D5;});
_tmp3D4;});break;case 66: _LL1FA: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3D7;_tmp3D7.tag=Cyc_TypeSpecifier_tok;_tmp3D7.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypedefType_struct*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));
_tmp3D8[0]=({struct Cyc_Absyn_TypedefType_struct _tmp3D9;_tmp3D9.tag=16;_tmp3D9.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3D9.f2=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp3D9.f3=0;_tmp3D9;});
_tmp3D8;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3D7;});
_tmp3D6;});break;case 67: _LL1FB: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3DA=_cycalloc(sizeof(*_tmp3DA));_tmp3DA[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3DB;_tmp3DB.tag=Cyc_TypeSpecifier_tok;_tmp3DB.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TupleType_struct*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));
_tmp3DC[0]=({struct Cyc_Absyn_TupleType_struct _tmp3DD;_tmp3DD.tag=9;_tmp3DD.f1=((
struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp3DD;});_tmp3DC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3DB;});
_tmp3DA;});break;case 68: _LL1FC: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3DF;_tmp3DF.tag=Cyc_TypeSpecifier_tok;_tmp3DF.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));
_tmp3E0[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp3E1;_tmp3E1.tag=15;_tmp3E1.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp3E1;});_tmp3E0;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3DF;});_tmp3DE;});break;case 69: _LL1FD: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));_tmp3E2[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3E3;_tmp3E3.tag=Cyc_TypeSpecifier_tok;
_tmp3E3.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp3E5;_tmp3E5.tag=14;_tmp3E5.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3E5;});_tmp3E4;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3E3;});
_tmp3E2;});break;case 70: _LL1FE: yyval=(void*)({struct Cyc_Kind_tok_struct*_tmp3E6=
_cycalloc(sizeof(*_tmp3E6));_tmp3E6[0]=({struct Cyc_Kind_tok_struct _tmp3E7;
_tmp3E7.tag=Cyc_Kind_tok;_tmp3E7.f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3E7;});_tmp3E6;});break;case 71: _LL1FF: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp3E8=_cycalloc_atomic(sizeof(*_tmp3E8));
_tmp3E8[0]=({struct Cyc_TypeQual_tok_struct _tmp3E9;_tmp3E9.tag=Cyc_TypeQual_tok;
_tmp3E9.f1=({struct Cyc_Absyn_Tqual _tmp3EA;_tmp3EA.q_const=1;_tmp3EA.q_volatile=0;
_tmp3EA.q_restrict=0;_tmp3EA;});_tmp3E9;});_tmp3E8;});break;case 72: _LL200: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp3EB=_cycalloc_atomic(sizeof(*_tmp3EB));
_tmp3EB[0]=({struct Cyc_TypeQual_tok_struct _tmp3EC;_tmp3EC.tag=Cyc_TypeQual_tok;
_tmp3EC.f1=({struct Cyc_Absyn_Tqual _tmp3ED;_tmp3ED.q_const=0;_tmp3ED.q_volatile=1;
_tmp3ED.q_restrict=0;_tmp3ED;});_tmp3EC;});_tmp3EB;});break;case 73: _LL201: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp3EE=_cycalloc_atomic(sizeof(*_tmp3EE));
_tmp3EE[0]=({struct Cyc_TypeQual_tok_struct _tmp3EF;_tmp3EF.tag=Cyc_TypeQual_tok;
_tmp3EF.f1=({struct Cyc_Absyn_Tqual _tmp3F0;_tmp3F0.q_const=0;_tmp3F0.q_volatile=0;
_tmp3F0.q_restrict=1;_tmp3F0;});_tmp3EF;});_tmp3EE;});break;case 74: _LL202: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3F1=_cycalloc(sizeof(*_tmp3F1));
_tmp3F1[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3F2;_tmp3F2.tag=Cyc_TypeSpecifier_tok;
_tmp3F2.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp3F3=_cycalloc(
sizeof(*_tmp3F3));_tmp3F3[0]=({struct Cyc_Parse_Decl_spec_struct _tmp3F4;_tmp3F4.tag=
5;_tmp3F4.f1=({struct Cyc_Absyn_Decl*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));_tmp3F5->r=(
void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));
_tmp3F6[0]=({struct Cyc_Absyn_Enum_d_struct _tmp3F7;_tmp3F7.tag=6;_tmp3F7.f1=({
struct Cyc_Absyn_Enumdecl*_tmp3F8=_cycalloc(sizeof(*_tmp3F8));_tmp3F8->sc=(void*)((
void*)2);_tmp3F8->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp3F8->fields=({struct Cyc_Core_Opt*_tmp3F9=_cycalloc(
sizeof(*_tmp3F9));_tmp3F9->v=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3F9;});_tmp3F8;});
_tmp3F7;});_tmp3F6;}));_tmp3F5->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp3F5;});_tmp3F4;});_tmp3F3;}));_tmp3F2;});_tmp3F1;});
break;case 75: _LL203: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3FA=
_cycalloc(sizeof(*_tmp3FA));_tmp3FA[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3FB;_tmp3FB.tag=Cyc_TypeSpecifier_tok;_tmp3FB.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));
_tmp3FC[0]=({struct Cyc_Absyn_EnumType_struct _tmp3FD;_tmp3FD.tag=12;_tmp3FD.f1=
Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);_tmp3FD.f2=0;_tmp3FD;});_tmp3FC;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3FB;});_tmp3FA;});break;case 76: _LL204: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));_tmp3FE[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3FF;_tmp3FF.tag=Cyc_TypeSpecifier_tok;
_tmp3FF.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp400=_cycalloc(
sizeof(*_tmp400));_tmp400[0]=({struct Cyc_Parse_Type_spec_struct _tmp401;_tmp401.tag=
4;_tmp401.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp402=
_cycalloc(sizeof(*_tmp402));_tmp402[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp403;_tmp403.tag=13;_tmp403.f1=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp403;});_tmp402;}));
_tmp401.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line);_tmp401;});
_tmp400;}));_tmp3FF;});_tmp3FE;});break;case 77: _LL205: yyval=(void*)({struct Cyc_Enumfield_tok_struct*
_tmp404=_cycalloc(sizeof(*_tmp404));_tmp404[0]=({struct Cyc_Enumfield_tok_struct
_tmp405;_tmp405.tag=Cyc_Enumfield_tok;_tmp405.f1=({struct Cyc_Absyn_Enumfield*
_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406->name=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp406->tag=0;_tmp406->loc=Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);_tmp406;});_tmp405;});
_tmp404;});break;case 78: _LL206: yyval=(void*)({struct Cyc_Enumfield_tok_struct*
_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407[0]=({struct Cyc_Enumfield_tok_struct
_tmp408;_tmp408.tag=Cyc_Enumfield_tok;_tmp408.f1=({struct Cyc_Absyn_Enumfield*
_tmp409=_cycalloc(sizeof(*_tmp409));_tmp409->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp409->tag=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp409->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp409;});_tmp408;});_tmp407;});break;case 79: _LL207:
yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*_tmp40A=_cycalloc(sizeof(*
_tmp40A));_tmp40A[0]=({struct Cyc_EnumfieldList_tok_struct _tmp40B;_tmp40B.tag=Cyc_EnumfieldList_tok;
_tmp40B.f1=({struct Cyc_List_List*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C->hd=
Cyc_yyget_Enumfield_tok(yyvs[yyvsp_offset]);_tmp40C->tl=0;_tmp40C;});_tmp40B;});
_tmp40A;});break;case 80: _LL208: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp40E;_tmp40E.tag=Cyc_EnumfieldList_tok;_tmp40E.f1=({struct Cyc_List_List*
_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp40F->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[yyvsp_offset]);_tmp40F;});_tmp40E;});_tmp40D;});break;case 81: _LL209: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp410=_cycalloc(sizeof(*_tmp410));
_tmp410[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp411;_tmp411.tag=Cyc_TypeSpecifier_tok;
_tmp411.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp413;_tmp413.tag=11;_tmp413.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp413.f2=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp413;});_tmp412;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp411;});
_tmp410;});break;case 82: _LL20A: {struct Cyc_List_List*_tmp414=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));struct Cyc_List_List*
_tmp415=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp417;_tmp417.tag=Cyc_TypeSpecifier_tok;
_tmp417.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp418=_cycalloc(
sizeof(*_tmp418));_tmp418[0]=({struct Cyc_Parse_Decl_spec_struct _tmp419;_tmp419.tag=
5;_tmp419.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmp414,({struct Cyc_Core_Opt*
_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A->v=_tmp415;_tmp41A;}),({struct Cyc_Core_Opt*
_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B->v=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp41B;}),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp419;});_tmp418;}));_tmp417;});_tmp416;});break;}
case 83: _LL20B: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp41C=
_cycalloc(sizeof(*_tmp41C));_tmp41C[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp41D;_tmp41D.tag=Cyc_TypeSpecifier_tok;_tmp41D.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp41E=_cycalloc(sizeof(*_tmp41E));
_tmp41E[0]=({struct Cyc_Absyn_AggrType_struct _tmp41F;_tmp41F.tag=10;_tmp41F.f1=({
struct Cyc_Absyn_AggrInfo _tmp420;_tmp420.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp422;_tmp422.tag=0;_tmp422.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp422.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp422;});_tmp421;}));
_tmp420.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp420;});_tmp41F;});
_tmp41E;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp41D;});
_tmp41C;});break;case 84: _LL20C: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423[0]=({struct Cyc_TypeList_tok_struct
_tmp424;_tmp424.tag=Cyc_TypeList_tok;_tmp424.f1=0;_tmp424;});_tmp423;});break;
case 85: _LL20D: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp425=_cycalloc(
sizeof(*_tmp425));_tmp425[0]=({struct Cyc_TypeList_tok_struct _tmp426;_tmp426.tag=
Cyc_TypeList_tok;_tmp426.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp426;});_tmp425;});
break;case 86: _LL20E: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp427=
_cycalloc(sizeof(*_tmp427));_tmp427[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp428;_tmp428.tag=Cyc_StructOrUnion_tok;_tmp428.f1=(void*)((void*)0);_tmp428;});
_tmp427;});break;case 87: _LL20F: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*
_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp42A;_tmp42A.tag=Cyc_StructOrUnion_tok;_tmp42A.f1=(void*)((void*)1);_tmp42A;});
_tmp429;});break;case 88: _LL210: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp42C;_tmp42C.tag=Cyc_AggrFieldDeclList_tok;_tmp42C.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_AggrFieldDeclListList_tok(yyvs[yyvsp_offset])));
_tmp42C;});_tmp42B;});break;case 89: _LL211: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp42D=_cycalloc(sizeof(*_tmp42D));_tmp42D[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp42E;_tmp42E.tag=Cyc_AggrFieldDeclListList_tok;_tmp42E.f1=({struct Cyc_List_List*
_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[yyvsp_offset]);_tmp42F->tl=0;_tmp42F;});_tmp42E;});_tmp42D;});break;case 90:
_LL212: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*_tmp430=
_cycalloc(sizeof(*_tmp430));_tmp430[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp431;_tmp431.tag=Cyc_AggrFieldDeclListList_tok;_tmp431.f1=({struct Cyc_List_List*
_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[yyvsp_offset]);_tmp432->tl=Cyc_yyget_AggrFieldDeclListList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp432;});_tmp431;});
_tmp430;});break;case 91: _LL213: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433[0]=({struct Cyc_InitDeclList_tok_struct
_tmp434;_tmp434.tag=Cyc_InitDeclList_tok;_tmp434.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
yyvsp_offset]));_tmp434;});_tmp433;});break;case 92: _LL214: yyval=(void*)({struct
Cyc_InitDeclList_tok_struct*_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435[0]=({
struct Cyc_InitDeclList_tok_struct _tmp436;_tmp436.tag=Cyc_InitDeclList_tok;
_tmp436.f1=({struct Cyc_List_List*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->hd=
Cyc_yyget_InitDecl_tok(yyvs[yyvsp_offset]);_tmp437->tl=0;_tmp437;});_tmp436;});
_tmp435;});break;case 93: _LL215: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438[0]=({struct Cyc_InitDeclList_tok_struct
_tmp439;_tmp439.tag=Cyc_InitDeclList_tok;_tmp439.f1=({struct Cyc_List_List*
_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->hd=Cyc_yyget_InitDecl_tok(yyvs[
yyvsp_offset]);_tmp43A->tl=Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp43A;});_tmp439;});
_tmp438;});break;case 94: _LL216: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B[0]=({struct Cyc_InitDecl_tok_struct
_tmp43C;_tmp43C.tag=Cyc_InitDecl_tok;_tmp43C.f1=({struct _tuple11*_tmp43D=
_cycalloc(sizeof(*_tmp43D));_tmp43D->f1=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp43D->f2=0;_tmp43D;});_tmp43C;});_tmp43B;});break;case 95:
_LL217: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp43E=_cycalloc(sizeof(*
_tmp43E));_tmp43E[0]=({struct Cyc_InitDecl_tok_struct _tmp43F;_tmp43F.tag=Cyc_InitDecl_tok;
_tmp43F.f1=({struct _tuple11*_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440->f1=Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp440->f2=(struct
Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp440;});_tmp43F;});
_tmp43E;});break;case 96: _LL218: {struct _tuple14 _tmp442;struct Cyc_Absyn_Tqual
_tmp443;struct Cyc_List_List*_tmp444;struct Cyc_List_List*_tmp445;struct _tuple14*
_tmp441=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp442=*_tmp441;_tmp443=_tmp442.f1;_tmp444=_tmp442.f2;
_tmp445=_tmp442.f3;{struct Cyc_List_List*_tmp447;struct Cyc_List_List*_tmp448;
struct _tuple0 _tmp446=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
Cyc_yyget_InitDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp447=_tmp446.f1;_tmp448=_tmp446.f2;{void*_tmp449=Cyc_Parse_speclist2typ(
_tmp444,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp44A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(Cyc_Parse_apply_tmss(
_tmp443,_tmp449,_tmp447,_tmp445),_tmp448);yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp44C;_tmp44C.tag=Cyc_AggrFieldDeclList_tok;_tmp44C.f1=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,struct _tuple7*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmp44A);_tmp44C;});_tmp44B;});break;}}}case 97:
_LL219: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp44D=_cycalloc(
sizeof(*_tmp44D));_tmp44D[0]=({struct Cyc_QualSpecList_tok_struct _tmp44E;_tmp44E.tag=
Cyc_QualSpecList_tok;_tmp44E.f1=({struct _tuple14*_tmp44F=_cycalloc(sizeof(*
_tmp44F));_tmp44F->f1=Cyc_Absyn_empty_tqual();_tmp44F->f2=({struct Cyc_List_List*
_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp450->tl=0;
_tmp450;});_tmp44F->f3=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp44F;});
_tmp44E;});_tmp44D;});break;case 98: _LL21A: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451[0]=({struct Cyc_QualSpecList_tok_struct
_tmp452;_tmp452.tag=Cyc_QualSpecList_tok;_tmp452.f1=({struct _tuple14*_tmp453=
_cycalloc(sizeof(*_tmp453));_tmp453->f1=(*Cyc_yyget_QualSpecList_tok(yyvs[
yyvsp_offset])).f1;_tmp453->f2=({struct Cyc_List_List*_tmp454=_cycalloc(sizeof(*
_tmp454));_tmp454->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp454->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f2;_tmp454;});_tmp453->f3=Cyc_List_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f3);_tmp453;});_tmp452;});_tmp451;});break;case 99: _LL21B:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp455=_cycalloc(sizeof(*
_tmp455));_tmp455[0]=({struct Cyc_QualSpecList_tok_struct _tmp456;_tmp456.tag=Cyc_QualSpecList_tok;
_tmp456.f1=({struct _tuple14*_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp457->f2=0;
_tmp457->f3=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp457;});_tmp456;});
_tmp455;});break;case 100: _LL21C: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp458=_cycalloc(sizeof(*_tmp458));_tmp458[0]=({struct Cyc_QualSpecList_tok_struct
_tmp459;_tmp459.tag=Cyc_QualSpecList_tok;_tmp459.f1=({struct _tuple14*_tmp45A=
_cycalloc(sizeof(*_tmp45A));_tmp45A->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f1);_tmp45A->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
yyvsp_offset])).f2;_tmp45A->f3=Cyc_List_append(Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f3);_tmp45A;});_tmp459;});_tmp458;});break;case 101: _LL21D:
yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp45B=_cycalloc(sizeof(*
_tmp45B));_tmp45B[0]=({struct Cyc_InitDeclList_tok_struct _tmp45C;_tmp45C.tag=Cyc_InitDeclList_tok;
_tmp45C.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok(yyvs[yyvsp_offset]));_tmp45C;});_tmp45B;});break;case
102: _LL21E: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp45D=_cycalloc(
sizeof(*_tmp45D));_tmp45D[0]=({struct Cyc_InitDeclList_tok_struct _tmp45E;_tmp45E.tag=
Cyc_InitDeclList_tok;_tmp45E.f1=({struct Cyc_List_List*_tmp45F=_cycalloc(sizeof(*
_tmp45F));_tmp45F->hd=Cyc_yyget_InitDecl_tok(yyvs[yyvsp_offset]);_tmp45F->tl=0;
_tmp45F;});_tmp45E;});_tmp45D;});break;case 103: _LL21F: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460[0]=({struct Cyc_InitDeclList_tok_struct
_tmp461;_tmp461.tag=Cyc_InitDeclList_tok;_tmp461.f1=({struct Cyc_List_List*
_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462->hd=Cyc_yyget_InitDecl_tok(yyvs[
yyvsp_offset]);_tmp462->tl=Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp462;});_tmp461;});
_tmp460;});break;case 104: _LL220: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp463=_cycalloc(sizeof(*_tmp463));_tmp463[0]=({struct Cyc_InitDecl_tok_struct
_tmp464;_tmp464.tag=Cyc_InitDecl_tok;_tmp464.f1=({struct _tuple11*_tmp465=
_cycalloc(sizeof(*_tmp465));_tmp465->f1=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp465->f2=0;_tmp465;});_tmp464;});_tmp463;});break;case 105:
_LL221: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp466=_cycalloc(sizeof(*
_tmp466));_tmp466[0]=({struct Cyc_InitDecl_tok_struct _tmp467;_tmp467.tag=Cyc_InitDecl_tok;
_tmp467.f1=({struct _tuple11*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468->f1=({
struct Cyc_Parse_Declarator*_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469->id=({
struct _tuple1*_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->f1=Cyc_Absyn_rel_ns_null;
_tmp46A->f2=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(
unsigned char),1);_tmp46A;});_tmp469->tms=0;_tmp469;});_tmp468->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp468;});_tmp467;});_tmp466;});break;
case 106: _LL222: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp46C=_cycalloc(
sizeof(*_tmp46C));_tmp46C[0]=({struct Cyc_InitDecl_tok_struct _tmp46D;_tmp46D.tag=
Cyc_InitDecl_tok;_tmp46D.f1=({struct _tuple11*_tmp46E=_cycalloc(sizeof(*_tmp46E));
_tmp46E->f1=Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp46E->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]);_tmp46E;});_tmp46D;});_tmp46C;});break;case 107: _LL223: {struct Cyc_List_List*
_tmp46F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp471;_tmp471.tag=Cyc_TypeSpecifier_tok;
_tmp471.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp472=_cycalloc(
sizeof(*_tmp472));_tmp472[0]=({struct Cyc_Parse_Decl_spec_struct _tmp473;_tmp473.tag=
5;_tmp473.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp46F,({struct Cyc_Core_Opt*
_tmp474=_cycalloc(sizeof(*_tmp474));_tmp474->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp474;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp473;});_tmp472;}));_tmp471;});_tmp470;});break;}
case 108: _LL224: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp475=
_cycalloc(sizeof(*_tmp475));_tmp475[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp476;_tmp476.tag=Cyc_TypeSpecifier_tok;_tmp476.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp477=_cycalloc(sizeof(*_tmp477));
_tmp477[0]=({struct Cyc_Absyn_TunionType_struct _tmp478;_tmp478.tag=2;_tmp478.f1=({
struct Cyc_Absyn_TunionInfo _tmp479;_tmp479.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp47B;_tmp47B.tag=0;_tmp47B.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp47C;
_tmp47C.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp47C.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp47C;});_tmp47B;});
_tmp47A;}));_tmp479.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp479.rgn=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp479;});_tmp478;});_tmp477;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp476;});_tmp475;});break;case 109: _LL225: {void*
_tmp47D=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp486=_cycalloc(sizeof(*
_tmp486));_tmp486->v=(void*)((void*)3);_tmp486;}),0);yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp47E=_cycalloc(sizeof(*_tmp47E));_tmp47E[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp47F;_tmp47F.tag=Cyc_TypeSpecifier_tok;_tmp47F.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp480=_cycalloc(sizeof(*_tmp480));
_tmp480[0]=({struct Cyc_Absyn_TunionType_struct _tmp481;_tmp481.tag=2;_tmp481.f1=({
struct Cyc_Absyn_TunionInfo _tmp482;_tmp482.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp484;_tmp484.tag=0;_tmp484.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp485;
_tmp485.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp485.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp485;});_tmp484;});
_tmp483;}));_tmp482.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp482.rgn=(
void*)_tmp47D;_tmp482;});_tmp481;});_tmp480;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp47F;});_tmp47E;});break;}case 110: _LL226: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp487=_cycalloc(sizeof(*_tmp487));
_tmp487[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp488;_tmp488.tag=Cyc_TypeSpecifier_tok;
_tmp488.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp48A;_tmp48A.tag=3;_tmp48A.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp48B;
_tmp48B.field_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*
_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp48D;_tmp48D.tag=0;_tmp48D.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp48E;
_tmp48E.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp48E.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp48E.is_xtunion=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp48E;});_tmp48D;});
_tmp48C;}));_tmp48B.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp48B;});
_tmp48A;});_tmp489;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp488;});_tmp487;});break;case 111: _LL227: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp48F=_cycalloc_atomic(sizeof(*_tmp48F));
_tmp48F[0]=({struct Cyc_Bool_tok_struct _tmp490;_tmp490.tag=Cyc_Bool_tok;_tmp490.f1=
0;_tmp490;});_tmp48F;});break;case 112: _LL228: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp491=_cycalloc_atomic(sizeof(*_tmp491));_tmp491[0]=({struct Cyc_Bool_tok_struct
_tmp492;_tmp492.tag=Cyc_Bool_tok;_tmp492.f1=1;_tmp492;});_tmp491;});break;case
113: _LL229: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp493=_cycalloc(
sizeof(*_tmp493));_tmp493[0]=({struct Cyc_TunionFieldList_tok_struct _tmp494;
_tmp494.tag=Cyc_TunionFieldList_tok;_tmp494.f1=({struct Cyc_List_List*_tmp495=
_cycalloc(sizeof(*_tmp495));_tmp495->hd=Cyc_yyget_TunionField_tok(yyvs[
yyvsp_offset]);_tmp495->tl=0;_tmp495;});_tmp494;});_tmp493;});break;case 114:
_LL22A: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp496=_cycalloc(
sizeof(*_tmp496));_tmp496[0]=({struct Cyc_TunionFieldList_tok_struct _tmp497;
_tmp497.tag=Cyc_TunionFieldList_tok;_tmp497.f1=({struct Cyc_List_List*_tmp498=
_cycalloc(sizeof(*_tmp498));_tmp498->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp498->tl=0;_tmp498;});
_tmp497;});_tmp496;});break;case 115: _LL22B: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp49A;_tmp49A.tag=Cyc_TunionFieldList_tok;_tmp49A.f1=({struct Cyc_List_List*
_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp49B->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[yyvsp_offset]);_tmp49B;});_tmp49A;});_tmp499;});break;case 116: _LL22C: yyval=(
void*)({struct Cyc_TunionFieldList_tok_struct*_tmp49C=_cycalloc(sizeof(*_tmp49C));
_tmp49C[0]=({struct Cyc_TunionFieldList_tok_struct _tmp49D;_tmp49D.tag=Cyc_TunionFieldList_tok;
_tmp49D.f1=({struct Cyc_List_List*_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E->hd=
Cyc_yyget_TunionField_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]);_tmp49E->tl=Cyc_yyget_TunionFieldList_tok(yyvs[yyvsp_offset]);_tmp49E;});
_tmp49D;});_tmp49C;});break;case 117: _LL22D: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp49F=_cycalloc(sizeof(*_tmp49F));_tmp49F[0]=({struct Cyc_Scope_tok_struct
_tmp4A0;_tmp4A0.tag=Cyc_Scope_tok;_tmp4A0.f1=(void*)((void*)2);_tmp4A0;});
_tmp49F;});break;case 118: _LL22E: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1[0]=({struct Cyc_Scope_tok_struct
_tmp4A2;_tmp4A2.tag=Cyc_Scope_tok;_tmp4A2.f1=(void*)((void*)3);_tmp4A2;});
_tmp4A1;});break;case 119: _LL22F: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3[0]=({struct Cyc_Scope_tok_struct
_tmp4A4;_tmp4A4.tag=Cyc_Scope_tok;_tmp4A4.f1=(void*)((void*)0);_tmp4A4;});
_tmp4A3;});break;case 120: _LL230: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp4A5=_cycalloc(sizeof(*_tmp4A5));_tmp4A5[0]=({struct Cyc_TunionField_tok_struct
_tmp4A6;_tmp4A6.tag=Cyc_TunionField_tok;_tmp4A6.f1=({struct Cyc_Absyn_Tunionfield*
_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7->name=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp4A7->typs=0;_tmp4A7->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4A7->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A7;});_tmp4A6;});
_tmp4A5;});break;case 121: _LL231: {struct Cyc_List_List*_tmp4A8=((struct Cyc_List_List*(*)(
struct _tuple8*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9[0]=({
struct Cyc_TunionField_tok_struct _tmp4AA;_tmp4AA.tag=Cyc_TunionField_tok;_tmp4AA.f1=({
struct Cyc_Absyn_Tunionfield*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp4AB->typs=_tmp4A8;_tmp4AB->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4AB->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp4AB;});_tmp4AA;});
_tmp4A9;});break;}case 122: _LL232: yyval=yyvs[yyvsp_offset];break;case 123: _LL233:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));
_tmp4AC[0]=({struct Cyc_Declarator_tok_struct _tmp4AD;_tmp4AD.tag=Cyc_Declarator_tok;
_tmp4AD.f1=({struct Cyc_Parse_Declarator*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));
_tmp4AE->id=(Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]))->id;_tmp4AE->tms=Cyc_List_imp_append(
Cyc_yyget_TypeModifierList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]))->tms);_tmp4AE;});
_tmp4AD;});_tmp4AC;});break;case 124: _LL234: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4AF=_cycalloc(sizeof(*_tmp4AF));_tmp4AF[0]=({struct Cyc_Declarator_tok_struct
_tmp4B0;_tmp4B0.tag=Cyc_Declarator_tok;_tmp4B0.f1=({struct Cyc_Parse_Declarator*
_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1->id=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp4B1->tms=0;_tmp4B1;});_tmp4B0;});_tmp4AF;});break;case 125:
_LL235: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 126: _LL236: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4B2=_cycalloc(
sizeof(*_tmp4B2));_tmp4B2[0]=({struct Cyc_Declarator_tok_struct _tmp4B3;_tmp4B3.tag=
Cyc_Declarator_tok;_tmp4B3.f1=({struct Cyc_Parse_Declarator*_tmp4B4=_cycalloc(
sizeof(*_tmp4B4));_tmp4B4->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->id;_tmp4B4->tms=({
struct Cyc_List_List*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5->hd=(void*)((
void*)0);_tmp4B5->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->tms;_tmp4B5;});_tmp4B4;});
_tmp4B3;});_tmp4B2;});break;case 127: _LL237: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6[0]=({struct Cyc_Declarator_tok_struct
_tmp4B7;_tmp4B7.tag=Cyc_Declarator_tok;_tmp4B7.f1=({struct Cyc_Parse_Declarator*
_tmp4B8=_cycalloc(sizeof(*_tmp4B8));_tmp4B8->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4B8->tms=({
struct Cyc_List_List*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));_tmp4B9->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));
_tmp4BA[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp4BB;_tmp4BB.tag=0;_tmp4BB.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4BB;});_tmp4BA;}));_tmp4B9->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp4B9;});_tmp4B8;});
_tmp4B7;});_tmp4B6;});break;case 128: _LL238: {struct _tuple13 _tmp4BD;struct Cyc_List_List*
_tmp4BE;int _tmp4BF;struct Cyc_Absyn_VarargInfo*_tmp4C0;struct Cyc_Core_Opt*_tmp4C1;
struct Cyc_List_List*_tmp4C2;struct _tuple13*_tmp4BC=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4BD=*_tmp4BC;_tmp4BE=
_tmp4BD.f1;_tmp4BF=_tmp4BD.f2;_tmp4C0=_tmp4BD.f3;_tmp4C1=_tmp4BD.f4;_tmp4C2=
_tmp4BD.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4C3=_cycalloc(
sizeof(*_tmp4C3));_tmp4C3[0]=({struct Cyc_Declarator_tok_struct _tmp4C4;_tmp4C4.tag=
Cyc_Declarator_tok;_tmp4C4.f1=({struct Cyc_Parse_Declarator*_tmp4C5=_cycalloc(
sizeof(*_tmp4C5));_tmp4C5->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4C5->tms=({
struct Cyc_List_List*_tmp4C6=_cycalloc(sizeof(*_tmp4C6));_tmp4C6->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4C7=_cycalloc(sizeof(*_tmp4C7));
_tmp4C7[0]=({struct Cyc_Absyn_Function_mod_struct _tmp4C8;_tmp4C8.tag=2;_tmp4C8.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp4C9=_cycalloc(sizeof(*
_tmp4C9));_tmp4C9[0]=({struct Cyc_Absyn_WithTypes_struct _tmp4CA;_tmp4CA.tag=1;
_tmp4CA.f1=_tmp4BE;_tmp4CA.f2=_tmp4BF;_tmp4CA.f3=_tmp4C0;_tmp4CA.f4=_tmp4C1;
_tmp4CA.f5=_tmp4C2;_tmp4CA;});_tmp4C9;}));_tmp4C8;});_tmp4C7;}));_tmp4C6->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp4C6;});_tmp4C5;});_tmp4C4;});_tmp4C3;});break;}case 129: _LL239:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));
_tmp4CB[0]=({struct Cyc_Declarator_tok_struct _tmp4CC;_tmp4CC.tag=Cyc_Declarator_tok;
_tmp4CC.f1=({struct Cyc_Parse_Declarator*_tmp4CD=_cycalloc(sizeof(*_tmp4CD));
_tmp4CD->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp4CD->tms=({struct Cyc_List_List*_tmp4CE=_cycalloc(
sizeof(*_tmp4CE));_tmp4CE->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp4CF=_cycalloc(sizeof(*_tmp4CF));_tmp4CF[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp4D0;_tmp4D0.tag=2;_tmp4D0.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp4D1=_cycalloc(sizeof(*_tmp4D1));_tmp4D1[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp4D2;_tmp4D2.tag=1;_tmp4D2.f1=0;_tmp4D2.f2=0;_tmp4D2.f3=0;_tmp4D2.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D2.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4D2;});_tmp4D1;}));
_tmp4D0;});_tmp4CF;}));_tmp4CE->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp4CE;});_tmp4CD;});
_tmp4CC;});_tmp4CB;});break;case 130: _LL23A: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4D3=_cycalloc(sizeof(*_tmp4D3));_tmp4D3[0]=({struct Cyc_Declarator_tok_struct
_tmp4D4;_tmp4D4.tag=Cyc_Declarator_tok;_tmp4D4.f1=({struct Cyc_Parse_Declarator*
_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4D5->tms=({
struct Cyc_List_List*_tmp4D6=_cycalloc(sizeof(*_tmp4D6));_tmp4D6->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4D7=_cycalloc(sizeof(*_tmp4D7));
_tmp4D7[0]=({struct Cyc_Absyn_Function_mod_struct _tmp4D8;_tmp4D8.tag=2;_tmp4D8.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp4D9=_cycalloc(sizeof(*_tmp4D9));
_tmp4D9[0]=({struct Cyc_Absyn_NoTypes_struct _tmp4DA;_tmp4DA.tag=0;_tmp4DA.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4DA.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4DA;});_tmp4D9;}));_tmp4D8;});_tmp4D7;}));_tmp4D6->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp4D6;});_tmp4D5;});_tmp4D4;});_tmp4D3;});break;case 131: _LL23B: {
struct Cyc_List_List*_tmp4DB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC[0]=({
struct Cyc_Declarator_tok_struct _tmp4DD;_tmp4DD.tag=Cyc_Declarator_tok;_tmp4DD.f1=({
struct Cyc_Parse_Declarator*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));_tmp4DE->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4DE->tms=({
struct Cyc_List_List*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));
_tmp4E0[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp4E1;_tmp4E1.tag=3;_tmp4E1.f1=
_tmp4DB;_tmp4E1.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4E1.f3=0;_tmp4E1;});_tmp4E0;}));_tmp4DF->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp4DF;});
_tmp4DE;});_tmp4DD;});_tmp4DC;});break;}case 132: _LL23C: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4E2=_cycalloc(sizeof(*_tmp4E2));_tmp4E2[0]=({struct Cyc_Declarator_tok_struct
_tmp4E3;_tmp4E3.tag=Cyc_Declarator_tok;_tmp4E3.f1=({struct Cyc_Parse_Declarator*
_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp4E4->tms=({
struct Cyc_List_List*_tmp4E5=_cycalloc(sizeof(*_tmp4E5));_tmp4E5->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));
_tmp4E6[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp4E7;_tmp4E7.tag=4;_tmp4E7.f1=
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);
_tmp4E7.f2=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp4E7;});_tmp4E6;}));
_tmp4E5->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp4E5;});_tmp4E4;});_tmp4E3;});_tmp4E2;});break;case
133: _LL23D: Cyc_Parse_err(_tag_arr("identifier has not been declared as a typedef",
sizeof(unsigned char),46),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(void*)({
struct Cyc_Declarator_tok_struct*_tmp4E8=_cycalloc(sizeof(*_tmp4E8));_tmp4E8[0]=({
struct Cyc_Declarator_tok_struct _tmp4E9;_tmp4E9.tag=Cyc_Declarator_tok;_tmp4E9.f1=({
struct Cyc_Parse_Declarator*_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA->id=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp4EA->tms=0;_tmp4EA;});_tmp4E9;});_tmp4E8;});break;case
134: _LL23E: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4EB=_cycalloc(
sizeof(*_tmp4EB));_tmp4EB[0]=({struct Cyc_Declarator_tok_struct _tmp4EC;_tmp4EC.tag=
Cyc_Declarator_tok;_tmp4EC.f1=({struct Cyc_Parse_Declarator*_tmp4ED=_cycalloc(
sizeof(*_tmp4ED));_tmp4ED->id=Cyc_Absyn_exn_name;_tmp4ED->tms=0;_tmp4ED;});
_tmp4EC;});_tmp4EB;});break;case 135: _LL23F: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp4EE=_cycalloc(sizeof(*_tmp4EE));_tmp4EE[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp4EF;_tmp4EF.tag=Cyc_TypeModifierList_tok;_tmp4EF.f1=({struct Cyc_List_List*
_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp4F2;_tmp4F2.tag=1;_tmp4F2.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4F2.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4F2.f3=Cyc_Absyn_empty_tqual();
_tmp4F2;});_tmp4F1;}));_tmp4F0->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]),0);_tmp4F0;});_tmp4EF;});_tmp4EE;});break;case 136: _LL240:
yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*_tmp4F3=_cycalloc(sizeof(*
_tmp4F3));_tmp4F3[0]=({struct Cyc_TypeModifierList_tok_struct _tmp4F4;_tmp4F4.tag=
Cyc_TypeModifierList_tok;_tmp4F4.f1=({struct Cyc_List_List*_tmp4F5=_cycalloc(
sizeof(*_tmp4F5));_tmp4F5->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp4F7;_tmp4F7.tag=1;_tmp4F7.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4F7.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4F7.f3=Cyc_yyget_TypeQual_tok(
yyvs[yyvsp_offset]);_tmp4F7;});_tmp4F6;}));_tmp4F5->tl=Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0);_tmp4F5;});_tmp4F4;});
_tmp4F3;});break;case 137: _LL241: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp4F9;_tmp4F9.tag=Cyc_TypeModifierList_tok;_tmp4F9.f1=({struct Cyc_List_List*
_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp4FC;_tmp4FC.tag=1;_tmp4FC.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4FC.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4FC.f3=Cyc_Absyn_empty_tqual();
_tmp4FC;});_tmp4FB;}));_tmp4FA->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp4FA;});_tmp4F9;});_tmp4F8;});break;case 138: _LL242: yyval=(
void*)({struct Cyc_TypeModifierList_tok_struct*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));
_tmp4FD[0]=({struct Cyc_TypeModifierList_tok_struct _tmp4FE;_tmp4FE.tag=Cyc_TypeModifierList_tok;
_tmp4FE.f1=({struct Cyc_List_List*_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF->hd=(
void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp500=_cycalloc(sizeof(*
_tmp500));_tmp500[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp501;_tmp501.tag=1;
_tmp501.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp501.f2=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp501.f3=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp501;});_tmp500;}));
_tmp4FF->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp4FF;});_tmp4FE;});_tmp4FD;});break;case 139: _LL243: yyval=(
void*)({struct Cyc_Pointer_Sort_tok_struct*_tmp502=_cycalloc(sizeof(*_tmp502));
_tmp502[0]=({struct Cyc_Pointer_Sort_tok_struct _tmp503;_tmp503.tag=Cyc_Pointer_Sort_tok;
_tmp503.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*_tmp504=_cycalloc(
sizeof(*_tmp504));_tmp504[0]=({struct Cyc_Absyn_Nullable_ps_struct _tmp505;_tmp505.tag=
1;_tmp505.f1=Cyc_Absyn_exp_unsigned_one;_tmp505;});_tmp504;}));_tmp503;});
_tmp502;});break;case 140: _LL244: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp507;_tmp507.tag=Cyc_Pointer_Sort_tok;_tmp507.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp508=_cycalloc(sizeof(*_tmp508));_tmp508[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp509;_tmp509.tag=0;_tmp509.f1=Cyc_Absyn_exp_unsigned_one;_tmp509;});_tmp508;}));
_tmp507;});_tmp506;});break;case 141: _LL245: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp50A=_cycalloc(sizeof(*_tmp50A));_tmp50A[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp50B;_tmp50B.tag=Cyc_Pointer_Sort_tok;_tmp50B.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*
_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp50D;_tmp50D.tag=1;_tmp50D.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp50D;});_tmp50C;}));
_tmp50B;});_tmp50A;});break;case 142: _LL246: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp50F;_tmp50F.tag=Cyc_Pointer_Sort_tok;_tmp50F.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp511;_tmp511.tag=0;_tmp511.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp511;});_tmp510;}));
_tmp50F;});_tmp50E;});break;case 143: _LL247: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp513;_tmp513.tag=Cyc_Pointer_Sort_tok;_tmp513.f1=(void*)((void*)0);_tmp513;});
_tmp512;});break;case 144: _LL248: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp514=
_cycalloc(sizeof(*_tmp514));_tmp514[0]=({struct Cyc_Type_tok_struct _tmp515;
_tmp515.tag=Cyc_Type_tok;_tmp515.f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->v=(void*)((void*)3);_tmp516;}),0);
_tmp515;});_tmp514;});break;case 145: _LL249: yyval=yyvs[yyvsp_offset];break;case
146: _LL24A: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(
void*)3);yyval=yyvs[yyvsp_offset];break;case 147: _LL24B: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp517=_cycalloc(sizeof(*_tmp517));_tmp517[0]=({struct Cyc_Type_tok_struct
_tmp518;_tmp518.tag=Cyc_Type_tok;_tmp518.f1=(void*)Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519->v=(void*)((void*)3);
_tmp519;}),0);_tmp518;});_tmp517;});break;case 148: _LL24C: yyval=yyvs[yyvsp_offset];
break;case 149: _LL24D: yyval=(void*)({struct Cyc_TypeQual_tok_struct*_tmp51A=
_cycalloc_atomic(sizeof(*_tmp51A));_tmp51A[0]=({struct Cyc_TypeQual_tok_struct
_tmp51B;_tmp51B.tag=Cyc_TypeQual_tok;_tmp51B.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[yyvsp_offset]));_tmp51B;});_tmp51A;});break;case 150: _LL24E: yyval=(void*)({
struct Cyc_YY1_struct*_tmp51C=_cycalloc(sizeof(*_tmp51C));_tmp51C[0]=({struct Cyc_YY1_struct
_tmp51D;_tmp51D.tag=Cyc_YY1;_tmp51D.f1=({struct _tuple13*_tmp51E=_cycalloc(
sizeof(*_tmp51E));_tmp51E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp51E->f2=0;_tmp51E->f3=
0;_tmp51E->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp51E->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp51E;});_tmp51D;});_tmp51C;});break;case 151: _LL24F: yyval=(void*)({struct Cyc_YY1_struct*
_tmp51F=_cycalloc(sizeof(*_tmp51F));_tmp51F[0]=({struct Cyc_YY1_struct _tmp520;
_tmp520.tag=Cyc_YY1;_tmp520.f1=({struct _tuple13*_tmp521=_cycalloc(sizeof(*
_tmp521));_tmp521->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]));_tmp521->f2=1;_tmp521->f3=0;_tmp521->f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp521->f5=Cyc_yyget_Rgnorder_tok(
yyvs[yyvsp_offset]);_tmp521;});_tmp520;});_tmp51F;});break;case 152: _LL250: {
struct _tuple2 _tmp523;struct Cyc_Core_Opt*_tmp524;struct Cyc_Absyn_Tqual _tmp525;
void*_tmp526;struct _tuple2*_tmp522=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp523=*_tmp522;_tmp524=
_tmp523.f1;_tmp525=_tmp523.f2;_tmp526=_tmp523.f3;{struct Cyc_Absyn_VarargInfo*
_tmp527=({struct Cyc_Absyn_VarargInfo*_tmp52B=_cycalloc(sizeof(*_tmp52B));_tmp52B->name=
_tmp524;_tmp52B->tq=_tmp525;_tmp52B->type=(void*)_tmp526;_tmp52B->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp52B;});yyval=(
void*)({struct Cyc_YY1_struct*_tmp528=_cycalloc(sizeof(*_tmp528));_tmp528[0]=({
struct Cyc_YY1_struct _tmp529;_tmp529.tag=Cyc_YY1;_tmp529.f1=({struct _tuple13*
_tmp52A=_cycalloc(sizeof(*_tmp52A));_tmp52A->f1=0;_tmp52A->f2=0;_tmp52A->f3=
_tmp527;_tmp52A->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp52A->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp52A;});_tmp529;});_tmp528;});break;}}case 153: _LL251: {struct _tuple2 _tmp52D;
struct Cyc_Core_Opt*_tmp52E;struct Cyc_Absyn_Tqual _tmp52F;void*_tmp530;struct
_tuple2*_tmp52C=Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp52D=*_tmp52C;_tmp52E=_tmp52D.f1;_tmp52F=_tmp52D.f2;
_tmp530=_tmp52D.f3;{struct Cyc_Absyn_VarargInfo*_tmp531=({struct Cyc_Absyn_VarargInfo*
_tmp535=_cycalloc(sizeof(*_tmp535));_tmp535->name=_tmp52E;_tmp535->tq=_tmp52F;
_tmp535->type=(void*)_tmp530;_tmp535->inject=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp535;});yyval=(void*)({
struct Cyc_YY1_struct*_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532[0]=({struct Cyc_YY1_struct
_tmp533;_tmp533.tag=Cyc_YY1;_tmp533.f1=({struct _tuple13*_tmp534=_cycalloc(
sizeof(*_tmp534));_tmp534->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp534->f2=0;_tmp534->f3=
_tmp531;_tmp534->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp534->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp534;});_tmp533;});_tmp532;});break;}}case 154: _LL252: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp536=_cycalloc(sizeof(*_tmp536));_tmp536[0]=({struct Cyc_Type_tok_struct
_tmp537;_tmp537.tag=Cyc_Type_tok;_tmp537.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp538=_cycalloc(
sizeof(*_tmp538));_tmp538[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp539;_tmp539.tag=
1;_tmp539.f1=0;_tmp539;});_tmp538;}));_tmp537;});_tmp536;});break;case 155: _LL253:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp53A=_cycalloc(sizeof(*_tmp53A));
_tmp53A[0]=({struct Cyc_Type_tok_struct _tmp53B;_tmp53B.tag=Cyc_Type_tok;_tmp53B.f1=(
void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp53C=_cycalloc(
sizeof(*_tmp53C));_tmp53C[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp53D;_tmp53D.tag=0;
_tmp53D.f1=(void*)Cyc_yyget_Kind_tok(yyvs[yyvsp_offset]);_tmp53D;});_tmp53C;}));
_tmp53B;});_tmp53A;});break;case 156: _LL254: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp53E=_cycalloc(sizeof(*_tmp53E));_tmp53E[0]=({struct Cyc_TypeOpt_tok_struct
_tmp53F;_tmp53F.tag=Cyc_TypeOpt_tok;_tmp53F.f1=0;_tmp53F;});_tmp53E;});break;
case 157: _LL255: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp540=_cycalloc(
sizeof(*_tmp540));_tmp540[0]=({struct Cyc_TypeOpt_tok_struct _tmp541;_tmp541.tag=
Cyc_TypeOpt_tok;_tmp541.f1=({struct Cyc_Core_Opt*_tmp542=_cycalloc(sizeof(*
_tmp542));_tmp542->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp543=
_cycalloc(sizeof(*_tmp543));_tmp543[0]=({struct Cyc_Absyn_JoinEff_struct _tmp544;
_tmp544.tag=18;_tmp544.f1=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp544;});
_tmp543;}));_tmp542;});_tmp541;});_tmp540;});break;case 158: _LL256: yyval=(void*)({
struct Cyc_Rgnorder_tok_struct*_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545[0]=({
struct Cyc_Rgnorder_tok_struct _tmp546;_tmp546.tag=Cyc_Rgnorder_tok;_tmp546.f1=0;
_tmp546;});_tmp545;});break;case 159: _LL257: yyval=yyvs[yyvsp_offset];break;case
160: _LL258: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp547=_cycalloc(
sizeof(*_tmp547));_tmp547[0]=({struct Cyc_Rgnorder_tok_struct _tmp548;_tmp548.tag=
Cyc_Rgnorder_tok;_tmp548.f1=({struct Cyc_List_List*_tmp549=_cycalloc(sizeof(*
_tmp549));_tmp549->hd=({struct _tuple16*_tmp54A=_cycalloc(sizeof(*_tmp54A));
_tmp54A->f1=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp54E;_tmp54E.tag=0;_tmp54E.f1=(void*)((void*)3);_tmp54E;});_tmp54D;}));
_tmp54A->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),(void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp54C;_tmp54C.tag=0;_tmp54C.f1=(void*)((void*)3);
_tmp54C;});_tmp54B;}));_tmp54A;});_tmp549->tl=0;_tmp549;});_tmp548;});_tmp547;});
break;case 161: _LL259: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp54F=
_cycalloc(sizeof(*_tmp54F));_tmp54F[0]=({struct Cyc_Rgnorder_tok_struct _tmp550;
_tmp550.tag=Cyc_Rgnorder_tok;_tmp550.f1=({struct Cyc_List_List*_tmp551=_cycalloc(
sizeof(*_tmp551));_tmp551->hd=({struct _tuple16*_tmp552=_cycalloc(sizeof(*_tmp552));
_tmp552->f1=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp556;_tmp556.tag=0;_tmp556.f1=(void*)((void*)3);_tmp556;});_tmp555;}));
_tmp552->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp554;_tmp554.tag=0;_tmp554.f1=(void*)((void*)3);_tmp554;});_tmp553;}));
_tmp552;});_tmp551->tl=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);_tmp551;});
_tmp550;});_tmp54F;});break;case 162: _LL25A: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp557=_cycalloc_atomic(sizeof(*_tmp557));_tmp557[0]=({struct Cyc_Bool_tok_struct
_tmp558;_tmp558.tag=Cyc_Bool_tok;_tmp558.f1=0;_tmp558;});_tmp557;});break;case
163: _LL25B: if(Cyc_Std_zstrcmp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),_tag_arr("inject",
sizeof(unsigned char),7))!= 0)Cyc_Parse_err(_tag_arr("missing type in function declaration",
sizeof(unsigned char),37),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Bool_tok_struct*_tmp559=
_cycalloc_atomic(sizeof(*_tmp559));_tmp559[0]=({struct Cyc_Bool_tok_struct _tmp55A;
_tmp55A.tag=Cyc_Bool_tok;_tmp55A.f1=1;_tmp55A;});_tmp559;});break;case 164: _LL25C:
yyval=yyvs[yyvsp_offset];break;case 165: _LL25D: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B[0]=({struct Cyc_TypeList_tok_struct
_tmp55C;_tmp55C.tag=Cyc_TypeList_tok;_tmp55C.f1=Cyc_List_imp_append(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeList_tok(
yyvs[yyvsp_offset]));_tmp55C;});_tmp55B;});break;case 166: _LL25E: yyval=(void*)({
struct Cyc_TypeList_tok_struct*_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D[0]=({
struct Cyc_TypeList_tok_struct _tmp55E;_tmp55E.tag=Cyc_TypeList_tok;_tmp55E.f1=0;
_tmp55E;});_tmp55D;});break;case 167: _LL25F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 168: _LL260:
yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp55F=_cycalloc(sizeof(*_tmp55F));
_tmp55F[0]=({struct Cyc_TypeList_tok_struct _tmp560;_tmp560.tag=Cyc_TypeList_tok;
_tmp560.f1=({struct Cyc_List_List*_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp562=_cycalloc(sizeof(*_tmp562));
_tmp562[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp563;_tmp563.tag=19;_tmp563.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp563;});_tmp562;}));_tmp561->tl=0;_tmp561;});_tmp560;});_tmp55F;});
break;case 169: _LL261: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
yyvsp_offset]),(void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp564=
_cycalloc(sizeof(*_tmp564));_tmp564[0]=({struct Cyc_TypeList_tok_struct _tmp565;
_tmp565.tag=Cyc_TypeList_tok;_tmp565.f1=({struct Cyc_List_List*_tmp566=_cycalloc(
sizeof(*_tmp566));_tmp566->hd=(void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);
_tmp566->tl=0;_tmp566;});_tmp565;});_tmp564;});break;case 170: _LL262: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp567=_cycalloc(sizeof(*_tmp567));_tmp567[0]=({struct Cyc_TypeList_tok_struct
_tmp568;_tmp568.tag=Cyc_TypeList_tok;_tmp568.f1=({struct Cyc_List_List*_tmp569=
_cycalloc(sizeof(*_tmp569));_tmp569->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp56B;_tmp56B.tag=17;_tmp56B.f1=(void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);
_tmp56B;});_tmp56A;}));_tmp569->tl=0;_tmp569;});_tmp568;});_tmp567;});break;case
171: _LL263: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(void*)({
struct Cyc_TypeList_tok_struct*_tmp56C=_cycalloc(sizeof(*_tmp56C));_tmp56C[0]=({
struct Cyc_TypeList_tok_struct _tmp56D;_tmp56D.tag=Cyc_TypeList_tok;_tmp56D.f1=({
struct Cyc_List_List*_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));
_tmp56F[0]=({struct Cyc_Absyn_AccessEff_struct _tmp570;_tmp570.tag=17;_tmp570.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp570;});_tmp56F;}));_tmp56E->tl=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);
_tmp56E;});_tmp56D;});_tmp56C;});break;case 172: _LL264: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp572;_tmp572.tag=Cyc_ParamDeclList_tok;_tmp572.f1=({struct Cyc_List_List*
_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->hd=Cyc_yyget_ParamDecl_tok(yyvs[
yyvsp_offset]);_tmp573->tl=0;_tmp573;});_tmp572;});_tmp571;});break;case 173:
_LL265: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*_tmp574=_cycalloc(
sizeof(*_tmp574));_tmp574[0]=({struct Cyc_ParamDeclList_tok_struct _tmp575;_tmp575.tag=
Cyc_ParamDeclList_tok;_tmp575.f1=({struct Cyc_List_List*_tmp576=_cycalloc(sizeof(*
_tmp576));_tmp576->hd=Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset]);_tmp576->tl=
Cyc_yyget_ParamDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp576;});_tmp575;});_tmp574;});break;case 174: _LL266: {
struct _tuple14 _tmp578;struct Cyc_Absyn_Tqual _tmp579;struct Cyc_List_List*_tmp57A;
struct Cyc_List_List*_tmp57B;struct _tuple14*_tmp577=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp578=*_tmp577;
_tmp579=_tmp578.f1;_tmp57A=_tmp578.f2;_tmp57B=_tmp578.f3;{struct Cyc_Parse_Declarator
_tmp57D;struct _tuple1*_tmp57E;struct Cyc_List_List*_tmp57F;struct Cyc_Parse_Declarator*
_tmp57C=Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]);_tmp57D=*_tmp57C;_tmp57E=
_tmp57D.id;_tmp57F=_tmp57D.tms;{void*_tmp580=Cyc_Parse_speclist2typ(_tmp57A,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual
_tmp582;void*_tmp583;struct Cyc_List_List*_tmp584;struct Cyc_List_List*_tmp585;
struct _tuple5 _tmp581=Cyc_Parse_apply_tms(_tmp579,_tmp580,_tmp57B,_tmp57F);
_tmp582=_tmp581.f1;_tmp583=_tmp581.f2;_tmp584=_tmp581.f3;_tmp585=_tmp581.f4;if(
_tmp584 != 0)Cyc_Parse_err(_tag_arr("parameter with bad type params",sizeof(
unsigned char),31),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));if(Cyc_Absyn_is_qvar_qualified(_tmp57E))Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a namespace",sizeof(unsigned char),
47),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*_tmp586=(struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt*_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->v=(*_tmp57E).f2;
_tmp58B;});if(_tmp585 != 0)({void*_tmp587[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("extra attributes on parameter, ignoring",
sizeof(unsigned char),40),_tag_arr(_tmp587,sizeof(void*),0));});yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588[0]=({
struct Cyc_ParamDecl_tok_struct _tmp589;_tmp589.tag=Cyc_ParamDecl_tok;_tmp589.f1=({
struct _tuple2*_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A->f1=_tmp586;_tmp58A->f2=
_tmp582;_tmp58A->f3=_tmp583;_tmp58A;});_tmp589;});_tmp588;});break;}}}}case 175:
_LL267: {struct _tuple14 _tmp58D;struct Cyc_Absyn_Tqual _tmp58E;struct Cyc_List_List*
_tmp58F;struct Cyc_List_List*_tmp590;struct _tuple14*_tmp58C=Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset]);_tmp58D=*_tmp58C;_tmp58E=_tmp58D.f1;_tmp58F=_tmp58D.f2;
_tmp590=_tmp58D.f3;{void*_tmp591=Cyc_Parse_speclist2typ(_tmp58F,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));if(_tmp590 != 0)({
void*_tmp592[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(unsigned char),38),_tag_arr(_tmp592,sizeof(void*),0));});yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593[0]=({
struct Cyc_ParamDecl_tok_struct _tmp594;_tmp594.tag=Cyc_ParamDecl_tok;_tmp594.f1=({
struct _tuple2*_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595->f1=0;_tmp595->f2=
_tmp58E;_tmp595->f3=_tmp591;_tmp595;});_tmp594;});_tmp593;});break;}}case 176:
_LL268: {struct _tuple14 _tmp597;struct Cyc_Absyn_Tqual _tmp598;struct Cyc_List_List*
_tmp599;struct Cyc_List_List*_tmp59A;struct _tuple14*_tmp596=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp597=*_tmp596;
_tmp598=_tmp597.f1;_tmp599=_tmp597.f2;_tmp59A=_tmp597.f3;{void*_tmp59B=Cyc_Parse_speclist2typ(
_tmp599,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp59C=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[yyvsp_offset]))->tms;struct Cyc_Absyn_Tqual _tmp59E;void*_tmp59F;struct Cyc_List_List*
_tmp5A0;struct Cyc_List_List*_tmp5A1;struct _tuple5 _tmp59D=Cyc_Parse_apply_tms(
_tmp598,_tmp59B,_tmp59A,_tmp59C);_tmp59E=_tmp59D.f1;_tmp59F=_tmp59D.f2;_tmp5A0=
_tmp59D.f3;_tmp5A1=_tmp59D.f4;if(_tmp5A0 != 0)({void*_tmp5A2[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad type parameters on formal argument, ignoring",
sizeof(unsigned char),49),_tag_arr(_tmp5A2,sizeof(void*),0));});if(_tmp5A1 != 0)({
void*_tmp5A3[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(unsigned char),38),_tag_arr(_tmp5A3,sizeof(void*),0));});yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4[0]=({
struct Cyc_ParamDecl_tok_struct _tmp5A5;_tmp5A5.tag=Cyc_ParamDecl_tok;_tmp5A5.f1=({
struct _tuple2*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));_tmp5A6->f1=0;_tmp5A6->f2=
_tmp59E;_tmp5A6->f3=_tmp59F;_tmp5A6;});_tmp5A5;});_tmp5A4;});break;}}case 177:
_LL269: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5A7=_cycalloc(sizeof(*
_tmp5A7));_tmp5A7[0]=({struct Cyc_IdList_tok_struct _tmp5A8;_tmp5A8.tag=Cyc_IdList_tok;
_tmp5A8.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(yyvs[yyvsp_offset]));_tmp5A8;});_tmp5A7;});break;case 178:
_LL26A: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5A9=_cycalloc(sizeof(*
_tmp5A9));_tmp5A9[0]=({struct Cyc_IdList_tok_struct _tmp5AA;_tmp5AA.tag=Cyc_IdList_tok;
_tmp5AA.f1=({struct Cyc_List_List*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB->hd=({
struct _tagged_arr*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));_tmp5AC[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp5AC;});_tmp5AB->tl=0;_tmp5AB;});_tmp5AA;});_tmp5A9;});
break;case 179: _LL26B: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5AD=
_cycalloc(sizeof(*_tmp5AD));_tmp5AD[0]=({struct Cyc_IdList_tok_struct _tmp5AE;
_tmp5AE.tag=Cyc_IdList_tok;_tmp5AE.f1=({struct Cyc_List_List*_tmp5AF=_cycalloc(
sizeof(*_tmp5AF));_tmp5AF->hd=({struct _tagged_arr*_tmp5B0=_cycalloc(sizeof(*
_tmp5B0));_tmp5B0[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp5B0;});_tmp5AF->tl=
Cyc_yyget_IdList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5AF;});_tmp5AE;});_tmp5AD;});break;case 180: _LL26C: yyval=yyvs[yyvsp_offset];
break;case 181: _LL26D: yyval=yyvs[yyvsp_offset];break;case 182: _LL26E: yyval=(void*)({
struct Cyc_Exp_tok_struct*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1[0]=({struct
Cyc_Exp_tok_struct _tmp5B2;_tmp5B2.tag=Cyc_Exp_tok;_tmp5B2.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5B3=_cycalloc(sizeof(*_tmp5B3));
_tmp5B3[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5B4;_tmp5B4.tag=34;
_tmp5B4.f1=0;_tmp5B4.f2=0;_tmp5B4;});_tmp5B3;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5B2;});_tmp5B1;});break;case 183: _LL26F: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5[0]=({
struct Cyc_Exp_tok_struct _tmp5B6;_tmp5B6.tag=Cyc_Exp_tok;_tmp5B6.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));
_tmp5B7[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5B8;_tmp5B8.tag=34;
_tmp5B8.f1=0;_tmp5B8.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp5B8;});_tmp5B7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5B6;});_tmp5B5;});break;case 184: _LL270: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9[0]=({
struct Cyc_Exp_tok_struct _tmp5BA;_tmp5BA.tag=Cyc_Exp_tok;_tmp5BA.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));
_tmp5BB[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5BC;_tmp5BC.tag=34;
_tmp5BC.f1=0;_tmp5BC.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp5BC;});_tmp5BB;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5BA;});_tmp5B9;});break;case 185: _LL271: {struct
Cyc_Absyn_Vardecl*_tmp5BD=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp5C3=
_cycalloc(sizeof(*_tmp5C3));_tmp5C3->f1=(void*)0;_tmp5C3->f2=({struct _tagged_arr*
_tmp5C4=_cycalloc(sizeof(*_tmp5C4));_tmp5C4[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp5C4;});_tmp5C3;}),
Cyc_Absyn_uint_t,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));_tmp5BD->tq=({
struct Cyc_Absyn_Tqual _tmp5BE;_tmp5BE.q_const=1;_tmp5BE.q_volatile=0;_tmp5BE.q_restrict=
1;_tmp5BE;});yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp5BF=_cycalloc(sizeof(*
_tmp5BF));_tmp5BF[0]=({struct Cyc_Exp_tok_struct _tmp5C0;_tmp5C0.tag=Cyc_Exp_tok;
_tmp5C0.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp5C2;_tmp5C2.tag=27;_tmp5C2.f1=_tmp5BD;_tmp5C2.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5C2.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5C2;});_tmp5C1;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmp5C0;});
_tmp5BF;});break;}case 186: _LL272: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp5C5=_cycalloc(sizeof(*_tmp5C5));_tmp5C5[0]=({struct Cyc_InitializerList_tok_struct
_tmp5C6;_tmp5C6.tag=Cyc_InitializerList_tok;_tmp5C6.f1=({struct Cyc_List_List*
_tmp5C7=_cycalloc(sizeof(*_tmp5C7));_tmp5C7->hd=({struct _tuple17*_tmp5C8=
_cycalloc(sizeof(*_tmp5C8));_tmp5C8->f1=0;_tmp5C8->f2=Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]);_tmp5C8;});_tmp5C7->tl=0;_tmp5C7;});_tmp5C6;});_tmp5C5;});break;
case 187: _LL273: yyval=(void*)({struct Cyc_InitializerList_tok_struct*_tmp5C9=
_cycalloc(sizeof(*_tmp5C9));_tmp5C9[0]=({struct Cyc_InitializerList_tok_struct
_tmp5CA;_tmp5CA.tag=Cyc_InitializerList_tok;_tmp5CA.f1=({struct Cyc_List_List*
_tmp5CB=_cycalloc(sizeof(*_tmp5CB));_tmp5CB->hd=({struct _tuple17*_tmp5CC=
_cycalloc(sizeof(*_tmp5CC));_tmp5CC->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5CC->f2=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp5CC;});_tmp5CB->tl=0;_tmp5CB;});_tmp5CA;});_tmp5C9;});
break;case 188: _LL274: yyval=(void*)({struct Cyc_InitializerList_tok_struct*_tmp5CD=
_cycalloc(sizeof(*_tmp5CD));_tmp5CD[0]=({struct Cyc_InitializerList_tok_struct
_tmp5CE;_tmp5CE.tag=Cyc_InitializerList_tok;_tmp5CE.f1=({struct Cyc_List_List*
_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF->hd=({struct _tuple17*_tmp5D0=
_cycalloc(sizeof(*_tmp5D0));_tmp5D0->f1=0;_tmp5D0->f2=Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]);_tmp5D0;});_tmp5CF->tl=Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5CF;});_tmp5CE;});
_tmp5CD;});break;case 189: _LL275: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp5D1=_cycalloc(sizeof(*_tmp5D1));_tmp5D1[0]=({struct Cyc_InitializerList_tok_struct
_tmp5D2;_tmp5D2.tag=Cyc_InitializerList_tok;_tmp5D2.f1=({struct Cyc_List_List*
_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3->hd=({struct _tuple17*_tmp5D4=
_cycalloc(sizeof(*_tmp5D4));_tmp5D4->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D4->f2=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp5D4;});_tmp5D3->tl=Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5D3;});_tmp5D2;});
_tmp5D1;});break;case 190: _LL276: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5[0]=({struct Cyc_DesignatorList_tok_struct
_tmp5D6;_tmp5D6.tag=Cyc_DesignatorList_tok;_tmp5D6.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp5D6;});_tmp5D5;});
break;case 191: _LL277: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp5D7=
_cycalloc(sizeof(*_tmp5D7));_tmp5D7[0]=({struct Cyc_DesignatorList_tok_struct
_tmp5D8;_tmp5D8.tag=Cyc_DesignatorList_tok;_tmp5D8.f1=({struct Cyc_List_List*
_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[yyvsp_offset]);_tmp5D9->tl=0;_tmp5D9;});_tmp5D8;});_tmp5D7;});break;case 192:
_LL278: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp5DA=_cycalloc(
sizeof(*_tmp5DA));_tmp5DA[0]=({struct Cyc_DesignatorList_tok_struct _tmp5DB;
_tmp5DB.tag=Cyc_DesignatorList_tok;_tmp5DB.f1=({struct Cyc_List_List*_tmp5DC=
_cycalloc(sizeof(*_tmp5DC));_tmp5DC->hd=(void*)Cyc_yyget_Designator_tok(yyvs[
yyvsp_offset]);_tmp5DC->tl=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5DC;});_tmp5DB;});
_tmp5DA;});break;case 193: _LL279: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD[0]=({struct Cyc_Designator_tok_struct
_tmp5DE;_tmp5DE.tag=Cyc_Designator_tok;_tmp5DE.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp5E0;_tmp5E0.tag=0;_tmp5E0.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E0;});_tmp5DF;}));
_tmp5DE;});_tmp5DD;});break;case 194: _LL27A: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1[0]=({struct Cyc_Designator_tok_struct
_tmp5E2;_tmp5E2.tag=Cyc_Designator_tok;_tmp5E2.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp5E3=_cycalloc(sizeof(*_tmp5E3));_tmp5E3[0]=({struct Cyc_Absyn_FieldName_struct
_tmp5E4;_tmp5E4.tag=1;_tmp5E4.f1=({struct _tagged_arr*_tmp5E5=_cycalloc(sizeof(*
_tmp5E5));_tmp5E5[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp5E5;});_tmp5E4;});
_tmp5E3;}));_tmp5E2;});_tmp5E1;});break;case 195: _LL27B: {void*_tmp5E6=Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));struct Cyc_List_List*
_tmp5E7=(*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f3;if(_tmp5E7 != 0)({
void*_tmp5E8[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("ignoring attributes in type",
sizeof(unsigned char),28),_tag_arr(_tmp5E8,sizeof(void*),0));});{struct Cyc_Absyn_Tqual
_tmp5E9=(*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f1;yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA[0]=({
struct Cyc_ParamDecl_tok_struct _tmp5EB;_tmp5EB.tag=Cyc_ParamDecl_tok;_tmp5EB.f1=({
struct _tuple2*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->f1=0;_tmp5EC->f2=
_tmp5E9;_tmp5EC->f3=_tmp5E6;_tmp5EC;});_tmp5EB;});_tmp5EA;});break;}}case 196:
_LL27C: {void*_tmp5ED=Cyc_Parse_speclist2typ((*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp5EE=(*Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f3;struct Cyc_Absyn_Tqual _tmp5EF=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f1;struct Cyc_List_List*
_tmp5F0=(Cyc_yyget_AbstractDeclarator_tok(yyvs[yyvsp_offset]))->tms;struct
_tuple5 _tmp5F1=Cyc_Parse_apply_tms(_tmp5EF,_tmp5ED,_tmp5EE,_tmp5F0);if(_tmp5F1.f3
!= 0)({void*_tmp5F2[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad type params, ignoring",
sizeof(unsigned char),26),_tag_arr(_tmp5F2,sizeof(void*),0));});if(_tmp5F1.f4 != 
0)({void*_tmp5F3[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad specifiers, ignoring",
sizeof(unsigned char),25),_tag_arr(_tmp5F3,sizeof(void*),0));});yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4[0]=({
struct Cyc_ParamDecl_tok_struct _tmp5F5;_tmp5F5.tag=Cyc_ParamDecl_tok;_tmp5F5.f1=({
struct _tuple2*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->f1=0;_tmp5F6->f2=
_tmp5F1.f1;_tmp5F6->f3=_tmp5F1.f2;_tmp5F6;});_tmp5F5;});_tmp5F4;});break;}case
197: _LL27D: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5F7=_cycalloc(sizeof(*
_tmp5F7));_tmp5F7[0]=({struct Cyc_Type_tok_struct _tmp5F8;_tmp5F8.tag=Cyc_Type_tok;
_tmp5F8.f1=(void*)(*Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset])).f3;_tmp5F8;});
_tmp5F7;});break;case 198: _LL27E: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5F9=
_cycalloc(sizeof(*_tmp5F9));_tmp5F9[0]=({struct Cyc_Type_tok_struct _tmp5FA;
_tmp5FA.tag=Cyc_Type_tok;_tmp5FA.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp5FC;_tmp5FC.tag=18;_tmp5FC.f1=0;_tmp5FC;});_tmp5FB;}));_tmp5FA;});_tmp5F9;});
break;case 199: _LL27F: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5FD=_cycalloc(
sizeof(*_tmp5FD));_tmp5FD[0]=({struct Cyc_Type_tok_struct _tmp5FE;_tmp5FE.tag=Cyc_Type_tok;
_tmp5FE.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5FF=_cycalloc(
sizeof(*_tmp5FF));_tmp5FF[0]=({struct Cyc_Absyn_JoinEff_struct _tmp600;_tmp600.tag=
18;_tmp600.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp600;});_tmp5FF;}));_tmp5FE;});_tmp5FD;});break;case 200:
_LL280: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp601=_cycalloc(sizeof(*
_tmp601));_tmp601[0]=({struct Cyc_Type_tok_struct _tmp602;_tmp602.tag=Cyc_Type_tok;
_tmp602.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp603=_cycalloc(
sizeof(*_tmp603));_tmp603[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp604;_tmp604.tag=
19;_tmp604.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp604;});_tmp603;}));_tmp602;});_tmp601;});break;case
201: _LL281: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp605=_cycalloc(sizeof(*
_tmp605));_tmp605[0]=({struct Cyc_Type_tok_struct _tmp606;_tmp606.tag=Cyc_Type_tok;
_tmp606.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp607=_cycalloc(
sizeof(*_tmp607));_tmp607[0]=({struct Cyc_Absyn_JoinEff_struct _tmp608;_tmp608.tag=
18;_tmp608.f1=({struct Cyc_List_List*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp609->tl=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp609;});_tmp608;});
_tmp607;}));_tmp606;});_tmp605;});break;case 202: _LL282: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A[0]=({struct Cyc_TypeList_tok_struct
_tmp60B;_tmp60B.tag=Cyc_TypeList_tok;_tmp60B.f1=({struct Cyc_List_List*_tmp60C=
_cycalloc(sizeof(*_tmp60C));_tmp60C->hd=(void*)Cyc_yyget_Type_tok(yyvs[
yyvsp_offset]);_tmp60C->tl=0;_tmp60C;});_tmp60B;});_tmp60A;});break;case 203:
_LL283: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp60D=_cycalloc(sizeof(*
_tmp60D));_tmp60D[0]=({struct Cyc_TypeList_tok_struct _tmp60E;_tmp60E.tag=Cyc_TypeList_tok;
_tmp60E.f1=({struct Cyc_List_List*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->hd=(
void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);_tmp60F->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp60F;});_tmp60E;});
_tmp60D;});break;case 204: _LL284: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp611;_tmp611.tag=Cyc_AbstractDeclarator_tok;_tmp611.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]);_tmp612;});_tmp611;});_tmp610;});break;case 205: _LL285: yyval=
yyvs[yyvsp_offset];break;case 206: _LL286: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp614;_tmp614.tag=Cyc_AbstractDeclarator_tok;_tmp614.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[yyvsp_offset]))->tms);_tmp615;});_tmp614;});_tmp613;});break;case 207: _LL287:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 208:
_LL288: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp616=_cycalloc(
sizeof(*_tmp616));_tmp616[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp617;
_tmp617.tag=Cyc_AbstractDeclarator_tok;_tmp617.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618->tms=({struct Cyc_List_List*_tmp619=
_cycalloc(sizeof(*_tmp619));_tmp619->hd=(void*)((void*)0);_tmp619->tl=0;_tmp619;});
_tmp618;});_tmp617;});_tmp616;});break;case 209: _LL289: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp61B;_tmp61B.tag=Cyc_AbstractDeclarator_tok;_tmp61B.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp61C=_cycalloc(sizeof(*_tmp61C));_tmp61C->tms=({struct Cyc_List_List*_tmp61D=
_cycalloc(sizeof(*_tmp61D));_tmp61D->hd=(void*)((void*)0);_tmp61D->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->tms;_tmp61D;});
_tmp61C;});_tmp61B;});_tmp61A;});break;case 210: _LL28A: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp61F;_tmp61F.tag=Cyc_AbstractDeclarator_tok;_tmp61F.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->tms=({struct Cyc_List_List*_tmp621=
_cycalloc(sizeof(*_tmp621));_tmp621->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp623;_tmp623.tag=0;_tmp623.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp623;});_tmp622;}));
_tmp621->tl=0;_tmp621;});_tmp620;});_tmp61F;});_tmp61E;});break;case 211: _LL28B:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp624=_cycalloc(sizeof(*
_tmp624));_tmp624[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp625;_tmp625.tag=
Cyc_AbstractDeclarator_tok;_tmp625.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->tms=({struct Cyc_List_List*_tmp627=
_cycalloc(sizeof(*_tmp627));_tmp627->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp629;_tmp629.tag=0;_tmp629.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp629;});_tmp628;}));
_tmp627->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp627;});_tmp626;});
_tmp625;});_tmp624;});break;case 212: _LL28C: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp62B;_tmp62B.tag=Cyc_AbstractDeclarator_tok;_tmp62B.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->tms=({struct Cyc_List_List*_tmp62D=
_cycalloc(sizeof(*_tmp62D));_tmp62D->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp62F;_tmp62F.tag=2;_tmp62F.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp631;_tmp631.tag=1;_tmp631.f1=0;_tmp631.f2=0;_tmp631.f3=0;_tmp631.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp631.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp631;});_tmp630;}));
_tmp62F;});_tmp62E;}));_tmp62D->tl=0;_tmp62D;});_tmp62C;});_tmp62B;});_tmp62A;});
break;case 213: _LL28D: {struct _tuple13 _tmp633;struct Cyc_List_List*_tmp634;int
_tmp635;struct Cyc_Absyn_VarargInfo*_tmp636;struct Cyc_Core_Opt*_tmp637;struct Cyc_List_List*
_tmp638;struct _tuple13*_tmp632=Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp633=*_tmp632;_tmp634=_tmp633.f1;_tmp635=_tmp633.f2;
_tmp636=_tmp633.f3;_tmp637=_tmp633.f4;_tmp638=_tmp633.f5;yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp63A;_tmp63A.tag=Cyc_AbstractDeclarator_tok;
_tmp63A.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp63B=_cycalloc(sizeof(*
_tmp63B));_tmp63B->tms=({struct Cyc_List_List*_tmp63C=_cycalloc(sizeof(*_tmp63C));
_tmp63C->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp63D=
_cycalloc(sizeof(*_tmp63D));_tmp63D[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp63E;_tmp63E.tag=2;_tmp63E.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp640;_tmp640.tag=1;_tmp640.f1=_tmp634;_tmp640.f2=_tmp635;_tmp640.f3=_tmp636;
_tmp640.f4=_tmp637;_tmp640.f5=_tmp638;_tmp640;});_tmp63F;}));_tmp63E;});_tmp63D;}));
_tmp63C->tl=0;_tmp63C;});_tmp63B;});_tmp63A;});_tmp639;});break;}case 214: _LL28E:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp641=_cycalloc(sizeof(*
_tmp641));_tmp641[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp642;_tmp642.tag=
Cyc_AbstractDeclarator_tok;_tmp642.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp643=_cycalloc(sizeof(*_tmp643));_tmp643->tms=({struct Cyc_List_List*_tmp644=
_cycalloc(sizeof(*_tmp644));_tmp644->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp646;_tmp646.tag=2;_tmp646.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp648;_tmp648.tag=1;_tmp648.f1=0;_tmp648.f2=0;_tmp648.f3=0;_tmp648.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp648.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp648;});_tmp647;}));
_tmp646;});_tmp645;}));_tmp644->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp644;});_tmp643;});
_tmp642;});_tmp641;});break;case 215: _LL28F: {struct _tuple13 _tmp64A;struct Cyc_List_List*
_tmp64B;int _tmp64C;struct Cyc_Absyn_VarargInfo*_tmp64D;struct Cyc_Core_Opt*_tmp64E;
struct Cyc_List_List*_tmp64F;struct _tuple13*_tmp649=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp64A=*_tmp649;_tmp64B=
_tmp64A.f1;_tmp64C=_tmp64A.f2;_tmp64D=_tmp64A.f3;_tmp64E=_tmp64A.f4;_tmp64F=
_tmp64A.f5;yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp650=
_cycalloc(sizeof(*_tmp650));_tmp650[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp651;_tmp651.tag=Cyc_AbstractDeclarator_tok;_tmp651.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652->tms=({struct Cyc_List_List*_tmp653=
_cycalloc(sizeof(*_tmp653));_tmp653->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp655;_tmp655.tag=2;_tmp655.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp657;_tmp657.tag=1;_tmp657.f1=_tmp64B;_tmp657.f2=_tmp64C;_tmp657.f3=_tmp64D;
_tmp657.f4=_tmp64E;_tmp657.f5=_tmp64F;_tmp657;});_tmp656;}));_tmp655;});_tmp654;}));
_tmp653->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp653;});_tmp652;});
_tmp651;});_tmp650;});break;}case 216: _LL290: {struct Cyc_List_List*_tmp658=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp659=_cycalloc(sizeof(*
_tmp659));_tmp659[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp65A;_tmp65A.tag=
Cyc_AbstractDeclarator_tok;_tmp65A.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->tms=({struct Cyc_List_List*_tmp65C=
_cycalloc(sizeof(*_tmp65C));_tmp65C->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp65E;_tmp65E.tag=3;_tmp65E.f1=_tmp658;_tmp65E.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp65E.f3=0;_tmp65E;});_tmp65D;}));_tmp65C->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp65C;});
_tmp65B;});_tmp65A;});_tmp659;});break;}case 217: _LL291: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp660;_tmp660.tag=Cyc_AbstractDeclarator_tok;_tmp660.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->tms=({struct Cyc_List_List*_tmp662=
_cycalloc(sizeof(*_tmp662));_tmp662->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp664;_tmp664.tag=4;_tmp664.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp664.f2=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp664;});_tmp663;}));_tmp662->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp662;});
_tmp661;});_tmp660;});_tmp65F;});break;case 218: _LL292: yyval=yyvs[yyvsp_offset];
break;case 219: _LL293: yyval=yyvs[yyvsp_offset];break;case 220: _LL294: yyval=yyvs[
yyvsp_offset];break;case 221: _LL295: yyval=yyvs[yyvsp_offset];break;case 222: _LL296:
yyval=yyvs[yyvsp_offset];break;case 223: _LL297: yyval=yyvs[yyvsp_offset];break;
case 224: _LL298: if(Cyc_Std_zstrcmp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tag_arr("`H",sizeof(
unsigned char),3))== 0)Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(unsigned char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->name=({
struct _tagged_arr*_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp672;});_tmp66F->identity=
0;_tmp66F->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp670=_cycalloc(
sizeof(*_tmp670));_tmp670[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp671;_tmp671.tag=0;
_tmp671.f1=(void*)((void*)3);_tmp671;});_tmp670;}));_tmp66F;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D[0]=({
struct Cyc_Absyn_VarType_struct _tmp66E;_tmp66E.tag=1;_tmp66E.f1=tv;_tmp66E;});
_tmp66D;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp665=_cycalloc(sizeof(*
_tmp665));_tmp665[0]=({struct Cyc_Stmt_tok_struct _tmp666;_tmp666.tag=Cyc_Stmt_tok;
_tmp666.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp667=
_cycalloc(sizeof(*_tmp667));_tmp667[0]=({struct Cyc_Absyn_Region_s_struct _tmp668;
_tmp668.tag=18;_tmp668.f1=tv;_tmp668.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->f1=(void*)0;_tmp669->f2=({struct
_tagged_arr*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp66A;});_tmp669;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp66B=_cycalloc(sizeof(*_tmp66B));
_tmp66B[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp66C;_tmp66C.tag=15;_tmp66C.f1=(
void*)t;_tmp66C;});_tmp66B;}),0);_tmp668.f3=Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);
_tmp668;});_tmp667;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp666;});_tmp665;});break;}case 225: _LL299: if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tag_arr("H",sizeof(unsigned char),2))== 0)Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(unsigned char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D->name=({
struct _tagged_arr*_tmp680=_cycalloc(sizeof(*_tmp680));_tmp680[0]=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp682;_tmp682.tag=0;_tmp682.f1=(
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);{void*_tmp681[1]={& _tmp682};Cyc_Std_aprintf(_tag_arr("`%s",
sizeof(unsigned char),4),_tag_arr(_tmp681,sizeof(void*),1));}});_tmp680;});
_tmp67D->identity=0;_tmp67D->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp67F;_tmp67F.tag=0;_tmp67F.f1=(void*)((void*)3);_tmp67F;});_tmp67E;}));
_tmp67D;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp67B=_cycalloc(
sizeof(*_tmp67B));_tmp67B[0]=({struct Cyc_Absyn_VarType_struct _tmp67C;_tmp67C.tag=
1;_tmp67C.f1=tv;_tmp67C;});_tmp67B;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673[0]=({struct Cyc_Stmt_tok_struct
_tmp674;_tmp674.tag=Cyc_Stmt_tok;_tmp674.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675[0]=({
struct Cyc_Absyn_Region_s_struct _tmp676;_tmp676.tag=18;_tmp676.f1=tv;_tmp676.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp677=_cycalloc(sizeof(*_tmp677));
_tmp677->f1=(void*)0;_tmp677->f2=({struct _tagged_arr*_tmp678=_cycalloc(sizeof(*
_tmp678));_tmp678[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp678;});_tmp677;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp67A;_tmp67A.tag=15;_tmp67A.f1=(void*)t;_tmp67A;});_tmp679;}),0);_tmp676.f3=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp676;});_tmp675;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp674;});_tmp673;});break;}case 226: _LL29A: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683[0]=({
struct Cyc_Stmt_tok_struct _tmp684;_tmp684.tag=Cyc_Stmt_tok;_tmp684.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Cut_s_struct*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685[
0]=({struct Cyc_Absyn_Cut_s_struct _tmp686;_tmp686.tag=13;_tmp686.f1=Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]);_tmp686;});_tmp685;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp684;});_tmp683;});break;case 227: _LL29B: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687[0]=({
struct Cyc_Stmt_tok_struct _tmp688;_tmp688.tag=Cyc_Stmt_tok;_tmp688.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmp689=_cycalloc(sizeof(*_tmp689));
_tmp689[0]=({struct Cyc_Absyn_Splice_s_struct _tmp68A;_tmp68A.tag=14;_tmp68A.f1=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp68A;});_tmp689;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp688;});_tmp687;});break;case 228: _LL29C: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B[0]=({
struct Cyc_Stmt_tok_struct _tmp68C;_tmp68C.tag=Cyc_Stmt_tok;_tmp68C.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp68D=_cycalloc(sizeof(*_tmp68D));
_tmp68D[0]=({struct Cyc_Absyn_Label_s_struct _tmp68E;_tmp68E.tag=15;_tmp68E.f1=({
struct _tagged_arr*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp68F;});_tmp68E.f2=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp68E;});_tmp68D;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp68C;});_tmp68B;});break;case 229: _LL29D: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690[0]=({
struct Cyc_Stmt_tok_struct _tmp691;_tmp691.tag=Cyc_Stmt_tok;_tmp691.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmp691;});_tmp690;});break;case 230: _LL29E: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692[0]=({struct Cyc_Stmt_tok_struct
_tmp693;_tmp693.tag=Cyc_Stmt_tok;_tmp693.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp693;});_tmp692;});break;case 231: _LL29F: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp694=_cycalloc(sizeof(*_tmp694));_tmp694[0]=({
struct Cyc_Stmt_tok_struct _tmp695;_tmp695.tag=Cyc_Stmt_tok;_tmp695.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp695;});
_tmp694;});break;case 232: _LL2A0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 233: _LL2A1: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696[0]=({struct Cyc_Stmt_tok_struct
_tmp697;_tmp697.tag=Cyc_Stmt_tok;_tmp697.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(0));_tmp697;});_tmp696;});break;case 234:
_LL2A2: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp698=_cycalloc(sizeof(*
_tmp698));_tmp698[0]=({struct Cyc_Stmt_tok_struct _tmp699;_tmp699.tag=Cyc_Stmt_tok;
_tmp699.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]));_tmp699;});_tmp698;});break;case 235: _LL2A3: yyval=yyvs[
yyvsp_offset];break;case 236: _LL2A4: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A[0]=({struct Cyc_Stmt_tok_struct
_tmp69B;_tmp69B.tag=Cyc_Stmt_tok;_tmp69B.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp69B;});_tmp69A;});break;case 237: _LL2A5: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C[0]=({
struct Cyc_Stmt_tok_struct _tmp69D;_tmp69D.tag=Cyc_Stmt_tok;_tmp69D.f1=Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*_tmp69E=_cycalloc(
sizeof(*_tmp69E));_tmp69E[0]=({struct Cyc_Absyn_Fn_d_struct _tmp69F;_tmp69F.tag=1;
_tmp69F.f1=Cyc_yyget_FnDecl_tok(yyvs[yyvsp_offset]);_tmp69F;});_tmp69E;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line)),Cyc_Absyn_skip_stmt(
0));_tmp69D;});_tmp69C;});break;case 238: _LL2A6: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0[0]=({struct Cyc_Stmt_tok_struct
_tmp6A1;_tmp6A1.tag=Cyc_Stmt_tok;_tmp6A1.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));_tmp6A2[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp6A3;_tmp6A3.tag=1;_tmp6A3.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6A3;});_tmp6A2;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]));_tmp6A1;});
_tmp6A0;});break;case 239: _LL2A7: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6A4=
_cycalloc(sizeof(*_tmp6A4));_tmp6A4[0]=({struct Cyc_Stmt_tok_struct _tmp6A5;
_tmp6A5.tag=Cyc_Stmt_tok;_tmp6A5.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6A5;});_tmp6A4;});break;case 240: _LL2A8: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6[0]=({
struct Cyc_Stmt_tok_struct _tmp6A7;_tmp6A7.tag=Cyc_Stmt_tok;_tmp6A7.f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6A7;});_tmp6A6;});break;case 241: _LL2A9: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8[0]=({
struct Cyc_Stmt_tok_struct _tmp6A9;_tmp6A9.tag=Cyc_Stmt_tok;_tmp6A9.f1=Cyc_Absyn_switch_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6A9;});_tmp6A8;});break;case 242: _LL2AA: if(Cyc_Std_strcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tag_arr("C",sizeof(unsigned char),2))!= 0)Cyc_Parse_err(_tag_arr("only switch \"C\" { ... } is allowed",
sizeof(unsigned char),35),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6AA=
_cycalloc(sizeof(*_tmp6AA));_tmp6AA[0]=({struct Cyc_Stmt_tok_struct _tmp6AB;
_tmp6AB.tag=Cyc_Stmt_tok;_tmp6AB.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_SwitchC_s_struct*
_tmp6AC=_cycalloc(sizeof(*_tmp6AC));_tmp6AC[0]=({struct Cyc_Absyn_SwitchC_s_struct
_tmp6AD;_tmp6AD.tag=10;_tmp6AD.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp6AD.f2=Cyc_yyget_SwitchCClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6AD;});_tmp6AC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6AB;});
_tmp6AA;});break;case 243: _LL2AB: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6AE=
_cycalloc(sizeof(*_tmp6AE));_tmp6AE[0]=({struct Cyc_Stmt_tok_struct _tmp6AF;
_tmp6AF.tag=Cyc_Stmt_tok;_tmp6AF.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6AF;});_tmp6AE;});break;case 244: _LL2AC: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));
_tmp6B0[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp6B1;_tmp6B1.tag=Cyc_SwitchClauseList_tok;
_tmp6B1.f1=0;_tmp6B1;});_tmp6B0;});break;case 245: _LL2AD: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6B2=_cycalloc(sizeof(*_tmp6B2));_tmp6B2[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6B3;_tmp6B3.tag=Cyc_SwitchClauseList_tok;_tmp6B3.f1=({struct Cyc_List_List*
_tmp6B4=_cycalloc(sizeof(*_tmp6B4));_tmp6B4->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp6B5->pat_vars=0;_tmp6B5->where_clause=0;
_tmp6B5->body=Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp6B5->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6B5;});_tmp6B4->tl=0;_tmp6B4;});_tmp6B3;});_tmp6B2;});
break;case 246: _LL2AE: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6B7;_tmp6B7.tag=Cyc_SwitchClauseList_tok;_tmp6B7.f1=({struct Cyc_List_List*
_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6B9->pat_vars=0;
_tmp6B9->where_clause=0;_tmp6B9->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp6B9->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line);_tmp6B9;});_tmp6B8->tl=
Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmp6B8;});_tmp6B7;});_tmp6B6;});
break;case 247: _LL2AF: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6BB;_tmp6BB.tag=Cyc_SwitchClauseList_tok;_tmp6BB.f1=({struct Cyc_List_List*
_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6BD->pat_vars=0;
_tmp6BD->where_clause=0;_tmp6BD->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6BD->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp6BD;});
_tmp6BC->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmp6BC;});
_tmp6BB;});_tmp6BA;});break;case 248: _LL2B0: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6BF;_tmp6BF.tag=Cyc_SwitchClauseList_tok;_tmp6BF.f1=({struct Cyc_List_List*
_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp6C1->pat_vars=0;
_tmp6C1->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6C1->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp6C1->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6C1;});_tmp6C0->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmp6C0;});_tmp6BF;});_tmp6BE;});break;case 249: _LL2B1: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));
_tmp6C2[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp6C3;_tmp6C3.tag=Cyc_SwitchClauseList_tok;
_tmp6C3.f1=({struct Cyc_List_List*_tmp6C4=_cycalloc(sizeof(*_tmp6C4));_tmp6C4->hd=({
struct Cyc_Absyn_Switch_clause*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp6C5->pat_vars=0;_tmp6C5->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6C5->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6C5->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6C5;});_tmp6C4->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmp6C4;});_tmp6C3;});_tmp6C2;});break;case 250: _LL2B2: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));
_tmp6C6[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp6C7;_tmp6C7.tag=Cyc_SwitchCClauseList_tok;
_tmp6C7.f1=0;_tmp6C7;});_tmp6C6;});break;case 251: _LL2B3: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmp6C9;_tmp6C9.tag=Cyc_SwitchCClauseList_tok;_tmp6C9.f1=({struct Cyc_List_List*
_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA->hd=({struct Cyc_Absyn_SwitchC_clause*
_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->cnst_exp=0;_tmp6CB->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Absyn_break_stmt(0),0);_tmp6CB->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line);_tmp6CB;});_tmp6CA->tl=
0;_tmp6CA;});_tmp6C9;});_tmp6C8;});break;case 252: _LL2B4: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmp6CC=_cycalloc(sizeof(*_tmp6CC));_tmp6CC[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmp6CD;_tmp6CD.tag=Cyc_SwitchCClauseList_tok;_tmp6CD.f1=({struct Cyc_List_List*
_tmp6CE=_cycalloc(sizeof(*_tmp6CE));_tmp6CE->hd=({struct Cyc_Absyn_SwitchC_clause*
_tmp6CF=_cycalloc(sizeof(*_tmp6CF));_tmp6CF->cnst_exp=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6CF->body=Cyc_Absyn_fallthru_stmt(
0,0);_tmp6CF->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6CF;});_tmp6CE->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmp6CE;});_tmp6CD;});_tmp6CC;});break;case 253: _LL2B5: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));
_tmp6D0[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp6D1;_tmp6D1.tag=Cyc_SwitchCClauseList_tok;
_tmp6D1.f1=({struct Cyc_List_List*_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2->hd=({
struct Cyc_Absyn_SwitchC_clause*_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3->cnst_exp=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp6D3->body=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Absyn_fallthru_stmt(
0,0),0);_tmp6D3->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6D3;});_tmp6D2->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmp6D2;});_tmp6D1;});_tmp6D0;});break;case 254: _LL2B6: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4[0]=({
struct Cyc_Stmt_tok_struct _tmp6D5;_tmp6D5.tag=Cyc_Stmt_tok;_tmp6D5.f1=Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6D5;});_tmp6D4;});break;case 255: _LL2B7: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6[0]=({
struct Cyc_Stmt_tok_struct _tmp6D7;_tmp6D7.tag=Cyc_Stmt_tok;_tmp6D7.f1=Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6D7;});
_tmp6D6;});break;case 256: _LL2B8: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6D8=
_cycalloc(sizeof(*_tmp6D8));_tmp6D8[0]=({struct Cyc_Stmt_tok_struct _tmp6D9;
_tmp6D9.tag=Cyc_Stmt_tok;_tmp6D9.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6D9;});
_tmp6D8;});break;case 257: _LL2B9: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6DA=
_cycalloc(sizeof(*_tmp6DA));_tmp6DA[0]=({struct Cyc_Stmt_tok_struct _tmp6DB;
_tmp6DB.tag=Cyc_Stmt_tok;_tmp6DB.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6DB;});_tmp6DA;});break;case 258: _LL2BA: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6DC=_cycalloc(sizeof(*_tmp6DC));_tmp6DC[0]=({
struct Cyc_Stmt_tok_struct _tmp6DD;_tmp6DD.tag=Cyc_Stmt_tok;_tmp6DD.f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6DD;});
_tmp6DC;});break;case 259: _LL2BB: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6DE=
_cycalloc(sizeof(*_tmp6DE));_tmp6DE[0]=({struct Cyc_Stmt_tok_struct _tmp6DF;
_tmp6DF.tag=Cyc_Stmt_tok;_tmp6DF.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp6DF;});
_tmp6DE;});break;case 260: _LL2BC: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6E0=
_cycalloc(sizeof(*_tmp6E0));_tmp6E0[0]=({struct Cyc_Stmt_tok_struct _tmp6E1;
_tmp6E1.tag=Cyc_Stmt_tok;_tmp6E1.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6E1;});_tmp6E0;});break;case 261: _LL2BD: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2[0]=({
struct Cyc_Stmt_tok_struct _tmp6E3;_tmp6E3.tag=Cyc_Stmt_tok;_tmp6E3.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6E3;});_tmp6E2;});break;case 262: _LL2BE: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6E4=_cycalloc(sizeof(*_tmp6E4));_tmp6E4[0]=({
struct Cyc_Stmt_tok_struct _tmp6E5;_tmp6E5.tag=Cyc_Stmt_tok;_tmp6E5.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6E5;});_tmp6E4;});break;case 263: _LL2BF: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6[0]=({
struct Cyc_Stmt_tok_struct _tmp6E7;_tmp6E7.tag=Cyc_Stmt_tok;_tmp6E7.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6E7;});_tmp6E6;});break;case 264: _LL2C0: {struct
Cyc_List_List*_tmp6E8=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*_tmp6E9=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp6EA=_cycalloc(sizeof(*_tmp6EA));_tmp6EA[0]=({
struct Cyc_Stmt_tok_struct _tmp6EB;_tmp6EB.tag=Cyc_Stmt_tok;_tmp6EB.f1=Cyc_Parse_flatten_declarations(
_tmp6E8,_tmp6E9);_tmp6EB;});_tmp6EA;});break;}case 265: _LL2C1: {struct Cyc_List_List*
_tmp6EC=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp6ED=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6EE=
_cycalloc(sizeof(*_tmp6EE));_tmp6EE[0]=({struct Cyc_Stmt_tok_struct _tmp6EF;
_tmp6EF.tag=Cyc_Stmt_tok;_tmp6EF.f1=Cyc_Parse_flatten_declarations(_tmp6EC,
_tmp6ED);_tmp6EF;});_tmp6EE;});break;}case 266: _LL2C2: {struct Cyc_List_List*
_tmp6F0=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp6F1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6F2=
_cycalloc(sizeof(*_tmp6F2));_tmp6F2[0]=({struct Cyc_Stmt_tok_struct _tmp6F3;
_tmp6F3.tag=Cyc_Stmt_tok;_tmp6F3.f1=Cyc_Parse_flatten_declarations(_tmp6F0,
_tmp6F1);_tmp6F3;});_tmp6F2;});break;}case 267: _LL2C3: {struct Cyc_List_List*
_tmp6F4=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp6F5=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6F6=
_cycalloc(sizeof(*_tmp6F6));_tmp6F6[0]=({struct Cyc_Stmt_tok_struct _tmp6F7;
_tmp6F7.tag=Cyc_Stmt_tok;_tmp6F7.f1=Cyc_Parse_flatten_declarations(_tmp6F4,
_tmp6F5);_tmp6F7;});_tmp6F6;});break;}case 268: _LL2C4: {struct Cyc_List_List*
_tmp6F8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_decl2vardecl,Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp6F9=_cycalloc(sizeof(*_tmp6F9));_tmp6F9[0]=({
struct Cyc_Stmt_tok_struct _tmp6FA;_tmp6FA.tag=Cyc_Stmt_tok;_tmp6FA.f1=Cyc_Absyn_forarray_stmt(
_tmp6F8,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6FA;});_tmp6F9;});break;}case 269: _LL2C5: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6FB=_cycalloc(sizeof(*_tmp6FB));_tmp6FB[0]=({
struct Cyc_Stmt_tok_struct _tmp6FC;_tmp6FC.tag=Cyc_Stmt_tok;_tmp6FC.f1=Cyc_Absyn_goto_stmt(({
struct _tagged_arr*_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6FD;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp6FC;});
_tmp6FB;});break;case 270: _LL2C6: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6FE=
_cycalloc(sizeof(*_tmp6FE));_tmp6FE[0]=({struct Cyc_Stmt_tok_struct _tmp6FF;
_tmp6FF.tag=Cyc_Stmt_tok;_tmp6FF.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp6FF;});
_tmp6FE;});break;case 271: _LL2C7: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp700=
_cycalloc(sizeof(*_tmp700));_tmp700[0]=({struct Cyc_Stmt_tok_struct _tmp701;
_tmp701.tag=Cyc_Stmt_tok;_tmp701.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp701;});
_tmp700;});break;case 272: _LL2C8: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp702=
_cycalloc(sizeof(*_tmp702));_tmp702[0]=({struct Cyc_Stmt_tok_struct _tmp703;
_tmp703.tag=Cyc_Stmt_tok;_tmp703.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp703;});
_tmp702;});break;case 273: _LL2C9: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp704=
_cycalloc(sizeof(*_tmp704));_tmp704[0]=({struct Cyc_Stmt_tok_struct _tmp705;
_tmp705.tag=Cyc_Stmt_tok;_tmp705.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp705;});_tmp704;});break;case 274: _LL2CA: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp706=_cycalloc(sizeof(*_tmp706));_tmp706[0]=({
struct Cyc_Stmt_tok_struct _tmp707;_tmp707.tag=Cyc_Stmt_tok;_tmp707.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp707;});_tmp706;});break;case 275: _LL2CB: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp708=_cycalloc(sizeof(*_tmp708));_tmp708[0]=({
struct Cyc_Stmt_tok_struct _tmp709;_tmp709.tag=Cyc_Stmt_tok;_tmp709.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp709;});_tmp708;});break;case 276: _LL2CC: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A[0]=({
struct Cyc_Stmt_tok_struct _tmp70B;_tmp70B.tag=Cyc_Stmt_tok;_tmp70B.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp70B;});_tmp70A;});break;case 277: _LL2CD: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp70C=_cycalloc(sizeof(*_tmp70C));_tmp70C[
0]=({struct Cyc_Pattern_tok_struct _tmp70D;_tmp70D.tag=Cyc_Pattern_tok;_tmp70D.f1=
Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp70D;});_tmp70C;});break;case 278: _LL2CE: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 279: _LL2CF:
yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp70E=_cycalloc(sizeof(*_tmp70E));
_tmp70E[0]=({struct Cyc_Pattern_tok_struct _tmp70F;_tmp70F.tag=Cyc_Pattern_tok;
_tmp70F.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp710=
_cycalloc(sizeof(*_tmp710));_tmp710[0]=({struct Cyc_Absyn_Int_p_struct _tmp711;
_tmp711.tag=1;_tmp711.f1=(void*)(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f1;
_tmp711.f2=(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2;_tmp711;});_tmp710;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp70F;});
_tmp70E;});break;case 280: _LL2D0: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp712=_cycalloc(sizeof(*_tmp712));_tmp712[0]=({struct Cyc_Pattern_tok_struct
_tmp713;_tmp713.tag=Cyc_Pattern_tok;_tmp713.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Int_p_struct*_tmp714=_cycalloc(sizeof(*_tmp714));_tmp714[0]=({struct
Cyc_Absyn_Int_p_struct _tmp715;_tmp715.tag=1;_tmp715.f1=(void*)((void*)0);_tmp715.f2=
-(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2;_tmp715;});_tmp714;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp713;});_tmp712;});break;case 281: _LL2D1: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716[
0]=({struct Cyc_Pattern_tok_struct _tmp717;_tmp717.tag=Cyc_Pattern_tok;_tmp717.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*_tmp718=_cycalloc(
sizeof(*_tmp718));_tmp718[0]=({struct Cyc_Absyn_Float_p_struct _tmp719;_tmp719.tag=
3;_tmp719.f1=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp719;});_tmp718;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp717;});
_tmp716;});break;case 282: _LL2D2: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp71A=_cycalloc(sizeof(*_tmp71A));_tmp71A[0]=({struct Cyc_Pattern_tok_struct
_tmp71B;_tmp71B.tag=Cyc_Pattern_tok;_tmp71B.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Char_p_struct*_tmp71C=_cycalloc_atomic(sizeof(*_tmp71C));_tmp71C[0]=({
struct Cyc_Absyn_Char_p_struct _tmp71D;_tmp71D.tag=2;_tmp71D.f1=Cyc_yyget_Char_tok(
yyvs[yyvsp_offset]);_tmp71D;});_tmp71C;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp71B;});_tmp71A;});
break;case 283: _LL2D3: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp71E=
_cycalloc(sizeof(*_tmp71E));_tmp71E[0]=({struct Cyc_Pattern_tok_struct _tmp71F;
_tmp71F.tag=Cyc_Pattern_tok;_tmp71F.f1=Cyc_Absyn_new_pat((void*)1,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp71F;});
_tmp71E;});break;case 284: _LL2D4: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp720=_cycalloc(sizeof(*_tmp720));_tmp720[0]=({struct Cyc_Pattern_tok_struct
_tmp721;_tmp721.tag=Cyc_Pattern_tok;_tmp721.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownId_p_struct*_tmp722=_cycalloc(sizeof(*_tmp722));_tmp722[0]=({
struct Cyc_Absyn_UnknownId_p_struct _tmp723;_tmp723.tag=11;_tmp723.f1=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp723;});_tmp722;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp721;});_tmp720;});
break;case 285: _LL2D5: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp724=
_cycalloc(sizeof(*_tmp724));_tmp724[0]=({struct Cyc_Pattern_tok_struct _tmp725;
_tmp725.tag=Cyc_Pattern_tok;_tmp725.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_struct*
_tmp726=_cycalloc(sizeof(*_tmp726));_tmp726[0]=({struct Cyc_Absyn_Tuple_p_struct
_tmp727;_tmp727.tag=4;_tmp727.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]));_tmp727;});_tmp726;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp725;});_tmp724;});break;case 286: _LL2D6: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp728=_cycalloc(sizeof(*_tmp728));_tmp728[
0]=({struct Cyc_Pattern_tok_struct _tmp729;_tmp729.tag=Cyc_Pattern_tok;_tmp729.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp72A=
_cycalloc(sizeof(*_tmp72A));_tmp72A[0]=({struct Cyc_Absyn_UnknownCall_p_struct
_tmp72B;_tmp72B.tag=12;_tmp72B.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp72B.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp72B;});_tmp72A;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp729;});
_tmp728;});break;case 287: _LL2D7: {struct Cyc_List_List*_tmp72C=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp72D=_cycalloc(sizeof(*_tmp72D));_tmp72D[0]=({
struct Cyc_Pattern_tok_struct _tmp72E;_tmp72E.tag=Cyc_Pattern_tok;_tmp72E.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp72F=_cycalloc(sizeof(*_tmp72F));
_tmp72F[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp730;_tmp730.tag=7;_tmp730.f1=({
struct Cyc_Absyn_AggrInfo _tmp731;_tmp731.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp733;_tmp733.tag=0;_tmp733.f1=(void*)((void*)0);_tmp733.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp733;});_tmp732;}));
_tmp731.targs=0;_tmp731;});_tmp730.f2=_tmp72C;_tmp730.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp730;});_tmp72F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp72E;});
_tmp72D;});break;}case 288: _LL2D8: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp734=_cycalloc(sizeof(*_tmp734));_tmp734[0]=({struct Cyc_Pattern_tok_struct
_tmp735;_tmp735.tag=Cyc_Pattern_tok;_tmp735.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Pointer_p_struct*_tmp736=_cycalloc(sizeof(*_tmp736));_tmp736[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp737;_tmp737.tag=5;_tmp737.f1=Cyc_yyget_Pattern_tok(
yyvs[yyvsp_offset]);_tmp737;});_tmp736;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp735;});_tmp734;});break;case 289: _LL2D9: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp738=_cycalloc(sizeof(*_tmp738));_tmp738[
0]=({struct Cyc_Pattern_tok_struct _tmp739;_tmp739.tag=Cyc_Pattern_tok;_tmp739.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp73A=_cycalloc(
sizeof(*_tmp73A));_tmp73A[0]=({struct Cyc_Absyn_Reference_p_struct _tmp73B;_tmp73B.tag=
6;_tmp73B.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp73C=_cycalloc(sizeof(*
_tmp73C));_tmp73C->f1=(void*)0;_tmp73C->f2=({struct _tagged_arr*_tmp73D=_cycalloc(
sizeof(*_tmp73D));_tmp73D[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp73D;});
_tmp73C;}),(void*)0,0);_tmp73B;});_tmp73A;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp739;});_tmp738;});break;case 290: _LL2DA: yyval=(
void*)({struct Cyc_PatternList_tok_struct*_tmp73E=_cycalloc(sizeof(*_tmp73E));
_tmp73E[0]=({struct Cyc_PatternList_tok_struct _tmp73F;_tmp73F.tag=Cyc_PatternList_tok;
_tmp73F.f1=({struct Cyc_List_List*_tmp740=_cycalloc(sizeof(*_tmp740));_tmp740->hd=
Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmp740->tl=0;_tmp740;});_tmp73F;});
_tmp73E;});break;case 291: _LL2DB: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741[0]=({struct Cyc_PatternList_tok_struct
_tmp742;_tmp742.tag=Cyc_PatternList_tok;_tmp742.f1=({struct Cyc_List_List*_tmp743=
_cycalloc(sizeof(*_tmp743));_tmp743->hd=Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);
_tmp743->tl=Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp743;});_tmp742;});_tmp741;});break;case 292: _LL2DC: yyval=(
void*)({struct Cyc_FieldPattern_tok_struct*_tmp744=_cycalloc(sizeof(*_tmp744));
_tmp744[0]=({struct Cyc_FieldPattern_tok_struct _tmp745;_tmp745.tag=Cyc_FieldPattern_tok;
_tmp745.f1=({struct _tuple10*_tmp746=_cycalloc(sizeof(*_tmp746));_tmp746->f1=0;
_tmp746->f2=Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmp746;});_tmp745;});
_tmp744;});break;case 293: _LL2DD: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp747=_cycalloc(sizeof(*_tmp747));_tmp747[0]=({struct Cyc_FieldPattern_tok_struct
_tmp748;_tmp748.tag=Cyc_FieldPattern_tok;_tmp748.f1=({struct _tuple10*_tmp749=
_cycalloc(sizeof(*_tmp749));_tmp749->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp749->f2=Cyc_yyget_Pattern_tok(
yyvs[yyvsp_offset]);_tmp749;});_tmp748;});_tmp747;});break;case 294: _LL2DE: yyval=(
void*)({struct Cyc_FieldPatternList_tok_struct*_tmp74A=_cycalloc(sizeof(*_tmp74A));
_tmp74A[0]=({struct Cyc_FieldPatternList_tok_struct _tmp74B;_tmp74B.tag=Cyc_FieldPatternList_tok;
_tmp74B.f1=({struct Cyc_List_List*_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C->hd=
Cyc_yyget_FieldPattern_tok(yyvs[yyvsp_offset]);_tmp74C->tl=0;_tmp74C;});_tmp74B;});
_tmp74A;});break;case 295: _LL2DF: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp74E;_tmp74E.tag=Cyc_FieldPatternList_tok;_tmp74E.f1=({struct Cyc_List_List*
_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F->hd=Cyc_yyget_FieldPattern_tok(yyvs[
yyvsp_offset]);_tmp74F->tl=Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp74F;});_tmp74E;});
_tmp74D;});break;case 296: _LL2E0: yyval=yyvs[yyvsp_offset];break;case 297: _LL2E1:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp750=_cycalloc(sizeof(*_tmp750));
_tmp750[0]=({struct Cyc_Exp_tok_struct _tmp751;_tmp751.tag=Cyc_Exp_tok;_tmp751.f1=
Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp751;});_tmp750;});break;case 298: _LL2E2: yyval=yyvs[
yyvsp_offset];break;case 299: _LL2E3: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp752=_cycalloc(sizeof(*_tmp752));_tmp752[0]=({struct Cyc_Exp_tok_struct _tmp753;
_tmp753.tag=Cyc_Exp_tok;_tmp753.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp753;});_tmp752;});break;case 300: _LL2E4: yyval=(
void*)({struct Cyc_Primopopt_tok_struct*_tmp754=_cycalloc(sizeof(*_tmp754));
_tmp754[0]=({struct Cyc_Primopopt_tok_struct _tmp755;_tmp755.tag=Cyc_Primopopt_tok;
_tmp755.f1=0;_tmp755;});_tmp754;});break;case 301: _LL2E5: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp756=_cycalloc(sizeof(*_tmp756));_tmp756[0]=({struct Cyc_Primopopt_tok_struct
_tmp757;_tmp757.tag=Cyc_Primopopt_tok;_tmp757.f1=({struct Cyc_Core_Opt*_tmp758=
_cycalloc(sizeof(*_tmp758));_tmp758->v=(void*)((void*)1);_tmp758;});_tmp757;});
_tmp756;});break;case 302: _LL2E6: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp759=_cycalloc(sizeof(*_tmp759));_tmp759[0]=({struct Cyc_Primopopt_tok_struct
_tmp75A;_tmp75A.tag=Cyc_Primopopt_tok;_tmp75A.f1=({struct Cyc_Core_Opt*_tmp75B=
_cycalloc(sizeof(*_tmp75B));_tmp75B->v=(void*)((void*)3);_tmp75B;});_tmp75A;});
_tmp759;});break;case 303: _LL2E7: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp75C=_cycalloc(sizeof(*_tmp75C));_tmp75C[0]=({struct Cyc_Primopopt_tok_struct
_tmp75D;_tmp75D.tag=Cyc_Primopopt_tok;_tmp75D.f1=({struct Cyc_Core_Opt*_tmp75E=
_cycalloc(sizeof(*_tmp75E));_tmp75E->v=(void*)((void*)4);_tmp75E;});_tmp75D;});
_tmp75C;});break;case 304: _LL2E8: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp75F=_cycalloc(sizeof(*_tmp75F));_tmp75F[0]=({struct Cyc_Primopopt_tok_struct
_tmp760;_tmp760.tag=Cyc_Primopopt_tok;_tmp760.f1=({struct Cyc_Core_Opt*_tmp761=
_cycalloc(sizeof(*_tmp761));_tmp761->v=(void*)((void*)0);_tmp761;});_tmp760;});
_tmp75F;});break;case 305: _LL2E9: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp762=_cycalloc(sizeof(*_tmp762));_tmp762[0]=({struct Cyc_Primopopt_tok_struct
_tmp763;_tmp763.tag=Cyc_Primopopt_tok;_tmp763.f1=({struct Cyc_Core_Opt*_tmp764=
_cycalloc(sizeof(*_tmp764));_tmp764->v=(void*)((void*)2);_tmp764;});_tmp763;});
_tmp762;});break;case 306: _LL2EA: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp765=_cycalloc(sizeof(*_tmp765));_tmp765[0]=({struct Cyc_Primopopt_tok_struct
_tmp766;_tmp766.tag=Cyc_Primopopt_tok;_tmp766.f1=({struct Cyc_Core_Opt*_tmp767=
_cycalloc(sizeof(*_tmp767));_tmp767->v=(void*)((void*)16);_tmp767;});_tmp766;});
_tmp765;});break;case 307: _LL2EB: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768[0]=({struct Cyc_Primopopt_tok_struct
_tmp769;_tmp769.tag=Cyc_Primopopt_tok;_tmp769.f1=({struct Cyc_Core_Opt*_tmp76A=
_cycalloc(sizeof(*_tmp76A));_tmp76A->v=(void*)((void*)17);_tmp76A;});_tmp769;});
_tmp768;});break;case 308: _LL2EC: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp76B=_cycalloc(sizeof(*_tmp76B));_tmp76B[0]=({struct Cyc_Primopopt_tok_struct
_tmp76C;_tmp76C.tag=Cyc_Primopopt_tok;_tmp76C.f1=({struct Cyc_Core_Opt*_tmp76D=
_cycalloc(sizeof(*_tmp76D));_tmp76D->v=(void*)((void*)13);_tmp76D;});_tmp76C;});
_tmp76B;});break;case 309: _LL2ED: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp76E=_cycalloc(sizeof(*_tmp76E));_tmp76E[0]=({struct Cyc_Primopopt_tok_struct
_tmp76F;_tmp76F.tag=Cyc_Primopopt_tok;_tmp76F.f1=({struct Cyc_Core_Opt*_tmp770=
_cycalloc(sizeof(*_tmp770));_tmp770->v=(void*)((void*)15);_tmp770;});_tmp76F;});
_tmp76E;});break;case 310: _LL2EE: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp771=_cycalloc(sizeof(*_tmp771));_tmp771[0]=({struct Cyc_Primopopt_tok_struct
_tmp772;_tmp772.tag=Cyc_Primopopt_tok;_tmp772.f1=({struct Cyc_Core_Opt*_tmp773=
_cycalloc(sizeof(*_tmp773));_tmp773->v=(void*)((void*)14);_tmp773;});_tmp772;});
_tmp771;});break;case 311: _LL2EF: yyval=yyvs[yyvsp_offset];break;case 312: _LL2F0:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp774=_cycalloc(sizeof(*_tmp774));
_tmp774[0]=({struct Cyc_Exp_tok_struct _tmp775;_tmp775.tag=Cyc_Exp_tok;_tmp775.f1=
Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp775;});_tmp774;});break;case 313: _LL2F1: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp776=_cycalloc(sizeof(*_tmp776));_tmp776[0]=({
struct Cyc_Exp_tok_struct _tmp777;_tmp777.tag=Cyc_Exp_tok;_tmp777.f1=Cyc_Absyn_throw_exp(
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp777;});_tmp776;});break;case 314: _LL2F2: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp778=_cycalloc(sizeof(*_tmp778));_tmp778[0]=({
struct Cyc_Exp_tok_struct _tmp779;_tmp779.tag=Cyc_Exp_tok;_tmp779.f1=Cyc_Absyn_New_exp(
0,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line));_tmp779;});
_tmp778;});break;case 315: _LL2F3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp77A=
_cycalloc(sizeof(*_tmp77A));_tmp77A[0]=({struct Cyc_Exp_tok_struct _tmp77B;_tmp77B.tag=
Cyc_Exp_tok;_tmp77B.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).last_line));_tmp77B;});_tmp77A;});break;case 316: _LL2F4: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp77C=_cycalloc(sizeof(*_tmp77C));_tmp77C[0]=({
struct Cyc_Exp_tok_struct _tmp77D;_tmp77D.tag=Cyc_Exp_tok;_tmp77D.f1=Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp77D;});_tmp77C;});break;case 317: _LL2F5: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp77E=_cycalloc(sizeof(*_tmp77E));_tmp77E[0]=({
struct Cyc_Exp_tok_struct _tmp77F;_tmp77F.tag=Cyc_Exp_tok;_tmp77F.f1=Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp77F;});_tmp77E;});break;case 318: _LL2F6: yyval=yyvs[
yyvsp_offset];break;case 319: _LL2F7: yyval=yyvs[yyvsp_offset];break;case 320: _LL2F8:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp780=_cycalloc(sizeof(*_tmp780));
_tmp780[0]=({struct Cyc_Exp_tok_struct _tmp781;_tmp781.tag=Cyc_Exp_tok;_tmp781.f1=
Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp781;});_tmp780;});break;case 321: _LL2F9: yyval=yyvs[
yyvsp_offset];break;case 322: _LL2FA: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp782=_cycalloc(sizeof(*_tmp782));_tmp782[0]=({struct Cyc_Exp_tok_struct _tmp783;
_tmp783.tag=Cyc_Exp_tok;_tmp783.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp783;});
_tmp782;});break;case 323: _LL2FB: yyval=yyvs[yyvsp_offset];break;case 324: _LL2FC:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp784=_cycalloc(sizeof(*_tmp784));
_tmp784[0]=({struct Cyc_Exp_tok_struct _tmp785;_tmp785.tag=Cyc_Exp_tok;_tmp785.f1=
Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp785;});
_tmp784;});break;case 325: _LL2FD: yyval=yyvs[yyvsp_offset];break;case 326: _LL2FE:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp786=_cycalloc(sizeof(*_tmp786));
_tmp786[0]=({struct Cyc_Exp_tok_struct _tmp787;_tmp787.tag=Cyc_Exp_tok;_tmp787.f1=
Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp787;});
_tmp786;});break;case 327: _LL2FF: yyval=yyvs[yyvsp_offset];break;case 328: _LL300:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp788=_cycalloc(sizeof(*_tmp788));
_tmp788[0]=({struct Cyc_Exp_tok_struct _tmp789;_tmp789.tag=Cyc_Exp_tok;_tmp789.f1=
Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp789;});
_tmp788;});break;case 329: _LL301: yyval=yyvs[yyvsp_offset];break;case 330: _LL302:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp78A=_cycalloc(sizeof(*_tmp78A));
_tmp78A[0]=({struct Cyc_Exp_tok_struct _tmp78B;_tmp78B.tag=Cyc_Exp_tok;_tmp78B.f1=
Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp78B;});_tmp78A;});break;case 331: _LL303: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp78C=_cycalloc(sizeof(*_tmp78C));_tmp78C[0]=({
struct Cyc_Exp_tok_struct _tmp78D;_tmp78D.tag=Cyc_Exp_tok;_tmp78D.f1=Cyc_Absyn_neq_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp78D;});_tmp78C;});break;case 332: _LL304: yyval=yyvs[
yyvsp_offset];break;case 333: _LL305: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp78E=_cycalloc(sizeof(*_tmp78E));_tmp78E[0]=({struct Cyc_Exp_tok_struct _tmp78F;
_tmp78F.tag=Cyc_Exp_tok;_tmp78F.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp78F;});
_tmp78E;});break;case 334: _LL306: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp790=
_cycalloc(sizeof(*_tmp790));_tmp790[0]=({struct Cyc_Exp_tok_struct _tmp791;_tmp791.tag=
Cyc_Exp_tok;_tmp791.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp791;});
_tmp790;});break;case 335: _LL307: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp792=
_cycalloc(sizeof(*_tmp792));_tmp792[0]=({struct Cyc_Exp_tok_struct _tmp793;_tmp793.tag=
Cyc_Exp_tok;_tmp793.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp793;});
_tmp792;});break;case 336: _LL308: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp794=
_cycalloc(sizeof(*_tmp794));_tmp794[0]=({struct Cyc_Exp_tok_struct _tmp795;_tmp795.tag=
Cyc_Exp_tok;_tmp795.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp795;});
_tmp794;});break;case 337: _LL309: yyval=yyvs[yyvsp_offset];break;case 338: _LL30A:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp796=_cycalloc(sizeof(*_tmp796));
_tmp796[0]=({struct Cyc_Exp_tok_struct _tmp797;_tmp797.tag=Cyc_Exp_tok;_tmp797.f1=
Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp797;});
_tmp796;});break;case 339: _LL30B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp798=
_cycalloc(sizeof(*_tmp798));_tmp798[0]=({struct Cyc_Exp_tok_struct _tmp799;_tmp799.tag=
Cyc_Exp_tok;_tmp799.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp799;});
_tmp798;});break;case 340: _LL30C: yyval=yyvs[yyvsp_offset];break;case 341: _LL30D:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp79A=_cycalloc(sizeof(*_tmp79A));
_tmp79A[0]=({struct Cyc_Exp_tok_struct _tmp79B;_tmp79B.tag=Cyc_Exp_tok;_tmp79B.f1=
Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp79B;});
_tmp79A;});break;case 342: _LL30E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp79C=
_cycalloc(sizeof(*_tmp79C));_tmp79C[0]=({struct Cyc_Exp_tok_struct _tmp79D;_tmp79D.tag=
Cyc_Exp_tok;_tmp79D.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp79D;});
_tmp79C;});break;case 343: _LL30F: yyval=yyvs[yyvsp_offset];break;case 344: _LL310:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp79E=_cycalloc(sizeof(*_tmp79E));
_tmp79E[0]=({struct Cyc_Exp_tok_struct _tmp79F;_tmp79F.tag=Cyc_Exp_tok;_tmp79F.f1=
Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp79F;});
_tmp79E;});break;case 345: _LL311: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A0=
_cycalloc(sizeof(*_tmp7A0));_tmp7A0[0]=({struct Cyc_Exp_tok_struct _tmp7A1;_tmp7A1.tag=
Cyc_Exp_tok;_tmp7A1.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A1;});
_tmp7A0;});break;case 346: _LL312: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A2=
_cycalloc(sizeof(*_tmp7A2));_tmp7A2[0]=({struct Cyc_Exp_tok_struct _tmp7A3;_tmp7A3.tag=
Cyc_Exp_tok;_tmp7A3.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A3;});
_tmp7A2;});break;case 347: _LL313: yyval=yyvs[yyvsp_offset];break;case 348: _LL314:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A4=_cycalloc(sizeof(*_tmp7A4));
_tmp7A4[0]=({struct Cyc_Exp_tok_struct _tmp7A5;_tmp7A5.tag=Cyc_Exp_tok;_tmp7A5.f1=
Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7A5;});_tmp7A4;});break;case 349: _LL315: yyval=yyvs[
yyvsp_offset];break;case 350: _LL316: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7A6=_cycalloc(sizeof(*_tmp7A6));_tmp7A6[0]=({struct Cyc_Exp_tok_struct _tmp7A7;
_tmp7A7.tag=Cyc_Exp_tok;_tmp7A7.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A7;});
_tmp7A6;});break;case 351: _LL317: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A8=
_cycalloc(sizeof(*_tmp7A8));_tmp7A8[0]=({struct Cyc_Exp_tok_struct _tmp7A9;_tmp7A9.tag=
Cyc_Exp_tok;_tmp7A9.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A9;});
_tmp7A8;});break;case 352: _LL318: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7AA=
_cycalloc(sizeof(*_tmp7AA));_tmp7AA[0]=({struct Cyc_Exp_tok_struct _tmp7AB;_tmp7AB.tag=
Cyc_Exp_tok;_tmp7AB.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7AB;});
_tmp7AA;});break;case 353: _LL319: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7AC=
_cycalloc(sizeof(*_tmp7AC));_tmp7AC[0]=({struct Cyc_Exp_tok_struct _tmp7AD;_tmp7AD.tag=
Cyc_Exp_tok;_tmp7AD.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7AD;});
_tmp7AC;});break;case 354: _LL31A: yyval=yyvs[yyvsp_offset];break;case 355: _LL31B:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7AE=_cycalloc(sizeof(*_tmp7AE));
_tmp7AE[0]=({struct Cyc_Exp_tok_struct _tmp7AF;_tmp7AF.tag=Cyc_Exp_tok;_tmp7AF.f1=
Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7AF;});_tmp7AE;});break;case 356: _LL31C: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7B0=_cycalloc(sizeof(*_tmp7B0));_tmp7B0[0]=({
struct Cyc_Exp_tok_struct _tmp7B1;_tmp7B1.tag=Cyc_Exp_tok;_tmp7B1.f1=Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)])).f3,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B1;});
_tmp7B0;});break;case 357: _LL31D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B2=
_cycalloc(sizeof(*_tmp7B2));_tmp7B2[0]=({struct Cyc_Exp_tok_struct _tmp7B3;_tmp7B3.tag=
Cyc_Exp_tok;_tmp7B3.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B3;});
_tmp7B2;});break;case 358: _LL31E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B4=
_cycalloc(sizeof(*_tmp7B4));_tmp7B4[0]=({struct Cyc_Exp_tok_struct _tmp7B5;_tmp7B5.tag=
Cyc_Exp_tok;_tmp7B5.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp7B6=_cycalloc(sizeof(*_tmp7B6));_tmp7B6[0]=({struct Cyc_Absyn_StructField_struct
_tmp7B7;_tmp7B7.tag=0;_tmp7B7.f1=({struct _tagged_arr*_tmp7B8=_cycalloc(sizeof(*
_tmp7B8));_tmp7B8[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp7B8;});_tmp7B7;});_tmp7B6;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7B5;});_tmp7B4;});break;case 359: _LL31F: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7B9=_cycalloc(sizeof(*_tmp7B9));_tmp7B9[0]=({
struct Cyc_Exp_tok_struct _tmp7BA;_tmp7BA.tag=Cyc_Exp_tok;_tmp7BA.f1=Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp7BB=_cycalloc_atomic(
sizeof(*_tmp7BB));_tmp7BB[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp7BC;_tmp7BC.tag=
1;_tmp7BC.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp7BC;});_tmp7BB;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7BA;});
_tmp7B9;});break;case 360: _LL320: {struct Cyc_Position_Segment*_tmp7BD=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);struct Cyc_List_List*_tmp7BE=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp7BD,Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_Exp_tok_struct*_tmp7BF=_cycalloc(sizeof(*_tmp7BF));_tmp7BF[0]=({struct
Cyc_Exp_tok_struct _tmp7C0;_tmp7C0.tag=Cyc_Exp_tok;_tmp7C0.f1=Cyc_Absyn_gentyp_exp(
_tmp7BE,(*Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C0;});_tmp7BF;});break;}case 361: _LL321: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7C1=_cycalloc(sizeof(*_tmp7C1));_tmp7C1[0]=({
struct Cyc_Exp_tok_struct _tmp7C2;_tmp7C2.tag=Cyc_Exp_tok;_tmp7C2.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7C3=_cycalloc(sizeof(*_tmp7C3));
_tmp7C3[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7C4;_tmp7C4.tag=33;_tmp7C4.f1=({
struct Cyc_Absyn_MallocInfo _tmp7C5;_tmp7C5.is_calloc=0;_tmp7C5.rgn=0;_tmp7C5.elt_type=
0;_tmp7C5.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp7C5.fat_result=0;_tmp7C5;});_tmp7C4;});_tmp7C3;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C2;});_tmp7C1;});break;case 362: _LL322: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7C6=_cycalloc(sizeof(*_tmp7C6));_tmp7C6[0]=({
struct Cyc_Exp_tok_struct _tmp7C7;_tmp7C7.tag=Cyc_Exp_tok;_tmp7C7.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7C8=_cycalloc(sizeof(*_tmp7C8));
_tmp7C8[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7C9;_tmp7C9.tag=33;_tmp7C9.f1=({
struct Cyc_Absyn_MallocInfo _tmp7CA;_tmp7CA.is_calloc=0;_tmp7CA.rgn=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp7CA.elt_type=0;_tmp7CA.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7CA.fat_result=0;
_tmp7CA;});_tmp7C9;});_tmp7C8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C7;});_tmp7C6;});break;case 363: _LL323: {void*
_tmp7CC;struct _tuple2 _tmp7CB=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7CC=_tmp7CB.f3;yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7CD=_cycalloc(sizeof(*_tmp7CD));_tmp7CD[0]=({
struct Cyc_Exp_tok_struct _tmp7CE;_tmp7CE.tag=Cyc_Exp_tok;_tmp7CE.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7CF=_cycalloc(sizeof(*_tmp7CF));
_tmp7CF[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7D0;_tmp7D0.tag=33;_tmp7D0.f1=({
struct Cyc_Absyn_MallocInfo _tmp7D1;_tmp7D1.is_calloc=1;_tmp7D1.rgn=0;_tmp7D1.elt_type=({
void**_tmp7D2=_cycalloc(sizeof(*_tmp7D2));_tmp7D2[0]=_tmp7CC;_tmp7D2;});_tmp7D1.num_elts=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp7D1.fat_result=0;_tmp7D1;});_tmp7D0;});_tmp7CF;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7CE;});_tmp7CD;});break;}case 364: _LL324: {void*
_tmp7D4;struct _tuple2 _tmp7D3=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7D4=_tmp7D3.f3;yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));_tmp7D5[0]=({
struct Cyc_Exp_tok_struct _tmp7D6;_tmp7D6.tag=Cyc_Exp_tok;_tmp7D6.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7D7=_cycalloc(sizeof(*_tmp7D7));
_tmp7D7[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7D8;_tmp7D8.tag=33;_tmp7D8.f1=({
struct Cyc_Absyn_MallocInfo _tmp7D9;_tmp7D9.is_calloc=1;_tmp7D9.rgn=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);
_tmp7D9.elt_type=({void**_tmp7DA=_cycalloc(sizeof(*_tmp7DA));_tmp7DA[0]=_tmp7D4;
_tmp7DA;});_tmp7D9.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp7D9.fat_result=0;
_tmp7D9;});_tmp7D8;});_tmp7D7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7D6;});_tmp7D5;});break;}case 365: _LL325: yyval=(
void*)({struct Cyc_Primop_tok_struct*_tmp7DB=_cycalloc(sizeof(*_tmp7DB));_tmp7DB[
0]=({struct Cyc_Primop_tok_struct _tmp7DC;_tmp7DC.tag=Cyc_Primop_tok;_tmp7DC.f1=(
void*)((void*)12);_tmp7DC;});_tmp7DB;});break;case 366: _LL326: yyval=(void*)({
struct Cyc_Primop_tok_struct*_tmp7DD=_cycalloc(sizeof(*_tmp7DD));_tmp7DD[0]=({
struct Cyc_Primop_tok_struct _tmp7DE;_tmp7DE.tag=Cyc_Primop_tok;_tmp7DE.f1=(void*)((
void*)11);_tmp7DE;});_tmp7DD;});break;case 367: _LL327: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp7DF=_cycalloc(sizeof(*_tmp7DF));_tmp7DF[0]=({struct Cyc_Primop_tok_struct
_tmp7E0;_tmp7E0.tag=Cyc_Primop_tok;_tmp7E0.f1=(void*)((void*)2);_tmp7E0;});
_tmp7DF;});break;case 368: _LL328: yyval=yyvs[yyvsp_offset];break;case 369: _LL329:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7E1=_cycalloc(sizeof(*_tmp7E1));
_tmp7E1[0]=({struct Cyc_Exp_tok_struct _tmp7E2;_tmp7E2.tag=Cyc_Exp_tok;_tmp7E2.f1=
Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7E2;});_tmp7E1;});break;case 370: _LL32A: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7E3=_cycalloc(sizeof(*_tmp7E3));_tmp7E3[0]=({
struct Cyc_Exp_tok_struct _tmp7E4;_tmp7E4.tag=Cyc_Exp_tok;_tmp7E4.f1=Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7E4;});
_tmp7E3;});break;case 371: _LL32B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7E5=
_cycalloc(sizeof(*_tmp7E5));_tmp7E5[0]=({struct Cyc_Exp_tok_struct _tmp7E6;_tmp7E6.tag=
Cyc_Exp_tok;_tmp7E6.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7E6;});_tmp7E5;});break;case 372: _LL32C: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7E7=_cycalloc(sizeof(*_tmp7E7));_tmp7E7[0]=({
struct Cyc_Exp_tok_struct _tmp7E8;_tmp7E8.tag=Cyc_Exp_tok;_tmp7E8.f1=Cyc_Absyn_aggrmember_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
struct _tagged_arr*_tmp7E9=_cycalloc(sizeof(*_tmp7E9));_tmp7E9[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp7E9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7E8;});_tmp7E7;});break;case 373: _LL32D: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7EA=_cycalloc(sizeof(*_tmp7EA));_tmp7EA[0]=({
struct Cyc_Exp_tok_struct _tmp7EB;_tmp7EB.tag=Cyc_Exp_tok;_tmp7EB.f1=Cyc_Absyn_aggrarrow_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
struct _tagged_arr*_tmp7EC=_cycalloc(sizeof(*_tmp7EC));_tmp7EC[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp7EC;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7EB;});_tmp7EA;});break;case 374: _LL32E: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7ED=_cycalloc(sizeof(*_tmp7ED));_tmp7ED[0]=({
struct Cyc_Exp_tok_struct _tmp7EE;_tmp7EE.tag=Cyc_Exp_tok;_tmp7EE.f1=Cyc_Absyn_post_inc_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7EE;});
_tmp7ED;});break;case 375: _LL32F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7EF=
_cycalloc(sizeof(*_tmp7EF));_tmp7EF[0]=({struct Cyc_Exp_tok_struct _tmp7F0;_tmp7F0.tag=
Cyc_Exp_tok;_tmp7F0.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F0;});_tmp7EF;});break;case 376: _LL330: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1[0]=({
struct Cyc_Exp_tok_struct _tmp7F2;_tmp7F2.tag=Cyc_Exp_tok;_tmp7F2.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp7F3=_cycalloc(sizeof(*_tmp7F3));
_tmp7F3[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp7F4;_tmp7F4.tag=25;_tmp7F4.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
4)]);_tmp7F4.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp7F4;});_tmp7F3;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F2;});_tmp7F1;});break;case 377: _LL331: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7F5=_cycalloc(sizeof(*_tmp7F5));_tmp7F5[0]=({
struct Cyc_Exp_tok_struct _tmp7F6;_tmp7F6.tag=Cyc_Exp_tok;_tmp7F6.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp7F7=_cycalloc(sizeof(*_tmp7F7));
_tmp7F7[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp7F8;_tmp7F8.tag=25;_tmp7F8.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmp7F8.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp7F8;});_tmp7F7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F6;});_tmp7F5;});break;case 378: _LL332: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7F9=_cycalloc(sizeof(*_tmp7F9));_tmp7F9[0]=({
struct Cyc_Exp_tok_struct _tmp7FA;_tmp7FA.tag=Cyc_Exp_tok;_tmp7FA.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Fill_e_struct*_tmp7FB=_cycalloc(sizeof(*_tmp7FB));
_tmp7FB[0]=({struct Cyc_Absyn_Fill_e_struct _tmp7FC;_tmp7FC.tag=37;_tmp7FC.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7FC;});_tmp7FB;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7FA;});
_tmp7F9;});break;case 379: _LL333: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7FD=
_cycalloc(sizeof(*_tmp7FD));_tmp7FD[0]=({struct Cyc_Exp_tok_struct _tmp7FE;_tmp7FE.tag=
Cyc_Exp_tok;_tmp7FE.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Codegen_e_struct*
_tmp7FF=_cycalloc(sizeof(*_tmp7FF));_tmp7FF[0]=({struct Cyc_Absyn_Codegen_e_struct
_tmp800;_tmp800.tag=36;_tmp800.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp800;});_tmp7FF;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7FE;});
_tmp7FD;});break;case 380: _LL334: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp801=
_cycalloc(sizeof(*_tmp801));_tmp801[0]=({struct Cyc_Exp_tok_struct _tmp802;_tmp802.tag=
Cyc_Exp_tok;_tmp802.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp803=_cycalloc(sizeof(*_tmp803));_tmp803[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp804;_tmp804.tag=2;_tmp804.f1=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);
_tmp804;});_tmp803;}),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp802;});_tmp801;});break;case 381: _LL335: yyval=
yyvs[yyvsp_offset];break;case 382: _LL336: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp805=_cycalloc(sizeof(*_tmp805));_tmp805[0]=({struct Cyc_Exp_tok_struct _tmp806;
_tmp806.tag=Cyc_Exp_tok;_tmp806.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp806;});_tmp805;});break;case 383: _LL337: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 384: _LL338:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp807=_cycalloc(sizeof(*_tmp807));
_tmp807[0]=({struct Cyc_Exp_tok_struct _tmp808;_tmp808.tag=Cyc_Exp_tok;_tmp808.f1=
Cyc_Absyn_noinstantiate_exp(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp80A;_tmp80A.tag=2;_tmp80A.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp80A;});_tmp809;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp808;});_tmp807;});break;case 385: _LL339: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp80B=_cycalloc(sizeof(*_tmp80B));_tmp80B[0]=({
struct Cyc_Exp_tok_struct _tmp80C;_tmp80C.tag=Cyc_Exp_tok;_tmp80C.f1=Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp80D=_cycalloc(
sizeof(*_tmp80D));_tmp80D[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp80E;_tmp80E.tag=
2;_tmp80E.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp80E;});_tmp80D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp80C;});
_tmp80B;});break;case 386: _LL33A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp80F=
_cycalloc(sizeof(*_tmp80F));_tmp80F[0]=({struct Cyc_Exp_tok_struct _tmp810;_tmp810.tag=
Cyc_Exp_tok;_tmp810.f1=Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp810;});_tmp80F;});break;case 387: _LL33B: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp811=_cycalloc(sizeof(*_tmp811));_tmp811[0]=({
struct Cyc_Exp_tok_struct _tmp812;_tmp812.tag=Cyc_Exp_tok;_tmp812.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Struct_e_struct*_tmp813=_cycalloc(sizeof(*_tmp813));
_tmp813[0]=({struct Cyc_Absyn_Struct_e_struct _tmp814;_tmp814.tag=28;_tmp814.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp814.f2=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp814.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp814.f4=0;_tmp814;});
_tmp813;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp812;});
_tmp811;});break;case 388: _LL33C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp815=
_cycalloc(sizeof(*_tmp815));_tmp815[0]=({struct Cyc_Exp_tok_struct _tmp816;_tmp816.tag=
Cyc_Exp_tok;_tmp816.f1=Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp816;});_tmp815;});break;case 389: _LL33D: yyval=(
void*)({struct Cyc_ExpList_tok_struct*_tmp817=_cycalloc(sizeof(*_tmp817));_tmp817[
0]=({struct Cyc_ExpList_tok_struct _tmp818;_tmp818.tag=Cyc_ExpList_tok;_tmp818.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(
yyvs[yyvsp_offset]));_tmp818;});_tmp817;});break;case 390: _LL33E: yyval=(void*)({
struct Cyc_ExpList_tok_struct*_tmp819=_cycalloc(sizeof(*_tmp819));_tmp819[0]=({
struct Cyc_ExpList_tok_struct _tmp81A;_tmp81A.tag=Cyc_ExpList_tok;_tmp81A.f1=({
struct Cyc_List_List*_tmp81B=_cycalloc(sizeof(*_tmp81B));_tmp81B->hd=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp81B->tl=0;_tmp81B;});_tmp81A;});_tmp819;});break;case 391:
_LL33F: yyval=(void*)({struct Cyc_ExpList_tok_struct*_tmp81C=_cycalloc(sizeof(*
_tmp81C));_tmp81C[0]=({struct Cyc_ExpList_tok_struct _tmp81D;_tmp81D.tag=Cyc_ExpList_tok;
_tmp81D.f1=({struct Cyc_List_List*_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E->hd=
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp81E->tl=Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp81E;});_tmp81D;});
_tmp81C;});break;case 392: _LL340: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp81F=
_cycalloc(sizeof(*_tmp81F));_tmp81F[0]=({struct Cyc_Exp_tok_struct _tmp820;_tmp820.tag=
Cyc_Exp_tok;_tmp820.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f1,(*
Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp820;});_tmp81F;});
break;case 393: _LL341: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp821=_cycalloc(
sizeof(*_tmp821));_tmp821[0]=({struct Cyc_Exp_tok_struct _tmp822;_tmp822.tag=Cyc_Exp_tok;
_tmp822.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp822;});
_tmp821;});break;case 394: _LL342: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp823=
_cycalloc(sizeof(*_tmp823));_tmp823[0]=({struct Cyc_Exp_tok_struct _tmp824;_tmp824.tag=
Cyc_Exp_tok;_tmp824.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));_tmp824;});_tmp823;});break;case 395: _LL343: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp825=_cycalloc(sizeof(*_tmp825));_tmp825[0]=({
struct Cyc_Exp_tok_struct _tmp826;_tmp826.tag=Cyc_Exp_tok;_tmp826.f1=Cyc_Absyn_null_exp(
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmp826;});_tmp825;});break;case 396: _LL344: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp827=_cycalloc(sizeof(*_tmp827));_tmp827[0]=({struct Cyc_QualId_tok_struct
_tmp828;_tmp828.tag=Cyc_QualId_tok;_tmp828.f1=({struct _tuple1*_tmp829=_cycalloc(
sizeof(*_tmp829));_tmp829->f1=Cyc_Absyn_rel_ns_null;_tmp829->f2=({struct
_tagged_arr*_tmp82A=_cycalloc(sizeof(*_tmp82A));_tmp82A[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp82A;});_tmp829;});_tmp828;});_tmp827;});break;case 397:
_LL345: yyval=yyvs[yyvsp_offset];break;case 398: _LL346: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp82B=_cycalloc(sizeof(*_tmp82B));_tmp82B[0]=({struct Cyc_QualId_tok_struct
_tmp82C;_tmp82C.tag=Cyc_QualId_tok;_tmp82C.f1=({struct _tuple1*_tmp82D=_cycalloc(
sizeof(*_tmp82D));_tmp82D->f1=Cyc_Absyn_rel_ns_null;_tmp82D->f2=({struct
_tagged_arr*_tmp82E=_cycalloc(sizeof(*_tmp82E));_tmp82E[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp82E;});_tmp82D;});_tmp82C;});_tmp82B;});break;case 399:
_LL347: yyval=yyvs[yyvsp_offset];break;case 400: _LL348: yyval=yyvs[yyvsp_offset];
break;case 401: _LL349: yyval=yyvs[yyvsp_offset];break;case 402: _LL34A: yyval=yyvs[
yyvsp_offset];break;case 403: _LL34B: yyval=yyvs[yyvsp_offset];break;case 404: _LL34C:
break;case 405: _LL34D:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;break;default: _LL34E: break;}
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
_check_known_subscript_notnull(114,yyn - 126)]+ yyss[yyssp_offset];if((yystate >= 
0?yystate <= 5002: 0)?Cyc_yycheck[yystate]== yyss[yyssp_offset]: 0)yystate=(int)Cyc_yytable[
yystate];else{yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(114,yyn - 
126)];}goto yynewstate;yyerrlab: if(yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
yystate];if(yyn > - 32768?yyn < 5002: 0){int sze=0;struct _tagged_arr msg;int x;int count;
count=0;for(x=yyn < 0?- yyn: 0;x < 240 / sizeof(unsigned char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(5003,x + yyn)]== x)(sze +=Cyc_Std_strlen(Cyc_yytname[
x])+ 15,count ++);}msg=({unsigned int _tmp82F=(unsigned int)(sze + 15);
unsigned char*_tmp830=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmp82F));struct _tagged_arr _tmp832=_tag_arr(_tmp830,sizeof(
unsigned char),(unsigned int)(sze + 15));{unsigned int _tmp831=_tmp82F;
unsigned int i;for(i=0;i < _tmp831;i ++){_tmp830[i]='\000';}}_tmp832;});Cyc_Std_strcpy(
msg,_tag_arr("parse error",sizeof(unsigned char),12));if(count < 5){count=0;for(x=
yyn < 0?- yyn: 0;x < 240 / sizeof(unsigned char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(5003,x + yyn)]== x){Cyc_Std_strcat(msg,count == 0?
_tag_arr(", expecting `",sizeof(unsigned char),14): _tag_arr(" or `",sizeof(
unsigned char),6));Cyc_Std_strcat(msg,Cyc_yytname[x]);Cyc_Std_strcat(msg,
_tag_arr("'",sizeof(unsigned char),2));count ++;}}}Cyc_yyerror((struct _tagged_arr)
msg);}else{Cyc_yyerror(_tag_arr("parse error",sizeof(unsigned char),12));}}goto
yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0)return 1;Cyc_yychar=- 2;}
yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0)return 1;
yyvsp_offset --;yystate=(int)yyss[_check_known_subscript_notnull(10000,--
yyssp_offset)];yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[yystate];if(yyn == 
- 32768)goto yyerrdefault;yyn +=1;if((yyn < 0?1: yyn > 5002)?1: Cyc_yycheck[yyn]!= 1)
goto yyerrdefault;yyn=(int)Cyc_yytable[yyn];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 825)
return 0;yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;
yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto
yynewstate;}void Cyc_yyprint(int i,void*v){void*_tmp83C=v;struct _tuple12*_tmp83D;
struct _tuple12 _tmp83E;int _tmp83F;unsigned char _tmp840;short _tmp841;struct
_tagged_arr _tmp842;struct Cyc_Core_Opt*_tmp843;struct Cyc_Core_Opt*_tmp844;struct
Cyc_Core_Opt _tmp845;struct _tagged_arr*_tmp846;struct _tuple1*_tmp847;struct
_tuple1 _tmp848;void*_tmp849;struct _tagged_arr*_tmp84A;_LL351: if(*((void**)
_tmp83C)!= Cyc_Int_tok)goto _LL353;_tmp83D=((struct Cyc_Int_tok_struct*)_tmp83C)->f1;
_tmp83E=*_tmp83D;_tmp83F=_tmp83E.f2;_LL352:({struct Cyc_Std_Int_pa_struct _tmp84C;
_tmp84C.tag=1;_tmp84C.f1=(int)((unsigned int)_tmp83F);{void*_tmp84B[1]={& _tmp84C};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(
_tmp84B,sizeof(void*),1));}});goto _LL350;_LL353: if(*((void**)_tmp83C)!= Cyc_Char_tok)
goto _LL355;_tmp840=((struct Cyc_Char_tok_struct*)_tmp83C)->f1;_LL354:({struct Cyc_Std_Int_pa_struct
_tmp84E;_tmp84E.tag=1;_tmp84E.f1=(int)((unsigned int)((int)_tmp840));{void*
_tmp84D[1]={& _tmp84E};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%c",sizeof(
unsigned char),3),_tag_arr(_tmp84D,sizeof(void*),1));}});goto _LL350;_LL355: if(*((
void**)_tmp83C)!= Cyc_Short_tok)goto _LL357;_tmp841=((struct Cyc_Short_tok_struct*)
_tmp83C)->f1;_LL356:({struct Cyc_Std_Int_pa_struct _tmp850;_tmp850.tag=1;_tmp850.f1=(
int)((unsigned int)((int)_tmp841));{void*_tmp84F[1]={& _tmp850};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%ds",sizeof(unsigned char),4),_tag_arr(_tmp84F,sizeof(
void*),1));}});goto _LL350;_LL357: if(*((void**)_tmp83C)!= Cyc_String_tok)goto
_LL359;_tmp842=((struct Cyc_String_tok_struct*)_tmp83C)->f1;_LL358:({struct Cyc_Std_String_pa_struct
_tmp852;_tmp852.tag=0;_tmp852.f1=(struct _tagged_arr)_tmp842;{void*_tmp851[1]={&
_tmp852};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\"%s\"",sizeof(unsigned char),
5),_tag_arr(_tmp851,sizeof(void*),1));}});goto _LL350;_LL359: if(*((void**)_tmp83C)
!= Cyc_Stringopt_tok)goto _LL35B;_tmp843=((struct Cyc_Stringopt_tok_struct*)
_tmp83C)->f1;if(_tmp843 != 0)goto _LL35B;_LL35A:({void*_tmp853[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("NULL",sizeof(unsigned char),5),_tag_arr(_tmp853,sizeof(
void*),0));});goto _LL350;_LL35B: if(*((void**)_tmp83C)!= Cyc_Stringopt_tok)goto
_LL35D;_tmp844=((struct Cyc_Stringopt_tok_struct*)_tmp83C)->f1;if(_tmp844 == 0)
goto _LL35D;_tmp845=*_tmp844;_tmp846=(struct _tagged_arr*)_tmp845.v;_LL35C:({
struct Cyc_Std_String_pa_struct _tmp855;_tmp855.tag=0;_tmp855.f1=(struct
_tagged_arr)*_tmp846;{void*_tmp854[1]={& _tmp855};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\"%s\"",sizeof(unsigned char),5),_tag_arr(_tmp854,sizeof(void*),1));}});
goto _LL350;_LL35D: if(*((void**)_tmp83C)!= Cyc_QualId_tok)goto _LL35F;_tmp847=((
struct Cyc_QualId_tok_struct*)_tmp83C)->f1;_tmp848=*_tmp847;_tmp849=_tmp848.f1;
_tmp84A=_tmp848.f2;_LL35E: {struct Cyc_List_List*_tmp856=0;{void*_tmp857=_tmp849;
struct Cyc_List_List*_tmp858;struct Cyc_List_List*_tmp859;_LL362: if(_tmp857 <= (
void*)1?1:*((int*)_tmp857)!= 0)goto _LL364;_tmp858=((struct Cyc_Absyn_Rel_n_struct*)
_tmp857)->f1;_LL363: _tmp856=_tmp858;goto _LL361;_LL364: if(_tmp857 <= (void*)1?1:*((
int*)_tmp857)!= 1)goto _LL366;_tmp859=((struct Cyc_Absyn_Abs_n_struct*)_tmp857)->f1;
_LL365: _tmp856=_tmp859;goto _LL361;_LL366: if((int)_tmp857 != 0)goto _LL361;_LL367:
goto _LL361;_LL361:;}for(0;_tmp856 != 0;_tmp856=_tmp856->tl){({struct Cyc_Std_String_pa_struct
_tmp85B;_tmp85B.tag=0;_tmp85B.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp856->hd);{void*_tmp85A[1]={& _tmp85B};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",
sizeof(unsigned char),5),_tag_arr(_tmp85A,sizeof(void*),1));}});}({struct Cyc_Std_String_pa_struct
_tmp85D;_tmp85D.tag=0;_tmp85D.f1=(struct _tagged_arr)*_tmp84A;{void*_tmp85C[1]={&
_tmp85D};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",sizeof(unsigned char),5),
_tag_arr(_tmp85C,sizeof(void*),1));}});goto _LL350;}_LL35F:;_LL360:({void*_tmp85E[
0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("?",sizeof(unsigned char),2),
_tag_arr(_tmp85E,sizeof(void*),0));});goto _LL350;_LL350:;}struct Cyc_List_List*
Cyc_Parse_parse_file(struct Cyc_Std___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({
struct Cyc_Core_Opt*_tmp85F=_cycalloc(sizeof(*_tmp85F));_tmp85F->v=Cyc_Lexing_from_file(
f);_tmp85F;});Cyc_yyparse();return Cyc_Parse_parse_result;}
