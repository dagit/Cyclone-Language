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
static inline 
void * _check_null(void * ptr) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
  return ptr;
}
static inline 
char * _check_known_subscript_null(void * ptr, unsigned bound, 
				   unsigned elt_sz, unsigned index) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return ((char *)ptr) + elt_sz*index;
}
static inline 
unsigned _check_known_subscript_notnull(unsigned bound, unsigned index) {
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return index;
}
static inline 
char * _check_unknown_subscript(struct _tagged_arr arr,
				unsigned elt_sz, unsigned index) {
  // caller casts first argument and result
  // multiplication looks inefficient, but C compiler has to insert it otherwise
  // by inlining, it should be able to avoid actual multiplication
  unsigned char * ans = arr.curr + elt_sz * index;
  // might be faster not to distinguish these two cases. definitely would be
  // smaller.
#ifndef NO_CYC_NULL_CHECKS
  if(!arr.base) 
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(ans < arr.base || ans >= arr.last_plus_one)
    _throw_arraybounds();
#endif
  return ans;
}
static inline 
struct _tagged_arr _tag_arr(const void * curr, 
			    unsigned elt_sz, unsigned num_elts) {
  // beware the gcc bug, can happen with *temp = _tag_arr(...) in weird places!
  struct _tagged_arr ans;
  ans.base = (void *)curr;
  ans.curr = (void *)curr;
  ans.last_plus_one = ((char *)curr) + elt_sz * num_elts;
  return ans;
}
static inline
struct _tagged_arr * _init_tag_arr(struct _tagged_arr * arr_ptr, void * arr, 
				   unsigned elt_sz, unsigned num_elts) {
  // we use this to (hopefully) avoid the gcc bug
  arr_ptr->base = arr_ptr->curr = arr;
  arr_ptr->last_plus_one = ((char *)arr) + elt_sz * num_elts;
  return arr_ptr;
}

static inline
char * _untag_arr(struct _tagged_arr arr, unsigned elt_sz, unsigned num_elts) {
  // Note: if arr is "null" base and curr should both be null, so this
  //       is correct (caller checks for null if untagging to @ type)
  // base may not be null if you use t ? pointer subtraction to get 0 -- oh well
#ifndef NO_CYC_BOUNDS_CHECKS
  if(arr.curr < arr.base || arr.curr + elt_sz * num_elts > arr.last_plus_one)
    _throw_arraybounds();
#endif
  return arr.curr;
}
static inline 
unsigned _get_arr_size(struct _tagged_arr arr, unsigned elt_sz) {
  return (arr.curr<arr.base || arr.curr>=arr.last_plus_one) ? 0 : ((arr.last_plus_one - arr.curr) / elt_sz);
}
static inline
struct _tagged_arr _tagged_arr_plus(struct _tagged_arr arr, unsigned elt_sz,
				    int change) {
  struct _tagged_arr ans = arr;
  ans.curr += ((int)elt_sz)*change;
  return ans;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus(struct _tagged_arr * arr_ptr,
					    unsigned elt_sz, int change) {
  arr_ptr->curr += ((int)elt_sz)*change;
  return *arr_ptr;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus_post(struct _tagged_arr * arr_ptr,
						 unsigned elt_sz, int change) {
  struct _tagged_arr ans = *arr_ptr;
  arr_ptr->curr += ((int)elt_sz)*change;
  return ans;
}
				  
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
struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;extern struct Cyc_Std___sFILE*Cyc_Std_stdout;
extern struct Cyc_Std___sFILE*Cyc_Std_stderr;extern int Cyc_Std_remove(struct
_tagged_arr);extern int Cyc_Std_fclose(struct Cyc_Std___sFILE*);extern int Cyc_Std_fflush(
struct Cyc_Std___sFILE*);extern struct Cyc_Std___sFILE*Cyc_Std_fopen(struct
_tagged_arr __filename,struct _tagged_arr __modes);extern int Cyc_Std_fputc(int __c,
struct Cyc_Std___sFILE*__stream);extern int Cyc_Std_fputs(struct _tagged_arr __s,
struct Cyc_Std___sFILE*__stream);extern unsigned char Cyc_Std_FileCloseError[19];
extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};extern void Cyc_Std_file_close(struct Cyc_Std___sFILE*);
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct
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
struct Cyc_Std___sFILE*);extern int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
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
15];extern void*Cyc_Absyn_compress_kb(void*);extern struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc_Std___sFILE*f);struct Cyc_Declaration_spec;struct Cyc_Declarator;struct
Cyc_Abstractdeclarator;extern int Cyc_yyparse();extern unsigned char Cyc_AbstractDeclarator_tok[
27];struct Cyc_AbstractDeclarator_tok_struct{unsigned char*tag;struct Cyc_Abstractdeclarator*
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
21];struct _tuple4{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
unsigned char*tag;struct _tuple4*f1;};extern unsigned char Cyc_FnDecl_tok[15];
struct Cyc_FnDecl_tok_struct{unsigned char*tag;struct Cyc_Absyn_Fndecl*f1;};extern
unsigned char Cyc_IdList_tok[15];struct Cyc_IdList_tok_struct{unsigned char*tag;
struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDeclList_tok[21];struct Cyc_InitDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDecl_tok[
17];struct _tuple5{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
unsigned char*tag;struct _tuple5*f1;};extern unsigned char Cyc_InitializerList_tok[
24];struct Cyc_InitializerList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_Int_tok[12];struct _tuple6{void*f1;int f2;};struct Cyc_Int_tok_struct{
unsigned char*tag;struct _tuple6*f1;};extern unsigned char Cyc_Kind_tok[13];struct
Cyc_Kind_tok_struct{unsigned char*tag;void*f1;};extern unsigned char Cyc_Okay_tok[
13];extern unsigned char Cyc_ParamDeclListBool_tok[26];struct _tuple7{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct Cyc_ParamDeclListBool_tok_struct{unsigned char*tag;struct _tuple7*f1;}
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
unsigned char Cyc_QualSpecList_tok[21];struct _tuple8{struct Cyc_Absyn_Tqual f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_QualSpecList_tok_struct{
unsigned char*tag;struct _tuple8*f1;};extern unsigned char Cyc_Rgnorder_tok[17];
struct Cyc_Rgnorder_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_Scope_tok[14];struct Cyc_Scope_tok_struct{unsigned char*tag;void*
f1;};extern unsigned char Cyc_Short_tok[14];struct Cyc_Short_tok_struct{
unsigned char*tag;short f1;};extern unsigned char Cyc_Stmt_tok[13];struct Cyc_Stmt_tok_struct{
unsigned char*tag;struct Cyc_Absyn_Stmt*f1;};extern unsigned char Cyc_StorageClass_tok[
21];struct Cyc_StorageClass_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_String_tok[15];struct Cyc_String_tok_struct{unsigned char*tag;
struct _tagged_arr f1;};extern unsigned char Cyc_Stringopt_tok[18];struct Cyc_Stringopt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_StructFieldDeclListList_tok[
32];struct Cyc_StructFieldDeclListList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_StructFieldDeclList_tok[28];struct Cyc_StructFieldDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_StructOrUnion_tok[
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
struct Cyc_YY1_struct{unsigned char*tag;struct _tuple7*f1;};extern unsigned char Cyc_YYINITIALSVAL[
18];struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;
int last_column;};struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern struct
_tagged_arr*Cyc_Absynpp_cyc_stringptr;extern int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
extern struct _tagged_arr Cyc_Absynpp_char_escape(unsigned char);extern struct
_tagged_arr Cyc_Absynpp_string_escape(struct _tagged_arr);extern struct _tagged_arr
Cyc_Absynpp_prim2str(void*p);struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;
struct Cyc_List_List*f3;};extern struct _tuple9 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual
tq,void*t);extern void Cyc_Lex_lex_init();struct Cyc_Position_Segment{int start;int
end;};struct Cyc_MatchDecl_struct{int tag;struct _tagged_arr f1;};struct Cyc_Standalone_struct{
int tag;struct _tagged_arr f1;};struct _tuple10{int f1;void*f2;};extern struct _tuple10*
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
_tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_struct));_tmp0[0]=({struct Cyc_Lexing_Error_struct
_tmp1;_tmp1.tag=Cyc_Lexing_Error;_tmp1.f1=_tag_arr("empty token",sizeof(
unsigned char),12);_tmp1;});_tmp0;}));}else{return lbuf->lex_last_action;}}else{
if(c == 256){lbuf->lex_eof_reached=0;}}}}struct _tuple10*Cyc_token_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL0: {int _tmp2=lexbuf->lex_start_pos + 5;int _tmp3=(lexbuf->lex_curr_pos - 
lexbuf->lex_start_pos)- 7;return({struct _tuple10*_tmp4=_cycalloc(sizeof(struct
_tuple10));_tmp4->f1=Cyc_Lexing_lexeme_start(lexbuf);_tmp4->f2=(void*)({struct
Cyc_Standalone_struct*_tmp5=_cycalloc(sizeof(struct Cyc_Standalone_struct));_tmp5[
0]=({struct Cyc_Standalone_struct _tmp6;_tmp6.tag=1;_tmp6.f1=Cyc_Std_substring((
struct _tagged_arr)lexbuf->lex_buffer,_tmp2,(unsigned int)_tmp3);_tmp6;});_tmp5;});
_tmp4;});}case 1: _LL1: {int _tmp7=lexbuf->lex_start_pos + 4;int _tmp8=(lexbuf->lex_curr_pos
- lexbuf->lex_start_pos)- 6;return({struct _tuple10*_tmp9=_cycalloc(sizeof(struct
_tuple10));_tmp9->f1=Cyc_Lexing_lexeme_start(lexbuf);_tmp9->f2=(void*)({struct
Cyc_MatchDecl_struct*_tmpA=_cycalloc(sizeof(struct Cyc_MatchDecl_struct));_tmpA[0]=({
struct Cyc_MatchDecl_struct _tmpB;_tmpB.tag=0;_tmpB.f1=Cyc_Std_substring((struct
_tagged_arr)lexbuf->lex_buffer,_tmp7,(unsigned int)_tmp8);_tmpB;});_tmpA;});
_tmp9;});}case 2: _LL2: return 0;case 3: _LL3: return Cyc_token(lexbuf);default: _LL4:(
lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(lexbuf,lexstate);}(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmpC=_cycalloc(sizeof(struct Cyc_Lexing_Error_struct));
_tmpC[0]=({struct Cyc_Lexing_Error_struct _tmpD;_tmpD.tag=Cyc_Lexing_Error;_tmpD.f1=
_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmpD;});_tmpC;}));}
struct _tuple10*Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(
lexbuf,0);}struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];
struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_StructRes_struct{
int tag;struct Cyc_Absyn_Structdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*structdecls;struct Cyc_Dict_Dict*uniondecls;struct
Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*
typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;};struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{int tag;void*f1;void*f2;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*
le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_compress(
void*t);extern unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
void*Cyc_Dump_last_dumped=(void*)1;void Cyc_Dump_dump_alpha(struct _tagged_arr s){
if(Cyc_Dump_last_dumped == (void*)0){({struct Cyc_Std_String_pa_struct _tmpF;_tmpF.tag=
0;_tmpF.f1=(struct _tagged_arr)s;{void*_tmpE[1]={& _tmpF};Cyc_Std_printf(_tag_arr(" %s",
sizeof(unsigned char),4),_tag_arr(_tmpE,sizeof(void*),1));}});}else{({struct Cyc_Std_String_pa_struct
_tmp11;_tmp11.tag=0;_tmp11.f1=(struct _tagged_arr)s;{void*_tmp10[1]={& _tmp11};Cyc_Std_printf(
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp10,sizeof(void*),1));}});Cyc_Dump_last_dumped=(
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
void Cyc_Dump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(void*),void*);void Cyc_Dump_dumpstructfields(
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
Cyc_List_List*_tmp1E=_cycalloc(sizeof(struct Cyc_List_List));_tmp1E->hd=Cyc_Absynpp_cyc_stringptr;
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
unsigned char),1));}struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Dump_dumparg(
struct _tuple11*pr){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Dump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Dump_ignore,0);}void Cyc_Dump_dumpargs(struct
Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple11*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(
Cyc_Dump_dumparg,ts,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(
unsigned char),2),_tag_arr(", ",sizeof(unsigned char),3));}int Cyc_Dump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0){return 0;}{void*_tmp25=(void*)tms->hd;_LL32:
if((unsigned int)_tmp25 > 1?*((int*)_tmp25)== 1: 0){goto _LL33;}else{goto _LL34;}
_LL34: goto _LL35;_LL33: return 1;_LL35: return 0;_LL31:;}}static void Cyc_Dump_dumprgn(
void*t){void*_tmp26=Cyc_Tcutil_compress(t);_LL37: if((int)_tmp26 == 2){goto _LL38;}
else{goto _LL39;}_LL39: goto _LL3A;_LL38: Cyc_Dump_dump_alpha(_tag_arr("`H",sizeof(
unsigned char),3));goto _LL36;_LL3A: Cyc_Dump_dumpntyp(t);goto _LL36;_LL36:;}struct
_tuple12{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static struct _tuple12 Cyc_Dump_effects_split(
void*t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;{void*_tmp27=
Cyc_Tcutil_compress(t);void*_tmp28;struct Cyc_List_List*_tmp29;_LL3C: if((
unsigned int)_tmp27 > 3?*((int*)_tmp27)== 19: 0){_LL42: _tmp28=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp27)->f1;goto _LL3D;}else{goto _LL3E;}_LL3E: if((unsigned int)_tmp27 > 3?*((int*)
_tmp27)== 20: 0){_LL43: _tmp29=((struct Cyc_Absyn_JoinEff_struct*)_tmp27)->f1;goto
_LL3F;}else{goto _LL40;}_LL40: goto _LL41;_LL3D: rgions=({struct Cyc_List_List*_tmp2A=
_cycalloc(sizeof(struct Cyc_List_List));_tmp2A->hd=(void*)_tmp28;_tmp2A->tl=
rgions;_tmp2A;});goto _LL3B;_LL3F: for(0;_tmp29 != 0;_tmp29=_tmp29->tl){struct Cyc_List_List*
_tmp2C;struct Cyc_List_List*_tmp2D;struct _tuple12 _tmp2B=Cyc_Dump_effects_split((
void*)_tmp29->hd);_LL46: _tmp2D=_tmp2B.f1;goto _LL45;_LL45: _tmp2C=_tmp2B.f2;goto
_LL44;_LL44: rgions=Cyc_List_imp_append(_tmp2D,rgions);effects=Cyc_List_imp_append(
_tmp2C,effects);}goto _LL3B;_LL41: effects=({struct Cyc_List_List*_tmp2E=_cycalloc(
sizeof(struct Cyc_List_List));_tmp2E->hd=(void*)t;_tmp2E->tl=effects;_tmp2E;});
goto _LL3B;_LL3B:;}return({struct _tuple12 _tmp2F;_tmp2F.f1=rgions;_tmp2F.f2=
effects;_tmp2F;});}static void Cyc_Dump_dumpeff(void*t){struct Cyc_List_List*_tmp31;
struct Cyc_List_List*_tmp32;struct _tuple12 _tmp30=Cyc_Dump_effects_split(t);_LL49:
_tmp32=_tmp30.f1;goto _LL48;_LL48: _tmp31=_tmp30.f2;goto _LL47;_LL47: _tmp32=Cyc_List_imp_rev(
_tmp32);_tmp31=Cyc_List_imp_rev(_tmp31);for(0;_tmp31 != 0;_tmp31=_tmp31->tl){Cyc_Dump_dumpntyp((
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
void*_tmp59;int _tmp5A;void*_tmp5B;struct Cyc_List_List*_tmp5C;struct Cyc_List_List*
_tmp5D;struct _tuple1*_tmp5E;struct Cyc_List_List*_tmp5F;struct _tuple1*_tmp60;
struct Cyc_List_List*_tmp61;struct _tuple1*_tmp62;struct Cyc_List_List*_tmp63;
struct _tuple1*_tmp64;struct Cyc_List_List*_tmp65;struct Cyc_List_List*_tmp66;
struct Cyc_Core_Opt*_tmp67;struct Cyc_List_List*_tmp68;struct _tuple1*_tmp69;void*
_tmp6A;_LL4B: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 7: 0){goto _LL4C;}else{
goto _LL4D;}_LL4D: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 8: 0){goto _LL4E;}
else{goto _LL4F;}_LL4F: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 4: 0){goto
_LL50;}else{goto _LL51;}_LL51: if((int)_tmp33 == 0){goto _LL52;}else{goto _LL53;}
_LL53: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 1: 0){_LL95: _tmp34=((struct Cyc_Absyn_VarType_struct*)
_tmp33)->f1;goto _LL54;}else{goto _LL55;}_LL55: if((unsigned int)_tmp33 > 3?*((int*)
_tmp33)== 0: 0){_LL98: _tmp37=((struct Cyc_Absyn_Evar_struct*)_tmp33)->f1;goto _LL97;
_LL97: _tmp36=((struct Cyc_Absyn_Evar_struct*)_tmp33)->f2;if(_tmp36 == 0){goto _LL96;}
else{goto _LL57;}_LL96: _tmp35=((struct Cyc_Absyn_Evar_struct*)_tmp33)->f3;goto
_LL56;}else{goto _LL57;}_LL57: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 0: 0){
_LL9C: _tmp3C=((struct Cyc_Absyn_Evar_struct*)_tmp33)->f1;goto _LL9A;_LL9A: _tmp39=((
struct Cyc_Absyn_Evar_struct*)_tmp33)->f2;if(_tmp39 == 0){goto _LL59;}else{_tmp3A=*
_tmp39;_LL9B: _tmp3B=(void*)_tmp3A.v;goto _LL99;}_LL99: _tmp38=((struct Cyc_Absyn_Evar_struct*)
_tmp33)->f3;goto _LL58;}else{goto _LL59;}_LL59: if((unsigned int)_tmp33 > 3?*((int*)
_tmp33)== 2: 0){_LL9D: _tmp3D=((struct Cyc_Absyn_TunionType_struct*)_tmp33)->f1;
_LLA0: _tmp40=(void*)_tmp3D.tunion_info;goto _LL9F;_LL9F: _tmp3F=_tmp3D.targs;goto
_LL9E;_LL9E: _tmp3E=(void*)_tmp3D.rgn;goto _LL5A;}else{goto _LL5B;}_LL5B: if((
unsigned int)_tmp33 > 3?*((int*)_tmp33)== 3: 0){_LLA1: _tmp41=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp33)->f1;_LLA3: _tmp43=(void*)_tmp41.field_info;goto _LLA2;_LLA2: _tmp42=_tmp41.targs;
goto _LL5C;}else{goto _LL5D;}_LL5D: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 12:
0){_LLA4: _tmp44=((struct Cyc_Absyn_EnumType_struct*)_tmp33)->f1;goto _LL5E;}else{
goto _LL5F;}_LL5F: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 16: 0){_LLA5: _tmp45=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp33)->f1;goto _LL60;}else{goto _LL61;}
_LL61: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5: 0){_LLA7: _tmp47=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp47 == 0){goto _LLA6;}else{
goto _LL63;}_LLA6: _tmp46=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((
int)_tmp46 == 2){goto _LL62;}else{goto _LL63;}}else{goto _LL63;}_LL63: if((
unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5: 0){_LLA9: _tmp49=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f1;if((int)_tmp49 == 0){goto _LLA8;}else{goto _LL65;}_LLA8: _tmp48=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((int)_tmp48 == 0){goto _LL64;}else{
goto _LL65;}}else{goto _LL65;}_LL65: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5:
0){_LLAB: _tmp4B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)
_tmp4B == 1){goto _LLAA;}else{goto _LL67;}_LLAA: _tmp4A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f2;if((int)_tmp4A == 0){goto _LL66;}else{goto _LL67;}}else{goto _LL67;}
_LL67: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5: 0){_LLAD: _tmp4D=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp4D == 0){goto _LLAC;}else{
goto _LL69;}_LLAC: _tmp4C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((
int)_tmp4C == 1){goto _LL68;}else{goto _LL69;}}else{goto _LL69;}_LL69: if((
unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5: 0){_LLAF: _tmp4F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f1;if((int)_tmp4F == 1){goto _LLAE;}else{goto _LL6B;}_LLAE: _tmp4E=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((int)_tmp4E == 1){goto _LL6A;}else{
goto _LL6B;}}else{goto _LL6B;}_LL6B: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5:
0){_LLB1: _tmp51=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)
_tmp51 == 1){goto _LLB0;}else{goto _LL6D;}_LLB0: _tmp50=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f2;if((int)_tmp50 == 2){goto _LL6C;}else{goto _LL6D;}}else{goto _LL6D;}
_LL6D: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5: 0){_LLB4: _tmp53=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp53 == 0){goto _LLB3;}else{
goto _LL6F;}_LLB3: _tmp52=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((
int)_tmp52 == 3){goto _LLB2;}else{goto _LL6F;}}else{goto _LL6F;}_LLB2: if(Cyc_Dump_to_VC){
goto _LL6E;}else{goto _LL6F;}_LL6F: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5:
0){_LLB6: _tmp55=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)
_tmp55 == 0){goto _LLB5;}else{goto _LL71;}_LLB5: _tmp54=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f2;if((int)_tmp54 == 3){goto _LL70;}else{goto _LL71;}}else{goto _LL71;}
_LL71: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5: 0){_LLB9: _tmp57=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)_tmp57 == 1){goto _LLB8;}else{
goto _LL73;}_LLB8: _tmp56=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f2;if((
int)_tmp56 == 3){goto _LLB7;}else{goto _LL73;}}else{goto _LL73;}_LLB7: if(Cyc_Dump_to_VC){
goto _LL72;}else{goto _LL73;}_LL73: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 5:
0){_LLBB: _tmp59=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33)->f1;if((int)
_tmp59 == 1){goto _LLBA;}else{goto _LL75;}_LLBA: _tmp58=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33)->f2;if((int)_tmp58 == 3){goto _LL74;}else{goto _LL75;}}else{goto _LL75;}
_LL75: if((int)_tmp33 == 1){goto _LL76;}else{goto _LL77;}_LL77: if((unsigned int)
_tmp33 > 3?*((int*)_tmp33)== 6: 0){_LLBC: _tmp5A=((struct Cyc_Absyn_DoubleType_struct*)
_tmp33)->f1;goto _LL78;}else{goto _LL79;}_LL79: if((unsigned int)_tmp33 > 3?*((int*)
_tmp33)== 13: 0){_LLBD: _tmp5B=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp33)->f1;
goto _LL7A;}else{goto _LL7B;}_LL7B: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 9:
0){_LLBE: _tmp5C=((struct Cyc_Absyn_TupleType_struct*)_tmp33)->f1;goto _LL7C;}else{
goto _LL7D;}_LL7D: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 10: 0){_LLC0: _tmp5E=((
struct Cyc_Absyn_StructType_struct*)_tmp33)->f1;if(_tmp5E == 0){goto _LLBF;}else{
goto _LL7F;}_LLBF: _tmp5D=((struct Cyc_Absyn_StructType_struct*)_tmp33)->f2;goto
_LL7E;}else{goto _LL7F;}_LL7F: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 10: 0){
_LLC2: _tmp60=((struct Cyc_Absyn_StructType_struct*)_tmp33)->f1;goto _LLC1;_LLC1:
_tmp5F=((struct Cyc_Absyn_StructType_struct*)_tmp33)->f2;goto _LL80;}else{goto
_LL81;}_LL81: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 11: 0){_LLC4: _tmp62=((
struct Cyc_Absyn_UnionType_struct*)_tmp33)->f1;if(_tmp62 == 0){goto _LLC3;}else{
goto _LL83;}_LLC3: _tmp61=((struct Cyc_Absyn_UnionType_struct*)_tmp33)->f2;goto
_LL82;}else{goto _LL83;}_LL83: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 11: 0){
_LLC6: _tmp64=((struct Cyc_Absyn_UnionType_struct*)_tmp33)->f1;goto _LLC5;_LLC5:
_tmp63=((struct Cyc_Absyn_UnionType_struct*)_tmp33)->f2;goto _LL84;}else{goto _LL85;}
_LL85: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 14: 0){_LLC7: _tmp65=((struct
Cyc_Absyn_AnonStructType_struct*)_tmp33)->f1;goto _LL86;}else{goto _LL87;}_LL87:
if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 15: 0){_LLC8: _tmp66=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp33)->f1;goto _LL88;}else{goto _LL89;}_LL89: if((unsigned int)_tmp33 > 3?*((int*)
_tmp33)== 18: 0){_LLCB: _tmp69=((struct Cyc_Absyn_TypedefType_struct*)_tmp33)->f1;
goto _LLCA;_LLCA: _tmp68=((struct Cyc_Absyn_TypedefType_struct*)_tmp33)->f2;goto
_LLC9;_LLC9: _tmp67=((struct Cyc_Absyn_TypedefType_struct*)_tmp33)->f3;goto _LL8A;}
else{goto _LL8B;}_LL8B: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 17: 0){_LLCC:
_tmp6A=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp33)->f1;goto _LL8C;}
else{goto _LL8D;}_LL8D: if((int)_tmp33 == 2){goto _LL8E;}else{goto _LL8F;}_LL8F: if((
unsigned int)_tmp33 > 3?*((int*)_tmp33)== 19: 0){goto _LL90;}else{goto _LL91;}_LL91:
if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 21: 0){goto _LL92;}else{goto _LL93;}
_LL93: if((unsigned int)_tmp33 > 3?*((int*)_tmp33)== 20: 0){goto _LL94;}else{goto
_LL4A;}_LL4C: return;_LL4E: return;_LL50: return;_LL52: Cyc_Dump_dump_alpha(_tag_arr("void",
sizeof(unsigned char),5));return;_LL54: Cyc_Dump_dump_alphastr(_tmp34->name);
return;_LL56: Cyc_Dump_dump_char((int)'%');if(_tmp37 == 0){Cyc_Dump_dump_char((int)'?');}
else{Cyc_Dump_dumpkind((void*)_tmp37->v);}Cyc_Dump_dump_symbol((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp6C;_tmp6C.tag=1;_tmp6C.f1=(int)((
unsigned int)_tmp35);{void*_tmp6B[1]={& _tmp6C};Cyc_Std_aprintf(_tag_arr("(%d)",
sizeof(unsigned char),5),_tag_arr(_tmp6B,sizeof(void*),1));}}));return;_LL58: Cyc_Dump_dumpntyp(
_tmp3B);return;_LL5A:{void*_tmp6D=_tmp40;struct Cyc_Absyn_UnknownTunionInfo _tmp6E;
int _tmp6F;struct _tuple1*_tmp70;struct Cyc_Absyn_Tuniondecl**_tmp71;struct Cyc_Absyn_Tuniondecl*
_tmp72;struct Cyc_Absyn_Tuniondecl _tmp73;int _tmp74;struct _tuple1*_tmp75;_LLCE: if(*((
int*)_tmp6D)== 0){_LLD2: _tmp6E=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp6D)->f1;
_LLD4: _tmp70=_tmp6E.name;goto _LLD3;_LLD3: _tmp6F=_tmp6E.is_xtunion;goto _LLCF;}
else{goto _LLD0;}_LLD0: if(*((int*)_tmp6D)== 1){_LLD5: _tmp71=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp6D)->f1;_tmp72=*_tmp71;_tmp73=*_tmp72;_LLD7: _tmp75=_tmp73.name;goto _LLD6;
_LLD6: _tmp74=_tmp73.is_xtunion;goto _LLD1;}else{goto _LLCD;}_LLCF: _tmp75=_tmp70;
_tmp74=_tmp6F;goto _LLD1;_LLD1: if(_tmp74){Cyc_Dump_dump_alpha(_tag_arr("xtunion",
sizeof(unsigned char),8));}else{Cyc_Dump_dump_alpha(_tag_arr("tunion",sizeof(
unsigned char),7));}{void*_tmp76=Cyc_Tcutil_compress(_tmp3E);_LLD9: if((int)
_tmp76 == 2){goto _LLDA;}else{goto _LLDB;}_LLDB: goto _LLDC;_LLDA: goto _LLD8;_LLDC: Cyc_Dump_dumptyp(
_tmp3E);goto _LLD8;_LLD8:;}Cyc_Dump_dumpqvar(_tmp75);Cyc_Dump_dumptps(_tmp3F);
goto _LLCD;_LLCD:;}goto _LL4A;_LL5C:{void*_tmp77=_tmp43;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp78;int _tmp79;struct _tuple1*_tmp7A;struct _tuple1*_tmp7B;struct Cyc_Absyn_Tunionfield*
_tmp7C;struct Cyc_Absyn_Tunionfield _tmp7D;struct _tuple1*_tmp7E;struct Cyc_Absyn_Tuniondecl*
_tmp7F;struct Cyc_Absyn_Tuniondecl _tmp80;int _tmp81;struct _tuple1*_tmp82;_LLDE: if(*((
int*)_tmp77)== 0){_LLE2: _tmp78=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp77)->f1;_LLE5: _tmp7B=_tmp78.tunion_name;goto _LLE4;_LLE4: _tmp7A=_tmp78.field_name;
goto _LLE3;_LLE3: _tmp79=_tmp78.is_xtunion;goto _LLDF;}else{goto _LLE0;}_LLE0: if(*((
int*)_tmp77)== 1){_LLE8: _tmp7F=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp77)->f1;
_tmp80=*_tmp7F;_LLEA: _tmp82=_tmp80.name;goto _LLE9;_LLE9: _tmp81=_tmp80.is_xtunion;
goto _LLE6;_LLE6: _tmp7C=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp77)->f2;
_tmp7D=*_tmp7C;_LLE7: _tmp7E=_tmp7D.name;goto _LLE1;}else{goto _LLDD;}_LLDF: _tmp82=
_tmp7B;_tmp81=_tmp79;_tmp7E=_tmp7A;goto _LLE1;_LLE1: if(_tmp81){Cyc_Dump_dump_alpha(
_tag_arr("xtunion",sizeof(unsigned char),8));}else{Cyc_Dump_dump_alpha(_tag_arr("tunion",
sizeof(unsigned char),7));}Cyc_Dump_dumpqvar(_tmp82);Cyc_Dump_dump_char((int)'.');
Cyc_Dump_dumpqvar(_tmp7E);Cyc_Dump_dumptps(_tmp42);goto _LLDD;_LLDD:;}goto _LL4A;
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
sizeof(unsigned char),3));Cyc_Dump_dumpargs(_tmp5C);return;_LL7E: Cyc_Dump_dump_alpha(
_tag_arr("struct",sizeof(unsigned char),7));Cyc_Dump_dumptps(_tmp5D);return;
_LL80: Cyc_Dump_dump_alpha(_tag_arr("struct",sizeof(unsigned char),7));Cyc_Dump_dumpqvar((
struct _tuple1*)_check_null(_tmp60));Cyc_Dump_dumptps(_tmp5F);return;_LL82: Cyc_Dump_dump_alpha(
_tag_arr("union",sizeof(unsigned char),6));Cyc_Dump_dumptps(_tmp61);return;_LL84:
Cyc_Dump_dump_alpha(_tag_arr("union",sizeof(unsigned char),6));Cyc_Dump_dumpqvar((
struct _tuple1*)_check_null(_tmp64));Cyc_Dump_dumptps(_tmp63);return;_LL86: Cyc_Dump_dump_alpha(
_tag_arr("struct",sizeof(unsigned char),7));Cyc_Dump_dump_lb();Cyc_Dump_dumpstructfields(
_tmp65);Cyc_Dump_dump_rb();return;_LL88: Cyc_Dump_dump_alpha(_tag_arr("union",
sizeof(unsigned char),6));Cyc_Dump_dump_lb();Cyc_Dump_dumpstructfields(_tmp66);
Cyc_Dump_dump_rb();return;_LL8A:(Cyc_Dump_dumpqvar(_tmp69),Cyc_Dump_dumptps(
_tmp68));return;_LL8C: Cyc_Dump_dumprgn(_tmp6A);return;_LL8E: return;_LL90: return;
_LL92: return;_LL94: return;_LL4A:;}void Cyc_Dump_dumpvaropt(struct Cyc_Core_Opt*vo){
if(vo != 0){Cyc_Dump_dump_alphastr((struct _tagged_arr*)vo->v);}}void Cyc_Dump_dumpfunarg(
struct _tuple2*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),
struct Cyc_Core_Opt*))Cyc_Dump_dumptqtd)((*t).f2,(*t).f3,Cyc_Dump_dumpvaropt,(*t).f1);}
void Cyc_Dump_dump_rgncmp(struct _tuple0*cmp){struct _tuple0 _tmp84;void*_tmp85;void*
_tmp86;struct _tuple0*_tmp83=cmp;_tmp84=*_tmp83;_LLED: _tmp86=_tmp84.f1;goto _LLEC;
_LLEC: _tmp85=_tmp84.f2;goto _LLEB;_LLEB: Cyc_Dump_dumptyp(_tmp86);Cyc_Dump_dump_char((
int)'<');Cyc_Dump_dumptyp(_tmp85);}void Cyc_Dump_dump_rgnpo(struct Cyc_List_List*
rgn_po){((void(*)(void(*f)(struct _tuple0*),struct Cyc_List_List*l,struct
_tagged_arr sep))Cyc_Dump_dump_sep)(Cyc_Dump_dump_rgncmp,rgn_po,_tag_arr(",",
sizeof(unsigned char),2));}void Cyc_Dump_dumpfunargs(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po){Cyc_Dump_dump_char((int)'(');for(0;args != 0;args=
args->tl){Cyc_Dump_dumpfunarg((struct _tuple2*)args->hd);if((args->tl != 0? 1:
c_varargs)? 1: cyc_varargs != 0){Cyc_Dump_dump_symbol(_tag_arr(", ",sizeof(
unsigned char),3));}}if(c_varargs){Cyc_Dump_dump_symbol(_tag_arr("...",sizeof(
unsigned char),4));}else{if(cyc_varargs != 0){struct _tuple2*_tmp87=({struct
_tuple2*_tmp88=_cycalloc(sizeof(struct _tuple2));_tmp88->f1=cyc_varargs->name;
_tmp88->f2=cyc_varargs->tq;_tmp88->f3=(void*)cyc_varargs->type;_tmp88;});Cyc_Dump_dump_symbol(
_tag_arr("...",sizeof(unsigned char),4));if(cyc_varargs->inject){Cyc_Dump_dump_alpha(
_tag_arr(" inject",sizeof(unsigned char),8));}Cyc_Dump_dumpfunarg(_tmp87);}}if(
effopt != 0){Cyc_Dump_dump_semi();Cyc_Dump_dumpeff((void*)effopt->v);}if(rgn_po != 
0){Cyc_Dump_dump_char((int)':');Cyc_Dump_dump_rgnpo(rgn_po);}Cyc_Dump_dump_char((
int)')');}void Cyc_Dump_dumptyp(void*t){((void(*)(struct Cyc_Absyn_Tqual,void*,
void(*f)(int),int))Cyc_Dump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp89;_tmp89.q_const=
0;_tmp89.q_volatile=0;_tmp89.q_restrict=0;_tmp89;}),t,(void(*)(int x))Cyc_Dump_ignore,
0);}void Cyc_Dump_dumpdesignator(void*d){void*_tmp8A=d;struct Cyc_Absyn_Exp*_tmp8B;
struct _tagged_arr*_tmp8C;_LLEF: if(*((int*)_tmp8A)== 0){_LLF3: _tmp8B=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp8A)->f1;goto _LLF0;}else{goto _LLF1;}_LLF1: if(*((int*)_tmp8A)== 1){_LLF4:
_tmp8C=((struct Cyc_Absyn_FieldName_struct*)_tmp8A)->f1;goto _LLF2;}else{goto _LLEE;}
_LLF0: Cyc_Dump_dump_symbol(_tag_arr(".[",sizeof(unsigned char),3));Cyc_Dump_dumpexp(
_tmp8B);Cyc_Dump_dump_char((int)']');goto _LLEE;_LLF2: Cyc_Dump_dump_char((int)'.');
Cyc_Dump_dump_alpha(*_tmp8C);goto _LLEE;_LLEE:;}struct _tuple13{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};void Cyc_Dump_dumpde(struct _tuple13*de){Cyc_Dump_egroup(
Cyc_Dump_dumpdesignator,(*de).f1,_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",
sizeof(unsigned char),2),_tag_arr("=",sizeof(unsigned char),2));Cyc_Dump_dumpexp((*
de).f2);}void Cyc_Dump_dumpexps_prec(int inprec,struct Cyc_List_List*es){((void(*)(
void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group_c)(
Cyc_Dump_dumpexp_prec,inprec,es,_tag_arr("",sizeof(unsigned char),1),_tag_arr("",
sizeof(unsigned char),1),_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Dump_dumpexp_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= 
myprec){Cyc_Dump_dump_char((int)'(');}{void*_tmp8D=(void*)e->r;void*_tmp8E;
unsigned char _tmp8F;void*_tmp90;void*_tmp91;short _tmp92;void*_tmp93;void*_tmp94;
int _tmp95;void*_tmp96;void*_tmp97;int _tmp98;void*_tmp99;void*_tmp9A;long long
_tmp9B;void*_tmp9C;void*_tmp9D;struct _tagged_arr _tmp9E;void*_tmp9F;void*_tmpA0;
struct _tagged_arr _tmpA1;struct _tuple1*_tmpA2;struct _tuple1*_tmpA3;struct Cyc_List_List*
_tmpA4;void*_tmpA5;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Core_Opt*_tmpA7;struct
Cyc_Absyn_Exp*_tmpA8;void*_tmpA9;struct Cyc_Absyn_Exp*_tmpAA;void*_tmpAB;struct
Cyc_Absyn_Exp*_tmpAC;void*_tmpAD;struct Cyc_Absyn_Exp*_tmpAE;void*_tmpAF;struct
Cyc_Absyn_Exp*_tmpB0;struct Cyc_Absyn_Exp*_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;
struct Cyc_Absyn_Exp*_tmpB3;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;
struct Cyc_List_List*_tmpB6;struct Cyc_Absyn_Exp*_tmpB7;struct Cyc_List_List*_tmpB8;
struct Cyc_Absyn_Exp*_tmpB9;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Exp*_tmpBB;
struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;void*_tmpBE;struct Cyc_Absyn_Exp*
_tmpBF;struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_Absyn_Exp*_tmpC1;void*_tmpC2;struct
Cyc_Absyn_Exp*_tmpC3;void*_tmpC4;struct _tagged_arr*_tmpC5;void*_tmpC6;void*
_tmpC7;unsigned int _tmpC8;void*_tmpC9;void*_tmpCA;struct Cyc_List_List*_tmpCB;
struct Cyc_Absyn_Exp*_tmpCC;struct _tagged_arr*_tmpCD;struct Cyc_Absyn_Exp*_tmpCE;
struct _tagged_arr*_tmpCF;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpD1;
struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_List_List*_tmpD3;struct Cyc_List_List*_tmpD4;
struct _tuple2*_tmpD5;struct Cyc_List_List*_tmpD6;struct Cyc_Absyn_Exp*_tmpD7;
struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Vardecl*_tmpD9;struct Cyc_List_List*
_tmpDA;struct _tuple1*_tmpDB;struct Cyc_List_List*_tmpDC;struct Cyc_Absyn_Tunionfield*
_tmpDD;struct Cyc_List_List*_tmpDE;struct _tuple1*_tmpDF;struct _tuple1*_tmpE0;
struct Cyc_Absyn_MallocInfo _tmpE1;struct Cyc_Absyn_Exp*_tmpE2;void**_tmpE3;struct
Cyc_Absyn_Exp*_tmpE4;int _tmpE5;struct Cyc_List_List*_tmpE6;struct Cyc_Core_Opt*
_tmpE7;struct Cyc_Absyn_Stmt*_tmpE8;struct Cyc_Absyn_Fndecl*_tmpE9;struct Cyc_Absyn_Exp*
_tmpEA;_LLF6: if(*((int*)_tmp8D)== 0){_LL158: _tmp8E=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8D)->f1;if((unsigned int)_tmp8E > 1?*((int*)_tmp8E)== 0: 0){_LL15A: _tmp90=(
void*)((struct Cyc_Absyn_Char_c_struct*)_tmp8E)->f1;goto _LL159;_LL159: _tmp8F=((
struct Cyc_Absyn_Char_c_struct*)_tmp8E)->f2;goto _LLF7;}else{goto _LLF8;}}else{goto
_LLF8;}_LLF8: if(*((int*)_tmp8D)== 0){_LL15B: _tmp91=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8D)->f1;if((unsigned int)_tmp91 > 1?*((int*)_tmp91)== 1: 0){_LL15D: _tmp93=(
void*)((struct Cyc_Absyn_Short_c_struct*)_tmp91)->f1;goto _LL15C;_LL15C: _tmp92=((
struct Cyc_Absyn_Short_c_struct*)_tmp91)->f2;goto _LLF9;}else{goto _LLFA;}}else{
goto _LLFA;}_LLFA: if(*((int*)_tmp8D)== 0){_LL15E: _tmp94=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8D)->f1;if((unsigned int)_tmp94 > 1?*((int*)_tmp94)== 2: 0){_LL160: _tmp96=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp94)->f1;if((int)_tmp96 == 0){goto _LL15F;}
else{goto _LLFC;}_LL15F: _tmp95=((struct Cyc_Absyn_Int_c_struct*)_tmp94)->f2;goto
_LLFB;}else{goto _LLFC;}}else{goto _LLFC;}_LLFC: if(*((int*)_tmp8D)== 0){_LL161:
_tmp97=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp8D)->f1;if((unsigned int)
_tmp97 > 1?*((int*)_tmp97)== 2: 0){_LL163: _tmp99=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp97)->f1;if((int)_tmp99 == 1){goto _LL162;}else{goto _LLFE;}_LL162: _tmp98=((
struct Cyc_Absyn_Int_c_struct*)_tmp97)->f2;goto _LLFD;}else{goto _LLFE;}}else{goto
_LLFE;}_LLFE: if(*((int*)_tmp8D)== 0){_LL164: _tmp9A=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8D)->f1;if((unsigned int)_tmp9A > 1?*((int*)_tmp9A)== 3: 0){_LL166: _tmp9C=(
void*)((struct Cyc_Absyn_LongLong_c_struct*)_tmp9A)->f1;goto _LL165;_LL165: _tmp9B=((
struct Cyc_Absyn_LongLong_c_struct*)_tmp9A)->f2;goto _LLFF;}else{goto _LL100;}}
else{goto _LL100;}_LL100: if(*((int*)_tmp8D)== 0){_LL167: _tmp9D=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8D)->f1;if((unsigned int)_tmp9D > 1?*((int*)_tmp9D)== 4: 0){_LL168: _tmp9E=((
struct Cyc_Absyn_Float_c_struct*)_tmp9D)->f1;goto _LL101;}else{goto _LL102;}}else{
goto _LL102;}_LL102: if(*((int*)_tmp8D)== 0){_LL169: _tmp9F=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8D)->f1;if((int)_tmp9F == 0){goto _LL103;}else{goto _LL104;}}else{goto _LL104;}
_LL104: if(*((int*)_tmp8D)== 0){_LL16A: _tmpA0=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8D)->f1;if((unsigned int)_tmpA0 > 1?*((int*)_tmpA0)== 5: 0){_LL16B: _tmpA1=((
struct Cyc_Absyn_String_c_struct*)_tmpA0)->f1;goto _LL105;}else{goto _LL106;}}else{
goto _LL106;}_LL106: if(*((int*)_tmp8D)== 2){_LL16C: _tmpA2=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp8D)->f1;goto _LL107;}else{goto _LL108;}_LL108: if(*((int*)_tmp8D)== 1){_LL16D:
_tmpA3=((struct Cyc_Absyn_Var_e_struct*)_tmp8D)->f1;goto _LL109;}else{goto _LL10A;}
_LL10A: if(*((int*)_tmp8D)== 3){_LL16F: _tmpA5=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp8D)->f1;goto _LL16E;_LL16E: _tmpA4=((struct Cyc_Absyn_Primop_e_struct*)_tmp8D)->f2;
goto _LL10B;}else{goto _LL10C;}_LL10C: if(*((int*)_tmp8D)== 4){_LL172: _tmpA8=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp8D)->f1;goto _LL171;_LL171: _tmpA7=((struct
Cyc_Absyn_AssignOp_e_struct*)_tmp8D)->f2;goto _LL170;_LL170: _tmpA6=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp8D)->f3;goto _LL10D;}else{goto _LL10E;}_LL10E: if(*((int*)_tmp8D)== 5){_LL174:
_tmpAA=((struct Cyc_Absyn_Increment_e_struct*)_tmp8D)->f1;goto _LL173;_LL173:
_tmpA9=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp8D)->f2;if((int)_tmpA9 == 
0){goto _LL10F;}else{goto _LL110;}}else{goto _LL110;}_LL110: if(*((int*)_tmp8D)== 5){
_LL176: _tmpAC=((struct Cyc_Absyn_Increment_e_struct*)_tmp8D)->f1;goto _LL175;
_LL175: _tmpAB=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp8D)->f2;if((int)
_tmpAB == 2){goto _LL111;}else{goto _LL112;}}else{goto _LL112;}_LL112: if(*((int*)
_tmp8D)== 5){_LL178: _tmpAE=((struct Cyc_Absyn_Increment_e_struct*)_tmp8D)->f1;
goto _LL177;_LL177: _tmpAD=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp8D)->f2;
if((int)_tmpAD == 1){goto _LL113;}else{goto _LL114;}}else{goto _LL114;}_LL114: if(*((
int*)_tmp8D)== 5){_LL17A: _tmpB0=((struct Cyc_Absyn_Increment_e_struct*)_tmp8D)->f1;
goto _LL179;_LL179: _tmpAF=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp8D)->f2;
if((int)_tmpAF == 3){goto _LL115;}else{goto _LL116;}}else{goto _LL116;}_LL116: if(*((
int*)_tmp8D)== 6){_LL17D: _tmpB3=((struct Cyc_Absyn_Conditional_e_struct*)_tmp8D)->f1;
goto _LL17C;_LL17C: _tmpB2=((struct Cyc_Absyn_Conditional_e_struct*)_tmp8D)->f2;
goto _LL17B;_LL17B: _tmpB1=((struct Cyc_Absyn_Conditional_e_struct*)_tmp8D)->f3;
goto _LL117;}else{goto _LL118;}_LL118: if(*((int*)_tmp8D)== 7){_LL17F: _tmpB5=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp8D)->f1;goto _LL17E;_LL17E: _tmpB4=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp8D)->f2;goto _LL119;}else{goto _LL11A;}_LL11A: if(*((
int*)_tmp8D)== 8){_LL181: _tmpB7=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp8D)->f1;
goto _LL180;_LL180: _tmpB6=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp8D)->f2;
goto _LL11B;}else{goto _LL11C;}_LL11C: if(*((int*)_tmp8D)== 9){_LL183: _tmpB9=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp8D)->f1;goto _LL182;_LL182: _tmpB8=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp8D)->f2;goto _LL11D;}else{goto _LL11E;}_LL11E: if(*((
int*)_tmp8D)== 10){_LL184: _tmpBA=((struct Cyc_Absyn_Throw_e_struct*)_tmp8D)->f1;
goto _LL11F;}else{goto _LL120;}_LL120: if(*((int*)_tmp8D)== 11){_LL185: _tmpBB=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp8D)->f1;goto _LL121;}else{goto _LL122;}
_LL122: if(*((int*)_tmp8D)== 12){_LL186: _tmpBC=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp8D)->f1;goto _LL123;}else{goto _LL124;}_LL124: if(*((int*)_tmp8D)== 13){_LL188:
_tmpBE=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp8D)->f1;goto _LL187;_LL187:
_tmpBD=((struct Cyc_Absyn_Cast_e_struct*)_tmp8D)->f2;goto _LL125;}else{goto _LL126;}
_LL126: if(*((int*)_tmp8D)== 14){_LL189: _tmpBF=((struct Cyc_Absyn_Address_e_struct*)
_tmp8D)->f1;goto _LL127;}else{goto _LL128;}_LL128: if(*((int*)_tmp8D)== 15){_LL18B:
_tmpC1=((struct Cyc_Absyn_New_e_struct*)_tmp8D)->f1;goto _LL18A;_LL18A: _tmpC0=((
struct Cyc_Absyn_New_e_struct*)_tmp8D)->f2;goto _LL129;}else{goto _LL12A;}_LL12A:
if(*((int*)_tmp8D)== 16){_LL18C: _tmpC2=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp8D)->f1;goto _LL12B;}else{goto _LL12C;}_LL12C: if(*((int*)_tmp8D)== 17){_LL18D:
_tmpC3=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp8D)->f1;goto _LL12D;}else{goto
_LL12E;}_LL12E: if(*((int*)_tmp8D)== 18){_LL190: _tmpC6=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp8D)->f1;goto _LL18E;_LL18E: _tmpC4=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp8D)->f2;if(*((int*)_tmpC4)== 0){_LL18F: _tmpC5=((struct Cyc_Absyn_StructField_struct*)
_tmpC4)->f1;goto _LL12F;}else{goto _LL130;}}else{goto _LL130;}_LL130: if(*((int*)
_tmp8D)== 18){_LL193: _tmpC9=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp8D)->f1;
goto _LL191;_LL191: _tmpC7=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp8D)->f2;
if(*((int*)_tmpC7)== 1){_LL192: _tmpC8=((struct Cyc_Absyn_TupleIndex_struct*)
_tmpC7)->f1;goto _LL131;}else{goto _LL132;}}else{goto _LL132;}_LL132: if(*((int*)
_tmp8D)== 19){_LL195: _tmpCB=((struct Cyc_Absyn_Gentyp_e_struct*)_tmp8D)->f1;goto
_LL194;_LL194: _tmpCA=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp8D)->f2;goto
_LL133;}else{goto _LL134;}_LL134: if(*((int*)_tmp8D)== 20){_LL196: _tmpCC=((struct
Cyc_Absyn_Deref_e_struct*)_tmp8D)->f1;goto _LL135;}else{goto _LL136;}_LL136: if(*((
int*)_tmp8D)== 21){_LL198: _tmpCE=((struct Cyc_Absyn_StructMember_e_struct*)_tmp8D)->f1;
goto _LL197;_LL197: _tmpCD=((struct Cyc_Absyn_StructMember_e_struct*)_tmp8D)->f2;
goto _LL137;}else{goto _LL138;}_LL138: if(*((int*)_tmp8D)== 22){_LL19A: _tmpD0=((
struct Cyc_Absyn_StructArrow_e_struct*)_tmp8D)->f1;goto _LL199;_LL199: _tmpCF=((
struct Cyc_Absyn_StructArrow_e_struct*)_tmp8D)->f2;goto _LL139;}else{goto _LL13A;}
_LL13A: if(*((int*)_tmp8D)== 23){_LL19C: _tmpD2=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp8D)->f1;goto _LL19B;_LL19B: _tmpD1=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp8D)->f2;goto _LL13B;}else{goto _LL13C;}_LL13C: if(*((int*)_tmp8D)== 24){_LL19D:
_tmpD3=((struct Cyc_Absyn_Tuple_e_struct*)_tmp8D)->f1;goto _LL13D;}else{goto _LL13E;}
_LL13E: if(*((int*)_tmp8D)== 25){_LL19F: _tmpD5=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp8D)->f1;goto _LL19E;_LL19E: _tmpD4=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp8D)->f2;goto _LL13F;}else{goto _LL140;}_LL140: if(*((int*)_tmp8D)== 26){_LL1A0:
_tmpD6=((struct Cyc_Absyn_Array_e_struct*)_tmp8D)->f1;goto _LL141;}else{goto _LL142;}
_LL142: if(*((int*)_tmp8D)== 27){_LL1A3: _tmpD9=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp8D)->f1;goto _LL1A2;_LL1A2: _tmpD8=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp8D)->f2;goto _LL1A1;_LL1A1: _tmpD7=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp8D)->f3;goto _LL143;}else{goto _LL144;}_LL144: if(*((int*)_tmp8D)== 28){_LL1A5:
_tmpDB=((struct Cyc_Absyn_Struct_e_struct*)_tmp8D)->f1;goto _LL1A4;_LL1A4: _tmpDA=((
struct Cyc_Absyn_Struct_e_struct*)_tmp8D)->f3;goto _LL145;}else{goto _LL146;}_LL146:
if(*((int*)_tmp8D)== 29){_LL1A6: _tmpDC=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp8D)->f2;goto _LL147;}else{goto _LL148;}_LL148: if(*((int*)_tmp8D)== 30){_LL1A8:
_tmpDE=((struct Cyc_Absyn_Tunion_e_struct*)_tmp8D)->f3;goto _LL1A7;_LL1A7: _tmpDD=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp8D)->f5;goto _LL149;}else{goto _LL14A;}_LL14A:
if(*((int*)_tmp8D)== 31){_LL1A9: _tmpDF=((struct Cyc_Absyn_Enum_e_struct*)_tmp8D)->f1;
goto _LL14B;}else{goto _LL14C;}_LL14C: if(*((int*)_tmp8D)== 32){_LL1AA: _tmpE0=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp8D)->f1;goto _LL14D;}else{goto _LL14E;}
_LL14E: if(*((int*)_tmp8D)== 33){_LL1AB: _tmpE1=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp8D)->f1;_LL1AF: _tmpE5=_tmpE1.is_calloc;goto _LL1AE;_LL1AE: _tmpE4=_tmpE1.rgn;
goto _LL1AD;_LL1AD: _tmpE3=_tmpE1.elt_type;goto _LL1AC;_LL1AC: _tmpE2=_tmpE1.num_elts;
goto _LL14F;}else{goto _LL150;}_LL150: if(*((int*)_tmp8D)== 34){_LL1B1: _tmpE7=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp8D)->f1;goto _LL1B0;_LL1B0: _tmpE6=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp8D)->f2;goto _LL151;}else{goto _LL152;}
_LL152: if(*((int*)_tmp8D)== 35){_LL1B2: _tmpE8=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp8D)->f1;goto _LL153;}else{goto _LL154;}_LL154: if(*((int*)_tmp8D)== 36){_LL1B3:
_tmpE9=((struct Cyc_Absyn_Codegen_e_struct*)_tmp8D)->f1;goto _LL155;}else{goto
_LL156;}_LL156: if(*((int*)_tmp8D)== 37){_LL1B4: _tmpEA=((struct Cyc_Absyn_Fill_e_struct*)
_tmp8D)->f1;goto _LL157;}else{goto _LLF5;}_LLF7: Cyc_Dump_dump_char((int)'\'');Cyc_Dump_dump_alpha(
Cyc_Absynpp_char_escape(_tmp8F));Cyc_Dump_dump_char((int)'\'');goto _LLF5;_LLF9:
Cyc_Dump_dump_alpha((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpEC;
_tmpEC.tag=1;_tmpEC.f1=(int)((unsigned int)((int)_tmp92));{void*_tmpEB[1]={&
_tmpEC};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmpEB,
sizeof(void*),1));}}));goto _LLF5;_LLFB: Cyc_Dump_dump_alpha((struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmpEE;_tmpEE.tag=1;_tmpEE.f1=(int)((unsigned int)
_tmp95);{void*_tmpED[1]={& _tmpEE};Cyc_Std_aprintf(_tag_arr("%d",sizeof(
unsigned char),3),_tag_arr(_tmpED,sizeof(void*),1));}}));goto _LLF5;_LLFD: Cyc_Dump_dump_alpha((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpF0;_tmpF0.tag=1;_tmpF0.f1=(
int)((unsigned int)_tmp98);{void*_tmpEF[1]={& _tmpF0};Cyc_Std_aprintf(_tag_arr("%du",
sizeof(unsigned char),4),_tag_arr(_tmpEF,sizeof(void*),1));}}));goto _LLF5;_LLFF:
Cyc_Dump_dump_alpha(_tag_arr("<<FIX LONG LONG CONSTANT>>",sizeof(unsigned char),
27));goto _LLF5;_LL101: Cyc_Dump_dump_alpha(_tmp9E);goto _LLF5;_LL103: Cyc_Dump_dump_alpha(
_tag_arr("NULL",sizeof(unsigned char),5));goto _LLF5;_LL105: Cyc_Dump_dump_char((
int)'"');Cyc_Dump_dump_alpha(Cyc_Absynpp_string_escape(_tmpA1));Cyc_Dump_dump_char((
int)'"');goto _LLF5;_LL107: _tmpA3=_tmpA2;goto _LL109;_LL109: Cyc_Dump_dumpqvar(
_tmpA3);goto _LLF5;_LL10B: {struct _tagged_arr _tmpF1=Cyc_Absynpp_prim2str(_tmpA5);
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA4)){case 1: _LL1B5:
if(_tmpA5 == (void*)19){Cyc_Dump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpA4))->hd);Cyc_Dump_dump_char((int)'.');Cyc_Dump_dump_alpha(
_tag_arr("size",sizeof(unsigned char),5));}else{Cyc_Dump_dump_symbol(_tmpF1);Cyc_Dump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpA4))->hd);}
break;case 2: _LL1B6: Cyc_Dump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmpA4))->hd);Cyc_Dump_dump_symbol(_tmpF1);Cyc_Dump_dump_char((
int)' ');Cyc_Dump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpA4->tl))->hd);break;default: _LL1B7:(int)_throw((void*)({struct
Cyc_Core_Failure_struct*_tmpF2=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmpF2[0]=({struct Cyc_Core_Failure_struct _tmpF3;_tmpF3.tag=Cyc_Core_Failure;
_tmpF3.f1=_tag_arr("Absyndump -- Bad number of arguments to primop",sizeof(
unsigned char),47);_tmpF3;});_tmpF2;}));}goto _LLF5;}_LL10D: Cyc_Dump_dumpexp_prec(
myprec,_tmpA8);if(_tmpA7 != 0){Cyc_Dump_dump_symbol(Cyc_Absynpp_prim2str((void*)
_tmpA7->v));}Cyc_Dump_dump_symbol(_tag_arr("=",sizeof(unsigned char),2));Cyc_Dump_dumpexp_prec(
myprec,_tmpA6);goto _LLF5;_LL10F: Cyc_Dump_dump_symbol(_tag_arr("++",sizeof(
unsigned char),3));Cyc_Dump_dumpexp_prec(myprec,_tmpAA);goto _LLF5;_LL111: Cyc_Dump_dump_symbol(
_tag_arr("--",sizeof(unsigned char),3));Cyc_Dump_dumpexp_prec(myprec,_tmpAC);
goto _LLF5;_LL113: Cyc_Dump_dumpexp_prec(myprec,_tmpAE);Cyc_Dump_dump_symbol(
_tag_arr("++",sizeof(unsigned char),3));goto _LLF5;_LL115: Cyc_Dump_dumpexp_prec(
myprec,_tmpB0);Cyc_Dump_dump_symbol(_tag_arr("--",sizeof(unsigned char),3));goto
_LLF5;_LL117: Cyc_Dump_dumpexp_prec(myprec,_tmpB3);Cyc_Dump_dump_char((int)'?');
Cyc_Dump_dumpexp_prec(0,_tmpB2);Cyc_Dump_dump_char((int)':');Cyc_Dump_dumpexp_prec(
myprec,_tmpB1);goto _LLF5;_LL119: Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumpexp_prec(
myprec,_tmpB5);Cyc_Dump_dump_char((int)',');Cyc_Dump_dumpexp_prec(myprec,_tmpB4);
Cyc_Dump_dump_char((int)')');goto _LLF5;_LL11B: _tmpB9=_tmpB7;_tmpB8=_tmpB6;goto
_LL11D;_LL11D: Cyc_Dump_dumpexp_prec(myprec,_tmpB9);Cyc_Dump_dump_char((int)'(');
Cyc_Dump_dumpexps_prec(20,_tmpB8);Cyc_Dump_dump_char((int)')');goto _LLF5;_LL11F:
Cyc_Dump_dump_alpha(_tag_arr("throw",sizeof(unsigned char),6));Cyc_Dump_dumpexp_prec(
myprec,_tmpBA);goto _LLF5;_LL121: _tmpBC=_tmpBB;goto _LL123;_LL123: Cyc_Dump_dumpexp_prec(
inprec,_tmpBC);goto _LLF5;_LL125: Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(
_tmpBE);Cyc_Dump_dump_char((int)')');Cyc_Dump_dumpexp_prec(myprec,_tmpBD);goto
_LLF5;_LL127: Cyc_Dump_dump_char((int)'&');Cyc_Dump_dumpexp_prec(myprec,_tmpBF);
goto _LLF5;_LL129: Cyc_Dump_dump_alpha(_tag_arr("new",sizeof(unsigned char),4));
Cyc_Dump_dumpexp_prec(myprec,_tmpC0);goto _LLF5;_LL12B: Cyc_Dump_dump_alpha(
_tag_arr("sizeof",sizeof(unsigned char),7));Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(
_tmpC2);Cyc_Dump_dump_char((int)')');goto _LLF5;_LL12D: Cyc_Dump_dump_alpha(
_tag_arr("sizeof",sizeof(unsigned char),7));Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumpexp_prec(
myprec,_tmpC3);Cyc_Dump_dump_char((int)')');goto _LLF5;_LL12F: Cyc_Dump_dump_alpha(
_tag_arr("offsetof",sizeof(unsigned char),9));Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(
_tmpC6);Cyc_Dump_dump_char((int)',');Cyc_Dump_dump_alpha(*_tmpC5);Cyc_Dump_dump_char((
int)')');goto _LLF5;_LL131: Cyc_Dump_dump_alpha(_tag_arr("offsetof(",sizeof(
unsigned char),10));Cyc_Dump_dumptyp(_tmpC9);Cyc_Dump_dump_char((int)',');Cyc_Dump_dump_alpha((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpF5;_tmpF5.tag=1;_tmpF5.f1=(
int)_tmpC8;{void*_tmpF4[1]={& _tmpF5};Cyc_Std_aprintf(_tag_arr("%d",sizeof(
unsigned char),3),_tag_arr(_tmpF4,sizeof(void*),1));}}));Cyc_Dump_dump_char((int)')');
goto _LLF5;_LL133: Cyc_Dump_dump_alpha(_tag_arr("__gen",sizeof(unsigned char),6));
Cyc_Dump_dumptvars(_tmpCB);Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(_tmpCA);
Cyc_Dump_dump_char((int)')');goto _LLF5;_LL135: Cyc_Dump_dump_char((int)'*');Cyc_Dump_dumpexp_prec(
myprec,_tmpCC);goto _LLF5;_LL137: Cyc_Dump_dumpexp_prec(myprec,_tmpCE);Cyc_Dump_dump_char((
int)'.');Cyc_Dump_dump_alpha(*_tmpCD);goto _LLF5;_LL139: Cyc_Dump_dumpexp_prec(
myprec,_tmpD0);Cyc_Dump_dump_symbol(_tag_arr("->",sizeof(unsigned char),3));Cyc_Dump_dump_alpha(*
_tmpCF);goto _LLF5;_LL13B: Cyc_Dump_dumpexp_prec(myprec,_tmpD2);Cyc_Dump_dump_char((
int)'[');Cyc_Dump_dumpexp(_tmpD1);Cyc_Dump_dump_char((int)']');goto _LLF5;_LL13D:
Cyc_Dump_dump_symbol(_tag_arr("\\$(",sizeof(unsigned char),4));Cyc_Dump_dumpexps_prec(
20,_tmpD3);Cyc_Dump_dump_char((int)')');goto _LLF5;_LL13F: Cyc_Dump_dump_char((int)'(');
Cyc_Dump_dumptyp((*_tmpD5).f3);Cyc_Dump_dump_char((int)')');((void(*)(void(*f)(
struct _tuple13*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr
end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmpD4,_tag_arr("{\\lb}",
sizeof(unsigned char),6),_tag_arr("{\\rb}",sizeof(unsigned char),6),_tag_arr(",",
sizeof(unsigned char),2));goto _LLF5;_LL141:((void(*)(void(*f)(struct _tuple13*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmpD6,_tag_arr("{\\lb}",sizeof(
unsigned char),6),_tag_arr("{\\rb}",sizeof(unsigned char),6),_tag_arr(",",
sizeof(unsigned char),2));goto _LLF5;_LL143: Cyc_Dump_dump_alpha(_tag_arr("new",
sizeof(unsigned char),4));Cyc_Dump_dump_char((int)'{');Cyc_Dump_dump_alpha(
_tag_arr("for",sizeof(unsigned char),4));Cyc_Dump_dump_alphastr((*_tmpD9->name).f2);
Cyc_Dump_dump_char((int)'<');Cyc_Dump_dumpexp(_tmpD8);Cyc_Dump_dump_char((int)':');
Cyc_Dump_dumpexp(_tmpD7);Cyc_Dump_dump_char((int)'}');goto _LLF5;_LL145: Cyc_Dump_dumpqvar(
_tmpDB);((void(*)(void(*f)(struct _tuple13*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,
_tmpDA,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LLF5;_LL147:((void(*)(void(*f)(
struct _tuple13*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr
end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmpDC,_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LLF5;_LL149: Cyc_Dump_dumpqvar(_tmpDD->name);if(
_tmpDE != 0){((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(
Cyc_Dump_dumpexp,_tmpDE,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}goto _LLF5;
_LL14B: Cyc_Dump_dumpqvar(_tmpDF);goto _LLF5;_LL14D: Cyc_Dump_dumpqvar(_tmpE0);goto
_LLF5;_LL14F: if(_tmpE4 != 0){if(_tmpE5){Cyc_Dump_dump_alpha(_tag_arr("rcalloc",
sizeof(unsigned char),8));}else{Cyc_Dump_dump_alpha(_tag_arr("rmalloc",sizeof(
unsigned char),8));}Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmpE4));Cyc_Dump_dump_symbol(_tag_arr(", ",sizeof(unsigned char),3));}
else{if(_tmpE5){Cyc_Dump_dump_alpha(_tag_arr("calloc",sizeof(unsigned char),7));}
else{Cyc_Dump_dump_alpha(_tag_arr("malloc",sizeof(unsigned char),7));}Cyc_Dump_dump_char((
int)'(');}Cyc_Dump_dumpexp(_tmpE2);if(_tmpE3 != 0){if(_tmpE5){Cyc_Dump_dump_symbol(
_tag_arr(", ",sizeof(unsigned char),3));}else{Cyc_Dump_dump_symbol(_tag_arr(" * ",
sizeof(unsigned char),4));}Cyc_Dump_dump_alpha(_tag_arr("sizeof",sizeof(
unsigned char),7));Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(*_tmpE3);Cyc_Dump_dump_symbol(
_tag_arr(")",sizeof(unsigned char),2));}Cyc_Dump_dump_symbol(_tag_arr(")",
sizeof(unsigned char),2));goto _LLF5;_LL151:((void(*)(void(*f)(struct _tuple13*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmpE6,_tag_arr("{",sizeof(
unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2));goto _LLF5;_LL153: goto _LLF5;_LL155: goto _LLF5;_LL157: goto _LLF5;
_LLF5:;}if(inprec >= myprec){Cyc_Dump_dump_char((int)')');}}void Cyc_Dump_dumpexp(
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
void Cyc_Dump_dumpstructfields(struct Cyc_List_List*fields){for(0;fields != 0;
fields=fields->tl){Cyc_Dump_dump_symbol(_tag_arr("~~",sizeof(unsigned char),3));{
struct Cyc_Absyn_Structfield _tmpF7;struct Cyc_List_List*_tmpF8;struct Cyc_Absyn_Exp*
_tmpF9;void*_tmpFA;struct Cyc_Absyn_Tqual _tmpFB;struct _tagged_arr*_tmpFC;struct
Cyc_Absyn_Structfield*_tmpF6=(struct Cyc_Absyn_Structfield*)fields->hd;_tmpF7=*
_tmpF6;_LL1BE: _tmpFC=_tmpF7.name;goto _LL1BD;_LL1BD: _tmpFB=_tmpF7.tq;goto _LL1BC;
_LL1BC: _tmpFA=(void*)_tmpF7.type;goto _LL1BB;_LL1BB: _tmpF9=_tmpF7.width;goto
_LL1BA;_LL1BA: _tmpF8=_tmpF7.attributes;goto _LL1B9;_LL1B9:((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _tagged_arr*),struct _tagged_arr*))Cyc_Dump_dumptqtd)(_tmpFB,
_tmpFA,Cyc_Dump_dump_alphastr,_tmpFC);if(_tmpF9 != 0){Cyc_Dump_dump_char((int)':');
Cyc_Dump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmpF9));}Cyc_Dump_dump_symbol(
_tag_arr(";\\\\\n",sizeof(unsigned char),5));}}}void Cyc_Dump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Dump_dumpqvar(td->name);Cyc_Dump_dumptvars(
td->tvs);}void Cyc_Dump_dump_boldtypedefname(struct Cyc_Absyn_Typedefdecl*td){Cyc_Dump_dump_boldqvar(
td->name);Cyc_Dump_dumptvars(td->tvs);}static void Cyc_Dump_dumpids(struct Cyc_List_List*
vds){for(0;vds != 0;vds=vds->tl){Cyc_Dump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
if(vds->tl != 0){Cyc_Dump_dump_char((int)',');}}}static void Cyc_Dump_dump_upperbound(
struct Cyc_Absyn_Exp*e){unsigned int i=Cyc_Evexp_eval_const_uint_exp(e);if(i != 1){
Cyc_Dump_dump_char((int)'{');Cyc_Dump_dumpexp(e);Cyc_Dump_dump_char((int)'}');}}
void Cyc_Dump_dumptms(struct Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){
f(a);return;}{void*_tmpFD=(void*)tms->hd;struct Cyc_Absyn_Tqual _tmpFE;void*_tmpFF;
void*_tmp100;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Tqual _tmp102;void*
_tmp103;void*_tmp104;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Tqual _tmp106;
void*_tmp107;void*_tmp108;struct Cyc_Absyn_Tqual _tmp109;void*_tmp10A;struct Cyc_Absyn_Tvar*
_tmp10B;void*_tmp10C;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Tqual _tmp10E;
void*_tmp10F;struct Cyc_Absyn_Tvar*_tmp110;void*_tmp111;struct Cyc_Absyn_Exp*
_tmp112;struct Cyc_Absyn_Tqual _tmp113;void*_tmp114;struct Cyc_Absyn_Tvar*_tmp115;
void*_tmp116;struct Cyc_Absyn_Tqual _tmp117;void*_tmp118;struct Cyc_Absyn_Exp*
_tmp119;struct Cyc_Absyn_Tqual _tmp11A;void*_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;
struct Cyc_Absyn_Tqual _tmp11D;void*_tmp11E;_LL1C0: if((unsigned int)_tmpFD > 1?*((
int*)_tmpFD)== 1: 0){_LL1D6: _tmp100=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFD)->f1;if((unsigned int)_tmp100 > 1?*((int*)_tmp100)== 1: 0){_LL1D7: _tmp101=((
struct Cyc_Absyn_Nullable_ps_struct*)_tmp100)->f1;goto _LL1D5;}else{goto _LL1C2;}
_LL1D5: _tmpFF=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFD)->f2;if((int)
_tmpFF == 2){goto _LL1D4;}else{goto _LL1C2;}_LL1D4: _tmpFE=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFD)->f3;goto _LL1C1;}else{goto _LL1C2;}_LL1C2: if((unsigned int)_tmpFD > 1?*((
int*)_tmpFD)== 1: 0){_LL1DA: _tmp104=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFD)->f1;if((unsigned int)_tmp104 > 1?*((int*)_tmp104)== 0: 0){_LL1DB: _tmp105=((
struct Cyc_Absyn_NonNullable_ps_struct*)_tmp104)->f1;goto _LL1D9;}else{goto _LL1C4;}
_LL1D9: _tmp103=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFD)->f2;if((int)
_tmp103 == 2){goto _LL1D8;}else{goto _LL1C4;}_LL1D8: _tmp102=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFD)->f3;goto _LL1C3;}else{goto _LL1C4;}_LL1C4: if((unsigned int)_tmpFD > 1?*((
int*)_tmpFD)== 1: 0){_LL1DE: _tmp108=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFD)->f1;if((int)_tmp108 == 0){goto _LL1DD;}else{goto _LL1C6;}_LL1DD: _tmp107=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFD)->f2;if((int)_tmp107 == 2){
goto _LL1DC;}else{goto _LL1C6;}_LL1DC: _tmp106=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFD)->f3;goto _LL1C5;}else{goto _LL1C6;}_LL1C6: if((unsigned int)_tmpFD > 1?*((
int*)_tmpFD)== 1: 0){_LL1E2: _tmp10C=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFD)->f1;if((unsigned int)_tmp10C > 1?*((int*)_tmp10C)== 1: 0){_LL1E3: _tmp10D=((
struct Cyc_Absyn_Nullable_ps_struct*)_tmp10C)->f1;goto _LL1E0;}else{goto _LL1C8;}
_LL1E0: _tmp10A=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFD)->f2;if((
unsigned int)_tmp10A > 3?*((int*)_tmp10A)== 1: 0){_LL1E1: _tmp10B=((struct Cyc_Absyn_VarType_struct*)
_tmp10A)->f1;goto _LL1DF;}else{goto _LL1C8;}_LL1DF: _tmp109=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFD)->f3;goto _LL1C7;}else{goto _LL1C8;}_LL1C8: if((unsigned int)_tmpFD > 1?*((
int*)_tmpFD)== 1: 0){_LL1E7: _tmp111=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFD)->f1;if((unsigned int)_tmp111 > 1?*((int*)_tmp111)== 0: 0){_LL1E8: _tmp112=((
struct Cyc_Absyn_NonNullable_ps_struct*)_tmp111)->f1;goto _LL1E5;}else{goto _LL1CA;}
_LL1E5: _tmp10F=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFD)->f2;if((
unsigned int)_tmp10F > 3?*((int*)_tmp10F)== 1: 0){_LL1E6: _tmp110=((struct Cyc_Absyn_VarType_struct*)
_tmp10F)->f1;goto _LL1E4;}else{goto _LL1CA;}_LL1E4: _tmp10E=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFD)->f3;goto _LL1C9;}else{goto _LL1CA;}_LL1CA: if((unsigned int)_tmpFD > 1?*((
int*)_tmpFD)== 1: 0){_LL1EC: _tmp116=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFD)->f1;if((int)_tmp116 == 0){goto _LL1EA;}else{goto _LL1CC;}_LL1EA: _tmp114=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFD)->f2;if((unsigned int)_tmp114
> 3?*((int*)_tmp114)== 1: 0){_LL1EB: _tmp115=((struct Cyc_Absyn_VarType_struct*)
_tmp114)->f1;goto _LL1E9;}else{goto _LL1CC;}_LL1E9: _tmp113=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFD)->f3;goto _LL1CB;}else{goto _LL1CC;}_LL1CC: if((unsigned int)_tmpFD > 1?*((
int*)_tmpFD)== 1: 0){_LL1EE: _tmp118=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFD)->f1;if((unsigned int)_tmp118 > 1?*((int*)_tmp118)== 1: 0){_LL1EF: _tmp119=((
struct Cyc_Absyn_Nullable_ps_struct*)_tmp118)->f1;goto _LL1ED;}else{goto _LL1CE;}
_LL1ED: _tmp117=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFD)->f3;goto _LL1CD;}
else{goto _LL1CE;}_LL1CE: if((unsigned int)_tmpFD > 1?*((int*)_tmpFD)== 1: 0){_LL1F1:
_tmp11B=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpFD)->f1;if((
unsigned int)_tmp11B > 1?*((int*)_tmp11B)== 0: 0){_LL1F2: _tmp11C=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmp11B)->f1;goto _LL1F0;}else{goto _LL1D0;}_LL1F0: _tmp11A=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFD)->f3;goto _LL1CF;}else{goto _LL1D0;}_LL1D0: if((unsigned int)_tmpFD > 1?*((
int*)_tmpFD)== 1: 0){_LL1F4: _tmp11E=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpFD)->f1;if((int)_tmp11E == 0){goto _LL1F3;}else{goto _LL1D2;}_LL1F3: _tmp11D=((
struct Cyc_Absyn_Pointer_mod_struct*)_tmpFD)->f3;goto _LL1D1;}else{goto _LL1D2;}
_LL1D2: goto _LL1D3;_LL1C1: Cyc_Dump_dump_pointer(_tag_arr("*",sizeof(unsigned char),
2));Cyc_Dump_dump_upperbound(_tmp101);Cyc_Dump_dump_alpha(_tag_arr("`H",sizeof(
unsigned char),3));Cyc_Dump_dumptms(tms->tl,f,a);return;_LL1C3: Cyc_Dump_dump_pointer(
_tag_arr("@",sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(_tmp105);Cyc_Dump_dump_alpha(
_tag_arr("`H",sizeof(unsigned char),3));Cyc_Dump_dumptms(tms->tl,f,a);return;
_LL1C5: Cyc_Dump_dump_pointer(_tag_arr("{?}",sizeof(unsigned char),4));Cyc_Dump_dump_alpha(
_tag_arr("`H",sizeof(unsigned char),3));Cyc_Dump_dumptms(tms->tl,f,a);return;
_LL1C7: Cyc_Dump_dump_pointer(_tag_arr("*",sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(
_tmp10D);Cyc_Dump_dump_alphastr(_tmp10B->name);Cyc_Dump_dumptms(tms->tl,f,a);
return;_LL1C9: Cyc_Dump_dump_pointer(_tag_arr("@",sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(
_tmp112);Cyc_Dump_dump_alphastr(_tmp110->name);Cyc_Dump_dumptms(tms->tl,f,a);
return;_LL1CB: Cyc_Dump_dump_pointer(_tag_arr("{?}",sizeof(unsigned char),4));Cyc_Dump_dump_alphastr(
_tmp115->name);Cyc_Dump_dumptms(tms->tl,f,a);return;_LL1CD: Cyc_Dump_dump_pointer(
_tag_arr("*",sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(_tmp119);Cyc_Dump_dumptms(
tms->tl,f,a);return;_LL1CF: Cyc_Dump_dump_pointer(_tag_arr("@",sizeof(
unsigned char),2));Cyc_Dump_dump_upperbound(_tmp11C);Cyc_Dump_dumptms(tms->tl,f,
a);return;_LL1D1: Cyc_Dump_dump_pointer(_tag_arr("{?}",sizeof(unsigned char),4));
Cyc_Dump_dumptms(tms->tl,f,a);return;_LL1D3: {int next_is_pointer=0;if(tms->tl != 
0){void*_tmp11F=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_LL1F6:
if((unsigned int)_tmp11F > 1?*((int*)_tmp11F)== 1: 0){goto _LL1F7;}else{goto _LL1F8;}
_LL1F8: goto _LL1F9;_LL1F7: next_is_pointer=1;goto _LL1F5;_LL1F9: goto _LL1F5;_LL1F5:;}
if(next_is_pointer){Cyc_Dump_dump_char((int)'(');}Cyc_Dump_dumptms(tms->tl,f,a);
if(next_is_pointer){Cyc_Dump_dump_char((int)')');}{void*_tmp120=(void*)tms->hd;
struct Cyc_Absyn_Exp*_tmp121;void*_tmp122;struct Cyc_List_List*_tmp123;struct Cyc_Core_Opt*
_tmp124;struct Cyc_Absyn_VarargInfo*_tmp125;int _tmp126;struct Cyc_List_List*
_tmp127;void*_tmp128;struct Cyc_Position_Segment*_tmp129;struct Cyc_List_List*
_tmp12A;int _tmp12B;struct Cyc_Position_Segment*_tmp12C;struct Cyc_List_List*
_tmp12D;void*_tmp12E;void*_tmp12F;_LL1FB: if((int)_tmp120 == 0){goto _LL1FC;}else{
goto _LL1FD;}_LL1FD: if((unsigned int)_tmp120 > 1?*((int*)_tmp120)== 0: 0){_LL209:
_tmp121=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmp120)->f1;goto _LL1FE;}else{
goto _LL1FF;}_LL1FF: if((unsigned int)_tmp120 > 1?*((int*)_tmp120)== 2: 0){_LL20A:
_tmp122=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp120)->f1;if(*((int*)
_tmp122)== 1){_LL20F: _tmp127=((struct Cyc_Absyn_WithTypes_struct*)_tmp122)->f1;
goto _LL20E;_LL20E: _tmp126=((struct Cyc_Absyn_WithTypes_struct*)_tmp122)->f2;goto
_LL20D;_LL20D: _tmp125=((struct Cyc_Absyn_WithTypes_struct*)_tmp122)->f3;goto
_LL20C;_LL20C: _tmp124=((struct Cyc_Absyn_WithTypes_struct*)_tmp122)->f4;goto
_LL20B;_LL20B: _tmp123=((struct Cyc_Absyn_WithTypes_struct*)_tmp122)->f5;goto
_LL200;}else{goto _LL201;}}else{goto _LL201;}_LL201: if((unsigned int)_tmp120 > 1?*((
int*)_tmp120)== 2: 0){_LL210: _tmp128=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp120)->f1;if(*((int*)_tmp128)== 0){_LL212: _tmp12A=((struct Cyc_Absyn_NoTypes_struct*)
_tmp128)->f1;goto _LL211;_LL211: _tmp129=((struct Cyc_Absyn_NoTypes_struct*)_tmp128)->f2;
goto _LL202;}else{goto _LL203;}}else{goto _LL203;}_LL203: if((unsigned int)_tmp120 > 
1?*((int*)_tmp120)== 3: 0){_LL215: _tmp12D=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp120)->f1;goto _LL214;_LL214: _tmp12C=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp120)->f2;goto _LL213;_LL213: _tmp12B=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp120)->f3;goto _LL204;}else{goto _LL205;}_LL205: if((unsigned int)_tmp120 > 1?*((
int*)_tmp120)== 4: 0){goto _LL206;}else{goto _LL207;}_LL207: if((unsigned int)
_tmp120 > 1?*((int*)_tmp120)== 1: 0){_LL217: _tmp12F=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp120)->f1;goto _LL216;_LL216: _tmp12E=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp120)->f2;goto _LL208;}else{goto _LL1FA;}_LL1FC: Cyc_Dump_dump_symbol(_tag_arr("[]",
sizeof(unsigned char),3));goto _LL1FA;_LL1FE: Cyc_Dump_dump_char((int)'[');Cyc_Dump_dumpexp(
_tmp121);Cyc_Dump_dump_char((int)']');goto _LL1FA;_LL200: Cyc_Dump_dumpfunargs(
_tmp127,_tmp126,_tmp125,_tmp124,_tmp123);goto _LL1FA;_LL202:((void(*)(void(*f)(
struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dump_alphastr,
_tmp12A,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),
2),_tag_arr(", ",sizeof(unsigned char),3));goto _LL1FA;_LL204: if(_tmp12B){Cyc_Dump_dumpkindedtvars(
_tmp12D);}else{Cyc_Dump_dumptvars(_tmp12D);}goto _LL1FA;_LL206: goto _LL1FA;_LL208:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp130=_cycalloc(sizeof(
struct Cyc_Core_Impossible_struct));_tmp130[0]=({struct Cyc_Core_Impossible_struct
_tmp131;_tmp131.tag=Cyc_Core_Impossible;_tmp131.f1=_tag_arr("dumptms",sizeof(
unsigned char),8);_tmp131;});_tmp130;}));_LL1FA:;}return;}_LL1BF:;}}void Cyc_Dump_dumptqtd(
struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){struct Cyc_List_List*
_tmp133;void*_tmp134;struct Cyc_Absyn_Tqual _tmp135;struct _tuple9 _tmp132=Cyc_Absynpp_to_tms(
tq,t);_LL21B: _tmp135=_tmp132.f1;goto _LL21A;_LL21A: _tmp134=_tmp132.f2;goto _LL219;
_LL219: _tmp133=_tmp132.f3;goto _LL218;_LL218: Cyc_Dump_dumptq(_tmp135);Cyc_Dump_dumpntyp(
_tmp134);Cyc_Dump_dumptms(Cyc_List_imp_rev(_tmp133),f,a);}void Cyc_Dump_pr_comment(
struct Cyc_Std___sFILE*outf,struct _tagged_arr s){int depth=0;int len=(int)Cyc_Std_strlen(
s);int i=0;for(0;i < len;i ++){unsigned char c=((const unsigned char*)s.curr)[i];if(c
!= '['){Cyc_Std_fputc((int)c,outf);continue;}Cyc_Std_fputs(_tag_arr("\\texttt{",
sizeof(unsigned char),9),outf);i ++;depth ++;for(0;i < len;i ++){unsigned char c=((
const unsigned char*)s.curr)[i];if(c == ']'){depth --;if(depth == 0){Cyc_Std_fputc((
int)'}',outf);break;}}else{if(c == '['){depth ++;}}Cyc_Std_fputc((int)c,outf);}}}
void Cyc_Dump_dump_comment(struct _tagged_arr s){Cyc_Dump_pr_comment(Cyc_Std_stdout,
s);}void Cyc_Dump_dumpdecl(struct Cyc_Absyn_Decl*d,struct _tagged_arr comment){void*
_tmp136=(void*)d->r;struct Cyc_Absyn_Structdecl*_tmp137;struct Cyc_Absyn_Uniondecl*
_tmp138;struct Cyc_Absyn_Vardecl*_tmp139;struct Cyc_Absyn_Vardecl _tmp13A;void*
_tmp13B;struct Cyc_Absyn_Tqual _tmp13C;struct _tuple1*_tmp13D;struct Cyc_Absyn_Tuniondecl*
_tmp13E;struct Cyc_Absyn_Tuniondecl _tmp13F;int _tmp140;struct Cyc_Core_Opt*_tmp141;
struct Cyc_List_List*_tmp142;struct _tuple1*_tmp143;void*_tmp144;struct Cyc_Absyn_Enumdecl*
_tmp145;struct Cyc_Absyn_Enumdecl _tmp146;struct Cyc_Core_Opt*_tmp147;struct _tuple1*
_tmp148;void*_tmp149;struct Cyc_Absyn_Typedefdecl*_tmp14A;_LL21D: if(*((int*)
_tmp136)== 4){_LL235: _tmp137=((struct Cyc_Absyn_Struct_d_struct*)_tmp136)->f1;
goto _LL21E;}else{goto _LL21F;}_LL21F: if(*((int*)_tmp136)== 5){_LL236: _tmp138=((
struct Cyc_Absyn_Union_d_struct*)_tmp136)->f1;goto _LL220;}else{goto _LL221;}_LL221:
if(*((int*)_tmp136)== 0){_LL237: _tmp139=((struct Cyc_Absyn_Var_d_struct*)_tmp136)->f1;
_tmp13A=*_tmp139;_LL23A: _tmp13D=_tmp13A.name;goto _LL239;_LL239: _tmp13C=_tmp13A.tq;
goto _LL238;_LL238: _tmp13B=(void*)_tmp13A.type;goto _LL222;}else{goto _LL223;}
_LL223: if(*((int*)_tmp136)== 6){_LL23B: _tmp13E=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp136)->f1;_tmp13F=*_tmp13E;_LL240: _tmp144=(void*)_tmp13F.sc;goto _LL23F;_LL23F:
_tmp143=_tmp13F.name;goto _LL23E;_LL23E: _tmp142=_tmp13F.tvs;goto _LL23D;_LL23D:
_tmp141=_tmp13F.fields;goto _LL23C;_LL23C: _tmp140=_tmp13F.is_xtunion;goto _LL224;}
else{goto _LL225;}_LL225: if(*((int*)_tmp136)== 7){_LL241: _tmp145=((struct Cyc_Absyn_Enum_d_struct*)
_tmp136)->f1;_tmp146=*_tmp145;_LL244: _tmp149=(void*)_tmp146.sc;goto _LL243;_LL243:
_tmp148=_tmp146.name;goto _LL242;_LL242: _tmp147=_tmp146.fields;goto _LL226;}else{
goto _LL227;}_LL227: if(*((int*)_tmp136)== 8){_LL245: _tmp14A=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp136)->f1;goto _LL228;}else{goto _LL229;}_LL229: if(*((int*)_tmp136)== 1){goto
_LL22A;}else{goto _LL22B;}_LL22B: if(*((int*)_tmp136)== 2){goto _LL22C;}else{goto
_LL22D;}_LL22D: if(*((int*)_tmp136)== 3){goto _LL22E;}else{goto _LL22F;}_LL22F: if(*((
int*)_tmp136)== 9){goto _LL230;}else{goto _LL231;}_LL231: if(*((int*)_tmp136)== 10){
goto _LL232;}else{goto _LL233;}_LL233: if(*((int*)_tmp136)== 11){goto _LL234;}else{
goto _LL21C;}_LL21E: Cyc_Dump_dump_begin();Cyc_Dump_dump_alpha(_tag_arr("struct",
sizeof(unsigned char),7));if(_tmp137->name != 0){Cyc_Dump_dump_boldqvar((struct
_tuple1*)((struct Cyc_Core_Opt*)_check_null(_tmp137->name))->v);}Cyc_Dump_dumptvars(
_tmp137->tvs);if(_tmp137->fields == 0){Cyc_Dump_dump_symbol(_tag_arr(";\n",
sizeof(unsigned char),3));}else{Cyc_Dump_dump_symbol(_tag_arr(" ",sizeof(
unsigned char),2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",
sizeof(unsigned char),4));Cyc_Dump_dumpstructfields((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp137->fields))->v);Cyc_Dump_dump_rb();Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));}Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(
comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(unsigned char),2));Cyc_Dump_dump_end();
goto _LL21C;_LL220: Cyc_Dump_dump_begin();Cyc_Dump_dump_alpha(_tag_arr("union",
sizeof(unsigned char),6));if(_tmp138->name != 0){Cyc_Dump_dump_boldqvar((struct
_tuple1*)((struct Cyc_Core_Opt*)_check_null(_tmp138->name))->v);}Cyc_Dump_dumptvars(
_tmp138->tvs);if(_tmp138->fields == 0){Cyc_Dump_dump_symbol(_tag_arr(";\n",
sizeof(unsigned char),3));}else{Cyc_Dump_dump_symbol(_tag_arr(" ",sizeof(
unsigned char),2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",
sizeof(unsigned char),4));Cyc_Dump_dumpstructfields((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp138->fields))->v);Cyc_Dump_dump_rb();Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));}Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(
comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(unsigned char),2));Cyc_Dump_dump_end();
goto _LL21C;_LL222: Cyc_Dump_dump_begin();((void(*)(struct Cyc_Absyn_Tqual tq,void*t,
void(*f)(struct _tuple1*),struct _tuple1*a))Cyc_Dump_dumptqtd)(_tmp13C,_tmp13B,Cyc_Dump_dump_boldqvar,
_tmp13D);Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(unsigned char),3));Cyc_Dump_dump_middle();
Cyc_Dump_dump_comment(comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(
unsigned char),2));Cyc_Dump_dump_end();goto _LL21C;_LL224: Cyc_Dump_dump_begin();
if(_tmp140){Cyc_Dump_dump_alpha(_tag_arr("xtunion",sizeof(unsigned char),8));}
else{Cyc_Dump_dump_alpha(_tag_arr("tunion",sizeof(unsigned char),7));}Cyc_Dump_dump_boldqvar(
_tmp143);Cyc_Dump_dumptvars(_tmp142);if(_tmp141 == 0){Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));}else{Cyc_Dump_dump_symbol(_tag_arr(" ",
sizeof(unsigned char),2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(_tag_arr("\\\\\n~~",
sizeof(unsigned char),6));Cyc_Dump_dumptunionfields((struct Cyc_List_List*)
_tmp141->v);Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",sizeof(unsigned char),4));Cyc_Dump_dump_rb();
Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(unsigned char),3));}Cyc_Dump_dump_middle();
Cyc_Dump_dump_comment(comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(
unsigned char),2));Cyc_Dump_dump_end();goto _LL21C;_LL226: Cyc_Dump_dump_begin();
Cyc_Dump_dump_alpha(_tag_arr("enum",sizeof(unsigned char),5));Cyc_Dump_dump_boldqvar(
_tmp148);if(_tmp147 == 0){Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(
unsigned char),3));}else{Cyc_Dump_dump_symbol(_tag_arr(" ",sizeof(unsigned char),
2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(_tag_arr("\\\\\n~~",sizeof(
unsigned char),6));Cyc_Dump_dumpenumfields((struct Cyc_List_List*)_tmp147->v);Cyc_Dump_dump_symbol(
_tag_arr("\\\\\n",sizeof(unsigned char),4));Cyc_Dump_dump_rb();Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));}Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(
comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(unsigned char),2));Cyc_Dump_dump_end();
goto _LL21C;_LL228: Cyc_Dump_dump_begin();Cyc_Dump_dump_alpha(_tag_arr("typedef",
sizeof(unsigned char),8));((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(
struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*a))Cyc_Dump_dumptqtd)(({
struct Cyc_Absyn_Tqual _tmp14B;_tmp14B.q_const=0;_tmp14B.q_volatile=0;_tmp14B.q_restrict=
0;_tmp14B;}),(void*)_tmp14A->defn,Cyc_Dump_dump_boldtypedefname,_tmp14A);Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(
comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(unsigned char),2));Cyc_Dump_dump_end();
goto _LL21C;_LL22A: goto _LL21C;_LL22C: goto _LL21C;_LL22E: goto _LL21C;_LL230: goto
_LL21C;_LL232: goto _LL21C;_LL234: goto _LL21C;_LL21C:;}static int Cyc_is_other_special(
unsigned char c){switch(c){case '\\': _LL246: goto _LL247;case '"': _LL247: goto _LL248;
case ';': _LL248: goto _LL249;case '&': _LL249: goto _LL24A;case '(': _LL24A: goto _LL24B;
case ')': _LL24B: goto _LL24C;case '|': _LL24C: goto _LL24D;case '^': _LL24D: goto _LL24E;
case '<': _LL24E: goto _LL24F;case '>': _LL24F: goto _LL250;case ' ': _LL250: goto _LL251;
case '\n': _LL251: goto _LL252;case '\t': _LL252: return 1;default: _LL253: return 0;}}
static struct _tagged_arr Cyc_sh_escape_string(struct _tagged_arr s){unsigned int
_tmp14C=Cyc_Std_strlen(s);int _tmp14D=0;int _tmp14E=0;{int i=0;for(0;i < _tmp14C;i ++){
unsigned char _tmp14F=((const unsigned char*)s.curr)[i];if(_tmp14F == '\''){_tmp14D
++;}else{if(Cyc_is_other_special(_tmp14F)){_tmp14E ++;}}}}if(_tmp14D == 0? _tmp14E
== 0: 0){return s;}if(_tmp14D == 0){return(struct _tagged_arr)Cyc_Std_strconcat_l(({
struct _tagged_arr*_tmp150[3];_tmp150[2]=_init_tag_arr(_cycalloc(sizeof(struct
_tagged_arr)),"'",sizeof(unsigned char),2);_tmp150[1]=({struct _tagged_arr*
_tmp151=_cycalloc(sizeof(struct _tagged_arr));_tmp151[0]=(struct _tagged_arr)s;
_tmp151;});_tmp150[0]=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",
sizeof(unsigned char),2);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp150,sizeof(struct _tagged_arr*),3));}));}{unsigned int _tmp152=(
_tmp14C + _tmp14D)+ _tmp14E;struct _tagged_arr _tmp153=({unsigned int _tmp157=
_tmp152 + 1;unsigned char*_tmp158=(unsigned char*)_cycalloc_atomic(_check_times(
sizeof(unsigned char),_tmp157));struct _tagged_arr _tmp15A=_tag_arr(_tmp158,
sizeof(unsigned char),_tmp152 + 1);{unsigned int _tmp159=_tmp157;unsigned int i;
for(i=0;i < _tmp159;i ++){_tmp158[i]='\000';}};_tmp15A;});int _tmp154=0;int _tmp155=
0;for(0;_tmp154 < _tmp14C;_tmp154 ++){unsigned char _tmp156=((const unsigned char*)s.curr)[
_tmp154];if(_tmp156 == '\''? 1: Cyc_is_other_special(_tmp156)){*((unsigned char*)
_check_unknown_subscript(_tmp153,sizeof(unsigned char),_tmp155 ++))='\\';}*((
unsigned char*)_check_unknown_subscript(_tmp153,sizeof(unsigned char),_tmp155 ++))=
_tmp156;}return(struct _tagged_arr)_tmp153;}}static struct _tagged_arr*Cyc_sh_escape_stringptr(
struct _tagged_arr*sp){return({struct _tagged_arr*_tmp15B=_cycalloc(sizeof(struct
_tagged_arr));_tmp15B[0]=Cyc_sh_escape_string(*sp);_tmp15B;});}static struct Cyc_Lineno_Pos*
Cyc_new_pos(){return({struct Cyc_Lineno_Pos*_tmp15C=_cycalloc(sizeof(struct Cyc_Lineno_Pos));
_tmp15C->logical_file=_tag_arr("",sizeof(unsigned char),1);_tmp15C->line=Cyc_Core_new_string(
0);_tmp15C->line_no=0;_tmp15C->col=0;_tmp15C;});}struct _tuple14{int f1;struct Cyc_Lineno_Pos*
f2;};static struct _tuple14*Cyc_start2pos(int x){return({struct _tuple14*_tmp15D=
_cycalloc(sizeof(struct _tuple14));_tmp15D->f1=x;_tmp15D->f2=Cyc_new_pos();
_tmp15D;});}static int Cyc_decl2start(struct Cyc_Absyn_Decl*d){return((struct Cyc_Position_Segment*)
_check_null(d->loc))->start;}struct _tuple15{struct Cyc_Lineno_Pos*f1;void*f2;};
static struct Cyc_List_List*Cyc_this_file(struct _tagged_arr file,struct Cyc_List_List*
x){struct Cyc_List_List*_tmp15E=0;for(0;x != 0;x=x->tl){if(Cyc_Std_strcmp(((*((
struct _tuple15*)x->hd)).f1)->logical_file,file)== 0){_tmp15E=({struct Cyc_List_List*
_tmp15F=_cycalloc(sizeof(struct Cyc_List_List));_tmp15F->hd=(struct _tuple15*)x->hd;
_tmp15F->tl=_tmp15E;_tmp15F;});}}_tmp15E=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp15E);return _tmp15E;}static int Cyc_lineno(struct Cyc_Lineno_Pos*
p){return p->line_no;}static struct Cyc_List_List*Cyc_flatten_decls(struct Cyc_List_List*
decls){struct Cyc_List_List*_tmp160=0;while(decls != 0){void*_tmp161=(void*)((
struct Cyc_Absyn_Decl*)decls->hd)->r;struct Cyc_List_List*_tmp162;struct Cyc_List_List*
_tmp163;struct Cyc_List_List*_tmp164;_LL256: if(*((int*)_tmp161)== 9){_LL26E:
_tmp162=((struct Cyc_Absyn_Namespace_d_struct*)_tmp161)->f2;goto _LL257;}else{goto
_LL258;}_LL258: if(*((int*)_tmp161)== 10){_LL26F: _tmp163=((struct Cyc_Absyn_Using_d_struct*)
_tmp161)->f2;goto _LL259;}else{goto _LL25A;}_LL25A: if(*((int*)_tmp161)== 11){
_LL270: _tmp164=((struct Cyc_Absyn_ExternC_d_struct*)_tmp161)->f1;goto _LL25B;}
else{goto _LL25C;}_LL25C: if(*((int*)_tmp161)== 4){goto _LL25D;}else{goto _LL25E;}
_LL25E: if(*((int*)_tmp161)== 5){goto _LL25F;}else{goto _LL260;}_LL260: if(*((int*)
_tmp161)== 0){goto _LL261;}else{goto _LL262;}_LL262: if(*((int*)_tmp161)== 6){goto
_LL263;}else{goto _LL264;}_LL264: if(*((int*)_tmp161)== 7){goto _LL265;}else{goto
_LL266;}_LL266: if(*((int*)_tmp161)== 8){goto _LL267;}else{goto _LL268;}_LL268: if(*((
int*)_tmp161)== 1){goto _LL269;}else{goto _LL26A;}_LL26A: if(*((int*)_tmp161)== 2){
goto _LL26B;}else{goto _LL26C;}_LL26C: if(*((int*)_tmp161)== 3){goto _LL26D;}else{
goto _LL255;}_LL257: _tmp163=_tmp162;goto _LL259;_LL259: _tmp164=_tmp163;goto _LL25B;
_LL25B: decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp164,decls->tl);goto _LL255;_LL25D: goto _LL25F;_LL25F: goto
_LL261;_LL261: goto _LL263;_LL263: goto _LL265;_LL265: goto _LL267;_LL267: goto _LL269;
_LL269: goto _LL26B;_LL26B: goto _LL26D;_LL26D: _tmp160=({struct Cyc_List_List*_tmp165=
_cycalloc(sizeof(struct Cyc_List_List));_tmp165->hd=(struct Cyc_Absyn_Decl*)decls->hd;
_tmp165->tl=_tmp160;_tmp165;});decls=decls->tl;goto _LL255;_LL255:;}return((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp160);}
static struct Cyc_List_List*Cyc_cycdoc_files=0;static void Cyc_add_other(struct
_tagged_arr s){Cyc_cycdoc_files=({struct Cyc_List_List*_tmp166=_cycalloc(sizeof(
struct Cyc_List_List));_tmp166->hd=({struct _tagged_arr*_tmp167=_cycalloc(sizeof(
struct _tagged_arr));_tmp167[0]=s;_tmp167;});_tmp166->tl=Cyc_cycdoc_files;_tmp166;});}
static struct Cyc_List_List*Cyc_cycargs=0;static void Cyc_add_cycarg(struct
_tagged_arr s){Cyc_cycargs=({struct Cyc_List_List*_tmp168=_cycalloc(sizeof(struct
Cyc_List_List));_tmp168->hd=({struct _tagged_arr*_tmp169=_cycalloc(sizeof(struct
_tagged_arr));_tmp169[0]=s;_tmp169;});_tmp168->tl=Cyc_cycargs;_tmp168;});}static
unsigned char _tmp16A[8]="cyclone";static struct _tagged_arr Cyc_cyclone_file={
_tmp16A,_tmp16A,_tmp16A + 8};static void Cyc_set_cyclone_file(struct _tagged_arr s){
Cyc_cyclone_file=s;}struct _tuple16{int f1;struct Cyc_Absyn_Decl*f2;};struct
_tuple17{struct Cyc_Lineno_Pos*f1;struct Cyc_Absyn_Decl*f2;};static void Cyc_process_file(
struct _tagged_arr filename){struct _tagged_arr _tmp16B=Cyc_Filename_chop_extension(
filename);struct _tagged_arr _tmp16C=Cyc_Std_strconcat((struct _tagged_arr)_tmp16B,
_tag_arr(".cyp",sizeof(unsigned char),5));struct _tagged_arr _tmp16D=Cyc_Std_str_sepstr(({
struct Cyc_List_List*_tmp18E=_cycalloc(sizeof(struct Cyc_List_List));_tmp18E->hd=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(unsigned char),1);
_tmp18E->tl=((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cycargs));_tmp18E;}),_tag_arr(" ",
sizeof(unsigned char),2));struct _tagged_arr _tmp16E=({struct Cyc_Std_String_pa_struct
_tmp18D;_tmp18D.tag=0;_tmp18D.f1=(struct _tagged_arr)Cyc_sh_escape_string(
filename);{struct Cyc_Std_String_pa_struct _tmp18C;_tmp18C.tag=0;_tmp18C.f1=(
struct _tagged_arr)Cyc_sh_escape_string((struct _tagged_arr)_tmp16C);{struct Cyc_Std_String_pa_struct
_tmp18B;_tmp18B.tag=0;_tmp18B.f1=(struct _tagged_arr)_tmp16D;{struct Cyc_Std_String_pa_struct
_tmp18A;_tmp18A.tag=0;_tmp18A.f1=(struct _tagged_arr)Cyc_cyclone_file;{void*
_tmp189[4]={& _tmp18A,& _tmp18B,& _tmp18C,& _tmp18D};Cyc_Std_aprintf(_tag_arr("%s %s -E -o %s -x cyc %s",
sizeof(unsigned char),25),_tag_arr(_tmp189,sizeof(void*),4));}}}}});if(Cyc_Std_system((
struct _tagged_arr)_tmp16E)!= 0){({void*_tmp16F[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\nError: preprocessing\n",sizeof(unsigned char),23),_tag_arr(_tmp16F,
sizeof(void*),0));});return;}Cyc_Position_reset_position((struct _tagged_arr)
_tmp16C);{struct Cyc_Std___sFILE*in_file=(struct Cyc_Std___sFILE*)_check_null(Cyc_Std_fopen((
struct _tagged_arr)_tmp16C,_tag_arr("r",sizeof(unsigned char),2)));Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp170=Cyc_Parse_parse_file(in_file);Cyc_Lex_lex_init();Cyc_Std_file_close((
struct Cyc_Std___sFILE*)in_file);_tmp170=Cyc_flatten_decls(_tmp170);{struct Cyc_List_List*
_tmp171=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(int),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_start2pos,((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_decl2start,_tmp170));Cyc_Lineno_poss_of_abss((
struct _tagged_arr)_tmp16C,_tmp171);Cyc_Std_remove((struct _tagged_arr)_tmp16C);{
struct Cyc_List_List*_tmp172=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple14*))Cyc_Core_snd,_tmp171),_tmp170);_tmp172=((struct Cyc_List_List*(*)(
struct _tagged_arr file,struct Cyc_List_List*x))Cyc_this_file)(filename,_tmp172);{
struct Cyc_List_List*_tmp173=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Lineno_Pos*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(struct
Cyc_Lineno_Pos*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)(struct _tuple17*))Cyc_Core_fst,_tmp172)),((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Decl*(*)(struct _tuple17*))Cyc_Core_snd,_tmp172));struct Cyc_Std___sFILE*
_tmp174=(struct Cyc_Std___sFILE*)_check_null(Cyc_Std_fopen(filename,_tag_arr("r",
sizeof(unsigned char),2)));struct Cyc_Lexing_lexbuf*_tmp175=Cyc_Lexing_from_file(
_tmp174);struct Cyc_List_List*_tmp176=0;struct _tuple10*comment;while((comment=((
struct _tuple10*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_token)(_tmp175))!= 0){
_tmp176=({struct Cyc_List_List*_tmp177=_cycalloc(sizeof(struct Cyc_List_List));
_tmp177->hd=(struct _tuple10*)_check_null(comment);_tmp177->tl=_tmp176;_tmp177;});}
Cyc_Std_fclose(_tmp174);_tmp176=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp176);{struct Cyc_List_List*_tmp178=((struct Cyc_List_List*(*)(
struct _tuple14*(*f)(int),struct Cyc_List_List*x))Cyc_List_map)(Cyc_start2pos,((
struct Cyc_List_List*(*)(int(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((
int(*)(struct _tuple10*))Cyc_Core_fst,_tmp176));Cyc_Lineno_poss_of_abss(filename,
_tmp178);{struct Cyc_List_List*_tmp179=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple14*))Cyc_Core_snd,_tmp178),((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple10*))
Cyc_Core_snd,_tmp176));_tmp179=Cyc_this_file(filename,_tmp179);{struct Cyc_List_List*
_tmp17A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*f)(
struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple15*))Cyc_Core_fst,_tmp179)),((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple15*))
Cyc_Core_snd,_tmp179));while(_tmp173 != 0? _tmp17A != 0: 0){struct _tuple16 _tmp17C;
struct Cyc_Absyn_Decl*_tmp17D;int _tmp17E;struct _tuple16*_tmp17B=(struct _tuple16*)
_tmp173->hd;_tmp17C=*_tmp17B;_LL273: _tmp17E=_tmp17C.f1;goto _LL272;_LL272: _tmp17D=
_tmp17C.f2;goto _LL271;_LL271: {struct _tuple10 _tmp180;void*_tmp181;int _tmp182;
struct _tuple10*_tmp17F=(struct _tuple10*)_tmp17A->hd;_tmp180=*_tmp17F;_LL276:
_tmp182=_tmp180.f1;goto _LL275;_LL275: _tmp181=_tmp180.f2;goto _LL274;_LL274: {void*
_tmp183=_tmp181;struct _tagged_arr _tmp184;struct _tagged_arr _tmp185;_LL278: if(*((
int*)_tmp183)== 1){_LL27C: _tmp184=((struct Cyc_Standalone_struct*)_tmp183)->f1;
goto _LL279;}else{goto _LL27A;}_LL27A: if(*((int*)_tmp183)== 0){_LL27D: _tmp185=((
struct Cyc_MatchDecl_struct*)_tmp183)->f1;goto _LL27B;}else{goto _LL277;}_LL279: Cyc_Dump_dump_comment((
struct _tagged_arr)_tmp184);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(
unsigned char),2));_tmp17A=_tmp17A->tl;goto _LL277;_LL27B: if(_tmp182 < _tmp17E){
_tmp17A=_tmp17A->tl;continue;}if(_tmp173->tl != 0){struct _tuple16 _tmp187;int
_tmp188;struct _tuple16*_tmp186=(struct _tuple16*)((struct Cyc_List_List*)
_check_null(_tmp173->tl))->hd;_tmp187=*_tmp186;_LL27F: _tmp188=_tmp187.f1;goto
_LL27E;_LL27E: if(_tmp188 < _tmp182){_tmp173=_tmp173->tl;continue;}}Cyc_Dump_dumpdecl(
_tmp17D,(struct _tagged_arr)_tmp185);Cyc_Std_fflush((struct Cyc_Std___sFILE*)Cyc_Std_stdout);
_tmp173=_tmp173->tl;_tmp17A=_tmp17A->tl;goto _LL277;_LL277:;}}}}}}}}}}}}struct
_tuple18{struct _tagged_arr f1;int f2;struct _tagged_arr f3;void*f4;struct _tagged_arr
f5;};int Cyc_main(int argc,struct _tagged_arr argv){struct Cyc_List_List*options=({
struct _tuple18*_tmp190[4];_tmp190[3]=({struct _tuple18*_tmp19A=_cycalloc(sizeof(
struct _tuple18));_tmp19A->f1=_tag_arr("-B",sizeof(unsigned char),3);_tmp19A->f2=
1;_tmp19A->f3=_tag_arr("<file>",sizeof(unsigned char),7);_tmp19A->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp19B=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));
_tmp19B[0]=({struct Cyc_Arg_Flag_spec_struct _tmp19C;_tmp19C.tag=1;_tmp19C.f1=Cyc_add_cycarg;
_tmp19C;});_tmp19B;});_tmp19A->f5=_tag_arr("Add to the list of directories to search for compiler files",
sizeof(unsigned char),60);_tmp19A;});_tmp190[2]=({struct _tuple18*_tmp197=
_cycalloc(sizeof(struct _tuple18));_tmp197->f1=_tag_arr("-I",sizeof(unsigned char),
3);_tmp197->f2=1;_tmp197->f3=_tag_arr("<dir>",sizeof(unsigned char),6);_tmp197->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp198=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));
_tmp198[0]=({struct Cyc_Arg_Flag_spec_struct _tmp199;_tmp199.tag=1;_tmp199.f1=Cyc_add_cycarg;
_tmp199;});_tmp198;});_tmp197->f5=_tag_arr("Add to the list of directories to search for include files",
sizeof(unsigned char),59);_tmp197;});_tmp190[1]=({struct _tuple18*_tmp194=
_cycalloc(sizeof(struct _tuple18));_tmp194->f1=_tag_arr("-D",sizeof(unsigned char),
3);_tmp194->f2=1;_tmp194->f3=_tag_arr("<name>[=<value>]",sizeof(unsigned char),
17);_tmp194->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp195=_cycalloc(
sizeof(struct Cyc_Arg_Flag_spec_struct));_tmp195[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp196;_tmp196.tag=1;_tmp196.f1=Cyc_add_cycarg;_tmp196;});_tmp195;});_tmp194->f5=
_tag_arr("Pass definition to preprocessor",sizeof(unsigned char),32);_tmp194;});
_tmp190[0]=({struct _tuple18*_tmp191=_cycalloc(sizeof(struct _tuple18));_tmp191->f1=
_tag_arr("-cyclone",sizeof(unsigned char),9);_tmp191->f2=0;_tmp191->f3=_tag_arr(" <file>",
sizeof(unsigned char),8);_tmp191->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp192=_cycalloc(sizeof(struct Cyc_Arg_String_spec_struct));_tmp192[0]=({struct
Cyc_Arg_String_spec_struct _tmp193;_tmp193.tag=5;_tmp193.f1=Cyc_set_cyclone_file;
_tmp193;});_tmp192;});_tmp191->f5=_tag_arr("Use <file> as the cyclone compiler",
sizeof(unsigned char),35);_tmp191;});((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp190,sizeof(struct _tuple18*),4));});Cyc_Arg_parse(
options,Cyc_add_other,_tag_arr("Options:",sizeof(unsigned char),9),argv);{struct
Cyc_List_List*_tmp18F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cycdoc_files);for(0;_tmp18F != 0;_tmp18F=_tmp18F->tl){Cyc_process_file(*((
struct _tagged_arr*)_tmp18F->hd));}}return 0;}
