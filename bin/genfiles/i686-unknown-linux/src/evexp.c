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
 struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern unsigned char Cyc_List_List_mismatch[
18];extern unsigned char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
unsigned char Cyc_Position_Nocontext[14];struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr
fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
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
15];extern struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*
x);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
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
le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_tvar_kind(struct
Cyc_Absyn_Tvar*t);extern void*Cyc_Tcutil_compress(void*t);extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);extern int Cyc_Evexp_okay_szofarg(void*t);void*Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp*e);void*Cyc_Evexp_promote_const(void*cn){void*_tmp0=cn;
unsigned char _tmp1;void*_tmp2;short _tmp3;void*_tmp4;_LL1: if((unsigned int)_tmp0 > 
1?*((int*)_tmp0)== 0: 0){_LL8: _tmp2=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp0)->f1;
goto _LL7;_LL7: _tmp1=((struct Cyc_Absyn_Char_c_struct*)_tmp0)->f2;goto _LL2;}else{
goto _LL3;}_LL3: if((unsigned int)_tmp0 > 1?*((int*)_tmp0)== 1: 0){_LLA: _tmp4=(void*)((
struct Cyc_Absyn_Short_c_struct*)_tmp0)->f1;goto _LL9;_LL9: _tmp3=((struct Cyc_Absyn_Short_c_struct*)
_tmp0)->f2;goto _LL4;}else{goto _LL5;}_LL5: goto _LL6;_LL2: return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));_tmp5[0]=({struct Cyc_Absyn_Int_c_struct
_tmp6;_tmp6.tag=2;_tmp6.f1=(void*)_tmp2;_tmp6.f2=(int)_tmp1;_tmp6;});_tmp5;});
_LL4: return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp7=_cycalloc(sizeof(struct
Cyc_Absyn_Int_c_struct));_tmp7[0]=({struct Cyc_Absyn_Int_c_struct _tmp8;_tmp8.tag=
2;_tmp8.f1=(void*)_tmp4;_tmp8.f2=(int)_tmp3;_tmp8;});_tmp7;});_LL6: return cn;_LL0:;}
unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){void*_tmp9=Cyc_Evexp_promote_const(
Cyc_Evexp_eval_const_exp(e));int _tmpA;_LLC: if((unsigned int)_tmp9 > 1?*((int*)
_tmp9)== 2: 0){_LL10: _tmpA=((struct Cyc_Absyn_Int_c_struct*)_tmp9)->f2;goto _LLD;}
else{goto _LLE;}_LLE: goto _LLF;_LLD: return(unsigned int)_tmpA;_LLF:({void*_tmpB[0]={};
Cyc_Tcutil_terr(e->loc,_tag_arr("expecting unsigned int",sizeof(unsigned char),
23),_tag_arr(_tmpB,sizeof(void*),0));});return 0;_LLB:;}int Cyc_Evexp_eval_const_bool_exp(
struct Cyc_Absyn_Exp*e){void*_tmpC=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(
e));int _tmpD;long long _tmpE;_LL12: if((unsigned int)_tmpC > 1?*((int*)_tmpC)== 2: 0){
_LL1A: _tmpD=((struct Cyc_Absyn_Int_c_struct*)_tmpC)->f2;goto _LL13;}else{goto _LL14;}
_LL14: if((unsigned int)_tmpC > 1?*((int*)_tmpC)== 3: 0){_LL1B: _tmpE=((struct Cyc_Absyn_LongLong_c_struct*)
_tmpC)->f2;goto _LL15;}else{goto _LL16;}_LL16: if((int)_tmpC == 0){goto _LL17;}else{
goto _LL18;}_LL18: goto _LL19;_LL13: return _tmpD != 0;_LL15: return _tmpE != 0;_LL17:
return 0;_LL19:({void*_tmpF[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("expecting bool",
sizeof(unsigned char),15),_tag_arr(_tmpF,sizeof(void*),0));});return 0;_LL11:;}
struct _tuple3{struct Cyc_Absyn_Tqual f1;void*f2;};int Cyc_Evexp_okay_szofarg(void*t){
void*_tmp10=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp11;struct Cyc_Core_Opt*
_tmp12;struct Cyc_Core_Opt _tmp13;void*_tmp14;struct Cyc_Absyn_TunionFieldInfo
_tmp15;void*_tmp16;struct Cyc_Absyn_Tunionfield*_tmp17;struct Cyc_List_List*_tmp18;
struct Cyc_Absyn_PtrInfo _tmp19;struct Cyc_Absyn_Conref*_tmp1A;struct Cyc_Absyn_Exp*
_tmp1B;void*_tmp1C;struct Cyc_Absyn_Structdecl**_tmp1D;struct Cyc_List_List*_tmp1E;
struct Cyc_Absyn_Uniondecl**_tmp1F;struct Cyc_List_List*_tmp20;struct Cyc_List_List*
_tmp21;_LL1D: if((int)_tmp10 == 0){goto _LL1E;}else{goto _LL1F;}_LL1F: if((
unsigned int)_tmp10 > 3?*((int*)_tmp10)== 1: 0){_LL53: _tmp11=((struct Cyc_Absyn_VarType_struct*)
_tmp10)->f1;goto _LL20;}else{goto _LL21;}_LL21: if((unsigned int)_tmp10 > 3?*((int*)
_tmp10)== 0: 0){_LL54: _tmp12=((struct Cyc_Absyn_Evar_struct*)_tmp10)->f1;if(_tmp12
== 0){goto _LL23;}else{_tmp13=*_tmp12;_LL55: _tmp14=(void*)_tmp13.v;if((int)_tmp14
== 2){goto _LL22;}else{goto _LL23;}}}else{goto _LL23;}_LL23: if((unsigned int)_tmp10
> 3?*((int*)_tmp10)== 0: 0){goto _LL24;}else{goto _LL25;}_LL25: if((unsigned int)
_tmp10 > 3?*((int*)_tmp10)== 2: 0){goto _LL26;}else{goto _LL27;}_LL27: if((
unsigned int)_tmp10 > 3?*((int*)_tmp10)== 3: 0){_LL56: _tmp15=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp10)->f1;_LL57: _tmp16=(void*)_tmp15.field_info;if(*((int*)_tmp16)== 1){_LL58:
_tmp17=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp16)->f2;goto _LL28;}else{
goto _LL29;}}else{goto _LL29;}_LL29: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 9:
0){_LL59: _tmp18=((struct Cyc_Absyn_TupleType_struct*)_tmp10)->f1;goto _LL2A;}else{
goto _LL2B;}_LL2B: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 3: 0){goto _LL2C;}
else{goto _LL2D;}_LL2D: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 4: 0){_LL5A:
_tmp19=((struct Cyc_Absyn_PointerType_struct*)_tmp10)->f1;_LL5B: _tmp1A=_tmp19.bounds;
goto _LL2E;}else{goto _LL2F;}_LL2F: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 13:
0){goto _LL30;}else{goto _LL31;}_LL31: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 
5: 0){goto _LL32;}else{goto _LL33;}_LL33: if((int)_tmp10 == 1){goto _LL34;}else{goto
_LL35;}_LL35: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 6: 0){goto _LL36;}else{
goto _LL37;}_LL37: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 7: 0){_LL5D: _tmp1C=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp10)->f1;goto _LL5C;_LL5C: _tmp1B=((
struct Cyc_Absyn_ArrayType_struct*)_tmp10)->f3;goto _LL38;}else{goto _LL39;}_LL39:
if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 8: 0){goto _LL3A;}else{goto _LL3B;}
_LL3B: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 17: 0){goto _LL3C;}else{goto
_LL3D;}_LL3D: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 10: 0){_LL5E: _tmp1D=((
struct Cyc_Absyn_StructType_struct*)_tmp10)->f3;goto _LL3E;}else{goto _LL3F;}_LL3F:
if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 14: 0){_LL5F: _tmp1E=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp10)->f1;goto _LL40;}else{goto _LL41;}_LL41: if((unsigned int)_tmp10 > 3?*((int*)
_tmp10)== 11: 0){_LL61: _tmp20=((struct Cyc_Absyn_UnionType_struct*)_tmp10)->f2;
goto _LL60;_LL60: _tmp1F=((struct Cyc_Absyn_UnionType_struct*)_tmp10)->f3;goto _LL42;}
else{goto _LL43;}_LL43: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 15: 0){_LL62:
_tmp21=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp10)->f1;goto _LL44;}else{goto
_LL45;}_LL45: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 12: 0){goto _LL46;}else{
goto _LL47;}_LL47: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 16: 0){goto _LL48;}
else{goto _LL49;}_LL49: if((int)_tmp10 == 2){goto _LL4A;}else{goto _LL4B;}_LL4B: if((
unsigned int)_tmp10 > 3?*((int*)_tmp10)== 20: 0){goto _LL4C;}else{goto _LL4D;}_LL4D:
if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 19: 0){goto _LL4E;}else{goto _LL4F;}
_LL4F: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 21: 0){goto _LL50;}else{goto
_LL51;}_LL51: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 18: 0){goto _LL52;}else{
goto _LL1C;}_LL1E: return 0;_LL20: {void*_tmp22=Cyc_Tcutil_tvar_kind(_tmp11);_LL64:
if((int)_tmp22 == 2){goto _LL65;}else{goto _LL66;}_LL66: goto _LL67;_LL65: return 1;
_LL67: return 0;_LL63:;}_LL22: return 1;_LL24: return 0;_LL26: return 1;_LL28: _tmp18=
_tmp17->typs;goto _LL2A;_LL2A: for(0;_tmp18 != 0;_tmp18=_tmp18->tl){if(! Cyc_Evexp_okay_szofarg((*((
struct _tuple3*)_tmp18->hd)).f2)){return 0;}}return 1;_LL2C: return 0;_LL2E: {void*
_tmp23=(void*)(Cyc_Absyn_compress_conref(_tmp1A))->v;void*_tmp24;_LL69: if((
unsigned int)_tmp23 > 1?*((int*)_tmp23)== 0: 0){_LL6F: _tmp24=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp23)->f1;if((int)_tmp24 == 0){goto _LL6A;}else{goto _LL6B;}}else{goto _LL6B;}
_LL6B: if((unsigned int)_tmp23 > 1?*((int*)_tmp23)== 0: 0){goto _LL6C;}else{goto
_LL6D;}_LL6D: goto _LL6E;_LL6A: return 1;_LL6C: return 1;_LL6E: return 0;_LL68:;}_LL30:
return 1;_LL32: return 1;_LL34: return 1;_LL36: return 1;_LL38: return _tmp1B != 0;_LL3A:
return 0;_LL3C: return 1;_LL3E: if(_tmp1D == 0){return({void*_tmp25[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("szof on unchecked StructType",
sizeof(unsigned char),29),_tag_arr(_tmp25,sizeof(void*),0));});}{struct Cyc_Absyn_Structdecl*
_tmp26=*_tmp1D;if(_tmp26->fields == 0){return 0;}_tmp1E=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp26->fields))->v;goto _LL40;}_LL40: for(0;
_tmp1E != 0;_tmp1E=_tmp1E->tl){if(! Cyc_Evexp_okay_szofarg((void*)((struct Cyc_Absyn_Structfield*)
_tmp1E->hd)->type)){return 0;}}return 1;_LL42: if(_tmp1F == 0){return({void*_tmp27[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("szof on unchecked UnionType",
sizeof(unsigned char),28),_tag_arr(_tmp27,sizeof(void*),0));});}{struct Cyc_Absyn_Uniondecl*
_tmp28=*_tmp1F;if(_tmp28->fields == 0){return 0;}_tmp21=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp28->fields))->v;goto _LL44;}_LL44: for(0;
_tmp21 != 0;_tmp21=_tmp21->tl){if(! Cyc_Evexp_okay_szofarg((void*)((struct Cyc_Absyn_Structfield*)
_tmp21->hd)->type)){return 0;}}return 1;_LL46: return 1;_LL48: return 1;_LL4A: return 0;
_LL4C: return 0;_LL4E: return 0;_LL50: return 0;_LL52: return({void*_tmp29[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)((struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp2B;_tmp2B.tag=0;_tmp2B.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(t);{void*_tmp2A[1]={& _tmp2B};Cyc_Std_aprintf(_tag_arr("szof typedeftype %s",
sizeof(unsigned char),20),_tag_arr(_tmp2A,sizeof(void*),1));}}),_tag_arr(_tmp29,
sizeof(void*),0));});_LL1C:;}struct _tuple4{void*f1;void*f2;};void*Cyc_Evexp_eval_const_unprimop(
void*p,struct Cyc_Absyn_Exp*e){void*cn=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(
e));struct _tuple4 _tmp2D=({struct _tuple4 _tmp2C;_tmp2C.f1=p;_tmp2C.f2=cn;_tmp2C;});
void*_tmp2E;void*_tmp2F;int _tmp30;void*_tmp31;void*_tmp32;void*_tmp33;int _tmp34;
void*_tmp35;void*_tmp36;void*_tmp37;int _tmp38;void*_tmp39;void*_tmp3A;void*
_tmp3B;_LL71: _LL7D: _tmp2E=_tmp2D.f1;if((int)_tmp2E == 0){goto _LL72;}else{goto
_LL73;}_LL73: _LL81: _tmp32=_tmp2D.f1;if((int)_tmp32 == 2){goto _LL7E;}else{goto
_LL75;}_LL7E: _tmp2F=_tmp2D.f2;if((unsigned int)_tmp2F > 1?*((int*)_tmp2F)== 2: 0){
_LL80: _tmp31=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp2F)->f1;goto _LL7F;_LL7F:
_tmp30=((struct Cyc_Absyn_Int_c_struct*)_tmp2F)->f2;goto _LL74;}else{goto _LL75;}
_LL75: _LL85: _tmp36=_tmp2D.f1;if((int)_tmp36 == 12){goto _LL82;}else{goto _LL77;}
_LL82: _tmp33=_tmp2D.f2;if((unsigned int)_tmp33 > 1?*((int*)_tmp33)== 2: 0){_LL84:
_tmp35=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp33)->f1;goto _LL83;_LL83: _tmp34=((
struct Cyc_Absyn_Int_c_struct*)_tmp33)->f2;goto _LL76;}else{goto _LL77;}_LL77: _LL88:
_tmp39=_tmp2D.f1;if((int)_tmp39 == 11){goto _LL86;}else{goto _LL79;}_LL86: _tmp37=
_tmp2D.f2;if((unsigned int)_tmp37 > 1?*((int*)_tmp37)== 2: 0){_LL87: _tmp38=((
struct Cyc_Absyn_Int_c_struct*)_tmp37)->f2;goto _LL78;}else{goto _LL79;}_LL79: _LL8A:
_tmp3B=_tmp2D.f1;if((int)_tmp3B == 11){goto _LL89;}else{goto _LL7B;}_LL89: _tmp3A=
_tmp2D.f2;if((int)_tmp3A == 0){goto _LL7A;}else{goto _LL7B;}_LL7B: goto _LL7C;_LL72:
return cn;_LL74: return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp3C=_cycalloc(
sizeof(struct Cyc_Absyn_Int_c_struct));_tmp3C[0]=({struct Cyc_Absyn_Int_c_struct
_tmp3D;_tmp3D.tag=2;_tmp3D.f1=(void*)((void*)0);_tmp3D.f2=- _tmp30;_tmp3D;});
_tmp3C;});_LL76: return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp3E=_cycalloc(
sizeof(struct Cyc_Absyn_Int_c_struct));_tmp3E[0]=({struct Cyc_Absyn_Int_c_struct
_tmp3F;_tmp3F.tag=2;_tmp3F.f1=(void*)((void*)1);_tmp3F.f2=~ _tmp34;_tmp3F;});
_tmp3E;});_LL78: return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp40=_cycalloc(
sizeof(struct Cyc_Absyn_Int_c_struct));_tmp40[0]=({struct Cyc_Absyn_Int_c_struct
_tmp41;_tmp41.tag=2;_tmp41.f1=(void*)((void*)0);_tmp41.f2=_tmp38 == 0? 1: 0;_tmp41;});
_tmp40;});_LL7A: return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp42=_cycalloc(
sizeof(struct Cyc_Absyn_Int_c_struct));_tmp42[0]=({struct Cyc_Absyn_Int_c_struct
_tmp43;_tmp43.tag=2;_tmp43.f1=(void*)((void*)0);_tmp43.f2=1;_tmp43;});_tmp42;});
_LL7C:({void*_tmp44[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("bad constant expression",
sizeof(unsigned char),24),_tag_arr(_tmp44,sizeof(void*),0));});return cn;_LL70:;}
struct _tuple5{void*f1;int f2;};void*Cyc_Evexp_eval_const_binprimop(void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp45=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(
e1));void*_tmp46=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(e2));void*s1;
void*s2;int i1;int i2;{void*_tmp47=_tmp45;int _tmp48;void*_tmp49;_LL8C: if((
unsigned int)_tmp47 > 1?*((int*)_tmp47)== 2: 0){_LL91: _tmp49=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp47)->f1;goto _LL90;_LL90: _tmp48=((struct Cyc_Absyn_Int_c_struct*)_tmp47)->f2;
goto _LL8D;}else{goto _LL8E;}_LL8E: goto _LL8F;_LL8D: s1=_tmp49;i1=_tmp48;goto _LL8B;
_LL8F:({void*_tmp4A[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("bad constant expression",
sizeof(unsigned char),24),_tag_arr(_tmp4A,sizeof(void*),0));});return _tmp45;
_LL8B:;}{void*_tmp4B=_tmp46;int _tmp4C;void*_tmp4D;_LL93: if((unsigned int)_tmp4B > 
1?*((int*)_tmp4B)== 2: 0){_LL98: _tmp4D=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp4B)->f1;goto _LL97;_LL97: _tmp4C=((struct Cyc_Absyn_Int_c_struct*)_tmp4B)->f2;
goto _LL94;}else{goto _LL95;}_LL95: goto _LL96;_LL94: s2=_tmp4D;i2=_tmp4C;goto _LL92;
_LL96:({void*_tmp4E[0]={};Cyc_Tcutil_terr(e2->loc,_tag_arr("bad constant expression",
sizeof(unsigned char),24),_tag_arr(_tmp4E,sizeof(void*),0));});return _tmp45;
_LL92:;}{void*_tmp4F=p;_LL9A: if((int)_tmp4F == 3){goto _LL9B;}else{goto _LL9C;}
_LL9C: if((int)_tmp4F == 4){goto _LL9D;}else{goto _LL9E;}_LL9E: goto _LL9F;_LL9B: goto
_LL9D;_LL9D: if(i2 == 0){({void*_tmp50[0]={};Cyc_Tcutil_terr(e2->loc,_tag_arr("division by zero in constant expression",
sizeof(unsigned char),40),_tag_arr(_tmp50,sizeof(void*),0));});return _tmp45;}
goto _LL99;_LL9F: goto _LL99;_LL99:;}{int has_u_arg=s1 == (void*)1? 1: s2 == (void*)1;
unsigned int u1=(unsigned int)i1;unsigned int u2=(unsigned int)i2;int i3=0;
unsigned int u3=0;int b3=1;int use_i3=0;int use_u3=0;int use_b3=0;{struct _tuple5
_tmp52=({struct _tuple5 _tmp51;_tmp51.f1=p;_tmp51.f2=has_u_arg;_tmp51;});int _tmp53;
void*_tmp54;int _tmp55;void*_tmp56;int _tmp57;void*_tmp58;int _tmp59;void*_tmp5A;
int _tmp5B;void*_tmp5C;int _tmp5D;void*_tmp5E;int _tmp5F;void*_tmp60;int _tmp61;void*
_tmp62;int _tmp63;void*_tmp64;int _tmp65;void*_tmp66;void*_tmp67;void*_tmp68;int
_tmp69;void*_tmp6A;int _tmp6B;void*_tmp6C;int _tmp6D;void*_tmp6E;int _tmp6F;void*
_tmp70;int _tmp71;void*_tmp72;int _tmp73;void*_tmp74;int _tmp75;void*_tmp76;int
_tmp77;void*_tmp78;void*_tmp79;void*_tmp7A;void*_tmp7B;void*_tmp7C;void*_tmp7D;
void*_tmp7E;_LLA1: _LLD8: _tmp54=_tmp52.f1;if((int)_tmp54 == 0){goto _LLD7;}else{
goto _LLA3;}_LLD7: _tmp53=_tmp52.f2;if(_tmp53 == 0){goto _LLA2;}else{goto _LLA3;}
_LLA3: _LLDA: _tmp56=_tmp52.f1;if((int)_tmp56 == 1){goto _LLD9;}else{goto _LLA5;}
_LLD9: _tmp55=_tmp52.f2;if(_tmp55 == 0){goto _LLA4;}else{goto _LLA5;}_LLA5: _LLDC:
_tmp58=_tmp52.f1;if((int)_tmp58 == 2){goto _LLDB;}else{goto _LLA7;}_LLDB: _tmp57=
_tmp52.f2;if(_tmp57 == 0){goto _LLA6;}else{goto _LLA7;}_LLA7: _LLDE: _tmp5A=_tmp52.f1;
if((int)_tmp5A == 3){goto _LLDD;}else{goto _LLA9;}_LLDD: _tmp59=_tmp52.f2;if(_tmp59
== 0){goto _LLA8;}else{goto _LLA9;}_LLA9: _LLE0: _tmp5C=_tmp52.f1;if((int)_tmp5C == 4){
goto _LLDF;}else{goto _LLAB;}_LLDF: _tmp5B=_tmp52.f2;if(_tmp5B == 0){goto _LLAA;}
else{goto _LLAB;}_LLAB: _LLE2: _tmp5E=_tmp52.f1;if((int)_tmp5E == 0){goto _LLE1;}
else{goto _LLAD;}_LLE1: _tmp5D=_tmp52.f2;if(_tmp5D == 1){goto _LLAC;}else{goto _LLAD;}
_LLAD: _LLE4: _tmp60=_tmp52.f1;if((int)_tmp60 == 1){goto _LLE3;}else{goto _LLAF;}
_LLE3: _tmp5F=_tmp52.f2;if(_tmp5F == 1){goto _LLAE;}else{goto _LLAF;}_LLAF: _LLE6:
_tmp62=_tmp52.f1;if((int)_tmp62 == 2){goto _LLE5;}else{goto _LLB1;}_LLE5: _tmp61=
_tmp52.f2;if(_tmp61 == 1){goto _LLB0;}else{goto _LLB1;}_LLB1: _LLE8: _tmp64=_tmp52.f1;
if((int)_tmp64 == 3){goto _LLE7;}else{goto _LLB3;}_LLE7: _tmp63=_tmp52.f2;if(_tmp63
== 1){goto _LLB2;}else{goto _LLB3;}_LLB3: _LLEA: _tmp66=_tmp52.f1;if((int)_tmp66 == 4){
goto _LLE9;}else{goto _LLB5;}_LLE9: _tmp65=_tmp52.f2;if(_tmp65 == 1){goto _LLB4;}
else{goto _LLB5;}_LLB5: _LLEB: _tmp67=_tmp52.f1;if((int)_tmp67 == 5){goto _LLB6;}
else{goto _LLB7;}_LLB7: _LLEC: _tmp68=_tmp52.f1;if((int)_tmp68 == 6){goto _LLB8;}
else{goto _LLB9;}_LLB9: _LLEE: _tmp6A=_tmp52.f1;if((int)_tmp6A == 7){goto _LLED;}
else{goto _LLBB;}_LLED: _tmp69=_tmp52.f2;if(_tmp69 == 0){goto _LLBA;}else{goto _LLBB;}
_LLBB: _LLF0: _tmp6C=_tmp52.f1;if((int)_tmp6C == 8){goto _LLEF;}else{goto _LLBD;}
_LLEF: _tmp6B=_tmp52.f2;if(_tmp6B == 0){goto _LLBC;}else{goto _LLBD;}_LLBD: _LLF2:
_tmp6E=_tmp52.f1;if((int)_tmp6E == 9){goto _LLF1;}else{goto _LLBF;}_LLF1: _tmp6D=
_tmp52.f2;if(_tmp6D == 0){goto _LLBE;}else{goto _LLBF;}_LLBF: _LLF4: _tmp70=_tmp52.f1;
if((int)_tmp70 == 10){goto _LLF3;}else{goto _LLC1;}_LLF3: _tmp6F=_tmp52.f2;if(_tmp6F
== 0){goto _LLC0;}else{goto _LLC1;}_LLC1: _LLF6: _tmp72=_tmp52.f1;if((int)_tmp72 == 7){
goto _LLF5;}else{goto _LLC3;}_LLF5: _tmp71=_tmp52.f2;if(_tmp71 == 1){goto _LLC2;}
else{goto _LLC3;}_LLC3: _LLF8: _tmp74=_tmp52.f1;if((int)_tmp74 == 8){goto _LLF7;}
else{goto _LLC5;}_LLF7: _tmp73=_tmp52.f2;if(_tmp73 == 1){goto _LLC4;}else{goto _LLC5;}
_LLC5: _LLFA: _tmp76=_tmp52.f1;if((int)_tmp76 == 9){goto _LLF9;}else{goto _LLC7;}
_LLF9: _tmp75=_tmp52.f2;if(_tmp75 == 1){goto _LLC6;}else{goto _LLC7;}_LLC7: _LLFC:
_tmp78=_tmp52.f1;if((int)_tmp78 == 10){goto _LLFB;}else{goto _LLC9;}_LLFB: _tmp77=
_tmp52.f2;if(_tmp77 == 1){goto _LLC8;}else{goto _LLC9;}_LLC9: _LLFD: _tmp79=_tmp52.f1;
if((int)_tmp79 == 13){goto _LLCA;}else{goto _LLCB;}_LLCB: _LLFE: _tmp7A=_tmp52.f1;if((
int)_tmp7A == 14){goto _LLCC;}else{goto _LLCD;}_LLCD: _LLFF: _tmp7B=_tmp52.f1;if((int)
_tmp7B == 15){goto _LLCE;}else{goto _LLCF;}_LLCF: _LL100: _tmp7C=_tmp52.f1;if((int)
_tmp7C == 16){goto _LLD0;}else{goto _LLD1;}_LLD1: _LL101: _tmp7D=_tmp52.f1;if((int)
_tmp7D == 17){goto _LLD2;}else{goto _LLD3;}_LLD3: _LL102: _tmp7E=_tmp52.f1;if((int)
_tmp7E == 18){goto _LLD4;}else{goto _LLD5;}_LLD5: goto _LLD6;_LLA2: i3=i1 + i2;use_i3=1;
goto _LLA0;_LLA4: i3=i1 * i2;use_i3=1;goto _LLA0;_LLA6: i3=i1 - i2;use_i3=1;goto _LLA0;
_LLA8: i3=i1 / i2;use_i3=1;goto _LLA0;_LLAA: i3=i1 % i2;use_i3=1;goto _LLA0;_LLAC: u3=
u1 + u2;use_u3=1;goto _LLA0;_LLAE: u3=u1 * u2;use_u3=1;goto _LLA0;_LLB0: u3=u1 - u2;
use_u3=1;goto _LLA0;_LLB2: u3=u1 / u2;use_u3=1;goto _LLA0;_LLB4: u3=u1 % u2;use_u3=1;
goto _LLA0;_LLB6: b3=i1 == i2;use_b3=1;goto _LLA0;_LLB8: b3=i1 != i2;use_b3=1;goto
_LLA0;_LLBA: b3=i1 > i2;use_b3=1;goto _LLA0;_LLBC: b3=i1 < i2;use_b3=1;goto _LLA0;
_LLBE: b3=i1 >= i2;use_b3=1;goto _LLA0;_LLC0: b3=i1 <= i2;use_b3=1;goto _LLA0;_LLC2: b3=
u1 > u2;use_b3=1;goto _LLA0;_LLC4: b3=u1 < u2;use_b3=1;goto _LLA0;_LLC6: b3=u1 >= u2;
use_b3=1;goto _LLA0;_LLC8: b3=u1 <= u2;use_b3=1;goto _LLA0;_LLCA: u3=u1 & u2;use_u3=1;
goto _LLA0;_LLCC: u3=u1 | u2;use_u3=1;goto _LLA0;_LLCE: u3=u1 ^ u2;use_u3=1;goto _LLA0;
_LLD0: u3=u1 << u2;use_u3=1;goto _LLA0;_LLD2: u3=u1 >> u2;use_u3=1;goto _LLA0;_LLD4:({
void*_tmp7F[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr(">>> NOT IMPLEMENTED",sizeof(
unsigned char),20),_tag_arr(_tmp7F,sizeof(void*),0));});return _tmp45;_LLD6:({
void*_tmp80[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("bad constant expression",
sizeof(unsigned char),24),_tag_arr(_tmp80,sizeof(void*),0));});return _tmp45;
_LLA0:;}if(use_i3){return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp81=_cycalloc(
sizeof(struct Cyc_Absyn_Int_c_struct));_tmp81[0]=({struct Cyc_Absyn_Int_c_struct
_tmp82;_tmp82.tag=2;_tmp82.f1=(void*)((void*)0);_tmp82.f2=i3;_tmp82;});_tmp81;});}
if(use_u3){return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp83=_cycalloc(sizeof(
struct Cyc_Absyn_Int_c_struct));_tmp83[0]=({struct Cyc_Absyn_Int_c_struct _tmp84;
_tmp84.tag=2;_tmp84.f1=(void*)((void*)1);_tmp84.f2=(int)u3;_tmp84;});_tmp83;});}
if(use_b3){return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp85=_cycalloc(sizeof(
struct Cyc_Absyn_Int_c_struct));_tmp85[0]=({struct Cyc_Absyn_Int_c_struct _tmp86;
_tmp86.tag=2;_tmp86.f1=(void*)((void*)0);_tmp86.f2=b3? 1: 0;_tmp86;});_tmp85;});}(
int)_throw((void*)({struct Cyc_Core_Unreachable_struct*_tmp87=_cycalloc(sizeof(
struct Cyc_Core_Unreachable_struct));_tmp87[0]=({struct Cyc_Core_Unreachable_struct
_tmp88;_tmp88.tag=Cyc_Core_Unreachable;_tmp88.f1=_tag_arr("Evexp::eval_const_binop",
sizeof(unsigned char),24);_tmp88;});_tmp87;}));}}void*Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp*e){void*cn;{void*_tmp89=(void*)e->r;void*_tmp8A;struct Cyc_Absyn_Exp*
_tmp8B;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_Absyn_Exp*_tmp8D;struct Cyc_List_List*
_tmp8E;void*_tmp8F;struct Cyc_Absyn_Exp*_tmp90;void*_tmp91;struct Cyc_Absyn_Enumfield*
_tmp92;struct Cyc_Absyn_Enumfield*_tmp93;_LL104: if(*((int*)_tmp89)== 0){_LL118:
_tmp8A=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp89)->f1;goto _LL105;}else{
goto _LL106;}_LL106: if(*((int*)_tmp89)== 6){_LL11B: _tmp8D=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp89)->f1;goto _LL11A;_LL11A: _tmp8C=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp89)->f2;goto _LL119;_LL119: _tmp8B=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp89)->f3;goto _LL107;}else{goto _LL108;}_LL108: if(*((int*)_tmp89)== 3){_LL11D:
_tmp8F=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp89)->f1;goto _LL11C;_LL11C:
_tmp8E=((struct Cyc_Absyn_Primop_e_struct*)_tmp89)->f2;goto _LL109;}else{goto
_LL10A;}_LL10A: if(*((int*)_tmp89)== 16){goto _LL10B;}else{goto _LL10C;}_LL10C: if(*((
int*)_tmp89)== 17){goto _LL10D;}else{goto _LL10E;}_LL10E: if(*((int*)_tmp89)== 18){
goto _LL10F;}else{goto _LL110;}_LL110: if(*((int*)_tmp89)== 13){_LL11F: _tmp91=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp89)->f1;goto _LL11E;_LL11E: _tmp90=((struct Cyc_Absyn_Cast_e_struct*)
_tmp89)->f2;goto _LL111;}else{goto _LL112;}_LL112: if(*((int*)_tmp89)== 32){_LL120:
_tmp92=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp89)->f3;goto _LL113;}else{goto
_LL114;}_LL114: if(*((int*)_tmp89)== 31){_LL121: _tmp93=((struct Cyc_Absyn_Enum_e_struct*)
_tmp89)->f3;goto _LL115;}else{goto _LL116;}_LL116: goto _LL117;_LL105: return _tmp8A;
_LL107: cn=Cyc_Evexp_eval_const_bool_exp(_tmp8D)? Cyc_Evexp_eval_const_exp(_tmp8C):
Cyc_Evexp_eval_const_exp(_tmp8B);goto _LL103;_LL109: if(_tmp8E == 0){({void*_tmp94[
0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("bad static expression (no args to primop)",
sizeof(unsigned char),42),_tag_arr(_tmp94,sizeof(void*),0));});return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp95=_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));
_tmp95[0]=({struct Cyc_Absyn_Int_c_struct _tmp96;_tmp96.tag=2;_tmp96.f1=(void*)((
void*)0);_tmp96.f2=0;_tmp96;});_tmp95;});}if(_tmp8E->tl == 0){cn=Cyc_Evexp_eval_const_unprimop(
_tmp8F,(struct Cyc_Absyn_Exp*)_tmp8E->hd);}else{if(((struct Cyc_List_List*)
_check_null(_tmp8E->tl))->tl != 0){({void*_tmp97[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("bad static expression (too many args to primop)",sizeof(unsigned char),
48),_tag_arr(_tmp97,sizeof(void*),0));});return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp98=_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));_tmp98[0]=({struct Cyc_Absyn_Int_c_struct
_tmp99;_tmp99.tag=2;_tmp99.f1=(void*)((void*)0);_tmp99.f2=0;_tmp99;});_tmp98;});}
else{cn=Cyc_Evexp_eval_const_binprimop(_tmp8F,(struct Cyc_Absyn_Exp*)_tmp8E->hd,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp8E->tl))->hd);}}goto
_LL103;_LL10B: goto _LL10D;_LL10D:({void*_tmp9A[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("sizeof(...) is not an integral constant expression in Cyclone",sizeof(
unsigned char),62),_tag_arr(_tmp9A,sizeof(void*),0));});return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp9B=_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));_tmp9B[0]=({struct Cyc_Absyn_Int_c_struct
_tmp9C;_tmp9C.tag=2;_tmp9C.f1=(void*)((void*)0);_tmp9C.f2=0;_tmp9C;});_tmp9B;});
_LL10F:({void*_tmp9D[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("offsetof(...) is not an integral constant expression in Cyclone",
sizeof(unsigned char),64),_tag_arr(_tmp9D,sizeof(void*),0));});return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp9E=_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));
_tmp9E[0]=({struct Cyc_Absyn_Int_c_struct _tmp9F;_tmp9F.tag=2;_tmp9F.f1=(void*)((
void*)0);_tmp9F.f2=0;_tmp9F;});_tmp9E;});_LL111: cn=Cyc_Evexp_eval_const_exp(
_tmp90);{struct _tuple4 _tmpA1=({struct _tuple4 _tmpA0;_tmpA0.f1=Cyc_Tcutil_compress(
_tmp91);_tmpA0.f2=cn;_tmpA0;});void*_tmpA2;unsigned char _tmpA3;void*_tmpA4;void*
_tmpA5;void*_tmpA6;void*_tmpA7;void*_tmpA8;short _tmpA9;void*_tmpAA;void*_tmpAB;
void*_tmpAC;void*_tmpAD;void*_tmpAE;int _tmpAF;void*_tmpB0;void*_tmpB1;void*
_tmpB2;void*_tmpB3;_LL123: _LL12E: _tmpA5=_tmpA1.f1;if((unsigned int)_tmpA5 > 3?*((
int*)_tmpA5)== 5: 0){_LL130: _tmpA7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpA5)->f1;goto _LL12F;_LL12F: _tmpA6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpA5)->f2;goto _LL12B;}else{goto _LL125;}_LL12B: _tmpA2=_tmpA1.f2;if((
unsigned int)_tmpA2 > 1?*((int*)_tmpA2)== 0: 0){_LL12D: _tmpA4=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmpA2)->f1;goto _LL12C;_LL12C: _tmpA3=((struct Cyc_Absyn_Char_c_struct*)_tmpA2)->f2;
goto _LL124;}else{goto _LL125;}_LL125: _LL134: _tmpAB=_tmpA1.f1;if((unsigned int)
_tmpAB > 3?*((int*)_tmpAB)== 5: 0){_LL136: _tmpAD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpAB)->f1;goto _LL135;_LL135: _tmpAC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpAB)->f2;goto _LL131;}else{goto _LL127;}_LL131: _tmpA8=_tmpA1.f2;if((
unsigned int)_tmpA8 > 1?*((int*)_tmpA8)== 1: 0){_LL133: _tmpAA=(void*)((struct Cyc_Absyn_Short_c_struct*)
_tmpA8)->f1;goto _LL132;_LL132: _tmpA9=((struct Cyc_Absyn_Short_c_struct*)_tmpA8)->f2;
goto _LL126;}else{goto _LL127;}_LL127: _LL13A: _tmpB1=_tmpA1.f1;if((unsigned int)
_tmpB1 > 3?*((int*)_tmpB1)== 5: 0){_LL13C: _tmpB3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpB1)->f1;goto _LL13B;_LL13B: _tmpB2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpB1)->f2;goto _LL137;}else{goto _LL129;}_LL137: _tmpAE=_tmpA1.f2;if((
unsigned int)_tmpAE > 1?*((int*)_tmpAE)== 2: 0){_LL139: _tmpB0=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmpAE)->f1;goto _LL138;_LL138: _tmpAF=((struct Cyc_Absyn_Int_c_struct*)_tmpAE)->f2;
goto _LL128;}else{goto _LL129;}_LL129: goto _LL12A;_LL124: _tmpAD=_tmpA7;_tmpAC=
_tmpA6;_tmpAA=_tmpA4;_tmpA9=(short)_tmpA3;goto _LL126;_LL126: _tmpB3=_tmpAD;_tmpB2=
_tmpAC;_tmpB0=_tmpAA;_tmpAF=(int)_tmpA9;goto _LL128;_LL128: if(_tmpB3 != _tmpB0){cn=(
void*)({struct Cyc_Absyn_Int_c_struct*_tmpB4=_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));
_tmpB4[0]=({struct Cyc_Absyn_Int_c_struct _tmpB5;_tmpB5.tag=2;_tmpB5.f1=(void*)
_tmpB3;_tmpB5.f2=_tmpAF;_tmpB5;});_tmpB4;});}goto _LL122;_LL12A:({struct Cyc_Std_String_pa_struct
_tmpB7;_tmpB7.tag=0;_tmpB7.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp91);{
void*_tmpB6[1]={& _tmpB7};Cyc_Tcutil_terr(e->loc,_tag_arr("eval_const: cannot cast to %s",
sizeof(unsigned char),30),_tag_arr(_tmpB6,sizeof(void*),1));}});return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmpB8=_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));
_tmpB8[0]=({struct Cyc_Absyn_Int_c_struct _tmpB9;_tmpB9.tag=2;_tmpB9.f1=(void*)((
void*)0);_tmpB9.f2=0;_tmpB9;});_tmpB8;});_LL122:;}goto _LL103;_LL113: _tmp93=
_tmp92;goto _LL115;_LL115: return Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)
_check_null(((struct Cyc_Absyn_Enumfield*)_check_null(_tmp93))->tag));_LL117:({
void*_tmpBA[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("bad static expression",
sizeof(unsigned char),22),_tag_arr(_tmpBA,sizeof(void*),0));});return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmpBB=_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));
_tmpBB[0]=({struct Cyc_Absyn_Int_c_struct _tmpBC;_tmpBC.tag=2;_tmpBC.f1=(void*)((
void*)0);_tmpBC.f2=0;_tmpBC;});_tmpBB;});_LL103:;}(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Const_e_struct*_tmpBD=_cycalloc(sizeof(struct Cyc_Absyn_Const_e_struct));
_tmpBD[0]=({struct Cyc_Absyn_Const_e_struct _tmpBE;_tmpBE.tag=0;_tmpBE.f1=(void*)
cn;_tmpBE;});_tmpBD;})));return cn;}
