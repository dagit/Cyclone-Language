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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;
extern struct Cyc_Std___sFILE*Cyc_Std_stderr;extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int
Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct _tagged_arr fmt,struct _tagged_arr);
extern struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);
struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
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
struct Cyc_Std___sFILE*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct
Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[18];extern void Cyc_List_iter(
void(*f)(void*),struct Cyc_List_List*x);extern void Cyc_List_iter_c(void(*f)(void*,
void*),void*env,struct Cyc_List_List*x);extern void*Cyc_List_fold_right(void*(*f)(
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
extern void*Cyc_Absyn_float_t;extern void*Cyc_Absyn_double_t(int);extern struct
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
struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple4*Cyc_Parse_fnargspec_to_arg(
struct Cyc_Position_Segment*loc,struct _tuple2*t);struct _tuple5{void*f1;struct Cyc_Core_Opt*
f2;};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*
ts,struct Cyc_Position_Segment*loc);struct _tuple6{struct Cyc_Absyn_Tqual f1;void*f2;
struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple6 Cyc_Parse_apply_tms(
struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);struct
_tuple7{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;
struct Cyc_List_List*f5;};static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment*loc,struct _tuple7*t);struct Cyc_Core_Opt*Cyc_Parse_lbuf=
0;static struct Cyc_List_List*Cyc_Parse_parse_result=0;struct Cyc_Absyn_Aligned_att_struct
Cyc_Parse_att_aligned={1,- 1};struct _tuple8{struct _tagged_arr f1;void*f2;};static
unsigned char _tmp1[8]="stdcall";static unsigned char _tmp2[6]="cdecl";static
unsigned char _tmp3[9]="fastcall";static unsigned char _tmp4[9]="noreturn";static
unsigned char _tmp5[6]="const";static unsigned char _tmp6[8]="aligned";static
unsigned char _tmp7[7]="packed";static unsigned char _tmp8[7]="shared";static
unsigned char _tmp9[7]="unused";static unsigned char _tmpA[5]="weak";static
unsigned char _tmpB[10]="dllimport";static unsigned char _tmpC[10]="dllexport";
static unsigned char _tmpD[23]="no_instrument_function";static unsigned char _tmpE[
12]="constructor";static unsigned char _tmpF[11]="destructor";static unsigned char
_tmp10[22]="no_check_memory_usage";struct _tuple8 Cyc_Parse_att_map[16]={{{_tmp1,
_tmp1,_tmp1 + 8},(void*)0},{{_tmp2,_tmp2,_tmp2 + 6},(void*)1},{{_tmp3,_tmp3,_tmp3 + 
9},(void*)2},{{_tmp4,_tmp4,_tmp4 + 9},(void*)3},{{_tmp5,_tmp5,_tmp5 + 6},(void*)4},{{
_tmp6,_tmp6,_tmp6 + 8},(void*)& Cyc_Parse_att_aligned},{{_tmp7,_tmp7,_tmp7 + 7},(
void*)5},{{_tmp8,_tmp8,_tmp8 + 7},(void*)7},{{_tmp9,_tmp9,_tmp9 + 7},(void*)8},{{
_tmpA,_tmpA,_tmpA + 5},(void*)9},{{_tmpB,_tmpB,_tmpB + 10},(void*)10},{{_tmpC,
_tmpC,_tmpC + 10},(void*)11},{{_tmpD,_tmpD,_tmpD + 23},(void*)12},{{_tmpE,_tmpE,
_tmpE + 12},(void*)13},{{_tmpF,_tmpF,_tmpF + 11},(void*)14},{{_tmp10,_tmp10,_tmp10
+ 22},(void*)15}};static void Cyc_Parse_err(struct _tagged_arr msg,struct Cyc_Position_Segment*
sg){Cyc_Position_post_error(Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(
struct _tagged_arr msg,struct Cyc_Position_Segment*sg){Cyc_Parse_err(msg,sg);(int)
_throw((void*)Cyc_Position_Exit);}static void*Cyc_Parse_unimp(struct _tagged_arr
msg,struct Cyc_Position_Segment*sg){return Cyc_Parse_abort((struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp12;_tmp12.tag=0;_tmp12.f1=(struct _tagged_arr)
msg;{void*_tmp11[1]={& _tmp12};Cyc_Std_aprintf(_tag_arr("%s unimplemented",
sizeof(unsigned char),17),_tag_arr(_tmp11,sizeof(void*),1));}}),sg);}static void
Cyc_Parse_unimp2(struct _tagged_arr msg,struct Cyc_Position_Segment*sg){({struct Cyc_Std_String_pa_struct
_tmp15;_tmp15.tag=0;_tmp15.f1=(struct _tagged_arr)msg;{struct Cyc_Std_String_pa_struct
_tmp14;_tmp14.tag=0;_tmp14.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(
sg);{void*_tmp13[2]={& _tmp14,& _tmp15};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: Warning: Cyclone does not yet support %s\n",
sizeof(unsigned char),46),_tag_arr(_tmp13,sizeof(void*),2));}}});return;}static
int Cyc_Parse_enum_counter=0;struct _tuple1*Cyc_Parse_gensym_enum(){return({struct
_tuple1*_tmp16=_cycalloc(sizeof(struct _tuple1));_tmp16->f1=(void*)({struct Cyc_Absyn_Rel_n_struct*
_tmp1A=_cycalloc(sizeof(struct Cyc_Absyn_Rel_n_struct));_tmp1A[0]=({struct Cyc_Absyn_Rel_n_struct
_tmp1B;_tmp1B.tag=0;_tmp1B.f1=0;_tmp1B;});_tmp1A;});_tmp16->f2=({struct
_tagged_arr*_tmp17=_cycalloc(sizeof(struct _tagged_arr));_tmp17[0]=(struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp19;_tmp19.tag=1;_tmp19.f1=(int)((
unsigned int)Cyc_Parse_enum_counter ++);{void*_tmp18[1]={& _tmp19};Cyc_Std_aprintf(
_tag_arr("__anonymous_enum_%d__",sizeof(unsigned char),22),_tag_arr(_tmp18,
sizeof(void*),1));}});_tmp17;});_tmp16;});}struct _tuple9{struct _tuple7*f1;struct
Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Structfield*Cyc_Parse_make_struct_field(
struct Cyc_Position_Segment*loc,struct _tuple9*field_info){struct _tuple9 _tmp1D;
struct Cyc_Absyn_Exp*_tmp1E;struct _tuple7*_tmp1F;struct _tuple7 _tmp20;struct Cyc_List_List*
_tmp21;struct Cyc_List_List*_tmp22;void*_tmp23;struct Cyc_Absyn_Tqual _tmp24;struct
_tuple1*_tmp25;struct _tuple9*_tmp1C=field_info;_tmp1D=*_tmp1C;_LL2: _tmp1F=_tmp1D.f1;
_tmp20=*_tmp1F;_LL7: _tmp25=_tmp20.f1;goto _LL6;_LL6: _tmp24=_tmp20.f2;goto _LL5;
_LL5: _tmp23=_tmp20.f3;goto _LL4;_LL4: _tmp22=_tmp20.f4;goto _LL3;_LL3: _tmp21=_tmp20.f5;
goto _LL1;_LL1: _tmp1E=_tmp1D.f2;goto _LL0;_LL0: if(_tmp22 != 0){Cyc_Parse_err(
_tag_arr("bad type params in struct field",sizeof(unsigned char),32),loc);}if(
Cyc_Absyn_is_qvar_qualified(_tmp25)){Cyc_Parse_err(_tag_arr("struct field cannot be qualified with a namespace",
sizeof(unsigned char),50),loc);}return({struct Cyc_Absyn_Structfield*_tmp26=
_cycalloc(sizeof(struct Cyc_Absyn_Structfield));_tmp26->name=(*_tmp25).f2;_tmp26->tq=
_tmp24;_tmp26->type=(void*)_tmp23;_tmp26->width=_tmp1E;_tmp26->attributes=_tmp21;
_tmp26;});}struct _tuple10{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;
struct Cyc_List_List*f4;};static struct _tuple2*Cyc_Parse_make_param(struct Cyc_Position_Segment*
loc,struct _tuple10*field){struct _tuple10 _tmp28;struct Cyc_List_List*_tmp29;void*
_tmp2A;struct Cyc_Absyn_Tqual _tmp2B;struct Cyc_Core_Opt*_tmp2C;struct _tuple10*
_tmp27=field;_tmp28=*_tmp27;_LLC: _tmp2C=_tmp28.f1;goto _LLB;_LLB: _tmp2B=_tmp28.f2;
goto _LLA;_LLA: _tmp2A=_tmp28.f3;goto _LL9;_LL9: _tmp29=_tmp28.f4;goto _LL8;_LL8: {
struct Cyc_Core_Opt*_tmp2D=0;if(_tmp2C != 0){if(Cyc_Absyn_is_qvar_qualified((
struct _tuple1*)_tmp2C->v)){Cyc_Parse_err(_tag_arr("parameter cannot be qualified with a namespace",
sizeof(unsigned char),47),loc);}_tmp2D=({struct Cyc_Core_Opt*_tmp2E=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp2E->v=(*((struct _tuple1*)_tmp2C->v)).f2;_tmp2E;});}
if(_tmp29 != 0){((int(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*sg))Cyc_Parse_abort)(
_tag_arr("parameter should have no type parameters",sizeof(unsigned char),41),
loc);}return({struct _tuple2*_tmp2F=_cycalloc(sizeof(struct _tuple2));_tmp2F->f1=
_tmp2D;_tmp2F->f2=_tmp2B;_tmp2F->f3=_tmp2A;_tmp2F;});}}static void*Cyc_Parse_type_spec(
void*t,struct Cyc_Position_Segment*loc){return(void*)({struct Cyc_Parse_Type_spec_struct*
_tmp30=_cycalloc(sizeof(struct Cyc_Parse_Type_spec_struct));_tmp30[0]=({struct Cyc_Parse_Type_spec_struct
_tmp31;_tmp31.tag=4;_tmp31.f1=(void*)t;_tmp31.f2=loc;_tmp31;});_tmp30;});}static
void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp32=t;struct Cyc_Absyn_Exp*
_tmp33;struct Cyc_Absyn_Tqual _tmp34;void*_tmp35;_LLE: if((unsigned int)_tmp32 > 3?*((
int*)_tmp32)== 7: 0){_LL14: _tmp35=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp32)->f1;goto _LL13;_LL13: _tmp34=((struct Cyc_Absyn_ArrayType_struct*)_tmp32)->f2;
goto _LL12;_LL12: _tmp33=((struct Cyc_Absyn_ArrayType_struct*)_tmp32)->f3;goto _LLF;}
else{goto _LL10;}_LL10: goto _LL11;_LLF: return Cyc_Absyn_starb_typ(_tmp35,argposn?
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp36=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp36->v=(void*)((void*)3);_tmp36;}),0):(void*)2,_tmp34,_tmp33 == 0?(void*)0:(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp37=_cycalloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
_tmp37[0]=({struct Cyc_Absyn_Upper_b_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct
Cyc_Absyn_Exp*)_check_null(_tmp33);_tmp38;});_tmp37;}));_LL11: return t;_LLD:;}
static void Cyc_Parse_arg_array2ptr(struct _tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*
x).f3,1);}struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple11*
Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple2*t){return({
struct _tuple11*_tmp39=_cycalloc(sizeof(struct _tuple11));_tmp39->f1=(*t).f2;
_tmp39->f2=(*t).f3;_tmp39;});}static void Cyc_Parse_only_vardecl(struct Cyc_List_List*
params,struct Cyc_Absyn_Decl*x){struct _tagged_arr decl_kind;{void*_tmp3A=(void*)x->r;
struct Cyc_Absyn_Vardecl*_tmp3B;_LL16: if(*((int*)_tmp3A)== 0){_LL2E: _tmp3B=((
struct Cyc_Absyn_Var_d_struct*)_tmp3A)->f1;goto _LL17;}else{goto _LL18;}_LL18: if(*((
int*)_tmp3A)== 2){goto _LL19;}else{goto _LL1A;}_LL1A: if(*((int*)_tmp3A)== 3){goto
_LL1B;}else{goto _LL1C;}_LL1C: if(*((int*)_tmp3A)== 1){goto _LL1D;}else{goto _LL1E;}
_LL1E: if(*((int*)_tmp3A)== 4){goto _LL1F;}else{goto _LL20;}_LL20: if(*((int*)_tmp3A)
== 5){goto _LL21;}else{goto _LL22;}_LL22: if(*((int*)_tmp3A)== 6){goto _LL23;}else{
goto _LL24;}_LL24: if(*((int*)_tmp3A)== 8){goto _LL25;}else{goto _LL26;}_LL26: if(*((
int*)_tmp3A)== 7){goto _LL27;}else{goto _LL28;}_LL28: if(*((int*)_tmp3A)== 9){goto
_LL29;}else{goto _LL2A;}_LL2A: if(*((int*)_tmp3A)== 10){goto _LL2B;}else{goto _LL2C;}
_LL2C: if(*((int*)_tmp3A)== 11){goto _LL2D;}else{goto _LL15;}_LL17: if(_tmp3B->initializer
!= 0){((int(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*sg))Cyc_Parse_abort)(
_tag_arr("initializers are not allowed in parameter declarations",sizeof(
unsigned char),55),x->loc);}if(Cyc_Absyn_is_qvar_qualified(_tmp3B->name)){Cyc_Parse_err(
_tag_arr("namespaces not allowed on parameter declarations",sizeof(unsigned char),
49),x->loc);}{int found=0;for(0;params != 0;params=params->tl){if(Cyc_Std_zstrptrcmp((*
_tmp3B->name).f2,(struct _tagged_arr*)params->hd)== 0){found=1;break;}}if(! found){((
int(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*sg))Cyc_Parse_abort)((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp3D;_tmp3D.tag=0;_tmp3D.f1=(
struct _tagged_arr)*(*_tmp3B->name).f2;{void*_tmp3C[1]={& _tmp3D};Cyc_Std_aprintf(
_tag_arr("%s is not listed as a parameter",sizeof(unsigned char),32),_tag_arr(
_tmp3C,sizeof(void*),1));}}),x->loc);}return;}_LL19: decl_kind=_tag_arr("let declaration",
sizeof(unsigned char),16);goto _LL15;_LL1B: decl_kind=_tag_arr("let declaration",
sizeof(unsigned char),16);goto _LL15;_LL1D: decl_kind=_tag_arr("function declaration",
sizeof(unsigned char),21);goto _LL15;_LL1F: decl_kind=_tag_arr("struct declaration",
sizeof(unsigned char),19);goto _LL15;_LL21: decl_kind=_tag_arr("union declaration",
sizeof(unsigned char),18);goto _LL15;_LL23: decl_kind=_tag_arr("tunion declaration",
sizeof(unsigned char),19);goto _LL15;_LL25: decl_kind=_tag_arr("typedef",sizeof(
unsigned char),8);goto _LL15;_LL27: decl_kind=_tag_arr("enum declaration",sizeof(
unsigned char),17);goto _LL15;_LL29: decl_kind=_tag_arr("namespace declaration",
sizeof(unsigned char),22);goto _LL15;_LL2B: decl_kind=_tag_arr("using declaration",
sizeof(unsigned char),18);goto _LL15;_LL2D: decl_kind=_tag_arr("extern C declaration",
sizeof(unsigned char),21);goto _LL15;_LL15:;}((int(*)(struct _tagged_arr msg,struct
Cyc_Position_Segment*sg))Cyc_Parse_abort)((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp3F;_tmp3F.tag=0;_tmp3F.f1=(struct _tagged_arr)decl_kind;{void*_tmp3E[1]={&
_tmp3F};Cyc_Std_aprintf(_tag_arr("%s appears in parameter type",sizeof(
unsigned char),29),_tag_arr(_tmp3E,sizeof(void*),1));}}),x->loc);return;}static
struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){void*
_tmp40=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp41;_LL30: if(*((int*)_tmp40)== 0){
_LL34: _tmp41=((struct Cyc_Absyn_Var_d_struct*)_tmp40)->f1;goto _LL31;}else{goto
_LL32;}_LL32: goto _LL33;_LL31: return _tmp41;_LL33: return((struct Cyc_Absyn_Vardecl*(*)(
struct _tagged_arr msg,struct Cyc_Position_Segment*sg))Cyc_Parse_abort)(_tag_arr("bad declaration in `forarray' statement",
sizeof(unsigned char),40),d->loc);_LL2F:;}struct _tuple12{struct Cyc_List_List*f1;
struct Cyc_Position_Segment*f2;};static struct _tuple2*Cyc_Parse_get_param_type(
struct _tuple12*env,struct _tagged_arr*x){struct _tuple12 _tmp43;struct Cyc_Position_Segment*
_tmp44;struct Cyc_List_List*_tmp45;struct _tuple12*_tmp42=env;_tmp43=*_tmp42;_LL37:
_tmp45=_tmp43.f1;goto _LL36;_LL36: _tmp44=_tmp43.f2;goto _LL35;_LL35: if(_tmp45 == 0){
return((struct _tuple2*(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*sg))
Cyc_Parse_abort)((struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp47;
_tmp47.tag=0;_tmp47.f1=(struct _tagged_arr)*x;{void*_tmp46[1]={& _tmp47};Cyc_Std_aprintf(
_tag_arr("missing type for parameter %s",sizeof(unsigned char),30),_tag_arr(
_tmp46,sizeof(void*),1));}}),_tmp44);}{void*_tmp48=(void*)((struct Cyc_Absyn_Decl*)
_tmp45->hd)->r;struct Cyc_Absyn_Vardecl*_tmp49;_LL39: if(*((int*)_tmp48)== 0){
_LL3D: _tmp49=((struct Cyc_Absyn_Var_d_struct*)_tmp48)->f1;goto _LL3A;}else{goto
_LL3B;}_LL3B: goto _LL3C;_LL3A: if(Cyc_Absyn_is_qvar_qualified(_tmp49->name)){Cyc_Parse_err(
_tag_arr("namespace not allowed on parameter",sizeof(unsigned char),35),_tmp44);}
if(Cyc_Std_zstrptrcmp((*_tmp49->name).f2,x)== 0){return({struct _tuple2*_tmp4A=
_cycalloc(sizeof(struct _tuple2));_tmp4A->f1=({struct Cyc_Core_Opt*_tmp4B=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp4B->v=(*_tmp49->name).f2;_tmp4B;});
_tmp4A->f2=_tmp49->tq;_tmp4A->f3=(void*)_tmp49->type;_tmp4A;});}else{return Cyc_Parse_get_param_type(({
struct _tuple12*_tmp4C=_cycalloc(sizeof(struct _tuple12));_tmp4C->f1=_tmp45->tl;
_tmp4C->f2=_tmp44;_tmp4C;}),x);}_LL3C: return((struct _tuple2*(*)(struct
_tagged_arr msg,struct Cyc_Position_Segment*sg))Cyc_Parse_abort)(_tag_arr("non-variable declaration",
sizeof(unsigned char),25),((struct Cyc_Absyn_Decl*)_tmp45->hd)->loc);_LL38:;}}
static int Cyc_Parse_is_typeparam(void*tm){void*_tmp4D=tm;_LL3F: if((unsigned int)
_tmp4D > 1?*((int*)_tmp4D)== 3: 0){goto _LL40;}else{goto _LL41;}_LL41: goto _LL42;
_LL40: return 1;_LL42: return 0;_LL3E:;}static void*Cyc_Parse_id2type(struct
_tagged_arr s,void*k){if(Cyc_Std_zstrcmp(s,_tag_arr("`H",sizeof(unsigned char),3))
== 0){return(void*)2;}else{return(void*)({struct Cyc_Absyn_VarType_struct*_tmp4E=
_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));_tmp4E[0]=({struct Cyc_Absyn_VarType_struct
_tmp4F;_tmp4F.tag=1;_tmp4F.f1=({struct Cyc_Absyn_Tvar*_tmp50=_cycalloc(sizeof(
struct Cyc_Absyn_Tvar));_tmp50->name=({struct _tagged_arr*_tmp51=_cycalloc(sizeof(
struct _tagged_arr));_tmp51[0]=s;_tmp51;});_tmp50->identity=0;_tmp50->kind=(void*)
k;_tmp50;});_tmp4F;});_tmp4E;});}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(
struct Cyc_Absyn_Tvar*t){void*k;{void*_tmp52=Cyc_Absyn_compress_kb((void*)t->kind);
void*_tmp53;void*_tmp54;_LL44: if(*((int*)_tmp52)== 0){_LL4A: _tmp53=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp52)->f1;goto _LL45;}else{goto _LL46;}_LL46: if(*((
int*)_tmp52)== 1){goto _LL47;}else{goto _LL48;}_LL48: if(*((int*)_tmp52)== 2){_LL4B:
_tmp54=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp52)->f2;goto _LL49;}else{goto
_LL43;}_LL45: k=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp55=_cycalloc(sizeof(
struct Cyc_Absyn_Eq_kb_struct));_tmp55[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp56;
_tmp56.tag=0;_tmp56.f1=(void*)_tmp53;_tmp56;});_tmp55;});goto _LL43;_LL47: k=(void*)({
struct Cyc_Absyn_Unknown_kb_struct*_tmp57=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_struct));
_tmp57[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp58;_tmp58.tag=1;_tmp58.f1=0;
_tmp58;});_tmp57;});goto _LL43;_LL49: k=(void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp59=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_struct));_tmp59[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp5A;_tmp5A.tag=2;_tmp5A.f1=0;_tmp5A.f2=(void*)_tmp54;_tmp5A;});_tmp59;});goto
_LL43;_LL43:;}return({struct Cyc_Absyn_Tvar*_tmp5B=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_tmp5B->name=t->name;_tmp5B->identity=0;_tmp5B->kind=(void*)k;_tmp5B;});}static
struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*loc,void*t){
void*_tmp5C=t;struct Cyc_Absyn_Tvar*_tmp5D;_LL4D: if((unsigned int)_tmp5C > 3?*((
int*)_tmp5C)== 1: 0){_LL51: _tmp5D=((struct Cyc_Absyn_VarType_struct*)_tmp5C)->f1;
goto _LL4E;}else{goto _LL4F;}_LL4F: goto _LL50;_LL4E: return _tmp5D;_LL50: return((
struct Cyc_Absyn_Tvar*(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*sg))Cyc_Parse_abort)(
_tag_arr("expecting a list of type variables, not types",sizeof(unsigned char),
46),loc);_LL4C:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){return(
void*)({struct Cyc_Absyn_VarType_struct*_tmp5E=_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));
_tmp5E[0]=({struct Cyc_Absyn_VarType_struct _tmp5F;_tmp5F.tag=1;_tmp5F.f1=pr;
_tmp5F;});_tmp5E;});}static void Cyc_Parse_set_vartyp_kind(void*t,void*k){void*
_tmp60=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp61;struct Cyc_Absyn_Tvar
_tmp62;void*_tmp63;void**_tmp64;_LL53: if((unsigned int)_tmp60 > 3?*((int*)_tmp60)
== 1: 0){_LL57: _tmp61=((struct Cyc_Absyn_VarType_struct*)_tmp60)->f1;_tmp62=*
_tmp61;_LL58: _tmp63=(void*)_tmp62.kind;_tmp64=(void**)&(*((struct Cyc_Absyn_VarType_struct*)
_tmp60)->f1).kind;goto _LL54;}else{goto _LL55;}_LL55: goto _LL56;_LL54: {void*_tmp65=
Cyc_Absyn_compress_kb(*_tmp64);void*_tmp66=_tmp65;_LL5A: if(*((int*)_tmp66)== 0){
goto _LL5B;}else{goto _LL5C;}_LL5C: if(*((int*)_tmp66)== 2){goto _LL5D;}else{goto
_LL5E;}_LL5E: if(*((int*)_tmp66)== 1){goto _LL5F;}else{goto _LL59;}_LL5B: return;
_LL5D: return;_LL5F:*_tmp64=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp67=
_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp67[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp68;_tmp68.tag=0;_tmp68.f1=(void*)k;_tmp68;});_tmp67;});return;_LL59:;}_LL56:
return;_LL52:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct Cyc_List_List*
tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){if(tds == 0){return
tms;}if(tms == 0){return 0;}{void*_tmp69=(void*)tms->hd;void*_tmp6A;_LL61: if((
unsigned int)_tmp69 > 1?*((int*)_tmp69)== 2: 0){_LL65: _tmp6A=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp69)->f1;goto _LL62;}else{goto _LL63;}_LL63: goto _LL64;_LL62: if(tms->tl == 0? 1:(
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)?((
struct Cyc_List_List*)_check_null(tms->tl))->tl == 0: 0)){void*_tmp6B=_tmp6A;struct
Cyc_List_List*_tmp6C;_LL67: if(*((int*)_tmp6B)== 1){goto _LL68;}else{goto _LL69;}
_LL69: if(*((int*)_tmp6B)== 0){_LL6B: _tmp6C=((struct Cyc_Absyn_NoTypes_struct*)
_tmp6B)->f1;goto _LL6A;}else{goto _LL66;}_LL68:({void*_tmp6D[0]={};Cyc_Tcutil_warn(
loc,_tag_arr("function declaration with both new- and old-style parameter declarations; ignoring old-style",
sizeof(unsigned char),93),_tag_arr(_tmp6D,sizeof(void*),0));});return tms;_LL6A:((
void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_only_vardecl,_tmp6C,tds);{
struct _tuple12*_tmp6E=({struct _tuple12*_tmp74=_cycalloc(sizeof(struct _tuple12));
_tmp74->f1=tds;_tmp74->f2=loc;_tmp74;});return({struct Cyc_List_List*_tmp6F=
_cycalloc(sizeof(struct Cyc_List_List));_tmp6F->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp70=_cycalloc(sizeof(struct Cyc_Absyn_Function_mod_struct));_tmp70[0]=({struct
Cyc_Absyn_Function_mod_struct _tmp71;_tmp71.tag=2;_tmp71.f1=(void*)((void*)({
struct Cyc_Absyn_WithTypes_struct*_tmp72=_cycalloc(sizeof(struct Cyc_Absyn_WithTypes_struct));
_tmp72[0]=({struct Cyc_Absyn_WithTypes_struct _tmp73;_tmp73.tag=1;_tmp73.f1=((
struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple12*,struct _tagged_arr*),
struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_param_type,
_tmp6E,_tmp6C);_tmp73.f2=0;_tmp73.f3=0;_tmp73.f4=0;_tmp73.f5=0;_tmp73;});_tmp72;}));
_tmp71;});_tmp70;}));_tmp6F->tl=0;_tmp6F;});}_LL66:;}else{return({struct Cyc_List_List*
_tmp75=_cycalloc(sizeof(struct Cyc_List_List));_tmp75->hd=(void*)((void*)tms->hd);
_tmp75->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);_tmp75;});}_LL64: return({
struct Cyc_List_List*_tmp76=_cycalloc(sizeof(struct Cyc_List_List));_tmp76->hd=(
void*)((void*)tms->hd);_tmp76->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);
_tmp76;});_LL60:;}}static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct
Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*
body,struct Cyc_Position_Segment*loc){if(tds != 0){d=({struct Cyc_Parse_Declarator*
_tmp77=_cycalloc(sizeof(struct Cyc_Parse_Declarator));_tmp77->id=d->id;_tmp77->tms=
Cyc_Parse_oldstyle2newstyle(d->tms,tds,loc);_tmp77;});}{void*sc=(void*)2;struct
Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual();int is_inline=
0;struct Cyc_List_List*atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((
struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)
dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0){void*
_tmp78=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc))->v;_LL6D: if((int)_tmp78 == 1){goto _LL6E;}else{goto _LL6F;}_LL6F: if((
int)_tmp78 == 2){goto _LL70;}else{goto _LL71;}_LL71: if((int)_tmp78 == 3){goto _LL72;}
else{goto _LL73;}_LL73: goto _LL74;_LL6E: sc=(void*)3;goto _LL6C;_LL70: sc=(void*)4;
goto _LL6C;_LL72: sc=(void*)0;goto _LL6C;_LL74: Cyc_Parse_err(_tag_arr("bad storage class on function",
sizeof(unsigned char),30),loc);goto _LL6C;_LL6C:;}}{struct Cyc_Core_Opt*_tmp7A;
void*_tmp7B;struct _tuple5 _tmp79=Cyc_Parse_collapse_type_specifiers(tss,loc);
_LL77: _tmp7B=_tmp79.f1;goto _LL76;_LL76: _tmp7A=_tmp79.f2;goto _LL75;_LL75: {struct
Cyc_List_List*_tmp7D;struct Cyc_List_List*_tmp7E;void*_tmp7F;struct Cyc_Absyn_Tqual
_tmp80;struct _tuple6 _tmp7C=Cyc_Parse_apply_tms(tq,_tmp7B,atts,d->tms);_LL7C:
_tmp80=_tmp7C.f1;goto _LL7B;_LL7B: _tmp7F=_tmp7C.f2;goto _LL7A;_LL7A: _tmp7E=_tmp7C.f3;
goto _LL79;_LL79: _tmp7D=_tmp7C.f4;goto _LL78;_LL78: if(_tmp7A != 0){({void*_tmp81[0]={};
Cyc_Tcutil_warn(loc,_tag_arr("nested type declaration within function prototype",
sizeof(unsigned char),50),_tag_arr(_tmp81,sizeof(void*),0));});}if(_tmp7E != 0){({
void*_tmp82[0]={};Cyc_Tcutil_warn(loc,_tag_arr("bad type params, ignoring",
sizeof(unsigned char),26),_tag_arr(_tmp82,sizeof(void*),0));});}{void*_tmp83=
_tmp7F;struct Cyc_Absyn_FnInfo _tmp84;struct Cyc_List_List*_tmp85;struct Cyc_List_List*
_tmp86;struct Cyc_Absyn_VarargInfo*_tmp87;int _tmp88;struct Cyc_List_List*_tmp89;
void*_tmp8A;struct Cyc_Core_Opt*_tmp8B;struct Cyc_List_List*_tmp8C;_LL7E: if((
unsigned int)_tmp83 > 3?*((int*)_tmp83)== 8: 0){_LL82: _tmp84=((struct Cyc_Absyn_FnType_struct*)
_tmp83)->f1;_LL8A: _tmp8C=_tmp84.tvars;goto _LL89;_LL89: _tmp8B=_tmp84.effect;goto
_LL88;_LL88: _tmp8A=(void*)_tmp84.ret_typ;goto _LL87;_LL87: _tmp89=_tmp84.args;goto
_LL86;_LL86: _tmp88=_tmp84.c_varargs;goto _LL85;_LL85: _tmp87=_tmp84.cyc_varargs;
goto _LL84;_LL84: _tmp86=_tmp84.rgn_po;goto _LL83;_LL83: _tmp85=_tmp84.attributes;
goto _LL7F;}else{goto _LL80;}_LL80: goto _LL81;_LL7F: {struct Cyc_List_List*_tmp8D=((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_fnargspec_to_arg,loc,_tmp89);return({struct Cyc_Absyn_Fndecl*_tmp8E=
_cycalloc(sizeof(struct Cyc_Absyn_Fndecl));_tmp8E->sc=(void*)sc;_tmp8E->name=d->id;
_tmp8E->tvs=_tmp8C;_tmp8E->is_inline=is_inline;_tmp8E->effect=_tmp8B;_tmp8E->ret_type=(
void*)_tmp8A;_tmp8E->args=_tmp8D;_tmp8E->c_varargs=_tmp88;_tmp8E->cyc_varargs=
_tmp87;_tmp8E->rgn_po=_tmp86;_tmp8E->body=body;_tmp8E->cached_typ=0;_tmp8E->param_vardecls=
0;_tmp8E->attributes=Cyc_List_append(_tmp85,_tmp7D);_tmp8E;});}_LL81: return((
struct Cyc_Absyn_Fndecl*(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*sg))
Cyc_Parse_abort)(_tag_arr("declarator is not a function prototype",sizeof(
unsigned char),39),loc);_LL7D:;}}}}}static struct _tuple4*Cyc_Parse_fnargspec_to_arg(
struct Cyc_Position_Segment*loc,struct _tuple2*t){if((*t).f1 == 0){Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype",sizeof(unsigned char),
48),loc);return({struct _tuple4*_tmp8F=_cycalloc(sizeof(struct _tuple4));_tmp8F->f1=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"?",sizeof(unsigned char),2);
_tmp8F->f2=(*t).f2;_tmp8F->f3=(*t).f3;_tmp8F;});}else{return({struct _tuple4*
_tmp90=_cycalloc(sizeof(struct _tuple4));_tmp90->f1=(struct _tagged_arr*)((struct
Cyc_Core_Opt*)_check_null((*t).f1))->v;_tmp90->f2=(*t).f2;_tmp90->f3=(*t).f3;
_tmp90;});}}static unsigned char _tmp91[52]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={_tmp91,_tmp91,_tmp91 + 52};static
unsigned char _tmp92[63]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={_tmp92,_tmp92,_tmp92 + 63};static
unsigned char _tmp93[50]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={_tmp93,_tmp93,_tmp93 + 50};static
unsigned char _tmp94[60]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={_tmp94,_tmp94,_tmp94 + 60};static struct
_tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;
void*t=(void*)0;void*sz=(void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*
last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp95=(void*)ts->hd;struct Cyc_Position_Segment*
_tmp96;void*_tmp97;struct Cyc_Position_Segment*_tmp98;struct Cyc_Position_Segment*
_tmp99;struct Cyc_Position_Segment*_tmp9A;struct Cyc_Position_Segment*_tmp9B;
struct Cyc_Absyn_Decl*_tmp9C;_LL8C: if(*((int*)_tmp95)== 4){_LL99: _tmp97=(void*)((
struct Cyc_Parse_Type_spec_struct*)_tmp95)->f1;goto _LL98;_LL98: _tmp96=((struct Cyc_Parse_Type_spec_struct*)
_tmp95)->f2;goto _LL8D;}else{goto _LL8E;}_LL8E: if(*((int*)_tmp95)== 0){_LL9A:
_tmp98=((struct Cyc_Parse_Signed_spec_struct*)_tmp95)->f1;goto _LL8F;}else{goto
_LL90;}_LL90: if(*((int*)_tmp95)== 1){_LL9B: _tmp99=((struct Cyc_Parse_Unsigned_spec_struct*)
_tmp95)->f1;goto _LL91;}else{goto _LL92;}_LL92: if(*((int*)_tmp95)== 2){_LL9C:
_tmp9A=((struct Cyc_Parse_Short_spec_struct*)_tmp95)->f1;goto _LL93;}else{goto
_LL94;}_LL94: if(*((int*)_tmp95)== 3){_LL9D: _tmp9B=((struct Cyc_Parse_Long_spec_struct*)
_tmp95)->f1;goto _LL95;}else{goto _LL96;}_LL96: if(*((int*)_tmp95)== 5){_LL9E:
_tmp9C=((struct Cyc_Parse_Decl_spec_struct*)_tmp95)->f1;goto _LL97;}else{goto _LL8B;}
_LL8D: if(seen_type){Cyc_Parse_err(Cyc_Parse_msg1,_tmp96);}last_loc=_tmp96;
seen_type=1;t=_tmp97;goto _LL8B;_LL8F: if(seen_sign){Cyc_Parse_err(Cyc_Parse_msg4,
_tmp98);}if(seen_type){Cyc_Parse_err(_tag_arr("signed qualifier must come before type",
sizeof(unsigned char),39),_tmp98);}last_loc=_tmp98;seen_sign=1;sgn=(void*)0;goto
_LL8B;_LL91: if(seen_sign){Cyc_Parse_err(Cyc_Parse_msg4,_tmp99);}if(seen_type){
Cyc_Parse_err(_tag_arr("signed qualifier must come before type",sizeof(
unsigned char),39),_tmp99);}last_loc=_tmp99;seen_sign=1;sgn=(void*)1;goto _LL8B;
_LL93: if(seen_size){Cyc_Parse_err(_tag_arr("integral size may appear only once within a type specifier",
sizeof(unsigned char),59),_tmp9A);}if(seen_type){Cyc_Parse_err(_tag_arr("short modifier must come before base type",
sizeof(unsigned char),42),_tmp9A);}last_loc=_tmp9A;seen_size=1;sz=(void*)1;goto
_LL8B;_LL95: if(seen_type){Cyc_Parse_err(_tag_arr("long modifier must come before base type",
sizeof(unsigned char),41),_tmp9B);}if(seen_size){void*_tmp9D=sz;_LLA0: if((int)
_tmp9D == 2){goto _LLA1;}else{goto _LLA2;}_LLA2: goto _LLA3;_LLA1: sz=(void*)3;goto
_LL9F;_LLA3: Cyc_Parse_err(_tag_arr("extra long in type specifier",sizeof(
unsigned char),29),_tmp9B);goto _LL9F;_LL9F:;}last_loc=_tmp9B;seen_size=1;goto
_LL8B;_LL97: last_loc=_tmp9C->loc;if(declopt == 0? ! seen_type: 0){seen_type=1;{void*
_tmp9E=(void*)_tmp9C->r;struct Cyc_Absyn_Structdecl*_tmp9F;struct Cyc_Absyn_Tuniondecl*
_tmpA0;struct Cyc_Absyn_Uniondecl*_tmpA1;struct Cyc_Absyn_Enumdecl*_tmpA2;_LLA5:
if(*((int*)_tmp9E)== 4){_LLAF: _tmp9F=((struct Cyc_Absyn_Struct_d_struct*)_tmp9E)->f1;
goto _LLA6;}else{goto _LLA7;}_LLA7: if(*((int*)_tmp9E)== 6){_LLB0: _tmpA0=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp9E)->f1;goto _LLA8;}else{goto _LLA9;}_LLA9: if(*((
int*)_tmp9E)== 5){_LLB1: _tmpA1=((struct Cyc_Absyn_Union_d_struct*)_tmp9E)->f1;
goto _LLAA;}else{goto _LLAB;}_LLAB: if(*((int*)_tmp9E)== 7){_LLB2: _tmpA2=((struct
Cyc_Absyn_Enum_d_struct*)_tmp9E)->f1;goto _LLAC;}else{goto _LLAD;}_LLAD: goto _LLAE;
_LLA6: {struct Cyc_List_List*_tmpA3=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmp9F->tvs));t=(void*)({struct Cyc_Absyn_StructType_struct*
_tmpA4=_cycalloc(sizeof(struct Cyc_Absyn_StructType_struct));_tmpA4[0]=({struct
Cyc_Absyn_StructType_struct _tmpA5;_tmpA5.tag=10;_tmpA5.f1=(struct _tuple1*)((
struct _tuple1*)((struct Cyc_Core_Opt*)_check_null(_tmp9F->name))->v);_tmpA5.f2=
_tmpA3;_tmpA5.f3=0;_tmpA5;});_tmpA4;});if(_tmp9F->fields != 0){declopt=({struct
Cyc_Core_Opt*_tmpA6=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpA6->v=_tmp9C;
_tmpA6;});}goto _LLA4;}_LLA8: {struct Cyc_List_List*_tmpA7=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmpA0->tvs));t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmpA8=_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));
_tmpA8[0]=({struct Cyc_Absyn_TunionType_struct _tmpA9;_tmpA9.tag=2;_tmpA9.f1=({
struct Cyc_Absyn_TunionInfo _tmpAA;_tmpAA.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmpAB=_cycalloc(sizeof(struct Cyc_Absyn_KnownTunion_struct));_tmpAB[0]=({struct
Cyc_Absyn_KnownTunion_struct _tmpAC;_tmpAC.tag=1;_tmpAC.f1=({struct Cyc_Absyn_Tuniondecl**
_tmpAD=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl*));_tmpAD[0]=_tmpA0;_tmpAD;});
_tmpAC;});_tmpAB;}));_tmpAA.targs=_tmpA7;_tmpAA.rgn=(void*)((void*)2);_tmpAA;});
_tmpA9;});_tmpA8;});if(_tmpA0->fields != 0){declopt=({struct Cyc_Core_Opt*_tmpAE=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpAE->v=_tmp9C;_tmpAE;});}goto _LLA4;}
_LLAA: {struct Cyc_List_List*_tmpAF=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmpA1->tvs));t=(void*)({struct Cyc_Absyn_UnionType_struct*
_tmpB0=_cycalloc(sizeof(struct Cyc_Absyn_UnionType_struct));_tmpB0[0]=({struct Cyc_Absyn_UnionType_struct
_tmpB1;_tmpB1.tag=11;_tmpB1.f1=(struct _tuple1*)((struct _tuple1*)((struct Cyc_Core_Opt*)
_check_null(_tmpA1->name))->v);_tmpB1.f2=_tmpAF;_tmpB1.f3=0;_tmpB1;});_tmpB0;});
if(_tmpA1->fields != 0){declopt=({struct Cyc_Core_Opt*_tmpB2=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmpB2->v=_tmp9C;_tmpB2;});}goto _LLA4;}_LLAC: t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmpB3=_cycalloc(sizeof(struct Cyc_Absyn_EnumType_struct));
_tmpB3[0]=({struct Cyc_Absyn_EnumType_struct _tmpB4;_tmpB4.tag=12;_tmpB4.f1=_tmpA2->name;
_tmpB4.f2=0;_tmpB4;});_tmpB3;});declopt=({struct Cyc_Core_Opt*_tmpB5=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmpB5->v=_tmp9C;_tmpB5;});goto _LLA4;_LLAE:((int(*)(
struct _tagged_arr msg,struct Cyc_Position_Segment*sg))Cyc_Parse_abort)(_tag_arr("bad declaration within type specifier",
sizeof(unsigned char),38),_tmp9C->loc);goto _LLA4;_LLA4:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,
_tmp9C->loc);}goto _LL8B;_LL8B:;}if(! seen_type){if(! seen_sign? ! seen_size: 0){({
void*_tmpB6[0]={};Cyc_Tcutil_warn(last_loc,_tag_arr("missing type within specifier",
sizeof(unsigned char),30),_tag_arr(_tmpB6,sizeof(void*),0));});}t=(void*)({
struct Cyc_Absyn_IntType_struct*_tmpB7=_cycalloc(sizeof(struct Cyc_Absyn_IntType_struct));
_tmpB7[0]=({struct Cyc_Absyn_IntType_struct _tmpB8;_tmpB8.tag=5;_tmpB8.f1=(void*)
sgn;_tmpB8.f2=(void*)sz;_tmpB8;});_tmpB7;});}else{if(seen_sign){void*_tmpB9=t;
void*_tmpBA;_LLB4: if((unsigned int)_tmpB9 > 3?*((int*)_tmpB9)== 5: 0){_LLB8: _tmpBA=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmpB9)->f2;goto _LLB5;}else{goto _LLB6;}
_LLB6: goto _LLB7;_LLB5: t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpBB=_cycalloc(
sizeof(struct Cyc_Absyn_IntType_struct));_tmpBB[0]=({struct Cyc_Absyn_IntType_struct
_tmpBC;_tmpBC.tag=5;_tmpBC.f1=(void*)sgn;_tmpBC.f2=(void*)_tmpBA;_tmpBC;});
_tmpBB;});goto _LLB3;_LLB7: Cyc_Parse_err(_tag_arr("sign specification on non-integral type",
sizeof(unsigned char),40),last_loc);goto _LLB3;_LLB3:;}if(seen_size){void*_tmpBD=
t;void*_tmpBE;_LLBA: if((unsigned int)_tmpBD > 3?*((int*)_tmpBD)== 5: 0){_LLC0:
_tmpBE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBD)->f1;goto _LLBB;}else{goto
_LLBC;}_LLBC: if((unsigned int)_tmpBD > 3?*((int*)_tmpBD)== 6: 0){goto _LLBD;}else{
goto _LLBE;}_LLBE: goto _LLBF;_LLBB: t=(void*)({struct Cyc_Absyn_IntType_struct*
_tmpBF=_cycalloc(sizeof(struct Cyc_Absyn_IntType_struct));_tmpBF[0]=({struct Cyc_Absyn_IntType_struct
_tmpC0;_tmpC0.tag=5;_tmpC0.f1=(void*)_tmpBE;_tmpC0.f2=(void*)sz;_tmpC0;});_tmpBF;});
goto _LLB9;_LLBD: t=(void*)({struct Cyc_Absyn_DoubleType_struct*_tmpC1=
_cycalloc_atomic(sizeof(struct Cyc_Absyn_DoubleType_struct));_tmpC1[0]=({struct
Cyc_Absyn_DoubleType_struct _tmpC2;_tmpC2.tag=6;_tmpC2.f1=1;_tmpC2;});_tmpC1;});
goto _LLB9;_LLBF: Cyc_Parse_err(_tag_arr("size qualifier on non-integral type",
sizeof(unsigned char),36),last_loc);goto _LLB9;_LLB9:;}}return({struct _tuple5
_tmpC3;_tmpC3.f1=t;_tmpC3.f2=declopt;_tmpC3;});}static struct Cyc_List_List*Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*
shared_atts){if(ds == 0){return 0;}{struct Cyc_Parse_Declarator*_tmpC4=(struct Cyc_Parse_Declarator*)
ds->hd;struct _tuple1*_tmpC5=_tmpC4->id;struct Cyc_List_List*_tmpC7;struct Cyc_List_List*
_tmpC8;void*_tmpC9;struct Cyc_Absyn_Tqual _tmpCA;struct _tuple6 _tmpC6=Cyc_Parse_apply_tms(
tq,t,shared_atts,_tmpC4->tms);_LLC5: _tmpCA=_tmpC6.f1;goto _LLC4;_LLC4: _tmpC9=
_tmpC6.f2;goto _LLC3;_LLC3: _tmpC8=_tmpC6.f3;goto _LLC2;_LLC2: _tmpC7=_tmpC6.f4;goto
_LLC1;_LLC1: return({struct Cyc_List_List*_tmpCB=_cycalloc(sizeof(struct Cyc_List_List));
_tmpCB->hd=({struct _tuple7*_tmpCC=_cycalloc(sizeof(struct _tuple7));_tmpCC->f1=
_tmpC5;_tmpCC->f2=_tmpCA;_tmpCC->f3=_tmpC9;_tmpCC->f4=_tmpC8;_tmpCC->f5=_tmpC7;
_tmpCC;});_tmpCB->tl=Cyc_Parse_apply_tmss(_tmpCA,Cyc_Tcutil_copy_type(t),ds->tl,
shared_atts);_tmpCB;});}}static struct _tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){if(tms == 0){return({
struct _tuple6 _tmpCD;_tmpCD.f1=tq;_tmpCD.f2=t;_tmpCD.f3=0;_tmpCD.f4=atts;_tmpCD;});}{
void*_tmpCE=(void*)tms->hd;struct Cyc_Absyn_Exp*_tmpCF;void*_tmpD0;struct Cyc_Position_Segment*
_tmpD1;struct Cyc_List_List*_tmpD2;struct Cyc_Absyn_Tqual _tmpD3;void*_tmpD4;void*
_tmpD5;struct Cyc_List_List*_tmpD6;struct Cyc_Position_Segment*_tmpD7;_LLC7: if((
int)_tmpCE == 0){goto _LLC8;}else{goto _LLC9;}_LLC9: if((unsigned int)_tmpCE > 1?*((
int*)_tmpCE)== 0: 0){_LLD3: _tmpCF=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCE)->f1;
goto _LLCA;}else{goto _LLCB;}_LLCB: if((unsigned int)_tmpCE > 1?*((int*)_tmpCE)== 2:
0){_LLD4: _tmpD0=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmpCE)->f1;goto
_LLCC;}else{goto _LLCD;}_LLCD: if((unsigned int)_tmpCE > 1?*((int*)_tmpCE)== 3: 0){
_LLD6: _tmpD2=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCE)->f1;goto _LLD5;
_LLD5: _tmpD1=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCE)->f2;goto _LLCE;}
else{goto _LLCF;}_LLCF: if((unsigned int)_tmpCE > 1?*((int*)_tmpCE)== 1: 0){_LLD9:
_tmpD5=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCE)->f1;goto _LLD8;_LLD8:
_tmpD4=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCE)->f2;goto _LLD7;_LLD7:
_tmpD3=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCE)->f3;goto _LLD0;}else{goto
_LLD1;}_LLD1: if((unsigned int)_tmpCE > 1?*((int*)_tmpCE)== 4: 0){_LLDB: _tmpD7=((
struct Cyc_Absyn_Attributes_mod_struct*)_tmpCE)->f1;goto _LLDA;_LLDA: _tmpD6=((
struct Cyc_Absyn_Attributes_mod_struct*)_tmpCE)->f2;goto _LLD2;}else{goto _LLC6;}
_LLC8: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpD8=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmpD8[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpD9;_tmpD9.tag=7;_tmpD9.f1=(void*)t;_tmpD9.f2=tq;_tmpD9.f3=0;_tmpD9;});_tmpD8;}),
atts,tms->tl);_LLCA: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmpDA=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));
_tmpDA[0]=({struct Cyc_Absyn_ArrayType_struct _tmpDB;_tmpDB.tag=7;_tmpDB.f1=(void*)
t;_tmpDB.f2=tq;_tmpDB.f3=(struct Cyc_Absyn_Exp*)_tmpCF;_tmpDB;});_tmpDA;}),atts,
tms->tl);_LLCC: {void*_tmpDC=_tmpD0;struct Cyc_List_List*_tmpDD;struct Cyc_Core_Opt*
_tmpDE;struct Cyc_Absyn_VarargInfo*_tmpDF;int _tmpE0;struct Cyc_List_List*_tmpE1;
struct Cyc_Position_Segment*_tmpE2;_LLDD: if(*((int*)_tmpDC)== 1){_LLE5: _tmpE1=((
struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f1;goto _LLE4;_LLE4: _tmpE0=((struct Cyc_Absyn_WithTypes_struct*)
_tmpDC)->f2;goto _LLE3;_LLE3: _tmpDF=((struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f3;
goto _LLE2;_LLE2: _tmpDE=((struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f4;goto _LLE1;
_LLE1: _tmpDD=((struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f5;goto _LLDE;}else{
goto _LLDF;}_LLDF: if(*((int*)_tmpDC)== 0){_LLE6: _tmpE2=((struct Cyc_Absyn_NoTypes_struct*)
_tmpDC)->f2;goto _LLE0;}else{goto _LLDC;}_LLDE: {struct Cyc_List_List*typvars=0;
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct Cyc_List_List*
as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd)){fn_atts=({
struct Cyc_List_List*_tmpE3=_cycalloc(sizeof(struct Cyc_List_List));_tmpE3->hd=(
void*)((void*)as->hd);_tmpE3->tl=fn_atts;_tmpE3;});}else{new_atts=({struct Cyc_List_List*
_tmpE4=_cycalloc(sizeof(struct Cyc_List_List));_tmpE4->hd=(void*)((void*)as->hd);
_tmpE4->tl=new_atts;_tmpE4;});}}}if(tms->tl != 0){void*_tmpE5=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpE6;_LLE8: if((unsigned int)
_tmpE5 > 1?*((int*)_tmpE5)== 3: 0){_LLEC: _tmpE6=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpE5)->f1;goto _LLE9;}else{goto _LLEA;}_LLEA: goto _LLEB;_LLE9: typvars=_tmpE6;tms=
tms->tl;goto _LLE7;_LLEB: goto _LLE7;_LLE7:;}if(((((! _tmpE0? _tmpDF == 0: 0)? _tmpE1 != 
0: 0)? _tmpE1->tl == 0: 0)?(*((struct _tuple2*)_tmpE1->hd)).f1 == 0: 0)?(*((struct
_tuple2*)_tmpE1->hd)).f3 == (void*)0: 0){_tmpE1=0;}t=Cyc_Parse_array2ptr(t,0);((
void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,
_tmpE1);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),Cyc_Absyn_function_typ(
typvars,_tmpDE,t,_tmpE1,_tmpE0,_tmpDF,_tmpDD,fn_atts),new_atts,((struct Cyc_List_List*)
_check_null(tms))->tl);}_LLE0:(int)_throw(Cyc_Parse_abort(_tag_arr("function declaration without parameter types",
sizeof(unsigned char),45),_tmpE2));_LLDC:;}_LLCE: if(tms->tl == 0){return({struct
_tuple6 _tmpE7;_tmpE7.f1=tq;_tmpE7.f2=t;_tmpE7.f3=_tmpD2;_tmpE7.f4=atts;_tmpE7;});}(
int)_throw(Cyc_Parse_abort(_tag_arr("type parameters must appear before function arguments in declarator",
sizeof(unsigned char),68),_tmpD1));_LLD0: {void*_tmpE8=_tmpD5;struct Cyc_Absyn_Exp*
_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;_LLEE: if((unsigned int)_tmpE8 > 1?*((int*)
_tmpE8)== 0: 0){_LLF4: _tmpE9=((struct Cyc_Absyn_NonNullable_ps_struct*)_tmpE8)->f1;
goto _LLEF;}else{goto _LLF0;}_LLF0: if((unsigned int)_tmpE8 > 1?*((int*)_tmpE8)== 1:
0){_LLF5: _tmpEA=((struct Cyc_Absyn_Nullable_ps_struct*)_tmpE8)->f1;goto _LLF1;}
else{goto _LLF2;}_LLF2: if((int)_tmpE8 == 0){goto _LLF3;}else{goto _LLED;}_LLEF:
return Cyc_Parse_apply_tms(_tmpD3,Cyc_Absyn_atb_typ(t,_tmpD4,tq,(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmpEB=_cycalloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
_tmpEB[0]=({struct Cyc_Absyn_Upper_b_struct _tmpEC;_tmpEC.tag=0;_tmpEC.f1=_tmpE9;
_tmpEC;});_tmpEB;})),atts,tms->tl);_LLF1: return Cyc_Parse_apply_tms(_tmpD3,Cyc_Absyn_starb_typ(
t,_tmpD4,tq,(void*)({struct Cyc_Absyn_Upper_b_struct*_tmpED=_cycalloc(sizeof(
struct Cyc_Absyn_Upper_b_struct));_tmpED[0]=({struct Cyc_Absyn_Upper_b_struct
_tmpEE;_tmpEE.tag=0;_tmpEE.f1=_tmpEA;_tmpEE;});_tmpED;})),atts,tms->tl);_LLF3:
return Cyc_Parse_apply_tms(_tmpD3,Cyc_Absyn_tagged_typ(t,_tmpD4,tq),atts,tms->tl);
_LLED:;}_LLD2: return Cyc_Parse_apply_tms(tq,t,Cyc_List_append(atts,_tmpD6),tms->tl);
_LLC6:;}}void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc){struct Cyc_Core_Opt*_tmpF0;void*_tmpF1;struct _tuple5 _tmpEF=Cyc_Parse_collapse_type_specifiers(
tss,loc);_LLF8: _tmpF1=_tmpEF.f1;goto _LLF7;_LLF7: _tmpF0=_tmpEF.f2;goto _LLF6;_LLF6:
if(_tmpF0 != 0){({void*_tmpF2[0]={};Cyc_Tcutil_warn(loc,_tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof(unsigned char),54),_tag_arr(_tmpF2,sizeof(void*),0));});}return _tmpF1;}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct
Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpF3=_cycalloc(sizeof(struct Cyc_Absyn_Decl_s_struct));_tmpF3[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpF4;_tmpF4.tag=12;_tmpF4.f1=d;_tmpF4.f2=s;_tmpF4;});_tmpF3;}),Cyc_Position_segment_join(
d->loc,s->loc));}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*loc){struct Cyc_List_List*
tss=ds->type_specs;struct Cyc_Absyn_Tqual tq=ds->tq;int istypedef=0;void*s=(void*)2;
struct Cyc_List_List*atts=ds->attributes;if(ds->is_inline){Cyc_Parse_err(_tag_arr("inline is only allowed on function definitions",
sizeof(unsigned char),47),loc);}if(tss == 0){Cyc_Parse_err(_tag_arr("missing type specifiers in declaration",
sizeof(unsigned char),39),loc);return 0;}if(ds->sc != 0){void*_tmpF5=(void*)((
struct Cyc_Core_Opt*)_check_null(ds->sc))->v;_LLFA: if((int)_tmpF5 == 0){goto _LLFB;}
else{goto _LLFC;}_LLFC: if((int)_tmpF5 == 1){goto _LLFD;}else{goto _LLFE;}_LLFE: if((
int)_tmpF5 == 2){goto _LLFF;}else{goto _LL100;}_LL100: if((int)_tmpF5 == 3){goto
_LL101;}else{goto _LL102;}_LL102: if((int)_tmpF5 == 4){goto _LL103;}else{goto _LL104;}
_LL104: if((int)_tmpF5 == 5){goto _LL105;}else{goto _LL106;}_LL106: if((int)_tmpF5 == 
6){goto _LL107;}else{goto _LLF9;}_LLFB: istypedef=1;goto _LLF9;_LLFD: s=(void*)3;goto
_LLF9;_LLFF: s=(void*)4;goto _LLF9;_LL101: s=(void*)0;goto _LLF9;_LL103: s=(void*)2;
goto _LLF9;_LL105: s=(void*)2;goto _LLF9;_LL107: s=(void*)1;goto _LLF9;_LLF9:;}{
struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpF8;struct _tuple0 _tmpF6=((
struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(ids);_LL10A: _tmpF8=
_tmpF6.f1;goto _LL109;_LL109: _tmpF7=_tmpF6.f2;goto _LL108;_LL108: {int exps_empty=1;{
struct Cyc_List_List*es=_tmpF7;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)
es->hd != 0){exps_empty=0;break;}}}{struct _tuple5 _tmpF9=Cyc_Parse_collapse_type_specifiers(
tss,loc);if(_tmpF8 == 0){struct Cyc_Core_Opt*_tmpFB;void*_tmpFC;struct _tuple5
_tmpFA=_tmpF9;_LL10D: _tmpFC=_tmpFA.f1;goto _LL10C;_LL10C: _tmpFB=_tmpFA.f2;goto
_LL10B;_LL10B: if(_tmpFB != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
_tmpFB->v;{void*_tmpFD=(void*)d->r;struct Cyc_Absyn_Enumdecl*_tmpFE;struct Cyc_Absyn_Structdecl*
_tmpFF;struct Cyc_Absyn_Uniondecl*_tmp100;struct Cyc_Absyn_Tuniondecl*_tmp101;
_LL10F: if(*((int*)_tmpFD)== 7){_LL119: _tmpFE=((struct Cyc_Absyn_Enum_d_struct*)
_tmpFD)->f1;goto _LL110;}else{goto _LL111;}_LL111: if(*((int*)_tmpFD)== 4){_LL11A:
_tmpFF=((struct Cyc_Absyn_Struct_d_struct*)_tmpFD)->f1;goto _LL112;}else{goto
_LL113;}_LL113: if(*((int*)_tmpFD)== 5){_LL11B: _tmp100=((struct Cyc_Absyn_Union_d_struct*)
_tmpFD)->f1;goto _LL114;}else{goto _LL115;}_LL115: if(*((int*)_tmpFD)== 6){_LL11C:
_tmp101=((struct Cyc_Absyn_Tunion_d_struct*)_tmpFD)->f1;goto _LL116;}else{goto
_LL117;}_LL117: goto _LL118;_LL110:(void*)(_tmpFE->sc=(void*)s);if(atts != 0){Cyc_Parse_err(
_tag_arr("bad attributes on enum",sizeof(unsigned char),23),loc);}goto _LL10E;
_LL112:(void*)(_tmpFF->sc=(void*)s);_tmpFF->attributes=atts;goto _LL10E;_LL114:(
void*)(_tmp100->sc=(void*)s);_tmp100->attributes=atts;goto _LL10E;_LL116:(void*)(
_tmp101->sc=(void*)s);if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on tunion",
sizeof(unsigned char),25),loc);}goto _LL10E;_LL118: Cyc_Parse_err(_tag_arr("bad declaration",
sizeof(unsigned char),16),loc);return 0;_LL10E:;}return({struct Cyc_List_List*
_tmp102=_cycalloc(sizeof(struct Cyc_List_List));_tmp102->hd=d;_tmp102->tl=0;
_tmp102;});}else{void*_tmp103=_tmpFC;struct Cyc_List_List*_tmp104;struct _tuple1*
_tmp105;struct Cyc_Absyn_TunionInfo _tmp106;void*_tmp107;struct Cyc_Absyn_Tuniondecl**
_tmp108;struct Cyc_Absyn_TunionInfo _tmp109;struct Cyc_List_List*_tmp10A;void*
_tmp10B;struct Cyc_Absyn_UnknownTunionInfo _tmp10C;int _tmp10D;struct _tuple1*
_tmp10E;struct Cyc_List_List*_tmp10F;struct _tuple1*_tmp110;struct _tuple1*_tmp111;
struct Cyc_List_List*_tmp112;_LL11E: if((unsigned int)_tmp103 > 3?*((int*)_tmp103)
== 10: 0){_LL12D: _tmp105=((struct Cyc_Absyn_StructType_struct*)_tmp103)->f1;goto
_LL12C;_LL12C: _tmp104=((struct Cyc_Absyn_StructType_struct*)_tmp103)->f2;goto
_LL11F;}else{goto _LL120;}_LL120: if((unsigned int)_tmp103 > 3?*((int*)_tmp103)== 2:
0){_LL12E: _tmp106=((struct Cyc_Absyn_TunionType_struct*)_tmp103)->f1;_LL12F:
_tmp107=(void*)_tmp106.tunion_info;if(*((int*)_tmp107)== 1){_LL130: _tmp108=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp107)->f1;goto _LL121;}else{goto _LL122;}}
else{goto _LL122;}_LL122: if((unsigned int)_tmp103 > 3?*((int*)_tmp103)== 2: 0){
_LL131: _tmp109=((struct Cyc_Absyn_TunionType_struct*)_tmp103)->f1;_LL133: _tmp10B=(
void*)_tmp109.tunion_info;if(*((int*)_tmp10B)== 0){_LL134: _tmp10C=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp10B)->f1;_LL136: _tmp10E=_tmp10C.name;goto _LL135;_LL135: _tmp10D=_tmp10C.is_xtunion;
goto _LL132;}else{goto _LL124;}_LL132: _tmp10A=_tmp109.targs;goto _LL123;}else{goto
_LL124;}_LL124: if((unsigned int)_tmp103 > 3?*((int*)_tmp103)== 11: 0){_LL138:
_tmp110=((struct Cyc_Absyn_UnionType_struct*)_tmp103)->f1;goto _LL137;_LL137:
_tmp10F=((struct Cyc_Absyn_UnionType_struct*)_tmp103)->f2;goto _LL125;}else{goto
_LL126;}_LL126: if((unsigned int)_tmp103 > 3?*((int*)_tmp103)== 12: 0){_LL139:
_tmp111=((struct Cyc_Absyn_EnumType_struct*)_tmp103)->f1;goto _LL127;}else{goto
_LL128;}_LL128: if((unsigned int)_tmp103 > 3?*((int*)_tmp103)== 16: 0){_LL13A:
_tmp112=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp103)->f1;goto _LL129;}else{
goto _LL12A;}_LL12A: goto _LL12B;_LL11F: {struct Cyc_List_List*_tmp113=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp104);
struct Cyc_Absyn_Structdecl*_tmp114=({struct Cyc_Absyn_Structdecl*_tmp118=
_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));_tmp118->sc=(void*)s;_tmp118->name=({
struct Cyc_Core_Opt*_tmp119=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp119->v=(
struct _tuple1*)_check_null(_tmp105);_tmp119;});_tmp118->tvs=_tmp113;_tmp118->fields=
0;_tmp118->attributes=0;_tmp118;});if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on struct",
sizeof(unsigned char),25),loc);}return({struct Cyc_List_List*_tmp115=_cycalloc(
sizeof(struct Cyc_List_List));_tmp115->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Struct_d_struct*
_tmp116=_cycalloc(sizeof(struct Cyc_Absyn_Struct_d_struct));_tmp116[0]=({struct
Cyc_Absyn_Struct_d_struct _tmp117;_tmp117.tag=4;_tmp117.f1=_tmp114;_tmp117;});
_tmp116;}),loc);_tmp115->tl=0;_tmp115;});}_LL121: if(atts != 0){Cyc_Parse_err(
_tag_arr("bad attributes on tunion",sizeof(unsigned char),25),loc);}return({
struct Cyc_List_List*_tmp11A=_cycalloc(sizeof(struct Cyc_List_List));_tmp11A->hd=
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp11B=_cycalloc(
sizeof(struct Cyc_Absyn_Tunion_d_struct));_tmp11B[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp11C;_tmp11C.tag=6;_tmp11C.f1=*_tmp108;_tmp11C;});_tmp11B;}),loc);_tmp11A->tl=
0;_tmp11A;});_LL123: {struct Cyc_List_List*_tmp11D=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp10A);
struct Cyc_Absyn_Decl*_tmp11E=Cyc_Absyn_tunion_decl(s,_tmp10E,_tmp11D,0,_tmp10D,
loc);if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on tunion",sizeof(
unsigned char),25),loc);}return({struct Cyc_List_List*_tmp11F=_cycalloc(sizeof(
struct Cyc_List_List));_tmp11F->hd=_tmp11E;_tmp11F->tl=0;_tmp11F;});}_LL125: {
struct Cyc_List_List*_tmp120=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp10F);struct Cyc_Absyn_Uniondecl*
_tmp121=({struct Cyc_Absyn_Uniondecl*_tmp126=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl));
_tmp126->sc=(void*)s;_tmp126->name=({struct Cyc_Core_Opt*_tmp127=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp127->v=(struct _tuple1*)_check_null(_tmp110);
_tmp127;});_tmp126->tvs=_tmp120;_tmp126->fields=0;_tmp126->attributes=0;_tmp126;});
if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on union",sizeof(
unsigned char),24),loc);}return({struct Cyc_List_List*_tmp122=_cycalloc(sizeof(
struct Cyc_List_List));_tmp122->hd=({struct Cyc_Absyn_Decl*_tmp123=_cycalloc(
sizeof(struct Cyc_Absyn_Decl));_tmp123->r=(void*)((void*)({struct Cyc_Absyn_Union_d_struct*
_tmp124=_cycalloc(sizeof(struct Cyc_Absyn_Union_d_struct));_tmp124[0]=({struct Cyc_Absyn_Union_d_struct
_tmp125;_tmp125.tag=5;_tmp125.f1=_tmp121;_tmp125;});_tmp124;}));_tmp123->loc=loc;
_tmp123;});_tmp122->tl=0;_tmp122;});}_LL127: {struct Cyc_Absyn_Enumdecl*_tmp128=({
struct Cyc_Absyn_Enumdecl*_tmp12D=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));
_tmp12D->sc=(void*)s;_tmp12D->name=_tmp111;_tmp12D->fields=0;_tmp12D;});if(atts
!= 0){Cyc_Parse_err(_tag_arr("bad attributes on enum",sizeof(unsigned char),23),
loc);}return({struct Cyc_List_List*_tmp129=_cycalloc(sizeof(struct Cyc_List_List));
_tmp129->hd=({struct Cyc_Absyn_Decl*_tmp12A=_cycalloc(sizeof(struct Cyc_Absyn_Decl));
_tmp12A->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp12B=_cycalloc(
sizeof(struct Cyc_Absyn_Enum_d_struct));_tmp12B[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp12C;_tmp12C.tag=7;_tmp12C.f1=_tmp128;_tmp12C;});_tmp12B;}));_tmp12A->loc=loc;
_tmp12A;});_tmp129->tl=0;_tmp129;});}_LL129: {struct Cyc_Absyn_Enumdecl*_tmp12E=({
struct Cyc_Absyn_Enumdecl*_tmp133=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));
_tmp133->sc=(void*)s;_tmp133->name=Cyc_Parse_gensym_enum();_tmp133->fields=({
struct Cyc_Core_Opt*_tmp134=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp134->v=
_tmp112;_tmp134;});_tmp133;});if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on enum",
sizeof(unsigned char),23),loc);}return({struct Cyc_List_List*_tmp12F=_cycalloc(
sizeof(struct Cyc_List_List));_tmp12F->hd=({struct Cyc_Absyn_Decl*_tmp130=
_cycalloc(sizeof(struct Cyc_Absyn_Decl));_tmp130->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp131=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_struct));_tmp131[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp132;_tmp132.tag=7;_tmp132.f1=_tmp12E;_tmp132;});_tmp131;}));_tmp130->loc=loc;
_tmp130;});_tmp12F->tl=0;_tmp12F;});}_LL12B: Cyc_Parse_err(_tag_arr("missing declarator",
sizeof(unsigned char),19),loc);return 0;_LL11D:;}}else{void*t=_tmpF9.f1;struct Cyc_List_List*
_tmp135=Cyc_Parse_apply_tmss(tq,t,_tmpF8,atts);if(istypedef){if(! exps_empty){Cyc_Parse_err(
_tag_arr("initializer in typedef declaration",sizeof(unsigned char),35),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(
struct Cyc_Position_Segment*,struct _tuple7*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp135);
if(_tmpF9.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(_tmpF9.f2)->v;{
void*_tmp136=(void*)d->r;struct Cyc_Absyn_Structdecl*_tmp137;struct Cyc_Absyn_Tuniondecl*
_tmp138;struct Cyc_Absyn_Uniondecl*_tmp139;struct Cyc_Absyn_Enumdecl*_tmp13A;
_LL13C: if(*((int*)_tmp136)== 4){_LL146: _tmp137=((struct Cyc_Absyn_Struct_d_struct*)
_tmp136)->f1;goto _LL13D;}else{goto _LL13E;}_LL13E: if(*((int*)_tmp136)== 6){_LL147:
_tmp138=((struct Cyc_Absyn_Tunion_d_struct*)_tmp136)->f1;goto _LL13F;}else{goto
_LL140;}_LL140: if(*((int*)_tmp136)== 5){_LL148: _tmp139=((struct Cyc_Absyn_Union_d_struct*)
_tmp136)->f1;goto _LL141;}else{goto _LL142;}_LL142: if(*((int*)_tmp136)== 7){_LL149:
_tmp13A=((struct Cyc_Absyn_Enum_d_struct*)_tmp136)->f1;goto _LL143;}else{goto
_LL144;}_LL144: goto _LL145;_LL13D:(void*)(_tmp137->sc=(void*)s);_tmp137->attributes=
atts;atts=0;goto _LL13B;_LL13F:(void*)(_tmp138->sc=(void*)s);goto _LL13B;_LL141:(
void*)(_tmp139->sc=(void*)s);goto _LL13B;_LL143:(void*)(_tmp13A->sc=(void*)s);
goto _LL13B;_LL145: Cyc_Parse_err(_tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof(unsigned char),70),loc);goto _LL13B;_LL13B:;}decls=({struct Cyc_List_List*
_tmp13B=_cycalloc(sizeof(struct Cyc_List_List));_tmp13B->hd=d;_tmp13B->tl=decls;
_tmp13B;});}if(atts != 0){Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp13D;_tmp13D.tag=0;_tmp13D.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp13C[1]={& _tmp13D};Cyc_Std_aprintf(_tag_arr("bad attribute %s in typedef",
sizeof(unsigned char),28),_tag_arr(_tmp13C,sizeof(void*),1));}}),loc);}return
decls;}}else{if(_tmpF9.f2 != 0){Cyc_Parse_unimp2(_tag_arr("nested type declaration within declarator",
sizeof(unsigned char),42),loc);}{struct Cyc_List_List*decls=0;{struct Cyc_List_List*
_tmp13E=_tmp135;for(0;_tmp13E != 0;(_tmp13E=_tmp13E->tl,_tmpF7=_tmpF7->tl)){
struct _tuple7 _tmp140;struct Cyc_List_List*_tmp141;struct Cyc_List_List*_tmp142;
void*_tmp143;struct Cyc_Absyn_Tqual _tmp144;struct _tuple1*_tmp145;struct _tuple7*
_tmp13F=(struct _tuple7*)_tmp13E->hd;_tmp140=*_tmp13F;_LL14F: _tmp145=_tmp140.f1;
goto _LL14E;_LL14E: _tmp144=_tmp140.f2;goto _LL14D;_LL14D: _tmp143=_tmp140.f3;goto
_LL14C;_LL14C: _tmp142=_tmp140.f4;goto _LL14B;_LL14B: _tmp141=_tmp140.f5;goto _LL14A;
_LL14A: if(_tmp142 != 0){({void*_tmp146[0]={};Cyc_Tcutil_warn(loc,_tag_arr("bad type params, ignoring",
sizeof(unsigned char),26),_tag_arr(_tmp146,sizeof(void*),0));});}if(_tmpF7 == 0){((
int(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*sg))Cyc_Parse_abort)(
_tag_arr("unexpected NULL in parse!",sizeof(unsigned char),26),loc);}{struct Cyc_Absyn_Exp*
_tmp147=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpF7))->hd;
struct Cyc_Absyn_Vardecl*_tmp148=Cyc_Absyn_new_vardecl(_tmp145,_tmp143,_tmp147);
_tmp148->tq=_tmp144;(void*)(_tmp148->sc=(void*)s);_tmp148->attributes=_tmp141;{
struct Cyc_Absyn_Decl*_tmp149=({struct Cyc_Absyn_Decl*_tmp14B=_cycalloc(sizeof(
struct Cyc_Absyn_Decl));_tmp14B->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp14C=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_struct));_tmp14C[0]=({struct Cyc_Absyn_Var_d_struct
_tmp14D;_tmp14D.tag=0;_tmp14D.f1=_tmp148;_tmp14D;});_tmp14C;}));_tmp14B->loc=loc;
_tmp14B;});decls=({struct Cyc_List_List*_tmp14A=_cycalloc(sizeof(struct Cyc_List_List));
_tmp14A->hd=_tmp149;_tmp14A->tl=decls;_tmp14A;});}}}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);}}}}}}}static void*Cyc_Parse_id_to_kind(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){if(Cyc_Std_strlen(s)!= 1){Cyc_Parse_err((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp14F;_tmp14F.tag=0;_tmp14F.f1=(
struct _tagged_arr)s;{void*_tmp14E[1]={& _tmp14F};Cyc_Std_aprintf(_tag_arr("bad kind: %s",
sizeof(unsigned char),13),_tag_arr(_tmp14E,sizeof(void*),1));}}),loc);return(
void*)2;}else{switch(*((const unsigned char*)_check_unknown_subscript(s,sizeof(
unsigned char),0))){case 'A': _LL150: return(void*)0;case 'M': _LL151: return(void*)1;
case 'B': _LL152: return(void*)2;case 'R': _LL153: return(void*)3;case 'E': _LL154:
return(void*)4;default: _LL155: Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp151;_tmp151.tag=0;_tmp151.f1=(struct _tagged_arr)s;{void*_tmp150[1]={& _tmp151};
Cyc_Std_aprintf(_tag_arr("bad kind: %s",sizeof(unsigned char),13),_tag_arr(
_tmp150,sizeof(void*),1));}}),loc);return(void*)2;}}}static struct Cyc_List_List*
Cyc_Parse_attopt_to_tms(struct Cyc_Position_Segment*loc,struct Cyc_List_List*atts,
struct Cyc_List_List*tms){if(atts == 0){return tms;}else{return({struct Cyc_List_List*
_tmp152=_cycalloc(sizeof(struct Cyc_List_List));_tmp152->hd=(void*)((void*)({
struct Cyc_Absyn_Attributes_mod_struct*_tmp153=_cycalloc(sizeof(struct Cyc_Absyn_Attributes_mod_struct));
_tmp153[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp154;_tmp154.tag=4;_tmp154.f1=
loc;_tmp154.f2=atts;_tmp154;});_tmp153;}));_tmp152->tl=tms;_tmp152;});}}static
struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*loc,
struct _tuple7*t){struct _tuple7 _tmp156;struct Cyc_List_List*_tmp157;struct Cyc_List_List*
_tmp158;void*_tmp159;struct Cyc_Absyn_Tqual _tmp15A;struct _tuple1*_tmp15B;struct
_tuple7*_tmp155=t;_tmp156=*_tmp155;_LL15C: _tmp15B=_tmp156.f1;goto _LL15B;_LL15B:
_tmp15A=_tmp156.f2;goto _LL15A;_LL15A: _tmp159=_tmp156.f3;goto _LL159;_LL159:
_tmp158=_tmp156.f4;goto _LL158;_LL158: _tmp157=_tmp156.f5;goto _LL157;_LL157: Cyc_Lex_register_typedef(
_tmp15B);if(_tmp157 != 0){Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp15D;_tmp15D.tag=0;_tmp15D.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp157->hd);{void*_tmp15C[1]={& _tmp15D};Cyc_Std_aprintf(_tag_arr("bad attribute %s within typedef",
sizeof(unsigned char),32),_tag_arr(_tmp15C,sizeof(void*),1));}}),loc);}return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Typedef_d_struct*_tmp15E=_cycalloc(sizeof(struct Cyc_Absyn_Typedef_d_struct));
_tmp15E[0]=({struct Cyc_Absyn_Typedef_d_struct _tmp15F;_tmp15F.tag=8;_tmp15F.f1=({
struct Cyc_Absyn_Typedefdecl*_tmp160=_cycalloc(sizeof(struct Cyc_Absyn_Typedefdecl));
_tmp160->name=_tmp15B;_tmp160->tvs=_tmp158;_tmp160->defn=(void*)_tmp159;_tmp160;});
_tmp15F;});_tmp15E;}),loc);}unsigned char Cyc_AbstractDeclarator_tok[27]="\000\000\000\000AbstractDeclarator_tok";
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
struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
unsigned char*tag;struct _tuple13*f1;};unsigned char Cyc_FnDecl_tok[15]="\000\000\000\000FnDecl_tok";
struct Cyc_FnDecl_tok_struct{unsigned char*tag;struct Cyc_Absyn_Fndecl*f1;};
unsigned char Cyc_IdList_tok[15]="\000\000\000\000IdList_tok";struct Cyc_IdList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};unsigned char Cyc_InitDeclList_tok[21]="\000\000\000\000InitDeclList_tok";
struct Cyc_InitDeclList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};
unsigned char Cyc_InitDecl_tok[17]="\000\000\000\000InitDecl_tok";struct _tuple14{
struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
unsigned char*tag;struct _tuple14*f1;};unsigned char Cyc_InitializerList_tok[24]="\000\000\000\000InitializerList_tok";
struct Cyc_InitializerList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};
unsigned char Cyc_Int_tok[12]="\000\000\000\000Int_tok";struct _tuple15{void*f1;
int f2;};struct Cyc_Int_tok_struct{unsigned char*tag;struct _tuple15*f1;};
unsigned char Cyc_Kind_tok[13]="\000\000\000\000Kind_tok";struct Cyc_Kind_tok_struct{
unsigned char*tag;void*f1;};unsigned char Cyc_ParamDeclListBool_tok[26]="\000\000\000\000ParamDeclListBool_tok";
struct _tuple16{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct
Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_ParamDeclListBool_tok_struct{
unsigned char*tag;struct _tuple16*f1;};unsigned char Cyc_ParamDeclList_tok[22]="\000\000\000\000ParamDeclList_tok";
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
struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct Cyc_QualSpecList_tok_struct{unsigned char*tag;struct _tuple17*f1;};
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
unsigned char*tag;struct _tuple16*f1;};unsigned char Cyc_YYINITIALSVAL[18]="\000\000\000\000YYINITIALSVAL";
unsigned char Cyc_Okay_tok[13]="\000\000\000\000Okay_tok";static unsigned char
_tmp162[15]="$(sign_t,int)@";static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{_tmp162,_tmp162,_tmp162 + 15}};struct _tuple15*Cyc_yyget_Int_tok(
void*yy1){struct _tuple15*yyzzz;{void*_tmp163=yy1;struct _tuple15*_tmp164;_LL15E:
if(*((void**)_tmp163)== Cyc_Int_tok){_LL162: _tmp164=((struct Cyc_Int_tok_struct*)
_tmp163)->f1;goto _LL15F;}else{goto _LL160;}_LL160: goto _LL161;_LL15F: yyzzz=_tmp164;
goto _LL15D;_LL161:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LL15D:;}return yyzzz;}
static unsigned char _tmp166[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp166,_tmp166,_tmp166 + 9}};struct _tagged_arr Cyc_yyget_String_tok(
void*yy1){struct _tagged_arr yyzzz;{void*_tmp167=yy1;struct _tagged_arr _tmp168;
_LL164: if(*((void**)_tmp167)== Cyc_String_tok){_LL168: _tmp168=((struct Cyc_String_tok_struct*)
_tmp167)->f1;goto _LL165;}else{goto _LL166;}_LL166: goto _LL167;_LL165: yyzzz=_tmp168;
goto _LL163;_LL167:(int)_throw((void*)& Cyc_yyfail_String_tok);_LL163:;}return
yyzzz;}static unsigned char _tmp16A[5]="char";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp16A,_tmp16A,_tmp16A + 5}};
unsigned char Cyc_yyget_Char_tok(void*yy1){unsigned char yyzzz;{void*_tmp16B=yy1;
unsigned char _tmp16C;_LL16A: if(*((void**)_tmp16B)== Cyc_Char_tok){_LL16E: _tmp16C=((
struct Cyc_Char_tok_struct*)_tmp16B)->f1;goto _LL16B;}else{goto _LL16C;}_LL16C: goto
_LL16D;_LL16B: yyzzz=_tmp16C;goto _LL169;_LL16D:(int)_throw((void*)& Cyc_yyfail_Char_tok);
_LL169:;}return yyzzz;}static unsigned char _tmp16E[20]="tunion Pointer_Sort";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={Cyc_Core_Failure,{
_tmp16E,_tmp16E,_tmp16E + 20}};void*Cyc_yyget_Pointer_Sort_tok(void*yy1){void*
yyzzz;{void*_tmp16F=yy1;void*_tmp170;_LL170: if(*((void**)_tmp16F)== Cyc_Pointer_Sort_tok){
_LL174: _tmp170=(void*)((struct Cyc_Pointer_Sort_tok_struct*)_tmp16F)->f1;goto
_LL171;}else{goto _LL172;}_LL172: goto _LL173;_LL171: yyzzz=_tmp170;goto _LL16F;
_LL173:(int)_throw((void*)& Cyc_yyfail_Pointer_Sort_tok);_LL16F:;}return yyzzz;}
static unsigned char _tmp172[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{_tmp172,_tmp172,_tmp172 + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(
void*yy1){struct Cyc_Absyn_Exp*yyzzz;{void*_tmp173=yy1;struct Cyc_Absyn_Exp*
_tmp174;_LL176: if(*((void**)_tmp173)== Cyc_Exp_tok){_LL17A: _tmp174=((struct Cyc_Exp_tok_struct*)
_tmp173)->f1;goto _LL177;}else{goto _LL178;}_LL178: goto _LL179;_LL177: yyzzz=_tmp174;
goto _LL175;_LL179:(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LL175:;}return yyzzz;}
static unsigned char _tmp176[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={Cyc_Core_Failure,{_tmp176,_tmp176,_tmp176 + 14}};struct
Cyc_List_List*Cyc_yyget_ExpList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp177=yy1;struct Cyc_List_List*_tmp178;_LL17C: if(*((void**)_tmp177)== Cyc_ExpList_tok){
_LL180: _tmp178=((struct Cyc_ExpList_tok_struct*)_tmp177)->f1;goto _LL17D;}else{
goto _LL17E;}_LL17E: goto _LL17F;_LL17D: yyzzz=_tmp178;goto _LL17B;_LL17F:(int)_throw((
void*)& Cyc_yyfail_ExpList_tok);_LL17B:;}return yyzzz;}static unsigned char _tmp17A[
39]="list_t<$(list_t<designator_t>,exp_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{_tmp17A,_tmp17A,_tmp17A + 39}};
struct Cyc_List_List*Cyc_yyget_InitializerList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp17B=yy1;struct Cyc_List_List*_tmp17C;_LL182: if(*((void**)_tmp17B)
== Cyc_InitializerList_tok){_LL186: _tmp17C=((struct Cyc_InitializerList_tok_struct*)
_tmp17B)->f1;goto _LL183;}else{goto _LL184;}_LL184: goto _LL185;_LL183: yyzzz=_tmp17C;
goto _LL181;_LL185:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);_LL181:;}
return yyzzz;}static unsigned char _tmp17E[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={Cyc_Core_Failure,{_tmp17E,_tmp17E,_tmp17E + 9}};void*Cyc_yyget_Primop_tok(
void*yy1){void*yyzzz;{void*_tmp17F=yy1;void*_tmp180;_LL188: if(*((void**)_tmp17F)
== Cyc_Primop_tok){_LL18C: _tmp180=(void*)((struct Cyc_Primop_tok_struct*)_tmp17F)->f1;
goto _LL189;}else{goto _LL18A;}_LL18A: goto _LL18B;_LL189: yyzzz=_tmp180;goto _LL187;
_LL18B:(int)_throw((void*)& Cyc_yyfail_Primop_tok);_LL187:;}return yyzzz;}static
unsigned char _tmp182[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp182,_tmp182,_tmp182 + 16}};struct
Cyc_Core_Opt*Cyc_yyget_Primopopt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp183=yy1;struct Cyc_Core_Opt*_tmp184;_LL18E: if(*((void**)_tmp183)== Cyc_Primopopt_tok){
_LL192: _tmp184=((struct Cyc_Primopopt_tok_struct*)_tmp183)->f1;goto _LL18F;}else{
goto _LL190;}_LL190: goto _LL191;_LL18F: yyzzz=_tmp184;goto _LL18D;_LL191:(int)_throw((
void*)& Cyc_yyfail_Primopopt_tok);_LL18D:;}return yyzzz;}static unsigned char
_tmp186[7]="qvar_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{_tmp186,_tmp186,_tmp186 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(
void*yy1){struct _tuple1*yyzzz;{void*_tmp187=yy1;struct _tuple1*_tmp188;_LL194: if(*((
void**)_tmp187)== Cyc_QualId_tok){_LL198: _tmp188=((struct Cyc_QualId_tok_struct*)
_tmp187)->f1;goto _LL195;}else{goto _LL196;}_LL196: goto _LL197;_LL195: yyzzz=_tmp188;
goto _LL193;_LL197:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL193:;}return
yyzzz;}static unsigned char _tmp18A[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp18A,_tmp18A,_tmp18A + 7}};struct Cyc_Absyn_Stmt*
Cyc_yyget_Stmt_tok(void*yy1){struct Cyc_Absyn_Stmt*yyzzz;{void*_tmp18B=yy1;struct
Cyc_Absyn_Stmt*_tmp18C;_LL19A: if(*((void**)_tmp18B)== Cyc_Stmt_tok){_LL19E:
_tmp18C=((struct Cyc_Stmt_tok_struct*)_tmp18B)->f1;goto _LL19B;}else{goto _LL19C;}
_LL19C: goto _LL19D;_LL19B: yyzzz=_tmp18C;goto _LL199;_LL19D:(int)_throw((void*)& Cyc_yyfail_Stmt_tok);
_LL199:;}return yyzzz;}static unsigned char _tmp18E[24]="list_t<switch_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={Cyc_Core_Failure,{
_tmp18E,_tmp18E,_tmp18E + 24}};struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp18F=yy1;struct Cyc_List_List*
_tmp190;_LL1A0: if(*((void**)_tmp18F)== Cyc_SwitchClauseList_tok){_LL1A4: _tmp190=((
struct Cyc_SwitchClauseList_tok_struct*)_tmp18F)->f1;goto _LL1A1;}else{goto _LL1A2;}
_LL1A2: goto _LL1A3;_LL1A1: yyzzz=_tmp190;goto _LL19F;_LL1A3:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL19F:;}return yyzzz;}static unsigned char _tmp192[25]="list_t<switchC_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={Cyc_Core_Failure,{
_tmp192,_tmp192,_tmp192 + 25}};struct Cyc_List_List*Cyc_yyget_SwitchCClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp193=yy1;struct Cyc_List_List*
_tmp194;_LL1A6: if(*((void**)_tmp193)== Cyc_SwitchCClauseList_tok){_LL1AA: _tmp194=((
struct Cyc_SwitchCClauseList_tok_struct*)_tmp193)->f1;goto _LL1A7;}else{goto _LL1A8;}
_LL1A8: goto _LL1A9;_LL1A7: yyzzz=_tmp194;goto _LL1A5;_LL1A9:(int)_throw((void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL1A5:;}return yyzzz;}static unsigned char _tmp196[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp196,_tmp196,_tmp196 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp197=yy1;
struct Cyc_Absyn_Pat*_tmp198;_LL1AC: if(*((void**)_tmp197)== Cyc_Pattern_tok){
_LL1B0: _tmp198=((struct Cyc_Pattern_tok_struct*)_tmp197)->f1;goto _LL1AD;}else{
goto _LL1AE;}_LL1AE: goto _LL1AF;_LL1AD: yyzzz=_tmp198;goto _LL1AB;_LL1AF:(int)_throw((
void*)& Cyc_yyfail_Pattern_tok);_LL1AB:;}return yyzzz;}static unsigned char _tmp19A[
14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure,{_tmp19A,_tmp19A,_tmp19A + 14}};struct Cyc_List_List*Cyc_yyget_PatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp19B=yy1;struct Cyc_List_List*
_tmp19C;_LL1B2: if(*((void**)_tmp19B)== Cyc_PatternList_tok){_LL1B6: _tmp19C=((
struct Cyc_PatternList_tok_struct*)_tmp19B)->f1;goto _LL1B3;}else{goto _LL1B4;}
_LL1B4: goto _LL1B5;_LL1B3: yyzzz=_tmp19C;goto _LL1B1;_LL1B5:(int)_throw((void*)& Cyc_yyfail_PatternList_tok);
_LL1B1:;}return yyzzz;}static unsigned char _tmp19E[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{
_tmp19E,_tmp19E,_tmp19E + 31}};struct _tuple13*Cyc_yyget_FieldPattern_tok(void*yy1){
struct _tuple13*yyzzz;{void*_tmp19F=yy1;struct _tuple13*_tmp1A0;_LL1B8: if(*((void**)
_tmp19F)== Cyc_FieldPattern_tok){_LL1BC: _tmp1A0=((struct Cyc_FieldPattern_tok_struct*)
_tmp19F)->f1;goto _LL1B9;}else{goto _LL1BA;}_LL1BA: goto _LL1BB;_LL1B9: yyzzz=_tmp1A0;
goto _LL1B7;_LL1BB:(int)_throw((void*)& Cyc_yyfail_FieldPattern_tok);_LL1B7:;}
return yyzzz;}static unsigned char _tmp1A2[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp1A2,_tmp1A2,_tmp1A2 + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1A3=yy1;struct Cyc_List_List*
_tmp1A4;_LL1BE: if(*((void**)_tmp1A3)== Cyc_FieldPatternList_tok){_LL1C2: _tmp1A4=((
struct Cyc_FieldPatternList_tok_struct*)_tmp1A3)->f1;goto _LL1BF;}else{goto _LL1C0;}
_LL1C0: goto _LL1C1;_LL1BF: yyzzz=_tmp1A4;goto _LL1BD;_LL1C1:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL1BD:;}return yyzzz;}static unsigned char _tmp1A6[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={Cyc_Core_Failure,{_tmp1A6,_tmp1A6,_tmp1A6 + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok(void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp1A7=yy1;
struct Cyc_Absyn_Fndecl*_tmp1A8;_LL1C4: if(*((void**)_tmp1A7)== Cyc_FnDecl_tok){
_LL1C8: _tmp1A8=((struct Cyc_FnDecl_tok_struct*)_tmp1A7)->f1;goto _LL1C5;}else{goto
_LL1C6;}_LL1C6: goto _LL1C7;_LL1C5: yyzzz=_tmp1A8;goto _LL1C3;_LL1C7:(int)_throw((
void*)& Cyc_yyfail_FnDecl_tok);_LL1C3:;}return yyzzz;}static unsigned char _tmp1AA[
15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={
Cyc_Core_Failure,{_tmp1AA,_tmp1AA,_tmp1AA + 15}};struct Cyc_List_List*Cyc_yyget_DeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1AB=yy1;struct Cyc_List_List*
_tmp1AC;_LL1CA: if(*((void**)_tmp1AB)== Cyc_DeclList_tok){_LL1CE: _tmp1AC=((struct
Cyc_DeclList_tok_struct*)_tmp1AB)->f1;goto _LL1CB;}else{goto _LL1CC;}_LL1CC: goto
_LL1CD;_LL1CB: yyzzz=_tmp1AC;goto _LL1C9;_LL1CD:(int)_throw((void*)& Cyc_yyfail_DeclList_tok);
_LL1C9:;}return yyzzz;}static unsigned char _tmp1AE[12]="decl_spec_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={Cyc_Core_Failure,{_tmp1AE,
_tmp1AE,_tmp1AE + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(
void*yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{void*_tmp1AF=yy1;struct Cyc_Parse_Declaration_spec*
_tmp1B0;_LL1D0: if(*((void**)_tmp1AF)== Cyc_DeclSpec_tok){_LL1D4: _tmp1B0=((struct
Cyc_DeclSpec_tok_struct*)_tmp1AF)->f1;goto _LL1D1;}else{goto _LL1D2;}_LL1D2: goto
_LL1D3;_LL1D1: yyzzz=_tmp1B0;goto _LL1CF;_LL1D3:(int)_throw((void*)& Cyc_yyfail_DeclSpec_tok);
_LL1CF:;}return yyzzz;}static unsigned char _tmp1B2[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{
_tmp1B2,_tmp1B2,_tmp1B2 + 27}};struct _tuple14*Cyc_yyget_InitDecl_tok(void*yy1){
struct _tuple14*yyzzz;{void*_tmp1B3=yy1;struct _tuple14*_tmp1B4;_LL1D6: if(*((void**)
_tmp1B3)== Cyc_InitDecl_tok){_LL1DA: _tmp1B4=((struct Cyc_InitDecl_tok_struct*)
_tmp1B3)->f1;goto _LL1D7;}else{goto _LL1D8;}_LL1D8: goto _LL1D9;_LL1D7: yyzzz=_tmp1B4;
goto _LL1D5;_LL1D9:(int)_throw((void*)& Cyc_yyfail_InitDecl_tok);_LL1D5:;}return
yyzzz;}static unsigned char _tmp1B6[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp1B6,_tmp1B6,_tmp1B6 + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B7=yy1;struct Cyc_List_List*
_tmp1B8;_LL1DC: if(*((void**)_tmp1B7)== Cyc_InitDeclList_tok){_LL1E0: _tmp1B8=((
struct Cyc_InitDeclList_tok_struct*)_tmp1B7)->f1;goto _LL1DD;}else{goto _LL1DE;}
_LL1DE: goto _LL1DF;_LL1DD: yyzzz=_tmp1B8;goto _LL1DB;_LL1DF:(int)_throw((void*)& Cyc_yyfail_InitDeclList_tok);
_LL1DB:;}return yyzzz;}static unsigned char _tmp1BA[16]="storage_class_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{
_tmp1BA,_tmp1BA,_tmp1BA + 16}};void*Cyc_yyget_StorageClass_tok(void*yy1){void*
yyzzz;{void*_tmp1BB=yy1;void*_tmp1BC;_LL1E2: if(*((void**)_tmp1BB)== Cyc_StorageClass_tok){
_LL1E6: _tmp1BC=(void*)((struct Cyc_StorageClass_tok_struct*)_tmp1BB)->f1;goto
_LL1E3;}else{goto _LL1E4;}_LL1E4: goto _LL1E5;_LL1E3: yyzzz=_tmp1BC;goto _LL1E1;
_LL1E5:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);_LL1E1:;}return yyzzz;}
static unsigned char _tmp1BE[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{_tmp1BE,_tmp1BE,_tmp1BE + 17}};
void*Cyc_yyget_TypeSpecifier_tok(void*yy1){void*yyzzz;{void*_tmp1BF=yy1;void*
_tmp1C0;_LL1E8: if(*((void**)_tmp1BF)== Cyc_TypeSpecifier_tok){_LL1EC: _tmp1C0=(
void*)((struct Cyc_TypeSpecifier_tok_struct*)_tmp1BF)->f1;goto _LL1E9;}else{goto
_LL1EA;}_LL1EA: goto _LL1EB;_LL1E9: yyzzz=_tmp1C0;goto _LL1E7;_LL1EB:(int)_throw((
void*)& Cyc_yyfail_TypeSpecifier_tok);_LL1E7:;}return yyzzz;}static unsigned char
_tmp1C2[18]="struct_or_union_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={
Cyc_Core_Failure,{_tmp1C2,_tmp1C2,_tmp1C2 + 18}};enum Cyc_Parse_Struct_or_union
Cyc_yyget_StructOrUnion_tok(void*yy1){enum Cyc_Parse_Struct_or_union yyzzz;{void*
_tmp1C3=yy1;enum Cyc_Parse_Struct_or_union _tmp1C4;_LL1EE: if(*((void**)_tmp1C3)== 
Cyc_StructOrUnion_tok){_LL1F2: _tmp1C4=((struct Cyc_StructOrUnion_tok_struct*)
_tmp1C3)->f1;goto _LL1EF;}else{goto _LL1F0;}_LL1F0: goto _LL1F1;_LL1EF: yyzzz=_tmp1C4;
goto _LL1ED;_LL1F1:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);_LL1ED:;}
return yyzzz;}static unsigned char _tmp1C6[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp1C6,_tmp1C6,_tmp1C6 + 8}};struct
Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*
_tmp1C7=yy1;struct Cyc_Absyn_Tqual _tmp1C8;_LL1F4: if(*((void**)_tmp1C7)== Cyc_TypeQual_tok){
_LL1F8: _tmp1C8=((struct Cyc_TypeQual_tok_struct*)_tmp1C7)->f1;goto _LL1F5;}else{
goto _LL1F6;}_LL1F6: goto _LL1F7;_LL1F5: yyzzz=_tmp1C8;goto _LL1F3;_LL1F7:(int)_throw((
void*)& Cyc_yyfail_TypeQual_tok);_LL1F3:;}return yyzzz;}static unsigned char _tmp1CA[
22]="list_t<structfield_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={
Cyc_Core_Failure,{_tmp1CA,_tmp1CA,_tmp1CA + 22}};struct Cyc_List_List*Cyc_yyget_StructFieldDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1CB=yy1;struct Cyc_List_List*
_tmp1CC;_LL1FA: if(*((void**)_tmp1CB)== Cyc_StructFieldDeclList_tok){_LL1FE:
_tmp1CC=((struct Cyc_StructFieldDeclList_tok_struct*)_tmp1CB)->f1;goto _LL1FB;}
else{goto _LL1FC;}_LL1FC: goto _LL1FD;_LL1FB: yyzzz=_tmp1CC;goto _LL1F9;_LL1FD:(int)
_throw((void*)& Cyc_yyfail_StructFieldDeclList_tok);_LL1F9:;}return yyzzz;}static
unsigned char _tmp1CE[30]="list_t<list_t<structfield_t>>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclListList_tok={Cyc_Core_Failure,{_tmp1CE,_tmp1CE,
_tmp1CE + 30}};struct Cyc_List_List*Cyc_yyget_StructFieldDeclListList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp1CF=yy1;struct Cyc_List_List*_tmp1D0;_LL200:
if(*((void**)_tmp1CF)== Cyc_StructFieldDeclListList_tok){_LL204: _tmp1D0=((struct
Cyc_StructFieldDeclListList_tok_struct*)_tmp1CF)->f1;goto _LL201;}else{goto _LL202;}
_LL202: goto _LL203;_LL201: yyzzz=_tmp1D0;goto _LL1FF;_LL203:(int)_throw((void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL1FF:;}return yyzzz;}static unsigned char _tmp1D2[24]="list_t<type_modifier_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{
_tmp1D2,_tmp1D2,_tmp1D2 + 24}};struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1D3=yy1;struct Cyc_List_List*
_tmp1D4;_LL206: if(*((void**)_tmp1D3)== Cyc_TypeModifierList_tok){_LL20A: _tmp1D4=((
struct Cyc_TypeModifierList_tok_struct*)_tmp1D3)->f1;goto _LL207;}else{goto _LL208;}
_LL208: goto _LL209;_LL207: yyzzz=_tmp1D4;goto _LL205;_LL209:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);
_LL205:;}return yyzzz;}static unsigned char _tmp1D6[13]="declarator_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp1D6,
_tmp1D6,_tmp1D6 + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_Declarator_tok(void*
yy1){struct Cyc_Parse_Declarator*yyzzz;{void*_tmp1D7=yy1;struct Cyc_Parse_Declarator*
_tmp1D8;_LL20C: if(*((void**)_tmp1D7)== Cyc_Declarator_tok){_LL210: _tmp1D8=((
struct Cyc_Declarator_tok_struct*)_tmp1D7)->f1;goto _LL20D;}else{goto _LL20E;}
_LL20E: goto _LL20F;_LL20D: yyzzz=_tmp1D8;goto _LL20B;_LL20F:(int)_throw((void*)& Cyc_yyfail_Declarator_tok);
_LL20B:;}return yyzzz;}static unsigned char _tmp1DA[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{
_tmp1DA,_tmp1DA,_tmp1DA + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(
void*yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp1DB=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp1DC;_LL212: if(*((void**)_tmp1DB)== Cyc_AbstractDeclarator_tok){_LL216:
_tmp1DC=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp1DB)->f1;goto _LL213;}
else{goto _LL214;}_LL214: goto _LL215;_LL213: yyzzz=_tmp1DC;goto _LL211;_LL215:(int)
_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);_LL211:;}return yyzzz;}static
unsigned char _tmp1DE[5]="bool";static struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={
Cyc_Core_Failure,{_tmp1DE,_tmp1DE,_tmp1DE + 5}};int Cyc_yyget_Bool_tok(void*yy1){
int yyzzz;{void*_tmp1DF=yy1;int _tmp1E0;_LL218: if(*((void**)_tmp1DF)== Cyc_Bool_tok){
_LL21C: _tmp1E0=((struct Cyc_Bool_tok_struct*)_tmp1DF)->f1;goto _LL219;}else{goto
_LL21A;}_LL21A: goto _LL21B;_LL219: yyzzz=_tmp1E0;goto _LL217;_LL21B:(int)_throw((
void*)& Cyc_yyfail_Bool_tok);_LL217:;}return yyzzz;}static unsigned char _tmp1E2[8]="scope_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Scope_tok={Cyc_Core_Failure,{
_tmp1E2,_tmp1E2,_tmp1E2 + 8}};void*Cyc_yyget_Scope_tok(void*yy1){void*yyzzz;{void*
_tmp1E3=yy1;void*_tmp1E4;_LL21E: if(*((void**)_tmp1E3)== Cyc_Scope_tok){_LL222:
_tmp1E4=(void*)((struct Cyc_Scope_tok_struct*)_tmp1E3)->f1;goto _LL21F;}else{goto
_LL220;}_LL220: goto _LL221;_LL21F: yyzzz=_tmp1E4;goto _LL21D;_LL221:(int)_throw((
void*)& Cyc_yyfail_Scope_tok);_LL21D:;}return yyzzz;}static unsigned char _tmp1E6[14]="tunionfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={Cyc_Core_Failure,{
_tmp1E6,_tmp1E6,_tmp1E6 + 14}};struct Cyc_Absyn_Tunionfield*Cyc_yyget_TunionField_tok(
void*yy1){struct Cyc_Absyn_Tunionfield*yyzzz;{void*_tmp1E7=yy1;struct Cyc_Absyn_Tunionfield*
_tmp1E8;_LL224: if(*((void**)_tmp1E7)== Cyc_TunionField_tok){_LL228: _tmp1E8=((
struct Cyc_TunionField_tok_struct*)_tmp1E7)->f1;goto _LL225;}else{goto _LL226;}
_LL226: goto _LL227;_LL225: yyzzz=_tmp1E8;goto _LL223;_LL227:(int)_throw((void*)& Cyc_yyfail_TunionField_tok);
_LL223:;}return yyzzz;}static unsigned char _tmp1EA[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{
_tmp1EA,_tmp1EA,_tmp1EA + 22}};struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1EB=yy1;struct Cyc_List_List*
_tmp1EC;_LL22A: if(*((void**)_tmp1EB)== Cyc_TunionFieldList_tok){_LL22E: _tmp1EC=((
struct Cyc_TunionFieldList_tok_struct*)_tmp1EB)->f1;goto _LL22B;}else{goto _LL22C;}
_LL22C: goto _LL22D;_LL22B: yyzzz=_tmp1EC;goto _LL229;_LL22D:(int)_throw((void*)& Cyc_yyfail_TunionFieldList_tok);
_LL229:;}return yyzzz;}static unsigned char _tmp1EE[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{
_tmp1EE,_tmp1EE,_tmp1EE + 50}};struct _tuple17*Cyc_yyget_QualSpecList_tok(void*yy1){
struct _tuple17*yyzzz;{void*_tmp1EF=yy1;struct _tuple17*_tmp1F0;_LL230: if(*((void**)
_tmp1EF)== Cyc_QualSpecList_tok){_LL234: _tmp1F0=((struct Cyc_QualSpecList_tok_struct*)
_tmp1EF)->f1;goto _LL231;}else{goto _LL232;}_LL232: goto _LL233;_LL231: yyzzz=_tmp1F0;
goto _LL22F;_LL233:(int)_throw((void*)& Cyc_yyfail_QualSpecList_tok);_LL22F:;}
return yyzzz;}static unsigned char _tmp1F2[14]="list_t<var_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={Cyc_Core_Failure,{_tmp1F2,_tmp1F2,_tmp1F2 + 14}};struct Cyc_List_List*
Cyc_yyget_IdList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1F3=yy1;
struct Cyc_List_List*_tmp1F4;_LL236: if(*((void**)_tmp1F3)== Cyc_IdList_tok){
_LL23A: _tmp1F4=((struct Cyc_IdList_tok_struct*)_tmp1F3)->f1;goto _LL237;}else{goto
_LL238;}_LL238: goto _LL239;_LL237: yyzzz=_tmp1F4;goto _LL235;_LL239:(int)_throw((
void*)& Cyc_yyfail_IdList_tok);_LL235:;}return yyzzz;}static unsigned char _tmp1F6[
32]="$(opt_t<var_t>,tqual_t,type_t)@";static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={
Cyc_Core_Failure,{_tmp1F6,_tmp1F6,_tmp1F6 + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(
void*yy1){struct _tuple2*yyzzz;{void*_tmp1F7=yy1;struct _tuple2*_tmp1F8;_LL23C: if(*((
void**)_tmp1F7)== Cyc_ParamDecl_tok){_LL240: _tmp1F8=((struct Cyc_ParamDecl_tok_struct*)
_tmp1F7)->f1;goto _LL23D;}else{goto _LL23E;}_LL23E: goto _LL23F;_LL23D: yyzzz=_tmp1F8;
goto _LL23B;_LL23F:(int)_throw((void*)& Cyc_yyfail_ParamDecl_tok);_LL23B:;}return
yyzzz;}static unsigned char _tmp1FA[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp1FA,_tmp1FA,_tmp1FA + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1FB=yy1;struct Cyc_List_List*
_tmp1FC;_LL242: if(*((void**)_tmp1FB)== Cyc_ParamDeclList_tok){_LL246: _tmp1FC=((
struct Cyc_ParamDeclList_tok_struct*)_tmp1FB)->f1;goto _LL243;}else{goto _LL244;}
_LL244: goto _LL245;_LL243: yyzzz=_tmp1FC;goto _LL241;_LL245:(int)_throw((void*)& Cyc_yyfail_ParamDeclList_tok);
_LL241:;}return yyzzz;}static unsigned char _tmp1FE[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1FE,
_tmp1FE,_tmp1FE + 107}};struct _tuple16*Cyc_yyget_YY1(void*yy1){struct _tuple16*
yyzzz;{void*_tmp1FF=yy1;struct _tuple16*_tmp200;_LL248: if(*((void**)_tmp1FF)== 
Cyc_YY1){_LL24C: _tmp200=((struct Cyc_YY1_struct*)_tmp1FF)->f1;goto _LL249;}else{
goto _LL24A;}_LL24A: goto _LL24B;_LL249: yyzzz=_tmp200;goto _LL247;_LL24B:(int)_throw((
void*)& Cyc_yyfail_YY1);_LL247:;}return yyzzz;}static unsigned char _tmp202[15]="list_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={Cyc_Core_Failure,{
_tmp202,_tmp202,_tmp202 + 15}};struct Cyc_List_List*Cyc_yyget_TypeList_tok(void*
yy1){struct Cyc_List_List*yyzzz;{void*_tmp203=yy1;struct Cyc_List_List*_tmp204;
_LL24E: if(*((void**)_tmp203)== Cyc_TypeList_tok){_LL252: _tmp204=((struct Cyc_TypeList_tok_struct*)
_tmp203)->f1;goto _LL24F;}else{goto _LL250;}_LL250: goto _LL251;_LL24F: yyzzz=_tmp204;
goto _LL24D;_LL251:(int)_throw((void*)& Cyc_yyfail_TypeList_tok);_LL24D:;}return
yyzzz;}static unsigned char _tmp206[21]="list_t<designator_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{_tmp206,_tmp206,_tmp206 + 21}};
struct Cyc_List_List*Cyc_yyget_DesignatorList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp207=yy1;struct Cyc_List_List*_tmp208;_LL254: if(*((void**)_tmp207)
== Cyc_DesignatorList_tok){_LL258: _tmp208=((struct Cyc_DesignatorList_tok_struct*)
_tmp207)->f1;goto _LL255;}else{goto _LL256;}_LL256: goto _LL257;_LL255: yyzzz=_tmp208;
goto _LL253;_LL257:(int)_throw((void*)& Cyc_yyfail_DesignatorList_tok);_LL253:;}
return yyzzz;}static unsigned char _tmp20A[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={Cyc_Core_Failure,{_tmp20A,_tmp20A,_tmp20A + 13}};void*
Cyc_yyget_Designator_tok(void*yy1){void*yyzzz;{void*_tmp20B=yy1;void*_tmp20C;
_LL25A: if(*((void**)_tmp20B)== Cyc_Designator_tok){_LL25E: _tmp20C=(void*)((
struct Cyc_Designator_tok_struct*)_tmp20B)->f1;goto _LL25B;}else{goto _LL25C;}
_LL25C: goto _LL25D;_LL25B: yyzzz=_tmp20C;goto _LL259;_LL25D:(int)_throw((void*)& Cyc_yyfail_Designator_tok);
_LL259:;}return yyzzz;}static unsigned char _tmp20E[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{_tmp20E,_tmp20E,_tmp20E + 7}};void*Cyc_yyget_Kind_tok(
void*yy1){void*yyzzz;{void*_tmp20F=yy1;void*_tmp210;_LL260: if(*((void**)_tmp20F)
== Cyc_Kind_tok){_LL264: _tmp210=(void*)((struct Cyc_Kind_tok_struct*)_tmp20F)->f1;
goto _LL261;}else{goto _LL262;}_LL262: goto _LL263;_LL261: yyzzz=_tmp210;goto _LL25F;
_LL263:(int)_throw((void*)& Cyc_yyfail_Kind_tok);_LL25F:;}return yyzzz;}static
unsigned char _tmp212[7]="type_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={
Cyc_Core_Failure,{_tmp212,_tmp212,_tmp212 + 7}};void*Cyc_yyget_Type_tok(void*yy1){
void*yyzzz;{void*_tmp213=yy1;void*_tmp214;_LL266: if(*((void**)_tmp213)== Cyc_Type_tok){
_LL26A: _tmp214=(void*)((struct Cyc_Type_tok_struct*)_tmp213)->f1;goto _LL267;}
else{goto _LL268;}_LL268: goto _LL269;_LL267: yyzzz=_tmp214;goto _LL265;_LL269:(int)
_throw((void*)& Cyc_yyfail_Type_tok);_LL265:;}return yyzzz;}static unsigned char
_tmp216[20]="list_t<attribute_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={
Cyc_Core_Failure,{_tmp216,_tmp216,_tmp216 + 20}};struct Cyc_List_List*Cyc_yyget_AttributeList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp217=yy1;struct Cyc_List_List*
_tmp218;_LL26C: if(*((void**)_tmp217)== Cyc_AttributeList_tok){_LL270: _tmp218=((
struct Cyc_AttributeList_tok_struct*)_tmp217)->f1;goto _LL26D;}else{goto _LL26E;}
_LL26E: goto _LL26F;_LL26D: yyzzz=_tmp218;goto _LL26B;_LL26F:(int)_throw((void*)& Cyc_yyfail_AttributeList_tok);
_LL26B:;}return yyzzz;}static unsigned char _tmp21A[12]="attribute_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{_tmp21A,
_tmp21A,_tmp21A + 12}};void*Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{void*
_tmp21B=yy1;void*_tmp21C;_LL272: if(*((void**)_tmp21B)== Cyc_Attribute_tok){
_LL276: _tmp21C=(void*)((struct Cyc_Attribute_tok_struct*)_tmp21B)->f1;goto _LL273;}
else{goto _LL274;}_LL274: goto _LL275;_LL273: yyzzz=_tmp21C;goto _LL271;_LL275:(int)
_throw((void*)& Cyc_yyfail_Attribute_tok);_LL271:;}return yyzzz;}static
unsigned char _tmp21E[12]="enumfield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={
Cyc_Core_Failure,{_tmp21E,_tmp21E,_tmp21E + 12}};struct Cyc_Absyn_Enumfield*Cyc_yyget_Enumfield_tok(
void*yy1){struct Cyc_Absyn_Enumfield*yyzzz;{void*_tmp21F=yy1;struct Cyc_Absyn_Enumfield*
_tmp220;_LL278: if(*((void**)_tmp21F)== Cyc_Enumfield_tok){_LL27C: _tmp220=((
struct Cyc_Enumfield_tok_struct*)_tmp21F)->f1;goto _LL279;}else{goto _LL27A;}_LL27A:
goto _LL27B;_LL279: yyzzz=_tmp220;goto _LL277;_LL27B:(int)_throw((void*)& Cyc_yyfail_Enumfield_tok);
_LL277:;}return yyzzz;}static unsigned char _tmp222[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{
_tmp222,_tmp222,_tmp222 + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp223=yy1;struct Cyc_List_List*
_tmp224;_LL27E: if(*((void**)_tmp223)== Cyc_EnumfieldList_tok){_LL282: _tmp224=((
struct Cyc_EnumfieldList_tok_struct*)_tmp223)->f1;goto _LL27F;}else{goto _LL280;}
_LL280: goto _LL281;_LL27F: yyzzz=_tmp224;goto _LL27D;_LL281:(int)_throw((void*)& Cyc_yyfail_EnumfieldList_tok);
_LL27D:;}return yyzzz;}static unsigned char _tmp226[14]="opt_t<type_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeOpt_tok={Cyc_Core_Failure,{_tmp226,
_tmp226,_tmp226 + 14}};struct Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(void*yy1){struct
Cyc_Core_Opt*yyzzz;{void*_tmp227=yy1;struct Cyc_Core_Opt*_tmp228;_LL284: if(*((
void**)_tmp227)== Cyc_TypeOpt_tok){_LL288: _tmp228=((struct Cyc_TypeOpt_tok_struct*)
_tmp227)->f1;goto _LL285;}else{goto _LL286;}_LL286: goto _LL287;_LL285: yyzzz=_tmp228;
goto _LL283;_LL287:(int)_throw((void*)& Cyc_yyfail_TypeOpt_tok);_LL283:;}return
yyzzz;}static unsigned char _tmp22A[26]="list_t<$(type_t,type_t)@>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{_tmp22A,
_tmp22A,_tmp22A + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp22B=yy1;struct Cyc_List_List*_tmp22C;_LL28A:
if(*((void**)_tmp22B)== Cyc_Rgnorder_tok){_LL28E: _tmp22C=((struct Cyc_Rgnorder_tok_struct*)
_tmp22B)->f1;goto _LL28B;}else{goto _LL28C;}_LL28C: goto _LL28D;_LL28B: yyzzz=_tmp22C;
goto _LL289;_LL28D:(int)_throw((void*)& Cyc_yyfail_Rgnorder_tok);_LL289:;}return
yyzzz;}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;
int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){return({struct Cyc_Yyltype
_tmp22D;_tmp22D.timestamp=0;_tmp22D.first_line=0;_tmp22D.first_column=0;_tmp22D.last_line=
0;_tmp22D.last_column=0;_tmp22D;});}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
static short Cyc_yytranslate[353]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,124,2,2,106,122,118,2,102,103,113,116,104,117,110,121,2,2,2,2,
2,2,2,2,2,2,109,98,107,101,108,115,114,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,111,2,112,120,105,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,99,119,100,123,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,
46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,
73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97};
static unsigned char _tmp22E[2]="$";static unsigned char _tmp22F[6]="error";static
unsigned char _tmp230[12]="$undefined.";static unsigned char _tmp231[5]="AUTO";
static unsigned char _tmp232[9]="REGISTER";static unsigned char _tmp233[7]="STATIC";
static unsigned char _tmp234[7]="EXTERN";static unsigned char _tmp235[8]="TYPEDEF";
static unsigned char _tmp236[5]="VOID";static unsigned char _tmp237[5]="CHAR";static
unsigned char _tmp238[6]="SHORT";static unsigned char _tmp239[4]="INT";static
unsigned char _tmp23A[5]="LONG";static unsigned char _tmp23B[6]="FLOAT";static
unsigned char _tmp23C[7]="DOUBLE";static unsigned char _tmp23D[7]="SIGNED";static
unsigned char _tmp23E[9]="UNSIGNED";static unsigned char _tmp23F[6]="CONST";static
unsigned char _tmp240[9]="VOLATILE";static unsigned char _tmp241[9]="RESTRICT";
static unsigned char _tmp242[7]="STRUCT";static unsigned char _tmp243[6]="UNION";
static unsigned char _tmp244[5]="CASE";static unsigned char _tmp245[8]="DEFAULT";
static unsigned char _tmp246[7]="INLINE";static unsigned char _tmp247[7]="SIZEOF";
static unsigned char _tmp248[9]="OFFSETOF";static unsigned char _tmp249[3]="IF";
static unsigned char _tmp24A[5]="ELSE";static unsigned char _tmp24B[7]="SWITCH";
static unsigned char _tmp24C[6]="WHILE";static unsigned char _tmp24D[3]="DO";static
unsigned char _tmp24E[4]="FOR";static unsigned char _tmp24F[5]="GOTO";static
unsigned char _tmp250[9]="CONTINUE";static unsigned char _tmp251[6]="BREAK";static
unsigned char _tmp252[7]="RETURN";static unsigned char _tmp253[5]="ENUM";static
unsigned char _tmp254[8]="NULL_kw";static unsigned char _tmp255[4]="LET";static
unsigned char _tmp256[6]="THROW";static unsigned char _tmp257[4]="TRY";static
unsigned char _tmp258[6]="CATCH";static unsigned char _tmp259[4]="NEW";static
unsigned char _tmp25A[9]="ABSTRACT";static unsigned char _tmp25B[9]="FALLTHRU";
static unsigned char _tmp25C[6]="USING";static unsigned char _tmp25D[10]="NAMESPACE";
static unsigned char _tmp25E[7]="TUNION";static unsigned char _tmp25F[8]="XTUNION";
static unsigned char _tmp260[9]="FORARRAY";static unsigned char _tmp261[5]="FILL";
static unsigned char _tmp262[8]="CODEGEN";static unsigned char _tmp263[4]="CUT";
static unsigned char _tmp264[7]="SPLICE";static unsigned char _tmp265[7]="MALLOC";
static unsigned char _tmp266[8]="RMALLOC";static unsigned char _tmp267[7]="CALLOC";
static unsigned char _tmp268[8]="RCALLOC";static unsigned char _tmp269[9]="REGION_T";
static unsigned char _tmp26A[9]="SIZEOF_T";static unsigned char _tmp26B[7]="REGION";
static unsigned char _tmp26C[5]="RNEW";static unsigned char _tmp26D[8]="REGIONS";
static unsigned char _tmp26E[4]="GEN";static unsigned char _tmp26F[7]="PTR_OP";
static unsigned char _tmp270[7]="INC_OP";static unsigned char _tmp271[7]="DEC_OP";
static unsigned char _tmp272[8]="LEFT_OP";static unsigned char _tmp273[9]="RIGHT_OP";
static unsigned char _tmp274[6]="LE_OP";static unsigned char _tmp275[6]="GE_OP";
static unsigned char _tmp276[6]="EQ_OP";static unsigned char _tmp277[6]="NE_OP";
static unsigned char _tmp278[7]="AND_OP";static unsigned char _tmp279[6]="OR_OP";
static unsigned char _tmp27A[11]="MUL_ASSIGN";static unsigned char _tmp27B[11]="DIV_ASSIGN";
static unsigned char _tmp27C[11]="MOD_ASSIGN";static unsigned char _tmp27D[11]="ADD_ASSIGN";
static unsigned char _tmp27E[11]="SUB_ASSIGN";static unsigned char _tmp27F[12]="LEFT_ASSIGN";
static unsigned char _tmp280[13]="RIGHT_ASSIGN";static unsigned char _tmp281[11]="AND_ASSIGN";
static unsigned char _tmp282[11]="XOR_ASSIGN";static unsigned char _tmp283[10]="OR_ASSIGN";
static unsigned char _tmp284[9]="ELLIPSIS";static unsigned char _tmp285[11]="LEFT_RIGHT";
static unsigned char _tmp286[12]="COLON_COLON";static unsigned char _tmp287[11]="IDENTIFIER";
static unsigned char _tmp288[17]="INTEGER_CONSTANT";static unsigned char _tmp289[7]="STRING";
static unsigned char _tmp28A[19]="CHARACTER_CONSTANT";static unsigned char _tmp28B[
18]="FLOATING_CONSTANT";static unsigned char _tmp28C[9]="TYPE_VAR";static
unsigned char _tmp28D[16]="QUAL_IDENTIFIER";static unsigned char _tmp28E[18]="QUAL_TYPEDEF_NAME";
static unsigned char _tmp28F[10]="ATTRIBUTE";static unsigned char _tmp290[4]="';'";
static unsigned char _tmp291[4]="'{'";static unsigned char _tmp292[4]="'}'";static
unsigned char _tmp293[4]="'='";static unsigned char _tmp294[4]="'('";static
unsigned char _tmp295[4]="')'";static unsigned char _tmp296[4]="','";static
unsigned char _tmp297[4]="'_'";static unsigned char _tmp298[4]="'$'";static
unsigned char _tmp299[4]="'<'";static unsigned char _tmp29A[4]="'>'";static
unsigned char _tmp29B[4]="':'";static unsigned char _tmp29C[4]="'.'";static
unsigned char _tmp29D[4]="'['";static unsigned char _tmp29E[4]="']'";static
unsigned char _tmp29F[4]="'*'";static unsigned char _tmp2A0[4]="'@'";static
unsigned char _tmp2A1[4]="'?'";static unsigned char _tmp2A2[4]="'+'";static
unsigned char _tmp2A3[4]="'-'";static unsigned char _tmp2A4[4]="'&'";static
unsigned char _tmp2A5[4]="'|'";static unsigned char _tmp2A6[4]="'^'";static
unsigned char _tmp2A7[4]="'/'";static unsigned char _tmp2A8[4]="'%'";static
unsigned char _tmp2A9[4]="'~'";static unsigned char _tmp2AA[4]="'!'";static
unsigned char _tmp2AB[5]="prog";static unsigned char _tmp2AC[17]="translation_unit";
static unsigned char _tmp2AD[21]="external_declaration";static unsigned char _tmp2AE[
20]="function_definition";static unsigned char _tmp2AF[21]="function_definition2";
static unsigned char _tmp2B0[13]="using_action";static unsigned char _tmp2B1[15]="unusing_action";
static unsigned char _tmp2B2[17]="namespace_action";static unsigned char _tmp2B3[19]="unnamespace_action";
static unsigned char _tmp2B4[12]="declaration";static unsigned char _tmp2B5[17]="declaration_list";
static unsigned char _tmp2B6[23]="declaration_specifiers";static unsigned char
_tmp2B7[24]="storage_class_specifier";static unsigned char _tmp2B8[15]="attributes_opt";
static unsigned char _tmp2B9[11]="attributes";static unsigned char _tmp2BA[15]="attribute_list";
static unsigned char _tmp2BB[10]="attribute";static unsigned char _tmp2BC[15]="type_specifier";
static unsigned char _tmp2BD[5]="kind";static unsigned char _tmp2BE[15]="type_qualifier";
static unsigned char _tmp2BF[15]="enum_specifier";static unsigned char _tmp2C0[11]="enum_field";
static unsigned char _tmp2C1[22]="enum_declaration_list";static unsigned char
_tmp2C2[26]="struct_or_union_specifier";static unsigned char _tmp2C3[16]="type_params_opt";
static unsigned char _tmp2C4[16]="struct_or_union";static unsigned char _tmp2C5[24]="struct_declaration_list";
static unsigned char _tmp2C6[25]="struct_declaration_list0";static unsigned char
_tmp2C7[21]="init_declarator_list";static unsigned char _tmp2C8[22]="init_declarator_list0";
static unsigned char _tmp2C9[16]="init_declarator";static unsigned char _tmp2CA[19]="struct_declaration";
static unsigned char _tmp2CB[25]="specifier_qualifier_list";static unsigned char
_tmp2CC[23]="struct_declarator_list";static unsigned char _tmp2CD[24]="struct_declarator_list0";
static unsigned char _tmp2CE[18]="struct_declarator";static unsigned char _tmp2CF[17]="tunion_specifier";
static unsigned char _tmp2D0[18]="tunion_or_xtunion";static unsigned char _tmp2D1[17]="tunionfield_list";
static unsigned char _tmp2D2[18]="tunionfield_scope";static unsigned char _tmp2D3[12]="tunionfield";
static unsigned char _tmp2D4[11]="declarator";static unsigned char _tmp2D5[18]="direct_declarator";
static unsigned char _tmp2D6[8]="pointer";static unsigned char _tmp2D7[13]="pointer_char";
static unsigned char _tmp2D8[8]="rgn_opt";static unsigned char _tmp2D9[4]="rgn";
static unsigned char _tmp2DA[20]="type_qualifier_list";static unsigned char _tmp2DB[
20]="parameter_type_list";static unsigned char _tmp2DC[9]="type_var";static
unsigned char _tmp2DD[16]="optional_effect";static unsigned char _tmp2DE[19]="optional_rgn_order";
static unsigned char _tmp2DF[10]="rgn_order";static unsigned char _tmp2E0[16]="optional_inject";
static unsigned char _tmp2E1[11]="effect_set";static unsigned char _tmp2E2[14]="atomic_effect";
static unsigned char _tmp2E3[11]="region_set";static unsigned char _tmp2E4[15]="parameter_list";
static unsigned char _tmp2E5[22]="parameter_declaration";static unsigned char
_tmp2E6[16]="identifier_list";static unsigned char _tmp2E7[17]="identifier_list0";
static unsigned char _tmp2E8[12]="initializer";static unsigned char _tmp2E9[18]="array_initializer";
static unsigned char _tmp2EA[17]="initializer_list";static unsigned char _tmp2EB[12]="designation";
static unsigned char _tmp2EC[16]="designator_list";static unsigned char _tmp2ED[11]="designator";
static unsigned char _tmp2EE[10]="type_name";static unsigned char _tmp2EF[14]="any_type_name";
static unsigned char _tmp2F0[15]="type_name_list";static unsigned char _tmp2F1[20]="abstract_declarator";
static unsigned char _tmp2F2[27]="direct_abstract_declarator";static unsigned char
_tmp2F3[10]="statement";static unsigned char _tmp2F4[18]="labeled_statement";
static unsigned char _tmp2F5[21]="expression_statement";static unsigned char _tmp2F6[
19]="compound_statement";static unsigned char _tmp2F7[16]="block_item_list";static
unsigned char _tmp2F8[20]="selection_statement";static unsigned char _tmp2F9[15]="switch_clauses";
static unsigned char _tmp2FA[16]="switchC_clauses";static unsigned char _tmp2FB[20]="iteration_statement";
static unsigned char _tmp2FC[15]="jump_statement";static unsigned char _tmp2FD[8]="pattern";
static unsigned char _tmp2FE[19]="tuple_pattern_list";static unsigned char _tmp2FF[
20]="tuple_pattern_list0";static unsigned char _tmp300[14]="field_pattern";static
unsigned char _tmp301[19]="field_pattern_list";static unsigned char _tmp302[20]="field_pattern_list0";
static unsigned char _tmp303[11]="expression";static unsigned char _tmp304[22]="assignment_expression";
static unsigned char _tmp305[20]="assignment_operator";static unsigned char _tmp306[
23]="conditional_expression";static unsigned char _tmp307[20]="constant_expression";
static unsigned char _tmp308[22]="logical_or_expression";static unsigned char
_tmp309[23]="logical_and_expression";static unsigned char _tmp30A[24]="inclusive_or_expression";
static unsigned char _tmp30B[24]="exclusive_or_expression";static unsigned char
_tmp30C[15]="and_expression";static unsigned char _tmp30D[20]="equality_expression";
static unsigned char _tmp30E[22]="relational_expression";static unsigned char
_tmp30F[17]="shift_expression";static unsigned char _tmp310[20]="additive_expression";
static unsigned char _tmp311[26]="multiplicative_expression";static unsigned char
_tmp312[16]="cast_expression";static unsigned char _tmp313[17]="unary_expression";
static unsigned char _tmp314[15]="unary_operator";static unsigned char _tmp315[19]="postfix_expression";
static unsigned char _tmp316[19]="primary_expression";static unsigned char _tmp317[
25]="argument_expression_list";static unsigned char _tmp318[26]="argument_expression_list0";
static unsigned char _tmp319[9]="constant";static unsigned char _tmp31A[20]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[237]={{_tmp22E,_tmp22E,_tmp22E + 2},{_tmp22F,
_tmp22F,_tmp22F + 6},{_tmp230,_tmp230,_tmp230 + 12},{_tmp231,_tmp231,_tmp231 + 5},{
_tmp232,_tmp232,_tmp232 + 9},{_tmp233,_tmp233,_tmp233 + 7},{_tmp234,_tmp234,
_tmp234 + 7},{_tmp235,_tmp235,_tmp235 + 8},{_tmp236,_tmp236,_tmp236 + 5},{_tmp237,
_tmp237,_tmp237 + 5},{_tmp238,_tmp238,_tmp238 + 6},{_tmp239,_tmp239,_tmp239 + 4},{
_tmp23A,_tmp23A,_tmp23A + 5},{_tmp23B,_tmp23B,_tmp23B + 6},{_tmp23C,_tmp23C,
_tmp23C + 7},{_tmp23D,_tmp23D,_tmp23D + 7},{_tmp23E,_tmp23E,_tmp23E + 9},{_tmp23F,
_tmp23F,_tmp23F + 6},{_tmp240,_tmp240,_tmp240 + 9},{_tmp241,_tmp241,_tmp241 + 9},{
_tmp242,_tmp242,_tmp242 + 7},{_tmp243,_tmp243,_tmp243 + 6},{_tmp244,_tmp244,
_tmp244 + 5},{_tmp245,_tmp245,_tmp245 + 8},{_tmp246,_tmp246,_tmp246 + 7},{_tmp247,
_tmp247,_tmp247 + 7},{_tmp248,_tmp248,_tmp248 + 9},{_tmp249,_tmp249,_tmp249 + 3},{
_tmp24A,_tmp24A,_tmp24A + 5},{_tmp24B,_tmp24B,_tmp24B + 7},{_tmp24C,_tmp24C,
_tmp24C + 6},{_tmp24D,_tmp24D,_tmp24D + 3},{_tmp24E,_tmp24E,_tmp24E + 4},{_tmp24F,
_tmp24F,_tmp24F + 5},{_tmp250,_tmp250,_tmp250 + 9},{_tmp251,_tmp251,_tmp251 + 6},{
_tmp252,_tmp252,_tmp252 + 7},{_tmp253,_tmp253,_tmp253 + 5},{_tmp254,_tmp254,
_tmp254 + 8},{_tmp255,_tmp255,_tmp255 + 4},{_tmp256,_tmp256,_tmp256 + 6},{_tmp257,
_tmp257,_tmp257 + 4},{_tmp258,_tmp258,_tmp258 + 6},{_tmp259,_tmp259,_tmp259 + 4},{
_tmp25A,_tmp25A,_tmp25A + 9},{_tmp25B,_tmp25B,_tmp25B + 9},{_tmp25C,_tmp25C,
_tmp25C + 6},{_tmp25D,_tmp25D,_tmp25D + 10},{_tmp25E,_tmp25E,_tmp25E + 7},{_tmp25F,
_tmp25F,_tmp25F + 8},{_tmp260,_tmp260,_tmp260 + 9},{_tmp261,_tmp261,_tmp261 + 5},{
_tmp262,_tmp262,_tmp262 + 8},{_tmp263,_tmp263,_tmp263 + 4},{_tmp264,_tmp264,
_tmp264 + 7},{_tmp265,_tmp265,_tmp265 + 7},{_tmp266,_tmp266,_tmp266 + 8},{_tmp267,
_tmp267,_tmp267 + 7},{_tmp268,_tmp268,_tmp268 + 8},{_tmp269,_tmp269,_tmp269 + 9},{
_tmp26A,_tmp26A,_tmp26A + 9},{_tmp26B,_tmp26B,_tmp26B + 7},{_tmp26C,_tmp26C,
_tmp26C + 5},{_tmp26D,_tmp26D,_tmp26D + 8},{_tmp26E,_tmp26E,_tmp26E + 4},{_tmp26F,
_tmp26F,_tmp26F + 7},{_tmp270,_tmp270,_tmp270 + 7},{_tmp271,_tmp271,_tmp271 + 7},{
_tmp272,_tmp272,_tmp272 + 8},{_tmp273,_tmp273,_tmp273 + 9},{_tmp274,_tmp274,
_tmp274 + 6},{_tmp275,_tmp275,_tmp275 + 6},{_tmp276,_tmp276,_tmp276 + 6},{_tmp277,
_tmp277,_tmp277 + 6},{_tmp278,_tmp278,_tmp278 + 7},{_tmp279,_tmp279,_tmp279 + 6},{
_tmp27A,_tmp27A,_tmp27A + 11},{_tmp27B,_tmp27B,_tmp27B + 11},{_tmp27C,_tmp27C,
_tmp27C + 11},{_tmp27D,_tmp27D,_tmp27D + 11},{_tmp27E,_tmp27E,_tmp27E + 11},{
_tmp27F,_tmp27F,_tmp27F + 12},{_tmp280,_tmp280,_tmp280 + 13},{_tmp281,_tmp281,
_tmp281 + 11},{_tmp282,_tmp282,_tmp282 + 11},{_tmp283,_tmp283,_tmp283 + 10},{
_tmp284,_tmp284,_tmp284 + 9},{_tmp285,_tmp285,_tmp285 + 11},{_tmp286,_tmp286,
_tmp286 + 12},{_tmp287,_tmp287,_tmp287 + 11},{_tmp288,_tmp288,_tmp288 + 17},{
_tmp289,_tmp289,_tmp289 + 7},{_tmp28A,_tmp28A,_tmp28A + 19},{_tmp28B,_tmp28B,
_tmp28B + 18},{_tmp28C,_tmp28C,_tmp28C + 9},{_tmp28D,_tmp28D,_tmp28D + 16},{_tmp28E,
_tmp28E,_tmp28E + 18},{_tmp28F,_tmp28F,_tmp28F + 10},{_tmp290,_tmp290,_tmp290 + 4},{
_tmp291,_tmp291,_tmp291 + 4},{_tmp292,_tmp292,_tmp292 + 4},{_tmp293,_tmp293,
_tmp293 + 4},{_tmp294,_tmp294,_tmp294 + 4},{_tmp295,_tmp295,_tmp295 + 4},{_tmp296,
_tmp296,_tmp296 + 4},{_tmp297,_tmp297,_tmp297 + 4},{_tmp298,_tmp298,_tmp298 + 4},{
_tmp299,_tmp299,_tmp299 + 4},{_tmp29A,_tmp29A,_tmp29A + 4},{_tmp29B,_tmp29B,
_tmp29B + 4},{_tmp29C,_tmp29C,_tmp29C + 4},{_tmp29D,_tmp29D,_tmp29D + 4},{_tmp29E,
_tmp29E,_tmp29E + 4},{_tmp29F,_tmp29F,_tmp29F + 4},{_tmp2A0,_tmp2A0,_tmp2A0 + 4},{
_tmp2A1,_tmp2A1,_tmp2A1 + 4},{_tmp2A2,_tmp2A2,_tmp2A2 + 4},{_tmp2A3,_tmp2A3,
_tmp2A3 + 4},{_tmp2A4,_tmp2A4,_tmp2A4 + 4},{_tmp2A5,_tmp2A5,_tmp2A5 + 4},{_tmp2A6,
_tmp2A6,_tmp2A6 + 4},{_tmp2A7,_tmp2A7,_tmp2A7 + 4},{_tmp2A8,_tmp2A8,_tmp2A8 + 4},{
_tmp2A9,_tmp2A9,_tmp2A9 + 4},{_tmp2AA,_tmp2AA,_tmp2AA + 4},{_tmp2AB,_tmp2AB,
_tmp2AB + 5},{_tmp2AC,_tmp2AC,_tmp2AC + 17},{_tmp2AD,_tmp2AD,_tmp2AD + 21},{_tmp2AE,
_tmp2AE,_tmp2AE + 20},{_tmp2AF,_tmp2AF,_tmp2AF + 21},{_tmp2B0,_tmp2B0,_tmp2B0 + 13},{
_tmp2B1,_tmp2B1,_tmp2B1 + 15},{_tmp2B2,_tmp2B2,_tmp2B2 + 17},{_tmp2B3,_tmp2B3,
_tmp2B3 + 19},{_tmp2B4,_tmp2B4,_tmp2B4 + 12},{_tmp2B5,_tmp2B5,_tmp2B5 + 17},{
_tmp2B6,_tmp2B6,_tmp2B6 + 23},{_tmp2B7,_tmp2B7,_tmp2B7 + 24},{_tmp2B8,_tmp2B8,
_tmp2B8 + 15},{_tmp2B9,_tmp2B9,_tmp2B9 + 11},{_tmp2BA,_tmp2BA,_tmp2BA + 15},{
_tmp2BB,_tmp2BB,_tmp2BB + 10},{_tmp2BC,_tmp2BC,_tmp2BC + 15},{_tmp2BD,_tmp2BD,
_tmp2BD + 5},{_tmp2BE,_tmp2BE,_tmp2BE + 15},{_tmp2BF,_tmp2BF,_tmp2BF + 15},{_tmp2C0,
_tmp2C0,_tmp2C0 + 11},{_tmp2C1,_tmp2C1,_tmp2C1 + 22},{_tmp2C2,_tmp2C2,_tmp2C2 + 26},{
_tmp2C3,_tmp2C3,_tmp2C3 + 16},{_tmp2C4,_tmp2C4,_tmp2C4 + 16},{_tmp2C5,_tmp2C5,
_tmp2C5 + 24},{_tmp2C6,_tmp2C6,_tmp2C6 + 25},{_tmp2C7,_tmp2C7,_tmp2C7 + 21},{
_tmp2C8,_tmp2C8,_tmp2C8 + 22},{_tmp2C9,_tmp2C9,_tmp2C9 + 16},{_tmp2CA,_tmp2CA,
_tmp2CA + 19},{_tmp2CB,_tmp2CB,_tmp2CB + 25},{_tmp2CC,_tmp2CC,_tmp2CC + 23},{
_tmp2CD,_tmp2CD,_tmp2CD + 24},{_tmp2CE,_tmp2CE,_tmp2CE + 18},{_tmp2CF,_tmp2CF,
_tmp2CF + 17},{_tmp2D0,_tmp2D0,_tmp2D0 + 18},{_tmp2D1,_tmp2D1,_tmp2D1 + 17},{
_tmp2D2,_tmp2D2,_tmp2D2 + 18},{_tmp2D3,_tmp2D3,_tmp2D3 + 12},{_tmp2D4,_tmp2D4,
_tmp2D4 + 11},{_tmp2D5,_tmp2D5,_tmp2D5 + 18},{_tmp2D6,_tmp2D6,_tmp2D6 + 8},{_tmp2D7,
_tmp2D7,_tmp2D7 + 13},{_tmp2D8,_tmp2D8,_tmp2D8 + 8},{_tmp2D9,_tmp2D9,_tmp2D9 + 4},{
_tmp2DA,_tmp2DA,_tmp2DA + 20},{_tmp2DB,_tmp2DB,_tmp2DB + 20},{_tmp2DC,_tmp2DC,
_tmp2DC + 9},{_tmp2DD,_tmp2DD,_tmp2DD + 16},{_tmp2DE,_tmp2DE,_tmp2DE + 19},{_tmp2DF,
_tmp2DF,_tmp2DF + 10},{_tmp2E0,_tmp2E0,_tmp2E0 + 16},{_tmp2E1,_tmp2E1,_tmp2E1 + 11},{
_tmp2E2,_tmp2E2,_tmp2E2 + 14},{_tmp2E3,_tmp2E3,_tmp2E3 + 11},{_tmp2E4,_tmp2E4,
_tmp2E4 + 15},{_tmp2E5,_tmp2E5,_tmp2E5 + 22},{_tmp2E6,_tmp2E6,_tmp2E6 + 16},{
_tmp2E7,_tmp2E7,_tmp2E7 + 17},{_tmp2E8,_tmp2E8,_tmp2E8 + 12},{_tmp2E9,_tmp2E9,
_tmp2E9 + 18},{_tmp2EA,_tmp2EA,_tmp2EA + 17},{_tmp2EB,_tmp2EB,_tmp2EB + 12},{
_tmp2EC,_tmp2EC,_tmp2EC + 16},{_tmp2ED,_tmp2ED,_tmp2ED + 11},{_tmp2EE,_tmp2EE,
_tmp2EE + 10},{_tmp2EF,_tmp2EF,_tmp2EF + 14},{_tmp2F0,_tmp2F0,_tmp2F0 + 15},{
_tmp2F1,_tmp2F1,_tmp2F1 + 20},{_tmp2F2,_tmp2F2,_tmp2F2 + 27},{_tmp2F3,_tmp2F3,
_tmp2F3 + 10},{_tmp2F4,_tmp2F4,_tmp2F4 + 18},{_tmp2F5,_tmp2F5,_tmp2F5 + 21},{
_tmp2F6,_tmp2F6,_tmp2F6 + 19},{_tmp2F7,_tmp2F7,_tmp2F7 + 16},{_tmp2F8,_tmp2F8,
_tmp2F8 + 20},{_tmp2F9,_tmp2F9,_tmp2F9 + 15},{_tmp2FA,_tmp2FA,_tmp2FA + 16},{
_tmp2FB,_tmp2FB,_tmp2FB + 20},{_tmp2FC,_tmp2FC,_tmp2FC + 15},{_tmp2FD,_tmp2FD,
_tmp2FD + 8},{_tmp2FE,_tmp2FE,_tmp2FE + 19},{_tmp2FF,_tmp2FF,_tmp2FF + 20},{_tmp300,
_tmp300,_tmp300 + 14},{_tmp301,_tmp301,_tmp301 + 19},{_tmp302,_tmp302,_tmp302 + 20},{
_tmp303,_tmp303,_tmp303 + 11},{_tmp304,_tmp304,_tmp304 + 22},{_tmp305,_tmp305,
_tmp305 + 20},{_tmp306,_tmp306,_tmp306 + 23},{_tmp307,_tmp307,_tmp307 + 20},{
_tmp308,_tmp308,_tmp308 + 22},{_tmp309,_tmp309,_tmp309 + 23},{_tmp30A,_tmp30A,
_tmp30A + 24},{_tmp30B,_tmp30B,_tmp30B + 24},{_tmp30C,_tmp30C,_tmp30C + 15},{
_tmp30D,_tmp30D,_tmp30D + 20},{_tmp30E,_tmp30E,_tmp30E + 22},{_tmp30F,_tmp30F,
_tmp30F + 17},{_tmp310,_tmp310,_tmp310 + 20},{_tmp311,_tmp311,_tmp311 + 26},{
_tmp312,_tmp312,_tmp312 + 16},{_tmp313,_tmp313,_tmp313 + 17},{_tmp314,_tmp314,
_tmp314 + 15},{_tmp315,_tmp315,_tmp315 + 19},{_tmp316,_tmp316,_tmp316 + 19},{
_tmp317,_tmp317,_tmp317 + 25},{_tmp318,_tmp318,_tmp318 + 26},{_tmp319,_tmp319,
_tmp319 + 9},{_tmp31A,_tmp31A,_tmp31A + 20}};static short Cyc_yyr1[410]={0,125,126,
126,126,126,126,126,126,127,127,128,128,128,128,129,129,130,131,132,133,134,134,
134,134,135,135,136,136,136,136,136,136,136,136,137,137,137,137,137,137,137,138,
138,139,140,140,141,141,141,141,141,142,142,142,142,142,142,142,142,142,142,142,
142,142,142,142,142,142,142,143,143,144,144,144,145,145,145,146,146,147,147,148,
148,148,148,148,149,149,149,150,150,151,152,152,153,154,154,155,155,156,157,157,
157,157,158,159,159,160,160,160,161,161,161,161,162,162,163,163,163,163,164,164,
164,165,165,166,166,167,167,167,167,167,167,167,167,167,167,167,167,168,168,168,
168,169,169,169,169,169,170,170,171,171,172,172,173,173,173,173,174,174,175,175,
176,176,177,177,178,178,179,179,180,180,180,180,181,181,182,182,183,183,183,184,
185,185,186,186,187,187,187,187,188,188,188,188,189,190,190,191,191,192,192,193,
193,193,193,193,194,194,195,195,195,196,196,196,196,196,196,196,196,196,196,196,
196,197,197,197,197,197,197,197,197,197,197,198,199,199,200,200,201,201,201,201,
201,201,202,202,202,202,202,203,203,203,203,203,203,204,204,204,204,205,205,205,
205,205,205,205,205,205,205,205,205,205,205,205,206,206,206,206,206,206,206,206,
207,207,207,207,207,207,207,207,207,207,207,207,207,207,208,208,209,209,210,210,
211,212,212,213,213,214,214,215,215,215,215,215,215,215,215,215,215,215,216,216,
216,216,216,216,216,217,218,218,219,219,220,220,221,221,222,222,223,223,223,224,
224,224,224,224,225,225,225,226,226,226,227,227,227,227,228,228,229,229,229,229,
229,229,229,229,229,229,229,229,229,229,229,229,230,230,230,231,231,231,231,231,
231,231,231,231,231,231,231,231,231,232,232,232,232,232,232,232,232,232,233,234,
234,235,235,235,235,236,236};static short Cyc_yyr2[410]={0,1,2,3,5,3,5,6,0,1,1,2,3,
3,4,3,4,2,1,2,1,2,3,5,3,1,2,2,3,2,3,2,3,2,3,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,8,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,2,4,4,4,1,1,1,1,1,5,2,4,1,3,1,3,4,6,6,3,3,0,3,3,1,1,1,
1,2,1,1,3,1,3,3,2,3,2,3,1,1,3,1,2,3,6,4,3,5,1,1,1,2,3,3,0,1,1,2,6,1,2,1,3,3,4,4,
5,4,4,4,2,2,1,3,4,4,5,1,1,4,4,1,0,1,1,1,1,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,
4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,
2,3,3,4,4,3,5,4,4,4,2,1,1,1,1,1,1,6,3,2,2,3,1,2,2,3,1,2,1,2,1,2,5,7,7,8,6,0,3,4,
5,6,7,0,3,4,5,5,7,6,7,7,8,7,8,8,9,6,7,7,8,9,3,2,2,2,3,2,4,5,1,3,1,2,1,1,1,1,5,4,
4,5,2,2,0,1,1,3,1,2,1,1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,
1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,
9,11,1,1,1,1,4,3,4,3,3,3,3,2,2,6,7,4,4,1,1,1,3,2,5,4,4,5,1,1,3,1,1,1,1,1,1};
static short Cyc_yydefact[831]={0,139,35,36,37,38,40,52,54,55,56,57,58,59,60,61,72,
73,74,90,91,42,0,0,41,0,0,115,116,0,0,408,159,409,87,0,53,0,144,145,148,1,0,9,0,
0,10,0,42,42,42,62,63,0,64,0,0,126,0,149,65,128,39,0,33,43,0,76,289,408,285,288,
287,0,283,0,0,0,0,0,182,0,290,17,19,0,0,0,0,66,0,0,0,0,2,0,0,0,0,21,0,95,96,98,
27,29,31,87,0,87,152,0,151,87,38,0,25,0,0,11,161,0,0,137,127,42,150,138,0,0,34,
80,0,78,0,0,297,296,286,295,24,0,0,0,0,0,42,42,200,202,0,0,70,71,160,207,0,129,0,
0,177,0,0,407,0,0,0,0,0,0,0,0,0,87,0,0,404,394,405,406,0,0,0,0,377,0,375,376,0,
308,321,329,331,333,335,337,339,342,347,350,353,357,0,359,378,393,392,0,3,0,5,0,
22,0,0,0,12,28,30,32,86,0,92,93,0,85,87,0,113,39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
408,235,237,0,243,239,0,241,224,225,226,0,227,228,229,0,306,26,13,98,167,183,0,0,
163,161,0,0,130,0,140,0,0,0,77,0,0,284,299,0,298,184,0,0,297,0,203,175,0,101,103,
161,0,209,201,210,68,0,69,89,0,88,0,179,0,181,67,0,0,367,0,323,357,0,324,325,0,0,
0,0,0,0,0,0,0,360,361,0,0,0,0,363,364,362,146,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,311,312,313,314,315,316,317,318,319,320,310,0,365,0,386,387,0,0,0,396,0,0,
147,18,0,20,0,97,99,186,185,14,0,82,94,0,0,105,106,108,0,112,87,121,0,0,0,0,0,0,
0,276,277,278,0,0,280,0,0,232,233,0,0,0,0,244,240,98,242,238,236,0,168,0,0,0,174,
162,169,132,0,0,0,163,134,136,135,131,153,142,141,0,48,47,0,45,81,328,79,75,292,
0,23,293,0,0,0,0,196,301,304,0,303,0,0,0,204,102,104,0,163,0,213,0,211,161,0,0,
223,206,208,178,0,0,0,187,191,0,0,0,0,0,0,0,0,0,0,0,0,0,0,395,402,0,401,330,0,
332,334,336,338,340,341,345,346,343,344,348,349,351,352,354,355,356,309,384,385,
380,0,382,383,0,0,0,4,6,0,109,100,0,0,0,114,123,122,0,0,117,0,0,0,0,0,0,0,0,275,
279,0,0,0,0,231,0,234,0,15,307,161,0,171,0,0,0,164,133,167,155,154,143,7,0,0,0,
300,199,0,302,195,197,294,0,291,205,176,218,0,212,215,0,163,0,214,0,366,0,0,188,
0,192,390,391,0,371,0,0,0,0,0,0,0,0,358,398,0,0,381,379,399,0,0,84,107,110,83,
111,124,121,121,0,0,0,0,0,0,0,0,0,0,250,281,0,0,0,16,163,0,172,170,0,163,0,0,0,0,
44,46,198,305,217,220,0,222,221,216,0,0,0,189,193,0,0,0,0,326,327,370,400,0,403,
322,397,0,120,119,245,0,250,260,0,0,0,0,0,0,0,0,0,0,0,0,282,0,0,157,173,165,156,
161,0,49,50,219,368,369,0,194,372,0,0,388,0,0,0,256,0,0,262,0,0,0,270,0,0,0,0,0,
0,0,0,249,0,230,0,163,0,0,0,0,389,0,246,0,0,0,247,261,263,264,0,272,271,0,266,0,
0,0,0,250,251,0,166,158,0,0,0,0,125,0,0,248,265,273,267,268,0,0,250,252,0,0,190,
373,0,256,257,269,250,253,274,51,0,256,258,250,254,374,259,255,0,0,0};static
short Cyc_yydefgoto[112]={828,41,42,43,249,44,383,45,385,46,215,47,48,64,65,453,
454,49,154,50,51,131,132,52,89,53,221,222,100,101,102,223,148,395,396,397,54,55,
554,555,556,56,57,58,59,125,111,449,478,60,479,440,583,432,436,437,293,270,160,
79,80,495,388,496,497,466,467,149,155,156,480,300,252,253,254,255,256,257,719,
773,258,259,283,284,285,469,470,471,260,261,370,189,457,190,191,192,193,194,195,
196,197,198,199,200,201,202,203,204,512,513,205,206};static short Cyc_yypact[831]={
2078,- -32768,- -32768,- -32768,- -32768,84,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- 4,222,905,- -32768,- 50,133,- -32768,- -32768,165,186,- -32768,218,- -32768,212,500,-
-32768,244,249,272,- -32768,- -32768,1962,- -32768,19,107,- -32768,227,- 4,- 4,- 4,-
-32768,- -32768,182,- -32768,55,674,80,131,16,- -32768,- 50,274,280,2496,- -32768,- 50,
292,- -32768,240,- -32768,- -32768,- -32768,1120,- -32768,291,313,343,1120,342,354,
365,161,- -32768,- -32768,4723,4723,145,4723,- -32768,361,4889,4275,4275,- -32768,
1962,2193,1962,2193,- -32768,376,379,- -32768,2373,2496,2496,2496,212,4889,212,-
-32768,- 50,- -32768,217,397,1359,- -32768,674,227,- -32768,1898,4723,2963,- -32768,
80,- 4,- -32768,- -32768,2193,389,- -32768,396,369,405,- 50,407,1120,- -32768,- -32768,
- -32768,- -32768,425,4275,53,415,203,- 4,- 4,199,- -32768,- 22,91,- -32768,- -32768,-
-32768,412,- 7,- -32768,514,260,- -32768,4513,419,- -32768,4275,4349,435,440,442,445,
447,449,454,212,4587,4587,- -32768,- -32768,- -32768,- -32768,1720,465,4661,4661,-
-32768,4661,- -32768,- -32768,476,- -32768,- 40,510,472,473,480,430,63,436,391,90,-
-32768,671,4661,597,- -32768,- -32768,29,519,- -32768,521,- -32768,524,- -32768,500,
3045,674,- -32768,- -32768,- -32768,- -32768,501,526,4889,- -32768,459,531,212,- 50,
533,- -32768,532,81,535,2553,537,563,577,599,3127,2553,326,598,2553,2553,- 2,594,-
-32768,- -32768,603,1481,1481,227,1481,- -32768,- -32768,- -32768,609,- -32768,-
-32768,- -32768,241,- -32768,- -32768,- -32768,620,635,- -32768,36,624,630,253,626,33,
- -32768,628,623,641,2,- 50,- -32768,4275,643,- -32768,- -32768,629,654,- -32768,255,
706,1120,4723,- -32768,655,645,4889,4889,2313,3209,56,- -32768,154,- -32768,36,-
-32768,- -32768,4723,- -32768,966,- -32768,348,- -32768,- -32768,4889,1720,- -32768,
4889,- -32768,- -32768,2635,- -32768,687,4275,2253,4275,4275,4275,4275,4275,663,
1720,- -32768,- -32768,1481,664,416,4275,- -32768,- -32768,- -32768,- -32768,4661,4275,
4661,4661,4661,4661,4661,4661,4661,4661,4661,4661,4661,4661,4661,4661,4661,4661,
4661,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,4275,- -32768,209,- -32768,- -32768,3291,247,4275,- -32768,2881,659,-
-32768,- -32768,1962,- -32768,1962,- -32768,- -32768,- -32768,- -32768,- -32768,4889,-
-32768,- -32768,4275,673,665,- -32768,667,4889,- -32768,212,518,4275,672,4275,4275,
747,1603,680,- -32768,- -32768,- -32768,268,739,- -32768,3373,2436,- -32768,- -32768,
2553,688,2553,1837,- -32768,- -32768,2373,- -32768,- -32768,- -32768,4275,- -32768,
4889,681,281,- -32768,- -32768,668,- -32768,691,683,4836,630,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,623,1962,- -32768,685,686,684,- -32768,- -32768,-
-32768,- -32768,- -32768,1120,- -32768,- -32768,701,4275,1120,144,- -32768,- -32768,-
-32768,692,689,694,- 13,697,- -32768,- -32768,- -32768,699,630,700,- -32768,682,154,
4783,4723,3455,- -32768,- -32768,412,- -32768,707,696,725,- -32768,- -32768,349,3045,
423,712,500,432,714,716,717,460,4889,719,727,4431,- -32768,- -32768,722,724,510,
135,472,473,480,430,63,63,436,436,436,436,391,391,90,90,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,726,- -32768,- -32768,179,363,4723,- -32768,- -32768,
731,- -32768,- -32768,459,4275,733,- -32768,- -32768,- -32768,735,- 50,303,467,4275,
477,479,740,3537,3619,312,- -32768,- -32768,738,743,741,745,- -32768,742,- -32768,
674,- -32768,- -32768,748,4723,- -32768,749,36,750,- -32768,- -32768,134,- -32768,-
-32768,- -32768,- -32768,99,744,2,- -32768,- -32768,746,- -32768,- -32768,- -32768,-
-32768,788,- -32768,- -32768,- -32768,- -32768,753,- -32768,- -32768,756,630,58,-
-32768,757,761,498,754,- -32768,2717,- -32768,- -32768,- -32768,674,- -32768,4275,827,
4275,4349,759,761,765,2881,- -32768,- -32768,4275,4275,- -32768,- -32768,- -32768,
2881,378,- -32768,- -32768,- -32768,- -32768,- -32768,167,74,518,2553,493,771,2553,
4275,3701,314,3783,319,3865,584,- -32768,773,4275,785,- -32768,630,45,- -32768,-
-32768,782,630,4889,775,779,781,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
783,- -32768,- -32768,- -32768,784,786,4275,- -32768,- -32768,3045,540,789,791,-
-32768,687,- -32768,- -32768,385,- -32768,- -32768,- -32768,794,- -32768,- -32768,857,
803,584,- -32768,543,2553,545,3947,2553,547,4029,4111,328,1120,798,809,- -32768,
344,2553,- -32768,- -32768,799,- -32768,748,821,- -32768,- -32768,- -32768,- -32768,-
-32768,219,- -32768,- -32768,4889,888,- -32768,2799,4889,2553,632,814,817,- -32768,
2553,2553,553,- -32768,2553,2553,555,2553,557,4193,- 42,1481,- -32768,4275,- -32768,
691,630,812,4275,815,818,- -32768,562,- -32768,4275,810,822,- -32768,- -32768,-
-32768,- -32768,2553,- -32768,- -32768,2553,- -32768,2553,2553,564,4275,1084,- -32768,
566,- -32768,- -32768,831,390,823,4889,- -32768,246,1481,- -32768,- -32768,- -32768,-
-32768,- -32768,2553,305,584,- -32768,2553,824,- -32768,- -32768,825,1237,- -32768,-
-32768,1084,- -32768,- -32768,- -32768,826,632,- -32768,584,- -32768,- -32768,- -32768,
- -32768,924,925,- -32768};static short Cyc_yypgoto[112]={- -32768,28,- -32768,610,-
-32768,- -32768,- -32768,- -32768,- -32768,- 52,- 27,- 56,- -32768,406,- 48,339,- -32768,
93,- -32768,- 65,- -32768,- -32768,- 110,- -32768,- 9,- -32768,- 107,- -32768,- -32768,-
-32768,720,715,- 37,- -32768,- -32768,386,- -32768,- -32768,27,- -32768,- -32768,- 5,- 30,
- 80,- -32768,- -32768,882,- -32768,- 106,- 49,- 105,- 360,183,359,366,644,- 393,- 88,- 290,
828,- -32768,- 202,- 164,- 348,- 270,- -32768,483,- 175,- 69,- 108,- 12,- 94,188,- -32768,-
-32768,- 29,- 212,- -32768,- 614,- 525,- -32768,- -32768,- 21,661,- -32768,351,- -32768,-
-32768,152,215,- -32768,- 104,- 369,- 158,612,611,613,614,615,370,148,367,371,- 71,
690,- -32768,- -32768,- -32768,- 331,- -32768,- -32768,11};static short Cyc_yytable[4996]={
118,319,81,159,116,333,112,320,130,123,112,61,387,272,268,269,150,151,465,451,
147,147,490,147,281,546,147,119,124,117,90,541,786,67,82,340,83,424,425,31,427,
580,103,147,537,33,61,118,217,218,219,116,135,61,158,147,147,139,61,251,316,118,
304,250,109,262,113,787,298,61,94,224,127,143,216,341,123,133,309,552,553,603,
586,158,82,569,301,420,263,82,602,452,744,63,302,595,292,305,220,433,225,306,444,
302,228,421,61,61,61,61,32,336,337,264,338,147,378,95,96,605,508,110,226,208,209,
210,211,681,379,61,32,371,1,348,349,434,299,305,491,61,492,445,577,380,31,133,
310,82,724,32,33,490,288,308,507,289,276,147,296,118,110,302,305,262,328,442,682,
297,455,61,350,351,404,807,- 118,62,456,63,146,146,642,146,120,405,146,224,390,
121,671,672,673,122,817,251,251,448,251,250,250,303,250,146,824,356,483,97,98,
302,264,827,447,357,358,146,146,483,298,400,435,398,31,473,84,431,61,147,33,309,
1,147,147,147,267,35,152,61,489,- 161,401,430,147,153,147,- 161,634,597,426,147,
147,680,147,63,487,435,463,464,484,476,477,158,- 87,485,61,- 87,147,486,500,468,88,
- 87,158,31,85,146,88,158,251,33,107,124,250,108,697,430,545,530,531,532,822,133,
456,636,550,86,287,618,826,32,534,82,82,296,90,291,723,535,87,188,207,726,297,31,
38,39,40,146,31,33,61,88,61,66,33,430,88,99,147,227,765,35,465,627,334,61,147,
159,538,274,- 183,429,38,39,40,539,- 183,430,91,689,92,1,430,267,118,461,224,813,
563,441,147,430,456,118,224,311,312,570,566,147,689,588,118,93,430,128,116,32,
147,610,608,609,727,579,129,146,587,292,158,146,146,146,413,134,551,136,61,158,
61,575,146,574,146,646,137,791,158,146,146,647,146,430,657,543,711,544,816,688,
430,714,430,147,147,407,146,430,415,292,756,414,416,389,418,419,430,138,639,487,
688,31,631,593,140,147,760,33,596,456,- 209,158,430,616,307,- 209,- 209,617,104,105,
106,- 209,141,297,1,61,693,637,157,334,142,638,694,279,689,82,664,498,212,501,82,
147,589,505,669,334,305,213,146,739,700,735,229,740,810,277,146,515,430,621,522,
523,524,525,278,1,346,347,352,353,280,354,355,665,282,61,482,147,286,1,146,290,
118,510,430,315,262,552,553,146,619,430,302,540,699,275,435,389,146,622,430,321,
688,502,503,504,322,398,323,663,788,324,31,325,511,326,294,295,33,557,327,559,
560,61,564,35,766,626,430,118,645,335,394,116,648,430,38,39,40,806,339,146,146,
468,650,430,651,430,342,533,814,684,685,31,511,343,216,344,389,33,705,430,345,
146,391,821,35,31,823,147,717,718,571,33,573,82,- 180,38,39,40,307,- 180,- 180,381,
812,382,763,- 180,384,297,392,38,39,40,399,511,402,158,403,146,701,406,511,408,16,
17,18,736,430,576,745,430,747,430,751,430,409,769,771,772,778,430,781,430,783,
430,372,373,374,796,312,804,430,808,430,146,147,702,703,410,147,2,3,4,114,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,757,411,21,375,417,612,251,422,158,423,250,
376,377,428,649,22,389,23,654,656,520,521,24,526,527,214,27,28,431,528,529,438,
82,443,147,251,459,29,30,250,38,39,40,439,446,450,251,458,68,475,250,359,360,361,
362,363,364,365,366,367,368,251,460,474,251,250,340,146,250,506,542,509,32,548,
34,547,369,115,558,690,549,561,565,36,37,567,572,578,581,582,584,590,592,591,594,
32,599,600,607,31,70,601,71,72,614,33,604,606,708,710,462,713,73,716,613,74,75,
721,615,620,463,464,623,76,624,625,628,77,78,632,68,629,633,635,146,640,389,643,
146,644,704,658,734,707,692,659,652,661,660,389,267,674,698,666,662,314,691,389,
317,317,678,668,676,679,630,686,695,749,330,331,753,755,696,683,706,720,317,317,
722,317,725,31,70,728,71,72,729,33,730,742,731,732,146,733,73,737,317,74,75,738,
741,746,463,464,750,76,743,762,389,77,78,758,785,759,761,764,789,767,774,775,792,
793,794,798,795,809,799,797,829,830,811,819,820,825,770,675,499,386,641,776,777,
393,805,779,780,126,782,68,670,790,488,667,271,598,472,677,514,516,0,389,517,0,
518,0,519,0,0,0,0,0,800,1,0,801,317,802,803,0,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,0,815,0,69,70,818,71,72,0,33,0,0,22,0,0,0,73,0,0,74,75,0,0,27,28,0,0,
76,0,0,0,77,78,0,29,30,0,0,0,317,0,317,317,317,317,317,317,317,317,317,317,317,
317,317,317,317,317,317,0,0,0,265,0,0,31,0,0,0,0,32,33,34,0,267,0,0,0,307,- 161,0,
36,37,0,0,- 161,0,297,0,38,39,40,0,0,317,0,0,2,3,4,114,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,717,718,21,161,162,230,0,231,232,233,234,235,236,237,238,22,163,
23,164,239,0,165,24,240,0,0,27,28,241,166,167,242,243,168,169,170,171,29,30,244,
172,0,173,0,174,175,0,0,317,0,0,0,68,0,0,0,0,0,0,0,0,0,0,0,0,0,0,245,176,177,178,
179,32,33,34,0,246,115,0,0,180,0,0,36,248,0,0,0,0,0,0,182,0,317,183,184,185,0,0,
0,0,186,187,31,70,0,71,72,0,33,0,0,0,0,0,0,73,0,0,74,75,0,0,0,0,0,0,76,0,0,0,77,
78,317,2,3,4,114,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,771,772,21,161,162,230,
0,231,232,233,234,235,236,237,238,22,163,23,164,239,0,165,24,240,0,0,27,28,241,
166,167,242,243,168,169,170,171,29,30,244,172,0,173,0,174,175,0,0,0,0,0,0,0,0,0,
0,0,317,0,0,0,0,0,0,0,317,0,245,176,177,178,179,32,33,34,0,246,115,0,0,180,0,0,
36,248,0,0,0,0,0,0,182,0,0,183,184,185,0,0,0,0,186,187,2,3,4,114,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,21,161,162,230,0,231,232,233,234,235,236,237,238,
22,163,23,164,239,0,165,24,240,0,0,27,28,241,166,167,242,243,168,169,170,171,29,
30,244,172,0,173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,245,176,177,
178,179,32,33,34,0,246,115,247,0,180,0,0,36,248,0,0,0,0,0,0,182,0,0,183,184,185,
0,0,0,0,186,187,2,3,4,114,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,161,
162,230,0,231,232,233,234,235,236,237,238,22,163,23,164,239,0,165,24,240,0,0,27,
28,241,166,167,242,243,168,169,170,171,29,30,244,172,0,173,0,174,175,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,245,176,177,178,179,32,33,34,0,246,115,0,0,180,0,0,
36,248,0,0,0,0,0,0,182,0,0,183,184,185,0,0,0,0,186,187,2,3,4,114,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,21,161,162,0,0,0,0,0,0,0,0,0,0,22,163,23,164,0,0,
165,24,0,0,0,27,28,0,166,167,0,0,168,169,170,171,29,30,0,172,0,173,0,174,175,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,32,33,34,0,562,0,0,0,
180,0,0,36,248,0,0,0,0,0,0,182,0,0,183,184,185,0,0,0,0,186,187,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,161,162,0,0,0,0,0,0,0,0,0,0,22,163,0,164,0,0,165,0,0,
0,0,27,28,0,166,167,0,0,168,169,170,171,29,30,0,172,0,173,0,174,175,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,32,33,34,0,0,332,0,0,180,0,0,36,
248,0,0,0,0,0,0,182,0,0,183,184,185,0,0,0,0,186,187,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,161,162,0,0,0,0,0,0,0,0,0,0,22,163,0,164,0,0,165,0,0,0,0,27,28,
0,166,167,0,0,168,169,170,171,29,30,0,172,0,173,0,174,175,0,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,0,0,0,0,31,176,177,178,179,32,33,34,0,22,0,0,0,180,0,0,36,
248,0,0,27,28,0,0,182,0,0,183,184,185,0,29,30,0,186,187,- 8,1,0,2,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,265,0,21,266,0,0,0,0,32,0,34,0,267,0,0,22,0,23,
0,36,37,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,32,33,34,0,0,0,- 8,0,35,0,0,36,37,0,0,0,0,0,
0,38,39,40,- 8,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,
0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,32,33,34,0,0,0,0,0,
35,0,0,36,37,0,0,0,0,0,0,38,39,40,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,
0,0,0,0,29,30,1,0,2,3,4,114,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,
0,0,31,0,0,0,0,32,33,34,22,0,0,- 8,0,35,0,24,36,37,0,27,28,0,0,0,38,39,40,0,0,0,
29,30,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,31,0,0,
0,0,32,33,34,22,0,0,0,0,35,0,0,36,37,0,27,28,0,0,0,38,39,40,0,0,0,29,30,0,0,2,3,
4,114,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,265,0,0,0,0,0,0,0,32,0,
34,22,267,23,0,0,296,0,24,36,37,0,27,28,0,297,0,38,39,40,0,0,0,29,30,0,0,0,0,0,2,
3,4,114,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,32,0,34,0,0,
115,22,214,23,0,0,36,37,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,2,3,4,114,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,32,0,34,22,0,0,0,0,
0,0,24,36,37,0,27,28,0,0,0,0,0,0,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,161,162,230,0,231,232,233,234,235,236,237,238,32,163,34,164,239,0,165,0,
240,0,0,36,37,241,166,167,242,243,168,169,170,171,0,0,244,172,0,173,0,174,175,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,245,176,177,178,179,0,33,0,0,246,115,0,0,
180,0,0,0,181,161,162,0,0,0,0,182,493,0,183,184,185,0,163,0,164,186,187,165,0,0,
0,0,0,0,0,166,167,0,0,168,169,170,171,0,0,0,172,0,173,0,174,175,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,0,33,0,0,0,318,494,0,180,0,0,0,181,
161,162,0,463,464,0,182,0,0,183,184,185,0,163,0,164,186,187,165,0,0,0,0,0,0,0,
166,167,0,0,168,169,170,171,0,0,0,172,0,173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,31,176,177,178,179,0,33,0,0,0,318,687,0,180,0,0,0,181,161,162,0,
463,464,0,182,0,0,183,184,185,0,163,0,164,186,187,165,0,0,0,0,0,0,0,166,167,0,0,
168,169,170,171,0,0,0,172,0,173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,31,176,177,178,179,0,33,0,0,0,318,768,0,180,0,0,0,181,161,162,0,463,464,0,
182,0,0,183,184,185,0,163,0,164,186,187,165,0,0,0,0,0,0,0,166,167,0,0,168,169,
170,171,0,0,0,172,0,173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,
176,177,178,179,0,33,0,0,0,318,0,0,180,0,0,0,181,161,162,0,463,464,0,182,0,0,183,
184,185,0,163,0,164,186,187,165,0,0,0,0,0,0,0,166,167,0,0,168,169,170,171,0,0,0,
172,0,173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,
0,33,0,0,0,0,0,0,180,0,0,0,181,161,162,0,0,0,273,182,0,0,183,184,185,0,163,0,164,
186,187,165,0,0,0,0,0,0,0,166,167,0,0,168,169,170,171,0,0,0,172,0,173,0,174,175,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,0,33,0,0,0,318,0,0,
180,0,0,0,181,161,162,0,0,0,0,182,0,0,183,184,185,0,163,0,164,186,187,165,0,0,0,
0,0,0,0,166,167,0,0,168,169,170,171,0,0,0,172,0,173,0,174,175,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,0,33,0,0,412,0,0,0,180,0,0,0,181,161,
162,0,0,0,0,182,0,0,183,184,185,0,163,0,164,186,187,165,0,0,0,0,0,0,0,166,167,0,
0,168,169,170,171,0,0,0,172,0,173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,31,176,177,178,179,0,33,0,0,0,0,0,0,180,0,0,0,181,161,162,0,0,0,481,182,0,
0,183,184,185,0,163,0,164,186,187,165,0,0,0,0,0,0,0,166,167,0,0,168,169,170,171,
0,0,0,172,0,173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,
178,179,0,33,0,0,0,0,0,0,180,536,0,0,181,161,162,0,0,0,0,182,0,0,183,184,185,0,
163,0,164,186,187,165,0,0,0,0,0,0,0,166,167,0,0,168,169,170,171,0,0,0,172,0,173,
0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,0,33,0,0,
0,0,0,0,180,568,0,0,181,161,162,0,0,0,0,182,0,0,183,184,185,0,163,0,164,186,187,
165,0,0,0,0,0,0,0,166,167,0,0,168,169,170,171,0,0,0,172,0,173,0,174,175,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,0,33,0,0,0,0,0,0,180,0,0,0,
181,161,162,0,0,0,611,182,0,0,183,184,185,0,163,0,164,186,187,165,0,0,0,0,0,0,0,
166,167,0,0,168,169,170,171,0,0,0,172,0,173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,31,176,177,178,179,0,33,0,0,653,0,0,0,180,0,0,0,181,161,162,0,0,
0,0,182,0,0,183,184,185,0,163,0,164,186,187,165,0,0,0,0,0,0,0,166,167,0,0,168,
169,170,171,0,0,0,172,0,173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
31,176,177,178,179,0,33,0,0,655,0,0,0,180,0,0,0,181,161,162,0,0,0,0,182,0,0,183,
184,185,0,163,0,164,186,187,165,0,0,0,0,0,0,0,166,167,0,0,168,169,170,171,0,0,0,
172,0,173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,
0,33,0,0,0,0,0,0,180,709,0,0,181,161,162,0,0,0,0,182,0,0,183,184,185,0,163,0,164,
186,187,165,0,0,0,0,0,0,0,166,167,0,0,168,169,170,171,0,0,0,172,0,173,0,174,175,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,0,33,0,0,0,0,0,0,
180,712,0,0,181,161,162,0,0,0,0,182,0,0,183,184,185,0,163,0,164,186,187,165,0,0,
0,0,0,0,0,166,167,0,0,168,169,170,171,0,0,0,172,0,173,0,174,175,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,0,33,0,0,715,0,0,0,180,0,0,0,181,
161,162,0,0,0,0,182,0,0,183,184,185,0,163,0,164,186,187,165,0,0,0,0,0,0,0,166,
167,0,0,168,169,170,171,0,0,0,172,0,173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,31,176,177,178,179,0,33,0,0,0,0,0,0,180,748,0,0,181,161,162,0,0,0,0,
182,0,0,183,184,185,0,163,0,164,186,187,165,0,0,0,0,0,0,0,166,167,0,0,168,169,
170,171,0,0,0,172,0,173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,
176,177,178,179,0,33,0,0,0,0,0,0,180,752,0,0,181,161,162,0,0,0,0,182,0,0,183,184,
185,0,163,0,164,186,187,165,0,0,0,0,0,0,0,166,167,0,0,168,169,170,171,0,0,0,172,
0,173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,0,
33,0,0,0,0,0,0,180,754,0,0,181,161,162,0,0,0,0,182,0,0,183,184,185,0,163,0,164,
186,187,165,0,0,0,0,0,0,0,166,167,0,0,168,169,170,171,0,0,0,172,0,173,0,174,175,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,0,33,0,0,0,0,0,0,
180,784,0,0,181,161,162,0,0,0,0,182,0,0,183,184,185,0,163,0,164,186,187,165,0,0,
0,0,0,0,0,166,167,0,0,168,169,170,171,0,0,0,172,0,173,0,174,175,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,0,33,0,0,0,161,162,0,180,0,0,0,181,
0,0,0,0,0,163,182,0,0,183,184,185,0,0,0,0,186,187,166,167,0,0,168,169,170,171,0,
0,0,0,0,173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,
179,0,33,0,0,0,318,0,0,180,0,0,0,181,161,162,0,0,0,0,182,0,0,183,184,185,0,163,0,
0,186,187,0,0,0,0,0,0,0,0,166,167,0,0,168,169,170,171,0,0,0,0,0,173,0,174,175,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,0,33,0,0,0,630,0,0,
180,0,0,0,181,161,162,0,0,0,0,182,0,0,183,184,185,0,163,0,0,186,187,0,0,0,0,0,0,
0,0,166,167,0,0,168,169,170,171,0,0,0,0,0,173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,31,176,177,178,179,0,33,0,0,0,161,162,0,313,0,0,0,181,0,0,0,0,
0,163,182,0,0,183,184,185,0,0,0,0,186,187,166,167,0,0,168,169,170,171,0,0,0,0,0,
173,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,176,177,178,179,0,33,
0,0,0,161,162,0,329,0,0,0,181,0,0,0,0,0,163,182,0,0,183,184,185,0,0,0,0,186,187,
166,167,0,0,168,169,170,171,0,0,0,0,0,173,0,174,175,0,0,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,0,0,31,176,177,178,179,0,33,0,0,0,22,0,0,180,0,0,0,181,0,0,
0,27,28,0,182,0,0,183,184,185,0,0,29,30,186,187,144,0,0,0,0,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,32,0,34,22,0,145,0,0,0,0,0,36,37,0,27,
28,0,0,0,0,0,0,0,0,0,29,30,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,
0,0,0,0,265,0,0,0,22,0,0,0,32,0,34,0,267,0,0,27,28,0,0,36,37,0,0,0,0,0,29,30,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,585,0,0,0,22,0,0,0,32,0,
34,0,0,0,0,27,28,0,0,36,37,0,0,0,0,0,29,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,34,0,0,0,0,0,0,0,0,36,37};static short Cyc_yycheck[
4996]={56,165,23,91,56,180,55,165,64,57,59,0,214,121,120,120,85,86,288,17,85,86,
312,88,134,394,91,56,58,56,35,379,74,22,23,75,25,249,250,89,252,434,47,108,375,
95,35,103,104,105,106,103,73,42,91,120,121,78,47,115,164,117,69,115,53,117,55,
109,148,58,42,108,61,82,103,115,124,66,158,5,6,474,442,120,73,416,108,89,117,78,
103,89,706,97,116,464,145,104,107,63,109,108,69,116,113,107,95,96,97,98,94,182,
183,118,185,180,87,98,99,479,332,105,111,95,96,97,98,69,99,118,94,202,1,70,71,99,
148,104,313,128,315,108,432,114,89,134,158,136,103,94,95,441,99,158,329,102,128,
222,102,215,105,116,104,215,173,270,108,111,278,158,107,108,91,787,100,91,280,97,
85,86,549,88,102,102,91,222,215,107,89,90,91,111,806,249,250,275,252,249,250,108,
252,108,816,113,298,98,99,116,213,823,275,121,122,120,121,309,296,226,267,224,89,
290,89,89,213,290,95,307,1,294,295,296,98,102,89,224,305,103,227,104,305,96,307,
109,109,101,251,312,313,609,315,97,300,302,110,111,102,294,295,296,99,107,251,
102,329,111,322,288,107,102,307,89,107,180,107,312,332,95,96,309,332,99,630,104,
391,356,357,358,813,278,394,112,399,107,142,497,821,94,89,288,289,102,307,100,
664,96,88,92,93,669,111,89,113,114,115,222,89,95,307,107,309,99,95,104,107,98,
391,110,109,102,600,506,180,322,399,423,89,122,98,98,113,114,115,96,104,104,102,
617,99,1,104,98,408,98,391,109,408,104,423,104,464,417,399,103,104,417,98,432,
638,449,426,99,104,99,426,94,441,485,484,484,670,100,102,290,449,434,423,294,295,
296,238,99,401,102,383,432,385,426,305,426,307,98,89,763,441,312,313,104,315,104,
98,383,98,385,109,617,104,98,104,484,485,233,329,104,98,474,98,239,102,214,242,
243,104,90,542,483,638,89,509,460,98,506,98,95,465,549,98,484,104,100,102,103,
104,104,48,49,50,109,104,111,1,450,626,100,103,313,101,104,626,100,740,460,577,
321,98,323,465,542,450,327,585,329,104,104,391,100,108,689,91,104,100,102,399,
341,104,500,348,349,350,351,104,1,72,73,68,69,101,116,117,578,103,500,297,578,89,
1,423,102,574,103,104,102,574,5,6,432,103,104,116,377,634,125,581,318,441,103,
104,102,740,324,325,326,102,548,102,574,758,102,89,102,335,102,146,147,95,403,
102,405,406,548,408,102,737,103,104,621,555,102,109,621,103,104,113,114,115,787,
100,484,485,600,103,104,103,104,74,370,798,89,90,89,375,119,621,120,379,95,103,
104,118,506,99,813,102,89,816,670,22,23,420,95,422,600,98,113,114,115,102,103,
104,100,795,100,727,109,100,111,100,113,114,115,99,416,99,670,102,542,645,102,
423,102,17,18,19,103,104,430,103,104,103,104,103,104,89,741,22,23,103,104,103,
104,103,104,65,66,67,103,104,103,104,103,104,578,737,646,647,98,741,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,717,98,24,102,102,486,758,109,741,102,758,
110,111,100,558,37,497,39,562,563,346,347,44,352,353,101,48,49,89,354,355,103,
717,103,795,787,103,59,60,787,113,114,115,109,112,100,798,100,38,100,798,76,77,
78,79,80,81,82,83,84,85,813,104,104,816,813,75,670,816,102,107,103,94,104,96,98,
101,99,102,623,109,30,98,105,106,42,94,102,116,94,103,102,104,103,89,94,100,104,
112,89,90,103,92,93,104,95,103,103,652,653,100,655,102,657,103,105,106,661,89,
103,110,111,104,113,104,104,103,117,118,103,38,100,104,103,737,100,617,100,741,
100,648,99,686,651,625,98,102,98,103,630,98,103,633,100,108,161,25,638,164,165,
103,107,112,103,99,107,103,711,174,175,714,715,103,112,99,98,182,183,89,185,94,
89,90,104,92,93,103,95,103,28,103,103,795,103,102,102,202,105,106,104,102,709,
110,111,712,113,99,104,689,117,118,109,756,100,722,90,760,25,100,98,104,765,103,
109,102,90,100,771,0,0,103,103,103,103,742,592,322,213,548,747,748,222,786,751,
752,59,754,38,585,762,302,581,120,466,289,600,340,342,- 1,740,343,- 1,344,- 1,345,-
1,- 1,- 1,- 1,- 1,778,1,- 1,781,280,783,784,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,- 1,- 1,804,- 1,89,90,808,92,93,- 1,95,- 1,- 1,37,- 1,- 1,- 1,102,- 1,- 1,105,106,
- 1,- 1,48,49,- 1,- 1,113,- 1,- 1,- 1,117,118,- 1,59,60,- 1,- 1,- 1,340,- 1,342,343,344,345,
346,347,348,349,350,351,352,353,354,355,356,357,358,- 1,- 1,- 1,86,- 1,- 1,89,- 1,- 1,-
1,- 1,94,95,96,- 1,98,- 1,- 1,- 1,102,103,- 1,105,106,- 1,- 1,109,- 1,111,- 1,113,114,115,
- 1,- 1,394,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,
27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,
54,55,56,57,58,59,60,61,62,- 1,64,- 1,66,67,- 1,- 1,464,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,- 1,98,99,- 1,- 1,102,- 1,- 1,105,
106,- 1,- 1,- 1,- 1,- 1,- 1,113,- 1,509,116,117,118,- 1,- 1,- 1,- 1,123,124,89,90,- 1,92,93,
- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,105,106,- 1,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,- 1,117,118,
549,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,
31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,626,- 1,- 1,- 1,- 1,-
1,- 1,- 1,634,- 1,89,90,91,92,93,94,95,96,- 1,98,99,- 1,- 1,102,- 1,- 1,105,106,- 1,- 1,- 1,
- 1,- 1,- 1,113,- 1,- 1,116,117,118,- 1,- 1,- 1,- 1,123,124,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,
- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,- 1,64,- 1,66,67,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,
96,- 1,98,99,100,- 1,102,- 1,- 1,105,106,- 1,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,116,117,118,- 1,
- 1,- 1,- 1,123,124,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,
26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,
53,54,55,56,57,58,59,60,61,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,- 1,98,99,- 1,- 1,102,- 1,- 1,105,
106,- 1,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,116,117,118,- 1,- 1,- 1,- 1,123,124,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
38,39,40,- 1,- 1,43,44,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,- 1,62,- 1,64,
- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,
92,93,94,95,96,- 1,98,- 1,- 1,- 1,102,- 1,- 1,105,106,- 1,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,116,
117,118,- 1,- 1,- 1,- 1,123,124,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,
26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,
- 1,- 1,55,56,57,58,59,60,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,95,96,- 1,- 1,99,- 1,- 1,102,- 1,- 1,105,
106,- 1,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,116,117,118,- 1,- 1,- 1,- 1,123,124,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,
- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,- 1,62,- 1,64,- 1,66,67,
- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,94,
95,96,- 1,37,- 1,- 1,- 1,102,- 1,- 1,105,106,- 1,- 1,48,49,- 1,- 1,113,- 1,- 1,116,117,118,-
1,59,60,- 1,123,124,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,86,-
1,24,89,- 1,- 1,- 1,- 1,94,- 1,96,- 1,98,- 1,- 1,37,- 1,39,- 1,105,106,- 1,44,- 1,46,47,48,
49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,96,- 1,- 1,- 1,100,- 1,
102,- 1,- 1,105,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,
- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,
95,96,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,105,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,1,- 1,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,
1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,89,-
1,- 1,- 1,- 1,94,95,96,37,- 1,- 1,100,- 1,102,- 1,44,105,106,- 1,48,49,- 1,- 1,- 1,113,114,
115,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,94,95,96,37,- 1,- 1,- 1,- 1,102,- 1,- 1,105,106,- 1,
48,49,- 1,- 1,- 1,113,114,115,- 1,- 1,- 1,59,60,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,24,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,96,37,98,39,- 1,- 1,
102,- 1,44,105,106,- 1,48,49,- 1,111,- 1,113,114,115,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,
96,- 1,- 1,99,37,101,39,- 1,- 1,105,106,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
59,60,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,96,37,- 1,- 1,- 1,- 1,- 1,- 1,44,105,106,- 1,48,49,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,94,38,96,40,41,- 1,43,- 1,45,- 1,- 1,105,106,
50,51,52,53,54,55,56,57,58,- 1,- 1,61,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,95,- 1,- 1,98,99,- 1,- 1,102,-
1,- 1,- 1,106,25,26,- 1,- 1,- 1,- 1,113,32,- 1,116,117,118,- 1,38,- 1,40,123,124,43,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,95,- 1,- 1,- 1,99,
100,- 1,102,- 1,- 1,- 1,106,25,26,- 1,110,111,- 1,113,- 1,- 1,116,117,118,- 1,38,- 1,40,
123,124,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,
67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,
- 1,95,- 1,- 1,- 1,99,100,- 1,102,- 1,- 1,- 1,106,25,26,- 1,110,111,- 1,113,- 1,- 1,116,117,
118,- 1,38,- 1,40,123,124,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,95,- 1,- 1,- 1,99,100,- 1,102,- 1,- 1,- 1,106,25,26,- 1,110,111,- 1,113,
- 1,- 1,116,117,118,- 1,38,- 1,40,123,124,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,
57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,95,- 1,- 1,- 1,99,- 1,- 1,102,- 1,- 1,- 1,106,25,26,- 1,
110,111,- 1,113,- 1,- 1,116,117,118,- 1,38,- 1,40,123,124,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,
52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,-
1,106,25,26,- 1,- 1,- 1,112,113,- 1,- 1,116,117,118,- 1,38,- 1,40,123,124,43,- 1,- 1,- 1,-
1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,95,- 1,- 1,- 1,99,- 1,
- 1,102,- 1,- 1,- 1,106,25,26,- 1,- 1,- 1,- 1,113,- 1,- 1,116,117,118,- 1,38,- 1,40,123,124,
43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,95,- 1,
- 1,98,- 1,- 1,- 1,102,- 1,- 1,- 1,106,25,26,- 1,- 1,- 1,- 1,113,- 1,- 1,116,117,118,- 1,38,- 1,
40,123,124,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,
66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,
93,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,- 1,106,25,26,- 1,- 1,- 1,112,113,- 1,- 1,116,117,
118,- 1,38,- 1,40,123,124,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,106,25,26,- 1,- 1,- 1,- 1,113,-
1,- 1,116,117,118,- 1,38,- 1,40,123,124,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,
57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,106,25,26,- 1,
- 1,- 1,- 1,113,- 1,- 1,116,117,118,- 1,38,- 1,40,123,124,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,
- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,- 1,
106,25,26,- 1,- 1,- 1,112,113,- 1,- 1,116,117,118,- 1,38,- 1,40,123,124,43,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,95,- 1,- 1,98,- 1,- 1,- 1,
102,- 1,- 1,- 1,106,25,26,- 1,- 1,- 1,- 1,113,- 1,- 1,116,117,118,- 1,38,- 1,40,123,124,43,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,95,- 1,- 1,
98,- 1,- 1,- 1,102,- 1,- 1,- 1,106,25,26,- 1,- 1,- 1,- 1,113,- 1,- 1,116,117,118,- 1,38,- 1,40,
123,124,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,
67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,
- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,106,25,26,- 1,- 1,- 1,- 1,113,- 1,- 1,116,117,
118,- 1,38,- 1,40,123,124,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,106,25,26,- 1,- 1,- 1,- 1,113,-
1,- 1,116,117,118,- 1,38,- 1,40,123,124,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,
57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,95,- 1,- 1,98,- 1,- 1,- 1,102,- 1,- 1,- 1,106,25,26,- 1,
- 1,- 1,- 1,113,- 1,- 1,116,117,118,- 1,38,- 1,40,123,124,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,
- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,
106,25,26,- 1,- 1,- 1,- 1,113,- 1,- 1,116,117,118,- 1,38,- 1,40,123,124,43,- 1,- 1,- 1,- 1,-
1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,
102,103,- 1,- 1,106,25,26,- 1,- 1,- 1,- 1,113,- 1,- 1,116,117,118,- 1,38,- 1,40,123,124,43,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,66,67,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,95,- 1,- 1,
- 1,- 1,- 1,- 1,102,103,- 1,- 1,106,25,26,- 1,- 1,- 1,- 1,113,- 1,- 1,116,117,118,- 1,38,- 1,
40,123,124,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,62,- 1,64,- 1,
66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,
93,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,106,25,26,- 1,- 1,- 1,- 1,113,- 1,- 1,116,117,
118,- 1,38,- 1,40,123,124,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
62,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,95,- 1,- 1,- 1,25,26,- 1,102,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,38,113,- 1,
- 1,116,117,118,- 1,- 1,- 1,- 1,123,124,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,
66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,90,91,92,
93,- 1,95,- 1,- 1,- 1,99,- 1,- 1,102,- 1,- 1,- 1,106,25,26,- 1,- 1,- 1,- 1,113,- 1,- 1,116,117,
118,- 1,38,- 1,- 1,123,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,95,- 1,- 1,- 1,99,- 1,- 1,102,- 1,- 1,- 1,106,25,26,- 1,- 1,- 1,- 1,113,- 1,
- 1,116,117,118,- 1,38,- 1,- 1,123,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,
58,- 1,- 1,- 1,- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,89,90,91,92,93,- 1,95,- 1,- 1,- 1,25,26,- 1,102,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,
- 1,38,113,- 1,- 1,116,117,118,- 1,- 1,- 1,- 1,123,124,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
- 1,- 1,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,90,91,92,93,- 1,95,- 1,- 1,- 1,25,26,- 1,102,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,38,113,- 1,
- 1,116,117,118,- 1,- 1,- 1,- 1,123,124,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,
66,67,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,89,90,91,92,
93,- 1,95,- 1,- 1,- 1,37,- 1,- 1,102,- 1,- 1,- 1,106,- 1,- 1,- 1,48,49,- 1,113,- 1,- 1,116,117,
118,- 1,- 1,59,60,123,124,63,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,96,37,- 1,99,- 1,- 1,- 1,- 1,- 1,105,106,- 1,
48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,8,9,10,11,12,13,14,15,16,17,18,19,20,21,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,37,- 1,- 1,- 1,94,- 1,96,- 1,98,- 1,- 1,48,
49,- 1,- 1,105,106,- 1,- 1,- 1,- 1,- 1,59,60,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,37,- 1,- 1,- 1,94,- 1,96,- 1,- 1,- 1,- 1,48,49,
- 1,- 1,105,106,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,105,106};unsigned char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible";
unsigned char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow";extern
void Cyc_yyerror(struct _tagged_arr);extern int Cyc_yylex();static int Cyc_yychar=(int)'\000';
void*Cyc_yylval=(void*)Cyc_YYINITIALSVAL;static int Cyc_yynerrs=0;struct _tuple18{
void*f1;void*f2;};struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;}
;int Cyc_yyparse(){int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp877=10000;short*_tmp878=(short*)
_cycalloc_atomic(_check_times(sizeof(short),_tmp877));{unsigned int _tmp879=
_tmp877;unsigned int i;for(i=0;i < _tmp879;i ++){_tmp878[i]=(short)0;}};_tmp878;});
int yyvsp_offset;void**yyvs=(void**)({unsigned int _tmp874=10000;void**_tmp875=(
void**)_cycalloc(_check_times(sizeof(void*),_tmp874));{unsigned int _tmp876=
_tmp874;unsigned int i;for(i=0;i < _tmp876;i ++){_tmp875[i]=Cyc_yylval;}};_tmp875;});
int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmp871=
10000;struct Cyc_Yyltype*_tmp872=(struct Cyc_Yyltype*)_cycalloc_atomic(
_check_times(sizeof(struct Cyc_Yyltype),_tmp871));{unsigned int _tmp873=_tmp871;
unsigned int i;for(i=0;i < _tmp873;i ++){_tmp872[i]=Cyc_yynewloc();}};_tmp872;});
int yystacksize=10000;void*yyval=Cyc_yylval;int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=
0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[
_check_known_subscript_notnull(10000,++ yyssp_offset)]=(short)yystate;if(
yyssp_offset >= yystacksize - 1){if(yystacksize >= 10000){Cyc_yyerror(_tag_arr("parser stack overflow",
sizeof(unsigned char),22));(int)_throw((void*)Cyc_Yystack_overflow);}}goto
yybackup;yybackup: yyn=(int)Cyc_yypact[yystate];if(yyn == - 32768){goto yydefault;}
if(Cyc_yychar == - 2){Cyc_yychar=Cyc_yylex();}if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=
0;}else{yychar1=(Cyc_yychar > 0? Cyc_yychar <= 352: 0)?(int)Cyc_yytranslate[
_check_known_subscript_notnull(353,Cyc_yychar)]: 237;}yyn +=yychar1;if((yyn < 0? 1:
yyn > 4995)? 1: Cyc_yycheck[yyn]!= yychar1){goto yydefault;}yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){if(yyn == - 32768){goto yyerrlab;}yyn=- yyn;goto yyreduce;}else{if(yyn == 0){
goto yyerrlab;}}if(yyn == 830){return 0;}if(Cyc_yychar != 0){Cyc_yychar=- 2;}yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0){yyerrstatus --;}yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
yystate];if(yyn == 0){goto yyerrlab;}yyreduce: yylen=(int)Cyc_yyr2[yyn];if(yylen > 0){
yyval=yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];}
switch(yyn){case 1: _LL28F: yyval=yyvs[yyvsp_offset];Cyc_Parse_parse_result=Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]);break;case 2: _LL290: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp31B=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));_tmp31B[0]=({struct Cyc_DeclList_tok_struct
_tmp31C;_tmp31C.tag=Cyc_DeclList_tok;_tmp31C.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]));_tmp31C;});_tmp31B;});break;case 3: _LL291: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp31D=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));
_tmp31D[0]=({struct Cyc_DeclList_tok_struct _tmp31E;_tmp31E.tag=Cyc_DeclList_tok;
_tmp31E.f1=({struct Cyc_List_List*_tmp31F=_cycalloc(sizeof(struct Cyc_List_List));
_tmp31F->hd=({struct Cyc_Absyn_Decl*_tmp320=_cycalloc(sizeof(struct Cyc_Absyn_Decl));
_tmp320->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp321=_cycalloc(
sizeof(struct Cyc_Absyn_Using_d_struct));_tmp321[0]=({struct Cyc_Absyn_Using_d_struct
_tmp322;_tmp322.tag=10;_tmp322.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp322.f2=Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]);_tmp322;});_tmp321;}));_tmp320->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp320;});_tmp31F->tl=0;_tmp31F;});_tmp31E;});_tmp31D;});
Cyc_Lex_leave_using();break;case 4: _LL292: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp323=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));_tmp323[0]=({struct Cyc_DeclList_tok_struct
_tmp324;_tmp324.tag=Cyc_DeclList_tok;_tmp324.f1=({struct Cyc_List_List*_tmp325=
_cycalloc(sizeof(struct Cyc_List_List));_tmp325->hd=({struct Cyc_Absyn_Decl*
_tmp326=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_tmp326->r=(void*)((void*)({
struct Cyc_Absyn_Using_d_struct*_tmp327=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_struct));
_tmp327[0]=({struct Cyc_Absyn_Using_d_struct _tmp328;_tmp328.tag=10;_tmp328.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp328.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp328;});_tmp327;}));
_tmp326->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp326;});_tmp325->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp325;});_tmp324;});_tmp323;});break;case 5: _LL293: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp329=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));
_tmp329[0]=({struct Cyc_DeclList_tok_struct _tmp32A;_tmp32A.tag=Cyc_DeclList_tok;
_tmp32A.f1=({struct Cyc_List_List*_tmp32B=_cycalloc(sizeof(struct Cyc_List_List));
_tmp32B->hd=({struct Cyc_Absyn_Decl*_tmp32C=_cycalloc(sizeof(struct Cyc_Absyn_Decl));
_tmp32C->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp32D=_cycalloc(
sizeof(struct Cyc_Absyn_Namespace_d_struct));_tmp32D[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp32E;_tmp32E.tag=9;_tmp32E.f1=({struct _tagged_arr*_tmp32F=_cycalloc(sizeof(
struct _tagged_arr));_tmp32F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp32F;});_tmp32E.f2=
Cyc_yyget_DeclList_tok(yyvs[yyvsp_offset]);_tmp32E;});_tmp32D;}));_tmp32C->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line);_tmp32C;});_tmp32B->tl=
0;_tmp32B;});_tmp32A;});_tmp329;});Cyc_Lex_leave_namespace();break;case 6: _LL294:
yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp330=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));
_tmp330[0]=({struct Cyc_DeclList_tok_struct _tmp331;_tmp331.tag=Cyc_DeclList_tok;
_tmp331.f1=({struct Cyc_List_List*_tmp332=_cycalloc(sizeof(struct Cyc_List_List));
_tmp332->hd=({struct Cyc_Absyn_Decl*_tmp333=_cycalloc(sizeof(struct Cyc_Absyn_Decl));
_tmp333->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp334=_cycalloc(
sizeof(struct Cyc_Absyn_Namespace_d_struct));_tmp334[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp335;_tmp335.tag=9;_tmp335.f1=({struct _tagged_arr*_tmp336=_cycalloc(sizeof(
struct _tagged_arr));_tmp336[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp336;});_tmp335.f2=
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp335;});_tmp334;}));_tmp333->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp333;});
_tmp332->tl=Cyc_yyget_DeclList_tok(yyvs[yyvsp_offset]);_tmp332;});_tmp331;});
_tmp330;});break;case 7: _LL295: if(Cyc_Std_strcmp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tag_arr("C",sizeof(
unsigned char),2))!= 0){Cyc_Parse_err(_tag_arr("only extern \"C\" { ... } is allowed",
sizeof(unsigned char),35),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));}yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp337=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));
_tmp337[0]=({struct Cyc_DeclList_tok_struct _tmp338;_tmp338.tag=Cyc_DeclList_tok;
_tmp338.f1=({struct Cyc_List_List*_tmp339=_cycalloc(sizeof(struct Cyc_List_List));
_tmp339->hd=({struct Cyc_Absyn_Decl*_tmp33A=_cycalloc(sizeof(struct Cyc_Absyn_Decl));
_tmp33A->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*_tmp33B=_cycalloc(
sizeof(struct Cyc_Absyn_ExternC_d_struct));_tmp33B[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp33C;_tmp33C.tag=11;_tmp33C.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp33C;});_tmp33B;}));
_tmp33A->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp33A;});_tmp339->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp339;});_tmp338;});_tmp337;});break;case 8: _LL296: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp33D=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));
_tmp33D[0]=({struct Cyc_DeclList_tok_struct _tmp33E;_tmp33E.tag=Cyc_DeclList_tok;
_tmp33E.f1=0;_tmp33E;});_tmp33D;});break;case 9: _LL297: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp33F=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));_tmp33F[0]=({struct Cyc_DeclList_tok_struct
_tmp340;_tmp340.tag=Cyc_DeclList_tok;_tmp340.f1=({struct Cyc_List_List*_tmp341=
_cycalloc(sizeof(struct Cyc_List_List));_tmp341->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Fn_d_struct*_tmp342=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_struct));
_tmp342[0]=({struct Cyc_Absyn_Fn_d_struct _tmp343;_tmp343.tag=1;_tmp343.f1=Cyc_yyget_FnDecl_tok(
yyvs[yyvsp_offset]);_tmp343;});_tmp342;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp341->tl=0;_tmp341;});
_tmp340;});_tmp33F;});break;case 10: _LL298: yyval=yyvs[yyvsp_offset];break;case 11:
_LL299: yyval=(void*)({struct Cyc_FnDecl_tok_struct*_tmp344=_cycalloc(sizeof(
struct Cyc_FnDecl_tok_struct));_tmp344[0]=({struct Cyc_FnDecl_tok_struct _tmp345;
_tmp345.tag=Cyc_FnDecl_tok;_tmp345.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp345;});_tmp344;});break;case 12: _LL29A: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp346=_cycalloc(sizeof(struct Cyc_FnDecl_tok_struct));
_tmp346[0]=({struct Cyc_FnDecl_tok_struct _tmp347;_tmp347.tag=Cyc_FnDecl_tok;
_tmp347.f1=Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp348=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp348->v=Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp348;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp347;});_tmp346;});break;case 13: _LL29B: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp349=_cycalloc(sizeof(struct Cyc_FnDecl_tok_struct));
_tmp349[0]=({struct Cyc_FnDecl_tok_struct _tmp34A;_tmp34A.tag=Cyc_FnDecl_tok;
_tmp34A.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp34A;});_tmp349;});break;case 14: _LL29C: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp34B=_cycalloc(sizeof(struct Cyc_FnDecl_tok_struct));
_tmp34B[0]=({struct Cyc_FnDecl_tok_struct _tmp34C;_tmp34C.tag=Cyc_FnDecl_tok;
_tmp34C.f1=Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp34D=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp34D->v=Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp34D;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp34C;});_tmp34B;});break;case 15: _LL29D: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp34E=_cycalloc(sizeof(struct Cyc_FnDecl_tok_struct));
_tmp34E[0]=({struct Cyc_FnDecl_tok_struct _tmp34F;_tmp34F.tag=Cyc_FnDecl_tok;
_tmp34F.f1=Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp350=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp350->v=Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp350;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp34F;});_tmp34E;});break;case 16: _LL29E: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp351=_cycalloc(sizeof(struct Cyc_FnDecl_tok_struct));
_tmp351[0]=({struct Cyc_FnDecl_tok_struct _tmp352;_tmp352.tag=Cyc_FnDecl_tok;
_tmp352.f1=Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp353=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp353->v=Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp353;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp352;});_tmp351;});break;case 17: _LL29F: Cyc_Lex_enter_using(
Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]));yyval=yyvs[yyvsp_offset];break;case 18:
_LL2A0: Cyc_Lex_leave_using();break;case 19: _LL2A1: Cyc_Lex_enter_namespace(({
struct _tagged_arr*_tmp354=_cycalloc(sizeof(struct _tagged_arr));_tmp354[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp354;}));yyval=yyvs[yyvsp_offset];break;case 20: _LL2A2: Cyc_Lex_leave_namespace();
break;case 21: _LL2A3: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp355=
_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));_tmp355[0]=({struct Cyc_DeclList_tok_struct
_tmp356;_tmp356.tag=Cyc_DeclList_tok;_tmp356.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp356;});
_tmp355;});break;case 22: _LL2A4: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp357=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));_tmp357[0]=({struct Cyc_DeclList_tok_struct
_tmp358;_tmp358.tag=Cyc_DeclList_tok;_tmp358.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp358;});_tmp357;});break;case 23: _LL2A5: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp359=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));
_tmp359[0]=({struct Cyc_DeclList_tok_struct _tmp35A;_tmp35A.tag=Cyc_DeclList_tok;
_tmp35A.f1=({struct Cyc_List_List*_tmp35B=_cycalloc(sizeof(struct Cyc_List_List));
_tmp35B->hd=Cyc_Absyn_let_decl(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp35B->tl=0;_tmp35B;});_tmp35A;});_tmp359;});break;
case 24: _LL2A6: {struct Cyc_List_List*_tmp35C=0;{struct Cyc_List_List*_tmp35D=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp35D != 0;
_tmp35D=_tmp35D->tl){struct _tagged_arr*_tmp35E=(struct _tagged_arr*)_tmp35D->hd;
struct _tuple1*_tmp35F=({struct _tuple1*_tmp362=_cycalloc(sizeof(struct _tuple1));
_tmp362->f1=Cyc_Absyn_rel_ns_null;_tmp362->f2=_tmp35E;_tmp362;});struct Cyc_Absyn_Vardecl*
_tmp360=Cyc_Absyn_new_vardecl(_tmp35F,Cyc_Absyn_wildtyp(0),0);_tmp35C=({struct
Cyc_List_List*_tmp361=_cycalloc(sizeof(struct Cyc_List_List));_tmp361->hd=_tmp360;
_tmp361->tl=_tmp35C;_tmp361;});}}_tmp35C=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp35C);yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp363=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));_tmp363[0]=({struct Cyc_DeclList_tok_struct
_tmp364;_tmp364.tag=Cyc_DeclList_tok;_tmp364.f1=({struct Cyc_List_List*_tmp365=
_cycalloc(sizeof(struct Cyc_List_List));_tmp365->hd=Cyc_Absyn_letv_decl(_tmp35C,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp365->tl=0;
_tmp365;});_tmp364;});_tmp363;});break;}case 25: _LL2A7: yyval=yyvs[yyvsp_offset];
break;case 26: _LL2A8: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp366=
_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));_tmp366[0]=({struct Cyc_DeclList_tok_struct
_tmp367;_tmp367.tag=Cyc_DeclList_tok;_tmp367.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]));_tmp367;});_tmp366;});break;case 27: _LL2A9: yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp368=_cycalloc(sizeof(struct Cyc_DeclSpec_tok_struct));
_tmp368[0]=({struct Cyc_DeclSpec_tok_struct _tmp369;_tmp369.tag=Cyc_DeclSpec_tok;
_tmp369.f1=({struct Cyc_Parse_Declaration_spec*_tmp36A=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));
_tmp36A->sc=({struct Cyc_Core_Opt*_tmp36B=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp36B->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp36B;});_tmp36A->tq=Cyc_Absyn_empty_tqual();_tmp36A->type_specs=
0;_tmp36A->is_inline=0;_tmp36A->attributes=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp36A;});_tmp369;});_tmp368;});break;case 28: _LL2AA: if((Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->sc != 0){({void*_tmp36C[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tag_arr("Only one storage class is allowed in a declaration",
sizeof(unsigned char),51),_tag_arr(_tmp36C,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp36D=_cycalloc(sizeof(struct Cyc_DeclSpec_tok_struct));
_tmp36D[0]=({struct Cyc_DeclSpec_tok_struct _tmp36E;_tmp36E.tag=Cyc_DeclSpec_tok;
_tmp36E.f1=({struct Cyc_Parse_Declaration_spec*_tmp36F=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));
_tmp36F->sc=({struct Cyc_Core_Opt*_tmp370=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp370->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp370;});_tmp36F->tq=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->tq;_tmp36F->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->type_specs;_tmp36F->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->is_inline;_tmp36F->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp36F;});_tmp36E;});_tmp36D;});break;case 29:
_LL2AB: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp371=_cycalloc(sizeof(
struct Cyc_DeclSpec_tok_struct));_tmp371[0]=({struct Cyc_DeclSpec_tok_struct
_tmp372;_tmp372.tag=Cyc_DeclSpec_tok;_tmp372.f1=({struct Cyc_Parse_Declaration_spec*
_tmp373=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));_tmp373->sc=0;
_tmp373->tq=Cyc_Absyn_empty_tqual();_tmp373->type_specs=({struct Cyc_List_List*
_tmp374=_cycalloc(sizeof(struct Cyc_List_List));_tmp374->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp374->tl=0;
_tmp374;});_tmp373->is_inline=0;_tmp373->attributes=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp373;});_tmp372;});_tmp371;});break;case 30: _LL2AC: yyval=(
void*)({struct Cyc_DeclSpec_tok_struct*_tmp375=_cycalloc(sizeof(struct Cyc_DeclSpec_tok_struct));
_tmp375[0]=({struct Cyc_DeclSpec_tok_struct _tmp376;_tmp376.tag=Cyc_DeclSpec_tok;
_tmp376.f1=({struct Cyc_Parse_Declaration_spec*_tmp377=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));
_tmp377->sc=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->sc;_tmp377->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->tq;_tmp377->type_specs=({struct Cyc_List_List*_tmp378=
_cycalloc(sizeof(struct Cyc_List_List));_tmp378->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp378->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp378;});_tmp377->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->is_inline;_tmp377->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp377;});_tmp376;});_tmp375;});break;case 31:
_LL2AD: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp379=_cycalloc(sizeof(
struct Cyc_DeclSpec_tok_struct));_tmp379[0]=({struct Cyc_DeclSpec_tok_struct
_tmp37A;_tmp37A.tag=Cyc_DeclSpec_tok;_tmp37A.f1=({struct Cyc_Parse_Declaration_spec*
_tmp37B=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));_tmp37B->sc=0;
_tmp37B->tq=Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp37B->type_specs=0;_tmp37B->is_inline=0;_tmp37B->attributes=
Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp37B;});_tmp37A;});_tmp379;});
break;case 32: _LL2AE: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp37C=
_cycalloc(sizeof(struct Cyc_DeclSpec_tok_struct));_tmp37C[0]=({struct Cyc_DeclSpec_tok_struct
_tmp37D;_tmp37D.tag=Cyc_DeclSpec_tok;_tmp37D.f1=({struct Cyc_Parse_Declaration_spec*
_tmp37E=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));_tmp37E->sc=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->sc;_tmp37E->tq=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->tq);_tmp37E->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->type_specs;_tmp37E->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->is_inline;_tmp37E->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp37E;});_tmp37D;});_tmp37C;});break;case 33:
_LL2AF: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp37F=_cycalloc(sizeof(
struct Cyc_DeclSpec_tok_struct));_tmp37F[0]=({struct Cyc_DeclSpec_tok_struct
_tmp380;_tmp380.tag=Cyc_DeclSpec_tok;_tmp380.f1=({struct Cyc_Parse_Declaration_spec*
_tmp381=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));_tmp381->sc=0;
_tmp381->tq=Cyc_Absyn_empty_tqual();_tmp381->type_specs=0;_tmp381->is_inline=1;
_tmp381->attributes=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp381;});
_tmp380;});_tmp37F;});break;case 34: _LL2B0: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp382=_cycalloc(sizeof(struct Cyc_DeclSpec_tok_struct));_tmp382[0]=({struct Cyc_DeclSpec_tok_struct
_tmp383;_tmp383.tag=Cyc_DeclSpec_tok;_tmp383.f1=({struct Cyc_Parse_Declaration_spec*
_tmp384=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));_tmp384->sc=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->sc;_tmp384->tq=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq;
_tmp384->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->type_specs;
_tmp384->is_inline=1;_tmp384->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp384;});_tmp383;});_tmp382;});break;case 35:
_LL2B1: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp385=_cycalloc(
sizeof(struct Cyc_StorageClass_tok_struct));_tmp385[0]=({struct Cyc_StorageClass_tok_struct
_tmp386;_tmp386.tag=Cyc_StorageClass_tok;_tmp386.f1=(void*)((void*)4);_tmp386;});
_tmp385;});break;case 36: _LL2B2: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp387=_cycalloc(sizeof(struct Cyc_StorageClass_tok_struct));_tmp387[0]=({struct
Cyc_StorageClass_tok_struct _tmp388;_tmp388.tag=Cyc_StorageClass_tok;_tmp388.f1=(
void*)((void*)5);_tmp388;});_tmp387;});break;case 37: _LL2B3: yyval=(void*)({struct
Cyc_StorageClass_tok_struct*_tmp389=_cycalloc(sizeof(struct Cyc_StorageClass_tok_struct));
_tmp389[0]=({struct Cyc_StorageClass_tok_struct _tmp38A;_tmp38A.tag=Cyc_StorageClass_tok;
_tmp38A.f1=(void*)((void*)3);_tmp38A;});_tmp389;});break;case 38: _LL2B4: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp38B=_cycalloc(sizeof(struct Cyc_StorageClass_tok_struct));
_tmp38B[0]=({struct Cyc_StorageClass_tok_struct _tmp38C;_tmp38C.tag=Cyc_StorageClass_tok;
_tmp38C.f1=(void*)((void*)1);_tmp38C;});_tmp38B;});break;case 39: _LL2B5: if(Cyc_Std_strcmp(
Cyc_yyget_String_tok(yyvs[yyvsp_offset]),_tag_arr("C",sizeof(unsigned char),2))
!= 0){Cyc_Parse_err(_tag_arr("only extern or extern \"C\" is allowed",sizeof(
unsigned char),37),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp38D=_cycalloc(sizeof(struct Cyc_StorageClass_tok_struct));_tmp38D[0]=({struct
Cyc_StorageClass_tok_struct _tmp38E;_tmp38E.tag=Cyc_StorageClass_tok;_tmp38E.f1=(
void*)((void*)2);_tmp38E;});_tmp38D;});break;case 40: _LL2B6: yyval=(void*)({struct
Cyc_StorageClass_tok_struct*_tmp38F=_cycalloc(sizeof(struct Cyc_StorageClass_tok_struct));
_tmp38F[0]=({struct Cyc_StorageClass_tok_struct _tmp390;_tmp390.tag=Cyc_StorageClass_tok;
_tmp390.f1=(void*)((void*)0);_tmp390;});_tmp38F;});break;case 41: _LL2B7: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp391=_cycalloc(sizeof(struct Cyc_StorageClass_tok_struct));
_tmp391[0]=({struct Cyc_StorageClass_tok_struct _tmp392;_tmp392.tag=Cyc_StorageClass_tok;
_tmp392.f1=(void*)((void*)6);_tmp392;});_tmp391;});break;case 42: _LL2B8: yyval=(
void*)({struct Cyc_AttributeList_tok_struct*_tmp393=_cycalloc(sizeof(struct Cyc_AttributeList_tok_struct));
_tmp393[0]=({struct Cyc_AttributeList_tok_struct _tmp394;_tmp394.tag=Cyc_AttributeList_tok;
_tmp394.f1=0;_tmp394;});_tmp393;});break;case 43: _LL2B9: yyval=yyvs[yyvsp_offset];
break;case 44: _LL2BA: yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp395=
_cycalloc(sizeof(struct Cyc_AttributeList_tok_struct));_tmp395[0]=({struct Cyc_AttributeList_tok_struct
_tmp396;_tmp396.tag=Cyc_AttributeList_tok;_tmp396.f1=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp396;});_tmp395;});
break;case 45: _LL2BB: yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp397=
_cycalloc(sizeof(struct Cyc_AttributeList_tok_struct));_tmp397[0]=({struct Cyc_AttributeList_tok_struct
_tmp398;_tmp398.tag=Cyc_AttributeList_tok;_tmp398.f1=({struct Cyc_List_List*
_tmp399=_cycalloc(sizeof(struct Cyc_List_List));_tmp399->hd=(void*)Cyc_yyget_Attribute_tok(
yyvs[yyvsp_offset]);_tmp399->tl=0;_tmp399;});_tmp398;});_tmp397;});break;case 46:
_LL2BC: yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp39A=_cycalloc(
sizeof(struct Cyc_AttributeList_tok_struct));_tmp39A[0]=({struct Cyc_AttributeList_tok_struct
_tmp39B;_tmp39B.tag=Cyc_AttributeList_tok;_tmp39B.f1=({struct Cyc_List_List*
_tmp39C=_cycalloc(sizeof(struct Cyc_List_List));_tmp39C->hd=(void*)Cyc_yyget_Attribute_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp39C->tl=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp39C;});_tmp39B;});_tmp39A;});break;case 47: _LL2BD: {
struct _tagged_arr _tmp39D=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);if((((
_get_arr_size(_tmp39D,sizeof(unsigned char))> 4?*((const unsigned char*)
_check_unknown_subscript(_tmp39D,sizeof(unsigned char),0))== '_': 0)?*((const
unsigned char*)_check_unknown_subscript(_tmp39D,sizeof(unsigned char),1))== '_':
0)?*((const unsigned char*)_check_unknown_subscript(_tmp39D,sizeof(unsigned char),(
int)(_get_arr_size(_tmp39D,sizeof(unsigned char))- 2)))== '_': 0)?*((const
unsigned char*)_check_unknown_subscript(_tmp39D,sizeof(unsigned char),(int)(
_get_arr_size(_tmp39D,sizeof(unsigned char))- 3)))== '_': 0){_tmp39D=(struct
_tagged_arr)Cyc_Std_substring(_tmp39D,2,_get_arr_size(_tmp39D,sizeof(
unsigned char))- 5);}{int i=0;for(0;i < 16;++ i){if(Cyc_Std_strcmp(_tmp39D,(Cyc_Parse_att_map[
i]).f1)== 0){yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp39E=_cycalloc(
sizeof(struct Cyc_Attribute_tok_struct));_tmp39E[0]=({struct Cyc_Attribute_tok_struct
_tmp39F;_tmp39F.tag=Cyc_Attribute_tok;_tmp39F.f1=(void*)(Cyc_Parse_att_map[i]).f2;
_tmp39F;});_tmp39E;});break;}}if(i == 16){Cyc_Parse_err(_tag_arr("unrecognized attribute",
sizeof(unsigned char),23),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Attribute_tok_struct*
_tmp3A0=_cycalloc(sizeof(struct Cyc_Attribute_tok_struct));_tmp3A0[0]=({struct Cyc_Attribute_tok_struct
_tmp3A1;_tmp3A1.tag=Cyc_Attribute_tok;_tmp3A1.f1=(void*)((void*)1);_tmp3A1;});
_tmp3A0;});}break;}}case 48: _LL2BE: yyval=(void*)({struct Cyc_Attribute_tok_struct*
_tmp3A2=_cycalloc(sizeof(struct Cyc_Attribute_tok_struct));_tmp3A2[0]=({struct Cyc_Attribute_tok_struct
_tmp3A3;_tmp3A3.tag=Cyc_Attribute_tok;_tmp3A3.f1=(void*)((void*)4);_tmp3A3;});
_tmp3A2;});break;case 49: _LL2BF: {struct _tagged_arr _tmp3A4=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct _tuple15
_tmp3A6;int _tmp3A7;struct _tuple15*_tmp3A5=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3A6=*_tmp3A5;_LL2C2:
_tmp3A7=_tmp3A6.f2;goto _LL2C1;_LL2C1: {void*a;if(Cyc_Std_zstrcmp(_tmp3A4,
_tag_arr("regparm",sizeof(unsigned char),8))== 0? 1: Cyc_Std_zstrcmp(_tmp3A4,
_tag_arr("__regparm__",sizeof(unsigned char),12))== 0){if(_tmp3A7 < 0? 1: _tmp3A7 > 
3){Cyc_Parse_err(_tag_arr("regparm requires value between 0 and 3",sizeof(
unsigned char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp3A8=_cycalloc_atomic(sizeof(struct Cyc_Absyn_Regparm_att_struct));
_tmp3A8[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp3A9;_tmp3A9.tag=0;_tmp3A9.f1=
_tmp3A7;_tmp3A9;});_tmp3A8;});}else{if(Cyc_Std_zstrcmp(_tmp3A4,_tag_arr("aligned",
sizeof(unsigned char),8))== 0? 1: Cyc_Std_zstrcmp(_tmp3A4,_tag_arr("__aligned__",
sizeof(unsigned char),12))== 0){if(_tmp3A7 < 0){Cyc_Parse_err(_tag_arr("aligned requires positive power of two",
sizeof(unsigned char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp3A7;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
Cyc_Parse_err(_tag_arr("aligned requires positive power of two",sizeof(
unsigned char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp3AA=_cycalloc_atomic(sizeof(struct Cyc_Absyn_Aligned_att_struct));
_tmp3AA[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp3AB;_tmp3AB.tag=1;_tmp3AB.f1=
_tmp3A7;_tmp3AB;});_tmp3AA;});}}else{Cyc_Parse_err(_tag_arr("unrecognized attribute",
sizeof(unsigned char),23),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3AC=_cycalloc(sizeof(struct Cyc_Attribute_tok_struct));
_tmp3AC[0]=({struct Cyc_Attribute_tok_struct _tmp3AD;_tmp3AD.tag=Cyc_Attribute_tok;
_tmp3AD.f1=(void*)a;_tmp3AD;});_tmp3AC;});break;}}case 50: _LL2C0: {struct
_tagged_arr _tmp3AE=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp3AF=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_Std_zstrcmp(
_tmp3AE,_tag_arr("section",sizeof(unsigned char),8))== 0? 1: Cyc_Std_zstrcmp(
_tmp3AE,_tag_arr("__section__",sizeof(unsigned char),12))== 0){a=(void*)({struct
Cyc_Absyn_Section_att_struct*_tmp3B0=_cycalloc(sizeof(struct Cyc_Absyn_Section_att_struct));
_tmp3B0[0]=({struct Cyc_Absyn_Section_att_struct _tmp3B1;_tmp3B1.tag=2;_tmp3B1.f1=
_tmp3AF;_tmp3B1;});_tmp3B0;});}else{Cyc_Parse_err(_tag_arr("unrecognized attribute",
sizeof(unsigned char),23),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3B2=_cycalloc(sizeof(struct Cyc_Attribute_tok_struct));
_tmp3B2[0]=({struct Cyc_Attribute_tok_struct _tmp3B3;_tmp3B3.tag=Cyc_Attribute_tok;
_tmp3B3.f1=(void*)a;_tmp3B3;});_tmp3B2;});break;}case 51: _LL2C3: {struct
_tagged_arr _tmp3B4=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp3B5=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp3B7;struct
_tuple15 _tmp3B6=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_LL2C6: _tmp3B7=_tmp3B6.f2;goto _LL2C5;_LL2C5: {int _tmp3B9;
struct _tuple15 _tmp3B8=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_LL2C8: _tmp3B9=_tmp3B8.f2;goto _LL2C7;_LL2C7: {void*a=(
void*)1;if(Cyc_Std_zstrcmp(_tmp3B4,_tag_arr("format",sizeof(unsigned char),7))== 
0? 1: Cyc_Std_zstrcmp(_tmp3B4,_tag_arr("__format__",sizeof(unsigned char),11))== 0){
if(Cyc_Std_zstrcmp(_tmp3B5,_tag_arr("printf",sizeof(unsigned char),7))== 0){a=(
void*)({struct Cyc_Absyn_Format_att_struct*_tmp3BA=_cycalloc(sizeof(struct Cyc_Absyn_Format_att_struct));
_tmp3BA[0]=({struct Cyc_Absyn_Format_att_struct _tmp3BB;_tmp3BB.tag=3;_tmp3BB.f1=(
void*)((void*)0);_tmp3BB.f2=_tmp3B7;_tmp3BB.f3=_tmp3B9;_tmp3BB;});_tmp3BA;});}
else{if(Cyc_Std_zstrcmp(_tmp3B5,_tag_arr("scanf",sizeof(unsigned char),6))== 0){
a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp3BC=_cycalloc(sizeof(struct Cyc_Absyn_Format_att_struct));
_tmp3BC[0]=({struct Cyc_Absyn_Format_att_struct _tmp3BD;_tmp3BD.tag=3;_tmp3BD.f1=(
void*)((void*)1);_tmp3BD.f2=_tmp3B7;_tmp3BD.f3=_tmp3B9;_tmp3BD;});_tmp3BC;});}
else{Cyc_Parse_err(_tag_arr("unrecognized format type",sizeof(unsigned char),25),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(_tag_arr("unrecognized attribute",
sizeof(unsigned char),23),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Attribute_tok_struct*
_tmp3BE=_cycalloc(sizeof(struct Cyc_Attribute_tok_struct));_tmp3BE[0]=({struct Cyc_Attribute_tok_struct
_tmp3BF;_tmp3BF.tag=Cyc_Attribute_tok;_tmp3BF.f1=(void*)a;_tmp3BF;});_tmp3BE;});
break;}}}case 52: _LL2C4: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3C0=
_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp3C0[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3C1;_tmp3C1.tag=Cyc_TypeSpecifier_tok;_tmp3C1.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3C1;});_tmp3C0;});break;case 53: _LL2C9: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3C2=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));
_tmp3C2[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3C3;_tmp3C3.tag=Cyc_TypeSpecifier_tok;
_tmp3C3.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3C3;});
_tmp3C2;});break;case 54: _LL2CA: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3C4=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp3C4[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3C5;_tmp3C5.tag=Cyc_TypeSpecifier_tok;
_tmp3C5.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_uchar_t,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3C5;});
_tmp3C4;});break;case 55: _LL2CB: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3C6=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp3C6[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3C7;_tmp3C7.tag=Cyc_TypeSpecifier_tok;
_tmp3C7.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*_tmp3C8=_cycalloc(
sizeof(struct Cyc_Parse_Short_spec_struct));_tmp3C8[0]=({struct Cyc_Parse_Short_spec_struct
_tmp3C9;_tmp3C9.tag=2;_tmp3C9.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp3C9;});_tmp3C8;}));_tmp3C7;});_tmp3C6;});
break;case 56: _LL2CC: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3CA=
_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp3CA[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3CB;_tmp3CB.tag=Cyc_TypeSpecifier_tok;_tmp3CB.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_t,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3CB;});_tmp3CA;});break;case 57: _LL2CD: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3CC=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));
_tmp3CC[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3CD;_tmp3CD.tag=Cyc_TypeSpecifier_tok;
_tmp3CD.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*_tmp3CE=_cycalloc(
sizeof(struct Cyc_Parse_Long_spec_struct));_tmp3CE[0]=({struct Cyc_Parse_Long_spec_struct
_tmp3CF;_tmp3CF.tag=3;_tmp3CF.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp3CF;});_tmp3CE;}));_tmp3CD;});_tmp3CC;});
break;case 58: _LL2CE: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3D0=
_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp3D0[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3D1;_tmp3D1.tag=Cyc_TypeSpecifier_tok;_tmp3D1.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_t,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3D1;});_tmp3D0;});break;case 59: _LL2CF: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3D2=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));
_tmp3D2[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3D3;_tmp3D3.tag=Cyc_TypeSpecifier_tok;
_tmp3D3.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_t(0),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3D3;});
_tmp3D2;});break;case 60: _LL2D0: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3D4=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp3D4[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3D5;_tmp3D5.tag=Cyc_TypeSpecifier_tok;
_tmp3D5.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*_tmp3D6=_cycalloc(
sizeof(struct Cyc_Parse_Signed_spec_struct));_tmp3D6[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp3D7;_tmp3D7.tag=0;_tmp3D7.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp3D7;});_tmp3D6;}));_tmp3D5;});_tmp3D4;});
break;case 61: _LL2D1: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3D8=
_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp3D8[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3D9;_tmp3D9.tag=Cyc_TypeSpecifier_tok;_tmp3D9.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp3DA=_cycalloc(sizeof(struct Cyc_Parse_Unsigned_spec_struct));_tmp3DA[0]=({
struct Cyc_Parse_Unsigned_spec_struct _tmp3DB;_tmp3DB.tag=1;_tmp3DB.f1=Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);_tmp3DB;});
_tmp3DA;}));_tmp3D9;});_tmp3D8;});break;case 62: _LL2D2: yyval=yyvs[yyvsp_offset];
break;case 63: _LL2D3: yyval=yyvs[yyvsp_offset];break;case 64: _LL2D4: yyval=yyvs[
yyvsp_offset];break;case 65: _LL2D5: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3DC=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp3DC[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3DD;_tmp3DD.tag=Cyc_TypeSpecifier_tok;
_tmp3DD.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3DD;});
_tmp3DC;});break;case 66: _LL2D6: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3DE=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp3DE[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3DF;_tmp3DF.tag=Cyc_TypeSpecifier_tok;
_tmp3DF.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp3E0=_cycalloc(sizeof(struct Cyc_Absyn_TypedefType_struct));_tmp3E0[0]=({
struct Cyc_Absyn_TypedefType_struct _tmp3E1;_tmp3E1.tag=18;_tmp3E1.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3E1.f2=Cyc_yyget_TypeList_tok(
yyvs[yyvsp_offset]);_tmp3E1.f3=0;_tmp3E1;});_tmp3E0;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3DF;});_tmp3DE;});break;case 67: _LL2D7: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3E2=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));
_tmp3E2[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3E3;_tmp3E3.tag=Cyc_TypeSpecifier_tok;
_tmp3E3.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TupleType_struct*
_tmp3E4=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_struct));_tmp3E4[0]=({struct
Cyc_Absyn_TupleType_struct _tmp3E5;_tmp3E5.tag=9;_tmp3E5.f1=((struct Cyc_List_List*(*)(
struct _tuple11*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp3E5;});_tmp3E4;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3E3;});
_tmp3E2;});break;case 68: _LL2D8: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3E6=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp3E6[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3E7;_tmp3E7.tag=Cyc_TypeSpecifier_tok;
_tmp3E7.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp3E8=_cycalloc(sizeof(struct Cyc_Absyn_RgnHandleType_struct));_tmp3E8[0]=({
struct Cyc_Absyn_RgnHandleType_struct _tmp3E9;_tmp3E9.tag=17;_tmp3E9.f1=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3E9;});_tmp3E8;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3E7;});
_tmp3E6;});break;case 69: _LL2D9: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3EA=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp3EA[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3EB;_tmp3EB.tag=Cyc_TypeSpecifier_tok;
_tmp3EB.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp3EC=_cycalloc(sizeof(struct Cyc_Absyn_SizeofType_struct));_tmp3EC[0]=({struct
Cyc_Absyn_SizeofType_struct _tmp3ED;_tmp3ED.tag=13;_tmp3ED.f1=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3ED;});_tmp3EC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3EB;});
_tmp3EA;});break;case 70: _LL2DA: yyval=(void*)({struct Cyc_Kind_tok_struct*_tmp3EE=
_cycalloc(sizeof(struct Cyc_Kind_tok_struct));_tmp3EE[0]=({struct Cyc_Kind_tok_struct
_tmp3EF;_tmp3EF.tag=Cyc_Kind_tok;_tmp3EF.f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3EF;});_tmp3EE;});break;case 71: _LL2DB: {
struct _tagged_arr*_tmp3F1;void*_tmp3F2;struct _tuple1 _tmp3F0=*Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_LL2DF: _tmp3F2=_tmp3F0.f1;goto _LL2DE;_LL2DE: _tmp3F1=_tmp3F0.f2;
goto _LL2DD;_LL2DD: if(_tmp3F2 != (void*)0){Cyc_Parse_err(_tag_arr("bad kind in type specifier",
sizeof(unsigned char),27),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Kind_tok_struct*
_tmp3F3=_cycalloc(sizeof(struct Cyc_Kind_tok_struct));_tmp3F3[0]=({struct Cyc_Kind_tok_struct
_tmp3F4;_tmp3F4.tag=Cyc_Kind_tok;_tmp3F4.f1=(void*)Cyc_Parse_id_to_kind(*_tmp3F1,
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmp3F4;});_tmp3F3;});break;}case 72: _LL2DC: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp3F5=_cycalloc_atomic(sizeof(struct Cyc_TypeQual_tok_struct));_tmp3F5[0]=({
struct Cyc_TypeQual_tok_struct _tmp3F6;_tmp3F6.tag=Cyc_TypeQual_tok;_tmp3F6.f1=({
struct Cyc_Absyn_Tqual _tmp3F7;_tmp3F7.q_const=1;_tmp3F7.q_volatile=0;_tmp3F7.q_restrict=
0;_tmp3F7;});_tmp3F6;});_tmp3F5;});break;case 73: _LL2E0: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp3F8=_cycalloc_atomic(sizeof(struct Cyc_TypeQual_tok_struct));_tmp3F8[0]=({
struct Cyc_TypeQual_tok_struct _tmp3F9;_tmp3F9.tag=Cyc_TypeQual_tok;_tmp3F9.f1=({
struct Cyc_Absyn_Tqual _tmp3FA;_tmp3FA.q_const=0;_tmp3FA.q_volatile=1;_tmp3FA.q_restrict=
0;_tmp3FA;});_tmp3F9;});_tmp3F8;});break;case 74: _LL2E1: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp3FB=_cycalloc_atomic(sizeof(struct Cyc_TypeQual_tok_struct));_tmp3FB[0]=({
struct Cyc_TypeQual_tok_struct _tmp3FC;_tmp3FC.tag=Cyc_TypeQual_tok;_tmp3FC.f1=({
struct Cyc_Absyn_Tqual _tmp3FD;_tmp3FD.q_const=0;_tmp3FD.q_volatile=0;_tmp3FD.q_restrict=
1;_tmp3FD;});_tmp3FC;});_tmp3FB;});break;case 75: _LL2E2: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3FE=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp3FE[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3FF;_tmp3FF.tag=Cyc_TypeSpecifier_tok;
_tmp3FF.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp400=_cycalloc(
sizeof(struct Cyc_Parse_Decl_spec_struct));_tmp400[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp401;_tmp401.tag=5;_tmp401.f1=({struct Cyc_Absyn_Decl*_tmp402=_cycalloc(
sizeof(struct Cyc_Absyn_Decl));_tmp402->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp403=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_struct));_tmp403[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp404;_tmp404.tag=7;_tmp404.f1=({struct Cyc_Absyn_Enumdecl*_tmp405=_cycalloc(
sizeof(struct Cyc_Absyn_Enumdecl));_tmp405->sc=(void*)((void*)2);_tmp405->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp405->fields=({struct Cyc_Core_Opt*_tmp406=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp406->v=Cyc_yyget_EnumfieldList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp406;});_tmp405;});_tmp404;});_tmp403;}));_tmp402->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp402;});_tmp401;});_tmp400;}));_tmp3FF;});_tmp3FE;});
break;case 76: _LL2E3: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp407=
_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp407[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp408;_tmp408.tag=Cyc_TypeSpecifier_tok;_tmp408.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp409=_cycalloc(sizeof(struct Cyc_Absyn_EnumType_struct));
_tmp409[0]=({struct Cyc_Absyn_EnumType_struct _tmp40A;_tmp40A.tag=12;_tmp40A.f1=
Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);_tmp40A.f2=0;_tmp40A;});_tmp409;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp408;});_tmp407;});break;case 77: _LL2E4: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp40B=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));
_tmp40B[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp40C;_tmp40C.tag=Cyc_TypeSpecifier_tok;
_tmp40C.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp40D=_cycalloc(
sizeof(struct Cyc_Parse_Type_spec_struct));_tmp40D[0]=({struct Cyc_Parse_Type_spec_struct
_tmp40E;_tmp40E.tag=4;_tmp40E.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmp40F=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnumType_struct));_tmp40F[0]=({
struct Cyc_Absyn_AnonEnumType_struct _tmp410;_tmp410.tag=16;_tmp410.f1=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp410;});_tmp40F;}));
_tmp40E.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line);_tmp40E;});
_tmp40D;}));_tmp40C;});_tmp40B;});break;case 78: _LL2E5: yyval=(void*)({struct Cyc_Enumfield_tok_struct*
_tmp411=_cycalloc(sizeof(struct Cyc_Enumfield_tok_struct));_tmp411[0]=({struct Cyc_Enumfield_tok_struct
_tmp412;_tmp412.tag=Cyc_Enumfield_tok;_tmp412.f1=({struct Cyc_Absyn_Enumfield*
_tmp413=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));_tmp413->name=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp413->tag=0;_tmp413->loc=Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);_tmp413;});
_tmp412;});_tmp411;});break;case 79: _LL2E6: yyval=(void*)({struct Cyc_Enumfield_tok_struct*
_tmp414=_cycalloc(sizeof(struct Cyc_Enumfield_tok_struct));_tmp414[0]=({struct Cyc_Enumfield_tok_struct
_tmp415;_tmp415.tag=Cyc_Enumfield_tok;_tmp415.f1=({struct Cyc_Absyn_Enumfield*
_tmp416=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));_tmp416->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp416->tag=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp416->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp416;});_tmp415;});_tmp414;});break;case 80: _LL2E7:
yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*_tmp417=_cycalloc(sizeof(
struct Cyc_EnumfieldList_tok_struct));_tmp417[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp418;_tmp418.tag=Cyc_EnumfieldList_tok;_tmp418.f1=({struct Cyc_List_List*
_tmp419=_cycalloc(sizeof(struct Cyc_List_List));_tmp419->hd=Cyc_yyget_Enumfield_tok(
yyvs[yyvsp_offset]);_tmp419->tl=0;_tmp419;});_tmp418;});_tmp417;});break;case 81:
_LL2E8: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*_tmp41A=_cycalloc(
sizeof(struct Cyc_EnumfieldList_tok_struct));_tmp41A[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp41B;_tmp41B.tag=Cyc_EnumfieldList_tok;_tmp41B.f1=({struct Cyc_List_List*
_tmp41C=_cycalloc(sizeof(struct Cyc_List_List));_tmp41C->hd=Cyc_yyget_Enumfield_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp41C->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[yyvsp_offset]);_tmp41C;});_tmp41B;});_tmp41A;});break;case 82: _LL2E9: {void*
t;switch(Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)])){case Cyc_Parse_Struct_su: _LL2EB: t=(void*)({struct Cyc_Absyn_AnonStructType_struct*
_tmp41D=_cycalloc(sizeof(struct Cyc_Absyn_AnonStructType_struct));_tmp41D[0]=({
struct Cyc_Absyn_AnonStructType_struct _tmp41E;_tmp41E.tag=14;_tmp41E.f1=Cyc_yyget_StructFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp41E;});_tmp41D;});
break;case Cyc_Parse_Union_su: _LL2EC: t=(void*)({struct Cyc_Absyn_AnonUnionType_struct*
_tmp41F=_cycalloc(sizeof(struct Cyc_Absyn_AnonUnionType_struct));_tmp41F[0]=({
struct Cyc_Absyn_AnonUnionType_struct _tmp420;_tmp420.tag=15;_tmp420.f1=Cyc_yyget_StructFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp420;});_tmp41F;});
break;}yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp421=_cycalloc(
sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp421[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp422;_tmp422.tag=Cyc_TypeSpecifier_tok;_tmp422.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*
_tmp423=_cycalloc(sizeof(struct Cyc_Parse_Type_spec_struct));_tmp423[0]=({struct
Cyc_Parse_Type_spec_struct _tmp424;_tmp424.tag=4;_tmp424.f1=(void*)t;_tmp424.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line);_tmp424;});_tmp423;}));
_tmp422;});_tmp421;});break;}case 83: _LL2EA: {struct Cyc_List_List*_tmp425=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));struct Cyc_Absyn_Decl*d;
switch(Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)])){case Cyc_Parse_Struct_su: _LL2EF: d=Cyc_Absyn_struct_decl((void*)
2,({struct Cyc_Core_Opt*_tmp426=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp426->v=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp426;}),_tmp425,({struct Cyc_Core_Opt*_tmp427=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp427->v=Cyc_yyget_StructFieldDeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp427;}),0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));break;case Cyc_Parse_Union_su: _LL2F0: d=Cyc_Absyn_union_decl((
void*)2,({struct Cyc_Core_Opt*_tmp428=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp428->v=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp428;}),_tmp425,({struct Cyc_Core_Opt*_tmp429=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp429->v=Cyc_yyget_StructFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp429;}),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));break;}yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp42A=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp42A[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp42B;_tmp42B.tag=Cyc_TypeSpecifier_tok;
_tmp42B.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp42C=_cycalloc(
sizeof(struct Cyc_Parse_Decl_spec_struct));_tmp42C[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp42D;_tmp42D.tag=5;_tmp42D.f1=d;_tmp42D;});_tmp42C;}));_tmp42B;});_tmp42A;});
break;}case 84: _LL2EE: {struct Cyc_List_List*_tmp42E=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));struct Cyc_Absyn_Decl*
d;switch(Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)])){case Cyc_Parse_Struct_su: _LL2F3: d=Cyc_Absyn_struct_decl((void*)
2,({struct Cyc_Core_Opt*_tmp42F=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp42F->v=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp42F;}),_tmp42E,({struct Cyc_Core_Opt*_tmp430=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp430->v=Cyc_yyget_StructFieldDeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp430;}),0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));break;case Cyc_Parse_Union_su: _LL2F4: d=Cyc_Absyn_union_decl((
void*)2,({struct Cyc_Core_Opt*_tmp431=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp431->v=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp431;}),_tmp42E,({struct Cyc_Core_Opt*_tmp432=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp432->v=Cyc_yyget_StructFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp432;}),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));break;}yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp433=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp433[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp434;_tmp434.tag=Cyc_TypeSpecifier_tok;
_tmp434.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp435=_cycalloc(
sizeof(struct Cyc_Parse_Decl_spec_struct));_tmp435[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp436;_tmp436.tag=5;_tmp436.f1=d;_tmp436;});_tmp435;}));_tmp434;});_tmp433;});
break;}case 85: _LL2F2: switch(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])){case Cyc_Parse_Struct_su:
_LL2F7: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp437=_cycalloc(
sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp437[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp438;_tmp438.tag=Cyc_TypeSpecifier_tok;_tmp438.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_StructType_struct*_tmp439=_cycalloc(sizeof(struct Cyc_Absyn_StructType_struct));
_tmp439[0]=({struct Cyc_Absyn_StructType_struct _tmp43A;_tmp43A.tag=10;_tmp43A.f1=(
struct _tuple1*)Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp43A.f2=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp43A.f3=
0;_tmp43A;});_tmp439;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp438;});_tmp437;});break;case Cyc_Parse_Union_su:
_LL2F8: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp43B=_cycalloc(
sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp43B[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp43C;_tmp43C.tag=Cyc_TypeSpecifier_tok;_tmp43C.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_UnionType_struct*_tmp43D=_cycalloc(sizeof(struct Cyc_Absyn_UnionType_struct));
_tmp43D[0]=({struct Cyc_Absyn_UnionType_struct _tmp43E;_tmp43E.tag=11;_tmp43E.f1=(
struct _tuple1*)Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp43E.f2=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp43E.f3=
0;_tmp43E;});_tmp43D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp43C;});_tmp43B;});break;}break;case 86: _LL2F6:
switch(Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])){case Cyc_Parse_Struct_su: _LL2FB: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp43F=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp43F[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp440;_tmp440.tag=Cyc_TypeSpecifier_tok;
_tmp440.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_StructType_struct*
_tmp441=_cycalloc(sizeof(struct Cyc_Absyn_StructType_struct));_tmp441[0]=({struct
Cyc_Absyn_StructType_struct _tmp442;_tmp442.tag=10;_tmp442.f1=(struct _tuple1*)Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp442.f2=Cyc_yyget_TypeList_tok(
yyvs[yyvsp_offset]);_tmp442.f3=0;_tmp442;});_tmp441;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp440;});_tmp43F;});break;case Cyc_Parse_Union_su:
_LL2FC: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp443=_cycalloc(
sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp443[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp444;_tmp444.tag=Cyc_TypeSpecifier_tok;_tmp444.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_UnionType_struct*_tmp445=_cycalloc(sizeof(struct Cyc_Absyn_UnionType_struct));
_tmp445[0]=({struct Cyc_Absyn_UnionType_struct _tmp446;_tmp446.tag=11;_tmp446.f1=(
struct _tuple1*)Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp446.f2=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp446.f3=
0;_tmp446;});_tmp445;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp444;});_tmp443;});break;}break;case 87: _LL2FA:
yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp447=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));
_tmp447[0]=({struct Cyc_TypeList_tok_struct _tmp448;_tmp448.tag=Cyc_TypeList_tok;
_tmp448.f1=0;_tmp448;});_tmp447;});break;case 88: _LL2FE: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp449=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));_tmp449[0]=({struct Cyc_TypeList_tok_struct
_tmp44A;_tmp44A.tag=Cyc_TypeList_tok;_tmp44A.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp44A;});_tmp449;});
break;case 89: _LL2FF:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp44B=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));_tmp44B[0]=({struct Cyc_TypeList_tok_struct
_tmp44C;_tmp44C.tag=Cyc_TypeList_tok;_tmp44C.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp44C;});_tmp44B;});
break;case 90: _LL300: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp44D=
_cycalloc_atomic(sizeof(struct Cyc_StructOrUnion_tok_struct));_tmp44D[0]=({struct
Cyc_StructOrUnion_tok_struct _tmp44E;_tmp44E.tag=Cyc_StructOrUnion_tok;_tmp44E.f1=
Cyc_Parse_Struct_su;_tmp44E;});_tmp44D;});break;case 91: _LL301: yyval=(void*)({
struct Cyc_StructOrUnion_tok_struct*_tmp44F=_cycalloc_atomic(sizeof(struct Cyc_StructOrUnion_tok_struct));
_tmp44F[0]=({struct Cyc_StructOrUnion_tok_struct _tmp450;_tmp450.tag=Cyc_StructOrUnion_tok;
_tmp450.f1=Cyc_Parse_Union_su;_tmp450;});_tmp44F;});break;case 92: _LL302: yyval=(
void*)({struct Cyc_StructFieldDeclList_tok_struct*_tmp451=_cycalloc(sizeof(struct
Cyc_StructFieldDeclList_tok_struct));_tmp451[0]=({struct Cyc_StructFieldDeclList_tok_struct
_tmp452;_tmp452.tag=Cyc_StructFieldDeclList_tok;_tmp452.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_StructFieldDeclListList_tok(yyvs[yyvsp_offset])));
_tmp452;});_tmp451;});break;case 93: _LL303: yyval=(void*)({struct Cyc_StructFieldDeclListList_tok_struct*
_tmp453=_cycalloc(sizeof(struct Cyc_StructFieldDeclListList_tok_struct));_tmp453[
0]=({struct Cyc_StructFieldDeclListList_tok_struct _tmp454;_tmp454.tag=Cyc_StructFieldDeclListList_tok;
_tmp454.f1=({struct Cyc_List_List*_tmp455=_cycalloc(sizeof(struct Cyc_List_List));
_tmp455->hd=Cyc_yyget_StructFieldDeclList_tok(yyvs[yyvsp_offset]);_tmp455->tl=0;
_tmp455;});_tmp454;});_tmp453;});break;case 94: _LL304: yyval=(void*)({struct Cyc_StructFieldDeclListList_tok_struct*
_tmp456=_cycalloc(sizeof(struct Cyc_StructFieldDeclListList_tok_struct));_tmp456[
0]=({struct Cyc_StructFieldDeclListList_tok_struct _tmp457;_tmp457.tag=Cyc_StructFieldDeclListList_tok;
_tmp457.f1=({struct Cyc_List_List*_tmp458=_cycalloc(sizeof(struct Cyc_List_List));
_tmp458->hd=Cyc_yyget_StructFieldDeclList_tok(yyvs[yyvsp_offset]);_tmp458->tl=
Cyc_yyget_StructFieldDeclListList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp458;});_tmp457;});_tmp456;});break;case 95: _LL305: yyval=(
void*)({struct Cyc_InitDeclList_tok_struct*_tmp459=_cycalloc(sizeof(struct Cyc_InitDeclList_tok_struct));
_tmp459[0]=({struct Cyc_InitDeclList_tok_struct _tmp45A;_tmp45A.tag=Cyc_InitDeclList_tok;
_tmp45A.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok(yyvs[yyvsp_offset]));_tmp45A;});_tmp459;});break;case
96: _LL306: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp45B=_cycalloc(
sizeof(struct Cyc_InitDeclList_tok_struct));_tmp45B[0]=({struct Cyc_InitDeclList_tok_struct
_tmp45C;_tmp45C.tag=Cyc_InitDeclList_tok;_tmp45C.f1=({struct Cyc_List_List*
_tmp45D=_cycalloc(sizeof(struct Cyc_List_List));_tmp45D->hd=Cyc_yyget_InitDecl_tok(
yyvs[yyvsp_offset]);_tmp45D->tl=0;_tmp45D;});_tmp45C;});_tmp45B;});break;case 97:
_LL307: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp45E=_cycalloc(
sizeof(struct Cyc_InitDeclList_tok_struct));_tmp45E[0]=({struct Cyc_InitDeclList_tok_struct
_tmp45F;_tmp45F.tag=Cyc_InitDeclList_tok;_tmp45F.f1=({struct Cyc_List_List*
_tmp460=_cycalloc(sizeof(struct Cyc_List_List));_tmp460->hd=Cyc_yyget_InitDecl_tok(
yyvs[yyvsp_offset]);_tmp460->tl=Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp460;});_tmp45F;});
_tmp45E;});break;case 98: _LL308: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp461=_cycalloc(sizeof(struct Cyc_InitDecl_tok_struct));_tmp461[0]=({struct Cyc_InitDecl_tok_struct
_tmp462;_tmp462.tag=Cyc_InitDecl_tok;_tmp462.f1=({struct _tuple14*_tmp463=
_cycalloc(sizeof(struct _tuple14));_tmp463->f1=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp463->f2=0;_tmp463;});_tmp462;});_tmp461;});break;case 99:
_LL309: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp464=_cycalloc(sizeof(
struct Cyc_InitDecl_tok_struct));_tmp464[0]=({struct Cyc_InitDecl_tok_struct
_tmp465;_tmp465.tag=Cyc_InitDecl_tok;_tmp465.f1=({struct _tuple14*_tmp466=
_cycalloc(sizeof(struct _tuple14));_tmp466->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp466->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp466;});_tmp465;});_tmp464;});break;
case 100: _LL30A: {struct _tuple17 _tmp468;struct Cyc_List_List*_tmp469;struct Cyc_List_List*
_tmp46A;struct Cyc_Absyn_Tqual _tmp46B;struct _tuple17*_tmp467=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp468=*_tmp467;
_LL30F: _tmp46B=_tmp468.f1;goto _LL30E;_LL30E: _tmp46A=_tmp468.f2;goto _LL30D;_LL30D:
_tmp469=_tmp468.f3;goto _LL30C;_LL30C: {struct Cyc_List_List*_tmp46D;struct Cyc_List_List*
_tmp46E;struct _tuple0 _tmp46C=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
Cyc_yyget_InitDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_LL312: _tmp46E=_tmp46C.f1;goto _LL311;_LL311: _tmp46D=_tmp46C.f2;
goto _LL310;_LL310: {void*_tmp46F=Cyc_Parse_speclist2typ(_tmp46A,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp470=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(Cyc_Parse_apply_tmss(_tmp46B,_tmp46F,_tmp46E,_tmp469),_tmp46D);
yyval=(void*)({struct Cyc_StructFieldDeclList_tok_struct*_tmp471=_cycalloc(
sizeof(struct Cyc_StructFieldDeclList_tok_struct));_tmp471[0]=({struct Cyc_StructFieldDeclList_tok_struct
_tmp472;_tmp472.tag=Cyc_StructFieldDeclList_tok;_tmp472.f1=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(*f)(struct Cyc_Position_Segment*,struct _tuple9*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmp470);_tmp472;});_tmp471;});break;}}}case 101:
_LL30B: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp473=_cycalloc(
sizeof(struct Cyc_QualSpecList_tok_struct));_tmp473[0]=({struct Cyc_QualSpecList_tok_struct
_tmp474;_tmp474.tag=Cyc_QualSpecList_tok;_tmp474.f1=({struct _tuple17*_tmp475=
_cycalloc(sizeof(struct _tuple17));_tmp475->f1=Cyc_Absyn_empty_tqual();_tmp475->f2=({
struct Cyc_List_List*_tmp476=_cycalloc(sizeof(struct Cyc_List_List));_tmp476->hd=(
void*)Cyc_yyget_TypeSpecifier_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp476->tl=0;_tmp476;});_tmp475->f3=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp475;});_tmp474;});_tmp473;});break;case 102: _LL313: yyval=(
void*)({struct Cyc_QualSpecList_tok_struct*_tmp477=_cycalloc(sizeof(struct Cyc_QualSpecList_tok_struct));
_tmp477[0]=({struct Cyc_QualSpecList_tok_struct _tmp478;_tmp478.tag=Cyc_QualSpecList_tok;
_tmp478.f1=({struct _tuple17*_tmp479=_cycalloc(sizeof(struct _tuple17));_tmp479->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f1;_tmp479->f2=({struct Cyc_List_List*
_tmp47A=_cycalloc(sizeof(struct Cyc_List_List));_tmp47A->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp47A->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f2;_tmp47A;});_tmp479->f3=Cyc_List_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f3);_tmp479;});_tmp478;});_tmp477;});break;case 103: _LL314:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp47B=_cycalloc(sizeof(struct
Cyc_QualSpecList_tok_struct));_tmp47B[0]=({struct Cyc_QualSpecList_tok_struct
_tmp47C;_tmp47C.tag=Cyc_QualSpecList_tok;_tmp47C.f1=({struct _tuple17*_tmp47D=
_cycalloc(sizeof(struct _tuple17));_tmp47D->f1=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp47D->f2=0;_tmp47D->f3=
Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp47D;});_tmp47C;});_tmp47B;});
break;case 104: _LL315: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp47E=
_cycalloc(sizeof(struct Cyc_QualSpecList_tok_struct));_tmp47E[0]=({struct Cyc_QualSpecList_tok_struct
_tmp47F;_tmp47F.tag=Cyc_QualSpecList_tok;_tmp47F.f1=({struct _tuple17*_tmp480=
_cycalloc(sizeof(struct _tuple17));_tmp480->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f1);_tmp480->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
yyvsp_offset])).f2;_tmp480->f3=Cyc_List_append(Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f3);_tmp480;});_tmp47F;});_tmp47E;});break;case 105: _LL316:
yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp481=_cycalloc(sizeof(struct
Cyc_InitDeclList_tok_struct));_tmp481[0]=({struct Cyc_InitDeclList_tok_struct
_tmp482;_tmp482.tag=Cyc_InitDeclList_tok;_tmp482.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
yyvsp_offset]));_tmp482;});_tmp481;});break;case 106: _LL317: yyval=(void*)({struct
Cyc_InitDeclList_tok_struct*_tmp483=_cycalloc(sizeof(struct Cyc_InitDeclList_tok_struct));
_tmp483[0]=({struct Cyc_InitDeclList_tok_struct _tmp484;_tmp484.tag=Cyc_InitDeclList_tok;
_tmp484.f1=({struct Cyc_List_List*_tmp485=_cycalloc(sizeof(struct Cyc_List_List));
_tmp485->hd=Cyc_yyget_InitDecl_tok(yyvs[yyvsp_offset]);_tmp485->tl=0;_tmp485;});
_tmp484;});_tmp483;});break;case 107: _LL318: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp486=_cycalloc(sizeof(struct Cyc_InitDeclList_tok_struct));_tmp486[0]=({struct
Cyc_InitDeclList_tok_struct _tmp487;_tmp487.tag=Cyc_InitDeclList_tok;_tmp487.f1=({
struct Cyc_List_List*_tmp488=_cycalloc(sizeof(struct Cyc_List_List));_tmp488->hd=
Cyc_yyget_InitDecl_tok(yyvs[yyvsp_offset]);_tmp488->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp488;});_tmp487;});
_tmp486;});break;case 108: _LL319: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp489=_cycalloc(sizeof(struct Cyc_InitDecl_tok_struct));_tmp489[0]=({struct Cyc_InitDecl_tok_struct
_tmp48A;_tmp48A.tag=Cyc_InitDecl_tok;_tmp48A.f1=({struct _tuple14*_tmp48B=
_cycalloc(sizeof(struct _tuple14));_tmp48B->f1=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp48B->f2=0;_tmp48B;});_tmp48A;});_tmp489;});break;case 109:
_LL31A: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp48C=_cycalloc(sizeof(
struct Cyc_InitDecl_tok_struct));_tmp48C[0]=({struct Cyc_InitDecl_tok_struct
_tmp48D;_tmp48D.tag=Cyc_InitDecl_tok;_tmp48D.f1=({struct _tuple14*_tmp48E=
_cycalloc(sizeof(struct _tuple14));_tmp48E->f1=({struct Cyc_Parse_Declarator*
_tmp48F=_cycalloc(sizeof(struct Cyc_Parse_Declarator));_tmp48F->id=({struct
_tuple1*_tmp490=_cycalloc(sizeof(struct _tuple1));_tmp490->f1=Cyc_Absyn_rel_ns_null;
_tmp490->f2=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(
unsigned char),1);_tmp490;});_tmp48F->tms=0;_tmp48F;});_tmp48E->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp48E;});_tmp48D;});_tmp48C;});break;
case 110: _LL31B: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp491=_cycalloc(
sizeof(struct Cyc_InitDecl_tok_struct));_tmp491[0]=({struct Cyc_InitDecl_tok_struct
_tmp492;_tmp492.tag=Cyc_InitDecl_tok;_tmp492.f1=({struct _tuple14*_tmp493=
_cycalloc(sizeof(struct _tuple14));_tmp493->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp493->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp493;});_tmp492;});_tmp491;});break;
case 111: _LL31C: {struct Cyc_List_List*_tmp494=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp495=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));
_tmp495[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp496;_tmp496.tag=Cyc_TypeSpecifier_tok;
_tmp496.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp497=_cycalloc(
sizeof(struct Cyc_Parse_Decl_spec_struct));_tmp497[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp498;_tmp498.tag=5;_tmp498.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp494,({struct Cyc_Core_Opt*
_tmp499=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp499->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp499;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp498;});_tmp497;}));_tmp496;});_tmp495;});break;}
case 112: _LL31D: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp49A=
_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp49A[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp49B;_tmp49B.tag=Cyc_TypeSpecifier_tok;_tmp49B.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp49C=_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));
_tmp49C[0]=({struct Cyc_Absyn_TunionType_struct _tmp49D;_tmp49D.tag=2;_tmp49D.f1=({
struct Cyc_Absyn_TunionInfo _tmp49E;_tmp49E.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp49F=_cycalloc(sizeof(struct Cyc_Absyn_UnknownTunion_struct));_tmp49F[0]=({
struct Cyc_Absyn_UnknownTunion_struct _tmp4A0;_tmp4A0.tag=0;_tmp4A0.f1=({struct Cyc_Absyn_UnknownTunionInfo
_tmp4A1;_tmp4A1.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp4A1.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4A1;});_tmp4A0;});
_tmp49F;}));_tmp49E.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp49E.rgn=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp49E;});_tmp49D;});_tmp49C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp49B;});_tmp49A;});break;case 113: _LL31E: {void*
_tmp4A2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp4AB=_cycalloc(sizeof(struct
Cyc_Core_Opt));_tmp4AB->v=(void*)((void*)3);_tmp4AB;}),0);yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp4A3=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));
_tmp4A3[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4A4;_tmp4A4.tag=Cyc_TypeSpecifier_tok;
_tmp4A4.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionType_struct*
_tmp4A5=_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));_tmp4A5[0]=({struct
Cyc_Absyn_TunionType_struct _tmp4A6;_tmp4A6.tag=2;_tmp4A6.f1=({struct Cyc_Absyn_TunionInfo
_tmp4A7;_tmp4A7.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp4A8=_cycalloc(sizeof(struct Cyc_Absyn_UnknownTunion_struct));_tmp4A8[0]=({
struct Cyc_Absyn_UnknownTunion_struct _tmp4A9;_tmp4A9.tag=0;_tmp4A9.f1=({struct Cyc_Absyn_UnknownTunionInfo
_tmp4AA;_tmp4AA.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp4AA.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4AA;});_tmp4A9;});
_tmp4A8;}));_tmp4A7.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp4A7.rgn=(
void*)_tmp4A2;_tmp4A7;});_tmp4A6;});_tmp4A5;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp4A4;});_tmp4A3;});break;}case 114: _LL31F: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4AC=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));
_tmp4AC[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4AD;_tmp4AD.tag=Cyc_TypeSpecifier_tok;
_tmp4AD.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp4AE=_cycalloc(sizeof(struct Cyc_Absyn_TunionFieldType_struct));_tmp4AE[0]=({
struct Cyc_Absyn_TunionFieldType_struct _tmp4AF;_tmp4AF.tag=3;_tmp4AF.f1=({struct
Cyc_Absyn_TunionFieldInfo _tmp4B0;_tmp4B0.field_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*
_tmp4B1=_cycalloc(sizeof(struct Cyc_Absyn_UnknownTunionfield_struct));_tmp4B1[0]=({
struct Cyc_Absyn_UnknownTunionfield_struct _tmp4B2;_tmp4B2.tag=0;_tmp4B2.f1=({
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp4B3;_tmp4B3.tunion_name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4B3.field_name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4B3.is_xtunion=Cyc_yyget_Bool_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp4B3;});_tmp4B2;});_tmp4B1;}));_tmp4B0.targs=Cyc_yyget_TypeList_tok(
yyvs[yyvsp_offset]);_tmp4B0;});_tmp4AF;});_tmp4AE;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp4AD;});_tmp4AC;});break;case 115: _LL320: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp4B4=_cycalloc_atomic(sizeof(struct Cyc_Bool_tok_struct));
_tmp4B4[0]=({struct Cyc_Bool_tok_struct _tmp4B5;_tmp4B5.tag=Cyc_Bool_tok;_tmp4B5.f1=
0;_tmp4B5;});_tmp4B4;});break;case 116: _LL321: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp4B6=_cycalloc_atomic(sizeof(struct Cyc_Bool_tok_struct));_tmp4B6[0]=({struct
Cyc_Bool_tok_struct _tmp4B7;_tmp4B7.tag=Cyc_Bool_tok;_tmp4B7.f1=1;_tmp4B7;});
_tmp4B6;});break;case 117: _LL322: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp4B8=_cycalloc(sizeof(struct Cyc_TunionFieldList_tok_struct));_tmp4B8[0]=({
struct Cyc_TunionFieldList_tok_struct _tmp4B9;_tmp4B9.tag=Cyc_TunionFieldList_tok;
_tmp4B9.f1=({struct Cyc_List_List*_tmp4BA=_cycalloc(sizeof(struct Cyc_List_List));
_tmp4BA->hd=Cyc_yyget_TunionField_tok(yyvs[yyvsp_offset]);_tmp4BA->tl=0;_tmp4BA;});
_tmp4B9;});_tmp4B8;});break;case 118: _LL323: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp4BB=_cycalloc(sizeof(struct Cyc_TunionFieldList_tok_struct));_tmp4BB[0]=({
struct Cyc_TunionFieldList_tok_struct _tmp4BC;_tmp4BC.tag=Cyc_TunionFieldList_tok;
_tmp4BC.f1=({struct Cyc_List_List*_tmp4BD=_cycalloc(sizeof(struct Cyc_List_List));
_tmp4BD->hd=Cyc_yyget_TunionField_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp4BD->tl=0;_tmp4BD;});_tmp4BC;});_tmp4BB;});break;case 119:
_LL324: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp4BE=_cycalloc(
sizeof(struct Cyc_TunionFieldList_tok_struct));_tmp4BE[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp4BF;_tmp4BF.tag=Cyc_TunionFieldList_tok;_tmp4BF.f1=({struct Cyc_List_List*
_tmp4C0=_cycalloc(sizeof(struct Cyc_List_List));_tmp4C0->hd=Cyc_yyget_TunionField_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4C0->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[yyvsp_offset]);_tmp4C0;});_tmp4BF;});_tmp4BE;});break;case 120: _LL325: yyval=(
void*)({struct Cyc_TunionFieldList_tok_struct*_tmp4C1=_cycalloc(sizeof(struct Cyc_TunionFieldList_tok_struct));
_tmp4C1[0]=({struct Cyc_TunionFieldList_tok_struct _tmp4C2;_tmp4C2.tag=Cyc_TunionFieldList_tok;
_tmp4C2.f1=({struct Cyc_List_List*_tmp4C3=_cycalloc(sizeof(struct Cyc_List_List));
_tmp4C3->hd=Cyc_yyget_TunionField_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp4C3->tl=Cyc_yyget_TunionFieldList_tok(yyvs[yyvsp_offset]);
_tmp4C3;});_tmp4C2;});_tmp4C1;});break;case 121: _LL326: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4C4=_cycalloc(sizeof(struct Cyc_Scope_tok_struct));_tmp4C4[0]=({struct Cyc_Scope_tok_struct
_tmp4C5;_tmp4C5.tag=Cyc_Scope_tok;_tmp4C5.f1=(void*)((void*)2);_tmp4C5;});
_tmp4C4;});break;case 122: _LL327: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4C6=_cycalloc(sizeof(struct Cyc_Scope_tok_struct));_tmp4C6[0]=({struct Cyc_Scope_tok_struct
_tmp4C7;_tmp4C7.tag=Cyc_Scope_tok;_tmp4C7.f1=(void*)((void*)3);_tmp4C7;});
_tmp4C6;});break;case 123: _LL328: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4C8=_cycalloc(sizeof(struct Cyc_Scope_tok_struct));_tmp4C8[0]=({struct Cyc_Scope_tok_struct
_tmp4C9;_tmp4C9.tag=Cyc_Scope_tok;_tmp4C9.f1=(void*)((void*)0);_tmp4C9;});
_tmp4C8;});break;case 124: _LL329: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp4CA=_cycalloc(sizeof(struct Cyc_TunionField_tok_struct));_tmp4CA[0]=({struct
Cyc_TunionField_tok_struct _tmp4CB;_tmp4CB.tag=Cyc_TunionField_tok;_tmp4CB.f1=({
struct Cyc_Absyn_Tunionfield*_tmp4CC=_cycalloc(sizeof(struct Cyc_Absyn_Tunionfield));
_tmp4CC->name=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);_tmp4CC->tvs=0;_tmp4CC->typs=
0;_tmp4CC->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line);_tmp4CC->sc=(
void*)Cyc_yyget_Scope_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]);_tmp4CC;});_tmp4CB;});_tmp4CA;});break;case 125: _LL32A: {struct Cyc_List_List*
_tmp4CD=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Position_Segment*,
struct _tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));struct Cyc_List_List*
_tmp4CE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp4CF=_cycalloc(sizeof(struct Cyc_TunionField_tok_struct));
_tmp4CF[0]=({struct Cyc_TunionField_tok_struct _tmp4D0;_tmp4D0.tag=Cyc_TunionField_tok;
_tmp4D0.f1=({struct Cyc_Absyn_Tunionfield*_tmp4D1=_cycalloc(sizeof(struct Cyc_Absyn_Tunionfield));
_tmp4D1->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp4D1->tvs=_tmp4CE;_tmp4D1->typs=_tmp4CD;_tmp4D1->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4D1->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp4D1;});_tmp4D0;});
_tmp4CF;});break;}case 126: _LL32B: yyval=yyvs[yyvsp_offset];break;case 127: _LL32C:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4D2=_cycalloc(sizeof(struct
Cyc_Declarator_tok_struct));_tmp4D2[0]=({struct Cyc_Declarator_tok_struct _tmp4D3;
_tmp4D3.tag=Cyc_Declarator_tok;_tmp4D3.f1=({struct Cyc_Parse_Declarator*_tmp4D4=
_cycalloc(sizeof(struct Cyc_Parse_Declarator));_tmp4D4->id=(Cyc_yyget_Declarator_tok(
yyvs[yyvsp_offset]))->id;_tmp4D4->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[yyvsp_offset]))->tms);_tmp4D4;});_tmp4D3;});_tmp4D2;});break;case 128: _LL32D:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4D5=_cycalloc(sizeof(struct
Cyc_Declarator_tok_struct));_tmp4D5[0]=({struct Cyc_Declarator_tok_struct _tmp4D6;
_tmp4D6.tag=Cyc_Declarator_tok;_tmp4D6.f1=({struct Cyc_Parse_Declarator*_tmp4D7=
_cycalloc(sizeof(struct Cyc_Parse_Declarator));_tmp4D7->id=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp4D7->tms=0;_tmp4D7;});_tmp4D6;});_tmp4D5;});break;case
129: _LL32E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 130: _LL32F: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4D8=
_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));_tmp4D8[0]=({struct Cyc_Declarator_tok_struct
_tmp4D9;_tmp4D9.tag=Cyc_Declarator_tok;_tmp4D9.f1=({struct Cyc_Parse_Declarator*
_tmp4DA=_cycalloc(sizeof(struct Cyc_Parse_Declarator));_tmp4DA->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->id;_tmp4DA->tms=({
struct Cyc_List_List*_tmp4DB=_cycalloc(sizeof(struct Cyc_List_List));_tmp4DB->hd=(
void*)((void*)0);_tmp4DB->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->tms;_tmp4DB;});_tmp4DA;});
_tmp4D9;});_tmp4D8;});break;case 131: _LL330: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4DC=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));_tmp4DC[0]=({struct
Cyc_Declarator_tok_struct _tmp4DD;_tmp4DD.tag=Cyc_Declarator_tok;_tmp4DD.f1=({
struct Cyc_Parse_Declarator*_tmp4DE=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp4DE->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp4DE->tms=({struct Cyc_List_List*_tmp4DF=_cycalloc(
sizeof(struct Cyc_List_List));_tmp4DF->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp4E0=_cycalloc(sizeof(struct Cyc_Absyn_ConstArray_mod_struct));_tmp4E0[0]=({
struct Cyc_Absyn_ConstArray_mod_struct _tmp4E1;_tmp4E1.tag=0;_tmp4E1.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E1;});_tmp4E0;}));
_tmp4DF->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp4DF;});_tmp4DE;});_tmp4DD;});_tmp4DC;});break;case
132: _LL331: {struct _tuple16 _tmp4E3;struct Cyc_List_List*_tmp4E4;struct Cyc_Core_Opt*
_tmp4E5;struct Cyc_Absyn_VarargInfo*_tmp4E6;int _tmp4E7;struct Cyc_List_List*
_tmp4E8;struct _tuple16*_tmp4E2=Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp4E3=*_tmp4E2;_LL338: _tmp4E8=_tmp4E3.f1;goto _LL337;
_LL337: _tmp4E7=_tmp4E3.f2;goto _LL336;_LL336: _tmp4E6=_tmp4E3.f3;goto _LL335;_LL335:
_tmp4E5=_tmp4E3.f4;goto _LL334;_LL334: _tmp4E4=_tmp4E3.f5;goto _LL333;_LL333: yyval=(
void*)({struct Cyc_Declarator_tok_struct*_tmp4E9=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));
_tmp4E9[0]=({struct Cyc_Declarator_tok_struct _tmp4EA;_tmp4EA.tag=Cyc_Declarator_tok;
_tmp4EA.f1=({struct Cyc_Parse_Declarator*_tmp4EB=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp4EB->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp4EB->tms=({struct Cyc_List_List*_tmp4EC=_cycalloc(
sizeof(struct Cyc_List_List));_tmp4EC->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp4ED=_cycalloc(sizeof(struct Cyc_Absyn_Function_mod_struct));_tmp4ED[0]=({
struct Cyc_Absyn_Function_mod_struct _tmp4EE;_tmp4EE.tag=2;_tmp4EE.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp4EF=_cycalloc(sizeof(struct Cyc_Absyn_WithTypes_struct));
_tmp4EF[0]=({struct Cyc_Absyn_WithTypes_struct _tmp4F0;_tmp4F0.tag=1;_tmp4F0.f1=
_tmp4E8;_tmp4F0.f2=_tmp4E7;_tmp4F0.f3=_tmp4E6;_tmp4F0.f4=_tmp4E5;_tmp4F0.f5=
_tmp4E4;_tmp4F0;});_tmp4EF;}));_tmp4EE;});_tmp4ED;}));_tmp4EC->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp4EC;});
_tmp4EB;});_tmp4EA;});_tmp4E9;});break;}case 133: _LL332: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4F1=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));_tmp4F1[0]=({struct
Cyc_Declarator_tok_struct _tmp4F2;_tmp4F2.tag=Cyc_Declarator_tok;_tmp4F2.f1=({
struct Cyc_Parse_Declarator*_tmp4F3=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp4F3->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp4F3->tms=({struct Cyc_List_List*_tmp4F4=_cycalloc(
sizeof(struct Cyc_List_List));_tmp4F4->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp4F5=_cycalloc(sizeof(struct Cyc_Absyn_Function_mod_struct));_tmp4F5[0]=({
struct Cyc_Absyn_Function_mod_struct _tmp4F6;_tmp4F6.tag=2;_tmp4F6.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp4F7=_cycalloc(sizeof(struct Cyc_Absyn_WithTypes_struct));
_tmp4F7[0]=({struct Cyc_Absyn_WithTypes_struct _tmp4F8;_tmp4F8.tag=1;_tmp4F8.f1=0;
_tmp4F8.f2=0;_tmp4F8.f3=0;_tmp4F8.f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4F8.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4F8;});_tmp4F7;}));
_tmp4F6;});_tmp4F5;}));_tmp4F4->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp4F4;});_tmp4F3;});
_tmp4F2;});_tmp4F1;});break;case 134: _LL339: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4F9=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));_tmp4F9[0]=({struct
Cyc_Declarator_tok_struct _tmp4FA;_tmp4FA.tag=Cyc_Declarator_tok;_tmp4FA.f1=({
struct Cyc_Parse_Declarator*_tmp4FB=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp4FB->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp4FB->tms=({struct Cyc_List_List*_tmp4FC=_cycalloc(
sizeof(struct Cyc_List_List));_tmp4FC->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp4FD=_cycalloc(sizeof(struct Cyc_Absyn_Function_mod_struct));_tmp4FD[0]=({
struct Cyc_Absyn_Function_mod_struct _tmp4FE;_tmp4FE.tag=2;_tmp4FE.f1=(void*)((
void*)({struct Cyc_Absyn_NoTypes_struct*_tmp4FF=_cycalloc(sizeof(struct Cyc_Absyn_NoTypes_struct));
_tmp4FF[0]=({struct Cyc_Absyn_NoTypes_struct _tmp500;_tmp500.tag=0;_tmp500.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp500.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp500;});_tmp4FF;}));_tmp4FE;});_tmp4FD;}));_tmp4FC->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp4FC;});_tmp4FB;});_tmp4FA;});_tmp4F9;});break;case 135: _LL33A: {
struct Cyc_List_List*_tmp501=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp502=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));
_tmp502[0]=({struct Cyc_Declarator_tok_struct _tmp503;_tmp503.tag=Cyc_Declarator_tok;
_tmp503.f1=({struct Cyc_Parse_Declarator*_tmp504=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp504->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp504->tms=({struct Cyc_List_List*_tmp505=_cycalloc(
sizeof(struct Cyc_List_List));_tmp505->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp506=_cycalloc(sizeof(struct Cyc_Absyn_TypeParams_mod_struct));_tmp506[0]=({
struct Cyc_Absyn_TypeParams_mod_struct _tmp507;_tmp507.tag=3;_tmp507.f1=_tmp501;
_tmp507.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line);_tmp507.f3=0;
_tmp507;});_tmp506;}));_tmp505->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp505;});_tmp504;});
_tmp503;});_tmp502;});break;}case 136: _LL33B:((struct Cyc_Lexing_lexbuf*)((struct
Cyc_Core_Opt*)_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;{struct Cyc_List_List*
_tmp508=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp509=_cycalloc(sizeof(struct
Cyc_Declarator_tok_struct));_tmp509[0]=({struct Cyc_Declarator_tok_struct _tmp50A;
_tmp50A.tag=Cyc_Declarator_tok;_tmp50A.f1=({struct Cyc_Parse_Declarator*_tmp50B=
_cycalloc(sizeof(struct Cyc_Parse_Declarator));_tmp50B->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp50B->tms=({
struct Cyc_List_List*_tmp50C=_cycalloc(sizeof(struct Cyc_List_List));_tmp50C->hd=(
void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp50D=_cycalloc(sizeof(
struct Cyc_Absyn_TypeParams_mod_struct));_tmp50D[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp50E;_tmp50E.tag=3;_tmp50E.f1=_tmp508;_tmp50E.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp50E.f3=0;_tmp50E;});_tmp50D;}));_tmp50C->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp50C;});
_tmp50B;});_tmp50A;});_tmp509;});break;}case 137: _LL33C: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp50F=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));_tmp50F[0]=({struct
Cyc_Declarator_tok_struct _tmp510;_tmp510.tag=Cyc_Declarator_tok;_tmp510.f1=({
struct Cyc_Parse_Declarator*_tmp511=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp511->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->id;_tmp511->tms=({struct Cyc_List_List*_tmp512=_cycalloc(
sizeof(struct Cyc_List_List));_tmp512->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp513=_cycalloc(sizeof(struct Cyc_Absyn_Attributes_mod_struct));_tmp513[0]=({
struct Cyc_Absyn_Attributes_mod_struct _tmp514;_tmp514.tag=4;_tmp514.f1=Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);_tmp514.f2=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp514;});_tmp513;}));_tmp512->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp512;});
_tmp511;});_tmp510;});_tmp50F;});break;case 138: _LL33D: Cyc_Parse_err(_tag_arr("identifier has not been declared as a typedef",
sizeof(unsigned char),46),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(void*)({
struct Cyc_Declarator_tok_struct*_tmp515=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));
_tmp515[0]=({struct Cyc_Declarator_tok_struct _tmp516;_tmp516.tag=Cyc_Declarator_tok;
_tmp516.f1=({struct Cyc_Parse_Declarator*_tmp517=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp517->id=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);_tmp517->tms=0;_tmp517;});
_tmp516;});_tmp515;});break;case 139: _LL33E: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp518=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));_tmp518[0]=({struct
Cyc_Declarator_tok_struct _tmp519;_tmp519.tag=Cyc_Declarator_tok;_tmp519.f1=({
struct Cyc_Parse_Declarator*_tmp51A=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp51A->id=Cyc_Absyn_exn_name;_tmp51A->tms=0;_tmp51A;});_tmp519;});_tmp518;});
break;case 140: _LL33F: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp51B=_cycalloc(sizeof(struct Cyc_TypeModifierList_tok_struct));_tmp51B[0]=({
struct Cyc_TypeModifierList_tok_struct _tmp51C;_tmp51C.tag=Cyc_TypeModifierList_tok;
_tmp51C.f1=({struct Cyc_List_List*_tmp51D=_cycalloc(sizeof(struct Cyc_List_List));
_tmp51D->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp51E=
_cycalloc(sizeof(struct Cyc_Absyn_Pointer_mod_struct));_tmp51E[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp51F;_tmp51F.tag=1;_tmp51F.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp51F.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp51F.f3=Cyc_Absyn_empty_tqual();
_tmp51F;});_tmp51E;}));_tmp51D->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]),0);_tmp51D;});_tmp51C;});_tmp51B;});break;case 141: _LL340:
yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*_tmp520=_cycalloc(sizeof(
struct Cyc_TypeModifierList_tok_struct));_tmp520[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp521;_tmp521.tag=Cyc_TypeModifierList_tok;_tmp521.f1=({struct Cyc_List_List*
_tmp522=_cycalloc(sizeof(struct Cyc_List_List));_tmp522->hd=(void*)((void*)({
struct Cyc_Absyn_Pointer_mod_struct*_tmp523=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_mod_struct));
_tmp523[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp524;_tmp524.tag=1;_tmp524.f1=(
void*)Cyc_yyget_Pointer_Sort_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp524.f2=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp524.f3=Cyc_yyget_TypeQual_tok(
yyvs[yyvsp_offset]);_tmp524;});_tmp523;}));_tmp522->tl=Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0);_tmp522;});_tmp521;});
_tmp520;});break;case 142: _LL341: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp525=_cycalloc(sizeof(struct Cyc_TypeModifierList_tok_struct));_tmp525[0]=({
struct Cyc_TypeModifierList_tok_struct _tmp526;_tmp526.tag=Cyc_TypeModifierList_tok;
_tmp526.f1=({struct Cyc_List_List*_tmp527=_cycalloc(sizeof(struct Cyc_List_List));
_tmp527->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp528=
_cycalloc(sizeof(struct Cyc_Absyn_Pointer_mod_struct));_tmp528[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp529;_tmp529.tag=1;_tmp529.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp529.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp529.f3=Cyc_Absyn_empty_tqual();
_tmp529;});_tmp528;}));_tmp527->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp527;});_tmp526;});_tmp525;});break;case 143: _LL342: yyval=(
void*)({struct Cyc_TypeModifierList_tok_struct*_tmp52A=_cycalloc(sizeof(struct Cyc_TypeModifierList_tok_struct));
_tmp52A[0]=({struct Cyc_TypeModifierList_tok_struct _tmp52B;_tmp52B.tag=Cyc_TypeModifierList_tok;
_tmp52B.f1=({struct Cyc_List_List*_tmp52C=_cycalloc(sizeof(struct Cyc_List_List));
_tmp52C->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp52D=
_cycalloc(sizeof(struct Cyc_Absyn_Pointer_mod_struct));_tmp52D[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp52E;_tmp52E.tag=1;_tmp52E.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp52E.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp52E.f3=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp52E;});_tmp52D;}));
_tmp52C->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp52C;});_tmp52B;});_tmp52A;});break;case 144: _LL343: yyval=(
void*)({struct Cyc_Pointer_Sort_tok_struct*_tmp52F=_cycalloc(sizeof(struct Cyc_Pointer_Sort_tok_struct));
_tmp52F[0]=({struct Cyc_Pointer_Sort_tok_struct _tmp530;_tmp530.tag=Cyc_Pointer_Sort_tok;
_tmp530.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*_tmp531=_cycalloc(
sizeof(struct Cyc_Absyn_Nullable_ps_struct));_tmp531[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp532;_tmp532.tag=1;_tmp532.f1=Cyc_Absyn_exp_unsigned_one;_tmp532;});_tmp531;}));
_tmp530;});_tmp52F;});break;case 145: _LL344: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp533=_cycalloc(sizeof(struct Cyc_Pointer_Sort_tok_struct));_tmp533[0]=({struct
Cyc_Pointer_Sort_tok_struct _tmp534;_tmp534.tag=Cyc_Pointer_Sort_tok;_tmp534.f1=(
void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*_tmp535=_cycalloc(sizeof(
struct Cyc_Absyn_NonNullable_ps_struct));_tmp535[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp536;_tmp536.tag=0;_tmp536.f1=Cyc_Absyn_exp_unsigned_one;_tmp536;});_tmp535;}));
_tmp534;});_tmp533;});break;case 146: _LL345: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp537=_cycalloc(sizeof(struct Cyc_Pointer_Sort_tok_struct));_tmp537[0]=({struct
Cyc_Pointer_Sort_tok_struct _tmp538;_tmp538.tag=Cyc_Pointer_Sort_tok;_tmp538.f1=(
void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*_tmp539=_cycalloc(sizeof(
struct Cyc_Absyn_Nullable_ps_struct));_tmp539[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp53A;_tmp53A.tag=1;_tmp53A.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp53A;});_tmp539;}));
_tmp538;});_tmp537;});break;case 147: _LL346: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp53B=_cycalloc(sizeof(struct Cyc_Pointer_Sort_tok_struct));_tmp53B[0]=({struct
Cyc_Pointer_Sort_tok_struct _tmp53C;_tmp53C.tag=Cyc_Pointer_Sort_tok;_tmp53C.f1=(
void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*_tmp53D=_cycalloc(sizeof(
struct Cyc_Absyn_NonNullable_ps_struct));_tmp53D[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp53E;_tmp53E.tag=0;_tmp53E.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp53E;});_tmp53D;}));
_tmp53C;});_tmp53B;});break;case 148: _LL347: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp53F=_cycalloc(sizeof(struct Cyc_Pointer_Sort_tok_struct));_tmp53F[0]=({struct
Cyc_Pointer_Sort_tok_struct _tmp540;_tmp540.tag=Cyc_Pointer_Sort_tok;_tmp540.f1=(
void*)((void*)0);_tmp540;});_tmp53F;});break;case 149: _LL348: yyval=(void*)({
struct Cyc_Type_tok_struct*_tmp541=_cycalloc(sizeof(struct Cyc_Type_tok_struct));
_tmp541[0]=({struct Cyc_Type_tok_struct _tmp542;_tmp542.tag=Cyc_Type_tok;_tmp542.f1=(
void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp543=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp543->v=(void*)((void*)3);_tmp543;}),0);_tmp542;});_tmp541;});break;case 150:
_LL349: yyval=yyvs[yyvsp_offset];break;case 151: _LL34A: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(void*)3);yyval=yyvs[yyvsp_offset];break;
case 152: _LL34B: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp544=_cycalloc(
sizeof(struct Cyc_Type_tok_struct));_tmp544[0]=({struct Cyc_Type_tok_struct _tmp545;
_tmp545.tag=Cyc_Type_tok;_tmp545.f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp546=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp546->v=(void*)((void*)3);
_tmp546;}),0);_tmp545;});_tmp544;});break;case 153: _LL34C: yyval=yyvs[yyvsp_offset];
break;case 154: _LL34D: yyval=(void*)({struct Cyc_TypeQual_tok_struct*_tmp547=
_cycalloc_atomic(sizeof(struct Cyc_TypeQual_tok_struct));_tmp547[0]=({struct Cyc_TypeQual_tok_struct
_tmp548;_tmp548.tag=Cyc_TypeQual_tok;_tmp548.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[yyvsp_offset]));_tmp548;});_tmp547;});break;case 155: _LL34E: yyval=(void*)({
struct Cyc_YY1_struct*_tmp549=_cycalloc(sizeof(struct Cyc_YY1_struct));_tmp549[0]=({
struct Cyc_YY1_struct _tmp54A;_tmp54A.tag=Cyc_YY1;_tmp54A.f1=({struct _tuple16*
_tmp54B=_cycalloc(sizeof(struct _tuple16));_tmp54B->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp54B->f2=0;_tmp54B->f3=
0;_tmp54B->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp54B->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp54B;});_tmp54A;});_tmp549;});break;case 156: _LL34F: yyval=(void*)({struct Cyc_YY1_struct*
_tmp54C=_cycalloc(sizeof(struct Cyc_YY1_struct));_tmp54C[0]=({struct Cyc_YY1_struct
_tmp54D;_tmp54D.tag=Cyc_YY1;_tmp54D.f1=({struct _tuple16*_tmp54E=_cycalloc(
sizeof(struct _tuple16));_tmp54E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));_tmp54E->f2=1;_tmp54E->f3=
0;_tmp54E->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp54E->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp54E;});_tmp54D;});_tmp54C;});break;case 157: _LL350: {struct _tuple2 _tmp550;
void*_tmp551;struct Cyc_Absyn_Tqual _tmp552;struct Cyc_Core_Opt*_tmp553;struct
_tuple2*_tmp54F=Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp550=*_tmp54F;_LL355: _tmp553=_tmp550.f1;goto _LL354;
_LL354: _tmp552=_tmp550.f2;goto _LL353;_LL353: _tmp551=_tmp550.f3;goto _LL352;_LL352: {
struct Cyc_Absyn_VarargInfo*_tmp554=({struct Cyc_Absyn_VarargInfo*_tmp558=
_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_tmp558->name=_tmp553;_tmp558->tq=
_tmp552;_tmp558->type=(void*)_tmp551;_tmp558->inject=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp558;});yyval=(void*)({
struct Cyc_YY1_struct*_tmp555=_cycalloc(sizeof(struct Cyc_YY1_struct));_tmp555[0]=({
struct Cyc_YY1_struct _tmp556;_tmp556.tag=Cyc_YY1;_tmp556.f1=({struct _tuple16*
_tmp557=_cycalloc(sizeof(struct _tuple16));_tmp557->f1=0;_tmp557->f2=0;_tmp557->f3=
_tmp554;_tmp557->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp557->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp557;});_tmp556;});_tmp555;});break;}}case 158: _LL351: {struct _tuple2 _tmp55A;
void*_tmp55B;struct Cyc_Absyn_Tqual _tmp55C;struct Cyc_Core_Opt*_tmp55D;struct
_tuple2*_tmp559=Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp55A=*_tmp559;_LL35A: _tmp55D=_tmp55A.f1;goto _LL359;
_LL359: _tmp55C=_tmp55A.f2;goto _LL358;_LL358: _tmp55B=_tmp55A.f3;goto _LL357;_LL357: {
struct Cyc_Absyn_VarargInfo*_tmp55E=({struct Cyc_Absyn_VarargInfo*_tmp562=
_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_tmp562->name=_tmp55D;_tmp562->tq=
_tmp55C;_tmp562->type=(void*)_tmp55B;_tmp562->inject=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp562;});yyval=(void*)({
struct Cyc_YY1_struct*_tmp55F=_cycalloc(sizeof(struct Cyc_YY1_struct));_tmp55F[0]=({
struct Cyc_YY1_struct _tmp560;_tmp560.tag=Cyc_YY1;_tmp560.f1=({struct _tuple16*
_tmp561=_cycalloc(sizeof(struct _tuple16));_tmp561->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp561->f2=0;_tmp561->f3=
_tmp55E;_tmp561->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp561->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp561;});_tmp560;});_tmp55F;});break;}}case 159: _LL356: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp563=_cycalloc(sizeof(struct Cyc_Type_tok_struct));_tmp563[0]=({struct Cyc_Type_tok_struct
_tmp564;_tmp564.tag=Cyc_Type_tok;_tmp564.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp565=_cycalloc(
sizeof(struct Cyc_Absyn_Unknown_kb_struct));_tmp565[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp566;_tmp566.tag=1;_tmp566.f1=0;_tmp566;});_tmp565;}));_tmp564;});_tmp563;});
break;case 160: _LL35B: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp567=_cycalloc(
sizeof(struct Cyc_Type_tok_struct));_tmp567[0]=({struct Cyc_Type_tok_struct _tmp568;
_tmp568.tag=Cyc_Type_tok;_tmp568.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp569=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp569[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp56A;_tmp56A.tag=0;_tmp56A.f1=(void*)Cyc_yyget_Kind_tok(yyvs[yyvsp_offset]);
_tmp56A;});_tmp569;}));_tmp568;});_tmp567;});break;case 161: _LL35C: yyval=(void*)({
struct Cyc_TypeOpt_tok_struct*_tmp56B=_cycalloc(sizeof(struct Cyc_TypeOpt_tok_struct));
_tmp56B[0]=({struct Cyc_TypeOpt_tok_struct _tmp56C;_tmp56C.tag=Cyc_TypeOpt_tok;
_tmp56C.f1=0;_tmp56C;});_tmp56B;});break;case 162: _LL35D: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp56D=_cycalloc(sizeof(struct Cyc_TypeOpt_tok_struct));_tmp56D[0]=({struct Cyc_TypeOpt_tok_struct
_tmp56E;_tmp56E.tag=Cyc_TypeOpt_tok;_tmp56E.f1=({struct Cyc_Core_Opt*_tmp56F=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp56F->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp570=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp570[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp571;_tmp571.tag=20;_tmp571.f1=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);
_tmp571;});_tmp570;}));_tmp56F;});_tmp56E;});_tmp56D;});break;case 163: _LL35E:
yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp572=_cycalloc(sizeof(struct Cyc_Rgnorder_tok_struct));
_tmp572[0]=({struct Cyc_Rgnorder_tok_struct _tmp573;_tmp573.tag=Cyc_Rgnorder_tok;
_tmp573.f1=0;_tmp573;});_tmp572;});break;case 164: _LL35F: yyval=yyvs[yyvsp_offset];
break;case 165: _LL360: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp574=
_cycalloc(sizeof(struct Cyc_Rgnorder_tok_struct));_tmp574[0]=({struct Cyc_Rgnorder_tok_struct
_tmp575;_tmp575.tag=Cyc_Rgnorder_tok;_tmp575.f1=({struct Cyc_List_List*_tmp576=
_cycalloc(sizeof(struct Cyc_List_List));_tmp576->hd=({struct _tuple18*_tmp577=
_cycalloc(sizeof(struct _tuple18));_tmp577->f1=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp57A=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp57A[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp57B;_tmp57B.tag=0;_tmp57B.f1=(void*)((void*)3);_tmp57B;});_tmp57A;}));
_tmp577->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),(void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp578=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));
_tmp578[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp579;_tmp579.tag=0;_tmp579.f1=(void*)((
void*)3);_tmp579;});_tmp578;}));_tmp577;});_tmp576->tl=0;_tmp576;});_tmp575;});
_tmp574;});break;case 166: _LL361: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*
_tmp57C=_cycalloc(sizeof(struct Cyc_Rgnorder_tok_struct));_tmp57C[0]=({struct Cyc_Rgnorder_tok_struct
_tmp57D;_tmp57D.tag=Cyc_Rgnorder_tok;_tmp57D.f1=({struct Cyc_List_List*_tmp57E=
_cycalloc(sizeof(struct Cyc_List_List));_tmp57E->hd=({struct _tuple18*_tmp57F=
_cycalloc(sizeof(struct _tuple18));_tmp57F->f1=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp582=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp582[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp583;_tmp583.tag=0;_tmp583.f1=(void*)((void*)3);_tmp583;});_tmp582;}));
_tmp57F->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp580=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp580[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp581;_tmp581.tag=0;_tmp581.f1=(void*)((void*)3);_tmp581;});_tmp580;}));
_tmp57F;});_tmp57E->tl=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);_tmp57E;});
_tmp57D;});_tmp57C;});break;case 167: _LL362: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp584=_cycalloc_atomic(sizeof(struct Cyc_Bool_tok_struct));_tmp584[0]=({struct
Cyc_Bool_tok_struct _tmp585;_tmp585.tag=Cyc_Bool_tok;_tmp585.f1=0;_tmp585;});
_tmp584;});break;case 168: _LL363: {struct _tagged_arr _tmp586=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);if(Cyc_Std_zstrcmp(_tmp586,_tag_arr("inject",sizeof(
unsigned char),7))!= 0){Cyc_Parse_err(_tag_arr("missing type in function declaration",
sizeof(unsigned char),37),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp587=_cycalloc_atomic(sizeof(struct Cyc_Bool_tok_struct));_tmp587[0]=({struct
Cyc_Bool_tok_struct _tmp588;_tmp588.tag=Cyc_Bool_tok;_tmp588.f1=1;_tmp588;});
_tmp587;});break;}case 169: _LL364: yyval=yyvs[yyvsp_offset];break;case 170: _LL365:
yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp589=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));
_tmp589[0]=({struct Cyc_TypeList_tok_struct _tmp58A;_tmp58A.tag=Cyc_TypeList_tok;
_tmp58A.f1=Cyc_List_imp_append(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeList_tok(
yyvs[yyvsp_offset]));_tmp58A;});_tmp589;});break;case 171: _LL366: yyval=(void*)({
struct Cyc_TypeList_tok_struct*_tmp58B=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));
_tmp58B[0]=({struct Cyc_TypeList_tok_struct _tmp58C;_tmp58C.tag=Cyc_TypeList_tok;
_tmp58C.f1=0;_tmp58C;});_tmp58B;});break;case 172: _LL367: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 173: _LL368:
yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp58D=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));
_tmp58D[0]=({struct Cyc_TypeList_tok_struct _tmp58E;_tmp58E.tag=Cyc_TypeList_tok;
_tmp58E.f1=({struct Cyc_List_List*_tmp58F=_cycalloc(sizeof(struct Cyc_List_List));
_tmp58F->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp590=_cycalloc(
sizeof(struct Cyc_Absyn_RgnsEff_struct));_tmp590[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp591;_tmp591.tag=21;_tmp591.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp591;});_tmp590;}));
_tmp58F->tl=0;_tmp58F;});_tmp58E;});_tmp58D;});break;case 174: _LL369: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp592=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));_tmp592[0]=({struct Cyc_TypeList_tok_struct
_tmp593;_tmp593.tag=Cyc_TypeList_tok;_tmp593.f1=({struct Cyc_List_List*_tmp594=
_cycalloc(sizeof(struct Cyc_List_List));_tmp594->hd=(void*)Cyc_yyget_Type_tok(
yyvs[yyvsp_offset]);_tmp594->tl=0;_tmp594;});_tmp593;});_tmp592;});break;case 175:
_LL36A: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(void*)3);
yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp595=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));
_tmp595[0]=({struct Cyc_TypeList_tok_struct _tmp596;_tmp596.tag=Cyc_TypeList_tok;
_tmp596.f1=({struct Cyc_List_List*_tmp597=_cycalloc(sizeof(struct Cyc_List_List));
_tmp597->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp598=_cycalloc(
sizeof(struct Cyc_Absyn_AccessEff_struct));_tmp598[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp599;_tmp599.tag=19;_tmp599.f1=(void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);
_tmp599;});_tmp598;}));_tmp597->tl=0;_tmp597;});_tmp596;});_tmp595;});break;case
176: _LL36B: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(void*)({
struct Cyc_TypeList_tok_struct*_tmp59A=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));
_tmp59A[0]=({struct Cyc_TypeList_tok_struct _tmp59B;_tmp59B.tag=Cyc_TypeList_tok;
_tmp59B.f1=({struct Cyc_List_List*_tmp59C=_cycalloc(sizeof(struct Cyc_List_List));
_tmp59C->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp59D=_cycalloc(
sizeof(struct Cyc_Absyn_AccessEff_struct));_tmp59D[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp59E;_tmp59E.tag=19;_tmp59E.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp59E;});_tmp59D;}));
_tmp59C->tl=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp59C;});_tmp59B;});
_tmp59A;});break;case 177: _LL36C: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp59F=_cycalloc(sizeof(struct Cyc_ParamDeclList_tok_struct));_tmp59F[0]=({
struct Cyc_ParamDeclList_tok_struct _tmp5A0;_tmp5A0.tag=Cyc_ParamDeclList_tok;
_tmp5A0.f1=({struct Cyc_List_List*_tmp5A1=_cycalloc(sizeof(struct Cyc_List_List));
_tmp5A1->hd=Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset]);_tmp5A1->tl=0;_tmp5A1;});
_tmp5A0;});_tmp59F;});break;case 178: _LL36D: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp5A2=_cycalloc(sizeof(struct Cyc_ParamDeclList_tok_struct));_tmp5A2[0]=({
struct Cyc_ParamDeclList_tok_struct _tmp5A3;_tmp5A3.tag=Cyc_ParamDeclList_tok;
_tmp5A3.f1=({struct Cyc_List_List*_tmp5A4=_cycalloc(sizeof(struct Cyc_List_List));
_tmp5A4->hd=Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset]);_tmp5A4->tl=Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A4;});_tmp5A3;});
_tmp5A2;});break;case 179: _LL36E: {struct _tuple17 _tmp5A6;struct Cyc_List_List*
_tmp5A7;struct Cyc_List_List*_tmp5A8;struct Cyc_Absyn_Tqual _tmp5A9;struct _tuple17*
_tmp5A5=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp5A6=*_tmp5A5;_LL373: _tmp5A9=_tmp5A6.f1;goto _LL372;_LL372:
_tmp5A8=_tmp5A6.f2;goto _LL371;_LL371: _tmp5A7=_tmp5A6.f3;goto _LL370;_LL370: {
struct Cyc_Parse_Declarator _tmp5AB;struct Cyc_List_List*_tmp5AC;struct _tuple1*
_tmp5AD;struct Cyc_Parse_Declarator*_tmp5AA=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp5AB=*_tmp5AA;_LL376: _tmp5AD=_tmp5AB.id;goto _LL375;_LL375:
_tmp5AC=_tmp5AB.tms;goto _LL374;_LL374: {void*_tmp5AE=Cyc_Parse_speclist2typ(
_tmp5A8,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp5B0;struct Cyc_List_List*
_tmp5B1;void*_tmp5B2;struct Cyc_Absyn_Tqual _tmp5B3;struct _tuple6 _tmp5AF=Cyc_Parse_apply_tms(
_tmp5A9,_tmp5AE,_tmp5A7,_tmp5AC);_LL37B: _tmp5B3=_tmp5AF.f1;goto _LL37A;_LL37A:
_tmp5B2=_tmp5AF.f2;goto _LL379;_LL379: _tmp5B1=_tmp5AF.f3;goto _LL378;_LL378:
_tmp5B0=_tmp5AF.f4;goto _LL377;_LL377: if(_tmp5B1 != 0){Cyc_Parse_err(_tag_arr("parameter with bad type params",
sizeof(unsigned char),31),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp5AD)){Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a namespace",sizeof(unsigned char),
47),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*_tmp5B4=(struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt*_tmp5B9=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp5B9->v=(*
_tmp5AD).f2;_tmp5B9;});if(_tmp5B0 != 0){({void*_tmp5B5[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("extra attributes on parameter, ignoring",
sizeof(unsigned char),40),_tag_arr(_tmp5B5,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5B6=_cycalloc(sizeof(struct Cyc_ParamDecl_tok_struct));
_tmp5B6[0]=({struct Cyc_ParamDecl_tok_struct _tmp5B7;_tmp5B7.tag=Cyc_ParamDecl_tok;
_tmp5B7.f1=({struct _tuple2*_tmp5B8=_cycalloc(sizeof(struct _tuple2));_tmp5B8->f1=
_tmp5B4;_tmp5B8->f2=_tmp5B3;_tmp5B8->f3=_tmp5B2;_tmp5B8;});_tmp5B7;});_tmp5B6;});
break;}}}}case 180: _LL36F: {struct _tuple17 _tmp5BB;struct Cyc_List_List*_tmp5BC;
struct Cyc_List_List*_tmp5BD;struct Cyc_Absyn_Tqual _tmp5BE;struct _tuple17*_tmp5BA=
Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset]);_tmp5BB=*_tmp5BA;_LL380: _tmp5BE=
_tmp5BB.f1;goto _LL37F;_LL37F: _tmp5BD=_tmp5BB.f2;goto _LL37E;_LL37E: _tmp5BC=
_tmp5BB.f3;goto _LL37D;_LL37D: {void*_tmp5BF=Cyc_Parse_speclist2typ(_tmp5BD,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));if(_tmp5BC != 0){({
void*_tmp5C0[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(unsigned char),38),_tag_arr(_tmp5C0,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5C1=_cycalloc(sizeof(struct Cyc_ParamDecl_tok_struct));
_tmp5C1[0]=({struct Cyc_ParamDecl_tok_struct _tmp5C2;_tmp5C2.tag=Cyc_ParamDecl_tok;
_tmp5C2.f1=({struct _tuple2*_tmp5C3=_cycalloc(sizeof(struct _tuple2));_tmp5C3->f1=
0;_tmp5C3->f2=_tmp5BE;_tmp5C3->f3=_tmp5BF;_tmp5C3;});_tmp5C2;});_tmp5C1;});
break;}}case 181: _LL37C: {struct _tuple17 _tmp5C5;struct Cyc_List_List*_tmp5C6;
struct Cyc_List_List*_tmp5C7;struct Cyc_Absyn_Tqual _tmp5C8;struct _tuple17*_tmp5C4=
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp5C5=*_tmp5C4;_LL385: _tmp5C8=_tmp5C5.f1;goto _LL384;_LL384:
_tmp5C7=_tmp5C5.f2;goto _LL383;_LL383: _tmp5C6=_tmp5C5.f3;goto _LL382;_LL382: {void*
_tmp5C9=Cyc_Parse_speclist2typ(_tmp5C7,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp5CA=(Cyc_yyget_AbstractDeclarator_tok(yyvs[yyvsp_offset]))->tms;struct Cyc_List_List*
_tmp5CC;struct Cyc_List_List*_tmp5CD;void*_tmp5CE;struct Cyc_Absyn_Tqual _tmp5CF;
struct _tuple6 _tmp5CB=Cyc_Parse_apply_tms(_tmp5C8,_tmp5C9,_tmp5C6,_tmp5CA);_LL38A:
_tmp5CF=_tmp5CB.f1;goto _LL389;_LL389: _tmp5CE=_tmp5CB.f2;goto _LL388;_LL388:
_tmp5CD=_tmp5CB.f3;goto _LL387;_LL387: _tmp5CC=_tmp5CB.f4;goto _LL386;_LL386: if(
_tmp5CD != 0){({void*_tmp5D0[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("bad type parameters on formal argument, ignoring",
sizeof(unsigned char),49),_tag_arr(_tmp5D0,sizeof(void*),0));});}if(_tmp5CC != 0){({
void*_tmp5D1[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(unsigned char),38),_tag_arr(_tmp5D1,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5D2=_cycalloc(sizeof(struct Cyc_ParamDecl_tok_struct));
_tmp5D2[0]=({struct Cyc_ParamDecl_tok_struct _tmp5D3;_tmp5D3.tag=Cyc_ParamDecl_tok;
_tmp5D3.f1=({struct _tuple2*_tmp5D4=_cycalloc(sizeof(struct _tuple2));_tmp5D4->f1=
0;_tmp5D4->f2=_tmp5CF;_tmp5D4->f3=_tmp5CE;_tmp5D4;});_tmp5D3;});_tmp5D2;});
break;}}case 182: _LL381: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5D5=
_cycalloc(sizeof(struct Cyc_IdList_tok_struct));_tmp5D5[0]=({struct Cyc_IdList_tok_struct
_tmp5D6;_tmp5D6.tag=Cyc_IdList_tok;_tmp5D6.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_IdList_tok(yyvs[yyvsp_offset]));
_tmp5D6;});_tmp5D5;});break;case 183: _LL38B: yyval=(void*)({struct Cyc_IdList_tok_struct*
_tmp5D7=_cycalloc(sizeof(struct Cyc_IdList_tok_struct));_tmp5D7[0]=({struct Cyc_IdList_tok_struct
_tmp5D8;_tmp5D8.tag=Cyc_IdList_tok;_tmp5D8.f1=({struct Cyc_List_List*_tmp5D9=
_cycalloc(sizeof(struct Cyc_List_List));_tmp5D9->hd=({struct _tagged_arr*_tmp5DA=
_cycalloc(sizeof(struct _tagged_arr));_tmp5DA[0]=Cyc_yyget_String_tok(yyvs[
yyvsp_offset]);_tmp5DA;});_tmp5D9->tl=0;_tmp5D9;});_tmp5D8;});_tmp5D7;});break;
case 184: _LL38C: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5DB=_cycalloc(
sizeof(struct Cyc_IdList_tok_struct));_tmp5DB[0]=({struct Cyc_IdList_tok_struct
_tmp5DC;_tmp5DC.tag=Cyc_IdList_tok;_tmp5DC.f1=({struct Cyc_List_List*_tmp5DD=
_cycalloc(sizeof(struct Cyc_List_List));_tmp5DD->hd=({struct _tagged_arr*_tmp5DE=
_cycalloc(sizeof(struct _tagged_arr));_tmp5DE[0]=Cyc_yyget_String_tok(yyvs[
yyvsp_offset]);_tmp5DE;});_tmp5DD->tl=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5DD;});_tmp5DC;});
_tmp5DB;});break;case 185: _LL38D: yyval=yyvs[yyvsp_offset];break;case 186: _LL38E:
yyval=yyvs[yyvsp_offset];break;case 187: _LL38F: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp5DF=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp5DF[0]=({struct Cyc_Exp_tok_struct
_tmp5E0;_tmp5E0.tag=Cyc_Exp_tok;_tmp5E0.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp5E1=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_struct));_tmp5E1[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5E2;_tmp5E2.tag=34;_tmp5E2.f1=0;
_tmp5E2.f2=0;_tmp5E2;});_tmp5E1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5E0;});_tmp5DF;});break;case 188: _LL390: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp5E3=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp5E3[0]=({struct Cyc_Exp_tok_struct _tmp5E4;_tmp5E4.tag=Cyc_Exp_tok;_tmp5E4.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5E5=
_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_struct));_tmp5E5[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp5E6;_tmp5E6.tag=34;_tmp5E6.f1=0;_tmp5E6.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp5E6;});_tmp5E5;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp5E4;});
_tmp5E3;});break;case 189: _LL391: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp5E7=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp5E7[0]=({struct Cyc_Exp_tok_struct
_tmp5E8;_tmp5E8.tag=Cyc_Exp_tok;_tmp5E8.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp5E9=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_struct));_tmp5E9[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5EA;_tmp5EA.tag=34;_tmp5EA.f1=0;
_tmp5EA.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp5EA;});_tmp5E9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5E8;});_tmp5E7;});break;case 190: _LL392: {struct
Cyc_Absyn_Vardecl*_tmp5EB=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp5F1=
_cycalloc(sizeof(struct _tuple1));_tmp5F1->f1=(void*)0;_tmp5F1->f2=({struct
_tagged_arr*_tmp5F2=_cycalloc(sizeof(struct _tagged_arr));_tmp5F2[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp5F2;});_tmp5F1;}),
Cyc_Absyn_uint_t,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));_tmp5EB->tq=({
struct Cyc_Absyn_Tqual _tmp5EC;_tmp5EC.q_const=1;_tmp5EC.q_volatile=0;_tmp5EC.q_restrict=
1;_tmp5EC;});yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp5ED=_cycalloc(sizeof(
struct Cyc_Exp_tok_struct));_tmp5ED[0]=({struct Cyc_Exp_tok_struct _tmp5EE;_tmp5EE.tag=
Cyc_Exp_tok;_tmp5EE.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp5EF=_cycalloc(sizeof(struct Cyc_Absyn_Comprehension_e_struct));_tmp5EF[0]=({
struct Cyc_Absyn_Comprehension_e_struct _tmp5F0;_tmp5F0.tag=27;_tmp5F0.f1=_tmp5EB;
_tmp5F0.f2=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp5F0.f3=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5F0;});_tmp5EF;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmp5EE;});
_tmp5ED;});break;}case 191: _LL393: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp5F3=_cycalloc(sizeof(struct Cyc_InitializerList_tok_struct));_tmp5F3[0]=({
struct Cyc_InitializerList_tok_struct _tmp5F4;_tmp5F4.tag=Cyc_InitializerList_tok;
_tmp5F4.f1=({struct Cyc_List_List*_tmp5F5=_cycalloc(sizeof(struct Cyc_List_List));
_tmp5F5->hd=({struct _tuple19*_tmp5F6=_cycalloc(sizeof(struct _tuple19));_tmp5F6->f1=
0;_tmp5F6->f2=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp5F6;});_tmp5F5->tl=0;
_tmp5F5;});_tmp5F4;});_tmp5F3;});break;case 192: _LL394: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp5F7=_cycalloc(sizeof(struct Cyc_InitializerList_tok_struct));_tmp5F7[0]=({
struct Cyc_InitializerList_tok_struct _tmp5F8;_tmp5F8.tag=Cyc_InitializerList_tok;
_tmp5F8.f1=({struct Cyc_List_List*_tmp5F9=_cycalloc(sizeof(struct Cyc_List_List));
_tmp5F9->hd=({struct _tuple19*_tmp5FA=_cycalloc(sizeof(struct _tuple19));_tmp5FA->f1=
Cyc_yyget_DesignatorList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp5FA->f2=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp5FA;});
_tmp5F9->tl=0;_tmp5F9;});_tmp5F8;});_tmp5F7;});break;case 193: _LL395: yyval=(void*)({
struct Cyc_InitializerList_tok_struct*_tmp5FB=_cycalloc(sizeof(struct Cyc_InitializerList_tok_struct));
_tmp5FB[0]=({struct Cyc_InitializerList_tok_struct _tmp5FC;_tmp5FC.tag=Cyc_InitializerList_tok;
_tmp5FC.f1=({struct Cyc_List_List*_tmp5FD=_cycalloc(sizeof(struct Cyc_List_List));
_tmp5FD->hd=({struct _tuple19*_tmp5FE=_cycalloc(sizeof(struct _tuple19));_tmp5FE->f1=
0;_tmp5FE->f2=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp5FE;});_tmp5FD->tl=Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5FD;});_tmp5FC;});
_tmp5FB;});break;case 194: _LL396: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp5FF=_cycalloc(sizeof(struct Cyc_InitializerList_tok_struct));_tmp5FF[0]=({
struct Cyc_InitializerList_tok_struct _tmp600;_tmp600.tag=Cyc_InitializerList_tok;
_tmp600.f1=({struct Cyc_List_List*_tmp601=_cycalloc(sizeof(struct Cyc_List_List));
_tmp601->hd=({struct _tuple19*_tmp602=_cycalloc(sizeof(struct _tuple19));_tmp602->f1=
Cyc_yyget_DesignatorList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp602->f2=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp602;});
_tmp601->tl=Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp601;});_tmp600;});_tmp5FF;});break;case 195: _LL397:
yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp603=_cycalloc(sizeof(
struct Cyc_DesignatorList_tok_struct));_tmp603[0]=({struct Cyc_DesignatorList_tok_struct
_tmp604;_tmp604.tag=Cyc_DesignatorList_tok;_tmp604.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp604;});_tmp603;});
break;case 196: _LL398: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp605=
_cycalloc(sizeof(struct Cyc_DesignatorList_tok_struct));_tmp605[0]=({struct Cyc_DesignatorList_tok_struct
_tmp606;_tmp606.tag=Cyc_DesignatorList_tok;_tmp606.f1=({struct Cyc_List_List*
_tmp607=_cycalloc(sizeof(struct Cyc_List_List));_tmp607->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[yyvsp_offset]);_tmp607->tl=0;_tmp607;});_tmp606;});_tmp605;});break;case 197:
_LL399: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp608=_cycalloc(
sizeof(struct Cyc_DesignatorList_tok_struct));_tmp608[0]=({struct Cyc_DesignatorList_tok_struct
_tmp609;_tmp609.tag=Cyc_DesignatorList_tok;_tmp609.f1=({struct Cyc_List_List*
_tmp60A=_cycalloc(sizeof(struct Cyc_List_List));_tmp60A->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[yyvsp_offset]);_tmp60A->tl=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp60A;});_tmp609;});
_tmp608;});break;case 198: _LL39A: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp60B=_cycalloc(sizeof(struct Cyc_Designator_tok_struct));_tmp60B[0]=({struct
Cyc_Designator_tok_struct _tmp60C;_tmp60C.tag=Cyc_Designator_tok;_tmp60C.f1=(void*)((
void*)({struct Cyc_Absyn_ArrayElement_struct*_tmp60D=_cycalloc(sizeof(struct Cyc_Absyn_ArrayElement_struct));
_tmp60D[0]=({struct Cyc_Absyn_ArrayElement_struct _tmp60E;_tmp60E.tag=0;_tmp60E.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp60E;});_tmp60D;}));_tmp60C;});_tmp60B;});break;case 199: _LL39B: yyval=(void*)({
struct Cyc_Designator_tok_struct*_tmp60F=_cycalloc(sizeof(struct Cyc_Designator_tok_struct));
_tmp60F[0]=({struct Cyc_Designator_tok_struct _tmp610;_tmp610.tag=Cyc_Designator_tok;
_tmp610.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp611=_cycalloc(
sizeof(struct Cyc_Absyn_FieldName_struct));_tmp611[0]=({struct Cyc_Absyn_FieldName_struct
_tmp612;_tmp612.tag=1;_tmp612.f1=({struct _tagged_arr*_tmp613=_cycalloc(sizeof(
struct _tagged_arr));_tmp613[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp613;});
_tmp612;});_tmp611;}));_tmp610;});_tmp60F;});break;case 200: _LL39C: {void*_tmp614=
Cyc_Parse_speclist2typ((*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));struct Cyc_List_List*
_tmp615=(*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f3;if(_tmp615 != 0){({
void*_tmp616[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("ignoring attributes in type",
sizeof(unsigned char),28),_tag_arr(_tmp616,sizeof(void*),0));});}{struct Cyc_Absyn_Tqual
_tmp617=(*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f1;yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp618=_cycalloc(sizeof(struct Cyc_ParamDecl_tok_struct));
_tmp618[0]=({struct Cyc_ParamDecl_tok_struct _tmp619;_tmp619.tag=Cyc_ParamDecl_tok;
_tmp619.f1=({struct _tuple2*_tmp61A=_cycalloc(sizeof(struct _tuple2));_tmp61A->f1=
0;_tmp61A->f2=_tmp617;_tmp61A->f3=_tmp614;_tmp61A;});_tmp619;});_tmp618;});
break;}}case 201: _LL39D: {void*_tmp61B=Cyc_Parse_speclist2typ((*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp61C=(*Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f3;struct Cyc_Absyn_Tqual _tmp61D=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f1;struct Cyc_List_List*
_tmp61E=(Cyc_yyget_AbstractDeclarator_tok(yyvs[yyvsp_offset]))->tms;struct
_tuple6 _tmp61F=Cyc_Parse_apply_tms(_tmp61D,_tmp61B,_tmp61C,_tmp61E);if(_tmp61F.f3
!= 0){({void*_tmp620[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad type params, ignoring",
sizeof(unsigned char),26),_tag_arr(_tmp620,sizeof(void*),0));});}if(_tmp61F.f4 != 
0){({void*_tmp621[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad specifiers, ignoring",
sizeof(unsigned char),25),_tag_arr(_tmp621,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp622=_cycalloc(sizeof(struct Cyc_ParamDecl_tok_struct));
_tmp622[0]=({struct Cyc_ParamDecl_tok_struct _tmp623;_tmp623.tag=Cyc_ParamDecl_tok;
_tmp623.f1=({struct _tuple2*_tmp624=_cycalloc(sizeof(struct _tuple2));_tmp624->f1=
0;_tmp624->f2=_tmp61F.f1;_tmp624->f3=_tmp61F.f2;_tmp624;});_tmp623;});_tmp622;});
break;}case 202: _LL39E: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp625=_cycalloc(
sizeof(struct Cyc_Type_tok_struct));_tmp625[0]=({struct Cyc_Type_tok_struct _tmp626;
_tmp626.tag=Cyc_Type_tok;_tmp626.f1=(void*)(*Cyc_yyget_ParamDecl_tok(yyvs[
yyvsp_offset])).f3;_tmp626;});_tmp625;});break;case 203: _LL39F: yyval=(void*)({
struct Cyc_Type_tok_struct*_tmp627=_cycalloc(sizeof(struct Cyc_Type_tok_struct));
_tmp627[0]=({struct Cyc_Type_tok_struct _tmp628;_tmp628.tag=Cyc_Type_tok;_tmp628.f1=(
void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp629=_cycalloc(sizeof(struct
Cyc_Absyn_JoinEff_struct));_tmp629[0]=({struct Cyc_Absyn_JoinEff_struct _tmp62A;
_tmp62A.tag=20;_tmp62A.f1=0;_tmp62A;});_tmp629;}));_tmp628;});_tmp627;});break;
case 204: _LL3A0: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp62B=_cycalloc(
sizeof(struct Cyc_Type_tok_struct));_tmp62B[0]=({struct Cyc_Type_tok_struct _tmp62C;
_tmp62C.tag=Cyc_Type_tok;_tmp62C.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp62D=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp62D[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp62E;_tmp62E.tag=20;_tmp62E.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp62E;});_tmp62D;}));
_tmp62C;});_tmp62B;});break;case 205: _LL3A1: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp62F=_cycalloc(sizeof(struct Cyc_Type_tok_struct));_tmp62F[0]=({struct Cyc_Type_tok_struct
_tmp630;_tmp630.tag=Cyc_Type_tok;_tmp630.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp631=_cycalloc(sizeof(struct Cyc_Absyn_RgnsEff_struct));_tmp631[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp632;_tmp632.tag=21;_tmp632.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp632;});_tmp631;}));
_tmp630;});_tmp62F;});break;case 206: _LL3A2: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp633=_cycalloc(sizeof(struct Cyc_Type_tok_struct));_tmp633[0]=({struct Cyc_Type_tok_struct
_tmp634;_tmp634.tag=Cyc_Type_tok;_tmp634.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp635=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp635[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp636;_tmp636.tag=20;_tmp636.f1=({struct Cyc_List_List*_tmp637=_cycalloc(
sizeof(struct Cyc_List_List));_tmp637->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp637->tl=Cyc_yyget_TypeList_tok(
yyvs[yyvsp_offset]);_tmp637;});_tmp636;});_tmp635;}));_tmp634;});_tmp633;});
break;case 207: _LL3A3: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp638=
_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));_tmp638[0]=({struct Cyc_TypeList_tok_struct
_tmp639;_tmp639.tag=Cyc_TypeList_tok;_tmp639.f1=({struct Cyc_List_List*_tmp63A=
_cycalloc(sizeof(struct Cyc_List_List));_tmp63A->hd=(void*)Cyc_yyget_Type_tok(
yyvs[yyvsp_offset]);_tmp63A->tl=0;_tmp63A;});_tmp639;});_tmp638;});break;case 208:
_LL3A4: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp63B=_cycalloc(sizeof(
struct Cyc_TypeList_tok_struct));_tmp63B[0]=({struct Cyc_TypeList_tok_struct
_tmp63C;_tmp63C.tag=Cyc_TypeList_tok;_tmp63C.f1=({struct Cyc_List_List*_tmp63D=
_cycalloc(sizeof(struct Cyc_List_List));_tmp63D->hd=(void*)Cyc_yyget_Type_tok(
yyvs[yyvsp_offset]);_tmp63D->tl=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp63D;});_tmp63C;});
_tmp63B;});break;case 209: _LL3A5: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp63E=_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmp63E[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp63F;_tmp63F.tag=Cyc_AbstractDeclarator_tok;
_tmp63F.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp640=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmp640->tms=Cyc_yyget_TypeModifierList_tok(yyvs[
yyvsp_offset]);_tmp640;});_tmp63F;});_tmp63E;});break;case 210: _LL3A6: yyval=yyvs[
yyvsp_offset];break;case 211: _LL3A7: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp641=_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmp641[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp642;_tmp642.tag=Cyc_AbstractDeclarator_tok;
_tmp642.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp643=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmp643->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[yyvsp_offset]))->tms);_tmp643;});_tmp642;});_tmp641;});break;case 212: _LL3A8:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 213:
_LL3A9: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp644=_cycalloc(
sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmp644[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp645;_tmp645.tag=Cyc_AbstractDeclarator_tok;_tmp645.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp646=_cycalloc(sizeof(struct Cyc_Parse_Abstractdeclarator));_tmp646->tms=({
struct Cyc_List_List*_tmp647=_cycalloc(sizeof(struct Cyc_List_List));_tmp647->hd=(
void*)((void*)0);_tmp647->tl=0;_tmp647;});_tmp646;});_tmp645;});_tmp644;});
break;case 214: _LL3AA: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp648=_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmp648[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp649;_tmp649.tag=Cyc_AbstractDeclarator_tok;
_tmp649.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp64A=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmp64A->tms=({struct Cyc_List_List*_tmp64B=
_cycalloc(sizeof(struct Cyc_List_List));_tmp64B->hd=(void*)((void*)0);_tmp64B->tl=(
Cyc_yyget_AbstractDeclarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]))->tms;_tmp64B;});_tmp64A;});_tmp649;});_tmp648;});break;case
215: _LL3AB: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp64C=
_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmp64C[0]=({struct
Cyc_AbstractDeclarator_tok_struct _tmp64D;_tmp64D.tag=Cyc_AbstractDeclarator_tok;
_tmp64D.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp64E=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmp64E->tms=({struct Cyc_List_List*_tmp64F=
_cycalloc(sizeof(struct Cyc_List_List));_tmp64F->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp650=_cycalloc(sizeof(struct Cyc_Absyn_ConstArray_mod_struct));_tmp650[0]=({
struct Cyc_Absyn_ConstArray_mod_struct _tmp651;_tmp651.tag=0;_tmp651.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp651;});_tmp650;}));
_tmp64F->tl=0;_tmp64F;});_tmp64E;});_tmp64D;});_tmp64C;});break;case 216: _LL3AC:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp652=_cycalloc(sizeof(
struct Cyc_AbstractDeclarator_tok_struct));_tmp652[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp653;_tmp653.tag=Cyc_AbstractDeclarator_tok;_tmp653.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp654=_cycalloc(sizeof(struct Cyc_Parse_Abstractdeclarator));_tmp654->tms=({
struct Cyc_List_List*_tmp655=_cycalloc(sizeof(struct Cyc_List_List));_tmp655->hd=(
void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp656=_cycalloc(sizeof(
struct Cyc_Absyn_ConstArray_mod_struct));_tmp656[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp657;_tmp657.tag=0;_tmp657.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp657;});_tmp656;}));
_tmp655->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp655;});_tmp654;});
_tmp653;});_tmp652;});break;case 217: _LL3AD: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp658=_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmp658[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp659;_tmp659.tag=Cyc_AbstractDeclarator_tok;
_tmp659.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp65A=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmp65A->tms=({struct Cyc_List_List*_tmp65B=
_cycalloc(sizeof(struct Cyc_List_List));_tmp65B->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp65C=_cycalloc(sizeof(struct Cyc_Absyn_Function_mod_struct));_tmp65C[0]=({
struct Cyc_Absyn_Function_mod_struct _tmp65D;_tmp65D.tag=2;_tmp65D.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp65E=_cycalloc(sizeof(struct Cyc_Absyn_WithTypes_struct));
_tmp65E[0]=({struct Cyc_Absyn_WithTypes_struct _tmp65F;_tmp65F.tag=1;_tmp65F.f1=0;
_tmp65F.f2=0;_tmp65F.f3=0;_tmp65F.f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp65F.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp65F;});_tmp65E;}));
_tmp65D;});_tmp65C;}));_tmp65B->tl=0;_tmp65B;});_tmp65A;});_tmp659;});_tmp658;});
break;case 218: _LL3AE: {struct _tuple16 _tmp661;struct Cyc_List_List*_tmp662;struct
Cyc_Core_Opt*_tmp663;struct Cyc_Absyn_VarargInfo*_tmp664;int _tmp665;struct Cyc_List_List*
_tmp666;struct _tuple16*_tmp660=Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp661=*_tmp660;_LL3B5: _tmp666=_tmp661.f1;goto _LL3B4;
_LL3B4: _tmp665=_tmp661.f2;goto _LL3B3;_LL3B3: _tmp664=_tmp661.f3;goto _LL3B2;_LL3B2:
_tmp663=_tmp661.f4;goto _LL3B1;_LL3B1: _tmp662=_tmp661.f5;goto _LL3B0;_LL3B0: yyval=(
void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp667=_cycalloc(sizeof(struct
Cyc_AbstractDeclarator_tok_struct));_tmp667[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp668;_tmp668.tag=Cyc_AbstractDeclarator_tok;_tmp668.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp669=_cycalloc(sizeof(struct Cyc_Parse_Abstractdeclarator));_tmp669->tms=({
struct Cyc_List_List*_tmp66A=_cycalloc(sizeof(struct Cyc_List_List));_tmp66A->hd=(
void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp66B=_cycalloc(sizeof(
struct Cyc_Absyn_Function_mod_struct));_tmp66B[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp66C;_tmp66C.tag=2;_tmp66C.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp66D=_cycalloc(sizeof(struct Cyc_Absyn_WithTypes_struct));_tmp66D[0]=({struct
Cyc_Absyn_WithTypes_struct _tmp66E;_tmp66E.tag=1;_tmp66E.f1=_tmp666;_tmp66E.f2=
_tmp665;_tmp66E.f3=_tmp664;_tmp66E.f4=_tmp663;_tmp66E.f5=_tmp662;_tmp66E;});
_tmp66D;}));_tmp66C;});_tmp66B;}));_tmp66A->tl=0;_tmp66A;});_tmp669;});_tmp668;});
_tmp667;});break;}case 219: _LL3AF: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp66F=_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmp66F[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp670;_tmp670.tag=Cyc_AbstractDeclarator_tok;
_tmp670.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp671=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmp671->tms=({struct Cyc_List_List*_tmp672=
_cycalloc(sizeof(struct Cyc_List_List));_tmp672->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp673=_cycalloc(sizeof(struct Cyc_Absyn_Function_mod_struct));_tmp673[0]=({
struct Cyc_Absyn_Function_mod_struct _tmp674;_tmp674.tag=2;_tmp674.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp675=_cycalloc(sizeof(struct Cyc_Absyn_WithTypes_struct));
_tmp675[0]=({struct Cyc_Absyn_WithTypes_struct _tmp676;_tmp676.tag=1;_tmp676.f1=0;
_tmp676.f2=0;_tmp676.f3=0;_tmp676.f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp676.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp676;});_tmp675;}));
_tmp674;});_tmp673;}));_tmp672->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp672;});_tmp671;});
_tmp670;});_tmp66F;});break;case 220: _LL3B6: {struct _tuple16 _tmp678;struct Cyc_List_List*
_tmp679;struct Cyc_Core_Opt*_tmp67A;struct Cyc_Absyn_VarargInfo*_tmp67B;int _tmp67C;
struct Cyc_List_List*_tmp67D;struct _tuple16*_tmp677=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp678=*_tmp677;_LL3BD:
_tmp67D=_tmp678.f1;goto _LL3BC;_LL3BC: _tmp67C=_tmp678.f2;goto _LL3BB;_LL3BB:
_tmp67B=_tmp678.f3;goto _LL3BA;_LL3BA: _tmp67A=_tmp678.f4;goto _LL3B9;_LL3B9:
_tmp679=_tmp678.f5;goto _LL3B8;_LL3B8: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp67E=_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmp67E[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp67F;_tmp67F.tag=Cyc_AbstractDeclarator_tok;
_tmp67F.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp680=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmp680->tms=({struct Cyc_List_List*_tmp681=
_cycalloc(sizeof(struct Cyc_List_List));_tmp681->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp682=_cycalloc(sizeof(struct Cyc_Absyn_Function_mod_struct));_tmp682[0]=({
struct Cyc_Absyn_Function_mod_struct _tmp683;_tmp683.tag=2;_tmp683.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp684=_cycalloc(sizeof(struct Cyc_Absyn_WithTypes_struct));
_tmp684[0]=({struct Cyc_Absyn_WithTypes_struct _tmp685;_tmp685.tag=1;_tmp685.f1=
_tmp67D;_tmp685.f2=_tmp67C;_tmp685.f3=_tmp67B;_tmp685.f4=_tmp67A;_tmp685.f5=
_tmp679;_tmp685;});_tmp684;}));_tmp683;});_tmp682;}));_tmp681->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp681;});
_tmp680;});_tmp67F;});_tmp67E;});break;}case 221: _LL3B7: {struct Cyc_List_List*
_tmp686=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp687=_cycalloc(sizeof(
struct Cyc_AbstractDeclarator_tok_struct));_tmp687[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp688;_tmp688.tag=Cyc_AbstractDeclarator_tok;_tmp688.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp689=_cycalloc(sizeof(struct Cyc_Parse_Abstractdeclarator));_tmp689->tms=({
struct Cyc_List_List*_tmp68A=_cycalloc(sizeof(struct Cyc_List_List));_tmp68A->hd=(
void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp68B=_cycalloc(sizeof(
struct Cyc_Absyn_TypeParams_mod_struct));_tmp68B[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp68C;_tmp68C.tag=3;_tmp68C.f1=_tmp686;_tmp68C.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp68C.f3=0;_tmp68C;});_tmp68B;}));_tmp68A->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp68A;});
_tmp689;});_tmp688;});_tmp687;});break;}case 222: _LL3BE:((struct Cyc_Lexing_lexbuf*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;{struct Cyc_List_List*
_tmp68D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp68E=_cycalloc(sizeof(
struct Cyc_AbstractDeclarator_tok_struct));_tmp68E[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp68F;_tmp68F.tag=Cyc_AbstractDeclarator_tok;_tmp68F.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp690=_cycalloc(sizeof(struct Cyc_Parse_Abstractdeclarator));_tmp690->tms=({
struct Cyc_List_List*_tmp691=_cycalloc(sizeof(struct Cyc_List_List));_tmp691->hd=(
void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp692=_cycalloc(sizeof(
struct Cyc_Absyn_TypeParams_mod_struct));_tmp692[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp693;_tmp693.tag=3;_tmp693.f1=_tmp68D;_tmp693.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp693.f3=0;_tmp693;});_tmp692;}));_tmp691->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp691;});
_tmp690;});_tmp68F;});_tmp68E;});break;}case 223: _LL3BF: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp694=_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmp694[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp695;_tmp695.tag=Cyc_AbstractDeclarator_tok;
_tmp695.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp696=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmp696->tms=({struct Cyc_List_List*_tmp697=
_cycalloc(sizeof(struct Cyc_List_List));_tmp697->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp698=_cycalloc(sizeof(struct Cyc_Absyn_Attributes_mod_struct));_tmp698[0]=({
struct Cyc_Absyn_Attributes_mod_struct _tmp699;_tmp699.tag=4;_tmp699.f1=Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);_tmp699.f2=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp699;});_tmp698;}));_tmp697->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp697;});
_tmp696;});_tmp695;});_tmp694;});break;case 224: _LL3C0: yyval=yyvs[yyvsp_offset];
break;case 225: _LL3C1: yyval=yyvs[yyvsp_offset];break;case 226: _LL3C2: yyval=yyvs[
yyvsp_offset];break;case 227: _LL3C3: yyval=yyvs[yyvsp_offset];break;case 228: _LL3C4:
yyval=yyvs[yyvsp_offset];break;case 229: _LL3C5: yyval=yyvs[yyvsp_offset];break;
case 230: _LL3C6: if(Cyc_Std_zstrcmp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tag_arr("`H",sizeof(
unsigned char),3))== 0){Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(unsigned char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp6A4=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_tmp6A4->name=({struct _tagged_arr*_tmp6A7=_cycalloc(sizeof(struct _tagged_arr));
_tmp6A7[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp6A7;});_tmp6A4->identity=0;_tmp6A4->kind=(void*)((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp6A5=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));
_tmp6A5[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6A6;_tmp6A6.tag=0;_tmp6A6.f1=(void*)((
void*)3);_tmp6A6;});_tmp6A5;}));_tmp6A4;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp6A2=_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));_tmp6A2[0]=({struct Cyc_Absyn_VarType_struct
_tmp6A3;_tmp6A3.tag=1;_tmp6A3.f1=tv;_tmp6A3;});_tmp6A2;});yyval=(void*)({struct
Cyc_Stmt_tok_struct*_tmp69A=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp69A[
0]=({struct Cyc_Stmt_tok_struct _tmp69B;_tmp69B.tag=Cyc_Stmt_tok;_tmp69B.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Region_s_struct*_tmp69C=_cycalloc(sizeof(struct Cyc_Absyn_Region_s_struct));
_tmp69C[0]=({struct Cyc_Absyn_Region_s_struct _tmp69D;_tmp69D.tag=18;_tmp69D.f1=tv;
_tmp69D.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp69E=_cycalloc(sizeof(struct
_tuple1));_tmp69E->f1=(void*)0;_tmp69E->f2=({struct _tagged_arr*_tmp69F=_cycalloc(
sizeof(struct _tagged_arr));_tmp69F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp69F;});_tmp69E;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp6A0=_cycalloc(sizeof(struct Cyc_Absyn_RgnHandleType_struct));
_tmp6A0[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp6A1;_tmp6A1.tag=17;_tmp6A1.f1=(
void*)t;_tmp6A1;});_tmp6A0;}),0);_tmp69D.f3=Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);
_tmp69D;});_tmp69C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp69B;});_tmp69A;});break;}case 231: _LL3C7: if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tag_arr("H",sizeof(unsigned char),2))== 0){Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(unsigned char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp6B2=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_tmp6B2->name=({struct _tagged_arr*_tmp6B5=_cycalloc(sizeof(struct _tagged_arr));
_tmp6B5[0]=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp6B7;_tmp6B7.tag=
0;_tmp6B7.f1=(struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{void*_tmp6B6[1]={&
_tmp6B7};Cyc_Std_aprintf(_tag_arr("`%s",sizeof(unsigned char),4),_tag_arr(
_tmp6B6,sizeof(void*),1));}});_tmp6B5;});_tmp6B2->identity=0;_tmp6B2->kind=(void*)((
void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6B3=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));
_tmp6B3[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6B4;_tmp6B4.tag=0;_tmp6B4.f1=(void*)((
void*)3);_tmp6B4;});_tmp6B3;}));_tmp6B2;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp6B0=_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));_tmp6B0[0]=({struct Cyc_Absyn_VarType_struct
_tmp6B1;_tmp6B1.tag=1;_tmp6B1.f1=tv;_tmp6B1;});_tmp6B0;});yyval=(void*)({struct
Cyc_Stmt_tok_struct*_tmp6A8=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp6A8[
0]=({struct Cyc_Stmt_tok_struct _tmp6A9;_tmp6A9.tag=Cyc_Stmt_tok;_tmp6A9.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Region_s_struct*_tmp6AA=_cycalloc(sizeof(struct Cyc_Absyn_Region_s_struct));
_tmp6AA[0]=({struct Cyc_Absyn_Region_s_struct _tmp6AB;_tmp6AB.tag=18;_tmp6AB.f1=tv;
_tmp6AB.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp6AC=_cycalloc(sizeof(struct
_tuple1));_tmp6AC->f1=(void*)0;_tmp6AC->f2=({struct _tagged_arr*_tmp6AD=_cycalloc(
sizeof(struct _tagged_arr));_tmp6AD[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6AD;});_tmp6AC;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp6AE=_cycalloc(sizeof(struct Cyc_Absyn_RgnHandleType_struct));
_tmp6AE[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp6AF;_tmp6AF.tag=17;_tmp6AF.f1=(
void*)t;_tmp6AF;});_tmp6AE;}),0);_tmp6AB.f3=Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);
_tmp6AB;});_tmp6AA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6A9;});_tmp6A8;});break;}case 232: _LL3C8: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6B8=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp6B8[0]=({struct Cyc_Stmt_tok_struct _tmp6B9;_tmp6B9.tag=Cyc_Stmt_tok;_tmp6B9.f1=
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Cut_s_struct*_tmp6BA=_cycalloc(
sizeof(struct Cyc_Absyn_Cut_s_struct));_tmp6BA[0]=({struct Cyc_Absyn_Cut_s_struct
_tmp6BB;_tmp6BB.tag=13;_tmp6BB.f1=Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp6BB;});
_tmp6BA;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6B9;});
_tmp6B8;});break;case 233: _LL3C9: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6BC=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp6BC[0]=({struct Cyc_Stmt_tok_struct
_tmp6BD;_tmp6BD.tag=Cyc_Stmt_tok;_tmp6BD.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Splice_s_struct*_tmp6BE=_cycalloc(sizeof(struct Cyc_Absyn_Splice_s_struct));
_tmp6BE[0]=({struct Cyc_Absyn_Splice_s_struct _tmp6BF;_tmp6BF.tag=14;_tmp6BF.f1=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp6BF;});_tmp6BE;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6BD;});_tmp6BC;});break;case 234: _LL3CA: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6C0=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp6C0[0]=({struct Cyc_Stmt_tok_struct _tmp6C1;_tmp6C1.tag=Cyc_Stmt_tok;_tmp6C1.f1=
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_struct*_tmp6C2=_cycalloc(
sizeof(struct Cyc_Absyn_Label_s_struct));_tmp6C2[0]=({struct Cyc_Absyn_Label_s_struct
_tmp6C3;_tmp6C3.tag=15;_tmp6C3.f1=({struct _tagged_arr*_tmp6C4=_cycalloc(sizeof(
struct _tagged_arr));_tmp6C4[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6C4;});_tmp6C3.f2=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp6C3;});_tmp6C2;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6C1;});_tmp6C0;});break;case 235: _LL3CB: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6C5=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp6C5[0]=({struct Cyc_Stmt_tok_struct _tmp6C6;_tmp6C6.tag=Cyc_Stmt_tok;_tmp6C6.f1=
Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp6C6;});_tmp6C5;});break;case 236: _LL3CC: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6C7=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp6C7[0]=({struct Cyc_Stmt_tok_struct _tmp6C8;_tmp6C8.tag=Cyc_Stmt_tok;_tmp6C8.f1=
Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6C8;});_tmp6C7;});break;case 237: _LL3CD: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6C9=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp6C9[0]=({struct Cyc_Stmt_tok_struct _tmp6CA;_tmp6CA.tag=Cyc_Stmt_tok;_tmp6CA.f1=
Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6CA;});_tmp6C9;});break;case 238: _LL3CE: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 239: _LL3CF:
yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6CB=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp6CB[0]=({struct Cyc_Stmt_tok_struct _tmp6CC;_tmp6CC.tag=Cyc_Stmt_tok;_tmp6CC.f1=
Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(
0));_tmp6CC;});_tmp6CB;});break;case 240: _LL3D0: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6CD=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp6CD[0]=({struct Cyc_Stmt_tok_struct
_tmp6CE;_tmp6CE.tag=Cyc_Stmt_tok;_tmp6CE.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]));_tmp6CE;});_tmp6CD;});break;case 241: _LL3D1: yyval=yyvs[
yyvsp_offset];break;case 242: _LL3D2: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6CF=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp6CF[0]=({struct Cyc_Stmt_tok_struct
_tmp6D0;_tmp6D0.tag=Cyc_Stmt_tok;_tmp6D0.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6D0;});_tmp6CF;});break;case 243: _LL3D3: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6D1=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp6D1[0]=({struct Cyc_Stmt_tok_struct _tmp6D2;_tmp6D2.tag=Cyc_Stmt_tok;_tmp6D2.f1=
Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp6D3=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_struct));_tmp6D3[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp6D4;_tmp6D4.tag=1;_tmp6D4.f1=Cyc_yyget_FnDecl_tok(yyvs[yyvsp_offset]);
_tmp6D4;});_tmp6D3;}),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp6D2;});_tmp6D1;});
break;case 244: _LL3D4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6D5=_cycalloc(
sizeof(struct Cyc_Stmt_tok_struct));_tmp6D5[0]=({struct Cyc_Stmt_tok_struct _tmp6D6;
_tmp6D6.tag=Cyc_Stmt_tok;_tmp6D6.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp6D7=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_struct));
_tmp6D7[0]=({struct Cyc_Absyn_Fn_d_struct _tmp6D8;_tmp6D8.tag=1;_tmp6D8.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D8;});_tmp6D7;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]));_tmp6D6;});
_tmp6D5;});break;case 245: _LL3D5: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6D9=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp6D9[0]=({struct Cyc_Stmt_tok_struct
_tmp6DA;_tmp6DA.tag=Cyc_Stmt_tok;_tmp6DA.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6DA;});_tmp6D9;});break;case 246: _LL3D6: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6DB=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp6DB[0]=({struct Cyc_Stmt_tok_struct _tmp6DC;_tmp6DC.tag=Cyc_Stmt_tok;_tmp6DC.f1=
Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6DC;});_tmp6DB;});break;case 247: _LL3D7: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6DD=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp6DD[0]=({struct Cyc_Stmt_tok_struct _tmp6DE;_tmp6DE.tag=Cyc_Stmt_tok;_tmp6DE.f1=
Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6DE;});_tmp6DD;});break;case 248: _LL3D8: if(Cyc_Std_strcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tag_arr("C",sizeof(unsigned char),2))!= 0){Cyc_Parse_err(_tag_arr("only switch \"C\" { ... } is allowed",
sizeof(unsigned char),35),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6DF=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp6DF[0]=({struct Cyc_Stmt_tok_struct
_tmp6E0;_tmp6E0.tag=Cyc_Stmt_tok;_tmp6E0.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_SwitchC_s_struct*_tmp6E1=_cycalloc(sizeof(struct Cyc_Absyn_SwitchC_s_struct));
_tmp6E1[0]=({struct Cyc_Absyn_SwitchC_s_struct _tmp6E2;_tmp6E2.tag=10;_tmp6E2.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp6E2.f2=Cyc_yyget_SwitchCClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp6E2;});_tmp6E1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6E0;});_tmp6DF;});break;case 249: _LL3D9: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6E3=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp6E3[0]=({struct Cyc_Stmt_tok_struct _tmp6E4;_tmp6E4.tag=Cyc_Stmt_tok;_tmp6E4.f1=
Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6E4;});_tmp6E3;});break;case 250: _LL3DA: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp6E5=_cycalloc(sizeof(struct Cyc_SwitchClauseList_tok_struct));
_tmp6E5[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp6E6;_tmp6E6.tag=Cyc_SwitchClauseList_tok;
_tmp6E6.f1=0;_tmp6E6;});_tmp6E5;});break;case 251: _LL3DB: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6E7=_cycalloc(sizeof(struct Cyc_SwitchClauseList_tok_struct));_tmp6E7[0]=({
struct Cyc_SwitchClauseList_tok_struct _tmp6E8;_tmp6E8.tag=Cyc_SwitchClauseList_tok;
_tmp6E8.f1=({struct Cyc_List_List*_tmp6E9=_cycalloc(sizeof(struct Cyc_List_List));
_tmp6E9->hd=({struct Cyc_Absyn_Switch_clause*_tmp6EA=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
_tmp6EA->pattern=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp6EA->pat_vars=
0;_tmp6EA->where_clause=0;_tmp6EA->body=Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);
_tmp6EA->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line);_tmp6EA;});
_tmp6E9->tl=0;_tmp6E9;});_tmp6E8;});_tmp6E7;});break;case 252: _LL3DC: yyval=(void*)({
struct Cyc_SwitchClauseList_tok_struct*_tmp6EB=_cycalloc(sizeof(struct Cyc_SwitchClauseList_tok_struct));
_tmp6EB[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp6EC;_tmp6EC.tag=Cyc_SwitchClauseList_tok;
_tmp6EC.f1=({struct Cyc_List_List*_tmp6ED=_cycalloc(sizeof(struct Cyc_List_List));
_tmp6ED->hd=({struct Cyc_Absyn_Switch_clause*_tmp6EE=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
_tmp6EE->pattern=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp6EE->pat_vars=0;_tmp6EE->where_clause=0;_tmp6EE->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp6EE->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6EE;});_tmp6ED->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmp6ED;});_tmp6EC;});_tmp6EB;});break;case 253: _LL3DD: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp6EF=_cycalloc(sizeof(struct Cyc_SwitchClauseList_tok_struct));
_tmp6EF[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp6F0;_tmp6F0.tag=Cyc_SwitchClauseList_tok;
_tmp6F0.f1=({struct Cyc_List_List*_tmp6F1=_cycalloc(sizeof(struct Cyc_List_List));
_tmp6F1->hd=({struct Cyc_Absyn_Switch_clause*_tmp6F2=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
_tmp6F2->pattern=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp6F2->pat_vars=0;_tmp6F2->where_clause=0;_tmp6F2->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6F2->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp6F2;});
_tmp6F1->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmp6F1;});
_tmp6F0;});_tmp6EF;});break;case 254: _LL3DE: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6F3=_cycalloc(sizeof(struct Cyc_SwitchClauseList_tok_struct));_tmp6F3[0]=({
struct Cyc_SwitchClauseList_tok_struct _tmp6F4;_tmp6F4.tag=Cyc_SwitchClauseList_tok;
_tmp6F4.f1=({struct Cyc_List_List*_tmp6F5=_cycalloc(sizeof(struct Cyc_List_List));
_tmp6F5->hd=({struct Cyc_Absyn_Switch_clause*_tmp6F6=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
_tmp6F6->pattern=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp6F6->pat_vars=0;_tmp6F6->where_clause=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp6F6->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp6F6->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line);_tmp6F6;});_tmp6F5->tl=
Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmp6F5;});_tmp6F4;});_tmp6F3;});
break;case 255: _LL3DF: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6F7=_cycalloc(sizeof(struct Cyc_SwitchClauseList_tok_struct));_tmp6F7[0]=({
struct Cyc_SwitchClauseList_tok_struct _tmp6F8;_tmp6F8.tag=Cyc_SwitchClauseList_tok;
_tmp6F8.f1=({struct Cyc_List_List*_tmp6F9=_cycalloc(sizeof(struct Cyc_List_List));
_tmp6F9->hd=({struct Cyc_Absyn_Switch_clause*_tmp6FA=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
_tmp6FA->pattern=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);_tmp6FA->pat_vars=0;_tmp6FA->where_clause=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp6FA->body=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp6FA->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6FA;});_tmp6F9->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmp6F9;});_tmp6F8;});_tmp6F7;});break;case 256: _LL3E0: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp6FB=_cycalloc(sizeof(struct
Cyc_SwitchCClauseList_tok_struct));_tmp6FB[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmp6FC;_tmp6FC.tag=Cyc_SwitchCClauseList_tok;_tmp6FC.f1=0;_tmp6FC;});_tmp6FB;});
break;case 257: _LL3E1: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmp6FD=_cycalloc(sizeof(struct Cyc_SwitchCClauseList_tok_struct));_tmp6FD[0]=({
struct Cyc_SwitchCClauseList_tok_struct _tmp6FE;_tmp6FE.tag=Cyc_SwitchCClauseList_tok;
_tmp6FE.f1=({struct Cyc_List_List*_tmp6FF=_cycalloc(sizeof(struct Cyc_List_List));
_tmp6FF->hd=({struct Cyc_Absyn_SwitchC_clause*_tmp700=_cycalloc(sizeof(struct Cyc_Absyn_SwitchC_clause));
_tmp700->cnst_exp=0;_tmp700->body=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Absyn_break_stmt(0),0);_tmp700->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp700;});_tmp6FF->tl=0;_tmp6FF;});_tmp6FE;});_tmp6FD;});
break;case 258: _LL3E2: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmp701=_cycalloc(sizeof(struct Cyc_SwitchCClauseList_tok_struct));_tmp701[0]=({
struct Cyc_SwitchCClauseList_tok_struct _tmp702;_tmp702.tag=Cyc_SwitchCClauseList_tok;
_tmp702.f1=({struct Cyc_List_List*_tmp703=_cycalloc(sizeof(struct Cyc_List_List));
_tmp703->hd=({struct Cyc_Absyn_SwitchC_clause*_tmp704=_cycalloc(sizeof(struct Cyc_Absyn_SwitchC_clause));
_tmp704->cnst_exp=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp704->body=Cyc_Absyn_fallthru_stmt(
0,0);_tmp704->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp704;});_tmp703->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmp703;});_tmp702;});_tmp701;});break;case 259: _LL3E3: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp705=_cycalloc(sizeof(struct
Cyc_SwitchCClauseList_tok_struct));_tmp705[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmp706;_tmp706.tag=Cyc_SwitchCClauseList_tok;_tmp706.f1=({struct Cyc_List_List*
_tmp707=_cycalloc(sizeof(struct Cyc_List_List));_tmp707->hd=({struct Cyc_Absyn_SwitchC_clause*
_tmp708=_cycalloc(sizeof(struct Cyc_Absyn_SwitchC_clause));_tmp708->cnst_exp=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp708->body=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Absyn_fallthru_stmt(
0,0),0);_tmp708->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp708;});_tmp707->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmp707;});_tmp706;});_tmp705;});break;case 260: _LL3E4: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp709=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp709[0]=({struct Cyc_Stmt_tok_struct _tmp70A;_tmp70A.tag=Cyc_Stmt_tok;_tmp70A.f1=
Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp70A;});_tmp709;});break;case 261: _LL3E5: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp70B=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp70B[0]=({struct Cyc_Stmt_tok_struct _tmp70C;_tmp70C.tag=Cyc_Stmt_tok;_tmp70C.f1=
Cyc_Absyn_do_stmt(Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp70C;});_tmp70B;});break;case 262: _LL3E6: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp70D=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp70D[0]=({struct Cyc_Stmt_tok_struct _tmp70E;_tmp70E.tag=Cyc_Stmt_tok;_tmp70E.f1=
Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp70E;});_tmp70D;});break;case 263: _LL3E7: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp70F=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp70F[0]=({struct Cyc_Stmt_tok_struct _tmp710;_tmp710.tag=Cyc_Stmt_tok;_tmp710.f1=
Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp710;});_tmp70F;});break;case 264: _LL3E8: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp711=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp711[0]=({struct Cyc_Stmt_tok_struct _tmp712;_tmp712.tag=Cyc_Stmt_tok;_tmp712.f1=
Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp712;});_tmp711;});break;case 265: _LL3E9: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp713=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp713[0]=({struct Cyc_Stmt_tok_struct _tmp714;_tmp714.tag=Cyc_Stmt_tok;_tmp714.f1=
Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 7)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp714;});_tmp713;});break;case 266: _LL3EA: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp715=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp715[0]=({struct Cyc_Stmt_tok_struct _tmp716;_tmp716.tag=Cyc_Stmt_tok;_tmp716.f1=
Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp716;});_tmp715;});break;case 267: _LL3EB: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp717=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp717[0]=({struct Cyc_Stmt_tok_struct _tmp718;_tmp718.tag=Cyc_Stmt_tok;_tmp718.f1=
Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmp718;});
_tmp717;});break;case 268: _LL3EC: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp719=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp719[0]=({struct Cyc_Stmt_tok_struct
_tmp71A;_tmp71A.tag=Cyc_Stmt_tok;_tmp71A.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp71A;});_tmp719;});break;case 269: _LL3ED: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp71B=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp71B[0]=({struct Cyc_Stmt_tok_struct _tmp71C;_tmp71C.tag=Cyc_Stmt_tok;_tmp71C.f1=
Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 6)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp71C;});_tmp71B;});break;case 270: _LL3EE: {struct
Cyc_List_List*_tmp71D=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*_tmp71E=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp71F=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp71F[0]=({struct Cyc_Stmt_tok_struct _tmp720;_tmp720.tag=Cyc_Stmt_tok;_tmp720.f1=
Cyc_Parse_flatten_declarations(_tmp71D,_tmp71E);_tmp720;});_tmp71F;});break;}
case 271: _LL3EF: {struct Cyc_List_List*_tmp721=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp722=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp723=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp723[0]=({struct Cyc_Stmt_tok_struct
_tmp724;_tmp724.tag=Cyc_Stmt_tok;_tmp724.f1=Cyc_Parse_flatten_declarations(
_tmp721,_tmp722);_tmp724;});_tmp723;});break;}case 272: _LL3F0: {struct Cyc_List_List*
_tmp725=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp726=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp727=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp727[0]=({struct Cyc_Stmt_tok_struct
_tmp728;_tmp728.tag=Cyc_Stmt_tok;_tmp728.f1=Cyc_Parse_flatten_declarations(
_tmp725,_tmp726);_tmp728;});_tmp727;});break;}case 273: _LL3F1: {struct Cyc_List_List*
_tmp729=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp72A=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp72B=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp72B[0]=({struct Cyc_Stmt_tok_struct
_tmp72C;_tmp72C.tag=Cyc_Stmt_tok;_tmp72C.f1=Cyc_Parse_flatten_declarations(
_tmp729,_tmp72A);_tmp72C;});_tmp72B;});break;}case 274: _LL3F2: {struct Cyc_List_List*
_tmp72D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_decl2vardecl,Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp72E=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp72E[0]=({struct Cyc_Stmt_tok_struct _tmp72F;_tmp72F.tag=Cyc_Stmt_tok;_tmp72F.f1=
Cyc_Absyn_forarray_stmt(_tmp72D,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 8)]).first_line,(yyls[yylsp_offset]).last_line));_tmp72F;});
_tmp72E;});break;}case 275: _LL3F3: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp730=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp730[0]=({struct Cyc_Stmt_tok_struct
_tmp731;_tmp731.tag=Cyc_Stmt_tok;_tmp731.f1=Cyc_Absyn_goto_stmt(({struct
_tagged_arr*_tmp732=_cycalloc(sizeof(struct _tagged_arr));_tmp732[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp732;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp731;});
_tmp730;});break;case 276: _LL3F4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp733=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp733[0]=({struct Cyc_Stmt_tok_struct
_tmp734;_tmp734.tag=Cyc_Stmt_tok;_tmp734.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp734;});
_tmp733;});break;case 277: _LL3F5: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp735=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp735[0]=({struct Cyc_Stmt_tok_struct
_tmp736;_tmp736.tag=Cyc_Stmt_tok;_tmp736.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp736;});
_tmp735;});break;case 278: _LL3F6: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp737=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp737[0]=({struct Cyc_Stmt_tok_struct
_tmp738;_tmp738.tag=Cyc_Stmt_tok;_tmp738.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp738;});
_tmp737;});break;case 279: _LL3F7: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp739=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp739[0]=({struct Cyc_Stmt_tok_struct
_tmp73A;_tmp73A.tag=Cyc_Stmt_tok;_tmp73A.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp73A;});_tmp739;});break;case 280: _LL3F8: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp73B=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmp73B[0]=({struct Cyc_Stmt_tok_struct _tmp73C;_tmp73C.tag=Cyc_Stmt_tok;_tmp73C.f1=
Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp73C;});
_tmp73B;});break;case 281: _LL3F9: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp73D=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp73D[0]=({struct Cyc_Stmt_tok_struct
_tmp73E;_tmp73E.tag=Cyc_Stmt_tok;_tmp73E.f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));_tmp73E;});
_tmp73D;});break;case 282: _LL3FA: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp73F=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmp73F[0]=({struct Cyc_Stmt_tok_struct
_tmp740;_tmp740.tag=Cyc_Stmt_tok;_tmp740.f1=Cyc_Absyn_fallthru_stmt(Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));_tmp740;});
_tmp73F;});break;case 283: _LL3FB: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp741=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));_tmp741[0]=({struct Cyc_Pattern_tok_struct
_tmp742;_tmp742.tag=Cyc_Pattern_tok;_tmp742.f1=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp742;});
_tmp741;});break;case 284: _LL3FC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 285: _LL3FD: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp743=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));_tmp743[0]=({struct Cyc_Pattern_tok_struct
_tmp744;_tmp744.tag=Cyc_Pattern_tok;_tmp744.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Int_p_struct*_tmp745=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_struct));
_tmp745[0]=({struct Cyc_Absyn_Int_p_struct _tmp746;_tmp746.tag=1;_tmp746.f1=(void*)(*
Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f1;_tmp746.f2=(*Cyc_yyget_Int_tok(yyvs[
yyvsp_offset])).f2;_tmp746;});_tmp745;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp744;});_tmp743;});
break;case 286: _LL3FE: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp747=
_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));_tmp747[0]=({struct Cyc_Pattern_tok_struct
_tmp748;_tmp748.tag=Cyc_Pattern_tok;_tmp748.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Int_p_struct*_tmp749=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_struct));
_tmp749[0]=({struct Cyc_Absyn_Int_p_struct _tmp74A;_tmp74A.tag=1;_tmp74A.f1=(void*)((
void*)0);_tmp74A.f2=-(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2;_tmp74A;});
_tmp749;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp748;});
_tmp747;});break;case 287: _LL3FF: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp74B=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));_tmp74B[0]=({struct Cyc_Pattern_tok_struct
_tmp74C;_tmp74C.tag=Cyc_Pattern_tok;_tmp74C.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Float_p_struct*_tmp74D=_cycalloc(sizeof(struct Cyc_Absyn_Float_p_struct));
_tmp74D[0]=({struct Cyc_Absyn_Float_p_struct _tmp74E;_tmp74E.tag=3;_tmp74E.f1=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp74E;});_tmp74D;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp74C;});_tmp74B;});
break;case 288: _LL400: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp74F=
_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));_tmp74F[0]=({struct Cyc_Pattern_tok_struct
_tmp750;_tmp750.tag=Cyc_Pattern_tok;_tmp750.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Char_p_struct*_tmp751=_cycalloc_atomic(sizeof(struct Cyc_Absyn_Char_p_struct));
_tmp751[0]=({struct Cyc_Absyn_Char_p_struct _tmp752;_tmp752.tag=2;_tmp752.f1=Cyc_yyget_Char_tok(
yyvs[yyvsp_offset]);_tmp752;});_tmp751;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp750;});_tmp74F;});
break;case 289: _LL401: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp753=
_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));_tmp753[0]=({struct Cyc_Pattern_tok_struct
_tmp754;_tmp754.tag=Cyc_Pattern_tok;_tmp754.f1=Cyc_Absyn_new_pat((void*)1,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp754;});
_tmp753;});break;case 290: _LL402: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp755=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));_tmp755[0]=({struct Cyc_Pattern_tok_struct
_tmp756;_tmp756.tag=Cyc_Pattern_tok;_tmp756.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownId_p_struct*_tmp757=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_p_struct));
_tmp757[0]=({struct Cyc_Absyn_UnknownId_p_struct _tmp758;_tmp758.tag=11;_tmp758.f1=
Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);_tmp758;});_tmp757;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp756;});
_tmp755;});break;case 291: _LL403: {struct Cyc_List_List*_tmp759=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp75A=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));
_tmp75A[0]=({struct Cyc_Pattern_tok_struct _tmp75B;_tmp75B.tag=Cyc_Pattern_tok;
_tmp75B.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownCall_p_struct*
_tmp75C=_cycalloc(sizeof(struct Cyc_Absyn_UnknownCall_p_struct));_tmp75C[0]=({
struct Cyc_Absyn_UnknownCall_p_struct _tmp75D;_tmp75D.tag=12;_tmp75D.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp75D.f2=_tmp759;
_tmp75D.f3=Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp75D;});_tmp75C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp75B;});_tmp75A;});break;}case 292: _LL404: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp75E=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));
_tmp75E[0]=({struct Cyc_Pattern_tok_struct _tmp75F;_tmp75F.tag=Cyc_Pattern_tok;
_tmp75F.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp760=
_cycalloc(sizeof(struct Cyc_Absyn_Tuple_p_struct));_tmp760[0]=({struct Cyc_Absyn_Tuple_p_struct
_tmp761;_tmp761.tag=4;_tmp761.f1=Cyc_yyget_PatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp761;});_tmp760;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp75F;});
_tmp75E;});break;case 293: _LL405: {struct Cyc_List_List*_tmp762=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp763=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));
_tmp763[0]=({struct Cyc_Pattern_tok_struct _tmp764;_tmp764.tag=Cyc_Pattern_tok;
_tmp764.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownFields_p_struct*
_tmp765=_cycalloc(sizeof(struct Cyc_Absyn_UnknownFields_p_struct));_tmp765[0]=({
struct Cyc_Absyn_UnknownFields_p_struct _tmp766;_tmp766.tag=13;_tmp766.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp766.f2=_tmp762;
_tmp766.f3=0;_tmp766;});_tmp765;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp764;});_tmp763;});break;}case 294: _LL406: {struct
Cyc_List_List*_tmp767=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp768=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));
_tmp768[0]=({struct Cyc_Pattern_tok_struct _tmp769;_tmp769.tag=Cyc_Pattern_tok;
_tmp769.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownFields_p_struct*
_tmp76A=_cycalloc(sizeof(struct Cyc_Absyn_UnknownFields_p_struct));_tmp76A[0]=({
struct Cyc_Absyn_UnknownFields_p_struct _tmp76B;_tmp76B.tag=13;_tmp76B.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp76B.f2=_tmp767;
_tmp76B.f3=Cyc_yyget_FieldPatternList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp76B;});_tmp76A;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp769;});_tmp768;});break;}case 295: _LL407: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp76C=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));
_tmp76C[0]=({struct Cyc_Pattern_tok_struct _tmp76D;_tmp76D.tag=Cyc_Pattern_tok;
_tmp76D.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp76E=
_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_struct));_tmp76E[0]=({struct Cyc_Absyn_Pointer_p_struct
_tmp76F;_tmp76F.tag=5;_tmp76F.f1=Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);
_tmp76F;});_tmp76E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp76D;});_tmp76C;});break;case 296: _LL408: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp770=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));
_tmp770[0]=({struct Cyc_Pattern_tok_struct _tmp771;_tmp771.tag=Cyc_Pattern_tok;
_tmp771.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp772=
_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_struct));_tmp772[0]=({struct Cyc_Absyn_Reference_p_struct
_tmp773;_tmp773.tag=6;_tmp773.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp774=
_cycalloc(sizeof(struct _tuple1));_tmp774->f1=(void*)0;_tmp774->f2=({struct
_tagged_arr*_tmp775=_cycalloc(sizeof(struct _tagged_arr));_tmp775[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp775;});_tmp774;}),(void*)0,0);_tmp773;});_tmp772;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp771;});_tmp770;});break;case 297: _LL409: yyval=(
void*)({struct Cyc_PatternList_tok_struct*_tmp776=_cycalloc(sizeof(struct Cyc_PatternList_tok_struct));
_tmp776[0]=({struct Cyc_PatternList_tok_struct _tmp777;_tmp777.tag=Cyc_PatternList_tok;
_tmp777.f1=0;_tmp777;});_tmp776;});break;case 298: _LL40A: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp778=_cycalloc(sizeof(struct Cyc_PatternList_tok_struct));_tmp778[0]=({struct
Cyc_PatternList_tok_struct _tmp779;_tmp779.tag=Cyc_PatternList_tok;_tmp779.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(
yyvs[yyvsp_offset]));_tmp779;});_tmp778;});break;case 299: _LL40B: yyval=(void*)({
struct Cyc_PatternList_tok_struct*_tmp77A=_cycalloc(sizeof(struct Cyc_PatternList_tok_struct));
_tmp77A[0]=({struct Cyc_PatternList_tok_struct _tmp77B;_tmp77B.tag=Cyc_PatternList_tok;
_tmp77B.f1=({struct Cyc_List_List*_tmp77C=_cycalloc(sizeof(struct Cyc_List_List));
_tmp77C->hd=Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmp77C->tl=0;_tmp77C;});
_tmp77B;});_tmp77A;});break;case 300: _LL40C: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp77D=_cycalloc(sizeof(struct Cyc_PatternList_tok_struct));_tmp77D[0]=({struct
Cyc_PatternList_tok_struct _tmp77E;_tmp77E.tag=Cyc_PatternList_tok;_tmp77E.f1=({
struct Cyc_List_List*_tmp77F=_cycalloc(sizeof(struct Cyc_List_List));_tmp77F->hd=
Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmp77F->tl=Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp77F;});_tmp77E;});
_tmp77D;});break;case 301: _LL40D: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp780=_cycalloc(sizeof(struct Cyc_FieldPattern_tok_struct));_tmp780[0]=({struct
Cyc_FieldPattern_tok_struct _tmp781;_tmp781.tag=Cyc_FieldPattern_tok;_tmp781.f1=({
struct _tuple13*_tmp782=_cycalloc(sizeof(struct _tuple13));_tmp782->f1=0;_tmp782->f2=
Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmp782;});_tmp781;});_tmp780;});
break;case 302: _LL40E: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*_tmp783=
_cycalloc(sizeof(struct Cyc_FieldPattern_tok_struct));_tmp783[0]=({struct Cyc_FieldPattern_tok_struct
_tmp784;_tmp784.tag=Cyc_FieldPattern_tok;_tmp784.f1=({struct _tuple13*_tmp785=
_cycalloc(sizeof(struct _tuple13));_tmp785->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp785->f2=Cyc_yyget_Pattern_tok(
yyvs[yyvsp_offset]);_tmp785;});_tmp784;});_tmp783;});break;case 303: _LL40F: yyval=(
void*)({struct Cyc_FieldPatternList_tok_struct*_tmp786=_cycalloc(sizeof(struct Cyc_FieldPatternList_tok_struct));
_tmp786[0]=({struct Cyc_FieldPatternList_tok_struct _tmp787;_tmp787.tag=Cyc_FieldPatternList_tok;
_tmp787.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_FieldPatternList_tok(yyvs[yyvsp_offset]));_tmp787;});_tmp786;});break;
case 304: _LL410: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*_tmp788=
_cycalloc(sizeof(struct Cyc_FieldPatternList_tok_struct));_tmp788[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp789;_tmp789.tag=Cyc_FieldPatternList_tok;_tmp789.f1=({struct Cyc_List_List*
_tmp78A=_cycalloc(sizeof(struct Cyc_List_List));_tmp78A->hd=Cyc_yyget_FieldPattern_tok(
yyvs[yyvsp_offset]);_tmp78A->tl=0;_tmp78A;});_tmp789;});_tmp788;});break;case 305:
_LL411: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*_tmp78B=_cycalloc(
sizeof(struct Cyc_FieldPatternList_tok_struct));_tmp78B[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp78C;_tmp78C.tag=Cyc_FieldPatternList_tok;_tmp78C.f1=({struct Cyc_List_List*
_tmp78D=_cycalloc(sizeof(struct Cyc_List_List));_tmp78D->hd=Cyc_yyget_FieldPattern_tok(
yyvs[yyvsp_offset]);_tmp78D->tl=Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp78D;});_tmp78C;});
_tmp78B;});break;case 306: _LL412: yyval=yyvs[yyvsp_offset];break;case 307: _LL413:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp78E=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp78E[0]=({struct Cyc_Exp_tok_struct _tmp78F;_tmp78F.tag=Cyc_Exp_tok;_tmp78F.f1=
Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp78F;});_tmp78E;});break;case 308: _LL414: yyval=yyvs[
yyvsp_offset];break;case 309: _LL415: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp790=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp790[0]=({struct Cyc_Exp_tok_struct
_tmp791;_tmp791.tag=Cyc_Exp_tok;_tmp791.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp791;});_tmp790;});break;case 310: _LL416: yyval=(
void*)({struct Cyc_Primopopt_tok_struct*_tmp792=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));
_tmp792[0]=({struct Cyc_Primopopt_tok_struct _tmp793;_tmp793.tag=Cyc_Primopopt_tok;
_tmp793.f1=0;_tmp793;});_tmp792;});break;case 311: _LL417: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp794=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmp794[0]=({struct Cyc_Primopopt_tok_struct
_tmp795;_tmp795.tag=Cyc_Primopopt_tok;_tmp795.f1=({struct Cyc_Core_Opt*_tmp796=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp796->v=(void*)((void*)1);_tmp796;});
_tmp795;});_tmp794;});break;case 312: _LL418: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp797=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmp797[0]=({struct Cyc_Primopopt_tok_struct
_tmp798;_tmp798.tag=Cyc_Primopopt_tok;_tmp798.f1=({struct Cyc_Core_Opt*_tmp799=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp799->v=(void*)((void*)3);_tmp799;});
_tmp798;});_tmp797;});break;case 313: _LL419: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp79A=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmp79A[0]=({struct Cyc_Primopopt_tok_struct
_tmp79B;_tmp79B.tag=Cyc_Primopopt_tok;_tmp79B.f1=({struct Cyc_Core_Opt*_tmp79C=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp79C->v=(void*)((void*)4);_tmp79C;});
_tmp79B;});_tmp79A;});break;case 314: _LL41A: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp79D=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmp79D[0]=({struct Cyc_Primopopt_tok_struct
_tmp79E;_tmp79E.tag=Cyc_Primopopt_tok;_tmp79E.f1=({struct Cyc_Core_Opt*_tmp79F=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp79F->v=(void*)((void*)0);_tmp79F;});
_tmp79E;});_tmp79D;});break;case 315: _LL41B: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp7A0=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmp7A0[0]=({struct Cyc_Primopopt_tok_struct
_tmp7A1;_tmp7A1.tag=Cyc_Primopopt_tok;_tmp7A1.f1=({struct Cyc_Core_Opt*_tmp7A2=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp7A2->v=(void*)((void*)2);_tmp7A2;});
_tmp7A1;});_tmp7A0;});break;case 316: _LL41C: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp7A3=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmp7A3[0]=({struct Cyc_Primopopt_tok_struct
_tmp7A4;_tmp7A4.tag=Cyc_Primopopt_tok;_tmp7A4.f1=({struct Cyc_Core_Opt*_tmp7A5=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp7A5->v=(void*)((void*)16);_tmp7A5;});
_tmp7A4;});_tmp7A3;});break;case 317: _LL41D: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp7A6=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmp7A6[0]=({struct Cyc_Primopopt_tok_struct
_tmp7A7;_tmp7A7.tag=Cyc_Primopopt_tok;_tmp7A7.f1=({struct Cyc_Core_Opt*_tmp7A8=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp7A8->v=(void*)((void*)17);_tmp7A8;});
_tmp7A7;});_tmp7A6;});break;case 318: _LL41E: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp7A9=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmp7A9[0]=({struct Cyc_Primopopt_tok_struct
_tmp7AA;_tmp7AA.tag=Cyc_Primopopt_tok;_tmp7AA.f1=({struct Cyc_Core_Opt*_tmp7AB=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp7AB->v=(void*)((void*)13);_tmp7AB;});
_tmp7AA;});_tmp7A9;});break;case 319: _LL41F: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp7AC=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmp7AC[0]=({struct Cyc_Primopopt_tok_struct
_tmp7AD;_tmp7AD.tag=Cyc_Primopopt_tok;_tmp7AD.f1=({struct Cyc_Core_Opt*_tmp7AE=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp7AE->v=(void*)((void*)15);_tmp7AE;});
_tmp7AD;});_tmp7AC;});break;case 320: _LL420: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp7AF=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmp7AF[0]=({struct Cyc_Primopopt_tok_struct
_tmp7B0;_tmp7B0.tag=Cyc_Primopopt_tok;_tmp7B0.f1=({struct Cyc_Core_Opt*_tmp7B1=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp7B1->v=(void*)((void*)14);_tmp7B1;});
_tmp7B0;});_tmp7AF;});break;case 321: _LL421: yyval=yyvs[yyvsp_offset];break;case
322: _LL422: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B2=_cycalloc(sizeof(
struct Cyc_Exp_tok_struct));_tmp7B2[0]=({struct Cyc_Exp_tok_struct _tmp7B3;_tmp7B3.tag=
Cyc_Exp_tok;_tmp7B3.f1=Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7B3;});
_tmp7B2;});break;case 323: _LL423: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B4=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7B4[0]=({struct Cyc_Exp_tok_struct
_tmp7B5;_tmp7B5.tag=Cyc_Exp_tok;_tmp7B5.f1=Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7B5;});_tmp7B4;});break;case 324: _LL424: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7B6=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7B6[0]=({struct Cyc_Exp_tok_struct _tmp7B7;_tmp7B7.tag=Cyc_Exp_tok;_tmp7B7.f1=
Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line));_tmp7B7;});
_tmp7B6;});break;case 325: _LL425: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B8=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7B8[0]=({struct Cyc_Exp_tok_struct
_tmp7B9;_tmp7B9.tag=Cyc_Exp_tok;_tmp7B9.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line));_tmp7B9;});
_tmp7B8;});break;case 326: _LL426: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7BA=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7BA[0]=({struct Cyc_Exp_tok_struct
_tmp7BB;_tmp7BB.tag=Cyc_Exp_tok;_tmp7BB.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7BB;});_tmp7BA;});break;case 327: _LL427: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7BC=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7BC[0]=({struct Cyc_Exp_tok_struct _tmp7BD;_tmp7BD.tag=Cyc_Exp_tok;_tmp7BD.f1=
Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7BD;});
_tmp7BC;});break;case 328: _LL428: yyval=yyvs[yyvsp_offset];break;case 329: _LL429:
yyval=yyvs[yyvsp_offset];break;case 330: _LL42A: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7BE=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7BE[0]=({struct Cyc_Exp_tok_struct
_tmp7BF;_tmp7BF.tag=Cyc_Exp_tok;_tmp7BF.f1=Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7BF;});_tmp7BE;});break;case 331: _LL42B: yyval=yyvs[
yyvsp_offset];break;case 332: _LL42C: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7C0=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7C0[0]=({struct Cyc_Exp_tok_struct
_tmp7C1;_tmp7C1.tag=Cyc_Exp_tok;_tmp7C1.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C1;});_tmp7C0;});break;case 333: _LL42D: yyval=yyvs[
yyvsp_offset];break;case 334: _LL42E: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7C2=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7C2[0]=({struct Cyc_Exp_tok_struct
_tmp7C3;_tmp7C3.tag=Cyc_Exp_tok;_tmp7C3.f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C3;});_tmp7C2;});break;case 335: _LL42F: yyval=yyvs[
yyvsp_offset];break;case 336: _LL430: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7C4=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7C4[0]=({struct Cyc_Exp_tok_struct
_tmp7C5;_tmp7C5.tag=Cyc_Exp_tok;_tmp7C5.f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C5;});_tmp7C4;});break;case 337: _LL431: yyval=yyvs[
yyvsp_offset];break;case 338: _LL432: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7C6=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7C6[0]=({struct Cyc_Exp_tok_struct
_tmp7C7;_tmp7C7.tag=Cyc_Exp_tok;_tmp7C7.f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C7;});_tmp7C6;});break;case 339: _LL433: yyval=yyvs[
yyvsp_offset];break;case 340: _LL434: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7C8=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7C8[0]=({struct Cyc_Exp_tok_struct
_tmp7C9;_tmp7C9.tag=Cyc_Exp_tok;_tmp7C9.f1=Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C9;});_tmp7C8;});break;case 341: _LL435: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7CA=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7CA[0]=({struct Cyc_Exp_tok_struct _tmp7CB;_tmp7CB.tag=Cyc_Exp_tok;_tmp7CB.f1=
Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7CB;});_tmp7CA;});break;case 342: _LL436: yyval=yyvs[
yyvsp_offset];break;case 343: _LL437: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7CC=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7CC[0]=({struct Cyc_Exp_tok_struct
_tmp7CD;_tmp7CD.tag=Cyc_Exp_tok;_tmp7CD.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7CD;});_tmp7CC;});break;case 344: _LL438: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7CE=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7CE[0]=({struct Cyc_Exp_tok_struct _tmp7CF;_tmp7CF.tag=Cyc_Exp_tok;_tmp7CF.f1=
Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7CF;});_tmp7CE;});break;case 345: _LL439: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7D0=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7D0[0]=({struct Cyc_Exp_tok_struct _tmp7D1;_tmp7D1.tag=Cyc_Exp_tok;_tmp7D1.f1=
Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7D1;});_tmp7D0;});break;case 346: _LL43A: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7D2=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7D2[0]=({struct Cyc_Exp_tok_struct _tmp7D3;_tmp7D3.tag=Cyc_Exp_tok;_tmp7D3.f1=
Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7D3;});_tmp7D2;});break;case 347: _LL43B: yyval=yyvs[
yyvsp_offset];break;case 348: _LL43C: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7D4=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7D4[0]=({struct Cyc_Exp_tok_struct
_tmp7D5;_tmp7D5.tag=Cyc_Exp_tok;_tmp7D5.f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7D5;});_tmp7D4;});break;case 349: _LL43D: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7D6=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7D6[0]=({struct Cyc_Exp_tok_struct _tmp7D7;_tmp7D7.tag=Cyc_Exp_tok;_tmp7D7.f1=
Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7D7;});
_tmp7D6;});break;case 350: _LL43E: yyval=yyvs[yyvsp_offset];break;case 351: _LL43F:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7D8=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7D8[0]=({struct Cyc_Exp_tok_struct _tmp7D9;_tmp7D9.tag=Cyc_Exp_tok;_tmp7D9.f1=
Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7D9;});
_tmp7D8;});break;case 352: _LL440: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7DA=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7DA[0]=({struct Cyc_Exp_tok_struct
_tmp7DB;_tmp7DB.tag=Cyc_Exp_tok;_tmp7DB.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7DB;});_tmp7DA;});break;case 353: _LL441: yyval=yyvs[
yyvsp_offset];break;case 354: _LL442: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7DC=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7DC[0]=({struct Cyc_Exp_tok_struct
_tmp7DD;_tmp7DD.tag=Cyc_Exp_tok;_tmp7DD.f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7DD;});_tmp7DC;});break;case 355: _LL443: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7DE=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7DE[0]=({struct Cyc_Exp_tok_struct _tmp7DF;_tmp7DF.tag=Cyc_Exp_tok;_tmp7DF.f1=
Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7DF;});
_tmp7DE;});break;case 356: _LL444: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7E0=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7E0[0]=({struct Cyc_Exp_tok_struct
_tmp7E1;_tmp7E1.tag=Cyc_Exp_tok;_tmp7E1.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7E1;});_tmp7E0;});break;case 357: _LL445: yyval=yyvs[
yyvsp_offset];break;case 358: _LL446: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7E2=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7E2[0]=({struct Cyc_Exp_tok_struct
_tmp7E3;_tmp7E3.tag=Cyc_Exp_tok;_tmp7E3.f1=Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7E3;});_tmp7E2;});break;case 359: _LL447: yyval=yyvs[
yyvsp_offset];break;case 360: _LL448: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7E4=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7E4[0]=({struct Cyc_Exp_tok_struct
_tmp7E5;_tmp7E5.tag=Cyc_Exp_tok;_tmp7E5.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7E5;});_tmp7E4;});break;case 361: _LL449: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7E6=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7E6[0]=({struct Cyc_Exp_tok_struct _tmp7E7;_tmp7E7.tag=Cyc_Exp_tok;_tmp7E7.f1=
Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7E7;});_tmp7E6;});break;case 362: _LL44A: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7E8=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7E8[0]=({struct Cyc_Exp_tok_struct _tmp7E9;_tmp7E9.tag=Cyc_Exp_tok;_tmp7E9.f1=
Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7E9;});_tmp7E8;});break;case 363: _LL44B: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7EA=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7EA[0]=({struct Cyc_Exp_tok_struct _tmp7EB;_tmp7EB.tag=Cyc_Exp_tok;_tmp7EB.f1=
Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7EB;});_tmp7EA;});break;case 364: _LL44C: yyval=yyvs[
yyvsp_offset];break;case 365: _LL44D: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7EC=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7EC[0]=({struct Cyc_Exp_tok_struct
_tmp7ED;_tmp7ED.tag=Cyc_Exp_tok;_tmp7ED.f1=Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7ED;});_tmp7EC;});break;case 366: _LL44E: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7EE=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7EE[0]=({struct Cyc_Exp_tok_struct _tmp7EF;_tmp7EF.tag=Cyc_Exp_tok;_tmp7EF.f1=
Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7EF;});_tmp7EE;});break;case 367: _LL44F: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7F0=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7F0[0]=({struct Cyc_Exp_tok_struct _tmp7F1;_tmp7F1.tag=Cyc_Exp_tok;_tmp7F1.f1=
Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F1;});_tmp7F0;});break;case 368: _LL450: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7F2=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7F2[0]=({struct Cyc_Exp_tok_struct _tmp7F3;_tmp7F3.tag=Cyc_Exp_tok;_tmp7F3.f1=
Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp7F4=_cycalloc(sizeof(struct Cyc_Absyn_StructField_struct));_tmp7F4[0]=({
struct Cyc_Absyn_StructField_struct _tmp7F5;_tmp7F5.tag=0;_tmp7F5.f1=({struct
_tagged_arr*_tmp7F6=_cycalloc(sizeof(struct _tagged_arr));_tmp7F6[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7F6;});_tmp7F5;});
_tmp7F4;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7F3;});
_tmp7F2;});break;case 369: _LL451: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7F7=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp7F7[0]=({struct Cyc_Exp_tok_struct
_tmp7F8;_tmp7F8.tag=Cyc_Exp_tok;_tmp7F8.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct
Cyc_Absyn_TupleIndex_struct*_tmp7F9=_cycalloc_atomic(sizeof(struct Cyc_Absyn_TupleIndex_struct));
_tmp7F9[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp7FA;_tmp7FA.tag=1;_tmp7FA.f1=(
unsigned int)(*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f2;_tmp7FA;});_tmp7F9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F8;});_tmp7F7;});break;case 370: _LL452: {struct
Cyc_Position_Segment*_tmp7FB=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);struct Cyc_List_List*_tmp7FC=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp7FB,Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_Exp_tok_struct*_tmp7FD=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7FD[0]=({struct Cyc_Exp_tok_struct _tmp7FE;_tmp7FE.tag=Cyc_Exp_tok;_tmp7FE.f1=
Cyc_Absyn_gentyp_exp(_tmp7FC,(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7FE;});_tmp7FD;});break;}case 371: _LL453: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7FF=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp7FF[0]=({struct Cyc_Exp_tok_struct _tmp800;_tmp800.tag=Cyc_Exp_tok;_tmp800.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp801=_cycalloc(
sizeof(struct Cyc_Absyn_Malloc_e_struct));_tmp801[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp802;_tmp802.tag=33;_tmp802.f1=({struct Cyc_Absyn_MallocInfo _tmp803;_tmp803.is_calloc=
0;_tmp803.rgn=0;_tmp803.elt_type=0;_tmp803.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp803.fat_result=0;
_tmp803;});_tmp802;});_tmp801;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp800;});_tmp7FF;});break;case 372: _LL454: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp804=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp804[0]=({struct Cyc_Exp_tok_struct _tmp805;_tmp805.tag=Cyc_Exp_tok;_tmp805.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp806=_cycalloc(
sizeof(struct Cyc_Absyn_Malloc_e_struct));_tmp806[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp807;_tmp807.tag=33;_tmp807.f1=({struct Cyc_Absyn_MallocInfo _tmp808;_tmp808.is_calloc=
0;_tmp808.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp808.elt_type=0;
_tmp808.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp808.fat_result=0;_tmp808;});_tmp807;});_tmp806;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp805;});_tmp804;});break;case 373: _LL455: {void*
_tmp80A;struct _tuple2 _tmp809=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_LL458: _tmp80A=_tmp809.f3;
goto _LL457;_LL457: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp80B=_cycalloc(
sizeof(struct Cyc_Exp_tok_struct));_tmp80B[0]=({struct Cyc_Exp_tok_struct _tmp80C;
_tmp80C.tag=Cyc_Exp_tok;_tmp80C.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp80D=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_struct));_tmp80D[0]=({struct
Cyc_Absyn_Malloc_e_struct _tmp80E;_tmp80E.tag=33;_tmp80E.f1=({struct Cyc_Absyn_MallocInfo
_tmp80F;_tmp80F.is_calloc=1;_tmp80F.rgn=0;_tmp80F.elt_type=({void**_tmp810=
_cycalloc(sizeof(void*));_tmp810[0]=_tmp80A;_tmp810;});_tmp80F.num_elts=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp80F.fat_result=
0;_tmp80F;});_tmp80E;});_tmp80D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp80C;});_tmp80B;});break;}case 374: _LL456: {void*
_tmp812;struct _tuple2 _tmp811=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_LL45B: _tmp812=_tmp811.f3;
goto _LL45A;_LL45A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp813=_cycalloc(
sizeof(struct Cyc_Exp_tok_struct));_tmp813[0]=({struct Cyc_Exp_tok_struct _tmp814;
_tmp814.tag=Cyc_Exp_tok;_tmp814.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp815=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_struct));_tmp815[0]=({struct
Cyc_Absyn_Malloc_e_struct _tmp816;_tmp816.tag=33;_tmp816.f1=({struct Cyc_Absyn_MallocInfo
_tmp817;_tmp817.is_calloc=1;_tmp817.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp817.elt_type=({
void**_tmp818=_cycalloc(sizeof(void*));_tmp818[0]=_tmp812;_tmp818;});_tmp817.num_elts=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp817.fat_result=0;_tmp817;});_tmp816;});_tmp815;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp814;});_tmp813;});break;}case 375: _LL459: yyval=(
void*)({struct Cyc_Primop_tok_struct*_tmp819=_cycalloc(sizeof(struct Cyc_Primop_tok_struct));
_tmp819[0]=({struct Cyc_Primop_tok_struct _tmp81A;_tmp81A.tag=Cyc_Primop_tok;
_tmp81A.f1=(void*)((void*)12);_tmp81A;});_tmp819;});break;case 376: _LL45C: yyval=(
void*)({struct Cyc_Primop_tok_struct*_tmp81B=_cycalloc(sizeof(struct Cyc_Primop_tok_struct));
_tmp81B[0]=({struct Cyc_Primop_tok_struct _tmp81C;_tmp81C.tag=Cyc_Primop_tok;
_tmp81C.f1=(void*)((void*)11);_tmp81C;});_tmp81B;});break;case 377: _LL45D: yyval=(
void*)({struct Cyc_Primop_tok_struct*_tmp81D=_cycalloc(sizeof(struct Cyc_Primop_tok_struct));
_tmp81D[0]=({struct Cyc_Primop_tok_struct _tmp81E;_tmp81E.tag=Cyc_Primop_tok;
_tmp81E.f1=(void*)((void*)2);_tmp81E;});_tmp81D;});break;case 378: _LL45E: yyval=
yyvs[yyvsp_offset];break;case 379: _LL45F: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp81F=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp81F[0]=({struct Cyc_Exp_tok_struct
_tmp820;_tmp820.tag=Cyc_Exp_tok;_tmp820.f1=Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp820;});_tmp81F;});break;case 380: _LL460: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp821=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp821[0]=({struct Cyc_Exp_tok_struct _tmp822;_tmp822.tag=Cyc_Exp_tok;_tmp822.f1=
Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp822;});_tmp821;});break;case 381: _LL461: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp823=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp823[0]=({struct Cyc_Exp_tok_struct _tmp824;_tmp824.tag=Cyc_Exp_tok;_tmp824.f1=
Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp824;});_tmp823;});break;case 382: _LL462: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp825=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp825[0]=({struct Cyc_Exp_tok_struct _tmp826;_tmp826.tag=Cyc_Exp_tok;_tmp826.f1=
Cyc_Absyn_structmember_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),({struct _tagged_arr*_tmp827=_cycalloc(sizeof(struct
_tagged_arr));_tmp827[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp827;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp826;});_tmp825;});break;case 383: _LL463: {struct
_tuple1*q=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);if(Cyc_Absyn_is_qvar_qualified(
q)){Cyc_Parse_err(_tag_arr("struct field name is qualified",sizeof(unsigned char),
31),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp828=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp828[0]=({struct Cyc_Exp_tok_struct
_tmp829;_tmp829.tag=Cyc_Exp_tok;_tmp829.f1=Cyc_Absyn_structmember_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(*q).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp829;});_tmp828;});break;}case 384: _LL464: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp82A=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp82A[0]=({struct Cyc_Exp_tok_struct _tmp82B;_tmp82B.tag=Cyc_Exp_tok;_tmp82B.f1=
Cyc_Absyn_structarrow_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),({struct _tagged_arr*_tmp82C=_cycalloc(sizeof(struct
_tagged_arr));_tmp82C[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp82C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp82B;});_tmp82A;});break;case 385: _LL465: {struct
_tuple1*q=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);if(Cyc_Absyn_is_qvar_qualified(
q)){Cyc_Parse_err(_tag_arr("struct field name is qualified",sizeof(unsigned char),
31),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp82D=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp82D[0]=({struct Cyc_Exp_tok_struct
_tmp82E;_tmp82E.tag=Cyc_Exp_tok;_tmp82E.f1=Cyc_Absyn_structarrow_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(*q).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp82E;});_tmp82D;});break;}case 386: _LL466: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp82F=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp82F[0]=({struct Cyc_Exp_tok_struct _tmp830;_tmp830.tag=Cyc_Exp_tok;_tmp830.f1=
Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp830;});_tmp82F;});break;case 387: _LL467: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp831=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp831[0]=({struct Cyc_Exp_tok_struct _tmp832;_tmp832.tag=Cyc_Exp_tok;_tmp832.f1=
Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp832;});_tmp831;});break;case 388: _LL468: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp833=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp833[0]=({struct Cyc_Exp_tok_struct _tmp834;_tmp834.tag=Cyc_Exp_tok;_tmp834.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp835=
_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_struct));_tmp835[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmp836;_tmp836.tag=25;_tmp836.f1=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp836.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp836;});_tmp835;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));_tmp834;});
_tmp833;});break;case 389: _LL469: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp837=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp837[0]=({struct Cyc_Exp_tok_struct
_tmp838;_tmp838.tag=Cyc_Exp_tok;_tmp838.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmp839=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_struct));_tmp839[0]=({
struct Cyc_Absyn_CompoundLit_e_struct _tmp83A;_tmp83A.tag=25;_tmp83A.f1=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp83A.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp83A;});_tmp839;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[yylsp_offset]).last_line));_tmp838;});
_tmp837;});break;case 390: _LL46A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp83B=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp83B[0]=({struct Cyc_Exp_tok_struct
_tmp83C;_tmp83C.tag=Cyc_Exp_tok;_tmp83C.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Fill_e_struct*
_tmp83D=_cycalloc(sizeof(struct Cyc_Absyn_Fill_e_struct));_tmp83D[0]=({struct Cyc_Absyn_Fill_e_struct
_tmp83E;_tmp83E.tag=37;_tmp83E.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp83E;});_tmp83D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp83C;});
_tmp83B;});break;case 391: _LL46B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp83F=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp83F[0]=({struct Cyc_Exp_tok_struct
_tmp840;_tmp840.tag=Cyc_Exp_tok;_tmp840.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Codegen_e_struct*
_tmp841=_cycalloc(sizeof(struct Cyc_Absyn_Codegen_e_struct));_tmp841[0]=({struct
Cyc_Absyn_Codegen_e_struct _tmp842;_tmp842.tag=36;_tmp842.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp842;});_tmp841;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp840;});
_tmp83F;});break;case 392: _LL46C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp843=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp843[0]=({struct Cyc_Exp_tok_struct
_tmp844;_tmp844.tag=Cyc_Exp_tok;_tmp844.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp845=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_e_struct));_tmp845[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp846;_tmp846.tag=2;_tmp846.f1=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp846;});_tmp845;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp844;});_tmp843;});
break;case 393: _LL46D: yyval=yyvs[yyvsp_offset];break;case 394: _LL46E: yyval=(void*)({
struct Cyc_Exp_tok_struct*_tmp847=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp847[0]=({struct Cyc_Exp_tok_struct _tmp848;_tmp848.tag=Cyc_Exp_tok;_tmp848.f1=
Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp848;});
_tmp847;});break;case 395: _LL46F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 396: _LL470: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp849=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp849[0]=({struct Cyc_Exp_tok_struct
_tmp84A;_tmp84A.tag=Cyc_Exp_tok;_tmp84A.f1=Cyc_Absyn_noinstantiate_exp(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp84B=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_e_struct));
_tmp84B[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp84C;_tmp84C.tag=2;_tmp84C.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp84C;});_tmp84B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp84A;});_tmp849;});break;case 397: _LL471: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp84D=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp84D[0]=({struct Cyc_Exp_tok_struct _tmp84E;_tmp84E.tag=Cyc_Exp_tok;_tmp84E.f1=
Cyc_Absyn_instantiate_exp(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp84F=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_e_struct));_tmp84F[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp850;_tmp850.tag=2;_tmp850.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp850;});_tmp84F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line)),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp84E;});_tmp84D;});break;case 398: _LL472: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp851=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp851[0]=({struct Cyc_Exp_tok_struct _tmp852;_tmp852.tag=Cyc_Exp_tok;_tmp852.f1=
Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp852;});_tmp851;});break;case 399: _LL473: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp853=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp853[0]=({struct Cyc_Exp_tok_struct _tmp854;_tmp854.tag=Cyc_Exp_tok;_tmp854.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Struct_e_struct*_tmp855=_cycalloc(
sizeof(struct Cyc_Absyn_Struct_e_struct));_tmp855[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp856;_tmp856.tag=28;_tmp856.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp856.f2=0;_tmp856.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp856.f4=0;
_tmp856;});_tmp855;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp854;});_tmp853;});break;case 400: _LL474: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp857=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp857[0]=({struct Cyc_Exp_tok_struct _tmp858;_tmp858.tag=Cyc_Exp_tok;_tmp858.f1=
Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp858;});_tmp857;});break;case 401: _LL475: yyval=(
void*)({struct Cyc_ExpList_tok_struct*_tmp859=_cycalloc(sizeof(struct Cyc_ExpList_tok_struct));
_tmp859[0]=({struct Cyc_ExpList_tok_struct _tmp85A;_tmp85A.tag=Cyc_ExpList_tok;
_tmp85A.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(yyvs[yyvsp_offset]));_tmp85A;});_tmp859;});break;case 402:
_LL476: yyval=(void*)({struct Cyc_ExpList_tok_struct*_tmp85B=_cycalloc(sizeof(
struct Cyc_ExpList_tok_struct));_tmp85B[0]=({struct Cyc_ExpList_tok_struct _tmp85C;
_tmp85C.tag=Cyc_ExpList_tok;_tmp85C.f1=({struct Cyc_List_List*_tmp85D=_cycalloc(
sizeof(struct Cyc_List_List));_tmp85D->hd=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);
_tmp85D->tl=0;_tmp85D;});_tmp85C;});_tmp85B;});break;case 403: _LL477: yyval=(void*)({
struct Cyc_ExpList_tok_struct*_tmp85E=_cycalloc(sizeof(struct Cyc_ExpList_tok_struct));
_tmp85E[0]=({struct Cyc_ExpList_tok_struct _tmp85F;_tmp85F.tag=Cyc_ExpList_tok;
_tmp85F.f1=({struct Cyc_List_List*_tmp860=_cycalloc(sizeof(struct Cyc_List_List));
_tmp860->hd=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp860->tl=Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp860;});_tmp85F;});
_tmp85E;});break;case 404: _LL478: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp861=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp861[0]=({struct Cyc_Exp_tok_struct
_tmp862;_tmp862.tag=Cyc_Exp_tok;_tmp862.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(
yyvs[yyvsp_offset])).f1,(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp862;});
_tmp861;});break;case 405: _LL479: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp863=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp863[0]=({struct Cyc_Exp_tok_struct
_tmp864;_tmp864.tag=Cyc_Exp_tok;_tmp864.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp864;});_tmp863;});break;case 406: _LL47A: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp865=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp865[0]=({struct Cyc_Exp_tok_struct _tmp866;_tmp866.tag=Cyc_Exp_tok;_tmp866.f1=
Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp866;});
_tmp865;});break;case 407: _LL47B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp867=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp867[0]=({struct Cyc_Exp_tok_struct
_tmp868;_tmp868.tag=Cyc_Exp_tok;_tmp868.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp868;});
_tmp867;});break;case 408: _LL47C: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp869=_cycalloc(sizeof(struct Cyc_QualId_tok_struct));_tmp869[0]=({struct Cyc_QualId_tok_struct
_tmp86A;_tmp86A.tag=Cyc_QualId_tok;_tmp86A.f1=({struct _tuple1*_tmp86B=_cycalloc(
sizeof(struct _tuple1));_tmp86B->f1=Cyc_Absyn_rel_ns_null;_tmp86B->f2=({struct
_tagged_arr*_tmp86C=_cycalloc(sizeof(struct _tagged_arr));_tmp86C[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp86C;});_tmp86B;});_tmp86A;});_tmp869;});break;case 409:
_LL47D: yyval=yyvs[yyvsp_offset];break;default: _LL47E: break;}yyvsp_offset -=yylen;
yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=yyval;yylsp_offset ++;if(yylen == 0){(yyls[yylsp_offset]).first_line=
Cyc_yylloc.first_line;(yyls[yylsp_offset]).first_column=Cyc_yylloc.first_column;(
yyls[yylsp_offset]).last_line=(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line;(yyls[yylsp_offset]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
yylsp_offset]).last_line=(yyls[_check_known_subscript_notnull(10000,(
yylsp_offset + yylen)- 1)]).last_line;(yyls[yylsp_offset]).last_column=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_column;}
yyn=(int)Cyc_yyr1[yyn];yystate=Cyc_yypgoto[_check_known_subscript_notnull(112,
yyn - 125)]+ yyss[yyssp_offset];if((yystate >= 0? yystate <= 4995: 0)? Cyc_yycheck[
yystate]== yyss[yyssp_offset]: 0){yystate=(int)Cyc_yytable[yystate];}else{yystate=(
int)Cyc_yydefgoto[_check_known_subscript_notnull(112,yyn - 125)];}goto yynewstate;
yyerrlab: if(yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[yystate];if(yyn > 
- 32768? yyn < 4995: 0){int sze=0;struct _tagged_arr msg;int x;int count;count=0;for(x=
yyn < 0? - yyn: 0;x < 237 / sizeof(unsigned char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(4996,x + yyn)]== x){(sze +=Cyc_Std_strlen(Cyc_yytname[
x])+ 15,count ++);}}msg=({unsigned int _tmp86D=(unsigned int)(sze + 15);
unsigned char*_tmp86E=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmp86D));struct _tagged_arr _tmp870=_tag_arr(_tmp86E,sizeof(
unsigned char),(unsigned int)(sze + 15));{unsigned int _tmp86F=_tmp86D;
unsigned int i;for(i=0;i < _tmp86F;i ++){_tmp86E[i]='\000';}};_tmp870;});Cyc_Std_strcpy(
msg,_tag_arr("parse error",sizeof(unsigned char),12));if(count < 5){count=0;for(x=
yyn < 0? - yyn: 0;x < 237 / sizeof(unsigned char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(4996,x + yyn)]== x){Cyc_Std_strcat(msg,count == 0?
_tag_arr(", expecting `",sizeof(unsigned char),14): _tag_arr(" or `",sizeof(
unsigned char),6));Cyc_Std_strcat(msg,Cyc_yytname[x]);Cyc_Std_strcat(msg,
_tag_arr("'",sizeof(unsigned char),2));count ++;}}}Cyc_yyerror((struct _tagged_arr)
msg);}else{Cyc_yyerror(_tag_arr("parse error",sizeof(unsigned char),12));}}goto
yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){return 1;}Cyc_yychar=-
2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0){
return 1;}yyvsp_offset --;yystate=(int)yyss[_check_known_subscript_notnull(10000,
-- yyssp_offset)];yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[yystate];if(yyn
== - 32768){goto yyerrdefault;}yyn +=1;if((yyn < 0? 1: yyn > 4995)? 1: Cyc_yycheck[yyn]!= 
1){goto yyerrdefault;}yyn=(int)Cyc_yytable[yyn];if(yyn < 0){if(yyn == - 32768){goto
yyerrpop;}yyn=- yyn;goto yyreduce;}else{if(yyn == 0){goto yyerrpop;}}if(yyn == 830){
return 0;}yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;
yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto
yynewstate;}void Cyc_yyprint(int i,void*v){void*_tmp87A=v;struct _tuple15*_tmp87B;
struct _tuple15 _tmp87C;int _tmp87D;unsigned char _tmp87E;short _tmp87F;struct
_tagged_arr _tmp880;struct Cyc_Core_Opt*_tmp881;struct Cyc_Core_Opt*_tmp882;struct
Cyc_Core_Opt _tmp883;struct _tagged_arr*_tmp884;struct _tuple1*_tmp885;struct
_tuple1 _tmp886;struct _tagged_arr*_tmp887;void*_tmp888;_LL481: if(*((void**)
_tmp87A)== Cyc_Int_tok){_LL491: _tmp87B=((struct Cyc_Int_tok_struct*)_tmp87A)->f1;
_tmp87C=*_tmp87B;_LL492: _tmp87D=_tmp87C.f2;goto _LL482;}else{goto _LL483;}_LL483:
if(*((void**)_tmp87A)== Cyc_Char_tok){_LL493: _tmp87E=((struct Cyc_Char_tok_struct*)
_tmp87A)->f1;goto _LL484;}else{goto _LL485;}_LL485: if(*((void**)_tmp87A)== Cyc_Short_tok){
_LL494: _tmp87F=((struct Cyc_Short_tok_struct*)_tmp87A)->f1;goto _LL486;}else{goto
_LL487;}_LL487: if(*((void**)_tmp87A)== Cyc_String_tok){_LL495: _tmp880=((struct
Cyc_String_tok_struct*)_tmp87A)->f1;goto _LL488;}else{goto _LL489;}_LL489: if(*((
void**)_tmp87A)== Cyc_Stringopt_tok){_LL496: _tmp881=((struct Cyc_Stringopt_tok_struct*)
_tmp87A)->f1;if(_tmp881 == 0){goto _LL48A;}else{goto _LL48B;}}else{goto _LL48B;}
_LL48B: if(*((void**)_tmp87A)== Cyc_Stringopt_tok){_LL497: _tmp882=((struct Cyc_Stringopt_tok_struct*)
_tmp87A)->f1;if(_tmp882 == 0){goto _LL48D;}else{_tmp883=*_tmp882;_LL498: _tmp884=(
struct _tagged_arr*)_tmp883.v;goto _LL48C;}}else{goto _LL48D;}_LL48D: if(*((void**)
_tmp87A)== Cyc_QualId_tok){_LL499: _tmp885=((struct Cyc_QualId_tok_struct*)_tmp87A)->f1;
_tmp886=*_tmp885;_LL49B: _tmp888=_tmp886.f1;goto _LL49A;_LL49A: _tmp887=_tmp886.f2;
goto _LL48E;}else{goto _LL48F;}_LL48F: goto _LL490;_LL482:({struct Cyc_Std_Int_pa_struct
_tmp88A;_tmp88A.tag=1;_tmp88A.f1=(int)((unsigned int)_tmp87D);{void*_tmp889[1]={&
_tmp88A};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%d",sizeof(unsigned char),3),
_tag_arr(_tmp889,sizeof(void*),1));}});goto _LL480;_LL484:({struct Cyc_Std_Int_pa_struct
_tmp88C;_tmp88C.tag=1;_tmp88C.f1=(int)((unsigned int)((int)_tmp87E));{void*
_tmp88B[1]={& _tmp88C};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%c",sizeof(
unsigned char),3),_tag_arr(_tmp88B,sizeof(void*),1));}});goto _LL480;_LL486:({
struct Cyc_Std_Int_pa_struct _tmp88E;_tmp88E.tag=1;_tmp88E.f1=(int)((unsigned int)((
int)_tmp87F));{void*_tmp88D[1]={& _tmp88E};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%ds",sizeof(unsigned char),4),_tag_arr(_tmp88D,sizeof(void*),1));}});
goto _LL480;_LL488:({struct Cyc_Std_String_pa_struct _tmp890;_tmp890.tag=0;_tmp890.f1=(
struct _tagged_arr)_tmp880;{void*_tmp88F[1]={& _tmp890};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\"%s\"",sizeof(unsigned char),5),_tag_arr(_tmp88F,sizeof(void*),1));}});
goto _LL480;_LL48A:({void*_tmp891[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("NULL",
sizeof(unsigned char),5),_tag_arr(_tmp891,sizeof(void*),0));});goto _LL480;_LL48C:({
struct Cyc_Std_String_pa_struct _tmp893;_tmp893.tag=0;_tmp893.f1=(struct
_tagged_arr)*_tmp884;{void*_tmp892[1]={& _tmp893};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\"%s\"",sizeof(unsigned char),5),_tag_arr(_tmp892,sizeof(void*),1));}});
goto _LL480;_LL48E: {struct Cyc_List_List*_tmp894=0;{void*_tmp895=_tmp888;struct
Cyc_List_List*_tmp896;struct Cyc_List_List*_tmp897;_LL49D: if((unsigned int)
_tmp895 > 1?*((int*)_tmp895)== 0: 0){_LL4A3: _tmp896=((struct Cyc_Absyn_Rel_n_struct*)
_tmp895)->f1;goto _LL49E;}else{goto _LL49F;}_LL49F: if((unsigned int)_tmp895 > 1?*((
int*)_tmp895)== 1: 0){_LL4A4: _tmp897=((struct Cyc_Absyn_Abs_n_struct*)_tmp895)->f1;
goto _LL4A0;}else{goto _LL4A1;}_LL4A1: if((int)_tmp895 == 0){goto _LL4A2;}else{goto
_LL49C;}_LL49E: _tmp894=_tmp896;goto _LL49C;_LL4A0: _tmp894=_tmp897;goto _LL49C;
_LL4A2: goto _LL49C;_LL49C:;}for(0;_tmp894 != 0;_tmp894=_tmp894->tl){({struct Cyc_Std_String_pa_struct
_tmp899;_tmp899.tag=0;_tmp899.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp894->hd);{void*_tmp898[1]={& _tmp899};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",
sizeof(unsigned char),5),_tag_arr(_tmp898,sizeof(void*),1));}});}({struct Cyc_Std_String_pa_struct
_tmp89B;_tmp89B.tag=0;_tmp89B.f1=(struct _tagged_arr)*_tmp887;{void*_tmp89A[1]={&
_tmp89B};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",sizeof(unsigned char),5),
_tag_arr(_tmp89A,sizeof(void*),1));}});goto _LL480;}_LL490:({void*_tmp89C[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("?",sizeof(unsigned char),2),_tag_arr(
_tmp89C,sizeof(void*),0));});goto _LL480;_LL480:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc_Std___sFILE*f){Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*
_tmp89D=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp89D->v=Cyc_Lexing_from_file(f);
_tmp89D;});Cyc_yyparse();return Cyc_Parse_parse_result;}
