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
static const int Cyc_Std_String_pa=0;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};static const int Cyc_Std_Int_pa=1;struct Cyc_Std_Int_pa_struct{int
tag;unsigned int f1;};static const int Cyc_Std_Double_pa=2;struct Cyc_Std_Double_pa_struct{
int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=3;struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=4;struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern int Cyc_Std_printf(struct _tagged_arr fmt,
struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,
struct _tagged_arr);static const int Cyc_Std_ShortPtr_sa=0;struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};static const int Cyc_Std_UShortPtr_sa=1;struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=2;struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=3;struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=4;struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};extern unsigned char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{void(*
refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};extern struct
Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc_Std___sFILE*);extern int Cyc_Lexing_lexeme_start(
struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
extern struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);extern int Cyc_List_length(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),
struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[18];extern
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern unsigned char Cyc_List_Nth[8];
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern unsigned int Cyc_Std_strlen(struct _tagged_arr s);extern int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_strconcat_l(
struct Cyc_List_List*);extern struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,
struct _tagged_arr);extern struct _tagged_arr Cyc_Std_substring(struct _tagged_arr,
int ofs,unsigned int n);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern void Cyc_Lineno_poss_of_abss(struct
_tagged_arr filename,struct Cyc_List_List*places);extern struct _tagged_arr Cyc_Filename_chop_extension(
struct _tagged_arr);extern unsigned char Cyc_Position_Exit[9];extern void Cyc_Position_reset_position(
struct _tagged_arr);struct Cyc_Position_Segment;static const int Cyc_Position_Lex=0;
static const int Cyc_Position_Parse=1;static const int Cyc_Position_Elab=2;struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];extern
unsigned char Cyc_Array_Array_mismatch[19];struct Cyc_Std__Div{int quot;int rem;};
struct Cyc_Std__Ldiv{int quot;int rem;};extern int abs(int __x);extern int atexit(void(*
__func)());extern struct Cyc_Std__Div div(int __numer,int __denom);extern struct Cyc_Std__Ldiv
ldiv(int __numer,int __denom);extern int random();extern void srandom(unsigned int
__seed);extern int rand();extern void srand(unsigned int __seed);extern int rand_r(
unsigned int*__seed);extern int grantpt(int __fd);extern int unlockpt(int __fd);extern
int Cyc_Std_system(struct _tagged_arr);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;extern unsigned char Cyc_Arg_Bad[8];struct Cyc_Arg_Bad_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Arg_Error[10];
static const int Cyc_Arg_Unit_spec=0;struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();
};static const int Cyc_Arg_Flag_spec=1;struct Cyc_Arg_Flag_spec_struct{int tag;void(*
f1)(struct _tagged_arr);};static const int Cyc_Arg_FlagString_spec=2;struct Cyc_Arg_FlagString_spec_struct{
int tag;void(*f1)(struct _tagged_arr,struct _tagged_arr);};static const int Cyc_Arg_Set_spec=
3;struct Cyc_Arg_Set_spec_struct{int tag;int*f1;};static const int Cyc_Arg_Clear_spec=
4;struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};static const int Cyc_Arg_String_spec=
5;struct Cyc_Arg_String_spec_struct{int tag;void(*f1)(struct _tagged_arr);};static
const int Cyc_Arg_Int_spec=6;struct Cyc_Arg_Int_spec_struct{int tag;void(*f1)(int);}
;static const int Cyc_Arg_Rest_spec=7;struct Cyc_Arg_Rest_spec_struct{int tag;void(*
f1)(struct _tagged_arr);};extern void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*
anonfun)(struct _tagged_arr),struct _tagged_arr errmsg,struct _tagged_arr args);
static const int Cyc_Absyn_Loc_n=0;static const int Cyc_Absyn_Rel_n=0;struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_Abs_n=1;struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};
struct Cyc_Absyn_Conref;static const int Cyc_Absyn_Static=0;static const int Cyc_Absyn_Abstract=
1;static const int Cyc_Absyn_Public=2;static const int Cyc_Absyn_Extern=3;static const
int Cyc_Absyn_ExternC=4;struct Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int
q_restrict: 1;};static const int Cyc_Absyn_B1=0;static const int Cyc_Absyn_B2=1;static
const int Cyc_Absyn_B4=2;static const int Cyc_Absyn_B8=3;static const int Cyc_Absyn_AnyKind=
0;static const int Cyc_Absyn_MemKind=1;static const int Cyc_Absyn_BoxKind=2;static
const int Cyc_Absyn_RgnKind=3;static const int Cyc_Absyn_EffKind=4;static const int Cyc_Absyn_Signed=
0;static const int Cyc_Absyn_Unsigned=1;struct Cyc_Absyn_Conref{void*v;};static const
int Cyc_Absyn_Eq_constr=0;struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};
static const int Cyc_Absyn_Forward_constr=1;struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};static const int Cyc_Absyn_No_constr=0;static
const int Cyc_Absyn_Eq_kb=0;struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};static
const int Cyc_Absyn_Unknown_kb=1;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct
Cyc_Core_Opt*f1;};static const int Cyc_Absyn_Less_kb=2;struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*
name;int*identity;void*kind;};static const int Cyc_Absyn_Unknown_b=0;static const int
Cyc_Absyn_Upper_b=0;struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;
};struct Cyc_Absyn_PtrInfo{void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*
nullable;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Conref*bounds;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple1*name;int is_xtunion;};static const int Cyc_Absyn_UnknownTunion=0;struct Cyc_Absyn_UnknownTunion_struct{
int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};static const int Cyc_Absyn_KnownTunion=
1;struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};
struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*targs;void*rgn;}
;struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*tunion_name;struct _tuple1*
field_name;int is_xtunion;};static const int Cyc_Absyn_UnknownTunionfield=0;struct
Cyc_Absyn_UnknownTunionfield_struct{int tag;struct Cyc_Absyn_UnknownTunionFieldInfo
f1;};static const int Cyc_Absyn_KnownTunionfield=1;struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};static const int Cyc_Absyn_VoidType=0;
static const int Cyc_Absyn_Evar=0;struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*
f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};static const int Cyc_Absyn_VarType=
1;struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};static const int
Cyc_Absyn_TunionType=2;struct Cyc_Absyn_TunionType_struct{int tag;struct Cyc_Absyn_TunionInfo
f1;};static const int Cyc_Absyn_TunionFieldType=3;struct Cyc_Absyn_TunionFieldType_struct{
int tag;struct Cyc_Absyn_TunionFieldInfo f1;};static const int Cyc_Absyn_PointerType=4;
struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};static
const int Cyc_Absyn_IntType=5;struct Cyc_Absyn_IntType_struct{int tag;void*f1;void*
f2;};static const int Cyc_Absyn_FloatType=1;static const int Cyc_Absyn_DoubleType=6;
struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};static const int Cyc_Absyn_ArrayType=
7;struct Cyc_Absyn_ArrayType_struct{int tag;void*f1;struct Cyc_Absyn_Tqual f2;struct
Cyc_Absyn_Exp*f3;};static const int Cyc_Absyn_FnType=8;struct Cyc_Absyn_FnType_struct{
int tag;struct Cyc_Absyn_FnInfo f1;};static const int Cyc_Absyn_TupleType=9;struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_StructType=10;struct Cyc_Absyn_StructType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Structdecl**f3;};
static const int Cyc_Absyn_UnionType=11;struct Cyc_Absyn_UnionType_struct{int tag;
struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Uniondecl**f3;};static
const int Cyc_Absyn_EnumType=12;struct Cyc_Absyn_EnumType_struct{int tag;struct
_tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};static const int Cyc_Absyn_SizeofType=13;
struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};static const int Cyc_Absyn_AnonStructType=
14;struct Cyc_Absyn_AnonStructType_struct{int tag;struct Cyc_List_List*f1;};static
const int Cyc_Absyn_AnonUnionType=15;struct Cyc_Absyn_AnonUnionType_struct{int tag;
struct Cyc_List_List*f1;};static const int Cyc_Absyn_AnonEnumType=16;struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_RgnHandleType=17;struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};static const int Cyc_Absyn_TypedefType=
18;struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;struct Cyc_Core_Opt*f3;};static const int Cyc_Absyn_HeapRgn=2;static const int Cyc_Absyn_AccessEff=
19;struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};static const int Cyc_Absyn_JoinEff=
20;struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};static const int
Cyc_Absyn_RgnsEff=21;struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};static const
int Cyc_Absyn_NoTypes=0;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;};static const int Cyc_Absyn_WithTypes=1;struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};static const int Cyc_Absyn_NonNullable_ps=0;struct Cyc_Absyn_NonNullable_ps_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_Nullable_ps=1;struct Cyc_Absyn_Nullable_ps_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_TaggedArray_ps=0;static
const int Cyc_Absyn_Printf_ft=0;static const int Cyc_Absyn_Scanf_ft=1;static const int
Cyc_Absyn_Regparm_att=0;struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};static
const int Cyc_Absyn_Stdcall_att=0;static const int Cyc_Absyn_Cdecl_att=1;static const
int Cyc_Absyn_Fastcall_att=2;static const int Cyc_Absyn_Noreturn_att=3;static const
int Cyc_Absyn_Const_att=4;static const int Cyc_Absyn_Aligned_att=1;struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};static const int Cyc_Absyn_Packed_att=5;static const int Cyc_Absyn_Section_att=
2;struct Cyc_Absyn_Section_att_struct{int tag;struct _tagged_arr f1;};static const int
Cyc_Absyn_Nocommon_att=6;static const int Cyc_Absyn_Shared_att=7;static const int Cyc_Absyn_Unused_att=
8;static const int Cyc_Absyn_Weak_att=9;static const int Cyc_Absyn_Dllimport_att=10;
static const int Cyc_Absyn_Dllexport_att=11;static const int Cyc_Absyn_No_instrument_function_att=
12;static const int Cyc_Absyn_Constructor_att=13;static const int Cyc_Absyn_Destructor_att=
14;static const int Cyc_Absyn_No_check_memory_usage_att=15;static const int Cyc_Absyn_Format_att=
3;struct Cyc_Absyn_Format_att_struct{int tag;void*f1;int f2;int f3;};static const int
Cyc_Absyn_Carray_mod=0;static const int Cyc_Absyn_ConstArray_mod=0;struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_Pointer_mod=1;struct Cyc_Absyn_Pointer_mod_struct{
int tag;void*f1;void*f2;struct Cyc_Absyn_Tqual f3;};static const int Cyc_Absyn_Function_mod=
2;struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};static const int Cyc_Absyn_TypeParams_mod=
3;struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};static const int Cyc_Absyn_Attributes_mod=4;struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Char_c=
0;struct Cyc_Absyn_Char_c_struct{int tag;void*f1;unsigned char f2;};static const int
Cyc_Absyn_Short_c=1;struct Cyc_Absyn_Short_c_struct{int tag;void*f1;short f2;};
static const int Cyc_Absyn_Int_c=2;struct Cyc_Absyn_Int_c_struct{int tag;void*f1;int
f2;};static const int Cyc_Absyn_LongLong_c=3;struct Cyc_Absyn_LongLong_c_struct{int
tag;void*f1;long long f2;};static const int Cyc_Absyn_Float_c=4;struct Cyc_Absyn_Float_c_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Absyn_String_c=5;struct Cyc_Absyn_String_c_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Absyn_Null_c=0;static const int Cyc_Absyn_Plus=
0;static const int Cyc_Absyn_Times=1;static const int Cyc_Absyn_Minus=2;static const int
Cyc_Absyn_Div=3;static const int Cyc_Absyn_Mod=4;static const int Cyc_Absyn_Eq=5;
static const int Cyc_Absyn_Neq=6;static const int Cyc_Absyn_Gt=7;static const int Cyc_Absyn_Lt=
8;static const int Cyc_Absyn_Gte=9;static const int Cyc_Absyn_Lte=10;static const int Cyc_Absyn_Not=
11;static const int Cyc_Absyn_Bitnot=12;static const int Cyc_Absyn_Bitand=13;static
const int Cyc_Absyn_Bitor=14;static const int Cyc_Absyn_Bitxor=15;static const int Cyc_Absyn_Bitlshift=
16;static const int Cyc_Absyn_Bitlrshift=17;static const int Cyc_Absyn_Bitarshift=18;
static const int Cyc_Absyn_Size=19;static const int Cyc_Absyn_PreInc=0;static const int
Cyc_Absyn_PostInc=1;static const int Cyc_Absyn_PreDec=2;static const int Cyc_Absyn_PostDec=
3;struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;
struct Cyc_Absyn_VarargInfo*vai;};static const int Cyc_Absyn_StructField=0;struct Cyc_Absyn_StructField_struct{
int tag;struct _tagged_arr*f1;};static const int Cyc_Absyn_TupleIndex=1;struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*
rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};static const int
Cyc_Absyn_Const_e=0;struct Cyc_Absyn_Const_e_struct{int tag;void*f1;};static const
int Cyc_Absyn_Var_e=1;struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*f1;void*
f2;};static const int Cyc_Absyn_UnknownId_e=2;struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple1*f1;};static const int Cyc_Absyn_Primop_e=3;struct Cyc_Absyn_Primop_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_AssignOp_e=4;
struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};static const int Cyc_Absyn_Increment_e=5;struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};static const int Cyc_Absyn_Conditional_e=6;
struct Cyc_Absyn_Conditional_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;struct Cyc_Absyn_Exp*f3;};static const int Cyc_Absyn_SeqExp_e=7;struct Cyc_Absyn_SeqExp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};static const int Cyc_Absyn_UnknownCall_e=
8;struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};static const int Cyc_Absyn_FnCall_e=9;struct Cyc_Absyn_FnCall_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;
};static const int Cyc_Absyn_Throw_e=10;struct Cyc_Absyn_Throw_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_NoInstantiate_e=11;struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_Instantiate_e=12;struct
Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};static const int Cyc_Absyn_Cast_e=13;struct Cyc_Absyn_Cast_e_struct{int tag;void*
f1;struct Cyc_Absyn_Exp*f2;};static const int Cyc_Absyn_Address_e=14;struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_New_e=15;struct Cyc_Absyn_New_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};static const int Cyc_Absyn_Sizeoftyp_e=
16;struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};static const int Cyc_Absyn_Sizeofexp_e=
17;struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};static
const int Cyc_Absyn_Offsetof_e=18;struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;
void*f2;};static const int Cyc_Absyn_Gentyp_e=19;struct Cyc_Absyn_Gentyp_e_struct{
int tag;struct Cyc_List_List*f1;void*f2;};static const int Cyc_Absyn_Deref_e=20;
struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_StructMember_e=
21;struct Cyc_Absyn_StructMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_tagged_arr*f2;};static const int Cyc_Absyn_StructArrow_e=22;struct Cyc_Absyn_StructArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};static const int Cyc_Absyn_Subscript_e=
23;struct Cyc_Absyn_Subscript_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};static const int Cyc_Absyn_Tuple_e=24;struct Cyc_Absyn_Tuple_e_struct{int tag;
struct Cyc_List_List*f1;};static const int Cyc_Absyn_CompoundLit_e=25;struct _tuple2{
struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Array_e=
26;struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};static const int
Cyc_Absyn_Comprehension_e=27;struct Cyc_Absyn_Comprehension_e_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
static const int Cyc_Absyn_Struct_e=28;struct Cyc_Absyn_Struct_e_struct{int tag;
struct _tuple1*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl*
f4;};static const int Cyc_Absyn_AnonStruct_e=29;struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Tunion_e=30;
struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Tuniondecl*f4;struct Cyc_Absyn_Tunionfield*
f5;};static const int Cyc_Absyn_Enum_e=31;struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
static const int Cyc_Absyn_AnonEnum_e=32;struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static const int Cyc_Absyn_Malloc_e=
33;struct Cyc_Absyn_Malloc_e_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};static
const int Cyc_Absyn_UnresolvedMem_e=34;struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_StmtExp_e=
35;struct Cyc_Absyn_StmtExp_e_struct{int tag;struct Cyc_Absyn_Stmt*f1;};static const
int Cyc_Absyn_Codegen_e=36;struct Cyc_Absyn_Codegen_e_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};static const int Cyc_Absyn_Fill_e=37;struct Cyc_Absyn_Fill_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;
struct Cyc_Position_Segment*loc;void*annot;};struct _tuple3{struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;
struct _tuple3 condition;struct _tuple3 delta;struct Cyc_Absyn_Stmt*body;};static
const int Cyc_Absyn_Skip_s=0;static const int Cyc_Absyn_Exp_s=0;struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_Seq_s=1;struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_Return_s=
2;struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};static const int
Cyc_Absyn_IfThenElse_s=3;struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};static const int Cyc_Absyn_While_s=
4;struct Cyc_Absyn_While_s_struct{int tag;struct _tuple3 f1;struct Cyc_Absyn_Stmt*f2;
};static const int Cyc_Absyn_Break_s=5;struct Cyc_Absyn_Break_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};static const int Cyc_Absyn_Continue_s=6;struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};static const int Cyc_Absyn_Goto_s=7;struct Cyc_Absyn_Goto_s_struct{
int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_For_s=
8;struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple3 f2;
struct _tuple3 f3;struct Cyc_Absyn_Stmt*f4;};static const int Cyc_Absyn_Switch_s=9;
struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};static const int Cyc_Absyn_SwitchC_s=10;struct Cyc_Absyn_SwitchC_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Fallthru_s=
11;struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};static const int Cyc_Absyn_Decl_s=12;struct Cyc_Absyn_Decl_s_struct{int tag;
struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_Cut_s=
13;struct Cyc_Absyn_Cut_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};static const int
Cyc_Absyn_Splice_s=14;struct Cyc_Absyn_Splice_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};static const int Cyc_Absyn_Label_s=15;struct Cyc_Absyn_Label_s_struct{int tag;
struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_Do_s=16;
struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple3 f2;};
static const int Cyc_Absyn_TryCatch_s=17;struct Cyc_Absyn_TryCatch_s_struct{int tag;
struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Region_s=
18;struct Cyc_Absyn_Region_s_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;struct Cyc_Absyn_Stmt*f3;};static const int Cyc_Absyn_ForArray_s=19;struct Cyc_Absyn_ForArray_s_struct{
int tag;struct Cyc_Absyn_ForArrayInfo f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};static const int
Cyc_Absyn_Wild_p=0;static const int Cyc_Absyn_Var_p=0;struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_Absyn_Null_p=1;static const
int Cyc_Absyn_Int_p=1;struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};static
const int Cyc_Absyn_Char_p=2;struct Cyc_Absyn_Char_p_struct{int tag;unsigned char f1;
};static const int Cyc_Absyn_Float_p=3;struct Cyc_Absyn_Float_p_struct{int tag;struct
_tagged_arr f1;};static const int Cyc_Absyn_Tuple_p=4;struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_Pointer_p=5;struct Cyc_Absyn_Pointer_p_struct{
int tag;struct Cyc_Absyn_Pat*f1;};static const int Cyc_Absyn_Reference_p=6;struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_Absyn_Struct_p=7;struct Cyc_Absyn_Struct_p_struct{
int tag;struct Cyc_Absyn_Structdecl*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;};static const int Cyc_Absyn_Tunion_p=8;struct Cyc_Absyn_Tunion_p_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;};static const int Cyc_Absyn_Enum_p=9;struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};static const int
Cyc_Absyn_AnonEnum_p=10;struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct
Cyc_Absyn_Enumfield*f2;};static const int Cyc_Absyn_UnknownId_p=11;struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple1*f1;};static const int Cyc_Absyn_UnknownCall_p=12;struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};static
const int Cyc_Absyn_UnknownFields_p=13;struct Cyc_Absyn_UnknownFields_p_struct{int
tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{
void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*cnst_exp;struct Cyc_Absyn_Stmt*body;
struct Cyc_Position_Segment*loc;};static const int Cyc_Absyn_Unresolved_b=0;static
const int Cyc_Absyn_Global_b=0;struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};static const int Cyc_Absyn_Funname_b=1;struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};static const int Cyc_Absyn_Param_b=2;struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_Absyn_Local_b=3;struct Cyc_Absyn_Local_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_Absyn_Pat_b=4;struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Structfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structdecl{
void*sc;struct Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Uniondecl{void*sc;struct
Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple1*name;struct Cyc_List_List*tvs;void*defn;};static const int Cyc_Absyn_Var_d=0;
struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};static const int
Cyc_Absyn_Fn_d=1;struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};
static const int Cyc_Absyn_Let_d=2;struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};
static const int Cyc_Absyn_Letv_d=3;struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};static const int Cyc_Absyn_Struct_d=4;struct Cyc_Absyn_Struct_d_struct{int tag;
struct Cyc_Absyn_Structdecl*f1;};static const int Cyc_Absyn_Union_d=5;struct Cyc_Absyn_Union_d_struct{
int tag;struct Cyc_Absyn_Uniondecl*f1;};static const int Cyc_Absyn_Tunion_d=6;struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};static const int
Cyc_Absyn_Enum_d=7;struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};static const int Cyc_Absyn_Typedef_d=8;struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};static const int Cyc_Absyn_Namespace_d=9;struct Cyc_Absyn_Namespace_d_struct{
int tag;struct _tagged_arr*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Using_d=
10;struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;
};static const int Cyc_Absyn_ExternC_d=11;struct Cyc_Absyn_ExternC_d_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};static const int Cyc_Absyn_ArrayElement=0;struct Cyc_Absyn_ArrayElement_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_FieldName=1;struct Cyc_Absyn_FieldName_struct{
int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[15];extern
void*Cyc_Absyn_compress_kb(void*);extern struct Cyc_List_List*Cyc_Parse_parse_file(
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
end;};static const int Cyc_MatchDecl=0;struct Cyc_MatchDecl_struct{int tag;struct
_tagged_arr f1;};static const int Cyc_Standalone=1;struct Cyc_Standalone_struct{int
tag;struct _tagged_arr f1;};struct _tuple10{int f1;void*f2;};extern struct _tuple10*
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
case 0: _LL2: {int _tmp4=lexbuf->lex_start_pos + 5;int _tmp5=(lexbuf->lex_curr_pos - 
lexbuf->lex_start_pos)- 7;return({struct _tuple10*_tmp6=_cycalloc(sizeof(struct
_tuple10));_tmp6->f1=Cyc_Lexing_lexeme_start(lexbuf);_tmp6->f2=(void*)({struct
Cyc_Standalone_struct*_tmp7=_cycalloc(sizeof(struct Cyc_Standalone_struct));_tmp7[
0]=({struct Cyc_Standalone_struct _tmp8;_tmp8.tag=Cyc_Standalone;_tmp8.f1=Cyc_Std_substring((
struct _tagged_arr)lexbuf->lex_buffer,_tmp4,(unsigned int)_tmp5);_tmp8;});_tmp7;});
_tmp6;});}case 1: _LL3: {int _tmpA=lexbuf->lex_start_pos + 4;int _tmpB=(lexbuf->lex_curr_pos
- lexbuf->lex_start_pos)- 6;return({struct _tuple10*_tmpC=_cycalloc(sizeof(struct
_tuple10));_tmpC->f1=Cyc_Lexing_lexeme_start(lexbuf);_tmpC->f2=(void*)({struct
Cyc_MatchDecl_struct*_tmpD=_cycalloc(sizeof(struct Cyc_MatchDecl_struct));_tmpD[0]=({
struct Cyc_MatchDecl_struct _tmpE;_tmpE.tag=Cyc_MatchDecl;_tmpE.f1=Cyc_Std_substring((
struct _tagged_arr)lexbuf->lex_buffer,_tmpA,(unsigned int)_tmpB);_tmpE;});_tmpD;});
_tmpC;});}case 2: _LL9: return 0;case 3: _LL15: return Cyc_token(lexbuf);default: _LL16:(
lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(lexbuf,lexstate);}(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmp12=_cycalloc(sizeof(struct Cyc_Lexing_Error_struct));
_tmp12[0]=({struct Cyc_Lexing_Error_struct _tmp13;_tmp13.tag=Cyc_Lexing_Error;
_tmp13.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp13;});
_tmp12;}));}struct _tuple10*Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(
lexbuf,0);}struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];
static const int Cyc_Tcenv_VarRes=0;struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};
static const int Cyc_Tcenv_StructRes=1;struct Cyc_Tcenv_StructRes_struct{int tag;
struct Cyc_Absyn_Structdecl*f1;};static const int Cyc_Tcenv_TunionRes=2;struct Cyc_Tcenv_TunionRes_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};static
const int Cyc_Tcenv_EnumRes=3;struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};static const int Cyc_Tcenv_AnonEnumRes=4;struct
Cyc_Tcenv_AnonEnumRes_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict*
structdecls;struct Cyc_Dict_Dict*uniondecls;struct Cyc_Dict_Dict*tuniondecls;
struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;static const int
Cyc_Tcenv_NotLoop_j=0;static const int Cyc_Tcenv_CaseEnd_j=1;static const int Cyc_Tcenv_FnEnd_j=
2;static const int Cyc_Tcenv_Stmt_j=0;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};static const int Cyc_Tcenv_Outermost=0;struct Cyc_Tcenv_Outermost_struct{
int tag;void*f1;};static const int Cyc_Tcenv_Frame=1;struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};static const int Cyc_Tcenv_Hidden=2;struct Cyc_Tcenv_Hidden_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*
ae;struct Cyc_Core_Opt*le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*
Cyc_Tcutil_compress(void*t);extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);static const int Cyc_Dump_ALPHA=0;static const int Cyc_Dump_SYMBOL=
1;static const int Cyc_Dump_POINTER=2;void*Cyc_Dump_last_dumped=(void*)1;void Cyc_Dump_dump_alpha(
struct _tagged_arr s){if(Cyc_Dump_last_dumped == (void*)Cyc_Dump_ALPHA){({struct Cyc_Std_String_pa_struct
_tmp15;_tmp15.tag=Cyc_Std_String_pa;_tmp15.f1=(struct _tagged_arr)s;{void*_tmp14[
1]={& _tmp15};Cyc_Std_printf(_tag_arr(" %s",sizeof(unsigned char),4),_tag_arr(
_tmp14,sizeof(void*),1));}});}else{({struct Cyc_Std_String_pa_struct _tmp17;_tmp17.tag=
Cyc_Std_String_pa;_tmp17.f1=(struct _tagged_arr)s;{void*_tmp16[1]={& _tmp17};Cyc_Std_printf(
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp16,sizeof(void*),1));}});Cyc_Dump_last_dumped=(
void*)Cyc_Dump_ALPHA;}}void Cyc_Dump_dump_alphastr(struct _tagged_arr*s){Cyc_Dump_dump_alpha(*
s);}void Cyc_Dump_dump_symbol(struct _tagged_arr s){({struct Cyc_Std_String_pa_struct
_tmp19;_tmp19.tag=Cyc_Std_String_pa;_tmp19.f1=(struct _tagged_arr)s;{void*_tmp18[
1]={& _tmp19};Cyc_Std_printf(_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(
_tmp18,sizeof(void*),1));}});Cyc_Dump_last_dumped=(void*)Cyc_Dump_SYMBOL;}void
Cyc_Dump_dump_char(int c){({struct Cyc_Std_Int_pa_struct _tmp1B;_tmp1B.tag=Cyc_Std_Int_pa;
_tmp1B.f1=(int)((unsigned int)c);{void*_tmp1A[1]={& _tmp1B};Cyc_Std_printf(
_tag_arr("%c",sizeof(unsigned char),3),_tag_arr(_tmp1A,sizeof(void*),1));}});Cyc_Dump_last_dumped=(
void*)Cyc_Dump_SYMBOL;}void Cyc_Dump_dump_pointer(struct _tagged_arr s){if(Cyc_Dump_last_dumped
== (void*)Cyc_Dump_ALPHA){({struct Cyc_Std_String_pa_struct _tmp1D;_tmp1D.tag=Cyc_Std_String_pa;
_tmp1D.f1=(struct _tagged_arr)s;{void*_tmp1C[1]={& _tmp1D};Cyc_Std_printf(_tag_arr(" %s",
sizeof(unsigned char),4),_tag_arr(_tmp1C,sizeof(void*),1));}});}else{({struct Cyc_Std_String_pa_struct
_tmp1F;_tmp1F.tag=Cyc_Std_String_pa;_tmp1F.f1=(struct _tagged_arr)s;{void*_tmp1E[
1]={& _tmp1F};Cyc_Std_printf(_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(
_tmp1E,sizeof(void*),1));}});}Cyc_Dump_last_dumped=(void*)Cyc_Dump_POINTER;}void
Cyc_Dump_dump_boldalpha(struct _tagged_arr x){Cyc_Dump_dump_alpha(_tag_arr("",
sizeof(unsigned char),1));Cyc_Dump_dump_symbol(_tag_arr("\\textbf{",sizeof(
unsigned char),9));Cyc_Dump_dump_alpha(x);Cyc_Dump_dump_symbol(_tag_arr("}",
sizeof(unsigned char),2));Cyc_Dump_dump_alpha(_tag_arr("",sizeof(unsigned char),
1));}void Cyc_Dump_dump_lb(){Cyc_Dump_dump_symbol(_tag_arr("{\\lb}",sizeof(
unsigned char),6));}void Cyc_Dump_dump_rb(){Cyc_Dump_dump_symbol(_tag_arr("{\\rb}",
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
struct _tuple1*v){struct Cyc_List_List*_tmp20=0;{void*_tmp21=(*v).f1;struct Cyc_List_List*
_tmp29;struct Cyc_List_List*_tmp2B;_LL35: if(_tmp21 == (void*)Cyc_Absyn_Loc_n){goto
_LL36;}else{goto _LL37;}_LL37: if((unsigned int)_tmp21 > 1?*((int*)_tmp21)== Cyc_Absyn_Rel_n:
0){_LL42: _tmp29=((struct Cyc_Absyn_Rel_n_struct*)_tmp21)->f1;goto _LL38;}else{goto
_LL39;}_LL39: if((unsigned int)_tmp21 > 1?*((int*)_tmp21)== Cyc_Absyn_Abs_n: 0){
_LL44: _tmp2B=((struct Cyc_Absyn_Abs_n_struct*)_tmp21)->f1;goto _LL40;}else{goto
_LL34;}_LL36: _tmp29=0;goto _LL38;_LL38: _tmp20=_tmp29;goto _LL34;_LL40: _tmp20=(Cyc_Dump_qvar_to_Cids?
Cyc_Dump_add_cyc_prefix: 0)?({struct Cyc_List_List*_tmp2D=_cycalloc(sizeof(struct
Cyc_List_List));_tmp2D->hd=Cyc_Absynpp_cyc_stringptr;_tmp2D->tl=_tmp2B;_tmp2D;}):
_tmp2B;goto _LL34;_LL34:;}if(_tmp20 != 0){Cyc_Dump_dump_alphastr((struct
_tagged_arr*)_tmp20->hd);for(_tmp20=_tmp20->tl;_tmp20 != 0;_tmp20=_tmp20->tl){if(
Cyc_Dump_qvar_to_Cids){Cyc_Dump_dump_char((int)'_');}else{Cyc_Dump_dump_symbol(
_tag_arr("::",sizeof(unsigned char),3));}Cyc_Dump_dump_alpha(*((struct
_tagged_arr*)_tmp20->hd));}if(Cyc_Dump_qvar_to_Cids){Cyc_Dump_dump_symbol(
_tag_arr("_",sizeof(unsigned char),2));}else{Cyc_Dump_dump_symbol(_tag_arr("::",
sizeof(unsigned char),3));}Cyc_Dump_dump_alpha(*(*v).f2);}else{Cyc_Dump_dump_alpha(*(*
v).f2);}}void Cyc_Dump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict){Cyc_Dump_dump_alpha(
_tag_arr("restrict",sizeof(unsigned char),9));}if(tq.q_volatile){Cyc_Dump_dump_alpha(
_tag_arr("volatile",sizeof(unsigned char),9));}if(tq.q_const){Cyc_Dump_dump_alpha(
_tag_arr("const",sizeof(unsigned char),6));}}void Cyc_Dump_dumpscope(void*sc){
void*_tmp2E=sc;_LL48: if(_tmp2E == (void*)Cyc_Absyn_Static){goto _LL49;}else{goto
_LL50;}_LL50: if(_tmp2E == (void*)Cyc_Absyn_Public){goto _LL51;}else{goto _LL52;}
_LL52: if(_tmp2E == (void*)Cyc_Absyn_Extern){goto _LL53;}else{goto _LL54;}_LL54: if(
_tmp2E == (void*)Cyc_Absyn_ExternC){goto _LL55;}else{goto _LL56;}_LL56: if(_tmp2E == (
void*)Cyc_Absyn_Abstract){goto _LL57;}else{goto _LL47;}_LL49: Cyc_Dump_dump_alpha(
_tag_arr("static",sizeof(unsigned char),7));return;_LL51: return;_LL53: Cyc_Dump_dump_alpha(
_tag_arr("extern",sizeof(unsigned char),7));return;_LL55: Cyc_Dump_dump_alpha(
_tag_arr("extern \"C\"",sizeof(unsigned char),11));return;_LL57: Cyc_Dump_dump_alpha(
_tag_arr("abstract",sizeof(unsigned char),9));return;_LL47:;}void Cyc_Dump_dumpkind(
void*k){void*_tmp3A=k;_LL60: if(_tmp3A == (void*)Cyc_Absyn_AnyKind){goto _LL61;}
else{goto _LL62;}_LL62: if(_tmp3A == (void*)Cyc_Absyn_MemKind){goto _LL63;}else{goto
_LL64;}_LL64: if(_tmp3A == (void*)Cyc_Absyn_BoxKind){goto _LL65;}else{goto _LL66;}
_LL66: if(_tmp3A == (void*)Cyc_Absyn_RgnKind){goto _LL67;}else{goto _LL68;}_LL68: if(
_tmp3A == (void*)Cyc_Absyn_EffKind){goto _LL69;}else{goto _LL59;}_LL61: Cyc_Dump_dump_alpha(
_tag_arr("A",sizeof(unsigned char),2));return;_LL63: Cyc_Dump_dump_alpha(_tag_arr("M",
sizeof(unsigned char),2));return;_LL65: Cyc_Dump_dump_alpha(_tag_arr("B",sizeof(
unsigned char),2));return;_LL67: Cyc_Dump_dump_alpha(_tag_arr("R",sizeof(
unsigned char),2));return;_LL69: Cyc_Dump_dump_alpha(_tag_arr("E",sizeof(
unsigned char),2));return;_LL59:;}void Cyc_Dump_dumptps(struct Cyc_List_List*ts){
Cyc_Dump_egroup(Cyc_Dump_dumptyp,ts,_tag_arr("<",sizeof(unsigned char),2),
_tag_arr(">",sizeof(unsigned char),2),_tag_arr(", ",sizeof(unsigned char),3));
Cyc_Dump_dump_alpha(_tag_arr("",sizeof(unsigned char),1));}void Cyc_Dump_dumptvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Dump_dump_alphastr(tv->name);}void Cyc_Dump_dumpkindedtvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Dump_dump_alphastr(tv->name);{void*_tmp46=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmp50;void*_tmp52;void*_tmp54;_LL72: if(*((int*)_tmp46)== 
Cyc_Absyn_Eq_kb){_LL81: _tmp50=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp46)->f1;
if(_tmp50 == (void*)Cyc_Absyn_BoxKind){goto _LL73;}else{goto _LL74;}}else{goto _LL74;}
_LL74: if(*((int*)_tmp46)== Cyc_Absyn_Less_kb){_LL83: _tmp52=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp46)->f2;goto _LL75;}else{goto _LL76;}_LL76: if(*((int*)_tmp46)== Cyc_Absyn_Eq_kb){
_LL85: _tmp54=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp46)->f1;goto _LL77;}else{
goto _LL78;}_LL78: goto _LL79;_LL73: goto _LL71;_LL75: _tmp54=_tmp52;goto _LL77;_LL77:
Cyc_Dump_dump_symbol(_tag_arr("::",sizeof(unsigned char),3));Cyc_Dump_dumpkind(
_tmp54);goto _LL71;_LL79: Cyc_Dump_dump_symbol(_tag_arr("::?",sizeof(unsigned char),
4));goto _LL71;_LL71:;}}void Cyc_Dump_dumptvars(struct Cyc_List_List*tvs){((void(*)(
void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_egroup)(Cyc_Dump_dumptvar,
tvs,_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",sizeof(unsigned char),2),
_tag_arr(",",sizeof(unsigned char),2));Cyc_Dump_dump_alpha(_tag_arr("",sizeof(
unsigned char),1));}void Cyc_Dump_dumpkindedtvars(struct Cyc_List_List*tvs){((void(*)(
void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_egroup)(Cyc_Dump_dumpkindedtvar,
tvs,_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",sizeof(unsigned char),2),
_tag_arr(",",sizeof(unsigned char),2));Cyc_Dump_dump_alpha(_tag_arr("",sizeof(
unsigned char),1));}struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Dump_dumparg(
struct _tuple11*pr){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Dump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Dump_ignore,0);}void Cyc_Dump_dumpargs(struct
Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple11*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(
Cyc_Dump_dumparg,ts,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(
unsigned char),2),_tag_arr(", ",sizeof(unsigned char),3));}int Cyc_Dump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0){return 0;}{void*_tmp56=(void*)tms->hd;_LL88:
if((unsigned int)_tmp56 > 1?*((int*)_tmp56)== Cyc_Absyn_Pointer_mod: 0){goto _LL89;}
else{goto _LL90;}_LL90: goto _LL91;_LL89: return 1;_LL91: return 0;_LL87:;}}static void
Cyc_Dump_dumprgn(void*t){void*_tmp5C=Cyc_Tcutil_compress(t);_LL94: if(_tmp5C == (
void*)Cyc_Absyn_HeapRgn){goto _LL95;}else{goto _LL96;}_LL96: goto _LL97;_LL95: Cyc_Dump_dump_alpha(
_tag_arr("`H",sizeof(unsigned char),3));goto _LL93;_LL97: Cyc_Dump_dumpntyp(t);
goto _LL93;_LL93:;}struct _tuple12{struct Cyc_List_List*f1;struct Cyc_List_List*f2;}
;static struct _tuple12 Cyc_Dump_effects_split(void*t){struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;{void*_tmp62=Cyc_Tcutil_compress(t);void*_tmp6A;
struct Cyc_List_List*_tmp6C;_LL100: if((unsigned int)_tmp62 > 3?*((int*)_tmp62)== 
Cyc_Absyn_AccessEff: 0){_LL107: _tmp6A=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp62)->f1;goto _LL101;}else{goto _LL102;}_LL102: if((unsigned int)_tmp62 > 3?*((
int*)_tmp62)== Cyc_Absyn_JoinEff: 0){_LL109: _tmp6C=((struct Cyc_Absyn_JoinEff_struct*)
_tmp62)->f1;goto _LL103;}else{goto _LL104;}_LL104: goto _LL105;_LL101: rgions=({
struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(struct Cyc_List_List));_tmp6E->hd=(
void*)_tmp6A;_tmp6E->tl=rgions;_tmp6E;});goto _LL99;_LL103: for(0;_tmp6C != 0;
_tmp6C=_tmp6C->tl){struct Cyc_List_List*_tmp71;struct Cyc_List_List*_tmp73;struct
_tuple12 _tmp6F=Cyc_Dump_effects_split((void*)_tmp6C->hd);_LL116: _tmp73=_tmp6F.f1;
goto _LL114;_LL114: _tmp71=_tmp6F.f2;goto _LL112;_LL112: rgions=Cyc_List_imp_append(
_tmp73,rgions);effects=Cyc_List_imp_append(_tmp71,effects);}goto _LL99;_LL105:
effects=({struct Cyc_List_List*_tmp75=_cycalloc(sizeof(struct Cyc_List_List));
_tmp75->hd=(void*)t;_tmp75->tl=effects;_tmp75;});goto _LL99;_LL99:;}return({
struct _tuple12 _tmp76;_tmp76.f1=rgions;_tmp76.f2=effects;_tmp76;});}static void Cyc_Dump_dumpeff(
void*t){struct Cyc_List_List*_tmp79;struct Cyc_List_List*_tmp7B;struct _tuple12
_tmp77=Cyc_Dump_effects_split(t);_LL124: _tmp7B=_tmp77.f1;goto _LL122;_LL122:
_tmp79=_tmp77.f2;goto _LL120;_LL120: _tmp7B=Cyc_List_imp_rev(_tmp7B);_tmp79=Cyc_List_imp_rev(
_tmp79);for(0;_tmp79 != 0;_tmp79=_tmp79->tl){Cyc_Dump_dumpntyp((void*)_tmp79->hd);
Cyc_Dump_dump_char((int)'+');}Cyc_Dump_dump_lb();for(0;_tmp7B != 0;_tmp7B=_tmp7B->tl){
Cyc_Dump_dumprgn((void*)_tmp7B->hd);if(_tmp7B->tl != 0){Cyc_Dump_dump_char((int)',');}}
Cyc_Dump_dump_rb();}void Cyc_Dump_dumpntyp(void*t){void*_tmp7D=t;struct Cyc_Absyn_Tvar*
_tmpC9;int _tmpCB;struct Cyc_Core_Opt*_tmpCD;struct Cyc_Core_Opt*_tmpCF;int _tmpD1;
struct Cyc_Core_Opt*_tmpD3;struct Cyc_Core_Opt _tmpD5;void*_tmpD6;struct Cyc_Core_Opt*
_tmpD8;struct Cyc_Absyn_TunionInfo _tmpDA;void*_tmpDC;struct Cyc_List_List*_tmpDE;
void*_tmpE0;struct Cyc_Absyn_TunionFieldInfo _tmpE2;struct Cyc_List_List*_tmpE4;
void*_tmpE6;struct _tuple1*_tmpE8;struct Cyc_List_List*_tmpEA;void*_tmpEC;void*
_tmpEE;void*_tmpF0;void*_tmpF2;void*_tmpF4;void*_tmpF6;void*_tmpF8;void*_tmpFA;
void*_tmpFC;void*_tmpFE;void*_tmp100;void*_tmp102;void*_tmp105;void*_tmp107;void*
_tmp109;void*_tmp10B;void*_tmp10E;void*_tmp110;void*_tmp112;void*_tmp114;int
_tmp116;void*_tmp118;struct Cyc_List_List*_tmp11A;struct Cyc_List_List*_tmp11C;
struct _tuple1*_tmp11E;struct Cyc_List_List*_tmp120;struct _tuple1*_tmp122;struct
Cyc_List_List*_tmp124;struct _tuple1*_tmp126;struct Cyc_List_List*_tmp128;struct
_tuple1*_tmp12A;struct Cyc_List_List*_tmp12C;struct Cyc_List_List*_tmp12E;struct
Cyc_Core_Opt*_tmp130;struct Cyc_List_List*_tmp132;struct _tuple1*_tmp134;void*
_tmp136;_LL127: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_ArrayType:
0){goto _LL128;}else{goto _LL129;}_LL129: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)
== Cyc_Absyn_FnType: 0){goto _LL130;}else{goto _LL131;}_LL131: if((unsigned int)
_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_PointerType: 0){goto _LL132;}else{goto _LL133;}
_LL133: if(_tmp7D == (void*)Cyc_Absyn_VoidType){goto _LL134;}else{goto _LL135;}
_LL135: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_VarType: 0){_LL202:
_tmpC9=((struct Cyc_Absyn_VarType_struct*)_tmp7D)->f1;goto _LL136;}else{goto _LL137;}
_LL137: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_Evar: 0){_LL208:
_tmpCF=((struct Cyc_Absyn_Evar_struct*)_tmp7D)->f1;goto _LL206;_LL206: _tmpCD=((
struct Cyc_Absyn_Evar_struct*)_tmp7D)->f2;if(_tmpCD == 0){goto _LL204;}else{goto
_LL139;}_LL204: _tmpCB=((struct Cyc_Absyn_Evar_struct*)_tmp7D)->f3;goto _LL138;}
else{goto _LL139;}_LL139: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_Evar:
0){_LL217: _tmpD8=((struct Cyc_Absyn_Evar_struct*)_tmp7D)->f1;goto _LL212;_LL212:
_tmpD3=((struct Cyc_Absyn_Evar_struct*)_tmp7D)->f2;if(_tmpD3 == 0){goto _LL141;}
else{_tmpD5=*_tmpD3;_LL215: _tmpD6=(void*)_tmpD5.v;goto _LL210;}_LL210: _tmpD1=((
struct Cyc_Absyn_Evar_struct*)_tmp7D)->f3;goto _LL140;}else{goto _LL141;}_LL141: if((
unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_TunionType: 0){_LL219: _tmpDA=((
struct Cyc_Absyn_TunionType_struct*)_tmp7D)->f1;_LL225: _tmpE0=(void*)_tmpDA.tunion_info;
goto _LL223;_LL223: _tmpDE=_tmpDA.targs;goto _LL221;_LL221: _tmpDC=(void*)_tmpDA.rgn;
goto _LL142;}else{goto _LL143;}_LL143: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== 
Cyc_Absyn_TunionFieldType: 0){_LL227: _tmpE2=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp7D)->f1;_LL231: _tmpE6=(void*)_tmpE2.field_info;goto _LL229;_LL229: _tmpE4=
_tmpE2.targs;goto _LL144;}else{goto _LL145;}_LL145: if((unsigned int)_tmp7D > 3?*((
int*)_tmp7D)== Cyc_Absyn_EnumType: 0){_LL233: _tmpE8=((struct Cyc_Absyn_EnumType_struct*)
_tmp7D)->f1;goto _LL146;}else{goto _LL147;}_LL147: if((unsigned int)_tmp7D > 3?*((
int*)_tmp7D)== Cyc_Absyn_AnonEnumType: 0){_LL235: _tmpEA=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp7D)->f1;goto _LL148;}else{goto _LL149;}_LL149: if((unsigned int)_tmp7D > 3?*((
int*)_tmp7D)== Cyc_Absyn_IntType: 0){_LL239: _tmpEE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp7D)->f1;if(_tmpEE == (void*)Cyc_Absyn_Signed){goto _LL237;}else{goto _LL151;}
_LL237: _tmpEC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7D)->f2;if(_tmpEC == (
void*)Cyc_Absyn_B4){goto _LL150;}else{goto _LL151;}}else{goto _LL151;}_LL151: if((
unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_IntType: 0){_LL243: _tmpF2=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp7D)->f1;if(_tmpF2 == (void*)Cyc_Absyn_Signed){
goto _LL241;}else{goto _LL153;}_LL241: _tmpF0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp7D)->f2;if(_tmpF0 == (void*)Cyc_Absyn_B1){goto _LL152;}else{goto _LL153;}}else{
goto _LL153;}_LL153: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_IntType:
0){_LL247: _tmpF6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7D)->f1;if(_tmpF6
== (void*)Cyc_Absyn_Unsigned){goto _LL245;}else{goto _LL155;}_LL245: _tmpF4=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp7D)->f2;if(_tmpF4 == (void*)Cyc_Absyn_B1){
goto _LL154;}else{goto _LL155;}}else{goto _LL155;}_LL155: if((unsigned int)_tmp7D > 3?*((
int*)_tmp7D)== Cyc_Absyn_IntType: 0){_LL251: _tmpFA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp7D)->f1;if(_tmpFA == (void*)Cyc_Absyn_Signed){goto _LL249;}else{goto _LL157;}
_LL249: _tmpF8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7D)->f2;if(_tmpF8 == (
void*)Cyc_Absyn_B2){goto _LL156;}else{goto _LL157;}}else{goto _LL157;}_LL157: if((
unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_IntType: 0){_LL255: _tmpFE=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp7D)->f1;if(_tmpFE == (void*)Cyc_Absyn_Unsigned){
goto _LL253;}else{goto _LL159;}_LL253: _tmpFC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp7D)->f2;if(_tmpFC == (void*)Cyc_Absyn_B2){goto _LL158;}else{goto _LL159;}}else{
goto _LL159;}_LL159: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_IntType:
0){_LL259: _tmp102=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7D)->f1;if(
_tmp102 == (void*)Cyc_Absyn_Unsigned){goto _LL257;}else{goto _LL161;}_LL257: _tmp100=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp7D)->f2;if(_tmp100 == (void*)Cyc_Absyn_B4){
goto _LL160;}else{goto _LL161;}}else{goto _LL161;}_LL161: if((unsigned int)_tmp7D > 3?*((
int*)_tmp7D)== Cyc_Absyn_IntType: 0){_LL264: _tmp107=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp7D)->f1;if(_tmp107 == (void*)Cyc_Absyn_Signed){goto _LL262;}else{goto _LL163;}
_LL262: _tmp105=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7D)->f2;if(_tmp105 == (
void*)Cyc_Absyn_B8){goto _LL260;}else{goto _LL163;}}else{goto _LL163;}_LL260: if(Cyc_Dump_to_VC){
goto _LL162;}else{goto _LL163;}_LL163: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== 
Cyc_Absyn_IntType: 0){_LL268: _tmp10B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp7D)->f1;if(_tmp10B == (void*)Cyc_Absyn_Signed){goto _LL266;}else{goto _LL165;}
_LL266: _tmp109=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7D)->f2;if(_tmp109 == (
void*)Cyc_Absyn_B8){goto _LL164;}else{goto _LL165;}}else{goto _LL165;}_LL165: if((
unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_IntType: 0){_LL273: _tmp110=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp7D)->f1;if(_tmp110 == (void*)Cyc_Absyn_Unsigned){
goto _LL271;}else{goto _LL167;}_LL271: _tmp10E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp7D)->f2;if(_tmp10E == (void*)Cyc_Absyn_B8){goto _LL269;}else{goto _LL167;}}
else{goto _LL167;}_LL269: if(Cyc_Dump_to_VC){goto _LL166;}else{goto _LL167;}_LL167:
if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_IntType: 0){_LL277: _tmp114=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp7D)->f1;if(_tmp114 == (void*)Cyc_Absyn_Unsigned){
goto _LL275;}else{goto _LL169;}_LL275: _tmp112=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp7D)->f2;if(_tmp112 == (void*)Cyc_Absyn_B8){goto _LL168;}else{goto _LL169;}}
else{goto _LL169;}_LL169: if(_tmp7D == (void*)Cyc_Absyn_FloatType){goto _LL170;}
else{goto _LL171;}_LL171: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_DoubleType:
0){_LL279: _tmp116=((struct Cyc_Absyn_DoubleType_struct*)_tmp7D)->f1;goto _LL172;}
else{goto _LL173;}_LL173: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_SizeofType:
0){_LL281: _tmp118=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp7D)->f1;goto
_LL174;}else{goto _LL175;}_LL175: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_TupleType:
0){_LL283: _tmp11A=((struct Cyc_Absyn_TupleType_struct*)_tmp7D)->f1;goto _LL176;}
else{goto _LL177;}_LL177: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_StructType:
0){_LL287: _tmp11E=((struct Cyc_Absyn_StructType_struct*)_tmp7D)->f1;if(_tmp11E == 
0){goto _LL285;}else{goto _LL179;}_LL285: _tmp11C=((struct Cyc_Absyn_StructType_struct*)
_tmp7D)->f2;goto _LL178;}else{goto _LL179;}_LL179: if((unsigned int)_tmp7D > 3?*((
int*)_tmp7D)== Cyc_Absyn_StructType: 0){_LL291: _tmp122=((struct Cyc_Absyn_StructType_struct*)
_tmp7D)->f1;goto _LL289;_LL289: _tmp120=((struct Cyc_Absyn_StructType_struct*)
_tmp7D)->f2;goto _LL180;}else{goto _LL181;}_LL181: if((unsigned int)_tmp7D > 3?*((
int*)_tmp7D)== Cyc_Absyn_UnionType: 0){_LL295: _tmp126=((struct Cyc_Absyn_UnionType_struct*)
_tmp7D)->f1;if(_tmp126 == 0){goto _LL293;}else{goto _LL183;}_LL293: _tmp124=((struct
Cyc_Absyn_UnionType_struct*)_tmp7D)->f2;goto _LL182;}else{goto _LL183;}_LL183: if((
unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_UnionType: 0){_LL299: _tmp12A=((
struct Cyc_Absyn_UnionType_struct*)_tmp7D)->f1;goto _LL297;_LL297: _tmp128=((struct
Cyc_Absyn_UnionType_struct*)_tmp7D)->f2;goto _LL184;}else{goto _LL185;}_LL185: if((
unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_AnonStructType: 0){_LL301:
_tmp12C=((struct Cyc_Absyn_AnonStructType_struct*)_tmp7D)->f1;goto _LL186;}else{
goto _LL187;}_LL187: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_AnonUnionType:
0){_LL303: _tmp12E=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp7D)->f1;goto _LL188;}
else{goto _LL189;}_LL189: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_TypedefType:
0){_LL309: _tmp134=((struct Cyc_Absyn_TypedefType_struct*)_tmp7D)->f1;goto _LL307;
_LL307: _tmp132=((struct Cyc_Absyn_TypedefType_struct*)_tmp7D)->f2;goto _LL305;
_LL305: _tmp130=((struct Cyc_Absyn_TypedefType_struct*)_tmp7D)->f3;goto _LL190;}
else{goto _LL191;}_LL191: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_RgnHandleType:
0){_LL311: _tmp136=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp7D)->f1;
goto _LL192;}else{goto _LL193;}_LL193: if(_tmp7D == (void*)Cyc_Absyn_HeapRgn){goto
_LL194;}else{goto _LL195;}_LL195: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_AccessEff:
0){goto _LL196;}else{goto _LL197;}_LL197: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)
== Cyc_Absyn_RgnsEff: 0){goto _LL198;}else{goto _LL199;}_LL199: if((unsigned int)
_tmp7D > 3?*((int*)_tmp7D)== Cyc_Absyn_JoinEff: 0){goto _LL200;}else{goto _LL126;}
_LL128: return;_LL130: return;_LL132: return;_LL134: Cyc_Dump_dump_alpha(_tag_arr("void",
sizeof(unsigned char),5));return;_LL136: Cyc_Dump_dump_alphastr(_tmpC9->name);
return;_LL138: Cyc_Dump_dump_char((int)'%');if(_tmpCF == 0){Cyc_Dump_dump_char((
int)'?');}else{Cyc_Dump_dumpkind((void*)_tmpCF->v);}Cyc_Dump_dump_symbol((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp139;_tmp139.tag=Cyc_Std_Int_pa;
_tmp139.f1=(int)((unsigned int)_tmpCB);{void*_tmp138[1]={& _tmp139};Cyc_Std_aprintf(
_tag_arr("(%d)",sizeof(unsigned char),5),_tag_arr(_tmp138,sizeof(void*),1));}}));
return;_LL140: Cyc_Dump_dumpntyp(_tmpD6);return;_LL142:{void*_tmp13A=_tmpE0;
struct Cyc_Absyn_UnknownTunionInfo _tmp140;int _tmp142;struct _tuple1*_tmp144;struct
Cyc_Absyn_Tuniondecl**_tmp146;struct Cyc_Absyn_Tuniondecl*_tmp148;struct Cyc_Absyn_Tuniondecl
_tmp149;int _tmp14A;struct _tuple1*_tmp14C;_LL316: if(*((int*)_tmp13A)== Cyc_Absyn_UnknownTunion){
_LL321: _tmp140=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp13A)->f1;_LL325:
_tmp144=_tmp140.name;goto _LL323;_LL323: _tmp142=_tmp140.is_xtunion;goto _LL317;}
else{goto _LL318;}_LL318: if(*((int*)_tmp13A)== Cyc_Absyn_KnownTunion){_LL327:
_tmp146=((struct Cyc_Absyn_KnownTunion_struct*)_tmp13A)->f1;_tmp148=*_tmp146;
_tmp149=*_tmp148;_LL333: _tmp14C=_tmp149.name;goto _LL331;_LL331: _tmp14A=_tmp149.is_xtunion;
goto _LL319;}else{goto _LL315;}_LL317: _tmp14C=_tmp144;_tmp14A=_tmp142;goto _LL319;
_LL319: if(_tmp14A){Cyc_Dump_dump_alpha(_tag_arr("xtunion",sizeof(unsigned char),
8));}else{Cyc_Dump_dump_alpha(_tag_arr("tunion",sizeof(unsigned char),7));}{void*
_tmp14E=Cyc_Tcutil_compress(_tmpDC);_LL336: if(_tmp14E == (void*)Cyc_Absyn_HeapRgn){
goto _LL337;}else{goto _LL338;}_LL338: goto _LL339;_LL337: goto _LL335;_LL339: Cyc_Dump_dumptyp(
_tmpDC);goto _LL335;_LL335:;}Cyc_Dump_dumpqvar(_tmp14C);Cyc_Dump_dumptps(_tmpDE);
goto _LL315;_LL315:;}goto _LL126;_LL144:{void*_tmp154=_tmpE6;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp15A;int _tmp15C;struct _tuple1*_tmp15E;struct _tuple1*_tmp160;struct Cyc_Absyn_Tunionfield*
_tmp162;struct Cyc_Absyn_Tunionfield _tmp164;struct _tuple1*_tmp165;struct Cyc_Absyn_Tuniondecl*
_tmp167;struct Cyc_Absyn_Tuniondecl _tmp169;int _tmp16A;struct _tuple1*_tmp16C;
_LL342: if(*((int*)_tmp154)== Cyc_Absyn_UnknownTunionfield){_LL347: _tmp15A=((
struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp154)->f1;_LL353: _tmp160=_tmp15A.tunion_name;
goto _LL351;_LL351: _tmp15E=_tmp15A.field_name;goto _LL349;_LL349: _tmp15C=_tmp15A.is_xtunion;
goto _LL343;}else{goto _LL344;}_LL344: if(*((int*)_tmp154)== Cyc_Absyn_KnownTunionfield){
_LL360: _tmp167=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp154)->f1;_tmp169=*
_tmp167;_LL365: _tmp16C=_tmp169.name;goto _LL363;_LL363: _tmp16A=_tmp169.is_xtunion;
goto _LL355;_LL355: _tmp162=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp154)->f2;
_tmp164=*_tmp162;_LL358: _tmp165=_tmp164.name;goto _LL345;}else{goto _LL341;}_LL343:
_tmp16C=_tmp160;_tmp16A=_tmp15C;_tmp165=_tmp15E;goto _LL345;_LL345: if(_tmp16A){
Cyc_Dump_dump_alpha(_tag_arr("xtunion",sizeof(unsigned char),8));}else{Cyc_Dump_dump_alpha(
_tag_arr("tunion",sizeof(unsigned char),7));}Cyc_Dump_dumpqvar(_tmp16C);Cyc_Dump_dump_char((
int)'.');Cyc_Dump_dumpqvar(_tmp165);Cyc_Dump_dumptps(_tmpE4);goto _LL341;_LL341:;}
goto _LL126;_LL146: Cyc_Dump_dump_alpha(_tag_arr("enum",sizeof(unsigned char),5));
Cyc_Dump_dumpqvar(_tmpE8);return;_LL148: Cyc_Dump_dump_alpha(_tag_arr("enum",
sizeof(unsigned char),5));Cyc_Dump_dump_symbol(_tag_arr(" ",sizeof(unsigned char),
2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(_tag_arr("\\\\\n~~",sizeof(
unsigned char),6));Cyc_Dump_dumpenumfields(_tmpEA);Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",
sizeof(unsigned char),4));Cyc_Dump_dump_rb();return;_LL150: Cyc_Dump_dump_alpha(
_tag_arr("int",sizeof(unsigned char),4));return;_LL152: Cyc_Dump_dump_alpha(
_tag_arr("signed char",sizeof(unsigned char),12));return;_LL154: Cyc_Dump_dump_alpha(
_tag_arr("unsigned char",sizeof(unsigned char),14));return;_LL156: Cyc_Dump_dump_alpha(
_tag_arr("short",sizeof(unsigned char),6));return;_LL158: Cyc_Dump_dump_alpha(
_tag_arr("unsigned short",sizeof(unsigned char),15));return;_LL160: Cyc_Dump_dump_alpha(
_tag_arr("unsigned int",sizeof(unsigned char),13));return;_LL162: Cyc_Dump_dump_alpha(
_tag_arr("__int64",sizeof(unsigned char),8));return;_LL164: Cyc_Dump_dump_alpha(
_tag_arr("long long",sizeof(unsigned char),10));return;_LL166: Cyc_Dump_dump_alpha(
_tag_arr("unsigned __int64",sizeof(unsigned char),17));return;_LL168: Cyc_Dump_dump_alpha(
_tag_arr("unsigned long long",sizeof(unsigned char),19));return;_LL170: Cyc_Dump_dump_alpha(
_tag_arr("float",sizeof(unsigned char),6));return;_LL172: if(_tmp116){Cyc_Dump_dump_alpha(
_tag_arr("long double",sizeof(unsigned char),12));}else{Cyc_Dump_dump_alpha(
_tag_arr("double",sizeof(unsigned char),7));}return;_LL174: Cyc_Dump_dump_alpha(
_tag_arr("sizeof_t<",sizeof(unsigned char),10));Cyc_Dump_dumptyp(_tmp118);Cyc_Dump_dump_alpha(
_tag_arr(">",sizeof(unsigned char),2));return;_LL176: Cyc_Dump_dump_symbol(
_tag_arr("\\$",sizeof(unsigned char),3));Cyc_Dump_dumpargs(_tmp11A);return;
_LL178: Cyc_Dump_dump_alpha(_tag_arr("struct",sizeof(unsigned char),7));Cyc_Dump_dumptps(
_tmp11C);return;_LL180: Cyc_Dump_dump_alpha(_tag_arr("struct",sizeof(
unsigned char),7));Cyc_Dump_dumpqvar((struct _tuple1*)_check_null(_tmp122));Cyc_Dump_dumptps(
_tmp120);return;_LL182: Cyc_Dump_dump_alpha(_tag_arr("union",sizeof(unsigned char),
6));Cyc_Dump_dumptps(_tmp124);return;_LL184: Cyc_Dump_dump_alpha(_tag_arr("union",
sizeof(unsigned char),6));Cyc_Dump_dumpqvar((struct _tuple1*)_check_null(_tmp12A));
Cyc_Dump_dumptps(_tmp128);return;_LL186: Cyc_Dump_dump_alpha(_tag_arr("struct",
sizeof(unsigned char),7));Cyc_Dump_dump_lb();Cyc_Dump_dumpstructfields(_tmp12C);
Cyc_Dump_dump_rb();return;_LL188: Cyc_Dump_dump_alpha(_tag_arr("union",sizeof(
unsigned char),6));Cyc_Dump_dump_lb();Cyc_Dump_dumpstructfields(_tmp12E);Cyc_Dump_dump_rb();
return;_LL190:(Cyc_Dump_dumpqvar(_tmp134),Cyc_Dump_dumptps(_tmp132));return;
_LL192: Cyc_Dump_dumprgn(_tmp136);return;_LL194: return;_LL196: return;_LL198:
return;_LL200: return;_LL126:;}void Cyc_Dump_dumpvaropt(struct Cyc_Core_Opt*vo){if(
vo != 0){Cyc_Dump_dump_alphastr((struct _tagged_arr*)vo->v);}}void Cyc_Dump_dumpfunarg(
struct _tuple2*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),
struct Cyc_Core_Opt*))Cyc_Dump_dumptqtd)((*t).f2,(*t).f3,Cyc_Dump_dumpvaropt,(*t).f1);}
void Cyc_Dump_dump_rgncmp(struct _tuple0*cmp){struct _tuple0 _tmp170;void*_tmp171;
void*_tmp173;struct _tuple0*_tmp16E=cmp;_tmp170=*_tmp16E;_LL372: _tmp173=_tmp170.f1;
goto _LL370;_LL370: _tmp171=_tmp170.f2;goto _LL367;_LL367: Cyc_Dump_dumptyp(_tmp173);
Cyc_Dump_dump_char((int)'<');Cyc_Dump_dumptyp(_tmp171);}void Cyc_Dump_dump_rgnpo(
struct Cyc_List_List*rgn_po){((void(*)(void(*f)(struct _tuple0*),struct Cyc_List_List*
l,struct _tagged_arr sep))Cyc_Dump_dump_sep)(Cyc_Dump_dump_rgncmp,rgn_po,_tag_arr(",",
sizeof(unsigned char),2));}void Cyc_Dump_dumpfunargs(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po){Cyc_Dump_dump_char((int)'(');for(0;args != 0;args=
args->tl){Cyc_Dump_dumpfunarg((struct _tuple2*)args->hd);if((args->tl != 0? 1:
c_varargs)? 1: cyc_varargs != 0){Cyc_Dump_dump_symbol(_tag_arr(", ",sizeof(
unsigned char),3));}}if(c_varargs){Cyc_Dump_dump_symbol(_tag_arr("...",sizeof(
unsigned char),4));}else{if(cyc_varargs != 0){struct _tuple2*_tmp175=({struct
_tuple2*_tmp176=_cycalloc(sizeof(struct _tuple2));_tmp176->f1=cyc_varargs->name;
_tmp176->f2=cyc_varargs->tq;_tmp176->f3=(void*)cyc_varargs->type;_tmp176;});Cyc_Dump_dump_symbol(
_tag_arr("...",sizeof(unsigned char),4));if(cyc_varargs->inject){Cyc_Dump_dump_alpha(
_tag_arr(" inject",sizeof(unsigned char),8));}Cyc_Dump_dumpfunarg(_tmp175);}}if(
effopt != 0){Cyc_Dump_dump_semi();Cyc_Dump_dumpeff((void*)effopt->v);}if(rgn_po != 
0){Cyc_Dump_dump_char((int)':');Cyc_Dump_dump_rgnpo(rgn_po);}Cyc_Dump_dump_char((
int)')');}void Cyc_Dump_dumptyp(void*t){((void(*)(struct Cyc_Absyn_Tqual,void*,
void(*f)(int),int))Cyc_Dump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp177;_tmp177.q_const=
0;_tmp177.q_volatile=0;_tmp177.q_restrict=0;_tmp177;}),t,(void(*)(int x))Cyc_Dump_ignore,
0);}void Cyc_Dump_dumpdesignator(void*d){void*_tmp178=d;struct Cyc_Absyn_Exp*
_tmp17E;struct _tagged_arr*_tmp180;_LL378: if(*((int*)_tmp178)== Cyc_Absyn_ArrayElement){
_LL383: _tmp17E=((struct Cyc_Absyn_ArrayElement_struct*)_tmp178)->f1;goto _LL379;}
else{goto _LL380;}_LL380: if(*((int*)_tmp178)== Cyc_Absyn_FieldName){_LL385:
_tmp180=((struct Cyc_Absyn_FieldName_struct*)_tmp178)->f1;goto _LL381;}else{goto
_LL377;}_LL379: Cyc_Dump_dump_symbol(_tag_arr(".[",sizeof(unsigned char),3));Cyc_Dump_dumpexp(
_tmp17E);Cyc_Dump_dump_char((int)']');goto _LL377;_LL381: Cyc_Dump_dump_char((int)'.');
Cyc_Dump_dump_alpha(*_tmp180);goto _LL377;_LL377:;}struct _tuple13{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};void Cyc_Dump_dumpde(struct _tuple13*de){Cyc_Dump_egroup(
Cyc_Dump_dumpdesignator,(*de).f1,_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",
sizeof(unsigned char),2),_tag_arr("=",sizeof(unsigned char),2));Cyc_Dump_dumpexp((*
de).f2);}void Cyc_Dump_dumpexps_prec(int inprec,struct Cyc_List_List*es){((void(*)(
void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group_c)(
Cyc_Dump_dumpexp_prec,inprec,es,_tag_arr("",sizeof(unsigned char),1),_tag_arr("",
sizeof(unsigned char),1),_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Dump_dumpexp_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= 
myprec){Cyc_Dump_dump_char((int)'(');}{void*_tmp182=(void*)e->r;void*_tmp1E6;
unsigned char _tmp1E8;void*_tmp1EA;void*_tmp1EC;short _tmp1EE;void*_tmp1F0;void*
_tmp1F2;int _tmp1F4;void*_tmp1F6;void*_tmp1F8;int _tmp1FA;void*_tmp1FC;void*
_tmp1FE;long long _tmp200;void*_tmp202;void*_tmp204;struct _tagged_arr _tmp206;void*
_tmp208;void*_tmp20A;struct _tagged_arr _tmp20C;struct _tuple1*_tmp20E;struct
_tuple1*_tmp210;struct Cyc_List_List*_tmp212;void*_tmp214;struct Cyc_Absyn_Exp*
_tmp216;struct Cyc_Core_Opt*_tmp218;struct Cyc_Absyn_Exp*_tmp21A;void*_tmp21C;
struct Cyc_Absyn_Exp*_tmp21E;void*_tmp220;struct Cyc_Absyn_Exp*_tmp222;void*
_tmp224;struct Cyc_Absyn_Exp*_tmp226;void*_tmp228;struct Cyc_Absyn_Exp*_tmp22A;
struct Cyc_Absyn_Exp*_tmp22C;struct Cyc_Absyn_Exp*_tmp22E;struct Cyc_Absyn_Exp*
_tmp230;struct Cyc_Absyn_Exp*_tmp232;struct Cyc_Absyn_Exp*_tmp234;struct Cyc_List_List*
_tmp236;struct Cyc_Absyn_Exp*_tmp238;struct Cyc_List_List*_tmp23A;struct Cyc_Absyn_Exp*
_tmp23C;struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_Absyn_Exp*_tmp240;struct Cyc_Absyn_Exp*
_tmp242;struct Cyc_Absyn_Exp*_tmp244;void*_tmp246;struct Cyc_Absyn_Exp*_tmp248;
struct Cyc_Absyn_Exp*_tmp24A;struct Cyc_Absyn_Exp*_tmp24C;void*_tmp24E;struct Cyc_Absyn_Exp*
_tmp250;void*_tmp252;struct _tagged_arr*_tmp254;void*_tmp256;void*_tmp258;
unsigned int _tmp25A;void*_tmp25C;void*_tmp25E;struct Cyc_List_List*_tmp260;struct
Cyc_Absyn_Exp*_tmp262;struct _tagged_arr*_tmp264;struct Cyc_Absyn_Exp*_tmp266;
struct _tagged_arr*_tmp268;struct Cyc_Absyn_Exp*_tmp26A;struct Cyc_Absyn_Exp*
_tmp26C;struct Cyc_Absyn_Exp*_tmp26E;struct Cyc_List_List*_tmp270;struct Cyc_List_List*
_tmp272;struct _tuple2*_tmp274;struct Cyc_List_List*_tmp276;struct Cyc_Absyn_Exp*
_tmp278;struct Cyc_Absyn_Exp*_tmp27A;struct Cyc_Absyn_Vardecl*_tmp27C;struct Cyc_List_List*
_tmp27E;struct _tuple1*_tmp280;struct Cyc_List_List*_tmp282;struct Cyc_Absyn_Tunionfield*
_tmp284;struct Cyc_List_List*_tmp286;struct _tuple1*_tmp288;struct _tuple1*_tmp28A;
struct Cyc_Absyn_MallocInfo _tmp28C;struct Cyc_Absyn_Exp*_tmp28E;void**_tmp290;
struct Cyc_Absyn_Exp*_tmp292;int _tmp294;struct Cyc_List_List*_tmp296;struct Cyc_Core_Opt*
_tmp298;struct Cyc_Absyn_Stmt*_tmp29A;struct Cyc_Absyn_Fndecl*_tmp29C;struct Cyc_Absyn_Exp*
_tmp29E;_LL388: if(*((int*)_tmp182)== Cyc_Absyn_Const_e){_LL487: _tmp1E6=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp182)->f1;if((unsigned int)_tmp1E6 > 1?*((int*)
_tmp1E6)== Cyc_Absyn_Char_c: 0){_LL491: _tmp1EA=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp1E6)->f1;goto _LL489;_LL489: _tmp1E8=((struct Cyc_Absyn_Char_c_struct*)_tmp1E6)->f2;
goto _LL389;}else{goto _LL390;}}else{goto _LL390;}_LL390: if(*((int*)_tmp182)== Cyc_Absyn_Const_e){
_LL493: _tmp1EC=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp182)->f1;if((
unsigned int)_tmp1EC > 1?*((int*)_tmp1EC)== Cyc_Absyn_Short_c: 0){_LL497: _tmp1F0=(
void*)((struct Cyc_Absyn_Short_c_struct*)_tmp1EC)->f1;goto _LL495;_LL495: _tmp1EE=((
struct Cyc_Absyn_Short_c_struct*)_tmp1EC)->f2;goto _LL391;}else{goto _LL392;}}else{
goto _LL392;}_LL392: if(*((int*)_tmp182)== Cyc_Absyn_Const_e){_LL499: _tmp1F2=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp182)->f1;if((unsigned int)_tmp1F2 > 1?*((int*)
_tmp1F2)== Cyc_Absyn_Int_c: 0){_LL503: _tmp1F6=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp1F2)->f1;if(_tmp1F6 == (void*)Cyc_Absyn_Signed){goto _LL501;}else{goto _LL394;}
_LL501: _tmp1F4=((struct Cyc_Absyn_Int_c_struct*)_tmp1F2)->f2;goto _LL393;}else{
goto _LL394;}}else{goto _LL394;}_LL394: if(*((int*)_tmp182)== Cyc_Absyn_Const_e){
_LL505: _tmp1F8=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp182)->f1;if((
unsigned int)_tmp1F8 > 1?*((int*)_tmp1F8)== Cyc_Absyn_Int_c: 0){_LL509: _tmp1FC=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp1F8)->f1;if(_tmp1FC == (void*)Cyc_Absyn_Unsigned){
goto _LL507;}else{goto _LL396;}_LL507: _tmp1FA=((struct Cyc_Absyn_Int_c_struct*)
_tmp1F8)->f2;goto _LL395;}else{goto _LL396;}}else{goto _LL396;}_LL396: if(*((int*)
_tmp182)== Cyc_Absyn_Const_e){_LL511: _tmp1FE=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp182)->f1;if((unsigned int)_tmp1FE > 1?*((int*)_tmp1FE)== Cyc_Absyn_LongLong_c:
0){_LL515: _tmp202=(void*)((struct Cyc_Absyn_LongLong_c_struct*)_tmp1FE)->f1;goto
_LL513;_LL513: _tmp200=((struct Cyc_Absyn_LongLong_c_struct*)_tmp1FE)->f2;goto
_LL397;}else{goto _LL398;}}else{goto _LL398;}_LL398: if(*((int*)_tmp182)== Cyc_Absyn_Const_e){
_LL517: _tmp204=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp182)->f1;if((
unsigned int)_tmp204 > 1?*((int*)_tmp204)== Cyc_Absyn_Float_c: 0){_LL519: _tmp206=((
struct Cyc_Absyn_Float_c_struct*)_tmp204)->f1;goto _LL399;}else{goto _LL400;}}else{
goto _LL400;}_LL400: if(*((int*)_tmp182)== Cyc_Absyn_Const_e){_LL521: _tmp208=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp182)->f1;if(_tmp208 == (void*)Cyc_Absyn_Null_c){
goto _LL401;}else{goto _LL402;}}else{goto _LL402;}_LL402: if(*((int*)_tmp182)== Cyc_Absyn_Const_e){
_LL523: _tmp20A=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp182)->f1;if((
unsigned int)_tmp20A > 1?*((int*)_tmp20A)== Cyc_Absyn_String_c: 0){_LL525: _tmp20C=((
struct Cyc_Absyn_String_c_struct*)_tmp20A)->f1;goto _LL403;}else{goto _LL404;}}
else{goto _LL404;}_LL404: if(*((int*)_tmp182)== Cyc_Absyn_UnknownId_e){_LL527:
_tmp20E=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp182)->f1;goto _LL405;}else{goto
_LL406;}_LL406: if(*((int*)_tmp182)== Cyc_Absyn_Var_e){_LL529: _tmp210=((struct Cyc_Absyn_Var_e_struct*)
_tmp182)->f1;goto _LL407;}else{goto _LL408;}_LL408: if(*((int*)_tmp182)== Cyc_Absyn_Primop_e){
_LL533: _tmp214=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp182)->f1;goto _LL531;
_LL531: _tmp212=((struct Cyc_Absyn_Primop_e_struct*)_tmp182)->f2;goto _LL409;}else{
goto _LL410;}_LL410: if(*((int*)_tmp182)== Cyc_Absyn_AssignOp_e){_LL539: _tmp21A=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp182)->f1;goto _LL537;_LL537: _tmp218=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp182)->f2;goto _LL535;_LL535: _tmp216=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp182)->f3;goto _LL411;}else{goto _LL412;}
_LL412: if(*((int*)_tmp182)== Cyc_Absyn_Increment_e){_LL543: _tmp21E=((struct Cyc_Absyn_Increment_e_struct*)
_tmp182)->f1;goto _LL541;_LL541: _tmp21C=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp182)->f2;if(_tmp21C == (void*)Cyc_Absyn_PreInc){goto _LL413;}else{goto _LL414;}}
else{goto _LL414;}_LL414: if(*((int*)_tmp182)== Cyc_Absyn_Increment_e){_LL547:
_tmp222=((struct Cyc_Absyn_Increment_e_struct*)_tmp182)->f1;goto _LL545;_LL545:
_tmp220=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp182)->f2;if(_tmp220 == (
void*)Cyc_Absyn_PreDec){goto _LL415;}else{goto _LL416;}}else{goto _LL416;}_LL416:
if(*((int*)_tmp182)== Cyc_Absyn_Increment_e){_LL551: _tmp226=((struct Cyc_Absyn_Increment_e_struct*)
_tmp182)->f1;goto _LL549;_LL549: _tmp224=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp182)->f2;if(_tmp224 == (void*)Cyc_Absyn_PostInc){goto _LL417;}else{goto _LL418;}}
else{goto _LL418;}_LL418: if(*((int*)_tmp182)== Cyc_Absyn_Increment_e){_LL555:
_tmp22A=((struct Cyc_Absyn_Increment_e_struct*)_tmp182)->f1;goto _LL553;_LL553:
_tmp228=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp182)->f2;if(_tmp228 == (
void*)Cyc_Absyn_PostDec){goto _LL419;}else{goto _LL420;}}else{goto _LL420;}_LL420:
if(*((int*)_tmp182)== Cyc_Absyn_Conditional_e){_LL561: _tmp230=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp182)->f1;goto _LL559;_LL559: _tmp22E=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp182)->f2;goto _LL557;_LL557: _tmp22C=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp182)->f3;goto _LL421;}else{goto _LL422;}_LL422: if(*((int*)_tmp182)== Cyc_Absyn_SeqExp_e){
_LL565: _tmp234=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp182)->f1;goto _LL563;_LL563:
_tmp232=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp182)->f2;goto _LL423;}else{goto
_LL424;}_LL424: if(*((int*)_tmp182)== Cyc_Absyn_UnknownCall_e){_LL569: _tmp238=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp182)->f1;goto _LL567;_LL567: _tmp236=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp182)->f2;goto _LL425;}else{goto _LL426;}
_LL426: if(*((int*)_tmp182)== Cyc_Absyn_FnCall_e){_LL573: _tmp23C=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp182)->f1;goto _LL571;_LL571: _tmp23A=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp182)->f2;goto _LL427;}else{goto _LL428;}_LL428: if(*((int*)_tmp182)== Cyc_Absyn_Throw_e){
_LL575: _tmp23E=((struct Cyc_Absyn_Throw_e_struct*)_tmp182)->f1;goto _LL429;}else{
goto _LL430;}_LL430: if(*((int*)_tmp182)== Cyc_Absyn_NoInstantiate_e){_LL577:
_tmp240=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp182)->f1;goto _LL431;}else{
goto _LL432;}_LL432: if(*((int*)_tmp182)== Cyc_Absyn_Instantiate_e){_LL579: _tmp242=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp182)->f1;goto _LL433;}else{goto _LL434;}
_LL434: if(*((int*)_tmp182)== Cyc_Absyn_Cast_e){_LL583: _tmp246=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp182)->f1;goto _LL581;_LL581: _tmp244=((struct Cyc_Absyn_Cast_e_struct*)_tmp182)->f2;
goto _LL435;}else{goto _LL436;}_LL436: if(*((int*)_tmp182)== Cyc_Absyn_Address_e){
_LL585: _tmp248=((struct Cyc_Absyn_Address_e_struct*)_tmp182)->f1;goto _LL437;}
else{goto _LL438;}_LL438: if(*((int*)_tmp182)== Cyc_Absyn_New_e){_LL589: _tmp24C=((
struct Cyc_Absyn_New_e_struct*)_tmp182)->f1;goto _LL587;_LL587: _tmp24A=((struct Cyc_Absyn_New_e_struct*)
_tmp182)->f2;goto _LL439;}else{goto _LL440;}_LL440: if(*((int*)_tmp182)== Cyc_Absyn_Sizeoftyp_e){
_LL591: _tmp24E=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp182)->f1;goto
_LL441;}else{goto _LL442;}_LL442: if(*((int*)_tmp182)== Cyc_Absyn_Sizeofexp_e){
_LL593: _tmp250=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp182)->f1;goto _LL443;}
else{goto _LL444;}_LL444: if(*((int*)_tmp182)== Cyc_Absyn_Offsetof_e){_LL599:
_tmp256=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp182)->f1;goto _LL595;
_LL595: _tmp252=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp182)->f2;if(*((
int*)_tmp252)== Cyc_Absyn_StructField){_LL597: _tmp254=((struct Cyc_Absyn_StructField_struct*)
_tmp252)->f1;goto _LL445;}else{goto _LL446;}}else{goto _LL446;}_LL446: if(*((int*)
_tmp182)== Cyc_Absyn_Offsetof_e){_LL605: _tmp25C=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp182)->f1;goto _LL601;_LL601: _tmp258=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp182)->f2;if(*((int*)_tmp258)== Cyc_Absyn_TupleIndex){_LL603: _tmp25A=((struct
Cyc_Absyn_TupleIndex_struct*)_tmp258)->f1;goto _LL447;}else{goto _LL448;}}else{
goto _LL448;}_LL448: if(*((int*)_tmp182)== Cyc_Absyn_Gentyp_e){_LL609: _tmp260=((
struct Cyc_Absyn_Gentyp_e_struct*)_tmp182)->f1;goto _LL607;_LL607: _tmp25E=(void*)((
struct Cyc_Absyn_Gentyp_e_struct*)_tmp182)->f2;goto _LL449;}else{goto _LL450;}
_LL450: if(*((int*)_tmp182)== Cyc_Absyn_Deref_e){_LL611: _tmp262=((struct Cyc_Absyn_Deref_e_struct*)
_tmp182)->f1;goto _LL451;}else{goto _LL452;}_LL452: if(*((int*)_tmp182)== Cyc_Absyn_StructMember_e){
_LL615: _tmp266=((struct Cyc_Absyn_StructMember_e_struct*)_tmp182)->f1;goto _LL613;
_LL613: _tmp264=((struct Cyc_Absyn_StructMember_e_struct*)_tmp182)->f2;goto _LL453;}
else{goto _LL454;}_LL454: if(*((int*)_tmp182)== Cyc_Absyn_StructArrow_e){_LL619:
_tmp26A=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp182)->f1;goto _LL617;_LL617:
_tmp268=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp182)->f2;goto _LL455;}else{
goto _LL456;}_LL456: if(*((int*)_tmp182)== Cyc_Absyn_Subscript_e){_LL623: _tmp26E=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp182)->f1;goto _LL621;_LL621: _tmp26C=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp182)->f2;goto _LL457;}else{goto _LL458;}
_LL458: if(*((int*)_tmp182)== Cyc_Absyn_Tuple_e){_LL625: _tmp270=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp182)->f1;goto _LL459;}else{goto _LL460;}_LL460: if(*((int*)_tmp182)== Cyc_Absyn_CompoundLit_e){
_LL629: _tmp274=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp182)->f1;goto _LL627;
_LL627: _tmp272=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp182)->f2;goto _LL461;}
else{goto _LL462;}_LL462: if(*((int*)_tmp182)== Cyc_Absyn_Array_e){_LL631: _tmp276=((
struct Cyc_Absyn_Array_e_struct*)_tmp182)->f1;goto _LL463;}else{goto _LL464;}_LL464:
if(*((int*)_tmp182)== Cyc_Absyn_Comprehension_e){_LL637: _tmp27C=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp182)->f1;goto _LL635;_LL635: _tmp27A=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp182)->f2;goto _LL633;_LL633: _tmp278=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp182)->f3;goto _LL465;}else{goto _LL466;}_LL466: if(*((int*)_tmp182)== Cyc_Absyn_Struct_e){
_LL641: _tmp280=((struct Cyc_Absyn_Struct_e_struct*)_tmp182)->f1;goto _LL639;_LL639:
_tmp27E=((struct Cyc_Absyn_Struct_e_struct*)_tmp182)->f3;goto _LL467;}else{goto
_LL468;}_LL468: if(*((int*)_tmp182)== Cyc_Absyn_AnonStruct_e){_LL643: _tmp282=((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp182)->f2;goto _LL469;}else{goto _LL470;}
_LL470: if(*((int*)_tmp182)== Cyc_Absyn_Tunion_e){_LL647: _tmp286=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp182)->f3;goto _LL645;_LL645: _tmp284=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp182)->f5;goto _LL471;}else{goto _LL472;}_LL472: if(*((int*)_tmp182)== Cyc_Absyn_Enum_e){
_LL649: _tmp288=((struct Cyc_Absyn_Enum_e_struct*)_tmp182)->f1;goto _LL473;}else{
goto _LL474;}_LL474: if(*((int*)_tmp182)== Cyc_Absyn_AnonEnum_e){_LL651: _tmp28A=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp182)->f1;goto _LL475;}else{goto _LL476;}
_LL476: if(*((int*)_tmp182)== Cyc_Absyn_Malloc_e){_LL653: _tmp28C=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp182)->f1;_LL661: _tmp294=_tmp28C.is_calloc;goto _LL659;_LL659: _tmp292=_tmp28C.rgn;
goto _LL657;_LL657: _tmp290=_tmp28C.elt_type;goto _LL655;_LL655: _tmp28E=_tmp28C.num_elts;
goto _LL477;}else{goto _LL478;}_LL478: if(*((int*)_tmp182)== Cyc_Absyn_UnresolvedMem_e){
_LL665: _tmp298=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp182)->f1;goto _LL663;
_LL663: _tmp296=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp182)->f2;goto _LL479;}
else{goto _LL480;}_LL480: if(*((int*)_tmp182)== Cyc_Absyn_StmtExp_e){_LL667:
_tmp29A=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp182)->f1;goto _LL481;}else{goto
_LL482;}_LL482: if(*((int*)_tmp182)== Cyc_Absyn_Codegen_e){_LL669: _tmp29C=((
struct Cyc_Absyn_Codegen_e_struct*)_tmp182)->f1;goto _LL483;}else{goto _LL484;}
_LL484: if(*((int*)_tmp182)== Cyc_Absyn_Fill_e){_LL671: _tmp29E=((struct Cyc_Absyn_Fill_e_struct*)
_tmp182)->f1;goto _LL485;}else{goto _LL387;}_LL389: Cyc_Dump_dump_char((int)'\'');
Cyc_Dump_dump_alpha(Cyc_Absynpp_char_escape(_tmp1E8));Cyc_Dump_dump_char((int)'\'');
goto _LL387;_LL391: Cyc_Dump_dump_alpha((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp2A1;_tmp2A1.tag=Cyc_Std_Int_pa;_tmp2A1.f1=(int)((unsigned int)((int)_tmp1EE));{
void*_tmp2A0[1]={& _tmp2A1};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),
_tag_arr(_tmp2A0,sizeof(void*),1));}}));goto _LL387;_LL393: Cyc_Dump_dump_alpha((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp2A3;_tmp2A3.tag=Cyc_Std_Int_pa;
_tmp2A3.f1=(int)((unsigned int)_tmp1F4);{void*_tmp2A2[1]={& _tmp2A3};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp2A2,sizeof(void*),1));}}));
goto _LL387;_LL395: Cyc_Dump_dump_alpha((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp2A5;_tmp2A5.tag=Cyc_Std_Int_pa;_tmp2A5.f1=(int)((unsigned int)_tmp1FA);{void*
_tmp2A4[1]={& _tmp2A5};Cyc_Std_aprintf(_tag_arr("%du",sizeof(unsigned char),4),
_tag_arr(_tmp2A4,sizeof(void*),1));}}));goto _LL387;_LL397: Cyc_Dump_dump_alpha(
_tag_arr("<<FIX LONG LONG CONSTANT>>",sizeof(unsigned char),27));goto _LL387;
_LL399: Cyc_Dump_dump_alpha(_tmp206);goto _LL387;_LL401: Cyc_Dump_dump_alpha(
_tag_arr("NULL",sizeof(unsigned char),5));goto _LL387;_LL403: Cyc_Dump_dump_char((
int)'"');Cyc_Dump_dump_alpha(Cyc_Absynpp_string_escape(_tmp20C));Cyc_Dump_dump_char((
int)'"');goto _LL387;_LL405: _tmp210=_tmp20E;goto _LL407;_LL407: Cyc_Dump_dumpqvar(
_tmp210);goto _LL387;_LL409: {struct _tagged_arr _tmp2A6=Cyc_Absynpp_prim2str(
_tmp214);switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp212)){case 1:
_LL679: if(_tmp214 == (void*)Cyc_Absyn_Size){Cyc_Dump_dumpexp_prec(myprec,(struct
Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp212))->hd);Cyc_Dump_dump_char((
int)'.');Cyc_Dump_dump_alpha(_tag_arr("size",sizeof(unsigned char),5));}else{Cyc_Dump_dump_symbol(
_tmp2A6);Cyc_Dump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp212))->hd);}break;case 2: _LL680: Cyc_Dump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp212))->hd);Cyc_Dump_dump_symbol(
_tmp2A6);Cyc_Dump_dump_char((int)' ');Cyc_Dump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp212->tl))->hd);break;default: _LL681:(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2AB=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp2AB[0]=({struct Cyc_Core_Failure_struct _tmp2AC;_tmp2AC.tag=Cyc_Core_Failure;
_tmp2AC.f1=_tag_arr("Absyndump -- Bad number of arguments to primop",sizeof(
unsigned char),47);_tmp2AC;});_tmp2AB;}));}goto _LL387;}_LL411: Cyc_Dump_dumpexp_prec(
myprec,_tmp21A);if(_tmp218 != 0){Cyc_Dump_dump_symbol(Cyc_Absynpp_prim2str((void*)
_tmp218->v));}Cyc_Dump_dump_symbol(_tag_arr("=",sizeof(unsigned char),2));Cyc_Dump_dumpexp_prec(
myprec,_tmp216);goto _LL387;_LL413: Cyc_Dump_dump_symbol(_tag_arr("++",sizeof(
unsigned char),3));Cyc_Dump_dumpexp_prec(myprec,_tmp21E);goto _LL387;_LL415: Cyc_Dump_dump_symbol(
_tag_arr("--",sizeof(unsigned char),3));Cyc_Dump_dumpexp_prec(myprec,_tmp222);
goto _LL387;_LL417: Cyc_Dump_dumpexp_prec(myprec,_tmp226);Cyc_Dump_dump_symbol(
_tag_arr("++",sizeof(unsigned char),3));goto _LL387;_LL419: Cyc_Dump_dumpexp_prec(
myprec,_tmp22A);Cyc_Dump_dump_symbol(_tag_arr("--",sizeof(unsigned char),3));
goto _LL387;_LL421: Cyc_Dump_dumpexp_prec(myprec,_tmp230);Cyc_Dump_dump_char((int)'?');
Cyc_Dump_dumpexp_prec(0,_tmp22E);Cyc_Dump_dump_char((int)':');Cyc_Dump_dumpexp_prec(
myprec,_tmp22C);goto _LL387;_LL423: Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumpexp_prec(
myprec,_tmp234);Cyc_Dump_dump_char((int)',');Cyc_Dump_dumpexp_prec(myprec,
_tmp232);Cyc_Dump_dump_char((int)')');goto _LL387;_LL425: _tmp23C=_tmp238;_tmp23A=
_tmp236;goto _LL427;_LL427: Cyc_Dump_dumpexp_prec(myprec,_tmp23C);Cyc_Dump_dump_char((
int)'(');Cyc_Dump_dumpexps_prec(20,_tmp23A);Cyc_Dump_dump_char((int)')');goto
_LL387;_LL429: Cyc_Dump_dump_alpha(_tag_arr("throw",sizeof(unsigned char),6));Cyc_Dump_dumpexp_prec(
myprec,_tmp23E);goto _LL387;_LL431: _tmp242=_tmp240;goto _LL433;_LL433: Cyc_Dump_dumpexp_prec(
inprec,_tmp242);goto _LL387;_LL435: Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(
_tmp246);Cyc_Dump_dump_char((int)')');Cyc_Dump_dumpexp_prec(myprec,_tmp244);goto
_LL387;_LL437: Cyc_Dump_dump_char((int)'&');Cyc_Dump_dumpexp_prec(myprec,_tmp248);
goto _LL387;_LL439: Cyc_Dump_dump_alpha(_tag_arr("new",sizeof(unsigned char),4));
Cyc_Dump_dumpexp_prec(myprec,_tmp24A);goto _LL387;_LL441: Cyc_Dump_dump_alpha(
_tag_arr("sizeof",sizeof(unsigned char),7));Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(
_tmp24E);Cyc_Dump_dump_char((int)')');goto _LL387;_LL443: Cyc_Dump_dump_alpha(
_tag_arr("sizeof",sizeof(unsigned char),7));Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumpexp_prec(
myprec,_tmp250);Cyc_Dump_dump_char((int)')');goto _LL387;_LL445: Cyc_Dump_dump_alpha(
_tag_arr("offsetof",sizeof(unsigned char),9));Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(
_tmp256);Cyc_Dump_dump_char((int)',');Cyc_Dump_dump_alpha(*_tmp254);Cyc_Dump_dump_char((
int)')');goto _LL387;_LL447: Cyc_Dump_dump_alpha(_tag_arr("offsetof(",sizeof(
unsigned char),10));Cyc_Dump_dumptyp(_tmp25C);Cyc_Dump_dump_char((int)',');Cyc_Dump_dump_alpha((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp2AE;_tmp2AE.tag=Cyc_Std_Int_pa;
_tmp2AE.f1=(int)_tmp25A;{void*_tmp2AD[1]={& _tmp2AE};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(unsigned char),3),_tag_arr(_tmp2AD,sizeof(void*),1));}}));Cyc_Dump_dump_char((
int)')');goto _LL387;_LL449: Cyc_Dump_dump_alpha(_tag_arr("__gen",sizeof(
unsigned char),6));Cyc_Dump_dumptvars(_tmp260);Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(
_tmp25E);Cyc_Dump_dump_char((int)')');goto _LL387;_LL451: Cyc_Dump_dump_char((int)'*');
Cyc_Dump_dumpexp_prec(myprec,_tmp262);goto _LL387;_LL453: Cyc_Dump_dumpexp_prec(
myprec,_tmp266);Cyc_Dump_dump_char((int)'.');Cyc_Dump_dump_alpha(*_tmp264);goto
_LL387;_LL455: Cyc_Dump_dumpexp_prec(myprec,_tmp26A);Cyc_Dump_dump_symbol(
_tag_arr("->",sizeof(unsigned char),3));Cyc_Dump_dump_alpha(*_tmp268);goto _LL387;
_LL457: Cyc_Dump_dumpexp_prec(myprec,_tmp26E);Cyc_Dump_dump_char((int)'[');Cyc_Dump_dumpexp(
_tmp26C);Cyc_Dump_dump_char((int)']');goto _LL387;_LL459: Cyc_Dump_dump_symbol(
_tag_arr("\\$(",sizeof(unsigned char),4));Cyc_Dump_dumpexps_prec(20,_tmp270);Cyc_Dump_dump_char((
int)')');goto _LL387;_LL461: Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp((*
_tmp274).f3);Cyc_Dump_dump_char((int)')');((void(*)(void(*f)(struct _tuple13*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmp272,_tag_arr("{\\lb}",
sizeof(unsigned char),6),_tag_arr("{\\rb}",sizeof(unsigned char),6),_tag_arr(",",
sizeof(unsigned char),2));goto _LL387;_LL463:((void(*)(void(*f)(struct _tuple13*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmp276,_tag_arr("{\\lb}",
sizeof(unsigned char),6),_tag_arr("{\\rb}",sizeof(unsigned char),6),_tag_arr(",",
sizeof(unsigned char),2));goto _LL387;_LL465: Cyc_Dump_dump_alpha(_tag_arr("new",
sizeof(unsigned char),4));Cyc_Dump_dump_char((int)'{');Cyc_Dump_dump_alpha(
_tag_arr("for",sizeof(unsigned char),4));Cyc_Dump_dump_alphastr((*_tmp27C->name).f2);
Cyc_Dump_dump_char((int)'<');Cyc_Dump_dumpexp(_tmp27A);Cyc_Dump_dump_char((int)':');
Cyc_Dump_dumpexp(_tmp278);Cyc_Dump_dump_char((int)'}');goto _LL387;_LL467: Cyc_Dump_dumpqvar(
_tmp280);((void(*)(void(*f)(struct _tuple13*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,
_tmp27E,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LL387;_LL469:((void(*)(void(*f)(
struct _tuple13*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr
end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmp282,_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LL387;_LL471: Cyc_Dump_dumpqvar(_tmp284->name);if(
_tmp286 != 0){((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(
Cyc_Dump_dumpexp,_tmp286,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}goto _LL387;
_LL473: Cyc_Dump_dumpqvar(_tmp288);goto _LL387;_LL475: Cyc_Dump_dumpqvar(_tmp28A);
goto _LL387;_LL477: if(_tmp292 != 0){if(_tmp294){Cyc_Dump_dump_alpha(_tag_arr("rcalloc",
sizeof(unsigned char),8));}else{Cyc_Dump_dump_alpha(_tag_arr("rmalloc",sizeof(
unsigned char),8));}Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmp292));Cyc_Dump_dump_symbol(_tag_arr(", ",sizeof(unsigned char),3));}
else{if(_tmp294){Cyc_Dump_dump_alpha(_tag_arr("calloc",sizeof(unsigned char),7));}
else{Cyc_Dump_dump_alpha(_tag_arr("malloc",sizeof(unsigned char),7));}Cyc_Dump_dump_char((
int)'(');}Cyc_Dump_dumpexp(_tmp28E);if(_tmp290 != 0){if(_tmp294){Cyc_Dump_dump_symbol(
_tag_arr(", ",sizeof(unsigned char),3));}else{Cyc_Dump_dump_symbol(_tag_arr(" * ",
sizeof(unsigned char),4));}Cyc_Dump_dump_alpha(_tag_arr("sizeof",sizeof(
unsigned char),7));Cyc_Dump_dump_char((int)'(');Cyc_Dump_dumptyp(*_tmp290);Cyc_Dump_dump_symbol(
_tag_arr(")",sizeof(unsigned char),2));}Cyc_Dump_dump_symbol(_tag_arr(")",
sizeof(unsigned char),2));goto _LL387;_LL479:((void(*)(void(*f)(struct _tuple13*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dumpde,_tmp296,_tag_arr("{",sizeof(
unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2));goto _LL387;_LL481: goto _LL387;_LL483: goto _LL387;_LL485: goto
_LL387;_LL387:;}if(inprec >= myprec){Cyc_Dump_dump_char((int)')');}}void Cyc_Dump_dumpexp(
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
struct Cyc_Absyn_Structfield _tmp2B1;struct Cyc_List_List*_tmp2B2;struct Cyc_Absyn_Exp*
_tmp2B4;void*_tmp2B6;struct Cyc_Absyn_Tqual _tmp2B8;struct _tagged_arr*_tmp2BA;
struct Cyc_Absyn_Structfield*_tmp2AF=(struct Cyc_Absyn_Structfield*)fields->hd;
_tmp2B1=*_tmp2AF;_LL699: _tmp2BA=_tmp2B1.name;goto _LL697;_LL697: _tmp2B8=_tmp2B1.tq;
goto _LL695;_LL695: _tmp2B6=(void*)_tmp2B1.type;goto _LL693;_LL693: _tmp2B4=_tmp2B1.width;
goto _LL691;_LL691: _tmp2B2=_tmp2B1.attributes;goto _LL688;_LL688:((void(*)(struct
Cyc_Absyn_Tqual,void*,void(*f)(struct _tagged_arr*),struct _tagged_arr*))Cyc_Dump_dumptqtd)(
_tmp2B8,_tmp2B6,Cyc_Dump_dump_alphastr,_tmp2BA);if(_tmp2B4 != 0){Cyc_Dump_dump_char((
int)':');Cyc_Dump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp2B4));}Cyc_Dump_dump_symbol(
_tag_arr(";\\\\\n",sizeof(unsigned char),5));}}}void Cyc_Dump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Dump_dumpqvar(td->name);Cyc_Dump_dumptvars(
td->tvs);}void Cyc_Dump_dump_boldtypedefname(struct Cyc_Absyn_Typedefdecl*td){Cyc_Dump_dump_boldqvar(
td->name);Cyc_Dump_dumptvars(td->tvs);}static void Cyc_Dump_dumpids(struct Cyc_List_List*
vds){for(0;vds != 0;vds=vds->tl){Cyc_Dump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
if(vds->tl != 0){Cyc_Dump_dump_char((int)',');}}}static void Cyc_Dump_dump_upperbound(
struct Cyc_Absyn_Exp*e){unsigned int i=Cyc_Evexp_eval_const_uint_exp(e);if(i != 1){
Cyc_Dump_dump_char((int)'{');Cyc_Dump_dumpexp(e);Cyc_Dump_dump_char((int)'}');}}
void Cyc_Dump_dumptms(struct Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){
f(a);return;}{void*_tmp2BC=(void*)tms->hd;struct Cyc_Absyn_Tqual _tmp2D2;void*
_tmp2D4;void*_tmp2D6;struct Cyc_Absyn_Exp*_tmp2D8;struct Cyc_Absyn_Tqual _tmp2DA;
void*_tmp2DC;void*_tmp2DE;struct Cyc_Absyn_Exp*_tmp2E0;struct Cyc_Absyn_Tqual
_tmp2E2;void*_tmp2E4;void*_tmp2E6;struct Cyc_Absyn_Tqual _tmp2E8;void*_tmp2EA;
struct Cyc_Absyn_Tvar*_tmp2EC;void*_tmp2EE;struct Cyc_Absyn_Exp*_tmp2F0;struct Cyc_Absyn_Tqual
_tmp2F2;void*_tmp2F4;struct Cyc_Absyn_Tvar*_tmp2F6;void*_tmp2F8;struct Cyc_Absyn_Exp*
_tmp2FA;struct Cyc_Absyn_Tqual _tmp2FC;void*_tmp2FE;struct Cyc_Absyn_Tvar*_tmp300;
void*_tmp302;struct Cyc_Absyn_Tqual _tmp304;void*_tmp306;struct Cyc_Absyn_Exp*
_tmp308;struct Cyc_Absyn_Tqual _tmp30A;void*_tmp30C;struct Cyc_Absyn_Exp*_tmp30E;
struct Cyc_Absyn_Tqual _tmp310;void*_tmp312;_LL702: if((unsigned int)_tmp2BC > 1?*((
int*)_tmp2BC)== Cyc_Absyn_Pointer_mod: 0){_LL727: _tmp2D6=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp2BC)->f1;if((unsigned int)_tmp2D6 > 1?*((int*)_tmp2D6)== Cyc_Absyn_Nullable_ps:
0){_LL729: _tmp2D8=((struct Cyc_Absyn_Nullable_ps_struct*)_tmp2D6)->f1;goto _LL725;}
else{goto _LL704;}_LL725: _tmp2D4=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp2BC)->f2;if(_tmp2D4 == (void*)Cyc_Absyn_HeapRgn){goto _LL723;}else{goto _LL704;}
_LL723: _tmp2D2=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp2BC)->f3;goto _LL703;}
else{goto _LL704;}_LL704: if((unsigned int)_tmp2BC > 1?*((int*)_tmp2BC)== Cyc_Absyn_Pointer_mod:
0){_LL735: _tmp2DE=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp2BC)->f1;if((
unsigned int)_tmp2DE > 1?*((int*)_tmp2DE)== Cyc_Absyn_NonNullable_ps: 0){_LL737:
_tmp2E0=((struct Cyc_Absyn_NonNullable_ps_struct*)_tmp2DE)->f1;goto _LL733;}else{
goto _LL706;}_LL733: _tmp2DC=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp2BC)->f2;
if(_tmp2DC == (void*)Cyc_Absyn_HeapRgn){goto _LL731;}else{goto _LL706;}_LL731:
_tmp2DA=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp2BC)->f3;goto _LL705;}else{goto
_LL706;}_LL706: if((unsigned int)_tmp2BC > 1?*((int*)_tmp2BC)== Cyc_Absyn_Pointer_mod:
0){_LL743: _tmp2E6=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp2BC)->f1;if(
_tmp2E6 == (void*)Cyc_Absyn_TaggedArray_ps){goto _LL741;}else{goto _LL708;}_LL741:
_tmp2E4=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp2BC)->f2;if(_tmp2E4 == (
void*)Cyc_Absyn_HeapRgn){goto _LL739;}else{goto _LL708;}_LL739: _tmp2E2=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp2BC)->f3;goto _LL707;}else{goto _LL708;}_LL708: if((unsigned int)_tmp2BC > 1?*((
int*)_tmp2BC)== Cyc_Absyn_Pointer_mod: 0){_LL751: _tmp2EE=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp2BC)->f1;if((unsigned int)_tmp2EE > 1?*((int*)_tmp2EE)== Cyc_Absyn_Nullable_ps:
0){_LL753: _tmp2F0=((struct Cyc_Absyn_Nullable_ps_struct*)_tmp2EE)->f1;goto _LL747;}
else{goto _LL710;}_LL747: _tmp2EA=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp2BC)->f2;if((unsigned int)_tmp2EA > 3?*((int*)_tmp2EA)== Cyc_Absyn_VarType: 0){
_LL749: _tmp2EC=((struct Cyc_Absyn_VarType_struct*)_tmp2EA)->f1;goto _LL745;}else{
goto _LL710;}_LL745: _tmp2E8=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp2BC)->f3;
goto _LL709;}else{goto _LL710;}_LL710: if((unsigned int)_tmp2BC > 1?*((int*)_tmp2BC)
== Cyc_Absyn_Pointer_mod: 0){_LL761: _tmp2F8=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp2BC)->f1;if((unsigned int)_tmp2F8 > 1?*((int*)_tmp2F8)== Cyc_Absyn_NonNullable_ps:
0){_LL763: _tmp2FA=((struct Cyc_Absyn_NonNullable_ps_struct*)_tmp2F8)->f1;goto
_LL757;}else{goto _LL712;}_LL757: _tmp2F4=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp2BC)->f2;if((unsigned int)_tmp2F4 > 3?*((int*)_tmp2F4)== Cyc_Absyn_VarType: 0){
_LL759: _tmp2F6=((struct Cyc_Absyn_VarType_struct*)_tmp2F4)->f1;goto _LL755;}else{
goto _LL712;}_LL755: _tmp2F2=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp2BC)->f3;
goto _LL711;}else{goto _LL712;}_LL712: if((unsigned int)_tmp2BC > 1?*((int*)_tmp2BC)
== Cyc_Absyn_Pointer_mod: 0){_LL771: _tmp302=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp2BC)->f1;if(_tmp302 == (void*)Cyc_Absyn_TaggedArray_ps){goto _LL767;}else{goto
_LL714;}_LL767: _tmp2FE=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp2BC)->f2;
if((unsigned int)_tmp2FE > 3?*((int*)_tmp2FE)== Cyc_Absyn_VarType: 0){_LL769:
_tmp300=((struct Cyc_Absyn_VarType_struct*)_tmp2FE)->f1;goto _LL765;}else{goto
_LL714;}_LL765: _tmp2FC=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp2BC)->f3;goto
_LL713;}else{goto _LL714;}_LL714: if((unsigned int)_tmp2BC > 1?*((int*)_tmp2BC)== 
Cyc_Absyn_Pointer_mod: 0){_LL775: _tmp306=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp2BC)->f1;if((unsigned int)_tmp306 > 1?*((int*)_tmp306)== Cyc_Absyn_Nullable_ps:
0){_LL777: _tmp308=((struct Cyc_Absyn_Nullable_ps_struct*)_tmp306)->f1;goto _LL773;}
else{goto _LL716;}_LL773: _tmp304=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp2BC)->f3;
goto _LL715;}else{goto _LL716;}_LL716: if((unsigned int)_tmp2BC > 1?*((int*)_tmp2BC)
== Cyc_Absyn_Pointer_mod: 0){_LL781: _tmp30C=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp2BC)->f1;if((unsigned int)_tmp30C > 1?*((int*)_tmp30C)== Cyc_Absyn_NonNullable_ps:
0){_LL783: _tmp30E=((struct Cyc_Absyn_NonNullable_ps_struct*)_tmp30C)->f1;goto
_LL779;}else{goto _LL718;}_LL779: _tmp30A=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp2BC)->f3;goto _LL717;}else{goto _LL718;}_LL718: if((unsigned int)_tmp2BC > 1?*((
int*)_tmp2BC)== Cyc_Absyn_Pointer_mod: 0){_LL787: _tmp312=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp2BC)->f1;if(_tmp312 == (void*)Cyc_Absyn_TaggedArray_ps){goto _LL785;}else{goto
_LL720;}_LL785: _tmp310=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp2BC)->f3;goto
_LL719;}else{goto _LL720;}_LL720: goto _LL721;_LL703: Cyc_Dump_dump_pointer(_tag_arr("*",
sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(_tmp2D8);Cyc_Dump_dump_alpha(
_tag_arr("`H",sizeof(unsigned char),3));Cyc_Dump_dumptms(tms->tl,f,a);return;
_LL705: Cyc_Dump_dump_pointer(_tag_arr("@",sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(
_tmp2E0);Cyc_Dump_dump_alpha(_tag_arr("`H",sizeof(unsigned char),3));Cyc_Dump_dumptms(
tms->tl,f,a);return;_LL707: Cyc_Dump_dump_pointer(_tag_arr("{?}",sizeof(
unsigned char),4));Cyc_Dump_dump_alpha(_tag_arr("`H",sizeof(unsigned char),3));
Cyc_Dump_dumptms(tms->tl,f,a);return;_LL709: Cyc_Dump_dump_pointer(_tag_arr("*",
sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(_tmp2F0);Cyc_Dump_dump_alphastr(
_tmp2EC->name);Cyc_Dump_dumptms(tms->tl,f,a);return;_LL711: Cyc_Dump_dump_pointer(
_tag_arr("@",sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(_tmp2FA);Cyc_Dump_dump_alphastr(
_tmp2F6->name);Cyc_Dump_dumptms(tms->tl,f,a);return;_LL713: Cyc_Dump_dump_pointer(
_tag_arr("{?}",sizeof(unsigned char),4));Cyc_Dump_dump_alphastr(_tmp300->name);
Cyc_Dump_dumptms(tms->tl,f,a);return;_LL715: Cyc_Dump_dump_pointer(_tag_arr("*",
sizeof(unsigned char),2));Cyc_Dump_dump_upperbound(_tmp308);Cyc_Dump_dumptms(tms->tl,
f,a);return;_LL717: Cyc_Dump_dump_pointer(_tag_arr("@",sizeof(unsigned char),2));
Cyc_Dump_dump_upperbound(_tmp30E);Cyc_Dump_dumptms(tms->tl,f,a);return;_LL719:
Cyc_Dump_dump_pointer(_tag_arr("{?}",sizeof(unsigned char),4));Cyc_Dump_dumptms(
tms->tl,f,a);return;_LL721: {int next_is_pointer=0;if(tms->tl != 0){void*_tmp314=(
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_LL790: if((unsigned int)
_tmp314 > 1?*((int*)_tmp314)== Cyc_Absyn_Pointer_mod: 0){goto _LL791;}else{goto
_LL792;}_LL792: goto _LL793;_LL791: next_is_pointer=1;goto _LL789;_LL793: goto _LL789;
_LL789:;}if(next_is_pointer){Cyc_Dump_dump_char((int)'(');}Cyc_Dump_dumptms(tms->tl,
f,a);if(next_is_pointer){Cyc_Dump_dump_char((int)')');}{void*_tmp31A=(void*)tms->hd;
struct Cyc_Absyn_Exp*_tmp32A;void*_tmp32C;struct Cyc_List_List*_tmp32E;struct Cyc_Core_Opt*
_tmp330;struct Cyc_Absyn_VarargInfo*_tmp332;int _tmp334;struct Cyc_List_List*
_tmp336;void*_tmp338;struct Cyc_Position_Segment*_tmp33A;struct Cyc_List_List*
_tmp33C;int _tmp33E;struct Cyc_Position_Segment*_tmp340;struct Cyc_List_List*
_tmp342;void*_tmp344;void*_tmp346;_LL796: if(_tmp31A == (void*)Cyc_Absyn_Carray_mod){
goto _LL797;}else{goto _LL798;}_LL798: if((unsigned int)_tmp31A > 1?*((int*)_tmp31A)
== Cyc_Absyn_ConstArray_mod: 0){_LL811: _tmp32A=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp31A)->f1;goto _LL799;}else{goto _LL800;}_LL800: if((unsigned int)_tmp31A > 1?*((
int*)_tmp31A)== Cyc_Absyn_Function_mod: 0){_LL813: _tmp32C=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp31A)->f1;if(*((int*)_tmp32C)== Cyc_Absyn_WithTypes){_LL823: _tmp336=((struct
Cyc_Absyn_WithTypes_struct*)_tmp32C)->f1;goto _LL821;_LL821: _tmp334=((struct Cyc_Absyn_WithTypes_struct*)
_tmp32C)->f2;goto _LL819;_LL819: _tmp332=((struct Cyc_Absyn_WithTypes_struct*)
_tmp32C)->f3;goto _LL817;_LL817: _tmp330=((struct Cyc_Absyn_WithTypes_struct*)
_tmp32C)->f4;goto _LL815;_LL815: _tmp32E=((struct Cyc_Absyn_WithTypes_struct*)
_tmp32C)->f5;goto _LL801;}else{goto _LL802;}}else{goto _LL802;}_LL802: if((
unsigned int)_tmp31A > 1?*((int*)_tmp31A)== Cyc_Absyn_Function_mod: 0){_LL825:
_tmp338=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp31A)->f1;if(*((int*)
_tmp338)== Cyc_Absyn_NoTypes){_LL829: _tmp33C=((struct Cyc_Absyn_NoTypes_struct*)
_tmp338)->f1;goto _LL827;_LL827: _tmp33A=((struct Cyc_Absyn_NoTypes_struct*)_tmp338)->f2;
goto _LL803;}else{goto _LL804;}}else{goto _LL804;}_LL804: if((unsigned int)_tmp31A > 
1?*((int*)_tmp31A)== Cyc_Absyn_TypeParams_mod: 0){_LL835: _tmp342=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp31A)->f1;goto _LL833;_LL833: _tmp340=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp31A)->f2;goto _LL831;_LL831: _tmp33E=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp31A)->f3;goto _LL805;}else{goto _LL806;}_LL806: if((unsigned int)_tmp31A > 1?*((
int*)_tmp31A)== Cyc_Absyn_Attributes_mod: 0){goto _LL807;}else{goto _LL808;}_LL808:
if((unsigned int)_tmp31A > 1?*((int*)_tmp31A)== Cyc_Absyn_Pointer_mod: 0){_LL839:
_tmp346=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp31A)->f1;goto _LL837;
_LL837: _tmp344=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp31A)->f2;goto
_LL809;}else{goto _LL795;}_LL797: Cyc_Dump_dump_symbol(_tag_arr("[]",sizeof(
unsigned char),3));goto _LL795;_LL799: Cyc_Dump_dump_char((int)'[');Cyc_Dump_dumpexp(
_tmp32A);Cyc_Dump_dump_char((int)']');goto _LL795;_LL801: Cyc_Dump_dumpfunargs(
_tmp336,_tmp334,_tmp332,_tmp330,_tmp32E);goto _LL795;_LL803:((void(*)(void(*f)(
struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Dump_group)(Cyc_Dump_dump_alphastr,
_tmp33C,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),
2),_tag_arr(", ",sizeof(unsigned char),3));goto _LL795;_LL805: if(_tmp33E){Cyc_Dump_dumpkindedtvars(
_tmp342);}else{Cyc_Dump_dumptvars(_tmp342);}goto _LL795;_LL807: goto _LL795;_LL809:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp348=_cycalloc(sizeof(
struct Cyc_Core_Impossible_struct));_tmp348[0]=({struct Cyc_Core_Impossible_struct
_tmp349;_tmp349.tag=Cyc_Core_Impossible;_tmp349.f1=_tag_arr("dumptms",sizeof(
unsigned char),8);_tmp349;});_tmp348;}));_LL795:;}return;}_LL701:;}}void Cyc_Dump_dumptqtd(
struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){struct Cyc_List_List*
_tmp34C;void*_tmp34E;struct Cyc_Absyn_Tqual _tmp350;struct _tuple9 _tmp34A=Cyc_Absynpp_to_tms(
tq,t);_LL849: _tmp350=_tmp34A.f1;goto _LL847;_LL847: _tmp34E=_tmp34A.f2;goto _LL845;
_LL845: _tmp34C=_tmp34A.f3;goto _LL843;_LL843: Cyc_Dump_dumptq(_tmp350);Cyc_Dump_dumpntyp(
_tmp34E);Cyc_Dump_dumptms(Cyc_List_imp_rev(_tmp34C),f,a);}void Cyc_Dump_pr_comment(
struct Cyc_Std___sFILE*outf,struct _tagged_arr s){int depth=0;int len=(int)Cyc_Std_strlen(
s);int i=0;for(0;i < len;i ++){unsigned char c=((const unsigned char*)s.curr)[i];if(c
!= '['){Cyc_Std_fputc((int)c,outf);continue;}Cyc_Std_fputs(_tag_arr("\\texttt{",
sizeof(unsigned char),9),outf);i ++;depth ++;for(0;i < len;i ++){unsigned char c=((
const unsigned char*)s.curr)[i];if(c == ']'){depth --;if(depth == 0){Cyc_Std_fputc((
int)'}',outf);break;}}else{if(c == '['){depth ++;}}Cyc_Std_fputc((int)c,outf);}}}
void Cyc_Dump_dump_comment(struct _tagged_arr s){Cyc_Dump_pr_comment(Cyc_Std_stdout,
s);}void Cyc_Dump_dumpdecl(struct Cyc_Absyn_Decl*d,struct _tagged_arr comment){void*
_tmp352=(void*)d->r;struct Cyc_Absyn_Structdecl*_tmp36C;struct Cyc_Absyn_Uniondecl*
_tmp36E;struct Cyc_Absyn_Vardecl*_tmp370;struct Cyc_Absyn_Vardecl _tmp372;void*
_tmp373;struct Cyc_Absyn_Tqual _tmp375;struct _tuple1*_tmp377;struct Cyc_Absyn_Tuniondecl*
_tmp379;struct Cyc_Absyn_Tuniondecl _tmp37B;int _tmp37C;struct Cyc_Core_Opt*_tmp37E;
struct Cyc_List_List*_tmp380;struct _tuple1*_tmp382;void*_tmp384;struct Cyc_Absyn_Enumdecl*
_tmp386;struct Cyc_Absyn_Enumdecl _tmp388;struct Cyc_Core_Opt*_tmp389;struct _tuple1*
_tmp38B;void*_tmp38D;struct Cyc_Absyn_Typedefdecl*_tmp38F;_LL852: if(*((int*)
_tmp352)== Cyc_Absyn_Struct_d){_LL877: _tmp36C=((struct Cyc_Absyn_Struct_d_struct*)
_tmp352)->f1;goto _LL853;}else{goto _LL854;}_LL854: if(*((int*)_tmp352)== Cyc_Absyn_Union_d){
_LL879: _tmp36E=((struct Cyc_Absyn_Union_d_struct*)_tmp352)->f1;goto _LL855;}else{
goto _LL856;}_LL856: if(*((int*)_tmp352)== Cyc_Absyn_Var_d){_LL881: _tmp370=((
struct Cyc_Absyn_Var_d_struct*)_tmp352)->f1;_tmp372=*_tmp370;_LL888: _tmp377=
_tmp372.name;goto _LL886;_LL886: _tmp375=_tmp372.tq;goto _LL884;_LL884: _tmp373=(
void*)_tmp372.type;goto _LL857;}else{goto _LL858;}_LL858: if(*((int*)_tmp352)== Cyc_Absyn_Tunion_d){
_LL890: _tmp379=((struct Cyc_Absyn_Tunion_d_struct*)_tmp352)->f1;_tmp37B=*_tmp379;
_LL901: _tmp384=(void*)_tmp37B.sc;goto _LL899;_LL899: _tmp382=_tmp37B.name;goto
_LL897;_LL897: _tmp380=_tmp37B.tvs;goto _LL895;_LL895: _tmp37E=_tmp37B.fields;goto
_LL893;_LL893: _tmp37C=_tmp37B.is_xtunion;goto _LL859;}else{goto _LL860;}_LL860: if(*((
int*)_tmp352)== Cyc_Absyn_Enum_d){_LL903: _tmp386=((struct Cyc_Absyn_Enum_d_struct*)
_tmp352)->f1;_tmp388=*_tmp386;_LL910: _tmp38D=(void*)_tmp388.sc;goto _LL908;_LL908:
_tmp38B=_tmp388.name;goto _LL906;_LL906: _tmp389=_tmp388.fields;goto _LL861;}else{
goto _LL862;}_LL862: if(*((int*)_tmp352)== Cyc_Absyn_Typedef_d){_LL912: _tmp38F=((
struct Cyc_Absyn_Typedef_d_struct*)_tmp352)->f1;goto _LL863;}else{goto _LL864;}
_LL864: if(*((int*)_tmp352)== Cyc_Absyn_Fn_d){goto _LL865;}else{goto _LL866;}_LL866:
if(*((int*)_tmp352)== Cyc_Absyn_Let_d){goto _LL867;}else{goto _LL868;}_LL868: if(*((
int*)_tmp352)== Cyc_Absyn_Letv_d){goto _LL869;}else{goto _LL870;}_LL870: if(*((int*)
_tmp352)== Cyc_Absyn_Namespace_d){goto _LL871;}else{goto _LL872;}_LL872: if(*((int*)
_tmp352)== Cyc_Absyn_Using_d){goto _LL873;}else{goto _LL874;}_LL874: if(*((int*)
_tmp352)== Cyc_Absyn_ExternC_d){goto _LL875;}else{goto _LL851;}_LL853: Cyc_Dump_dump_begin();
Cyc_Dump_dump_alpha(_tag_arr("struct",sizeof(unsigned char),7));if(_tmp36C->name
!= 0){Cyc_Dump_dump_boldqvar((struct _tuple1*)((struct Cyc_Core_Opt*)_check_null(
_tmp36C->name))->v);}Cyc_Dump_dumptvars(_tmp36C->tvs);if(_tmp36C->fields == 0){
Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(unsigned char),3));}else{Cyc_Dump_dump_symbol(
_tag_arr(" ",sizeof(unsigned char),2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(
_tag_arr("\\\\\n",sizeof(unsigned char),4));Cyc_Dump_dumpstructfields((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp36C->fields))->v);Cyc_Dump_dump_rb();Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));}Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(
comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(unsigned char),2));Cyc_Dump_dump_end();
goto _LL851;_LL855: Cyc_Dump_dump_begin();Cyc_Dump_dump_alpha(_tag_arr("union",
sizeof(unsigned char),6));if(_tmp36E->name != 0){Cyc_Dump_dump_boldqvar((struct
_tuple1*)((struct Cyc_Core_Opt*)_check_null(_tmp36E->name))->v);}Cyc_Dump_dumptvars(
_tmp36E->tvs);if(_tmp36E->fields == 0){Cyc_Dump_dump_symbol(_tag_arr(";\n",
sizeof(unsigned char),3));}else{Cyc_Dump_dump_symbol(_tag_arr(" ",sizeof(
unsigned char),2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",
sizeof(unsigned char),4));Cyc_Dump_dumpstructfields((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp36E->fields))->v);Cyc_Dump_dump_rb();Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));}Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(
comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(unsigned char),2));Cyc_Dump_dump_end();
goto _LL851;_LL857: Cyc_Dump_dump_begin();((void(*)(struct Cyc_Absyn_Tqual tq,void*t,
void(*f)(struct _tuple1*),struct _tuple1*a))Cyc_Dump_dumptqtd)(_tmp375,_tmp373,Cyc_Dump_dump_boldqvar,
_tmp377);Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(unsigned char),3));Cyc_Dump_dump_middle();
Cyc_Dump_dump_comment(comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(
unsigned char),2));Cyc_Dump_dump_end();goto _LL851;_LL859: Cyc_Dump_dump_begin();
if(_tmp37C){Cyc_Dump_dump_alpha(_tag_arr("xtunion",sizeof(unsigned char),8));}
else{Cyc_Dump_dump_alpha(_tag_arr("tunion",sizeof(unsigned char),7));}Cyc_Dump_dump_boldqvar(
_tmp382);Cyc_Dump_dumptvars(_tmp380);if(_tmp37E == 0){Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));}else{Cyc_Dump_dump_symbol(_tag_arr(" ",
sizeof(unsigned char),2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(_tag_arr("\\\\\n~~",
sizeof(unsigned char),6));Cyc_Dump_dumptunionfields((struct Cyc_List_List*)
_tmp37E->v);Cyc_Dump_dump_symbol(_tag_arr("\\\\\n",sizeof(unsigned char),4));Cyc_Dump_dump_rb();
Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(unsigned char),3));}Cyc_Dump_dump_middle();
Cyc_Dump_dump_comment(comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(
unsigned char),2));Cyc_Dump_dump_end();goto _LL851;_LL861: Cyc_Dump_dump_begin();
Cyc_Dump_dump_alpha(_tag_arr("enum",sizeof(unsigned char),5));Cyc_Dump_dump_boldqvar(
_tmp38B);if(_tmp389 == 0){Cyc_Dump_dump_symbol(_tag_arr(";\n",sizeof(
unsigned char),3));}else{Cyc_Dump_dump_symbol(_tag_arr(" ",sizeof(unsigned char),
2));Cyc_Dump_dump_lb();Cyc_Dump_dump_symbol(_tag_arr("\\\\\n~~",sizeof(
unsigned char),6));Cyc_Dump_dumpenumfields((struct Cyc_List_List*)_tmp389->v);Cyc_Dump_dump_symbol(
_tag_arr("\\\\\n",sizeof(unsigned char),4));Cyc_Dump_dump_rb();Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));}Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(
comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(unsigned char),2));Cyc_Dump_dump_end();
goto _LL851;_LL863: Cyc_Dump_dump_begin();Cyc_Dump_dump_alpha(_tag_arr("typedef",
sizeof(unsigned char),8));((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(
struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*a))Cyc_Dump_dumptqtd)(({
struct Cyc_Absyn_Tqual _tmp391;_tmp391.q_const=0;_tmp391.q_volatile=0;_tmp391.q_restrict=
0;_tmp391;}),(void*)_tmp38F->defn,Cyc_Dump_dump_boldtypedefname,_tmp38F);Cyc_Dump_dump_symbol(
_tag_arr(";\n",sizeof(unsigned char),3));Cyc_Dump_dump_middle();Cyc_Dump_dump_comment(
comment);Cyc_Dump_dump_symbol(_tag_arr("\n",sizeof(unsigned char),2));Cyc_Dump_dump_end();
goto _LL851;_LL865: goto _LL851;_LL867: goto _LL851;_LL869: goto _LL851;_LL871: goto
_LL851;_LL873: goto _LL851;_LL875: goto _LL851;_LL851:;}static int Cyc_is_other_special(
unsigned char c){switch(c){case '\\': _LL914: goto _LL915;case '"': _LL915: goto _LL916;
case ';': _LL916: goto _LL917;case '&': _LL917: goto _LL918;case '(': _LL918: goto _LL919;
case ')': _LL919: goto _LL920;case '|': _LL920: goto _LL921;case '^': _LL921: goto _LL922;
case '<': _LL922: goto _LL923;case '>': _LL923: goto _LL924;case ' ': _LL924: goto _LL925;
case '\n': _LL925: goto _LL926;case '\t': _LL926: return 1;default: _LL927: return 0;}}
static struct _tagged_arr Cyc_sh_escape_string(struct _tagged_arr s){unsigned int
_tmp3A1=Cyc_Std_strlen(s);int _tmp3A2=0;int _tmp3A3=0;{int i=0;for(0;i < _tmp3A1;i ++){
unsigned char _tmp3A4=((const unsigned char*)s.curr)[i];if(_tmp3A4 == '\''){_tmp3A2
++;}else{if(Cyc_is_other_special(_tmp3A4)){_tmp3A3 ++;}}}}if(_tmp3A2 == 0? _tmp3A3
== 0: 0){return s;}if(_tmp3A2 == 0){return(struct _tagged_arr)Cyc_Std_strconcat_l(({
struct _tagged_arr*_tmp3A5[3];_tmp3A5[2]=_init_tag_arr(_cycalloc(sizeof(struct
_tagged_arr)),"'",sizeof(unsigned char),2);_tmp3A5[1]=({struct _tagged_arr*
_tmp3A6=_cycalloc(sizeof(struct _tagged_arr));_tmp3A6[0]=(struct _tagged_arr)s;
_tmp3A6;});_tmp3A5[0]=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",
sizeof(unsigned char),2);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp3A5,sizeof(struct _tagged_arr*),3));}));}{unsigned int _tmp3A7=(
_tmp3A1 + _tmp3A2)+ _tmp3A3;struct _tagged_arr _tmp3A8=({unsigned int _tmp3AC=
_tmp3A7 + 1;unsigned char*_tmp3AD=(unsigned char*)_cycalloc_atomic(_check_times(
sizeof(unsigned char),_tmp3AC));struct _tagged_arr _tmp3AF=_tag_arr(_tmp3AD,
sizeof(unsigned char),_tmp3A7 + 1);{unsigned int _tmp3AE=_tmp3AC;unsigned int i;
for(i=0;i < _tmp3AE;i ++){_tmp3AD[i]='\000';}};_tmp3AF;});int _tmp3A9=0;int _tmp3AA=
0;for(0;_tmp3A9 < _tmp3A1;_tmp3A9 ++){unsigned char _tmp3AB=((const unsigned char*)s.curr)[
_tmp3A9];if(_tmp3AB == '\''? 1: Cyc_is_other_special(_tmp3AB)){*((unsigned char*)
_check_unknown_subscript(_tmp3A8,sizeof(unsigned char),_tmp3AA ++))='\\';}*((
unsigned char*)_check_unknown_subscript(_tmp3A8,sizeof(unsigned char),_tmp3AA ++))=
_tmp3AB;}return(struct _tagged_arr)_tmp3A8;}}static struct _tagged_arr*Cyc_sh_escape_stringptr(
struct _tagged_arr*sp){return({struct _tagged_arr*_tmp3B0=_cycalloc(sizeof(struct
_tagged_arr));_tmp3B0[0]=Cyc_sh_escape_string(*sp);_tmp3B0;});}static struct Cyc_Lineno_Pos*
Cyc_new_pos(){return({struct Cyc_Lineno_Pos*_tmp3B1=_cycalloc(sizeof(struct Cyc_Lineno_Pos));
_tmp3B1->logical_file=_tag_arr("",sizeof(unsigned char),1);_tmp3B1->line=Cyc_Core_new_string(
0);_tmp3B1->line_no=0;_tmp3B1->col=0;_tmp3B1;});}struct _tuple14{int f1;struct Cyc_Lineno_Pos*
f2;};static struct _tuple14*Cyc_start2pos(int x){return({struct _tuple14*_tmp3B2=
_cycalloc(sizeof(struct _tuple14));_tmp3B2->f1=x;_tmp3B2->f2=Cyc_new_pos();
_tmp3B2;});}static int Cyc_decl2start(struct Cyc_Absyn_Decl*d){return((struct Cyc_Position_Segment*)
_check_null(d->loc))->start;}struct _tuple15{struct Cyc_Lineno_Pos*f1;void*f2;};
static struct Cyc_List_List*Cyc_this_file(struct _tagged_arr file,struct Cyc_List_List*
x){struct Cyc_List_List*_tmp3B3=0;for(0;x != 0;x=x->tl){if(Cyc_Std_strcmp(((*((
struct _tuple15*)x->hd)).f1)->logical_file,file)== 0){_tmp3B3=({struct Cyc_List_List*
_tmp3B4=_cycalloc(sizeof(struct Cyc_List_List));_tmp3B4->hd=(struct _tuple15*)x->hd;
_tmp3B4->tl=_tmp3B3;_tmp3B4;});}}_tmp3B3=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp3B3);return _tmp3B3;}static int Cyc_lineno(struct Cyc_Lineno_Pos*
p){return p->line_no;}static struct Cyc_List_List*Cyc_flatten_decls(struct Cyc_List_List*
decls){struct Cyc_List_List*_tmp3B5=0;while(decls != 0){void*_tmp3B6=(void*)((
struct Cyc_Absyn_Decl*)decls->hd)->r;struct Cyc_List_List*_tmp3D0;struct Cyc_List_List*
_tmp3D2;struct Cyc_List_List*_tmp3D4;_LL952: if(*((int*)_tmp3B6)== Cyc_Absyn_Namespace_d){
_LL977: _tmp3D0=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3B6)->f2;goto _LL953;}
else{goto _LL954;}_LL954: if(*((int*)_tmp3B6)== Cyc_Absyn_Using_d){_LL979: _tmp3D2=((
struct Cyc_Absyn_Using_d_struct*)_tmp3B6)->f2;goto _LL955;}else{goto _LL956;}_LL956:
if(*((int*)_tmp3B6)== Cyc_Absyn_ExternC_d){_LL981: _tmp3D4=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp3B6)->f1;goto _LL957;}else{goto _LL958;}_LL958: if(*((int*)_tmp3B6)== Cyc_Absyn_Struct_d){
goto _LL959;}else{goto _LL960;}_LL960: if(*((int*)_tmp3B6)== Cyc_Absyn_Union_d){
goto _LL961;}else{goto _LL962;}_LL962: if(*((int*)_tmp3B6)== Cyc_Absyn_Var_d){goto
_LL963;}else{goto _LL964;}_LL964: if(*((int*)_tmp3B6)== Cyc_Absyn_Tunion_d){goto
_LL965;}else{goto _LL966;}_LL966: if(*((int*)_tmp3B6)== Cyc_Absyn_Enum_d){goto
_LL967;}else{goto _LL968;}_LL968: if(*((int*)_tmp3B6)== Cyc_Absyn_Typedef_d){goto
_LL969;}else{goto _LL970;}_LL970: if(*((int*)_tmp3B6)== Cyc_Absyn_Fn_d){goto _LL971;}
else{goto _LL972;}_LL972: if(*((int*)_tmp3B6)== Cyc_Absyn_Let_d){goto _LL973;}else{
goto _LL974;}_LL974: if(*((int*)_tmp3B6)== Cyc_Absyn_Letv_d){goto _LL975;}else{goto
_LL951;}_LL953: _tmp3D2=_tmp3D0;goto _LL955;_LL955: _tmp3D4=_tmp3D2;goto _LL957;
_LL957: decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp3D4,decls->tl);goto _LL951;_LL959: goto _LL961;_LL961: goto
_LL963;_LL963: goto _LL965;_LL965: goto _LL967;_LL967: goto _LL969;_LL969: goto _LL971;
_LL971: goto _LL973;_LL973: goto _LL975;_LL975: _tmp3B5=({struct Cyc_List_List*_tmp3D6=
_cycalloc(sizeof(struct Cyc_List_List));_tmp3D6->hd=(struct Cyc_Absyn_Decl*)decls->hd;
_tmp3D6->tl=_tmp3B5;_tmp3D6;});decls=decls->tl;goto _LL951;_LL951:;}return((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3B5);}
static struct Cyc_List_List*Cyc_cycdoc_files=0;static void Cyc_add_other(struct
_tagged_arr s){Cyc_cycdoc_files=({struct Cyc_List_List*_tmp3D7=_cycalloc(sizeof(
struct Cyc_List_List));_tmp3D7->hd=({struct _tagged_arr*_tmp3D8=_cycalloc(sizeof(
struct _tagged_arr));_tmp3D8[0]=s;_tmp3D8;});_tmp3D7->tl=Cyc_cycdoc_files;_tmp3D7;});}
static struct Cyc_List_List*Cyc_cycargs=0;static void Cyc_add_cycarg(struct
_tagged_arr s){Cyc_cycargs=({struct Cyc_List_List*_tmp3D9=_cycalloc(sizeof(struct
Cyc_List_List));_tmp3D9->hd=({struct _tagged_arr*_tmp3DA=_cycalloc(sizeof(struct
_tagged_arr));_tmp3DA[0]=s;_tmp3DA;});_tmp3D9->tl=Cyc_cycargs;_tmp3D9;});}static
unsigned char _tmp3DB[8]="cyclone";static struct _tagged_arr Cyc_cyclone_file={
_tmp3DB,_tmp3DB,_tmp3DB + 8};static void Cyc_set_cyclone_file(struct _tagged_arr s){
Cyc_cyclone_file=s;}struct _tuple16{int f1;struct Cyc_Absyn_Decl*f2;};struct
_tuple17{struct Cyc_Lineno_Pos*f1;struct Cyc_Absyn_Decl*f2;};static void Cyc_process_file(
struct _tagged_arr filename){struct _tagged_arr _tmp3DC=Cyc_Filename_chop_extension(
filename);struct _tagged_arr _tmp3DD=Cyc_Std_strconcat((struct _tagged_arr)_tmp3DC,
_tag_arr(".cyp",sizeof(unsigned char),5));struct _tagged_arr _tmp3DE=Cyc_Std_str_sepstr(({
struct Cyc_List_List*_tmp40E=_cycalloc(sizeof(struct Cyc_List_List));_tmp40E->hd=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(unsigned char),1);
_tmp40E->tl=((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cycargs));_tmp40E;}),_tag_arr(" ",
sizeof(unsigned char),2));struct _tagged_arr _tmp3DF=({struct Cyc_Std_String_pa_struct
_tmp40D;_tmp40D.tag=Cyc_Std_String_pa;_tmp40D.f1=(struct _tagged_arr)Cyc_sh_escape_string(
filename);{struct Cyc_Std_String_pa_struct _tmp40C;_tmp40C.tag=Cyc_Std_String_pa;
_tmp40C.f1=(struct _tagged_arr)Cyc_sh_escape_string((struct _tagged_arr)_tmp3DD);{
struct Cyc_Std_String_pa_struct _tmp40B;_tmp40B.tag=Cyc_Std_String_pa;_tmp40B.f1=(
struct _tagged_arr)_tmp3DE;{struct Cyc_Std_String_pa_struct _tmp40A;_tmp40A.tag=Cyc_Std_String_pa;
_tmp40A.f1=(struct _tagged_arr)Cyc_cyclone_file;{void*_tmp409[4]={& _tmp40A,&
_tmp40B,& _tmp40C,& _tmp40D};Cyc_Std_aprintf(_tag_arr("%s %s -E -o %s -x cyc %s",
sizeof(unsigned char),25),_tag_arr(_tmp409,sizeof(void*),4));}}}}});if(Cyc_Std_system((
struct _tagged_arr)_tmp3DF)!= 0){({void*_tmp3E0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\nError: preprocessing\n",sizeof(unsigned char),23),_tag_arr(_tmp3E0,
sizeof(void*),0));});return;}Cyc_Position_reset_position((struct _tagged_arr)
_tmp3DD);{struct Cyc_Std___sFILE*in_file=(struct Cyc_Std___sFILE*)_check_null(Cyc_Std_fopen((
struct _tagged_arr)_tmp3DD,_tag_arr("r",sizeof(unsigned char),2)));Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp3E1=Cyc_Parse_parse_file(in_file);Cyc_Lex_lex_init();Cyc_Std_file_close((
struct Cyc_Std___sFILE*)in_file);_tmp3E1=Cyc_flatten_decls(_tmp3E1);{struct Cyc_List_List*
_tmp3E2=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(int),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_start2pos,((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_decl2start,_tmp3E1));Cyc_Lineno_poss_of_abss((
struct _tagged_arr)_tmp3DD,_tmp3E2);Cyc_Std_remove((struct _tagged_arr)_tmp3DD);{
struct Cyc_List_List*_tmp3E3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple14*))Cyc_Core_snd,_tmp3E2),_tmp3E1);_tmp3E3=((struct Cyc_List_List*(*)(
struct _tagged_arr file,struct Cyc_List_List*x))Cyc_this_file)(filename,_tmp3E3);{
struct Cyc_List_List*_tmp3E4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Lineno_Pos*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(struct
Cyc_Lineno_Pos*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)(struct _tuple17*))Cyc_Core_fst,_tmp3E3)),((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Decl*(*)(struct _tuple17*))Cyc_Core_snd,_tmp3E3));struct Cyc_Std___sFILE*
_tmp3E5=(struct Cyc_Std___sFILE*)_check_null(Cyc_Std_fopen(filename,_tag_arr("r",
sizeof(unsigned char),2)));struct Cyc_Lexing_lexbuf*_tmp3E6=Cyc_Lexing_from_file(
_tmp3E5);struct Cyc_List_List*_tmp3E7=0;struct _tuple10*comment;while((comment=((
struct _tuple10*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_token)(_tmp3E6))!= 0){
_tmp3E7=({struct Cyc_List_List*_tmp3E8=_cycalloc(sizeof(struct Cyc_List_List));
_tmp3E8->hd=(struct _tuple10*)_check_null(comment);_tmp3E8->tl=_tmp3E7;_tmp3E8;});}
Cyc_Std_fclose(_tmp3E5);_tmp3E7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp3E7);{struct Cyc_List_List*_tmp3E9=((struct Cyc_List_List*(*)(
struct _tuple14*(*f)(int),struct Cyc_List_List*x))Cyc_List_map)(Cyc_start2pos,((
struct Cyc_List_List*(*)(int(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((
int(*)(struct _tuple10*))Cyc_Core_fst,_tmp3E7));Cyc_Lineno_poss_of_abss(filename,
_tmp3E9);{struct Cyc_List_List*_tmp3EA=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple14*))Cyc_Core_snd,_tmp3E9),((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple10*))
Cyc_Core_snd,_tmp3E7));_tmp3EA=Cyc_this_file(filename,_tmp3EA);{struct Cyc_List_List*
_tmp3EB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*f)(
struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple15*))Cyc_Core_fst,_tmp3EA)),((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple15*))
Cyc_Core_snd,_tmp3EA));while(_tmp3E4 != 0? _tmp3EB != 0: 0){struct _tuple16 _tmp3EE;
struct Cyc_Absyn_Decl*_tmp3EF;int _tmp3F1;struct _tuple16*_tmp3EC=(struct _tuple16*)
_tmp3E4->hd;_tmp3EE=*_tmp3EC;_LL1010: _tmp3F1=_tmp3EE.f1;goto _LL1008;_LL1008:
_tmp3EF=_tmp3EE.f2;goto _LL1005;_LL1005: {struct _tuple10 _tmp3F5;void*_tmp3F6;int
_tmp3F8;struct _tuple10*_tmp3F3=(struct _tuple10*)_tmp3EB->hd;_tmp3F5=*_tmp3F3;
_LL1017: _tmp3F8=_tmp3F5.f1;goto _LL1015;_LL1015: _tmp3F6=_tmp3F5.f2;goto _LL1012;
_LL1012: {void*_tmp3FA=_tmp3F6;struct _tagged_arr _tmp400;struct _tagged_arr _tmp402;
_LL1020: if(*((int*)_tmp3FA)== Cyc_Standalone){_LL1025: _tmp400=((struct Cyc_Standalone_struct*)
_tmp3FA)->f1;goto _LL1021;}else{goto _LL1022;}_LL1022: if(*((int*)_tmp3FA)== Cyc_MatchDecl){
_LL1027: _tmp402=((struct Cyc_MatchDecl_struct*)_tmp3FA)->f1;goto _LL1023;}else{
goto _LL1019;}_LL1021: Cyc_Dump_dump_comment((struct _tagged_arr)_tmp400);Cyc_Dump_dump_symbol(
_tag_arr("\n",sizeof(unsigned char),2));_tmp3EB=_tmp3EB->tl;goto _LL1019;_LL1023:
if(_tmp3F8 < _tmp3F1){_tmp3EB=_tmp3EB->tl;continue;}if(_tmp3E4->tl != 0){struct
_tuple16 _tmp406;int _tmp407;struct _tuple16*_tmp404=(struct _tuple16*)((struct Cyc_List_List*)
_check_null(_tmp3E4->tl))->hd;_tmp406=*_tmp404;_LL1032: _tmp407=_tmp406.f1;goto
_LL1029;_LL1029: if(_tmp407 < _tmp3F8){_tmp3E4=_tmp3E4->tl;continue;}}Cyc_Dump_dumpdecl(
_tmp3EF,(struct _tagged_arr)_tmp402);Cyc_Std_fflush((struct Cyc_Std___sFILE*)Cyc_Std_stdout);
_tmp3E4=_tmp3E4->tl;_tmp3EB=_tmp3EB->tl;goto _LL1019;_LL1019:;}}}}}}}}}}}}struct
_tuple18{struct _tagged_arr f1;int f2;struct _tagged_arr f3;void*f4;struct _tagged_arr
f5;};int Cyc_main(int argc,struct _tagged_arr argv){struct Cyc_List_List*options=({
struct _tuple18*_tmp410[4];_tmp410[3]=({struct _tuple18*_tmp41A=_cycalloc(sizeof(
struct _tuple18));_tmp41A->f1=_tag_arr("-B",sizeof(unsigned char),3);_tmp41A->f2=
1;_tmp41A->f3=_tag_arr("<file>",sizeof(unsigned char),7);_tmp41A->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp41B=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));
_tmp41B[0]=({struct Cyc_Arg_Flag_spec_struct _tmp41C;_tmp41C.tag=Cyc_Arg_Flag_spec;
_tmp41C.f1=Cyc_add_cycarg;_tmp41C;});_tmp41B;});_tmp41A->f5=_tag_arr("Add to the list of directories to search for compiler files",
sizeof(unsigned char),60);_tmp41A;});_tmp410[2]=({struct _tuple18*_tmp417=
_cycalloc(sizeof(struct _tuple18));_tmp417->f1=_tag_arr("-I",sizeof(unsigned char),
3);_tmp417->f2=1;_tmp417->f3=_tag_arr("<dir>",sizeof(unsigned char),6);_tmp417->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp418=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));
_tmp418[0]=({struct Cyc_Arg_Flag_spec_struct _tmp419;_tmp419.tag=Cyc_Arg_Flag_spec;
_tmp419.f1=Cyc_add_cycarg;_tmp419;});_tmp418;});_tmp417->f5=_tag_arr("Add to the list of directories to search for include files",
sizeof(unsigned char),59);_tmp417;});_tmp410[1]=({struct _tuple18*_tmp414=
_cycalloc(sizeof(struct _tuple18));_tmp414->f1=_tag_arr("-D",sizeof(unsigned char),
3);_tmp414->f2=1;_tmp414->f3=_tag_arr("<name>[=<value>]",sizeof(unsigned char),
17);_tmp414->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp415=_cycalloc(
sizeof(struct Cyc_Arg_Flag_spec_struct));_tmp415[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp416;_tmp416.tag=Cyc_Arg_Flag_spec;_tmp416.f1=Cyc_add_cycarg;_tmp416;});
_tmp415;});_tmp414->f5=_tag_arr("Pass definition to preprocessor",sizeof(
unsigned char),32);_tmp414;});_tmp410[0]=({struct _tuple18*_tmp411=_cycalloc(
sizeof(struct _tuple18));_tmp411->f1=_tag_arr("-cyclone",sizeof(unsigned char),9);
_tmp411->f2=0;_tmp411->f3=_tag_arr(" <file>",sizeof(unsigned char),8);_tmp411->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp412=_cycalloc(sizeof(struct Cyc_Arg_String_spec_struct));
_tmp412[0]=({struct Cyc_Arg_String_spec_struct _tmp413;_tmp413.tag=Cyc_Arg_String_spec;
_tmp413.f1=Cyc_set_cyclone_file;_tmp413;});_tmp412;});_tmp411->f5=_tag_arr("Use <file> as the cyclone compiler",
sizeof(unsigned char),35);_tmp411;});((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp410,sizeof(struct _tuple18*),4));});Cyc_Arg_parse(
options,Cyc_add_other,_tag_arr("Options:",sizeof(unsigned char),9),argv);{struct
Cyc_List_List*_tmp40F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cycdoc_files);for(0;_tmp40F != 0;_tmp40F=_tmp40F->tl){Cyc_process_file(*((
struct _tagged_arr*)_tmp40F->hd));}}return 0;}
