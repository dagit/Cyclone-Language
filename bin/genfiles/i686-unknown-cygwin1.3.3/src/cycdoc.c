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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};struct
_tagged_arr Cyc_Core_new_string(unsigned int);struct _tuple0{void*f1;void*f2;};
void*Cyc_Core_fst(struct _tuple0*);void*Cyc_Core_snd(struct _tuple0*);extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Cstdio___abstractFILE;struct
Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stdout;extern struct Cyc_Std___cycFILE*
Cyc_Std_stderr;int Cyc_Std_remove(struct _tagged_arr);int Cyc_Std_fclose(struct Cyc_Std___cycFILE*);
int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);struct Cyc_Std___cycFILE*Cyc_Std_fopen(
struct _tagged_arr __filename,struct _tagged_arr __modes);int Cyc_Std_fputc(int __c,
struct Cyc_Std___cycFILE*__stream);int Cyc_Std_fputs(struct _tagged_arr __s,struct
Cyc_Std___cycFILE*__stream);extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};void Cyc_Std_file_close(struct Cyc_Std___cycFILE*);
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
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};extern unsigned char Cyc_Lexing_Error[
10];struct Cyc_Lexing_Error_struct{unsigned char*tag;struct _tagged_arr f1;};struct
Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;
struct _tagged_arr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;
int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct
Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _tagged_arr,int,void*);
void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _tagged_arr lex_base;
struct _tagged_arr lex_backtrk;struct _tagged_arr lex_default;struct _tagged_arr
lex_trans;struct _tagged_arr lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(
struct Cyc_Std___cycFILE*);int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _tagged_arr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern unsigned char Cyc_List_Nth[8];
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
unsigned int Cyc_Std_strlen(struct _tagged_arr s);int Cyc_Std_strcmp(struct
_tagged_arr s1,struct _tagged_arr s2);struct _tagged_arr Cyc_Std_strconcat(struct
_tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_Std_strconcat_l(struct Cyc_List_List*);
struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,struct _tagged_arr);
struct _tagged_arr Cyc_Std_substring(struct _tagged_arr,int ofs,unsigned int n);
struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
line_no;int col;};void Cyc_Lineno_poss_of_abss(struct _tagged_arr filename,struct Cyc_List_List*
places);struct _tagged_arr Cyc_Filename_chop_extension(struct _tagged_arr);extern
unsigned char Cyc_Position_Exit[9];void Cyc_Position_reset_position(struct
_tagged_arr);struct Cyc_Position_Segment;struct Cyc_Position_Error{struct
_tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;
};extern unsigned char Cyc_Position_Nocontext[14];extern unsigned char Cyc_Array_Array_mismatch[
19];struct Cyc_Std__Div{int quot;int rem;};struct Cyc_Std__Ldiv{int quot;int rem;};int
Cyc_Std_system(struct _tagged_arr);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;extern unsigned char Cyc_Arg_Bad[8];struct Cyc_Arg_Bad_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Arg_Error[10];
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
f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_ForArray_s_struct{
int tag;struct Cyc_Absyn_ForArrayInfo f1;};struct Cyc_Absyn_ResetRegion_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
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
_tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[15];void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
struct _tuple4{void*f1;struct _tuple1*f2;};struct _tuple4 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc_Std___cycFILE*f);
struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;int
Cyc_yyparse();extern unsigned char Cyc_AbstractDeclarator_tok[27];struct Cyc_AbstractDeclarator_tok_struct{
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
unsigned char*tag;void*f1;};int Cyc_yyparse();extern unsigned char Cyc_YY1[8];
struct Cyc_YY1_struct{unsigned char*tag;struct _tuple8*f1;};extern unsigned char Cyc_YYINITIALSVAL[
18];struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;
int last_column;};struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern struct
_tagged_arr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _tagged_arr Cyc_Absynpp_char_escape(unsigned char);struct _tagged_arr Cyc_Absynpp_string_escape(
struct _tagged_arr);struct _tagged_arr Cyc_Absynpp_prim2str(void*p);struct _tuple10{
struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple10 Cyc_Absynpp_to_tms(
struct Cyc_Absyn_Tqual tq,void*t);void Cyc_Lex_lex_init();struct Cyc_Position_Segment{
int start;int end;};struct Cyc_MatchDecl_struct{int tag;struct _tagged_arr f1;};struct
Cyc_Standalone_struct{int tag;struct _tagged_arr f1;};struct _tuple11{int f1;void*f2;
};struct _tuple11*Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf);const int Cyc_lex_base[
15]=(const int[15]){0,- 4,0,- 3,1,2,3,0,4,6,7,- 1,8,9,- 2};const int Cyc_lex_backtrk[15]=(
const int[15]){- 1,- 1,3,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[
15]=(const int[15]){1,0,- 1,0,- 1,- 1,6,- 1,8,8,8,0,6,6,0};const int Cyc_lex_trans[266]=(
const int[266]){0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,
0,6,0,0,0,0,0,0,0,4,5,7,12,9,2,10,10,13,13,0,11,- 1,14,- 1,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,- 1,- 1,0,- 1,- 1,- 1,- 1};
const int Cyc_lex_check[266]=(const int[266]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,7,- 1,5,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,2,4,5,6,8,0,9,10,12,13,- 1,9,10,12,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,
- 1,6,8,- 1,9,10,12,13};int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*
lbuf){int state;int base;int backtrk;int c;state=start_state;if(state >= 0){lbuf->lex_last_pos=(
lbuf->lex_start_pos=lbuf->lex_curr_pos);lbuf->lex_last_action=- 1;}else{state=(-
state)- 1;}while(1){base=Cyc_lex_base[_check_known_subscript_notnull(15,state)];
if(base < 0)return(- base)- 1;backtrk=Cyc_lex_backtrk[
_check_known_subscript_notnull(15,state)];if(backtrk >= 0){lbuf->lex_last_pos=
lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}else{c=(int)*((
unsigned char*)_check_unknown_subscript(lbuf->lex_buffer,sizeof(unsigned char),
lbuf->lex_curr_pos ++));if(c == - 1)c=256;}if(Cyc_lex_check[
_check_known_subscript_notnull(266,base + c)]== state)state=Cyc_lex_trans[
_check_known_subscript_notnull(266,base + c)];else{state=Cyc_lex_default[
_check_known_subscript_notnull(15,state)];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({struct Cyc_Lexing_Error_struct _tmp1;
_tmp1.tag=Cyc_Lexing_Error;_tmp1.f1=_tag_arr("empty token",sizeof(unsigned char),
12);_tmp1;});_tmp0;}));else{return lbuf->lex_last_action;}}else{if(c == 256)lbuf->lex_eof_reached=
0;}}}struct _tuple11*Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
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
lexbuf){return Cyc_token_rec(lexbuf,0);}struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
12];extern unsigned char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*
effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
_tag_arr(" %s",sizeof(unsigned char),4),_tag_arr(_tmpE,sizeof(void*),1));}});
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
void*)0)({struct Cyc_Std_String_pa_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(struct
_tagged_arr)s;{void*_tmp16[1]={& _tmp17};Cyc_Std_printf(_tag_arr(" %s",sizeof(
unsigned char),4),_tag_arr(_tmp16,sizeof(void*),1));}});else{({struct Cyc_Std_String_pa_struct
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
_tag_arr("::",sizeof(unsigned char),3));}Cyc_Dump_dump_alpha(*((struct
_tagged_arr*)_tmp1A->hd));}if(Cyc_Dump_qvar_to_Cids)Cyc_Dump_dump_symbol(
_tag_arr("_",sizeof(unsigned char),2));else{Cyc_Dump_dump_symbol(_tag_arr("::",
sizeof(unsigned char),3));}Cyc_Dump_dump_alpha(*(*v).f2);}else{Cyc_Dump_dump_alpha(*(*
v).f2);}}void Cyc_Dump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict)Cyc_Dump_dump_alpha(
_tag_arr("restrict",sizeof(unsigned char),9));if(tq.q_volatile)Cyc_Dump_dump_alpha(
_tag_arr("volatile",sizeof(unsigned char),9));if(tq.q_const)Cyc_Dump_dump_alpha(
_tag_arr("const",sizeof(unsigned char),6));}void Cyc_Dump_dumpscope(void*sc){void*
_tmp1F=sc;_LLE: if((int)_tmp1F != 0)goto _LL10;_LLF: Cyc_Dump_dump_alpha(_tag_arr("static",
sizeof(unsigned char),7));return;_LL10: if((int)_tmp1F != 2)goto _LL12;_LL11:
return;_LL12: if((int)_tmp1F != 3)goto _LL14;_LL13: Cyc_Dump_dump_alpha(_tag_arr("extern",
sizeof(unsigned char),7));return;_LL14: if((int)_tmp1F != 4)goto _LL16;_LL15: Cyc_Dump_dump_alpha(
_tag_arr("extern \"C\"",sizeof(unsigned char),11));return;_LL16: if((int)_tmp1F != 
1)goto _LL18;_LL17: Cyc_Dump_dump_alpha(_tag_arr("abstract",sizeof(unsigned char),
9));return;_LL18: if((int)_tmp1F != 5)goto _LLD;_LL19: Cyc_Dump_dump_alpha(_tag_arr("register",
sizeof(unsigned char),9));return;_LLD:;}void Cyc_Dump_dumpkind(void*k){void*
_tmp20=k;_LL1B: if((int)_tmp20 != 0)goto _LL1D;_LL1C: Cyc_Dump_dump_alpha(_tag_arr("A",
sizeof(unsigned char),2));return;_LL1D: if((int)_tmp20 != 1)goto _LL1F;_LL1E: Cyc_Dump_dump_alpha(
_tag_arr("M",sizeof(unsigned char),2));return;_LL1F: if((int)_tmp20 != 2)goto _LL21;
_LL20: Cyc_Dump_dump_alpha(_tag_arr("B",sizeof(unsigned char),2));return;_LL21:
if((int)_tmp20 != 3)goto _LL23;_LL22: Cyc_Dump_dump_alpha(_tag_arr("R",sizeof(
unsigned char),2));return;_LL23: if((int)_tmp20 != 4)goto _LL25;_LL24: Cyc_Dump_dump_alpha(
_tag_arr("E",sizeof(unsigned char),2));return;_LL25: if((int)_tmp20 != 5)goto _LL1A;
_LL26: Cyc_Dump_dump_alpha(_tag_arr("I",sizeof(unsigned char),2));return;_LL1A:;}
void Cyc_Dump_dumptps(struct Cyc_List_List*ts){Cyc_Dump_egroup(Cyc_Dump_dumptyp,ts,
_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",sizeof(unsigned char),2),
_tag_arr(", ",sizeof(unsigned char),3));Cyc_Dump_dump_alpha(_tag_arr("",sizeof(
unsigned char),1));}void Cyc_Dump_dumptvar(struct Cyc_Absyn_Tvar*tv){Cyc_Dump_dump_alphastr(
tv->name);}void Cyc_Dump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){Cyc_Dump_dump_alphastr(
tv->name);{void*_tmp21=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp22;void*
_tmp23;void*_tmp24;_LL28: if(*((int*)_tmp21)!= 0)goto _LL2A;_tmp22=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmp21)->f1;if((int)_tmp22 != 2)goto _LL2A;_LL29: goto _LL27;
_LL2A: if(*((int*)_tmp21)!= 2)goto _LL2C;_tmp23=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp21)->f2;_LL2B: _tmp24=_tmp23;goto _LL2D;_LL2C: if(*((int*)_tmp21)!= 0)goto _LL2E;
_tmp24=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp21)->f1;_LL2D: Cyc_Dump_dump_symbol(
_tag_arr("::",sizeof(unsigned char),3));Cyc_Dump_dumpkind(_tmp24);goto _LL27;
_LL2E:;_LL2F: Cyc_Dump_dump_symbol(_tag_arr("::?",sizeof(unsigned char),4));goto
_LL27;_LL27:;}}void Cyc_Dump_dumptvars(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Dump_egroup)(Cyc_Dump_dumptvar,tvs,
_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",sizeof(unsigned char),2),
_tag_arr(",",sizeof(unsigned char),2));Cyc_Dump_dump_alpha(_tag_arr("",sizeof(
unsigned char),1));}void Cyc_Dump_dumpkindedtvars(struct Cyc_List_List*tvs){((void(*)(
void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_egroup)(Cyc_Dump_dumpkindedtvar,
tvs,_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",sizeof(unsigned char),2),
_tag_arr(",",sizeof(unsigned char),2));Cyc_Dump_dump_alpha(_tag_arr("",sizeof(
unsigned char),1));}struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Dump_dumparg(
struct _tuple12*pr){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Dump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Dump_ignore,0);}void Cyc_Dump_dumpargs(struct
Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple12*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(
Cyc_Dump_dumparg,ts,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(
unsigned char),2),_tag_arr(", ",sizeof(unsigned char),3));}int Cyc_Dump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmp25=(void*)tms->hd;_LL31:
if(_tmp25 <= (void*)1?1:*((int*)_tmp25)!= 1)goto _LL33;_LL32: return 1;_LL33:;_LL34:
return 0;_LL30:;}}static void Cyc_Dump_dumprgn(void*t){void*_tmp26=Cyc_Tcutil_compress(
t);_LL36: if((int)_tmp26 != 2)goto _LL38;_LL37: Cyc_Dump_dump_alpha(_tag_arr("`H",
sizeof(unsigned char),3));goto _LL35;_LL38:;_LL39: Cyc_Dump_dumpntyp(t);goto _LL35;
_LL35:;}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static
struct _tuple13 Cyc_Dump_effects_split(void*t){struct Cyc_List_List*rgions=0;struct
Cyc_List_List*effects=0;{void*_tmp27=Cyc_Tcutil_compress(t);void*_tmp28;struct
Cyc_List_List*_tmp29;_LL3B: if(_tmp27 <= (void*)3?1:*((int*)_tmp27)!= 19)goto _LL3D;
_tmp28=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp27)->f1;_LL3C: rgions=({
struct Cyc_List_List*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->hd=(void*)_tmp28;
_tmp2A->tl=rgions;_tmp2A;});goto _LL3A;_LL3D: if(_tmp27 <= (void*)3?1:*((int*)
_tmp27)!= 20)goto _LL3F;_tmp29=((struct Cyc_Absyn_JoinEff_struct*)_tmp27)->f1;
_LL3E: for(0;_tmp29 != 0;_tmp29=_tmp29->tl){struct Cyc_List_List*_tmp2C;struct Cyc_List_List*
_tmp2D;struct _tuple13 _tmp2B=Cyc_Dump_effects_split((void*)_tmp29->hd);_tmp2C=
_tmp2B.f1;_tmp2D=_tmp2B.f2;rgions=Cyc_List_imp_append(_tmp2C,rgions);effects=Cyc_List_imp_append(
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
void*_tmp55;void*_tmp56;void*_tmp57;void*_tmp58;void*_tmp59;int _tmp5A;void*
_tmp5B;void*_tmp5C;int _tmp5D;struct Cyc_List_List*_tmp5E;struct Cyc_Absyn_AggrInfo
_tmp5F;void*_tmp60;struct Cyc_List_List*_tmp61;void*_tmp62;struct Cyc_List_List*
_tmp63;struct _tuple1*_tmp64;struct Cyc_List_List*_tmp65;struct Cyc_Core_Opt*_tmp66;
void*_tmp67;_LL42: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 7)goto _LL44;_LL43:
goto _LL45;_LL44: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 8)goto _LL46;_LL45: goto
_LL47;_LL46: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 4)goto _LL48;_LL47: return;
_LL48: if((int)_tmp33 != 0)goto _LL4A;_LL49: Cyc_Dump_dump_alpha(_tag_arr("void",
sizeof(unsigned char),5));return;_LL4A: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 
1)goto _LL4C;_tmp34=((struct Cyc_Absyn_VarType_struct*)_tmp33)->f1;_LL4B: Cyc_Dump_dump_alphastr(
_tmp34->name);return;_LL4C: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 0)goto _LL4E;
_tmp35=((struct Cyc_Absyn_Evar_struct*)_tmp33)->f1;_tmp36=((struct Cyc_Absyn_Evar_struct*)
_tmp33)->f2;if(_tmp36 != 0)goto _LL4E;_tmp37=((struct Cyc_Absyn_Evar_struct*)_tmp33)->f3;
_LL4D: Cyc_Dump_dump_char((int)'%');if(_tmp35 == 0)Cyc_Dump_dump_char((int)'?');
else{Cyc_Dump_dumpkind((void*)_tmp35->v);}Cyc_Dump_dump_symbol((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp69;_tmp69.tag=1;_tmp69.f1=(int)((
unsigned int)_tmp37);{void*_tmp68[1]={& _tmp69};Cyc_Std_aprintf(_tag_arr("(%d)",
sizeof(unsigned char),5),_tag_arr(_tmp68,sizeof(void*),1));}}));return;_LL4E: if(
_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 0)goto _LL50;_tmp38=((struct Cyc_Absyn_Evar_struct*)
_tmp33)->f1;_tmp39=((struct Cyc_Absyn_Evar_struct*)_tmp33)->f2;if(_tmp39 == 0)goto
_LL50;_tmp3A=*_tmp39;_tmp3B=(void*)_tmp3A.v;_tmp3C=((struct Cyc_Absyn_Evar_struct*)
_tmp33)->f3;_LL4F: Cyc_Dump_dumpntyp(_tmp3B);return;_LL50: if(_tmp33 <= (void*)3?1:*((
int*)_tmp33)!= 2)goto _LL52;_tmp3D=((struct Cyc_Absyn_TunionType_struct*)_tmp33)->f1;
_tmp3E=(void*)_tmp3D.tunion_info;_tmp3F=_tmp3D.targs;_tmp40=(void*)_tmp3D.rgn;
_LL51:{void*_tmp6A=_tmp3E;struct Cyc_Absyn_UnknownTunionInfo _tmp6B;struct _tuple1*
_tmp6C;int _tmp6D;struct Cyc_Absyn_Tuniondecl**_tmp6E;struct Cyc_Absyn_Tuniondecl*
_tmp6F;struct Cyc_Absyn_Tuniondecl _tmp70;struct _tuple1*_tmp71;int _tmp72;_LL89: if(*((
int*)_tmp6A)!= 0)goto _LL8B;_tmp6B=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp6A)->f1;
_tmp6C=_tmp6B.name;_tmp6D=_tmp6B.is_xtunion;_LL8A: _tmp71=_tmp6C;_tmp72=_tmp6D;
goto _LL8C;_LL8B: if(*((int*)_tmp6A)!= 1)goto _LL88;_tmp6E=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp6A)->f1;_tmp6F=*_tmp6E;_tmp70=*_tmp6F;_tmp71=_tmp70.name;_tmp72=_tmp70.is_xtunion;
_LL8C: if(_tmp72)Cyc_Dump_dump_alpha(_tag_arr("xtunion",sizeof(unsigned char),8));
else{Cyc_Dump_dump_alpha(_tag_arr("tunion",sizeof(unsigned char),7));}{void*
_tmp73=Cyc_Tcutil_compress(_tmp40);_LL8E: if((int)_tmp73 != 2)goto _LL90;_LL8F: goto
_LL8D;_LL90:;_LL91: Cyc_Dump_dumptyp(_tmp40);goto _LL8D;_LL8D:;}Cyc_Dump_dumpqvar(
_tmp71);Cyc_Dump_dumptps(_tmp3F);goto _LL88;_LL88:;}goto _LL41;_LL52: if(_tmp33 <= (
void*)3?1:*((int*)_tmp33)!= 3)goto _LL54;_tmp41=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp33)->f1;_tmp42=(void*)_tmp41.field_info;_tmp43=_tmp41.targs;_LL53:{void*
_tmp74=_tmp42;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp75;struct _tuple1*_tmp76;
struct _tuple1*_tmp77;int _tmp78;struct Cyc_Absyn_Tuniondecl*_tmp79;struct Cyc_Absyn_Tuniondecl
_tmp7A;struct _tuple1*_tmp7B;int _tmp7C;struct Cyc_Absyn_Tunionfield*_tmp7D;struct
Cyc_Absyn_Tunionfield _tmp7E;struct _tuple1*_tmp7F;_LL93: if(*((int*)_tmp74)!= 0)
goto _LL95;_tmp75=((struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp74)->f1;_tmp76=
_tmp75.tunion_name;_tmp77=_tmp75.field_name;_tmp78=_tmp75.is_xtunion;_LL94:
_tmp7B=_tmp76;_tmp7C=_tmp78;_tmp7F=_tmp77;goto _LL96;_LL95: if(*((int*)_tmp74)!= 1)
goto _LL92;_tmp79=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp74)->f1;_tmp7A=*
_tmp79;_tmp7B=_tmp7A.name;_tmp7C=_tmp7A.is_xtunion;_tmp7D=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp74)->f2;_tmp7E=*_tmp7D;_tmp7F=_tmp7E.name;_LL96: if(_tmp7C)Cyc_Dump_dump_alpha(
_tag_arr("xtunion",sizeof(unsigned char),8));else{Cyc_Dump_dump_alpha(_tag_arr("tunion",
sizeof(unsigned char),7));}Cyc_Dump_dumpqvar(_tmp7B);Cyc_Dump_dump_char((int)'.');
Cyc_Dump_dumpqvar(_tmp7F);Cyc_Dump_dumptps(_tmp43);goto _LL92;_LL92:;}goto _LL41;
_LL54: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 12)goto _LL56;_tmp44=((struct Cyc_Absyn_EnumType_struct*)
_tmp33)->f1;_LL55: Cyc_Dump_dump_alpha(_tag_arr("enum",sizeof(unsigned char),5));
Cyc_Dump_dumpqvar(_tmp44);return;_LL56: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 
13)goto _LL58;_tmp45=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp33)->f1;_LL57: Cyc_Dump_dump_alpha(
_tag_arr("enum",sizeof(unsigned char),5));Cyc_Dump_dump_symbol(_tag_arr(" ",
sizeof(unsigned char),2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(_tag_arr("\\\\\n~~",
sizeof(unsigned char),6));Cyc_Dump_dumpenumfields(_tmp45);Cyc_Dump_dump_symbol(
_tag_arr("\\\\\n",sizeof(unsigned char),4));Cyc_Dump_dump_rb();return;_LL58: if(
_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 5)goto _LL5A;_tmp46=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f1;if((int)_tmp46 != 0)goto _LL5A;_tmp47=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f2;if((int)_tmp47 != 2)goto _LL5A;_LL59: Cyc_Dump_dump_alpha(_tag_arr("int",
sizeof(unsigned char),4));return;_LL5A: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 
5)goto _LL5C;_tmp48=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)
_tmp48 != 0)goto _LL5C;_tmp49=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;
if((int)_tmp49 != 0)goto _LL5C;_LL5B: Cyc_Dump_dump_alpha(_tag_arr("signed char",
sizeof(unsigned char),12));return;_LL5C: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 
5)goto _LL5E;_tmp4A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)
_tmp4A != 1)goto _LL5E;_tmp4B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;
if((int)_tmp4B != 0)goto _LL5E;_LL5D: Cyc_Dump_dump_alpha(_tag_arr("unsigned char",
sizeof(unsigned char),14));return;_LL5E: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 
5)goto _LL60;_tmp4C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)
_tmp4C != 0)goto _LL60;_tmp4D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;
if((int)_tmp4D != 1)goto _LL60;_LL5F: Cyc_Dump_dump_alpha(_tag_arr("short",sizeof(
unsigned char),6));return;_LL60: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 5)goto
_LL62;_tmp4E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp4E
!= 1)goto _LL62;_tmp4F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((
int)_tmp4F != 1)goto _LL62;_LL61: Cyc_Dump_dump_alpha(_tag_arr("unsigned short",
sizeof(unsigned char),15));return;_LL62: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 
5)goto _LL64;_tmp50=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)
_tmp50 != 1)goto _LL64;_tmp51=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;
if((int)_tmp51 != 2)goto _LL64;_LL63: Cyc_Dump_dump_alpha(_tag_arr("unsigned int",
sizeof(unsigned char),13));return;_LL64: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 
5)goto _LL66;_tmp52=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)
_tmp52 != 0)goto _LL66;_tmp53=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;
if((int)_tmp53 != 3)goto _LL66;if(!Cyc_Dump_to_VC)goto _LL66;_LL65: Cyc_Dump_dump_alpha(
_tag_arr("__int64",sizeof(unsigned char),8));return;_LL66: if(_tmp33 <= (void*)3?1:*((
int*)_tmp33)!= 5)goto _LL68;_tmp54=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f1;if((int)_tmp54 != 0)goto _LL68;_tmp55=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f2;if((int)_tmp55 != 3)goto _LL68;_LL67: Cyc_Dump_dump_alpha(_tag_arr("long long",
sizeof(unsigned char),10));return;_LL68: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 
5)goto _LL6A;_tmp56=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)
_tmp56 != 1)goto _LL6A;_tmp57=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;
if((int)_tmp57 != 3)goto _LL6A;if(!Cyc_Dump_to_VC)goto _LL6A;_LL69: Cyc_Dump_dump_alpha(
_tag_arr("unsigned __int64",sizeof(unsigned char),17));return;_LL6A: if(_tmp33 <= (
void*)3?1:*((int*)_tmp33)!= 5)goto _LL6C;_tmp58=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f1;if((int)_tmp58 != 1)goto _LL6C;_tmp59=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f2;if((int)_tmp59 != 3)goto _LL6C;_LL6B: Cyc_Dump_dump_alpha(_tag_arr("unsigned long long",
sizeof(unsigned char),19));return;_LL6C: if((int)_tmp33 != 1)goto _LL6E;_LL6D: Cyc_Dump_dump_alpha(
_tag_arr("float",sizeof(unsigned char),6));return;_LL6E: if(_tmp33 <= (void*)3?1:*((
int*)_tmp33)!= 6)goto _LL70;_tmp5A=((struct Cyc_Absyn_DoubleType_struct*)_tmp33)->f1;
_LL6F: if(_tmp5A)Cyc_Dump_dump_alpha(_tag_arr("long double",sizeof(unsigned char),
12));else{Cyc_Dump_dump_alpha(_tag_arr("double",sizeof(unsigned char),7));}
return;_LL70: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 14)goto _LL72;_tmp5B=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp33)->f1;_LL71: Cyc_Dump_dump_alpha(
_tag_arr("sizeof_t<",sizeof(unsigned char),10));Cyc_Dump_dumptyp(_tmp5B);Cyc_Dump_dump_alpha(
_tag_arr(">",sizeof(unsigned char),2));return;_LL72: if(_tmp33 <= (void*)3?1:*((
int*)_tmp33)!= 17)goto _LL74;_tmp5C=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp33)->f1;_LL73: Cyc_Dump_dump_alpha(_tag_arr("tag_t<",sizeof(unsigned char),7));
Cyc_Dump_dumptyp(_tmp5C);Cyc_Dump_dump_alpha(_tag_arr(">",sizeof(unsigned char),
2));return;_LL74: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 18)goto _LL76;_tmp5D=((
struct Cyc_Absyn_TypeInt_struct*)_tmp33)->f1;_LL75: Cyc_Dump_dump_alpha((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp81;_tmp81.tag=1;_tmp81.f1=(int)((
unsigned int)_tmp5D);{void*_tmp80[1]={& _tmp81};Cyc_Std_aprintf(_tag_arr("`%d",
sizeof(unsigned char),4),_tag_arr(_tmp80,sizeof(void*),1));}}));return;_LL76: if(
_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 9)goto _LL78;_tmp5E=((struct Cyc_Absyn_TupleType_struct*)
_tmp33)->f1;_LL77: Cyc_Dump_dump_symbol(_tag_arr("\\$",sizeof(unsigned char),3));
Cyc_Dump_dumpargs(_tmp5E);return;_LL78: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 
10)goto _LL7A;_tmp5F=((struct Cyc_Absyn_AggrType_struct*)_tmp33)->f1;_tmp60=(void*)
_tmp5F.aggr_info;_tmp61=_tmp5F.targs;_LL79: {void*_tmp83;struct _tuple1*_tmp84;
struct _tuple4 _tmp82=Cyc_Absyn_aggr_kinded_name(_tmp60);_tmp83=_tmp82.f1;_tmp84=
_tmp82.f2;Cyc_Dump_dump_alpha(_tmp83 == (void*)1?_tag_arr("union",sizeof(
unsigned char),6): _tag_arr("struct",sizeof(unsigned char),7));Cyc_Dump_dumpqvar(
_tmp84);Cyc_Dump_dumptps(_tmp61);return;}_LL7A: if(_tmp33 <= (void*)3?1:*((int*)
_tmp33)!= 11)goto _LL7C;_tmp62=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp33)->f1;_tmp63=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp33)->f2;_LL7B: Cyc_Dump_dump_alpha(
_tmp62 == (void*)1?_tag_arr("union",sizeof(unsigned char),6): _tag_arr("struct",
sizeof(unsigned char),7));Cyc_Dump_dump_lb();Cyc_Dump_dumpaggrfields(_tmp63);Cyc_Dump_dump_rb();
return;_LL7C: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 16)goto _LL7E;_tmp64=((
struct Cyc_Absyn_TypedefType_struct*)_tmp33)->f1;_tmp65=((struct Cyc_Absyn_TypedefType_struct*)
_tmp33)->f2;_tmp66=((struct Cyc_Absyn_TypedefType_struct*)_tmp33)->f4;_LL7D:(Cyc_Dump_dumpqvar(
_tmp64),Cyc_Dump_dumptps(_tmp65));return;_LL7E: if(_tmp33 <= (void*)3?1:*((int*)
_tmp33)!= 15)goto _LL80;_tmp67=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp33)->f1;_LL7F: Cyc_Dump_dumprgn(_tmp67);return;_LL80: if((int)_tmp33 != 2)goto
_LL82;_LL81: goto _LL83;_LL82: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 19)goto
_LL84;_LL83: goto _LL85;_LL84: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 21)goto
_LL86;_LL85: goto _LL87;_LL86: if(_tmp33 <= (void*)3?1:*((int*)_tmp33)!= 20)goto
_LL41;_LL87: return;_LL41:;}void Cyc_Dump_dumpvaropt(struct Cyc_Core_Opt*vo){if(vo
!= 0)Cyc_Dump_dump_alphastr((struct _tagged_arr*)vo->v);}void Cyc_Dump_dumpfunarg(
struct _tuple2*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),
struct Cyc_Core_Opt*))Cyc_Dump_dumptqtd)((*t).f2,(*t).f3,Cyc_Dump_dumpvaropt,(*t).f1);}
void Cyc_Dump_dump_rgncmp(struct _tuple0*cmp){struct _tuple0 _tmp86;void*_tmp87;void*
_tmp88;struct _tuple0*_tmp85=cmp;_tmp86=*_tmp85;_tmp87=_tmp86.f1;_tmp88=_tmp86.f2;
Cyc_Dump_dumptyp(_tmp87);Cyc_Dump_dump_char((int)'<');Cyc_Dump_dumptyp(_tmp88);}
void Cyc_Dump_dump_rgnpo(struct Cyc_List_List*rgn_po){((void(*)(void(*f)(struct
_tuple0*),struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Dump_dump_sep)(Cyc_Dump_dump_rgncmp,
rgn_po,_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Dump_dumpfunargs(struct
Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){Cyc_Dump_dump_char((int)'(');for(0;args != 0;
args=args->tl){Cyc_Dump_dumpfunarg((struct _tuple2*)args->hd);if((args->tl != 0?1:
c_varargs)?1: cyc_varargs != 0)Cyc_Dump_dump_symbol(_tag_arr(", ",sizeof(
unsigned char),3));}if(c_varargs)Cyc_Dump_dump_symbol(_tag_arr("...",sizeof(
unsigned char),4));else{if(cyc_varargs != 0){struct _tuple2*_tmp89=({struct _tuple2*
_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->f1=cyc_varargs->name;_tmp8A->f2=
cyc_varargs->tq;_tmp8A->f3=(void*)cyc_varargs->type;_tmp8A;});Cyc_Dump_dump_symbol(
_tag_arr("...",sizeof(unsigned char),4));if(cyc_varargs->inject)Cyc_Dump_dump_alpha(
_tag_arr(" inject",sizeof(unsigned char),8));Cyc_Dump_dumpfunarg(_tmp89);}}if(
effopt != 0){Cyc_Dump_dump_semi();Cyc_Dump_dumpeff((void*)effopt->v);}if(rgn_po != 
0){Cyc_Dump_dump_char((int)':');Cyc_Dump_dump_rgnpo(rgn_po);}Cyc_Dump_dump_char((
int)')');}void Cyc_Dump_dumptyp(void*t){((void(*)(struct Cyc_Absyn_Tqual,void*,
void(*f)(int),int))Cyc_Dump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp8B;_tmp8B.q_const=
0;_tmp8B.q_volatile=0;_tmp8B.q_restrict=0;_tmp8B;}),t,(void(*)(int x))Cyc_Dump_ignore,
0);}void Cyc_Dump_dumpdesignator(void*d){void*_tmp8C=d;struct Cyc_Absyn_Exp*_tmp8D;
struct _tagged_arr*_tmp8E;_LL98: if(*((int*)_tmp8C)!= 0)goto _LL9A;_tmp8D=((struct
Cyc_Absyn_ArrayElement_struct*)_tmp8C)->f1;_LL99: Cyc_Dump_dump_symbol(_tag_arr(".[",
sizeof(unsigned char),3));Cyc_Dump_dumpexp(_tmp8D);Cyc_Dump_dump_char((int)']');
goto _LL97;_LL9A: if(*((int*)_tmp8C)!= 1)goto _LL97;_tmp8E=((struct Cyc_Absyn_FieldName_struct*)
_tmp8C)->f1;_LL9B: Cyc_Dump_dump_char((int)'.');Cyc_Dump_dump_alpha(*_tmp8E);goto
_LL97;_LL97:;}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_Dump_dumpde(struct _tuple14*de){Cyc_Dump_egroup(Cyc_Dump_dumpdesignator,(*
de).f1,_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",sizeof(unsigned char),2),
_tag_arr("=",sizeof(unsigned char),2));Cyc_Dump_dumpexp((*de).f2);}void Cyc_Dump_dumpexps_prec(
int inprec,struct Cyc_List_List*es){((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),
int env,struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Dump_group_c)(Cyc_Dump_dumpexp_prec,inprec,es,_tag_arr("",
sizeof(unsigned char),1),_tag_arr("",sizeof(unsigned char),1),_tag_arr(",",
sizeof(unsigned char),2));}void Cyc_Dump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*
e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= myprec)Cyc_Dump_dump_char((int)'(');{
void*_tmp8F=(void*)e->r;void*_tmp90;void*_tmp91;unsigned char _tmp92;void*_tmp93;
void*_tmp94;short _tmp95;void*_tmp96;void*_tmp97;int _tmp98;void*_tmp99;void*
_tmp9A;int _tmp9B;void*_tmp9C;void*_tmp9D;long long _tmp9E;void*_tmp9F;struct
_tagged_arr _tmpA0;void*_tmpA1;void*_tmpA2;struct _tagged_arr _tmpA3;struct _tuple1*
_tmpA4;struct _tuple1*_tmpA5;void*_tmpA6;struct Cyc_List_List*_tmpA7;struct Cyc_Absyn_Exp*
_tmpA8;struct Cyc_Core_Opt*_tmpA9;struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Exp*
_tmpAB;void*_tmpAC;struct Cyc_Absyn_Exp*_tmpAD;void*_tmpAE;struct Cyc_Absyn_Exp*
_tmpAF;void*_tmpB0;struct Cyc_Absyn_Exp*_tmpB1;void*_tmpB2;struct Cyc_Absyn_Exp*
_tmpB3;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;struct Cyc_Absyn_Exp*
_tmpB6;struct Cyc_Absyn_Exp*_tmpB7;struct Cyc_Absyn_Exp*_tmpB8;struct Cyc_List_List*
_tmpB9;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_List_List*_tmpBB;struct Cyc_Absyn_Exp*
_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Exp*_tmpBE;void*_tmpBF;struct
Cyc_Absyn_Exp*_tmpC0;struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_Absyn_Exp*_tmpC2;
struct Cyc_Absyn_Exp*_tmpC3;void*_tmpC4;struct Cyc_Absyn_Exp*_tmpC5;void*_tmpC6;
void*_tmpC7;struct _tagged_arr*_tmpC8;void*_tmpC9;void*_tmpCA;unsigned int _tmpCB;
struct Cyc_List_List*_tmpCC;void*_tmpCD;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*
_tmpCF;struct _tagged_arr*_tmpD0;struct Cyc_Absyn_Exp*_tmpD1;struct _tagged_arr*
_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_List_List*
_tmpD5;struct _tuple2*_tmpD6;struct Cyc_List_List*_tmpD7;struct Cyc_List_List*
_tmpD8;struct Cyc_Absyn_Vardecl*_tmpD9;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*
_tmpDB;struct _tuple1*_tmpDC;struct Cyc_List_List*_tmpDD;struct Cyc_List_List*
_tmpDE;struct Cyc_List_List*_tmpDF;struct Cyc_Absyn_Tunionfield*_tmpE0;struct
_tuple1*_tmpE1;struct _tuple1*_tmpE2;struct Cyc_Absyn_MallocInfo _tmpE3;int _tmpE4;
struct Cyc_Absyn_Exp*_tmpE5;void**_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Core_Opt*
_tmpE8;struct Cyc_List_List*_tmpE9;struct Cyc_Absyn_Stmt*_tmpEA;struct Cyc_Absyn_Fndecl*
_tmpEB;struct Cyc_Absyn_Exp*_tmpEC;_LL9D: if(*((int*)_tmp8F)!= 0)goto _LL9F;_tmp90=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp8F)->f1;if(_tmp90 <= (void*)1?1:*((
int*)_tmp90)!= 0)goto _LL9F;_tmp91=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp90)->f1;
_tmp92=((struct Cyc_Absyn_Char_c_struct*)_tmp90)->f2;_LL9E: Cyc_Dump_dump_char((
int)'\'');Cyc_Dump_dump_alpha(Cyc_Absynpp_char_escape(_tmp92));Cyc_Dump_dump_char((
int)'\'');goto _LL9C;_LL9F: if(*((int*)_tmp8F)!= 0)goto _LLA1;_tmp93=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp8F)->f1;if(_tmp93 <= (void*)1?1:*((int*)
_tmp93)!= 1)goto _LLA1;_tmp94=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp93)->f1;
_tmp95=((struct Cyc_Absyn_Short_c_struct*)_tmp93)->f2;_LLA0: Cyc_Dump_dump_alpha((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpEE;_tmpEE.tag=1;_tmpEE.f1=(
int)((unsigned int)((int)_tmp95));{void*_tmpED[1]={& _tmpEE};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmpED,sizeof(void*),1));}}));
goto _LL9C;_LLA1: if(*((int*)_tmp8F)!= 0)goto _LLA3;_tmp96=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8F)->f1;if(_tmp96 <= (void*)1?1:*((int*)_tmp96)!= 2)goto _LLA3;_tmp97=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp96)->f1;if((int)_tmp97 != 0)goto _LLA3;_tmp98=((
struct Cyc_Absyn_Int_c_struct*)_tmp96)->f2;_LLA2: Cyc_Dump_dump_alpha((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpF0;_tmpF0.tag=1;_tmpF0.f1=(int)((
unsigned int)_tmp98);{void*_tmpEF[1]={& _tmpF0};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(unsigned char),3),_tag_arr(_tmpEF,sizeof(void*),1));}}));goto _LL9C;_LLA3:
if(*((int*)_tmp8F)!= 0)goto _LLA5;_tmp99=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8F)->f1;if(_tmp99 <= (void*)1?1:*((int*)_tmp99)!= 2)goto _LLA5;_tmp9A=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp99)->f1;if((int)_tmp9A != 1)goto _LLA5;_tmp9B=((
struct Cyc_Absyn_Int_c_struct*)_tmp99)->f2;_LLA4: Cyc_Dump_dump_alpha((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpF2;_tmpF2.tag=1;_tmpF2.f1=(int)((
unsigned int)_tmp9B);{void*_tmpF1[1]={& _tmpF2};Cyc_Std_aprintf(_tag_arr("%du",
sizeof(unsigned char),4),_tag_arr(_tmpF1,sizeof(void*),1));}}));goto _LL9C;_LLA5:
if(*((int*)_tmp8F)!= 0)goto _LLA7;_tmp9C=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8F)->f1;if(_tmp9C <= (void*)1?1:*((int*)_tmp9C)!= 3)goto _LLA7;_tmp9D=(void*)((
struct Cyc_Absyn_LongLong_c_struct*)_tmp9C)->f1;_tmp9E=((struct Cyc_Absyn_LongLong_c_struct*)
_tmp9C)->f2;_LLA6: Cyc_Dump_dump_alpha(_tag_arr("<<FIX LONG LONG CONSTANT>>",
sizeof(unsigned char),27));goto _LL9C;_LLA7: if(*((int*)_tmp8F)!= 0)goto _LLA9;
_tmp9F=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp8F)->f1;if(_tmp9F <= (void*)1?
1:*((int*)_tmp9F)!= 4)goto _LLA9;_tmpA0=((struct Cyc_Absyn_Float_c_struct*)_tmp9F)->f1;
_LLA8: Cyc_Dump_dump_alpha(_tmpA0);goto _LL9C;_LLA9: if(*((int*)_tmp8F)!= 0)goto
_LLAB;_tmpA1=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp8F)->f1;if((int)_tmpA1
!= 0)goto _LLAB;_LLAA: Cyc_Dump_dump_alpha(_tag_arr("NULL",sizeof(unsigned char),5));
goto _LL9C;_LLAB: if(*((int*)_tmp8F)!= 0)goto _LLAD;_tmpA2=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8F)->f1;if(_tmpA2 <= (void*)1?1:*((int*)_tmpA2)!= 5)goto _LLAD;_tmpA3=((struct
Cyc_Absyn_String_c_struct*)_tmpA2)->f1;_LLAC: Cyc_Dump_dump_char((int)'"');Cyc_Dump_dump_alpha(
Cyc_Absynpp_string_escape(_tmpA3));Cyc_Dump_dump_char((int)'"');goto _LL9C;_LLAD:
if(*((int*)_tmp8F)!= 2)goto _LLAF;_tmpA4=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp8F)->f1;_LLAE: _tmpA5=_tmpA4;goto _LLB0;_LLAF: if(*((int*)_tmp8F)!= 1)goto _LLB1;
_tmpA5=((struct Cyc_Absyn_Var_e_struct*)_tmp8F)->f1;_LLB0: Cyc_Dump_dumpqvar(
_tmpA5);goto _LL9C;_LLB1: if(*((int*)_tmp8F)!= 3)goto _LLB3;_tmpA6=(void*)((struct
Cyc_Absyn_Primop_e_struct*)_tmp8F)->f1;_tmpA7=((struct Cyc_Absyn_Primop_e_struct*)
_tmp8F)->f2;_LLB2: {struct _tagged_arr _tmpF3=Cyc_Absynpp_prim2str(_tmpA6);switch(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA7)){case 1: _LLFF: if(_tmpA6 == (
void*)19){Cyc_Dump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpA7))->hd);Cyc_Dump_dump_char((int)'.');Cyc_Dump_dump_alpha(
_tag_arr("size",sizeof(unsigned char),5));}else{Cyc_Dump_dump_symbol(_tmpF3);Cyc_Dump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpA7))->hd);}
break;case 2: _LL100: Cyc_Dump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmpA7))->hd);Cyc_Dump_dump_symbol(_tmpF3);Cyc_Dump_dump_char((
int)' ');Cyc_Dump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpA7->tl))->hd);break;default: _LL101:(int)_throw((void*)({struct
Cyc_Core_Failure_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=({struct Cyc_Core_Failure_struct
_tmpF5;_tmpF5.tag=Cyc_Core_Failure;_tmpF5.f1=_tag_arr("Absyndump -- Bad number of arguments to primop",
sizeof(unsigned char),47);_tmpF5;});_tmpF4;}));}goto _LL9C;}_LLB3: if(*((int*)
_tmp8F)!= 4)goto _LLB5;_tmpA8=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp8F)->f1;
_tmpA9=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp8F)->f2;_tmpAA=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp8F)->f3;_LLB4: Cyc_Dump_dumpexp_prec(myprec,_tmpA8);if(_tmpA9 != 0)Cyc_Dump_dump_symbol(
Cyc_Absynpp_prim2str((void*)_tmpA9->v));Cyc_Dump_dump_symbol(_tag_arr("=",
sizeof(unsigned char),2));Cyc_Dump_dumpexp_prec(myprec,_tmpAA);goto _LL9C;_LLB5:
if(*((int*)_tmp8F)!= 5)goto _LLB7;_tmpAB=((struct Cyc_Absyn_Increment_e_struct*)
_tmp8F)->f1;_tmpAC=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp8F)->f2;if((
int)_tmpAC != 0)goto _LLB7;_LLB6: Cyc_Dump_dump_symbol(_tag_arr("++",sizeof(
unsigned char),3));Cyc_Dump_dumpexp_prec(myprec,_tmpAB);goto _LL9C;_LLB7: if(*((
int*)_tmp8F)!= 5)goto _LLB9;_tmpAD=((struct Cyc_Absyn_Increment_e_struct*)_tmp8F)->f1;
_tmpAE=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp8F)->f2;if((int)_tmpAE != 
2)goto _LLB9;_LLB8: Cyc_Dump_dump_symbol(_tag_arr("--",sizeof(unsigned char),3));
Cyc_Dump_dumpexp_prec(myprec,_tmpAD);goto _LL9C;_LLB9: if(*((int*)_tmp8F)!= 5)goto
_LLBB;_tmpAF=((struct Cyc_Absyn_Increment_e_struct*)_tmp8F)->f1;_tmpB0=(void*)((
struct Cyc_Absyn_Increment_e_struct*)_tmp8F)->f2;if((int)_tmpB0 != 1)goto _LLBB;
_LLBA: Cyc_Dump_dumpexp_prec(myprec,_tmpAF);Cyc_Dump_dump_symbol(_tag_arr("++",
sizeof(unsigned char),3));goto _LL9C;_LLBB: if(*((int*)_tmp8F)!= 5)goto _LLBD;
_tmpB1=((struct Cyc_Absyn_Increment_e_struct*)_tmp8F)->f1;_tmpB2=(void*)((struct
Cyc_Absyn_Increment_e_struct*)_tmp8F)->f2;if((int)_tmpB2 != 3)goto _LLBD;_LLBC: Cyc_Dump_dumpexp_prec(
myprec,_tmpB1);Cyc_Dump_dump_symbol(_tag_arr("--",sizeof(unsigned char),3));goto
_LL9C;_LLBD: if(*((int*)_tmp8F)!= 6)goto _LLBF;_tmpB3=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp8F)->f1;_tmpB4=((struct Cyc_Absyn_Conditional_e_struct*)_tmp8F)->f2;_tmpB5=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp8F)->f3;_LLBE: Cyc_Dump_dumpexp_prec(
myprec,_tmpB3);Cyc_Dump_dump_char((int)'?');Cyc_Dump_dumpexp_prec(0,_tmpB4);Cyc_Dump_dump_char((
int)':');Cyc_Dump_dumpexp_prec(myprec,_tmpB5);goto _LL9C;_LLBF: if(*((int*)_tmp8F)
!= 7)goto _LLC1;_tmpB6=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp8F)->f1;_tmpB7=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp8F)->f2;_LLC0: Cyc_Dump_dump_char((int)'(');
Cyc_Dump_dumpexp_prec(myprec,_tmpB6);Cyc_Dump_dump_char((int)',');Cyc_Dump_dumpexp_prec(
myprec,_tmpB7);Cyc_Dump_dump_char((int)')');goto _LL9C;_LLC1: if(*((int*)_tmp8F)!= 
8)goto _LLC3;_tmpB8=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp8F)->f1;_tmpB9=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp8F)->f2;_LLC2: _tmpBA=_tmpB8;_tmpBB=
_tmpB9;goto _LLC4;_LLC3: if(*((int*)_tmp8F)!= 9)goto _LLC5;_tmpBA=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp8F)->f1;_tmpBB=((struct Cyc_Absyn_FnCall_e_struct*)_tmp8F)->f2;_LLC4: Cyc_Dump_dumpexp_prec(
myprec,_tmpBA);Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumpexps_prec(20,_tmpBB);
Cyc_Dump_dump_char((int)')');goto _LL9C;_LLC5: if(*((int*)_tmp8F)!= 10)goto _LLC7;
_tmpBC=((struct Cyc_Absyn_Throw_e_struct*)_tmp8F)->f1;_LLC6: Cyc_Dump_dump_alpha(
_tag_arr("throw",sizeof(unsigned char),6));Cyc_Dump_dumpexp_prec(myprec,_tmpBC);
goto _LL9C;_LLC7: if(*((int*)_tmp8F)!= 11)goto _LLC9;_tmpBD=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp8F)->f1;_LLC8: _tmpBE=_tmpBD;goto _LLCA;_LLC9: if(*((int*)_tmp8F)!= 12)goto
_LLCB;_tmpBE=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp8F)->f1;_LLCA: Cyc_Dump_dumpexp_prec(
inprec,_tmpBE);goto _LL9C;_LLCB: if(*((int*)_tmp8F)!= 13)goto _LLCD;_tmpBF=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp8F)->f1;_tmpC0=((struct Cyc_Absyn_Cast_e_struct*)
_tmp8F)->f2;_LLCC: Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(_tmpBF);Cyc_Dump_dump_char((
int)')');Cyc_Dump_dumpexp_prec(myprec,_tmpC0);goto _LL9C;_LLCD: if(*((int*)_tmp8F)
!= 14)goto _LLCF;_tmpC1=((struct Cyc_Absyn_Address_e_struct*)_tmp8F)->f1;_LLCE: Cyc_Dump_dump_char((
int)'&');Cyc_Dump_dumpexp_prec(myprec,_tmpC1);goto _LL9C;_LLCF: if(*((int*)_tmp8F)
!= 15)goto _LLD1;_tmpC2=((struct Cyc_Absyn_New_e_struct*)_tmp8F)->f1;_tmpC3=((
struct Cyc_Absyn_New_e_struct*)_tmp8F)->f2;_LLD0: Cyc_Dump_dump_alpha(_tag_arr("new",
sizeof(unsigned char),4));Cyc_Dump_dumpexp_prec(myprec,_tmpC3);goto _LL9C;_LLD1:
if(*((int*)_tmp8F)!= 16)goto _LLD3;_tmpC4=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp8F)->f1;_LLD2: Cyc_Dump_dump_alpha(_tag_arr("sizeof",sizeof(unsigned char),7));
Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(_tmpC4);Cyc_Dump_dump_char((int)')');
goto _LL9C;_LLD3: if(*((int*)_tmp8F)!= 17)goto _LLD5;_tmpC5=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp8F)->f1;_LLD4: Cyc_Dump_dump_alpha(_tag_arr("sizeof",sizeof(unsigned char),7));
Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumpexp_prec(myprec,_tmpC5);Cyc_Dump_dump_char((
int)')');goto _LL9C;_LLD5: if(*((int*)_tmp8F)!= 18)goto _LLD7;_tmpC6=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp8F)->f1;_tmpC7=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp8F)->f2;if(*((int*)_tmpC7)!= 0)goto _LLD7;_tmpC8=((struct Cyc_Absyn_StructField_struct*)
_tmpC7)->f1;_LLD6: Cyc_Dump_dump_alpha(_tag_arr("offsetof",sizeof(unsigned char),
9));Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(_tmpC6);Cyc_Dump_dump_char((
int)',');Cyc_Dump_dump_alpha(*_tmpC8);Cyc_Dump_dump_char((int)')');goto _LL9C;
_LLD7: if(*((int*)_tmp8F)!= 18)goto _LLD9;_tmpC9=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp8F)->f1;_tmpCA=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp8F)->f2;if(*((
int*)_tmpCA)!= 1)goto _LLD9;_tmpCB=((struct Cyc_Absyn_TupleIndex_struct*)_tmpCA)->f1;
_LLD8: Cyc_Dump_dump_alpha(_tag_arr("offsetof(",sizeof(unsigned char),10));Cyc_Dump_dumptyp(
_tmpC9);Cyc_Dump_dump_char((int)',');Cyc_Dump_dump_alpha((struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmpF7;_tmpF7.tag=1;_tmpF7.f1=(int)_tmpCB;{void*
_tmpF6[1]={& _tmpF7};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),
_tag_arr(_tmpF6,sizeof(void*),1));}}));Cyc_Dump_dump_char((int)')');goto _LL9C;
_LLD9: if(*((int*)_tmp8F)!= 19)goto _LLDB;_tmpCC=((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp8F)->f1;_tmpCD=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp8F)->f2;_LLDA:
Cyc_Dump_dump_alpha(_tag_arr("__gen",sizeof(unsigned char),6));Cyc_Dump_dumptvars(
_tmpCC);Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(_tmpCD);Cyc_Dump_dump_char((
int)')');goto _LL9C;_LLDB: if(*((int*)_tmp8F)!= 20)goto _LLDD;_tmpCE=((struct Cyc_Absyn_Deref_e_struct*)
_tmp8F)->f1;_LLDC: Cyc_Dump_dump_char((int)'*');Cyc_Dump_dumpexp_prec(myprec,
_tmpCE);goto _LL9C;_LLDD: if(*((int*)_tmp8F)!= 21)goto _LLDF;_tmpCF=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp8F)->f1;_tmpD0=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp8F)->f2;_LLDE: Cyc_Dump_dumpexp_prec(
myprec,_tmpCF);Cyc_Dump_dump_char((int)'.');Cyc_Dump_dump_alpha(*_tmpD0);goto
_LL9C;_LLDF: if(*((int*)_tmp8F)!= 22)goto _LLE1;_tmpD1=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp8F)->f1;_tmpD2=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp8F)->f2;_LLE0: Cyc_Dump_dumpexp_prec(
myprec,_tmpD1);Cyc_Dump_dump_symbol(_tag_arr("->",sizeof(unsigned char),3));Cyc_Dump_dump_alpha(*
_tmpD2);goto _LL9C;_LLE1: if(*((int*)_tmp8F)!= 23)goto _LLE3;_tmpD3=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp8F)->f1;_tmpD4=((struct Cyc_Absyn_Subscript_e_struct*)_tmp8F)->f2;_LLE2: Cyc_Dump_dumpexp_prec(
myprec,_tmpD3);Cyc_Dump_dump_char((int)'[');Cyc_Dump_dumpexp(_tmpD4);Cyc_Dump_dump_char((
int)']');goto _LL9C;_LLE3: if(*((int*)_tmp8F)!= 24)goto _LLE5;_tmpD5=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp8F)->f1;_LLE4: Cyc_Dump_dump_symbol(_tag_arr("\\$(",sizeof(unsigned char),4));
Cyc_Dump_dumpexps_prec(20,_tmpD5);Cyc_Dump_dump_char((int)')');goto _LL9C;_LLE5:
if(*((int*)_tmp8F)!= 25)goto _LLE7;_tmpD6=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp8F)->f1;_tmpD7=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp8F)->f2;_LLE6: Cyc_Dump_dump_char((
int)'(');Cyc_Dump_dumptyp((*_tmpD6).f3);Cyc_Dump_dump_char((int)')');((void(*)(
void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmpD7,
_tag_arr("{\\lb}",sizeof(unsigned char),6),_tag_arr("{\\rb}",sizeof(
unsigned char),6),_tag_arr(",",sizeof(unsigned char),2));goto _LL9C;_LLE7: if(*((
int*)_tmp8F)!= 26)goto _LLE9;_tmpD8=((struct Cyc_Absyn_Array_e_struct*)_tmp8F)->f1;
_LLE8:((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,
_tmpD8,_tag_arr("{\\lb}",sizeof(unsigned char),6),_tag_arr("{\\rb}",sizeof(
unsigned char),6),_tag_arr(",",sizeof(unsigned char),2));goto _LL9C;_LLE9: if(*((
int*)_tmp8F)!= 27)goto _LLEB;_tmpD9=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp8F)->f1;_tmpDA=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp8F)->f2;_tmpDB=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp8F)->f3;_LLEA: Cyc_Dump_dump_alpha(
_tag_arr("new",sizeof(unsigned char),4));Cyc_Dump_dump_char((int)'{');Cyc_Dump_dump_alpha(
_tag_arr("for",sizeof(unsigned char),4));Cyc_Dump_dump_alphastr((*_tmpD9->name).f2);
Cyc_Dump_dump_char((int)'<');Cyc_Dump_dumpexp(_tmpDA);Cyc_Dump_dump_char((int)':');
Cyc_Dump_dumpexp(_tmpDB);Cyc_Dump_dump_char((int)'}');goto _LL9C;_LLEB: if(*((int*)
_tmp8F)!= 28)goto _LLED;_tmpDC=((struct Cyc_Absyn_Struct_e_struct*)_tmp8F)->f1;
_tmpDD=((struct Cyc_Absyn_Struct_e_struct*)_tmp8F)->f3;_LLEC: Cyc_Dump_dumpqvar(
_tmpDC);((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,
_tmpDD,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LL9C;_LLED: if(*((int*)_tmp8F)!= 
29)goto _LLEF;_tmpDE=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp8F)->f2;_LLEE:((
void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,
_tmpDE,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LL9C;_LLEF: if(*((int*)_tmp8F)!= 
30)goto _LLF1;_tmpDF=((struct Cyc_Absyn_Tunion_e_struct*)_tmp8F)->f1;_tmpE0=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp8F)->f3;_LLF0: Cyc_Dump_dumpqvar(_tmpE0->name);
if(_tmpDF != 0)((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(
Cyc_Dump_dumpexp,_tmpDF,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));goto _LL9C;_LLF1:
if(*((int*)_tmp8F)!= 31)goto _LLF3;_tmpE1=((struct Cyc_Absyn_Enum_e_struct*)_tmp8F)->f1;
_LLF2: Cyc_Dump_dumpqvar(_tmpE1);goto _LL9C;_LLF3: if(*((int*)_tmp8F)!= 32)goto
_LLF5;_tmpE2=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp8F)->f1;_LLF4: Cyc_Dump_dumpqvar(
_tmpE2);goto _LL9C;_LLF5: if(*((int*)_tmp8F)!= 33)goto _LLF7;_tmpE3=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp8F)->f1;_tmpE4=_tmpE3.is_calloc;_tmpE5=_tmpE3.rgn;_tmpE6=_tmpE3.elt_type;
_tmpE7=_tmpE3.num_elts;_LLF6: if(_tmpE5 != 0){if(_tmpE4)Cyc_Dump_dump_alpha(
_tag_arr("rcalloc",sizeof(unsigned char),8));else{Cyc_Dump_dump_alpha(_tag_arr("rmalloc",
sizeof(unsigned char),8));}Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumpexp((struct
Cyc_Absyn_Exp*)_check_null(_tmpE5));Cyc_Dump_dump_symbol(_tag_arr(", ",sizeof(
unsigned char),3));}else{if(_tmpE4)Cyc_Dump_dump_alpha(_tag_arr("calloc",sizeof(
unsigned char),7));else{Cyc_Dump_dump_alpha(_tag_arr("malloc",sizeof(
unsigned char),7));}Cyc_Dump_dump_char((int)'(');}Cyc_Dump_dumpexp(_tmpE7);if(
_tmpE6 != 0){if(_tmpE4)Cyc_Dump_dump_symbol(_tag_arr(", ",sizeof(unsigned char),3));
else{Cyc_Dump_dump_symbol(_tag_arr(" * ",sizeof(unsigned char),4));}Cyc_Dump_dump_alpha(
_tag_arr("sizeof",sizeof(unsigned char),7));Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(*
_tmpE6);Cyc_Dump_dump_symbol(_tag_arr(")",sizeof(unsigned char),2));}Cyc_Dump_dump_symbol(
_tag_arr(")",sizeof(unsigned char),2));goto _LL9C;_LLF7: if(*((int*)_tmp8F)!= 34)
goto _LLF9;_tmpE8=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp8F)->f1;_tmpE9=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp8F)->f2;_LLF8:((void(*)(void(*f)(
struct _tuple14*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr
end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmpE9,_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LL9C;_LLF9: if(*((int*)_tmp8F)!= 35)goto _LLFB;
_tmpEA=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp8F)->f1;_LLFA: goto _LL9C;_LLFB: if(*((
int*)_tmp8F)!= 36)goto _LLFD;_tmpEB=((struct Cyc_Absyn_Codegen_e_struct*)_tmp8F)->f1;
_LLFC: goto _LL9C;_LLFD: if(*((int*)_tmp8F)!= 37)goto _LL9C;_tmpEC=((struct Cyc_Absyn_Fill_e_struct*)
_tmp8F)->f1;_LLFE: goto _LL9C;_LL9C:;}if(inprec >= myprec)Cyc_Dump_dump_char((int)')');}
void Cyc_Dump_dumpexp(struct Cyc_Absyn_Exp*e){Cyc_Dump_dumpexp_prec(0,e);}void Cyc_Dump_dumptunionfield(
struct Cyc_Absyn_Tunionfield*ef){Cyc_Dump_dumpqvar(ef->name);if(ef->typs != 0)Cyc_Dump_dumpargs(
ef->typs);}void Cyc_Dump_dumptunionfields(struct Cyc_List_List*fields){((void(*)(
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
Cyc_Absyn_Aggrfield _tmpF9;struct _tagged_arr*_tmpFA;struct Cyc_Absyn_Tqual _tmpFB;
void*_tmpFC;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_List_List*_tmpFE;struct Cyc_Absyn_Aggrfield*
_tmpF8=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmpF9=*_tmpF8;_tmpFA=_tmpF9.name;
_tmpFB=_tmpF9.tq;_tmpFC=(void*)_tmpF9.type;_tmpFD=_tmpF9.width;_tmpFE=_tmpF9.attributes;((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tagged_arr*),struct
_tagged_arr*))Cyc_Dump_dumptqtd)(_tmpFB,_tmpFC,Cyc_Dump_dump_alphastr,_tmpFA);
if(_tmpFD != 0){Cyc_Dump_dump_char((int)':');Cyc_Dump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmpFD));}Cyc_Dump_dump_symbol(_tag_arr(";\\\\\n",sizeof(
unsigned char),5));}}}void Cyc_Dump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*
td){Cyc_Dump_dumpqvar(td->name);Cyc_Dump_dumptvars(td->tvs);}void Cyc_Dump_dump_boldtypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Dump_dump_boldqvar(td->name);Cyc_Dump_dumptvars(
td->tvs);}static void Cyc_Dump_dumpids(struct Cyc_List_List*vds){for(0;vds != 0;vds=
vds->tl){Cyc_Dump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl
!= 0)Cyc_Dump_dump_char((int)',');}}static void Cyc_Dump_dump_upperbound(struct Cyc_Absyn_Exp*
e){unsigned int i=Cyc_Evexp_eval_const_uint_exp(e);if(i != 1){Cyc_Dump_dump_char((
int)'{');Cyc_Dump_dumpexp(e);Cyc_Dump_dump_char((int)'}');}}void Cyc_Dump_dumptms(
struct Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){f(a);return;}{void*
_tmpFF=(void*)tms->hd;void*_tmp100;struct Cyc_Absyn_Exp*_tmp101;void*_tmp102;
struct Cyc_Absyn_Tqual _tmp103;void*_tmp104;struct Cyc_Absyn_Exp*_tmp105;void*
_tmp106;struct Cyc_Absyn_Tqual _tmp107;void*_tmp108;void*_tmp109;struct Cyc_Absyn_Tqual
_tmp10A;void*_tmp10B;struct Cyc_Absyn_Exp*_tmp10C;void*_tmp10D;struct Cyc_Absyn_Tvar*
_tmp10E;struct Cyc_Absyn_Tqual _tmp10F;void*_tmp110;struct Cyc_Absyn_Exp*_tmp111;
void*_tmp112;struct Cyc_Absyn_Tvar*_tmp113;struct Cyc_Absyn_Tqual _tmp114;void*
_tmp115;void*_tmp116;struct Cyc_Absyn_Tvar*_tmp117;struct Cyc_Absyn_Tqual _tmp118;
void*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Tqual _tmp11B;void*
_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Tqual _tmp11E;void*_tmp11F;
struct Cyc_Absyn_Tqual _tmp120;_LL104: if(_tmpFF <= (void*)1?1:*((int*)_tmpFF)!= 1)
goto _LL106;_tmp100=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFF)->f1;if(
_tmp100 <= (void*)1?1:*((int*)_tmp100)!= 1)goto _LL106;_tmp101=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmp100)->f1;_tmp102=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFF)->f2;
if((int)_tmp102 != 2)goto _LL106;_tmp103=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFF)->f3;_LL105: Cyc_Dump_dump_pointer(_tag_arr("*",sizeof(unsigned char),2));
Cyc_Dump_dump_upperbound(_tmp101);Cyc_Dump_dump_alpha(_tag_arr("`H",sizeof(
unsigned char),3));Cyc_Dump_dumptms(tms->tl,f,a);return;_LL106: if(_tmpFF <= (void*)
1?1:*((int*)_tmpFF)!= 1)goto _LL108;_tmp104=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFF)->f1;if(_tmp104 <= (void*)1?1:*((int*)_tmp104)!= 0)goto _LL108;_tmp105=((
struct Cyc_Absyn_NonNullable_ps_struct*)_tmp104)->f1;_tmp106=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFF)->f2;if((int)_tmp106 != 2)goto _LL108;_tmp107=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFF)->f3;_LL107: Cyc_Dump_dump_pointer(_tag_arr("@",sizeof(unsigned char),2));
Cyc_Dump_dump_upperbound(_tmp105);Cyc_Dump_dump_alpha(_tag_arr("`H",sizeof(
unsigned char),3));Cyc_Dump_dumptms(tms->tl,f,a);return;_LL108: if(_tmpFF <= (void*)
1?1:*((int*)_tmpFF)!= 1)goto _LL10A;_tmp108=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFF)->f1;if((int)_tmp108 != 0)goto _LL10A;_tmp109=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFF)->f2;if((int)_tmp109 != 2)goto _LL10A;_tmp10A=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFF)->f3;_LL109: Cyc_Dump_dump_pointer(_tag_arr("{?}",sizeof(unsigned char),4));
Cyc_Dump_dump_alpha(_tag_arr("`H",sizeof(unsigned char),3));Cyc_Dump_dumptms(tms->tl,
f,a);return;_LL10A: if(_tmpFF <= (void*)1?1:*((int*)_tmpFF)!= 1)goto _LL10C;_tmp10B=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFF)->f1;if(_tmp10B <= (void*)1?1:*((
int*)_tmp10B)!= 1)goto _LL10C;_tmp10C=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmp10B)->f1;_tmp10D=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFF)->f2;
if(_tmp10D <= (void*)3?1:*((int*)_tmp10D)!= 1)goto _LL10C;_tmp10E=((struct Cyc_Absyn_VarType_struct*)
_tmp10D)->f1;_tmp10F=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFF)->f3;_LL10B:
Cyc_Dump_dump_pointer(_tag_arr("*",sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(
_tmp10C);Cyc_Dump_dump_alphastr(_tmp10E->name);Cyc_Dump_dumptms(tms->tl,f,a);
return;_LL10C: if(_tmpFF <= (void*)1?1:*((int*)_tmpFF)!= 1)goto _LL10E;_tmp110=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFF)->f1;if(_tmp110 <= (void*)1?1:*((
int*)_tmp110)!= 0)goto _LL10E;_tmp111=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmp110)->f1;_tmp112=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFF)->f2;
if(_tmp112 <= (void*)3?1:*((int*)_tmp112)!= 1)goto _LL10E;_tmp113=((struct Cyc_Absyn_VarType_struct*)
_tmp112)->f1;_tmp114=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFF)->f3;_LL10D:
Cyc_Dump_dump_pointer(_tag_arr("@",sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(
_tmp111);Cyc_Dump_dump_alphastr(_tmp113->name);Cyc_Dump_dumptms(tms->tl,f,a);
return;_LL10E: if(_tmpFF <= (void*)1?1:*((int*)_tmpFF)!= 1)goto _LL110;_tmp115=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFF)->f1;if((int)_tmp115 != 0)goto
_LL110;_tmp116=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFF)->f2;if(
_tmp116 <= (void*)3?1:*((int*)_tmp116)!= 1)goto _LL110;_tmp117=((struct Cyc_Absyn_VarType_struct*)
_tmp116)->f1;_tmp118=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFF)->f3;_LL10F:
Cyc_Dump_dump_pointer(_tag_arr("{?}",sizeof(unsigned char),4));Cyc_Dump_dump_alphastr(
_tmp117->name);Cyc_Dump_dumptms(tms->tl,f,a);return;_LL110: if(_tmpFF <= (void*)1?
1:*((int*)_tmpFF)!= 1)goto _LL112;_tmp119=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFF)->f1;if(_tmp119 <= (void*)1?1:*((int*)_tmp119)!= 1)goto _LL112;_tmp11A=((
struct Cyc_Absyn_Nullable_ps_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFF)->f3;_LL111: Cyc_Dump_dump_pointer(_tag_arr("*",sizeof(unsigned char),2));
Cyc_Dump_dump_upperbound(_tmp11A);Cyc_Dump_dumptms(tms->tl,f,a);return;_LL112:
if(_tmpFF <= (void*)1?1:*((int*)_tmpFF)!= 1)goto _LL114;_tmp11C=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFF)->f1;if(_tmp11C <= (void*)1?1:*((int*)_tmp11C)!= 0)goto _LL114;_tmp11D=((
struct Cyc_Absyn_NonNullable_ps_struct*)_tmp11C)->f1;_tmp11E=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFF)->f3;_LL113: Cyc_Dump_dump_pointer(_tag_arr("@",sizeof(unsigned char),2));
Cyc_Dump_dump_upperbound(_tmp11D);Cyc_Dump_dumptms(tms->tl,f,a);return;_LL114:
if(_tmpFF <= (void*)1?1:*((int*)_tmpFF)!= 1)goto _LL116;_tmp11F=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFF)->f1;if((int)_tmp11F != 0)goto _LL116;_tmp120=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFF)->f3;_LL115: Cyc_Dump_dump_pointer(_tag_arr("{?}",sizeof(unsigned char),4));
Cyc_Dump_dumptms(tms->tl,f,a);return;_LL116:;_LL117: {int next_is_pointer=0;if(
tms->tl != 0){void*_tmp121=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;
_LL119: if(_tmp121 <= (void*)1?1:*((int*)_tmp121)!= 1)goto _LL11B;_LL11A:
next_is_pointer=1;goto _LL118;_LL11B:;_LL11C: goto _LL118;_LL118:;}if(
next_is_pointer)Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptms(tms->tl,f,a);if(
next_is_pointer)Cyc_Dump_dump_char((int)')');{void*_tmp122=(void*)tms->hd;struct
Cyc_Absyn_Exp*_tmp123;void*_tmp124;struct Cyc_List_List*_tmp125;int _tmp126;struct
Cyc_Absyn_VarargInfo*_tmp127;struct Cyc_Core_Opt*_tmp128;struct Cyc_List_List*
_tmp129;void*_tmp12A;struct Cyc_List_List*_tmp12B;struct Cyc_Position_Segment*
_tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_Position_Segment*_tmp12E;int
_tmp12F;void*_tmp130;void*_tmp131;_LL11E: if((int)_tmp122 != 0)goto _LL120;_LL11F:
Cyc_Dump_dump_symbol(_tag_arr("[]",sizeof(unsigned char),3));goto _LL11D;_LL120:
if(_tmp122 <= (void*)1?1:*((int*)_tmp122)!= 0)goto _LL122;_tmp123=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp122)->f1;_LL121: Cyc_Dump_dump_char((int)'[');Cyc_Dump_dumpexp(_tmp123);Cyc_Dump_dump_char((
int)']');goto _LL11D;_LL122: if(_tmp122 <= (void*)1?1:*((int*)_tmp122)!= 2)goto
_LL124;_tmp124=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp122)->f1;if(*((
int*)_tmp124)!= 1)goto _LL124;_tmp125=((struct Cyc_Absyn_WithTypes_struct*)_tmp124)->f1;
_tmp126=((struct Cyc_Absyn_WithTypes_struct*)_tmp124)->f2;_tmp127=((struct Cyc_Absyn_WithTypes_struct*)
_tmp124)->f3;_tmp128=((struct Cyc_Absyn_WithTypes_struct*)_tmp124)->f4;_tmp129=((
struct Cyc_Absyn_WithTypes_struct*)_tmp124)->f5;_LL123: Cyc_Dump_dumpfunargs(
_tmp125,_tmp126,_tmp127,_tmp128,_tmp129);goto _LL11D;_LL124: if(_tmp122 <= (void*)1?
1:*((int*)_tmp122)!= 2)goto _LL126;_tmp12A=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp122)->f1;if(*((int*)_tmp12A)!= 0)goto _LL126;_tmp12B=((struct Cyc_Absyn_NoTypes_struct*)
_tmp12A)->f1;_tmp12C=((struct Cyc_Absyn_NoTypes_struct*)_tmp12A)->f2;_LL125:((
void(*)(void(*f)(struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr
start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dump_alphastr,
_tmp12B,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),
2),_tag_arr(", ",sizeof(unsigned char),3));goto _LL11D;_LL126: if(_tmp122 <= (void*)
1?1:*((int*)_tmp122)!= 3)goto _LL128;_tmp12D=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp122)->f1;_tmp12E=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp122)->f2;
_tmp12F=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp122)->f3;_LL127: if(_tmp12F)
Cyc_Dump_dumpkindedtvars(_tmp12D);else{Cyc_Dump_dumptvars(_tmp12D);}goto _LL11D;
_LL128: if(_tmp122 <= (void*)1?1:*((int*)_tmp122)!= 4)goto _LL12A;_LL129: goto _LL11D;
_LL12A: if(_tmp122 <= (void*)1?1:*((int*)_tmp122)!= 1)goto _LL11D;_tmp130=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp122)->f1;_tmp131=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp122)->f2;_LL12B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132[0]=({struct Cyc_Core_Impossible_struct
_tmp133;_tmp133.tag=Cyc_Core_Impossible;_tmp133.f1=_tag_arr("dumptms",sizeof(
unsigned char),8);_tmp133;});_tmp132;}));_LL11D:;}return;}_LL103:;}}void Cyc_Dump_dumptqtd(
struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){struct Cyc_Absyn_Tqual
_tmp135;void*_tmp136;struct Cyc_List_List*_tmp137;struct _tuple10 _tmp134=Cyc_Absynpp_to_tms(
tq,t);_tmp135=_tmp134.f1;_tmp136=_tmp134.f2;_tmp137=_tmp134.f3;Cyc_Dump_dumptq(
_tmp135);Cyc_Dump_dumpntyp(_tmp136);Cyc_Dump_dumptms(Cyc_List_imp_rev(_tmp137),f,
a);}void Cyc_Dump_pr_comment(struct Cyc_Std___cycFILE*outf,struct _tagged_arr s){int
depth=0;int len=(int)Cyc_Std_strlen(s);int i=0;for(0;i < len;i ++){unsigned char c=*((
const unsigned char*)_check_unknown_subscript(s,sizeof(unsigned char),i));if(c != '['){
Cyc_Std_fputc((int)c,outf);continue;}Cyc_Std_fputs(_tag_arr("\\texttt{",sizeof(
unsigned char),9),outf);i ++;depth ++;for(0;i < len;i ++){unsigned char c=*((const
unsigned char*)_check_unknown_subscript(s,sizeof(unsigned char),i));if(c == ']'){
depth --;if(depth == 0){Cyc_Std_fputc((int)'}',outf);break;}}else{if(c == '[')depth
++;}Cyc_Std_fputc((int)c,outf);}}}void Cyc_Dump_dump_comment(struct _tagged_arr s){
Cyc_Dump_pr_comment(Cyc_Std_stdout,s);}void Cyc_Dump_dumpdecl(struct Cyc_Absyn_Decl*
d,struct _tagged_arr comment){void*_tmp138=(void*)d->r;struct Cyc_Absyn_Aggrdecl*
_tmp139;struct Cyc_Absyn_Vardecl*_tmp13A;struct Cyc_Absyn_Vardecl _tmp13B;struct
_tuple1*_tmp13C;struct Cyc_Absyn_Tqual _tmp13D;void*_tmp13E;struct Cyc_Absyn_Tuniondecl*
_tmp13F;struct Cyc_Absyn_Tuniondecl _tmp140;void*_tmp141;struct _tuple1*_tmp142;
struct Cyc_List_List*_tmp143;struct Cyc_Core_Opt*_tmp144;int _tmp145;struct Cyc_Absyn_Enumdecl*
_tmp146;struct Cyc_Absyn_Enumdecl _tmp147;void*_tmp148;struct _tuple1*_tmp149;
struct Cyc_Core_Opt*_tmp14A;struct Cyc_Absyn_Typedefdecl*_tmp14B;_LL12D: if(*((int*)
_tmp138)!= 4)goto _LL12F;_tmp139=((struct Cyc_Absyn_Aggr_d_struct*)_tmp138)->f1;
_LL12E: Cyc_Dump_dump_begin();Cyc_Dump_dump_alpha((void*)_tmp139->kind == (void*)1?
_tag_arr("union",sizeof(unsigned char),6): _tag_arr("struct",sizeof(unsigned char),
7));Cyc_Dump_dump_boldqvar(_tmp139->name);Cyc_Dump_dumptvars(_tmp139->tvs);if(
_tmp139->impl == 0)Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(unsigned char),3));
else{Cyc_Dump_dump_symbol(_tag_arr(" ",sizeof(unsigned char),2));Cyc_Dump_dump_lb();
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp139->impl))->exist_vars != 0){((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _tagged_arr
start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_egroup)(Cyc_Dump_dumptvar,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp139->impl))->exist_vars,_tag_arr("<",
sizeof(unsigned char),2),_tag_arr(">",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));Cyc_Dump_dump_alpha(_tag_arr("",sizeof(unsigned char),
1));}if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp139->impl))->rgn_po != 0){
Cyc_Dump_dump_char((int)':');Cyc_Dump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp139->impl))->rgn_po);}Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",
sizeof(unsigned char),4));Cyc_Dump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp139->impl))->fields);Cyc_Dump_dump_rb();Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));}Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(
comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(unsigned char),2));Cyc_Dump_dump_end();
goto _LL12C;_LL12F: if(*((int*)_tmp138)!= 0)goto _LL131;_tmp13A=((struct Cyc_Absyn_Var_d_struct*)
_tmp138)->f1;_tmp13B=*_tmp13A;_tmp13C=_tmp13B.name;_tmp13D=_tmp13B.tq;_tmp13E=(
void*)_tmp13B.type;_LL130: Cyc_Dump_dump_begin();((void(*)(struct Cyc_Absyn_Tqual
tq,void*t,void(*f)(struct _tuple1*),struct _tuple1*a))Cyc_Dump_dumptqtd)(_tmp13D,
_tmp13E,Cyc_Dump_dump_boldqvar,_tmp13C);Cyc_Dump_dump_symbol(_tag_arr(";\n",
sizeof(unsigned char),3));Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(comment);
Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(unsigned char),2));Cyc_Dump_dump_end();
goto _LL12C;_LL131: if(*((int*)_tmp138)!= 5)goto _LL133;_tmp13F=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp138)->f1;_tmp140=*_tmp13F;_tmp141=(void*)_tmp140.sc;_tmp142=_tmp140.name;
_tmp143=_tmp140.tvs;_tmp144=_tmp140.fields;_tmp145=_tmp140.is_xtunion;_LL132: Cyc_Dump_dump_begin();
if(_tmp145)Cyc_Dump_dump_alpha(_tag_arr("xtunion",sizeof(unsigned char),8));
else{Cyc_Dump_dump_alpha(_tag_arr("tunion",sizeof(unsigned char),7));}Cyc_Dump_dump_boldqvar(
_tmp142);Cyc_Dump_dumptvars(_tmp143);if(_tmp144 == 0)Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));else{Cyc_Dump_dump_symbol(_tag_arr(" ",
sizeof(unsigned char),2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(_tag_arr("\\\\\n~~",
sizeof(unsigned char),6));Cyc_Dump_dumptunionfields((struct Cyc_List_List*)
_tmp144->v);Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",sizeof(unsigned char),4));Cyc_Dump_dump_rb();
Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(unsigned char),3));}Cyc_Dump_dump_middle();
Cyc_Dump_dump_comment(comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(
unsigned char),2));Cyc_Dump_dump_end();goto _LL12C;_LL133: if(*((int*)_tmp138)!= 6)
goto _LL135;_tmp146=((struct Cyc_Absyn_Enum_d_struct*)_tmp138)->f1;_tmp147=*
_tmp146;_tmp148=(void*)_tmp147.sc;_tmp149=_tmp147.name;_tmp14A=_tmp147.fields;
_LL134: Cyc_Dump_dump_begin();Cyc_Dump_dump_alpha(_tag_arr("enum",sizeof(
unsigned char),5));Cyc_Dump_dump_boldqvar(_tmp149);if(_tmp14A == 0)Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));else{Cyc_Dump_dump_symbol(_tag_arr(" ",
sizeof(unsigned char),2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(_tag_arr("\\\\\n~~",
sizeof(unsigned char),6));Cyc_Dump_dumpenumfields((struct Cyc_List_List*)_tmp14A->v);
Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",sizeof(unsigned char),4));Cyc_Dump_dump_rb();
Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(unsigned char),3));}Cyc_Dump_dump_middle();
Cyc_Dump_dump_comment(comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(
unsigned char),2));Cyc_Dump_dump_end();goto _LL12C;_LL135: if(*((int*)_tmp138)!= 7)
goto _LL137;_tmp14B=((struct Cyc_Absyn_Typedef_d_struct*)_tmp138)->f1;_LL136: Cyc_Dump_dump_begin();
Cyc_Dump_dump_alpha(_tag_arr("typedef",sizeof(unsigned char),8));{void*t;if(
_tmp14B->defn != 0)t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp14B->defn))->v;
else{t=Cyc_Absyn_new_evar(_tmp14B->kind,0);}((void(*)(struct Cyc_Absyn_Tqual tq,
void*t,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*a))Cyc_Dump_dumptqtd)(({
struct Cyc_Absyn_Tqual _tmp14C;_tmp14C.q_const=0;_tmp14C.q_volatile=0;_tmp14C.q_restrict=
0;_tmp14C;}),t,Cyc_Dump_dump_boldtypedefname,_tmp14B);Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(
comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(unsigned char),2));Cyc_Dump_dump_end();
goto _LL12C;}_LL137: if(*((int*)_tmp138)!= 1)goto _LL139;_LL138: goto _LL13A;_LL139:
if(*((int*)_tmp138)!= 2)goto _LL13B;_LL13A: goto _LL13C;_LL13B: if(*((int*)_tmp138)
!= 3)goto _LL13D;_LL13C: goto _LL13E;_LL13D: if(*((int*)_tmp138)!= 8)goto _LL13F;
_LL13E: goto _LL140;_LL13F: if(*((int*)_tmp138)!= 9)goto _LL141;_LL140: goto _LL142;
_LL141: if(*((int*)_tmp138)!= 10)goto _LL12C;_LL142: goto _LL12C;_LL12C:;}static int
Cyc_is_other_special(unsigned char c){switch(c){case '\\': _LL143: goto _LL144;case '"':
_LL144: goto _LL145;case ';': _LL145: goto _LL146;case '&': _LL146: goto _LL147;case '(':
_LL147: goto _LL148;case ')': _LL148: goto _LL149;case '|': _LL149: goto _LL14A;case '^':
_LL14A: goto _LL14B;case '<': _LL14B: goto _LL14C;case '>': _LL14C: goto _LL14D;case ' ':
_LL14D: goto _LL14E;case '\n': _LL14E: goto _LL14F;case '\t': _LL14F: return 1;default:
_LL150: return 0;}}static struct _tagged_arr Cyc_sh_escape_string(struct _tagged_arr s){
unsigned int _tmp14D=Cyc_Std_strlen(s);int _tmp14E=0;int _tmp14F=0;{int i=0;for(0;i < 
_tmp14D;i ++){unsigned char _tmp150=*((const unsigned char*)
_check_unknown_subscript(s,sizeof(unsigned char),i));if(_tmp150 == '\'')_tmp14E ++;
else{if(Cyc_is_other_special(_tmp150))_tmp14F ++;}}}if(_tmp14E == 0?_tmp14F == 0: 0)
return s;if(_tmp14E == 0)return(struct _tagged_arr)Cyc_Std_strconcat_l(({struct
_tagged_arr*_tmp151[3];_tmp151[2]=_init_tag_arr(_cycalloc(sizeof(struct
_tagged_arr)),"'",sizeof(unsigned char),2);_tmp151[1]=({struct _tagged_arr*
_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153[0]=(struct _tagged_arr)s;_tmp153;});
_tmp151[0]=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",sizeof(
unsigned char),2);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp151,sizeof(struct _tagged_arr*),3));}));{unsigned int _tmp155=(
_tmp14D + _tmp14E)+ _tmp14F;struct _tagged_arr _tmp156=({unsigned int _tmp15A=
_tmp155 + 1;unsigned char*_tmp15B=(unsigned char*)_cycalloc_atomic(_check_times(
sizeof(unsigned char),_tmp15A));struct _tagged_arr _tmp15D=_tag_arr(_tmp15B,
sizeof(unsigned char),_tmp155 + 1);{unsigned int _tmp15C=_tmp15A;unsigned int i;
for(i=0;i < _tmp15C;i ++){_tmp15B[i]='\000';}}_tmp15D;});int _tmp157=0;int _tmp158=0;
for(0;_tmp157 < _tmp14D;_tmp157 ++){unsigned char _tmp159=*((const unsigned char*)
_check_unknown_subscript(s,sizeof(unsigned char),_tmp157));if(_tmp159 == '\''?1:
Cyc_is_other_special(_tmp159))*((unsigned char*)_check_unknown_subscript(_tmp156,
sizeof(unsigned char),_tmp158 ++))='\\';*((unsigned char*)
_check_unknown_subscript(_tmp156,sizeof(unsigned char),_tmp158 ++))=_tmp159;}
return(struct _tagged_arr)_tmp156;}}static struct _tagged_arr*Cyc_sh_escape_stringptr(
struct _tagged_arr*sp){return({struct _tagged_arr*_tmp15E=_cycalloc(sizeof(*
_tmp15E));_tmp15E[0]=Cyc_sh_escape_string(*sp);_tmp15E;});}static struct Cyc_Lineno_Pos*
Cyc_new_pos(){return({struct Cyc_Lineno_Pos*_tmp15F=_cycalloc(sizeof(*_tmp15F));
_tmp15F->logical_file=_tag_arr("",sizeof(unsigned char),1);_tmp15F->line=Cyc_Core_new_string(
0);_tmp15F->line_no=0;_tmp15F->col=0;_tmp15F;});}struct _tuple15{int f1;struct Cyc_Lineno_Pos*
f2;};static struct _tuple15*Cyc_start2pos(int x){return({struct _tuple15*_tmp160=
_cycalloc(sizeof(*_tmp160));_tmp160->f1=x;_tmp160->f2=Cyc_new_pos();_tmp160;});}
static int Cyc_decl2start(struct Cyc_Absyn_Decl*d){return((struct Cyc_Position_Segment*)
_check_null(d->loc))->start;}struct _tuple16{struct Cyc_Lineno_Pos*f1;void*f2;};
static struct Cyc_List_List*Cyc_this_file(struct _tagged_arr file,struct Cyc_List_List*
x){struct Cyc_List_List*_tmp161=0;for(0;x != 0;x=x->tl){if(Cyc_Std_strcmp(((*((
struct _tuple16*)x->hd)).f1)->logical_file,file)== 0)_tmp161=({struct Cyc_List_List*
_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162->hd=(struct _tuple16*)x->hd;_tmp162->tl=
_tmp161;_tmp162;});}_tmp161=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp161);return _tmp161;}static int Cyc_lineno(struct Cyc_Lineno_Pos*p){return p->line_no;}
static struct Cyc_List_List*Cyc_flatten_decls(struct Cyc_List_List*decls){struct Cyc_List_List*
_tmp163=0;while(decls != 0){void*_tmp164=(void*)((struct Cyc_Absyn_Decl*)decls->hd)->r;
struct Cyc_List_List*_tmp165;struct Cyc_List_List*_tmp166;struct Cyc_List_List*
_tmp167;_LL153: if(*((int*)_tmp164)!= 8)goto _LL155;_tmp165=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp164)->f2;_LL154: _tmp166=_tmp165;goto _LL156;_LL155: if(*((int*)_tmp164)!= 9)
goto _LL157;_tmp166=((struct Cyc_Absyn_Using_d_struct*)_tmp164)->f2;_LL156: _tmp167=
_tmp166;goto _LL158;_LL157: if(*((int*)_tmp164)!= 10)goto _LL159;_tmp167=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp164)->f1;_LL158: decls=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp167,decls->tl);
goto _LL152;_LL159: if(*((int*)_tmp164)!= 4)goto _LL15B;_LL15A: goto _LL15C;_LL15B:
if(*((int*)_tmp164)!= 0)goto _LL15D;_LL15C: goto _LL15E;_LL15D: if(*((int*)_tmp164)
!= 5)goto _LL15F;_LL15E: goto _LL160;_LL15F: if(*((int*)_tmp164)!= 6)goto _LL161;
_LL160: goto _LL162;_LL161: if(*((int*)_tmp164)!= 7)goto _LL163;_LL162: goto _LL164;
_LL163: if(*((int*)_tmp164)!= 1)goto _LL165;_LL164: goto _LL166;_LL165: if(*((int*)
_tmp164)!= 2)goto _LL167;_LL166: goto _LL168;_LL167: if(*((int*)_tmp164)!= 3)goto
_LL152;_LL168: _tmp163=({struct Cyc_List_List*_tmp168=_cycalloc(sizeof(*_tmp168));
_tmp168->hd=(struct Cyc_Absyn_Decl*)decls->hd;_tmp168->tl=_tmp163;_tmp168;});
decls=decls->tl;goto _LL152;_LL152:;}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp163);}static struct Cyc_List_List*Cyc_cycdoc_files=0;
static void Cyc_add_other(struct _tagged_arr s){Cyc_cycdoc_files=({struct Cyc_List_List*
_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->hd=({struct _tagged_arr*_tmp16A=
_cycalloc(sizeof(*_tmp16A));_tmp16A[0]=s;_tmp16A;});_tmp169->tl=Cyc_cycdoc_files;
_tmp169;});}static struct Cyc_List_List*Cyc_cycargs=0;static void Cyc_add_cycarg(
struct _tagged_arr s){Cyc_cycargs=({struct Cyc_List_List*_tmp16B=_cycalloc(sizeof(*
_tmp16B));_tmp16B->hd=({struct _tagged_arr*_tmp16C=_cycalloc(sizeof(*_tmp16C));
_tmp16C[0]=s;_tmp16C;});_tmp16B->tl=Cyc_cycargs;_tmp16B;});}static unsigned char
_tmp16D[8]="cyclone";static struct _tagged_arr Cyc_cyclone_file={_tmp16D,_tmp16D,
_tmp16D + 8};static void Cyc_set_cyclone_file(struct _tagged_arr s){Cyc_cyclone_file=
s;}struct _tuple17{int f1;struct Cyc_Absyn_Decl*f2;};struct _tuple18{struct Cyc_Lineno_Pos*
f1;struct Cyc_Absyn_Decl*f2;};static void Cyc_process_file(struct _tagged_arr
filename){struct _tagged_arr _tmp16E=Cyc_Filename_chop_extension(filename);struct
_tagged_arr _tmp16F=Cyc_Std_strconcat((struct _tagged_arr)_tmp16E,_tag_arr(".cyp",
sizeof(unsigned char),5));struct _tagged_arr _tmp170=Cyc_Std_str_sepstr(({struct
Cyc_List_List*_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191->hd=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(unsigned char),1);_tmp191->tl=((
struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_cycargs));_tmp191;}),_tag_arr(" ",sizeof(unsigned char),2));
struct _tagged_arr _tmp171=({struct Cyc_Std_String_pa_struct _tmp190;_tmp190.tag=0;
_tmp190.f1=(struct _tagged_arr)Cyc_sh_escape_string(filename);{struct Cyc_Std_String_pa_struct
_tmp18F;_tmp18F.tag=0;_tmp18F.f1=(struct _tagged_arr)Cyc_sh_escape_string((struct
_tagged_arr)_tmp16F);{struct Cyc_Std_String_pa_struct _tmp18E;_tmp18E.tag=0;
_tmp18E.f1=(struct _tagged_arr)_tmp170;{struct Cyc_Std_String_pa_struct _tmp18D;
_tmp18D.tag=0;_tmp18D.f1=(struct _tagged_arr)Cyc_cyclone_file;{void*_tmp18C[4]={&
_tmp18D,& _tmp18E,& _tmp18F,& _tmp190};Cyc_Std_aprintf(_tag_arr("%s %s -E -o %s -x cyc %s",
sizeof(unsigned char),25),_tag_arr(_tmp18C,sizeof(void*),4));}}}}});if(Cyc_Std_system((
struct _tagged_arr)_tmp171)!= 0){({void*_tmp172[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\nError: preprocessing\n",sizeof(unsigned char),23),_tag_arr(_tmp172,
sizeof(void*),0));});return;}Cyc_Position_reset_position((struct _tagged_arr)
_tmp16F);{struct Cyc_Std___cycFILE*in_file=(struct Cyc_Std___cycFILE*)_check_null(
Cyc_Std_fopen((struct _tagged_arr)_tmp16F,_tag_arr("r",sizeof(unsigned char),2)));
Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp173=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init();Cyc_Std_file_close((struct Cyc_Std___cycFILE*)in_file);_tmp173=
Cyc_flatten_decls(_tmp173);{struct Cyc_List_List*_tmp174=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(int),struct Cyc_List_List*x))Cyc_List_map)(Cyc_start2pos,((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_decl2start,_tmp173));Cyc_Lineno_poss_of_abss((struct
_tagged_arr)_tmp16F,_tmp174);Cyc_Std_remove((struct _tagged_arr)_tmp16F);{struct
Cyc_List_List*_tmp175=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*f)(struct
_tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(struct
_tuple15*))Cyc_Core_snd,_tmp174),_tmp173);_tmp175=((struct Cyc_List_List*(*)(
struct _tagged_arr file,struct Cyc_List_List*x))Cyc_this_file)(filename,_tmp175);{
struct Cyc_List_List*_tmp176=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Lineno_Pos*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(struct
Cyc_Lineno_Pos*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)(struct _tuple18*))Cyc_Core_fst,_tmp175)),((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Decl*(*)(struct _tuple18*))Cyc_Core_snd,_tmp175));struct Cyc_Std___cycFILE*
_tmp177=(struct Cyc_Std___cycFILE*)_check_null(Cyc_Std_fopen(filename,_tag_arr("r",
sizeof(unsigned char),2)));struct Cyc_Lexing_lexbuf*_tmp178=Cyc_Lexing_from_file(
_tmp177);struct Cyc_List_List*_tmp179=0;struct _tuple11*comment;while((comment=((
struct _tuple11*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_token)(_tmp178))!= 0){
_tmp179=({struct Cyc_List_List*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->hd=(
struct _tuple11*)_check_null(comment);_tmp17A->tl=_tmp179;_tmp17A;});}Cyc_Std_fclose(
_tmp177);_tmp179=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp179);{struct Cyc_List_List*_tmp17B=((struct Cyc_List_List*(*)(struct _tuple15*(*
f)(int),struct Cyc_List_List*x))Cyc_List_map)(Cyc_start2pos,((struct Cyc_List_List*(*)(
int(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((int(*)(struct
_tuple11*))Cyc_Core_fst,_tmp179));Cyc_Lineno_poss_of_abss(filename,_tmp17B);{
struct Cyc_List_List*_tmp17C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*
f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple15*))Cyc_Core_snd,_tmp17B),((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))
Cyc_Core_snd,_tmp179));_tmp17C=Cyc_this_file(filename,_tmp17C);{struct Cyc_List_List*
_tmp17D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*f)(
struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple16*))Cyc_Core_fst,_tmp17C)),((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple16*))
Cyc_Core_snd,_tmp17C));while(_tmp176 != 0?_tmp17D != 0: 0){struct _tuple17 _tmp17F;
int _tmp180;struct Cyc_Absyn_Decl*_tmp181;struct _tuple17*_tmp17E=(struct _tuple17*)
_tmp176->hd;_tmp17F=*_tmp17E;_tmp180=_tmp17F.f1;_tmp181=_tmp17F.f2;{struct
_tuple11 _tmp183;int _tmp184;void*_tmp185;struct _tuple11*_tmp182=(struct _tuple11*)
_tmp17D->hd;_tmp183=*_tmp182;_tmp184=_tmp183.f1;_tmp185=_tmp183.f2;{void*_tmp186=
_tmp185;struct _tagged_arr _tmp187;struct _tagged_arr _tmp188;_LL16A: if(*((int*)
_tmp186)!= 1)goto _LL16C;_tmp187=((struct Cyc_Standalone_struct*)_tmp186)->f1;
_LL16B: Cyc_Dump_dump_comment((struct _tagged_arr)_tmp187);Cyc_Dump_dump_symbol(
_tag_arr("\n",sizeof(unsigned char),2));_tmp17D=_tmp17D->tl;goto _LL169;_LL16C:
if(*((int*)_tmp186)!= 0)goto _LL169;_tmp188=((struct Cyc_MatchDecl_struct*)_tmp186)->f1;
_LL16D: if(_tmp184 < _tmp180){_tmp17D=_tmp17D->tl;continue;}if(_tmp176->tl != 0){
struct _tuple17 _tmp18A;int _tmp18B;struct _tuple17*_tmp189=(struct _tuple17*)((
struct Cyc_List_List*)_check_null(_tmp176->tl))->hd;_tmp18A=*_tmp189;_tmp18B=
_tmp18A.f1;if(_tmp18B < _tmp184){_tmp176=_tmp176->tl;continue;}}Cyc_Dump_dumpdecl(
_tmp181,(struct _tagged_arr)_tmp188);Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stdout);
_tmp176=_tmp176->tl;_tmp17D=_tmp17D->tl;goto _LL169;_LL169:;}}}}}}}}}}}}struct
_tuple19{struct _tagged_arr f1;int f2;struct _tagged_arr f3;void*f4;struct _tagged_arr
f5;};int Cyc_main(int argc,struct _tagged_arr argv){struct Cyc_List_List*options=({
struct _tuple19*_tmp194[4];_tmp194[3]=({struct _tuple19*_tmp19E=_cycalloc(sizeof(*
_tmp19E));_tmp19E->f1=_tag_arr("-B",sizeof(unsigned char),3);_tmp19E->f2=1;
_tmp19E->f3=_tag_arr("<file>",sizeof(unsigned char),7);_tmp19E->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp1A0;_tmp1A0.tag=1;_tmp1A0.f1=Cyc_add_cycarg;
_tmp1A0;});_tmp19F;});_tmp19E->f5=_tag_arr("Add to the list of directories to search for compiler files",
sizeof(unsigned char),60);_tmp19E;});_tmp194[2]=({struct _tuple19*_tmp19B=
_cycalloc(sizeof(*_tmp19B));_tmp19B->f1=_tag_arr("-I",sizeof(unsigned char),3);
_tmp19B->f2=1;_tmp19B->f3=_tag_arr("<dir>",sizeof(unsigned char),6);_tmp19B->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp19C=_cycalloc(sizeof(*_tmp19C));
_tmp19C[0]=({struct Cyc_Arg_Flag_spec_struct _tmp19D;_tmp19D.tag=1;_tmp19D.f1=Cyc_add_cycarg;
_tmp19D;});_tmp19C;});_tmp19B->f5=_tag_arr("Add to the list of directories to search for include files",
sizeof(unsigned char),59);_tmp19B;});_tmp194[1]=({struct _tuple19*_tmp198=
_cycalloc(sizeof(*_tmp198));_tmp198->f1=_tag_arr("-D",sizeof(unsigned char),3);
_tmp198->f2=1;_tmp198->f3=_tag_arr("<name>[=<value>]",sizeof(unsigned char),17);
_tmp198->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp199=_cycalloc(sizeof(*
_tmp199));_tmp199[0]=({struct Cyc_Arg_Flag_spec_struct _tmp19A;_tmp19A.tag=1;
_tmp19A.f1=Cyc_add_cycarg;_tmp19A;});_tmp199;});_tmp198->f5=_tag_arr("Pass definition to preprocessor",
sizeof(unsigned char),32);_tmp198;});_tmp194[0]=({struct _tuple19*_tmp195=
_cycalloc(sizeof(*_tmp195));_tmp195->f1=_tag_arr("-cyclone",sizeof(unsigned char),
9);_tmp195->f2=0;_tmp195->f3=_tag_arr(" <file>",sizeof(unsigned char),8);_tmp195->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp196=_cycalloc(sizeof(*_tmp196));
_tmp196[0]=({struct Cyc_Arg_String_spec_struct _tmp197;_tmp197.tag=5;_tmp197.f1=
Cyc_set_cyclone_file;_tmp197;});_tmp196;});_tmp195->f5=_tag_arr("Use <file> as the cyclone compiler",
sizeof(unsigned char),35);_tmp195;});((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp194,sizeof(struct _tuple19*),4));});Cyc_Arg_parse(
options,Cyc_add_other,_tag_arr("Options:",sizeof(unsigned char),9),argv);{struct
Cyc_List_List*_tmp193=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cycdoc_files);for(0;_tmp193 != 0;_tmp193=_tmp193->tl){Cyc_process_file(*((
struct _tagged_arr*)_tmp193->hd));}}return 0;}
