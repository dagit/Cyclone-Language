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
 struct Cyc_Core_Opt{void*v;};struct _tagged_arr Cyc_Core_new_string(unsigned int);
struct _tuple0{void*f1;void*f2;};void*Cyc_Core_fst(struct _tuple0*);void*Cyc_Core_snd(
struct _tuple0*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stdout;extern
struct Cyc_Std___cycFILE*Cyc_Std_stderr;int Cyc_Std_remove(struct _tagged_arr);int
Cyc_Std_fclose(struct Cyc_Std___cycFILE*);int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);
struct Cyc_Std___cycFILE*Cyc_Std_fopen(struct _tagged_arr __filename,struct
_tagged_arr __modes);int Cyc_Std_fputc(int __c,struct Cyc_Std___cycFILE*__stream);
int Cyc_Std_fputs(struct _tagged_arr __s,struct Cyc_Std___cycFILE*__stream);extern
char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};void Cyc_Std_file_close(struct Cyc_Std___cycFILE*);
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr);int Cyc_Std_printf(struct _tagged_arr fmt,struct
_tagged_arr);struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct
_tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};extern char Cyc_Lexing_Error[
10];struct Cyc_Lexing_Error_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file(struct Cyc_Std___cycFILE*);int Cyc_Lexing_lexeme_start(struct
Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct
Cyc_List_List*Cyc_List_list(struct _tagged_arr);int Cyc_List_length(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
unsigned int Cyc_Std_strlen(struct _tagged_arr s);int Cyc_Std_strcmp(struct
_tagged_arr s1,struct _tagged_arr s2);struct _tagged_arr Cyc_Std_strconcat(struct
_tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_Std_strconcat_l(struct Cyc_List_List*);
struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,struct _tagged_arr);
struct _tagged_arr Cyc_Std_substring(struct _tagged_arr,int ofs,unsigned int n);
struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
line_no;int col;};void Cyc_Lineno_poss_of_abss(struct _tagged_arr filename,struct Cyc_List_List*
places);struct _tagged_arr Cyc_Filename_chop_extension(struct _tagged_arr);extern
char Cyc_Position_Exit[9];void Cyc_Position_reset_position(struct _tagged_arr);
struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[
14];extern char Cyc_Array_Array_mismatch[19];struct Cyc_Std__Div{int quot;int rem;};
struct Cyc_Std__Ldiv{int quot;int rem;};int Cyc_Std_system(struct _tagged_arr);struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;extern char Cyc_Arg_Bad[8];struct
Cyc_Arg_Bad_struct{char*tag;struct _tagged_arr f1;};extern char Cyc_Arg_Error[10];
struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};struct Cyc_Arg_FlagString_spec_struct{int
tag;void(*f1)(struct _tagged_arr,struct _tagged_arr);};struct Cyc_Arg_Set_spec_struct{
int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*
f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _tagged_arr);};
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _tagged_arr),
struct _tagged_arr errmsg,struct _tagged_arr args);struct Cyc_Absyn_Rel_n_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
struct _tuple4{void*f1;struct _tuple1*f2;};struct _tuple4 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc_Std___cycFILE*f);
struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;int
Cyc_yyparse();extern char Cyc_AbstractDeclarator_tok[27];struct Cyc_AbstractDeclarator_tok_struct{
char*tag;struct Cyc_Abstractdeclarator*f1;};extern char Cyc_AggrFieldDeclListList_tok[
30];struct Cyc_AggrFieldDeclListList_tok_struct{char*tag;struct Cyc_List_List*f1;}
;extern char Cyc_AggrFieldDeclList_tok[26];struct Cyc_AggrFieldDeclList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_AttributeList_tok[22];struct Cyc_AttributeList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Attribute_tok[18];struct Cyc_Attribute_tok_struct{
char*tag;void*f1;};extern char Cyc_Bool_tok[13];struct Cyc_Bool_tok_struct{char*tag;
int f1;};extern char Cyc_Char_tok[13];struct Cyc_Char_tok_struct{char*tag;char f1;};
extern char Cyc_DeclList_tok[17];struct Cyc_DeclList_tok_struct{char*tag;struct Cyc_List_List*
f1;};extern char Cyc_DeclSpec_tok[17];struct Cyc_DeclSpec_tok_struct{char*tag;
struct Cyc_Declaration_spec*f1;};extern char Cyc_Declarator_tok[19];struct Cyc_Declarator_tok_struct{
char*tag;struct Cyc_Declarator*f1;};extern char Cyc_DesignatorList_tok[23];struct
Cyc_DesignatorList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_Designator_tok[
19];struct Cyc_Designator_tok_struct{char*tag;void*f1;};extern char Cyc_EnumfieldList_tok[
22];struct Cyc_EnumfieldList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern
char Cyc_Enumfield_tok[18];struct Cyc_Enumfield_tok_struct{char*tag;struct Cyc_Absyn_Enumfield*
f1;};extern char Cyc_ExpList_tok[16];struct Cyc_ExpList_tok_struct{char*tag;struct
Cyc_List_List*f1;};extern char Cyc_Exp_tok[12];struct Cyc_Exp_tok_struct{char*tag;
struct Cyc_Absyn_Exp*f1;};extern char Cyc_FieldPatternList_tok[25];struct Cyc_FieldPatternList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_FieldPattern_tok[21];struct
_tuple5{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
char*tag;struct _tuple5*f1;};extern char Cyc_FnDecl_tok[15];struct Cyc_FnDecl_tok_struct{
char*tag;struct Cyc_Absyn_Fndecl*f1;};extern char Cyc_IdList_tok[15];struct Cyc_IdList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_InitDeclList_tok[21];struct Cyc_InitDeclList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_InitDecl_tok[17];struct _tuple6{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
char*tag;struct _tuple6*f1;};extern char Cyc_InitializerList_tok[24];struct Cyc_InitializerList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Int_tok[12];struct _tuple7{void*
f1;int f2;};struct Cyc_Int_tok_struct{char*tag;struct _tuple7*f1;};extern char Cyc_Kind_tok[
13];struct Cyc_Kind_tok_struct{char*tag;void*f1;};extern char Cyc_Okay_tok[13];
extern char Cyc_ParamDeclListBool_tok[26];struct _tuple8{struct Cyc_List_List*f1;int
f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};
struct Cyc_ParamDeclListBool_tok_struct{char*tag;struct _tuple8*f1;};extern char Cyc_ParamDeclList_tok[
22];struct Cyc_ParamDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern
char Cyc_ParamDecl_tok[18];struct Cyc_ParamDecl_tok_struct{char*tag;struct _tuple2*
f1;};extern char Cyc_PatternList_tok[20];struct Cyc_PatternList_tok_struct{char*tag;
struct Cyc_List_List*f1;};extern char Cyc_Pattern_tok[16];struct Cyc_Pattern_tok_struct{
char*tag;struct Cyc_Absyn_Pat*f1;};extern char Cyc_Pointer_Sort_tok[21];struct Cyc_Pointer_Sort_tok_struct{
char*tag;void*f1;};extern char Cyc_Primop_tok[15];struct Cyc_Primop_tok_struct{char*
tag;void*f1;};extern char Cyc_Primopopt_tok[18];struct Cyc_Primopopt_tok_struct{
char*tag;struct Cyc_Core_Opt*f1;};extern char Cyc_QualId_tok[15];struct Cyc_QualId_tok_struct{
char*tag;struct _tuple1*f1;};extern char Cyc_QualSpecList_tok[21];struct _tuple9{
struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
Cyc_QualSpecList_tok_struct{char*tag;struct _tuple9*f1;};extern char Cyc_Rgnorder_tok[
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
extern char Cyc_YY1[8];struct Cyc_YY1_struct{char*tag;struct _tuple8*f1;};extern char
Cyc_YYINITIALSVAL[18];struct Cyc_Yyltype{int timestamp;int first_line;int
first_column;int last_line;int last_column;};struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern struct _tagged_arr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*);struct _tagged_arr Cyc_Absynpp_char_escape(char);struct
_tagged_arr Cyc_Absynpp_string_escape(struct _tagged_arr);struct _tagged_arr Cyc_Absynpp_prim2str(
void*p);struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
struct _tuple10 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t);void Cyc_Lex_lex_init();
struct Cyc_Position_Segment{int start;int end;};struct Cyc_MatchDecl_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Standalone_struct{int tag;struct _tagged_arr f1;};
struct _tuple11{int f1;void*f2;};struct _tuple11*Cyc_token(struct Cyc_Lexing_lexbuf*
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
== 256)lbuf->lex_eof_reached=0;}}}struct _tuple11*Cyc_token_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL0: {int _tmp2=lexbuf->lex_start_pos + 5;int _tmp3=(lexbuf->lex_curr_pos - 
lexbuf->lex_start_pos)- 7;return({struct _tuple11*_tmp4=_cycalloc(sizeof(*_tmp4));
_tmp4->f1=Cyc_Lexing_lexeme_start(lexbuf);_tmp4->f2=(void*)({struct Cyc_Standalone_struct*
_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=({struct Cyc_Standalone_struct _tmp6;
_tmp6.tag=1;_tmp6.f1=Cyc_Std_substring((struct _tagged_arr)lexbuf->lex_buffer,
_tmp2,(unsigned int)_tmp3);_tmp6;});_tmp5;});_tmp4;});}case 1: _LL1: {int _tmp7=
lexbuf->lex_start_pos + 4;int _tmp8=(lexbuf->lex_curr_pos - lexbuf->lex_start_pos)- 
6;return({struct _tuple11*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->f1=Cyc_Lexing_lexeme_start(
lexbuf);_tmp9->f2=(void*)({struct Cyc_MatchDecl_struct*_tmpA=_cycalloc(sizeof(*
_tmpA));_tmpA[0]=({struct Cyc_MatchDecl_struct _tmpB;_tmpB.tag=0;_tmpB.f1=Cyc_Std_substring((
struct _tagged_arr)lexbuf->lex_buffer,_tmp7,(unsigned int)_tmp8);_tmpB;});_tmpA;});
_tmp9;});}case 2: _LL2: return 0;case 3: _LL3: return Cyc_token(lexbuf);default: _LL4:(
lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(lexbuf,lexstate);}(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC[0]=({
struct Cyc_Lexing_Error_struct _tmpD;_tmpD.tag=Cyc_Lexing_Error;_tmpD.f1=_tag_arr("some action didn't return!",
sizeof(char),27);_tmpD;});_tmpC;}));}struct _tuple11*Cyc_token(struct Cyc_Lexing_lexbuf*
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_compress(void*t);unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);void*Cyc_Dump_last_dumped=(void*)1;void Cyc_Dump_dump_alpha(
struct _tagged_arr s){if(Cyc_Dump_last_dumped == (void*)0)({struct Cyc_Std_String_pa_struct
_tmpF;_tmpF.tag=0;_tmpF.f1=(struct _tagged_arr)s;{void*_tmpE[1]={& _tmpF};Cyc_Std_printf(
_tag_arr(" %s",sizeof(char),4),_tag_arr(_tmpE,sizeof(void*),1));}});else{({
struct Cyc_Std_String_pa_struct _tmp11;_tmp11.tag=0;_tmp11.f1=(struct _tagged_arr)s;{
void*_tmp10[1]={& _tmp11};Cyc_Std_printf(_tag_arr("%s",sizeof(char),3),_tag_arr(
_tmp10,sizeof(void*),1));}});Cyc_Dump_last_dumped=(void*)0;}}void Cyc_Dump_dump_alphastr(
struct _tagged_arr*s){Cyc_Dump_dump_alpha(*s);}void Cyc_Dump_dump_symbol(struct
_tagged_arr s){({struct Cyc_Std_String_pa_struct _tmp13;_tmp13.tag=0;_tmp13.f1=(
struct _tagged_arr)s;{void*_tmp12[1]={& _tmp13};Cyc_Std_printf(_tag_arr("%s",
sizeof(char),3),_tag_arr(_tmp12,sizeof(void*),1));}});Cyc_Dump_last_dumped=(void*)
1;}void Cyc_Dump_dump_char(int c){({struct Cyc_Std_Int_pa_struct _tmp15;_tmp15.tag=1;
_tmp15.f1=(int)((unsigned int)c);{void*_tmp14[1]={& _tmp15};Cyc_Std_printf(
_tag_arr("%c",sizeof(char),3),_tag_arr(_tmp14,sizeof(void*),1));}});Cyc_Dump_last_dumped=(
void*)1;}void Cyc_Dump_dump_pointer(struct _tagged_arr s){if(Cyc_Dump_last_dumped == (
void*)0)({struct Cyc_Std_String_pa_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(struct
_tagged_arr)s;{void*_tmp16[1]={& _tmp17};Cyc_Std_printf(_tag_arr(" %s",sizeof(
char),4),_tag_arr(_tmp16,sizeof(void*),1));}});else{({struct Cyc_Std_String_pa_struct
_tmp19;_tmp19.tag=0;_tmp19.f1=(struct _tagged_arr)s;{void*_tmp18[1]={& _tmp19};Cyc_Std_printf(
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp18,sizeof(void*),1));}});}Cyc_Dump_last_dumped=(
void*)2;}void Cyc_Dump_dump_boldalpha(struct _tagged_arr x){Cyc_Dump_dump_alpha(
_tag_arr("",sizeof(char),1));Cyc_Dump_dump_symbol(_tag_arr("\\textbf{",sizeof(
char),9));Cyc_Dump_dump_alpha(x);Cyc_Dump_dump_symbol(_tag_arr("}",sizeof(char),
2));Cyc_Dump_dump_alpha(_tag_arr("",sizeof(char),1));}void Cyc_Dump_dump_lb(){Cyc_Dump_dump_symbol(
_tag_arr("{\\lb}",sizeof(char),6));}void Cyc_Dump_dump_rb(){Cyc_Dump_dump_symbol(
_tag_arr("{\\rb}",sizeof(char),6));}void Cyc_Dump_dump_begin(){Cyc_Dump_dump_symbol(
_tag_arr("%%HEVEA \\begin{latexonly}\n\\begin{list}{}{\\setlength\\itemsep{0pt}\\setlength\\topsep{0pt}\\setlength\\leftmargin{\\parindent}\\setlength\\itemindent{-\\leftmargin}}\\item[]\\colorbox{cycdochighlight}{\\ttfamily\\begin{minipage}[b]{\\textwidth}\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}<dl><dt><table><tr><td bgcolor=\"c0d0ff\">\\end{rawhtml}\n\\begin{tabbing}\n",
sizeof(char),348));}void Cyc_Dump_dump_middle(){Cyc_Dump_dump_symbol(_tag_arr("\\end{tabbing}\n%%HEVEA \\begin{latexonly}\n\\end{minipage}}\\\\\\strut\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</td></tr></table><dd>\\end{rawhtml}\n",
sizeof(char),148));}void Cyc_Dump_dump_end(){Cyc_Dump_dump_symbol(_tag_arr("%%HEVEA \\begin{latexonly}\n\\end{list}\\smallskip\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</dl>\\end{rawhtml}\n",
sizeof(char),114));}void Cyc_Dump_dumpqvar(struct _tuple1*v);void Cyc_Dump_dump_boldqvar(
struct _tuple1*v){Cyc_Dump_dump_alpha(_tag_arr("",sizeof(char),1));Cyc_Dump_dump_symbol(
_tag_arr("\\textbf{",sizeof(char),9));Cyc_Dump_dumpqvar(v);Cyc_Dump_dump_symbol(
_tag_arr("}",sizeof(char),2));Cyc_Dump_dump_alpha(_tag_arr("",sizeof(char),1));}
static int Cyc_Dump_expand_typedefs=0;static int Cyc_Dump_qvar_to_Cids=0;static int
Cyc_Dump_add_cyc_prefix=0;static int Cyc_Dump_to_VC=0;void Cyc_Dump_dumpenumfields(
struct Cyc_List_List*fields);void Cyc_Dump_dumptyp(void*);void Cyc_Dump_dumpntyp(
void*t);void Cyc_Dump_dumpexp(struct Cyc_Absyn_Exp*);void Cyc_Dump_dumpexp_prec(int,
struct Cyc_Absyn_Exp*);void Cyc_Dump_dumpdecl(struct Cyc_Absyn_Decl*,struct
_tagged_arr);void Cyc_Dump_dumptms(struct Cyc_List_List*,void(*f)(void*),void*);
void Cyc_Dump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(void*),void*);void Cyc_Dump_dumpaggrfields(
struct Cyc_List_List*fields);void Cyc_Dump_ignore(void*x){return;}void Cyc_Dump_dump_semi(){
Cyc_Dump_dump_char((int)';');}void Cyc_Dump_dump_sep(void(*f)(void*),struct Cyc_List_List*
l,struct _tagged_arr sep){if(l == 0)return;for(0;l->tl != 0;l=l->tl){f((void*)l->hd);
Cyc_Dump_dump_symbol(sep);}f((void*)l->hd);}void Cyc_Dump_dump_sep_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*l,struct _tagged_arr sep){if(l == 0)
return;for(0;l->tl != 0;l=l->tl){f(env,(void*)l->hd);Cyc_Dump_dump_symbol(sep);}f(
env,(void*)l->hd);}void Cyc_Dump_group(void(*f)(void*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep){Cyc_Dump_dump_symbol(
start);Cyc_Dump_dump_sep(f,l,sep);Cyc_Dump_dump_symbol(end);}void Cyc_Dump_group_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep){Cyc_Dump_dump_symbol(start);Cyc_Dump_dump_sep_c(
f,env,l,sep);Cyc_Dump_dump_symbol(end);}void Cyc_Dump_egroup(void(*f)(void*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep){if(l != 0)Cyc_Dump_group(f,l,start,end,sep);}void Cyc_Dump_dumpqvar(
struct _tuple1*v){struct Cyc_List_List*_tmp1A=0;{void*_tmp1B=(*v).f1;struct Cyc_List_List*
_tmp1C;struct Cyc_List_List*_tmp1D;_LL7: if((int)_tmp1B != 0)goto _LL9;_LL8: _tmp1C=0;
goto _LLA;_LL9: if(_tmp1B <= (void*)1?1:*((int*)_tmp1B)!= 0)goto _LLB;_tmp1C=((
struct Cyc_Absyn_Rel_n_struct*)_tmp1B)->f1;_LLA: _tmp1A=_tmp1C;goto _LL6;_LLB: if(
_tmp1B <= (void*)1?1:*((int*)_tmp1B)!= 1)goto _LL6;_tmp1D=((struct Cyc_Absyn_Abs_n_struct*)
_tmp1B)->f1;_LLC: _tmp1A=(Cyc_Dump_qvar_to_Cids?Cyc_Dump_add_cyc_prefix: 0)?({
struct Cyc_List_List*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->hd=Cyc_Absynpp_cyc_stringptr;
_tmp1E->tl=_tmp1D;_tmp1E;}): _tmp1D;goto _LL6;_LL6:;}if(_tmp1A != 0){Cyc_Dump_dump_alphastr((
struct _tagged_arr*)_tmp1A->hd);for(_tmp1A=_tmp1A->tl;_tmp1A != 0;_tmp1A=_tmp1A->tl){
if(Cyc_Dump_qvar_to_Cids)Cyc_Dump_dump_char((int)'_');else{Cyc_Dump_dump_symbol(
_tag_arr("::",sizeof(char),3));}Cyc_Dump_dump_alpha(*((struct _tagged_arr*)_tmp1A->hd));}
if(Cyc_Dump_qvar_to_Cids)Cyc_Dump_dump_symbol(_tag_arr("_",sizeof(char),2));
else{Cyc_Dump_dump_symbol(_tag_arr("::",sizeof(char),3));}Cyc_Dump_dump_alpha(*(*
v).f2);}else{Cyc_Dump_dump_alpha(*(*v).f2);}}void Cyc_Dump_dumptq(struct Cyc_Absyn_Tqual
tq){if(tq.q_restrict)Cyc_Dump_dump_alpha(_tag_arr("restrict",sizeof(char),9));
if(tq.q_volatile)Cyc_Dump_dump_alpha(_tag_arr("volatile",sizeof(char),9));if(tq.q_const)
Cyc_Dump_dump_alpha(_tag_arr("const",sizeof(char),6));}void Cyc_Dump_dumpscope(
void*sc){void*_tmp1F=sc;_LLE: if((int)_tmp1F != 0)goto _LL10;_LLF: Cyc_Dump_dump_alpha(
_tag_arr("static",sizeof(char),7));return;_LL10: if((int)_tmp1F != 2)goto _LL12;
_LL11: return;_LL12: if((int)_tmp1F != 3)goto _LL14;_LL13: Cyc_Dump_dump_alpha(
_tag_arr("extern",sizeof(char),7));return;_LL14: if((int)_tmp1F != 4)goto _LL16;
_LL15: Cyc_Dump_dump_alpha(_tag_arr("extern \"C\"",sizeof(char),11));return;_LL16:
if((int)_tmp1F != 1)goto _LL18;_LL17: Cyc_Dump_dump_alpha(_tag_arr("abstract",
sizeof(char),9));return;_LL18: if((int)_tmp1F != 5)goto _LLD;_LL19: Cyc_Dump_dump_alpha(
_tag_arr("register",sizeof(char),9));return;_LLD:;}void Cyc_Dump_dumpkind(void*k){
void*_tmp20=k;_LL1B: if((int)_tmp20 != 0)goto _LL1D;_LL1C: Cyc_Dump_dump_alpha(
_tag_arr("A",sizeof(char),2));return;_LL1D: if((int)_tmp20 != 1)goto _LL1F;_LL1E:
Cyc_Dump_dump_alpha(_tag_arr("M",sizeof(char),2));return;_LL1F: if((int)_tmp20 != 
2)goto _LL21;_LL20: Cyc_Dump_dump_alpha(_tag_arr("B",sizeof(char),2));return;_LL21:
if((int)_tmp20 != 3)goto _LL23;_LL22: Cyc_Dump_dump_alpha(_tag_arr("R",sizeof(char),
2));return;_LL23: if((int)_tmp20 != 4)goto _LL25;_LL24: Cyc_Dump_dump_alpha(_tag_arr("E",
sizeof(char),2));return;_LL25: if((int)_tmp20 != 5)goto _LL1A;_LL26: Cyc_Dump_dump_alpha(
_tag_arr("I",sizeof(char),2));return;_LL1A:;}void Cyc_Dump_dumptps(struct Cyc_List_List*
ts){Cyc_Dump_egroup(Cyc_Dump_dumptyp,ts,_tag_arr("<",sizeof(char),2),_tag_arr(">",
sizeof(char),2),_tag_arr(", ",sizeof(char),3));Cyc_Dump_dump_alpha(_tag_arr("",
sizeof(char),1));}void Cyc_Dump_dumptvar(struct Cyc_Absyn_Tvar*tv){Cyc_Dump_dump_alphastr(
tv->name);}void Cyc_Dump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){Cyc_Dump_dump_alphastr(
tv->name);{void*_tmp21=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp22;void*
_tmp23;void*_tmp24;_LL28: if(*((int*)_tmp21)!= 0)goto _LL2A;_tmp22=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmp21)->f1;if((int)_tmp22 != 2)goto _LL2A;_LL29: goto _LL27;
_LL2A: if(*((int*)_tmp21)!= 2)goto _LL2C;_tmp23=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp21)->f2;_LL2B: _tmp24=_tmp23;goto _LL2D;_LL2C: if(*((int*)_tmp21)!= 0)goto _LL2E;
_tmp24=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp21)->f1;_LL2D: Cyc_Dump_dump_symbol(
_tag_arr("::",sizeof(char),3));Cyc_Dump_dumpkind(_tmp24);goto _LL27;_LL2E:;_LL2F:
Cyc_Dump_dump_symbol(_tag_arr("::?",sizeof(char),4));goto _LL27;_LL27:;}}void Cyc_Dump_dumptvars(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_egroup)(
Cyc_Dump_dumptvar,tvs,_tag_arr("<",sizeof(char),2),_tag_arr(">",sizeof(char),2),
_tag_arr(",",sizeof(char),2));Cyc_Dump_dump_alpha(_tag_arr("",sizeof(char),1));}
void Cyc_Dump_dumpkindedtvars(struct Cyc_List_List*tvs){((void(*)(void(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr
end,struct _tagged_arr sep))Cyc_Dump_egroup)(Cyc_Dump_dumpkindedtvar,tvs,_tag_arr("<",
sizeof(char),2),_tag_arr(">",sizeof(char),2),_tag_arr(",",sizeof(char),2));Cyc_Dump_dump_alpha(
_tag_arr("",sizeof(char),1));}struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
void Cyc_Dump_dumparg(struct _tuple12*pr){((void(*)(struct Cyc_Absyn_Tqual,void*,
void(*f)(int),int))Cyc_Dump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Dump_ignore,
0);}void Cyc_Dump_dumpargs(struct Cyc_List_List*ts){((void(*)(void(*f)(struct
_tuple12*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,
struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumparg,ts,_tag_arr("(",sizeof(
char),2),_tag_arr(")",sizeof(char),2),_tag_arr(", ",sizeof(char),3));}int Cyc_Dump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmp25=(void*)tms->hd;_LL31:
if(_tmp25 <= (void*)1?1:*((int*)_tmp25)!= 1)goto _LL33;_LL32: return 1;_LL33:;_LL34:
return 0;_LL30:;}}static void Cyc_Dump_dumprgn(void*t){void*_tmp26=Cyc_Tcutil_compress(
t);_LL36: if((int)_tmp26 != 2)goto _LL38;_LL37: Cyc_Dump_dump_alpha(_tag_arr("`H",
sizeof(char),3));goto _LL35;_LL38:;_LL39: Cyc_Dump_dumpntyp(t);goto _LL35;_LL35:;}
struct _tuple13{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static struct
_tuple13 Cyc_Dump_effects_split(void*t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*
effects=0;{void*_tmp27=Cyc_Tcutil_compress(t);void*_tmp28;struct Cyc_List_List*
_tmp29;_LL3B: if(_tmp27 <= (void*)3?1:*((int*)_tmp27)!= 19)goto _LL3D;_tmp28=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp27)->f1;_LL3C: rgions=({struct Cyc_List_List*
_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->hd=(void*)_tmp28;_tmp2A->tl=rgions;
_tmp2A;});goto _LL3A;_LL3D: if(_tmp27 <= (void*)3?1:*((int*)_tmp27)!= 20)goto _LL3F;
_tmp29=((struct Cyc_Absyn_JoinEff_struct*)_tmp27)->f1;_LL3E: for(0;_tmp29 != 0;
_tmp29=_tmp29->tl){struct Cyc_List_List*_tmp2C;struct Cyc_List_List*_tmp2D;struct
_tuple13 _tmp2B=Cyc_Dump_effects_split((void*)_tmp29->hd);_tmp2C=_tmp2B.f1;_tmp2D=
_tmp2B.f2;rgions=Cyc_List_imp_append(_tmp2C,rgions);effects=Cyc_List_imp_append(
_tmp2D,effects);}goto _LL3A;_LL3F:;_LL40: effects=({struct Cyc_List_List*_tmp2E=
_cycalloc(sizeof(*_tmp2E));_tmp2E->hd=(void*)t;_tmp2E->tl=effects;_tmp2E;});goto
_LL3A;_LL3A:;}return({struct _tuple13 _tmp2F;_tmp2F.f1=rgions;_tmp2F.f2=effects;
_tmp2F;});}static void Cyc_Dump_dumpeff(void*t){struct Cyc_List_List*_tmp31;struct
Cyc_List_List*_tmp32;struct _tuple13 _tmp30=Cyc_Dump_effects_split(t);_tmp31=
_tmp30.f1;_tmp32=_tmp30.f2;_tmp31=Cyc_List_imp_rev(_tmp31);_tmp32=Cyc_List_imp_rev(
_tmp32);for(0;_tmp32 != 0;_tmp32=_tmp32->tl){Cyc_Dump_dumpntyp((void*)_tmp32->hd);
Cyc_Dump_dump_char((int)'+');}Cyc_Dump_dump_lb();for(0;_tmp31 != 0;_tmp31=_tmp31->tl){
Cyc_Dump_dumprgn((void*)_tmp31->hd);if(_tmp31->tl != 0)Cyc_Dump_dump_char((int)',');}
Cyc_Dump_dump_rb();}void Cyc_Dump_dumpntyp(void*t){void*_tmp33=t;struct Cyc_Absyn_Tvar*
_tmp34;struct Cyc_Core_Opt*_tmp35;struct Cyc_Core_Opt*_tmp36;int _tmp37;struct Cyc_Core_Opt*
_tmp38;struct Cyc_Core_Opt*_tmp39;struct Cyc_Core_Opt _tmp3A;void*_tmp3B;int _tmp3C;
struct Cyc_Absyn_TunionInfo _tmp3D;void*_tmp3E;struct Cyc_List_List*_tmp3F;void*
_tmp40;struct Cyc_Absyn_TunionFieldInfo _tmp41;void*_tmp42;struct Cyc_List_List*
_tmp43;struct _tuple1*_tmp44;struct Cyc_List_List*_tmp45;void*_tmp46;void*_tmp47;
void*_tmp48;void*_tmp49;void*_tmp4A;void*_tmp4B;void*_tmp4C;void*_tmp4D;void*
_tmp4E;void*_tmp4F;void*_tmp50;void*_tmp51;void*_tmp52;void*_tmp53;void*_tmp54;
void*_tmp55;void*_tmp56;void*_tmp57;void*_tmp58;void*_tmp59;void*_tmp5A;void*
_tmp5B;void*_tmp5C;void*_tmp5D;void*_tmp5E;void*_tmp5F;void*_tmp60;void*_tmp61;
void*_tmp62;void*_tmp63;int _tmp64;void*_tmp65;void*_tmp66;int _tmp67;struct Cyc_List_List*
_tmp68;struct Cyc_Absyn_AggrInfo _tmp69;void*_tmp6A;struct Cyc_List_List*_tmp6B;
void*_tmp6C;struct Cyc_List_List*_tmp6D;struct _tuple1*_tmp6E;struct Cyc_List_List*
_tmp6F;struct Cyc_Core_Opt*_tmp70;void*_tmp71;_LL42: if(_tmp33 <= (void*)3?1:*((int*)
_tmp33)!= 7)goto _LL44;_LL43: goto _LL45;_LL44: if(_tmp33 <= (void*)3?1:*((int*)
_tmp33)!= 8)goto _LL46;_LL45: goto _LL47;_LL46: if(_tmp33 <= (void*)3?1:*((int*)
_tmp33)!= 4)goto _LL48;_LL47: return;_LL48: if((int)_tmp33 != 0)goto _LL4A;_LL49: Cyc_Dump_dump_alpha(
_tag_arr("void",sizeof(char),5));return;_LL4A: if(_tmp33 <= (void*)3?1:*((int*)
_tmp33)!= 1)goto _LL4C;_tmp34=((struct Cyc_Absyn_VarType_struct*)_tmp33)->f1;_LL4B:
Cyc_Dump_dump_alphastr(_tmp34->name);return;_LL4C: if(_tmp33 <= (void*)3?1:*((int*)
_tmp33)!= 0)goto _LL4E;_tmp35=((struct Cyc_Absyn_Evar_struct*)_tmp33)->f1;_tmp36=((
struct Cyc_Absyn_Evar_struct*)_tmp33)->f2;if(_tmp36 != 0)goto _LL4E;_tmp37=((struct
Cyc_Absyn_Evar_struct*)_tmp33)->f3;_LL4D: Cyc_Dump_dump_char((int)'%');if(_tmp35
== 0)Cyc_Dump_dump_char((int)'?');else{Cyc_Dump_dumpkind((void*)_tmp35->v);}Cyc_Dump_dump_symbol((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp73;_tmp73.tag=1;_tmp73.f1=(
int)((unsigned int)_tmp37);{void*_tmp72[1]={& _tmp73};Cyc_Std_aprintf(_tag_arr("(%d)",
sizeof(char),5),_tag_arr(_tmp72,sizeof(void*),1));}}));return;_LL4E: if(_tmp33 <= (
void*)3?1:*((int*)_tmp33)!= 0)goto _LL50;_tmp38=((struct Cyc_Absyn_Evar_struct*)
_tmp33)->f1;_tmp39=((struct Cyc_Absyn_Evar_struct*)_tmp33)->f2;if(_tmp39 == 0)goto
_LL50;_tmp3A=*_tmp39;_tmp3B=(void*)_tmp3A.v;_tmp3C=((struct Cyc_Absyn_Evar_struct*)
_tmp33)->f3;_LL4F: Cyc_Dump_dumpntyp(_tmp3B);return;_LL50: if(_tmp33 <= (void*)3?1:*((
int*)_tmp33)!= 2)goto _LL52;_tmp3D=((struct Cyc_Absyn_TunionType_struct*)_tmp33)->f1;
_tmp3E=(void*)_tmp3D.tunion_info;_tmp3F=_tmp3D.targs;_tmp40=(void*)_tmp3D.rgn;
_LL51:{void*_tmp74=_tmp3E;struct Cyc_Absyn_UnknownTunionInfo _tmp75;struct _tuple1*
_tmp76;int _tmp77;struct Cyc_Absyn_Tuniondecl**_tmp78;struct Cyc_Absyn_Tuniondecl*
_tmp79;struct Cyc_Absyn_Tuniondecl _tmp7A;struct _tuple1*_tmp7B;int _tmp7C;_LL93: if(*((
int*)_tmp74)!= 0)goto _LL95;_tmp75=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp74)->f1;
_tmp76=_tmp75.name;_tmp77=_tmp75.is_xtunion;_LL94: _tmp7B=_tmp76;_tmp7C=_tmp77;
goto _LL96;_LL95: if(*((int*)_tmp74)!= 1)goto _LL92;_tmp78=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp74)->f1;_tmp79=*_tmp78;_tmp7A=*_tmp79;_tmp7B=_tmp7A.name;_tmp7C=_tmp7A.is_xtunion;
_LL96: if(_tmp7C)Cyc_Dump_dump_alpha(_tag_arr("xtunion",sizeof(char),8));else{Cyc_Dump_dump_alpha(
_tag_arr("tunion",sizeof(char),7));}{void*_tmp7D=Cyc_Tcutil_compress(_tmp40);
_LL98: if((int)_tmp7D != 2)goto _LL9A;_LL99: goto _LL97;_LL9A:;_LL9B: Cyc_Dump_dumptyp(
_tmp40);goto _LL97;_LL97:;}Cyc_Dump_dumpqvar(_tmp7B);Cyc_Dump_dumptps(_tmp3F);
goto _LL92;_LL92:;}goto _LL41;_LL52: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 3)
goto _LL54;_tmp41=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp33)->f1;_tmp42=(
void*)_tmp41.field_info;_tmp43=_tmp41.targs;_LL53:{void*_tmp7E=_tmp42;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp7F;struct _tuple1*_tmp80;struct _tuple1*_tmp81;int _tmp82;struct Cyc_Absyn_Tuniondecl*
_tmp83;struct Cyc_Absyn_Tuniondecl _tmp84;struct _tuple1*_tmp85;int _tmp86;struct Cyc_Absyn_Tunionfield*
_tmp87;struct Cyc_Absyn_Tunionfield _tmp88;struct _tuple1*_tmp89;_LL9D: if(*((int*)
_tmp7E)!= 0)goto _LL9F;_tmp7F=((struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp7E)->f1;
_tmp80=_tmp7F.tunion_name;_tmp81=_tmp7F.field_name;_tmp82=_tmp7F.is_xtunion;
_LL9E: _tmp85=_tmp80;_tmp86=_tmp82;_tmp89=_tmp81;goto _LLA0;_LL9F: if(*((int*)
_tmp7E)!= 1)goto _LL9C;_tmp83=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp7E)->f1;
_tmp84=*_tmp83;_tmp85=_tmp84.name;_tmp86=_tmp84.is_xtunion;_tmp87=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp7E)->f2;_tmp88=*_tmp87;_tmp89=_tmp88.name;_LLA0: if(_tmp86)Cyc_Dump_dump_alpha(
_tag_arr("xtunion",sizeof(char),8));else{Cyc_Dump_dump_alpha(_tag_arr("tunion",
sizeof(char),7));}Cyc_Dump_dumpqvar(_tmp85);Cyc_Dump_dump_char((int)'.');Cyc_Dump_dumpqvar(
_tmp89);Cyc_Dump_dumptps(_tmp43);goto _LL9C;_LL9C:;}goto _LL41;_LL54: if(_tmp33 <= (
void*)3?1:*((int*)_tmp33)!= 12)goto _LL56;_tmp44=((struct Cyc_Absyn_EnumType_struct*)
_tmp33)->f1;_LL55: Cyc_Dump_dump_alpha(_tag_arr("enum",sizeof(char),5));Cyc_Dump_dumpqvar(
_tmp44);return;_LL56: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 13)goto _LL58;
_tmp45=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp33)->f1;_LL57: Cyc_Dump_dump_alpha(
_tag_arr("enum",sizeof(char),5));Cyc_Dump_dump_symbol(_tag_arr(" ",sizeof(char),
2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(_tag_arr("\\\\\n~~",sizeof(char),6));
Cyc_Dump_dumpenumfields(_tmp45);Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",sizeof(
char),4));Cyc_Dump_dump_rb();return;_LL58: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)
!= 5)goto _LL5A;_tmp46=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((
int)_tmp46 != 2)goto _LL5A;_tmp47=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;
if((int)_tmp47 != 2)goto _LL5A;_LL59: goto _LL5B;_LL5A: if(_tmp33 <= (void*)3?1:*((int*)
_tmp33)!= 5)goto _LL5C;_tmp48=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;
if((int)_tmp48 != 0)goto _LL5C;_tmp49=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f2;if((int)_tmp49 != 2)goto _LL5C;_LL5B: Cyc_Dump_dump_alpha(_tag_arr("int",
sizeof(char),4));return;_LL5C: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 5)goto
_LL5E;_tmp4A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp4A
!= 2)goto _LL5E;_tmp4B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((
int)_tmp4B != 0)goto _LL5E;_LL5D: Cyc_Dump_dump_alpha(_tag_arr("char",sizeof(char),
5));return;_LL5E: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 5)goto _LL60;_tmp4C=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp4C != 0)goto _LL60;
_tmp4D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((int)_tmp4D != 0)
goto _LL60;_LL5F: Cyc_Dump_dump_alpha(_tag_arr("signed char",sizeof(char),12));
return;_LL60: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 5)goto _LL62;_tmp4E=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp4E != 1)goto _LL62;_tmp4F=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((int)_tmp4F != 0)goto _LL62;
_LL61: Cyc_Dump_dump_alpha(_tag_arr("unsigned char",sizeof(char),14));return;
_LL62: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 5)goto _LL64;_tmp50=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp50 != 2)goto _LL64;_tmp51=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((int)_tmp51 != 1)goto _LL64;
_LL63: goto _LL65;_LL64: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 5)goto _LL66;
_tmp52=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp52 != 0)
goto _LL66;_tmp53=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((int)
_tmp53 != 1)goto _LL66;_LL65: Cyc_Dump_dump_alpha(_tag_arr("short",sizeof(char),6));
return;_LL66: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 5)goto _LL68;_tmp54=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp54 != 1)goto _LL68;_tmp55=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((int)_tmp55 != 1)goto _LL68;
_LL67: Cyc_Dump_dump_alpha(_tag_arr("unsigned short",sizeof(char),15));return;
_LL68: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 5)goto _LL6A;_tmp56=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp56 != 1)goto _LL6A;_tmp57=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((int)_tmp57 != 2)goto _LL6A;
_LL69: Cyc_Dump_dump_alpha(_tag_arr("unsigned int",sizeof(char),13));return;_LL6A:
if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 5)goto _LL6C;_tmp58=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f1;if((int)_tmp58 != 2)goto _LL6C;_tmp59=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f2;if((int)_tmp59 != 3)goto _LL6C;if(!Cyc_Dump_to_VC)goto _LL6C;_LL6B: goto
_LL6D;_LL6C: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 5)goto _LL6E;_tmp5A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp5A != 0)goto _LL6E;_tmp5B=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((int)_tmp5B != 3)goto _LL6E;
if(!Cyc_Dump_to_VC)goto _LL6E;_LL6D: Cyc_Dump_dump_alpha(_tag_arr("__int64",
sizeof(char),8));return;_LL6E: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 5)goto
_LL70;_tmp5C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp5C
!= 2)goto _LL70;_tmp5D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((
int)_tmp5D != 3)goto _LL70;_LL6F: goto _LL71;_LL70: if(_tmp33 <= (void*)3?1:*((int*)
_tmp33)!= 5)goto _LL72;_tmp5E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;
if((int)_tmp5E != 0)goto _LL72;_tmp5F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f2;if((int)_tmp5F != 3)goto _LL72;_LL71: Cyc_Dump_dump_alpha(_tag_arr("long long",
sizeof(char),10));return;_LL72: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 5)goto
_LL74;_tmp60=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp60
!= 1)goto _LL74;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((
int)_tmp61 != 3)goto _LL74;if(!Cyc_Dump_to_VC)goto _LL74;_LL73: Cyc_Dump_dump_alpha(
_tag_arr("unsigned __int64",sizeof(char),17));return;_LL74: if(_tmp33 <= (void*)3?
1:*((int*)_tmp33)!= 5)goto _LL76;_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f1;if((int)_tmp62 != 1)goto _LL76;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f2;if((int)_tmp63 != 3)goto _LL76;_LL75: Cyc_Dump_dump_alpha(_tag_arr("unsigned long long",
sizeof(char),19));return;_LL76: if((int)_tmp33 != 1)goto _LL78;_LL77: Cyc_Dump_dump_alpha(
_tag_arr("float",sizeof(char),6));return;_LL78: if(_tmp33 <= (void*)3?1:*((int*)
_tmp33)!= 6)goto _LL7A;_tmp64=((struct Cyc_Absyn_DoubleType_struct*)_tmp33)->f1;
_LL79: if(_tmp64)Cyc_Dump_dump_alpha(_tag_arr("long double",sizeof(char),12));
else{Cyc_Dump_dump_alpha(_tag_arr("double",sizeof(char),7));}return;_LL7A: if(
_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 14)goto _LL7C;_tmp65=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp33)->f1;_LL7B: Cyc_Dump_dump_alpha(_tag_arr("sizeof_t<",sizeof(char),10));Cyc_Dump_dumptyp(
_tmp65);Cyc_Dump_dump_alpha(_tag_arr(">",sizeof(char),2));return;_LL7C: if(_tmp33
<= (void*)3?1:*((int*)_tmp33)!= 17)goto _LL7E;_tmp66=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp33)->f1;_LL7D: Cyc_Dump_dump_alpha(_tag_arr("tag_t<",sizeof(char),7));Cyc_Dump_dumptyp(
_tmp66);Cyc_Dump_dump_alpha(_tag_arr(">",sizeof(char),2));return;_LL7E: if(_tmp33
<= (void*)3?1:*((int*)_tmp33)!= 18)goto _LL80;_tmp67=((struct Cyc_Absyn_TypeInt_struct*)
_tmp33)->f1;_LL7F: Cyc_Dump_dump_alpha((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp8B;_tmp8B.tag=1;_tmp8B.f1=(int)((unsigned int)_tmp67);{void*_tmp8A[1]={&
_tmp8B};Cyc_Std_aprintf(_tag_arr("`%d",sizeof(char),4),_tag_arr(_tmp8A,sizeof(
void*),1));}}));return;_LL80: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 9)goto
_LL82;_tmp68=((struct Cyc_Absyn_TupleType_struct*)_tmp33)->f1;_LL81: Cyc_Dump_dump_symbol(
_tag_arr("\\$",sizeof(char),3));Cyc_Dump_dumpargs(_tmp68);return;_LL82: if(_tmp33
<= (void*)3?1:*((int*)_tmp33)!= 10)goto _LL84;_tmp69=((struct Cyc_Absyn_AggrType_struct*)
_tmp33)->f1;_tmp6A=(void*)_tmp69.aggr_info;_tmp6B=_tmp69.targs;_LL83: {void*
_tmp8D;struct _tuple1*_tmp8E;struct _tuple4 _tmp8C=Cyc_Absyn_aggr_kinded_name(
_tmp6A);_tmp8D=_tmp8C.f1;_tmp8E=_tmp8C.f2;Cyc_Dump_dump_alpha(_tmp8D == (void*)1?
_tag_arr("union",sizeof(char),6): _tag_arr("struct",sizeof(char),7));Cyc_Dump_dumpqvar(
_tmp8E);Cyc_Dump_dumptps(_tmp6B);return;}_LL84: if(_tmp33 <= (void*)3?1:*((int*)
_tmp33)!= 11)goto _LL86;_tmp6C=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp33)->f1;_tmp6D=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp33)->f2;_LL85: Cyc_Dump_dump_alpha(
_tmp6C == (void*)1?_tag_arr("union",sizeof(char),6): _tag_arr("struct",sizeof(char),
7));Cyc_Dump_dump_lb();Cyc_Dump_dumpaggrfields(_tmp6D);Cyc_Dump_dump_rb();
return;_LL86: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 16)goto _LL88;_tmp6E=((
struct Cyc_Absyn_TypedefType_struct*)_tmp33)->f1;_tmp6F=((struct Cyc_Absyn_TypedefType_struct*)
_tmp33)->f2;_tmp70=((struct Cyc_Absyn_TypedefType_struct*)_tmp33)->f4;_LL87:(Cyc_Dump_dumpqvar(
_tmp6E),Cyc_Dump_dumptps(_tmp6F));return;_LL88: if(_tmp33 <= (void*)3?1:*((int*)
_tmp33)!= 15)goto _LL8A;_tmp71=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp33)->f1;_LL89: Cyc_Dump_dumprgn(_tmp71);return;_LL8A: if((int)_tmp33 != 2)goto
_LL8C;_LL8B: goto _LL8D;_LL8C: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 19)goto
_LL8E;_LL8D: goto _LL8F;_LL8E: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 21)goto
_LL90;_LL8F: goto _LL91;_LL90: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 20)goto
_LL41;_LL91: return;_LL41:;}void Cyc_Dump_dumpvaropt(struct Cyc_Core_Opt*vo){if(vo
!= 0)Cyc_Dump_dump_alphastr((struct _tagged_arr*)vo->v);}void Cyc_Dump_dumpfunarg(
struct _tuple2*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),
struct Cyc_Core_Opt*))Cyc_Dump_dumptqtd)((*t).f2,(*t).f3,Cyc_Dump_dumpvaropt,(*t).f1);}
void Cyc_Dump_dump_rgncmp(struct _tuple0*cmp){struct _tuple0 _tmp90;void*_tmp91;void*
_tmp92;struct _tuple0*_tmp8F=cmp;_tmp90=*_tmp8F;_tmp91=_tmp90.f1;_tmp92=_tmp90.f2;
Cyc_Dump_dumptyp(_tmp91);Cyc_Dump_dump_char((int)'<');Cyc_Dump_dumptyp(_tmp92);}
void Cyc_Dump_dump_rgnpo(struct Cyc_List_List*rgn_po){((void(*)(void(*f)(struct
_tuple0*),struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Dump_dump_sep)(Cyc_Dump_dump_rgncmp,
rgn_po,_tag_arr(",",sizeof(char),2));}void Cyc_Dump_dumpfunargs(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){Cyc_Dump_dump_char((int)'(');for(0;args != 0;
args=args->tl){Cyc_Dump_dumpfunarg((struct _tuple2*)args->hd);if((args->tl != 0?1:
c_varargs)?1: cyc_varargs != 0)Cyc_Dump_dump_symbol(_tag_arr(", ",sizeof(char),3));}
if(c_varargs)Cyc_Dump_dump_symbol(_tag_arr("...",sizeof(char),4));else{if(
cyc_varargs != 0){struct _tuple2*_tmp93=({struct _tuple2*_tmp94=_cycalloc(sizeof(*
_tmp94));_tmp94->f1=cyc_varargs->name;_tmp94->f2=cyc_varargs->tq;_tmp94->f3=(
void*)cyc_varargs->type;_tmp94;});Cyc_Dump_dump_symbol(_tag_arr("...",sizeof(
char),4));if(cyc_varargs->inject)Cyc_Dump_dump_alpha(_tag_arr(" inject",sizeof(
char),8));Cyc_Dump_dumpfunarg(_tmp93);}}if(effopt != 0){Cyc_Dump_dump_semi();Cyc_Dump_dumpeff((
void*)effopt->v);}if(rgn_po != 0){Cyc_Dump_dump_char((int)':');Cyc_Dump_dump_rgnpo(
rgn_po);}Cyc_Dump_dump_char((int)')');}void Cyc_Dump_dumptyp(void*t){((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Dump_dumptqtd)(({struct Cyc_Absyn_Tqual
_tmp95;_tmp95.q_const=0;_tmp95.q_volatile=0;_tmp95.q_restrict=0;_tmp95;}),t,(
void(*)(int x))Cyc_Dump_ignore,0);}void Cyc_Dump_dumpdesignator(void*d){void*
_tmp96=d;struct Cyc_Absyn_Exp*_tmp97;struct _tagged_arr*_tmp98;_LLA2: if(*((int*)
_tmp96)!= 0)goto _LLA4;_tmp97=((struct Cyc_Absyn_ArrayElement_struct*)_tmp96)->f1;
_LLA3: Cyc_Dump_dump_symbol(_tag_arr(".[",sizeof(char),3));Cyc_Dump_dumpexp(
_tmp97);Cyc_Dump_dump_char((int)']');goto _LLA1;_LLA4: if(*((int*)_tmp96)!= 1)goto
_LLA1;_tmp98=((struct Cyc_Absyn_FieldName_struct*)_tmp96)->f1;_LLA5: Cyc_Dump_dump_char((
int)'.');Cyc_Dump_dump_alpha(*_tmp98);goto _LLA1;_LLA1:;}struct _tuple14{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};void Cyc_Dump_dumpde(struct _tuple14*de){Cyc_Dump_egroup(
Cyc_Dump_dumpdesignator,(*de).f1,_tag_arr("",sizeof(char),1),_tag_arr("=",
sizeof(char),2),_tag_arr("=",sizeof(char),2));Cyc_Dump_dumpexp((*de).f2);}void
Cyc_Dump_dumpexps_prec(int inprec,struct Cyc_List_List*es){((void(*)(void(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group_c)(Cyc_Dump_dumpexp_prec,
inprec,es,_tag_arr("",sizeof(char),1),_tag_arr("",sizeof(char),1),_tag_arr(",",
sizeof(char),2));}void Cyc_Dump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){int
myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= myprec)Cyc_Dump_dump_char((int)'(');{
void*_tmp99=(void*)e->r;void*_tmp9A;void*_tmp9B;char _tmp9C;void*_tmp9D;void*
_tmp9E;short _tmp9F;void*_tmpA0;void*_tmpA1;int _tmpA2;void*_tmpA3;void*_tmpA4;int
_tmpA5;void*_tmpA6;void*_tmpA7;int _tmpA8;void*_tmpA9;void*_tmpAA;long long _tmpAB;
void*_tmpAC;struct _tagged_arr _tmpAD;void*_tmpAE;void*_tmpAF;struct _tagged_arr
_tmpB0;struct _tuple1*_tmpB1;struct _tuple1*_tmpB2;void*_tmpB3;struct Cyc_List_List*
_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;struct Cyc_Core_Opt*_tmpB6;struct Cyc_Absyn_Exp*
_tmpB7;struct Cyc_Absyn_Exp*_tmpB8;void*_tmpB9;struct Cyc_Absyn_Exp*_tmpBA;void*
_tmpBB;struct Cyc_Absyn_Exp*_tmpBC;void*_tmpBD;struct Cyc_Absyn_Exp*_tmpBE;void*
_tmpBF;struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_Absyn_Exp*
_tmpC2;struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Exp*
_tmpC5;struct Cyc_List_List*_tmpC6;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_List_List*
_tmpC8;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_Absyn_Exp*
_tmpCB;void*_tmpCC;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Exp*_tmpCE;struct
Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpD0;void*_tmpD1;struct Cyc_Absyn_Exp*
_tmpD2;void*_tmpD3;void*_tmpD4;struct _tagged_arr*_tmpD5;void*_tmpD6;void*_tmpD7;
unsigned int _tmpD8;struct Cyc_List_List*_tmpD9;void*_tmpDA;struct Cyc_Absyn_Exp*
_tmpDB;struct Cyc_Absyn_Exp*_tmpDC;struct _tagged_arr*_tmpDD;struct Cyc_Absyn_Exp*
_tmpDE;struct _tagged_arr*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*
_tmpE1;struct Cyc_List_List*_tmpE2;struct _tuple2*_tmpE3;struct Cyc_List_List*
_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_Absyn_Vardecl*_tmpE6;struct Cyc_Absyn_Exp*
_tmpE7;struct Cyc_Absyn_Exp*_tmpE8;struct _tuple1*_tmpE9;struct Cyc_List_List*
_tmpEA;struct Cyc_List_List*_tmpEB;struct Cyc_List_List*_tmpEC;struct Cyc_Absyn_Tunionfield*
_tmpED;struct _tuple1*_tmpEE;struct _tuple1*_tmpEF;struct Cyc_Absyn_MallocInfo
_tmpF0;int _tmpF1;struct Cyc_Absyn_Exp*_tmpF2;void**_tmpF3;struct Cyc_Absyn_Exp*
_tmpF4;struct Cyc_Core_Opt*_tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_Absyn_Stmt*
_tmpF7;struct Cyc_Absyn_Fndecl*_tmpF8;struct Cyc_Absyn_Exp*_tmpF9;_LLA7: if(*((int*)
_tmp99)!= 0)goto _LLA9;_tmp9A=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp99)->f1;
if(_tmp9A <= (void*)1?1:*((int*)_tmp9A)!= 0)goto _LLA9;_tmp9B=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp9A)->f1;_tmp9C=((struct Cyc_Absyn_Char_c_struct*)_tmp9A)->f2;_LLA8: Cyc_Dump_dump_char((
int)'\'');Cyc_Dump_dump_alpha(Cyc_Absynpp_char_escape(_tmp9C));Cyc_Dump_dump_char((
int)'\'');goto _LLA6;_LLA9: if(*((int*)_tmp99)!= 0)goto _LLAB;_tmp9D=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp99)->f1;if(_tmp9D <= (void*)1?1:*((int*)
_tmp9D)!= 1)goto _LLAB;_tmp9E=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp9D)->f1;
_tmp9F=((struct Cyc_Absyn_Short_c_struct*)_tmp9D)->f2;_LLAA: Cyc_Dump_dump_alpha((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpFB;_tmpFB.tag=1;_tmpFB.f1=(
int)((unsigned int)((int)_tmp9F));{void*_tmpFA[1]={& _tmpFB};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(char),3),_tag_arr(_tmpFA,sizeof(void*),1));}}));goto _LLA6;
_LLAB: if(*((int*)_tmp99)!= 0)goto _LLAD;_tmpA0=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp99)->f1;if(_tmpA0 <= (void*)1?1:*((int*)_tmpA0)!= 2)goto _LLAD;_tmpA1=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmpA0)->f1;if((int)_tmpA1 != 2)goto _LLAD;_tmpA2=((
struct Cyc_Absyn_Int_c_struct*)_tmpA0)->f2;_LLAC: _tmpA5=_tmpA2;goto _LLAE;_LLAD:
if(*((int*)_tmp99)!= 0)goto _LLAF;_tmpA3=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp99)->f1;if(_tmpA3 <= (void*)1?1:*((int*)_tmpA3)!= 2)goto _LLAF;_tmpA4=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmpA3)->f1;if((int)_tmpA4 != 0)goto _LLAF;_tmpA5=((
struct Cyc_Absyn_Int_c_struct*)_tmpA3)->f2;_LLAE: Cyc_Dump_dump_alpha((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpFD;_tmpFD.tag=1;_tmpFD.f1=(int)((
unsigned int)_tmpA5);{void*_tmpFC[1]={& _tmpFD};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(char),3),_tag_arr(_tmpFC,sizeof(void*),1));}}));goto _LLA6;_LLAF: if(*((int*)
_tmp99)!= 0)goto _LLB1;_tmpA6=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp99)->f1;
if(_tmpA6 <= (void*)1?1:*((int*)_tmpA6)!= 2)goto _LLB1;_tmpA7=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmpA6)->f1;if((int)_tmpA7 != 1)goto _LLB1;_tmpA8=((struct Cyc_Absyn_Int_c_struct*)
_tmpA6)->f2;_LLB0: Cyc_Dump_dump_alpha((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmpFF;_tmpFF.tag=1;_tmpFF.f1=(int)((unsigned int)_tmpA8);{void*_tmpFE[1]={&
_tmpFF};Cyc_Std_aprintf(_tag_arr("%du",sizeof(char),4),_tag_arr(_tmpFE,sizeof(
void*),1));}}));goto _LLA6;_LLB1: if(*((int*)_tmp99)!= 0)goto _LLB3;_tmpA9=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp99)->f1;if(_tmpA9 <= (void*)1?1:*((int*)
_tmpA9)!= 3)goto _LLB3;_tmpAA=(void*)((struct Cyc_Absyn_LongLong_c_struct*)_tmpA9)->f1;
_tmpAB=((struct Cyc_Absyn_LongLong_c_struct*)_tmpA9)->f2;_LLB2: Cyc_Dump_dump_alpha(
_tag_arr("<<FIX LONG LONG CONSTANT>>",sizeof(char),27));goto _LLA6;_LLB3: if(*((
int*)_tmp99)!= 0)goto _LLB5;_tmpAC=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp99)->f1;if(_tmpAC <= (void*)1?1:*((int*)_tmpAC)!= 4)goto _LLB5;_tmpAD=((struct
Cyc_Absyn_Float_c_struct*)_tmpAC)->f1;_LLB4: Cyc_Dump_dump_alpha(_tmpAD);goto
_LLA6;_LLB5: if(*((int*)_tmp99)!= 0)goto _LLB7;_tmpAE=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp99)->f1;if((int)_tmpAE != 0)goto _LLB7;_LLB6: Cyc_Dump_dump_alpha(_tag_arr("NULL",
sizeof(char),5));goto _LLA6;_LLB7: if(*((int*)_tmp99)!= 0)goto _LLB9;_tmpAF=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp99)->f1;if(_tmpAF <= (void*)1?1:*((int*)
_tmpAF)!= 5)goto _LLB9;_tmpB0=((struct Cyc_Absyn_String_c_struct*)_tmpAF)->f1;
_LLB8: Cyc_Dump_dump_char((int)'"');Cyc_Dump_dump_alpha(Cyc_Absynpp_string_escape(
_tmpB0));Cyc_Dump_dump_char((int)'"');goto _LLA6;_LLB9: if(*((int*)_tmp99)!= 2)
goto _LLBB;_tmpB1=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp99)->f1;_LLBA: _tmpB2=
_tmpB1;goto _LLBC;_LLBB: if(*((int*)_tmp99)!= 1)goto _LLBD;_tmpB2=((struct Cyc_Absyn_Var_e_struct*)
_tmp99)->f1;_LLBC: Cyc_Dump_dumpqvar(_tmpB2);goto _LLA6;_LLBD: if(*((int*)_tmp99)!= 
3)goto _LLBF;_tmpB3=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp99)->f1;_tmpB4=((
struct Cyc_Absyn_Primop_e_struct*)_tmp99)->f2;_LLBE: {struct _tagged_arr _tmp100=
Cyc_Absynpp_prim2str(_tmpB3);switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpB4)){case 1: _LL10B: if(_tmpB3 == (void*)19){Cyc_Dump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpB4))->hd);Cyc_Dump_dump_char((
int)'.');Cyc_Dump_dump_alpha(_tag_arr("size",sizeof(char),5));}else{Cyc_Dump_dump_symbol(
_tmp100);Cyc_Dump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpB4))->hd);}break;case 2: _LL10C: Cyc_Dump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpB4))->hd);Cyc_Dump_dump_symbol(
_tmp100);Cyc_Dump_dump_char((int)' ');Cyc_Dump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpB4->tl))->hd);break;default: _LL10D:(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmp101=_cycalloc(sizeof(*_tmp101));
_tmp101[0]=({struct Cyc_Core_Failure_struct _tmp102;_tmp102.tag=Cyc_Core_Failure;
_tmp102.f1=_tag_arr("Absyndump -- Bad number of arguments to primop",sizeof(char),
47);_tmp102;});_tmp101;}));}goto _LLA6;}_LLBF: if(*((int*)_tmp99)!= 4)goto _LLC1;
_tmpB5=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp99)->f1;_tmpB6=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp99)->f2;_tmpB7=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp99)->f3;_LLC0: Cyc_Dump_dumpexp_prec(
myprec,_tmpB5);if(_tmpB6 != 0)Cyc_Dump_dump_symbol(Cyc_Absynpp_prim2str((void*)
_tmpB6->v));Cyc_Dump_dump_symbol(_tag_arr("=",sizeof(char),2));Cyc_Dump_dumpexp_prec(
myprec,_tmpB7);goto _LLA6;_LLC1: if(*((int*)_tmp99)!= 5)goto _LLC3;_tmpB8=((struct
Cyc_Absyn_Increment_e_struct*)_tmp99)->f1;_tmpB9=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp99)->f2;if((int)_tmpB9 != 0)goto _LLC3;_LLC2: Cyc_Dump_dump_symbol(_tag_arr("++",
sizeof(char),3));Cyc_Dump_dumpexp_prec(myprec,_tmpB8);goto _LLA6;_LLC3: if(*((int*)
_tmp99)!= 5)goto _LLC5;_tmpBA=((struct Cyc_Absyn_Increment_e_struct*)_tmp99)->f1;
_tmpBB=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp99)->f2;if((int)_tmpBB != 
2)goto _LLC5;_LLC4: Cyc_Dump_dump_symbol(_tag_arr("--",sizeof(char),3));Cyc_Dump_dumpexp_prec(
myprec,_tmpBA);goto _LLA6;_LLC5: if(*((int*)_tmp99)!= 5)goto _LLC7;_tmpBC=((struct
Cyc_Absyn_Increment_e_struct*)_tmp99)->f1;_tmpBD=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp99)->f2;if((int)_tmpBD != 1)goto _LLC7;_LLC6: Cyc_Dump_dumpexp_prec(myprec,
_tmpBC);Cyc_Dump_dump_symbol(_tag_arr("++",sizeof(char),3));goto _LLA6;_LLC7: if(*((
int*)_tmp99)!= 5)goto _LLC9;_tmpBE=((struct Cyc_Absyn_Increment_e_struct*)_tmp99)->f1;
_tmpBF=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp99)->f2;if((int)_tmpBF != 
3)goto _LLC9;_LLC8: Cyc_Dump_dumpexp_prec(myprec,_tmpBE);Cyc_Dump_dump_symbol(
_tag_arr("--",sizeof(char),3));goto _LLA6;_LLC9: if(*((int*)_tmp99)!= 6)goto _LLCB;
_tmpC0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp99)->f1;_tmpC1=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp99)->f2;_tmpC2=((struct Cyc_Absyn_Conditional_e_struct*)_tmp99)->f3;_LLCA: Cyc_Dump_dumpexp_prec(
myprec,_tmpC0);Cyc_Dump_dump_char((int)'?');Cyc_Dump_dumpexp_prec(0,_tmpC1);Cyc_Dump_dump_char((
int)':');Cyc_Dump_dumpexp_prec(myprec,_tmpC2);goto _LLA6;_LLCB: if(*((int*)_tmp99)
!= 7)goto _LLCD;_tmpC3=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp99)->f1;_tmpC4=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp99)->f2;_LLCC: Cyc_Dump_dump_char((int)'(');
Cyc_Dump_dumpexp_prec(myprec,_tmpC3);Cyc_Dump_dump_char((int)',');Cyc_Dump_dumpexp_prec(
myprec,_tmpC4);Cyc_Dump_dump_char((int)')');goto _LLA6;_LLCD: if(*((int*)_tmp99)!= 
8)goto _LLCF;_tmpC5=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp99)->f1;_tmpC6=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp99)->f2;_LLCE: _tmpC7=_tmpC5;_tmpC8=
_tmpC6;goto _LLD0;_LLCF: if(*((int*)_tmp99)!= 9)goto _LLD1;_tmpC7=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp99)->f1;_tmpC8=((struct Cyc_Absyn_FnCall_e_struct*)_tmp99)->f2;_LLD0: Cyc_Dump_dumpexp_prec(
myprec,_tmpC7);Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumpexps_prec(20,_tmpC8);
Cyc_Dump_dump_char((int)')');goto _LLA6;_LLD1: if(*((int*)_tmp99)!= 10)goto _LLD3;
_tmpC9=((struct Cyc_Absyn_Throw_e_struct*)_tmp99)->f1;_LLD2: Cyc_Dump_dump_alpha(
_tag_arr("throw",sizeof(char),6));Cyc_Dump_dumpexp_prec(myprec,_tmpC9);goto _LLA6;
_LLD3: if(*((int*)_tmp99)!= 11)goto _LLD5;_tmpCA=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp99)->f1;_LLD4: _tmpCB=_tmpCA;goto _LLD6;_LLD5: if(*((int*)_tmp99)!= 12)goto
_LLD7;_tmpCB=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp99)->f1;_LLD6: Cyc_Dump_dumpexp_prec(
inprec,_tmpCB);goto _LLA6;_LLD7: if(*((int*)_tmp99)!= 13)goto _LLD9;_tmpCC=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp99)->f1;_tmpCD=((struct Cyc_Absyn_Cast_e_struct*)
_tmp99)->f2;_LLD8: Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(_tmpCC);Cyc_Dump_dump_char((
int)')');Cyc_Dump_dumpexp_prec(myprec,_tmpCD);goto _LLA6;_LLD9: if(*((int*)_tmp99)
!= 14)goto _LLDB;_tmpCE=((struct Cyc_Absyn_Address_e_struct*)_tmp99)->f1;_LLDA: Cyc_Dump_dump_char((
int)'&');Cyc_Dump_dumpexp_prec(myprec,_tmpCE);goto _LLA6;_LLDB: if(*((int*)_tmp99)
!= 15)goto _LLDD;_tmpCF=((struct Cyc_Absyn_New_e_struct*)_tmp99)->f1;_tmpD0=((
struct Cyc_Absyn_New_e_struct*)_tmp99)->f2;_LLDC: Cyc_Dump_dump_alpha(_tag_arr("new",
sizeof(char),4));Cyc_Dump_dumpexp_prec(myprec,_tmpD0);goto _LLA6;_LLDD: if(*((int*)
_tmp99)!= 16)goto _LLDF;_tmpD1=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp99)->f1;_LLDE: Cyc_Dump_dump_alpha(_tag_arr("sizeof",sizeof(char),7));Cyc_Dump_dump_char((
int)'(');Cyc_Dump_dumptyp(_tmpD1);Cyc_Dump_dump_char((int)')');goto _LLA6;_LLDF:
if(*((int*)_tmp99)!= 17)goto _LLE1;_tmpD2=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp99)->f1;_LLE0: Cyc_Dump_dump_alpha(_tag_arr("sizeof",sizeof(char),7));Cyc_Dump_dump_char((
int)'(');Cyc_Dump_dumpexp_prec(myprec,_tmpD2);Cyc_Dump_dump_char((int)')');goto
_LLA6;_LLE1: if(*((int*)_tmp99)!= 18)goto _LLE3;_tmpD3=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp99)->f1;_tmpD4=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp99)->f2;if(*((
int*)_tmpD4)!= 0)goto _LLE3;_tmpD5=((struct Cyc_Absyn_StructField_struct*)_tmpD4)->f1;
_LLE2: Cyc_Dump_dump_alpha(_tag_arr("offsetof",sizeof(char),9));Cyc_Dump_dump_char((
int)'(');Cyc_Dump_dumptyp(_tmpD3);Cyc_Dump_dump_char((int)',');Cyc_Dump_dump_alpha(*
_tmpD5);Cyc_Dump_dump_char((int)')');goto _LLA6;_LLE3: if(*((int*)_tmp99)!= 18)
goto _LLE5;_tmpD6=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp99)->f1;_tmpD7=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp99)->f2;if(*((int*)_tmpD7)!= 1)
goto _LLE5;_tmpD8=((struct Cyc_Absyn_TupleIndex_struct*)_tmpD7)->f1;_LLE4: Cyc_Dump_dump_alpha(
_tag_arr("offsetof(",sizeof(char),10));Cyc_Dump_dumptyp(_tmpD6);Cyc_Dump_dump_char((
int)',');Cyc_Dump_dump_alpha((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp104;_tmp104.tag=1;_tmp104.f1=(int)_tmpD8;{void*_tmp103[1]={& _tmp104};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(char),3),_tag_arr(_tmp103,sizeof(void*),1));}}));Cyc_Dump_dump_char((
int)')');goto _LLA6;_LLE5: if(*((int*)_tmp99)!= 19)goto _LLE7;_tmpD9=((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp99)->f1;_tmpDA=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp99)->f2;_LLE6:
Cyc_Dump_dump_alpha(_tag_arr("__gen",sizeof(char),6));Cyc_Dump_dumptvars(_tmpD9);
Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(_tmpDA);Cyc_Dump_dump_char((int)')');
goto _LLA6;_LLE7: if(*((int*)_tmp99)!= 20)goto _LLE9;_tmpDB=((struct Cyc_Absyn_Deref_e_struct*)
_tmp99)->f1;_LLE8: Cyc_Dump_dump_char((int)'*');Cyc_Dump_dumpexp_prec(myprec,
_tmpDB);goto _LLA6;_LLE9: if(*((int*)_tmp99)!= 21)goto _LLEB;_tmpDC=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp99)->f1;_tmpDD=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp99)->f2;_LLEA: Cyc_Dump_dumpexp_prec(
myprec,_tmpDC);Cyc_Dump_dump_char((int)'.');Cyc_Dump_dump_alpha(*_tmpDD);goto
_LLA6;_LLEB: if(*((int*)_tmp99)!= 22)goto _LLED;_tmpDE=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp99)->f1;_tmpDF=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp99)->f2;_LLEC: Cyc_Dump_dumpexp_prec(
myprec,_tmpDE);Cyc_Dump_dump_symbol(_tag_arr("->",sizeof(char),3));Cyc_Dump_dump_alpha(*
_tmpDF);goto _LLA6;_LLED: if(*((int*)_tmp99)!= 23)goto _LLEF;_tmpE0=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp99)->f1;_tmpE1=((struct Cyc_Absyn_Subscript_e_struct*)_tmp99)->f2;_LLEE: Cyc_Dump_dumpexp_prec(
myprec,_tmpE0);Cyc_Dump_dump_char((int)'[');Cyc_Dump_dumpexp(_tmpE1);Cyc_Dump_dump_char((
int)']');goto _LLA6;_LLEF: if(*((int*)_tmp99)!= 24)goto _LLF1;_tmpE2=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp99)->f1;_LLF0: Cyc_Dump_dump_symbol(_tag_arr("\\$(",sizeof(char),4));Cyc_Dump_dumpexps_prec(
20,_tmpE2);Cyc_Dump_dump_char((int)')');goto _LLA6;_LLF1: if(*((int*)_tmp99)!= 25)
goto _LLF3;_tmpE3=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp99)->f1;_tmpE4=((
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp99)->f2;_LLF2: Cyc_Dump_dump_char((int)'(');
Cyc_Dump_dumptyp((*_tmpE3).f3);Cyc_Dump_dump_char((int)')');((void(*)(void(*f)(
struct _tuple14*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr
end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmpE4,_tag_arr("{\\lb}",
sizeof(char),6),_tag_arr("{\\rb}",sizeof(char),6),_tag_arr(",",sizeof(char),2));
goto _LLA6;_LLF3: if(*((int*)_tmp99)!= 26)goto _LLF5;_tmpE5=((struct Cyc_Absyn_Array_e_struct*)
_tmp99)->f1;_LLF4:((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(
Cyc_Dump_dumpde,_tmpE5,_tag_arr("{\\lb}",sizeof(char),6),_tag_arr("{\\rb}",
sizeof(char),6),_tag_arr(",",sizeof(char),2));goto _LLA6;_LLF5: if(*((int*)_tmp99)
!= 27)goto _LLF7;_tmpE6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp99)->f1;
_tmpE7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp99)->f2;_tmpE8=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp99)->f3;_LLF6: Cyc_Dump_dump_alpha(_tag_arr("new",sizeof(char),4));Cyc_Dump_dump_char((
int)'{');Cyc_Dump_dump_alpha(_tag_arr("for",sizeof(char),4));Cyc_Dump_dump_alphastr((*
_tmpE6->name).f2);Cyc_Dump_dump_char((int)'<');Cyc_Dump_dumpexp(_tmpE7);Cyc_Dump_dump_char((
int)':');Cyc_Dump_dumpexp(_tmpE8);Cyc_Dump_dump_char((int)'}');goto _LLA6;_LLF7:
if(*((int*)_tmp99)!= 28)goto _LLF9;_tmpE9=((struct Cyc_Absyn_Struct_e_struct*)
_tmp99)->f1;_tmpEA=((struct Cyc_Absyn_Struct_e_struct*)_tmp99)->f3;_LLF8: Cyc_Dump_dumpqvar(
_tmpE9);((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,
_tmpEA,_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(char),2),_tag_arr(",",
sizeof(char),2));goto _LLA6;_LLF9: if(*((int*)_tmp99)!= 29)goto _LLFB;_tmpEB=((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp99)->f2;_LLFA:((void(*)(void(*f)(struct
_tuple14*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,
struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmpEB,_tag_arr("{",
sizeof(char),2),_tag_arr("}",sizeof(char),2),_tag_arr(",",sizeof(char),2));goto
_LLA6;_LLFB: if(*((int*)_tmp99)!= 30)goto _LLFD;_tmpEC=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp99)->f1;_tmpED=((struct Cyc_Absyn_Tunion_e_struct*)_tmp99)->f3;_LLFC: Cyc_Dump_dumpqvar(
_tmpED->name);if(_tmpEC != 0)((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(
Cyc_Dump_dumpexp,_tmpEC,_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(char),2),
_tag_arr(",",sizeof(char),2));goto _LLA6;_LLFD: if(*((int*)_tmp99)!= 31)goto _LLFF;
_tmpEE=((struct Cyc_Absyn_Enum_e_struct*)_tmp99)->f1;_LLFE: Cyc_Dump_dumpqvar(
_tmpEE);goto _LLA6;_LLFF: if(*((int*)_tmp99)!= 32)goto _LL101;_tmpEF=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp99)->f1;_LL100: Cyc_Dump_dumpqvar(_tmpEF);goto _LLA6;_LL101: if(*((int*)_tmp99)
!= 33)goto _LL103;_tmpF0=((struct Cyc_Absyn_Malloc_e_struct*)_tmp99)->f1;_tmpF1=
_tmpF0.is_calloc;_tmpF2=_tmpF0.rgn;_tmpF3=_tmpF0.elt_type;_tmpF4=_tmpF0.num_elts;
_LL102: if(_tmpF2 != 0){if(_tmpF1)Cyc_Dump_dump_alpha(_tag_arr("rcalloc",sizeof(
char),8));else{Cyc_Dump_dump_alpha(_tag_arr("rmalloc",sizeof(char),8));}Cyc_Dump_dump_char((
int)'(');Cyc_Dump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmpF2));Cyc_Dump_dump_symbol(
_tag_arr(", ",sizeof(char),3));}else{if(_tmpF1)Cyc_Dump_dump_alpha(_tag_arr("calloc",
sizeof(char),7));else{Cyc_Dump_dump_alpha(_tag_arr("malloc",sizeof(char),7));}
Cyc_Dump_dump_char((int)'(');}Cyc_Dump_dumpexp(_tmpF4);if(_tmpF3 != 0){if(_tmpF1)
Cyc_Dump_dump_symbol(_tag_arr(", ",sizeof(char),3));else{Cyc_Dump_dump_symbol(
_tag_arr(" * ",sizeof(char),4));}Cyc_Dump_dump_alpha(_tag_arr("sizeof",sizeof(
char),7));Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(*_tmpF3);Cyc_Dump_dump_symbol(
_tag_arr(")",sizeof(char),2));}Cyc_Dump_dump_symbol(_tag_arr(")",sizeof(char),2));
goto _LLA6;_LL103: if(*((int*)_tmp99)!= 34)goto _LL105;_tmpF5=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp99)->f1;_tmpF6=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp99)->f2;_LL104:((
void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,
_tmpF6,_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(char),2),_tag_arr(",",
sizeof(char),2));goto _LLA6;_LL105: if(*((int*)_tmp99)!= 35)goto _LL107;_tmpF7=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp99)->f1;_LL106: goto _LLA6;_LL107: if(*((int*)
_tmp99)!= 36)goto _LL109;_tmpF8=((struct Cyc_Absyn_Codegen_e_struct*)_tmp99)->f1;
_LL108: goto _LLA6;_LL109: if(*((int*)_tmp99)!= 37)goto _LLA6;_tmpF9=((struct Cyc_Absyn_Fill_e_struct*)
_tmp99)->f1;_LL10A: goto _LLA6;_LLA6:;}if(inprec >= myprec)Cyc_Dump_dump_char((int)')');}
void Cyc_Dump_dumpexp(struct Cyc_Absyn_Exp*e){Cyc_Dump_dumpexp_prec(0,e);}void Cyc_Dump_dumptunionfield(
struct Cyc_Absyn_Tunionfield*ef){Cyc_Dump_dumpqvar(ef->name);if(ef->typs != 0)Cyc_Dump_dumpargs(
ef->typs);}void Cyc_Dump_dumptunionfields(struct Cyc_List_List*fields){((void(*)(
void(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*l,struct _tagged_arr
sep))Cyc_Dump_dump_sep)(Cyc_Dump_dumptunionfield,fields,_tag_arr(",\\\\\n~~",
sizeof(char),7));}void Cyc_Dump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){Cyc_Dump_dumpqvar(
ef->name);if(ef->tag != 0){Cyc_Dump_dump_symbol(_tag_arr(" = ",sizeof(char),4));
Cyc_Dump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}void Cyc_Dump_dumpenumfields(
struct Cyc_List_List*fields){((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Dump_dump_sep)(Cyc_Dump_dumpenumfield,
fields,_tag_arr(",\\\\\n~~",sizeof(char),7));}void Cyc_Dump_dumpaggrfields(struct
Cyc_List_List*fields){for(0;fields != 0;fields=fields->tl){Cyc_Dump_dump_symbol(
_tag_arr("~~",sizeof(char),3));{struct Cyc_Absyn_Aggrfield _tmp106;struct
_tagged_arr*_tmp107;struct Cyc_Absyn_Tqual _tmp108;void*_tmp109;struct Cyc_Absyn_Exp*
_tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_Absyn_Aggrfield*_tmp105=(struct Cyc_Absyn_Aggrfield*)
fields->hd;_tmp106=*_tmp105;_tmp107=_tmp106.name;_tmp108=_tmp106.tq;_tmp109=(
void*)_tmp106.type;_tmp10A=_tmp106.width;_tmp10B=_tmp106.attributes;((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tagged_arr*),struct _tagged_arr*))Cyc_Dump_dumptqtd)(
_tmp108,_tmp109,Cyc_Dump_dump_alphastr,_tmp107);if(_tmp10A != 0){Cyc_Dump_dump_char((
int)':');Cyc_Dump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp10A));}Cyc_Dump_dump_symbol(
_tag_arr(";\\\\\n",sizeof(char),5));}}}void Cyc_Dump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*
td){Cyc_Dump_dumpqvar(td->name);Cyc_Dump_dumptvars(td->tvs);}void Cyc_Dump_dump_boldtypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Dump_dump_boldqvar(td->name);Cyc_Dump_dumptvars(
td->tvs);}static void Cyc_Dump_dumpids(struct Cyc_List_List*vds){for(0;vds != 0;vds=
vds->tl){Cyc_Dump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl
!= 0)Cyc_Dump_dump_char((int)',');}}static void Cyc_Dump_dump_upperbound(struct Cyc_Absyn_Exp*
e){unsigned int i=Cyc_Evexp_eval_const_uint_exp(e);if(i != 1){Cyc_Dump_dump_char((
int)'{');Cyc_Dump_dumpexp(e);Cyc_Dump_dump_char((int)'}');}}void Cyc_Dump_dumptms(
struct Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){f(a);return;}{void*
_tmp10C=(void*)tms->hd;void*_tmp10D;struct Cyc_Absyn_Exp*_tmp10E;void*_tmp10F;
struct Cyc_Absyn_Tqual _tmp110;void*_tmp111;struct Cyc_Absyn_Exp*_tmp112;void*
_tmp113;struct Cyc_Absyn_Tqual _tmp114;void*_tmp115;void*_tmp116;struct Cyc_Absyn_Tqual
_tmp117;void*_tmp118;struct Cyc_Absyn_Exp*_tmp119;void*_tmp11A;struct Cyc_Absyn_Tvar*
_tmp11B;struct Cyc_Absyn_Tqual _tmp11C;void*_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;
void*_tmp11F;struct Cyc_Absyn_Tvar*_tmp120;struct Cyc_Absyn_Tqual _tmp121;void*
_tmp122;void*_tmp123;struct Cyc_Absyn_Tvar*_tmp124;struct Cyc_Absyn_Tqual _tmp125;
void*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Tqual _tmp128;void*
_tmp129;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Tqual _tmp12B;void*_tmp12C;
struct Cyc_Absyn_Tqual _tmp12D;_LL110: if(_tmp10C <= (void*)1?1:*((int*)_tmp10C)!= 1)
goto _LL112;_tmp10D=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp10C)->f1;if(
_tmp10D <= (void*)1?1:*((int*)_tmp10D)!= 1)goto _LL112;_tmp10E=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmp10D)->f1;_tmp10F=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp10C)->f2;
if((int)_tmp10F != 2)goto _LL112;_tmp110=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp10C)->f3;_LL111: Cyc_Dump_dump_pointer(_tag_arr("*",sizeof(char),2));Cyc_Dump_dump_upperbound(
_tmp10E);Cyc_Dump_dump_alpha(_tag_arr("`H",sizeof(char),3));Cyc_Dump_dumptms(tms->tl,
f,a);return;_LL112: if(_tmp10C <= (void*)1?1:*((int*)_tmp10C)!= 1)goto _LL114;
_tmp111=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp10C)->f1;if(_tmp111 <= (
void*)1?1:*((int*)_tmp111)!= 0)goto _LL114;_tmp112=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmp111)->f1;_tmp113=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp10C)->f2;
if((int)_tmp113 != 2)goto _LL114;_tmp114=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp10C)->f3;_LL113: Cyc_Dump_dump_pointer(_tag_arr("@",sizeof(char),2));Cyc_Dump_dump_upperbound(
_tmp112);Cyc_Dump_dump_alpha(_tag_arr("`H",sizeof(char),3));Cyc_Dump_dumptms(tms->tl,
f,a);return;_LL114: if(_tmp10C <= (void*)1?1:*((int*)_tmp10C)!= 1)goto _LL116;
_tmp115=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp10C)->f1;if((int)
_tmp115 != 0)goto _LL116;_tmp116=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp10C)->f2;if((int)_tmp116 != 2)goto _LL116;_tmp117=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp10C)->f3;_LL115: Cyc_Dump_dump_pointer(_tag_arr("{?}",sizeof(char),4));Cyc_Dump_dump_alpha(
_tag_arr("`H",sizeof(char),3));Cyc_Dump_dumptms(tms->tl,f,a);return;_LL116: if(
_tmp10C <= (void*)1?1:*((int*)_tmp10C)!= 1)goto _LL118;_tmp118=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp10C)->f1;if(_tmp118 <= (void*)1?1:*((int*)_tmp118)!= 1)goto _LL118;_tmp119=((
struct Cyc_Absyn_Nullable_ps_struct*)_tmp118)->f1;_tmp11A=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp10C)->f2;if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 1)goto _LL118;_tmp11B=((
struct Cyc_Absyn_VarType_struct*)_tmp11A)->f1;_tmp11C=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp10C)->f3;_LL117: Cyc_Dump_dump_pointer(_tag_arr("*",sizeof(char),2));Cyc_Dump_dump_upperbound(
_tmp119);Cyc_Dump_dump_alphastr(_tmp11B->name);Cyc_Dump_dumptms(tms->tl,f,a);
return;_LL118: if(_tmp10C <= (void*)1?1:*((int*)_tmp10C)!= 1)goto _LL11A;_tmp11D=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp10C)->f1;if(_tmp11D <= (void*)1?1:*((
int*)_tmp11D)!= 0)goto _LL11A;_tmp11E=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmp11D)->f1;_tmp11F=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp10C)->f2;
if(_tmp11F <= (void*)3?1:*((int*)_tmp11F)!= 1)goto _LL11A;_tmp120=((struct Cyc_Absyn_VarType_struct*)
_tmp11F)->f1;_tmp121=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp10C)->f3;_LL119:
Cyc_Dump_dump_pointer(_tag_arr("@",sizeof(char),2));Cyc_Dump_dump_upperbound(
_tmp11E);Cyc_Dump_dump_alphastr(_tmp120->name);Cyc_Dump_dumptms(tms->tl,f,a);
return;_LL11A: if(_tmp10C <= (void*)1?1:*((int*)_tmp10C)!= 1)goto _LL11C;_tmp122=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp10C)->f1;if((int)_tmp122 != 0)
goto _LL11C;_tmp123=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp10C)->f2;if(
_tmp123 <= (void*)3?1:*((int*)_tmp123)!= 1)goto _LL11C;_tmp124=((struct Cyc_Absyn_VarType_struct*)
_tmp123)->f1;_tmp125=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp10C)->f3;_LL11B:
Cyc_Dump_dump_pointer(_tag_arr("{?}",sizeof(char),4));Cyc_Dump_dump_alphastr(
_tmp124->name);Cyc_Dump_dumptms(tms->tl,f,a);return;_LL11C: if(_tmp10C <= (void*)1?
1:*((int*)_tmp10C)!= 1)goto _LL11E;_tmp126=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp10C)->f1;if(_tmp126 <= (void*)1?1:*((int*)_tmp126)!= 1)goto _LL11E;_tmp127=((
struct Cyc_Absyn_Nullable_ps_struct*)_tmp126)->f1;_tmp128=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp10C)->f3;_LL11D: Cyc_Dump_dump_pointer(_tag_arr("*",sizeof(char),2));Cyc_Dump_dump_upperbound(
_tmp127);Cyc_Dump_dumptms(tms->tl,f,a);return;_LL11E: if(_tmp10C <= (void*)1?1:*((
int*)_tmp10C)!= 1)goto _LL120;_tmp129=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp10C)->f1;if(_tmp129 <= (void*)1?1:*((int*)_tmp129)!= 0)goto _LL120;_tmp12A=((
struct Cyc_Absyn_NonNullable_ps_struct*)_tmp129)->f1;_tmp12B=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp10C)->f3;_LL11F: Cyc_Dump_dump_pointer(_tag_arr("@",sizeof(char),2));Cyc_Dump_dump_upperbound(
_tmp12A);Cyc_Dump_dumptms(tms->tl,f,a);return;_LL120: if(_tmp10C <= (void*)1?1:*((
int*)_tmp10C)!= 1)goto _LL122;_tmp12C=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp10C)->f1;if((int)_tmp12C != 0)goto _LL122;_tmp12D=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp10C)->f3;_LL121: Cyc_Dump_dump_pointer(_tag_arr("{?}",sizeof(char),4));Cyc_Dump_dumptms(
tms->tl,f,a);return;_LL122:;_LL123: {int next_is_pointer=0;if(tms->tl != 0){void*
_tmp12E=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_LL125: if(
_tmp12E <= (void*)1?1:*((int*)_tmp12E)!= 1)goto _LL127;_LL126: next_is_pointer=1;
goto _LL124;_LL127:;_LL128: goto _LL124;_LL124:;}if(next_is_pointer)Cyc_Dump_dump_char((
int)'(');Cyc_Dump_dumptms(tms->tl,f,a);if(next_is_pointer)Cyc_Dump_dump_char((
int)')');{void*_tmp12F=(void*)tms->hd;struct Cyc_Absyn_Exp*_tmp130;void*_tmp131;
struct Cyc_List_List*_tmp132;int _tmp133;struct Cyc_Absyn_VarargInfo*_tmp134;struct
Cyc_Core_Opt*_tmp135;struct Cyc_List_List*_tmp136;void*_tmp137;struct Cyc_List_List*
_tmp138;struct Cyc_Position_Segment*_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_Position_Segment*
_tmp13B;int _tmp13C;void*_tmp13D;void*_tmp13E;_LL12A: if((int)_tmp12F != 0)goto
_LL12C;_LL12B: Cyc_Dump_dump_symbol(_tag_arr("[]",sizeof(char),3));goto _LL129;
_LL12C: if(_tmp12F <= (void*)1?1:*((int*)_tmp12F)!= 0)goto _LL12E;_tmp130=((struct
Cyc_Absyn_ConstArray_mod_struct*)_tmp12F)->f1;_LL12D: Cyc_Dump_dump_char((int)'[');
Cyc_Dump_dumpexp(_tmp130);Cyc_Dump_dump_char((int)']');goto _LL129;_LL12E: if(
_tmp12F <= (void*)1?1:*((int*)_tmp12F)!= 2)goto _LL130;_tmp131=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp12F)->f1;if(*((int*)_tmp131)!= 1)goto _LL130;_tmp132=((struct Cyc_Absyn_WithTypes_struct*)
_tmp131)->f1;_tmp133=((struct Cyc_Absyn_WithTypes_struct*)_tmp131)->f2;_tmp134=((
struct Cyc_Absyn_WithTypes_struct*)_tmp131)->f3;_tmp135=((struct Cyc_Absyn_WithTypes_struct*)
_tmp131)->f4;_tmp136=((struct Cyc_Absyn_WithTypes_struct*)_tmp131)->f5;_LL12F: Cyc_Dump_dumpfunargs(
_tmp132,_tmp133,_tmp134,_tmp135,_tmp136);goto _LL129;_LL130: if(_tmp12F <= (void*)1?
1:*((int*)_tmp12F)!= 2)goto _LL132;_tmp137=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp12F)->f1;if(*((int*)_tmp137)!= 0)goto _LL132;_tmp138=((struct Cyc_Absyn_NoTypes_struct*)
_tmp137)->f1;_tmp139=((struct Cyc_Absyn_NoTypes_struct*)_tmp137)->f2;_LL131:((
void(*)(void(*f)(struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr
start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dump_alphastr,
_tmp138,_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(char),2),_tag_arr(", ",
sizeof(char),3));goto _LL129;_LL132: if(_tmp12F <= (void*)1?1:*((int*)_tmp12F)!= 3)
goto _LL134;_tmp13A=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp12F)->f1;_tmp13B=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmp12F)->f2;_tmp13C=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp12F)->f3;_LL133: if(_tmp13C)Cyc_Dump_dumpkindedtvars(_tmp13A);else{Cyc_Dump_dumptvars(
_tmp13A);}goto _LL129;_LL134: if(_tmp12F <= (void*)1?1:*((int*)_tmp12F)!= 4)goto
_LL136;_LL135: goto _LL129;_LL136: if(_tmp12F <= (void*)1?1:*((int*)_tmp12F)!= 1)
goto _LL129;_tmp13D=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp12F)->f1;
_tmp13E=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp12F)->f2;_LL137:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp13F=_cycalloc(sizeof(*
_tmp13F));_tmp13F[0]=({struct Cyc_Core_Impossible_struct _tmp140;_tmp140.tag=Cyc_Core_Impossible;
_tmp140.f1=_tag_arr("dumptms",sizeof(char),8);_tmp140;});_tmp13F;}));_LL129:;}
return;}_LL10F:;}}void Cyc_Dump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(
void*),void*a){struct Cyc_Absyn_Tqual _tmp142;void*_tmp143;struct Cyc_List_List*
_tmp144;struct _tuple10 _tmp141=Cyc_Absynpp_to_tms(tq,t);_tmp142=_tmp141.f1;
_tmp143=_tmp141.f2;_tmp144=_tmp141.f3;Cyc_Dump_dumptq(_tmp142);Cyc_Dump_dumpntyp(
_tmp143);Cyc_Dump_dumptms(Cyc_List_imp_rev(_tmp144),f,a);}void Cyc_Dump_pr_comment(
struct Cyc_Std___cycFILE*outf,struct _tagged_arr s){int depth=0;int len=(int)Cyc_Std_strlen(
s);int i=0;for(0;i < len;i ++){char c=*((const char*)_check_unknown_subscript(s,
sizeof(char),i));if(c != '['){Cyc_Std_fputc((int)c,outf);continue;}Cyc_Std_fputs(
_tag_arr("\\texttt{",sizeof(char),9),outf);i ++;depth ++;for(0;i < len;i ++){char c=*((
const char*)_check_unknown_subscript(s,sizeof(char),i));if(c == ']'){depth --;if(
depth == 0){Cyc_Std_fputc((int)'}',outf);break;}}else{if(c == '[')depth ++;}Cyc_Std_fputc((
int)c,outf);}}}void Cyc_Dump_dump_comment(struct _tagged_arr s){Cyc_Dump_pr_comment(
Cyc_Std_stdout,s);}void Cyc_Dump_dumpdecl(struct Cyc_Absyn_Decl*d,struct
_tagged_arr comment){void*_tmp145=(void*)d->r;struct Cyc_Absyn_Aggrdecl*_tmp146;
struct Cyc_Absyn_Vardecl*_tmp147;struct Cyc_Absyn_Vardecl _tmp148;struct _tuple1*
_tmp149;struct Cyc_Absyn_Tqual _tmp14A;void*_tmp14B;struct Cyc_Absyn_Tuniondecl*
_tmp14C;struct Cyc_Absyn_Tuniondecl _tmp14D;void*_tmp14E;struct _tuple1*_tmp14F;
struct Cyc_List_List*_tmp150;struct Cyc_Core_Opt*_tmp151;int _tmp152;struct Cyc_Absyn_Enumdecl*
_tmp153;struct Cyc_Absyn_Enumdecl _tmp154;void*_tmp155;struct _tuple1*_tmp156;
struct Cyc_Core_Opt*_tmp157;struct Cyc_Absyn_Typedefdecl*_tmp158;_LL139: if(*((int*)
_tmp145)!= 4)goto _LL13B;_tmp146=((struct Cyc_Absyn_Aggr_d_struct*)_tmp145)->f1;
_LL13A: Cyc_Dump_dump_begin();Cyc_Dump_dump_alpha((void*)_tmp146->kind == (void*)1?
_tag_arr("union",sizeof(char),6): _tag_arr("struct",sizeof(char),7));Cyc_Dump_dump_boldqvar(
_tmp146->name);Cyc_Dump_dumptvars(_tmp146->tvs);if(_tmp146->impl == 0)Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(char),3));else{Cyc_Dump_dump_symbol(_tag_arr(" ",sizeof(
char),2));Cyc_Dump_dump_lb();if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp146->impl))->exist_vars != 0){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Dump_egroup)(Cyc_Dump_dumptvar,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp146->impl))->exist_vars,_tag_arr("<",sizeof(char),2),_tag_arr(">",
sizeof(char),2),_tag_arr(",",sizeof(char),2));Cyc_Dump_dump_alpha(_tag_arr("",
sizeof(char),1));}if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp146->impl))->rgn_po
!= 0){Cyc_Dump_dump_char((int)':');Cyc_Dump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp146->impl))->rgn_po);}Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",
sizeof(char),4));Cyc_Dump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp146->impl))->fields);Cyc_Dump_dump_rb();Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(char),3));}Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(
comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(char),2));Cyc_Dump_dump_end();
goto _LL138;_LL13B: if(*((int*)_tmp145)!= 0)goto _LL13D;_tmp147=((struct Cyc_Absyn_Var_d_struct*)
_tmp145)->f1;_tmp148=*_tmp147;_tmp149=_tmp148.name;_tmp14A=_tmp148.tq;_tmp14B=(
void*)_tmp148.type;_LL13C: Cyc_Dump_dump_begin();((void(*)(struct Cyc_Absyn_Tqual
tq,void*t,void(*f)(struct _tuple1*),struct _tuple1*a))Cyc_Dump_dumptqtd)(_tmp14A,
_tmp14B,Cyc_Dump_dump_boldqvar,_tmp149);Cyc_Dump_dump_symbol(_tag_arr(";\n",
sizeof(char),3));Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(comment);Cyc_Dump_dump_symbol(
_tag_arr("\n",sizeof(char),2));Cyc_Dump_dump_end();goto _LL138;_LL13D: if(*((int*)
_tmp145)!= 5)goto _LL13F;_tmp14C=((struct Cyc_Absyn_Tunion_d_struct*)_tmp145)->f1;
_tmp14D=*_tmp14C;_tmp14E=(void*)_tmp14D.sc;_tmp14F=_tmp14D.name;_tmp150=_tmp14D.tvs;
_tmp151=_tmp14D.fields;_tmp152=_tmp14D.is_xtunion;_LL13E: Cyc_Dump_dump_begin();
if(_tmp152)Cyc_Dump_dump_alpha(_tag_arr("xtunion",sizeof(char),8));else{Cyc_Dump_dump_alpha(
_tag_arr("tunion",sizeof(char),7));}Cyc_Dump_dump_boldqvar(_tmp14F);Cyc_Dump_dumptvars(
_tmp150);if(_tmp151 == 0)Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(char),3));
else{Cyc_Dump_dump_symbol(_tag_arr(" ",sizeof(char),2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(
_tag_arr("\\\\\n~~",sizeof(char),6));Cyc_Dump_dumptunionfields((struct Cyc_List_List*)
_tmp151->v);Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",sizeof(char),4));Cyc_Dump_dump_rb();
Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(char),3));}Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(
comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(char),2));Cyc_Dump_dump_end();
goto _LL138;_LL13F: if(*((int*)_tmp145)!= 6)goto _LL141;_tmp153=((struct Cyc_Absyn_Enum_d_struct*)
_tmp145)->f1;_tmp154=*_tmp153;_tmp155=(void*)_tmp154.sc;_tmp156=_tmp154.name;
_tmp157=_tmp154.fields;_LL140: Cyc_Dump_dump_begin();Cyc_Dump_dump_alpha(_tag_arr("enum",
sizeof(char),5));Cyc_Dump_dump_boldqvar(_tmp156);if(_tmp157 == 0)Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(char),3));else{Cyc_Dump_dump_symbol(_tag_arr(" ",sizeof(
char),2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(_tag_arr("\\\\\n~~",sizeof(
char),6));Cyc_Dump_dumpenumfields((struct Cyc_List_List*)_tmp157->v);Cyc_Dump_dump_symbol(
_tag_arr("\\\\\n",sizeof(char),4));Cyc_Dump_dump_rb();Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(char),3));}Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(
comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(char),2));Cyc_Dump_dump_end();
goto _LL138;_LL141: if(*((int*)_tmp145)!= 7)goto _LL143;_tmp158=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp145)->f1;_LL142: Cyc_Dump_dump_begin();Cyc_Dump_dump_alpha(_tag_arr("typedef",
sizeof(char),8));{void*t;if(_tmp158->defn != 0)t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp158->defn))->v;else{t=Cyc_Absyn_new_evar(_tmp158->kind,0);}((
void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*a))Cyc_Dump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp159;
_tmp159.q_const=0;_tmp159.q_volatile=0;_tmp159.q_restrict=0;_tmp159;}),t,Cyc_Dump_dump_boldtypedefname,
_tmp158);Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(char),3));Cyc_Dump_dump_middle();
Cyc_Dump_dump_comment(comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(char),2));
Cyc_Dump_dump_end();goto _LL138;}_LL143: if(*((int*)_tmp145)!= 1)goto _LL145;_LL144:
goto _LL146;_LL145: if(*((int*)_tmp145)!= 2)goto _LL147;_LL146: goto _LL148;_LL147:
if(*((int*)_tmp145)!= 3)goto _LL149;_LL148: goto _LL14A;_LL149: if(*((int*)_tmp145)
!= 8)goto _LL14B;_LL14A: goto _LL14C;_LL14B: if(*((int*)_tmp145)!= 9)goto _LL14D;
_LL14C: goto _LL14E;_LL14D: if(*((int*)_tmp145)!= 10)goto _LL138;_LL14E: goto _LL138;
_LL138:;}static int Cyc_is_other_special(char c){switch(c){case '\\': _LL14F: goto
_LL150;case '"': _LL150: goto _LL151;case ';': _LL151: goto _LL152;case '&': _LL152: goto
_LL153;case '(': _LL153: goto _LL154;case ')': _LL154: goto _LL155;case '|': _LL155: goto
_LL156;case '^': _LL156: goto _LL157;case '<': _LL157: goto _LL158;case '>': _LL158: goto
_LL159;case ' ': _LL159: goto _LL15A;case '\n': _LL15A: goto _LL15B;case '\t': _LL15B:
return 1;default: _LL15C: return 0;}}static struct _tagged_arr Cyc_sh_escape_string(
struct _tagged_arr s){unsigned int _tmp15A=Cyc_Std_strlen(s);int _tmp15B=0;int
_tmp15C=0;{int i=0;for(0;i < _tmp15A;i ++){char _tmp15D=*((const char*)
_check_unknown_subscript(s,sizeof(char),i));if(_tmp15D == '\'')_tmp15B ++;else{if(
Cyc_is_other_special(_tmp15D))_tmp15C ++;}}}if(_tmp15B == 0?_tmp15C == 0: 0)return s;
if(_tmp15B == 0)return(struct _tagged_arr)Cyc_Std_strconcat_l(({struct _tagged_arr*
_tmp15E[3];_tmp15E[2]=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",
sizeof(char),2);_tmp15E[1]=({struct _tagged_arr*_tmp160=_cycalloc(sizeof(*_tmp160));
_tmp160[0]=(struct _tagged_arr)s;_tmp160;});_tmp15E[0]=_init_tag_arr(_cycalloc(
sizeof(struct _tagged_arr)),"'",sizeof(char),2);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp15E,sizeof(struct _tagged_arr*),3));}));{
unsigned int _tmp162=(_tmp15A + _tmp15B)+ _tmp15C;struct _tagged_arr _tmp163=({
unsigned int _tmp167=_tmp162 + 1;char*_tmp168=(char*)_cycalloc_atomic(_check_times(
sizeof(char),_tmp167));struct _tagged_arr _tmp16A=_tag_arr(_tmp168,sizeof(char),
_tmp162 + 1);{unsigned int _tmp169=_tmp167;unsigned int i;for(i=0;i < _tmp169;i ++){
_tmp168[i]='\000';}}_tmp16A;});int _tmp164=0;int _tmp165=0;for(0;_tmp164 < _tmp15A;
_tmp164 ++){char _tmp166=*((const char*)_check_unknown_subscript(s,sizeof(char),
_tmp164));if(_tmp166 == '\''?1: Cyc_is_other_special(_tmp166))*((char*)
_check_unknown_subscript(_tmp163,sizeof(char),_tmp165 ++))='\\';*((char*)
_check_unknown_subscript(_tmp163,sizeof(char),_tmp165 ++))=_tmp166;}return(struct
_tagged_arr)_tmp163;}}static struct _tagged_arr*Cyc_sh_escape_stringptr(struct
_tagged_arr*sp){return({struct _tagged_arr*_tmp16B=_cycalloc(sizeof(*_tmp16B));
_tmp16B[0]=Cyc_sh_escape_string(*sp);_tmp16B;});}static struct Cyc_Lineno_Pos*Cyc_new_pos(){
return({struct Cyc_Lineno_Pos*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->logical_file=
_tag_arr("",sizeof(char),1);_tmp16C->line=Cyc_Core_new_string(0);_tmp16C->line_no=
0;_tmp16C->col=0;_tmp16C;});}struct _tuple15{int f1;struct Cyc_Lineno_Pos*f2;};
static struct _tuple15*Cyc_start2pos(int x){return({struct _tuple15*_tmp16D=
_cycalloc(sizeof(*_tmp16D));_tmp16D->f1=x;_tmp16D->f2=Cyc_new_pos();_tmp16D;});}
static int Cyc_decl2start(struct Cyc_Absyn_Decl*d){return((struct Cyc_Position_Segment*)
_check_null(d->loc))->start;}struct _tuple16{struct Cyc_Lineno_Pos*f1;void*f2;};
static struct Cyc_List_List*Cyc_this_file(struct _tagged_arr file,struct Cyc_List_List*
x){struct Cyc_List_List*_tmp16E=0;for(0;x != 0;x=x->tl){if(Cyc_Std_strcmp(((*((
struct _tuple16*)x->hd)).f1)->logical_file,file)== 0)_tmp16E=({struct Cyc_List_List*
_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->hd=(struct _tuple16*)x->hd;_tmp16F->tl=
_tmp16E;_tmp16F;});}_tmp16E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp16E);return _tmp16E;}static int Cyc_lineno(struct Cyc_Lineno_Pos*p){return p->line_no;}
static struct Cyc_List_List*Cyc_flatten_decls(struct Cyc_List_List*decls){struct Cyc_List_List*
_tmp170=0;while(decls != 0){void*_tmp171=(void*)((struct Cyc_Absyn_Decl*)decls->hd)->r;
struct Cyc_List_List*_tmp172;struct Cyc_List_List*_tmp173;struct Cyc_List_List*
_tmp174;_LL15F: if(*((int*)_tmp171)!= 8)goto _LL161;_tmp172=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp171)->f2;_LL160: _tmp173=_tmp172;goto _LL162;_LL161: if(*((int*)_tmp171)!= 9)
goto _LL163;_tmp173=((struct Cyc_Absyn_Using_d_struct*)_tmp171)->f2;_LL162: _tmp174=
_tmp173;goto _LL164;_LL163: if(*((int*)_tmp171)!= 10)goto _LL165;_tmp174=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp171)->f1;_LL164: decls=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp174,decls->tl);
goto _LL15E;_LL165: if(*((int*)_tmp171)!= 4)goto _LL167;_LL166: goto _LL168;_LL167:
if(*((int*)_tmp171)!= 0)goto _LL169;_LL168: goto _LL16A;_LL169: if(*((int*)_tmp171)
!= 5)goto _LL16B;_LL16A: goto _LL16C;_LL16B: if(*((int*)_tmp171)!= 6)goto _LL16D;
_LL16C: goto _LL16E;_LL16D: if(*((int*)_tmp171)!= 7)goto _LL16F;_LL16E: goto _LL170;
_LL16F: if(*((int*)_tmp171)!= 1)goto _LL171;_LL170: goto _LL172;_LL171: if(*((int*)
_tmp171)!= 2)goto _LL173;_LL172: goto _LL174;_LL173: if(*((int*)_tmp171)!= 3)goto
_LL15E;_LL174: _tmp170=({struct Cyc_List_List*_tmp175=_cycalloc(sizeof(*_tmp175));
_tmp175->hd=(struct Cyc_Absyn_Decl*)decls->hd;_tmp175->tl=_tmp170;_tmp175;});
decls=decls->tl;goto _LL15E;_LL15E:;}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp170);}static struct Cyc_List_List*Cyc_cycdoc_files=0;
static void Cyc_add_other(struct _tagged_arr s){Cyc_cycdoc_files=({struct Cyc_List_List*
_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->hd=({struct _tagged_arr*_tmp177=
_cycalloc(sizeof(*_tmp177));_tmp177[0]=s;_tmp177;});_tmp176->tl=Cyc_cycdoc_files;
_tmp176;});}static struct Cyc_List_List*Cyc_cycargs=0;static void Cyc_add_cycarg(
struct _tagged_arr s){Cyc_cycargs=({struct Cyc_List_List*_tmp178=_cycalloc(sizeof(*
_tmp178));_tmp178->hd=({struct _tagged_arr*_tmp179=_cycalloc(sizeof(*_tmp179));
_tmp179[0]=s;_tmp179;});_tmp178->tl=Cyc_cycargs;_tmp178;});}static char _tmp17A[8]="cyclone";
static struct _tagged_arr Cyc_cyclone_file={_tmp17A,_tmp17A,_tmp17A + 8};static void
Cyc_set_cyclone_file(struct _tagged_arr s){Cyc_cyclone_file=s;}struct _tuple17{int
f1;struct Cyc_Absyn_Decl*f2;};struct _tuple18{struct Cyc_Lineno_Pos*f1;struct Cyc_Absyn_Decl*
f2;};static void Cyc_process_file(struct _tagged_arr filename){struct _tagged_arr
_tmp17B=Cyc_Filename_chop_extension(filename);struct _tagged_arr _tmp17C=Cyc_Std_strconcat((
struct _tagged_arr)_tmp17B,_tag_arr(".cyp",sizeof(char),5));struct _tagged_arr
_tmp17D=Cyc_Std_str_sepstr(({struct Cyc_List_List*_tmp19E=_cycalloc(sizeof(*
_tmp19E));_tmp19E->hd=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",
sizeof(char),1);_tmp19E->tl=((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cycargs));
_tmp19E;}),_tag_arr(" ",sizeof(char),2));struct _tagged_arr _tmp17E=({struct Cyc_Std_String_pa_struct
_tmp19D;_tmp19D.tag=0;_tmp19D.f1=(struct _tagged_arr)Cyc_sh_escape_string(
filename);{struct Cyc_Std_String_pa_struct _tmp19C;_tmp19C.tag=0;_tmp19C.f1=(
struct _tagged_arr)Cyc_sh_escape_string((struct _tagged_arr)_tmp17C);{struct Cyc_Std_String_pa_struct
_tmp19B;_tmp19B.tag=0;_tmp19B.f1=(struct _tagged_arr)_tmp17D;{struct Cyc_Std_String_pa_struct
_tmp19A;_tmp19A.tag=0;_tmp19A.f1=(struct _tagged_arr)Cyc_cyclone_file;{void*
_tmp199[4]={& _tmp19A,& _tmp19B,& _tmp19C,& _tmp19D};Cyc_Std_aprintf(_tag_arr("%s %s -E -o %s -x cyc %s",
sizeof(char),25),_tag_arr(_tmp199,sizeof(void*),4));}}}}});if(Cyc_Std_system((
struct _tagged_arr)_tmp17E)!= 0){({void*_tmp17F[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\nError: preprocessing\n",sizeof(char),23),_tag_arr(_tmp17F,sizeof(
void*),0));});return;}Cyc_Position_reset_position((struct _tagged_arr)_tmp17C);{
struct Cyc_Std___cycFILE*in_file=(struct Cyc_Std___cycFILE*)_check_null(Cyc_Std_fopen((
struct _tagged_arr)_tmp17C,_tag_arr("r",sizeof(char),2)));Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp180=Cyc_Parse_parse_file(in_file);Cyc_Lex_lex_init();Cyc_Std_file_close((
struct Cyc_Std___cycFILE*)in_file);_tmp180=Cyc_flatten_decls(_tmp180);{struct Cyc_List_List*
_tmp181=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_start2pos,((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_decl2start,_tmp180));Cyc_Lineno_poss_of_abss((
struct _tagged_arr)_tmp17C,_tmp181);Cyc_Std_remove((struct _tagged_arr)_tmp17C);{
struct Cyc_List_List*_tmp182=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*
f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple15*))Cyc_Core_snd,_tmp181),_tmp180);_tmp182=((struct Cyc_List_List*(*)(
struct _tagged_arr file,struct Cyc_List_List*x))Cyc_this_file)(filename,_tmp182);{
struct Cyc_List_List*_tmp183=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Lineno_Pos*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(struct
Cyc_Lineno_Pos*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)(struct _tuple18*))Cyc_Core_fst,_tmp182)),((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Decl*(*)(struct _tuple18*))Cyc_Core_snd,_tmp182));struct Cyc_Std___cycFILE*
_tmp184=(struct Cyc_Std___cycFILE*)_check_null(Cyc_Std_fopen(filename,_tag_arr("r",
sizeof(char),2)));struct Cyc_Lexing_lexbuf*_tmp185=Cyc_Lexing_from_file(_tmp184);
struct Cyc_List_List*_tmp186=0;struct _tuple11*comment;while((comment=((struct
_tuple11*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_token)(_tmp185))!= 0){_tmp186=({
struct Cyc_List_List*_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187->hd=(struct
_tuple11*)_check_null(comment);_tmp187->tl=_tmp186;_tmp187;});}Cyc_Std_fclose(
_tmp184);_tmp186=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp186);{struct Cyc_List_List*_tmp188=((struct Cyc_List_List*(*)(struct _tuple15*(*
f)(int),struct Cyc_List_List*x))Cyc_List_map)(Cyc_start2pos,((struct Cyc_List_List*(*)(
int(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((int(*)(struct
_tuple11*))Cyc_Core_fst,_tmp186));Cyc_Lineno_poss_of_abss(filename,_tmp188);{
struct Cyc_List_List*_tmp189=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*
f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple15*))Cyc_Core_snd,_tmp188),((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))
Cyc_Core_snd,_tmp186));_tmp189=Cyc_this_file(filename,_tmp189);{struct Cyc_List_List*
_tmp18A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*f)(
struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple16*))Cyc_Core_fst,_tmp189)),((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple16*))
Cyc_Core_snd,_tmp189));while(_tmp183 != 0?_tmp18A != 0: 0){struct _tuple17 _tmp18C;
int _tmp18D;struct Cyc_Absyn_Decl*_tmp18E;struct _tuple17*_tmp18B=(struct _tuple17*)
_tmp183->hd;_tmp18C=*_tmp18B;_tmp18D=_tmp18C.f1;_tmp18E=_tmp18C.f2;{struct
_tuple11 _tmp190;int _tmp191;void*_tmp192;struct _tuple11*_tmp18F=(struct _tuple11*)
_tmp18A->hd;_tmp190=*_tmp18F;_tmp191=_tmp190.f1;_tmp192=_tmp190.f2;{void*_tmp193=
_tmp192;struct _tagged_arr _tmp194;struct _tagged_arr _tmp195;_LL176: if(*((int*)
_tmp193)!= 1)goto _LL178;_tmp194=((struct Cyc_Standalone_struct*)_tmp193)->f1;
_LL177: Cyc_Dump_dump_comment((struct _tagged_arr)_tmp194);Cyc_Dump_dump_symbol(
_tag_arr("\n",sizeof(char),2));_tmp18A=_tmp18A->tl;goto _LL175;_LL178: if(*((int*)
_tmp193)!= 0)goto _LL175;_tmp195=((struct Cyc_MatchDecl_struct*)_tmp193)->f1;
_LL179: if(_tmp191 < _tmp18D){_tmp18A=_tmp18A->tl;continue;}if(_tmp183->tl != 0){
struct _tuple17 _tmp197;int _tmp198;struct _tuple17*_tmp196=(struct _tuple17*)((
struct Cyc_List_List*)_check_null(_tmp183->tl))->hd;_tmp197=*_tmp196;_tmp198=
_tmp197.f1;if(_tmp198 < _tmp191){_tmp183=_tmp183->tl;continue;}}Cyc_Dump_dumpdecl(
_tmp18E,(struct _tagged_arr)_tmp195);Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stdout);
_tmp183=_tmp183->tl;_tmp18A=_tmp18A->tl;goto _LL175;_LL175:;}}}}}}}}}}}}struct
_tuple19{struct _tagged_arr f1;int f2;struct _tagged_arr f3;void*f4;struct _tagged_arr
f5;};int Cyc_main(int argc,struct _tagged_arr argv){struct Cyc_List_List*options=({
struct _tuple19*_tmp1A1[4];_tmp1A1[3]=({struct _tuple19*_tmp1AB=_cycalloc(sizeof(*
_tmp1AB));_tmp1AB->f1=_tag_arr("-B",sizeof(char),3);_tmp1AB->f2=1;_tmp1AB->f3=
_tag_arr("<file>",sizeof(char),7);_tmp1AB->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1AD;_tmp1AD.tag=1;_tmp1AD.f1=Cyc_add_cycarg;_tmp1AD;});_tmp1AC;});_tmp1AB->f5=
_tag_arr("Add to the list of directories to search for compiler files",sizeof(
char),60);_tmp1AB;});_tmp1A1[2]=({struct _tuple19*_tmp1A8=_cycalloc(sizeof(*
_tmp1A8));_tmp1A8->f1=_tag_arr("-I",sizeof(char),3);_tmp1A8->f2=1;_tmp1A8->f3=
_tag_arr("<dir>",sizeof(char),6);_tmp1A8->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1AA;_tmp1AA.tag=1;_tmp1AA.f1=Cyc_add_cycarg;_tmp1AA;});_tmp1A9;});_tmp1A8->f5=
_tag_arr("Add to the list of directories to search for include files",sizeof(
char),59);_tmp1A8;});_tmp1A1[1]=({struct _tuple19*_tmp1A5=_cycalloc(sizeof(*
_tmp1A5));_tmp1A5->f1=_tag_arr("-D",sizeof(char),3);_tmp1A5->f2=1;_tmp1A5->f3=
_tag_arr("<name>[=<value>]",sizeof(char),17);_tmp1A5->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1A7;_tmp1A7.tag=1;_tmp1A7.f1=Cyc_add_cycarg;_tmp1A7;});_tmp1A6;});_tmp1A5->f5=
_tag_arr("Pass definition to preprocessor",sizeof(char),32);_tmp1A5;});_tmp1A1[0]=({
struct _tuple19*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->f1=_tag_arr("-cyclone",
sizeof(char),9);_tmp1A2->f2=0;_tmp1A2->f3=_tag_arr(" <file>",sizeof(char),8);
_tmp1A2->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp1A3=_cycalloc(sizeof(*
_tmp1A3));_tmp1A3[0]=({struct Cyc_Arg_String_spec_struct _tmp1A4;_tmp1A4.tag=5;
_tmp1A4.f1=Cyc_set_cyclone_file;_tmp1A4;});_tmp1A3;});_tmp1A2->f5=_tag_arr("Use <file> as the cyclone compiler",
sizeof(char),35);_tmp1A2;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1A1,sizeof(struct _tuple19*),4));});Cyc_Arg_parse(options,Cyc_add_other,
_tag_arr("Options:",sizeof(char),9),argv);{struct Cyc_List_List*_tmp1A0=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cycdoc_files);for(0;
_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){Cyc_process_file(*((struct _tagged_arr*)_tmp1A0->hd));}}
return 0;}
