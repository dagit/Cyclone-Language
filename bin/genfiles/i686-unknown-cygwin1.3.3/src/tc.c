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
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern int Cyc_List_length(struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern unsigned char Cyc_List_Nth[8];extern int Cyc_List_mem(int(*compare)(void*,
void*),struct Cyc_List_List*l,void*x);extern struct Cyc_List_List*Cyc_List_filter_c(
int(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);extern int Cyc_Std_strptrcmp(struct
_tagged_arr*s1,struct _tagged_arr*s2);extern int Cyc_Std_zstrptrcmp(struct
_tagged_arr*,struct _tagged_arr*);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
unsigned char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
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
_tuple0*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*
tunion_name;struct _tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
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
Cyc_List_List*f1;};struct Cyc_Absyn_StructType_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Structdecl**f3;};struct Cyc_Absyn_UnionType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Uniondecl**f3;};
struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_AnonStructType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonUnionType_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Core_Opt*f3;};struct Cyc_Absyn_AccessEff_struct{
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
tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Primop_e_struct{
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
int tag;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Tuniondecl*f4;struct Cyc_Absyn_Tunionfield*
f5;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Codegen_e_struct{int tag;struct
Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Fill_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct _tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;struct _tuple2 condition;
struct _tuple2 delta;struct Cyc_Absyn_Stmt*body;};struct Cyc_Absyn_Exp_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple2
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct
Cyc_Absyn_Goto_s_struct{int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple2 f2;
struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_SwitchC_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Cut_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Splice_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Label_s_struct{int tag;struct _tagged_arr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct _tuple2 f2;};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
Cyc_Absyn_UnknownFields_p_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;
struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*
cnst_exp;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
void*sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structfield{
struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structdecl{void*sc;struct
Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Uniondecl{void*sc;struct Cyc_Core_Opt*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Struct_d_struct{int tag;struct Cyc_Absyn_Structdecl*
f1;};struct Cyc_Absyn_Union_d_struct{int tag;struct Cyc_Absyn_Uniondecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[
15];extern void*Cyc_Absyn_compress_kb(void*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);extern struct _tagged_arr Cyc_Absyn_attribute2string(
void*);extern int Cyc_Absyn_fntype_att(void*a);struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*);extern struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Set_Set;
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));extern struct Cyc_Set_Set*
Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);extern int Cyc_Set_member(struct Cyc_Set_Set*
s,void*elt);extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern
unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];extern
int Cyc_Dict_member(struct Cyc_Dict_Dict*d,void*k);extern struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*
d,void*k);extern struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,
void*k);extern void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict*d);
extern struct Cyc_Dict_Dict*Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict*d);
extern struct Cyc_Dict_Dict*Cyc_Dict_filter_c(int(*f)(void*,void*,void*),void*env,
struct Cyc_Dict_Dict*d);struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_StructRes_struct{
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
le;};extern struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv();extern struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_new_fenv(struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);extern
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tagged_arr*,struct Cyc_List_List*);extern struct Cyc_Core_Opt*Cyc_Tcenv_lookup_xtuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple0*);extern int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*);extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_compress(void*t);
extern int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);extern int Cyc_Tcutil_is_function_type(void*t);extern void Cyc_Tcutil_explain_failure();
extern void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);extern void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn);extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,void*);extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);extern void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);extern int Cyc_Tcutil_bits_only(void*t);extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);extern int Cyc_Tcutil_supports_default(
void*);extern void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct
Cyc_Absyn_Exp*);extern void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,int new_block);extern unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);extern void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*
ds);extern struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
extern void Cyc_Tc_tcStructdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct
Cyc_Position_Segment*,struct Cyc_Absyn_Structdecl*);extern void Cyc_Tc_tcUniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Uniondecl*);extern void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Tuniondecl*);extern
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*);extern unsigned char Cyc_Tcdecl_Incompatible[17];struct
Cyc_Tcdecl_Xtunionfielddecl{struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*
field;};extern struct Cyc_Absyn_Structdecl*Cyc_Tcdecl_merge_structdecl(struct Cyc_Absyn_Structdecl*
d0,struct Cyc_Absyn_Structdecl*d1,struct Cyc_Position_Segment*loc,struct
_tagged_arr*msg);extern struct Cyc_Absyn_Uniondecl*Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl*d0,struct Cyc_Absyn_Uniondecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Tuniondecl*Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_List_List*
Cyc_Tcdecl_sort_xtunion_fields(struct Cyc_List_List*f,int*res,struct _tagged_arr*v,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Tcgenrep_RepInfo;
extern struct Cyc_Dict_Dict*Cyc_Tcgenrep_empty_typerep_dict();struct _tuple3{struct
Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*f3;};extern struct
_tuple3 Cyc_Tcgenrep_tcGenrep(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,
struct Cyc_Position_Segment*loc,void*type,struct Cyc_Dict_Dict*dict);static
unsigned char _tmp0[1]="";static struct _tagged_arr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,
_tmp0 + 1};static struct _tagged_arr*Cyc_Tc_tc_msg=(struct _tagged_arr*)& Cyc_Tc_tc_msg_c;
static struct Cyc_List_List*Cyc_Tc_transfer_fn_type_atts(void*t,struct Cyc_List_List*
atts){void*_tmp1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp2;struct Cyc_List_List*
_tmp3;struct Cyc_List_List**_tmp4;_LL1: if((unsigned int)_tmp1 > 3?*((int*)_tmp1)== 
8: 0){_LL5: _tmp2=((struct Cyc_Absyn_FnType_struct*)_tmp1)->f1;_LL6: _tmp3=_tmp2.attributes;
_tmp4=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp1)->f1).attributes;
goto _LL2;}else{goto _LL3;}_LL3: goto _LL4;_LL2: {struct Cyc_List_List*_tmp5=0;for(0;
atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){*_tmp4=({
struct Cyc_List_List*_tmp6=_cycalloc(sizeof(struct Cyc_List_List));_tmp6->hd=(void*)((
void*)atts->hd);_tmp6->tl=*_tmp4;_tmp6;});}else{_tmp5=({struct Cyc_List_List*
_tmp7=_cycalloc(sizeof(struct Cyc_List_List));_tmp7->hd=(void*)((void*)atts->hd);
_tmp7->tl=_tmp5;_tmp7;});}}return _tmp5;}_LL4: return({void*_tmp8[0]={};((struct
Cyc_List_List*(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("transfer_fn_type_atts",sizeof(unsigned char),22),_tag_arr(_tmp8,
sizeof(void*),0));});_LL0:;}struct _tuple4{void*f1;int f2;};static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Vardecl*vd,int check_var_init){struct _tagged_arr*v=(*vd->name).f2;
void*t=(void*)vd->type;void*sc=(void*)vd->sc;struct Cyc_List_List*atts=vd->attributes;
vd->escapes=1;{void*_tmp9=(*vd->name).f1;struct Cyc_List_List*_tmpA;struct Cyc_List_List*
_tmpB;_LL8: if((unsigned int)_tmp9 > 1?*((int*)_tmp9)== 0: 0){_LLE: _tmpA=((struct
Cyc_Absyn_Rel_n_struct*)_tmp9)->f1;if(_tmpA == 0){goto _LL9;}else{goto _LLA;}}else{
goto _LLA;}_LLA: if((unsigned int)_tmp9 > 1?*((int*)_tmp9)== 1: 0){_LLF: _tmpB=((
struct Cyc_Absyn_Abs_n_struct*)_tmp9)->f1;if(_tmpB == 0){goto _LLB;}else{goto _LLC;}}
else{goto _LLC;}_LLC: goto _LLD;_LL9: goto _LL7;_LLB: goto _LL7;_LLD:({struct Cyc_Std_String_pa_struct
_tmpD;_tmpD.tag=0;_tmpD.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name);{
void*_tmpC[1]={& _tmpD};Cyc_Tcutil_terr(loc,_tag_arr("qualified variable declarations are not implemented (%s)",
sizeof(unsigned char),57),_tag_arr(_tmpC,sizeof(void*),1));}});return;_LL7:;}(*
vd->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmpE=_cycalloc(sizeof(struct
Cyc_Absyn_Abs_n_struct));_tmpE[0]=({struct Cyc_Absyn_Abs_n_struct _tmpF;_tmpF.tag=
1;_tmpF.f1=te->ns;_tmpF;});_tmpE;});{void*_tmp10=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Tqual _tmp12;void*_tmp13;_LL11: if((
unsigned int)_tmp10 > 3?*((int*)_tmp10)== 7: 0){_LL18: _tmp13=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp10)->f1;goto _LL17;_LL17: _tmp12=((struct Cyc_Absyn_ArrayType_struct*)_tmp10)->f2;
goto _LL16;_LL16: _tmp11=((struct Cyc_Absyn_ArrayType_struct*)_tmp10)->f3;if(_tmp11
== 0){goto _LL15;}else{goto _LL13;}}else{goto _LL13;}_LL15: if(vd->initializer != 0){
goto _LL12;}else{goto _LL13;}_LL13: goto _LL14;_LL12:{void*_tmp14=(void*)((struct Cyc_Absyn_Exp*)
_check_null(vd->initializer))->r;void*_tmp15;struct _tagged_arr _tmp16;struct Cyc_Absyn_Exp*
_tmp17;struct Cyc_List_List*_tmp18;struct Cyc_List_List*_tmp19;_LL1A: if(*((int*)
_tmp14)== 0){_LL24: _tmp15=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp14)->f1;
if((unsigned int)_tmp15 > 1?*((int*)_tmp15)== 5: 0){_LL25: _tmp16=((struct Cyc_Absyn_String_c_struct*)
_tmp15)->f1;goto _LL1B;}else{goto _LL1C;}}else{goto _LL1C;}_LL1C: if(*((int*)_tmp14)
== 27){_LL26: _tmp17=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp14)->f2;goto
_LL1D;}else{goto _LL1E;}_LL1E: if(*((int*)_tmp14)== 34){_LL27: _tmp18=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp14)->f2;goto _LL1F;}else{goto _LL20;}_LL20: if(*((int*)_tmp14)== 26){_LL28:
_tmp19=((struct Cyc_Absyn_Array_e_struct*)_tmp14)->f1;goto _LL21;}else{goto _LL22;}
_LL22: goto _LL23;_LL1B: t=(void*)(vd->type=(void*)((void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp1A=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp1A[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp1B;_tmp1B.tag=7;_tmp1B.f1=(void*)_tmp13;_tmp1B.f2=_tmp12;_tmp1B.f3=(struct
Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_arr_size(_tmp16,sizeof(unsigned char)),0);
_tmp1B;});_tmp1A;})));goto _LL19;_LL1D: t=(void*)(vd->type=(void*)((void*)({struct
Cyc_Absyn_ArrayType_struct*_tmp1C=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));
_tmp1C[0]=({struct Cyc_Absyn_ArrayType_struct _tmp1D;_tmp1D.tag=7;_tmp1D.f1=(void*)
_tmp13;_tmp1D.f2=_tmp12;_tmp1D.f3=(struct Cyc_Absyn_Exp*)_tmp17;_tmp1D;});_tmp1C;})));
goto _LL19;_LL1F: _tmp19=_tmp18;goto _LL21;_LL21: t=(void*)(vd->type=(void*)((void*)({
struct Cyc_Absyn_ArrayType_struct*_tmp1E=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));
_tmp1E[0]=({struct Cyc_Absyn_ArrayType_struct _tmp1F;_tmp1F.tag=7;_tmp1F.f1=(void*)
_tmp13;_tmp1F.f2=_tmp12;_tmp1F.f3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp19),0);_tmp1F;});
_tmp1E;})));goto _LL19;_LL23: goto _LL19;_LL19:;}goto _LL10;_LL14: goto _LL10;_LL10:;}
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmp20=Cyc_Tcutil_compress(
t);_LL2A: if((unsigned int)_tmp20 > 3?*((int*)_tmp20)== 7: 0){goto _LL2B;}else{goto
_LL2C;}_LL2C: goto _LL2D;_LL2B: vd->escapes=0;goto _LL29;_LL2D: goto _LL29;_LL29:;}if(
Cyc_Tcutil_is_function_type(t)){atts=Cyc_Tc_transfer_fn_type_atts(t,atts);}if(sc
== (void*)3? 1: sc == (void*)4){if(vd->initializer != 0){({void*_tmp21[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("extern declaration should not have initializer",sizeof(
unsigned char),47),_tag_arr(_tmp21,sizeof(void*),0));});}}else{if(! Cyc_Tcutil_is_function_type(
t)){for(0;atts != 0;atts=atts->tl){void*_tmp22=(void*)atts->hd;_LL2F: if((
unsigned int)_tmp22 > 16?*((int*)_tmp22)== 1: 0){goto _LL30;}else{goto _LL31;}_LL31:
if((unsigned int)_tmp22 > 16?*((int*)_tmp22)== 2: 0){goto _LL32;}else{goto _LL33;}
_LL33: if((int)_tmp22 == 6){goto _LL34;}else{goto _LL35;}_LL35: if((int)_tmp22 == 7){
goto _LL36;}else{goto _LL37;}_LL37: if((int)_tmp22 == 8){goto _LL38;}else{goto _LL39;}
_LL39: if((int)_tmp22 == 9){goto _LL3A;}else{goto _LL3B;}_LL3B: if((int)_tmp22 == 10){
goto _LL3C;}else{goto _LL3D;}_LL3D: if((int)_tmp22 == 11){goto _LL3E;}else{goto _LL3F;}
_LL3F: goto _LL40;_LL30: goto _LL32;_LL32: goto _LL34;_LL34: goto _LL36;_LL36: goto _LL38;
_LL38: goto _LL3A;_LL3A: goto _LL3C;_LL3C: goto _LL3E;_LL3E: continue;_LL40:({struct Cyc_Std_String_pa_struct
_tmp25;_tmp25.tag=0;_tmp25.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name);{
struct Cyc_Std_String_pa_struct _tmp24;_tmp24.tag=0;_tmp24.f1=(struct _tagged_arr)
Cyc_Absyn_attribute2string((void*)atts->hd);{void*_tmp23[2]={& _tmp24,& _tmp25};
Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s for variable %s",sizeof(
unsigned char),33),_tag_arr(_tmp23,sizeof(void*),2));}}});goto _LL2E;_LL2E:;}if(
vd->initializer == 0){if(check_var_init? ! Cyc_Tcutil_supports_default(t): 0){({
struct Cyc_Std_String_pa_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp27;_tmp27.tag=0;
_tmp27.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name);{void*_tmp26[2]={&
_tmp27,& _tmp28};Cyc_Tcutil_terr(loc,_tag_arr("initializer required for variable %s of type %s",
sizeof(unsigned char),48),_tag_arr(_tmp26,sizeof(void*),2));}}});}}else{struct
Cyc_Absyn_Exp*_tmp29=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);void*
_tmp2A=Cyc_Tcexp_tcExpInitializer(te,(void**)& t,_tmp29);if(! Cyc_Tcutil_coerce_assign(
te,_tmp29,t)){({struct Cyc_Std_String_pa_struct _tmp2E;_tmp2E.tag=0;_tmp2E.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp2A);{struct Cyc_Std_String_pa_struct
_tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_Std_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(vd->name);{void*_tmp2B[3]={& _tmp2C,& _tmp2D,& _tmp2E};Cyc_Tcutil_terr(
loc,_tag_arr("%s is declared with type \n%s\n but initialized with type \n%s",
sizeof(unsigned char),60),_tag_arr(_tmp2B,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}
if(! Cyc_Tcutil_is_const_exp(te,_tmp29)){({void*_tmp2F[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("initializer is not a constant expression",sizeof(unsigned char),41),
_tag_arr(_tmp2F,sizeof(void*),0));});}}}else{for(0;atts != 0;atts=atts->tl){void*
_tmp30=(void*)atts->hd;_LL42: if((unsigned int)_tmp30 > 16?*((int*)_tmp30)== 0: 0){
goto _LL43;}else{goto _LL44;}_LL44: if((int)_tmp30 == 0){goto _LL45;}else{goto _LL46;}
_LL46: if((int)_tmp30 == 1){goto _LL47;}else{goto _LL48;}_LL48: if((int)_tmp30 == 2){
goto _LL49;}else{goto _LL4A;}_LL4A: if((int)_tmp30 == 3){goto _LL4B;}else{goto _LL4C;}
_LL4C: if((unsigned int)_tmp30 > 16?*((int*)_tmp30)== 3: 0){goto _LL4D;}else{goto
_LL4E;}_LL4E: if((int)_tmp30 == 4){goto _LL4F;}else{goto _LL50;}_LL50: if((
unsigned int)_tmp30 > 16?*((int*)_tmp30)== 1: 0){goto _LL51;}else{goto _LL52;}_LL52:
if((int)_tmp30 == 5){goto _LL53;}else{goto _LL54;}_LL54: goto _LL55;_LL43: goto _LL45;
_LL45: goto _LL47;_LL47: goto _LL49;_LL49: goto _LL4B;_LL4B: goto _LL4D;_LL4D: goto _LL4F;
_LL4F:({void*_tmp31[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("tcVardecl: fn type attributes in function var decl",sizeof(
unsigned char),51),_tag_arr(_tmp31,sizeof(void*),0));});goto _LL41;_LL51: goto
_LL53;_LL53:({struct Cyc_Std_String_pa_struct _tmp33;_tmp33.tag=0;_tmp33.f1=(
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd);{void*_tmp32[1]={&
_tmp33};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s in function declaration",
sizeof(unsigned char),41),_tag_arr(_tmp32,sizeof(void*),1));}});goto _LL41;_LL55:
continue;_LL41:;}}}{struct _handler_cons _tmp34;_push_handler(& _tmp34);{int _tmp36=
0;if(setjmp(_tmp34.handler)){_tmp36=1;}if(! _tmp36){{struct _tuple4*_tmp37=((
struct _tuple4*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->ordinaries,
v);void*_tmp38=(*_tmp37).f1;void*_tmp39;_LL57: if(*((int*)_tmp38)== 0){_LL61:
_tmp39=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp38)->f1;goto _LL58;}else{goto
_LL59;}_LL59: if(*((int*)_tmp38)== 1){goto _LL5A;}else{goto _LL5B;}_LL5B: if(*((int*)
_tmp38)== 2){goto _LL5C;}else{goto _LL5D;}_LL5D: if(*((int*)_tmp38)== 4){goto _LL5E;}
else{goto _LL5F;}_LL5F: if(*((int*)_tmp38)== 3){goto _LL60;}else{goto _LL56;}_LL58: {
struct Cyc_Absyn_Global_b_struct*_tmp3A=({struct Cyc_Absyn_Global_b_struct*_tmp3F=
_cycalloc(sizeof(struct Cyc_Absyn_Global_b_struct));_tmp3F[0]=({struct Cyc_Absyn_Global_b_struct
_tmp40;_tmp40.tag=0;_tmp40.f1=vd;_tmp40;});_tmp3F;});void*_tmp3B=Cyc_Tcdecl_merge_binding(
_tmp39,(void*)_tmp3A,loc,Cyc_Tc_tc_msg);if(_tmp3B == (void*)0){goto _LL56;}if(
_tmp3B == _tmp39?(*_tmp37).f2: 0){goto _LL56;}ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*v))Cyc_Dict_insert)(ge->ordinaries,
v,({struct _tuple4*_tmp3C=_cycalloc(sizeof(struct _tuple4));_tmp3C->f1=(void*)({
struct Cyc_Tcenv_VarRes_struct*_tmp3D=_cycalloc(sizeof(struct Cyc_Tcenv_VarRes_struct));
_tmp3D[0]=({struct Cyc_Tcenv_VarRes_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(void*)
_tmp3B;_tmp3E;});_tmp3D;});_tmp3C->f2=1;_tmp3C;}));goto _LL56;}_LL5A: goto _LL56;
_LL5C:({void*_tmp41[0]={};Cyc_Tcutil_warn(loc,_tag_arr("variable declaration shadows previous [x]tunion constructor",
sizeof(unsigned char),60),_tag_arr(_tmp41,sizeof(void*),0));});goto _LL56;_LL5E:
goto _LL60;_LL60:({void*_tmp42[0]={};Cyc_Tcutil_warn(loc,_tag_arr("variable declaration shadows previous enum tag",
sizeof(unsigned char),47),_tag_arr(_tmp42,sizeof(void*),0));});goto _LL56;_LL56:;};
_pop_handler();}else{void*_tmp35=(void*)_exn_thrown;void*_tmp44=_tmp35;_LL63: if(
_tmp44 == Cyc_Dict_Absent){goto _LL64;}else{goto _LL65;}_LL65: goto _LL66;_LL64: ge->ordinaries=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*
v))Cyc_Dict_insert)(ge->ordinaries,v,({struct _tuple4*_tmp45=_cycalloc(sizeof(
struct _tuple4));_tmp45->f1=(void*)({struct Cyc_Tcenv_VarRes_struct*_tmp46=
_cycalloc(sizeof(struct Cyc_Tcenv_VarRes_struct));_tmp46[0]=({struct Cyc_Tcenv_VarRes_struct
_tmp47;_tmp47.tag=0;_tmp47.f1=(void*)((void*)({struct Cyc_Absyn_Global_b_struct*
_tmp48=_cycalloc(sizeof(struct Cyc_Absyn_Global_b_struct));_tmp48[0]=({struct Cyc_Absyn_Global_b_struct
_tmp49;_tmp49.tag=0;_tmp49.f1=vd;_tmp49;});_tmp48;}));_tmp47;});_tmp46;});_tmp45->f2=
0;_tmp45;}));goto _LL62;_LL66:(void)_throw(_tmp44);_LL62:;}}}}static void Cyc_Tc_tcFndecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Fndecl*fd){struct _tagged_arr*v=(*fd->name).f2;void*sc=(void*)fd->sc;{
void*_tmp4A=(*fd->name).f1;struct Cyc_List_List*_tmp4B;struct Cyc_List_List*_tmp4C;
_LL68: if((unsigned int)_tmp4A > 1?*((int*)_tmp4A)== 0: 0){_LL6E: _tmp4B=((struct Cyc_Absyn_Rel_n_struct*)
_tmp4A)->f1;if(_tmp4B == 0){goto _LL69;}else{goto _LL6A;}}else{goto _LL6A;}_LL6A: if((
unsigned int)_tmp4A > 1?*((int*)_tmp4A)== 1: 0){_LL6F: _tmp4C=((struct Cyc_Absyn_Abs_n_struct*)
_tmp4A)->f1;goto _LL6B;}else{goto _LL6C;}_LL6C: goto _LL6D;_LL69: goto _LL67;_LL6B:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp4D=_cycalloc(sizeof(
struct Cyc_Core_Impossible_struct));_tmp4D[0]=({struct Cyc_Core_Impossible_struct
_tmp4E;_tmp4E.tag=Cyc_Core_Impossible;_tmp4E.f1=_tag_arr("tc: Abs_n in tcFndecl",
sizeof(unsigned char),22);_tmp4E;});_tmp4D;}));_LL6D:({struct Cyc_Std_String_pa_struct
_tmp50;_tmp50.tag=0;_tmp50.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(fd->name);{
void*_tmp4F[1]={& _tmp50};Cyc_Tcutil_terr(loc,_tag_arr("qualified function declarations are not implemented (%s)",
sizeof(unsigned char),57),_tag_arr(_tmp4F,sizeof(void*),1));}});return;_LL67:;}(*
fd->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp51=_cycalloc(sizeof(
struct Cyc_Absyn_Abs_n_struct));_tmp51[0]=({struct Cyc_Absyn_Abs_n_struct _tmp52;
_tmp52.tag=1;_tmp52.f1=te->ns;_tmp52;});_tmp51;});Cyc_Tcutil_check_fndecl_valid_type(
loc,te,fd);{void*t=Cyc_Tcutil_fndecl2typ(fd);fd->attributes=Cyc_Tc_transfer_fn_type_atts(
t,fd->attributes);{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=
atts->tl){void*_tmp53=(void*)atts->hd;_LL71: if((int)_tmp53 == 5){goto _LL72;}else{
goto _LL73;}_LL73: if((unsigned int)_tmp53 > 16?*((int*)_tmp53)== 1: 0){goto _LL74;}
else{goto _LL75;}_LL75: goto _LL76;_LL72: goto _LL74;_LL74:({struct Cyc_Std_String_pa_struct
_tmp55;_tmp55.tag=0;_tmp55.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp54[1]={& _tmp55};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s for function",
sizeof(unsigned char),30),_tag_arr(_tmp54,sizeof(void*),1));}});goto _LL70;_LL76:
goto _LL70;_LL70:;}}{struct _handler_cons _tmp56;_push_handler(& _tmp56);{int _tmp58=
0;if(setjmp(_tmp56.handler)){_tmp58=1;}if(! _tmp58){{struct _tuple4*_tmp59=((
struct _tuple4*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->ordinaries,
v);void*_tmp5A=(*_tmp59).f1;void*_tmp5B;_LL78: if(*((int*)_tmp5A)== 0){_LL82:
_tmp5B=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp5A)->f1;goto _LL79;}else{goto
_LL7A;}_LL7A: if(*((int*)_tmp5A)== 1){goto _LL7B;}else{goto _LL7C;}_LL7C: if(*((int*)
_tmp5A)== 2){goto _LL7D;}else{goto _LL7E;}_LL7E: if(*((int*)_tmp5A)== 4){goto _LL7F;}
else{goto _LL80;}_LL80: if(*((int*)_tmp5A)== 3){goto _LL81;}else{goto _LL77;}_LL79: {
struct Cyc_Absyn_Funname_b_struct*_tmp5C=({struct Cyc_Absyn_Funname_b_struct*
_tmp61=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_struct));_tmp61[0]=({struct Cyc_Absyn_Funname_b_struct
_tmp62;_tmp62.tag=1;_tmp62.f1=fd;_tmp62;});_tmp61;});void*_tmp5D=Cyc_Tcdecl_merge_binding(
_tmp5B,(void*)_tmp5C,loc,Cyc_Tc_tc_msg);if(_tmp5D == (void*)0){goto _LL77;}if(
_tmp5D == _tmp5B?(*_tmp59).f2: 0){goto _LL77;}ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*v))Cyc_Dict_insert)(ge->ordinaries,
v,({struct _tuple4*_tmp5E=_cycalloc(sizeof(struct _tuple4));_tmp5E->f1=(void*)({
struct Cyc_Tcenv_VarRes_struct*_tmp5F=_cycalloc(sizeof(struct Cyc_Tcenv_VarRes_struct));
_tmp5F[0]=({struct Cyc_Tcenv_VarRes_struct _tmp60;_tmp60.tag=0;_tmp60.f1=(void*)
_tmp5D;_tmp60;});_tmp5F;});_tmp5E->f2=1;_tmp5E;}));goto _LL77;}_LL7B: goto _LL77;
_LL7D:({void*_tmp63[0]={};Cyc_Tcutil_warn(loc,_tag_arr("function declaration shadows previous [x]tunion constructor",
sizeof(unsigned char),60),_tag_arr(_tmp63,sizeof(void*),0));});goto _LL77;_LL7F:
goto _LL81;_LL81:({void*_tmp64[0]={};Cyc_Tcutil_warn(loc,_tag_arr("function declaration shadows previous enum tag",
sizeof(unsigned char),47),_tag_arr(_tmp64,sizeof(void*),0));});goto _LL77;_LL77:;};
_pop_handler();}else{void*_tmp57=(void*)_exn_thrown;void*_tmp66=_tmp57;_LL84: if(
_tmp66 == Cyc_Dict_Absent){goto _LL85;}else{goto _LL86;}_LL86: goto _LL87;_LL85: ge->ordinaries=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*
v))Cyc_Dict_insert)(ge->ordinaries,v,({struct _tuple4*_tmp67=_cycalloc(sizeof(
struct _tuple4));_tmp67->f1=(void*)({struct Cyc_Tcenv_VarRes_struct*_tmp68=
_cycalloc(sizeof(struct Cyc_Tcenv_VarRes_struct));_tmp68[0]=({struct Cyc_Tcenv_VarRes_struct
_tmp69;_tmp69.tag=0;_tmp69.f1=(void*)((void*)({struct Cyc_Absyn_Funname_b_struct*
_tmp6A=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_struct));_tmp6A[0]=({struct Cyc_Absyn_Funname_b_struct
_tmp6B;_tmp6B.tag=1;_tmp6B.f1=fd;_tmp6B;});_tmp6A;}));_tmp69;});_tmp68;});_tmp67->f2=
0;_tmp67;}));goto _LL83;_LL87:(void)_throw(_tmp66);_LL83:;}}}te->le=({struct Cyc_Core_Opt*
_tmp6C=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp6C->v=(void*)((void*)({struct
Cyc_Tcenv_Outermost_struct*_tmp6D=_cycalloc(sizeof(struct Cyc_Tcenv_Outermost_struct));
_tmp6D[0]=({struct Cyc_Tcenv_Outermost_struct _tmp6E;_tmp6E.tag=0;_tmp6E.f1=(void*)
Cyc_Tcenv_new_fenv(loc,fd);_tmp6E;});_tmp6D;}));_tmp6C;});Cyc_Tcstmt_tcStmt(te,
fd->body,0);if(! Cyc_Tcenv_all_labels_resolved(te)){({void*_tmp6F[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("function has goto statements to undefined labels",sizeof(
unsigned char),49),_tag_arr(_tmp6F,sizeof(void*),0));});}te->le=0;}}static void
Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Typedefdecl*td){struct _tagged_arr*v=(*td->name).f2;{void*
_tmp70=(*td->name).f1;struct Cyc_List_List*_tmp71;struct Cyc_List_List*_tmp72;
_LL89: if((unsigned int)_tmp70 > 1?*((int*)_tmp70)== 0: 0){_LL8F: _tmp71=((struct Cyc_Absyn_Rel_n_struct*)
_tmp70)->f1;if(_tmp71 == 0){goto _LL8A;}else{goto _LL8B;}}else{goto _LL8B;}_LL8B: if((
unsigned int)_tmp70 > 1?*((int*)_tmp70)== 1: 0){_LL90: _tmp72=((struct Cyc_Absyn_Abs_n_struct*)
_tmp70)->f1;if(_tmp72 == 0){goto _LL8C;}else{goto _LL8D;}}else{goto _LL8D;}_LL8D:
goto _LL8E;_LL8A: goto _LL88;_LL8C: goto _LL88;_LL8E:({struct Cyc_Std_String_pa_struct
_tmp74;_tmp74.tag=0;_tmp74.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(td->name);{
void*_tmp73[1]={& _tmp74};Cyc_Tcutil_terr(loc,_tag_arr("qualified typedef declarations are not implemented (%s)",
sizeof(unsigned char),56),_tag_arr(_tmp73,sizeof(void*),1));}});return;_LL88:;}
if(((int(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_member)(ge->typedefs,
v)){({struct Cyc_Std_String_pa_struct _tmp76;_tmp76.tag=0;_tmp76.f1=(struct
_tagged_arr)*v;{void*_tmp75[1]={& _tmp76};Cyc_Tcutil_terr(loc,_tag_arr("redeclaration of typedef %s",
sizeof(unsigned char),28),_tag_arr(_tmp75,sizeof(void*),1));}});return;}(*td->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp77=_cycalloc(sizeof(struct Cyc_Absyn_Abs_n_struct));
_tmp77[0]=({struct Cyc_Absyn_Abs_n_struct _tmp78;_tmp78.tag=1;_tmp78.f1=te->ns;
_tmp78;});_tmp77;});Cyc_Tcutil_check_unique_tvars(loc,td->tvs);Cyc_Tcutil_add_tvar_identities(
td->tvs);Cyc_Tcutil_check_type(loc,te,td->tvs,(void*)0,0,(void*)td->defn);ge->typedefs=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Typedefdecl*
v))Cyc_Dict_insert)(ge->typedefs,v,td);}static void Cyc_Tc_tcStructFields(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct
_tagged_arr obj,struct Cyc_List_List*fields,struct Cyc_List_List*tvs){struct
_RegionHandle _tmp79=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=&
_tmp79;_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;struct Cyc_List_List*
_tmp7A=fields;for(0;_tmp7A != 0;_tmp7A=_tmp7A->tl){struct Cyc_Absyn_Structfield
_tmp7C;struct Cyc_List_List*_tmp7D;struct Cyc_Absyn_Exp*_tmp7E;void*_tmp7F;struct
Cyc_Absyn_Tqual _tmp80;struct _tagged_arr*_tmp81;struct Cyc_Absyn_Structfield*
_tmp7B=(struct Cyc_Absyn_Structfield*)_tmp7A->hd;_tmp7C=*_tmp7B;_LL96: _tmp81=
_tmp7C.name;goto _LL95;_LL95: _tmp80=_tmp7C.tq;goto _LL94;_LL94: _tmp7F=(void*)
_tmp7C.type;goto _LL93;_LL93: _tmp7E=_tmp7C.width;goto _LL92;_LL92: _tmp7D=_tmp7C.attributes;
goto _LL91;_LL91: if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),
struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,
prev_fields,_tmp81)){({struct Cyc_Std_String_pa_struct _tmp84;_tmp84.tag=0;_tmp84.f1=(
struct _tagged_arr)obj;{struct Cyc_Std_String_pa_struct _tmp83;_tmp83.tag=0;_tmp83.f1=(
struct _tagged_arr)*_tmp81;{void*_tmp82[2]={& _tmp83,& _tmp84};Cyc_Tcutil_terr(loc,
_tag_arr("duplicate field %s in %s",sizeof(unsigned char),25),_tag_arr(_tmp82,
sizeof(void*),2));}}});}if(Cyc_Std_strcmp(*_tmp81,_tag_arr("",sizeof(
unsigned char),1))!= 0){prev_fields=({struct Cyc_List_List*_tmp85=_region_malloc(
uprev_rgn,sizeof(struct Cyc_List_List));_tmp85->hd=_tmp81;_tmp85->tl=prev_fields;
_tmp85;});}Cyc_Tcutil_check_type(loc,te,tvs,(void*)1,0,_tmp7F);Cyc_Tcutil_check_bitfield(
loc,te,_tmp7F,_tmp7E,_tmp81);}};_pop_region(uprev_rgn);}static void Cyc_Tc_tcStructOrUniondecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct _tagged_arr obj,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Structdecl*sd){struct _tagged_arr*_tmp86=(*((struct _tuple0*)((
struct Cyc_Core_Opt*)_check_null(sd->name))->v)).f2;if(sd->name == 0){({struct Cyc_Std_String_pa_struct
_tmp88;_tmp88.tag=0;_tmp88.f1=(struct _tagged_arr)obj;{void*_tmp87[1]={& _tmp88};
Cyc_Tcutil_terr(loc,_tag_arr("anonymous %ss are not allowed at top level",
sizeof(unsigned char),43),_tag_arr(_tmp87,sizeof(void*),1));}});return;}{struct
Cyc_List_List*atts=sd->attributes;for(0;atts != 0;atts=atts->tl){void*_tmp89=(
void*)atts->hd;_LL98: if((int)_tmp89 == 5){goto _LL99;}else{goto _LL9A;}_LL9A: if((
unsigned int)_tmp89 > 16?*((int*)_tmp89)== 1: 0){goto _LL9B;}else{goto _LL9C;}_LL9C:
goto _LL9D;_LL99: continue;_LL9B: continue;_LL9D:({struct Cyc_Std_String_pa_struct
_tmp8D;_tmp8D.tag=0;_tmp8D.f1=(struct _tagged_arr)*_tmp86;{struct Cyc_Std_String_pa_struct
_tmp8C;_tmp8C.tag=0;_tmp8C.f1=(struct _tagged_arr)obj;{struct Cyc_Std_String_pa_struct
_tmp8B;_tmp8B.tag=0;_tmp8B.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp8A[3]={& _tmp8B,& _tmp8C,& _tmp8D};Cyc_Tcutil_terr(loc,
_tag_arr("bad attribute %s in %s %s definition",sizeof(unsigned char),37),
_tag_arr(_tmp8A,sizeof(void*),3));}}}});goto _LL97;_LL97:;}}{struct Cyc_List_List*
tvs=sd->tvs;for(0;tvs != 0;tvs=tvs->tl){void*_tmp8E=Cyc_Absyn_compress_kb((void*)((
struct Cyc_Absyn_Tvar*)tvs->hd)->kind);struct Cyc_Core_Opt*_tmp8F;struct Cyc_Core_Opt**
_tmp90;void*_tmp91;struct Cyc_Core_Opt*_tmp92;struct Cyc_Core_Opt**_tmp93;void*
_tmp94;struct Cyc_Core_Opt*_tmp95;struct Cyc_Core_Opt**_tmp96;void*_tmp97;_LL9F:
if(*((int*)_tmp8E)== 1){_LLA9: _tmp8F=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8E)->f1;
_tmp90=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8E)->f1;
goto _LLA0;}else{goto _LLA1;}_LLA1: if(*((int*)_tmp8E)== 2){_LLAB: _tmp92=((struct
Cyc_Absyn_Less_kb_struct*)_tmp8E)->f1;_tmp93=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E)->f1;goto _LLAA;_LLAA: _tmp91=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E)->f2;if((int)_tmp91 == 1){goto _LLA2;}else{goto _LLA3;}}else{goto _LLA3;}
_LLA3: if(*((int*)_tmp8E)== 2){_LLAD: _tmp95=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E)->f1;_tmp96=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E)->f1;goto _LLAC;_LLAC: _tmp94=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E)->f2;if((int)_tmp94 == 0){goto _LLA4;}else{goto _LLA5;}}else{goto _LLA5;}
_LLA5: if(*((int*)_tmp8E)== 0){_LLAE: _tmp97=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp8E)->f1;if((int)_tmp97 == 1){goto _LLA6;}else{goto _LLA7;}}else{goto _LLA7;}
_LLA7: goto _LLA8;_LLA0: _tmp93=_tmp90;goto _LLA2;_LLA2: _tmp96=_tmp93;goto _LLA4;
_LLA4:*_tmp96=({struct Cyc_Core_Opt*_tmp98=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp98->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp99=_cycalloc(sizeof(
struct Cyc_Absyn_Eq_kb_struct));_tmp99[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp9A;
_tmp9A.tag=0;_tmp9A.f1=(void*)((void*)2);_tmp9A;});_tmp99;}));_tmp98;});
continue;_LLA6:({struct Cyc_Std_String_pa_struct _tmp9E;_tmp9E.tag=0;_tmp9E.f1=(
struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name;{struct Cyc_Std_String_pa_struct
_tmp9D;_tmp9D.tag=0;_tmp9D.f1=(struct _tagged_arr)*_tmp86;{struct Cyc_Std_String_pa_struct
_tmp9C;_tmp9C.tag=0;_tmp9C.f1=(struct _tagged_arr)obj;{void*_tmp9B[3]={& _tmp9C,&
_tmp9D,& _tmp9E};Cyc_Tcutil_terr(loc,_tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof(unsigned char),54),_tag_arr(_tmp9B,sizeof(void*),3));}}}});continue;_LLA8:
continue;_LL9E:;}}{void*_tmp9F=(*((struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(sd->name))->v)).f1;struct Cyc_List_List*_tmpA0;struct Cyc_List_List*
_tmpA1;_LLB0: if((unsigned int)_tmp9F > 1?*((int*)_tmp9F)== 0: 0){_LLB6: _tmpA0=((
struct Cyc_Absyn_Rel_n_struct*)_tmp9F)->f1;if(_tmpA0 == 0){goto _LLB1;}else{goto
_LLB2;}}else{goto _LLB2;}_LLB2: if((unsigned int)_tmp9F > 1?*((int*)_tmp9F)== 1: 0){
_LLB7: _tmpA1=((struct Cyc_Absyn_Abs_n_struct*)_tmp9F)->f1;if(_tmpA1 == 0){goto
_LLB3;}else{goto _LLB4;}}else{goto _LLB4;}_LLB4: goto _LLB5;_LLB1: goto _LLAF;_LLB3:
goto _LLAF;_LLB5:({struct Cyc_Std_String_pa_struct _tmpA3;_tmpA3.tag=0;_tmpA3.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string((struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(sd->name))->v);{void*_tmpA2[1]={& _tmpA3};Cyc_Tcutil_terr(loc,
_tag_arr("qualified struct declarations are not implemented (%s)",sizeof(
unsigned char),55),_tag_arr(_tmpA2,sizeof(void*),1));}});return;_LLAF:;}(*((
struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(sd->name))->v)).f1=(void*)({
struct Cyc_Absyn_Abs_n_struct*_tmpA4=_cycalloc(sizeof(struct Cyc_Absyn_Abs_n_struct));
_tmpA4[0]=({struct Cyc_Absyn_Abs_n_struct _tmpA5;_tmpA5.tag=1;_tmpA5.f1=te->ns;
_tmpA5;});_tmpA4;});Cyc_Tcutil_check_unique_tvars(loc,sd->tvs);Cyc_Tcutil_add_tvar_identities(
sd->tvs);}struct _tuple5{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tc_tcStructdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Structdecl*sd){struct _tagged_arr*_tmpA6=(*((struct _tuple0*)((
struct Cyc_Core_Opt*)_check_null(sd->name))->v)).f2;const unsigned char*_tmpA7="struct";
Cyc_Tc_tcStructOrUniondecl(te,ge,_tag_arr(_tmpA7,sizeof(unsigned char),7),loc,sd);{
struct Cyc_List_List*_tmpA8=sd->tvs;{struct _tuple5 _tmpAA=({struct _tuple5 _tmpA9;
_tmpA9.f1=sd->fields;_tmpA9.f2=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup_opt)(ge->structdecls,_tmpA6);_tmpA9;});
struct Cyc_Core_Opt*_tmpAB;struct Cyc_Core_Opt*_tmpAC;struct Cyc_Core_Opt*_tmpAD;
struct Cyc_Core_Opt*_tmpAE;struct Cyc_Core_Opt _tmpAF;struct Cyc_List_List*_tmpB0;
struct Cyc_Core_Opt*_tmpB1;struct Cyc_Core_Opt _tmpB2;struct Cyc_Absyn_Structdecl**
_tmpB3;struct Cyc_Core_Opt*_tmpB4;struct Cyc_Core_Opt _tmpB5;struct Cyc_List_List*
_tmpB6;struct Cyc_Core_Opt*_tmpB7;struct Cyc_Core_Opt _tmpB8;struct Cyc_Absyn_Structdecl**
_tmpB9;struct Cyc_Core_Opt*_tmpBA;_LLB9: _LLC2: _tmpAC=_tmpAA.f1;if(_tmpAC == 0){
goto _LLC1;}else{goto _LLBB;}_LLC1: _tmpAB=_tmpAA.f2;if(_tmpAB == 0){goto _LLBA;}
else{goto _LLBB;}_LLBB: _LLC4: _tmpAE=_tmpAA.f1;if(_tmpAE == 0){goto _LLBD;}else{
_tmpAF=*_tmpAE;_LLC5: _tmpB0=(struct Cyc_List_List*)_tmpAF.v;goto _LLC3;}_LLC3:
_tmpAD=_tmpAA.f2;if(_tmpAD == 0){goto _LLBC;}else{goto _LLBD;}_LLBD: _LLC8: _tmpB4=
_tmpAA.f1;if(_tmpB4 == 0){goto _LLBF;}else{_tmpB5=*_tmpB4;_LLC9: _tmpB6=(struct Cyc_List_List*)
_tmpB5.v;goto _LLC6;}_LLC6: _tmpB1=_tmpAA.f2;if(_tmpB1 == 0){goto _LLBF;}else{_tmpB2=*
_tmpB1;_LLC7: _tmpB3=(struct Cyc_Absyn_Structdecl**)_tmpB2.v;goto _LLBE;}_LLBF:
_LLCC: _tmpBA=_tmpAA.f1;if(_tmpBA == 0){goto _LLCA;}else{goto _LLB8;}_LLCA: _tmpB7=
_tmpAA.f2;if(_tmpB7 == 0){goto _LLB8;}else{_tmpB8=*_tmpB7;_LLCB: _tmpB9=(struct Cyc_Absyn_Structdecl**)
_tmpB8.v;goto _LLC0;}_LLBA: ge->structdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Structdecl**v))Cyc_Dict_insert)(ge->structdecls,
_tmpA6,({struct Cyc_Absyn_Structdecl**_tmpBB=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl*));
_tmpBB[0]=sd;_tmpBB;}));goto _LLB8;_LLBC: {struct Cyc_Absyn_Structdecl**_tmpBC=({
struct Cyc_Absyn_Structdecl**_tmpBD=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl*));
_tmpBD[0]=({struct Cyc_Absyn_Structdecl*_tmpBE=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));
_tmpBE->sc=(void*)((void*)3);_tmpBE->name=sd->name;_tmpBE->tvs=_tmpA8;_tmpBE->fields=
0;_tmpBE->attributes=0;_tmpBE;});_tmpBD;});ge->structdecls=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Structdecl**v))Cyc_Dict_insert)(
ge->structdecls,_tmpA6,_tmpBC);Cyc_Tc_tcStructFields(te,ge,loc,_tag_arr(_tmpA7,
sizeof(unsigned char),7),_tmpB0,_tmpA8);*_tmpBC=sd;goto _LLB8;}_LLBE: {struct Cyc_Absyn_Structdecl*
_tmpBF=*_tmpB3;*_tmpB3=({struct Cyc_Absyn_Structdecl*_tmpC0=_cycalloc(sizeof(
struct Cyc_Absyn_Structdecl));_tmpC0->sc=(void*)((void*)3);_tmpC0->name=sd->name;
_tmpC0->tvs=_tmpA8;_tmpC0->fields=0;_tmpC0->attributes=0;_tmpC0;});Cyc_Tc_tcStructFields(
te,ge,loc,_tag_arr(_tmpA7,sizeof(unsigned char),7),_tmpB6,_tmpA8);*_tmpB3=_tmpBF;
_tmpB9=_tmpB3;goto _LLC0;}_LLC0: {struct Cyc_Absyn_Structdecl*_tmpC1=Cyc_Tcdecl_merge_structdecl(*
_tmpB9,sd,loc,Cyc_Tc_tc_msg);if(_tmpC1 == 0){return;}else{*_tmpB9=(struct Cyc_Absyn_Structdecl*)
_check_null(_tmpC1);sd=(struct Cyc_Absyn_Structdecl*)_check_null(_tmpC1);goto
_LLB8;}}_LLB8:;}ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct _tuple4*v))Cyc_Dict_insert)(ge->ordinaries,_tmpA6,({
struct _tuple4*_tmpC2=_cycalloc(sizeof(struct _tuple4));_tmpC2->f1=(void*)({struct
Cyc_Tcenv_StructRes_struct*_tmpC3=_cycalloc(sizeof(struct Cyc_Tcenv_StructRes_struct));
_tmpC3[0]=({struct Cyc_Tcenv_StructRes_struct _tmpC4;_tmpC4.tag=1;_tmpC4.f1=sd;
_tmpC4;});_tmpC3;});_tmpC2->f2=1;_tmpC2;}));}}void Cyc_Tc_tcUniondecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Uniondecl*
ud){struct _tagged_arr*_tmpC5=(*((struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(ud->name))->v)).f2;const unsigned char*_tmpC6="union";Cyc_Tc_tcStructOrUniondecl(
te,ge,_tag_arr(_tmpC6,sizeof(unsigned char),6),loc,(struct Cyc_Absyn_Structdecl*)
ud);{struct Cyc_List_List*_tmpC7=ud->tvs;struct _tuple5 _tmpC9=({struct _tuple5
_tmpC8;_tmpC8.f1=ud->fields;_tmpC8.f2=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)(ge->uniondecls,_tmpC5);_tmpC8;});
struct Cyc_Core_Opt*_tmpCA;struct Cyc_Core_Opt*_tmpCB;struct Cyc_Core_Opt*_tmpCC;
struct Cyc_Core_Opt*_tmpCD;struct Cyc_Core_Opt _tmpCE;struct Cyc_List_List*_tmpCF;
struct Cyc_Core_Opt*_tmpD0;struct Cyc_Core_Opt _tmpD1;struct Cyc_Absyn_Uniondecl**
_tmpD2;struct Cyc_Core_Opt*_tmpD3;struct Cyc_Core_Opt _tmpD4;struct Cyc_List_List*
_tmpD5;struct Cyc_Core_Opt*_tmpD6;struct Cyc_Core_Opt _tmpD7;struct Cyc_Absyn_Uniondecl**
_tmpD8;struct Cyc_Core_Opt*_tmpD9;_LLCE: _LLD7: _tmpCB=_tmpC9.f1;if(_tmpCB == 0){
goto _LLD6;}else{goto _LLD0;}_LLD6: _tmpCA=_tmpC9.f2;if(_tmpCA == 0){goto _LLCF;}
else{goto _LLD0;}_LLD0: _LLD9: _tmpCD=_tmpC9.f1;if(_tmpCD == 0){goto _LLD2;}else{
_tmpCE=*_tmpCD;_LLDA: _tmpCF=(struct Cyc_List_List*)_tmpCE.v;goto _LLD8;}_LLD8:
_tmpCC=_tmpC9.f2;if(_tmpCC == 0){goto _LLD1;}else{goto _LLD2;}_LLD2: _LLDD: _tmpD3=
_tmpC9.f1;if(_tmpD3 == 0){goto _LLD4;}else{_tmpD4=*_tmpD3;_LLDE: _tmpD5=(struct Cyc_List_List*)
_tmpD4.v;goto _LLDB;}_LLDB: _tmpD0=_tmpC9.f2;if(_tmpD0 == 0){goto _LLD4;}else{_tmpD1=*
_tmpD0;_LLDC: _tmpD2=(struct Cyc_Absyn_Uniondecl**)_tmpD1.v;goto _LLD3;}_LLD4: _LLE1:
_tmpD9=_tmpC9.f1;if(_tmpD9 == 0){goto _LLDF;}else{goto _LLCD;}_LLDF: _tmpD6=_tmpC9.f2;
if(_tmpD6 == 0){goto _LLCD;}else{_tmpD7=*_tmpD6;_LLE0: _tmpD8=(struct Cyc_Absyn_Uniondecl**)
_tmpD7.v;goto _LLD5;}_LLCF: ge->uniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Uniondecl**v))Cyc_Dict_insert)(ge->uniondecls,
_tmpC5,({struct Cyc_Absyn_Uniondecl**_tmpDA=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl*));
_tmpDA[0]=ud;_tmpDA;}));goto _LLCD;_LLD1: {struct Cyc_Absyn_Uniondecl**_tmpDB=({
struct Cyc_Absyn_Uniondecl**_tmpE0=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl*));
_tmpE0[0]=({struct Cyc_Absyn_Uniondecl*_tmpE1=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl));
_tmpE1->sc=(void*)((void*)3);_tmpE1->name=ud->name;_tmpE1->tvs=_tmpC7;_tmpE1->fields=
0;_tmpE1->attributes=ud->attributes;_tmpE1;});_tmpE0;});ge->uniondecls=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Uniondecl**
v))Cyc_Dict_insert)(ge->uniondecls,_tmpC5,_tmpDB);Cyc_Tc_tcStructFields(te,ge,
loc,_tag_arr(_tmpC6,sizeof(unsigned char),6),_tmpCF,_tmpC7);{struct Cyc_List_List*
f=_tmpCF;for(0;f != 0;f=f->tl){if(! Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Structfield*)
f->hd)->type)){({struct Cyc_Std_String_pa_struct _tmpDF;_tmpDF.tag=0;_tmpDF.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Absyn_Structfield*)f->hd)->type);{
struct Cyc_Std_String_pa_struct _tmpDE;_tmpDE.tag=0;_tmpDE.f1=(struct _tagged_arr)*
_tmpC5;{struct Cyc_Std_String_pa_struct _tmpDD;_tmpDD.tag=0;_tmpDD.f1=(struct
_tagged_arr)*((struct Cyc_Absyn_Structfield*)f->hd)->name;{void*_tmpDC[3]={&
_tmpDD,& _tmpDE,& _tmpDF};Cyc_Tcutil_terr(loc,_tag_arr("field %s of union %s has type %s which is not `bits-only'",
sizeof(unsigned char),58),_tag_arr(_tmpDC,sizeof(void*),3));}}}});}}}*_tmpDB=ud;
goto _LLCD;}_LLD3: {struct Cyc_Absyn_Uniondecl*_tmpE2=*_tmpD2;*_tmpD2=({struct Cyc_Absyn_Uniondecl*
_tmpE3=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl));_tmpE3->sc=(void*)((void*)3);
_tmpE3->name=ud->name;_tmpE3->tvs=_tmpC7;_tmpE3->fields=0;_tmpE3->attributes=ud->attributes;
_tmpE3;});Cyc_Tc_tcStructFields(te,ge,loc,_tag_arr(_tmpC6,sizeof(unsigned char),
6),_tmpD5,_tmpC7);*_tmpD2=_tmpE2;_tmpD8=_tmpD2;goto _LLD5;}_LLD5: {struct Cyc_Absyn_Uniondecl*
_tmpE4=Cyc_Tcdecl_merge_uniondecl(*_tmpD8,ud,loc,Cyc_Tc_tc_msg);if(_tmpE4 == 0){
return;}else{*_tmpD8=(struct Cyc_Absyn_Uniondecl*)_check_null(_tmpE4);goto _LLCD;}}
_LLCD:;}}struct _tuple6{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_List_List*
Cyc_Tc_tcTunionFields(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*
loc,struct _tagged_arr obj,int is_xtunion,struct _tuple0*name,struct Cyc_List_List*
fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tuniondecl*tudres){{struct Cyc_List_List*
_tmpE5=fields;for(0;_tmpE5 != 0;_tmpE5=_tmpE5->tl){struct Cyc_Absyn_Tunionfield*
_tmpE6=(struct Cyc_Absyn_Tunionfield*)_tmpE5->hd;{struct Cyc_List_List*tvs=_tmpE6->tvs;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpE7=(struct Cyc_Absyn_Tvar*)
tvs->hd;void*_tmpE8=Cyc_Absyn_compress_kb((void*)_tmpE7->kind);void*_tmpE9;
struct Cyc_Core_Opt*_tmpEA;struct Cyc_Core_Opt**_tmpEB;void*_tmpEC;struct Cyc_Core_Opt*
_tmpED;struct Cyc_Core_Opt**_tmpEE;void*_tmpEF;struct Cyc_Core_Opt*_tmpF0;struct
Cyc_Core_Opt**_tmpF1;_LLE3: if(*((int*)_tmpE8)== 0){_LLED: _tmpE9=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmpE8)->f1;if((int)_tmpE9 == 1){goto _LLE4;}else{goto
_LLE5;}}else{goto _LLE5;}_LLE5: if(*((int*)_tmpE8)== 1){_LLEE: _tmpEA=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpE8)->f1;_tmpEB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpE8)->f1;goto _LLE6;}else{goto _LLE7;}_LLE7: if(*((int*)_tmpE8)== 2){_LLF0:
_tmpED=((struct Cyc_Absyn_Less_kb_struct*)_tmpE8)->f1;_tmpEE=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmpE8)->f1;goto _LLEF;_LLEF: _tmpEC=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmpE8)->f2;if((int)_tmpEC == 1){goto _LLE8;}else{
goto _LLE9;}}else{goto _LLE9;}_LLE9: if(*((int*)_tmpE8)== 2){_LLF2: _tmpF0=((struct
Cyc_Absyn_Less_kb_struct*)_tmpE8)->f1;_tmpF1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpE8)->f1;goto _LLF1;_LLF1: _tmpEF=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpE8)->f2;if((int)_tmpEF == 0){goto _LLEA;}else{goto _LLEB;}}else{goto _LLEB;}
_LLEB: goto _LLEC;_LLE4:({struct Cyc_Std_String_pa_struct _tmpF3;_tmpF3.tag=0;_tmpF3.f1=(
struct _tagged_arr)*(*_tmpE6->name).f2;{void*_tmpF2[1]={& _tmpF3};Cyc_Tcutil_terr(
_tmpE6->loc,_tag_arr("field %s abstracts type variable of kind M",sizeof(
unsigned char),43),_tag_arr(_tmpF2,sizeof(void*),1));}});goto _LLE2;_LLE6: _tmpEE=
_tmpEB;goto _LLE8;_LLE8: _tmpF1=_tmpEE;goto _LLEA;_LLEA:*_tmpF1=({struct Cyc_Core_Opt*
_tmpF4=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpF4->v=(void*)((void*)({struct
Cyc_Absyn_Eq_kb_struct*_tmpF5=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));
_tmpF5[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpF6;_tmpF6.tag=0;_tmpF6.f1=(void*)((
void*)2);_tmpF6;});_tmpF5;}));_tmpF4;});goto _LLE2;_LLEC: goto _LLE2;_LLE2:;}}{
struct Cyc_List_List*alltvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpE6->tvs);Cyc_Tcutil_check_unique_tvars(
loc,alltvs);Cyc_Tcutil_add_tvar_identities(_tmpE6->tvs);{struct Cyc_List_List*
typs=_tmpE6->typs;for(0;typs != 0;typs=typs->tl){Cyc_Tcutil_check_type(_tmpE6->loc,
te,alltvs,(void*)1,0,(*((struct _tuple6*)typs->hd)).f2);}}{void*_tmpF7=(*_tmpE6->name).f1;
struct Cyc_List_List*_tmpF8;_LLF4: if((unsigned int)_tmpF7 > 1?*((int*)_tmpF7)== 0:
0){_LLFC: _tmpF8=((struct Cyc_Absyn_Rel_n_struct*)_tmpF7)->f1;if(_tmpF8 == 0){goto
_LLF5;}else{goto _LLF6;}}else{goto _LLF6;}_LLF6: if((unsigned int)_tmpF7 > 1?*((int*)
_tmpF7)== 0: 0){goto _LLF7;}else{goto _LLF8;}_LLF8: if((unsigned int)_tmpF7 > 1?*((
int*)_tmpF7)== 1: 0){goto _LLF9;}else{goto _LLFA;}_LLFA: if((int)_tmpF7 == 0){goto
_LLFB;}else{goto _LLF3;}_LLF5: if(is_xtunion){(*_tmpE6->name).f1=(void*)({struct
Cyc_Absyn_Abs_n_struct*_tmpF9=_cycalloc(sizeof(struct Cyc_Absyn_Abs_n_struct));
_tmpF9[0]=({struct Cyc_Absyn_Abs_n_struct _tmpFA;_tmpFA.tag=1;_tmpFA.f1=te->ns;
_tmpFA;});_tmpF9;});}else{(*_tmpE6->name).f1=(*name).f1;}goto _LLF3;_LLF7:({
struct Cyc_Std_String_pa_struct _tmpFC;_tmpFC.tag=0;_tmpFC.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(_tmpE6->name);{void*_tmpFB[1]={& _tmpFC};Cyc_Tcutil_terr(
_tmpE6->loc,_tag_arr("qualified tunionfield declarations are not allowed (%s)",
sizeof(unsigned char),56),_tag_arr(_tmpFB,sizeof(void*),1));}});goto _LLF3;_LLF9:
goto _LLF3;_LLFB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpFD=
_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmpFD[0]=({struct Cyc_Core_Impossible_struct
_tmpFE;_tmpFE.tag=Cyc_Core_Impossible;_tmpFE.f1=_tag_arr("tcTunionFields: Loc_n",
sizeof(unsigned char),22);_tmpFE;});_tmpFD;}));goto _LLF3;_LLF3:;}}}}{struct Cyc_List_List*
fields2;if(is_xtunion){int _tmpFF=1;struct Cyc_List_List*_tmp100=Cyc_Tcdecl_sort_xtunion_fields(
fields,& _tmpFF,(*name).f2,loc,Cyc_Tc_tc_msg);if(_tmpFF){fields2=_tmp100;}else{
fields2=0;}}else{struct _RegionHandle _tmp101=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmp101;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Tunionfield*
_tmp102=(struct Cyc_Absyn_Tunionfield*)fs->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_zstrptrcmp,prev_fields,(*_tmp102->name).f2)){({struct Cyc_Std_String_pa_struct
_tmp105;_tmp105.tag=0;_tmp105.f1=(struct _tagged_arr)obj;{struct Cyc_Std_String_pa_struct
_tmp104;_tmp104.tag=0;_tmp104.f1=(struct _tagged_arr)*(*_tmp102->name).f2;{void*
_tmp103[2]={& _tmp104,& _tmp105};Cyc_Tcutil_terr(_tmp102->loc,_tag_arr("duplicate field name %s in %s",
sizeof(unsigned char),30),_tag_arr(_tmp103,sizeof(void*),2));}}});}else{
prev_fields=({struct Cyc_List_List*_tmp106=_region_malloc(uprev_rgn,sizeof(struct
Cyc_List_List));_tmp106->hd=(*_tmp102->name).f2;_tmp106->tl=prev_fields;_tmp106;});}
if((void*)_tmp102->sc != (void*)2){({struct Cyc_Std_String_pa_struct _tmp108;
_tmp108.tag=0;_tmp108.f1=(struct _tagged_arr)*(*_tmp102->name).f2;{void*_tmp107[1]={&
_tmp108};Cyc_Tcutil_warn(loc,_tag_arr("ignoring scope of field %s",sizeof(
unsigned char),27),_tag_arr(_tmp107,sizeof(void*),1));}});(void*)(_tmp102->sc=(
void*)((void*)2));}}}fields2=fields;};_pop_region(uprev_rgn);}{struct Cyc_List_List*
_tmp109=fields;for(0;_tmp109 != 0;_tmp109=_tmp109->tl){struct Cyc_Absyn_Tunionfield*
_tmp10A=(struct Cyc_Absyn_Tunionfield*)_tmp109->hd;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp10A->name).f2,({struct _tuple4*_tmp10B=_cycalloc(sizeof(struct _tuple4));
_tmp10B->f1=(void*)({struct Cyc_Tcenv_TunionRes_struct*_tmp10C=_cycalloc(sizeof(
struct Cyc_Tcenv_TunionRes_struct));_tmp10C[0]=({struct Cyc_Tcenv_TunionRes_struct
_tmp10D;_tmp10D.tag=2;_tmp10D.f1=tudres;_tmp10D.f2=_tmp10A;_tmp10D;});_tmp10C;});
_tmp10B->f2=1;_tmp10B;}));}}return fields2;}}void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl*
tud){struct _tagged_arr*v=(*tud->name).f2;struct _tagged_arr obj;if(tud->is_xtunion){
obj=_tag_arr("xtunion",sizeof(unsigned char),8);}else{obj=_tag_arr("tunion",
sizeof(unsigned char),7);}{struct Cyc_List_List*tvs=tud->tvs;{struct Cyc_List_List*
tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){void*_tmp10E=Cyc_Absyn_compress_kb((void*)((
struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp10F;struct Cyc_Core_Opt**
_tmp110;void*_tmp111;struct Cyc_Core_Opt*_tmp112;struct Cyc_Core_Opt**_tmp113;void*
_tmp114;struct Cyc_Core_Opt*_tmp115;struct Cyc_Core_Opt**_tmp116;void*_tmp117;
_LLFE: if(*((int*)_tmp10E)== 1){_LL108: _tmp10F=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp10E)->f1;_tmp110=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp10E)->f1;goto _LLFF;}else{goto _LL100;}_LL100: if(*((int*)_tmp10E)== 2){_LL10A:
_tmp112=((struct Cyc_Absyn_Less_kb_struct*)_tmp10E)->f1;_tmp113=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp10E)->f1;goto _LL109;_LL109: _tmp111=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp10E)->f2;if((int)_tmp111 == 1){goto _LL101;}
else{goto _LL102;}}else{goto _LL102;}_LL102: if(*((int*)_tmp10E)== 2){_LL10C:
_tmp115=((struct Cyc_Absyn_Less_kb_struct*)_tmp10E)->f1;_tmp116=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp10E)->f1;goto _LL10B;_LL10B: _tmp114=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp10E)->f2;if((int)_tmp114 == 0){goto _LL103;}
else{goto _LL104;}}else{goto _LL104;}_LL104: if(*((int*)_tmp10E)== 0){_LL10D:
_tmp117=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp10E)->f1;if((int)_tmp117 == 1){
goto _LL105;}else{goto _LL106;}}else{goto _LL106;}_LL106: goto _LL107;_LLFF: _tmp113=
_tmp110;goto _LL101;_LL101: _tmp116=_tmp113;goto _LL103;_LL103:*_tmp116=({struct Cyc_Core_Opt*
_tmp118=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp118->v=(void*)((void*)({struct
Cyc_Absyn_Eq_kb_struct*_tmp119=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));
_tmp119[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp11A;_tmp11A.tag=0;_tmp11A.f1=(void*)((
void*)2);_tmp11A;});_tmp119;}));_tmp118;});goto _LLFD;_LL105:({struct Cyc_Std_String_pa_struct
_tmp11E;_tmp11E.tag=0;_tmp11E.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)
tvs2->hd)->name;{struct Cyc_Std_String_pa_struct _tmp11D;_tmp11D.tag=0;_tmp11D.f1=(
struct _tagged_arr)*v;{struct Cyc_Std_String_pa_struct _tmp11C;_tmp11C.tag=0;
_tmp11C.f1=(struct _tagged_arr)obj;{void*_tmp11B[3]={& _tmp11C,& _tmp11D,& _tmp11E};
Cyc_Tcutil_terr(loc,_tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof(unsigned char),54),_tag_arr(_tmp11B,sizeof(void*),3));}}}});goto _LLFD;
_LL107: goto _LLFD;_LLFD:;}}Cyc_Tcutil_check_unique_tvars(loc,tvs);Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_Core_Opt*tud_opt;if(tud->is_xtunion){{struct _handler_cons _tmp11F;
_push_handler(& _tmp11F);{int _tmp121=0;if(setjmp(_tmp11F.handler)){_tmp121=1;}if(
! _tmp121){tud_opt=Cyc_Tcenv_lookup_xtuniondecl(te,loc,tud->name);;_pop_handler();}
else{void*_tmp120=(void*)_exn_thrown;void*_tmp123=_tmp120;_LL10F: if(_tmp123 == 
Cyc_Dict_Absent){goto _LL110;}else{goto _LL111;}_LL111: goto _LL112;_LL110:({struct
Cyc_Std_String_pa_struct _tmp125;_tmp125.tag=0;_tmp125.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
tud->name);{void*_tmp124[1]={& _tmp125};Cyc_Tcutil_terr(loc,_tag_arr("qualified xtunion declaration %s is not an existing xtunion",
sizeof(unsigned char),60),_tag_arr(_tmp124,sizeof(void*),1));}});return;_LL112:(
void)_throw(_tmp123);_LL10E:;}}}if(tud_opt != 0){tud->name=(*((struct Cyc_Absyn_Tuniondecl**)
tud_opt->v))->name;}else{(*tud->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp126=_cycalloc(sizeof(struct Cyc_Absyn_Abs_n_struct));_tmp126[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp127;_tmp127.tag=1;_tmp127.f1=te->ns;_tmp127;});_tmp126;});}}else{{void*
_tmp128=(*tud->name).f1;struct Cyc_List_List*_tmp129;_LL114: if((unsigned int)
_tmp128 > 1?*((int*)_tmp128)== 0: 0){_LL11A: _tmp129=((struct Cyc_Absyn_Rel_n_struct*)
_tmp128)->f1;if(_tmp129 == 0){goto _LL115;}else{goto _LL116;}}else{goto _LL116;}
_LL116: if((unsigned int)_tmp128 > 1?*((int*)_tmp128)== 1: 0){goto _LL117;}else{goto
_LL118;}_LL118: goto _LL119;_LL115:(*tud->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp12A=_cycalloc(sizeof(struct Cyc_Absyn_Abs_n_struct));_tmp12A[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp12B;_tmp12B.tag=1;_tmp12B.f1=te->ns;_tmp12B;});_tmp12A;});goto _LL113;_LL117:
goto _LL119;_LL119:({struct Cyc_Std_String_pa_struct _tmp12D;_tmp12D.tag=0;_tmp12D.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(tud->name);{void*_tmp12C[1]={& _tmp12D};
Cyc_Tcutil_terr(loc,_tag_arr("qualified tunion declarations are not implemented (%s)",
sizeof(unsigned char),55),_tag_arr(_tmp12C,sizeof(void*),1));}});return;_LL113:;}
tud_opt=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)(
ge->tuniondecls,v);}{struct _tuple5 _tmp12F=({struct _tuple5 _tmp12E;_tmp12E.f1=tud->fields;
_tmp12E.f2=tud_opt;_tmp12E;});struct Cyc_Core_Opt*_tmp130;struct Cyc_Core_Opt*
_tmp131;struct Cyc_Core_Opt*_tmp132;struct Cyc_Core_Opt*_tmp133;struct Cyc_Core_Opt
_tmp134;struct Cyc_List_List*_tmp135;struct Cyc_List_List**_tmp136;struct Cyc_Core_Opt*
_tmp137;struct Cyc_Core_Opt _tmp138;struct Cyc_Absyn_Tuniondecl**_tmp139;struct Cyc_Core_Opt*
_tmp13A;struct Cyc_Core_Opt _tmp13B;struct Cyc_List_List*_tmp13C;struct Cyc_List_List**
_tmp13D;struct Cyc_Core_Opt*_tmp13E;struct Cyc_Core_Opt _tmp13F;struct Cyc_Absyn_Tuniondecl**
_tmp140;struct Cyc_Core_Opt*_tmp141;_LL11C: _LL125: _tmp131=_tmp12F.f1;if(_tmp131 == 
0){goto _LL124;}else{goto _LL11E;}_LL124: _tmp130=_tmp12F.f2;if(_tmp130 == 0){goto
_LL11D;}else{goto _LL11E;}_LL11E: _LL127: _tmp133=_tmp12F.f1;if(_tmp133 == 0){goto
_LL120;}else{_tmp134=*_tmp133;_LL128: _tmp135=(struct Cyc_List_List*)_tmp134.v;
_tmp136=(struct Cyc_List_List**)&(*_tmp12F.f1).v;goto _LL126;}_LL126: _tmp132=
_tmp12F.f2;if(_tmp132 == 0){goto _LL11F;}else{goto _LL120;}_LL120: _LL12B: _tmp13A=
_tmp12F.f1;if(_tmp13A == 0){goto _LL122;}else{_tmp13B=*_tmp13A;_LL12C: _tmp13C=(
struct Cyc_List_List*)_tmp13B.v;_tmp13D=(struct Cyc_List_List**)&(*_tmp12F.f1).v;
goto _LL129;}_LL129: _tmp137=_tmp12F.f2;if(_tmp137 == 0){goto _LL122;}else{_tmp138=*
_tmp137;_LL12A: _tmp139=(struct Cyc_Absyn_Tuniondecl**)_tmp138.v;goto _LL121;}
_LL122: _LL12F: _tmp141=_tmp12F.f1;if(_tmp141 == 0){goto _LL12D;}else{goto _LL11B;}
_LL12D: _tmp13E=_tmp12F.f2;if(_tmp13E == 0){goto _LL11B;}else{_tmp13F=*_tmp13E;
_LL12E: _tmp140=(struct Cyc_Absyn_Tuniondecl**)_tmp13F.v;goto _LL123;}_LL11D: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl**
v))Cyc_Dict_insert)(ge->tuniondecls,v,({struct Cyc_Absyn_Tuniondecl**_tmp142=
_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl*));_tmp142[0]=tud;_tmp142;}));goto
_LL11B;_LL11F: {struct Cyc_Absyn_Tuniondecl**_tmp143=({struct Cyc_Absyn_Tuniondecl**
_tmp144=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl*));_tmp144[0]=({struct Cyc_Absyn_Tuniondecl*
_tmp145=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));_tmp145->sc=(void*)((void*)
3);_tmp145->name=tud->name;_tmp145->tvs=tvs;_tmp145->fields=0;_tmp145->is_xtunion=
tud->is_xtunion;_tmp145;});_tmp144;});ge->tuniondecls=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl**v))Cyc_Dict_insert)(
ge->tuniondecls,v,_tmp143);*_tmp136=Cyc_Tc_tcTunionFields(te,ge,loc,obj,tud->is_xtunion,
tud->name,*_tmp136,tvs,tud);*_tmp143=tud;goto _LL11B;}_LL121: {struct Cyc_Absyn_Tuniondecl*
_tmp146=*_tmp139;*_tmp139=({struct Cyc_Absyn_Tuniondecl*_tmp147=_cycalloc(sizeof(
struct Cyc_Absyn_Tuniondecl));_tmp147->sc=(void*)((void*)3);_tmp147->name=tud->name;
_tmp147->tvs=tvs;_tmp147->fields=0;_tmp147->is_xtunion=tud->is_xtunion;_tmp147;});*
_tmp13D=Cyc_Tc_tcTunionFields(te,ge,loc,obj,tud->is_xtunion,tud->name,*_tmp13D,
tvs,tud);*_tmp139=_tmp146;_tmp140=_tmp139;goto _LL123;}_LL123: {struct Cyc_Absyn_Tuniondecl*
_tmp148=Cyc_Tcdecl_merge_tuniondecl(*_tmp140,tud,loc,Cyc_Tc_tc_msg);if(_tmp148 == 
0){return;}else{*_tmp140=(struct Cyc_Absyn_Tuniondecl*)_check_null(_tmp148);goto
_LL11B;}}_LL11B:;}}}}void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Enumdecl*ed){struct _tagged_arr*
v=(*ed->name).f2;{void*_tmp149=(*ed->name).f1;struct Cyc_List_List*_tmp14A;struct
Cyc_List_List*_tmp14B;_LL131: if((unsigned int)_tmp149 > 1?*((int*)_tmp149)== 0: 0){
_LL137: _tmp14A=((struct Cyc_Absyn_Rel_n_struct*)_tmp149)->f1;if(_tmp14A == 0){goto
_LL132;}else{goto _LL133;}}else{goto _LL133;}_LL133: if((unsigned int)_tmp149 > 1?*((
int*)_tmp149)== 1: 0){_LL138: _tmp14B=((struct Cyc_Absyn_Abs_n_struct*)_tmp149)->f1;
if(_tmp14B == 0){goto _LL134;}else{goto _LL135;}}else{goto _LL135;}_LL135: goto _LL136;
_LL132: goto _LL130;_LL134: goto _LL130;_LL136:({struct Cyc_Std_String_pa_struct
_tmp14D;_tmp14D.tag=0;_tmp14D.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(ed->name);{
void*_tmp14C[1]={& _tmp14D};Cyc_Tcutil_terr(loc,_tag_arr("qualified enum declarations are not implemented (%s)",
sizeof(unsigned char),53),_tag_arr(_tmp14C,sizeof(void*),1));}});return;_LL130:;}(*
ed->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp14E=_cycalloc(sizeof(
struct Cyc_Absyn_Abs_n_struct));_tmp14E[0]=({struct Cyc_Absyn_Abs_n_struct _tmp14F;
_tmp14F.tag=1;_tmp14F.f1=te->ns;_tmp14F;});_tmp14E;});if(ed->fields != 0){struct
_RegionHandle _tmp150=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=&
_tmp150;_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int
tag_count=0;struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp151=(struct Cyc_Absyn_Enumfield*)fs->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_zstrptrcmp,prev_fields,(*_tmp151->name).f2)){({struct Cyc_Std_String_pa_struct
_tmp153;_tmp153.tag=0;_tmp153.f1=(struct _tagged_arr)*(*_tmp151->name).f2;{void*
_tmp152[1]={& _tmp153};Cyc_Tcutil_terr(_tmp151->loc,_tag_arr("duplicate field name %s",
sizeof(unsigned char),24),_tag_arr(_tmp152,sizeof(void*),1));}});}else{
prev_fields=({struct Cyc_List_List*_tmp154=_region_malloc(uprev_rgn,sizeof(struct
Cyc_List_List));_tmp154->hd=(*_tmp151->name).f2;_tmp154->tl=prev_fields;_tmp154;});}
if(_tmp151->tag == 0){_tmp151->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp151->loc);}else{if(! Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp151->tag))){({struct Cyc_Std_String_pa_struct _tmp157;_tmp157.tag=
0;_tmp157.f1=(struct _tagged_arr)*(*_tmp151->name).f2;{struct Cyc_Std_String_pa_struct
_tmp156;_tmp156.tag=0;_tmp156.f1=(struct _tagged_arr)*v;{void*_tmp155[2]={&
_tmp156,& _tmp157};Cyc_Tcutil_terr(loc,_tag_arr("enum %s, field %s: expression is not constant",
sizeof(unsigned char),46),_tag_arr(_tmp155,sizeof(void*),2));}}});}}{
unsigned int t1=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp151->tag));tag_count=t1 + 1;(*_tmp151->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp158=_cycalloc(sizeof(struct Cyc_Absyn_Abs_n_struct));_tmp158[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp159;_tmp159.tag=1;_tmp159.f1=te->ns;_tmp159;});_tmp158;});}}};_pop_region(
uprev_rgn);}{struct _handler_cons _tmp15A;_push_handler(& _tmp15A);{int _tmp15C=0;
if(setjmp(_tmp15A.handler)){_tmp15C=1;}if(! _tmp15C){{struct Cyc_Absyn_Enumdecl**
_tmp15D=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*
k))Cyc_Dict_lookup)(ge->enumdecls,v);struct Cyc_Absyn_Enumdecl*_tmp15E=Cyc_Tcdecl_merge_enumdecl(*
_tmp15D,ed,loc,Cyc_Tc_tc_msg);if(_tmp15E == 0){_npop_handler(0);return;}*_tmp15D=(
struct Cyc_Absyn_Enumdecl*)_check_null(_tmp15E);};_pop_handler();}else{void*
_tmp15B=(void*)_exn_thrown;void*_tmp160=_tmp15B;_LL13A: if(_tmp160 == Cyc_Dict_Absent){
goto _LL13B;}else{goto _LL13C;}_LL13C: goto _LL13D;_LL13B: {struct Cyc_Absyn_Enumdecl**
_tmp161=({struct Cyc_Absyn_Enumdecl**_tmp162=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl*));
_tmp162[0]=ed;_tmp162;});ge->enumdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,
v,_tmp161);goto _LL139;}_LL13D:(void)_throw(_tmp160);_LL139:;}}}if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*_tmp163=(
struct Cyc_Absyn_Enumfield*)fs->hd;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp163->name).f2,({struct _tuple4*_tmp164=_cycalloc(sizeof(struct _tuple4));
_tmp164->f1=(void*)({struct Cyc_Tcenv_EnumRes_struct*_tmp165=_cycalloc(sizeof(
struct Cyc_Tcenv_EnumRes_struct));_tmp165[0]=({struct Cyc_Tcenv_EnumRes_struct
_tmp166;_tmp166.tag=3;_tmp166.f1=ed;_tmp166.f2=_tmp163;_tmp166;});_tmp165;});
_tmp164->f2=1;_tmp164;}));}}}static int Cyc_Tc_okay_externC(struct Cyc_Position_Segment*
loc,void*sc){void*_tmp167=sc;_LL13F: if((int)_tmp167 == 0){goto _LL140;}else{goto
_LL141;}_LL141: if((int)_tmp167 == 1){goto _LL142;}else{goto _LL143;}_LL143: if((int)
_tmp167 == 2){goto _LL144;}else{goto _LL145;}_LL145: if((int)_tmp167 == 3){goto _LL146;}
else{goto _LL147;}_LL147: if((int)_tmp167 == 4){goto _LL148;}else{goto _LL13E;}_LL140:({
void*_tmp168[0]={};Cyc_Tcutil_warn(loc,_tag_arr("static declaration nested within extern \"C\"",
sizeof(unsigned char),44),_tag_arr(_tmp168,sizeof(void*),0));});return 0;_LL142:({
void*_tmp169[0]={};Cyc_Tcutil_warn(loc,_tag_arr("abstract declaration nested within extern \"C\"",
sizeof(unsigned char),46),_tag_arr(_tmp169,sizeof(void*),0));});return 0;_LL144:
return 1;_LL146: return 1;_LL148:({void*_tmp16A[0]={};Cyc_Tcutil_warn(loc,_tag_arr("nested extern \"C\" declaration",
sizeof(unsigned char),30),_tag_arr(_tmp16A,sizeof(void*),0));});return 1;_LL13E:;}
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int
in_externC,int check_var_init){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;struct Cyc_Dict_Dict*dict=Cyc_Tcgenrep_empty_typerep_dict();
struct Cyc_List_List*_tmp16B=ds0;for(0;_tmp16B != 0;(last=_tmp16B,_tmp16B=_tmp16B->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp16B->hd;struct Cyc_Position_Segment*
loc=d->loc;void*_tmp16C=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp16D;struct Cyc_Absyn_Fndecl*
_tmp16E;struct Cyc_Absyn_Typedefdecl*_tmp16F;struct Cyc_Absyn_Structdecl*_tmp170;
struct Cyc_Absyn_Uniondecl*_tmp171;struct Cyc_Absyn_Tuniondecl*_tmp172;struct Cyc_Absyn_Enumdecl*
_tmp173;struct Cyc_List_List*_tmp174;struct _tagged_arr*_tmp175;struct Cyc_List_List*
_tmp176;struct _tuple0*_tmp177;struct _tuple0 _tmp178;struct _tagged_arr*_tmp179;
void*_tmp17A;struct Cyc_List_List*_tmp17B;_LL14A: if(*((int*)_tmp16C)== 2){goto
_LL14B;}else{goto _LL14C;}_LL14C: if(*((int*)_tmp16C)== 3){goto _LL14D;}else{goto
_LL14E;}_LL14E: if(*((int*)_tmp16C)== 0){_LL162: _tmp16D=((struct Cyc_Absyn_Var_d_struct*)
_tmp16C)->f1;goto _LL14F;}else{goto _LL150;}_LL150: if(*((int*)_tmp16C)== 1){_LL163:
_tmp16E=((struct Cyc_Absyn_Fn_d_struct*)_tmp16C)->f1;goto _LL151;}else{goto _LL152;}
_LL152: if(*((int*)_tmp16C)== 8){_LL164: _tmp16F=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp16C)->f1;goto _LL153;}else{goto _LL154;}_LL154: if(*((int*)_tmp16C)== 4){_LL165:
_tmp170=((struct Cyc_Absyn_Struct_d_struct*)_tmp16C)->f1;goto _LL155;}else{goto
_LL156;}_LL156: if(*((int*)_tmp16C)== 5){_LL166: _tmp171=((struct Cyc_Absyn_Union_d_struct*)
_tmp16C)->f1;goto _LL157;}else{goto _LL158;}_LL158: if(*((int*)_tmp16C)== 6){_LL167:
_tmp172=((struct Cyc_Absyn_Tunion_d_struct*)_tmp16C)->f1;goto _LL159;}else{goto
_LL15A;}_LL15A: if(*((int*)_tmp16C)== 7){_LL168: _tmp173=((struct Cyc_Absyn_Enum_d_struct*)
_tmp16C)->f1;goto _LL15B;}else{goto _LL15C;}_LL15C: if(*((int*)_tmp16C)== 9){_LL16A:
_tmp175=((struct Cyc_Absyn_Namespace_d_struct*)_tmp16C)->f1;goto _LL169;_LL169:
_tmp174=((struct Cyc_Absyn_Namespace_d_struct*)_tmp16C)->f2;goto _LL15D;}else{goto
_LL15E;}_LL15E: if(*((int*)_tmp16C)== 10){_LL16C: _tmp177=((struct Cyc_Absyn_Using_d_struct*)
_tmp16C)->f1;_tmp178=*_tmp177;_LL16E: _tmp17A=_tmp178.f1;goto _LL16D;_LL16D:
_tmp179=_tmp178.f2;goto _LL16B;_LL16B: _tmp176=((struct Cyc_Absyn_Using_d_struct*)
_tmp16C)->f2;goto _LL15F;}else{goto _LL160;}_LL160: if(*((int*)_tmp16C)== 11){
_LL16F: _tmp17B=((struct Cyc_Absyn_ExternC_d_struct*)_tmp16C)->f1;goto _LL161;}
else{goto _LL149;}_LL14B: goto _LL14D;_LL14D:({void*_tmp17C[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("top level let-declarations are not implemented",sizeof(
unsigned char),47),_tag_arr(_tmp17C,sizeof(void*),0));});goto _LL149;_LL14F: if(
in_externC? Cyc_Tc_okay_externC(d->loc,(void*)_tmp16D->sc): 0){(void*)(_tmp16D->sc=(
void*)((void*)4));}if(_tmp16D->initializer != 0){void*_tmp17D=(void*)((struct Cyc_Absyn_Exp*)
_check_null(_tmp16D->initializer))->r;void*_tmp17E;_LL171: if(*((int*)_tmp17D)== 
19){_LL175: _tmp17E=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp17D)->f2;goto
_LL172;}else{goto _LL173;}_LL173: goto _LL174;_LL172: {struct Cyc_Absyn_Exp*_tmp180;
struct Cyc_List_List*_tmp181;struct Cyc_Dict_Dict*_tmp182;struct _tuple3 _tmp17F=Cyc_Tcgenrep_tcGenrep(
te,ge,loc,_tmp17E,dict);_LL179: _tmp182=_tmp17F.f1;goto _LL178;_LL178: _tmp181=
_tmp17F.f2;goto _LL177;_LL177: _tmp180=_tmp17F.f3;goto _LL176;_LL176: dict=_tmp182;
Cyc_Tc_tc_decls(te,_tmp181,in_externC,check_var_init);_tmp16D->initializer=(
struct Cyc_Absyn_Exp*)_tmp180;Cyc_Tc_tcVardecl(te,ge,loc,_tmp16D,check_var_init);
if(_tmp181 != 0){if(last != 0){((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp181,_tmp16B);((struct Cyc_List_List*)
_check_null(last))->tl=_tmp181;}else{struct Cyc_List_List tmp=({struct Cyc_List_List
_tmp183;_tmp183.hd=(struct Cyc_Absyn_Decl*)_tmp16B->hd;_tmp183.tl=_tmp16B->tl;
_tmp183;});(struct Cyc_Absyn_Decl*)(_tmp16B->hd=(void*)((struct Cyc_Absyn_Decl*)
_tmp181->hd));_tmp16B->tl=_tmp181->tl;(struct Cyc_Absyn_Decl*)(_tmp181->hd=(void*)((
struct Cyc_Absyn_Decl*)tmp.hd));_tmp181->tl=tmp.tl;((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp16B,
_tmp181);}}continue;}_LL174: goto _LL170;_LL170:;}Cyc_Tc_tcVardecl(te,ge,loc,
_tmp16D,check_var_init);goto _LL149;_LL151: if(in_externC? Cyc_Tc_okay_externC(d->loc,(
void*)_tmp16E->sc): 0){(void*)(_tmp16E->sc=(void*)((void*)4));}Cyc_Tc_tcFndecl(te,
ge,loc,_tmp16E);goto _LL149;_LL153: Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp16F);goto
_LL149;_LL155: if(in_externC? Cyc_Tc_okay_externC(d->loc,(void*)_tmp170->sc): 0){(
void*)(_tmp170->sc=(void*)((void*)4));}Cyc_Tc_tcStructdecl(te,ge,loc,_tmp170);
goto _LL149;_LL157: if(in_externC? Cyc_Tc_okay_externC(d->loc,(void*)_tmp171->sc): 0){(
void*)(_tmp171->sc=(void*)((void*)4));}Cyc_Tc_tcUniondecl(te,ge,loc,_tmp171);
goto _LL149;_LL159: if(in_externC? Cyc_Tc_okay_externC(d->loc,(void*)_tmp172->sc): 0){(
void*)(_tmp172->sc=(void*)((void*)4));}Cyc_Tc_tcTuniondecl(te,ge,loc,_tmp172);
goto _LL149;_LL15B: if(in_externC? Cyc_Tc_okay_externC(d->loc,(void*)_tmp173->sc): 0){(
void*)(_tmp173->sc=(void*)((void*)4));}Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp173);goto
_LL149;_LL15D: {struct Cyc_List_List*_tmp184=te->ns;struct Cyc_List_List*_tmp185=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp184,({struct Cyc_List_List*_tmp186=_cycalloc(sizeof(struct Cyc_List_List));
_tmp186->hd=_tmp175;_tmp186->tl=0;_tmp186;}));if(!((int(*)(struct Cyc_Set_Set*s,
struct _tagged_arr*elt))Cyc_Set_member)(ge->namespaces,_tmp175)){ge->namespaces=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(
ge->namespaces,_tmp175);te->ae=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp185,
Cyc_Tcenv_empty_genv());}te->ns=_tmp185;Cyc_Tc_tc_decls(te,_tmp174,in_externC,
check_var_init);te->ns=_tmp184;goto _LL149;}_LL15F: {struct _tagged_arr*first;
struct Cyc_List_List*rest;{void*_tmp187=_tmp17A;struct Cyc_List_List*_tmp188;
struct Cyc_List_List*_tmp189;struct Cyc_List_List*_tmp18A;struct Cyc_List_List
_tmp18B;struct Cyc_List_List*_tmp18C;struct _tagged_arr*_tmp18D;struct Cyc_List_List*
_tmp18E;struct Cyc_List_List _tmp18F;struct Cyc_List_List*_tmp190;struct _tagged_arr*
_tmp191;_LL17B: if((int)_tmp187 == 0){goto _LL17C;}else{goto _LL17D;}_LL17D: if((
unsigned int)_tmp187 > 1?*((int*)_tmp187)== 0: 0){_LL185: _tmp188=((struct Cyc_Absyn_Rel_n_struct*)
_tmp187)->f1;if(_tmp188 == 0){goto _LL17E;}else{goto _LL17F;}}else{goto _LL17F;}
_LL17F: if((unsigned int)_tmp187 > 1?*((int*)_tmp187)== 1: 0){_LL186: _tmp189=((
struct Cyc_Absyn_Abs_n_struct*)_tmp187)->f1;if(_tmp189 == 0){goto _LL180;}else{goto
_LL181;}}else{goto _LL181;}_LL181: if((unsigned int)_tmp187 > 1?*((int*)_tmp187)== 
0: 0){_LL187: _tmp18A=((struct Cyc_Absyn_Rel_n_struct*)_tmp187)->f1;if(_tmp18A == 0){
goto _LL183;}else{_tmp18B=*_tmp18A;_LL189: _tmp18D=(struct _tagged_arr*)_tmp18B.hd;
goto _LL188;_LL188: _tmp18C=_tmp18B.tl;goto _LL182;}}else{goto _LL183;}_LL183: if((
unsigned int)_tmp187 > 1?*((int*)_tmp187)== 1: 0){_LL18A: _tmp18E=((struct Cyc_Absyn_Abs_n_struct*)
_tmp187)->f1;if(_tmp18E == 0){goto _LL17A;}else{_tmp18F=*_tmp18E;_LL18C: _tmp191=(
struct _tagged_arr*)_tmp18F.hd;goto _LL18B;_LL18B: _tmp190=_tmp18F.tl;goto _LL184;}}
else{goto _LL17A;}_LL17C: goto _LL17E;_LL17E: goto _LL180;_LL180: first=_tmp179;rest=0;
goto _LL17A;_LL182: _tmp191=_tmp18D;_tmp190=_tmp18C;goto _LL184;_LL184: first=
_tmp191;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp190,({struct Cyc_List_List*_tmp192=_cycalloc(sizeof(
struct Cyc_List_List));_tmp192->hd=_tmp179;_tmp192->tl=0;_tmp192;}));goto _LL17A;
_LL17A:;}{struct Cyc_List_List*_tmp193=Cyc_Tcenv_resolve_namespace(te,loc,first,
rest);ge->availables=({struct Cyc_List_List*_tmp194=_cycalloc(sizeof(struct Cyc_List_List));
_tmp194->hd=_tmp193;_tmp194->tl=ge->availables;_tmp194;});Cyc_Tc_tc_decls(te,
_tmp176,in_externC,check_var_init);ge->availables=((struct Cyc_List_List*)
_check_null(ge->availables))->tl;goto _LL149;}}_LL161: Cyc_Tc_tc_decls(te,_tmp17B,
1,check_var_init);goto _LL149;_LL149:;}}void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int
check_var_init,struct Cyc_List_List*ds){Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,check_var_init);}static int Cyc_Tc_vardecl_needed(struct
Cyc_Dict_Dict*env,struct Cyc_Absyn_Decl*d);static struct Cyc_List_List*Cyc_Tc_treeshake_f(
struct Cyc_Dict_Dict*env,struct Cyc_List_List*ds){return((struct Cyc_List_List*(*)(
int(*f)(struct Cyc_Dict_Dict*,struct Cyc_Absyn_Decl*),struct Cyc_Dict_Dict*env,
struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}struct
_tuple7{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};static int Cyc_Tc_vardecl_needed(
struct Cyc_Dict_Dict*env,struct Cyc_Absyn_Decl*d){void*_tmp195=(void*)d->r;struct
Cyc_Absyn_Vardecl*_tmp196;struct Cyc_List_List*_tmp197;struct Cyc_List_List**
_tmp198;struct Cyc_List_List*_tmp199;struct Cyc_List_List**_tmp19A;struct Cyc_List_List*
_tmp19B;struct Cyc_List_List**_tmp19C;_LL18E: if(*((int*)_tmp195)== 0){_LL198:
_tmp196=((struct Cyc_Absyn_Var_d_struct*)_tmp195)->f1;goto _LL18F;}else{goto _LL190;}
_LL190: if(*((int*)_tmp195)== 11){_LL199: _tmp197=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp195)->f1;_tmp198=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_struct*)
_tmp195)->f1;goto _LL191;}else{goto _LL192;}_LL192: if(*((int*)_tmp195)== 10){
_LL19A: _tmp199=((struct Cyc_Absyn_Using_d_struct*)_tmp195)->f2;_tmp19A=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Using_d_struct*)_tmp195)->f2;goto _LL193;}else{goto _LL194;}_LL194:
if(*((int*)_tmp195)== 9){_LL19B: _tmp19B=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp195)->f2;_tmp19C=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_struct*)
_tmp195)->f2;goto _LL195;}else{goto _LL196;}_LL196: goto _LL197;_LL18F: if((void*)
_tmp196->sc != (void*)3?(void*)_tmp196->sc != (void*)4: 0){return 1;}{struct _tuple0
_tmp19E;struct _tagged_arr*_tmp19F;void*_tmp1A0;struct _tuple0*_tmp19D=_tmp196->name;
_tmp19E=*_tmp19D;_LL19E: _tmp1A0=_tmp19E.f1;goto _LL19D;_LL19D: _tmp19F=_tmp19E.f2;
goto _LL19C;_LL19C: {struct Cyc_List_List*ns;{void*_tmp1A1=_tmp1A0;struct Cyc_List_List*
_tmp1A2;struct Cyc_List_List*_tmp1A3;_LL1A0: if((int)_tmp1A1 == 0){goto _LL1A1;}
else{goto _LL1A2;}_LL1A2: if((unsigned int)_tmp1A1 > 1?*((int*)_tmp1A1)== 0: 0){
_LL1A6: _tmp1A2=((struct Cyc_Absyn_Rel_n_struct*)_tmp1A1)->f1;goto _LL1A3;}else{
goto _LL1A4;}_LL1A4: if((unsigned int)_tmp1A1 > 1?*((int*)_tmp1A1)== 1: 0){_LL1A7:
_tmp1A3=((struct Cyc_Absyn_Abs_n_struct*)_tmp1A1)->f1;goto _LL1A5;}else{goto _LL19F;}
_LL1A1: ns=0;goto _LL19F;_LL1A3: ns=_tmp1A2;goto _LL19F;_LL1A5: ns=_tmp1A3;goto _LL19F;
_LL19F:;}{struct _tuple7*_tmp1A4=((struct _tuple7*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(env,ns);struct Cyc_Tcenv_Genv*_tmp1A5=(*_tmp1A4).f1;
int _tmp1A6=(*((struct _tuple4*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp1A5->ordinaries,_tmp19F)).f2;if(! _tmp1A6){(*_tmp1A4).f2=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)((*_tmp1A4).f2,_tmp19F);}
return _tmp1A6;}}}_LL191: _tmp19A=_tmp198;goto _LL193;_LL193: _tmp19C=_tmp19A;goto
_LL195;_LL195:*_tmp19C=Cyc_Tc_treeshake_f(env,*_tmp19C);return 1;_LL197: return 1;
_LL18D:;}static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct
_tagged_arr*x,void*y){return !((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))
Cyc_Set_member)(set,x);}static void Cyc_Tc_treeshake_remove(struct Cyc_List_List*ns,
struct _tuple7*nsenv){struct _tuple7 _tmp1A8;struct Cyc_Set_Set*_tmp1A9;struct Cyc_Tcenv_Genv*
_tmp1AA;struct _tuple7*_tmp1A7=nsenv;_tmp1A8=*_tmp1A7;_LL1AA: _tmp1AA=_tmp1A8.f1;
goto _LL1A9;_LL1A9: _tmp1A9=_tmp1A8.f2;goto _LL1A8;_LL1A8: _tmp1AA->ordinaries=((
struct Cyc_Dict_Dict*(*)(int(*f)(struct Cyc_Set_Set*,struct _tagged_arr*,struct
_tuple4*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict*d))Cyc_Dict_filter_c)((int(*)(
struct Cyc_Set_Set*set,struct _tagged_arr*x,struct _tuple4*y))Cyc_Tc_treeshake_remove_f,
_tmp1A9,_tmp1AA->ordinaries);}static struct _tuple7*Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv*ge){return({struct _tuple7*_tmp1AB=_cycalloc(sizeof(struct
_tuple7));_tmp1AB->f1=ge;_tmp1AB->f2=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_tmp1AB;});}
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
ds){struct Cyc_Dict_Dict*_tmp1AC=((struct Cyc_Dict_Dict*(*)(struct _tuple7*(*f)(
struct Cyc_Tcenv_Genv*),struct Cyc_Dict_Dict*d))Cyc_Dict_map)(Cyc_Tc_treeshake_make_env_f,
te->ae);struct Cyc_List_List*_tmp1AD=Cyc_Tc_treeshake_f(_tmp1AC,ds);((void(*)(
void(*f)(struct Cyc_List_List*,struct _tuple7*),struct Cyc_Dict_Dict*d))Cyc_Dict_iter)(
Cyc_Tc_treeshake_remove,_tmp1AC);return _tmp1AD;}
