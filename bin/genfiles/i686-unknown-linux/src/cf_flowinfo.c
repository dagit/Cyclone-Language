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
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern void*Cyc_List_hd(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*
x);extern unsigned char Cyc_List_List_mismatch[18];extern unsigned char Cyc_List_Nth[
8];extern int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
extern int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2);struct Cyc_Set_Set;extern struct Cyc_Set_Set*Cyc_Set_empty(int(*
cmp)(void*,void*));extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,
void*elt);extern int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern struct Cyc_List_List*
Cyc_Set_elements(struct Cyc_Set_Set*s);extern unsigned char Cyc_Set_Absent[11];
struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern unsigned char
Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));extern struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,
void*v);extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);extern void*Cyc_Dict_fold_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d,void*accum);
extern void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*
d);extern struct Cyc_Dict_Dict*Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,
struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*Cyc_Dict_union_two_c(void*(*f)(
void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);
extern struct Cyc_Dict_Dict*Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),
void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);extern int Cyc_Dict_forall_c(
int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);extern int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);extern
int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
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
15];extern struct _tagged_arr*Cyc_Absyn_fieldname(int);struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE;extern struct Cyc_Std___sFILE*Cyc_Std_stderr;extern
unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_Tcenv_VarRes_struct{
int tag;void*f1;};struct Cyc_Tcenv_StructRes_struct{int tag;struct Cyc_Absyn_Structdecl*
f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;
struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*structdecls;struct Cyc_Dict_Dict*uniondecls;struct
Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*
typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;};struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{int tag;void*f1;void*f2;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*
le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void Cyc_Tcutil_terr(struct
Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_compress(
void*t);extern int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern struct
_tagged_arr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct
Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;void*rop;};
unsigned char Cyc_CfFlowInfo_IsZero[11]="\000\000\000\000IsZero";struct Cyc_CfFlowInfo_IsZero_struct{
unsigned char*tag;struct Cyc_List_List*f1;};unsigned char Cyc_CfFlowInfo_NotZero[
12]="\000\000\000\000NotZero";struct Cyc_CfFlowInfo_NotZero_struct{unsigned char*
tag;struct Cyc_List_List*f1;};unsigned char Cyc_CfFlowInfo_UnknownZ[13]="\000\000\000\000UnknownZ";
struct Cyc_CfFlowInfo_UnknownZ_struct{unsigned char*tag;struct Cyc_List_List*f1;};
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct
Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_Dict_Dict*f1;};
struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict*f1;struct Cyc_List_List*
f2;};extern struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();extern void*Cyc_CfFlowInfo_unknown_none;
extern void*Cyc_CfFlowInfo_unknown_this;extern void*Cyc_CfFlowInfo_unknown_all;
extern int Cyc_CfFlowInfo_root_cmp(void*,void*);extern int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);extern void*Cyc_CfFlowInfo_typ_to_absrval(
void*t,void*leafval);extern void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,
void*r);extern void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place);extern int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place);extern int Cyc_CfFlowInfo_flow_lessthan_approx(void*f1,void*f2);extern
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,struct
Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);extern struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);extern struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
extern struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);extern void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);
extern struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(struct Cyc_Dict_Dict*d,
struct Cyc_Set_Set**all_changed,void*r);extern struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);extern void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**,void*,void*);extern void*Cyc_CfFlowInfo_after_flow(struct Cyc_Set_Set**,
void*,void*,struct Cyc_Set_Set*,struct Cyc_Set_Set*);static struct Cyc_CfFlowInfo_UnknownR_struct
Cyc_CfFlowInfo_unknown_none_v={0,(void*)((void*)0)};static struct Cyc_CfFlowInfo_UnknownR_struct
Cyc_CfFlowInfo_unknown_this_v={0,(void*)((void*)1)};static struct Cyc_CfFlowInfo_UnknownR_struct
Cyc_CfFlowInfo_unknown_all_v={0,(void*)((void*)2)};static struct Cyc_CfFlowInfo_Esc_struct
Cyc_CfFlowInfo_esc_none_v={1,(void*)((void*)0)};static struct Cyc_CfFlowInfo_Esc_struct
Cyc_CfFlowInfo_esc_this_v={1,(void*)((void*)1)};static struct Cyc_CfFlowInfo_Esc_struct
Cyc_CfFlowInfo_esc_all_v={1,(void*)((void*)2)};void*Cyc_CfFlowInfo_unknown_none=(
void*)& Cyc_CfFlowInfo_unknown_none_v;void*Cyc_CfFlowInfo_unknown_this=(void*)&
Cyc_CfFlowInfo_unknown_this_v;void*Cyc_CfFlowInfo_unknown_all=(void*)& Cyc_CfFlowInfo_unknown_all_v;
void*Cyc_CfFlowInfo_esc_none=(void*)& Cyc_CfFlowInfo_esc_none_v;void*Cyc_CfFlowInfo_esc_this=(
void*)& Cyc_CfFlowInfo_esc_this_v;void*Cyc_CfFlowInfo_esc_all=(void*)& Cyc_CfFlowInfo_esc_all_v;
static struct Cyc_Set_Set**Cyc_CfFlowInfo_mt_place_set_opt=0;struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set(){
if(Cyc_CfFlowInfo_mt_place_set_opt == 0){Cyc_CfFlowInfo_mt_place_set_opt=({struct
Cyc_Set_Set**_tmp6=_cycalloc(sizeof(struct Cyc_Set_Set*));_tmp6[0]=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))Cyc_Set_empty)(
Cyc_CfFlowInfo_place_cmp);_tmp6;});}return*((struct Cyc_Set_Set**)_check_null(Cyc_CfFlowInfo_mt_place_set_opt));}
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2){return 0;}{int
p1;int p2;{void*_tmp7=r1;struct Cyc_Absyn_Vardecl*_tmp8;struct Cyc_Absyn_Exp*_tmp9;
_LL1: if(*((int*)_tmp7)== 0){_LL5: _tmp8=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp7)->f1;goto _LL2;}else{goto _LL3;}_LL3: if(*((int*)_tmp7)== 1){_LL6: _tmp9=((
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp7)->f1;goto _LL4;}else{goto _LL0;}_LL2:
p1=(int)_tmp8;goto _LL0;_LL4: p1=(int)_tmp9;_LL0:;}{void*_tmpA=r2;struct Cyc_Absyn_Vardecl*
_tmpB;struct Cyc_Absyn_Exp*_tmpC;_LL8: if(*((int*)_tmpA)== 0){_LLC: _tmpB=((struct
Cyc_CfFlowInfo_VarRoot_struct*)_tmpA)->f1;goto _LL9;}else{goto _LLA;}_LLA: if(*((
int*)_tmpA)== 1){_LLD: _tmpC=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmpA)->f1;
goto _LLB;}else{goto _LL7;}_LL9: p2=(int)_tmpB;goto _LL7;_LLB: p2=(int)_tmpC;_LL7:;}
return p1 - p2;}}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct
Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)p2){return 0;}{int i=Cyc_CfFlowInfo_root_cmp((
void*)p1->root,(void*)p2->root);if(i != 0){return i;}return((int(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Std_zstrptrcmp,p1->fields,p2->fields);}}static struct
_tagged_arr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){struct Cyc_List_List*
sl=0;{void*_tmpD=(void*)p->root;struct Cyc_Absyn_Vardecl*_tmpE;struct Cyc_Absyn_Exp*
_tmpF;_LLF: if(*((int*)_tmpD)== 0){_LL13: _tmpE=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmpD)->f1;goto _LL10;}else{goto _LL11;}_LL11: if(*((int*)_tmpD)== 1){_LL14: _tmpF=((
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmpD)->f1;goto _LL12;}else{goto _LLE;}_LL10:
sl=({struct Cyc_List_List*_tmp10=_cycalloc(sizeof(struct Cyc_List_List));_tmp10->hd=({
struct _tagged_arr*_tmp11=_cycalloc(sizeof(struct _tagged_arr));_tmp11[0]=({struct
Cyc_Std_String_pa_struct _tmp13;_tmp13.tag=0;_tmp13.f1=(struct _tagged_arr)*(*
_tmpE->name).f2;{void*_tmp12[1]={& _tmp13};Cyc_Std_aprintf(_tag_arr("%s",sizeof(
unsigned char),3),_tag_arr(_tmp12,sizeof(void*),1));}});_tmp11;});_tmp10->tl=sl;
_tmp10;});goto _LLE;_LL12: sl=({struct Cyc_List_List*_tmp14=_cycalloc(sizeof(struct
Cyc_List_List));_tmp14->hd=({struct _tagged_arr*_tmp15=_cycalloc(sizeof(struct
_tagged_arr));_tmp15[0]=({struct Cyc_Std_Int_pa_struct _tmp17;_tmp17.tag=1;_tmp17.f1=(
int)((unsigned int)((int)_tmpF));{void*_tmp16[1]={& _tmp17};Cyc_Std_aprintf(
_tag_arr("mpt%d",sizeof(unsigned char),6),_tag_arr(_tmp16,sizeof(void*),1));}});
_tmp15;});_tmp14->tl=sl;_tmp14;});_LLE:;}{struct Cyc_List_List*fields=p->fields;
for(0;fields != 0;fields=fields->tl){sl=({struct Cyc_List_List*_tmp18=_cycalloc(
sizeof(struct Cyc_List_List));_tmp18->hd=({struct _tagged_arr*_tmp19=_cycalloc(
sizeof(struct _tagged_arr));_tmp19[0]=({struct Cyc_Std_String_pa_struct _tmp1B;
_tmp1B.tag=0;_tmp1B.f1=(struct _tagged_arr)*((struct _tagged_arr*)fields->hd);{
void*_tmp1A[1]={& _tmp1B};Cyc_Std_aprintf(_tag_arr("%s",sizeof(unsigned char),3),
_tag_arr(_tmp1A,sizeof(void*),1));}});_tmp19;});_tmp18->tl=sl;_tmp18;});}}{
struct _tagged_arr*_tmp1C=({struct _tagged_arr*_tmp20=_cycalloc(sizeof(struct
_tagged_arr));_tmp20[0]=({struct Cyc_Std_String_pa_struct _tmp22;_tmp22.tag=0;
_tmp22.f1=_tag_arr("",sizeof(unsigned char),1);{void*_tmp21[1]={& _tmp22};Cyc_Std_aprintf(
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp21,sizeof(void*),1));}});
_tmp20;});for(0;sl != 0;sl=sl->tl){*_tmp1C=({struct Cyc_Std_String_pa_struct _tmp1F;
_tmp1F.tag=0;_tmp1F.f1=(struct _tagged_arr)*_tmp1C;{struct Cyc_Std_String_pa_struct
_tmp1E;_tmp1E.tag=0;_tmp1E.f1=(struct _tagged_arr)*((struct _tagged_arr*)sl->hd);{
void*_tmp1D[2]={& _tmp1E,& _tmp1F};Cyc_Std_aprintf(_tag_arr("%s.%s",sizeof(
unsigned char),6),_tag_arr(_tmp1D,sizeof(void*),2));}}});}return _tmp1C;}}struct
_tuple3{struct Cyc_Absyn_Tqual f1;void*f2;};void*Cyc_CfFlowInfo_typ_to_absrval(
void*t,void*leafval){void*_tmp23=Cyc_Tcutil_compress(t);struct Cyc_Absyn_TunionFieldInfo
_tmp24;void*_tmp25;struct Cyc_Absyn_Tunionfield*_tmp26;struct Cyc_List_List*_tmp27;
struct Cyc_List_List*_tmp28;struct Cyc_Absyn_Structdecl**_tmp29;struct Cyc_Absyn_Structdecl*
_tmp2A;struct Cyc_Absyn_Structdecl _tmp2B;struct Cyc_Core_Opt*_tmp2C;struct Cyc_Core_Opt
_tmp2D;struct Cyc_List_List*_tmp2E;_LL16: if((unsigned int)_tmp23 > 3?*((int*)
_tmp23)== 3: 0){_LL20: _tmp24=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp23)->f1;
_LL21: _tmp25=(void*)_tmp24.field_info;if(*((int*)_tmp25)== 1){_LL22: _tmp26=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp25)->f2;goto _LL17;}else{goto _LL18;}}
else{goto _LL18;}_LL18: if((unsigned int)_tmp23 > 3?*((int*)_tmp23)== 9: 0){_LL23:
_tmp27=((struct Cyc_Absyn_TupleType_struct*)_tmp23)->f1;goto _LL19;}else{goto _LL1A;}
_LL1A: if((unsigned int)_tmp23 > 3?*((int*)_tmp23)== 14: 0){_LL24: _tmp28=((struct
Cyc_Absyn_AnonStructType_struct*)_tmp23)->f1;goto _LL1B;}else{goto _LL1C;}_LL1C:
if((unsigned int)_tmp23 > 3?*((int*)_tmp23)== 10: 0){_LL25: _tmp29=((struct Cyc_Absyn_StructType_struct*)
_tmp23)->f3;if(_tmp29 == 0){goto _LL1E;}else{_tmp2A=*_tmp29;_tmp2B=*_tmp2A;_LL26:
_tmp2C=_tmp2B.fields;if(_tmp2C == 0){goto _LL1E;}else{_tmp2D=*_tmp2C;_LL27: _tmp2E=(
struct Cyc_List_List*)_tmp2D.v;goto _LL1D;}}}else{goto _LL1E;}_LL1E: goto _LL1F;_LL17:
if(_tmp26->typs == 0){return leafval;}_tmp27=_tmp26->typs;goto _LL19;_LL19: {struct
Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);{int i=0;for(0;_tmp27 != 0;(
_tmp27=_tmp27->tl,++ i)){d=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,void*v))Cyc_Dict_insert)(d,Cyc_Absyn_fieldname(i),Cyc_CfFlowInfo_typ_to_absrval((*((
struct _tuple3*)_tmp27->hd)).f2,leafval));}}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2F=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));_tmp2F[0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _tmp30;_tmp30.tag=3;_tmp30.f1=d;_tmp30;});
_tmp2F;});}_LL1B: _tmp2E=_tmp28;goto _LL1D;_LL1D: {struct Cyc_Dict_Dict*d=((struct
Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);for(0;_tmp2E != 0;_tmp2E=_tmp2E->tl){struct Cyc_Absyn_Structfield
_tmp32;void*_tmp33;struct _tagged_arr*_tmp34;struct Cyc_Absyn_Structfield*_tmp31=(
struct Cyc_Absyn_Structfield*)_tmp2E->hd;_tmp32=*_tmp31;_LL2A: _tmp34=_tmp32.name;
goto _LL29;_LL29: _tmp33=(void*)_tmp32.type;goto _LL28;_LL28: if(_get_arr_size(*
_tmp34,sizeof(unsigned char))!= 1){d=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(d,_tmp34,Cyc_CfFlowInfo_typ_to_absrval(
_tmp33,leafval));}}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp35=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));_tmp35[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp36;_tmp36.tag=3;_tmp36.f1=d;_tmp36;});_tmp35;});}_LL1F: return Cyc_Tcutil_bits_only(
t)? Cyc_CfFlowInfo_unknown_all: leafval;_LL15:;}static int Cyc_CfFlowInfo_prefix_of_member(
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Set_Set*set){{struct Cyc_List_List*
_tmp37=((struct Cyc_List_List*(*)(struct Cyc_Set_Set*s))Cyc_Set_elements)(set);
for(0;_tmp37 != 0;_tmp37=_tmp37->tl){struct Cyc_CfFlowInfo_Place*_tmp38=(struct Cyc_CfFlowInfo_Place*)
_tmp37->hd;if(Cyc_CfFlowInfo_root_cmp((void*)place->root,(void*)_tmp38->root)!= 
0){continue;}{struct Cyc_List_List*_tmp39=place->fields;struct Cyc_List_List*
_tmp3A=_tmp38->fields;for(0;_tmp39 != 0? _tmp3A != 0: 0;(_tmp39=_tmp39->tl,_tmp3A=
_tmp3A->tl)){if(Cyc_Std_zstrptrcmp((struct _tagged_arr*)_tmp39->hd,(struct
_tagged_arr*)_tmp3A->hd)!= 0){break;}}if(_tmp39 == 0){return 1;}}}}return 0;}struct
Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*places;};
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*
place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,
pile->places,place)){pile->places=({struct Cyc_List_List*_tmp3B=_region_malloc(
pile->rgn,sizeof(struct Cyc_List_List));_tmp3B->hd=place;_tmp3B->tl=pile->places;
_tmp3B;});}}static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*
pile,void*a,void*r){void*_tmp3C=r;struct Cyc_CfFlowInfo_Place*_tmp3D;struct Cyc_Dict_Dict*
_tmp3E;_LL2C: if((unsigned int)_tmp3C > 3?*((int*)_tmp3C)== 2: 0){_LL32: _tmp3D=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3C)->f1;goto _LL2D;}else{goto _LL2E;}
_LL2E: if((unsigned int)_tmp3C > 3?*((int*)_tmp3C)== 3: 0){_LL33: _tmp3E=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp3C)->f1;goto _LL2F;}else{goto _LL30;}_LL30: goto _LL31;_LL2D: Cyc_CfFlowInfo_add_place(
pile,_tmp3D);return;_LL2F:((void(*)(void(*f)(struct Cyc_CfFlowInfo_EscPile*,
struct _tagged_arr*,void*),struct Cyc_CfFlowInfo_EscPile*env,struct Cyc_Dict_Dict*d))
Cyc_Dict_iter_c)((void(*)(struct Cyc_CfFlowInfo_EscPile*pile,struct _tagged_arr*a,
void*r))Cyc_CfFlowInfo_add_places,pile,_tmp3E);return;_LL31: return;_LL2B:;}
static void*Cyc_CfFlowInfo_insert_place_inner(void*new_val,void*old_val){void*
_tmp3F=old_val;struct Cyc_Dict_Dict*_tmp40;_LL35: if((unsigned int)_tmp3F > 3?*((
int*)_tmp3F)== 3: 0){_LL39: _tmp40=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp3F)->f1;
goto _LL36;}else{goto _LL37;}_LL37: goto _LL38;_LL36: return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp41=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));_tmp41[0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _tmp42;_tmp42.tag=3;_tmp42.f1=((struct Cyc_Dict_Dict*(*)(
void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict*d))Cyc_Dict_map_c)(Cyc_CfFlowInfo_insert_place_inner,
new_val,_tmp40);_tmp42;});_tmp41;});_LL38: return new_val;_LL34:;}struct _tuple4{
struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 0){return Cyc_CfFlowInfo_insert_place_inner(
new_val,old_val);}{struct _tuple4 _tmp44=({struct _tuple4 _tmp43;_tmp43.f1=fs;_tmp43.f2=
old_val;_tmp43;});void*_tmp45;struct Cyc_Dict_Dict*_tmp46;struct Cyc_List_List*
_tmp47;struct Cyc_List_List _tmp48;struct Cyc_List_List*_tmp49;struct _tagged_arr*
_tmp4A;_LL3B: _LL41: _tmp47=_tmp44.f1;if(_tmp47 == 0){goto _LL3D;}else{_tmp48=*
_tmp47;_LL43: _tmp4A=(struct _tagged_arr*)_tmp48.hd;goto _LL42;_LL42: _tmp49=_tmp48.tl;
goto _LL3F;}_LL3F: _tmp45=_tmp44.f2;if((unsigned int)_tmp45 > 3?*((int*)_tmp45)== 3:
0){_LL40: _tmp46=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp45)->f1;goto _LL3C;}
else{goto _LL3D;}_LL3D: goto _LL3E;_LL3C: {void*_tmp4B=Cyc_CfFlowInfo_insert_place_outer(
_tmp49,((void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp46,_tmp4A),new_val);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp4C=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));_tmp4C[0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _tmp4D;_tmp4D.tag=3;_tmp4D.f1=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(_tmp46,_tmp4A,
_tmp4B);_tmp4D;});_tmp4C;});}_LL3E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp4E=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp4E[0]=({struct Cyc_Core_Impossible_struct
_tmp4F;_tmp4F.tag=Cyc_Core_Impossible;_tmp4F.f1=_tag_arr("bad insert place",
sizeof(unsigned char),17);_tmp4F;});_tmp4E;}));_LL3A:;}}static struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Set_Set**
all_changed,struct Cyc_Dict_Dict*d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*
_tmp50=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;if(
all_changed != 0){*all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*all_changed,_tmp50);}{void*oldval;
void*newval;{struct _handler_cons _tmp51;_push_handler(& _tmp51);{int _tmp53=0;if(
setjmp(_tmp51.handler)){_tmp53=1;}if(! _tmp53){oldval=Cyc_CfFlowInfo_lookup_place(
d,_tmp50);;_pop_handler();}else{void*_tmp52=(void*)_exn_thrown;void*_tmp55=
_tmp52;_LL45: if(_tmp55 == Cyc_Dict_Absent){goto _LL46;}else{goto _LL47;}_LL47: goto
_LL48;_LL46: continue;_LL48:(void)_throw(_tmp55);_LL44:;}}}{void*_tmp56=Cyc_CfFlowInfo_initlevel(
d,oldval);_LL4A: if((int)_tmp56 == 2){goto _LL4B;}else{goto _LL4C;}_LL4C: if((int)
_tmp56 == 1){goto _LL4D;}else{goto _LL4E;}_LL4E: if((int)_tmp56 == 0){goto _LL4F;}
else{goto _LL49;}_LL4B: newval=Cyc_CfFlowInfo_esc_all;goto _LL49;_LL4D: newval=Cyc_CfFlowInfo_esc_this;
goto _LL49;_LL4F: newval=Cyc_CfFlowInfo_esc_none;goto _LL49;_LL49:;}((void(*)(
struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(pile,0,
oldval);d=Cyc_Dict_insert(d,(void*)_tmp50->root,Cyc_CfFlowInfo_insert_place_outer(
_tmp50->fields,Cyc_Dict_lookup(d,(void*)_tmp50->root),newval));}}return d;}struct
Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict*d;struct Cyc_List_List*seen;};
static void*Cyc_CfFlowInfo_initlevel_approx(void*r){void*_tmp57=r;void*_tmp58;
void*_tmp59;_LL51: if((unsigned int)_tmp57 > 3?*((int*)_tmp57)== 0: 0){_LL5D: _tmp58=(
void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp57)->f1;goto _LL52;}else{goto
_LL53;}_LL53: if((unsigned int)_tmp57 > 3?*((int*)_tmp57)== 1: 0){_LL5E: _tmp59=(
void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp57)->f1;goto _LL54;}else{goto _LL55;}
_LL55: if((int)_tmp57 == 0){goto _LL56;}else{goto _LL57;}_LL57: if((int)_tmp57 == 1){
goto _LL58;}else{goto _LL59;}_LL59: if((int)_tmp57 == 2){goto _LL5A;}else{goto _LL5B;}
_LL5B: goto _LL5C;_LL52: return _tmp58;_LL54: return _tmp59;_LL56: goto _LL58;_LL58:
return(void*)2;_LL5A: return(void*)1;_LL5C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp5A=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp5A[0]=({struct Cyc_Core_Impossible_struct
_tmp5B;_tmp5B.tag=Cyc_Core_Impossible;_tmp5B.f1=_tag_arr("initlevel_approx",
sizeof(unsigned char),17);_tmp5B;});_tmp5A;}));_LL50:;}static void*Cyc_CfFlowInfo_initlevel_rec(
struct Cyc_CfFlowInfo_InitlevelEnv*env,void*a,void*r,void*acc){void*this_ans;if(
acc == (void*)0){return(void*)0;}{void*_tmp5C=r;struct Cyc_Dict_Dict*_tmp5D;struct
Cyc_CfFlowInfo_Place*_tmp5E;_LL60: if((unsigned int)_tmp5C > 3?*((int*)_tmp5C)== 3:
0){_LL66: _tmp5D=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp5C)->f1;goto _LL61;}
else{goto _LL62;}_LL62: if((unsigned int)_tmp5C > 3?*((int*)_tmp5C)== 2: 0){_LL67:
_tmp5E=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp5C)->f1;goto _LL63;}else{goto
_LL64;}_LL64: goto _LL65;_LL61: this_ans=((void*(*)(void*(*f)(struct Cyc_CfFlowInfo_InitlevelEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_InitlevelEnv*env,struct Cyc_Dict_Dict*
d,void*accum))Cyc_Dict_fold_c)((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,
struct _tagged_arr*a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec,env,_tmp5D,(
void*)2);goto _LL5F;_LL63: if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmp5E)){this_ans=(void*)2;}
else{env->seen=({struct Cyc_List_List*_tmp5F=_cycalloc(sizeof(struct Cyc_List_List));
_tmp5F->hd=_tmp5E;_tmp5F->tl=env->seen;_tmp5F;});this_ans=((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*
env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(env,0,Cyc_CfFlowInfo_lookup_place(
env->d,_tmp5E),(void*)2);env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;
if(this_ans == (void*)0){this_ans=(void*)1;}}goto _LL5F;_LL65: this_ans=Cyc_CfFlowInfo_initlevel_approx(
r);_LL5F:;}if(this_ans == (void*)0){return(void*)0;}if(this_ans == (void*)1? 1: acc
== (void*)1){return(void*)1;}return(void*)2;}void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict*d,void*r){struct Cyc_CfFlowInfo_InitlevelEnv _tmp60=({struct
Cyc_CfFlowInfo_InitlevelEnv _tmp61;_tmp61.d=d;_tmp61.seen=0;_tmp61;});return((
void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(&
_tmp60,0,r,(void*)2);}void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,
struct Cyc_CfFlowInfo_Place*place){struct Cyc_CfFlowInfo_Place _tmp63;struct Cyc_List_List*
_tmp64;void*_tmp65;struct Cyc_CfFlowInfo_Place*_tmp62=place;_tmp63=*_tmp62;_LL6A:
_tmp65=(void*)_tmp63.root;goto _LL69;_LL69: _tmp64=_tmp63.fields;goto _LL68;_LL68: {
void*_tmp66=Cyc_Dict_lookup(d,_tmp65);for(0;_tmp64 != 0;_tmp64=_tmp64->tl){struct
_tuple0 _tmp68=({struct _tuple0 _tmp67;_tmp67.f1=_tmp66;_tmp67.f2=(struct
_tagged_arr*)_tmp64->hd;_tmp67;});struct _tagged_arr*_tmp69;void*_tmp6A;struct Cyc_Dict_Dict*
_tmp6B;_LL6C: _LL71: _tmp6A=_tmp68.f1;if((unsigned int)_tmp6A > 3?*((int*)_tmp6A)== 
3: 0){_LL72: _tmp6B=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp6A)->f1;goto _LL70;}
else{goto _LL6E;}_LL70: _tmp69=_tmp68.f2;goto _LL6D;_LL6E: goto _LL6F;_LL6D: _tmp66=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp6B,
_tmp69);goto _LL6B;_LL6F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp6C=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp6C[0]=({struct Cyc_Core_Impossible_struct
_tmp6D;_tmp6D.tag=Cyc_Core_Impossible;_tmp6D.f1=_tag_arr("bad lookup_place",
sizeof(unsigned char),17);_tmp6D;});_tmp6C;}));_LL6B:;}return _tmp66;}}static int
Cyc_CfFlowInfo_is_rval_unescaped(void*a,void*b,void*rval){void*_tmp6E=rval;
struct Cyc_Dict_Dict*_tmp6F;_LL74: if((unsigned int)_tmp6E > 3?*((int*)_tmp6E)== 1:
0){goto _LL75;}else{goto _LL76;}_LL76: if((unsigned int)_tmp6E > 3?*((int*)_tmp6E)== 
3: 0){_LL7A: _tmp6F=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp6E)->f1;goto _LL77;}
else{goto _LL78;}_LL78: goto _LL79;_LL75: return 0;_LL77: return((int(*)(int(*f)(int,
struct _tagged_arr*,void*),int env,struct Cyc_Dict_Dict*d))Cyc_Dict_forall_c)((int(*)(
int a,struct _tagged_arr*b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped,0,_tmp6F);
_LL79: return 1;_LL73:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*d,
struct Cyc_CfFlowInfo_Place*place){return((int(*)(int a,int b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped)(
0,0,Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,void*r){struct _RegionHandle
_tmp70=_new_region("rgn");struct _RegionHandle*rgn=& _tmp70;_push_region(rgn);{
struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmp72=
_region_malloc(rgn,sizeof(struct Cyc_CfFlowInfo_EscPile));_tmp72->rgn=rgn;_tmp72->places=
0;_tmp72;});((void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,r);{struct Cyc_Dict_Dict*_tmp71=Cyc_CfFlowInfo_escape_these(pile,
all_changed,d);_npop_handler(0);return _tmp71;}};_pop_region(rgn);}struct Cyc_CfFlowInfo_AssignEnv{
struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict*d;struct Cyc_Position_Segment*
loc;};struct _tuple5{void*f1;void*f2;};static void*Cyc_CfFlowInfo_assign_place_inner(
struct Cyc_CfFlowInfo_AssignEnv*env,void*a,void*oldval,void*newval){struct _tuple5
_tmp74=({struct _tuple5 _tmp73;_tmp73.f1=oldval;_tmp73.f2=newval;_tmp73;});void*
_tmp75;struct Cyc_CfFlowInfo_Place*_tmp76;void*_tmp77;void*_tmp78;void*_tmp79;
struct Cyc_Dict_Dict*_tmp7A;void*_tmp7B;struct Cyc_Dict_Dict*_tmp7C;void*_tmp7D;
void*_tmp7E;_LL7C: _LL88: _tmp77=_tmp74.f1;if((unsigned int)_tmp77 > 3?*((int*)
_tmp77)== 1: 0){goto _LL86;}else{goto _LL7E;}_LL86: _tmp75=_tmp74.f2;if((
unsigned int)_tmp75 > 3?*((int*)_tmp75)== 2: 0){_LL87: _tmp76=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp75)->f1;goto _LL7D;}else{goto _LL7E;}_LL7E: _LL89: _tmp78=_tmp74.f1;if((
unsigned int)_tmp78 > 3?*((int*)_tmp78)== 1: 0){goto _LL7F;}else{goto _LL80;}_LL80:
_LL8C: _tmp7B=_tmp74.f1;if((unsigned int)_tmp7B > 3?*((int*)_tmp7B)== 3: 0){_LL8D:
_tmp7C=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp7B)->f1;goto _LL8A;}else{goto
_LL82;}_LL8A: _tmp79=_tmp74.f2;if((unsigned int)_tmp79 > 3?*((int*)_tmp79)== 3: 0){
_LL8B: _tmp7A=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp79)->f1;goto _LL81;}
else{goto _LL82;}_LL82: _LL8E: _tmp7D=_tmp74.f2;if((unsigned int)_tmp7D > 3?*((int*)
_tmp7D)== 1: 0){_LL8F: _tmp7E=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp7D)->f1;
goto _LL83;}else{goto _LL84;}_LL84: goto _LL85;_LL7D: Cyc_CfFlowInfo_add_place(env->pile,
_tmp76);goto _LL7F;_LL7F: if(Cyc_CfFlowInfo_initlevel(env->d,newval)!= (void*)2){({
void*_tmp7F[0]={};Cyc_Tcutil_terr(env->loc,_tag_arr("assignment puts possibly-uninitialized data in an escaped location",
sizeof(unsigned char),67),_tag_arr(_tmp7F,sizeof(void*),0));});}return Cyc_CfFlowInfo_esc_all;
_LL81: {struct Cyc_Dict_Dict*_tmp80=((struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_AssignEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(struct Cyc_CfFlowInfo_AssignEnv*
env,struct _tagged_arr*a,void*oldval,void*newval))Cyc_CfFlowInfo_assign_place_inner,
env,_tmp7C,_tmp7A);if(_tmp80 == _tmp7C){return oldval;}if(_tmp80 == _tmp7A){return
newval;}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp81=_cycalloc(
sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));_tmp81[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp82;_tmp82.tag=3;_tmp82.f1=_tmp80;_tmp82;});_tmp81;});}_LL83: {void*_tmp83=
_tmp7E;_LL91: if((int)_tmp83 == 0){goto _LL92;}else{goto _LL93;}_LL93: if((int)_tmp83
== 1){goto _LL94;}else{goto _LL95;}_LL95: if((int)_tmp83 == 2){goto _LL96;}else{goto
_LL90;}_LL92: return Cyc_CfFlowInfo_unknown_none;_LL94: return Cyc_CfFlowInfo_unknown_this;
_LL96: return Cyc_CfFlowInfo_unknown_all;_LL90:;}_LL85: return newval;_LL7B:;}static
void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct
Cyc_List_List*fs,void*oldval,void*newval){if(fs == 0){return((void*(*)(struct Cyc_CfFlowInfo_AssignEnv*
env,int a,void*oldval,void*newval))Cyc_CfFlowInfo_assign_place_inner)(env,0,
oldval,newval);}{struct _tuple4 _tmp85=({struct _tuple4 _tmp84;_tmp84.f1=fs;_tmp84.f2=
oldval;_tmp84;});void*_tmp86;struct Cyc_Dict_Dict*_tmp87;struct Cyc_List_List*
_tmp88;struct Cyc_List_List _tmp89;struct Cyc_List_List*_tmp8A;struct _tagged_arr*
_tmp8B;_LL98: _LL9E: _tmp88=_tmp85.f1;if(_tmp88 == 0){goto _LL9A;}else{_tmp89=*
_tmp88;_LLA0: _tmp8B=(struct _tagged_arr*)_tmp89.hd;goto _LL9F;_LL9F: _tmp8A=_tmp89.tl;
goto _LL9C;}_LL9C: _tmp86=_tmp85.f2;if((unsigned int)_tmp86 > 3?*((int*)_tmp86)== 3:
0){_LL9D: _tmp87=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp86)->f1;goto _LL99;}
else{goto _LL9A;}_LL9A: goto _LL9B;_LL99: {void*_tmp8C=Cyc_CfFlowInfo_assign_place_outer(
env,_tmp8A,((void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp87,_tmp8B),newval);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp8D=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));_tmp8D[0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _tmp8E;_tmp8E.tag=3;_tmp8E.f1=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(_tmp87,_tmp8B,
_tmp8C);_tmp8E;});_tmp8D;});}_LL9B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp8F=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp8F[0]=({struct Cyc_Core_Impossible_struct
_tmp90;_tmp90.tag=Cyc_Core_Impossible;_tmp90.f1=_tag_arr("bad insert place",
sizeof(unsigned char),17);_tmp90;});_tmp8F;}));_LL97:;}}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){if(all_changed != 0){*
all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*
elt))Cyc_Set_insert)(*all_changed,place);}{struct _RegionHandle _tmp91=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp91;_push_region(rgn);{struct Cyc_CfFlowInfo_Place
_tmp93;struct Cyc_List_List*_tmp94;void*_tmp95;struct Cyc_CfFlowInfo_Place*_tmp92=
place;_tmp93=*_tmp92;_LLA3: _tmp95=(void*)_tmp93.root;goto _LLA2;_LLA2: _tmp94=
_tmp93.fields;goto _LLA1;_LLA1: {struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv
_tmp97;_tmp97.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp98=_region_malloc(rgn,
sizeof(struct Cyc_CfFlowInfo_EscPile));_tmp98->rgn=rgn;_tmp98->places=0;_tmp98;});
_tmp97.d=d;_tmp97.loc=loc;_tmp97;});void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp94,Cyc_Dict_lookup(d,_tmp95),r);struct Cyc_Dict_Dict*_tmp96=Cyc_CfFlowInfo_escape_these(
env.pile,all_changed,Cyc_Dict_insert(d,_tmp95,newval));_npop_handler(0);return
_tmp96;}};_pop_region(rgn);}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict*d1;struct Cyc_Dict_Dict*d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Set_Set*
chg1;struct Cyc_Set_Set*chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static void*Cyc_CfFlowInfo_join_absRval(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){if(r1 == r2){return r1;}{
struct _tuple5 _tmp9A=({struct _tuple5 _tmp99;_tmp99.f1=r1;_tmp99.f2=r2;_tmp99;});
void*_tmp9B;struct Cyc_CfFlowInfo_Place*_tmp9C;void*_tmp9D;struct Cyc_CfFlowInfo_Place*
_tmp9E;void*_tmp9F;struct Cyc_CfFlowInfo_Place*_tmpA0;void*_tmpA1;struct Cyc_CfFlowInfo_Place*
_tmpA2;void*_tmpA3;void*_tmpA4;void*_tmpA5;void*_tmpA6;void*_tmpA7;struct Cyc_Dict_Dict*
_tmpA8;void*_tmpA9;struct Cyc_Dict_Dict*_tmpAA;_LLA5: _LLB5: _tmp9D=_tmp9A.f1;if((
unsigned int)_tmp9D > 3?*((int*)_tmp9D)== 2: 0){_LLB6: _tmp9E=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp9D)->f1;goto _LLB3;}else{goto _LLA7;}_LLB3: _tmp9B=_tmp9A.f2;if((unsigned int)
_tmp9B > 3?*((int*)_tmp9B)== 2: 0){_LLB4: _tmp9C=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp9B)->f1;goto _LLA6;}else{goto _LLA7;}_LLA7: _LLB7: _tmp9F=_tmp9A.f1;if((
unsigned int)_tmp9F > 3?*((int*)_tmp9F)== 2: 0){_LLB8: _tmpA0=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp9F)->f1;goto _LLA8;}else{goto _LLA9;}_LLA9: _LLB9: _tmpA1=_tmp9A.f2;if((
unsigned int)_tmpA1 > 3?*((int*)_tmpA1)== 2: 0){_LLBA: _tmpA2=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpA1)->f1;goto _LLAA;}else{goto _LLAB;}_LLAB: _LLBC: _tmpA4=_tmp9A.f1;if((int)
_tmpA4 == 1){goto _LLBB;}else{goto _LLAD;}_LLBB: _tmpA3=_tmp9A.f2;if((int)_tmpA3 == 2){
goto _LLAC;}else{goto _LLAD;}_LLAD: _LLBE: _tmpA6=_tmp9A.f1;if((int)_tmpA6 == 2){goto
_LLBD;}else{goto _LLAF;}_LLBD: _tmpA5=_tmp9A.f2;if((int)_tmpA5 == 1){goto _LLAE;}
else{goto _LLAF;}_LLAF: _LLC1: _tmpA9=_tmp9A.f1;if((unsigned int)_tmpA9 > 3?*((int*)
_tmpA9)== 3: 0){_LLC2: _tmpAA=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpA9)->f1;
goto _LLBF;}else{goto _LLB1;}_LLBF: _tmpA7=_tmp9A.f2;if((unsigned int)_tmpA7 > 3?*((
int*)_tmpA7)== 3: 0){_LLC0: _tmpA8=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpA7)->f1;
goto _LLB0;}else{goto _LLB1;}_LLB1: goto _LLB2;_LLA6: if(Cyc_CfFlowInfo_place_cmp(
_tmp9E,_tmp9C)== 0){return r1;}Cyc_CfFlowInfo_add_place(env->pile,_tmp9E);Cyc_CfFlowInfo_add_place(
env->pile,_tmp9C);goto _LLA4;_LLA8: Cyc_CfFlowInfo_add_place(env->pile,_tmpA0);
goto _LLA4;_LLAA: Cyc_CfFlowInfo_add_place(env->pile,_tmpA2);goto _LLA4;_LLAC: goto
_LLAE;_LLAE: return(void*)2;_LLB0: {struct Cyc_Dict_Dict*_tmpAB=((struct Cyc_Dict_Dict*(*)(
void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,struct _tagged_arr*,void*,void*),struct
Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((
void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,struct _tagged_arr*a,void*r1,void*r2))
Cyc_CfFlowInfo_join_absRval,env,_tmpAA,_tmpA8);if(_tmpAB == _tmpAA){return r1;}if(
_tmpAB == _tmpA8){return r2;}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpAC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));_tmpAC[0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _tmpAD;_tmpAD.tag=3;_tmpAD.f1=_tmpAB;_tmpAD;});
_tmpAC;});}_LLB2: goto _LLA4;_LLA4:;}{void*il1=Cyc_CfFlowInfo_initlevel(env->d1,r1);
void*il2=Cyc_CfFlowInfo_initlevel(env->d2,r2);struct _tuple5 _tmpAF=({struct
_tuple5 _tmpAE;_tmpAE.f1=r1;_tmpAE.f2=r2;_tmpAE;});void*_tmpB0;void*_tmpB1;_LLC4:
_LLCA: _tmpB0=_tmpAF.f1;if((unsigned int)_tmpB0 > 3?*((int*)_tmpB0)== 1: 0){goto
_LLC5;}else{goto _LLC6;}_LLC6: _LLCB: _tmpB1=_tmpAF.f2;if((unsigned int)_tmpB1 > 3?*((
int*)_tmpB1)== 1: 0){goto _LLC7;}else{goto _LLC8;}_LLC8: goto _LLC9;_LLC5: goto _LLC7;
_LLC7: {struct _tuple5 _tmpB3=({struct _tuple5 _tmpB2;_tmpB2.f1=il1;_tmpB2.f2=il2;
_tmpB2;});void*_tmpB4;void*_tmpB5;void*_tmpB6;void*_tmpB7;_LLCD: _LLD7: _tmpB4=
_tmpB3.f2;if((int)_tmpB4 == 0){goto _LLCE;}else{goto _LLCF;}_LLCF: _LLD8: _tmpB5=
_tmpB3.f1;if((int)_tmpB5 == 0){goto _LLD0;}else{goto _LLD1;}_LLD1: _LLD9: _tmpB6=
_tmpB3.f2;if((int)_tmpB6 == 1){goto _LLD2;}else{goto _LLD3;}_LLD3: _LLDA: _tmpB7=
_tmpB3.f1;if((int)_tmpB7 == 1){goto _LLD4;}else{goto _LLD5;}_LLD5: goto _LLD6;_LLCE:
goto _LLD0;_LLD0: return Cyc_CfFlowInfo_esc_none;_LLD2: goto _LLD4;_LLD4: return Cyc_CfFlowInfo_esc_this;
_LLD6: return Cyc_CfFlowInfo_esc_all;_LLCC:;}_LLC9: {struct _tuple5 _tmpB9=({struct
_tuple5 _tmpB8;_tmpB8.f1=il1;_tmpB8.f2=il2;_tmpB8;});void*_tmpBA;void*_tmpBB;void*
_tmpBC;void*_tmpBD;_LLDC: _LLE6: _tmpBA=_tmpB9.f2;if((int)_tmpBA == 0){goto _LLDD;}
else{goto _LLDE;}_LLDE: _LLE7: _tmpBB=_tmpB9.f1;if((int)_tmpBB == 0){goto _LLDF;}
else{goto _LLE0;}_LLE0: _LLE8: _tmpBC=_tmpB9.f2;if((int)_tmpBC == 1){goto _LLE1;}
else{goto _LLE2;}_LLE2: _LLE9: _tmpBD=_tmpB9.f1;if((int)_tmpBD == 1){goto _LLE3;}
else{goto _LLE4;}_LLE4: goto _LLE5;_LLDD: goto _LLDF;_LLDF: return Cyc_CfFlowInfo_unknown_none;
_LLE1: goto _LLE3;_LLE3: return Cyc_CfFlowInfo_unknown_this;_LLE5: return Cyc_CfFlowInfo_unknown_all;
_LLDB:;}_LLC3:;}}int Cyc_CfFlowInfo_same_relop(void*r1,void*r2){if(r1 == r2){
return 1;}{struct _tuple5 _tmpBF=({struct _tuple5 _tmpBE;_tmpBE.f1=r1;_tmpBE.f2=r2;
_tmpBE;});void*_tmpC0;unsigned int _tmpC1;void*_tmpC2;unsigned int _tmpC3;void*
_tmpC4;struct Cyc_Absyn_Vardecl*_tmpC5;void*_tmpC6;struct Cyc_Absyn_Vardecl*_tmpC7;
void*_tmpC8;struct Cyc_Absyn_Vardecl*_tmpC9;void*_tmpCA;struct Cyc_Absyn_Vardecl*
_tmpCB;void*_tmpCC;unsigned int _tmpCD;void*_tmpCE;unsigned int _tmpCF;void*_tmpD0;
struct Cyc_Absyn_Vardecl*_tmpD1;void*_tmpD2;struct Cyc_Absyn_Vardecl*_tmpD3;_LLEB:
_LLF9: _tmpC2=_tmpBF.f1;if(*((int*)_tmpC2)== 0){_LLFA: _tmpC3=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmpC2)->f1;goto _LLF7;}else{goto _LLED;}_LLF7: _tmpC0=_tmpBF.f2;if(*((int*)_tmpC0)
== 0){_LLF8: _tmpC1=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmpC0)->f1;goto
_LLEC;}else{goto _LLED;}_LLED: _LLFD: _tmpC6=_tmpBF.f1;if(*((int*)_tmpC6)== 1){
_LLFE: _tmpC7=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmpC6)->f1;goto _LLFB;}else{
goto _LLEF;}_LLFB: _tmpC4=_tmpBF.f2;if(*((int*)_tmpC4)== 1){_LLFC: _tmpC5=((struct
Cyc_CfFlowInfo_LessVar_struct*)_tmpC4)->f1;goto _LLEE;}else{goto _LLEF;}_LLEF:
_LL101: _tmpCA=_tmpBF.f1;if(*((int*)_tmpCA)== 2){_LL102: _tmpCB=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmpCA)->f1;goto _LLFF;}else{goto _LLF1;}_LLFF: _tmpC8=_tmpBF.f2;if(*((int*)_tmpC8)
== 2){_LL100: _tmpC9=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmpC8)->f1;goto
_LLF0;}else{goto _LLF1;}_LLF1: _LL105: _tmpCE=_tmpBF.f1;if(*((int*)_tmpCE)== 3){
_LL106: _tmpCF=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmpCE)->f1;goto _LL103;}
else{goto _LLF3;}_LL103: _tmpCC=_tmpBF.f2;if(*((int*)_tmpCC)== 3){_LL104: _tmpCD=((
struct Cyc_CfFlowInfo_LessConst_struct*)_tmpCC)->f1;goto _LLF2;}else{goto _LLF3;}
_LLF3: _LL109: _tmpD2=_tmpBF.f1;if(*((int*)_tmpD2)== 4){_LL10A: _tmpD3=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmpD2)->f1;goto _LL107;}else{goto _LLF5;}_LL107: _tmpD0=_tmpBF.f2;if(*((int*)
_tmpD0)== 4){_LL108: _tmpD1=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmpD0)->f1;
goto _LLF4;}else{goto _LLF5;}_LLF5: goto _LLF6;_LLEC: return _tmpC3 == _tmpC1;_LLEE:
return _tmpC7 == _tmpC5;_LLF0: return _tmpCB == _tmpC9;_LLF2: return _tmpCF == _tmpCD;
_LLF4: return _tmpD3 == _tmpD1;_LLF6: return 0;_LLEA:;}}struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(r1s == r2s){return r1s;}{
struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*_tmpD4=r1s;for(0;_tmpD4
!= 0;_tmpD4=_tmpD4->tl){struct Cyc_CfFlowInfo_Reln*_tmpD5=(struct Cyc_CfFlowInfo_Reln*)
_tmpD4->hd;int found=0;{struct Cyc_List_List*_tmpD6=r2s;for(0;_tmpD6 != 0;_tmpD6=
_tmpD6->tl){struct Cyc_CfFlowInfo_Reln*_tmpD7=(struct Cyc_CfFlowInfo_Reln*)_tmpD6->hd;
if(_tmpD5 == _tmpD7? 1:(_tmpD5->vd == _tmpD7->vd? Cyc_CfFlowInfo_same_relop((void*)
_tmpD5->rop,(void*)_tmpD7->rop): 0)){res=({struct Cyc_List_List*_tmpD8=_cycalloc(
sizeof(struct Cyc_List_List));_tmpD8->hd=_tmpD5;_tmpD8->tl=res;_tmpD8;});found=1;
break;}}}if(! found){diff=1;}}}if(! diff){res=r1s;}return res;}}void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**all_changed,void*f1,void*f2){if(f1 == f2){return f1;}{struct
_tuple5 _tmpDA=({struct _tuple5 _tmpD9;_tmpD9.f1=f1;_tmpD9.f2=f2;_tmpD9;});void*
_tmpDB;void*_tmpDC;void*_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_Dict_Dict*
_tmpDF;void*_tmpE0;struct Cyc_List_List*_tmpE1;struct Cyc_Dict_Dict*_tmpE2;_LL10C:
_LL112: _tmpDB=_tmpDA.f1;if((int)_tmpDB == 0){goto _LL10D;}else{goto _LL10E;}_LL10E:
_LL113: _tmpDC=_tmpDA.f2;if((int)_tmpDC == 0){goto _LL10F;}else{goto _LL110;}_LL110:
_LL117: _tmpE0=_tmpDA.f1;if((unsigned int)_tmpE0 > 1?*((int*)_tmpE0)== 0: 0){_LL119:
_tmpE2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpE0)->f1;goto _LL118;_LL118:
_tmpE1=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpE0)->f2;goto _LL114;}else{
goto _LL10B;}_LL114: _tmpDD=_tmpDA.f2;if((unsigned int)_tmpDD > 1?*((int*)_tmpDD)== 
0: 0){_LL116: _tmpDF=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpDD)->f1;goto
_LL115;_LL115: _tmpDE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpDD)->f2;goto
_LL111;}else{goto _LL10B;}_LL10D: return f2;_LL10F: return f1;_LL111: if(_tmpE2 == 
_tmpDF? _tmpE1 == _tmpDE: 0){return f1;}if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2)){
return f2;}if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1)){return f1;}{struct
_RegionHandle _tmpE3=_new_region("rgn");struct _RegionHandle*rgn=& _tmpE3;
_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmpE4=({struct Cyc_CfFlowInfo_JoinEnv
_tmpEA;_tmpEA.pile=({struct Cyc_CfFlowInfo_EscPile*_tmpEB=_region_malloc(rgn,
sizeof(struct Cyc_CfFlowInfo_EscPile));_tmpEB->rgn=rgn;_tmpEB->places=0;_tmpEB;});
_tmpEA.d1=_tmpE2;_tmpEA.d2=_tmpDF;_tmpEA;});struct Cyc_Dict_Dict*_tmpE5=((struct
Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),
struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))
Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,& _tmpE4,_tmpE2,_tmpDF);struct
Cyc_List_List*_tmpE6=Cyc_CfFlowInfo_join_relns(_tmpE1,_tmpDE);void*_tmpE9=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmpE7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));
_tmpE7[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmpE8;_tmpE8.tag=0;_tmpE8.f1=
Cyc_CfFlowInfo_escape_these(_tmpE4.pile,all_changed,_tmpE5);_tmpE8.f2=_tmpE6;
_tmpE8;});_tmpE7;});_npop_handler(0);return _tmpE9;};_pop_region(rgn);}_LL10B:;}}
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,struct
_tagged_arr*,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*
env,void*r1,void*r2){int changed1=env->changed == Cyc_CfFlowInfo_One? 1:((int(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_member)(env->chg1,
env->curr_place);int changed2=env->changed == Cyc_CfFlowInfo_Two? 1:((int(*)(struct
Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_member)(env->chg2,env->curr_place);
if(changed1? changed2: 0){return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,
void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}if(
changed1){if(! Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2)){return
r1;}env->changed=Cyc_CfFlowInfo_One;}if(changed2){if(! Cyc_CfFlowInfo_prefix_of_member(
env->curr_place,env->chg1)){return r2;}env->changed=Cyc_CfFlowInfo_Two;}{struct
_tuple5 _tmpED=({struct _tuple5 _tmpEC;_tmpEC.f1=r1;_tmpEC.f2=r2;_tmpEC;});void*
_tmpEE;struct Cyc_Dict_Dict*_tmpEF;void*_tmpF0;struct Cyc_Dict_Dict*_tmpF1;_LL11B:
_LL121: _tmpF0=_tmpED.f1;if((unsigned int)_tmpF0 > 3?*((int*)_tmpF0)== 3: 0){_LL122:
_tmpF1=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpF0)->f1;goto _LL11F;}else{
goto _LL11D;}_LL11F: _tmpEE=_tmpED.f2;if((unsigned int)_tmpEE > 3?*((int*)_tmpEE)== 
3: 0){_LL120: _tmpEF=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpEE)->f1;goto
_LL11C;}else{goto _LL11D;}_LL11D: goto _LL11E;_LL11C: {struct Cyc_Dict_Dict*_tmpF2=((
struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,struct
_tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_absRval_d,
env,_tmpF1,_tmpEF);if(_tmpF2 == _tmpF1){return r1;}if(_tmpF2 == _tmpEF){return r2;}
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpF3=_cycalloc(sizeof(
struct Cyc_CfFlowInfo_Aggregate_struct));_tmpF3[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpF4;_tmpF4.tag=3;_tmpF4.f1=_tmpF2;_tmpF4;});_tmpF3;});}_LL11E:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmpF5=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));
_tmpF5[0]=({struct Cyc_Core_Impossible_struct _tmpF6;_tmpF6.tag=Cyc_Core_Impossible;
_tmpF6.f1=_tag_arr("after_pathinfo -- non-aggregates!",sizeof(unsigned char),34);
_tmpF6;});_tmpF5;}));_LL11A:;}}static void*Cyc_CfFlowInfo_after_absRval_d(struct
Cyc_CfFlowInfo_AfterEnv*env,struct _tagged_arr*key,void*r1,void*r2){if(r1 == r2){
return r1;}{struct Cyc_List_List**_tmpF7=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
_tmpF8=env->changed;*env->last_field_cell=({struct Cyc_List_List*_tmpF9=_cycalloc(
sizeof(struct Cyc_List_List));_tmpF9->hd=key;_tmpF9->tl=0;_tmpF9;});env->last_field_cell=&((
struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{void*_tmpFA=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmpF7;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmpF8;return _tmpFA;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2){return
r1;}*env->curr_place=({struct Cyc_CfFlowInfo_Place _tmpFB;_tmpFB.root=(void*)root;
_tmpFB.fields=0;_tmpFB;});env->last_field_cell=&(env->curr_place)->fields;env->changed=
Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}void*Cyc_CfFlowInfo_after_flow(
struct Cyc_Set_Set**all_changed,void*f1,void*f2,struct Cyc_Set_Set*chg1,struct Cyc_Set_Set*
chg2){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,(void*)((void*)0)};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((void*)& dummy_rawexp),0,(void*)((
void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_CfFlowInfo_MallocPt_struct
dummy_root={1,& dummy_exp};if(f1 == f2){return f1;}{struct _tuple5 _tmpFD=({struct
_tuple5 _tmpFC;_tmpFC.f1=f1;_tmpFC.f2=f2;_tmpFC;});void*_tmpFE;void*_tmpFF;void*
_tmp100;struct Cyc_List_List*_tmp101;struct Cyc_Dict_Dict*_tmp102;void*_tmp103;
struct Cyc_List_List*_tmp104;struct Cyc_Dict_Dict*_tmp105;_LL124: _LL12A: _tmpFE=
_tmpFD.f1;if((int)_tmpFE == 0){goto _LL125;}else{goto _LL126;}_LL126: _LL12B: _tmpFF=
_tmpFD.f2;if((int)_tmpFF == 0){goto _LL127;}else{goto _LL128;}_LL128: _LL12F: _tmp103=
_tmpFD.f1;if((unsigned int)_tmp103 > 1?*((int*)_tmp103)== 0: 0){_LL131: _tmp105=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp103)->f1;goto _LL130;_LL130: _tmp104=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp103)->f2;goto _LL12C;}else{goto
_LL123;}_LL12C: _tmp100=_tmpFD.f2;if((unsigned int)_tmp100 > 1?*((int*)_tmp100)== 
0: 0){_LL12E: _tmp102=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp100)->f1;goto
_LL12D;_LL12D: _tmp101=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp100)->f2;
goto _LL129;}else{goto _LL123;}_LL125: goto _LL127;_LL127: return(void*)0;_LL129: if(
_tmp105 == _tmp102? _tmp104 == _tmp101: 0){return f1;}{struct _RegionHandle _tmp106=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp106;_push_region(rgn);{struct Cyc_CfFlowInfo_Place*
_tmp107=({struct Cyc_CfFlowInfo_Place*_tmp111=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));
_tmp111->root=(void*)((void*)& dummy_root);_tmp111->fields=0;_tmp111;});struct Cyc_CfFlowInfo_AfterEnv
_tmp108=({struct Cyc_CfFlowInfo_AfterEnv _tmp10E;_tmp10E.joinenv=({struct Cyc_CfFlowInfo_JoinEnv
_tmp10F;_tmp10F.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp110=_region_malloc(rgn,
sizeof(struct Cyc_CfFlowInfo_EscPile));_tmp110->rgn=rgn;_tmp110->places=0;_tmp110;});
_tmp10F.d1=_tmp105;_tmp10F.d2=_tmp102;_tmp10F;});_tmp10E.chg1=chg1;_tmp10E.chg2=
chg2;_tmp10E.curr_place=_tmp107;_tmp10E.last_field_cell=& _tmp107->fields;_tmp10E.changed=
Cyc_CfFlowInfo_Neither;_tmp10E;});struct Cyc_Dict_Dict*_tmp109=((struct Cyc_Dict_Dict*(*)(
void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp108,_tmp105,_tmp102);struct Cyc_List_List*_tmp10A=Cyc_CfFlowInfo_join_relns(
_tmp104,_tmp101);void*_tmp10D=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp10B=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp10B[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp10C;_tmp10C.tag=0;_tmp10C.f1=Cyc_CfFlowInfo_escape_these((
_tmp108.joinenv).pile,all_changed,_tmp109);_tmp10C.f2=_tmp10A;_tmp10C;});_tmp10B;});
_npop_handler(0);return _tmp10D;};_pop_region(rgn);}_LL123:;}}static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2){return 1;}{struct _tuple5 _tmp115=({struct
_tuple5 _tmp114;_tmp114.f1=r1;_tmp114.f2=r2;_tmp114;});void*_tmp116;struct Cyc_CfFlowInfo_Place*
_tmp117;void*_tmp118;struct Cyc_CfFlowInfo_Place*_tmp119;void*_tmp11A;void*
_tmp11B;void*_tmp11C;struct Cyc_Dict_Dict*_tmp11D;void*_tmp11E;struct Cyc_Dict_Dict*
_tmp11F;void*_tmp120;void*_tmp121;void*_tmp122;void*_tmp123;void*_tmp124;void*
_tmp125;_LL133: _LL149: _tmp118=_tmp115.f1;if((unsigned int)_tmp118 > 3?*((int*)
_tmp118)== 2: 0){_LL14A: _tmp119=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp118)->f1;
goto _LL147;}else{goto _LL135;}_LL147: _tmp116=_tmp115.f2;if((unsigned int)_tmp116 > 
3?*((int*)_tmp116)== 2: 0){_LL148: _tmp117=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp116)->f1;goto _LL134;}else{goto _LL135;}_LL135: _LL14B: _tmp11A=_tmp115.f1;if((
unsigned int)_tmp11A > 3?*((int*)_tmp11A)== 2: 0){goto _LL136;}else{goto _LL137;}
_LL137: _LL14C: _tmp11B=_tmp115.f2;if((unsigned int)_tmp11B > 3?*((int*)_tmp11B)== 
2: 0){goto _LL138;}else{goto _LL139;}_LL139: _LL14F: _tmp11E=_tmp115.f1;if((
unsigned int)_tmp11E > 3?*((int*)_tmp11E)== 3: 0){_LL150: _tmp11F=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp11E)->f1;goto _LL14D;}else{goto _LL13B;}_LL14D: _tmp11C=_tmp115.f2;if((
unsigned int)_tmp11C > 3?*((int*)_tmp11C)== 3: 0){_LL14E: _tmp11D=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp11C)->f1;goto _LL13A;}else{goto _LL13B;}_LL13B: _LL151: _tmp120=_tmp115.f2;if((
int)_tmp120 == 2){goto _LL13C;}else{goto _LL13D;}_LL13D: _LL152: _tmp121=_tmp115.f2;
if((int)_tmp121 == 0){goto _LL13E;}else{goto _LL13F;}_LL13F: _LL153: _tmp122=_tmp115.f2;
if((int)_tmp122 == 1){goto _LL140;}else{goto _LL141;}_LL141: _LL155: _tmp124=_tmp115.f1;
if((unsigned int)_tmp124 > 3?*((int*)_tmp124)== 1: 0){goto _LL154;}else{goto _LL143;}
_LL154: _tmp123=_tmp115.f2;if((unsigned int)_tmp123 > 3?*((int*)_tmp123)== 1: 0){
goto _LL142;}else{goto _LL143;}_LL143: _LL156: _tmp125=_tmp115.f1;if((unsigned int)
_tmp125 > 3?*((int*)_tmp125)== 1: 0){goto _LL144;}else{goto _LL145;}_LL145: goto
_LL146;_LL134: return Cyc_CfFlowInfo_place_cmp(_tmp119,_tmp117)== 0;_LL136: goto
_LL138;_LL138: return 0;_LL13A: return _tmp11F == _tmp11D? 1:((int(*)(int(*f)(struct
_tagged_arr*,void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_forall_intersect)((
int(*)(struct _tagged_arr*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp11F,_tmp11D);_LL13C: return r1 == (void*)1;_LL13E: goto _LL140;_LL140: return 0;
_LL142: goto _LL132;_LL144: return 0;_LL146: goto _LL132;_LL132:;}{struct _tuple5
_tmp127=({struct _tuple5 _tmp126;_tmp126.f1=Cyc_CfFlowInfo_initlevel_approx(r1);
_tmp126.f2=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp126;});void*_tmp128;void*
_tmp129;void*_tmp12A;void*_tmp12B;void*_tmp12C;void*_tmp12D;_LL158: _LL163:
_tmp129=_tmp127.f1;if((int)_tmp129 == 2){goto _LL162;}else{goto _LL15A;}_LL162:
_tmp128=_tmp127.f2;if((int)_tmp128 == 2){goto _LL159;}else{goto _LL15A;}_LL15A:
_LL164: _tmp12A=_tmp127.f2;if((int)_tmp12A == 0){goto _LL15B;}else{goto _LL15C;}
_LL15C: _LL165: _tmp12B=_tmp127.f1;if((int)_tmp12B == 0){goto _LL15D;}else{goto
_LL15E;}_LL15E: _LL166: _tmp12C=_tmp127.f2;if((int)_tmp12C == 1){goto _LL15F;}else{
goto _LL160;}_LL160: _LL167: _tmp12D=_tmp127.f1;if((int)_tmp12D == 1){goto _LL161;}
else{goto _LL157;}_LL159: return 1;_LL15B: return 1;_LL15D: return 0;_LL15F: return 1;
_LL161: return 0;_LL157:;}}int Cyc_CfFlowInfo_relns_approx(struct Cyc_List_List*r2s,
struct Cyc_List_List*r1s){if(r1s == r2s){return 1;}for(0;r1s != 0;r1s=r1s->tl){
struct Cyc_CfFlowInfo_Reln*_tmp12E=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;int found=0;{
struct Cyc_List_List*_tmp12F=r2s;for(0;_tmp12F != 0;_tmp12F=_tmp12F->tl){struct Cyc_CfFlowInfo_Reln*
_tmp130=(struct Cyc_CfFlowInfo_Reln*)_tmp12F->hd;if(_tmp12E == _tmp130? 1:(_tmp12E->vd
== _tmp130->vd? Cyc_CfFlowInfo_same_relop((void*)_tmp12E->rop,(void*)_tmp130->rop):
0)){found=1;break;}}}if(! found){return 0;}}return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(
void*f1,void*f2){if(f1 == f2){return 1;}{struct _tuple5 _tmp132=({struct _tuple5
_tmp131;_tmp131.f1=f1;_tmp131.f2=f2;_tmp131;});void*_tmp133;void*_tmp134;void*
_tmp135;struct Cyc_List_List*_tmp136;struct Cyc_Dict_Dict*_tmp137;void*_tmp138;
struct Cyc_List_List*_tmp139;struct Cyc_Dict_Dict*_tmp13A;_LL169: _LL16F: _tmp133=
_tmp132.f1;if((int)_tmp133 == 0){goto _LL16A;}else{goto _LL16B;}_LL16B: _LL170:
_tmp134=_tmp132.f2;if((int)_tmp134 == 0){goto _LL16C;}else{goto _LL16D;}_LL16D:
_LL174: _tmp138=_tmp132.f1;if((unsigned int)_tmp138 > 1?*((int*)_tmp138)== 0: 0){
_LL176: _tmp13A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp138)->f1;goto
_LL175;_LL175: _tmp139=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp138)->f2;
goto _LL171;}else{goto _LL168;}_LL171: _tmp135=_tmp132.f2;if((unsigned int)_tmp135 > 
1?*((int*)_tmp135)== 0: 0){_LL173: _tmp137=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp135)->f1;goto _LL172;_LL172: _tmp136=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp135)->f2;goto _LL16E;}else{goto _LL168;}_LL16A: return 1;_LL16C: return 0;_LL16E:
if(_tmp13A == _tmp137? _tmp139 == _tmp136: 0){return 1;}return Cyc_Dict_forall_intersect(
Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp13A,_tmp137)? Cyc_CfFlowInfo_relns_approx(
_tmp139,_tmp136): 0;_LL168:;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=
0;for(p=rs;! found? p != 0: 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp13B=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp13B->vd == v){found=1;break;}{void*_tmp13C=(void*)_tmp13B->rop;
struct Cyc_Absyn_Vardecl*_tmp13D;struct Cyc_Absyn_Vardecl*_tmp13E;struct Cyc_Absyn_Vardecl*
_tmp13F;_LL178: if(*((int*)_tmp13C)== 1){_LL180: _tmp13D=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp13C)->f1;goto _LL179;}else{goto _LL17A;}_LL17A: if(*((int*)_tmp13C)== 2){_LL181:
_tmp13E=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp13C)->f1;goto _LL17B;}else{
goto _LL17C;}_LL17C: if(*((int*)_tmp13C)== 4){_LL182: _tmp13F=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp13C)->f1;goto _LL17D;}else{goto _LL17E;}_LL17E: goto _LL17F;_LL179: _tmp13E=
_tmp13D;goto _LL17B;_LL17B: _tmp13F=_tmp13E;goto _LL17D;_LL17D: found=1;continue;
_LL17F: goto _LL177;_LL177:;}}if(! found){return rs;}{struct Cyc_List_List*_tmp140=0;
for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp141=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp141->vd != v){{void*_tmp142=(void*)_tmp141->rop;struct Cyc_Absyn_Vardecl*
_tmp143;struct Cyc_Absyn_Vardecl*_tmp144;struct Cyc_Absyn_Vardecl*_tmp145;_LL184:
if(*((int*)_tmp142)== 1){_LL18C: _tmp143=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp142)->f1;goto _LL185;}else{goto _LL186;}_LL186: if(*((int*)_tmp142)== 2){_LL18D:
_tmp144=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp142)->f1;goto _LL187;}else{
goto _LL188;}_LL188: if(*((int*)_tmp142)== 4){_LL18E: _tmp145=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp142)->f1;goto _LL189;}else{goto _LL18A;}_LL18A: goto _LL18B;_LL185: _tmp144=
_tmp143;goto _LL187;_LL187: _tmp145=_tmp144;goto _LL189;_LL189: if(v == _tmp145){
continue;}else{goto _LL183;}_LL18B: goto _LL183;;_LL183:;}_tmp140=({struct Cyc_List_List*
_tmp146=_cycalloc(sizeof(struct Cyc_List_List));_tmp146->hd=_tmp141;_tmp146->tl=
_tmp140;_tmp146;});}}return _tmp140;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*_tmp147=(void*)e->r;void*
_tmp148;struct Cyc_Absyn_Vardecl*_tmp149;void*_tmp14A;struct Cyc_Absyn_Vardecl*
_tmp14B;void*_tmp14C;struct Cyc_Absyn_Vardecl*_tmp14D;void*_tmp14E;struct Cyc_Absyn_Vardecl*
_tmp14F;_LL190: if(*((int*)_tmp147)== 1){_LL19A: _tmp148=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp147)->f2;if((unsigned int)_tmp148 > 1?*((int*)_tmp148)== 0: 0){_LL19B: _tmp149=((
struct Cyc_Absyn_Global_b_struct*)_tmp148)->f1;goto _LL191;}else{goto _LL192;}}
else{goto _LL192;}_LL192: if(*((int*)_tmp147)== 1){_LL19C: _tmp14A=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp147)->f2;if((unsigned int)_tmp14A > 1?*((int*)_tmp14A)
== 2: 0){_LL19D: _tmp14B=((struct Cyc_Absyn_Param_b_struct*)_tmp14A)->f1;goto _LL193;}
else{goto _LL194;}}else{goto _LL194;}_LL194: if(*((int*)_tmp147)== 1){_LL19E:
_tmp14C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp147)->f2;if((unsigned int)
_tmp14C > 1?*((int*)_tmp14C)== 3: 0){_LL19F: _tmp14D=((struct Cyc_Absyn_Local_b_struct*)
_tmp14C)->f1;goto _LL195;}else{goto _LL196;}}else{goto _LL196;}_LL196: if(*((int*)
_tmp147)== 1){_LL1A0: _tmp14E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp147)->f2;
if((unsigned int)_tmp14E > 1?*((int*)_tmp14E)== 4: 0){_LL1A1: _tmp14F=((struct Cyc_Absyn_Pat_b_struct*)
_tmp14E)->f1;goto _LL197;}else{goto _LL198;}}else{goto _LL198;}_LL198: goto _LL199;
_LL191: _tmp14B=_tmp149;goto _LL193;_LL193: _tmp14D=_tmp14B;goto _LL195;_LL195:
_tmp14F=_tmp14D;goto _LL197;_LL197: if(! _tmp14F->escapes){return Cyc_CfFlowInfo_reln_kill_var(
r,_tmp14F);}goto _LL18F;_LL199: goto _LL18F;_LL18F:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,
struct Cyc_Absyn_Exp*e){if(v->escapes){return r;}r=Cyc_CfFlowInfo_reln_kill_var(r,
v);{void*_tmp150=(void*)e->r;struct Cyc_Absyn_MallocInfo _tmp151;int _tmp152;struct
Cyc_Absyn_Exp*_tmp153;_LL1A3: if(*((int*)_tmp150)== 33){_LL1A7: _tmp151=((struct
Cyc_Absyn_Malloc_e_struct*)_tmp150)->f1;_LL1A9: _tmp153=_tmp151.num_elts;goto
_LL1A8;_LL1A8: _tmp152=_tmp151.fat_result;if(_tmp152 == 1){goto _LL1A4;}else{goto
_LL1A5;}}else{goto _LL1A5;}_LL1A5: goto _LL1A6;_LL1A4: malloc_loop: {void*_tmp154=(
void*)_tmp153->r;struct Cyc_Absyn_Exp*_tmp155;void*_tmp156;struct Cyc_Absyn_Vardecl*
_tmp157;void*_tmp158;struct Cyc_Absyn_Vardecl*_tmp159;void*_tmp15A;struct Cyc_Absyn_Vardecl*
_tmp15B;void*_tmp15C;struct Cyc_Absyn_Vardecl*_tmp15D;_LL1AB: if(*((int*)_tmp154)
== 13){_LL1B7: _tmp155=((struct Cyc_Absyn_Cast_e_struct*)_tmp154)->f2;goto _LL1AC;}
else{goto _LL1AD;}_LL1AD: if(*((int*)_tmp154)== 1){_LL1B8: _tmp156=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp154)->f2;if((unsigned int)_tmp156 > 1?*((int*)_tmp156)
== 4: 0){_LL1B9: _tmp157=((struct Cyc_Absyn_Pat_b_struct*)_tmp156)->f1;goto _LL1AE;}
else{goto _LL1AF;}}else{goto _LL1AF;}_LL1AF: if(*((int*)_tmp154)== 1){_LL1BA:
_tmp158=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp154)->f2;if((unsigned int)
_tmp158 > 1?*((int*)_tmp158)== 3: 0){_LL1BB: _tmp159=((struct Cyc_Absyn_Local_b_struct*)
_tmp158)->f1;goto _LL1B0;}else{goto _LL1B1;}}else{goto _LL1B1;}_LL1B1: if(*((int*)
_tmp154)== 1){_LL1BC: _tmp15A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp154)->f2;
if((unsigned int)_tmp15A > 1?*((int*)_tmp15A)== 2: 0){_LL1BD: _tmp15B=((struct Cyc_Absyn_Param_b_struct*)
_tmp15A)->f1;goto _LL1B2;}else{goto _LL1B3;}}else{goto _LL1B3;}_LL1B3: if(*((int*)
_tmp154)== 1){_LL1BE: _tmp15C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp154)->f2;
if((unsigned int)_tmp15C > 1?*((int*)_tmp15C)== 0: 0){_LL1BF: _tmp15D=((struct Cyc_Absyn_Global_b_struct*)
_tmp15C)->f1;goto _LL1B4;}else{goto _LL1B5;}}else{goto _LL1B5;}_LL1B5: goto _LL1B6;
_LL1AC: _tmp153=_tmp155;goto malloc_loop;_LL1AE: _tmp159=_tmp157;goto _LL1B0;_LL1B0:
_tmp15B=_tmp159;goto _LL1B2;_LL1B2: _tmp15D=_tmp15B;goto _LL1B4;_LL1B4: if(_tmp15D->escapes){
return r;}return({struct Cyc_List_List*_tmp15E=_cycalloc(sizeof(struct Cyc_List_List));
_tmp15E->hd=({struct Cyc_CfFlowInfo_Reln*_tmp15F=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));
_tmp15F->vd=_tmp15D;_tmp15F->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp160=_cycalloc(sizeof(struct Cyc_CfFlowInfo_LessEqSize_struct));_tmp160[0]=({
struct Cyc_CfFlowInfo_LessEqSize_struct _tmp161;_tmp161.tag=4;_tmp161.f1=v;_tmp161;});
_tmp160;}));_tmp15F;});_tmp15E->tl=r;_tmp15E;});_LL1B6: return r;_LL1AA:;}_LL1A6:
goto _LL1A2;_LL1A2:;}{void*_tmp162=Cyc_Tcutil_compress((void*)v->type);_LL1C1: if((
unsigned int)_tmp162 > 3?*((int*)_tmp162)== 5: 0){goto _LL1C2;}else{goto _LL1C3;}
_LL1C3: goto _LL1C4;_LL1C2: goto _LL1C0;_LL1C4: return r;_LL1C0:;}loop: {void*_tmp163=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp164;void*_tmp165;int _tmp166;struct Cyc_List_List*
_tmp167;struct Cyc_List_List _tmp168;struct Cyc_List_List*_tmp169;struct Cyc_List_List
_tmp16A;struct Cyc_Absyn_Exp*_tmp16B;void*_tmp16C;struct Cyc_List_List*_tmp16D;
struct Cyc_List_List _tmp16E;struct Cyc_Absyn_Exp*_tmp16F;void*_tmp170;_LL1C6: if(*((
int*)_tmp163)== 13){_LL1D0: _tmp164=((struct Cyc_Absyn_Cast_e_struct*)_tmp163)->f2;
goto _LL1C7;}else{goto _LL1C8;}_LL1C8: if(*((int*)_tmp163)== 0){_LL1D1: _tmp165=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp163)->f1;if((unsigned int)_tmp165 > 1?*((
int*)_tmp165)== 2: 0){_LL1D2: _tmp166=((struct Cyc_Absyn_Int_c_struct*)_tmp165)->f2;
goto _LL1C9;}else{goto _LL1CA;}}else{goto _LL1CA;}_LL1CA: if(*((int*)_tmp163)== 3){
_LL1D6: _tmp16C=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp163)->f1;if((int)
_tmp16C == 4){goto _LL1D3;}else{goto _LL1CC;}_LL1D3: _tmp167=((struct Cyc_Absyn_Primop_e_struct*)
_tmp163)->f2;if(_tmp167 == 0){goto _LL1CC;}else{_tmp168=*_tmp167;_LL1D4: _tmp169=
_tmp168.tl;if(_tmp169 == 0){goto _LL1CC;}else{_tmp16A=*_tmp169;_LL1D5: _tmp16B=(
struct Cyc_Absyn_Exp*)_tmp16A.hd;goto _LL1CB;}}}else{goto _LL1CC;}_LL1CC: if(*((int*)
_tmp163)== 3){_LL1D9: _tmp170=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp163)->f1;
if((int)_tmp170 == 19){goto _LL1D7;}else{goto _LL1CE;}_LL1D7: _tmp16D=((struct Cyc_Absyn_Primop_e_struct*)
_tmp163)->f2;if(_tmp16D == 0){goto _LL1CE;}else{_tmp16E=*_tmp16D;_LL1D8: _tmp16F=(
struct Cyc_Absyn_Exp*)_tmp16E.hd;goto _LL1CD;}}else{goto _LL1CE;}_LL1CE: goto _LL1CF;
_LL1C7: e=_tmp164;goto loop;_LL1C9: return({struct Cyc_List_List*_tmp171=_cycalloc(
sizeof(struct Cyc_List_List));_tmp171->hd=({struct Cyc_CfFlowInfo_Reln*_tmp172=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));_tmp172->vd=v;_tmp172->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_EqualConst_struct*_tmp173=_cycalloc_atomic(sizeof(
struct Cyc_CfFlowInfo_EqualConst_struct));_tmp173[0]=({struct Cyc_CfFlowInfo_EqualConst_struct
_tmp174;_tmp174.tag=0;_tmp174.f1=(unsigned int)_tmp166;_tmp174;});_tmp173;}));
_tmp172;});_tmp171->tl=r;_tmp171;});_LL1CB:{void*_tmp175=(void*)_tmp16B->r;
struct Cyc_List_List*_tmp176;struct Cyc_List_List _tmp177;struct Cyc_Absyn_Exp*
_tmp178;void*_tmp179;_LL1DB: if(*((int*)_tmp175)== 3){_LL1E1: _tmp179=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp175)->f1;if((int)_tmp179 == 19){goto _LL1DF;}
else{goto _LL1DD;}_LL1DF: _tmp176=((struct Cyc_Absyn_Primop_e_struct*)_tmp175)->f2;
if(_tmp176 == 0){goto _LL1DD;}else{_tmp177=*_tmp176;_LL1E0: _tmp178=(struct Cyc_Absyn_Exp*)
_tmp177.hd;goto _LL1DC;}}else{goto _LL1DD;}_LL1DD: goto _LL1DE;_LL1DC:{void*_tmp17A=(
void*)_tmp178->r;void*_tmp17B;struct Cyc_Absyn_Vardecl*_tmp17C;void*_tmp17D;
struct Cyc_Absyn_Vardecl*_tmp17E;void*_tmp17F;struct Cyc_Absyn_Vardecl*_tmp180;
void*_tmp181;struct Cyc_Absyn_Vardecl*_tmp182;_LL1E3: if(*((int*)_tmp17A)== 1){
_LL1ED: _tmp17B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp17A)->f2;if((
unsigned int)_tmp17B > 1?*((int*)_tmp17B)== 0: 0){_LL1EE: _tmp17C=((struct Cyc_Absyn_Global_b_struct*)
_tmp17B)->f1;goto _LL1E4;}else{goto _LL1E5;}}else{goto _LL1E5;}_LL1E5: if(*((int*)
_tmp17A)== 1){_LL1EF: _tmp17D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp17A)->f2;
if((unsigned int)_tmp17D > 1?*((int*)_tmp17D)== 3: 0){_LL1F0: _tmp17E=((struct Cyc_Absyn_Local_b_struct*)
_tmp17D)->f1;goto _LL1E6;}else{goto _LL1E7;}}else{goto _LL1E7;}_LL1E7: if(*((int*)
_tmp17A)== 1){_LL1F1: _tmp17F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp17A)->f2;
if((unsigned int)_tmp17F > 1?*((int*)_tmp17F)== 2: 0){_LL1F2: _tmp180=((struct Cyc_Absyn_Param_b_struct*)
_tmp17F)->f1;goto _LL1E8;}else{goto _LL1E9;}}else{goto _LL1E9;}_LL1E9: if(*((int*)
_tmp17A)== 1){_LL1F3: _tmp181=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp17A)->f2;
if((unsigned int)_tmp181 > 1?*((int*)_tmp181)== 4: 0){_LL1F4: _tmp182=((struct Cyc_Absyn_Pat_b_struct*)
_tmp181)->f1;goto _LL1EA;}else{goto _LL1EB;}}else{goto _LL1EB;}_LL1EB: goto _LL1EC;
_LL1E4: _tmp17E=_tmp17C;goto _LL1E6;_LL1E6: _tmp180=_tmp17E;goto _LL1E8;_LL1E8:
_tmp182=_tmp180;goto _LL1EA;_LL1EA: if(_tmp182->escapes){return r;}return({struct
Cyc_List_List*_tmp183=_cycalloc(sizeof(struct Cyc_List_List));_tmp183->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp184=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));
_tmp184->vd=v;_tmp184->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp185=_cycalloc(sizeof(struct Cyc_CfFlowInfo_LessSize_struct));_tmp185[0]=({
struct Cyc_CfFlowInfo_LessSize_struct _tmp186;_tmp186.tag=2;_tmp186.f1=_tmp182;
_tmp186;});_tmp185;}));_tmp184;});_tmp183->tl=r;_tmp183;});_LL1EC: goto _LL1E2;
_LL1E2:;}goto _LL1DA;_LL1DE: goto _LL1DA;_LL1DA:;}goto _LL1C5;_LL1CD:{void*_tmp187=(
void*)_tmp16F->r;void*_tmp188;struct Cyc_Absyn_Vardecl*_tmp189;void*_tmp18A;
struct Cyc_Absyn_Vardecl*_tmp18B;void*_tmp18C;struct Cyc_Absyn_Vardecl*_tmp18D;
void*_tmp18E;struct Cyc_Absyn_Vardecl*_tmp18F;_LL1F6: if(*((int*)_tmp187)== 1){
_LL200: _tmp188=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp187)->f2;if((
unsigned int)_tmp188 > 1?*((int*)_tmp188)== 0: 0){_LL201: _tmp189=((struct Cyc_Absyn_Global_b_struct*)
_tmp188)->f1;goto _LL1F7;}else{goto _LL1F8;}}else{goto _LL1F8;}_LL1F8: if(*((int*)
_tmp187)== 1){_LL202: _tmp18A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp187)->f2;
if((unsigned int)_tmp18A > 1?*((int*)_tmp18A)== 3: 0){_LL203: _tmp18B=((struct Cyc_Absyn_Local_b_struct*)
_tmp18A)->f1;goto _LL1F9;}else{goto _LL1FA;}}else{goto _LL1FA;}_LL1FA: if(*((int*)
_tmp187)== 1){_LL204: _tmp18C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp187)->f2;
if((unsigned int)_tmp18C > 1?*((int*)_tmp18C)== 2: 0){_LL205: _tmp18D=((struct Cyc_Absyn_Param_b_struct*)
_tmp18C)->f1;goto _LL1FB;}else{goto _LL1FC;}}else{goto _LL1FC;}_LL1FC: if(*((int*)
_tmp187)== 1){_LL206: _tmp18E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp187)->f2;
if((unsigned int)_tmp18E > 1?*((int*)_tmp18E)== 4: 0){_LL207: _tmp18F=((struct Cyc_Absyn_Pat_b_struct*)
_tmp18E)->f1;goto _LL1FD;}else{goto _LL1FE;}}else{goto _LL1FE;}_LL1FE: goto _LL1FF;
_LL1F7: _tmp18B=_tmp189;goto _LL1F9;_LL1F9: _tmp18D=_tmp18B;goto _LL1FB;_LL1FB:
_tmp18F=_tmp18D;goto _LL1FD;_LL1FD: if(_tmp18F->escapes){return r;}return({struct
Cyc_List_List*_tmp190=_cycalloc(sizeof(struct Cyc_List_List));_tmp190->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp191=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));
_tmp191->vd=v;_tmp191->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp192=_cycalloc(sizeof(struct Cyc_CfFlowInfo_LessEqSize_struct));_tmp192[0]=({
struct Cyc_CfFlowInfo_LessEqSize_struct _tmp193;_tmp193.tag=4;_tmp193.f1=_tmp18F;
_tmp193;});_tmp192;}));_tmp191;});_tmp190->tl=r;_tmp190;});_LL1FF: goto _LL1F5;
_LL1F5:;}goto _LL1C5;_LL1CF: goto _LL1C5;_LL1C5:;}return r;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*
_tmp194=(void*)e1->r;void*_tmp195;struct Cyc_Absyn_Vardecl*_tmp196;void*_tmp197;
struct Cyc_Absyn_Vardecl*_tmp198;void*_tmp199;struct Cyc_Absyn_Vardecl*_tmp19A;
void*_tmp19B;struct Cyc_Absyn_Vardecl*_tmp19C;_LL209: if(*((int*)_tmp194)== 1){
_LL213: _tmp195=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp194)->f2;if((
unsigned int)_tmp195 > 1?*((int*)_tmp195)== 0: 0){_LL214: _tmp196=((struct Cyc_Absyn_Global_b_struct*)
_tmp195)->f1;goto _LL20A;}else{goto _LL20B;}}else{goto _LL20B;}_LL20B: if(*((int*)
_tmp194)== 1){_LL215: _tmp197=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp194)->f2;
if((unsigned int)_tmp197 > 1?*((int*)_tmp197)== 2: 0){_LL216: _tmp198=((struct Cyc_Absyn_Param_b_struct*)
_tmp197)->f1;goto _LL20C;}else{goto _LL20D;}}else{goto _LL20D;}_LL20D: if(*((int*)
_tmp194)== 1){_LL217: _tmp199=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp194)->f2;
if((unsigned int)_tmp199 > 1?*((int*)_tmp199)== 3: 0){_LL218: _tmp19A=((struct Cyc_Absyn_Local_b_struct*)
_tmp199)->f1;goto _LL20E;}else{goto _LL20F;}}else{goto _LL20F;}_LL20F: if(*((int*)
_tmp194)== 1){_LL219: _tmp19B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp194)->f2;
if((unsigned int)_tmp19B > 1?*((int*)_tmp19B)== 4: 0){_LL21A: _tmp19C=((struct Cyc_Absyn_Pat_b_struct*)
_tmp19B)->f1;goto _LL210;}else{goto _LL211;}}else{goto _LL211;}_LL211: goto _LL212;
_LL20A: _tmp198=_tmp196;goto _LL20C;_LL20C: _tmp19A=_tmp198;goto _LL20E;_LL20E:
_tmp19C=_tmp19A;goto _LL210;_LL210: if(! _tmp19C->escapes){return Cyc_CfFlowInfo_reln_assign_var(
r,_tmp19C,e2);}goto _LL208;_LL212: goto _LL208;_LL208:;}return r;}void Cyc_CfFlowInfo_print_reln(
struct Cyc_CfFlowInfo_Reln*r){({struct Cyc_Std_String_pa_struct _tmp19E;_tmp19E.tag=
0;_tmp19E.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((r->vd)->name);{void*
_tmp19D[1]={& _tmp19E};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s",sizeof(
unsigned char),3),_tag_arr(_tmp19D,sizeof(void*),1));}});{void*_tmp19F=(void*)r->rop;
unsigned int _tmp1A0;struct Cyc_Absyn_Vardecl*_tmp1A1;struct Cyc_Absyn_Vardecl*
_tmp1A2;unsigned int _tmp1A3;struct Cyc_Absyn_Vardecl*_tmp1A4;_LL21C: if(*((int*)
_tmp19F)== 0){_LL226: _tmp1A0=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp19F)->f1;
goto _LL21D;}else{goto _LL21E;}_LL21E: if(*((int*)_tmp19F)== 1){_LL227: _tmp1A1=((
struct Cyc_CfFlowInfo_LessVar_struct*)_tmp19F)->f1;goto _LL21F;}else{goto _LL220;}
_LL220: if(*((int*)_tmp19F)== 2){_LL228: _tmp1A2=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp19F)->f1;goto _LL221;}else{goto _LL222;}_LL222: if(*((int*)_tmp19F)== 3){_LL229:
_tmp1A3=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp19F)->f1;goto _LL223;}else{
goto _LL224;}_LL224: if(*((int*)_tmp19F)== 4){_LL22A: _tmp1A4=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp19F)->f1;goto _LL225;}else{goto _LL21B;}_LL21D:({struct Cyc_Std_Int_pa_struct
_tmp1A6;_tmp1A6.tag=1;_tmp1A6.f1=(int)_tmp1A0;{void*_tmp1A5[1]={& _tmp1A6};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("==%d",sizeof(unsigned char),5),_tag_arr(_tmp1A5,sizeof(
void*),1));}});goto _LL21B;_LL21F:({struct Cyc_Std_String_pa_struct _tmp1A8;_tmp1A8.tag=
0;_tmp1A8.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1A1->name);{void*
_tmp1A7[1]={& _tmp1A8};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("<%s",sizeof(
unsigned char),4),_tag_arr(_tmp1A7,sizeof(void*),1));}});goto _LL21B;_LL221:({
struct Cyc_Std_String_pa_struct _tmp1AA;_tmp1AA.tag=0;_tmp1AA.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp1A2->name);{void*_tmp1A9[1]={& _tmp1AA};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("<%s.size",sizeof(unsigned char),9),
_tag_arr(_tmp1A9,sizeof(void*),1));}});goto _LL21B;_LL223:({struct Cyc_Std_Int_pa_struct
_tmp1AC;_tmp1AC.tag=1;_tmp1AC.f1=(int)_tmp1A3;{void*_tmp1AB[1]={& _tmp1AC};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("<%d",sizeof(unsigned char),4),_tag_arr(_tmp1AB,sizeof(
void*),1));}});goto _LL21B;_LL225:({struct Cyc_Std_String_pa_struct _tmp1AE;_tmp1AE.tag=
0;_tmp1AE.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1A4->name);{void*
_tmp1AD[1]={& _tmp1AE};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("<=%s.size",
sizeof(unsigned char),10),_tag_arr(_tmp1AD,sizeof(void*),1));}});goto _LL21B;
_LL21B:;}}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(0;r != 0;r=r->tl){
Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);if(r->tl != 0){({
void*_tmp1AF[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr(",",sizeof(
unsigned char),2),_tag_arr(_tmp1AF,sizeof(void*),0));});}}}
