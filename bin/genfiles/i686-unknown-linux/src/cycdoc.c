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
unsigned int);struct _tuple0{void*f1;void*f2;};extern void*Cyc_Core_fst(struct
_tuple0*);extern void*Cyc_Core_snd(struct _tuple0*);extern unsigned char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;
};extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*
Cyc_Std_stdout;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;extern int Cyc_Std_remove(
struct _tagged_arr);extern int Cyc_Std_fclose(struct Cyc_Std___cycFILE*);extern int
Cyc_Std_fflush(struct Cyc_Std___cycFILE*);extern struct Cyc_Std___cycFILE*Cyc_Std_fopen(
struct _tagged_arr __filename,struct _tagged_arr __modes);extern int Cyc_Std_fputc(int
__c,struct Cyc_Std___cycFILE*__stream);extern int Cyc_Std_fputs(struct _tagged_arr
__s,struct Cyc_Std___cycFILE*__stream);extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};extern void Cyc_Std_file_close(struct Cyc_Std___cycFILE*);
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern int Cyc_Std_printf(struct _tagged_arr fmt,
struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,
struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};extern unsigned char Cyc_Lexing_Error[
10];struct Cyc_Lexing_Error_struct{unsigned char*tag;struct _tagged_arr f1;};struct
Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;
struct _tagged_arr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;
int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct
Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _tagged_arr,int,void*);
void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _tagged_arr lex_base;
struct _tagged_arr lex_backtrk;struct _tagged_arr lex_default;struct _tagged_arr
lex_trans;struct _tagged_arr lex_check;};extern struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(
struct Cyc_Std___cycFILE*);extern int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern struct Cyc_List_List*
Cyc_List_list(struct _tagged_arr);extern int Cyc_List_length(struct Cyc_List_List*x);
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
extern unsigned char Cyc_List_List_mismatch[18];extern struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern unsigned char Cyc_List_Nth[8];extern struct Cyc_List_List*Cyc_List_zip(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern unsigned int Cyc_Std_strlen(
struct _tagged_arr s);extern int Cyc_Std_strcmp(struct _tagged_arr s1,struct
_tagged_arr s2);extern struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct
_tagged_arr);extern struct _tagged_arr Cyc_Std_strconcat_l(struct Cyc_List_List*);
extern struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,struct _tagged_arr);
extern struct _tagged_arr Cyc_Std_substring(struct _tagged_arr,int ofs,unsigned int n);
struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
line_no;int col;};extern void Cyc_Lineno_poss_of_abss(struct _tagged_arr filename,
struct Cyc_List_List*places);extern struct _tagged_arr Cyc_Filename_chop_extension(
struct _tagged_arr);extern unsigned char Cyc_Position_Exit[9];extern void Cyc_Position_reset_position(
struct _tagged_arr);struct Cyc_Position_Segment;struct Cyc_Position_Error{struct
_tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;
};extern unsigned char Cyc_Position_Nocontext[14];extern unsigned char Cyc_Array_Array_mismatch[
19];struct Cyc_Std__Div{int quot;int rem;};struct Cyc_Std__Ldiv{int quot;int rem;};
extern int Cyc_Std_system(struct _tagged_arr);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;extern unsigned char Cyc_Arg_Bad[8];struct Cyc_Arg_Bad_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Arg_Error[10];
struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};struct Cyc_Arg_FlagString_spec_struct{int
tag;void(*f1)(struct _tagged_arr,struct _tagged_arr);};struct Cyc_Arg_Set_spec_struct{
int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*
f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _tagged_arr);};
extern void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct
_tagged_arr),struct _tagged_arr errmsg,struct _tagged_arr args);struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
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
15];extern void*Cyc_Absyn_compress_kb(void*);struct _tuple4{void*f1;struct _tuple1*
f2;};extern struct _tuple4 Cyc_Absyn_aggr_kinded_name(void*);extern struct Cyc_List_List*
Cyc_Parse_parse_file(struct Cyc_Std___cycFILE*f);struct Cyc_Declaration_spec;
struct Cyc_Declarator;struct Cyc_Abstractdeclarator;extern int Cyc_yyparse();extern
unsigned char Cyc_AbstractDeclarator_tok[27];struct Cyc_AbstractDeclarator_tok_struct{
unsigned char*tag;struct Cyc_Abstractdeclarator*f1;};extern unsigned char Cyc_AggrFieldDeclListList_tok[
30];struct Cyc_AggrFieldDeclListList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_AggrFieldDeclList_tok[26];struct Cyc_AggrFieldDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_AttributeList_tok[
22];struct Cyc_AttributeList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;
};extern unsigned char Cyc_Attribute_tok[18];struct Cyc_Attribute_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Bool_tok[13];struct Cyc_Bool_tok_struct{
unsigned char*tag;int f1;};extern unsigned char Cyc_Char_tok[13];struct Cyc_Char_tok_struct{
unsigned char*tag;unsigned char f1;};extern unsigned char Cyc_DeclList_tok[17];
struct Cyc_DeclList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_DeclSpec_tok[17];struct Cyc_DeclSpec_tok_struct{unsigned char*
tag;struct Cyc_Declaration_spec*f1;};extern unsigned char Cyc_Declarator_tok[19];
struct Cyc_Declarator_tok_struct{unsigned char*tag;struct Cyc_Declarator*f1;};
extern unsigned char Cyc_DesignatorList_tok[23];struct Cyc_DesignatorList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Designator_tok[
19];struct Cyc_Designator_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_EnumfieldList_tok[22];struct Cyc_EnumfieldList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Enumfield_tok[
18];struct Cyc_Enumfield_tok_struct{unsigned char*tag;struct Cyc_Absyn_Enumfield*
f1;};extern unsigned char Cyc_ExpList_tok[16];struct Cyc_ExpList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Exp_tok[12];
struct Cyc_Exp_tok_struct{unsigned char*tag;struct Cyc_Absyn_Exp*f1;};extern
unsigned char Cyc_FieldPatternList_tok[25];struct Cyc_FieldPatternList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_FieldPattern_tok[
21];struct _tuple5{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
unsigned char*tag;struct _tuple5*f1;};extern unsigned char Cyc_FnDecl_tok[15];
struct Cyc_FnDecl_tok_struct{unsigned char*tag;struct Cyc_Absyn_Fndecl*f1;};extern
unsigned char Cyc_IdList_tok[15];struct Cyc_IdList_tok_struct{unsigned char*tag;
struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDeclList_tok[21];struct Cyc_InitDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDecl_tok[
17];struct _tuple6{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
unsigned char*tag;struct _tuple6*f1;};extern unsigned char Cyc_InitializerList_tok[
24];struct Cyc_InitializerList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_Int_tok[12];struct _tuple7{void*f1;int f2;};struct Cyc_Int_tok_struct{
unsigned char*tag;struct _tuple7*f1;};extern unsigned char Cyc_Kind_tok[13];struct
Cyc_Kind_tok_struct{unsigned char*tag;void*f1;};extern unsigned char Cyc_Okay_tok[
13];extern unsigned char Cyc_ParamDeclListBool_tok[26];struct _tuple8{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct Cyc_ParamDeclListBool_tok_struct{unsigned char*tag;struct _tuple8*f1;}
;extern unsigned char Cyc_ParamDeclList_tok[22];struct Cyc_ParamDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_ParamDecl_tok[
18];struct Cyc_ParamDecl_tok_struct{unsigned char*tag;struct _tuple2*f1;};extern
unsigned char Cyc_PatternList_tok[20];struct Cyc_PatternList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Pattern_tok[16];
struct Cyc_Pattern_tok_struct{unsigned char*tag;struct Cyc_Absyn_Pat*f1;};extern
unsigned char Cyc_Pointer_Sort_tok[21];struct Cyc_Pointer_Sort_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Primop_tok[15];struct Cyc_Primop_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Primopopt_tok[18];struct Cyc_Primopopt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_QualId_tok[15];
struct Cyc_QualId_tok_struct{unsigned char*tag;struct _tuple1*f1;};extern
unsigned char Cyc_QualSpecList_tok[21];struct _tuple9{struct Cyc_Absyn_Tqual f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_QualSpecList_tok_struct{
unsigned char*tag;struct _tuple9*f1;};extern unsigned char Cyc_Rgnorder_tok[17];
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
struct Cyc_YY1_struct{unsigned char*tag;struct _tuple8*f1;};extern unsigned char Cyc_YYINITIALSVAL[
18];struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;
int last_column;};struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern struct
_tagged_arr*Cyc_Absynpp_cyc_stringptr;extern int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
extern struct _tagged_arr Cyc_Absynpp_char_escape(unsigned char);extern struct
_tagged_arr Cyc_Absynpp_string_escape(struct _tagged_arr);extern struct _tagged_arr
Cyc_Absynpp_prim2str(void*p);struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;
struct Cyc_List_List*f3;};extern struct _tuple10 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual
tq,void*t);extern void Cyc_Lex_lex_init();struct Cyc_Position_Segment{int start;int
end;};struct Cyc_MatchDecl_struct{int tag;struct _tagged_arr f1;};struct Cyc_Standalone_struct{
int tag;struct _tagged_arr f1;};struct _tuple11{int f1;void*f2;};extern struct _tuple11*
Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf);const int Cyc_lex_base[15]=(const int[15]){
0,- 4,0,- 3,1,2,3,0,4,6,7,- 1,8,9,- 2};const int Cyc_lex_backtrk[15]=(const int[15]){- 1,
- 1,3,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[15]=(const int[
15]){1,0,- 1,0,- 1,- 1,6,- 1,8,8,8,0,6,6,0};const int Cyc_lex_trans[266]=(const int[266]){
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,6,0,0,0,0,0,
0,0,4,5,7,12,9,2,10,10,13,13,0,11,- 1,14,- 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,- 1,- 1,0,- 1,- 1,- 1,- 1};const int Cyc_lex_check[
266]=(const int[266]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,7,- 1,5,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,4,5,6,8,0,9,10,
12,13,- 1,9,10,12,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,6,8,- 1,9,10,12,13};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;int base;
int backtrk;int c;state=start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=
lbuf->lex_curr_pos);lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){
base=Cyc_lex_base[state];if(base < 0){return(- base)- 1;}backtrk=Cyc_lex_backtrk[
state];if(backtrk >= 0){lbuf->lex_last_pos=lbuf->lex_curr_pos;lbuf->lex_last_action=
backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){if(! lbuf->lex_eof_reached){
return(- state)- 1;}else{c=256;}}else{c=(int)*((unsigned char*)
_check_unknown_subscript(lbuf->lex_buffer,sizeof(unsigned char),lbuf->lex_curr_pos
++));if(c == - 1){c=256;}}if(Cyc_lex_check[_check_known_subscript_notnull(266,base
+ c)]== state){state=Cyc_lex_trans[_check_known_subscript_notnull(266,base + c)];}
else{state=Cyc_lex_default[state];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1){(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({struct Cyc_Lexing_Error_struct _tmp1;
_tmp1.tag=Cyc_Lexing_Error;_tmp1.f1=_tag_arr("empty token",sizeof(unsigned char),
12);_tmp1;});_tmp0;}));}else{return lbuf->lex_last_action;}}else{if(c == 256){lbuf->lex_eof_reached=
0;}}}}struct _tuple11*Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL0: {int _tmp2=
lexbuf->lex_start_pos + 5;int _tmp3=(lexbuf->lex_curr_pos - lexbuf->lex_start_pos)- 
7;return({struct _tuple11*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->f1=Cyc_Lexing_lexeme_start(
lexbuf);_tmp4->f2=(void*)({struct Cyc_Standalone_struct*_tmp5=_cycalloc(sizeof(*
_tmp5));_tmp5[0]=({struct Cyc_Standalone_struct _tmp6;_tmp6.tag=1;_tmp6.f1=Cyc_Std_substring((
struct _tagged_arr)lexbuf->lex_buffer,_tmp2,(unsigned int)_tmp3);_tmp6;});_tmp5;});
_tmp4;});}case 1: _LL1: {int _tmp7=lexbuf->lex_start_pos + 4;int _tmp8=(lexbuf->lex_curr_pos
- lexbuf->lex_start_pos)- 6;return({struct _tuple11*_tmp9=_cycalloc(sizeof(*_tmp9));
_tmp9->f1=Cyc_Lexing_lexeme_start(lexbuf);_tmp9->f2=(void*)({struct Cyc_MatchDecl_struct*
_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA[0]=({struct Cyc_MatchDecl_struct _tmpB;_tmpB.tag=
0;_tmpB.f1=Cyc_Std_substring((struct _tagged_arr)lexbuf->lex_buffer,_tmp7,(
unsigned int)_tmp8);_tmpB;});_tmpA;});_tmp9;});}case 2: _LL2: return 0;case 3: _LL3:
return Cyc_token(lexbuf);default: _LL4:(lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpC=
_cycalloc(sizeof(*_tmpC));_tmpC[0]=({struct Cyc_Lexing_Error_struct _tmpD;_tmpD.tag=
Cyc_Lexing_Error;_tmpD.f1=_tag_arr("some action didn't return!",sizeof(
unsigned char),27);_tmpD;});_tmpC;}));}struct _tuple11*Cyc_token(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_token_rec(lexbuf,0);}struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[
11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern
unsigned char Cyc_Dict_Absent[11];struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};
struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct
Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;
};struct Cyc_Tcenv_Frame_struct{int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*
ae;struct Cyc_Core_Opt*le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*
Cyc_Tcutil_compress(void*t);extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);void*Cyc_Dump_last_dumped=(void*)1;void Cyc_Dump_dump_alpha(
struct _tagged_arr s){if(Cyc_Dump_last_dumped == (void*)0){({struct Cyc_Std_String_pa_struct
_tmpF;_tmpF.tag=0;_tmpF.f1=(struct _tagged_arr)s;{void*_tmpE[1]={& _tmpF};Cyc_Std_printf(
_tag_arr(" %s",sizeof(unsigned char),4),_tag_arr(_tmpE,sizeof(void*),1));}});}
else{({struct Cyc_Std_String_pa_struct _tmp11;_tmp11.tag=0;_tmp11.f1=(struct
_tagged_arr)s;{void*_tmp10[1]={& _tmp11};Cyc_Std_printf(_tag_arr("%s",sizeof(
unsigned char),3),_tag_arr(_tmp10,sizeof(void*),1));}});Cyc_Dump_last_dumped=(
void*)0;}}void Cyc_Dump_dump_alphastr(struct _tagged_arr*s){Cyc_Dump_dump_alpha(*s);}
void Cyc_Dump_dump_symbol(struct _tagged_arr s){({struct Cyc_Std_String_pa_struct
_tmp13;_tmp13.tag=0;_tmp13.f1=(struct _tagged_arr)s;{void*_tmp12[1]={& _tmp13};Cyc_Std_printf(
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp12,sizeof(void*),1));}});Cyc_Dump_last_dumped=(
void*)1;}void Cyc_Dump_dump_char(int c){({struct Cyc_Std_Int_pa_struct _tmp15;_tmp15.tag=
1;_tmp15.f1=(int)((unsigned int)c);{void*_tmp14[1]={& _tmp15};Cyc_Std_printf(
_tag_arr("%c",sizeof(unsigned char),3),_tag_arr(_tmp14,sizeof(void*),1));}});Cyc_Dump_last_dumped=(
void*)1;}void Cyc_Dump_dump_pointer(struct _tagged_arr s){if(Cyc_Dump_last_dumped == (
void*)0){({struct Cyc_Std_String_pa_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(struct
_tagged_arr)s;{void*_tmp16[1]={& _tmp17};Cyc_Std_printf(_tag_arr(" %s",sizeof(
unsigned char),4),_tag_arr(_tmp16,sizeof(void*),1));}});}else{({struct Cyc_Std_String_pa_struct
_tmp19;_tmp19.tag=0;_tmp19.f1=(struct _tagged_arr)s;{void*_tmp18[1]={& _tmp19};Cyc_Std_printf(
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp18,sizeof(void*),1));}});}
Cyc_Dump_last_dumped=(void*)2;}void Cyc_Dump_dump_boldalpha(struct _tagged_arr x){
Cyc_Dump_dump_alpha(_tag_arr("",sizeof(unsigned char),1));Cyc_Dump_dump_symbol(
_tag_arr("\\textbf{",sizeof(unsigned char),9));Cyc_Dump_dump_alpha(x);Cyc_Dump_dump_symbol(
_tag_arr("}",sizeof(unsigned char),2));Cyc_Dump_dump_alpha(_tag_arr("",sizeof(
unsigned char),1));}void Cyc_Dump_dump_lb(){Cyc_Dump_dump_symbol(_tag_arr("{\\lb}",
sizeof(unsigned char),6));}void Cyc_Dump_dump_rb(){Cyc_Dump_dump_symbol(_tag_arr("{\\rb}",
sizeof(unsigned char),6));}void Cyc_Dump_dump_begin(){Cyc_Dump_dump_symbol(
_tag_arr("%%HEVEA \\begin{latexonly}\n\\begin{list}{}{\\setlength\\itemsep{0pt}\\setlength\\topsep{0pt}\\setlength\\leftmargin{\\parindent}\\setlength\\itemindent{-\\leftmargin}}\\item[]\\colorbox{cycdochighlight}{\\ttfamily\\begin{minipage}[b]{\\textwidth}\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}<dl><dt><table><tr><td bgcolor=\"c0d0ff\">\\end{rawhtml}\n\\begin{tabbing}\n",
sizeof(unsigned char),348));}void Cyc_Dump_dump_middle(){Cyc_Dump_dump_symbol(
_tag_arr("\\end{tabbing}\n%%HEVEA \\begin{latexonly}\n\\end{minipage}}\\\\\\strut\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</td></tr></table><dd>\\end{rawhtml}\n",
sizeof(unsigned char),148));}void Cyc_Dump_dump_end(){Cyc_Dump_dump_symbol(
_tag_arr("%%HEVEA \\begin{latexonly}\n\\end{list}\\smallskip\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</dl>\\end{rawhtml}\n",
sizeof(unsigned char),114));}void Cyc_Dump_dumpqvar(struct _tuple1*v);void Cyc_Dump_dump_boldqvar(
struct _tuple1*v){Cyc_Dump_dump_alpha(_tag_arr("",sizeof(unsigned char),1));Cyc_Dump_dump_symbol(
_tag_arr("\\textbf{",sizeof(unsigned char),9));Cyc_Dump_dumpqvar(v);Cyc_Dump_dump_symbol(
_tag_arr("}",sizeof(unsigned char),2));Cyc_Dump_dump_alpha(_tag_arr("",sizeof(
unsigned char),1));}static int Cyc_Dump_expand_typedefs=0;static int Cyc_Dump_qvar_to_Cids=
0;static int Cyc_Dump_add_cyc_prefix=0;static int Cyc_Dump_to_VC=0;void Cyc_Dump_dumpenumfields(
struct Cyc_List_List*fields);void Cyc_Dump_dumptyp(void*);void Cyc_Dump_dumpntyp(
void*t);void Cyc_Dump_dumpexp(struct Cyc_Absyn_Exp*);void Cyc_Dump_dumpexp_prec(int,
struct Cyc_Absyn_Exp*);void Cyc_Dump_dumpdecl(struct Cyc_Absyn_Decl*,struct
_tagged_arr);void Cyc_Dump_dumptms(struct Cyc_List_List*,void(*f)(void*),void*);
void Cyc_Dump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(void*),void*);void Cyc_Dump_dumpaggrfields(
struct Cyc_List_List*fields);void Cyc_Dump_ignore(void*x){return;}void Cyc_Dump_dump_semi(){
Cyc_Dump_dump_char((int)';');}void Cyc_Dump_dump_sep(void(*f)(void*),struct Cyc_List_List*
l,struct _tagged_arr sep){if(l == 0){return;}for(0;l->tl != 0;l=l->tl){f((void*)l->hd);
Cyc_Dump_dump_symbol(sep);}f((void*)l->hd);}void Cyc_Dump_dump_sep_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*l,struct _tagged_arr sep){if(l == 0){
return;}for(0;l->tl != 0;l=l->tl){f(env,(void*)l->hd);Cyc_Dump_dump_symbol(sep);}
f(env,(void*)l->hd);}void Cyc_Dump_group(void(*f)(void*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep){Cyc_Dump_dump_symbol(
start);Cyc_Dump_dump_sep(f,l,sep);Cyc_Dump_dump_symbol(end);}void Cyc_Dump_group_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep){Cyc_Dump_dump_symbol(start);Cyc_Dump_dump_sep_c(
f,env,l,sep);Cyc_Dump_dump_symbol(end);}void Cyc_Dump_egroup(void(*f)(void*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep){if(l != 0){Cyc_Dump_group(f,l,start,end,sep);}}void Cyc_Dump_dumpqvar(
struct _tuple1*v){struct Cyc_List_List*_tmp1A=0;{void*_tmp1B=(*v).f1;struct Cyc_List_List*
_tmp1C;struct Cyc_List_List*_tmp1D;_LL7: if((int)_tmp1B == 0){goto _LL8;}else{goto
_LL9;}_LL9: if((unsigned int)_tmp1B > 1?*((int*)_tmp1B)== 0: 0){_LLD: _tmp1C=((
struct Cyc_Absyn_Rel_n_struct*)_tmp1B)->f1;goto _LLA;}else{goto _LLB;}_LLB: if((
unsigned int)_tmp1B > 1?*((int*)_tmp1B)== 1: 0){_LLE: _tmp1D=((struct Cyc_Absyn_Abs_n_struct*)
_tmp1B)->f1;goto _LLC;}else{goto _LL6;}_LL8: _tmp1C=0;goto _LLA;_LLA: _tmp1A=_tmp1C;
goto _LL6;_LLC: _tmp1A=(Cyc_Dump_qvar_to_Cids? Cyc_Dump_add_cyc_prefix: 0)?({struct
Cyc_List_List*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->hd=Cyc_Absynpp_cyc_stringptr;
_tmp1E->tl=_tmp1D;_tmp1E;}): _tmp1D;goto _LL6;_LL6:;}if(_tmp1A != 0){Cyc_Dump_dump_alphastr((
struct _tagged_arr*)_tmp1A->hd);for(_tmp1A=_tmp1A->tl;_tmp1A != 0;_tmp1A=_tmp1A->tl){
if(Cyc_Dump_qvar_to_Cids){Cyc_Dump_dump_char((int)'_');}else{Cyc_Dump_dump_symbol(
_tag_arr("::",sizeof(unsigned char),3));}Cyc_Dump_dump_alpha(*((struct
_tagged_arr*)_tmp1A->hd));}if(Cyc_Dump_qvar_to_Cids){Cyc_Dump_dump_symbol(
_tag_arr("_",sizeof(unsigned char),2));}else{Cyc_Dump_dump_symbol(_tag_arr("::",
sizeof(unsigned char),3));}Cyc_Dump_dump_alpha(*(*v).f2);}else{Cyc_Dump_dump_alpha(*(*
v).f2);}}void Cyc_Dump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict){Cyc_Dump_dump_alpha(
_tag_arr("restrict",sizeof(unsigned char),9));}if(tq.q_volatile){Cyc_Dump_dump_alpha(
_tag_arr("volatile",sizeof(unsigned char),9));}if(tq.q_const){Cyc_Dump_dump_alpha(
_tag_arr("const",sizeof(unsigned char),6));}}void Cyc_Dump_dumpscope(void*sc){
void*_tmp1F=sc;_LL10: if((int)_tmp1F == 0){goto _LL11;}else{goto _LL12;}_LL12: if((
int)_tmp1F == 2){goto _LL13;}else{goto _LL14;}_LL14: if((int)_tmp1F == 3){goto _LL15;}
else{goto _LL16;}_LL16: if((int)_tmp1F == 4){goto _LL17;}else{goto _LL18;}_LL18: if((
int)_tmp1F == 1){goto _LL19;}else{goto _LLF;}_LL11: Cyc_Dump_dump_alpha(_tag_arr("static",
sizeof(unsigned char),7));return;_LL13: return;_LL15: Cyc_Dump_dump_alpha(_tag_arr("extern",
sizeof(unsigned char),7));return;_LL17: Cyc_Dump_dump_alpha(_tag_arr("extern \"C\"",
sizeof(unsigned char),11));return;_LL19: Cyc_Dump_dump_alpha(_tag_arr("abstract",
sizeof(unsigned char),9));return;_LLF:;}void Cyc_Dump_dumpkind(void*k){void*
_tmp20=k;_LL1B: if((int)_tmp20 == 0){goto _LL1C;}else{goto _LL1D;}_LL1D: if((int)
_tmp20 == 1){goto _LL1E;}else{goto _LL1F;}_LL1F: if((int)_tmp20 == 2){goto _LL20;}
else{goto _LL21;}_LL21: if((int)_tmp20 == 3){goto _LL22;}else{goto _LL23;}_LL23: if((
int)_tmp20 == 4){goto _LL24;}else{goto _LL1A;}_LL1C: Cyc_Dump_dump_alpha(_tag_arr("A",
sizeof(unsigned char),2));return;_LL1E: Cyc_Dump_dump_alpha(_tag_arr("M",sizeof(
unsigned char),2));return;_LL20: Cyc_Dump_dump_alpha(_tag_arr("B",sizeof(
unsigned char),2));return;_LL22: Cyc_Dump_dump_alpha(_tag_arr("R",sizeof(
unsigned char),2));return;_LL24: Cyc_Dump_dump_alpha(_tag_arr("E",sizeof(
unsigned char),2));return;_LL1A:;}void Cyc_Dump_dumptps(struct Cyc_List_List*ts){
Cyc_Dump_egroup(Cyc_Dump_dumptyp,ts,_tag_arr("<",sizeof(unsigned char),2),
_tag_arr(">",sizeof(unsigned char),2),_tag_arr(", ",sizeof(unsigned char),3));
Cyc_Dump_dump_alpha(_tag_arr("",sizeof(unsigned char),1));}void Cyc_Dump_dumptvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Dump_dump_alphastr(tv->name);}void Cyc_Dump_dumpkindedtvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Dump_dump_alphastr(tv->name);{void*_tmp21=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmp22;void*_tmp23;void*_tmp24;_LL26: if(*((int*)_tmp21)== 0){
_LL2E: _tmp22=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp21)->f1;if((int)_tmp22 == 
2){goto _LL27;}else{goto _LL28;}}else{goto _LL28;}_LL28: if(*((int*)_tmp21)== 2){
_LL2F: _tmp23=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp21)->f2;goto _LL29;}
else{goto _LL2A;}_LL2A: if(*((int*)_tmp21)== 0){_LL30: _tmp24=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp21)->f1;goto _LL2B;}else{goto _LL2C;}_LL2C: goto _LL2D;_LL27: goto _LL25;_LL29:
_tmp24=_tmp23;goto _LL2B;_LL2B: Cyc_Dump_dump_symbol(_tag_arr("::",sizeof(
unsigned char),3));Cyc_Dump_dumpkind(_tmp24);goto _LL25;_LL2D: Cyc_Dump_dump_symbol(
_tag_arr("::?",sizeof(unsigned char),4));goto _LL25;_LL25:;}}void Cyc_Dump_dumptvars(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_egroup)(
Cyc_Dump_dumptvar,tvs,_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",sizeof(
unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));Cyc_Dump_dump_alpha(
_tag_arr("",sizeof(unsigned char),1));}void Cyc_Dump_dumpkindedtvars(struct Cyc_List_List*
tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_egroup)(Cyc_Dump_dumpkindedtvar,
tvs,_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",sizeof(unsigned char),2),
_tag_arr(",",sizeof(unsigned char),2));Cyc_Dump_dump_alpha(_tag_arr("",sizeof(
unsigned char),1));}struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Dump_dumparg(
struct _tuple12*pr){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Dump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Dump_ignore,0);}void Cyc_Dump_dumpargs(struct
Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple12*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(
Cyc_Dump_dumparg,ts,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(
unsigned char),2),_tag_arr(", ",sizeof(unsigned char),3));}int Cyc_Dump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0){return 0;}{void*_tmp25=(void*)tms->hd;_LL32:
if((unsigned int)_tmp25 > 1?*((int*)_tmp25)== 1: 0){goto _LL33;}else{goto _LL34;}
_LL34: goto _LL35;_LL33: return 1;_LL35: return 0;_LL31:;}}static void Cyc_Dump_dumprgn(
void*t){void*_tmp26=Cyc_Tcutil_compress(t);_LL37: if((int)_tmp26 == 2){goto _LL38;}
else{goto _LL39;}_LL39: goto _LL3A;_LL38: Cyc_Dump_dump_alpha(_tag_arr("`H",sizeof(
unsigned char),3));goto _LL36;_LL3A: Cyc_Dump_dumpntyp(t);goto _LL36;_LL36:;}struct
_tuple13{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static struct _tuple13 Cyc_Dump_effects_split(
void*t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;{void*_tmp27=
Cyc_Tcutil_compress(t);void*_tmp28;struct Cyc_List_List*_tmp29;_LL3C: if((
unsigned int)_tmp27 > 3?*((int*)_tmp27)== 17: 0){_LL42: _tmp28=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp27)->f1;goto _LL3D;}else{goto _LL3E;}_LL3E: if((unsigned int)_tmp27 > 3?*((int*)
_tmp27)== 18: 0){_LL43: _tmp29=((struct Cyc_Absyn_JoinEff_struct*)_tmp27)->f1;goto
_LL3F;}else{goto _LL40;}_LL40: goto _LL41;_LL3D: rgions=({struct Cyc_List_List*_tmp2A=
_cycalloc(sizeof(*_tmp2A));_tmp2A->hd=(void*)_tmp28;_tmp2A->tl=rgions;_tmp2A;});
goto _LL3B;_LL3F: for(0;_tmp29 != 0;_tmp29=_tmp29->tl){struct Cyc_List_List*_tmp2C;
struct Cyc_List_List*_tmp2D;struct _tuple13 _tmp2B=Cyc_Dump_effects_split((void*)
_tmp29->hd);_LL46: _tmp2D=_tmp2B.f1;goto _LL45;_LL45: _tmp2C=_tmp2B.f2;goto _LL44;
_LL44: rgions=Cyc_List_imp_append(_tmp2D,rgions);effects=Cyc_List_imp_append(
_tmp2C,effects);}goto _LL3B;_LL41: effects=({struct Cyc_List_List*_tmp2E=_cycalloc(
sizeof(*_tmp2E));_tmp2E->hd=(void*)t;_tmp2E->tl=effects;_tmp2E;});goto _LL3B;
_LL3B:;}return({struct _tuple13 _tmp2F;_tmp2F.f1=rgions;_tmp2F.f2=effects;_tmp2F;});}
static void Cyc_Dump_dumpeff(void*t){struct Cyc_List_List*_tmp31;struct Cyc_List_List*
_tmp32;struct _tuple13 _tmp30=Cyc_Dump_effects_split(t);_LL49: _tmp32=_tmp30.f1;
goto _LL48;_LL48: _tmp31=_tmp30.f2;goto _LL47;_LL47: _tmp32=Cyc_List_imp_rev(_tmp32);
_tmp31=Cyc_List_imp_rev(_tmp31);for(0;_tmp31 != 0;_tmp31=_tmp31->tl){Cyc_Dump_dumpntyp((
void*)_tmp31->hd);Cyc_Dump_dump_char((int)'+');}Cyc_Dump_dump_lb();for(0;_tmp32
!= 0;_tmp32=_tmp32->tl){Cyc_Dump_dumprgn((void*)_tmp32->hd);if(_tmp32->tl != 0){
Cyc_Dump_dump_char((int)',');}}Cyc_Dump_dump_rb();}void Cyc_Dump_dumpntyp(void*t){
void*_tmp33=t;struct Cyc_Absyn_Tvar*_tmp34;int _tmp35;struct Cyc_Core_Opt*_tmp36;
struct Cyc_Core_Opt*_tmp37;int _tmp38;struct Cyc_Core_Opt*_tmp39;struct Cyc_Core_Opt
_tmp3A;void*_tmp3B;struct Cyc_Core_Opt*_tmp3C;struct Cyc_Absyn_TunionInfo _tmp3D;
void*_tmp3E;struct Cyc_List_List*_tmp3F;void*_tmp40;struct Cyc_Absyn_TunionFieldInfo
_tmp41;struct Cyc_List_List*_tmp42;void*_tmp43;struct _tuple1*_tmp44;struct Cyc_List_List*
_tmp45;void*_tmp46;void*_tmp47;void*_tmp48;void*_tmp49;void*_tmp4A;void*_tmp4B;
void*_tmp4C;void*_tmp4D;void*_tmp4E;void*_tmp4F;void*_tmp50;void*_tmp51;void*
_tmp52;void*_tmp53;void*_tmp54;void*_tmp55;void*_tmp56;void*_tmp57;void*_tmp58;
void*_tmp59;int _tmp5A;void*_tmp5B;struct Cyc_List_List*_tmp5C;struct Cyc_Absyn_AggrInfo
_tmp5D;struct Cyc_List_List*_tmp5E;void*_tmp5F;struct Cyc_List_List*_tmp60;void*
_tmp61;struct Cyc_Core_Opt*_tmp62;struct Cyc_List_List*_tmp63;struct _tuple1*_tmp64;
void*_tmp65;_LL4B: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 7: 0){goto _LL4C;}
else{goto _LL4D;}_LL4D: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 8: 0){goto
_LL4E;}else{goto _LL4F;}_LL4F: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 4: 0){
goto _LL50;}else{goto _LL51;}_LL51: if((int)_tmp33 == 0){goto _LL52;}else{goto _LL53;}
_LL53: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 1: 0){_LL8D: _tmp34=((struct Cyc_Absyn_VarType_struct*)
_tmp33)->f1;goto _LL54;}else{goto _LL55;}_LL55: if((unsigned int)_tmp33 > 3?*((int*)
_tmp33)== 0: 0){_LL90: _tmp37=((struct Cyc_Absyn_Evar_struct*)_tmp33)->f1;goto _LL8F;
_LL8F: _tmp36=((struct Cyc_Absyn_Evar_struct*)_tmp33)->f2;if(_tmp36 == 0){goto _LL8E;}
else{goto _LL57;}_LL8E: _tmp35=((struct Cyc_Absyn_Evar_struct*)_tmp33)->f3;goto
_LL56;}else{goto _LL57;}_LL57: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 0: 0){
_LL94: _tmp3C=((struct Cyc_Absyn_Evar_struct*)_tmp33)->f1;goto _LL92;_LL92: _tmp39=((
struct Cyc_Absyn_Evar_struct*)_tmp33)->f2;if(_tmp39 == 0){goto _LL59;}else{_tmp3A=*
_tmp39;_LL93: _tmp3B=(void*)_tmp3A.v;goto _LL91;}_LL91: _tmp38=((struct Cyc_Absyn_Evar_struct*)
_tmp33)->f3;goto _LL58;}else{goto _LL59;}_LL59: if((unsigned int)_tmp33 > 3?*((int*)
_tmp33)== 2: 0){_LL95: _tmp3D=((struct Cyc_Absyn_TunionType_struct*)_tmp33)->f1;
_LL98: _tmp40=(void*)_tmp3D.tunion_info;goto _LL97;_LL97: _tmp3F=_tmp3D.targs;goto
_LL96;_LL96: _tmp3E=(void*)_tmp3D.rgn;goto _LL5A;}else{goto _LL5B;}_LL5B: if((
unsigned int)_tmp33 > 3?*((int*)_tmp33)== 3: 0){_LL99: _tmp41=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp33)->f1;_LL9B: _tmp43=(void*)_tmp41.field_info;goto _LL9A;_LL9A: _tmp42=_tmp41.targs;
goto _LL5C;}else{goto _LL5D;}_LL5D: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 12:
0){_LL9C: _tmp44=((struct Cyc_Absyn_EnumType_struct*)_tmp33)->f1;goto _LL5E;}else{
goto _LL5F;}_LL5F: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 13: 0){_LL9D: _tmp45=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp33)->f1;goto _LL60;}else{goto _LL61;}
_LL61: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5: 0){_LL9F: _tmp47=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp47 == 0){goto _LL9E;}else{
goto _LL63;}_LL9E: _tmp46=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((
int)_tmp46 == 2){goto _LL62;}else{goto _LL63;}}else{goto _LL63;}_LL63: if((
unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5: 0){_LLA1: _tmp49=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f1;if((int)_tmp49 == 0){goto _LLA0;}else{goto _LL65;}_LLA0: _tmp48=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((int)_tmp48 == 0){goto _LL64;}else{
goto _LL65;}}else{goto _LL65;}_LL65: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5:
0){_LLA3: _tmp4B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)
_tmp4B == 1){goto _LLA2;}else{goto _LL67;}_LLA2: _tmp4A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f2;if((int)_tmp4A == 0){goto _LL66;}else{goto _LL67;}}else{goto _LL67;}
_LL67: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5: 0){_LLA5: _tmp4D=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp4D == 0){goto _LLA4;}else{
goto _LL69;}_LLA4: _tmp4C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((
int)_tmp4C == 1){goto _LL68;}else{goto _LL69;}}else{goto _LL69;}_LL69: if((
unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5: 0){_LLA7: _tmp4F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f1;if((int)_tmp4F == 1){goto _LLA6;}else{goto _LL6B;}_LLA6: _tmp4E=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((int)_tmp4E == 1){goto _LL6A;}else{
goto _LL6B;}}else{goto _LL6B;}_LL6B: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5:
0){_LLA9: _tmp51=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)
_tmp51 == 1){goto _LLA8;}else{goto _LL6D;}_LLA8: _tmp50=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f2;if((int)_tmp50 == 2){goto _LL6C;}else{goto _LL6D;}}else{goto _LL6D;}
_LL6D: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5: 0){_LLAC: _tmp53=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp53 == 0){goto _LLAB;}else{
goto _LL6F;}_LLAB: _tmp52=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((
int)_tmp52 == 3){goto _LLAA;}else{goto _LL6F;}}else{goto _LL6F;}_LLAA: if(Cyc_Dump_to_VC){
goto _LL6E;}else{goto _LL6F;}_LL6F: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5:
0){_LLAE: _tmp55=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)
_tmp55 == 0){goto _LLAD;}else{goto _LL71;}_LLAD: _tmp54=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f2;if((int)_tmp54 == 3){goto _LL70;}else{goto _LL71;}}else{goto _LL71;}
_LL71: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5: 0){_LLB1: _tmp57=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp57 == 1){goto _LLB0;}else{
goto _LL73;}_LLB0: _tmp56=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((
int)_tmp56 == 3){goto _LLAF;}else{goto _LL73;}}else{goto _LL73;}_LLAF: if(Cyc_Dump_to_VC){
goto _LL72;}else{goto _LL73;}_LL73: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5:
0){_LLB3: _tmp59=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)
_tmp59 == 1){goto _LLB2;}else{goto _LL75;}_LLB2: _tmp58=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f2;if((int)_tmp58 == 3){goto _LL74;}else{goto _LL75;}}else{goto _LL75;}
_LL75: if((int)_tmp33 == 1){goto _LL76;}else{goto _LL77;}_LL77: if((unsigned int)
_tmp33 > 3?*((int*)_tmp33)== 6: 0){_LLB4: _tmp5A=((struct Cyc_Absyn_DoubleType_struct*)
_tmp33)->f1;goto _LL78;}else{goto _LL79;}_LL79: if((unsigned int)_tmp33 > 3?*((int*)
_tmp33)== 14: 0){_LLB5: _tmp5B=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp33)->f1;
goto _LL7A;}else{goto _LL7B;}_LL7B: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 9:
0){_LLB6: _tmp5C=((struct Cyc_Absyn_TupleType_struct*)_tmp33)->f1;goto _LL7C;}else{
goto _LL7D;}_LL7D: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 10: 0){_LLB7: _tmp5D=((
struct Cyc_Absyn_AggrType_struct*)_tmp33)->f1;_LLB9: _tmp5F=(void*)_tmp5D.aggr_info;
goto _LLB8;_LLB8: _tmp5E=_tmp5D.targs;goto _LL7E;}else{goto _LL7F;}_LL7F: if((
unsigned int)_tmp33 > 3?*((int*)_tmp33)== 11: 0){_LLBB: _tmp61=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp33)->f1;goto _LLBA;_LLBA: _tmp60=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp33)->f2;
goto _LL80;}else{goto _LL81;}_LL81: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 16:
0){_LLBE: _tmp64=((struct Cyc_Absyn_TypedefType_struct*)_tmp33)->f1;goto _LLBD;
_LLBD: _tmp63=((struct Cyc_Absyn_TypedefType_struct*)_tmp33)->f2;goto _LLBC;_LLBC:
_tmp62=((struct Cyc_Absyn_TypedefType_struct*)_tmp33)->f3;goto _LL82;}else{goto
_LL83;}_LL83: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 15: 0){_LLBF: _tmp65=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp33)->f1;goto _LL84;}else{goto
_LL85;}_LL85: if((int)_tmp33 == 2){goto _LL86;}else{goto _LL87;}_LL87: if((
unsigned int)_tmp33 > 3?*((int*)_tmp33)== 17: 0){goto _LL88;}else{goto _LL89;}_LL89:
if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 19: 0){goto _LL8A;}else{goto _LL8B;}
_LL8B: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 18: 0){goto _LL8C;}else{goto
_LL4A;}_LL4C: goto _LL4E;_LL4E: goto _LL50;_LL50: return;_LL52: Cyc_Dump_dump_alpha(
_tag_arr("void",sizeof(unsigned char),5));return;_LL54: Cyc_Dump_dump_alphastr(
_tmp34->name);return;_LL56: Cyc_Dump_dump_char((int)'%');if(_tmp37 == 0){Cyc_Dump_dump_char((
int)'?');}else{Cyc_Dump_dumpkind((void*)_tmp37->v);}Cyc_Dump_dump_symbol((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp67;_tmp67.tag=1;_tmp67.f1=(int)((
unsigned int)_tmp35);{void*_tmp66[1]={& _tmp67};Cyc_Std_aprintf(_tag_arr("(%d)",
sizeof(unsigned char),5),_tag_arr(_tmp66,sizeof(void*),1));}}));return;_LL58: Cyc_Dump_dumpntyp(
_tmp3B);return;_LL5A:{void*_tmp68=_tmp40;struct Cyc_Absyn_UnknownTunionInfo _tmp69;
int _tmp6A;struct _tuple1*_tmp6B;struct Cyc_Absyn_Tuniondecl**_tmp6C;struct Cyc_Absyn_Tuniondecl*
_tmp6D;struct Cyc_Absyn_Tuniondecl _tmp6E;int _tmp6F;struct _tuple1*_tmp70;_LLC1: if(*((
int*)_tmp68)== 0){_LLC5: _tmp69=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp68)->f1;
_LLC7: _tmp6B=_tmp69.name;goto _LLC6;_LLC6: _tmp6A=_tmp69.is_xtunion;goto _LLC2;}
else{goto _LLC3;}_LLC3: if(*((int*)_tmp68)== 1){_LLC8: _tmp6C=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp68)->f1;_tmp6D=*_tmp6C;_tmp6E=*_tmp6D;_LLCA: _tmp70=_tmp6E.name;goto _LLC9;
_LLC9: _tmp6F=_tmp6E.is_xtunion;goto _LLC4;}else{goto _LLC0;}_LLC2: _tmp70=_tmp6B;
_tmp6F=_tmp6A;goto _LLC4;_LLC4: if(_tmp6F){Cyc_Dump_dump_alpha(_tag_arr("xtunion",
sizeof(unsigned char),8));}else{Cyc_Dump_dump_alpha(_tag_arr("tunion",sizeof(
unsigned char),7));}{void*_tmp71=Cyc_Tcutil_compress(_tmp3E);_LLCC: if((int)
_tmp71 == 2){goto _LLCD;}else{goto _LLCE;}_LLCE: goto _LLCF;_LLCD: goto _LLCB;_LLCF: Cyc_Dump_dumptyp(
_tmp3E);goto _LLCB;_LLCB:;}Cyc_Dump_dumpqvar(_tmp70);Cyc_Dump_dumptps(_tmp3F);
goto _LLC0;_LLC0:;}goto _LL4A;_LL5C:{void*_tmp72=_tmp43;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp73;int _tmp74;struct _tuple1*_tmp75;struct _tuple1*_tmp76;struct Cyc_Absyn_Tunionfield*
_tmp77;struct Cyc_Absyn_Tunionfield _tmp78;struct _tuple1*_tmp79;struct Cyc_Absyn_Tuniondecl*
_tmp7A;struct Cyc_Absyn_Tuniondecl _tmp7B;int _tmp7C;struct _tuple1*_tmp7D;_LLD1: if(*((
int*)_tmp72)== 0){_LLD5: _tmp73=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp72)->f1;_LLD8: _tmp76=_tmp73.tunion_name;goto _LLD7;_LLD7: _tmp75=_tmp73.field_name;
goto _LLD6;_LLD6: _tmp74=_tmp73.is_xtunion;goto _LLD2;}else{goto _LLD3;}_LLD3: if(*((
int*)_tmp72)== 1){_LLDB: _tmp7A=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp72)->f1;
_tmp7B=*_tmp7A;_LLDD: _tmp7D=_tmp7B.name;goto _LLDC;_LLDC: _tmp7C=_tmp7B.is_xtunion;
goto _LLD9;_LLD9: _tmp77=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp72)->f2;
_tmp78=*_tmp77;_LLDA: _tmp79=_tmp78.name;goto _LLD4;}else{goto _LLD0;}_LLD2: _tmp7D=
_tmp76;_tmp7C=_tmp74;_tmp79=_tmp75;goto _LLD4;_LLD4: if(_tmp7C){Cyc_Dump_dump_alpha(
_tag_arr("xtunion",sizeof(unsigned char),8));}else{Cyc_Dump_dump_alpha(_tag_arr("tunion",
sizeof(unsigned char),7));}Cyc_Dump_dumpqvar(_tmp7D);Cyc_Dump_dump_char((int)'.');
Cyc_Dump_dumpqvar(_tmp79);Cyc_Dump_dumptps(_tmp42);goto _LLD0;_LLD0:;}goto _LL4A;
_LL5E: Cyc_Dump_dump_alpha(_tag_arr("enum",sizeof(unsigned char),5));Cyc_Dump_dumpqvar(
_tmp44);return;_LL60: Cyc_Dump_dump_alpha(_tag_arr("enum",sizeof(unsigned char),5));
Cyc_Dump_dump_symbol(_tag_arr(" ",sizeof(unsigned char),2));Cyc_Dump_dump_lb();
Cyc_Dump_dump_symbol(_tag_arr("\\\\\n~~",sizeof(unsigned char),6));Cyc_Dump_dumpenumfields(
_tmp45);Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",sizeof(unsigned char),4));Cyc_Dump_dump_rb();
return;_LL62: Cyc_Dump_dump_alpha(_tag_arr("int",sizeof(unsigned char),4));
return;_LL64: Cyc_Dump_dump_alpha(_tag_arr("signed char",sizeof(unsigned char),12));
return;_LL66: Cyc_Dump_dump_alpha(_tag_arr("unsigned char",sizeof(unsigned char),
14));return;_LL68: Cyc_Dump_dump_alpha(_tag_arr("short",sizeof(unsigned char),6));
return;_LL6A: Cyc_Dump_dump_alpha(_tag_arr("unsigned short",sizeof(unsigned char),
15));return;_LL6C: Cyc_Dump_dump_alpha(_tag_arr("unsigned int",sizeof(
unsigned char),13));return;_LL6E: Cyc_Dump_dump_alpha(_tag_arr("__int64",sizeof(
unsigned char),8));return;_LL70: Cyc_Dump_dump_alpha(_tag_arr("long long",sizeof(
unsigned char),10));return;_LL72: Cyc_Dump_dump_alpha(_tag_arr("unsigned __int64",
sizeof(unsigned char),17));return;_LL74: Cyc_Dump_dump_alpha(_tag_arr("unsigned long long",
sizeof(unsigned char),19));return;_LL76: Cyc_Dump_dump_alpha(_tag_arr("float",
sizeof(unsigned char),6));return;_LL78: if(_tmp5A){Cyc_Dump_dump_alpha(_tag_arr("long double",
sizeof(unsigned char),12));}else{Cyc_Dump_dump_alpha(_tag_arr("double",sizeof(
unsigned char),7));}return;_LL7A: Cyc_Dump_dump_alpha(_tag_arr("sizeof_t<",
sizeof(unsigned char),10));Cyc_Dump_dumptyp(_tmp5B);Cyc_Dump_dump_alpha(_tag_arr(">",
sizeof(unsigned char),2));return;_LL7C: Cyc_Dump_dump_symbol(_tag_arr("\\$",
sizeof(unsigned char),3));Cyc_Dump_dumpargs(_tmp5C);return;_LL7E: {struct _tuple1*
_tmp7F;void*_tmp80;struct _tuple4 _tmp7E=Cyc_Absyn_aggr_kinded_name(_tmp5F);_LLE0:
_tmp80=_tmp7E.f1;goto _LLDF;_LLDF: _tmp7F=_tmp7E.f2;goto _LLDE;_LLDE: Cyc_Dump_dump_alpha(
_tmp80 == (void*)1? _tag_arr("union",sizeof(unsigned char),6): _tag_arr("struct",
sizeof(unsigned char),7));Cyc_Dump_dumpqvar(_tmp7F);Cyc_Dump_dumptps(_tmp5E);
return;}_LL80: Cyc_Dump_dump_alpha(_tmp61 == (void*)1? _tag_arr("union",sizeof(
unsigned char),6): _tag_arr("struct",sizeof(unsigned char),7));Cyc_Dump_dump_lb();
Cyc_Dump_dumpaggrfields(_tmp60);Cyc_Dump_dump_rb();return;_LL82:(Cyc_Dump_dumpqvar(
_tmp64),Cyc_Dump_dumptps(_tmp63));return;_LL84: Cyc_Dump_dumprgn(_tmp65);return;
_LL86: goto _LL88;_LL88: goto _LL8A;_LL8A: goto _LL8C;_LL8C: return;_LL4A:;}void Cyc_Dump_dumpvaropt(
struct Cyc_Core_Opt*vo){if(vo != 0){Cyc_Dump_dump_alphastr((struct _tagged_arr*)vo->v);}}
void Cyc_Dump_dumpfunarg(struct _tuple2*t){((void(*)(struct Cyc_Absyn_Tqual,void*,
void(*f)(struct Cyc_Core_Opt*),struct Cyc_Core_Opt*))Cyc_Dump_dumptqtd)((*t).f2,(*
t).f3,Cyc_Dump_dumpvaropt,(*t).f1);}void Cyc_Dump_dump_rgncmp(struct _tuple0*cmp){
struct _tuple0 _tmp82;void*_tmp83;void*_tmp84;struct _tuple0*_tmp81=cmp;_tmp82=*
_tmp81;_LLE3: _tmp84=_tmp82.f1;goto _LLE2;_LLE2: _tmp83=_tmp82.f2;goto _LLE1;_LLE1:
Cyc_Dump_dumptyp(_tmp84);Cyc_Dump_dump_char((int)'<');Cyc_Dump_dumptyp(_tmp83);}
void Cyc_Dump_dump_rgnpo(struct Cyc_List_List*rgn_po){((void(*)(void(*f)(struct
_tuple0*),struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Dump_dump_sep)(Cyc_Dump_dump_rgncmp,
rgn_po,_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Dump_dumpfunargs(struct
Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){Cyc_Dump_dump_char((int)'(');for(0;args != 0;
args=args->tl){Cyc_Dump_dumpfunarg((struct _tuple2*)args->hd);if((args->tl != 0? 1:
c_varargs)? 1: cyc_varargs != 0){Cyc_Dump_dump_symbol(_tag_arr(", ",sizeof(
unsigned char),3));}}if(c_varargs){Cyc_Dump_dump_symbol(_tag_arr("...",sizeof(
unsigned char),4));}else{if(cyc_varargs != 0){struct _tuple2*_tmp85=({struct
_tuple2*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->f1=cyc_varargs->name;_tmp86->f2=
cyc_varargs->tq;_tmp86->f3=(void*)cyc_varargs->type;_tmp86;});Cyc_Dump_dump_symbol(
_tag_arr("...",sizeof(unsigned char),4));if(cyc_varargs->inject){Cyc_Dump_dump_alpha(
_tag_arr(" inject",sizeof(unsigned char),8));}Cyc_Dump_dumpfunarg(_tmp85);}}if(
effopt != 0){Cyc_Dump_dump_semi();Cyc_Dump_dumpeff((void*)effopt->v);}if(rgn_po != 
0){Cyc_Dump_dump_char((int)':');Cyc_Dump_dump_rgnpo(rgn_po);}Cyc_Dump_dump_char((
int)')');}void Cyc_Dump_dumptyp(void*t){((void(*)(struct Cyc_Absyn_Tqual,void*,
void(*f)(int),int))Cyc_Dump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp87;_tmp87.q_const=
0;_tmp87.q_volatile=0;_tmp87.q_restrict=0;_tmp87;}),t,(void(*)(int x))Cyc_Dump_ignore,
0);}void Cyc_Dump_dumpdesignator(void*d){void*_tmp88=d;struct Cyc_Absyn_Exp*_tmp89;
struct _tagged_arr*_tmp8A;_LLE5: if(*((int*)_tmp88)== 0){_LLE9: _tmp89=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp88)->f1;goto _LLE6;}else{goto _LLE7;}_LLE7: if(*((int*)_tmp88)== 1){_LLEA:
_tmp8A=((struct Cyc_Absyn_FieldName_struct*)_tmp88)->f1;goto _LLE8;}else{goto _LLE4;}
_LLE6: Cyc_Dump_dump_symbol(_tag_arr(".[",sizeof(unsigned char),3));Cyc_Dump_dumpexp(
_tmp89);Cyc_Dump_dump_char((int)']');goto _LLE4;_LLE8: Cyc_Dump_dump_char((int)'.');
Cyc_Dump_dump_alpha(*_tmp8A);goto _LLE4;_LLE4:;}struct _tuple14{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};void Cyc_Dump_dumpde(struct _tuple14*de){Cyc_Dump_egroup(
Cyc_Dump_dumpdesignator,(*de).f1,_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",
sizeof(unsigned char),2),_tag_arr("=",sizeof(unsigned char),2));Cyc_Dump_dumpexp((*
de).f2);}void Cyc_Dump_dumpexps_prec(int inprec,struct Cyc_List_List*es){((void(*)(
void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group_c)(
Cyc_Dump_dumpexp_prec,inprec,es,_tag_arr("",sizeof(unsigned char),1),_tag_arr("",
sizeof(unsigned char),1),_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Dump_dumpexp_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= 
myprec){Cyc_Dump_dump_char((int)'(');}{void*_tmp8B=(void*)e->r;void*_tmp8C;
unsigned char _tmp8D;void*_tmp8E;void*_tmp8F;short _tmp90;void*_tmp91;void*_tmp92;
int _tmp93;void*_tmp94;void*_tmp95;int _tmp96;void*_tmp97;void*_tmp98;long long
_tmp99;void*_tmp9A;void*_tmp9B;struct _tagged_arr _tmp9C;void*_tmp9D;void*_tmp9E;
struct _tagged_arr _tmp9F;struct _tuple1*_tmpA0;struct _tuple1*_tmpA1;struct Cyc_List_List*
_tmpA2;void*_tmpA3;struct Cyc_Absyn_Exp*_tmpA4;struct Cyc_Core_Opt*_tmpA5;struct
Cyc_Absyn_Exp*_tmpA6;void*_tmpA7;struct Cyc_Absyn_Exp*_tmpA8;void*_tmpA9;struct
Cyc_Absyn_Exp*_tmpAA;void*_tmpAB;struct Cyc_Absyn_Exp*_tmpAC;void*_tmpAD;struct
Cyc_Absyn_Exp*_tmpAE;struct Cyc_Absyn_Exp*_tmpAF;struct Cyc_Absyn_Exp*_tmpB0;
struct Cyc_Absyn_Exp*_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_Exp*_tmpB3;
struct Cyc_List_List*_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;struct Cyc_List_List*_tmpB6;
struct Cyc_Absyn_Exp*_tmpB7;struct Cyc_Absyn_Exp*_tmpB8;struct Cyc_Absyn_Exp*_tmpB9;
struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Exp*_tmpBB;void*_tmpBC;struct Cyc_Absyn_Exp*
_tmpBD;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_Absyn_Exp*_tmpBF;void*_tmpC0;struct
Cyc_Absyn_Exp*_tmpC1;void*_tmpC2;struct _tagged_arr*_tmpC3;void*_tmpC4;void*
_tmpC5;unsigned int _tmpC6;void*_tmpC7;void*_tmpC8;struct Cyc_List_List*_tmpC9;
struct Cyc_Absyn_Exp*_tmpCA;struct _tagged_arr*_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;
struct _tagged_arr*_tmpCD;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;
struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_List_List*_tmpD1;struct Cyc_List_List*_tmpD2;
struct _tuple2*_tmpD3;struct Cyc_List_List*_tmpD4;struct Cyc_Absyn_Exp*_tmpD5;
struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Vardecl*_tmpD7;struct Cyc_List_List*
_tmpD8;struct _tuple1*_tmpD9;struct Cyc_List_List*_tmpDA;struct Cyc_Absyn_Tunionfield*
_tmpDB;struct Cyc_List_List*_tmpDC;struct _tuple1*_tmpDD;struct _tuple1*_tmpDE;
struct Cyc_Absyn_MallocInfo _tmpDF;struct Cyc_Absyn_Exp*_tmpE0;void**_tmpE1;struct
Cyc_Absyn_Exp*_tmpE2;int _tmpE3;struct Cyc_List_List*_tmpE4;struct Cyc_Core_Opt*
_tmpE5;struct Cyc_Absyn_Stmt*_tmpE6;struct Cyc_Absyn_Fndecl*_tmpE7;struct Cyc_Absyn_Exp*
_tmpE8;_LLEC: if(*((int*)_tmp8B)== 0){_LL14E: _tmp8C=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8B)->f1;if((unsigned int)_tmp8C > 1?*((int*)_tmp8C)== 0: 0){_LL150: _tmp8E=(
void*)((struct Cyc_Absyn_Char_c_struct*)_tmp8C)->f1;goto _LL14F;_LL14F: _tmp8D=((
struct Cyc_Absyn_Char_c_struct*)_tmp8C)->f2;goto _LLED;}else{goto _LLEE;}}else{goto
_LLEE;}_LLEE: if(*((int*)_tmp8B)== 0){_LL151: _tmp8F=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8B)->f1;if((unsigned int)_tmp8F > 1?*((int*)_tmp8F)== 1: 0){_LL153: _tmp91=(
void*)((struct Cyc_Absyn_Short_c_struct*)_tmp8F)->f1;goto _LL152;_LL152: _tmp90=((
struct Cyc_Absyn_Short_c_struct*)_tmp8F)->f2;goto _LLEF;}else{goto _LLF0;}}else{
goto _LLF0;}_LLF0: if(*((int*)_tmp8B)== 0){_LL154: _tmp92=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8B)->f1;if((unsigned int)_tmp92 > 1?*((int*)_tmp92)== 2: 0){_LL156: _tmp94=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp92)->f1;if((int)_tmp94 == 0){goto _LL155;}
else{goto _LLF2;}_LL155: _tmp93=((struct Cyc_Absyn_Int_c_struct*)_tmp92)->f2;goto
_LLF1;}else{goto _LLF2;}}else{goto _LLF2;}_LLF2: if(*((int*)_tmp8B)== 0){_LL157:
_tmp95=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp8B)->f1;if((unsigned int)
_tmp95 > 1?*((int*)_tmp95)== 2: 0){_LL159: _tmp97=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp95)->f1;if((int)_tmp97 == 1){goto _LL158;}else{goto _LLF4;}_LL158: _tmp96=((
struct Cyc_Absyn_Int_c_struct*)_tmp95)->f2;goto _LLF3;}else{goto _LLF4;}}else{goto
_LLF4;}_LLF4: if(*((int*)_tmp8B)== 0){_LL15A: _tmp98=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8B)->f1;if((unsigned int)_tmp98 > 1?*((int*)_tmp98)== 3: 0){_LL15C: _tmp9A=(
void*)((struct Cyc_Absyn_LongLong_c_struct*)_tmp98)->f1;goto _LL15B;_LL15B: _tmp99=((
struct Cyc_Absyn_LongLong_c_struct*)_tmp98)->f2;goto _LLF5;}else{goto _LLF6;}}else{
goto _LLF6;}_LLF6: if(*((int*)_tmp8B)== 0){_LL15D: _tmp9B=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8B)->f1;if((unsigned int)_tmp9B > 1?*((int*)_tmp9B)== 4: 0){_LL15E: _tmp9C=((
struct Cyc_Absyn_Float_c_struct*)_tmp9B)->f1;goto _LLF7;}else{goto _LLF8;}}else{
goto _LLF8;}_LLF8: if(*((int*)_tmp8B)== 0){_LL15F: _tmp9D=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8B)->f1;if((int)_tmp9D == 0){goto _LLF9;}else{goto _LLFA;}}else{goto _LLFA;}
_LLFA: if(*((int*)_tmp8B)== 0){_LL160: _tmp9E=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8B)->f1;if((unsigned int)_tmp9E > 1?*((int*)_tmp9E)== 5: 0){_LL161: _tmp9F=((
struct Cyc_Absyn_String_c_struct*)_tmp9E)->f1;goto _LLFB;}else{goto _LLFC;}}else{
goto _LLFC;}_LLFC: if(*((int*)_tmp8B)== 2){_LL162: _tmpA0=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp8B)->f1;goto _LLFD;}else{goto _LLFE;}_LLFE: if(*((int*)_tmp8B)== 1){_LL163:
_tmpA1=((struct Cyc_Absyn_Var_e_struct*)_tmp8B)->f1;goto _LLFF;}else{goto _LL100;}
_LL100: if(*((int*)_tmp8B)== 3){_LL165: _tmpA3=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp8B)->f1;goto _LL164;_LL164: _tmpA2=((struct Cyc_Absyn_Primop_e_struct*)_tmp8B)->f2;
goto _LL101;}else{goto _LL102;}_LL102: if(*((int*)_tmp8B)== 4){_LL168: _tmpA6=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp8B)->f1;goto _LL167;_LL167: _tmpA5=((struct
Cyc_Absyn_AssignOp_e_struct*)_tmp8B)->f2;goto _LL166;_LL166: _tmpA4=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp8B)->f3;goto _LL103;}else{goto _LL104;}_LL104: if(*((int*)_tmp8B)== 5){_LL16A:
_tmpA8=((struct Cyc_Absyn_Increment_e_struct*)_tmp8B)->f1;goto _LL169;_LL169:
_tmpA7=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp8B)->f2;if((int)_tmpA7 == 
0){goto _LL105;}else{goto _LL106;}}else{goto _LL106;}_LL106: if(*((int*)_tmp8B)== 5){
_LL16C: _tmpAA=((struct Cyc_Absyn_Increment_e_struct*)_tmp8B)->f1;goto _LL16B;
_LL16B: _tmpA9=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp8B)->f2;if((int)
_tmpA9 == 2){goto _LL107;}else{goto _LL108;}}else{goto _LL108;}_LL108: if(*((int*)
_tmp8B)== 5){_LL16E: _tmpAC=((struct Cyc_Absyn_Increment_e_struct*)_tmp8B)->f1;
goto _LL16D;_LL16D: _tmpAB=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp8B)->f2;
if((int)_tmpAB == 1){goto _LL109;}else{goto _LL10A;}}else{goto _LL10A;}_LL10A: if(*((
int*)_tmp8B)== 5){_LL170: _tmpAE=((struct Cyc_Absyn_Increment_e_struct*)_tmp8B)->f1;
goto _LL16F;_LL16F: _tmpAD=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp8B)->f2;
if((int)_tmpAD == 3){goto _LL10B;}else{goto _LL10C;}}else{goto _LL10C;}_LL10C: if(*((
int*)_tmp8B)== 6){_LL173: _tmpB1=((struct Cyc_Absyn_Conditional_e_struct*)_tmp8B)->f1;
goto _LL172;_LL172: _tmpB0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp8B)->f2;
goto _LL171;_LL171: _tmpAF=((struct Cyc_Absyn_Conditional_e_struct*)_tmp8B)->f3;
goto _LL10D;}else{goto _LL10E;}_LL10E: if(*((int*)_tmp8B)== 7){_LL175: _tmpB3=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp8B)->f1;goto _LL174;_LL174: _tmpB2=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp8B)->f2;goto _LL10F;}else{goto _LL110;}_LL110: if(*((
int*)_tmp8B)== 8){_LL177: _tmpB5=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp8B)->f1;
goto _LL176;_LL176: _tmpB4=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp8B)->f2;
goto _LL111;}else{goto _LL112;}_LL112: if(*((int*)_tmp8B)== 9){_LL179: _tmpB7=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp8B)->f1;goto _LL178;_LL178: _tmpB6=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp8B)->f2;goto _LL113;}else{goto _LL114;}_LL114: if(*((
int*)_tmp8B)== 10){_LL17A: _tmpB8=((struct Cyc_Absyn_Throw_e_struct*)_tmp8B)->f1;
goto _LL115;}else{goto _LL116;}_LL116: if(*((int*)_tmp8B)== 11){_LL17B: _tmpB9=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp8B)->f1;goto _LL117;}else{goto _LL118;}
_LL118: if(*((int*)_tmp8B)== 12){_LL17C: _tmpBA=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp8B)->f1;goto _LL119;}else{goto _LL11A;}_LL11A: if(*((int*)_tmp8B)== 13){_LL17E:
_tmpBC=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp8B)->f1;goto _LL17D;_LL17D:
_tmpBB=((struct Cyc_Absyn_Cast_e_struct*)_tmp8B)->f2;goto _LL11B;}else{goto _LL11C;}
_LL11C: if(*((int*)_tmp8B)== 14){_LL17F: _tmpBD=((struct Cyc_Absyn_Address_e_struct*)
_tmp8B)->f1;goto _LL11D;}else{goto _LL11E;}_LL11E: if(*((int*)_tmp8B)== 15){_LL181:
_tmpBF=((struct Cyc_Absyn_New_e_struct*)_tmp8B)->f1;goto _LL180;_LL180: _tmpBE=((
struct Cyc_Absyn_New_e_struct*)_tmp8B)->f2;goto _LL11F;}else{goto _LL120;}_LL120:
if(*((int*)_tmp8B)== 16){_LL182: _tmpC0=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp8B)->f1;goto _LL121;}else{goto _LL122;}_LL122: if(*((int*)_tmp8B)== 17){_LL183:
_tmpC1=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp8B)->f1;goto _LL123;}else{goto
_LL124;}_LL124: if(*((int*)_tmp8B)== 18){_LL186: _tmpC4=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp8B)->f1;goto _LL184;_LL184: _tmpC2=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp8B)->f2;if(*((int*)_tmpC2)== 0){_LL185: _tmpC3=((struct Cyc_Absyn_StructField_struct*)
_tmpC2)->f1;goto _LL125;}else{goto _LL126;}}else{goto _LL126;}_LL126: if(*((int*)
_tmp8B)== 18){_LL189: _tmpC7=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp8B)->f1;
goto _LL187;_LL187: _tmpC5=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp8B)->f2;
if(*((int*)_tmpC5)== 1){_LL188: _tmpC6=((struct Cyc_Absyn_TupleIndex_struct*)
_tmpC5)->f1;goto _LL127;}else{goto _LL128;}}else{goto _LL128;}_LL128: if(*((int*)
_tmp8B)== 19){_LL18B: _tmpC9=((struct Cyc_Absyn_Gentyp_e_struct*)_tmp8B)->f1;goto
_LL18A;_LL18A: _tmpC8=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp8B)->f2;goto
_LL129;}else{goto _LL12A;}_LL12A: if(*((int*)_tmp8B)== 20){_LL18C: _tmpCA=((struct
Cyc_Absyn_Deref_e_struct*)_tmp8B)->f1;goto _LL12B;}else{goto _LL12C;}_LL12C: if(*((
int*)_tmp8B)== 21){_LL18E: _tmpCC=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp8B)->f1;
goto _LL18D;_LL18D: _tmpCB=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp8B)->f2;goto
_LL12D;}else{goto _LL12E;}_LL12E: if(*((int*)_tmp8B)== 22){_LL190: _tmpCE=((struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp8B)->f1;goto _LL18F;_LL18F: _tmpCD=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp8B)->f2;goto _LL12F;}else{goto _LL130;}_LL130: if(*((int*)_tmp8B)== 23){_LL192:
_tmpD0=((struct Cyc_Absyn_Subscript_e_struct*)_tmp8B)->f1;goto _LL191;_LL191:
_tmpCF=((struct Cyc_Absyn_Subscript_e_struct*)_tmp8B)->f2;goto _LL131;}else{goto
_LL132;}_LL132: if(*((int*)_tmp8B)== 24){_LL193: _tmpD1=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp8B)->f1;goto _LL133;}else{goto _LL134;}_LL134: if(*((int*)_tmp8B)== 25){_LL195:
_tmpD3=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp8B)->f1;goto _LL194;_LL194:
_tmpD2=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp8B)->f2;goto _LL135;}else{goto
_LL136;}_LL136: if(*((int*)_tmp8B)== 26){_LL196: _tmpD4=((struct Cyc_Absyn_Array_e_struct*)
_tmp8B)->f1;goto _LL137;}else{goto _LL138;}_LL138: if(*((int*)_tmp8B)== 27){_LL199:
_tmpD7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp8B)->f1;goto _LL198;_LL198:
_tmpD6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp8B)->f2;goto _LL197;_LL197:
_tmpD5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp8B)->f3;goto _LL139;}else{
goto _LL13A;}_LL13A: if(*((int*)_tmp8B)== 28){_LL19B: _tmpD9=((struct Cyc_Absyn_Struct_e_struct*)
_tmp8B)->f1;goto _LL19A;_LL19A: _tmpD8=((struct Cyc_Absyn_Struct_e_struct*)_tmp8B)->f2;
goto _LL13B;}else{goto _LL13C;}_LL13C: if(*((int*)_tmp8B)== 29){_LL19C: _tmpDA=((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp8B)->f2;goto _LL13D;}else{goto _LL13E;}
_LL13E: if(*((int*)_tmp8B)== 30){_LL19E: _tmpDC=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp8B)->f1;goto _LL19D;_LL19D: _tmpDB=((struct Cyc_Absyn_Tunion_e_struct*)_tmp8B)->f3;
goto _LL13F;}else{goto _LL140;}_LL140: if(*((int*)_tmp8B)== 31){_LL19F: _tmpDD=((
struct Cyc_Absyn_Enum_e_struct*)_tmp8B)->f1;goto _LL141;}else{goto _LL142;}_LL142:
if(*((int*)_tmp8B)== 32){_LL1A0: _tmpDE=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp8B)->f1;goto _LL143;}else{goto _LL144;}_LL144: if(*((int*)_tmp8B)== 33){_LL1A1:
_tmpDF=((struct Cyc_Absyn_Malloc_e_struct*)_tmp8B)->f1;_LL1A5: _tmpE3=_tmpDF.is_calloc;
goto _LL1A4;_LL1A4: _tmpE2=_tmpDF.rgn;goto _LL1A3;_LL1A3: _tmpE1=_tmpDF.elt_type;
goto _LL1A2;_LL1A2: _tmpE0=_tmpDF.num_elts;goto _LL145;}else{goto _LL146;}_LL146: if(*((
int*)_tmp8B)== 34){_LL1A7: _tmpE5=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp8B)->f1;goto _LL1A6;_LL1A6: _tmpE4=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp8B)->f2;goto _LL147;}else{goto _LL148;}_LL148: if(*((int*)_tmp8B)== 35){_LL1A8:
_tmpE6=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp8B)->f1;goto _LL149;}else{goto
_LL14A;}_LL14A: if(*((int*)_tmp8B)== 36){_LL1A9: _tmpE7=((struct Cyc_Absyn_Codegen_e_struct*)
_tmp8B)->f1;goto _LL14B;}else{goto _LL14C;}_LL14C: if(*((int*)_tmp8B)== 37){_LL1AA:
_tmpE8=((struct Cyc_Absyn_Fill_e_struct*)_tmp8B)->f1;goto _LL14D;}else{goto _LLEB;}
_LLED: Cyc_Dump_dump_char((int)'\'');Cyc_Dump_dump_alpha(Cyc_Absynpp_char_escape(
_tmp8D));Cyc_Dump_dump_char((int)'\'');goto _LLEB;_LLEF: Cyc_Dump_dump_alpha((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpEA;_tmpEA.tag=1;_tmpEA.f1=(
int)((unsigned int)((int)_tmp90));{void*_tmpE9[1]={& _tmpEA};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmpE9,sizeof(void*),1));}}));
goto _LLEB;_LLF1: Cyc_Dump_dump_alpha((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmpEC;_tmpEC.tag=1;_tmpEC.f1=(int)((unsigned int)_tmp93);{void*_tmpEB[1]={&
_tmpEC};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmpEB,
sizeof(void*),1));}}));goto _LLEB;_LLF3: Cyc_Dump_dump_alpha((struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmpEE;_tmpEE.tag=1;_tmpEE.f1=(int)((unsigned int)
_tmp96);{void*_tmpED[1]={& _tmpEE};Cyc_Std_aprintf(_tag_arr("%du",sizeof(
unsigned char),4),_tag_arr(_tmpED,sizeof(void*),1));}}));goto _LLEB;_LLF5: Cyc_Dump_dump_alpha(
_tag_arr("<<FIX LONG LONG CONSTANT>>",sizeof(unsigned char),27));goto _LLEB;_LLF7:
Cyc_Dump_dump_alpha(_tmp9C);goto _LLEB;_LLF9: Cyc_Dump_dump_alpha(_tag_arr("NULL",
sizeof(unsigned char),5));goto _LLEB;_LLFB: Cyc_Dump_dump_char((int)'"');Cyc_Dump_dump_alpha(
Cyc_Absynpp_string_escape(_tmp9F));Cyc_Dump_dump_char((int)'"');goto _LLEB;_LLFD:
_tmpA1=_tmpA0;goto _LLFF;_LLFF: Cyc_Dump_dumpqvar(_tmpA1);goto _LLEB;_LL101: {
struct _tagged_arr _tmpEF=Cyc_Absynpp_prim2str(_tmpA3);switch(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpA2)){case 1: _LL1AB: if(_tmpA3 == (void*)19){Cyc_Dump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpA2))->hd);
Cyc_Dump_dump_char((int)'.');Cyc_Dump_dump_alpha(_tag_arr("size",sizeof(
unsigned char),5));}else{Cyc_Dump_dump_symbol(_tmpEF);Cyc_Dump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpA2))->hd);}
break;case 2: _LL1AC: Cyc_Dump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmpA2))->hd);Cyc_Dump_dump_symbol(_tmpEF);Cyc_Dump_dump_char((
int)' ');Cyc_Dump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpA2->tl))->hd);break;default: _LL1AD:(int)_throw((void*)({struct
Cyc_Core_Failure_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_Core_Failure_struct
_tmpF1;_tmpF1.tag=Cyc_Core_Failure;_tmpF1.f1=_tag_arr("Absyndump -- Bad number of arguments to primop",
sizeof(unsigned char),47);_tmpF1;});_tmpF0;}));}goto _LLEB;}_LL103: Cyc_Dump_dumpexp_prec(
myprec,_tmpA6);if(_tmpA5 != 0){Cyc_Dump_dump_symbol(Cyc_Absynpp_prim2str((void*)
_tmpA5->v));}Cyc_Dump_dump_symbol(_tag_arr("=",sizeof(unsigned char),2));Cyc_Dump_dumpexp_prec(
myprec,_tmpA4);goto _LLEB;_LL105: Cyc_Dump_dump_symbol(_tag_arr("++",sizeof(
unsigned char),3));Cyc_Dump_dumpexp_prec(myprec,_tmpA8);goto _LLEB;_LL107: Cyc_Dump_dump_symbol(
_tag_arr("--",sizeof(unsigned char),3));Cyc_Dump_dumpexp_prec(myprec,_tmpAA);
goto _LLEB;_LL109: Cyc_Dump_dumpexp_prec(myprec,_tmpAC);Cyc_Dump_dump_symbol(
_tag_arr("++",sizeof(unsigned char),3));goto _LLEB;_LL10B: Cyc_Dump_dumpexp_prec(
myprec,_tmpAE);Cyc_Dump_dump_symbol(_tag_arr("--",sizeof(unsigned char),3));goto
_LLEB;_LL10D: Cyc_Dump_dumpexp_prec(myprec,_tmpB1);Cyc_Dump_dump_char((int)'?');
Cyc_Dump_dumpexp_prec(0,_tmpB0);Cyc_Dump_dump_char((int)':');Cyc_Dump_dumpexp_prec(
myprec,_tmpAF);goto _LLEB;_LL10F: Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumpexp_prec(
myprec,_tmpB3);Cyc_Dump_dump_char((int)',');Cyc_Dump_dumpexp_prec(myprec,_tmpB2);
Cyc_Dump_dump_char((int)')');goto _LLEB;_LL111: _tmpB7=_tmpB5;_tmpB6=_tmpB4;goto
_LL113;_LL113: Cyc_Dump_dumpexp_prec(myprec,_tmpB7);Cyc_Dump_dump_char((int)'(');
Cyc_Dump_dumpexps_prec(20,_tmpB6);Cyc_Dump_dump_char((int)')');goto _LLEB;_LL115:
Cyc_Dump_dump_alpha(_tag_arr("throw",sizeof(unsigned char),6));Cyc_Dump_dumpexp_prec(
myprec,_tmpB8);goto _LLEB;_LL117: _tmpBA=_tmpB9;goto _LL119;_LL119: Cyc_Dump_dumpexp_prec(
inprec,_tmpBA);goto _LLEB;_LL11B: Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(
_tmpBC);Cyc_Dump_dump_char((int)')');Cyc_Dump_dumpexp_prec(myprec,_tmpBB);goto
_LLEB;_LL11D: Cyc_Dump_dump_char((int)'&');Cyc_Dump_dumpexp_prec(myprec,_tmpBD);
goto _LLEB;_LL11F: Cyc_Dump_dump_alpha(_tag_arr("new",sizeof(unsigned char),4));
Cyc_Dump_dumpexp_prec(myprec,_tmpBE);goto _LLEB;_LL121: Cyc_Dump_dump_alpha(
_tag_arr("sizeof",sizeof(unsigned char),7));Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(
_tmpC0);Cyc_Dump_dump_char((int)')');goto _LLEB;_LL123: Cyc_Dump_dump_alpha(
_tag_arr("sizeof",sizeof(unsigned char),7));Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumpexp_prec(
myprec,_tmpC1);Cyc_Dump_dump_char((int)')');goto _LLEB;_LL125: Cyc_Dump_dump_alpha(
_tag_arr("offsetof",sizeof(unsigned char),9));Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(
_tmpC4);Cyc_Dump_dump_char((int)',');Cyc_Dump_dump_alpha(*_tmpC3);Cyc_Dump_dump_char((
int)')');goto _LLEB;_LL127: Cyc_Dump_dump_alpha(_tag_arr("offsetof(",sizeof(
unsigned char),10));Cyc_Dump_dumptyp(_tmpC7);Cyc_Dump_dump_char((int)',');Cyc_Dump_dump_alpha((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpF3;_tmpF3.tag=1;_tmpF3.f1=(
int)_tmpC6;{void*_tmpF2[1]={& _tmpF3};Cyc_Std_aprintf(_tag_arr("%d",sizeof(
unsigned char),3),_tag_arr(_tmpF2,sizeof(void*),1));}}));Cyc_Dump_dump_char((int)')');
goto _LLEB;_LL129: Cyc_Dump_dump_alpha(_tag_arr("__gen",sizeof(unsigned char),6));
Cyc_Dump_dumptvars(_tmpC9);Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(_tmpC8);
Cyc_Dump_dump_char((int)')');goto _LLEB;_LL12B: Cyc_Dump_dump_char((int)'*');Cyc_Dump_dumpexp_prec(
myprec,_tmpCA);goto _LLEB;_LL12D: Cyc_Dump_dumpexp_prec(myprec,_tmpCC);Cyc_Dump_dump_char((
int)'.');Cyc_Dump_dump_alpha(*_tmpCB);goto _LLEB;_LL12F: Cyc_Dump_dumpexp_prec(
myprec,_tmpCE);Cyc_Dump_dump_symbol(_tag_arr("->",sizeof(unsigned char),3));Cyc_Dump_dump_alpha(*
_tmpCD);goto _LLEB;_LL131: Cyc_Dump_dumpexp_prec(myprec,_tmpD0);Cyc_Dump_dump_char((
int)'[');Cyc_Dump_dumpexp(_tmpCF);Cyc_Dump_dump_char((int)']');goto _LLEB;_LL133:
Cyc_Dump_dump_symbol(_tag_arr("\\$(",sizeof(unsigned char),4));Cyc_Dump_dumpexps_prec(
20,_tmpD1);Cyc_Dump_dump_char((int)')');goto _LLEB;_LL135: Cyc_Dump_dump_char((int)'(');
Cyc_Dump_dumptyp((*_tmpD3).f3);Cyc_Dump_dump_char((int)')');((void(*)(void(*f)(
struct _tuple14*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr
end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmpD2,_tag_arr("{\\lb}",
sizeof(unsigned char),6),_tag_arr("{\\rb}",sizeof(unsigned char),6),_tag_arr(",",
sizeof(unsigned char),2));goto _LLEB;_LL137:((void(*)(void(*f)(struct _tuple14*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmpD4,_tag_arr("{\\lb}",sizeof(
unsigned char),6),_tag_arr("{\\rb}",sizeof(unsigned char),6),_tag_arr(",",
sizeof(unsigned char),2));goto _LLEB;_LL139: Cyc_Dump_dump_alpha(_tag_arr("new",
sizeof(unsigned char),4));Cyc_Dump_dump_char((int)'{');Cyc_Dump_dump_alpha(
_tag_arr("for",sizeof(unsigned char),4));Cyc_Dump_dump_alphastr((*_tmpD7->name).f2);
Cyc_Dump_dump_char((int)'<');Cyc_Dump_dumpexp(_tmpD6);Cyc_Dump_dump_char((int)':');
Cyc_Dump_dumpexp(_tmpD5);Cyc_Dump_dump_char((int)'}');goto _LLEB;_LL13B: Cyc_Dump_dumpqvar(
_tmpD9);((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,
_tmpD8,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LLEB;_LL13D:((void(*)(void(*f)(
struct _tuple14*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr
end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmpDA,_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LLEB;_LL13F: Cyc_Dump_dumpqvar(_tmpDB->name);if(
_tmpDC != 0){((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(
Cyc_Dump_dumpexp,_tmpDC,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}goto _LLEB;
_LL141: Cyc_Dump_dumpqvar(_tmpDD);goto _LLEB;_LL143: Cyc_Dump_dumpqvar(_tmpDE);goto
_LLEB;_LL145: if(_tmpE2 != 0){if(_tmpE3){Cyc_Dump_dump_alpha(_tag_arr("rcalloc",
sizeof(unsigned char),8));}else{Cyc_Dump_dump_alpha(_tag_arr("rmalloc",sizeof(
unsigned char),8));}Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmpE2));Cyc_Dump_dump_symbol(_tag_arr(", ",sizeof(unsigned char),3));}
else{if(_tmpE3){Cyc_Dump_dump_alpha(_tag_arr("calloc",sizeof(unsigned char),7));}
else{Cyc_Dump_dump_alpha(_tag_arr("malloc",sizeof(unsigned char),7));}Cyc_Dump_dump_char((
int)'(');}Cyc_Dump_dumpexp(_tmpE0);if(_tmpE1 != 0){if(_tmpE3){Cyc_Dump_dump_symbol(
_tag_arr(", ",sizeof(unsigned char),3));}else{Cyc_Dump_dump_symbol(_tag_arr(" * ",
sizeof(unsigned char),4));}Cyc_Dump_dump_alpha(_tag_arr("sizeof",sizeof(
unsigned char),7));Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(*_tmpE1);Cyc_Dump_dump_symbol(
_tag_arr(")",sizeof(unsigned char),2));}Cyc_Dump_dump_symbol(_tag_arr(")",
sizeof(unsigned char),2));goto _LLEB;_LL147:((void(*)(void(*f)(struct _tuple14*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmpE4,_tag_arr("{",sizeof(
unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2));goto _LLEB;_LL149: goto _LLEB;_LL14B: goto _LLEB;_LL14D: goto _LLEB;
_LLEB:;}if(inprec >= myprec){Cyc_Dump_dump_char((int)')');}}void Cyc_Dump_dumpexp(
struct Cyc_Absyn_Exp*e){Cyc_Dump_dumpexp_prec(0,e);}void Cyc_Dump_dumptunionfield(
struct Cyc_Absyn_Tunionfield*ef){Cyc_Dump_dumpqvar(ef->name);if(ef->typs != 0){Cyc_Dump_dumpargs(
ef->typs);}}void Cyc_Dump_dumptunionfields(struct Cyc_List_List*fields){((void(*)(
void(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*l,struct _tagged_arr
sep))Cyc_Dump_dump_sep)(Cyc_Dump_dumptunionfield,fields,_tag_arr(",\\\\\n~~",
sizeof(unsigned char),7));}void Cyc_Dump_dumpenumfield(struct Cyc_Absyn_Enumfield*
ef){Cyc_Dump_dumpqvar(ef->name);if(ef->tag != 0){Cyc_Dump_dump_symbol(_tag_arr(" = ",
sizeof(unsigned char),4));Cyc_Dump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
void Cyc_Dump_dumpenumfields(struct Cyc_List_List*fields){((void(*)(void(*f)(
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Dump_dump_sep)(
Cyc_Dump_dumpenumfield,fields,_tag_arr(",\\\\\n~~",sizeof(unsigned char),7));}
void Cyc_Dump_dumpaggrfields(struct Cyc_List_List*fields){for(0;fields != 0;fields=
fields->tl){Cyc_Dump_dump_symbol(_tag_arr("~~",sizeof(unsigned char),3));{struct
Cyc_Absyn_Aggrfield _tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_Absyn_Exp*_tmpF7;
void*_tmpF8;struct Cyc_Absyn_Tqual _tmpF9;struct _tagged_arr*_tmpFA;struct Cyc_Absyn_Aggrfield*
_tmpF4=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmpF5=*_tmpF4;_LL1B4: _tmpFA=
_tmpF5.name;goto _LL1B3;_LL1B3: _tmpF9=_tmpF5.tq;goto _LL1B2;_LL1B2: _tmpF8=(void*)
_tmpF5.type;goto _LL1B1;_LL1B1: _tmpF7=_tmpF5.width;goto _LL1B0;_LL1B0: _tmpF6=
_tmpF5.attributes;goto _LL1AF;_LL1AF:((void(*)(struct Cyc_Absyn_Tqual,void*,void(*
f)(struct _tagged_arr*),struct _tagged_arr*))Cyc_Dump_dumptqtd)(_tmpF9,_tmpF8,Cyc_Dump_dump_alphastr,
_tmpFA);if(_tmpF7 != 0){Cyc_Dump_dump_char((int)':');Cyc_Dump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmpF7));}Cyc_Dump_dump_symbol(_tag_arr(";\\\\\n",sizeof(
unsigned char),5));}}}void Cyc_Dump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*
td){Cyc_Dump_dumpqvar(td->name);Cyc_Dump_dumptvars(td->tvs);}void Cyc_Dump_dump_boldtypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Dump_dump_boldqvar(td->name);Cyc_Dump_dumptvars(
td->tvs);}static void Cyc_Dump_dumpids(struct Cyc_List_List*vds){for(0;vds != 0;vds=
vds->tl){Cyc_Dump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl
!= 0){Cyc_Dump_dump_char((int)',');}}}static void Cyc_Dump_dump_upperbound(struct
Cyc_Absyn_Exp*e){unsigned int i=Cyc_Evexp_eval_const_uint_exp(e);if(i != 1){Cyc_Dump_dump_char((
int)'{');Cyc_Dump_dumpexp(e);Cyc_Dump_dump_char((int)'}');}}void Cyc_Dump_dumptms(
struct Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){f(a);return;}{void*
_tmpFB=(void*)tms->hd;struct Cyc_Absyn_Tqual _tmpFC;void*_tmpFD;void*_tmpFE;struct
Cyc_Absyn_Exp*_tmpFF;struct Cyc_Absyn_Tqual _tmp100;void*_tmp101;void*_tmp102;
struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Tqual _tmp104;void*_tmp105;void*
_tmp106;struct Cyc_Absyn_Tqual _tmp107;void*_tmp108;struct Cyc_Absyn_Tvar*_tmp109;
void*_tmp10A;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Tqual _tmp10C;void*
_tmp10D;struct Cyc_Absyn_Tvar*_tmp10E;void*_tmp10F;struct Cyc_Absyn_Exp*_tmp110;
struct Cyc_Absyn_Tqual _tmp111;void*_tmp112;struct Cyc_Absyn_Tvar*_tmp113;void*
_tmp114;struct Cyc_Absyn_Tqual _tmp115;void*_tmp116;struct Cyc_Absyn_Exp*_tmp117;
struct Cyc_Absyn_Tqual _tmp118;void*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Tqual
_tmp11B;void*_tmp11C;_LL1B6: if((unsigned int)_tmpFB > 1?*((int*)_tmpFB)== 1: 0){
_LL1CC: _tmpFE=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFB)->f1;if((
unsigned int)_tmpFE > 1?*((int*)_tmpFE)== 1: 0){_LL1CD: _tmpFF=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmpFE)->f1;goto _LL1CB;}else{goto _LL1B8;}_LL1CB: _tmpFD=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFB)->f2;if((int)_tmpFD == 2){goto _LL1CA;}else{goto _LL1B8;}_LL1CA: _tmpFC=((
struct Cyc_Absyn_Pointer_mod_struct*)_tmpFB)->f3;goto _LL1B7;}else{goto _LL1B8;}
_LL1B8: if((unsigned int)_tmpFB > 1?*((int*)_tmpFB)== 1: 0){_LL1D0: _tmp102=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmpFB)->f1;if((unsigned int)_tmp102 > 1?*((
int*)_tmp102)== 0: 0){_LL1D1: _tmp103=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmp102)->f1;goto _LL1CF;}else{goto _LL1BA;}_LL1CF: _tmp101=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFB)->f2;if((int)_tmp101 == 2){goto _LL1CE;}else{goto _LL1BA;}_LL1CE: _tmp100=((
struct Cyc_Absyn_Pointer_mod_struct*)_tmpFB)->f3;goto _LL1B9;}else{goto _LL1BA;}
_LL1BA: if((unsigned int)_tmpFB > 1?*((int*)_tmpFB)== 1: 0){_LL1D4: _tmp106=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmpFB)->f1;if((int)_tmp106 == 0){goto _LL1D3;}
else{goto _LL1BC;}_LL1D3: _tmp105=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFB)->f2;if((int)_tmp105 == 2){goto _LL1D2;}else{goto _LL1BC;}_LL1D2: _tmp104=((
struct Cyc_Absyn_Pointer_mod_struct*)_tmpFB)->f3;goto _LL1BB;}else{goto _LL1BC;}
_LL1BC: if((unsigned int)_tmpFB > 1?*((int*)_tmpFB)== 1: 0){_LL1D8: _tmp10A=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmpFB)->f1;if((unsigned int)_tmp10A > 1?*((
int*)_tmp10A)== 1: 0){_LL1D9: _tmp10B=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmp10A)->f1;goto _LL1D6;}else{goto _LL1BE;}_LL1D6: _tmp108=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFB)->f2;if((unsigned int)_tmp108 > 3?*((int*)_tmp108)== 1: 0){_LL1D7: _tmp109=((
struct Cyc_Absyn_VarType_struct*)_tmp108)->f1;goto _LL1D5;}else{goto _LL1BE;}_LL1D5:
_tmp107=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFB)->f3;goto _LL1BD;}else{goto
_LL1BE;}_LL1BE: if((unsigned int)_tmpFB > 1?*((int*)_tmpFB)== 1: 0){_LL1DD: _tmp10F=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFB)->f1;if((unsigned int)_tmp10F
> 1?*((int*)_tmp10F)== 0: 0){_LL1DE: _tmp110=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmp10F)->f1;goto _LL1DB;}else{goto _LL1C0;}_LL1DB: _tmp10D=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFB)->f2;if((unsigned int)_tmp10D > 3?*((int*)_tmp10D)== 1: 0){_LL1DC: _tmp10E=((
struct Cyc_Absyn_VarType_struct*)_tmp10D)->f1;goto _LL1DA;}else{goto _LL1C0;}_LL1DA:
_tmp10C=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFB)->f3;goto _LL1BF;}else{goto
_LL1C0;}_LL1C0: if((unsigned int)_tmpFB > 1?*((int*)_tmpFB)== 1: 0){_LL1E2: _tmp114=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFB)->f1;if((int)_tmp114 == 0){
goto _LL1E0;}else{goto _LL1C2;}_LL1E0: _tmp112=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFB)->f2;if((unsigned int)_tmp112 > 3?*((int*)_tmp112)== 1: 0){_LL1E1: _tmp113=((
struct Cyc_Absyn_VarType_struct*)_tmp112)->f1;goto _LL1DF;}else{goto _LL1C2;}_LL1DF:
_tmp111=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFB)->f3;goto _LL1C1;}else{goto
_LL1C2;}_LL1C2: if((unsigned int)_tmpFB > 1?*((int*)_tmpFB)== 1: 0){_LL1E4: _tmp116=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFB)->f1;if((unsigned int)_tmp116
> 1?*((int*)_tmp116)== 1: 0){_LL1E5: _tmp117=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmp116)->f1;goto _LL1E3;}else{goto _LL1C4;}_LL1E3: _tmp115=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFB)->f3;goto _LL1C3;}else{goto _LL1C4;}_LL1C4: if((unsigned int)_tmpFB > 1?*((
int*)_tmpFB)== 1: 0){_LL1E7: _tmp119=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFB)->f1;if((unsigned int)_tmp119 > 1?*((int*)_tmp119)== 0: 0){_LL1E8: _tmp11A=((
struct Cyc_Absyn_NonNullable_ps_struct*)_tmp119)->f1;goto _LL1E6;}else{goto _LL1C6;}
_LL1E6: _tmp118=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFB)->f3;goto _LL1C5;}
else{goto _LL1C6;}_LL1C6: if((unsigned int)_tmpFB > 1?*((int*)_tmpFB)== 1: 0){_LL1EA:
_tmp11C=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFB)->f1;if((int)_tmp11C
== 0){goto _LL1E9;}else{goto _LL1C8;}_LL1E9: _tmp11B=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFB)->f3;goto _LL1C7;}else{goto _LL1C8;}_LL1C8: goto _LL1C9;_LL1B7: Cyc_Dump_dump_pointer(
_tag_arr("*",sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(_tmpFF);Cyc_Dump_dump_alpha(
_tag_arr("`H",sizeof(unsigned char),3));Cyc_Dump_dumptms(tms->tl,f,a);return;
_LL1B9: Cyc_Dump_dump_pointer(_tag_arr("@",sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(
_tmp103);Cyc_Dump_dump_alpha(_tag_arr("`H",sizeof(unsigned char),3));Cyc_Dump_dumptms(
tms->tl,f,a);return;_LL1BB: Cyc_Dump_dump_pointer(_tag_arr("{?}",sizeof(
unsigned char),4));Cyc_Dump_dump_alpha(_tag_arr("`H",sizeof(unsigned char),3));
Cyc_Dump_dumptms(tms->tl,f,a);return;_LL1BD: Cyc_Dump_dump_pointer(_tag_arr("*",
sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(_tmp10B);Cyc_Dump_dump_alphastr(
_tmp109->name);Cyc_Dump_dumptms(tms->tl,f,a);return;_LL1BF: Cyc_Dump_dump_pointer(
_tag_arr("@",sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(_tmp110);Cyc_Dump_dump_alphastr(
_tmp10E->name);Cyc_Dump_dumptms(tms->tl,f,a);return;_LL1C1: Cyc_Dump_dump_pointer(
_tag_arr("{?}",sizeof(unsigned char),4));Cyc_Dump_dump_alphastr(_tmp113->name);
Cyc_Dump_dumptms(tms->tl,f,a);return;_LL1C3: Cyc_Dump_dump_pointer(_tag_arr("*",
sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(_tmp117);Cyc_Dump_dumptms(tms->tl,
f,a);return;_LL1C5: Cyc_Dump_dump_pointer(_tag_arr("@",sizeof(unsigned char),2));
Cyc_Dump_dump_upperbound(_tmp11A);Cyc_Dump_dumptms(tms->tl,f,a);return;_LL1C7:
Cyc_Dump_dump_pointer(_tag_arr("{?}",sizeof(unsigned char),4));Cyc_Dump_dumptms(
tms->tl,f,a);return;_LL1C9: {int next_is_pointer=0;if(tms->tl != 0){void*_tmp11D=(
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_LL1EC: if((unsigned int)
_tmp11D > 1?*((int*)_tmp11D)== 1: 0){goto _LL1ED;}else{goto _LL1EE;}_LL1EE: goto
_LL1EF;_LL1ED: next_is_pointer=1;goto _LL1EB;_LL1EF: goto _LL1EB;_LL1EB:;}if(
next_is_pointer){Cyc_Dump_dump_char((int)'(');}Cyc_Dump_dumptms(tms->tl,f,a);if(
next_is_pointer){Cyc_Dump_dump_char((int)')');}{void*_tmp11E=(void*)tms->hd;
struct Cyc_Absyn_Exp*_tmp11F;void*_tmp120;struct Cyc_List_List*_tmp121;struct Cyc_Core_Opt*
_tmp122;struct Cyc_Absyn_VarargInfo*_tmp123;int _tmp124;struct Cyc_List_List*
_tmp125;void*_tmp126;struct Cyc_Position_Segment*_tmp127;struct Cyc_List_List*
_tmp128;int _tmp129;struct Cyc_Position_Segment*_tmp12A;struct Cyc_List_List*
_tmp12B;void*_tmp12C;void*_tmp12D;_LL1F1: if((int)_tmp11E == 0){goto _LL1F2;}else{
goto _LL1F3;}_LL1F3: if((unsigned int)_tmp11E > 1?*((int*)_tmp11E)== 0: 0){_LL1FF:
_tmp11F=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmp11E)->f1;goto _LL1F4;}else{
goto _LL1F5;}_LL1F5: if((unsigned int)_tmp11E > 1?*((int*)_tmp11E)== 2: 0){_LL200:
_tmp120=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp11E)->f1;if(*((int*)
_tmp120)== 1){_LL205: _tmp125=((struct Cyc_Absyn_WithTypes_struct*)_tmp120)->f1;
goto _LL204;_LL204: _tmp124=((struct Cyc_Absyn_WithTypes_struct*)_tmp120)->f2;goto
_LL203;_LL203: _tmp123=((struct Cyc_Absyn_WithTypes_struct*)_tmp120)->f3;goto
_LL202;_LL202: _tmp122=((struct Cyc_Absyn_WithTypes_struct*)_tmp120)->f4;goto
_LL201;_LL201: _tmp121=((struct Cyc_Absyn_WithTypes_struct*)_tmp120)->f5;goto
_LL1F6;}else{goto _LL1F7;}}else{goto _LL1F7;}_LL1F7: if((unsigned int)_tmp11E > 1?*((
int*)_tmp11E)== 2: 0){_LL206: _tmp126=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp11E)->f1;if(*((int*)_tmp126)== 0){_LL208: _tmp128=((struct Cyc_Absyn_NoTypes_struct*)
_tmp126)->f1;goto _LL207;_LL207: _tmp127=((struct Cyc_Absyn_NoTypes_struct*)_tmp126)->f2;
goto _LL1F8;}else{goto _LL1F9;}}else{goto _LL1F9;}_LL1F9: if((unsigned int)_tmp11E > 
1?*((int*)_tmp11E)== 3: 0){_LL20B: _tmp12B=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp11E)->f1;goto _LL20A;_LL20A: _tmp12A=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp11E)->f2;goto _LL209;_LL209: _tmp129=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp11E)->f3;goto _LL1FA;}else{goto _LL1FB;}_LL1FB: if((unsigned int)_tmp11E > 1?*((
int*)_tmp11E)== 4: 0){goto _LL1FC;}else{goto _LL1FD;}_LL1FD: if((unsigned int)
_tmp11E > 1?*((int*)_tmp11E)== 1: 0){_LL20D: _tmp12D=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp11E)->f1;goto _LL20C;_LL20C: _tmp12C=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp11E)->f2;goto _LL1FE;}else{goto _LL1F0;}_LL1F2: Cyc_Dump_dump_symbol(_tag_arr("[]",
sizeof(unsigned char),3));goto _LL1F0;_LL1F4: Cyc_Dump_dump_char((int)'[');Cyc_Dump_dumpexp(
_tmp11F);Cyc_Dump_dump_char((int)']');goto _LL1F0;_LL1F6: Cyc_Dump_dumpfunargs(
_tmp125,_tmp124,_tmp123,_tmp122,_tmp121);goto _LL1F0;_LL1F8:((void(*)(void(*f)(
struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dump_alphastr,
_tmp128,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),
2),_tag_arr(", ",sizeof(unsigned char),3));goto _LL1F0;_LL1FA: if(_tmp129){Cyc_Dump_dumpkindedtvars(
_tmp12B);}else{Cyc_Dump_dumptvars(_tmp12B);}goto _LL1F0;_LL1FC: goto _LL1F0;_LL1FE:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp12E=_cycalloc(sizeof(*
_tmp12E));_tmp12E[0]=({struct Cyc_Core_Impossible_struct _tmp12F;_tmp12F.tag=Cyc_Core_Impossible;
_tmp12F.f1=_tag_arr("dumptms",sizeof(unsigned char),8);_tmp12F;});_tmp12E;}));
_LL1F0:;}return;}_LL1B5:;}}void Cyc_Dump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,
void(*f)(void*),void*a){struct Cyc_List_List*_tmp131;void*_tmp132;struct Cyc_Absyn_Tqual
_tmp133;struct _tuple10 _tmp130=Cyc_Absynpp_to_tms(tq,t);_LL211: _tmp133=_tmp130.f1;
goto _LL210;_LL210: _tmp132=_tmp130.f2;goto _LL20F;_LL20F: _tmp131=_tmp130.f3;goto
_LL20E;_LL20E: Cyc_Dump_dumptq(_tmp133);Cyc_Dump_dumpntyp(_tmp132);Cyc_Dump_dumptms(
Cyc_List_imp_rev(_tmp131),f,a);}void Cyc_Dump_pr_comment(struct Cyc_Std___cycFILE*
outf,struct _tagged_arr s){int depth=0;int len=(int)Cyc_Std_strlen(s);int i=0;for(0;i
< len;i ++){unsigned char c=((const unsigned char*)s.curr)[i];if(c != '['){Cyc_Std_fputc((
int)c,outf);continue;}Cyc_Std_fputs(_tag_arr("\\texttt{",sizeof(unsigned char),9),
outf);i ++;depth ++;for(0;i < len;i ++){unsigned char c=((const unsigned char*)s.curr)[
i];if(c == ']'){depth --;if(depth == 0){Cyc_Std_fputc((int)'}',outf);break;}}else{
if(c == '['){depth ++;}}Cyc_Std_fputc((int)c,outf);}}}void Cyc_Dump_dump_comment(
struct _tagged_arr s){Cyc_Dump_pr_comment(Cyc_Std_stdout,s);}void Cyc_Dump_dumpdecl(
struct Cyc_Absyn_Decl*d,struct _tagged_arr comment){void*_tmp134=(void*)d->r;struct
Cyc_Absyn_Aggrdecl*_tmp135;struct Cyc_Absyn_Vardecl*_tmp136;struct Cyc_Absyn_Vardecl
_tmp137;void*_tmp138;struct Cyc_Absyn_Tqual _tmp139;struct _tuple1*_tmp13A;struct
Cyc_Absyn_Tuniondecl*_tmp13B;struct Cyc_Absyn_Tuniondecl _tmp13C;int _tmp13D;struct
Cyc_Core_Opt*_tmp13E;struct Cyc_List_List*_tmp13F;struct _tuple1*_tmp140;void*
_tmp141;struct Cyc_Absyn_Enumdecl*_tmp142;struct Cyc_Absyn_Enumdecl _tmp143;struct
Cyc_Core_Opt*_tmp144;struct _tuple1*_tmp145;void*_tmp146;struct Cyc_Absyn_Typedefdecl*
_tmp147;_LL213: if(*((int*)_tmp134)== 4){_LL229: _tmp135=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp134)->f1;goto _LL214;}else{goto _LL215;}_LL215: if(*((int*)_tmp134)== 0){_LL22A:
_tmp136=((struct Cyc_Absyn_Var_d_struct*)_tmp134)->f1;_tmp137=*_tmp136;_LL22D:
_tmp13A=_tmp137.name;goto _LL22C;_LL22C: _tmp139=_tmp137.tq;goto _LL22B;_LL22B:
_tmp138=(void*)_tmp137.type;goto _LL216;}else{goto _LL217;}_LL217: if(*((int*)
_tmp134)== 5){_LL22E: _tmp13B=((struct Cyc_Absyn_Tunion_d_struct*)_tmp134)->f1;
_tmp13C=*_tmp13B;_LL233: _tmp141=(void*)_tmp13C.sc;goto _LL232;_LL232: _tmp140=
_tmp13C.name;goto _LL231;_LL231: _tmp13F=_tmp13C.tvs;goto _LL230;_LL230: _tmp13E=
_tmp13C.fields;goto _LL22F;_LL22F: _tmp13D=_tmp13C.is_xtunion;goto _LL218;}else{
goto _LL219;}_LL219: if(*((int*)_tmp134)== 6){_LL234: _tmp142=((struct Cyc_Absyn_Enum_d_struct*)
_tmp134)->f1;_tmp143=*_tmp142;_LL237: _tmp146=(void*)_tmp143.sc;goto _LL236;_LL236:
_tmp145=_tmp143.name;goto _LL235;_LL235: _tmp144=_tmp143.fields;goto _LL21A;}else{
goto _LL21B;}_LL21B: if(*((int*)_tmp134)== 7){_LL238: _tmp147=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp134)->f1;goto _LL21C;}else{goto _LL21D;}_LL21D: if(*((int*)_tmp134)== 1){goto
_LL21E;}else{goto _LL21F;}_LL21F: if(*((int*)_tmp134)== 2){goto _LL220;}else{goto
_LL221;}_LL221: if(*((int*)_tmp134)== 3){goto _LL222;}else{goto _LL223;}_LL223: if(*((
int*)_tmp134)== 8){goto _LL224;}else{goto _LL225;}_LL225: if(*((int*)_tmp134)== 9){
goto _LL226;}else{goto _LL227;}_LL227: if(*((int*)_tmp134)== 10){goto _LL228;}else{
goto _LL212;}_LL214: Cyc_Dump_dump_begin();Cyc_Dump_dump_alpha((void*)_tmp135->kind
== (void*)1? _tag_arr("union",sizeof(unsigned char),6): _tag_arr("struct",sizeof(
unsigned char),7));Cyc_Dump_dump_boldqvar(_tmp135->name);Cyc_Dump_dumptvars(
_tmp135->tvs);if(_tmp135->fields == 0){Cyc_Dump_dump_symbol(_tag_arr(";\n",
sizeof(unsigned char),3));}else{Cyc_Dump_dump_symbol(_tag_arr(" ",sizeof(
unsigned char),2));Cyc_Dump_dump_lb();if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp135->exist_vars))->v != 0){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Dump_egroup)(Cyc_Dump_dumptvar,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp135->exist_vars))->v,_tag_arr("[",sizeof(
unsigned char),2),_tag_arr("]",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2));Cyc_Dump_dump_alpha(_tag_arr("",sizeof(unsigned char),1));}
Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",sizeof(unsigned char),4));Cyc_Dump_dumpaggrfields((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp135->fields))->v);Cyc_Dump_dump_rb();
Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(unsigned char),3));}Cyc_Dump_dump_middle();
Cyc_Dump_dump_comment(comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(
unsigned char),2));Cyc_Dump_dump_end();goto _LL212;_LL216: Cyc_Dump_dump_begin();((
void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct _tuple1*),struct _tuple1*a))
Cyc_Dump_dumptqtd)(_tmp139,_tmp138,Cyc_Dump_dump_boldqvar,_tmp13A);Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(
comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(unsigned char),2));Cyc_Dump_dump_end();
goto _LL212;_LL218: Cyc_Dump_dump_begin();if(_tmp13D){Cyc_Dump_dump_alpha(_tag_arr("xtunion",
sizeof(unsigned char),8));}else{Cyc_Dump_dump_alpha(_tag_arr("tunion",sizeof(
unsigned char),7));}Cyc_Dump_dump_boldqvar(_tmp140);Cyc_Dump_dumptvars(_tmp13F);
if(_tmp13E == 0){Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(unsigned char),3));}
else{Cyc_Dump_dump_symbol(_tag_arr(" ",sizeof(unsigned char),2));Cyc_Dump_dump_lb();
Cyc_Dump_dump_symbol(_tag_arr("\\\\\n~~",sizeof(unsigned char),6));Cyc_Dump_dumptunionfields((
struct Cyc_List_List*)_tmp13E->v);Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",sizeof(
unsigned char),4));Cyc_Dump_dump_rb();Cyc_Dump_dump_symbol(_tag_arr(";\n",
sizeof(unsigned char),3));}Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(comment);
Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(unsigned char),2));Cyc_Dump_dump_end();
goto _LL212;_LL21A: Cyc_Dump_dump_begin();Cyc_Dump_dump_alpha(_tag_arr("enum",
sizeof(unsigned char),5));Cyc_Dump_dump_boldqvar(_tmp145);if(_tmp144 == 0){Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));}else{Cyc_Dump_dump_symbol(_tag_arr(" ",
sizeof(unsigned char),2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(_tag_arr("\\\\\n~~",
sizeof(unsigned char),6));Cyc_Dump_dumpenumfields((struct Cyc_List_List*)_tmp144->v);
Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",sizeof(unsigned char),4));Cyc_Dump_dump_rb();
Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(unsigned char),3));}Cyc_Dump_dump_middle();
Cyc_Dump_dump_comment(comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(
unsigned char),2));Cyc_Dump_dump_end();goto _LL212;_LL21C: Cyc_Dump_dump_begin();
Cyc_Dump_dump_alpha(_tag_arr("typedef",sizeof(unsigned char),8));((void(*)(
struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*
a))Cyc_Dump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp148;_tmp148.q_const=0;_tmp148.q_volatile=
0;_tmp148.q_restrict=0;_tmp148;}),(void*)_tmp147->defn,Cyc_Dump_dump_boldtypedefname,
_tmp147);Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(unsigned char),3));Cyc_Dump_dump_middle();
Cyc_Dump_dump_comment(comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(
unsigned char),2));Cyc_Dump_dump_end();goto _LL212;_LL21E: goto _LL220;_LL220: goto
_LL222;_LL222: goto _LL224;_LL224: goto _LL226;_LL226: goto _LL228;_LL228: goto _LL212;
_LL212:;}static int Cyc_is_other_special(unsigned char c){switch(c){case '\\': _LL239:
goto _LL23A;case '"': _LL23A: goto _LL23B;case ';': _LL23B: goto _LL23C;case '&': _LL23C:
goto _LL23D;case '(': _LL23D: goto _LL23E;case ')': _LL23E: goto _LL23F;case '|': _LL23F:
goto _LL240;case '^': _LL240: goto _LL241;case '<': _LL241: goto _LL242;case '>': _LL242:
goto _LL243;case ' ': _LL243: goto _LL244;case '\n': _LL244: goto _LL245;case '\t': _LL245:
return 1;default: _LL246: return 0;}}static struct _tagged_arr Cyc_sh_escape_string(
struct _tagged_arr s){unsigned int _tmp149=Cyc_Std_strlen(s);int _tmp14A=0;int
_tmp14B=0;{int i=0;for(0;i < _tmp149;i ++){unsigned char _tmp14C=((const
unsigned char*)s.curr)[i];if(_tmp14C == '\''){_tmp14A ++;}else{if(Cyc_is_other_special(
_tmp14C)){_tmp14B ++;}}}}if(_tmp14A == 0? _tmp14B == 0: 0){return s;}if(_tmp14A == 0){
return(struct _tagged_arr)Cyc_Std_strconcat_l(({struct _tagged_arr*_tmp14D[3];
_tmp14D[2]=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",sizeof(
unsigned char),2);_tmp14D[1]=({struct _tagged_arr*_tmp14F=_cycalloc(sizeof(*
_tmp14F));_tmp14F[0]=(struct _tagged_arr)s;_tmp14F;});_tmp14D[0]=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"'",sizeof(unsigned char),2);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp14D,sizeof(struct _tagged_arr*),3));}));}{
unsigned int _tmp151=(_tmp149 + _tmp14A)+ _tmp14B;struct _tagged_arr _tmp152=({
unsigned int _tmp156=_tmp151 + 1;unsigned char*_tmp157=(unsigned char*)
_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmp156));struct _tagged_arr
_tmp159=_tag_arr(_tmp157,sizeof(unsigned char),_tmp151 + 1);{unsigned int _tmp158=
_tmp156;unsigned int i;for(i=0;i < _tmp158;i ++){_tmp157[i]='\000';}};_tmp159;});
int _tmp153=0;int _tmp154=0;for(0;_tmp153 < _tmp149;_tmp153 ++){unsigned char _tmp155=((
const unsigned char*)s.curr)[_tmp153];if(_tmp155 == '\''? 1: Cyc_is_other_special(
_tmp155)){*((unsigned char*)_check_unknown_subscript(_tmp152,sizeof(
unsigned char),_tmp154 ++))='\\';}*((unsigned char*)_check_unknown_subscript(
_tmp152,sizeof(unsigned char),_tmp154 ++))=_tmp155;}return(struct _tagged_arr)
_tmp152;}}static struct _tagged_arr*Cyc_sh_escape_stringptr(struct _tagged_arr*sp){
return({struct _tagged_arr*_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A[0]=Cyc_sh_escape_string(*
sp);_tmp15A;});}static struct Cyc_Lineno_Pos*Cyc_new_pos(){return({struct Cyc_Lineno_Pos*
_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B->logical_file=_tag_arr("",sizeof(
unsigned char),1);_tmp15B->line=Cyc_Core_new_string(0);_tmp15B->line_no=0;
_tmp15B->col=0;_tmp15B;});}struct _tuple15{int f1;struct Cyc_Lineno_Pos*f2;};static
struct _tuple15*Cyc_start2pos(int x){return({struct _tuple15*_tmp15C=_cycalloc(
sizeof(*_tmp15C));_tmp15C->f1=x;_tmp15C->f2=Cyc_new_pos();_tmp15C;});}static int
Cyc_decl2start(struct Cyc_Absyn_Decl*d){return((struct Cyc_Position_Segment*)
_check_null(d->loc))->start;}struct _tuple16{struct Cyc_Lineno_Pos*f1;void*f2;};
static struct Cyc_List_List*Cyc_this_file(struct _tagged_arr file,struct Cyc_List_List*
x){struct Cyc_List_List*_tmp15D=0;for(0;x != 0;x=x->tl){if(Cyc_Std_strcmp(((*((
struct _tuple16*)x->hd)).f1)->logical_file,file)== 0){_tmp15D=({struct Cyc_List_List*
_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E->hd=(struct _tuple16*)x->hd;_tmp15E->tl=
_tmp15D;_tmp15E;});}}_tmp15D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp15D);return _tmp15D;}static int Cyc_lineno(struct Cyc_Lineno_Pos*
p){return p->line_no;}static struct Cyc_List_List*Cyc_flatten_decls(struct Cyc_List_List*
decls){struct Cyc_List_List*_tmp15F=0;while(decls != 0){void*_tmp160=(void*)((
struct Cyc_Absyn_Decl*)decls->hd)->r;struct Cyc_List_List*_tmp161;struct Cyc_List_List*
_tmp162;struct Cyc_List_List*_tmp163;_LL249: if(*((int*)_tmp160)== 8){_LL25F:
_tmp161=((struct Cyc_Absyn_Namespace_d_struct*)_tmp160)->f2;goto _LL24A;}else{goto
_LL24B;}_LL24B: if(*((int*)_tmp160)== 9){_LL260: _tmp162=((struct Cyc_Absyn_Using_d_struct*)
_tmp160)->f2;goto _LL24C;}else{goto _LL24D;}_LL24D: if(*((int*)_tmp160)== 10){
_LL261: _tmp163=((struct Cyc_Absyn_ExternC_d_struct*)_tmp160)->f1;goto _LL24E;}
else{goto _LL24F;}_LL24F: if(*((int*)_tmp160)== 4){goto _LL250;}else{goto _LL251;}
_LL251: if(*((int*)_tmp160)== 0){goto _LL252;}else{goto _LL253;}_LL253: if(*((int*)
_tmp160)== 5){goto _LL254;}else{goto _LL255;}_LL255: if(*((int*)_tmp160)== 6){goto
_LL256;}else{goto _LL257;}_LL257: if(*((int*)_tmp160)== 7){goto _LL258;}else{goto
_LL259;}_LL259: if(*((int*)_tmp160)== 1){goto _LL25A;}else{goto _LL25B;}_LL25B: if(*((
int*)_tmp160)== 2){goto _LL25C;}else{goto _LL25D;}_LL25D: if(*((int*)_tmp160)== 3){
goto _LL25E;}else{goto _LL248;}_LL24A: _tmp162=_tmp161;goto _LL24C;_LL24C: _tmp163=
_tmp162;goto _LL24E;_LL24E: decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmp163,decls->tl);goto _LL248;_LL250:
goto _LL252;_LL252: goto _LL254;_LL254: goto _LL256;_LL256: goto _LL258;_LL258: goto
_LL25A;_LL25A: goto _LL25C;_LL25C: goto _LL25E;_LL25E: _tmp15F=({struct Cyc_List_List*
_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->hd=(struct Cyc_Absyn_Decl*)decls->hd;
_tmp164->tl=_tmp15F;_tmp164;});decls=decls->tl;goto _LL248;_LL248:;}return((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15F);}
static struct Cyc_List_List*Cyc_cycdoc_files=0;static void Cyc_add_other(struct
_tagged_arr s){Cyc_cycdoc_files=({struct Cyc_List_List*_tmp165=_cycalloc(sizeof(*
_tmp165));_tmp165->hd=({struct _tagged_arr*_tmp166=_cycalloc(sizeof(*_tmp166));
_tmp166[0]=s;_tmp166;});_tmp165->tl=Cyc_cycdoc_files;_tmp165;});}static struct Cyc_List_List*
Cyc_cycargs=0;static void Cyc_add_cycarg(struct _tagged_arr s){Cyc_cycargs=({struct
Cyc_List_List*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167->hd=({struct
_tagged_arr*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168[0]=s;_tmp168;});_tmp167->tl=
Cyc_cycargs;_tmp167;});}static unsigned char _tmp169[8]="cyclone";static struct
_tagged_arr Cyc_cyclone_file={_tmp169,_tmp169,_tmp169 + 8};static void Cyc_set_cyclone_file(
struct _tagged_arr s){Cyc_cyclone_file=s;}struct _tuple17{int f1;struct Cyc_Absyn_Decl*
f2;};struct _tuple18{struct Cyc_Lineno_Pos*f1;struct Cyc_Absyn_Decl*f2;};static void
Cyc_process_file(struct _tagged_arr filename){struct _tagged_arr _tmp16A=Cyc_Filename_chop_extension(
filename);struct _tagged_arr _tmp16B=Cyc_Std_strconcat((struct _tagged_arr)_tmp16A,
_tag_arr(".cyp",sizeof(unsigned char),5));struct _tagged_arr _tmp16C=Cyc_Std_str_sepstr(({
struct Cyc_List_List*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->hd=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(unsigned char),1);
_tmp18D->tl=((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cycargs));_tmp18D;}),_tag_arr(" ",
sizeof(unsigned char),2));struct _tagged_arr _tmp16D=({struct Cyc_Std_String_pa_struct
_tmp18C;_tmp18C.tag=0;_tmp18C.f1=(struct _tagged_arr)Cyc_sh_escape_string(
filename);{struct Cyc_Std_String_pa_struct _tmp18B;_tmp18B.tag=0;_tmp18B.f1=(
struct _tagged_arr)Cyc_sh_escape_string((struct _tagged_arr)_tmp16B);{struct Cyc_Std_String_pa_struct
_tmp18A;_tmp18A.tag=0;_tmp18A.f1=(struct _tagged_arr)_tmp16C;{struct Cyc_Std_String_pa_struct
_tmp189;_tmp189.tag=0;_tmp189.f1=(struct _tagged_arr)Cyc_cyclone_file;{void*
_tmp188[4]={& _tmp189,& _tmp18A,& _tmp18B,& _tmp18C};Cyc_Std_aprintf(_tag_arr("%s %s -E -o %s -x cyc %s",
sizeof(unsigned char),25),_tag_arr(_tmp188,sizeof(void*),4));}}}}});if(Cyc_Std_system((
struct _tagged_arr)_tmp16D)!= 0){({void*_tmp16E[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\nError: preprocessing\n",sizeof(unsigned char),23),_tag_arr(_tmp16E,
sizeof(void*),0));});return;}Cyc_Position_reset_position((struct _tagged_arr)
_tmp16B);{struct Cyc_Std___cycFILE*in_file=(struct Cyc_Std___cycFILE*)_check_null(
Cyc_Std_fopen((struct _tagged_arr)_tmp16B,_tag_arr("r",sizeof(unsigned char),2)));
Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp16F=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init();Cyc_Std_file_close((struct Cyc_Std___cycFILE*)in_file);_tmp16F=
Cyc_flatten_decls(_tmp16F);{struct Cyc_List_List*_tmp170=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(int),struct Cyc_List_List*x))Cyc_List_map)(Cyc_start2pos,((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_decl2start,_tmp16F));Cyc_Lineno_poss_of_abss((struct
_tagged_arr)_tmp16B,_tmp170);Cyc_Std_remove((struct _tagged_arr)_tmp16B);{struct
Cyc_List_List*_tmp171=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*f)(struct
_tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(struct
_tuple15*))Cyc_Core_snd,_tmp170),_tmp16F);_tmp171=((struct Cyc_List_List*(*)(
struct _tagged_arr file,struct Cyc_List_List*x))Cyc_this_file)(filename,_tmp171);{
struct Cyc_List_List*_tmp172=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Lineno_Pos*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(struct
Cyc_Lineno_Pos*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)(struct _tuple18*))Cyc_Core_fst,_tmp171)),((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Decl*(*)(struct _tuple18*))Cyc_Core_snd,_tmp171));struct Cyc_Std___cycFILE*
_tmp173=(struct Cyc_Std___cycFILE*)_check_null(Cyc_Std_fopen(filename,_tag_arr("r",
sizeof(unsigned char),2)));struct Cyc_Lexing_lexbuf*_tmp174=Cyc_Lexing_from_file(
_tmp173);struct Cyc_List_List*_tmp175=0;struct _tuple11*comment;while((comment=((
struct _tuple11*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_token)(_tmp174))!= 0){
_tmp175=({struct Cyc_List_List*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->hd=(
struct _tuple11*)_check_null(comment);_tmp176->tl=_tmp175;_tmp176;});}Cyc_Std_fclose(
_tmp173);_tmp175=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp175);{struct Cyc_List_List*_tmp177=((struct Cyc_List_List*(*)(struct _tuple15*(*
f)(int),struct Cyc_List_List*x))Cyc_List_map)(Cyc_start2pos,((struct Cyc_List_List*(*)(
int(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((int(*)(struct
_tuple11*))Cyc_Core_fst,_tmp175));Cyc_Lineno_poss_of_abss(filename,_tmp177);{
struct Cyc_List_List*_tmp178=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*
f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple15*))Cyc_Core_snd,_tmp177),((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))
Cyc_Core_snd,_tmp175));_tmp178=Cyc_this_file(filename,_tmp178);{struct Cyc_List_List*
_tmp179=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*f)(
struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple16*))Cyc_Core_fst,_tmp178)),((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple16*))
Cyc_Core_snd,_tmp178));while(_tmp172 != 0? _tmp179 != 0: 0){struct _tuple17 _tmp17B;
struct Cyc_Absyn_Decl*_tmp17C;int _tmp17D;struct _tuple17*_tmp17A=(struct _tuple17*)
_tmp172->hd;_tmp17B=*_tmp17A;_LL264: _tmp17D=_tmp17B.f1;goto _LL263;_LL263: _tmp17C=
_tmp17B.f2;goto _LL262;_LL262: {struct _tuple11 _tmp17F;void*_tmp180;int _tmp181;
struct _tuple11*_tmp17E=(struct _tuple11*)_tmp179->hd;_tmp17F=*_tmp17E;_LL267:
_tmp181=_tmp17F.f1;goto _LL266;_LL266: _tmp180=_tmp17F.f2;goto _LL265;_LL265: {void*
_tmp182=_tmp180;struct _tagged_arr _tmp183;struct _tagged_arr _tmp184;_LL269: if(*((
int*)_tmp182)== 1){_LL26D: _tmp183=((struct Cyc_Standalone_struct*)_tmp182)->f1;
goto _LL26A;}else{goto _LL26B;}_LL26B: if(*((int*)_tmp182)== 0){_LL26E: _tmp184=((
struct Cyc_MatchDecl_struct*)_tmp182)->f1;goto _LL26C;}else{goto _LL268;}_LL26A: Cyc_Dump_dump_comment((
struct _tagged_arr)_tmp183);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(
unsigned char),2));_tmp179=_tmp179->tl;goto _LL268;_LL26C: if(_tmp181 < _tmp17D){
_tmp179=_tmp179->tl;continue;}if(_tmp172->tl != 0){struct _tuple17 _tmp186;int
_tmp187;struct _tuple17*_tmp185=(struct _tuple17*)((struct Cyc_List_List*)
_check_null(_tmp172->tl))->hd;_tmp186=*_tmp185;_LL270: _tmp187=_tmp186.f1;goto
_LL26F;_LL26F: if(_tmp187 < _tmp181){_tmp172=_tmp172->tl;continue;}}Cyc_Dump_dumpdecl(
_tmp17C,(struct _tagged_arr)_tmp184);Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stdout);
_tmp172=_tmp172->tl;_tmp179=_tmp179->tl;goto _LL268;_LL268:;}}}}}}}}}}}}struct
_tuple19{struct _tagged_arr f1;int f2;struct _tagged_arr f3;void*f4;struct _tagged_arr
f5;};int Cyc_main(int argc,struct _tagged_arr argv){struct Cyc_List_List*options=({
struct _tuple19*_tmp190[4];_tmp190[3]=({struct _tuple19*_tmp19A=_cycalloc(sizeof(*
_tmp19A));_tmp19A->f1=_tag_arr("-B",sizeof(unsigned char),3);_tmp19A->f2=1;
_tmp19A->f3=_tag_arr("<file>",sizeof(unsigned char),7);_tmp19A->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp19C;_tmp19C.tag=1;_tmp19C.f1=Cyc_add_cycarg;
_tmp19C;});_tmp19B;});_tmp19A->f5=_tag_arr("Add to the list of directories to search for compiler files",
sizeof(unsigned char),60);_tmp19A;});_tmp190[2]=({struct _tuple19*_tmp197=
_cycalloc(sizeof(*_tmp197));_tmp197->f1=_tag_arr("-I",sizeof(unsigned char),3);
_tmp197->f2=1;_tmp197->f3=_tag_arr("<dir>",sizeof(unsigned char),6);_tmp197->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp198=_cycalloc(sizeof(*_tmp198));
_tmp198[0]=({struct Cyc_Arg_Flag_spec_struct _tmp199;_tmp199.tag=1;_tmp199.f1=Cyc_add_cycarg;
_tmp199;});_tmp198;});_tmp197->f5=_tag_arr("Add to the list of directories to search for include files",
sizeof(unsigned char),59);_tmp197;});_tmp190[1]=({struct _tuple19*_tmp194=
_cycalloc(sizeof(*_tmp194));_tmp194->f1=_tag_arr("-D",sizeof(unsigned char),3);
_tmp194->f2=1;_tmp194->f3=_tag_arr("<name>[=<value>]",sizeof(unsigned char),17);
_tmp194->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp195=_cycalloc(sizeof(*
_tmp195));_tmp195[0]=({struct Cyc_Arg_Flag_spec_struct _tmp196;_tmp196.tag=1;
_tmp196.f1=Cyc_add_cycarg;_tmp196;});_tmp195;});_tmp194->f5=_tag_arr("Pass definition to preprocessor",
sizeof(unsigned char),32);_tmp194;});_tmp190[0]=({struct _tuple19*_tmp191=
_cycalloc(sizeof(*_tmp191));_tmp191->f1=_tag_arr("-cyclone",sizeof(unsigned char),
9);_tmp191->f2=0;_tmp191->f3=_tag_arr(" <file>",sizeof(unsigned char),8);_tmp191->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp192=_cycalloc(sizeof(*_tmp192));
_tmp192[0]=({struct Cyc_Arg_String_spec_struct _tmp193;_tmp193.tag=5;_tmp193.f1=
Cyc_set_cyclone_file;_tmp193;});_tmp192;});_tmp191->f5=_tag_arr("Use <file> as the cyclone compiler",
sizeof(unsigned char),35);_tmp191;});((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp190,sizeof(struct _tuple19*),4));});Cyc_Arg_parse(
options,Cyc_add_other,_tag_arr("Options:",sizeof(unsigned char),9),argv);{struct
Cyc_List_List*_tmp18F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cycdoc_files);for(0;_tmp18F != 0;_tmp18F=_tmp18F->tl){Cyc_process_file(*((
struct _tagged_arr*)_tmp18F->hd));}}return 0;}
