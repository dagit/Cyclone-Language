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
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_TunionType_struct{
int tag;struct Cyc_Absyn_TunionInfo f1;};struct Cyc_Absyn_TunionFieldType_struct{int
tag;struct Cyc_Absyn_TunionFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{int
tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;void*f1;struct Cyc_Absyn_Tqual f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_FnType_struct{
int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_StructType_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Structdecl**f3;};struct Cyc_Absyn_UnionType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Uniondecl**f3;};
struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_AnonStructType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonUnionType_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
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
};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_StructMember_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_StructArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple1*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Tuniondecl*f4;struct Cyc_Absyn_Tunionfield*
f5;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
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
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Struct_p_struct{int tag;
struct Cyc_Absyn_Structdecl*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;};struct Cyc_Absyn_Tunion_p_struct{int tag;struct Cyc_Absyn_Tuniondecl*
f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;
};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
Cyc_Absyn_UnknownFields_p_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;
struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*
cnst_exp;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
void*sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structfield{
struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structdecl{void*sc;struct
Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Uniondecl{void*sc;struct Cyc_Core_Opt*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Struct_d_struct{int tag;struct Cyc_Absyn_Structdecl*
f1;};struct Cyc_Absyn_Union_d_struct{int tag;struct Cyc_Absyn_Uniondecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
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
struct Cyc_Absyn_Exp*Cyc_Absyn_structmember_exp(struct Cyc_Absyn_Exp*,struct
_tagged_arr*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_structarrow_exp(
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
struct Cyc_Absyn_Exp*init);extern struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(void*
s,struct Cyc_Core_Opt*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(
void*s,struct Cyc_Core_Opt*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,struct
Cyc_List_List*atts,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Decl*
Cyc_Absyn_tunion_decl(void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_xtunion,struct Cyc_Position_Segment*loc);extern void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*);extern struct _tagged_arr Cyc_Absyn_attribute2string(
void*);extern int Cyc_Absyn_fntype_att(void*a);struct Cyc_Set_Set;extern
unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
12];extern unsigned char Cyc_Dict_Absent[11];struct Cyc_Tcenv_VarRes_struct{int tag;
void*f1;};struct Cyc_Tcenv_StructRes_struct{int tag;struct Cyc_Absyn_Structdecl*f1;
};struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;struct
Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*structdecls;struct Cyc_Dict_Dict*uniondecls;struct
Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*
typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;};struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{int tag;void*f1;void*f2;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*
le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_copy_type(
void*t);extern void*Cyc_Tcutil_compress(void*t);extern void Cyc_Lex_register_typedef(
struct _tuple1*s);extern void Cyc_Lex_enter_namespace(struct _tagged_arr*);extern
void Cyc_Lex_leave_namespace();extern void Cyc_Lex_enter_using(struct _tuple1*);
extern void Cyc_Lex_leave_using();enum Cyc_Parse_Struct_or_union{Cyc_Parse_Struct_su
 = 0,Cyc_Parse_Union_su  = 1};struct Cyc_Parse_Signed_spec_struct{int tag;struct Cyc_Position_Segment*
f1;};struct Cyc_Parse_Unsigned_spec_struct{int tag;struct Cyc_Position_Segment*f1;}
;struct Cyc_Parse_Short_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct
Cyc_Parse_Long_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{
int tag;void*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{
int tag;struct Cyc_Absyn_Decl*f1;};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple4{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple4 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple5{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple5
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
struct Cyc_Core_Opt*Cyc_Parse_lbuf=0;static struct Cyc_List_List*Cyc_Parse_parse_result=
0;static void Cyc_Parse_err(struct _tagged_arr msg,struct Cyc_Position_Segment*sg){
Cyc_Position_post_error(Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(
struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Parse_err((
struct _tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);(int)_throw((
void*)Cyc_Position_Exit);}static void Cyc_Parse_unimp(struct _tagged_arr msg,struct
Cyc_Position_Segment*sg){({struct Cyc_Std_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(
struct _tagged_arr)msg;{struct Cyc_Std_String_pa_struct _tmp1;_tmp1.tag=0;_tmp1.f1=(
struct _tagged_arr)Cyc_Position_string_of_segment(sg);{void*_tmp0[2]={& _tmp1,&
_tmp2};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: Warning: Cyclone does not yet support %s\n",
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
f2;};static struct Cyc_Absyn_Structfield*Cyc_Parse_make_struct_field(struct Cyc_Position_Segment*
loc,struct _tuple7*field_info){struct _tuple7 _tmpA;struct Cyc_Absyn_Exp*_tmpB;
struct _tuple6*_tmpC;struct _tuple6 _tmpD;struct Cyc_List_List*_tmpE;struct Cyc_List_List*
_tmpF;void*_tmp10;struct Cyc_Absyn_Tqual _tmp11;struct _tuple1*_tmp12;struct _tuple7*
_tmp9=field_info;_tmpA=*_tmp9;_LL2: _tmpC=_tmpA.f1;_tmpD=*_tmpC;_LL7: _tmp12=_tmpD.f1;
goto _LL6;_LL6: _tmp11=_tmpD.f2;goto _LL5;_LL5: _tmp10=_tmpD.f3;goto _LL4;_LL4: _tmpF=
_tmpD.f4;goto _LL3;_LL3: _tmpE=_tmpD.f5;goto _LL1;_LL1: _tmpB=_tmpA.f2;goto _LL0;_LL0:
if(_tmpF != 0){Cyc_Parse_err(_tag_arr("bad type params in struct field",sizeof(
unsigned char),32),loc);}if(Cyc_Absyn_is_qvar_qualified(_tmp12)){Cyc_Parse_err(
_tag_arr("struct field cannot be qualified with a namespace",sizeof(
unsigned char),50),loc);}return({struct Cyc_Absyn_Structfield*_tmp13=_cycalloc(
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
_tmp87=(void*)_tmp85->r;struct Cyc_Absyn_Structdecl*_tmp88;struct Cyc_Absyn_Tuniondecl*
_tmp89;struct Cyc_Absyn_Uniondecl*_tmp8A;struct Cyc_Absyn_Enumdecl*_tmp8B;_LL83:
if(*((int*)_tmp87)== 4){_LL8D: _tmp88=((struct Cyc_Absyn_Struct_d_struct*)_tmp87)->f1;
goto _LL84;}else{goto _LL85;}_LL85: if(*((int*)_tmp87)== 6){_LL8E: _tmp89=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp87)->f1;goto _LL86;}else{goto _LL87;}_LL87: if(*((
int*)_tmp87)== 5){_LL8F: _tmp8A=((struct Cyc_Absyn_Union_d_struct*)_tmp87)->f1;
goto _LL88;}else{goto _LL89;}_LL89: if(*((int*)_tmp87)== 7){_LL90: _tmp8B=((struct
Cyc_Absyn_Enum_d_struct*)_tmp87)->f1;goto _LL8A;}else{goto _LL8B;}_LL8B: goto _LL8C;
_LL84: {struct Cyc_List_List*_tmp8C=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmp88->tvs));t=(void*)({struct Cyc_Absyn_StructType_struct*
_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Absyn_StructType_struct
_tmp8E;_tmp8E.tag=10;_tmp8E.f1=(struct _tuple1*)((struct _tuple1*)((struct Cyc_Core_Opt*)
_check_null(_tmp88->name))->v);_tmp8E.f2=_tmp8C;_tmp8E.f3=0;_tmp8E;});_tmp8D;});
if(_tmp88->fields != 0){declopt=({struct Cyc_Core_Opt*_tmp8F=_cycalloc(sizeof(*
_tmp8F));_tmp8F->v=_tmp85;_tmp8F;});}goto _LL82;}_LL86: {struct Cyc_List_List*
_tmp90=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmp89->tvs));t=(void*)({struct Cyc_Absyn_TunionType_struct*_tmp91=_cycalloc(
sizeof(*_tmp91));_tmp91[0]=({struct Cyc_Absyn_TunionType_struct _tmp92;_tmp92.tag=
2;_tmp92.f1=({struct Cyc_Absyn_TunionInfo _tmp93;_tmp93.tunion_info=(void*)((void*)({
struct Cyc_Absyn_KnownTunion_struct*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94[0]=({
struct Cyc_Absyn_KnownTunion_struct _tmp95;_tmp95.tag=1;_tmp95.f1=({struct Cyc_Absyn_Tuniondecl**
_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[0]=_tmp89;_tmp96;});_tmp95;});_tmp94;}));
_tmp93.targs=_tmp90;_tmp93.rgn=(void*)((void*)2);_tmp93;});_tmp92;});_tmp91;});
if(_tmp89->fields != 0){declopt=({struct Cyc_Core_Opt*_tmp97=_cycalloc(sizeof(*
_tmp97));_tmp97->v=_tmp85;_tmp97;});}goto _LL82;}_LL88: {struct Cyc_List_List*
_tmp98=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmp8A->tvs));t=(void*)({struct Cyc_Absyn_UnionType_struct*_tmp99=_cycalloc(
sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Absyn_UnionType_struct _tmp9A;_tmp9A.tag=
11;_tmp9A.f1=(struct _tuple1*)((struct _tuple1*)((struct Cyc_Core_Opt*)_check_null(
_tmp8A->name))->v);_tmp9A.f2=_tmp98;_tmp9A.f3=0;_tmp9A;});_tmp99;});if(_tmp8A->fields
!= 0){declopt=({struct Cyc_Core_Opt*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->v=
_tmp85;_tmp9B;});}goto _LL82;}_LL8A: t=(void*)({struct Cyc_Absyn_EnumType_struct*
_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({struct Cyc_Absyn_EnumType_struct
_tmp9D;_tmp9D.tag=12;_tmp9D.f1=_tmp8B->name;_tmp9D.f2=0;_tmp9D;});_tmp9C;});
declopt=({struct Cyc_Core_Opt*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->v=_tmp85;
_tmp9E;});goto _LL82;_LL8C:({void*_tmp9F[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp85->loc,
_tag_arr("bad declaration within type specifier",sizeof(unsigned char),38),
_tag_arr(_tmp9F,sizeof(void*),0));});_LL82:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,
_tmp85->loc);}goto _LL69;_LL69:;}if(! seen_type){if(! seen_sign? ! seen_size: 0){({
void*_tmpA0[0]={};Cyc_Tcutil_warn(last_loc,_tag_arr("missing type within specifier",
sizeof(unsigned char),30),_tag_arr(_tmpA0,sizeof(void*),0));});}t=(void*)({
struct Cyc_Absyn_IntType_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1[0]=({
struct Cyc_Absyn_IntType_struct _tmpA2;_tmpA2.tag=5;_tmpA2.f1=(void*)sgn;_tmpA2.f2=(
void*)sz;_tmpA2;});_tmpA1;});}else{if(seen_sign){void*_tmpA3=t;void*_tmpA4;_LL92:
if((unsigned int)_tmpA3 > 3?*((int*)_tmpA3)== 5: 0){_LL96: _tmpA4=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmpA3)->f2;goto _LL93;}else{goto _LL94;}_LL94: goto _LL95;
_LL93: t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));
_tmpA5[0]=({struct Cyc_Absyn_IntType_struct _tmpA6;_tmpA6.tag=5;_tmpA6.f1=(void*)
sgn;_tmpA6.f2=(void*)_tmpA4;_tmpA6;});_tmpA5;});goto _LL91;_LL95: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type",sizeof(unsigned char),40),
last_loc);goto _LL91;_LL91:;}if(seen_size){void*_tmpA7=t;void*_tmpA8;_LL98: if((
unsigned int)_tmpA7 > 3?*((int*)_tmpA7)== 5: 0){_LL9E: _tmpA8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpA7)->f1;goto _LL99;}else{goto _LL9A;}_LL9A: if((unsigned int)_tmpA7 > 3?*((int*)
_tmpA7)== 6: 0){goto _LL9B;}else{goto _LL9C;}_LL9C: goto _LL9D;_LL99: t=(void*)({
struct Cyc_Absyn_IntType_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({
struct Cyc_Absyn_IntType_struct _tmpAA;_tmpAA.tag=5;_tmpAA.f1=(void*)_tmpA8;_tmpAA.f2=(
void*)sz;_tmpAA;});_tmpA9;});goto _LL97;_LL9B: t=(void*)({struct Cyc_Absyn_DoubleType_struct*
_tmpAB=_cycalloc_atomic(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpAC;_tmpAC.tag=6;_tmpAC.f1=1;_tmpAC;});_tmpAB;});goto _LL97;_LL9D: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type",sizeof(unsigned char),36),
last_loc);goto _LL97;_LL97:;}}return({struct _tuple4 _tmpAD;_tmpAD.f1=t;_tmpAD.f2=
declopt;_tmpAD;});}static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0){
return 0;}{struct Cyc_Parse_Declarator*_tmpAE=(struct Cyc_Parse_Declarator*)ds->hd;
struct _tuple1*_tmpAF=_tmpAE->id;struct Cyc_List_List*_tmpB1;struct Cyc_List_List*
_tmpB2;void*_tmpB3;struct Cyc_Absyn_Tqual _tmpB4;struct _tuple5 _tmpB0=Cyc_Parse_apply_tms(
tq,t,shared_atts,_tmpAE->tms);_LLA3: _tmpB4=_tmpB0.f1;goto _LLA2;_LLA2: _tmpB3=
_tmpB0.f2;goto _LLA1;_LLA1: _tmpB2=_tmpB0.f3;goto _LLA0;_LLA0: _tmpB1=_tmpB0.f4;goto
_LL9F;_LL9F: return({struct Cyc_List_List*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->hd=({
struct _tuple6*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->f1=_tmpAF;_tmpB6->f2=
_tmpB4;_tmpB6->f3=_tmpB3;_tmpB6->f4=_tmpB2;_tmpB6->f5=_tmpB1;_tmpB6;});_tmpB5->tl=
Cyc_Parse_apply_tmss(_tmpB4,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts);_tmpB5;});}}
static struct _tuple5 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0){return({struct _tuple5 _tmpB7;_tmpB7.f1=
tq;_tmpB7.f2=t;_tmpB7.f3=0;_tmpB7.f4=atts;_tmpB7;});}{void*_tmpB8=(void*)tms->hd;
struct Cyc_Absyn_Exp*_tmpB9;void*_tmpBA;struct Cyc_Position_Segment*_tmpBB;struct
Cyc_List_List*_tmpBC;struct Cyc_Absyn_Tqual _tmpBD;void*_tmpBE;void*_tmpBF;struct
Cyc_List_List*_tmpC0;struct Cyc_Position_Segment*_tmpC1;_LLA5: if((int)_tmpB8 == 0){
goto _LLA6;}else{goto _LLA7;}_LLA7: if((unsigned int)_tmpB8 > 1?*((int*)_tmpB8)== 0:
0){_LLB1: _tmpB9=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpB8)->f1;goto _LLA8;}
else{goto _LLA9;}_LLA9: if((unsigned int)_tmpB8 > 1?*((int*)_tmpB8)== 2: 0){_LLB2:
_tmpBA=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmpB8)->f1;goto _LLAA;}
else{goto _LLAB;}_LLAB: if((unsigned int)_tmpB8 > 1?*((int*)_tmpB8)== 3: 0){_LLB4:
_tmpBC=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpB8)->f1;goto _LLB3;_LLB3:
_tmpBB=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpB8)->f2;goto _LLAC;}else{goto
_LLAD;}_LLAD: if((unsigned int)_tmpB8 > 1?*((int*)_tmpB8)== 1: 0){_LLB7: _tmpBF=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpB8)->f1;goto _LLB6;_LLB6: _tmpBE=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpB8)->f2;goto _LLB5;_LLB5: _tmpBD=((
struct Cyc_Absyn_Pointer_mod_struct*)_tmpB8)->f3;goto _LLAE;}else{goto _LLAF;}_LLAF:
if((unsigned int)_tmpB8 > 1?*((int*)_tmpB8)== 4: 0){_LLB9: _tmpC1=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpB8)->f1;goto _LLB8;_LLB8: _tmpC0=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpB8)->f2;goto _LLB0;}else{goto _LLA4;}_LLA6: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),(
void*)({struct Cyc_Absyn_ArrayType_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));
_tmpC2[0]=({struct Cyc_Absyn_ArrayType_struct _tmpC3;_tmpC3.tag=7;_tmpC3.f1=(void*)
t;_tmpC3.f2=tq;_tmpC3.f3=0;_tmpC3;});_tmpC2;}),atts,tms->tl);_LLA8: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),(void*)({struct Cyc_Absyn_ArrayType_struct*_tmpC4=
_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_ArrayType_struct _tmpC5;
_tmpC5.tag=7;_tmpC5.f1=(void*)t;_tmpC5.f2=tq;_tmpC5.f3=(struct Cyc_Absyn_Exp*)
_tmpB9;_tmpC5;});_tmpC4;}),atts,tms->tl);_LLAA: {void*_tmpC6=_tmpBA;struct Cyc_List_List*
_tmpC7;struct Cyc_Core_Opt*_tmpC8;struct Cyc_Absyn_VarargInfo*_tmpC9;int _tmpCA;
struct Cyc_List_List*_tmpCB;struct Cyc_Position_Segment*_tmpCC;_LLBB: if(*((int*)
_tmpC6)== 1){_LLC3: _tmpCB=((struct Cyc_Absyn_WithTypes_struct*)_tmpC6)->f1;goto
_LLC2;_LLC2: _tmpCA=((struct Cyc_Absyn_WithTypes_struct*)_tmpC6)->f2;goto _LLC1;
_LLC1: _tmpC9=((struct Cyc_Absyn_WithTypes_struct*)_tmpC6)->f3;goto _LLC0;_LLC0:
_tmpC8=((struct Cyc_Absyn_WithTypes_struct*)_tmpC6)->f4;goto _LLBF;_LLBF: _tmpC7=((
struct Cyc_Absyn_WithTypes_struct*)_tmpC6)->f5;goto _LLBC;}else{goto _LLBD;}_LLBD:
if(*((int*)_tmpC6)== 0){_LLC4: _tmpCC=((struct Cyc_Absyn_NoTypes_struct*)_tmpC6)->f2;
goto _LLBE;}else{goto _LLBA;}_LLBC: {struct Cyc_List_List*typvars=0;struct Cyc_List_List*
fn_atts=0;struct Cyc_List_List*new_atts=0;{struct Cyc_List_List*as=atts;for(0;as != 
0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd)){fn_atts=({struct Cyc_List_List*
_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->hd=(void*)((void*)as->hd);_tmpCD->tl=
fn_atts;_tmpCD;});}else{new_atts=({struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*
_tmpCE));_tmpCE->hd=(void*)((void*)as->hd);_tmpCE->tl=new_atts;_tmpCE;});}}}if(
tms->tl != 0){void*_tmpCF=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;
struct Cyc_List_List*_tmpD0;_LLC6: if((unsigned int)_tmpCF > 1?*((int*)_tmpCF)== 3:
0){_LLCA: _tmpD0=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCF)->f1;goto _LLC7;}
else{goto _LLC8;}_LLC8: goto _LLC9;_LLC7: typvars=_tmpD0;tms=tms->tl;goto _LLC5;_LLC9:
goto _LLC5;_LLC5:;}if(((((! _tmpCA? _tmpC9 == 0: 0)? _tmpCB != 0: 0)? _tmpCB->tl == 0: 0)?(*((
struct _tuple2*)_tmpCB->hd)).f1 == 0: 0)?(*((struct _tuple2*)_tmpCB->hd)).f3 == (void*)
0: 0){_tmpCB=0;}t=Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpCB);return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),Cyc_Absyn_function_typ(typvars,_tmpC8,t,_tmpCB,_tmpCA,
_tmpC9,_tmpC7,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}
_LLBE:({void*_tmpD1[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmpCC,_tag_arr("function declaration without parameter types",
sizeof(unsigned char),45),_tag_arr(_tmpD1,sizeof(void*),0));});_LLBA:;}_LLAC: if(
tms->tl == 0){return({struct _tuple5 _tmpD2;_tmpD2.f1=tq;_tmpD2.f2=t;_tmpD2.f3=
_tmpBC;_tmpD2.f4=atts;_tmpD2;});}({void*_tmpD3[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmpBB,_tag_arr("type parameters must appear before function arguments in declarator",
sizeof(unsigned char),68),_tag_arr(_tmpD3,sizeof(void*),0));});_LLAE: {void*
_tmpD4=_tmpBF;struct Cyc_Absyn_Exp*_tmpD5;struct Cyc_Absyn_Exp*_tmpD6;_LLCC: if((
unsigned int)_tmpD4 > 1?*((int*)_tmpD4)== 0: 0){_LLD2: _tmpD5=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmpD4)->f1;goto _LLCD;}else{goto _LLCE;}_LLCE: if((unsigned int)_tmpD4 > 1?*((int*)
_tmpD4)== 1: 0){_LLD3: _tmpD6=((struct Cyc_Absyn_Nullable_ps_struct*)_tmpD4)->f1;
goto _LLCF;}else{goto _LLD0;}_LLD0: if((int)_tmpD4 == 0){goto _LLD1;}else{goto _LLCB;}
_LLCD: return Cyc_Parse_apply_tms(_tmpBD,Cyc_Absyn_atb_typ(t,_tmpBE,tq,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7[0]=({
struct Cyc_Absyn_Upper_b_struct _tmpD8;_tmpD8.tag=0;_tmpD8.f1=_tmpD5;_tmpD8;});
_tmpD7;})),atts,tms->tl);_LLCF: return Cyc_Parse_apply_tms(_tmpBD,Cyc_Absyn_starb_typ(
t,_tmpBE,tq,(void*)({struct Cyc_Absyn_Upper_b_struct*_tmpD9=_cycalloc(sizeof(*
_tmpD9));_tmpD9[0]=({struct Cyc_Absyn_Upper_b_struct _tmpDA;_tmpDA.tag=0;_tmpDA.f1=
_tmpD6;_tmpDA;});_tmpD9;})),atts,tms->tl);_LLD1: return Cyc_Parse_apply_tms(_tmpBD,
Cyc_Absyn_tagged_typ(t,_tmpBE,tq),atts,tms->tl);_LLCB:;}_LLB0: return Cyc_Parse_apply_tms(
tq,t,Cyc_List_append(atts,_tmpC0),tms->tl);_LLA4:;}}void*Cyc_Parse_speclist2typ(
struct Cyc_List_List*tss,struct Cyc_Position_Segment*loc){struct Cyc_Core_Opt*
_tmpDC;void*_tmpDD;struct _tuple4 _tmpDB=Cyc_Parse_collapse_type_specifiers(tss,
loc);_LLD6: _tmpDD=_tmpDB.f1;goto _LLD5;_LLD5: _tmpDC=_tmpDB.f2;goto _LLD4;_LLD4: if(
_tmpDC != 0){({void*_tmpDE[0]={};Cyc_Tcutil_warn(loc,_tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof(unsigned char),54),_tag_arr(_tmpDE,sizeof(void*),0));});}return _tmpDD;}
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple6*t){struct _tuple6 _tmpE0;struct Cyc_List_List*_tmpE1;struct Cyc_List_List*
_tmpE2;void*_tmpE3;struct Cyc_Absyn_Tqual _tmpE4;struct _tuple1*_tmpE5;struct
_tuple6*_tmpDF=t;_tmpE0=*_tmpDF;_LLDC: _tmpE5=_tmpE0.f1;goto _LLDB;_LLDB: _tmpE4=
_tmpE0.f2;goto _LLDA;_LLDA: _tmpE3=_tmpE0.f3;goto _LLD9;_LLD9: _tmpE2=_tmpE0.f4;goto
_LLD8;_LLD8: _tmpE1=_tmpE0.f5;goto _LLD7;_LLD7: Cyc_Lex_register_typedef(_tmpE5);
if(_tmpE1 != 0){Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmpE7;_tmpE7.tag=0;_tmpE7.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmpE1->hd);{void*_tmpE6[1]={& _tmpE7};Cyc_Std_aprintf(_tag_arr("bad attribute %s within typedef",
sizeof(unsigned char),32),_tag_arr(_tmpE6,sizeof(void*),1));}}),loc);}return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Typedef_d_struct*_tmpE8=_cycalloc(sizeof(*_tmpE8));
_tmpE8[0]=({struct Cyc_Absyn_Typedef_d_struct _tmpE9;_tmpE9.tag=8;_tmpE9.f1=({
struct Cyc_Absyn_Typedefdecl*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->name=
_tmpE5;_tmpEA->tvs=_tmpE2;_tmpEA->defn=(void*)_tmpE3;_tmpEA;});_tmpE9;});_tmpE8;}),
loc);}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,
struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpEC;_tmpEC.tag=12;_tmpEC.f1=d;_tmpEC.f2=s;_tmpEC;});_tmpEB;}),Cyc_Position_segment_join(
d->loc,s->loc));}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*loc){struct Cyc_List_List*
tss=ds->type_specs;struct Cyc_Absyn_Tqual tq=ds->tq;int istypedef=0;void*s=(void*)2;
struct Cyc_List_List*atts=ds->attributes;if(ds->is_inline){Cyc_Parse_err(_tag_arr("inline is allowed only on function definitions",
sizeof(unsigned char),47),loc);}if(tss == 0){Cyc_Parse_err(_tag_arr("missing type specifiers in declaration",
sizeof(unsigned char),39),loc);return 0;}if(ds->sc != 0){void*_tmpED=(void*)((
struct Cyc_Core_Opt*)_check_null(ds->sc))->v;_LLDE: if((int)_tmpED == 0){goto _LLDF;}
else{goto _LLE0;}_LLE0: if((int)_tmpED == 1){goto _LLE1;}else{goto _LLE2;}_LLE2: if((
int)_tmpED == 2){goto _LLE3;}else{goto _LLE4;}_LLE4: if((int)_tmpED == 3){goto _LLE5;}
else{goto _LLE6;}_LLE6: if((int)_tmpED == 4){goto _LLE7;}else{goto _LLE8;}_LLE8: if((
int)_tmpED == 5){goto _LLE9;}else{goto _LLEA;}_LLEA: if((int)_tmpED == 6){goto _LLEB;}
else{goto _LLDD;}_LLDF: istypedef=1;goto _LLDD;_LLE1: s=(void*)3;goto _LLDD;_LLE3: s=(
void*)4;goto _LLDD;_LLE5: s=(void*)0;goto _LLDD;_LLE7: s=(void*)2;goto _LLDD;_LLE9: s=(
void*)2;goto _LLDD;_LLEB: s=(void*)1;goto _LLDD;_LLDD:;}{struct Cyc_List_List*_tmpEF;
struct Cyc_List_List*_tmpF0;struct _tuple0 _tmpEE=((struct _tuple0(*)(struct Cyc_List_List*
x))Cyc_List_split)(ids);_LLEE: _tmpF0=_tmpEE.f1;goto _LLED;_LLED: _tmpEF=_tmpEE.f2;
goto _LLEC;_LLEC: {int exps_empty=1;{struct Cyc_List_List*es=_tmpEF;for(0;es != 0;es=
es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){exps_empty=0;break;}}}{struct
_tuple4 _tmpF1=Cyc_Parse_collapse_type_specifiers(tss,loc);if(_tmpF0 == 0){struct
Cyc_Core_Opt*_tmpF3;void*_tmpF4;struct _tuple4 _tmpF2=_tmpF1;_LLF1: _tmpF4=_tmpF2.f1;
goto _LLF0;_LLF0: _tmpF3=_tmpF2.f2;goto _LLEF;_LLEF: if(_tmpF3 != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)_tmpF3->v;{void*_tmpF5=(void*)d->r;struct Cyc_Absyn_Enumdecl*
_tmpF6;struct Cyc_Absyn_Structdecl*_tmpF7;struct Cyc_Absyn_Uniondecl*_tmpF8;struct
Cyc_Absyn_Tuniondecl*_tmpF9;_LLF3: if(*((int*)_tmpF5)== 7){_LLFD: _tmpF6=((struct
Cyc_Absyn_Enum_d_struct*)_tmpF5)->f1;goto _LLF4;}else{goto _LLF5;}_LLF5: if(*((int*)
_tmpF5)== 4){_LLFE: _tmpF7=((struct Cyc_Absyn_Struct_d_struct*)_tmpF5)->f1;goto
_LLF6;}else{goto _LLF7;}_LLF7: if(*((int*)_tmpF5)== 5){_LLFF: _tmpF8=((struct Cyc_Absyn_Union_d_struct*)
_tmpF5)->f1;goto _LLF8;}else{goto _LLF9;}_LLF9: if(*((int*)_tmpF5)== 6){_LL100:
_tmpF9=((struct Cyc_Absyn_Tunion_d_struct*)_tmpF5)->f1;goto _LLFA;}else{goto _LLFB;}
_LLFB: goto _LLFC;_LLF4:(void*)(_tmpF6->sc=(void*)s);if(atts != 0){Cyc_Parse_err(
_tag_arr("bad attributes on enum",sizeof(unsigned char),23),loc);}goto _LLF2;
_LLF6:(void*)(_tmpF7->sc=(void*)s);_tmpF7->attributes=atts;goto _LLF2;_LLF8:(void*)(
_tmpF8->sc=(void*)s);_tmpF8->attributes=atts;goto _LLF2;_LLFA:(void*)(_tmpF9->sc=(
void*)s);if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on tunion",sizeof(
unsigned char),25),loc);}goto _LLF2;_LLFC: Cyc_Parse_err(_tag_arr("bad declaration",
sizeof(unsigned char),16),loc);return 0;_LLF2:;}return({struct Cyc_List_List*
_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->hd=d;_tmpFA->tl=0;_tmpFA;});}else{void*
_tmpFB=_tmpF4;struct Cyc_List_List*_tmpFC;struct _tuple1*_tmpFD;struct Cyc_Absyn_TunionInfo
_tmpFE;void*_tmpFF;struct Cyc_Absyn_Tuniondecl**_tmp100;struct Cyc_Absyn_TunionInfo
_tmp101;struct Cyc_List_List*_tmp102;void*_tmp103;struct Cyc_Absyn_UnknownTunionInfo
_tmp104;int _tmp105;struct _tuple1*_tmp106;struct Cyc_List_List*_tmp107;struct
_tuple1*_tmp108;struct _tuple1*_tmp109;struct Cyc_List_List*_tmp10A;_LL102: if((
unsigned int)_tmpFB > 3?*((int*)_tmpFB)== 10: 0){_LL111: _tmpFD=((struct Cyc_Absyn_StructType_struct*)
_tmpFB)->f1;goto _LL110;_LL110: _tmpFC=((struct Cyc_Absyn_StructType_struct*)_tmpFB)->f2;
goto _LL103;}else{goto _LL104;}_LL104: if((unsigned int)_tmpFB > 3?*((int*)_tmpFB)== 
2: 0){_LL112: _tmpFE=((struct Cyc_Absyn_TunionType_struct*)_tmpFB)->f1;_LL113:
_tmpFF=(void*)_tmpFE.tunion_info;if(*((int*)_tmpFF)== 1){_LL114: _tmp100=((struct
Cyc_Absyn_KnownTunion_struct*)_tmpFF)->f1;goto _LL105;}else{goto _LL106;}}else{
goto _LL106;}_LL106: if((unsigned int)_tmpFB > 3?*((int*)_tmpFB)== 2: 0){_LL115:
_tmp101=((struct Cyc_Absyn_TunionType_struct*)_tmpFB)->f1;_LL117: _tmp103=(void*)
_tmp101.tunion_info;if(*((int*)_tmp103)== 0){_LL118: _tmp104=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp103)->f1;_LL11A: _tmp106=_tmp104.name;goto _LL119;_LL119: _tmp105=_tmp104.is_xtunion;
goto _LL116;}else{goto _LL108;}_LL116: _tmp102=_tmp101.targs;goto _LL107;}else{goto
_LL108;}_LL108: if((unsigned int)_tmpFB > 3?*((int*)_tmpFB)== 11: 0){_LL11C: _tmp108=((
struct Cyc_Absyn_UnionType_struct*)_tmpFB)->f1;goto _LL11B;_LL11B: _tmp107=((struct
Cyc_Absyn_UnionType_struct*)_tmpFB)->f2;goto _LL109;}else{goto _LL10A;}_LL10A: if((
unsigned int)_tmpFB > 3?*((int*)_tmpFB)== 12: 0){_LL11D: _tmp109=((struct Cyc_Absyn_EnumType_struct*)
_tmpFB)->f1;goto _LL10B;}else{goto _LL10C;}_LL10C: if((unsigned int)_tmpFB > 3?*((
int*)_tmpFB)== 16: 0){_LL11E: _tmp10A=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmpFB)->f1;goto _LL10D;}else{goto _LL10E;}_LL10E: goto _LL10F;_LL103: {struct Cyc_List_List*
_tmp10B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmpFC);struct Cyc_Absyn_Structdecl*_tmp10C=({struct Cyc_Absyn_Structdecl*
_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->sc=(void*)s;_tmp110->name=({struct
Cyc_Core_Opt*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->v=(struct _tuple1*)
_check_null(_tmpFD);_tmp111;});_tmp110->tvs=_tmp10B;_tmp110->fields=0;_tmp110->attributes=
0;_tmp110;});if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on struct",
sizeof(unsigned char),25),loc);}return({struct Cyc_List_List*_tmp10D=_cycalloc(
sizeof(*_tmp10D));_tmp10D->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Struct_d_struct*
_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=({struct Cyc_Absyn_Struct_d_struct
_tmp10F;_tmp10F.tag=4;_tmp10F.f1=_tmp10C;_tmp10F;});_tmp10E;}),loc);_tmp10D->tl=
0;_tmp10D;});}_LL105: if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on tunion",
sizeof(unsigned char),25),loc);}return({struct Cyc_List_List*_tmp112=_cycalloc(
sizeof(*_tmp112));_tmp112->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp114;_tmp114.tag=6;_tmp114.f1=*_tmp100;_tmp114;});_tmp113;}),loc);_tmp112->tl=
0;_tmp112;});_LL107: {struct Cyc_List_List*_tmp115=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp102);
struct Cyc_Absyn_Decl*_tmp116=Cyc_Absyn_tunion_decl(s,_tmp106,_tmp115,0,_tmp105,
loc);if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on tunion",sizeof(
unsigned char),25),loc);}return({struct Cyc_List_List*_tmp117=_cycalloc(sizeof(*
_tmp117));_tmp117->hd=_tmp116;_tmp117->tl=0;_tmp117;});}_LL109: {struct Cyc_List_List*
_tmp118=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp107);struct Cyc_Absyn_Uniondecl*_tmp119=({struct Cyc_Absyn_Uniondecl*
_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->sc=(void*)s;_tmp11E->name=({struct
Cyc_Core_Opt*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->v=(struct _tuple1*)
_check_null(_tmp108);_tmp11F;});_tmp11E->tvs=_tmp118;_tmp11E->fields=0;_tmp11E->attributes=
0;_tmp11E;});if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on union",
sizeof(unsigned char),24),loc);}return({struct Cyc_List_List*_tmp11A=_cycalloc(
sizeof(*_tmp11A));_tmp11A->hd=({struct Cyc_Absyn_Decl*_tmp11B=_cycalloc(sizeof(*
_tmp11B));_tmp11B->r=(void*)((void*)({struct Cyc_Absyn_Union_d_struct*_tmp11C=
_cycalloc(sizeof(*_tmp11C));_tmp11C[0]=({struct Cyc_Absyn_Union_d_struct _tmp11D;
_tmp11D.tag=5;_tmp11D.f1=_tmp119;_tmp11D;});_tmp11C;}));_tmp11B->loc=loc;_tmp11B;});
_tmp11A->tl=0;_tmp11A;});}_LL10B: {struct Cyc_Absyn_Enumdecl*_tmp120=({struct Cyc_Absyn_Enumdecl*
_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->sc=(void*)s;_tmp125->name=_tmp109;
_tmp125->fields=0;_tmp125;});if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on enum",
sizeof(unsigned char),23),loc);}return({struct Cyc_List_List*_tmp121=_cycalloc(
sizeof(*_tmp121));_tmp121->hd=({struct Cyc_Absyn_Decl*_tmp122=_cycalloc(sizeof(*
_tmp122));_tmp122->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp123=
_cycalloc(sizeof(*_tmp123));_tmp123[0]=({struct Cyc_Absyn_Enum_d_struct _tmp124;
_tmp124.tag=7;_tmp124.f1=_tmp120;_tmp124;});_tmp123;}));_tmp122->loc=loc;_tmp122;});
_tmp121->tl=0;_tmp121;});}_LL10D: {struct Cyc_Absyn_Enumdecl*_tmp126=({struct Cyc_Absyn_Enumdecl*
_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->sc=(void*)s;_tmp12B->name=Cyc_Parse_gensym_enum();
_tmp12B->fields=({struct Cyc_Core_Opt*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->v=
_tmp10A;_tmp12C;});_tmp12B;});if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on enum",
sizeof(unsigned char),23),loc);}return({struct Cyc_List_List*_tmp127=_cycalloc(
sizeof(*_tmp127));_tmp127->hd=({struct Cyc_Absyn_Decl*_tmp128=_cycalloc(sizeof(*
_tmp128));_tmp128->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp129=
_cycalloc(sizeof(*_tmp129));_tmp129[0]=({struct Cyc_Absyn_Enum_d_struct _tmp12A;
_tmp12A.tag=7;_tmp12A.f1=_tmp126;_tmp12A;});_tmp129;}));_tmp128->loc=loc;_tmp128;});
_tmp127->tl=0;_tmp127;});}_LL10F: Cyc_Parse_err(_tag_arr("missing declarator",
sizeof(unsigned char),19),loc);return 0;_LL101:;}}else{void*t=_tmpF1.f1;struct Cyc_List_List*
_tmp12D=Cyc_Parse_apply_tmss(tq,t,_tmpF0,atts);if(istypedef){if(! exps_empty){Cyc_Parse_err(
_tag_arr("initializer in typedef declaration",sizeof(unsigned char),35),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(
struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp12D);
if(_tmpF1.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(_tmpF1.f2)->v;{
void*_tmp12E=(void*)d->r;struct Cyc_Absyn_Structdecl*_tmp12F;struct Cyc_Absyn_Tuniondecl*
_tmp130;struct Cyc_Absyn_Uniondecl*_tmp131;struct Cyc_Absyn_Enumdecl*_tmp132;
_LL120: if(*((int*)_tmp12E)== 4){_LL12A: _tmp12F=((struct Cyc_Absyn_Struct_d_struct*)
_tmp12E)->f1;goto _LL121;}else{goto _LL122;}_LL122: if(*((int*)_tmp12E)== 6){_LL12B:
_tmp130=((struct Cyc_Absyn_Tunion_d_struct*)_tmp12E)->f1;goto _LL123;}else{goto
_LL124;}_LL124: if(*((int*)_tmp12E)== 5){_LL12C: _tmp131=((struct Cyc_Absyn_Union_d_struct*)
_tmp12E)->f1;goto _LL125;}else{goto _LL126;}_LL126: if(*((int*)_tmp12E)== 7){_LL12D:
_tmp132=((struct Cyc_Absyn_Enum_d_struct*)_tmp12E)->f1;goto _LL127;}else{goto
_LL128;}_LL128: goto _LL129;_LL121:(void*)(_tmp12F->sc=(void*)s);_tmp12F->attributes=
atts;atts=0;goto _LL11F;_LL123:(void*)(_tmp130->sc=(void*)s);goto _LL11F;_LL125:(
void*)(_tmp131->sc=(void*)s);goto _LL11F;_LL127:(void*)(_tmp132->sc=(void*)s);
goto _LL11F;_LL129: Cyc_Parse_err(_tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof(unsigned char),70),loc);goto _LL11F;_LL11F:;}decls=({struct Cyc_List_List*
_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->hd=d;_tmp133->tl=decls;_tmp133;});}
if(atts != 0){Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp135;_tmp135.tag=0;_tmp135.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp134[1]={& _tmp135};Cyc_Std_aprintf(_tag_arr("bad attribute %s in typedef",
sizeof(unsigned char),28),_tag_arr(_tmp134,sizeof(void*),1));}}),loc);}return
decls;}}else{if(_tmpF1.f2 != 0){Cyc_Parse_unimp(_tag_arr("nested type declaration within declarator",
sizeof(unsigned char),42),loc);}{struct Cyc_List_List*decls=0;{struct Cyc_List_List*
_tmp136=_tmp12D;for(0;_tmp136 != 0;(_tmp136=_tmp136->tl,_tmpEF=_tmpEF->tl)){
struct _tuple6 _tmp138;struct Cyc_List_List*_tmp139;struct Cyc_List_List*_tmp13A;
void*_tmp13B;struct Cyc_Absyn_Tqual _tmp13C;struct _tuple1*_tmp13D;struct _tuple6*
_tmp137=(struct _tuple6*)_tmp136->hd;_tmp138=*_tmp137;_LL133: _tmp13D=_tmp138.f1;
goto _LL132;_LL132: _tmp13C=_tmp138.f2;goto _LL131;_LL131: _tmp13B=_tmp138.f3;goto
_LL130;_LL130: _tmp13A=_tmp138.f4;goto _LL12F;_LL12F: _tmp139=_tmp138.f5;goto _LL12E;
_LL12E: if(_tmp13A != 0){({void*_tmp13E[0]={};Cyc_Tcutil_warn(loc,_tag_arr("bad type params, ignoring",
sizeof(unsigned char),26),_tag_arr(_tmp13E,sizeof(void*),0));});}if(_tmpEF == 0){({
void*_tmp13F[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Parse_abort)(loc,_tag_arr("unexpected NULL in parse!",
sizeof(unsigned char),26),_tag_arr(_tmp13F,sizeof(void*),0));});}{struct Cyc_Absyn_Exp*
_tmp140=(struct Cyc_Absyn_Exp*)_tmpEF->hd;struct Cyc_Absyn_Vardecl*_tmp141=Cyc_Absyn_new_vardecl(
_tmp13D,_tmp13B,_tmp140);_tmp141->tq=_tmp13C;(void*)(_tmp141->sc=(void*)s);
_tmp141->attributes=_tmp139;{struct Cyc_Absyn_Decl*_tmp142=({struct Cyc_Absyn_Decl*
_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145[0]=({struct Cyc_Absyn_Var_d_struct
_tmp146;_tmp146.tag=0;_tmp146.f1=_tmp141;_tmp146;});_tmp145;}));_tmp144->loc=loc;
_tmp144;});decls=({struct Cyc_List_List*_tmp143=_cycalloc(sizeof(*_tmp143));
_tmp143->hd=_tmp142;_tmp143->tl=decls;_tmp143;});}}}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);}}}}}}}static void*Cyc_Parse_id_to_kind(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){if(Cyc_Std_strlen(s)== 1){
switch(*((const unsigned char*)_check_unknown_subscript(s,sizeof(unsigned char),0))){
case 'A': _LL134: return(void*)0;case 'M': _LL135: return(void*)1;case 'B': _LL136:
return(void*)2;case 'R': _LL137: return(void*)3;case 'E': _LL138: return(void*)4;
default: _LL139: break;}}Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp148;_tmp148.tag=0;_tmp148.f1=(struct _tagged_arr)s;{void*_tmp147[1]={& _tmp148};
Cyc_Std_aprintf(_tag_arr("bad kind: %s",sizeof(unsigned char),13),_tag_arr(
_tmp147,sizeof(void*),1));}}),loc);return(void*)2;}static struct Cyc_List_List*Cyc_Parse_attopt_to_tms(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
if(atts == 0){return tms;}else{return({struct Cyc_List_List*_tmp149=_cycalloc(
sizeof(*_tmp149));_tmp149->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp14B;_tmp14B.tag=4;_tmp14B.f1=loc;_tmp14B.f2=atts;_tmp14B;});_tmp14A;}));
_tmp149->tl=tms;_tmp149;});}}unsigned char Cyc_AbstractDeclarator_tok[27]="\000\000\000\000AbstractDeclarator_tok";
struct Cyc_AbstractDeclarator_tok_struct{unsigned char*tag;struct Cyc_Parse_Abstractdeclarator*
f1;};unsigned char Cyc_AttributeList_tok[22]="\000\000\000\000AttributeList_tok";
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
unsigned char Cyc_StructFieldDeclListList_tok[32]="\000\000\000\000StructFieldDeclListList_tok";
struct Cyc_StructFieldDeclListList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};unsigned char Cyc_StructFieldDeclList_tok[28]="\000\000\000\000StructFieldDeclList_tok";
struct Cyc_StructFieldDeclList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};unsigned char Cyc_StructOrUnion_tok[22]="\000\000\000\000StructOrUnion_tok";
struct Cyc_StructOrUnion_tok_struct{unsigned char*tag;enum Cyc_Parse_Struct_or_union
f1;};unsigned char Cyc_SwitchCClauseList_tok[26]="\000\000\000\000SwitchCClauseList_tok";
struct Cyc_SwitchCClauseList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;
};unsigned char Cyc_SwitchClauseList_tok[25]="\000\000\000\000SwitchClauseList_tok";
struct Cyc_SwitchClauseList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;}
;unsigned char Cyc_TunionFieldList_tok[24]="\000\000\000\000TunionFieldList_tok";
struct Cyc_TunionFieldList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};
unsigned char Cyc_TunionField_tok[20]="\000\000\000\000TunionField_tok";struct Cyc_TunionField_tok_struct{
unsigned char*tag;struct Cyc_Absyn_Tunionfield*f1;};unsigned char Cyc_TypeList_tok[
17]="\000\000\000\000TypeList_tok";struct Cyc_TypeList_tok_struct{unsigned char*
tag;struct Cyc_List_List*f1;};unsigned char Cyc_TypeModifierList_tok[25]="\000\000\000\000TypeModifierList_tok";
struct Cyc_TypeModifierList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;}
;unsigned char Cyc_TypeOpt_tok[16]="\000\000\000\000TypeOpt_tok";struct Cyc_TypeOpt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};unsigned char Cyc_TypeQual_tok[17]="\000\000\000\000TypeQual_tok";
struct Cyc_TypeQual_tok_struct{unsigned char*tag;struct Cyc_Absyn_Tqual f1;};
unsigned char Cyc_TypeSpecifier_tok[22]="\000\000\000\000TypeSpecifier_tok";
struct Cyc_TypeSpecifier_tok_struct{unsigned char*tag;void*f1;};unsigned char Cyc_Type_tok[
13]="\000\000\000\000Type_tok";struct Cyc_Type_tok_struct{unsigned char*tag;void*
f1;};unsigned char Cyc_YY1[8]="\000\000\000\000YY1";struct Cyc_YY1_struct{
unsigned char*tag;struct _tuple13*f1;};unsigned char Cyc_YYINITIALSVAL[18]="\000\000\000\000YYINITIALSVAL";
unsigned char Cyc_Okay_tok[13]="\000\000\000\000Okay_tok";static unsigned char
_tmp14D[15]="$(sign_t,int)@";static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{_tmp14D,_tmp14D,_tmp14D + 15}};struct _tuple12*Cyc_yyget_Int_tok(
void*yy1){struct _tuple12*yyzzz;{void*_tmp14E=yy1;struct _tuple12*_tmp14F;_LL13C:
if(*((void**)_tmp14E)== Cyc_Int_tok){_LL140: _tmp14F=((struct Cyc_Int_tok_struct*)
_tmp14E)->f1;goto _LL13D;}else{goto _LL13E;}_LL13E: goto _LL13F;_LL13D: yyzzz=_tmp14F;
goto _LL13B;_LL13F:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LL13B:;}return yyzzz;}
static unsigned char _tmp151[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp151,_tmp151,_tmp151 + 5}};unsigned char Cyc_yyget_Char_tok(
void*yy1){unsigned char yyzzz;{void*_tmp152=yy1;unsigned char _tmp153;_LL142: if(*((
void**)_tmp152)== Cyc_Char_tok){_LL146: _tmp153=((struct Cyc_Char_tok_struct*)
_tmp152)->f1;goto _LL143;}else{goto _LL144;}_LL144: goto _LL145;_LL143: yyzzz=_tmp153;
goto _LL141;_LL145:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LL141:;}return yyzzz;}
static unsigned char _tmp155[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp155,_tmp155,_tmp155 + 9}};struct _tagged_arr Cyc_yyget_String_tok(
void*yy1){struct _tagged_arr yyzzz;{void*_tmp156=yy1;struct _tagged_arr _tmp157;
_LL148: if(*((void**)_tmp156)== Cyc_String_tok){_LL14C: _tmp157=((struct Cyc_String_tok_struct*)
_tmp156)->f1;goto _LL149;}else{goto _LL14A;}_LL14A: goto _LL14B;_LL149: yyzzz=_tmp157;
goto _LL147;_LL14B:(int)_throw((void*)& Cyc_yyfail_String_tok);_LL147:;}return
yyzzz;}static unsigned char _tmp159[20]="tunion Pointer_Sort";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={Cyc_Core_Failure,{_tmp159,_tmp159,_tmp159 + 20}};
void*Cyc_yyget_Pointer_Sort_tok(void*yy1){void*yyzzz;{void*_tmp15A=yy1;void*
_tmp15B;_LL14E: if(*((void**)_tmp15A)== Cyc_Pointer_Sort_tok){_LL152: _tmp15B=(
void*)((struct Cyc_Pointer_Sort_tok_struct*)_tmp15A)->f1;goto _LL14F;}else{goto
_LL150;}_LL150: goto _LL151;_LL14F: yyzzz=_tmp15B;goto _LL14D;_LL151:(int)_throw((
void*)& Cyc_yyfail_Pointer_Sort_tok);_LL14D:;}return yyzzz;}static unsigned char
_tmp15D[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{
_tmp15D,_tmp15D,_tmp15D + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(void*yy1){
struct Cyc_Absyn_Exp*yyzzz;{void*_tmp15E=yy1;struct Cyc_Absyn_Exp*_tmp15F;_LL154:
if(*((void**)_tmp15E)== Cyc_Exp_tok){_LL158: _tmp15F=((struct Cyc_Exp_tok_struct*)
_tmp15E)->f1;goto _LL155;}else{goto _LL156;}_LL156: goto _LL157;_LL155: yyzzz=_tmp15F;
goto _LL153;_LL157:(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LL153:;}return yyzzz;}
static unsigned char _tmp161[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={Cyc_Core_Failure,{_tmp161,_tmp161,_tmp161 + 14}};struct
Cyc_List_List*Cyc_yyget_ExpList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp162=yy1;struct Cyc_List_List*_tmp163;_LL15A: if(*((void**)_tmp162)== Cyc_ExpList_tok){
_LL15E: _tmp163=((struct Cyc_ExpList_tok_struct*)_tmp162)->f1;goto _LL15B;}else{
goto _LL15C;}_LL15C: goto _LL15D;_LL15B: yyzzz=_tmp163;goto _LL159;_LL15D:(int)_throw((
void*)& Cyc_yyfail_ExpList_tok);_LL159:;}return yyzzz;}static unsigned char _tmp165[
39]="list_t<$(list_t<designator_t>,exp_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{_tmp165,_tmp165,_tmp165 + 39}};
struct Cyc_List_List*Cyc_yyget_InitializerList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp166=yy1;struct Cyc_List_List*_tmp167;_LL160: if(*((void**)_tmp166)
== Cyc_InitializerList_tok){_LL164: _tmp167=((struct Cyc_InitializerList_tok_struct*)
_tmp166)->f1;goto _LL161;}else{goto _LL162;}_LL162: goto _LL163;_LL161: yyzzz=_tmp167;
goto _LL15F;_LL163:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);_LL15F:;}
return yyzzz;}static unsigned char _tmp169[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={Cyc_Core_Failure,{_tmp169,_tmp169,_tmp169 + 9}};void*Cyc_yyget_Primop_tok(
void*yy1){void*yyzzz;{void*_tmp16A=yy1;void*_tmp16B;_LL166: if(*((void**)_tmp16A)
== Cyc_Primop_tok){_LL16A: _tmp16B=(void*)((struct Cyc_Primop_tok_struct*)_tmp16A)->f1;
goto _LL167;}else{goto _LL168;}_LL168: goto _LL169;_LL167: yyzzz=_tmp16B;goto _LL165;
_LL169:(int)_throw((void*)& Cyc_yyfail_Primop_tok);_LL165:;}return yyzzz;}static
unsigned char _tmp16D[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp16D,_tmp16D,_tmp16D + 16}};struct
Cyc_Core_Opt*Cyc_yyget_Primopopt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp16E=yy1;struct Cyc_Core_Opt*_tmp16F;_LL16C: if(*((void**)_tmp16E)== Cyc_Primopopt_tok){
_LL170: _tmp16F=((struct Cyc_Primopopt_tok_struct*)_tmp16E)->f1;goto _LL16D;}else{
goto _LL16E;}_LL16E: goto _LL16F;_LL16D: yyzzz=_tmp16F;goto _LL16B;_LL16F:(int)_throw((
void*)& Cyc_yyfail_Primopopt_tok);_LL16B:;}return yyzzz;}static unsigned char
_tmp171[7]="qvar_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{_tmp171,_tmp171,_tmp171 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(
void*yy1){struct _tuple1*yyzzz;{void*_tmp172=yy1;struct _tuple1*_tmp173;_LL172: if(*((
void**)_tmp172)== Cyc_QualId_tok){_LL176: _tmp173=((struct Cyc_QualId_tok_struct*)
_tmp172)->f1;goto _LL173;}else{goto _LL174;}_LL174: goto _LL175;_LL173: yyzzz=_tmp173;
goto _LL171;_LL175:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL171:;}return
yyzzz;}static unsigned char _tmp175[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp175,_tmp175,_tmp175 + 7}};struct Cyc_Absyn_Stmt*
Cyc_yyget_Stmt_tok(void*yy1){struct Cyc_Absyn_Stmt*yyzzz;{void*_tmp176=yy1;struct
Cyc_Absyn_Stmt*_tmp177;_LL178: if(*((void**)_tmp176)== Cyc_Stmt_tok){_LL17C:
_tmp177=((struct Cyc_Stmt_tok_struct*)_tmp176)->f1;goto _LL179;}else{goto _LL17A;}
_LL17A: goto _LL17B;_LL179: yyzzz=_tmp177;goto _LL177;_LL17B:(int)_throw((void*)& Cyc_yyfail_Stmt_tok);
_LL177:;}return yyzzz;}static unsigned char _tmp179[24]="list_t<switch_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={Cyc_Core_Failure,{
_tmp179,_tmp179,_tmp179 + 24}};struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp17A=yy1;struct Cyc_List_List*
_tmp17B;_LL17E: if(*((void**)_tmp17A)== Cyc_SwitchClauseList_tok){_LL182: _tmp17B=((
struct Cyc_SwitchClauseList_tok_struct*)_tmp17A)->f1;goto _LL17F;}else{goto _LL180;}
_LL180: goto _LL181;_LL17F: yyzzz=_tmp17B;goto _LL17D;_LL181:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL17D:;}return yyzzz;}static unsigned char _tmp17D[25]="list_t<switchC_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={Cyc_Core_Failure,{
_tmp17D,_tmp17D,_tmp17D + 25}};struct Cyc_List_List*Cyc_yyget_SwitchCClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp17E=yy1;struct Cyc_List_List*
_tmp17F;_LL184: if(*((void**)_tmp17E)== Cyc_SwitchCClauseList_tok){_LL188: _tmp17F=((
struct Cyc_SwitchCClauseList_tok_struct*)_tmp17E)->f1;goto _LL185;}else{goto _LL186;}
_LL186: goto _LL187;_LL185: yyzzz=_tmp17F;goto _LL183;_LL187:(int)_throw((void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL183:;}return yyzzz;}static unsigned char _tmp181[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp181,_tmp181,_tmp181 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp182=yy1;
struct Cyc_Absyn_Pat*_tmp183;_LL18A: if(*((void**)_tmp182)== Cyc_Pattern_tok){
_LL18E: _tmp183=((struct Cyc_Pattern_tok_struct*)_tmp182)->f1;goto _LL18B;}else{
goto _LL18C;}_LL18C: goto _LL18D;_LL18B: yyzzz=_tmp183;goto _LL189;_LL18D:(int)_throw((
void*)& Cyc_yyfail_Pattern_tok);_LL189:;}return yyzzz;}static unsigned char _tmp185[
14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure,{_tmp185,_tmp185,_tmp185 + 14}};struct Cyc_List_List*Cyc_yyget_PatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp186=yy1;struct Cyc_List_List*
_tmp187;_LL190: if(*((void**)_tmp186)== Cyc_PatternList_tok){_LL194: _tmp187=((
struct Cyc_PatternList_tok_struct*)_tmp186)->f1;goto _LL191;}else{goto _LL192;}
_LL192: goto _LL193;_LL191: yyzzz=_tmp187;goto _LL18F;_LL193:(int)_throw((void*)& Cyc_yyfail_PatternList_tok);
_LL18F:;}return yyzzz;}static unsigned char _tmp189[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{
_tmp189,_tmp189,_tmp189 + 31}};struct _tuple10*Cyc_yyget_FieldPattern_tok(void*yy1){
struct _tuple10*yyzzz;{void*_tmp18A=yy1;struct _tuple10*_tmp18B;_LL196: if(*((void**)
_tmp18A)== Cyc_FieldPattern_tok){_LL19A: _tmp18B=((struct Cyc_FieldPattern_tok_struct*)
_tmp18A)->f1;goto _LL197;}else{goto _LL198;}_LL198: goto _LL199;_LL197: yyzzz=_tmp18B;
goto _LL195;_LL199:(int)_throw((void*)& Cyc_yyfail_FieldPattern_tok);_LL195:;}
return yyzzz;}static unsigned char _tmp18D[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp18D,_tmp18D,_tmp18D + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp18E=yy1;struct Cyc_List_List*
_tmp18F;_LL19C: if(*((void**)_tmp18E)== Cyc_FieldPatternList_tok){_LL1A0: _tmp18F=((
struct Cyc_FieldPatternList_tok_struct*)_tmp18E)->f1;goto _LL19D;}else{goto _LL19E;}
_LL19E: goto _LL19F;_LL19D: yyzzz=_tmp18F;goto _LL19B;_LL19F:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL19B:;}return yyzzz;}static unsigned char _tmp191[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={Cyc_Core_Failure,{_tmp191,_tmp191,_tmp191 + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok(void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp192=yy1;
struct Cyc_Absyn_Fndecl*_tmp193;_LL1A2: if(*((void**)_tmp192)== Cyc_FnDecl_tok){
_LL1A6: _tmp193=((struct Cyc_FnDecl_tok_struct*)_tmp192)->f1;goto _LL1A3;}else{goto
_LL1A4;}_LL1A4: goto _LL1A5;_LL1A3: yyzzz=_tmp193;goto _LL1A1;_LL1A5:(int)_throw((
void*)& Cyc_yyfail_FnDecl_tok);_LL1A1:;}return yyzzz;}static unsigned char _tmp195[
15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={
Cyc_Core_Failure,{_tmp195,_tmp195,_tmp195 + 15}};struct Cyc_List_List*Cyc_yyget_DeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp196=yy1;struct Cyc_List_List*
_tmp197;_LL1A8: if(*((void**)_tmp196)== Cyc_DeclList_tok){_LL1AC: _tmp197=((struct
Cyc_DeclList_tok_struct*)_tmp196)->f1;goto _LL1A9;}else{goto _LL1AA;}_LL1AA: goto
_LL1AB;_LL1A9: yyzzz=_tmp197;goto _LL1A7;_LL1AB:(int)_throw((void*)& Cyc_yyfail_DeclList_tok);
_LL1A7:;}return yyzzz;}static unsigned char _tmp199[12]="decl_spec_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={Cyc_Core_Failure,{_tmp199,
_tmp199,_tmp199 + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(
void*yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{void*_tmp19A=yy1;struct Cyc_Parse_Declaration_spec*
_tmp19B;_LL1AE: if(*((void**)_tmp19A)== Cyc_DeclSpec_tok){_LL1B2: _tmp19B=((struct
Cyc_DeclSpec_tok_struct*)_tmp19A)->f1;goto _LL1AF;}else{goto _LL1B0;}_LL1B0: goto
_LL1B1;_LL1AF: yyzzz=_tmp19B;goto _LL1AD;_LL1B1:(int)_throw((void*)& Cyc_yyfail_DeclSpec_tok);
_LL1AD:;}return yyzzz;}static unsigned char _tmp19D[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{
_tmp19D,_tmp19D,_tmp19D + 27}};struct _tuple11*Cyc_yyget_InitDecl_tok(void*yy1){
struct _tuple11*yyzzz;{void*_tmp19E=yy1;struct _tuple11*_tmp19F;_LL1B4: if(*((void**)
_tmp19E)== Cyc_InitDecl_tok){_LL1B8: _tmp19F=((struct Cyc_InitDecl_tok_struct*)
_tmp19E)->f1;goto _LL1B5;}else{goto _LL1B6;}_LL1B6: goto _LL1B7;_LL1B5: yyzzz=_tmp19F;
goto _LL1B3;_LL1B7:(int)_throw((void*)& Cyc_yyfail_InitDecl_tok);_LL1B3:;}return
yyzzz;}static unsigned char _tmp1A1[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp1A1,_tmp1A1,_tmp1A1 + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1A2=yy1;struct Cyc_List_List*
_tmp1A3;_LL1BA: if(*((void**)_tmp1A2)== Cyc_InitDeclList_tok){_LL1BE: _tmp1A3=((
struct Cyc_InitDeclList_tok_struct*)_tmp1A2)->f1;goto _LL1BB;}else{goto _LL1BC;}
_LL1BC: goto _LL1BD;_LL1BB: yyzzz=_tmp1A3;goto _LL1B9;_LL1BD:(int)_throw((void*)& Cyc_yyfail_InitDeclList_tok);
_LL1B9:;}return yyzzz;}static unsigned char _tmp1A5[16]="storage_class_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{
_tmp1A5,_tmp1A5,_tmp1A5 + 16}};void*Cyc_yyget_StorageClass_tok(void*yy1){void*
yyzzz;{void*_tmp1A6=yy1;void*_tmp1A7;_LL1C0: if(*((void**)_tmp1A6)== Cyc_StorageClass_tok){
_LL1C4: _tmp1A7=(void*)((struct Cyc_StorageClass_tok_struct*)_tmp1A6)->f1;goto
_LL1C1;}else{goto _LL1C2;}_LL1C2: goto _LL1C3;_LL1C1: yyzzz=_tmp1A7;goto _LL1BF;
_LL1C3:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);_LL1BF:;}return yyzzz;}
static unsigned char _tmp1A9[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{_tmp1A9,_tmp1A9,_tmp1A9 + 17}};
void*Cyc_yyget_TypeSpecifier_tok(void*yy1){void*yyzzz;{void*_tmp1AA=yy1;void*
_tmp1AB;_LL1C6: if(*((void**)_tmp1AA)== Cyc_TypeSpecifier_tok){_LL1CA: _tmp1AB=(
void*)((struct Cyc_TypeSpecifier_tok_struct*)_tmp1AA)->f1;goto _LL1C7;}else{goto
_LL1C8;}_LL1C8: goto _LL1C9;_LL1C7: yyzzz=_tmp1AB;goto _LL1C5;_LL1C9:(int)_throw((
void*)& Cyc_yyfail_TypeSpecifier_tok);_LL1C5:;}return yyzzz;}static unsigned char
_tmp1AD[18]="struct_or_union_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={
Cyc_Core_Failure,{_tmp1AD,_tmp1AD,_tmp1AD + 18}};enum Cyc_Parse_Struct_or_union
Cyc_yyget_StructOrUnion_tok(void*yy1){enum Cyc_Parse_Struct_or_union yyzzz;{void*
_tmp1AE=yy1;enum Cyc_Parse_Struct_or_union _tmp1AF;_LL1CC: if(*((void**)_tmp1AE)== 
Cyc_StructOrUnion_tok){_LL1D0: _tmp1AF=((struct Cyc_StructOrUnion_tok_struct*)
_tmp1AE)->f1;goto _LL1CD;}else{goto _LL1CE;}_LL1CE: goto _LL1CF;_LL1CD: yyzzz=_tmp1AF;
goto _LL1CB;_LL1CF:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);_LL1CB:;}
return yyzzz;}static unsigned char _tmp1B1[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp1B1,_tmp1B1,_tmp1B1 + 8}};struct
Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*
_tmp1B2=yy1;struct Cyc_Absyn_Tqual _tmp1B3;_LL1D2: if(*((void**)_tmp1B2)== Cyc_TypeQual_tok){
_LL1D6: _tmp1B3=((struct Cyc_TypeQual_tok_struct*)_tmp1B2)->f1;goto _LL1D3;}else{
goto _LL1D4;}_LL1D4: goto _LL1D5;_LL1D3: yyzzz=_tmp1B3;goto _LL1D1;_LL1D5:(int)_throw((
void*)& Cyc_yyfail_TypeQual_tok);_LL1D1:;}return yyzzz;}static unsigned char _tmp1B5[
22]="list_t<structfield_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={
Cyc_Core_Failure,{_tmp1B5,_tmp1B5,_tmp1B5 + 22}};struct Cyc_List_List*Cyc_yyget_StructFieldDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B6=yy1;struct Cyc_List_List*
_tmp1B7;_LL1D8: if(*((void**)_tmp1B6)== Cyc_StructFieldDeclList_tok){_LL1DC:
_tmp1B7=((struct Cyc_StructFieldDeclList_tok_struct*)_tmp1B6)->f1;goto _LL1D9;}
else{goto _LL1DA;}_LL1DA: goto _LL1DB;_LL1D9: yyzzz=_tmp1B7;goto _LL1D7;_LL1DB:(int)
_throw((void*)& Cyc_yyfail_StructFieldDeclList_tok);_LL1D7:;}return yyzzz;}static
unsigned char _tmp1B9[30]="list_t<list_t<structfield_t>>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclListList_tok={Cyc_Core_Failure,{_tmp1B9,_tmp1B9,
_tmp1B9 + 30}};struct Cyc_List_List*Cyc_yyget_StructFieldDeclListList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp1BA=yy1;struct Cyc_List_List*_tmp1BB;_LL1DE:
if(*((void**)_tmp1BA)== Cyc_StructFieldDeclListList_tok){_LL1E2: _tmp1BB=((struct
Cyc_StructFieldDeclListList_tok_struct*)_tmp1BA)->f1;goto _LL1DF;}else{goto _LL1E0;}
_LL1E0: goto _LL1E1;_LL1DF: yyzzz=_tmp1BB;goto _LL1DD;_LL1E1:(int)_throw((void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL1DD:;}return yyzzz;}static unsigned char _tmp1BD[24]="list_t<type_modifier_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{
_tmp1BD,_tmp1BD,_tmp1BD + 24}};struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1BE=yy1;struct Cyc_List_List*
_tmp1BF;_LL1E4: if(*((void**)_tmp1BE)== Cyc_TypeModifierList_tok){_LL1E8: _tmp1BF=((
struct Cyc_TypeModifierList_tok_struct*)_tmp1BE)->f1;goto _LL1E5;}else{goto _LL1E6;}
_LL1E6: goto _LL1E7;_LL1E5: yyzzz=_tmp1BF;goto _LL1E3;_LL1E7:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);
_LL1E3:;}return yyzzz;}static unsigned char _tmp1C1[13]="declarator_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp1C1,
_tmp1C1,_tmp1C1 + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_Declarator_tok(void*
yy1){struct Cyc_Parse_Declarator*yyzzz;{void*_tmp1C2=yy1;struct Cyc_Parse_Declarator*
_tmp1C3;_LL1EA: if(*((void**)_tmp1C2)== Cyc_Declarator_tok){_LL1EE: _tmp1C3=((
struct Cyc_Declarator_tok_struct*)_tmp1C2)->f1;goto _LL1EB;}else{goto _LL1EC;}
_LL1EC: goto _LL1ED;_LL1EB: yyzzz=_tmp1C3;goto _LL1E9;_LL1ED:(int)_throw((void*)& Cyc_yyfail_Declarator_tok);
_LL1E9:;}return yyzzz;}static unsigned char _tmp1C5[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{
_tmp1C5,_tmp1C5,_tmp1C5 + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(
void*yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp1C6=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp1C7;_LL1F0: if(*((void**)_tmp1C6)== Cyc_AbstractDeclarator_tok){_LL1F4:
_tmp1C7=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp1C6)->f1;goto _LL1F1;}
else{goto _LL1F2;}_LL1F2: goto _LL1F3;_LL1F1: yyzzz=_tmp1C7;goto _LL1EF;_LL1F3:(int)
_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);_LL1EF:;}return yyzzz;}static
unsigned char _tmp1C9[5]="bool";static struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={
Cyc_Core_Failure,{_tmp1C9,_tmp1C9,_tmp1C9 + 5}};int Cyc_yyget_Bool_tok(void*yy1){
int yyzzz;{void*_tmp1CA=yy1;int _tmp1CB;_LL1F6: if(*((void**)_tmp1CA)== Cyc_Bool_tok){
_LL1FA: _tmp1CB=((struct Cyc_Bool_tok_struct*)_tmp1CA)->f1;goto _LL1F7;}else{goto
_LL1F8;}_LL1F8: goto _LL1F9;_LL1F7: yyzzz=_tmp1CB;goto _LL1F5;_LL1F9:(int)_throw((
void*)& Cyc_yyfail_Bool_tok);_LL1F5:;}return yyzzz;}static unsigned char _tmp1CD[8]="scope_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Scope_tok={Cyc_Core_Failure,{
_tmp1CD,_tmp1CD,_tmp1CD + 8}};void*Cyc_yyget_Scope_tok(void*yy1){void*yyzzz;{void*
_tmp1CE=yy1;void*_tmp1CF;_LL1FC: if(*((void**)_tmp1CE)== Cyc_Scope_tok){_LL200:
_tmp1CF=(void*)((struct Cyc_Scope_tok_struct*)_tmp1CE)->f1;goto _LL1FD;}else{goto
_LL1FE;}_LL1FE: goto _LL1FF;_LL1FD: yyzzz=_tmp1CF;goto _LL1FB;_LL1FF:(int)_throw((
void*)& Cyc_yyfail_Scope_tok);_LL1FB:;}return yyzzz;}static unsigned char _tmp1D1[14]="tunionfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={Cyc_Core_Failure,{
_tmp1D1,_tmp1D1,_tmp1D1 + 14}};struct Cyc_Absyn_Tunionfield*Cyc_yyget_TunionField_tok(
void*yy1){struct Cyc_Absyn_Tunionfield*yyzzz;{void*_tmp1D2=yy1;struct Cyc_Absyn_Tunionfield*
_tmp1D3;_LL202: if(*((void**)_tmp1D2)== Cyc_TunionField_tok){_LL206: _tmp1D3=((
struct Cyc_TunionField_tok_struct*)_tmp1D2)->f1;goto _LL203;}else{goto _LL204;}
_LL204: goto _LL205;_LL203: yyzzz=_tmp1D3;goto _LL201;_LL205:(int)_throw((void*)& Cyc_yyfail_TunionField_tok);
_LL201:;}return yyzzz;}static unsigned char _tmp1D5[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{
_tmp1D5,_tmp1D5,_tmp1D5 + 22}};struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1D6=yy1;struct Cyc_List_List*
_tmp1D7;_LL208: if(*((void**)_tmp1D6)== Cyc_TunionFieldList_tok){_LL20C: _tmp1D7=((
struct Cyc_TunionFieldList_tok_struct*)_tmp1D6)->f1;goto _LL209;}else{goto _LL20A;}
_LL20A: goto _LL20B;_LL209: yyzzz=_tmp1D7;goto _LL207;_LL20B:(int)_throw((void*)& Cyc_yyfail_TunionFieldList_tok);
_LL207:;}return yyzzz;}static unsigned char _tmp1D9[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{
_tmp1D9,_tmp1D9,_tmp1D9 + 50}};struct _tuple14*Cyc_yyget_QualSpecList_tok(void*yy1){
struct _tuple14*yyzzz;{void*_tmp1DA=yy1;struct _tuple14*_tmp1DB;_LL20E: if(*((void**)
_tmp1DA)== Cyc_QualSpecList_tok){_LL212: _tmp1DB=((struct Cyc_QualSpecList_tok_struct*)
_tmp1DA)->f1;goto _LL20F;}else{goto _LL210;}_LL210: goto _LL211;_LL20F: yyzzz=_tmp1DB;
goto _LL20D;_LL211:(int)_throw((void*)& Cyc_yyfail_QualSpecList_tok);_LL20D:;}
return yyzzz;}static unsigned char _tmp1DD[14]="list_t<var_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={Cyc_Core_Failure,{_tmp1DD,_tmp1DD,_tmp1DD + 14}};struct Cyc_List_List*
Cyc_yyget_IdList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1DE=yy1;
struct Cyc_List_List*_tmp1DF;_LL214: if(*((void**)_tmp1DE)== Cyc_IdList_tok){
_LL218: _tmp1DF=((struct Cyc_IdList_tok_struct*)_tmp1DE)->f1;goto _LL215;}else{goto
_LL216;}_LL216: goto _LL217;_LL215: yyzzz=_tmp1DF;goto _LL213;_LL217:(int)_throw((
void*)& Cyc_yyfail_IdList_tok);_LL213:;}return yyzzz;}static unsigned char _tmp1E1[
32]="$(opt_t<var_t>,tqual_t,type_t)@";static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={
Cyc_Core_Failure,{_tmp1E1,_tmp1E1,_tmp1E1 + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(
void*yy1){struct _tuple2*yyzzz;{void*_tmp1E2=yy1;struct _tuple2*_tmp1E3;_LL21A: if(*((
void**)_tmp1E2)== Cyc_ParamDecl_tok){_LL21E: _tmp1E3=((struct Cyc_ParamDecl_tok_struct*)
_tmp1E2)->f1;goto _LL21B;}else{goto _LL21C;}_LL21C: goto _LL21D;_LL21B: yyzzz=_tmp1E3;
goto _LL219;_LL21D:(int)_throw((void*)& Cyc_yyfail_ParamDecl_tok);_LL219:;}return
yyzzz;}static unsigned char _tmp1E5[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp1E5,_tmp1E5,_tmp1E5 + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1E6=yy1;struct Cyc_List_List*
_tmp1E7;_LL220: if(*((void**)_tmp1E6)== Cyc_ParamDeclList_tok){_LL224: _tmp1E7=((
struct Cyc_ParamDeclList_tok_struct*)_tmp1E6)->f1;goto _LL221;}else{goto _LL222;}
_LL222: goto _LL223;_LL221: yyzzz=_tmp1E7;goto _LL21F;_LL223:(int)_throw((void*)& Cyc_yyfail_ParamDeclList_tok);
_LL21F:;}return yyzzz;}static unsigned char _tmp1E9[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1E9,
_tmp1E9,_tmp1E9 + 107}};struct _tuple13*Cyc_yyget_YY1(void*yy1){struct _tuple13*
yyzzz;{void*_tmp1EA=yy1;struct _tuple13*_tmp1EB;_LL226: if(*((void**)_tmp1EA)== 
Cyc_YY1){_LL22A: _tmp1EB=((struct Cyc_YY1_struct*)_tmp1EA)->f1;goto _LL227;}else{
goto _LL228;}_LL228: goto _LL229;_LL227: yyzzz=_tmp1EB;goto _LL225;_LL229:(int)_throw((
void*)& Cyc_yyfail_YY1);_LL225:;}return yyzzz;}static unsigned char _tmp1ED[15]="list_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={Cyc_Core_Failure,{
_tmp1ED,_tmp1ED,_tmp1ED + 15}};struct Cyc_List_List*Cyc_yyget_TypeList_tok(void*
yy1){struct Cyc_List_List*yyzzz;{void*_tmp1EE=yy1;struct Cyc_List_List*_tmp1EF;
_LL22C: if(*((void**)_tmp1EE)== Cyc_TypeList_tok){_LL230: _tmp1EF=((struct Cyc_TypeList_tok_struct*)
_tmp1EE)->f1;goto _LL22D;}else{goto _LL22E;}_LL22E: goto _LL22F;_LL22D: yyzzz=_tmp1EF;
goto _LL22B;_LL22F:(int)_throw((void*)& Cyc_yyfail_TypeList_tok);_LL22B:;}return
yyzzz;}static unsigned char _tmp1F1[21]="list_t<designator_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{_tmp1F1,_tmp1F1,_tmp1F1 + 21}};
struct Cyc_List_List*Cyc_yyget_DesignatorList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp1F2=yy1;struct Cyc_List_List*_tmp1F3;_LL232: if(*((void**)_tmp1F2)
== Cyc_DesignatorList_tok){_LL236: _tmp1F3=((struct Cyc_DesignatorList_tok_struct*)
_tmp1F2)->f1;goto _LL233;}else{goto _LL234;}_LL234: goto _LL235;_LL233: yyzzz=_tmp1F3;
goto _LL231;_LL235:(int)_throw((void*)& Cyc_yyfail_DesignatorList_tok);_LL231:;}
return yyzzz;}static unsigned char _tmp1F5[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={Cyc_Core_Failure,{_tmp1F5,_tmp1F5,_tmp1F5 + 13}};void*
Cyc_yyget_Designator_tok(void*yy1){void*yyzzz;{void*_tmp1F6=yy1;void*_tmp1F7;
_LL238: if(*((void**)_tmp1F6)== Cyc_Designator_tok){_LL23C: _tmp1F7=(void*)((
struct Cyc_Designator_tok_struct*)_tmp1F6)->f1;goto _LL239;}else{goto _LL23A;}
_LL23A: goto _LL23B;_LL239: yyzzz=_tmp1F7;goto _LL237;_LL23B:(int)_throw((void*)& Cyc_yyfail_Designator_tok);
_LL237:;}return yyzzz;}static unsigned char _tmp1F9[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{_tmp1F9,_tmp1F9,_tmp1F9 + 7}};void*Cyc_yyget_Kind_tok(
void*yy1){void*yyzzz;{void*_tmp1FA=yy1;void*_tmp1FB;_LL23E: if(*((void**)_tmp1FA)
== Cyc_Kind_tok){_LL242: _tmp1FB=(void*)((struct Cyc_Kind_tok_struct*)_tmp1FA)->f1;
goto _LL23F;}else{goto _LL240;}_LL240: goto _LL241;_LL23F: yyzzz=_tmp1FB;goto _LL23D;
_LL241:(int)_throw((void*)& Cyc_yyfail_Kind_tok);_LL23D:;}return yyzzz;}static
unsigned char _tmp1FD[7]="type_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={
Cyc_Core_Failure,{_tmp1FD,_tmp1FD,_tmp1FD + 7}};void*Cyc_yyget_Type_tok(void*yy1){
void*yyzzz;{void*_tmp1FE=yy1;void*_tmp1FF;_LL244: if(*((void**)_tmp1FE)== Cyc_Type_tok){
_LL248: _tmp1FF=(void*)((struct Cyc_Type_tok_struct*)_tmp1FE)->f1;goto _LL245;}
else{goto _LL246;}_LL246: goto _LL247;_LL245: yyzzz=_tmp1FF;goto _LL243;_LL247:(int)
_throw((void*)& Cyc_yyfail_Type_tok);_LL243:;}return yyzzz;}static unsigned char
_tmp201[20]="list_t<attribute_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={
Cyc_Core_Failure,{_tmp201,_tmp201,_tmp201 + 20}};struct Cyc_List_List*Cyc_yyget_AttributeList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp202=yy1;struct Cyc_List_List*
_tmp203;_LL24A: if(*((void**)_tmp202)== Cyc_AttributeList_tok){_LL24E: _tmp203=((
struct Cyc_AttributeList_tok_struct*)_tmp202)->f1;goto _LL24B;}else{goto _LL24C;}
_LL24C: goto _LL24D;_LL24B: yyzzz=_tmp203;goto _LL249;_LL24D:(int)_throw((void*)& Cyc_yyfail_AttributeList_tok);
_LL249:;}return yyzzz;}static unsigned char _tmp205[12]="attribute_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{_tmp205,
_tmp205,_tmp205 + 12}};void*Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{void*
_tmp206=yy1;void*_tmp207;_LL250: if(*((void**)_tmp206)== Cyc_Attribute_tok){
_LL254: _tmp207=(void*)((struct Cyc_Attribute_tok_struct*)_tmp206)->f1;goto _LL251;}
else{goto _LL252;}_LL252: goto _LL253;_LL251: yyzzz=_tmp207;goto _LL24F;_LL253:(int)
_throw((void*)& Cyc_yyfail_Attribute_tok);_LL24F:;}return yyzzz;}static
unsigned char _tmp209[12]="enumfield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={
Cyc_Core_Failure,{_tmp209,_tmp209,_tmp209 + 12}};struct Cyc_Absyn_Enumfield*Cyc_yyget_Enumfield_tok(
void*yy1){struct Cyc_Absyn_Enumfield*yyzzz;{void*_tmp20A=yy1;struct Cyc_Absyn_Enumfield*
_tmp20B;_LL256: if(*((void**)_tmp20A)== Cyc_Enumfield_tok){_LL25A: _tmp20B=((
struct Cyc_Enumfield_tok_struct*)_tmp20A)->f1;goto _LL257;}else{goto _LL258;}_LL258:
goto _LL259;_LL257: yyzzz=_tmp20B;goto _LL255;_LL259:(int)_throw((void*)& Cyc_yyfail_Enumfield_tok);
_LL255:;}return yyzzz;}static unsigned char _tmp20D[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{
_tmp20D,_tmp20D,_tmp20D + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp20E=yy1;struct Cyc_List_List*
_tmp20F;_LL25C: if(*((void**)_tmp20E)== Cyc_EnumfieldList_tok){_LL260: _tmp20F=((
struct Cyc_EnumfieldList_tok_struct*)_tmp20E)->f1;goto _LL25D;}else{goto _LL25E;}
_LL25E: goto _LL25F;_LL25D: yyzzz=_tmp20F;goto _LL25B;_LL25F:(int)_throw((void*)& Cyc_yyfail_EnumfieldList_tok);
_LL25B:;}return yyzzz;}static unsigned char _tmp211[14]="opt_t<type_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeOpt_tok={Cyc_Core_Failure,{_tmp211,
_tmp211,_tmp211 + 14}};struct Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(void*yy1){struct
Cyc_Core_Opt*yyzzz;{void*_tmp212=yy1;struct Cyc_Core_Opt*_tmp213;_LL262: if(*((
void**)_tmp212)== Cyc_TypeOpt_tok){_LL266: _tmp213=((struct Cyc_TypeOpt_tok_struct*)
_tmp212)->f1;goto _LL263;}else{goto _LL264;}_LL264: goto _LL265;_LL263: yyzzz=_tmp213;
goto _LL261;_LL265:(int)_throw((void*)& Cyc_yyfail_TypeOpt_tok);_LL261:;}return
yyzzz;}static unsigned char _tmp215[26]="list_t<$(type_t,type_t)@>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{_tmp215,
_tmp215,_tmp215 + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp216=yy1;struct Cyc_List_List*_tmp217;_LL268:
if(*((void**)_tmp216)== Cyc_Rgnorder_tok){_LL26C: _tmp217=((struct Cyc_Rgnorder_tok_struct*)
_tmp216)->f1;goto _LL269;}else{goto _LL26A;}_LL26A: goto _LL26B;_LL269: yyzzz=_tmp217;
goto _LL267;_LL26B:(int)_throw((void*)& Cyc_yyfail_Rgnorder_tok);_LL267:;}return
yyzzz;}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;
int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){return({struct Cyc_Yyltype
_tmp218;_tmp218.timestamp=0;_tmp218.first_line=0;_tmp218.first_column=0;_tmp218.last_line=
0;_tmp218.last_column=0;_tmp218;});}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
static short Cyc_yytranslate[354]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,125,2,2,107,123,119,2,103,104,113,116,105,118,110,122,2,2,2,2,
2,2,2,2,2,2,109,99,108,102,117,115,114,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,111,2,112,121,106,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,100,120,101,124,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,
46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,
73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98};
static unsigned char _tmp219[2]="$";static unsigned char _tmp21A[6]="error";static
unsigned char _tmp21B[12]="$undefined.";static unsigned char _tmp21C[5]="AUTO";
static unsigned char _tmp21D[9]="REGISTER";static unsigned char _tmp21E[7]="STATIC";
static unsigned char _tmp21F[7]="EXTERN";static unsigned char _tmp220[8]="TYPEDEF";
static unsigned char _tmp221[5]="VOID";static unsigned char _tmp222[5]="CHAR";static
unsigned char _tmp223[6]="SHORT";static unsigned char _tmp224[4]="INT";static
unsigned char _tmp225[5]="LONG";static unsigned char _tmp226[6]="FLOAT";static
unsigned char _tmp227[7]="DOUBLE";static unsigned char _tmp228[7]="SIGNED";static
unsigned char _tmp229[9]="UNSIGNED";static unsigned char _tmp22A[6]="CONST";static
unsigned char _tmp22B[9]="VOLATILE";static unsigned char _tmp22C[9]="RESTRICT";
static unsigned char _tmp22D[7]="STRUCT";static unsigned char _tmp22E[6]="UNION";
static unsigned char _tmp22F[5]="CASE";static unsigned char _tmp230[8]="DEFAULT";
static unsigned char _tmp231[7]="INLINE";static unsigned char _tmp232[7]="SIZEOF";
static unsigned char _tmp233[9]="OFFSETOF";static unsigned char _tmp234[3]="IF";
static unsigned char _tmp235[5]="ELSE";static unsigned char _tmp236[7]="SWITCH";
static unsigned char _tmp237[6]="WHILE";static unsigned char _tmp238[3]="DO";static
unsigned char _tmp239[4]="FOR";static unsigned char _tmp23A[5]="GOTO";static
unsigned char _tmp23B[9]="CONTINUE";static unsigned char _tmp23C[6]="BREAK";static
unsigned char _tmp23D[7]="RETURN";static unsigned char _tmp23E[5]="ENUM";static
unsigned char _tmp23F[8]="NULL_kw";static unsigned char _tmp240[4]="LET";static
unsigned char _tmp241[6]="THROW";static unsigned char _tmp242[4]="TRY";static
unsigned char _tmp243[6]="CATCH";static unsigned char _tmp244[4]="NEW";static
unsigned char _tmp245[9]="ABSTRACT";static unsigned char _tmp246[9]="FALLTHRU";
static unsigned char _tmp247[6]="USING";static unsigned char _tmp248[10]="NAMESPACE";
static unsigned char _tmp249[7]="TUNION";static unsigned char _tmp24A[8]="XTUNION";
static unsigned char _tmp24B[9]="FORARRAY";static unsigned char _tmp24C[5]="FILL";
static unsigned char _tmp24D[8]="CODEGEN";static unsigned char _tmp24E[4]="CUT";
static unsigned char _tmp24F[7]="SPLICE";static unsigned char _tmp250[7]="MALLOC";
static unsigned char _tmp251[8]="RMALLOC";static unsigned char _tmp252[7]="CALLOC";
static unsigned char _tmp253[8]="RCALLOC";static unsigned char _tmp254[9]="REGION_T";
static unsigned char _tmp255[9]="SIZEOF_T";static unsigned char _tmp256[7]="REGION";
static unsigned char _tmp257[5]="RNEW";static unsigned char _tmp258[8]="REGIONS";
static unsigned char _tmp259[4]="GEN";static unsigned char _tmp25A[7]="PTR_OP";
static unsigned char _tmp25B[7]="INC_OP";static unsigned char _tmp25C[7]="DEC_OP";
static unsigned char _tmp25D[8]="LEFT_OP";static unsigned char _tmp25E[9]="RIGHT_OP";
static unsigned char _tmp25F[6]="LE_OP";static unsigned char _tmp260[6]="GE_OP";
static unsigned char _tmp261[6]="EQ_OP";static unsigned char _tmp262[6]="NE_OP";
static unsigned char _tmp263[7]="AND_OP";static unsigned char _tmp264[6]="OR_OP";
static unsigned char _tmp265[11]="MUL_ASSIGN";static unsigned char _tmp266[11]="DIV_ASSIGN";
static unsigned char _tmp267[11]="MOD_ASSIGN";static unsigned char _tmp268[11]="ADD_ASSIGN";
static unsigned char _tmp269[11]="SUB_ASSIGN";static unsigned char _tmp26A[12]="LEFT_ASSIGN";
static unsigned char _tmp26B[13]="RIGHT_ASSIGN";static unsigned char _tmp26C[11]="AND_ASSIGN";
static unsigned char _tmp26D[11]="XOR_ASSIGN";static unsigned char _tmp26E[10]="OR_ASSIGN";
static unsigned char _tmp26F[9]="ELLIPSIS";static unsigned char _tmp270[11]="LEFT_RIGHT";
static unsigned char _tmp271[12]="COLON_COLON";static unsigned char _tmp272[11]="IDENTIFIER";
static unsigned char _tmp273[17]="INTEGER_CONSTANT";static unsigned char _tmp274[7]="STRING";
static unsigned char _tmp275[19]="CHARACTER_CONSTANT";static unsigned char _tmp276[
18]="FLOATING_CONSTANT";static unsigned char _tmp277[9]="TYPE_VAR";static
unsigned char _tmp278[13]="TYPEDEF_NAME";static unsigned char _tmp279[16]="QUAL_IDENTIFIER";
static unsigned char _tmp27A[18]="QUAL_TYPEDEF_NAME";static unsigned char _tmp27B[10]="ATTRIBUTE";
static unsigned char _tmp27C[4]="';'";static unsigned char _tmp27D[4]="'{'";static
unsigned char _tmp27E[4]="'}'";static unsigned char _tmp27F[4]="'='";static
unsigned char _tmp280[4]="'('";static unsigned char _tmp281[4]="')'";static
unsigned char _tmp282[4]="','";static unsigned char _tmp283[4]="'_'";static
unsigned char _tmp284[4]="'$'";static unsigned char _tmp285[4]="'<'";static
unsigned char _tmp286[4]="':'";static unsigned char _tmp287[4]="'.'";static
unsigned char _tmp288[4]="'['";static unsigned char _tmp289[4]="']'";static
unsigned char _tmp28A[4]="'*'";static unsigned char _tmp28B[4]="'@'";static
unsigned char _tmp28C[4]="'?'";static unsigned char _tmp28D[4]="'+'";static
unsigned char _tmp28E[4]="'>'";static unsigned char _tmp28F[4]="'-'";static
unsigned char _tmp290[4]="'&'";static unsigned char _tmp291[4]="'|'";static
unsigned char _tmp292[4]="'^'";static unsigned char _tmp293[4]="'/'";static
unsigned char _tmp294[4]="'%'";static unsigned char _tmp295[4]="'~'";static
unsigned char _tmp296[4]="'!'";static unsigned char _tmp297[5]="prog";static
unsigned char _tmp298[17]="translation_unit";static unsigned char _tmp299[21]="external_declaration";
static unsigned char _tmp29A[20]="function_definition";static unsigned char _tmp29B[
21]="function_definition2";static unsigned char _tmp29C[13]="using_action";static
unsigned char _tmp29D[15]="unusing_action";static unsigned char _tmp29E[17]="namespace_action";
static unsigned char _tmp29F[19]="unnamespace_action";static unsigned char _tmp2A0[
12]="declaration";static unsigned char _tmp2A1[17]="declaration_list";static
unsigned char _tmp2A2[23]="declaration_specifiers";static unsigned char _tmp2A3[24]="storage_class_specifier";
static unsigned char _tmp2A4[15]="attributes_opt";static unsigned char _tmp2A5[11]="attributes";
static unsigned char _tmp2A6[15]="attribute_list";static unsigned char _tmp2A7[10]="attribute";
static unsigned char _tmp2A8[15]="type_specifier";static unsigned char _tmp2A9[5]="kind";
static unsigned char _tmp2AA[15]="type_qualifier";static unsigned char _tmp2AB[15]="enum_specifier";
static unsigned char _tmp2AC[11]="enum_field";static unsigned char _tmp2AD[22]="enum_declaration_list";
static unsigned char _tmp2AE[26]="struct_or_union_specifier";static unsigned char
_tmp2AF[16]="type_params_opt";static unsigned char _tmp2B0[16]="struct_or_union";
static unsigned char _tmp2B1[24]="struct_declaration_list";static unsigned char
_tmp2B2[25]="struct_declaration_list0";static unsigned char _tmp2B3[21]="init_declarator_list";
static unsigned char _tmp2B4[22]="init_declarator_list0";static unsigned char
_tmp2B5[16]="init_declarator";static unsigned char _tmp2B6[19]="struct_declaration";
static unsigned char _tmp2B7[25]="specifier_qualifier_list";static unsigned char
_tmp2B8[23]="struct_declarator_list";static unsigned char _tmp2B9[24]="struct_declarator_list0";
static unsigned char _tmp2BA[18]="struct_declarator";static unsigned char _tmp2BB[17]="tunion_specifier";
static unsigned char _tmp2BC[18]="tunion_or_xtunion";static unsigned char _tmp2BD[17]="tunionfield_list";
static unsigned char _tmp2BE[18]="tunionfield_scope";static unsigned char _tmp2BF[12]="tunionfield";
static unsigned char _tmp2C0[11]="declarator";static unsigned char _tmp2C1[18]="direct_declarator";
static unsigned char _tmp2C2[8]="pointer";static unsigned char _tmp2C3[13]="pointer_char";
static unsigned char _tmp2C4[8]="rgn_opt";static unsigned char _tmp2C5[4]="rgn";
static unsigned char _tmp2C6[20]="type_qualifier_list";static unsigned char _tmp2C7[
20]="parameter_type_list";static unsigned char _tmp2C8[9]="type_var";static
unsigned char _tmp2C9[16]="optional_effect";static unsigned char _tmp2CA[19]="optional_rgn_order";
static unsigned char _tmp2CB[10]="rgn_order";static unsigned char _tmp2CC[16]="optional_inject";
static unsigned char _tmp2CD[11]="effect_set";static unsigned char _tmp2CE[14]="atomic_effect";
static unsigned char _tmp2CF[11]="region_set";static unsigned char _tmp2D0[15]="parameter_list";
static unsigned char _tmp2D1[22]="parameter_declaration";static unsigned char
_tmp2D2[16]="identifier_list";static unsigned char _tmp2D3[17]="identifier_list0";
static unsigned char _tmp2D4[12]="initializer";static unsigned char _tmp2D5[18]="array_initializer";
static unsigned char _tmp2D6[17]="initializer_list";static unsigned char _tmp2D7[12]="designation";
static unsigned char _tmp2D8[16]="designator_list";static unsigned char _tmp2D9[11]="designator";
static unsigned char _tmp2DA[10]="type_name";static unsigned char _tmp2DB[14]="any_type_name";
static unsigned char _tmp2DC[15]="type_name_list";static unsigned char _tmp2DD[20]="abstract_declarator";
static unsigned char _tmp2DE[27]="direct_abstract_declarator";static unsigned char
_tmp2DF[10]="statement";static unsigned char _tmp2E0[18]="labeled_statement";
static unsigned char _tmp2E1[21]="expression_statement";static unsigned char _tmp2E2[
19]="compound_statement";static unsigned char _tmp2E3[16]="block_item_list";static
unsigned char _tmp2E4[20]="selection_statement";static unsigned char _tmp2E5[15]="switch_clauses";
static unsigned char _tmp2E6[16]="switchC_clauses";static unsigned char _tmp2E7[20]="iteration_statement";
static unsigned char _tmp2E8[15]="jump_statement";static unsigned char _tmp2E9[8]="pattern";
static unsigned char _tmp2EA[19]="tuple_pattern_list";static unsigned char _tmp2EB[
20]="tuple_pattern_list0";static unsigned char _tmp2EC[14]="field_pattern";static
unsigned char _tmp2ED[19]="field_pattern_list";static unsigned char _tmp2EE[20]="field_pattern_list0";
static unsigned char _tmp2EF[11]="expression";static unsigned char _tmp2F0[22]="assignment_expression";
static unsigned char _tmp2F1[20]="assignment_operator";static unsigned char _tmp2F2[
23]="conditional_expression";static unsigned char _tmp2F3[20]="constant_expression";
static unsigned char _tmp2F4[22]="logical_or_expression";static unsigned char
_tmp2F5[23]="logical_and_expression";static unsigned char _tmp2F6[24]="inclusive_or_expression";
static unsigned char _tmp2F7[24]="exclusive_or_expression";static unsigned char
_tmp2F8[15]="and_expression";static unsigned char _tmp2F9[20]="equality_expression";
static unsigned char _tmp2FA[22]="relational_expression";static unsigned char
_tmp2FB[17]="shift_expression";static unsigned char _tmp2FC[20]="additive_expression";
static unsigned char _tmp2FD[26]="multiplicative_expression";static unsigned char
_tmp2FE[16]="cast_expression";static unsigned char _tmp2FF[17]="unary_expression";
static unsigned char _tmp300[15]="unary_operator";static unsigned char _tmp301[19]="postfix_expression";
static unsigned char _tmp302[19]="primary_expression";static unsigned char _tmp303[
25]="argument_expression_list";static unsigned char _tmp304[26]="argument_expression_list0";
static unsigned char _tmp305[9]="constant";static unsigned char _tmp306[20]="qual_opt_identifier";
static unsigned char _tmp307[17]="qual_opt_typedef";static unsigned char _tmp308[18]="struct_union_name";
static unsigned char _tmp309[11]="field_name";static unsigned char _tmp30A[12]="right_angle";
static struct _tagged_arr Cyc_yytname[242]={{_tmp219,_tmp219,_tmp219 + 2},{_tmp21A,
_tmp21A,_tmp21A + 6},{_tmp21B,_tmp21B,_tmp21B + 12},{_tmp21C,_tmp21C,_tmp21C + 5},{
_tmp21D,_tmp21D,_tmp21D + 9},{_tmp21E,_tmp21E,_tmp21E + 7},{_tmp21F,_tmp21F,
_tmp21F + 7},{_tmp220,_tmp220,_tmp220 + 8},{_tmp221,_tmp221,_tmp221 + 5},{_tmp222,
_tmp222,_tmp222 + 5},{_tmp223,_tmp223,_tmp223 + 6},{_tmp224,_tmp224,_tmp224 + 4},{
_tmp225,_tmp225,_tmp225 + 5},{_tmp226,_tmp226,_tmp226 + 6},{_tmp227,_tmp227,
_tmp227 + 7},{_tmp228,_tmp228,_tmp228 + 7},{_tmp229,_tmp229,_tmp229 + 9},{_tmp22A,
_tmp22A,_tmp22A + 6},{_tmp22B,_tmp22B,_tmp22B + 9},{_tmp22C,_tmp22C,_tmp22C + 9},{
_tmp22D,_tmp22D,_tmp22D + 7},{_tmp22E,_tmp22E,_tmp22E + 6},{_tmp22F,_tmp22F,
_tmp22F + 5},{_tmp230,_tmp230,_tmp230 + 8},{_tmp231,_tmp231,_tmp231 + 7},{_tmp232,
_tmp232,_tmp232 + 7},{_tmp233,_tmp233,_tmp233 + 9},{_tmp234,_tmp234,_tmp234 + 3},{
_tmp235,_tmp235,_tmp235 + 5},{_tmp236,_tmp236,_tmp236 + 7},{_tmp237,_tmp237,
_tmp237 + 6},{_tmp238,_tmp238,_tmp238 + 3},{_tmp239,_tmp239,_tmp239 + 4},{_tmp23A,
_tmp23A,_tmp23A + 5},{_tmp23B,_tmp23B,_tmp23B + 9},{_tmp23C,_tmp23C,_tmp23C + 6},{
_tmp23D,_tmp23D,_tmp23D + 7},{_tmp23E,_tmp23E,_tmp23E + 5},{_tmp23F,_tmp23F,
_tmp23F + 8},{_tmp240,_tmp240,_tmp240 + 4},{_tmp241,_tmp241,_tmp241 + 6},{_tmp242,
_tmp242,_tmp242 + 4},{_tmp243,_tmp243,_tmp243 + 6},{_tmp244,_tmp244,_tmp244 + 4},{
_tmp245,_tmp245,_tmp245 + 9},{_tmp246,_tmp246,_tmp246 + 9},{_tmp247,_tmp247,
_tmp247 + 6},{_tmp248,_tmp248,_tmp248 + 10},{_tmp249,_tmp249,_tmp249 + 7},{_tmp24A,
_tmp24A,_tmp24A + 8},{_tmp24B,_tmp24B,_tmp24B + 9},{_tmp24C,_tmp24C,_tmp24C + 5},{
_tmp24D,_tmp24D,_tmp24D + 8},{_tmp24E,_tmp24E,_tmp24E + 4},{_tmp24F,_tmp24F,
_tmp24F + 7},{_tmp250,_tmp250,_tmp250 + 7},{_tmp251,_tmp251,_tmp251 + 8},{_tmp252,
_tmp252,_tmp252 + 7},{_tmp253,_tmp253,_tmp253 + 8},{_tmp254,_tmp254,_tmp254 + 9},{
_tmp255,_tmp255,_tmp255 + 9},{_tmp256,_tmp256,_tmp256 + 7},{_tmp257,_tmp257,
_tmp257 + 5},{_tmp258,_tmp258,_tmp258 + 8},{_tmp259,_tmp259,_tmp259 + 4},{_tmp25A,
_tmp25A,_tmp25A + 7},{_tmp25B,_tmp25B,_tmp25B + 7},{_tmp25C,_tmp25C,_tmp25C + 7},{
_tmp25D,_tmp25D,_tmp25D + 8},{_tmp25E,_tmp25E,_tmp25E + 9},{_tmp25F,_tmp25F,
_tmp25F + 6},{_tmp260,_tmp260,_tmp260 + 6},{_tmp261,_tmp261,_tmp261 + 6},{_tmp262,
_tmp262,_tmp262 + 6},{_tmp263,_tmp263,_tmp263 + 7},{_tmp264,_tmp264,_tmp264 + 6},{
_tmp265,_tmp265,_tmp265 + 11},{_tmp266,_tmp266,_tmp266 + 11},{_tmp267,_tmp267,
_tmp267 + 11},{_tmp268,_tmp268,_tmp268 + 11},{_tmp269,_tmp269,_tmp269 + 11},{
_tmp26A,_tmp26A,_tmp26A + 12},{_tmp26B,_tmp26B,_tmp26B + 13},{_tmp26C,_tmp26C,
_tmp26C + 11},{_tmp26D,_tmp26D,_tmp26D + 11},{_tmp26E,_tmp26E,_tmp26E + 10},{
_tmp26F,_tmp26F,_tmp26F + 9},{_tmp270,_tmp270,_tmp270 + 11},{_tmp271,_tmp271,
_tmp271 + 12},{_tmp272,_tmp272,_tmp272 + 11},{_tmp273,_tmp273,_tmp273 + 17},{
_tmp274,_tmp274,_tmp274 + 7},{_tmp275,_tmp275,_tmp275 + 19},{_tmp276,_tmp276,
_tmp276 + 18},{_tmp277,_tmp277,_tmp277 + 9},{_tmp278,_tmp278,_tmp278 + 13},{_tmp279,
_tmp279,_tmp279 + 16},{_tmp27A,_tmp27A,_tmp27A + 18},{_tmp27B,_tmp27B,_tmp27B + 10},{
_tmp27C,_tmp27C,_tmp27C + 4},{_tmp27D,_tmp27D,_tmp27D + 4},{_tmp27E,_tmp27E,
_tmp27E + 4},{_tmp27F,_tmp27F,_tmp27F + 4},{_tmp280,_tmp280,_tmp280 + 4},{_tmp281,
_tmp281,_tmp281 + 4},{_tmp282,_tmp282,_tmp282 + 4},{_tmp283,_tmp283,_tmp283 + 4},{
_tmp284,_tmp284,_tmp284 + 4},{_tmp285,_tmp285,_tmp285 + 4},{_tmp286,_tmp286,
_tmp286 + 4},{_tmp287,_tmp287,_tmp287 + 4},{_tmp288,_tmp288,_tmp288 + 4},{_tmp289,
_tmp289,_tmp289 + 4},{_tmp28A,_tmp28A,_tmp28A + 4},{_tmp28B,_tmp28B,_tmp28B + 4},{
_tmp28C,_tmp28C,_tmp28C + 4},{_tmp28D,_tmp28D,_tmp28D + 4},{_tmp28E,_tmp28E,
_tmp28E + 4},{_tmp28F,_tmp28F,_tmp28F + 4},{_tmp290,_tmp290,_tmp290 + 4},{_tmp291,
_tmp291,_tmp291 + 4},{_tmp292,_tmp292,_tmp292 + 4},{_tmp293,_tmp293,_tmp293 + 4},{
_tmp294,_tmp294,_tmp294 + 4},{_tmp295,_tmp295,_tmp295 + 4},{_tmp296,_tmp296,
_tmp296 + 4},{_tmp297,_tmp297,_tmp297 + 5},{_tmp298,_tmp298,_tmp298 + 17},{_tmp299,
_tmp299,_tmp299 + 21},{_tmp29A,_tmp29A,_tmp29A + 20},{_tmp29B,_tmp29B,_tmp29B + 21},{
_tmp29C,_tmp29C,_tmp29C + 13},{_tmp29D,_tmp29D,_tmp29D + 15},{_tmp29E,_tmp29E,
_tmp29E + 17},{_tmp29F,_tmp29F,_tmp29F + 19},{_tmp2A0,_tmp2A0,_tmp2A0 + 12},{
_tmp2A1,_tmp2A1,_tmp2A1 + 17},{_tmp2A2,_tmp2A2,_tmp2A2 + 23},{_tmp2A3,_tmp2A3,
_tmp2A3 + 24},{_tmp2A4,_tmp2A4,_tmp2A4 + 15},{_tmp2A5,_tmp2A5,_tmp2A5 + 11},{
_tmp2A6,_tmp2A6,_tmp2A6 + 15},{_tmp2A7,_tmp2A7,_tmp2A7 + 10},{_tmp2A8,_tmp2A8,
_tmp2A8 + 15},{_tmp2A9,_tmp2A9,_tmp2A9 + 5},{_tmp2AA,_tmp2AA,_tmp2AA + 15},{_tmp2AB,
_tmp2AB,_tmp2AB + 15},{_tmp2AC,_tmp2AC,_tmp2AC + 11},{_tmp2AD,_tmp2AD,_tmp2AD + 22},{
_tmp2AE,_tmp2AE,_tmp2AE + 26},{_tmp2AF,_tmp2AF,_tmp2AF + 16},{_tmp2B0,_tmp2B0,
_tmp2B0 + 16},{_tmp2B1,_tmp2B1,_tmp2B1 + 24},{_tmp2B2,_tmp2B2,_tmp2B2 + 25},{
_tmp2B3,_tmp2B3,_tmp2B3 + 21},{_tmp2B4,_tmp2B4,_tmp2B4 + 22},{_tmp2B5,_tmp2B5,
_tmp2B5 + 16},{_tmp2B6,_tmp2B6,_tmp2B6 + 19},{_tmp2B7,_tmp2B7,_tmp2B7 + 25},{
_tmp2B8,_tmp2B8,_tmp2B8 + 23},{_tmp2B9,_tmp2B9,_tmp2B9 + 24},{_tmp2BA,_tmp2BA,
_tmp2BA + 18},{_tmp2BB,_tmp2BB,_tmp2BB + 17},{_tmp2BC,_tmp2BC,_tmp2BC + 18},{
_tmp2BD,_tmp2BD,_tmp2BD + 17},{_tmp2BE,_tmp2BE,_tmp2BE + 18},{_tmp2BF,_tmp2BF,
_tmp2BF + 12},{_tmp2C0,_tmp2C0,_tmp2C0 + 11},{_tmp2C1,_tmp2C1,_tmp2C1 + 18},{
_tmp2C2,_tmp2C2,_tmp2C2 + 8},{_tmp2C3,_tmp2C3,_tmp2C3 + 13},{_tmp2C4,_tmp2C4,
_tmp2C4 + 8},{_tmp2C5,_tmp2C5,_tmp2C5 + 4},{_tmp2C6,_tmp2C6,_tmp2C6 + 20},{_tmp2C7,
_tmp2C7,_tmp2C7 + 20},{_tmp2C8,_tmp2C8,_tmp2C8 + 9},{_tmp2C9,_tmp2C9,_tmp2C9 + 16},{
_tmp2CA,_tmp2CA,_tmp2CA + 19},{_tmp2CB,_tmp2CB,_tmp2CB + 10},{_tmp2CC,_tmp2CC,
_tmp2CC + 16},{_tmp2CD,_tmp2CD,_tmp2CD + 11},{_tmp2CE,_tmp2CE,_tmp2CE + 14},{
_tmp2CF,_tmp2CF,_tmp2CF + 11},{_tmp2D0,_tmp2D0,_tmp2D0 + 15},{_tmp2D1,_tmp2D1,
_tmp2D1 + 22},{_tmp2D2,_tmp2D2,_tmp2D2 + 16},{_tmp2D3,_tmp2D3,_tmp2D3 + 17},{
_tmp2D4,_tmp2D4,_tmp2D4 + 12},{_tmp2D5,_tmp2D5,_tmp2D5 + 18},{_tmp2D6,_tmp2D6,
_tmp2D6 + 17},{_tmp2D7,_tmp2D7,_tmp2D7 + 12},{_tmp2D8,_tmp2D8,_tmp2D8 + 16},{
_tmp2D9,_tmp2D9,_tmp2D9 + 11},{_tmp2DA,_tmp2DA,_tmp2DA + 10},{_tmp2DB,_tmp2DB,
_tmp2DB + 14},{_tmp2DC,_tmp2DC,_tmp2DC + 15},{_tmp2DD,_tmp2DD,_tmp2DD + 20},{
_tmp2DE,_tmp2DE,_tmp2DE + 27},{_tmp2DF,_tmp2DF,_tmp2DF + 10},{_tmp2E0,_tmp2E0,
_tmp2E0 + 18},{_tmp2E1,_tmp2E1,_tmp2E1 + 21},{_tmp2E2,_tmp2E2,_tmp2E2 + 19},{
_tmp2E3,_tmp2E3,_tmp2E3 + 16},{_tmp2E4,_tmp2E4,_tmp2E4 + 20},{_tmp2E5,_tmp2E5,
_tmp2E5 + 15},{_tmp2E6,_tmp2E6,_tmp2E6 + 16},{_tmp2E7,_tmp2E7,_tmp2E7 + 20},{
_tmp2E8,_tmp2E8,_tmp2E8 + 15},{_tmp2E9,_tmp2E9,_tmp2E9 + 8},{_tmp2EA,_tmp2EA,
_tmp2EA + 19},{_tmp2EB,_tmp2EB,_tmp2EB + 20},{_tmp2EC,_tmp2EC,_tmp2EC + 14},{
_tmp2ED,_tmp2ED,_tmp2ED + 19},{_tmp2EE,_tmp2EE,_tmp2EE + 20},{_tmp2EF,_tmp2EF,
_tmp2EF + 11},{_tmp2F0,_tmp2F0,_tmp2F0 + 22},{_tmp2F1,_tmp2F1,_tmp2F1 + 20},{
_tmp2F2,_tmp2F2,_tmp2F2 + 23},{_tmp2F3,_tmp2F3,_tmp2F3 + 20},{_tmp2F4,_tmp2F4,
_tmp2F4 + 22},{_tmp2F5,_tmp2F5,_tmp2F5 + 23},{_tmp2F6,_tmp2F6,_tmp2F6 + 24},{
_tmp2F7,_tmp2F7,_tmp2F7 + 24},{_tmp2F8,_tmp2F8,_tmp2F8 + 15},{_tmp2F9,_tmp2F9,
_tmp2F9 + 20},{_tmp2FA,_tmp2FA,_tmp2FA + 22},{_tmp2FB,_tmp2FB,_tmp2FB + 17},{
_tmp2FC,_tmp2FC,_tmp2FC + 20},{_tmp2FD,_tmp2FD,_tmp2FD + 26},{_tmp2FE,_tmp2FE,
_tmp2FE + 16},{_tmp2FF,_tmp2FF,_tmp2FF + 17},{_tmp300,_tmp300,_tmp300 + 15},{
_tmp301,_tmp301,_tmp301 + 19},{_tmp302,_tmp302,_tmp302 + 19},{_tmp303,_tmp303,
_tmp303 + 25},{_tmp304,_tmp304,_tmp304 + 26},{_tmp305,_tmp305,_tmp305 + 9},{_tmp306,
_tmp306,_tmp306 + 20},{_tmp307,_tmp307,_tmp307 + 17},{_tmp308,_tmp308,_tmp308 + 18},{
_tmp309,_tmp309,_tmp309 + 11},{_tmp30A,_tmp30A,_tmp30A + 12}};static short Cyc_yyr1[
410]={0,126,127,127,127,127,127,127,127,128,128,129,129,129,129,130,130,131,132,
133,134,135,135,135,135,136,136,137,137,137,137,137,137,137,137,138,138,138,138,
138,138,138,139,139,140,141,141,142,142,142,142,142,143,143,143,143,143,143,143,
143,143,143,143,143,143,143,143,143,143,143,144,145,145,145,146,146,146,147,147,
148,148,149,149,149,150,150,151,151,152,153,153,154,155,155,156,156,157,158,158,
158,158,159,160,160,161,161,161,162,162,162,162,163,163,164,164,164,164,165,165,
165,166,166,167,167,168,168,168,168,168,168,168,168,168,168,168,169,169,169,169,
170,170,170,170,170,171,171,172,172,173,173,174,174,174,174,175,175,176,176,177,
177,178,178,179,179,180,180,181,181,181,181,182,182,183,183,184,184,184,185,186,
186,187,187,188,188,188,188,189,189,189,189,190,191,191,192,192,193,193,194,194,
194,194,194,195,195,196,196,196,197,197,197,197,197,197,197,197,197,197,197,198,
198,198,198,198,198,198,198,198,198,199,200,200,201,201,202,202,202,202,202,202,
203,203,203,203,203,204,204,204,204,204,204,205,205,205,205,206,206,206,206,206,
206,206,206,206,206,206,206,206,206,206,207,207,207,207,207,207,207,207,208,208,
208,208,208,208,208,208,208,208,208,208,208,208,209,209,210,210,211,211,212,213,
213,214,214,215,215,216,216,216,216,216,216,216,216,216,216,216,217,217,217,217,
217,217,217,218,219,219,220,220,221,221,222,222,223,223,224,224,224,225,225,225,
225,225,226,226,226,227,227,227,228,228,228,228,229,229,230,230,230,230,230,230,
230,230,230,230,230,230,230,230,230,230,231,231,231,232,232,232,232,232,232,232,
232,232,232,232,232,233,233,233,233,233,233,233,233,233,234,235,235,236,236,236,
236,237,237,238,238,239,239,240,240,241,241};static short Cyc_yyr2[410]={0,1,2,3,5,
3,5,6,0,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,1,2,2,3,2,3,2,3,2,3,1,1,1,1,2,1,1,0,1,6,
1,3,1,1,4,4,8,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,4,4,4,1,1,1,1,5,2,4,1,3,1,3,4,6,3,0,
3,1,1,1,1,2,1,1,3,1,3,3,2,3,2,3,1,1,3,1,2,3,6,4,3,5,1,1,1,2,3,3,0,1,1,2,6,1,2,1,
3,3,4,4,5,4,4,2,2,1,3,4,4,5,1,1,4,4,1,0,1,1,1,1,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,
3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,
1,2,3,2,3,3,4,4,3,5,4,4,2,1,1,1,1,1,1,6,3,2,2,3,1,2,2,3,1,2,1,2,1,2,5,7,7,8,6,0,
3,4,5,6,7,0,3,4,5,5,7,6,7,7,8,7,8,8,9,6,7,7,8,9,3,2,2,2,3,2,4,5,1,3,1,2,1,1,1,1,
5,4,4,5,2,2,0,1,1,3,1,2,1,1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,
1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,
4,6,9,11,1,1,1,1,4,3,4,3,3,2,2,6,7,4,4,1,1,1,3,2,5,4,4,5,1,1,3,1,1,1,1,1,1,1,1,1,
1,1,1,1,1};static short Cyc_yydefact[828]={0,134,35,36,37,38,40,52,54,55,56,57,58,
59,60,61,71,72,73,86,87,42,0,0,41,0,0,111,112,0,0,400,154,402,401,403,0,53,0,139,
140,143,1,0,9,0,0,10,0,42,42,42,62,63,0,64,0,0,122,0,144,65,124,84,39,0,33,43,0,
75,283,400,279,282,281,0,277,0,0,0,0,0,177,0,284,17,19,0,0,0,0,0,0,0,2,0,0,0,0,
21,0,91,92,94,27,29,31,0,404,405,84,147,0,146,84,38,0,25,0,0,11,156,0,0,132,123,
42,145,133,0,66,0,0,34,79,0,77,0,0,291,290,280,289,24,0,0,0,0,0,42,42,195,197,0,
0,406,407,155,70,125,0,0,172,0,0,399,0,0,0,0,0,0,0,0,0,84,0,0,396,386,397,398,0,
0,0,0,371,0,369,370,0,302,315,323,325,327,329,331,333,336,341,344,347,351,0,353,
372,385,384,0,3,0,5,0,22,0,0,0,12,28,30,32,0,88,89,0,83,84,0,109,39,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,400,229,231,0,237,233,0,235,218,219,220,0,221,222,223,0,300,26,
13,94,162,178,0,0,158,156,0,202,0,126,0,135,0,0,0,0,76,0,0,278,293,0,292,179,0,0,
291,0,198,170,0,97,99,156,0,204,196,205,409,0,408,68,69,0,174,0,176,67,0,0,361,0,
317,351,0,318,319,0,0,0,0,0,0,0,0,0,354,355,0,0,0,0,357,358,356,141,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,305,306,307,308,309,310,311,312,313,314,304,0,359,0,
378,379,0,0,0,388,0,0,142,18,0,20,0,93,95,181,180,14,81,90,0,0,101,102,104,0,108,
84,117,0,0,0,0,0,0,0,270,271,272,0,0,274,0,0,226,227,0,0,0,0,238,234,94,236,232,
230,0,163,0,0,0,169,157,164,128,0,0,0,158,130,0,131,127,148,137,136,85,0,48,47,0,
45,80,322,78,74,286,0,23,287,0,0,0,0,191,295,298,0,297,0,0,0,199,98,100,0,158,0,
208,0,206,156,0,0,217,201,173,0,0,0,182,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,387,394,
0,393,324,0,326,328,330,332,334,335,339,340,337,338,342,343,345,346,348,349,350,
303,377,374,0,376,0,0,0,4,6,105,96,0,0,0,110,119,118,0,0,113,0,0,0,0,0,0,0,0,269,
273,0,0,0,0,225,0,228,0,15,301,156,0,166,0,0,0,159,129,162,150,203,149,138,7,0,0,
0,294,194,0,296,190,192,288,0,285,200,171,213,0,207,210,0,158,0,209,0,360,0,0,
183,0,187,382,383,0,365,0,0,0,0,0,0,0,0,352,390,0,0,375,373,391,0,0,103,106,82,
107,120,117,117,0,0,0,0,0,0,0,0,0,0,244,275,0,0,0,16,158,0,167,165,0,158,0,0,0,0,
44,46,193,299,212,215,0,216,211,0,0,0,184,188,0,0,0,0,320,321,364,392,0,395,316,
389,0,116,115,239,0,244,254,0,0,0,0,0,0,0,0,0,0,0,0,276,0,0,152,168,160,151,156,
0,49,50,214,363,362,0,189,366,0,0,380,0,0,0,250,0,0,256,0,0,0,264,0,0,0,0,0,0,0,
0,243,0,224,0,158,0,0,0,0,381,0,240,0,0,0,241,255,257,258,0,266,265,0,260,0,0,0,
0,244,245,0,161,153,0,0,0,0,121,0,0,242,259,267,261,262,0,0,244,246,0,0,185,367,
0,250,251,263,244,247,268,51,0,250,252,244,248,368,253,249,0,0,0};static short Cyc_yydefgoto[
116]={825,42,43,44,250,45,385,46,387,47,217,48,49,66,67,455,456,50,157,51,52,134,
135,53,130,54,222,223,100,101,102,224,151,396,397,398,55,56,552,553,554,57,58,59,
60,126,112,450,480,61,481,441,581,433,437,438,296,271,162,81,82,496,390,497,498,
468,469,152,273,274,482,303,253,254,255,256,257,258,716,770,259,260,286,287,288,
471,472,473,261,262,372,191,459,192,193,194,195,196,197,198,199,200,201,202,203,
204,205,206,513,514,207,208,63,110,158,307};static short Cyc_yypact[828]={1987,-
-32768,- -32768,- -32768,- -32768,- 66,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
20,172,697,- -32768,131,28,- -32768,- -32768,35,97,- -32768,149,- -32768,- -32768,-
-32768,481,- -32768,139,154,193,- -32768,- -32768,1871,- -32768,- 5,85,- -32768,332,-
20,- 20,- 20,- -32768,- -32768,508,- -32768,110,2457,308,39,27,- -32768,131,163,204,
198,2582,- -32768,131,216,- -32768,- 60,- -32768,- -32768,- -32768,816,- -32768,234,250,
268,816,283,262,287,15,- -32768,- -32768,4851,4851,247,290,5022,4408,4408,- -32768,
1871,2102,1871,2102,- -32768,321,317,- -32768,2393,2582,2582,2582,5022,- -32768,-
-32768,163,- -32768,131,- -32768,61,334,1263,- -32768,2457,332,- -32768,1807,4851,
3080,- -32768,308,- 20,- -32768,- -32768,4851,- -32768,2102,340,- -32768,350,372,376,
131,379,816,- -32768,- -32768,- -32768,- -32768,399,4408,156,391,185,- 20,- 20,100,-
-32768,42,42,- -32768,- -32768,- -32768,- -32768,- -32768,540,243,- -32768,4636,394,-
-32768,4408,4484,396,420,422,424,432,434,439,163,4712,4712,- -32768,- -32768,-
-32768,- -32768,1627,446,4788,4788,- -32768,4788,- -32768,- -32768,410,- -32768,- 6,
477,433,435,447,408,104,336,178,64,- -32768,756,4788,436,- -32768,- -32768,76,475,-
-32768,486,- -32768,497,- -32768,481,3163,2457,- -32768,- -32768,- -32768,- -32768,513,
5022,- -32768,466,482,163,131,489,- -32768,506,119,514,2665,525,543,541,551,3246,
2665,171,549,2665,2665,- 17,547,- -32768,- -32768,556,1386,1386,332,1386,- -32768,-
-32768,- -32768,567,- -32768,- -32768,- -32768,254,- -32768,- -32768,- -32768,583,553,-
-32768,56,566,571,258,584,585,24,- -32768,587,67,24,613,12,131,- -32768,4408,618,-
-32768,- -32768,599,615,- -32768,263,783,816,4851,- -32768,616,623,5022,5022,2332,
3329,- 1,- -32768,368,- -32768,56,- -32768,- -32768,- -32768,2224,- -32768,409,- -32768,
- -32768,5022,1627,- -32768,5022,- -32768,- -32768,2748,- -32768,653,4408,2163,4408,
4408,4408,4408,4408,626,1627,- -32768,- -32768,1386,628,426,4408,- -32768,- -32768,-
-32768,- -32768,4788,4408,4788,4788,4788,4788,4788,4788,4788,4788,4788,4788,4788,
4788,4788,4788,4788,4788,4788,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,4408,- -32768,247,- -32768,- -32768,3412,247,
4408,- -32768,2997,622,- -32768,- -32768,1871,- -32768,1871,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,4408,634,629,- -32768,627,5022,- -32768,163,539,
4408,636,4408,4408,707,1509,641,- -32768,- -32768,- -32768,278,699,- -32768,3495,
2521,- -32768,- -32768,2665,651,2665,1745,- -32768,- -32768,2393,- -32768,- -32768,-
-32768,4408,- -32768,5022,645,189,- -32768,- -32768,633,- -32768,656,640,4922,571,-
-32768,4851,- -32768,- -32768,- -32768,- -32768,67,- -32768,1871,- -32768,648,655,647,
- -32768,- -32768,- -32768,- -32768,- -32768,816,- -32768,- -32768,247,4408,816,124,-
-32768,- -32768,- -32768,654,649,659,63,670,- -32768,- -32768,- -32768,662,571,663,-
-32768,661,368,4907,4851,3578,- -32768,- -32768,- -32768,664,669,687,- -32768,-
-32768,220,3163,455,673,481,488,676,678,679,507,5022,681,690,4560,- -32768,-
-32768,688,689,477,298,433,435,447,408,104,104,336,336,336,336,178,178,64,64,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,691,- -32768,180,322,4851,- -32768,
- -32768,- -32768,- -32768,466,4408,695,- -32768,- -32768,- -32768,701,131,279,515,
4408,517,520,702,3661,3744,325,- -32768,- -32768,698,708,705,712,- -32768,706,-
-32768,2457,- -32768,- -32768,713,4851,- -32768,718,56,714,- -32768,- -32768,206,-
-32768,585,- -32768,- -32768,- -32768,511,720,12,- -32768,- -32768,694,- -32768,-
-32768,- -32768,- -32768,568,- -32768,- -32768,- -32768,- -32768,721,- -32768,- -32768,
722,571,24,- -32768,715,728,240,734,- -32768,2831,- -32768,- -32768,- -32768,2457,-
-32768,4408,804,4408,4484,726,728,739,2997,- -32768,- -32768,4408,4408,- -32768,-
-32768,- -32768,2997,24,- -32768,- -32768,- -32768,- -32768,99,31,539,2665,522,745,
2665,4408,3827,335,3910,345,3993,608,- -32768,747,4408,759,- -32768,571,127,-
-32768,- -32768,757,571,5022,748,751,752,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,753,- -32768,- -32768,761,762,4408,- -32768,- -32768,3163,533,749,758,-
-32768,653,- -32768,- -32768,363,- -32768,- -32768,- -32768,764,- -32768,- -32768,833,
769,608,- -32768,542,2665,558,4076,2665,562,4159,4242,357,816,768,770,- -32768,360,
2665,- -32768,- -32768,775,- -32768,713,791,- -32768,- -32768,- -32768,- -32768,- -32768,
384,- -32768,- -32768,5022,857,- -32768,2914,5022,2665,650,782,786,- -32768,2665,
2665,578,- -32768,2665,2665,586,2665,589,4325,16,1386,- -32768,4408,- -32768,656,
571,790,4408,784,788,- -32768,591,- -32768,4408,778,796,- -32768,- -32768,- -32768,-
-32768,2665,- -32768,- -32768,2665,- -32768,2665,2665,602,4408,990,- -32768,604,-
-32768,- -32768,808,405,795,5022,- -32768,412,1386,- -32768,- -32768,- -32768,- -32768,
- -32768,2665,417,608,- -32768,2665,799,- -32768,- -32768,800,1140,- -32768,- -32768,
990,- -32768,- -32768,- -32768,803,650,- -32768,608,- -32768,- -32768,- -32768,- -32768,
900,910,- -32768};static short Cyc_yypgoto[116]={- -32768,248,- -32768,590,- -32768,-
-32768,- -32768,- -32768,- -32768,- 53,- 25,- 57,- -32768,201,19,320,- -32768,9,- -32768,
- 69,- -32768,- -32768,- 103,- -32768,- 2,- -32768,516,- -32768,- -32768,- -32768,700,703,
- 34,- -32768,- -32768,367,- -32768,- -32768,66,- -32768,- -32768,32,- 24,- 11,- -32768,-
-32768,858,- -32768,- 94,- 50,- 114,- 389,165,342,348,625,- 377,- 90,- 272,807,- -32768,-
201,- 162,- 360,- 288,- -32768,463,- 169,- 76,- 112,93,- 177,69,- -32768,- -32768,- 29,- 227,
- -32768,- 648,- 609,- -32768,- -32768,- 9,644,- -32768,333,- -32768,- -32768,209,60,-
-32768,- 146,- 352,- 151,596,612,588,611,614,364,221,362,369,54,595,- -32768,- -32768,
- -32768,- 336,- -32768,- -32768,8,904,- -32768,- 299,- 152};static short Cyc_yytable[
5130]={119,161,308,467,117,321,113,270,62,133,113,153,154,335,83,389,322,278,150,
150,318,540,150,425,426,64,428,269,120,453,69,84,118,85,284,125,550,551,150,- 178,
1,537,491,544,62,- 178,119,219,220,221,117,62,150,150,584,741,62,160,578,252,150,
119,108,251,114,263,138,62,90,342,128,142,421,225,218,535,136,124,65,538,103,567,
146,84,16,17,18,160,84,264,783,422,604,304,95,96,149,149,295,602,149,454,299,62,
62,62,62,509,226,343,300,304,229,150,594,- 84,149,86,- 84,434,227,32,446,129,485,
784,451,62,31,445,149,149,- 114,111,485,34,804,458,149,62,301,306,36,87,124,136,
492,84,493,311,32,265,190,209,150,814,435,443,305,306,119,575,508,380,263,821,
593,601,62,129,491,228,824,330,350,351,381,358,457,305,39,40,41,276,97,98,359,
360,392,225,382,149,310,252,252,640,252,251,251,31,251,819,- 84,299,32,88,34,129,
448,823,405,300,352,39,40,41,111,475,436,678,31,353,406,62,150,401,596,34,150,
150,150,721,149,62,465,466,402,89,338,339,150,340,91,305,302,150,150,265,150,458,
104,105,106,312,92,436,291,399,373,292,62,31,150,478,479,160,449,501,34,694,416,
129,68,720,417,160,391,252,723,32,160,251,470,32,427,431,294,125,301,136,577,94,
635,93,356,432,357,617,311,84,84,132,149,408,131,268,149,149,149,415,- 156,467,
419,420,682,- 156,137,62,149,62,458,615,489,149,149,616,149,277,686,155,681,150,
62,1,161,156,155,139,626,140,149,90,156,210,211,212,213,313,314,686,297,298,119,
430,290,150,561,268,141,431,484,119,463,442,150,568,225,144,431,585,119,788,608,
150,117,609,150,564,644,279,391,586,143,431,645,295,503,504,505,145,160,336,607,
62,159,62,724,512,573,160,549,458,572,431,354,355,65,633,160,149,1,121,531,532,
533,685,122,150,150,123,214,31,215,636,655,230,295,637,34,638,431,99,534,149,708,
36,685,512,150,587,431,391,149,280,711,39,40,41,414,686,431,149,160,592,149,281,
753,679,595,757,62,662,431,690,736,431,65,1,737,667,84,486,150,282,691,84,487,
512,283,488,348,349,1,285,512,732,697,696,289,431,569,574,571,762,293,149,149,
317,31,323,663,374,375,376,489,34,807,150,- 204,62,431,341,309,- 204,- 204,119,149,
431,- 204,263,300,810,431,324,336,325,813,326,785,436,511,431,499,620,502,327,685,
328,506,377,336,1,329,661,550,551,378,379,611,337,149,344,516,345,62,31,346,803,
391,618,431,643,34,119,630,763,347,117,811,36,31,523,524,525,526,395,383,34,399,
39,40,41,400,818,36,149,820,384,539,403,470,218,621,431,39,40,41,31,386,150,669,
670,671,33,34,35,70,84,107,404,760,625,431,555,393,557,558,407,562,646,431,648,
431,809,649,431,702,431,409,31,714,715,410,542,160,543,34,733,431,- 175,411,698,
432,309,- 175,- 175,742,431,766,- 175,412,300,418,39,40,41,423,31,72,424,73,74,744,
431,34,150,748,431,429,150,439,75,768,769,76,77,391,149,465,466,440,78,775,431,
689,216,79,80,444,391,778,431,695,780,431,793,314,391,252,447,588,305,251,461,
160,754,801,431,805,431,699,700,521,522,452,701,527,528,704,460,462,476,84,150,
477,529,530,252,342,507,541,251,510,545,546,70,547,559,252,556,563,565,251,149,
582,570,391,149,576,579,580,589,591,252,599,598,252,251,316,590,251,319,319,600,
32,647,603,605,612,652,654,332,333,606,613,743,614,619,747,319,319,622,319,623,
624,627,71,72,758,73,74,628,631,34,632,634,641,391,656,319,75,149,642,76,77,650,
674,657,767,658,78,659,268,773,774,79,80,776,777,664,779,70,666,660,672,676,677,
680,629,688,692,687,361,362,363,364,365,366,367,368,369,370,683,693,797,703,717,
798,719,799,800,722,734,725,70,726,727,728,371,705,707,739,710,735,713,729,730,
738,718,740,812,756,31,72,815,73,74,755,319,34,759,761,764,771,464,772,75,795,
791,76,77,792,731,465,466,789,78,796,806,808,826,79,80,816,817,31,72,822,73,74,
827,673,34,639,500,388,548,746,127,75,750,752,76,77,787,668,394,665,272,78,490,
597,675,518,79,80,474,319,515,319,319,319,319,319,319,319,319,319,319,319,319,
319,319,319,319,319,517,519,109,0,0,520,782,0,0,0,786,0,0,0,0,790,0,0,0,0,0,794,
0,0,0,0,0,0,0,0,0,0,0,0,319,0,802,2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,714,715,21,163,164,231,0,232,233,234,235,236,237,238,239,22,165,23,166,240,
0,167,24,241,0,0,27,28,242,168,169,243,244,170,171,172,173,29,30,245,174,0,175,0,
176,177,0,0,0,319,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,246,178,179,180,181,32,33,34,
35,0,247,116,0,0,182,0,0,37,249,0,0,0,0,0,184,0,319,185,0,186,187,0,0,0,0,188,
189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,319,2,3,4,115,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,768,769,21,163,164,231,0,232,233,234,235,236,237,
238,239,22,165,23,166,240,0,167,24,241,0,0,27,28,242,168,169,243,244,170,171,172,
173,29,30,245,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,319,0,0,0,0,0,0,0,319,
246,178,179,180,181,32,33,34,35,0,247,116,0,0,182,0,0,37,249,0,0,0,0,0,184,0,0,
185,0,186,187,0,0,0,0,188,189,2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,21,163,164,231,0,232,233,234,235,236,237,238,239,22,165,23,166,240,0,167,24,
241,0,0,27,28,242,168,169,243,244,170,171,172,173,29,30,245,174,0,175,0,176,177,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,246,178,179,180,181,32,33,34,35,0,247,
116,248,0,182,0,0,37,249,0,0,0,0,0,184,0,0,185,0,186,187,0,0,0,0,188,189,2,3,4,
115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,163,164,231,0,232,233,234,
235,236,237,238,239,22,165,23,166,240,0,167,24,241,0,0,27,28,242,168,169,243,244,
170,171,172,173,29,30,245,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,246,178,179,180,181,32,33,34,35,0,247,116,0,0,182,0,0,37,249,0,0,0,0,0,
184,0,0,185,0,186,187,0,0,0,0,188,189,2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,163,164,0,0,0,0,0,0,0,0,0,0,22,165,23,166,0,0,167,24,0,0,0,27,28,
0,168,169,0,0,170,171,172,173,29,30,0,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,31,178,179,180,181,32,33,34,35,0,560,0,0,0,182,0,0,37,249,0,
0,0,0,0,184,0,0,185,0,186,187,0,0,0,0,188,189,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,163,164,0,0,0,0,0,0,0,0,0,0,22,165,0,166,0,0,167,0,0,0,0,27,28,0,168,
169,0,0,170,171,172,173,29,30,0,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,31,178,179,180,181,32,33,34,35,0,0,334,0,0,182,0,0,37,249,0,0,0,0,
0,184,0,0,185,0,186,187,0,0,0,0,188,189,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,0,163,164,0,0,0,0,0,0,0,0,0,0,22,165,0,166,0,0,167,0,0,0,0,27,28,0,168,169,0,0,
170,171,172,173,29,30,0,174,0,175,0,176,177,0,0,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,0,0,31,178,179,180,181,32,33,34,35,0,22,0,0,0,182,0,0,37,249,0,0,27,
28,0,184,0,0,185,0,186,187,0,29,30,0,188,189,- 8,1,0,2,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,266,0,21,267,0,0,0,0,32,33,0,35,0,268,0,22,0,23,0,0,37,38,
24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,31,0,0,0,0,32,33,34,35,0,0,0,- 8,0,36,0,0,37,38,0,0,0,0,0,39,40,
41,- 8,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,
0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,32,33,34,35,0,0,0,0,0,36,0,
0,37,38,0,0,0,0,0,39,40,41,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,
0,29,30,0,1,0,2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,31,
0,0,0,0,32,33,34,35,22,0,0,- 8,0,36,0,24,37,38,0,27,28,0,0,39,40,41,0,0,0,0,29,30,
0,1,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,31,0,0,0,0,32,
33,34,35,22,0,0,0,0,36,0,0,37,38,0,27,28,0,0,39,40,41,0,0,0,0,29,30,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,266,0,0,31,0,0,0,0,32,33,34,35,0,268,0,0,0,
309,- 156,0,37,38,0,- 156,0,300,0,39,40,41,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,
30,0,0,0,2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,266,0,0,0,0,0,
0,0,32,33,0,35,22,268,23,0,0,299,0,24,37,38,0,27,28,300,0,39,40,41,0,0,0,0,29,30,
0,0,0,0,0,0,2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,
32,33,0,35,0,0,116,22,216,23,0,0,37,38,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,
0,0,0,0,0,2,3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,32,
33,0,35,0,0,116,22,0,23,0,0,37,38,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,2,
3,4,115,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,32,33,0,
35,22,0,0,0,0,0,0,24,37,38,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,33,0,35,0,0,0,0,0,0,0,0,37,38,163,
164,231,0,232,233,234,235,236,237,238,239,0,165,0,166,240,0,167,0,241,0,0,0,0,
242,168,169,243,244,170,171,172,173,0,0,245,174,0,175,0,176,177,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,246,178,179,180,181,0,0,34,0,0,247,116,0,0,182,0,0,0,
183,163,164,0,0,0,184,0,494,185,0,186,187,0,165,0,166,188,189,167,0,0,0,0,0,0,0,
168,169,0,0,170,171,172,173,0,0,0,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,31,178,179,180,181,0,0,34,0,0,0,320,495,0,182,0,0,0,183,163,164,
465,466,0,184,0,0,185,0,186,187,0,165,0,166,188,189,167,0,0,0,0,0,0,0,168,169,0,
0,170,171,172,173,0,0,0,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,31,178,179,180,181,0,0,34,0,0,0,320,684,0,182,0,0,0,183,163,164,465,466,0,
184,0,0,185,0,186,187,0,165,0,166,188,189,167,0,0,0,0,0,0,0,168,169,0,0,170,171,
172,173,0,0,0,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,
178,179,180,181,0,0,34,0,0,0,320,765,0,182,0,0,0,183,163,164,465,466,0,184,0,0,
185,0,186,187,0,165,0,166,188,189,167,0,0,0,0,0,0,0,168,169,0,0,170,171,172,173,
0,0,0,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,178,179,
180,181,0,0,34,0,0,0,320,0,0,182,0,0,0,183,163,164,465,466,0,184,0,0,185,0,186,
187,0,165,0,166,188,189,167,0,0,0,0,0,0,0,168,169,0,0,170,171,172,173,0,0,0,174,
0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,178,179,180,181,0,0,
34,0,0,0,0,0,0,182,0,0,0,183,163,164,0,0,275,184,0,0,185,0,186,187,0,165,0,166,
188,189,167,0,0,0,0,0,0,0,168,169,0,0,170,171,172,173,0,0,0,174,0,175,0,176,177,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,178,179,180,181,0,0,34,0,0,0,320,0,
0,182,0,0,0,183,163,164,0,0,0,184,0,0,185,0,186,187,0,165,0,166,188,189,167,0,0,
0,0,0,0,0,168,169,0,0,170,171,172,173,0,0,0,174,0,175,0,176,177,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,31,178,179,180,181,0,0,34,0,0,413,0,0,0,182,0,0,0,183,
163,164,0,0,0,184,0,0,185,0,186,187,0,165,0,166,188,189,167,0,0,0,0,0,0,0,168,
169,0,0,170,171,172,173,0,0,0,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,31,178,179,180,181,0,0,34,0,0,0,0,0,0,182,0,0,0,183,163,164,0,0,483,
184,0,0,185,0,186,187,0,165,0,166,188,189,167,0,0,0,0,0,0,0,168,169,0,0,170,171,
172,173,0,0,0,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,
178,179,180,181,0,0,34,0,0,0,0,0,0,182,536,0,0,183,163,164,0,0,0,184,0,0,185,0,
186,187,0,165,0,166,188,189,167,0,0,0,0,0,0,0,168,169,0,0,170,171,172,173,0,0,0,
174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,178,179,180,181,
0,0,34,0,0,0,0,0,0,182,566,0,0,183,163,164,0,0,0,184,0,0,185,0,186,187,0,165,0,
166,188,189,167,0,0,0,0,0,0,0,168,169,0,0,170,171,172,173,0,0,0,174,0,175,0,176,
177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,178,179,180,181,0,0,34,0,0,0,0,
0,0,182,0,0,0,183,163,164,0,0,610,184,0,0,185,0,186,187,0,165,0,166,188,189,167,
0,0,0,0,0,0,0,168,169,0,0,170,171,172,173,0,0,0,174,0,175,0,176,177,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,178,179,180,181,0,0,34,0,0,651,0,0,0,182,0,0,0,
183,163,164,0,0,0,184,0,0,185,0,186,187,0,165,0,166,188,189,167,0,0,0,0,0,0,0,
168,169,0,0,170,171,172,173,0,0,0,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,31,178,179,180,181,0,0,34,0,0,653,0,0,0,182,0,0,0,183,163,164,0,
0,0,184,0,0,185,0,186,187,0,165,0,166,188,189,167,0,0,0,0,0,0,0,168,169,0,0,170,
171,172,173,0,0,0,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
31,178,179,180,181,0,0,34,0,0,0,0,0,0,182,706,0,0,183,163,164,0,0,0,184,0,0,185,
0,186,187,0,165,0,166,188,189,167,0,0,0,0,0,0,0,168,169,0,0,170,171,172,173,0,0,
0,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,178,179,180,
181,0,0,34,0,0,0,0,0,0,182,709,0,0,183,163,164,0,0,0,184,0,0,185,0,186,187,0,165,
0,166,188,189,167,0,0,0,0,0,0,0,168,169,0,0,170,171,172,173,0,0,0,174,0,175,0,
176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,178,179,180,181,0,0,34,0,0,
712,0,0,0,182,0,0,0,183,163,164,0,0,0,184,0,0,185,0,186,187,0,165,0,166,188,189,
167,0,0,0,0,0,0,0,168,169,0,0,170,171,172,173,0,0,0,174,0,175,0,176,177,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,178,179,180,181,0,0,34,0,0,0,0,0,0,182,745,
0,0,183,163,164,0,0,0,184,0,0,185,0,186,187,0,165,0,166,188,189,167,0,0,0,0,0,0,
0,168,169,0,0,170,171,172,173,0,0,0,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,31,178,179,180,181,0,0,34,0,0,0,0,0,0,182,749,0,0,183,163,164,
0,0,0,184,0,0,185,0,186,187,0,165,0,166,188,189,167,0,0,0,0,0,0,0,168,169,0,0,
170,171,172,173,0,0,0,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,31,178,179,180,181,0,0,34,0,0,0,0,0,0,182,751,0,0,183,163,164,0,0,0,184,0,0,
185,0,186,187,0,165,0,166,188,189,167,0,0,0,0,0,0,0,168,169,0,0,170,171,172,173,
0,0,0,174,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,178,179,
180,181,0,0,34,0,0,0,0,0,0,182,781,0,0,183,163,164,0,0,0,184,0,0,185,0,186,187,0,
165,0,166,188,189,167,0,0,0,0,0,0,0,168,169,0,0,170,171,172,173,0,0,0,174,0,175,
0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,178,179,180,181,0,0,34,0,
0,0,0,163,164,182,0,0,0,183,0,0,0,0,0,184,165,0,185,0,186,187,0,0,0,0,188,189,0,
168,169,0,0,170,171,172,173,0,0,0,0,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,31,178,179,180,181,0,0,34,0,0,0,320,163,164,182,0,0,0,183,0,0,0,0,
0,184,165,0,185,0,186,187,0,0,0,0,188,189,0,168,169,0,0,170,171,172,173,0,0,0,0,
0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,178,179,180,181,0,0,
34,0,0,0,629,163,164,182,0,0,0,183,0,0,0,0,0,184,165,0,185,0,186,187,0,0,0,0,188,
189,0,168,169,0,0,170,171,172,173,0,0,0,0,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,31,178,179,180,181,0,0,34,0,0,0,0,163,164,315,0,0,0,183,0,0,
0,0,0,184,165,0,185,0,186,187,0,0,0,0,188,189,0,168,169,0,0,170,171,172,173,0,0,
0,0,0,175,0,176,177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,178,179,180,181,
0,0,34,0,0,0,0,163,164,331,0,0,0,183,0,0,0,0,0,184,165,0,185,0,186,187,0,0,0,0,
188,189,0,168,169,0,0,170,171,172,173,0,0,0,0,0,175,0,176,177,0,0,0,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,0,0,31,178,179,180,181,0,0,34,0,0,0,22,0,0,182,0,
0,0,183,0,0,0,27,28,184,0,0,185,0,186,187,0,0,29,30,188,189,147,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,32,33,0,35,0,
0,148,0,0,0,27,28,37,38,22,0,0,0,0,0,0,29,30,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,
0,0,0,0,0,0,0,0,0,266,0,0,0,0,0,0,0,32,33,0,35,0,268,0,583,0,0,0,0,37,38,0,32,33,
0,35,0,0,0,0,0,0,0,0,37,38,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,33,0,35,0,0,0,0,0,0,0,0,
37,38};static short Cyc_yycheck[5130]={57,91,154,291,57,167,56,121,0,66,60,87,88,
182,23,216,167,129,87,88,166,381,91,250,251,91,253,121,57,17,22,23,57,25,137,59,
5,6,107,99,1,377,314,395,36,105,103,104,105,106,103,43,121,122,443,703,48,91,435,
116,129,118,54,116,56,118,75,59,36,75,62,80,89,107,103,374,68,58,98,378,48,417,
84,75,17,18,19,121,80,118,74,108,481,69,99,100,87,88,148,476,91,89,103,95,96,97,
98,334,110,115,111,69,114,182,466,100,107,89,103,63,112,94,274,108,301,109,278,
119,89,105,121,122,101,106,311,96,784,283,129,131,151,117,103,108,125,137,315,
139,317,160,94,119,92,93,223,803,100,271,116,117,217,433,331,87,217,813,465,104,
160,108,442,110,820,175,70,71,100,113,281,116,113,114,115,123,99,100,122,123,217,
223,114,182,160,250,251,547,253,250,251,89,253,810,103,103,94,108,96,108,277,818,
91,111,108,113,114,115,106,293,268,608,89,117,103,215,293,227,102,96,297,298,299,
104,223,225,110,111,228,88,184,185,309,187,103,116,151,314,315,215,317,395,49,50,
51,160,100,305,100,225,204,103,252,89,331,297,298,299,277,324,96,629,99,108,100,
662,103,309,216,334,667,94,314,334,291,94,252,105,101,311,299,281,101,43,112,100,
116,89,118,498,309,291,292,103,293,234,100,99,297,298,299,240,104,599,243,244,
613,109,100,309,309,311,466,101,303,314,315,105,317,126,616,89,90,400,324,1,424,
95,89,103,507,89,331,309,95,95,96,97,98,104,105,637,149,150,409,99,145,424,409,
99,90,105,300,418,99,105,433,418,400,105,105,445,427,760,486,442,427,487,445,99,
99,131,320,450,99,105,105,435,326,327,328,102,424,182,486,385,104,387,668,337,
427,433,402,547,427,105,68,69,98,109,442,400,1,103,358,359,360,616,108,486,487,
111,99,89,105,101,99,91,476,105,96,541,105,99,372,424,99,103,637,377,507,450,105,
381,433,103,99,113,114,115,239,737,105,442,486,462,445,105,99,609,467,99,452,575,
105,625,101,105,98,1,105,583,462,103,541,101,625,467,108,417,102,111,72,73,1,104,
424,686,638,633,89,105,421,431,423,109,103,486,487,103,89,103,576,65,66,67,485,
96,101,576,99,501,105,101,103,104,105,572,507,105,109,572,111,109,105,103,315,
103,109,103,755,579,104,105,323,501,325,103,737,103,329,103,331,1,103,572,5,6,
110,111,488,103,541,74,343,120,546,89,121,784,498,104,105,553,96,620,510,734,119,
620,795,103,89,350,351,352,353,109,101,96,546,113,114,115,100,810,103,576,813,
101,379,100,599,620,104,105,113,114,115,89,101,668,89,90,91,95,96,97,38,599,100,
103,724,104,105,404,101,406,407,103,409,104,105,104,105,792,104,105,104,105,103,
89,22,23,89,385,668,387,96,104,105,99,99,643,89,103,104,105,104,105,738,109,99,
111,103,113,114,115,109,89,90,103,92,93,104,105,96,734,104,105,101,738,104,103,
22,23,106,107,616,668,110,111,109,113,104,105,624,102,118,119,104,629,104,105,
632,104,105,104,105,637,755,112,452,116,755,104,738,714,104,105,104,105,644,645,
348,349,101,646,354,355,649,101,105,105,714,792,101,356,357,784,75,103,108,784,
104,99,105,38,109,30,795,103,99,42,795,734,104,94,686,738,103,116,94,103,105,810,
105,101,813,810,163,104,813,166,167,104,94,556,104,104,104,560,561,176,177,112,
105,706,89,104,709,184,185,105,187,105,105,104,89,90,719,92,93,101,104,96,105,
104,101,737,100,204,103,792,101,106,107,103,112,99,739,104,113,99,99,744,745,118,
119,748,749,101,751,38,108,117,104,104,104,112,100,25,104,622,76,77,78,79,80,81,
82,83,84,85,108,104,775,100,99,778,89,780,781,94,103,105,38,104,104,104,102,650,
651,28,653,105,655,104,104,103,659,100,801,101,89,90,805,92,93,109,283,96,105,90,
25,101,101,99,103,109,104,106,107,103,683,110,111,105,113,101,90,104,0,118,119,
104,104,89,90,104,92,93,0,591,96,546,324,215,400,708,60,103,711,712,106,107,759,
583,223,579,121,113,305,468,599,345,118,119,292,342,342,344,345,346,347,348,349,
350,351,352,353,354,355,356,357,358,359,360,344,346,54,- 1,- 1,347,753,- 1,- 1,- 1,
757,- 1,- 1,- 1,- 1,762,- 1,- 1,- 1,- 1,- 1,768,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,395,-
1,783,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,
30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,
57,58,59,60,61,62,- 1,64,- 1,66,67,- 1,- 1,- 1,466,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,97,- 1,99,100,- 1,- 1,103,- 1,- 1,106,107,- 1,
- 1,- 1,- 1,- 1,113,- 1,510,116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,547,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,
39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,- 1,64,- 1,
66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,625,- 1,- 1,- 1,- 1,- 1,- 1,- 1,633,89,90,91,
92,93,94,95,96,97,- 1,99,100,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,116,
- 1,118,119,- 1,- 1,- 1,- 1,124,125,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,
49,50,51,52,53,54,55,56,57,58,59,60,61,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,97,- 1,99,100,101,
- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,
32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,
59,60,61,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,89,90,91,92,93,94,95,96,97,- 1,99,100,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,-
1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,
- 1,43,44,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,- 1,62,- 1,64,- 1,66,67,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,
96,97,- 1,99,- 1,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,-
1,- 1,- 1,- 1,124,125,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,
56,57,58,59,60,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,97,- 1,- 1,100,- 1,- 1,103,- 1,- 1,106,107,-
1,- 1,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,
43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,- 1,62,- 1,64,- 1,66,67,- 1,- 1,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,
97,- 1,37,- 1,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,48,49,- 1,113,- 1,- 1,116,- 1,118,119,- 1,
59,60,- 1,124,125,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,86,- 1,
24,89,- 1,- 1,- 1,- 1,94,95,- 1,97,- 1,99,- 1,37,- 1,39,- 1,- 1,106,107,44,- 1,46,47,48,49,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,96,97,- 1,- 1,- 1,101,- 1,
103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,113,114,115,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,
- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,
96,97,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,113,114,115,1,- 1,3,4,5,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,
1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,89,- 1,-
1,- 1,- 1,94,95,96,97,37,- 1,- 1,101,- 1,103,- 1,44,106,107,- 1,48,49,- 1,- 1,113,114,115,
- 1,- 1,- 1,- 1,59,60,- 1,1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
- 1,- 1,- 1,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,96,97,37,- 1,- 1,- 1,- 1,103,- 1,- 1,106,107,- 1,
48,49,- 1,- 1,113,114,115,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,96,97,- 1,99,- 1,
- 1,- 1,103,104,- 1,106,107,- 1,109,- 1,111,- 1,113,114,115,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,- 1,97,37,99,
39,- 1,- 1,103,- 1,44,106,107,- 1,48,49,111,- 1,113,114,115,- 1,- 1,- 1,- 1,59,60,- 1,- 1,-
1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,-
1,- 1,94,95,- 1,97,- 1,- 1,100,37,102,39,- 1,- 1,106,107,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,94,95,- 1,97,- 1,- 1,100,37,- 1,39,- 1,- 1,106,107,44,
- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,- 1,97,37,- 1,- 1,
- 1,- 1,- 1,- 1,44,106,107,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,94,95,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,25,26,27,- 1,29,30,31,32,33,34,35,
36,- 1,38,- 1,40,41,- 1,43,- 1,45,- 1,- 1,- 1,- 1,50,51,52,53,54,55,56,57,58,- 1,- 1,61,62,
- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,
90,91,92,93,- 1,- 1,96,- 1,- 1,99,100,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,
32,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,
57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,101,- 1,103,- 1,- 1,- 1,107,25,
26,110,111,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,101,- 1,
103,- 1,- 1,- 1,107,25,26,110,111,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,
43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,
- 1,- 1,- 1,100,101,- 1,103,- 1,- 1,- 1,107,25,26,110,111,- 1,113,- 1,- 1,116,- 1,118,119,-
1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,-
1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,
90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,- 1,- 1,103,- 1,- 1,- 1,107,25,26,110,111,- 1,113,- 1,
- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,
57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,
- 1,- 1,112,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,- 1,- 1,103,
- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,-
1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,
99,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,
124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,
67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,
- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,112,113,- 1,- 1,116,- 1,118,
119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,113,-
1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,
56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,
26,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,-
1,- 1,- 1,107,25,26,- 1,- 1,112,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,-
1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,
99,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,
124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,
67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,
- 1,- 1,96,- 1,- 1,99,- 1,- 1,- 1,103,- 1,- 1,- 1,107,25,26,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,
119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,113,-
1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,
56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,
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
89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,26,- 1,- 1,- 1,113,-
1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,
56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,25,
26,- 1,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,38,- 1,40,124,125,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,25,26,103,-
1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,113,38,- 1,116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,- 1,51,52,
- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,25,26,103,- 1,- 1,
- 1,107,- 1,- 1,- 1,- 1,- 1,113,38,- 1,116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,- 1,51,52,- 1,-
1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,100,25,26,103,- 1,- 1,- 1,
107,- 1,- 1,- 1,- 1,- 1,113,38,- 1,116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,- 1,51,52,- 1,- 1,
55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,25,26,103,- 1,- 1,- 1,107,
- 1,- 1,- 1,- 1,- 1,113,38,- 1,116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,- 1,51,52,- 1,- 1,55,56,
57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,- 1,25,26,103,- 1,- 1,- 1,107,- 1,- 1,
- 1,- 1,- 1,113,38,- 1,116,- 1,118,119,- 1,- 1,- 1,- 1,124,125,- 1,51,52,- 1,- 1,55,56,57,58,
- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,-
1,- 1,- 1,89,90,91,92,93,- 1,- 1,96,- 1,- 1,- 1,37,- 1,- 1,103,- 1,- 1,- 1,107,- 1,- 1,- 1,48,
49,113,- 1,- 1,116,- 1,118,119,- 1,- 1,59,60,124,125,63,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,94,95,- 1,97,- 1,- 1,100,
- 1,- 1,- 1,48,49,106,107,37,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,- 1,97,
- 1,99,- 1,86,- 1,- 1,- 1,- 1,106,107,- 1,94,95,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,94,95,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107};unsigned char Cyc_Yyimpossible[
17]="\000\000\000\000Yyimpossible";unsigned char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror(struct _tagged_arr);extern int Cyc_yylex();static int Cyc_yychar=(
int)'\000';void*Cyc_yylval=(void*)Cyc_YYINITIALSVAL;static int Cyc_yynerrs=0;
struct _tuple15{struct _tagged_arr f1;void*f2;};static unsigned char _tmp392[8]="stdcall";
static unsigned char _tmp393[6]="cdecl";static unsigned char _tmp394[9]="fastcall";
static unsigned char _tmp395[9]="noreturn";static unsigned char _tmp396[6]="const";
static unsigned char _tmp397[8]="aligned";static unsigned char _tmp398[7]="packed";
static unsigned char _tmp399[7]="shared";static unsigned char _tmp39A[7]="unused";
static unsigned char _tmp39B[5]="weak";static unsigned char _tmp39C[10]="dllimport";
static unsigned char _tmp39D[10]="dllexport";static unsigned char _tmp39E[23]="no_instrument_function";
static unsigned char _tmp39F[12]="constructor";static unsigned char _tmp3A0[11]="destructor";
static unsigned char _tmp3A1[22]="no_check_memory_usage";struct _tuple16{void*f1;
void*f2;};struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(){
int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int yyssp_offset;short*yyss=(
short*)({unsigned int _tmp84C=10000;short*_tmp84D=(short*)_cycalloc_atomic(
_check_times(sizeof(short),_tmp84C));{unsigned int _tmp84E=_tmp84C;unsigned int i;
for(i=0;i < _tmp84E;i ++){_tmp84D[i]=(short)0;}};_tmp84D;});int yyvsp_offset;void**
yyvs=(void**)({unsigned int _tmp849=10000;void**_tmp84A=(void**)_cycalloc(
_check_times(sizeof(void*),_tmp849));{unsigned int _tmp84B=_tmp849;unsigned int i;
for(i=0;i < _tmp84B;i ++){_tmp84A[i]=Cyc_yylval;}};_tmp84A;});int yylsp_offset;
struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmp846=10000;struct Cyc_Yyltype*
_tmp847=(struct Cyc_Yyltype*)_cycalloc_atomic(_check_times(sizeof(struct Cyc_Yyltype),
_tmp846));{unsigned int _tmp848=_tmp846;unsigned int i;for(i=0;i < _tmp848;i ++){
_tmp847[i]=Cyc_yynewloc();}};_tmp847;});int yystacksize=10000;void*yyval=Cyc_yylval;
int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;
yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(
10000,++ yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(
yystacksize >= 10000){Cyc_yyerror(_tag_arr("parser stack overflow",sizeof(
unsigned char),22));(int)_throw((void*)Cyc_Yystack_overflow);}}goto yybackup;
yybackup: yyn=(int)Cyc_yypact[yystate];if(yyn == - 32768){goto yydefault;}if(Cyc_yychar
== - 2){Cyc_yychar=Cyc_yylex();}if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{
yychar1=(Cyc_yychar > 0? Cyc_yychar <= 353: 0)?(int)Cyc_yytranslate[
_check_known_subscript_notnull(354,Cyc_yychar)]: 242;}yyn +=yychar1;if((yyn < 0? 1:
yyn > 5129)? 1: Cyc_yycheck[yyn]!= yychar1){goto yydefault;}yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){if(yyn == - 32768){goto yyerrlab;}yyn=- yyn;goto yyreduce;}else{if(yyn == 0){
goto yyerrlab;}}if(yyn == 827){return 0;}if(Cyc_yychar != 0){Cyc_yychar=- 2;}yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0){yyerrstatus --;}yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
yystate];if(yyn == 0){goto yyerrlab;}yyreduce: yylen=(int)Cyc_yyr2[yyn];if(yylen > 0){
yyval=yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];}
switch(yyn){case 1: _LL26D: yyval=yyvs[yyvsp_offset];Cyc_Parse_parse_result=Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]);break;case 2: _LL26E: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B[0]=({struct Cyc_DeclList_tok_struct
_tmp30C;_tmp30C.tag=Cyc_DeclList_tok;_tmp30C.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]));_tmp30C;});_tmp30B;});break;case 3: _LL26F: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp30D=_cycalloc(sizeof(*_tmp30D));_tmp30D[0]=({
struct Cyc_DeclList_tok_struct _tmp30E;_tmp30E.tag=Cyc_DeclList_tok;_tmp30E.f1=({
struct Cyc_List_List*_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F->hd=({struct Cyc_Absyn_Decl*
_tmp310=_cycalloc(sizeof(*_tmp310));_tmp310->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp311=_cycalloc(sizeof(*_tmp311));_tmp311[0]=({struct Cyc_Absyn_Using_d_struct
_tmp312;_tmp312.tag=10;_tmp312.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp312.f2=Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]);_tmp312;});_tmp311;}));_tmp310->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp310;});_tmp30F->tl=0;_tmp30F;});_tmp30E;});_tmp30D;});
Cyc_Lex_leave_using();break;case 4: _LL270: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313[0]=({struct Cyc_DeclList_tok_struct
_tmp314;_tmp314.tag=Cyc_DeclList_tok;_tmp314.f1=({struct Cyc_List_List*_tmp315=
_cycalloc(sizeof(*_tmp315));_tmp315->hd=({struct Cyc_Absyn_Decl*_tmp316=_cycalloc(
sizeof(*_tmp316));_tmp316->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317[0]=({struct Cyc_Absyn_Using_d_struct
_tmp318;_tmp318.tag=10;_tmp318.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp318.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp318;});_tmp317;}));
_tmp316->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp316;});_tmp315->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp315;});_tmp314;});_tmp313;});break;case 5: _LL271: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp319=_cycalloc(sizeof(*_tmp319));_tmp319[0]=({
struct Cyc_DeclList_tok_struct _tmp31A;_tmp31A.tag=Cyc_DeclList_tok;_tmp31A.f1=({
struct Cyc_List_List*_tmp31B=_cycalloc(sizeof(*_tmp31B));_tmp31B->hd=({struct Cyc_Absyn_Decl*
_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp31D=_cycalloc(sizeof(*_tmp31D));_tmp31D[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp31E;_tmp31E.tag=9;_tmp31E.f1=({struct _tagged_arr*_tmp31F=_cycalloc(sizeof(*
_tmp31F));_tmp31F[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp31F;});_tmp31E.f2=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp31E;});_tmp31D;}));_tmp31C->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp31C;});_tmp31B->tl=0;_tmp31B;});_tmp31A;});_tmp319;});
Cyc_Lex_leave_namespace();break;case 6: _LL272: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp320=_cycalloc(sizeof(*_tmp320));_tmp320[0]=({struct Cyc_DeclList_tok_struct
_tmp321;_tmp321.tag=Cyc_DeclList_tok;_tmp321.f1=({struct Cyc_List_List*_tmp322=
_cycalloc(sizeof(*_tmp322));_tmp322->hd=({struct Cyc_Absyn_Decl*_tmp323=_cycalloc(
sizeof(*_tmp323));_tmp323->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp325;_tmp325.tag=9;_tmp325.f1=({struct _tagged_arr*_tmp326=_cycalloc(sizeof(*
_tmp326));_tmp326[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp326;});_tmp325.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp325;});_tmp324;}));
_tmp323->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp323;});_tmp322->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp322;});_tmp321;});_tmp320;});break;case 7: _LL273: if(Cyc_Std_strcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tag_arr("C",sizeof(unsigned char),2))!= 0){Cyc_Parse_err(_tag_arr("only extern \"C\" { ... } is allowed",
sizeof(unsigned char),35),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));}yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp327=_cycalloc(sizeof(*_tmp327));_tmp327[0]=({
struct Cyc_DeclList_tok_struct _tmp328;_tmp328.tag=Cyc_DeclList_tok;_tmp328.f1=({
struct Cyc_List_List*_tmp329=_cycalloc(sizeof(*_tmp329));_tmp329->hd=({struct Cyc_Absyn_Decl*
_tmp32A=_cycalloc(sizeof(*_tmp32A));_tmp32A->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp32C;_tmp32C.tag=11;_tmp32C.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp32C;});_tmp32B;}));
_tmp32A->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp32A;});_tmp329->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp329;});_tmp328;});_tmp327;});break;case 8: _LL274: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp32D=_cycalloc(sizeof(*_tmp32D));_tmp32D[0]=({
struct Cyc_DeclList_tok_struct _tmp32E;_tmp32E.tag=Cyc_DeclList_tok;_tmp32E.f1=0;
_tmp32E;});_tmp32D;});break;case 9: _LL275: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp32F=_cycalloc(sizeof(*_tmp32F));_tmp32F[0]=({struct Cyc_DeclList_tok_struct
_tmp330;_tmp330.tag=Cyc_DeclList_tok;_tmp330.f1=({struct Cyc_List_List*_tmp331=
_cycalloc(sizeof(*_tmp331));_tmp331->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp332=_cycalloc(sizeof(*_tmp332));_tmp332[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp333;_tmp333.tag=1;_tmp333.f1=Cyc_yyget_FnDecl_tok(yyvs[yyvsp_offset]);
_tmp333;});_tmp332;}),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp331->tl=0;_tmp331;});_tmp330;});_tmp32F;});
break;case 10: _LL276: yyval=yyvs[yyvsp_offset];break;case 11: _LL277: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp334=_cycalloc(sizeof(*_tmp334));_tmp334[0]=({
struct Cyc_FnDecl_tok_struct _tmp335;_tmp335.tag=Cyc_FnDecl_tok;_tmp335.f1=Cyc_Parse_make_function(
0,Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp335;});_tmp334;});break;case 12: _LL278: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp336=_cycalloc(sizeof(*_tmp336));_tmp336[0]=({
struct Cyc_FnDecl_tok_struct _tmp337;_tmp337.tag=Cyc_FnDecl_tok;_tmp337.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp338=_cycalloc(sizeof(*_tmp338));_tmp338->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp338;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp337;});_tmp336;});break;case 13: _LL279: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339[0]=({
struct Cyc_FnDecl_tok_struct _tmp33A;_tmp33A.tag=Cyc_FnDecl_tok;_tmp33A.f1=Cyc_Parse_make_function(
0,Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp33A;});_tmp339;});break;case 14: _LL27A: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp33B=_cycalloc(sizeof(*_tmp33B));_tmp33B[0]=({
struct Cyc_FnDecl_tok_struct _tmp33C;_tmp33C.tag=Cyc_FnDecl_tok;_tmp33C.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp33D;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp33C;});_tmp33B;});break;case 15: _LL27B: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp33E=_cycalloc(sizeof(*_tmp33E));_tmp33E[0]=({
struct Cyc_FnDecl_tok_struct _tmp33F;_tmp33F.tag=Cyc_FnDecl_tok;_tmp33F.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp340=_cycalloc(sizeof(*_tmp340));_tmp340->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp340;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp33F;});_tmp33E;});break;case 16: _LL27C: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp341=_cycalloc(sizeof(*_tmp341));_tmp341[0]=({
struct Cyc_FnDecl_tok_struct _tmp342;_tmp342.tag=Cyc_FnDecl_tok;_tmp342.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp343=_cycalloc(sizeof(*_tmp343));_tmp343->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp343;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp342;});_tmp341;});break;case 17: _LL27D: Cyc_Lex_enter_using(
Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]));yyval=yyvs[yyvsp_offset];break;case 18:
_LL27E: Cyc_Lex_leave_using();break;case 19: _LL27F: Cyc_Lex_enter_namespace(({
struct _tagged_arr*_tmp344=_cycalloc(sizeof(*_tmp344));_tmp344[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp344;}));yyval=yyvs[yyvsp_offset];break;case 20: _LL280: Cyc_Lex_leave_namespace();
break;case 21: _LL281: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp345=
_cycalloc(sizeof(*_tmp345));_tmp345[0]=({struct Cyc_DeclList_tok_struct _tmp346;
_tmp346.tag=Cyc_DeclList_tok;_tmp346.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp346;});
_tmp345;});break;case 22: _LL282: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347[0]=({struct Cyc_DeclList_tok_struct
_tmp348;_tmp348.tag=Cyc_DeclList_tok;_tmp348.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp348;});_tmp347;});break;case 23: _LL283: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp349=_cycalloc(sizeof(*_tmp349));_tmp349[0]=({
struct Cyc_DeclList_tok_struct _tmp34A;_tmp34A.tag=Cyc_DeclList_tok;_tmp34A.f1=({
struct Cyc_List_List*_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B->hd=Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
0,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp34B->tl=0;
_tmp34B;});_tmp34A;});_tmp349;});break;case 24: _LL284: {struct Cyc_List_List*
_tmp34C=0;{struct Cyc_List_List*_tmp34D=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp34D != 0;
_tmp34D=_tmp34D->tl){struct _tagged_arr*_tmp34E=(struct _tagged_arr*)_tmp34D->hd;
struct _tuple1*_tmp34F=({struct _tuple1*_tmp352=_cycalloc(sizeof(*_tmp352));
_tmp352->f1=Cyc_Absyn_rel_ns_null;_tmp352->f2=_tmp34E;_tmp352;});struct Cyc_Absyn_Vardecl*
_tmp350=Cyc_Absyn_new_vardecl(_tmp34F,Cyc_Absyn_wildtyp(0),0);_tmp34C=({struct
Cyc_List_List*_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351->hd=_tmp350;_tmp351->tl=
_tmp34C;_tmp351;});}}_tmp34C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp34C);yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp353=
_cycalloc(sizeof(*_tmp353));_tmp353[0]=({struct Cyc_DeclList_tok_struct _tmp354;
_tmp354.tag=Cyc_DeclList_tok;_tmp354.f1=({struct Cyc_List_List*_tmp355=_cycalloc(
sizeof(*_tmp355));_tmp355->hd=Cyc_Absyn_letv_decl(_tmp34C,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp355->tl=0;_tmp355;});_tmp354;});_tmp353;});break;}
case 25: _LL285: yyval=yyvs[yyvsp_offset];break;case 26: _LL286: yyval=(void*)({struct
Cyc_DeclList_tok_struct*_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356[0]=({struct
Cyc_DeclList_tok_struct _tmp357;_tmp357.tag=Cyc_DeclList_tok;_tmp357.f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_yyget_DeclList_tok(yyvs[yyvsp_offset]));_tmp357;});_tmp356;});break;case 27:
_LL287: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp358=_cycalloc(sizeof(*
_tmp358));_tmp358[0]=({struct Cyc_DeclSpec_tok_struct _tmp359;_tmp359.tag=Cyc_DeclSpec_tok;
_tmp359.f1=({struct Cyc_Parse_Declaration_spec*_tmp35A=_cycalloc(sizeof(*_tmp35A));
_tmp35A->sc=({struct Cyc_Core_Opt*_tmp35B=_cycalloc(sizeof(*_tmp35B));_tmp35B->v=(
void*)Cyc_yyget_StorageClass_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp35B;});_tmp35A->tq=Cyc_Absyn_empty_tqual();_tmp35A->type_specs=
0;_tmp35A->is_inline=0;_tmp35A->attributes=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp35A;});_tmp359;});_tmp358;});break;case 28: _LL288: if((Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->sc != 0){({void*_tmp35C[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tag_arr("Only one storage class is allowed in a declaration",
sizeof(unsigned char),51),_tag_arr(_tmp35C,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp35D=_cycalloc(sizeof(*_tmp35D));_tmp35D[0]=({
struct Cyc_DeclSpec_tok_struct _tmp35E;_tmp35E.tag=Cyc_DeclSpec_tok;_tmp35E.f1=({
struct Cyc_Parse_Declaration_spec*_tmp35F=_cycalloc(sizeof(*_tmp35F));_tmp35F->sc=({
struct Cyc_Core_Opt*_tmp360=_cycalloc(sizeof(*_tmp360));_tmp360->v=(void*)Cyc_yyget_StorageClass_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp360;});_tmp35F->tq=(
Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq;_tmp35F->type_specs=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp35F->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->is_inline;_tmp35F->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp35F;});_tmp35E;});_tmp35D;});break;case 29:
_LL289: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp361=_cycalloc(sizeof(*
_tmp361));_tmp361[0]=({struct Cyc_DeclSpec_tok_struct _tmp362;_tmp362.tag=Cyc_DeclSpec_tok;
_tmp362.f1=({struct Cyc_Parse_Declaration_spec*_tmp363=_cycalloc(sizeof(*_tmp363));
_tmp363->sc=0;_tmp363->tq=Cyc_Absyn_empty_tqual();_tmp363->type_specs=({struct
Cyc_List_List*_tmp364=_cycalloc(sizeof(*_tmp364));_tmp364->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp364->tl=0;
_tmp364;});_tmp363->is_inline=0;_tmp363->attributes=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp363;});_tmp362;});_tmp361;});break;case 30: _LL28A: yyval=(
void*)({struct Cyc_DeclSpec_tok_struct*_tmp365=_cycalloc(sizeof(*_tmp365));
_tmp365[0]=({struct Cyc_DeclSpec_tok_struct _tmp366;_tmp366.tag=Cyc_DeclSpec_tok;
_tmp366.f1=({struct Cyc_Parse_Declaration_spec*_tmp367=_cycalloc(sizeof(*_tmp367));
_tmp367->sc=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->sc;_tmp367->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->tq;_tmp367->type_specs=({struct Cyc_List_List*_tmp368=
_cycalloc(sizeof(*_tmp368));_tmp368->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp368->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp368;});_tmp367->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->is_inline;_tmp367->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp367;});_tmp366;});_tmp365;});break;case 31:
_LL28B: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp369=_cycalloc(sizeof(*
_tmp369));_tmp369[0]=({struct Cyc_DeclSpec_tok_struct _tmp36A;_tmp36A.tag=Cyc_DeclSpec_tok;
_tmp36A.f1=({struct Cyc_Parse_Declaration_spec*_tmp36B=_cycalloc(sizeof(*_tmp36B));
_tmp36B->sc=0;_tmp36B->tq=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp36B->type_specs=0;
_tmp36B->is_inline=0;_tmp36B->attributes=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp36B;});_tmp36A;});_tmp369;});break;case 32: _LL28C: yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C[0]=({
struct Cyc_DeclSpec_tok_struct _tmp36D;_tmp36D.tag=Cyc_DeclSpec_tok;_tmp36D.f1=({
struct Cyc_Parse_Declaration_spec*_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E->sc=(
Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->sc;_tmp36E->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq);_tmp36E->type_specs=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp36E->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->is_inline;_tmp36E->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp36E;});_tmp36D;});_tmp36C;});break;case 33:
_LL28D: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp36F=_cycalloc(sizeof(*
_tmp36F));_tmp36F[0]=({struct Cyc_DeclSpec_tok_struct _tmp370;_tmp370.tag=Cyc_DeclSpec_tok;
_tmp370.f1=({struct Cyc_Parse_Declaration_spec*_tmp371=_cycalloc(sizeof(*_tmp371));
_tmp371->sc=0;_tmp371->tq=Cyc_Absyn_empty_tqual();_tmp371->type_specs=0;_tmp371->is_inline=
1;_tmp371->attributes=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp371;});
_tmp370;});_tmp36F;});break;case 34: _LL28E: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372[0]=({struct Cyc_DeclSpec_tok_struct
_tmp373;_tmp373.tag=Cyc_DeclSpec_tok;_tmp373.f1=({struct Cyc_Parse_Declaration_spec*
_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->sc;_tmp374->tq=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq;
_tmp374->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->type_specs;
_tmp374->is_inline=1;_tmp374->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp374;});_tmp373;});_tmp372;});break;case 35:
_LL28F: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp375=_cycalloc(
sizeof(*_tmp375));_tmp375[0]=({struct Cyc_StorageClass_tok_struct _tmp376;_tmp376.tag=
Cyc_StorageClass_tok;_tmp376.f1=(void*)((void*)4);_tmp376;});_tmp375;});break;
case 36: _LL290: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp377=_cycalloc(
sizeof(*_tmp377));_tmp377[0]=({struct Cyc_StorageClass_tok_struct _tmp378;_tmp378.tag=
Cyc_StorageClass_tok;_tmp378.f1=(void*)((void*)5);_tmp378;});_tmp377;});break;
case 37: _LL291: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp379=_cycalloc(
sizeof(*_tmp379));_tmp379[0]=({struct Cyc_StorageClass_tok_struct _tmp37A;_tmp37A.tag=
Cyc_StorageClass_tok;_tmp37A.f1=(void*)((void*)3);_tmp37A;});_tmp379;});break;
case 38: _LL292: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp37B=_cycalloc(
sizeof(*_tmp37B));_tmp37B[0]=({struct Cyc_StorageClass_tok_struct _tmp37C;_tmp37C.tag=
Cyc_StorageClass_tok;_tmp37C.f1=(void*)((void*)1);_tmp37C;});_tmp37B;});break;
case 39: _LL293: if(Cyc_Std_strcmp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),
_tag_arr("C",sizeof(unsigned char),2))!= 0){Cyc_Parse_err(_tag_arr("only extern or extern \"C\" is allowed",
sizeof(unsigned char),37),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp37D=_cycalloc(sizeof(*_tmp37D));_tmp37D[0]=({struct Cyc_StorageClass_tok_struct
_tmp37E;_tmp37E.tag=Cyc_StorageClass_tok;_tmp37E.f1=(void*)((void*)2);_tmp37E;});
_tmp37D;});break;case 40: _LL294: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp37F=_cycalloc(sizeof(*_tmp37F));_tmp37F[0]=({struct Cyc_StorageClass_tok_struct
_tmp380;_tmp380.tag=Cyc_StorageClass_tok;_tmp380.f1=(void*)((void*)0);_tmp380;});
_tmp37F;});break;case 41: _LL295: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp381=_cycalloc(sizeof(*_tmp381));_tmp381[0]=({struct Cyc_StorageClass_tok_struct
_tmp382;_tmp382.tag=Cyc_StorageClass_tok;_tmp382.f1=(void*)((void*)6);_tmp382;});
_tmp381;});break;case 42: _LL296: yyval=(void*)({struct Cyc_AttributeList_tok_struct*
_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383[0]=({struct Cyc_AttributeList_tok_struct
_tmp384;_tmp384.tag=Cyc_AttributeList_tok;_tmp384.f1=0;_tmp384;});_tmp383;});
break;case 43: _LL297: yyval=yyvs[yyvsp_offset];break;case 44: _LL298: yyval=(void*)({
struct Cyc_AttributeList_tok_struct*_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385[0]=({
struct Cyc_AttributeList_tok_struct _tmp386;_tmp386.tag=Cyc_AttributeList_tok;
_tmp386.f1=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp386;});_tmp385;});break;case 45: _LL299: yyval=(void*)({
struct Cyc_AttributeList_tok_struct*_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387[0]=({
struct Cyc_AttributeList_tok_struct _tmp388;_tmp388.tag=Cyc_AttributeList_tok;
_tmp388.f1=({struct Cyc_List_List*_tmp389=_cycalloc(sizeof(*_tmp389));_tmp389->hd=(
void*)Cyc_yyget_Attribute_tok(yyvs[yyvsp_offset]);_tmp389->tl=0;_tmp389;});
_tmp388;});_tmp387;});break;case 46: _LL29A: yyval=(void*)({struct Cyc_AttributeList_tok_struct*
_tmp38A=_cycalloc(sizeof(*_tmp38A));_tmp38A[0]=({struct Cyc_AttributeList_tok_struct
_tmp38B;_tmp38B.tag=Cyc_AttributeList_tok;_tmp38B.f1=({struct Cyc_List_List*
_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C->hd=(void*)Cyc_yyget_Attribute_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp38C->tl=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp38C;});_tmp38B;});_tmp38A;});break;case 47: _LL29B: {
static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};static struct _tuple15
att_map[16]={{{_tmp392,_tmp392,_tmp392 + 8},(void*)0},{{_tmp393,_tmp393,_tmp393 + 
6},(void*)1},{{_tmp394,_tmp394,_tmp394 + 9},(void*)2},{{_tmp395,_tmp395,_tmp395 + 
9},(void*)3},{{_tmp396,_tmp396,_tmp396 + 6},(void*)4},{{_tmp397,_tmp397,_tmp397 + 
8},(void*)& att_aligned},{{_tmp398,_tmp398,_tmp398 + 7},(void*)5},{{_tmp399,
_tmp399,_tmp399 + 7},(void*)7},{{_tmp39A,_tmp39A,_tmp39A + 7},(void*)8},{{_tmp39B,
_tmp39B,_tmp39B + 5},(void*)9},{{_tmp39C,_tmp39C,_tmp39C + 10},(void*)10},{{
_tmp39D,_tmp39D,_tmp39D + 10},(void*)11},{{_tmp39E,_tmp39E,_tmp39E + 23},(void*)12},{{
_tmp39F,_tmp39F,_tmp39F + 12},(void*)13},{{_tmp3A0,_tmp3A0,_tmp3A0 + 11},(void*)14},{{
_tmp3A1,_tmp3A1,_tmp3A1 + 22},(void*)15}};struct _tagged_arr _tmp38D=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);if((((_get_arr_size(_tmp38D,sizeof(unsigned char))> 4?*((
const unsigned char*)_check_unknown_subscript(_tmp38D,sizeof(unsigned char),0))== '_':
0)?*((const unsigned char*)_check_unknown_subscript(_tmp38D,sizeof(unsigned char),
1))== '_': 0)?*((const unsigned char*)_check_unknown_subscript(_tmp38D,sizeof(
unsigned char),(int)(_get_arr_size(_tmp38D,sizeof(unsigned char))- 2)))== '_': 0)?*((
const unsigned char*)_check_unknown_subscript(_tmp38D,sizeof(unsigned char),(int)(
_get_arr_size(_tmp38D,sizeof(unsigned char))- 3)))== '_': 0){_tmp38D=(struct
_tagged_arr)Cyc_Std_substring(_tmp38D,2,_get_arr_size(_tmp38D,sizeof(
unsigned char))- 5);}{int i=0;for(0;i < 16;++ i){if(Cyc_Std_strcmp(_tmp38D,(att_map[
i]).f1)== 0){yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp38E=_cycalloc(
sizeof(*_tmp38E));_tmp38E[0]=({struct Cyc_Attribute_tok_struct _tmp38F;_tmp38F.tag=
Cyc_Attribute_tok;_tmp38F.f1=(void*)(att_map[i]).f2;_tmp38F;});_tmp38E;});break;}}
if(i == 16){Cyc_Parse_err(_tag_arr("unrecognized attribute",sizeof(unsigned char),
23),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp390=
_cycalloc(sizeof(*_tmp390));_tmp390[0]=({struct Cyc_Attribute_tok_struct _tmp391;
_tmp391.tag=Cyc_Attribute_tok;_tmp391.f1=(void*)((void*)1);_tmp391;});_tmp390;});}
break;}}case 48: _LL29C: yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3A3=
_cycalloc(sizeof(*_tmp3A3));_tmp3A3[0]=({struct Cyc_Attribute_tok_struct _tmp3A4;
_tmp3A4.tag=Cyc_Attribute_tok;_tmp3A4.f1=(void*)((void*)4);_tmp3A4;});_tmp3A3;});
break;case 49: _LL29D: {struct _tagged_arr _tmp3A5=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct _tuple12 _tmp3A7;
int _tmp3A8;struct _tuple12*_tmp3A6=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3A7=*_tmp3A6;_LL2A0:
_tmp3A8=_tmp3A7.f2;goto _LL29F;_LL29F: {void*a;if(Cyc_Std_zstrcmp(_tmp3A5,
_tag_arr("regparm",sizeof(unsigned char),8))== 0? 1: Cyc_Std_zstrcmp(_tmp3A5,
_tag_arr("__regparm__",sizeof(unsigned char),12))== 0){if(_tmp3A8 < 0? 1: _tmp3A8 > 
3){Cyc_Parse_err(_tag_arr("regparm requires value between 0 and 3",sizeof(
unsigned char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp3A9=_cycalloc_atomic(sizeof(*_tmp3A9));
_tmp3A9[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp3AA;_tmp3AA.tag=0;_tmp3AA.f1=
_tmp3A8;_tmp3AA;});_tmp3A9;});}else{if(Cyc_Std_zstrcmp(_tmp3A5,_tag_arr("aligned",
sizeof(unsigned char),8))== 0? 1: Cyc_Std_zstrcmp(_tmp3A5,_tag_arr("__aligned__",
sizeof(unsigned char),12))== 0){if(_tmp3A8 < 0){Cyc_Parse_err(_tag_arr("aligned requires positive power of two",
sizeof(unsigned char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp3A8;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
Cyc_Parse_err(_tag_arr("aligned requires positive power of two",sizeof(
unsigned char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp3AB=_cycalloc_atomic(sizeof(*_tmp3AB));
_tmp3AB[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp3AC;_tmp3AC.tag=1;_tmp3AC.f1=
_tmp3A8;_tmp3AC;});_tmp3AB;});}}else{Cyc_Parse_err(_tag_arr("unrecognized attribute",
sizeof(unsigned char),23),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));
_tmp3AD[0]=({struct Cyc_Attribute_tok_struct _tmp3AE;_tmp3AE.tag=Cyc_Attribute_tok;
_tmp3AE.f1=(void*)a;_tmp3AE;});_tmp3AD;});break;}}case 50: _LL29E: {struct
_tagged_arr _tmp3AF=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp3B0=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_Std_zstrcmp(
_tmp3AF,_tag_arr("section",sizeof(unsigned char),8))== 0? 1: Cyc_Std_zstrcmp(
_tmp3AF,_tag_arr("__section__",sizeof(unsigned char),12))== 0){a=(void*)({struct
Cyc_Absyn_Section_att_struct*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1[0]=({
struct Cyc_Absyn_Section_att_struct _tmp3B2;_tmp3B2.tag=2;_tmp3B2.f1=_tmp3B0;
_tmp3B2;});_tmp3B1;});}else{Cyc_Parse_err(_tag_arr("unrecognized attribute",
sizeof(unsigned char),23),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));
_tmp3B3[0]=({struct Cyc_Attribute_tok_struct _tmp3B4;_tmp3B4.tag=Cyc_Attribute_tok;
_tmp3B4.f1=(void*)a;_tmp3B4;});_tmp3B3;});break;}case 51: _LL2A1: {struct
_tagged_arr _tmp3B5=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp3B6=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp3B8;struct
_tuple12 _tmp3B7=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_LL2A4: _tmp3B8=_tmp3B7.f2;goto _LL2A3;_LL2A3: {int _tmp3BA;
struct _tuple12 _tmp3B9=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_LL2A6: _tmp3BA=_tmp3B9.f2;goto _LL2A5;_LL2A5: {void*a=(
void*)1;if(Cyc_Std_zstrcmp(_tmp3B5,_tag_arr("format",sizeof(unsigned char),7))== 
0? 1: Cyc_Std_zstrcmp(_tmp3B5,_tag_arr("__format__",sizeof(unsigned char),11))== 0){
if(Cyc_Std_zstrcmp(_tmp3B6,_tag_arr("printf",sizeof(unsigned char),7))== 0){a=(
void*)({struct Cyc_Absyn_Format_att_struct*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));
_tmp3BB[0]=({struct Cyc_Absyn_Format_att_struct _tmp3BC;_tmp3BC.tag=3;_tmp3BC.f1=(
void*)((void*)0);_tmp3BC.f2=_tmp3B8;_tmp3BC.f3=_tmp3BA;_tmp3BC;});_tmp3BB;});}
else{if(Cyc_Std_zstrcmp(_tmp3B6,_tag_arr("scanf",sizeof(unsigned char),6))== 0){
a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));
_tmp3BD[0]=({struct Cyc_Absyn_Format_att_struct _tmp3BE;_tmp3BE.tag=3;_tmp3BE.f1=(
void*)((void*)1);_tmp3BE.f2=_tmp3B8;_tmp3BE.f3=_tmp3BA;_tmp3BE;});_tmp3BD;});}
else{Cyc_Parse_err(_tag_arr("unrecognized format type",sizeof(unsigned char),25),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(_tag_arr("unrecognized attribute",
sizeof(unsigned char),23),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Attribute_tok_struct*
_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF[0]=({struct Cyc_Attribute_tok_struct
_tmp3C0;_tmp3C0.tag=Cyc_Attribute_tok;_tmp3C0.f1=(void*)a;_tmp3C0;});_tmp3BF;});
break;}}}case 52: _LL2A2: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3C1=
_cycalloc(sizeof(*_tmp3C1));_tmp3C1[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3C2;_tmp3C2.tag=Cyc_TypeSpecifier_tok;_tmp3C2.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3C2;});_tmp3C1;});break;case 53: _LL2A7: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3C4;_tmp3C4.tag=Cyc_TypeSpecifier_tok;
_tmp3C4.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3C4;});
_tmp3C3;});break;case 54: _LL2A8: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3C6;_tmp3C6.tag=Cyc_TypeSpecifier_tok;_tmp3C6.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_uchar_t,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3C6;});_tmp3C5;});break;case 55: _LL2A9: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));
_tmp3C7[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3C8;_tmp3C8.tag=Cyc_TypeSpecifier_tok;
_tmp3C8.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*_tmp3C9=_cycalloc(
sizeof(*_tmp3C9));_tmp3C9[0]=({struct Cyc_Parse_Short_spec_struct _tmp3CA;_tmp3CA.tag=
2;_tmp3CA.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line);_tmp3CA;});_tmp3C9;}));_tmp3C8;});_tmp3C7;});break;
case 56: _LL2AA: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3CB=
_cycalloc(sizeof(*_tmp3CB));_tmp3CB[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3CC;_tmp3CC.tag=Cyc_TypeSpecifier_tok;_tmp3CC.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_t,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3CC;});_tmp3CB;});break;case 57: _LL2AB: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));
_tmp3CD[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3CE;_tmp3CE.tag=Cyc_TypeSpecifier_tok;
_tmp3CE.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*_tmp3CF=_cycalloc(
sizeof(*_tmp3CF));_tmp3CF[0]=({struct Cyc_Parse_Long_spec_struct _tmp3D0;_tmp3D0.tag=
3;_tmp3D0.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line);_tmp3D0;});_tmp3CF;}));_tmp3CE;});_tmp3CD;});break;
case 58: _LL2AC: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3D1=
_cycalloc(sizeof(*_tmp3D1));_tmp3D1[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3D2;_tmp3D2.tag=Cyc_TypeSpecifier_tok;_tmp3D2.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3D2;});_tmp3D1;});break;case 59: _LL2AD: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));
_tmp3D3[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3D4;_tmp3D4.tag=Cyc_TypeSpecifier_tok;
_tmp3D4.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3D4;});
_tmp3D3;});break;case 60: _LL2AE: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3D6;_tmp3D6.tag=Cyc_TypeSpecifier_tok;_tmp3D6.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*
_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp3D8;_tmp3D8.tag=0;_tmp3D8.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp3D8;});_tmp3D7;}));_tmp3D6;});_tmp3D5;});
break;case 61: _LL2AF: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3D9=
_cycalloc(sizeof(*_tmp3D9));_tmp3D9[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3DA;_tmp3DA.tag=Cyc_TypeSpecifier_tok;_tmp3DA.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp3DC;_tmp3DC.tag=1;_tmp3DC.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp3DC;});_tmp3DB;}));_tmp3DA;});_tmp3D9;});
break;case 62: _LL2B0: yyval=yyvs[yyvsp_offset];break;case 63: _LL2B1: yyval=yyvs[
yyvsp_offset];break;case 64: _LL2B2: yyval=yyvs[yyvsp_offset];break;case 65: _LL2B3:
yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3DD=_cycalloc(sizeof(*
_tmp3DD));_tmp3DD[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3DE;_tmp3DE.tag=Cyc_TypeSpecifier_tok;
_tmp3DE.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3DE;});
_tmp3DD;});break;case 66: _LL2B4: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3E0;_tmp3E0.tag=Cyc_TypeSpecifier_tok;_tmp3E0.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypedefType_struct*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));
_tmp3E1[0]=({struct Cyc_Absyn_TypedefType_struct _tmp3E2;_tmp3E2.tag=18;_tmp3E2.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3E2.f2=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp3E2.f3=0;_tmp3E2;});
_tmp3E1;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3E0;});
_tmp3DF;});break;case 67: _LL2B5: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3E3=_cycalloc(sizeof(*_tmp3E3));_tmp3E3[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3E4;_tmp3E4.tag=Cyc_TypeSpecifier_tok;_tmp3E4.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TupleType_struct*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));
_tmp3E5[0]=({struct Cyc_Absyn_TupleType_struct _tmp3E6;_tmp3E6.tag=9;_tmp3E6.f1=((
struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp3E6;});_tmp3E5;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3E4;});
_tmp3E3;});break;case 68: _LL2B6: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3E7=_cycalloc(sizeof(*_tmp3E7));_tmp3E7[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3E8;_tmp3E8.tag=Cyc_TypeSpecifier_tok;_tmp3E8.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));
_tmp3E9[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp3EA;_tmp3EA.tag=17;_tmp3EA.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp3EA;});_tmp3E9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3E8;});_tmp3E7;});break;case 69: _LL2B7: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));_tmp3EB[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3EC;_tmp3EC.tag=Cyc_TypeSpecifier_tok;
_tmp3EC.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp3EE;_tmp3EE.tag=13;_tmp3EE.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3EE;});_tmp3ED;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3EC;});
_tmp3EB;});break;case 70: _LL2B8: yyval=(void*)({struct Cyc_Kind_tok_struct*_tmp3EF=
_cycalloc(sizeof(*_tmp3EF));_tmp3EF[0]=({struct Cyc_Kind_tok_struct _tmp3F0;
_tmp3F0.tag=Cyc_Kind_tok;_tmp3F0.f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3F0;});_tmp3EF;});break;case 71: _LL2B9: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp3F1=_cycalloc_atomic(sizeof(*_tmp3F1));
_tmp3F1[0]=({struct Cyc_TypeQual_tok_struct _tmp3F2;_tmp3F2.tag=Cyc_TypeQual_tok;
_tmp3F2.f1=({struct Cyc_Absyn_Tqual _tmp3F3;_tmp3F3.q_const=1;_tmp3F3.q_volatile=0;
_tmp3F3.q_restrict=0;_tmp3F3;});_tmp3F2;});_tmp3F1;});break;case 72: _LL2BA: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp3F4=_cycalloc_atomic(sizeof(*_tmp3F4));
_tmp3F4[0]=({struct Cyc_TypeQual_tok_struct _tmp3F5;_tmp3F5.tag=Cyc_TypeQual_tok;
_tmp3F5.f1=({struct Cyc_Absyn_Tqual _tmp3F6;_tmp3F6.q_const=0;_tmp3F6.q_volatile=1;
_tmp3F6.q_restrict=0;_tmp3F6;});_tmp3F5;});_tmp3F4;});break;case 73: _LL2BB: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp3F7=_cycalloc_atomic(sizeof(*_tmp3F7));
_tmp3F7[0]=({struct Cyc_TypeQual_tok_struct _tmp3F8;_tmp3F8.tag=Cyc_TypeQual_tok;
_tmp3F8.f1=({struct Cyc_Absyn_Tqual _tmp3F9;_tmp3F9.q_const=0;_tmp3F9.q_volatile=0;
_tmp3F9.q_restrict=1;_tmp3F9;});_tmp3F8;});_tmp3F7;});break;case 74: _LL2BC: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3FA=_cycalloc(sizeof(*_tmp3FA));
_tmp3FA[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3FB;_tmp3FB.tag=Cyc_TypeSpecifier_tok;
_tmp3FB.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp3FC=_cycalloc(
sizeof(*_tmp3FC));_tmp3FC[0]=({struct Cyc_Parse_Decl_spec_struct _tmp3FD;_tmp3FD.tag=
5;_tmp3FD.f1=({struct Cyc_Absyn_Decl*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));_tmp3FE->r=(
void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));
_tmp3FF[0]=({struct Cyc_Absyn_Enum_d_struct _tmp400;_tmp400.tag=7;_tmp400.f1=({
struct Cyc_Absyn_Enumdecl*_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401->sc=(void*)((
void*)2);_tmp401->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp401->fields=({struct Cyc_Core_Opt*_tmp402=_cycalloc(
sizeof(*_tmp402));_tmp402->v=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp402;});_tmp401;});
_tmp400;});_tmp3FF;}));_tmp3FE->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp3FE;});_tmp3FD;});_tmp3FC;}));_tmp3FB;});_tmp3FA;});
break;case 75: _LL2BD: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp403=
_cycalloc(sizeof(*_tmp403));_tmp403[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp404;_tmp404.tag=Cyc_TypeSpecifier_tok;_tmp404.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp405=_cycalloc(sizeof(*_tmp405));
_tmp405[0]=({struct Cyc_Absyn_EnumType_struct _tmp406;_tmp406.tag=12;_tmp406.f1=
Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);_tmp406.f2=0;_tmp406;});_tmp405;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp404;});_tmp403;});break;case 76: _LL2BE: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp408;_tmp408.tag=Cyc_TypeSpecifier_tok;
_tmp408.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp409=_cycalloc(
sizeof(*_tmp409));_tmp409[0]=({struct Cyc_Parse_Type_spec_struct _tmp40A;_tmp40A.tag=
4;_tmp40A.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp40B=
_cycalloc(sizeof(*_tmp40B));_tmp40B[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp40C;_tmp40C.tag=16;_tmp40C.f1=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp40C;});_tmp40B;}));
_tmp40A.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line);_tmp40A;});
_tmp409;}));_tmp408;});_tmp407;});break;case 77: _LL2BF: yyval=(void*)({struct Cyc_Enumfield_tok_struct*
_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D[0]=({struct Cyc_Enumfield_tok_struct
_tmp40E;_tmp40E.tag=Cyc_Enumfield_tok;_tmp40E.f1=({struct Cyc_Absyn_Enumfield*
_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F->name=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp40F->tag=0;_tmp40F->loc=Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);_tmp40F;});_tmp40E;});
_tmp40D;});break;case 78: _LL2C0: yyval=(void*)({struct Cyc_Enumfield_tok_struct*
_tmp410=_cycalloc(sizeof(*_tmp410));_tmp410[0]=({struct Cyc_Enumfield_tok_struct
_tmp411;_tmp411.tag=Cyc_Enumfield_tok;_tmp411.f1=({struct Cyc_Absyn_Enumfield*
_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp412->tag=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp412->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp412;});_tmp411;});_tmp410;});break;case 79: _LL2C1:
yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*_tmp413=_cycalloc(sizeof(*
_tmp413));_tmp413[0]=({struct Cyc_EnumfieldList_tok_struct _tmp414;_tmp414.tag=Cyc_EnumfieldList_tok;
_tmp414.f1=({struct Cyc_List_List*_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415->hd=
Cyc_yyget_Enumfield_tok(yyvs[yyvsp_offset]);_tmp415->tl=0;_tmp415;});_tmp414;});
_tmp413;});break;case 80: _LL2C2: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp417;_tmp417.tag=Cyc_EnumfieldList_tok;_tmp417.f1=({struct Cyc_List_List*
_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp418->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[yyvsp_offset]);_tmp418;});_tmp417;});_tmp416;});break;case 81: _LL2C3: {void*
t;switch(Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)])){case Cyc_Parse_Struct_su: _LL2C5: t=(void*)({struct Cyc_Absyn_AnonStructType_struct*
_tmp419=_cycalloc(sizeof(*_tmp419));_tmp419[0]=({struct Cyc_Absyn_AnonStructType_struct
_tmp41A;_tmp41A.tag=14;_tmp41A.f1=Cyc_yyget_StructFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp41A;});_tmp419;});
break;case Cyc_Parse_Union_su: _LL2C6: t=(void*)({struct Cyc_Absyn_AnonUnionType_struct*
_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B[0]=({struct Cyc_Absyn_AnonUnionType_struct
_tmp41C;_tmp41C.tag=15;_tmp41C.f1=Cyc_yyget_StructFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp41C;});_tmp41B;});
break;}yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp41D=_cycalloc(
sizeof(*_tmp41D));_tmp41D[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp41E;_tmp41E.tag=
Cyc_TypeSpecifier_tok;_tmp41E.f1=(void*)Cyc_Parse_type_spec(t,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp41E;});_tmp41D;});break;}case 82: _LL2C4: {struct
Cyc_List_List*_tmp41F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));struct Cyc_Absyn_Decl*(*
decl_maker)(void*s,struct Cyc_Core_Opt*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);switch(Cyc_yyget_StructOrUnion_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)])){case Cyc_Parse_Struct_su:
_LL2C9: decl_maker=Cyc_Absyn_struct_decl;break;case Cyc_Parse_Union_su: _LL2CA:
decl_maker=Cyc_Absyn_union_decl;break;}yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp421;_tmp421.tag=Cyc_TypeSpecifier_tok;_tmp421.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp423;_tmp423.tag=5;_tmp423.f1=decl_maker((void*)2,({struct Cyc_Core_Opt*
_tmp424=_cycalloc(sizeof(*_tmp424));_tmp424->v=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp424;}),_tmp41F,({
struct Cyc_Core_Opt*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->v=Cyc_yyget_StructFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp425;}),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp423;});_tmp422;}));_tmp421;});_tmp420;});break;}
case 83: _LL2C8: {void*t;switch(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])){case Cyc_Parse_Struct_su:
_LL2CD: t=(void*)({struct Cyc_Absyn_StructType_struct*_tmp426=_cycalloc(sizeof(*
_tmp426));_tmp426[0]=({struct Cyc_Absyn_StructType_struct _tmp427;_tmp427.tag=10;
_tmp427.f1=(struct _tuple1*)Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp427.f2=Cyc_yyget_TypeList_tok(
yyvs[yyvsp_offset]);_tmp427.f3=0;_tmp427;});_tmp426;});break;case Cyc_Parse_Union_su:
_LL2CE: t=(void*)({struct Cyc_Absyn_UnionType_struct*_tmp428=_cycalloc(sizeof(*
_tmp428));_tmp428[0]=({struct Cyc_Absyn_UnionType_struct _tmp429;_tmp429.tag=11;
_tmp429.f1=(struct _tuple1*)Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp429.f2=Cyc_yyget_TypeList_tok(
yyvs[yyvsp_offset]);_tmp429.f3=0;_tmp429;});_tmp428;});break;}yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp42A=_cycalloc(sizeof(*_tmp42A));_tmp42A[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp42B;_tmp42B.tag=Cyc_TypeSpecifier_tok;
_tmp42B.f1=(void*)Cyc_Parse_type_spec(t,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp42B;});_tmp42A;});break;}case 84: _LL2CC: yyval=(
void*)({struct Cyc_TypeList_tok_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));
_tmp42C[0]=({struct Cyc_TypeList_tok_struct _tmp42D;_tmp42D.tag=Cyc_TypeList_tok;
_tmp42D.f1=0;_tmp42D;});_tmp42C;});break;case 85: _LL2D0: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp42E=_cycalloc(sizeof(*_tmp42E));_tmp42E[0]=({struct Cyc_TypeList_tok_struct
_tmp42F;_tmp42F.tag=Cyc_TypeList_tok;_tmp42F.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp42F;});_tmp42E;});
break;case 86: _LL2D1: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp430=
_cycalloc_atomic(sizeof(*_tmp430));_tmp430[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp431;_tmp431.tag=Cyc_StructOrUnion_tok;_tmp431.f1=Cyc_Parse_Struct_su;_tmp431;});
_tmp430;});break;case 87: _LL2D2: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*
_tmp432=_cycalloc_atomic(sizeof(*_tmp432));_tmp432[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp433;_tmp433.tag=Cyc_StructOrUnion_tok;_tmp433.f1=Cyc_Parse_Union_su;_tmp433;});
_tmp432;});break;case 88: _LL2D3: yyval=(void*)({struct Cyc_StructFieldDeclList_tok_struct*
_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434[0]=({struct Cyc_StructFieldDeclList_tok_struct
_tmp435;_tmp435.tag=Cyc_StructFieldDeclList_tok;_tmp435.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_StructFieldDeclListList_tok(yyvs[yyvsp_offset])));
_tmp435;});_tmp434;});break;case 89: _LL2D4: yyval=(void*)({struct Cyc_StructFieldDeclListList_tok_struct*
_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436[0]=({struct Cyc_StructFieldDeclListList_tok_struct
_tmp437;_tmp437.tag=Cyc_StructFieldDeclListList_tok;_tmp437.f1=({struct Cyc_List_List*
_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->hd=Cyc_yyget_StructFieldDeclList_tok(
yyvs[yyvsp_offset]);_tmp438->tl=0;_tmp438;});_tmp437;});_tmp436;});break;case 90:
_LL2D5: yyval=(void*)({struct Cyc_StructFieldDeclListList_tok_struct*_tmp439=
_cycalloc(sizeof(*_tmp439));_tmp439[0]=({struct Cyc_StructFieldDeclListList_tok_struct
_tmp43A;_tmp43A.tag=Cyc_StructFieldDeclListList_tok;_tmp43A.f1=({struct Cyc_List_List*
_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B->hd=Cyc_yyget_StructFieldDeclList_tok(
yyvs[yyvsp_offset]);_tmp43B->tl=Cyc_yyget_StructFieldDeclListList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp43B;});_tmp43A;});
_tmp439;});break;case 91: _LL2D6: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C[0]=({struct Cyc_InitDeclList_tok_struct
_tmp43D;_tmp43D.tag=Cyc_InitDeclList_tok;_tmp43D.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
yyvsp_offset]));_tmp43D;});_tmp43C;});break;case 92: _LL2D7: yyval=(void*)({struct
Cyc_InitDeclList_tok_struct*_tmp43E=_cycalloc(sizeof(*_tmp43E));_tmp43E[0]=({
struct Cyc_InitDeclList_tok_struct _tmp43F;_tmp43F.tag=Cyc_InitDeclList_tok;
_tmp43F.f1=({struct Cyc_List_List*_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440->hd=
Cyc_yyget_InitDecl_tok(yyvs[yyvsp_offset]);_tmp440->tl=0;_tmp440;});_tmp43F;});
_tmp43E;});break;case 93: _LL2D8: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441[0]=({struct Cyc_InitDeclList_tok_struct
_tmp442;_tmp442.tag=Cyc_InitDeclList_tok;_tmp442.f1=({struct Cyc_List_List*
_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443->hd=Cyc_yyget_InitDecl_tok(yyvs[
yyvsp_offset]);_tmp443->tl=Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp443;});_tmp442;});
_tmp441;});break;case 94: _LL2D9: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444[0]=({struct Cyc_InitDecl_tok_struct
_tmp445;_tmp445.tag=Cyc_InitDecl_tok;_tmp445.f1=({struct _tuple11*_tmp446=
_cycalloc(sizeof(*_tmp446));_tmp446->f1=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp446->f2=0;_tmp446;});_tmp445;});_tmp444;});break;case 95:
_LL2DA: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp447=_cycalloc(sizeof(*
_tmp447));_tmp447[0]=({struct Cyc_InitDecl_tok_struct _tmp448;_tmp448.tag=Cyc_InitDecl_tok;
_tmp448.f1=({struct _tuple11*_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->f1=Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp449->f2=(struct
Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp449;});_tmp448;});
_tmp447;});break;case 96: _LL2DB: {struct _tuple14 _tmp44B;struct Cyc_List_List*
_tmp44C;struct Cyc_List_List*_tmp44D;struct Cyc_Absyn_Tqual _tmp44E;struct _tuple14*
_tmp44A=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp44B=*_tmp44A;_LL2E0: _tmp44E=_tmp44B.f1;goto _LL2DF;_LL2DF:
_tmp44D=_tmp44B.f2;goto _LL2DE;_LL2DE: _tmp44C=_tmp44B.f3;goto _LL2DD;_LL2DD: {
struct Cyc_List_List*_tmp450;struct Cyc_List_List*_tmp451;struct _tuple0 _tmp44F=((
struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_LL2E3: _tmp451=
_tmp44F.f1;goto _LL2E2;_LL2E2: _tmp450=_tmp44F.f2;goto _LL2E1;_LL2E1: {void*_tmp452=
Cyc_Parse_speclist2typ(_tmp44D,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp453=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(Cyc_Parse_apply_tmss(_tmp44E,_tmp452,_tmp451,_tmp44C),_tmp450);
yyval=(void*)({struct Cyc_StructFieldDeclList_tok_struct*_tmp454=_cycalloc(
sizeof(*_tmp454));_tmp454[0]=({struct Cyc_StructFieldDeclList_tok_struct _tmp455;
_tmp455.tag=Cyc_StructFieldDeclList_tok;_tmp455.f1=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(*f)(struct Cyc_Position_Segment*,struct _tuple7*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmp453);_tmp455;});_tmp454;});break;}}}case 97:
_LL2DC: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp456=_cycalloc(
sizeof(*_tmp456));_tmp456[0]=({struct Cyc_QualSpecList_tok_struct _tmp457;_tmp457.tag=
Cyc_QualSpecList_tok;_tmp457.f1=({struct _tuple14*_tmp458=_cycalloc(sizeof(*
_tmp458));_tmp458->f1=Cyc_Absyn_empty_tqual();_tmp458->f2=({struct Cyc_List_List*
_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp459->tl=0;
_tmp459;});_tmp458->f3=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp458;});
_tmp457;});_tmp456;});break;case 98: _LL2E4: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A[0]=({struct Cyc_QualSpecList_tok_struct
_tmp45B;_tmp45B.tag=Cyc_QualSpecList_tok;_tmp45B.f1=({struct _tuple14*_tmp45C=
_cycalloc(sizeof(*_tmp45C));_tmp45C->f1=(*Cyc_yyget_QualSpecList_tok(yyvs[
yyvsp_offset])).f1;_tmp45C->f2=({struct Cyc_List_List*_tmp45D=_cycalloc(sizeof(*
_tmp45D));_tmp45D->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp45D->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f2;_tmp45D;});_tmp45C->f3=Cyc_List_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f3);_tmp45C;});_tmp45B;});_tmp45A;});break;case 99: _LL2E5:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp45E=_cycalloc(sizeof(*
_tmp45E));_tmp45E[0]=({struct Cyc_QualSpecList_tok_struct _tmp45F;_tmp45F.tag=Cyc_QualSpecList_tok;
_tmp45F.f1=({struct _tuple14*_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp460->f2=0;
_tmp460->f3=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp460;});_tmp45F;});
_tmp45E;});break;case 100: _LL2E6: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461[0]=({struct Cyc_QualSpecList_tok_struct
_tmp462;_tmp462.tag=Cyc_QualSpecList_tok;_tmp462.f1=({struct _tuple14*_tmp463=
_cycalloc(sizeof(*_tmp463));_tmp463->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f1);_tmp463->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
yyvsp_offset])).f2;_tmp463->f3=Cyc_List_append(Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f3);_tmp463;});_tmp462;});_tmp461;});break;case 101: _LL2E7:
yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp464=_cycalloc(sizeof(*
_tmp464));_tmp464[0]=({struct Cyc_InitDeclList_tok_struct _tmp465;_tmp465.tag=Cyc_InitDeclList_tok;
_tmp465.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok(yyvs[yyvsp_offset]));_tmp465;});_tmp464;});break;case
102: _LL2E8: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp466=_cycalloc(
sizeof(*_tmp466));_tmp466[0]=({struct Cyc_InitDeclList_tok_struct _tmp467;_tmp467.tag=
Cyc_InitDeclList_tok;_tmp467.f1=({struct Cyc_List_List*_tmp468=_cycalloc(sizeof(*
_tmp468));_tmp468->hd=Cyc_yyget_InitDecl_tok(yyvs[yyvsp_offset]);_tmp468->tl=0;
_tmp468;});_tmp467;});_tmp466;});break;case 103: _LL2E9: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469[0]=({struct Cyc_InitDeclList_tok_struct
_tmp46A;_tmp46A.tag=Cyc_InitDeclList_tok;_tmp46A.f1=({struct Cyc_List_List*
_tmp46B=_cycalloc(sizeof(*_tmp46B));_tmp46B->hd=Cyc_yyget_InitDecl_tok(yyvs[
yyvsp_offset]);_tmp46B->tl=Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp46B;});_tmp46A;});
_tmp469;});break;case 104: _LL2EA: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp46C=_cycalloc(sizeof(*_tmp46C));_tmp46C[0]=({struct Cyc_InitDecl_tok_struct
_tmp46D;_tmp46D.tag=Cyc_InitDecl_tok;_tmp46D.f1=({struct _tuple11*_tmp46E=
_cycalloc(sizeof(*_tmp46E));_tmp46E->f1=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp46E->f2=0;_tmp46E;});_tmp46D;});_tmp46C;});break;case 105:
_LL2EB: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp46F=_cycalloc(sizeof(*
_tmp46F));_tmp46F[0]=({struct Cyc_InitDecl_tok_struct _tmp470;_tmp470.tag=Cyc_InitDecl_tok;
_tmp470.f1=({struct _tuple11*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->f1=({
struct Cyc_Parse_Declarator*_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->id=({
struct _tuple1*_tmp473=_cycalloc(sizeof(*_tmp473));_tmp473->f1=Cyc_Absyn_rel_ns_null;
_tmp473->f2=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(
unsigned char),1);_tmp473;});_tmp472->tms=0;_tmp472;});_tmp471->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp471;});_tmp470;});_tmp46F;});break;
case 106: _LL2EC: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp475=_cycalloc(
sizeof(*_tmp475));_tmp475[0]=({struct Cyc_InitDecl_tok_struct _tmp476;_tmp476.tag=
Cyc_InitDecl_tok;_tmp476.f1=({struct _tuple11*_tmp477=_cycalloc(sizeof(*_tmp477));
_tmp477->f1=Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp477->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]);_tmp477;});_tmp476;});_tmp475;});break;case 107: _LL2ED: {struct Cyc_List_List*
_tmp478=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp47A;_tmp47A.tag=Cyc_TypeSpecifier_tok;
_tmp47A.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp47B=_cycalloc(
sizeof(*_tmp47B));_tmp47B[0]=({struct Cyc_Parse_Decl_spec_struct _tmp47C;_tmp47C.tag=
5;_tmp47C.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp478,({struct Cyc_Core_Opt*
_tmp47D=_cycalloc(sizeof(*_tmp47D));_tmp47D->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp47D;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp47C;});_tmp47B;}));_tmp47A;});_tmp479;});break;}
case 108: _LL2EE: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp47E=
_cycalloc(sizeof(*_tmp47E));_tmp47E[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp47F;_tmp47F.tag=Cyc_TypeSpecifier_tok;_tmp47F.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp480=_cycalloc(sizeof(*_tmp480));
_tmp480[0]=({struct Cyc_Absyn_TunionType_struct _tmp481;_tmp481.tag=2;_tmp481.f1=({
struct Cyc_Absyn_TunionInfo _tmp482;_tmp482.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp484;_tmp484.tag=0;_tmp484.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp485;
_tmp485.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp485.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp485;});_tmp484;});
_tmp483;}));_tmp482.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp482.rgn=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp482;});_tmp481;});_tmp480;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp47F;});_tmp47E;});break;case 109: _LL2EF: {void*
_tmp486=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp48F=_cycalloc(sizeof(*
_tmp48F));_tmp48F->v=(void*)((void*)3);_tmp48F;}),0);yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp487=_cycalloc(sizeof(*_tmp487));_tmp487[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp488;_tmp488.tag=Cyc_TypeSpecifier_tok;_tmp488.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp489=_cycalloc(sizeof(*_tmp489));
_tmp489[0]=({struct Cyc_Absyn_TunionType_struct _tmp48A;_tmp48A.tag=2;_tmp48A.f1=({
struct Cyc_Absyn_TunionInfo _tmp48B;_tmp48B.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp48D;_tmp48D.tag=0;_tmp48D.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp48E;
_tmp48E.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp48E.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp48E;});_tmp48D;});
_tmp48C;}));_tmp48B.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp48B.rgn=(
void*)_tmp486;_tmp48B;});_tmp48A;});_tmp489;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp488;});_tmp487;});break;}case 110: _LL2F0: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp490=_cycalloc(sizeof(*_tmp490));
_tmp490[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp491;_tmp491.tag=Cyc_TypeSpecifier_tok;
_tmp491.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp492=_cycalloc(sizeof(*_tmp492));_tmp492[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp493;_tmp493.tag=3;_tmp493.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp494;
_tmp494.field_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*
_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp496;_tmp496.tag=0;_tmp496.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp497;
_tmp497.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp497.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp497.is_xtunion=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp497;});_tmp496;});
_tmp495;}));_tmp494.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp494;});
_tmp493;});_tmp492;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp491;});_tmp490;});break;case 111: _LL2F1: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp498=_cycalloc_atomic(sizeof(*_tmp498));
_tmp498[0]=({struct Cyc_Bool_tok_struct _tmp499;_tmp499.tag=Cyc_Bool_tok;_tmp499.f1=
0;_tmp499;});_tmp498;});break;case 112: _LL2F2: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp49A=_cycalloc_atomic(sizeof(*_tmp49A));_tmp49A[0]=({struct Cyc_Bool_tok_struct
_tmp49B;_tmp49B.tag=Cyc_Bool_tok;_tmp49B.f1=1;_tmp49B;});_tmp49A;});break;case
113: _LL2F3: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp49C=_cycalloc(
sizeof(*_tmp49C));_tmp49C[0]=({struct Cyc_TunionFieldList_tok_struct _tmp49D;
_tmp49D.tag=Cyc_TunionFieldList_tok;_tmp49D.f1=({struct Cyc_List_List*_tmp49E=
_cycalloc(sizeof(*_tmp49E));_tmp49E->hd=Cyc_yyget_TunionField_tok(yyvs[
yyvsp_offset]);_tmp49E->tl=0;_tmp49E;});_tmp49D;});_tmp49C;});break;case 114:
_LL2F4: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp49F=_cycalloc(
sizeof(*_tmp49F));_tmp49F[0]=({struct Cyc_TunionFieldList_tok_struct _tmp4A0;
_tmp4A0.tag=Cyc_TunionFieldList_tok;_tmp4A0.f1=({struct Cyc_List_List*_tmp4A1=
_cycalloc(sizeof(*_tmp4A1));_tmp4A1->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A1->tl=0;_tmp4A1;});
_tmp4A0;});_tmp49F;});break;case 115: _LL2F5: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp4A2=_cycalloc(sizeof(*_tmp4A2));_tmp4A2[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp4A3;_tmp4A3.tag=Cyc_TunionFieldList_tok;_tmp4A3.f1=({struct Cyc_List_List*
_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4A4->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[yyvsp_offset]);_tmp4A4;});_tmp4A3;});_tmp4A2;});break;case 116: _LL2F6: yyval=(
void*)({struct Cyc_TunionFieldList_tok_struct*_tmp4A5=_cycalloc(sizeof(*_tmp4A5));
_tmp4A5[0]=({struct Cyc_TunionFieldList_tok_struct _tmp4A6;_tmp4A6.tag=Cyc_TunionFieldList_tok;
_tmp4A6.f1=({struct Cyc_List_List*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7->hd=
Cyc_yyget_TunionField_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]);_tmp4A7->tl=Cyc_yyget_TunionFieldList_tok(yyvs[yyvsp_offset]);_tmp4A7;});
_tmp4A6;});_tmp4A5;});break;case 117: _LL2F7: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4A8=_cycalloc(sizeof(*_tmp4A8));_tmp4A8[0]=({struct Cyc_Scope_tok_struct
_tmp4A9;_tmp4A9.tag=Cyc_Scope_tok;_tmp4A9.f1=(void*)((void*)2);_tmp4A9;});
_tmp4A8;});break;case 118: _LL2F8: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA[0]=({struct Cyc_Scope_tok_struct
_tmp4AB;_tmp4AB.tag=Cyc_Scope_tok;_tmp4AB.f1=(void*)((void*)3);_tmp4AB;});
_tmp4AA;});break;case 119: _LL2F9: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC[0]=({struct Cyc_Scope_tok_struct
_tmp4AD;_tmp4AD.tag=Cyc_Scope_tok;_tmp4AD.f1=(void*)((void*)0);_tmp4AD;});
_tmp4AC;});break;case 120: _LL2FA: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp4AE=_cycalloc(sizeof(*_tmp4AE));_tmp4AE[0]=({struct Cyc_TunionField_tok_struct
_tmp4AF;_tmp4AF.tag=Cyc_TunionField_tok;_tmp4AF.f1=({struct Cyc_Absyn_Tunionfield*
_tmp4B0=_cycalloc(sizeof(*_tmp4B0));_tmp4B0->name=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp4B0->tvs=0;_tmp4B0->typs=0;_tmp4B0->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4B0->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B0;});_tmp4AF;});
_tmp4AE;});break;case 121: _LL2FB: {struct Cyc_List_List*_tmp4B1=((struct Cyc_List_List*(*)(
struct _tuple8*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));struct Cyc_List_List*
_tmp4B2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp4B3=_cycalloc(sizeof(*_tmp4B3));_tmp4B3[0]=({
struct Cyc_TunionField_tok_struct _tmp4B4;_tmp4B4.tag=Cyc_TunionField_tok;_tmp4B4.f1=({
struct Cyc_Absyn_Tunionfield*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp4B5->tvs=_tmp4B2;_tmp4B5->typs=_tmp4B1;_tmp4B5->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4B5->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp4B5;});_tmp4B4;});
_tmp4B3;});break;}case 122: _LL2FC: yyval=yyvs[yyvsp_offset];break;case 123: _LL2FD:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));
_tmp4B6[0]=({struct Cyc_Declarator_tok_struct _tmp4B7;_tmp4B7.tag=Cyc_Declarator_tok;
_tmp4B7.f1=({struct Cyc_Parse_Declarator*_tmp4B8=_cycalloc(sizeof(*_tmp4B8));
_tmp4B8->id=(Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]))->id;_tmp4B8->tms=Cyc_List_imp_append(
Cyc_yyget_TypeModifierList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]))->tms);_tmp4B8;});
_tmp4B7;});_tmp4B6;});break;case 124: _LL2FE: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4B9=_cycalloc(sizeof(*_tmp4B9));_tmp4B9[0]=({struct Cyc_Declarator_tok_struct
_tmp4BA;_tmp4BA.tag=Cyc_Declarator_tok;_tmp4BA.f1=({struct Cyc_Parse_Declarator*
_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB->id=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp4BB->tms=0;_tmp4BB;});_tmp4BA;});_tmp4B9;});break;case 125:
_LL2FF: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 126: _LL300: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4BC=_cycalloc(
sizeof(*_tmp4BC));_tmp4BC[0]=({struct Cyc_Declarator_tok_struct _tmp4BD;_tmp4BD.tag=
Cyc_Declarator_tok;_tmp4BD.f1=({struct Cyc_Parse_Declarator*_tmp4BE=_cycalloc(
sizeof(*_tmp4BE));_tmp4BE->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->id;_tmp4BE->tms=({
struct Cyc_List_List*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF->hd=(void*)((
void*)0);_tmp4BF->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->tms;_tmp4BF;});_tmp4BE;});
_tmp4BD;});_tmp4BC;});break;case 127: _LL301: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0[0]=({struct Cyc_Declarator_tok_struct
_tmp4C1;_tmp4C1.tag=Cyc_Declarator_tok;_tmp4C1.f1=({struct Cyc_Parse_Declarator*
_tmp4C2=_cycalloc(sizeof(*_tmp4C2));_tmp4C2->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4C2->tms=({
struct Cyc_List_List*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));_tmp4C3->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp4C4=_cycalloc(sizeof(*_tmp4C4));
_tmp4C4[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp4C5;_tmp4C5.tag=0;_tmp4C5.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4C5;});_tmp4C4;}));_tmp4C3->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp4C3;});_tmp4C2;});
_tmp4C1;});_tmp4C0;});break;case 128: _LL302: {struct _tuple13 _tmp4C7;struct Cyc_List_List*
_tmp4C8;struct Cyc_Core_Opt*_tmp4C9;struct Cyc_Absyn_VarargInfo*_tmp4CA;int _tmp4CB;
struct Cyc_List_List*_tmp4CC;struct _tuple13*_tmp4C6=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4C7=*_tmp4C6;_LL309:
_tmp4CC=_tmp4C7.f1;goto _LL308;_LL308: _tmp4CB=_tmp4C7.f2;goto _LL307;_LL307:
_tmp4CA=_tmp4C7.f3;goto _LL306;_LL306: _tmp4C9=_tmp4C7.f4;goto _LL305;_LL305:
_tmp4C8=_tmp4C7.f5;goto _LL304;_LL304: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4CD=_cycalloc(sizeof(*_tmp4CD));_tmp4CD[0]=({struct Cyc_Declarator_tok_struct
_tmp4CE;_tmp4CE.tag=Cyc_Declarator_tok;_tmp4CE.f1=({struct Cyc_Parse_Declarator*
_tmp4CF=_cycalloc(sizeof(*_tmp4CF));_tmp4CF->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4CF->tms=({
struct Cyc_List_List*_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4D1=_cycalloc(sizeof(*_tmp4D1));
_tmp4D1[0]=({struct Cyc_Absyn_Function_mod_struct _tmp4D2;_tmp4D2.tag=2;_tmp4D2.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp4D3=_cycalloc(sizeof(*
_tmp4D3));_tmp4D3[0]=({struct Cyc_Absyn_WithTypes_struct _tmp4D4;_tmp4D4.tag=1;
_tmp4D4.f1=_tmp4CC;_tmp4D4.f2=_tmp4CB;_tmp4D4.f3=_tmp4CA;_tmp4D4.f4=_tmp4C9;
_tmp4D4.f5=_tmp4C8;_tmp4D4;});_tmp4D3;}));_tmp4D2;});_tmp4D1;}));_tmp4D0->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp4D0;});_tmp4CF;});_tmp4CE;});_tmp4CD;});break;}case 129: _LL303:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4D5=_cycalloc(sizeof(*_tmp4D5));
_tmp4D5[0]=({struct Cyc_Declarator_tok_struct _tmp4D6;_tmp4D6.tag=Cyc_Declarator_tok;
_tmp4D6.f1=({struct Cyc_Parse_Declarator*_tmp4D7=_cycalloc(sizeof(*_tmp4D7));
_tmp4D7->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp4D7->tms=({struct Cyc_List_List*_tmp4D8=_cycalloc(
sizeof(*_tmp4D8));_tmp4D8->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp4DA;_tmp4DA.tag=2;_tmp4DA.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp4DC;_tmp4DC.tag=1;_tmp4DC.f1=0;_tmp4DC.f2=0;_tmp4DC.f3=0;_tmp4DC.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4DC.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4DC;});_tmp4DB;}));
_tmp4DA;});_tmp4D9;}));_tmp4D8->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp4D8;});_tmp4D7;});
_tmp4D6;});_tmp4D5;});break;case 130: _LL30A: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD[0]=({struct Cyc_Declarator_tok_struct
_tmp4DE;_tmp4DE.tag=Cyc_Declarator_tok;_tmp4DE.f1=({struct Cyc_Parse_Declarator*
_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4DF->tms=({
struct Cyc_List_List*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));
_tmp4E1[0]=({struct Cyc_Absyn_Function_mod_struct _tmp4E2;_tmp4E2.tag=2;_tmp4E2.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp4E3=_cycalloc(sizeof(*_tmp4E3));
_tmp4E3[0]=({struct Cyc_Absyn_NoTypes_struct _tmp4E4;_tmp4E4.tag=0;_tmp4E4.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E4.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4E4;});_tmp4E3;}));_tmp4E2;});_tmp4E1;}));_tmp4E0->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp4E0;});_tmp4DF;});_tmp4DE;});_tmp4DD;});break;case 131: _LL30B: {
struct Cyc_List_List*_tmp4E5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));_tmp4E6[0]=({
struct Cyc_Declarator_tok_struct _tmp4E7;_tmp4E7.tag=Cyc_Declarator_tok;_tmp4E7.f1=({
struct Cyc_Parse_Declarator*_tmp4E8=_cycalloc(sizeof(*_tmp4E8));_tmp4E8->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4E8->tms=({
struct Cyc_List_List*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp4EA=_cycalloc(sizeof(*_tmp4EA));
_tmp4EA[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp4EB;_tmp4EB.tag=3;_tmp4EB.f1=
_tmp4E5;_tmp4EB.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4EB.f3=0;_tmp4EB;});_tmp4EA;}));_tmp4E9->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp4E9;});
_tmp4E8;});_tmp4E7;});_tmp4E6;});break;}case 132: _LL30C: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4EC=_cycalloc(sizeof(*_tmp4EC));_tmp4EC[0]=({struct Cyc_Declarator_tok_struct
_tmp4ED;_tmp4ED.tag=Cyc_Declarator_tok;_tmp4ED.f1=({struct Cyc_Parse_Declarator*
_tmp4EE=_cycalloc(sizeof(*_tmp4EE));_tmp4EE->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp4EE->tms=({
struct Cyc_List_List*_tmp4EF=_cycalloc(sizeof(*_tmp4EF));_tmp4EF->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp4F0=_cycalloc(sizeof(*_tmp4F0));
_tmp4F0[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp4F1;_tmp4F1.tag=4;_tmp4F1.f1=
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);
_tmp4F1.f2=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp4F1;});_tmp4F0;}));
_tmp4EF->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp4EF;});_tmp4EE;});_tmp4ED;});_tmp4EC;});break;case
133: _LL30D: Cyc_Parse_err(_tag_arr("identifier has not been declared as a typedef",
sizeof(unsigned char),46),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(void*)({
struct Cyc_Declarator_tok_struct*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2[0]=({
struct Cyc_Declarator_tok_struct _tmp4F3;_tmp4F3.tag=Cyc_Declarator_tok;_tmp4F3.f1=({
struct Cyc_Parse_Declarator*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4->id=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp4F4->tms=0;_tmp4F4;});_tmp4F3;});_tmp4F2;});break;case
134: _LL30E: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4F5=_cycalloc(
sizeof(*_tmp4F5));_tmp4F5[0]=({struct Cyc_Declarator_tok_struct _tmp4F6;_tmp4F6.tag=
Cyc_Declarator_tok;_tmp4F6.f1=({struct Cyc_Parse_Declarator*_tmp4F7=_cycalloc(
sizeof(*_tmp4F7));_tmp4F7->id=Cyc_Absyn_exn_name;_tmp4F7->tms=0;_tmp4F7;});
_tmp4F6;});_tmp4F5;});break;case 135: _LL30F: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp4F9;_tmp4F9.tag=Cyc_TypeModifierList_tok;_tmp4F9.f1=({struct Cyc_List_List*
_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp4FC;_tmp4FC.tag=1;_tmp4FC.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4FC.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4FC.f3=Cyc_Absyn_empty_tqual();
_tmp4FC;});_tmp4FB;}));_tmp4FA->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]),0);_tmp4FA;});_tmp4F9;});_tmp4F8;});break;case 136: _LL310:
yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*_tmp4FD=_cycalloc(sizeof(*
_tmp4FD));_tmp4FD[0]=({struct Cyc_TypeModifierList_tok_struct _tmp4FE;_tmp4FE.tag=
Cyc_TypeModifierList_tok;_tmp4FE.f1=({struct Cyc_List_List*_tmp4FF=_cycalloc(
sizeof(*_tmp4FF));_tmp4FF->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp501;_tmp501.tag=1;_tmp501.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp501.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp501.f3=Cyc_yyget_TypeQual_tok(
yyvs[yyvsp_offset]);_tmp501;});_tmp500;}));_tmp4FF->tl=Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0);_tmp4FF;});_tmp4FE;});
_tmp4FD;});break;case 137: _LL311: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp503;_tmp503.tag=Cyc_TypeModifierList_tok;_tmp503.f1=({struct Cyc_List_List*
_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp506;_tmp506.tag=1;_tmp506.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp506.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp506.f3=Cyc_Absyn_empty_tqual();
_tmp506;});_tmp505;}));_tmp504->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp504;});_tmp503;});_tmp502;});break;case 138: _LL312: yyval=(
void*)({struct Cyc_TypeModifierList_tok_struct*_tmp507=_cycalloc(sizeof(*_tmp507));
_tmp507[0]=({struct Cyc_TypeModifierList_tok_struct _tmp508;_tmp508.tag=Cyc_TypeModifierList_tok;
_tmp508.f1=({struct Cyc_List_List*_tmp509=_cycalloc(sizeof(*_tmp509));_tmp509->hd=(
void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp50A=_cycalloc(sizeof(*
_tmp50A));_tmp50A[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp50B;_tmp50B.tag=1;
_tmp50B.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp50B.f2=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp50B.f3=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp50B;});_tmp50A;}));
_tmp509->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp509;});_tmp508;});_tmp507;});break;case 139: _LL313: yyval=(
void*)({struct Cyc_Pointer_Sort_tok_struct*_tmp50C=_cycalloc(sizeof(*_tmp50C));
_tmp50C[0]=({struct Cyc_Pointer_Sort_tok_struct _tmp50D;_tmp50D.tag=Cyc_Pointer_Sort_tok;
_tmp50D.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*_tmp50E=_cycalloc(
sizeof(*_tmp50E));_tmp50E[0]=({struct Cyc_Absyn_Nullable_ps_struct _tmp50F;_tmp50F.tag=
1;_tmp50F.f1=Cyc_Absyn_exp_unsigned_one;_tmp50F;});_tmp50E;}));_tmp50D;});
_tmp50C;});break;case 140: _LL314: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp511;_tmp511.tag=Cyc_Pointer_Sort_tok;_tmp511.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp513;_tmp513.tag=0;_tmp513.f1=Cyc_Absyn_exp_unsigned_one;_tmp513;});_tmp512;}));
_tmp511;});_tmp510;});break;case 141: _LL315: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp515;_tmp515.tag=Cyc_Pointer_Sort_tok;_tmp515.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*
_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp517;_tmp517.tag=1;_tmp517.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp517;});_tmp516;}));
_tmp515;});_tmp514;});break;case 142: _LL316: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp518=_cycalloc(sizeof(*_tmp518));_tmp518[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp519;_tmp519.tag=Cyc_Pointer_Sort_tok;_tmp519.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp51B;_tmp51B.tag=0;_tmp51B.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp51B;});_tmp51A;}));
_tmp519;});_tmp518;});break;case 143: _LL317: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp51C=_cycalloc(sizeof(*_tmp51C));_tmp51C[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp51D;_tmp51D.tag=Cyc_Pointer_Sort_tok;_tmp51D.f1=(void*)((void*)0);_tmp51D;});
_tmp51C;});break;case 144: _LL318: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp51E=
_cycalloc(sizeof(*_tmp51E));_tmp51E[0]=({struct Cyc_Type_tok_struct _tmp51F;
_tmp51F.tag=Cyc_Type_tok;_tmp51F.f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp520=_cycalloc(sizeof(*_tmp520));_tmp520->v=(void*)((void*)3);_tmp520;}),0);
_tmp51F;});_tmp51E;});break;case 145: _LL319: yyval=yyvs[yyvsp_offset];break;case
146: _LL31A: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(
void*)3);yyval=yyvs[yyvsp_offset];break;case 147: _LL31B: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521[0]=({struct Cyc_Type_tok_struct
_tmp522;_tmp522.tag=Cyc_Type_tok;_tmp522.f1=(void*)Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp523=_cycalloc(sizeof(*_tmp523));_tmp523->v=(void*)((void*)3);
_tmp523;}),0);_tmp522;});_tmp521;});break;case 148: _LL31C: yyval=yyvs[yyvsp_offset];
break;case 149: _LL31D: yyval=(void*)({struct Cyc_TypeQual_tok_struct*_tmp524=
_cycalloc_atomic(sizeof(*_tmp524));_tmp524[0]=({struct Cyc_TypeQual_tok_struct
_tmp525;_tmp525.tag=Cyc_TypeQual_tok;_tmp525.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[yyvsp_offset]));_tmp525;});_tmp524;});break;case 150: _LL31E: yyval=(void*)({
struct Cyc_YY1_struct*_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526[0]=({struct Cyc_YY1_struct
_tmp527;_tmp527.tag=Cyc_YY1;_tmp527.f1=({struct _tuple13*_tmp528=_cycalloc(
sizeof(*_tmp528));_tmp528->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp528->f2=0;_tmp528->f3=
0;_tmp528->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp528->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp528;});_tmp527;});_tmp526;});break;case 151: _LL31F: yyval=(void*)({struct Cyc_YY1_struct*
_tmp529=_cycalloc(sizeof(*_tmp529));_tmp529[0]=({struct Cyc_YY1_struct _tmp52A;
_tmp52A.tag=Cyc_YY1;_tmp52A.f1=({struct _tuple13*_tmp52B=_cycalloc(sizeof(*
_tmp52B));_tmp52B->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]));_tmp52B->f2=1;_tmp52B->f3=0;_tmp52B->f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp52B->f5=Cyc_yyget_Rgnorder_tok(
yyvs[yyvsp_offset]);_tmp52B;});_tmp52A;});_tmp529;});break;case 152: _LL320: {
struct _tuple2 _tmp52D;void*_tmp52E;struct Cyc_Absyn_Tqual _tmp52F;struct Cyc_Core_Opt*
_tmp530;struct _tuple2*_tmp52C=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp52D=*_tmp52C;_LL325:
_tmp530=_tmp52D.f1;goto _LL324;_LL324: _tmp52F=_tmp52D.f2;goto _LL323;_LL323:
_tmp52E=_tmp52D.f3;goto _LL322;_LL322: {struct Cyc_Absyn_VarargInfo*_tmp531=({
struct Cyc_Absyn_VarargInfo*_tmp535=_cycalloc(sizeof(*_tmp535));_tmp535->name=
_tmp530;_tmp535->tq=_tmp52F;_tmp535->type=(void*)_tmp52E;_tmp535->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp535;});yyval=(
void*)({struct Cyc_YY1_struct*_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532[0]=({
struct Cyc_YY1_struct _tmp533;_tmp533.tag=Cyc_YY1;_tmp533.f1=({struct _tuple13*
_tmp534=_cycalloc(sizeof(*_tmp534));_tmp534->f1=0;_tmp534->f2=0;_tmp534->f3=
_tmp531;_tmp534->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp534->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp534;});_tmp533;});_tmp532;});break;}}case 153: _LL321: {struct _tuple2 _tmp537;
void*_tmp538;struct Cyc_Absyn_Tqual _tmp539;struct Cyc_Core_Opt*_tmp53A;struct
_tuple2*_tmp536=Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp537=*_tmp536;_LL32A: _tmp53A=_tmp537.f1;goto _LL329;
_LL329: _tmp539=_tmp537.f2;goto _LL328;_LL328: _tmp538=_tmp537.f3;goto _LL327;_LL327: {
struct Cyc_Absyn_VarargInfo*_tmp53B=({struct Cyc_Absyn_VarargInfo*_tmp53F=
_cycalloc(sizeof(*_tmp53F));_tmp53F->name=_tmp53A;_tmp53F->tq=_tmp539;_tmp53F->type=(
void*)_tmp538;_tmp53F->inject=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp53F;});yyval=(void*)({
struct Cyc_YY1_struct*_tmp53C=_cycalloc(sizeof(*_tmp53C));_tmp53C[0]=({struct Cyc_YY1_struct
_tmp53D;_tmp53D.tag=Cyc_YY1;_tmp53D.f1=({struct _tuple13*_tmp53E=_cycalloc(
sizeof(*_tmp53E));_tmp53E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp53E->f2=0;_tmp53E->f3=
_tmp53B;_tmp53E->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp53E->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp53E;});_tmp53D;});_tmp53C;});break;}}case 154: _LL326: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp540=_cycalloc(sizeof(*_tmp540));_tmp540[0]=({struct Cyc_Type_tok_struct
_tmp541;_tmp541.tag=Cyc_Type_tok;_tmp541.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp542=_cycalloc(
sizeof(*_tmp542));_tmp542[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp543;_tmp543.tag=
1;_tmp543.f1=0;_tmp543;});_tmp542;}));_tmp541;});_tmp540;});break;case 155: _LL32B:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp544=_cycalloc(sizeof(*_tmp544));
_tmp544[0]=({struct Cyc_Type_tok_struct _tmp545;_tmp545.tag=Cyc_Type_tok;_tmp545.f1=(
void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp546=_cycalloc(
sizeof(*_tmp546));_tmp546[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp547;_tmp547.tag=0;
_tmp547.f1=(void*)Cyc_yyget_Kind_tok(yyvs[yyvsp_offset]);_tmp547;});_tmp546;}));
_tmp545;});_tmp544;});break;case 156: _LL32C: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp548=_cycalloc(sizeof(*_tmp548));_tmp548[0]=({struct Cyc_TypeOpt_tok_struct
_tmp549;_tmp549.tag=Cyc_TypeOpt_tok;_tmp549.f1=0;_tmp549;});_tmp548;});break;
case 157: _LL32D: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp54A=_cycalloc(
sizeof(*_tmp54A));_tmp54A[0]=({struct Cyc_TypeOpt_tok_struct _tmp54B;_tmp54B.tag=
Cyc_TypeOpt_tok;_tmp54B.f1=({struct Cyc_Core_Opt*_tmp54C=_cycalloc(sizeof(*
_tmp54C));_tmp54C->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp54D=
_cycalloc(sizeof(*_tmp54D));_tmp54D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp54E;
_tmp54E.tag=20;_tmp54E.f1=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp54E;});
_tmp54D;}));_tmp54C;});_tmp54B;});_tmp54A;});break;case 158: _LL32E: yyval=(void*)({
struct Cyc_Rgnorder_tok_struct*_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F[0]=({
struct Cyc_Rgnorder_tok_struct _tmp550;_tmp550.tag=Cyc_Rgnorder_tok;_tmp550.f1=0;
_tmp550;});_tmp54F;});break;case 159: _LL32F: yyval=yyvs[yyvsp_offset];break;case
160: _LL330: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp551=_cycalloc(
sizeof(*_tmp551));_tmp551[0]=({struct Cyc_Rgnorder_tok_struct _tmp552;_tmp552.tag=
Cyc_Rgnorder_tok;_tmp552.f1=({struct Cyc_List_List*_tmp553=_cycalloc(sizeof(*
_tmp553));_tmp553->hd=({struct _tuple16*_tmp554=_cycalloc(sizeof(*_tmp554));
_tmp554->f1=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp558;_tmp558.tag=0;_tmp558.f1=(void*)((void*)3);_tmp558;});_tmp557;}));
_tmp554->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),(void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp556;_tmp556.tag=0;_tmp556.f1=(void*)((void*)3);
_tmp556;});_tmp555;}));_tmp554;});_tmp553->tl=0;_tmp553;});_tmp552;});_tmp551;});
break;case 161: _LL331: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp559=
_cycalloc(sizeof(*_tmp559));_tmp559[0]=({struct Cyc_Rgnorder_tok_struct _tmp55A;
_tmp55A.tag=Cyc_Rgnorder_tok;_tmp55A.f1=({struct Cyc_List_List*_tmp55B=_cycalloc(
sizeof(*_tmp55B));_tmp55B->hd=({struct _tuple16*_tmp55C=_cycalloc(sizeof(*_tmp55C));
_tmp55C->f1=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp560;_tmp560.tag=0;_tmp560.f1=(void*)((void*)3);_tmp560;});_tmp55F;}));
_tmp55C->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp55E;_tmp55E.tag=0;_tmp55E.f1=(void*)((void*)3);_tmp55E;});_tmp55D;}));
_tmp55C;});_tmp55B->tl=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);_tmp55B;});
_tmp55A;});_tmp559;});break;case 162: _LL332: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp561=_cycalloc_atomic(sizeof(*_tmp561));_tmp561[0]=({struct Cyc_Bool_tok_struct
_tmp562;_tmp562.tag=Cyc_Bool_tok;_tmp562.f1=0;_tmp562;});_tmp561;});break;case
163: _LL333: if(Cyc_Std_zstrcmp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),_tag_arr("inject",
sizeof(unsigned char),7))!= 0){Cyc_Parse_err(_tag_arr("missing type in function declaration",
sizeof(unsigned char),37),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp563=_cycalloc_atomic(sizeof(*_tmp563));_tmp563[0]=({struct Cyc_Bool_tok_struct
_tmp564;_tmp564.tag=Cyc_Bool_tok;_tmp564.f1=1;_tmp564;});_tmp563;});break;case
164: _LL334: yyval=yyvs[yyvsp_offset];break;case 165: _LL335: yyval=(void*)({struct
Cyc_TypeList_tok_struct*_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565[0]=({struct
Cyc_TypeList_tok_struct _tmp566;_tmp566.tag=Cyc_TypeList_tok;_tmp566.f1=Cyc_List_imp_append(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]));_tmp566;});_tmp565;});break;case 166:
_LL336: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp567=_cycalloc(sizeof(*
_tmp567));_tmp567[0]=({struct Cyc_TypeList_tok_struct _tmp568;_tmp568.tag=Cyc_TypeList_tok;
_tmp568.f1=0;_tmp568;});_tmp567;});break;case 167: _LL337: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 168: _LL338:
yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp569=_cycalloc(sizeof(*_tmp569));
_tmp569[0]=({struct Cyc_TypeList_tok_struct _tmp56A;_tmp56A.tag=Cyc_TypeList_tok;
_tmp56A.f1=({struct Cyc_List_List*_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp56C=_cycalloc(sizeof(*_tmp56C));
_tmp56C[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp56D;_tmp56D.tag=21;_tmp56D.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp56D;});_tmp56C;}));_tmp56B->tl=0;_tmp56B;});_tmp56A;});_tmp569;});
break;case 169: _LL339: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
yyvsp_offset]),(void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp56E=
_cycalloc(sizeof(*_tmp56E));_tmp56E[0]=({struct Cyc_TypeList_tok_struct _tmp56F;
_tmp56F.tag=Cyc_TypeList_tok;_tmp56F.f1=({struct Cyc_List_List*_tmp570=_cycalloc(
sizeof(*_tmp570));_tmp570->hd=(void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);
_tmp570->tl=0;_tmp570;});_tmp56F;});_tmp56E;});break;case 170: _LL33A: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571[0]=({struct Cyc_TypeList_tok_struct
_tmp572;_tmp572.tag=Cyc_TypeList_tok;_tmp572.f1=({struct Cyc_List_List*_tmp573=
_cycalloc(sizeof(*_tmp573));_tmp573->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp575;_tmp575.tag=19;_tmp575.f1=(void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);
_tmp575;});_tmp574;}));_tmp573->tl=0;_tmp573;});_tmp572;});_tmp571;});break;case
171: _LL33B: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(void*)({
struct Cyc_TypeList_tok_struct*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576[0]=({
struct Cyc_TypeList_tok_struct _tmp577;_tmp577.tag=Cyc_TypeList_tok;_tmp577.f1=({
struct Cyc_List_List*_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp579=_cycalloc(sizeof(*_tmp579));
_tmp579[0]=({struct Cyc_Absyn_AccessEff_struct _tmp57A;_tmp57A.tag=19;_tmp57A.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp57A;});_tmp579;}));_tmp578->tl=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);
_tmp578;});_tmp577;});_tmp576;});break;case 172: _LL33C: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp57C;_tmp57C.tag=Cyc_ParamDeclList_tok;_tmp57C.f1=({struct Cyc_List_List*
_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->hd=Cyc_yyget_ParamDecl_tok(yyvs[
yyvsp_offset]);_tmp57D->tl=0;_tmp57D;});_tmp57C;});_tmp57B;});break;case 173:
_LL33D: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*_tmp57E=_cycalloc(
sizeof(*_tmp57E));_tmp57E[0]=({struct Cyc_ParamDeclList_tok_struct _tmp57F;_tmp57F.tag=
Cyc_ParamDeclList_tok;_tmp57F.f1=({struct Cyc_List_List*_tmp580=_cycalloc(sizeof(*
_tmp580));_tmp580->hd=Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset]);_tmp580->tl=
Cyc_yyget_ParamDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp580;});_tmp57F;});_tmp57E;});break;case 174: _LL33E: {
struct _tuple14 _tmp582;struct Cyc_List_List*_tmp583;struct Cyc_List_List*_tmp584;
struct Cyc_Absyn_Tqual _tmp585;struct _tuple14*_tmp581=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp582=*_tmp581;
_LL343: _tmp585=_tmp582.f1;goto _LL342;_LL342: _tmp584=_tmp582.f2;goto _LL341;_LL341:
_tmp583=_tmp582.f3;goto _LL340;_LL340: {struct Cyc_Parse_Declarator _tmp587;struct
Cyc_List_List*_tmp588;struct _tuple1*_tmp589;struct Cyc_Parse_Declarator*_tmp586=
Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]);_tmp587=*_tmp586;_LL346: _tmp589=
_tmp587.id;goto _LL345;_LL345: _tmp588=_tmp587.tms;goto _LL344;_LL344: {void*
_tmp58A=Cyc_Parse_speclist2typ(_tmp584,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp58C;struct Cyc_List_List*_tmp58D;void*_tmp58E;struct Cyc_Absyn_Tqual _tmp58F;
struct _tuple5 _tmp58B=Cyc_Parse_apply_tms(_tmp585,_tmp58A,_tmp583,_tmp588);_LL34B:
_tmp58F=_tmp58B.f1;goto _LL34A;_LL34A: _tmp58E=_tmp58B.f2;goto _LL349;_LL349:
_tmp58D=_tmp58B.f3;goto _LL348;_LL348: _tmp58C=_tmp58B.f4;goto _LL347;_LL347: if(
_tmp58D != 0){Cyc_Parse_err(_tag_arr("parameter with bad type params",sizeof(
unsigned char),31),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp589)){Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a namespace",sizeof(unsigned char),
47),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*_tmp590=(struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt*_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595->v=(*_tmp589).f2;
_tmp595;});if(_tmp58C != 0){({void*_tmp591[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("extra attributes on parameter, ignoring",
sizeof(unsigned char),40),_tag_arr(_tmp591,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592[0]=({
struct Cyc_ParamDecl_tok_struct _tmp593;_tmp593.tag=Cyc_ParamDecl_tok;_tmp593.f1=({
struct _tuple2*_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594->f1=_tmp590;_tmp594->f2=
_tmp58F;_tmp594->f3=_tmp58E;_tmp594;});_tmp593;});_tmp592;});break;}}}}case 175:
_LL33F: {struct _tuple14 _tmp597;struct Cyc_List_List*_tmp598;struct Cyc_List_List*
_tmp599;struct Cyc_Absyn_Tqual _tmp59A;struct _tuple14*_tmp596=Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset]);_tmp597=*_tmp596;_LL350: _tmp59A=_tmp597.f1;goto _LL34F;_LL34F:
_tmp599=_tmp597.f2;goto _LL34E;_LL34E: _tmp598=_tmp597.f3;goto _LL34D;_LL34D: {void*
_tmp59B=Cyc_Parse_speclist2typ(_tmp599,Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));if(_tmp598 != 0){({
void*_tmp59C[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(unsigned char),38),_tag_arr(_tmp59C,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D[0]=({
struct Cyc_ParamDecl_tok_struct _tmp59E;_tmp59E.tag=Cyc_ParamDecl_tok;_tmp59E.f1=({
struct _tuple2*_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F->f1=0;_tmp59F->f2=
_tmp59A;_tmp59F->f3=_tmp59B;_tmp59F;});_tmp59E;});_tmp59D;});break;}}case 176:
_LL34C: {struct _tuple14 _tmp5A1;struct Cyc_List_List*_tmp5A2;struct Cyc_List_List*
_tmp5A3;struct Cyc_Absyn_Tqual _tmp5A4;struct _tuple14*_tmp5A0=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A1=*_tmp5A0;
_LL355: _tmp5A4=_tmp5A1.f1;goto _LL354;_LL354: _tmp5A3=_tmp5A1.f2;goto _LL353;_LL353:
_tmp5A2=_tmp5A1.f3;goto _LL352;_LL352: {void*_tmp5A5=Cyc_Parse_speclist2typ(
_tmp5A3,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp5A6=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[yyvsp_offset]))->tms;struct Cyc_List_List*_tmp5A8;struct Cyc_List_List*
_tmp5A9;void*_tmp5AA;struct Cyc_Absyn_Tqual _tmp5AB;struct _tuple5 _tmp5A7=Cyc_Parse_apply_tms(
_tmp5A4,_tmp5A5,_tmp5A2,_tmp5A6);_LL35A: _tmp5AB=_tmp5A7.f1;goto _LL359;_LL359:
_tmp5AA=_tmp5A7.f2;goto _LL358;_LL358: _tmp5A9=_tmp5A7.f3;goto _LL357;_LL357:
_tmp5A8=_tmp5A7.f4;goto _LL356;_LL356: if(_tmp5A9 != 0){({void*_tmp5AC[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad type parameters on formal argument, ignoring",
sizeof(unsigned char),49),_tag_arr(_tmp5AC,sizeof(void*),0));});}if(_tmp5A8 != 0){({
void*_tmp5AD[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(unsigned char),38),_tag_arr(_tmp5AD,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE[0]=({
struct Cyc_ParamDecl_tok_struct _tmp5AF;_tmp5AF.tag=Cyc_ParamDecl_tok;_tmp5AF.f1=({
struct _tuple2*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->f1=0;_tmp5B0->f2=
_tmp5AB;_tmp5B0->f3=_tmp5AA;_tmp5B0;});_tmp5AF;});_tmp5AE;});break;}}case 177:
_LL351: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5B1=_cycalloc(sizeof(*
_tmp5B1));_tmp5B1[0]=({struct Cyc_IdList_tok_struct _tmp5B2;_tmp5B2.tag=Cyc_IdList_tok;
_tmp5B2.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(yyvs[yyvsp_offset]));_tmp5B2;});_tmp5B1;});break;case 178:
_LL35B: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5B3=_cycalloc(sizeof(*
_tmp5B3));_tmp5B3[0]=({struct Cyc_IdList_tok_struct _tmp5B4;_tmp5B4.tag=Cyc_IdList_tok;
_tmp5B4.f1=({struct Cyc_List_List*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5->hd=({
struct _tagged_arr*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp5B6;});_tmp5B5->tl=0;_tmp5B5;});_tmp5B4;});_tmp5B3;});
break;case 179: _LL35C: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5B7=
_cycalloc(sizeof(*_tmp5B7));_tmp5B7[0]=({struct Cyc_IdList_tok_struct _tmp5B8;
_tmp5B8.tag=Cyc_IdList_tok;_tmp5B8.f1=({struct Cyc_List_List*_tmp5B9=_cycalloc(
sizeof(*_tmp5B9));_tmp5B9->hd=({struct _tagged_arr*_tmp5BA=_cycalloc(sizeof(*
_tmp5BA));_tmp5BA[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp5BA;});_tmp5B9->tl=
Cyc_yyget_IdList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5B9;});_tmp5B8;});_tmp5B7;});break;case 180: _LL35D: yyval=yyvs[yyvsp_offset];
break;case 181: _LL35E: yyval=yyvs[yyvsp_offset];break;case 182: _LL35F: yyval=(void*)({
struct Cyc_Exp_tok_struct*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB[0]=({struct
Cyc_Exp_tok_struct _tmp5BC;_tmp5BC.tag=Cyc_Exp_tok;_tmp5BC.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));
_tmp5BD[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5BE;_tmp5BE.tag=34;
_tmp5BE.f1=0;_tmp5BE.f2=0;_tmp5BE;});_tmp5BD;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5BC;});_tmp5BB;});break;case 183: _LL360: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF[0]=({
struct Cyc_Exp_tok_struct _tmp5C0;_tmp5C0.tag=Cyc_Exp_tok;_tmp5C0.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));
_tmp5C1[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5C2;_tmp5C2.tag=34;
_tmp5C2.f1=0;_tmp5C2.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp5C2;});_tmp5C1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5C0;});_tmp5BF;});break;case 184: _LL361: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3[0]=({
struct Cyc_Exp_tok_struct _tmp5C4;_tmp5C4.tag=Cyc_Exp_tok;_tmp5C4.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5C5=_cycalloc(sizeof(*_tmp5C5));
_tmp5C5[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5C6;_tmp5C6.tag=34;
_tmp5C6.f1=0;_tmp5C6.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp5C6;});_tmp5C5;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5C4;});_tmp5C3;});break;case 185: _LL362: {struct
Cyc_Absyn_Vardecl*_tmp5C7=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp5CD=
_cycalloc(sizeof(*_tmp5CD));_tmp5CD->f1=(void*)0;_tmp5CD->f2=({struct _tagged_arr*
_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp5CE;});_tmp5CD;}),
Cyc_Absyn_uint_t,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));_tmp5C7->tq=({
struct Cyc_Absyn_Tqual _tmp5C8;_tmp5C8.q_const=1;_tmp5C8.q_volatile=0;_tmp5C8.q_restrict=
1;_tmp5C8;});yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp5C9=_cycalloc(sizeof(*
_tmp5C9));_tmp5C9[0]=({struct Cyc_Exp_tok_struct _tmp5CA;_tmp5CA.tag=Cyc_Exp_tok;
_tmp5CA.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp5CB=_cycalloc(sizeof(*_tmp5CB));_tmp5CB[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp5CC;_tmp5CC.tag=27;_tmp5CC.f1=_tmp5C7;_tmp5CC.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5CC.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5CC;});_tmp5CB;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmp5CA;});
_tmp5C9;});break;}case 186: _LL363: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF[0]=({struct Cyc_InitializerList_tok_struct
_tmp5D0;_tmp5D0.tag=Cyc_InitializerList_tok;_tmp5D0.f1=({struct Cyc_List_List*
_tmp5D1=_cycalloc(sizeof(*_tmp5D1));_tmp5D1->hd=({struct _tuple17*_tmp5D2=
_cycalloc(sizeof(*_tmp5D2));_tmp5D2->f1=0;_tmp5D2->f2=Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]);_tmp5D2;});_tmp5D1->tl=0;_tmp5D1;});_tmp5D0;});_tmp5CF;});break;
case 187: _LL364: yyval=(void*)({struct Cyc_InitializerList_tok_struct*_tmp5D3=
_cycalloc(sizeof(*_tmp5D3));_tmp5D3[0]=({struct Cyc_InitializerList_tok_struct
_tmp5D4;_tmp5D4.tag=Cyc_InitializerList_tok;_tmp5D4.f1=({struct Cyc_List_List*
_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->hd=({struct _tuple17*_tmp5D6=
_cycalloc(sizeof(*_tmp5D6));_tmp5D6->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D6->f2=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp5D6;});_tmp5D5->tl=0;_tmp5D5;});_tmp5D4;});_tmp5D3;});
break;case 188: _LL365: yyval=(void*)({struct Cyc_InitializerList_tok_struct*_tmp5D7=
_cycalloc(sizeof(*_tmp5D7));_tmp5D7[0]=({struct Cyc_InitializerList_tok_struct
_tmp5D8;_tmp5D8.tag=Cyc_InitializerList_tok;_tmp5D8.f1=({struct Cyc_List_List*
_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->hd=({struct _tuple17*_tmp5DA=
_cycalloc(sizeof(*_tmp5DA));_tmp5DA->f1=0;_tmp5DA->f2=Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]);_tmp5DA;});_tmp5D9->tl=Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D9;});_tmp5D8;});
_tmp5D7;});break;case 189: _LL366: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB[0]=({struct Cyc_InitializerList_tok_struct
_tmp5DC;_tmp5DC.tag=Cyc_InitializerList_tok;_tmp5DC.f1=({struct Cyc_List_List*
_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD->hd=({struct _tuple17*_tmp5DE=
_cycalloc(sizeof(*_tmp5DE));_tmp5DE->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5DE->f2=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp5DE;});_tmp5DD->tl=Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5DD;});_tmp5DC;});
_tmp5DB;});break;case 190: _LL367: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF[0]=({struct Cyc_DesignatorList_tok_struct
_tmp5E0;_tmp5E0.tag=Cyc_DesignatorList_tok;_tmp5E0.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp5E0;});_tmp5DF;});
break;case 191: _LL368: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp5E1=
_cycalloc(sizeof(*_tmp5E1));_tmp5E1[0]=({struct Cyc_DesignatorList_tok_struct
_tmp5E2;_tmp5E2.tag=Cyc_DesignatorList_tok;_tmp5E2.f1=({struct Cyc_List_List*
_tmp5E3=_cycalloc(sizeof(*_tmp5E3));_tmp5E3->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[yyvsp_offset]);_tmp5E3->tl=0;_tmp5E3;});_tmp5E2;});_tmp5E1;});break;case 192:
_LL369: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp5E4=_cycalloc(
sizeof(*_tmp5E4));_tmp5E4[0]=({struct Cyc_DesignatorList_tok_struct _tmp5E5;
_tmp5E5.tag=Cyc_DesignatorList_tok;_tmp5E5.f1=({struct Cyc_List_List*_tmp5E6=
_cycalloc(sizeof(*_tmp5E6));_tmp5E6->hd=(void*)Cyc_yyget_Designator_tok(yyvs[
yyvsp_offset]);_tmp5E6->tl=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E6;});_tmp5E5;});
_tmp5E4;});break;case 193: _LL36A: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7[0]=({struct Cyc_Designator_tok_struct
_tmp5E8;_tmp5E8.tag=Cyc_Designator_tok;_tmp5E8.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp5EA;_tmp5EA.tag=0;_tmp5EA.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5EA;});_tmp5E9;}));
_tmp5E8;});_tmp5E7;});break;case 194: _LL36B: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp5EB=_cycalloc(sizeof(*_tmp5EB));_tmp5EB[0]=({struct Cyc_Designator_tok_struct
_tmp5EC;_tmp5EC.tag=Cyc_Designator_tok;_tmp5EC.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp5ED=_cycalloc(sizeof(*_tmp5ED));_tmp5ED[0]=({struct Cyc_Absyn_FieldName_struct
_tmp5EE;_tmp5EE.tag=1;_tmp5EE.f1=({struct _tagged_arr*_tmp5EF=_cycalloc(sizeof(*
_tmp5EF));_tmp5EF[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp5EF;});_tmp5EE;});
_tmp5ED;}));_tmp5EC;});_tmp5EB;});break;case 195: _LL36C: {void*_tmp5F0=Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));struct Cyc_List_List*
_tmp5F1=(*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f3;if(_tmp5F1 != 0){({
void*_tmp5F2[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("ignoring attributes in type",
sizeof(unsigned char),28),_tag_arr(_tmp5F2,sizeof(void*),0));});}{struct Cyc_Absyn_Tqual
_tmp5F3=(*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f1;yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4[0]=({
struct Cyc_ParamDecl_tok_struct _tmp5F5;_tmp5F5.tag=Cyc_ParamDecl_tok;_tmp5F5.f1=({
struct _tuple2*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->f1=0;_tmp5F6->f2=
_tmp5F3;_tmp5F6->f3=_tmp5F0;_tmp5F6;});_tmp5F5;});_tmp5F4;});break;}}case 196:
_LL36D: {void*_tmp5F7=Cyc_Parse_speclist2typ((*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp5F8=(*Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f3;struct Cyc_Absyn_Tqual _tmp5F9=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f1;struct Cyc_List_List*
_tmp5FA=(Cyc_yyget_AbstractDeclarator_tok(yyvs[yyvsp_offset]))->tms;struct
_tuple5 _tmp5FB=Cyc_Parse_apply_tms(_tmp5F9,_tmp5F7,_tmp5F8,_tmp5FA);if(_tmp5FB.f3
!= 0){({void*_tmp5FC[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad type params, ignoring",
sizeof(unsigned char),26),_tag_arr(_tmp5FC,sizeof(void*),0));});}if(_tmp5FB.f4 != 
0){({void*_tmp5FD[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad specifiers, ignoring",
sizeof(unsigned char),25),_tag_arr(_tmp5FD,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE[0]=({
struct Cyc_ParamDecl_tok_struct _tmp5FF;_tmp5FF.tag=Cyc_ParamDecl_tok;_tmp5FF.f1=({
struct _tuple2*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->f1=0;_tmp600->f2=
_tmp5FB.f1;_tmp600->f3=_tmp5FB.f2;_tmp600;});_tmp5FF;});_tmp5FE;});break;}case
197: _LL36E: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp601=_cycalloc(sizeof(*
_tmp601));_tmp601[0]=({struct Cyc_Type_tok_struct _tmp602;_tmp602.tag=Cyc_Type_tok;
_tmp602.f1=(void*)(*Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset])).f3;_tmp602;});
_tmp601;});break;case 198: _LL36F: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp603=
_cycalloc(sizeof(*_tmp603));_tmp603[0]=({struct Cyc_Type_tok_struct _tmp604;
_tmp604.tag=Cyc_Type_tok;_tmp604.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp606;_tmp606.tag=20;_tmp606.f1=0;_tmp606;});_tmp605;}));_tmp604;});_tmp603;});
break;case 199: _LL370: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp607=_cycalloc(
sizeof(*_tmp607));_tmp607[0]=({struct Cyc_Type_tok_struct _tmp608;_tmp608.tag=Cyc_Type_tok;
_tmp608.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp609=_cycalloc(
sizeof(*_tmp609));_tmp609[0]=({struct Cyc_Absyn_JoinEff_struct _tmp60A;_tmp60A.tag=
20;_tmp60A.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp60A;});_tmp609;}));_tmp608;});_tmp607;});break;case 200:
_LL371: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp60B=_cycalloc(sizeof(*
_tmp60B));_tmp60B[0]=({struct Cyc_Type_tok_struct _tmp60C;_tmp60C.tag=Cyc_Type_tok;
_tmp60C.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp60D=_cycalloc(
sizeof(*_tmp60D));_tmp60D[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp60E;_tmp60E.tag=
21;_tmp60E.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp60E;});_tmp60D;}));_tmp60C;});_tmp60B;});break;case
201: _LL372: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp60F=_cycalloc(sizeof(*
_tmp60F));_tmp60F[0]=({struct Cyc_Type_tok_struct _tmp610;_tmp610.tag=Cyc_Type_tok;
_tmp610.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp611=_cycalloc(
sizeof(*_tmp611));_tmp611[0]=({struct Cyc_Absyn_JoinEff_struct _tmp612;_tmp612.tag=
20;_tmp612.f1=({struct Cyc_List_List*_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp613->tl=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp613;});_tmp612;});
_tmp611;}));_tmp610;});_tmp60F;});break;case 202: _LL373: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614[0]=({struct Cyc_TypeList_tok_struct
_tmp615;_tmp615.tag=Cyc_TypeList_tok;_tmp615.f1=({struct Cyc_List_List*_tmp616=
_cycalloc(sizeof(*_tmp616));_tmp616->hd=(void*)Cyc_yyget_Type_tok(yyvs[
yyvsp_offset]);_tmp616->tl=0;_tmp616;});_tmp615;});_tmp614;});break;case 203:
_LL374: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp617=_cycalloc(sizeof(*
_tmp617));_tmp617[0]=({struct Cyc_TypeList_tok_struct _tmp618;_tmp618.tag=Cyc_TypeList_tok;
_tmp618.f1=({struct Cyc_List_List*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->hd=(
void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);_tmp619->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp619;});_tmp618;});
_tmp617;});break;case 204: _LL375: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp61B;_tmp61B.tag=Cyc_AbstractDeclarator_tok;_tmp61B.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp61C=_cycalloc(sizeof(*_tmp61C));_tmp61C->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]);_tmp61C;});_tmp61B;});_tmp61A;});break;case 205: _LL376: yyval=
yyvs[yyvsp_offset];break;case 206: _LL377: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp61D=_cycalloc(sizeof(*_tmp61D));_tmp61D[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp61E;_tmp61E.tag=Cyc_AbstractDeclarator_tok;_tmp61E.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[yyvsp_offset]))->tms);_tmp61F;});_tmp61E;});_tmp61D;});break;case 207: _LL378:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 208:
_LL379: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp620=_cycalloc(
sizeof(*_tmp620));_tmp620[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp621;
_tmp621.tag=Cyc_AbstractDeclarator_tok;_tmp621.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622->tms=({struct Cyc_List_List*_tmp623=
_cycalloc(sizeof(*_tmp623));_tmp623->hd=(void*)((void*)0);_tmp623->tl=0;_tmp623;});
_tmp622;});_tmp621;});_tmp620;});break;case 209: _LL37A: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp625;_tmp625.tag=Cyc_AbstractDeclarator_tok;_tmp625.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->tms=({struct Cyc_List_List*_tmp627=
_cycalloc(sizeof(*_tmp627));_tmp627->hd=(void*)((void*)0);_tmp627->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->tms;_tmp627;});
_tmp626;});_tmp625;});_tmp624;});break;case 210: _LL37B: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp629;_tmp629.tag=Cyc_AbstractDeclarator_tok;_tmp629.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->tms=({struct Cyc_List_List*_tmp62B=
_cycalloc(sizeof(*_tmp62B));_tmp62B->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp62D;_tmp62D.tag=0;_tmp62D.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp62D;});_tmp62C;}));
_tmp62B->tl=0;_tmp62B;});_tmp62A;});_tmp629;});_tmp628;});break;case 211: _LL37C:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp62E=_cycalloc(sizeof(*
_tmp62E));_tmp62E[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp62F;_tmp62F.tag=
Cyc_AbstractDeclarator_tok;_tmp62F.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630->tms=({struct Cyc_List_List*_tmp631=
_cycalloc(sizeof(*_tmp631));_tmp631->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp633;_tmp633.tag=0;_tmp633.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp633;});_tmp632;}));
_tmp631->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp631;});_tmp630;});
_tmp62F;});_tmp62E;});break;case 212: _LL37D: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp635;_tmp635.tag=Cyc_AbstractDeclarator_tok;_tmp635.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636->tms=({struct Cyc_List_List*_tmp637=
_cycalloc(sizeof(*_tmp637));_tmp637->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp639;_tmp639.tag=2;_tmp639.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp63B;_tmp63B.tag=1;_tmp63B.f1=0;_tmp63B.f2=0;_tmp63B.f3=0;_tmp63B.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp63B.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp63B;});_tmp63A;}));
_tmp639;});_tmp638;}));_tmp637->tl=0;_tmp637;});_tmp636;});_tmp635;});_tmp634;});
break;case 213: _LL37E: {struct _tuple13 _tmp63D;struct Cyc_List_List*_tmp63E;struct
Cyc_Core_Opt*_tmp63F;struct Cyc_Absyn_VarargInfo*_tmp640;int _tmp641;struct Cyc_List_List*
_tmp642;struct _tuple13*_tmp63C=Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp63D=*_tmp63C;_LL385: _tmp642=_tmp63D.f1;goto _LL384;
_LL384: _tmp641=_tmp63D.f2;goto _LL383;_LL383: _tmp640=_tmp63D.f3;goto _LL382;_LL382:
_tmp63F=_tmp63D.f4;goto _LL381;_LL381: _tmp63E=_tmp63D.f5;goto _LL380;_LL380: yyval=(
void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp643=_cycalloc(sizeof(*
_tmp643));_tmp643[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp644;_tmp644.tag=
Cyc_AbstractDeclarator_tok;_tmp644.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->tms=({struct Cyc_List_List*_tmp646=
_cycalloc(sizeof(*_tmp646));_tmp646->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp648;_tmp648.tag=2;_tmp648.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp64A;_tmp64A.tag=1;_tmp64A.f1=_tmp642;_tmp64A.f2=_tmp641;_tmp64A.f3=_tmp640;
_tmp64A.f4=_tmp63F;_tmp64A.f5=_tmp63E;_tmp64A;});_tmp649;}));_tmp648;});_tmp647;}));
_tmp646->tl=0;_tmp646;});_tmp645;});_tmp644;});_tmp643;});break;}case 214: _LL37F:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp64B=_cycalloc(sizeof(*
_tmp64B));_tmp64B[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp64C;_tmp64C.tag=
Cyc_AbstractDeclarator_tok;_tmp64C.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D->tms=({struct Cyc_List_List*_tmp64E=
_cycalloc(sizeof(*_tmp64E));_tmp64E->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp650;_tmp650.tag=2;_tmp650.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp652;_tmp652.tag=1;_tmp652.f1=0;_tmp652.f2=0;_tmp652.f3=0;_tmp652.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp652.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp652;});_tmp651;}));
_tmp650;});_tmp64F;}));_tmp64E->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp64E;});_tmp64D;});
_tmp64C;});_tmp64B;});break;case 215: _LL386: {struct _tuple13 _tmp654;struct Cyc_List_List*
_tmp655;struct Cyc_Core_Opt*_tmp656;struct Cyc_Absyn_VarargInfo*_tmp657;int _tmp658;
struct Cyc_List_List*_tmp659;struct _tuple13*_tmp653=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp654=*_tmp653;_LL38D:
_tmp659=_tmp654.f1;goto _LL38C;_LL38C: _tmp658=_tmp654.f2;goto _LL38B;_LL38B:
_tmp657=_tmp654.f3;goto _LL38A;_LL38A: _tmp656=_tmp654.f4;goto _LL389;_LL389:
_tmp655=_tmp654.f5;goto _LL388;_LL388: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp65B;_tmp65B.tag=Cyc_AbstractDeclarator_tok;_tmp65B.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->tms=({struct Cyc_List_List*_tmp65D=
_cycalloc(sizeof(*_tmp65D));_tmp65D->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp65F;_tmp65F.tag=2;_tmp65F.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp661;_tmp661.tag=1;_tmp661.f1=_tmp659;_tmp661.f2=_tmp658;_tmp661.f3=_tmp657;
_tmp661.f4=_tmp656;_tmp661.f5=_tmp655;_tmp661;});_tmp660;}));_tmp65F;});_tmp65E;}));
_tmp65D->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp65D;});_tmp65C;});
_tmp65B;});_tmp65A;});break;}case 216: _LL387: {struct Cyc_List_List*_tmp662=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp663=_cycalloc(sizeof(*
_tmp663));_tmp663[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp664;_tmp664.tag=
Cyc_AbstractDeclarator_tok;_tmp664.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665->tms=({struct Cyc_List_List*_tmp666=
_cycalloc(sizeof(*_tmp666));_tmp666->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp668;_tmp668.tag=3;_tmp668.f1=_tmp662;_tmp668.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp668.f3=0;_tmp668;});_tmp667;}));_tmp666->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp666;});
_tmp665;});_tmp664;});_tmp663;});break;}case 217: _LL38E: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp66A;_tmp66A.tag=Cyc_AbstractDeclarator_tok;_tmp66A.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B->tms=({struct Cyc_List_List*_tmp66C=
_cycalloc(sizeof(*_tmp66C));_tmp66C->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp66E;_tmp66E.tag=4;_tmp66E.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp66E.f2=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp66E;});_tmp66D;}));_tmp66C->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp66C;});
_tmp66B;});_tmp66A;});_tmp669;});break;case 218: _LL38F: yyval=yyvs[yyvsp_offset];
break;case 219: _LL390: yyval=yyvs[yyvsp_offset];break;case 220: _LL391: yyval=yyvs[
yyvsp_offset];break;case 221: _LL392: yyval=yyvs[yyvsp_offset];break;case 222: _LL393:
yyval=yyvs[yyvsp_offset];break;case 223: _LL394: yyval=yyvs[yyvsp_offset];break;
case 224: _LL395: if(Cyc_Std_zstrcmp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tag_arr("`H",sizeof(
unsigned char),3))== 0){Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(unsigned char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->name=({
struct _tagged_arr*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp67C;});_tmp679->identity=
0;_tmp679->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp67A=_cycalloc(
sizeof(*_tmp67A));_tmp67A[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp67B;_tmp67B.tag=0;
_tmp67B.f1=(void*)((void*)3);_tmp67B;});_tmp67A;}));_tmp679;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677[0]=({
struct Cyc_Absyn_VarType_struct _tmp678;_tmp678.tag=1;_tmp678.f1=tv;_tmp678;});
_tmp677;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp66F=_cycalloc(sizeof(*
_tmp66F));_tmp66F[0]=({struct Cyc_Stmt_tok_struct _tmp670;_tmp670.tag=Cyc_Stmt_tok;
_tmp670.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp671=
_cycalloc(sizeof(*_tmp671));_tmp671[0]=({struct Cyc_Absyn_Region_s_struct _tmp672;
_tmp672.tag=18;_tmp672.f1=tv;_tmp672.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673->f1=(void*)0;_tmp673->f2=({struct
_tagged_arr*_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp674;});_tmp673;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp675=_cycalloc(sizeof(*_tmp675));
_tmp675[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp676;_tmp676.tag=17;_tmp676.f1=(
void*)t;_tmp676;});_tmp675;}),0);_tmp672.f3=Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);
_tmp672;});_tmp671;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp670;});_tmp66F;});break;}case 225: _LL396: if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tag_arr("H",sizeof(unsigned char),2))== 0){Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(unsigned char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687->name=({
struct _tagged_arr*_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A[0]=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp68C;_tmp68C.tag=0;_tmp68C.f1=(
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);{void*_tmp68B[1]={& _tmp68C};Cyc_Std_aprintf(_tag_arr("`%s",
sizeof(unsigned char),4),_tag_arr(_tmp68B,sizeof(void*),1));}});_tmp68A;});
_tmp687->identity=0;_tmp687->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp689;_tmp689.tag=0;_tmp689.f1=(void*)((void*)3);_tmp689;});_tmp688;}));
_tmp687;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp685=_cycalloc(
sizeof(*_tmp685));_tmp685[0]=({struct Cyc_Absyn_VarType_struct _tmp686;_tmp686.tag=
1;_tmp686.f1=tv;_tmp686;});_tmp685;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D[0]=({struct Cyc_Stmt_tok_struct
_tmp67E;_tmp67E.tag=Cyc_Stmt_tok;_tmp67E.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F[0]=({
struct Cyc_Absyn_Region_s_struct _tmp680;_tmp680.tag=18;_tmp680.f1=tv;_tmp680.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp681=_cycalloc(sizeof(*_tmp681));
_tmp681->f1=(void*)0;_tmp681->f2=({struct _tagged_arr*_tmp682=_cycalloc(sizeof(*
_tmp682));_tmp682[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp682;});_tmp681;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp684;_tmp684.tag=17;_tmp684.f1=(void*)t;_tmp684;});_tmp683;}),0);_tmp680.f3=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp680;});_tmp67F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp67E;});_tmp67D;});break;}case 226: _LL397: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D[0]=({
struct Cyc_Stmt_tok_struct _tmp68E;_tmp68E.tag=Cyc_Stmt_tok;_tmp68E.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Cut_s_struct*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F[
0]=({struct Cyc_Absyn_Cut_s_struct _tmp690;_tmp690.tag=13;_tmp690.f1=Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]);_tmp690;});_tmp68F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp68E;});_tmp68D;});break;case 227: _LL398: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691[0]=({
struct Cyc_Stmt_tok_struct _tmp692;_tmp692.tag=Cyc_Stmt_tok;_tmp692.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmp693=_cycalloc(sizeof(*_tmp693));
_tmp693[0]=({struct Cyc_Absyn_Splice_s_struct _tmp694;_tmp694.tag=14;_tmp694.f1=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp694;});_tmp693;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp692;});_tmp691;});break;case 228: _LL399: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695[0]=({
struct Cyc_Stmt_tok_struct _tmp696;_tmp696.tag=Cyc_Stmt_tok;_tmp696.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp697=_cycalloc(sizeof(*_tmp697));
_tmp697[0]=({struct Cyc_Absyn_Label_s_struct _tmp698;_tmp698.tag=15;_tmp698.f1=({
struct _tagged_arr*_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp699;});_tmp698.f2=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp698;});_tmp697;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp696;});_tmp695;});break;case 229: _LL39A: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A[0]=({
struct Cyc_Stmt_tok_struct _tmp69B;_tmp69B.tag=Cyc_Stmt_tok;_tmp69B.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmp69B;});_tmp69A;});break;case 230: _LL39B: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C[0]=({struct Cyc_Stmt_tok_struct
_tmp69D;_tmp69D.tag=Cyc_Stmt_tok;_tmp69D.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp69D;});_tmp69C;});break;case 231: _LL39C: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp69E=_cycalloc(sizeof(*_tmp69E));_tmp69E[0]=({
struct Cyc_Stmt_tok_struct _tmp69F;_tmp69F.tag=Cyc_Stmt_tok;_tmp69F.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp69F;});
_tmp69E;});break;case 232: _LL39D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 233: _LL39E: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0[0]=({struct Cyc_Stmt_tok_struct
_tmp6A1;_tmp6A1.tag=Cyc_Stmt_tok;_tmp6A1.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(0));_tmp6A1;});_tmp6A0;});break;case 234:
_LL39F: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6A2=_cycalloc(sizeof(*
_tmp6A2));_tmp6A2[0]=({struct Cyc_Stmt_tok_struct _tmp6A3;_tmp6A3.tag=Cyc_Stmt_tok;
_tmp6A3.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]));_tmp6A3;});_tmp6A2;});break;case 235: _LL3A0: yyval=yyvs[
yyvsp_offset];break;case 236: _LL3A1: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6A4=_cycalloc(sizeof(*_tmp6A4));_tmp6A4[0]=({struct Cyc_Stmt_tok_struct
_tmp6A5;_tmp6A5.tag=Cyc_Stmt_tok;_tmp6A5.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6A5;});_tmp6A4;});break;case 237: _LL3A2: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6[0]=({
struct Cyc_Stmt_tok_struct _tmp6A7;_tmp6A7.tag=Cyc_Stmt_tok;_tmp6A7.f1=Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*_tmp6A8=_cycalloc(
sizeof(*_tmp6A8));_tmp6A8[0]=({struct Cyc_Absyn_Fn_d_struct _tmp6A9;_tmp6A9.tag=1;
_tmp6A9.f1=Cyc_yyget_FnDecl_tok(yyvs[yyvsp_offset]);_tmp6A9;});_tmp6A8;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line)),Cyc_Absyn_skip_stmt(
0));_tmp6A7;});_tmp6A6;});break;case 238: _LL3A3: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA[0]=({struct Cyc_Stmt_tok_struct
_tmp6AB;_tmp6AB.tag=Cyc_Stmt_tok;_tmp6AB.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));_tmp6AC[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp6AD;_tmp6AD.tag=1;_tmp6AD.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6AD;});_tmp6AC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]));_tmp6AB;});
_tmp6AA;});break;case 239: _LL3A4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6AE=
_cycalloc(sizeof(*_tmp6AE));_tmp6AE[0]=({struct Cyc_Stmt_tok_struct _tmp6AF;
_tmp6AF.tag=Cyc_Stmt_tok;_tmp6AF.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6AF;});_tmp6AE;});break;case 240: _LL3A5: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));_tmp6B0[0]=({
struct Cyc_Stmt_tok_struct _tmp6B1;_tmp6B1.tag=Cyc_Stmt_tok;_tmp6B1.f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6B1;});_tmp6B0;});break;case 241: _LL3A6: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6B2=_cycalloc(sizeof(*_tmp6B2));_tmp6B2[0]=({
struct Cyc_Stmt_tok_struct _tmp6B3;_tmp6B3.tag=Cyc_Stmt_tok;_tmp6B3.f1=Cyc_Absyn_switch_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6B3;});_tmp6B2;});break;case 242: _LL3A7: if(Cyc_Std_strcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tag_arr("C",sizeof(unsigned char),2))!= 0){Cyc_Parse_err(_tag_arr("only switch \"C\" { ... } is allowed",
sizeof(unsigned char),35),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6B4=
_cycalloc(sizeof(*_tmp6B4));_tmp6B4[0]=({struct Cyc_Stmt_tok_struct _tmp6B5;
_tmp6B5.tag=Cyc_Stmt_tok;_tmp6B5.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_SwitchC_s_struct*
_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6[0]=({struct Cyc_Absyn_SwitchC_s_struct
_tmp6B7;_tmp6B7.tag=10;_tmp6B7.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp6B7.f2=Cyc_yyget_SwitchCClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6B7;});_tmp6B6;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6B5;});
_tmp6B4;});break;case 243: _LL3A8: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6B8=
_cycalloc(sizeof(*_tmp6B8));_tmp6B8[0]=({struct Cyc_Stmt_tok_struct _tmp6B9;
_tmp6B9.tag=Cyc_Stmt_tok;_tmp6B9.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6B9;});_tmp6B8;});break;case 244: _LL3A9: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));
_tmp6BA[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp6BB;_tmp6BB.tag=Cyc_SwitchClauseList_tok;
_tmp6BB.f1=0;_tmp6BB;});_tmp6BA;});break;case 245: _LL3AA: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6BD;_tmp6BD.tag=Cyc_SwitchClauseList_tok;_tmp6BD.f1=({struct Cyc_List_List*
_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp6BF->pat_vars=0;_tmp6BF->where_clause=0;
_tmp6BF->body=Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp6BF->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6BF;});_tmp6BE->tl=0;_tmp6BE;});_tmp6BD;});_tmp6BC;});
break;case 246: _LL3AB: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6C1;_tmp6C1.tag=Cyc_SwitchClauseList_tok;_tmp6C1.f1=({struct Cyc_List_List*
_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6C3->pat_vars=0;
_tmp6C3->where_clause=0;_tmp6C3->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp6C3->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line);_tmp6C3;});_tmp6C2->tl=
Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmp6C2;});_tmp6C1;});_tmp6C0;});
break;case 247: _LL3AC: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6C4=_cycalloc(sizeof(*_tmp6C4));_tmp6C4[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6C5;_tmp6C5.tag=Cyc_SwitchClauseList_tok;_tmp6C5.f1=({struct Cyc_List_List*
_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6C7->pat_vars=0;
_tmp6C7->where_clause=0;_tmp6C7->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6C7->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp6C7;});
_tmp6C6->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmp6C6;});
_tmp6C5;});_tmp6C4;});break;case 248: _LL3AD: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6C9;_tmp6C9.tag=Cyc_SwitchClauseList_tok;_tmp6C9.f1=({struct Cyc_List_List*
_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp6CB->pat_vars=0;
_tmp6CB->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6CB->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp6CB->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6CB;});_tmp6CA->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmp6CA;});_tmp6C9;});_tmp6C8;});break;case 249: _LL3AE: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));
_tmp6CC[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp6CD;_tmp6CD.tag=Cyc_SwitchClauseList_tok;
_tmp6CD.f1=({struct Cyc_List_List*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));_tmp6CE->hd=({
struct Cyc_Absyn_Switch_clause*_tmp6CF=_cycalloc(sizeof(*_tmp6CF));_tmp6CF->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp6CF->pat_vars=0;_tmp6CF->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6CF->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6CF->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6CF;});_tmp6CE->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmp6CE;});_tmp6CD;});_tmp6CC;});break;case 250: _LL3AF: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));
_tmp6D0[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp6D1;_tmp6D1.tag=Cyc_SwitchCClauseList_tok;
_tmp6D1.f1=0;_tmp6D1;});_tmp6D0;});break;case 251: _LL3B0: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmp6D3;_tmp6D3.tag=Cyc_SwitchCClauseList_tok;_tmp6D3.f1=({struct Cyc_List_List*
_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4->hd=({struct Cyc_Absyn_SwitchC_clause*
_tmp6D5=_cycalloc(sizeof(*_tmp6D5));_tmp6D5->cnst_exp=0;_tmp6D5->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Absyn_break_stmt(0),0);_tmp6D5->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line);_tmp6D5;});_tmp6D4->tl=
0;_tmp6D4;});_tmp6D3;});_tmp6D2;});break;case 252: _LL3B1: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmp6D7;_tmp6D7.tag=Cyc_SwitchCClauseList_tok;_tmp6D7.f1=({struct Cyc_List_List*
_tmp6D8=_cycalloc(sizeof(*_tmp6D8));_tmp6D8->hd=({struct Cyc_Absyn_SwitchC_clause*
_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9->cnst_exp=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6D9->body=Cyc_Absyn_fallthru_stmt(
0,0);_tmp6D9->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6D9;});_tmp6D8->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmp6D8;});_tmp6D7;});_tmp6D6;});break;case 253: _LL3B2: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));
_tmp6DA[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp6DB;_tmp6DB.tag=Cyc_SwitchCClauseList_tok;
_tmp6DB.f1=({struct Cyc_List_List*_tmp6DC=_cycalloc(sizeof(*_tmp6DC));_tmp6DC->hd=({
struct Cyc_Absyn_SwitchC_clause*_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD->cnst_exp=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp6DD->body=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Absyn_fallthru_stmt(
0,0),0);_tmp6DD->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6DD;});_tmp6DC->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmp6DC;});_tmp6DB;});_tmp6DA;});break;case 254: _LL3B3: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE[0]=({
struct Cyc_Stmt_tok_struct _tmp6DF;_tmp6DF.tag=Cyc_Stmt_tok;_tmp6DF.f1=Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6DF;});_tmp6DE;});break;case 255: _LL3B4: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0[0]=({
struct Cyc_Stmt_tok_struct _tmp6E1;_tmp6E1.tag=Cyc_Stmt_tok;_tmp6E1.f1=Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6E1;});
_tmp6E0;});break;case 256: _LL3B5: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6E2=
_cycalloc(sizeof(*_tmp6E2));_tmp6E2[0]=({struct Cyc_Stmt_tok_struct _tmp6E3;
_tmp6E3.tag=Cyc_Stmt_tok;_tmp6E3.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6E3;});
_tmp6E2;});break;case 257: _LL3B6: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6E4=
_cycalloc(sizeof(*_tmp6E4));_tmp6E4[0]=({struct Cyc_Stmt_tok_struct _tmp6E5;
_tmp6E5.tag=Cyc_Stmt_tok;_tmp6E5.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6E5;});_tmp6E4;});break;case 258: _LL3B7: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6[0]=({
struct Cyc_Stmt_tok_struct _tmp6E7;_tmp6E7.tag=Cyc_Stmt_tok;_tmp6E7.f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6E7;});
_tmp6E6;});break;case 259: _LL3B8: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6E8=
_cycalloc(sizeof(*_tmp6E8));_tmp6E8[0]=({struct Cyc_Stmt_tok_struct _tmp6E9;
_tmp6E9.tag=Cyc_Stmt_tok;_tmp6E9.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp6E9;});
_tmp6E8;});break;case 260: _LL3B9: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6EA=
_cycalloc(sizeof(*_tmp6EA));_tmp6EA[0]=({struct Cyc_Stmt_tok_struct _tmp6EB;
_tmp6EB.tag=Cyc_Stmt_tok;_tmp6EB.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6EB;});_tmp6EA;});break;case 261: _LL3BA: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6EC=_cycalloc(sizeof(*_tmp6EC));_tmp6EC[0]=({
struct Cyc_Stmt_tok_struct _tmp6ED;_tmp6ED.tag=Cyc_Stmt_tok;_tmp6ED.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6ED;});_tmp6EC;});break;case 262: _LL3BB: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6EE=_cycalloc(sizeof(*_tmp6EE));_tmp6EE[0]=({
struct Cyc_Stmt_tok_struct _tmp6EF;_tmp6EF.tag=Cyc_Stmt_tok;_tmp6EF.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6EF;});_tmp6EE;});break;case 263: _LL3BC: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6F0=_cycalloc(sizeof(*_tmp6F0));_tmp6F0[0]=({
struct Cyc_Stmt_tok_struct _tmp6F1;_tmp6F1.tag=Cyc_Stmt_tok;_tmp6F1.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6F1;});_tmp6F0;});break;case 264: _LL3BD: {struct
Cyc_List_List*_tmp6F2=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*_tmp6F3=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4[0]=({
struct Cyc_Stmt_tok_struct _tmp6F5;_tmp6F5.tag=Cyc_Stmt_tok;_tmp6F5.f1=Cyc_Parse_flatten_declarations(
_tmp6F2,_tmp6F3);_tmp6F5;});_tmp6F4;});break;}case 265: _LL3BE: {struct Cyc_List_List*
_tmp6F6=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp6F7=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6F8=
_cycalloc(sizeof(*_tmp6F8));_tmp6F8[0]=({struct Cyc_Stmt_tok_struct _tmp6F9;
_tmp6F9.tag=Cyc_Stmt_tok;_tmp6F9.f1=Cyc_Parse_flatten_declarations(_tmp6F6,
_tmp6F7);_tmp6F9;});_tmp6F8;});break;}case 266: _LL3BF: {struct Cyc_List_List*
_tmp6FA=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp6FB=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6FC=
_cycalloc(sizeof(*_tmp6FC));_tmp6FC[0]=({struct Cyc_Stmt_tok_struct _tmp6FD;
_tmp6FD.tag=Cyc_Stmt_tok;_tmp6FD.f1=Cyc_Parse_flatten_declarations(_tmp6FA,
_tmp6FB);_tmp6FD;});_tmp6FC;});break;}case 267: _LL3C0: {struct Cyc_List_List*
_tmp6FE=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp6FF=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp700=
_cycalloc(sizeof(*_tmp700));_tmp700[0]=({struct Cyc_Stmt_tok_struct _tmp701;
_tmp701.tag=Cyc_Stmt_tok;_tmp701.f1=Cyc_Parse_flatten_declarations(_tmp6FE,
_tmp6FF);_tmp701;});_tmp700;});break;}case 268: _LL3C1: {struct Cyc_List_List*
_tmp702=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_decl2vardecl,Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp703=_cycalloc(sizeof(*_tmp703));_tmp703[0]=({
struct Cyc_Stmt_tok_struct _tmp704;_tmp704.tag=Cyc_Stmt_tok;_tmp704.f1=Cyc_Absyn_forarray_stmt(
_tmp702,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp704;});_tmp703;});break;}case 269: _LL3C2: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp705=_cycalloc(sizeof(*_tmp705));_tmp705[0]=({
struct Cyc_Stmt_tok_struct _tmp706;_tmp706.tag=Cyc_Stmt_tok;_tmp706.f1=Cyc_Absyn_goto_stmt(({
struct _tagged_arr*_tmp707=_cycalloc(sizeof(*_tmp707));_tmp707[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp707;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp706;});
_tmp705;});break;case 270: _LL3C3: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp708=
_cycalloc(sizeof(*_tmp708));_tmp708[0]=({struct Cyc_Stmt_tok_struct _tmp709;
_tmp709.tag=Cyc_Stmt_tok;_tmp709.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp709;});
_tmp708;});break;case 271: _LL3C4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp70A=
_cycalloc(sizeof(*_tmp70A));_tmp70A[0]=({struct Cyc_Stmt_tok_struct _tmp70B;
_tmp70B.tag=Cyc_Stmt_tok;_tmp70B.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp70B;});
_tmp70A;});break;case 272: _LL3C5: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp70C=
_cycalloc(sizeof(*_tmp70C));_tmp70C[0]=({struct Cyc_Stmt_tok_struct _tmp70D;
_tmp70D.tag=Cyc_Stmt_tok;_tmp70D.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp70D;});
_tmp70C;});break;case 273: _LL3C6: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp70E=
_cycalloc(sizeof(*_tmp70E));_tmp70E[0]=({struct Cyc_Stmt_tok_struct _tmp70F;
_tmp70F.tag=Cyc_Stmt_tok;_tmp70F.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp70F;});_tmp70E;});break;case 274: _LL3C7: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710[0]=({
struct Cyc_Stmt_tok_struct _tmp711;_tmp711.tag=Cyc_Stmt_tok;_tmp711.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp711;});_tmp710;});break;case 275: _LL3C8: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp712=_cycalloc(sizeof(*_tmp712));_tmp712[0]=({
struct Cyc_Stmt_tok_struct _tmp713;_tmp713.tag=Cyc_Stmt_tok;_tmp713.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp713;});_tmp712;});break;case 276: _LL3C9: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp714=_cycalloc(sizeof(*_tmp714));_tmp714[0]=({
struct Cyc_Stmt_tok_struct _tmp715;_tmp715.tag=Cyc_Stmt_tok;_tmp715.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp715;});_tmp714;});break;case 277: _LL3CA: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716[
0]=({struct Cyc_Pattern_tok_struct _tmp717;_tmp717.tag=Cyc_Pattern_tok;_tmp717.f1=
Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp717;});_tmp716;});break;case 278: _LL3CB: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 279: _LL3CC:
yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp718=_cycalloc(sizeof(*_tmp718));
_tmp718[0]=({struct Cyc_Pattern_tok_struct _tmp719;_tmp719.tag=Cyc_Pattern_tok;
_tmp719.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp71A=
_cycalloc(sizeof(*_tmp71A));_tmp71A[0]=({struct Cyc_Absyn_Int_p_struct _tmp71B;
_tmp71B.tag=1;_tmp71B.f1=(void*)(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f1;
_tmp71B.f2=(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2;_tmp71B;});_tmp71A;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp719;});
_tmp718;});break;case 280: _LL3CD: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C[0]=({struct Cyc_Pattern_tok_struct
_tmp71D;_tmp71D.tag=Cyc_Pattern_tok;_tmp71D.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Int_p_struct*_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E[0]=({struct
Cyc_Absyn_Int_p_struct _tmp71F;_tmp71F.tag=1;_tmp71F.f1=(void*)((void*)0);_tmp71F.f2=
-(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2;_tmp71F;});_tmp71E;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp71D;});_tmp71C;});break;case 281: _LL3CE: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp720=_cycalloc(sizeof(*_tmp720));_tmp720[
0]=({struct Cyc_Pattern_tok_struct _tmp721;_tmp721.tag=Cyc_Pattern_tok;_tmp721.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*_tmp722=_cycalloc(
sizeof(*_tmp722));_tmp722[0]=({struct Cyc_Absyn_Float_p_struct _tmp723;_tmp723.tag=
3;_tmp723.f1=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp723;});_tmp722;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp721;});
_tmp720;});break;case 282: _LL3CF: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp724=_cycalloc(sizeof(*_tmp724));_tmp724[0]=({struct Cyc_Pattern_tok_struct
_tmp725;_tmp725.tag=Cyc_Pattern_tok;_tmp725.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Char_p_struct*_tmp726=_cycalloc_atomic(sizeof(*_tmp726));_tmp726[0]=({
struct Cyc_Absyn_Char_p_struct _tmp727;_tmp727.tag=2;_tmp727.f1=Cyc_yyget_Char_tok(
yyvs[yyvsp_offset]);_tmp727;});_tmp726;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp725;});_tmp724;});
break;case 283: _LL3D0: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp728=
_cycalloc(sizeof(*_tmp728));_tmp728[0]=({struct Cyc_Pattern_tok_struct _tmp729;
_tmp729.tag=Cyc_Pattern_tok;_tmp729.f1=Cyc_Absyn_new_pat((void*)1,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp729;});
_tmp728;});break;case 284: _LL3D1: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp72A=_cycalloc(sizeof(*_tmp72A));_tmp72A[0]=({struct Cyc_Pattern_tok_struct
_tmp72B;_tmp72B.tag=Cyc_Pattern_tok;_tmp72B.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownId_p_struct*_tmp72C=_cycalloc(sizeof(*_tmp72C));_tmp72C[0]=({
struct Cyc_Absyn_UnknownId_p_struct _tmp72D;_tmp72D.tag=11;_tmp72D.f1=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp72D;});_tmp72C;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp72B;});_tmp72A;});
break;case 285: _LL3D2: {struct Cyc_List_List*_tmp72E=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp72F=_cycalloc(sizeof(*_tmp72F));_tmp72F[0]=({
struct Cyc_Pattern_tok_struct _tmp730;_tmp730.tag=Cyc_Pattern_tok;_tmp730.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp731=_cycalloc(sizeof(*_tmp731));
_tmp731[0]=({struct Cyc_Absyn_UnknownCall_p_struct _tmp732;_tmp732.tag=12;_tmp732.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp732.f2=_tmp72E;_tmp732.f3=Cyc_yyget_PatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp732;});_tmp731;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp730;});
_tmp72F;});break;}case 286: _LL3D3: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp733=_cycalloc(sizeof(*_tmp733));_tmp733[0]=({struct Cyc_Pattern_tok_struct
_tmp734;_tmp734.tag=Cyc_Pattern_tok;_tmp734.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Tuple_p_struct*_tmp735=_cycalloc(sizeof(*_tmp735));_tmp735[0]=({struct
Cyc_Absyn_Tuple_p_struct _tmp736;_tmp736.tag=4;_tmp736.f1=Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp736;});_tmp735;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp734;});
_tmp733;});break;case 287: _LL3D4: {struct Cyc_List_List*_tmp737=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp738=_cycalloc(sizeof(*_tmp738));_tmp738[0]=({
struct Cyc_Pattern_tok_struct _tmp739;_tmp739.tag=Cyc_Pattern_tok;_tmp739.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_UnknownFields_p_struct*_tmp73A=_cycalloc(sizeof(*_tmp73A));
_tmp73A[0]=({struct Cyc_Absyn_UnknownFields_p_struct _tmp73B;_tmp73B.tag=13;
_tmp73B.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp73B.f2=_tmp737;_tmp73B.f3=0;_tmp73B;});_tmp73A;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp739;});_tmp738;});break;}case 288: _LL3D5: {struct
Cyc_List_List*_tmp73C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp73D=_cycalloc(sizeof(*_tmp73D));_tmp73D[0]=({
struct Cyc_Pattern_tok_struct _tmp73E;_tmp73E.tag=Cyc_Pattern_tok;_tmp73E.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_UnknownFields_p_struct*_tmp73F=_cycalloc(sizeof(*_tmp73F));
_tmp73F[0]=({struct Cyc_Absyn_UnknownFields_p_struct _tmp740;_tmp740.tag=13;
_tmp740.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp740.f2=_tmp73C;_tmp740.f3=Cyc_yyget_FieldPatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp740;});_tmp73F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp73E;});
_tmp73D;});break;}case 289: _LL3D6: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741[0]=({struct Cyc_Pattern_tok_struct
_tmp742;_tmp742.tag=Cyc_Pattern_tok;_tmp742.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Pointer_p_struct*_tmp743=_cycalloc(sizeof(*_tmp743));_tmp743[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp744;_tmp744.tag=5;_tmp744.f1=Cyc_yyget_Pattern_tok(
yyvs[yyvsp_offset]);_tmp744;});_tmp743;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp742;});_tmp741;});break;case 290: _LL3D7: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745[
0]=({struct Cyc_Pattern_tok_struct _tmp746;_tmp746.tag=Cyc_Pattern_tok;_tmp746.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp747=_cycalloc(
sizeof(*_tmp747));_tmp747[0]=({struct Cyc_Absyn_Reference_p_struct _tmp748;_tmp748.tag=
6;_tmp748.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp749=_cycalloc(sizeof(*
_tmp749));_tmp749->f1=(void*)0;_tmp749->f2=({struct _tagged_arr*_tmp74A=_cycalloc(
sizeof(*_tmp74A));_tmp74A[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp74A;});
_tmp749;}),(void*)0,0);_tmp748;});_tmp747;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp746;});_tmp745;});break;case 291: _LL3D8: yyval=(
void*)({struct Cyc_PatternList_tok_struct*_tmp74B=_cycalloc(sizeof(*_tmp74B));
_tmp74B[0]=({struct Cyc_PatternList_tok_struct _tmp74C;_tmp74C.tag=Cyc_PatternList_tok;
_tmp74C.f1=0;_tmp74C;});_tmp74B;});break;case 292: _LL3D9: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D[0]=({struct Cyc_PatternList_tok_struct
_tmp74E;_tmp74E.tag=Cyc_PatternList_tok;_tmp74E.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(yyvs[
yyvsp_offset]));_tmp74E;});_tmp74D;});break;case 293: _LL3DA: yyval=(void*)({struct
Cyc_PatternList_tok_struct*_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F[0]=({
struct Cyc_PatternList_tok_struct _tmp750;_tmp750.tag=Cyc_PatternList_tok;_tmp750.f1=({
struct Cyc_List_List*_tmp751=_cycalloc(sizeof(*_tmp751));_tmp751->hd=Cyc_yyget_Pattern_tok(
yyvs[yyvsp_offset]);_tmp751->tl=0;_tmp751;});_tmp750;});_tmp74F;});break;case 294:
_LL3DB: yyval=(void*)({struct Cyc_PatternList_tok_struct*_tmp752=_cycalloc(sizeof(*
_tmp752));_tmp752[0]=({struct Cyc_PatternList_tok_struct _tmp753;_tmp753.tag=Cyc_PatternList_tok;
_tmp753.f1=({struct Cyc_List_List*_tmp754=_cycalloc(sizeof(*_tmp754));_tmp754->hd=
Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmp754->tl=Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp754;});_tmp753;});
_tmp752;});break;case 295: _LL3DC: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp755=_cycalloc(sizeof(*_tmp755));_tmp755[0]=({struct Cyc_FieldPattern_tok_struct
_tmp756;_tmp756.tag=Cyc_FieldPattern_tok;_tmp756.f1=({struct _tuple10*_tmp757=
_cycalloc(sizeof(*_tmp757));_tmp757->f1=0;_tmp757->f2=Cyc_yyget_Pattern_tok(yyvs[
yyvsp_offset]);_tmp757;});_tmp756;});_tmp755;});break;case 296: _LL3DD: yyval=(void*)({
struct Cyc_FieldPattern_tok_struct*_tmp758=_cycalloc(sizeof(*_tmp758));_tmp758[0]=({
struct Cyc_FieldPattern_tok_struct _tmp759;_tmp759.tag=Cyc_FieldPattern_tok;
_tmp759.f1=({struct _tuple10*_tmp75A=_cycalloc(sizeof(*_tmp75A));_tmp75A->f1=Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp75A->f2=Cyc_yyget_Pattern_tok(
yyvs[yyvsp_offset]);_tmp75A;});_tmp759;});_tmp758;});break;case 297: _LL3DE: yyval=(
void*)({struct Cyc_FieldPatternList_tok_struct*_tmp75B=_cycalloc(sizeof(*_tmp75B));
_tmp75B[0]=({struct Cyc_FieldPatternList_tok_struct _tmp75C;_tmp75C.tag=Cyc_FieldPatternList_tok;
_tmp75C.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_FieldPatternList_tok(yyvs[yyvsp_offset]));_tmp75C;});_tmp75B;});break;
case 298: _LL3DF: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*_tmp75D=
_cycalloc(sizeof(*_tmp75D));_tmp75D[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp75E;_tmp75E.tag=Cyc_FieldPatternList_tok;_tmp75E.f1=({struct Cyc_List_List*
_tmp75F=_cycalloc(sizeof(*_tmp75F));_tmp75F->hd=Cyc_yyget_FieldPattern_tok(yyvs[
yyvsp_offset]);_tmp75F->tl=0;_tmp75F;});_tmp75E;});_tmp75D;});break;case 299:
_LL3E0: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*_tmp760=_cycalloc(
sizeof(*_tmp760));_tmp760[0]=({struct Cyc_FieldPatternList_tok_struct _tmp761;
_tmp761.tag=Cyc_FieldPatternList_tok;_tmp761.f1=({struct Cyc_List_List*_tmp762=
_cycalloc(sizeof(*_tmp762));_tmp762->hd=Cyc_yyget_FieldPattern_tok(yyvs[
yyvsp_offset]);_tmp762->tl=Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp762;});_tmp761;});
_tmp760;});break;case 300: _LL3E1: yyval=yyvs[yyvsp_offset];break;case 301: _LL3E2:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp763=_cycalloc(sizeof(*_tmp763));
_tmp763[0]=({struct Cyc_Exp_tok_struct _tmp764;_tmp764.tag=Cyc_Exp_tok;_tmp764.f1=
Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp764;});_tmp763;});break;case 302: _LL3E3: yyval=yyvs[
yyvsp_offset];break;case 303: _LL3E4: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp765=_cycalloc(sizeof(*_tmp765));_tmp765[0]=({struct Cyc_Exp_tok_struct _tmp766;
_tmp766.tag=Cyc_Exp_tok;_tmp766.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp766;});_tmp765;});break;case 304: _LL3E5: yyval=(
void*)({struct Cyc_Primopopt_tok_struct*_tmp767=_cycalloc(sizeof(*_tmp767));
_tmp767[0]=({struct Cyc_Primopopt_tok_struct _tmp768;_tmp768.tag=Cyc_Primopopt_tok;
_tmp768.f1=0;_tmp768;});_tmp767;});break;case 305: _LL3E6: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp769=_cycalloc(sizeof(*_tmp769));_tmp769[0]=({struct Cyc_Primopopt_tok_struct
_tmp76A;_tmp76A.tag=Cyc_Primopopt_tok;_tmp76A.f1=({struct Cyc_Core_Opt*_tmp76B=
_cycalloc(sizeof(*_tmp76B));_tmp76B->v=(void*)((void*)1);_tmp76B;});_tmp76A;});
_tmp769;});break;case 306: _LL3E7: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp76C=_cycalloc(sizeof(*_tmp76C));_tmp76C[0]=({struct Cyc_Primopopt_tok_struct
_tmp76D;_tmp76D.tag=Cyc_Primopopt_tok;_tmp76D.f1=({struct Cyc_Core_Opt*_tmp76E=
_cycalloc(sizeof(*_tmp76E));_tmp76E->v=(void*)((void*)3);_tmp76E;});_tmp76D;});
_tmp76C;});break;case 307: _LL3E8: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp76F=_cycalloc(sizeof(*_tmp76F));_tmp76F[0]=({struct Cyc_Primopopt_tok_struct
_tmp770;_tmp770.tag=Cyc_Primopopt_tok;_tmp770.f1=({struct Cyc_Core_Opt*_tmp771=
_cycalloc(sizeof(*_tmp771));_tmp771->v=(void*)((void*)4);_tmp771;});_tmp770;});
_tmp76F;});break;case 308: _LL3E9: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp772=_cycalloc(sizeof(*_tmp772));_tmp772[0]=({struct Cyc_Primopopt_tok_struct
_tmp773;_tmp773.tag=Cyc_Primopopt_tok;_tmp773.f1=({struct Cyc_Core_Opt*_tmp774=
_cycalloc(sizeof(*_tmp774));_tmp774->v=(void*)((void*)0);_tmp774;});_tmp773;});
_tmp772;});break;case 309: _LL3EA: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp775=_cycalloc(sizeof(*_tmp775));_tmp775[0]=({struct Cyc_Primopopt_tok_struct
_tmp776;_tmp776.tag=Cyc_Primopopt_tok;_tmp776.f1=({struct Cyc_Core_Opt*_tmp777=
_cycalloc(sizeof(*_tmp777));_tmp777->v=(void*)((void*)2);_tmp777;});_tmp776;});
_tmp775;});break;case 310: _LL3EB: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp778=_cycalloc(sizeof(*_tmp778));_tmp778[0]=({struct Cyc_Primopopt_tok_struct
_tmp779;_tmp779.tag=Cyc_Primopopt_tok;_tmp779.f1=({struct Cyc_Core_Opt*_tmp77A=
_cycalloc(sizeof(*_tmp77A));_tmp77A->v=(void*)((void*)16);_tmp77A;});_tmp779;});
_tmp778;});break;case 311: _LL3EC: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp77B=_cycalloc(sizeof(*_tmp77B));_tmp77B[0]=({struct Cyc_Primopopt_tok_struct
_tmp77C;_tmp77C.tag=Cyc_Primopopt_tok;_tmp77C.f1=({struct Cyc_Core_Opt*_tmp77D=
_cycalloc(sizeof(*_tmp77D));_tmp77D->v=(void*)((void*)17);_tmp77D;});_tmp77C;});
_tmp77B;});break;case 312: _LL3ED: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp77E=_cycalloc(sizeof(*_tmp77E));_tmp77E[0]=({struct Cyc_Primopopt_tok_struct
_tmp77F;_tmp77F.tag=Cyc_Primopopt_tok;_tmp77F.f1=({struct Cyc_Core_Opt*_tmp780=
_cycalloc(sizeof(*_tmp780));_tmp780->v=(void*)((void*)13);_tmp780;});_tmp77F;});
_tmp77E;});break;case 313: _LL3EE: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp781=_cycalloc(sizeof(*_tmp781));_tmp781[0]=({struct Cyc_Primopopt_tok_struct
_tmp782;_tmp782.tag=Cyc_Primopopt_tok;_tmp782.f1=({struct Cyc_Core_Opt*_tmp783=
_cycalloc(sizeof(*_tmp783));_tmp783->v=(void*)((void*)15);_tmp783;});_tmp782;});
_tmp781;});break;case 314: _LL3EF: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp784=_cycalloc(sizeof(*_tmp784));_tmp784[0]=({struct Cyc_Primopopt_tok_struct
_tmp785;_tmp785.tag=Cyc_Primopopt_tok;_tmp785.f1=({struct Cyc_Core_Opt*_tmp786=
_cycalloc(sizeof(*_tmp786));_tmp786->v=(void*)((void*)14);_tmp786;});_tmp785;});
_tmp784;});break;case 315: _LL3F0: yyval=yyvs[yyvsp_offset];break;case 316: _LL3F1:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp787=_cycalloc(sizeof(*_tmp787));
_tmp787[0]=({struct Cyc_Exp_tok_struct _tmp788;_tmp788.tag=Cyc_Exp_tok;_tmp788.f1=
Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp788;});_tmp787;});break;case 317: _LL3F2: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp789=_cycalloc(sizeof(*_tmp789));_tmp789[0]=({
struct Cyc_Exp_tok_struct _tmp78A;_tmp78A.tag=Cyc_Exp_tok;_tmp78A.f1=Cyc_Absyn_throw_exp(
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp78A;});_tmp789;});break;case 318: _LL3F3: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp78B=_cycalloc(sizeof(*_tmp78B));_tmp78B[0]=({
struct Cyc_Exp_tok_struct _tmp78C;_tmp78C.tag=Cyc_Exp_tok;_tmp78C.f1=Cyc_Absyn_New_exp(
0,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line));_tmp78C;});
_tmp78B;});break;case 319: _LL3F4: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp78D=
_cycalloc(sizeof(*_tmp78D));_tmp78D[0]=({struct Cyc_Exp_tok_struct _tmp78E;_tmp78E.tag=
Cyc_Exp_tok;_tmp78E.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).last_line));_tmp78E;});_tmp78D;});break;case 320: _LL3F5: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp78F=_cycalloc(sizeof(*_tmp78F));_tmp78F[0]=({
struct Cyc_Exp_tok_struct _tmp790;_tmp790.tag=Cyc_Exp_tok;_tmp790.f1=Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp790;});_tmp78F;});break;case 321: _LL3F6: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp791=_cycalloc(sizeof(*_tmp791));_tmp791[0]=({
struct Cyc_Exp_tok_struct _tmp792;_tmp792.tag=Cyc_Exp_tok;_tmp792.f1=Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp792;});_tmp791;});break;case 322: _LL3F7: yyval=yyvs[
yyvsp_offset];break;case 323: _LL3F8: yyval=yyvs[yyvsp_offset];break;case 324: _LL3F9:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp793=_cycalloc(sizeof(*_tmp793));
_tmp793[0]=({struct Cyc_Exp_tok_struct _tmp794;_tmp794.tag=Cyc_Exp_tok;_tmp794.f1=
Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp794;});_tmp793;});break;case 325: _LL3FA: yyval=yyvs[
yyvsp_offset];break;case 326: _LL3FB: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp795=_cycalloc(sizeof(*_tmp795));_tmp795[0]=({struct Cyc_Exp_tok_struct _tmp796;
_tmp796.tag=Cyc_Exp_tok;_tmp796.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp796;});
_tmp795;});break;case 327: _LL3FC: yyval=yyvs[yyvsp_offset];break;case 328: _LL3FD:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp797=_cycalloc(sizeof(*_tmp797));
_tmp797[0]=({struct Cyc_Exp_tok_struct _tmp798;_tmp798.tag=Cyc_Exp_tok;_tmp798.f1=
Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp798;});
_tmp797;});break;case 329: _LL3FE: yyval=yyvs[yyvsp_offset];break;case 330: _LL3FF:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp799=_cycalloc(sizeof(*_tmp799));
_tmp799[0]=({struct Cyc_Exp_tok_struct _tmp79A;_tmp79A.tag=Cyc_Exp_tok;_tmp79A.f1=
Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp79A;});
_tmp799;});break;case 331: _LL400: yyval=yyvs[yyvsp_offset];break;case 332: _LL401:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp79B=_cycalloc(sizeof(*_tmp79B));
_tmp79B[0]=({struct Cyc_Exp_tok_struct _tmp79C;_tmp79C.tag=Cyc_Exp_tok;_tmp79C.f1=
Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp79C;});
_tmp79B;});break;case 333: _LL402: yyval=yyvs[yyvsp_offset];break;case 334: _LL403:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp79D=_cycalloc(sizeof(*_tmp79D));
_tmp79D[0]=({struct Cyc_Exp_tok_struct _tmp79E;_tmp79E.tag=Cyc_Exp_tok;_tmp79E.f1=
Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp79E;});_tmp79D;});break;case 335: _LL404: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp79F=_cycalloc(sizeof(*_tmp79F));_tmp79F[0]=({
struct Cyc_Exp_tok_struct _tmp7A0;_tmp7A0.tag=Cyc_Exp_tok;_tmp7A0.f1=Cyc_Absyn_neq_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7A0;});_tmp79F;});break;case 336: _LL405: yyval=yyvs[
yyvsp_offset];break;case 337: _LL406: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7A1=_cycalloc(sizeof(*_tmp7A1));_tmp7A1[0]=({struct Cyc_Exp_tok_struct _tmp7A2;
_tmp7A2.tag=Cyc_Exp_tok;_tmp7A2.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A2;});
_tmp7A1;});break;case 338: _LL407: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A3=
_cycalloc(sizeof(*_tmp7A3));_tmp7A3[0]=({struct Cyc_Exp_tok_struct _tmp7A4;_tmp7A4.tag=
Cyc_Exp_tok;_tmp7A4.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A4;});
_tmp7A3;});break;case 339: _LL408: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A5=
_cycalloc(sizeof(*_tmp7A5));_tmp7A5[0]=({struct Cyc_Exp_tok_struct _tmp7A6;_tmp7A6.tag=
Cyc_Exp_tok;_tmp7A6.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A6;});
_tmp7A5;});break;case 340: _LL409: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A7=
_cycalloc(sizeof(*_tmp7A7));_tmp7A7[0]=({struct Cyc_Exp_tok_struct _tmp7A8;_tmp7A8.tag=
Cyc_Exp_tok;_tmp7A8.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7A8;});
_tmp7A7;});break;case 341: _LL40A: yyval=yyvs[yyvsp_offset];break;case 342: _LL40B:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A9=_cycalloc(sizeof(*_tmp7A9));
_tmp7A9[0]=({struct Cyc_Exp_tok_struct _tmp7AA;_tmp7AA.tag=Cyc_Exp_tok;_tmp7AA.f1=
Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7AA;});
_tmp7A9;});break;case 343: _LL40C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7AB=
_cycalloc(sizeof(*_tmp7AB));_tmp7AB[0]=({struct Cyc_Exp_tok_struct _tmp7AC;_tmp7AC.tag=
Cyc_Exp_tok;_tmp7AC.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7AC;});
_tmp7AB;});break;case 344: _LL40D: yyval=yyvs[yyvsp_offset];break;case 345: _LL40E:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7AD=_cycalloc(sizeof(*_tmp7AD));
_tmp7AD[0]=({struct Cyc_Exp_tok_struct _tmp7AE;_tmp7AE.tag=Cyc_Exp_tok;_tmp7AE.f1=
Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7AE;});
_tmp7AD;});break;case 346: _LL40F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7AF=
_cycalloc(sizeof(*_tmp7AF));_tmp7AF[0]=({struct Cyc_Exp_tok_struct _tmp7B0;_tmp7B0.tag=
Cyc_Exp_tok;_tmp7B0.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B0;});
_tmp7AF;});break;case 347: _LL410: yyval=yyvs[yyvsp_offset];break;case 348: _LL411:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B1=_cycalloc(sizeof(*_tmp7B1));
_tmp7B1[0]=({struct Cyc_Exp_tok_struct _tmp7B2;_tmp7B2.tag=Cyc_Exp_tok;_tmp7B2.f1=
Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B2;});
_tmp7B1;});break;case 349: _LL412: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B3=
_cycalloc(sizeof(*_tmp7B3));_tmp7B3[0]=({struct Cyc_Exp_tok_struct _tmp7B4;_tmp7B4.tag=
Cyc_Exp_tok;_tmp7B4.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B4;});
_tmp7B3;});break;case 350: _LL413: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B5=
_cycalloc(sizeof(*_tmp7B5));_tmp7B5[0]=({struct Cyc_Exp_tok_struct _tmp7B6;_tmp7B6.tag=
Cyc_Exp_tok;_tmp7B6.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B6;});
_tmp7B5;});break;case 351: _LL414: yyval=yyvs[yyvsp_offset];break;case 352: _LL415:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));
_tmp7B7[0]=({struct Cyc_Exp_tok_struct _tmp7B8;_tmp7B8.tag=Cyc_Exp_tok;_tmp7B8.f1=
Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7B8;});_tmp7B7;});break;case 353: _LL416: yyval=yyvs[
yyvsp_offset];break;case 354: _LL417: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7B9=_cycalloc(sizeof(*_tmp7B9));_tmp7B9[0]=({struct Cyc_Exp_tok_struct _tmp7BA;
_tmp7BA.tag=Cyc_Exp_tok;_tmp7BA.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7BA;});
_tmp7B9;});break;case 355: _LL418: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7BB=
_cycalloc(sizeof(*_tmp7BB));_tmp7BB[0]=({struct Cyc_Exp_tok_struct _tmp7BC;_tmp7BC.tag=
Cyc_Exp_tok;_tmp7BC.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7BC;});
_tmp7BB;});break;case 356: _LL419: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7BD=
_cycalloc(sizeof(*_tmp7BD));_tmp7BD[0]=({struct Cyc_Exp_tok_struct _tmp7BE;_tmp7BE.tag=
Cyc_Exp_tok;_tmp7BE.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7BE;});
_tmp7BD;});break;case 357: _LL41A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7BF=
_cycalloc(sizeof(*_tmp7BF));_tmp7BF[0]=({struct Cyc_Exp_tok_struct _tmp7C0;_tmp7C0.tag=
Cyc_Exp_tok;_tmp7C0.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7C0;});
_tmp7BF;});break;case 358: _LL41B: yyval=yyvs[yyvsp_offset];break;case 359: _LL41C:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7C1=_cycalloc(sizeof(*_tmp7C1));
_tmp7C1[0]=({struct Cyc_Exp_tok_struct _tmp7C2;_tmp7C2.tag=Cyc_Exp_tok;_tmp7C2.f1=
Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C2;});_tmp7C1;});break;case 360: _LL41D: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7C3=_cycalloc(sizeof(*_tmp7C3));_tmp7C3[0]=({
struct Cyc_Exp_tok_struct _tmp7C4;_tmp7C4.tag=Cyc_Exp_tok;_tmp7C4.f1=Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)])).f3,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7C4;});
_tmp7C3;});break;case 361: _LL41E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7C5=
_cycalloc(sizeof(*_tmp7C5));_tmp7C5[0]=({struct Cyc_Exp_tok_struct _tmp7C6;_tmp7C6.tag=
Cyc_Exp_tok;_tmp7C6.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7C6;});
_tmp7C5;});break;case 362: _LL41F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7C7=
_cycalloc(sizeof(*_tmp7C7));_tmp7C7[0]=({struct Cyc_Exp_tok_struct _tmp7C8;_tmp7C8.tag=
Cyc_Exp_tok;_tmp7C8.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp7C9=_cycalloc(sizeof(*_tmp7C9));_tmp7C9[0]=({struct Cyc_Absyn_StructField_struct
_tmp7CA;_tmp7CA.tag=0;_tmp7CA.f1=({struct _tagged_arr*_tmp7CB=_cycalloc(sizeof(*
_tmp7CB));_tmp7CB[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp7CB;});_tmp7CA;});_tmp7C9;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C8;});_tmp7C7;});break;case 363: _LL420: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7CC=_cycalloc(sizeof(*_tmp7CC));_tmp7CC[0]=({
struct Cyc_Exp_tok_struct _tmp7CD;_tmp7CD.tag=Cyc_Exp_tok;_tmp7CD.f1=Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp7CE=_cycalloc_atomic(
sizeof(*_tmp7CE));_tmp7CE[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp7CF;_tmp7CF.tag=
1;_tmp7CF.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp7CF;});_tmp7CE;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7CD;});
_tmp7CC;});break;case 364: _LL421: {struct Cyc_Position_Segment*_tmp7D0=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);struct Cyc_List_List*_tmp7D1=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp7D0,Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_Exp_tok_struct*_tmp7D2=_cycalloc(sizeof(*_tmp7D2));_tmp7D2[0]=({struct
Cyc_Exp_tok_struct _tmp7D3;_tmp7D3.tag=Cyc_Exp_tok;_tmp7D3.f1=Cyc_Absyn_gentyp_exp(
_tmp7D1,(*Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7D3;});_tmp7D2;});break;}case 365: _LL422: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7D4=_cycalloc(sizeof(*_tmp7D4));_tmp7D4[0]=({
struct Cyc_Exp_tok_struct _tmp7D5;_tmp7D5.tag=Cyc_Exp_tok;_tmp7D5.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7D6=_cycalloc(sizeof(*_tmp7D6));
_tmp7D6[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7D7;_tmp7D7.tag=33;_tmp7D7.f1=({
struct Cyc_Absyn_MallocInfo _tmp7D8;_tmp7D8.is_calloc=0;_tmp7D8.rgn=0;_tmp7D8.elt_type=
0;_tmp7D8.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp7D8.fat_result=0;_tmp7D8;});_tmp7D7;});_tmp7D6;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7D5;});_tmp7D4;});break;case 366: _LL423: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7D9=_cycalloc(sizeof(*_tmp7D9));_tmp7D9[0]=({
struct Cyc_Exp_tok_struct _tmp7DA;_tmp7DA.tag=Cyc_Exp_tok;_tmp7DA.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7DB=_cycalloc(sizeof(*_tmp7DB));
_tmp7DB[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7DC;_tmp7DC.tag=33;_tmp7DC.f1=({
struct Cyc_Absyn_MallocInfo _tmp7DD;_tmp7DD.is_calloc=0;_tmp7DD.rgn=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp7DD.elt_type=0;_tmp7DD.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7DD.fat_result=0;
_tmp7DD;});_tmp7DC;});_tmp7DB;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7DA;});_tmp7D9;});break;case 367: _LL424: {void*
_tmp7DF;struct _tuple2 _tmp7DE=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_LL427: _tmp7DF=_tmp7DE.f3;
goto _LL426;_LL426: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7E0=_cycalloc(
sizeof(*_tmp7E0));_tmp7E0[0]=({struct Cyc_Exp_tok_struct _tmp7E1;_tmp7E1.tag=Cyc_Exp_tok;
_tmp7E1.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7E2=
_cycalloc(sizeof(*_tmp7E2));_tmp7E2[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7E3;
_tmp7E3.tag=33;_tmp7E3.f1=({struct Cyc_Absyn_MallocInfo _tmp7E4;_tmp7E4.is_calloc=
1;_tmp7E4.rgn=0;_tmp7E4.elt_type=({void**_tmp7E5=_cycalloc(sizeof(*_tmp7E5));
_tmp7E5[0]=_tmp7DF;_tmp7E5;});_tmp7E4.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp7E4.fat_result=0;
_tmp7E4;});_tmp7E3;});_tmp7E2;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7E1;});_tmp7E0;});break;}case 368: _LL425: {void*
_tmp7E7;struct _tuple2 _tmp7E6=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_LL42A: _tmp7E7=_tmp7E6.f3;
goto _LL429;_LL429: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7E8=_cycalloc(
sizeof(*_tmp7E8));_tmp7E8[0]=({struct Cyc_Exp_tok_struct _tmp7E9;_tmp7E9.tag=Cyc_Exp_tok;
_tmp7E9.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7EA=
_cycalloc(sizeof(*_tmp7EA));_tmp7EA[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7EB;
_tmp7EB.tag=33;_tmp7EB.f1=({struct Cyc_Absyn_MallocInfo _tmp7EC;_tmp7EC.is_calloc=
1;_tmp7EC.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp7EC.elt_type=({void**
_tmp7ED=_cycalloc(sizeof(*_tmp7ED));_tmp7ED[0]=_tmp7E7;_tmp7ED;});_tmp7EC.num_elts=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp7EC.fat_result=0;_tmp7EC;});_tmp7EB;});_tmp7EA;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7E9;});_tmp7E8;});break;}case 369: _LL428: yyval=(
void*)({struct Cyc_Primop_tok_struct*_tmp7EE=_cycalloc(sizeof(*_tmp7EE));_tmp7EE[
0]=({struct Cyc_Primop_tok_struct _tmp7EF;_tmp7EF.tag=Cyc_Primop_tok;_tmp7EF.f1=(
void*)((void*)12);_tmp7EF;});_tmp7EE;});break;case 370: _LL42B: yyval=(void*)({
struct Cyc_Primop_tok_struct*_tmp7F0=_cycalloc(sizeof(*_tmp7F0));_tmp7F0[0]=({
struct Cyc_Primop_tok_struct _tmp7F1;_tmp7F1.tag=Cyc_Primop_tok;_tmp7F1.f1=(void*)((
void*)11);_tmp7F1;});_tmp7F0;});break;case 371: _LL42C: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp7F2=_cycalloc(sizeof(*_tmp7F2));_tmp7F2[0]=({struct Cyc_Primop_tok_struct
_tmp7F3;_tmp7F3.tag=Cyc_Primop_tok;_tmp7F3.f1=(void*)((void*)2);_tmp7F3;});
_tmp7F2;});break;case 372: _LL42D: yyval=yyvs[yyvsp_offset];break;case 373: _LL42E:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7F4=_cycalloc(sizeof(*_tmp7F4));
_tmp7F4[0]=({struct Cyc_Exp_tok_struct _tmp7F5;_tmp7F5.tag=Cyc_Exp_tok;_tmp7F5.f1=
Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F5;});_tmp7F4;});break;case 374: _LL42F: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7F6=_cycalloc(sizeof(*_tmp7F6));_tmp7F6[0]=({
struct Cyc_Exp_tok_struct _tmp7F7;_tmp7F7.tag=Cyc_Exp_tok;_tmp7F7.f1=Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7F7;});
_tmp7F6;});break;case 375: _LL430: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7F8=
_cycalloc(sizeof(*_tmp7F8));_tmp7F8[0]=({struct Cyc_Exp_tok_struct _tmp7F9;_tmp7F9.tag=
Cyc_Exp_tok;_tmp7F9.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F9;});_tmp7F8;});break;case 376: _LL431: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7FA=_cycalloc(sizeof(*_tmp7FA));_tmp7FA[0]=({
struct Cyc_Exp_tok_struct _tmp7FB;_tmp7FB.tag=Cyc_Exp_tok;_tmp7FB.f1=Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
struct _tagged_arr*_tmp7FC=_cycalloc(sizeof(*_tmp7FC));_tmp7FC[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp7FC;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7FB;});_tmp7FA;});break;case 377: _LL432: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7FD=_cycalloc(sizeof(*_tmp7FD));_tmp7FD[0]=({
struct Cyc_Exp_tok_struct _tmp7FE;_tmp7FE.tag=Cyc_Exp_tok;_tmp7FE.f1=Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
struct _tagged_arr*_tmp7FF=_cycalloc(sizeof(*_tmp7FF));_tmp7FF[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp7FF;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7FE;});_tmp7FD;});break;case 378: _LL433: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp800=_cycalloc(sizeof(*_tmp800));_tmp800[0]=({
struct Cyc_Exp_tok_struct _tmp801;_tmp801.tag=Cyc_Exp_tok;_tmp801.f1=Cyc_Absyn_post_inc_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp801;});
_tmp800;});break;case 379: _LL434: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp802=
_cycalloc(sizeof(*_tmp802));_tmp802[0]=({struct Cyc_Exp_tok_struct _tmp803;_tmp803.tag=
Cyc_Exp_tok;_tmp803.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp803;});_tmp802;});break;case 380: _LL435: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp804=_cycalloc(sizeof(*_tmp804));_tmp804[0]=({
struct Cyc_Exp_tok_struct _tmp805;_tmp805.tag=Cyc_Exp_tok;_tmp805.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp806=_cycalloc(sizeof(*_tmp806));
_tmp806[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp807;_tmp807.tag=25;_tmp807.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
4)]);_tmp807.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp807;});_tmp806;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp805;});_tmp804;});break;case 381: _LL436: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp808=_cycalloc(sizeof(*_tmp808));_tmp808[0]=({
struct Cyc_Exp_tok_struct _tmp809;_tmp809.tag=Cyc_Exp_tok;_tmp809.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp80A=_cycalloc(sizeof(*_tmp80A));
_tmp80A[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp80B;_tmp80B.tag=25;_tmp80B.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmp80B.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp80B;});_tmp80A;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp809;});_tmp808;});break;case 382: _LL437: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp80C=_cycalloc(sizeof(*_tmp80C));_tmp80C[0]=({
struct Cyc_Exp_tok_struct _tmp80D;_tmp80D.tag=Cyc_Exp_tok;_tmp80D.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Fill_e_struct*_tmp80E=_cycalloc(sizeof(*_tmp80E));
_tmp80E[0]=({struct Cyc_Absyn_Fill_e_struct _tmp80F;_tmp80F.tag=37;_tmp80F.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp80F;});_tmp80E;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp80D;});
_tmp80C;});break;case 383: _LL438: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp810=
_cycalloc(sizeof(*_tmp810));_tmp810[0]=({struct Cyc_Exp_tok_struct _tmp811;_tmp811.tag=
Cyc_Exp_tok;_tmp811.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Codegen_e_struct*
_tmp812=_cycalloc(sizeof(*_tmp812));_tmp812[0]=({struct Cyc_Absyn_Codegen_e_struct
_tmp813;_tmp813.tag=36;_tmp813.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp813;});_tmp812;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp811;});
_tmp810;});break;case 384: _LL439: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp814=
_cycalloc(sizeof(*_tmp814));_tmp814[0]=({struct Cyc_Exp_tok_struct _tmp815;_tmp815.tag=
Cyc_Exp_tok;_tmp815.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp817;_tmp817.tag=2;_tmp817.f1=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);
_tmp817;});_tmp816;}),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp815;});_tmp814;});break;case 385: _LL43A: yyval=
yyvs[yyvsp_offset];break;case 386: _LL43B: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp818=_cycalloc(sizeof(*_tmp818));_tmp818[0]=({struct Cyc_Exp_tok_struct _tmp819;
_tmp819.tag=Cyc_Exp_tok;_tmp819.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp819;});_tmp818;});break;case 387: _LL43C: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 388: _LL43D:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp81A=_cycalloc(sizeof(*_tmp81A));
_tmp81A[0]=({struct Cyc_Exp_tok_struct _tmp81B;_tmp81B.tag=Cyc_Exp_tok;_tmp81B.f1=
Cyc_Absyn_noinstantiate_exp(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp81D;_tmp81D.tag=2;_tmp81D.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp81D;});_tmp81C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp81B;});_tmp81A;});break;case 389: _LL43E: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E[0]=({
struct Cyc_Exp_tok_struct _tmp81F;_tmp81F.tag=Cyc_Exp_tok;_tmp81F.f1=Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp820=_cycalloc(
sizeof(*_tmp820));_tmp820[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp821;_tmp821.tag=
2;_tmp821.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp821;});_tmp820;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp81F;});
_tmp81E;});break;case 390: _LL43F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp822=
_cycalloc(sizeof(*_tmp822));_tmp822[0]=({struct Cyc_Exp_tok_struct _tmp823;_tmp823.tag=
Cyc_Exp_tok;_tmp823.f1=Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp823;});_tmp822;});break;case 391: _LL440: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp824=_cycalloc(sizeof(*_tmp824));_tmp824[0]=({
struct Cyc_Exp_tok_struct _tmp825;_tmp825.tag=Cyc_Exp_tok;_tmp825.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Struct_e_struct*_tmp826=_cycalloc(sizeof(*_tmp826));
_tmp826[0]=({struct Cyc_Absyn_Struct_e_struct _tmp827;_tmp827.tag=28;_tmp827.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp827.f2=0;_tmp827.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp827.f4=0;_tmp827;});_tmp826;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp825;});_tmp824;});break;case 392: _LL441: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp828=_cycalloc(sizeof(*_tmp828));_tmp828[0]=({
struct Cyc_Exp_tok_struct _tmp829;_tmp829.tag=Cyc_Exp_tok;_tmp829.f1=Cyc_Absyn_stmt_exp(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp829;});
_tmp828;});break;case 393: _LL442: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp82A=_cycalloc(sizeof(*_tmp82A));_tmp82A[0]=({struct Cyc_ExpList_tok_struct
_tmp82B;_tmp82B.tag=Cyc_ExpList_tok;_tmp82B.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(yyvs[yyvsp_offset]));
_tmp82B;});_tmp82A;});break;case 394: _LL443: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp82C=_cycalloc(sizeof(*_tmp82C));_tmp82C[0]=({struct Cyc_ExpList_tok_struct
_tmp82D;_tmp82D.tag=Cyc_ExpList_tok;_tmp82D.f1=({struct Cyc_List_List*_tmp82E=
_cycalloc(sizeof(*_tmp82E));_tmp82E->hd=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);
_tmp82E->tl=0;_tmp82E;});_tmp82D;});_tmp82C;});break;case 395: _LL444: yyval=(void*)({
struct Cyc_ExpList_tok_struct*_tmp82F=_cycalloc(sizeof(*_tmp82F));_tmp82F[0]=({
struct Cyc_ExpList_tok_struct _tmp830;_tmp830.tag=Cyc_ExpList_tok;_tmp830.f1=({
struct Cyc_List_List*_tmp831=_cycalloc(sizeof(*_tmp831));_tmp831->hd=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp831->tl=Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp831;});_tmp830;});
_tmp82F;});break;case 396: _LL445: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp832=
_cycalloc(sizeof(*_tmp832));_tmp832[0]=({struct Cyc_Exp_tok_struct _tmp833;_tmp833.tag=
Cyc_Exp_tok;_tmp833.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f1,(*
Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp833;});_tmp832;});
break;case 397: _LL446: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp834=_cycalloc(
sizeof(*_tmp834));_tmp834[0]=({struct Cyc_Exp_tok_struct _tmp835;_tmp835.tag=Cyc_Exp_tok;
_tmp835.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp835;});
_tmp834;});break;case 398: _LL447: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp836=
_cycalloc(sizeof(*_tmp836));_tmp836[0]=({struct Cyc_Exp_tok_struct _tmp837;_tmp837.tag=
Cyc_Exp_tok;_tmp837.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));_tmp837;});_tmp836;});break;case 399: _LL448: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp838=_cycalloc(sizeof(*_tmp838));_tmp838[0]=({
struct Cyc_Exp_tok_struct _tmp839;_tmp839.tag=Cyc_Exp_tok;_tmp839.f1=Cyc_Absyn_null_exp(
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmp839;});_tmp838;});break;case 400: _LL449: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp83A=_cycalloc(sizeof(*_tmp83A));_tmp83A[0]=({struct Cyc_QualId_tok_struct
_tmp83B;_tmp83B.tag=Cyc_QualId_tok;_tmp83B.f1=({struct _tuple1*_tmp83C=_cycalloc(
sizeof(*_tmp83C));_tmp83C->f1=Cyc_Absyn_rel_ns_null;_tmp83C->f2=({struct
_tagged_arr*_tmp83D=_cycalloc(sizeof(*_tmp83D));_tmp83D[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp83D;});_tmp83C;});_tmp83B;});_tmp83A;});break;case 401:
_LL44A: yyval=yyvs[yyvsp_offset];break;case 402: _LL44B: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp83E=_cycalloc(sizeof(*_tmp83E));_tmp83E[0]=({struct Cyc_QualId_tok_struct
_tmp83F;_tmp83F.tag=Cyc_QualId_tok;_tmp83F.f1=({struct _tuple1*_tmp840=_cycalloc(
sizeof(*_tmp840));_tmp840->f1=Cyc_Absyn_rel_ns_null;_tmp840->f2=({struct
_tagged_arr*_tmp841=_cycalloc(sizeof(*_tmp841));_tmp841[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp841;});_tmp840;});_tmp83F;});_tmp83E;});break;case 403:
_LL44C: yyval=yyvs[yyvsp_offset];break;case 404: _LL44D: yyval=yyvs[yyvsp_offset];
break;case 405: _LL44E: yyval=yyvs[yyvsp_offset];break;case 406: _LL44F: yyval=yyvs[
yyvsp_offset];break;case 407: _LL450: yyval=yyvs[yyvsp_offset];break;case 408: _LL451:
break;case 409: _LL452:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;break;default: _LL453: break;}
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
_check_known_subscript_notnull(116,yyn - 126)]+ yyss[yyssp_offset];if((yystate >= 
0? yystate <= 5129: 0)? Cyc_yycheck[yystate]== yyss[yyssp_offset]: 0){yystate=(int)
Cyc_yytable[yystate];}else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(116,yyn - 126)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[yystate];if(yyn > - 32768? yyn < 
5129: 0){int sze=0;struct _tagged_arr msg;int x;int count;count=0;for(x=yyn < 0? - yyn: 0;
x < 242 / sizeof(unsigned char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(5130,x + yyn)]== x){(sze +=Cyc_Std_strlen(Cyc_yytname[
x])+ 15,count ++);}}msg=({unsigned int _tmp842=(unsigned int)(sze + 15);
unsigned char*_tmp843=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmp842));struct _tagged_arr _tmp845=_tag_arr(_tmp843,sizeof(
unsigned char),(unsigned int)(sze + 15));{unsigned int _tmp844=_tmp842;
unsigned int i;for(i=0;i < _tmp844;i ++){_tmp843[i]='\000';}};_tmp845;});Cyc_Std_strcpy(
msg,_tag_arr("parse error",sizeof(unsigned char),12));if(count < 5){count=0;for(x=
yyn < 0? - yyn: 0;x < 242 / sizeof(unsigned char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(5130,x + yyn)]== x){Cyc_Std_strcat(msg,count == 0?
_tag_arr(", expecting `",sizeof(unsigned char),14): _tag_arr(" or `",sizeof(
unsigned char),6));Cyc_Std_strcat(msg,Cyc_yytname[x]);Cyc_Std_strcat(msg,
_tag_arr("'",sizeof(unsigned char),2));count ++;}}}Cyc_yyerror((struct _tagged_arr)
msg);}else{Cyc_yyerror(_tag_arr("parse error",sizeof(unsigned char),12));}}goto
yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){return 1;}Cyc_yychar=-
2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0){
return 1;}yyvsp_offset --;yystate=(int)yyss[_check_known_subscript_notnull(10000,
-- yyssp_offset)];yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[yystate];if(yyn
== - 32768){goto yyerrdefault;}yyn +=1;if((yyn < 0? 1: yyn > 5129)? 1: Cyc_yycheck[yyn]!= 
1){goto yyerrdefault;}yyn=(int)Cyc_yytable[yyn];if(yyn < 0){if(yyn == - 32768){goto
yyerrpop;}yyn=- yyn;goto yyreduce;}else{if(yyn == 0){goto yyerrpop;}}if(yyn == 827){
return 0;}yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;
yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto
yynewstate;}void Cyc_yyprint(int i,void*v){void*_tmp84F=v;struct _tuple12*_tmp850;
struct _tuple12 _tmp851;int _tmp852;unsigned char _tmp853;short _tmp854;struct
_tagged_arr _tmp855;struct Cyc_Core_Opt*_tmp856;struct Cyc_Core_Opt*_tmp857;struct
Cyc_Core_Opt _tmp858;struct _tagged_arr*_tmp859;struct _tuple1*_tmp85A;struct
_tuple1 _tmp85B;struct _tagged_arr*_tmp85C;void*_tmp85D;_LL456: if(*((void**)
_tmp84F)== Cyc_Int_tok){_LL466: _tmp850=((struct Cyc_Int_tok_struct*)_tmp84F)->f1;
_tmp851=*_tmp850;_LL467: _tmp852=_tmp851.f2;goto _LL457;}else{goto _LL458;}_LL458:
if(*((void**)_tmp84F)== Cyc_Char_tok){_LL468: _tmp853=((struct Cyc_Char_tok_struct*)
_tmp84F)->f1;goto _LL459;}else{goto _LL45A;}_LL45A: if(*((void**)_tmp84F)== Cyc_Short_tok){
_LL469: _tmp854=((struct Cyc_Short_tok_struct*)_tmp84F)->f1;goto _LL45B;}else{goto
_LL45C;}_LL45C: if(*((void**)_tmp84F)== Cyc_String_tok){_LL46A: _tmp855=((struct
Cyc_String_tok_struct*)_tmp84F)->f1;goto _LL45D;}else{goto _LL45E;}_LL45E: if(*((
void**)_tmp84F)== Cyc_Stringopt_tok){_LL46B: _tmp856=((struct Cyc_Stringopt_tok_struct*)
_tmp84F)->f1;if(_tmp856 == 0){goto _LL45F;}else{goto _LL460;}}else{goto _LL460;}
_LL460: if(*((void**)_tmp84F)== Cyc_Stringopt_tok){_LL46C: _tmp857=((struct Cyc_Stringopt_tok_struct*)
_tmp84F)->f1;if(_tmp857 == 0){goto _LL462;}else{_tmp858=*_tmp857;_LL46D: _tmp859=(
struct _tagged_arr*)_tmp858.v;goto _LL461;}}else{goto _LL462;}_LL462: if(*((void**)
_tmp84F)== Cyc_QualId_tok){_LL46E: _tmp85A=((struct Cyc_QualId_tok_struct*)_tmp84F)->f1;
_tmp85B=*_tmp85A;_LL470: _tmp85D=_tmp85B.f1;goto _LL46F;_LL46F: _tmp85C=_tmp85B.f2;
goto _LL463;}else{goto _LL464;}_LL464: goto _LL465;_LL457:({struct Cyc_Std_Int_pa_struct
_tmp85F;_tmp85F.tag=1;_tmp85F.f1=(int)((unsigned int)_tmp852);{void*_tmp85E[1]={&
_tmp85F};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%d",sizeof(unsigned char),3),
_tag_arr(_tmp85E,sizeof(void*),1));}});goto _LL455;_LL459:({struct Cyc_Std_Int_pa_struct
_tmp861;_tmp861.tag=1;_tmp861.f1=(int)((unsigned int)((int)_tmp853));{void*
_tmp860[1]={& _tmp861};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%c",sizeof(
unsigned char),3),_tag_arr(_tmp860,sizeof(void*),1));}});goto _LL455;_LL45B:({
struct Cyc_Std_Int_pa_struct _tmp863;_tmp863.tag=1;_tmp863.f1=(int)((unsigned int)((
int)_tmp854));{void*_tmp862[1]={& _tmp863};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%ds",sizeof(unsigned char),4),_tag_arr(_tmp862,sizeof(void*),1));}});
goto _LL455;_LL45D:({struct Cyc_Std_String_pa_struct _tmp865;_tmp865.tag=0;_tmp865.f1=(
struct _tagged_arr)_tmp855;{void*_tmp864[1]={& _tmp865};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\"%s\"",sizeof(unsigned char),5),_tag_arr(_tmp864,sizeof(void*),1));}});
goto _LL455;_LL45F:({void*_tmp866[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("NULL",
sizeof(unsigned char),5),_tag_arr(_tmp866,sizeof(void*),0));});goto _LL455;_LL461:({
struct Cyc_Std_String_pa_struct _tmp868;_tmp868.tag=0;_tmp868.f1=(struct
_tagged_arr)*_tmp859;{void*_tmp867[1]={& _tmp868};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\"%s\"",sizeof(unsigned char),5),_tag_arr(_tmp867,sizeof(void*),1));}});
goto _LL455;_LL463: {struct Cyc_List_List*_tmp869=0;{void*_tmp86A=_tmp85D;struct
Cyc_List_List*_tmp86B;struct Cyc_List_List*_tmp86C;_LL472: if((unsigned int)
_tmp86A > 1?*((int*)_tmp86A)== 0: 0){_LL478: _tmp86B=((struct Cyc_Absyn_Rel_n_struct*)
_tmp86A)->f1;goto _LL473;}else{goto _LL474;}_LL474: if((unsigned int)_tmp86A > 1?*((
int*)_tmp86A)== 1: 0){_LL479: _tmp86C=((struct Cyc_Absyn_Abs_n_struct*)_tmp86A)->f1;
goto _LL475;}else{goto _LL476;}_LL476: if((int)_tmp86A == 0){goto _LL477;}else{goto
_LL471;}_LL473: _tmp869=_tmp86B;goto _LL471;_LL475: _tmp869=_tmp86C;goto _LL471;
_LL477: goto _LL471;_LL471:;}for(0;_tmp869 != 0;_tmp869=_tmp869->tl){({struct Cyc_Std_String_pa_struct
_tmp86E;_tmp86E.tag=0;_tmp86E.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp869->hd);{void*_tmp86D[1]={& _tmp86E};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",
sizeof(unsigned char),5),_tag_arr(_tmp86D,sizeof(void*),1));}});}({struct Cyc_Std_String_pa_struct
_tmp870;_tmp870.tag=0;_tmp870.f1=(struct _tagged_arr)*_tmp85C;{void*_tmp86F[1]={&
_tmp870};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",sizeof(unsigned char),5),
_tag_arr(_tmp86F,sizeof(void*),1));}});goto _LL455;}_LL465:({void*_tmp871[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("?",sizeof(unsigned char),2),_tag_arr(
_tmp871,sizeof(void*),0));});goto _LL455;_LL455:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc_Std___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*
_tmp872=_cycalloc(sizeof(*_tmp872));_tmp872->v=Cyc_Lexing_from_file(f);_tmp872;});
Cyc_yyparse();return Cyc_Parse_parse_result;}
