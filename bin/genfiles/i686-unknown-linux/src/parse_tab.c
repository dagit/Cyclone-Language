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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Cstdio___abstractFILE;struct
Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr);struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);struct _tagged_arr Cyc_Std_vrprintf(struct
_RegionHandle*r1,struct _tagged_arr fmt,struct _tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct
Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file(struct Cyc_Std___cycFILE*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void*Cyc_List_fold_right(
void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*
x);extern char Cyc_List_Nth[8];struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);unsigned int Cyc_Std_strlen(
struct _tagged_arr s);int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);
int Cyc_Std_zstrcmp(struct _tagged_arr,struct _tagged_arr);int Cyc_Std_zstrptrcmp(
struct _tagged_arr*,struct _tagged_arr*);struct _tagged_arr Cyc_Std_strcat(struct
_tagged_arr dest,struct _tagged_arr src);struct _tagged_arr Cyc_Std_strcpy(struct
_tagged_arr dest,struct _tagged_arr src);struct _tagged_arr Cyc_Std_substring(struct
_tagged_arr,int ofs,unsigned int n);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct Cyc_Position_Segment*Cyc_Position_segment_join(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};struct Cyc_Position_Error*
Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*,struct _tagged_arr);extern
char Cyc_Position_Nocontext[14];void Cyc_Position_post_error(struct Cyc_Position_Error*);
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
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct
Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{
int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct Cyc_Absyn_NonNullable_ps_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Nullable_ps_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{
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
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
exist_vars;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;struct
Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[
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
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
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
void*a);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple4{void*f1;void*f2;};struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Tcenv_VarRes_struct{int tag;void*
f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*
f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict*
aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct
Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*
availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;};struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{int tag;void*f1;void*f2;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*
le;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct
_tagged_arr ap);void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_compress(void*t);
void Cyc_Lex_register_typedef(struct _tuple1*s);void Cyc_Lex_enter_namespace(struct
_tagged_arr*);void Cyc_Lex_leave_namespace();void Cyc_Lex_enter_using(struct
_tuple1*);void Cyc_Lex_leave_using();struct Cyc_Parse_Signed_spec_struct{int tag;
struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Unsigned_spec_struct{int tag;
struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Short_spec_struct{int tag;struct
Cyc_Position_Segment*f1;};struct Cyc_Parse_Long_spec_struct{int tag;struct Cyc_Position_Segment*
f1;};struct Cyc_Parse_Type_spec_struct{int tag;void*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Parse_Decl_spec_struct{int tag;struct Cyc_Absyn_Decl*f1;};struct Cyc_Parse_Declaration_spec{
struct Cyc_Core_Opt*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int
is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Declarator{struct
_tuple1*id;struct Cyc_List_List*tms;};struct Cyc_Parse_Abstractdeclarator{struct
Cyc_List_List*tms;};struct _tuple5{void*f1;struct Cyc_Core_Opt*f2;};static struct
_tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc);struct _tuple6{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct
Cyc_List_List*f4;};static struct _tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,
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
sizeof(char),46),_tag_arr(_tmp0,sizeof(void*),2));}}});return;}static int Cyc_Parse_enum_counter=
0;struct _tuple1*Cyc_Parse_gensym_enum(){return({struct _tuple1*_tmp3=_cycalloc(
sizeof(*_tmp3));_tmp3->f1=(void*)({struct Cyc_Absyn_Rel_n_struct*_tmp7=_cycalloc(
sizeof(*_tmp7));_tmp7[0]=({struct Cyc_Absyn_Rel_n_struct _tmp8;_tmp8.tag=0;_tmp8.f1=
0;_tmp8;});_tmp7;});_tmp3->f2=({struct _tagged_arr*_tmp4=_cycalloc(sizeof(*_tmp4));
_tmp4[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp6;_tmp6.tag=1;_tmp6.f1=(
int)((unsigned int)Cyc_Parse_enum_counter ++);{void*_tmp5[1]={& _tmp6};Cyc_Std_aprintf(
_tag_arr("__anonymous_enum_%d__",sizeof(char),22),_tag_arr(_tmp5,sizeof(void*),1));}});
_tmp4;});_tmp3;});}struct _tuple7{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*
f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple8{struct _tuple7*
f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple8*field_info){struct _tuple8 _tmpA;
struct _tuple7*_tmpB;struct _tuple7 _tmpC;struct _tuple1*_tmpD;struct Cyc_Absyn_Tqual
_tmpE;void*_tmpF;struct Cyc_List_List*_tmp10;struct Cyc_List_List*_tmp11;struct Cyc_Absyn_Exp*
_tmp12;struct _tuple8*_tmp9=field_info;_tmpA=*_tmp9;_tmpB=_tmpA.f1;_tmpC=*_tmpB;
_tmpD=_tmpC.f1;_tmpE=_tmpC.f2;_tmpF=_tmpC.f3;_tmp10=_tmpC.f4;_tmp11=_tmpC.f5;
_tmp12=_tmpA.f2;if(_tmp10 != 0)Cyc_Parse_err(_tag_arr("bad type params in struct field",
sizeof(char),32),loc);if(Cyc_Absyn_is_qvar_qualified(_tmpD))Cyc_Parse_err(
_tag_arr("struct or union field cannot be qualified with a namespace",sizeof(
char),59),loc);return({struct Cyc_Absyn_Aggrfield*_tmp13=_cycalloc(sizeof(*_tmp13));
_tmp13->name=(*_tmpD).f2;_tmp13->tq=_tmpE;_tmp13->type=(void*)_tmpF;_tmp13->width=
_tmp12;_tmp13->attributes=_tmp11;_tmp13;});}static void*Cyc_Parse_type_spec(void*
t,struct Cyc_Position_Segment*loc){return(void*)({struct Cyc_Parse_Type_spec_struct*
_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14[0]=({struct Cyc_Parse_Type_spec_struct
_tmp15;_tmp15.tag=4;_tmp15.f1=(void*)t;_tmp15.f2=loc;_tmp15;});_tmp14;});}static
void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp16=t;void*_tmp17;struct Cyc_Absyn_Tqual
_tmp18;struct Cyc_Absyn_Exp*_tmp19;_LL1: if(_tmp16 <= (void*)3?1:*((int*)_tmp16)!= 
7)goto _LL3;_tmp17=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp16)->f1;_tmp18=((
struct Cyc_Absyn_ArrayType_struct*)_tmp16)->f2;_tmp19=((struct Cyc_Absyn_ArrayType_struct*)
_tmp16)->f3;_LL2: return Cyc_Absyn_starb_typ(_tmp17,argposn?Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->v=(void*)((void*)3);
_tmp1A;}),0):(void*)2,_tmp18,_tmp19 == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp19);_tmp1C;});
_tmp1B;}));_LL3:;_LL4: return t;_LL0:;}static void Cyc_Parse_arg_array2ptr(struct
_tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*x).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual
f1;void*f2;};static struct _tuple9*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*
loc,struct _tuple2*t){return({struct _tuple9*_tmp1D=_cycalloc(sizeof(*_tmp1D));
_tmp1D->f1=(*t).f2;_tmp1D->f2=(*t).f3;_tmp1D;});}static struct Cyc_Absyn_Vardecl*
Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){void*_tmp1E=(void*)d->r;struct Cyc_Absyn_Vardecl*
_tmp1F;_LL6: if(*((int*)_tmp1E)!= 0)goto _LL8;_tmp1F=((struct Cyc_Absyn_Var_d_struct*)
_tmp1E)->f1;_LL7: return _tmp1F;_LL8:;_LL9:({void*_tmp20[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(d->loc,_tag_arr("bad declaration in `forarray' statement",
sizeof(char),40),_tag_arr(_tmp20,sizeof(void*),0));});_LL5:;}static int Cyc_Parse_is_typeparam(
void*tm){void*_tmp21=tm;_LLB: if(_tmp21 <= (void*)1?1:*((int*)_tmp21)!= 3)goto _LLD;
_LLC: return 1;_LLD:;_LLE: return 0;_LLA:;}static void*Cyc_Parse_id2type(struct
_tagged_arr s,void*k){if(Cyc_Std_zstrcmp(s,_tag_arr("`H",sizeof(char),3))== 0)
return(void*)2;else{return(void*)({struct Cyc_Absyn_VarType_struct*_tmp22=
_cycalloc(sizeof(*_tmp22));_tmp22[0]=({struct Cyc_Absyn_VarType_struct _tmp23;
_tmp23.tag=1;_tmp23.f1=({struct Cyc_Absyn_Tvar*_tmp24=_cycalloc(sizeof(*_tmp24));
_tmp24->name=({struct _tagged_arr*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25[0]=s;
_tmp25;});_tmp24->identity=0;_tmp24->kind=(void*)k;_tmp24;});_tmp23;});_tmp22;});}}
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){void*k;{
void*_tmp26=Cyc_Absyn_compress_kb((void*)t->kind);void*_tmp27;void*_tmp28;_LL10:
if(*((int*)_tmp26)!= 0)goto _LL12;_tmp27=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
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
sizeof(char),46),_tag_arr(_tmp32,sizeof(void*),0));});_LL16:;}static void*Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar*pr){return(void*)({struct Cyc_Absyn_VarType_struct*_tmp33=
_cycalloc(sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Absyn_VarType_struct _tmp34;
_tmp34.tag=1;_tmp34.f1=pr;_tmp34;});_tmp33;});}static void Cyc_Parse_set_vartyp_kind(
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
sizeof(char),93),_tag_arr(_tmp41,sizeof(void*),0));});return tms;_LL2D: if(*((int*)
_tmp3F)!= 0)goto _LL2A;_tmp40=((struct Cyc_Absyn_NoTypes_struct*)_tmp3F)->f1;_LL2E:
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp40)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(tds))({void*_tmp42[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,_tag_arr("wrong number of parameter declarations in old-style function declaration",
sizeof(char),73),_tag_arr(_tmp42,sizeof(void*),0));});{struct Cyc_List_List*
_tmp43=0;for(0;_tmp40 != 0;_tmp40=_tmp40->tl){struct Cyc_List_List*_tmp44=tds;for(
0;_tmp44 != 0;_tmp44=_tmp44->tl){struct Cyc_Absyn_Decl*_tmp45=(struct Cyc_Absyn_Decl*)
_tmp44->hd;void*_tmp46=(void*)_tmp45->r;struct Cyc_Absyn_Vardecl*_tmp47;_LL30: if(*((
int*)_tmp46)!= 0)goto _LL32;_tmp47=((struct Cyc_Absyn_Var_d_struct*)_tmp46)->f1;
_LL31: if(Cyc_Std_zstrptrcmp((*_tmp47->name).f2,(struct _tagged_arr*)_tmp40->hd)!= 
0)continue;if(_tmp47->initializer != 0)({void*_tmp48[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp45->loc,
_tag_arr("initializer found in parameter declaration",sizeof(char),43),_tag_arr(
_tmp48,sizeof(void*),0));});if(Cyc_Absyn_is_qvar_qualified(_tmp47->name))({void*
_tmp49[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Parse_abort)(_tmp45->loc,_tag_arr("namespaces forbidden in parameter declarations",
sizeof(char),47),_tag_arr(_tmp49,sizeof(void*),0));});_tmp43=({struct Cyc_List_List*
_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->hd=({struct _tuple2*_tmp4B=_cycalloc(
sizeof(*_tmp4B));_tmp4B->f1=({struct Cyc_Core_Opt*_tmp4C=_cycalloc(sizeof(*_tmp4C));
_tmp4C->v=(*_tmp47->name).f2;_tmp4C;});_tmp4B->f2=_tmp47->tq;_tmp4B->f3=(void*)
_tmp47->type;_tmp4B;});_tmp4A->tl=_tmp43;_tmp4A;});goto L;_LL32:;_LL33:({void*
_tmp4D[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Parse_abort)(_tmp45->loc,_tag_arr("nonvariable declaration in parameter type",
sizeof(char),42),_tag_arr(_tmp4D,sizeof(void*),0));});_LL2F:;}L: if(_tmp44 == 0)({
struct Cyc_Std_String_pa_struct _tmp4F;_tmp4F.tag=0;_tmp4F.f1=(struct _tagged_arr)*((
struct _tagged_arr*)_tmp40->hd);{void*_tmp4E[1]={& _tmp4F};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,_tag_arr("%s is not given a type",
sizeof(char),23),_tag_arr(_tmp4E,sizeof(void*),1));}});}return({struct Cyc_List_List*
_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp52;_tmp52.tag=2;_tmp52.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp54;_tmp54.tag=1;_tmp54.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp43);_tmp54.f2=0;_tmp54.f3=0;_tmp54.f4=0;_tmp54.f5=0;_tmp54;});
_tmp53;}));_tmp52;});_tmp51;}));_tmp50->tl=0;_tmp50;});}_LL2A:;}goto _LL29;_LL28:;
_LL29: return({struct Cyc_List_List*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->hd=(
void*)((void*)tms->hd);_tmp55->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);
_tmp55;});_LL25:;}}struct _tuple10{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct Cyc_Core_Opt*
dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*
body,struct Cyc_Position_Segment*loc){if(tds != 0)d=({struct Cyc_Parse_Declarator*
_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->id=d->id;_tmp56->tms=Cyc_Parse_oldstyle2newstyle(
d->tms,tds,loc);_tmp56;});{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual();int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp57=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL35: if((int)_tmp57 != 1)goto _LL37;
_LL36: sc=(void*)3;goto _LL34;_LL37: if((int)_tmp57 != 3)goto _LL39;_LL38: sc=(void*)0;
goto _LL34;_LL39:;_LL3A: Cyc_Parse_err(_tag_arr("bad storage class on function",
sizeof(char),30),loc);goto _LL34;_LL34:;}}{void*_tmp59;struct Cyc_Core_Opt*_tmp5A;
struct _tuple5 _tmp58=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmp59=_tmp58.f1;
_tmp5A=_tmp58.f2;{struct Cyc_Absyn_Tqual _tmp5C;void*_tmp5D;struct Cyc_List_List*
_tmp5E;struct Cyc_List_List*_tmp5F;struct _tuple6 _tmp5B=Cyc_Parse_apply_tms(tq,
_tmp59,atts,d->tms);_tmp5C=_tmp5B.f1;_tmp5D=_tmp5B.f2;_tmp5E=_tmp5B.f3;_tmp5F=
_tmp5B.f4;if(_tmp5A != 0)({void*_tmp60[0]={};Cyc_Tcutil_warn(loc,_tag_arr("nested type declaration within function prototype",
sizeof(char),50),_tag_arr(_tmp60,sizeof(void*),0));});if(_tmp5E != 0)({void*
_tmp61[0]={};Cyc_Tcutil_warn(loc,_tag_arr("bad type params, ignoring",sizeof(
char),26),_tag_arr(_tmp61,sizeof(void*),0));});{void*_tmp62=_tmp5D;struct Cyc_Absyn_FnInfo
_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_Core_Opt*_tmp65;void*_tmp66;struct
Cyc_List_List*_tmp67;int _tmp68;struct Cyc_Absyn_VarargInfo*_tmp69;struct Cyc_List_List*
_tmp6A;struct Cyc_List_List*_tmp6B;_LL3C: if(_tmp62 <= (void*)3?1:*((int*)_tmp62)!= 
8)goto _LL3E;_tmp63=((struct Cyc_Absyn_FnType_struct*)_tmp62)->f1;_tmp64=_tmp63.tvars;
_tmp65=_tmp63.effect;_tmp66=(void*)_tmp63.ret_typ;_tmp67=_tmp63.args;_tmp68=
_tmp63.c_varargs;_tmp69=_tmp63.cyc_varargs;_tmp6A=_tmp63.rgn_po;_tmp6B=_tmp63.attributes;
_LL3D: {struct Cyc_List_List*_tmp6C=0;{struct Cyc_List_List*_tmp6D=_tmp67;for(0;
_tmp6D != 0;_tmp6D=_tmp6D->tl){struct _tuple2 _tmp6F;struct Cyc_Core_Opt*_tmp70;
struct Cyc_Absyn_Tqual _tmp71;void*_tmp72;struct _tuple2*_tmp6E=(struct _tuple2*)
_tmp6D->hd;_tmp6F=*_tmp6E;_tmp70=_tmp6F.f1;_tmp71=_tmp6F.f2;_tmp72=_tmp6F.f3;if(
_tmp70 == 0){Cyc_Parse_err(_tag_arr("missing argument variable in function prototype",
sizeof(char),48),loc);_tmp6C=({struct Cyc_List_List*_tmp73=_cycalloc(sizeof(*
_tmp73));_tmp73->hd=({struct _tuple10*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->f1=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"?",sizeof(char),2);_tmp74->f2=
_tmp71;_tmp74->f3=_tmp72;_tmp74;});_tmp73->tl=_tmp6C;_tmp73;});}else{_tmp6C=({
struct Cyc_List_List*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->hd=({struct
_tuple10*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->f1=(struct _tagged_arr*)_tmp70->v;
_tmp77->f2=_tmp71;_tmp77->f3=_tmp72;_tmp77;});_tmp76->tl=_tmp6C;_tmp76;});}}}
return({struct Cyc_Absyn_Fndecl*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->sc=(
void*)sc;_tmp78->name=d->id;_tmp78->tvs=_tmp64;_tmp78->is_inline=is_inline;
_tmp78->effect=_tmp65;_tmp78->ret_type=(void*)_tmp66;_tmp78->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6C);_tmp78->c_varargs=_tmp68;_tmp78->cyc_varargs=
_tmp69;_tmp78->rgn_po=_tmp6A;_tmp78->body=body;_tmp78->cached_typ=0;_tmp78->param_vardecls=
0;_tmp78->attributes=Cyc_List_append(_tmp6B,_tmp5F);_tmp78;});}_LL3E:;_LL3F:({
void*_tmp79[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Parse_abort)(loc,_tag_arr("declarator is not a function prototype",
sizeof(char),39),_tag_arr(_tmp79,sizeof(void*),0));});_LL3B:;}}}}}static char
_tmp7A[52]="at most one type may appear within a type specifier";static struct
_tagged_arr Cyc_Parse_msg1={_tmp7A,_tmp7A,_tmp7A + 52};static char _tmp7B[63]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={_tmp7B,_tmp7B,_tmp7B + 63};static char
_tmp7C[50]="type specifier includes more than one declaration";static struct
_tagged_arr Cyc_Parse_msg3={_tmp7C,_tmp7C,_tmp7C + 50};static char _tmp7D[60]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={_tmp7D,_tmp7D,_tmp7D + 60};static struct
_tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
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
sizeof(char),39),_tmp81);last_loc=_tmp81;seen_sign=1;sgn=(void*)0;goto _LL40;
_LL45: if(*((int*)_tmp7E)!= 1)goto _LL47;_tmp82=((struct Cyc_Parse_Unsigned_spec_struct*)
_tmp7E)->f1;_LL46: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp82);if(seen_type)
Cyc_Parse_err(_tag_arr("signed qualifier must come before type",sizeof(char),39),
_tmp82);last_loc=_tmp82;seen_sign=1;sgn=(void*)1;goto _LL40;_LL47: if(*((int*)
_tmp7E)!= 2)goto _LL49;_tmp83=((struct Cyc_Parse_Short_spec_struct*)_tmp7E)->f1;
_LL48: if(seen_size)Cyc_Parse_err(_tag_arr("integral size may appear only once within a type specifier",
sizeof(char),59),_tmp83);if(seen_type)Cyc_Parse_err(_tag_arr("short modifier must come before base type",
sizeof(char),42),_tmp83);last_loc=_tmp83;seen_size=1;sz=(void*)1;goto _LL40;_LL49:
if(*((int*)_tmp7E)!= 3)goto _LL4B;_tmp84=((struct Cyc_Parse_Long_spec_struct*)
_tmp7E)->f1;_LL4A: if(seen_type)Cyc_Parse_err(_tag_arr("long modifier must come before base type",
sizeof(char),41),_tmp84);if(seen_size){void*_tmp86=sz;_LL4E: if((int)_tmp86 != 2)
goto _LL50;_LL4F: sz=(void*)3;goto _LL4D;_LL50:;_LL51: Cyc_Parse_err(_tag_arr("extra long in type specifier",
sizeof(char),29),_tmp84);goto _LL4D;_LL4D:;}last_loc=_tmp84;seen_size=1;goto _LL40;
_LL4B: if(*((int*)_tmp7E)!= 5)goto _LL40;_tmp85=((struct Cyc_Parse_Decl_spec_struct*)
_tmp7E)->f1;_LL4C: last_loc=_tmp85->loc;if(declopt == 0?!seen_type: 0){seen_type=1;{
void*_tmp87=(void*)_tmp85->r;struct Cyc_Absyn_Aggrdecl*_tmp88;struct Cyc_Absyn_Tuniondecl*
_tmp89;struct Cyc_Absyn_Enumdecl*_tmp8A;_LL53: if(*((int*)_tmp87)!= 4)goto _LL55;
_tmp88=((struct Cyc_Absyn_Aggr_d_struct*)_tmp87)->f1;_LL54: {struct Cyc_List_List*
_tmp8B=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmp88->tvs));t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmp8C=_cycalloc(
sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Absyn_AggrType_struct _tmp8D;_tmp8D.tag=10;
_tmp8D.f1=({struct Cyc_Absyn_AggrInfo _tmp8E;_tmp8E.aggr_info=(void*)((void*)({
struct Cyc_Absyn_UnknownAggr_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({
struct Cyc_Absyn_UnknownAggr_struct _tmp90;_tmp90.tag=0;_tmp90.f1=(void*)((void*)
_tmp88->kind);_tmp90.f2=_tmp88->name;_tmp90;});_tmp8F;}));_tmp8E.targs=_tmp8B;
_tmp8E;});_tmp8D;});_tmp8C;});if(_tmp88->fields != 0)declopt=({struct Cyc_Core_Opt*
_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->v=_tmp85;_tmp91;});goto _LL52;}_LL55:
if(*((int*)_tmp87)!= 5)goto _LL57;_tmp89=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp87)->f1;_LL56: {struct Cyc_List_List*_tmp92=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmp89->tvs));t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[0]=({
struct Cyc_Absyn_TunionType_struct _tmp94;_tmp94.tag=2;_tmp94.f1=({struct Cyc_Absyn_TunionInfo
_tmp95;_tmp95.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp97;_tmp97.tag=1;_tmp97.f1=({struct Cyc_Absyn_Tuniondecl**_tmp98=_cycalloc(
sizeof(*_tmp98));_tmp98[0]=_tmp89;_tmp98;});_tmp97;});_tmp96;}));_tmp95.targs=
_tmp92;_tmp95.rgn=(void*)((void*)2);_tmp95;});_tmp94;});_tmp93;});if(_tmp89->fields
!= 0)declopt=({struct Cyc_Core_Opt*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->v=
_tmp85;_tmp99;});goto _LL52;}_LL57: if(*((int*)_tmp87)!= 6)goto _LL59;_tmp8A=((
struct Cyc_Absyn_Enum_d_struct*)_tmp87)->f1;_LL58: t=(void*)({struct Cyc_Absyn_EnumType_struct*
_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_Absyn_EnumType_struct
_tmp9B;_tmp9B.tag=12;_tmp9B.f1=_tmp8A->name;_tmp9B.f2=0;_tmp9B;});_tmp9A;});
declopt=({struct Cyc_Core_Opt*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->v=_tmp85;
_tmp9C;});goto _LL52;_LL59:;_LL5A:({void*_tmp9D[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp85->loc,
_tag_arr("bad declaration within type specifier",sizeof(char),38),_tag_arr(
_tmp9D,sizeof(void*),0));});_LL52:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmp85->loc);}
goto _LL40;_LL40:;}if(!seen_type){if(!seen_sign?!seen_size: 0)({void*_tmp9E[0]={};
Cyc_Tcutil_warn(last_loc,_tag_arr("missing type within specifier",sizeof(char),
30),_tag_arr(_tmp9E,sizeof(void*),0));});t=(void*)({struct Cyc_Absyn_IntType_struct*
_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({struct Cyc_Absyn_IntType_struct
_tmpA0;_tmpA0.tag=5;_tmpA0.f1=(void*)sgn;_tmpA0.f2=(void*)sz;_tmpA0;});_tmp9F;});}
else{if(seen_sign){void*_tmpA1=t;void*_tmpA2;_LL5C: if(_tmpA1 <= (void*)3?1:*((int*)
_tmpA1)!= 5)goto _LL5E;_tmpA2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpA1)->f2;
_LL5D: t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));
_tmpA3[0]=({struct Cyc_Absyn_IntType_struct _tmpA4;_tmpA4.tag=5;_tmpA4.f1=(void*)
sgn;_tmpA4.f2=(void*)_tmpA2;_tmpA4;});_tmpA3;});goto _LL5B;_LL5E:;_LL5F: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type",sizeof(char),40),last_loc);
goto _LL5B;_LL5B:;}if(seen_size){void*_tmpA5=t;void*_tmpA6;_LL61: if(_tmpA5 <= (
void*)3?1:*((int*)_tmpA5)!= 5)goto _LL63;_tmpA6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpA5)->f1;_LL62: t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpA7=_cycalloc(
sizeof(*_tmpA7));_tmpA7[0]=({struct Cyc_Absyn_IntType_struct _tmpA8;_tmpA8.tag=5;
_tmpA8.f1=(void*)_tmpA6;_tmpA8.f2=(void*)sz;_tmpA8;});_tmpA7;});goto _LL60;_LL63:
if(_tmpA5 <= (void*)3?1:*((int*)_tmpA5)!= 6)goto _LL65;_LL64: t=(void*)({struct Cyc_Absyn_DoubleType_struct*
_tmpA9=_cycalloc_atomic(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpAA;_tmpAA.tag=6;_tmpAA.f1=1;_tmpAA;});_tmpA9;});goto _LL60;_LL65:;_LL66: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type",sizeof(char),36),last_loc);goto
_LL60;_LL60:;}}return({struct _tuple5 _tmpAB;_tmpAB.f1=t;_tmpAB.f2=declopt;_tmpAB;});}
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct Cyc_Absyn_Tqual tq,void*t,
struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{
struct Cyc_Parse_Declarator*_tmpAC=(struct Cyc_Parse_Declarator*)ds->hd;struct
_tuple1*_tmpAD=_tmpAC->id;struct Cyc_Absyn_Tqual _tmpAF;void*_tmpB0;struct Cyc_List_List*
_tmpB1;struct Cyc_List_List*_tmpB2;struct _tuple6 _tmpAE=Cyc_Parse_apply_tms(tq,t,
shared_atts,_tmpAC->tms);_tmpAF=_tmpAE.f1;_tmpB0=_tmpAE.f2;_tmpB1=_tmpAE.f3;
_tmpB2=_tmpAE.f4;return({struct Cyc_List_List*_tmpB3=_cycalloc(sizeof(*_tmpB3));
_tmpB3->hd=({struct _tuple7*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->f1=_tmpAD;
_tmpB4->f2=_tmpAF;_tmpB4->f3=_tmpB0;_tmpB4->f4=_tmpB1;_tmpB4->f5=_tmpB2;_tmpB4;});
_tmpB3->tl=Cyc_Parse_apply_tmss(_tmpAF,Cyc_Tcutil_copy_type(t),ds->tl,
shared_atts);_tmpB3;});}}static struct _tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){if(tms == 0)return({
struct _tuple6 _tmpB5;_tmpB5.f1=tq;_tmpB5.f2=t;_tmpB5.f3=0;_tmpB5.f4=atts;_tmpB5;});{
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
sizeof(char),45),_tag_arr(_tmpCF,sizeof(void*),0));});_LL74:;}_LL6E: if(_tmpB6 <= (
void*)1?1:*((int*)_tmpB6)!= 3)goto _LL70;_tmpB9=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpB6)->f1;_tmpBA=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpB6)->f2;_LL6F:
if(tms->tl == 0)return({struct _tuple6 _tmpD0;_tmpD0.f1=tq;_tmpD0.f2=t;_tmpD0.f3=
_tmpB9;_tmpD0.f4=atts;_tmpD0;});({void*_tmpD1[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmpBA,_tag_arr("type parameters must appear before function arguments in declarator",
sizeof(char),68),_tag_arr(_tmpD1,sizeof(void*),0));});_LL70: if(_tmpB6 <= (void*)1?
1:*((int*)_tmpB6)!= 1)goto _LL72;_tmpBB=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
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
_tmpDB;struct _tuple5 _tmpD9=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmpDA=
_tmpD9.f1;_tmpDB=_tmpD9.f2;if(_tmpDB != 0)({void*_tmpDC[0]={};Cyc_Tcutil_warn(loc,
_tag_arr("ignoring nested type declaration(s) in specifier list",sizeof(char),54),
_tag_arr(_tmpDC,sizeof(void*),0));});return _tmpDA;}static struct Cyc_Absyn_Decl*
Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*loc,struct _tuple7*t){
struct _tuple7 _tmpDE;struct _tuple1*_tmpDF;struct Cyc_Absyn_Tqual _tmpE0;void*_tmpE1;
struct Cyc_List_List*_tmpE2;struct Cyc_List_List*_tmpE3;struct _tuple7*_tmpDD=t;
_tmpDE=*_tmpDD;_tmpDF=_tmpDE.f1;_tmpE0=_tmpDE.f2;_tmpE1=_tmpDE.f3;_tmpE2=_tmpDE.f4;
_tmpE3=_tmpDE.f5;Cyc_Lex_register_typedef(_tmpDF);if(_tmpE3 != 0)Cyc_Parse_err((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpE5;_tmpE5.tag=0;_tmpE5.f1=(
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)_tmpE3->hd);{void*_tmpE4[1]={&
_tmpE5};Cyc_Std_aprintf(_tag_arr("bad attribute %s within typedef",sizeof(char),
32),_tag_arr(_tmpE4,sizeof(void*),1));}}),loc);{struct Cyc_Core_Opt*kind;struct
Cyc_Core_Opt*type;{void*_tmpE6=_tmpE1;struct Cyc_Core_Opt*_tmpE7;_LL86: if(_tmpE6
<= (void*)3?1:*((int*)_tmpE6)!= 0)goto _LL88;_tmpE7=((struct Cyc_Absyn_Evar_struct*)
_tmpE6)->f1;_LL87: type=0;if(_tmpE7 == 0)kind=({struct Cyc_Core_Opt*_tmpE8=
_cycalloc(sizeof(*_tmpE8));_tmpE8->v=(void*)((void*)2);_tmpE8;});else{kind=
_tmpE7;}goto _LL85;_LL88:;_LL89: kind=0;type=({struct Cyc_Core_Opt*_tmpE9=_cycalloc(
sizeof(*_tmpE9));_tmpE9->v=(void*)_tmpE1;_tmpE9;});goto _LL85;_LL85:;}return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Typedef_d_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));
_tmpEA[0]=({struct Cyc_Absyn_Typedef_d_struct _tmpEB;_tmpEB.tag=7;_tmpEB.f1=({
struct Cyc_Absyn_Typedefdecl*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->name=
_tmpDF;_tmpEC->tvs=_tmpE2;_tmpEC->kind=kind;_tmpEC->defn=type;_tmpEC;});_tmpEB;});
_tmpEA;}),loc);}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*
d,struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpEE;_tmpEE.tag=12;_tmpEE.f1=d;_tmpEE.f2=s;_tmpEE;});_tmpED;}),Cyc_Position_segment_join(
d->loc,s->loc));}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*loc){struct Cyc_Parse_Declaration_spec
_tmpF0;struct Cyc_Absyn_Tqual _tmpF1;struct Cyc_List_List*_tmpF2;struct Cyc_List_List*
_tmpF3;struct Cyc_Parse_Declaration_spec*_tmpEF=ds;_tmpF0=*_tmpEF;_tmpF1=_tmpF0.tq;
_tmpF2=_tmpF0.type_specs;_tmpF3=_tmpF0.attributes;if(ds->is_inline)Cyc_Parse_err(
_tag_arr("inline is allowed only on function definitions",sizeof(char),47),loc);
if(_tmpF2 == 0){Cyc_Parse_err(_tag_arr("missing type specifiers in declaration",
sizeof(char),39),loc);return 0;}{void*s=(void*)2;int istypedef=0;if(ds->sc != 0){
void*_tmpF4=(void*)((struct Cyc_Core_Opt*)_check_null(ds->sc))->v;_LL8B: if((int)
_tmpF4 != 0)goto _LL8D;_LL8C: istypedef=1;goto _LL8A;_LL8D: if((int)_tmpF4 != 1)goto
_LL8F;_LL8E: s=(void*)3;goto _LL8A;_LL8F: if((int)_tmpF4 != 2)goto _LL91;_LL90: s=(
void*)4;goto _LL8A;_LL91: if((int)_tmpF4 != 3)goto _LL93;_LL92: s=(void*)0;goto _LL8A;
_LL93: if((int)_tmpF4 != 4)goto _LL95;_LL94: s=(void*)2;goto _LL8A;_LL95: if((int)
_tmpF4 != 5)goto _LL97;_LL96: s=(void*)2;goto _LL8A;_LL97: if((int)_tmpF4 != 6)goto
_LL8A;_LL98: s=(void*)1;goto _LL8A;_LL8A:;}{struct Cyc_List_List*_tmpF6;struct Cyc_List_List*
_tmpF7;struct _tuple0 _tmpF5=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
ids);_tmpF6=_tmpF5.f1;_tmpF7=_tmpF5.f2;{int exps_empty=1;{struct Cyc_List_List*es=
_tmpF7;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){exps_empty=0;
break;}}}{struct _tuple5 _tmpF8=Cyc_Parse_collapse_type_specifiers(_tmpF2,loc);if(
_tmpF6 == 0){void*_tmpFA;struct Cyc_Core_Opt*_tmpFB;struct _tuple5 _tmpF9=_tmpF8;
_tmpFA=_tmpF9.f1;_tmpFB=_tmpF9.f2;if(_tmpFB != 0){struct Cyc_Absyn_Decl*d=(struct
Cyc_Absyn_Decl*)_tmpFB->v;{void*_tmpFC=(void*)d->r;struct Cyc_Absyn_Enumdecl*
_tmpFD;struct Cyc_Absyn_Aggrdecl*_tmpFE;struct Cyc_Absyn_Tuniondecl*_tmpFF;_LL9A:
if(*((int*)_tmpFC)!= 6)goto _LL9C;_tmpFD=((struct Cyc_Absyn_Enum_d_struct*)_tmpFC)->f1;
_LL9B:(void*)(_tmpFD->sc=(void*)s);if(_tmpF3 != 0)Cyc_Parse_err(_tag_arr("bad attributes on enum",
sizeof(char),23),loc);goto _LL99;_LL9C: if(*((int*)_tmpFC)!= 4)goto _LL9E;_tmpFE=((
struct Cyc_Absyn_Aggr_d_struct*)_tmpFC)->f1;_LL9D:(void*)(_tmpFE->sc=(void*)s);
_tmpFE->attributes=_tmpF3;goto _LL99;_LL9E: if(*((int*)_tmpFC)!= 5)goto _LLA0;
_tmpFF=((struct Cyc_Absyn_Tunion_d_struct*)_tmpFC)->f1;_LL9F:(void*)(_tmpFF->sc=(
void*)s);if(_tmpF3 != 0)Cyc_Parse_err(_tag_arr("bad attributes on tunion",sizeof(
char),25),loc);goto _LL99;_LLA0:;_LLA1: Cyc_Parse_err(_tag_arr("bad declaration",
sizeof(char),16),loc);return 0;_LL99:;}return({struct Cyc_List_List*_tmp100=
_cycalloc(sizeof(*_tmp100));_tmp100->hd=d;_tmp100->tl=0;_tmp100;});}else{void*
_tmp101=_tmpFA;struct Cyc_Absyn_AggrInfo _tmp102;void*_tmp103;void*_tmp104;struct
_tuple1*_tmp105;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_TunionInfo _tmp107;
void*_tmp108;struct Cyc_Absyn_Tuniondecl**_tmp109;struct Cyc_Absyn_TunionInfo
_tmp10A;void*_tmp10B;struct Cyc_Absyn_UnknownTunionInfo _tmp10C;struct _tuple1*
_tmp10D;int _tmp10E;struct Cyc_List_List*_tmp10F;struct _tuple1*_tmp110;struct Cyc_List_List*
_tmp111;_LLA3: if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 10)goto _LLA5;_tmp102=((
struct Cyc_Absyn_AggrType_struct*)_tmp101)->f1;_tmp103=(void*)_tmp102.aggr_info;
if(*((int*)_tmp103)!= 0)goto _LLA5;_tmp104=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp103)->f1;_tmp105=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp103)->f2;_tmp106=
_tmp102.targs;_LLA4: {struct Cyc_List_List*_tmp112=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp106);
struct Cyc_Absyn_Aggrdecl*_tmp113=({struct Cyc_Absyn_Aggrdecl*_tmp117=_cycalloc(
sizeof(*_tmp117));_tmp117->kind=(void*)_tmp104;_tmp117->sc=(void*)s;_tmp117->name=
_tmp105;_tmp117->tvs=_tmp112;_tmp117->exist_vars=0;_tmp117->fields=0;_tmp117->attributes=
0;_tmp117;});if(_tmpF3 != 0)Cyc_Parse_err(_tag_arr("bad attributes on type declaration",
sizeof(char),35),loc);return({struct Cyc_List_List*_tmp114=_cycalloc(sizeof(*
_tmp114));_tmp114->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp116;_tmp116.tag=4;_tmp116.f1=_tmp113;_tmp116;});_tmp115;}),loc);_tmp114->tl=
0;_tmp114;});}_LLA5: if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 2)goto _LLA7;
_tmp107=((struct Cyc_Absyn_TunionType_struct*)_tmp101)->f1;_tmp108=(void*)_tmp107.tunion_info;
if(*((int*)_tmp108)!= 1)goto _LLA7;_tmp109=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp108)->f1;_LLA6: if(_tmpF3 != 0)Cyc_Parse_err(_tag_arr("bad attributes on tunion",
sizeof(char),25),loc);return({struct Cyc_List_List*_tmp118=_cycalloc(sizeof(*
_tmp118));_tmp118->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp11A;_tmp11A.tag=5;_tmp11A.f1=*_tmp109;_tmp11A;});_tmp119;}),loc);_tmp118->tl=
0;_tmp118;});_LLA7: if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 2)goto _LLA9;
_tmp10A=((struct Cyc_Absyn_TunionType_struct*)_tmp101)->f1;_tmp10B=(void*)_tmp10A.tunion_info;
if(*((int*)_tmp10B)!= 0)goto _LLA9;_tmp10C=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp10B)->f1;_tmp10D=_tmp10C.name;_tmp10E=_tmp10C.is_xtunion;_tmp10F=_tmp10A.targs;
_LLA8: {struct Cyc_List_List*_tmp11B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp10F);struct Cyc_Absyn_Decl*_tmp11C=
Cyc_Absyn_tunion_decl(s,_tmp10D,_tmp11B,0,_tmp10E,loc);if(_tmpF3 != 0)Cyc_Parse_err(
_tag_arr("bad attributes on tunion",sizeof(char),25),loc);return({struct Cyc_List_List*
_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->hd=_tmp11C;_tmp11D->tl=0;_tmp11D;});}
_LLA9: if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 12)goto _LLAB;_tmp110=((struct
Cyc_Absyn_EnumType_struct*)_tmp101)->f1;_LLAA: {struct Cyc_Absyn_Enumdecl*_tmp11E=({
struct Cyc_Absyn_Enumdecl*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->sc=(void*)
s;_tmp123->name=_tmp110;_tmp123->fields=0;_tmp123;});if(_tmpF3 != 0)Cyc_Parse_err(
_tag_arr("bad attributes on enum",sizeof(char),23),loc);return({struct Cyc_List_List*
_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->hd=({struct Cyc_Absyn_Decl*_tmp120=
_cycalloc(sizeof(*_tmp120));_tmp120->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp122;_tmp122.tag=6;_tmp122.f1=_tmp11E;_tmp122;});_tmp121;}));_tmp120->loc=loc;
_tmp120;});_tmp11F->tl=0;_tmp11F;});}_LLAB: if(_tmp101 <= (void*)3?1:*((int*)
_tmp101)!= 13)goto _LLAD;_tmp111=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp101)->f1;
_LLAC: {struct Cyc_Absyn_Enumdecl*_tmp124=({struct Cyc_Absyn_Enumdecl*_tmp129=
_cycalloc(sizeof(*_tmp129));_tmp129->sc=(void*)s;_tmp129->name=Cyc_Parse_gensym_enum();
_tmp129->fields=({struct Cyc_Core_Opt*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->v=
_tmp111;_tmp12A;});_tmp129;});if(_tmpF3 != 0)Cyc_Parse_err(_tag_arr("bad attributes on enum",
sizeof(char),23),loc);return({struct Cyc_List_List*_tmp125=_cycalloc(sizeof(*
_tmp125));_tmp125->hd=({struct Cyc_Absyn_Decl*_tmp126=_cycalloc(sizeof(*_tmp126));
_tmp126->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp127=_cycalloc(
sizeof(*_tmp127));_tmp127[0]=({struct Cyc_Absyn_Enum_d_struct _tmp128;_tmp128.tag=
6;_tmp128.f1=_tmp124;_tmp128;});_tmp127;}));_tmp126->loc=loc;_tmp126;});_tmp125->tl=
0;_tmp125;});}_LLAD:;_LLAE: Cyc_Parse_err(_tag_arr("missing declarator",sizeof(
char),19),loc);return 0;_LLA2:;}}else{void*t=_tmpF8.f1;struct Cyc_List_List*
_tmp12B=Cyc_Parse_apply_tmss(_tmpF1,t,_tmpF6,_tmpF3);if(istypedef){if(!
exps_empty)Cyc_Parse_err(_tag_arr("initializer in typedef declaration",sizeof(
char),35),loc);{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*
f)(struct Cyc_Position_Segment*,struct _tuple7*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp12B);
if(_tmpF8.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(_tmpF8.f2)->v;{
void*_tmp12C=(void*)d->r;struct Cyc_Absyn_Aggrdecl*_tmp12D;struct Cyc_Absyn_Tuniondecl*
_tmp12E;struct Cyc_Absyn_Enumdecl*_tmp12F;_LLB0: if(*((int*)_tmp12C)!= 4)goto _LLB2;
_tmp12D=((struct Cyc_Absyn_Aggr_d_struct*)_tmp12C)->f1;_LLB1:(void*)(_tmp12D->sc=(
void*)s);_tmp12D->attributes=_tmpF3;_tmpF3=0;goto _LLAF;_LLB2: if(*((int*)_tmp12C)
!= 5)goto _LLB4;_tmp12E=((struct Cyc_Absyn_Tunion_d_struct*)_tmp12C)->f1;_LLB3:(
void*)(_tmp12E->sc=(void*)s);goto _LLAF;_LLB4: if(*((int*)_tmp12C)!= 6)goto _LLB6;
_tmp12F=((struct Cyc_Absyn_Enum_d_struct*)_tmp12C)->f1;_LLB5:(void*)(_tmp12F->sc=(
void*)s);goto _LLAF;_LLB6:;_LLB7: Cyc_Parse_err(_tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof(char),70),loc);goto _LLAF;_LLAF:;}decls=({struct Cyc_List_List*_tmp130=
_cycalloc(sizeof(*_tmp130));_tmp130->hd=d;_tmp130->tl=decls;_tmp130;});}if(
_tmpF3 != 0)Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp132;_tmp132.tag=0;_tmp132.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmpF3->hd);{void*_tmp131[1]={& _tmp132};Cyc_Std_aprintf(_tag_arr("bad attribute %s in typedef",
sizeof(char),28),_tag_arr(_tmp131,sizeof(void*),1));}}),loc);return decls;}}else{
if(_tmpF8.f2 != 0)Cyc_Parse_unimp(_tag_arr("nested type declaration within declarator",
sizeof(char),42),loc);{struct Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp133=
_tmp12B;for(0;_tmp133 != 0;(_tmp133=_tmp133->tl,_tmpF7=_tmpF7->tl)){struct _tuple7
_tmp135;struct _tuple1*_tmp136;struct Cyc_Absyn_Tqual _tmp137;void*_tmp138;struct
Cyc_List_List*_tmp139;struct Cyc_List_List*_tmp13A;struct _tuple7*_tmp134=(struct
_tuple7*)_tmp133->hd;_tmp135=*_tmp134;_tmp136=_tmp135.f1;_tmp137=_tmp135.f2;
_tmp138=_tmp135.f3;_tmp139=_tmp135.f4;_tmp13A=_tmp135.f5;if(_tmp139 != 0)({void*
_tmp13B[0]={};Cyc_Tcutil_warn(loc,_tag_arr("bad type params, ignoring",sizeof(
char),26),_tag_arr(_tmp13B,sizeof(void*),0));});if(_tmpF7 == 0)({void*_tmp13C[0]={};((
int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Parse_abort)(loc,_tag_arr("unexpected NULL in parse!",sizeof(char),26),
_tag_arr(_tmp13C,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp13D=(struct Cyc_Absyn_Exp*)
_tmpF7->hd;struct Cyc_Absyn_Vardecl*_tmp13E=Cyc_Absyn_new_vardecl(_tmp136,_tmp138,
_tmp13D);_tmp13E->tq=_tmp137;(void*)(_tmp13E->sc=(void*)s);_tmp13E->attributes=
_tmp13A;{struct Cyc_Absyn_Decl*_tmp13F=({struct Cyc_Absyn_Decl*_tmp141=_cycalloc(
sizeof(*_tmp141));_tmp141->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142[0]=({struct Cyc_Absyn_Var_d_struct
_tmp143;_tmp143.tag=0;_tmp143.f1=_tmp13E;_tmp143;});_tmp142;}));_tmp141->loc=loc;
_tmp141;});decls=({struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));
_tmp140->hd=_tmp13F;_tmp140->tl=decls;_tmp140;});}}}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);}}}}}}}}static void*Cyc_Parse_id_to_kind(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){if(Cyc_Std_strlen(s)== 1)
switch(*((const char*)_check_unknown_subscript(s,sizeof(char),0))){case 'A': _LLB8:
return(void*)0;case 'M': _LLB9: return(void*)1;case 'B': _LLBA: return(void*)2;case 'R':
_LLBB: return(void*)3;case 'E': _LLBC: return(void*)4;default: _LLBD: break;}Cyc_Parse_err((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp145;_tmp145.tag=0;_tmp145.f1=(
struct _tagged_arr)s;{void*_tmp144[1]={& _tmp145};Cyc_Std_aprintf(_tag_arr("bad kind: %s",
sizeof(char),13),_tag_arr(_tmp144,sizeof(void*),1));}}),loc);return(void*)2;}
static struct Cyc_List_List*Cyc_Parse_attopt_to_tms(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*atts,struct Cyc_List_List*tms){if(atts == 0)return tms;else{
return({struct Cyc_List_List*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->hd=(
void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp147=_cycalloc(sizeof(*
_tmp147));_tmp147[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp148;_tmp148.tag=
4;_tmp148.f1=loc;_tmp148.f2=atts;_tmp148;});_tmp147;}));_tmp146->tl=tms;_tmp146;});}}
char Cyc_AbstractDeclarator_tok[27]="\000\000\000\000AbstractDeclarator_tok";
struct Cyc_AbstractDeclarator_tok_struct{char*tag;struct Cyc_Parse_Abstractdeclarator*
f1;};char Cyc_AggrFieldDeclListList_tok[30]="\000\000\000\000AggrFieldDeclListList_tok";
struct Cyc_AggrFieldDeclListList_tok_struct{char*tag;struct Cyc_List_List*f1;};
char Cyc_AggrFieldDeclList_tok[26]="\000\000\000\000AggrFieldDeclList_tok";struct
Cyc_AggrFieldDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_AttributeList_tok[
22]="\000\000\000\000AttributeList_tok";struct Cyc_AttributeList_tok_struct{char*
tag;struct Cyc_List_List*f1;};char Cyc_Attribute_tok[18]="\000\000\000\000Attribute_tok";
struct Cyc_Attribute_tok_struct{char*tag;void*f1;};char Cyc_Bool_tok[13]="\000\000\000\000Bool_tok";
struct Cyc_Bool_tok_struct{char*tag;int f1;};char Cyc_Char_tok[13]="\000\000\000\000Char_tok";
struct Cyc_Char_tok_struct{char*tag;char f1;};char Cyc_DeclList_tok[17]="\000\000\000\000DeclList_tok";
struct Cyc_DeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_DeclSpec_tok[
17]="\000\000\000\000DeclSpec_tok";struct Cyc_DeclSpec_tok_struct{char*tag;struct
Cyc_Parse_Declaration_spec*f1;};char Cyc_Declarator_tok[19]="\000\000\000\000Declarator_tok";
struct Cyc_Declarator_tok_struct{char*tag;struct Cyc_Parse_Declarator*f1;};char Cyc_DesignatorList_tok[
23]="\000\000\000\000DesignatorList_tok";struct Cyc_DesignatorList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_Designator_tok[19]="\000\000\000\000Designator_tok";
struct Cyc_Designator_tok_struct{char*tag;void*f1;};char Cyc_EnumfieldList_tok[22]="\000\000\000\000EnumfieldList_tok";
struct Cyc_EnumfieldList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Enumfield_tok[
18]="\000\000\000\000Enumfield_tok";struct Cyc_Enumfield_tok_struct{char*tag;
struct Cyc_Absyn_Enumfield*f1;};char Cyc_ExpList_tok[16]="\000\000\000\000ExpList_tok";
struct Cyc_ExpList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Exp_tok[
12]="\000\000\000\000Exp_tok";struct Cyc_Exp_tok_struct{char*tag;struct Cyc_Absyn_Exp*
f1;};char Cyc_FieldPatternList_tok[25]="\000\000\000\000FieldPatternList_tok";
struct Cyc_FieldPatternList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_FieldPattern_tok[
21]="\000\000\000\000FieldPattern_tok";struct _tuple11{struct Cyc_List_List*f1;
struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{char*tag;struct
_tuple11*f1;};char Cyc_FnDecl_tok[15]="\000\000\000\000FnDecl_tok";struct Cyc_FnDecl_tok_struct{
char*tag;struct Cyc_Absyn_Fndecl*f1;};char Cyc_IdList_tok[15]="\000\000\000\000IdList_tok";
struct Cyc_IdList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_InitDeclList_tok[
21]="\000\000\000\000InitDeclList_tok";struct Cyc_InitDeclList_tok_struct{char*
tag;struct Cyc_List_List*f1;};char Cyc_InitDecl_tok[17]="\000\000\000\000InitDecl_tok";
struct _tuple12{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
char*tag;struct _tuple12*f1;};char Cyc_InitializerList_tok[24]="\000\000\000\000InitializerList_tok";
struct Cyc_InitializerList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Int_tok[
12]="\000\000\000\000Int_tok";struct _tuple13{void*f1;int f2;};struct Cyc_Int_tok_struct{
char*tag;struct _tuple13*f1;};char Cyc_Kind_tok[13]="\000\000\000\000Kind_tok";
struct Cyc_Kind_tok_struct{char*tag;void*f1;};char Cyc_ParamDeclListBool_tok[26]="\000\000\000\000ParamDeclListBool_tok";
struct _tuple14{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct
Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_ParamDeclListBool_tok_struct{
char*tag;struct _tuple14*f1;};char Cyc_ParamDeclList_tok[22]="\000\000\000\000ParamDeclList_tok";
struct Cyc_ParamDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_ParamDecl_tok[
18]="\000\000\000\000ParamDecl_tok";struct Cyc_ParamDecl_tok_struct{char*tag;
struct _tuple2*f1;};char Cyc_PatternList_tok[20]="\000\000\000\000PatternList_tok";
struct Cyc_PatternList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Pattern_tok[
16]="\000\000\000\000Pattern_tok";struct Cyc_Pattern_tok_struct{char*tag;struct
Cyc_Absyn_Pat*f1;};char Cyc_Pointer_Sort_tok[21]="\000\000\000\000Pointer_Sort_tok";
struct Cyc_Pointer_Sort_tok_struct{char*tag;void*f1;};char Cyc_Primop_tok[15]="\000\000\000\000Primop_tok";
struct Cyc_Primop_tok_struct{char*tag;void*f1;};char Cyc_Primopopt_tok[18]="\000\000\000\000Primopopt_tok";
struct Cyc_Primopopt_tok_struct{char*tag;struct Cyc_Core_Opt*f1;};char Cyc_QualId_tok[
15]="\000\000\000\000QualId_tok";struct Cyc_QualId_tok_struct{char*tag;struct
_tuple1*f1;};char Cyc_QualSpecList_tok[21]="\000\000\000\000QualSpecList_tok";
struct _tuple15{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct Cyc_QualSpecList_tok_struct{char*tag;struct _tuple15*f1;};char Cyc_Rgnorder_tok[
17]="\000\000\000\000Rgnorder_tok";struct Cyc_Rgnorder_tok_struct{char*tag;struct
Cyc_List_List*f1;};char Cyc_Scope_tok[14]="\000\000\000\000Scope_tok";struct Cyc_Scope_tok_struct{
char*tag;void*f1;};char Cyc_Short_tok[14]="\000\000\000\000Short_tok";struct Cyc_Short_tok_struct{
char*tag;short f1;};char Cyc_Stmt_tok[13]="\000\000\000\000Stmt_tok";struct Cyc_Stmt_tok_struct{
char*tag;struct Cyc_Absyn_Stmt*f1;};char Cyc_StorageClass_tok[21]="\000\000\000\000StorageClass_tok";
struct Cyc_StorageClass_tok_struct{char*tag;void*f1;};char Cyc_String_tok[15]="\000\000\000\000String_tok";
struct Cyc_String_tok_struct{char*tag;struct _tagged_arr f1;};char Cyc_Stringopt_tok[
18]="\000\000\000\000Stringopt_tok";struct Cyc_Stringopt_tok_struct{char*tag;
struct Cyc_Core_Opt*f1;};char Cyc_StructOrUnion_tok[22]="\000\000\000\000StructOrUnion_tok";
struct Cyc_StructOrUnion_tok_struct{char*tag;void*f1;};char Cyc_SwitchCClauseList_tok[
26]="\000\000\000\000SwitchCClauseList_tok";struct Cyc_SwitchCClauseList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_SwitchClauseList_tok[25]="\000\000\000\000SwitchClauseList_tok";
struct Cyc_SwitchClauseList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_TunionFieldList_tok[
24]="\000\000\000\000TunionFieldList_tok";struct Cyc_TunionFieldList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_TunionField_tok[20]="\000\000\000\000TunionField_tok";
struct Cyc_TunionField_tok_struct{char*tag;struct Cyc_Absyn_Tunionfield*f1;};char
Cyc_TypeList_tok[17]="\000\000\000\000TypeList_tok";struct Cyc_TypeList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_TypeModifierList_tok[25]="\000\000\000\000TypeModifierList_tok";
struct Cyc_TypeModifierList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_TypeOpt_tok[
16]="\000\000\000\000TypeOpt_tok";struct Cyc_TypeOpt_tok_struct{char*tag;struct
Cyc_Core_Opt*f1;};char Cyc_TypeQual_tok[17]="\000\000\000\000TypeQual_tok";struct
Cyc_TypeQual_tok_struct{char*tag;struct Cyc_Absyn_Tqual f1;};char Cyc_TypeSpecifier_tok[
22]="\000\000\000\000TypeSpecifier_tok";struct Cyc_TypeSpecifier_tok_struct{char*
tag;void*f1;};char Cyc_Type_tok[13]="\000\000\000\000Type_tok";struct Cyc_Type_tok_struct{
char*tag;void*f1;};char Cyc_YY1[8]="\000\000\000\000YY1";struct Cyc_YY1_struct{
char*tag;struct _tuple14*f1;};char Cyc_YYINITIALSVAL[18]="\000\000\000\000YYINITIALSVAL";
char Cyc_Okay_tok[13]="\000\000\000\000Okay_tok";static char _tmp14A[15]="$(sign_t,int)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp14A,
_tmp14A,_tmp14A + 15}};struct _tuple13*Cyc_yyget_Int_tok(void*yy1){struct _tuple13*
yyzzz;{void*_tmp14B=yy1;struct _tuple13*_tmp14C;_LLC0: if(*((void**)_tmp14B)!= Cyc_Int_tok)
goto _LLC2;_tmp14C=((struct Cyc_Int_tok_struct*)_tmp14B)->f1;_LLC1: yyzzz=_tmp14C;
goto _LLBF;_LLC2:;_LLC3:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLBF:;}return
yyzzz;}static char _tmp14E[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp14E,_tmp14E,_tmp14E + 5}};char Cyc_yyget_Char_tok(void*yy1){
char yyzzz;{void*_tmp14F=yy1;char _tmp150;_LLC5: if(*((void**)_tmp14F)!= Cyc_Char_tok)
goto _LLC7;_tmp150=((struct Cyc_Char_tok_struct*)_tmp14F)->f1;_LLC6: yyzzz=_tmp150;
goto _LLC4;_LLC7:;_LLC8:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLC4:;}return
yyzzz;}static char _tmp152[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp152,_tmp152,_tmp152 + 9}};struct _tagged_arr Cyc_yyget_String_tok(
void*yy1){struct _tagged_arr yyzzz;{void*_tmp153=yy1;struct _tagged_arr _tmp154;
_LLCA: if(*((void**)_tmp153)!= Cyc_String_tok)goto _LLCC;_tmp154=((struct Cyc_String_tok_struct*)
_tmp153)->f1;_LLCB: yyzzz=_tmp154;goto _LLC9;_LLCC:;_LLCD:(int)_throw((void*)& Cyc_yyfail_String_tok);
_LLC9:;}return yyzzz;}static char _tmp156[20]="tunion Pointer_Sort";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={Cyc_Core_Failure,{_tmp156,_tmp156,_tmp156 + 20}};
void*Cyc_yyget_Pointer_Sort_tok(void*yy1){void*yyzzz;{void*_tmp157=yy1;void*
_tmp158;_LLCF: if(*((void**)_tmp157)!= Cyc_Pointer_Sort_tok)goto _LLD1;_tmp158=(
void*)((struct Cyc_Pointer_Sort_tok_struct*)_tmp157)->f1;_LLD0: yyzzz=_tmp158;goto
_LLCE;_LLD1:;_LLD2:(int)_throw((void*)& Cyc_yyfail_Pointer_Sort_tok);_LLCE:;}
return yyzzz;}static char _tmp15A[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{_tmp15A,_tmp15A,_tmp15A + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(
void*yy1){struct Cyc_Absyn_Exp*yyzzz;{void*_tmp15B=yy1;struct Cyc_Absyn_Exp*
_tmp15C;_LLD4: if(*((void**)_tmp15B)!= Cyc_Exp_tok)goto _LLD6;_tmp15C=((struct Cyc_Exp_tok_struct*)
_tmp15B)->f1;_LLD5: yyzzz=_tmp15C;goto _LLD3;_LLD6:;_LLD7:(int)_throw((void*)& Cyc_yyfail_Exp_tok);
_LLD3:;}return yyzzz;}static char _tmp15E[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={Cyc_Core_Failure,{_tmp15E,_tmp15E,_tmp15E + 14}};struct
Cyc_List_List*Cyc_yyget_ExpList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp15F=yy1;struct Cyc_List_List*_tmp160;_LLD9: if(*((void**)_tmp15F)!= Cyc_ExpList_tok)
goto _LLDB;_tmp160=((struct Cyc_ExpList_tok_struct*)_tmp15F)->f1;_LLDA: yyzzz=
_tmp160;goto _LLD8;_LLDB:;_LLDC:(int)_throw((void*)& Cyc_yyfail_ExpList_tok);_LLD8:;}
return yyzzz;}static char _tmp162[39]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{
_tmp162,_tmp162,_tmp162 + 39}};struct Cyc_List_List*Cyc_yyget_InitializerList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp163=yy1;struct Cyc_List_List*
_tmp164;_LLDE: if(*((void**)_tmp163)!= Cyc_InitializerList_tok)goto _LLE0;_tmp164=((
struct Cyc_InitializerList_tok_struct*)_tmp163)->f1;_LLDF: yyzzz=_tmp164;goto _LLDD;
_LLE0:;_LLE1:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);_LLDD:;}return
yyzzz;}static char _tmp166[9]="primop_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Primop_tok={
Cyc_Core_Failure,{_tmp166,_tmp166,_tmp166 + 9}};void*Cyc_yyget_Primop_tok(void*
yy1){void*yyzzz;{void*_tmp167=yy1;void*_tmp168;_LLE3: if(*((void**)_tmp167)!= Cyc_Primop_tok)
goto _LLE5;_tmp168=(void*)((struct Cyc_Primop_tok_struct*)_tmp167)->f1;_LLE4: yyzzz=
_tmp168;goto _LLE2;_LLE5:;_LLE6:(int)_throw((void*)& Cyc_yyfail_Primop_tok);_LLE2:;}
return yyzzz;}static char _tmp16A[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp16A,_tmp16A,_tmp16A + 16}};struct
Cyc_Core_Opt*Cyc_yyget_Primopopt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp16B=yy1;struct Cyc_Core_Opt*_tmp16C;_LLE8: if(*((void**)_tmp16B)!= Cyc_Primopopt_tok)
goto _LLEA;_tmp16C=((struct Cyc_Primopopt_tok_struct*)_tmp16B)->f1;_LLE9: yyzzz=
_tmp16C;goto _LLE7;_LLEA:;_LLEB:(int)_throw((void*)& Cyc_yyfail_Primopopt_tok);
_LLE7:;}return yyzzz;}static char _tmp16E[7]="qvar_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp16E,_tmp16E,_tmp16E + 7}};struct
_tuple1*Cyc_yyget_QualId_tok(void*yy1){struct _tuple1*yyzzz;{void*_tmp16F=yy1;
struct _tuple1*_tmp170;_LLED: if(*((void**)_tmp16F)!= Cyc_QualId_tok)goto _LLEF;
_tmp170=((struct Cyc_QualId_tok_struct*)_tmp16F)->f1;_LLEE: yyzzz=_tmp170;goto
_LLEC;_LLEF:;_LLF0:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LLEC:;}return
yyzzz;}static char _tmp172[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,{_tmp172,_tmp172,_tmp172 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(
void*yy1){struct Cyc_Absyn_Stmt*yyzzz;{void*_tmp173=yy1;struct Cyc_Absyn_Stmt*
_tmp174;_LLF2: if(*((void**)_tmp173)!= Cyc_Stmt_tok)goto _LLF4;_tmp174=((struct Cyc_Stmt_tok_struct*)
_tmp173)->f1;_LLF3: yyzzz=_tmp174;goto _LLF1;_LLF4:;_LLF5:(int)_throw((void*)& Cyc_yyfail_Stmt_tok);
_LLF1:;}return yyzzz;}static char _tmp176[24]="list_t<switch_clause_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={Cyc_Core_Failure,{
_tmp176,_tmp176,_tmp176 + 24}};struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp177=yy1;struct Cyc_List_List*
_tmp178;_LLF7: if(*((void**)_tmp177)!= Cyc_SwitchClauseList_tok)goto _LLF9;_tmp178=((
struct Cyc_SwitchClauseList_tok_struct*)_tmp177)->f1;_LLF8: yyzzz=_tmp178;goto
_LLF6;_LLF9:;_LLFA:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);_LLF6:;}
return yyzzz;}static char _tmp17A[25]="list_t<switchC_clause_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchCClauseList_tok={Cyc_Core_Failure,{_tmp17A,_tmp17A,_tmp17A + 25}};
struct Cyc_List_List*Cyc_yyget_SwitchCClauseList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp17B=yy1;struct Cyc_List_List*_tmp17C;_LLFC: if(*((void**)_tmp17B)
!= Cyc_SwitchCClauseList_tok)goto _LLFE;_tmp17C=((struct Cyc_SwitchCClauseList_tok_struct*)
_tmp17B)->f1;_LLFD: yyzzz=_tmp17C;goto _LLFB;_LLFE:;_LLFF:(int)_throw((void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LLFB:;}return yyzzz;}static char _tmp17E[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp17E,_tmp17E,_tmp17E + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp17F=yy1;
struct Cyc_Absyn_Pat*_tmp180;_LL101: if(*((void**)_tmp17F)!= Cyc_Pattern_tok)goto
_LL103;_tmp180=((struct Cyc_Pattern_tok_struct*)_tmp17F)->f1;_LL102: yyzzz=_tmp180;
goto _LL100;_LL103:;_LL104:(int)_throw((void*)& Cyc_yyfail_Pattern_tok);_LL100:;}
return yyzzz;}static char _tmp182[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={Cyc_Core_Failure,{_tmp182,_tmp182,_tmp182 + 14}};
struct Cyc_List_List*Cyc_yyget_PatternList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp183=yy1;struct Cyc_List_List*_tmp184;_LL106: if(*((void**)_tmp183)
!= Cyc_PatternList_tok)goto _LL108;_tmp184=((struct Cyc_PatternList_tok_struct*)
_tmp183)->f1;_LL107: yyzzz=_tmp184;goto _LL105;_LL108:;_LL109:(int)_throw((void*)&
Cyc_yyfail_PatternList_tok);_LL105:;}return yyzzz;}static char _tmp186[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{
_tmp186,_tmp186,_tmp186 + 31}};struct _tuple11*Cyc_yyget_FieldPattern_tok(void*yy1){
struct _tuple11*yyzzz;{void*_tmp187=yy1;struct _tuple11*_tmp188;_LL10B: if(*((void**)
_tmp187)!= Cyc_FieldPattern_tok)goto _LL10D;_tmp188=((struct Cyc_FieldPattern_tok_struct*)
_tmp187)->f1;_LL10C: yyzzz=_tmp188;goto _LL10A;_LL10D:;_LL10E:(int)_throw((void*)&
Cyc_yyfail_FieldPattern_tok);_LL10A:;}return yyzzz;}static char _tmp18A[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp18A,_tmp18A,_tmp18A + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp18B=yy1;struct Cyc_List_List*
_tmp18C;_LL110: if(*((void**)_tmp18B)!= Cyc_FieldPatternList_tok)goto _LL112;
_tmp18C=((struct Cyc_FieldPatternList_tok_struct*)_tmp18B)->f1;_LL111: yyzzz=
_tmp18C;goto _LL10F;_LL112:;_LL113:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL10F:;}return yyzzz;}static char _tmp18E[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={Cyc_Core_Failure,{_tmp18E,_tmp18E,_tmp18E + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok(void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp18F=yy1;
struct Cyc_Absyn_Fndecl*_tmp190;_LL115: if(*((void**)_tmp18F)!= Cyc_FnDecl_tok)
goto _LL117;_tmp190=((struct Cyc_FnDecl_tok_struct*)_tmp18F)->f1;_LL116: yyzzz=
_tmp190;goto _LL114;_LL117:;_LL118:(int)_throw((void*)& Cyc_yyfail_FnDecl_tok);
_LL114:;}return yyzzz;}static char _tmp192[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={Cyc_Core_Failure,{_tmp192,_tmp192,_tmp192 + 15}};struct
Cyc_List_List*Cyc_yyget_DeclList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp193=yy1;struct Cyc_List_List*_tmp194;_LL11A: if(*((void**)_tmp193)!= Cyc_DeclList_tok)
goto _LL11C;_tmp194=((struct Cyc_DeclList_tok_struct*)_tmp193)->f1;_LL11B: yyzzz=
_tmp194;goto _LL119;_LL11C:;_LL11D:(int)_throw((void*)& Cyc_yyfail_DeclList_tok);
_LL119:;}return yyzzz;}static char _tmp196[12]="decl_spec_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={Cyc_Core_Failure,{_tmp196,_tmp196,_tmp196 + 12}};struct
Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(void*yy1){struct Cyc_Parse_Declaration_spec*
yyzzz;{void*_tmp197=yy1;struct Cyc_Parse_Declaration_spec*_tmp198;_LL11F: if(*((
void**)_tmp197)!= Cyc_DeclSpec_tok)goto _LL121;_tmp198=((struct Cyc_DeclSpec_tok_struct*)
_tmp197)->f1;_LL120: yyzzz=_tmp198;goto _LL11E;_LL121:;_LL122:(int)_throw((void*)&
Cyc_yyfail_DeclSpec_tok);_LL11E:;}return yyzzz;}static char _tmp19A[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{
_tmp19A,_tmp19A,_tmp19A + 27}};struct _tuple12*Cyc_yyget_InitDecl_tok(void*yy1){
struct _tuple12*yyzzz;{void*_tmp19B=yy1;struct _tuple12*_tmp19C;_LL124: if(*((void**)
_tmp19B)!= Cyc_InitDecl_tok)goto _LL126;_tmp19C=((struct Cyc_InitDecl_tok_struct*)
_tmp19B)->f1;_LL125: yyzzz=_tmp19C;goto _LL123;_LL126:;_LL127:(int)_throw((void*)&
Cyc_yyfail_InitDecl_tok);_LL123:;}return yyzzz;}static char _tmp19E[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp19E,_tmp19E,_tmp19E + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp19F=yy1;struct Cyc_List_List*
_tmp1A0;_LL129: if(*((void**)_tmp19F)!= Cyc_InitDeclList_tok)goto _LL12B;_tmp1A0=((
struct Cyc_InitDeclList_tok_struct*)_tmp19F)->f1;_LL12A: yyzzz=_tmp1A0;goto _LL128;
_LL12B:;_LL12C:(int)_throw((void*)& Cyc_yyfail_InitDeclList_tok);_LL128:;}return
yyzzz;}static char _tmp1A2[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{_tmp1A2,_tmp1A2,_tmp1A2 + 16}};
void*Cyc_yyget_StorageClass_tok(void*yy1){void*yyzzz;{void*_tmp1A3=yy1;void*
_tmp1A4;_LL12E: if(*((void**)_tmp1A3)!= Cyc_StorageClass_tok)goto _LL130;_tmp1A4=(
void*)((struct Cyc_StorageClass_tok_struct*)_tmp1A3)->f1;_LL12F: yyzzz=_tmp1A4;
goto _LL12D;_LL130:;_LL131:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);
_LL12D:;}return yyzzz;}static char _tmp1A6[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{_tmp1A6,_tmp1A6,_tmp1A6 + 17}};
void*Cyc_yyget_TypeSpecifier_tok(void*yy1){void*yyzzz;{void*_tmp1A7=yy1;void*
_tmp1A8;_LL133: if(*((void**)_tmp1A7)!= Cyc_TypeSpecifier_tok)goto _LL135;_tmp1A8=(
void*)((struct Cyc_TypeSpecifier_tok_struct*)_tmp1A7)->f1;_LL134: yyzzz=_tmp1A8;
goto _LL132;_LL135:;_LL136:(int)_throw((void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL132:;}return yyzzz;}static char _tmp1AA[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={Cyc_Core_Failure,{_tmp1AA,_tmp1AA,_tmp1AA + 12}};
void*Cyc_yyget_StructOrUnion_tok(void*yy1){void*yyzzz;{void*_tmp1AB=yy1;void*
_tmp1AC;_LL138: if(*((void**)_tmp1AB)!= Cyc_StructOrUnion_tok)goto _LL13A;_tmp1AC=(
void*)((struct Cyc_StructOrUnion_tok_struct*)_tmp1AB)->f1;_LL139: yyzzz=_tmp1AC;
goto _LL137;_LL13A:;_LL13B:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);
_LL137:;}return yyzzz;}static char _tmp1AE[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp1AE,_tmp1AE,_tmp1AE + 8}};struct
Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*
_tmp1AF=yy1;struct Cyc_Absyn_Tqual _tmp1B0;_LL13D: if(*((void**)_tmp1AF)!= Cyc_TypeQual_tok)
goto _LL13F;_tmp1B0=((struct Cyc_TypeQual_tok_struct*)_tmp1AF)->f1;_LL13E: yyzzz=
_tmp1B0;goto _LL13C;_LL13F:;_LL140:(int)_throw((void*)& Cyc_yyfail_TypeQual_tok);
_LL13C:;}return yyzzz;}static char _tmp1B2[20]="list_t<aggrfield_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclList_tok={Cyc_Core_Failure,{
_tmp1B2,_tmp1B2,_tmp1B2 + 20}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B3=yy1;struct Cyc_List_List*
_tmp1B4;_LL142: if(*((void**)_tmp1B3)!= Cyc_AggrFieldDeclList_tok)goto _LL144;
_tmp1B4=((struct Cyc_AggrFieldDeclList_tok_struct*)_tmp1B3)->f1;_LL143: yyzzz=
_tmp1B4;goto _LL141;_LL144:;_LL145:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclList_tok);
_LL141:;}return yyzzz;}static char _tmp1B6[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclListList_tok={Cyc_Core_Failure,{
_tmp1B6,_tmp1B6,_tmp1B6 + 28}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclListList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B7=yy1;struct Cyc_List_List*
_tmp1B8;_LL147: if(*((void**)_tmp1B7)!= Cyc_AggrFieldDeclListList_tok)goto _LL149;
_tmp1B8=((struct Cyc_AggrFieldDeclListList_tok_struct*)_tmp1B7)->f1;_LL148: yyzzz=
_tmp1B8;goto _LL146;_LL149:;_LL14A:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclListList_tok);
_LL146:;}return yyzzz;}static char _tmp1BA[24]="list_t<type_modifier_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{
_tmp1BA,_tmp1BA,_tmp1BA + 24}};struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1BB=yy1;struct Cyc_List_List*
_tmp1BC;_LL14C: if(*((void**)_tmp1BB)!= Cyc_TypeModifierList_tok)goto _LL14E;
_tmp1BC=((struct Cyc_TypeModifierList_tok_struct*)_tmp1BB)->f1;_LL14D: yyzzz=
_tmp1BC;goto _LL14B;_LL14E:;_LL14F:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);
_LL14B:;}return yyzzz;}static char _tmp1BE[13]="declarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp1BE,_tmp1BE,_tmp1BE + 13}};
struct Cyc_Parse_Declarator*Cyc_yyget_Declarator_tok(void*yy1){struct Cyc_Parse_Declarator*
yyzzz;{void*_tmp1BF=yy1;struct Cyc_Parse_Declarator*_tmp1C0;_LL151: if(*((void**)
_tmp1BF)!= Cyc_Declarator_tok)goto _LL153;_tmp1C0=((struct Cyc_Declarator_tok_struct*)
_tmp1BF)->f1;_LL152: yyzzz=_tmp1C0;goto _LL150;_LL153:;_LL154:(int)_throw((void*)&
Cyc_yyfail_Declarator_tok);_LL150:;}return yyzzz;}static char _tmp1C2[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{
_tmp1C2,_tmp1C2,_tmp1C2 + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(
void*yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp1C3=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp1C4;_LL156: if(*((void**)_tmp1C3)!= Cyc_AbstractDeclarator_tok)goto _LL158;
_tmp1C4=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp1C3)->f1;_LL157: yyzzz=
_tmp1C4;goto _LL155;_LL158:;_LL159:(int)_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL155:;}return yyzzz;}static char _tmp1C6[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={Cyc_Core_Failure,{_tmp1C6,_tmp1C6,_tmp1C6 + 5}};int Cyc_yyget_Bool_tok(
void*yy1){int yyzzz;{void*_tmp1C7=yy1;int _tmp1C8;_LL15B: if(*((void**)_tmp1C7)!= 
Cyc_Bool_tok)goto _LL15D;_tmp1C8=((struct Cyc_Bool_tok_struct*)_tmp1C7)->f1;_LL15C:
yyzzz=_tmp1C8;goto _LL15A;_LL15D:;_LL15E:(int)_throw((void*)& Cyc_yyfail_Bool_tok);
_LL15A:;}return yyzzz;}static char _tmp1CA[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={Cyc_Core_Failure,{_tmp1CA,_tmp1CA,_tmp1CA + 8}};void*Cyc_yyget_Scope_tok(
void*yy1){void*yyzzz;{void*_tmp1CB=yy1;void*_tmp1CC;_LL160: if(*((void**)_tmp1CB)
!= Cyc_Scope_tok)goto _LL162;_tmp1CC=(void*)((struct Cyc_Scope_tok_struct*)_tmp1CB)->f1;
_LL161: yyzzz=_tmp1CC;goto _LL15F;_LL162:;_LL163:(int)_throw((void*)& Cyc_yyfail_Scope_tok);
_LL15F:;}return yyzzz;}static char _tmp1CE[14]="tunionfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={Cyc_Core_Failure,{_tmp1CE,_tmp1CE,_tmp1CE + 14}};
struct Cyc_Absyn_Tunionfield*Cyc_yyget_TunionField_tok(void*yy1){struct Cyc_Absyn_Tunionfield*
yyzzz;{void*_tmp1CF=yy1;struct Cyc_Absyn_Tunionfield*_tmp1D0;_LL165: if(*((void**)
_tmp1CF)!= Cyc_TunionField_tok)goto _LL167;_tmp1D0=((struct Cyc_TunionField_tok_struct*)
_tmp1CF)->f1;_LL166: yyzzz=_tmp1D0;goto _LL164;_LL167:;_LL168:(int)_throw((void*)&
Cyc_yyfail_TunionField_tok);_LL164:;}return yyzzz;}static char _tmp1D2[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{
_tmp1D2,_tmp1D2,_tmp1D2 + 22}};struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1D3=yy1;struct Cyc_List_List*
_tmp1D4;_LL16A: if(*((void**)_tmp1D3)!= Cyc_TunionFieldList_tok)goto _LL16C;
_tmp1D4=((struct Cyc_TunionFieldList_tok_struct*)_tmp1D3)->f1;_LL16B: yyzzz=
_tmp1D4;goto _LL169;_LL16C:;_LL16D:(int)_throw((void*)& Cyc_yyfail_TunionFieldList_tok);
_LL169:;}return yyzzz;}static char _tmp1D6[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{
_tmp1D6,_tmp1D6,_tmp1D6 + 50}};struct _tuple15*Cyc_yyget_QualSpecList_tok(void*yy1){
struct _tuple15*yyzzz;{void*_tmp1D7=yy1;struct _tuple15*_tmp1D8;_LL16F: if(*((void**)
_tmp1D7)!= Cyc_QualSpecList_tok)goto _LL171;_tmp1D8=((struct Cyc_QualSpecList_tok_struct*)
_tmp1D7)->f1;_LL170: yyzzz=_tmp1D8;goto _LL16E;_LL171:;_LL172:(int)_throw((void*)&
Cyc_yyfail_QualSpecList_tok);_LL16E:;}return yyzzz;}static char _tmp1DA[14]="list_t<var_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={Cyc_Core_Failure,{
_tmp1DA,_tmp1DA,_tmp1DA + 14}};struct Cyc_List_List*Cyc_yyget_IdList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp1DB=yy1;struct Cyc_List_List*_tmp1DC;_LL174:
if(*((void**)_tmp1DB)!= Cyc_IdList_tok)goto _LL176;_tmp1DC=((struct Cyc_IdList_tok_struct*)
_tmp1DB)->f1;_LL175: yyzzz=_tmp1DC;goto _LL173;_LL176:;_LL177:(int)_throw((void*)&
Cyc_yyfail_IdList_tok);_LL173:;}return yyzzz;}static char _tmp1DE[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={Cyc_Core_Failure,{
_tmp1DE,_tmp1DE,_tmp1DE + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(void*yy1){
struct _tuple2*yyzzz;{void*_tmp1DF=yy1;struct _tuple2*_tmp1E0;_LL179: if(*((void**)
_tmp1DF)!= Cyc_ParamDecl_tok)goto _LL17B;_tmp1E0=((struct Cyc_ParamDecl_tok_struct*)
_tmp1DF)->f1;_LL17A: yyzzz=_tmp1E0;goto _LL178;_LL17B:;_LL17C:(int)_throw((void*)&
Cyc_yyfail_ParamDecl_tok);_LL178:;}return yyzzz;}static char _tmp1E2[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp1E2,_tmp1E2,_tmp1E2 + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1E3=yy1;struct Cyc_List_List*
_tmp1E4;_LL17E: if(*((void**)_tmp1E3)!= Cyc_ParamDeclList_tok)goto _LL180;_tmp1E4=((
struct Cyc_ParamDeclList_tok_struct*)_tmp1E3)->f1;_LL17F: yyzzz=_tmp1E4;goto _LL17D;
_LL180:;_LL181:(int)_throw((void*)& Cyc_yyfail_ParamDeclList_tok);_LL17D:;}return
yyzzz;}static char _tmp1E6[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1E6,
_tmp1E6,_tmp1E6 + 107}};struct _tuple14*Cyc_yyget_YY1(void*yy1){struct _tuple14*
yyzzz;{void*_tmp1E7=yy1;struct _tuple14*_tmp1E8;_LL183: if(*((void**)_tmp1E7)!= 
Cyc_YY1)goto _LL185;_tmp1E8=((struct Cyc_YY1_struct*)_tmp1E7)->f1;_LL184: yyzzz=
_tmp1E8;goto _LL182;_LL185:;_LL186:(int)_throw((void*)& Cyc_yyfail_YY1);_LL182:;}
return yyzzz;}static char _tmp1EA[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={Cyc_Core_Failure,{_tmp1EA,_tmp1EA,_tmp1EA + 15}};struct
Cyc_List_List*Cyc_yyget_TypeList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp1EB=yy1;struct Cyc_List_List*_tmp1EC;_LL188: if(*((void**)_tmp1EB)!= Cyc_TypeList_tok)
goto _LL18A;_tmp1EC=((struct Cyc_TypeList_tok_struct*)_tmp1EB)->f1;_LL189: yyzzz=
_tmp1EC;goto _LL187;_LL18A:;_LL18B:(int)_throw((void*)& Cyc_yyfail_TypeList_tok);
_LL187:;}return yyzzz;}static char _tmp1EE[21]="list_t<designator_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{_tmp1EE,
_tmp1EE,_tmp1EE + 21}};struct Cyc_List_List*Cyc_yyget_DesignatorList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp1EF=yy1;struct Cyc_List_List*_tmp1F0;_LL18D:
if(*((void**)_tmp1EF)!= Cyc_DesignatorList_tok)goto _LL18F;_tmp1F0=((struct Cyc_DesignatorList_tok_struct*)
_tmp1EF)->f1;_LL18E: yyzzz=_tmp1F0;goto _LL18C;_LL18F:;_LL190:(int)_throw((void*)&
Cyc_yyfail_DesignatorList_tok);_LL18C:;}return yyzzz;}static char _tmp1F2[13]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={Cyc_Core_Failure,{
_tmp1F2,_tmp1F2,_tmp1F2 + 13}};void*Cyc_yyget_Designator_tok(void*yy1){void*yyzzz;{
void*_tmp1F3=yy1;void*_tmp1F4;_LL192: if(*((void**)_tmp1F3)!= Cyc_Designator_tok)
goto _LL194;_tmp1F4=(void*)((struct Cyc_Designator_tok_struct*)_tmp1F3)->f1;_LL193:
yyzzz=_tmp1F4;goto _LL191;_LL194:;_LL195:(int)_throw((void*)& Cyc_yyfail_Designator_tok);
_LL191:;}return yyzzz;}static char _tmp1F6[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{_tmp1F6,_tmp1F6,_tmp1F6 + 7}};void*Cyc_yyget_Kind_tok(
void*yy1){void*yyzzz;{void*_tmp1F7=yy1;void*_tmp1F8;_LL197: if(*((void**)_tmp1F7)
!= Cyc_Kind_tok)goto _LL199;_tmp1F8=(void*)((struct Cyc_Kind_tok_struct*)_tmp1F7)->f1;
_LL198: yyzzz=_tmp1F8;goto _LL196;_LL199:;_LL19A:(int)_throw((void*)& Cyc_yyfail_Kind_tok);
_LL196:;}return yyzzz;}static char _tmp1FA[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Type_tok={Cyc_Core_Failure,{_tmp1FA,_tmp1FA,_tmp1FA + 7}};void*Cyc_yyget_Type_tok(
void*yy1){void*yyzzz;{void*_tmp1FB=yy1;void*_tmp1FC;_LL19C: if(*((void**)_tmp1FB)
!= Cyc_Type_tok)goto _LL19E;_tmp1FC=(void*)((struct Cyc_Type_tok_struct*)_tmp1FB)->f1;
_LL19D: yyzzz=_tmp1FC;goto _LL19B;_LL19E:;_LL19F:(int)_throw((void*)& Cyc_yyfail_Type_tok);
_LL19B:;}return yyzzz;}static char _tmp1FE[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={Cyc_Core_Failure,{_tmp1FE,
_tmp1FE,_tmp1FE + 20}};struct Cyc_List_List*Cyc_yyget_AttributeList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp1FF=yy1;struct Cyc_List_List*_tmp200;_LL1A1:
if(*((void**)_tmp1FF)!= Cyc_AttributeList_tok)goto _LL1A3;_tmp200=((struct Cyc_AttributeList_tok_struct*)
_tmp1FF)->f1;_LL1A2: yyzzz=_tmp200;goto _LL1A0;_LL1A3:;_LL1A4:(int)_throw((void*)&
Cyc_yyfail_AttributeList_tok);_LL1A0:;}return yyzzz;}static char _tmp202[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{
_tmp202,_tmp202,_tmp202 + 12}};void*Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{
void*_tmp203=yy1;void*_tmp204;_LL1A6: if(*((void**)_tmp203)!= Cyc_Attribute_tok)
goto _LL1A8;_tmp204=(void*)((struct Cyc_Attribute_tok_struct*)_tmp203)->f1;_LL1A7:
yyzzz=_tmp204;goto _LL1A5;_LL1A8:;_LL1A9:(int)_throw((void*)& Cyc_yyfail_Attribute_tok);
_LL1A5:;}return yyzzz;}static char _tmp206[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={Cyc_Core_Failure,{_tmp206,_tmp206,_tmp206 + 12}};struct
Cyc_Absyn_Enumfield*Cyc_yyget_Enumfield_tok(void*yy1){struct Cyc_Absyn_Enumfield*
yyzzz;{void*_tmp207=yy1;struct Cyc_Absyn_Enumfield*_tmp208;_LL1AB: if(*((void**)
_tmp207)!= Cyc_Enumfield_tok)goto _LL1AD;_tmp208=((struct Cyc_Enumfield_tok_struct*)
_tmp207)->f1;_LL1AC: yyzzz=_tmp208;goto _LL1AA;_LL1AD:;_LL1AE:(int)_throw((void*)&
Cyc_yyfail_Enumfield_tok);_LL1AA:;}return yyzzz;}static char _tmp20A[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{
_tmp20A,_tmp20A,_tmp20A + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp20B=yy1;struct Cyc_List_List*
_tmp20C;_LL1B0: if(*((void**)_tmp20B)!= Cyc_EnumfieldList_tok)goto _LL1B2;_tmp20C=((
struct Cyc_EnumfieldList_tok_struct*)_tmp20B)->f1;_LL1B1: yyzzz=_tmp20C;goto _LL1AF;
_LL1B2:;_LL1B3:(int)_throw((void*)& Cyc_yyfail_EnumfieldList_tok);_LL1AF:;}return
yyzzz;}static char _tmp20E[14]="opt_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={Cyc_Core_Failure,{_tmp20E,_tmp20E,_tmp20E + 14}};struct
Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp20F=yy1;struct Cyc_Core_Opt*_tmp210;_LL1B5: if(*((void**)_tmp20F)!= Cyc_TypeOpt_tok)
goto _LL1B7;_tmp210=((struct Cyc_TypeOpt_tok_struct*)_tmp20F)->f1;_LL1B6: yyzzz=
_tmp210;goto _LL1B4;_LL1B7:;_LL1B8:(int)_throw((void*)& Cyc_yyfail_TypeOpt_tok);
_LL1B4:;}return yyzzz;}static char _tmp212[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{_tmp212,
_tmp212,_tmp212 + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp213=yy1;struct Cyc_List_List*_tmp214;_LL1BA:
if(*((void**)_tmp213)!= Cyc_Rgnorder_tok)goto _LL1BC;_tmp214=((struct Cyc_Rgnorder_tok_struct*)
_tmp213)->f1;_LL1BB: yyzzz=_tmp214;goto _LL1B9;_LL1BC:;_LL1BD:(int)_throw((void*)&
Cyc_yyfail_Rgnorder_tok);_LL1B9:;}return yyzzz;}struct Cyc_Yyltype{int timestamp;
int first_line;int first_column;int last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp215;_tmp215.timestamp=0;_tmp215.first_line=0;
_tmp215.first_column=0;_tmp215.last_line=0;_tmp215.last_column=0;_tmp215;});}
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[355]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,126,2,2,108,124,120,2,
104,105,114,117,106,119,111,123,2,2,2,2,2,2,2,2,2,2,110,100,109,103,118,116,115,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,112,2,113,122,107,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,101,121,102,125,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99};static char _tmp216[2]="$";static
char _tmp217[6]="error";static char _tmp218[12]="$undefined.";static char _tmp219[5]="AUTO";
static char _tmp21A[9]="REGISTER";static char _tmp21B[7]="STATIC";static char _tmp21C[
7]="EXTERN";static char _tmp21D[8]="TYPEDEF";static char _tmp21E[5]="VOID";static
char _tmp21F[5]="CHAR";static char _tmp220[6]="SHORT";static char _tmp221[4]="INT";
static char _tmp222[5]="LONG";static char _tmp223[6]="FLOAT";static char _tmp224[7]="DOUBLE";
static char _tmp225[7]="SIGNED";static char _tmp226[9]="UNSIGNED";static char _tmp227[
6]="CONST";static char _tmp228[9]="VOLATILE";static char _tmp229[9]="RESTRICT";
static char _tmp22A[7]="STRUCT";static char _tmp22B[6]="UNION";static char _tmp22C[5]="CASE";
static char _tmp22D[8]="DEFAULT";static char _tmp22E[7]="INLINE";static char _tmp22F[7]="SIZEOF";
static char _tmp230[9]="OFFSETOF";static char _tmp231[3]="IF";static char _tmp232[5]="ELSE";
static char _tmp233[7]="SWITCH";static char _tmp234[6]="WHILE";static char _tmp235[3]="DO";
static char _tmp236[4]="FOR";static char _tmp237[5]="GOTO";static char _tmp238[9]="CONTINUE";
static char _tmp239[6]="BREAK";static char _tmp23A[7]="RETURN";static char _tmp23B[5]="ENUM";
static char _tmp23C[8]="NULL_kw";static char _tmp23D[4]="LET";static char _tmp23E[6]="THROW";
static char _tmp23F[4]="TRY";static char _tmp240[6]="CATCH";static char _tmp241[4]="NEW";
static char _tmp242[9]="ABSTRACT";static char _tmp243[9]="FALLTHRU";static char
_tmp244[6]="USING";static char _tmp245[10]="NAMESPACE";static char _tmp246[7]="TUNION";
static char _tmp247[8]="XTUNION";static char _tmp248[9]="FORARRAY";static char _tmp249[
5]="FILL";static char _tmp24A[8]="CODEGEN";static char _tmp24B[4]="CUT";static char
_tmp24C[7]="SPLICE";static char _tmp24D[7]="MALLOC";static char _tmp24E[8]="RMALLOC";
static char _tmp24F[7]="CALLOC";static char _tmp250[8]="RCALLOC";static char _tmp251[9]="REGION_T";
static char _tmp252[9]="SIZEOF_T";static char _tmp253[7]="REGION";static char _tmp254[
5]="RNEW";static char _tmp255[8]="REGIONS";static char _tmp256[13]="RESET_REGION";
static char _tmp257[4]="GEN";static char _tmp258[7]="PTR_OP";static char _tmp259[7]="INC_OP";
static char _tmp25A[7]="DEC_OP";static char _tmp25B[8]="LEFT_OP";static char _tmp25C[9]="RIGHT_OP";
static char _tmp25D[6]="LE_OP";static char _tmp25E[6]="GE_OP";static char _tmp25F[6]="EQ_OP";
static char _tmp260[6]="NE_OP";static char _tmp261[7]="AND_OP";static char _tmp262[6]="OR_OP";
static char _tmp263[11]="MUL_ASSIGN";static char _tmp264[11]="DIV_ASSIGN";static char
_tmp265[11]="MOD_ASSIGN";static char _tmp266[11]="ADD_ASSIGN";static char _tmp267[11]="SUB_ASSIGN";
static char _tmp268[12]="LEFT_ASSIGN";static char _tmp269[13]="RIGHT_ASSIGN";static
char _tmp26A[11]="AND_ASSIGN";static char _tmp26B[11]="XOR_ASSIGN";static char
_tmp26C[10]="OR_ASSIGN";static char _tmp26D[9]="ELLIPSIS";static char _tmp26E[11]="LEFT_RIGHT";
static char _tmp26F[12]="COLON_COLON";static char _tmp270[11]="IDENTIFIER";static
char _tmp271[17]="INTEGER_CONSTANT";static char _tmp272[7]="STRING";static char
_tmp273[19]="CHARACTER_CONSTANT";static char _tmp274[18]="FLOATING_CONSTANT";
static char _tmp275[9]="TYPE_VAR";static char _tmp276[13]="TYPEDEF_NAME";static char
_tmp277[16]="QUAL_IDENTIFIER";static char _tmp278[18]="QUAL_TYPEDEF_NAME";static
char _tmp279[10]="ATTRIBUTE";static char _tmp27A[4]="';'";static char _tmp27B[4]="'{'";
static char _tmp27C[4]="'}'";static char _tmp27D[4]="'='";static char _tmp27E[4]="'('";
static char _tmp27F[4]="')'";static char _tmp280[4]="','";static char _tmp281[4]="'_'";
static char _tmp282[4]="'$'";static char _tmp283[4]="'<'";static char _tmp284[4]="':'";
static char _tmp285[4]="'.'";static char _tmp286[4]="'['";static char _tmp287[4]="']'";
static char _tmp288[4]="'*'";static char _tmp289[4]="'@'";static char _tmp28A[4]="'?'";
static char _tmp28B[4]="'+'";static char _tmp28C[4]="'>'";static char _tmp28D[4]="'-'";
static char _tmp28E[4]="'&'";static char _tmp28F[4]="'|'";static char _tmp290[4]="'^'";
static char _tmp291[4]="'/'";static char _tmp292[4]="'%'";static char _tmp293[4]="'~'";
static char _tmp294[4]="'!'";static char _tmp295[5]="prog";static char _tmp296[17]="translation_unit";
static char _tmp297[21]="external_declaration";static char _tmp298[20]="function_definition";
static char _tmp299[21]="function_definition2";static char _tmp29A[13]="using_action";
static char _tmp29B[15]="unusing_action";static char _tmp29C[17]="namespace_action";
static char _tmp29D[19]="unnamespace_action";static char _tmp29E[12]="declaration";
static char _tmp29F[17]="declaration_list";static char _tmp2A0[23]="declaration_specifiers";
static char _tmp2A1[24]="storage_class_specifier";static char _tmp2A2[15]="attributes_opt";
static char _tmp2A3[11]="attributes";static char _tmp2A4[15]="attribute_list";static
char _tmp2A5[10]="attribute";static char _tmp2A6[15]="type_specifier";static char
_tmp2A7[5]="kind";static char _tmp2A8[15]="type_qualifier";static char _tmp2A9[15]="enum_specifier";
static char _tmp2AA[11]="enum_field";static char _tmp2AB[22]="enum_declaration_list";
static char _tmp2AC[26]="struct_or_union_specifier";static char _tmp2AD[16]="type_params_opt";
static char _tmp2AE[16]="struct_or_union";static char _tmp2AF[24]="struct_declaration_list";
static char _tmp2B0[25]="struct_declaration_list0";static char _tmp2B1[21]="init_declarator_list";
static char _tmp2B2[22]="init_declarator_list0";static char _tmp2B3[16]="init_declarator";
static char _tmp2B4[19]="struct_declaration";static char _tmp2B5[25]="specifier_qualifier_list";
static char _tmp2B6[23]="struct_declarator_list";static char _tmp2B7[24]="struct_declarator_list0";
static char _tmp2B8[18]="struct_declarator";static char _tmp2B9[17]="tunion_specifier";
static char _tmp2BA[18]="tunion_or_xtunion";static char _tmp2BB[17]="tunionfield_list";
static char _tmp2BC[18]="tunionfield_scope";static char _tmp2BD[12]="tunionfield";
static char _tmp2BE[11]="declarator";static char _tmp2BF[18]="direct_declarator";
static char _tmp2C0[8]="pointer";static char _tmp2C1[13]="pointer_char";static char
_tmp2C2[8]="rgn_opt";static char _tmp2C3[4]="rgn";static char _tmp2C4[20]="type_qualifier_list";
static char _tmp2C5[20]="parameter_type_list";static char _tmp2C6[9]="type_var";
static char _tmp2C7[16]="optional_effect";static char _tmp2C8[19]="optional_rgn_order";
static char _tmp2C9[10]="rgn_order";static char _tmp2CA[16]="optional_inject";static
char _tmp2CB[11]="effect_set";static char _tmp2CC[14]="atomic_effect";static char
_tmp2CD[11]="region_set";static char _tmp2CE[15]="parameter_list";static char
_tmp2CF[22]="parameter_declaration";static char _tmp2D0[16]="identifier_list";
static char _tmp2D1[17]="identifier_list0";static char _tmp2D2[12]="initializer";
static char _tmp2D3[18]="array_initializer";static char _tmp2D4[17]="initializer_list";
static char _tmp2D5[12]="designation";static char _tmp2D6[16]="designator_list";
static char _tmp2D7[11]="designator";static char _tmp2D8[10]="type_name";static char
_tmp2D9[14]="any_type_name";static char _tmp2DA[15]="type_name_list";static char
_tmp2DB[20]="abstract_declarator";static char _tmp2DC[27]="direct_abstract_declarator";
static char _tmp2DD[10]="statement";static char _tmp2DE[18]="labeled_statement";
static char _tmp2DF[21]="expression_statement";static char _tmp2E0[19]="compound_statement";
static char _tmp2E1[16]="block_item_list";static char _tmp2E2[20]="selection_statement";
static char _tmp2E3[15]="switch_clauses";static char _tmp2E4[16]="switchC_clauses";
static char _tmp2E5[20]="iteration_statement";static char _tmp2E6[15]="jump_statement";
static char _tmp2E7[8]="pattern";static char _tmp2E8[19]="tuple_pattern_list";static
char _tmp2E9[14]="field_pattern";static char _tmp2EA[19]="field_pattern_list";
static char _tmp2EB[11]="expression";static char _tmp2EC[22]="assignment_expression";
static char _tmp2ED[20]="assignment_operator";static char _tmp2EE[23]="conditional_expression";
static char _tmp2EF[20]="constant_expression";static char _tmp2F0[22]="logical_or_expression";
static char _tmp2F1[23]="logical_and_expression";static char _tmp2F2[24]="inclusive_or_expression";
static char _tmp2F3[24]="exclusive_or_expression";static char _tmp2F4[15]="and_expression";
static char _tmp2F5[20]="equality_expression";static char _tmp2F6[22]="relational_expression";
static char _tmp2F7[17]="shift_expression";static char _tmp2F8[20]="additive_expression";
static char _tmp2F9[26]="multiplicative_expression";static char _tmp2FA[16]="cast_expression";
static char _tmp2FB[17]="unary_expression";static char _tmp2FC[15]="unary_operator";
static char _tmp2FD[19]="postfix_expression";static char _tmp2FE[19]="primary_expression";
static char _tmp2FF[25]="argument_expression_list";static char _tmp300[26]="argument_expression_list0";
static char _tmp301[9]="constant";static char _tmp302[20]="qual_opt_identifier";
static char _tmp303[17]="qual_opt_typedef";static char _tmp304[18]="struct_union_name";
static char _tmp305[11]="field_name";static char _tmp306[12]="right_angle";static
struct _tagged_arr Cyc_yytname[241]={{_tmp216,_tmp216,_tmp216 + 2},{_tmp217,_tmp217,
_tmp217 + 6},{_tmp218,_tmp218,_tmp218 + 12},{_tmp219,_tmp219,_tmp219 + 5},{_tmp21A,
_tmp21A,_tmp21A + 9},{_tmp21B,_tmp21B,_tmp21B + 7},{_tmp21C,_tmp21C,_tmp21C + 7},{
_tmp21D,_tmp21D,_tmp21D + 8},{_tmp21E,_tmp21E,_tmp21E + 5},{_tmp21F,_tmp21F,
_tmp21F + 5},{_tmp220,_tmp220,_tmp220 + 6},{_tmp221,_tmp221,_tmp221 + 4},{_tmp222,
_tmp222,_tmp222 + 5},{_tmp223,_tmp223,_tmp223 + 6},{_tmp224,_tmp224,_tmp224 + 7},{
_tmp225,_tmp225,_tmp225 + 7},{_tmp226,_tmp226,_tmp226 + 9},{_tmp227,_tmp227,
_tmp227 + 6},{_tmp228,_tmp228,_tmp228 + 9},{_tmp229,_tmp229,_tmp229 + 9},{_tmp22A,
_tmp22A,_tmp22A + 7},{_tmp22B,_tmp22B,_tmp22B + 6},{_tmp22C,_tmp22C,_tmp22C + 5},{
_tmp22D,_tmp22D,_tmp22D + 8},{_tmp22E,_tmp22E,_tmp22E + 7},{_tmp22F,_tmp22F,
_tmp22F + 7},{_tmp230,_tmp230,_tmp230 + 9},{_tmp231,_tmp231,_tmp231 + 3},{_tmp232,
_tmp232,_tmp232 + 5},{_tmp233,_tmp233,_tmp233 + 7},{_tmp234,_tmp234,_tmp234 + 6},{
_tmp235,_tmp235,_tmp235 + 3},{_tmp236,_tmp236,_tmp236 + 4},{_tmp237,_tmp237,
_tmp237 + 5},{_tmp238,_tmp238,_tmp238 + 9},{_tmp239,_tmp239,_tmp239 + 6},{_tmp23A,
_tmp23A,_tmp23A + 7},{_tmp23B,_tmp23B,_tmp23B + 5},{_tmp23C,_tmp23C,_tmp23C + 8},{
_tmp23D,_tmp23D,_tmp23D + 4},{_tmp23E,_tmp23E,_tmp23E + 6},{_tmp23F,_tmp23F,
_tmp23F + 4},{_tmp240,_tmp240,_tmp240 + 6},{_tmp241,_tmp241,_tmp241 + 4},{_tmp242,
_tmp242,_tmp242 + 9},{_tmp243,_tmp243,_tmp243 + 9},{_tmp244,_tmp244,_tmp244 + 6},{
_tmp245,_tmp245,_tmp245 + 10},{_tmp246,_tmp246,_tmp246 + 7},{_tmp247,_tmp247,
_tmp247 + 8},{_tmp248,_tmp248,_tmp248 + 9},{_tmp249,_tmp249,_tmp249 + 5},{_tmp24A,
_tmp24A,_tmp24A + 8},{_tmp24B,_tmp24B,_tmp24B + 4},{_tmp24C,_tmp24C,_tmp24C + 7},{
_tmp24D,_tmp24D,_tmp24D + 7},{_tmp24E,_tmp24E,_tmp24E + 8},{_tmp24F,_tmp24F,
_tmp24F + 7},{_tmp250,_tmp250,_tmp250 + 8},{_tmp251,_tmp251,_tmp251 + 9},{_tmp252,
_tmp252,_tmp252 + 9},{_tmp253,_tmp253,_tmp253 + 7},{_tmp254,_tmp254,_tmp254 + 5},{
_tmp255,_tmp255,_tmp255 + 8},{_tmp256,_tmp256,_tmp256 + 13},{_tmp257,_tmp257,
_tmp257 + 4},{_tmp258,_tmp258,_tmp258 + 7},{_tmp259,_tmp259,_tmp259 + 7},{_tmp25A,
_tmp25A,_tmp25A + 7},{_tmp25B,_tmp25B,_tmp25B + 8},{_tmp25C,_tmp25C,_tmp25C + 9},{
_tmp25D,_tmp25D,_tmp25D + 6},{_tmp25E,_tmp25E,_tmp25E + 6},{_tmp25F,_tmp25F,
_tmp25F + 6},{_tmp260,_tmp260,_tmp260 + 6},{_tmp261,_tmp261,_tmp261 + 7},{_tmp262,
_tmp262,_tmp262 + 6},{_tmp263,_tmp263,_tmp263 + 11},{_tmp264,_tmp264,_tmp264 + 11},{
_tmp265,_tmp265,_tmp265 + 11},{_tmp266,_tmp266,_tmp266 + 11},{_tmp267,_tmp267,
_tmp267 + 11},{_tmp268,_tmp268,_tmp268 + 12},{_tmp269,_tmp269,_tmp269 + 13},{
_tmp26A,_tmp26A,_tmp26A + 11},{_tmp26B,_tmp26B,_tmp26B + 11},{_tmp26C,_tmp26C,
_tmp26C + 10},{_tmp26D,_tmp26D,_tmp26D + 9},{_tmp26E,_tmp26E,_tmp26E + 11},{_tmp26F,
_tmp26F,_tmp26F + 12},{_tmp270,_tmp270,_tmp270 + 11},{_tmp271,_tmp271,_tmp271 + 17},{
_tmp272,_tmp272,_tmp272 + 7},{_tmp273,_tmp273,_tmp273 + 19},{_tmp274,_tmp274,
_tmp274 + 18},{_tmp275,_tmp275,_tmp275 + 9},{_tmp276,_tmp276,_tmp276 + 13},{_tmp277,
_tmp277,_tmp277 + 16},{_tmp278,_tmp278,_tmp278 + 18},{_tmp279,_tmp279,_tmp279 + 10},{
_tmp27A,_tmp27A,_tmp27A + 4},{_tmp27B,_tmp27B,_tmp27B + 4},{_tmp27C,_tmp27C,
_tmp27C + 4},{_tmp27D,_tmp27D,_tmp27D + 4},{_tmp27E,_tmp27E,_tmp27E + 4},{_tmp27F,
_tmp27F,_tmp27F + 4},{_tmp280,_tmp280,_tmp280 + 4},{_tmp281,_tmp281,_tmp281 + 4},{
_tmp282,_tmp282,_tmp282 + 4},{_tmp283,_tmp283,_tmp283 + 4},{_tmp284,_tmp284,
_tmp284 + 4},{_tmp285,_tmp285,_tmp285 + 4},{_tmp286,_tmp286,_tmp286 + 4},{_tmp287,
_tmp287,_tmp287 + 4},{_tmp288,_tmp288,_tmp288 + 4},{_tmp289,_tmp289,_tmp289 + 4},{
_tmp28A,_tmp28A,_tmp28A + 4},{_tmp28B,_tmp28B,_tmp28B + 4},{_tmp28C,_tmp28C,
_tmp28C + 4},{_tmp28D,_tmp28D,_tmp28D + 4},{_tmp28E,_tmp28E,_tmp28E + 4},{_tmp28F,
_tmp28F,_tmp28F + 4},{_tmp290,_tmp290,_tmp290 + 4},{_tmp291,_tmp291,_tmp291 + 4},{
_tmp292,_tmp292,_tmp292 + 4},{_tmp293,_tmp293,_tmp293 + 4},{_tmp294,_tmp294,
_tmp294 + 4},{_tmp295,_tmp295,_tmp295 + 5},{_tmp296,_tmp296,_tmp296 + 17},{_tmp297,
_tmp297,_tmp297 + 21},{_tmp298,_tmp298,_tmp298 + 20},{_tmp299,_tmp299,_tmp299 + 21},{
_tmp29A,_tmp29A,_tmp29A + 13},{_tmp29B,_tmp29B,_tmp29B + 15},{_tmp29C,_tmp29C,
_tmp29C + 17},{_tmp29D,_tmp29D,_tmp29D + 19},{_tmp29E,_tmp29E,_tmp29E + 12},{
_tmp29F,_tmp29F,_tmp29F + 17},{_tmp2A0,_tmp2A0,_tmp2A0 + 23},{_tmp2A1,_tmp2A1,
_tmp2A1 + 24},{_tmp2A2,_tmp2A2,_tmp2A2 + 15},{_tmp2A3,_tmp2A3,_tmp2A3 + 11},{
_tmp2A4,_tmp2A4,_tmp2A4 + 15},{_tmp2A5,_tmp2A5,_tmp2A5 + 10},{_tmp2A6,_tmp2A6,
_tmp2A6 + 15},{_tmp2A7,_tmp2A7,_tmp2A7 + 5},{_tmp2A8,_tmp2A8,_tmp2A8 + 15},{_tmp2A9,
_tmp2A9,_tmp2A9 + 15},{_tmp2AA,_tmp2AA,_tmp2AA + 11},{_tmp2AB,_tmp2AB,_tmp2AB + 22},{
_tmp2AC,_tmp2AC,_tmp2AC + 26},{_tmp2AD,_tmp2AD,_tmp2AD + 16},{_tmp2AE,_tmp2AE,
_tmp2AE + 16},{_tmp2AF,_tmp2AF,_tmp2AF + 24},{_tmp2B0,_tmp2B0,_tmp2B0 + 25},{
_tmp2B1,_tmp2B1,_tmp2B1 + 21},{_tmp2B2,_tmp2B2,_tmp2B2 + 22},{_tmp2B3,_tmp2B3,
_tmp2B3 + 16},{_tmp2B4,_tmp2B4,_tmp2B4 + 19},{_tmp2B5,_tmp2B5,_tmp2B5 + 25},{
_tmp2B6,_tmp2B6,_tmp2B6 + 23},{_tmp2B7,_tmp2B7,_tmp2B7 + 24},{_tmp2B8,_tmp2B8,
_tmp2B8 + 18},{_tmp2B9,_tmp2B9,_tmp2B9 + 17},{_tmp2BA,_tmp2BA,_tmp2BA + 18},{
_tmp2BB,_tmp2BB,_tmp2BB + 17},{_tmp2BC,_tmp2BC,_tmp2BC + 18},{_tmp2BD,_tmp2BD,
_tmp2BD + 12},{_tmp2BE,_tmp2BE,_tmp2BE + 11},{_tmp2BF,_tmp2BF,_tmp2BF + 18},{
_tmp2C0,_tmp2C0,_tmp2C0 + 8},{_tmp2C1,_tmp2C1,_tmp2C1 + 13},{_tmp2C2,_tmp2C2,
_tmp2C2 + 8},{_tmp2C3,_tmp2C3,_tmp2C3 + 4},{_tmp2C4,_tmp2C4,_tmp2C4 + 20},{_tmp2C5,
_tmp2C5,_tmp2C5 + 20},{_tmp2C6,_tmp2C6,_tmp2C6 + 9},{_tmp2C7,_tmp2C7,_tmp2C7 + 16},{
_tmp2C8,_tmp2C8,_tmp2C8 + 19},{_tmp2C9,_tmp2C9,_tmp2C9 + 10},{_tmp2CA,_tmp2CA,
_tmp2CA + 16},{_tmp2CB,_tmp2CB,_tmp2CB + 11},{_tmp2CC,_tmp2CC,_tmp2CC + 14},{
_tmp2CD,_tmp2CD,_tmp2CD + 11},{_tmp2CE,_tmp2CE,_tmp2CE + 15},{_tmp2CF,_tmp2CF,
_tmp2CF + 22},{_tmp2D0,_tmp2D0,_tmp2D0 + 16},{_tmp2D1,_tmp2D1,_tmp2D1 + 17},{
_tmp2D2,_tmp2D2,_tmp2D2 + 12},{_tmp2D3,_tmp2D3,_tmp2D3 + 18},{_tmp2D4,_tmp2D4,
_tmp2D4 + 17},{_tmp2D5,_tmp2D5,_tmp2D5 + 12},{_tmp2D6,_tmp2D6,_tmp2D6 + 16},{
_tmp2D7,_tmp2D7,_tmp2D7 + 11},{_tmp2D8,_tmp2D8,_tmp2D8 + 10},{_tmp2D9,_tmp2D9,
_tmp2D9 + 14},{_tmp2DA,_tmp2DA,_tmp2DA + 15},{_tmp2DB,_tmp2DB,_tmp2DB + 20},{
_tmp2DC,_tmp2DC,_tmp2DC + 27},{_tmp2DD,_tmp2DD,_tmp2DD + 10},{_tmp2DE,_tmp2DE,
_tmp2DE + 18},{_tmp2DF,_tmp2DF,_tmp2DF + 21},{_tmp2E0,_tmp2E0,_tmp2E0 + 19},{
_tmp2E1,_tmp2E1,_tmp2E1 + 16},{_tmp2E2,_tmp2E2,_tmp2E2 + 20},{_tmp2E3,_tmp2E3,
_tmp2E3 + 15},{_tmp2E4,_tmp2E4,_tmp2E4 + 16},{_tmp2E5,_tmp2E5,_tmp2E5 + 20},{
_tmp2E6,_tmp2E6,_tmp2E6 + 15},{_tmp2E7,_tmp2E7,_tmp2E7 + 8},{_tmp2E8,_tmp2E8,
_tmp2E8 + 19},{_tmp2E9,_tmp2E9,_tmp2E9 + 14},{_tmp2EA,_tmp2EA,_tmp2EA + 19},{
_tmp2EB,_tmp2EB,_tmp2EB + 11},{_tmp2EC,_tmp2EC,_tmp2EC + 22},{_tmp2ED,_tmp2ED,
_tmp2ED + 20},{_tmp2EE,_tmp2EE,_tmp2EE + 23},{_tmp2EF,_tmp2EF,_tmp2EF + 20},{
_tmp2F0,_tmp2F0,_tmp2F0 + 22},{_tmp2F1,_tmp2F1,_tmp2F1 + 23},{_tmp2F2,_tmp2F2,
_tmp2F2 + 24},{_tmp2F3,_tmp2F3,_tmp2F3 + 24},{_tmp2F4,_tmp2F4,_tmp2F4 + 15},{
_tmp2F5,_tmp2F5,_tmp2F5 + 20},{_tmp2F6,_tmp2F6,_tmp2F6 + 22},{_tmp2F7,_tmp2F7,
_tmp2F7 + 17},{_tmp2F8,_tmp2F8,_tmp2F8 + 20},{_tmp2F9,_tmp2F9,_tmp2F9 + 26},{
_tmp2FA,_tmp2FA,_tmp2FA + 16},{_tmp2FB,_tmp2FB,_tmp2FB + 17},{_tmp2FC,_tmp2FC,
_tmp2FC + 15},{_tmp2FD,_tmp2FD,_tmp2FD + 19},{_tmp2FE,_tmp2FE,_tmp2FE + 19},{
_tmp2FF,_tmp2FF,_tmp2FF + 25},{_tmp300,_tmp300,_tmp300 + 26},{_tmp301,_tmp301,
_tmp301 + 9},{_tmp302,_tmp302,_tmp302 + 20},{_tmp303,_tmp303,_tmp303 + 17},{_tmp304,
_tmp304,_tmp304 + 18},{_tmp305,_tmp305,_tmp305 + 11},{_tmp306,_tmp306,_tmp306 + 12}};
static short Cyc_yyr1[410]={0,127,128,128,128,128,128,128,128,129,129,130,130,130,
130,131,131,132,133,134,135,136,136,136,136,137,137,138,138,138,138,138,138,138,
138,139,139,139,139,139,139,139,140,140,141,142,142,143,143,143,143,143,144,144,
144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,145,146,146,
146,147,147,147,148,148,149,149,150,150,150,151,151,152,152,153,154,154,155,156,
156,157,157,158,159,159,159,159,160,161,161,162,162,162,163,163,163,163,164,164,
165,165,165,165,166,166,166,167,167,168,168,169,169,169,169,169,169,169,169,169,
169,169,170,170,170,170,171,171,171,171,171,172,172,173,173,174,174,175,175,175,
175,176,176,177,177,178,178,179,179,180,180,181,181,182,182,182,182,183,183,184,
184,185,185,185,186,187,187,188,188,189,189,189,189,190,190,190,190,191,192,192,
193,193,194,194,195,195,195,195,195,196,196,197,197,197,198,198,198,198,198,198,
198,198,198,198,198,199,199,199,199,199,199,199,199,199,199,199,199,199,200,201,
201,202,202,203,203,203,203,203,203,204,204,204,204,204,205,205,205,205,205,205,
206,206,206,206,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,208,
208,208,208,208,208,208,208,209,209,209,209,209,209,209,209,209,209,209,209,209,
210,210,211,211,212,212,213,213,214,214,215,215,215,215,215,215,215,215,215,215,
215,216,216,216,216,216,216,216,217,218,218,219,219,220,220,221,221,222,222,223,
223,223,224,224,224,224,224,225,225,225,226,226,226,227,227,227,227,228,228,229,
229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,230,230,230,231,231,
231,231,231,231,231,231,231,231,231,231,232,232,232,232,232,232,232,232,232,233,
234,234,235,235,235,235,236,236,237,237,238,238,239,239,240,240};static short Cyc_yyr2[
410]={0,1,2,3,5,3,5,6,0,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,1,2,2,3,2,3,2,3,2,3,1,1,
1,1,2,1,1,0,1,6,1,3,1,1,4,4,8,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,2,4,4,4,1,1,1,1,5,2,
4,1,3,1,3,4,7,3,0,3,1,1,1,1,2,1,1,3,1,3,3,2,3,2,3,1,1,3,1,2,3,6,4,3,5,1,1,1,2,3,
3,0,1,1,2,5,1,2,1,3,3,4,4,5,4,4,2,2,1,3,4,4,5,1,1,4,4,1,0,1,1,1,1,2,3,5,5,7,1,3,
0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,
2,1,2,3,4,3,1,3,1,1,2,3,2,3,3,4,4,3,5,4,4,2,1,1,1,1,1,1,6,3,9,6,5,2,2,3,1,2,2,3,
1,2,1,2,1,2,5,7,7,8,6,0,3,4,5,6,7,0,3,4,5,5,7,6,7,7,8,7,8,8,9,6,7,7,8,9,3,2,2,2,
3,2,4,5,1,3,1,2,1,1,1,1,4,4,5,2,2,1,3,1,2,1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,5,
2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,
2,2,2,4,2,6,6,5,4,6,9,11,1,1,1,1,4,3,4,3,3,2,2,6,7,4,4,1,1,1,3,2,5,4,5,5,1,1,3,1,
1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[843]={0,135,35,36,37,38,40,52,
55,56,57,58,59,60,61,62,72,73,74,87,88,42,0,0,41,0,0,112,113,0,0,400,155,402,401,
403,0,53,0,140,141,144,1,0,9,0,0,10,0,42,42,42,63,64,0,65,0,0,123,0,145,66,125,
85,39,0,33,43,0,76,287,400,283,286,285,0,281,0,0,0,0,0,178,0,288,17,19,0,0,0,0,0,
0,0,0,2,0,0,0,0,21,0,92,93,95,27,29,31,0,404,405,85,148,0,147,85,38,0,25,0,0,11,
157,0,0,133,124,42,146,134,0,67,0,0,34,80,0,78,0,0,0,293,284,292,24,0,0,85,0,0,0,
42,42,196,198,0,0,406,407,156,71,126,54,0,0,173,0,0,399,0,0,0,0,0,0,0,0,0,85,0,0,
396,386,397,398,0,0,0,0,371,0,369,370,0,302,315,323,325,327,329,331,333,336,341,
344,347,351,0,353,372,385,384,0,3,0,5,0,22,0,0,0,12,28,30,32,0,89,90,0,84,85,0,
110,39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,400,233,235,0,241,237,0,239,219,220,221,0,
222,223,224,0,300,26,13,95,163,179,0,0,159,157,0,203,0,127,0,136,0,0,0,0,77,0,0,
282,294,0,180,0,0,0,0,199,171,0,98,100,157,0,205,197,206,409,0,408,69,70,0,175,0,
177,68,0,0,361,0,317,351,0,318,319,0,0,0,0,0,0,0,0,0,354,355,0,0,0,0,357,358,356,
142,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,305,306,307,308,309,310,311,312,313,
314,304,0,359,0,378,379,0,0,0,388,0,85,143,18,0,20,0,94,96,182,181,14,82,91,0,0,
102,103,105,85,109,85,118,0,0,0,0,0,0,0,274,275,276,0,0,278,0,0,230,231,0,0,0,0,
0,0,242,238,95,240,236,234,0,164,0,0,0,170,158,165,129,0,0,0,159,131,0,132,128,
149,138,137,86,0,48,47,0,45,81,322,79,75,289,0,23,0,0,0,0,192,296,298,0,290,0,0,
200,99,101,0,159,0,209,0,207,157,0,0,218,202,174,0,0,0,183,187,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,387,394,0,393,324,0,326,328,330,332,334,335,339,340,337,338,342,343,
345,346,348,349,350,303,377,374,0,376,0,0,0,4,6,106,97,0,0,0,111,120,119,0,0,114,
0,0,0,0,0,0,0,0,273,277,0,0,0,0,226,0,0,0,232,0,15,301,157,0,167,0,0,0,160,130,
163,151,204,150,139,7,0,0,0,295,195,0,297,191,193,291,0,201,172,214,0,208,211,0,
159,0,210,0,360,0,0,184,0,188,382,383,0,365,0,0,0,0,0,0,0,0,352,390,0,0,375,373,
0,0,104,107,0,108,121,118,118,0,0,0,0,0,0,0,0,0,0,248,279,0,0,0,0,0,16,159,0,168,
166,0,159,0,0,0,0,44,46,194,299,213,216,0,217,212,0,0,0,185,189,0,0,0,0,320,321,
364,392,0,395,316,389,391,0,83,0,117,116,243,0,248,258,0,0,0,0,0,0,0,0,0,0,0,0,
280,0,0,0,0,229,153,169,161,152,157,0,49,50,215,363,362,0,190,366,0,0,380,0,0,0,
254,0,0,260,0,0,0,268,0,0,0,0,0,0,0,0,247,0,225,228,0,0,159,0,0,0,0,381,122,244,
0,0,0,245,259,261,262,0,270,269,0,264,0,0,0,0,248,249,0,0,162,154,0,0,0,0,0,0,
246,263,271,265,266,0,0,248,250,0,0,0,186,367,0,254,255,267,248,251,272,227,51,0,
254,256,248,252,368,257,253,0,0,0};static short Cyc_yydefgoto[114]={840,42,43,44,
254,45,388,46,390,47,220,48,49,66,67,460,461,50,159,51,52,135,136,53,131,54,225,
226,101,102,103,227,153,399,400,401,55,56,555,556,557,57,58,59,60,127,113,455,
483,61,484,446,586,438,442,443,299,275,165,81,82,499,393,500,501,472,473,154,277,
278,485,306,257,258,259,260,261,262,724,783,263,264,290,291,475,476,265,266,375,
194,464,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,516,517,210,
211,63,111,160,310};static short Cyc_yypact[843]={2036,- -32768,- -32768,- -32768,-
-32768,- 40,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 34,162,696,- -32768,40,96,
- -32768,- -32768,86,105,- -32768,137,- -32768,- -32768,- -32768,257,159,133,185,223,-
-32768,- -32768,1919,- -32768,208,297,- -32768,242,- 34,- 34,- 34,- -32768,- -32768,358,
- -32768,326,2512,216,24,41,- -32768,40,197,240,248,2639,- -32768,40,259,- -32768,
234,- -32768,- -32768,- -32768,797,- -32768,265,291,304,797,288,308,332,167,- -32768,
- -32768,4553,4553,205,340,205,1492,4247,4247,- -32768,1919,2152,1919,2152,- -32768,
361,366,- -32768,2447,2639,2639,2639,1492,- -32768,- -32768,197,- -32768,40,- -32768,
66,388,1306,- -32768,2512,242,- -32768,1854,4553,3143,- -32768,216,- 34,- -32768,-
-32768,4553,- -32768,2152,378,- -32768,381,393,410,40,403,797,- -32768,- -32768,-
-32768,- -32768,426,4247,197,797,418,44,- 34,- 34,295,- -32768,1,1,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,421,322,- -32768,4368,435,- -32768,4247,914,450,456,
462,465,470,478,487,197,4412,4412,- -32768,- -32768,- -32768,- -32768,1673,505,4489,
4489,- -32768,4489,- -32768,- -32768,430,- -32768,- 36,543,499,500,518,396,94,476,90,
152,- -32768,584,4489,179,- 19,- -32768,581,546,- -32768,548,- -32768,586,- -32768,257,
3187,2512,- -32768,- -32768,- -32768,- -32768,589,1492,- -32768,184,585,197,40,598,-
-32768,599,68,603,2723,606,616,611,612,3271,2723,9,610,2723,2723,101,615,608,-
-32768,- -32768,618,1430,1430,242,1430,- -32768,- -32768,- -32768,621,- -32768,-
-32768,- -32768,243,- -32768,- -32768,- -32768,613,630,- -32768,28,619,617,276,623,
609,32,- -32768,620,163,32,629,18,40,- -32768,4247,635,- -32768,- -32768,474,- -32768,
283,645,481,4553,- -32768,626,639,1492,1492,2385,3355,3,- -32768,405,- -32768,28,-
-32768,- -32768,- -32768,2276,- -32768,452,- -32768,- -32768,1492,1673,- -32768,1492,-
-32768,- -32768,2807,- -32768,667,4247,2214,4247,4247,4247,4247,4247,641,1673,-
-32768,- -32768,1430,642,495,4247,- -32768,- -32768,- -32768,- -32768,4489,4247,4489,
4489,4489,4489,4489,4489,4489,4489,4489,4489,4489,4489,4489,4489,4489,4489,4489,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,4247,- -32768,205,- -32768,- -32768,3399,205,4247,- -32768,637,197,- -32768,-
-32768,1919,- -32768,1919,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
4247,648,644,- -32768,650,197,- -32768,197,602,4247,657,4247,4247,724,1554,662,-
-32768,- -32768,- -32768,285,721,- -32768,3483,2577,- -32768,- -32768,2723,671,679,
4247,2723,1792,- -32768,- -32768,2447,- -32768,- -32768,- -32768,4247,- -32768,1492,
668,62,- -32768,- -32768,658,- -32768,682,673,4662,617,- -32768,4553,- -32768,- -32768,
- -32768,- -32768,163,- -32768,1919,- -32768,675,677,674,- -32768,- -32768,- -32768,-
-32768,- -32768,797,- -32768,205,4247,797,- 29,- -32768,- -32768,- -32768,151,- -32768,
56,688,- -32768,- -32768,- -32768,680,617,683,- -32768,678,405,4609,4553,3527,-
-32768,- -32768,- -32768,689,690,705,- -32768,- -32768,191,3187,506,693,257,508,695,
699,701,511,1492,694,700,4291,- -32768,- -32768,703,706,543,375,499,500,518,396,94,
94,476,476,476,476,90,90,152,152,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
704,- -32768,176,4553,3059,- -32768,- -32768,- -32768,- -32768,184,4247,1492,- -32768,
- -32768,- -32768,709,40,313,519,4247,521,523,710,3611,3695,330,- -32768,- -32768,
712,717,713,723,- -32768,707,708,527,- -32768,2512,- -32768,- -32768,726,4553,-
-32768,725,28,720,- -32768,- -32768,217,- -32768,609,- -32768,- -32768,- -32768,504,
727,18,- -32768,- -32768,718,- -32768,- -32768,- -32768,- -32768,645,- -32768,- -32768,-
-32768,728,- -32768,- -32768,729,617,32,- -32768,730,735,220,731,- -32768,2891,-
-32768,- -32768,- -32768,2512,- -32768,4247,805,4247,914,732,735,733,3059,- -32768,-
-32768,4247,4247,- -32768,- -32768,32,395,- -32768,- -32768,739,- -32768,738,50,602,
2723,529,745,2723,4247,3779,344,3823,346,3907,614,- -32768,748,4247,759,35,750,-
-32768,617,73,- -32768,- -32768,756,617,1492,747,751,769,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,770,- -32768,- -32768,771,772,4247,- -32768,- -32768,3187,536,
774,773,- -32768,667,- -32768,- -32768,400,- -32768,- -32768,- -32768,- -32768,3059,-
-32768,1492,- -32768,- -32768,852,780,614,- -32768,539,2723,541,3991,2723,547,4035,
4119,357,797,779,781,- -32768,368,2723,2723,789,- -32768,- -32768,- -32768,786,-
-32768,726,802,- -32768,- -32768,- -32768,- -32768,- -32768,409,- -32768,- -32768,1492,
870,- -32768,2975,549,2723,634,795,798,- -32768,2723,2723,553,- -32768,2723,2723,
566,2723,568,4163,- 5,1430,- -32768,4247,- -32768,- -32768,782,682,617,793,4247,808,
799,- -32768,- -32768,- -32768,4247,792,812,- -32768,- -32768,- -32768,- -32768,2723,-
-32768,- -32768,2723,- -32768,2723,2723,572,4247,1058,- -32768,579,817,- -32768,-
-32768,819,428,810,1492,453,1430,- -32768,- -32768,- -32768,- -32768,- -32768,2723,
455,614,- -32768,2723,2723,813,- -32768,- -32768,814,1182,- -32768,- -32768,1058,-
-32768,- -32768,- -32768,- -32768,815,634,- -32768,614,- -32768,- -32768,- -32768,-
-32768,921,924,- -32768};static short Cyc_yypgoto[114]={- -32768,- 10,- -32768,600,-
-32768,- -32768,- -32768,- -32768,- -32768,- 53,- 42,- 57,- -32768,104,- 45,329,- -32768,
100,835,- 85,- -32768,- -32768,- 114,- -32768,- 72,- -32768,377,- -32768,- -32768,- -32768,
711,716,2,- -32768,- -32768,382,- -32768,- -32768,46,- -32768,- -32768,6,- 41,- 119,-
-32768,- -32768,872,- -32768,- 102,- 3,- 105,- 407,161,347,352,636,- 394,- 91,- 296,816,-
-32768,- 205,- 164,- 516,- 275,- -32768,471,- 175,- 76,- 104,76,- 64,195,- -32768,- -32768,
- 26,- 225,- -32768,- 591,- 757,- -32768,- -32768,- 7,800,333,- -32768,166,148,- -32768,-
147,- 371,- 162,601,627,605,607,625,350,250,338,345,31,510,- -32768,- -32768,- -32768,
- 317,- -32768,- -32768,36,895,- -32768,- 280,- 151};static short Cyc_yytable[4771]={120,
164,152,152,118,311,324,152,325,134,338,155,156,125,392,119,83,274,126,471,273,
494,321,152,288,1,282,547,641,430,431,121,433,95,304,458,62,152,152,229,345,589,
90,232,314,152,583,120,222,223,224,118,64,114,104,553,554,114,69,84,256,85,120,
540,255,65,267,834,139,383,796,307,62,143,601,294,838,608,221,62,346,125,469,470,
62,606,213,214,215,216,109,439,115,268,163,62,384,538,129,599,152,541,307,570,
137,797,333,302,459,419,228,84,512,420,31,303,84,699,308,309,752,34,283,32,163,
728,269,451,36,440,31,456,62,62,62,62,32,34,450,32,463,152,580,495,729,496,297,
298,112,230,309,494,- 115,105,106,107,62,32,404,511,408,605,454,120,582,353,354,
267,62,313,448,462,409,308,137,130,84,231,732,643,16,17,18,304,84,1,86,151,151,
598,308,424,151,314,395,87,453,256,256,62,256,255,255,355,255,683,817,359,151,
360,425,152,356,426,88,152,152,152,341,342,478,343,151,151,269,828,89,152,228,
305,151,281,152,152,402,152,836,92,376,315,488,193,212,1,839,377,378,379,91,152,
488,463,31,603,62,300,301,604,1,34,731,492,432,68,62,734,361,405,147,441,504,148,
280,126,31,362,363,39,40,41,256,34,436,380,255,151,93,474,36,639,381,382,62,619,
398,157,621,620,39,40,41,158,481,482,163,441,130,437,96,97,157,686,293,544,163,
65,158,272,90,163,122,137,- 157,463,94,123,151,- 157,124,471,84,551,31,552,- 179,
630,592,687,164,34,- 179,132,100,435,152,691,36,31,62,436,62,339,133,152,34,120,
39,40,41,564,138,36,152,62,120,152,802,394,571,140,591,39,40,41,590,120,272,735,
545,118,546,141,447,468,612,567,613,611,144,436,577,436,534,535,536,142,151,98,
99,302,151,151,151,463,152,152,578,303,417,39,40,41,151,647,145,690,31,151,151,
648,151,32,1,34,62,152,62,316,317,691,658,163,411,112,151,146,436,298,418,640,
163,422,423,492,716,161,719,593,31,163,436,487,436,1,33,34,35,764,152,108,597,
217,684,436,600,695,152,696,768,351,352,394,218,691,436,667,298,506,507,508,233,
436,284,672,339,637,743,285,515,702,701,163,502,62,505,286,152,703,509,690,339,
704,747,84,65,668,748,84,289,489,624,31,519,287,490,436,292,491,34,775,120,- 176,
296,537,267,312,- 176,- 176,515,151,821,- 176,344,303,436,39,40,41,151,320,62,798,
31,690,634,357,358,151,542,34,151,666,- 205,228,326,402,312,- 205,- 205,436,327,436,
- 205,824,303,827,328,120,515,329,776,118,816,558,330,560,561,515,565,466,467,441,
331,825,579,62,477,467,152,151,151,332,646,575,674,675,676,474,221,833,514,436,
835,526,527,528,529,553,554,340,151,622,436,625,436,749,629,436,347,572,348,152,
349,576,649,436,651,436,652,436,773,823,665,436,710,436,722,723,350,615,84,744,
436,151,753,436,755,436,386,394,387,151,759,436,779,317,781,782,788,436,152,364,
365,366,367,368,369,370,371,372,373,791,436,793,436,163,319,814,436,322,322,151,
385,70,818,436,403,374,389,335,336,396,394,707,708,530,531,322,322,406,322,524,
525,407,532,533,413,410,163,256,412,414,415,255,421,765,219,322,428,427,437,152,
429,434,444,650,308,445,449,655,657,457,479,452,70,31,72,465,73,74,256,480,34,
345,255,510,543,513,548,75,549,256,76,77,562,255,469,470,84,78,550,559,566,568,
79,80,573,256,394,574,256,255,581,151,255,584,694,585,587,594,596,394,595,32,700,
607,71,72,609,73,74,610,692,34,616,618,617,322,623,631,75,626,632,76,77,627,151,
628,635,638,78,645,636,659,653,79,80,660,661,713,715,664,718,662,721,663,272,669,
726,671,693,679,677,681,682,70,633,697,698,394,688,705,706,685,709,151,711,712,
725,727,730,733,394,736,742,322,737,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,738,739,740,741,745,746,750,751,757,767,771,761,763,31,
72,766,73,74,772,774,34,777,394,784,785,803,800,75,808,806,76,77,151,819,322,754,
820,78,758,805,809,822,79,80,831,832,837,841,769,770,842,678,162,503,644,391,795,
642,128,801,799,673,670,680,276,166,167,804,397,602,493,780,518,807,295,110,786,
787,168,521,789,790,522,792,0,0,0,0,815,0,0,171,172,0,0,173,174,175,176,0,520,
523,0,0,0,178,322,179,180,810,0,0,811,0,812,813,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,
181,182,183,184,826,0,34,0,829,830,323,0,0,185,0,0,0,186,322,0,0,0,0,187,0,0,188,
0,189,190,0,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,322,2,3,4,116,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,722,723,21,166,167,234,0,235,236,237,238,
239,240,241,242,22,168,23,169,243,0,170,24,244,0,0,27,28,245,171,172,246,247,173,
174,175,176,29,30,248,177,0,249,178,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,322,0,0,0,
0,0,0,0,322,250,181,182,183,184,32,33,34,35,0,251,117,0,0,185,0,0,37,253,0,0,0,0,
0,187,0,0,188,0,189,190,0,0,0,0,191,192,2,3,4,116,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,781,782,21,166,167,234,0,235,236,237,238,239,240,241,242,22,168,23,
169,243,0,170,24,244,0,0,27,28,245,171,172,246,247,173,174,175,176,29,30,248,177,
0,249,178,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,250,181,182,183,
184,32,33,34,35,0,251,117,0,0,185,0,0,37,253,0,0,0,0,0,187,0,0,188,0,189,190,0,0,
0,0,191,192,2,3,4,116,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,166,167,
234,0,235,236,237,238,239,240,241,242,22,168,23,169,243,0,170,24,244,0,0,27,28,
245,171,172,246,247,173,174,175,176,29,30,248,177,0,249,178,0,179,180,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,250,181,182,183,184,32,33,34,35,0,251,117,252,0,
185,0,0,37,253,0,0,0,0,0,187,0,0,188,0,189,190,0,0,0,0,191,192,2,3,4,116,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,166,167,234,0,235,236,237,238,239,240,
241,242,22,168,23,169,243,0,170,24,244,0,0,27,28,245,171,172,246,247,173,174,175,
176,29,30,248,177,0,249,178,0,179,180,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,0,0,0,0,250,181,182,183,184,32,33,34,35,22,251,117,0,0,185,0,0,37,253,0,27,28,
0,0,187,0,0,188,0,189,190,29,30,0,0,191,192,2,3,4,116,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,166,167,0,0,0,0,0,0,32,33,0,35,22,168,23,169,0,0,170,24,37,
38,0,27,28,0,171,172,0,0,173,174,175,176,29,30,0,177,0,0,178,0,179,180,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,181,182,183,184,32,33,34,35,0,563,0,0,0,185,0,
0,37,253,0,0,0,0,0,187,0,0,188,0,189,190,0,0,0,0,191,192,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,166,167,0,0,0,0,0,0,0,0,0,0,22,168,0,169,0,0,170,0,0,0,0,27,
28,0,171,172,0,0,173,174,175,176,29,30,0,177,0,0,178,0,179,180,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,31,181,182,183,184,32,33,34,35,0,0,337,0,0,185,0,0,37,
253,0,0,0,0,0,187,0,0,188,0,189,190,0,0,0,0,191,192,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,166,167,0,0,0,0,0,0,0,0,0,0,22,168,0,169,0,0,170,0,0,0,0,27,28,
0,171,172,0,0,173,174,175,176,29,30,0,177,0,0,178,0,179,180,0,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,0,0,0,31,181,182,183,184,32,33,34,35,22,0,0,0,0,185,0,
0,37,253,0,27,28,0,0,187,0,0,188,0,189,190,29,30,0,0,191,192,- 8,1,0,2,3,4,5,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,270,0,21,271,0,0,0,0,32,33,0,35,0,272,0,22,
0,23,0,0,37,38,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,32,33,34,35,0,0,0,- 8,0,36,0,0,37,38,
0,0,0,0,0,39,40,41,- 8,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,
30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,32,33,34,
35,0,0,0,0,0,36,0,0,37,38,0,0,0,0,0,39,40,41,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,
28,0,0,0,0,0,0,0,0,0,29,30,0,0,1,0,2,3,4,116,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,0,0,0,31,0,0,0,0,32,33,34,35,22,0,0,- 8,0,36,0,24,37,38,0,27,28,0,0,
39,40,41,0,0,0,0,29,30,0,0,1,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,0,0,0,0,31,0,0,0,0,32,33,34,35,22,0,0,0,0,36,0,0,37,38,0,27,28,0,0,39,40,41,
0,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,270,0,0,31,0,0,
0,0,32,33,34,35,0,272,0,0,0,312,- 157,0,37,38,0,- 157,0,303,0,39,40,41,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,
27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,2,3,4,116,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,270,0,0,0,0,0,0,0,32,33,0,35,22,272,23,0,0,302,0,24,37,38,0,27,
28,303,0,39,40,41,0,0,0,0,29,30,0,0,0,0,0,0,0,2,3,4,116,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,21,0,0,0,0,0,32,33,0,35,0,0,117,22,219,23,0,0,37,38,24,0,0,
0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,0,2,3,4,116,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,21,0,0,0,0,0,32,33,0,35,0,0,117,22,0,23,0,0,37,38,24,0,0,0,
27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,2,3,4,116,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,0,0,0,0,0,0,0,0,32,33,0,35,22,0,0,0,0,0,0,24,37,38,0,27,28,0,0,0,
0,0,0,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,32,33,0,35,0,0,0,0,0,0,0,0,37,38,166,167,234,0,235,236,237,238,239,240,241,
242,0,168,0,169,243,0,170,0,244,0,0,0,0,245,171,172,246,247,173,174,175,176,0,0,
248,177,0,249,178,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,250,181,
182,183,184,0,0,34,0,0,251,117,0,0,185,0,0,0,186,166,167,0,0,0,187,0,497,188,0,
189,190,0,168,0,169,191,192,170,0,0,0,0,0,0,0,171,172,0,0,173,174,175,176,0,0,0,
177,0,0,178,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,181,182,183,
184,0,0,34,0,0,0,323,498,0,185,0,0,0,186,166,167,469,470,0,187,0,0,188,0,189,190,
0,168,0,169,191,192,170,0,0,0,0,0,0,0,171,172,0,0,173,174,175,176,0,0,0,177,0,0,
178,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,181,182,183,184,0,0,
34,0,0,0,323,689,0,185,0,0,0,186,166,167,469,470,0,187,0,0,188,0,189,190,0,168,0,
169,191,192,170,0,0,0,0,0,0,0,171,172,0,0,173,174,175,176,0,0,0,177,0,0,178,0,
179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,181,182,183,184,0,0,34,0,0,
0,323,778,0,185,0,0,0,186,166,167,469,470,0,187,0,0,188,0,189,190,0,168,0,169,
191,192,170,0,0,0,0,0,0,0,171,172,0,0,173,174,175,176,0,0,0,177,0,0,178,0,179,
180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,181,182,183,184,0,0,34,0,0,0,
323,0,0,185,0,0,0,186,166,167,469,470,0,187,0,0,188,0,189,190,0,168,0,169,191,
192,170,0,0,0,0,0,0,0,171,172,0,0,173,174,175,176,0,0,0,177,0,0,178,0,179,180,
166,167,0,0,0,0,0,0,0,0,0,0,0,168,0,169,0,0,170,0,0,31,181,182,183,184,171,172,
34,0,173,174,175,176,0,185,0,177,0,186,178,0,179,180,279,187,0,0,188,0,189,190,0,
0,0,0,191,192,0,0,0,0,0,0,0,31,181,182,183,184,0,0,34,0,0,0,323,0,0,185,0,0,0,
186,166,167,0,0,0,187,0,0,188,0,189,190,0,168,0,169,191,192,170,0,0,0,0,0,0,0,
171,172,0,0,173,174,175,176,0,0,0,177,0,0,178,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,31,181,182,183,184,0,0,34,0,0,416,0,0,0,185,0,0,0,186,166,167,
0,0,0,187,0,0,188,0,189,190,0,168,0,169,191,192,170,0,0,0,0,0,0,0,171,172,0,0,
173,174,175,176,0,0,0,177,0,0,178,0,179,180,166,167,0,0,0,0,0,0,0,0,0,0,0,168,0,
169,0,0,170,0,0,31,181,182,183,184,171,172,34,0,173,174,175,176,0,185,0,177,0,
186,178,0,179,180,486,187,0,0,188,0,189,190,0,0,0,0,191,192,0,0,0,0,0,0,0,31,181,
182,183,184,0,0,34,0,0,0,0,0,0,185,539,0,0,186,166,167,0,0,0,187,0,0,188,0,189,
190,0,168,0,169,191,192,170,0,0,0,0,0,0,0,171,172,0,0,173,174,175,176,0,0,0,177,
0,0,178,0,179,180,166,167,0,0,0,0,0,0,0,0,0,0,0,168,0,169,0,0,170,0,0,31,181,182,
183,184,171,172,34,0,173,174,175,176,0,185,569,177,0,186,178,0,179,180,0,187,0,0,
188,0,189,190,0,0,0,0,191,192,0,0,0,0,0,0,0,31,181,182,183,184,0,0,34,0,0,0,0,0,
0,185,0,0,0,186,166,167,0,0,614,187,0,0,188,0,189,190,0,168,0,169,191,192,170,0,
0,0,0,0,0,0,171,172,0,0,173,174,175,176,0,0,0,177,0,0,178,0,179,180,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,181,182,183,184,0,0,34,0,0,654,0,0,0,185,0,0,0,
186,166,167,0,0,0,187,0,0,188,0,189,190,0,168,0,169,191,192,170,0,0,0,0,0,0,0,
171,172,0,0,173,174,175,176,0,0,0,177,0,0,178,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,31,181,182,183,184,0,0,34,0,0,656,0,0,0,185,0,0,0,186,166,167,
0,0,0,187,0,0,188,0,189,190,0,168,0,169,191,192,170,0,0,0,0,0,0,0,171,172,0,0,
173,174,175,176,0,0,0,177,0,0,178,0,179,180,166,167,0,0,0,0,0,0,0,0,0,0,0,168,0,
169,0,0,170,0,0,31,181,182,183,184,171,172,34,0,173,174,175,176,0,185,714,177,0,
186,178,0,179,180,0,187,0,0,188,0,189,190,0,0,0,0,191,192,0,0,0,0,0,0,0,31,181,
182,183,184,0,0,34,0,0,0,0,0,0,185,717,0,0,186,166,167,0,0,0,187,0,0,188,0,189,
190,0,168,0,169,191,192,170,0,0,0,0,0,0,0,171,172,0,0,173,174,175,176,0,0,0,177,
0,0,178,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,181,182,183,184,0,
0,34,0,0,720,0,0,0,185,0,0,0,186,166,167,0,0,0,187,0,0,188,0,189,190,0,168,0,169,
191,192,170,0,0,0,0,0,0,0,171,172,0,0,173,174,175,176,0,0,0,177,0,0,178,0,179,
180,166,167,0,0,0,0,0,0,0,0,0,0,0,168,0,169,0,0,170,0,0,31,181,182,183,184,171,
172,34,0,173,174,175,176,0,185,756,177,0,186,178,0,179,180,0,187,0,0,188,0,189,
190,0,0,0,0,191,192,0,0,0,0,0,0,0,31,181,182,183,184,0,0,34,0,0,0,0,0,0,185,760,
0,0,186,166,167,0,0,0,187,0,0,188,0,189,190,0,168,0,169,191,192,170,0,0,0,0,0,0,
0,171,172,0,0,173,174,175,176,0,0,0,177,0,0,178,0,179,180,166,167,0,0,0,0,0,0,0,
0,0,0,0,168,0,169,0,0,170,0,0,31,181,182,183,184,171,172,34,0,173,174,175,176,0,
185,762,177,0,186,178,0,179,180,0,187,0,0,188,0,189,190,0,0,0,0,191,192,0,0,0,0,
0,0,0,31,181,182,183,184,0,0,34,0,0,0,0,0,0,185,794,0,0,186,166,167,0,0,0,187,0,
0,188,0,189,190,0,168,0,169,191,192,170,0,0,0,0,0,0,0,171,172,0,0,173,174,175,
176,0,0,0,177,0,0,178,0,179,180,166,167,0,0,0,0,0,0,0,0,0,0,0,168,0,0,0,0,0,0,0,
31,181,182,183,184,171,172,34,0,173,174,175,176,0,185,0,0,0,186,178,0,179,180,0,
187,0,0,188,0,189,190,0,0,0,0,191,192,0,0,0,0,0,0,0,31,181,182,183,184,0,0,34,0,
0,0,633,166,167,185,0,0,0,186,0,0,0,0,0,187,168,0,188,0,189,190,0,0,0,0,191,192,
0,171,172,0,0,173,174,175,176,0,0,0,0,0,0,178,0,179,180,166,167,0,0,0,0,0,0,0,0,
0,0,0,168,0,0,0,0,0,0,0,31,181,182,183,184,171,172,34,0,173,174,175,176,0,318,0,
0,0,186,178,0,179,180,0,187,0,0,188,0,189,190,0,0,0,0,191,192,0,0,0,0,0,0,0,31,
181,182,183,184,0,0,34,0,0,0,0,166,167,334,0,0,0,186,0,0,0,0,0,187,168,0,188,0,
189,190,0,0,0,0,191,192,0,171,172,0,0,173,174,175,176,0,0,0,0,0,0,178,0,179,180,
0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,31,181,182,183,184,0,0,34,0,
0,0,22,0,0,185,0,0,0,186,0,0,0,27,28,187,0,0,188,0,189,190,0,0,29,30,191,192,149,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,32,33,
0,35,0,0,150,0,0,27,28,0,37,38,0,0,0,0,0,0,29,30,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,270,0,0,22,0,0,0,0,32,33,0,35,0,272,27,28,0,0,0,
0,37,38,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,588,0,0,
0,0,0,0,0,32,33,0,35,0,0,0,0,0,0,0,0,37,38};static short Cyc_yycheck[4771]={57,92,
87,88,57,156,170,92,170,66,185,87,88,58,219,57,23,122,59,294,122,317,169,108,138,
1,130,398,544,254,255,57,257,43,153,17,0,122,123,111,76,448,36,115,163,130,440,
104,105,106,107,104,92,56,48,5,6,60,22,23,117,25,119,380,117,99,119,824,75,88,75,
70,36,80,103,147,833,484,104,43,116,126,111,112,48,479,96,97,98,99,54,63,56,119,
92,59,115,377,62,470,185,381,70,420,68,110,178,104,90,100,108,75,337,104,90,112,
80,633,117,118,711,97,132,95,122,90,120,278,104,101,90,282,96,97,98,99,95,97,106,
95,287,226,438,318,109,320,102,150,107,113,118,447,102,49,50,51,120,95,230,334,
92,105,281,220,102,71,72,220,132,163,275,285,104,117,138,109,140,111,105,550,17,
18,19,302,148,1,90,87,88,469,117,90,92,312,220,109,281,254,255,163,257,254,255,
109,257,612,797,117,108,119,109,296,118,112,109,300,301,302,187,188,296,190,122,
123,218,816,89,312,226,153,130,127,317,318,228,320,827,104,207,163,304,93,94,1,
835,66,67,68,89,334,314,398,90,102,218,151,152,106,1,97,667,306,256,101,228,672,
114,231,101,272,327,104,124,314,90,123,124,114,115,116,337,97,106,104,337,185,
101,294,104,113,111,112,256,102,110,90,501,106,114,115,116,96,300,301,302,308,
109,90,100,101,90,91,146,385,312,99,96,100,312,317,104,285,105,470,101,109,226,
110,112,604,294,403,90,405,100,510,455,617,429,97,106,101,100,100,429,620,104,90,
312,106,314,185,104,438,97,412,114,115,116,412,101,104,447,327,421,450,773,219,
421,104,455,114,115,116,450,432,100,673,388,432,390,90,106,100,489,100,490,489,
100,106,432,106,361,362,363,91,296,100,101,104,300,301,302,550,489,490,432,112,
242,114,115,116,312,100,106,620,90,317,318,106,320,95,1,97,388,510,390,105,106,
704,100,429,237,107,334,103,106,440,243,543,438,246,247,488,100,105,100,457,90,
447,106,303,106,1,96,97,98,100,543,101,467,100,613,106,471,629,551,629,100,73,74,
323,106,748,106,580,479,329,330,331,92,106,104,588,318,110,691,106,340,640,637,
489,326,457,328,102,581,102,332,704,334,106,102,467,99,581,106,471,105,104,504,
90,346,103,109,106,90,112,97,110,577,100,104,375,577,104,105,106,380,429,102,110,
102,112,106,114,115,116,438,104,504,766,90,748,513,69,70,447,382,97,450,577,100,
551,104,549,104,105,106,106,104,106,110,110,112,110,104,624,420,104,745,624,797,
407,104,409,410,429,412,105,106,584,104,808,436,549,105,106,673,489,490,104,556,
427,90,91,92,604,624,824,105,106,827,353,354,355,356,5,6,104,510,105,106,105,106,
706,105,106,75,424,121,706,122,428,105,106,105,106,105,106,735,806,105,106,105,
106,22,23,120,491,604,105,106,543,105,106,105,106,102,501,102,551,105,106,105,
106,22,23,105,106,745,77,78,79,80,81,82,83,84,85,86,105,106,105,106,673,166,105,
106,169,170,581,101,38,105,106,101,103,102,179,180,102,544,647,648,357,358,187,
188,101,190,351,352,104,359,360,90,104,706,766,104,100,100,766,104,722,103,207,
110,104,90,806,104,102,105,559,117,110,105,563,564,102,106,113,38,90,91,102,93,
94,797,102,97,76,797,104,109,105,100,104,106,808,107,108,30,808,111,112,722,114,
110,104,100,42,119,120,95,824,620,90,827,824,104,673,827,117,628,95,105,104,106,
633,105,95,636,105,90,91,105,93,94,113,626,97,105,90,106,287,105,105,104,106,102,
107,108,106,706,106,105,105,114,102,106,101,104,119,120,100,105,653,654,113,656,
100,658,118,100,102,662,109,25,113,105,105,105,38,101,105,105,691,109,102,104,
113,649,745,101,652,100,90,100,95,704,106,688,345,105,347,348,349,350,351,352,
353,354,355,356,357,358,359,360,361,362,363,105,105,105,105,104,106,28,101,716,
102,95,719,720,90,91,110,93,94,106,91,97,25,748,102,100,106,118,104,110,104,107,
108,806,90,398,714,91,114,717,105,102,105,119,120,105,105,105,0,727,728,0,596,91,
327,551,218,764,549,60,772,768,588,584,604,122,25,26,775,226,472,308,750,345,781,
148,54,755,756,38,348,759,760,349,762,- 1,- 1,- 1,- 1,796,- 1,- 1,51,52,- 1,- 1,55,56,57,
58,- 1,347,350,- 1,- 1,- 1,65,470,67,68,788,- 1,- 1,791,- 1,793,794,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,814,- 1,97,- 1,818,819,101,- 1,- 1,104,- 1,- 1,-
1,108,513,- 1,- 1,- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,- 1,- 1,- 1,125,126,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,550,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,
43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,- 1,64,65,- 1,67,68,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,629,- 1,- 1,- 1,- 1,- 1,- 1,- 1,637,90,91,92,93,94,95,
96,97,98,- 1,100,101,- 1,- 1,104,- 1,- 1,107,108,- 1,- 1,- 1,- 1,- 1,114,- 1,- 1,117,- 1,119,
120,- 1,- 1,- 1,- 1,125,126,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,
24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,
51,52,53,54,55,56,57,58,59,60,61,62,- 1,64,65,- 1,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,95,96,97,98,- 1,100,101,- 1,- 1,
104,- 1,- 1,107,108,- 1,- 1,- 1,- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,- 1,- 1,- 1,125,126,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,
33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,
60,61,62,- 1,64,65,- 1,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,90,91,92,93,94,95,96,97,98,- 1,100,101,102,- 1,104,- 1,- 1,107,108,- 1,- 1,- 1,
- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,- 1,- 1,- 1,125,126,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,
- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,- 1,64,65,- 1,67,68,
- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,95,
96,97,98,37,100,101,- 1,- 1,104,- 1,- 1,107,108,- 1,48,49,- 1,- 1,114,- 1,- 1,117,- 1,119,
120,59,60,- 1,- 1,125,126,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,95,96,- 1,98,37,38,39,40,- 1,- 1,43,44,107,108,- 1,48,49,
- 1,51,52,- 1,- 1,55,56,57,58,59,60,- 1,62,- 1,- 1,65,- 1,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,95,96,97,98,- 1,100,- 1,- 1,-
1,104,- 1,- 1,107,108,- 1,- 1,- 1,- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,- 1,- 1,- 1,125,126,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,- 1,
62,- 1,- 1,65,- 1,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,90,91,92,93,94,95,96,97,98,- 1,- 1,101,- 1,- 1,104,- 1,- 1,107,108,- 1,- 1,- 1,- 1,- 1,
114,- 1,- 1,117,- 1,119,120,- 1,- 1,- 1,- 1,125,126,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,
- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,- 1,62,- 1,- 1,65,- 1,67,68,- 1,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,95,96,97,98,37,- 1,
- 1,- 1,- 1,104,- 1,- 1,107,108,- 1,48,49,- 1,- 1,114,- 1,- 1,117,- 1,119,120,59,60,- 1,- 1,
125,126,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,87,- 1,24,90,- 1,
- 1,- 1,- 1,95,96,- 1,98,- 1,100,- 1,37,- 1,39,- 1,- 1,107,108,44,- 1,46,47,48,49,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,- 1,- 1,95,96,97,98,- 1,- 1,- 1,102,- 1,104,- 1,
- 1,107,108,- 1,- 1,- 1,- 1,- 1,114,115,116,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,
44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,- 1,- 1,95,96,97,
98,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,107,108,- 1,- 1,- 1,- 1,- 1,114,115,116,1,- 1,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,
1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,90,- 1,-
1,- 1,- 1,95,96,97,98,37,- 1,- 1,102,- 1,104,- 1,44,107,108,- 1,48,49,- 1,- 1,114,115,116,
- 1,- 1,- 1,- 1,59,60,- 1,- 1,1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,- 1,- 1,95,96,97,98,37,- 1,- 1,- 1,- 1,104,- 1,- 1,107,108,
- 1,48,49,- 1,- 1,114,115,116,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,- 1,- 1,- 1,- 1,95,96,97,98,- 1,
100,- 1,- 1,- 1,104,105,- 1,107,108,- 1,110,- 1,112,- 1,114,115,116,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,- 1,
98,37,100,39,- 1,- 1,104,- 1,44,107,108,- 1,48,49,112,- 1,114,115,116,- 1,- 1,- 1,- 1,59,
60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,- 1,- 1,- 1,- 1,- 1,95,96,- 1,98,- 1,- 1,101,37,103,39,- 1,- 1,107,108,44,- 1,- 1,- 1,48,
49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,95,96,- 1,98,- 1,- 1,101,37,- 1,
39,- 1,- 1,107,108,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,95,96,- 1,98,37,- 1,- 1,- 1,- 1,- 1,- 1,44,107,108,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,25,26,
27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,43,- 1,45,- 1,- 1,- 1,- 1,50,51,52,53,
54,55,56,57,58,- 1,- 1,61,62,- 1,64,65,- 1,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,- 1,97,- 1,- 1,100,101,- 1,- 1,104,- 1,- 1,
- 1,108,25,26,- 1,- 1,- 1,114,- 1,32,117,- 1,119,120,- 1,38,- 1,40,125,126,43,- 1,- 1,- 1,-
1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,- 1,65,- 1,67,68,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,- 1,97,- 1,- 1,- 1,
101,102,- 1,104,- 1,- 1,- 1,108,25,26,111,112,- 1,114,- 1,- 1,117,- 1,119,120,- 1,38,- 1,
40,125,126,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,- 1,65,
- 1,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,
93,94,- 1,- 1,97,- 1,- 1,- 1,101,102,- 1,104,- 1,- 1,- 1,108,25,26,111,112,- 1,114,- 1,- 1,
117,- 1,119,120,- 1,38,- 1,40,125,126,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,
58,- 1,- 1,- 1,62,- 1,- 1,65,- 1,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,- 1,97,- 1,- 1,- 1,101,102,- 1,104,- 1,- 1,- 1,108,25,
26,111,112,- 1,114,- 1,- 1,117,- 1,119,120,- 1,38,- 1,40,125,126,43,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,- 1,65,- 1,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,- 1,97,- 1,- 1,- 1,101,- 1,-
1,104,- 1,- 1,- 1,108,25,26,111,112,- 1,114,- 1,- 1,117,- 1,119,120,- 1,38,- 1,40,125,126,
43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,- 1,65,- 1,67,68,25,
26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,43,- 1,- 1,90,91,92,93,94,51,52,
97,- 1,55,56,57,58,- 1,104,- 1,62,- 1,108,65,- 1,67,68,113,114,- 1,- 1,117,- 1,119,120,-
1,- 1,- 1,- 1,125,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,- 1,97,- 1,- 1,- 1,101,- 1,
- 1,104,- 1,- 1,- 1,108,25,26,- 1,- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,38,- 1,40,125,126,
43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,- 1,65,- 1,67,68,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,- 1,
97,- 1,- 1,100,- 1,- 1,- 1,104,- 1,- 1,- 1,108,25,26,- 1,- 1,- 1,114,- 1,- 1,117,- 1,119,120,-
1,38,- 1,40,125,126,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,-
1,- 1,65,- 1,67,68,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,43,- 1,- 1,
90,91,92,93,94,51,52,97,- 1,55,56,57,58,- 1,104,- 1,62,- 1,108,65,- 1,67,68,113,114,-
1,- 1,117,- 1,119,120,- 1,- 1,- 1,- 1,125,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,-
1,97,- 1,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,108,25,26,- 1,- 1,- 1,114,- 1,- 1,117,- 1,119,120,
- 1,38,- 1,40,125,126,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,
- 1,- 1,65,- 1,67,68,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,43,- 1,- 1,
90,91,92,93,94,51,52,97,- 1,55,56,57,58,- 1,104,105,62,- 1,108,65,- 1,67,68,- 1,114,-
1,- 1,117,- 1,119,120,- 1,- 1,- 1,- 1,125,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,-
1,97,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,- 1,108,25,26,- 1,- 1,113,114,- 1,- 1,117,- 1,119,120,
- 1,38,- 1,40,125,126,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,
- 1,- 1,65,- 1,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
90,91,92,93,94,- 1,- 1,97,- 1,- 1,100,- 1,- 1,- 1,104,- 1,- 1,- 1,108,25,26,- 1,- 1,- 1,114,-
1,- 1,117,- 1,119,120,- 1,38,- 1,40,125,126,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,
56,57,58,- 1,- 1,- 1,62,- 1,- 1,65,- 1,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,- 1,97,- 1,- 1,100,- 1,- 1,- 1,104,- 1,- 1,- 1,108,
25,26,- 1,- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,38,- 1,40,125,126,43,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,- 1,65,- 1,67,68,25,26,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,43,- 1,- 1,90,91,92,93,94,51,52,97,- 1,55,56,57,58,- 1,
104,105,62,- 1,108,65,- 1,67,68,- 1,114,- 1,- 1,117,- 1,119,120,- 1,- 1,- 1,- 1,125,126,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,108,25,
26,- 1,- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,38,- 1,40,125,126,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,- 1,65,- 1,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,- 1,97,- 1,- 1,100,- 1,- 1,- 1,
104,- 1,- 1,- 1,108,25,26,- 1,- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,38,- 1,40,125,126,43,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,- 1,65,- 1,67,68,25,26,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,43,- 1,- 1,90,91,92,93,94,51,52,97,
- 1,55,56,57,58,- 1,104,105,62,- 1,108,65,- 1,67,68,- 1,114,- 1,- 1,117,- 1,119,120,- 1,-
1,- 1,- 1,125,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,
104,105,- 1,- 1,108,25,26,- 1,- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,38,- 1,40,125,126,43,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,- 1,65,- 1,67,68,25,26,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,43,- 1,- 1,90,91,92,93,94,51,52,97,
- 1,55,56,57,58,- 1,104,105,62,- 1,108,65,- 1,67,68,- 1,114,- 1,- 1,117,- 1,119,120,- 1,-
1,- 1,- 1,125,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,
104,105,- 1,- 1,108,25,26,- 1,- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,38,- 1,40,125,126,43,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,- 1,65,- 1,67,68,25,26,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,51,52,97,
- 1,55,56,57,58,- 1,104,- 1,- 1,- 1,108,65,- 1,67,68,- 1,114,- 1,- 1,117,- 1,119,120,- 1,- 1,
- 1,- 1,125,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,- 1,97,- 1,- 1,- 1,101,25,26,
104,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,114,38,- 1,117,- 1,119,120,- 1,- 1,- 1,- 1,125,126,- 1,
51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,65,- 1,67,68,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,51,52,97,- 1,55,56,57,58,- 1,
104,- 1,- 1,- 1,108,65,- 1,67,68,- 1,114,- 1,- 1,117,- 1,119,120,- 1,- 1,- 1,- 1,125,126,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,- 1,97,- 1,- 1,- 1,- 1,25,26,104,- 1,- 1,- 1,108,- 1,
- 1,- 1,- 1,- 1,114,38,- 1,117,- 1,119,120,- 1,- 1,- 1,- 1,125,126,- 1,51,52,- 1,- 1,55,56,57,
58,- 1,- 1,- 1,- 1,- 1,- 1,65,- 1,67,68,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,- 1,- 1,90,91,92,93,94,- 1,- 1,97,- 1,- 1,- 1,37,- 1,- 1,104,- 1,- 1,- 1,108,- 1,- 1,
- 1,48,49,114,- 1,- 1,117,- 1,119,120,- 1,- 1,59,60,125,126,63,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,95,96,- 1,98,
- 1,- 1,101,- 1,- 1,48,49,- 1,107,108,- 1,- 1,- 1,- 1,- 1,- 1,59,60,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,37,- 1,- 1,- 1,- 1,95,
96,- 1,98,- 1,100,48,49,- 1,- 1,- 1,- 1,107,108,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,
96,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108};char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible";
char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow";void Cyc_yyerror(
struct _tagged_arr);int Cyc_yylex();static int Cyc_yychar=(int)'\000';void*Cyc_yylval=(
void*)Cyc_YYINITIALSVAL;static int Cyc_yynerrs=0;struct _tuple16{struct _tagged_arr
f1;void*f2;};static char _tmp38E[8]="stdcall";static char _tmp38F[6]="cdecl";static
char _tmp390[9]="fastcall";static char _tmp391[9]="noreturn";static char _tmp392[6]="const";
static char _tmp393[8]="aligned";static char _tmp394[7]="packed";static char _tmp395[7]="shared";
static char _tmp396[7]="unused";static char _tmp397[5]="weak";static char _tmp398[10]="dllimport";
static char _tmp399[10]="dllexport";static char _tmp39A[23]="no_instrument_function";
static char _tmp39B[12]="constructor";static char _tmp39C[11]="destructor";static
char _tmp39D[22]="no_check_memory_usage";struct _tuple17{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(){int yystate;int yyn=0;int yyerrstatus;int
yychar1=0;int yyssp_offset;short*yyss=(short*)({unsigned int _tmp85F=10000;short*
_tmp860=(short*)_cycalloc_atomic(_check_times(sizeof(short),_tmp85F));{
unsigned int _tmp861=_tmp85F;unsigned int i;for(i=0;i < _tmp861;i ++){_tmp860[i]=(
short)0;}}_tmp860;});int yyvsp_offset;void**yyvs=(void**)({unsigned int _tmp85C=
10000;void**_tmp85D=(void**)_cycalloc(_check_times(sizeof(void*),_tmp85C));{
unsigned int _tmp85E=_tmp85C;unsigned int i;for(i=0;i < _tmp85E;i ++){_tmp85D[i]=Cyc_yylval;}}
_tmp85D;});int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({
unsigned int _tmp859=10000;struct Cyc_Yyltype*_tmp85A=(struct Cyc_Yyltype*)
_cycalloc_atomic(_check_times(sizeof(struct Cyc_Yyltype),_tmp859));{unsigned int
_tmp85B=_tmp859;unsigned int i;for(i=0;i < _tmp85B;i ++){_tmp85A[i]=Cyc_yynewloc();}}
_tmp85A;});int yystacksize=10000;void*yyval=Cyc_yylval;int yylen;yystate=0;
yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=0;
yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){Cyc_yyerror(_tag_arr("parser stack overflow",sizeof(char),22));(int)
_throw((void*)Cyc_Yystack_overflow);}}goto yybackup;yybackup: yyn=(int)Cyc_yypact[
yystate];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex();
if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=(Cyc_yychar > 0?Cyc_yychar
<= 354: 0)?(int)Cyc_yytranslate[_check_known_subscript_notnull(355,Cyc_yychar)]:
241;}yyn +=yychar1;if((yyn < 0?1: yyn > 4770)?1: Cyc_yycheck[yyn]!= yychar1)goto
yydefault;yyn=(int)Cyc_yytable[yyn];if(yyn < 0){if(yyn == - 32768)goto yyerrlab;yyn=
- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 842)return 0;if(Cyc_yychar
!= 0)Cyc_yychar=- 2;yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=
Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;
if(yyerrstatus != 0)yyerrstatus --;yystate=yyn;goto yynewstate;yydefault: yyn=(int)
Cyc_yydefact[yystate];if(yyn == 0)goto yyerrlab;yyreduce: yylen=(int)Cyc_yyr2[yyn];
if(yylen > 0)yyval=yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- 
yylen)];switch(yyn){case 1: _LL1BE: yyval=yyvs[yyvsp_offset];Cyc_Parse_parse_result=
Cyc_yyget_DeclList_tok(yyvs[yyvsp_offset]);break;case 2: _LL1BF: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp307=_cycalloc(sizeof(*_tmp307));_tmp307[0]=({
struct Cyc_DeclList_tok_struct _tmp308;_tmp308.tag=Cyc_DeclList_tok;_tmp308.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_yyget_DeclList_tok(yyvs[yyvsp_offset]));_tmp308;});_tmp307;});break;case 3:
_LL1C0: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp309=_cycalloc(sizeof(*
_tmp309));_tmp309[0]=({struct Cyc_DeclList_tok_struct _tmp30A;_tmp30A.tag=Cyc_DeclList_tok;
_tmp30A.f1=({struct Cyc_List_List*_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B->hd=({
struct Cyc_Absyn_Decl*_tmp30C=_cycalloc(sizeof(*_tmp30C));_tmp30C->r=(void*)((
void*)({struct Cyc_Absyn_Using_d_struct*_tmp30D=_cycalloc(sizeof(*_tmp30D));
_tmp30D[0]=({struct Cyc_Absyn_Using_d_struct _tmp30E;_tmp30E.tag=9;_tmp30E.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp30E.f2=Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]);_tmp30E;});_tmp30D;}));_tmp30C->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp30C;});_tmp30B->tl=0;_tmp30B;});_tmp30A;});_tmp309;});
Cyc_Lex_leave_using();break;case 4: _LL1C1: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F[0]=({struct Cyc_DeclList_tok_struct
_tmp310;_tmp310.tag=Cyc_DeclList_tok;_tmp310.f1=({struct Cyc_List_List*_tmp311=
_cycalloc(sizeof(*_tmp311));_tmp311->hd=({struct Cyc_Absyn_Decl*_tmp312=_cycalloc(
sizeof(*_tmp312));_tmp312->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313[0]=({struct Cyc_Absyn_Using_d_struct
_tmp314;_tmp314.tag=9;_tmp314.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp314.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp314;});_tmp313;}));
_tmp312->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp312;});_tmp311->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp311;});_tmp310;});_tmp30F;});break;case 5: _LL1C2: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp315=_cycalloc(sizeof(*_tmp315));_tmp315[0]=({
struct Cyc_DeclList_tok_struct _tmp316;_tmp316.tag=Cyc_DeclList_tok;_tmp316.f1=({
struct Cyc_List_List*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317->hd=({struct Cyc_Absyn_Decl*
_tmp318=_cycalloc(sizeof(*_tmp318));_tmp318->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp319=_cycalloc(sizeof(*_tmp319));_tmp319[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp31A;_tmp31A.tag=8;_tmp31A.f1=({struct _tagged_arr*_tmp31B=_cycalloc(sizeof(*
_tmp31B));_tmp31B[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp31B;});_tmp31A.f2=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp31A;});_tmp319;}));_tmp318->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp318;});_tmp317->tl=0;_tmp317;});_tmp316;});_tmp315;});
Cyc_Lex_leave_namespace();break;case 6: _LL1C3: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C[0]=({struct Cyc_DeclList_tok_struct
_tmp31D;_tmp31D.tag=Cyc_DeclList_tok;_tmp31D.f1=({struct Cyc_List_List*_tmp31E=
_cycalloc(sizeof(*_tmp31E));_tmp31E->hd=({struct Cyc_Absyn_Decl*_tmp31F=_cycalloc(
sizeof(*_tmp31F));_tmp31F->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp320=_cycalloc(sizeof(*_tmp320));_tmp320[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp321;_tmp321.tag=8;_tmp321.f1=({struct _tagged_arr*_tmp322=_cycalloc(sizeof(*
_tmp322));_tmp322[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp322;});_tmp321.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp321;});_tmp320;}));
_tmp31F->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp31F;});_tmp31E->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp31E;});_tmp31D;});_tmp31C;});break;case 7: _LL1C4: if(Cyc_Std_strcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tag_arr("C",sizeof(char),2))!= 0)Cyc_Parse_err(_tag_arr("only extern \"C\" { ... } is allowed",
sizeof(char),35),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp323=_cycalloc(sizeof(*_tmp323));_tmp323[0]=({
struct Cyc_DeclList_tok_struct _tmp324;_tmp324.tag=Cyc_DeclList_tok;_tmp324.f1=({
struct Cyc_List_List*_tmp325=_cycalloc(sizeof(*_tmp325));_tmp325->hd=({struct Cyc_Absyn_Decl*
_tmp326=_cycalloc(sizeof(*_tmp326));_tmp326->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp327=_cycalloc(sizeof(*_tmp327));_tmp327[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp328;_tmp328.tag=10;_tmp328.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp328;});_tmp327;}));
_tmp326->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp326;});_tmp325->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp325;});_tmp324;});_tmp323;});break;case 8: _LL1C5: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp329=_cycalloc(sizeof(*_tmp329));_tmp329[0]=({
struct Cyc_DeclList_tok_struct _tmp32A;_tmp32A.tag=Cyc_DeclList_tok;_tmp32A.f1=0;
_tmp32A;});_tmp329;});break;case 9: _LL1C6: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B[0]=({struct Cyc_DeclList_tok_struct
_tmp32C;_tmp32C.tag=Cyc_DeclList_tok;_tmp32C.f1=({struct Cyc_List_List*_tmp32D=
_cycalloc(sizeof(*_tmp32D));_tmp32D->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp32F;_tmp32F.tag=1;_tmp32F.f1=Cyc_yyget_FnDecl_tok(yyvs[yyvsp_offset]);
_tmp32F;});_tmp32E;}),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp32D->tl=0;_tmp32D;});_tmp32C;});_tmp32B;});
break;case 10: _LL1C7: yyval=yyvs[yyvsp_offset];break;case 11: _LL1C8: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp330=_cycalloc(sizeof(*_tmp330));_tmp330[0]=({
struct Cyc_FnDecl_tok_struct _tmp331;_tmp331.tag=Cyc_FnDecl_tok;_tmp331.f1=Cyc_Parse_make_function(
0,Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp331;});_tmp330;});break;case 12: _LL1C9: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp332=_cycalloc(sizeof(*_tmp332));_tmp332[0]=({
struct Cyc_FnDecl_tok_struct _tmp333;_tmp333.tag=Cyc_FnDecl_tok;_tmp333.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp334=_cycalloc(sizeof(*_tmp334));_tmp334->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp334;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp333;});_tmp332;});break;case 13: _LL1CA: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp335=_cycalloc(sizeof(*_tmp335));_tmp335[0]=({
struct Cyc_FnDecl_tok_struct _tmp336;_tmp336.tag=Cyc_FnDecl_tok;_tmp336.f1=Cyc_Parse_make_function(
0,Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp336;});_tmp335;});break;case 14: _LL1CB: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp337=_cycalloc(sizeof(*_tmp337));_tmp337[0]=({
struct Cyc_FnDecl_tok_struct _tmp338;_tmp338.tag=Cyc_FnDecl_tok;_tmp338.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp339;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp338;});_tmp337;});break;case 15: _LL1CC: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp33A=_cycalloc(sizeof(*_tmp33A));_tmp33A[0]=({
struct Cyc_FnDecl_tok_struct _tmp33B;_tmp33B.tag=Cyc_FnDecl_tok;_tmp33B.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp33C;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp33B;});_tmp33A;});break;case 16: _LL1CD: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D[0]=({
struct Cyc_FnDecl_tok_struct _tmp33E;_tmp33E.tag=Cyc_FnDecl_tok;_tmp33E.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp33F=_cycalloc(sizeof(*_tmp33F));_tmp33F->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp33F;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp33E;});_tmp33D;});break;case 17: _LL1CE: Cyc_Lex_enter_using(
Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]));yyval=yyvs[yyvsp_offset];break;case 18:
_LL1CF: Cyc_Lex_leave_using();break;case 19: _LL1D0: Cyc_Lex_enter_namespace(({
struct _tagged_arr*_tmp340=_cycalloc(sizeof(*_tmp340));_tmp340[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp340;}));yyval=yyvs[yyvsp_offset];break;case 20: _LL1D1: Cyc_Lex_leave_namespace();
break;case 21: _LL1D2: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp341=
_cycalloc(sizeof(*_tmp341));_tmp341[0]=({struct Cyc_DeclList_tok_struct _tmp342;
_tmp342.tag=Cyc_DeclList_tok;_tmp342.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp342;});
_tmp341;});break;case 22: _LL1D3: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp343=_cycalloc(sizeof(*_tmp343));_tmp343[0]=({struct Cyc_DeclList_tok_struct
_tmp344;_tmp344.tag=Cyc_DeclList_tok;_tmp344.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp344;});_tmp343;});break;case 23: _LL1D4: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345[0]=({
struct Cyc_DeclList_tok_struct _tmp346;_tmp346.tag=Cyc_DeclList_tok;_tmp346.f1=({
struct Cyc_List_List*_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347->hd=Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
0,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp347->tl=0;
_tmp347;});_tmp346;});_tmp345;});break;case 24: _LL1D5: {struct Cyc_List_List*
_tmp348=0;{struct Cyc_List_List*_tmp349=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp349 != 0;
_tmp349=_tmp349->tl){struct _tagged_arr*_tmp34A=(struct _tagged_arr*)_tmp349->hd;
struct _tuple1*_tmp34B=({struct _tuple1*_tmp34E=_cycalloc(sizeof(*_tmp34E));
_tmp34E->f1=Cyc_Absyn_rel_ns_null;_tmp34E->f2=_tmp34A;_tmp34E;});struct Cyc_Absyn_Vardecl*
_tmp34C=Cyc_Absyn_new_vardecl(_tmp34B,Cyc_Absyn_wildtyp(0),0);_tmp348=({struct
Cyc_List_List*_tmp34D=_cycalloc(sizeof(*_tmp34D));_tmp34D->hd=_tmp34C;_tmp34D->tl=
_tmp348;_tmp34D;});}}_tmp348=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp348);yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp34F=
_cycalloc(sizeof(*_tmp34F));_tmp34F[0]=({struct Cyc_DeclList_tok_struct _tmp350;
_tmp350.tag=Cyc_DeclList_tok;_tmp350.f1=({struct Cyc_List_List*_tmp351=_cycalloc(
sizeof(*_tmp351));_tmp351->hd=Cyc_Absyn_letv_decl(_tmp348,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp351->tl=0;_tmp351;});_tmp350;});_tmp34F;});break;}
case 25: _LL1D6: yyval=yyvs[yyvsp_offset];break;case 26: _LL1D7: yyval=(void*)({struct
Cyc_DeclList_tok_struct*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352[0]=({struct
Cyc_DeclList_tok_struct _tmp353;_tmp353.tag=Cyc_DeclList_tok;_tmp353.f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_yyget_DeclList_tok(yyvs[yyvsp_offset]));_tmp353;});_tmp352;});break;case 27:
_LL1D8: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp354=_cycalloc(sizeof(*
_tmp354));_tmp354[0]=({struct Cyc_DeclSpec_tok_struct _tmp355;_tmp355.tag=Cyc_DeclSpec_tok;
_tmp355.f1=({struct Cyc_Parse_Declaration_spec*_tmp356=_cycalloc(sizeof(*_tmp356));
_tmp356->sc=({struct Cyc_Core_Opt*_tmp357=_cycalloc(sizeof(*_tmp357));_tmp357->v=(
void*)Cyc_yyget_StorageClass_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp357;});_tmp356->tq=Cyc_Absyn_empty_tqual();_tmp356->type_specs=
0;_tmp356->is_inline=0;_tmp356->attributes=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp356;});_tmp355;});_tmp354;});break;case 28: _LL1D9: if((Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->sc != 0)({void*_tmp358[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tag_arr("Only one storage class is allowed in a declaration",
sizeof(char),51),_tag_arr(_tmp358,sizeof(void*),0));});yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp359=_cycalloc(sizeof(*_tmp359));_tmp359[0]=({struct Cyc_DeclSpec_tok_struct
_tmp35A;_tmp35A.tag=Cyc_DeclSpec_tok;_tmp35A.f1=({struct Cyc_Parse_Declaration_spec*
_tmp35B=_cycalloc(sizeof(*_tmp35B));_tmp35B->sc=({struct Cyc_Core_Opt*_tmp35C=
_cycalloc(sizeof(*_tmp35C));_tmp35C->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp35C;});_tmp35B->tq=(
Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq;_tmp35B->type_specs=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp35B->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->is_inline;_tmp35B->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp35B;});_tmp35A;});_tmp359;});break;case 29:
_LL1DA: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp35D=_cycalloc(sizeof(*
_tmp35D));_tmp35D[0]=({struct Cyc_DeclSpec_tok_struct _tmp35E;_tmp35E.tag=Cyc_DeclSpec_tok;
_tmp35E.f1=({struct Cyc_Parse_Declaration_spec*_tmp35F=_cycalloc(sizeof(*_tmp35F));
_tmp35F->sc=0;_tmp35F->tq=Cyc_Absyn_empty_tqual();_tmp35F->type_specs=({struct
Cyc_List_List*_tmp360=_cycalloc(sizeof(*_tmp360));_tmp360->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp360->tl=0;
_tmp360;});_tmp35F->is_inline=0;_tmp35F->attributes=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp35F;});_tmp35E;});_tmp35D;});break;case 30: _LL1DB: yyval=(
void*)({struct Cyc_DeclSpec_tok_struct*_tmp361=_cycalloc(sizeof(*_tmp361));
_tmp361[0]=({struct Cyc_DeclSpec_tok_struct _tmp362;_tmp362.tag=Cyc_DeclSpec_tok;
_tmp362.f1=({struct Cyc_Parse_Declaration_spec*_tmp363=_cycalloc(sizeof(*_tmp363));
_tmp363->sc=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->sc;_tmp363->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->tq;_tmp363->type_specs=({struct Cyc_List_List*_tmp364=
_cycalloc(sizeof(*_tmp364));_tmp364->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp364->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp364;});_tmp363->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->is_inline;_tmp363->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp363;});_tmp362;});_tmp361;});break;case 31:
_LL1DC: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp365=_cycalloc(sizeof(*
_tmp365));_tmp365[0]=({struct Cyc_DeclSpec_tok_struct _tmp366;_tmp366.tag=Cyc_DeclSpec_tok;
_tmp366.f1=({struct Cyc_Parse_Declaration_spec*_tmp367=_cycalloc(sizeof(*_tmp367));
_tmp367->sc=0;_tmp367->tq=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp367->type_specs=0;
_tmp367->is_inline=0;_tmp367->attributes=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp367;});_tmp366;});_tmp365;});break;case 32: _LL1DD: yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp368=_cycalloc(sizeof(*_tmp368));_tmp368[0]=({
struct Cyc_DeclSpec_tok_struct _tmp369;_tmp369.tag=Cyc_DeclSpec_tok;_tmp369.f1=({
struct Cyc_Parse_Declaration_spec*_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A->sc=(
Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->sc;_tmp36A->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq);_tmp36A->type_specs=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp36A->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->is_inline;_tmp36A->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp36A;});_tmp369;});_tmp368;});break;case 33:
_LL1DE: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp36B=_cycalloc(sizeof(*
_tmp36B));_tmp36B[0]=({struct Cyc_DeclSpec_tok_struct _tmp36C;_tmp36C.tag=Cyc_DeclSpec_tok;
_tmp36C.f1=({struct Cyc_Parse_Declaration_spec*_tmp36D=_cycalloc(sizeof(*_tmp36D));
_tmp36D->sc=0;_tmp36D->tq=Cyc_Absyn_empty_tqual();_tmp36D->type_specs=0;_tmp36D->is_inline=
1;_tmp36D->attributes=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp36D;});
_tmp36C;});_tmp36B;});break;case 34: _LL1DF: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E[0]=({struct Cyc_DeclSpec_tok_struct
_tmp36F;_tmp36F.tag=Cyc_DeclSpec_tok;_tmp36F.f1=({struct Cyc_Parse_Declaration_spec*
_tmp370=_cycalloc(sizeof(*_tmp370));_tmp370->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->sc;_tmp370->tq=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq;
_tmp370->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->type_specs;
_tmp370->is_inline=1;_tmp370->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp370;});_tmp36F;});_tmp36E;});break;case 35:
_LL1E0: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp371=_cycalloc(
sizeof(*_tmp371));_tmp371[0]=({struct Cyc_StorageClass_tok_struct _tmp372;_tmp372.tag=
Cyc_StorageClass_tok;_tmp372.f1=(void*)((void*)4);_tmp372;});_tmp371;});break;
case 36: _LL1E1: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp373=_cycalloc(
sizeof(*_tmp373));_tmp373[0]=({struct Cyc_StorageClass_tok_struct _tmp374;_tmp374.tag=
Cyc_StorageClass_tok;_tmp374.f1=(void*)((void*)5);_tmp374;});_tmp373;});break;
case 37: _LL1E2: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp375=_cycalloc(
sizeof(*_tmp375));_tmp375[0]=({struct Cyc_StorageClass_tok_struct _tmp376;_tmp376.tag=
Cyc_StorageClass_tok;_tmp376.f1=(void*)((void*)3);_tmp376;});_tmp375;});break;
case 38: _LL1E3: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp377=_cycalloc(
sizeof(*_tmp377));_tmp377[0]=({struct Cyc_StorageClass_tok_struct _tmp378;_tmp378.tag=
Cyc_StorageClass_tok;_tmp378.f1=(void*)((void*)1);_tmp378;});_tmp377;});break;
case 39: _LL1E4: if(Cyc_Std_strcmp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),
_tag_arr("C",sizeof(char),2))!= 0)Cyc_Parse_err(_tag_arr("only extern or extern \"C\" is allowed",
sizeof(char),37),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379[0]=({struct Cyc_StorageClass_tok_struct
_tmp37A;_tmp37A.tag=Cyc_StorageClass_tok;_tmp37A.f1=(void*)((void*)2);_tmp37A;});
_tmp379;});break;case 40: _LL1E5: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B[0]=({struct Cyc_StorageClass_tok_struct
_tmp37C;_tmp37C.tag=Cyc_StorageClass_tok;_tmp37C.f1=(void*)((void*)0);_tmp37C;});
_tmp37B;});break;case 41: _LL1E6: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp37D=_cycalloc(sizeof(*_tmp37D));_tmp37D[0]=({struct Cyc_StorageClass_tok_struct
_tmp37E;_tmp37E.tag=Cyc_StorageClass_tok;_tmp37E.f1=(void*)((void*)6);_tmp37E;});
_tmp37D;});break;case 42: _LL1E7: yyval=(void*)({struct Cyc_AttributeList_tok_struct*
_tmp37F=_cycalloc(sizeof(*_tmp37F));_tmp37F[0]=({struct Cyc_AttributeList_tok_struct
_tmp380;_tmp380.tag=Cyc_AttributeList_tok;_tmp380.f1=0;_tmp380;});_tmp37F;});
break;case 43: _LL1E8: yyval=yyvs[yyvsp_offset];break;case 44: _LL1E9: yyval=(void*)({
struct Cyc_AttributeList_tok_struct*_tmp381=_cycalloc(sizeof(*_tmp381));_tmp381[0]=({
struct Cyc_AttributeList_tok_struct _tmp382;_tmp382.tag=Cyc_AttributeList_tok;
_tmp382.f1=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp382;});_tmp381;});break;case 45: _LL1EA: yyval=(void*)({
struct Cyc_AttributeList_tok_struct*_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383[0]=({
struct Cyc_AttributeList_tok_struct _tmp384;_tmp384.tag=Cyc_AttributeList_tok;
_tmp384.f1=({struct Cyc_List_List*_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385->hd=(
void*)Cyc_yyget_Attribute_tok(yyvs[yyvsp_offset]);_tmp385->tl=0;_tmp385;});
_tmp384;});_tmp383;});break;case 46: _LL1EB: yyval=(void*)({struct Cyc_AttributeList_tok_struct*
_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386[0]=({struct Cyc_AttributeList_tok_struct
_tmp387;_tmp387.tag=Cyc_AttributeList_tok;_tmp387.f1=({struct Cyc_List_List*
_tmp388=_cycalloc(sizeof(*_tmp388));_tmp388->hd=(void*)Cyc_yyget_Attribute_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp388->tl=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp388;});_tmp387;});_tmp386;});break;case 47: _LL1EC: {
static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};static struct _tuple16
att_map[16]={{{_tmp38E,_tmp38E,_tmp38E + 8},(void*)0},{{_tmp38F,_tmp38F,_tmp38F + 
6},(void*)1},{{_tmp390,_tmp390,_tmp390 + 9},(void*)2},{{_tmp391,_tmp391,_tmp391 + 
9},(void*)3},{{_tmp392,_tmp392,_tmp392 + 6},(void*)4},{{_tmp393,_tmp393,_tmp393 + 
8},(void*)& att_aligned},{{_tmp394,_tmp394,_tmp394 + 7},(void*)5},{{_tmp395,
_tmp395,_tmp395 + 7},(void*)7},{{_tmp396,_tmp396,_tmp396 + 7},(void*)8},{{_tmp397,
_tmp397,_tmp397 + 5},(void*)9},{{_tmp398,_tmp398,_tmp398 + 10},(void*)10},{{
_tmp399,_tmp399,_tmp399 + 10},(void*)11},{{_tmp39A,_tmp39A,_tmp39A + 23},(void*)12},{{
_tmp39B,_tmp39B,_tmp39B + 12},(void*)13},{{_tmp39C,_tmp39C,_tmp39C + 11},(void*)14},{{
_tmp39D,_tmp39D,_tmp39D + 22},(void*)15}};struct _tagged_arr _tmp389=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);if((((_get_arr_size(_tmp389,sizeof(char))> 4?*((const char*)
_check_unknown_subscript(_tmp389,sizeof(char),0))== '_': 0)?*((const char*)
_check_unknown_subscript(_tmp389,sizeof(char),1))== '_': 0)?*((const char*)
_check_unknown_subscript(_tmp389,sizeof(char),(int)(_get_arr_size(_tmp389,
sizeof(char))- 2)))== '_': 0)?*((const char*)_check_unknown_subscript(_tmp389,
sizeof(char),(int)(_get_arr_size(_tmp389,sizeof(char))- 3)))== '_': 0)_tmp389=(
struct _tagged_arr)Cyc_Std_substring(_tmp389,2,_get_arr_size(_tmp389,sizeof(char))
- 5);{int i=0;for(0;i < 16;++ i){if(Cyc_Std_strcmp(_tmp389,(att_map[i]).f1)== 0){
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp38A=_cycalloc(sizeof(*_tmp38A));
_tmp38A[0]=({struct Cyc_Attribute_tok_struct _tmp38B;_tmp38B.tag=Cyc_Attribute_tok;
_tmp38B.f1=(void*)(att_map[i]).f2;_tmp38B;});_tmp38A;});break;}}if(i == 16){Cyc_Parse_err(
_tag_arr("unrecognized attribute",sizeof(char),23),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C[0]=({
struct Cyc_Attribute_tok_struct _tmp38D;_tmp38D.tag=Cyc_Attribute_tok;_tmp38D.f1=(
void*)((void*)1);_tmp38D;});_tmp38C;});}break;}}case 48: _LL1ED: yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F[0]=({
struct Cyc_Attribute_tok_struct _tmp3A0;_tmp3A0.tag=Cyc_Attribute_tok;_tmp3A0.f1=(
void*)((void*)4);_tmp3A0;});_tmp39F;});break;case 49: _LL1EE: {struct _tagged_arr
_tmp3A1=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);struct _tuple13 _tmp3A3;int _tmp3A4;struct _tuple13*_tmp3A2=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3A3=*_tmp3A2;
_tmp3A4=_tmp3A3.f2;{void*a;if(Cyc_Std_zstrcmp(_tmp3A1,_tag_arr("regparm",sizeof(
char),8))== 0?1: Cyc_Std_zstrcmp(_tmp3A1,_tag_arr("__regparm__",sizeof(char),12))
== 0){if(_tmp3A4 < 0?1: _tmp3A4 > 3)Cyc_Parse_err(_tag_arr("regparm requires value between 0 and 3",
sizeof(char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp3A5=_cycalloc_atomic(sizeof(*_tmp3A5));
_tmp3A5[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp3A6;_tmp3A6.tag=0;_tmp3A6.f1=
_tmp3A4;_tmp3A6;});_tmp3A5;});}else{if(Cyc_Std_zstrcmp(_tmp3A1,_tag_arr("aligned",
sizeof(char),8))== 0?1: Cyc_Std_zstrcmp(_tmp3A1,_tag_arr("__aligned__",sizeof(
char),12))== 0){if(_tmp3A4 < 0)Cyc_Parse_err(_tag_arr("aligned requires positive power of two",
sizeof(char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp3A4;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(_tag_arr("aligned requires positive power of two",sizeof(char),39),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));a=(void*)({struct Cyc_Absyn_Aligned_att_struct*
_tmp3A7=_cycalloc_atomic(sizeof(*_tmp3A7));_tmp3A7[0]=({struct Cyc_Absyn_Aligned_att_struct
_tmp3A8;_tmp3A8.tag=1;_tmp3A8.f1=_tmp3A4;_tmp3A8;});_tmp3A7;});}}else{Cyc_Parse_err(
_tag_arr("unrecognized attribute",sizeof(char),23),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));
_tmp3A9[0]=({struct Cyc_Attribute_tok_struct _tmp3AA;_tmp3AA.tag=Cyc_Attribute_tok;
_tmp3AA.f1=(void*)a;_tmp3AA;});_tmp3A9;});break;}}case 50: _LL1EF: {struct
_tagged_arr _tmp3AB=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp3AC=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_Std_zstrcmp(
_tmp3AB,_tag_arr("section",sizeof(char),8))== 0?1: Cyc_Std_zstrcmp(_tmp3AB,
_tag_arr("__section__",sizeof(char),12))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD[0]=({struct Cyc_Absyn_Section_att_struct
_tmp3AE;_tmp3AE.tag=2;_tmp3AE.f1=_tmp3AC;_tmp3AE;});_tmp3AD;});else{Cyc_Parse_err(
_tag_arr("unrecognized attribute",sizeof(char),23),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));
_tmp3AF[0]=({struct Cyc_Attribute_tok_struct _tmp3B0;_tmp3B0.tag=Cyc_Attribute_tok;
_tmp3B0.f1=(void*)a;_tmp3B0;});_tmp3AF;});break;}case 51: _LL1F0: {struct
_tagged_arr _tmp3B1=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp3B2=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp3B4;struct
_tuple13 _tmp3B3=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp3B4=_tmp3B3.f2;{int _tmp3B6;struct _tuple13 _tmp3B5=*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3B6=_tmp3B5.f2;{
void*a=(void*)1;if(Cyc_Std_zstrcmp(_tmp3B1,_tag_arr("format",sizeof(char),7))== 
0?1: Cyc_Std_zstrcmp(_tmp3B1,_tag_arr("__format__",sizeof(char),11))== 0){if(Cyc_Std_zstrcmp(
_tmp3B2,_tag_arr("printf",sizeof(char),7))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*
_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7[0]=({struct Cyc_Absyn_Format_att_struct
_tmp3B8;_tmp3B8.tag=3;_tmp3B8.f1=(void*)((void*)0);_tmp3B8.f2=_tmp3B4;_tmp3B8.f3=
_tmp3B6;_tmp3B8;});_tmp3B7;});else{if(Cyc_Std_zstrcmp(_tmp3B2,_tag_arr("scanf",
sizeof(char),6))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp3B9=
_cycalloc(sizeof(*_tmp3B9));_tmp3B9[0]=({struct Cyc_Absyn_Format_att_struct
_tmp3BA;_tmp3BA.tag=3;_tmp3BA.f1=(void*)((void*)1);_tmp3BA.f2=_tmp3B4;_tmp3BA.f3=
_tmp3B6;_tmp3BA;});_tmp3B9;});else{Cyc_Parse_err(_tag_arr("unrecognized format type",
sizeof(char),25),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(
_tag_arr("unrecognized attribute",sizeof(char),23),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Attribute_tok_struct*
_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB[0]=({struct Cyc_Attribute_tok_struct
_tmp3BC;_tmp3BC.tag=Cyc_Attribute_tok;_tmp3BC.f1=(void*)a;_tmp3BC;});_tmp3BB;});
break;}}}case 52: _LL1F1: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3BD=
_cycalloc(sizeof(*_tmp3BD));_tmp3BD[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3BE;_tmp3BE.tag=Cyc_TypeSpecifier_tok;_tmp3BE.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3BE;});_tmp3BD;});break;case 53: _LL1F2: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3C0;_tmp3C0.tag=Cyc_TypeSpecifier_tok;
_tmp3C0.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3C0;});
_tmp3BF;});break;case 54: _LL1F3: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3C2;_tmp3C2.tag=Cyc_TypeSpecifier_tok;_tmp3C2.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));
_tmp3C3->v=(void*)Cyc_yyget_Kind_tok(yyvs[yyvsp_offset]);_tmp3C3;}),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3C2;});_tmp3C1;});break;case 55: _LL1F4: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3C5;_tmp3C5.tag=Cyc_TypeSpecifier_tok;
_tmp3C5.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_uchar_t,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3C5;});
_tmp3C4;});break;case 56: _LL1F5: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3C7;_tmp3C7.tag=Cyc_TypeSpecifier_tok;_tmp3C7.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*
_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8[0]=({struct Cyc_Parse_Short_spec_struct
_tmp3C9;_tmp3C9.tag=2;_tmp3C9.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp3C9;});_tmp3C8;}));_tmp3C7;});_tmp3C6;});
break;case 57: _LL1F6: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3CA=
_cycalloc(sizeof(*_tmp3CA));_tmp3CA[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3CB;_tmp3CB.tag=Cyc_TypeSpecifier_tok;_tmp3CB.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_t,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3CB;});_tmp3CA;});break;case 58: _LL1F7: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));
_tmp3CC[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3CD;_tmp3CD.tag=Cyc_TypeSpecifier_tok;
_tmp3CD.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*_tmp3CE=_cycalloc(
sizeof(*_tmp3CE));_tmp3CE[0]=({struct Cyc_Parse_Long_spec_struct _tmp3CF;_tmp3CF.tag=
3;_tmp3CF.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line);_tmp3CF;});_tmp3CE;}));_tmp3CD;});_tmp3CC;});break;
case 59: _LL1F8: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3D0=
_cycalloc(sizeof(*_tmp3D0));_tmp3D0[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3D1;_tmp3D1.tag=Cyc_TypeSpecifier_tok;_tmp3D1.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3D1;});_tmp3D0;});break;case 60: _LL1F9: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));
_tmp3D2[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3D3;_tmp3D3.tag=Cyc_TypeSpecifier_tok;
_tmp3D3.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3D3;});
_tmp3D2;});break;case 61: _LL1FA: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3D5;_tmp3D5.tag=Cyc_TypeSpecifier_tok;_tmp3D5.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*
_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp3D7;_tmp3D7.tag=0;_tmp3D7.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp3D7;});_tmp3D6;}));_tmp3D5;});_tmp3D4;});
break;case 62: _LL1FB: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3D8=
_cycalloc(sizeof(*_tmp3D8));_tmp3D8[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3D9;_tmp3D9.tag=Cyc_TypeSpecifier_tok;_tmp3D9.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp3DA=_cycalloc(sizeof(*_tmp3DA));_tmp3DA[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp3DB;_tmp3DB.tag=1;_tmp3DB.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp3DB;});_tmp3DA;}));_tmp3D9;});_tmp3D8;});
break;case 63: _LL1FC: yyval=yyvs[yyvsp_offset];break;case 64: _LL1FD: yyval=yyvs[
yyvsp_offset];break;case 65: _LL1FE: yyval=yyvs[yyvsp_offset];break;case 66: _LL1FF:
yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3DC=_cycalloc(sizeof(*
_tmp3DC));_tmp3DC[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3DD;_tmp3DD.tag=Cyc_TypeSpecifier_tok;
_tmp3DD.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3DD;});
_tmp3DC;});break;case 67: _LL200: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3DF;_tmp3DF.tag=Cyc_TypeSpecifier_tok;_tmp3DF.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypedefType_struct*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));
_tmp3E0[0]=({struct Cyc_Absyn_TypedefType_struct _tmp3E1;_tmp3E1.tag=16;_tmp3E1.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3E1.f2=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp3E1.f3=0;_tmp3E1.f4=0;
_tmp3E1;});_tmp3E0;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3DF;});_tmp3DE;});break;case 68: _LL201: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));_tmp3E2[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3E3;_tmp3E3.tag=Cyc_TypeSpecifier_tok;
_tmp3E3.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TupleType_struct*
_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4[0]=({struct Cyc_Absyn_TupleType_struct
_tmp3E5;_tmp3E5.tag=9;_tmp3E5.f1=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(
struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp3E5;});_tmp3E4;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3E3;});
_tmp3E2;});break;case 69: _LL202: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3E6=_cycalloc(sizeof(*_tmp3E6));_tmp3E6[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3E7;_tmp3E7.tag=Cyc_TypeSpecifier_tok;_tmp3E7.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp3E8=_cycalloc(sizeof(*_tmp3E8));
_tmp3E8[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp3E9;_tmp3E9.tag=15;_tmp3E9.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp3E9;});_tmp3E8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3E7;});_tmp3E6;});break;case 70: _LL203: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3EB;_tmp3EB.tag=Cyc_TypeSpecifier_tok;
_tmp3EB.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp3EC=_cycalloc(sizeof(*_tmp3EC));_tmp3EC[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp3ED;_tmp3ED.tag=14;_tmp3ED.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3ED;});_tmp3EC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3EB;});
_tmp3EA;});break;case 71: _LL204: yyval=(void*)({struct Cyc_Kind_tok_struct*_tmp3EE=
_cycalloc(sizeof(*_tmp3EE));_tmp3EE[0]=({struct Cyc_Kind_tok_struct _tmp3EF;
_tmp3EF.tag=Cyc_Kind_tok;_tmp3EF.f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3EF;});_tmp3EE;});break;case 72: _LL205: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp3F0=_cycalloc_atomic(sizeof(*_tmp3F0));
_tmp3F0[0]=({struct Cyc_TypeQual_tok_struct _tmp3F1;_tmp3F1.tag=Cyc_TypeQual_tok;
_tmp3F1.f1=({struct Cyc_Absyn_Tqual _tmp3F2;_tmp3F2.q_const=1;_tmp3F2.q_volatile=0;
_tmp3F2.q_restrict=0;_tmp3F2;});_tmp3F1;});_tmp3F0;});break;case 73: _LL206: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp3F3=_cycalloc_atomic(sizeof(*_tmp3F3));
_tmp3F3[0]=({struct Cyc_TypeQual_tok_struct _tmp3F4;_tmp3F4.tag=Cyc_TypeQual_tok;
_tmp3F4.f1=({struct Cyc_Absyn_Tqual _tmp3F5;_tmp3F5.q_const=0;_tmp3F5.q_volatile=1;
_tmp3F5.q_restrict=0;_tmp3F5;});_tmp3F4;});_tmp3F3;});break;case 74: _LL207: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp3F6=_cycalloc_atomic(sizeof(*_tmp3F6));
_tmp3F6[0]=({struct Cyc_TypeQual_tok_struct _tmp3F7;_tmp3F7.tag=Cyc_TypeQual_tok;
_tmp3F7.f1=({struct Cyc_Absyn_Tqual _tmp3F8;_tmp3F8.q_const=0;_tmp3F8.q_volatile=0;
_tmp3F8.q_restrict=1;_tmp3F8;});_tmp3F7;});_tmp3F6;});break;case 75: _LL208: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));
_tmp3F9[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3FA;_tmp3FA.tag=Cyc_TypeSpecifier_tok;
_tmp3FA.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp3FB=_cycalloc(
sizeof(*_tmp3FB));_tmp3FB[0]=({struct Cyc_Parse_Decl_spec_struct _tmp3FC;_tmp3FC.tag=
5;_tmp3FC.f1=({struct Cyc_Absyn_Decl*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD->r=(
void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));
_tmp3FE[0]=({struct Cyc_Absyn_Enum_d_struct _tmp3FF;_tmp3FF.tag=6;_tmp3FF.f1=({
struct Cyc_Absyn_Enumdecl*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->sc=(void*)((
void*)2);_tmp400->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp400->fields=({struct Cyc_Core_Opt*_tmp401=_cycalloc(
sizeof(*_tmp401));_tmp401->v=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp401;});_tmp400;});
_tmp3FF;});_tmp3FE;}));_tmp3FD->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp3FD;});_tmp3FC;});_tmp3FB;}));_tmp3FA;});_tmp3F9;});
break;case 76: _LL209: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp402=
_cycalloc(sizeof(*_tmp402));_tmp402[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp403;_tmp403.tag=Cyc_TypeSpecifier_tok;_tmp403.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp404=_cycalloc(sizeof(*_tmp404));
_tmp404[0]=({struct Cyc_Absyn_EnumType_struct _tmp405;_tmp405.tag=12;_tmp405.f1=
Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);_tmp405.f2=0;_tmp405;});_tmp404;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp403;});_tmp402;});break;case 77: _LL20A: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp407;_tmp407.tag=Cyc_TypeSpecifier_tok;
_tmp407.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp408=_cycalloc(
sizeof(*_tmp408));_tmp408[0]=({struct Cyc_Parse_Type_spec_struct _tmp409;_tmp409.tag=
4;_tmp409.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp40A=
_cycalloc(sizeof(*_tmp40A));_tmp40A[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp40B;_tmp40B.tag=13;_tmp40B.f1=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp40B;});_tmp40A;}));
_tmp409.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line);_tmp409;});
_tmp408;}));_tmp407;});_tmp406;});break;case 78: _LL20B: yyval=(void*)({struct Cyc_Enumfield_tok_struct*
_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C[0]=({struct Cyc_Enumfield_tok_struct
_tmp40D;_tmp40D.tag=Cyc_Enumfield_tok;_tmp40D.f1=({struct Cyc_Absyn_Enumfield*
_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E->name=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp40E->tag=0;_tmp40E->loc=Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);_tmp40E;});_tmp40D;});
_tmp40C;});break;case 79: _LL20C: yyval=(void*)({struct Cyc_Enumfield_tok_struct*
_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F[0]=({struct Cyc_Enumfield_tok_struct
_tmp410;_tmp410.tag=Cyc_Enumfield_tok;_tmp410.f1=({struct Cyc_Absyn_Enumfield*
_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp411->tag=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp411->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp411;});_tmp410;});_tmp40F;});break;case 80: _LL20D:
yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*_tmp412=_cycalloc(sizeof(*
_tmp412));_tmp412[0]=({struct Cyc_EnumfieldList_tok_struct _tmp413;_tmp413.tag=Cyc_EnumfieldList_tok;
_tmp413.f1=({struct Cyc_List_List*_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414->hd=
Cyc_yyget_Enumfield_tok(yyvs[yyvsp_offset]);_tmp414->tl=0;_tmp414;});_tmp413;});
_tmp412;});break;case 81: _LL20E: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp416;_tmp416.tag=Cyc_EnumfieldList_tok;_tmp416.f1=({struct Cyc_List_List*
_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp417->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[yyvsp_offset]);_tmp417;});_tmp416;});_tmp415;});break;case 82: _LL20F: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp418=_cycalloc(sizeof(*_tmp418));
_tmp418[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp419;_tmp419.tag=Cyc_TypeSpecifier_tok;
_tmp419.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp41B;_tmp41B.tag=11;_tmp41B.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp41B.f2=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp41B;});_tmp41A;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp419;});
_tmp418;});break;case 83: _LL210: {struct Cyc_List_List*_tmp41C=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));struct Cyc_List_List*
_tmp41D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp41E=_cycalloc(sizeof(*_tmp41E));_tmp41E[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp41F;_tmp41F.tag=Cyc_TypeSpecifier_tok;
_tmp41F.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp420=_cycalloc(
sizeof(*_tmp420));_tmp420[0]=({struct Cyc_Parse_Decl_spec_struct _tmp421;_tmp421.tag=
5;_tmp421.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmp41C,({struct Cyc_Core_Opt*
_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422->v=_tmp41D;_tmp422;}),({struct Cyc_Core_Opt*
_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->v=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp423;}),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp421;});_tmp420;}));_tmp41F;});_tmp41E;});break;}
case 84: _LL211: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp424=
_cycalloc(sizeof(*_tmp424));_tmp424[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp425;_tmp425.tag=Cyc_TypeSpecifier_tok;_tmp425.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp426=_cycalloc(sizeof(*_tmp426));
_tmp426[0]=({struct Cyc_Absyn_AggrType_struct _tmp427;_tmp427.tag=10;_tmp427.f1=({
struct Cyc_Absyn_AggrInfo _tmp428;_tmp428.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp42A;_tmp42A.tag=0;_tmp42A.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp42A.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp42A;});_tmp429;}));
_tmp428.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp428;});_tmp427;});
_tmp426;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp425;});
_tmp424;});break;case 85: _LL212: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B[0]=({struct Cyc_TypeList_tok_struct
_tmp42C;_tmp42C.tag=Cyc_TypeList_tok;_tmp42C.f1=0;_tmp42C;});_tmp42B;});break;
case 86: _LL213: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp42D=_cycalloc(
sizeof(*_tmp42D));_tmp42D[0]=({struct Cyc_TypeList_tok_struct _tmp42E;_tmp42E.tag=
Cyc_TypeList_tok;_tmp42E.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp42E;});_tmp42D;});
break;case 87: _LL214: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp42F=
_cycalloc(sizeof(*_tmp42F));_tmp42F[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp430;_tmp430.tag=Cyc_StructOrUnion_tok;_tmp430.f1=(void*)((void*)0);_tmp430;});
_tmp42F;});break;case 88: _LL215: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*
_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp432;_tmp432.tag=Cyc_StructOrUnion_tok;_tmp432.f1=(void*)((void*)1);_tmp432;});
_tmp431;});break;case 89: _LL216: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp434;_tmp434.tag=Cyc_AggrFieldDeclList_tok;_tmp434.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_AggrFieldDeclListList_tok(yyvs[yyvsp_offset])));
_tmp434;});_tmp433;});break;case 90: _LL217: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp436;_tmp436.tag=Cyc_AggrFieldDeclListList_tok;_tmp436.f1=({struct Cyc_List_List*
_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[yyvsp_offset]);_tmp437->tl=0;_tmp437;});_tmp436;});_tmp435;});break;case 91:
_LL218: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*_tmp438=
_cycalloc(sizeof(*_tmp438));_tmp438[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp439;_tmp439.tag=Cyc_AggrFieldDeclListList_tok;_tmp439.f1=({struct Cyc_List_List*
_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[yyvsp_offset]);_tmp43A->tl=Cyc_yyget_AggrFieldDeclListList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp43A;});_tmp439;});
_tmp438;});break;case 92: _LL219: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B[0]=({struct Cyc_InitDeclList_tok_struct
_tmp43C;_tmp43C.tag=Cyc_InitDeclList_tok;_tmp43C.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
yyvsp_offset]));_tmp43C;});_tmp43B;});break;case 93: _LL21A: yyval=(void*)({struct
Cyc_InitDeclList_tok_struct*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D[0]=({
struct Cyc_InitDeclList_tok_struct _tmp43E;_tmp43E.tag=Cyc_InitDeclList_tok;
_tmp43E.f1=({struct Cyc_List_List*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->hd=
Cyc_yyget_InitDecl_tok(yyvs[yyvsp_offset]);_tmp43F->tl=0;_tmp43F;});_tmp43E;});
_tmp43D;});break;case 94: _LL21B: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440[0]=({struct Cyc_InitDeclList_tok_struct
_tmp441;_tmp441.tag=Cyc_InitDeclList_tok;_tmp441.f1=({struct Cyc_List_List*
_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->hd=Cyc_yyget_InitDecl_tok(yyvs[
yyvsp_offset]);_tmp442->tl=Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp442;});_tmp441;});
_tmp440;});break;case 95: _LL21C: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443[0]=({struct Cyc_InitDecl_tok_struct
_tmp444;_tmp444.tag=Cyc_InitDecl_tok;_tmp444.f1=({struct _tuple12*_tmp445=
_cycalloc(sizeof(*_tmp445));_tmp445->f1=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp445->f2=0;_tmp445;});_tmp444;});_tmp443;});break;case 96:
_LL21D: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp446=_cycalloc(sizeof(*
_tmp446));_tmp446[0]=({struct Cyc_InitDecl_tok_struct _tmp447;_tmp447.tag=Cyc_InitDecl_tok;
_tmp447.f1=({struct _tuple12*_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448->f1=Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp448->f2=(struct
Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp448;});_tmp447;});
_tmp446;});break;case 97: _LL21E: {struct _tuple15 _tmp44A;struct Cyc_Absyn_Tqual
_tmp44B;struct Cyc_List_List*_tmp44C;struct Cyc_List_List*_tmp44D;struct _tuple15*
_tmp449=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp44A=*_tmp449;_tmp44B=_tmp44A.f1;_tmp44C=_tmp44A.f2;
_tmp44D=_tmp44A.f3;{struct Cyc_List_List*_tmp44F;struct Cyc_List_List*_tmp450;
struct _tuple0 _tmp44E=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
Cyc_yyget_InitDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp44F=_tmp44E.f1;_tmp450=_tmp44E.f2;{void*_tmp451=Cyc_Parse_speclist2typ(
_tmp44C,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp452=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(Cyc_Parse_apply_tmss(
_tmp44B,_tmp451,_tmp44F,_tmp44D),_tmp450);yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp453=_cycalloc(sizeof(*_tmp453));_tmp453[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp454;_tmp454.tag=Cyc_AggrFieldDeclList_tok;_tmp454.f1=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmp452);_tmp454;});_tmp453;});break;}}}case 98:
_LL21F: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp455=_cycalloc(
sizeof(*_tmp455));_tmp455[0]=({struct Cyc_QualSpecList_tok_struct _tmp456;_tmp456.tag=
Cyc_QualSpecList_tok;_tmp456.f1=({struct _tuple15*_tmp457=_cycalloc(sizeof(*
_tmp457));_tmp457->f1=Cyc_Absyn_empty_tqual();_tmp457->f2=({struct Cyc_List_List*
_tmp458=_cycalloc(sizeof(*_tmp458));_tmp458->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp458->tl=0;
_tmp458;});_tmp457->f3=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp457;});
_tmp456;});_tmp455;});break;case 99: _LL220: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459[0]=({struct Cyc_QualSpecList_tok_struct
_tmp45A;_tmp45A.tag=Cyc_QualSpecList_tok;_tmp45A.f1=({struct _tuple15*_tmp45B=
_cycalloc(sizeof(*_tmp45B));_tmp45B->f1=(*Cyc_yyget_QualSpecList_tok(yyvs[
yyvsp_offset])).f1;_tmp45B->f2=({struct Cyc_List_List*_tmp45C=_cycalloc(sizeof(*
_tmp45C));_tmp45C->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp45C->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f2;_tmp45C;});_tmp45B->f3=Cyc_List_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f3);_tmp45B;});_tmp45A;});_tmp459;});break;case 100: _LL221:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp45D=_cycalloc(sizeof(*
_tmp45D));_tmp45D[0]=({struct Cyc_QualSpecList_tok_struct _tmp45E;_tmp45E.tag=Cyc_QualSpecList_tok;
_tmp45E.f1=({struct _tuple15*_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp45F->f2=0;
_tmp45F->f3=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp45F;});_tmp45E;});
_tmp45D;});break;case 101: _LL222: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460[0]=({struct Cyc_QualSpecList_tok_struct
_tmp461;_tmp461.tag=Cyc_QualSpecList_tok;_tmp461.f1=({struct _tuple15*_tmp462=
_cycalloc(sizeof(*_tmp462));_tmp462->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f1);_tmp462->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
yyvsp_offset])).f2;_tmp462->f3=Cyc_List_append(Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f3);_tmp462;});_tmp461;});_tmp460;});break;case 102: _LL223:
yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp463=_cycalloc(sizeof(*
_tmp463));_tmp463[0]=({struct Cyc_InitDeclList_tok_struct _tmp464;_tmp464.tag=Cyc_InitDeclList_tok;
_tmp464.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok(yyvs[yyvsp_offset]));_tmp464;});_tmp463;});break;case
103: _LL224: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp465=_cycalloc(
sizeof(*_tmp465));_tmp465[0]=({struct Cyc_InitDeclList_tok_struct _tmp466;_tmp466.tag=
Cyc_InitDeclList_tok;_tmp466.f1=({struct Cyc_List_List*_tmp467=_cycalloc(sizeof(*
_tmp467));_tmp467->hd=Cyc_yyget_InitDecl_tok(yyvs[yyvsp_offset]);_tmp467->tl=0;
_tmp467;});_tmp466;});_tmp465;});break;case 104: _LL225: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468[0]=({struct Cyc_InitDeclList_tok_struct
_tmp469;_tmp469.tag=Cyc_InitDeclList_tok;_tmp469.f1=({struct Cyc_List_List*
_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->hd=Cyc_yyget_InitDecl_tok(yyvs[
yyvsp_offset]);_tmp46A->tl=Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp46A;});_tmp469;});
_tmp468;});break;case 105: _LL226: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp46B=_cycalloc(sizeof(*_tmp46B));_tmp46B[0]=({struct Cyc_InitDecl_tok_struct
_tmp46C;_tmp46C.tag=Cyc_InitDecl_tok;_tmp46C.f1=({struct _tuple12*_tmp46D=
_cycalloc(sizeof(*_tmp46D));_tmp46D->f1=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp46D->f2=0;_tmp46D;});_tmp46C;});_tmp46B;});break;case 106:
_LL227: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp46E=_cycalloc(sizeof(*
_tmp46E));_tmp46E[0]=({struct Cyc_InitDecl_tok_struct _tmp46F;_tmp46F.tag=Cyc_InitDecl_tok;
_tmp46F.f1=({struct _tuple12*_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470->f1=({
struct Cyc_Parse_Declarator*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->id=({
struct _tuple1*_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->f1=Cyc_Absyn_rel_ns_null;
_tmp472->f2=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);
_tmp472;});_tmp471->tms=0;_tmp471;});_tmp470->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp470;});_tmp46F;});_tmp46E;});break;case 107: _LL228: yyval=(
void*)({struct Cyc_InitDecl_tok_struct*_tmp474=_cycalloc(sizeof(*_tmp474));
_tmp474[0]=({struct Cyc_InitDecl_tok_struct _tmp475;_tmp475.tag=Cyc_InitDecl_tok;
_tmp475.f1=({struct _tuple12*_tmp476=_cycalloc(sizeof(*_tmp476));_tmp476->f1=Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp476->f2=(struct
Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp476;});_tmp475;});
_tmp474;});break;case 108: _LL229: {struct Cyc_List_List*_tmp477=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp479;_tmp479.tag=Cyc_TypeSpecifier_tok;
_tmp479.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp47A=_cycalloc(
sizeof(*_tmp47A));_tmp47A[0]=({struct Cyc_Parse_Decl_spec_struct _tmp47B;_tmp47B.tag=
5;_tmp47B.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp477,({struct Cyc_Core_Opt*
_tmp47C=_cycalloc(sizeof(*_tmp47C));_tmp47C->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp47C;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp47B;});_tmp47A;}));_tmp479;});_tmp478;});break;}
case 109: _LL22A: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp47D=
_cycalloc(sizeof(*_tmp47D));_tmp47D[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp47E;_tmp47E.tag=Cyc_TypeSpecifier_tok;_tmp47E.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp47F=_cycalloc(sizeof(*_tmp47F));
_tmp47F[0]=({struct Cyc_Absyn_TunionType_struct _tmp480;_tmp480.tag=2;_tmp480.f1=({
struct Cyc_Absyn_TunionInfo _tmp481;_tmp481.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp483;_tmp483.tag=0;_tmp483.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp484;
_tmp484.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp484.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp484;});_tmp483;});
_tmp482;}));_tmp481.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp481.rgn=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp481;});_tmp480;});_tmp47F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp47E;});_tmp47D;});break;case 110: _LL22B: {void*
_tmp485=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp48E=_cycalloc(sizeof(*
_tmp48E));_tmp48E->v=(void*)((void*)3);_tmp48E;}),0);yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp487;_tmp487.tag=Cyc_TypeSpecifier_tok;_tmp487.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp488=_cycalloc(sizeof(*_tmp488));
_tmp488[0]=({struct Cyc_Absyn_TunionType_struct _tmp489;_tmp489.tag=2;_tmp489.f1=({
struct Cyc_Absyn_TunionInfo _tmp48A;_tmp48A.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp48B=_cycalloc(sizeof(*_tmp48B));_tmp48B[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp48C;_tmp48C.tag=0;_tmp48C.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp48D;
_tmp48D.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp48D.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp48D;});_tmp48C;});
_tmp48B;}));_tmp48A.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp48A.rgn=(
void*)_tmp485;_tmp48A;});_tmp489;});_tmp488;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp487;});_tmp486;});break;}case 111: _LL22C: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp48F=_cycalloc(sizeof(*_tmp48F));
_tmp48F[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp490;_tmp490.tag=Cyc_TypeSpecifier_tok;
_tmp490.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp492;_tmp492.tag=3;_tmp492.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp493;
_tmp493.field_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*
_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp495;_tmp495.tag=0;_tmp495.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp496;
_tmp496.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp496.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp496.is_xtunion=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp496;});_tmp495;});
_tmp494;}));_tmp493.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp493;});
_tmp492;});_tmp491;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp490;});_tmp48F;});break;case 112: _LL22D: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp497=_cycalloc_atomic(sizeof(*_tmp497));
_tmp497[0]=({struct Cyc_Bool_tok_struct _tmp498;_tmp498.tag=Cyc_Bool_tok;_tmp498.f1=
0;_tmp498;});_tmp497;});break;case 113: _LL22E: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp499=_cycalloc_atomic(sizeof(*_tmp499));_tmp499[0]=({struct Cyc_Bool_tok_struct
_tmp49A;_tmp49A.tag=Cyc_Bool_tok;_tmp49A.f1=1;_tmp49A;});_tmp499;});break;case
114: _LL22F: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp49B=_cycalloc(
sizeof(*_tmp49B));_tmp49B[0]=({struct Cyc_TunionFieldList_tok_struct _tmp49C;
_tmp49C.tag=Cyc_TunionFieldList_tok;_tmp49C.f1=({struct Cyc_List_List*_tmp49D=
_cycalloc(sizeof(*_tmp49D));_tmp49D->hd=Cyc_yyget_TunionField_tok(yyvs[
yyvsp_offset]);_tmp49D->tl=0;_tmp49D;});_tmp49C;});_tmp49B;});break;case 115:
_LL230: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp49E=_cycalloc(
sizeof(*_tmp49E));_tmp49E[0]=({struct Cyc_TunionFieldList_tok_struct _tmp49F;
_tmp49F.tag=Cyc_TunionFieldList_tok;_tmp49F.f1=({struct Cyc_List_List*_tmp4A0=
_cycalloc(sizeof(*_tmp4A0));_tmp4A0->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A0->tl=0;_tmp4A0;});
_tmp49F;});_tmp49E;});break;case 116: _LL231: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp4A2;_tmp4A2.tag=Cyc_TunionFieldList_tok;_tmp4A2.f1=({struct Cyc_List_List*
_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4A3->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[yyvsp_offset]);_tmp4A3;});_tmp4A2;});_tmp4A1;});break;case 117: _LL232: yyval=(
void*)({struct Cyc_TunionFieldList_tok_struct*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));
_tmp4A4[0]=({struct Cyc_TunionFieldList_tok_struct _tmp4A5;_tmp4A5.tag=Cyc_TunionFieldList_tok;
_tmp4A5.f1=({struct Cyc_List_List*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6->hd=
Cyc_yyget_TunionField_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]);_tmp4A6->tl=Cyc_yyget_TunionFieldList_tok(yyvs[yyvsp_offset]);_tmp4A6;});
_tmp4A5;});_tmp4A4;});break;case 118: _LL233: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7[0]=({struct Cyc_Scope_tok_struct
_tmp4A8;_tmp4A8.tag=Cyc_Scope_tok;_tmp4A8.f1=(void*)((void*)2);_tmp4A8;});
_tmp4A7;});break;case 119: _LL234: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9[0]=({struct Cyc_Scope_tok_struct
_tmp4AA;_tmp4AA.tag=Cyc_Scope_tok;_tmp4AA.f1=(void*)((void*)3);_tmp4AA;});
_tmp4A9;});break;case 120: _LL235: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB[0]=({struct Cyc_Scope_tok_struct
_tmp4AC;_tmp4AC.tag=Cyc_Scope_tok;_tmp4AC.f1=(void*)((void*)0);_tmp4AC;});
_tmp4AB;});break;case 121: _LL236: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD[0]=({struct Cyc_TunionField_tok_struct
_tmp4AE;_tmp4AE.tag=Cyc_TunionField_tok;_tmp4AE.f1=({struct Cyc_Absyn_Tunionfield*
_tmp4AF=_cycalloc(sizeof(*_tmp4AF));_tmp4AF->name=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp4AF->typs=0;_tmp4AF->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4AF->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4AF;});_tmp4AE;});
_tmp4AD;});break;case 122: _LL237: {struct Cyc_List_List*_tmp4B0=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1[0]=({
struct Cyc_TunionField_tok_struct _tmp4B2;_tmp4B2.tag=Cyc_TunionField_tok;_tmp4B2.f1=({
struct Cyc_Absyn_Tunionfield*_tmp4B3=_cycalloc(sizeof(*_tmp4B3));_tmp4B3->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp4B3->typs=_tmp4B0;_tmp4B3->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4B3->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp4B3;});_tmp4B2;});
_tmp4B1;});break;}case 123: _LL238: yyval=yyvs[yyvsp_offset];break;case 124: _LL239:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));
_tmp4B4[0]=({struct Cyc_Declarator_tok_struct _tmp4B5;_tmp4B5.tag=Cyc_Declarator_tok;
_tmp4B5.f1=({struct Cyc_Parse_Declarator*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));
_tmp4B6->id=(Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]))->id;_tmp4B6->tms=Cyc_List_imp_append(
Cyc_yyget_TypeModifierList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]))->tms);_tmp4B6;});
_tmp4B5;});_tmp4B4;});break;case 125: _LL23A: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7[0]=({struct Cyc_Declarator_tok_struct
_tmp4B8;_tmp4B8.tag=Cyc_Declarator_tok;_tmp4B8.f1=({struct Cyc_Parse_Declarator*
_tmp4B9=_cycalloc(sizeof(*_tmp4B9));_tmp4B9->id=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp4B9->tms=0;_tmp4B9;});_tmp4B8;});_tmp4B7;});break;case 126:
_LL23B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 127: _LL23C: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4BA=_cycalloc(
sizeof(*_tmp4BA));_tmp4BA[0]=({struct Cyc_Declarator_tok_struct _tmp4BB;_tmp4BB.tag=
Cyc_Declarator_tok;_tmp4BB.f1=({struct Cyc_Parse_Declarator*_tmp4BC=_cycalloc(
sizeof(*_tmp4BC));_tmp4BC->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->id;_tmp4BC->tms=({
struct Cyc_List_List*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD->hd=(void*)((
void*)0);_tmp4BD->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->tms;_tmp4BD;});_tmp4BC;});
_tmp4BB;});_tmp4BA;});break;case 128: _LL23D: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE[0]=({struct Cyc_Declarator_tok_struct
_tmp4BF;_tmp4BF.tag=Cyc_Declarator_tok;_tmp4BF.f1=({struct Cyc_Parse_Declarator*
_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4C0->tms=({
struct Cyc_List_List*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp4C2=_cycalloc(sizeof(*_tmp4C2));
_tmp4C2[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp4C3;_tmp4C3.tag=0;_tmp4C3.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4C3;});_tmp4C2;}));_tmp4C1->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp4C1;});_tmp4C0;});
_tmp4BF;});_tmp4BE;});break;case 129: _LL23E: {struct _tuple14 _tmp4C5;struct Cyc_List_List*
_tmp4C6;int _tmp4C7;struct Cyc_Absyn_VarargInfo*_tmp4C8;struct Cyc_Core_Opt*_tmp4C9;
struct Cyc_List_List*_tmp4CA;struct _tuple14*_tmp4C4=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4C5=*_tmp4C4;_tmp4C6=
_tmp4C5.f1;_tmp4C7=_tmp4C5.f2;_tmp4C8=_tmp4C5.f3;_tmp4C9=_tmp4C5.f4;_tmp4CA=
_tmp4C5.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4CB=_cycalloc(
sizeof(*_tmp4CB));_tmp4CB[0]=({struct Cyc_Declarator_tok_struct _tmp4CC;_tmp4CC.tag=
Cyc_Declarator_tok;_tmp4CC.f1=({struct Cyc_Parse_Declarator*_tmp4CD=_cycalloc(
sizeof(*_tmp4CD));_tmp4CD->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4CD->tms=({
struct Cyc_List_List*_tmp4CE=_cycalloc(sizeof(*_tmp4CE));_tmp4CE->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4CF=_cycalloc(sizeof(*_tmp4CF));
_tmp4CF[0]=({struct Cyc_Absyn_Function_mod_struct _tmp4D0;_tmp4D0.tag=2;_tmp4D0.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp4D1=_cycalloc(sizeof(*
_tmp4D1));_tmp4D1[0]=({struct Cyc_Absyn_WithTypes_struct _tmp4D2;_tmp4D2.tag=1;
_tmp4D2.f1=_tmp4C6;_tmp4D2.f2=_tmp4C7;_tmp4D2.f3=_tmp4C8;_tmp4D2.f4=_tmp4C9;
_tmp4D2.f5=_tmp4CA;_tmp4D2;});_tmp4D1;}));_tmp4D0;});_tmp4CF;}));_tmp4CE->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp4CE;});_tmp4CD;});_tmp4CC;});_tmp4CB;});break;}case 130: _LL23F:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4D3=_cycalloc(sizeof(*_tmp4D3));
_tmp4D3[0]=({struct Cyc_Declarator_tok_struct _tmp4D4;_tmp4D4.tag=Cyc_Declarator_tok;
_tmp4D4.f1=({struct Cyc_Parse_Declarator*_tmp4D5=_cycalloc(sizeof(*_tmp4D5));
_tmp4D5->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp4D5->tms=({struct Cyc_List_List*_tmp4D6=_cycalloc(
sizeof(*_tmp4D6));_tmp4D6->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp4D7=_cycalloc(sizeof(*_tmp4D7));_tmp4D7[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp4D8;_tmp4D8.tag=2;_tmp4D8.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp4DA;_tmp4DA.tag=1;_tmp4DA.f1=0;_tmp4DA.f2=0;_tmp4DA.f3=0;_tmp4DA.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4DA.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4DA;});_tmp4D9;}));
_tmp4D8;});_tmp4D7;}));_tmp4D6->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp4D6;});_tmp4D5;});
_tmp4D4;});_tmp4D3;});break;case 131: _LL240: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB[0]=({struct Cyc_Declarator_tok_struct
_tmp4DC;_tmp4DC.tag=Cyc_Declarator_tok;_tmp4DC.f1=({struct Cyc_Parse_Declarator*
_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4DD->tms=({
struct Cyc_List_List*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));_tmp4DE->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));
_tmp4DF[0]=({struct Cyc_Absyn_Function_mod_struct _tmp4E0;_tmp4E0.tag=2;_tmp4E0.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));
_tmp4E1[0]=({struct Cyc_Absyn_NoTypes_struct _tmp4E2;_tmp4E2.tag=0;_tmp4E2.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E2.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4E2;});_tmp4E1;}));_tmp4E0;});_tmp4DF;}));_tmp4DE->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp4DE;});_tmp4DD;});_tmp4DC;});_tmp4DB;});break;case 132: _LL241: {
struct Cyc_List_List*_tmp4E3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4[0]=({
struct Cyc_Declarator_tok_struct _tmp4E5;_tmp4E5.tag=Cyc_Declarator_tok;_tmp4E5.f1=({
struct Cyc_Parse_Declarator*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));_tmp4E6->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4E6->tms=({
struct Cyc_List_List*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp4E8=_cycalloc(sizeof(*_tmp4E8));
_tmp4E8[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp4E9;_tmp4E9.tag=3;_tmp4E9.f1=
_tmp4E3;_tmp4E9.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4E9.f3=0;_tmp4E9;});_tmp4E8;}));_tmp4E7->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp4E7;});
_tmp4E6;});_tmp4E5;});_tmp4E4;});break;}case 133: _LL242: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA[0]=({struct Cyc_Declarator_tok_struct
_tmp4EB;_tmp4EB.tag=Cyc_Declarator_tok;_tmp4EB.f1=({struct Cyc_Parse_Declarator*
_tmp4EC=_cycalloc(sizeof(*_tmp4EC));_tmp4EC->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp4EC->tms=({
struct Cyc_List_List*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp4EE=_cycalloc(sizeof(*_tmp4EE));
_tmp4EE[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp4EF;_tmp4EF.tag=4;_tmp4EF.f1=
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);
_tmp4EF.f2=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp4EF;});_tmp4EE;}));
_tmp4ED->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp4ED;});_tmp4EC;});_tmp4EB;});_tmp4EA;});break;case
134: _LL243: Cyc_Parse_err(_tag_arr("identifier has not been declared as a typedef",
sizeof(char),46),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(void*)({
struct Cyc_Declarator_tok_struct*_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0[0]=({
struct Cyc_Declarator_tok_struct _tmp4F1;_tmp4F1.tag=Cyc_Declarator_tok;_tmp4F1.f1=({
struct Cyc_Parse_Declarator*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2->id=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp4F2->tms=0;_tmp4F2;});_tmp4F1;});_tmp4F0;});break;case
135: _LL244: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4F3=_cycalloc(
sizeof(*_tmp4F3));_tmp4F3[0]=({struct Cyc_Declarator_tok_struct _tmp4F4;_tmp4F4.tag=
Cyc_Declarator_tok;_tmp4F4.f1=({struct Cyc_Parse_Declarator*_tmp4F5=_cycalloc(
sizeof(*_tmp4F5));_tmp4F5->id=Cyc_Absyn_exn_name;_tmp4F5->tms=0;_tmp4F5;});
_tmp4F4;});_tmp4F3;});break;case 136: _LL245: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp4F7;_tmp4F7.tag=Cyc_TypeModifierList_tok;_tmp4F7.f1=({struct Cyc_List_List*
_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp4FA;_tmp4FA.tag=1;_tmp4FA.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4FA.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4FA.f3=Cyc_Absyn_empty_tqual();
_tmp4FA;});_tmp4F9;}));_tmp4F8->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]),0);_tmp4F8;});_tmp4F7;});_tmp4F6;});break;case 137: _LL246:
yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*_tmp4FB=_cycalloc(sizeof(*
_tmp4FB));_tmp4FB[0]=({struct Cyc_TypeModifierList_tok_struct _tmp4FC;_tmp4FC.tag=
Cyc_TypeModifierList_tok;_tmp4FC.f1=({struct Cyc_List_List*_tmp4FD=_cycalloc(
sizeof(*_tmp4FD));_tmp4FD->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp4FF;_tmp4FF.tag=1;_tmp4FF.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4FF.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4FF.f3=Cyc_yyget_TypeQual_tok(
yyvs[yyvsp_offset]);_tmp4FF;});_tmp4FE;}));_tmp4FD->tl=Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0);_tmp4FD;});_tmp4FC;});
_tmp4FB;});break;case 138: _LL247: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp501;_tmp501.tag=Cyc_TypeModifierList_tok;_tmp501.f1=({struct Cyc_List_List*
_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp503=_cycalloc(sizeof(*_tmp503));_tmp503[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp504;_tmp504.tag=1;_tmp504.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp504.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp504.f3=Cyc_Absyn_empty_tqual();
_tmp504;});_tmp503;}));_tmp502->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp502;});_tmp501;});_tmp500;});break;case 139: _LL248: yyval=(
void*)({struct Cyc_TypeModifierList_tok_struct*_tmp505=_cycalloc(sizeof(*_tmp505));
_tmp505[0]=({struct Cyc_TypeModifierList_tok_struct _tmp506;_tmp506.tag=Cyc_TypeModifierList_tok;
_tmp506.f1=({struct Cyc_List_List*_tmp507=_cycalloc(sizeof(*_tmp507));_tmp507->hd=(
void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp508=_cycalloc(sizeof(*
_tmp508));_tmp508[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp509;_tmp509.tag=1;
_tmp509.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp509.f2=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp509.f3=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp509;});_tmp508;}));
_tmp507->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp507;});_tmp506;});_tmp505;});break;case 140: _LL249: yyval=(
void*)({struct Cyc_Pointer_Sort_tok_struct*_tmp50A=_cycalloc(sizeof(*_tmp50A));
_tmp50A[0]=({struct Cyc_Pointer_Sort_tok_struct _tmp50B;_tmp50B.tag=Cyc_Pointer_Sort_tok;
_tmp50B.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*_tmp50C=_cycalloc(
sizeof(*_tmp50C));_tmp50C[0]=({struct Cyc_Absyn_Nullable_ps_struct _tmp50D;_tmp50D.tag=
1;_tmp50D.f1=Cyc_Absyn_exp_unsigned_one;_tmp50D;});_tmp50C;}));_tmp50B;});
_tmp50A;});break;case 141: _LL24A: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp50F;_tmp50F.tag=Cyc_Pointer_Sort_tok;_tmp50F.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp511;_tmp511.tag=0;_tmp511.f1=Cyc_Absyn_exp_unsigned_one;_tmp511;});_tmp510;}));
_tmp50F;});_tmp50E;});break;case 142: _LL24B: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp513;_tmp513.tag=Cyc_Pointer_Sort_tok;_tmp513.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*
_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp515;_tmp515.tag=1;_tmp515.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp515;});_tmp514;}));
_tmp513;});_tmp512;});break;case 143: _LL24C: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp517;_tmp517.tag=Cyc_Pointer_Sort_tok;_tmp517.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp518=_cycalloc(sizeof(*_tmp518));_tmp518[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp519;_tmp519.tag=0;_tmp519.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp519;});_tmp518;}));
_tmp517;});_tmp516;});break;case 144: _LL24D: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp51B;_tmp51B.tag=Cyc_Pointer_Sort_tok;_tmp51B.f1=(void*)((void*)0);_tmp51B;});
_tmp51A;});break;case 145: _LL24E: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp51C=
_cycalloc(sizeof(*_tmp51C));_tmp51C[0]=({struct Cyc_Type_tok_struct _tmp51D;
_tmp51D.tag=Cyc_Type_tok;_tmp51D.f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E->v=(void*)((void*)3);_tmp51E;}),0);
_tmp51D;});_tmp51C;});break;case 146: _LL24F: yyval=yyvs[yyvsp_offset];break;case
147: _LL250: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(
void*)3);yyval=yyvs[yyvsp_offset];break;case 148: _LL251: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp51F=_cycalloc(sizeof(*_tmp51F));_tmp51F[0]=({struct Cyc_Type_tok_struct
_tmp520;_tmp520.tag=Cyc_Type_tok;_tmp520.f1=(void*)Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521->v=(void*)((void*)3);
_tmp521;}),0);_tmp520;});_tmp51F;});break;case 149: _LL252: yyval=yyvs[yyvsp_offset];
break;case 150: _LL253: yyval=(void*)({struct Cyc_TypeQual_tok_struct*_tmp522=
_cycalloc_atomic(sizeof(*_tmp522));_tmp522[0]=({struct Cyc_TypeQual_tok_struct
_tmp523;_tmp523.tag=Cyc_TypeQual_tok;_tmp523.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[yyvsp_offset]));_tmp523;});_tmp522;});break;case 151: _LL254: yyval=(void*)({
struct Cyc_YY1_struct*_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524[0]=({struct Cyc_YY1_struct
_tmp525;_tmp525.tag=Cyc_YY1;_tmp525.f1=({struct _tuple14*_tmp526=_cycalloc(
sizeof(*_tmp526));_tmp526->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp526->f2=0;_tmp526->f3=
0;_tmp526->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp526->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp526;});_tmp525;});_tmp524;});break;case 152: _LL255: yyval=(void*)({struct Cyc_YY1_struct*
_tmp527=_cycalloc(sizeof(*_tmp527));_tmp527[0]=({struct Cyc_YY1_struct _tmp528;
_tmp528.tag=Cyc_YY1;_tmp528.f1=({struct _tuple14*_tmp529=_cycalloc(sizeof(*
_tmp529));_tmp529->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]));_tmp529->f2=1;_tmp529->f3=0;_tmp529->f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp529->f5=Cyc_yyget_Rgnorder_tok(
yyvs[yyvsp_offset]);_tmp529;});_tmp528;});_tmp527;});break;case 153: _LL256: {
struct _tuple2 _tmp52B;struct Cyc_Core_Opt*_tmp52C;struct Cyc_Absyn_Tqual _tmp52D;
void*_tmp52E;struct _tuple2*_tmp52A=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp52B=*_tmp52A;_tmp52C=
_tmp52B.f1;_tmp52D=_tmp52B.f2;_tmp52E=_tmp52B.f3;{struct Cyc_Absyn_VarargInfo*
_tmp52F=({struct Cyc_Absyn_VarargInfo*_tmp533=_cycalloc(sizeof(*_tmp533));_tmp533->name=
_tmp52C;_tmp533->tq=_tmp52D;_tmp533->type=(void*)_tmp52E;_tmp533->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp533;});yyval=(
void*)({struct Cyc_YY1_struct*_tmp530=_cycalloc(sizeof(*_tmp530));_tmp530[0]=({
struct Cyc_YY1_struct _tmp531;_tmp531.tag=Cyc_YY1;_tmp531.f1=({struct _tuple14*
_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532->f1=0;_tmp532->f2=0;_tmp532->f3=
_tmp52F;_tmp532->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp532->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp532;});_tmp531;});_tmp530;});break;}}case 154: _LL257: {struct _tuple2 _tmp535;
struct Cyc_Core_Opt*_tmp536;struct Cyc_Absyn_Tqual _tmp537;void*_tmp538;struct
_tuple2*_tmp534=Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp535=*_tmp534;_tmp536=_tmp535.f1;_tmp537=_tmp535.f2;
_tmp538=_tmp535.f3;{struct Cyc_Absyn_VarargInfo*_tmp539=({struct Cyc_Absyn_VarargInfo*
_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->name=_tmp536;_tmp53D->tq=_tmp537;
_tmp53D->type=(void*)_tmp538;_tmp53D->inject=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp53D;});yyval=(void*)({
struct Cyc_YY1_struct*_tmp53A=_cycalloc(sizeof(*_tmp53A));_tmp53A[0]=({struct Cyc_YY1_struct
_tmp53B;_tmp53B.tag=Cyc_YY1;_tmp53B.f1=({struct _tuple14*_tmp53C=_cycalloc(
sizeof(*_tmp53C));_tmp53C->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp53C->f2=0;_tmp53C->f3=
_tmp539;_tmp53C->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp53C->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp53C;});_tmp53B;});_tmp53A;});break;}}case 155: _LL258: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp53E=_cycalloc(sizeof(*_tmp53E));_tmp53E[0]=({struct Cyc_Type_tok_struct
_tmp53F;_tmp53F.tag=Cyc_Type_tok;_tmp53F.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp540=_cycalloc(
sizeof(*_tmp540));_tmp540[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp541;_tmp541.tag=
1;_tmp541.f1=0;_tmp541;});_tmp540;}));_tmp53F;});_tmp53E;});break;case 156: _LL259:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp542=_cycalloc(sizeof(*_tmp542));
_tmp542[0]=({struct Cyc_Type_tok_struct _tmp543;_tmp543.tag=Cyc_Type_tok;_tmp543.f1=(
void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp544=_cycalloc(
sizeof(*_tmp544));_tmp544[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp545;_tmp545.tag=0;
_tmp545.f1=(void*)Cyc_yyget_Kind_tok(yyvs[yyvsp_offset]);_tmp545;});_tmp544;}));
_tmp543;});_tmp542;});break;case 157: _LL25A: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546[0]=({struct Cyc_TypeOpt_tok_struct
_tmp547;_tmp547.tag=Cyc_TypeOpt_tok;_tmp547.f1=0;_tmp547;});_tmp546;});break;
case 158: _LL25B: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp548=_cycalloc(
sizeof(*_tmp548));_tmp548[0]=({struct Cyc_TypeOpt_tok_struct _tmp549;_tmp549.tag=
Cyc_TypeOpt_tok;_tmp549.f1=({struct Cyc_Core_Opt*_tmp54A=_cycalloc(sizeof(*
_tmp54A));_tmp54A->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp54B=
_cycalloc(sizeof(*_tmp54B));_tmp54B[0]=({struct Cyc_Absyn_JoinEff_struct _tmp54C;
_tmp54C.tag=18;_tmp54C.f1=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp54C;});
_tmp54B;}));_tmp54A;});_tmp549;});_tmp548;});break;case 159: _LL25C: yyval=(void*)({
struct Cyc_Rgnorder_tok_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D[0]=({
struct Cyc_Rgnorder_tok_struct _tmp54E;_tmp54E.tag=Cyc_Rgnorder_tok;_tmp54E.f1=0;
_tmp54E;});_tmp54D;});break;case 160: _LL25D: yyval=yyvs[yyvsp_offset];break;case
161: _LL25E: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp54F=_cycalloc(
sizeof(*_tmp54F));_tmp54F[0]=({struct Cyc_Rgnorder_tok_struct _tmp550;_tmp550.tag=
Cyc_Rgnorder_tok;_tmp550.f1=({struct Cyc_List_List*_tmp551=_cycalloc(sizeof(*
_tmp551));_tmp551->hd=({struct _tuple4*_tmp552=_cycalloc(sizeof(*_tmp552));
_tmp552->f1=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp556;_tmp556.tag=0;_tmp556.f1=(void*)((void*)3);_tmp556;});_tmp555;}));
_tmp552->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),(void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp554;_tmp554.tag=0;_tmp554.f1=(void*)((void*)3);
_tmp554;});_tmp553;}));_tmp552;});_tmp551->tl=0;_tmp551;});_tmp550;});_tmp54F;});
break;case 162: _LL25F: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp557=
_cycalloc(sizeof(*_tmp557));_tmp557[0]=({struct Cyc_Rgnorder_tok_struct _tmp558;
_tmp558.tag=Cyc_Rgnorder_tok;_tmp558.f1=({struct Cyc_List_List*_tmp559=_cycalloc(
sizeof(*_tmp559));_tmp559->hd=({struct _tuple4*_tmp55A=_cycalloc(sizeof(*_tmp55A));
_tmp55A->f1=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp55E;_tmp55E.tag=0;_tmp55E.f1=(void*)((void*)3);_tmp55E;});_tmp55D;}));
_tmp55A->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp55C;_tmp55C.tag=0;_tmp55C.f1=(void*)((void*)3);_tmp55C;});_tmp55B;}));
_tmp55A;});_tmp559->tl=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);_tmp559;});
_tmp558;});_tmp557;});break;case 163: _LL260: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp55F=_cycalloc_atomic(sizeof(*_tmp55F));_tmp55F[0]=({struct Cyc_Bool_tok_struct
_tmp560;_tmp560.tag=Cyc_Bool_tok;_tmp560.f1=0;_tmp560;});_tmp55F;});break;case
164: _LL261: if(Cyc_Std_zstrcmp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),_tag_arr("inject",
sizeof(char),7))!= 0)Cyc_Parse_err(_tag_arr("missing type in function declaration",
sizeof(char),37),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Bool_tok_struct*_tmp561=
_cycalloc_atomic(sizeof(*_tmp561));_tmp561[0]=({struct Cyc_Bool_tok_struct _tmp562;
_tmp562.tag=Cyc_Bool_tok;_tmp562.f1=1;_tmp562;});_tmp561;});break;case 165: _LL262:
yyval=yyvs[yyvsp_offset];break;case 166: _LL263: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563[0]=({struct Cyc_TypeList_tok_struct
_tmp564;_tmp564.tag=Cyc_TypeList_tok;_tmp564.f1=Cyc_List_imp_append(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeList_tok(
yyvs[yyvsp_offset]));_tmp564;});_tmp563;});break;case 167: _LL264: yyval=(void*)({
struct Cyc_TypeList_tok_struct*_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565[0]=({
struct Cyc_TypeList_tok_struct _tmp566;_tmp566.tag=Cyc_TypeList_tok;_tmp566.f1=0;
_tmp566;});_tmp565;});break;case 168: _LL265: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 169: _LL266:
yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp567=_cycalloc(sizeof(*_tmp567));
_tmp567[0]=({struct Cyc_TypeList_tok_struct _tmp568;_tmp568.tag=Cyc_TypeList_tok;
_tmp568.f1=({struct Cyc_List_List*_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp56A=_cycalloc(sizeof(*_tmp56A));
_tmp56A[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp56B;_tmp56B.tag=19;_tmp56B.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp56B;});_tmp56A;}));_tmp569->tl=0;_tmp569;});_tmp568;});_tmp567;});
break;case 170: _LL267: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
yyvsp_offset]),(void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp56C=
_cycalloc(sizeof(*_tmp56C));_tmp56C[0]=({struct Cyc_TypeList_tok_struct _tmp56D;
_tmp56D.tag=Cyc_TypeList_tok;_tmp56D.f1=({struct Cyc_List_List*_tmp56E=_cycalloc(
sizeof(*_tmp56E));_tmp56E->hd=(void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);
_tmp56E->tl=0;_tmp56E;});_tmp56D;});_tmp56C;});break;case 171: _LL268: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F[0]=({struct Cyc_TypeList_tok_struct
_tmp570;_tmp570.tag=Cyc_TypeList_tok;_tmp570.f1=({struct Cyc_List_List*_tmp571=
_cycalloc(sizeof(*_tmp571));_tmp571->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp573;_tmp573.tag=17;_tmp573.f1=(void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);
_tmp573;});_tmp572;}));_tmp571->tl=0;_tmp571;});_tmp570;});_tmp56F;});break;case
172: _LL269: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(void*)({
struct Cyc_TypeList_tok_struct*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574[0]=({
struct Cyc_TypeList_tok_struct _tmp575;_tmp575.tag=Cyc_TypeList_tok;_tmp575.f1=({
struct Cyc_List_List*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp577=_cycalloc(sizeof(*_tmp577));
_tmp577[0]=({struct Cyc_Absyn_AccessEff_struct _tmp578;_tmp578.tag=17;_tmp578.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp578;});_tmp577;}));_tmp576->tl=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);
_tmp576;});_tmp575;});_tmp574;});break;case 173: _LL26A: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp57A;_tmp57A.tag=Cyc_ParamDeclList_tok;_tmp57A.f1=({struct Cyc_List_List*
_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->hd=Cyc_yyget_ParamDecl_tok(yyvs[
yyvsp_offset]);_tmp57B->tl=0;_tmp57B;});_tmp57A;});_tmp579;});break;case 174:
_LL26B: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*_tmp57C=_cycalloc(
sizeof(*_tmp57C));_tmp57C[0]=({struct Cyc_ParamDeclList_tok_struct _tmp57D;_tmp57D.tag=
Cyc_ParamDeclList_tok;_tmp57D.f1=({struct Cyc_List_List*_tmp57E=_cycalloc(sizeof(*
_tmp57E));_tmp57E->hd=Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset]);_tmp57E->tl=
Cyc_yyget_ParamDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp57E;});_tmp57D;});_tmp57C;});break;case 175: _LL26C: {
struct _tuple15 _tmp580;struct Cyc_Absyn_Tqual _tmp581;struct Cyc_List_List*_tmp582;
struct Cyc_List_List*_tmp583;struct _tuple15*_tmp57F=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp580=*_tmp57F;
_tmp581=_tmp580.f1;_tmp582=_tmp580.f2;_tmp583=_tmp580.f3;{struct Cyc_Parse_Declarator
_tmp585;struct _tuple1*_tmp586;struct Cyc_List_List*_tmp587;struct Cyc_Parse_Declarator*
_tmp584=Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]);_tmp585=*_tmp584;_tmp586=
_tmp585.id;_tmp587=_tmp585.tms;{void*_tmp588=Cyc_Parse_speclist2typ(_tmp582,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual
_tmp58A;void*_tmp58B;struct Cyc_List_List*_tmp58C;struct Cyc_List_List*_tmp58D;
struct _tuple6 _tmp589=Cyc_Parse_apply_tms(_tmp581,_tmp588,_tmp583,_tmp587);
_tmp58A=_tmp589.f1;_tmp58B=_tmp589.f2;_tmp58C=_tmp589.f3;_tmp58D=_tmp589.f4;if(
_tmp58C != 0)Cyc_Parse_err(_tag_arr("parameter with bad type params",sizeof(char),
31),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));if(Cyc_Absyn_is_qvar_qualified(_tmp586))Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a namespace",sizeof(char),47),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp58E=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp593=_cycalloc(sizeof(*
_tmp593));_tmp593->v=(*_tmp586).f2;_tmp593;});if(_tmp58D != 0)({void*_tmp58F[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("extra attributes on parameter, ignoring",
sizeof(char),40),_tag_arr(_tmp58F,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp590=_cycalloc(sizeof(*_tmp590));_tmp590[0]=({struct Cyc_ParamDecl_tok_struct
_tmp591;_tmp591.tag=Cyc_ParamDecl_tok;_tmp591.f1=({struct _tuple2*_tmp592=
_cycalloc(sizeof(*_tmp592));_tmp592->f1=_tmp58E;_tmp592->f2=_tmp58A;_tmp592->f3=
_tmp58B;_tmp592;});_tmp591;});_tmp590;});break;}}}}case 176: _LL26D: {struct
_tuple15 _tmp595;struct Cyc_Absyn_Tqual _tmp596;struct Cyc_List_List*_tmp597;struct
Cyc_List_List*_tmp598;struct _tuple15*_tmp594=Cyc_yyget_QualSpecList_tok(yyvs[
yyvsp_offset]);_tmp595=*_tmp594;_tmp596=_tmp595.f1;_tmp597=_tmp595.f2;_tmp598=
_tmp595.f3;{void*_tmp599=Cyc_Parse_speclist2typ(_tmp597,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));if(_tmp598 != 0)({
void*_tmp59A[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(char),38),_tag_arr(_tmp59A,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B[0]=({struct Cyc_ParamDecl_tok_struct
_tmp59C;_tmp59C.tag=Cyc_ParamDecl_tok;_tmp59C.f1=({struct _tuple2*_tmp59D=
_cycalloc(sizeof(*_tmp59D));_tmp59D->f1=0;_tmp59D->f2=_tmp596;_tmp59D->f3=
_tmp599;_tmp59D;});_tmp59C;});_tmp59B;});break;}}case 177: _LL26E: {struct _tuple15
_tmp59F;struct Cyc_Absyn_Tqual _tmp5A0;struct Cyc_List_List*_tmp5A1;struct Cyc_List_List*
_tmp5A2;struct _tuple15*_tmp59E=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp59F=*_tmp59E;_tmp5A0=
_tmp59F.f1;_tmp5A1=_tmp59F.f2;_tmp5A2=_tmp59F.f3;{void*_tmp5A3=Cyc_Parse_speclist2typ(
_tmp5A1,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp5A4=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[yyvsp_offset]))->tms;struct Cyc_Absyn_Tqual _tmp5A6;void*_tmp5A7;struct Cyc_List_List*
_tmp5A8;struct Cyc_List_List*_tmp5A9;struct _tuple6 _tmp5A5=Cyc_Parse_apply_tms(
_tmp5A0,_tmp5A3,_tmp5A2,_tmp5A4);_tmp5A6=_tmp5A5.f1;_tmp5A7=_tmp5A5.f2;_tmp5A8=
_tmp5A5.f3;_tmp5A9=_tmp5A5.f4;if(_tmp5A8 != 0)({void*_tmp5AA[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad type parameters on formal argument, ignoring",
sizeof(char),49),_tag_arr(_tmp5AA,sizeof(void*),0));});if(_tmp5A9 != 0)({void*
_tmp5AB[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(char),38),_tag_arr(_tmp5AB,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp5AC=_cycalloc(sizeof(*_tmp5AC));_tmp5AC[0]=({struct Cyc_ParamDecl_tok_struct
_tmp5AD;_tmp5AD.tag=Cyc_ParamDecl_tok;_tmp5AD.f1=({struct _tuple2*_tmp5AE=
_cycalloc(sizeof(*_tmp5AE));_tmp5AE->f1=0;_tmp5AE->f2=_tmp5A6;_tmp5AE->f3=
_tmp5A7;_tmp5AE;});_tmp5AD;});_tmp5AC;});break;}}case 178: _LL26F: yyval=(void*)({
struct Cyc_IdList_tok_struct*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF[0]=({
struct Cyc_IdList_tok_struct _tmp5B0;_tmp5B0.tag=Cyc_IdList_tok;_tmp5B0.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_IdList_tok(
yyvs[yyvsp_offset]));_tmp5B0;});_tmp5AF;});break;case 179: _LL270: yyval=(void*)({
struct Cyc_IdList_tok_struct*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1[0]=({
struct Cyc_IdList_tok_struct _tmp5B2;_tmp5B2.tag=Cyc_IdList_tok;_tmp5B2.f1=({
struct Cyc_List_List*_tmp5B3=_cycalloc(sizeof(*_tmp5B3));_tmp5B3->hd=({struct
_tagged_arr*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp5B4;});_tmp5B3->tl=0;_tmp5B3;});_tmp5B2;});_tmp5B1;});
break;case 180: _LL271: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5B5=
_cycalloc(sizeof(*_tmp5B5));_tmp5B5[0]=({struct Cyc_IdList_tok_struct _tmp5B6;
_tmp5B6.tag=Cyc_IdList_tok;_tmp5B6.f1=({struct Cyc_List_List*_tmp5B7=_cycalloc(
sizeof(*_tmp5B7));_tmp5B7->hd=({struct _tagged_arr*_tmp5B8=_cycalloc(sizeof(*
_tmp5B8));_tmp5B8[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp5B8;});_tmp5B7->tl=
Cyc_yyget_IdList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5B7;});_tmp5B6;});_tmp5B5;});break;case 181: _LL272: yyval=yyvs[yyvsp_offset];
break;case 182: _LL273: yyval=yyvs[yyvsp_offset];break;case 183: _LL274: yyval=(void*)({
struct Cyc_Exp_tok_struct*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9[0]=({struct
Cyc_Exp_tok_struct _tmp5BA;_tmp5BA.tag=Cyc_Exp_tok;_tmp5BA.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));
_tmp5BB[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5BC;_tmp5BC.tag=34;
_tmp5BC.f1=0;_tmp5BC.f2=0;_tmp5BC;});_tmp5BB;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5BA;});_tmp5B9;});break;case 184: _LL275: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));_tmp5BD[0]=({
struct Cyc_Exp_tok_struct _tmp5BE;_tmp5BE.tag=Cyc_Exp_tok;_tmp5BE.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));
_tmp5BF[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5C0;_tmp5C0.tag=34;
_tmp5C0.f1=0;_tmp5C0.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp5C0;});_tmp5BF;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5BE;});_tmp5BD;});break;case 185: _LL276: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1[0]=({
struct Cyc_Exp_tok_struct _tmp5C2;_tmp5C2.tag=Cyc_Exp_tok;_tmp5C2.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));
_tmp5C3[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5C4;_tmp5C4.tag=34;
_tmp5C4.f1=0;_tmp5C4.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp5C4;});_tmp5C3;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5C2;});_tmp5C1;});break;case 186: _LL277: {struct
Cyc_Absyn_Vardecl*_tmp5C5=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp5CB=
_cycalloc(sizeof(*_tmp5CB));_tmp5CB->f1=(void*)0;_tmp5CB->f2=({struct _tagged_arr*
_tmp5CC=_cycalloc(sizeof(*_tmp5CC));_tmp5CC[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp5CC;});_tmp5CB;}),
Cyc_Absyn_uint_t,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));_tmp5C5->tq=({
struct Cyc_Absyn_Tqual _tmp5C6;_tmp5C6.q_const=1;_tmp5C6.q_volatile=0;_tmp5C6.q_restrict=
1;_tmp5C6;});yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp5C7=_cycalloc(sizeof(*
_tmp5C7));_tmp5C7[0]=({struct Cyc_Exp_tok_struct _tmp5C8;_tmp5C8.tag=Cyc_Exp_tok;
_tmp5C8.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp5CA;_tmp5CA.tag=27;_tmp5CA.f1=_tmp5C5;_tmp5CA.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5CA.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5CA;});_tmp5C9;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmp5C8;});
_tmp5C7;});break;}case 187: _LL278: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp5CD=_cycalloc(sizeof(*_tmp5CD));_tmp5CD[0]=({struct Cyc_InitializerList_tok_struct
_tmp5CE;_tmp5CE.tag=Cyc_InitializerList_tok;_tmp5CE.f1=({struct Cyc_List_List*
_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF->hd=({struct _tuple17*_tmp5D0=
_cycalloc(sizeof(*_tmp5D0));_tmp5D0->f1=0;_tmp5D0->f2=Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]);_tmp5D0;});_tmp5CF->tl=0;_tmp5CF;});_tmp5CE;});_tmp5CD;});break;
case 188: _LL279: yyval=(void*)({struct Cyc_InitializerList_tok_struct*_tmp5D1=
_cycalloc(sizeof(*_tmp5D1));_tmp5D1[0]=({struct Cyc_InitializerList_tok_struct
_tmp5D2;_tmp5D2.tag=Cyc_InitializerList_tok;_tmp5D2.f1=({struct Cyc_List_List*
_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3->hd=({struct _tuple17*_tmp5D4=
_cycalloc(sizeof(*_tmp5D4));_tmp5D4->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D4->f2=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp5D4;});_tmp5D3->tl=0;_tmp5D3;});_tmp5D2;});_tmp5D1;});
break;case 189: _LL27A: yyval=(void*)({struct Cyc_InitializerList_tok_struct*_tmp5D5=
_cycalloc(sizeof(*_tmp5D5));_tmp5D5[0]=({struct Cyc_InitializerList_tok_struct
_tmp5D6;_tmp5D6.tag=Cyc_InitializerList_tok;_tmp5D6.f1=({struct Cyc_List_List*
_tmp5D7=_cycalloc(sizeof(*_tmp5D7));_tmp5D7->hd=({struct _tuple17*_tmp5D8=
_cycalloc(sizeof(*_tmp5D8));_tmp5D8->f1=0;_tmp5D8->f2=Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]);_tmp5D8;});_tmp5D7->tl=Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D7;});_tmp5D6;});
_tmp5D5;});break;case 190: _LL27B: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9[0]=({struct Cyc_InitializerList_tok_struct
_tmp5DA;_tmp5DA.tag=Cyc_InitializerList_tok;_tmp5DA.f1=({struct Cyc_List_List*
_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB->hd=({struct _tuple17*_tmp5DC=
_cycalloc(sizeof(*_tmp5DC));_tmp5DC->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5DC->f2=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp5DC;});_tmp5DB->tl=Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5DB;});_tmp5DA;});
_tmp5D9;});break;case 191: _LL27C: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD[0]=({struct Cyc_DesignatorList_tok_struct
_tmp5DE;_tmp5DE.tag=Cyc_DesignatorList_tok;_tmp5DE.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp5DE;});_tmp5DD;});
break;case 192: _LL27D: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp5DF=
_cycalloc(sizeof(*_tmp5DF));_tmp5DF[0]=({struct Cyc_DesignatorList_tok_struct
_tmp5E0;_tmp5E0.tag=Cyc_DesignatorList_tok;_tmp5E0.f1=({struct Cyc_List_List*
_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[yyvsp_offset]);_tmp5E1->tl=0;_tmp5E1;});_tmp5E0;});_tmp5DF;});break;case 193:
_LL27E: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp5E2=_cycalloc(
sizeof(*_tmp5E2));_tmp5E2[0]=({struct Cyc_DesignatorList_tok_struct _tmp5E3;
_tmp5E3.tag=Cyc_DesignatorList_tok;_tmp5E3.f1=({struct Cyc_List_List*_tmp5E4=
_cycalloc(sizeof(*_tmp5E4));_tmp5E4->hd=(void*)Cyc_yyget_Designator_tok(yyvs[
yyvsp_offset]);_tmp5E4->tl=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E4;});_tmp5E3;});
_tmp5E2;});break;case 194: _LL27F: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp5E5=_cycalloc(sizeof(*_tmp5E5));_tmp5E5[0]=({struct Cyc_Designator_tok_struct
_tmp5E6;_tmp5E6.tag=Cyc_Designator_tok;_tmp5E6.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp5E8;_tmp5E8.tag=0;_tmp5E8.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E8;});_tmp5E7;}));
_tmp5E6;});_tmp5E5;});break;case 195: _LL280: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9[0]=({struct Cyc_Designator_tok_struct
_tmp5EA;_tmp5EA.tag=Cyc_Designator_tok;_tmp5EA.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp5EB=_cycalloc(sizeof(*_tmp5EB));_tmp5EB[0]=({struct Cyc_Absyn_FieldName_struct
_tmp5EC;_tmp5EC.tag=1;_tmp5EC.f1=({struct _tagged_arr*_tmp5ED=_cycalloc(sizeof(*
_tmp5ED));_tmp5ED[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp5ED;});_tmp5EC;});
_tmp5EB;}));_tmp5EA;});_tmp5E9;});break;case 196: _LL281: {void*_tmp5EE=Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));struct Cyc_List_List*
_tmp5EF=(*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f3;if(_tmp5EF != 0)({
void*_tmp5F0[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("ignoring attributes in type",
sizeof(char),28),_tag_arr(_tmp5F0,sizeof(void*),0));});{struct Cyc_Absyn_Tqual
_tmp5F1=(*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f1;yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2[0]=({
struct Cyc_ParamDecl_tok_struct _tmp5F3;_tmp5F3.tag=Cyc_ParamDecl_tok;_tmp5F3.f1=({
struct _tuple2*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->f1=0;_tmp5F4->f2=
_tmp5F1;_tmp5F4->f3=_tmp5EE;_tmp5F4;});_tmp5F3;});_tmp5F2;});break;}}case 197:
_LL282: {void*_tmp5F5=Cyc_Parse_speclist2typ((*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp5F6=(*Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f3;struct Cyc_Absyn_Tqual _tmp5F7=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f1;struct Cyc_List_List*
_tmp5F8=(Cyc_yyget_AbstractDeclarator_tok(yyvs[yyvsp_offset]))->tms;struct
_tuple6 _tmp5F9=Cyc_Parse_apply_tms(_tmp5F7,_tmp5F5,_tmp5F6,_tmp5F8);if(_tmp5F9.f3
!= 0)({void*_tmp5FA[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad type params, ignoring",
sizeof(char),26),_tag_arr(_tmp5FA,sizeof(void*),0));});if(_tmp5F9.f4 != 0)({void*
_tmp5FB[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line),_tag_arr("bad specifiers, ignoring",sizeof(char),
25),_tag_arr(_tmp5FB,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC[0]=({struct Cyc_ParamDecl_tok_struct
_tmp5FD;_tmp5FD.tag=Cyc_ParamDecl_tok;_tmp5FD.f1=({struct _tuple2*_tmp5FE=
_cycalloc(sizeof(*_tmp5FE));_tmp5FE->f1=0;_tmp5FE->f2=_tmp5F9.f1;_tmp5FE->f3=
_tmp5F9.f2;_tmp5FE;});_tmp5FD;});_tmp5FC;});break;}case 198: _LL283: yyval=(void*)({
struct Cyc_Type_tok_struct*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF[0]=({
struct Cyc_Type_tok_struct _tmp600;_tmp600.tag=Cyc_Type_tok;_tmp600.f1=(void*)(*
Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset])).f3;_tmp600;});_tmp5FF;});break;case
199: _LL284: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp601=_cycalloc(sizeof(*
_tmp601));_tmp601[0]=({struct Cyc_Type_tok_struct _tmp602;_tmp602.tag=Cyc_Type_tok;
_tmp602.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp603=_cycalloc(
sizeof(*_tmp603));_tmp603[0]=({struct Cyc_Absyn_JoinEff_struct _tmp604;_tmp604.tag=
18;_tmp604.f1=0;_tmp604;});_tmp603;}));_tmp602;});_tmp601;});break;case 200:
_LL285: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp605=_cycalloc(sizeof(*
_tmp605));_tmp605[0]=({struct Cyc_Type_tok_struct _tmp606;_tmp606.tag=Cyc_Type_tok;
_tmp606.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp607=_cycalloc(
sizeof(*_tmp607));_tmp607[0]=({struct Cyc_Absyn_JoinEff_struct _tmp608;_tmp608.tag=
18;_tmp608.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp608;});_tmp607;}));_tmp606;});_tmp605;});break;case 201:
_LL286: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp609=_cycalloc(sizeof(*
_tmp609));_tmp609[0]=({struct Cyc_Type_tok_struct _tmp60A;_tmp60A.tag=Cyc_Type_tok;
_tmp60A.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp60B=_cycalloc(
sizeof(*_tmp60B));_tmp60B[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp60C;_tmp60C.tag=
19;_tmp60C.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp60C;});_tmp60B;}));_tmp60A;});_tmp609;});break;case
202: _LL287: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp60D=_cycalloc(sizeof(*
_tmp60D));_tmp60D[0]=({struct Cyc_Type_tok_struct _tmp60E;_tmp60E.tag=Cyc_Type_tok;
_tmp60E.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp60F=_cycalloc(
sizeof(*_tmp60F));_tmp60F[0]=({struct Cyc_Absyn_JoinEff_struct _tmp610;_tmp610.tag=
18;_tmp610.f1=({struct Cyc_List_List*_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp611->tl=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp611;});_tmp610;});
_tmp60F;}));_tmp60E;});_tmp60D;});break;case 203: _LL288: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612[0]=({struct Cyc_TypeList_tok_struct
_tmp613;_tmp613.tag=Cyc_TypeList_tok;_tmp613.f1=({struct Cyc_List_List*_tmp614=
_cycalloc(sizeof(*_tmp614));_tmp614->hd=(void*)Cyc_yyget_Type_tok(yyvs[
yyvsp_offset]);_tmp614->tl=0;_tmp614;});_tmp613;});_tmp612;});break;case 204:
_LL289: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp615=_cycalloc(sizeof(*
_tmp615));_tmp615[0]=({struct Cyc_TypeList_tok_struct _tmp616;_tmp616.tag=Cyc_TypeList_tok;
_tmp616.f1=({struct Cyc_List_List*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->hd=(
void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);_tmp617->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp617;});_tmp616;});
_tmp615;});break;case 205: _LL28A: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp619;_tmp619.tag=Cyc_AbstractDeclarator_tok;_tmp619.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]);_tmp61A;});_tmp619;});_tmp618;});break;case 206: _LL28B: yyval=
yyvs[yyvsp_offset];break;case 207: _LL28C: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp61C;_tmp61C.tag=Cyc_AbstractDeclarator_tok;_tmp61C.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp61D=_cycalloc(sizeof(*_tmp61D));_tmp61D->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[yyvsp_offset]))->tms);_tmp61D;});_tmp61C;});_tmp61B;});break;case 208: _LL28D:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 209:
_LL28E: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp61E=_cycalloc(
sizeof(*_tmp61E));_tmp61E[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp61F;
_tmp61F.tag=Cyc_AbstractDeclarator_tok;_tmp61F.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->tms=({struct Cyc_List_List*_tmp621=
_cycalloc(sizeof(*_tmp621));_tmp621->hd=(void*)((void*)0);_tmp621->tl=0;_tmp621;});
_tmp620;});_tmp61F;});_tmp61E;});break;case 210: _LL28F: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp623;_tmp623.tag=Cyc_AbstractDeclarator_tok;_tmp623.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->tms=({struct Cyc_List_List*_tmp625=
_cycalloc(sizeof(*_tmp625));_tmp625->hd=(void*)((void*)0);_tmp625->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->tms;_tmp625;});
_tmp624;});_tmp623;});_tmp622;});break;case 211: _LL290: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp627;_tmp627.tag=Cyc_AbstractDeclarator_tok;_tmp627.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->tms=({struct Cyc_List_List*_tmp629=
_cycalloc(sizeof(*_tmp629));_tmp629->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp62B;_tmp62B.tag=0;_tmp62B.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp62B;});_tmp62A;}));
_tmp629->tl=0;_tmp629;});_tmp628;});_tmp627;});_tmp626;});break;case 212: _LL291:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp62C=_cycalloc(sizeof(*
_tmp62C));_tmp62C[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp62D;_tmp62D.tag=
Cyc_AbstractDeclarator_tok;_tmp62D.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E->tms=({struct Cyc_List_List*_tmp62F=
_cycalloc(sizeof(*_tmp62F));_tmp62F->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp631;_tmp631.tag=0;_tmp631.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp631;});_tmp630;}));
_tmp62F->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp62F;});_tmp62E;});
_tmp62D;});_tmp62C;});break;case 213: _LL292: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp633;_tmp633.tag=Cyc_AbstractDeclarator_tok;_tmp633.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->tms=({struct Cyc_List_List*_tmp635=
_cycalloc(sizeof(*_tmp635));_tmp635->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp637;_tmp637.tag=2;_tmp637.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp639;_tmp639.tag=1;_tmp639.f1=0;_tmp639.f2=0;_tmp639.f3=0;_tmp639.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp639.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp639;});_tmp638;}));
_tmp637;});_tmp636;}));_tmp635->tl=0;_tmp635;});_tmp634;});_tmp633;});_tmp632;});
break;case 214: _LL293: {struct _tuple14 _tmp63B;struct Cyc_List_List*_tmp63C;int
_tmp63D;struct Cyc_Absyn_VarargInfo*_tmp63E;struct Cyc_Core_Opt*_tmp63F;struct Cyc_List_List*
_tmp640;struct _tuple14*_tmp63A=Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp63B=*_tmp63A;_tmp63C=_tmp63B.f1;_tmp63D=_tmp63B.f2;
_tmp63E=_tmp63B.f3;_tmp63F=_tmp63B.f4;_tmp640=_tmp63B.f5;yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp642;_tmp642.tag=Cyc_AbstractDeclarator_tok;
_tmp642.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp643=_cycalloc(sizeof(*
_tmp643));_tmp643->tms=({struct Cyc_List_List*_tmp644=_cycalloc(sizeof(*_tmp644));
_tmp644->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp645=
_cycalloc(sizeof(*_tmp645));_tmp645[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp646;_tmp646.tag=2;_tmp646.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp648;_tmp648.tag=1;_tmp648.f1=_tmp63C;_tmp648.f2=_tmp63D;_tmp648.f3=_tmp63E;
_tmp648.f4=_tmp63F;_tmp648.f5=_tmp640;_tmp648;});_tmp647;}));_tmp646;});_tmp645;}));
_tmp644->tl=0;_tmp644;});_tmp643;});_tmp642;});_tmp641;});break;}case 215: _LL294:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp649=_cycalloc(sizeof(*
_tmp649));_tmp649[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp64A;_tmp64A.tag=
Cyc_AbstractDeclarator_tok;_tmp64A.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B->tms=({struct Cyc_List_List*_tmp64C=
_cycalloc(sizeof(*_tmp64C));_tmp64C->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp64E;_tmp64E.tag=2;_tmp64E.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp650;_tmp650.tag=1;_tmp650.f1=0;_tmp650.f2=0;_tmp650.f3=0;_tmp650.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp650.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp650;});_tmp64F;}));
_tmp64E;});_tmp64D;}));_tmp64C->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp64C;});_tmp64B;});
_tmp64A;});_tmp649;});break;case 216: _LL295: {struct _tuple14 _tmp652;struct Cyc_List_List*
_tmp653;int _tmp654;struct Cyc_Absyn_VarargInfo*_tmp655;struct Cyc_Core_Opt*_tmp656;
struct Cyc_List_List*_tmp657;struct _tuple14*_tmp651=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp652=*_tmp651;_tmp653=
_tmp652.f1;_tmp654=_tmp652.f2;_tmp655=_tmp652.f3;_tmp656=_tmp652.f4;_tmp657=
_tmp652.f5;yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp658=
_cycalloc(sizeof(*_tmp658));_tmp658[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp659;_tmp659.tag=Cyc_AbstractDeclarator_tok;_tmp659.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->tms=({struct Cyc_List_List*_tmp65B=
_cycalloc(sizeof(*_tmp65B));_tmp65B->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp65D;_tmp65D.tag=2;_tmp65D.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp65F;_tmp65F.tag=1;_tmp65F.f1=_tmp653;_tmp65F.f2=_tmp654;_tmp65F.f3=_tmp655;
_tmp65F.f4=_tmp656;_tmp65F.f5=_tmp657;_tmp65F;});_tmp65E;}));_tmp65D;});_tmp65C;}));
_tmp65B->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp65B;});_tmp65A;});
_tmp659;});_tmp658;});break;}case 217: _LL296: {struct Cyc_List_List*_tmp660=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp661=_cycalloc(sizeof(*
_tmp661));_tmp661[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp662;_tmp662.tag=
Cyc_AbstractDeclarator_tok;_tmp662.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663->tms=({struct Cyc_List_List*_tmp664=
_cycalloc(sizeof(*_tmp664));_tmp664->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp666;_tmp666.tag=3;_tmp666.f1=_tmp660;_tmp666.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp666.f3=0;_tmp666;});_tmp665;}));_tmp664->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp664;});
_tmp663;});_tmp662;});_tmp661;});break;}case 218: _LL297: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp668;_tmp668.tag=Cyc_AbstractDeclarator_tok;_tmp668.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->tms=({struct Cyc_List_List*_tmp66A=
_cycalloc(sizeof(*_tmp66A));_tmp66A->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp66C;_tmp66C.tag=4;_tmp66C.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp66C.f2=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp66C;});_tmp66B;}));_tmp66A->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp66A;});
_tmp669;});_tmp668;});_tmp667;});break;case 219: _LL298: yyval=yyvs[yyvsp_offset];
break;case 220: _LL299: yyval=yyvs[yyvsp_offset];break;case 221: _LL29A: yyval=yyvs[
yyvsp_offset];break;case 222: _LL29B: yyval=yyvs[yyvsp_offset];break;case 223: _LL29C:
yyval=yyvs[yyvsp_offset];break;case 224: _LL29D: yyval=yyvs[yyvsp_offset];break;
case 225: _LL29E: if(Cyc_Std_zstrcmp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tag_arr("`H",sizeof(
char),3))== 0)Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",sizeof(
char),33),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677->name=({struct _tagged_arr*_tmp67A=
_cycalloc(sizeof(*_tmp67A));_tmp67A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp67A;});_tmp677->identity=
0;_tmp677->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp678=_cycalloc(
sizeof(*_tmp678));_tmp678[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp679;_tmp679.tag=0;
_tmp679.f1=(void*)((void*)3);_tmp679;});_tmp678;}));_tmp677;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675[0]=({
struct Cyc_Absyn_VarType_struct _tmp676;_tmp676.tag=1;_tmp676.f1=tv;_tmp676;});
_tmp675;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp66D=_cycalloc(sizeof(*
_tmp66D));_tmp66D[0]=({struct Cyc_Stmt_tok_struct _tmp66E;_tmp66E.tag=Cyc_Stmt_tok;
_tmp66E.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp66F=
_cycalloc(sizeof(*_tmp66F));_tmp66F[0]=({struct Cyc_Absyn_Region_s_struct _tmp670;
_tmp670.tag=18;_tmp670.f1=tv;_tmp670.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp671=_cycalloc(sizeof(*_tmp671));_tmp671->f1=(void*)0;_tmp671->f2=({struct
_tagged_arr*_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp672;});_tmp671;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp673=_cycalloc(sizeof(*_tmp673));
_tmp673[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp674;_tmp674.tag=15;_tmp674.f1=(
void*)t;_tmp674;});_tmp673;}),0);_tmp670.f3=0;_tmp670.f4=Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]);_tmp670;});_tmp66F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp66E;});_tmp66D;});break;}case 226: _LL29F: if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tag_arr("H",sizeof(char),2))== 0)Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685->name=({
struct _tagged_arr*_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688[0]=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp68A;_tmp68A.tag=0;_tmp68A.f1=(
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);{void*_tmp689[1]={& _tmp68A};Cyc_Std_aprintf(_tag_arr("`%s",
sizeof(char),4),_tag_arr(_tmp689,sizeof(void*),1));}});_tmp688;});_tmp685->identity=
0;_tmp685->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp686=_cycalloc(
sizeof(*_tmp686));_tmp686[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp687;_tmp687.tag=0;
_tmp687.f1=(void*)((void*)3);_tmp687;});_tmp686;}));_tmp685;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683[0]=({
struct Cyc_Absyn_VarType_struct _tmp684;_tmp684.tag=1;_tmp684.f1=tv;_tmp684;});
_tmp683;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp67B=_cycalloc(sizeof(*
_tmp67B));_tmp67B[0]=({struct Cyc_Stmt_tok_struct _tmp67C;_tmp67C.tag=Cyc_Stmt_tok;
_tmp67C.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp67D=
_cycalloc(sizeof(*_tmp67D));_tmp67D[0]=({struct Cyc_Absyn_Region_s_struct _tmp67E;
_tmp67E.tag=18;_tmp67E.f1=tv;_tmp67E.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F->f1=(void*)0;_tmp67F->f2=({struct
_tagged_arr*_tmp680=_cycalloc(sizeof(*_tmp680));_tmp680[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp680;});_tmp67F;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp681=_cycalloc(sizeof(*_tmp681));
_tmp681[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp682;_tmp682.tag=15;_tmp682.f1=(
void*)t;_tmp682;});_tmp681;}),0);_tmp67E.f3=0;_tmp67E.f4=Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]);_tmp67E;});_tmp67D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp67C;});_tmp67B;});break;}case 227: _LL2A0: if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tag_arr("resetable",sizeof(char),10))!= 0)Cyc_Parse_err(_tag_arr("expecting [resetable]",
sizeof(char),22),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tag_arr("`H",sizeof(char),3))== 0)Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695->name=({
struct _tagged_arr*_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp698;});_tmp695->identity=
0;_tmp695->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp696=_cycalloc(
sizeof(*_tmp696));_tmp696[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp697;_tmp697.tag=0;
_tmp697.f1=(void*)((void*)3);_tmp697;});_tmp696;}));_tmp695;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693[0]=({
struct Cyc_Absyn_VarType_struct _tmp694;_tmp694.tag=1;_tmp694.f1=tv;_tmp694;});
_tmp693;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp68B=_cycalloc(sizeof(*
_tmp68B));_tmp68B[0]=({struct Cyc_Stmt_tok_struct _tmp68C;_tmp68C.tag=Cyc_Stmt_tok;
_tmp68C.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp68D=
_cycalloc(sizeof(*_tmp68D));_tmp68D[0]=({struct Cyc_Absyn_Region_s_struct _tmp68E;
_tmp68E.tag=18;_tmp68E.f1=tv;_tmp68E.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->f1=(void*)0;_tmp68F->f2=({struct
_tagged_arr*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp690;});_tmp68F;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp691=_cycalloc(sizeof(*_tmp691));
_tmp691[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp692;_tmp692.tag=15;_tmp692.f1=(
void*)t;_tmp692;});_tmp691;}),0);_tmp68E.f3=1;_tmp68E.f4=Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]);_tmp68E;});_tmp68D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp68C;});_tmp68B;});break;}case 228: _LL2A1: if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tag_arr("resetable",sizeof(char),10))!= 0)Cyc_Parse_err(_tag_arr("expecting `resetable'",
sizeof(char),22),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tag_arr("H",sizeof(char),2))== 0)Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3->name=({
struct _tagged_arr*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6[0]=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp6A8;_tmp6A8.tag=0;_tmp6A8.f1=(
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);{void*_tmp6A7[1]={& _tmp6A8};Cyc_Std_aprintf(_tag_arr("`%s",
sizeof(char),4),_tag_arr(_tmp6A7,sizeof(void*),1));}});_tmp6A6;});_tmp6A3->identity=
0;_tmp6A3->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6A4=_cycalloc(
sizeof(*_tmp6A4));_tmp6A4[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6A5;_tmp6A5.tag=0;
_tmp6A5.f1=(void*)((void*)3);_tmp6A5;});_tmp6A4;}));_tmp6A3;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1[0]=({
struct Cyc_Absyn_VarType_struct _tmp6A2;_tmp6A2.tag=1;_tmp6A2.f1=tv;_tmp6A2;});
_tmp6A1;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp699=_cycalloc(sizeof(*
_tmp699));_tmp699[0]=({struct Cyc_Stmt_tok_struct _tmp69A;_tmp69A.tag=Cyc_Stmt_tok;
_tmp69A.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp69B=
_cycalloc(sizeof(*_tmp69B));_tmp69B[0]=({struct Cyc_Absyn_Region_s_struct _tmp69C;
_tmp69C.tag=18;_tmp69C.f1=tv;_tmp69C.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D->f1=(void*)0;_tmp69D->f2=({struct
_tagged_arr*_tmp69E=_cycalloc(sizeof(*_tmp69E));_tmp69E[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp69E;});_tmp69D;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp69F=_cycalloc(sizeof(*_tmp69F));
_tmp69F[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp6A0;_tmp6A0.tag=15;_tmp6A0.f1=(
void*)t;_tmp6A0;});_tmp69F;}),0);_tmp69C.f3=1;_tmp69C.f4=Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]);_tmp69C;});_tmp69B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp69A;});_tmp699;});break;}case 229: _LL2A2: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9[0]=({
struct Cyc_Stmt_tok_struct _tmp6AA;_tmp6AA.tag=Cyc_Stmt_tok;_tmp6AA.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_ResetRegion_s_struct*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));
_tmp6AB[0]=({struct Cyc_Absyn_ResetRegion_s_struct _tmp6AC;_tmp6AC.tag=20;_tmp6AC.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp6AC;});_tmp6AB;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6AA;});_tmp6A9;});break;case 230: _LL2A3: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD[0]=({
struct Cyc_Stmt_tok_struct _tmp6AE;_tmp6AE.tag=Cyc_Stmt_tok;_tmp6AE.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Cut_s_struct*_tmp6AF=_cycalloc(sizeof(*_tmp6AF));_tmp6AF[
0]=({struct Cyc_Absyn_Cut_s_struct _tmp6B0;_tmp6B0.tag=13;_tmp6B0.f1=Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]);_tmp6B0;});_tmp6AF;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6AE;});_tmp6AD;});break;case 231: _LL2A4: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6B1=_cycalloc(sizeof(*_tmp6B1));_tmp6B1[0]=({
struct Cyc_Stmt_tok_struct _tmp6B2;_tmp6B2.tag=Cyc_Stmt_tok;_tmp6B2.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmp6B3=_cycalloc(sizeof(*_tmp6B3));
_tmp6B3[0]=({struct Cyc_Absyn_Splice_s_struct _tmp6B4;_tmp6B4.tag=14;_tmp6B4.f1=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp6B4;});_tmp6B3;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6B2;});_tmp6B1;});break;case 232: _LL2A5: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5[0]=({
struct Cyc_Stmt_tok_struct _tmp6B6;_tmp6B6.tag=Cyc_Stmt_tok;_tmp6B6.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));
_tmp6B7[0]=({struct Cyc_Absyn_Label_s_struct _tmp6B8;_tmp6B8.tag=15;_tmp6B8.f1=({
struct _tagged_arr*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6B9;});_tmp6B8.f2=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp6B8;});_tmp6B7;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6B6;});_tmp6B5;});break;case 233: _LL2A6: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA[0]=({
struct Cyc_Stmt_tok_struct _tmp6BB;_tmp6BB.tag=Cyc_Stmt_tok;_tmp6BB.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmp6BB;});_tmp6BA;});break;case 234: _LL2A7: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC[0]=({struct Cyc_Stmt_tok_struct
_tmp6BD;_tmp6BD.tag=Cyc_Stmt_tok;_tmp6BD.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6BD;});_tmp6BC;});break;case 235: _LL2A8: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE[0]=({
struct Cyc_Stmt_tok_struct _tmp6BF;_tmp6BF.tag=Cyc_Stmt_tok;_tmp6BF.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6BF;});
_tmp6BE;});break;case 236: _LL2A9: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 237: _LL2AA: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0[0]=({struct Cyc_Stmt_tok_struct
_tmp6C1;_tmp6C1.tag=Cyc_Stmt_tok;_tmp6C1.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(0));_tmp6C1;});_tmp6C0;});break;case 238:
_LL2AB: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6C2=_cycalloc(sizeof(*
_tmp6C2));_tmp6C2[0]=({struct Cyc_Stmt_tok_struct _tmp6C3;_tmp6C3.tag=Cyc_Stmt_tok;
_tmp6C3.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]));_tmp6C3;});_tmp6C2;});break;case 239: _LL2AC: yyval=yyvs[
yyvsp_offset];break;case 240: _LL2AD: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6C4=_cycalloc(sizeof(*_tmp6C4));_tmp6C4[0]=({struct Cyc_Stmt_tok_struct
_tmp6C5;_tmp6C5.tag=Cyc_Stmt_tok;_tmp6C5.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6C5;});_tmp6C4;});break;case 241: _LL2AE: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6[0]=({
struct Cyc_Stmt_tok_struct _tmp6C7;_tmp6C7.tag=Cyc_Stmt_tok;_tmp6C7.f1=Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*_tmp6C8=_cycalloc(
sizeof(*_tmp6C8));_tmp6C8[0]=({struct Cyc_Absyn_Fn_d_struct _tmp6C9;_tmp6C9.tag=1;
_tmp6C9.f1=Cyc_yyget_FnDecl_tok(yyvs[yyvsp_offset]);_tmp6C9;});_tmp6C8;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line)),Cyc_Absyn_skip_stmt(
0));_tmp6C7;});_tmp6C6;});break;case 242: _LL2AF: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA[0]=({struct Cyc_Stmt_tok_struct
_tmp6CB;_tmp6CB.tag=Cyc_Stmt_tok;_tmp6CB.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));_tmp6CC[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp6CD;_tmp6CD.tag=1;_tmp6CD.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6CD;});_tmp6CC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]));_tmp6CB;});
_tmp6CA;});break;case 243: _LL2B0: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6CE=
_cycalloc(sizeof(*_tmp6CE));_tmp6CE[0]=({struct Cyc_Stmt_tok_struct _tmp6CF;
_tmp6CF.tag=Cyc_Stmt_tok;_tmp6CF.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6CF;});_tmp6CE;});break;case 244: _LL2B1: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0[0]=({
struct Cyc_Stmt_tok_struct _tmp6D1;_tmp6D1.tag=Cyc_Stmt_tok;_tmp6D1.f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6D1;});_tmp6D0;});break;case 245: _LL2B2: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2[0]=({
struct Cyc_Stmt_tok_struct _tmp6D3;_tmp6D3.tag=Cyc_Stmt_tok;_tmp6D3.f1=Cyc_Absyn_switch_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6D3;});_tmp6D2;});break;case 246: _LL2B3: if(Cyc_Std_strcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tag_arr("C",sizeof(char),2))!= 0)Cyc_Parse_err(_tag_arr("only switch \"C\" { ... } is allowed",
sizeof(char),35),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6D4=
_cycalloc(sizeof(*_tmp6D4));_tmp6D4[0]=({struct Cyc_Stmt_tok_struct _tmp6D5;
_tmp6D5.tag=Cyc_Stmt_tok;_tmp6D5.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_SwitchC_s_struct*
_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6[0]=({struct Cyc_Absyn_SwitchC_s_struct
_tmp6D7;_tmp6D7.tag=10;_tmp6D7.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp6D7.f2=Cyc_yyget_SwitchCClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D7;});_tmp6D6;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6D5;});
_tmp6D4;});break;case 247: _LL2B4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6D8=
_cycalloc(sizeof(*_tmp6D8));_tmp6D8[0]=({struct Cyc_Stmt_tok_struct _tmp6D9;
_tmp6D9.tag=Cyc_Stmt_tok;_tmp6D9.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6D9;});_tmp6D8;});break;case 248: _LL2B5: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));
_tmp6DA[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp6DB;_tmp6DB.tag=Cyc_SwitchClauseList_tok;
_tmp6DB.f1=0;_tmp6DB;});_tmp6DA;});break;case 249: _LL2B6: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6DC=_cycalloc(sizeof(*_tmp6DC));_tmp6DC[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6DD;_tmp6DD.tag=Cyc_SwitchClauseList_tok;_tmp6DD.f1=({struct Cyc_List_List*
_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp6DF->pat_vars=0;_tmp6DF->where_clause=0;
_tmp6DF->body=Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp6DF->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6DF;});_tmp6DE->tl=0;_tmp6DE;});_tmp6DD;});_tmp6DC;});
break;case 250: _LL2B7: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6E1;_tmp6E1.tag=Cyc_SwitchClauseList_tok;_tmp6E1.f1=({struct Cyc_List_List*
_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6E3=_cycalloc(sizeof(*_tmp6E3));_tmp6E3->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6E3->pat_vars=0;
_tmp6E3->where_clause=0;_tmp6E3->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp6E3->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line);_tmp6E3;});_tmp6E2->tl=
Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmp6E2;});_tmp6E1;});_tmp6E0;});
break;case 251: _LL2B8: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6E4=_cycalloc(sizeof(*_tmp6E4));_tmp6E4[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6E5;_tmp6E5.tag=Cyc_SwitchClauseList_tok;_tmp6E5.f1=({struct Cyc_List_List*
_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6E7=_cycalloc(sizeof(*_tmp6E7));_tmp6E7->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6E7->pat_vars=0;
_tmp6E7->where_clause=0;_tmp6E7->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E7->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp6E7;});
_tmp6E6->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmp6E6;});
_tmp6E5;});_tmp6E4;});break;case 252: _LL2B9: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6E9;_tmp6E9.tag=Cyc_SwitchClauseList_tok;_tmp6E9.f1=({struct Cyc_List_List*
_tmp6EA=_cycalloc(sizeof(*_tmp6EA));_tmp6EA->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6EB=_cycalloc(sizeof(*_tmp6EB));_tmp6EB->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp6EB->pat_vars=0;
_tmp6EB->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6EB->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp6EB->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6EB;});_tmp6EA->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmp6EA;});_tmp6E9;});_tmp6E8;});break;case 253: _LL2BA: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp6EC=_cycalloc(sizeof(*_tmp6EC));
_tmp6EC[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp6ED;_tmp6ED.tag=Cyc_SwitchClauseList_tok;
_tmp6ED.f1=({struct Cyc_List_List*_tmp6EE=_cycalloc(sizeof(*_tmp6EE));_tmp6EE->hd=({
struct Cyc_Absyn_Switch_clause*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp6EF->pat_vars=0;_tmp6EF->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6EF->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6EF->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6EF;});_tmp6EE->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmp6EE;});_tmp6ED;});_tmp6EC;});break;case 254: _LL2BB: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp6F0=_cycalloc(sizeof(*_tmp6F0));
_tmp6F0[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp6F1;_tmp6F1.tag=Cyc_SwitchCClauseList_tok;
_tmp6F1.f1=0;_tmp6F1;});_tmp6F0;});break;case 255: _LL2BC: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmp6F3;_tmp6F3.tag=Cyc_SwitchCClauseList_tok;_tmp6F3.f1=({struct Cyc_List_List*
_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4->hd=({struct Cyc_Absyn_SwitchC_clause*
_tmp6F5=_cycalloc(sizeof(*_tmp6F5));_tmp6F5->cnst_exp=0;_tmp6F5->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Absyn_break_stmt(0),0);_tmp6F5->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line);_tmp6F5;});_tmp6F4->tl=
0;_tmp6F4;});_tmp6F3;});_tmp6F2;});break;case 256: _LL2BD: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmp6F6=_cycalloc(sizeof(*_tmp6F6));_tmp6F6[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmp6F7;_tmp6F7.tag=Cyc_SwitchCClauseList_tok;_tmp6F7.f1=({struct Cyc_List_List*
_tmp6F8=_cycalloc(sizeof(*_tmp6F8));_tmp6F8->hd=({struct Cyc_Absyn_SwitchC_clause*
_tmp6F9=_cycalloc(sizeof(*_tmp6F9));_tmp6F9->cnst_exp=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6F9->body=Cyc_Absyn_fallthru_stmt(
0,0);_tmp6F9->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6F9;});_tmp6F8->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmp6F8;});_tmp6F7;});_tmp6F6;});break;case 257: _LL2BE: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));
_tmp6FA[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp6FB;_tmp6FB.tag=Cyc_SwitchCClauseList_tok;
_tmp6FB.f1=({struct Cyc_List_List*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC->hd=({
struct Cyc_Absyn_SwitchC_clause*_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD->cnst_exp=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp6FD->body=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Absyn_fallthru_stmt(
0,0),0);_tmp6FD->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6FD;});_tmp6FC->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmp6FC;});_tmp6FB;});_tmp6FA;});break;case 258: _LL2BF: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE[0]=({
struct Cyc_Stmt_tok_struct _tmp6FF;_tmp6FF.tag=Cyc_Stmt_tok;_tmp6FF.f1=Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6FF;});_tmp6FE;});break;case 259: _LL2C0: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp700=_cycalloc(sizeof(*_tmp700));_tmp700[0]=({
struct Cyc_Stmt_tok_struct _tmp701;_tmp701.tag=Cyc_Stmt_tok;_tmp701.f1=Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[yylsp_offset]).last_line));_tmp701;});
_tmp700;});break;case 260: _LL2C1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp702=
_cycalloc(sizeof(*_tmp702));_tmp702[0]=({struct Cyc_Stmt_tok_struct _tmp703;
_tmp703.tag=Cyc_Stmt_tok;_tmp703.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));_tmp703;});
_tmp702;});break;case 261: _LL2C2: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp704=
_cycalloc(sizeof(*_tmp704));_tmp704[0]=({struct Cyc_Stmt_tok_struct _tmp705;
_tmp705.tag=Cyc_Stmt_tok;_tmp705.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp705;});_tmp704;});break;case 262: _LL2C3: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp706=_cycalloc(sizeof(*_tmp706));_tmp706[0]=({
struct Cyc_Stmt_tok_struct _tmp707;_tmp707.tag=Cyc_Stmt_tok;_tmp707.f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[yylsp_offset]).last_line));_tmp707;});
_tmp706;});break;case 263: _LL2C4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp708=
_cycalloc(sizeof(*_tmp708));_tmp708[0]=({struct Cyc_Stmt_tok_struct _tmp709;
_tmp709.tag=Cyc_Stmt_tok;_tmp709.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp709;});
_tmp708;});break;case 264: _LL2C5: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp70A=
_cycalloc(sizeof(*_tmp70A));_tmp70A[0]=({struct Cyc_Stmt_tok_struct _tmp70B;
_tmp70B.tag=Cyc_Stmt_tok;_tmp70B.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp70B;});_tmp70A;});break;case 265: _LL2C6: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp70C=_cycalloc(sizeof(*_tmp70C));_tmp70C[0]=({
struct Cyc_Stmt_tok_struct _tmp70D;_tmp70D.tag=Cyc_Stmt_tok;_tmp70D.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp70D;});_tmp70C;});break;case 266: _LL2C7: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp70E=_cycalloc(sizeof(*_tmp70E));_tmp70E[0]=({
struct Cyc_Stmt_tok_struct _tmp70F;_tmp70F.tag=Cyc_Stmt_tok;_tmp70F.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp70F;});_tmp70E;});break;case 267: _LL2C8: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710[0]=({
struct Cyc_Stmt_tok_struct _tmp711;_tmp711.tag=Cyc_Stmt_tok;_tmp711.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp711;});_tmp710;});break;case 268: _LL2C9: {struct
Cyc_List_List*_tmp712=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*_tmp713=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp714=_cycalloc(sizeof(*_tmp714));_tmp714[0]=({
struct Cyc_Stmt_tok_struct _tmp715;_tmp715.tag=Cyc_Stmt_tok;_tmp715.f1=Cyc_Parse_flatten_declarations(
_tmp712,_tmp713);_tmp715;});_tmp714;});break;}case 269: _LL2CA: {struct Cyc_List_List*
_tmp716=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp717=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp718=
_cycalloc(sizeof(*_tmp718));_tmp718[0]=({struct Cyc_Stmt_tok_struct _tmp719;
_tmp719.tag=Cyc_Stmt_tok;_tmp719.f1=Cyc_Parse_flatten_declarations(_tmp716,
_tmp717);_tmp719;});_tmp718;});break;}case 270: _LL2CB: {struct Cyc_List_List*
_tmp71A=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp71B=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp71C=
_cycalloc(sizeof(*_tmp71C));_tmp71C[0]=({struct Cyc_Stmt_tok_struct _tmp71D;
_tmp71D.tag=Cyc_Stmt_tok;_tmp71D.f1=Cyc_Parse_flatten_declarations(_tmp71A,
_tmp71B);_tmp71D;});_tmp71C;});break;}case 271: _LL2CC: {struct Cyc_List_List*
_tmp71E=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp71F=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp720=
_cycalloc(sizeof(*_tmp720));_tmp720[0]=({struct Cyc_Stmt_tok_struct _tmp721;
_tmp721.tag=Cyc_Stmt_tok;_tmp721.f1=Cyc_Parse_flatten_declarations(_tmp71E,
_tmp71F);_tmp721;});_tmp720;});break;}case 272: _LL2CD: {struct Cyc_List_List*
_tmp722=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_decl2vardecl,Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp723=_cycalloc(sizeof(*_tmp723));_tmp723[0]=({
struct Cyc_Stmt_tok_struct _tmp724;_tmp724.tag=Cyc_Stmt_tok;_tmp724.f1=Cyc_Absyn_forarray_stmt(
_tmp722,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp724;});_tmp723;});break;}case 273: _LL2CE: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp725=_cycalloc(sizeof(*_tmp725));_tmp725[0]=({
struct Cyc_Stmt_tok_struct _tmp726;_tmp726.tag=Cyc_Stmt_tok;_tmp726.f1=Cyc_Absyn_goto_stmt(({
struct _tagged_arr*_tmp727=_cycalloc(sizeof(*_tmp727));_tmp727[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp727;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp726;});
_tmp725;});break;case 274: _LL2CF: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp728=
_cycalloc(sizeof(*_tmp728));_tmp728[0]=({struct Cyc_Stmt_tok_struct _tmp729;
_tmp729.tag=Cyc_Stmt_tok;_tmp729.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp729;});
_tmp728;});break;case 275: _LL2D0: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp72A=
_cycalloc(sizeof(*_tmp72A));_tmp72A[0]=({struct Cyc_Stmt_tok_struct _tmp72B;
_tmp72B.tag=Cyc_Stmt_tok;_tmp72B.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp72B;});
_tmp72A;});break;case 276: _LL2D1: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp72C=
_cycalloc(sizeof(*_tmp72C));_tmp72C[0]=({struct Cyc_Stmt_tok_struct _tmp72D;
_tmp72D.tag=Cyc_Stmt_tok;_tmp72D.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp72D;});
_tmp72C;});break;case 277: _LL2D2: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp72E=
_cycalloc(sizeof(*_tmp72E));_tmp72E[0]=({struct Cyc_Stmt_tok_struct _tmp72F;
_tmp72F.tag=Cyc_Stmt_tok;_tmp72F.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp72F;});_tmp72E;});break;case 278: _LL2D3: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp730=_cycalloc(sizeof(*_tmp730));_tmp730[0]=({
struct Cyc_Stmt_tok_struct _tmp731;_tmp731.tag=Cyc_Stmt_tok;_tmp731.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp731;});_tmp730;});break;case 279: _LL2D4: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732[0]=({
struct Cyc_Stmt_tok_struct _tmp733;_tmp733.tag=Cyc_Stmt_tok;_tmp733.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp733;});_tmp732;});break;case 280: _LL2D5: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp734=_cycalloc(sizeof(*_tmp734));_tmp734[0]=({
struct Cyc_Stmt_tok_struct _tmp735;_tmp735.tag=Cyc_Stmt_tok;_tmp735.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp735;});_tmp734;});break;case 281: _LL2D6: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp736=_cycalloc(sizeof(*_tmp736));_tmp736[
0]=({struct Cyc_Pattern_tok_struct _tmp737;_tmp737.tag=Cyc_Pattern_tok;_tmp737.f1=
Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp737;});_tmp736;});break;case 282: _LL2D7: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 283: _LL2D8:
yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp738=_cycalloc(sizeof(*_tmp738));
_tmp738[0]=({struct Cyc_Pattern_tok_struct _tmp739;_tmp739.tag=Cyc_Pattern_tok;
_tmp739.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp73A=
_cycalloc(sizeof(*_tmp73A));_tmp73A[0]=({struct Cyc_Absyn_Int_p_struct _tmp73B;
_tmp73B.tag=1;_tmp73B.f1=(void*)(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f1;
_tmp73B.f2=(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2;_tmp73B;});_tmp73A;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp739;});
_tmp738;});break;case 284: _LL2D9: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp73C=_cycalloc(sizeof(*_tmp73C));_tmp73C[0]=({struct Cyc_Pattern_tok_struct
_tmp73D;_tmp73D.tag=Cyc_Pattern_tok;_tmp73D.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Int_p_struct*_tmp73E=_cycalloc(sizeof(*_tmp73E));_tmp73E[0]=({struct
Cyc_Absyn_Int_p_struct _tmp73F;_tmp73F.tag=1;_tmp73F.f1=(void*)((void*)0);_tmp73F.f2=
-(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2;_tmp73F;});_tmp73E;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp73D;});_tmp73C;});break;case 285: _LL2DA: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp740=_cycalloc(sizeof(*_tmp740));_tmp740[
0]=({struct Cyc_Pattern_tok_struct _tmp741;_tmp741.tag=Cyc_Pattern_tok;_tmp741.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*_tmp742=_cycalloc(
sizeof(*_tmp742));_tmp742[0]=({struct Cyc_Absyn_Float_p_struct _tmp743;_tmp743.tag=
3;_tmp743.f1=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp743;});_tmp742;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp741;});
_tmp740;});break;case 286: _LL2DB: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp744=_cycalloc(sizeof(*_tmp744));_tmp744[0]=({struct Cyc_Pattern_tok_struct
_tmp745;_tmp745.tag=Cyc_Pattern_tok;_tmp745.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Char_p_struct*_tmp746=_cycalloc_atomic(sizeof(*_tmp746));_tmp746[0]=({
struct Cyc_Absyn_Char_p_struct _tmp747;_tmp747.tag=2;_tmp747.f1=Cyc_yyget_Char_tok(
yyvs[yyvsp_offset]);_tmp747;});_tmp746;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp745;});_tmp744;});
break;case 287: _LL2DC: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp748=
_cycalloc(sizeof(*_tmp748));_tmp748[0]=({struct Cyc_Pattern_tok_struct _tmp749;
_tmp749.tag=Cyc_Pattern_tok;_tmp749.f1=Cyc_Absyn_new_pat((void*)1,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp749;});
_tmp748;});break;case 288: _LL2DD: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A[0]=({struct Cyc_Pattern_tok_struct
_tmp74B;_tmp74B.tag=Cyc_Pattern_tok;_tmp74B.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownId_p_struct*_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C[0]=({
struct Cyc_Absyn_UnknownId_p_struct _tmp74D;_tmp74D.tag=11;_tmp74D.f1=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp74D;});_tmp74C;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp74B;});_tmp74A;});
break;case 289: _LL2DE: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp74E=
_cycalloc(sizeof(*_tmp74E));_tmp74E[0]=({struct Cyc_Pattern_tok_struct _tmp74F;
_tmp74F.tag=Cyc_Pattern_tok;_tmp74F.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_struct*
_tmp750=_cycalloc(sizeof(*_tmp750));_tmp750[0]=({struct Cyc_Absyn_Tuple_p_struct
_tmp751;_tmp751.tag=4;_tmp751.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]));_tmp751;});_tmp750;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp74F;});_tmp74E;});break;case 290: _LL2DF: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp752=_cycalloc(sizeof(*_tmp752));_tmp752[
0]=({struct Cyc_Pattern_tok_struct _tmp753;_tmp753.tag=Cyc_Pattern_tok;_tmp753.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp754=
_cycalloc(sizeof(*_tmp754));_tmp754[0]=({struct Cyc_Absyn_UnknownCall_p_struct
_tmp755;_tmp755.tag=12;_tmp755.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp755.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp755;});_tmp754;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp753;});
_tmp752;});break;case 291: _LL2E0: {struct Cyc_List_List*_tmp756=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp757=_cycalloc(sizeof(*_tmp757));_tmp757[0]=({
struct Cyc_Pattern_tok_struct _tmp758;_tmp758.tag=Cyc_Pattern_tok;_tmp758.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp759=_cycalloc(sizeof(*_tmp759));
_tmp759[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp75A;_tmp75A.tag=7;_tmp75A.f1=({
struct Cyc_Absyn_AggrInfo _tmp75B;_tmp75B.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp75C=_cycalloc(sizeof(*_tmp75C));_tmp75C[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp75D;_tmp75D.tag=0;_tmp75D.f1=(void*)((void*)0);_tmp75D.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp75D;});_tmp75C;}));
_tmp75B.targs=0;_tmp75B;});_tmp75A.f2=_tmp756;_tmp75A.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp75A;});_tmp759;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp758;});
_tmp757;});break;}case 292: _LL2E1: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp75E=_cycalloc(sizeof(*_tmp75E));_tmp75E[0]=({struct Cyc_Pattern_tok_struct
_tmp75F;_tmp75F.tag=Cyc_Pattern_tok;_tmp75F.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Pointer_p_struct*_tmp760=_cycalloc(sizeof(*_tmp760));_tmp760[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp761;_tmp761.tag=5;_tmp761.f1=Cyc_yyget_Pattern_tok(
yyvs[yyvsp_offset]);_tmp761;});_tmp760;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp75F;});_tmp75E;});break;case 293: _LL2E2: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp762=_cycalloc(sizeof(*_tmp762));_tmp762[
0]=({struct Cyc_Pattern_tok_struct _tmp763;_tmp763.tag=Cyc_Pattern_tok;_tmp763.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp764=_cycalloc(
sizeof(*_tmp764));_tmp764[0]=({struct Cyc_Absyn_Reference_p_struct _tmp765;_tmp765.tag=
6;_tmp765.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp766=_cycalloc(sizeof(*
_tmp766));_tmp766->f1=(void*)0;_tmp766->f2=({struct _tagged_arr*_tmp767=_cycalloc(
sizeof(*_tmp767));_tmp767[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp767;});
_tmp766;}),(void*)0,0);_tmp765;});_tmp764;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp763;});_tmp762;});break;case 294: _LL2E3: yyval=(
void*)({struct Cyc_PatternList_tok_struct*_tmp768=_cycalloc(sizeof(*_tmp768));
_tmp768[0]=({struct Cyc_PatternList_tok_struct _tmp769;_tmp769.tag=Cyc_PatternList_tok;
_tmp769.f1=({struct Cyc_List_List*_tmp76A=_cycalloc(sizeof(*_tmp76A));_tmp76A->hd=
Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmp76A->tl=0;_tmp76A;});_tmp769;});
_tmp768;});break;case 295: _LL2E4: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp76B=_cycalloc(sizeof(*_tmp76B));_tmp76B[0]=({struct Cyc_PatternList_tok_struct
_tmp76C;_tmp76C.tag=Cyc_PatternList_tok;_tmp76C.f1=({struct Cyc_List_List*_tmp76D=
_cycalloc(sizeof(*_tmp76D));_tmp76D->hd=Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);
_tmp76D->tl=Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp76D;});_tmp76C;});_tmp76B;});break;case 296: _LL2E5: yyval=(
void*)({struct Cyc_FieldPattern_tok_struct*_tmp76E=_cycalloc(sizeof(*_tmp76E));
_tmp76E[0]=({struct Cyc_FieldPattern_tok_struct _tmp76F;_tmp76F.tag=Cyc_FieldPattern_tok;
_tmp76F.f1=({struct _tuple11*_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770->f1=0;
_tmp770->f2=Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmp770;});_tmp76F;});
_tmp76E;});break;case 297: _LL2E6: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp771=_cycalloc(sizeof(*_tmp771));_tmp771[0]=({struct Cyc_FieldPattern_tok_struct
_tmp772;_tmp772.tag=Cyc_FieldPattern_tok;_tmp772.f1=({struct _tuple11*_tmp773=
_cycalloc(sizeof(*_tmp773));_tmp773->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp773->f2=Cyc_yyget_Pattern_tok(
yyvs[yyvsp_offset]);_tmp773;});_tmp772;});_tmp771;});break;case 298: _LL2E7: yyval=(
void*)({struct Cyc_FieldPatternList_tok_struct*_tmp774=_cycalloc(sizeof(*_tmp774));
_tmp774[0]=({struct Cyc_FieldPatternList_tok_struct _tmp775;_tmp775.tag=Cyc_FieldPatternList_tok;
_tmp775.f1=({struct Cyc_List_List*_tmp776=_cycalloc(sizeof(*_tmp776));_tmp776->hd=
Cyc_yyget_FieldPattern_tok(yyvs[yyvsp_offset]);_tmp776->tl=0;_tmp776;});_tmp775;});
_tmp774;});break;case 299: _LL2E8: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp777=_cycalloc(sizeof(*_tmp777));_tmp777[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp778;_tmp778.tag=Cyc_FieldPatternList_tok;_tmp778.f1=({struct Cyc_List_List*
_tmp779=_cycalloc(sizeof(*_tmp779));_tmp779->hd=Cyc_yyget_FieldPattern_tok(yyvs[
yyvsp_offset]);_tmp779->tl=Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp779;});_tmp778;});
_tmp777;});break;case 300: _LL2E9: yyval=yyvs[yyvsp_offset];break;case 301: _LL2EA:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp77A=_cycalloc(sizeof(*_tmp77A));
_tmp77A[0]=({struct Cyc_Exp_tok_struct _tmp77B;_tmp77B.tag=Cyc_Exp_tok;_tmp77B.f1=
Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp77B;});_tmp77A;});break;case 302: _LL2EB: yyval=yyvs[
yyvsp_offset];break;case 303: _LL2EC: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp77C=_cycalloc(sizeof(*_tmp77C));_tmp77C[0]=({struct Cyc_Exp_tok_struct _tmp77D;
_tmp77D.tag=Cyc_Exp_tok;_tmp77D.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp77D;});_tmp77C;});break;case 304: _LL2ED: yyval=(
void*)({struct Cyc_Primopopt_tok_struct*_tmp77E=_cycalloc(sizeof(*_tmp77E));
_tmp77E[0]=({struct Cyc_Primopopt_tok_struct _tmp77F;_tmp77F.tag=Cyc_Primopopt_tok;
_tmp77F.f1=0;_tmp77F;});_tmp77E;});break;case 305: _LL2EE: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp780=_cycalloc(sizeof(*_tmp780));_tmp780[0]=({struct Cyc_Primopopt_tok_struct
_tmp781;_tmp781.tag=Cyc_Primopopt_tok;_tmp781.f1=({struct Cyc_Core_Opt*_tmp782=
_cycalloc(sizeof(*_tmp782));_tmp782->v=(void*)((void*)1);_tmp782;});_tmp781;});
_tmp780;});break;case 306: _LL2EF: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp783=_cycalloc(sizeof(*_tmp783));_tmp783[0]=({struct Cyc_Primopopt_tok_struct
_tmp784;_tmp784.tag=Cyc_Primopopt_tok;_tmp784.f1=({struct Cyc_Core_Opt*_tmp785=
_cycalloc(sizeof(*_tmp785));_tmp785->v=(void*)((void*)3);_tmp785;});_tmp784;});
_tmp783;});break;case 307: _LL2F0: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp786=_cycalloc(sizeof(*_tmp786));_tmp786[0]=({struct Cyc_Primopopt_tok_struct
_tmp787;_tmp787.tag=Cyc_Primopopt_tok;_tmp787.f1=({struct Cyc_Core_Opt*_tmp788=
_cycalloc(sizeof(*_tmp788));_tmp788->v=(void*)((void*)4);_tmp788;});_tmp787;});
_tmp786;});break;case 308: _LL2F1: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp789=_cycalloc(sizeof(*_tmp789));_tmp789[0]=({struct Cyc_Primopopt_tok_struct
_tmp78A;_tmp78A.tag=Cyc_Primopopt_tok;_tmp78A.f1=({struct Cyc_Core_Opt*_tmp78B=
_cycalloc(sizeof(*_tmp78B));_tmp78B->v=(void*)((void*)0);_tmp78B;});_tmp78A;});
_tmp789;});break;case 309: _LL2F2: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp78C=_cycalloc(sizeof(*_tmp78C));_tmp78C[0]=({struct Cyc_Primopopt_tok_struct
_tmp78D;_tmp78D.tag=Cyc_Primopopt_tok;_tmp78D.f1=({struct Cyc_Core_Opt*_tmp78E=
_cycalloc(sizeof(*_tmp78E));_tmp78E->v=(void*)((void*)2);_tmp78E;});_tmp78D;});
_tmp78C;});break;case 310: _LL2F3: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp78F=_cycalloc(sizeof(*_tmp78F));_tmp78F[0]=({struct Cyc_Primopopt_tok_struct
_tmp790;_tmp790.tag=Cyc_Primopopt_tok;_tmp790.f1=({struct Cyc_Core_Opt*_tmp791=
_cycalloc(sizeof(*_tmp791));_tmp791->v=(void*)((void*)16);_tmp791;});_tmp790;});
_tmp78F;});break;case 311: _LL2F4: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp792=_cycalloc(sizeof(*_tmp792));_tmp792[0]=({struct Cyc_Primopopt_tok_struct
_tmp793;_tmp793.tag=Cyc_Primopopt_tok;_tmp793.f1=({struct Cyc_Core_Opt*_tmp794=
_cycalloc(sizeof(*_tmp794));_tmp794->v=(void*)((void*)17);_tmp794;});_tmp793;});
_tmp792;});break;case 312: _LL2F5: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp795=_cycalloc(sizeof(*_tmp795));_tmp795[0]=({struct Cyc_Primopopt_tok_struct
_tmp796;_tmp796.tag=Cyc_Primopopt_tok;_tmp796.f1=({struct Cyc_Core_Opt*_tmp797=
_cycalloc(sizeof(*_tmp797));_tmp797->v=(void*)((void*)13);_tmp797;});_tmp796;});
_tmp795;});break;case 313: _LL2F6: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp798=_cycalloc(sizeof(*_tmp798));_tmp798[0]=({struct Cyc_Primopopt_tok_struct
_tmp799;_tmp799.tag=Cyc_Primopopt_tok;_tmp799.f1=({struct Cyc_Core_Opt*_tmp79A=
_cycalloc(sizeof(*_tmp79A));_tmp79A->v=(void*)((void*)15);_tmp79A;});_tmp799;});
_tmp798;});break;case 314: _LL2F7: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp79B=_cycalloc(sizeof(*_tmp79B));_tmp79B[0]=({struct Cyc_Primopopt_tok_struct
_tmp79C;_tmp79C.tag=Cyc_Primopopt_tok;_tmp79C.f1=({struct Cyc_Core_Opt*_tmp79D=
_cycalloc(sizeof(*_tmp79D));_tmp79D->v=(void*)((void*)14);_tmp79D;});_tmp79C;});
_tmp79B;});break;case 315: _LL2F8: yyval=yyvs[yyvsp_offset];break;case 316: _LL2F9:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp79E=_cycalloc(sizeof(*_tmp79E));
_tmp79E[0]=({struct Cyc_Exp_tok_struct _tmp79F;_tmp79F.tag=Cyc_Exp_tok;_tmp79F.f1=
Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp79F;});_tmp79E;});break;case 317: _LL2FA: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7A0=_cycalloc(sizeof(*_tmp7A0));_tmp7A0[0]=({
struct Cyc_Exp_tok_struct _tmp7A1;_tmp7A1.tag=Cyc_Exp_tok;_tmp7A1.f1=Cyc_Absyn_throw_exp(
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7A1;});_tmp7A0;});break;case 318: _LL2FB: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7A2=_cycalloc(sizeof(*_tmp7A2));_tmp7A2[0]=({
struct Cyc_Exp_tok_struct _tmp7A3;_tmp7A3.tag=Cyc_Exp_tok;_tmp7A3.f1=Cyc_Absyn_New_exp(
0,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line));_tmp7A3;});
_tmp7A2;});break;case 319: _LL2FC: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7A4=
_cycalloc(sizeof(*_tmp7A4));_tmp7A4[0]=({struct Cyc_Exp_tok_struct _tmp7A5;_tmp7A5.tag=
Cyc_Exp_tok;_tmp7A5.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).last_line));_tmp7A5;});_tmp7A4;});break;case 320: _LL2FD: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7A6=_cycalloc(sizeof(*_tmp7A6));_tmp7A6[0]=({
struct Cyc_Exp_tok_struct _tmp7A7;_tmp7A7.tag=Cyc_Exp_tok;_tmp7A7.f1=Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7A7;});_tmp7A6;});break;case 321: _LL2FE: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7A8=_cycalloc(sizeof(*_tmp7A8));_tmp7A8[0]=({
struct Cyc_Exp_tok_struct _tmp7A9;_tmp7A9.tag=Cyc_Exp_tok;_tmp7A9.f1=Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7A9;});_tmp7A8;});break;case 322: _LL2FF: yyval=yyvs[
yyvsp_offset];break;case 323: _LL300: yyval=yyvs[yyvsp_offset];break;case 324: _LL301:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7AA=_cycalloc(sizeof(*_tmp7AA));
_tmp7AA[0]=({struct Cyc_Exp_tok_struct _tmp7AB;_tmp7AB.tag=Cyc_Exp_tok;_tmp7AB.f1=
Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7AB;});_tmp7AA;});break;case 325: _LL302: yyval=yyvs[
yyvsp_offset];break;case 326: _LL303: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7AC=_cycalloc(sizeof(*_tmp7AC));_tmp7AC[0]=({struct Cyc_Exp_tok_struct _tmp7AD;
_tmp7AD.tag=Cyc_Exp_tok;_tmp7AD.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7AD;});
_tmp7AC;});break;case 327: _LL304: yyval=yyvs[yyvsp_offset];break;case 328: _LL305:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7AE=_cycalloc(sizeof(*_tmp7AE));
_tmp7AE[0]=({struct Cyc_Exp_tok_struct _tmp7AF;_tmp7AF.tag=Cyc_Exp_tok;_tmp7AF.f1=
Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7AF;});
_tmp7AE;});break;case 329: _LL306: yyval=yyvs[yyvsp_offset];break;case 330: _LL307:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B0=_cycalloc(sizeof(*_tmp7B0));
_tmp7B0[0]=({struct Cyc_Exp_tok_struct _tmp7B1;_tmp7B1.tag=Cyc_Exp_tok;_tmp7B1.f1=
Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B1;});
_tmp7B0;});break;case 331: _LL308: yyval=yyvs[yyvsp_offset];break;case 332: _LL309:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B2=_cycalloc(sizeof(*_tmp7B2));
_tmp7B2[0]=({struct Cyc_Exp_tok_struct _tmp7B3;_tmp7B3.tag=Cyc_Exp_tok;_tmp7B3.f1=
Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B3;});
_tmp7B2;});break;case 333: _LL30A: yyval=yyvs[yyvsp_offset];break;case 334: _LL30B:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B4=_cycalloc(sizeof(*_tmp7B4));
_tmp7B4[0]=({struct Cyc_Exp_tok_struct _tmp7B5;_tmp7B5.tag=Cyc_Exp_tok;_tmp7B5.f1=
Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7B5;});_tmp7B4;});break;case 335: _LL30C: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7B6=_cycalloc(sizeof(*_tmp7B6));_tmp7B6[0]=({
struct Cyc_Exp_tok_struct _tmp7B7;_tmp7B7.tag=Cyc_Exp_tok;_tmp7B7.f1=Cyc_Absyn_neq_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7B7;});_tmp7B6;});break;case 336: _LL30D: yyval=yyvs[
yyvsp_offset];break;case 337: _LL30E: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7B8=_cycalloc(sizeof(*_tmp7B8));_tmp7B8[0]=({struct Cyc_Exp_tok_struct _tmp7B9;
_tmp7B9.tag=Cyc_Exp_tok;_tmp7B9.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B9;});
_tmp7B8;});break;case 338: _LL30F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7BA=
_cycalloc(sizeof(*_tmp7BA));_tmp7BA[0]=({struct Cyc_Exp_tok_struct _tmp7BB;_tmp7BB.tag=
Cyc_Exp_tok;_tmp7BB.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7BB;});
_tmp7BA;});break;case 339: _LL310: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7BC=
_cycalloc(sizeof(*_tmp7BC));_tmp7BC[0]=({struct Cyc_Exp_tok_struct _tmp7BD;_tmp7BD.tag=
Cyc_Exp_tok;_tmp7BD.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7BD;});
_tmp7BC;});break;case 340: _LL311: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7BE=
_cycalloc(sizeof(*_tmp7BE));_tmp7BE[0]=({struct Cyc_Exp_tok_struct _tmp7BF;_tmp7BF.tag=
Cyc_Exp_tok;_tmp7BF.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7BF;});
_tmp7BE;});break;case 341: _LL312: yyval=yyvs[yyvsp_offset];break;case 342: _LL313:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7C0=_cycalloc(sizeof(*_tmp7C0));
_tmp7C0[0]=({struct Cyc_Exp_tok_struct _tmp7C1;_tmp7C1.tag=Cyc_Exp_tok;_tmp7C1.f1=
Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7C1;});
_tmp7C0;});break;case 343: _LL314: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7C2=
_cycalloc(sizeof(*_tmp7C2));_tmp7C2[0]=({struct Cyc_Exp_tok_struct _tmp7C3;_tmp7C3.tag=
Cyc_Exp_tok;_tmp7C3.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7C3;});
_tmp7C2;});break;case 344: _LL315: yyval=yyvs[yyvsp_offset];break;case 345: _LL316:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7C4=_cycalloc(sizeof(*_tmp7C4));
_tmp7C4[0]=({struct Cyc_Exp_tok_struct _tmp7C5;_tmp7C5.tag=Cyc_Exp_tok;_tmp7C5.f1=
Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7C5;});
_tmp7C4;});break;case 346: _LL317: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7C6=
_cycalloc(sizeof(*_tmp7C6));_tmp7C6[0]=({struct Cyc_Exp_tok_struct _tmp7C7;_tmp7C7.tag=
Cyc_Exp_tok;_tmp7C7.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7C7;});
_tmp7C6;});break;case 347: _LL318: yyval=yyvs[yyvsp_offset];break;case 348: _LL319:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7C8=_cycalloc(sizeof(*_tmp7C8));
_tmp7C8[0]=({struct Cyc_Exp_tok_struct _tmp7C9;_tmp7C9.tag=Cyc_Exp_tok;_tmp7C9.f1=
Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7C9;});
_tmp7C8;});break;case 349: _LL31A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7CA=
_cycalloc(sizeof(*_tmp7CA));_tmp7CA[0]=({struct Cyc_Exp_tok_struct _tmp7CB;_tmp7CB.tag=
Cyc_Exp_tok;_tmp7CB.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7CB;});
_tmp7CA;});break;case 350: _LL31B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7CC=
_cycalloc(sizeof(*_tmp7CC));_tmp7CC[0]=({struct Cyc_Exp_tok_struct _tmp7CD;_tmp7CD.tag=
Cyc_Exp_tok;_tmp7CD.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7CD;});
_tmp7CC;});break;case 351: _LL31C: yyval=yyvs[yyvsp_offset];break;case 352: _LL31D:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7CE=_cycalloc(sizeof(*_tmp7CE));
_tmp7CE[0]=({struct Cyc_Exp_tok_struct _tmp7CF;_tmp7CF.tag=Cyc_Exp_tok;_tmp7CF.f1=
Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7CF;});_tmp7CE;});break;case 353: _LL31E: yyval=yyvs[
yyvsp_offset];break;case 354: _LL31F: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7D0=_cycalloc(sizeof(*_tmp7D0));_tmp7D0[0]=({struct Cyc_Exp_tok_struct _tmp7D1;
_tmp7D1.tag=Cyc_Exp_tok;_tmp7D1.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7D1;});
_tmp7D0;});break;case 355: _LL320: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7D2=
_cycalloc(sizeof(*_tmp7D2));_tmp7D2[0]=({struct Cyc_Exp_tok_struct _tmp7D3;_tmp7D3.tag=
Cyc_Exp_tok;_tmp7D3.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7D3;});
_tmp7D2;});break;case 356: _LL321: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7D4=
_cycalloc(sizeof(*_tmp7D4));_tmp7D4[0]=({struct Cyc_Exp_tok_struct _tmp7D5;_tmp7D5.tag=
Cyc_Exp_tok;_tmp7D5.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7D5;});
_tmp7D4;});break;case 357: _LL322: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7D6=
_cycalloc(sizeof(*_tmp7D6));_tmp7D6[0]=({struct Cyc_Exp_tok_struct _tmp7D7;_tmp7D7.tag=
Cyc_Exp_tok;_tmp7D7.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7D7;});
_tmp7D6;});break;case 358: _LL323: yyval=yyvs[yyvsp_offset];break;case 359: _LL324:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7D8=_cycalloc(sizeof(*_tmp7D8));
_tmp7D8[0]=({struct Cyc_Exp_tok_struct _tmp7D9;_tmp7D9.tag=Cyc_Exp_tok;_tmp7D9.f1=
Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7D9;});_tmp7D8;});break;case 360: _LL325: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7DA=_cycalloc(sizeof(*_tmp7DA));_tmp7DA[0]=({
struct Cyc_Exp_tok_struct _tmp7DB;_tmp7DB.tag=Cyc_Exp_tok;_tmp7DB.f1=Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)])).f3,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7DB;});
_tmp7DA;});break;case 361: _LL326: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7DC=
_cycalloc(sizeof(*_tmp7DC));_tmp7DC[0]=({struct Cyc_Exp_tok_struct _tmp7DD;_tmp7DD.tag=
Cyc_Exp_tok;_tmp7DD.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7DD;});
_tmp7DC;});break;case 362: _LL327: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7DE=
_cycalloc(sizeof(*_tmp7DE));_tmp7DE[0]=({struct Cyc_Exp_tok_struct _tmp7DF;_tmp7DF.tag=
Cyc_Exp_tok;_tmp7DF.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp7E0=_cycalloc(sizeof(*_tmp7E0));_tmp7E0[0]=({struct Cyc_Absyn_StructField_struct
_tmp7E1;_tmp7E1.tag=0;_tmp7E1.f1=({struct _tagged_arr*_tmp7E2=_cycalloc(sizeof(*
_tmp7E2));_tmp7E2[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp7E2;});_tmp7E1;});_tmp7E0;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7DF;});_tmp7DE;});break;case 363: _LL328: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7E3=_cycalloc(sizeof(*_tmp7E3));_tmp7E3[0]=({
struct Cyc_Exp_tok_struct _tmp7E4;_tmp7E4.tag=Cyc_Exp_tok;_tmp7E4.f1=Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp7E5=_cycalloc_atomic(
sizeof(*_tmp7E5));_tmp7E5[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp7E6;_tmp7E6.tag=
1;_tmp7E6.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp7E6;});_tmp7E5;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7E4;});
_tmp7E3;});break;case 364: _LL329: {struct Cyc_Position_Segment*_tmp7E7=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);struct Cyc_List_List*_tmp7E8=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp7E7,Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_Exp_tok_struct*_tmp7E9=_cycalloc(sizeof(*_tmp7E9));_tmp7E9[0]=({struct
Cyc_Exp_tok_struct _tmp7EA;_tmp7EA.tag=Cyc_Exp_tok;_tmp7EA.f1=Cyc_Absyn_gentyp_exp(
_tmp7E8,(*Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7EA;});_tmp7E9;});break;}case 365: _LL32A: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7EB=_cycalloc(sizeof(*_tmp7EB));_tmp7EB[0]=({
struct Cyc_Exp_tok_struct _tmp7EC;_tmp7EC.tag=Cyc_Exp_tok;_tmp7EC.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7ED=_cycalloc(sizeof(*_tmp7ED));
_tmp7ED[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7EE;_tmp7EE.tag=33;_tmp7EE.f1=({
struct Cyc_Absyn_MallocInfo _tmp7EF;_tmp7EF.is_calloc=0;_tmp7EF.rgn=0;_tmp7EF.elt_type=
0;_tmp7EF.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp7EF.fat_result=0;_tmp7EF;});_tmp7EE;});_tmp7ED;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7EC;});_tmp7EB;});break;case 366: _LL32B: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7F0=_cycalloc(sizeof(*_tmp7F0));_tmp7F0[0]=({
struct Cyc_Exp_tok_struct _tmp7F1;_tmp7F1.tag=Cyc_Exp_tok;_tmp7F1.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7F2=_cycalloc(sizeof(*_tmp7F2));
_tmp7F2[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7F3;_tmp7F3.tag=33;_tmp7F3.f1=({
struct Cyc_Absyn_MallocInfo _tmp7F4;_tmp7F4.is_calloc=0;_tmp7F4.rgn=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp7F4.elt_type=0;_tmp7F4.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7F4.fat_result=0;
_tmp7F4;});_tmp7F3;});_tmp7F2;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F1;});_tmp7F0;});break;case 367: _LL32C: {void*
_tmp7F6;struct _tuple2 _tmp7F5=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7F6=_tmp7F5.f3;yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7F7=_cycalloc(sizeof(*_tmp7F7));_tmp7F7[0]=({
struct Cyc_Exp_tok_struct _tmp7F8;_tmp7F8.tag=Cyc_Exp_tok;_tmp7F8.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7F9=_cycalloc(sizeof(*_tmp7F9));
_tmp7F9[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7FA;_tmp7FA.tag=33;_tmp7FA.f1=({
struct Cyc_Absyn_MallocInfo _tmp7FB;_tmp7FB.is_calloc=1;_tmp7FB.rgn=0;_tmp7FB.elt_type=({
void**_tmp7FC=_cycalloc(sizeof(*_tmp7FC));_tmp7FC[0]=_tmp7F6;_tmp7FC;});_tmp7FB.num_elts=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp7FB.fat_result=0;_tmp7FB;});_tmp7FA;});_tmp7F9;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F8;});_tmp7F7;});break;}case 368: _LL32D: {void*
_tmp7FE;struct _tuple2 _tmp7FD=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7FE=_tmp7FD.f3;yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7FF=_cycalloc(sizeof(*_tmp7FF));_tmp7FF[0]=({
struct Cyc_Exp_tok_struct _tmp800;_tmp800.tag=Cyc_Exp_tok;_tmp800.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp801=_cycalloc(sizeof(*_tmp801));
_tmp801[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp802;_tmp802.tag=33;_tmp802.f1=({
struct Cyc_Absyn_MallocInfo _tmp803;_tmp803.is_calloc=1;_tmp803.rgn=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);
_tmp803.elt_type=({void**_tmp804=_cycalloc(sizeof(*_tmp804));_tmp804[0]=_tmp7FE;
_tmp804;});_tmp803.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp803.fat_result=0;
_tmp803;});_tmp802;});_tmp801;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp800;});_tmp7FF;});break;}case 369: _LL32E: yyval=(
void*)({struct Cyc_Primop_tok_struct*_tmp805=_cycalloc(sizeof(*_tmp805));_tmp805[
0]=({struct Cyc_Primop_tok_struct _tmp806;_tmp806.tag=Cyc_Primop_tok;_tmp806.f1=(
void*)((void*)12);_tmp806;});_tmp805;});break;case 370: _LL32F: yyval=(void*)({
struct Cyc_Primop_tok_struct*_tmp807=_cycalloc(sizeof(*_tmp807));_tmp807[0]=({
struct Cyc_Primop_tok_struct _tmp808;_tmp808.tag=Cyc_Primop_tok;_tmp808.f1=(void*)((
void*)11);_tmp808;});_tmp807;});break;case 371: _LL330: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809[0]=({struct Cyc_Primop_tok_struct
_tmp80A;_tmp80A.tag=Cyc_Primop_tok;_tmp80A.f1=(void*)((void*)2);_tmp80A;});
_tmp809;});break;case 372: _LL331: yyval=yyvs[yyvsp_offset];break;case 373: _LL332:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp80B=_cycalloc(sizeof(*_tmp80B));
_tmp80B[0]=({struct Cyc_Exp_tok_struct _tmp80C;_tmp80C.tag=Cyc_Exp_tok;_tmp80C.f1=
Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp80C;});_tmp80B;});break;case 374: _LL333: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp80D=_cycalloc(sizeof(*_tmp80D));_tmp80D[0]=({
struct Cyc_Exp_tok_struct _tmp80E;_tmp80E.tag=Cyc_Exp_tok;_tmp80E.f1=Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp80E;});
_tmp80D;});break;case 375: _LL334: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp80F=
_cycalloc(sizeof(*_tmp80F));_tmp80F[0]=({struct Cyc_Exp_tok_struct _tmp810;_tmp810.tag=
Cyc_Exp_tok;_tmp810.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp810;});_tmp80F;});break;case 376: _LL335: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp811=_cycalloc(sizeof(*_tmp811));_tmp811[0]=({
struct Cyc_Exp_tok_struct _tmp812;_tmp812.tag=Cyc_Exp_tok;_tmp812.f1=Cyc_Absyn_aggrmember_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
struct _tagged_arr*_tmp813=_cycalloc(sizeof(*_tmp813));_tmp813[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp813;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp812;});_tmp811;});break;case 377: _LL336: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp814=_cycalloc(sizeof(*_tmp814));_tmp814[0]=({
struct Cyc_Exp_tok_struct _tmp815;_tmp815.tag=Cyc_Exp_tok;_tmp815.f1=Cyc_Absyn_aggrarrow_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
struct _tagged_arr*_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp816;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp815;});_tmp814;});break;case 378: _LL337: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp817=_cycalloc(sizeof(*_tmp817));_tmp817[0]=({
struct Cyc_Exp_tok_struct _tmp818;_tmp818.tag=Cyc_Exp_tok;_tmp818.f1=Cyc_Absyn_post_inc_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp818;});
_tmp817;});break;case 379: _LL338: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp819=
_cycalloc(sizeof(*_tmp819));_tmp819[0]=({struct Cyc_Exp_tok_struct _tmp81A;_tmp81A.tag=
Cyc_Exp_tok;_tmp81A.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp81A;});_tmp819;});break;case 380: _LL339: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp81B=_cycalloc(sizeof(*_tmp81B));_tmp81B[0]=({
struct Cyc_Exp_tok_struct _tmp81C;_tmp81C.tag=Cyc_Exp_tok;_tmp81C.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp81D=_cycalloc(sizeof(*_tmp81D));
_tmp81D[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp81E;_tmp81E.tag=25;_tmp81E.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
4)]);_tmp81E.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp81E;});_tmp81D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp81C;});_tmp81B;});break;case 381: _LL33A: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp81F=_cycalloc(sizeof(*_tmp81F));_tmp81F[0]=({
struct Cyc_Exp_tok_struct _tmp820;_tmp820.tag=Cyc_Exp_tok;_tmp820.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp821=_cycalloc(sizeof(*_tmp821));
_tmp821[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp822;_tmp822.tag=25;_tmp822.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmp822.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp822;});_tmp821;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp820;});_tmp81F;});break;case 382: _LL33B: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp823=_cycalloc(sizeof(*_tmp823));_tmp823[0]=({
struct Cyc_Exp_tok_struct _tmp824;_tmp824.tag=Cyc_Exp_tok;_tmp824.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Fill_e_struct*_tmp825=_cycalloc(sizeof(*_tmp825));
_tmp825[0]=({struct Cyc_Absyn_Fill_e_struct _tmp826;_tmp826.tag=37;_tmp826.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp826;});_tmp825;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp824;});
_tmp823;});break;case 383: _LL33C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp827=
_cycalloc(sizeof(*_tmp827));_tmp827[0]=({struct Cyc_Exp_tok_struct _tmp828;_tmp828.tag=
Cyc_Exp_tok;_tmp828.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Codegen_e_struct*
_tmp829=_cycalloc(sizeof(*_tmp829));_tmp829[0]=({struct Cyc_Absyn_Codegen_e_struct
_tmp82A;_tmp82A.tag=36;_tmp82A.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp82A;});_tmp829;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp828;});
_tmp827;});break;case 384: _LL33D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp82B=
_cycalloc(sizeof(*_tmp82B));_tmp82B[0]=({struct Cyc_Exp_tok_struct _tmp82C;_tmp82C.tag=
Cyc_Exp_tok;_tmp82C.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp82D=_cycalloc(sizeof(*_tmp82D));_tmp82D[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp82E;_tmp82E.tag=2;_tmp82E.f1=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);
_tmp82E;});_tmp82D;}),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp82C;});_tmp82B;});break;case 385: _LL33E: yyval=
yyvs[yyvsp_offset];break;case 386: _LL33F: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp82F=_cycalloc(sizeof(*_tmp82F));_tmp82F[0]=({struct Cyc_Exp_tok_struct _tmp830;
_tmp830.tag=Cyc_Exp_tok;_tmp830.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp830;});_tmp82F;});break;case 387: _LL340: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 388: _LL341:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp831=_cycalloc(sizeof(*_tmp831));
_tmp831[0]=({struct Cyc_Exp_tok_struct _tmp832;_tmp832.tag=Cyc_Exp_tok;_tmp832.f1=
Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp832;});_tmp831;});break;case 389: _LL342: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp833=_cycalloc(sizeof(*_tmp833));_tmp833[0]=({
struct Cyc_Exp_tok_struct _tmp834;_tmp834.tag=Cyc_Exp_tok;_tmp834.f1=Cyc_Absyn_instantiate_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp834;});_tmp833;});break;case 390: _LL343: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp835=_cycalloc(sizeof(*_tmp835));_tmp835[0]=({
struct Cyc_Exp_tok_struct _tmp836;_tmp836.tag=Cyc_Exp_tok;_tmp836.f1=Cyc_Absyn_tuple_exp(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp836;});
_tmp835;});break;case 391: _LL344: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp837=
_cycalloc(sizeof(*_tmp837));_tmp837[0]=({struct Cyc_Exp_tok_struct _tmp838;_tmp838.tag=
Cyc_Exp_tok;_tmp838.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp839=_cycalloc(sizeof(*_tmp839));_tmp839[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp83A;_tmp83A.tag=28;_tmp83A.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp83A.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp83A.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp83A.f4=0;
_tmp83A;});_tmp839;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp838;});_tmp837;});break;case 392: _LL345: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp83B=_cycalloc(sizeof(*_tmp83B));_tmp83B[0]=({
struct Cyc_Exp_tok_struct _tmp83C;_tmp83C.tag=Cyc_Exp_tok;_tmp83C.f1=Cyc_Absyn_stmt_exp(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp83C;});
_tmp83B;});break;case 393: _LL346: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp83D=_cycalloc(sizeof(*_tmp83D));_tmp83D[0]=({struct Cyc_ExpList_tok_struct
_tmp83E;_tmp83E.tag=Cyc_ExpList_tok;_tmp83E.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(yyvs[yyvsp_offset]));
_tmp83E;});_tmp83D;});break;case 394: _LL347: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp83F=_cycalloc(sizeof(*_tmp83F));_tmp83F[0]=({struct Cyc_ExpList_tok_struct
_tmp840;_tmp840.tag=Cyc_ExpList_tok;_tmp840.f1=({struct Cyc_List_List*_tmp841=
_cycalloc(sizeof(*_tmp841));_tmp841->hd=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);
_tmp841->tl=0;_tmp841;});_tmp840;});_tmp83F;});break;case 395: _LL348: yyval=(void*)({
struct Cyc_ExpList_tok_struct*_tmp842=_cycalloc(sizeof(*_tmp842));_tmp842[0]=({
struct Cyc_ExpList_tok_struct _tmp843;_tmp843.tag=Cyc_ExpList_tok;_tmp843.f1=({
struct Cyc_List_List*_tmp844=_cycalloc(sizeof(*_tmp844));_tmp844->hd=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp844->tl=Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp844;});_tmp843;});
_tmp842;});break;case 396: _LL349: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp845=
_cycalloc(sizeof(*_tmp845));_tmp845[0]=({struct Cyc_Exp_tok_struct _tmp846;_tmp846.tag=
Cyc_Exp_tok;_tmp846.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f1,(*
Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp846;});_tmp845;});
break;case 397: _LL34A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp847=_cycalloc(
sizeof(*_tmp847));_tmp847[0]=({struct Cyc_Exp_tok_struct _tmp848;_tmp848.tag=Cyc_Exp_tok;
_tmp848.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp848;});
_tmp847;});break;case 398: _LL34B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp849=
_cycalloc(sizeof(*_tmp849));_tmp849[0]=({struct Cyc_Exp_tok_struct _tmp84A;_tmp84A.tag=
Cyc_Exp_tok;_tmp84A.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));_tmp84A;});_tmp849;});break;case 399: _LL34C: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp84B=_cycalloc(sizeof(*_tmp84B));_tmp84B[0]=({
struct Cyc_Exp_tok_struct _tmp84C;_tmp84C.tag=Cyc_Exp_tok;_tmp84C.f1=Cyc_Absyn_null_exp(
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmp84C;});_tmp84B;});break;case 400: _LL34D: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp84D=_cycalloc(sizeof(*_tmp84D));_tmp84D[0]=({struct Cyc_QualId_tok_struct
_tmp84E;_tmp84E.tag=Cyc_QualId_tok;_tmp84E.f1=({struct _tuple1*_tmp84F=_cycalloc(
sizeof(*_tmp84F));_tmp84F->f1=Cyc_Absyn_rel_ns_null;_tmp84F->f2=({struct
_tagged_arr*_tmp850=_cycalloc(sizeof(*_tmp850));_tmp850[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp850;});_tmp84F;});_tmp84E;});_tmp84D;});break;case 401:
_LL34E: yyval=yyvs[yyvsp_offset];break;case 402: _LL34F: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp851=_cycalloc(sizeof(*_tmp851));_tmp851[0]=({struct Cyc_QualId_tok_struct
_tmp852;_tmp852.tag=Cyc_QualId_tok;_tmp852.f1=({struct _tuple1*_tmp853=_cycalloc(
sizeof(*_tmp853));_tmp853->f1=Cyc_Absyn_rel_ns_null;_tmp853->f2=({struct
_tagged_arr*_tmp854=_cycalloc(sizeof(*_tmp854));_tmp854[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp854;});_tmp853;});_tmp852;});_tmp851;});break;case 403:
_LL350: yyval=yyvs[yyvsp_offset];break;case 404: _LL351: yyval=yyvs[yyvsp_offset];
break;case 405: _LL352: yyval=yyvs[yyvsp_offset];break;case 406: _LL353: yyval=yyvs[
yyvsp_offset];break;case 407: _LL354: yyval=yyvs[yyvsp_offset];break;case 408: _LL355:
break;case 409: _LL356:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;break;default: _LL357: break;}
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
_check_known_subscript_notnull(114,yyn - 127)]+ yyss[yyssp_offset];if((yystate >= 
0?yystate <= 4770: 0)?Cyc_yycheck[yystate]== yyss[yyssp_offset]: 0)yystate=(int)Cyc_yytable[
yystate];else{yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(114,yyn - 
127)];}goto yynewstate;yyerrlab: if(yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
yystate];if(yyn > - 32768?yyn < 4770: 0){int sze=0;struct _tagged_arr msg;int x;int count;
count=0;for(x=yyn < 0?- yyn: 0;x < 241 / sizeof(char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(4771,x + yyn)]== x)(sze +=Cyc_Std_strlen(Cyc_yytname[
x])+ 15,count ++);}msg=({unsigned int _tmp855=(unsigned int)(sze + 15);char*_tmp856=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp855));struct _tagged_arr
_tmp858=_tag_arr(_tmp856,sizeof(char),(unsigned int)(sze + 15));{unsigned int
_tmp857=_tmp855;unsigned int i;for(i=0;i < _tmp857;i ++){_tmp856[i]='\000';}}
_tmp858;});Cyc_Std_strcpy(msg,_tag_arr("parse error",sizeof(char),12));if(count < 
5){count=0;for(x=yyn < 0?- yyn: 0;x < 241 / sizeof(char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(4771,x + yyn)]== x){Cyc_Std_strcat(msg,count == 0?
_tag_arr(", expecting `",sizeof(char),14): _tag_arr(" or `",sizeof(char),6));Cyc_Std_strcat(
msg,Cyc_yytname[x]);Cyc_Std_strcat(msg,_tag_arr("'",sizeof(char),2));count ++;}}}
Cyc_yyerror((struct _tagged_arr)msg);}else{Cyc_yyerror(_tag_arr("parse error",
sizeof(char),12));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 
0)return 1;Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0)return 1;yyvsp_offset --;yystate=(int)yyss[
_check_known_subscript_notnull(10000,-- yyssp_offset)];yylsp_offset --;yyerrhandle:
yyn=(int)Cyc_yypact[yystate];if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0?
1: yyn > 4770)?1: Cyc_yycheck[yyn]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){if(yyn == - 32768)goto yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)
goto yyerrpop;}if(yyn == 842)return 0;yyvs[_check_known_subscript_notnull(10000,++
yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;goto yynewstate;}void Cyc_yyprint(int i,void*v){void*
_tmp862=v;struct _tuple13*_tmp863;struct _tuple13 _tmp864;int _tmp865;char _tmp866;
short _tmp867;struct _tagged_arr _tmp868;struct Cyc_Core_Opt*_tmp869;struct Cyc_Core_Opt*
_tmp86A;struct Cyc_Core_Opt _tmp86B;struct _tagged_arr*_tmp86C;struct _tuple1*
_tmp86D;struct _tuple1 _tmp86E;void*_tmp86F;struct _tagged_arr*_tmp870;_LL35A: if(*((
void**)_tmp862)!= Cyc_Int_tok)goto _LL35C;_tmp863=((struct Cyc_Int_tok_struct*)
_tmp862)->f1;_tmp864=*_tmp863;_tmp865=_tmp864.f2;_LL35B:({struct Cyc_Std_Int_pa_struct
_tmp872;_tmp872.tag=1;_tmp872.f1=(int)((unsigned int)_tmp865);{void*_tmp871[1]={&
_tmp872};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%d",sizeof(char),3),_tag_arr(
_tmp871,sizeof(void*),1));}});goto _LL359;_LL35C: if(*((void**)_tmp862)!= Cyc_Char_tok)
goto _LL35E;_tmp866=((struct Cyc_Char_tok_struct*)_tmp862)->f1;_LL35D:({struct Cyc_Std_Int_pa_struct
_tmp874;_tmp874.tag=1;_tmp874.f1=(int)((unsigned int)((int)_tmp866));{void*
_tmp873[1]={& _tmp874};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%c",sizeof(char),
3),_tag_arr(_tmp873,sizeof(void*),1));}});goto _LL359;_LL35E: if(*((void**)_tmp862)
!= Cyc_Short_tok)goto _LL360;_tmp867=((struct Cyc_Short_tok_struct*)_tmp862)->f1;
_LL35F:({struct Cyc_Std_Int_pa_struct _tmp876;_tmp876.tag=1;_tmp876.f1=(int)((
unsigned int)((int)_tmp867));{void*_tmp875[1]={& _tmp876};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%ds",sizeof(char),4),_tag_arr(_tmp875,sizeof(void*),1));}});goto _LL359;
_LL360: if(*((void**)_tmp862)!= Cyc_String_tok)goto _LL362;_tmp868=((struct Cyc_String_tok_struct*)
_tmp862)->f1;_LL361:({struct Cyc_Std_String_pa_struct _tmp878;_tmp878.tag=0;
_tmp878.f1=(struct _tagged_arr)_tmp868;{void*_tmp877[1]={& _tmp878};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("\"%s\"",sizeof(char),5),_tag_arr(_tmp877,sizeof(void*),
1));}});goto _LL359;_LL362: if(*((void**)_tmp862)!= Cyc_Stringopt_tok)goto _LL364;
_tmp869=((struct Cyc_Stringopt_tok_struct*)_tmp862)->f1;if(_tmp869 != 0)goto _LL364;
_LL363:({void*_tmp879[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("NULL",
sizeof(char),5),_tag_arr(_tmp879,sizeof(void*),0));});goto _LL359;_LL364: if(*((
void**)_tmp862)!= Cyc_Stringopt_tok)goto _LL366;_tmp86A=((struct Cyc_Stringopt_tok_struct*)
_tmp862)->f1;if(_tmp86A == 0)goto _LL366;_tmp86B=*_tmp86A;_tmp86C=(struct
_tagged_arr*)_tmp86B.v;_LL365:({struct Cyc_Std_String_pa_struct _tmp87B;_tmp87B.tag=
0;_tmp87B.f1=(struct _tagged_arr)*_tmp86C;{void*_tmp87A[1]={& _tmp87B};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("\"%s\"",sizeof(char),5),_tag_arr(_tmp87A,sizeof(void*),
1));}});goto _LL359;_LL366: if(*((void**)_tmp862)!= Cyc_QualId_tok)goto _LL368;
_tmp86D=((struct Cyc_QualId_tok_struct*)_tmp862)->f1;_tmp86E=*_tmp86D;_tmp86F=
_tmp86E.f1;_tmp870=_tmp86E.f2;_LL367: {struct Cyc_List_List*_tmp87C=0;{void*
_tmp87D=_tmp86F;struct Cyc_List_List*_tmp87E;struct Cyc_List_List*_tmp87F;_LL36B:
if(_tmp87D <= (void*)1?1:*((int*)_tmp87D)!= 0)goto _LL36D;_tmp87E=((struct Cyc_Absyn_Rel_n_struct*)
_tmp87D)->f1;_LL36C: _tmp87C=_tmp87E;goto _LL36A;_LL36D: if(_tmp87D <= (void*)1?1:*((
int*)_tmp87D)!= 1)goto _LL36F;_tmp87F=((struct Cyc_Absyn_Abs_n_struct*)_tmp87D)->f1;
_LL36E: _tmp87C=_tmp87F;goto _LL36A;_LL36F: if((int)_tmp87D != 0)goto _LL36A;_LL370:
goto _LL36A;_LL36A:;}for(0;_tmp87C != 0;_tmp87C=_tmp87C->tl){({struct Cyc_Std_String_pa_struct
_tmp881;_tmp881.tag=0;_tmp881.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp87C->hd);{void*_tmp880[1]={& _tmp881};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",
sizeof(char),5),_tag_arr(_tmp880,sizeof(void*),1));}});}({struct Cyc_Std_String_pa_struct
_tmp883;_tmp883.tag=0;_tmp883.f1=(struct _tagged_arr)*_tmp870;{void*_tmp882[1]={&
_tmp883};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",sizeof(char),5),_tag_arr(
_tmp882,sizeof(void*),1));}});goto _LL359;}_LL368:;_LL369:({void*_tmp884[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("?",sizeof(char),2),_tag_arr(_tmp884,
sizeof(void*),0));});goto _LL359;_LL359:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc_Std___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*
_tmp885=_cycalloc(sizeof(*_tmp885));_tmp885->v=Cyc_Lexing_from_file(f);_tmp885;});
Cyc_yyparse();return Cyc_Parse_parse_result;}
