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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*
Cyc_Std_stderr;extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char
Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;
struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;
};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,
struct _tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(
struct _tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_vrprintf(
struct _RegionHandle*r1,struct _tagged_arr fmt,struct _tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{
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
1,1,3,1,2,3,6,4,3,5,1,1,1,2,3,3,0,1,1,2,5,1,2,1,3,3,4,4,5,4,4,2,2,1,3,4,4,5,1,1,
4,4,1,0,1,1,1,1,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,
1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,2,3,3,4,4,3,5,4,4,2,1,1,1,
1,1,1,6,3,2,2,3,1,2,2,3,1,2,1,2,1,2,5,7,7,8,6,0,3,4,5,6,7,0,3,4,5,5,7,6,7,7,8,7,
8,8,9,6,7,7,8,9,3,2,2,2,3,2,4,5,1,3,1,2,1,1,1,1,4,4,5,2,2,1,3,1,2,1,3,1,3,1,3,1,
1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,
3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,1,1,1,1,4,3,4,3,3,2,2,6,7,4,4,1,
1,1,3,2,5,4,4,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[828]={
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
193,294,296,0,288,0,0,201,100,102,0,160,0,210,0,208,158,0,0,219,203,175,0,0,0,
184,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,385,392,0,391,322,0,324,326,328,330,332,333,
337,338,335,336,340,341,343,344,346,347,348,301,375,372,0,374,0,0,0,4,6,107,98,0,
0,0,112,121,120,0,0,115,0,0,0,0,0,0,0,0,271,275,0,0,0,0,227,0,230,0,15,299,158,0,
168,0,0,0,161,131,164,152,205,151,140,7,0,0,0,293,85,196,0,295,192,194,289,0,202,
173,215,0,209,212,0,160,0,211,0,358,0,0,185,0,189,380,381,0,363,0,0,0,0,0,0,0,0,
350,388,0,0,373,371,389,0,0,105,108,0,109,122,119,119,0,0,0,0,0,0,0,0,0,0,246,
277,0,0,0,16,160,0,169,167,0,160,0,0,0,0,44,46,195,297,214,217,0,218,213,0,0,0,
186,190,0,0,0,0,318,319,362,390,0,393,314,387,82,0,118,117,241,0,246,256,0,0,0,0,
0,0,0,0,0,0,0,0,278,0,0,154,170,162,153,158,0,49,50,216,361,360,0,191,364,0,0,
378,0,0,0,252,0,0,258,0,0,0,266,0,0,0,0,0,0,0,0,245,0,226,0,160,0,0,0,0,379,123,
242,0,0,0,243,257,259,260,0,268,267,0,262,0,0,0,0,246,247,0,163,155,0,0,0,0,0,0,
244,261,269,263,264,0,0,246,248,0,0,187,365,0,252,253,265,246,249,270,51,0,252,
254,246,250,366,255,251,0,0,0};static short Cyc_yydefgoto[115]={825,42,43,44,251,
45,386,46,388,47,218,48,49,66,67,456,457,50,158,51,52,134,135,53,292,130,54,223,
224,100,101,102,225,152,397,398,399,55,56,552,553,554,57,58,59,60,126,112,451,
480,61,481,442,581,434,438,439,297,272,163,81,82,496,391,497,498,469,470,153,274,
275,482,304,254,255,256,257,258,259,717,771,260,261,287,288,472,473,262,263,373,
192,460,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,513,514,208,
209,63,110,159,308};static short Cyc_yypact[828]={1977,- -32768,- -32768,- -32768,-
-32768,- 38,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,4,- 3,602,- -32768,234,54,-
-32768,- -32768,148,163,- -32768,104,- -32768,- -32768,- -32768,462,- -32768,200,219,
237,- -32768,- -32768,1860,- -32768,- 62,109,- -32768,117,4,4,4,- -32768,- -32768,317,-
-32768,146,2449,150,41,94,- -32768,234,244,281,231,2574,- -32768,234,292,- -32768,
74,- -32768,- -32768,- -32768,2257,- -32768,302,306,334,2257,308,331,356,269,- -32768,
- -32768,4771,4771,191,329,4942,4317,4317,- -32768,1860,2093,1860,2093,- -32768,361,
364,- -32768,2385,2574,2574,2574,4942,- -32768,- -32768,244,- -32768,234,- -32768,143,
384,1252,- -32768,2449,117,- -32768,1796,4771,2989,- -32768,150,4,- -32768,- -32768,
4771,- -32768,2093,400,- -32768,405,404,428,234,449,2257,- -32768,- -32768,- -32768,-
-32768,466,4317,452,2257,454,314,4,4,424,- -32768,93,93,- -32768,- -32768,- -32768,-
-32768,- -32768,572,201,- -32768,4558,459,- -32768,4317,4392,472,479,489,502,504,
513,519,244,4633,4633,- -32768,- -32768,- -32768,- -32768,1616,521,4708,4708,- -32768,
4708,- -32768,- -32768,463,- -32768,- 32,495,510,516,526,471,151,398,467,224,- -32768,
763,4708,262,- -32768,- -32768,- 60,547,- -32768,566,- -32768,582,- -32768,462,3072,
2449,- -32768,- -32768,- -32768,- -32768,584,4942,- -32768,397,560,244,234,573,-
-32768,599,186,601,2657,604,623,618,619,3155,2657,124,616,2657,2657,- 48,612,-
-32768,- -32768,622,1375,1375,117,1375,- -32768,- -32768,- -32768,625,- -32768,-
-32768,- -32768,203,- -32768,- -32768,- -32768,628,638,- -32768,87,627,624,249,629,
620,102,- -32768,626,226,102,635,28,234,- -32768,4317,640,- -32768,- -32768,483,-
-32768,311,4771,886,485,4771,- -32768,630,642,4942,4942,2324,3238,16,- -32768,213,
- -32768,- -32768,87,- -32768,- -32768,2215,- -32768,79,- -32768,- -32768,4942,1616,-
-32768,4942,- -32768,- -32768,2740,- -32768,671,4317,2154,4317,4317,4317,4317,4317,
644,1616,- -32768,- -32768,1375,645,494,4317,- -32768,- -32768,- -32768,- -32768,4708,
4317,4708,4708,4708,4708,4708,4708,4708,4708,4708,4708,4708,4708,4708,4708,4708,
4708,4708,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,4317,- -32768,191,- -32768,- -32768,3321,191,4317,- -32768,
848,643,- -32768,- -32768,1860,- -32768,1860,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,4317,649,647,- -32768,648,452,- -32768,244,589,4317,651,
4317,4317,720,1498,656,- -32768,- -32768,- -32768,322,717,- -32768,3404,2513,- -32768,
- -32768,2657,667,2657,1734,- -32768,- -32768,2385,- -32768,- -32768,- -32768,4317,-
-32768,4942,659,328,- -32768,- -32768,646,- -32768,670,661,4842,624,- -32768,4771,-
-32768,- -32768,- -32768,- -32768,226,- -32768,1860,- -32768,663,666,662,- -32768,-
-32768,- -32768,- -32768,- -32768,2257,- -32768,275,191,4317,2257,52,- -32768,- -32768,
- -32768,401,- -32768,34,678,- -32768,- -32768,- -32768,672,624,674,- -32768,668,213,
4827,4771,3487,- -32768,- -32768,- -32768,675,669,686,- -32768,- -32768,415,3072,496,
679,462,499,677,680,681,507,4942,684,683,4475,- -32768,- -32768,685,687,495,372,
510,516,526,471,151,151,398,398,398,398,467,467,224,224,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,690,- -32768,162,417,4771,- -32768,- -32768,- -32768,- -32768,
397,4317,4942,- -32768,- -32768,- -32768,694,234,326,509,4317,515,522,688,3570,3653,
333,- -32768,- -32768,696,698,695,699,- -32768,691,- -32768,2449,- -32768,- -32768,703,
4771,- -32768,702,87,697,- -32768,- -32768,126,- -32768,620,- -32768,- -32768,- -32768,
490,719,28,- -32768,- -32768,- -32768,711,- -32768,- -32768,- -32768,- -32768,886,-
-32768,- -32768,- -32768,721,- -32768,- -32768,722,624,102,- -32768,715,729,261,724,-
-32768,2823,- -32768,- -32768,- -32768,2449,- -32768,4317,808,4317,4392,730,729,731,
848,- -32768,- -32768,4317,4317,- -32768,- -32768,- -32768,848,102,- -32768,- -32768,
735,- -32768,734,84,589,2657,528,749,2657,4317,3736,336,3819,344,3902,613,- -32768,
751,4317,764,- -32768,624,55,- -32768,- -32768,758,624,4942,750,752,754,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,755,- -32768,- -32768,756,757,4317,- -32768,
- -32768,3072,537,759,761,- -32768,671,- -32768,- -32768,432,- -32768,- -32768,- -32768,
- -32768,4942,- -32768,- -32768,835,767,613,- -32768,539,2657,546,3985,2657,548,4068,
4151,348,2257,760,768,- -32768,352,2657,- -32768,- -32768,766,- -32768,703,778,-
-32768,- -32768,- -32768,- -32768,- -32768,380,- -32768,- -32768,4942,850,- -32768,2906,
550,2657,634,776,779,- -32768,2657,2657,554,- -32768,2657,2657,559,2657,565,4234,-
2,1375,- -32768,4317,- -32768,670,624,774,4317,781,777,- -32768,- -32768,- -32768,
4317,772,782,- -32768,- -32768,- -32768,- -32768,2657,- -32768,- -32768,2657,- -32768,
2657,2657,576,4317,1006,- -32768,585,- -32768,- -32768,802,440,791,4942,382,1375,-
-32768,- -32768,- -32768,- -32768,- -32768,2657,394,613,- -32768,2657,793,- -32768,-
-32768,794,1129,- -32768,- -32768,1006,- -32768,- -32768,- -32768,797,634,- -32768,613,
- -32768,- -32768,- -32768,- -32768,907,908,- -32768};static short Cyc_yypgoto[115]={-
-32768,15,- -32768,586,- -32768,- -32768,- -32768,- -32768,- -32768,- 53,- 35,- 57,-
-32768,446,- 30,318,- -32768,45,- -32768,- 6,- -32768,- -32768,- 112,- -32768,512,- 16,-
-32768,368,- -32768,- -32768,- -32768,701,705,- 34,- -32768,- -32768,373,- -32768,-
-32768,56,- -32768,- -32768,- 12,- 33,- 61,- -32768,- -32768,858,- -32768,- 116,- 50,- 119,
- 412,161,339,351,636,- 402,- 90,- 286,806,- -32768,- 214,- 152,- 359,- 217,- -32768,464,-
176,- 68,- 111,86,20,144,- -32768,- -32768,11,- 239,- -32768,- 627,- 519,- -32768,- -32768,
- 9,784,332,- -32768,118,- 14,- -32768,- 150,- 342,- 147,591,590,596,600,588,347,196,
355,357,- 64,461,- -32768,- -32768,- -32768,- 326,- -32768,- -32768,8,892,- -32768,- 340,
- 144};static short Cyc_yytable[5050]={119,162,271,390,117,270,113,336,62,133,113,
309,426,427,83,429,322,319,279,154,155,323,118,540,90,285,125,381,124,491,69,84,
584,85,578,535,103,95,96,538,382,422,1,343,62,454,119,220,221,222,117,62,537,64,
544,383,62,161,94,253,423,119,108,252,114,264,138,62,120,604,128,142,784,226,602,
468,136,742,191,210,1,151,151,84,344,151,31,161,84,550,551,302,567,34,227,124,
509,68,230,296,312,151,65,62,62,62,62,266,785,277,211,212,213,214,219,151,151,
455,1,300,228,339,340,151,341,595,594,62,301,265,31,447,150,150,459,452,150,34,
601,62,492,374,493,86,36,136,280,84,575,311,435,307,150,444,597,84,508,491,804,
722,331,119,305,466,467,264,150,150,31,62,458,305,307,- 180,150,34,814,151,- 206,-
180,465,32,310,- 206,- 206,- 116,821,436,32,- 206,226,301,89,824,253,253,678,253,252,
252,111,252,306,392,266,640,31,446,97,98,307,306,402,34,400,433,99,450,151,437,
36,351,352,417,62,269,475,418,150,393,- 158,39,40,41,62,31,- 158,403,303,302,32,
428,34,16,17,18,459,313,65,312,721,129,111,121,229,724,87,437,122,353,354,62,123,
290,478,479,161,432,501,150,694,88,449,682,489,635,161,406,253,125,156,161,252,
471,617,151,157,484,151,407,136,819,151,151,151,531,532,533,90,823,84,337,431,91,
151,314,315,392,432,151,151,65,151,503,504,505,486,459,62,92,62,487,485,31,512,
488,151,375,376,377,34,626,485,62,132,162,150,93,359,150,39,40,41,150,150,150,
360,361,269,789,156,681,129,119,443,150,157,561,415,534,150,150,119,150,512,378,
568,608,392,146,607,119,147,379,380,117,609,150,585,409,446,131,725,468,593,416,
296,549,420,421,587,161,137,572,62,140,62,459,1,686,161,542,685,543,512,139,31,
143,32,161,464,512,33,34,35,295,432,107,574,151,686,564,32,685,141,644,296,432,
151,577,638,645,655,160,337,709,144,151,432,573,151,432,499,712,502,586,630,754,
506,432,337,758,161,432,592,489,662,432,145,596,215,62,516,1,667,679,355,356,588,
216,150,84,733,690,611,231,84,432,691,150,151,151,633,696,392,432,31,432,150,620,
763,150,810,34,697,104,105,106,539,432,36,151,599,281,813,283,600,396,663,62,282,
39,40,41,226,119,615,786,636,264,616,686,637,555,685,557,558,300,562,437,284,150,
150,737,400,151,301,738,39,40,41,807,151,349,350,432,803,523,524,525,526,31,150,
286,62,289,811,294,34,764,291,643,318,119,342,36,569,117,571,345,151,818,278,1,
820,324,39,40,41,669,670,671,325,661,357,358,150,462,463,474,463,471,326,150,550,
551,298,299,511,432,618,432,392,621,432,327,761,328,84,739,689,625,432,646,432,
392,329,809,695,648,432,150,330,392,338,317,649,432,320,320,346,219,703,432,161,
715,716,347,333,334,70,734,432,743,432,348,320,320,384,320,745,432,749,432,767,
315,769,770,776,432,401,31,151,779,432,161,320,385,34,781,432,- 177,392,404,647,
310,- 177,- 177,652,654,801,432,- 177,387,301,394,39,40,41,805,432,71,72,151,73,74,
521,522,34,253,700,701,405,252,408,75,755,410,76,77,527,528,411,150,529,530,78,
412,413,419,79,80,424,84,392,425,430,433,253,151,217,440,252,445,441,476,453,307,
253,448,687,461,252,477,150,320,343,507,545,510,559,541,546,253,556,563,253,252,
547,565,252,570,576,579,580,582,589,591,706,708,590,711,32,714,613,614,603,719,
605,612,150,606,622,619,628,623,624,151,627,631,702,650,632,705,634,642,656,657,
659,658,660,732,269,664,320,666,320,320,320,320,320,320,320,320,320,320,320,320,
320,320,320,320,320,672,674,676,677,747,680,629,751,753,683,688,692,693,698,699,
150,362,363,364,365,366,367,368,369,370,371,704,718,744,723,720,748,726,727,320,
728,729,730,731,735,740,759,372,736,741,762,757,756,760,783,164,165,765,787,772,
773,790,793,791,795,796,768,792,166,794,167,774,775,168,806,777,778,808,780,816,
817,169,170,822,802,171,172,173,174,826,827,673,175,500,176,548,177,178,641,389,
127,639,797,788,668,798,70,799,800,273,320,395,665,293,675,598,515,517,520,31,
179,180,181,182,518,490,34,812,109,519,321,815,0,183,0,0,0,184,0,0,0,466,467,0,
185,0,0,186,187,188,0,0,0,320,189,190,0,31,72,0,73,74,0,0,34,0,0,0,0,0,0,75,0,0,
76,77,0,0,0,466,467,0,78,0,0,0,79,80,0,0,320,2,3,4,115,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,715,716,21,164,165,232,0,233,234,235,236,237,238,239,240,22,166,
23,167,241,0,168,24,242,0,0,27,28,243,169,170,244,245,171,172,173,174,29,30,246,
175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,320,0,0,0,0,0,0,0,320,247,179,180,
181,182,32,33,34,35,0,248,116,0,0,183,0,0,37,250,0,0,0,0,0,0,185,0,0,186,187,188,
0,0,0,0,189,190,2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,769,770,21,
164,165,232,0,233,234,235,236,237,238,239,240,22,166,23,167,241,0,168,24,242,0,0,
27,28,243,169,170,244,245,171,172,173,174,29,30,246,175,0,176,0,177,178,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,247,179,180,181,182,32,33,34,35,0,248,116,0,0,
183,0,0,37,250,0,0,0,0,0,0,185,0,0,186,187,188,0,0,0,0,189,190,2,3,4,115,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,164,165,232,0,233,234,235,236,237,238,
239,240,22,166,23,167,241,0,168,24,242,0,0,27,28,243,169,170,244,245,171,172,173,
174,29,30,246,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,247,
179,180,181,182,32,33,34,35,0,248,116,249,0,183,0,0,37,250,0,0,0,0,0,0,185,0,0,
186,187,188,0,0,0,0,189,190,2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,21,164,165,232,0,233,234,235,236,237,238,239,240,22,166,23,167,241,0,168,24,
242,0,0,27,28,243,169,170,244,245,171,172,173,174,29,30,246,175,0,176,0,177,178,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,247,179,180,181,182,32,33,34,35,0,248,
116,0,0,183,0,0,37,250,0,0,0,0,0,0,185,0,0,186,187,188,0,0,0,0,189,190,2,3,4,115,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,164,165,0,0,0,0,0,0,0,0,0,0,22,
166,23,167,0,0,168,24,0,0,0,27,28,0,169,170,0,0,171,172,173,174,29,30,0,175,0,
176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,32,33,
34,35,0,560,0,0,0,183,0,0,37,250,0,0,0,0,0,0,185,0,0,186,187,188,0,0,0,0,189,190,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,164,165,0,0,0,0,0,0,0,0,0,0,22,166,
0,167,0,0,168,0,0,0,0,27,28,0,169,170,0,0,171,172,173,174,29,30,0,175,0,176,0,
177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,32,33,34,35,
0,0,335,0,0,183,0,0,37,250,0,0,0,0,0,0,185,0,0,186,187,188,0,0,0,0,189,190,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,0,164,165,0,0,0,0,0,0,0,0,0,0,22,166,0,167,
0,0,168,0,0,0,0,27,28,0,169,170,0,0,171,172,173,174,29,30,0,175,0,176,0,177,178,
0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,31,179,180,181,182,32,33,34,
35,0,22,0,0,0,183,0,0,37,250,0,0,27,28,0,0,185,0,0,186,187,188,0,29,30,0,189,190,
- 8,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,267,0,21,268,0,0,0,0,32,
33,0,35,0,269,0,22,0,23,0,0,37,38,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,32,33,34,35,0,0,0,-
8,0,36,0,0,37,38,0,0,0,0,0,0,39,40,41,- 8,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,
0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,
0,0,0,0,32,33,34,35,0,0,0,0,0,36,0,0,37,38,0,0,0,0,0,0,39,40,41,1,0,2,3,4,5,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,
0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,0,1,0,2,3,4,115,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,21,0,0,0,31,0,0,0,0,32,33,34,35,22,0,0,- 8,0,36,0,24,37,
38,0,27,28,0,0,0,39,40,41,0,0,0,29,30,0,1,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,0,0,0,31,0,0,0,0,32,33,34,35,22,0,0,0,0,36,0,0,37,38,0,27,28,0,
0,0,39,40,41,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,70,0,0,0,0,0,267,
0,0,31,0,0,0,0,32,33,34,35,0,269,0,0,0,310,- 158,0,37,38,0,0,- 158,0,301,0,39,40,
41,7,8,9,10,11,12,13,14,15,16,17,18,19,20,31,72,0,73,74,0,0,34,0,0,0,0,0,0,75,22,
0,76,77,0,0,0,0,0,0,78,27,28,0,79,80,0,0,0,0,0,0,29,30,0,0,0,2,3,4,115,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,267,0,0,0,0,0,0,0,32,33,0,35,22,269,23,0,
0,300,0,24,37,38,0,27,28,0,301,0,39,40,41,0,0,0,29,30,0,0,0,0,0,0,2,3,4,115,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,32,33,0,35,0,0,116,22,217,
23,0,0,37,38,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,2,3,4,115,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,32,33,0,35,0,0,116,22,0,23,0,
0,37,38,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,2,3,4,115,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,32,33,0,35,22,0,0,0,0,0,0,24,37,
38,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,32,33,0,35,0,0,0,0,0,0,0,0,37,38,164,165,232,0,233,234,235,
236,237,238,239,240,0,166,0,167,241,0,168,0,242,0,0,0,0,243,169,170,244,245,171,
172,173,174,0,0,246,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,247,179,180,181,182,0,0,34,0,0,248,116,0,0,183,0,0,0,184,164,165,0,0,0,0,185,
494,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,
174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,
180,181,182,0,0,34,0,0,0,321,495,0,183,0,0,0,184,164,165,0,466,467,0,185,0,0,186,
187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,
175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,
0,0,34,0,0,0,321,684,0,183,0,0,0,184,164,165,0,466,467,0,185,0,0,186,187,188,0,
166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,
0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,
0,0,321,766,0,183,0,0,0,184,164,165,0,466,467,0,185,0,0,186,187,188,0,166,0,167,
189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,0,0,0,
183,0,0,0,184,164,165,0,0,0,276,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,
0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,321,0,0,183,0,0,0,184,
164,165,0,0,0,0,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,
170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,414,0,0,0,183,0,0,0,184,164,165,0,0,0,
0,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,
173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,
179,180,181,182,0,0,34,0,0,0,0,0,0,183,0,0,0,184,164,165,0,0,0,483,185,0,0,186,
187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,
175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,
0,0,34,0,0,0,0,0,0,183,536,0,0,184,164,165,0,0,0,0,185,0,0,186,187,188,0,166,0,
167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,
178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,0,
0,0,183,566,0,0,184,164,165,0,0,0,0,185,0,0,186,187,188,0,166,0,167,189,190,168,
0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,0,0,0,183,0,0,0,
184,164,165,0,0,0,610,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,
169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,651,0,0,0,183,0,0,0,184,164,165,0,
0,0,0,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,
172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
31,179,180,181,182,0,0,34,0,0,653,0,0,0,183,0,0,0,184,164,165,0,0,0,0,185,0,0,
186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,
0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,
182,0,0,34,0,0,0,0,0,0,183,707,0,0,184,164,165,0,0,0,0,185,0,0,186,187,188,0,166,
0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,
177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,
0,0,0,0,183,710,0,0,184,164,165,0,0,0,0,185,0,0,186,187,188,0,166,0,167,189,190,
168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,713,0,0,0,183,0,
0,0,184,164,165,0,0,0,0,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,
0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,0,0,0,183,746,0,0,184,164,165,
0,0,0,0,185,0,0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,
171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,31,179,180,181,182,0,0,34,0,0,0,0,0,0,183,750,0,0,184,164,165,0,0,0,0,185,0,
0,186,187,188,0,166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,
0,0,0,175,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,
181,182,0,0,34,0,0,0,0,0,0,183,752,0,0,184,164,165,0,0,0,0,185,0,0,186,187,188,0,
166,0,167,189,190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,
0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,
0,0,0,0,0,183,782,0,0,184,164,165,0,0,0,0,185,0,0,186,187,188,0,166,0,167,189,
190,168,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,175,0,176,0,177,178,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,164,165,0,
183,0,0,0,184,0,0,0,0,0,166,185,0,0,186,187,188,0,0,0,0,189,190,169,170,0,0,171,
172,173,174,0,0,0,0,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,
179,180,181,182,0,0,34,0,0,0,321,0,0,183,0,0,0,184,164,165,0,0,0,0,185,0,0,186,
187,188,0,166,0,0,189,190,0,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,0,0,
176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,
34,0,0,0,629,0,0,183,0,0,0,184,164,165,0,0,0,0,185,0,0,186,187,188,0,166,0,0,189,
190,0,0,0,0,0,0,0,0,169,170,0,0,171,172,173,174,0,0,0,0,0,176,0,177,178,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,164,165,0,316,
0,0,0,184,0,0,0,0,0,166,185,0,0,186,187,188,0,0,0,0,189,190,169,170,0,0,171,172,
173,174,0,0,0,0,0,176,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,179,
180,181,182,0,0,34,0,0,0,164,165,0,332,0,0,0,184,0,0,0,0,0,166,185,0,0,186,187,
188,0,0,0,0,189,190,169,170,0,0,171,172,173,174,0,0,0,0,0,176,0,177,178,0,0,0,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,31,179,180,181,182,0,0,34,0,0,0,22,
0,0,183,0,0,0,184,0,0,0,27,28,0,185,0,0,186,187,188,0,0,29,30,189,190,148,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,32,
33,0,35,0,0,149,0,0,0,27,28,37,38,22,0,0,0,0,0,0,29,30,0,0,27,28,0,0,0,0,0,0,0,0,
0,29,30,0,0,0,0,0,0,0,0,0,0,267,0,0,0,0,0,0,0,32,33,0,35,0,269,0,583,0,0,0,0,37,
38,0,32,33,0,35,0,0,0,0,0,0,0,0,37,38,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,33,0,35,0,0,0,
0,0,0,0,0,37,38};static short Cyc_yycheck[5050]={57,91,121,217,57,121,56,183,0,66,
60,155,251,252,23,254,168,167,129,87,88,168,57,382,36,137,59,87,58,315,22,23,444,
25,436,375,48,99,100,379,100,89,1,75,36,17,103,104,105,106,103,43,378,91,396,115,
48,91,43,116,108,118,54,116,56,118,75,59,57,481,62,80,74,107,476,292,68,704,92,
93,1,87,88,75,116,91,89,121,80,5,6,152,418,96,110,125,335,100,114,149,161,107,98,
95,96,97,98,119,110,123,95,96,97,98,103,121,122,89,1,103,112,185,186,129,188,467,
466,119,112,118,89,275,87,88,284,279,91,96,104,131,316,205,318,89,103,137,131,
139,434,161,63,117,107,272,102,147,332,443,785,104,176,218,69,111,112,218,121,
122,89,161,282,69,117,99,129,96,803,183,99,105,291,94,103,104,105,101,813,100,94,
110,224,112,88,820,251,252,608,254,251,252,106,254,109,217,216,547,89,105,99,100,
117,109,228,96,226,89,99,278,224,269,103,70,71,99,216,99,294,103,183,218,104,114,
115,116,226,89,110,229,152,300,94,253,96,17,18,19,396,161,98,310,662,108,106,103,
111,667,108,307,108,108,109,253,112,145,298,299,300,105,325,224,629,108,278,613,
304,113,310,91,335,312,89,315,335,292,498,291,95,301,294,103,282,810,298,299,300,
359,360,361,310,818,292,183,99,103,310,104,105,321,105,315,316,98,318,327,328,
329,103,467,310,100,312,108,302,89,338,112,332,65,66,67,96,507,312,325,103,425,
291,100,114,294,114,115,116,298,299,300,122,123,99,761,89,90,108,410,105,310,95,
410,240,373,315,316,419,318,378,103,419,486,382,100,486,428,103,111,112,428,487,
332,446,235,105,100,668,600,109,241,436,403,244,245,451,425,100,428,386,89,388,
547,1,616,434,386,616,388,418,103,89,99,94,443,99,425,95,96,97,101,105,100,432,
425,637,99,94,637,90,99,476,105,434,101,541,105,99,104,316,99,105,443,105,428,
446,105,324,99,326,451,510,99,330,105,332,99,486,105,463,485,575,105,102,468,99,
453,344,1,583,609,68,69,453,105,425,463,686,625,488,91,468,105,625,434,486,487,
110,633,498,105,89,105,443,501,110,446,110,96,638,49,50,51,380,105,103,507,101,
103,110,101,105,110,576,501,105,114,115,116,548,572,101,756,101,572,105,738,105,
405,738,407,408,103,410,579,102,486,487,101,546,541,112,105,114,115,116,101,548,
72,73,105,785,351,352,353,354,89,507,104,546,89,795,103,96,735,108,553,103,620,
101,103,422,620,424,74,576,810,126,1,813,103,114,115,116,89,90,91,103,572,117,
118,541,104,105,104,105,600,103,548,5,6,150,151,104,105,104,105,616,104,105,103,
725,103,600,699,624,104,105,104,105,629,103,793,632,104,105,576,103,637,103,164,
104,105,167,168,120,620,104,105,668,22,23,121,177,178,38,104,105,104,105,119,185,
186,101,188,104,105,104,105,104,105,22,23,104,105,100,89,668,104,105,699,205,101,
96,104,105,99,686,100,556,103,104,105,560,561,104,105,110,101,112,101,114,115,
116,104,105,89,90,699,92,93,349,350,96,756,644,645,103,756,103,103,715,103,106,
107,355,356,89,668,357,358,114,99,99,103,118,119,110,715,738,103,101,89,785,735,
102,104,785,104,110,105,101,117,795,113,622,101,795,101,699,284,75,103,99,104,30,
108,105,810,103,99,813,810,110,42,813,94,103,117,94,104,103,105,650,651,104,653,
94,655,105,89,104,659,104,104,735,113,105,104,101,105,105,793,104,104,646,103,
105,649,104,101,100,99,99,104,109,683,99,101,343,108,345,346,347,348,349,350,351,
352,353,354,355,356,357,358,359,360,361,104,113,104,104,709,113,100,712,713,108,
25,104,104,101,103,793,76,77,78,79,80,81,82,83,84,85,100,99,707,94,89,710,105,
104,396,104,104,104,104,103,28,720,102,105,100,90,101,110,105,754,25,26,25,758,
101,99,105,103,763,110,101,740,104,38,769,40,745,746,43,90,749,750,104,752,104,
104,51,52,104,784,55,56,57,58,0,0,591,62,325,64,401,66,67,548,216,60,546,776,760,
583,779,38,781,782,121,467,224,579,147,600,469,343,345,348,89,90,91,92,93,346,
307,96,801,54,347,100,805,- 1,103,- 1,- 1,- 1,107,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,117,
118,119,- 1,- 1,- 1,510,124,125,- 1,89,90,- 1,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,
- 1,106,107,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,118,119,- 1,- 1,547,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,
38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,- 1,64,
- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,625,- 1,- 1,- 1,- 1,- 1,- 1,- 1,633,89,90,
91,92,93,94,95,96,97,- 1,99,100,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,114,- 1,
- 1,117,118,119,- 1,- 1,- 1,- 1,124,125,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,
- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,97,- 1,99,
100,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,- 1,- 1,- 1,
124,125,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,
29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,
56,57,58,59,60,61,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,97,- 1,99,100,101,- 1,103,- 1,- 1,106,107,
- 1,- 1,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,- 1,- 1,- 1,124,125,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,
39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,- 1,64,- 1,
66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,
93,94,95,96,97,- 1,99,100,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,114,- 1,- 1,117,
118,119,- 1,- 1,- 1,- 1,124,125,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,43,44,- 1,- 1,- 1,48,49,
- 1,51,52,- 1,- 1,55,56,57,58,59,60,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,97,- 1,99,- 1,- 1,- 1,
103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,- 1,- 1,- 1,124,125,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,- 1,62,
- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,
90,91,92,93,94,95,96,97,- 1,- 1,100,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,114,
- 1,- 1,117,118,119,- 1,- 1,- 1,- 1,124,125,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,
- 1,51,52,- 1,- 1,55,56,57,58,59,60,- 1,62,- 1,64,- 1,66,67,- 1,- 1,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,97,- 1,37,- 1,- 1,- 1,
103,- 1,- 1,106,107,- 1,- 1,48,49,- 1,- 1,114,- 1,- 1,117,118,119,- 1,59,60,- 1,124,125,0,
1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,86,- 1,24,89,- 1,- 1,- 1,- 1,
94,95,- 1,97,- 1,99,- 1,37,- 1,39,- 1,- 1,106,107,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,96,97,- 1,- 1,- 1,101,- 1,103,- 1,- 1,106,107,-
1,- 1,- 1,- 1,- 1,- 1,114,115,116,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,
47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,96,97,- 1,- 1,- 1,
- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,1,- 1,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,
39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,1,- 1,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,
95,96,97,37,- 1,- 1,101,- 1,103,- 1,44,106,107,- 1,48,49,- 1,- 1,- 1,114,115,116,- 1,- 1,-
1,59,60,- 1,1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,
- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,96,97,37,- 1,- 1,- 1,- 1,103,- 1,- 1,106,107,- 1,48,49,- 1,
- 1,- 1,114,115,116,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,96,97,- 1,99,- 1,- 1,- 1,
103,104,- 1,106,107,- 1,- 1,110,- 1,112,- 1,114,115,116,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,89,90,- 1,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,37,- 1,106,107,- 1,- 1,- 1,
- 1,- 1,- 1,114,48,49,- 1,118,119,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,- 1,97,37,
99,39,- 1,- 1,103,- 1,44,106,107,- 1,48,49,- 1,112,- 1,114,115,116,- 1,- 1,- 1,59,60,- 1,-
1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,-
1,- 1,- 1,94,95,- 1,97,- 1,- 1,100,37,102,39,- 1,- 1,106,107,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,94,95,- 1,97,- 1,- 1,100,37,- 1,39,- 1,- 1,106,107,
44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,- 1,97,37,- 1,
- 1,- 1,- 1,- 1,- 1,44,106,107,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,94,95,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,25,26,27,- 1,29,30,31,32,33,34,
35,36,- 1,38,- 1,40,41,- 1,43,- 1,45,- 1,- 1,- 1,- 1,50,51,52,53,54,55,56,57,58,- 1,- 1,61,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,- 1,96,- 1,- 1,99,100,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,
114,32,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,
56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,101,- 1,103,- 1,- 1,- 1,107,
25,26,- 1,111,112,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,101,
- 1,103,- 1,- 1,- 1,107,25,26,- 1,111,112,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,
125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,
- 1,96,- 1,- 1,- 1,100,101,- 1,103,- 1,- 1,- 1,107,25,26,- 1,111,112,- 1,114,- 1,- 1,117,118,
119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,113,
114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,
56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,- 1,- 1,103,- 1,- 1,- 1,107,25,
26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,99,- 1,- 1,- 1,103,-
1,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,
- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,113,114,- 1,- 1,117,118,119,- 1,38,- 1,40,
124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,
67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,
- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,
119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,
114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,
56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,
26,- 1,- 1,- 1,113,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,-
1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,99,- 1,- 1,- 1,
103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,
- 1,99,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,
40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,
66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,
93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,
118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,-
1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,
- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,
55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,99,- 1,- 1,- 1,103,- 1,- 1,- 1,107,
25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,
103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,
- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,-
1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,-
1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,
92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,
117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,
- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,
- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,
- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,25,26,- 1,103,- 1,- 1,- 1,
107,- 1,- 1,- 1,- 1,- 1,38,114,- 1,- 1,117,118,119,- 1,- 1,- 1,- 1,124,125,51,52,- 1,- 1,55,
56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,- 1,- 1,103,- 1,- 1,- 1,107,25,
26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,- 1,124,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,- 1,- 1,103,
- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,114,- 1,- 1,117,118,119,- 1,38,- 1,- 1,124,125,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,
- 1,25,26,- 1,103,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,38,114,- 1,- 1,117,118,119,- 1,- 1,- 1,- 1,
124,125,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,25,26,
- 1,103,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,38,114,- 1,- 1,117,118,119,- 1,- 1,- 1,- 1,124,125,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,37,- 1,- 1,103,-
1,- 1,- 1,107,- 1,- 1,- 1,48,49,- 1,114,- 1,- 1,117,118,119,- 1,- 1,59,60,124,125,63,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
37,94,95,- 1,97,- 1,- 1,100,- 1,- 1,- 1,48,49,106,107,37,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,
48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,94,95,- 1,97,- 1,99,- 1,86,- 1,- 1,- 1,- 1,106,107,- 1,94,95,- 1,97,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,106,107,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107};
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
short*)({unsigned int _tmp83E=10000;short*_tmp83F=(short*)_cycalloc_atomic(
_check_times(sizeof(short),_tmp83E));{unsigned int _tmp840=_tmp83E;unsigned int i;
for(i=0;i < _tmp840;i ++){_tmp83F[i]=(short)0;}};_tmp83F;});int yyvsp_offset;void**
yyvs=(void**)({unsigned int _tmp83B=10000;void**_tmp83C=(void**)_cycalloc(
_check_times(sizeof(void*),_tmp83B));{unsigned int _tmp83D=_tmp83B;unsigned int i;
for(i=0;i < _tmp83D;i ++){_tmp83C[i]=Cyc_yylval;}};_tmp83C;});int yylsp_offset;
struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmp838=10000;struct Cyc_Yyltype*
_tmp839=(struct Cyc_Yyltype*)_cycalloc_atomic(_check_times(sizeof(struct Cyc_Yyltype),
_tmp838));{unsigned int _tmp83A=_tmp838;unsigned int i;for(i=0;i < _tmp83A;i ++){
_tmp839[i]=Cyc_yynewloc();}};_tmp839;});int yystacksize=10000;void*yyval=Cyc_yylval;
int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;
yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(
10000,++ yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(
yystacksize >= 10000){Cyc_yyerror(_tag_arr("parser stack overflow",sizeof(
unsigned char),22));(int)_throw((void*)Cyc_Yystack_overflow);}}goto yybackup;
yybackup: yyn=(int)Cyc_yypact[yystate];if(yyn == - 32768){goto yydefault;}if(Cyc_yychar
== - 2){Cyc_yychar=Cyc_yylex();}if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{
yychar1=(Cyc_yychar > 0? Cyc_yychar <= 353: 0)?(int)Cyc_yytranslate[
_check_known_subscript_notnull(354,Cyc_yychar)]: 241;}yyn +=yychar1;if((yyn < 0? 1:
yyn > 5049)? 1: Cyc_yycheck[yyn]!= yychar1){goto yydefault;}yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){if(yyn == - 32768){goto yyerrlab;}yyn=- yyn;goto yyreduce;}else{if(yyn == 0){
goto yyerrlab;}}if(yyn == 827){return 0;}if(Cyc_yychar != 0){Cyc_yychar=- 2;}yyvs[
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
yyvsp_offset]);_tmp4AC->typs=0;_tmp4AC->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4AC->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4AC;});_tmp4AB;});
_tmp4AA;});break;case 123: _LL2EE: {struct Cyc_List_List*_tmp4AD=((struct Cyc_List_List*(*)(
struct _tuple8*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));_tmp4AE[0]=({
struct Cyc_TunionField_tok_struct _tmp4AF;_tmp4AF.tag=Cyc_TunionField_tok;_tmp4AF.f1=({
struct Cyc_Absyn_Tunionfield*_tmp4B0=_cycalloc(sizeof(*_tmp4B0));_tmp4B0->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp4B0->typs=_tmp4AD;_tmp4B0->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4B0->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp4B0;});_tmp4AF;});
_tmp4AE;});break;}case 124: _LL2EF: yyval=yyvs[yyvsp_offset];break;case 125: _LL2F0:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));
_tmp4B1[0]=({struct Cyc_Declarator_tok_struct _tmp4B2;_tmp4B2.tag=Cyc_Declarator_tok;
_tmp4B2.f1=({struct Cyc_Parse_Declarator*_tmp4B3=_cycalloc(sizeof(*_tmp4B3));
_tmp4B3->id=(Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]))->id;_tmp4B3->tms=Cyc_List_imp_append(
Cyc_yyget_TypeModifierList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]))->tms);_tmp4B3;});
_tmp4B2;});_tmp4B1;});break;case 126: _LL2F1: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4[0]=({struct Cyc_Declarator_tok_struct
_tmp4B5;_tmp4B5.tag=Cyc_Declarator_tok;_tmp4B5.f1=({struct Cyc_Parse_Declarator*
_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6->id=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp4B6->tms=0;_tmp4B6;});_tmp4B5;});_tmp4B4;});break;case 127:
_LL2F2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 128: _LL2F3: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4B7=_cycalloc(
sizeof(*_tmp4B7));_tmp4B7[0]=({struct Cyc_Declarator_tok_struct _tmp4B8;_tmp4B8.tag=
Cyc_Declarator_tok;_tmp4B8.f1=({struct Cyc_Parse_Declarator*_tmp4B9=_cycalloc(
sizeof(*_tmp4B9));_tmp4B9->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->id;_tmp4B9->tms=({
struct Cyc_List_List*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));_tmp4BA->hd=(void*)((
void*)0);_tmp4BA->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->tms;_tmp4BA;});_tmp4B9;});
_tmp4B8;});_tmp4B7;});break;case 129: _LL2F4: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB[0]=({struct Cyc_Declarator_tok_struct
_tmp4BC;_tmp4BC.tag=Cyc_Declarator_tok;_tmp4BC.f1=({struct Cyc_Parse_Declarator*
_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4BD->tms=({
struct Cyc_List_List*_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));
_tmp4BF[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp4C0;_tmp4C0.tag=0;_tmp4C0.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4C0;});_tmp4BF;}));_tmp4BE->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp4BE;});_tmp4BD;});
_tmp4BC;});_tmp4BB;});break;case 130: _LL2F5: {struct _tuple13 _tmp4C2;struct Cyc_List_List*
_tmp4C3;struct Cyc_Core_Opt*_tmp4C4;struct Cyc_Absyn_VarargInfo*_tmp4C5;int _tmp4C6;
struct Cyc_List_List*_tmp4C7;struct _tuple13*_tmp4C1=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4C2=*_tmp4C1;_LL2FC:
_tmp4C7=_tmp4C2.f1;goto _LL2FB;_LL2FB: _tmp4C6=_tmp4C2.f2;goto _LL2FA;_LL2FA:
_tmp4C5=_tmp4C2.f3;goto _LL2F9;_LL2F9: _tmp4C4=_tmp4C2.f4;goto _LL2F8;_LL2F8:
_tmp4C3=_tmp4C2.f5;goto _LL2F7;_LL2F7: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4C8=_cycalloc(sizeof(*_tmp4C8));_tmp4C8[0]=({struct Cyc_Declarator_tok_struct
_tmp4C9;_tmp4C9.tag=Cyc_Declarator_tok;_tmp4C9.f1=({struct Cyc_Parse_Declarator*
_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4CA->tms=({
struct Cyc_List_List*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4CC=_cycalloc(sizeof(*_tmp4CC));
_tmp4CC[0]=({struct Cyc_Absyn_Function_mod_struct _tmp4CD;_tmp4CD.tag=2;_tmp4CD.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp4CE=_cycalloc(sizeof(*
_tmp4CE));_tmp4CE[0]=({struct Cyc_Absyn_WithTypes_struct _tmp4CF;_tmp4CF.tag=1;
_tmp4CF.f1=_tmp4C7;_tmp4CF.f2=_tmp4C6;_tmp4CF.f3=_tmp4C5;_tmp4CF.f4=_tmp4C4;
_tmp4CF.f5=_tmp4C3;_tmp4CF;});_tmp4CE;}));_tmp4CD;});_tmp4CC;}));_tmp4CB->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp4CB;});_tmp4CA;});_tmp4C9;});_tmp4C8;});break;}case 131: _LL2F6:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4D0=_cycalloc(sizeof(*_tmp4D0));
_tmp4D0[0]=({struct Cyc_Declarator_tok_struct _tmp4D1;_tmp4D1.tag=Cyc_Declarator_tok;
_tmp4D1.f1=({struct Cyc_Parse_Declarator*_tmp4D2=_cycalloc(sizeof(*_tmp4D2));
_tmp4D2->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp4D2->tms=({struct Cyc_List_List*_tmp4D3=_cycalloc(
sizeof(*_tmp4D3));_tmp4D3->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp4D4=_cycalloc(sizeof(*_tmp4D4));_tmp4D4[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp4D5;_tmp4D5.tag=2;_tmp4D5.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp4D6=_cycalloc(sizeof(*_tmp4D6));_tmp4D6[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp4D7;_tmp4D7.tag=1;_tmp4D7.f1=0;_tmp4D7.f2=0;_tmp4D7.f3=0;_tmp4D7.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D7.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4D7;});_tmp4D6;}));
_tmp4D5;});_tmp4D4;}));_tmp4D3->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp4D3;});_tmp4D2;});
_tmp4D1;});_tmp4D0;});break;case 132: _LL2FD: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4D8=_cycalloc(sizeof(*_tmp4D8));_tmp4D8[0]=({struct Cyc_Declarator_tok_struct
_tmp4D9;_tmp4D9.tag=Cyc_Declarator_tok;_tmp4D9.f1=({struct Cyc_Parse_Declarator*
_tmp4DA=_cycalloc(sizeof(*_tmp4DA));_tmp4DA->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4DA->tms=({
struct Cyc_List_List*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));
_tmp4DC[0]=({struct Cyc_Absyn_Function_mod_struct _tmp4DD;_tmp4DD.tag=2;_tmp4DD.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));
_tmp4DE[0]=({struct Cyc_Absyn_NoTypes_struct _tmp4DF;_tmp4DF.tag=0;_tmp4DF.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4DF.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4DF;});_tmp4DE;}));_tmp4DD;});_tmp4DC;}));_tmp4DB->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp4DB;});_tmp4DA;});_tmp4D9;});_tmp4D8;});break;case 133: _LL2FE: {
struct Cyc_List_List*_tmp4E0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1[0]=({
struct Cyc_Declarator_tok_struct _tmp4E2;_tmp4E2.tag=Cyc_Declarator_tok;_tmp4E2.f1=({
struct Cyc_Parse_Declarator*_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4E3->tms=({
struct Cyc_List_List*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp4E5=_cycalloc(sizeof(*_tmp4E5));
_tmp4E5[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp4E6;_tmp4E6.tag=3;_tmp4E6.f1=
_tmp4E0;_tmp4E6.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4E6.f3=0;_tmp4E6;});_tmp4E5;}));_tmp4E4->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp4E4;});
_tmp4E3;});_tmp4E2;});_tmp4E1;});break;}case 134: _LL2FF: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7[0]=({struct Cyc_Declarator_tok_struct
_tmp4E8;_tmp4E8.tag=Cyc_Declarator_tok;_tmp4E8.f1=({struct Cyc_Parse_Declarator*
_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp4E9->tms=({
struct Cyc_List_List*_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp4EB=_cycalloc(sizeof(*_tmp4EB));
_tmp4EB[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp4EC;_tmp4EC.tag=4;_tmp4EC.f1=
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);
_tmp4EC.f2=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp4EC;});_tmp4EB;}));
_tmp4EA->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp4EA;});_tmp4E9;});_tmp4E8;});_tmp4E7;});break;case
135: _LL300: Cyc_Parse_err(_tag_arr("identifier has not been declared as a typedef",
sizeof(unsigned char),46),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(void*)({
struct Cyc_Declarator_tok_struct*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED[0]=({
struct Cyc_Declarator_tok_struct _tmp4EE;_tmp4EE.tag=Cyc_Declarator_tok;_tmp4EE.f1=({
struct Cyc_Parse_Declarator*_tmp4EF=_cycalloc(sizeof(*_tmp4EF));_tmp4EF->id=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp4EF->tms=0;_tmp4EF;});_tmp4EE;});_tmp4ED;});break;case
136: _LL301: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4F0=_cycalloc(
sizeof(*_tmp4F0));_tmp4F0[0]=({struct Cyc_Declarator_tok_struct _tmp4F1;_tmp4F1.tag=
Cyc_Declarator_tok;_tmp4F1.f1=({struct Cyc_Parse_Declarator*_tmp4F2=_cycalloc(
sizeof(*_tmp4F2));_tmp4F2->id=Cyc_Absyn_exn_name;_tmp4F2->tms=0;_tmp4F2;});
_tmp4F1;});_tmp4F0;});break;case 137: _LL302: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp4F4;_tmp4F4.tag=Cyc_TypeModifierList_tok;_tmp4F4.f1=({struct Cyc_List_List*
_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp4F7;_tmp4F7.tag=1;_tmp4F7.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4F7.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4F7.f3=Cyc_Absyn_empty_tqual();
_tmp4F7;});_tmp4F6;}));_tmp4F5->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]),0);_tmp4F5;});_tmp4F4;});_tmp4F3;});break;case 138: _LL303:
yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*_tmp4F8=_cycalloc(sizeof(*
_tmp4F8));_tmp4F8[0]=({struct Cyc_TypeModifierList_tok_struct _tmp4F9;_tmp4F9.tag=
Cyc_TypeModifierList_tok;_tmp4F9.f1=({struct Cyc_List_List*_tmp4FA=_cycalloc(
sizeof(*_tmp4FA));_tmp4FA->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp4FC;_tmp4FC.tag=1;_tmp4FC.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4FC.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4FC.f3=Cyc_yyget_TypeQual_tok(
yyvs[yyvsp_offset]);_tmp4FC;});_tmp4FB;}));_tmp4FA->tl=Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0);_tmp4FA;});_tmp4F9;});
_tmp4F8;});break;case 139: _LL304: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp4FE;_tmp4FE.tag=Cyc_TypeModifierList_tok;_tmp4FE.f1=({struct Cyc_List_List*
_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp501;_tmp501.tag=1;_tmp501.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp501.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp501.f3=Cyc_Absyn_empty_tqual();
_tmp501;});_tmp500;}));_tmp4FF->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp4FF;});_tmp4FE;});_tmp4FD;});break;case 140: _LL305: yyval=(
void*)({struct Cyc_TypeModifierList_tok_struct*_tmp502=_cycalloc(sizeof(*_tmp502));
_tmp502[0]=({struct Cyc_TypeModifierList_tok_struct _tmp503;_tmp503.tag=Cyc_TypeModifierList_tok;
_tmp503.f1=({struct Cyc_List_List*_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504->hd=(
void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp505=_cycalloc(sizeof(*
_tmp505));_tmp505[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp506;_tmp506.tag=1;
_tmp506.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp506.f2=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp506.f3=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp506;});_tmp505;}));
_tmp504->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp504;});_tmp503;});_tmp502;});break;case 141: _LL306: yyval=(
void*)({struct Cyc_Pointer_Sort_tok_struct*_tmp507=_cycalloc(sizeof(*_tmp507));
_tmp507[0]=({struct Cyc_Pointer_Sort_tok_struct _tmp508;_tmp508.tag=Cyc_Pointer_Sort_tok;
_tmp508.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*_tmp509=_cycalloc(
sizeof(*_tmp509));_tmp509[0]=({struct Cyc_Absyn_Nullable_ps_struct _tmp50A;_tmp50A.tag=
1;_tmp50A.f1=Cyc_Absyn_exp_unsigned_one;_tmp50A;});_tmp509;}));_tmp508;});
_tmp507;});break;case 142: _LL307: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp50C;_tmp50C.tag=Cyc_Pointer_Sort_tok;_tmp50C.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp50E;_tmp50E.tag=0;_tmp50E.f1=Cyc_Absyn_exp_unsigned_one;_tmp50E;});_tmp50D;}));
_tmp50C;});_tmp50B;});break;case 143: _LL308: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp50F=_cycalloc(sizeof(*_tmp50F));_tmp50F[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp510;_tmp510.tag=Cyc_Pointer_Sort_tok;_tmp510.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*
_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp512;_tmp512.tag=1;_tmp512.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp512;});_tmp511;}));
_tmp510;});_tmp50F;});break;case 144: _LL309: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp513=_cycalloc(sizeof(*_tmp513));_tmp513[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp514;_tmp514.tag=Cyc_Pointer_Sort_tok;_tmp514.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp516;_tmp516.tag=0;_tmp516.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp516;});_tmp515;}));
_tmp514;});_tmp513;});break;case 145: _LL30A: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp517=_cycalloc(sizeof(*_tmp517));_tmp517[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp518;_tmp518.tag=Cyc_Pointer_Sort_tok;_tmp518.f1=(void*)((void*)0);_tmp518;});
_tmp517;});break;case 146: _LL30B: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp519=
_cycalloc(sizeof(*_tmp519));_tmp519[0]=({struct Cyc_Type_tok_struct _tmp51A;
_tmp51A.tag=Cyc_Type_tok;_tmp51A.f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp51B=_cycalloc(sizeof(*_tmp51B));_tmp51B->v=(void*)((void*)3);_tmp51B;}),0);
_tmp51A;});_tmp519;});break;case 147: _LL30C: yyval=yyvs[yyvsp_offset];break;case
148: _LL30D: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(
void*)3);yyval=yyvs[yyvsp_offset];break;case 149: _LL30E: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp51C=_cycalloc(sizeof(*_tmp51C));_tmp51C[0]=({struct Cyc_Type_tok_struct
_tmp51D;_tmp51D.tag=Cyc_Type_tok;_tmp51D.f1=(void*)Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E->v=(void*)((void*)3);
_tmp51E;}),0);_tmp51D;});_tmp51C;});break;case 150: _LL30F: yyval=yyvs[yyvsp_offset];
break;case 151: _LL310: yyval=(void*)({struct Cyc_TypeQual_tok_struct*_tmp51F=
_cycalloc_atomic(sizeof(*_tmp51F));_tmp51F[0]=({struct Cyc_TypeQual_tok_struct
_tmp520;_tmp520.tag=Cyc_TypeQual_tok;_tmp520.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[yyvsp_offset]));_tmp520;});_tmp51F;});break;case 152: _LL311: yyval=(void*)({
struct Cyc_YY1_struct*_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521[0]=({struct Cyc_YY1_struct
_tmp522;_tmp522.tag=Cyc_YY1;_tmp522.f1=({struct _tuple13*_tmp523=_cycalloc(
sizeof(*_tmp523));_tmp523->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp523->f2=0;_tmp523->f3=
0;_tmp523->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp523->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp523;});_tmp522;});_tmp521;});break;case 153: _LL312: yyval=(void*)({struct Cyc_YY1_struct*
_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524[0]=({struct Cyc_YY1_struct _tmp525;
_tmp525.tag=Cyc_YY1;_tmp525.f1=({struct _tuple13*_tmp526=_cycalloc(sizeof(*
_tmp526));_tmp526->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]));_tmp526->f2=1;_tmp526->f3=0;_tmp526->f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp526->f5=Cyc_yyget_Rgnorder_tok(
yyvs[yyvsp_offset]);_tmp526;});_tmp525;});_tmp524;});break;case 154: _LL313: {
struct _tuple2 _tmp528;void*_tmp529;struct Cyc_Absyn_Tqual _tmp52A;struct Cyc_Core_Opt*
_tmp52B;struct _tuple2*_tmp527=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp528=*_tmp527;_LL318:
_tmp52B=_tmp528.f1;goto _LL317;_LL317: _tmp52A=_tmp528.f2;goto _LL316;_LL316:
_tmp529=_tmp528.f3;goto _LL315;_LL315: {struct Cyc_Absyn_VarargInfo*_tmp52C=({
struct Cyc_Absyn_VarargInfo*_tmp530=_cycalloc(sizeof(*_tmp530));_tmp530->name=
_tmp52B;_tmp530->tq=_tmp52A;_tmp530->type=(void*)_tmp529;_tmp530->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp530;});yyval=(
void*)({struct Cyc_YY1_struct*_tmp52D=_cycalloc(sizeof(*_tmp52D));_tmp52D[0]=({
struct Cyc_YY1_struct _tmp52E;_tmp52E.tag=Cyc_YY1;_tmp52E.f1=({struct _tuple13*
_tmp52F=_cycalloc(sizeof(*_tmp52F));_tmp52F->f1=0;_tmp52F->f2=0;_tmp52F->f3=
_tmp52C;_tmp52F->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp52F->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp52F;});_tmp52E;});_tmp52D;});break;}}case 155: _LL314: {struct _tuple2 _tmp532;
void*_tmp533;struct Cyc_Absyn_Tqual _tmp534;struct Cyc_Core_Opt*_tmp535;struct
_tuple2*_tmp531=Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp532=*_tmp531;_LL31D: _tmp535=_tmp532.f1;goto _LL31C;
_LL31C: _tmp534=_tmp532.f2;goto _LL31B;_LL31B: _tmp533=_tmp532.f3;goto _LL31A;_LL31A: {
struct Cyc_Absyn_VarargInfo*_tmp536=({struct Cyc_Absyn_VarargInfo*_tmp53A=
_cycalloc(sizeof(*_tmp53A));_tmp53A->name=_tmp535;_tmp53A->tq=_tmp534;_tmp53A->type=(
void*)_tmp533;_tmp53A->inject=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp53A;});yyval=(void*)({
struct Cyc_YY1_struct*_tmp537=_cycalloc(sizeof(*_tmp537));_tmp537[0]=({struct Cyc_YY1_struct
_tmp538;_tmp538.tag=Cyc_YY1;_tmp538.f1=({struct _tuple13*_tmp539=_cycalloc(
sizeof(*_tmp539));_tmp539->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp539->f2=0;_tmp539->f3=
_tmp536;_tmp539->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp539->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp539;});_tmp538;});_tmp537;});break;}}case 156: _LL319: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp53B=_cycalloc(sizeof(*_tmp53B));_tmp53B[0]=({struct Cyc_Type_tok_struct
_tmp53C;_tmp53C.tag=Cyc_Type_tok;_tmp53C.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp53D=_cycalloc(
sizeof(*_tmp53D));_tmp53D[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp53E;_tmp53E.tag=
1;_tmp53E.f1=0;_tmp53E;});_tmp53D;}));_tmp53C;});_tmp53B;});break;case 157: _LL31E:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp53F=_cycalloc(sizeof(*_tmp53F));
_tmp53F[0]=({struct Cyc_Type_tok_struct _tmp540;_tmp540.tag=Cyc_Type_tok;_tmp540.f1=(
void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp541=_cycalloc(
sizeof(*_tmp541));_tmp541[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp542;_tmp542.tag=0;
_tmp542.f1=(void*)Cyc_yyget_Kind_tok(yyvs[yyvsp_offset]);_tmp542;});_tmp541;}));
_tmp540;});_tmp53F;});break;case 158: _LL31F: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp543=_cycalloc(sizeof(*_tmp543));_tmp543[0]=({struct Cyc_TypeOpt_tok_struct
_tmp544;_tmp544.tag=Cyc_TypeOpt_tok;_tmp544.f1=0;_tmp544;});_tmp543;});break;
case 159: _LL320: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp545=_cycalloc(
sizeof(*_tmp545));_tmp545[0]=({struct Cyc_TypeOpt_tok_struct _tmp546;_tmp546.tag=
Cyc_TypeOpt_tok;_tmp546.f1=({struct Cyc_Core_Opt*_tmp547=_cycalloc(sizeof(*
_tmp547));_tmp547->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp548=
_cycalloc(sizeof(*_tmp548));_tmp548[0]=({struct Cyc_Absyn_JoinEff_struct _tmp549;
_tmp549.tag=18;_tmp549.f1=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp549;});
_tmp548;}));_tmp547;});_tmp546;});_tmp545;});break;case 160: _LL321: yyval=(void*)({
struct Cyc_Rgnorder_tok_struct*_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A[0]=({
struct Cyc_Rgnorder_tok_struct _tmp54B;_tmp54B.tag=Cyc_Rgnorder_tok;_tmp54B.f1=0;
_tmp54B;});_tmp54A;});break;case 161: _LL322: yyval=yyvs[yyvsp_offset];break;case
162: _LL323: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp54C=_cycalloc(
sizeof(*_tmp54C));_tmp54C[0]=({struct Cyc_Rgnorder_tok_struct _tmp54D;_tmp54D.tag=
Cyc_Rgnorder_tok;_tmp54D.f1=({struct Cyc_List_List*_tmp54E=_cycalloc(sizeof(*
_tmp54E));_tmp54E->hd=({struct _tuple16*_tmp54F=_cycalloc(sizeof(*_tmp54F));
_tmp54F->f1=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp553;_tmp553.tag=0;_tmp553.f1=(void*)((void*)3);_tmp553;});_tmp552;}));
_tmp54F->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),(void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp551;_tmp551.tag=0;_tmp551.f1=(void*)((void*)3);
_tmp551;});_tmp550;}));_tmp54F;});_tmp54E->tl=0;_tmp54E;});_tmp54D;});_tmp54C;});
break;case 163: _LL324: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp554=
_cycalloc(sizeof(*_tmp554));_tmp554[0]=({struct Cyc_Rgnorder_tok_struct _tmp555;
_tmp555.tag=Cyc_Rgnorder_tok;_tmp555.f1=({struct Cyc_List_List*_tmp556=_cycalloc(
sizeof(*_tmp556));_tmp556->hd=({struct _tuple16*_tmp557=_cycalloc(sizeof(*_tmp557));
_tmp557->f1=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp55B;_tmp55B.tag=0;_tmp55B.f1=(void*)((void*)3);_tmp55B;});_tmp55A;}));
_tmp557->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp559;_tmp559.tag=0;_tmp559.f1=(void*)((void*)3);_tmp559;});_tmp558;}));
_tmp557;});_tmp556->tl=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);_tmp556;});
_tmp555;});_tmp554;});break;case 164: _LL325: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp55C=_cycalloc_atomic(sizeof(*_tmp55C));_tmp55C[0]=({struct Cyc_Bool_tok_struct
_tmp55D;_tmp55D.tag=Cyc_Bool_tok;_tmp55D.f1=0;_tmp55D;});_tmp55C;});break;case
165: _LL326: if(Cyc_Std_zstrcmp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),_tag_arr("inject",
sizeof(unsigned char),7))!= 0){Cyc_Parse_err(_tag_arr("missing type in function declaration",
sizeof(unsigned char),37),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp55E=_cycalloc_atomic(sizeof(*_tmp55E));_tmp55E[0]=({struct Cyc_Bool_tok_struct
_tmp55F;_tmp55F.tag=Cyc_Bool_tok;_tmp55F.f1=1;_tmp55F;});_tmp55E;});break;case
166: _LL327: yyval=yyvs[yyvsp_offset];break;case 167: _LL328: yyval=(void*)({struct
Cyc_TypeList_tok_struct*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560[0]=({struct
Cyc_TypeList_tok_struct _tmp561;_tmp561.tag=Cyc_TypeList_tok;_tmp561.f1=Cyc_List_imp_append(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]));_tmp561;});_tmp560;});break;case 168:
_LL329: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp562=_cycalloc(sizeof(*
_tmp562));_tmp562[0]=({struct Cyc_TypeList_tok_struct _tmp563;_tmp563.tag=Cyc_TypeList_tok;
_tmp563.f1=0;_tmp563;});_tmp562;});break;case 169: _LL32A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 170: _LL32B:
yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp564=_cycalloc(sizeof(*_tmp564));
_tmp564[0]=({struct Cyc_TypeList_tok_struct _tmp565;_tmp565.tag=Cyc_TypeList_tok;
_tmp565.f1=({struct Cyc_List_List*_tmp566=_cycalloc(sizeof(*_tmp566));_tmp566->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp567=_cycalloc(sizeof(*_tmp567));
_tmp567[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp568;_tmp568.tag=19;_tmp568.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp568;});_tmp567;}));_tmp566->tl=0;_tmp566;});_tmp565;});_tmp564;});
break;case 171: _LL32C: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
yyvsp_offset]),(void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp569=
_cycalloc(sizeof(*_tmp569));_tmp569[0]=({struct Cyc_TypeList_tok_struct _tmp56A;
_tmp56A.tag=Cyc_TypeList_tok;_tmp56A.f1=({struct Cyc_List_List*_tmp56B=_cycalloc(
sizeof(*_tmp56B));_tmp56B->hd=(void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);
_tmp56B->tl=0;_tmp56B;});_tmp56A;});_tmp569;});break;case 172: _LL32D: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp56C=_cycalloc(sizeof(*_tmp56C));_tmp56C[0]=({struct Cyc_TypeList_tok_struct
_tmp56D;_tmp56D.tag=Cyc_TypeList_tok;_tmp56D.f1=({struct Cyc_List_List*_tmp56E=
_cycalloc(sizeof(*_tmp56E));_tmp56E->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp570;_tmp570.tag=17;_tmp570.f1=(void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);
_tmp570;});_tmp56F;}));_tmp56E->tl=0;_tmp56E;});_tmp56D;});_tmp56C;});break;case
173: _LL32E: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(void*)({
struct Cyc_TypeList_tok_struct*_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571[0]=({
struct Cyc_TypeList_tok_struct _tmp572;_tmp572.tag=Cyc_TypeList_tok;_tmp572.f1=({
struct Cyc_List_List*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp574=_cycalloc(sizeof(*_tmp574));
_tmp574[0]=({struct Cyc_Absyn_AccessEff_struct _tmp575;_tmp575.tag=17;_tmp575.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp575;});_tmp574;}));_tmp573->tl=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);
_tmp573;});_tmp572;});_tmp571;});break;case 174: _LL32F: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp577;_tmp577.tag=Cyc_ParamDeclList_tok;_tmp577.f1=({struct Cyc_List_List*
_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578->hd=Cyc_yyget_ParamDecl_tok(yyvs[
yyvsp_offset]);_tmp578->tl=0;_tmp578;});_tmp577;});_tmp576;});break;case 175:
_LL330: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*_tmp579=_cycalloc(
sizeof(*_tmp579));_tmp579[0]=({struct Cyc_ParamDeclList_tok_struct _tmp57A;_tmp57A.tag=
Cyc_ParamDeclList_tok;_tmp57A.f1=({struct Cyc_List_List*_tmp57B=_cycalloc(sizeof(*
_tmp57B));_tmp57B->hd=Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset]);_tmp57B->tl=
Cyc_yyget_ParamDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp57B;});_tmp57A;});_tmp579;});break;case 176: _LL331: {
struct _tuple14 _tmp57D;struct Cyc_List_List*_tmp57E;struct Cyc_List_List*_tmp57F;
struct Cyc_Absyn_Tqual _tmp580;struct _tuple14*_tmp57C=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp57D=*_tmp57C;
_LL336: _tmp580=_tmp57D.f1;goto _LL335;_LL335: _tmp57F=_tmp57D.f2;goto _LL334;_LL334:
_tmp57E=_tmp57D.f3;goto _LL333;_LL333: {struct Cyc_Parse_Declarator _tmp582;struct
Cyc_List_List*_tmp583;struct _tuple1*_tmp584;struct Cyc_Parse_Declarator*_tmp581=
Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]);_tmp582=*_tmp581;_LL339: _tmp584=
_tmp582.id;goto _LL338;_LL338: _tmp583=_tmp582.tms;goto _LL337;_LL337: {void*
_tmp585=Cyc_Parse_speclist2typ(_tmp57F,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp587;struct Cyc_List_List*_tmp588;void*_tmp589;struct Cyc_Absyn_Tqual _tmp58A;
struct _tuple5 _tmp586=Cyc_Parse_apply_tms(_tmp580,_tmp585,_tmp57E,_tmp583);_LL33E:
_tmp58A=_tmp586.f1;goto _LL33D;_LL33D: _tmp589=_tmp586.f2;goto _LL33C;_LL33C:
_tmp588=_tmp586.f3;goto _LL33B;_LL33B: _tmp587=_tmp586.f4;goto _LL33A;_LL33A: if(
_tmp588 != 0){Cyc_Parse_err(_tag_arr("parameter with bad type params",sizeof(
unsigned char),31),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp584)){Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a namespace",sizeof(unsigned char),
47),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*_tmp58B=(struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt*_tmp590=_cycalloc(sizeof(*_tmp590));_tmp590->v=(*_tmp584).f2;
_tmp590;});if(_tmp587 != 0){({void*_tmp58C[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("extra attributes on parameter, ignoring",
sizeof(unsigned char),40),_tag_arr(_tmp58C,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp58D=_cycalloc(sizeof(*_tmp58D));_tmp58D[0]=({
struct Cyc_ParamDecl_tok_struct _tmp58E;_tmp58E.tag=Cyc_ParamDecl_tok;_tmp58E.f1=({
struct _tuple2*_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F->f1=_tmp58B;_tmp58F->f2=
_tmp58A;_tmp58F->f3=_tmp589;_tmp58F;});_tmp58E;});_tmp58D;});break;}}}}case 177:
_LL332: {struct _tuple14 _tmp592;struct Cyc_List_List*_tmp593;struct Cyc_List_List*
_tmp594;struct Cyc_Absyn_Tqual _tmp595;struct _tuple14*_tmp591=Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset]);_tmp592=*_tmp591;_LL343: _tmp595=_tmp592.f1;goto _LL342;_LL342:
_tmp594=_tmp592.f2;goto _LL341;_LL341: _tmp593=_tmp592.f3;goto _LL340;_LL340: {void*
_tmp596=Cyc_Parse_speclist2typ(_tmp594,Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));if(_tmp593 != 0){({
void*_tmp597[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(unsigned char),38),_tag_arr(_tmp597,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598[0]=({
struct Cyc_ParamDecl_tok_struct _tmp599;_tmp599.tag=Cyc_ParamDecl_tok;_tmp599.f1=({
struct _tuple2*_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A->f1=0;_tmp59A->f2=
_tmp595;_tmp59A->f3=_tmp596;_tmp59A;});_tmp599;});_tmp598;});break;}}case 178:
_LL33F: {struct _tuple14 _tmp59C;struct Cyc_List_List*_tmp59D;struct Cyc_List_List*
_tmp59E;struct Cyc_Absyn_Tqual _tmp59F;struct _tuple14*_tmp59B=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp59C=*_tmp59B;
_LL348: _tmp59F=_tmp59C.f1;goto _LL347;_LL347: _tmp59E=_tmp59C.f2;goto _LL346;_LL346:
_tmp59D=_tmp59C.f3;goto _LL345;_LL345: {void*_tmp5A0=Cyc_Parse_speclist2typ(
_tmp59E,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp5A1=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[yyvsp_offset]))->tms;struct Cyc_List_List*_tmp5A3;struct Cyc_List_List*
_tmp5A4;void*_tmp5A5;struct Cyc_Absyn_Tqual _tmp5A6;struct _tuple5 _tmp5A2=Cyc_Parse_apply_tms(
_tmp59F,_tmp5A0,_tmp59D,_tmp5A1);_LL34D: _tmp5A6=_tmp5A2.f1;goto _LL34C;_LL34C:
_tmp5A5=_tmp5A2.f2;goto _LL34B;_LL34B: _tmp5A4=_tmp5A2.f3;goto _LL34A;_LL34A:
_tmp5A3=_tmp5A2.f4;goto _LL349;_LL349: if(_tmp5A4 != 0){({void*_tmp5A7[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad type parameters on formal argument, ignoring",
sizeof(unsigned char),49),_tag_arr(_tmp5A7,sizeof(void*),0));});}if(_tmp5A3 != 0){({
void*_tmp5A8[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(unsigned char),38),_tag_arr(_tmp5A8,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5A9=_cycalloc(sizeof(*_tmp5A9));_tmp5A9[0]=({
struct Cyc_ParamDecl_tok_struct _tmp5AA;_tmp5AA.tag=Cyc_ParamDecl_tok;_tmp5AA.f1=({
struct _tuple2*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB->f1=0;_tmp5AB->f2=
_tmp5A6;_tmp5AB->f3=_tmp5A5;_tmp5AB;});_tmp5AA;});_tmp5A9;});break;}}case 179:
_LL344: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5AC=_cycalloc(sizeof(*
_tmp5AC));_tmp5AC[0]=({struct Cyc_IdList_tok_struct _tmp5AD;_tmp5AD.tag=Cyc_IdList_tok;
_tmp5AD.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(yyvs[yyvsp_offset]));_tmp5AD;});_tmp5AC;});break;case 180:
_LL34E: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5AE=_cycalloc(sizeof(*
_tmp5AE));_tmp5AE[0]=({struct Cyc_IdList_tok_struct _tmp5AF;_tmp5AF.tag=Cyc_IdList_tok;
_tmp5AF.f1=({struct Cyc_List_List*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->hd=({
struct _tagged_arr*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp5B1;});_tmp5B0->tl=0;_tmp5B0;});_tmp5AF;});_tmp5AE;});
break;case 181: _LL34F: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5B2=
_cycalloc(sizeof(*_tmp5B2));_tmp5B2[0]=({struct Cyc_IdList_tok_struct _tmp5B3;
_tmp5B3.tag=Cyc_IdList_tok;_tmp5B3.f1=({struct Cyc_List_List*_tmp5B4=_cycalloc(
sizeof(*_tmp5B4));_tmp5B4->hd=({struct _tagged_arr*_tmp5B5=_cycalloc(sizeof(*
_tmp5B5));_tmp5B5[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp5B5;});_tmp5B4->tl=
Cyc_yyget_IdList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5B4;});_tmp5B3;});_tmp5B2;});break;case 182: _LL350: yyval=yyvs[yyvsp_offset];
break;case 183: _LL351: yyval=yyvs[yyvsp_offset];break;case 184: _LL352: yyval=(void*)({
struct Cyc_Exp_tok_struct*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6[0]=({struct
Cyc_Exp_tok_struct _tmp5B7;_tmp5B7.tag=Cyc_Exp_tok;_tmp5B7.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));
_tmp5B8[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5B9;_tmp5B9.tag=34;
_tmp5B9.f1=0;_tmp5B9.f2=0;_tmp5B9;});_tmp5B8;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5B7;});_tmp5B6;});break;case 185: _LL353: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA[0]=({
struct Cyc_Exp_tok_struct _tmp5BB;_tmp5BB.tag=Cyc_Exp_tok;_tmp5BB.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));
_tmp5BC[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5BD;_tmp5BD.tag=34;
_tmp5BD.f1=0;_tmp5BD.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp5BD;});_tmp5BC;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5BB;});_tmp5BA;});break;case 186: _LL354: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE[0]=({
struct Cyc_Exp_tok_struct _tmp5BF;_tmp5BF.tag=Cyc_Exp_tok;_tmp5BF.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));
_tmp5C0[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5C1;_tmp5C1.tag=34;
_tmp5C1.f1=0;_tmp5C1.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp5C1;});_tmp5C0;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5BF;});_tmp5BE;});break;case 187: _LL355: {struct
Cyc_Absyn_Vardecl*_tmp5C2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp5C8=
_cycalloc(sizeof(*_tmp5C8));_tmp5C8->f1=(void*)0;_tmp5C8->f2=({struct _tagged_arr*
_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp5C9;});_tmp5C8;}),
Cyc_Absyn_uint_t,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));_tmp5C2->tq=({
struct Cyc_Absyn_Tqual _tmp5C3;_tmp5C3.q_const=1;_tmp5C3.q_volatile=0;_tmp5C3.q_restrict=
1;_tmp5C3;});yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp5C4=_cycalloc(sizeof(*
_tmp5C4));_tmp5C4[0]=({struct Cyc_Exp_tok_struct _tmp5C5;_tmp5C5.tag=Cyc_Exp_tok;
_tmp5C5.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp5C6=_cycalloc(sizeof(*_tmp5C6));_tmp5C6[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp5C7;_tmp5C7.tag=27;_tmp5C7.f1=_tmp5C2;_tmp5C7.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5C7.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5C7;});_tmp5C6;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmp5C5;});
_tmp5C4;});break;}case 188: _LL356: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp5CA=_cycalloc(sizeof(*_tmp5CA));_tmp5CA[0]=({struct Cyc_InitializerList_tok_struct
_tmp5CB;_tmp5CB.tag=Cyc_InitializerList_tok;_tmp5CB.f1=({struct Cyc_List_List*
_tmp5CC=_cycalloc(sizeof(*_tmp5CC));_tmp5CC->hd=({struct _tuple17*_tmp5CD=
_cycalloc(sizeof(*_tmp5CD));_tmp5CD->f1=0;_tmp5CD->f2=Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]);_tmp5CD;});_tmp5CC->tl=0;_tmp5CC;});_tmp5CB;});_tmp5CA;});break;
case 189: _LL357: yyval=(void*)({struct Cyc_InitializerList_tok_struct*_tmp5CE=
_cycalloc(sizeof(*_tmp5CE));_tmp5CE[0]=({struct Cyc_InitializerList_tok_struct
_tmp5CF;_tmp5CF.tag=Cyc_InitializerList_tok;_tmp5CF.f1=({struct Cyc_List_List*
_tmp5D0=_cycalloc(sizeof(*_tmp5D0));_tmp5D0->hd=({struct _tuple17*_tmp5D1=
_cycalloc(sizeof(*_tmp5D1));_tmp5D1->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D1->f2=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp5D1;});_tmp5D0->tl=0;_tmp5D0;});_tmp5CF;});_tmp5CE;});
break;case 190: _LL358: yyval=(void*)({struct Cyc_InitializerList_tok_struct*_tmp5D2=
_cycalloc(sizeof(*_tmp5D2));_tmp5D2[0]=({struct Cyc_InitializerList_tok_struct
_tmp5D3;_tmp5D3.tag=Cyc_InitializerList_tok;_tmp5D3.f1=({struct Cyc_List_List*
_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->hd=({struct _tuple17*_tmp5D5=
_cycalloc(sizeof(*_tmp5D5));_tmp5D5->f1=0;_tmp5D5->f2=Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]);_tmp5D5;});_tmp5D4->tl=Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D4;});_tmp5D3;});
_tmp5D2;});break;case 191: _LL359: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6[0]=({struct Cyc_InitializerList_tok_struct
_tmp5D7;_tmp5D7.tag=Cyc_InitializerList_tok;_tmp5D7.f1=({struct Cyc_List_List*
_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->hd=({struct _tuple17*_tmp5D9=
_cycalloc(sizeof(*_tmp5D9));_tmp5D9->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D9->f2=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp5D9;});_tmp5D8->tl=Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5D8;});_tmp5D7;});
_tmp5D6;});break;case 192: _LL35A: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA[0]=({struct Cyc_DesignatorList_tok_struct
_tmp5DB;_tmp5DB.tag=Cyc_DesignatorList_tok;_tmp5DB.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp5DB;});_tmp5DA;});
break;case 193: _LL35B: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp5DC=
_cycalloc(sizeof(*_tmp5DC));_tmp5DC[0]=({struct Cyc_DesignatorList_tok_struct
_tmp5DD;_tmp5DD.tag=Cyc_DesignatorList_tok;_tmp5DD.f1=({struct Cyc_List_List*
_tmp5DE=_cycalloc(sizeof(*_tmp5DE));_tmp5DE->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[yyvsp_offset]);_tmp5DE->tl=0;_tmp5DE;});_tmp5DD;});_tmp5DC;});break;case 194:
_LL35C: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp5DF=_cycalloc(
sizeof(*_tmp5DF));_tmp5DF[0]=({struct Cyc_DesignatorList_tok_struct _tmp5E0;
_tmp5E0.tag=Cyc_DesignatorList_tok;_tmp5E0.f1=({struct Cyc_List_List*_tmp5E1=
_cycalloc(sizeof(*_tmp5E1));_tmp5E1->hd=(void*)Cyc_yyget_Designator_tok(yyvs[
yyvsp_offset]);_tmp5E1->tl=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E1;});_tmp5E0;});
_tmp5DF;});break;case 195: _LL35D: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp5E2=_cycalloc(sizeof(*_tmp5E2));_tmp5E2[0]=({struct Cyc_Designator_tok_struct
_tmp5E3;_tmp5E3.tag=Cyc_Designator_tok;_tmp5E3.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp5E5;_tmp5E5.tag=0;_tmp5E5.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E5;});_tmp5E4;}));
_tmp5E3;});_tmp5E2;});break;case 196: _LL35E: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6[0]=({struct Cyc_Designator_tok_struct
_tmp5E7;_tmp5E7.tag=Cyc_Designator_tok;_tmp5E7.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8[0]=({struct Cyc_Absyn_FieldName_struct
_tmp5E9;_tmp5E9.tag=1;_tmp5E9.f1=({struct _tagged_arr*_tmp5EA=_cycalloc(sizeof(*
_tmp5EA));_tmp5EA[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp5EA;});_tmp5E9;});
_tmp5E8;}));_tmp5E7;});_tmp5E6;});break;case 197: _LL35F: {void*_tmp5EB=Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));struct Cyc_List_List*
_tmp5EC=(*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f3;if(_tmp5EC != 0){({
void*_tmp5ED[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("ignoring attributes in type",
sizeof(unsigned char),28),_tag_arr(_tmp5ED,sizeof(void*),0));});}{struct Cyc_Absyn_Tqual
_tmp5EE=(*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f1;yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF[0]=({
struct Cyc_ParamDecl_tok_struct _tmp5F0;_tmp5F0.tag=Cyc_ParamDecl_tok;_tmp5F0.f1=({
struct _tuple2*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1->f1=0;_tmp5F1->f2=
_tmp5EE;_tmp5F1->f3=_tmp5EB;_tmp5F1;});_tmp5F0;});_tmp5EF;});break;}}case 198:
_LL360: {void*_tmp5F2=Cyc_Parse_speclist2typ((*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp5F3=(*Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f3;struct Cyc_Absyn_Tqual _tmp5F4=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f1;struct Cyc_List_List*
_tmp5F5=(Cyc_yyget_AbstractDeclarator_tok(yyvs[yyvsp_offset]))->tms;struct
_tuple5 _tmp5F6=Cyc_Parse_apply_tms(_tmp5F4,_tmp5F2,_tmp5F3,_tmp5F5);if(_tmp5F6.f3
!= 0){({void*_tmp5F7[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad type params, ignoring",
sizeof(unsigned char),26),_tag_arr(_tmp5F7,sizeof(void*),0));});}if(_tmp5F6.f4 != 
0){({void*_tmp5F8[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad specifiers, ignoring",
sizeof(unsigned char),25),_tag_arr(_tmp5F8,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9[0]=({
struct Cyc_ParamDecl_tok_struct _tmp5FA;_tmp5FA.tag=Cyc_ParamDecl_tok;_tmp5FA.f1=({
struct _tuple2*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->f1=0;_tmp5FB->f2=
_tmp5F6.f1;_tmp5FB->f3=_tmp5F6.f2;_tmp5FB;});_tmp5FA;});_tmp5F9;});break;}case
199: _LL361: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5FC=_cycalloc(sizeof(*
_tmp5FC));_tmp5FC[0]=({struct Cyc_Type_tok_struct _tmp5FD;_tmp5FD.tag=Cyc_Type_tok;
_tmp5FD.f1=(void*)(*Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset])).f3;_tmp5FD;});
_tmp5FC;});break;case 200: _LL362: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp5FE=
_cycalloc(sizeof(*_tmp5FE));_tmp5FE[0]=({struct Cyc_Type_tok_struct _tmp5FF;
_tmp5FF.tag=Cyc_Type_tok;_tmp5FF.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp601;_tmp601.tag=18;_tmp601.f1=0;_tmp601;});_tmp600;}));_tmp5FF;});_tmp5FE;});
break;case 201: _LL363: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp602=_cycalloc(
sizeof(*_tmp602));_tmp602[0]=({struct Cyc_Type_tok_struct _tmp603;_tmp603.tag=Cyc_Type_tok;
_tmp603.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp604=_cycalloc(
sizeof(*_tmp604));_tmp604[0]=({struct Cyc_Absyn_JoinEff_struct _tmp605;_tmp605.tag=
18;_tmp605.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp605;});_tmp604;}));_tmp603;});_tmp602;});break;case 202:
_LL364: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp606=_cycalloc(sizeof(*
_tmp606));_tmp606[0]=({struct Cyc_Type_tok_struct _tmp607;_tmp607.tag=Cyc_Type_tok;
_tmp607.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp608=_cycalloc(
sizeof(*_tmp608));_tmp608[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp609;_tmp609.tag=
19;_tmp609.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp609;});_tmp608;}));_tmp607;});_tmp606;});break;case
203: _LL365: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp60A=_cycalloc(sizeof(*
_tmp60A));_tmp60A[0]=({struct Cyc_Type_tok_struct _tmp60B;_tmp60B.tag=Cyc_Type_tok;
_tmp60B.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp60C=_cycalloc(
sizeof(*_tmp60C));_tmp60C[0]=({struct Cyc_Absyn_JoinEff_struct _tmp60D;_tmp60D.tag=
18;_tmp60D.f1=({struct Cyc_List_List*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp60E->tl=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp60E;});_tmp60D;});
_tmp60C;}));_tmp60B;});_tmp60A;});break;case 204: _LL366: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F[0]=({struct Cyc_TypeList_tok_struct
_tmp610;_tmp610.tag=Cyc_TypeList_tok;_tmp610.f1=({struct Cyc_List_List*_tmp611=
_cycalloc(sizeof(*_tmp611));_tmp611->hd=(void*)Cyc_yyget_Type_tok(yyvs[
yyvsp_offset]);_tmp611->tl=0;_tmp611;});_tmp610;});_tmp60F;});break;case 205:
_LL367: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp612=_cycalloc(sizeof(*
_tmp612));_tmp612[0]=({struct Cyc_TypeList_tok_struct _tmp613;_tmp613.tag=Cyc_TypeList_tok;
_tmp613.f1=({struct Cyc_List_List*_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614->hd=(
void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);_tmp614->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp614;});_tmp613;});
_tmp612;});break;case 206: _LL368: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp616;_tmp616.tag=Cyc_AbstractDeclarator_tok;_tmp616.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]);_tmp617;});_tmp616;});_tmp615;});break;case 207: _LL369: yyval=
yyvs[yyvsp_offset];break;case 208: _LL36A: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp619;_tmp619.tag=Cyc_AbstractDeclarator_tok;_tmp619.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[yyvsp_offset]))->tms);_tmp61A;});_tmp619;});_tmp618;});break;case 209: _LL36B:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 210:
_LL36C: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp61B=_cycalloc(
sizeof(*_tmp61B));_tmp61B[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp61C;
_tmp61C.tag=Cyc_AbstractDeclarator_tok;_tmp61C.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp61D=_cycalloc(sizeof(*_tmp61D));_tmp61D->tms=({struct Cyc_List_List*_tmp61E=
_cycalloc(sizeof(*_tmp61E));_tmp61E->hd=(void*)((void*)0);_tmp61E->tl=0;_tmp61E;});
_tmp61D;});_tmp61C;});_tmp61B;});break;case 211: _LL36D: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp620;_tmp620.tag=Cyc_AbstractDeclarator_tok;_tmp620.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->tms=({struct Cyc_List_List*_tmp622=
_cycalloc(sizeof(*_tmp622));_tmp622->hd=(void*)((void*)0);_tmp622->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->tms;_tmp622;});
_tmp621;});_tmp620;});_tmp61F;});break;case 212: _LL36E: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp623=_cycalloc(sizeof(*_tmp623));_tmp623[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp624;_tmp624.tag=Cyc_AbstractDeclarator_tok;_tmp624.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625->tms=({struct Cyc_List_List*_tmp626=
_cycalloc(sizeof(*_tmp626));_tmp626->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp628;_tmp628.tag=0;_tmp628.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp628;});_tmp627;}));
_tmp626->tl=0;_tmp626;});_tmp625;});_tmp624;});_tmp623;});break;case 213: _LL36F:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp629=_cycalloc(sizeof(*
_tmp629));_tmp629[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp62A;_tmp62A.tag=
Cyc_AbstractDeclarator_tok;_tmp62A.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->tms=({struct Cyc_List_List*_tmp62C=
_cycalloc(sizeof(*_tmp62C));_tmp62C->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp62E;_tmp62E.tag=0;_tmp62E.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp62E;});_tmp62D;}));
_tmp62C->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp62C;});_tmp62B;});
_tmp62A;});_tmp629;});break;case 214: _LL370: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp630;_tmp630.tag=Cyc_AbstractDeclarator_tok;_tmp630.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->tms=({struct Cyc_List_List*_tmp632=
_cycalloc(sizeof(*_tmp632));_tmp632->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp634;_tmp634.tag=2;_tmp634.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp636;_tmp636.tag=1;_tmp636.f1=0;_tmp636.f2=0;_tmp636.f3=0;_tmp636.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp636.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp636;});_tmp635;}));
_tmp634;});_tmp633;}));_tmp632->tl=0;_tmp632;});_tmp631;});_tmp630;});_tmp62F;});
break;case 215: _LL371: {struct _tuple13 _tmp638;struct Cyc_List_List*_tmp639;struct
Cyc_Core_Opt*_tmp63A;struct Cyc_Absyn_VarargInfo*_tmp63B;int _tmp63C;struct Cyc_List_List*
_tmp63D;struct _tuple13*_tmp637=Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp638=*_tmp637;_LL378: _tmp63D=_tmp638.f1;goto _LL377;
_LL377: _tmp63C=_tmp638.f2;goto _LL376;_LL376: _tmp63B=_tmp638.f3;goto _LL375;_LL375:
_tmp63A=_tmp638.f4;goto _LL374;_LL374: _tmp639=_tmp638.f5;goto _LL373;_LL373: yyval=(
void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp63E=_cycalloc(sizeof(*
_tmp63E));_tmp63E[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp63F;_tmp63F.tag=
Cyc_AbstractDeclarator_tok;_tmp63F.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->tms=({struct Cyc_List_List*_tmp641=
_cycalloc(sizeof(*_tmp641));_tmp641->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp643;_tmp643.tag=2;_tmp643.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp644=_cycalloc(sizeof(*_tmp644));_tmp644[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp645;_tmp645.tag=1;_tmp645.f1=_tmp63D;_tmp645.f2=_tmp63C;_tmp645.f3=_tmp63B;
_tmp645.f4=_tmp63A;_tmp645.f5=_tmp639;_tmp645;});_tmp644;}));_tmp643;});_tmp642;}));
_tmp641->tl=0;_tmp641;});_tmp640;});_tmp63F;});_tmp63E;});break;}case 216: _LL372:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp646=_cycalloc(sizeof(*
_tmp646));_tmp646[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp647;_tmp647.tag=
Cyc_AbstractDeclarator_tok;_tmp647.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648->tms=({struct Cyc_List_List*_tmp649=
_cycalloc(sizeof(*_tmp649));_tmp649->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp64B;_tmp64B.tag=2;_tmp64B.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp64D;_tmp64D.tag=1;_tmp64D.f1=0;_tmp64D.f2=0;_tmp64D.f3=0;_tmp64D.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp64D.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp64D;});_tmp64C;}));
_tmp64B;});_tmp64A;}));_tmp649->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp649;});_tmp648;});
_tmp647;});_tmp646;});break;case 217: _LL379: {struct _tuple13 _tmp64F;struct Cyc_List_List*
_tmp650;struct Cyc_Core_Opt*_tmp651;struct Cyc_Absyn_VarargInfo*_tmp652;int _tmp653;
struct Cyc_List_List*_tmp654;struct _tuple13*_tmp64E=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp64F=*_tmp64E;_LL380:
_tmp654=_tmp64F.f1;goto _LL37F;_LL37F: _tmp653=_tmp64F.f2;goto _LL37E;_LL37E:
_tmp652=_tmp64F.f3;goto _LL37D;_LL37D: _tmp651=_tmp64F.f4;goto _LL37C;_LL37C:
_tmp650=_tmp64F.f5;goto _LL37B;_LL37B: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp656;_tmp656.tag=Cyc_AbstractDeclarator_tok;_tmp656.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657->tms=({struct Cyc_List_List*_tmp658=
_cycalloc(sizeof(*_tmp658));_tmp658->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp65A;_tmp65A.tag=2;_tmp65A.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp65C;_tmp65C.tag=1;_tmp65C.f1=_tmp654;_tmp65C.f2=_tmp653;_tmp65C.f3=_tmp652;
_tmp65C.f4=_tmp651;_tmp65C.f5=_tmp650;_tmp65C;});_tmp65B;}));_tmp65A;});_tmp659;}));
_tmp658->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp658;});_tmp657;});
_tmp656;});_tmp655;});break;}case 218: _LL37A: {struct Cyc_List_List*_tmp65D=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp65E=_cycalloc(sizeof(*
_tmp65E));_tmp65E[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp65F;_tmp65F.tag=
Cyc_AbstractDeclarator_tok;_tmp65F.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->tms=({struct Cyc_List_List*_tmp661=
_cycalloc(sizeof(*_tmp661));_tmp661->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp663;_tmp663.tag=3;_tmp663.f1=_tmp65D;_tmp663.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp663.f3=0;_tmp663;});_tmp662;}));_tmp661->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp661;});
_tmp660;});_tmp65F;});_tmp65E;});break;}case 219: _LL381: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp665;_tmp665.tag=Cyc_AbstractDeclarator_tok;_tmp665.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666->tms=({struct Cyc_List_List*_tmp667=
_cycalloc(sizeof(*_tmp667));_tmp667->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp669;_tmp669.tag=4;_tmp669.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp669.f2=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp669;});_tmp668;}));_tmp667->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp667;});
_tmp666;});_tmp665;});_tmp664;});break;case 220: _LL382: yyval=yyvs[yyvsp_offset];
break;case 221: _LL383: yyval=yyvs[yyvsp_offset];break;case 222: _LL384: yyval=yyvs[
yyvsp_offset];break;case 223: _LL385: yyval=yyvs[yyvsp_offset];break;case 224: _LL386:
yyval=yyvs[yyvsp_offset];break;case 225: _LL387: yyval=yyvs[yyvsp_offset];break;
case 226: _LL388: if(Cyc_Std_zstrcmp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tag_arr("`H",sizeof(
unsigned char),3))== 0){Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(unsigned char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674->name=({
struct _tagged_arr*_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp677;});_tmp674->identity=
0;_tmp674->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp675=_cycalloc(
sizeof(*_tmp675));_tmp675[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp676;_tmp676.tag=0;
_tmp676.f1=(void*)((void*)3);_tmp676;});_tmp675;}));_tmp674;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672[0]=({
struct Cyc_Absyn_VarType_struct _tmp673;_tmp673.tag=1;_tmp673.f1=tv;_tmp673;});
_tmp672;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp66A=_cycalloc(sizeof(*
_tmp66A));_tmp66A[0]=({struct Cyc_Stmt_tok_struct _tmp66B;_tmp66B.tag=Cyc_Stmt_tok;
_tmp66B.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp66C=
_cycalloc(sizeof(*_tmp66C));_tmp66C[0]=({struct Cyc_Absyn_Region_s_struct _tmp66D;
_tmp66D.tag=18;_tmp66D.f1=tv;_tmp66D.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E->f1=(void*)0;_tmp66E->f2=({struct
_tagged_arr*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp66F;});_tmp66E;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp670=_cycalloc(sizeof(*_tmp670));
_tmp670[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp671;_tmp671.tag=15;_tmp671.f1=(
void*)t;_tmp671;});_tmp670;}),0);_tmp66D.f3=Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);
_tmp66D;});_tmp66C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp66B;});_tmp66A;});break;}case 227: _LL389: if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tag_arr("H",sizeof(unsigned char),2))== 0){Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(unsigned char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->name=({
struct _tagged_arr*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685[0]=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp687;_tmp687.tag=0;_tmp687.f1=(
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);{void*_tmp686[1]={& _tmp687};Cyc_Std_aprintf(_tag_arr("`%s",
sizeof(unsigned char),4),_tag_arr(_tmp686,sizeof(void*),1));}});_tmp685;});
_tmp682->identity=0;_tmp682->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp684;_tmp684.tag=0;_tmp684.f1=(void*)((void*)3);_tmp684;});_tmp683;}));
_tmp682;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp680=_cycalloc(
sizeof(*_tmp680));_tmp680[0]=({struct Cyc_Absyn_VarType_struct _tmp681;_tmp681.tag=
1;_tmp681.f1=tv;_tmp681;});_tmp680;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678[0]=({struct Cyc_Stmt_tok_struct
_tmp679;_tmp679.tag=Cyc_Stmt_tok;_tmp679.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A[0]=({
struct Cyc_Absyn_Region_s_struct _tmp67B;_tmp67B.tag=18;_tmp67B.f1=tv;_tmp67B.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp67C=_cycalloc(sizeof(*_tmp67C));
_tmp67C->f1=(void*)0;_tmp67C->f2=({struct _tagged_arr*_tmp67D=_cycalloc(sizeof(*
_tmp67D));_tmp67D[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp67D;});_tmp67C;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp67F;_tmp67F.tag=15;_tmp67F.f1=(void*)t;_tmp67F;});_tmp67E;}),0);_tmp67B.f3=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp67B;});_tmp67A;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp679;});_tmp678;});break;}case 228: _LL38A: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688[0]=({
struct Cyc_Stmt_tok_struct _tmp689;_tmp689.tag=Cyc_Stmt_tok;_tmp689.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Cut_s_struct*_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A[
0]=({struct Cyc_Absyn_Cut_s_struct _tmp68B;_tmp68B.tag=13;_tmp68B.f1=Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]);_tmp68B;});_tmp68A;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp689;});_tmp688;});break;case 229: _LL38B: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C[0]=({
struct Cyc_Stmt_tok_struct _tmp68D;_tmp68D.tag=Cyc_Stmt_tok;_tmp68D.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmp68E=_cycalloc(sizeof(*_tmp68E));
_tmp68E[0]=({struct Cyc_Absyn_Splice_s_struct _tmp68F;_tmp68F.tag=14;_tmp68F.f1=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp68F;});_tmp68E;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp68D;});_tmp68C;});break;case 230: _LL38C: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690[0]=({
struct Cyc_Stmt_tok_struct _tmp691;_tmp691.tag=Cyc_Stmt_tok;_tmp691.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp692=_cycalloc(sizeof(*_tmp692));
_tmp692[0]=({struct Cyc_Absyn_Label_s_struct _tmp693;_tmp693.tag=15;_tmp693.f1=({
struct _tagged_arr*_tmp694=_cycalloc(sizeof(*_tmp694));_tmp694[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp694;});_tmp693.f2=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp693;});_tmp692;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp691;});_tmp690;});break;case 231: _LL38D: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695[0]=({
struct Cyc_Stmt_tok_struct _tmp696;_tmp696.tag=Cyc_Stmt_tok;_tmp696.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmp696;});_tmp695;});break;case 232: _LL38E: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697[0]=({struct Cyc_Stmt_tok_struct
_tmp698;_tmp698.tag=Cyc_Stmt_tok;_tmp698.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp698;});_tmp697;});break;case 233: _LL38F: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699[0]=({
struct Cyc_Stmt_tok_struct _tmp69A;_tmp69A.tag=Cyc_Stmt_tok;_tmp69A.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp69A;});
_tmp699;});break;case 234: _LL390: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 235: _LL391: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B[0]=({struct Cyc_Stmt_tok_struct
_tmp69C;_tmp69C.tag=Cyc_Stmt_tok;_tmp69C.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(0));_tmp69C;});_tmp69B;});break;case 236:
_LL392: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp69D=_cycalloc(sizeof(*
_tmp69D));_tmp69D[0]=({struct Cyc_Stmt_tok_struct _tmp69E;_tmp69E.tag=Cyc_Stmt_tok;
_tmp69E.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]));_tmp69E;});_tmp69D;});break;case 237: _LL393: yyval=yyvs[
yyvsp_offset];break;case 238: _LL394: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp69F=_cycalloc(sizeof(*_tmp69F));_tmp69F[0]=({struct Cyc_Stmt_tok_struct
_tmp6A0;_tmp6A0.tag=Cyc_Stmt_tok;_tmp6A0.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6A0;});_tmp69F;});break;case 239: _LL395: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1[0]=({
struct Cyc_Stmt_tok_struct _tmp6A2;_tmp6A2.tag=Cyc_Stmt_tok;_tmp6A2.f1=Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*_tmp6A3=_cycalloc(
sizeof(*_tmp6A3));_tmp6A3[0]=({struct Cyc_Absyn_Fn_d_struct _tmp6A4;_tmp6A4.tag=1;
_tmp6A4.f1=Cyc_yyget_FnDecl_tok(yyvs[yyvsp_offset]);_tmp6A4;});_tmp6A3;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line)),Cyc_Absyn_skip_stmt(
0));_tmp6A2;});_tmp6A1;});break;case 240: _LL396: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6A5=_cycalloc(sizeof(*_tmp6A5));_tmp6A5[0]=({struct Cyc_Stmt_tok_struct
_tmp6A6;_tmp6A6.tag=Cyc_Stmt_tok;_tmp6A6.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp6A8;_tmp6A8.tag=1;_tmp6A8.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6A8;});_tmp6A7;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]));_tmp6A6;});
_tmp6A5;});break;case 241: _LL397: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6A9=
_cycalloc(sizeof(*_tmp6A9));_tmp6A9[0]=({struct Cyc_Stmt_tok_struct _tmp6AA;
_tmp6AA.tag=Cyc_Stmt_tok;_tmp6AA.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6AA;});_tmp6A9;});break;case 242: _LL398: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));_tmp6AB[0]=({
struct Cyc_Stmt_tok_struct _tmp6AC;_tmp6AC.tag=Cyc_Stmt_tok;_tmp6AC.f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6AC;});_tmp6AB;});break;case 243: _LL399: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD[0]=({
struct Cyc_Stmt_tok_struct _tmp6AE;_tmp6AE.tag=Cyc_Stmt_tok;_tmp6AE.f1=Cyc_Absyn_switch_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6AE;});_tmp6AD;});break;case 244: _LL39A: if(Cyc_Std_strcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tag_arr("C",sizeof(unsigned char),2))!= 0){Cyc_Parse_err(_tag_arr("only switch \"C\" { ... } is allowed",
sizeof(unsigned char),35),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6AF=
_cycalloc(sizeof(*_tmp6AF));_tmp6AF[0]=({struct Cyc_Stmt_tok_struct _tmp6B0;
_tmp6B0.tag=Cyc_Stmt_tok;_tmp6B0.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_SwitchC_s_struct*
_tmp6B1=_cycalloc(sizeof(*_tmp6B1));_tmp6B1[0]=({struct Cyc_Absyn_SwitchC_s_struct
_tmp6B2;_tmp6B2.tag=10;_tmp6B2.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp6B2.f2=Cyc_yyget_SwitchCClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6B2;});_tmp6B1;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6B0;});
_tmp6AF;});break;case 245: _LL39B: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6B3=
_cycalloc(sizeof(*_tmp6B3));_tmp6B3[0]=({struct Cyc_Stmt_tok_struct _tmp6B4;
_tmp6B4.tag=Cyc_Stmt_tok;_tmp6B4.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6B4;});_tmp6B3;});break;case 246: _LL39C: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));
_tmp6B5[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp6B6;_tmp6B6.tag=Cyc_SwitchClauseList_tok;
_tmp6B6.f1=0;_tmp6B6;});_tmp6B5;});break;case 247: _LL39D: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6B8;_tmp6B8.tag=Cyc_SwitchClauseList_tok;_tmp6B8.f1=({struct Cyc_List_List*
_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp6BA->pat_vars=0;_tmp6BA->where_clause=0;
_tmp6BA->body=Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp6BA->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6BA;});_tmp6B9->tl=0;_tmp6B9;});_tmp6B8;});_tmp6B7;});
break;case 248: _LL39E: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6BC;_tmp6BC.tag=Cyc_SwitchClauseList_tok;_tmp6BC.f1=({struct Cyc_List_List*
_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6BE->pat_vars=0;
_tmp6BE->where_clause=0;_tmp6BE->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp6BE->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line);_tmp6BE;});_tmp6BD->tl=
Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmp6BD;});_tmp6BC;});_tmp6BB;});
break;case 249: _LL39F: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6C0;_tmp6C0.tag=Cyc_SwitchClauseList_tok;_tmp6C0.f1=({struct Cyc_List_List*
_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6C2->pat_vars=0;
_tmp6C2->where_clause=0;_tmp6C2->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6C2->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp6C2;});
_tmp6C1->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmp6C1;});
_tmp6C0;});_tmp6BF;});break;case 250: _LL3A0: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6C4;_tmp6C4.tag=Cyc_SwitchClauseList_tok;_tmp6C4.f1=({struct Cyc_List_List*
_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp6C6->pat_vars=0;
_tmp6C6->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6C6->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp6C6->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6C6;});_tmp6C5->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmp6C5;});_tmp6C4;});_tmp6C3;});break;case 251: _LL3A1: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));
_tmp6C7[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp6C8;_tmp6C8.tag=Cyc_SwitchClauseList_tok;
_tmp6C8.f1=({struct Cyc_List_List*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9->hd=({
struct Cyc_Absyn_Switch_clause*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp6CA->pat_vars=0;_tmp6CA->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6CA->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6CA->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6CA;});_tmp6C9->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmp6C9;});_tmp6C8;});_tmp6C7;});break;case 252: _LL3A2: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));
_tmp6CB[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp6CC;_tmp6CC.tag=Cyc_SwitchCClauseList_tok;
_tmp6CC.f1=0;_tmp6CC;});_tmp6CB;});break;case 253: _LL3A3: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmp6CE;_tmp6CE.tag=Cyc_SwitchCClauseList_tok;_tmp6CE.f1=({struct Cyc_List_List*
_tmp6CF=_cycalloc(sizeof(*_tmp6CF));_tmp6CF->hd=({struct Cyc_Absyn_SwitchC_clause*
_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0->cnst_exp=0;_tmp6D0->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Absyn_break_stmt(0),0);_tmp6D0->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line);_tmp6D0;});_tmp6CF->tl=
0;_tmp6CF;});_tmp6CE;});_tmp6CD;});break;case 254: _LL3A4: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmp6D2;_tmp6D2.tag=Cyc_SwitchCClauseList_tok;_tmp6D2.f1=({struct Cyc_List_List*
_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3->hd=({struct Cyc_Absyn_SwitchC_clause*
_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4->cnst_exp=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6D4->body=Cyc_Absyn_fallthru_stmt(
0,0);_tmp6D4->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6D4;});_tmp6D3->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmp6D3;});_tmp6D2;});_tmp6D1;});break;case 255: _LL3A5: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp6D5=_cycalloc(sizeof(*_tmp6D5));
_tmp6D5[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp6D6;_tmp6D6.tag=Cyc_SwitchCClauseList_tok;
_tmp6D6.f1=({struct Cyc_List_List*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7->hd=({
struct Cyc_Absyn_SwitchC_clause*_tmp6D8=_cycalloc(sizeof(*_tmp6D8));_tmp6D8->cnst_exp=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp6D8->body=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Absyn_fallthru_stmt(
0,0),0);_tmp6D8->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6D8;});_tmp6D7->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmp6D7;});_tmp6D6;});_tmp6D5;});break;case 256: _LL3A6: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9[0]=({
struct Cyc_Stmt_tok_struct _tmp6DA;_tmp6DA.tag=Cyc_Stmt_tok;_tmp6DA.f1=Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6DA;});_tmp6D9;});break;case 257: _LL3A7: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6DB=_cycalloc(sizeof(*_tmp6DB));_tmp6DB[0]=({
struct Cyc_Stmt_tok_struct _tmp6DC;_tmp6DC.tag=Cyc_Stmt_tok;_tmp6DC.f1=Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6DC;});
_tmp6DB;});break;case 258: _LL3A8: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6DD=
_cycalloc(sizeof(*_tmp6DD));_tmp6DD[0]=({struct Cyc_Stmt_tok_struct _tmp6DE;
_tmp6DE.tag=Cyc_Stmt_tok;_tmp6DE.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6DE;});
_tmp6DD;});break;case 259: _LL3A9: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6DF=
_cycalloc(sizeof(*_tmp6DF));_tmp6DF[0]=({struct Cyc_Stmt_tok_struct _tmp6E0;
_tmp6E0.tag=Cyc_Stmt_tok;_tmp6E0.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6E0;});_tmp6DF;});break;case 260: _LL3AA: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6E1=_cycalloc(sizeof(*_tmp6E1));_tmp6E1[0]=({
struct Cyc_Stmt_tok_struct _tmp6E2;_tmp6E2.tag=Cyc_Stmt_tok;_tmp6E2.f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6E2;});
_tmp6E1;});break;case 261: _LL3AB: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6E3=
_cycalloc(sizeof(*_tmp6E3));_tmp6E3[0]=({struct Cyc_Stmt_tok_struct _tmp6E4;
_tmp6E4.tag=Cyc_Stmt_tok;_tmp6E4.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp6E4;});
_tmp6E3;});break;case 262: _LL3AC: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6E5=
_cycalloc(sizeof(*_tmp6E5));_tmp6E5[0]=({struct Cyc_Stmt_tok_struct _tmp6E6;
_tmp6E6.tag=Cyc_Stmt_tok;_tmp6E6.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6E6;});_tmp6E5;});break;case 263: _LL3AD: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6E7=_cycalloc(sizeof(*_tmp6E7));_tmp6E7[0]=({
struct Cyc_Stmt_tok_struct _tmp6E8;_tmp6E8.tag=Cyc_Stmt_tok;_tmp6E8.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6E8;});_tmp6E7;});break;case 264: _LL3AE: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9[0]=({
struct Cyc_Stmt_tok_struct _tmp6EA;_tmp6EA.tag=Cyc_Stmt_tok;_tmp6EA.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6EA;});_tmp6E9;});break;case 265: _LL3AF: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6EB=_cycalloc(sizeof(*_tmp6EB));_tmp6EB[0]=({
struct Cyc_Stmt_tok_struct _tmp6EC;_tmp6EC.tag=Cyc_Stmt_tok;_tmp6EC.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6EC;});_tmp6EB;});break;case 266: _LL3B0: {struct
Cyc_List_List*_tmp6ED=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*_tmp6EE=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF[0]=({
struct Cyc_Stmt_tok_struct _tmp6F0;_tmp6F0.tag=Cyc_Stmt_tok;_tmp6F0.f1=Cyc_Parse_flatten_declarations(
_tmp6ED,_tmp6EE);_tmp6F0;});_tmp6EF;});break;}case 267: _LL3B1: {struct Cyc_List_List*
_tmp6F1=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp6F2=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6F3=
_cycalloc(sizeof(*_tmp6F3));_tmp6F3[0]=({struct Cyc_Stmt_tok_struct _tmp6F4;
_tmp6F4.tag=Cyc_Stmt_tok;_tmp6F4.f1=Cyc_Parse_flatten_declarations(_tmp6F1,
_tmp6F2);_tmp6F4;});_tmp6F3;});break;}case 268: _LL3B2: {struct Cyc_List_List*
_tmp6F5=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp6F6=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6F7=
_cycalloc(sizeof(*_tmp6F7));_tmp6F7[0]=({struct Cyc_Stmt_tok_struct _tmp6F8;
_tmp6F8.tag=Cyc_Stmt_tok;_tmp6F8.f1=Cyc_Parse_flatten_declarations(_tmp6F5,
_tmp6F6);_tmp6F8;});_tmp6F7;});break;}case 269: _LL3B3: {struct Cyc_List_List*
_tmp6F9=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp6FA=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6FB=
_cycalloc(sizeof(*_tmp6FB));_tmp6FB[0]=({struct Cyc_Stmt_tok_struct _tmp6FC;
_tmp6FC.tag=Cyc_Stmt_tok;_tmp6FC.f1=Cyc_Parse_flatten_declarations(_tmp6F9,
_tmp6FA);_tmp6FC;});_tmp6FB;});break;}case 270: _LL3B4: {struct Cyc_List_List*
_tmp6FD=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_decl2vardecl,Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE[0]=({
struct Cyc_Stmt_tok_struct _tmp6FF;_tmp6FF.tag=Cyc_Stmt_tok;_tmp6FF.f1=Cyc_Absyn_forarray_stmt(
_tmp6FD,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6FF;});_tmp6FE;});break;}case 271: _LL3B5: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp700=_cycalloc(sizeof(*_tmp700));_tmp700[0]=({
struct Cyc_Stmt_tok_struct _tmp701;_tmp701.tag=Cyc_Stmt_tok;_tmp701.f1=Cyc_Absyn_goto_stmt(({
struct _tagged_arr*_tmp702=_cycalloc(sizeof(*_tmp702));_tmp702[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp702;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp701;});
_tmp700;});break;case 272: _LL3B6: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp703=
_cycalloc(sizeof(*_tmp703));_tmp703[0]=({struct Cyc_Stmt_tok_struct _tmp704;
_tmp704.tag=Cyc_Stmt_tok;_tmp704.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp704;});
_tmp703;});break;case 273: _LL3B7: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp705=
_cycalloc(sizeof(*_tmp705));_tmp705[0]=({struct Cyc_Stmt_tok_struct _tmp706;
_tmp706.tag=Cyc_Stmt_tok;_tmp706.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp706;});
_tmp705;});break;case 274: _LL3B8: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp707=
_cycalloc(sizeof(*_tmp707));_tmp707[0]=({struct Cyc_Stmt_tok_struct _tmp708;
_tmp708.tag=Cyc_Stmt_tok;_tmp708.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp708;});
_tmp707;});break;case 275: _LL3B9: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp709=
_cycalloc(sizeof(*_tmp709));_tmp709[0]=({struct Cyc_Stmt_tok_struct _tmp70A;
_tmp70A.tag=Cyc_Stmt_tok;_tmp70A.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp70A;});_tmp709;});break;case 276: _LL3BA: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp70B=_cycalloc(sizeof(*_tmp70B));_tmp70B[0]=({
struct Cyc_Stmt_tok_struct _tmp70C;_tmp70C.tag=Cyc_Stmt_tok;_tmp70C.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp70C;});_tmp70B;});break;case 277: _LL3BB: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D[0]=({
struct Cyc_Stmt_tok_struct _tmp70E;_tmp70E.tag=Cyc_Stmt_tok;_tmp70E.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp70E;});_tmp70D;});break;case 278: _LL3BC: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp70F=_cycalloc(sizeof(*_tmp70F));_tmp70F[0]=({
struct Cyc_Stmt_tok_struct _tmp710;_tmp710.tag=Cyc_Stmt_tok;_tmp710.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp710;});_tmp70F;});break;case 279: _LL3BD: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711[
0]=({struct Cyc_Pattern_tok_struct _tmp712;_tmp712.tag=Cyc_Pattern_tok;_tmp712.f1=
Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp712;});_tmp711;});break;case 280: _LL3BE: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 281: _LL3BF:
yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp713=_cycalloc(sizeof(*_tmp713));
_tmp713[0]=({struct Cyc_Pattern_tok_struct _tmp714;_tmp714.tag=Cyc_Pattern_tok;
_tmp714.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp715=
_cycalloc(sizeof(*_tmp715));_tmp715[0]=({struct Cyc_Absyn_Int_p_struct _tmp716;
_tmp716.tag=1;_tmp716.f1=(void*)(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f1;
_tmp716.f2=(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2;_tmp716;});_tmp715;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp714;});
_tmp713;});break;case 282: _LL3C0: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp717=_cycalloc(sizeof(*_tmp717));_tmp717[0]=({struct Cyc_Pattern_tok_struct
_tmp718;_tmp718.tag=Cyc_Pattern_tok;_tmp718.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Int_p_struct*_tmp719=_cycalloc(sizeof(*_tmp719));_tmp719[0]=({struct
Cyc_Absyn_Int_p_struct _tmp71A;_tmp71A.tag=1;_tmp71A.f1=(void*)((void*)0);_tmp71A.f2=
-(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2;_tmp71A;});_tmp719;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp718;});_tmp717;});break;case 283: _LL3C1: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp71B=_cycalloc(sizeof(*_tmp71B));_tmp71B[
0]=({struct Cyc_Pattern_tok_struct _tmp71C;_tmp71C.tag=Cyc_Pattern_tok;_tmp71C.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*_tmp71D=_cycalloc(
sizeof(*_tmp71D));_tmp71D[0]=({struct Cyc_Absyn_Float_p_struct _tmp71E;_tmp71E.tag=
3;_tmp71E.f1=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp71E;});_tmp71D;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp71C;});
_tmp71B;});break;case 284: _LL3C2: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp71F=_cycalloc(sizeof(*_tmp71F));_tmp71F[0]=({struct Cyc_Pattern_tok_struct
_tmp720;_tmp720.tag=Cyc_Pattern_tok;_tmp720.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Char_p_struct*_tmp721=_cycalloc_atomic(sizeof(*_tmp721));_tmp721[0]=({
struct Cyc_Absyn_Char_p_struct _tmp722;_tmp722.tag=2;_tmp722.f1=Cyc_yyget_Char_tok(
yyvs[yyvsp_offset]);_tmp722;});_tmp721;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp720;});_tmp71F;});
break;case 285: _LL3C3: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp723=
_cycalloc(sizeof(*_tmp723));_tmp723[0]=({struct Cyc_Pattern_tok_struct _tmp724;
_tmp724.tag=Cyc_Pattern_tok;_tmp724.f1=Cyc_Absyn_new_pat((void*)1,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp724;});
_tmp723;});break;case 286: _LL3C4: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp725=_cycalloc(sizeof(*_tmp725));_tmp725[0]=({struct Cyc_Pattern_tok_struct
_tmp726;_tmp726.tag=Cyc_Pattern_tok;_tmp726.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownId_p_struct*_tmp727=_cycalloc(sizeof(*_tmp727));_tmp727[0]=({
struct Cyc_Absyn_UnknownId_p_struct _tmp728;_tmp728.tag=11;_tmp728.f1=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp728;});_tmp727;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp726;});_tmp725;});
break;case 287: _LL3C5: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp729=
_cycalloc(sizeof(*_tmp729));_tmp729[0]=({struct Cyc_Pattern_tok_struct _tmp72A;
_tmp72A.tag=Cyc_Pattern_tok;_tmp72A.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_struct*
_tmp72B=_cycalloc(sizeof(*_tmp72B));_tmp72B[0]=({struct Cyc_Absyn_Tuple_p_struct
_tmp72C;_tmp72C.tag=4;_tmp72C.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]));_tmp72C;});_tmp72B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp72A;});_tmp729;});break;case 288: _LL3C6: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp72D=_cycalloc(sizeof(*_tmp72D));_tmp72D[
0]=({struct Cyc_Pattern_tok_struct _tmp72E;_tmp72E.tag=Cyc_Pattern_tok;_tmp72E.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp72F=
_cycalloc(sizeof(*_tmp72F));_tmp72F[0]=({struct Cyc_Absyn_UnknownCall_p_struct
_tmp730;_tmp730.tag=12;_tmp730.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp730.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp730;});_tmp72F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp72E;});
_tmp72D;});break;case 289: _LL3C7: {struct Cyc_List_List*_tmp731=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732[0]=({
struct Cyc_Pattern_tok_struct _tmp733;_tmp733.tag=Cyc_Pattern_tok;_tmp733.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp734=_cycalloc(sizeof(*_tmp734));
_tmp734[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp735;_tmp735.tag=7;_tmp735.f1=({
struct Cyc_Absyn_AggrInfo _tmp736;_tmp736.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp737=_cycalloc(sizeof(*_tmp737));_tmp737[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp738;_tmp738.tag=0;_tmp738.f1=(void*)((void*)0);_tmp738.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp738;});_tmp737;}));
_tmp736.targs=0;_tmp736;});_tmp735.f2=_tmp731;_tmp735.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp735;});_tmp734;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp733;});
_tmp732;});break;}case 290: _LL3C8: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp739=_cycalloc(sizeof(*_tmp739));_tmp739[0]=({struct Cyc_Pattern_tok_struct
_tmp73A;_tmp73A.tag=Cyc_Pattern_tok;_tmp73A.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Pointer_p_struct*_tmp73B=_cycalloc(sizeof(*_tmp73B));_tmp73B[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp73C;_tmp73C.tag=5;_tmp73C.f1=Cyc_yyget_Pattern_tok(
yyvs[yyvsp_offset]);_tmp73C;});_tmp73B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp73A;});_tmp739;});break;case 291: _LL3C9: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp73D=_cycalloc(sizeof(*_tmp73D));_tmp73D[
0]=({struct Cyc_Pattern_tok_struct _tmp73E;_tmp73E.tag=Cyc_Pattern_tok;_tmp73E.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp73F=_cycalloc(
sizeof(*_tmp73F));_tmp73F[0]=({struct Cyc_Absyn_Reference_p_struct _tmp740;_tmp740.tag=
6;_tmp740.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp741=_cycalloc(sizeof(*
_tmp741));_tmp741->f1=(void*)0;_tmp741->f2=({struct _tagged_arr*_tmp742=_cycalloc(
sizeof(*_tmp742));_tmp742[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp742;});
_tmp741;}),(void*)0,0);_tmp740;});_tmp73F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp73E;});_tmp73D;});break;case 292: _LL3CA: yyval=(
void*)({struct Cyc_PatternList_tok_struct*_tmp743=_cycalloc(sizeof(*_tmp743));
_tmp743[0]=({struct Cyc_PatternList_tok_struct _tmp744;_tmp744.tag=Cyc_PatternList_tok;
_tmp744.f1=({struct Cyc_List_List*_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745->hd=
Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmp745->tl=0;_tmp745;});_tmp744;});
_tmp743;});break;case 293: _LL3CB: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp746=_cycalloc(sizeof(*_tmp746));_tmp746[0]=({struct Cyc_PatternList_tok_struct
_tmp747;_tmp747.tag=Cyc_PatternList_tok;_tmp747.f1=({struct Cyc_List_List*_tmp748=
_cycalloc(sizeof(*_tmp748));_tmp748->hd=Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);
_tmp748->tl=Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp748;});_tmp747;});_tmp746;});break;case 294: _LL3CC: yyval=(
void*)({struct Cyc_FieldPattern_tok_struct*_tmp749=_cycalloc(sizeof(*_tmp749));
_tmp749[0]=({struct Cyc_FieldPattern_tok_struct _tmp74A;_tmp74A.tag=Cyc_FieldPattern_tok;
_tmp74A.f1=({struct _tuple10*_tmp74B=_cycalloc(sizeof(*_tmp74B));_tmp74B->f1=0;
_tmp74B->f2=Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmp74B;});_tmp74A;});
_tmp749;});break;case 295: _LL3CD: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C[0]=({struct Cyc_FieldPattern_tok_struct
_tmp74D;_tmp74D.tag=Cyc_FieldPattern_tok;_tmp74D.f1=({struct _tuple10*_tmp74E=
_cycalloc(sizeof(*_tmp74E));_tmp74E->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp74E->f2=Cyc_yyget_Pattern_tok(
yyvs[yyvsp_offset]);_tmp74E;});_tmp74D;});_tmp74C;});break;case 296: _LL3CE: yyval=(
void*)({struct Cyc_FieldPatternList_tok_struct*_tmp74F=_cycalloc(sizeof(*_tmp74F));
_tmp74F[0]=({struct Cyc_FieldPatternList_tok_struct _tmp750;_tmp750.tag=Cyc_FieldPatternList_tok;
_tmp750.f1=({struct Cyc_List_List*_tmp751=_cycalloc(sizeof(*_tmp751));_tmp751->hd=
Cyc_yyget_FieldPattern_tok(yyvs[yyvsp_offset]);_tmp751->tl=0;_tmp751;});_tmp750;});
_tmp74F;});break;case 297: _LL3CF: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp752=_cycalloc(sizeof(*_tmp752));_tmp752[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp753;_tmp753.tag=Cyc_FieldPatternList_tok;_tmp753.f1=({struct Cyc_List_List*
_tmp754=_cycalloc(sizeof(*_tmp754));_tmp754->hd=Cyc_yyget_FieldPattern_tok(yyvs[
yyvsp_offset]);_tmp754->tl=Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp754;});_tmp753;});
_tmp752;});break;case 298: _LL3D0: yyval=yyvs[yyvsp_offset];break;case 299: _LL3D1:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp755=_cycalloc(sizeof(*_tmp755));
_tmp755[0]=({struct Cyc_Exp_tok_struct _tmp756;_tmp756.tag=Cyc_Exp_tok;_tmp756.f1=
Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp756;});_tmp755;});break;case 300: _LL3D2: yyval=yyvs[
yyvsp_offset];break;case 301: _LL3D3: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp757=_cycalloc(sizeof(*_tmp757));_tmp757[0]=({struct Cyc_Exp_tok_struct _tmp758;
_tmp758.tag=Cyc_Exp_tok;_tmp758.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp758;});_tmp757;});break;case 302: _LL3D4: yyval=(
void*)({struct Cyc_Primopopt_tok_struct*_tmp759=_cycalloc(sizeof(*_tmp759));
_tmp759[0]=({struct Cyc_Primopopt_tok_struct _tmp75A;_tmp75A.tag=Cyc_Primopopt_tok;
_tmp75A.f1=0;_tmp75A;});_tmp759;});break;case 303: _LL3D5: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp75B=_cycalloc(sizeof(*_tmp75B));_tmp75B[0]=({struct Cyc_Primopopt_tok_struct
_tmp75C;_tmp75C.tag=Cyc_Primopopt_tok;_tmp75C.f1=({struct Cyc_Core_Opt*_tmp75D=
_cycalloc(sizeof(*_tmp75D));_tmp75D->v=(void*)((void*)1);_tmp75D;});_tmp75C;});
_tmp75B;});break;case 304: _LL3D6: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp75E=_cycalloc(sizeof(*_tmp75E));_tmp75E[0]=({struct Cyc_Primopopt_tok_struct
_tmp75F;_tmp75F.tag=Cyc_Primopopt_tok;_tmp75F.f1=({struct Cyc_Core_Opt*_tmp760=
_cycalloc(sizeof(*_tmp760));_tmp760->v=(void*)((void*)3);_tmp760;});_tmp75F;});
_tmp75E;});break;case 305: _LL3D7: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp761=_cycalloc(sizeof(*_tmp761));_tmp761[0]=({struct Cyc_Primopopt_tok_struct
_tmp762;_tmp762.tag=Cyc_Primopopt_tok;_tmp762.f1=({struct Cyc_Core_Opt*_tmp763=
_cycalloc(sizeof(*_tmp763));_tmp763->v=(void*)((void*)4);_tmp763;});_tmp762;});
_tmp761;});break;case 306: _LL3D8: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp764=_cycalloc(sizeof(*_tmp764));_tmp764[0]=({struct Cyc_Primopopt_tok_struct
_tmp765;_tmp765.tag=Cyc_Primopopt_tok;_tmp765.f1=({struct Cyc_Core_Opt*_tmp766=
_cycalloc(sizeof(*_tmp766));_tmp766->v=(void*)((void*)0);_tmp766;});_tmp765;});
_tmp764;});break;case 307: _LL3D9: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp767=_cycalloc(sizeof(*_tmp767));_tmp767[0]=({struct Cyc_Primopopt_tok_struct
_tmp768;_tmp768.tag=Cyc_Primopopt_tok;_tmp768.f1=({struct Cyc_Core_Opt*_tmp769=
_cycalloc(sizeof(*_tmp769));_tmp769->v=(void*)((void*)2);_tmp769;});_tmp768;});
_tmp767;});break;case 308: _LL3DA: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp76A=_cycalloc(sizeof(*_tmp76A));_tmp76A[0]=({struct Cyc_Primopopt_tok_struct
_tmp76B;_tmp76B.tag=Cyc_Primopopt_tok;_tmp76B.f1=({struct Cyc_Core_Opt*_tmp76C=
_cycalloc(sizeof(*_tmp76C));_tmp76C->v=(void*)((void*)16);_tmp76C;});_tmp76B;});
_tmp76A;});break;case 309: _LL3DB: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp76D=_cycalloc(sizeof(*_tmp76D));_tmp76D[0]=({struct Cyc_Primopopt_tok_struct
_tmp76E;_tmp76E.tag=Cyc_Primopopt_tok;_tmp76E.f1=({struct Cyc_Core_Opt*_tmp76F=
_cycalloc(sizeof(*_tmp76F));_tmp76F->v=(void*)((void*)17);_tmp76F;});_tmp76E;});
_tmp76D;});break;case 310: _LL3DC: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770[0]=({struct Cyc_Primopopt_tok_struct
_tmp771;_tmp771.tag=Cyc_Primopopt_tok;_tmp771.f1=({struct Cyc_Core_Opt*_tmp772=
_cycalloc(sizeof(*_tmp772));_tmp772->v=(void*)((void*)13);_tmp772;});_tmp771;});
_tmp770;});break;case 311: _LL3DD: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp773=_cycalloc(sizeof(*_tmp773));_tmp773[0]=({struct Cyc_Primopopt_tok_struct
_tmp774;_tmp774.tag=Cyc_Primopopt_tok;_tmp774.f1=({struct Cyc_Core_Opt*_tmp775=
_cycalloc(sizeof(*_tmp775));_tmp775->v=(void*)((void*)15);_tmp775;});_tmp774;});
_tmp773;});break;case 312: _LL3DE: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp776=_cycalloc(sizeof(*_tmp776));_tmp776[0]=({struct Cyc_Primopopt_tok_struct
_tmp777;_tmp777.tag=Cyc_Primopopt_tok;_tmp777.f1=({struct Cyc_Core_Opt*_tmp778=
_cycalloc(sizeof(*_tmp778));_tmp778->v=(void*)((void*)14);_tmp778;});_tmp777;});
_tmp776;});break;case 313: _LL3DF: yyval=yyvs[yyvsp_offset];break;case 314: _LL3E0:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp779=_cycalloc(sizeof(*_tmp779));
_tmp779[0]=({struct Cyc_Exp_tok_struct _tmp77A;_tmp77A.tag=Cyc_Exp_tok;_tmp77A.f1=
Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp77A;});_tmp779;});break;case 315: _LL3E1: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp77B=_cycalloc(sizeof(*_tmp77B));_tmp77B[0]=({
struct Cyc_Exp_tok_struct _tmp77C;_tmp77C.tag=Cyc_Exp_tok;_tmp77C.f1=Cyc_Absyn_throw_exp(
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp77C;});_tmp77B;});break;case 316: _LL3E2: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp77D=_cycalloc(sizeof(*_tmp77D));_tmp77D[0]=({
struct Cyc_Exp_tok_struct _tmp77E;_tmp77E.tag=Cyc_Exp_tok;_tmp77E.f1=Cyc_Absyn_New_exp(
0,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line));_tmp77E;});
_tmp77D;});break;case 317: _LL3E3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp77F=
_cycalloc(sizeof(*_tmp77F));_tmp77F[0]=({struct Cyc_Exp_tok_struct _tmp780;_tmp780.tag=
Cyc_Exp_tok;_tmp780.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).last_line));_tmp780;});_tmp77F;});break;case 318: _LL3E4: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp781=_cycalloc(sizeof(*_tmp781));_tmp781[0]=({
struct Cyc_Exp_tok_struct _tmp782;_tmp782.tag=Cyc_Exp_tok;_tmp782.f1=Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp782;});_tmp781;});break;case 319: _LL3E5: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp783=_cycalloc(sizeof(*_tmp783));_tmp783[0]=({
struct Cyc_Exp_tok_struct _tmp784;_tmp784.tag=Cyc_Exp_tok;_tmp784.f1=Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp784;});_tmp783;});break;case 320: _LL3E6: yyval=yyvs[
yyvsp_offset];break;case 321: _LL3E7: yyval=yyvs[yyvsp_offset];break;case 322: _LL3E8:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp785=_cycalloc(sizeof(*_tmp785));
_tmp785[0]=({struct Cyc_Exp_tok_struct _tmp786;_tmp786.tag=Cyc_Exp_tok;_tmp786.f1=
Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp786;});_tmp785;});break;case 323: _LL3E9: yyval=yyvs[
yyvsp_offset];break;case 324: _LL3EA: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp787=_cycalloc(sizeof(*_tmp787));_tmp787[0]=({struct Cyc_Exp_tok_struct _tmp788;
_tmp788.tag=Cyc_Exp_tok;_tmp788.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp788;});
_tmp787;});break;case 325: _LL3EB: yyval=yyvs[yyvsp_offset];break;case 326: _LL3EC:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp789=_cycalloc(sizeof(*_tmp789));
_tmp789[0]=({struct Cyc_Exp_tok_struct _tmp78A;_tmp78A.tag=Cyc_Exp_tok;_tmp78A.f1=
Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp78A;});
_tmp789;});break;case 327: _LL3ED: yyval=yyvs[yyvsp_offset];break;case 328: _LL3EE:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp78B=_cycalloc(sizeof(*_tmp78B));
_tmp78B[0]=({struct Cyc_Exp_tok_struct _tmp78C;_tmp78C.tag=Cyc_Exp_tok;_tmp78C.f1=
Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp78C;});
_tmp78B;});break;case 329: _LL3EF: yyval=yyvs[yyvsp_offset];break;case 330: _LL3F0:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp78D=_cycalloc(sizeof(*_tmp78D));
_tmp78D[0]=({struct Cyc_Exp_tok_struct _tmp78E;_tmp78E.tag=Cyc_Exp_tok;_tmp78E.f1=
Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp78E;});
_tmp78D;});break;case 331: _LL3F1: yyval=yyvs[yyvsp_offset];break;case 332: _LL3F2:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp78F=_cycalloc(sizeof(*_tmp78F));
_tmp78F[0]=({struct Cyc_Exp_tok_struct _tmp790;_tmp790.tag=Cyc_Exp_tok;_tmp790.f1=
Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp790;});_tmp78F;});break;case 333: _LL3F3: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp791=_cycalloc(sizeof(*_tmp791));_tmp791[0]=({
struct Cyc_Exp_tok_struct _tmp792;_tmp792.tag=Cyc_Exp_tok;_tmp792.f1=Cyc_Absyn_neq_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp792;});_tmp791;});break;case 334: _LL3F4: yyval=yyvs[
yyvsp_offset];break;case 335: _LL3F5: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp793=_cycalloc(sizeof(*_tmp793));_tmp793[0]=({struct Cyc_Exp_tok_struct _tmp794;
_tmp794.tag=Cyc_Exp_tok;_tmp794.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp794;});
_tmp793;});break;case 336: _LL3F6: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp795=
_cycalloc(sizeof(*_tmp795));_tmp795[0]=({struct Cyc_Exp_tok_struct _tmp796;_tmp796.tag=
Cyc_Exp_tok;_tmp796.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp796;});
_tmp795;});break;case 337: _LL3F7: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp797=
_cycalloc(sizeof(*_tmp797));_tmp797[0]=({struct Cyc_Exp_tok_struct _tmp798;_tmp798.tag=
Cyc_Exp_tok;_tmp798.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp798;});
_tmp797;});break;case 338: _LL3F8: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp799=
_cycalloc(sizeof(*_tmp799));_tmp799[0]=({struct Cyc_Exp_tok_struct _tmp79A;_tmp79A.tag=
Cyc_Exp_tok;_tmp79A.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp79A;});
_tmp799;});break;case 339: _LL3F9: yyval=yyvs[yyvsp_offset];break;case 340: _LL3FA:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp79B=_cycalloc(sizeof(*_tmp79B));
_tmp79B[0]=({struct Cyc_Exp_tok_struct _tmp79C;_tmp79C.tag=Cyc_Exp_tok;_tmp79C.f1=
Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp79C;});
_tmp79B;});break;case 341: _LL3FB: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp79D=
_cycalloc(sizeof(*_tmp79D));_tmp79D[0]=({struct Cyc_Exp_tok_struct _tmp79E;_tmp79E.tag=
Cyc_Exp_tok;_tmp79E.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp79E;});
_tmp79D;});break;case 342: _LL3FC: yyval=yyvs[yyvsp_offset];break;case 343: _LL3FD:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp79F=_cycalloc(sizeof(*_tmp79F));
_tmp79F[0]=({struct Cyc_Exp_tok_struct _tmp7A0;_tmp7A0.tag=Cyc_Exp_tok;_tmp7A0.f1=
Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A0;});
_tmp79F;});break;case 344: _LL3FE: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A1=
_cycalloc(sizeof(*_tmp7A1));_tmp7A1[0]=({struct Cyc_Exp_tok_struct _tmp7A2;_tmp7A2.tag=
Cyc_Exp_tok;_tmp7A2.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A2;});
_tmp7A1;});break;case 345: _LL3FF: yyval=yyvs[yyvsp_offset];break;case 346: _LL400:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A3=_cycalloc(sizeof(*_tmp7A3));
_tmp7A3[0]=({struct Cyc_Exp_tok_struct _tmp7A4;_tmp7A4.tag=Cyc_Exp_tok;_tmp7A4.f1=
Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A4;});
_tmp7A3;});break;case 347: _LL401: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A5=
_cycalloc(sizeof(*_tmp7A5));_tmp7A5[0]=({struct Cyc_Exp_tok_struct _tmp7A6;_tmp7A6.tag=
Cyc_Exp_tok;_tmp7A6.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A6;});
_tmp7A5;});break;case 348: _LL402: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A7=
_cycalloc(sizeof(*_tmp7A7));_tmp7A7[0]=({struct Cyc_Exp_tok_struct _tmp7A8;_tmp7A8.tag=
Cyc_Exp_tok;_tmp7A8.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A8;});
_tmp7A7;});break;case 349: _LL403: yyval=yyvs[yyvsp_offset];break;case 350: _LL404:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A9=_cycalloc(sizeof(*_tmp7A9));
_tmp7A9[0]=({struct Cyc_Exp_tok_struct _tmp7AA;_tmp7AA.tag=Cyc_Exp_tok;_tmp7AA.f1=
Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7AA;});_tmp7A9;});break;case 351: _LL405: yyval=yyvs[
yyvsp_offset];break;case 352: _LL406: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7AB=_cycalloc(sizeof(*_tmp7AB));_tmp7AB[0]=({struct Cyc_Exp_tok_struct _tmp7AC;
_tmp7AC.tag=Cyc_Exp_tok;_tmp7AC.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7AC;});
_tmp7AB;});break;case 353: _LL407: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7AD=
_cycalloc(sizeof(*_tmp7AD));_tmp7AD[0]=({struct Cyc_Exp_tok_struct _tmp7AE;_tmp7AE.tag=
Cyc_Exp_tok;_tmp7AE.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7AE;});
_tmp7AD;});break;case 354: _LL408: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7AF=
_cycalloc(sizeof(*_tmp7AF));_tmp7AF[0]=({struct Cyc_Exp_tok_struct _tmp7B0;_tmp7B0.tag=
Cyc_Exp_tok;_tmp7B0.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B0;});
_tmp7AF;});break;case 355: _LL409: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B1=
_cycalloc(sizeof(*_tmp7B1));_tmp7B1[0]=({struct Cyc_Exp_tok_struct _tmp7B2;_tmp7B2.tag=
Cyc_Exp_tok;_tmp7B2.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B2;});
_tmp7B1;});break;case 356: _LL40A: yyval=yyvs[yyvsp_offset];break;case 357: _LL40B:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B3=_cycalloc(sizeof(*_tmp7B3));
_tmp7B3[0]=({struct Cyc_Exp_tok_struct _tmp7B4;_tmp7B4.tag=Cyc_Exp_tok;_tmp7B4.f1=
Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7B4;});_tmp7B3;});break;case 358: _LL40C: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7B5=_cycalloc(sizeof(*_tmp7B5));_tmp7B5[0]=({
struct Cyc_Exp_tok_struct _tmp7B6;_tmp7B6.tag=Cyc_Exp_tok;_tmp7B6.f1=Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)])).f3,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B6;});
_tmp7B5;});break;case 359: _LL40D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B7=
_cycalloc(sizeof(*_tmp7B7));_tmp7B7[0]=({struct Cyc_Exp_tok_struct _tmp7B8;_tmp7B8.tag=
Cyc_Exp_tok;_tmp7B8.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B8;});
_tmp7B7;});break;case 360: _LL40E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B9=
_cycalloc(sizeof(*_tmp7B9));_tmp7B9[0]=({struct Cyc_Exp_tok_struct _tmp7BA;_tmp7BA.tag=
Cyc_Exp_tok;_tmp7BA.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp7BB=_cycalloc(sizeof(*_tmp7BB));_tmp7BB[0]=({struct Cyc_Absyn_StructField_struct
_tmp7BC;_tmp7BC.tag=0;_tmp7BC.f1=({struct _tagged_arr*_tmp7BD=_cycalloc(sizeof(*
_tmp7BD));_tmp7BD[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp7BD;});_tmp7BC;});_tmp7BB;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7BA;});_tmp7B9;});break;case 361: _LL40F: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7BE=_cycalloc(sizeof(*_tmp7BE));_tmp7BE[0]=({
struct Cyc_Exp_tok_struct _tmp7BF;_tmp7BF.tag=Cyc_Exp_tok;_tmp7BF.f1=Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp7C0=_cycalloc_atomic(
sizeof(*_tmp7C0));_tmp7C0[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp7C1;_tmp7C1.tag=
1;_tmp7C1.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp7C1;});_tmp7C0;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7BF;});
_tmp7BE;});break;case 362: _LL410: {struct Cyc_Position_Segment*_tmp7C2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);struct Cyc_List_List*_tmp7C3=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp7C2,Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_Exp_tok_struct*_tmp7C4=_cycalloc(sizeof(*_tmp7C4));_tmp7C4[0]=({struct
Cyc_Exp_tok_struct _tmp7C5;_tmp7C5.tag=Cyc_Exp_tok;_tmp7C5.f1=Cyc_Absyn_gentyp_exp(
_tmp7C3,(*Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C5;});_tmp7C4;});break;}case 363: _LL411: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7C6=_cycalloc(sizeof(*_tmp7C6));_tmp7C6[0]=({
struct Cyc_Exp_tok_struct _tmp7C7;_tmp7C7.tag=Cyc_Exp_tok;_tmp7C7.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7C8=_cycalloc(sizeof(*_tmp7C8));
_tmp7C8[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7C9;_tmp7C9.tag=33;_tmp7C9.f1=({
struct Cyc_Absyn_MallocInfo _tmp7CA;_tmp7CA.is_calloc=0;_tmp7CA.rgn=0;_tmp7CA.elt_type=
0;_tmp7CA.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp7CA.fat_result=0;_tmp7CA;});_tmp7C9;});_tmp7C8;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C7;});_tmp7C6;});break;case 364: _LL412: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7CB=_cycalloc(sizeof(*_tmp7CB));_tmp7CB[0]=({
struct Cyc_Exp_tok_struct _tmp7CC;_tmp7CC.tag=Cyc_Exp_tok;_tmp7CC.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7CD=_cycalloc(sizeof(*_tmp7CD));
_tmp7CD[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7CE;_tmp7CE.tag=33;_tmp7CE.f1=({
struct Cyc_Absyn_MallocInfo _tmp7CF;_tmp7CF.is_calloc=0;_tmp7CF.rgn=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp7CF.elt_type=0;_tmp7CF.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7CF.fat_result=0;
_tmp7CF;});_tmp7CE;});_tmp7CD;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7CC;});_tmp7CB;});break;case 365: _LL413: {void*
_tmp7D1;struct _tuple2 _tmp7D0=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_LL416: _tmp7D1=_tmp7D0.f3;
goto _LL415;_LL415: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7D2=_cycalloc(
sizeof(*_tmp7D2));_tmp7D2[0]=({struct Cyc_Exp_tok_struct _tmp7D3;_tmp7D3.tag=Cyc_Exp_tok;
_tmp7D3.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7D4=
_cycalloc(sizeof(*_tmp7D4));_tmp7D4[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7D5;
_tmp7D5.tag=33;_tmp7D5.f1=({struct Cyc_Absyn_MallocInfo _tmp7D6;_tmp7D6.is_calloc=
1;_tmp7D6.rgn=0;_tmp7D6.elt_type=({void**_tmp7D7=_cycalloc(sizeof(*_tmp7D7));
_tmp7D7[0]=_tmp7D1;_tmp7D7;});_tmp7D6.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp7D6.fat_result=0;
_tmp7D6;});_tmp7D5;});_tmp7D4;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7D3;});_tmp7D2;});break;}case 366: _LL414: {void*
_tmp7D9;struct _tuple2 _tmp7D8=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_LL419: _tmp7D9=_tmp7D8.f3;
goto _LL418;_LL418: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7DA=_cycalloc(
sizeof(*_tmp7DA));_tmp7DA[0]=({struct Cyc_Exp_tok_struct _tmp7DB;_tmp7DB.tag=Cyc_Exp_tok;
_tmp7DB.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7DC=
_cycalloc(sizeof(*_tmp7DC));_tmp7DC[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7DD;
_tmp7DD.tag=33;_tmp7DD.f1=({struct Cyc_Absyn_MallocInfo _tmp7DE;_tmp7DE.is_calloc=
1;_tmp7DE.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp7DE.elt_type=({void**
_tmp7DF=_cycalloc(sizeof(*_tmp7DF));_tmp7DF[0]=_tmp7D9;_tmp7DF;});_tmp7DE.num_elts=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp7DE.fat_result=0;_tmp7DE;});_tmp7DD;});_tmp7DC;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7DB;});_tmp7DA;});break;}case 367: _LL417: yyval=(
void*)({struct Cyc_Primop_tok_struct*_tmp7E0=_cycalloc(sizeof(*_tmp7E0));_tmp7E0[
0]=({struct Cyc_Primop_tok_struct _tmp7E1;_tmp7E1.tag=Cyc_Primop_tok;_tmp7E1.f1=(
void*)((void*)12);_tmp7E1;});_tmp7E0;});break;case 368: _LL41A: yyval=(void*)({
struct Cyc_Primop_tok_struct*_tmp7E2=_cycalloc(sizeof(*_tmp7E2));_tmp7E2[0]=({
struct Cyc_Primop_tok_struct _tmp7E3;_tmp7E3.tag=Cyc_Primop_tok;_tmp7E3.f1=(void*)((
void*)11);_tmp7E3;});_tmp7E2;});break;case 369: _LL41B: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp7E4=_cycalloc(sizeof(*_tmp7E4));_tmp7E4[0]=({struct Cyc_Primop_tok_struct
_tmp7E5;_tmp7E5.tag=Cyc_Primop_tok;_tmp7E5.f1=(void*)((void*)2);_tmp7E5;});
_tmp7E4;});break;case 370: _LL41C: yyval=yyvs[yyvsp_offset];break;case 371: _LL41D:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7E6=_cycalloc(sizeof(*_tmp7E6));
_tmp7E6[0]=({struct Cyc_Exp_tok_struct _tmp7E7;_tmp7E7.tag=Cyc_Exp_tok;_tmp7E7.f1=
Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7E7;});_tmp7E6;});break;case 372: _LL41E: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7E8=_cycalloc(sizeof(*_tmp7E8));_tmp7E8[0]=({
struct Cyc_Exp_tok_struct _tmp7E9;_tmp7E9.tag=Cyc_Exp_tok;_tmp7E9.f1=Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7E9;});
_tmp7E8;});break;case 373: _LL41F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7EA=
_cycalloc(sizeof(*_tmp7EA));_tmp7EA[0]=({struct Cyc_Exp_tok_struct _tmp7EB;_tmp7EB.tag=
Cyc_Exp_tok;_tmp7EB.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7EB;});_tmp7EA;});break;case 374: _LL420: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7EC=_cycalloc(sizeof(*_tmp7EC));_tmp7EC[0]=({
struct Cyc_Exp_tok_struct _tmp7ED;_tmp7ED.tag=Cyc_Exp_tok;_tmp7ED.f1=Cyc_Absyn_aggrmember_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
struct _tagged_arr*_tmp7EE=_cycalloc(sizeof(*_tmp7EE));_tmp7EE[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp7EE;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7ED;});_tmp7EC;});break;case 375: _LL421: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7EF=_cycalloc(sizeof(*_tmp7EF));_tmp7EF[0]=({
struct Cyc_Exp_tok_struct _tmp7F0;_tmp7F0.tag=Cyc_Exp_tok;_tmp7F0.f1=Cyc_Absyn_aggrarrow_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
struct _tagged_arr*_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp7F1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F0;});_tmp7EF;});break;case 376: _LL422: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7F2=_cycalloc(sizeof(*_tmp7F2));_tmp7F2[0]=({
struct Cyc_Exp_tok_struct _tmp7F3;_tmp7F3.tag=Cyc_Exp_tok;_tmp7F3.f1=Cyc_Absyn_post_inc_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7F3;});
_tmp7F2;});break;case 377: _LL423: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7F4=
_cycalloc(sizeof(*_tmp7F4));_tmp7F4[0]=({struct Cyc_Exp_tok_struct _tmp7F5;_tmp7F5.tag=
Cyc_Exp_tok;_tmp7F5.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F5;});_tmp7F4;});break;case 378: _LL424: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7F6=_cycalloc(sizeof(*_tmp7F6));_tmp7F6[0]=({
struct Cyc_Exp_tok_struct _tmp7F7;_tmp7F7.tag=Cyc_Exp_tok;_tmp7F7.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp7F8=_cycalloc(sizeof(*_tmp7F8));
_tmp7F8[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp7F9;_tmp7F9.tag=25;_tmp7F9.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
4)]);_tmp7F9.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp7F9;});_tmp7F8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F7;});_tmp7F6;});break;case 379: _LL425: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7FA=_cycalloc(sizeof(*_tmp7FA));_tmp7FA[0]=({
struct Cyc_Exp_tok_struct _tmp7FB;_tmp7FB.tag=Cyc_Exp_tok;_tmp7FB.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp7FC=_cycalloc(sizeof(*_tmp7FC));
_tmp7FC[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp7FD;_tmp7FD.tag=25;_tmp7FD.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmp7FD.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp7FD;});_tmp7FC;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7FB;});_tmp7FA;});break;case 380: _LL426: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7FE=_cycalloc(sizeof(*_tmp7FE));_tmp7FE[0]=({
struct Cyc_Exp_tok_struct _tmp7FF;_tmp7FF.tag=Cyc_Exp_tok;_tmp7FF.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Fill_e_struct*_tmp800=_cycalloc(sizeof(*_tmp800));
_tmp800[0]=({struct Cyc_Absyn_Fill_e_struct _tmp801;_tmp801.tag=37;_tmp801.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp801;});_tmp800;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7FF;});
_tmp7FE;});break;case 381: _LL427: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp802=
_cycalloc(sizeof(*_tmp802));_tmp802[0]=({struct Cyc_Exp_tok_struct _tmp803;_tmp803.tag=
Cyc_Exp_tok;_tmp803.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Codegen_e_struct*
_tmp804=_cycalloc(sizeof(*_tmp804));_tmp804[0]=({struct Cyc_Absyn_Codegen_e_struct
_tmp805;_tmp805.tag=36;_tmp805.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp805;});_tmp804;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp803;});
_tmp802;});break;case 382: _LL428: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp806=
_cycalloc(sizeof(*_tmp806));_tmp806[0]=({struct Cyc_Exp_tok_struct _tmp807;_tmp807.tag=
Cyc_Exp_tok;_tmp807.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp808=_cycalloc(sizeof(*_tmp808));_tmp808[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp809;_tmp809.tag=2;_tmp809.f1=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);
_tmp809;});_tmp808;}),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp807;});_tmp806;});break;case 383: _LL429: yyval=
yyvs[yyvsp_offset];break;case 384: _LL42A: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp80A=_cycalloc(sizeof(*_tmp80A));_tmp80A[0]=({struct Cyc_Exp_tok_struct _tmp80B;
_tmp80B.tag=Cyc_Exp_tok;_tmp80B.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp80B;});_tmp80A;});break;case 385: _LL42B: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 386: _LL42C:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp80C=_cycalloc(sizeof(*_tmp80C));
_tmp80C[0]=({struct Cyc_Exp_tok_struct _tmp80D;_tmp80D.tag=Cyc_Exp_tok;_tmp80D.f1=
Cyc_Absyn_noinstantiate_exp(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp80E=_cycalloc(sizeof(*_tmp80E));_tmp80E[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp80F;_tmp80F.tag=2;_tmp80F.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp80F;});_tmp80E;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp80D;});_tmp80C;});break;case 387: _LL42D: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp810=_cycalloc(sizeof(*_tmp810));_tmp810[0]=({
struct Cyc_Exp_tok_struct _tmp811;_tmp811.tag=Cyc_Exp_tok;_tmp811.f1=Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp812=_cycalloc(
sizeof(*_tmp812));_tmp812[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp813;_tmp813.tag=
2;_tmp813.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp813;});_tmp812;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp811;});
_tmp810;});break;case 388: _LL42E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp814=
_cycalloc(sizeof(*_tmp814));_tmp814[0]=({struct Cyc_Exp_tok_struct _tmp815;_tmp815.tag=
Cyc_Exp_tok;_tmp815.f1=Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp815;});_tmp814;});break;case 389: _LL42F: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816[0]=({
struct Cyc_Exp_tok_struct _tmp817;_tmp817.tag=Cyc_Exp_tok;_tmp817.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Struct_e_struct*_tmp818=_cycalloc(sizeof(*_tmp818));
_tmp818[0]=({struct Cyc_Absyn_Struct_e_struct _tmp819;_tmp819.tag=28;_tmp819.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp819.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp819.f3=0;_tmp819;});_tmp818;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp817;});_tmp816;});break;case 390: _LL430: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp81A=_cycalloc(sizeof(*_tmp81A));_tmp81A[0]=({
struct Cyc_Exp_tok_struct _tmp81B;_tmp81B.tag=Cyc_Exp_tok;_tmp81B.f1=Cyc_Absyn_stmt_exp(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp81B;});
_tmp81A;});break;case 391: _LL431: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C[0]=({struct Cyc_ExpList_tok_struct
_tmp81D;_tmp81D.tag=Cyc_ExpList_tok;_tmp81D.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(yyvs[yyvsp_offset]));
_tmp81D;});_tmp81C;});break;case 392: _LL432: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E[0]=({struct Cyc_ExpList_tok_struct
_tmp81F;_tmp81F.tag=Cyc_ExpList_tok;_tmp81F.f1=({struct Cyc_List_List*_tmp820=
_cycalloc(sizeof(*_tmp820));_tmp820->hd=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);
_tmp820->tl=0;_tmp820;});_tmp81F;});_tmp81E;});break;case 393: _LL433: yyval=(void*)({
struct Cyc_ExpList_tok_struct*_tmp821=_cycalloc(sizeof(*_tmp821));_tmp821[0]=({
struct Cyc_ExpList_tok_struct _tmp822;_tmp822.tag=Cyc_ExpList_tok;_tmp822.f1=({
struct Cyc_List_List*_tmp823=_cycalloc(sizeof(*_tmp823));_tmp823->hd=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp823->tl=Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp823;});_tmp822;});
_tmp821;});break;case 394: _LL434: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp824=
_cycalloc(sizeof(*_tmp824));_tmp824[0]=({struct Cyc_Exp_tok_struct _tmp825;_tmp825.tag=
Cyc_Exp_tok;_tmp825.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f1,(*
Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp825;});_tmp824;});
break;case 395: _LL435: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp826=_cycalloc(
sizeof(*_tmp826));_tmp826[0]=({struct Cyc_Exp_tok_struct _tmp827;_tmp827.tag=Cyc_Exp_tok;
_tmp827.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp827;});
_tmp826;});break;case 396: _LL436: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp828=
_cycalloc(sizeof(*_tmp828));_tmp828[0]=({struct Cyc_Exp_tok_struct _tmp829;_tmp829.tag=
Cyc_Exp_tok;_tmp829.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));_tmp829;});_tmp828;});break;case 397: _LL437: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp82A=_cycalloc(sizeof(*_tmp82A));_tmp82A[0]=({
struct Cyc_Exp_tok_struct _tmp82B;_tmp82B.tag=Cyc_Exp_tok;_tmp82B.f1=Cyc_Absyn_null_exp(
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmp82B;});_tmp82A;});break;case 398: _LL438: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp82C=_cycalloc(sizeof(*_tmp82C));_tmp82C[0]=({struct Cyc_QualId_tok_struct
_tmp82D;_tmp82D.tag=Cyc_QualId_tok;_tmp82D.f1=({struct _tuple1*_tmp82E=_cycalloc(
sizeof(*_tmp82E));_tmp82E->f1=Cyc_Absyn_rel_ns_null;_tmp82E->f2=({struct
_tagged_arr*_tmp82F=_cycalloc(sizeof(*_tmp82F));_tmp82F[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp82F;});_tmp82E;});_tmp82D;});_tmp82C;});break;case 399:
_LL439: yyval=yyvs[yyvsp_offset];break;case 400: _LL43A: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp830=_cycalloc(sizeof(*_tmp830));_tmp830[0]=({struct Cyc_QualId_tok_struct
_tmp831;_tmp831.tag=Cyc_QualId_tok;_tmp831.f1=({struct _tuple1*_tmp832=_cycalloc(
sizeof(*_tmp832));_tmp832->f1=Cyc_Absyn_rel_ns_null;_tmp832->f2=({struct
_tagged_arr*_tmp833=_cycalloc(sizeof(*_tmp833));_tmp833[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp833;});_tmp832;});_tmp831;});_tmp830;});break;case 401:
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
0? yystate <= 5049: 0)? Cyc_yycheck[yystate]== yyss[yyssp_offset]: 0){yystate=(int)
Cyc_yytable[yystate];}else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(115,yyn - 126)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[yystate];if(yyn > - 32768? yyn < 
5049: 0){int sze=0;struct _tagged_arr msg;int x;int count;count=0;for(x=yyn < 0? - yyn: 0;
x < 241 / sizeof(unsigned char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(5050,x + yyn)]== x){(sze +=Cyc_Std_strlen(Cyc_yytname[
x])+ 15,count ++);}}msg=({unsigned int _tmp834=(unsigned int)(sze + 15);
unsigned char*_tmp835=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmp834));struct _tagged_arr _tmp837=_tag_arr(_tmp835,sizeof(
unsigned char),(unsigned int)(sze + 15));{unsigned int _tmp836=_tmp834;
unsigned int i;for(i=0;i < _tmp836;i ++){_tmp835[i]='\000';}};_tmp837;});Cyc_Std_strcpy(
msg,_tag_arr("parse error",sizeof(unsigned char),12));if(count < 5){count=0;for(x=
yyn < 0? - yyn: 0;x < 241 / sizeof(unsigned char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(5050,x + yyn)]== x){Cyc_Std_strcat(msg,count == 0?
_tag_arr(", expecting `",sizeof(unsigned char),14): _tag_arr(" or `",sizeof(
unsigned char),6));Cyc_Std_strcat(msg,Cyc_yytname[x]);Cyc_Std_strcat(msg,
_tag_arr("'",sizeof(unsigned char),2));count ++;}}}Cyc_yyerror((struct _tagged_arr)
msg);}else{Cyc_yyerror(_tag_arr("parse error",sizeof(unsigned char),12));}}goto
yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){return 1;}Cyc_yychar=-
2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0){
return 1;}yyvsp_offset --;yystate=(int)yyss[_check_known_subscript_notnull(10000,
-- yyssp_offset)];yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[yystate];if(yyn
== - 32768){goto yyerrdefault;}yyn +=1;if((yyn < 0? 1: yyn > 5049)? 1: Cyc_yycheck[yyn]!= 
1){goto yyerrdefault;}yyn=(int)Cyc_yytable[yyn];if(yyn < 0){if(yyn == - 32768){goto
yyerrpop;}yyn=- yyn;goto yyreduce;}else{if(yyn == 0){goto yyerrpop;}}if(yyn == 827){
return 0;}yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;
yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto
yynewstate;}void Cyc_yyprint(int i,void*v){void*_tmp841=v;struct _tuple12*_tmp842;
struct _tuple12 _tmp843;int _tmp844;unsigned char _tmp845;short _tmp846;struct
_tagged_arr _tmp847;struct Cyc_Core_Opt*_tmp848;struct Cyc_Core_Opt*_tmp849;struct
Cyc_Core_Opt _tmp84A;struct _tagged_arr*_tmp84B;struct _tuple1*_tmp84C;struct
_tuple1 _tmp84D;struct _tagged_arr*_tmp84E;void*_tmp84F;_LL445: if(*((void**)
_tmp841)== Cyc_Int_tok){_LL455: _tmp842=((struct Cyc_Int_tok_struct*)_tmp841)->f1;
_tmp843=*_tmp842;_LL456: _tmp844=_tmp843.f2;goto _LL446;}else{goto _LL447;}_LL447:
if(*((void**)_tmp841)== Cyc_Char_tok){_LL457: _tmp845=((struct Cyc_Char_tok_struct*)
_tmp841)->f1;goto _LL448;}else{goto _LL449;}_LL449: if(*((void**)_tmp841)== Cyc_Short_tok){
_LL458: _tmp846=((struct Cyc_Short_tok_struct*)_tmp841)->f1;goto _LL44A;}else{goto
_LL44B;}_LL44B: if(*((void**)_tmp841)== Cyc_String_tok){_LL459: _tmp847=((struct
Cyc_String_tok_struct*)_tmp841)->f1;goto _LL44C;}else{goto _LL44D;}_LL44D: if(*((
void**)_tmp841)== Cyc_Stringopt_tok){_LL45A: _tmp848=((struct Cyc_Stringopt_tok_struct*)
_tmp841)->f1;if(_tmp848 == 0){goto _LL44E;}else{goto _LL44F;}}else{goto _LL44F;}
_LL44F: if(*((void**)_tmp841)== Cyc_Stringopt_tok){_LL45B: _tmp849=((struct Cyc_Stringopt_tok_struct*)
_tmp841)->f1;if(_tmp849 == 0){goto _LL451;}else{_tmp84A=*_tmp849;_LL45C: _tmp84B=(
struct _tagged_arr*)_tmp84A.v;goto _LL450;}}else{goto _LL451;}_LL451: if(*((void**)
_tmp841)== Cyc_QualId_tok){_LL45D: _tmp84C=((struct Cyc_QualId_tok_struct*)_tmp841)->f1;
_tmp84D=*_tmp84C;_LL45F: _tmp84F=_tmp84D.f1;goto _LL45E;_LL45E: _tmp84E=_tmp84D.f2;
goto _LL452;}else{goto _LL453;}_LL453: goto _LL454;_LL446:({struct Cyc_Std_Int_pa_struct
_tmp851;_tmp851.tag=1;_tmp851.f1=(int)((unsigned int)_tmp844);{void*_tmp850[1]={&
_tmp851};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%d",sizeof(unsigned char),3),
_tag_arr(_tmp850,sizeof(void*),1));}});goto _LL444;_LL448:({struct Cyc_Std_Int_pa_struct
_tmp853;_tmp853.tag=1;_tmp853.f1=(int)((unsigned int)((int)_tmp845));{void*
_tmp852[1]={& _tmp853};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%c",sizeof(
unsigned char),3),_tag_arr(_tmp852,sizeof(void*),1));}});goto _LL444;_LL44A:({
struct Cyc_Std_Int_pa_struct _tmp855;_tmp855.tag=1;_tmp855.f1=(int)((unsigned int)((
int)_tmp846));{void*_tmp854[1]={& _tmp855};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%ds",sizeof(unsigned char),4),_tag_arr(_tmp854,sizeof(void*),1));}});
goto _LL444;_LL44C:({struct Cyc_Std_String_pa_struct _tmp857;_tmp857.tag=0;_tmp857.f1=(
struct _tagged_arr)_tmp847;{void*_tmp856[1]={& _tmp857};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\"%s\"",sizeof(unsigned char),5),_tag_arr(_tmp856,sizeof(void*),1));}});
goto _LL444;_LL44E:({void*_tmp858[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("NULL",
sizeof(unsigned char),5),_tag_arr(_tmp858,sizeof(void*),0));});goto _LL444;_LL450:({
struct Cyc_Std_String_pa_struct _tmp85A;_tmp85A.tag=0;_tmp85A.f1=(struct
_tagged_arr)*_tmp84B;{void*_tmp859[1]={& _tmp85A};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\"%s\"",sizeof(unsigned char),5),_tag_arr(_tmp859,sizeof(void*),1));}});
goto _LL444;_LL452: {struct Cyc_List_List*_tmp85B=0;{void*_tmp85C=_tmp84F;struct
Cyc_List_List*_tmp85D;struct Cyc_List_List*_tmp85E;_LL461: if((unsigned int)
_tmp85C > 1?*((int*)_tmp85C)== 0: 0){_LL467: _tmp85D=((struct Cyc_Absyn_Rel_n_struct*)
_tmp85C)->f1;goto _LL462;}else{goto _LL463;}_LL463: if((unsigned int)_tmp85C > 1?*((
int*)_tmp85C)== 1: 0){_LL468: _tmp85E=((struct Cyc_Absyn_Abs_n_struct*)_tmp85C)->f1;
goto _LL464;}else{goto _LL465;}_LL465: if((int)_tmp85C == 0){goto _LL466;}else{goto
_LL460;}_LL462: _tmp85B=_tmp85D;goto _LL460;_LL464: _tmp85B=_tmp85E;goto _LL460;
_LL466: goto _LL460;_LL460:;}for(0;_tmp85B != 0;_tmp85B=_tmp85B->tl){({struct Cyc_Std_String_pa_struct
_tmp860;_tmp860.tag=0;_tmp860.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp85B->hd);{void*_tmp85F[1]={& _tmp860};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",
sizeof(unsigned char),5),_tag_arr(_tmp85F,sizeof(void*),1));}});}({struct Cyc_Std_String_pa_struct
_tmp862;_tmp862.tag=0;_tmp862.f1=(struct _tagged_arr)*_tmp84E;{void*_tmp861[1]={&
_tmp862};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",sizeof(unsigned char),5),
_tag_arr(_tmp861,sizeof(void*),1));}});goto _LL444;}_LL454:({void*_tmp863[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("?",sizeof(unsigned char),2),_tag_arr(
_tmp863,sizeof(void*),0));});goto _LL444;_LL444:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc_Std___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*
_tmp864=_cycalloc(sizeof(*_tmp864));_tmp864->v=Cyc_Lexing_from_file(f);_tmp864;});
Cyc_yyparse();return Cyc_Parse_parse_result;}
