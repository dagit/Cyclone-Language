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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};extern void*Cyc_Core_snd(
struct _tuple0*);extern unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);extern struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
extern unsigned char Cyc_List_Nth[8];struct Cyc_Set_Set;extern struct Cyc_Set_Set*Cyc_Set_insert(
struct Cyc_Set_Set*s,void*elt);extern struct Cyc_Set_Set*Cyc_Set_union_two(struct
Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);extern unsigned char Cyc_Set_Absent[11];
struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern unsigned char
Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));extern struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,
void*v);extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);extern int Cyc_Std_zstrptrcmp(
struct _tagged_arr*,struct _tagged_arr*);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
unsigned char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
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
15];extern struct _tagged_arr*Cyc_Absyn_fieldname(int);struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE;extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_StructRes_struct{
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
le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void Cyc_Tcutil_terr(struct
Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);extern void*
Cyc_Tcutil_compress(void*t);extern void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
extern int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b);extern int Cyc_Tcutil_is_noreturn(
void*);extern unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};extern unsigned char Cyc_CfFlowInfo_IsZero[11];struct Cyc_CfFlowInfo_IsZero_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_CfFlowInfo_NotZero[
12];struct Cyc_CfFlowInfo_NotZero_struct{unsigned char*tag;struct Cyc_List_List*f1;
};extern unsigned char Cyc_CfFlowInfo_UnknownZ[13];struct Cyc_CfFlowInfo_UnknownZ_struct{
unsigned char*tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int
tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int
tag;struct Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};extern struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();
extern void*Cyc_CfFlowInfo_unknown_none;extern void*Cyc_CfFlowInfo_unknown_all;
extern int Cyc_CfFlowInfo_root_cmp(void*,void*);extern void*Cyc_CfFlowInfo_typ_to_absrval(
void*t,void*leafval);extern void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,
void*r);extern void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place);extern int Cyc_CfFlowInfo_flow_lessthan_approx(void*f1,void*f2);extern
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,struct
Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);extern struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);extern struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);extern
struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**
all_changed,void*r);extern struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(struct
Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,
struct Cyc_CfFlowInfo_Place*place,void*r);extern void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**,void*,void*);extern void*Cyc_CfFlowInfo_after_flow(struct Cyc_Set_Set**,
void*,void*,struct Cyc_Set_Set*,struct Cyc_Set_Set*);extern void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);extern void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;void*flow;};static unsigned char Cyc_NewControlFlow_CFAnnot[
12]="\000\000\000\000CFAnnot";struct Cyc_NewControlFlow_CFAnnot_struct{
unsigned char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){(void*)(enclosee->annot=(
void*)((void*)({struct Cyc_NewControlFlow_CFAnnot_struct*_tmp0=_cycalloc(sizeof(
struct Cyc_NewControlFlow_CFAnnot_struct));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct
_tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot
_tmp2;_tmp2.encloser=encloser;_tmp2.visited=0;_tmp2.flow=(void*)((void*)0);_tmp2;});
_tmp1;});_tmp0;})));}static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s){void*_tmp3=(void*)s->annot;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp4;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: if(*((void**)_tmp3)== Cyc_NewControlFlow_CFAnnot){
_LL5: _tmp4=((struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmp5=(struct
Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp3)->f1;goto _LL2;}else{goto _LL3;}_LL3: goto _LL4;_LL2: return _tmp5;_LL4:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp6=_cycalloc(sizeof(struct
Cyc_Core_Impossible_struct));_tmp6[0]=({struct Cyc_Core_Impossible_struct _tmp7;
_tmp7.tag=Cyc_Core_Impossible;_tmp7.f1=_tag_arr("ControlFlow -- wrong stmt annotation",
sizeof(unsigned char),37);_tmp7;});_tmp6;}));_LL0:;}struct Cyc_NewControlFlow_AnalEnv{
int iterate_again;int iteration_num;int in_try;void*tryflow;struct Cyc_Set_Set**
all_changed;int noreturn;};static void*Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Stmt*);static void*Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Decl*);static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,void*,struct Cyc_Absyn_Exp*);static struct
_tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,void*,
struct Cyc_Absyn_Exp*);static struct _tuple0 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Exp*);static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Stmt*s){struct
Cyc_NewControlFlow_CFStmtAnnot*_tmp8=Cyc_NewControlFlow_get_stmt_annot(s);(void*)(
_tmp8->flow=(void*)Cyc_CfFlowInfo_join_flow(env->all_changed,inflow,(void*)_tmp8->flow));
_tmp8->visited=env->iteration_num;return _tmp8;}static void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv*env,void*new_flow){if(env->in_try){(void*)(env->tryflow=(
void*)Cyc_CfFlowInfo_join_flow(env->all_changed,new_flow,(void*)env->tryflow));}}
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_Absyn_Stmt*s,void*flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp9=Cyc_NewControlFlow_get_stmt_annot(
s);void*_tmpA=Cyc_CfFlowInfo_join_flow(env->all_changed,flow,(void*)_tmp9->flow);
if(! Cyc_CfFlowInfo_flow_lessthan_approx(_tmpA,(void*)_tmp9->flow)){(void*)(_tmp9->flow=(
void*)_tmpA);if(_tmp9->visited == env->iteration_num){env->iterate_again=1;}}}
static void*Cyc_NewControlFlow_add_vars(void*inflow,struct Cyc_List_List*vds,void*
leafval){void*_tmpB=inflow;struct Cyc_List_List*_tmpC;struct Cyc_Dict_Dict*_tmpD;
_LL7: if((int)_tmpB == 0){goto _LL8;}else{goto _LL9;}_LL9: if((unsigned int)_tmpB > 1?*((
int*)_tmpB)== 0: 0){_LLC: _tmpD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpB)->f1;
goto _LLB;_LLB: _tmpC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpB)->f2;goto
_LLA;}else{goto _LL6;}_LL8: return(void*)0;_LLA: for(0;vds != 0;vds=vds->tl){struct
Cyc_CfFlowInfo_VarRoot_struct*_tmpE=({struct Cyc_CfFlowInfo_VarRoot_struct*_tmpF=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_struct));_tmpF[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp10;_tmp10.tag=0;_tmp10.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp10;});_tmpF;});
_tmpD=Cyc_Dict_insert(_tmpD,(void*)_tmpE,Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp11=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp11[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp12;_tmp12.tag=0;_tmp12.f1=_tmpD;
_tmp12.f2=_tmpC;_tmp12;});_tmp11;});_LL6:;}static void*Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*inflow,
void*r){void*_tmp13=inflow;struct Cyc_List_List*_tmp14;struct Cyc_Dict_Dict*_tmp15;
_LLE: if((int)_tmp13 == 0){goto _LLF;}else{goto _LL10;}_LL10: if((unsigned int)_tmp13
> 1?*((int*)_tmp13)== 0: 0){_LL13: _tmp15=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp13)->f1;goto _LL12;_LL12: _tmp14=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp13)->f2;goto _LL11;}else{goto _LLD;}_LLF: return(void*)0;_LL11: if(Cyc_CfFlowInfo_initlevel(
_tmp15,r)!= (void*)2){({void*_tmp16[0]={};Cyc_Tcutil_terr(loc,_tag_arr("expression may not be fully initialized",
sizeof(unsigned char),40),_tag_arr(_tmp16,sizeof(void*),0));});}{struct Cyc_Dict_Dict*
_tmp17=Cyc_CfFlowInfo_escape_deref(_tmp15,env->all_changed,r);if(_tmp15 == _tmp17){
return inflow;}{struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp18=({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp19=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp19[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp1A;_tmp1A.tag=0;_tmp1A.f1=_tmp17;
_tmp1A.f2=_tmp14;_tmp1A;});_tmp19;});Cyc_NewControlFlow_update_tryflow(env,(void*)
_tmp18);return(void*)_tmp18;}}_LLD:;}struct _tuple4{void*f1;struct Cyc_List_List*
f2;};static struct _tuple4 Cyc_NewControlFlow_anal_unordered_Rexps(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es){if(es == 0){return({struct _tuple4 _tmp1B;
_tmp1B.f1=inflow;_tmp1B.f2=0;_tmp1B;});}if(es->tl == 0){void*_tmp1D;void*_tmp1E;
struct _tuple0 _tmp1C=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_LL16: _tmp1E=_tmp1C.f1;goto _LL15;_LL15: _tmp1D=_tmp1C.f2;goto _LL14;_LL14:
return({struct _tuple4 _tmp1F;_tmp1F.f1=_tmp1E;_tmp1F.f2=({struct Cyc_List_List*
_tmp20=_cycalloc(sizeof(struct Cyc_List_List));_tmp20->hd=(void*)_tmp1D;_tmp20->tl=
0;_tmp20;});_tmp1F;});}{struct Cyc_Set_Set**outer_all_changed=env->all_changed;
struct Cyc_Set_Set*this_all_changed;void*old_inflow;void*outflow;struct Cyc_List_List*
rvals;do {this_all_changed=Cyc_CfFlowInfo_mt_place_set();env->all_changed=({
struct Cyc_Set_Set**_tmp21=_cycalloc(sizeof(struct Cyc_Set_Set*));_tmp21[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp21;});{void*_tmp23;void*_tmp24;struct _tuple0 _tmp22=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)es->hd);_LL19: _tmp24=_tmp22.f1;goto _LL18;_LL18:
_tmp23=_tmp22.f2;goto _LL17;_LL17: outflow=_tmp24;rvals=({struct Cyc_List_List*
_tmp25=_cycalloc(sizeof(struct Cyc_List_List));_tmp25->hd=(void*)_tmp23;_tmp25->tl=
0;_tmp25;});this_all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct
Cyc_Set_Set*s2))Cyc_Set_union_two)(this_all_changed,*((struct Cyc_Set_Set**)
_check_null(env->all_changed)));{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;
es2=es2->tl){env->all_changed=({struct Cyc_Set_Set**_tmp26=_cycalloc(sizeof(
struct Cyc_Set_Set*));_tmp26[0]=Cyc_CfFlowInfo_mt_place_set();_tmp26;});{void*
_tmp28;void*_tmp29;struct _tuple0 _tmp27=Cyc_NewControlFlow_anal_Rexp(env,inflow,(
struct Cyc_Absyn_Exp*)es2->hd);_LL1C: _tmp29=_tmp27.f1;goto _LL1B;_LL1B: _tmp28=
_tmp27.f2;goto _LL1A;_LL1A: rvals=({struct Cyc_List_List*_tmp2A=_cycalloc(sizeof(
struct Cyc_List_List));_tmp2A->hd=(void*)_tmp28;_tmp2A->tl=rvals;_tmp2A;});
outflow=Cyc_CfFlowInfo_after_flow((struct Cyc_Set_Set**)& this_all_changed,outflow,
_tmp29,this_all_changed,*((struct Cyc_Set_Set**)_check_null(env->all_changed)));
this_all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(this_all_changed,*((struct Cyc_Set_Set**)_check_null(env->all_changed)));}}}
old_inflow=inflow;inflow=Cyc_CfFlowInfo_join_flow(outer_all_changed,inflow,
outflow);}} while (! Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));if(
outer_all_changed == 0){env->all_changed=0;}else{env->all_changed=({struct Cyc_Set_Set**
_tmp2B=_cycalloc(sizeof(struct Cyc_Set_Set*));_tmp2B[0]=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*outer_all_changed,
this_all_changed);_tmp2B;});}Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple4 _tmp2C;_tmp2C.f1=outflow;_tmp2C.f2=Cyc_List_imp_rev(rvals);
_tmp2C;});}}static struct _tuple0 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es){struct Cyc_List_List*_tmp2E;void*_tmp2F;
struct _tuple4 _tmp2D=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,es);_LL1F:
_tmp2F=_tmp2D.f1;goto _LL1E;_LL1E: _tmp2E=_tmp2D.f2;goto _LL1D;_LL1D:{void*_tmp30=
_tmp2F;struct Cyc_List_List*_tmp31;struct Cyc_Dict_Dict*_tmp32;_LL21: if((
unsigned int)_tmp30 > 1?*((int*)_tmp30)== 0: 0){_LL26: _tmp32=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp30)->f1;goto _LL25;_LL25: _tmp31=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp30)->f2;goto _LL22;}else{goto _LL23;}_LL23: if((int)_tmp30 == 0){goto _LL24;}
else{goto _LL20;}_LL22: for(0;_tmp2E != 0;(_tmp2E=_tmp2E->tl,es=((struct Cyc_List_List*)
_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(_tmp32,(void*)_tmp2E->hd)== (
void*)0){({void*_tmp33[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,_tag_arr("expression may not be initialized",sizeof(
unsigned char),34),_tag_arr(_tmp33,sizeof(void*),0));});}}goto _LL20;_LL24: goto
_LL20;_LL20:;}return({struct _tuple0 _tmp34;_tmp34.f1=_tmp2F;_tmp34.f2=Cyc_CfFlowInfo_unknown_all;
_tmp34;});}static void*Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*outflow,struct Cyc_Absyn_Exp*e,void*il){void*_tmp35=outflow;
struct Cyc_List_List*_tmp36;struct Cyc_Dict_Dict*_tmp37;_LL28: if((int)_tmp35 == 0){
goto _LL29;}else{goto _LL2A;}_LL2A: if((unsigned int)_tmp35 > 1?*((int*)_tmp35)== 0:
0){_LL2D: _tmp37=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp35)->f1;goto _LL2C;
_LL2C: _tmp36=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp35)->f2;goto _LL2B;}
else{goto _LL27;}_LL29: return outflow;_LL2B: {void*_tmp38=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp39;_LL2F: if((int)_tmp38 == 0){
goto _LL30;}else{goto _LL31;}_LL31: if((unsigned int)_tmp38 > 1?*((int*)_tmp38)== 0:
0){_LL33: _tmp39=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp38)->f1;goto _LL32;}
else{goto _LL2E;}_LL30: return outflow;_LL32: {void*nzval=il == (void*)2?(void*)1:(
void*)2;return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3A=_cycalloc(
sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp3A[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp3B;_tmp3B.tag=0;_tmp3B.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp37,env->all_changed,
_tmp39,nzval);_tmp3B.f2=_tmp36;_tmp3B;});_tmp3A;});}_LL2E:;}_LL27:;}static struct
_tuple0 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,void*outflow,struct Cyc_Absyn_Exp*e,void*il){void*_tmp3C=outflow;struct Cyc_List_List*
_tmp3D;struct Cyc_Dict_Dict*_tmp3E;_LL35: if((int)_tmp3C == 0){goto _LL36;}else{goto
_LL37;}_LL37: if((unsigned int)_tmp3C > 1?*((int*)_tmp3C)== 0: 0){_LL3A: _tmp3E=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3C)->f1;goto _LL39;_LL39: _tmp3D=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3C)->f2;goto _LL38;}else{goto _LL34;}
_LL36: return({struct _tuple0 _tmp3F;_tmp3F.f1=outflow;_tmp3F.f2=outflow;_tmp3F;});
_LL38: {void*_tmp40=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*
_tmp41;_LL3C: if((int)_tmp40 == 0){goto _LL3D;}else{goto _LL3E;}_LL3E: if((
unsigned int)_tmp40 > 1?*((int*)_tmp40)== 0: 0){_LL40: _tmp41=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp40)->f1;goto _LL3F;}else{goto _LL3B;}_LL3D: return({struct _tuple0 _tmp42;_tmp42.f1=
outflow;_tmp42.f2=outflow;_tmp42;});_LL3F: {void*nzval=il == (void*)2?(void*)1:(
void*)2;return({struct _tuple0 _tmp43;_tmp43.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp46=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp46[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp47;_tmp47.tag=0;_tmp47.f1=Cyc_CfFlowInfo_assign_place(
e->loc,_tmp3E,env->all_changed,_tmp41,nzval);_tmp47.f2=_tmp3D;_tmp47;});_tmp46;});
_tmp43.f2=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp44=_cycalloc(
sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp44[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp45;_tmp45.tag=0;_tmp45.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp3E,env->all_changed,
_tmp41,(void*)0);_tmp45.f2=_tmp3D;_tmp45;});_tmp44;});_tmp43;});}_LL3B:;}_LL34:;}
static struct _tuple0 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp48=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp49;struct Cyc_Absyn_Conref*_tmp4A;void*_tmp4B;_LL42: if((unsigned int)_tmp48 > 
3?*((int*)_tmp48)== 4: 0){_LL46: _tmp49=((struct Cyc_Absyn_PointerType_struct*)
_tmp48)->f1;_LL48: _tmp4B=(void*)_tmp49.elt_typ;goto _LL47;_LL47: _tmp4A=_tmp49.bounds;
goto _LL43;}else{goto _LL44;}_LL44: goto _LL45;_LL43: {void*_tmp4C=f;struct Cyc_List_List*
_tmp4D;struct Cyc_Dict_Dict*_tmp4E;_LL4A: if((int)_tmp4C == 0){goto _LL4B;}else{goto
_LL4C;}_LL4C: if((unsigned int)_tmp4C > 1?*((int*)_tmp4C)== 0: 0){_LL4F: _tmp4E=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4C)->f1;goto _LL4E;_LL4E: _tmp4D=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4C)->f2;goto _LL4D;}else{goto _LL49;}
_LL4B: return({struct _tuple0 _tmp4F;_tmp4F.f1=f;_tmp4F.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp4B,Cyc_CfFlowInfo_unknown_all);_tmp4F;});_LL4D: if(Cyc_Tcutil_is_bound_one(
_tmp4A)){void*_tmp50=r;struct Cyc_CfFlowInfo_Place*_tmp51;void*_tmp52;_LL51: if((
int)_tmp50 == 1){goto _LL52;}else{goto _LL53;}_LL53: if((int)_tmp50 == 2){goto _LL54;}
else{goto _LL55;}_LL55: if((unsigned int)_tmp50 > 3?*((int*)_tmp50)== 2: 0){_LL5D:
_tmp51=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp50)->f1;goto _LL56;}else{goto
_LL57;}_LL57: if((int)_tmp50 == 0){goto _LL58;}else{goto _LL59;}_LL59: if((
unsigned int)_tmp50 > 3?*((int*)_tmp50)== 0: 0){_LL5E: _tmp52=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp50)->f1;goto _LL5A;}else{goto _LL5B;}_LL5B: goto _LL5C;_LL52: goto _LL54;_LL54:(
void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp53=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_NotZero_struct));_tmp53[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp54;_tmp54.tag=Cyc_CfFlowInfo_NotZero;_tmp54.f1=_tmp4D;_tmp54;});_tmp53;})));
goto _LL50;_LL56:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp55=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NotZero_struct));_tmp55[0]=({struct
Cyc_CfFlowInfo_NotZero_struct _tmp56;_tmp56.tag=Cyc_CfFlowInfo_NotZero;_tmp56.f1=
_tmp4D;_tmp56;});_tmp55;})));return({struct _tuple0 _tmp57;_tmp57.f1=f;_tmp57.f2=
Cyc_CfFlowInfo_lookup_place(_tmp4E,_tmp51);_tmp57;});_LL58:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_IsZero_struct*_tmp58=_cycalloc(sizeof(
struct Cyc_CfFlowInfo_IsZero_struct));_tmp58[0]=({struct Cyc_CfFlowInfo_IsZero_struct
_tmp59;_tmp59.tag=Cyc_CfFlowInfo_IsZero;_tmp59.f1=_tmp4D;_tmp59;});_tmp58;})));
return({struct _tuple0 _tmp5A;_tmp5A.f1=(void*)0;_tmp5A.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp4B,Cyc_CfFlowInfo_unknown_all);_tmp5A;});_LL5A: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp52);goto _LL5C;_LL5C:(void*)(e->annot=(void*)((void*)({struct
Cyc_CfFlowInfo_UnknownZ_struct*_tmp5B=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownZ_struct));
_tmp5B[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp5C;_tmp5C.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp5C.f1=_tmp4D;_tmp5C;});_tmp5B;})));_LL50:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp5D=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownZ_struct));
_tmp5D[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp5E;_tmp5E.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp5E.f1=_tmp4D;_tmp5E;});_tmp5D;})));}{void*_tmp5F=Cyc_CfFlowInfo_initlevel(
_tmp4E,r);_LL60: if((int)_tmp5F == 0){goto _LL61;}else{goto _LL62;}_LL62: if((int)
_tmp5F == 2){goto _LL63;}else{goto _LL64;}_LL64: if((int)_tmp5F == 1){goto _LL65;}
else{goto _LL5F;}_LL61:({void*_tmp60[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of possibly uninitialized pointer",
sizeof(unsigned char),46),_tag_arr(_tmp60,sizeof(void*),0));});goto _LL63;_LL63:
return({struct _tuple0 _tmp61;_tmp61.f1=f;_tmp61.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp4B,Cyc_CfFlowInfo_unknown_all);_tmp61;});_LL65: return({struct _tuple0 _tmp62;
_tmp62.f1=f;_tmp62.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp4B,Cyc_CfFlowInfo_unknown_none);
_tmp62;});_LL5F:;}_LL49:;}_LL45:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp63=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp63[0]=({struct Cyc_Core_Impossible_struct
_tmp64;_tmp64.tag=Cyc_Core_Impossible;_tmp64.f1=_tag_arr("right deref of non-pointer-type",
sizeof(unsigned char),32);_tmp64;});_tmp63;}));_LL41:;}static struct Cyc_List_List*
Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*_tmp65=(void*)e1->r;void*_tmp66;struct Cyc_Absyn_Vardecl*
_tmp67;void*_tmp68;struct Cyc_Absyn_Vardecl*_tmp69;void*_tmp6A;struct Cyc_Absyn_Vardecl*
_tmp6B;void*_tmp6C;struct Cyc_Absyn_Vardecl*_tmp6D;_LL67: if(*((int*)_tmp65)== 1){
_LL71: _tmp66=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp65)->f2;if((unsigned int)
_tmp66 > 1?*((int*)_tmp66)== 4: 0){_LL72: _tmp67=((struct Cyc_Absyn_Pat_b_struct*)
_tmp66)->f1;goto _LL68;}else{goto _LL69;}}else{goto _LL69;}_LL69: if(*((int*)_tmp65)
== 1){_LL73: _tmp68=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp65)->f2;if((
unsigned int)_tmp68 > 1?*((int*)_tmp68)== 3: 0){_LL74: _tmp69=((struct Cyc_Absyn_Local_b_struct*)
_tmp68)->f1;goto _LL6A;}else{goto _LL6B;}}else{goto _LL6B;}_LL6B: if(*((int*)_tmp65)
== 1){_LL75: _tmp6A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp65)->f2;if((
unsigned int)_tmp6A > 1?*((int*)_tmp6A)== 2: 0){_LL76: _tmp6B=((struct Cyc_Absyn_Param_b_struct*)
_tmp6A)->f1;goto _LL6C;}else{goto _LL6D;}}else{goto _LL6D;}_LL6D: if(*((int*)_tmp65)
== 1){_LL77: _tmp6C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp65)->f2;if((
unsigned int)_tmp6C > 1?*((int*)_tmp6C)== 0: 0){_LL78: _tmp6D=((struct Cyc_Absyn_Global_b_struct*)
_tmp6C)->f1;goto _LL6E;}else{goto _LL6F;}}else{goto _LL6F;}_LL6F: goto _LL70;_LL68:
_tmp69=_tmp67;goto _LL6A;_LL6A: _tmp6B=_tmp69;goto _LL6C;_LL6C: _tmp6D=_tmp6B;goto
_LL6E;_LL6E: if(! _tmp6D->escapes){void*_tmp6E=(void*)e2->r;void*_tmp6F;struct Cyc_Absyn_Vardecl*
_tmp70;void*_tmp71;struct Cyc_Absyn_Vardecl*_tmp72;void*_tmp73;struct Cyc_Absyn_Vardecl*
_tmp74;void*_tmp75;struct Cyc_Absyn_Vardecl*_tmp76;_LL7A: if(*((int*)_tmp6E)== 1){
_LL84: _tmp6F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6E)->f2;if((unsigned int)
_tmp6F > 1?*((int*)_tmp6F)== 4: 0){_LL85: _tmp70=((struct Cyc_Absyn_Pat_b_struct*)
_tmp6F)->f1;goto _LL7B;}else{goto _LL7C;}}else{goto _LL7C;}_LL7C: if(*((int*)_tmp6E)
== 1){_LL86: _tmp71=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6E)->f2;if((
unsigned int)_tmp71 > 1?*((int*)_tmp71)== 3: 0){_LL87: _tmp72=((struct Cyc_Absyn_Local_b_struct*)
_tmp71)->f1;goto _LL7D;}else{goto _LL7E;}}else{goto _LL7E;}_LL7E: if(*((int*)_tmp6E)
== 1){_LL88: _tmp73=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6E)->f2;if((
unsigned int)_tmp73 > 1?*((int*)_tmp73)== 2: 0){_LL89: _tmp74=((struct Cyc_Absyn_Param_b_struct*)
_tmp73)->f1;goto _LL7F;}else{goto _LL80;}}else{goto _LL80;}_LL80: if(*((int*)_tmp6E)
== 1){_LL8A: _tmp75=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6E)->f2;if((
unsigned int)_tmp75 > 1?*((int*)_tmp75)== 0: 0){_LL8B: _tmp76=((struct Cyc_Absyn_Global_b_struct*)
_tmp75)->f1;goto _LL81;}else{goto _LL82;}}else{goto _LL82;}_LL82: goto _LL83;_LL7B:
_tmp72=_tmp70;goto _LL7D;_LL7D: _tmp74=_tmp72;goto _LL7F;_LL7F: _tmp76=_tmp74;goto
_LL81;_LL81: if(! _tmp76->escapes){int found=0;{struct Cyc_List_List*_tmp77=relns;
for(0;_tmp77 != 0;_tmp77=_tmp77->tl){struct Cyc_CfFlowInfo_Reln*_tmp78=(struct Cyc_CfFlowInfo_Reln*)
_tmp77->hd;if(_tmp78->vd == _tmp76){void*_tmp79=(void*)_tmp78->rop;struct Cyc_Absyn_Vardecl*
_tmp7A;_LL8D: if(*((int*)_tmp79)== 2){_LL92: _tmp7A=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp79)->f1;goto _LL91;}else{goto _LL8F;}_LL91: if(_tmp7A == _tmp6D){goto _LL8E;}
else{goto _LL8F;}_LL8F: goto _LL90;_LL8E: return relns;_LL90: continue;_LL8C:;}}}if(!
found){return({struct Cyc_List_List*_tmp7B=_cycalloc(sizeof(struct Cyc_List_List));
_tmp7B->hd=({struct Cyc_CfFlowInfo_Reln*_tmp7C=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));
_tmp7C->vd=_tmp76;_tmp7C->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp7D=_cycalloc(sizeof(struct Cyc_CfFlowInfo_LessSize_struct));_tmp7D[0]=({
struct Cyc_CfFlowInfo_LessSize_struct _tmp7E;_tmp7E.tag=2;_tmp7E.f1=_tmp6D;_tmp7E;});
_tmp7D;}));_tmp7C;});_tmp7B->tl=relns;_tmp7B;});}}return relns;_LL83: return relns;
_LL79:;}else{return relns;}_LL70: return relns;_LL66:;}struct _tuple5{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e){struct
Cyc_Dict_Dict*d;struct Cyc_List_List*relns;{void*_tmp7F=inflow;struct Cyc_List_List*
_tmp80;struct Cyc_Dict_Dict*_tmp81;_LL94: if((int)_tmp7F == 0){goto _LL95;}else{goto
_LL96;}_LL96: if((unsigned int)_tmp7F > 1?*((int*)_tmp7F)== 0: 0){_LL99: _tmp81=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp7F)->f1;goto _LL98;_LL98: _tmp80=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp7F)->f2;goto _LL97;}else{goto _LL93;}
_LL95: return({struct _tuple0 _tmp82;_tmp82.f1=(void*)0;_tmp82.f2=Cyc_CfFlowInfo_unknown_all;
_tmp82;});_LL97: d=_tmp81;relns=_tmp80;_LL93:;}{void*_tmp83=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp84;struct Cyc_Absyn_Exp*_tmp85;struct Cyc_Absyn_Exp*_tmp86;void*_tmp87;void*
_tmp88;int _tmp89;void*_tmp8A;void*_tmp8B;void*_tmp8C;void*_tmp8D;struct Cyc_List_List*
_tmp8E;void*_tmp8F;void*_tmp90;struct Cyc_Absyn_Vardecl*_tmp91;void*_tmp92;struct
Cyc_Absyn_Vardecl*_tmp93;void*_tmp94;struct Cyc_Absyn_Vardecl*_tmp95;struct Cyc_List_List*
_tmp96;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Exp*_tmp98;struct Cyc_Core_Opt*
_tmp99;struct Cyc_Core_Opt _tmp9A;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Exp*
_tmp9C;struct Cyc_Core_Opt*_tmp9D;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp*
_tmp9F;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Exp*_tmpA1;struct Cyc_List_List*
_tmpA2;struct Cyc_Absyn_Exp*_tmpA3;struct Cyc_Absyn_MallocInfo _tmpA4;int _tmpA5;
struct Cyc_Absyn_Exp*_tmpA6;void**_tmpA7;struct Cyc_Absyn_Exp*_tmpA8;int _tmpA9;
struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Exp*_tmpAB;struct Cyc_Absyn_Exp*_tmpAC;
struct Cyc_Absyn_Exp*_tmpAD;struct _tagged_arr*_tmpAE;struct Cyc_Absyn_Exp*_tmpAF;
struct _tagged_arr*_tmpB0;struct Cyc_Absyn_Exp*_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;
struct Cyc_Absyn_Exp*_tmpB3;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;
struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_List_List*_tmpB7;struct Cyc_List_List*_tmpB8;
struct Cyc_List_List*_tmpB9;struct Cyc_List_List*_tmpBA;struct Cyc_List_List*_tmpBB;
struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Vardecl*
_tmpBE;struct Cyc_Absyn_Stmt*_tmpBF;void*_tmpC0;_LL9B: if(*((int*)_tmp83)== 13){
_LLFB: _tmp84=((struct Cyc_Absyn_Cast_e_struct*)_tmp83)->f2;goto _LL9C;}else{goto
_LL9D;}_LL9D: if(*((int*)_tmp83)== 11){_LLFC: _tmp85=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp83)->f1;goto _LL9E;}else{goto _LL9F;}_LL9F: if(*((int*)_tmp83)== 12){_LLFD:
_tmp86=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp83)->f1;goto _LLA0;}else{goto
_LLA1;}_LLA1: if(*((int*)_tmp83)== 0){_LLFE: _tmp87=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp83)->f1;if((int)_tmp87 == 0){goto _LLA2;}else{goto _LLA3;}}else{goto _LLA3;}
_LLA3: if(*((int*)_tmp83)== 0){_LLFF: _tmp88=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp83)->f1;if((unsigned int)_tmp88 > 1?*((int*)_tmp88)== 2: 0){_LL101: _tmp8A=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp88)->f1;if((int)_tmp8A == 0){goto _LL100;}
else{goto _LLA5;}_LL100: _tmp89=((struct Cyc_Absyn_Int_c_struct*)_tmp88)->f2;if(
_tmp89 == 0){goto _LLA4;}else{goto _LLA5;}}else{goto _LLA5;}}else{goto _LLA5;}_LLA5:
if(*((int*)_tmp83)== 0){_LL102: _tmp8B=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp83)->f1;if((unsigned int)_tmp8B > 1?*((int*)_tmp8B)== 2: 0){_LL103: _tmp8C=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp8B)->f1;if((int)_tmp8C == 0){goto _LLA6;}
else{goto _LLA7;}}else{goto _LLA7;}}else{goto _LLA7;}_LLA7: if(*((int*)_tmp83)== 1){
_LL104: _tmp8D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp83)->f2;if((
unsigned int)_tmp8D > 1?*((int*)_tmp8D)== 1: 0){goto _LLA8;}else{goto _LLA9;}}else{
goto _LLA9;}_LLA9: if(*((int*)_tmp83)== 30){_LL105: _tmp8E=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp83)->f3;if(_tmp8E == 0){goto _LLAA;}else{goto _LLAB;}}else{goto _LLAB;}_LLAB: if(*((
int*)_tmp83)== 0){goto _LLAC;}else{goto _LLAD;}_LLAD: if(*((int*)_tmp83)== 17){goto
_LLAE;}else{goto _LLAF;}_LLAF: if(*((int*)_tmp83)== 16){goto _LLB0;}else{goto _LLB1;}
_LLB1: if(*((int*)_tmp83)== 18){goto _LLB2;}else{goto _LLB3;}_LLB3: if(*((int*)
_tmp83)== 19){goto _LLB4;}else{goto _LLB5;}_LLB5: if(*((int*)_tmp83)== 32){goto
_LLB6;}else{goto _LLB7;}_LLB7: if(*((int*)_tmp83)== 31){goto _LLB8;}else{goto _LLB9;}
_LLB9: if(*((int*)_tmp83)== 1){_LL106: _tmp8F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp83)->f2;if((unsigned int)_tmp8F > 1?*((int*)_tmp8F)== 0: 0){goto _LLBA;}else{
goto _LLBB;}}else{goto _LLBB;}_LLBB: if(*((int*)_tmp83)== 1){_LL107: _tmp90=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp83)->f2;if((unsigned int)_tmp90 > 1?*((int*)
_tmp90)== 2: 0){_LL108: _tmp91=((struct Cyc_Absyn_Param_b_struct*)_tmp90)->f1;goto
_LLBC;}else{goto _LLBD;}}else{goto _LLBD;}_LLBD: if(*((int*)_tmp83)== 1){_LL109:
_tmp92=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp83)->f2;if((unsigned int)
_tmp92 > 1?*((int*)_tmp92)== 3: 0){_LL10A: _tmp93=((struct Cyc_Absyn_Local_b_struct*)
_tmp92)->f1;goto _LLBE;}else{goto _LLBF;}}else{goto _LLBF;}_LLBF: if(*((int*)_tmp83)
== 1){_LL10B: _tmp94=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp83)->f2;if((
unsigned int)_tmp94 > 1?*((int*)_tmp94)== 4: 0){_LL10C: _tmp95=((struct Cyc_Absyn_Pat_b_struct*)
_tmp94)->f1;goto _LLC0;}else{goto _LLC1;}}else{goto _LLC1;}_LLC1: if(*((int*)_tmp83)
== 3){_LL10D: _tmp96=((struct Cyc_Absyn_Primop_e_struct*)_tmp83)->f2;goto _LLC2;}
else{goto _LLC3;}_LLC3: if(*((int*)_tmp83)== 5){_LL10E: _tmp97=((struct Cyc_Absyn_Increment_e_struct*)
_tmp83)->f1;goto _LLC4;}else{goto _LLC5;}_LLC5: if(*((int*)_tmp83)== 4){_LL111:
_tmp9B=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f1;goto _LL110;_LL110: _tmp99=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f2;if(_tmp99 == 0){goto _LLC7;}else{
_tmp9A=*_tmp99;goto _LL10F;}_LL10F: _tmp98=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp83)->f3;goto _LLC6;}else{goto _LLC7;}_LLC7: if(*((int*)_tmp83)== 4){_LL114:
_tmp9E=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f1;goto _LL113;_LL113: _tmp9D=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f2;if(_tmp9D == 0){goto _LL112;}else{
goto _LLC9;}_LL112: _tmp9C=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f3;goto
_LLC8;}else{goto _LLC9;}_LLC9: if(*((int*)_tmp83)== 7){_LL116: _tmpA0=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp83)->f1;goto _LL115;_LL115: _tmp9F=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp83)->f2;
goto _LLCA;}else{goto _LLCB;}_LLCB: if(*((int*)_tmp83)== 10){_LL117: _tmpA1=((struct
Cyc_Absyn_Throw_e_struct*)_tmp83)->f1;goto _LLCC;}else{goto _LLCD;}_LLCD: if(*((int*)
_tmp83)== 9){_LL119: _tmpA3=((struct Cyc_Absyn_FnCall_e_struct*)_tmp83)->f1;goto
_LL118;_LL118: _tmpA2=((struct Cyc_Absyn_FnCall_e_struct*)_tmp83)->f2;goto _LLCE;}
else{goto _LLCF;}_LLCF: if(*((int*)_tmp83)== 33){_LL11A: _tmpA4=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp83)->f1;_LL11F: _tmpA9=_tmpA4.is_calloc;goto _LL11E;_LL11E: _tmpA8=_tmpA4.rgn;
goto _LL11D;_LL11D: _tmpA7=_tmpA4.elt_type;goto _LL11C;_LL11C: _tmpA6=_tmpA4.num_elts;
goto _LL11B;_LL11B: _tmpA5=_tmpA4.fat_result;goto _LLD0;}else{goto _LLD1;}_LLD1: if(*((
int*)_tmp83)== 15){_LL121: _tmpAB=((struct Cyc_Absyn_New_e_struct*)_tmp83)->f1;
goto _LL120;_LL120: _tmpAA=((struct Cyc_Absyn_New_e_struct*)_tmp83)->f2;goto _LLD2;}
else{goto _LLD3;}_LLD3: if(*((int*)_tmp83)== 14){_LL122: _tmpAC=((struct Cyc_Absyn_Address_e_struct*)
_tmp83)->f1;goto _LLD4;}else{goto _LLD5;}_LLD5: if(*((int*)_tmp83)== 20){_LL123:
_tmpAD=((struct Cyc_Absyn_Deref_e_struct*)_tmp83)->f1;goto _LLD6;}else{goto _LLD7;}
_LLD7: if(*((int*)_tmp83)== 21){_LL125: _tmpAF=((struct Cyc_Absyn_StructMember_e_struct*)
_tmp83)->f1;goto _LL124;_LL124: _tmpAE=((struct Cyc_Absyn_StructMember_e_struct*)
_tmp83)->f2;goto _LLD8;}else{goto _LLD9;}_LLD9: if(*((int*)_tmp83)== 22){_LL127:
_tmpB1=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp83)->f1;goto _LL126;_LL126:
_tmpB0=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp83)->f2;goto _LLDA;}else{goto
_LLDB;}_LLDB: if(*((int*)_tmp83)== 6){_LL12A: _tmpB4=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp83)->f1;goto _LL129;_LL129: _tmpB3=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp83)->f2;goto _LL128;_LL128: _tmpB2=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp83)->f3;goto _LLDC;}else{goto _LLDD;}_LLDD: if(*((int*)_tmp83)== 23){_LL12C:
_tmpB6=((struct Cyc_Absyn_Subscript_e_struct*)_tmp83)->f1;goto _LL12B;_LL12B:
_tmpB5=((struct Cyc_Absyn_Subscript_e_struct*)_tmp83)->f2;goto _LLDE;}else{goto
_LLDF;}_LLDF: if(*((int*)_tmp83)== 30){_LL12D: _tmpB7=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp83)->f3;goto _LLE0;}else{goto _LLE1;}_LLE1: if(*((int*)_tmp83)== 24){_LL12E:
_tmpB8=((struct Cyc_Absyn_Tuple_e_struct*)_tmp83)->f1;goto _LLE2;}else{goto _LLE3;}
_LLE3: if(*((int*)_tmp83)== 29){_LL12F: _tmpB9=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp83)->f2;goto _LLE4;}else{goto _LLE5;}_LLE5: if(*((int*)_tmp83)== 28){_LL130:
_tmpBA=((struct Cyc_Absyn_Struct_e_struct*)_tmp83)->f3;goto _LLE6;}else{goto _LLE7;}
_LLE7: if(*((int*)_tmp83)== 26){_LL131: _tmpBB=((struct Cyc_Absyn_Array_e_struct*)
_tmp83)->f1;goto _LLE8;}else{goto _LLE9;}_LLE9: if(*((int*)_tmp83)== 27){_LL134:
_tmpBE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp83)->f1;goto _LL133;_LL133:
_tmpBD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp83)->f2;goto _LL132;_LL132:
_tmpBC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp83)->f3;goto _LLEA;}else{
goto _LLEB;}_LLEB: if(*((int*)_tmp83)== 35){_LL135: _tmpBF=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp83)->f1;goto _LLEC;}else{goto _LLED;}_LLED: if(*((int*)_tmp83)== 1){_LL136:
_tmpC0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp83)->f2;if((int)_tmpC0 == 0){
goto _LLEE;}else{goto _LLEF;}}else{goto _LLEF;}_LLEF: if(*((int*)_tmp83)== 2){goto
_LLF0;}else{goto _LLF1;}_LLF1: if(*((int*)_tmp83)== 8){goto _LLF2;}else{goto _LLF3;}
_LLF3: if(*((int*)_tmp83)== 34){goto _LLF4;}else{goto _LLF5;}_LLF5: if(*((int*)
_tmp83)== 25){goto _LLF6;}else{goto _LLF7;}_LLF7: if(*((int*)_tmp83)== 36){goto
_LLF8;}else{goto _LLF9;}_LLF9: if(*((int*)_tmp83)== 37){goto _LLFA;}else{goto _LL9A;}
_LL9C: _tmp85=_tmp84;goto _LL9E;_LL9E: _tmp86=_tmp85;goto _LLA0;_LLA0: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp86);_LLA2: goto _LLA4;_LLA4: return({struct _tuple0 _tmpC1;_tmpC1.f1=
inflow;_tmpC1.f2=(void*)0;_tmpC1;});_LLA6: goto _LLA8;_LLA8: return({struct _tuple0
_tmpC2;_tmpC2.f1=inflow;_tmpC2.f2=(void*)1;_tmpC2;});_LLAA: goto _LLAC;_LLAC: goto
_LLAE;_LLAE: goto _LLB0;_LLB0: goto _LLB2;_LLB2: goto _LLB4;_LLB4: goto _LLB6;_LLB6: goto
_LLB8;_LLB8: return({struct _tuple0 _tmpC3;_tmpC3.f1=inflow;_tmpC3.f2=Cyc_CfFlowInfo_unknown_all;
_tmpC3;});_LLBA: return({struct _tuple0 _tmpC4;_tmpC4.f1=inflow;_tmpC4.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmpC4;});_LLBC: _tmp93=_tmp91;goto _LLBE;_LLBE: _tmp95=_tmp93;goto _LLC0;_LLC0:
return({struct _tuple0 _tmpC5;_tmpC5.f1=inflow;_tmpC5.f2=Cyc_Dict_lookup(d,(void*)({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmpC6=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_struct));
_tmpC6[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmpC7;_tmpC7.tag=0;_tmpC7.f1=
_tmp95;_tmpC7;});_tmpC6;}));_tmpC5;});_LLC2: return Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmp96);_LLC4: {void*_tmpCA;struct _tuple0 _tmpC9=Cyc_NewControlFlow_anal_use_ints(
env,inflow,({struct Cyc_Absyn_Exp*_tmpC8[1];_tmpC8[0]=_tmp97;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpC8,sizeof(struct Cyc_Absyn_Exp*),1));}));
_LL138: _tmpCA=_tmpC9.f1;goto _LL137;_LL137: {void*_tmpCC;struct _tuple0 _tmpCB=Cyc_NewControlFlow_anal_Lexp(
env,_tmpCA,_tmp97);_LL13A: _tmpCC=_tmpCB.f2;goto _LL139;_LL139:{struct _tuple0
_tmpCE=({struct _tuple0 _tmpCD;_tmpCD.f1=_tmpCC;_tmpCD.f2=_tmpCA;_tmpCD;});void*
_tmpCF;struct Cyc_List_List*_tmpD0;struct Cyc_Dict_Dict*_tmpD1;void*_tmpD2;struct
Cyc_CfFlowInfo_Place*_tmpD3;_LL13C: _LL143: _tmpD2=_tmpCE.f1;if((unsigned int)
_tmpD2 > 1?*((int*)_tmpD2)== 0: 0){_LL144: _tmpD3=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmpD2)->f1;goto _LL140;}else{goto _LL13E;}_LL140: _tmpCF=_tmpCE.f2;if((
unsigned int)_tmpCF > 1?*((int*)_tmpCF)== 0: 0){_LL142: _tmpD1=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpCF)->f1;goto _LL141;_LL141: _tmpD0=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpCF)->f2;goto _LL13D;}else{goto _LL13E;}_LL13E: goto _LL13F;_LL13D: _tmpD0=Cyc_CfFlowInfo_reln_kill_exp(
_tmpD0,_tmp97);_tmpCA=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmpD4=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmpD4[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmpD5;_tmpD5.tag=0;_tmpD5.f1=Cyc_CfFlowInfo_assign_place(_tmp97->loc,_tmpD1,env->all_changed,
_tmpD3,Cyc_CfFlowInfo_unknown_all);_tmpD5.f2=_tmpD0;_tmpD5;});_tmpD4;});goto
_LL13B;_LL13F: goto _LL13B;_LL13B:;}return({struct _tuple0 _tmpD6;_tmpD6.f1=_tmpCA;
_tmpD6.f2=Cyc_CfFlowInfo_unknown_all;_tmpD6;});}}_LLC6: {void*_tmpD9;struct
_tuple0 _tmpD8=Cyc_NewControlFlow_anal_use_ints(env,inflow,({struct Cyc_Absyn_Exp*
_tmpD7[2];_tmpD7[1]=_tmp98;_tmpD7[0]=_tmp9B;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmpD7,sizeof(struct Cyc_Absyn_Exp*),2));}));
_LL146: _tmpD9=_tmpD8.f1;goto _LL145;_LL145: {void*_tmpDB;struct _tuple0 _tmpDA=Cyc_NewControlFlow_anal_Lexp(
env,_tmpD9,e);_LL148: _tmpDB=_tmpDA.f2;goto _LL147;_LL147:{struct _tuple0 _tmpDD=({
struct _tuple0 _tmpDC;_tmpDC.f1=_tmpDB;_tmpDC.f2=_tmpD9;_tmpDC;});void*_tmpDE;
struct Cyc_List_List*_tmpDF;struct Cyc_Dict_Dict*_tmpE0;void*_tmpE1;struct Cyc_CfFlowInfo_Place*
_tmpE2;_LL14A: _LL151: _tmpE1=_tmpDD.f1;if((unsigned int)_tmpE1 > 1?*((int*)_tmpE1)
== 0: 0){_LL152: _tmpE2=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmpE1)->f1;goto
_LL14E;}else{goto _LL14C;}_LL14E: _tmpDE=_tmpDD.f2;if((unsigned int)_tmpDE > 1?*((
int*)_tmpDE)== 0: 0){_LL150: _tmpE0=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpDE)->f1;goto _LL14F;_LL14F: _tmpDF=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpDE)->f2;goto _LL14B;}else{goto _LL14C;}_LL14C: goto _LL14D;_LL14B: _tmpDF=Cyc_CfFlowInfo_reln_kill_exp(
_tmpDF,_tmp9B);_tmpD9=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmpE3=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmpE3[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmpE4;_tmpE4.tag=0;_tmpE4.f1=Cyc_CfFlowInfo_assign_place(_tmp9B->loc,_tmpE0,env->all_changed,
_tmpE2,Cyc_CfFlowInfo_unknown_all);_tmpE4.f2=_tmpDF;_tmpE4;});_tmpE3;});goto
_LL149;_LL14D: goto _LL149;_LL149:;}return({struct _tuple0 _tmpE5;_tmpE5.f1=_tmpD9;
_tmpE5.f2=Cyc_CfFlowInfo_unknown_all;_tmpE5;});}}_LLC8: {struct Cyc_Set_Set**
_tmpE6=env->all_changed;while(1){env->all_changed=({struct Cyc_Set_Set**_tmpE7=
_cycalloc(sizeof(struct Cyc_Set_Set*));_tmpE7[0]=Cyc_CfFlowInfo_mt_place_set();
_tmpE7;});{void*_tmpE9;void*_tmpEA;struct _tuple0 _tmpE8=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmp9E);_LL155: _tmpEA=_tmpE8.f1;goto _LL154;_LL154: _tmpE9=_tmpE8.f2;
goto _LL153;_LL153: {struct Cyc_Set_Set*_tmpEB=*((struct Cyc_Set_Set**)_check_null(
env->all_changed));env->all_changed=({struct Cyc_Set_Set**_tmpEC=_cycalloc(
sizeof(struct Cyc_Set_Set*));_tmpEC[0]=Cyc_CfFlowInfo_mt_place_set();_tmpEC;});{
void*_tmpEE;void*_tmpEF;struct _tuple0 _tmpED=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp9C);_LL158: _tmpEF=_tmpED.f1;goto _LL157;_LL157: _tmpEE=_tmpED.f2;goto
_LL156;_LL156: {struct Cyc_Set_Set*_tmpF0=*((struct Cyc_Set_Set**)_check_null(env->all_changed));
void*_tmpF1=Cyc_CfFlowInfo_after_flow((struct Cyc_Set_Set**)& _tmpEB,_tmpEA,_tmpEF,
_tmpEB,_tmpF0);void*_tmpF2=Cyc_CfFlowInfo_join_flow(_tmpE6,inflow,_tmpF1);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmpF2,inflow)){if(_tmpE6 == 0){env->all_changed=0;}else{*((struct Cyc_Set_Set**)
_check_null(env->all_changed))=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,
struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*_tmpE6,((struct Cyc_Set_Set*(*)(struct
Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmpEB,_tmpF0));}{void*
_tmpF3=_tmpF1;struct Cyc_List_List*_tmpF4;struct Cyc_Dict_Dict*_tmpF5;_LL15A: if((
int)_tmpF3 == 0){goto _LL15B;}else{goto _LL15C;}_LL15C: if((unsigned int)_tmpF3 > 1?*((
int*)_tmpF3)== 0: 0){_LL15F: _tmpF5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpF3)->f1;goto _LL15E;_LL15E: _tmpF4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpF3)->f2;goto _LL15D;}else{goto _LL159;}_LL15B: return({struct _tuple0 _tmpF6;
_tmpF6.f1=(void*)0;_tmpF6.f2=_tmpEE;_tmpF6;});_LL15D: {void*_tmpF7=_tmpE9;struct
Cyc_CfFlowInfo_Place*_tmpF8;_LL161: if((unsigned int)_tmpF7 > 1?*((int*)_tmpF7)== 
0: 0){_LL165: _tmpF8=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmpF7)->f1;goto _LL162;}
else{goto _LL163;}_LL163: if((int)_tmpF7 == 0){goto _LL164;}else{goto _LL160;}_LL162:
_tmpF5=Cyc_CfFlowInfo_assign_place(e->loc,_tmpF5,env->all_changed,_tmpF8,_tmpEE);
_tmpF4=Cyc_CfFlowInfo_reln_assign_exp(_tmpF4,_tmp9E,_tmp9C);_tmpF1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmpF9=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));
_tmpF9[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmpFA;_tmpFA.tag=0;_tmpFA.f1=
_tmpF5;_tmpFA.f2=_tmpF4;_tmpFA;});_tmpF9;});Cyc_NewControlFlow_update_tryflow(
env,_tmpF1);return({struct _tuple0 _tmpFB;_tmpFB.f1=_tmpF1;_tmpFB.f2=_tmpEE;_tmpFB;});
_LL164: return({struct _tuple0 _tmpFC;_tmpFC.f1=Cyc_NewControlFlow_use_Rval(env,
_tmp9C->loc,_tmpF1,_tmpEE);_tmpFC.f2=_tmpEE;_tmpFC;});_LL160:;}_LL159:;}}inflow=
_tmpF2;}}}}}}_LLCA: return Cyc_NewControlFlow_anal_Rexp(env,(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpA0)).f1,_tmp9F);_LLCC: {void*_tmpFE;void*_tmpFF;struct _tuple0
_tmpFD=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpA1);_LL168: _tmpFF=_tmpFD.f1;
goto _LL167;_LL167: _tmpFE=_tmpFD.f2;goto _LL166;_LL166: Cyc_NewControlFlow_use_Rval(
env,_tmpA1->loc,_tmpFF,_tmpFE);return({struct _tuple0 _tmp100;_tmp100.f1=(void*)0;
_tmp100.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp100;});}_LLCE: _tmpA2=({
struct Cyc_List_List*_tmp101=_cycalloc(sizeof(struct Cyc_List_List));_tmp101->hd=
_tmpA3;_tmp101->tl=_tmpA2;_tmp101;});{struct Cyc_List_List*_tmp103;void*_tmp104;
struct _tuple4 _tmp102=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmpA2);
_LL16B: _tmp104=_tmp102.f1;goto _LL16A;_LL16A: _tmp103=_tmp102.f2;goto _LL169;_LL169:
for(0;_tmp103 != 0;(_tmp103=_tmp103->tl,_tmpA2=((struct Cyc_List_List*)_check_null(
_tmpA2))->tl)){_tmp104=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpA2))->hd)->loc,_tmp104,(void*)_tmp103->hd);}
if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA3->topt))->v)){
return({struct _tuple0 _tmp105;_tmp105.f1=(void*)0;_tmp105.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp105;});}else{return({struct _tuple0 _tmp106;_tmp106.f1=_tmp104;_tmp106.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp106;});}}_LLD0: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp117=_cycalloc(sizeof(struct Cyc_CfFlowInfo_MallocPt_struct));_tmp117[0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _tmp118;_tmp118.tag=1;_tmp118.f1=_tmpA6;
_tmp118;});_tmp117;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp116=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_tmp116->root=(void*)root;
_tmp116->fields=0;_tmp116;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp114=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_struct));_tmp114[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp115;_tmp115.tag=2;_tmp115.f1=place;
_tmp115;});_tmp114;});void*place_val=_tmpA5?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(*((
void**)_check_null(_tmpA7)),Cyc_CfFlowInfo_unknown_none);void*outflow;if(env->all_changed
!= 0){*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(env->all_changed)),place);}if(_tmpA8 != 0){struct Cyc_List_List*
_tmp109;void*_tmp10A;struct _tuple4 _tmp108=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp107[2];_tmp107[1]=_tmpA6;_tmp107[0]=(struct
Cyc_Absyn_Exp*)_check_null(_tmpA8);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp107,sizeof(struct Cyc_Absyn_Exp*),2));}));_LL16E:
_tmp10A=_tmp108.f1;goto _LL16D;_LL16D: _tmp109=_tmp108.f2;goto _LL16C;_LL16C:
outflow=_tmp10A;}else{void*_tmp10C;struct _tuple0 _tmp10B=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpA6);_LL170: _tmp10C=_tmp10B.f1;goto _LL16F;_LL16F: outflow=_tmp10C;}{
void*_tmp10D=outflow;struct Cyc_List_List*_tmp10E;struct Cyc_Dict_Dict*_tmp10F;
_LL172: if((int)_tmp10D == 0){goto _LL173;}else{goto _LL174;}_LL174: if((unsigned int)
_tmp10D > 1?*((int*)_tmp10D)== 0: 0){_LL177: _tmp10F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp10D)->f1;goto _LL176;_LL176: _tmp10E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp10D)->f2;goto _LL175;}else{goto _LL171;}_LL173: return({struct _tuple0 _tmp110;
_tmp110.f1=outflow;_tmp110.f2=rval;_tmp110;});_LL175: return({struct _tuple0
_tmp111;_tmp111.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp112=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp112[0]=({struct
Cyc_CfFlowInfo_ReachableFL_struct _tmp113;_tmp113.tag=0;_tmp113.f1=Cyc_Dict_insert(
_tmp10F,root,place_val);_tmp113.f2=_tmp10E;_tmp113;});_tmp112;});_tmp111.f2=rval;
_tmp111;});_LL171:;}}_LLD2: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp12A=_cycalloc(sizeof(struct Cyc_CfFlowInfo_MallocPt_struct));_tmp12A[0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _tmp12B;_tmp12B.tag=1;_tmp12B.f1=_tmpAA;
_tmp12B;});_tmp12A;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp129=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_tmp129->root=(void*)root;
_tmp129->fields=0;_tmp129;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp127=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_struct));_tmp127[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp128;_tmp128.tag=2;_tmp128.f1=place;
_tmp128;});_tmp127;});if(env->all_changed != 0){*((struct Cyc_Set_Set**)
_check_null(env->all_changed))=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,
struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(env->all_changed)),place);}{void*outflow;void*place_val;if(_tmpAB != 
0){struct Cyc_List_List*_tmp11B;void*_tmp11C;struct _tuple4 _tmp11A=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp119[2];_tmp119[1]=_tmpAA;_tmp119[0]=(struct
Cyc_Absyn_Exp*)_check_null(_tmpAB);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp119,sizeof(struct Cyc_Absyn_Exp*),2));}));_LL17A:
_tmp11C=_tmp11A.f1;goto _LL179;_LL179: _tmp11B=_tmp11A.f2;goto _LL178;_LL178:
outflow=_tmp11C;place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp11B))->tl))->hd;}else{void*_tmp11E;void*_tmp11F;struct _tuple0
_tmp11D=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpAA);_LL17D: _tmp11F=_tmp11D.f1;
goto _LL17C;_LL17C: _tmp11E=_tmp11D.f2;goto _LL17B;_LL17B: outflow=_tmp11F;place_val=
_tmp11E;}{void*_tmp120=outflow;struct Cyc_List_List*_tmp121;struct Cyc_Dict_Dict*
_tmp122;_LL17F: if((int)_tmp120 == 0){goto _LL180;}else{goto _LL181;}_LL181: if((
unsigned int)_tmp120 > 1?*((int*)_tmp120)== 0: 0){_LL184: _tmp122=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp120)->f1;goto _LL183;_LL183: _tmp121=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp120)->f2;goto _LL182;}else{goto _LL17E;}_LL180: return({struct _tuple0 _tmp123;
_tmp123.f1=outflow;_tmp123.f2=rval;_tmp123;});_LL182: return({struct _tuple0
_tmp124;_tmp124.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp125=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp125[0]=({struct
Cyc_CfFlowInfo_ReachableFL_struct _tmp126;_tmp126.tag=0;_tmp126.f1=Cyc_Dict_insert(
_tmp122,root,place_val);_tmp126.f2=_tmp121;_tmp126;});_tmp125;});_tmp124.f2=rval;
_tmp124;});_LL17E:;}}}_LLD4: {void*_tmp12D;void*_tmp12E;struct _tuple0 _tmp12C=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmpAC);_LL187: _tmp12E=_tmp12C.f1;goto _LL186;_LL186: _tmp12D=_tmp12C.f2;
goto _LL185;_LL185: {void*_tmp12F=_tmp12D;struct Cyc_CfFlowInfo_Place*_tmp130;
_LL189: if((int)_tmp12F == 0){goto _LL18A;}else{goto _LL18B;}_LL18B: if((unsigned int)
_tmp12F > 1?*((int*)_tmp12F)== 0: 0){_LL18D: _tmp130=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp12F)->f1;goto _LL18C;}else{goto _LL188;}_LL18A: return({struct _tuple0 _tmp131;
_tmp131.f1=_tmp12E;_tmp131.f2=(void*)1;_tmp131;});_LL18C: return({struct _tuple0
_tmp132;_tmp132.f1=_tmp12E;_tmp132.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp133=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_struct));_tmp133[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp134;_tmp134.tag=2;_tmp134.f1=_tmp130;
_tmp134;});_tmp133;});_tmp132;});_LL188:;}}_LLD6: {void*_tmp136;void*_tmp137;
struct _tuple0 _tmp135=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpAD);_LL190:
_tmp137=_tmp135.f1;goto _LL18F;_LL18F: _tmp136=_tmp135.f2;goto _LL18E;_LL18E: return
Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp137,_tmpAD,_tmp136);}_LLD8: {void*
_tmp139;void*_tmp13A;struct _tuple0 _tmp138=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpAF);_LL193: _tmp13A=_tmp138.f1;goto _LL192;_LL192: _tmp139=_tmp138.f2;
goto _LL191;_LL191:{void*_tmp13B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpAF->topt))->v);_LL195: if((unsigned int)_tmp13B > 3?*((int*)
_tmp13B)== 15: 0){goto _LL196;}else{goto _LL197;}_LL197: if((unsigned int)_tmp13B > 3?*((
int*)_tmp13B)== 11: 0){goto _LL198;}else{goto _LL199;}_LL199: goto _LL19A;_LL196: goto
_LL198;_LL198: return({struct _tuple0 _tmp13C;_tmp13C.f1=_tmp13A;_tmp13C.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp13C;});_LL19A: goto _LL194;_LL194:;}{void*_tmp13D=_tmp139;struct Cyc_Dict_Dict*
_tmp13E;_LL19C: if((unsigned int)_tmp13D > 3?*((int*)_tmp13D)== 3: 0){_LL1A0:
_tmp13E=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp13D)->f1;goto _LL19D;}else{
goto _LL19E;}_LL19E: goto _LL19F;_LL19D: return({struct _tuple0 _tmp13F;_tmp13F.f1=
_tmp13A;_tmp13F.f2=((void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp13E,_tmpAE);_tmp13F;});_LL19F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp140=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp140[0]=({struct
Cyc_Core_Impossible_struct _tmp141;_tmp141.tag=Cyc_Core_Impossible;_tmp141.f1=
_tag_arr("anal_Rexp: StructMember",sizeof(unsigned char),24);_tmp141;});_tmp140;}));
_LL19B:;}}_LLDA: {void*_tmp143;void*_tmp144;struct _tuple0 _tmp142=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpB1);_LL1A3: _tmp144=_tmp142.f1;goto _LL1A2;_LL1A2: _tmp143=_tmp142.f2;
goto _LL1A1;_LL1A1: {void*_tmp146;void*_tmp147;struct _tuple0 _tmp145=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp144,_tmpB1,_tmp143);_LL1A6: _tmp147=_tmp145.f1;goto _LL1A5;_LL1A5:
_tmp146=_tmp145.f2;goto _LL1A4;_LL1A4:{void*_tmp148=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpB1->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp149;void*_tmp14A;_LL1A8: if((unsigned int)_tmp148 > 3?*((int*)_tmp148)== 4: 0){
_LL1AC: _tmp149=((struct Cyc_Absyn_PointerType_struct*)_tmp148)->f1;_LL1AD: _tmp14A=(
void*)_tmp149.elt_typ;goto _LL1A9;}else{goto _LL1AA;}_LL1AA: goto _LL1AB;_LL1A9:{
void*_tmp14B=Cyc_Tcutil_compress(_tmp14A);_LL1AF: if((unsigned int)_tmp14B > 3?*((
int*)_tmp14B)== 15: 0){goto _LL1B0;}else{goto _LL1B1;}_LL1B1: if((unsigned int)
_tmp14B > 3?*((int*)_tmp14B)== 11: 0){goto _LL1B2;}else{goto _LL1B3;}_LL1B3: goto
_LL1B4;_LL1B0: goto _LL1B2;_LL1B2: return({struct _tuple0 _tmp14C;_tmp14C.f1=_tmp147;
_tmp14C.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp14C;});_LL1B4: goto
_LL1AE;_LL1AE:;}goto _LL1A7;_LL1AB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp14D=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp14D[0]=({struct
Cyc_Core_Impossible_struct _tmp14E;_tmp14E.tag=Cyc_Core_Impossible;_tmp14E.f1=
_tag_arr("anal_Rexp: StructArrow ptr",sizeof(unsigned char),27);_tmp14E;});
_tmp14D;}));_LL1A7:;}{void*_tmp14F=_tmp146;struct Cyc_Dict_Dict*_tmp150;_LL1B6:
if((unsigned int)_tmp14F > 3?*((int*)_tmp14F)== 3: 0){_LL1BA: _tmp150=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp14F)->f1;goto _LL1B7;}else{goto _LL1B8;}_LL1B8: goto _LL1B9;_LL1B7: return({
struct _tuple0 _tmp151;_tmp151.f1=_tmp147;_tmp151.f2=((void*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp150,_tmpB0);_tmp151;});_LL1B9:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp152=_cycalloc(sizeof(struct
Cyc_Core_Impossible_struct));_tmp152[0]=({struct Cyc_Core_Impossible_struct
_tmp153;_tmp153.tag=Cyc_Core_Impossible;_tmp153.f1=_tag_arr("anal_Rexp: StructArrow",
sizeof(unsigned char),23);_tmp153;});_tmp152;}));_LL1B5:;}}}_LLDC: {void*_tmp155;
void*_tmp156;struct _tuple0 _tmp154=Cyc_NewControlFlow_anal_test(env,inflow,_tmpB4);
_LL1BD: _tmp156=_tmp154.f1;goto _LL1BC;_LL1BC: _tmp155=_tmp154.f2;goto _LL1BB;_LL1BB: {
void*_tmp158;void*_tmp159;struct _tuple0 _tmp157=Cyc_NewControlFlow_anal_Rexp(env,
_tmp156,_tmpB3);_LL1C0: _tmp159=_tmp157.f1;goto _LL1BF;_LL1BF: _tmp158=_tmp157.f2;
goto _LL1BE;_LL1BE: {void*_tmp15B;void*_tmp15C;struct _tuple0 _tmp15A=Cyc_NewControlFlow_anal_Rexp(
env,_tmp155,_tmpB2);_LL1C3: _tmp15C=_tmp15A.f1;goto _LL1C2;_LL1C2: _tmp15B=_tmp15A.f2;
goto _LL1C1;_LL1C1: return({struct _tuple0 _tmp15D;_tmp15D.f1=Cyc_CfFlowInfo_join_flow(
env->all_changed,Cyc_NewControlFlow_use_Rval(env,_tmpB3->loc,_tmp159,_tmp158),
Cyc_NewControlFlow_use_Rval(env,_tmpB2->loc,_tmp15C,_tmp15B));_tmp15D.f2=Cyc_CfFlowInfo_unknown_all;
_tmp15D;});}}}_LLDE: {struct Cyc_List_List*_tmp160;void*_tmp161;struct _tuple4
_tmp15F=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({struct Cyc_Absyn_Exp*
_tmp15E[2];_tmp15E[1]=_tmpB5;_tmp15E[0]=_tmpB6;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp15E,sizeof(struct Cyc_Absyn_Exp*),2));}));
_LL1C6: _tmp161=_tmp15F.f1;goto _LL1C5;_LL1C5: _tmp160=_tmp15F.f2;goto _LL1C4;_LL1C4:{
void*_tmp162=_tmp161;struct Cyc_List_List*_tmp163;struct Cyc_Dict_Dict*_tmp164;
_LL1C8: if((unsigned int)_tmp162 > 1?*((int*)_tmp162)== 0: 0){_LL1CD: _tmp164=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp162)->f1;goto _LL1CC;_LL1CC: _tmp163=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp162)->f2;goto _LL1C9;}else{goto
_LL1CA;}_LL1CA: goto _LL1CB;_LL1C9: if(Cyc_CfFlowInfo_initlevel(_tmp164,(void*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp160))->tl))->hd)
== (void*)0){({void*_tmp165[0]={};Cyc_Tcutil_terr(_tmpB5->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp165,sizeof(void*),0));});}{struct Cyc_List_List*
_tmp166=Cyc_NewControlFlow_add_subscript_reln(_tmp163,_tmpB6,_tmpB5);if(_tmp163
!= _tmp166){_tmp161=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp167=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp167[0]=({struct
Cyc_CfFlowInfo_ReachableFL_struct _tmp168;_tmp168.tag=0;_tmp168.f1=_tmp164;
_tmp168.f2=_tmp166;_tmp168;});_tmp167;});}goto _LL1C7;}_LL1CB: goto _LL1C7;_LL1C7:;}{
void*_tmp169=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpB6->topt))->v);
_LL1CF: if((unsigned int)_tmp169 > 3?*((int*)_tmp169)== 9: 0){goto _LL1D0;}else{goto
_LL1D1;}_LL1D1: goto _LL1D2;_LL1D0: {void*_tmp16A=(void*)((struct Cyc_List_List*)
_check_null(_tmp160))->hd;struct Cyc_Dict_Dict*_tmp16B;_LL1D4: if((unsigned int)
_tmp16A > 3?*((int*)_tmp16A)== 3: 0){_LL1D8: _tmp16B=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp16A)->f1;goto _LL1D5;}else{goto _LL1D6;}_LL1D6: goto _LL1D7;_LL1D5: return({
struct _tuple0 _tmp16C;_tmp16C.f1=_tmp161;_tmp16C.f2=((void*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp16B,Cyc_Absyn_fieldname((int)Cyc_Evexp_eval_const_uint_exp(
_tmpB5)));_tmp16C;});_LL1D7:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp16D=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp16D[0]=({struct
Cyc_Core_Impossible_struct _tmp16E;_tmp16E.tag=Cyc_Core_Impossible;_tmp16E.f1=
_tag_arr("anal_Rexp: Subscript",sizeof(unsigned char),21);_tmp16E;});_tmp16D;}));
_LL1D3:;}_LL1D2: goto _LL1CE;_LL1CE:;}return Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp161,_tmpB6,(void*)((struct Cyc_List_List*)_check_null(_tmp160))->hd);}
_LLE0: _tmpB8=_tmpB7;goto _LLE2;_LLE2: {struct Cyc_List_List*_tmp170;void*_tmp171;
struct _tuple4 _tmp16F=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmpB8);
_LL1DB: _tmp171=_tmp16F.f1;goto _LL1DA;_LL1DA: _tmp170=_tmp16F.f2;goto _LL1D9;_LL1D9: {
struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);{int i=0;
for(0;_tmp170 != 0;(_tmp170=_tmp170->tl,++ i)){aggrdict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,Cyc_Absyn_fieldname(
i),(void*)_tmp170->hd);}}return({struct _tuple0 _tmp172;_tmp172.f1=_tmp171;_tmp172.f2=(
void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp173=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));
_tmp173[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp174;_tmp174.tag=3;_tmp174.f1=
aggrdict;_tmp174;});_tmp173;});_tmp172;});}}_LLE4: _tmpBA=_tmpB9;goto _LLE6;_LLE6: {
struct Cyc_List_List*_tmp176;void*_tmp177;struct _tuple4 _tmp175=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple5*))
Cyc_Core_snd,_tmpBA));_LL1DE: _tmp177=_tmp175.f1;goto _LL1DD;_LL1DD: _tmp176=
_tmp175.f2;goto _LL1DC;_LL1DC: {struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);{
int i=0;for(0;_tmp176 != 0;(((_tmp176=_tmp176->tl,_tmpBA=_tmpBA->tl)),++ i)){struct
Cyc_List_List*ds=(*((struct _tuple5*)((struct Cyc_List_List*)_check_null(_tmpBA))->hd)).f1;
for(0;ds != 0;ds=ds->tl){void*_tmp178=(void*)ds->hd;struct _tagged_arr*_tmp179;
_LL1E0: if(*((int*)_tmp178)== 0){goto _LL1E1;}else{goto _LL1E2;}_LL1E2: if(*((int*)
_tmp178)== 1){_LL1E4: _tmp179=((struct Cyc_Absyn_FieldName_struct*)_tmp178)->f1;
goto _LL1E3;}else{goto _LL1DF;}_LL1E1:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp17A=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp17A[0]=({struct
Cyc_Core_Impossible_struct _tmp17B;_tmp17B.tag=Cyc_Core_Impossible;_tmp17B.f1=
_tag_arr("anal_Rexp:Struct_e",sizeof(unsigned char),19);_tmp17B;});_tmp17A;}));
_LL1E3: aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,_tmp179,(void*)_tmp176->hd);
_LL1DF:;}}}return({struct _tuple0 _tmp17C;_tmp17C.f1=_tmp177;_tmp17C.f2=(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp17D=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));
_tmp17D[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp17E;_tmp17E.tag=3;_tmp17E.f1=
aggrdict;_tmp17E;});_tmp17D;});_tmp17C;});}}_LLE8: {struct Cyc_List_List*_tmp17F=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple5*),struct Cyc_List_List*
x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple5*))Cyc_Core_snd,_tmpBB);
struct Cyc_List_List*_tmp181;void*_tmp182;struct _tuple4 _tmp180=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp17F);_LL1E7: _tmp182=_tmp180.f1;goto _LL1E6;_LL1E6: _tmp181=_tmp180.f2;
goto _LL1E5;_LL1E5: for(0;_tmp181 != 0;(_tmp181=_tmp181->tl,_tmp17F=_tmp17F->tl)){
_tmp182=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp17F))->hd)->loc,_tmp182,(void*)_tmp181->hd);}return({struct
_tuple0 _tmp183;_tmp183.f1=_tmp182;_tmp183.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp183;});}
_LLEA: {void*_tmp185;void*_tmp186;struct _tuple0 _tmp184=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpBD);_LL1EA: _tmp186=_tmp184.f1;goto _LL1E9;_LL1E9: _tmp185=_tmp184.f2;
goto _LL1E8;_LL1E8: {void*_tmp187=_tmp186;struct Cyc_List_List*_tmp188;struct Cyc_Dict_Dict*
_tmp189;_LL1EC: if((int)_tmp187 == 0){goto _LL1ED;}else{goto _LL1EE;}_LL1EE: if((
unsigned int)_tmp187 > 1?*((int*)_tmp187)== 0: 0){_LL1F1: _tmp189=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp187)->f1;goto _LL1F0;_LL1F0: _tmp188=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp187)->f2;goto _LL1EF;}else{goto _LL1EB;}_LL1ED: return({struct _tuple0 _tmp18A;
_tmp18A.f1=_tmp186;_tmp18A.f2=Cyc_CfFlowInfo_unknown_all;_tmp18A;});_LL1EF: if(
Cyc_CfFlowInfo_initlevel(_tmp189,_tmp185)== (void*)0){({void*_tmp18B[0]={};Cyc_Tcutil_terr(
_tmpBD->loc,_tag_arr("expression may not be initialized",sizeof(unsigned char),
34),_tag_arr(_tmp18B,sizeof(void*),0));});}{struct Cyc_List_List*new_relns=
_tmp188;comp_loop: {void*_tmp18C=(void*)_tmpBD->r;struct Cyc_Absyn_Exp*_tmp18D;
void*_tmp18E;struct Cyc_Absyn_Vardecl*_tmp18F;void*_tmp190;struct Cyc_Absyn_Vardecl*
_tmp191;void*_tmp192;struct Cyc_Absyn_Vardecl*_tmp193;void*_tmp194;struct Cyc_Absyn_Vardecl*
_tmp195;void*_tmp196;int _tmp197;struct Cyc_List_List*_tmp198;struct Cyc_List_List
_tmp199;struct Cyc_Absyn_Exp*_tmp19A;void*_tmp19B;_LL1F3: if(*((int*)_tmp18C)== 13){
_LL203: _tmp18D=((struct Cyc_Absyn_Cast_e_struct*)_tmp18C)->f2;goto _LL1F4;}else{
goto _LL1F5;}_LL1F5: if(*((int*)_tmp18C)== 1){_LL205: _tmp18E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp18C)->f2;if((unsigned int)_tmp18E > 1?*((int*)_tmp18E)== 0: 0){_LL206: _tmp18F=((
struct Cyc_Absyn_Global_b_struct*)_tmp18E)->f1;goto _LL204;}else{goto _LL1F7;}}
else{goto _LL1F7;}_LL204: if(! _tmp18F->escapes){goto _LL1F6;}else{goto _LL1F7;}
_LL1F7: if(*((int*)_tmp18C)== 1){_LL208: _tmp190=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp18C)->f2;if((unsigned int)_tmp190 > 1?*((int*)_tmp190)== 3: 0){_LL209: _tmp191=((
struct Cyc_Absyn_Local_b_struct*)_tmp190)->f1;goto _LL207;}else{goto _LL1F9;}}else{
goto _LL1F9;}_LL207: if(! _tmp191->escapes){goto _LL1F8;}else{goto _LL1F9;}_LL1F9: if(*((
int*)_tmp18C)== 1){_LL20B: _tmp192=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp18C)->f2;
if((unsigned int)_tmp192 > 1?*((int*)_tmp192)== 4: 0){_LL20C: _tmp193=((struct Cyc_Absyn_Pat_b_struct*)
_tmp192)->f1;goto _LL20A;}else{goto _LL1FB;}}else{goto _LL1FB;}_LL20A: if(! _tmp193->escapes){
goto _LL1FA;}else{goto _LL1FB;}_LL1FB: if(*((int*)_tmp18C)== 1){_LL20E: _tmp194=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp18C)->f2;if((unsigned int)_tmp194 > 1?*((
int*)_tmp194)== 2: 0){_LL20F: _tmp195=((struct Cyc_Absyn_Param_b_struct*)_tmp194)->f1;
goto _LL20D;}else{goto _LL1FD;}}else{goto _LL1FD;}_LL20D: if(! _tmp195->escapes){goto
_LL1FC;}else{goto _LL1FD;}_LL1FD: if(*((int*)_tmp18C)== 0){_LL210: _tmp196=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp18C)->f1;if((unsigned int)_tmp196 > 1?*((int*)
_tmp196)== 2: 0){_LL211: _tmp197=((struct Cyc_Absyn_Int_c_struct*)_tmp196)->f2;goto
_LL1FE;}else{goto _LL1FF;}}else{goto _LL1FF;}_LL1FF: if(*((int*)_tmp18C)== 3){
_LL214: _tmp19B=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp18C)->f1;if((int)
_tmp19B == 19){goto _LL212;}else{goto _LL201;}_LL212: _tmp198=((struct Cyc_Absyn_Primop_e_struct*)
_tmp18C)->f2;if(_tmp198 == 0){goto _LL201;}else{_tmp199=*_tmp198;_LL213: _tmp19A=(
struct Cyc_Absyn_Exp*)_tmp199.hd;goto _LL200;}}else{goto _LL201;}_LL201: goto _LL202;
_LL1F4: _tmpBD=_tmp18D;goto comp_loop;_LL1F6: _tmp191=_tmp18F;goto _LL1F8;_LL1F8:
_tmp193=_tmp191;goto _LL1FA;_LL1FA: _tmp195=_tmp193;goto _LL1FC;_LL1FC: new_relns=({
struct Cyc_List_List*_tmp19C=_cycalloc(sizeof(struct Cyc_List_List));_tmp19C->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp19D=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));
_tmp19D->vd=_tmpBE;_tmp19D->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp19E=_cycalloc(sizeof(struct Cyc_CfFlowInfo_LessVar_struct));_tmp19E[0]=({
struct Cyc_CfFlowInfo_LessVar_struct _tmp19F;_tmp19F.tag=1;_tmp19F.f1=_tmp195;
_tmp19F;});_tmp19E;}));_tmp19D;});_tmp19C->tl=_tmp188;_tmp19C;});goto _LL1F2;
_LL1FE: new_relns=({struct Cyc_List_List*_tmp1A0=_cycalloc(sizeof(struct Cyc_List_List));
_tmp1A0->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1A1=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));
_tmp1A1->vd=_tmpBE;_tmp1A1->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp1A2=_cycalloc_atomic(sizeof(struct Cyc_CfFlowInfo_LessConst_struct));_tmp1A2[
0]=({struct Cyc_CfFlowInfo_LessConst_struct _tmp1A3;_tmp1A3.tag=3;_tmp1A3.f1=(
unsigned int)_tmp197;_tmp1A3;});_tmp1A2;}));_tmp1A1;});_tmp1A0->tl=_tmp188;
_tmp1A0;});goto _LL1F2;_LL200:{void*_tmp1A4=(void*)_tmp19A->r;void*_tmp1A5;struct
Cyc_Absyn_Vardecl*_tmp1A6;void*_tmp1A7;struct Cyc_Absyn_Vardecl*_tmp1A8;void*
_tmp1A9;struct Cyc_Absyn_Vardecl*_tmp1AA;void*_tmp1AB;struct Cyc_Absyn_Vardecl*
_tmp1AC;_LL216: if(*((int*)_tmp1A4)== 1){_LL221: _tmp1A5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A4)->f2;if((unsigned int)_tmp1A5 > 1?*((int*)_tmp1A5)== 0: 0){_LL222: _tmp1A6=((
struct Cyc_Absyn_Global_b_struct*)_tmp1A5)->f1;goto _LL220;}else{goto _LL218;}}
else{goto _LL218;}_LL220: if(! _tmp1A6->escapes){goto _LL217;}else{goto _LL218;}
_LL218: if(*((int*)_tmp1A4)== 1){_LL224: _tmp1A7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A4)->f2;if((unsigned int)_tmp1A7 > 1?*((int*)_tmp1A7)== 3: 0){_LL225: _tmp1A8=((
struct Cyc_Absyn_Local_b_struct*)_tmp1A7)->f1;goto _LL223;}else{goto _LL21A;}}else{
goto _LL21A;}_LL223: if(! _tmp1A8->escapes){goto _LL219;}else{goto _LL21A;}_LL21A: if(*((
int*)_tmp1A4)== 1){_LL227: _tmp1A9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1A4)->f2;
if((unsigned int)_tmp1A9 > 1?*((int*)_tmp1A9)== 4: 0){_LL228: _tmp1AA=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1A9)->f1;goto _LL226;}else{goto _LL21C;}}else{goto _LL21C;}_LL226: if(! _tmp1AA->escapes){
goto _LL21B;}else{goto _LL21C;}_LL21C: if(*((int*)_tmp1A4)== 1){_LL22A: _tmp1AB=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1A4)->f2;if((unsigned int)_tmp1AB > 1?*((
int*)_tmp1AB)== 2: 0){_LL22B: _tmp1AC=((struct Cyc_Absyn_Param_b_struct*)_tmp1AB)->f1;
goto _LL229;}else{goto _LL21E;}}else{goto _LL21E;}_LL229: if(! _tmp1AC->escapes){goto
_LL21D;}else{goto _LL21E;}_LL21E: goto _LL21F;_LL217: _tmp1A8=_tmp1A6;goto _LL219;
_LL219: _tmp1AA=_tmp1A8;goto _LL21B;_LL21B: _tmp1AC=_tmp1AA;goto _LL21D;_LL21D:
new_relns=({struct Cyc_List_List*_tmp1AD=_cycalloc(sizeof(struct Cyc_List_List));
_tmp1AD->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1AE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));
_tmp1AE->vd=_tmpBE;_tmp1AE->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp1AF=_cycalloc(sizeof(struct Cyc_CfFlowInfo_LessSize_struct));_tmp1AF[0]=({
struct Cyc_CfFlowInfo_LessSize_struct _tmp1B0;_tmp1B0.tag=2;_tmp1B0.f1=_tmp1AC;
_tmp1B0;});_tmp1AF;}));_tmp1AE;});_tmp1AD->tl=_tmp188;_tmp1AD;});goto _LL215;
_LL21F: goto _LL215;_LL215:;}goto _LL1F2;_LL202: goto _LL1F2;_LL1F2:;}if(_tmp188 != 
new_relns){_tmp186=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp1B1=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp1B1[0]=({struct
Cyc_CfFlowInfo_ReachableFL_struct _tmp1B2;_tmp1B2.tag=0;_tmp1B2.f1=_tmp189;
_tmp1B2.f2=new_relns;_tmp1B2;});_tmp1B1;});}{void*_tmp1B3=_tmp185;_LL22D: if((int)
_tmp1B3 == 0){goto _LL22E;}else{goto _LL22F;}_LL22F: if((int)_tmp1B3 == 2){goto _LL230;}
else{goto _LL231;}_LL231: if((int)_tmp1B3 == 1){goto _LL232;}else{goto _LL233;}_LL233:
if((unsigned int)_tmp1B3 > 3?*((int*)_tmp1B3)== 2: 0){goto _LL234;}else{goto _LL235;}
_LL235: goto _LL236;_LL22E: return({struct _tuple0 _tmp1B4;_tmp1B4.f1=_tmp186;_tmp1B4.f2=
Cyc_CfFlowInfo_unknown_all;_tmp1B4;});_LL230: goto _LL232;_LL232: goto _LL234;_LL234: {
struct Cyc_List_List _tmp1B5=({struct Cyc_List_List _tmp1BE;_tmp1BE.hd=_tmpBE;
_tmp1BE.tl=0;_tmp1BE;});_tmp186=Cyc_NewControlFlow_add_vars(_tmp186,(struct Cyc_List_List*)&
_tmp1B5,Cyc_CfFlowInfo_unknown_all);{void*_tmp1B7;void*_tmp1B8;struct _tuple0
_tmp1B6=Cyc_NewControlFlow_anal_Rexp(env,_tmp186,_tmpBC);_LL239: _tmp1B8=_tmp1B6.f1;
goto _LL238;_LL238: _tmp1B7=_tmp1B6.f2;goto _LL237;_LL237:{void*_tmp1B9=_tmp1B8;
struct Cyc_Dict_Dict*_tmp1BA;_LL23B: if((int)_tmp1B9 == 0){goto _LL23C;}else{goto
_LL23D;}_LL23D: if((unsigned int)_tmp1B9 > 1?*((int*)_tmp1B9)== 0: 0){_LL23F:
_tmp1BA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1B9)->f1;goto _LL23E;}
else{goto _LL23A;}_LL23C: return({struct _tuple0 _tmp1BB;_tmp1BB.f1=_tmp1B8;_tmp1BB.f2=
Cyc_CfFlowInfo_unknown_all;_tmp1BB;});_LL23E: if(Cyc_CfFlowInfo_initlevel(_tmp1BA,
_tmp1B7)!= (void*)2){({void*_tmp1BC[0]={};Cyc_Tcutil_terr(_tmpBD->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp1BC,sizeof(void*),0));});return({struct
_tuple0 _tmp1BD;_tmp1BD.f1=(void*)0;_tmp1BD.f2=Cyc_CfFlowInfo_unknown_all;_tmp1BD;});}
_LL23A:;}_tmp186=_tmp1B8;goto _LL236;}}_LL236: while(1){struct Cyc_List_List _tmp1BF=({
struct Cyc_List_List _tmp1C8;_tmp1C8.hd=_tmpBE;_tmp1C8.tl=0;_tmp1C8;});_tmp186=Cyc_NewControlFlow_add_vars(
_tmp186,(struct Cyc_List_List*)& _tmp1BF,Cyc_CfFlowInfo_unknown_all);{void*_tmp1C1;
void*_tmp1C2;struct _tuple0 _tmp1C0=Cyc_NewControlFlow_anal_Rexp(env,_tmp186,
_tmpBC);_LL242: _tmp1C2=_tmp1C0.f1;goto _LL241;_LL241: _tmp1C1=_tmp1C0.f2;goto
_LL240;_LL240:{void*_tmp1C3=_tmp1C2;struct Cyc_Dict_Dict*_tmp1C4;_LL244: if((int)
_tmp1C3 == 0){goto _LL245;}else{goto _LL246;}_LL246: if((unsigned int)_tmp1C3 > 1?*((
int*)_tmp1C3)== 0: 0){_LL248: _tmp1C4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1C3)->f1;goto _LL247;}else{goto _LL243;}_LL245: goto _LL243;_LL247: if(Cyc_CfFlowInfo_initlevel(
_tmp1C4,_tmp1C1)!= (void*)2){({void*_tmp1C5[0]={};Cyc_Tcutil_terr(_tmpBD->loc,
_tag_arr("expression may not be initialized",sizeof(unsigned char),34),_tag_arr(
_tmp1C5,sizeof(void*),0));});return({struct _tuple0 _tmp1C6;_tmp1C6.f1=(void*)0;
_tmp1C6.f2=Cyc_CfFlowInfo_unknown_all;_tmp1C6;});}_LL243:;}{void*_tmp1C7=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp186,_tmp1C2);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1C7,
_tmp186)){break;}_tmp186=_tmp1C7;}}}return({struct _tuple0 _tmp1C9;_tmp1C9.f1=
_tmp186;_tmp1C9.f2=Cyc_CfFlowInfo_unknown_all;_tmp1C9;});_LL22C:;}}_LL1EB:;}}
_LLEC: while(1){inflow=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmpBF))->flow;{void*_tmp1CA=(void*)_tmpBF->r;struct Cyc_Absyn_Stmt*_tmp1CB;
struct Cyc_Absyn_Stmt*_tmp1CC;struct Cyc_Absyn_Stmt*_tmp1CD;struct Cyc_Absyn_Decl*
_tmp1CE;struct Cyc_Absyn_Exp*_tmp1CF;_LL24A: if((unsigned int)_tmp1CA > 1?*((int*)
_tmp1CA)== 1: 0){_LL253: _tmp1CC=((struct Cyc_Absyn_Seq_s_struct*)_tmp1CA)->f1;goto
_LL252;_LL252: _tmp1CB=((struct Cyc_Absyn_Seq_s_struct*)_tmp1CA)->f2;goto _LL24B;}
else{goto _LL24C;}_LL24C: if((unsigned int)_tmp1CA > 1?*((int*)_tmp1CA)== 12: 0){
_LL255: _tmp1CE=((struct Cyc_Absyn_Decl_s_struct*)_tmp1CA)->f1;goto _LL254;_LL254:
_tmp1CD=((struct Cyc_Absyn_Decl_s_struct*)_tmp1CA)->f2;goto _LL24D;}else{goto
_LL24E;}_LL24E: if((unsigned int)_tmp1CA > 1?*((int*)_tmp1CA)== 0: 0){_LL256:
_tmp1CF=((struct Cyc_Absyn_Exp_s_struct*)_tmp1CA)->f1;goto _LL24F;}else{goto _LL250;}
_LL250: goto _LL251;_LL24B: inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp1CC);
_tmpBF=_tmp1CB;goto _LL249;_LL24D: inflow=Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp1CE);_tmpBF=_tmp1CD;goto _LL249;_LL24F: return Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp1CF);_LL251:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1D0=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp1D0[0]=({struct
Cyc_Core_Impossible_struct _tmp1D1;_tmp1D1.tag=Cyc_Core_Impossible;_tmp1D1.f1=
_tag_arr("analyze_Rexp: ill-formed StmtExp",sizeof(unsigned char),33);_tmp1D1;});
_tmp1D0;}));_LL249:;}}_LLEE: goto _LLF0;_LLF0: goto _LLF2;_LLF2: goto _LLF4;_LLF4: goto
_LLF6;_LLF6: goto _LLF8;_LLF8: goto _LLFA;_LLFA:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1D2=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp1D2[0]=({struct
Cyc_Core_Impossible_struct _tmp1D3;_tmp1D3.tag=Cyc_Core_Impossible;_tmp1D3.f1=
_tag_arr("anal_Rexp, unexpected exp form",sizeof(unsigned char),31);_tmp1D3;});
_tmp1D2;}));_LL9A:;}}static struct _tuple0 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){
void*_tmp1D4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1D5;struct Cyc_Absyn_Conref*_tmp1D6;void*_tmp1D7;
_LL258: if((unsigned int)_tmp1D4 > 3?*((int*)_tmp1D4)== 4: 0){_LL25C: _tmp1D5=((
struct Cyc_Absyn_PointerType_struct*)_tmp1D4)->f1;_LL25E: _tmp1D7=(void*)_tmp1D5.elt_typ;
goto _LL25D;_LL25D: _tmp1D6=_tmp1D5.bounds;goto _LL259;}else{goto _LL25A;}_LL25A:
goto _LL25B;_LL259: {void*_tmp1D8=f;struct Cyc_List_List*_tmp1D9;struct Cyc_Dict_Dict*
_tmp1DA;_LL260: if((int)_tmp1D8 == 0){goto _LL261;}else{goto _LL262;}_LL262: if((
unsigned int)_tmp1D8 > 1?*((int*)_tmp1D8)== 0: 0){_LL265: _tmp1DA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1D8)->f1;goto _LL264;_LL264: _tmp1D9=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1D8)->f2;goto _LL263;}else{goto _LL25F;}_LL261: return({struct _tuple0 _tmp1DB;
_tmp1DB.f1=f;_tmp1DB.f2=(void*)0;_tmp1DB;});_LL263: if(Cyc_Tcutil_is_bound_one(
_tmp1D6)){void*_tmp1DC=r;struct Cyc_CfFlowInfo_Place*_tmp1DD;struct Cyc_CfFlowInfo_Place
_tmp1DE;struct Cyc_List_List*_tmp1DF;void*_tmp1E0;void*_tmp1E1;_LL267: if((int)
_tmp1DC == 1){goto _LL268;}else{goto _LL269;}_LL269: if((int)_tmp1DC == 2){goto _LL26A;}
else{goto _LL26B;}_LL26B: if((unsigned int)_tmp1DC > 3?*((int*)_tmp1DC)== 2: 0){
_LL273: _tmp1DD=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1DC)->f1;_tmp1DE=*
_tmp1DD;_LL275: _tmp1E0=(void*)_tmp1DE.root;goto _LL274;_LL274: _tmp1DF=_tmp1DE.fields;
goto _LL26C;}else{goto _LL26D;}_LL26D: if((int)_tmp1DC == 0){goto _LL26E;}else{goto
_LL26F;}_LL26F: if((unsigned int)_tmp1DC > 3?*((int*)_tmp1DC)== 0: 0){_LL276:
_tmp1E1=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp1DC)->f1;goto _LL270;}
else{goto _LL271;}_LL271: goto _LL272;_LL268: goto _LL26A;_LL26A:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp1E2=_cycalloc(sizeof(
struct Cyc_CfFlowInfo_NotZero_struct));_tmp1E2[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp1E3;_tmp1E3.tag=Cyc_CfFlowInfo_NotZero;_tmp1E3.f1=_tmp1D9;_tmp1E3;});_tmp1E2;})));
goto _LL266;_LL26C:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp1E4=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NotZero_struct));_tmp1E4[0]=({
struct Cyc_CfFlowInfo_NotZero_struct _tmp1E5;_tmp1E5.tag=Cyc_CfFlowInfo_NotZero;
_tmp1E5.f1=_tmp1D9;_tmp1E5;});_tmp1E4;})));return({struct _tuple0 _tmp1E6;_tmp1E6.f1=
f;_tmp1E6.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*_tmp1E7=_cycalloc(
sizeof(struct Cyc_CfFlowInfo_PlaceL_struct));_tmp1E7[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp1E8;_tmp1E8.tag=0;_tmp1E8.f1=({struct Cyc_CfFlowInfo_Place*_tmp1E9=_cycalloc(
sizeof(struct Cyc_CfFlowInfo_Place));_tmp1E9->root=(void*)_tmp1E0;_tmp1E9->fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1DF,flds);_tmp1E9;});_tmp1E8;});_tmp1E7;});_tmp1E6;});_LL26E:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_IsZero_struct*_tmp1EA=_cycalloc(sizeof(
struct Cyc_CfFlowInfo_IsZero_struct));_tmp1EA[0]=({struct Cyc_CfFlowInfo_IsZero_struct
_tmp1EB;_tmp1EB.tag=Cyc_CfFlowInfo_IsZero;_tmp1EB.f1=_tmp1D9;_tmp1EB;});_tmp1EA;})));
return({struct _tuple0 _tmp1EC;_tmp1EC.f1=(void*)0;_tmp1EC.f2=(void*)0;_tmp1EC;});
_LL270: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp1E1);goto _LL272;_LL272:(
void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp1ED=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownZ_struct));_tmp1ED[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp1EE;_tmp1EE.tag=Cyc_CfFlowInfo_UnknownZ;_tmp1EE.f1=_tmp1D9;_tmp1EE;});
_tmp1ED;})));_LL266:;}else{(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp1EF=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownZ_struct));_tmp1EF[0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _tmp1F0;_tmp1F0.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp1F0.f1=_tmp1D9;_tmp1F0;});_tmp1EF;})));}if(Cyc_CfFlowInfo_initlevel(_tmp1DA,
r)== (void*)0){({void*_tmp1F1[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of possibly uninitialized pointer",
sizeof(unsigned char),46),_tag_arr(_tmp1F1,sizeof(void*),0));});}return({struct
_tuple0 _tmp1F2;_tmp1F2.f1=f;_tmp1F2.f2=(void*)0;_tmp1F2;});_LL25F:;}_LL25B:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1F3=_cycalloc(sizeof(struct
Cyc_Core_Impossible_struct));_tmp1F3[0]=({struct Cyc_Core_Impossible_struct
_tmp1F4;_tmp1F4.tag=Cyc_Core_Impossible;_tmp1F4.f1=_tag_arr("left deref of non-pointer-type",
sizeof(unsigned char),31);_tmp1F4;});_tmp1F3;}));_LL257:;}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict*d;{void*_tmp1F5=inflow;struct Cyc_List_List*_tmp1F6;
struct Cyc_Dict_Dict*_tmp1F7;_LL278: if((int)_tmp1F5 == 0){goto _LL279;}else{goto
_LL27A;}_LL27A: if((unsigned int)_tmp1F5 > 1?*((int*)_tmp1F5)== 0: 0){_LL27D:
_tmp1F7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1F5)->f1;goto _LL27C;
_LL27C: _tmp1F6=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1F5)->f2;goto
_LL27B;}else{goto _LL277;}_LL279: return({struct _tuple0 _tmp1F8;_tmp1F8.f1=(void*)0;
_tmp1F8.f2=(void*)0;_tmp1F8;});_LL27B: d=_tmp1F7;_LL277:;}{void*_tmp1F9=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp1FA;struct Cyc_Absyn_Exp*_tmp1FB;struct Cyc_Absyn_Exp*
_tmp1FC;void*_tmp1FD;struct Cyc_Absyn_Vardecl*_tmp1FE;void*_tmp1FF;struct Cyc_Absyn_Vardecl*
_tmp200;void*_tmp201;struct Cyc_Absyn_Vardecl*_tmp202;void*_tmp203;struct Cyc_Absyn_Vardecl*
_tmp204;struct _tagged_arr*_tmp205;struct Cyc_Absyn_Exp*_tmp206;struct Cyc_Absyn_Exp*
_tmp207;struct Cyc_Absyn_Exp*_tmp208;struct Cyc_Absyn_Exp*_tmp209;struct
_tagged_arr*_tmp20A;struct Cyc_Absyn_Exp*_tmp20B;_LL27F: if(*((int*)_tmp1F9)== 13){
_LL297: _tmp1FA=((struct Cyc_Absyn_Cast_e_struct*)_tmp1F9)->f2;goto _LL280;}else{
goto _LL281;}_LL281: if(*((int*)_tmp1F9)== 11){_LL298: _tmp1FB=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp1F9)->f1;goto _LL282;}else{goto _LL283;}_LL283: if(*((int*)_tmp1F9)== 12){
_LL299: _tmp1FC=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1F9)->f1;goto _LL284;}
else{goto _LL285;}_LL285: if(*((int*)_tmp1F9)== 1){_LL29A: _tmp1FD=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp1F9)->f2;if((unsigned int)_tmp1FD > 1?*((int*)_tmp1FD)
== 2: 0){_LL29B: _tmp1FE=((struct Cyc_Absyn_Param_b_struct*)_tmp1FD)->f1;goto _LL286;}
else{goto _LL287;}}else{goto _LL287;}_LL287: if(*((int*)_tmp1F9)== 1){_LL29C:
_tmp1FF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F9)->f2;if((unsigned int)
_tmp1FF > 1?*((int*)_tmp1FF)== 3: 0){_LL29D: _tmp200=((struct Cyc_Absyn_Local_b_struct*)
_tmp1FF)->f1;goto _LL288;}else{goto _LL289;}}else{goto _LL289;}_LL289: if(*((int*)
_tmp1F9)== 1){_LL29E: _tmp201=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F9)->f2;
if((unsigned int)_tmp201 > 1?*((int*)_tmp201)== 4: 0){_LL29F: _tmp202=((struct Cyc_Absyn_Pat_b_struct*)
_tmp201)->f1;goto _LL28A;}else{goto _LL28B;}}else{goto _LL28B;}_LL28B: if(*((int*)
_tmp1F9)== 1){_LL2A0: _tmp203=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F9)->f2;
if((unsigned int)_tmp203 > 1?*((int*)_tmp203)== 0: 0){_LL2A1: _tmp204=((struct Cyc_Absyn_Global_b_struct*)
_tmp203)->f1;goto _LL28C;}else{goto _LL28D;}}else{goto _LL28D;}_LL28D: if(*((int*)
_tmp1F9)== 22){_LL2A3: _tmp206=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp1F9)->f1;
goto _LL2A2;_LL2A2: _tmp205=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp1F9)->f2;
goto _LL28E;}else{goto _LL28F;}_LL28F: if(*((int*)_tmp1F9)== 20){_LL2A4: _tmp207=((
struct Cyc_Absyn_Deref_e_struct*)_tmp1F9)->f1;goto _LL290;}else{goto _LL291;}_LL291:
if(*((int*)_tmp1F9)== 23){_LL2A6: _tmp209=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1F9)->f1;goto _LL2A5;_LL2A5: _tmp208=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1F9)->f2;goto _LL292;}else{goto _LL293;}_LL293: if(*((int*)_tmp1F9)== 21){
_LL2A8: _tmp20B=((struct Cyc_Absyn_StructMember_e_struct*)_tmp1F9)->f1;goto _LL2A7;
_LL2A7: _tmp20A=((struct Cyc_Absyn_StructMember_e_struct*)_tmp1F9)->f2;goto _LL294;}
else{goto _LL295;}_LL295: goto _LL296;_LL280: _tmp1FB=_tmp1FA;goto _LL282;_LL282:
_tmp1FC=_tmp1FB;goto _LL284;_LL284: return Cyc_NewControlFlow_anal_Lexp_rec(env,
inflow,_tmp1FC,flds);_LL286: _tmp200=_tmp1FE;goto _LL288;_LL288: _tmp202=_tmp200;
goto _LL28A;_LL28A: return({struct _tuple0 _tmp20C;_tmp20C.f1=inflow;_tmp20C.f2=(
void*)({struct Cyc_CfFlowInfo_PlaceL_struct*_tmp20D=_cycalloc(sizeof(struct Cyc_CfFlowInfo_PlaceL_struct));
_tmp20D[0]=({struct Cyc_CfFlowInfo_PlaceL_struct _tmp20E;_tmp20E.tag=0;_tmp20E.f1=({
struct Cyc_CfFlowInfo_Place*_tmp20F=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));
_tmp20F->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp210=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_struct));_tmp210[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp211;_tmp211.tag=0;_tmp211.f1=_tmp202;_tmp211;});_tmp210;}));_tmp20F->fields=
flds;_tmp20F;});_tmp20E;});_tmp20D;});_tmp20C;});_LL28C: return({struct _tuple0
_tmp212;_tmp212.f1=inflow;_tmp212.f2=(void*)0;_tmp212;});_LL28E:{void*_tmp213=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp206->topt))->v);
_LL2AA: if((unsigned int)_tmp213 > 3?*((int*)_tmp213)== 15: 0){goto _LL2AB;}else{
goto _LL2AC;}_LL2AC: if((unsigned int)_tmp213 > 3?*((int*)_tmp213)== 11: 0){goto
_LL2AD;}else{goto _LL2AE;}_LL2AE: goto _LL2AF;_LL2AB: goto _LL2AD;_LL2AD: goto _LL2A9;
_LL2AF: flds=({struct Cyc_List_List*_tmp214=_cycalloc(sizeof(struct Cyc_List_List));
_tmp214->hd=_tmp205;_tmp214->tl=flds;_tmp214;});_LL2A9:;}_tmp207=_tmp206;goto
_LL290;_LL290: {void*_tmp216;void*_tmp217;struct _tuple0 _tmp215=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp207);_LL2B2: _tmp217=_tmp215.f1;goto _LL2B1;_LL2B1: _tmp216=_tmp215.f2;
goto _LL2B0;_LL2B0: return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp217,
_tmp207,_tmp216,flds);}_LL292:{void*_tmp218=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp209->topt))->v);_LL2B4: if((unsigned int)_tmp218 > 3?*((
int*)_tmp218)== 9: 0){goto _LL2B5;}else{goto _LL2B6;}_LL2B6: goto _LL2B7;_LL2B5: {
struct _tagged_arr*_tmp219=Cyc_Absyn_fieldname((int)Cyc_Evexp_eval_const_uint_exp(
_tmp208));return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp209,({struct Cyc_List_List*
_tmp21A=_cycalloc(sizeof(struct Cyc_List_List));_tmp21A->hd=_tmp219;_tmp21A->tl=
flds;_tmp21A;}));}_LL2B7: goto _LL2B3;_LL2B3:;}{struct Cyc_List_List*_tmp21D;void*
_tmp21E;struct _tuple4 _tmp21C=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({
struct Cyc_Absyn_Exp*_tmp21B[2];_tmp21B[1]=_tmp208;_tmp21B[0]=_tmp209;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp21B,sizeof(struct Cyc_Absyn_Exp*),
2));}));_LL2BA: _tmp21E=_tmp21C.f1;goto _LL2B9;_LL2B9: _tmp21D=_tmp21C.f2;goto
_LL2B8;_LL2B8:{void*_tmp21F=_tmp21E;struct Cyc_List_List*_tmp220;struct Cyc_Dict_Dict*
_tmp221;_LL2BC: if((unsigned int)_tmp21F > 1?*((int*)_tmp21F)== 0: 0){_LL2C1:
_tmp221=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp21F)->f1;goto _LL2C0;
_LL2C0: _tmp220=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp21F)->f2;goto
_LL2BD;}else{goto _LL2BE;}_LL2BE: goto _LL2BF;_LL2BD: if(Cyc_CfFlowInfo_initlevel(
_tmp221,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp21D))->tl))->hd)== (void*)0){({void*_tmp222[0]={};Cyc_Tcutil_terr(
_tmp208->loc,_tag_arr("expression may not be initialized",sizeof(unsigned char),
34),_tag_arr(_tmp222,sizeof(void*),0));});}{struct Cyc_List_List*_tmp223=Cyc_NewControlFlow_add_subscript_reln(
_tmp220,_tmp209,_tmp208);if(_tmp220 != _tmp223){_tmp21E=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp224=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp224[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp225;_tmp225.tag=0;_tmp225.f1=_tmp221;
_tmp225.f2=_tmp223;_tmp225;});_tmp224;});}goto _LL2BB;}_LL2BF: goto _LL2BB;_LL2BB:;}
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp21E,_tmp209,(void*)((struct
Cyc_List_List*)_check_null(_tmp21D))->hd,flds);}_LL294:{void*_tmp226=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp20B->topt))->v);_LL2C3: if((
unsigned int)_tmp226 > 3?*((int*)_tmp226)== 15: 0){goto _LL2C4;}else{goto _LL2C5;}
_LL2C5: if((unsigned int)_tmp226 > 3?*((int*)_tmp226)== 11: 0){goto _LL2C6;}else{
goto _LL2C7;}_LL2C7: goto _LL2C8;_LL2C4: goto _LL2C6;_LL2C6: return({struct _tuple0
_tmp227;_tmp227.f1=inflow;_tmp227.f2=(void*)0;_tmp227;});_LL2C8: flds=({struct Cyc_List_List*
_tmp228=_cycalloc(sizeof(struct Cyc_List_List));_tmp228->hd=_tmp20A;_tmp228->tl=
flds;_tmp228;});_LL2C2:;}return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,
_tmp20B,flds);_LL296: return({struct _tuple0 _tmp229;_tmp229.f1=(void*)0;_tmp229.f2=(
void*)0;_tmp229;});_LL27E:;}}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e){return
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,e,0);}static struct _tuple0 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e){{void*
_tmp22A=(void*)e->r;struct Cyc_Absyn_Exp*_tmp22B;struct Cyc_Absyn_Exp*_tmp22C;
struct Cyc_Absyn_Exp*_tmp22D;struct Cyc_Absyn_Exp*_tmp22E;struct Cyc_Absyn_Exp*
_tmp22F;struct Cyc_List_List*_tmp230;struct Cyc_List_List _tmp231;struct Cyc_List_List*
_tmp232;struct Cyc_Absyn_Exp*_tmp233;void*_tmp234;struct Cyc_List_List*_tmp235;
void*_tmp236;struct Cyc_List_List*_tmp237;void*_tmp238;struct Cyc_List_List*
_tmp239;void*_tmp23A;struct Cyc_List_List*_tmp23B;void*_tmp23C;_LL2CA: if(*((int*)
_tmp22A)== 6){_LL2DC: _tmp22D=((struct Cyc_Absyn_Conditional_e_struct*)_tmp22A)->f1;
goto _LL2DB;_LL2DB: _tmp22C=((struct Cyc_Absyn_Conditional_e_struct*)_tmp22A)->f2;
goto _LL2DA;_LL2DA: _tmp22B=((struct Cyc_Absyn_Conditional_e_struct*)_tmp22A)->f3;
goto _LL2CB;}else{goto _LL2CC;}_LL2CC: if(*((int*)_tmp22A)== 7){_LL2DE: _tmp22F=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp22A)->f1;goto _LL2DD;_LL2DD: _tmp22E=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp22A)->f2;goto _LL2CD;}else{goto _LL2CE;}_LL2CE: if(*((
int*)_tmp22A)== 3){_LL2E2: _tmp234=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp22A)->f1;if((int)_tmp234 == 11){goto _LL2DF;}else{goto _LL2D0;}_LL2DF: _tmp230=((
struct Cyc_Absyn_Primop_e_struct*)_tmp22A)->f2;if(_tmp230 == 0){goto _LL2D0;}else{
_tmp231=*_tmp230;_LL2E1: _tmp233=(struct Cyc_Absyn_Exp*)_tmp231.hd;goto _LL2E0;
_LL2E0: _tmp232=_tmp231.tl;if(_tmp232 == 0){goto _LL2CF;}else{goto _LL2D0;}}}else{
goto _LL2D0;}_LL2D0: if(*((int*)_tmp22A)== 3){_LL2E4: _tmp236=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp22A)->f1;if((int)_tmp236 == 5){goto _LL2E3;}else{goto _LL2D2;}_LL2E3: _tmp235=((
struct Cyc_Absyn_Primop_e_struct*)_tmp22A)->f2;goto _LL2D1;}else{goto _LL2D2;}
_LL2D2: if(*((int*)_tmp22A)== 3){_LL2E6: _tmp238=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp22A)->f1;if((int)_tmp238 == 6){goto _LL2E5;}else{goto _LL2D4;}_LL2E5: _tmp237=((
struct Cyc_Absyn_Primop_e_struct*)_tmp22A)->f2;goto _LL2D3;}else{goto _LL2D4;}
_LL2D4: if(*((int*)_tmp22A)== 3){_LL2E8: _tmp23A=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp22A)->f1;if((int)_tmp23A == 8){goto _LL2E7;}else{goto _LL2D6;}_LL2E7: _tmp239=((
struct Cyc_Absyn_Primop_e_struct*)_tmp22A)->f2;goto _LL2D5;}else{goto _LL2D6;}
_LL2D6: if(*((int*)_tmp22A)== 3){_LL2EA: _tmp23C=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp22A)->f1;if((int)_tmp23C == 10){goto _LL2E9;}else{goto _LL2D8;}_LL2E9: _tmp23B=((
struct Cyc_Absyn_Primop_e_struct*)_tmp22A)->f2;goto _LL2D7;}else{goto _LL2D8;}
_LL2D8: goto _LL2D9;_LL2CB: {void*_tmp23E;void*_tmp23F;struct _tuple0 _tmp23D=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp22D);_LL2ED: _tmp23F=_tmp23D.f1;goto _LL2EC;_LL2EC: _tmp23E=_tmp23D.f2;
goto _LL2EB;_LL2EB: {void*_tmp241;void*_tmp242;struct _tuple0 _tmp240=Cyc_NewControlFlow_anal_test(
env,_tmp23F,_tmp22C);_LL2F0: _tmp242=_tmp240.f1;goto _LL2EF;_LL2EF: _tmp241=_tmp240.f2;
goto _LL2EE;_LL2EE: {void*_tmp244;void*_tmp245;struct _tuple0 _tmp243=Cyc_NewControlFlow_anal_test(
env,_tmp23E,_tmp22B);_LL2F3: _tmp245=_tmp243.f1;goto _LL2F2;_LL2F2: _tmp244=_tmp243.f2;
goto _LL2F1;_LL2F1: return({struct _tuple0 _tmp246;_tmp246.f1=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp242,_tmp245);_tmp246.f2=Cyc_CfFlowInfo_join_flow(env->all_changed,
_tmp241,_tmp244);_tmp246;});}}}_LL2CD: return Cyc_NewControlFlow_anal_test(env,(
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp22F)).f1,_tmp22E);_LL2CF: {void*
_tmp248;void*_tmp249;struct _tuple0 _tmp247=Cyc_NewControlFlow_anal_test(env,
inflow,_tmp233);_LL2F6: _tmp249=_tmp247.f1;goto _LL2F5;_LL2F5: _tmp248=_tmp247.f2;
goto _LL2F4;_LL2F4: return({struct _tuple0 _tmp24A;_tmp24A.f1=_tmp248;_tmp24A.f2=
_tmp249;_tmp24A;});}_LL2D1: {struct Cyc_List_List*_tmp24C;void*_tmp24D;struct
_tuple4 _tmp24B=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp235);
_LL2F9: _tmp24D=_tmp24B.f1;goto _LL2F8;_LL2F8: _tmp24C=_tmp24B.f2;goto _LL2F7;_LL2F7: {
void*_tmp24E=_tmp24D;struct Cyc_List_List*_tmp24F;struct Cyc_Dict_Dict*_tmp250;
_LL2FB: if((int)_tmp24E == 0){goto _LL2FC;}else{goto _LL2FD;}_LL2FD: if((unsigned int)
_tmp24E > 1?*((int*)_tmp24E)== 0: 0){_LL300: _tmp250=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp24E)->f1;goto _LL2FF;_LL2FF: _tmp24F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp24E)->f2;goto _LL2FE;}else{goto _LL2FA;}_LL2FC: return({struct _tuple0 _tmp251;
_tmp251.f1=_tmp24D;_tmp251.f2=_tmp24D;_tmp251;});_LL2FE: {void*_tmp252=(void*)((
struct Cyc_List_List*)_check_null(_tmp24C))->hd;void*_tmp253=(void*)((struct Cyc_List_List*)
_check_null(_tmp24C->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp250,_tmp252)== (
void*)0){({void*_tmp254[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp235))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp254,sizeof(void*),0));});}if(Cyc_CfFlowInfo_initlevel(
_tmp250,_tmp253)== (void*)0){({void*_tmp255[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp235))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(unsigned char),34),_tag_arr(
_tmp255,sizeof(void*),0));});}{struct _tuple0 _tmp257=({struct _tuple0 _tmp256;
_tmp256.f1=_tmp252;_tmp256.f2=_tmp253;_tmp256;});void*_tmp258;void*_tmp259;void*
_tmp25A;void*_tmp25B;void*_tmp25C;void*_tmp25D;_LL302: _LL309: _tmp259=_tmp257.f1;
if((unsigned int)_tmp259 > 3?*((int*)_tmp259)== 0: 0){_LL30A: _tmp25A=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp259)->f1;goto _LL308;}else{goto _LL304;}
_LL308: _tmp258=_tmp257.f2;if((int)_tmp258 == 0){goto _LL303;}else{goto _LL304;}
_LL304: _LL30D: _tmp25D=_tmp257.f1;if((int)_tmp25D == 0){goto _LL30B;}else{goto
_LL306;}_LL30B: _tmp25B=_tmp257.f2;if((unsigned int)_tmp25B > 3?*((int*)_tmp25B)== 
0: 0){_LL30C: _tmp25C=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp25B)->f1;
goto _LL305;}else{goto _LL306;}_LL306: goto _LL307;_LL303: {void*_tmp25F;void*
_tmp260;struct _tuple0 _tmp25E=Cyc_NewControlFlow_splitzero(env,_tmp24D,_tmp24D,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp235))->hd,_tmp25A);
_LL310: _tmp260=_tmp25E.f1;goto _LL30F;_LL30F: _tmp25F=_tmp25E.f2;goto _LL30E;_LL30E:
return({struct _tuple0 _tmp261;_tmp261.f1=_tmp25F;_tmp261.f2=_tmp260;_tmp261;});}
_LL305: {void*_tmp263;void*_tmp264;struct _tuple0 _tmp262=Cyc_NewControlFlow_splitzero(
env,_tmp24D,_tmp24D,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((
struct Cyc_List_List*)_check_null(_tmp235))->tl))->hd,_tmp25C);_LL313: _tmp264=
_tmp262.f1;goto _LL312;_LL312: _tmp263=_tmp262.f2;goto _LL311;_LL311: return({struct
_tuple0 _tmp265;_tmp265.f1=_tmp263;_tmp265.f2=_tmp264;_tmp265;});}_LL307: return({
struct _tuple0 _tmp266;_tmp266.f1=_tmp24D;_tmp266.f2=_tmp24D;_tmp266;});_LL301:;}}
_LL2FA:;}}_LL2D3: {struct Cyc_List_List*_tmp268;void*_tmp269;struct _tuple4 _tmp267=
Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp237);_LL316: _tmp269=
_tmp267.f1;goto _LL315;_LL315: _tmp268=_tmp267.f2;goto _LL314;_LL314: {void*_tmp26A=
_tmp269;struct Cyc_List_List*_tmp26B;struct Cyc_Dict_Dict*_tmp26C;_LL318: if((int)
_tmp26A == 0){goto _LL319;}else{goto _LL31A;}_LL31A: if((unsigned int)_tmp26A > 1?*((
int*)_tmp26A)== 0: 0){_LL31D: _tmp26C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp26A)->f1;goto _LL31C;_LL31C: _tmp26B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp26A)->f2;goto _LL31B;}else{goto _LL317;}_LL319: return({struct _tuple0 _tmp26D;
_tmp26D.f1=_tmp269;_tmp26D.f2=_tmp269;_tmp26D;});_LL31B: {void*_tmp26E=(void*)((
struct Cyc_List_List*)_check_null(_tmp268))->hd;void*_tmp26F=(void*)((struct Cyc_List_List*)
_check_null(_tmp268->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp26C,_tmp26E)== (
void*)0){({void*_tmp270[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp237))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp270,sizeof(void*),0));});}if(Cyc_CfFlowInfo_initlevel(
_tmp26C,_tmp26F)== (void*)0){({void*_tmp271[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp237))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(unsigned char),34),_tag_arr(
_tmp271,sizeof(void*),0));});}{struct _tuple0 _tmp273=({struct _tuple0 _tmp272;
_tmp272.f1=_tmp26E;_tmp272.f2=_tmp26F;_tmp272;});void*_tmp274;void*_tmp275;void*
_tmp276;void*_tmp277;void*_tmp278;void*_tmp279;_LL31F: _LL326: _tmp275=_tmp273.f1;
if((unsigned int)_tmp275 > 3?*((int*)_tmp275)== 0: 0){_LL327: _tmp276=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp275)->f1;goto _LL325;}else{goto _LL321;}
_LL325: _tmp274=_tmp273.f2;if((int)_tmp274 == 0){goto _LL320;}else{goto _LL321;}
_LL321: _LL32A: _tmp279=_tmp273.f1;if((int)_tmp279 == 0){goto _LL328;}else{goto
_LL323;}_LL328: _tmp277=_tmp273.f2;if((unsigned int)_tmp277 > 3?*((int*)_tmp277)== 
0: 0){_LL329: _tmp278=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp277)->f1;
goto _LL322;}else{goto _LL323;}_LL323: goto _LL324;_LL320: return Cyc_NewControlFlow_splitzero(
env,_tmp269,_tmp269,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp237))->hd,_tmp276);_LL322: return Cyc_NewControlFlow_splitzero(env,_tmp269,
_tmp269,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp237))->tl))->hd,_tmp278);_LL324: return({struct _tuple0 _tmp27A;
_tmp27A.f1=_tmp269;_tmp27A.f2=_tmp269;_tmp27A;});_LL31E:;}}_LL317:;}}_LL2D5: {
struct Cyc_List_List*_tmp27C;void*_tmp27D;struct _tuple4 _tmp27B=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp239);_LL32D: _tmp27D=_tmp27B.f1;goto _LL32C;_LL32C: _tmp27C=_tmp27B.f2;
goto _LL32B;_LL32B: {void*_tmp27E=_tmp27D;struct Cyc_List_List*_tmp27F;struct Cyc_Dict_Dict*
_tmp280;_LL32F: if((int)_tmp27E == 0){goto _LL330;}else{goto _LL331;}_LL331: if((
unsigned int)_tmp27E > 1?*((int*)_tmp27E)== 0: 0){_LL334: _tmp280=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp27E)->f1;goto _LL333;_LL333: _tmp27F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp27E)->f2;goto _LL332;}else{goto _LL32E;}_LL330: return({struct _tuple0 _tmp281;
_tmp281.f1=_tmp27D;_tmp281.f2=_tmp27D;_tmp281;});_LL332: {void*_tmp282=(void*)((
struct Cyc_List_List*)_check_null(_tmp27C))->hd;void*_tmp283=(void*)((struct Cyc_List_List*)
_check_null(_tmp27C->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp280,_tmp282)== (
void*)0){({void*_tmp284[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp239))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp284,sizeof(void*),0));});}if(Cyc_CfFlowInfo_initlevel(
_tmp280,_tmp283)== (void*)0){({void*_tmp285[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp239))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(unsigned char),34),_tag_arr(
_tmp285,sizeof(void*),0));});}{struct Cyc_Absyn_Exp*_tmp286=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp239))->hd;struct Cyc_Absyn_Exp*_tmp287=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp239->tl))->hd;void*
_tmp288=(void*)_tmp286->r;void*_tmp289;struct Cyc_Absyn_Vardecl*_tmp28A;void*
_tmp28B;struct Cyc_Absyn_Vardecl*_tmp28C;void*_tmp28D;struct Cyc_Absyn_Vardecl*
_tmp28E;void*_tmp28F;struct Cyc_Absyn_Vardecl*_tmp290;_LL336: if(*((int*)_tmp288)
== 1){_LL341: _tmp289=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp288)->f2;if((
unsigned int)_tmp289 > 1?*((int*)_tmp289)== 0: 0){_LL342: _tmp28A=((struct Cyc_Absyn_Global_b_struct*)
_tmp289)->f1;goto _LL340;}else{goto _LL338;}}else{goto _LL338;}_LL340: if(! _tmp28A->escapes){
goto _LL337;}else{goto _LL338;}_LL338: if(*((int*)_tmp288)== 1){_LL344: _tmp28B=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp288)->f2;if((unsigned int)_tmp28B > 1?*((
int*)_tmp28B)== 3: 0){_LL345: _tmp28C=((struct Cyc_Absyn_Local_b_struct*)_tmp28B)->f1;
goto _LL343;}else{goto _LL33A;}}else{goto _LL33A;}_LL343: if(! _tmp28C->escapes){goto
_LL339;}else{goto _LL33A;}_LL33A: if(*((int*)_tmp288)== 1){_LL347: _tmp28D=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp288)->f2;if((unsigned int)_tmp28D > 1?*((int*)
_tmp28D)== 4: 0){_LL348: _tmp28E=((struct Cyc_Absyn_Pat_b_struct*)_tmp28D)->f1;goto
_LL346;}else{goto _LL33C;}}else{goto _LL33C;}_LL346: if(! _tmp28E->escapes){goto
_LL33B;}else{goto _LL33C;}_LL33C: if(*((int*)_tmp288)== 1){_LL34A: _tmp28F=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp288)->f2;if((unsigned int)_tmp28F > 1?*((int*)
_tmp28F)== 2: 0){_LL34B: _tmp290=((struct Cyc_Absyn_Param_b_struct*)_tmp28F)->f1;
goto _LL349;}else{goto _LL33E;}}else{goto _LL33E;}_LL349: if(! _tmp290->escapes){goto
_LL33D;}else{goto _LL33E;}_LL33E: goto _LL33F;_LL337: _tmp28C=_tmp28A;goto _LL339;
_LL339: _tmp28E=_tmp28C;goto _LL33B;_LL33B: _tmp290=_tmp28E;goto _LL33D;_LL33D: {
void*_tmp291=(void*)_tmp287->r;void*_tmp292;struct Cyc_Absyn_Vardecl*_tmp293;void*
_tmp294;struct Cyc_Absyn_Vardecl*_tmp295;void*_tmp296;struct Cyc_Absyn_Vardecl*
_tmp297;void*_tmp298;struct Cyc_Absyn_Vardecl*_tmp299;void*_tmp29A;int _tmp29B;
struct Cyc_List_List*_tmp29C;struct Cyc_List_List _tmp29D;struct Cyc_Absyn_Exp*
_tmp29E;void*_tmp29F;_LL34D: if(*((int*)_tmp291)== 1){_LL35C: _tmp292=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp291)->f2;if((unsigned int)_tmp292 > 1?*((int*)
_tmp292)== 0: 0){_LL35D: _tmp293=((struct Cyc_Absyn_Global_b_struct*)_tmp292)->f1;
goto _LL35B;}else{goto _LL34F;}}else{goto _LL34F;}_LL35B: if(! _tmp293->escapes){goto
_LL34E;}else{goto _LL34F;}_LL34F: if(*((int*)_tmp291)== 1){_LL35F: _tmp294=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp291)->f2;if((unsigned int)_tmp294 > 1?*((int*)
_tmp294)== 3: 0){_LL360: _tmp295=((struct Cyc_Absyn_Local_b_struct*)_tmp294)->f1;
goto _LL35E;}else{goto _LL351;}}else{goto _LL351;}_LL35E: if(! _tmp295->escapes){goto
_LL350;}else{goto _LL351;}_LL351: if(*((int*)_tmp291)== 1){_LL362: _tmp296=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp291)->f2;if((unsigned int)_tmp296 > 1?*((int*)
_tmp296)== 4: 0){_LL363: _tmp297=((struct Cyc_Absyn_Pat_b_struct*)_tmp296)->f1;goto
_LL361;}else{goto _LL353;}}else{goto _LL353;}_LL361: if(! _tmp297->escapes){goto
_LL352;}else{goto _LL353;}_LL353: if(*((int*)_tmp291)== 1){_LL365: _tmp298=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp291)->f2;if((unsigned int)_tmp298 > 1?*((int*)
_tmp298)== 2: 0){_LL366: _tmp299=((struct Cyc_Absyn_Param_b_struct*)_tmp298)->f1;
goto _LL364;}else{goto _LL355;}}else{goto _LL355;}_LL364: if(! _tmp299->escapes){goto
_LL354;}else{goto _LL355;}_LL355: if(*((int*)_tmp291)== 0){_LL367: _tmp29A=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp291)->f1;if((unsigned int)_tmp29A > 1?*((int*)
_tmp29A)== 2: 0){_LL368: _tmp29B=((struct Cyc_Absyn_Int_c_struct*)_tmp29A)->f2;goto
_LL356;}else{goto _LL357;}}else{goto _LL357;}_LL357: if(*((int*)_tmp291)== 3){
_LL36B: _tmp29F=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp291)->f1;if((int)
_tmp29F == 19){goto _LL369;}else{goto _LL359;}_LL369: _tmp29C=((struct Cyc_Absyn_Primop_e_struct*)
_tmp291)->f2;if(_tmp29C == 0){goto _LL359;}else{_tmp29D=*_tmp29C;_LL36A: _tmp29E=(
struct Cyc_Absyn_Exp*)_tmp29D.hd;goto _LL358;}}else{goto _LL359;}_LL359: goto _LL35A;
_LL34E: _tmp295=_tmp293;goto _LL350;_LL350: _tmp297=_tmp295;goto _LL352;_LL352:
_tmp299=_tmp297;goto _LL354;_LL354:{struct _tuple0 _tmp2A1=({struct _tuple0 _tmp2A0;
_tmp2A0.f1=Cyc_Tcutil_compress((void*)_tmp290->type);_tmp2A0.f2=Cyc_Tcutil_compress((
void*)_tmp299->type);_tmp2A0;});void*_tmp2A2;void*_tmp2A3;void*_tmp2A4;void*
_tmp2A5;void*_tmp2A6;void*_tmp2A7;_LL36D: _LL373: _tmp2A2=_tmp2A1.f1;if((
unsigned int)_tmp2A2 > 3?*((int*)_tmp2A2)== 5: 0){_LL375: _tmp2A4=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp2A2)->f1;if((int)_tmp2A4 == 1){goto _LL374;}else{
goto _LL36F;}_LL374: _tmp2A3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2A2)->f2;
if((int)_tmp2A3 == 2){goto _LL36E;}else{goto _LL36F;}}else{goto _LL36F;}_LL36F:
_LL376: _tmp2A5=_tmp2A1.f2;if((unsigned int)_tmp2A5 > 3?*((int*)_tmp2A5)== 5: 0){
_LL378: _tmp2A7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2A5)->f1;if((int)
_tmp2A7 == 1){goto _LL377;}else{goto _LL371;}_LL377: _tmp2A6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2A5)->f2;if((int)_tmp2A6 == 2){goto _LL370;}else{goto _LL371;}}else{goto _LL371;}
_LL371: goto _LL372;_LL36E: goto _LL36C;_LL370: goto _LL36C;_LL372: return({struct
_tuple0 _tmp2A8;_tmp2A8.f1=_tmp27D;_tmp2A8.f2=_tmp27D;_tmp2A8;});_LL36C:;}{struct
Cyc_List_List*_tmp2A9=({struct Cyc_List_List*_tmp2AD=_cycalloc(sizeof(struct Cyc_List_List));
_tmp2AD->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2AE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));
_tmp2AE->vd=_tmp290;_tmp2AE->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp2AF=_cycalloc(sizeof(struct Cyc_CfFlowInfo_LessVar_struct));_tmp2AF[0]=({
struct Cyc_CfFlowInfo_LessVar_struct _tmp2B0;_tmp2B0.tag=1;_tmp2B0.f1=_tmp299;
_tmp2B0;});_tmp2AF;}));_tmp2AE;});_tmp2AD->tl=_tmp27F;_tmp2AD;});return({struct
_tuple0 _tmp2AA;_tmp2AA.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp2AB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp2AB[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2AC;_tmp2AC.tag=0;_tmp2AC.f1=_tmp280;
_tmp2AC.f2=_tmp2A9;_tmp2AC;});_tmp2AB;});_tmp2AA.f2=_tmp27D;_tmp2AA;});}_LL356:{
struct _tuple0 _tmp2B2=({struct _tuple0 _tmp2B1;_tmp2B1.f1=Cyc_Tcutil_compress((void*)
_tmp290->type);_tmp2B1.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp287->topt))->v);_tmp2B1;});void*_tmp2B3;void*_tmp2B4;void*
_tmp2B5;void*_tmp2B6;void*_tmp2B7;void*_tmp2B8;_LL37A: _LL380: _tmp2B3=_tmp2B2.f1;
if((unsigned int)_tmp2B3 > 3?*((int*)_tmp2B3)== 5: 0){_LL382: _tmp2B5=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp2B3)->f1;if((int)_tmp2B5 == 1){goto _LL381;}
else{goto _LL37C;}_LL381: _tmp2B4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2B3)->f2;
if((int)_tmp2B4 == 2){goto _LL37B;}else{goto _LL37C;}}else{goto _LL37C;}_LL37C:
_LL383: _tmp2B6=_tmp2B2.f2;if((unsigned int)_tmp2B6 > 3?*((int*)_tmp2B6)== 5: 0){
_LL385: _tmp2B8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2B6)->f1;if((int)
_tmp2B8 == 1){goto _LL384;}else{goto _LL37E;}_LL384: _tmp2B7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2B6)->f2;if((int)_tmp2B7 == 2){goto _LL37D;}else{goto _LL37E;}}else{goto _LL37E;}
_LL37E: goto _LL37F;_LL37B: goto _LL379;_LL37D: goto _LL379;_LL37F: return({struct
_tuple0 _tmp2B9;_tmp2B9.f1=_tmp27D;_tmp2B9.f2=_tmp27D;_tmp2B9;});_LL379:;}{struct
Cyc_List_List*_tmp2BA=({struct Cyc_List_List*_tmp2BE=_cycalloc(sizeof(struct Cyc_List_List));
_tmp2BE->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2BF=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));
_tmp2BF->vd=_tmp290;_tmp2BF->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp2C0=_cycalloc_atomic(sizeof(struct Cyc_CfFlowInfo_LessConst_struct));_tmp2C0[
0]=({struct Cyc_CfFlowInfo_LessConst_struct _tmp2C1;_tmp2C1.tag=3;_tmp2C1.f1=(
unsigned int)_tmp29B;_tmp2C1;});_tmp2C0;}));_tmp2BF;});_tmp2BE->tl=_tmp27F;
_tmp2BE;});return({struct _tuple0 _tmp2BB;_tmp2BB.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp2BC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp2BC[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2BD;_tmp2BD.tag=0;_tmp2BD.f1=_tmp280;
_tmp2BD.f2=_tmp2BA;_tmp2BD;});_tmp2BC;});_tmp2BB.f2=_tmp27D;_tmp2BB;});}_LL358: {
void*_tmp2C2=(void*)_tmp29E->r;void*_tmp2C3;struct Cyc_Absyn_Vardecl*_tmp2C4;void*
_tmp2C5;struct Cyc_Absyn_Vardecl*_tmp2C6;void*_tmp2C7;struct Cyc_Absyn_Vardecl*
_tmp2C8;void*_tmp2C9;struct Cyc_Absyn_Vardecl*_tmp2CA;_LL387: if(*((int*)_tmp2C2)
== 1){_LL392: _tmp2C3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2C2)->f2;if((
unsigned int)_tmp2C3 > 1?*((int*)_tmp2C3)== 0: 0){_LL393: _tmp2C4=((struct Cyc_Absyn_Global_b_struct*)
_tmp2C3)->f1;goto _LL391;}else{goto _LL389;}}else{goto _LL389;}_LL391: if(! _tmp2C4->escapes){
goto _LL388;}else{goto _LL389;}_LL389: if(*((int*)_tmp2C2)== 1){_LL395: _tmp2C5=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2C2)->f2;if((unsigned int)_tmp2C5 > 1?*((
int*)_tmp2C5)== 3: 0){_LL396: _tmp2C6=((struct Cyc_Absyn_Local_b_struct*)_tmp2C5)->f1;
goto _LL394;}else{goto _LL38B;}}else{goto _LL38B;}_LL394: if(! _tmp2C6->escapes){goto
_LL38A;}else{goto _LL38B;}_LL38B: if(*((int*)_tmp2C2)== 1){_LL398: _tmp2C7=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2C2)->f2;if((unsigned int)_tmp2C7 > 1?*((int*)
_tmp2C7)== 4: 0){_LL399: _tmp2C8=((struct Cyc_Absyn_Pat_b_struct*)_tmp2C7)->f1;goto
_LL397;}else{goto _LL38D;}}else{goto _LL38D;}_LL397: if(! _tmp2C8->escapes){goto
_LL38C;}else{goto _LL38D;}_LL38D: if(*((int*)_tmp2C2)== 1){_LL39B: _tmp2C9=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2C2)->f2;if((unsigned int)_tmp2C9 > 1?*((int*)
_tmp2C9)== 2: 0){_LL39C: _tmp2CA=((struct Cyc_Absyn_Param_b_struct*)_tmp2C9)->f1;
goto _LL39A;}else{goto _LL38F;}}else{goto _LL38F;}_LL39A: if(! _tmp2CA->escapes){goto
_LL38E;}else{goto _LL38F;}_LL38F: goto _LL390;_LL388: _tmp2C6=_tmp2C4;goto _LL38A;
_LL38A: _tmp2C8=_tmp2C6;goto _LL38C;_LL38C: _tmp2CA=_tmp2C8;goto _LL38E;_LL38E: {
struct Cyc_List_List*_tmp2CB=({struct Cyc_List_List*_tmp2CF=_cycalloc(sizeof(
struct Cyc_List_List));_tmp2CF->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2D0=_cycalloc(
sizeof(struct Cyc_CfFlowInfo_Reln));_tmp2D0->vd=_tmp290;_tmp2D0->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessSize_struct*_tmp2D1=_cycalloc(sizeof(struct Cyc_CfFlowInfo_LessSize_struct));
_tmp2D1[0]=({struct Cyc_CfFlowInfo_LessSize_struct _tmp2D2;_tmp2D2.tag=2;_tmp2D2.f1=
_tmp2CA;_tmp2D2;});_tmp2D1;}));_tmp2D0;});_tmp2CF->tl=_tmp27F;_tmp2CF;});return({
struct _tuple0 _tmp2CC;_tmp2CC.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp2CD=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp2CD[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2CE;_tmp2CE.tag=0;_tmp2CE.f1=_tmp280;
_tmp2CE.f2=_tmp2CB;_tmp2CE;});_tmp2CD;});_tmp2CC.f2=_tmp27D;_tmp2CC;});}_LL390:
return({struct _tuple0 _tmp2D3;_tmp2D3.f1=_tmp27D;_tmp2D3.f2=_tmp27D;_tmp2D3;});
_LL386:;}_LL35A: return({struct _tuple0 _tmp2D4;_tmp2D4.f1=_tmp27D;_tmp2D4.f2=
_tmp27D;_tmp2D4;});_LL34C:;}_LL33F: return({struct _tuple0 _tmp2D5;_tmp2D5.f1=
_tmp27D;_tmp2D5.f2=_tmp27D;_tmp2D5;});_LL335:;}}_LL32E:;}}_LL2D7: {struct Cyc_List_List*
_tmp2D7;void*_tmp2D8;struct _tuple4 _tmp2D6=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp23B);_LL39F: _tmp2D8=_tmp2D6.f1;goto _LL39E;_LL39E: _tmp2D7=_tmp2D6.f2;
goto _LL39D;_LL39D: {void*_tmp2D9=_tmp2D8;struct Cyc_List_List*_tmp2DA;struct Cyc_Dict_Dict*
_tmp2DB;_LL3A1: if((int)_tmp2D9 == 0){goto _LL3A2;}else{goto _LL3A3;}_LL3A3: if((
unsigned int)_tmp2D9 > 1?*((int*)_tmp2D9)== 0: 0){_LL3A6: _tmp2DB=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2D9)->f1;goto _LL3A5;_LL3A5: _tmp2DA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2D9)->f2;goto _LL3A4;}else{goto _LL3A0;}_LL3A2: return({struct _tuple0 _tmp2DC;
_tmp2DC.f1=_tmp2D8;_tmp2DC.f2=_tmp2D8;_tmp2DC;});_LL3A4: {void*_tmp2DD=(void*)((
struct Cyc_List_List*)_check_null(_tmp2D7))->hd;void*_tmp2DE=(void*)((struct Cyc_List_List*)
_check_null(_tmp2D7->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp2DB,_tmp2DD)== (
void*)0){({void*_tmp2DF[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp23B))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp2DF,sizeof(void*),0));});}if(Cyc_CfFlowInfo_initlevel(
_tmp2DB,_tmp2DE)== (void*)0){({void*_tmp2E0[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp23B))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(unsigned char),34),_tag_arr(
_tmp2E0,sizeof(void*),0));});}{struct Cyc_Absyn_Exp*_tmp2E1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp23B))->hd;struct Cyc_Absyn_Exp*_tmp2E2=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp23B->tl))->hd;void*
_tmp2E3=(void*)_tmp2E1->r;void*_tmp2E4;struct Cyc_Absyn_Vardecl*_tmp2E5;void*
_tmp2E6;struct Cyc_Absyn_Vardecl*_tmp2E7;void*_tmp2E8;struct Cyc_Absyn_Vardecl*
_tmp2E9;void*_tmp2EA;struct Cyc_Absyn_Vardecl*_tmp2EB;_LL3A8: if(*((int*)_tmp2E3)
== 1){_LL3B3: _tmp2E4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E3)->f2;if((
unsigned int)_tmp2E4 > 1?*((int*)_tmp2E4)== 0: 0){_LL3B4: _tmp2E5=((struct Cyc_Absyn_Global_b_struct*)
_tmp2E4)->f1;goto _LL3B2;}else{goto _LL3AA;}}else{goto _LL3AA;}_LL3B2: if(! _tmp2E5->escapes){
goto _LL3A9;}else{goto _LL3AA;}_LL3AA: if(*((int*)_tmp2E3)== 1){_LL3B6: _tmp2E6=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E3)->f2;if((unsigned int)_tmp2E6 > 1?*((
int*)_tmp2E6)== 3: 0){_LL3B7: _tmp2E7=((struct Cyc_Absyn_Local_b_struct*)_tmp2E6)->f1;
goto _LL3B5;}else{goto _LL3AC;}}else{goto _LL3AC;}_LL3B5: if(! _tmp2E7->escapes){goto
_LL3AB;}else{goto _LL3AC;}_LL3AC: if(*((int*)_tmp2E3)== 1){_LL3B9: _tmp2E8=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2E3)->f2;if((unsigned int)_tmp2E8 > 1?*((int*)
_tmp2E8)== 4: 0){_LL3BA: _tmp2E9=((struct Cyc_Absyn_Pat_b_struct*)_tmp2E8)->f1;goto
_LL3B8;}else{goto _LL3AE;}}else{goto _LL3AE;}_LL3B8: if(! _tmp2E9->escapes){goto
_LL3AD;}else{goto _LL3AE;}_LL3AE: if(*((int*)_tmp2E3)== 1){_LL3BC: _tmp2EA=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2E3)->f2;if((unsigned int)_tmp2EA > 1?*((int*)
_tmp2EA)== 2: 0){_LL3BD: _tmp2EB=((struct Cyc_Absyn_Param_b_struct*)_tmp2EA)->f1;
goto _LL3BB;}else{goto _LL3B0;}}else{goto _LL3B0;}_LL3BB: if(! _tmp2EB->escapes){goto
_LL3AF;}else{goto _LL3B0;}_LL3B0: goto _LL3B1;_LL3A9: _tmp2E7=_tmp2E5;goto _LL3AB;
_LL3AB: _tmp2E9=_tmp2E7;goto _LL3AD;_LL3AD: _tmp2EB=_tmp2E9;goto _LL3AF;_LL3AF: {
void*_tmp2EC=(void*)_tmp2E2->r;struct Cyc_List_List*_tmp2ED;struct Cyc_List_List
_tmp2EE;struct Cyc_Absyn_Exp*_tmp2EF;void*_tmp2F0;_LL3BF: if(*((int*)_tmp2EC)== 3){
_LL3C5: _tmp2F0=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2EC)->f1;if((int)
_tmp2F0 == 19){goto _LL3C3;}else{goto _LL3C1;}_LL3C3: _tmp2ED=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2EC)->f2;if(_tmp2ED == 0){goto _LL3C1;}else{_tmp2EE=*_tmp2ED;_LL3C4: _tmp2EF=(
struct Cyc_Absyn_Exp*)_tmp2EE.hd;goto _LL3C0;}}else{goto _LL3C1;}_LL3C1: goto _LL3C2;
_LL3C0: {void*_tmp2F1=(void*)_tmp2EF->r;void*_tmp2F2;struct Cyc_Absyn_Vardecl*
_tmp2F3;void*_tmp2F4;struct Cyc_Absyn_Vardecl*_tmp2F5;void*_tmp2F6;struct Cyc_Absyn_Vardecl*
_tmp2F7;void*_tmp2F8;struct Cyc_Absyn_Vardecl*_tmp2F9;_LL3C7: if(*((int*)_tmp2F1)
== 1){_LL3D2: _tmp2F2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2F1)->f2;if((
unsigned int)_tmp2F2 > 1?*((int*)_tmp2F2)== 0: 0){_LL3D3: _tmp2F3=((struct Cyc_Absyn_Global_b_struct*)
_tmp2F2)->f1;goto _LL3D1;}else{goto _LL3C9;}}else{goto _LL3C9;}_LL3D1: if(! _tmp2F3->escapes){
goto _LL3C8;}else{goto _LL3C9;}_LL3C9: if(*((int*)_tmp2F1)== 1){_LL3D5: _tmp2F4=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2F1)->f2;if((unsigned int)_tmp2F4 > 1?*((
int*)_tmp2F4)== 3: 0){_LL3D6: _tmp2F5=((struct Cyc_Absyn_Local_b_struct*)_tmp2F4)->f1;
goto _LL3D4;}else{goto _LL3CB;}}else{goto _LL3CB;}_LL3D4: if(! _tmp2F5->escapes){goto
_LL3CA;}else{goto _LL3CB;}_LL3CB: if(*((int*)_tmp2F1)== 1){_LL3D8: _tmp2F6=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2F1)->f2;if((unsigned int)_tmp2F6 > 1?*((int*)
_tmp2F6)== 4: 0){_LL3D9: _tmp2F7=((struct Cyc_Absyn_Pat_b_struct*)_tmp2F6)->f1;goto
_LL3D7;}else{goto _LL3CD;}}else{goto _LL3CD;}_LL3D7: if(! _tmp2F7->escapes){goto
_LL3CC;}else{goto _LL3CD;}_LL3CD: if(*((int*)_tmp2F1)== 1){_LL3DB: _tmp2F8=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2F1)->f2;if((unsigned int)_tmp2F8 > 1?*((int*)
_tmp2F8)== 2: 0){_LL3DC: _tmp2F9=((struct Cyc_Absyn_Param_b_struct*)_tmp2F8)->f1;
goto _LL3DA;}else{goto _LL3CF;}}else{goto _LL3CF;}_LL3DA: if(! _tmp2F9->escapes){goto
_LL3CE;}else{goto _LL3CF;}_LL3CF: goto _LL3D0;_LL3C8: _tmp2F5=_tmp2F3;goto _LL3CA;
_LL3CA: _tmp2F7=_tmp2F5;goto _LL3CC;_LL3CC: _tmp2F9=_tmp2F7;goto _LL3CE;_LL3CE: {
struct Cyc_List_List*_tmp2FA=({struct Cyc_List_List*_tmp2FE=_cycalloc(sizeof(
struct Cyc_List_List));_tmp2FE->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2FF=_cycalloc(
sizeof(struct Cyc_CfFlowInfo_Reln));_tmp2FF->vd=_tmp2EB;_tmp2FF->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp300=_cycalloc(sizeof(struct
Cyc_CfFlowInfo_LessEqSize_struct));_tmp300[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp301;_tmp301.tag=4;_tmp301.f1=_tmp2F9;_tmp301;});_tmp300;}));_tmp2FF;});
_tmp2FE->tl=_tmp2DA;_tmp2FE;});return({struct _tuple0 _tmp2FB;_tmp2FB.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2FC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));
_tmp2FC[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2FD;_tmp2FD.tag=0;
_tmp2FD.f1=_tmp2DB;_tmp2FD.f2=_tmp2FA;_tmp2FD;});_tmp2FC;});_tmp2FB.f2=_tmp2D8;
_tmp2FB;});}_LL3D0: return({struct _tuple0 _tmp302;_tmp302.f1=_tmp2D8;_tmp302.f2=
_tmp2D8;_tmp302;});_LL3C6:;}_LL3C2: return({struct _tuple0 _tmp303;_tmp303.f1=
_tmp2D8;_tmp303.f2=_tmp2D8;_tmp303;});_LL3BE:;}_LL3B1: return({struct _tuple0
_tmp304;_tmp304.f1=_tmp2D8;_tmp304.f2=_tmp2D8;_tmp304;});_LL3A7:;}}_LL3A0:;}}
_LL2D9: goto _LL2C9;_LL2C9:;}{void*_tmp306;void*_tmp307;struct _tuple0 _tmp305=Cyc_NewControlFlow_anal_Rexp(
env,inflow,e);_LL3DF: _tmp307=_tmp305.f1;goto _LL3DE;_LL3DE: _tmp306=_tmp305.f2;
goto _LL3DD;_LL3DD: {void*_tmp308=_tmp307;struct Cyc_List_List*_tmp309;struct Cyc_Dict_Dict*
_tmp30A;_LL3E1: if((int)_tmp308 == 0){goto _LL3E2;}else{goto _LL3E3;}_LL3E3: if((
unsigned int)_tmp308 > 1?*((int*)_tmp308)== 0: 0){_LL3E6: _tmp30A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp308)->f1;goto _LL3E5;_LL3E5: _tmp309=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp308)->f2;goto _LL3E4;}else{goto _LL3E0;}_LL3E2: return({struct _tuple0 _tmp30B;
_tmp30B.f1=_tmp307;_tmp30B.f2=_tmp307;_tmp30B;});_LL3E4: {void*_tmp30C=_tmp306;
void*_tmp30D;void*_tmp30E;void*_tmp30F;_LL3E8: if((int)_tmp30C == 0){goto _LL3E9;}
else{goto _LL3EA;}_LL3EA: if((int)_tmp30C == 2){goto _LL3EB;}else{goto _LL3EC;}_LL3EC:
if((int)_tmp30C == 1){goto _LL3ED;}else{goto _LL3EE;}_LL3EE: if((unsigned int)
_tmp30C > 3?*((int*)_tmp30C)== 2: 0){goto _LL3EF;}else{goto _LL3F0;}_LL3F0: if((
unsigned int)_tmp30C > 3?*((int*)_tmp30C)== 0: 0){_LL3FA: _tmp30D=(void*)((struct
Cyc_CfFlowInfo_UnknownR_struct*)_tmp30C)->f1;if((int)_tmp30D == 0){goto _LL3F1;}
else{goto _LL3F2;}}else{goto _LL3F2;}_LL3F2: if((unsigned int)_tmp30C > 3?*((int*)
_tmp30C)== 1: 0){_LL3FB: _tmp30E=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp30C)->f1;
if((int)_tmp30E == 0){goto _LL3F3;}else{goto _LL3F4;}}else{goto _LL3F4;}_LL3F4: if((
unsigned int)_tmp30C > 3?*((int*)_tmp30C)== 0: 0){_LL3FC: _tmp30F=(void*)((struct
Cyc_CfFlowInfo_UnknownR_struct*)_tmp30C)->f1;goto _LL3F5;}else{goto _LL3F6;}_LL3F6:
if((unsigned int)_tmp30C > 3?*((int*)_tmp30C)== 1: 0){goto _LL3F7;}else{goto _LL3F8;}
_LL3F8: if((unsigned int)_tmp30C > 3?*((int*)_tmp30C)== 3: 0){goto _LL3F9;}else{goto
_LL3E7;}_LL3E9: return({struct _tuple0 _tmp310;_tmp310.f1=(void*)0;_tmp310.f2=
_tmp307;_tmp310;});_LL3EB: goto _LL3ED;_LL3ED: goto _LL3EF;_LL3EF: return({struct
_tuple0 _tmp311;_tmp311.f1=_tmp307;_tmp311.f2=(void*)0;_tmp311;});_LL3F1: goto
_LL3F3;_LL3F3:({void*_tmp312[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp312,sizeof(void*),0));});return({struct
_tuple0 _tmp313;_tmp313.f1=(void*)0;_tmp313.f2=(void*)0;_tmp313;});_LL3F5: return
Cyc_NewControlFlow_splitzero(env,inflow,_tmp307,e,_tmp30F);_LL3F7: return({struct
_tuple0 _tmp314;_tmp314.f1=_tmp307;_tmp314.f2=_tmp307;_tmp314;});_LL3F9:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp315=_cycalloc(sizeof(struct
Cyc_Core_Impossible_struct));_tmp315[0]=({struct Cyc_Core_Impossible_struct
_tmp316;_tmp316.tag=Cyc_Core_Impossible;_tmp316.f1=_tag_arr("anal_test",sizeof(
unsigned char),10);_tmp316;});_tmp315;}));_LL3E7:;}_LL3E0:;}}}static void*Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_List_List*scs){for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp318;struct Cyc_Absyn_Stmt*
_tmp319;struct Cyc_Absyn_Exp*_tmp31A;struct Cyc_Core_Opt*_tmp31B;struct Cyc_Absyn_Switch_clause*
_tmp317=(struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp318=*_tmp317;_LL400: _tmp31B=
_tmp318.pat_vars;goto _LL3FF;_LL3FF: _tmp31A=_tmp318.where_clause;goto _LL3FE;
_LL3FE: _tmp319=_tmp318.body;goto _LL3FD;_LL3FD: {void*clause_inflow=Cyc_NewControlFlow_add_vars(
inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp31B))->v,Cyc_CfFlowInfo_unknown_all);
void*clause_outflow;if(_tmp31A != 0){void*_tmp31D;void*_tmp31E;struct _tuple0
_tmp31C=Cyc_NewControlFlow_anal_test(env,clause_inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp31A));_LL403: _tmp31E=_tmp31C.f1;goto _LL402;_LL402: _tmp31D=
_tmp31C.f2;goto _LL401;_LL401: inflow=_tmp31D;clause_outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp31E,_tmp319);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp319);}{void*_tmp31F=clause_outflow;_LL405: if((int)_tmp31F == 0){
goto _LL406;}else{goto _LL407;}_LL407: goto _LL408;_LL406: goto _LL404;_LL408: if(scs->tl
== 0){return clause_outflow;}else{if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(
scs->tl))->hd)->pat_vars))->v != 0){({void*_tmp320[0]={};Cyc_Tcutil_terr(_tmp319->loc,
_tag_arr("switch clause may implicitly fallthru",sizeof(unsigned char),38),
_tag_arr(_tmp320,sizeof(void*),0));});}else{({void*_tmp321[0]={};Cyc_Tcutil_warn(
_tmp319->loc,_tag_arr("switch clause may implicitly fallthru",sizeof(
unsigned char),38),_tag_arr(_tmp321,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(
env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL404;_LL404:;}}}return(void*)0;}static void*Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Stmt*s){struct
Cyc_NewControlFlow_CFStmtAnnot*_tmp322=Cyc_NewControlFlow_pre_stmt_check(env,
inflow,s);inflow=(void*)_tmp322->flow;{void*_tmp323=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp324;struct Cyc_Absyn_Exp*_tmp325;struct Cyc_Absyn_Exp*_tmp326;struct Cyc_Absyn_Stmt*
_tmp327;struct Cyc_Absyn_Stmt*_tmp328;struct Cyc_Absyn_Stmt*_tmp329;struct Cyc_Absyn_Stmt*
_tmp32A;struct Cyc_Absyn_Exp*_tmp32B;struct Cyc_Absyn_Stmt*_tmp32C;struct _tuple3
_tmp32D;struct Cyc_Absyn_Stmt*_tmp32E;struct Cyc_Absyn_Exp*_tmp32F;struct _tuple3
_tmp330;struct Cyc_Absyn_Stmt*_tmp331;struct Cyc_Absyn_Exp*_tmp332;struct Cyc_Absyn_Stmt*
_tmp333;struct Cyc_Absyn_Stmt*_tmp334;struct _tuple3 _tmp335;struct Cyc_Absyn_Stmt*
_tmp336;struct Cyc_Absyn_Exp*_tmp337;struct _tuple3 _tmp338;struct Cyc_Absyn_Stmt*
_tmp339;struct Cyc_Absyn_Exp*_tmp33A;struct Cyc_Absyn_Exp*_tmp33B;struct Cyc_Absyn_Stmt*
_tmp33C;struct Cyc_Absyn_Switch_clause**_tmp33D;struct Cyc_Absyn_Switch_clause*
_tmp33E;struct Cyc_List_List*_tmp33F;struct Cyc_Absyn_Stmt*_tmp340;struct Cyc_Absyn_Stmt*
_tmp341;struct Cyc_Absyn_Stmt*_tmp342;struct Cyc_List_List*_tmp343;struct Cyc_Absyn_Exp*
_tmp344;struct Cyc_List_List*_tmp345;struct Cyc_Absyn_Stmt*_tmp346;struct Cyc_Absyn_Stmt*
_tmp347;struct Cyc_Absyn_Decl*_tmp348;struct Cyc_Absyn_Stmt*_tmp349;struct Cyc_Absyn_Stmt*
_tmp34A;struct Cyc_Absyn_Vardecl*_tmp34B;struct Cyc_Absyn_Tvar*_tmp34C;_LL40A: if((
int)_tmp323 == 0){goto _LL40B;}else{goto _LL40C;}_LL40C: if((unsigned int)_tmp323 > 1?*((
int*)_tmp323)== 2: 0){_LL432: _tmp324=((struct Cyc_Absyn_Return_s_struct*)_tmp323)->f1;
if(_tmp324 == 0){goto _LL40D;}else{goto _LL40E;}}else{goto _LL40E;}_LL40E: if((
unsigned int)_tmp323 > 1?*((int*)_tmp323)== 2: 0){_LL433: _tmp325=((struct Cyc_Absyn_Return_s_struct*)
_tmp323)->f1;goto _LL40F;}else{goto _LL410;}_LL410: if((unsigned int)_tmp323 > 1?*((
int*)_tmp323)== 0: 0){_LL434: _tmp326=((struct Cyc_Absyn_Exp_s_struct*)_tmp323)->f1;
goto _LL411;}else{goto _LL412;}_LL412: if((unsigned int)_tmp323 > 1?*((int*)_tmp323)
== 1: 0){_LL436: _tmp328=((struct Cyc_Absyn_Seq_s_struct*)_tmp323)->f1;goto _LL435;
_LL435: _tmp327=((struct Cyc_Absyn_Seq_s_struct*)_tmp323)->f2;goto _LL413;}else{
goto _LL414;}_LL414: if((unsigned int)_tmp323 > 1?*((int*)_tmp323)== 3: 0){_LL439:
_tmp32B=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp323)->f1;goto _LL438;_LL438:
_tmp32A=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp323)->f2;goto _LL437;_LL437:
_tmp329=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp323)->f3;goto _LL415;}else{
goto _LL416;}_LL416: if((unsigned int)_tmp323 > 1?*((int*)_tmp323)== 4: 0){_LL43B:
_tmp32D=((struct Cyc_Absyn_While_s_struct*)_tmp323)->f1;_LL43D: _tmp32F=_tmp32D.f1;
goto _LL43C;_LL43C: _tmp32E=_tmp32D.f2;goto _LL43A;_LL43A: _tmp32C=((struct Cyc_Absyn_While_s_struct*)
_tmp323)->f2;goto _LL417;}else{goto _LL418;}_LL418: if((unsigned int)_tmp323 > 1?*((
int*)_tmp323)== 16: 0){_LL441: _tmp333=((struct Cyc_Absyn_Do_s_struct*)_tmp323)->f1;
goto _LL43E;_LL43E: _tmp330=((struct Cyc_Absyn_Do_s_struct*)_tmp323)->f2;_LL440:
_tmp332=_tmp330.f1;goto _LL43F;_LL43F: _tmp331=_tmp330.f2;goto _LL419;}else{goto
_LL41A;}_LL41A: if((unsigned int)_tmp323 > 1?*((int*)_tmp323)== 8: 0){_LL449:
_tmp33B=((struct Cyc_Absyn_For_s_struct*)_tmp323)->f1;goto _LL446;_LL446: _tmp338=((
struct Cyc_Absyn_For_s_struct*)_tmp323)->f2;_LL448: _tmp33A=_tmp338.f1;goto _LL447;
_LL447: _tmp339=_tmp338.f2;goto _LL443;_LL443: _tmp335=((struct Cyc_Absyn_For_s_struct*)
_tmp323)->f3;_LL445: _tmp337=_tmp335.f1;goto _LL444;_LL444: _tmp336=_tmp335.f2;goto
_LL442;_LL442: _tmp334=((struct Cyc_Absyn_For_s_struct*)_tmp323)->f4;goto _LL41B;}
else{goto _LL41C;}_LL41C: if((unsigned int)_tmp323 > 1?*((int*)_tmp323)== 5: 0){
_LL44A: _tmp33C=((struct Cyc_Absyn_Break_s_struct*)_tmp323)->f1;if(_tmp33C == 0){
goto _LL41D;}else{goto _LL41E;}}else{goto _LL41E;}_LL41E: if((unsigned int)_tmp323 > 
1?*((int*)_tmp323)== 11: 0){_LL44C: _tmp33F=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp323)->f1;goto _LL44B;_LL44B: _tmp33D=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp323)->f2;if(_tmp33D == 0){goto _LL420;}else{_tmp33E=*_tmp33D;goto _LL41F;}}
else{goto _LL420;}_LL420: if((unsigned int)_tmp323 > 1?*((int*)_tmp323)== 5: 0){
_LL44D: _tmp340=((struct Cyc_Absyn_Break_s_struct*)_tmp323)->f1;goto _LL421;}else{
goto _LL422;}_LL422: if((unsigned int)_tmp323 > 1?*((int*)_tmp323)== 6: 0){_LL44E:
_tmp341=((struct Cyc_Absyn_Continue_s_struct*)_tmp323)->f1;goto _LL423;}else{goto
_LL424;}_LL424: if((unsigned int)_tmp323 > 1?*((int*)_tmp323)== 7: 0){_LL44F:
_tmp342=((struct Cyc_Absyn_Goto_s_struct*)_tmp323)->f2;goto _LL425;}else{goto
_LL426;}_LL426: if((unsigned int)_tmp323 > 1?*((int*)_tmp323)== 9: 0){_LL451:
_tmp344=((struct Cyc_Absyn_Switch_s_struct*)_tmp323)->f1;goto _LL450;_LL450:
_tmp343=((struct Cyc_Absyn_Switch_s_struct*)_tmp323)->f2;goto _LL427;}else{goto
_LL428;}_LL428: if((unsigned int)_tmp323 > 1?*((int*)_tmp323)== 17: 0){_LL453:
_tmp346=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp323)->f1;goto _LL452;_LL452:
_tmp345=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp323)->f2;goto _LL429;}else{goto
_LL42A;}_LL42A: if((unsigned int)_tmp323 > 1?*((int*)_tmp323)== 12: 0){_LL455:
_tmp348=((struct Cyc_Absyn_Decl_s_struct*)_tmp323)->f1;goto _LL454;_LL454: _tmp347=((
struct Cyc_Absyn_Decl_s_struct*)_tmp323)->f2;goto _LL42B;}else{goto _LL42C;}_LL42C:
if((unsigned int)_tmp323 > 1?*((int*)_tmp323)== 15: 0){_LL456: _tmp349=((struct Cyc_Absyn_Label_s_struct*)
_tmp323)->f2;goto _LL42D;}else{goto _LL42E;}_LL42E: if((unsigned int)_tmp323 > 1?*((
int*)_tmp323)== 18: 0){_LL459: _tmp34C=((struct Cyc_Absyn_Region_s_struct*)_tmp323)->f1;
goto _LL458;_LL458: _tmp34B=((struct Cyc_Absyn_Region_s_struct*)_tmp323)->f2;goto
_LL457;_LL457: _tmp34A=((struct Cyc_Absyn_Region_s_struct*)_tmp323)->f3;goto _LL42F;}
else{goto _LL430;}_LL430: goto _LL431;_LL40B: return inflow;_LL40D: if(env->noreturn){({
void*_tmp34D[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("`noreturn' function might return",
sizeof(unsigned char),33),_tag_arr(_tmp34D,sizeof(void*),0));});}return(void*)0;
_LL40F: if(env->noreturn){({void*_tmp34E[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("`noreturn' function might return",
sizeof(unsigned char),33),_tag_arr(_tmp34E,sizeof(void*),0));});}{void*_tmp350;
void*_tmp351;struct _tuple0 _tmp34F=Cyc_NewControlFlow_anal_Rexp(env,inflow,(
struct Cyc_Absyn_Exp*)_check_null(_tmp325));_LL45C: _tmp351=_tmp34F.f1;goto _LL45B;
_LL45B: _tmp350=_tmp34F.f2;goto _LL45A;_LL45A: Cyc_NewControlFlow_use_Rval(env,((
struct Cyc_Absyn_Exp*)_check_null(_tmp325))->loc,_tmp351,_tmp350);return(void*)0;}
_LL411: return(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp326)).f1;_LL413: return
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp328),
_tmp327);_LL415: {void*_tmp353;void*_tmp354;struct _tuple0 _tmp352=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp32B);_LL45F: _tmp354=_tmp352.f1;goto _LL45E;_LL45E: _tmp353=_tmp352.f2;
goto _LL45D;_LL45D: return Cyc_CfFlowInfo_join_flow(env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp354,_tmp32A),Cyc_NewControlFlow_anal_stmt(env,_tmp353,_tmp329));}_LL417: {
void*_tmp355=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp32E))->flow;
void*_tmp357;void*_tmp358;struct _tuple0 _tmp356=Cyc_NewControlFlow_anal_test(env,
_tmp355,_tmp32F);_LL462: _tmp358=_tmp356.f1;goto _LL461;_LL461: _tmp357=_tmp356.f2;
goto _LL460;_LL460: {void*_tmp359=Cyc_NewControlFlow_anal_stmt(env,_tmp358,
_tmp32C);Cyc_NewControlFlow_update_flow(env,_tmp32E,_tmp359);return _tmp357;}}
_LL419: {void*_tmp35A=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp333);void*
_tmp35B=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp35A,_tmp331))->flow;
void*_tmp35D;void*_tmp35E;struct _tuple0 _tmp35C=Cyc_NewControlFlow_anal_test(env,
_tmp35B,_tmp332);_LL465: _tmp35E=_tmp35C.f1;goto _LL464;_LL464: _tmp35D=_tmp35C.f2;
goto _LL463;_LL463: Cyc_NewControlFlow_update_flow(env,_tmp333,_tmp35E);return
_tmp35D;}_LL41B: {void*_tmp35F=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp33B)).f1;
void*_tmp360=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp35F,_tmp339))->flow;
void*_tmp362;void*_tmp363;struct _tuple0 _tmp361=Cyc_NewControlFlow_anal_test(env,
_tmp360,_tmp33A);_LL468: _tmp363=_tmp361.f1;goto _LL467;_LL467: _tmp362=_tmp361.f2;
goto _LL466;_LL466: {void*_tmp364=Cyc_NewControlFlow_anal_stmt(env,_tmp363,
_tmp334);void*_tmp365=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp364,
_tmp336))->flow;void*_tmp366=(Cyc_NewControlFlow_anal_Rexp(env,_tmp365,_tmp337)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp339,_tmp366);return _tmp362;}}_LL41D:
return(void*)0;_LL41F: {struct Cyc_List_List*_tmp368;void*_tmp369;struct _tuple4
_tmp367=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp33F);_LL46B:
_tmp369=_tmp367.f1;goto _LL46A;_LL46A: _tmp368=_tmp367.f2;goto _LL469;_LL469: for(0;
_tmp368 != 0;(_tmp368=_tmp368->tl,_tmp33F=_tmp33F->tl)){_tmp369=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp33F))->hd)->loc,
_tmp369,(void*)_tmp368->hd);}_tmp369=Cyc_NewControlFlow_add_vars(_tmp369,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp33E->pat_vars))->v,Cyc_CfFlowInfo_unknown_all);
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp33E->body,_tmp369);
return(void*)0;}_LL421: _tmp341=_tmp340;goto _LL423;_LL423: _tmp342=_tmp341;goto
_LL425;_LL425: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp36A=_tmp322->encloser;
struct Cyc_Absyn_Stmt*_tmp36B=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)
_check_null(_tmp342)))->encloser;while(_tmp36B != _tmp36A){struct Cyc_Absyn_Stmt*
_tmp36C=(Cyc_NewControlFlow_get_stmt_annot(_tmp36A))->encloser;if(_tmp36C == 
_tmp36A){({void*_tmp36D[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("goto enters local scope or exception handler",
sizeof(unsigned char),45),_tag_arr(_tmp36D,sizeof(void*),0));});break;}_tmp36A=
_tmp36C;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_check_null(
_tmp342),inflow);return(void*)0;_LL427: {void*_tmp36F;void*_tmp370;struct _tuple0
_tmp36E=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp344);_LL46E: _tmp370=_tmp36E.f1;
goto _LL46D;_LL46D: _tmp36F=_tmp36E.f2;goto _LL46C;_LL46C: _tmp370=Cyc_NewControlFlow_use_Rval(
env,_tmp344->loc,_tmp370,_tmp36F);return Cyc_NewControlFlow_anal_scs(env,_tmp370,
_tmp343);}_LL429: {int old_in_try=env->in_try;void*old_tryflow=(void*)env->tryflow;
env->in_try=1;(void*)(env->tryflow=(void*)inflow);{void*s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp346);void*scs_inflow=(void*)env->tryflow;env->in_try=old_in_try;(
void*)(env->tryflow=(void*)old_tryflow);Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{void*scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp345);{
void*_tmp371=scs_outflow;_LL470: if((int)_tmp371 == 0){goto _LL471;}else{goto _LL472;}
_LL472: goto _LL473;_LL471: goto _LL46F;_LL473:({void*_tmp372[0]={};Cyc_Tcutil_terr(
s->loc,_tag_arr("last catch clause may implicitly fallthru",sizeof(unsigned char),
42),_tag_arr(_tmp372,sizeof(void*),0));});_LL46F:;}return s1_outflow;}}}_LL42B:
return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp348),_tmp347);_LL42D: return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp349);
_LL42F: {struct Cyc_List_List _tmp373=({struct Cyc_List_List _tmp374;_tmp374.hd=
_tmp34B;_tmp374.tl=0;_tmp374;});inflow=Cyc_NewControlFlow_add_vars(inflow,(
struct Cyc_List_List*)& _tmp373,Cyc_CfFlowInfo_unknown_all);return Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp34A);}_LL431:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp375=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp375[0]=({struct
Cyc_Core_Impossible_struct _tmp376;_tmp376.tag=Cyc_Core_Impossible;_tmp376.f1=
_tag_arr("anal_stmt -- bad stmt syntax or unimplemented stmt form",sizeof(
unsigned char),56);_tmp376;});_tmp375;}));_LL409:;}}static void*Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Decl*decl){void*
_tmp377=(void*)decl->r;struct Cyc_Absyn_Vardecl*_tmp378;struct Cyc_Absyn_Exp*
_tmp379;struct Cyc_Core_Opt*_tmp37A;struct Cyc_Core_Opt _tmp37B;struct Cyc_List_List*
_tmp37C;struct Cyc_List_List*_tmp37D;_LL475: if(*((int*)_tmp377)== 0){_LL47D:
_tmp378=((struct Cyc_Absyn_Var_d_struct*)_tmp377)->f1;goto _LL476;}else{goto _LL477;}
_LL477: if(*((int*)_tmp377)== 2){_LL47F: _tmp37A=((struct Cyc_Absyn_Let_d_struct*)
_tmp377)->f2;if(_tmp37A == 0){goto _LL479;}else{_tmp37B=*_tmp37A;_LL480: _tmp37C=(
struct Cyc_List_List*)_tmp37B.v;goto _LL47E;}_LL47E: _tmp379=((struct Cyc_Absyn_Let_d_struct*)
_tmp377)->f4;goto _LL478;}else{goto _LL479;}_LL479: if(*((int*)_tmp377)== 3){_LL481:
_tmp37D=((struct Cyc_Absyn_Letv_d_struct*)_tmp377)->f1;goto _LL47A;}else{goto
_LL47B;}_LL47B: goto _LL47C;_LL476: {struct Cyc_List_List _tmp37E=({struct Cyc_List_List
_tmp38C;_tmp38C.hd=_tmp378;_tmp38C.tl=0;_tmp38C;});inflow=Cyc_NewControlFlow_add_vars(
inflow,(struct Cyc_List_List*)& _tmp37E,Cyc_CfFlowInfo_unknown_none);{struct Cyc_Absyn_Exp*
_tmp37F=_tmp378->initializer;if(_tmp37F == 0){return inflow;}{void*_tmp381;void*
_tmp382;struct _tuple0 _tmp380=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp37F));_LL484: _tmp382=_tmp380.f1;goto _LL483;_LL483: _tmp381=
_tmp380.f2;goto _LL482;_LL482: {void*_tmp383=_tmp382;struct Cyc_List_List*_tmp384;
struct Cyc_Dict_Dict*_tmp385;_LL486: if((int)_tmp383 == 0){goto _LL487;}else{goto
_LL488;}_LL488: if((unsigned int)_tmp383 > 1?*((int*)_tmp383)== 0: 0){_LL48B:
_tmp385=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp383)->f1;goto _LL48A;
_LL48A: _tmp384=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp383)->f2;goto
_LL489;}else{goto _LL485;}_LL487: return(void*)0;_LL489: _tmp385=Cyc_CfFlowInfo_assign_place(
decl->loc,_tmp385,env->all_changed,({struct Cyc_CfFlowInfo_Place*_tmp386=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_tmp386->root=(void*)((void*)({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp387=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_struct));
_tmp387[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp388;_tmp388.tag=0;_tmp388.f1=
_tmp378;_tmp388;});_tmp387;}));_tmp386->fields=0;_tmp386;}),_tmp381);_tmp384=Cyc_CfFlowInfo_reln_assign_var(
_tmp384,_tmp378,(struct Cyc_Absyn_Exp*)_check_null(_tmp378->initializer));{struct
Cyc_CfFlowInfo_ReachableFL_struct*_tmp389=({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp38A=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp38A[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp38B;_tmp38B.tag=0;_tmp38B.f1=_tmp385;
_tmp38B.f2=_tmp384;_tmp38B;});_tmp38A;});Cyc_NewControlFlow_update_tryflow(env,(
void*)_tmp389);return(void*)_tmp389;}_LL485:;}}}}_LL478: {void*_tmp38E;void*
_tmp38F;struct _tuple0 _tmp38D=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp379);
_LL48E: _tmp38F=_tmp38D.f1;goto _LL48D;_LL48D: _tmp38E=_tmp38D.f2;goto _LL48C;_LL48C:
_tmp38F=Cyc_NewControlFlow_use_Rval(env,_tmp379->loc,_tmp38F,_tmp38E);return Cyc_NewControlFlow_add_vars(
_tmp38F,_tmp37C,Cyc_CfFlowInfo_unknown_all);}_LL47A: return Cyc_NewControlFlow_add_vars(
inflow,_tmp37D,Cyc_CfFlowInfo_unknown_none);_LL47C:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp390=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));
_tmp390[0]=({struct Cyc_Core_Impossible_struct _tmp391;_tmp391.tag=Cyc_Core_Impossible;
_tmp391.f1=_tag_arr("anal_decl: unexpected decl variant",sizeof(unsigned char),
35);_tmp391;});_tmp390;}));_LL474:;}static void Cyc_NewControlFlow_postpass(struct
Cyc_Absyn_Stmt*s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{void*
_tmp392=(void*)s->r;struct Cyc_Absyn_Stmt*_tmp393;struct _tuple3 _tmp394;struct Cyc_Absyn_Stmt*
_tmp395;struct _tuple3 _tmp396;struct Cyc_Absyn_Stmt*_tmp397;struct Cyc_Absyn_Stmt*
_tmp398;struct Cyc_Absyn_Stmt*_tmp399;struct Cyc_Absyn_Stmt*_tmp39A;struct _tuple3
_tmp39B;struct Cyc_Absyn_Stmt*_tmp39C;struct _tuple3 _tmp39D;struct Cyc_Absyn_Stmt*
_tmp39E;struct Cyc_Absyn_Stmt*_tmp39F;struct Cyc_Absyn_Stmt*_tmp3A0;struct Cyc_Absyn_Stmt*
_tmp3A1;struct Cyc_Absyn_Stmt*_tmp3A2;struct Cyc_Absyn_Stmt*_tmp3A3;struct Cyc_Absyn_Stmt*
_tmp3A4;struct Cyc_List_List*_tmp3A5;struct Cyc_Absyn_Stmt*_tmp3A6;struct Cyc_List_List*
_tmp3A7;_LL490: if((unsigned int)_tmp392 > 1?*((int*)_tmp392)== 8: 0){_LL4A9:
_tmp396=((struct Cyc_Absyn_For_s_struct*)_tmp392)->f2;_LL4AA: _tmp397=_tmp396.f2;
goto _LL4A7;_LL4A7: _tmp394=((struct Cyc_Absyn_For_s_struct*)_tmp392)->f3;_LL4A8:
_tmp395=_tmp394.f2;goto _LL4A6;_LL4A6: _tmp393=((struct Cyc_Absyn_For_s_struct*)
_tmp392)->f4;goto _LL491;}else{goto _LL492;}_LL492: if((unsigned int)_tmp392 > 1?*((
int*)_tmp392)== 1: 0){_LL4AC: _tmp399=((struct Cyc_Absyn_Seq_s_struct*)_tmp392)->f1;
goto _LL4AB;_LL4AB: _tmp398=((struct Cyc_Absyn_Seq_s_struct*)_tmp392)->f2;goto
_LL493;}else{goto _LL494;}_LL494: if((unsigned int)_tmp392 > 1?*((int*)_tmp392)== 4:
0){_LL4AE: _tmp39B=((struct Cyc_Absyn_While_s_struct*)_tmp392)->f1;_LL4AF: _tmp39C=
_tmp39B.f2;goto _LL4AD;_LL4AD: _tmp39A=((struct Cyc_Absyn_While_s_struct*)_tmp392)->f2;
goto _LL495;}else{goto _LL496;}_LL496: if((unsigned int)_tmp392 > 1?*((int*)_tmp392)
== 16: 0){_LL4B2: _tmp39F=((struct Cyc_Absyn_Do_s_struct*)_tmp392)->f1;goto _LL4B0;
_LL4B0: _tmp39D=((struct Cyc_Absyn_Do_s_struct*)_tmp392)->f2;_LL4B1: _tmp39E=
_tmp39D.f2;goto _LL497;}else{goto _LL498;}_LL498: if((unsigned int)_tmp392 > 1?*((
int*)_tmp392)== 3: 0){_LL4B4: _tmp3A1=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp392)->f2;goto _LL4B3;_LL4B3: _tmp3A0=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp392)->f3;goto _LL499;}else{goto _LL49A;}_LL49A: if((unsigned int)_tmp392 > 1?*((
int*)_tmp392)== 18: 0){_LL4B5: _tmp3A2=((struct Cyc_Absyn_Region_s_struct*)_tmp392)->f3;
goto _LL49B;}else{goto _LL49C;}_LL49C: if((unsigned int)_tmp392 > 1?*((int*)_tmp392)
== 15: 0){_LL4B6: _tmp3A3=((struct Cyc_Absyn_Label_s_struct*)_tmp392)->f2;goto
_LL49D;}else{goto _LL49E;}_LL49E: if((unsigned int)_tmp392 > 1?*((int*)_tmp392)== 
12: 0){_LL4B7: _tmp3A4=((struct Cyc_Absyn_Decl_s_struct*)_tmp392)->f2;goto _LL49F;}
else{goto _LL4A0;}_LL4A0: if((unsigned int)_tmp392 > 1?*((int*)_tmp392)== 17: 0){
_LL4B9: _tmp3A6=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp392)->f1;goto _LL4B8;
_LL4B8: _tmp3A5=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp392)->f2;goto _LL4A1;}
else{goto _LL4A2;}_LL4A2: if((unsigned int)_tmp392 > 1?*((int*)_tmp392)== 9: 0){
_LL4BA: _tmp3A7=((struct Cyc_Absyn_Switch_s_struct*)_tmp392)->f2;goto _LL4A3;}else{
goto _LL4A4;}_LL4A4: goto _LL4A5;_LL491: Cyc_NewControlFlow_postpass(_tmp395);
_tmp399=_tmp393;_tmp398=_tmp397;goto _LL493;_LL493: _tmp39C=_tmp399;_tmp39A=
_tmp398;goto _LL495;_LL495: _tmp39F=_tmp39A;_tmp39E=_tmp39C;goto _LL497;_LL497:
_tmp3A1=_tmp39F;_tmp3A0=_tmp39E;goto _LL499;_LL499: Cyc_NewControlFlow_postpass(
_tmp3A1);_tmp3A2=_tmp3A0;goto _LL49B;_LL49B: _tmp3A3=_tmp3A2;goto _LL49D;_LL49D:
_tmp3A4=_tmp3A3;goto _LL49F;_LL49F: Cyc_NewControlFlow_postpass(_tmp3A4);return;
_LL4A1: Cyc_NewControlFlow_postpass(_tmp3A6);_tmp3A7=_tmp3A5;goto _LL4A3;_LL4A3:
for(0;_tmp3A7 != 0;_tmp3A7=_tmp3A7->tl){Cyc_NewControlFlow_postpass(((struct Cyc_Absyn_Switch_clause*)
_tmp3A7->hd)->body);}return;_LL4A5: return;_LL48F:;}}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){void*inflow=Cyc_NewControlFlow_add_vars((void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3AE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));
_tmp3AE[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3AF;_tmp3AF.tag=0;
_tmp3AF.f1=Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp3AF.f2=0;_tmp3AF;});
_tmp3AE;}),(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,
Cyc_CfFlowInfo_unknown_all);int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp3AD=_cycalloc(sizeof(struct Cyc_NewControlFlow_AnalEnv));_tmp3AD->iterate_again=
1;_tmp3AD->iteration_num=0;_tmp3AD->in_try=0;_tmp3AD->tryflow=(void*)inflow;
_tmp3AD->all_changed=0;_tmp3AD->noreturn=noreturn;_tmp3AD;});void*outflow=inflow;
while(env->iterate_again){++ env->iteration_num;env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,fd->body);}{void*_tmp3A8=outflow;_LL4BC: if((int)_tmp3A8 == 0){goto
_LL4BD;}else{goto _LL4BE;}_LL4BE: goto _LL4BF;_LL4BD: goto _LL4BB;_LL4BF: if(noreturn){({
void*_tmp3A9[0]={};Cyc_Tcutil_terr((fd->body)->loc,_tag_arr("`noreturn' function might (implicitly) return",
sizeof(unsigned char),46),_tag_arr(_tmp3A9,sizeof(void*),0));});}else{{void*
_tmp3AA=Cyc_Tcutil_compress((void*)fd->ret_type);_LL4C1: if((int)_tmp3AA == 0){
goto _LL4C2;}else{goto _LL4C3;}_LL4C3: if((unsigned int)_tmp3AA > 3?*((int*)_tmp3AA)
== 6: 0){goto _LL4C4;}else{goto _LL4C5;}_LL4C5: if((int)_tmp3AA == 1){goto _LL4C6;}
else{goto _LL4C7;}_LL4C7: if((unsigned int)_tmp3AA > 3?*((int*)_tmp3AA)== 5: 0){goto
_LL4C8;}else{goto _LL4C9;}_LL4C9: goto _LL4CA;_LL4C2: goto _LL4C0;_LL4C4: goto _LL4C6;
_LL4C6: goto _LL4C8;_LL4C8:({void*_tmp3AB[0]={};Cyc_Tcutil_warn((fd->body)->loc,
_tag_arr("function may not return a value",sizeof(unsigned char),32),_tag_arr(
_tmp3AB,sizeof(void*),0));});goto _LL4C0;_LL4CA:({void*_tmp3AC[0]={};Cyc_Tcutil_terr((
fd->body)->loc,_tag_arr("function may not return a value",sizeof(unsigned char),
32),_tag_arr(_tmp3AC,sizeof(void*),0));});goto _LL4C0;_LL4C0:;}goto _LL4BB;}_LL4BB:;}
Cyc_NewControlFlow_postpass(fd->body);}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*
ds){for(0;ds != 0;ds=ds->tl){void*_tmp3B0=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;
struct Cyc_Absyn_Fndecl*_tmp3B1;struct Cyc_List_List*_tmp3B2;struct Cyc_List_List*
_tmp3B3;struct Cyc_List_List*_tmp3B4;_LL4CC: if(*((int*)_tmp3B0)== 1){_LL4D6:
_tmp3B1=((struct Cyc_Absyn_Fn_d_struct*)_tmp3B0)->f1;goto _LL4CD;}else{goto _LL4CE;}
_LL4CE: if(*((int*)_tmp3B0)== 11){_LL4D7: _tmp3B2=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp3B0)->f1;goto _LL4CF;}else{goto _LL4D0;}_LL4D0: if(*((int*)_tmp3B0)== 10){
_LL4D8: _tmp3B3=((struct Cyc_Absyn_Using_d_struct*)_tmp3B0)->f2;goto _LL4D1;}else{
goto _LL4D2;}_LL4D2: if(*((int*)_tmp3B0)== 9){_LL4D9: _tmp3B4=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3B0)->f2;goto _LL4D3;}else{goto _LL4D4;}_LL4D4: goto _LL4D5;_LL4CD: Cyc_NewControlFlow_check_fun(
_tmp3B1);goto _LL4CB;_LL4CF: _tmp3B3=_tmp3B2;goto _LL4D1;_LL4D1: _tmp3B4=_tmp3B3;
goto _LL4D3;_LL4D3: Cyc_NewControlFlow_cf_check(_tmp3B4);goto _LL4CB;_LL4D5: goto
_LL4CB;_LL4CB:;}}
