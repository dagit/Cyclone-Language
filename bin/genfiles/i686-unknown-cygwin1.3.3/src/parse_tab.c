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
unsigned char*tag;struct _tagged_arr f1;};static const int Cyc_Std_String_pa=0;struct
Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Std_Int_pa=
1;struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};static const int Cyc_Std_Double_pa=
2;struct Cyc_Std_Double_pa_struct{int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=
3;struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=
4;struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int Cyc_Std_fprintf(
struct Cyc_Std___sFILE*,struct _tagged_arr fmt,struct _tagged_arr);extern struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);static const
int Cyc_Std_ShortPtr_sa=0;struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};
static const int Cyc_Std_UShortPtr_sa=1;struct Cyc_Std_UShortPtr_sa_struct{int tag;
unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=2;struct Cyc_Std_IntPtr_sa_struct{
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
Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc_Std___sFILE*);struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern struct Cyc_List_List*Cyc_List_map(void*(*f)(
void*),struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(
void*,void*),void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);extern void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern void*Cyc_List_fold_right(
void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);extern struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern struct Cyc_List_List*Cyc_List_flatten(
struct Cyc_List_List*x);extern unsigned char Cyc_List_Nth[8];extern struct Cyc_List_List*
Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct
Cyc_List_List*f1;struct Cyc_List_List*f2;};extern struct _tuple0 Cyc_List_split(
struct Cyc_List_List*x);extern unsigned int Cyc_Std_strlen(struct _tagged_arr s);
extern int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);extern int Cyc_Std_zstrcmp(
struct _tagged_arr,struct _tagged_arr);extern int Cyc_Std_zstrptrcmp(struct
_tagged_arr*,struct _tagged_arr*);extern struct _tagged_arr Cyc_Std_strcat(struct
_tagged_arr dest,struct _tagged_arr src);extern struct _tagged_arr Cyc_Std_strcpy(
struct _tagged_arr dest,struct _tagged_arr src);extern struct _tagged_arr Cyc_Std_substring(
struct _tagged_arr,int ofs,unsigned int n);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;extern struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);extern struct Cyc_Position_Segment*Cyc_Position_segment_join(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);extern struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);static const int Cyc_Position_Lex=0;static const int Cyc_Position_Parse=
1;static const int Cyc_Position_Elab=2;struct Cyc_Position_Error{struct _tagged_arr
source;struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
struct Cyc_Position_Error*Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*,
struct _tagged_arr);extern unsigned char Cyc_Position_Nocontext[14];extern void Cyc_Position_post_error(
struct Cyc_Position_Error*);static const int Cyc_Absyn_Loc_n=0;static const int Cyc_Absyn_Rel_n=
0;struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_Abs_n=
1;struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple1{
void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;static const int Cyc_Absyn_Static=
0;static const int Cyc_Absyn_Abstract=1;static const int Cyc_Absyn_Public=2;static
const int Cyc_Absyn_Extern=3;static const int Cyc_Absyn_ExternC=4;struct Cyc_Absyn_Tqual{
int q_const: 1;int q_volatile: 1;int q_restrict: 1;};static const int Cyc_Absyn_B1=0;
static const int Cyc_Absyn_B2=1;static const int Cyc_Absyn_B4=2;static const int Cyc_Absyn_B8=
3;static const int Cyc_Absyn_AnyKind=0;static const int Cyc_Absyn_MemKind=1;static
const int Cyc_Absyn_BoxKind=2;static const int Cyc_Absyn_RgnKind=3;static const int Cyc_Absyn_EffKind=
4;static const int Cyc_Absyn_Signed=0;static const int Cyc_Absyn_Unsigned=1;struct Cyc_Absyn_Conref{
void*v;};static const int Cyc_Absyn_Eq_constr=0;struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};static const int Cyc_Absyn_Forward_constr=1;struct Cyc_Absyn_Forward_constr_struct{
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
void*Cyc_Absyn_rel_ns_null;extern int Cyc_Absyn_is_qvar_qualified(struct _tuple1*);
extern struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct
Cyc_Absyn_Tqual y);extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();extern void*
Cyc_Absyn_compress_kb(void*);extern void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
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
12];extern unsigned char Cyc_Dict_Absent[11];static const int Cyc_Tcenv_VarRes=0;
struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};static const int Cyc_Tcenv_StructRes=
1;struct Cyc_Tcenv_StructRes_struct{int tag;struct Cyc_Absyn_Structdecl*f1;};static
const int Cyc_Tcenv_TunionRes=2;struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*
f1;struct Cyc_Absyn_Tunionfield*f2;};static const int Cyc_Tcenv_EnumRes=3;struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};static const int
Cyc_Tcenv_AnonEnumRes=4;struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*f1;struct
Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*namespaces;
struct Cyc_Dict_Dict*structdecls;struct Cyc_Dict_Dict*uniondecls;struct Cyc_Dict_Dict*
tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;static const int
Cyc_Tcenv_NotLoop_j=0;static const int Cyc_Tcenv_CaseEnd_j=1;static const int Cyc_Tcenv_FnEnd_j=
2;static const int Cyc_Tcenv_Stmt_j=0;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};static const int Cyc_Tcenv_Outermost=0;struct Cyc_Tcenv_Outermost_struct{
int tag;void*f1;};static const int Cyc_Tcenv_Frame=1;struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};static const int Cyc_Tcenv_Hidden=2;struct Cyc_Tcenv_Hidden_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*
ae;struct Cyc_Core_Opt*le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void
Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct
_tagged_arr ap);extern void*Cyc_Tcutil_copy_type(void*t);extern void*Cyc_Tcutil_compress(
void*t);extern void Cyc_Lex_register_typedef(struct _tuple1*s);extern void Cyc_Lex_enter_namespace(
struct _tagged_arr*);extern void Cyc_Lex_leave_namespace();extern void Cyc_Lex_enter_using(
struct _tuple1*);extern void Cyc_Lex_leave_using();enum Cyc_Parse_Struct_or_union{
Cyc_Parse_Struct_su  = 0,Cyc_Parse_Union_su  = 1};static const int Cyc_Parse_Signed_spec=
0;struct Cyc_Parse_Signed_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
static const int Cyc_Parse_Unsigned_spec=1;struct Cyc_Parse_Unsigned_spec_struct{int
tag;struct Cyc_Position_Segment*f1;};static const int Cyc_Parse_Short_spec=2;struct
Cyc_Parse_Short_spec_struct{int tag;struct Cyc_Position_Segment*f1;};static const
int Cyc_Parse_Long_spec=3;struct Cyc_Parse_Long_spec_struct{int tag;struct Cyc_Position_Segment*
f1;};static const int Cyc_Parse_Type_spec=4;struct Cyc_Parse_Type_spec_struct{int tag;
void*f1;struct Cyc_Position_Segment*f2;};static const int Cyc_Parse_Decl_spec=5;
struct Cyc_Parse_Decl_spec_struct{int tag;struct Cyc_Absyn_Decl*f1;};static const int
Cyc_Parse_Typedef_sc=0;static const int Cyc_Parse_Extern_sc=1;static const int Cyc_Parse_ExternC_sc=
2;static const int Cyc_Parse_Static_sc=3;static const int Cyc_Parse_Auto_sc=4;static
const int Cyc_Parse_Register_sc=5;static const int Cyc_Parse_Abstract_sc=6;struct Cyc_Parse_Declaration_spec{
struct Cyc_Core_Opt*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int
is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Declarator{struct
_tuple1*id;struct Cyc_List_List*tms;};struct Cyc_Parse_Abstractdeclarator{struct
Cyc_List_List*tms;};struct _tuple4{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};static struct _tuple4*Cyc_Parse_fnargspec_to_arg(struct Cyc_Position_Segment*
loc,struct _tuple2*t);struct _tuple5{void*f1;struct Cyc_Core_Opt*f2;};static struct
_tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc);struct _tuple6{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct
Cyc_List_List*f4;};static struct _tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,
void*,struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple7{struct _tuple1*f1;
struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple7*t);struct Cyc_Core_Opt*Cyc_Parse_lbuf=0;static struct Cyc_List_List*
Cyc_Parse_parse_result=0;struct Cyc_Absyn_Aligned_att_struct Cyc_Parse_att_aligned={
1,- 1};struct _tuple8{struct _tagged_arr f1;void*f2;};static unsigned char _tmp1[8]="stdcall";
static unsigned char _tmp2[6]="cdecl";static unsigned char _tmp3[9]="fastcall";
static unsigned char _tmp4[9]="noreturn";static unsigned char _tmp5[6]="const";
static unsigned char _tmp6[8]="aligned";static unsigned char _tmp7[7]="packed";
static unsigned char _tmp8[7]="shared";static unsigned char _tmp9[7]="unused";static
unsigned char _tmpA[5]="weak";static unsigned char _tmpB[10]="dllimport";static
unsigned char _tmpC[10]="dllexport";static unsigned char _tmpD[23]="no_instrument_function";
static unsigned char _tmpE[12]="constructor";static unsigned char _tmpF[11]="destructor";
static unsigned char _tmp10[22]="no_check_memory_usage";struct _tuple8 Cyc_Parse_att_map[
16]={{{_tmp1,_tmp1,_tmp1 + 8},(void*)0},{{_tmp2,_tmp2,_tmp2 + 6},(void*)1},{{_tmp3,
_tmp3,_tmp3 + 9},(void*)2},{{_tmp4,_tmp4,_tmp4 + 9},(void*)3},{{_tmp5,_tmp5,_tmp5 + 
6},(void*)4},{{_tmp6,_tmp6,_tmp6 + 8},(void*)& Cyc_Parse_att_aligned},{{_tmp7,
_tmp7,_tmp7 + 7},(void*)5},{{_tmp8,_tmp8,_tmp8 + 7},(void*)7},{{_tmp9,_tmp9,_tmp9 + 
7},(void*)8},{{_tmpA,_tmpA,_tmpA + 5},(void*)9},{{_tmpB,_tmpB,_tmpB + 10},(void*)
10},{{_tmpC,_tmpC,_tmpC + 10},(void*)11},{{_tmpD,_tmpD,_tmpD + 23},(void*)12},{{
_tmpE,_tmpE,_tmpE + 12},(void*)13},{{_tmpF,_tmpF,_tmpF + 11},(void*)14},{{_tmp10,
_tmp10,_tmp10 + 22},(void*)15}};static void Cyc_Parse_err(struct _tagged_arr msg,
struct Cyc_Position_Segment*sg){Cyc_Position_post_error(Cyc_Position_mk_err_parse(
sg,msg));}static void*Cyc_Parse_abort(struct _tagged_arr msg,struct Cyc_Position_Segment*
sg){Cyc_Parse_err(msg,sg);(int)_throw((void*)Cyc_Position_Exit);}static void*Cyc_Parse_unimp(
struct _tagged_arr msg,struct Cyc_Position_Segment*sg){return Cyc_Parse_abort((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp12;_tmp12.tag=Cyc_Std_String_pa;
_tmp12.f1=(struct _tagged_arr)msg;{void*_tmp11[1]={& _tmp12};Cyc_Std_aprintf(
_tag_arr("%s unimplemented",sizeof(unsigned char),17),_tag_arr(_tmp11,sizeof(
void*),1));}}),sg);}static void Cyc_Parse_unimp2(struct _tagged_arr msg,struct Cyc_Position_Segment*
sg){({struct Cyc_Std_String_pa_struct _tmp15;_tmp15.tag=Cyc_Std_String_pa;_tmp15.f1=(
struct _tagged_arr)msg;{struct Cyc_Std_String_pa_struct _tmp14;_tmp14.tag=Cyc_Std_String_pa;
_tmp14.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(sg);{void*_tmp13[2]={&
_tmp14,& _tmp15};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: Warning: Cyclone does not yet support %s\n",
sizeof(unsigned char),46),_tag_arr(_tmp13,sizeof(void*),2));}}});return;}static
int Cyc_Parse_enum_counter=0;struct _tuple1*Cyc_Parse_gensym_enum(){return({struct
_tuple1*_tmp16=_cycalloc(sizeof(struct _tuple1));_tmp16->f1=(void*)({struct Cyc_Absyn_Rel_n_struct*
_tmp1A=_cycalloc(sizeof(struct Cyc_Absyn_Rel_n_struct));_tmp1A[0]=({struct Cyc_Absyn_Rel_n_struct
_tmp1B;_tmp1B.tag=Cyc_Absyn_Rel_n;_tmp1B.f1=0;_tmp1B;});_tmp1A;});_tmp16->f2=({
struct _tagged_arr*_tmp17=_cycalloc(sizeof(struct _tagged_arr));_tmp17[0]=(struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp19;_tmp19.tag=Cyc_Std_Int_pa;_tmp19.f1=(
int)((unsigned int)Cyc_Parse_enum_counter ++);{void*_tmp18[1]={& _tmp19};Cyc_Std_aprintf(
_tag_arr("__anonymous_enum_%d__",sizeof(unsigned char),22),_tag_arr(_tmp18,
sizeof(void*),1));}});_tmp17;});_tmp16;});}struct _tuple9{struct _tuple7*f1;struct
Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Structfield*Cyc_Parse_make_struct_field(
struct Cyc_Position_Segment*loc,struct _tuple9*field_info){struct _tuple9 _tmp1E;
struct Cyc_Absyn_Exp*_tmp1F;struct _tuple7*_tmp21;struct _tuple7 _tmp23;struct Cyc_List_List*
_tmp24;struct Cyc_List_List*_tmp26;void*_tmp28;struct Cyc_Absyn_Tqual _tmp2A;struct
_tuple1*_tmp2C;struct _tuple9*_tmp1C=field_info;_tmp1E=*_tmp1C;_LL34: _tmp21=
_tmp1E.f1;_tmp23=*_tmp21;_LL45: _tmp2C=_tmp23.f1;goto _LL43;_LL43: _tmp2A=_tmp23.f2;
goto _LL41;_LL41: _tmp28=_tmp23.f3;goto _LL39;_LL39: _tmp26=_tmp23.f4;goto _LL37;
_LL37: _tmp24=_tmp23.f5;goto _LL32;_LL32: _tmp1F=_tmp1E.f2;goto _LL29;_LL29: if(
_tmp26 != 0){Cyc_Parse_err(_tag_arr("bad type params in struct field",sizeof(
unsigned char),32),loc);}if(Cyc_Absyn_is_qvar_qualified(_tmp2C)){Cyc_Parse_err(
_tag_arr("struct field cannot be qualified with a namespace",sizeof(
unsigned char),50),loc);}return({struct Cyc_Absyn_Structfield*_tmp2E=_cycalloc(
sizeof(struct Cyc_Absyn_Structfield));_tmp2E->name=(*_tmp2C).f2;_tmp2E->tq=_tmp2A;
_tmp2E->type=(void*)_tmp28;_tmp2E->width=_tmp1F;_tmp2E->attributes=_tmp24;_tmp2E;});}
struct _tuple10{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*
f4;};static struct _tuple2*Cyc_Parse_make_param(struct Cyc_Position_Segment*loc,
struct _tuple10*field){struct _tuple10 _tmp31;struct Cyc_List_List*_tmp32;void*
_tmp34;struct Cyc_Absyn_Tqual _tmp36;struct Cyc_Core_Opt*_tmp38;struct _tuple10*
_tmp2F=field;_tmp31=*_tmp2F;_LL57: _tmp38=_tmp31.f1;goto _LL55;_LL55: _tmp36=_tmp31.f2;
goto _LL53;_LL53: _tmp34=_tmp31.f3;goto _LL51;_LL51: _tmp32=_tmp31.f4;goto _LL48;
_LL48: {struct Cyc_Core_Opt*_tmp3A=0;if(_tmp38 != 0){if(Cyc_Absyn_is_qvar_qualified((
struct _tuple1*)_tmp38->v)){Cyc_Parse_err(_tag_arr("parameter cannot be qualified with a namespace",
sizeof(unsigned char),47),loc);}_tmp3A=({struct Cyc_Core_Opt*_tmp3B=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp3B->v=(*((struct _tuple1*)_tmp38->v)).f2;_tmp3B;});}
if(_tmp32 != 0){((int(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*sg))Cyc_Parse_abort)(
_tag_arr("parameter should have no type parameters",sizeof(unsigned char),41),
loc);}return({struct _tuple2*_tmp3C=_cycalloc(sizeof(struct _tuple2));_tmp3C->f1=
_tmp3A;_tmp3C->f2=_tmp36;_tmp3C->f3=_tmp34;_tmp3C;});}}static void*Cyc_Parse_type_spec(
void*t,struct Cyc_Position_Segment*loc){return(void*)({struct Cyc_Parse_Type_spec_struct*
_tmp3D=_cycalloc(sizeof(struct Cyc_Parse_Type_spec_struct));_tmp3D[0]=({struct Cyc_Parse_Type_spec_struct
_tmp3E;_tmp3E.tag=Cyc_Parse_Type_spec;_tmp3E.f1=(void*)t;_tmp3E.f2=loc;_tmp3E;});
_tmp3D;});}static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp3F=t;
struct Cyc_Absyn_Exp*_tmp45;struct Cyc_Absyn_Tqual _tmp47;void*_tmp49;_LL65: if((
unsigned int)_tmp3F > 3?*((int*)_tmp3F)== Cyc_Absyn_ArrayType: 0){_LL74: _tmp49=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3F)->f1;goto _LL72;_LL72: _tmp47=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F)->f2;goto _LL70;_LL70: _tmp45=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3F)->f3;goto _LL66;}else{goto _LL67;}_LL67: goto _LL68;_LL66: return Cyc_Absyn_starb_typ(
_tmp49,argposn? Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp4B=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmp4B->v=(void*)((void*)Cyc_Absyn_RgnKind);_tmp4B;}),0):(
void*)Cyc_Absyn_HeapRgn,_tmp47,_tmp45 == 0?(void*)Cyc_Absyn_Unknown_b:(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp4C=_cycalloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
_tmp4C[0]=({struct Cyc_Absyn_Upper_b_struct _tmp4D;_tmp4D.tag=Cyc_Absyn_Upper_b;
_tmp4D.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp45);_tmp4D;});_tmp4C;}));_LL68:
return t;_LL64:;}static void Cyc_Parse_arg_array2ptr(struct _tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*
x).f3,1);}struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple11*
Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple2*t){return({
struct _tuple11*_tmp4E=_cycalloc(sizeof(struct _tuple11));_tmp4E->f1=(*t).f2;
_tmp4E->f2=(*t).f3;_tmp4E;});}static void Cyc_Parse_only_vardecl(struct Cyc_List_List*
params,struct Cyc_Absyn_Decl*x){struct _tagged_arr decl_kind;{void*_tmp4F=(void*)x->r;
struct Cyc_Absyn_Vardecl*_tmp69;_LL81: if(*((int*)_tmp4F)== Cyc_Absyn_Var_d){
_LL106: _tmp69=((struct Cyc_Absyn_Var_d_struct*)_tmp4F)->f1;goto _LL82;}else{goto
_LL83;}_LL83: if(*((int*)_tmp4F)== Cyc_Absyn_Let_d){goto _LL84;}else{goto _LL85;}
_LL85: if(*((int*)_tmp4F)== Cyc_Absyn_Letv_d){goto _LL86;}else{goto _LL87;}_LL87:
if(*((int*)_tmp4F)== Cyc_Absyn_Fn_d){goto _LL88;}else{goto _LL89;}_LL89: if(*((int*)
_tmp4F)== Cyc_Absyn_Struct_d){goto _LL90;}else{goto _LL91;}_LL91: if(*((int*)_tmp4F)
== Cyc_Absyn_Union_d){goto _LL92;}else{goto _LL93;}_LL93: if(*((int*)_tmp4F)== Cyc_Absyn_Tunion_d){
goto _LL94;}else{goto _LL95;}_LL95: if(*((int*)_tmp4F)== Cyc_Absyn_Typedef_d){goto
_LL96;}else{goto _LL97;}_LL97: if(*((int*)_tmp4F)== Cyc_Absyn_Enum_d){goto _LL98;}
else{goto _LL99;}_LL99: if(*((int*)_tmp4F)== Cyc_Absyn_Namespace_d){goto _LL100;}
else{goto _LL101;}_LL101: if(*((int*)_tmp4F)== Cyc_Absyn_Using_d){goto _LL102;}
else{goto _LL103;}_LL103: if(*((int*)_tmp4F)== Cyc_Absyn_ExternC_d){goto _LL104;}
else{goto _LL80;}_LL82: if(_tmp69->initializer != 0){((int(*)(struct _tagged_arr msg,
struct Cyc_Position_Segment*sg))Cyc_Parse_abort)(_tag_arr("initializers are not allowed in parameter declarations",
sizeof(unsigned char),55),x->loc);}if(Cyc_Absyn_is_qvar_qualified(_tmp69->name)){
Cyc_Parse_err(_tag_arr("namespaces not allowed on parameter declarations",
sizeof(unsigned char),49),x->loc);}{int found=0;for(0;params != 0;params=params->tl){
if(Cyc_Std_zstrptrcmp((*_tmp69->name).f2,(struct _tagged_arr*)params->hd)== 0){
found=1;break;}}if(! found){((int(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*
sg))Cyc_Parse_abort)((struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp6C;
_tmp6C.tag=Cyc_Std_String_pa;_tmp6C.f1=(struct _tagged_arr)*(*_tmp69->name).f2;{
void*_tmp6B[1]={& _tmp6C};Cyc_Std_aprintf(_tag_arr("%s is not listed as a parameter",
sizeof(unsigned char),32),_tag_arr(_tmp6B,sizeof(void*),1));}}),x->loc);}return;}
_LL84: decl_kind=_tag_arr("let declaration",sizeof(unsigned char),16);goto _LL80;
_LL86: decl_kind=_tag_arr("let declaration",sizeof(unsigned char),16);goto _LL80;
_LL88: decl_kind=_tag_arr("function declaration",sizeof(unsigned char),21);goto
_LL80;_LL90: decl_kind=_tag_arr("struct declaration",sizeof(unsigned char),19);
goto _LL80;_LL92: decl_kind=_tag_arr("union declaration",sizeof(unsigned char),18);
goto _LL80;_LL94: decl_kind=_tag_arr("tunion declaration",sizeof(unsigned char),19);
goto _LL80;_LL96: decl_kind=_tag_arr("typedef",sizeof(unsigned char),8);goto _LL80;
_LL98: decl_kind=_tag_arr("enum declaration",sizeof(unsigned char),17);goto _LL80;
_LL100: decl_kind=_tag_arr("namespace declaration",sizeof(unsigned char),22);goto
_LL80;_LL102: decl_kind=_tag_arr("using declaration",sizeof(unsigned char),18);
goto _LL80;_LL104: decl_kind=_tag_arr("extern C declaration",sizeof(unsigned char),
21);goto _LL80;_LL80:;}((int(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*
sg))Cyc_Parse_abort)((struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp6E;
_tmp6E.tag=Cyc_Std_String_pa;_tmp6E.f1=(struct _tagged_arr)decl_kind;{void*_tmp6D[
1]={& _tmp6E};Cyc_Std_aprintf(_tag_arr("%s appears in parameter type",sizeof(
unsigned char),29),_tag_arr(_tmp6D,sizeof(void*),1));}}),x->loc);return;}static
struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){void*
_tmp6F=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp75;_LL113: if(*((int*)_tmp6F)== 
Cyc_Absyn_Var_d){_LL118: _tmp75=((struct Cyc_Absyn_Var_d_struct*)_tmp6F)->f1;goto
_LL114;}else{goto _LL115;}_LL115: goto _LL116;_LL114: return _tmp75;_LL116: return((
struct Cyc_Absyn_Vardecl*(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*sg))
Cyc_Parse_abort)(_tag_arr("bad declaration in `forarray' statement",sizeof(
unsigned char),40),d->loc);_LL112:;}struct _tuple12{struct Cyc_List_List*f1;struct
Cyc_Position_Segment*f2;};static struct _tuple2*Cyc_Parse_get_param_type(struct
_tuple12*env,struct _tagged_arr*x){struct _tuple12 _tmp79;struct Cyc_Position_Segment*
_tmp7A;struct Cyc_List_List*_tmp7C;struct _tuple12*_tmp77=env;_tmp79=*_tmp77;
_LL125: _tmp7C=_tmp79.f1;goto _LL123;_LL123: _tmp7A=_tmp79.f2;goto _LL120;_LL120: if(
_tmp7C == 0){return((struct _tuple2*(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*
sg))Cyc_Parse_abort)((struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp7F;
_tmp7F.tag=Cyc_Std_String_pa;_tmp7F.f1=(struct _tagged_arr)*x;{void*_tmp7E[1]={&
_tmp7F};Cyc_Std_aprintf(_tag_arr("missing type for parameter %s",sizeof(
unsigned char),30),_tag_arr(_tmp7E,sizeof(void*),1));}}),_tmp7A);}{void*_tmp80=(
void*)((struct Cyc_Absyn_Decl*)_tmp7C->hd)->r;struct Cyc_Absyn_Vardecl*_tmp86;
_LL130: if(*((int*)_tmp80)== Cyc_Absyn_Var_d){_LL135: _tmp86=((struct Cyc_Absyn_Var_d_struct*)
_tmp80)->f1;goto _LL131;}else{goto _LL132;}_LL132: goto _LL133;_LL131: if(Cyc_Absyn_is_qvar_qualified(
_tmp86->name)){Cyc_Parse_err(_tag_arr("namespace not allowed on parameter",
sizeof(unsigned char),35),_tmp7A);}if(Cyc_Std_zstrptrcmp((*_tmp86->name).f2,x)== 
0){return({struct _tuple2*_tmp88=_cycalloc(sizeof(struct _tuple2));_tmp88->f1=({
struct Cyc_Core_Opt*_tmp89=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp89->v=(*
_tmp86->name).f2;_tmp89;});_tmp88->f2=_tmp86->tq;_tmp88->f3=(void*)_tmp86->type;
_tmp88;});}else{return Cyc_Parse_get_param_type(({struct _tuple12*_tmp8A=_cycalloc(
sizeof(struct _tuple12));_tmp8A->f1=_tmp7C->tl;_tmp8A->f2=_tmp7A;_tmp8A;}),x);}
_LL133: return((struct _tuple2*(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*
sg))Cyc_Parse_abort)(_tag_arr("non-variable declaration",sizeof(unsigned char),
25),((struct Cyc_Absyn_Decl*)_tmp7C->hd)->loc);_LL129:;}}static int Cyc_Parse_is_typeparam(
void*tm){void*_tmp8B=tm;_LL141: if((unsigned int)_tmp8B > 1?*((int*)_tmp8B)== Cyc_Absyn_TypeParams_mod:
0){goto _LL142;}else{goto _LL143;}_LL143: goto _LL144;_LL142: return 1;_LL144: return 0;
_LL140:;}static void*Cyc_Parse_id2type(struct _tagged_arr s,void*k){if(Cyc_Std_zstrcmp(
s,_tag_arr("`H",sizeof(unsigned char),3))== 0){return(void*)Cyc_Absyn_HeapRgn;}
else{return(void*)({struct Cyc_Absyn_VarType_struct*_tmp91=_cycalloc(sizeof(
struct Cyc_Absyn_VarType_struct));_tmp91[0]=({struct Cyc_Absyn_VarType_struct
_tmp92;_tmp92.tag=Cyc_Absyn_VarType;_tmp92.f1=({struct Cyc_Absyn_Tvar*_tmp93=
_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_tmp93->name=({struct _tagged_arr*_tmp94=
_cycalloc(sizeof(struct _tagged_arr));_tmp94[0]=s;_tmp94;});_tmp93->identity=0;
_tmp93->kind=(void*)k;_tmp93;});_tmp92;});_tmp91;});}}static struct Cyc_Absyn_Tvar*
Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){void*k;{void*_tmp95=Cyc_Absyn_compress_kb((
void*)t->kind);void*_tmp9D;void*_tmp9F;_LL151: if(*((int*)_tmp95)== Cyc_Absyn_Eq_kb){
_LL158: _tmp9D=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp95)->f1;goto _LL152;}
else{goto _LL153;}_LL153: if(*((int*)_tmp95)== Cyc_Absyn_Unknown_kb){goto _LL154;}
else{goto _LL155;}_LL155: if(*((int*)_tmp95)== Cyc_Absyn_Less_kb){_LL160: _tmp9F=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp95)->f2;goto _LL156;}else{goto _LL150;}
_LL152: k=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmpA1=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));
_tmpA1[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpA2;_tmpA2.tag=Cyc_Absyn_Eq_kb;_tmpA2.f1=(
void*)_tmp9D;_tmpA2;});_tmpA1;});goto _LL150;_LL154: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmpA3=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_struct));_tmpA3[0]=({struct
Cyc_Absyn_Unknown_kb_struct _tmpA4;_tmpA4.tag=Cyc_Absyn_Unknown_kb;_tmpA4.f1=0;
_tmpA4;});_tmpA3;});goto _LL150;_LL156: k=(void*)({struct Cyc_Absyn_Less_kb_struct*
_tmpA5=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_struct));_tmpA5[0]=({struct Cyc_Absyn_Less_kb_struct
_tmpA6;_tmpA6.tag=Cyc_Absyn_Less_kb;_tmpA6.f1=0;_tmpA6.f2=(void*)_tmp9F;_tmpA6;});
_tmpA5;});goto _LL150;_LL150:;}return({struct Cyc_Absyn_Tvar*_tmpA7=_cycalloc(
sizeof(struct Cyc_Absyn_Tvar));_tmpA7->name=t->name;_tmpA7->identity=0;_tmpA7->kind=(
void*)k;_tmpA7;});}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmpA8=t;struct Cyc_Absyn_Tvar*_tmpAE;_LL170: if((unsigned int)
_tmpA8 > 3?*((int*)_tmpA8)== Cyc_Absyn_VarType: 0){_LL175: _tmpAE=((struct Cyc_Absyn_VarType_struct*)
_tmpA8)->f1;goto _LL171;}else{goto _LL172;}_LL172: goto _LL173;_LL171: return _tmpAE;
_LL173: return((struct Cyc_Absyn_Tvar*(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*
sg))Cyc_Parse_abort)(_tag_arr("expecting a list of type variables, not types",
sizeof(unsigned char),46),loc);_LL169:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*
pr){return(void*)({struct Cyc_Absyn_VarType_struct*_tmpB0=_cycalloc(sizeof(struct
Cyc_Absyn_VarType_struct));_tmpB0[0]=({struct Cyc_Absyn_VarType_struct _tmpB1;
_tmpB1.tag=Cyc_Absyn_VarType;_tmpB1.f1=pr;_tmpB1;});_tmpB0;});}static void Cyc_Parse_set_vartyp_kind(
void*t,void*k){void*_tmpB2=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpB8;
struct Cyc_Absyn_Tvar _tmpBA;void*_tmpBB;void**_tmpBD;_LL180: if((unsigned int)
_tmpB2 > 3?*((int*)_tmpB2)== Cyc_Absyn_VarType: 0){_LL185: _tmpB8=((struct Cyc_Absyn_VarType_struct*)
_tmpB2)->f1;_tmpBA=*_tmpB8;_LL188: _tmpBB=(void*)_tmpBA.kind;_tmpBD=(void**)&(*((
struct Cyc_Absyn_VarType_struct*)_tmpB2)->f1).kind;goto _LL181;}else{goto _LL182;}
_LL182: goto _LL183;_LL181: {void*_tmpBE=Cyc_Absyn_compress_kb(*_tmpBD);void*
_tmpBF=_tmpBE;_LL193: if(*((int*)_tmpBF)== Cyc_Absyn_Eq_kb){goto _LL194;}else{goto
_LL195;}_LL195: if(*((int*)_tmpBF)== Cyc_Absyn_Less_kb){goto _LL196;}else{goto
_LL197;}_LL197: if(*((int*)_tmpBF)== Cyc_Absyn_Unknown_kb){goto _LL198;}else{goto
_LL192;}_LL194: return;_LL196: return;_LL198:*_tmpBD=(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmpC7=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmpC7[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmpC8;_tmpC8.tag=Cyc_Absyn_Eq_kb;_tmpC8.f1=(void*)k;_tmpC8;});_tmpC7;});return;
_LL192:;}_LL183: return;_LL179:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0){return tms;}if(tms == 0){return 0;}{void*_tmpC9=(void*)tms->hd;void*
_tmpCF;_LL203: if((unsigned int)_tmpC9 > 1?*((int*)_tmpC9)== Cyc_Absyn_Function_mod:
0){_LL208: _tmpCF=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmpC9)->f1;goto
_LL204;}else{goto _LL205;}_LL205: goto _LL206;_LL204: if(tms->tl == 0? 1:(Cyc_Parse_is_typeparam((
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)?((struct Cyc_List_List*)
_check_null(tms->tl))->tl == 0: 0)){void*_tmpD1=_tmpCF;struct Cyc_List_List*_tmpD7;
_LL211: if(*((int*)_tmpD1)== Cyc_Absyn_WithTypes){goto _LL212;}else{goto _LL213;}
_LL213: if(*((int*)_tmpD1)== Cyc_Absyn_NoTypes){_LL216: _tmpD7=((struct Cyc_Absyn_NoTypes_struct*)
_tmpD1)->f1;goto _LL214;}else{goto _LL210;}_LL212:({void*_tmpD9[0]={};Cyc_Tcutil_warn(
loc,_tag_arr("function declaration with both new- and old-style parameter declarations; ignoring old-style",
sizeof(unsigned char),93),_tag_arr(_tmpD9,sizeof(void*),0));});return tms;_LL214:((
void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_only_vardecl,_tmpD7,tds);{
struct _tuple12*_tmpDA=({struct _tuple12*_tmpE0=_cycalloc(sizeof(struct _tuple12));
_tmpE0->f1=tds;_tmpE0->f2=loc;_tmpE0;});return({struct Cyc_List_List*_tmpDB=
_cycalloc(sizeof(struct Cyc_List_List));_tmpDB->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmpDC=_cycalloc(sizeof(struct Cyc_Absyn_Function_mod_struct));_tmpDC[0]=({struct
Cyc_Absyn_Function_mod_struct _tmpDD;_tmpDD.tag=Cyc_Absyn_Function_mod;_tmpDD.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmpDE=_cycalloc(sizeof(struct
Cyc_Absyn_WithTypes_struct));_tmpDE[0]=({struct Cyc_Absyn_WithTypes_struct _tmpDF;
_tmpDF.tag=Cyc_Absyn_WithTypes;_tmpDF.f1=((struct Cyc_List_List*(*)(struct _tuple2*(*
f)(struct _tuple12*,struct _tagged_arr*),struct _tuple12*env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Parse_get_param_type,_tmpDA,_tmpD7);_tmpDF.f2=0;_tmpDF.f3=0;
_tmpDF.f4=0;_tmpDF.f5=0;_tmpDF;});_tmpDE;}));_tmpDD;});_tmpDC;}));_tmpDB->tl=0;
_tmpDB;});}_LL210:;}else{return({struct Cyc_List_List*_tmpE1=_cycalloc(sizeof(
struct Cyc_List_List));_tmpE1->hd=(void*)((void*)tms->hd);_tmpE1->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc);_tmpE1;});}_LL206: return({struct Cyc_List_List*_tmpE2=_cycalloc(
sizeof(struct Cyc_List_List));_tmpE2->hd=(void*)((void*)tms->hd);_tmpE2->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc);_tmpE2;});_LL202:;}}static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(
struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,
struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){if(tds != 0){d=({struct
Cyc_Parse_Declarator*_tmpE3=_cycalloc(sizeof(struct Cyc_Parse_Declarator));_tmpE3->id=
d->id;_tmpE3->tms=Cyc_Parse_oldstyle2newstyle(d->tms,tds,loc);_tmpE3;});}{void*
sc=(void*)Cyc_Absyn_Public;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual();
int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((
struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)
dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0){void*
_tmpE4=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc))->v;_LL230: if(_tmpE4 == (void*)Cyc_Parse_Extern_sc){goto _LL231;}
else{goto _LL232;}_LL232: if(_tmpE4 == (void*)Cyc_Parse_ExternC_sc){goto _LL233;}
else{goto _LL234;}_LL234: if(_tmpE4 == (void*)Cyc_Parse_Static_sc){goto _LL235;}
else{goto _LL236;}_LL236: goto _LL237;_LL231: sc=(void*)Cyc_Absyn_Extern;goto _LL229;
_LL233: sc=(void*)Cyc_Absyn_ExternC;goto _LL229;_LL235: sc=(void*)Cyc_Absyn_Static;
goto _LL229;_LL237: Cyc_Parse_err(_tag_arr("bad storage class on function",sizeof(
unsigned char),30),loc);goto _LL229;_LL229:;}}{struct Cyc_Core_Opt*_tmpF0;void*
_tmpF2;struct _tuple5 _tmpEE=Cyc_Parse_collapse_type_specifiers(tss,loc);_LL243:
_tmpF2=_tmpEE.f1;goto _LL241;_LL241: _tmpF0=_tmpEE.f2;goto _LL239;_LL239: {struct
Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF8;void*_tmpFA;struct Cyc_Absyn_Tqual
_tmpFC;struct _tuple6 _tmpF4=Cyc_Parse_apply_tms(tq,_tmpF2,atts,d->tms);_LL253:
_tmpFC=_tmpF4.f1;goto _LL251;_LL251: _tmpFA=_tmpF4.f2;goto _LL249;_LL249: _tmpF8=
_tmpF4.f3;goto _LL247;_LL247: _tmpF6=_tmpF4.f4;goto _LL245;_LL245: if(_tmpF0 != 0){({
void*_tmpFE[0]={};Cyc_Tcutil_warn(loc,_tag_arr("nested type declaration within function prototype",
sizeof(unsigned char),50),_tag_arr(_tmpFE,sizeof(void*),0));});}if(_tmpF8 != 0){({
void*_tmpFF[0]={};Cyc_Tcutil_warn(loc,_tag_arr("bad type params, ignoring",
sizeof(unsigned char),26),_tag_arr(_tmpFF,sizeof(void*),0));});}{void*_tmp100=
_tmpFA;struct Cyc_Absyn_FnInfo _tmp106;struct Cyc_List_List*_tmp108;struct Cyc_List_List*
_tmp10A;struct Cyc_Absyn_VarargInfo*_tmp10C;int _tmp10E;struct Cyc_List_List*
_tmp110;void*_tmp112;struct Cyc_Core_Opt*_tmp114;struct Cyc_List_List*_tmp116;
_LL258: if((unsigned int)_tmp100 > 3?*((int*)_tmp100)== Cyc_Absyn_FnType: 0){_LL263:
_tmp106=((struct Cyc_Absyn_FnType_struct*)_tmp100)->f1;_LL279: _tmp116=_tmp106.tvars;
goto _LL277;_LL277: _tmp114=_tmp106.effect;goto _LL275;_LL275: _tmp112=(void*)
_tmp106.ret_typ;goto _LL273;_LL273: _tmp110=_tmp106.args;goto _LL271;_LL271: _tmp10E=
_tmp106.c_varargs;goto _LL269;_LL269: _tmp10C=_tmp106.cyc_varargs;goto _LL267;
_LL267: _tmp10A=_tmp106.rgn_po;goto _LL265;_LL265: _tmp108=_tmp106.attributes;goto
_LL259;}else{goto _LL260;}_LL260: goto _LL261;_LL259: {struct Cyc_List_List*_tmp118=((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_fnargspec_to_arg,loc,_tmp110);return({struct Cyc_Absyn_Fndecl*_tmp119=
_cycalloc(sizeof(struct Cyc_Absyn_Fndecl));_tmp119->sc=(void*)sc;_tmp119->name=d->id;
_tmp119->tvs=_tmp116;_tmp119->is_inline=is_inline;_tmp119->effect=_tmp114;
_tmp119->ret_type=(void*)_tmp112;_tmp119->args=_tmp118;_tmp119->c_varargs=
_tmp10E;_tmp119->cyc_varargs=_tmp10C;_tmp119->rgn_po=_tmp10A;_tmp119->body=body;
_tmp119->cached_typ=0;_tmp119->param_vardecls=0;_tmp119->attributes=Cyc_List_append(
_tmp108,_tmpF6);_tmp119;});}_LL261: return((struct Cyc_Absyn_Fndecl*(*)(struct
_tagged_arr msg,struct Cyc_Position_Segment*sg))Cyc_Parse_abort)(_tag_arr("declarator is not a function prototype",
sizeof(unsigned char),39),loc);_LL257:;}}}}}static struct _tuple4*Cyc_Parse_fnargspec_to_arg(
struct Cyc_Position_Segment*loc,struct _tuple2*t){if((*t).f1 == 0){Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype",sizeof(unsigned char),
48),loc);return({struct _tuple4*_tmp11A=_cycalloc(sizeof(struct _tuple4));_tmp11A->f1=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"?",sizeof(unsigned char),2);
_tmp11A->f2=(*t).f2;_tmp11A->f3=(*t).f3;_tmp11A;});}else{return({struct _tuple4*
_tmp11B=_cycalloc(sizeof(struct _tuple4));_tmp11B->f1=(struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null((*t).f1))->v;_tmp11B->f2=(*t).f2;_tmp11B->f3=(*t).f3;
_tmp11B;});}}static unsigned char _tmp11C[52]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={_tmp11C,_tmp11C,_tmp11C + 52};static
unsigned char _tmp11D[63]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={_tmp11D,_tmp11D,_tmp11D + 63};static
unsigned char _tmp11E[50]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={_tmp11E,_tmp11E,_tmp11E + 50};static
unsigned char _tmp11F[60]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={_tmp11F,_tmp11F,_tmp11F + 60};static struct
_tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;
void*t=(void*)Cyc_Absyn_VoidType;void*sz=(void*)Cyc_Absyn_B4;void*sgn=(void*)Cyc_Absyn_Signed;
struct Cyc_Position_Segment*last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp120=(
void*)ts->hd;struct Cyc_Position_Segment*_tmp12E;void*_tmp130;struct Cyc_Position_Segment*
_tmp132;struct Cyc_Position_Segment*_tmp134;struct Cyc_Position_Segment*_tmp136;
struct Cyc_Position_Segment*_tmp138;struct Cyc_Absyn_Decl*_tmp13A;_LL290: if(*((int*)
_tmp120)== Cyc_Parse_Type_spec){_LL305: _tmp130=(void*)((struct Cyc_Parse_Type_spec_struct*)
_tmp120)->f1;goto _LL303;_LL303: _tmp12E=((struct Cyc_Parse_Type_spec_struct*)
_tmp120)->f2;goto _LL291;}else{goto _LL292;}_LL292: if(*((int*)_tmp120)== Cyc_Parse_Signed_spec){
_LL307: _tmp132=((struct Cyc_Parse_Signed_spec_struct*)_tmp120)->f1;goto _LL293;}
else{goto _LL294;}_LL294: if(*((int*)_tmp120)== Cyc_Parse_Unsigned_spec){_LL309:
_tmp134=((struct Cyc_Parse_Unsigned_spec_struct*)_tmp120)->f1;goto _LL295;}else{
goto _LL296;}_LL296: if(*((int*)_tmp120)== Cyc_Parse_Short_spec){_LL311: _tmp136=((
struct Cyc_Parse_Short_spec_struct*)_tmp120)->f1;goto _LL297;}else{goto _LL298;}
_LL298: if(*((int*)_tmp120)== Cyc_Parse_Long_spec){_LL313: _tmp138=((struct Cyc_Parse_Long_spec_struct*)
_tmp120)->f1;goto _LL299;}else{goto _LL300;}_LL300: if(*((int*)_tmp120)== Cyc_Parse_Decl_spec){
_LL315: _tmp13A=((struct Cyc_Parse_Decl_spec_struct*)_tmp120)->f1;goto _LL301;}
else{goto _LL289;}_LL291: if(seen_type){Cyc_Parse_err(Cyc_Parse_msg1,_tmp12E);}
last_loc=_tmp12E;seen_type=1;t=_tmp130;goto _LL289;_LL293: if(seen_sign){Cyc_Parse_err(
Cyc_Parse_msg4,_tmp132);}if(seen_type){Cyc_Parse_err(_tag_arr("signed qualifier must come before type",
sizeof(unsigned char),39),_tmp132);}last_loc=_tmp132;seen_sign=1;sgn=(void*)Cyc_Absyn_Signed;
goto _LL289;_LL295: if(seen_sign){Cyc_Parse_err(Cyc_Parse_msg4,_tmp134);}if(
seen_type){Cyc_Parse_err(_tag_arr("signed qualifier must come before type",
sizeof(unsigned char),39),_tmp134);}last_loc=_tmp134;seen_sign=1;sgn=(void*)Cyc_Absyn_Unsigned;
goto _LL289;_LL297: if(seen_size){Cyc_Parse_err(_tag_arr("integral size may appear only once within a type specifier",
sizeof(unsigned char),59),_tmp136);}if(seen_type){Cyc_Parse_err(_tag_arr("short modifier must come before base type",
sizeof(unsigned char),42),_tmp136);}last_loc=_tmp136;seen_size=1;sz=(void*)Cyc_Absyn_B2;
goto _LL289;_LL299: if(seen_type){Cyc_Parse_err(_tag_arr("long modifier must come before base type",
sizeof(unsigned char),41),_tmp138);}if(seen_size){void*_tmp13C=sz;_LL318: if(
_tmp13C == (void*)Cyc_Absyn_B4){goto _LL319;}else{goto _LL320;}_LL320: goto _LL321;
_LL319: sz=(void*)Cyc_Absyn_B8;goto _LL317;_LL321: Cyc_Parse_err(_tag_arr("extra long in type specifier",
sizeof(unsigned char),29),_tmp138);goto _LL317;_LL317:;}last_loc=_tmp138;
seen_size=1;goto _LL289;_LL301: last_loc=_tmp13A->loc;if(declopt == 0? ! seen_type: 0){
seen_type=1;{void*_tmp142=(void*)_tmp13A->r;struct Cyc_Absyn_Structdecl*_tmp14E;
struct Cyc_Absyn_Tuniondecl*_tmp150;struct Cyc_Absyn_Uniondecl*_tmp152;struct Cyc_Absyn_Enumdecl*
_tmp154;_LL324: if(*((int*)_tmp142)== Cyc_Absyn_Struct_d){_LL335: _tmp14E=((struct
Cyc_Absyn_Struct_d_struct*)_tmp142)->f1;goto _LL325;}else{goto _LL326;}_LL326: if(*((
int*)_tmp142)== Cyc_Absyn_Tunion_d){_LL337: _tmp150=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp142)->f1;goto _LL327;}else{goto _LL328;}_LL328: if(*((int*)_tmp142)== Cyc_Absyn_Union_d){
_LL339: _tmp152=((struct Cyc_Absyn_Union_d_struct*)_tmp142)->f1;goto _LL329;}else{
goto _LL330;}_LL330: if(*((int*)_tmp142)== Cyc_Absyn_Enum_d){_LL341: _tmp154=((
struct Cyc_Absyn_Enum_d_struct*)_tmp142)->f1;goto _LL331;}else{goto _LL332;}_LL332:
goto _LL333;_LL325: {struct Cyc_List_List*_tmp156=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmp14E->tvs));t=(void*)({
struct Cyc_Absyn_StructType_struct*_tmp157=_cycalloc(sizeof(struct Cyc_Absyn_StructType_struct));
_tmp157[0]=({struct Cyc_Absyn_StructType_struct _tmp158;_tmp158.tag=Cyc_Absyn_StructType;
_tmp158.f1=(struct _tuple1*)((struct _tuple1*)((struct Cyc_Core_Opt*)_check_null(
_tmp14E->name))->v);_tmp158.f2=_tmp156;_tmp158.f3=0;_tmp158;});_tmp157;});if(
_tmp14E->fields != 0){declopt=({struct Cyc_Core_Opt*_tmp159=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmp159->v=_tmp13A;_tmp159;});}goto _LL323;}_LL327: {struct
Cyc_List_List*_tmp15A=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmp150->tvs));t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp15B=_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));_tmp15B[0]=({struct
Cyc_Absyn_TunionType_struct _tmp15C;_tmp15C.tag=Cyc_Absyn_TunionType;_tmp15C.f1=({
struct Cyc_Absyn_TunionInfo _tmp15D;_tmp15D.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp15E=_cycalloc(sizeof(struct Cyc_Absyn_KnownTunion_struct));_tmp15E[0]=({
struct Cyc_Absyn_KnownTunion_struct _tmp15F;_tmp15F.tag=Cyc_Absyn_KnownTunion;
_tmp15F.f1=({struct Cyc_Absyn_Tuniondecl**_tmp160=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl*));
_tmp160[0]=_tmp150;_tmp160;});_tmp15F;});_tmp15E;}));_tmp15D.targs=_tmp15A;
_tmp15D.rgn=(void*)((void*)Cyc_Absyn_HeapRgn);_tmp15D;});_tmp15C;});_tmp15B;});
if(_tmp150->fields != 0){declopt=({struct Cyc_Core_Opt*_tmp161=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmp161->v=_tmp13A;_tmp161;});}goto _LL323;}_LL329: {struct
Cyc_List_List*_tmp162=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmp152->tvs));t=(void*)({struct Cyc_Absyn_UnionType_struct*
_tmp163=_cycalloc(sizeof(struct Cyc_Absyn_UnionType_struct));_tmp163[0]=({struct
Cyc_Absyn_UnionType_struct _tmp164;_tmp164.tag=Cyc_Absyn_UnionType;_tmp164.f1=(
struct _tuple1*)((struct _tuple1*)((struct Cyc_Core_Opt*)_check_null(_tmp152->name))->v);
_tmp164.f2=_tmp162;_tmp164.f3=0;_tmp164;});_tmp163;});if(_tmp152->fields != 0){
declopt=({struct Cyc_Core_Opt*_tmp165=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp165->v=_tmp13A;_tmp165;});}goto _LL323;}_LL331: t=(void*)({struct Cyc_Absyn_EnumType_struct*
_tmp166=_cycalloc(sizeof(struct Cyc_Absyn_EnumType_struct));_tmp166[0]=({struct
Cyc_Absyn_EnumType_struct _tmp167;_tmp167.tag=Cyc_Absyn_EnumType;_tmp167.f1=
_tmp154->name;_tmp167.f2=0;_tmp167;});_tmp166;});declopt=({struct Cyc_Core_Opt*
_tmp168=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp168->v=_tmp13A;_tmp168;});goto
_LL323;_LL333:((int(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*sg))Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier",sizeof(unsigned char),38),
_tmp13A->loc);goto _LL323;_LL323:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmp13A->loc);}
goto _LL289;_LL289:;}if(! seen_type){if(! seen_sign? ! seen_size: 0){({void*_tmp169[0]={};
Cyc_Tcutil_warn(last_loc,_tag_arr("missing type within specifier",sizeof(
unsigned char),30),_tag_arr(_tmp169,sizeof(void*),0));});}t=(void*)({struct Cyc_Absyn_IntType_struct*
_tmp16A=_cycalloc(sizeof(struct Cyc_Absyn_IntType_struct));_tmp16A[0]=({struct Cyc_Absyn_IntType_struct
_tmp16B;_tmp16B.tag=Cyc_Absyn_IntType;_tmp16B.f1=(void*)sgn;_tmp16B.f2=(void*)sz;
_tmp16B;});_tmp16A;});}else{if(seen_sign){void*_tmp16C=t;void*_tmp172;_LL366: if((
unsigned int)_tmp16C > 3?*((int*)_tmp16C)== Cyc_Absyn_IntType: 0){_LL371: _tmp172=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp16C)->f2;goto _LL367;}else{goto _LL368;}
_LL368: goto _LL369;_LL367: t=(void*)({struct Cyc_Absyn_IntType_struct*_tmp174=
_cycalloc(sizeof(struct Cyc_Absyn_IntType_struct));_tmp174[0]=({struct Cyc_Absyn_IntType_struct
_tmp175;_tmp175.tag=Cyc_Absyn_IntType;_tmp175.f1=(void*)sgn;_tmp175.f2=(void*)
_tmp172;_tmp175;});_tmp174;});goto _LL365;_LL369: Cyc_Parse_err(_tag_arr("sign specification on non-integral type",
sizeof(unsigned char),40),last_loc);goto _LL365;_LL365:;}if(seen_size){void*
_tmp176=t;void*_tmp17E;_LL376: if((unsigned int)_tmp176 > 3?*((int*)_tmp176)== Cyc_Absyn_IntType:
0){_LL383: _tmp17E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp176)->f1;goto
_LL377;}else{goto _LL378;}_LL378: if((unsigned int)_tmp176 > 3?*((int*)_tmp176)== 
Cyc_Absyn_DoubleType: 0){goto _LL379;}else{goto _LL380;}_LL380: goto _LL381;_LL377: t=(
void*)({struct Cyc_Absyn_IntType_struct*_tmp180=_cycalloc(sizeof(struct Cyc_Absyn_IntType_struct));
_tmp180[0]=({struct Cyc_Absyn_IntType_struct _tmp181;_tmp181.tag=Cyc_Absyn_IntType;
_tmp181.f1=(void*)_tmp17E;_tmp181.f2=(void*)sz;_tmp181;});_tmp180;});goto _LL375;
_LL379: t=(void*)({struct Cyc_Absyn_DoubleType_struct*_tmp182=_cycalloc_atomic(
sizeof(struct Cyc_Absyn_DoubleType_struct));_tmp182[0]=({struct Cyc_Absyn_DoubleType_struct
_tmp183;_tmp183.tag=Cyc_Absyn_DoubleType;_tmp183.f1=1;_tmp183;});_tmp182;});goto
_LL375;_LL381: Cyc_Parse_err(_tag_arr("size qualifier on non-integral type",
sizeof(unsigned char),36),last_loc);goto _LL375;_LL375:;}}return({struct _tuple5
_tmp184;_tmp184.f1=t;_tmp184.f2=declopt;_tmp184;});}static struct Cyc_List_List*
Cyc_Parse_apply_tmss(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*ds,
struct Cyc_List_List*shared_atts){if(ds == 0){return 0;}{struct Cyc_Parse_Declarator*
_tmp185=(struct Cyc_Parse_Declarator*)ds->hd;struct _tuple1*_tmp186=_tmp185->id;
struct Cyc_List_List*_tmp189;struct Cyc_List_List*_tmp18B;void*_tmp18D;struct Cyc_Absyn_Tqual
_tmp18F;struct _tuple6 _tmp187=Cyc_Parse_apply_tms(tq,t,shared_atts,_tmp185->tms);
_LL400: _tmp18F=_tmp187.f1;goto _LL398;_LL398: _tmp18D=_tmp187.f2;goto _LL396;_LL396:
_tmp18B=_tmp187.f3;goto _LL394;_LL394: _tmp189=_tmp187.f4;goto _LL392;_LL392: return({
struct Cyc_List_List*_tmp191=_cycalloc(sizeof(struct Cyc_List_List));_tmp191->hd=({
struct _tuple7*_tmp192=_cycalloc(sizeof(struct _tuple7));_tmp192->f1=_tmp186;
_tmp192->f2=_tmp18F;_tmp192->f3=_tmp18D;_tmp192->f4=_tmp18B;_tmp192->f5=_tmp189;
_tmp192;});_tmp191->tl=Cyc_Parse_apply_tmss(_tmp18F,Cyc_Tcutil_copy_type(t),ds->tl,
shared_atts);_tmp191;});}}static struct _tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){if(tms == 0){return({
struct _tuple6 _tmp193;_tmp193.f1=tq;_tmp193.f2=t;_tmp193.f3=0;_tmp193.f4=atts;
_tmp193;});}{void*_tmp194=(void*)tms->hd;struct Cyc_Absyn_Exp*_tmp1A2;void*
_tmp1A4;struct Cyc_Position_Segment*_tmp1A6;struct Cyc_List_List*_tmp1A8;struct Cyc_Absyn_Tqual
_tmp1AA;void*_tmp1AC;void*_tmp1AE;struct Cyc_List_List*_tmp1B0;struct Cyc_Position_Segment*
_tmp1B2;_LL406: if(_tmp194 == (void*)Cyc_Absyn_Carray_mod){goto _LL407;}else{goto
_LL408;}_LL408: if((unsigned int)_tmp194 > 1?*((int*)_tmp194)== Cyc_Absyn_ConstArray_mod:
0){_LL419: _tmp1A2=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmp194)->f1;goto
_LL409;}else{goto _LL410;}_LL410: if((unsigned int)_tmp194 > 1?*((int*)_tmp194)== 
Cyc_Absyn_Function_mod: 0){_LL421: _tmp1A4=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp194)->f1;goto _LL411;}else{goto _LL412;}_LL412: if((unsigned int)_tmp194 > 1?*((
int*)_tmp194)== Cyc_Absyn_TypeParams_mod: 0){_LL425: _tmp1A8=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp194)->f1;goto _LL423;_LL423: _tmp1A6=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp194)->f2;goto _LL413;}else{goto _LL414;}_LL414: if((unsigned int)_tmp194 > 1?*((
int*)_tmp194)== Cyc_Absyn_Pointer_mod: 0){_LL431: _tmp1AE=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp194)->f1;goto _LL429;_LL429: _tmp1AC=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp194)->f2;goto _LL427;_LL427: _tmp1AA=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp194)->f3;goto _LL415;}else{goto _LL416;}_LL416: if((unsigned int)_tmp194 > 1?*((
int*)_tmp194)== Cyc_Absyn_Attributes_mod: 0){_LL435: _tmp1B2=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmp194)->f1;goto _LL433;_LL433: _tmp1B0=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmp194)->f2;goto _LL417;}else{goto _LL405;}_LL407: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),(
void*)({struct Cyc_Absyn_ArrayType_struct*_tmp1B4=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));
_tmp1B4[0]=({struct Cyc_Absyn_ArrayType_struct _tmp1B5;_tmp1B5.tag=Cyc_Absyn_ArrayType;
_tmp1B5.f1=(void*)t;_tmp1B5.f2=tq;_tmp1B5.f3=0;_tmp1B5;});_tmp1B4;}),atts,tms->tl);
_LL409: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp1B6=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp1B6[0]=({struct
Cyc_Absyn_ArrayType_struct _tmp1B7;_tmp1B7.tag=Cyc_Absyn_ArrayType;_tmp1B7.f1=(
void*)t;_tmp1B7.f2=tq;_tmp1B7.f3=(struct Cyc_Absyn_Exp*)_tmp1A2;_tmp1B7;});
_tmp1B6;}),atts,tms->tl);_LL411: {void*_tmp1B8=_tmp1A4;struct Cyc_List_List*
_tmp1BE;struct Cyc_Core_Opt*_tmp1C0;struct Cyc_Absyn_VarargInfo*_tmp1C2;int _tmp1C4;
struct Cyc_List_List*_tmp1C6;struct Cyc_Position_Segment*_tmp1C8;_LL442: if(*((int*)
_tmp1B8)== Cyc_Absyn_WithTypes){_LL455: _tmp1C6=((struct Cyc_Absyn_WithTypes_struct*)
_tmp1B8)->f1;goto _LL453;_LL453: _tmp1C4=((struct Cyc_Absyn_WithTypes_struct*)
_tmp1B8)->f2;goto _LL451;_LL451: _tmp1C2=((struct Cyc_Absyn_WithTypes_struct*)
_tmp1B8)->f3;goto _LL449;_LL449: _tmp1C0=((struct Cyc_Absyn_WithTypes_struct*)
_tmp1B8)->f4;goto _LL447;_LL447: _tmp1BE=((struct Cyc_Absyn_WithTypes_struct*)
_tmp1B8)->f5;goto _LL443;}else{goto _LL444;}_LL444: if(*((int*)_tmp1B8)== Cyc_Absyn_NoTypes){
_LL457: _tmp1C8=((struct Cyc_Absyn_NoTypes_struct*)_tmp1B8)->f2;goto _LL445;}else{
goto _LL441;}_LL443: {struct Cyc_List_List*typvars=0;struct Cyc_List_List*fn_atts=0;
struct Cyc_List_List*new_atts=0;{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd)){fn_atts=({struct Cyc_List_List*_tmp1CA=
_cycalloc(sizeof(struct Cyc_List_List));_tmp1CA->hd=(void*)((void*)as->hd);
_tmp1CA->tl=fn_atts;_tmp1CA;});}else{new_atts=({struct Cyc_List_List*_tmp1CB=
_cycalloc(sizeof(struct Cyc_List_List));_tmp1CB->hd=(void*)((void*)as->hd);
_tmp1CB->tl=new_atts;_tmp1CB;});}}}if(tms->tl != 0){void*_tmp1CC=(void*)((struct
Cyc_List_List*)_check_null(tms->tl))->hd;struct Cyc_List_List*_tmp1D2;_LL462: if((
unsigned int)_tmp1CC > 1?*((int*)_tmp1CC)== Cyc_Absyn_TypeParams_mod: 0){_LL467:
_tmp1D2=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp1CC)->f1;goto _LL463;}else{
goto _LL464;}_LL464: goto _LL465;_LL463: typvars=_tmp1D2;tms=tms->tl;goto _LL461;
_LL465: goto _LL461;_LL461:;}if(((((! _tmp1C4? _tmp1C2 == 0: 0)? _tmp1C6 != 0: 0)? _tmp1C6->tl
== 0: 0)?(*((struct _tuple2*)_tmp1C6->hd)).f1 == 0: 0)?(*((struct _tuple2*)_tmp1C6->hd)).f3
== (void*)Cyc_Absyn_VoidType: 0){_tmp1C6=0;}t=Cyc_Parse_array2ptr(t,0);((void(*)(
void(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,
_tmp1C6);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),Cyc_Absyn_function_typ(
typvars,_tmp1C0,t,_tmp1C6,_tmp1C4,_tmp1C2,_tmp1BE,fn_atts),new_atts,((struct Cyc_List_List*)
_check_null(tms))->tl);}_LL445:(int)_throw(Cyc_Parse_abort(_tag_arr("function declaration without parameter types",
sizeof(unsigned char),45),_tmp1C8));_LL441:;}_LL413: if(tms->tl == 0){return({
struct _tuple6 _tmp1D4;_tmp1D4.f1=tq;_tmp1D4.f2=t;_tmp1D4.f3=_tmp1A8;_tmp1D4.f4=
atts;_tmp1D4;});}(int)_throw(Cyc_Parse_abort(_tag_arr("type parameters must appear before function arguments in declarator",
sizeof(unsigned char),68),_tmp1A6));_LL415: {void*_tmp1D5=_tmp1AE;struct Cyc_Absyn_Exp*
_tmp1DD;struct Cyc_Absyn_Exp*_tmp1DF;_LL471: if((unsigned int)_tmp1D5 > 1?*((int*)
_tmp1D5)== Cyc_Absyn_NonNullable_ps: 0){_LL478: _tmp1DD=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmp1D5)->f1;goto _LL472;}else{goto _LL473;}_LL473: if((unsigned int)_tmp1D5 > 1?*((
int*)_tmp1D5)== Cyc_Absyn_Nullable_ps: 0){_LL480: _tmp1DF=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmp1D5)->f1;goto _LL474;}else{goto _LL475;}_LL475: if(_tmp1D5 == (void*)Cyc_Absyn_TaggedArray_ps){
goto _LL476;}else{goto _LL470;}_LL472: return Cyc_Parse_apply_tms(_tmp1AA,Cyc_Absyn_atb_typ(
t,_tmp1AC,tq,(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp1E1=_cycalloc(sizeof(
struct Cyc_Absyn_Upper_b_struct));_tmp1E1[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp1E2;_tmp1E2.tag=Cyc_Absyn_Upper_b;_tmp1E2.f1=_tmp1DD;_tmp1E2;});_tmp1E1;})),
atts,tms->tl);_LL474: return Cyc_Parse_apply_tms(_tmp1AA,Cyc_Absyn_starb_typ(t,
_tmp1AC,tq,(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp1E3=_cycalloc(sizeof(
struct Cyc_Absyn_Upper_b_struct));_tmp1E3[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp1E4;_tmp1E4.tag=Cyc_Absyn_Upper_b;_tmp1E4.f1=_tmp1DF;_tmp1E4;});_tmp1E3;})),
atts,tms->tl);_LL476: return Cyc_Parse_apply_tms(_tmp1AA,Cyc_Absyn_tagged_typ(t,
_tmp1AC,tq),atts,tms->tl);_LL470:;}_LL417: return Cyc_Parse_apply_tms(tq,t,Cyc_List_append(
atts,_tmp1B0),tms->tl);_LL405:;}}void*Cyc_Parse_speclist2typ(struct Cyc_List_List*
tss,struct Cyc_Position_Segment*loc){struct Cyc_Core_Opt*_tmp1E7;void*_tmp1E9;
struct _tuple5 _tmp1E5=Cyc_Parse_collapse_type_specifiers(tss,loc);_LL490: _tmp1E9=
_tmp1E5.f1;goto _LL488;_LL488: _tmp1E7=_tmp1E5.f2;goto _LL486;_LL486: if(_tmp1E7 != 0){({
void*_tmp1EB[0]={};Cyc_Tcutil_warn(loc,_tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof(unsigned char),54),_tag_arr(_tmp1EB,sizeof(void*),0));});}return _tmp1E9;}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct
Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp1EC=_cycalloc(sizeof(struct Cyc_Absyn_Decl_s_struct));_tmp1EC[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp1ED;_tmp1ED.tag=Cyc_Absyn_Decl_s;_tmp1ED.f1=d;_tmp1ED.f2=s;_tmp1ED;});
_tmp1EC;}),Cyc_Position_segment_join(d->loc,s->loc));}static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,
struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(
Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*
loc){struct Cyc_List_List*tss=ds->type_specs;struct Cyc_Absyn_Tqual tq=ds->tq;int
istypedef=0;void*s=(void*)Cyc_Absyn_Public;struct Cyc_List_List*atts=ds->attributes;
if(ds->is_inline){Cyc_Parse_err(_tag_arr("inline is only allowed on function definitions",
sizeof(unsigned char),47),loc);}if(tss == 0){Cyc_Parse_err(_tag_arr("missing type specifiers in declaration",
sizeof(unsigned char),39),loc);return 0;}if(ds->sc != 0){void*_tmp1EE=(void*)((
struct Cyc_Core_Opt*)_check_null(ds->sc))->v;_LL496: if(_tmp1EE == (void*)Cyc_Parse_Typedef_sc){
goto _LL497;}else{goto _LL498;}_LL498: if(_tmp1EE == (void*)Cyc_Parse_Extern_sc){
goto _LL499;}else{goto _LL500;}_LL500: if(_tmp1EE == (void*)Cyc_Parse_ExternC_sc){
goto _LL501;}else{goto _LL502;}_LL502: if(_tmp1EE == (void*)Cyc_Parse_Static_sc){
goto _LL503;}else{goto _LL504;}_LL504: if(_tmp1EE == (void*)Cyc_Parse_Auto_sc){goto
_LL505;}else{goto _LL506;}_LL506: if(_tmp1EE == (void*)Cyc_Parse_Register_sc){goto
_LL507;}else{goto _LL508;}_LL508: if(_tmp1EE == (void*)Cyc_Parse_Abstract_sc){goto
_LL509;}else{goto _LL495;}_LL497: istypedef=1;goto _LL495;_LL499: s=(void*)Cyc_Absyn_Extern;
goto _LL495;_LL501: s=(void*)Cyc_Absyn_ExternC;goto _LL495;_LL503: s=(void*)Cyc_Absyn_Static;
goto _LL495;_LL505: s=(void*)Cyc_Absyn_Public;goto _LL495;_LL507: s=(void*)Cyc_Absyn_Public;
goto _LL495;_LL509: s=(void*)Cyc_Absyn_Abstract;goto _LL495;_LL495:;}{struct Cyc_List_List*
_tmp200;struct Cyc_List_List*_tmp202;struct _tuple0 _tmp1FE=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(ids);_LL515: _tmp202=_tmp1FE.f1;goto _LL513;
_LL513: _tmp200=_tmp1FE.f2;goto _LL511;_LL511: {int exps_empty=1;{struct Cyc_List_List*
es=_tmp200;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;break;}}}{struct _tuple5 _tmp204=Cyc_Parse_collapse_type_specifiers(
tss,loc);if(_tmp202 == 0){struct Cyc_Core_Opt*_tmp207;void*_tmp209;struct _tuple5
_tmp205=_tmp204;_LL522: _tmp209=_tmp205.f1;goto _LL520;_LL520: _tmp207=_tmp205.f2;
goto _LL518;_LL518: if(_tmp207 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
_tmp207->v;{void*_tmp20B=(void*)d->r;struct Cyc_Absyn_Enumdecl*_tmp217;struct Cyc_Absyn_Structdecl*
_tmp219;struct Cyc_Absyn_Uniondecl*_tmp21B;struct Cyc_Absyn_Tuniondecl*_tmp21D;
_LL525: if(*((int*)_tmp20B)== Cyc_Absyn_Enum_d){_LL536: _tmp217=((struct Cyc_Absyn_Enum_d_struct*)
_tmp20B)->f1;goto _LL526;}else{goto _LL527;}_LL527: if(*((int*)_tmp20B)== Cyc_Absyn_Struct_d){
_LL538: _tmp219=((struct Cyc_Absyn_Struct_d_struct*)_tmp20B)->f1;goto _LL528;}else{
goto _LL529;}_LL529: if(*((int*)_tmp20B)== Cyc_Absyn_Union_d){_LL540: _tmp21B=((
struct Cyc_Absyn_Union_d_struct*)_tmp20B)->f1;goto _LL530;}else{goto _LL531;}_LL531:
if(*((int*)_tmp20B)== Cyc_Absyn_Tunion_d){_LL542: _tmp21D=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp20B)->f1;goto _LL532;}else{goto _LL533;}_LL533: goto _LL534;_LL526:(void*)(
_tmp217->sc=(void*)s);if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on enum",
sizeof(unsigned char),23),loc);}goto _LL524;_LL528:(void*)(_tmp219->sc=(void*)s);
_tmp219->attributes=atts;goto _LL524;_LL530:(void*)(_tmp21B->sc=(void*)s);_tmp21B->attributes=
atts;goto _LL524;_LL532:(void*)(_tmp21D->sc=(void*)s);if(atts != 0){Cyc_Parse_err(
_tag_arr("bad attributes on tunion",sizeof(unsigned char),25),loc);}goto _LL524;
_LL534: Cyc_Parse_err(_tag_arr("bad declaration",sizeof(unsigned char),16),loc);
return 0;_LL524:;}return({struct Cyc_List_List*_tmp21F=_cycalloc(sizeof(struct Cyc_List_List));
_tmp21F->hd=d;_tmp21F->tl=0;_tmp21F;});}else{void*_tmp220=_tmp209;struct Cyc_List_List*
_tmp230;struct _tuple1*_tmp232;struct Cyc_Absyn_TunionInfo _tmp234;void*_tmp236;
struct Cyc_Absyn_Tuniondecl**_tmp238;struct Cyc_Absyn_TunionInfo _tmp23A;struct Cyc_List_List*
_tmp23C;void*_tmp23E;struct Cyc_Absyn_UnknownTunionInfo _tmp240;int _tmp242;struct
_tuple1*_tmp244;struct Cyc_List_List*_tmp246;struct _tuple1*_tmp248;struct _tuple1*
_tmp24A;struct Cyc_List_List*_tmp24C;_LL546: if((unsigned int)_tmp220 > 3?*((int*)
_tmp220)== Cyc_Absyn_StructType: 0){_LL563: _tmp232=((struct Cyc_Absyn_StructType_struct*)
_tmp220)->f1;goto _LL561;_LL561: _tmp230=((struct Cyc_Absyn_StructType_struct*)
_tmp220)->f2;goto _LL547;}else{goto _LL548;}_LL548: if((unsigned int)_tmp220 > 3?*((
int*)_tmp220)== Cyc_Absyn_TunionType: 0){_LL565: _tmp234=((struct Cyc_Absyn_TunionType_struct*)
_tmp220)->f1;_LL567: _tmp236=(void*)_tmp234.tunion_info;if(*((int*)_tmp236)== Cyc_Absyn_KnownTunion){
_LL569: _tmp238=((struct Cyc_Absyn_KnownTunion_struct*)_tmp236)->f1;goto _LL549;}
else{goto _LL550;}}else{goto _LL550;}_LL550: if((unsigned int)_tmp220 > 3?*((int*)
_tmp220)== Cyc_Absyn_TunionType: 0){_LL571: _tmp23A=((struct Cyc_Absyn_TunionType_struct*)
_tmp220)->f1;_LL575: _tmp23E=(void*)_tmp23A.tunion_info;if(*((int*)_tmp23E)== Cyc_Absyn_UnknownTunion){
_LL577: _tmp240=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp23E)->f1;_LL581:
_tmp244=_tmp240.name;goto _LL579;_LL579: _tmp242=_tmp240.is_xtunion;goto _LL573;}
else{goto _LL552;}_LL573: _tmp23C=_tmp23A.targs;goto _LL551;}else{goto _LL552;}
_LL552: if((unsigned int)_tmp220 > 3?*((int*)_tmp220)== Cyc_Absyn_UnionType: 0){
_LL585: _tmp248=((struct Cyc_Absyn_UnionType_struct*)_tmp220)->f1;goto _LL583;
_LL583: _tmp246=((struct Cyc_Absyn_UnionType_struct*)_tmp220)->f2;goto _LL553;}
else{goto _LL554;}_LL554: if((unsigned int)_tmp220 > 3?*((int*)_tmp220)== Cyc_Absyn_EnumType:
0){_LL587: _tmp24A=((struct Cyc_Absyn_EnumType_struct*)_tmp220)->f1;goto _LL555;}
else{goto _LL556;}_LL556: if((unsigned int)_tmp220 > 3?*((int*)_tmp220)== Cyc_Absyn_AnonEnumType:
0){_LL589: _tmp24C=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp220)->f1;goto _LL557;}
else{goto _LL558;}_LL558: goto _LL559;_LL547: {struct Cyc_List_List*_tmp24E=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp230);struct Cyc_Absyn_Structdecl*_tmp24F=({struct Cyc_Absyn_Structdecl*
_tmp253=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));_tmp253->sc=(void*)s;
_tmp253->name=({struct Cyc_Core_Opt*_tmp254=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp254->v=(struct _tuple1*)_check_null(_tmp232);_tmp254;});_tmp253->tvs=_tmp24E;
_tmp253->fields=0;_tmp253->attributes=0;_tmp253;});if(atts != 0){Cyc_Parse_err(
_tag_arr("bad attributes on struct",sizeof(unsigned char),25),loc);}return({
struct Cyc_List_List*_tmp250=_cycalloc(sizeof(struct Cyc_List_List));_tmp250->hd=
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Struct_d_struct*_tmp251=_cycalloc(
sizeof(struct Cyc_Absyn_Struct_d_struct));_tmp251[0]=({struct Cyc_Absyn_Struct_d_struct
_tmp252;_tmp252.tag=Cyc_Absyn_Struct_d;_tmp252.f1=_tmp24F;_tmp252;});_tmp251;}),
loc);_tmp250->tl=0;_tmp250;});}_LL549: if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on tunion",
sizeof(unsigned char),25),loc);}return({struct Cyc_List_List*_tmp255=_cycalloc(
sizeof(struct Cyc_List_List));_tmp255->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp256=_cycalloc(sizeof(struct Cyc_Absyn_Tunion_d_struct));_tmp256[0]=({struct
Cyc_Absyn_Tunion_d_struct _tmp257;_tmp257.tag=Cyc_Absyn_Tunion_d;_tmp257.f1=*
_tmp238;_tmp257;});_tmp256;}),loc);_tmp255->tl=0;_tmp255;});_LL551: {struct Cyc_List_List*
_tmp258=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp23C);struct Cyc_Absyn_Decl*_tmp259=Cyc_Absyn_tunion_decl(s,_tmp244,
_tmp258,0,_tmp242,loc);if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on tunion",
sizeof(unsigned char),25),loc);}return({struct Cyc_List_List*_tmp25A=_cycalloc(
sizeof(struct Cyc_List_List));_tmp25A->hd=_tmp259;_tmp25A->tl=0;_tmp25A;});}
_LL553: {struct Cyc_List_List*_tmp25B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp246);struct Cyc_Absyn_Uniondecl*
_tmp25C=({struct Cyc_Absyn_Uniondecl*_tmp261=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl));
_tmp261->sc=(void*)s;_tmp261->name=({struct Cyc_Core_Opt*_tmp262=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp262->v=(struct _tuple1*)_check_null(_tmp248);
_tmp262;});_tmp261->tvs=_tmp25B;_tmp261->fields=0;_tmp261->attributes=0;_tmp261;});
if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on union",sizeof(
unsigned char),24),loc);}return({struct Cyc_List_List*_tmp25D=_cycalloc(sizeof(
struct Cyc_List_List));_tmp25D->hd=({struct Cyc_Absyn_Decl*_tmp25E=_cycalloc(
sizeof(struct Cyc_Absyn_Decl));_tmp25E->r=(void*)((void*)({struct Cyc_Absyn_Union_d_struct*
_tmp25F=_cycalloc(sizeof(struct Cyc_Absyn_Union_d_struct));_tmp25F[0]=({struct Cyc_Absyn_Union_d_struct
_tmp260;_tmp260.tag=Cyc_Absyn_Union_d;_tmp260.f1=_tmp25C;_tmp260;});_tmp25F;}));
_tmp25E->loc=loc;_tmp25E;});_tmp25D->tl=0;_tmp25D;});}_LL555: {struct Cyc_Absyn_Enumdecl*
_tmp263=({struct Cyc_Absyn_Enumdecl*_tmp268=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));
_tmp268->sc=(void*)s;_tmp268->name=_tmp24A;_tmp268->fields=0;_tmp268;});if(atts
!= 0){Cyc_Parse_err(_tag_arr("bad attributes on enum",sizeof(unsigned char),23),
loc);}return({struct Cyc_List_List*_tmp264=_cycalloc(sizeof(struct Cyc_List_List));
_tmp264->hd=({struct Cyc_Absyn_Decl*_tmp265=_cycalloc(sizeof(struct Cyc_Absyn_Decl));
_tmp265->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp266=_cycalloc(
sizeof(struct Cyc_Absyn_Enum_d_struct));_tmp266[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp267;_tmp267.tag=Cyc_Absyn_Enum_d;_tmp267.f1=_tmp263;_tmp267;});_tmp266;}));
_tmp265->loc=loc;_tmp265;});_tmp264->tl=0;_tmp264;});}_LL557: {struct Cyc_Absyn_Enumdecl*
_tmp269=({struct Cyc_Absyn_Enumdecl*_tmp26E=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));
_tmp26E->sc=(void*)s;_tmp26E->name=Cyc_Parse_gensym_enum();_tmp26E->fields=({
struct Cyc_Core_Opt*_tmp26F=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp26F->v=
_tmp24C;_tmp26F;});_tmp26E;});if(atts != 0){Cyc_Parse_err(_tag_arr("bad attributes on enum",
sizeof(unsigned char),23),loc);}return({struct Cyc_List_List*_tmp26A=_cycalloc(
sizeof(struct Cyc_List_List));_tmp26A->hd=({struct Cyc_Absyn_Decl*_tmp26B=
_cycalloc(sizeof(struct Cyc_Absyn_Decl));_tmp26B->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp26C=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_struct));_tmp26C[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp26D;_tmp26D.tag=Cyc_Absyn_Enum_d;_tmp26D.f1=_tmp269;_tmp26D;});_tmp26C;}));
_tmp26B->loc=loc;_tmp26B;});_tmp26A->tl=0;_tmp26A;});}_LL559: Cyc_Parse_err(
_tag_arr("missing declarator",sizeof(unsigned char),19),loc);return 0;_LL545:;}}
else{void*t=_tmp204.f1;struct Cyc_List_List*_tmp270=Cyc_Parse_apply_tmss(tq,t,
_tmp202,atts);if(istypedef){if(! exps_empty){Cyc_Parse_err(_tag_arr("initializer in typedef declaration",
sizeof(unsigned char),35),loc);}{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple7*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp270);if(_tmp204.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(
_tmp204.f2)->v;{void*_tmp271=(void*)d->r;struct Cyc_Absyn_Structdecl*_tmp27D;
struct Cyc_Absyn_Tuniondecl*_tmp27F;struct Cyc_Absyn_Uniondecl*_tmp281;struct Cyc_Absyn_Enumdecl*
_tmp283;_LL627: if(*((int*)_tmp271)== Cyc_Absyn_Struct_d){_LL638: _tmp27D=((struct
Cyc_Absyn_Struct_d_struct*)_tmp271)->f1;goto _LL628;}else{goto _LL629;}_LL629: if(*((
int*)_tmp271)== Cyc_Absyn_Tunion_d){_LL640: _tmp27F=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp271)->f1;goto _LL630;}else{goto _LL631;}_LL631: if(*((int*)_tmp271)== Cyc_Absyn_Union_d){
_LL642: _tmp281=((struct Cyc_Absyn_Union_d_struct*)_tmp271)->f1;goto _LL632;}else{
goto _LL633;}_LL633: if(*((int*)_tmp271)== Cyc_Absyn_Enum_d){_LL644: _tmp283=((
struct Cyc_Absyn_Enum_d_struct*)_tmp271)->f1;goto _LL634;}else{goto _LL635;}_LL635:
goto _LL636;_LL628:(void*)(_tmp27D->sc=(void*)s);_tmp27D->attributes=atts;atts=0;
goto _LL626;_LL630:(void*)(_tmp27F->sc=(void*)s);goto _LL626;_LL632:(void*)(
_tmp281->sc=(void*)s);goto _LL626;_LL634:(void*)(_tmp283->sc=(void*)s);goto _LL626;
_LL636: Cyc_Parse_err(_tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof(unsigned char),70),loc);goto _LL626;_LL626:;}decls=({struct Cyc_List_List*
_tmp285=_cycalloc(sizeof(struct Cyc_List_List));_tmp285->hd=d;_tmp285->tl=decls;
_tmp285;});}if(atts != 0){Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp287;_tmp287.tag=Cyc_Std_String_pa;_tmp287.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp286[1]={& _tmp287};Cyc_Std_aprintf(_tag_arr("bad attribute %s in typedef",
sizeof(unsigned char),28),_tag_arr(_tmp286,sizeof(void*),1));}}),loc);}return
decls;}}else{if(_tmp204.f2 != 0){Cyc_Parse_unimp2(_tag_arr("nested type declaration within declarator",
sizeof(unsigned char),42),loc);}{struct Cyc_List_List*decls=0;{struct Cyc_List_List*
_tmp288=_tmp270;for(0;_tmp288 != 0;(_tmp288=_tmp288->tl,_tmp200=_tmp200->tl)){
struct _tuple7 _tmp28B;struct Cyc_List_List*_tmp28C;struct Cyc_List_List*_tmp28E;
void*_tmp290;struct Cyc_Absyn_Tqual _tmp292;struct _tuple1*_tmp294;struct _tuple7*
_tmp289=(struct _tuple7*)_tmp288->hd;_tmp28B=*_tmp289;_LL661: _tmp294=_tmp28B.f1;
goto _LL659;_LL659: _tmp292=_tmp28B.f2;goto _LL657;_LL657: _tmp290=_tmp28B.f3;goto
_LL655;_LL655: _tmp28E=_tmp28B.f4;goto _LL653;_LL653: _tmp28C=_tmp28B.f5;goto _LL650;
_LL650: if(_tmp28E != 0){({void*_tmp296[0]={};Cyc_Tcutil_warn(loc,_tag_arr("bad type params, ignoring",
sizeof(unsigned char),26),_tag_arr(_tmp296,sizeof(void*),0));});}if(_tmp200 == 0){((
int(*)(struct _tagged_arr msg,struct Cyc_Position_Segment*sg))Cyc_Parse_abort)(
_tag_arr("unexpected NULL in parse!",sizeof(unsigned char),26),loc);}{struct Cyc_Absyn_Exp*
_tmp297=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp200))->hd;
struct Cyc_Absyn_Vardecl*_tmp298=Cyc_Absyn_new_vardecl(_tmp294,_tmp290,_tmp297);
_tmp298->tq=_tmp292;(void*)(_tmp298->sc=(void*)s);_tmp298->attributes=_tmp28C;{
struct Cyc_Absyn_Decl*_tmp299=({struct Cyc_Absyn_Decl*_tmp29B=_cycalloc(sizeof(
struct Cyc_Absyn_Decl));_tmp29B->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp29C=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_struct));_tmp29C[0]=({struct Cyc_Absyn_Var_d_struct
_tmp29D;_tmp29D.tag=Cyc_Absyn_Var_d;_tmp29D.f1=_tmp298;_tmp29D;});_tmp29C;}));
_tmp29B->loc=loc;_tmp29B;});decls=({struct Cyc_List_List*_tmp29A=_cycalloc(
sizeof(struct Cyc_List_List));_tmp29A->hd=_tmp299;_tmp29A->tl=decls;_tmp29A;});}}}}
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);}}}}}}}
static void*Cyc_Parse_id_to_kind(struct _tagged_arr s,struct Cyc_Position_Segment*
loc){if(Cyc_Std_strlen(s)!= 1){Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp29F;_tmp29F.tag=Cyc_Std_String_pa;_tmp29F.f1=(struct _tagged_arr)s;{void*
_tmp29E[1]={& _tmp29F};Cyc_Std_aprintf(_tag_arr("bad kind: %s",sizeof(
unsigned char),13),_tag_arr(_tmp29E,sizeof(void*),1));}}),loc);return(void*)Cyc_Absyn_BoxKind;}
else{switch(*((const unsigned char*)_check_unknown_subscript(s,sizeof(
unsigned char),0))){case 'A': _LL672: return(void*)Cyc_Absyn_AnyKind;case 'M': _LL673:
return(void*)Cyc_Absyn_MemKind;case 'B': _LL674: return(void*)Cyc_Absyn_BoxKind;
case 'R': _LL675: return(void*)Cyc_Absyn_RgnKind;case 'E': _LL676: return(void*)Cyc_Absyn_EffKind;
default: _LL677: Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp2A8;_tmp2A8.tag=Cyc_Std_String_pa;_tmp2A8.f1=(struct _tagged_arr)s;{void*
_tmp2A7[1]={& _tmp2A8};Cyc_Std_aprintf(_tag_arr("bad kind: %s",sizeof(
unsigned char),13),_tag_arr(_tmp2A7,sizeof(void*),1));}}),loc);return(void*)Cyc_Absyn_BoxKind;}}}
static struct Cyc_List_List*Cyc_Parse_attopt_to_tms(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*atts,struct Cyc_List_List*tms){if(atts == 0){return tms;}else{
return({struct Cyc_List_List*_tmp2A9=_cycalloc(sizeof(struct Cyc_List_List));
_tmp2A9->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp2AA=
_cycalloc(sizeof(struct Cyc_Absyn_Attributes_mod_struct));_tmp2AA[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp2AB;_tmp2AB.tag=Cyc_Absyn_Attributes_mod;_tmp2AB.f1=loc;_tmp2AB.f2=atts;
_tmp2AB;});_tmp2AA;}));_tmp2A9->tl=tms;_tmp2A9;});}}static struct Cyc_Absyn_Decl*
Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*loc,struct _tuple7*t){
struct _tuple7 _tmp2AE;struct Cyc_List_List*_tmp2AF;struct Cyc_List_List*_tmp2B1;
void*_tmp2B3;struct Cyc_Absyn_Tqual _tmp2B5;struct _tuple1*_tmp2B7;struct _tuple7*
_tmp2AC=t;_tmp2AE=*_tmp2AC;_LL696: _tmp2B7=_tmp2AE.f1;goto _LL694;_LL694: _tmp2B5=
_tmp2AE.f2;goto _LL692;_LL692: _tmp2B3=_tmp2AE.f3;goto _LL690;_LL690: _tmp2B1=
_tmp2AE.f4;goto _LL688;_LL688: _tmp2AF=_tmp2AE.f5;goto _LL685;_LL685: Cyc_Lex_register_typedef(
_tmp2B7);if(_tmp2AF != 0){Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp2BA;_tmp2BA.tag=Cyc_Std_String_pa;_tmp2BA.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp2AF->hd);{void*_tmp2B9[1]={& _tmp2BA};Cyc_Std_aprintf(_tag_arr("bad attribute %s within typedef",
sizeof(unsigned char),32),_tag_arr(_tmp2B9,sizeof(void*),1));}}),loc);}return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Typedef_d_struct*_tmp2BB=_cycalloc(sizeof(struct Cyc_Absyn_Typedef_d_struct));
_tmp2BB[0]=({struct Cyc_Absyn_Typedef_d_struct _tmp2BC;_tmp2BC.tag=Cyc_Absyn_Typedef_d;
_tmp2BC.f1=({struct Cyc_Absyn_Typedefdecl*_tmp2BD=_cycalloc(sizeof(struct Cyc_Absyn_Typedefdecl));
_tmp2BD->name=_tmp2B7;_tmp2BD->tvs=_tmp2B1;_tmp2BD->defn=(void*)_tmp2B3;_tmp2BD;});
_tmp2BC;});_tmp2BB;}),loc);}unsigned char Cyc_AbstractDeclarator_tok[27]="\000\000\000\000AbstractDeclarator_tok";
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
_tmp2BF[15]="$(sign_t,int)@";static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{_tmp2BF,_tmp2BF,_tmp2BF + 15}};struct _tuple15*Cyc_yyget_Int_tok(
void*yy1){struct _tuple15*yyzzz;{void*_tmp2C0=yy1;struct _tuple15*_tmp2C6;_LL706:
if(*((void**)_tmp2C0)== Cyc_Int_tok){_LL711: _tmp2C6=((struct Cyc_Int_tok_struct*)
_tmp2C0)->f1;goto _LL707;}else{goto _LL708;}_LL708: goto _LL709;_LL707: yyzzz=_tmp2C6;
goto _LL705;_LL709:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LL705:;}return yyzzz;}
static unsigned char _tmp2C9[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp2C9,_tmp2C9,_tmp2C9 + 9}};struct _tagged_arr Cyc_yyget_String_tok(
void*yy1){struct _tagged_arr yyzzz;{void*_tmp2CA=yy1;struct _tagged_arr _tmp2D0;
_LL716: if(*((void**)_tmp2CA)== Cyc_String_tok){_LL721: _tmp2D0=((struct Cyc_String_tok_struct*)
_tmp2CA)->f1;goto _LL717;}else{goto _LL718;}_LL718: goto _LL719;_LL717: yyzzz=_tmp2D0;
goto _LL715;_LL719:(int)_throw((void*)& Cyc_yyfail_String_tok);_LL715:;}return
yyzzz;}static unsigned char _tmp2D3[5]="char";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp2D3,_tmp2D3,_tmp2D3 + 5}};
unsigned char Cyc_yyget_Char_tok(void*yy1){unsigned char yyzzz;{void*_tmp2D4=yy1;
unsigned char _tmp2DA;_LL726: if(*((void**)_tmp2D4)== Cyc_Char_tok){_LL731: _tmp2DA=((
struct Cyc_Char_tok_struct*)_tmp2D4)->f1;goto _LL727;}else{goto _LL728;}_LL728: goto
_LL729;_LL727: yyzzz=_tmp2DA;goto _LL725;_LL729:(int)_throw((void*)& Cyc_yyfail_Char_tok);
_LL725:;}return yyzzz;}static unsigned char _tmp2DD[20]="tunion Pointer_Sort";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={Cyc_Core_Failure,{
_tmp2DD,_tmp2DD,_tmp2DD + 20}};void*Cyc_yyget_Pointer_Sort_tok(void*yy1){void*
yyzzz;{void*_tmp2DE=yy1;void*_tmp2E4;_LL736: if(*((void**)_tmp2DE)== Cyc_Pointer_Sort_tok){
_LL741: _tmp2E4=(void*)((struct Cyc_Pointer_Sort_tok_struct*)_tmp2DE)->f1;goto
_LL737;}else{goto _LL738;}_LL738: goto _LL739;_LL737: yyzzz=_tmp2E4;goto _LL735;
_LL739:(int)_throw((void*)& Cyc_yyfail_Pointer_Sort_tok);_LL735:;}return yyzzz;}
static unsigned char _tmp2E7[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{_tmp2E7,_tmp2E7,_tmp2E7 + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(
void*yy1){struct Cyc_Absyn_Exp*yyzzz;{void*_tmp2E8=yy1;struct Cyc_Absyn_Exp*
_tmp2EE;_LL746: if(*((void**)_tmp2E8)== Cyc_Exp_tok){_LL751: _tmp2EE=((struct Cyc_Exp_tok_struct*)
_tmp2E8)->f1;goto _LL747;}else{goto _LL748;}_LL748: goto _LL749;_LL747: yyzzz=_tmp2EE;
goto _LL745;_LL749:(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LL745:;}return yyzzz;}
static unsigned char _tmp2F1[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={Cyc_Core_Failure,{_tmp2F1,_tmp2F1,_tmp2F1 + 14}};struct
Cyc_List_List*Cyc_yyget_ExpList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp2F2=yy1;struct Cyc_List_List*_tmp2F8;_LL756: if(*((void**)_tmp2F2)== Cyc_ExpList_tok){
_LL761: _tmp2F8=((struct Cyc_ExpList_tok_struct*)_tmp2F2)->f1;goto _LL757;}else{
goto _LL758;}_LL758: goto _LL759;_LL757: yyzzz=_tmp2F8;goto _LL755;_LL759:(int)_throw((
void*)& Cyc_yyfail_ExpList_tok);_LL755:;}return yyzzz;}static unsigned char _tmp2FB[
39]="list_t<$(list_t<designator_t>,exp_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{_tmp2FB,_tmp2FB,_tmp2FB + 39}};
struct Cyc_List_List*Cyc_yyget_InitializerList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp2FC=yy1;struct Cyc_List_List*_tmp302;_LL766: if(*((void**)_tmp2FC)
== Cyc_InitializerList_tok){_LL771: _tmp302=((struct Cyc_InitializerList_tok_struct*)
_tmp2FC)->f1;goto _LL767;}else{goto _LL768;}_LL768: goto _LL769;_LL767: yyzzz=_tmp302;
goto _LL765;_LL769:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);_LL765:;}
return yyzzz;}static unsigned char _tmp305[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={Cyc_Core_Failure,{_tmp305,_tmp305,_tmp305 + 9}};void*Cyc_yyget_Primop_tok(
void*yy1){void*yyzzz;{void*_tmp306=yy1;void*_tmp30C;_LL776: if(*((void**)_tmp306)
== Cyc_Primop_tok){_LL781: _tmp30C=(void*)((struct Cyc_Primop_tok_struct*)_tmp306)->f1;
goto _LL777;}else{goto _LL778;}_LL778: goto _LL779;_LL777: yyzzz=_tmp30C;goto _LL775;
_LL779:(int)_throw((void*)& Cyc_yyfail_Primop_tok);_LL775:;}return yyzzz;}static
unsigned char _tmp30F[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp30F,_tmp30F,_tmp30F + 16}};struct
Cyc_Core_Opt*Cyc_yyget_Primopopt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp310=yy1;struct Cyc_Core_Opt*_tmp316;_LL786: if(*((void**)_tmp310)== Cyc_Primopopt_tok){
_LL791: _tmp316=((struct Cyc_Primopopt_tok_struct*)_tmp310)->f1;goto _LL787;}else{
goto _LL788;}_LL788: goto _LL789;_LL787: yyzzz=_tmp316;goto _LL785;_LL789:(int)_throw((
void*)& Cyc_yyfail_Primopopt_tok);_LL785:;}return yyzzz;}static unsigned char
_tmp319[7]="qvar_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{_tmp319,_tmp319,_tmp319 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(
void*yy1){struct _tuple1*yyzzz;{void*_tmp31A=yy1;struct _tuple1*_tmp320;_LL796: if(*((
void**)_tmp31A)== Cyc_QualId_tok){_LL801: _tmp320=((struct Cyc_QualId_tok_struct*)
_tmp31A)->f1;goto _LL797;}else{goto _LL798;}_LL798: goto _LL799;_LL797: yyzzz=_tmp320;
goto _LL795;_LL799:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL795:;}return
yyzzz;}static unsigned char _tmp323[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp323,_tmp323,_tmp323 + 7}};struct Cyc_Absyn_Stmt*
Cyc_yyget_Stmt_tok(void*yy1){struct Cyc_Absyn_Stmt*yyzzz;{void*_tmp324=yy1;struct
Cyc_Absyn_Stmt*_tmp32A;_LL806: if(*((void**)_tmp324)== Cyc_Stmt_tok){_LL811:
_tmp32A=((struct Cyc_Stmt_tok_struct*)_tmp324)->f1;goto _LL807;}else{goto _LL808;}
_LL808: goto _LL809;_LL807: yyzzz=_tmp32A;goto _LL805;_LL809:(int)_throw((void*)& Cyc_yyfail_Stmt_tok);
_LL805:;}return yyzzz;}static unsigned char _tmp32D[24]="list_t<switch_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={Cyc_Core_Failure,{
_tmp32D,_tmp32D,_tmp32D + 24}};struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp32E=yy1;struct Cyc_List_List*
_tmp334;_LL816: if(*((void**)_tmp32E)== Cyc_SwitchClauseList_tok){_LL821: _tmp334=((
struct Cyc_SwitchClauseList_tok_struct*)_tmp32E)->f1;goto _LL817;}else{goto _LL818;}
_LL818: goto _LL819;_LL817: yyzzz=_tmp334;goto _LL815;_LL819:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL815:;}return yyzzz;}static unsigned char _tmp337[25]="list_t<switchC_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={Cyc_Core_Failure,{
_tmp337,_tmp337,_tmp337 + 25}};struct Cyc_List_List*Cyc_yyget_SwitchCClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp338=yy1;struct Cyc_List_List*
_tmp33E;_LL826: if(*((void**)_tmp338)== Cyc_SwitchCClauseList_tok){_LL831: _tmp33E=((
struct Cyc_SwitchCClauseList_tok_struct*)_tmp338)->f1;goto _LL827;}else{goto _LL828;}
_LL828: goto _LL829;_LL827: yyzzz=_tmp33E;goto _LL825;_LL829:(int)_throw((void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL825:;}return yyzzz;}static unsigned char _tmp341[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp341,_tmp341,_tmp341 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp342=yy1;
struct Cyc_Absyn_Pat*_tmp348;_LL836: if(*((void**)_tmp342)== Cyc_Pattern_tok){
_LL841: _tmp348=((struct Cyc_Pattern_tok_struct*)_tmp342)->f1;goto _LL837;}else{
goto _LL838;}_LL838: goto _LL839;_LL837: yyzzz=_tmp348;goto _LL835;_LL839:(int)_throw((
void*)& Cyc_yyfail_Pattern_tok);_LL835:;}return yyzzz;}static unsigned char _tmp34B[
14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure,{_tmp34B,_tmp34B,_tmp34B + 14}};struct Cyc_List_List*Cyc_yyget_PatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp34C=yy1;struct Cyc_List_List*
_tmp352;_LL846: if(*((void**)_tmp34C)== Cyc_PatternList_tok){_LL851: _tmp352=((
struct Cyc_PatternList_tok_struct*)_tmp34C)->f1;goto _LL847;}else{goto _LL848;}
_LL848: goto _LL849;_LL847: yyzzz=_tmp352;goto _LL845;_LL849:(int)_throw((void*)& Cyc_yyfail_PatternList_tok);
_LL845:;}return yyzzz;}static unsigned char _tmp355[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{
_tmp355,_tmp355,_tmp355 + 31}};struct _tuple13*Cyc_yyget_FieldPattern_tok(void*yy1){
struct _tuple13*yyzzz;{void*_tmp356=yy1;struct _tuple13*_tmp35C;_LL856: if(*((void**)
_tmp356)== Cyc_FieldPattern_tok){_LL861: _tmp35C=((struct Cyc_FieldPattern_tok_struct*)
_tmp356)->f1;goto _LL857;}else{goto _LL858;}_LL858: goto _LL859;_LL857: yyzzz=_tmp35C;
goto _LL855;_LL859:(int)_throw((void*)& Cyc_yyfail_FieldPattern_tok);_LL855:;}
return yyzzz;}static unsigned char _tmp35F[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp35F,_tmp35F,_tmp35F + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp360=yy1;struct Cyc_List_List*
_tmp366;_LL866: if(*((void**)_tmp360)== Cyc_FieldPatternList_tok){_LL871: _tmp366=((
struct Cyc_FieldPatternList_tok_struct*)_tmp360)->f1;goto _LL867;}else{goto _LL868;}
_LL868: goto _LL869;_LL867: yyzzz=_tmp366;goto _LL865;_LL869:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL865:;}return yyzzz;}static unsigned char _tmp369[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={Cyc_Core_Failure,{_tmp369,_tmp369,_tmp369 + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok(void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp36A=yy1;
struct Cyc_Absyn_Fndecl*_tmp370;_LL876: if(*((void**)_tmp36A)== Cyc_FnDecl_tok){
_LL881: _tmp370=((struct Cyc_FnDecl_tok_struct*)_tmp36A)->f1;goto _LL877;}else{goto
_LL878;}_LL878: goto _LL879;_LL877: yyzzz=_tmp370;goto _LL875;_LL879:(int)_throw((
void*)& Cyc_yyfail_FnDecl_tok);_LL875:;}return yyzzz;}static unsigned char _tmp373[
15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={
Cyc_Core_Failure,{_tmp373,_tmp373,_tmp373 + 15}};struct Cyc_List_List*Cyc_yyget_DeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp374=yy1;struct Cyc_List_List*
_tmp37A;_LL886: if(*((void**)_tmp374)== Cyc_DeclList_tok){_LL891: _tmp37A=((struct
Cyc_DeclList_tok_struct*)_tmp374)->f1;goto _LL887;}else{goto _LL888;}_LL888: goto
_LL889;_LL887: yyzzz=_tmp37A;goto _LL885;_LL889:(int)_throw((void*)& Cyc_yyfail_DeclList_tok);
_LL885:;}return yyzzz;}static unsigned char _tmp37D[12]="decl_spec_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={Cyc_Core_Failure,{_tmp37D,
_tmp37D,_tmp37D + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(
void*yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{void*_tmp37E=yy1;struct Cyc_Parse_Declaration_spec*
_tmp384;_LL896: if(*((void**)_tmp37E)== Cyc_DeclSpec_tok){_LL901: _tmp384=((struct
Cyc_DeclSpec_tok_struct*)_tmp37E)->f1;goto _LL897;}else{goto _LL898;}_LL898: goto
_LL899;_LL897: yyzzz=_tmp384;goto _LL895;_LL899:(int)_throw((void*)& Cyc_yyfail_DeclSpec_tok);
_LL895:;}return yyzzz;}static unsigned char _tmp387[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{
_tmp387,_tmp387,_tmp387 + 27}};struct _tuple14*Cyc_yyget_InitDecl_tok(void*yy1){
struct _tuple14*yyzzz;{void*_tmp388=yy1;struct _tuple14*_tmp38E;_LL906: if(*((void**)
_tmp388)== Cyc_InitDecl_tok){_LL911: _tmp38E=((struct Cyc_InitDecl_tok_struct*)
_tmp388)->f1;goto _LL907;}else{goto _LL908;}_LL908: goto _LL909;_LL907: yyzzz=_tmp38E;
goto _LL905;_LL909:(int)_throw((void*)& Cyc_yyfail_InitDecl_tok);_LL905:;}return
yyzzz;}static unsigned char _tmp391[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp391,_tmp391,_tmp391 + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp392=yy1;struct Cyc_List_List*
_tmp398;_LL916: if(*((void**)_tmp392)== Cyc_InitDeclList_tok){_LL921: _tmp398=((
struct Cyc_InitDeclList_tok_struct*)_tmp392)->f1;goto _LL917;}else{goto _LL918;}
_LL918: goto _LL919;_LL917: yyzzz=_tmp398;goto _LL915;_LL919:(int)_throw((void*)& Cyc_yyfail_InitDeclList_tok);
_LL915:;}return yyzzz;}static unsigned char _tmp39B[16]="storage_class_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{
_tmp39B,_tmp39B,_tmp39B + 16}};void*Cyc_yyget_StorageClass_tok(void*yy1){void*
yyzzz;{void*_tmp39C=yy1;void*_tmp3A2;_LL926: if(*((void**)_tmp39C)== Cyc_StorageClass_tok){
_LL931: _tmp3A2=(void*)((struct Cyc_StorageClass_tok_struct*)_tmp39C)->f1;goto
_LL927;}else{goto _LL928;}_LL928: goto _LL929;_LL927: yyzzz=_tmp3A2;goto _LL925;
_LL929:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);_LL925:;}return yyzzz;}
static unsigned char _tmp3A5[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{_tmp3A5,_tmp3A5,_tmp3A5 + 17}};
void*Cyc_yyget_TypeSpecifier_tok(void*yy1){void*yyzzz;{void*_tmp3A6=yy1;void*
_tmp3AC;_LL936: if(*((void**)_tmp3A6)== Cyc_TypeSpecifier_tok){_LL941: _tmp3AC=(
void*)((struct Cyc_TypeSpecifier_tok_struct*)_tmp3A6)->f1;goto _LL937;}else{goto
_LL938;}_LL938: goto _LL939;_LL937: yyzzz=_tmp3AC;goto _LL935;_LL939:(int)_throw((
void*)& Cyc_yyfail_TypeSpecifier_tok);_LL935:;}return yyzzz;}static unsigned char
_tmp3AF[18]="struct_or_union_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={
Cyc_Core_Failure,{_tmp3AF,_tmp3AF,_tmp3AF + 18}};enum Cyc_Parse_Struct_or_union
Cyc_yyget_StructOrUnion_tok(void*yy1){enum Cyc_Parse_Struct_or_union yyzzz;{void*
_tmp3B0=yy1;enum Cyc_Parse_Struct_or_union _tmp3B6;_LL946: if(*((void**)_tmp3B0)== 
Cyc_StructOrUnion_tok){_LL951: _tmp3B6=((struct Cyc_StructOrUnion_tok_struct*)
_tmp3B0)->f1;goto _LL947;}else{goto _LL948;}_LL948: goto _LL949;_LL947: yyzzz=_tmp3B6;
goto _LL945;_LL949:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);_LL945:;}
return yyzzz;}static unsigned char _tmp3B9[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp3B9,_tmp3B9,_tmp3B9 + 8}};struct
Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*
_tmp3BA=yy1;struct Cyc_Absyn_Tqual _tmp3C0;_LL956: if(*((void**)_tmp3BA)== Cyc_TypeQual_tok){
_LL961: _tmp3C0=((struct Cyc_TypeQual_tok_struct*)_tmp3BA)->f1;goto _LL957;}else{
goto _LL958;}_LL958: goto _LL959;_LL957: yyzzz=_tmp3C0;goto _LL955;_LL959:(int)_throw((
void*)& Cyc_yyfail_TypeQual_tok);_LL955:;}return yyzzz;}static unsigned char _tmp3C3[
22]="list_t<structfield_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={
Cyc_Core_Failure,{_tmp3C3,_tmp3C3,_tmp3C3 + 22}};struct Cyc_List_List*Cyc_yyget_StructFieldDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp3C4=yy1;struct Cyc_List_List*
_tmp3CA;_LL966: if(*((void**)_tmp3C4)== Cyc_StructFieldDeclList_tok){_LL971:
_tmp3CA=((struct Cyc_StructFieldDeclList_tok_struct*)_tmp3C4)->f1;goto _LL967;}
else{goto _LL968;}_LL968: goto _LL969;_LL967: yyzzz=_tmp3CA;goto _LL965;_LL969:(int)
_throw((void*)& Cyc_yyfail_StructFieldDeclList_tok);_LL965:;}return yyzzz;}static
unsigned char _tmp3CD[30]="list_t<list_t<structfield_t>>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclListList_tok={Cyc_Core_Failure,{_tmp3CD,_tmp3CD,
_tmp3CD + 30}};struct Cyc_List_List*Cyc_yyget_StructFieldDeclListList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp3CE=yy1;struct Cyc_List_List*_tmp3D4;_LL976:
if(*((void**)_tmp3CE)== Cyc_StructFieldDeclListList_tok){_LL981: _tmp3D4=((struct
Cyc_StructFieldDeclListList_tok_struct*)_tmp3CE)->f1;goto _LL977;}else{goto _LL978;}
_LL978: goto _LL979;_LL977: yyzzz=_tmp3D4;goto _LL975;_LL979:(int)_throw((void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL975:;}return yyzzz;}static unsigned char _tmp3D7[24]="list_t<type_modifier_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{
_tmp3D7,_tmp3D7,_tmp3D7 + 24}};struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp3D8=yy1;struct Cyc_List_List*
_tmp3DE;_LL986: if(*((void**)_tmp3D8)== Cyc_TypeModifierList_tok){_LL991: _tmp3DE=((
struct Cyc_TypeModifierList_tok_struct*)_tmp3D8)->f1;goto _LL987;}else{goto _LL988;}
_LL988: goto _LL989;_LL987: yyzzz=_tmp3DE;goto _LL985;_LL989:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);
_LL985:;}return yyzzz;}static unsigned char _tmp3E1[13]="declarator_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp3E1,
_tmp3E1,_tmp3E1 + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_Declarator_tok(void*
yy1){struct Cyc_Parse_Declarator*yyzzz;{void*_tmp3E2=yy1;struct Cyc_Parse_Declarator*
_tmp3E8;_LL996: if(*((void**)_tmp3E2)== Cyc_Declarator_tok){_LL1001: _tmp3E8=((
struct Cyc_Declarator_tok_struct*)_tmp3E2)->f1;goto _LL997;}else{goto _LL998;}
_LL998: goto _LL999;_LL997: yyzzz=_tmp3E8;goto _LL995;_LL999:(int)_throw((void*)& Cyc_yyfail_Declarator_tok);
_LL995:;}return yyzzz;}static unsigned char _tmp3EB[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{
_tmp3EB,_tmp3EB,_tmp3EB + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(
void*yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp3EC=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp3F2;_LL1006: if(*((void**)_tmp3EC)== Cyc_AbstractDeclarator_tok){_LL1011:
_tmp3F2=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp3EC)->f1;goto _LL1007;}
else{goto _LL1008;}_LL1008: goto _LL1009;_LL1007: yyzzz=_tmp3F2;goto _LL1005;_LL1009:(
int)_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);_LL1005:;}return yyzzz;}
static unsigned char _tmp3F5[5]="bool";static struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={
Cyc_Core_Failure,{_tmp3F5,_tmp3F5,_tmp3F5 + 5}};int Cyc_yyget_Bool_tok(void*yy1){
int yyzzz;{void*_tmp3F6=yy1;int _tmp3FC;_LL1016: if(*((void**)_tmp3F6)== Cyc_Bool_tok){
_LL1021: _tmp3FC=((struct Cyc_Bool_tok_struct*)_tmp3F6)->f1;goto _LL1017;}else{goto
_LL1018;}_LL1018: goto _LL1019;_LL1017: yyzzz=_tmp3FC;goto _LL1015;_LL1019:(int)
_throw((void*)& Cyc_yyfail_Bool_tok);_LL1015:;}return yyzzz;}static unsigned char
_tmp3FF[8]="scope_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Scope_tok={
Cyc_Core_Failure,{_tmp3FF,_tmp3FF,_tmp3FF + 8}};void*Cyc_yyget_Scope_tok(void*yy1){
void*yyzzz;{void*_tmp400=yy1;void*_tmp406;_LL1026: if(*((void**)_tmp400)== Cyc_Scope_tok){
_LL1031: _tmp406=(void*)((struct Cyc_Scope_tok_struct*)_tmp400)->f1;goto _LL1027;}
else{goto _LL1028;}_LL1028: goto _LL1029;_LL1027: yyzzz=_tmp406;goto _LL1025;_LL1029:(
int)_throw((void*)& Cyc_yyfail_Scope_tok);_LL1025:;}return yyzzz;}static
unsigned char _tmp409[14]="tunionfield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={
Cyc_Core_Failure,{_tmp409,_tmp409,_tmp409 + 14}};struct Cyc_Absyn_Tunionfield*Cyc_yyget_TunionField_tok(
void*yy1){struct Cyc_Absyn_Tunionfield*yyzzz;{void*_tmp40A=yy1;struct Cyc_Absyn_Tunionfield*
_tmp410;_LL1036: if(*((void**)_tmp40A)== Cyc_TunionField_tok){_LL1041: _tmp410=((
struct Cyc_TunionField_tok_struct*)_tmp40A)->f1;goto _LL1037;}else{goto _LL1038;}
_LL1038: goto _LL1039;_LL1037: yyzzz=_tmp410;goto _LL1035;_LL1039:(int)_throw((void*)&
Cyc_yyfail_TunionField_tok);_LL1035:;}return yyzzz;}static unsigned char _tmp413[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{
_tmp413,_tmp413,_tmp413 + 22}};struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp414=yy1;struct Cyc_List_List*
_tmp41A;_LL1046: if(*((void**)_tmp414)== Cyc_TunionFieldList_tok){_LL1051: _tmp41A=((
struct Cyc_TunionFieldList_tok_struct*)_tmp414)->f1;goto _LL1047;}else{goto _LL1048;}
_LL1048: goto _LL1049;_LL1047: yyzzz=_tmp41A;goto _LL1045;_LL1049:(int)_throw((void*)&
Cyc_yyfail_TunionFieldList_tok);_LL1045:;}return yyzzz;}static unsigned char
_tmp41D[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{_tmp41D,_tmp41D,_tmp41D + 50}};
struct _tuple17*Cyc_yyget_QualSpecList_tok(void*yy1){struct _tuple17*yyzzz;{void*
_tmp41E=yy1;struct _tuple17*_tmp424;_LL1056: if(*((void**)_tmp41E)== Cyc_QualSpecList_tok){
_LL1061: _tmp424=((struct Cyc_QualSpecList_tok_struct*)_tmp41E)->f1;goto _LL1057;}
else{goto _LL1058;}_LL1058: goto _LL1059;_LL1057: yyzzz=_tmp424;goto _LL1055;_LL1059:(
int)_throw((void*)& Cyc_yyfail_QualSpecList_tok);_LL1055:;}return yyzzz;}static
unsigned char _tmp427[14]="list_t<var_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={
Cyc_Core_Failure,{_tmp427,_tmp427,_tmp427 + 14}};struct Cyc_List_List*Cyc_yyget_IdList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp428=yy1;struct Cyc_List_List*
_tmp42E;_LL1066: if(*((void**)_tmp428)== Cyc_IdList_tok){_LL1071: _tmp42E=((struct
Cyc_IdList_tok_struct*)_tmp428)->f1;goto _LL1067;}else{goto _LL1068;}_LL1068: goto
_LL1069;_LL1067: yyzzz=_tmp42E;goto _LL1065;_LL1069:(int)_throw((void*)& Cyc_yyfail_IdList_tok);
_LL1065:;}return yyzzz;}static unsigned char _tmp431[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={Cyc_Core_Failure,{
_tmp431,_tmp431,_tmp431 + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(void*yy1){
struct _tuple2*yyzzz;{void*_tmp432=yy1;struct _tuple2*_tmp438;_LL1076: if(*((void**)
_tmp432)== Cyc_ParamDecl_tok){_LL1081: _tmp438=((struct Cyc_ParamDecl_tok_struct*)
_tmp432)->f1;goto _LL1077;}else{goto _LL1078;}_LL1078: goto _LL1079;_LL1077: yyzzz=
_tmp438;goto _LL1075;_LL1079:(int)_throw((void*)& Cyc_yyfail_ParamDecl_tok);
_LL1075:;}return yyzzz;}static unsigned char _tmp43B[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp43B,_tmp43B,_tmp43B + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp43C=yy1;struct Cyc_List_List*
_tmp442;_LL1086: if(*((void**)_tmp43C)== Cyc_ParamDeclList_tok){_LL1091: _tmp442=((
struct Cyc_ParamDeclList_tok_struct*)_tmp43C)->f1;goto _LL1087;}else{goto _LL1088;}
_LL1088: goto _LL1089;_LL1087: yyzzz=_tmp442;goto _LL1085;_LL1089:(int)_throw((void*)&
Cyc_yyfail_ParamDeclList_tok);_LL1085:;}return yyzzz;}static unsigned char _tmp445[
107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp445,
_tmp445,_tmp445 + 107}};struct _tuple16*Cyc_yyget_YY1(void*yy1){struct _tuple16*
yyzzz;{void*_tmp446=yy1;struct _tuple16*_tmp44C;_LL1096: if(*((void**)_tmp446)== 
Cyc_YY1){_LL1101: _tmp44C=((struct Cyc_YY1_struct*)_tmp446)->f1;goto _LL1097;}else{
goto _LL1098;}_LL1098: goto _LL1099;_LL1097: yyzzz=_tmp44C;goto _LL1095;_LL1099:(int)
_throw((void*)& Cyc_yyfail_YY1);_LL1095:;}return yyzzz;}static unsigned char _tmp44F[
15]="list_t<type_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={
Cyc_Core_Failure,{_tmp44F,_tmp44F,_tmp44F + 15}};struct Cyc_List_List*Cyc_yyget_TypeList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp450=yy1;struct Cyc_List_List*
_tmp456;_LL1106: if(*((void**)_tmp450)== Cyc_TypeList_tok){_LL1111: _tmp456=((
struct Cyc_TypeList_tok_struct*)_tmp450)->f1;goto _LL1107;}else{goto _LL1108;}
_LL1108: goto _LL1109;_LL1107: yyzzz=_tmp456;goto _LL1105;_LL1109:(int)_throw((void*)&
Cyc_yyfail_TypeList_tok);_LL1105:;}return yyzzz;}static unsigned char _tmp459[21]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{
_tmp459,_tmp459,_tmp459 + 21}};struct Cyc_List_List*Cyc_yyget_DesignatorList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp45A=yy1;struct Cyc_List_List*
_tmp460;_LL1116: if(*((void**)_tmp45A)== Cyc_DesignatorList_tok){_LL1121: _tmp460=((
struct Cyc_DesignatorList_tok_struct*)_tmp45A)->f1;goto _LL1117;}else{goto _LL1118;}
_LL1118: goto _LL1119;_LL1117: yyzzz=_tmp460;goto _LL1115;_LL1119:(int)_throw((void*)&
Cyc_yyfail_DesignatorList_tok);_LL1115:;}return yyzzz;}static unsigned char _tmp463[
13]="designator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={
Cyc_Core_Failure,{_tmp463,_tmp463,_tmp463 + 13}};void*Cyc_yyget_Designator_tok(
void*yy1){void*yyzzz;{void*_tmp464=yy1;void*_tmp46A;_LL1126: if(*((void**)_tmp464)
== Cyc_Designator_tok){_LL1131: _tmp46A=(void*)((struct Cyc_Designator_tok_struct*)
_tmp464)->f1;goto _LL1127;}else{goto _LL1128;}_LL1128: goto _LL1129;_LL1127: yyzzz=
_tmp46A;goto _LL1125;_LL1129:(int)_throw((void*)& Cyc_yyfail_Designator_tok);
_LL1125:;}return yyzzz;}static unsigned char _tmp46D[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{_tmp46D,_tmp46D,_tmp46D + 7}};void*Cyc_yyget_Kind_tok(
void*yy1){void*yyzzz;{void*_tmp46E=yy1;void*_tmp474;_LL1136: if(*((void**)_tmp46E)
== Cyc_Kind_tok){_LL1141: _tmp474=(void*)((struct Cyc_Kind_tok_struct*)_tmp46E)->f1;
goto _LL1137;}else{goto _LL1138;}_LL1138: goto _LL1139;_LL1137: yyzzz=_tmp474;goto
_LL1135;_LL1139:(int)_throw((void*)& Cyc_yyfail_Kind_tok);_LL1135:;}return yyzzz;}
static unsigned char _tmp477[7]="type_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={
Cyc_Core_Failure,{_tmp477,_tmp477,_tmp477 + 7}};void*Cyc_yyget_Type_tok(void*yy1){
void*yyzzz;{void*_tmp478=yy1;void*_tmp47E;_LL1146: if(*((void**)_tmp478)== Cyc_Type_tok){
_LL1151: _tmp47E=(void*)((struct Cyc_Type_tok_struct*)_tmp478)->f1;goto _LL1147;}
else{goto _LL1148;}_LL1148: goto _LL1149;_LL1147: yyzzz=_tmp47E;goto _LL1145;_LL1149:(
int)_throw((void*)& Cyc_yyfail_Type_tok);_LL1145:;}return yyzzz;}static
unsigned char _tmp481[20]="list_t<attribute_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={Cyc_Core_Failure,{_tmp481,_tmp481,_tmp481 + 20}};
struct Cyc_List_List*Cyc_yyget_AttributeList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp482=yy1;struct Cyc_List_List*_tmp488;_LL1156: if(*((void**)_tmp482)
== Cyc_AttributeList_tok){_LL1161: _tmp488=((struct Cyc_AttributeList_tok_struct*)
_tmp482)->f1;goto _LL1157;}else{goto _LL1158;}_LL1158: goto _LL1159;_LL1157: yyzzz=
_tmp488;goto _LL1155;_LL1159:(int)_throw((void*)& Cyc_yyfail_AttributeList_tok);
_LL1155:;}return yyzzz;}static unsigned char _tmp48B[12]="attribute_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{_tmp48B,
_tmp48B,_tmp48B + 12}};void*Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{void*
_tmp48C=yy1;void*_tmp492;_LL1166: if(*((void**)_tmp48C)== Cyc_Attribute_tok){
_LL1171: _tmp492=(void*)((struct Cyc_Attribute_tok_struct*)_tmp48C)->f1;goto
_LL1167;}else{goto _LL1168;}_LL1168: goto _LL1169;_LL1167: yyzzz=_tmp492;goto _LL1165;
_LL1169:(int)_throw((void*)& Cyc_yyfail_Attribute_tok);_LL1165:;}return yyzzz;}
static unsigned char _tmp495[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={Cyc_Core_Failure,{_tmp495,_tmp495,_tmp495 + 12}};struct
Cyc_Absyn_Enumfield*Cyc_yyget_Enumfield_tok(void*yy1){struct Cyc_Absyn_Enumfield*
yyzzz;{void*_tmp496=yy1;struct Cyc_Absyn_Enumfield*_tmp49C;_LL1176: if(*((void**)
_tmp496)== Cyc_Enumfield_tok){_LL1181: _tmp49C=((struct Cyc_Enumfield_tok_struct*)
_tmp496)->f1;goto _LL1177;}else{goto _LL1178;}_LL1178: goto _LL1179;_LL1177: yyzzz=
_tmp49C;goto _LL1175;_LL1179:(int)_throw((void*)& Cyc_yyfail_Enumfield_tok);
_LL1175:;}return yyzzz;}static unsigned char _tmp49F[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{
_tmp49F,_tmp49F,_tmp49F + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp4A0=yy1;struct Cyc_List_List*
_tmp4A6;_LL1186: if(*((void**)_tmp4A0)== Cyc_EnumfieldList_tok){_LL1191: _tmp4A6=((
struct Cyc_EnumfieldList_tok_struct*)_tmp4A0)->f1;goto _LL1187;}else{goto _LL1188;}
_LL1188: goto _LL1189;_LL1187: yyzzz=_tmp4A6;goto _LL1185;_LL1189:(int)_throw((void*)&
Cyc_yyfail_EnumfieldList_tok);_LL1185:;}return yyzzz;}static unsigned char _tmp4A9[
14]="opt_t<type_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeOpt_tok={
Cyc_Core_Failure,{_tmp4A9,_tmp4A9,_tmp4A9 + 14}};struct Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(
void*yy1){struct Cyc_Core_Opt*yyzzz;{void*_tmp4AA=yy1;struct Cyc_Core_Opt*_tmp4B0;
_LL1196: if(*((void**)_tmp4AA)== Cyc_TypeOpt_tok){_LL1201: _tmp4B0=((struct Cyc_TypeOpt_tok_struct*)
_tmp4AA)->f1;goto _LL1197;}else{goto _LL1198;}_LL1198: goto _LL1199;_LL1197: yyzzz=
_tmp4B0;goto _LL1195;_LL1199:(int)_throw((void*)& Cyc_yyfail_TypeOpt_tok);_LL1195:;}
return yyzzz;}static unsigned char _tmp4B3[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{_tmp4B3,
_tmp4B3,_tmp4B3 + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp4B4=yy1;struct Cyc_List_List*_tmp4BA;_LL1206:
if(*((void**)_tmp4B4)== Cyc_Rgnorder_tok){_LL1211: _tmp4BA=((struct Cyc_Rgnorder_tok_struct*)
_tmp4B4)->f1;goto _LL1207;}else{goto _LL1208;}_LL1208: goto _LL1209;_LL1207: yyzzz=
_tmp4BA;goto _LL1205;_LL1209:(int)_throw((void*)& Cyc_yyfail_Rgnorder_tok);_LL1205:;}
return yyzzz;}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int
last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){return({struct Cyc_Yyltype
_tmp4BC;_tmp4BC.timestamp=0;_tmp4BC.first_line=0;_tmp4BC.first_column=0;_tmp4BC.last_line=
0;_tmp4BC.last_column=0;_tmp4BC;});}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
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
static unsigned char _tmp4BD[2]="$";static unsigned char _tmp4BE[6]="error";static
unsigned char _tmp4BF[12]="$undefined.";static unsigned char _tmp4C0[5]="AUTO";
static unsigned char _tmp4C1[9]="REGISTER";static unsigned char _tmp4C2[7]="STATIC";
static unsigned char _tmp4C3[7]="EXTERN";static unsigned char _tmp4C4[8]="TYPEDEF";
static unsigned char _tmp4C5[5]="VOID";static unsigned char _tmp4C6[5]="CHAR";static
unsigned char _tmp4C7[6]="SHORT";static unsigned char _tmp4C8[4]="INT";static
unsigned char _tmp4C9[5]="LONG";static unsigned char _tmp4CA[6]="FLOAT";static
unsigned char _tmp4CB[7]="DOUBLE";static unsigned char _tmp4CC[7]="SIGNED";static
unsigned char _tmp4CD[9]="UNSIGNED";static unsigned char _tmp4CE[6]="CONST";static
unsigned char _tmp4CF[9]="VOLATILE";static unsigned char _tmp4D0[9]="RESTRICT";
static unsigned char _tmp4D1[7]="STRUCT";static unsigned char _tmp4D2[6]="UNION";
static unsigned char _tmp4D3[5]="CASE";static unsigned char _tmp4D4[8]="DEFAULT";
static unsigned char _tmp4D5[7]="INLINE";static unsigned char _tmp4D6[7]="SIZEOF";
static unsigned char _tmp4D7[9]="OFFSETOF";static unsigned char _tmp4D8[3]="IF";
static unsigned char _tmp4D9[5]="ELSE";static unsigned char _tmp4DA[7]="SWITCH";
static unsigned char _tmp4DB[6]="WHILE";static unsigned char _tmp4DC[3]="DO";static
unsigned char _tmp4DD[4]="FOR";static unsigned char _tmp4DE[5]="GOTO";static
unsigned char _tmp4DF[9]="CONTINUE";static unsigned char _tmp4E0[6]="BREAK";static
unsigned char _tmp4E1[7]="RETURN";static unsigned char _tmp4E2[5]="ENUM";static
unsigned char _tmp4E3[8]="NULL_kw";static unsigned char _tmp4E4[4]="LET";static
unsigned char _tmp4E5[6]="THROW";static unsigned char _tmp4E6[4]="TRY";static
unsigned char _tmp4E7[6]="CATCH";static unsigned char _tmp4E8[4]="NEW";static
unsigned char _tmp4E9[9]="ABSTRACT";static unsigned char _tmp4EA[9]="FALLTHRU";
static unsigned char _tmp4EB[6]="USING";static unsigned char _tmp4EC[10]="NAMESPACE";
static unsigned char _tmp4ED[7]="TUNION";static unsigned char _tmp4EE[8]="XTUNION";
static unsigned char _tmp4EF[9]="FORARRAY";static unsigned char _tmp4F0[5]="FILL";
static unsigned char _tmp4F1[8]="CODEGEN";static unsigned char _tmp4F2[4]="CUT";
static unsigned char _tmp4F3[7]="SPLICE";static unsigned char _tmp4F4[7]="MALLOC";
static unsigned char _tmp4F5[8]="RMALLOC";static unsigned char _tmp4F6[7]="CALLOC";
static unsigned char _tmp4F7[8]="RCALLOC";static unsigned char _tmp4F8[9]="REGION_T";
static unsigned char _tmp4F9[9]="SIZEOF_T";static unsigned char _tmp4FA[7]="REGION";
static unsigned char _tmp4FB[5]="RNEW";static unsigned char _tmp4FC[8]="REGIONS";
static unsigned char _tmp4FD[4]="GEN";static unsigned char _tmp4FE[7]="PTR_OP";
static unsigned char _tmp4FF[7]="INC_OP";static unsigned char _tmp500[7]="DEC_OP";
static unsigned char _tmp501[8]="LEFT_OP";static unsigned char _tmp502[9]="RIGHT_OP";
static unsigned char _tmp503[6]="LE_OP";static unsigned char _tmp504[6]="GE_OP";
static unsigned char _tmp505[6]="EQ_OP";static unsigned char _tmp506[6]="NE_OP";
static unsigned char _tmp507[7]="AND_OP";static unsigned char _tmp508[6]="OR_OP";
static unsigned char _tmp509[11]="MUL_ASSIGN";static unsigned char _tmp50A[11]="DIV_ASSIGN";
static unsigned char _tmp50B[11]="MOD_ASSIGN";static unsigned char _tmp50C[11]="ADD_ASSIGN";
static unsigned char _tmp50D[11]="SUB_ASSIGN";static unsigned char _tmp50E[12]="LEFT_ASSIGN";
static unsigned char _tmp50F[13]="RIGHT_ASSIGN";static unsigned char _tmp510[11]="AND_ASSIGN";
static unsigned char _tmp511[11]="XOR_ASSIGN";static unsigned char _tmp512[10]="OR_ASSIGN";
static unsigned char _tmp513[9]="ELLIPSIS";static unsigned char _tmp514[11]="LEFT_RIGHT";
static unsigned char _tmp515[12]="COLON_COLON";static unsigned char _tmp516[11]="IDENTIFIER";
static unsigned char _tmp517[17]="INTEGER_CONSTANT";static unsigned char _tmp518[7]="STRING";
static unsigned char _tmp519[19]="CHARACTER_CONSTANT";static unsigned char _tmp51A[
18]="FLOATING_CONSTANT";static unsigned char _tmp51B[9]="TYPE_VAR";static
unsigned char _tmp51C[16]="QUAL_IDENTIFIER";static unsigned char _tmp51D[18]="QUAL_TYPEDEF_NAME";
static unsigned char _tmp51E[10]="ATTRIBUTE";static unsigned char _tmp51F[4]="';'";
static unsigned char _tmp520[4]="'{'";static unsigned char _tmp521[4]="'}'";static
unsigned char _tmp522[4]="'='";static unsigned char _tmp523[4]="'('";static
unsigned char _tmp524[4]="')'";static unsigned char _tmp525[4]="','";static
unsigned char _tmp526[4]="'_'";static unsigned char _tmp527[4]="'$'";static
unsigned char _tmp528[4]="'<'";static unsigned char _tmp529[4]="'>'";static
unsigned char _tmp52A[4]="':'";static unsigned char _tmp52B[4]="'.'";static
unsigned char _tmp52C[4]="'['";static unsigned char _tmp52D[4]="']'";static
unsigned char _tmp52E[4]="'*'";static unsigned char _tmp52F[4]="'@'";static
unsigned char _tmp530[4]="'?'";static unsigned char _tmp531[4]="'+'";static
unsigned char _tmp532[4]="'-'";static unsigned char _tmp533[4]="'&'";static
unsigned char _tmp534[4]="'|'";static unsigned char _tmp535[4]="'^'";static
unsigned char _tmp536[4]="'/'";static unsigned char _tmp537[4]="'%'";static
unsigned char _tmp538[4]="'~'";static unsigned char _tmp539[4]="'!'";static
unsigned char _tmp53A[5]="prog";static unsigned char _tmp53B[17]="translation_unit";
static unsigned char _tmp53C[21]="external_declaration";static unsigned char _tmp53D[
20]="function_definition";static unsigned char _tmp53E[21]="function_definition2";
static unsigned char _tmp53F[13]="using_action";static unsigned char _tmp540[15]="unusing_action";
static unsigned char _tmp541[17]="namespace_action";static unsigned char _tmp542[19]="unnamespace_action";
static unsigned char _tmp543[12]="declaration";static unsigned char _tmp544[17]="declaration_list";
static unsigned char _tmp545[23]="declaration_specifiers";static unsigned char
_tmp546[24]="storage_class_specifier";static unsigned char _tmp547[15]="attributes_opt";
static unsigned char _tmp548[11]="attributes";static unsigned char _tmp549[15]="attribute_list";
static unsigned char _tmp54A[10]="attribute";static unsigned char _tmp54B[15]="type_specifier";
static unsigned char _tmp54C[5]="kind";static unsigned char _tmp54D[15]="type_qualifier";
static unsigned char _tmp54E[15]="enum_specifier";static unsigned char _tmp54F[11]="enum_field";
static unsigned char _tmp550[22]="enum_declaration_list";static unsigned char
_tmp551[26]="struct_or_union_specifier";static unsigned char _tmp552[16]="type_params_opt";
static unsigned char _tmp553[16]="struct_or_union";static unsigned char _tmp554[24]="struct_declaration_list";
static unsigned char _tmp555[25]="struct_declaration_list0";static unsigned char
_tmp556[21]="init_declarator_list";static unsigned char _tmp557[22]="init_declarator_list0";
static unsigned char _tmp558[16]="init_declarator";static unsigned char _tmp559[19]="struct_declaration";
static unsigned char _tmp55A[25]="specifier_qualifier_list";static unsigned char
_tmp55B[23]="struct_declarator_list";static unsigned char _tmp55C[24]="struct_declarator_list0";
static unsigned char _tmp55D[18]="struct_declarator";static unsigned char _tmp55E[17]="tunion_specifier";
static unsigned char _tmp55F[18]="tunion_or_xtunion";static unsigned char _tmp560[17]="tunionfield_list";
static unsigned char _tmp561[18]="tunionfield_scope";static unsigned char _tmp562[12]="tunionfield";
static unsigned char _tmp563[11]="declarator";static unsigned char _tmp564[18]="direct_declarator";
static unsigned char _tmp565[8]="pointer";static unsigned char _tmp566[13]="pointer_char";
static unsigned char _tmp567[8]="rgn_opt";static unsigned char _tmp568[4]="rgn";
static unsigned char _tmp569[20]="type_qualifier_list";static unsigned char _tmp56A[
20]="parameter_type_list";static unsigned char _tmp56B[9]="type_var";static
unsigned char _tmp56C[16]="optional_effect";static unsigned char _tmp56D[19]="optional_rgn_order";
static unsigned char _tmp56E[10]="rgn_order";static unsigned char _tmp56F[16]="optional_inject";
static unsigned char _tmp570[11]="effect_set";static unsigned char _tmp571[14]="atomic_effect";
static unsigned char _tmp572[11]="region_set";static unsigned char _tmp573[15]="parameter_list";
static unsigned char _tmp574[22]="parameter_declaration";static unsigned char
_tmp575[16]="identifier_list";static unsigned char _tmp576[17]="identifier_list0";
static unsigned char _tmp577[12]="initializer";static unsigned char _tmp578[18]="array_initializer";
static unsigned char _tmp579[17]="initializer_list";static unsigned char _tmp57A[12]="designation";
static unsigned char _tmp57B[16]="designator_list";static unsigned char _tmp57C[11]="designator";
static unsigned char _tmp57D[10]="type_name";static unsigned char _tmp57E[14]="any_type_name";
static unsigned char _tmp57F[15]="type_name_list";static unsigned char _tmp580[20]="abstract_declarator";
static unsigned char _tmp581[27]="direct_abstract_declarator";static unsigned char
_tmp582[10]="statement";static unsigned char _tmp583[18]="labeled_statement";
static unsigned char _tmp584[21]="expression_statement";static unsigned char _tmp585[
19]="compound_statement";static unsigned char _tmp586[16]="block_item_list";static
unsigned char _tmp587[20]="selection_statement";static unsigned char _tmp588[15]="switch_clauses";
static unsigned char _tmp589[16]="switchC_clauses";static unsigned char _tmp58A[20]="iteration_statement";
static unsigned char _tmp58B[15]="jump_statement";static unsigned char _tmp58C[8]="pattern";
static unsigned char _tmp58D[19]="tuple_pattern_list";static unsigned char _tmp58E[
20]="tuple_pattern_list0";static unsigned char _tmp58F[14]="field_pattern";static
unsigned char _tmp590[19]="field_pattern_list";static unsigned char _tmp591[20]="field_pattern_list0";
static unsigned char _tmp592[11]="expression";static unsigned char _tmp593[22]="assignment_expression";
static unsigned char _tmp594[20]="assignment_operator";static unsigned char _tmp595[
23]="conditional_expression";static unsigned char _tmp596[20]="constant_expression";
static unsigned char _tmp597[22]="logical_or_expression";static unsigned char
_tmp598[23]="logical_and_expression";static unsigned char _tmp599[24]="inclusive_or_expression";
static unsigned char _tmp59A[24]="exclusive_or_expression";static unsigned char
_tmp59B[15]="and_expression";static unsigned char _tmp59C[20]="equality_expression";
static unsigned char _tmp59D[22]="relational_expression";static unsigned char
_tmp59E[17]="shift_expression";static unsigned char _tmp59F[20]="additive_expression";
static unsigned char _tmp5A0[26]="multiplicative_expression";static unsigned char
_tmp5A1[16]="cast_expression";static unsigned char _tmp5A2[17]="unary_expression";
static unsigned char _tmp5A3[15]="unary_operator";static unsigned char _tmp5A4[19]="postfix_expression";
static unsigned char _tmp5A5[19]="primary_expression";static unsigned char _tmp5A6[
25]="argument_expression_list";static unsigned char _tmp5A7[26]="argument_expression_list0";
static unsigned char _tmp5A8[9]="constant";static unsigned char _tmp5A9[20]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[237]={{_tmp4BD,_tmp4BD,_tmp4BD + 2},{_tmp4BE,
_tmp4BE,_tmp4BE + 6},{_tmp4BF,_tmp4BF,_tmp4BF + 12},{_tmp4C0,_tmp4C0,_tmp4C0 + 5},{
_tmp4C1,_tmp4C1,_tmp4C1 + 9},{_tmp4C2,_tmp4C2,_tmp4C2 + 7},{_tmp4C3,_tmp4C3,
_tmp4C3 + 7},{_tmp4C4,_tmp4C4,_tmp4C4 + 8},{_tmp4C5,_tmp4C5,_tmp4C5 + 5},{_tmp4C6,
_tmp4C6,_tmp4C6 + 5},{_tmp4C7,_tmp4C7,_tmp4C7 + 6},{_tmp4C8,_tmp4C8,_tmp4C8 + 4},{
_tmp4C9,_tmp4C9,_tmp4C9 + 5},{_tmp4CA,_tmp4CA,_tmp4CA + 6},{_tmp4CB,_tmp4CB,
_tmp4CB + 7},{_tmp4CC,_tmp4CC,_tmp4CC + 7},{_tmp4CD,_tmp4CD,_tmp4CD + 9},{_tmp4CE,
_tmp4CE,_tmp4CE + 6},{_tmp4CF,_tmp4CF,_tmp4CF + 9},{_tmp4D0,_tmp4D0,_tmp4D0 + 9},{
_tmp4D1,_tmp4D1,_tmp4D1 + 7},{_tmp4D2,_tmp4D2,_tmp4D2 + 6},{_tmp4D3,_tmp4D3,
_tmp4D3 + 5},{_tmp4D4,_tmp4D4,_tmp4D4 + 8},{_tmp4D5,_tmp4D5,_tmp4D5 + 7},{_tmp4D6,
_tmp4D6,_tmp4D6 + 7},{_tmp4D7,_tmp4D7,_tmp4D7 + 9},{_tmp4D8,_tmp4D8,_tmp4D8 + 3},{
_tmp4D9,_tmp4D9,_tmp4D9 + 5},{_tmp4DA,_tmp4DA,_tmp4DA + 7},{_tmp4DB,_tmp4DB,
_tmp4DB + 6},{_tmp4DC,_tmp4DC,_tmp4DC + 3},{_tmp4DD,_tmp4DD,_tmp4DD + 4},{_tmp4DE,
_tmp4DE,_tmp4DE + 5},{_tmp4DF,_tmp4DF,_tmp4DF + 9},{_tmp4E0,_tmp4E0,_tmp4E0 + 6},{
_tmp4E1,_tmp4E1,_tmp4E1 + 7},{_tmp4E2,_tmp4E2,_tmp4E2 + 5},{_tmp4E3,_tmp4E3,
_tmp4E3 + 8},{_tmp4E4,_tmp4E4,_tmp4E4 + 4},{_tmp4E5,_tmp4E5,_tmp4E5 + 6},{_tmp4E6,
_tmp4E6,_tmp4E6 + 4},{_tmp4E7,_tmp4E7,_tmp4E7 + 6},{_tmp4E8,_tmp4E8,_tmp4E8 + 4},{
_tmp4E9,_tmp4E9,_tmp4E9 + 9},{_tmp4EA,_tmp4EA,_tmp4EA + 9},{_tmp4EB,_tmp4EB,
_tmp4EB + 6},{_tmp4EC,_tmp4EC,_tmp4EC + 10},{_tmp4ED,_tmp4ED,_tmp4ED + 7},{_tmp4EE,
_tmp4EE,_tmp4EE + 8},{_tmp4EF,_tmp4EF,_tmp4EF + 9},{_tmp4F0,_tmp4F0,_tmp4F0 + 5},{
_tmp4F1,_tmp4F1,_tmp4F1 + 8},{_tmp4F2,_tmp4F2,_tmp4F2 + 4},{_tmp4F3,_tmp4F3,
_tmp4F3 + 7},{_tmp4F4,_tmp4F4,_tmp4F4 + 7},{_tmp4F5,_tmp4F5,_tmp4F5 + 8},{_tmp4F6,
_tmp4F6,_tmp4F6 + 7},{_tmp4F7,_tmp4F7,_tmp4F7 + 8},{_tmp4F8,_tmp4F8,_tmp4F8 + 9},{
_tmp4F9,_tmp4F9,_tmp4F9 + 9},{_tmp4FA,_tmp4FA,_tmp4FA + 7},{_tmp4FB,_tmp4FB,
_tmp4FB + 5},{_tmp4FC,_tmp4FC,_tmp4FC + 8},{_tmp4FD,_tmp4FD,_tmp4FD + 4},{_tmp4FE,
_tmp4FE,_tmp4FE + 7},{_tmp4FF,_tmp4FF,_tmp4FF + 7},{_tmp500,_tmp500,_tmp500 + 7},{
_tmp501,_tmp501,_tmp501 + 8},{_tmp502,_tmp502,_tmp502 + 9},{_tmp503,_tmp503,
_tmp503 + 6},{_tmp504,_tmp504,_tmp504 + 6},{_tmp505,_tmp505,_tmp505 + 6},{_tmp506,
_tmp506,_tmp506 + 6},{_tmp507,_tmp507,_tmp507 + 7},{_tmp508,_tmp508,_tmp508 + 6},{
_tmp509,_tmp509,_tmp509 + 11},{_tmp50A,_tmp50A,_tmp50A + 11},{_tmp50B,_tmp50B,
_tmp50B + 11},{_tmp50C,_tmp50C,_tmp50C + 11},{_tmp50D,_tmp50D,_tmp50D + 11},{
_tmp50E,_tmp50E,_tmp50E + 12},{_tmp50F,_tmp50F,_tmp50F + 13},{_tmp510,_tmp510,
_tmp510 + 11},{_tmp511,_tmp511,_tmp511 + 11},{_tmp512,_tmp512,_tmp512 + 10},{
_tmp513,_tmp513,_tmp513 + 9},{_tmp514,_tmp514,_tmp514 + 11},{_tmp515,_tmp515,
_tmp515 + 12},{_tmp516,_tmp516,_tmp516 + 11},{_tmp517,_tmp517,_tmp517 + 17},{
_tmp518,_tmp518,_tmp518 + 7},{_tmp519,_tmp519,_tmp519 + 19},{_tmp51A,_tmp51A,
_tmp51A + 18},{_tmp51B,_tmp51B,_tmp51B + 9},{_tmp51C,_tmp51C,_tmp51C + 16},{_tmp51D,
_tmp51D,_tmp51D + 18},{_tmp51E,_tmp51E,_tmp51E + 10},{_tmp51F,_tmp51F,_tmp51F + 4},{
_tmp520,_tmp520,_tmp520 + 4},{_tmp521,_tmp521,_tmp521 + 4},{_tmp522,_tmp522,
_tmp522 + 4},{_tmp523,_tmp523,_tmp523 + 4},{_tmp524,_tmp524,_tmp524 + 4},{_tmp525,
_tmp525,_tmp525 + 4},{_tmp526,_tmp526,_tmp526 + 4},{_tmp527,_tmp527,_tmp527 + 4},{
_tmp528,_tmp528,_tmp528 + 4},{_tmp529,_tmp529,_tmp529 + 4},{_tmp52A,_tmp52A,
_tmp52A + 4},{_tmp52B,_tmp52B,_tmp52B + 4},{_tmp52C,_tmp52C,_tmp52C + 4},{_tmp52D,
_tmp52D,_tmp52D + 4},{_tmp52E,_tmp52E,_tmp52E + 4},{_tmp52F,_tmp52F,_tmp52F + 4},{
_tmp530,_tmp530,_tmp530 + 4},{_tmp531,_tmp531,_tmp531 + 4},{_tmp532,_tmp532,
_tmp532 + 4},{_tmp533,_tmp533,_tmp533 + 4},{_tmp534,_tmp534,_tmp534 + 4},{_tmp535,
_tmp535,_tmp535 + 4},{_tmp536,_tmp536,_tmp536 + 4},{_tmp537,_tmp537,_tmp537 + 4},{
_tmp538,_tmp538,_tmp538 + 4},{_tmp539,_tmp539,_tmp539 + 4},{_tmp53A,_tmp53A,
_tmp53A + 5},{_tmp53B,_tmp53B,_tmp53B + 17},{_tmp53C,_tmp53C,_tmp53C + 21},{_tmp53D,
_tmp53D,_tmp53D + 20},{_tmp53E,_tmp53E,_tmp53E + 21},{_tmp53F,_tmp53F,_tmp53F + 13},{
_tmp540,_tmp540,_tmp540 + 15},{_tmp541,_tmp541,_tmp541 + 17},{_tmp542,_tmp542,
_tmp542 + 19},{_tmp543,_tmp543,_tmp543 + 12},{_tmp544,_tmp544,_tmp544 + 17},{
_tmp545,_tmp545,_tmp545 + 23},{_tmp546,_tmp546,_tmp546 + 24},{_tmp547,_tmp547,
_tmp547 + 15},{_tmp548,_tmp548,_tmp548 + 11},{_tmp549,_tmp549,_tmp549 + 15},{
_tmp54A,_tmp54A,_tmp54A + 10},{_tmp54B,_tmp54B,_tmp54B + 15},{_tmp54C,_tmp54C,
_tmp54C + 5},{_tmp54D,_tmp54D,_tmp54D + 15},{_tmp54E,_tmp54E,_tmp54E + 15},{_tmp54F,
_tmp54F,_tmp54F + 11},{_tmp550,_tmp550,_tmp550 + 22},{_tmp551,_tmp551,_tmp551 + 26},{
_tmp552,_tmp552,_tmp552 + 16},{_tmp553,_tmp553,_tmp553 + 16},{_tmp554,_tmp554,
_tmp554 + 24},{_tmp555,_tmp555,_tmp555 + 25},{_tmp556,_tmp556,_tmp556 + 21},{
_tmp557,_tmp557,_tmp557 + 22},{_tmp558,_tmp558,_tmp558 + 16},{_tmp559,_tmp559,
_tmp559 + 19},{_tmp55A,_tmp55A,_tmp55A + 25},{_tmp55B,_tmp55B,_tmp55B + 23},{
_tmp55C,_tmp55C,_tmp55C + 24},{_tmp55D,_tmp55D,_tmp55D + 18},{_tmp55E,_tmp55E,
_tmp55E + 17},{_tmp55F,_tmp55F,_tmp55F + 18},{_tmp560,_tmp560,_tmp560 + 17},{
_tmp561,_tmp561,_tmp561 + 18},{_tmp562,_tmp562,_tmp562 + 12},{_tmp563,_tmp563,
_tmp563 + 11},{_tmp564,_tmp564,_tmp564 + 18},{_tmp565,_tmp565,_tmp565 + 8},{_tmp566,
_tmp566,_tmp566 + 13},{_tmp567,_tmp567,_tmp567 + 8},{_tmp568,_tmp568,_tmp568 + 4},{
_tmp569,_tmp569,_tmp569 + 20},{_tmp56A,_tmp56A,_tmp56A + 20},{_tmp56B,_tmp56B,
_tmp56B + 9},{_tmp56C,_tmp56C,_tmp56C + 16},{_tmp56D,_tmp56D,_tmp56D + 19},{_tmp56E,
_tmp56E,_tmp56E + 10},{_tmp56F,_tmp56F,_tmp56F + 16},{_tmp570,_tmp570,_tmp570 + 11},{
_tmp571,_tmp571,_tmp571 + 14},{_tmp572,_tmp572,_tmp572 + 11},{_tmp573,_tmp573,
_tmp573 + 15},{_tmp574,_tmp574,_tmp574 + 22},{_tmp575,_tmp575,_tmp575 + 16},{
_tmp576,_tmp576,_tmp576 + 17},{_tmp577,_tmp577,_tmp577 + 12},{_tmp578,_tmp578,
_tmp578 + 18},{_tmp579,_tmp579,_tmp579 + 17},{_tmp57A,_tmp57A,_tmp57A + 12},{
_tmp57B,_tmp57B,_tmp57B + 16},{_tmp57C,_tmp57C,_tmp57C + 11},{_tmp57D,_tmp57D,
_tmp57D + 10},{_tmp57E,_tmp57E,_tmp57E + 14},{_tmp57F,_tmp57F,_tmp57F + 15},{
_tmp580,_tmp580,_tmp580 + 20},{_tmp581,_tmp581,_tmp581 + 27},{_tmp582,_tmp582,
_tmp582 + 10},{_tmp583,_tmp583,_tmp583 + 18},{_tmp584,_tmp584,_tmp584 + 21},{
_tmp585,_tmp585,_tmp585 + 19},{_tmp586,_tmp586,_tmp586 + 16},{_tmp587,_tmp587,
_tmp587 + 20},{_tmp588,_tmp588,_tmp588 + 15},{_tmp589,_tmp589,_tmp589 + 16},{
_tmp58A,_tmp58A,_tmp58A + 20},{_tmp58B,_tmp58B,_tmp58B + 15},{_tmp58C,_tmp58C,
_tmp58C + 8},{_tmp58D,_tmp58D,_tmp58D + 19},{_tmp58E,_tmp58E,_tmp58E + 20},{_tmp58F,
_tmp58F,_tmp58F + 14},{_tmp590,_tmp590,_tmp590 + 19},{_tmp591,_tmp591,_tmp591 + 20},{
_tmp592,_tmp592,_tmp592 + 11},{_tmp593,_tmp593,_tmp593 + 22},{_tmp594,_tmp594,
_tmp594 + 20},{_tmp595,_tmp595,_tmp595 + 23},{_tmp596,_tmp596,_tmp596 + 20},{
_tmp597,_tmp597,_tmp597 + 22},{_tmp598,_tmp598,_tmp598 + 23},{_tmp599,_tmp599,
_tmp599 + 24},{_tmp59A,_tmp59A,_tmp59A + 24},{_tmp59B,_tmp59B,_tmp59B + 15},{
_tmp59C,_tmp59C,_tmp59C + 20},{_tmp59D,_tmp59D,_tmp59D + 22},{_tmp59E,_tmp59E,
_tmp59E + 17},{_tmp59F,_tmp59F,_tmp59F + 20},{_tmp5A0,_tmp5A0,_tmp5A0 + 26},{
_tmp5A1,_tmp5A1,_tmp5A1 + 16},{_tmp5A2,_tmp5A2,_tmp5A2 + 17},{_tmp5A3,_tmp5A3,
_tmp5A3 + 15},{_tmp5A4,_tmp5A4,_tmp5A4 + 19},{_tmp5A5,_tmp5A5,_tmp5A5 + 19},{
_tmp5A6,_tmp5A6,_tmp5A6 + 25},{_tmp5A7,_tmp5A7,_tmp5A7 + 26},{_tmp5A8,_tmp5A8,
_tmp5A8 + 9},{_tmp5A9,_tmp5A9,_tmp5A9 + 20}};static short Cyc_yyr1[410]={0,125,126,
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
yyssp_offset;short*yyss=(short*)({unsigned int _tmpCF7=10000;short*_tmpCF8=(short*)
_cycalloc_atomic(_check_times(sizeof(short),_tmpCF7));{unsigned int _tmpCF9=
_tmpCF7;unsigned int i;for(i=0;i < _tmpCF9;i ++){_tmpCF8[i]=(short)0;}};_tmpCF8;});
int yyvsp_offset;void**yyvs=(void**)({unsigned int _tmpCF4=10000;void**_tmpCF5=(
void**)_cycalloc(_check_times(sizeof(void*),_tmpCF4));{unsigned int _tmpCF6=
_tmpCF4;unsigned int i;for(i=0;i < _tmpCF6;i ++){_tmpCF5[i]=Cyc_yylval;}};_tmpCF5;});
int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmpCF1=
10000;struct Cyc_Yyltype*_tmpCF2=(struct Cyc_Yyltype*)_cycalloc_atomic(
_check_times(sizeof(struct Cyc_Yyltype),_tmpCF1));{unsigned int _tmpCF3=_tmpCF1;
unsigned int i;for(i=0;i < _tmpCF3;i ++){_tmpCF2[i]=Cyc_yynewloc();}};_tmpCF2;});
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
switch(yyn){case 1: _LL1450: yyval=yyvs[yyvsp_offset];Cyc_Parse_parse_result=Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]);break;case 2: _LL1451: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp5AD=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));_tmp5AD[0]=({struct Cyc_DeclList_tok_struct
_tmp5AE;_tmp5AE.tag=Cyc_DeclList_tok;_tmp5AE.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]));_tmp5AE;});_tmp5AD;});break;case 3: _LL1452: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp5B0=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));
_tmp5B0[0]=({struct Cyc_DeclList_tok_struct _tmp5B1;_tmp5B1.tag=Cyc_DeclList_tok;
_tmp5B1.f1=({struct Cyc_List_List*_tmp5B2=_cycalloc(sizeof(struct Cyc_List_List));
_tmp5B2->hd=({struct Cyc_Absyn_Decl*_tmp5B3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));
_tmp5B3->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp5B4=_cycalloc(
sizeof(struct Cyc_Absyn_Using_d_struct));_tmp5B4[0]=({struct Cyc_Absyn_Using_d_struct
_tmp5B5;_tmp5B5.tag=Cyc_Absyn_Using_d;_tmp5B5.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5B5.f2=Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]);_tmp5B5;});_tmp5B4;}));_tmp5B3->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp5B3;});_tmp5B2->tl=0;_tmp5B2;});_tmp5B1;});_tmp5B0;});
Cyc_Lex_leave_using();break;case 4: _LL1455: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp5B7=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));_tmp5B7[0]=({struct Cyc_DeclList_tok_struct
_tmp5B8;_tmp5B8.tag=Cyc_DeclList_tok;_tmp5B8.f1=({struct Cyc_List_List*_tmp5B9=
_cycalloc(sizeof(struct Cyc_List_List));_tmp5B9->hd=({struct Cyc_Absyn_Decl*
_tmp5BA=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_tmp5BA->r=(void*)((void*)({
struct Cyc_Absyn_Using_d_struct*_tmp5BB=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_struct));
_tmp5BB[0]=({struct Cyc_Absyn_Using_d_struct _tmp5BC;_tmp5BC.tag=Cyc_Absyn_Using_d;
_tmp5BC.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp5BC.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5BC;});_tmp5BB;}));
_tmp5BA->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp5BA;});_tmp5B9->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp5B9;});_tmp5B8;});_tmp5B7;});break;case 5: _LL1462: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp5BE=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));
_tmp5BE[0]=({struct Cyc_DeclList_tok_struct _tmp5BF;_tmp5BF.tag=Cyc_DeclList_tok;
_tmp5BF.f1=({struct Cyc_List_List*_tmp5C0=_cycalloc(sizeof(struct Cyc_List_List));
_tmp5C0->hd=({struct Cyc_Absyn_Decl*_tmp5C1=_cycalloc(sizeof(struct Cyc_Absyn_Decl));
_tmp5C1->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp5C2=_cycalloc(
sizeof(struct Cyc_Absyn_Namespace_d_struct));_tmp5C2[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp5C3;_tmp5C3.tag=Cyc_Absyn_Namespace_d;_tmp5C3.f1=({struct _tagged_arr*_tmp5C4=
_cycalloc(sizeof(struct _tagged_arr));_tmp5C4[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5C4;});_tmp5C3.f2=
Cyc_yyget_DeclList_tok(yyvs[yyvsp_offset]);_tmp5C3;});_tmp5C2;}));_tmp5C1->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line);_tmp5C1;});_tmp5C0->tl=
0;_tmp5C0;});_tmp5BF;});_tmp5BE;});Cyc_Lex_leave_namespace();break;case 6: _LL1469:
yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp5C6=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));
_tmp5C6[0]=({struct Cyc_DeclList_tok_struct _tmp5C7;_tmp5C7.tag=Cyc_DeclList_tok;
_tmp5C7.f1=({struct Cyc_List_List*_tmp5C8=_cycalloc(sizeof(struct Cyc_List_List));
_tmp5C8->hd=({struct Cyc_Absyn_Decl*_tmp5C9=_cycalloc(sizeof(struct Cyc_Absyn_Decl));
_tmp5C9->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp5CA=_cycalloc(
sizeof(struct Cyc_Absyn_Namespace_d_struct));_tmp5CA[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp5CB;_tmp5CB.tag=Cyc_Absyn_Namespace_d;_tmp5CB.f1=({struct _tagged_arr*_tmp5CC=
_cycalloc(sizeof(struct _tagged_arr));_tmp5CC[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp5CC;});_tmp5CB.f2=
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5CB;});_tmp5CA;}));_tmp5C9->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp5C9;});
_tmp5C8->tl=Cyc_yyget_DeclList_tok(yyvs[yyvsp_offset]);_tmp5C8;});_tmp5C7;});
_tmp5C6;});break;case 7: _LL1477: if(Cyc_Std_strcmp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tag_arr("C",sizeof(
unsigned char),2))!= 0){Cyc_Parse_err(_tag_arr("only extern \"C\" { ... } is allowed",
sizeof(unsigned char),35),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));}yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp5CE=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));
_tmp5CE[0]=({struct Cyc_DeclList_tok_struct _tmp5CF;_tmp5CF.tag=Cyc_DeclList_tok;
_tmp5CF.f1=({struct Cyc_List_List*_tmp5D0=_cycalloc(sizeof(struct Cyc_List_List));
_tmp5D0->hd=({struct Cyc_Absyn_Decl*_tmp5D1=_cycalloc(sizeof(struct Cyc_Absyn_Decl));
_tmp5D1->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*_tmp5D2=_cycalloc(
sizeof(struct Cyc_Absyn_ExternC_d_struct));_tmp5D2[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp5D3;_tmp5D3.tag=Cyc_Absyn_ExternC_d;_tmp5D3.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D3;});_tmp5D2;}));
_tmp5D1->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp5D1;});_tmp5D0->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp5D0;});_tmp5CF;});_tmp5CE;});break;case 8: _LL1485: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp5D5=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));
_tmp5D5[0]=({struct Cyc_DeclList_tok_struct _tmp5D6;_tmp5D6.tag=Cyc_DeclList_tok;
_tmp5D6.f1=0;_tmp5D6;});_tmp5D5;});break;case 9: _LL1492: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp5D8=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));_tmp5D8[0]=({struct Cyc_DeclList_tok_struct
_tmp5D9;_tmp5D9.tag=Cyc_DeclList_tok;_tmp5D9.f1=({struct Cyc_List_List*_tmp5DA=
_cycalloc(sizeof(struct Cyc_List_List));_tmp5DA->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Fn_d_struct*_tmp5DB=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_struct));
_tmp5DB[0]=({struct Cyc_Absyn_Fn_d_struct _tmp5DC;_tmp5DC.tag=Cyc_Absyn_Fn_d;
_tmp5DC.f1=Cyc_yyget_FnDecl_tok(yyvs[yyvsp_offset]);_tmp5DC;});_tmp5DB;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp5DA->tl=0;
_tmp5DA;});_tmp5D9;});_tmp5D8;});break;case 10: _LL1495: yyval=yyvs[yyvsp_offset];
break;case 11: _LL1501: yyval=(void*)({struct Cyc_FnDecl_tok_struct*_tmp5DF=
_cycalloc(sizeof(struct Cyc_FnDecl_tok_struct));_tmp5DF[0]=({struct Cyc_FnDecl_tok_struct
_tmp5E0;_tmp5E0.tag=Cyc_FnDecl_tok;_tmp5E0.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5E0;});_tmp5DF;});break;case 12: _LL1502: yyval=(
void*)({struct Cyc_FnDecl_tok_struct*_tmp5E2=_cycalloc(sizeof(struct Cyc_FnDecl_tok_struct));
_tmp5E2[0]=({struct Cyc_FnDecl_tok_struct _tmp5E3;_tmp5E3.tag=Cyc_FnDecl_tok;
_tmp5E3.f1=Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp5E4=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp5E4->v=Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5E4;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5E3;});_tmp5E2;});break;case 13: _LL1505: yyval=(
void*)({struct Cyc_FnDecl_tok_struct*_tmp5E6=_cycalloc(sizeof(struct Cyc_FnDecl_tok_struct));
_tmp5E6[0]=({struct Cyc_FnDecl_tok_struct _tmp5E7;_tmp5E7.tag=Cyc_FnDecl_tok;
_tmp5E7.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5E7;});_tmp5E6;});break;case 14: _LL1509: yyval=(
void*)({struct Cyc_FnDecl_tok_struct*_tmp5E9=_cycalloc(sizeof(struct Cyc_FnDecl_tok_struct));
_tmp5E9[0]=({struct Cyc_FnDecl_tok_struct _tmp5EA;_tmp5EA.tag=Cyc_FnDecl_tok;
_tmp5EA.f1=Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp5EB=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp5EB->v=Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5EB;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5EA;});_tmp5E9;});break;case 15: _LL1512: yyval=(
void*)({struct Cyc_FnDecl_tok_struct*_tmp5ED=_cycalloc(sizeof(struct Cyc_FnDecl_tok_struct));
_tmp5ED[0]=({struct Cyc_FnDecl_tok_struct _tmp5EE;_tmp5EE.tag=Cyc_FnDecl_tok;
_tmp5EE.f1=Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp5EF=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp5EF->v=Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5EF;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5EE;});_tmp5ED;});break;case 16: _LL1516: yyval=(
void*)({struct Cyc_FnDecl_tok_struct*_tmp5F1=_cycalloc(sizeof(struct Cyc_FnDecl_tok_struct));
_tmp5F1[0]=({struct Cyc_FnDecl_tok_struct _tmp5F2;_tmp5F2.tag=Cyc_FnDecl_tok;
_tmp5F2.f1=Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp5F3=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp5F3->v=Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5F3;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5F2;});_tmp5F1;});break;case 17: _LL1520: Cyc_Lex_enter_using(
Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]));yyval=yyvs[yyvsp_offset];break;case 18:
_LL1524: Cyc_Lex_leave_using();break;case 19: _LL1525: Cyc_Lex_enter_namespace(({
struct _tagged_arr*_tmp5F7=_cycalloc(sizeof(struct _tagged_arr));_tmp5F7[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp5F7;}));yyval=yyvs[yyvsp_offset];break;case 20: _LL1526:
Cyc_Lex_leave_namespace();break;case 21: _LL1528: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp5FA=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));_tmp5FA[0]=({struct Cyc_DeclList_tok_struct
_tmp5FB;_tmp5FB.tag=Cyc_DeclList_tok;_tmp5FB.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp5FB;});
_tmp5FA;});break;case 22: _LL1529: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp5FD=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));_tmp5FD[0]=({struct Cyc_DeclList_tok_struct
_tmp5FE;_tmp5FE.tag=Cyc_DeclList_tok;_tmp5FE.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5FE;});_tmp5FD;});break;case 23: _LL1532: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp600=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));
_tmp600[0]=({struct Cyc_DeclList_tok_struct _tmp601;_tmp601.tag=Cyc_DeclList_tok;
_tmp601.f1=({struct Cyc_List_List*_tmp602=_cycalloc(sizeof(struct Cyc_List_List));
_tmp602->hd=Cyc_Absyn_let_decl(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp602->tl=0;_tmp602;});_tmp601;});_tmp600;});break;
case 24: _LL1535: {struct Cyc_List_List*_tmp604=0;{struct Cyc_List_List*_tmp605=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp605 != 0;
_tmp605=_tmp605->tl){struct _tagged_arr*_tmp606=(struct _tagged_arr*)_tmp605->hd;
struct _tuple1*_tmp607=({struct _tuple1*_tmp60A=_cycalloc(sizeof(struct _tuple1));
_tmp60A->f1=Cyc_Absyn_rel_ns_null;_tmp60A->f2=_tmp606;_tmp60A;});struct Cyc_Absyn_Vardecl*
_tmp608=Cyc_Absyn_new_vardecl(_tmp607,Cyc_Absyn_wildtyp(0),0);_tmp604=({struct
Cyc_List_List*_tmp609=_cycalloc(sizeof(struct Cyc_List_List));_tmp609->hd=_tmp608;
_tmp609->tl=_tmp604;_tmp609;});}}_tmp604=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp604);yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp60B=_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));_tmp60B[0]=({struct Cyc_DeclList_tok_struct
_tmp60C;_tmp60C.tag=Cyc_DeclList_tok;_tmp60C.f1=({struct Cyc_List_List*_tmp60D=
_cycalloc(sizeof(struct Cyc_List_List));_tmp60D->hd=Cyc_Absyn_letv_decl(_tmp604,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp60D->tl=0;
_tmp60D;});_tmp60C;});_tmp60B;});break;}case 25: _LL1539: yyval=yyvs[yyvsp_offset];
break;case 26: _LL1550: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp610=
_cycalloc(sizeof(struct Cyc_DeclList_tok_struct));_tmp610[0]=({struct Cyc_DeclList_tok_struct
_tmp611;_tmp611.tag=Cyc_DeclList_tok;_tmp611.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]));_tmp611;});_tmp610;});break;case 27: _LL1551: yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp613=_cycalloc(sizeof(struct Cyc_DeclSpec_tok_struct));
_tmp613[0]=({struct Cyc_DeclSpec_tok_struct _tmp614;_tmp614.tag=Cyc_DeclSpec_tok;
_tmp614.f1=({struct Cyc_Parse_Declaration_spec*_tmp615=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));
_tmp615->sc=({struct Cyc_Core_Opt*_tmp616=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp616->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp616;});_tmp615->tq=Cyc_Absyn_empty_tqual();_tmp615->type_specs=
0;_tmp615->is_inline=0;_tmp615->attributes=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp615;});_tmp614;});_tmp613;});break;case 28: _LL1554: if((Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->sc != 0){({void*_tmp618[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tag_arr("Only one storage class is allowed in a declaration",
sizeof(unsigned char),51),_tag_arr(_tmp618,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp619=_cycalloc(sizeof(struct Cyc_DeclSpec_tok_struct));
_tmp619[0]=({struct Cyc_DeclSpec_tok_struct _tmp61A;_tmp61A.tag=Cyc_DeclSpec_tok;
_tmp61A.f1=({struct Cyc_Parse_Declaration_spec*_tmp61B=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));
_tmp61B->sc=({struct Cyc_Core_Opt*_tmp61C=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp61C->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp61C;});_tmp61B->tq=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->tq;_tmp61B->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->type_specs;_tmp61B->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->is_inline;_tmp61B->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp61B;});_tmp61A;});_tmp619;});break;case 29:
_LL1559: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp61E=_cycalloc(sizeof(
struct Cyc_DeclSpec_tok_struct));_tmp61E[0]=({struct Cyc_DeclSpec_tok_struct
_tmp61F;_tmp61F.tag=Cyc_DeclSpec_tok;_tmp61F.f1=({struct Cyc_Parse_Declaration_spec*
_tmp620=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));_tmp620->sc=0;
_tmp620->tq=Cyc_Absyn_empty_tqual();_tmp620->type_specs=({struct Cyc_List_List*
_tmp621=_cycalloc(sizeof(struct Cyc_List_List));_tmp621->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp621->tl=0;
_tmp621;});_tmp620->is_inline=0;_tmp620->attributes=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp620;});_tmp61F;});_tmp61E;});break;case 30: _LL1565: yyval=(
void*)({struct Cyc_DeclSpec_tok_struct*_tmp623=_cycalloc(sizeof(struct Cyc_DeclSpec_tok_struct));
_tmp623[0]=({struct Cyc_DeclSpec_tok_struct _tmp624;_tmp624.tag=Cyc_DeclSpec_tok;
_tmp624.f1=({struct Cyc_Parse_Declaration_spec*_tmp625=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));
_tmp625->sc=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->sc;_tmp625->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->tq;_tmp625->type_specs=({struct Cyc_List_List*_tmp626=
_cycalloc(sizeof(struct Cyc_List_List));_tmp626->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp626->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp626;});_tmp625->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->is_inline;_tmp625->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp625;});_tmp624;});_tmp623;});break;case 31:
_LL1570: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp628=_cycalloc(sizeof(
struct Cyc_DeclSpec_tok_struct));_tmp628[0]=({struct Cyc_DeclSpec_tok_struct
_tmp629;_tmp629.tag=Cyc_DeclSpec_tok;_tmp629.f1=({struct Cyc_Parse_Declaration_spec*
_tmp62A=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));_tmp62A->sc=0;
_tmp62A->tq=Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp62A->type_specs=0;_tmp62A->is_inline=0;_tmp62A->attributes=
Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp62A;});_tmp629;});_tmp628;});
break;case 32: _LL1575: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp62C=
_cycalloc(sizeof(struct Cyc_DeclSpec_tok_struct));_tmp62C[0]=({struct Cyc_DeclSpec_tok_struct
_tmp62D;_tmp62D.tag=Cyc_DeclSpec_tok;_tmp62D.f1=({struct Cyc_Parse_Declaration_spec*
_tmp62E=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));_tmp62E->sc=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->sc;_tmp62E->tq=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->tq);_tmp62E->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->type_specs;_tmp62E->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->is_inline;_tmp62E->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp62E;});_tmp62D;});_tmp62C;});break;case 33:
_LL1579: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp630=_cycalloc(sizeof(
struct Cyc_DeclSpec_tok_struct));_tmp630[0]=({struct Cyc_DeclSpec_tok_struct
_tmp631;_tmp631.tag=Cyc_DeclSpec_tok;_tmp631.f1=({struct Cyc_Parse_Declaration_spec*
_tmp632=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));_tmp632->sc=0;
_tmp632->tq=Cyc_Absyn_empty_tqual();_tmp632->type_specs=0;_tmp632->is_inline=1;
_tmp632->attributes=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp632;});
_tmp631;});_tmp630;});break;case 34: _LL1583: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp634=_cycalloc(sizeof(struct Cyc_DeclSpec_tok_struct));_tmp634[0]=({struct Cyc_DeclSpec_tok_struct
_tmp635;_tmp635.tag=Cyc_DeclSpec_tok;_tmp635.f1=({struct Cyc_Parse_Declaration_spec*
_tmp636=_cycalloc(sizeof(struct Cyc_Parse_Declaration_spec));_tmp636->sc=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->sc;_tmp636->tq=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq;
_tmp636->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->type_specs;
_tmp636->is_inline=1;_tmp636->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp636;});_tmp635;});_tmp634;});break;case 35:
_LL1587: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp638=_cycalloc(
sizeof(struct Cyc_StorageClass_tok_struct));_tmp638[0]=({struct Cyc_StorageClass_tok_struct
_tmp639;_tmp639.tag=Cyc_StorageClass_tok;_tmp639.f1=(void*)((void*)Cyc_Parse_Auto_sc);
_tmp639;});_tmp638;});break;case 36: _LL1591: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp63B=_cycalloc(sizeof(struct Cyc_StorageClass_tok_struct));_tmp63B[0]=({struct
Cyc_StorageClass_tok_struct _tmp63C;_tmp63C.tag=Cyc_StorageClass_tok;_tmp63C.f1=(
void*)((void*)Cyc_Parse_Register_sc);_tmp63C;});_tmp63B;});break;case 37: _LL1594:
yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp63E=_cycalloc(sizeof(struct
Cyc_StorageClass_tok_struct));_tmp63E[0]=({struct Cyc_StorageClass_tok_struct
_tmp63F;_tmp63F.tag=Cyc_StorageClass_tok;_tmp63F.f1=(void*)((void*)Cyc_Parse_Static_sc);
_tmp63F;});_tmp63E;});break;case 38: _LL1597: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp641=_cycalloc(sizeof(struct Cyc_StorageClass_tok_struct));_tmp641[0]=({struct
Cyc_StorageClass_tok_struct _tmp642;_tmp642.tag=Cyc_StorageClass_tok;_tmp642.f1=(
void*)((void*)Cyc_Parse_Extern_sc);_tmp642;});_tmp641;});break;case 39: _LL1600:
if(Cyc_Std_strcmp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),_tag_arr("C",sizeof(
unsigned char),2))!= 0){Cyc_Parse_err(_tag_arr("only extern or extern \"C\" is allowed",
sizeof(unsigned char),37),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp644=_cycalloc(sizeof(struct Cyc_StorageClass_tok_struct));_tmp644[0]=({struct
Cyc_StorageClass_tok_struct _tmp645;_tmp645.tag=Cyc_StorageClass_tok;_tmp645.f1=(
void*)((void*)Cyc_Parse_ExternC_sc);_tmp645;});_tmp644;});break;case 40: _LL1603:
yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp647=_cycalloc(sizeof(struct
Cyc_StorageClass_tok_struct));_tmp647[0]=({struct Cyc_StorageClass_tok_struct
_tmp648;_tmp648.tag=Cyc_StorageClass_tok;_tmp648.f1=(void*)((void*)Cyc_Parse_Typedef_sc);
_tmp648;});_tmp647;});break;case 41: _LL1606: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp64A=_cycalloc(sizeof(struct Cyc_StorageClass_tok_struct));_tmp64A[0]=({struct
Cyc_StorageClass_tok_struct _tmp64B;_tmp64B.tag=Cyc_StorageClass_tok;_tmp64B.f1=(
void*)((void*)Cyc_Parse_Abstract_sc);_tmp64B;});_tmp64A;});break;case 42: _LL1609:
yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp64D=_cycalloc(sizeof(
struct Cyc_AttributeList_tok_struct));_tmp64D[0]=({struct Cyc_AttributeList_tok_struct
_tmp64E;_tmp64E.tag=Cyc_AttributeList_tok;_tmp64E.f1=0;_tmp64E;});_tmp64D;});
break;case 43: _LL1612: yyval=yyvs[yyvsp_offset];break;case 44: _LL1615: yyval=(void*)({
struct Cyc_AttributeList_tok_struct*_tmp651=_cycalloc(sizeof(struct Cyc_AttributeList_tok_struct));
_tmp651[0]=({struct Cyc_AttributeList_tok_struct _tmp652;_tmp652.tag=Cyc_AttributeList_tok;
_tmp652.f1=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp652;});_tmp651;});break;case 45: _LL1616: yyval=(void*)({
struct Cyc_AttributeList_tok_struct*_tmp654=_cycalloc(sizeof(struct Cyc_AttributeList_tok_struct));
_tmp654[0]=({struct Cyc_AttributeList_tok_struct _tmp655;_tmp655.tag=Cyc_AttributeList_tok;
_tmp655.f1=({struct Cyc_List_List*_tmp656=_cycalloc(sizeof(struct Cyc_List_List));
_tmp656->hd=(void*)Cyc_yyget_Attribute_tok(yyvs[yyvsp_offset]);_tmp656->tl=0;
_tmp656;});_tmp655;});_tmp654;});break;case 46: _LL1619: yyval=(void*)({struct Cyc_AttributeList_tok_struct*
_tmp658=_cycalloc(sizeof(struct Cyc_AttributeList_tok_struct));_tmp658[0]=({
struct Cyc_AttributeList_tok_struct _tmp659;_tmp659.tag=Cyc_AttributeList_tok;
_tmp659.f1=({struct Cyc_List_List*_tmp65A=_cycalloc(sizeof(struct Cyc_List_List));
_tmp65A->hd=(void*)Cyc_yyget_Attribute_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp65A->tl=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp65A;});_tmp659;});_tmp658;});break;case 47: _LL1623: {struct
_tagged_arr _tmp65C=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);if((((_get_arr_size(
_tmp65C,sizeof(unsigned char))> 4?*((const unsigned char*)
_check_unknown_subscript(_tmp65C,sizeof(unsigned char),0))== '_': 0)?*((const
unsigned char*)_check_unknown_subscript(_tmp65C,sizeof(unsigned char),1))== '_':
0)?*((const unsigned char*)_check_unknown_subscript(_tmp65C,sizeof(unsigned char),(
int)(_get_arr_size(_tmp65C,sizeof(unsigned char))- 2)))== '_': 0)?*((const
unsigned char*)_check_unknown_subscript(_tmp65C,sizeof(unsigned char),(int)(
_get_arr_size(_tmp65C,sizeof(unsigned char))- 3)))== '_': 0){_tmp65C=(struct
_tagged_arr)Cyc_Std_substring(_tmp65C,2,_get_arr_size(_tmp65C,sizeof(
unsigned char))- 5);}{int i=0;for(0;i < 16;++ i){if(Cyc_Std_strcmp(_tmp65C,(Cyc_Parse_att_map[
i]).f1)== 0){yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp65D=_cycalloc(
sizeof(struct Cyc_Attribute_tok_struct));_tmp65D[0]=({struct Cyc_Attribute_tok_struct
_tmp65E;_tmp65E.tag=Cyc_Attribute_tok;_tmp65E.f1=(void*)(Cyc_Parse_att_map[i]).f2;
_tmp65E;});_tmp65D;});break;}}if(i == 16){Cyc_Parse_err(_tag_arr("unrecognized attribute",
sizeof(unsigned char),23),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Attribute_tok_struct*
_tmp65F=_cycalloc(sizeof(struct Cyc_Attribute_tok_struct));_tmp65F[0]=({struct Cyc_Attribute_tok_struct
_tmp660;_tmp660.tag=Cyc_Attribute_tok;_tmp660.f1=(void*)((void*)Cyc_Absyn_Cdecl_att);
_tmp660;});_tmp65F;});}break;}}case 48: _LL1627: yyval=(void*)({struct Cyc_Attribute_tok_struct*
_tmp662=_cycalloc(sizeof(struct Cyc_Attribute_tok_struct));_tmp662[0]=({struct Cyc_Attribute_tok_struct
_tmp663;_tmp663.tag=Cyc_Attribute_tok;_tmp663.f1=(void*)((void*)Cyc_Absyn_Const_att);
_tmp663;});_tmp662;});break;case 49: _LL1633: {struct _tagged_arr _tmp665=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct _tuple15
_tmp668;int _tmp669;struct _tuple15*_tmp666=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp668=*_tmp666;_LL1642:
_tmp669=_tmp668.f2;goto _LL1639;_LL1639: {void*a;if(Cyc_Std_zstrcmp(_tmp665,
_tag_arr("regparm",sizeof(unsigned char),8))== 0? 1: Cyc_Std_zstrcmp(_tmp665,
_tag_arr("__regparm__",sizeof(unsigned char),12))== 0){if(_tmp669 < 0? 1: _tmp669 > 
3){Cyc_Parse_err(_tag_arr("regparm requires value between 0 and 3",sizeof(
unsigned char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp66B=_cycalloc_atomic(sizeof(struct Cyc_Absyn_Regparm_att_struct));
_tmp66B[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp66C;_tmp66C.tag=Cyc_Absyn_Regparm_att;
_tmp66C.f1=_tmp669;_tmp66C;});_tmp66B;});}else{if(Cyc_Std_zstrcmp(_tmp665,
_tag_arr("aligned",sizeof(unsigned char),8))== 0? 1: Cyc_Std_zstrcmp(_tmp665,
_tag_arr("__aligned__",sizeof(unsigned char),12))== 0){if(_tmp669 < 0){Cyc_Parse_err(
_tag_arr("aligned requires positive power of two",sizeof(unsigned char),39),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp669;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
Cyc_Parse_err(_tag_arr("aligned requires positive power of two",sizeof(
unsigned char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp66D=_cycalloc_atomic(sizeof(struct Cyc_Absyn_Aligned_att_struct));
_tmp66D[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp66E;_tmp66E.tag=Cyc_Absyn_Aligned_att;
_tmp66E.f1=_tmp669;_tmp66E;});_tmp66D;});}}else{Cyc_Parse_err(_tag_arr("unrecognized attribute",
sizeof(unsigned char),23),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)Cyc_Absyn_Cdecl_att;}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp66F=_cycalloc(sizeof(struct Cyc_Attribute_tok_struct));
_tmp66F[0]=({struct Cyc_Attribute_tok_struct _tmp670;_tmp670.tag=Cyc_Attribute_tok;
_tmp670.f1=(void*)a;_tmp670;});_tmp66F;});break;}}case 50: _LL1636: {struct
_tagged_arr _tmp672=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp673=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_Std_zstrcmp(
_tmp672,_tag_arr("section",sizeof(unsigned char),8))== 0? 1: Cyc_Std_zstrcmp(
_tmp672,_tag_arr("__section__",sizeof(unsigned char),12))== 0){a=(void*)({struct
Cyc_Absyn_Section_att_struct*_tmp674=_cycalloc(sizeof(struct Cyc_Absyn_Section_att_struct));
_tmp674[0]=({struct Cyc_Absyn_Section_att_struct _tmp675;_tmp675.tag=Cyc_Absyn_Section_att;
_tmp675.f1=_tmp673;_tmp675;});_tmp674;});}else{Cyc_Parse_err(_tag_arr("unrecognized attribute",
sizeof(unsigned char),23),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)Cyc_Absyn_Cdecl_att;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp676=_cycalloc(sizeof(struct Cyc_Attribute_tok_struct));
_tmp676[0]=({struct Cyc_Attribute_tok_struct _tmp677;_tmp677.tag=Cyc_Attribute_tok;
_tmp677.f1=(void*)a;_tmp677;});_tmp676;});break;}case 51: _LL1649: {struct
_tagged_arr _tmp679=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp67A=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp67D;struct
_tuple15 _tmp67B=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_LL1662: _tmp67D=_tmp67B.f2;goto _LL1660;_LL1660: {int _tmp681;
struct _tuple15 _tmp67F=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_LL1666: _tmp681=_tmp67F.f2;goto _LL1664;_LL1664: {void*a=(
void*)Cyc_Absyn_Cdecl_att;if(Cyc_Std_zstrcmp(_tmp679,_tag_arr("format",sizeof(
unsigned char),7))== 0? 1: Cyc_Std_zstrcmp(_tmp679,_tag_arr("__format__",sizeof(
unsigned char),11))== 0){if(Cyc_Std_zstrcmp(_tmp67A,_tag_arr("printf",sizeof(
unsigned char),7))== 0){a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp683=
_cycalloc(sizeof(struct Cyc_Absyn_Format_att_struct));_tmp683[0]=({struct Cyc_Absyn_Format_att_struct
_tmp684;_tmp684.tag=Cyc_Absyn_Format_att;_tmp684.f1=(void*)((void*)Cyc_Absyn_Printf_ft);
_tmp684.f2=_tmp67D;_tmp684.f3=_tmp681;_tmp684;});_tmp683;});}else{if(Cyc_Std_zstrcmp(
_tmp67A,_tag_arr("scanf",sizeof(unsigned char),6))== 0){a=(void*)({struct Cyc_Absyn_Format_att_struct*
_tmp685=_cycalloc(sizeof(struct Cyc_Absyn_Format_att_struct));_tmp685[0]=({struct
Cyc_Absyn_Format_att_struct _tmp686;_tmp686.tag=Cyc_Absyn_Format_att;_tmp686.f1=(
void*)((void*)Cyc_Absyn_Scanf_ft);_tmp686.f2=_tmp67D;_tmp686.f3=_tmp681;_tmp686;});
_tmp685;});}else{Cyc_Parse_err(_tag_arr("unrecognized format type",sizeof(
unsigned char),25),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(
_tag_arr("unrecognized attribute",sizeof(unsigned char),23),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Attribute_tok_struct*
_tmp687=_cycalloc(sizeof(struct Cyc_Attribute_tok_struct));_tmp687[0]=({struct Cyc_Attribute_tok_struct
_tmp688;_tmp688.tag=Cyc_Attribute_tok;_tmp688.f1=(void*)a;_tmp688;});_tmp687;});
break;}}}case 52: _LL1656: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp68A=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp68A[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp68B;_tmp68B.tag=Cyc_TypeSpecifier_tok;
_tmp68B.f1=(void*)Cyc_Parse_type_spec((void*)Cyc_Absyn_VoidType,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp68B;});
_tmp68A;});break;case 53: _LL1673: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp68D=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp68D[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp68E;_tmp68E.tag=Cyc_TypeSpecifier_tok;
_tmp68E.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp68E;});
_tmp68D;});break;case 54: _LL1676: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp690=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp690[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp691;_tmp691.tag=Cyc_TypeSpecifier_tok;
_tmp691.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_uchar_t,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp691;});
_tmp690;});break;case 55: _LL1679: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp693=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp693[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp694;_tmp694.tag=Cyc_TypeSpecifier_tok;
_tmp694.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*_tmp695=_cycalloc(
sizeof(struct Cyc_Parse_Short_spec_struct));_tmp695[0]=({struct Cyc_Parse_Short_spec_struct
_tmp696;_tmp696.tag=Cyc_Parse_Short_spec;_tmp696.f1=Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);_tmp696;});
_tmp695;}));_tmp694;});_tmp693;});break;case 56: _LL1682: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp698=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp698[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp699;_tmp699.tag=Cyc_TypeSpecifier_tok;
_tmp699.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_sint_t,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp699;});
_tmp698;});break;case 57: _LL1687: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp69B=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp69B[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp69C;_tmp69C.tag=Cyc_TypeSpecifier_tok;
_tmp69C.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*_tmp69D=_cycalloc(
sizeof(struct Cyc_Parse_Long_spec_struct));_tmp69D[0]=({struct Cyc_Parse_Long_spec_struct
_tmp69E;_tmp69E.tag=Cyc_Parse_Long_spec;_tmp69E.f1=Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);_tmp69E;});
_tmp69D;}));_tmp69C;});_tmp69B;});break;case 58: _LL1690: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp6A0=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp6A0[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp6A1;_tmp6A1.tag=Cyc_TypeSpecifier_tok;
_tmp6A1.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_float_t,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp6A1;});
_tmp6A0;});break;case 59: _LL1695: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp6A3=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp6A3[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp6A4;_tmp6A4.tag=Cyc_TypeSpecifier_tok;
_tmp6A4.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_t(0),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp6A4;});
_tmp6A3;});break;case 60: _LL1698: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp6A6=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp6A6[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp6A7;_tmp6A7.tag=Cyc_TypeSpecifier_tok;
_tmp6A7.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*_tmp6A8=_cycalloc(
sizeof(struct Cyc_Parse_Signed_spec_struct));_tmp6A8[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp6A9;_tmp6A9.tag=Cyc_Parse_Signed_spec;_tmp6A9.f1=Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);_tmp6A9;});
_tmp6A8;}));_tmp6A7;});_tmp6A6;});break;case 61: _LL1701: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp6AB=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp6AB[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp6AC;_tmp6AC.tag=Cyc_TypeSpecifier_tok;
_tmp6AC.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*_tmp6AD=
_cycalloc(sizeof(struct Cyc_Parse_Unsigned_spec_struct));_tmp6AD[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp6AE;_tmp6AE.tag=Cyc_Parse_Unsigned_spec;_tmp6AE.f1=Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);_tmp6AE;});
_tmp6AD;}));_tmp6AC;});_tmp6AB;});break;case 62: _LL1706: yyval=yyvs[yyvsp_offset];
break;case 63: _LL1711: yyval=yyvs[yyvsp_offset];break;case 64: _LL1712: yyval=yyvs[
yyvsp_offset];break;case 65: _LL1713: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp6B3=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp6B3[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp6B4;_tmp6B4.tag=Cyc_TypeSpecifier_tok;
_tmp6B4.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp6B4;});
_tmp6B3;});break;case 66: _LL1714: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp6B6=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp6B6[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp6B7;_tmp6B7.tag=Cyc_TypeSpecifier_tok;
_tmp6B7.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp6B8=_cycalloc(sizeof(struct Cyc_Absyn_TypedefType_struct));_tmp6B8[0]=({
struct Cyc_Absyn_TypedefType_struct _tmp6B9;_tmp6B9.tag=Cyc_Absyn_TypedefType;
_tmp6B9.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp6B9.f2=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp6B9.f3=
0;_tmp6B9;});_tmp6B8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6B7;});_tmp6B6;});break;case 67: _LL1717: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp6BB=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));
_tmp6BB[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp6BC;_tmp6BC.tag=Cyc_TypeSpecifier_tok;
_tmp6BC.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TupleType_struct*
_tmp6BD=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_struct));_tmp6BD[0]=({struct
Cyc_Absyn_TupleType_struct _tmp6BE;_tmp6BE.tag=Cyc_Absyn_TupleType;_tmp6BE.f1=((
struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp6BE;});_tmp6BD;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6BC;});
_tmp6BB;});break;case 68: _LL1722: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp6C0=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp6C0[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp6C1;_tmp6C1.tag=Cyc_TypeSpecifier_tok;
_tmp6C1.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp6C2=_cycalloc(sizeof(struct Cyc_Absyn_RgnHandleType_struct));_tmp6C2[0]=({
struct Cyc_Absyn_RgnHandleType_struct _tmp6C3;_tmp6C3.tag=Cyc_Absyn_RgnHandleType;
_tmp6C3.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp6C3;});_tmp6C2;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6C1;});_tmp6C0;});break;case 69: _LL1727: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp6C5=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));
_tmp6C5[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp6C6;_tmp6C6.tag=Cyc_TypeSpecifier_tok;
_tmp6C6.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp6C7=_cycalloc(sizeof(struct Cyc_Absyn_SizeofType_struct));_tmp6C7[0]=({struct
Cyc_Absyn_SizeofType_struct _tmp6C8;_tmp6C8.tag=Cyc_Absyn_SizeofType;_tmp6C8.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp6C8;});_tmp6C7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6C6;});_tmp6C5;});break;case 70: _LL1732: yyval=(
void*)({struct Cyc_Kind_tok_struct*_tmp6CA=_cycalloc(sizeof(struct Cyc_Kind_tok_struct));
_tmp6CA[0]=({struct Cyc_Kind_tok_struct _tmp6CB;_tmp6CB.tag=Cyc_Kind_tok;_tmp6CB.f1=(
void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp6CB;});
_tmp6CA;});break;case 71: _LL1737: {struct _tagged_arr*_tmp6CF;void*_tmp6D1;struct
_tuple1 _tmp6CD=*Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);_LL1746: _tmp6D1=_tmp6CD.f1;
goto _LL1744;_LL1744: _tmp6CF=_tmp6CD.f2;goto _LL1742;_LL1742: if(_tmp6D1 != (void*)
Cyc_Absyn_Loc_n){Cyc_Parse_err(_tag_arr("bad kind in type specifier",sizeof(
unsigned char),27),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Kind_tok_struct*
_tmp6D3=_cycalloc(sizeof(struct Cyc_Kind_tok_struct));_tmp6D3[0]=({struct Cyc_Kind_tok_struct
_tmp6D4;_tmp6D4.tag=Cyc_Kind_tok;_tmp6D4.f1=(void*)Cyc_Parse_id_to_kind(*_tmp6CF,
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmp6D4;});_tmp6D3;});break;}case 72: _LL1740: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp6D6=_cycalloc_atomic(sizeof(struct Cyc_TypeQual_tok_struct));_tmp6D6[0]=({
struct Cyc_TypeQual_tok_struct _tmp6D7;_tmp6D7.tag=Cyc_TypeQual_tok;_tmp6D7.f1=({
struct Cyc_Absyn_Tqual _tmp6D8;_tmp6D8.q_const=1;_tmp6D8.q_volatile=0;_tmp6D8.q_restrict=
0;_tmp6D8;});_tmp6D7;});_tmp6D6;});break;case 73: _LL1749: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp6DA=_cycalloc_atomic(sizeof(struct Cyc_TypeQual_tok_struct));_tmp6DA[0]=({
struct Cyc_TypeQual_tok_struct _tmp6DB;_tmp6DB.tag=Cyc_TypeQual_tok;_tmp6DB.f1=({
struct Cyc_Absyn_Tqual _tmp6DC;_tmp6DC.q_const=0;_tmp6DC.q_volatile=1;_tmp6DC.q_restrict=
0;_tmp6DC;});_tmp6DB;});_tmp6DA;});break;case 74: _LL1753: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp6DE=_cycalloc_atomic(sizeof(struct Cyc_TypeQual_tok_struct));_tmp6DE[0]=({
struct Cyc_TypeQual_tok_struct _tmp6DF;_tmp6DF.tag=Cyc_TypeQual_tok;_tmp6DF.f1=({
struct Cyc_Absyn_Tqual _tmp6E0;_tmp6E0.q_const=0;_tmp6E0.q_volatile=0;_tmp6E0.q_restrict=
1;_tmp6E0;});_tmp6DF;});_tmp6DE;});break;case 75: _LL1757: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp6E2=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp6E2[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp6E3;_tmp6E3.tag=Cyc_TypeSpecifier_tok;
_tmp6E3.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp6E4=_cycalloc(
sizeof(struct Cyc_Parse_Decl_spec_struct));_tmp6E4[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp6E5;_tmp6E5.tag=Cyc_Parse_Decl_spec;_tmp6E5.f1=({struct Cyc_Absyn_Decl*
_tmp6E6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_tmp6E6->r=(void*)((void*)({
struct Cyc_Absyn_Enum_d_struct*_tmp6E7=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_struct));
_tmp6E7[0]=({struct Cyc_Absyn_Enum_d_struct _tmp6E8;_tmp6E8.tag=Cyc_Absyn_Enum_d;
_tmp6E8.f1=({struct Cyc_Absyn_Enumdecl*_tmp6E9=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));
_tmp6E9->sc=(void*)((void*)Cyc_Absyn_Public);_tmp6E9->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6E9->fields=({
struct Cyc_Core_Opt*_tmp6EA=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp6EA->v=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6EA;});_tmp6E9;});
_tmp6E8;});_tmp6E7;}));_tmp6E6->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6E6;});_tmp6E5;});_tmp6E4;}));_tmp6E3;});_tmp6E2;});
break;case 76: _LL1761: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp6EC=
_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp6EC[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp6ED;_tmp6ED.tag=Cyc_TypeSpecifier_tok;_tmp6ED.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp6EE=_cycalloc(sizeof(struct Cyc_Absyn_EnumType_struct));
_tmp6EE[0]=({struct Cyc_Absyn_EnumType_struct _tmp6EF;_tmp6EF.tag=Cyc_Absyn_EnumType;
_tmp6EF.f1=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);_tmp6EF.f2=0;_tmp6EF;});
_tmp6EE;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6ED;});
_tmp6EC;});break;case 77: _LL1771: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp6F1=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp6F1[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp6F2;_tmp6F2.tag=Cyc_TypeSpecifier_tok;
_tmp6F2.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp6F3=_cycalloc(
sizeof(struct Cyc_Parse_Type_spec_struct));_tmp6F3[0]=({struct Cyc_Parse_Type_spec_struct
_tmp6F4;_tmp6F4.tag=Cyc_Parse_Type_spec;_tmp6F4.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmp6F5=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnumType_struct));_tmp6F5[0]=({
struct Cyc_Absyn_AnonEnumType_struct _tmp6F6;_tmp6F6.tag=Cyc_Absyn_AnonEnumType;
_tmp6F6.f1=Cyc_yyget_EnumfieldList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp6F6;});_tmp6F5;}));_tmp6F4.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6F4;});_tmp6F3;}));_tmp6F2;});_tmp6F1;});break;
case 78: _LL1776: yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp6F8=_cycalloc(
sizeof(struct Cyc_Enumfield_tok_struct));_tmp6F8[0]=({struct Cyc_Enumfield_tok_struct
_tmp6F9;_tmp6F9.tag=Cyc_Enumfield_tok;_tmp6F9.f1=({struct Cyc_Absyn_Enumfield*
_tmp6FA=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));_tmp6FA->name=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp6FA->tag=0;_tmp6FA->loc=Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);_tmp6FA;});
_tmp6F9;});_tmp6F8;});break;case 79: _LL1783: yyval=(void*)({struct Cyc_Enumfield_tok_struct*
_tmp6FC=_cycalloc(sizeof(struct Cyc_Enumfield_tok_struct));_tmp6FC[0]=({struct Cyc_Enumfield_tok_struct
_tmp6FD;_tmp6FD.tag=Cyc_Enumfield_tok;_tmp6FD.f1=({struct Cyc_Absyn_Enumfield*
_tmp6FE=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));_tmp6FE->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6FE->tag=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp6FE->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6FE;});_tmp6FD;});_tmp6FC;});break;case 80: _LL1787:
yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*_tmp700=_cycalloc(sizeof(
struct Cyc_EnumfieldList_tok_struct));_tmp700[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp701;_tmp701.tag=Cyc_EnumfieldList_tok;_tmp701.f1=({struct Cyc_List_List*
_tmp702=_cycalloc(sizeof(struct Cyc_List_List));_tmp702->hd=Cyc_yyget_Enumfield_tok(
yyvs[yyvsp_offset]);_tmp702->tl=0;_tmp702;});_tmp701;});_tmp700;});break;case 81:
_LL1791: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*_tmp704=_cycalloc(
sizeof(struct Cyc_EnumfieldList_tok_struct));_tmp704[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp705;_tmp705.tag=Cyc_EnumfieldList_tok;_tmp705.f1=({struct Cyc_List_List*
_tmp706=_cycalloc(sizeof(struct Cyc_List_List));_tmp706->hd=Cyc_yyget_Enumfield_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp706->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[yyvsp_offset]);_tmp706;});_tmp705;});_tmp704;});break;case 82: _LL1795: {void*
t;switch(Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)])){case Cyc_Parse_Struct_su: _LL1800: t=(void*)({struct Cyc_Absyn_AnonStructType_struct*
_tmp70A=_cycalloc(sizeof(struct Cyc_Absyn_AnonStructType_struct));_tmp70A[0]=({
struct Cyc_Absyn_AnonStructType_struct _tmp70B;_tmp70B.tag=Cyc_Absyn_AnonStructType;
_tmp70B.f1=Cyc_yyget_StructFieldDeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp70B;});_tmp70A;});break;case Cyc_Parse_Union_su:
_LL1801: t=(void*)({struct Cyc_Absyn_AnonUnionType_struct*_tmp70D=_cycalloc(
sizeof(struct Cyc_Absyn_AnonUnionType_struct));_tmp70D[0]=({struct Cyc_Absyn_AnonUnionType_struct
_tmp70E;_tmp70E.tag=Cyc_Absyn_AnonUnionType;_tmp70E.f1=Cyc_yyget_StructFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp70E;});_tmp70D;});
break;}yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp70F=_cycalloc(
sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp70F[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp710;_tmp710.tag=Cyc_TypeSpecifier_tok;_tmp710.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*
_tmp711=_cycalloc(sizeof(struct Cyc_Parse_Type_spec_struct));_tmp711[0]=({struct
Cyc_Parse_Type_spec_struct _tmp712;_tmp712.tag=Cyc_Parse_Type_spec;_tmp712.f1=(
void*)t;_tmp712.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp712;});_tmp711;}));_tmp710;});_tmp70F;});break;}
case 83: _LL1799: {struct Cyc_List_List*_tmp714=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));struct Cyc_Absyn_Decl*
d;switch(Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)])){case Cyc_Parse_Struct_su: _LL1813: d=Cyc_Absyn_struct_decl((
void*)Cyc_Absyn_Public,({struct Cyc_Core_Opt*_tmp717=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp717->v=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp717;}),_tmp714,({struct Cyc_Core_Opt*_tmp718=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp718->v=Cyc_yyget_StructFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp718;}),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));break;case Cyc_Parse_Union_su: _LL1814: d=Cyc_Absyn_union_decl((
void*)Cyc_Absyn_Public,({struct Cyc_Core_Opt*_tmp71A=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp71A->v=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp71A;}),_tmp714,({struct Cyc_Core_Opt*_tmp71B=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp71B->v=Cyc_yyget_StructFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp71B;}),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));break;}yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp71C=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp71C[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp71D;_tmp71D.tag=Cyc_TypeSpecifier_tok;
_tmp71D.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp71E=_cycalloc(
sizeof(struct Cyc_Parse_Decl_spec_struct));_tmp71E[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp71F;_tmp71F.tag=Cyc_Parse_Decl_spec;_tmp71F.f1=d;_tmp71F;});_tmp71E;}));
_tmp71D;});_tmp71C;});break;}case 84: _LL1811: {struct Cyc_List_List*_tmp721=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));struct Cyc_Absyn_Decl*d;
switch(Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)])){case Cyc_Parse_Struct_su: _LL1826: d=Cyc_Absyn_struct_decl((
void*)Cyc_Absyn_Public,({struct Cyc_Core_Opt*_tmp724=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp724->v=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp724;}),_tmp721,({struct Cyc_Core_Opt*_tmp725=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp725->v=Cyc_yyget_StructFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp725;}),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));break;case Cyc_Parse_Union_su: _LL1827: d=Cyc_Absyn_union_decl((
void*)Cyc_Absyn_Public,({struct Cyc_Core_Opt*_tmp727=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp727->v=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp727;}),_tmp721,({struct Cyc_Core_Opt*_tmp728=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp728->v=Cyc_yyget_StructFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp728;}),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));break;}yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp729=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp729[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp72A;_tmp72A.tag=Cyc_TypeSpecifier_tok;
_tmp72A.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp72B=_cycalloc(
sizeof(struct Cyc_Parse_Decl_spec_struct));_tmp72B[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp72C;_tmp72C.tag=Cyc_Parse_Decl_spec;_tmp72C.f1=d;_tmp72C;});_tmp72B;}));
_tmp72A;});_tmp729;});break;}case 85: _LL1824: switch(Cyc_yyget_StructOrUnion_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])){case Cyc_Parse_Struct_su:
_LL1838: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp730=_cycalloc(
sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp730[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp731;_tmp731.tag=Cyc_TypeSpecifier_tok;_tmp731.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_StructType_struct*_tmp732=_cycalloc(sizeof(struct Cyc_Absyn_StructType_struct));
_tmp732[0]=({struct Cyc_Absyn_StructType_struct _tmp733;_tmp733.tag=Cyc_Absyn_StructType;
_tmp733.f1=(struct _tuple1*)Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp733.f2=Cyc_yyget_TypeList_tok(
yyvs[yyvsp_offset]);_tmp733.f3=0;_tmp733;});_tmp732;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp731;});_tmp730;});break;case Cyc_Parse_Union_su:
_LL1839: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp735=_cycalloc(
sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp735[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp736;_tmp736.tag=Cyc_TypeSpecifier_tok;_tmp736.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_UnionType_struct*_tmp737=_cycalloc(sizeof(struct Cyc_Absyn_UnionType_struct));
_tmp737[0]=({struct Cyc_Absyn_UnionType_struct _tmp738;_tmp738.tag=Cyc_Absyn_UnionType;
_tmp738.f1=(struct _tuple1*)Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp738.f2=Cyc_yyget_TypeList_tok(
yyvs[yyvsp_offset]);_tmp738.f3=0;_tmp738;});_tmp737;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp736;});_tmp735;});break;}break;case 86: _LL1837:
switch(Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])){case Cyc_Parse_Struct_su: _LL1850: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp73C=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp73C[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp73D;_tmp73D.tag=Cyc_TypeSpecifier_tok;
_tmp73D.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_StructType_struct*
_tmp73E=_cycalloc(sizeof(struct Cyc_Absyn_StructType_struct));_tmp73E[0]=({struct
Cyc_Absyn_StructType_struct _tmp73F;_tmp73F.tag=Cyc_Absyn_StructType;_tmp73F.f1=(
struct _tuple1*)Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp73F.f2=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp73F.f3=
0;_tmp73F;});_tmp73E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp73D;});_tmp73C;});break;case Cyc_Parse_Union_su:
_LL1851: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp741=_cycalloc(
sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp741[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp742;_tmp742.tag=Cyc_TypeSpecifier_tok;_tmp742.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_UnionType_struct*_tmp743=_cycalloc(sizeof(struct Cyc_Absyn_UnionType_struct));
_tmp743[0]=({struct Cyc_Absyn_UnionType_struct _tmp744;_tmp744.tag=Cyc_Absyn_UnionType;
_tmp744.f1=(struct _tuple1*)Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp744.f2=Cyc_yyget_TypeList_tok(
yyvs[yyvsp_offset]);_tmp744.f3=0;_tmp744;});_tmp743;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp742;});_tmp741;});break;}break;case 87: _LL1849:
yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp746=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));
_tmp746[0]=({struct Cyc_TypeList_tok_struct _tmp747;_tmp747.tag=Cyc_TypeList_tok;
_tmp747.f1=0;_tmp747;});_tmp746;});break;case 88: _LL1861: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp749=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));_tmp749[0]=({struct Cyc_TypeList_tok_struct
_tmp74A;_tmp74A.tag=Cyc_TypeList_tok;_tmp74A.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp74A;});_tmp749;});
break;case 89: _LL1864:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp74C=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));_tmp74C[0]=({struct Cyc_TypeList_tok_struct
_tmp74D;_tmp74D.tag=Cyc_TypeList_tok;_tmp74D.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp74D;});_tmp74C;});
break;case 90: _LL1867: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp74F=
_cycalloc_atomic(sizeof(struct Cyc_StructOrUnion_tok_struct));_tmp74F[0]=({struct
Cyc_StructOrUnion_tok_struct _tmp750;_tmp750.tag=Cyc_StructOrUnion_tok;_tmp750.f1=
Cyc_Parse_Struct_su;_tmp750;});_tmp74F;});break;case 91: _LL1870: yyval=(void*)({
struct Cyc_StructOrUnion_tok_struct*_tmp752=_cycalloc_atomic(sizeof(struct Cyc_StructOrUnion_tok_struct));
_tmp752[0]=({struct Cyc_StructOrUnion_tok_struct _tmp753;_tmp753.tag=Cyc_StructOrUnion_tok;
_tmp753.f1=Cyc_Parse_Union_su;_tmp753;});_tmp752;});break;case 92: _LL1873: yyval=(
void*)({struct Cyc_StructFieldDeclList_tok_struct*_tmp755=_cycalloc(sizeof(struct
Cyc_StructFieldDeclList_tok_struct));_tmp755[0]=({struct Cyc_StructFieldDeclList_tok_struct
_tmp756;_tmp756.tag=Cyc_StructFieldDeclList_tok;_tmp756.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_StructFieldDeclListList_tok(yyvs[yyvsp_offset])));
_tmp756;});_tmp755;});break;case 93: _LL1876: yyval=(void*)({struct Cyc_StructFieldDeclListList_tok_struct*
_tmp758=_cycalloc(sizeof(struct Cyc_StructFieldDeclListList_tok_struct));_tmp758[
0]=({struct Cyc_StructFieldDeclListList_tok_struct _tmp759;_tmp759.tag=Cyc_StructFieldDeclListList_tok;
_tmp759.f1=({struct Cyc_List_List*_tmp75A=_cycalloc(sizeof(struct Cyc_List_List));
_tmp75A->hd=Cyc_yyget_StructFieldDeclList_tok(yyvs[yyvsp_offset]);_tmp75A->tl=0;
_tmp75A;});_tmp759;});_tmp758;});break;case 94: _LL1879: yyval=(void*)({struct Cyc_StructFieldDeclListList_tok_struct*
_tmp75C=_cycalloc(sizeof(struct Cyc_StructFieldDeclListList_tok_struct));_tmp75C[
0]=({struct Cyc_StructFieldDeclListList_tok_struct _tmp75D;_tmp75D.tag=Cyc_StructFieldDeclListList_tok;
_tmp75D.f1=({struct Cyc_List_List*_tmp75E=_cycalloc(sizeof(struct Cyc_List_List));
_tmp75E->hd=Cyc_yyget_StructFieldDeclList_tok(yyvs[yyvsp_offset]);_tmp75E->tl=
Cyc_yyget_StructFieldDeclListList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp75E;});_tmp75D;});_tmp75C;});break;case 95: _LL1883: yyval=(
void*)({struct Cyc_InitDeclList_tok_struct*_tmp760=_cycalloc(sizeof(struct Cyc_InitDeclList_tok_struct));
_tmp760[0]=({struct Cyc_InitDeclList_tok_struct _tmp761;_tmp761.tag=Cyc_InitDeclList_tok;
_tmp761.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok(yyvs[yyvsp_offset]));_tmp761;});_tmp760;});break;case
96: _LL1887: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp763=_cycalloc(
sizeof(struct Cyc_InitDeclList_tok_struct));_tmp763[0]=({struct Cyc_InitDeclList_tok_struct
_tmp764;_tmp764.tag=Cyc_InitDeclList_tok;_tmp764.f1=({struct Cyc_List_List*
_tmp765=_cycalloc(sizeof(struct Cyc_List_List));_tmp765->hd=Cyc_yyget_InitDecl_tok(
yyvs[yyvsp_offset]);_tmp765->tl=0;_tmp765;});_tmp764;});_tmp763;});break;case 97:
_LL1890: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp767=_cycalloc(
sizeof(struct Cyc_InitDeclList_tok_struct));_tmp767[0]=({struct Cyc_InitDeclList_tok_struct
_tmp768;_tmp768.tag=Cyc_InitDeclList_tok;_tmp768.f1=({struct Cyc_List_List*
_tmp769=_cycalloc(sizeof(struct Cyc_List_List));_tmp769->hd=Cyc_yyget_InitDecl_tok(
yyvs[yyvsp_offset]);_tmp769->tl=Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp769;});_tmp768;});
_tmp767;});break;case 98: _LL1894: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp76B=_cycalloc(sizeof(struct Cyc_InitDecl_tok_struct));_tmp76B[0]=({struct Cyc_InitDecl_tok_struct
_tmp76C;_tmp76C.tag=Cyc_InitDecl_tok;_tmp76C.f1=({struct _tuple14*_tmp76D=
_cycalloc(sizeof(struct _tuple14));_tmp76D->f1=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp76D->f2=0;_tmp76D;});_tmp76C;});_tmp76B;});break;case 99:
_LL1898: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp76F=_cycalloc(sizeof(
struct Cyc_InitDecl_tok_struct));_tmp76F[0]=({struct Cyc_InitDecl_tok_struct
_tmp770;_tmp770.tag=Cyc_InitDecl_tok;_tmp770.f1=({struct _tuple14*_tmp771=
_cycalloc(sizeof(struct _tuple14));_tmp771->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp771->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp771;});_tmp770;});_tmp76F;});break;
case 100: _LL1902: {struct _tuple17 _tmp775;struct Cyc_List_List*_tmp776;struct Cyc_List_List*
_tmp778;struct Cyc_Absyn_Tqual _tmp77A;struct _tuple17*_tmp773=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp775=*_tmp773;
_LL1915: _tmp77A=_tmp775.f1;goto _LL1913;_LL1913: _tmp778=_tmp775.f2;goto _LL1911;
_LL1911: _tmp776=_tmp775.f3;goto _LL1908;_LL1908: {struct Cyc_List_List*_tmp77E;
struct Cyc_List_List*_tmp780;struct _tuple0 _tmp77C=((struct _tuple0(*)(struct Cyc_List_List*
x))Cyc_List_split)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_LL1921: _tmp780=_tmp77C.f1;
goto _LL1919;_LL1919: _tmp77E=_tmp77C.f2;goto _LL1917;_LL1917: {void*_tmp782=Cyc_Parse_speclist2typ(
_tmp778,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp783=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(Cyc_Parse_apply_tmss(
_tmp77A,_tmp782,_tmp780,_tmp776),_tmp77E);yyval=(void*)({struct Cyc_StructFieldDeclList_tok_struct*
_tmp784=_cycalloc(sizeof(struct Cyc_StructFieldDeclList_tok_struct));_tmp784[0]=({
struct Cyc_StructFieldDeclList_tok_struct _tmp785;_tmp785.tag=Cyc_StructFieldDeclList_tok;
_tmp785.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*f)(struct Cyc_Position_Segment*,
struct _tuple9*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_make_struct_field,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp783);
_tmp785;});_tmp784;});break;}}}case 101: _LL1906: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp787=_cycalloc(sizeof(struct Cyc_QualSpecList_tok_struct));_tmp787[0]=({struct
Cyc_QualSpecList_tok_struct _tmp788;_tmp788.tag=Cyc_QualSpecList_tok;_tmp788.f1=({
struct _tuple17*_tmp789=_cycalloc(sizeof(struct _tuple17));_tmp789->f1=Cyc_Absyn_empty_tqual();
_tmp789->f2=({struct Cyc_List_List*_tmp78A=_cycalloc(sizeof(struct Cyc_List_List));
_tmp78A->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp78A->tl=0;_tmp78A;});
_tmp789->f3=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp789;});_tmp788;});
_tmp787;});break;case 102: _LL1926: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp78C=_cycalloc(sizeof(struct Cyc_QualSpecList_tok_struct));_tmp78C[0]=({struct
Cyc_QualSpecList_tok_struct _tmp78D;_tmp78D.tag=Cyc_QualSpecList_tok;_tmp78D.f1=({
struct _tuple17*_tmp78E=_cycalloc(sizeof(struct _tuple17));_tmp78E->f1=(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f1;_tmp78E->f2=({struct Cyc_List_List*_tmp78F=_cycalloc(
sizeof(struct Cyc_List_List));_tmp78F->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp78F->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f2;_tmp78F;});_tmp78E->f3=Cyc_List_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f3);_tmp78E;});_tmp78D;});_tmp78C;});break;case 103: _LL1931:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp791=_cycalloc(sizeof(struct
Cyc_QualSpecList_tok_struct));_tmp791[0]=({struct Cyc_QualSpecList_tok_struct
_tmp792;_tmp792.tag=Cyc_QualSpecList_tok;_tmp792.f1=({struct _tuple17*_tmp793=
_cycalloc(sizeof(struct _tuple17));_tmp793->f1=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp793->f2=0;_tmp793->f3=
Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp793;});_tmp792;});_tmp791;});
break;case 104: _LL1936: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp795=
_cycalloc(sizeof(struct Cyc_QualSpecList_tok_struct));_tmp795[0]=({struct Cyc_QualSpecList_tok_struct
_tmp796;_tmp796.tag=Cyc_QualSpecList_tok;_tmp796.f1=({struct _tuple17*_tmp797=
_cycalloc(sizeof(struct _tuple17));_tmp797->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f1);_tmp797->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
yyvsp_offset])).f2;_tmp797->f3=Cyc_List_append(Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f3);_tmp797;});_tmp796;});_tmp795;});break;case 105: _LL1940:
yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp799=_cycalloc(sizeof(struct
Cyc_InitDeclList_tok_struct));_tmp799[0]=({struct Cyc_InitDeclList_tok_struct
_tmp79A;_tmp79A.tag=Cyc_InitDeclList_tok;_tmp79A.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
yyvsp_offset]));_tmp79A;});_tmp799;});break;case 106: _LL1944: yyval=(void*)({
struct Cyc_InitDeclList_tok_struct*_tmp79C=_cycalloc(sizeof(struct Cyc_InitDeclList_tok_struct));
_tmp79C[0]=({struct Cyc_InitDeclList_tok_struct _tmp79D;_tmp79D.tag=Cyc_InitDeclList_tok;
_tmp79D.f1=({struct Cyc_List_List*_tmp79E=_cycalloc(sizeof(struct Cyc_List_List));
_tmp79E->hd=Cyc_yyget_InitDecl_tok(yyvs[yyvsp_offset]);_tmp79E->tl=0;_tmp79E;});
_tmp79D;});_tmp79C;});break;case 107: _LL1947: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp7A0=_cycalloc(sizeof(struct Cyc_InitDeclList_tok_struct));_tmp7A0[0]=({struct
Cyc_InitDeclList_tok_struct _tmp7A1;_tmp7A1.tag=Cyc_InitDeclList_tok;_tmp7A1.f1=({
struct Cyc_List_List*_tmp7A2=_cycalloc(sizeof(struct Cyc_List_List));_tmp7A2->hd=
Cyc_yyget_InitDecl_tok(yyvs[yyvsp_offset]);_tmp7A2->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7A2;});_tmp7A1;});
_tmp7A0;});break;case 108: _LL1951: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp7A4=_cycalloc(sizeof(struct Cyc_InitDecl_tok_struct));_tmp7A4[0]=({struct Cyc_InitDecl_tok_struct
_tmp7A5;_tmp7A5.tag=Cyc_InitDecl_tok;_tmp7A5.f1=({struct _tuple14*_tmp7A6=
_cycalloc(sizeof(struct _tuple14));_tmp7A6->f1=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp7A6->f2=0;_tmp7A6;});_tmp7A5;});_tmp7A4;});break;case 109:
_LL1955: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp7A8=_cycalloc(sizeof(
struct Cyc_InitDecl_tok_struct));_tmp7A8[0]=({struct Cyc_InitDecl_tok_struct
_tmp7A9;_tmp7A9.tag=Cyc_InitDecl_tok;_tmp7A9.f1=({struct _tuple14*_tmp7AA=
_cycalloc(sizeof(struct _tuple14));_tmp7AA->f1=({struct Cyc_Parse_Declarator*
_tmp7AB=_cycalloc(sizeof(struct Cyc_Parse_Declarator));_tmp7AB->id=({struct
_tuple1*_tmp7AC=_cycalloc(sizeof(struct _tuple1));_tmp7AC->f1=Cyc_Absyn_rel_ns_null;
_tmp7AC->f2=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(
unsigned char),1);_tmp7AC;});_tmp7AB->tms=0;_tmp7AB;});_tmp7AA->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp7AA;});_tmp7A9;});_tmp7A8;});break;
case 110: _LL1959: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp7AE=_cycalloc(
sizeof(struct Cyc_InitDecl_tok_struct));_tmp7AE[0]=({struct Cyc_InitDecl_tok_struct
_tmp7AF;_tmp7AF.tag=Cyc_InitDecl_tok;_tmp7AF.f1=({struct _tuple14*_tmp7B0=
_cycalloc(sizeof(struct _tuple14));_tmp7B0->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7B0->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp7B0;});_tmp7AF;});_tmp7AE;});break;
case 111: _LL1965: {struct Cyc_List_List*_tmp7B2=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp7B3=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));
_tmp7B3[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp7B4;_tmp7B4.tag=Cyc_TypeSpecifier_tok;
_tmp7B4.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp7B5=_cycalloc(
sizeof(struct Cyc_Parse_Decl_spec_struct));_tmp7B5[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp7B6;_tmp7B6.tag=Cyc_Parse_Decl_spec;_tmp7B6.f1=Cyc_Absyn_tunion_decl((void*)
Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),_tmp7B2,({struct Cyc_Core_Opt*_tmp7B7=_cycalloc(sizeof(struct
Cyc_Core_Opt));_tmp7B7->v=Cyc_yyget_TunionFieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7B7;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7B6;});_tmp7B5;}));_tmp7B4;});_tmp7B3;});break;}
case 112: _LL1969: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp7B9=
_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp7B9[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp7BA;_tmp7BA.tag=Cyc_TypeSpecifier_tok;_tmp7BA.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp7BB=_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));
_tmp7BB[0]=({struct Cyc_Absyn_TunionType_struct _tmp7BC;_tmp7BC.tag=Cyc_Absyn_TunionType;
_tmp7BC.f1=({struct Cyc_Absyn_TunionInfo _tmp7BD;_tmp7BD.tunion_info=(void*)((void*)({
struct Cyc_Absyn_UnknownTunion_struct*_tmp7BE=_cycalloc(sizeof(struct Cyc_Absyn_UnknownTunion_struct));
_tmp7BE[0]=({struct Cyc_Absyn_UnknownTunion_struct _tmp7BF;_tmp7BF.tag=Cyc_Absyn_UnknownTunion;
_tmp7BF.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp7C0;_tmp7C0.name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7C0.is_xtunion=
Cyc_yyget_Bool_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp7C0;});_tmp7BF;});_tmp7BE;}));_tmp7BD.targs=Cyc_yyget_TypeList_tok(yyvs[
yyvsp_offset]);_tmp7BD.rgn=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7BD;});_tmp7BC;});
_tmp7BB;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7BA;});
_tmp7B9;});break;case 113: _LL1976: {void*_tmp7C2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp7CB=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp7CB->v=(void*)((void*)Cyc_Absyn_RgnKind);
_tmp7CB;}),0);yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp7C3=
_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));_tmp7C3[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp7C4;_tmp7C4.tag=Cyc_TypeSpecifier_tok;_tmp7C4.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp7C5=_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));
_tmp7C5[0]=({struct Cyc_Absyn_TunionType_struct _tmp7C6;_tmp7C6.tag=Cyc_Absyn_TunionType;
_tmp7C6.f1=({struct Cyc_Absyn_TunionInfo _tmp7C7;_tmp7C7.tunion_info=(void*)((void*)({
struct Cyc_Absyn_UnknownTunion_struct*_tmp7C8=_cycalloc(sizeof(struct Cyc_Absyn_UnknownTunion_struct));
_tmp7C8[0]=({struct Cyc_Absyn_UnknownTunion_struct _tmp7C9;_tmp7C9.tag=Cyc_Absyn_UnknownTunion;
_tmp7C9.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp7CA;_tmp7CA.name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7CA.is_xtunion=
Cyc_yyget_Bool_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp7CA;});_tmp7C9;});_tmp7C8;}));_tmp7C7.targs=Cyc_yyget_TypeList_tok(yyvs[
yyvsp_offset]);_tmp7C7.rgn=(void*)_tmp7C2;_tmp7C7;});_tmp7C6;});_tmp7C5;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C4;});_tmp7C3;});break;}case 114: _LL1985: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp7CD=_cycalloc(sizeof(struct Cyc_TypeSpecifier_tok_struct));
_tmp7CD[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp7CE;_tmp7CE.tag=Cyc_TypeSpecifier_tok;
_tmp7CE.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp7CF=_cycalloc(sizeof(struct Cyc_Absyn_TunionFieldType_struct));_tmp7CF[0]=({
struct Cyc_Absyn_TunionFieldType_struct _tmp7D0;_tmp7D0.tag=Cyc_Absyn_TunionFieldType;
_tmp7D0.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp7D1;_tmp7D1.field_info=(void*)((
void*)({struct Cyc_Absyn_UnknownTunionfield_struct*_tmp7D2=_cycalloc(sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct));_tmp7D2[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp7D3;_tmp7D3.tag=Cyc_Absyn_UnknownTunionfield;_tmp7D3.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp7D4;_tmp7D4.tunion_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7D4.field_name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7D4.is_xtunion=
Cyc_yyget_Bool_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp7D4;});_tmp7D3;});_tmp7D2;}));_tmp7D1.targs=Cyc_yyget_TypeList_tok(yyvs[
yyvsp_offset]);_tmp7D1;});_tmp7D0;});_tmp7CF;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7CE;});_tmp7CD;});break;case 115: _LL1996: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp7D6=_cycalloc_atomic(sizeof(struct Cyc_Bool_tok_struct));
_tmp7D6[0]=({struct Cyc_Bool_tok_struct _tmp7D7;_tmp7D7.tag=Cyc_Bool_tok;_tmp7D7.f1=
0;_tmp7D7;});_tmp7D6;});break;case 116: _LL2005: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp7D9=_cycalloc_atomic(sizeof(struct Cyc_Bool_tok_struct));_tmp7D9[0]=({struct
Cyc_Bool_tok_struct _tmp7DA;_tmp7DA.tag=Cyc_Bool_tok;_tmp7DA.f1=1;_tmp7DA;});
_tmp7D9;});break;case 117: _LL2008: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp7DC=_cycalloc(sizeof(struct Cyc_TunionFieldList_tok_struct));_tmp7DC[0]=({
struct Cyc_TunionFieldList_tok_struct _tmp7DD;_tmp7DD.tag=Cyc_TunionFieldList_tok;
_tmp7DD.f1=({struct Cyc_List_List*_tmp7DE=_cycalloc(sizeof(struct Cyc_List_List));
_tmp7DE->hd=Cyc_yyget_TunionField_tok(yyvs[yyvsp_offset]);_tmp7DE->tl=0;_tmp7DE;});
_tmp7DD;});_tmp7DC;});break;case 118: _LL2011: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp7E0=_cycalloc(sizeof(struct Cyc_TunionFieldList_tok_struct));_tmp7E0[0]=({
struct Cyc_TunionFieldList_tok_struct _tmp7E1;_tmp7E1.tag=Cyc_TunionFieldList_tok;
_tmp7E1.f1=({struct Cyc_List_List*_tmp7E2=_cycalloc(sizeof(struct Cyc_List_List));
_tmp7E2->hd=Cyc_yyget_TunionField_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp7E2->tl=0;_tmp7E2;});_tmp7E1;});_tmp7E0;});break;case 119:
_LL2015: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp7E4=_cycalloc(
sizeof(struct Cyc_TunionFieldList_tok_struct));_tmp7E4[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp7E5;_tmp7E5.tag=Cyc_TunionFieldList_tok;_tmp7E5.f1=({struct Cyc_List_List*
_tmp7E6=_cycalloc(sizeof(struct Cyc_List_List));_tmp7E6->hd=Cyc_yyget_TunionField_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7E6->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[yyvsp_offset]);_tmp7E6;});_tmp7E5;});_tmp7E4;});break;case 120: _LL2019: yyval=(
void*)({struct Cyc_TunionFieldList_tok_struct*_tmp7E8=_cycalloc(sizeof(struct Cyc_TunionFieldList_tok_struct));
_tmp7E8[0]=({struct Cyc_TunionFieldList_tok_struct _tmp7E9;_tmp7E9.tag=Cyc_TunionFieldList_tok;
_tmp7E9.f1=({struct Cyc_List_List*_tmp7EA=_cycalloc(sizeof(struct Cyc_List_List));
_tmp7EA->hd=Cyc_yyget_TunionField_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp7EA->tl=Cyc_yyget_TunionFieldList_tok(yyvs[yyvsp_offset]);
_tmp7EA;});_tmp7E9;});_tmp7E8;});break;case 121: _LL2023: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp7EC=_cycalloc(sizeof(struct Cyc_Scope_tok_struct));_tmp7EC[0]=({struct Cyc_Scope_tok_struct
_tmp7ED;_tmp7ED.tag=Cyc_Scope_tok;_tmp7ED.f1=(void*)((void*)Cyc_Absyn_Public);
_tmp7ED;});_tmp7EC;});break;case 122: _LL2027: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp7EF=_cycalloc(sizeof(struct Cyc_Scope_tok_struct));_tmp7EF[0]=({struct Cyc_Scope_tok_struct
_tmp7F0;_tmp7F0.tag=Cyc_Scope_tok;_tmp7F0.f1=(void*)((void*)Cyc_Absyn_Extern);
_tmp7F0;});_tmp7EF;});break;case 123: _LL2030: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp7F2=_cycalloc(sizeof(struct Cyc_Scope_tok_struct));_tmp7F2[0]=({struct Cyc_Scope_tok_struct
_tmp7F3;_tmp7F3.tag=Cyc_Scope_tok;_tmp7F3.f1=(void*)((void*)Cyc_Absyn_Static);
_tmp7F3;});_tmp7F2;});break;case 124: _LL2033: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp7F5=_cycalloc(sizeof(struct Cyc_TunionField_tok_struct));_tmp7F5[0]=({struct
Cyc_TunionField_tok_struct _tmp7F6;_tmp7F6.tag=Cyc_TunionField_tok;_tmp7F6.f1=({
struct Cyc_Absyn_Tunionfield*_tmp7F7=_cycalloc(sizeof(struct Cyc_Absyn_Tunionfield));
_tmp7F7->name=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);_tmp7F7->tvs=0;_tmp7F7->typs=
0;_tmp7F7->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line);_tmp7F7->sc=(
void*)Cyc_yyget_Scope_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]);_tmp7F7;});_tmp7F6;});_tmp7F5;});break;case 125: _LL2036: {struct Cyc_List_List*
_tmp7F9=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Position_Segment*,
struct _tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));struct Cyc_List_List*
_tmp7FA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp7FB=_cycalloc(sizeof(struct Cyc_TunionField_tok_struct));
_tmp7FB[0]=({struct Cyc_TunionField_tok_struct _tmp7FC;_tmp7FC.tag=Cyc_TunionField_tok;
_tmp7FC.f1=({struct Cyc_Absyn_Tunionfield*_tmp7FD=_cycalloc(sizeof(struct Cyc_Absyn_Tunionfield));
_tmp7FD->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmp7FD->tvs=_tmp7FA;_tmp7FD->typs=_tmp7F9;_tmp7FD->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp7FD->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp7FD;});_tmp7FC;});
_tmp7FB;});break;}case 126: _LL2040: yyval=yyvs[yyvsp_offset];break;case 127: _LL2046:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp800=_cycalloc(sizeof(struct
Cyc_Declarator_tok_struct));_tmp800[0]=({struct Cyc_Declarator_tok_struct _tmp801;
_tmp801.tag=Cyc_Declarator_tok;_tmp801.f1=({struct Cyc_Parse_Declarator*_tmp802=
_cycalloc(sizeof(struct Cyc_Parse_Declarator));_tmp802->id=(Cyc_yyget_Declarator_tok(
yyvs[yyvsp_offset]))->id;_tmp802->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[yyvsp_offset]))->tms);_tmp802;});_tmp801;});_tmp800;});break;case 128:
_LL2047: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp804=_cycalloc(sizeof(
struct Cyc_Declarator_tok_struct));_tmp804[0]=({struct Cyc_Declarator_tok_struct
_tmp805;_tmp805.tag=Cyc_Declarator_tok;_tmp805.f1=({struct Cyc_Parse_Declarator*
_tmp806=_cycalloc(sizeof(struct Cyc_Parse_Declarator));_tmp806->id=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp806->tms=0;_tmp806;});_tmp805;});_tmp804;});break;case
129: _LL2051: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 130: _LL2055: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp809=
_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));_tmp809[0]=({struct Cyc_Declarator_tok_struct
_tmp80A;_tmp80A.tag=Cyc_Declarator_tok;_tmp80A.f1=({struct Cyc_Parse_Declarator*
_tmp80B=_cycalloc(sizeof(struct Cyc_Parse_Declarator));_tmp80B->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->id;_tmp80B->tms=({
struct Cyc_List_List*_tmp80C=_cycalloc(sizeof(struct Cyc_List_List));_tmp80C->hd=(
void*)((void*)Cyc_Absyn_Carray_mod);_tmp80C->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->tms;_tmp80C;});_tmp80B;});
_tmp80A;});_tmp809;});break;case 131: _LL2056: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp80E=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));_tmp80E[0]=({struct
Cyc_Declarator_tok_struct _tmp80F;_tmp80F.tag=Cyc_Declarator_tok;_tmp80F.f1=({
struct Cyc_Parse_Declarator*_tmp810=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp810->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp810->tms=({struct Cyc_List_List*_tmp811=_cycalloc(
sizeof(struct Cyc_List_List));_tmp811->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp812=_cycalloc(sizeof(struct Cyc_Absyn_ConstArray_mod_struct));_tmp812[0]=({
struct Cyc_Absyn_ConstArray_mod_struct _tmp813;_tmp813.tag=Cyc_Absyn_ConstArray_mod;
_tmp813.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp813;});_tmp812;}));_tmp811->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp811;});
_tmp810;});_tmp80F;});_tmp80E;});break;case 132: _LL2061: {struct _tuple16 _tmp817;
struct Cyc_List_List*_tmp818;struct Cyc_Core_Opt*_tmp81A;struct Cyc_Absyn_VarargInfo*
_tmp81C;int _tmp81E;struct Cyc_List_List*_tmp820;struct _tuple16*_tmp815=Cyc_yyget_YY1(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp817=*_tmp815;
_LL2081: _tmp820=_tmp817.f1;goto _LL2079;_LL2079: _tmp81E=_tmp817.f2;goto _LL2077;
_LL2077: _tmp81C=_tmp817.f3;goto _LL2075;_LL2075: _tmp81A=_tmp817.f4;goto _LL2073;
_LL2073: _tmp818=_tmp817.f5;goto _LL2070;_LL2070: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp822=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));_tmp822[0]=({struct
Cyc_Declarator_tok_struct _tmp823;_tmp823.tag=Cyc_Declarator_tok;_tmp823.f1=({
struct Cyc_Parse_Declarator*_tmp824=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp824->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp824->tms=({struct Cyc_List_List*_tmp825=_cycalloc(
sizeof(struct Cyc_List_List));_tmp825->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp826=_cycalloc(sizeof(struct Cyc_Absyn_Function_mod_struct));_tmp826[0]=({
struct Cyc_Absyn_Function_mod_struct _tmp827;_tmp827.tag=Cyc_Absyn_Function_mod;
_tmp827.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp828=_cycalloc(
sizeof(struct Cyc_Absyn_WithTypes_struct));_tmp828[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp829;_tmp829.tag=Cyc_Absyn_WithTypes;_tmp829.f1=_tmp820;_tmp829.f2=_tmp81E;
_tmp829.f3=_tmp81C;_tmp829.f4=_tmp81A;_tmp829.f5=_tmp818;_tmp829;});_tmp828;}));
_tmp827;});_tmp826;}));_tmp825->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp825;});_tmp824;});
_tmp823;});_tmp822;});break;}case 133: _LL2068: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp82B=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));_tmp82B[0]=({struct
Cyc_Declarator_tok_struct _tmp82C;_tmp82C.tag=Cyc_Declarator_tok;_tmp82C.f1=({
struct Cyc_Parse_Declarator*_tmp82D=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp82D->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp82D->tms=({struct Cyc_List_List*_tmp82E=_cycalloc(
sizeof(struct Cyc_List_List));_tmp82E->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp82F=_cycalloc(sizeof(struct Cyc_Absyn_Function_mod_struct));_tmp82F[0]=({
struct Cyc_Absyn_Function_mod_struct _tmp830;_tmp830.tag=Cyc_Absyn_Function_mod;
_tmp830.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp831=_cycalloc(
sizeof(struct Cyc_Absyn_WithTypes_struct));_tmp831[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp832;_tmp832.tag=Cyc_Absyn_WithTypes;_tmp832.f1=0;_tmp832.f2=0;_tmp832.f3=0;
_tmp832.f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp832.f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp832;});_tmp831;}));
_tmp830;});_tmp82F;}));_tmp82E->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp82E;});_tmp82D;});
_tmp82C;});_tmp82B;});break;case 134: _LL2090: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp834=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));_tmp834[0]=({struct
Cyc_Declarator_tok_struct _tmp835;_tmp835.tag=Cyc_Declarator_tok;_tmp835.f1=({
struct Cyc_Parse_Declarator*_tmp836=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp836->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp836->tms=({struct Cyc_List_List*_tmp837=_cycalloc(
sizeof(struct Cyc_List_List));_tmp837->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp838=_cycalloc(sizeof(struct Cyc_Absyn_Function_mod_struct));_tmp838[0]=({
struct Cyc_Absyn_Function_mod_struct _tmp839;_tmp839.tag=Cyc_Absyn_Function_mod;
_tmp839.f1=(void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp83A=_cycalloc(
sizeof(struct Cyc_Absyn_NoTypes_struct));_tmp83A[0]=({struct Cyc_Absyn_NoTypes_struct
_tmp83B;_tmp83B.tag=Cyc_Absyn_NoTypes;_tmp83B.f1=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp83B.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp83B;});_tmp83A;}));_tmp839;});_tmp838;}));_tmp837->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp837;});_tmp836;});_tmp835;});_tmp834;});break;case 135: _LL2099: {
struct Cyc_List_List*_tmp83D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp83E=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));
_tmp83E[0]=({struct Cyc_Declarator_tok_struct _tmp83F;_tmp83F.tag=Cyc_Declarator_tok;
_tmp83F.f1=({struct Cyc_Parse_Declarator*_tmp840=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp840->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp840->tms=({struct Cyc_List_List*_tmp841=_cycalloc(
sizeof(struct Cyc_List_List));_tmp841->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp842=_cycalloc(sizeof(struct Cyc_Absyn_TypeParams_mod_struct));_tmp842[0]=({
struct Cyc_Absyn_TypeParams_mod_struct _tmp843;_tmp843.tag=Cyc_Absyn_TypeParams_mod;
_tmp843.f1=_tmp83D;_tmp843.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp843.f3=0;_tmp843;});_tmp842;}));_tmp841->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp841;});
_tmp840;});_tmp83F;});_tmp83E;});break;}case 136: _LL2108:((struct Cyc_Lexing_lexbuf*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;{struct Cyc_List_List*
_tmp845=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp846=_cycalloc(sizeof(struct
Cyc_Declarator_tok_struct));_tmp846[0]=({struct Cyc_Declarator_tok_struct _tmp847;
_tmp847.tag=Cyc_Declarator_tok;_tmp847.f1=({struct Cyc_Parse_Declarator*_tmp848=
_cycalloc(sizeof(struct Cyc_Parse_Declarator));_tmp848->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp848->tms=({
struct Cyc_List_List*_tmp849=_cycalloc(sizeof(struct Cyc_List_List));_tmp849->hd=(
void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp84A=_cycalloc(sizeof(
struct Cyc_Absyn_TypeParams_mod_struct));_tmp84A[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp84B;_tmp84B.tag=Cyc_Absyn_TypeParams_mod;_tmp84B.f1=_tmp845;_tmp84B.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp84B.f3=0;_tmp84B;});_tmp84A;}));_tmp849->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp849;});
_tmp848;});_tmp847;});_tmp846;});break;}case 137: _LL2116: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp84D=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));_tmp84D[0]=({struct
Cyc_Declarator_tok_struct _tmp84E;_tmp84E.tag=Cyc_Declarator_tok;_tmp84E.f1=({
struct Cyc_Parse_Declarator*_tmp84F=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp84F->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->id;_tmp84F->tms=({struct Cyc_List_List*_tmp850=_cycalloc(
sizeof(struct Cyc_List_List));_tmp850->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp851=_cycalloc(sizeof(struct Cyc_Absyn_Attributes_mod_struct));_tmp851[0]=({
struct Cyc_Absyn_Attributes_mod_struct _tmp852;_tmp852.tag=Cyc_Absyn_Attributes_mod;
_tmp852.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line);_tmp852.f2=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp852;});_tmp851;}));_tmp850->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp850;});_tmp84F;});
_tmp84E;});_tmp84D;});break;case 138: _LL2124: Cyc_Parse_err(_tag_arr("identifier has not been declared as a typedef",
sizeof(unsigned char),46),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(void*)({
struct Cyc_Declarator_tok_struct*_tmp854=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));
_tmp854[0]=({struct Cyc_Declarator_tok_struct _tmp855;_tmp855.tag=Cyc_Declarator_tok;
_tmp855.f1=({struct Cyc_Parse_Declarator*_tmp856=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp856->id=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);_tmp856->tms=0;_tmp856;});
_tmp855;});_tmp854;});break;case 139: _LL2131: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp858=_cycalloc(sizeof(struct Cyc_Declarator_tok_struct));_tmp858[0]=({struct
Cyc_Declarator_tok_struct _tmp859;_tmp859.tag=Cyc_Declarator_tok;_tmp859.f1=({
struct Cyc_Parse_Declarator*_tmp85A=_cycalloc(sizeof(struct Cyc_Parse_Declarator));
_tmp85A->id=Cyc_Absyn_exn_name;_tmp85A->tms=0;_tmp85A;});_tmp859;});_tmp858;});
break;case 140: _LL2135: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp85C=_cycalloc(sizeof(struct Cyc_TypeModifierList_tok_struct));_tmp85C[0]=({
struct Cyc_TypeModifierList_tok_struct _tmp85D;_tmp85D.tag=Cyc_TypeModifierList_tok;
_tmp85D.f1=({struct Cyc_List_List*_tmp85E=_cycalloc(sizeof(struct Cyc_List_List));
_tmp85E->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp85F=
_cycalloc(sizeof(struct Cyc_Absyn_Pointer_mod_struct));_tmp85F[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp860;_tmp860.tag=Cyc_Absyn_Pointer_mod;_tmp860.f1=(void*)Cyc_yyget_Pointer_Sort_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp860.f2=(void*)
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp860.f3=Cyc_Absyn_empty_tqual();_tmp860;});_tmp85F;}));_tmp85E->tl=Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),
Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]),0);_tmp85E;});_tmp85D;});_tmp85C;});
break;case 141: _LL2139: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp862=_cycalloc(sizeof(struct Cyc_TypeModifierList_tok_struct));_tmp862[0]=({
struct Cyc_TypeModifierList_tok_struct _tmp863;_tmp863.tag=Cyc_TypeModifierList_tok;
_tmp863.f1=({struct Cyc_List_List*_tmp864=_cycalloc(sizeof(struct Cyc_List_List));
_tmp864->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp865=
_cycalloc(sizeof(struct Cyc_Absyn_Pointer_mod_struct));_tmp865[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp866;_tmp866.tag=Cyc_Absyn_Pointer_mod;_tmp866.f1=(void*)Cyc_yyget_Pointer_Sort_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp866.f2=(void*)
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp866.f3=Cyc_yyget_TypeQual_tok(yyvs[yyvsp_offset]);_tmp866;});_tmp865;}));
_tmp864->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0);_tmp864;});
_tmp863;});_tmp862;});break;case 142: _LL2145: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp868=_cycalloc(sizeof(struct Cyc_TypeModifierList_tok_struct));_tmp868[0]=({
struct Cyc_TypeModifierList_tok_struct _tmp869;_tmp869.tag=Cyc_TypeModifierList_tok;
_tmp869.f1=({struct Cyc_List_List*_tmp86A=_cycalloc(sizeof(struct Cyc_List_List));
_tmp86A->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp86B=
_cycalloc(sizeof(struct Cyc_Absyn_Pointer_mod_struct));_tmp86B[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp86C;_tmp86C.tag=Cyc_Absyn_Pointer_mod;_tmp86C.f1=(void*)Cyc_yyget_Pointer_Sort_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp86C.f2=(void*)
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp86C.f3=Cyc_Absyn_empty_tqual();_tmp86C;});_tmp86B;}));_tmp86A->tl=Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp86A;});_tmp869;});_tmp868;});break;case 143: _LL2151:
yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*_tmp86E=_cycalloc(sizeof(
struct Cyc_TypeModifierList_tok_struct));_tmp86E[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp86F;_tmp86F.tag=Cyc_TypeModifierList_tok;_tmp86F.f1=({struct Cyc_List_List*
_tmp870=_cycalloc(sizeof(struct Cyc_List_List));_tmp870->hd=(void*)((void*)({
struct Cyc_Absyn_Pointer_mod_struct*_tmp871=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_mod_struct));
_tmp871[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp872;_tmp872.tag=Cyc_Absyn_Pointer_mod;
_tmp872.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp872.f2=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp872.f3=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp872;});_tmp871;}));
_tmp870->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp870;});_tmp86F;});_tmp86E;});break;case 144: _LL2157:
yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*_tmp874=_cycalloc(sizeof(struct
Cyc_Pointer_Sort_tok_struct));_tmp874[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp875;_tmp875.tag=Cyc_Pointer_Sort_tok;_tmp875.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*
_tmp876=_cycalloc(sizeof(struct Cyc_Absyn_Nullable_ps_struct));_tmp876[0]=({
struct Cyc_Absyn_Nullable_ps_struct _tmp877;_tmp877.tag=Cyc_Absyn_Nullable_ps;
_tmp877.f1=Cyc_Absyn_exp_unsigned_one;_tmp877;});_tmp876;}));_tmp875;});_tmp874;});
break;case 145: _LL2163: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*_tmp879=
_cycalloc(sizeof(struct Cyc_Pointer_Sort_tok_struct));_tmp879[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp87A;_tmp87A.tag=Cyc_Pointer_Sort_tok;_tmp87A.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp87B=_cycalloc(sizeof(struct Cyc_Absyn_NonNullable_ps_struct));_tmp87B[0]=({
struct Cyc_Absyn_NonNullable_ps_struct _tmp87C;_tmp87C.tag=Cyc_Absyn_NonNullable_ps;
_tmp87C.f1=Cyc_Absyn_exp_unsigned_one;_tmp87C;});_tmp87B;}));_tmp87A;});_tmp879;});
break;case 146: _LL2168: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*_tmp87E=
_cycalloc(sizeof(struct Cyc_Pointer_Sort_tok_struct));_tmp87E[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp87F;_tmp87F.tag=Cyc_Pointer_Sort_tok;_tmp87F.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*
_tmp880=_cycalloc(sizeof(struct Cyc_Absyn_Nullable_ps_struct));_tmp880[0]=({
struct Cyc_Absyn_Nullable_ps_struct _tmp881;_tmp881.tag=Cyc_Absyn_Nullable_ps;
_tmp881.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp881;});_tmp880;}));_tmp87F;});_tmp87E;});break;case 147:
_LL2173: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*_tmp883=_cycalloc(
sizeof(struct Cyc_Pointer_Sort_tok_struct));_tmp883[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp884;_tmp884.tag=Cyc_Pointer_Sort_tok;_tmp884.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp885=_cycalloc(sizeof(struct Cyc_Absyn_NonNullable_ps_struct));_tmp885[0]=({
struct Cyc_Absyn_NonNullable_ps_struct _tmp886;_tmp886.tag=Cyc_Absyn_NonNullable_ps;
_tmp886.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp886;});_tmp885;}));_tmp884;});_tmp883;});break;case 148:
_LL2178: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*_tmp888=_cycalloc(
sizeof(struct Cyc_Pointer_Sort_tok_struct));_tmp888[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp889;_tmp889.tag=Cyc_Pointer_Sort_tok;_tmp889.f1=(void*)((void*)Cyc_Absyn_TaggedArray_ps);
_tmp889;});_tmp888;});break;case 149: _LL2183: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp88B=_cycalloc(sizeof(struct Cyc_Type_tok_struct));_tmp88B[0]=({struct Cyc_Type_tok_struct
_tmp88C;_tmp88C.tag=Cyc_Type_tok;_tmp88C.f1=(void*)Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp88D=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp88D->v=(void*)((
void*)Cyc_Absyn_RgnKind);_tmp88D;}),0);_tmp88C;});_tmp88B;});break;case 150:
_LL2186: yyval=yyvs[yyvsp_offset];break;case 151: _LL2190: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(void*)Cyc_Absyn_RgnKind);yyval=yyvs[
yyvsp_offset];break;case 152: _LL2191: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp891=_cycalloc(sizeof(struct Cyc_Type_tok_struct));_tmp891[0]=({struct Cyc_Type_tok_struct
_tmp892;_tmp892.tag=Cyc_Type_tok;_tmp892.f1=(void*)Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp893=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp893->v=(void*)((
void*)Cyc_Absyn_RgnKind);_tmp893;}),0);_tmp892;});_tmp891;});break;case 153:
_LL2192: yyval=yyvs[yyvsp_offset];break;case 154: _LL2196: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp896=_cycalloc_atomic(sizeof(struct Cyc_TypeQual_tok_struct));_tmp896[0]=({
struct Cyc_TypeQual_tok_struct _tmp897;_tmp897.tag=Cyc_TypeQual_tok;_tmp897.f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_yyget_TypeQual_tok(yyvs[yyvsp_offset]));_tmp897;});_tmp896;});break;case 155:
_LL2197: yyval=(void*)({struct Cyc_YY1_struct*_tmp899=_cycalloc(sizeof(struct Cyc_YY1_struct));
_tmp899[0]=({struct Cyc_YY1_struct _tmp89A;_tmp89A.tag=Cyc_YY1;_tmp89A.f1=({struct
_tuple16*_tmp89B=_cycalloc(sizeof(struct _tuple16));_tmp89B->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp89B->f2=0;_tmp89B->f3=
0;_tmp89B->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp89B->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp89B;});_tmp89A;});_tmp899;});break;case 156: _LL2200: yyval=(void*)({struct Cyc_YY1_struct*
_tmp89D=_cycalloc(sizeof(struct Cyc_YY1_struct));_tmp89D[0]=({struct Cyc_YY1_struct
_tmp89E;_tmp89E.tag=Cyc_YY1;_tmp89E.f1=({struct _tuple16*_tmp89F=_cycalloc(
sizeof(struct _tuple16));_tmp89F->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));_tmp89F->f2=1;_tmp89F->f3=
0;_tmp89F->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp89F->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp89F;});_tmp89E;});_tmp89D;});break;case 157: _LL2204: {struct _tuple2 _tmp8A3;
void*_tmp8A4;struct Cyc_Absyn_Tqual _tmp8A6;struct Cyc_Core_Opt*_tmp8A8;struct
_tuple2*_tmp8A1=Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp8A3=*_tmp8A1;_LL2217: _tmp8A8=_tmp8A3.f1;goto _LL2215;
_LL2215: _tmp8A6=_tmp8A3.f2;goto _LL2213;_LL2213: _tmp8A4=_tmp8A3.f3;goto _LL2210;
_LL2210: {struct Cyc_Absyn_VarargInfo*_tmp8AA=({struct Cyc_Absyn_VarargInfo*
_tmp8AE=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_tmp8AE->name=_tmp8A8;
_tmp8AE->tq=_tmp8A6;_tmp8AE->type=(void*)_tmp8A4;_tmp8AE->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp8AE;});yyval=(
void*)({struct Cyc_YY1_struct*_tmp8AB=_cycalloc(sizeof(struct Cyc_YY1_struct));
_tmp8AB[0]=({struct Cyc_YY1_struct _tmp8AC;_tmp8AC.tag=Cyc_YY1;_tmp8AC.f1=({struct
_tuple16*_tmp8AD=_cycalloc(sizeof(struct _tuple16));_tmp8AD->f1=0;_tmp8AD->f2=0;
_tmp8AD->f3=_tmp8AA;_tmp8AD->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp8AD->f5=Cyc_yyget_Rgnorder_tok(
yyvs[yyvsp_offset]);_tmp8AD;});_tmp8AC;});_tmp8AB;});break;}}case 158: _LL2208: {
struct _tuple2 _tmp8B2;void*_tmp8B3;struct Cyc_Absyn_Tqual _tmp8B5;struct Cyc_Core_Opt*
_tmp8B7;struct _tuple2*_tmp8B0=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8B2=*_tmp8B0;_LL2232:
_tmp8B7=_tmp8B2.f1;goto _LL2230;_LL2230: _tmp8B5=_tmp8B2.f2;goto _LL2228;_LL2228:
_tmp8B3=_tmp8B2.f3;goto _LL2225;_LL2225: {struct Cyc_Absyn_VarargInfo*_tmp8B9=({
struct Cyc_Absyn_VarargInfo*_tmp8BD=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));
_tmp8BD->name=_tmp8B7;_tmp8BD->tq=_tmp8B5;_tmp8BD->type=(void*)_tmp8B3;_tmp8BD->inject=
Cyc_yyget_Bool_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp8BD;});yyval=(void*)({struct Cyc_YY1_struct*_tmp8BA=_cycalloc(sizeof(struct
Cyc_YY1_struct));_tmp8BA[0]=({struct Cyc_YY1_struct _tmp8BB;_tmp8BB.tag=Cyc_YY1;
_tmp8BB.f1=({struct _tuple16*_tmp8BC=_cycalloc(sizeof(struct _tuple16));_tmp8BC->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp8BC->f2=0;
_tmp8BC->f3=_tmp8B9;_tmp8BC->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp8BC->f5=Cyc_yyget_Rgnorder_tok(
yyvs[yyvsp_offset]);_tmp8BC;});_tmp8BB;});_tmp8BA;});break;}}case 159: _LL2223:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp8BF=_cycalloc(sizeof(struct Cyc_Type_tok_struct));
_tmp8BF[0]=({struct Cyc_Type_tok_struct _tmp8C0;_tmp8C0.tag=Cyc_Type_tok;_tmp8C0.f1=(
void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),(void*)({struct
Cyc_Absyn_Unknown_kb_struct*_tmp8C1=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_struct));
_tmp8C1[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp8C2;_tmp8C2.tag=Cyc_Absyn_Unknown_kb;
_tmp8C2.f1=0;_tmp8C2;});_tmp8C1;}));_tmp8C0;});_tmp8BF;});break;case 160: _LL2238:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp8C4=_cycalloc(sizeof(struct Cyc_Type_tok_struct));
_tmp8C4[0]=({struct Cyc_Type_tok_struct _tmp8C5;_tmp8C5.tag=Cyc_Type_tok;_tmp8C5.f1=(
void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8C6=_cycalloc(
sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp8C6[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp8C7;_tmp8C7.tag=Cyc_Absyn_Eq_kb;_tmp8C7.f1=(void*)Cyc_yyget_Kind_tok(yyvs[
yyvsp_offset]);_tmp8C7;});_tmp8C6;}));_tmp8C5;});_tmp8C4;});break;case 161:
_LL2243: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp8C9=_cycalloc(sizeof(
struct Cyc_TypeOpt_tok_struct));_tmp8C9[0]=({struct Cyc_TypeOpt_tok_struct _tmp8CA;
_tmp8CA.tag=Cyc_TypeOpt_tok;_tmp8CA.f1=0;_tmp8CA;});_tmp8C9;});break;case 162:
_LL2248: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp8CC=_cycalloc(sizeof(
struct Cyc_TypeOpt_tok_struct));_tmp8CC[0]=({struct Cyc_TypeOpt_tok_struct _tmp8CD;
_tmp8CD.tag=Cyc_TypeOpt_tok;_tmp8CD.f1=({struct Cyc_Core_Opt*_tmp8CE=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp8CE->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp8CF=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp8CF[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp8D0;_tmp8D0.tag=Cyc_Absyn_JoinEff;_tmp8D0.f1=Cyc_yyget_TypeList_tok(yyvs[
yyvsp_offset]);_tmp8D0;});_tmp8CF;}));_tmp8CE;});_tmp8CD;});_tmp8CC;});break;
case 163: _LL2251: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp8D2=_cycalloc(
sizeof(struct Cyc_Rgnorder_tok_struct));_tmp8D2[0]=({struct Cyc_Rgnorder_tok_struct
_tmp8D3;_tmp8D3.tag=Cyc_Rgnorder_tok;_tmp8D3.f1=0;_tmp8D3;});_tmp8D2;});break;
case 164: _LL2257: yyval=yyvs[yyvsp_offset];break;case 165: _LL2260: yyval=(void*)({
struct Cyc_Rgnorder_tok_struct*_tmp8D6=_cycalloc(sizeof(struct Cyc_Rgnorder_tok_struct));
_tmp8D6[0]=({struct Cyc_Rgnorder_tok_struct _tmp8D7;_tmp8D7.tag=Cyc_Rgnorder_tok;
_tmp8D7.f1=({struct Cyc_List_List*_tmp8D8=_cycalloc(sizeof(struct Cyc_List_List));
_tmp8D8->hd=({struct _tuple18*_tmp8D9=_cycalloc(sizeof(struct _tuple18));_tmp8D9->f1=
Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8DC=_cycalloc(
sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp8DC[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp8DD;_tmp8DD.tag=Cyc_Absyn_Eq_kb;_tmp8DD.f1=(void*)((void*)Cyc_Absyn_RgnKind);
_tmp8DD;});_tmp8DC;}));_tmp8D9->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
yyvsp_offset]),(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8DA=_cycalloc(sizeof(
struct Cyc_Absyn_Eq_kb_struct));_tmp8DA[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8DB;
_tmp8DB.tag=Cyc_Absyn_Eq_kb;_tmp8DB.f1=(void*)((void*)Cyc_Absyn_RgnKind);_tmp8DB;});
_tmp8DA;}));_tmp8D9;});_tmp8D8->tl=0;_tmp8D8;});_tmp8D7;});_tmp8D6;});break;case
166: _LL2261: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp8DF=_cycalloc(
sizeof(struct Cyc_Rgnorder_tok_struct));_tmp8DF[0]=({struct Cyc_Rgnorder_tok_struct
_tmp8E0;_tmp8E0.tag=Cyc_Rgnorder_tok;_tmp8E0.f1=({struct Cyc_List_List*_tmp8E1=
_cycalloc(sizeof(struct Cyc_List_List));_tmp8E1->hd=({struct _tuple18*_tmp8E2=
_cycalloc(sizeof(struct _tuple18));_tmp8E2->f1=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp8E5=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp8E5[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp8E6;_tmp8E6.tag=Cyc_Absyn_Eq_kb;_tmp8E6.f1=(void*)((void*)Cyc_Absyn_RgnKind);
_tmp8E6;});_tmp8E5;}));_tmp8E2->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp8E3=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp8E3[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp8E4;_tmp8E4.tag=Cyc_Absyn_Eq_kb;_tmp8E4.f1=(void*)((void*)Cyc_Absyn_RgnKind);
_tmp8E4;});_tmp8E3;}));_tmp8E2;});_tmp8E1->tl=Cyc_yyget_Rgnorder_tok(yyvs[
yyvsp_offset]);_tmp8E1;});_tmp8E0;});_tmp8DF;});break;case 167: _LL2270: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp8E8=_cycalloc_atomic(sizeof(struct Cyc_Bool_tok_struct));
_tmp8E8[0]=({struct Cyc_Bool_tok_struct _tmp8E9;_tmp8E9.tag=Cyc_Bool_tok;_tmp8E9.f1=
0;_tmp8E9;});_tmp8E8;});break;case 168: _LL2279: {struct _tagged_arr _tmp8EB=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);if(Cyc_Std_zstrcmp(_tmp8EB,_tag_arr("inject",sizeof(
unsigned char),7))!= 0){Cyc_Parse_err(_tag_arr("missing type in function declaration",
sizeof(unsigned char),37),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp8EC=_cycalloc_atomic(sizeof(struct Cyc_Bool_tok_struct));_tmp8EC[0]=({struct
Cyc_Bool_tok_struct _tmp8ED;_tmp8ED.tag=Cyc_Bool_tok;_tmp8ED.f1=1;_tmp8ED;});
_tmp8EC;});break;}case 169: _LL2282: yyval=yyvs[yyvsp_offset];break;case 170: _LL2286:
yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp8F0=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));
_tmp8F0[0]=({struct Cyc_TypeList_tok_struct _tmp8F1;_tmp8F1.tag=Cyc_TypeList_tok;
_tmp8F1.f1=Cyc_List_imp_append(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeList_tok(
yyvs[yyvsp_offset]));_tmp8F1;});_tmp8F0;});break;case 171: _LL2287: yyval=(void*)({
struct Cyc_TypeList_tok_struct*_tmp8F3=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));
_tmp8F3[0]=({struct Cyc_TypeList_tok_struct _tmp8F4;_tmp8F4.tag=Cyc_TypeList_tok;
_tmp8F4.f1=0;_tmp8F4;});_tmp8F3;});break;case 172: _LL2290: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 173: _LL2293:
yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp8F7=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));
_tmp8F7[0]=({struct Cyc_TypeList_tok_struct _tmp8F8;_tmp8F8.tag=Cyc_TypeList_tok;
_tmp8F8.f1=({struct Cyc_List_List*_tmp8F9=_cycalloc(sizeof(struct Cyc_List_List));
_tmp8F9->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp8FA=_cycalloc(
sizeof(struct Cyc_Absyn_RgnsEff_struct));_tmp8FA[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp8FB;_tmp8FB.tag=Cyc_Absyn_RgnsEff;_tmp8FB.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp8FB;});_tmp8FA;}));
_tmp8F9->tl=0;_tmp8F9;});_tmp8F8;});_tmp8F7;});break;case 174: _LL2294: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(void*)Cyc_Absyn_EffKind);yyval=(void*)({
struct Cyc_TypeList_tok_struct*_tmp8FD=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));
_tmp8FD[0]=({struct Cyc_TypeList_tok_struct _tmp8FE;_tmp8FE.tag=Cyc_TypeList_tok;
_tmp8FE.f1=({struct Cyc_List_List*_tmp8FF=_cycalloc(sizeof(struct Cyc_List_List));
_tmp8FF->hd=(void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);_tmp8FF->tl=0;_tmp8FF;});
_tmp8FE;});_tmp8FD;});break;case 175: _LL2300: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(
yyvs[yyvsp_offset]),(void*)Cyc_Absyn_RgnKind);yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp901=_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));_tmp901[0]=({struct Cyc_TypeList_tok_struct
_tmp902;_tmp902.tag=Cyc_TypeList_tok;_tmp902.f1=({struct Cyc_List_List*_tmp903=
_cycalloc(sizeof(struct Cyc_List_List));_tmp903->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp904=_cycalloc(sizeof(struct Cyc_Absyn_AccessEff_struct));_tmp904[0]=({struct
Cyc_Absyn_AccessEff_struct _tmp905;_tmp905.tag=Cyc_Absyn_AccessEff;_tmp905.f1=(
void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);_tmp905;});_tmp904;}));_tmp903->tl=
0;_tmp903;});_tmp902;});_tmp901;});break;case 176: _LL2304: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
void*)Cyc_Absyn_RgnKind);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp907=
_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));_tmp907[0]=({struct Cyc_TypeList_tok_struct
_tmp908;_tmp908.tag=Cyc_TypeList_tok;_tmp908.f1=({struct Cyc_List_List*_tmp909=
_cycalloc(sizeof(struct Cyc_List_List));_tmp909->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp90A=_cycalloc(sizeof(struct Cyc_Absyn_AccessEff_struct));_tmp90A[0]=({struct
Cyc_Absyn_AccessEff_struct _tmp90B;_tmp90B.tag=Cyc_Absyn_AccessEff;_tmp90B.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp90B;});_tmp90A;}));_tmp909->tl=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);
_tmp909;});_tmp908;});_tmp907;});break;case 177: _LL2310: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp90D=_cycalloc(sizeof(struct Cyc_ParamDeclList_tok_struct));_tmp90D[0]=({
struct Cyc_ParamDeclList_tok_struct _tmp90E;_tmp90E.tag=Cyc_ParamDeclList_tok;
_tmp90E.f1=({struct Cyc_List_List*_tmp90F=_cycalloc(sizeof(struct Cyc_List_List));
_tmp90F->hd=Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset]);_tmp90F->tl=0;_tmp90F;});
_tmp90E;});_tmp90D;});break;case 178: _LL2316: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp911=_cycalloc(sizeof(struct Cyc_ParamDeclList_tok_struct));_tmp911[0]=({
struct Cyc_ParamDeclList_tok_struct _tmp912;_tmp912.tag=Cyc_ParamDeclList_tok;
_tmp912.f1=({struct Cyc_List_List*_tmp913=_cycalloc(sizeof(struct Cyc_List_List));
_tmp913->hd=Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset]);_tmp913->tl=Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp913;});_tmp912;});
_tmp911;});break;case 179: _LL2320: {struct _tuple17 _tmp917;struct Cyc_List_List*
_tmp918;struct Cyc_List_List*_tmp91A;struct Cyc_Absyn_Tqual _tmp91C;struct _tuple17*
_tmp915=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp917=*_tmp915;_LL2333: _tmp91C=_tmp917.f1;goto _LL2331;
_LL2331: _tmp91A=_tmp917.f2;goto _LL2329;_LL2329: _tmp918=_tmp917.f3;goto _LL2326;
_LL2326: {struct Cyc_Parse_Declarator _tmp920;struct Cyc_List_List*_tmp921;struct
_tuple1*_tmp923;struct Cyc_Parse_Declarator*_tmp91E=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp920=*_tmp91E;_LL2340: _tmp923=_tmp920.id;goto _LL2338;_LL2338:
_tmp921=_tmp920.tms;goto _LL2335;_LL2335: {void*_tmp925=Cyc_Parse_speclist2typ(
_tmp91A,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp928;struct Cyc_List_List*
_tmp92A;void*_tmp92C;struct Cyc_Absyn_Tqual _tmp92E;struct _tuple6 _tmp926=Cyc_Parse_apply_tms(
_tmp91C,_tmp925,_tmp918,_tmp921);_LL2351: _tmp92E=_tmp926.f1;goto _LL2349;_LL2349:
_tmp92C=_tmp926.f2;goto _LL2347;_LL2347: _tmp92A=_tmp926.f3;goto _LL2345;_LL2345:
_tmp928=_tmp926.f4;goto _LL2343;_LL2343: if(_tmp92A != 0){Cyc_Parse_err(_tag_arr("parameter with bad type params",
sizeof(unsigned char),31),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp923)){Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a namespace",sizeof(unsigned char),
47),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*_tmp930=(struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt*_tmp935=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp935->v=(*
_tmp923).f2;_tmp935;});if(_tmp928 != 0){({void*_tmp931[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("extra attributes on parameter, ignoring",
sizeof(unsigned char),40),_tag_arr(_tmp931,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp932=_cycalloc(sizeof(struct Cyc_ParamDecl_tok_struct));
_tmp932[0]=({struct Cyc_ParamDecl_tok_struct _tmp933;_tmp933.tag=Cyc_ParamDecl_tok;
_tmp933.f1=({struct _tuple2*_tmp934=_cycalloc(sizeof(struct _tuple2));_tmp934->f1=
_tmp930;_tmp934->f2=_tmp92E;_tmp934->f3=_tmp92C;_tmp934;});_tmp933;});_tmp932;});
break;}}}}case 180: _LL2324: {struct _tuple17 _tmp939;struct Cyc_List_List*_tmp93A;
struct Cyc_List_List*_tmp93C;struct Cyc_Absyn_Tqual _tmp93E;struct _tuple17*_tmp937=
Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset]);_tmp939=*_tmp937;_LL2367: _tmp93E=
_tmp939.f1;goto _LL2365;_LL2365: _tmp93C=_tmp939.f2;goto _LL2363;_LL2363: _tmp93A=
_tmp939.f3;goto _LL2360;_LL2360: {void*_tmp940=Cyc_Parse_speclist2typ(_tmp93C,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));if(_tmp93A != 0){({
void*_tmp941[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(unsigned char),38),_tag_arr(_tmp941,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp942=_cycalloc(sizeof(struct Cyc_ParamDecl_tok_struct));
_tmp942[0]=({struct Cyc_ParamDecl_tok_struct _tmp943;_tmp943.tag=Cyc_ParamDecl_tok;
_tmp943.f1=({struct _tuple2*_tmp944=_cycalloc(sizeof(struct _tuple2));_tmp944->f1=
0;_tmp944->f2=_tmp93E;_tmp944->f3=_tmp940;_tmp944;});_tmp943;});_tmp942;});
break;}}case 181: _LL2358: {struct _tuple17 _tmp948;struct Cyc_List_List*_tmp949;
struct Cyc_List_List*_tmp94B;struct Cyc_Absyn_Tqual _tmp94D;struct _tuple17*_tmp946=
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp948=*_tmp946;_LL2382: _tmp94D=_tmp948.f1;goto _LL2380;
_LL2380: _tmp94B=_tmp948.f2;goto _LL2378;_LL2378: _tmp949=_tmp948.f3;goto _LL2375;
_LL2375: {void*_tmp94F=Cyc_Parse_speclist2typ(_tmp94B,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp950=(Cyc_yyget_AbstractDeclarator_tok(yyvs[yyvsp_offset]))->tms;struct Cyc_List_List*
_tmp953;struct Cyc_List_List*_tmp955;void*_tmp957;struct Cyc_Absyn_Tqual _tmp959;
struct _tuple6 _tmp951=Cyc_Parse_apply_tms(_tmp94D,_tmp94F,_tmp949,_tmp950);
_LL2394: _tmp959=_tmp951.f1;goto _LL2392;_LL2392: _tmp957=_tmp951.f2;goto _LL2390;
_LL2390: _tmp955=_tmp951.f3;goto _LL2388;_LL2388: _tmp953=_tmp951.f4;goto _LL2386;
_LL2386: if(_tmp955 != 0){({void*_tmp95B[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("bad type parameters on formal argument, ignoring",
sizeof(unsigned char),49),_tag_arr(_tmp95B,sizeof(void*),0));});}if(_tmp953 != 0){({
void*_tmp95C[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(unsigned char),38),_tag_arr(_tmp95C,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp95D=_cycalloc(sizeof(struct Cyc_ParamDecl_tok_struct));
_tmp95D[0]=({struct Cyc_ParamDecl_tok_struct _tmp95E;_tmp95E.tag=Cyc_ParamDecl_tok;
_tmp95E.f1=({struct _tuple2*_tmp95F=_cycalloc(sizeof(struct _tuple2));_tmp95F->f1=
0;_tmp95F->f2=_tmp959;_tmp95F->f3=_tmp957;_tmp95F;});_tmp95E;});_tmp95D;});
break;}}case 182: _LL2373: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp961=
_cycalloc(sizeof(struct Cyc_IdList_tok_struct));_tmp961[0]=({struct Cyc_IdList_tok_struct
_tmp962;_tmp962.tag=Cyc_IdList_tok;_tmp962.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_IdList_tok(yyvs[yyvsp_offset]));
_tmp962;});_tmp961;});break;case 183: _LL2400: yyval=(void*)({struct Cyc_IdList_tok_struct*
_tmp964=_cycalloc(sizeof(struct Cyc_IdList_tok_struct));_tmp964[0]=({struct Cyc_IdList_tok_struct
_tmp965;_tmp965.tag=Cyc_IdList_tok;_tmp965.f1=({struct Cyc_List_List*_tmp966=
_cycalloc(sizeof(struct Cyc_List_List));_tmp966->hd=({struct _tagged_arr*_tmp967=
_cycalloc(sizeof(struct _tagged_arr));_tmp967[0]=Cyc_yyget_String_tok(yyvs[
yyvsp_offset]);_tmp967;});_tmp966->tl=0;_tmp966;});_tmp965;});_tmp964;});break;
case 184: _LL2403: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp969=_cycalloc(
sizeof(struct Cyc_IdList_tok_struct));_tmp969[0]=({struct Cyc_IdList_tok_struct
_tmp96A;_tmp96A.tag=Cyc_IdList_tok;_tmp96A.f1=({struct Cyc_List_List*_tmp96B=
_cycalloc(sizeof(struct Cyc_List_List));_tmp96B->hd=({struct _tagged_arr*_tmp96C=
_cycalloc(sizeof(struct _tagged_arr));_tmp96C[0]=Cyc_yyget_String_tok(yyvs[
yyvsp_offset]);_tmp96C;});_tmp96B->tl=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp96B;});_tmp96A;});
_tmp969;});break;case 185: _LL2408: yyval=yyvs[yyvsp_offset];break;case 186: _LL2413:
yyval=yyvs[yyvsp_offset];break;case 187: _LL2414: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp970=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp970[0]=({struct Cyc_Exp_tok_struct
_tmp971;_tmp971.tag=Cyc_Exp_tok;_tmp971.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp972=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_struct));_tmp972[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp973;_tmp973.tag=Cyc_Absyn_UnresolvedMem_e;
_tmp973.f1=0;_tmp973.f2=0;_tmp973;});_tmp972;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp971;});_tmp970;});break;case 188: _LL2415: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp975=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmp975[0]=({struct Cyc_Exp_tok_struct _tmp976;_tmp976.tag=Cyc_Exp_tok;_tmp976.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp977=
_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_struct));_tmp977[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp978;_tmp978.tag=Cyc_Absyn_UnresolvedMem_e;_tmp978.f1=0;_tmp978.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp978;});_tmp977;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp976;});
_tmp975;});break;case 189: _LL2420: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp97A=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmp97A[0]=({struct Cyc_Exp_tok_struct
_tmp97B;_tmp97B.tag=Cyc_Exp_tok;_tmp97B.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp97C=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_struct));_tmp97C[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp97D;_tmp97D.tag=Cyc_Absyn_UnresolvedMem_e;
_tmp97D.f1=0;_tmp97D.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp97D;});_tmp97C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp97B;});_tmp97A;});break;case 190: _LL2425: {struct
Cyc_Absyn_Vardecl*_tmp97F=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp985=
_cycalloc(sizeof(struct _tuple1));_tmp985->f1=(void*)Cyc_Absyn_Loc_n;_tmp985->f2=({
struct _tagged_arr*_tmp986=_cycalloc(sizeof(struct _tagged_arr));_tmp986[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp986;});_tmp985;}),
Cyc_Absyn_uint_t,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));_tmp97F->tq=({
struct Cyc_Absyn_Tqual _tmp980;_tmp980.q_const=1;_tmp980.q_volatile=0;_tmp980.q_restrict=
1;_tmp980;});yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp981=_cycalloc(sizeof(
struct Cyc_Exp_tok_struct));_tmp981[0]=({struct Cyc_Exp_tok_struct _tmp982;_tmp982.tag=
Cyc_Exp_tok;_tmp982.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp983=_cycalloc(sizeof(struct Cyc_Absyn_Comprehension_e_struct));_tmp983[0]=({
struct Cyc_Absyn_Comprehension_e_struct _tmp984;_tmp984.tag=Cyc_Absyn_Comprehension_e;
_tmp984.f1=_tmp97F;_tmp984.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp984.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp984;});_tmp983;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmp982;});
_tmp981;});break;}case 191: _LL2430: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp988=_cycalloc(sizeof(struct Cyc_InitializerList_tok_struct));_tmp988[0]=({
struct Cyc_InitializerList_tok_struct _tmp989;_tmp989.tag=Cyc_InitializerList_tok;
_tmp989.f1=({struct Cyc_List_List*_tmp98A=_cycalloc(sizeof(struct Cyc_List_List));
_tmp98A->hd=({struct _tuple19*_tmp98B=_cycalloc(sizeof(struct _tuple19));_tmp98B->f1=
0;_tmp98B->f2=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp98B;});_tmp98A->tl=0;
_tmp98A;});_tmp989;});_tmp988;});break;case 192: _LL2439: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp98D=_cycalloc(sizeof(struct Cyc_InitializerList_tok_struct));_tmp98D[0]=({
struct Cyc_InitializerList_tok_struct _tmp98E;_tmp98E.tag=Cyc_InitializerList_tok;
_tmp98E.f1=({struct Cyc_List_List*_tmp98F=_cycalloc(sizeof(struct Cyc_List_List));
_tmp98F->hd=({struct _tuple19*_tmp990=_cycalloc(sizeof(struct _tuple19));_tmp990->f1=
Cyc_yyget_DesignatorList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp990->f2=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp990;});
_tmp98F->tl=0;_tmp98F;});_tmp98E;});_tmp98D;});break;case 193: _LL2444: yyval=(void*)({
struct Cyc_InitializerList_tok_struct*_tmp992=_cycalloc(sizeof(struct Cyc_InitializerList_tok_struct));
_tmp992[0]=({struct Cyc_InitializerList_tok_struct _tmp993;_tmp993.tag=Cyc_InitializerList_tok;
_tmp993.f1=({struct Cyc_List_List*_tmp994=_cycalloc(sizeof(struct Cyc_List_List));
_tmp994->hd=({struct _tuple19*_tmp995=_cycalloc(sizeof(struct _tuple19));_tmp995->f1=
0;_tmp995->f2=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp995;});_tmp994->tl=Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp994;});_tmp993;});
_tmp992;});break;case 194: _LL2449: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp997=_cycalloc(sizeof(struct Cyc_InitializerList_tok_struct));_tmp997[0]=({
struct Cyc_InitializerList_tok_struct _tmp998;_tmp998.tag=Cyc_InitializerList_tok;
_tmp998.f1=({struct Cyc_List_List*_tmp999=_cycalloc(sizeof(struct Cyc_List_List));
_tmp999->hd=({struct _tuple19*_tmp99A=_cycalloc(sizeof(struct _tuple19));_tmp99A->f1=
Cyc_yyget_DesignatorList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp99A->f2=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp99A;});
_tmp999->tl=Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp999;});_tmp998;});_tmp997;});break;case 195: _LL2454:
yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp99C=_cycalloc(sizeof(
struct Cyc_DesignatorList_tok_struct));_tmp99C[0]=({struct Cyc_DesignatorList_tok_struct
_tmp99D;_tmp99D.tag=Cyc_DesignatorList_tok;_tmp99D.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp99D;});_tmp99C;});
break;case 196: _LL2459: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp99F=
_cycalloc(sizeof(struct Cyc_DesignatorList_tok_struct));_tmp99F[0]=({struct Cyc_DesignatorList_tok_struct
_tmp9A0;_tmp9A0.tag=Cyc_DesignatorList_tok;_tmp9A0.f1=({struct Cyc_List_List*
_tmp9A1=_cycalloc(sizeof(struct Cyc_List_List));_tmp9A1->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[yyvsp_offset]);_tmp9A1->tl=0;_tmp9A1;});_tmp9A0;});_tmp99F;});break;case 197:
_LL2462: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp9A3=_cycalloc(
sizeof(struct Cyc_DesignatorList_tok_struct));_tmp9A3[0]=({struct Cyc_DesignatorList_tok_struct
_tmp9A4;_tmp9A4.tag=Cyc_DesignatorList_tok;_tmp9A4.f1=({struct Cyc_List_List*
_tmp9A5=_cycalloc(sizeof(struct Cyc_List_List));_tmp9A5->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[yyvsp_offset]);_tmp9A5->tl=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp9A5;});_tmp9A4;});
_tmp9A3;});break;case 198: _LL2466: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp9A7=_cycalloc(sizeof(struct Cyc_Designator_tok_struct));_tmp9A7[0]=({struct
Cyc_Designator_tok_struct _tmp9A8;_tmp9A8.tag=Cyc_Designator_tok;_tmp9A8.f1=(void*)((
void*)({struct Cyc_Absyn_ArrayElement_struct*_tmp9A9=_cycalloc(sizeof(struct Cyc_Absyn_ArrayElement_struct));
_tmp9A9[0]=({struct Cyc_Absyn_ArrayElement_struct _tmp9AA;_tmp9AA.tag=Cyc_Absyn_ArrayElement;
_tmp9AA.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp9AA;});_tmp9A9;}));_tmp9A8;});_tmp9A7;});break;case 199:
_LL2470: yyval=(void*)({struct Cyc_Designator_tok_struct*_tmp9AC=_cycalloc(sizeof(
struct Cyc_Designator_tok_struct));_tmp9AC[0]=({struct Cyc_Designator_tok_struct
_tmp9AD;_tmp9AD.tag=Cyc_Designator_tok;_tmp9AD.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp9AE=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_struct));_tmp9AE[0]=({struct
Cyc_Absyn_FieldName_struct _tmp9AF;_tmp9AF.tag=Cyc_Absyn_FieldName;_tmp9AF.f1=({
struct _tagged_arr*_tmp9B0=_cycalloc(sizeof(struct _tagged_arr));_tmp9B0[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp9B0;});_tmp9AF;});_tmp9AE;}));_tmp9AD;});_tmp9AC;});
break;case 200: _LL2475: {void*_tmp9B2=Cyc_Parse_speclist2typ((*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));struct Cyc_List_List*_tmp9B3=(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f3;if(_tmp9B3 != 0){({void*_tmp9B4[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("ignoring attributes in type",
sizeof(unsigned char),28),_tag_arr(_tmp9B4,sizeof(void*),0));});}{struct Cyc_Absyn_Tqual
_tmp9B5=(*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f1;yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp9B6=_cycalloc(sizeof(struct Cyc_ParamDecl_tok_struct));
_tmp9B6[0]=({struct Cyc_ParamDecl_tok_struct _tmp9B7;_tmp9B7.tag=Cyc_ParamDecl_tok;
_tmp9B7.f1=({struct _tuple2*_tmp9B8=_cycalloc(sizeof(struct _tuple2));_tmp9B8->f1=
0;_tmp9B8->f2=_tmp9B5;_tmp9B8->f3=_tmp9B2;_tmp9B8;});_tmp9B7;});_tmp9B6;});
break;}}case 201: _LL2481: {void*_tmp9BA=Cyc_Parse_speclist2typ((*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp9BB=(*Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f3;struct Cyc_Absyn_Tqual _tmp9BC=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f1;struct Cyc_List_List*
_tmp9BD=(Cyc_yyget_AbstractDeclarator_tok(yyvs[yyvsp_offset]))->tms;struct
_tuple6 _tmp9BE=Cyc_Parse_apply_tms(_tmp9BC,_tmp9BA,_tmp9BB,_tmp9BD);if(_tmp9BE.f3
!= 0){({void*_tmp9BF[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad type params, ignoring",
sizeof(unsigned char),26),_tag_arr(_tmp9BF,sizeof(void*),0));});}if(_tmp9BE.f4 != 
0){({void*_tmp9C0[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad specifiers, ignoring",
sizeof(unsigned char),25),_tag_arr(_tmp9C0,sizeof(void*),0));});}yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp9C1=_cycalloc(sizeof(struct Cyc_ParamDecl_tok_struct));
_tmp9C1[0]=({struct Cyc_ParamDecl_tok_struct _tmp9C2;_tmp9C2.tag=Cyc_ParamDecl_tok;
_tmp9C2.f1=({struct _tuple2*_tmp9C3=_cycalloc(sizeof(struct _tuple2));_tmp9C3->f1=
0;_tmp9C3->f2=_tmp9BE.f1;_tmp9C3->f3=_tmp9BE.f2;_tmp9C3;});_tmp9C2;});_tmp9C1;});
break;}case 202: _LL2489: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp9C5=
_cycalloc(sizeof(struct Cyc_Type_tok_struct));_tmp9C5[0]=({struct Cyc_Type_tok_struct
_tmp9C6;_tmp9C6.tag=Cyc_Type_tok;_tmp9C6.f1=(void*)(*Cyc_yyget_ParamDecl_tok(
yyvs[yyvsp_offset])).f3;_tmp9C6;});_tmp9C5;});break;case 203: _LL2500: yyval=(void*)({
struct Cyc_Type_tok_struct*_tmp9C8=_cycalloc(sizeof(struct Cyc_Type_tok_struct));
_tmp9C8[0]=({struct Cyc_Type_tok_struct _tmp9C9;_tmp9C9.tag=Cyc_Type_tok;_tmp9C9.f1=(
void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp9CA=_cycalloc(sizeof(struct
Cyc_Absyn_JoinEff_struct));_tmp9CA[0]=({struct Cyc_Absyn_JoinEff_struct _tmp9CB;
_tmp9CB.tag=Cyc_Absyn_JoinEff;_tmp9CB.f1=0;_tmp9CB;});_tmp9CA;}));_tmp9C9;});
_tmp9C8;});break;case 204: _LL2503: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp9CD=_cycalloc(sizeof(struct Cyc_Type_tok_struct));_tmp9CD[0]=({struct Cyc_Type_tok_struct
_tmp9CE;_tmp9CE.tag=Cyc_Type_tok;_tmp9CE.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp9CF=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp9CF[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp9D0;_tmp9D0.tag=Cyc_Absyn_JoinEff;_tmp9D0.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp9D0;});_tmp9CF;}));
_tmp9CE;});_tmp9CD;});break;case 205: _LL2508: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp9D2=_cycalloc(sizeof(struct Cyc_Type_tok_struct));_tmp9D2[0]=({struct Cyc_Type_tok_struct
_tmp9D3;_tmp9D3.tag=Cyc_Type_tok;_tmp9D3.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp9D4=_cycalloc(sizeof(struct Cyc_Absyn_RgnsEff_struct));_tmp9D4[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp9D5;_tmp9D5.tag=Cyc_Absyn_RgnsEff;_tmp9D5.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp9D5;});_tmp9D4;}));
_tmp9D3;});_tmp9D2;});break;case 206: _LL2513: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp9D7=_cycalloc(sizeof(struct Cyc_Type_tok_struct));_tmp9D7[0]=({struct Cyc_Type_tok_struct
_tmp9D8;_tmp9D8.tag=Cyc_Type_tok;_tmp9D8.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp9D9=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp9D9[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp9DA;_tmp9DA.tag=Cyc_Absyn_JoinEff;_tmp9DA.f1=({struct Cyc_List_List*_tmp9DB=
_cycalloc(sizeof(struct Cyc_List_List));_tmp9DB->hd=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp9DB->tl=Cyc_yyget_TypeList_tok(
yyvs[yyvsp_offset]);_tmp9DB;});_tmp9DA;});_tmp9D9;}));_tmp9D8;});_tmp9D7;});
break;case 207: _LL2518: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp9DD=
_cycalloc(sizeof(struct Cyc_TypeList_tok_struct));_tmp9DD[0]=({struct Cyc_TypeList_tok_struct
_tmp9DE;_tmp9DE.tag=Cyc_TypeList_tok;_tmp9DE.f1=({struct Cyc_List_List*_tmp9DF=
_cycalloc(sizeof(struct Cyc_List_List));_tmp9DF->hd=(void*)Cyc_yyget_Type_tok(
yyvs[yyvsp_offset]);_tmp9DF->tl=0;_tmp9DF;});_tmp9DE;});_tmp9DD;});break;case 208:
_LL2524: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp9E1=_cycalloc(sizeof(
struct Cyc_TypeList_tok_struct));_tmp9E1[0]=({struct Cyc_TypeList_tok_struct
_tmp9E2;_tmp9E2.tag=Cyc_TypeList_tok;_tmp9E2.f1=({struct Cyc_List_List*_tmp9E3=
_cycalloc(sizeof(struct Cyc_List_List));_tmp9E3->hd=(void*)Cyc_yyget_Type_tok(
yyvs[yyvsp_offset]);_tmp9E3->tl=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp9E3;});_tmp9E2;});
_tmp9E1;});break;case 209: _LL2528: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp9E5=_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmp9E5[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp9E6;_tmp9E6.tag=Cyc_AbstractDeclarator_tok;
_tmp9E6.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp9E7=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmp9E7->tms=Cyc_yyget_TypeModifierList_tok(yyvs[
yyvsp_offset]);_tmp9E7;});_tmp9E6;});_tmp9E5;});break;case 210: _LL2532: yyval=yyvs[
yyvsp_offset];break;case 211: _LL2536: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp9EA=_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmp9EA[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp9EB;_tmp9EB.tag=Cyc_AbstractDeclarator_tok;
_tmp9EB.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp9EC=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmp9EC->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[yyvsp_offset]))->tms);_tmp9EC;});_tmp9EB;});_tmp9EA;});break;case 212:
_LL2537: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 213: _LL2541: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp9EF=
_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmp9EF[0]=({struct
Cyc_AbstractDeclarator_tok_struct _tmp9F0;_tmp9F0.tag=Cyc_AbstractDeclarator_tok;
_tmp9F0.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp9F1=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmp9F1->tms=({struct Cyc_List_List*_tmp9F2=
_cycalloc(sizeof(struct Cyc_List_List));_tmp9F2->hd=(void*)((void*)Cyc_Absyn_Carray_mod);
_tmp9F2->tl=0;_tmp9F2;});_tmp9F1;});_tmp9F0;});_tmp9EF;});break;case 214: _LL2542:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp9F4=_cycalloc(sizeof(
struct Cyc_AbstractDeclarator_tok_struct));_tmp9F4[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp9F5;_tmp9F5.tag=Cyc_AbstractDeclarator_tok;_tmp9F5.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp9F6=_cycalloc(sizeof(struct Cyc_Parse_Abstractdeclarator));_tmp9F6->tms=({
struct Cyc_List_List*_tmp9F7=_cycalloc(sizeof(struct Cyc_List_List));_tmp9F7->hd=(
void*)((void*)Cyc_Absyn_Carray_mod);_tmp9F7->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->tms;_tmp9F7;});
_tmp9F6;});_tmp9F5;});_tmp9F4;});break;case 215: _LL2547: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp9F9=_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmp9F9[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp9FA;_tmp9FA.tag=Cyc_AbstractDeclarator_tok;
_tmp9FA.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp9FB=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmp9FB->tms=({struct Cyc_List_List*_tmp9FC=
_cycalloc(sizeof(struct Cyc_List_List));_tmp9FC->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp9FD=_cycalloc(sizeof(struct Cyc_Absyn_ConstArray_mod_struct));_tmp9FD[0]=({
struct Cyc_Absyn_ConstArray_mod_struct _tmp9FE;_tmp9FE.tag=Cyc_Absyn_ConstArray_mod;
_tmp9FE.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp9FE;});_tmp9FD;}));_tmp9FC->tl=0;_tmp9FC;});_tmp9FB;});
_tmp9FA;});_tmp9F9;});break;case 216: _LL2552: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmpA00=_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmpA00[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmpA01;_tmpA01.tag=Cyc_AbstractDeclarator_tok;
_tmpA01.f1=({struct Cyc_Parse_Abstractdeclarator*_tmpA02=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmpA02->tms=({struct Cyc_List_List*_tmpA03=
_cycalloc(sizeof(struct Cyc_List_List));_tmpA03->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmpA04=_cycalloc(sizeof(struct Cyc_Absyn_ConstArray_mod_struct));_tmpA04[0]=({
struct Cyc_Absyn_ConstArray_mod_struct _tmpA05;_tmpA05.tag=Cyc_Absyn_ConstArray_mod;
_tmpA05.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmpA05;});_tmpA04;}));_tmpA03->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmpA03;});
_tmpA02;});_tmpA01;});_tmpA00;});break;case 217: _LL2559: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmpA07=_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmpA07[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmpA08;_tmpA08.tag=Cyc_AbstractDeclarator_tok;
_tmpA08.f1=({struct Cyc_Parse_Abstractdeclarator*_tmpA09=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmpA09->tms=({struct Cyc_List_List*_tmpA0A=
_cycalloc(sizeof(struct Cyc_List_List));_tmpA0A->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmpA0B=_cycalloc(sizeof(struct Cyc_Absyn_Function_mod_struct));_tmpA0B[0]=({
struct Cyc_Absyn_Function_mod_struct _tmpA0C;_tmpA0C.tag=Cyc_Absyn_Function_mod;
_tmpA0C.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmpA0D=_cycalloc(
sizeof(struct Cyc_Absyn_WithTypes_struct));_tmpA0D[0]=({struct Cyc_Absyn_WithTypes_struct
_tmpA0E;_tmpA0E.tag=Cyc_Absyn_WithTypes;_tmpA0E.f1=0;_tmpA0E.f2=0;_tmpA0E.f3=0;
_tmpA0E.f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmpA0E.f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmpA0E;});_tmpA0D;}));
_tmpA0C;});_tmpA0B;}));_tmpA0A->tl=0;_tmpA0A;});_tmpA09;});_tmpA08;});_tmpA07;});
break;case 218: _LL2566: {struct _tuple16 _tmpA12;struct Cyc_List_List*_tmpA13;struct
Cyc_Core_Opt*_tmpA15;struct Cyc_Absyn_VarargInfo*_tmpA17;int _tmpA19;struct Cyc_List_List*
_tmpA1B;struct _tuple16*_tmpA10=Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmpA12=*_tmpA10;_LL2588: _tmpA1B=_tmpA12.f1;goto _LL2586;
_LL2586: _tmpA19=_tmpA12.f2;goto _LL2584;_LL2584: _tmpA17=_tmpA12.f3;goto _LL2582;
_LL2582: _tmpA15=_tmpA12.f4;goto _LL2580;_LL2580: _tmpA13=_tmpA12.f5;goto _LL2577;
_LL2577: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmpA1D=_cycalloc(
sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmpA1D[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmpA1E;_tmpA1E.tag=Cyc_AbstractDeclarator_tok;_tmpA1E.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmpA1F=_cycalloc(sizeof(struct Cyc_Parse_Abstractdeclarator));_tmpA1F->tms=({
struct Cyc_List_List*_tmpA20=_cycalloc(sizeof(struct Cyc_List_List));_tmpA20->hd=(
void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmpA21=_cycalloc(sizeof(
struct Cyc_Absyn_Function_mod_struct));_tmpA21[0]=({struct Cyc_Absyn_Function_mod_struct
_tmpA22;_tmpA22.tag=Cyc_Absyn_Function_mod;_tmpA22.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmpA23=_cycalloc(sizeof(struct Cyc_Absyn_WithTypes_struct));_tmpA23[0]=({struct
Cyc_Absyn_WithTypes_struct _tmpA24;_tmpA24.tag=Cyc_Absyn_WithTypes;_tmpA24.f1=
_tmpA1B;_tmpA24.f2=_tmpA19;_tmpA24.f3=_tmpA17;_tmpA24.f4=_tmpA15;_tmpA24.f5=
_tmpA13;_tmpA24;});_tmpA23;}));_tmpA22;});_tmpA21;}));_tmpA20->tl=0;_tmpA20;});
_tmpA1F;});_tmpA1E;});_tmpA1D;});break;}case 219: _LL2575: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmpA26=_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmpA26[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmpA27;_tmpA27.tag=Cyc_AbstractDeclarator_tok;
_tmpA27.f1=({struct Cyc_Parse_Abstractdeclarator*_tmpA28=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmpA28->tms=({struct Cyc_List_List*_tmpA29=
_cycalloc(sizeof(struct Cyc_List_List));_tmpA29->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmpA2A=_cycalloc(sizeof(struct Cyc_Absyn_Function_mod_struct));_tmpA2A[0]=({
struct Cyc_Absyn_Function_mod_struct _tmpA2B;_tmpA2B.tag=Cyc_Absyn_Function_mod;
_tmpA2B.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmpA2C=_cycalloc(
sizeof(struct Cyc_Absyn_WithTypes_struct));_tmpA2C[0]=({struct Cyc_Absyn_WithTypes_struct
_tmpA2D;_tmpA2D.tag=Cyc_Absyn_WithTypes;_tmpA2D.f1=0;_tmpA2D.f2=0;_tmpA2D.f3=0;
_tmpA2D.f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmpA2D.f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmpA2D;});_tmpA2C;}));
_tmpA2B;});_tmpA2A;}));_tmpA29->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmpA29;});_tmpA28;});
_tmpA27;});_tmpA26;});break;case 220: _LL2597: {struct _tuple16 _tmpA31;struct Cyc_List_List*
_tmpA32;struct Cyc_Core_Opt*_tmpA34;struct Cyc_Absyn_VarargInfo*_tmpA36;int _tmpA38;
struct Cyc_List_List*_tmpA3A;struct _tuple16*_tmpA2F=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmpA31=*_tmpA2F;_LL2619:
_tmpA3A=_tmpA31.f1;goto _LL2617;_LL2617: _tmpA38=_tmpA31.f2;goto _LL2615;_LL2615:
_tmpA36=_tmpA31.f3;goto _LL2613;_LL2613: _tmpA34=_tmpA31.f4;goto _LL2611;_LL2611:
_tmpA32=_tmpA31.f5;goto _LL2608;_LL2608: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmpA3C=_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmpA3C[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmpA3D;_tmpA3D.tag=Cyc_AbstractDeclarator_tok;
_tmpA3D.f1=({struct Cyc_Parse_Abstractdeclarator*_tmpA3E=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmpA3E->tms=({struct Cyc_List_List*_tmpA3F=
_cycalloc(sizeof(struct Cyc_List_List));_tmpA3F->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmpA40=_cycalloc(sizeof(struct Cyc_Absyn_Function_mod_struct));_tmpA40[0]=({
struct Cyc_Absyn_Function_mod_struct _tmpA41;_tmpA41.tag=Cyc_Absyn_Function_mod;
_tmpA41.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmpA42=_cycalloc(
sizeof(struct Cyc_Absyn_WithTypes_struct));_tmpA42[0]=({struct Cyc_Absyn_WithTypes_struct
_tmpA43;_tmpA43.tag=Cyc_Absyn_WithTypes;_tmpA43.f1=_tmpA3A;_tmpA43.f2=_tmpA38;
_tmpA43.f3=_tmpA36;_tmpA43.f4=_tmpA34;_tmpA43.f5=_tmpA32;_tmpA43;});_tmpA42;}));
_tmpA41;});_tmpA40;}));_tmpA3F->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmpA3F;});_tmpA3E;});
_tmpA3D;});_tmpA3C;});break;}case 221: _LL2606: {struct Cyc_List_List*_tmpA45=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmpA46=_cycalloc(sizeof(
struct Cyc_AbstractDeclarator_tok_struct));_tmpA46[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmpA47;_tmpA47.tag=Cyc_AbstractDeclarator_tok;_tmpA47.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmpA48=_cycalloc(sizeof(struct Cyc_Parse_Abstractdeclarator));_tmpA48->tms=({
struct Cyc_List_List*_tmpA49=_cycalloc(sizeof(struct Cyc_List_List));_tmpA49->hd=(
void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmpA4A=_cycalloc(sizeof(
struct Cyc_Absyn_TypeParams_mod_struct));_tmpA4A[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmpA4B;_tmpA4B.tag=Cyc_Absyn_TypeParams_mod;_tmpA4B.f1=_tmpA45;_tmpA4B.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmpA4B.f3=0;_tmpA4B;});_tmpA4A;}));_tmpA49->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmpA49;});
_tmpA48;});_tmpA47;});_tmpA46;});break;}case 222: _LL2628:((struct Cyc_Lexing_lexbuf*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;{struct Cyc_List_List*
_tmpA4D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmpA4E=_cycalloc(sizeof(
struct Cyc_AbstractDeclarator_tok_struct));_tmpA4E[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmpA4F;_tmpA4F.tag=Cyc_AbstractDeclarator_tok;_tmpA4F.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmpA50=_cycalloc(sizeof(struct Cyc_Parse_Abstractdeclarator));_tmpA50->tms=({
struct Cyc_List_List*_tmpA51=_cycalloc(sizeof(struct Cyc_List_List));_tmpA51->hd=(
void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmpA52=_cycalloc(sizeof(
struct Cyc_Absyn_TypeParams_mod_struct));_tmpA52[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmpA53;_tmpA53.tag=Cyc_Absyn_TypeParams_mod;_tmpA53.f1=_tmpA4D;_tmpA53.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmpA53.f3=0;_tmpA53;});_tmpA52;}));_tmpA51->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmpA51;});
_tmpA50;});_tmpA4F;});_tmpA4E;});break;}case 223: _LL2636: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmpA55=_cycalloc(sizeof(struct Cyc_AbstractDeclarator_tok_struct));_tmpA55[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmpA56;_tmpA56.tag=Cyc_AbstractDeclarator_tok;
_tmpA56.f1=({struct Cyc_Parse_Abstractdeclarator*_tmpA57=_cycalloc(sizeof(struct
Cyc_Parse_Abstractdeclarator));_tmpA57->tms=({struct Cyc_List_List*_tmpA58=
_cycalloc(sizeof(struct Cyc_List_List));_tmpA58->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmpA59=_cycalloc(sizeof(struct Cyc_Absyn_Attributes_mod_struct));_tmpA59[0]=({
struct Cyc_Absyn_Attributes_mod_struct _tmpA5A;_tmpA5A.tag=Cyc_Absyn_Attributes_mod;
_tmpA5A.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line);_tmpA5A.f2=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmpA5A;});_tmpA59;}));_tmpA58->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmpA58;});
_tmpA57;});_tmpA56;});_tmpA55;});break;case 224: _LL2644: yyval=yyvs[yyvsp_offset];
break;case 225: _LL2651: yyval=yyvs[yyvsp_offset];break;case 226: _LL2652: yyval=yyvs[
yyvsp_offset];break;case 227: _LL2653: yyval=yyvs[yyvsp_offset];break;case 228:
_LL2654: yyval=yyvs[yyvsp_offset];break;case 229: _LL2655: yyval=yyvs[yyvsp_offset];
break;case 230: _LL2656: if(Cyc_Std_zstrcmp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tag_arr("`H",sizeof(
unsigned char),3))== 0){Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(unsigned char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmpA6C=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_tmpA6C->name=({struct _tagged_arr*_tmpA6F=_cycalloc(sizeof(struct _tagged_arr));
_tmpA6F[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmpA6F;});_tmpA6C->identity=0;_tmpA6C->kind=(void*)((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmpA6D=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));
_tmpA6D[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpA6E;_tmpA6E.tag=Cyc_Absyn_Eq_kb;
_tmpA6E.f1=(void*)((void*)Cyc_Absyn_RgnKind);_tmpA6E;});_tmpA6D;}));_tmpA6C;});
void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmpA6A=_cycalloc(sizeof(struct
Cyc_Absyn_VarType_struct));_tmpA6A[0]=({struct Cyc_Absyn_VarType_struct _tmpA6B;
_tmpA6B.tag=Cyc_Absyn_VarType;_tmpA6B.f1=tv;_tmpA6B;});_tmpA6A;});yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmpA62=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpA62[0]=({struct Cyc_Stmt_tok_struct _tmpA63;_tmpA63.tag=Cyc_Stmt_tok;_tmpA63.f1=
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmpA64=_cycalloc(
sizeof(struct Cyc_Absyn_Region_s_struct));_tmpA64[0]=({struct Cyc_Absyn_Region_s_struct
_tmpA65;_tmpA65.tag=Cyc_Absyn_Region_s;_tmpA65.f1=tv;_tmpA65.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmpA66=_cycalloc(sizeof(struct _tuple1));_tmpA66->f1=(void*)Cyc_Absyn_Loc_n;
_tmpA66->f2=({struct _tagged_arr*_tmpA67=_cycalloc(sizeof(struct _tagged_arr));
_tmpA67[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmpA67;});_tmpA66;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmpA68=_cycalloc(sizeof(struct Cyc_Absyn_RgnHandleType_struct));_tmpA68[0]=({
struct Cyc_Absyn_RgnHandleType_struct _tmpA69;_tmpA69.tag=Cyc_Absyn_RgnHandleType;
_tmpA69.f1=(void*)t;_tmpA69;});_tmpA68;}),0);_tmpA65.f3=Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]);_tmpA65;});_tmpA64;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpA63;});_tmpA62;});break;}case 231: _LL2657: if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tag_arr("H",sizeof(unsigned char),2))== 0){Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(unsigned char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmpA7B=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_tmpA7B->name=({struct _tagged_arr*_tmpA7E=_cycalloc(sizeof(struct _tagged_arr));
_tmpA7E[0]=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpA80;_tmpA80.tag=
Cyc_Std_String_pa;_tmpA80.f1=(struct _tagged_arr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{void*_tmpA7F[1]={&
_tmpA80};Cyc_Std_aprintf(_tag_arr("`%s",sizeof(unsigned char),4),_tag_arr(
_tmpA7F,sizeof(void*),1));}});_tmpA7E;});_tmpA7B->identity=0;_tmpA7B->kind=(void*)((
void*)({struct Cyc_Absyn_Eq_kb_struct*_tmpA7C=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));
_tmpA7C[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpA7D;_tmpA7D.tag=Cyc_Absyn_Eq_kb;
_tmpA7D.f1=(void*)((void*)Cyc_Absyn_RgnKind);_tmpA7D;});_tmpA7C;}));_tmpA7B;});
void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmpA79=_cycalloc(sizeof(struct
Cyc_Absyn_VarType_struct));_tmpA79[0]=({struct Cyc_Absyn_VarType_struct _tmpA7A;
_tmpA7A.tag=Cyc_Absyn_VarType;_tmpA7A.f1=tv;_tmpA7A;});_tmpA79;});yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmpA71=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpA71[0]=({struct Cyc_Stmt_tok_struct _tmpA72;_tmpA72.tag=Cyc_Stmt_tok;_tmpA72.f1=
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmpA73=_cycalloc(
sizeof(struct Cyc_Absyn_Region_s_struct));_tmpA73[0]=({struct Cyc_Absyn_Region_s_struct
_tmpA74;_tmpA74.tag=Cyc_Absyn_Region_s;_tmpA74.f1=tv;_tmpA74.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmpA75=_cycalloc(sizeof(struct _tuple1));_tmpA75->f1=(void*)Cyc_Absyn_Loc_n;
_tmpA75->f2=({struct _tagged_arr*_tmpA76=_cycalloc(sizeof(struct _tagged_arr));
_tmpA76[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmpA76;});_tmpA75;}),(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmpA77=_cycalloc(sizeof(struct Cyc_Absyn_RgnHandleType_struct));_tmpA77[0]=({
struct Cyc_Absyn_RgnHandleType_struct _tmpA78;_tmpA78.tag=Cyc_Absyn_RgnHandleType;
_tmpA78.f1=(void*)t;_tmpA78;});_tmpA77;}),0);_tmpA74.f3=Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]);_tmpA74;});_tmpA73;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpA72;});_tmpA71;});break;}case 232: _LL2672: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpA82=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpA82[0]=({struct Cyc_Stmt_tok_struct _tmpA83;_tmpA83.tag=Cyc_Stmt_tok;_tmpA83.f1=
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Cut_s_struct*_tmpA84=_cycalloc(
sizeof(struct Cyc_Absyn_Cut_s_struct));_tmpA84[0]=({struct Cyc_Absyn_Cut_s_struct
_tmpA85;_tmpA85.tag=Cyc_Absyn_Cut_s;_tmpA85.f1=Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]);_tmpA85;});_tmpA84;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpA83;});_tmpA82;});break;case 233: _LL2689: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpA87=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpA87[0]=({struct Cyc_Stmt_tok_struct _tmpA88;_tmpA88.tag=Cyc_Stmt_tok;_tmpA88.f1=
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Splice_s_struct*_tmpA89=_cycalloc(
sizeof(struct Cyc_Absyn_Splice_s_struct));_tmpA89[0]=({struct Cyc_Absyn_Splice_s_struct
_tmpA8A;_tmpA8A.tag=Cyc_Absyn_Splice_s;_tmpA8A.f1=Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]);_tmpA8A;});_tmpA89;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpA88;});_tmpA87;});break;case 234: _LL2694: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpA8C=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpA8C[0]=({struct Cyc_Stmt_tok_struct _tmpA8D;_tmpA8D.tag=Cyc_Stmt_tok;_tmpA8D.f1=
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_struct*_tmpA8E=_cycalloc(
sizeof(struct Cyc_Absyn_Label_s_struct));_tmpA8E[0]=({struct Cyc_Absyn_Label_s_struct
_tmpA8F;_tmpA8F.tag=Cyc_Absyn_Label_s;_tmpA8F.f1=({struct _tagged_arr*_tmpA90=
_cycalloc(sizeof(struct _tagged_arr));_tmpA90[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmpA90;});_tmpA8F.f2=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmpA8F;});_tmpA8E;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpA8D;});_tmpA8C;});break;case 235: _LL2699: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpA92=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpA92[0]=({struct Cyc_Stmt_tok_struct _tmpA93;_tmpA93.tag=Cyc_Stmt_tok;_tmpA93.f1=
Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmpA93;});_tmpA92;});break;case 236: _LL2705:
yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmpA95=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpA95[0]=({struct Cyc_Stmt_tok_struct _tmpA96;_tmpA96.tag=Cyc_Stmt_tok;_tmpA96.f1=
Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpA96;});_tmpA95;});break;case 237: _LL2708: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpA98=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpA98[0]=({struct Cyc_Stmt_tok_struct _tmpA99;_tmpA99.tag=Cyc_Stmt_tok;_tmpA99.f1=
Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpA99;});_tmpA98;});break;case 238: _LL2711: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 239:
_LL2714: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmpA9C=_cycalloc(sizeof(struct
Cyc_Stmt_tok_struct));_tmpA9C[0]=({struct Cyc_Stmt_tok_struct _tmpA9D;_tmpA9D.tag=
Cyc_Stmt_tok;_tmpA9D.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(0));_tmpA9D;});_tmpA9C;});break;case 240:
_LL2715: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmpA9F=_cycalloc(sizeof(struct
Cyc_Stmt_tok_struct));_tmpA9F[0]=({struct Cyc_Stmt_tok_struct _tmpAA0;_tmpAA0.tag=
Cyc_Stmt_tok;_tmpAA0.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]));_tmpAA0;});_tmpA9F;});break;case 241: _LL2718: yyval=yyvs[
yyvsp_offset];break;case 242: _LL2721: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmpAA3=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpAA3[0]=({struct Cyc_Stmt_tok_struct
_tmpAA4;_tmpAA4.tag=Cyc_Stmt_tok;_tmpAA4.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpAA4;});_tmpAA3;});break;case 243: _LL2722: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpAA6=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpAA6[0]=({struct Cyc_Stmt_tok_struct _tmpAA7;_tmpAA7.tag=Cyc_Stmt_tok;_tmpAA7.f1=
Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmpAA8=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_struct));_tmpAA8[0]=({struct Cyc_Absyn_Fn_d_struct
_tmpAA9;_tmpAA9.tag=Cyc_Absyn_Fn_d;_tmpAA9.f1=Cyc_yyget_FnDecl_tok(yyvs[
yyvsp_offset]);_tmpAA9;});_tmpAA8;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line)),Cyc_Absyn_skip_stmt(0));
_tmpAA7;});_tmpAA6;});break;case 244: _LL2725: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmpAAB=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpAAB[0]=({struct Cyc_Stmt_tok_struct
_tmpAAC;_tmpAAC.tag=Cyc_Stmt_tok;_tmpAAC.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmpAAD=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_struct));
_tmpAAD[0]=({struct Cyc_Absyn_Fn_d_struct _tmpAAE;_tmpAAE.tag=Cyc_Absyn_Fn_d;
_tmpAAE.f1=Cyc_yyget_FnDecl_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmpAAE;});_tmpAAD;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]));_tmpAAC;});_tmpAAB;});break;case 245: _LL2730: yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmpAB0=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpAB0[0]=({struct Cyc_Stmt_tok_struct _tmpAB1;_tmpAB1.tag=Cyc_Stmt_tok;_tmpAB1.f1=
Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(
0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmpAB1;});
_tmpAB0;});break;case 246: _LL2735: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmpAB3=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpAB3[0]=({struct Cyc_Stmt_tok_struct
_tmpAB4;_tmpAB4.tag=Cyc_Stmt_tok;_tmpAB4.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpAB4;});_tmpAB3;});break;case 247: _LL2738: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpAB6=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpAB6[0]=({struct Cyc_Stmt_tok_struct _tmpAB7;_tmpAB7.tag=Cyc_Stmt_tok;_tmpAB7.f1=
Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpAB7;});_tmpAB6;});break;case 248: _LL2741: if(Cyc_Std_strcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tag_arr("C",sizeof(unsigned char),2))!= 0){Cyc_Parse_err(_tag_arr("only switch \"C\" { ... } is allowed",
sizeof(unsigned char),35),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmpAB9=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpAB9[0]=({struct Cyc_Stmt_tok_struct
_tmpABA;_tmpABA.tag=Cyc_Stmt_tok;_tmpABA.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_SwitchC_s_struct*_tmpABB=_cycalloc(sizeof(struct Cyc_Absyn_SwitchC_s_struct));
_tmpABB[0]=({struct Cyc_Absyn_SwitchC_s_struct _tmpABC;_tmpABC.tag=Cyc_Absyn_SwitchC_s;
_tmpABC.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmpABC.f2=Cyc_yyget_SwitchCClauseList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmpABC;});_tmpABB;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmpABA;});
_tmpAB9;});break;case 249: _LL2744: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmpABE=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpABE[0]=({struct Cyc_Stmt_tok_struct
_tmpABF;_tmpABF.tag=Cyc_Stmt_tok;_tmpABF.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpABF;});_tmpABE;});break;case 250: _LL2749: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmpAC1=_cycalloc(sizeof(struct Cyc_SwitchClauseList_tok_struct));
_tmpAC1[0]=({struct Cyc_SwitchClauseList_tok_struct _tmpAC2;_tmpAC2.tag=Cyc_SwitchClauseList_tok;
_tmpAC2.f1=0;_tmpAC2;});_tmpAC1;});break;case 251: _LL2752: yyval=(void*)({struct
Cyc_SwitchClauseList_tok_struct*_tmpAC4=_cycalloc(sizeof(struct Cyc_SwitchClauseList_tok_struct));
_tmpAC4[0]=({struct Cyc_SwitchClauseList_tok_struct _tmpAC5;_tmpAC5.tag=Cyc_SwitchClauseList_tok;
_tmpAC5.f1=({struct Cyc_List_List*_tmpAC6=_cycalloc(sizeof(struct Cyc_List_List));
_tmpAC6->hd=({struct Cyc_Absyn_Switch_clause*_tmpAC7=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
_tmpAC7->pattern=Cyc_Absyn_new_pat((void*)Cyc_Absyn_Wild_p,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmpAC7->pat_vars=
0;_tmpAC7->where_clause=0;_tmpAC7->body=Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);
_tmpAC7->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line);_tmpAC7;});
_tmpAC6->tl=0;_tmpAC6;});_tmpAC5;});_tmpAC4;});break;case 252: _LL2755: yyval=(void*)({
struct Cyc_SwitchClauseList_tok_struct*_tmpAC9=_cycalloc(sizeof(struct Cyc_SwitchClauseList_tok_struct));
_tmpAC9[0]=({struct Cyc_SwitchClauseList_tok_struct _tmpACA;_tmpACA.tag=Cyc_SwitchClauseList_tok;
_tmpACA.f1=({struct Cyc_List_List*_tmpACB=_cycalloc(sizeof(struct Cyc_List_List));
_tmpACB->hd=({struct Cyc_Absyn_Switch_clause*_tmpACC=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
_tmpACC->pattern=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmpACC->pat_vars=0;_tmpACC->where_clause=0;_tmpACC->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmpACC->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmpACC;});_tmpACB->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmpACB;});_tmpACA;});_tmpAC9;});break;case 253: _LL2760: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmpACE=_cycalloc(sizeof(struct Cyc_SwitchClauseList_tok_struct));
_tmpACE[0]=({struct Cyc_SwitchClauseList_tok_struct _tmpACF;_tmpACF.tag=Cyc_SwitchClauseList_tok;
_tmpACF.f1=({struct Cyc_List_List*_tmpAD0=_cycalloc(sizeof(struct Cyc_List_List));
_tmpAD0->hd=({struct Cyc_Absyn_Switch_clause*_tmpAD1=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
_tmpAD1->pattern=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmpAD1->pat_vars=0;_tmpAD1->where_clause=0;_tmpAD1->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmpAD1->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmpAD1;});
_tmpAD0->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmpAD0;});
_tmpACF;});_tmpACE;});break;case 254: _LL2765: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmpAD3=_cycalloc(sizeof(struct Cyc_SwitchClauseList_tok_struct));_tmpAD3[0]=({
struct Cyc_SwitchClauseList_tok_struct _tmpAD4;_tmpAD4.tag=Cyc_SwitchClauseList_tok;
_tmpAD4.f1=({struct Cyc_List_List*_tmpAD5=_cycalloc(sizeof(struct Cyc_List_List));
_tmpAD5->hd=({struct Cyc_Absyn_Switch_clause*_tmpAD6=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
_tmpAD6->pattern=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmpAD6->pat_vars=0;_tmpAD6->where_clause=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmpAD6->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmpAD6->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line);_tmpAD6;});_tmpAD5->tl=
Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmpAD5;});_tmpAD4;});_tmpAD3;});
break;case 255: _LL2770: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmpAD8=_cycalloc(sizeof(struct Cyc_SwitchClauseList_tok_struct));_tmpAD8[0]=({
struct Cyc_SwitchClauseList_tok_struct _tmpAD9;_tmpAD9.tag=Cyc_SwitchClauseList_tok;
_tmpAD9.f1=({struct Cyc_List_List*_tmpADA=_cycalloc(sizeof(struct Cyc_List_List));
_tmpADA->hd=({struct Cyc_Absyn_Switch_clause*_tmpADB=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
_tmpADB->pattern=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);_tmpADB->pat_vars=0;_tmpADB->where_clause=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmpADB->body=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmpADB->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line);_tmpADB;});_tmpADA->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmpADA;});_tmpAD9;});_tmpAD8;});break;case 256: _LL2775: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmpADD=_cycalloc(sizeof(struct
Cyc_SwitchCClauseList_tok_struct));_tmpADD[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmpADE;_tmpADE.tag=Cyc_SwitchCClauseList_tok;_tmpADE.f1=0;_tmpADE;});_tmpADD;});
break;case 257: _LL2780: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmpAE0=_cycalloc(sizeof(struct Cyc_SwitchCClauseList_tok_struct));_tmpAE0[0]=({
struct Cyc_SwitchCClauseList_tok_struct _tmpAE1;_tmpAE1.tag=Cyc_SwitchCClauseList_tok;
_tmpAE1.f1=({struct Cyc_List_List*_tmpAE2=_cycalloc(sizeof(struct Cyc_List_List));
_tmpAE2->hd=({struct Cyc_Absyn_SwitchC_clause*_tmpAE3=_cycalloc(sizeof(struct Cyc_Absyn_SwitchC_clause));
_tmpAE3->cnst_exp=0;_tmpAE3->body=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Absyn_break_stmt(0),0);_tmpAE3->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmpAE3;});_tmpAE2->tl=0;_tmpAE2;});_tmpAE1;});_tmpAE0;});
break;case 258: _LL2783: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmpAE5=_cycalloc(sizeof(struct Cyc_SwitchCClauseList_tok_struct));_tmpAE5[0]=({
struct Cyc_SwitchCClauseList_tok_struct _tmpAE6;_tmpAE6.tag=Cyc_SwitchCClauseList_tok;
_tmpAE6.f1=({struct Cyc_List_List*_tmpAE7=_cycalloc(sizeof(struct Cyc_List_List));
_tmpAE7->hd=({struct Cyc_Absyn_SwitchC_clause*_tmpAE8=_cycalloc(sizeof(struct Cyc_Absyn_SwitchC_clause));
_tmpAE8->cnst_exp=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmpAE8->body=Cyc_Absyn_fallthru_stmt(
0,0);_tmpAE8->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmpAE8;});_tmpAE7->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmpAE7;});_tmpAE6;});_tmpAE5;});break;case 259: _LL2788: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmpAEA=_cycalloc(sizeof(struct
Cyc_SwitchCClauseList_tok_struct));_tmpAEA[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmpAEB;_tmpAEB.tag=Cyc_SwitchCClauseList_tok;_tmpAEB.f1=({struct Cyc_List_List*
_tmpAEC=_cycalloc(sizeof(struct Cyc_List_List));_tmpAEC->hd=({struct Cyc_Absyn_SwitchC_clause*
_tmpAED=_cycalloc(sizeof(struct Cyc_Absyn_SwitchC_clause));_tmpAED->cnst_exp=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmpAED->body=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Absyn_fallthru_stmt(
0,0),0);_tmpAED->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmpAED;});_tmpAEC->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmpAEC;});_tmpAEB;});_tmpAEA;});break;case 260: _LL2793: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpAEF=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpAEF[0]=({struct Cyc_Stmt_tok_struct _tmpAF0;_tmpAF0.tag=Cyc_Stmt_tok;_tmpAF0.f1=
Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpAF0;});_tmpAEF;});break;case 261: _LL2798: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpAF2=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpAF2[0]=({struct Cyc_Stmt_tok_struct _tmpAF3;_tmpAF3.tag=Cyc_Stmt_tok;_tmpAF3.f1=
Cyc_Absyn_do_stmt(Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpAF3;});_tmpAF2;});break;case 262: _LL2801: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpAF5=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpAF5[0]=({struct Cyc_Stmt_tok_struct _tmpAF6;_tmpAF6.tag=Cyc_Stmt_tok;_tmpAF6.f1=
Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpAF6;});_tmpAF5;});break;case 263: _LL2804: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpAF8=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpAF8[0]=({struct Cyc_Stmt_tok_struct _tmpAF9;_tmpAF9.tag=Cyc_Stmt_tok;_tmpAF9.f1=
Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpAF9;});_tmpAF8;});break;case 264: _LL2807: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpAFB=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpAFB[0]=({struct Cyc_Stmt_tok_struct _tmpAFC;_tmpAFC.tag=Cyc_Stmt_tok;_tmpAFC.f1=
Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpAFC;});_tmpAFB;});break;case 265: _LL2810: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpAFE=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpAFE[0]=({struct Cyc_Stmt_tok_struct _tmpAFF;_tmpAFF.tag=Cyc_Stmt_tok;_tmpAFF.f1=
Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 7)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmpAFF;});_tmpAFE;});break;case 266: _LL2813: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpB01=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpB01[0]=({struct Cyc_Stmt_tok_struct _tmpB02;_tmpB02.tag=Cyc_Stmt_tok;_tmpB02.f1=
Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpB02;});_tmpB01;});break;case 267: _LL2816: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpB04=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpB04[0]=({struct Cyc_Stmt_tok_struct _tmpB05;_tmpB05.tag=Cyc_Stmt_tok;_tmpB05.f1=
Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmpB05;});
_tmpB04;});break;case 268: _LL2819: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmpB07=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpB07[0]=({struct Cyc_Stmt_tok_struct
_tmpB08;_tmpB08.tag=Cyc_Stmt_tok;_tmpB08.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpB08;});_tmpB07;});break;case 269: _LL2822: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpB0A=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpB0A[0]=({struct Cyc_Stmt_tok_struct _tmpB0B;_tmpB0B.tag=Cyc_Stmt_tok;_tmpB0B.f1=
Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 6)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpB0B;});_tmpB0A;});break;case 270: _LL2825: {struct
Cyc_List_List*_tmpB0D=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*_tmpB0E=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmpB0F=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpB0F[0]=({struct Cyc_Stmt_tok_struct _tmpB10;_tmpB10.tag=Cyc_Stmt_tok;_tmpB10.f1=
Cyc_Parse_flatten_declarations(_tmpB0D,_tmpB0E);_tmpB10;});_tmpB0F;});break;}
case 271: _LL2828: {struct Cyc_List_List*_tmpB12=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmpB13=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmpB14=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpB14[0]=({struct Cyc_Stmt_tok_struct
_tmpB15;_tmpB15.tag=Cyc_Stmt_tok;_tmpB15.f1=Cyc_Parse_flatten_declarations(
_tmpB12,_tmpB13);_tmpB15;});_tmpB14;});break;}case 272: _LL2833: {struct Cyc_List_List*
_tmpB17=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmpB18=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmpB19=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpB19[0]=({struct Cyc_Stmt_tok_struct
_tmpB1A;_tmpB1A.tag=Cyc_Stmt_tok;_tmpB1A.f1=Cyc_Parse_flatten_declarations(
_tmpB17,_tmpB18);_tmpB1A;});_tmpB19;});break;}case 273: _LL2838: {struct Cyc_List_List*
_tmpB1C=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmpB1D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmpB1E=
_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpB1E[0]=({struct Cyc_Stmt_tok_struct
_tmpB1F;_tmpB1F.tag=Cyc_Stmt_tok;_tmpB1F.f1=Cyc_Parse_flatten_declarations(
_tmpB1C,_tmpB1D);_tmpB1F;});_tmpB1E;});break;}case 274: _LL2843: {struct Cyc_List_List*
_tmpB21=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_decl2vardecl,Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmpB22=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpB22[0]=({struct Cyc_Stmt_tok_struct _tmpB23;_tmpB23.tag=Cyc_Stmt_tok;_tmpB23.f1=
Cyc_Absyn_forarray_stmt(_tmpB21,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 8)]).first_line,(yyls[yylsp_offset]).last_line));_tmpB23;});
_tmpB22;});break;}case 275: _LL2848: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmpB25=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpB25[0]=({struct Cyc_Stmt_tok_struct
_tmpB26;_tmpB26.tag=Cyc_Stmt_tok;_tmpB26.f1=Cyc_Absyn_goto_stmt(({struct
_tagged_arr*_tmpB27=_cycalloc(sizeof(struct _tagged_arr));_tmpB27[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmpB27;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmpB26;});
_tmpB25;});break;case 276: _LL2852: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmpB29=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpB29[0]=({struct Cyc_Stmt_tok_struct
_tmpB2A;_tmpB2A.tag=Cyc_Stmt_tok;_tmpB2A.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmpB2A;});
_tmpB29;});break;case 277: _LL2856: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmpB2C=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpB2C[0]=({struct Cyc_Stmt_tok_struct
_tmpB2D;_tmpB2D.tag=Cyc_Stmt_tok;_tmpB2D.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmpB2D;});
_tmpB2C;});break;case 278: _LL2859: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmpB2F=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpB2F[0]=({struct Cyc_Stmt_tok_struct
_tmpB30;_tmpB30.tag=Cyc_Stmt_tok;_tmpB30.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmpB30;});
_tmpB2F;});break;case 279: _LL2862: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmpB32=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpB32[0]=({struct Cyc_Stmt_tok_struct
_tmpB33;_tmpB33.tag=Cyc_Stmt_tok;_tmpB33.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmpB33;});_tmpB32;});break;case 280: _LL2865: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmpB35=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));
_tmpB35[0]=({struct Cyc_Stmt_tok_struct _tmpB36;_tmpB36.tag=Cyc_Stmt_tok;_tmpB36.f1=
Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmpB36;});
_tmpB35;});break;case 281: _LL2868: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmpB38=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpB38[0]=({struct Cyc_Stmt_tok_struct
_tmpB39;_tmpB39.tag=Cyc_Stmt_tok;_tmpB39.f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));_tmpB39;});
_tmpB38;});break;case 282: _LL2871: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmpB3B=_cycalloc(sizeof(struct Cyc_Stmt_tok_struct));_tmpB3B[0]=({struct Cyc_Stmt_tok_struct
_tmpB3C;_tmpB3C.tag=Cyc_Stmt_tok;_tmpB3C.f1=Cyc_Absyn_fallthru_stmt(Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));_tmpB3C;});
_tmpB3B;});break;case 283: _LL2874: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmpB3E=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));_tmpB3E[0]=({struct Cyc_Pattern_tok_struct
_tmpB3F;_tmpB3F.tag=Cyc_Pattern_tok;_tmpB3F.f1=Cyc_Absyn_new_pat((void*)Cyc_Absyn_Wild_p,
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmpB3F;});_tmpB3E;});break;case 284: _LL2877: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 285: _LL2880:
yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmpB42=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));
_tmpB42[0]=({struct Cyc_Pattern_tok_struct _tmpB43;_tmpB43.tag=Cyc_Pattern_tok;
_tmpB43.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmpB44=
_cycalloc(sizeof(struct Cyc_Absyn_Int_p_struct));_tmpB44[0]=({struct Cyc_Absyn_Int_p_struct
_tmpB45;_tmpB45.tag=Cyc_Absyn_Int_p;_tmpB45.f1=(void*)(*Cyc_yyget_Int_tok(yyvs[
yyvsp_offset])).f1;_tmpB45.f2=(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2;
_tmpB45;});_tmpB44;}),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmpB43;});_tmpB42;});break;case 286: _LL2881:
yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmpB47=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));
_tmpB47[0]=({struct Cyc_Pattern_tok_struct _tmpB48;_tmpB48.tag=Cyc_Pattern_tok;
_tmpB48.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmpB49=
_cycalloc(sizeof(struct Cyc_Absyn_Int_p_struct));_tmpB49[0]=({struct Cyc_Absyn_Int_p_struct
_tmpB4A;_tmpB4A.tag=Cyc_Absyn_Int_p;_tmpB4A.f1=(void*)((void*)Cyc_Absyn_Signed);
_tmpB4A.f2=-(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2;_tmpB4A;});_tmpB49;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmpB48;});
_tmpB47;});break;case 287: _LL2886: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmpB4C=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));_tmpB4C[0]=({struct Cyc_Pattern_tok_struct
_tmpB4D;_tmpB4D.tag=Cyc_Pattern_tok;_tmpB4D.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Float_p_struct*_tmpB4E=_cycalloc(sizeof(struct Cyc_Absyn_Float_p_struct));
_tmpB4E[0]=({struct Cyc_Absyn_Float_p_struct _tmpB4F;_tmpB4F.tag=Cyc_Absyn_Float_p;
_tmpB4F.f1=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmpB4F;});_tmpB4E;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmpB4D;});
_tmpB4C;});break;case 288: _LL2891: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmpB51=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));_tmpB51[0]=({struct Cyc_Pattern_tok_struct
_tmpB52;_tmpB52.tag=Cyc_Pattern_tok;_tmpB52.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Char_p_struct*_tmpB53=_cycalloc_atomic(sizeof(struct Cyc_Absyn_Char_p_struct));
_tmpB53[0]=({struct Cyc_Absyn_Char_p_struct _tmpB54;_tmpB54.tag=Cyc_Absyn_Char_p;
_tmpB54.f1=Cyc_yyget_Char_tok(yyvs[yyvsp_offset]);_tmpB54;});_tmpB53;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmpB52;});
_tmpB51;});break;case 289: _LL2896: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmpB56=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));_tmpB56[0]=({struct Cyc_Pattern_tok_struct
_tmpB57;_tmpB57.tag=Cyc_Pattern_tok;_tmpB57.f1=Cyc_Absyn_new_pat((void*)Cyc_Absyn_Null_p,
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmpB57;});_tmpB56;});break;case 290: _LL2901: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmpB59=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));_tmpB59[0]=({struct Cyc_Pattern_tok_struct
_tmpB5A;_tmpB5A.tag=Cyc_Pattern_tok;_tmpB5A.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownId_p_struct*_tmpB5B=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_p_struct));
_tmpB5B[0]=({struct Cyc_Absyn_UnknownId_p_struct _tmpB5C;_tmpB5C.tag=Cyc_Absyn_UnknownId_p;
_tmpB5C.f1=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);_tmpB5C;});_tmpB5B;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmpB5A;});
_tmpB59;});break;case 291: _LL2904: {struct Cyc_List_List*_tmpB5E=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmpB5F=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));
_tmpB5F[0]=({struct Cyc_Pattern_tok_struct _tmpB60;_tmpB60.tag=Cyc_Pattern_tok;
_tmpB60.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownCall_p_struct*
_tmpB61=_cycalloc(sizeof(struct Cyc_Absyn_UnknownCall_p_struct));_tmpB61[0]=({
struct Cyc_Absyn_UnknownCall_p_struct _tmpB62;_tmpB62.tag=Cyc_Absyn_UnknownCall_p;
_tmpB62.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmpB62.f2=_tmpB5E;_tmpB62.f3=Cyc_yyget_PatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmpB62;});_tmpB61;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmpB60;});
_tmpB5F;});break;}case 292: _LL2909: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmpB64=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));_tmpB64[0]=({struct Cyc_Pattern_tok_struct
_tmpB65;_tmpB65.tag=Cyc_Pattern_tok;_tmpB65.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Tuple_p_struct*_tmpB66=_cycalloc(sizeof(struct Cyc_Absyn_Tuple_p_struct));
_tmpB66[0]=({struct Cyc_Absyn_Tuple_p_struct _tmpB67;_tmpB67.tag=Cyc_Absyn_Tuple_p;
_tmpB67.f1=Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmpB67;});_tmpB66;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpB65;});_tmpB64;});break;case 293: _LL2915: {struct
Cyc_List_List*_tmpB69=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmpB6A=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));
_tmpB6A[0]=({struct Cyc_Pattern_tok_struct _tmpB6B;_tmpB6B.tag=Cyc_Pattern_tok;
_tmpB6B.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownFields_p_struct*
_tmpB6C=_cycalloc(sizeof(struct Cyc_Absyn_UnknownFields_p_struct));_tmpB6C[0]=({
struct Cyc_Absyn_UnknownFields_p_struct _tmpB6D;_tmpB6D.tag=Cyc_Absyn_UnknownFields_p;
_tmpB6D.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmpB6D.f2=_tmpB69;_tmpB6D.f3=0;_tmpB6D;});_tmpB6C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpB6B;});_tmpB6A;});break;}case 294: _LL2920: {struct
Cyc_List_List*_tmpB6F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmpB70=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));
_tmpB70[0]=({struct Cyc_Pattern_tok_struct _tmpB71;_tmpB71.tag=Cyc_Pattern_tok;
_tmpB71.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownFields_p_struct*
_tmpB72=_cycalloc(sizeof(struct Cyc_Absyn_UnknownFields_p_struct));_tmpB72[0]=({
struct Cyc_Absyn_UnknownFields_p_struct _tmpB73;_tmpB73.tag=Cyc_Absyn_UnknownFields_p;
_tmpB73.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmpB73.f2=_tmpB6F;_tmpB73.f3=Cyc_yyget_FieldPatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmpB73;});_tmpB72;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmpB71;});
_tmpB70;});break;}case 295: _LL2926: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmpB75=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));_tmpB75[0]=({struct Cyc_Pattern_tok_struct
_tmpB76;_tmpB76.tag=Cyc_Pattern_tok;_tmpB76.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Pointer_p_struct*_tmpB77=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_struct));
_tmpB77[0]=({struct Cyc_Absyn_Pointer_p_struct _tmpB78;_tmpB78.tag=Cyc_Absyn_Pointer_p;
_tmpB78.f1=Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmpB78;});_tmpB77;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpB76;});_tmpB75;});break;case 296: _LL2932: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmpB7A=_cycalloc(sizeof(struct Cyc_Pattern_tok_struct));
_tmpB7A[0]=({struct Cyc_Pattern_tok_struct _tmpB7B;_tmpB7B.tag=Cyc_Pattern_tok;
_tmpB7B.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmpB7C=
_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_struct));_tmpB7C[0]=({struct Cyc_Absyn_Reference_p_struct
_tmpB7D;_tmpB7D.tag=Cyc_Absyn_Reference_p;_tmpB7D.f1=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmpB7E=_cycalloc(sizeof(struct _tuple1));_tmpB7E->f1=(void*)Cyc_Absyn_Loc_n;
_tmpB7E->f2=({struct _tagged_arr*_tmpB7F=_cycalloc(sizeof(struct _tagged_arr));
_tmpB7F[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmpB7F;});_tmpB7E;}),(void*)
Cyc_Absyn_VoidType,0);_tmpB7D;});_tmpB7C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpB7B;});_tmpB7A;});break;case 297: _LL2937: yyval=(
void*)({struct Cyc_PatternList_tok_struct*_tmpB81=_cycalloc(sizeof(struct Cyc_PatternList_tok_struct));
_tmpB81[0]=({struct Cyc_PatternList_tok_struct _tmpB82;_tmpB82.tag=Cyc_PatternList_tok;
_tmpB82.f1=0;_tmpB82;});_tmpB81;});break;case 298: _LL2944: yyval=(void*)({struct
Cyc_PatternList_tok_struct*_tmpB84=_cycalloc(sizeof(struct Cyc_PatternList_tok_struct));
_tmpB84[0]=({struct Cyc_PatternList_tok_struct _tmpB85;_tmpB85.tag=Cyc_PatternList_tok;
_tmpB85.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(yyvs[yyvsp_offset]));_tmpB85;});_tmpB84;});break;case
299: _LL2947: yyval=(void*)({struct Cyc_PatternList_tok_struct*_tmpB87=_cycalloc(
sizeof(struct Cyc_PatternList_tok_struct));_tmpB87[0]=({struct Cyc_PatternList_tok_struct
_tmpB88;_tmpB88.tag=Cyc_PatternList_tok;_tmpB88.f1=({struct Cyc_List_List*_tmpB89=
_cycalloc(sizeof(struct Cyc_List_List));_tmpB89->hd=Cyc_yyget_Pattern_tok(yyvs[
yyvsp_offset]);_tmpB89->tl=0;_tmpB89;});_tmpB88;});_tmpB87;});break;case 300:
_LL2950: yyval=(void*)({struct Cyc_PatternList_tok_struct*_tmpB8B=_cycalloc(
sizeof(struct Cyc_PatternList_tok_struct));_tmpB8B[0]=({struct Cyc_PatternList_tok_struct
_tmpB8C;_tmpB8C.tag=Cyc_PatternList_tok;_tmpB8C.f1=({struct Cyc_List_List*_tmpB8D=
_cycalloc(sizeof(struct Cyc_List_List));_tmpB8D->hd=Cyc_yyget_Pattern_tok(yyvs[
yyvsp_offset]);_tmpB8D->tl=Cyc_yyget_PatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmpB8D;});_tmpB8C;});
_tmpB8B;});break;case 301: _LL2954: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmpB8F=_cycalloc(sizeof(struct Cyc_FieldPattern_tok_struct));_tmpB8F[0]=({struct
Cyc_FieldPattern_tok_struct _tmpB90;_tmpB90.tag=Cyc_FieldPattern_tok;_tmpB90.f1=({
struct _tuple13*_tmpB91=_cycalloc(sizeof(struct _tuple13));_tmpB91->f1=0;_tmpB91->f2=
Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmpB91;});_tmpB90;});_tmpB8F;});
break;case 302: _LL2958: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*_tmpB93=
_cycalloc(sizeof(struct Cyc_FieldPattern_tok_struct));_tmpB93[0]=({struct Cyc_FieldPattern_tok_struct
_tmpB94;_tmpB94.tag=Cyc_FieldPattern_tok;_tmpB94.f1=({struct _tuple13*_tmpB95=
_cycalloc(sizeof(struct _tuple13));_tmpB95->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmpB95->f2=Cyc_yyget_Pattern_tok(
yyvs[yyvsp_offset]);_tmpB95;});_tmpB94;});_tmpB93;});break;case 303: _LL2962: yyval=(
void*)({struct Cyc_FieldPatternList_tok_struct*_tmpB97=_cycalloc(sizeof(struct Cyc_FieldPatternList_tok_struct));
_tmpB97[0]=({struct Cyc_FieldPatternList_tok_struct _tmpB98;_tmpB98.tag=Cyc_FieldPatternList_tok;
_tmpB98.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_FieldPatternList_tok(yyvs[yyvsp_offset]));_tmpB98;});_tmpB97;});break;
case 304: _LL2966: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*_tmpB9A=
_cycalloc(sizeof(struct Cyc_FieldPatternList_tok_struct));_tmpB9A[0]=({struct Cyc_FieldPatternList_tok_struct
_tmpB9B;_tmpB9B.tag=Cyc_FieldPatternList_tok;_tmpB9B.f1=({struct Cyc_List_List*
_tmpB9C=_cycalloc(sizeof(struct Cyc_List_List));_tmpB9C->hd=Cyc_yyget_FieldPattern_tok(
yyvs[yyvsp_offset]);_tmpB9C->tl=0;_tmpB9C;});_tmpB9B;});_tmpB9A;});break;case 305:
_LL2969: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*_tmpB9E=_cycalloc(
sizeof(struct Cyc_FieldPatternList_tok_struct));_tmpB9E[0]=({struct Cyc_FieldPatternList_tok_struct
_tmpB9F;_tmpB9F.tag=Cyc_FieldPatternList_tok;_tmpB9F.f1=({struct Cyc_List_List*
_tmpBA0=_cycalloc(sizeof(struct Cyc_List_List));_tmpBA0->hd=Cyc_yyget_FieldPattern_tok(
yyvs[yyvsp_offset]);_tmpBA0->tl=Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmpBA0;});_tmpB9F;});
_tmpB9E;});break;case 306: _LL2973: yyval=yyvs[yyvsp_offset];break;case 307: _LL2977:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpBA3=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpBA3[0]=({struct Cyc_Exp_tok_struct _tmpBA4;_tmpBA4.tag=Cyc_Exp_tok;_tmpBA4.f1=
Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpBA4;});_tmpBA3;});break;case 308: _LL2978: yyval=
yyvs[yyvsp_offset];break;case 309: _LL2981: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpBA7=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpBA7[0]=({struct Cyc_Exp_tok_struct
_tmpBA8;_tmpBA8.tag=Cyc_Exp_tok;_tmpBA8.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpBA8;});_tmpBA7;});break;case 310: _LL2982: yyval=(
void*)({struct Cyc_Primopopt_tok_struct*_tmpBAA=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));
_tmpBAA[0]=({struct Cyc_Primopopt_tok_struct _tmpBAB;_tmpBAB.tag=Cyc_Primopopt_tok;
_tmpBAB.f1=0;_tmpBAB;});_tmpBAA;});break;case 311: _LL2985: yyval=(void*)({struct
Cyc_Primopopt_tok_struct*_tmpBAD=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));
_tmpBAD[0]=({struct Cyc_Primopopt_tok_struct _tmpBAE;_tmpBAE.tag=Cyc_Primopopt_tok;
_tmpBAE.f1=({struct Cyc_Core_Opt*_tmpBAF=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmpBAF->v=(void*)((void*)Cyc_Absyn_Times);_tmpBAF;});_tmpBAE;});_tmpBAD;});
break;case 312: _LL2988: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmpBB1=
_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmpBB1[0]=({struct Cyc_Primopopt_tok_struct
_tmpBB2;_tmpBB2.tag=Cyc_Primopopt_tok;_tmpBB2.f1=({struct Cyc_Core_Opt*_tmpBB3=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpBB3->v=(void*)((void*)Cyc_Absyn_Div);
_tmpBB3;});_tmpBB2;});_tmpBB1;});break;case 313: _LL2992: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmpBB5=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmpBB5[0]=({struct Cyc_Primopopt_tok_struct
_tmpBB6;_tmpBB6.tag=Cyc_Primopopt_tok;_tmpBB6.f1=({struct Cyc_Core_Opt*_tmpBB7=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpBB7->v=(void*)((void*)Cyc_Absyn_Mod);
_tmpBB7;});_tmpBB6;});_tmpBB5;});break;case 314: _LL2996: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmpBB9=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmpBB9[0]=({struct Cyc_Primopopt_tok_struct
_tmpBBA;_tmpBBA.tag=Cyc_Primopopt_tok;_tmpBBA.f1=({struct Cyc_Core_Opt*_tmpBBB=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpBBB->v=(void*)((void*)Cyc_Absyn_Plus);
_tmpBBB;});_tmpBBA;});_tmpBB9;});break;case 315: _LL3000: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmpBBD=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmpBBD[0]=({struct Cyc_Primopopt_tok_struct
_tmpBBE;_tmpBBE.tag=Cyc_Primopopt_tok;_tmpBBE.f1=({struct Cyc_Core_Opt*_tmpBBF=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpBBF->v=(void*)((void*)Cyc_Absyn_Minus);
_tmpBBF;});_tmpBBE;});_tmpBBD;});break;case 316: _LL3004: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmpBC1=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmpBC1[0]=({struct Cyc_Primopopt_tok_struct
_tmpBC2;_tmpBC2.tag=Cyc_Primopopt_tok;_tmpBC2.f1=({struct Cyc_Core_Opt*_tmpBC3=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpBC3->v=(void*)((void*)Cyc_Absyn_Bitlshift);
_tmpBC3;});_tmpBC2;});_tmpBC1;});break;case 317: _LL3008: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmpBC5=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmpBC5[0]=({struct Cyc_Primopopt_tok_struct
_tmpBC6;_tmpBC6.tag=Cyc_Primopopt_tok;_tmpBC6.f1=({struct Cyc_Core_Opt*_tmpBC7=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpBC7->v=(void*)((void*)Cyc_Absyn_Bitlrshift);
_tmpBC7;});_tmpBC6;});_tmpBC5;});break;case 318: _LL3012: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmpBC9=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmpBC9[0]=({struct Cyc_Primopopt_tok_struct
_tmpBCA;_tmpBCA.tag=Cyc_Primopopt_tok;_tmpBCA.f1=({struct Cyc_Core_Opt*_tmpBCB=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpBCB->v=(void*)((void*)Cyc_Absyn_Bitand);
_tmpBCB;});_tmpBCA;});_tmpBC9;});break;case 319: _LL3016: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmpBCD=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmpBCD[0]=({struct Cyc_Primopopt_tok_struct
_tmpBCE;_tmpBCE.tag=Cyc_Primopopt_tok;_tmpBCE.f1=({struct Cyc_Core_Opt*_tmpBCF=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpBCF->v=(void*)((void*)Cyc_Absyn_Bitxor);
_tmpBCF;});_tmpBCE;});_tmpBCD;});break;case 320: _LL3020: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmpBD1=_cycalloc(sizeof(struct Cyc_Primopopt_tok_struct));_tmpBD1[0]=({struct Cyc_Primopopt_tok_struct
_tmpBD2;_tmpBD2.tag=Cyc_Primopopt_tok;_tmpBD2.f1=({struct Cyc_Core_Opt*_tmpBD3=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpBD3->v=(void*)((void*)Cyc_Absyn_Bitor);
_tmpBD3;});_tmpBD2;});_tmpBD1;});break;case 321: _LL3024: yyval=yyvs[yyvsp_offset];
break;case 322: _LL3028: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpBD6=_cycalloc(
sizeof(struct Cyc_Exp_tok_struct));_tmpBD6[0]=({struct Cyc_Exp_tok_struct _tmpBD7;
_tmpBD7.tag=Cyc_Exp_tok;_tmpBD7.f1=Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpBD7;});_tmpBD6;});break;case 323: _LL3029: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpBD9=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpBD9[0]=({struct Cyc_Exp_tok_struct _tmpBDA;_tmpBDA.tag=Cyc_Exp_tok;_tmpBDA.f1=
Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpBDA;});_tmpBD9;});break;case 324: _LL3032: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpBDC=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpBDC[0]=({struct Cyc_Exp_tok_struct _tmpBDD;_tmpBDD.tag=Cyc_Exp_tok;_tmpBDD.f1=
Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line));_tmpBDD;});
_tmpBDC;});break;case 325: _LL3035: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpBDF=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpBDF[0]=({struct Cyc_Exp_tok_struct
_tmpBE0;_tmpBE0.tag=Cyc_Exp_tok;_tmpBE0.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line));_tmpBE0;});
_tmpBDF;});break;case 326: _LL3038: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpBE2=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpBE2[0]=({struct Cyc_Exp_tok_struct
_tmpBE3;_tmpBE3.tag=Cyc_Exp_tok;_tmpBE3.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpBE3;});_tmpBE2;});break;case 327: _LL3041: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpBE5=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpBE5[0]=({struct Cyc_Exp_tok_struct _tmpBE6;_tmpBE6.tag=Cyc_Exp_tok;_tmpBE6.f1=
Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmpBE6;});
_tmpBE5;});break;case 328: _LL3044: yyval=yyvs[yyvsp_offset];break;case 329: _LL3047:
yyval=yyvs[yyvsp_offset];break;case 330: _LL3048: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpBEA=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpBEA[0]=({struct Cyc_Exp_tok_struct
_tmpBEB;_tmpBEB.tag=Cyc_Exp_tok;_tmpBEB.f1=Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpBEB;});_tmpBEA;});break;case 331: _LL3049: yyval=
yyvs[yyvsp_offset];break;case 332: _LL3052: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpBEE=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpBEE[0]=({struct Cyc_Exp_tok_struct
_tmpBEF;_tmpBEF.tag=Cyc_Exp_tok;_tmpBEF.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpBEF;});_tmpBEE;});break;case 333: _LL3053: yyval=
yyvs[yyvsp_offset];break;case 334: _LL3056: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpBF2=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpBF2[0]=({struct Cyc_Exp_tok_struct
_tmpBF3;_tmpBF3.tag=Cyc_Exp_tok;_tmpBF3.f1=Cyc_Absyn_prim2_exp((void*)Cyc_Absyn_Bitor,
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpBF3;});_tmpBF2;});break;case 335: _LL3057: yyval=
yyvs[yyvsp_offset];break;case 336: _LL3060: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpBF6=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpBF6[0]=({struct Cyc_Exp_tok_struct
_tmpBF7;_tmpBF7.tag=Cyc_Exp_tok;_tmpBF7.f1=Cyc_Absyn_prim2_exp((void*)Cyc_Absyn_Bitxor,
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpBF7;});_tmpBF6;});break;case 337: _LL3061: yyval=
yyvs[yyvsp_offset];break;case 338: _LL3064: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpBFA=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpBFA[0]=({struct Cyc_Exp_tok_struct
_tmpBFB;_tmpBFB.tag=Cyc_Exp_tok;_tmpBFB.f1=Cyc_Absyn_prim2_exp((void*)Cyc_Absyn_Bitand,
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpBFB;});_tmpBFA;});break;case 339: _LL3065: yyval=
yyvs[yyvsp_offset];break;case 340: _LL3068: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpBFE=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpBFE[0]=({struct Cyc_Exp_tok_struct
_tmpBFF;_tmpBFF.tag=Cyc_Exp_tok;_tmpBFF.f1=Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpBFF;});_tmpBFE;});break;case 341: _LL3069: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC01=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC01[0]=({struct Cyc_Exp_tok_struct _tmpC02;_tmpC02.tag=Cyc_Exp_tok;_tmpC02.f1=
Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC02;});_tmpC01;});break;case 342: _LL3072: yyval=
yyvs[yyvsp_offset];break;case 343: _LL3075: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpC05=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpC05[0]=({struct Cyc_Exp_tok_struct
_tmpC06;_tmpC06.tag=Cyc_Exp_tok;_tmpC06.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC06;});_tmpC05;});break;case 344: _LL3076: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC08=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC08[0]=({struct Cyc_Exp_tok_struct _tmpC09;_tmpC09.tag=Cyc_Exp_tok;_tmpC09.f1=
Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC09;});_tmpC08;});break;case 345: _LL3079: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC0B=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC0B[0]=({struct Cyc_Exp_tok_struct _tmpC0C;_tmpC0C.tag=Cyc_Exp_tok;_tmpC0C.f1=
Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC0C;});_tmpC0B;});break;case 346: _LL3082: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC0E=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC0E[0]=({struct Cyc_Exp_tok_struct _tmpC0F;_tmpC0F.tag=Cyc_Exp_tok;_tmpC0F.f1=
Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC0F;});_tmpC0E;});break;case 347: _LL3085: yyval=
yyvs[yyvsp_offset];break;case 348: _LL3088: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpC12=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpC12[0]=({struct Cyc_Exp_tok_struct
_tmpC13;_tmpC13.tag=Cyc_Exp_tok;_tmpC13.f1=Cyc_Absyn_prim2_exp((void*)Cyc_Absyn_Bitlshift,
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC13;});_tmpC12;});break;case 349: _LL3089: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC15=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC15[0]=({struct Cyc_Exp_tok_struct _tmpC16;_tmpC16.tag=Cyc_Exp_tok;_tmpC16.f1=
Cyc_Absyn_prim2_exp((void*)Cyc_Absyn_Bitlrshift,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmpC16;});
_tmpC15;});break;case 350: _LL3092: yyval=yyvs[yyvsp_offset];break;case 351: _LL3095:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpC19=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC19[0]=({struct Cyc_Exp_tok_struct _tmpC1A;_tmpC1A.tag=Cyc_Exp_tok;_tmpC1A.f1=
Cyc_Absyn_prim2_exp((void*)Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmpC1A;});
_tmpC19;});break;case 352: _LL3096: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpC1C=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpC1C[0]=({struct Cyc_Exp_tok_struct
_tmpC1D;_tmpC1D.tag=Cyc_Exp_tok;_tmpC1D.f1=Cyc_Absyn_prim2_exp((void*)Cyc_Absyn_Minus,
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC1D;});_tmpC1C;});break;case 353: _LL3099: yyval=
yyvs[yyvsp_offset];break;case 354: _LL3102: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpC20=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpC20[0]=({struct Cyc_Exp_tok_struct
_tmpC21;_tmpC21.tag=Cyc_Exp_tok;_tmpC21.f1=Cyc_Absyn_prim2_exp((void*)Cyc_Absyn_Times,
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC21;});_tmpC20;});break;case 355: _LL3103: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC23=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC23[0]=({struct Cyc_Exp_tok_struct _tmpC24;_tmpC24.tag=Cyc_Exp_tok;_tmpC24.f1=
Cyc_Absyn_prim2_exp((void*)Cyc_Absyn_Div,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmpC24;});
_tmpC23;});break;case 356: _LL3106: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpC26=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpC26[0]=({struct Cyc_Exp_tok_struct
_tmpC27;_tmpC27.tag=Cyc_Exp_tok;_tmpC27.f1=Cyc_Absyn_prim2_exp((void*)Cyc_Absyn_Mod,
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC27;});_tmpC26;});break;case 357: _LL3109: yyval=
yyvs[yyvsp_offset];break;case 358: _LL3112: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpC2A=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpC2A[0]=({struct Cyc_Exp_tok_struct
_tmpC2B;_tmpC2B.tag=Cyc_Exp_tok;_tmpC2B.f1=Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC2B;});_tmpC2A;});break;case 359: _LL3113: yyval=
yyvs[yyvsp_offset];break;case 360: _LL3116: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpC2E=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpC2E[0]=({struct Cyc_Exp_tok_struct
_tmpC2F;_tmpC2F.tag=Cyc_Exp_tok;_tmpC2F.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC2F;});_tmpC2E;});break;case 361: _LL3117: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC31=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC31[0]=({struct Cyc_Exp_tok_struct _tmpC32;_tmpC32.tag=Cyc_Exp_tok;_tmpC32.f1=
Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC32;});_tmpC31;});break;case 362: _LL3120: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC34=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC34[0]=({struct Cyc_Exp_tok_struct _tmpC35;_tmpC35.tag=Cyc_Exp_tok;_tmpC35.f1=
Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC35;});_tmpC34;});break;case 363: _LL3123: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC37=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC37[0]=({struct Cyc_Exp_tok_struct _tmpC38;_tmpC38.tag=Cyc_Exp_tok;_tmpC38.f1=
Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC38;});_tmpC37;});break;case 364: _LL3126: yyval=
yyvs[yyvsp_offset];break;case 365: _LL3129: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpC3B=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpC3B[0]=({struct Cyc_Exp_tok_struct
_tmpC3C;_tmpC3C.tag=Cyc_Exp_tok;_tmpC3C.f1=Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC3C;});_tmpC3B;});break;case 366: _LL3130: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC3E=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC3E[0]=({struct Cyc_Exp_tok_struct _tmpC3F;_tmpC3F.tag=Cyc_Exp_tok;_tmpC3F.f1=
Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC3F;});_tmpC3E;});break;case 367: _LL3133: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC41=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC41[0]=({struct Cyc_Exp_tok_struct _tmpC42;_tmpC42.tag=Cyc_Exp_tok;_tmpC42.f1=
Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC42;});_tmpC41;});break;case 368: _LL3136: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC44=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC44[0]=({struct Cyc_Exp_tok_struct _tmpC45;_tmpC45.tag=Cyc_Exp_tok;_tmpC45.f1=
Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmpC46=_cycalloc(sizeof(struct Cyc_Absyn_StructField_struct));_tmpC46[0]=({
struct Cyc_Absyn_StructField_struct _tmpC47;_tmpC47.tag=Cyc_Absyn_StructField;
_tmpC47.f1=({struct _tagged_arr*_tmpC48=_cycalloc(sizeof(struct _tagged_arr));
_tmpC48[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmpC48;});_tmpC47;});_tmpC46;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC45;});_tmpC44;});break;case 369: _LL3139: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC4A=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC4A[0]=({struct Cyc_Exp_tok_struct _tmpC4B;_tmpC4B.tag=Cyc_Exp_tok;_tmpC4B.f1=
Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*
_tmpC4C=_cycalloc_atomic(sizeof(struct Cyc_Absyn_TupleIndex_struct));_tmpC4C[0]=({
struct Cyc_Absyn_TupleIndex_struct _tmpC4D;_tmpC4D.tag=Cyc_Absyn_TupleIndex;
_tmpC4D.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2;_tmpC4D;});_tmpC4C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC4B;});_tmpC4A;});break;case 370: _LL3145: {struct
Cyc_Position_Segment*_tmpC4F=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);struct Cyc_List_List*_tmpC50=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpC4F,Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_Exp_tok_struct*_tmpC51=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC51[0]=({struct Cyc_Exp_tok_struct _tmpC52;_tmpC52.tag=Cyc_Exp_tok;_tmpC52.f1=
Cyc_Absyn_gentyp_exp(_tmpC50,(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC52;});_tmpC51;});break;}case 371: _LL3150: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC54=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC54[0]=({struct Cyc_Exp_tok_struct _tmpC55;_tmpC55.tag=Cyc_Exp_tok;_tmpC55.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmpC56=_cycalloc(
sizeof(struct Cyc_Absyn_Malloc_e_struct));_tmpC56[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmpC57;_tmpC57.tag=Cyc_Absyn_Malloc_e;_tmpC57.f1=({struct Cyc_Absyn_MallocInfo
_tmpC58;_tmpC58.is_calloc=0;_tmpC58.rgn=0;_tmpC58.elt_type=0;_tmpC58.num_elts=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmpC58.fat_result=0;_tmpC58;});_tmpC57;});_tmpC56;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC55;});_tmpC54;});break;case 372: _LL3155: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC5A=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC5A[0]=({struct Cyc_Exp_tok_struct _tmpC5B;_tmpC5B.tag=Cyc_Exp_tok;_tmpC5B.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmpC5C=_cycalloc(
sizeof(struct Cyc_Absyn_Malloc_e_struct));_tmpC5C[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmpC5D;_tmpC5D.tag=Cyc_Absyn_Malloc_e;_tmpC5D.f1=({struct Cyc_Absyn_MallocInfo
_tmpC5E;_tmpC5E.is_calloc=0;_tmpC5E.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmpC5E.elt_type=0;
_tmpC5E.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmpC5E.fat_result=0;_tmpC5E;});_tmpC5D;});_tmpC5C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC5B;});_tmpC5A;});break;case 373: _LL3161: {void*
_tmpC62;struct _tuple2 _tmpC60=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_LL3171: _tmpC62=_tmpC60.f3;
goto _LL3169;_LL3169: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpC64=_cycalloc(
sizeof(struct Cyc_Exp_tok_struct));_tmpC64[0]=({struct Cyc_Exp_tok_struct _tmpC65;
_tmpC65.tag=Cyc_Exp_tok;_tmpC65.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmpC66=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_struct));_tmpC66[0]=({struct
Cyc_Absyn_Malloc_e_struct _tmpC67;_tmpC67.tag=Cyc_Absyn_Malloc_e;_tmpC67.f1=({
struct Cyc_Absyn_MallocInfo _tmpC68;_tmpC68.is_calloc=1;_tmpC68.rgn=0;_tmpC68.elt_type=({
void**_tmpC69=_cycalloc(sizeof(void*));_tmpC69[0]=_tmpC62;_tmpC69;});_tmpC68.num_elts=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmpC68.fat_result=0;_tmpC68;});_tmpC67;});_tmpC66;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC65;});_tmpC64;});break;}case 374: _LL3167: {void*
_tmpC6D;struct _tuple2 _tmpC6B=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_LL3182: _tmpC6D=_tmpC6B.f3;
goto _LL3180;_LL3180: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpC6F=_cycalloc(
sizeof(struct Cyc_Exp_tok_struct));_tmpC6F[0]=({struct Cyc_Exp_tok_struct _tmpC70;
_tmpC70.tag=Cyc_Exp_tok;_tmpC70.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmpC71=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_struct));_tmpC71[0]=({struct
Cyc_Absyn_Malloc_e_struct _tmpC72;_tmpC72.tag=Cyc_Absyn_Malloc_e;_tmpC72.f1=({
struct Cyc_Absyn_MallocInfo _tmpC73;_tmpC73.is_calloc=1;_tmpC73.rgn=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);
_tmpC73.elt_type=({void**_tmpC74=_cycalloc(sizeof(void*));_tmpC74[0]=_tmpC6D;
_tmpC74;});_tmpC73.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmpC73.fat_result=0;
_tmpC73;});_tmpC72;});_tmpC71;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC70;});_tmpC6F;});break;}case 375: _LL3178: yyval=(
void*)({struct Cyc_Primop_tok_struct*_tmpC76=_cycalloc(sizeof(struct Cyc_Primop_tok_struct));
_tmpC76[0]=({struct Cyc_Primop_tok_struct _tmpC77;_tmpC77.tag=Cyc_Primop_tok;
_tmpC77.f1=(void*)((void*)Cyc_Absyn_Bitnot);_tmpC77;});_tmpC76;});break;case 376:
_LL3189: yyval=(void*)({struct Cyc_Primop_tok_struct*_tmpC79=_cycalloc(sizeof(
struct Cyc_Primop_tok_struct));_tmpC79[0]=({struct Cyc_Primop_tok_struct _tmpC7A;
_tmpC7A.tag=Cyc_Primop_tok;_tmpC7A.f1=(void*)((void*)Cyc_Absyn_Not);_tmpC7A;});
_tmpC79;});break;case 377: _LL3192: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmpC7C=_cycalloc(sizeof(struct Cyc_Primop_tok_struct));_tmpC7C[0]=({struct Cyc_Primop_tok_struct
_tmpC7D;_tmpC7D.tag=Cyc_Primop_tok;_tmpC7D.f1=(void*)((void*)Cyc_Absyn_Minus);
_tmpC7D;});_tmpC7C;});break;case 378: _LL3195: yyval=yyvs[yyvsp_offset];break;case
379: _LL3198: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpC80=_cycalloc(sizeof(
struct Cyc_Exp_tok_struct));_tmpC80[0]=({struct Cyc_Exp_tok_struct _tmpC81;_tmpC81.tag=
Cyc_Exp_tok;_tmpC81.f1=Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC81;});_tmpC80;});break;case 380: _LL3199: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC83=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC83[0]=({struct Cyc_Exp_tok_struct _tmpC84;_tmpC84.tag=Cyc_Exp_tok;_tmpC84.f1=
Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC84;});_tmpC83;});break;case 381: _LL3202: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC86=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC86[0]=({struct Cyc_Exp_tok_struct _tmpC87;_tmpC87.tag=Cyc_Exp_tok;_tmpC87.f1=
Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC87;});_tmpC86;});break;case 382: _LL3205: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC89=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC89[0]=({struct Cyc_Exp_tok_struct _tmpC8A;_tmpC8A.tag=Cyc_Exp_tok;_tmpC8A.f1=
Cyc_Absyn_structmember_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),({struct _tagged_arr*_tmpC8B=_cycalloc(sizeof(struct
_tagged_arr));_tmpC8B[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmpC8B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC8A;});_tmpC89;});break;case 383: _LL3208: {struct
_tuple1*q=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);if(Cyc_Absyn_is_qvar_qualified(
q)){Cyc_Parse_err(_tag_arr("struct field name is qualified",sizeof(unsigned char),
31),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpC8D=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpC8D[0]=({struct Cyc_Exp_tok_struct
_tmpC8E;_tmpC8E.tag=Cyc_Exp_tok;_tmpC8E.f1=Cyc_Absyn_structmember_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(*q).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC8E;});_tmpC8D;});break;}case 384: _LL3212: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC90=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC90[0]=({struct Cyc_Exp_tok_struct _tmpC91;_tmpC91.tag=Cyc_Exp_tok;_tmpC91.f1=
Cyc_Absyn_structarrow_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),({struct _tagged_arr*_tmpC92=_cycalloc(sizeof(struct
_tagged_arr));_tmpC92[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmpC92;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC91;});_tmpC90;});break;case 385: _LL3215: {struct
_tuple1*q=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);if(Cyc_Absyn_is_qvar_qualified(
q)){Cyc_Parse_err(_tag_arr("struct field name is qualified",sizeof(unsigned char),
31),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpC94=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpC94[0]=({struct Cyc_Exp_tok_struct
_tmpC95;_tmpC95.tag=Cyc_Exp_tok;_tmpC95.f1=Cyc_Absyn_structarrow_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(*q).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC95;});_tmpC94;});break;}case 386: _LL3219: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC97=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC97[0]=({struct Cyc_Exp_tok_struct _tmpC98;_tmpC98.tag=Cyc_Exp_tok;_tmpC98.f1=
Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC98;});_tmpC97;});break;case 387: _LL3222: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC9A=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC9A[0]=({struct Cyc_Exp_tok_struct _tmpC9B;_tmpC9B.tag=Cyc_Exp_tok;_tmpC9B.f1=
Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpC9B;});_tmpC9A;});break;case 388: _LL3225: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpC9D=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpC9D[0]=({struct Cyc_Exp_tok_struct _tmpC9E;_tmpC9E.tag=Cyc_Exp_tok;_tmpC9E.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmpC9F=
_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_struct));_tmpC9F[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmpCA0;_tmpCA0.tag=Cyc_Absyn_CompoundLit_e;_tmpCA0.f1=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmpCA0.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmpCA0;});_tmpC9F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));_tmpC9E;});
_tmpC9D;});break;case 389: _LL3228: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpCA2=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpCA2[0]=({struct Cyc_Exp_tok_struct
_tmpCA3;_tmpCA3.tag=Cyc_Exp_tok;_tmpCA3.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmpCA4=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_struct));_tmpCA4[0]=({
struct Cyc_Absyn_CompoundLit_e_struct _tmpCA5;_tmpCA5.tag=Cyc_Absyn_CompoundLit_e;
_tmpCA5.f1=Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);_tmpCA5.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmpCA5;});_tmpCA4;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[yylsp_offset]).last_line));_tmpCA3;});
_tmpCA2;});break;case 390: _LL3233: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpCA7=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpCA7[0]=({struct Cyc_Exp_tok_struct
_tmpCA8;_tmpCA8.tag=Cyc_Exp_tok;_tmpCA8.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Fill_e_struct*
_tmpCA9=_cycalloc(sizeof(struct Cyc_Absyn_Fill_e_struct));_tmpCA9[0]=({struct Cyc_Absyn_Fill_e_struct
_tmpCAA;_tmpCAA.tag=Cyc_Absyn_Fill_e;_tmpCAA.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmpCAA;});_tmpCA9;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmpCA8;});
_tmpCA7;});break;case 391: _LL3238: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpCAC=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpCAC[0]=({struct Cyc_Exp_tok_struct
_tmpCAD;_tmpCAD.tag=Cyc_Exp_tok;_tmpCAD.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Codegen_e_struct*
_tmpCAE=_cycalloc(sizeof(struct Cyc_Absyn_Codegen_e_struct));_tmpCAE[0]=({struct
Cyc_Absyn_Codegen_e_struct _tmpCAF;_tmpCAF.tag=Cyc_Absyn_Codegen_e;_tmpCAF.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmpCAF;});_tmpCAE;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmpCAD;});
_tmpCAC;});break;case 392: _LL3243: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpCB1=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpCB1[0]=({struct Cyc_Exp_tok_struct
_tmpCB2;_tmpCB2.tag=Cyc_Exp_tok;_tmpCB2.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmpCB3=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_e_struct));_tmpCB3[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmpCB4;_tmpCB4.tag=Cyc_Absyn_UnknownId_e;
_tmpCB4.f1=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);_tmpCB4;});_tmpCB3;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmpCB2;});
_tmpCB1;});break;case 393: _LL3248: yyval=yyvs[yyvsp_offset];break;case 394: _LL3253:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpCB7=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpCB7[0]=({struct Cyc_Exp_tok_struct _tmpCB8;_tmpCB8.tag=Cyc_Exp_tok;_tmpCB8.f1=
Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmpCB8;});
_tmpCB7;});break;case 395: _LL3254: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 396: _LL3257: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpCBB=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpCBB[0]=({struct Cyc_Exp_tok_struct
_tmpCBC;_tmpCBC.tag=Cyc_Exp_tok;_tmpCBC.f1=Cyc_Absyn_noinstantiate_exp(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmpCBD=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_e_struct));
_tmpCBD[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmpCBE;_tmpCBE.tag=Cyc_Absyn_UnknownId_e;
_tmpCBE.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmpCBE;});_tmpCBD;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpCBC;});_tmpCBB;});break;case 397: _LL3258: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpCC0=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpCC0[0]=({struct Cyc_Exp_tok_struct _tmpCC1;_tmpCC1.tag=Cyc_Exp_tok;_tmpCC1.f1=
Cyc_Absyn_instantiate_exp(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmpCC2=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_e_struct));_tmpCC2[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmpCC3;_tmpCC3.tag=Cyc_Absyn_UnknownId_e;
_tmpCC3.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);_tmpCC3;});_tmpCC2;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmpCC1;});
_tmpCC0;});break;case 398: _LL3263: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpCC5=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpCC5[0]=({struct Cyc_Exp_tok_struct
_tmpCC6;_tmpCC6.tag=Cyc_Exp_tok;_tmpCC6.f1=Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpCC6;});_tmpCC5;});break;case 399: _LL3268: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpCC8=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpCC8[0]=({struct Cyc_Exp_tok_struct _tmpCC9;_tmpCC9.tag=Cyc_Exp_tok;_tmpCC9.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Struct_e_struct*_tmpCCA=_cycalloc(
sizeof(struct Cyc_Absyn_Struct_e_struct));_tmpCCA[0]=({struct Cyc_Absyn_Struct_e_struct
_tmpCCB;_tmpCCB.tag=Cyc_Absyn_Struct_e;_tmpCCB.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmpCCB.f2=0;_tmpCCB.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmpCCB.f4=0;
_tmpCCB;});_tmpCCA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpCC9;});_tmpCC8;});break;case 400: _LL3271: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpCCD=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpCCD[0]=({struct Cyc_Exp_tok_struct _tmpCCE;_tmpCCE.tag=Cyc_Exp_tok;_tmpCCE.f1=
Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmpCCE;});_tmpCCD;});break;case 401: _LL3276: yyval=(
void*)({struct Cyc_ExpList_tok_struct*_tmpCD0=_cycalloc(sizeof(struct Cyc_ExpList_tok_struct));
_tmpCD0[0]=({struct Cyc_ExpList_tok_struct _tmpCD1;_tmpCD1.tag=Cyc_ExpList_tok;
_tmpCD1.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(yyvs[yyvsp_offset]));_tmpCD1;});_tmpCD0;});break;case 402:
_LL3279: yyval=(void*)({struct Cyc_ExpList_tok_struct*_tmpCD3=_cycalloc(sizeof(
struct Cyc_ExpList_tok_struct));_tmpCD3[0]=({struct Cyc_ExpList_tok_struct _tmpCD4;
_tmpCD4.tag=Cyc_ExpList_tok;_tmpCD4.f1=({struct Cyc_List_List*_tmpCD5=_cycalloc(
sizeof(struct Cyc_List_List));_tmpCD5->hd=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);
_tmpCD5->tl=0;_tmpCD5;});_tmpCD4;});_tmpCD3;});break;case 403: _LL3282: yyval=(void*)({
struct Cyc_ExpList_tok_struct*_tmpCD7=_cycalloc(sizeof(struct Cyc_ExpList_tok_struct));
_tmpCD7[0]=({struct Cyc_ExpList_tok_struct _tmpCD8;_tmpCD8.tag=Cyc_ExpList_tok;
_tmpCD8.f1=({struct Cyc_List_List*_tmpCD9=_cycalloc(sizeof(struct Cyc_List_List));
_tmpCD9->hd=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmpCD9->tl=Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmpCD9;});_tmpCD8;});
_tmpCD7;});break;case 404: _LL3286: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpCDB=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpCDB[0]=({struct Cyc_Exp_tok_struct
_tmpCDC;_tmpCDC.tag=Cyc_Exp_tok;_tmpCDC.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(
yyvs[yyvsp_offset])).f1,(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmpCDC;});
_tmpCDB;});break;case 405: _LL3290: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpCDE=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpCDE[0]=({struct Cyc_Exp_tok_struct
_tmpCDF;_tmpCDF.tag=Cyc_Exp_tok;_tmpCDF.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmpCDF;});_tmpCDE;});break;case 406: _LL3293:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpCE1=_cycalloc(sizeof(struct Cyc_Exp_tok_struct));
_tmpCE1[0]=({struct Cyc_Exp_tok_struct _tmpCE2;_tmpCE2.tag=Cyc_Exp_tok;_tmpCE2.f1=
Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmpCE2;});
_tmpCE1;});break;case 407: _LL3296: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpCE4=
_cycalloc(sizeof(struct Cyc_Exp_tok_struct));_tmpCE4[0]=({struct Cyc_Exp_tok_struct
_tmpCE5;_tmpCE5.tag=Cyc_Exp_tok;_tmpCE5.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmpCE5;});
_tmpCE4;});break;case 408: _LL3299: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmpCE7=_cycalloc(sizeof(struct Cyc_QualId_tok_struct));_tmpCE7[0]=({struct Cyc_QualId_tok_struct
_tmpCE8;_tmpCE8.tag=Cyc_QualId_tok;_tmpCE8.f1=({struct _tuple1*_tmpCE9=_cycalloc(
sizeof(struct _tuple1));_tmpCE9->f1=Cyc_Absyn_rel_ns_null;_tmpCE9->f2=({struct
_tagged_arr*_tmpCEA=_cycalloc(sizeof(struct _tagged_arr));_tmpCEA[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmpCEA;});_tmpCE9;});_tmpCE8;});_tmpCE7;});break;case 409:
_LL3302: yyval=yyvs[yyvsp_offset];break;default: _LL3307: break;}yyvsp_offset -=
yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;yylsp_offset ++;if(
yylen == 0){(yyls[yylsp_offset]).first_line=Cyc_yylloc.first_line;(yyls[
yylsp_offset]).first_column=Cyc_yylloc.first_column;(yyls[yylsp_offset]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
yylsp_offset]).last_column=(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_column;}else{(yyls[yylsp_offset]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
yylsp_offset]).last_column=(yyls[_check_known_subscript_notnull(10000,(
yylsp_offset + yylen)- 1)]).last_column;}yyn=(int)Cyc_yyr1[yyn];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(112,yyn - 125)]+ yyss[yyssp_offset];if((yystate >= 
0? yystate <= 4995: 0)? Cyc_yycheck[yystate]== yyss[yyssp_offset]: 0){yystate=(int)
Cyc_yytable[yystate];}else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(112,yyn - 125)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[yystate];if(yyn > - 32768? yyn < 
4995: 0){int sze=0;struct _tagged_arr msg;int x;int count;count=0;for(x=yyn < 0? - yyn: 0;
x < 237 / sizeof(unsigned char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(4996,x + yyn)]== x){(sze +=Cyc_Std_strlen(Cyc_yytname[
x])+ 15,count ++);}}msg=({unsigned int _tmpCED=(unsigned int)(sze + 15);
unsigned char*_tmpCEE=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmpCED));struct _tagged_arr _tmpCF0=_tag_arr(_tmpCEE,sizeof(
unsigned char),(unsigned int)(sze + 15));{unsigned int _tmpCEF=_tmpCED;
unsigned int i;for(i=0;i < _tmpCEF;i ++){_tmpCEE[i]='\000';}};_tmpCF0;});Cyc_Std_strcpy(
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
yynewstate;}void Cyc_yyprint(int i,void*v){void*_tmpCFA=v;struct _tuple15*_tmpD0C;
struct _tuple15 _tmpD0E;int _tmpD0F;unsigned char _tmpD11;short _tmpD13;struct
_tagged_arr _tmpD15;struct Cyc_Core_Opt*_tmpD17;struct Cyc_Core_Opt*_tmpD19;struct
Cyc_Core_Opt _tmpD1B;struct _tagged_arr*_tmpD1C;struct _tuple1*_tmpD1E;struct
_tuple1 _tmpD20;struct _tagged_arr*_tmpD21;void*_tmpD23;_LL3324: if(*((void**)
_tmpCFA)== Cyc_Int_tok){_LL3341: _tmpD0C=((struct Cyc_Int_tok_struct*)_tmpCFA)->f1;
_tmpD0E=*_tmpD0C;_LL3344: _tmpD0F=_tmpD0E.f2;goto _LL3325;}else{goto _LL3326;}
_LL3326: if(*((void**)_tmpCFA)== Cyc_Char_tok){_LL3346: _tmpD11=((struct Cyc_Char_tok_struct*)
_tmpCFA)->f1;goto _LL3327;}else{goto _LL3328;}_LL3328: if(*((void**)_tmpCFA)== Cyc_Short_tok){
_LL3348: _tmpD13=((struct Cyc_Short_tok_struct*)_tmpCFA)->f1;goto _LL3329;}else{
goto _LL3330;}_LL3330: if(*((void**)_tmpCFA)== Cyc_String_tok){_LL3350: _tmpD15=((
struct Cyc_String_tok_struct*)_tmpCFA)->f1;goto _LL3331;}else{goto _LL3332;}_LL3332:
if(*((void**)_tmpCFA)== Cyc_Stringopt_tok){_LL3352: _tmpD17=((struct Cyc_Stringopt_tok_struct*)
_tmpCFA)->f1;if(_tmpD17 == 0){goto _LL3333;}else{goto _LL3334;}}else{goto _LL3334;}
_LL3334: if(*((void**)_tmpCFA)== Cyc_Stringopt_tok){_LL3354: _tmpD19=((struct Cyc_Stringopt_tok_struct*)
_tmpCFA)->f1;if(_tmpD19 == 0){goto _LL3336;}else{_tmpD1B=*_tmpD19;_LL3357: _tmpD1C=(
struct _tagged_arr*)_tmpD1B.v;goto _LL3335;}}else{goto _LL3336;}_LL3336: if(*((void**)
_tmpCFA)== Cyc_QualId_tok){_LL3359: _tmpD1E=((struct Cyc_QualId_tok_struct*)
_tmpCFA)->f1;_tmpD20=*_tmpD1E;_LL3364: _tmpD23=_tmpD20.f1;goto _LL3362;_LL3362:
_tmpD21=_tmpD20.f2;goto _LL3337;}else{goto _LL3338;}_LL3338: goto _LL3339;_LL3325:({
struct Cyc_Std_Int_pa_struct _tmpD26;_tmpD26.tag=Cyc_Std_Int_pa;_tmpD26.f1=(int)((
unsigned int)_tmpD0F);{void*_tmpD25[1]={& _tmpD26};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmpD25,sizeof(void*),1));}});
goto _LL3323;_LL3327:({struct Cyc_Std_Int_pa_struct _tmpD28;_tmpD28.tag=Cyc_Std_Int_pa;
_tmpD28.f1=(int)((unsigned int)((int)_tmpD11));{void*_tmpD27[1]={& _tmpD28};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%c",sizeof(unsigned char),3),_tag_arr(_tmpD27,sizeof(
void*),1));}});goto _LL3323;_LL3329:({struct Cyc_Std_Int_pa_struct _tmpD2A;_tmpD2A.tag=
Cyc_Std_Int_pa;_tmpD2A.f1=(int)((unsigned int)((int)_tmpD13));{void*_tmpD29[1]={&
_tmpD2A};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%ds",sizeof(unsigned char),4),
_tag_arr(_tmpD29,sizeof(void*),1));}});goto _LL3323;_LL3331:({struct Cyc_Std_String_pa_struct
_tmpD2C;_tmpD2C.tag=Cyc_Std_String_pa;_tmpD2C.f1=(struct _tagged_arr)_tmpD15;{
void*_tmpD2B[1]={& _tmpD2C};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\"%s\"",
sizeof(unsigned char),5),_tag_arr(_tmpD2B,sizeof(void*),1));}});goto _LL3323;
_LL3333:({void*_tmpD2D[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("NULL",
sizeof(unsigned char),5),_tag_arr(_tmpD2D,sizeof(void*),0));});goto _LL3323;
_LL3335:({struct Cyc_Std_String_pa_struct _tmpD2F;_tmpD2F.tag=Cyc_Std_String_pa;
_tmpD2F.f1=(struct _tagged_arr)*_tmpD1C;{void*_tmpD2E[1]={& _tmpD2F};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("\"%s\"",sizeof(unsigned char),5),_tag_arr(_tmpD2E,
sizeof(void*),1));}});goto _LL3323;_LL3337: {struct Cyc_List_List*_tmpD30=0;{void*
_tmpD31=_tmpD23;struct Cyc_List_List*_tmpD39;struct Cyc_List_List*_tmpD3B;_LL3379:
if((unsigned int)_tmpD31 > 1?*((int*)_tmpD31)== Cyc_Absyn_Rel_n: 0){_LL3386:
_tmpD39=((struct Cyc_Absyn_Rel_n_struct*)_tmpD31)->f1;goto _LL3380;}else{goto
_LL3381;}_LL3381: if((unsigned int)_tmpD31 > 1?*((int*)_tmpD31)== Cyc_Absyn_Abs_n:
0){_LL3388: _tmpD3B=((struct Cyc_Absyn_Abs_n_struct*)_tmpD31)->f1;goto _LL3382;}
else{goto _LL3383;}_LL3383: if(_tmpD31 == (void*)Cyc_Absyn_Loc_n){goto _LL3384;}
else{goto _LL3378;}_LL3380: _tmpD30=_tmpD39;goto _LL3378;_LL3382: _tmpD30=_tmpD3B;
goto _LL3378;_LL3384: goto _LL3378;_LL3378:;}for(0;_tmpD30 != 0;_tmpD30=_tmpD30->tl){({
struct Cyc_Std_String_pa_struct _tmpD3E;_tmpD3E.tag=Cyc_Std_String_pa;_tmpD3E.f1=(
struct _tagged_arr)*((struct _tagged_arr*)_tmpD30->hd);{void*_tmpD3D[1]={& _tmpD3E};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",sizeof(unsigned char),5),_tag_arr(
_tmpD3D,sizeof(void*),1));}});}({struct Cyc_Std_String_pa_struct _tmpD40;_tmpD40.tag=
Cyc_Std_String_pa;_tmpD40.f1=(struct _tagged_arr)*_tmpD21;{void*_tmpD3F[1]={&
_tmpD40};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",sizeof(unsigned char),5),
_tag_arr(_tmpD3F,sizeof(void*),1));}});goto _LL3323;}_LL3339:({void*_tmpD41[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("?",sizeof(unsigned char),2),_tag_arr(
_tmpD41,sizeof(void*),0));});goto _LL3323;_LL3323:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc_Std___sFILE*f){Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*
_tmpD42=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpD42->v=Cyc_Lexing_from_file(f);
_tmpD42;});Cyc_yyparse();return Cyc_Parse_parse_result;}
