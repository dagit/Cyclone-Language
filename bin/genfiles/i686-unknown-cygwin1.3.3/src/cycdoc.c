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
 void exit(int);struct Cyc_Core_Opt{void*v;};struct _tagged_arr Cyc_Core_new_string(
unsigned int);struct _tuple0{void*f1;void*f2;};void*Cyc_Core_fst(struct _tuple0*);
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[21];struct
Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[
12];struct Cyc_Core_Failure_struct{char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{char*tag;struct _tagged_arr f1;};extern char
Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*
Cyc_Std_stdout;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;struct Cyc_Std_Cstdio___abstractFILE;
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_Std_fclose(struct Cyc_Std___cycFILE*);int Cyc_Std_fflush(
struct Cyc_Std___cycFILE*);struct Cyc_Std___cycFILE*Cyc_Std_fopen(struct
_tagged_arr,struct _tagged_arr);int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,
struct _tagged_arr,struct _tagged_arr);int Cyc_Std_fputc(int,struct Cyc_Std___cycFILE*);
int Cyc_Std_fputs(struct _tagged_arr,struct Cyc_Std___cycFILE*);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};int Cyc_Std_printf(struct _tagged_arr,struct _tagged_arr);int Cyc_Std_puts(
struct _tagged_arr);int Cyc_Std_remove(struct _tagged_arr);extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};void Cyc_Std_file_close(struct Cyc_Std___cycFILE*);
extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{char*tag;struct
_tagged_arr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _tagged_arr lex_buffer;int lex_buffer_len;int lex_abs_pos;
int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int
lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct
_tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct
_tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr lex_default;
struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file(struct Cyc_Std___cycFILE*);int Cyc_Lexing_lexeme_start(struct
Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct
Cyc_List_List*Cyc_List_list(struct _tagged_arr);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_List_List*Cyc_List_zip(
struct Cyc_List_List*x,struct Cyc_List_List*y);unsigned int Cyc_Std_strlen(struct
_tagged_arr s);int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);struct
_tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);struct
_tagged_arr Cyc_Std_strconcat_l(struct Cyc_List_List*);struct _tagged_arr Cyc_Std_str_sepstr(
struct Cyc_List_List*,struct _tagged_arr);struct _tagged_arr Cyc_Std_substring(
struct _tagged_arr,int ofs,unsigned int n);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};void Cyc_Lineno_poss_of_abss(
struct _tagged_arr filename,struct Cyc_List_List*places);struct _tagged_arr Cyc_Filename_chop_extension(
struct _tagged_arr);extern char Cyc_Position_Exit[9];void Cyc_Position_reset_position(
struct _tagged_arr);struct Cyc_Position_Segment;struct Cyc_Position_Error{struct
_tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;
};extern char Cyc_Position_Nocontext[14];typedef struct{int quot;int rem;}Cyc_Std_div_t;
typedef struct{int quot;int rem;}Cyc_Std_ldiv_t;int Cyc_Std_system(struct _tagged_arr);
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct _tagged_arr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);extern char Cyc_Arg_Bad[
8];struct Cyc_Arg_Bad_struct{char*tag;struct _tagged_arr f1;};extern char Cyc_Arg_Error[
10];struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};struct Cyc_Arg_FlagString_spec_struct{int
tag;void(*f1)(struct _tagged_arr,struct _tagged_arr);};struct Cyc_Arg_Set_spec_struct{
int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*
f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _tagged_arr);};
void Cyc_Arg_usage(struct Cyc_List_List*,struct _tagged_arr);void Cyc_Arg_parse(
struct Cyc_List_List*specs,void(*anonfun)(struct _tagged_arr),struct _tagged_arr
errmsg,struct _tagged_arr args);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple1{
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
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
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
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;};struct Cyc_Absyn_Aggrdecl{void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _tagged_arr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{
void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc_Std___cycFILE*f);struct Cyc_Declaration_spec;struct Cyc_Declarator;
struct Cyc_Abstractdeclarator;int Cyc_yyparse();extern char Cyc_AbstractDeclarator_tok[
27];struct Cyc_AbstractDeclarator_tok_struct{char*tag;struct Cyc_Abstractdeclarator*
f1;};extern char Cyc_AggrFieldDeclListList_tok[30];struct Cyc_AggrFieldDeclListList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_AggrFieldDeclList_tok[26];struct
Cyc_AggrFieldDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_AttributeList_tok[
22];struct Cyc_AttributeList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern
char Cyc_Attribute_tok[18];struct Cyc_Attribute_tok_struct{char*tag;void*f1;};
extern char Cyc_Bool_tok[13];struct Cyc_Bool_tok_struct{char*tag;int f1;};extern char
Cyc_Char_tok[13];struct Cyc_Char_tok_struct{char*tag;char f1;};extern char Cyc_DeclList_tok[
17];struct Cyc_DeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char
Cyc_DeclSpec_tok[17];struct Cyc_DeclSpec_tok_struct{char*tag;struct Cyc_Declaration_spec*
f1;};extern char Cyc_Declarator_tok[19];struct Cyc_Declarator_tok_struct{char*tag;
struct Cyc_Declarator*f1;};extern char Cyc_DesignatorList_tok[23];struct Cyc_DesignatorList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Designator_tok[19];struct Cyc_Designator_tok_struct{
char*tag;void*f1;};extern char Cyc_EnumfieldList_tok[22];struct Cyc_EnumfieldList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Enumfield_tok[18];struct Cyc_Enumfield_tok_struct{
char*tag;struct Cyc_Absyn_Enumfield*f1;};extern char Cyc_ExpList_tok[16];struct Cyc_ExpList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Exp_tok[12];struct Cyc_Exp_tok_struct{
char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_FieldPatternList_tok[25];struct
Cyc_FieldPatternList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_FieldPattern_tok[
21];struct _tuple4{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
char*tag;struct _tuple4*f1;};extern char Cyc_FnDecl_tok[15];struct Cyc_FnDecl_tok_struct{
char*tag;struct Cyc_Absyn_Fndecl*f1;};extern char Cyc_IdList_tok[15];struct Cyc_IdList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_InitDeclList_tok[21];struct Cyc_InitDeclList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_InitDecl_tok[17];struct _tuple5{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
char*tag;struct _tuple5*f1;};extern char Cyc_InitializerList_tok[24];struct Cyc_InitializerList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Int_tok[12];struct _tuple6{void*
f1;int f2;};struct Cyc_Int_tok_struct{char*tag;struct _tuple6*f1;};extern char Cyc_Kind_tok[
13];struct Cyc_Kind_tok_struct{char*tag;void*f1;};extern char Cyc_Okay_tok[13];
extern char Cyc_ParamDeclListBool_tok[26];struct _tuple7{struct Cyc_List_List*f1;int
f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};
struct Cyc_ParamDeclListBool_tok_struct{char*tag;struct _tuple7*f1;};extern char Cyc_ParamDeclList_tok[
22];struct Cyc_ParamDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern
char Cyc_ParamDecl_tok[18];struct Cyc_ParamDecl_tok_struct{char*tag;struct _tuple2*
f1;};extern char Cyc_PatternList_tok[20];struct Cyc_PatternList_tok_struct{char*tag;
struct Cyc_List_List*f1;};extern char Cyc_Pattern_tok[16];struct Cyc_Pattern_tok_struct{
char*tag;struct Cyc_Absyn_Pat*f1;};extern char Cyc_Pointer_Sort_tok[21];struct Cyc_Pointer_Sort_tok_struct{
char*tag;void*f1;};extern char Cyc_Primop_tok[15];struct Cyc_Primop_tok_struct{char*
tag;void*f1;};extern char Cyc_Primopopt_tok[18];struct Cyc_Primopopt_tok_struct{
char*tag;struct Cyc_Core_Opt*f1;};extern char Cyc_QualId_tok[15];struct Cyc_QualId_tok_struct{
char*tag;struct _tuple1*f1;};extern char Cyc_QualSpecList_tok[21];struct _tuple8{
struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
Cyc_QualSpecList_tok_struct{char*tag;struct _tuple8*f1;};extern char Cyc_Rgnorder_tok[
17];struct Cyc_Rgnorder_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char
Cyc_Scope_tok[14];struct Cyc_Scope_tok_struct{char*tag;void*f1;};extern char Cyc_Short_tok[
14];struct Cyc_Short_tok_struct{char*tag;short f1;};extern char Cyc_Stmt_tok[13];
struct Cyc_Stmt_tok_struct{char*tag;struct Cyc_Absyn_Stmt*f1;};extern char Cyc_StorageClass_tok[
21];struct Cyc_StorageClass_tok_struct{char*tag;void*f1;};extern char Cyc_String_tok[
15];struct Cyc_String_tok_struct{char*tag;struct _tagged_arr f1;};extern char Cyc_Stringopt_tok[
18];struct Cyc_Stringopt_tok_struct{char*tag;struct Cyc_Core_Opt*f1;};extern char
Cyc_StructOrUnion_tok[22];struct Cyc_StructOrUnion_tok_struct{char*tag;void*f1;};
extern char Cyc_SwitchCClauseList_tok[26];struct Cyc_SwitchCClauseList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_SwitchClauseList_tok[25];struct
Cyc_SwitchClauseList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_TunionFieldList_tok[
24];struct Cyc_TunionFieldList_tok_struct{char*tag;struct Cyc_List_List*f1;};
extern char Cyc_TunionField_tok[20];struct Cyc_TunionField_tok_struct{char*tag;
struct Cyc_Absyn_Tunionfield*f1;};extern char Cyc_TypeList_tok[17];struct Cyc_TypeList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_TypeModifierList_tok[25];struct
Cyc_TypeModifierList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_TypeOpt_tok[
16];struct Cyc_TypeOpt_tok_struct{char*tag;struct Cyc_Core_Opt*f1;};extern char Cyc_TypeQual_tok[
17];struct Cyc_TypeQual_tok_struct{char*tag;struct Cyc_Absyn_Tqual f1;};extern char
Cyc_TypeSpecifier_tok[22];struct Cyc_TypeSpecifier_tok_struct{char*tag;void*f1;};
extern char Cyc_Type_tok[13];struct Cyc_Type_tok_struct{char*tag;void*f1;};int Cyc_yyparse();
extern char Cyc_YY1[8];struct Cyc_YY1_struct{char*tag;struct _tuple7*f1;};extern char
Cyc_YYINITIALSVAL[18];struct Cyc_Yyltype{int timestamp;int first_line;int
first_column;int last_line;int last_column;};struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);
void Cyc_Lex_lex_init();struct Cyc_Position_Segment{int start;int end;};struct Cyc_MatchDecl_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Standalone_struct{int tag;struct
_tagged_arr f1;};struct _tuple9{int f1;void*f2;};struct _tuple9*Cyc_token(struct Cyc_Lexing_lexbuf*
lexbuf);const int Cyc_lex_base[15]=(const int[15]){0,- 4,0,- 3,1,2,3,0,4,6,7,- 1,8,9,-
2};const int Cyc_lex_backtrk[15]=(const int[15]){- 1,- 1,3,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1};const int Cyc_lex_default[15]=(const int[15]){1,0,- 1,0,- 1,- 1,6,- 1,8,8,8,0,
6,6,0};const int Cyc_lex_trans[266]=(const int[266]){0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,6,0,0,0,0,0,0,0,4,5,7,12,9,2,10,10,13,13,0,
11,- 1,14,- 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,3,0,0,- 1,- 1,0,- 1,- 1,- 1,- 1};const int Cyc_lex_check[266]=(const int[266]){
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,7,- 1,5,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,4,5,6,8,0,9,10,12,13,- 1,9,10,12,13,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,6,8,- 1,9,10,12,13};int Cyc_lex_engine(int
start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;state=
start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){base=Cyc_lex_base[
_check_known_subscript_notnull(15,state)];if(base < 0)return(- base)- 1;backtrk=
Cyc_lex_backtrk[_check_known_subscript_notnull(15,state)];if(backtrk >= 0){lbuf->lex_last_pos=
lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}else{c=(int)*((char*)
_check_unknown_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}if(Cyc_lex_check[_check_known_subscript_notnull(266,base + c)]== 
state)state=Cyc_lex_trans[_check_known_subscript_notnull(266,base + c)];else{
state=Cyc_lex_default[_check_known_subscript_notnull(15,state)];}if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;if(lbuf->lex_last_action == - 1)(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({
struct Cyc_Lexing_Error_struct _tmp1;_tmp1.tag=Cyc_Lexing_Error;_tmp1.f1=_tag_arr("empty token",
sizeof(char),12);_tmp1;});_tmp0;}));else{return lbuf->lex_last_action;}}else{if(c
== 256)lbuf->lex_eof_reached=0;}}}struct _tuple9*Cyc_token_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL0: {int _tmp2=lexbuf->lex_start_pos + 5;int _tmp3=(lexbuf->lex_curr_pos - 
lexbuf->lex_start_pos)- 7;return({struct _tuple9*_tmp4=_cycalloc(sizeof(*_tmp4));
_tmp4->f1=Cyc_Lexing_lexeme_start(lexbuf);_tmp4->f2=(void*)({struct Cyc_Standalone_struct*
_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=({struct Cyc_Standalone_struct _tmp6;
_tmp6.tag=1;_tmp6.f1=Cyc_Std_substring((struct _tagged_arr)lexbuf->lex_buffer,
_tmp2,(unsigned int)_tmp3);_tmp6;});_tmp5;});_tmp4;});}case 1: _LL1: {int _tmp7=
lexbuf->lex_start_pos + 4;int _tmp8=(lexbuf->lex_curr_pos - lexbuf->lex_start_pos)- 
6;return({struct _tuple9*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->f1=Cyc_Lexing_lexeme_start(
lexbuf);_tmp9->f2=(void*)({struct Cyc_MatchDecl_struct*_tmpA=_cycalloc(sizeof(*
_tmpA));_tmpA[0]=({struct Cyc_MatchDecl_struct _tmpB;_tmpB.tag=0;_tmpB.f1=Cyc_Std_substring((
struct _tagged_arr)lexbuf->lex_buffer,_tmp7,(unsigned int)_tmp8);_tmpB;});_tmpA;});
_tmp9;});}case 2: _LL2: return 0;case 3: _LL3: return Cyc_token(lexbuf);default: _LL4:(
lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(lexbuf,lexstate);}(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC[0]=({
struct Cyc_Lexing_Error_struct _tmpD;_tmpD.tag=Cyc_Lexing_Error;_tmpD.f1=_tag_arr("some action didn't return!",
sizeof(char),27);_tmpD;});_tmpC;}));}struct _tuple9*Cyc_token(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_token_rec(lexbuf,0);}struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*
r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
ae;struct Cyc_Core_Opt*le;};static void Cyc_dump_begin(){Cyc_Std_puts(_tag_arr("%%HEVEA \\begin{latexonly}\n\\begin{list}{}{\\setlength\\itemsep{0pt}\\setlength\\topsep{0pt}\\setlength\\leftmargin{\\parindent}\\setlength\\itemindent{-\\leftmargin}}\\item[]\\colorbox{cycdochighlight}{\\ttfamily\\begin{minipage}[b]{\\textwidth}\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}<dl><dt><table><tr><td bgcolor=\"c0d0ff\">\\end{rawhtml}\n\\begin{tabbing}\n",
sizeof(char),348));}static void Cyc_dump_middle(){Cyc_Std_puts(_tag_arr("\\end{tabbing}\n%%HEVEA \\begin{latexonly}\n\\end{minipage}}\\\\\\strut\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</td></tr></table><dd>\\end{rawhtml}\n",
sizeof(char),148));}static void Cyc_dump_end(){Cyc_Std_puts(_tag_arr("%%HEVEA \\begin{latexonly}\n\\end{list}\\smallskip\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</dl>\\end{rawhtml}\n",
sizeof(char),114));}static void Cyc_pr_comment(struct Cyc_Std___cycFILE*outf,struct
_tagged_arr s){int depth=0;int len=(int)Cyc_Std_strlen(s);int i=0;for(0;i < len;i ++){
char c=*((const char*)_check_unknown_subscript(s,sizeof(char),i));if(c != '['){Cyc_Std_fputc((
int)c,outf);continue;}Cyc_Std_fputs(_tag_arr("\\texttt{",sizeof(char),9),outf);i
++;depth ++;for(0;i < len;i ++){char c=*((const char*)_check_unknown_subscript(s,
sizeof(char),i));if(c == ']'){depth --;if(depth == 0){Cyc_Std_fputc((int)'}',outf);
break;}}else{if(c == '[')depth ++;}Cyc_Std_fputc((int)c,outf);}}}static int Cyc_width=
50;static void Cyc_set_width(int w){Cyc_width=w;}static struct Cyc_List_List*Cyc_cycdoc_files=
0;static void Cyc_add_other(struct _tagged_arr s){Cyc_cycdoc_files=({struct Cyc_List_List*
_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->hd=({struct _tagged_arr*_tmpF=_cycalloc(
sizeof(*_tmpF));_tmpF[0]=s;_tmpF;});_tmpE->tl=Cyc_cycdoc_files;_tmpE;});}static
struct Cyc_List_List*Cyc_cycargs=0;static void Cyc_add_cycarg(struct _tagged_arr s){
Cyc_cycargs=({struct Cyc_List_List*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->hd=({
struct _tagged_arr*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11[0]=s;_tmp11;});_tmp10->tl=
Cyc_cycargs;_tmp10;});}static char _tmp12[8]="cyclone";static struct _tagged_arr Cyc_cyclone_file={
_tmp12,_tmp12,_tmp12 + 8};static void Cyc_set_cyclone_file(struct _tagged_arr s){Cyc_cyclone_file=
s;}static void Cyc_dumpdecl(struct Cyc_Absyn_Decl*d,struct _tagged_arr comment){Cyc_dump_begin();({
struct Cyc_Std_String_pa_struct _tmp14;_tmp14.tag=0;_tmp14.f1=(struct _tagged_arr)
Cyc_PP_string_of_doc(Cyc_Absynpp_decl2doc(d),50);{void*_tmp13[1]={& _tmp14};Cyc_Std_printf(
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp13,sizeof(void*),1));}});Cyc_dump_middle();
Cyc_pr_comment(Cyc_Std_stdout,comment);({void*_tmp15[0]={};Cyc_Std_printf(
_tag_arr("\n",sizeof(char),2),_tag_arr(_tmp15,sizeof(void*),0));});Cyc_dump_end();}
static int Cyc_is_other_special(char c){switch(c){case '\\': _LL6: goto _LL7;case '"':
_LL7: goto _LL8;case ';': _LL8: goto _LL9;case '&': _LL9: goto _LLA;case '(': _LLA: goto _LLB;
case ')': _LLB: goto _LLC;case '|': _LLC: goto _LLD;case '^': _LLD: goto _LLE;case '<': _LLE:
goto _LLF;case '>': _LLF: goto _LL10;case ' ': _LL10: goto _LL11;case '\n': _LL11: goto _LL12;
case '\t': _LL12: return 1;default: _LL13: return 0;}}static struct _tagged_arr Cyc_sh_escape_string(
struct _tagged_arr s){unsigned int _tmp16=Cyc_Std_strlen(s);int _tmp17=0;int _tmp18=0;{
int i=0;for(0;i < _tmp16;i ++){char _tmp19=*((const char*)_check_unknown_subscript(s,
sizeof(char),i));if(_tmp19 == '\'')_tmp17 ++;else{if(Cyc_is_other_special(_tmp19))
_tmp18 ++;}}}if(_tmp17 == 0?_tmp18 == 0: 0)return s;if(_tmp17 == 0)return(struct
_tagged_arr)Cyc_Std_strconcat_l(({struct _tagged_arr*_tmp1A[3];_tmp1A[2]=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",sizeof(char),2);_tmp1A[1]=({
struct _tagged_arr*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=(struct _tagged_arr)
s;_tmp1C;});_tmp1A[0]=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",
sizeof(char),2);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1A,sizeof(struct _tagged_arr*),3));}));{unsigned int _tmp1E=(_tmp16 + 
_tmp17)+ _tmp18;struct _tagged_arr _tmp1F=({unsigned int _tmp23=_tmp1E + 1;char*
_tmp24=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp23));struct
_tagged_arr _tmp26=_tag_arr(_tmp24,sizeof(char),_tmp1E + 1);{unsigned int _tmp25=
_tmp23;unsigned int i;for(i=0;i < _tmp25;i ++){_tmp24[i]='\000';}}_tmp26;});int
_tmp20=0;int _tmp21=0;for(0;_tmp20 < _tmp16;_tmp20 ++){char _tmp22=*((const char*)
_check_unknown_subscript(s,sizeof(char),_tmp20));if(_tmp22 == '\''?1: Cyc_is_other_special(
_tmp22))*((char*)_check_unknown_subscript(_tmp1F,sizeof(char),_tmp21 ++))='\\';*((
char*)_check_unknown_subscript(_tmp1F,sizeof(char),_tmp21 ++))=_tmp22;}return(
struct _tagged_arr)_tmp1F;}}static struct _tagged_arr*Cyc_sh_escape_stringptr(
struct _tagged_arr*sp){return({struct _tagged_arr*_tmp27=_cycalloc(sizeof(*_tmp27));
_tmp27[0]=Cyc_sh_escape_string(*sp);_tmp27;});}static struct Cyc_Lineno_Pos*Cyc_new_pos(){
return({struct Cyc_Lineno_Pos*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->logical_file=
_tag_arr("",sizeof(char),1);_tmp28->line=Cyc_Core_new_string(0);_tmp28->line_no=
0;_tmp28->col=0;_tmp28;});}struct _tuple10{int f1;struct Cyc_Lineno_Pos*f2;};static
struct _tuple10*Cyc_start2pos(int x){return({struct _tuple10*_tmp29=_cycalloc(
sizeof(*_tmp29));_tmp29->f1=x;_tmp29->f2=Cyc_new_pos();_tmp29;});}static int Cyc_decl2start(
struct Cyc_Absyn_Decl*d){return((struct Cyc_Position_Segment*)_check_null(d->loc))->start;}
struct _tuple11{struct Cyc_Lineno_Pos*f1;void*f2;};static struct Cyc_List_List*Cyc_this_file(
struct _tagged_arr file,struct Cyc_List_List*x){struct Cyc_List_List*_tmp2A=0;for(0;
x != 0;x=x->tl){if(Cyc_Std_strcmp(((*((struct _tuple11*)x->hd)).f1)->logical_file,
file)== 0)_tmp2A=({struct Cyc_List_List*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->hd=(
struct _tuple11*)x->hd;_tmp2B->tl=_tmp2A;_tmp2B;});}_tmp2A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2A);return _tmp2A;}static int Cyc_lineno(
struct Cyc_Lineno_Pos*p){return p->line_no;}static struct Cyc_List_List*Cyc_flatten_decls(
struct Cyc_List_List*decls){struct Cyc_List_List*_tmp2C=0;while(decls != 0){void*
_tmp2D=(void*)((struct Cyc_Absyn_Decl*)decls->hd)->r;struct Cyc_List_List*_tmp2E;
struct Cyc_List_List*_tmp2F;struct Cyc_List_List*_tmp30;_LL16: if(*((int*)_tmp2D)!= 
8)goto _LL18;_tmp2E=((struct Cyc_Absyn_Namespace_d_struct*)_tmp2D)->f2;_LL17:
_tmp2F=_tmp2E;goto _LL19;_LL18: if(*((int*)_tmp2D)!= 9)goto _LL1A;_tmp2F=((struct
Cyc_Absyn_Using_d_struct*)_tmp2D)->f2;_LL19: _tmp30=_tmp2F;goto _LL1B;_LL1A: if(*((
int*)_tmp2D)!= 10)goto _LL1C;_tmp30=((struct Cyc_Absyn_ExternC_d_struct*)_tmp2D)->f1;
_LL1B: decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp30,decls->tl);goto _LL15;_LL1C: if(*((int*)_tmp2D)!= 4)
goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp2D)!= 0)goto _LL20;_LL1F: goto _LL21;
_LL20: if(*((int*)_tmp2D)!= 5)goto _LL22;_LL21: goto _LL23;_LL22: if(*((int*)_tmp2D)
!= 6)goto _LL24;_LL23: goto _LL25;_LL24: if(*((int*)_tmp2D)!= 7)goto _LL26;_LL25: goto
_LL27;_LL26: if(*((int*)_tmp2D)!= 1)goto _LL28;_LL27: goto _LL29;_LL28: if(*((int*)
_tmp2D)!= 2)goto _LL2A;_LL29: goto _LL2B;_LL2A: if(*((int*)_tmp2D)!= 3)goto _LL15;
_LL2B: _tmp2C=({struct Cyc_List_List*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->hd=(
struct Cyc_Absyn_Decl*)decls->hd;_tmp31->tl=_tmp2C;_tmp31;});decls=decls->tl;goto
_LL15;_LL15:;}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp2C);}struct _tuple12{int f1;struct Cyc_Absyn_Decl*f2;};struct _tuple13{struct Cyc_Lineno_Pos*
f1;struct Cyc_Absyn_Decl*f2;};static void Cyc_process_file(struct _tagged_arr
filename){struct _tagged_arr _tmp32=Cyc_Filename_chop_extension(filename);struct
_tagged_arr _tmp33=Cyc_Std_strconcat((struct _tagged_arr)_tmp32,_tag_arr(".cyp",
sizeof(char),5));struct _tagged_arr _tmp34=Cyc_Std_str_sepstr(({struct Cyc_List_List*
_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->hd=_init_tag_arr(_cycalloc(sizeof(
struct _tagged_arr)),"",sizeof(char),1);_tmp56->tl=((struct Cyc_List_List*(*)(
struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cycargs));_tmp56;}),_tag_arr(" ",sizeof(char),2));struct _tagged_arr _tmp35=({
struct Cyc_Std_String_pa_struct _tmp55;_tmp55.tag=0;_tmp55.f1=(struct _tagged_arr)
Cyc_sh_escape_string(filename);{struct Cyc_Std_String_pa_struct _tmp54;_tmp54.tag=
0;_tmp54.f1=(struct _tagged_arr)Cyc_sh_escape_string((struct _tagged_arr)_tmp33);{
struct Cyc_Std_String_pa_struct _tmp53;_tmp53.tag=0;_tmp53.f1=(struct _tagged_arr)
_tmp34;{struct Cyc_Std_String_pa_struct _tmp52;_tmp52.tag=0;_tmp52.f1=(struct
_tagged_arr)Cyc_cyclone_file;{void*_tmp51[4]={& _tmp52,& _tmp53,& _tmp54,& _tmp55};
Cyc_Std_aprintf(_tag_arr("%s %s -E -o %s -x cyc %s",sizeof(char),25),_tag_arr(
_tmp51,sizeof(void*),4));}}}}});if(Cyc_Std_system((struct _tagged_arr)_tmp35)!= 0){({
void*_tmp36[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\nError: preprocessing\n",
sizeof(char),23),_tag_arr(_tmp36,sizeof(void*),0));});return;}Cyc_Position_reset_position((
struct _tagged_arr)_tmp33);{struct Cyc_Std___cycFILE*in_file=(struct Cyc_Std___cycFILE*)
_check_null(Cyc_Std_fopen((struct _tagged_arr)_tmp33,_tag_arr("r",sizeof(char),2)));
Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp37=Cyc_Parse_parse_file(in_file);Cyc_Lex_lex_init();
Cyc_Std_file_close((struct Cyc_Std___cycFILE*)in_file);_tmp37=Cyc_flatten_decls(
_tmp37);{struct Cyc_List_List*_tmp38=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(
int),struct Cyc_List_List*x))Cyc_List_map)(Cyc_start2pos,((struct Cyc_List_List*(*)(
int(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_decl2start,
_tmp37));Cyc_Lineno_poss_of_abss((struct _tagged_arr)_tmp33,_tmp38);Cyc_Std_remove((
struct _tagged_arr)_tmp33);{struct Cyc_List_List*_tmp39=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(
struct Cyc_Lineno_Pos*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)(struct _tuple10*))Cyc_Core_snd,_tmp38),_tmp37);_tmp39=((
struct Cyc_List_List*(*)(struct _tagged_arr file,struct Cyc_List_List*x))Cyc_this_file)(
filename,_tmp39);{struct Cyc_List_List*_tmp3A=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct
Cyc_Lineno_Pos*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(
struct Cyc_Lineno_Pos*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)(struct _tuple13*))Cyc_Core_fst,_tmp39)),((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Decl*(*)(struct _tuple13*))Cyc_Core_snd,_tmp39));struct Cyc_Std___cycFILE*
_tmp3B=(struct Cyc_Std___cycFILE*)_check_null(Cyc_Std_fopen(filename,_tag_arr("r",
sizeof(char),2)));struct Cyc_Lexing_lexbuf*_tmp3C=Cyc_Lexing_from_file(_tmp3B);
struct Cyc_List_List*_tmp3D=0;struct _tuple9*comment;while((comment=((struct
_tuple9*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_token)(_tmp3C))!= 0){_tmp3D=({
struct Cyc_List_List*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->hd=(struct _tuple9*)
_check_null(comment);_tmp3E->tl=_tmp3D;_tmp3E;});}Cyc_Std_fclose(_tmp3B);_tmp3D=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3D);{struct
Cyc_List_List*_tmp3F=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(int),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_start2pos,((struct Cyc_List_List*(*)(int(*f)(
struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)((int(*)(struct _tuple9*))Cyc_Core_fst,
_tmp3D));Cyc_Lineno_poss_of_abss(filename,_tmp3F);{struct Cyc_List_List*_tmp40=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((
struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*f)(struct _tuple10*),struct Cyc_List_List*
x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(struct _tuple10*))Cyc_Core_snd,_tmp3F),((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple9*))Cyc_Core_snd,_tmp3D));_tmp40=Cyc_this_file(filename,
_tmp40);{struct Cyc_List_List*_tmp41=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct
Cyc_Lineno_Pos*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(
struct Cyc_Lineno_Pos*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)(struct _tuple11*))Cyc_Core_fst,_tmp40)),((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple11*))Cyc_Core_snd,_tmp40));while(_tmp3A != 0?_tmp41 != 0: 0){struct _tuple12
_tmp43;int _tmp44;struct Cyc_Absyn_Decl*_tmp45;struct _tuple12*_tmp42=(struct
_tuple12*)_tmp3A->hd;_tmp43=*_tmp42;_tmp44=_tmp43.f1;_tmp45=_tmp43.f2;{struct
_tuple9 _tmp47;int _tmp48;void*_tmp49;struct _tuple9*_tmp46=(struct _tuple9*)_tmp41->hd;
_tmp47=*_tmp46;_tmp48=_tmp47.f1;_tmp49=_tmp47.f2;{void*_tmp4A=_tmp49;struct
_tagged_arr _tmp4B;struct _tagged_arr _tmp4C;_LL2D: if(*((int*)_tmp4A)!= 1)goto _LL2F;
_tmp4B=((struct Cyc_Standalone_struct*)_tmp4A)->f1;_LL2E: Cyc_pr_comment(Cyc_Std_stdout,(
struct _tagged_arr)_tmp4B);({void*_tmp4D[0]={};Cyc_Std_printf(_tag_arr("\n",
sizeof(char),2),_tag_arr(_tmp4D,sizeof(void*),0));});_tmp41=_tmp41->tl;goto _LL2C;
_LL2F: if(*((int*)_tmp4A)!= 0)goto _LL2C;_tmp4C=((struct Cyc_MatchDecl_struct*)
_tmp4A)->f1;_LL30: if(_tmp48 < _tmp44){_tmp41=_tmp41->tl;continue;}if(_tmp3A->tl != 
0){struct _tuple12 _tmp4F;int _tmp50;struct _tuple12*_tmp4E=(struct _tuple12*)((
struct Cyc_List_List*)_check_null(_tmp3A->tl))->hd;_tmp4F=*_tmp4E;_tmp50=_tmp4F.f1;
if(_tmp50 < _tmp48){_tmp3A=_tmp3A->tl;continue;}}Cyc_dumpdecl(_tmp45,(struct
_tagged_arr)_tmp4C);Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stdout);
_tmp3A=_tmp3A->tl;_tmp41=_tmp41->tl;goto _LL2C;_LL2C:;}}}}}}}}}}}}void
GC_blacklist_warn_clear();struct _tuple14{struct _tagged_arr f1;int f2;struct
_tagged_arr f3;void*f4;struct _tagged_arr f5;};int Cyc_main(int argc,struct
_tagged_arr argv){GC_blacklist_warn_clear();{struct Cyc_List_List*options=({struct
_tuple14*_tmp59[5];_tmp59[4]=({struct _tuple14*_tmp66=_cycalloc(sizeof(*_tmp66));
_tmp66->f1=_tag_arr("-B",sizeof(char),3);_tmp66->f2=1;_tmp66->f3=_tag_arr("<file>",
sizeof(char),7);_tmp66->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp67=
_cycalloc(sizeof(*_tmp67));_tmp67[0]=({struct Cyc_Arg_Flag_spec_struct _tmp68;
_tmp68.tag=1;_tmp68.f1=Cyc_add_cycarg;_tmp68;});_tmp67;});_tmp66->f5=_tag_arr("Add to the list of directories to search for compiler files",
sizeof(char),60);_tmp66;});_tmp59[3]=({struct _tuple14*_tmp63=_cycalloc(sizeof(*
_tmp63));_tmp63->f1=_tag_arr("-I",sizeof(char),3);_tmp63->f2=1;_tmp63->f3=
_tag_arr("<dir>",sizeof(char),6);_tmp63->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp65;_tmp65.tag=1;_tmp65.f1=Cyc_add_cycarg;_tmp65;});_tmp64;});_tmp63->f5=
_tag_arr("Add to the list of directories to search for include files",sizeof(
char),59);_tmp63;});_tmp59[2]=({struct _tuple14*_tmp60=_cycalloc(sizeof(*_tmp60));
_tmp60->f1=_tag_arr("-D",sizeof(char),3);_tmp60->f2=1;_tmp60->f3=_tag_arr("<name>[=<value>]",
sizeof(char),17);_tmp60->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp61=
_cycalloc(sizeof(*_tmp61));_tmp61[0]=({struct Cyc_Arg_Flag_spec_struct _tmp62;
_tmp62.tag=1;_tmp62.f1=Cyc_add_cycarg;_tmp62;});_tmp61;});_tmp60->f5=_tag_arr("Pass definition to preprocessor",
sizeof(char),32);_tmp60;});_tmp59[1]=({struct _tuple14*_tmp5D=_cycalloc(sizeof(*
_tmp5D));_tmp5D->f1=_tag_arr("-w",sizeof(char),3);_tmp5D->f2=0;_tmp5D->f3=
_tag_arr(" <width>",sizeof(char),9);_tmp5D->f4=(void*)({struct Cyc_Arg_Int_spec_struct*
_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E[0]=({struct Cyc_Arg_Int_spec_struct
_tmp5F;_tmp5F.tag=6;_tmp5F.f1=Cyc_set_width;_tmp5F;});_tmp5E;});_tmp5D->f5=
_tag_arr("Use <width> as the max width for printing declarations",sizeof(char),
55);_tmp5D;});_tmp59[0]=({struct _tuple14*_tmp5A=_cycalloc(sizeof(*_tmp5A));
_tmp5A->f1=_tag_arr("-cyclone",sizeof(char),9);_tmp5A->f2=0;_tmp5A->f3=_tag_arr(" <file>",
sizeof(char),8);_tmp5A->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp5B=
_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({struct Cyc_Arg_String_spec_struct _tmp5C;
_tmp5C.tag=5;_tmp5C.f1=Cyc_set_cyclone_file;_tmp5C;});_tmp5B;});_tmp5A->f5=
_tag_arr("Use <file> as the cyclone compiler",sizeof(char),35);_tmp5A;});((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp59,
sizeof(struct _tuple14*),5));});Cyc_Arg_parse(options,Cyc_add_other,_tag_arr("Options:",
sizeof(char),9),argv);if(Cyc_cycdoc_files == 0){Cyc_Arg_usage(options,_tag_arr("Usage: cycdoc [options] file1 file2 ...\nOptions:",
sizeof(char),49));exit(1);}Cyc_PP_tex_output=1;{struct Cyc_List_List*_tmp58=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cycdoc_files);
for(0;_tmp58 != 0;_tmp58=_tmp58->tl){Cyc_process_file(*((struct _tagged_arr*)
_tmp58->hd));}}return 0;}}
