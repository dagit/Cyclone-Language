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
void*Cyc_Core_identity(void*);extern unsigned char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
extern int isalnum(int);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
extern struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);extern int Cyc_List_length(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,
void*),void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*
x);extern void*Cyc_List_fold_right_c(void*(*f)(void*,void*,void*),void*,struct Cyc_List_List*
x,void*accum);extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*
x);extern unsigned char Cyc_List_Nth[8];extern int Cyc_List_forall(int(*pred)(void*),
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};extern struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_tabulate(int n,void*(*f)(int));extern struct Cyc_List_List*Cyc_List_filter(
int(*f)(void*),struct Cyc_List_List*x);struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
12];extern unsigned char Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(
int(*cmp)(void*,void*));extern struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*
d,void*k,void*v);extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);extern
struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*k);extern void
Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*
Cyc_Dict_delete(struct Cyc_Dict_Dict*,void*);extern unsigned int Cyc_Std_strlen(
struct _tagged_arr s);extern int Cyc_Std_strcmp(struct _tagged_arr s1,struct
_tagged_arr s2);extern struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct
_tagged_arr);extern struct _tagged_arr Cyc_Std_strdup(struct _tagged_arr src);struct
Cyc_Hashtable_Table;extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,
int(*cmp)(void*,void*),int(*hash)(void*));extern void Cyc_Hashtable_insert(struct
Cyc_Hashtable_Table*t,void*key,void*val);extern void*Cyc_Hashtable_lookup(struct
Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;extern struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*
seg;void*kind;struct _tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[
14];struct Cyc_Typerep_Var_struct{int tag;struct _tagged_arr*f1;};struct Cyc_Typerep_Int_struct{
int tag;unsigned int f1;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;
void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple1{
unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;
struct _tagged_arr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _tagged_arr f1;
};struct _tuple2{struct _tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Typerep_Union_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Typerep_Forall_struct{int tag;struct _tagged_arr f1;void**
f2;};struct Cyc_Typerep_App_struct{int tag;void*f1;struct _tagged_arr f2;};
unsigned int Cyc_Typerep_size_type(void*rep);struct Cyc_Std_tm{int tm_sec;int tm_min;
int tm_hour;int tm_mday;int tm_mon;int tm_year;int tm_wday;int tm_yday;int tm_isdst;};
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple3{void*f1;struct _tagged_arr*f2;};
struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int
q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*
f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*name;int*identity;void*kind;
};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrInfo{
void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Tqual
tq;struct Cyc_Absyn_Conref*bounds;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*
name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct
Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple3*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple3*
tunion_name;struct _tuple3*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
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
Cyc_List_List*f1;};struct Cyc_Absyn_StructType_struct{int tag;struct _tuple3*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Structdecl**f3;};struct Cyc_Absyn_UnionType_struct{
int tag;struct _tuple3*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Uniondecl**f3;};
struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple3*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_AnonStructType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonUnionType_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple3*f1;struct Cyc_List_List*f2;struct Cyc_Core_Opt*f3;};struct Cyc_Absyn_AccessEff_struct{
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
tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple3*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple3*f1;};struct Cyc_Absyn_Primop_e_struct{
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
int tag;struct Cyc_List_List*f1;};struct _tuple4{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple4*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple3*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Tuniondecl*f4;struct Cyc_Absyn_Tunionfield*
f5;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple3*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple3*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Codegen_e_struct{int tag;struct
Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Fill_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct _tuple5{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;struct _tuple5 condition;
struct _tuple5 delta;struct Cyc_Absyn_Stmt*body;};struct Cyc_Absyn_Exp_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple5
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct
Cyc_Absyn_Goto_s_struct{int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple5 f2;
struct _tuple5 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_SwitchC_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Cut_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Splice_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Label_s_struct{int tag;struct _tagged_arr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct _tuple5 f2;};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple3*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple3*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
Cyc_Absyn_UnknownFields_p_struct{int tag;struct _tuple3*f1;struct Cyc_List_List*f2;
struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*
cnst_exp;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
void*sc;struct _tuple3*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct _tuple3*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structfield{
struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structdecl{void*sc;struct
Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Uniondecl{void*sc;struct Cyc_Core_Opt*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple3*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple3*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple3*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple3*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple3*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Struct_d_struct{int tag;struct Cyc_Absyn_Structdecl*
f1;};struct Cyc_Absyn_Union_d_struct{int tag;struct Cyc_Absyn_Uniondecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple3*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[
15];extern int Cyc_Absyn_qvar_cmp(struct _tuple3*,struct _tuple3*);extern void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);extern void*Cyc_Absyn_uchar_t;extern void*Cyc_Absyn_uint_t;
extern void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern void*Cyc_Absyn_tagged_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
void*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(
struct _tuple3*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple3*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Cstdio___sFILE;struct
Cyc_Std___sFILE;extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int
Cyc_Std_printf(struct _tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr
Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct
Cyc_List_List*curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*);extern struct _tagged_arr Cyc_Absynpp_typ2cstring(void*);extern unsigned int
Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Set_Set;extern
unsigned char Cyc_Set_Absent[11];struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};
struct Cyc_Tcenv_StructRes_struct{int tag;struct Cyc_Absyn_Structdecl*f1;};struct
Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*
f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict*
structdecls;struct Cyc_Dict_Dict*uniondecls;struct Cyc_Dict_Dict*tuniondecls;
struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;
};struct Cyc_Tcenv_Frame_struct{int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*
ae;struct Cyc_Core_Opt*le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*
Cyc_Tcutil_impos(struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);extern void*
Cyc_Tcutil_compress(void*t);extern int Cyc_Tcutil_typecmp(void*,void*);extern void*
Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcgenrep_RepInfo{
struct Cyc_List_List*decls;struct Cyc_Absyn_Exp*exp;struct Cyc_List_List*
dependencies;struct Cyc_Core_Opt*fwd_decl;int emitted;int is_extern;};struct Cyc_Dict_Dict*
Cyc_Tcgenrep_empty_typerep_dict(){return((struct Cyc_Dict_Dict*(*)(int(*cmp)(void*,
void*)))Cyc_Dict_empty)(Cyc_Tcutil_typecmp);}void Cyc_Tcgenrep_print_dict_entry(
void*type,struct Cyc_Tcgenrep_RepInfo*info){({struct Cyc_Std_Int_pa_struct _tmp3;
_tmp3.tag=1;_tmp3.f1=(int)((unsigned int)info->emitted);{struct Cyc_Std_Int_pa_struct
_tmp2;_tmp2.tag=1;_tmp2.f1=(unsigned int)info;{struct Cyc_Std_String_pa_struct
_tmp1;_tmp1.tag=0;_tmp1.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(type);{void*
_tmp0[3]={& _tmp1,& _tmp2,& _tmp3};Cyc_Std_printf(_tag_arr("(%s,%x:%d,",sizeof(
unsigned char),11),_tag_arr(_tmp0,sizeof(void*),3));}}}});{struct Cyc_List_List*p=
info->dependencies;for(0;p != 0;p=p->tl){({struct Cyc_Std_Int_pa_struct _tmp5;_tmp5.tag=
1;_tmp5.f1=(unsigned int)((struct Cyc_Tcgenrep_RepInfo*)p->hd);{void*_tmp4[1]={&
_tmp5};Cyc_Std_printf(_tag_arr("%x",sizeof(unsigned char),3),_tag_arr(_tmp4,
sizeof(void*),1));}});if(p->tl != 0){({void*_tmp6[0]={};Cyc_Std_printf(_tag_arr(",",
sizeof(unsigned char),2),_tag_arr(_tmp6,sizeof(void*),0));});}}}({void*_tmp7[0]={};
Cyc_Std_printf(_tag_arr(")\n",sizeof(unsigned char),3),_tag_arr(_tmp7,sizeof(
void*),0));});}void Cyc_Tcgenrep_print_typerep_dict(struct Cyc_Dict_Dict*dict){((
void(*)(void(*f)(void*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Dict_Dict*d))Cyc_Dict_iter)(
Cyc_Tcgenrep_print_dict_entry,dict);}static int Cyc_Tcgenrep_rephash(struct Cyc_Tcgenrep_RepInfo*
ri){return(int)ri;}static int Cyc_Tcgenrep_repcmp(struct Cyc_Tcgenrep_RepInfo*r1,
struct Cyc_Tcgenrep_RepInfo*r2){unsigned int r1p;unsigned int r2p;r1p=(unsigned int)
r1;r2p=(unsigned int)r2;if(r1 < r2){return - 1;}else{if(r1 == r2){return 0;}else{
return 1;}}}static struct Cyc_Absyn_Tunionfield*Cyc_Tcgenrep_getField(struct Cyc_Absyn_Tuniondecl*
td,struct _tuple3*fieldname){if(td->fields == 0){({void*_tmp8[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Could not find field in tuniondecl",
sizeof(unsigned char),35),_tag_arr(_tmp8,sizeof(void*),0));});}else{struct Cyc_List_List*
l=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;
l != 0;l=l->tl){if(! Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Tunionfield*)l->hd)->name,
fieldname)){return(struct Cyc_Absyn_Tunionfield*)l->hd;}}}({void*_tmp9[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Could not find field in tuniondecl",
sizeof(unsigned char),35),_tag_arr(_tmp9,sizeof(void*),0));});(int)_throw((void*)
Cyc_Tcutil_TypeErr);}static unsigned char _tmpA[9]="Tcgenrep";static struct
_tagged_arr Cyc_Tcgenrep_tcgenrep_nm={_tmpA,_tmpA,_tmpA + 9};static struct Cyc_List_List
Cyc_Tcgenrep_l={(void*)& Cyc_Tcgenrep_tcgenrep_nm,0};static struct Cyc_Absyn_Abs_n_struct
Cyc_Tcgenrep_tcgenrep_nmspace={1,(struct Cyc_List_List*)& Cyc_Tcgenrep_l};static
void*Cyc_Tcgenrep_tcgenrep_nmspace2=(void*)& Cyc_Tcgenrep_tcgenrep_nmspace;static
unsigned char _tmpC[8]="Typerep";static struct _tagged_arr Cyc_Tcgenrep_typerep_nm={
_tmpC,_tmpC,_tmpC + 8};static struct Cyc_List_List Cyc_Tcgenrep_l2={(void*)& Cyc_Tcgenrep_typerep_nm,
0};static struct Cyc_Absyn_Abs_n_struct Cyc_Tcgenrep_typerep_nmspace={1,(struct Cyc_List_List*)&
Cyc_Tcgenrep_l2};static struct Cyc_List_List*Cyc_Tcgenrep_dfsvisit(struct Cyc_Tcgenrep_RepInfo*
ri,struct Cyc_Hashtable_Table*visited){struct _handler_cons _tmpE;_push_handler(&
_tmpE);{int _tmp10=0;if(setjmp(_tmpE.handler)){_tmp10=1;}if(! _tmp10){((int(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Tcgenrep_RepInfo*key))Cyc_Hashtable_lookup)(
visited,ri);{struct Cyc_List_List*_tmp11=0;_npop_handler(0);return _tmp11;};
_pop_handler();}else{void*_tmpF=(void*)_exn_thrown;void*_tmp13=_tmpF;_LL1: if(
_tmp13 == Cyc_Core_Not_found){goto _LL2;}else{goto _LL3;}_LL3: goto _LL4;_LL2:((void(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Tcgenrep_RepInfo*key,int val))Cyc_Hashtable_insert)(
visited,ri,1);{struct Cyc_List_List*ds=0;{struct Cyc_List_List*l=ri->dependencies;
for(0;l != 0;l=l->tl){ds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_imp_append)(ds,Cyc_Tcgenrep_dfsvisit((struct Cyc_Tcgenrep_RepInfo*)
l->hd,visited));}}if(ri->emitted == 0){if(ri->fwd_decl != 0){ds=({struct Cyc_List_List*
_tmp14=_cycalloc(sizeof(struct Cyc_List_List));_tmp14->hd=({struct Cyc_Absyn_Decl*
_tmp15[1];_tmp15[0]=(struct Cyc_Absyn_Decl*)((struct Cyc_Core_Opt*)_check_null(ri->fwd_decl))->v;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp15,
sizeof(struct Cyc_Absyn_Decl*),1));});_tmp14->tl=ds;_tmp14;});}{struct Cyc_List_List*
_tmp16=({struct Cyc_List_List*_tmp17=_cycalloc(sizeof(struct Cyc_List_List));
_tmp17->hd=ri->decls;_tmp17->tl=0;_tmp17;});ri->emitted=1;return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(ds,_tmp16);}}
else{return ds;}}_LL4:(void)_throw(_tmp13);_LL0:;}}}static struct Cyc_List_List*Cyc_Tcgenrep_dfs(
struct Cyc_Tcgenrep_RepInfo*ri){struct Cyc_Hashtable_Table*tab=((struct Cyc_Hashtable_Table*(*)(
int sz,int(*cmp)(struct Cyc_Tcgenrep_RepInfo*,struct Cyc_Tcgenrep_RepInfo*),int(*
hash)(struct Cyc_Tcgenrep_RepInfo*)))Cyc_Hashtable_create)(53,Cyc_Tcgenrep_repcmp,
Cyc_Tcgenrep_rephash);struct Cyc_List_List*ds=Cyc_Tcgenrep_dfsvisit(ri,tab);
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(ds);}
static unsigned char _tmp18[11]="Typestruct";static struct _tagged_arr Cyc_Tcgenrep_typestruct_str={
_tmp18,_tmp18,_tmp18 + 11};static unsigned char _tmp19[4]="Var";static struct
_tagged_arr Cyc_Tcgenrep_var_str={_tmp19,_tmp19,_tmp19 + 4};static unsigned char
_tmp1A[4]="Int";static struct _tagged_arr Cyc_Tcgenrep_int_str={_tmp1A,_tmp1A,
_tmp1A + 4};static unsigned char _tmp1B[6]="Float";static struct _tagged_arr Cyc_Tcgenrep_float_str={
_tmp1B,_tmp1B,_tmp1B + 6};static unsigned char _tmp1C[7]="Double";static struct
_tagged_arr Cyc_Tcgenrep_double_str={_tmp1C,_tmp1C,_tmp1C + 7};static unsigned char
_tmp1D[8]="ThinPtr";static struct _tagged_arr Cyc_Tcgenrep_thinptr_str={_tmp1D,
_tmp1D,_tmp1D + 8};static unsigned char _tmp1E[7]="FatPtr";static struct _tagged_arr
Cyc_Tcgenrep_fatptr_str={_tmp1E,_tmp1E,_tmp1E + 7};static unsigned char _tmp1F[6]="Tuple";
static struct _tagged_arr Cyc_Tcgenrep_tuple_str={_tmp1F,_tmp1F,_tmp1F + 6};static
unsigned char _tmp20[7]="TUnion";static struct _tagged_arr Cyc_Tcgenrep_tunion_str={
_tmp20,_tmp20,_tmp20 + 7};static unsigned char _tmp21[8]="XTUnion";static struct
_tagged_arr Cyc_Tcgenrep_xtunion_str={_tmp21,_tmp21,_tmp21 + 8};static
unsigned char _tmp22[6]="Union";static struct _tagged_arr Cyc_Tcgenrep_union_str={
_tmp22,_tmp22,_tmp22 + 6};static unsigned char _tmp23[7]="name_t";static struct
_tagged_arr Cyc_Tcgenrep_name_t_str={_tmp23,_tmp23,_tmp23 + 7};static struct _tuple3*
Cyc_Tcgenrep_typerep_name(struct _tagged_arr*name){return({struct _tuple3*_tmp24=
_cycalloc(sizeof(struct _tuple3));_tmp24->f1=(void*)& Cyc_Tcgenrep_typerep_nmspace;
_tmp24->f2=name;_tmp24;});}static int Cyc_Tcgenrep_gen_id_counter=0;static struct
_tagged_arr*Cyc_Tcgenrep_new_gen_id(struct _tagged_arr name){return({struct
_tagged_arr*_tmp25=_cycalloc(sizeof(struct _tagged_arr));_tmp25[0]=(struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp28;_tmp28.tag=1;_tmp28.f1=(int)((
unsigned int)Cyc_Tcgenrep_gen_id_counter ++);{struct Cyc_Std_String_pa_struct
_tmp27;_tmp27.tag=0;_tmp27.f1=(struct _tagged_arr)name;{void*_tmp26[2]={& _tmp27,&
_tmp28};Cyc_Std_aprintf(_tag_arr("_gen%s_%d",sizeof(unsigned char),10),_tag_arr(
_tmp26,sizeof(void*),2));}}});_tmp25;});}static void Cyc_Tcgenrep_print_params(
struct Cyc_List_List*l){({void*_tmp29[0]={};Cyc_Std_printf(_tag_arr("<",sizeof(
unsigned char),2),_tag_arr(_tmp29,sizeof(void*),0));});{struct Cyc_List_List*p=l;
for(0;p != 0;p=p->tl){({struct Cyc_Std_Int_pa_struct _tmp2C;_tmp2C.tag=1;_tmp2C.f1=(
int)((unsigned int)((int)(p->tl != 0?',':' ')));{struct Cyc_Std_String_pa_struct
_tmp2B;_tmp2B.tag=0;_tmp2B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)p->hd);{
void*_tmp2A[2]={& _tmp2B,& _tmp2C};Cyc_Std_printf(_tag_arr("%s%c",sizeof(
unsigned char),5),_tag_arr(_tmp2A,sizeof(void*),2));}}});}}({void*_tmp2D[0]={};
Cyc_Std_printf(_tag_arr(">\n",sizeof(unsigned char),3),_tag_arr(_tmp2D,sizeof(
void*),0));});}static void Cyc_Tcgenrep_print_tvars(struct Cyc_List_List*l){({void*
_tmp2E[0]={};Cyc_Std_printf(_tag_arr("<",sizeof(unsigned char),2),_tag_arr(
_tmp2E,sizeof(void*),0));});{struct Cyc_List_List*p=l;for(0;p != 0;p=p->tl){({
struct Cyc_Std_Int_pa_struct _tmp31;_tmp31.tag=1;_tmp31.f1=(int)((unsigned int)((
int)(p->tl != 0?',':' ')));{struct Cyc_Std_String_pa_struct _tmp30;_tmp30.tag=0;
_tmp30.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)p->hd)->name;{void*_tmp2F[
2]={& _tmp30,& _tmp31};Cyc_Std_printf(_tag_arr("%s%c",sizeof(unsigned char),5),
_tag_arr(_tmp2F,sizeof(void*),2));}}});}}({void*_tmp32[0]={};Cyc_Std_printf(
_tag_arr(">\n",sizeof(unsigned char),3),_tag_arr(_tmp32,sizeof(void*),0));});}
static struct Cyc_Absyn_Rel_n_struct Cyc_Tcgenrep_top_nm_0={0,0};static void*Cyc_Tcgenrep_top_nm=(
void*)& Cyc_Tcgenrep_top_nm_0;static struct _tuple3*Cyc_Tcgenrep_toplevel_name(
struct _tagged_arr*name){return({struct _tuple3*_tmp34=_cycalloc(sizeof(struct
_tuple3));_tmp34->f1=Cyc_Tcgenrep_top_nm;_tmp34->f2=(struct _tagged_arr*)name;
_tmp34;});}static struct _tuple3*Cyc_Tcgenrep_relative_name(struct Cyc_List_List*
pathl,struct _tagged_arr*name){struct Cyc_Absyn_Rel_n_struct*path=({struct Cyc_Absyn_Rel_n_struct*
_tmp36=_cycalloc(sizeof(struct Cyc_Absyn_Rel_n_struct));_tmp36[0]=({struct Cyc_Absyn_Rel_n_struct
_tmp37;_tmp37.tag=0;_tmp37.f1=pathl;_tmp37;});_tmp36;});return({struct _tuple3*
_tmp35=_cycalloc(sizeof(struct _tuple3));_tmp35->f1=(void*)path;_tmp35->f2=(
struct _tagged_arr*)name;_tmp35;});}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl(
struct _tagged_arr*name,void*type,struct Cyc_Absyn_Exp*init,struct Cyc_Position_Segment*
seg){struct _tuple3*qvar=Cyc_Tcgenrep_toplevel_name(name);struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(qvar,type,init);(void*)(vd->sc=(void*)((void*)0));{
struct Cyc_Absyn_Var_d_struct*r1=({struct Cyc_Absyn_Var_d_struct*_tmp38=_cycalloc(
sizeof(struct Cyc_Absyn_Var_d_struct));_tmp38[0]=({struct Cyc_Absyn_Var_d_struct
_tmp39;_tmp39.tag=0;_tmp39.f1=vd;_tmp39;});_tmp38;});void*r2=(void*)r1;return Cyc_Absyn_new_decl(
r2,seg);}}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_vardecl(struct _tuple3*name,
void*type,struct Cyc_Absyn_Exp*init,void*sc,struct Cyc_Position_Segment*seg){
struct _tuple3*topname=Cyc_Tcgenrep_toplevel_name((*name).f2);struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(topname,type,init);(void*)(vd->sc=(void*)sc);{struct Cyc_Absyn_Var_d_struct*
r1=({struct Cyc_Absyn_Var_d_struct*_tmp3A=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_struct));
_tmp3A[0]=({struct Cyc_Absyn_Var_d_struct _tmp3B;_tmp3B.tag=0;_tmp3B.f1=vd;_tmp3B;});
_tmp3A;});void*r2=(void*)r1;return Cyc_Absyn_new_decl(r2,seg);}}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_string(struct _tagged_arr s,struct Cyc_Position_Segment*seg){
return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_String_c_struct*_tmp3C=
_cycalloc(sizeof(struct Cyc_Absyn_String_c_struct));_tmp3C[0]=({struct Cyc_Absyn_String_c_struct
_tmp3D;_tmp3D.tag=5;_tmp3D.f1=s;_tmp3D;});_tmp3C;}),seg);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_string_cls(struct Cyc_Position_Segment*seg,struct _tagged_arr*s){
return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_String_c_struct*_tmp3E=
_cycalloc(sizeof(struct Cyc_Absyn_String_c_struct));_tmp3E[0]=({struct Cyc_Absyn_String_c_struct
_tmp3F;_tmp3F.tag=5;_tmp3F.f1=*s;_tmp3F;});_tmp3E;}),seg);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_int(int i,struct Cyc_Position_Segment*seg){return Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Int_c_struct*_tmp40=_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));
_tmp40[0]=({struct Cyc_Absyn_Int_c_struct _tmp41;_tmp41.tag=2;_tmp41.f1=(void*)((
void*)0);_tmp41.f2=i;_tmp41;});_tmp40;}),seg);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int_cls(
struct Cyc_Position_Segment*seg,int i){return Cyc_Absyn_const_exp((void*)({struct
Cyc_Absyn_Int_c_struct*_tmp42=_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));
_tmp42[0]=({struct Cyc_Absyn_Int_c_struct _tmp43;_tmp43.tag=2;_tmp43.f1=(void*)((
void*)0);_tmp43.f2=i;_tmp43;});_tmp42;}),seg);}static int Cyc_Tcgenrep_size_of2int(
void*sz){void*_tmp44=sz;_LL6: if((int)_tmp44 == 0){goto _LL7;}else{goto _LL8;}_LL8:
if((int)_tmp44 == 1){goto _LL9;}else{goto _LLA;}_LLA: if((int)_tmp44 == 2){goto _LLB;}
else{goto _LLC;}_LLC: if((int)_tmp44 == 3){goto _LLD;}else{goto _LL5;}_LL7: return 8;
_LL9: return 16;_LLB: return 32;_LLD: return 64;_LL5:;}static void*Cyc_Tcgenrep_array_typ(
void*ty,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp45=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp45[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp46;_tmp46.tag=7;_tmp46.f1=(void*)ty;_tmp46.f2=tq;_tmp46.f3=e;_tmp46;});
_tmp45;});}static void*Cyc_Tcgenrep_tunion_typ(struct _tuple3*name){return(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp47=_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));
_tmp47[0]=({struct Cyc_Absyn_TunionType_struct _tmp48;_tmp48.tag=2;_tmp48.f1=({
struct Cyc_Absyn_TunionInfo _tmp49;_tmp49.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp4A=_cycalloc(sizeof(struct Cyc_Absyn_UnknownTunion_struct));_tmp4A[0]=({
struct Cyc_Absyn_UnknownTunion_struct _tmp4B;_tmp4B.tag=0;_tmp4B.f1=({struct Cyc_Absyn_UnknownTunionInfo
_tmp4C;_tmp4C.name=name;_tmp4C.is_xtunion=0;_tmp4C;});_tmp4B;});_tmp4A;}));
_tmp49.targs=0;_tmp49.rgn=(void*)((void*)2);_tmp49;});_tmp48;});_tmp47;});}
static void*Cyc_Tcgenrep_tunionfield_typ(struct _tuple3*name,struct _tuple3*
fieldname){return(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp4D=
_cycalloc(sizeof(struct Cyc_Absyn_TunionFieldType_struct));_tmp4D[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp4E;_tmp4E.tag=3;_tmp4E.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp4F;_tmp4F.field_info=(
void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*_tmp50=_cycalloc(
sizeof(struct Cyc_Absyn_UnknownTunionfield_struct));_tmp50[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp51;_tmp51.tag=0;_tmp51.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp52;
_tmp52.tunion_name=name;_tmp52.field_name=fieldname;_tmp52.is_xtunion=0;_tmp52;});
_tmp51;});_tmp50;}));_tmp4F.targs=0;_tmp4F;});_tmp4E;});_tmp4D;});}static struct
Cyc_Absyn_Exp*Cyc_Tcgenrep_call_exp(struct _tuple3*name,struct Cyc_List_List*args,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_unknowncall_exp(Cyc_Absyn_unknownid_exp(
name,loc),args,loc);}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_tunion_constructor_decl(
struct _tuple3*tunionname,struct _tuple3*fieldname,struct _tuple3*varname,struct Cyc_List_List*
args,void*sc,struct Cyc_Position_Segment*loc){void*_tmp53=Cyc_Tcgenrep_tunionfield_typ(
tunionname,fieldname);struct Cyc_Absyn_Exp*_tmp54=Cyc_Tcgenrep_call_exp(fieldname,
args,loc);struct Cyc_Absyn_Decl*_tmp55=Cyc_Tcgenrep_gen_vardecl(varname,_tmp53,(
struct Cyc_Absyn_Exp*)_tmp54,sc,loc);return _tmp55;}struct _tuple6{struct Cyc_Absyn_Tqual
f1;void*f2;};static void*Cyc_Tcgenrep_get_second(struct _tuple6*pair){return(*pair).f2;}
struct _tuple7{struct _tagged_arr*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tagged_arr*Cyc_Tcgenrep_get_first(struct _tuple7*pair){return(*pair).f1;}static
unsigned char _tmp56[5]="list";static struct _tagged_arr Cyc_Tcgenrep_list_str={
_tmp56,_tmp56,_tmp56 + 5};static unsigned char _tmp57[5]="List";static struct
_tagged_arr Cyc_Tcgenrep_List_str={_tmp57,_tmp57,_tmp57 + 5};struct _tuple8{struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple2_exp_cls(
struct Cyc_Position_Segment*loc,struct _tuple8*es){struct _tuple8 _tmp59;struct Cyc_Absyn_Exp*
_tmp5A;struct Cyc_Absyn_Exp*_tmp5B;struct _tuple8*_tmp58=es;_tmp59=*_tmp58;_LL10:
_tmp5B=_tmp59.f1;goto _LLF;_LLF: _tmp5A=_tmp59.f2;goto _LLE;_LLE: return Cyc_Absyn_tuple_exp(({
struct Cyc_Absyn_Exp*_tmp5C[2];_tmp5C[1]=_tmp5A;_tmp5C[0]=_tmp5B;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp5C,sizeof(struct Cyc_Absyn_Exp*),2));}),
loc);}static unsigned char _tmp5D[5]="NULL";static struct _tagged_arr Cyc_Tcgenrep_null_str={
_tmp5D,_tmp5D,_tmp5D + 5};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_list_exp(struct
Cyc_List_List*l,struct Cyc_Position_Segment*loc){if(l == 0){return Cyc_Absyn_null_exp(
loc);}return Cyc_Tcgenrep_call_exp(Cyc_Tcgenrep_toplevel_name(& Cyc_Tcgenrep_list_str),
l,loc);}struct _tuple9{void*f1;struct Cyc_Position_Segment*f2;};static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_make_offsetof_exp(struct _tuple9*typeloc,int index){struct _tuple9
_tmp5F;struct Cyc_Position_Segment*_tmp60;void*_tmp61;struct _tuple9*_tmp5E=
typeloc;_tmp5F=*_tmp5E;_LL13: _tmp61=_tmp5F.f1;goto _LL12;_LL12: _tmp60=_tmp5F.f2;
goto _LL11;_LL11: return Cyc_Absyn_offsetof_exp(_tmp61,(void*)({struct Cyc_Absyn_TupleIndex_struct*
_tmp62=_cycalloc_atomic(sizeof(struct Cyc_Absyn_TupleIndex_struct));_tmp62[0]=({
struct Cyc_Absyn_TupleIndex_struct _tmp63;_tmp63.tag=1;_tmp63.f1=(unsigned int)
index;_tmp63;});_tmp62;}),_tmp60);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_get_and_cast_ri_exp(
struct Cyc_Position_Segment*loc,struct Cyc_Tcgenrep_RepInfo*info){return Cyc_Absyn_cast_exp(
Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str)),(
struct Cyc_Absyn_Exp*)_check_null(info->exp),loc);}static struct _tuple7*Cyc_Tcgenrep_gen_id_for_exp(
struct Cyc_Absyn_Exp*e){return({struct _tuple7*_tmp64=_cycalloc(sizeof(struct
_tuple7));_tmp64->f1=Cyc_Tcgenrep_new_gen_id(_tag_arr("tuple",sizeof(
unsigned char),6));_tmp64->f2=e;_tmp64;});}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl_cls(
struct _tuple9*env,struct _tuple7*name_exp){struct _tuple9 _tmp66;struct Cyc_Position_Segment*
_tmp67;void*_tmp68;struct _tuple9*_tmp65=env;_tmp66=*_tmp65;_LL16: _tmp68=_tmp66.f1;
goto _LL15;_LL15: _tmp67=_tmp66.f2;goto _LL14;_LL14: {struct _tuple7 _tmp6A;struct Cyc_Absyn_Exp*
_tmp6B;struct _tagged_arr*_tmp6C;struct _tuple7*_tmp69=name_exp;_tmp6A=*_tmp69;
_LL19: _tmp6C=_tmp6A.f1;goto _LL18;_LL18: _tmp6B=_tmp6A.f2;goto _LL17;_LL17: return
Cyc_Tcgenrep_gen_decl(_tmp6C,_tmp68,(struct Cyc_Absyn_Exp*)_tmp6B,_tmp67);}}
struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple10*Cyc_Tcgenrep_null_designator_exp(struct Cyc_Absyn_Exp*e){return({struct
_tuple10*_tmp6D=_cycalloc(sizeof(struct _tuple10));_tmp6D->f1=0;_tmp6D->f2=e;
_tmp6D;});}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_arr_init_exp(struct Cyc_List_List*
l,struct Cyc_Position_Segment*loc){struct Cyc_List_List*_tmp6E=((struct Cyc_List_List*(*)(
struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_null_designator_exp,l);return Cyc_Absyn_unresolvedmem_exp(0,_tmp6E,
loc);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_address_exp_cls(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_address_exp(e,loc);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_unknownid_exp_cls(struct Cyc_Position_Segment*loc,struct _tuple3*e){
return Cyc_Absyn_unknownid_exp(e,loc);}static int Cyc_Tcgenrep_has_bitfield(struct
Cyc_Absyn_Structfield*sf){return sf->width != 0;}static int Cyc_Tcgenrep_add_bitfield_sizes(
int total,struct Cyc_Absyn_Structfield*sf){return(int)(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(sf->width))+ total);}static void*Cyc_Tcgenrep_select_structfield_type(
struct Cyc_Absyn_Structfield*sf){if(Cyc_Std_strcmp(*sf->name,_tag_arr("",sizeof(
unsigned char),1))== 0){({void*_tmp6F[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): anonymous (padding) structfield not handled yet",
sizeof(unsigned char),55),_tag_arr(_tmp6F,sizeof(void*),0));});}if(Cyc_Tcgenrep_has_bitfield(
sf)){({void*_tmp70[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen(): mixed struct bitfields and union bitfields not handled",sizeof(
unsigned char),62),_tag_arr(_tmp70,sizeof(void*),0));});}return(void*)sf->type;}
static struct Cyc_Dict_Dict*Cyc_Tcgenrep_update_info(struct Cyc_Dict_Dict*dict,void*
type,struct Cyc_List_List*decls,struct Cyc_Absyn_Exp*exp,struct Cyc_List_List*
dependencies,struct Cyc_Core_Opt*fwd_decl){struct Cyc_Core_Opt*_tmp71=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(dict,type);if(_tmp71 != 0){if(((
struct Cyc_Tcgenrep_RepInfo*)_tmp71->v)->decls != 0){Cyc_Tcgenrep_print_typerep_dict(
dict);({void*_tmp72[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("Updating non-forward declaration",sizeof(unsigned char),33),_tag_arr(
_tmp72,sizeof(void*),0));});}((struct Cyc_Tcgenrep_RepInfo*)_tmp71->v)->decls=
decls;((struct Cyc_Tcgenrep_RepInfo*)_tmp71->v)->exp=exp;((struct Cyc_Tcgenrep_RepInfo*)
_tmp71->v)->dependencies=dependencies;return dict;}else{return((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(dict,
type,({struct Cyc_Tcgenrep_RepInfo*_tmp73=_cycalloc(sizeof(struct Cyc_Tcgenrep_RepInfo));
_tmp73->decls=decls;_tmp73->exp=exp;_tmp73->dependencies=dependencies;_tmp73->fwd_decl=
fwd_decl;_tmp73->emitted=0;_tmp73->is_extern=0;_tmp73;}));}}static struct Cyc_Dict_Dict*
Cyc_Tcgenrep_make_fwd_decl_info(struct Cyc_Dict_Dict*dict,void*type,struct Cyc_Absyn_Exp*
exp,struct Cyc_Core_Opt*fwd_decl,int is_extern){struct Cyc_Core_Opt*_tmp74=((struct
Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(dict,type);
if(_tmp74 != 0){({struct Cyc_Std_String_pa_struct _tmp76;_tmp76.tag=0;_tmp76.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(type);{void*_tmp75[1]={& _tmp76};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Repinfo for fwd declaration of %s already exists",
sizeof(unsigned char),49),_tag_arr(_tmp75,sizeof(void*),1));}});return dict;}
else{return((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(dict,type,({struct Cyc_Tcgenrep_RepInfo*_tmp77=_cycalloc(
sizeof(struct Cyc_Tcgenrep_RepInfo));_tmp77->decls=0;_tmp77->exp=exp;_tmp77->dependencies=
0;_tmp77->fwd_decl=fwd_decl;_tmp77->emitted=0;_tmp77->is_extern=is_extern;_tmp77;}));}}
static struct Cyc_Absyn_Tqual Cyc_Tcgenrep_tq_none={0,0,0};static struct _tuple6*Cyc_Tcgenrep_tqual_type(
struct Cyc_Absyn_Tqual*tq,void*type){return({struct _tuple6*_tmp78=_cycalloc(
sizeof(struct _tuple6));_tmp78->f1=*((struct Cyc_Absyn_Tqual*)_check_null(tq));
_tmp78->f2=type;_tmp78;});}static void*Cyc_Tcgenrep_tuple_typ(struct Cyc_List_List*
types){struct Cyc_List_List*_tmp79=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(
struct Cyc_Absyn_Tqual*,void*),struct Cyc_Absyn_Tqual*env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Tcgenrep_tqual_type,(struct Cyc_Absyn_Tqual*)& Cyc_Tcgenrep_tq_none,
types);struct Cyc_Absyn_TupleType_struct*tuple_type_base=({struct Cyc_Absyn_TupleType_struct*
_tmp7A=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_struct));_tmp7A[0]=({struct Cyc_Absyn_TupleType_struct
_tmp7B;_tmp7B.tag=9;_tmp7B.f1=_tmp79;_tmp7B;});_tmp7A;});void*tuple_type=(void*)
tuple_type_base;return tuple_type;}static struct _tuple10*Cyc_Tcgenrep_array_decls(
void*type,struct Cyc_List_List*exps,struct Cyc_Position_Segment*loc){struct Cyc_List_List*
_tmp7C=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_gen_id_for_exp,exps);struct Cyc_List_List*
_tmp7D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(struct _tuple9*,struct
_tuple7*),struct _tuple9*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_gen_decl_cls,({
struct _tuple9*_tmp8A=_cycalloc(sizeof(struct _tuple9));_tmp8A->f1=type;_tmp8A->f2=
loc;_tmp8A;}),_tmp7C);struct Cyc_List_List*_tmp7E=((struct Cyc_List_List*(*)(
struct _tagged_arr*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_first,
_tmp7C);struct Cyc_List_List*_tmp7F=((struct Cyc_List_List*(*)(struct _tuple3*(*f)(
struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_toplevel_name,
_tmp7E);struct Cyc_List_List*_tmp80=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple3*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_unknownid_exp_cls,loc,_tmp7F);
struct Cyc_List_List*_tmp81=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Exp*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_address_exp_cls,loc,_tmp80);
struct Cyc_Absyn_Exp*_tmp82=Cyc_Tcgenrep_arr_init_exp(_tmp81,loc);struct
_tagged_arr*_tmp83=Cyc_Tcgenrep_new_gen_id(_tag_arr("arr",sizeof(unsigned char),
4));void*_tmp84=Cyc_Absyn_at_typ(type,(void*)2,Cyc_Tcgenrep_tq_none);void*_tmp85=
Cyc_Tcgenrep_array_typ(_tmp84,Cyc_Tcgenrep_tq_none,0);struct Cyc_Absyn_Decl*
_tmp86=Cyc_Tcgenrep_gen_decl(_tmp83,_tmp85,(struct Cyc_Absyn_Exp*)_tmp82,loc);
struct Cyc_Absyn_Exp*_tmp87=Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmp83),loc);return({struct _tuple10*_tmp88=_cycalloc(sizeof(struct _tuple10));
_tmp88->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp7D,({struct Cyc_Absyn_Decl*_tmp89[1];_tmp89[0]=_tmp86;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp89,
sizeof(struct Cyc_Absyn_Decl*),1));}));_tmp88->f2=_tmp87;_tmp88;});}static void*
Cyc_Tcgenrep_check_tunionfield_and_get_type(struct Cyc_Absyn_Tunionfield*tuf){if(
tuf->tvs != 0){({void*_tmp8B[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield type varaibles not handled",
sizeof(unsigned char),46),_tag_arr(_tmp8B,sizeof(void*),0));});}return(void*)({
struct Cyc_Absyn_TupleType_struct*_tmp8C=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_struct));
_tmp8C[0]=({struct Cyc_Absyn_TupleType_struct _tmp8D;_tmp8D.tag=9;_tmp8D.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(({
struct _tuple6*_tmp8E[1];_tmp8E[0]=({struct _tuple6*_tmp8F=_cycalloc(sizeof(struct
_tuple6));_tmp8F->f1=Cyc_Tcgenrep_tq_none;_tmp8F->f2=Cyc_Absyn_uint_t;_tmp8F;});((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp8E,
sizeof(struct _tuple6*),1));}),tuf->typs);_tmp8D;});_tmp8C;});}struct _tuple11{
struct _tagged_arr*f1;void*f2;};static struct _tuple11*Cyc_Tcgenrep_check_xtunionfield_and_get_name_type(
struct Cyc_Absyn_Tunionfield*tuf){if(tuf->tvs != 0){({void*_tmp90[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield type varaibles not handled",
sizeof(unsigned char),46),_tag_arr(_tmp90,sizeof(void*),0));});}return({struct
_tuple11*_tmp91=_cycalloc(sizeof(struct _tuple11));_tmp91->f1=(*tuf->name).f2;
_tmp91->f2=(void*)({struct Cyc_Absyn_TupleType_struct*_tmp92=_cycalloc(sizeof(
struct Cyc_Absyn_TupleType_struct));_tmp92[0]=({struct Cyc_Absyn_TupleType_struct
_tmp93;_tmp93.tag=9;_tmp93.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple6*_tmp94[1];_tmp94[0]=({
struct _tuple6*_tmp95=_cycalloc(sizeof(struct _tuple6));_tmp95->f1=Cyc_Tcgenrep_tq_none;
_tmp95->f2=Cyc_Absyn_star_typ(Cyc_Absyn_uchar_t,(void*)2,Cyc_Tcgenrep_tq_none);
_tmp95;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp94,sizeof(struct _tuple6*),1));}),tuf->typs);_tmp93;});_tmp92;});_tmp91;});}
static int Cyc_Tcgenrep_filter_empty_tunionfield(struct Cyc_Absyn_Tunionfield*tuf){
return tuf->typs != 0;}static struct _tagged_arr*Cyc_Tcgenrep_get_tunionfield_name(
struct Cyc_Absyn_Tunionfield*tuf){return(*tuf->name).f2;}struct Cyc_Absyn_Structfield*
Cyc_Tcgenrep_substitute_structfield_type(struct Cyc_List_List*subst,struct Cyc_Absyn_Structfield*
sf){return({struct Cyc_Absyn_Structfield*_tmp96=_cycalloc(sizeof(struct Cyc_Absyn_Structfield));
_tmp96->name=sf->name;_tmp96->tq=sf->tq;_tmp96->type=(void*)Cyc_Tcutil_substitute(
subst,(void*)sf->type);_tmp96->width=sf->width;_tmp96->attributes=sf->attributes;
_tmp96;});}struct _tuple6*Cyc_Tcgenrep_substitute_tqual_type(struct Cyc_List_List*
subst,struct _tuple6*pair){struct _tuple6 _tmp98;void*_tmp99;struct Cyc_Absyn_Tqual
_tmp9A;struct _tuple6*_tmp97=pair;_tmp98=*_tmp97;_LL1C: _tmp9A=_tmp98.f1;goto _LL1B;
_LL1B: _tmp99=_tmp98.f2;goto _LL1A;_LL1A: return({struct _tuple6*_tmp9B=_cycalloc(
sizeof(struct _tuple6));_tmp9B->f1=_tmp9A;_tmp9B->f2=Cyc_Tcutil_substitute(subst,
_tmp99);_tmp9B;});}struct Cyc_Absyn_Tunionfield*Cyc_Tcgenrep_substitute_tunionfield_type(
struct Cyc_List_List*subst,struct Cyc_Absyn_Tunionfield*tf){return({struct Cyc_Absyn_Tunionfield*
_tmp9C=_cycalloc(sizeof(struct Cyc_Absyn_Tunionfield));_tmp9C->name=tf->name;
_tmp9C->tvs=tf->tvs;_tmp9C->typs=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(
struct Cyc_List_List*,struct _tuple6*),struct Cyc_List_List*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_tqual_type,subst,tf->typs);_tmp9C->loc=
tf->loc;_tmp9C->sc=(void*)((void*)tf->sc);_tmp9C;});}void*Cyc_Tcgenrep_monomorphize_type(
void*type){void*_tmp9D=Cyc_Tcutil_compress(type);struct Cyc_Absyn_Structdecl**
_tmp9E;struct Cyc_List_List*_tmp9F;struct _tuple3*_tmpA0;struct Cyc_Absyn_Uniondecl**
_tmpA1;struct Cyc_List_List*_tmpA2;struct _tuple3*_tmpA3;struct Cyc_Absyn_TunionInfo
_tmpA4;void*_tmpA5;struct Cyc_List_List*_tmpA6;void*_tmpA7;struct Cyc_Absyn_Tuniondecl**
_tmpA8;struct Cyc_Absyn_Tuniondecl*_tmpA9;_LL1E: if((unsigned int)_tmp9D > 3?*((int*)
_tmp9D)== 10: 0){_LL28: _tmpA0=((struct Cyc_Absyn_StructType_struct*)_tmp9D)->f1;
goto _LL27;_LL27: _tmp9F=((struct Cyc_Absyn_StructType_struct*)_tmp9D)->f2;goto
_LL26;_LL26: _tmp9E=((struct Cyc_Absyn_StructType_struct*)_tmp9D)->f3;goto _LL1F;}
else{goto _LL20;}_LL20: if((unsigned int)_tmp9D > 3?*((int*)_tmp9D)== 11: 0){_LL2B:
_tmpA3=((struct Cyc_Absyn_UnionType_struct*)_tmp9D)->f1;goto _LL2A;_LL2A: _tmpA2=((
struct Cyc_Absyn_UnionType_struct*)_tmp9D)->f2;goto _LL29;_LL29: _tmpA1=((struct Cyc_Absyn_UnionType_struct*)
_tmp9D)->f3;goto _LL21;}else{goto _LL22;}_LL22: if((unsigned int)_tmp9D > 3?*((int*)
_tmp9D)== 2: 0){_LL2C: _tmpA4=((struct Cyc_Absyn_TunionType_struct*)_tmp9D)->f1;
_LL2F: _tmpA7=(void*)_tmpA4.tunion_info;if(*((int*)_tmpA7)== 1){_LL30: _tmpA8=((
struct Cyc_Absyn_KnownTunion_struct*)_tmpA7)->f1;_tmpA9=*_tmpA8;goto _LL2E;}else{
goto _LL24;}_LL2E: _tmpA6=_tmpA4.targs;goto _LL2D;_LL2D: _tmpA5=(void*)_tmpA4.rgn;
goto _LL23;}else{goto _LL24;}_LL24: goto _LL25;_LL1F: if(_tmp9E == 0){({void*_tmpAA[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("struct decl must exist at this point",
sizeof(unsigned char),37),_tag_arr(_tmpAA,sizeof(void*),0));});}{struct Cyc_Absyn_Structdecl*
_tmpAB=*((struct Cyc_Absyn_Structdecl**)_check_null(_tmp9E));struct Cyc_List_List*
_tmpAC=_tmpAB->tvs;if(Cyc_List_length(_tmp9F)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmpAC)){({struct Cyc_Std_Int_pa_struct _tmpAF;_tmpAF.tag=1;
_tmpAF.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpAC));{struct Cyc_Std_Int_pa_struct _tmpAE;_tmpAE.tag=1;_tmpAE.f1=(int)((
unsigned int)Cyc_List_length(_tmp9F));{void*_tmpAD[2]={& _tmpAE,& _tmpAF};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof(unsigned char),60),_tag_arr(_tmpAD,sizeof(void*),2));}}});}{struct Cyc_List_List*
_tmpB0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmpAC,_tmp9F);struct Cyc_Core_Opt*fields=0;if(_tmpAB->fields != 0){
fields=({struct Cyc_Core_Opt*_tmpB1=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpB1->v=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_structfield_type,_tmpB0,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpAB->fields))->v);_tmpB1;});}{struct Cyc_Absyn_Structdecl*
stdecl2=({struct Cyc_Absyn_Structdecl*_tmpB7=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));
_tmpB7->sc=(void*)((void*)_tmpAB->sc);_tmpB7->name=_tmpAB->name;_tmpB7->tvs=0;
_tmpB7->fields=fields;_tmpB7->attributes=_tmpAB->attributes;_tmpB7;});struct Cyc_Absyn_Structdecl**
_tmpB2=({struct Cyc_Absyn_Structdecl**_tmpB6=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl*));
_tmpB6[0]=stdecl2;_tmpB6;});struct Cyc_Absyn_StructType_struct*_tmpB3=({struct Cyc_Absyn_StructType_struct*
_tmpB4=_cycalloc(sizeof(struct Cyc_Absyn_StructType_struct));_tmpB4[0]=({struct
Cyc_Absyn_StructType_struct _tmpB5;_tmpB5.tag=10;_tmpB5.f1=_tmpA0;_tmpB5.f2=
_tmp9F;_tmpB5.f3=_tmpB2;_tmpB5;});_tmpB4;});return(void*)_tmpB3;}}}_LL21: if(
_tmpA1 == 0){({void*_tmpB8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(_tag_arr("struct decl must exist at this point",sizeof(
unsigned char),37),_tag_arr(_tmpB8,sizeof(void*),0));});}{struct Cyc_Absyn_Uniondecl*
_tmpB9=*((struct Cyc_Absyn_Uniondecl**)_check_null(_tmpA1));struct Cyc_List_List*
_tmpBA=_tmpB9->tvs;if(Cyc_List_length(_tmpA2)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmpBA)){({struct Cyc_Std_Int_pa_struct _tmpBD;_tmpBD.tag=1;
_tmpBD.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpBA));{struct Cyc_Std_Int_pa_struct _tmpBC;_tmpBC.tag=1;_tmpBC.f1=(int)((
unsigned int)Cyc_List_length(_tmpA2));{void*_tmpBB[2]={& _tmpBC,& _tmpBD};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof(unsigned char),60),_tag_arr(_tmpBB,sizeof(void*),2));}}});}{struct Cyc_List_List*
_tmpBE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmpBA,_tmpA2);struct Cyc_Core_Opt*fields=0;if(_tmpB9->fields != 0){
fields=({struct Cyc_Core_Opt*_tmpBF=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpBF->v=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_structfield_type,_tmpBE,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpB9->fields))->v);_tmpBF;});}{struct Cyc_Absyn_Uniondecl*
udecl2=({struct Cyc_Absyn_Uniondecl*_tmpC5=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl));
_tmpC5->sc=(void*)((void*)_tmpB9->sc);_tmpC5->name=_tmpB9->name;_tmpC5->tvs=0;
_tmpC5->fields=fields;_tmpC5->attributes=_tmpB9->attributes;_tmpC5;});struct Cyc_Absyn_Uniondecl**
_tmpC0=({struct Cyc_Absyn_Uniondecl**_tmpC4=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl*));
_tmpC4[0]=udecl2;_tmpC4;});struct Cyc_Absyn_UnionType_struct*_tmpC1=({struct Cyc_Absyn_UnionType_struct*
_tmpC2=_cycalloc(sizeof(struct Cyc_Absyn_UnionType_struct));_tmpC2[0]=({struct Cyc_Absyn_UnionType_struct
_tmpC3;_tmpC3.tag=11;_tmpC3.f1=_tmpA3;_tmpC3.f2=_tmpA2;_tmpC3.f3=_tmpC0;_tmpC3;});
_tmpC2;});return(void*)_tmpC1;}}}_LL23: {struct Cyc_List_List*_tmpC6=_tmpA9->tvs;
if(Cyc_List_length(_tmpA6)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpC6)){({struct Cyc_Std_Int_pa_struct _tmpC9;_tmpC9.tag=1;_tmpC9.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC6));{struct
Cyc_Std_Int_pa_struct _tmpC8;_tmpC8.tag=1;_tmpC8.f1=(int)((unsigned int)Cyc_List_length(
_tmpA6));{void*_tmpC7[2]={& _tmpC8,& _tmpC9};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof(unsigned char),60),_tag_arr(_tmpC7,sizeof(void*),2));}}});}{struct Cyc_List_List*
_tmpCA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmpC6,_tmpA6);struct Cyc_Core_Opt*fields=0;if(_tmpA9->fields != 0){
fields=({struct Cyc_Core_Opt*_tmpCB=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpCB->v=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tunionfield*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_tunionfield_type,_tmpCA,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpA9->fields))->v);_tmpCB;});}{struct Cyc_Absyn_Tuniondecl*
_tmpCC=({struct Cyc_Absyn_Tuniondecl*_tmpD5=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));
_tmpD5->sc=(void*)((void*)_tmpA9->sc);_tmpD5->name=_tmpA9->name;_tmpD5->tvs=0;
_tmpD5->fields=fields;_tmpD5->is_xtunion=_tmpA9->is_xtunion;_tmpD5;});struct Cyc_Absyn_TunionType_struct*
_tmpCD=({struct Cyc_Absyn_TunionType_struct*_tmpCF=_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));
_tmpCF[0]=({struct Cyc_Absyn_TunionType_struct _tmpD0;_tmpD0.tag=2;_tmpD0.f1=({
struct Cyc_Absyn_TunionInfo _tmpD1;_tmpD1.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmpD2=_cycalloc(sizeof(struct Cyc_Absyn_KnownTunion_struct));_tmpD2[0]=({struct
Cyc_Absyn_KnownTunion_struct _tmpD3;_tmpD3.tag=1;_tmpD3.f1=({struct Cyc_Absyn_Tuniondecl**
_tmpD4=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl*));_tmpD4[0]=_tmpCC;_tmpD4;});
_tmpD3;});_tmpD2;}));_tmpD1.targs=_tmpA6;_tmpD1.rgn=(void*)_tmpA5;_tmpD1;});
_tmpD0;});_tmpCF;});struct Cyc_Position_Segment*_tmpCE=Cyc_Position_segment_of_abs(
0,0);return(void*)_tmpCD;}}}_LL25: return type;_LL1D:;}struct _tagged_arr Cyc_Tcgenrep_make_type_cstring(
void*t){struct _tagged_arr s=Cyc_Std_strdup(Cyc_Absynpp_typ2cstring(t));{int i=0;
for(0;i < Cyc_Std_strlen((struct _tagged_arr)s);i ++){if(((unsigned char*)s.curr)[i]
== ' '){((unsigned char*)s.curr)[i]='_';}else{if(! isalnum((int)((unsigned char*)
s.curr)[i])?((unsigned char*)s.curr)[i]!= '_': 0){((unsigned char*)s.curr)[i]=(
unsigned char)('0' + ((unsigned char*)s.curr)[i]% 10);}}}}return(struct
_tagged_arr)Cyc_Std_strconcat((struct _tagged_arr)s,_tag_arr("_rep",sizeof(
unsigned char),5));}struct _tuple12{struct Cyc_Dict_Dict*f1;struct Cyc_Tcgenrep_RepInfo*
f2;};static struct _tuple12*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,void*type);struct _tuple13{
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};struct _tuple14{struct Cyc_Tcenv_Tenv*
f1;struct Cyc_Position_Segment*f2;};static struct _tuple13*Cyc_Tcgenrep_lookupRepsCls(
struct _tuple14*env,void*type,struct _tuple13*carry){struct _tuple12 _tmpD7;struct
Cyc_Tcgenrep_RepInfo*_tmpD8;struct Cyc_Dict_Dict*_tmpD9;struct _tuple12*_tmpD6=Cyc_Tcgenrep_lookupRep((*
env).f1,(*carry).f1,(*env).f2,type);_tmpD7=*_tmpD6;_LL33: _tmpD9=_tmpD7.f1;goto
_LL32;_LL32: _tmpD8=_tmpD7.f2;goto _LL31;_LL31: return({struct _tuple13*_tmpDA=
_cycalloc(sizeof(struct _tuple13));_tmpDA->f1=_tmpD9;_tmpDA->f2=({struct Cyc_List_List*
_tmpDB=_cycalloc(sizeof(struct Cyc_List_List));_tmpDB->hd=_tmpD8;_tmpDB->tl=(*
carry).f2;_tmpDB;});_tmpDA;});}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepTuple(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,
struct _tuple3*varname,void*sc,void*type,struct Cyc_List_List*types){struct Cyc_Absyn_Exp*
_tmpDC=Cyc_Absyn_sizeoftyp_exp(type,loc);struct _tuple14*_tmpDD=({struct _tuple14*
_tmpF2=_cycalloc(sizeof(struct _tuple14));_tmpF2->f1=te;_tmpF2->f2=loc;_tmpF2;});
struct _tuple13*_tmpDE=({struct _tuple13*_tmpF1=_cycalloc(sizeof(struct _tuple13));
_tmpF1->f1=dict;_tmpF1->f2=0;_tmpF1;});struct _tuple13 _tmpE0;struct Cyc_List_List*
_tmpE1;struct Cyc_Dict_Dict*_tmpE2;struct _tuple13*_tmpDF=((struct _tuple13*(*)(
struct _tuple13*(*f)(struct _tuple14*,void*,struct _tuple13*),struct _tuple14*,
struct Cyc_List_List*x,struct _tuple13*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmpDD,types,_tmpDE);_tmpE0=*_tmpDF;_LL36: _tmpE2=_tmpE0.f1;goto _LL35;_LL35:
_tmpE1=_tmpE0.f2;goto _LL34;_LL34: dict=_tmpE2;{struct Cyc_List_List*_tmpE3=((
struct Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(Cyc_List_length(
types),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmpE4=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple9*,int),struct _tuple9*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({struct _tuple9*_tmpF0=
_cycalloc(sizeof(struct _tuple9));_tmpF0->f1=type;_tmpF0->f2=loc;_tmpF0;}),_tmpE3);
struct Cyc_List_List*_tmpE5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmpE1);struct Cyc_List_List*_tmpE6=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmpE4,_tmpE5));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmpEF[2];_tmpEF[1]=
Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmpEF[0]=Cyc_Absyn_uint_t;Cyc_List_list(_tag_arr(_tmpEF,sizeof(void*),2));}));
struct _tuple10 _tmpE8;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_List_List*_tmpEA;
struct _tuple10*_tmpE7=Cyc_Tcgenrep_array_decls(tuple_type,_tmpE6,loc);_tmpE8=*
_tmpE7;_LL39: _tmpEA=_tmpE8.f1;goto _LL38;_LL38: _tmpE9=_tmpE8.f2;goto _LL37;_LL37: {
struct Cyc_Absyn_Decl*_tmpEB=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str),
varname,({struct Cyc_Absyn_Exp*_tmpEE[2];_tmpEE[1]=_tmpE9;_tmpEE[0]=_tmpDC;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpEE,
sizeof(struct Cyc_Absyn_Exp*),2));}),sc,loc);struct Cyc_List_List*_tmpEC=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmpEA,({struct Cyc_Absyn_Decl*_tmpED[1];_tmpED[0]=_tmpEB;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpED,sizeof(struct Cyc_Absyn_Decl*),
1));}));return Cyc_Tcgenrep_update_info(dict,type,_tmpEC,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmpE1,0);}}}
static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepUnion(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,struct _tuple3*varname,
void*sc,void*type,struct Cyc_List_List*types){struct _tuple14*_tmpF3=({struct
_tuple14*_tmp102=_cycalloc(sizeof(struct _tuple14));_tmp102->f1=te;_tmp102->f2=
loc;_tmp102;});struct _tuple13*_tmpF4=({struct _tuple13*_tmp101=_cycalloc(sizeof(
struct _tuple13));_tmp101->f1=dict;_tmp101->f2=0;_tmp101;});struct _tuple13 _tmpF6;
struct Cyc_List_List*_tmpF7;struct Cyc_Dict_Dict*_tmpF8;struct _tuple13*_tmpF5=((
struct _tuple13*(*)(struct _tuple13*(*f)(struct _tuple14*,void*,struct _tuple13*),
struct _tuple14*,struct Cyc_List_List*x,struct _tuple13*accum))Cyc_List_fold_right_c)(
Cyc_Tcgenrep_lookupRepsCls,_tmpF3,types,_tmpF4);_tmpF6=*_tmpF5;_LL3C: _tmpF8=
_tmpF6.f1;goto _LL3B;_LL3B: _tmpF7=_tmpF6.f2;goto _LL3A;_LL3A: dict=_tmpF8;{struct
Cyc_List_List*_tmpF9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct
Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmpF7);struct _tuple10 _tmpFB;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_List_List*
_tmpFD;struct _tuple10*_tmpFA=Cyc_Tcgenrep_array_decls(Cyc_Tcgenrep_tunion_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str)),_tmpF9,loc);_tmpFB=*
_tmpFA;_LL3F: _tmpFD=_tmpFB.f1;goto _LL3E;_LL3E: _tmpFC=_tmpFB.f2;goto _LL3D;_LL3D: {
struct Cyc_Absyn_Decl*_tmpFE=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_union_str),
varname,({struct Cyc_Absyn_Exp*_tmp100[1];_tmp100[0]=_tmpFC;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp100,sizeof(struct Cyc_Absyn_Exp*),
1));}),sc,loc);return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpFD,({
struct Cyc_Absyn_Decl*_tmpFF[1];_tmpFF[0]=_tmpFE;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmpFF,sizeof(struct Cyc_Absyn_Decl*),1));})),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(varname,loc),
loc),_tmpF7,0);}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepTunion(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,struct
_tuple3*varname,void*sc,void*type,struct Cyc_List_List*types){struct _tuple14*
_tmp103=({struct _tuple14*_tmp117=_cycalloc(sizeof(struct _tuple14));_tmp117->f1=
te;_tmp117->f2=loc;_tmp117;});struct _tuple13*_tmp104=({struct _tuple13*_tmp116=
_cycalloc(sizeof(struct _tuple13));_tmp116->f1=dict;_tmp116->f2=0;_tmp116;});
struct _tuple13 _tmp106;struct Cyc_List_List*_tmp107;struct Cyc_Dict_Dict*_tmp108;
struct _tuple13*_tmp105=((struct _tuple13*(*)(struct _tuple13*(*f)(struct _tuple14*,
void*,struct _tuple13*),struct _tuple14*,struct Cyc_List_List*x,struct _tuple13*
accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp103,types,_tmp104);
_tmp106=*_tmp105;_LL42: _tmp108=_tmp106.f1;goto _LL41;_LL41: _tmp107=_tmp106.f2;
goto _LL40;_LL40: dict=_tmp108;{struct Cyc_List_List*_tmp109=((struct Cyc_List_List*(*)(
int n,int(*f)(int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp107),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp10A=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp109);
struct Cyc_List_List*_tmp10B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp107);struct Cyc_List_List*_tmp10C=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp10A,_tmp10B);struct Cyc_List_List*
_tmp10D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp10C);void*tuple_type=Cyc_Tcgenrep_tuple_typ(({
void*_tmp115[2];_tmp115[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str));_tmp115[0]=Cyc_Absyn_uint_t;Cyc_List_list(_tag_arr(
_tmp115,sizeof(void*),2));}));struct _tuple10 _tmp10F;struct Cyc_Absyn_Exp*_tmp110;
struct Cyc_List_List*_tmp111;struct _tuple10*_tmp10E=Cyc_Tcgenrep_array_decls(
tuple_type,_tmp10D,loc);_tmp10F=*_tmp10E;_LL45: _tmp111=_tmp10F.f1;goto _LL44;
_LL44: _tmp110=_tmp10F.f2;goto _LL43;_LL43: {struct Cyc_Absyn_Decl*_tmp112=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tunion_str),varname,({struct Cyc_Absyn_Exp*_tmp114[1];_tmp114[0]=
_tmp110;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp114,sizeof(struct Cyc_Absyn_Exp*),1));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp111,({struct Cyc_Absyn_Decl*_tmp113[1];_tmp113[0]=
_tmp112;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp113,sizeof(struct Cyc_Absyn_Decl*),1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp107,0);return dict;}}}static struct
Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepXTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*
dict,struct Cyc_Position_Segment*loc,struct _tuple3*varname,void*sc,void*type,
struct Cyc_List_List*nametypes){struct Cyc_List_List*_tmp119;struct Cyc_List_List*
_tmp11A;struct _tuple0 _tmp118=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
nametypes);_LL48: _tmp11A=_tmp118.f1;goto _LL47;_LL47: _tmp119=_tmp118.f2;goto _LL46;
_LL46: {struct _tuple14*_tmp11B=({struct _tuple14*_tmp12F=_cycalloc(sizeof(struct
_tuple14));_tmp12F->f1=te;_tmp12F->f2=loc;_tmp12F;});struct _tuple13*_tmp11C=({
struct _tuple13*_tmp12E=_cycalloc(sizeof(struct _tuple13));_tmp12E->f1=dict;
_tmp12E->f2=0;_tmp12E;});struct _tuple13 _tmp11E;struct Cyc_List_List*_tmp11F;
struct Cyc_Dict_Dict*_tmp120;struct _tuple13*_tmp11D=((struct _tuple13*(*)(struct
_tuple13*(*f)(struct _tuple14*,void*,struct _tuple13*),struct _tuple14*,struct Cyc_List_List*
x,struct _tuple13*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp11B,_tmp119,_tmp11C);_tmp11E=*_tmp11D;_LL4B: _tmp120=_tmp11E.f1;goto _LL4A;
_LL4A: _tmp11F=_tmp11E.f2;goto _LL49;_LL49: dict=_tmp120;{struct Cyc_List_List*
_tmp121=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tagged_arr*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_cnst_string_cls,loc,_tmp11A);struct Cyc_List_List*_tmp122=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,
loc,_tmp11F);struct Cyc_List_List*_tmp123=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp121,_tmp122);struct Cyc_List_List*
_tmp124=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp123);void*name_type=Cyc_Absyn_tagged_typ(Cyc_Absyn_uchar_t,(
void*)2,({struct Cyc_Absyn_Tqual _tmp12D;_tmp12D.q_const=1;_tmp12D.q_volatile=0;
_tmp12D.q_restrict=0;_tmp12D;}));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*
_tmp12C[2];_tmp12C[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp12C[0]=name_type;Cyc_List_list(_tag_arr(_tmp12C,sizeof(void*),2));}));struct
_tuple10 _tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_List_List*_tmp128;struct
_tuple10*_tmp125=Cyc_Tcgenrep_array_decls(tuple_type,_tmp124,loc);_tmp126=*
_tmp125;_LL4E: _tmp128=_tmp126.f1;goto _LL4D;_LL4D: _tmp127=_tmp126.f2;goto _LL4C;
_LL4C: {struct Cyc_Absyn_Decl*_tmp129=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_xtunion_str),
varname,({struct Cyc_Absyn_Exp*_tmp12B[1];_tmp12B[0]=_tmp127;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp12B,sizeof(struct Cyc_Absyn_Exp*),
1));}),sc,loc);return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp128,({
struct Cyc_Absyn_Decl*_tmp12A[1];_tmp12A[0]=_tmp129;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp12A,sizeof(struct Cyc_Absyn_Decl*),
1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp11F,0);return dict;}}}}static struct _tuple12*Cyc_Tcgenrep_lookupRep(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,
void*type){struct Cyc_Core_Opt*info=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
d,void*k))Cyc_Dict_lookup_opt)(dict,type);if(info != 0){if(((struct Cyc_Tcgenrep_RepInfo*)
info->v)->is_extern){dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,void*))
Cyc_Dict_delete)(dict,type);}else{struct _tuple12*_tmp130=({struct _tuple12*
_tmp131=_cycalloc(sizeof(struct _tuple12));_tmp131->f1=dict;_tmp131->f2=(struct
Cyc_Tcgenrep_RepInfo*)info->v;_tmp131;});return _tmp130;}}{void*_tmp132=Cyc_Tcutil_compress(
Cyc_Tcgenrep_monomorphize_type(type));void*_tmp133;int _tmp134;struct Cyc_Absyn_PtrInfo
_tmp135;struct Cyc_Absyn_Exp*_tmp136;struct Cyc_Absyn_Tqual _tmp137;void*_tmp138;
struct Cyc_List_List*_tmp139;struct Cyc_Core_Opt*_tmp13A;struct Cyc_List_List*
_tmp13B;struct _tuple3*_tmp13C;struct Cyc_Absyn_Structdecl**_tmp13D;struct Cyc_List_List*
_tmp13E;struct _tuple3*_tmp13F;struct Cyc_List_List*_tmp140;struct Cyc_Absyn_Tvar*
_tmp141;struct Cyc_Absyn_FnInfo _tmp142;struct Cyc_Absyn_TunionInfo _tmp143;void*
_tmp144;struct Cyc_List_List*_tmp145;void*_tmp146;struct Cyc_Absyn_Tuniondecl**
_tmp147;struct Cyc_Absyn_Tuniondecl*_tmp148;struct Cyc_Absyn_TunionFieldInfo
_tmp149;struct Cyc_Absyn_Uniondecl**_tmp14A;struct Cyc_List_List*_tmp14B;struct
_tuple3*_tmp14C;struct Cyc_List_List*_tmp14D;_LL50: if((unsigned int)_tmp132 > 3?*((
int*)_tmp132)== 5: 0){_LL84: _tmp133=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp132)->f2;goto _LL51;}else{goto _LL52;}_LL52: if((int)_tmp132 == 1){goto _LL53;}
else{goto _LL54;}_LL54: if((unsigned int)_tmp132 > 3?*((int*)_tmp132)== 6: 0){_LL85:
_tmp134=((struct Cyc_Absyn_DoubleType_struct*)_tmp132)->f1;goto _LL55;}else{goto
_LL56;}_LL56: if((unsigned int)_tmp132 > 3?*((int*)_tmp132)== 4: 0){_LL86: _tmp135=((
struct Cyc_Absyn_PointerType_struct*)_tmp132)->f1;goto _LL57;}else{goto _LL58;}
_LL58: if((unsigned int)_tmp132 > 3?*((int*)_tmp132)== 7: 0){_LL89: _tmp138=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp132)->f1;goto _LL88;_LL88: _tmp137=((struct
Cyc_Absyn_ArrayType_struct*)_tmp132)->f2;goto _LL87;_LL87: _tmp136=((struct Cyc_Absyn_ArrayType_struct*)
_tmp132)->f3;goto _LL59;}else{goto _LL5A;}_LL5A: if((unsigned int)_tmp132 > 3?*((int*)
_tmp132)== 9: 0){_LL8A: _tmp139=((struct Cyc_Absyn_TupleType_struct*)_tmp132)->f1;
goto _LL5B;}else{goto _LL5C;}_LL5C: if((unsigned int)_tmp132 > 3?*((int*)_tmp132)== 
18: 0){_LL8D: _tmp13C=((struct Cyc_Absyn_TypedefType_struct*)_tmp132)->f1;goto _LL8C;
_LL8C: _tmp13B=((struct Cyc_Absyn_TypedefType_struct*)_tmp132)->f2;goto _LL8B;_LL8B:
_tmp13A=((struct Cyc_Absyn_TypedefType_struct*)_tmp132)->f3;goto _LL5D;}else{goto
_LL5E;}_LL5E: if((unsigned int)_tmp132 > 3?*((int*)_tmp132)== 10: 0){_LL90: _tmp13F=((
struct Cyc_Absyn_StructType_struct*)_tmp132)->f1;goto _LL8F;_LL8F: _tmp13E=((struct
Cyc_Absyn_StructType_struct*)_tmp132)->f2;goto _LL8E;_LL8E: _tmp13D=((struct Cyc_Absyn_StructType_struct*)
_tmp132)->f3;goto _LL5F;}else{goto _LL60;}_LL60: if((unsigned int)_tmp132 > 3?*((int*)
_tmp132)== 14: 0){_LL91: _tmp140=((struct Cyc_Absyn_AnonStructType_struct*)_tmp132)->f1;
goto _LL61;}else{goto _LL62;}_LL62: if((int)_tmp132 == 0){goto _LL63;}else{goto _LL64;}
_LL64: if((unsigned int)_tmp132 > 3?*((int*)_tmp132)== 0: 0){goto _LL65;}else{goto
_LL66;}_LL66: if((unsigned int)_tmp132 > 3?*((int*)_tmp132)== 1: 0){_LL92: _tmp141=((
struct Cyc_Absyn_VarType_struct*)_tmp132)->f1;goto _LL67;}else{goto _LL68;}_LL68:
if((unsigned int)_tmp132 > 3?*((int*)_tmp132)== 8: 0){_LL93: _tmp142=((struct Cyc_Absyn_FnType_struct*)
_tmp132)->f1;goto _LL69;}else{goto _LL6A;}_LL6A: if((unsigned int)_tmp132 > 3?*((int*)
_tmp132)== 17: 0){goto _LL6B;}else{goto _LL6C;}_LL6C: if((int)_tmp132 == 2){goto _LL6D;}
else{goto _LL6E;}_LL6E: if((unsigned int)_tmp132 > 3?*((int*)_tmp132)== 19: 0){goto
_LL6F;}else{goto _LL70;}_LL70: if((unsigned int)_tmp132 > 3?*((int*)_tmp132)== 20: 0){
goto _LL71;}else{goto _LL72;}_LL72: if((unsigned int)_tmp132 > 3?*((int*)_tmp132)== 
21: 0){goto _LL73;}else{goto _LL74;}_LL74: if((unsigned int)_tmp132 > 3?*((int*)
_tmp132)== 2: 0){_LL94: _tmp143=((struct Cyc_Absyn_TunionType_struct*)_tmp132)->f1;
_LL97: _tmp146=(void*)_tmp143.tunion_info;if(*((int*)_tmp146)== 1){_LL98: _tmp147=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp146)->f1;_tmp148=*_tmp147;goto _LL96;}
else{goto _LL76;}_LL96: _tmp145=_tmp143.targs;goto _LL95;_LL95: _tmp144=(void*)
_tmp143.rgn;goto _LL75;}else{goto _LL76;}_LL76: if((unsigned int)_tmp132 > 3?*((int*)
_tmp132)== 2: 0){goto _LL77;}else{goto _LL78;}_LL78: if((unsigned int)_tmp132 > 3?*((
int*)_tmp132)== 3: 0){_LL99: _tmp149=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp132)->f1;goto _LL79;}else{goto _LL7A;}_LL7A: if((unsigned int)_tmp132 > 3?*((int*)
_tmp132)== 11: 0){_LL9C: _tmp14C=((struct Cyc_Absyn_UnionType_struct*)_tmp132)->f1;
goto _LL9B;_LL9B: _tmp14B=((struct Cyc_Absyn_UnionType_struct*)_tmp132)->f2;goto
_LL9A;_LL9A: _tmp14A=((struct Cyc_Absyn_UnionType_struct*)_tmp132)->f3;goto _LL7B;}
else{goto _LL7C;}_LL7C: if((unsigned int)_tmp132 > 3?*((int*)_tmp132)== 15: 0){_LL9D:
_tmp14D=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp132)->f1;goto _LL7D;}else{
goto _LL7E;}_LL7E: if((unsigned int)_tmp132 > 3?*((int*)_tmp132)== 12: 0){goto _LL7F;}
else{goto _LL80;}_LL80: if((unsigned int)_tmp132 > 3?*((int*)_tmp132)== 16: 0){goto
_LL81;}else{goto _LL82;}_LL82: if((unsigned int)_tmp132 > 3?*((int*)_tmp132)== 13: 0){
goto _LL83;}else{goto _LL4F;}_LL51: {struct _tuple3*_tmp14E=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",sizeof(unsigned char),4)));struct Cyc_Absyn_Decl*
_tmp14F=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_int_str),_tmp14E,({struct Cyc_Absyn_Exp*
_tmp151[1];_tmp151[0]=Cyc_Tcgenrep_cnst_int(Cyc_Tcgenrep_size_of2int(_tmp133),
loc);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp151,sizeof(struct Cyc_Absyn_Exp*),1));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(
dict,type,({struct Cyc_Absyn_Decl*_tmp150[1];_tmp150[0]=_tmp14F;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp150,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp14E,loc),loc),0,0);goto _LL4F;}_LL53: dict=Cyc_Tcgenrep_update_info(dict,type,
0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_float_str),
loc),0,0);goto _LL4F;_LL55: dict=Cyc_Tcgenrep_update_info(dict,type,0,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_double_str),loc),
0,0);goto _LL4F;_LL57:{void*_tmp152=Cyc_Absyn_conref_val(_tmp135.bounds);struct
Cyc_Absyn_Exp*_tmp153;_LL9F: if((unsigned int)_tmp152 > 1?*((int*)_tmp152)== 0: 0){
_LLA3: _tmp153=((struct Cyc_Absyn_Upper_b_struct*)_tmp152)->f1;goto _LLA0;}else{
goto _LLA1;}_LLA1: if((int)_tmp152 == 0){goto _LLA2;}else{goto _LL9E;}_LLA0: {struct
_tuple3*_tmp154=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",
sizeof(unsigned char),4)));struct Cyc_Core_Opt*_tmp155=({struct Cyc_Core_Opt*
_tmp15E=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp15E->v=Cyc_Tcgenrep_gen_vardecl(
_tmp154,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str)),0,(void*)3,loc);_tmp15E;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp154,loc),loc),_tmp155,0);{struct _tuple12 _tmp157;
struct Cyc_Tcgenrep_RepInfo*_tmp158;struct Cyc_Dict_Dict*_tmp159;struct _tuple12*
_tmp156=Cyc_Tcgenrep_lookupRep(te,dict,loc,(void*)_tmp135.elt_typ);_tmp157=*
_tmp156;_LLA6: _tmp159=_tmp157.f1;goto _LLA5;_LLA5: _tmp158=_tmp157.f2;goto _LLA4;
_LLA4: dict=_tmp159;{unsigned int bound=Cyc_Evexp_eval_const_uint_exp(_tmp153);
struct Cyc_Absyn_Decl*_tmp15A=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str),
_tmp154,({struct Cyc_Absyn_Exp*_tmp15D[2];_tmp15D[1]=(struct Cyc_Absyn_Exp*)
_check_null(_tmp158->exp);_tmp15D[0]=Cyc_Tcgenrep_cnst_int((int)bound,loc);((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp15D,
sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(
dict,type,({struct Cyc_Absyn_Decl*_tmp15B[1];_tmp15B[0]=_tmp15A;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp15B,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp154,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp15C[1];_tmp15C[0]=_tmp158;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp15C,
sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL9E;}}}_LLA2: {struct _tuple3*
_tmp15F=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",
sizeof(unsigned char),4)));struct Cyc_Core_Opt*_tmp160=({struct Cyc_Core_Opt*
_tmp169=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp169->v=Cyc_Tcgenrep_gen_vardecl(
_tmp15F,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str)),0,(void*)3,loc);_tmp169;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp15F,loc),loc),_tmp160,0);{struct _tuple12 _tmp162;
struct Cyc_Tcgenrep_RepInfo*_tmp163;struct Cyc_Dict_Dict*_tmp164;struct _tuple12*
_tmp161=Cyc_Tcgenrep_lookupRep(te,dict,loc,(void*)_tmp135.elt_typ);_tmp162=*
_tmp161;_LLA9: _tmp164=_tmp162.f1;goto _LLA8;_LLA8: _tmp163=_tmp162.f2;goto _LLA7;
_LLA7: dict=_tmp164;{struct Cyc_Absyn_Decl*_tmp165=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_fatptr_str),_tmp15F,({struct Cyc_Absyn_Exp*_tmp168[1];_tmp168[0]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp163->exp);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp168,sizeof(struct Cyc_Absyn_Exp*),1));}),(
void*)0,loc);dict=Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*
_tmp166[1];_tmp166[0]=_tmp165;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp166,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp15F,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp167[1];
_tmp167[0]=_tmp163;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp167,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL9E;}}}
_LL9E:;}goto _LL4F;_LL59: if(_tmp136 == 0){({void*_tmp16A[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcgenrep:At this point, array bounds must be constants",
sizeof(unsigned char),55),_tag_arr(_tmp16A,sizeof(void*),0));});}{struct _tuple12
_tmp16C;struct Cyc_Tcgenrep_RepInfo*_tmp16D;struct Cyc_Dict_Dict*_tmp16E;struct
_tuple12*_tmp16B=Cyc_Tcgenrep_lookupRep(te,dict,loc,_tmp138);_tmp16C=*_tmp16B;
_LLAC: _tmp16E=_tmp16C.f1;goto _LLAB;_LLAB: _tmp16D=_tmp16C.f2;goto _LLAA;_LLAA: dict=
_tmp16E;{unsigned int bound=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp136));struct _tuple3*_tmp16F=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(unsigned char),4)));struct Cyc_Absyn_Decl*_tmp170=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str),_tmp16F,({struct Cyc_Absyn_Exp*_tmp173[2];_tmp173[1]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp16D->exp);_tmp173[0]=Cyc_Tcgenrep_cnst_int((
int)bound,loc);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp173,sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(
dict,type,({struct Cyc_Absyn_Decl*_tmp171[1];_tmp171[0]=_tmp170;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp171,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp16F,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp172[1];_tmp172[0]=_tmp16D;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp172,
sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL4F;}}_LL5B: {struct _tuple3*
_tmp174=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",
sizeof(unsigned char),4)));struct Cyc_Core_Opt*_tmp175=({struct Cyc_Core_Opt*
_tmp177=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp177->v=Cyc_Tcgenrep_gen_vardecl(
_tmp174,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str)),0,(void*)3,loc);_tmp177;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp174,loc),loc),_tmp175,0);{struct Cyc_List_List*
_tmp176=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple6*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcgenrep_get_second,_tmp139);dict=Cyc_Tcgenrep_buildRepTuple(
te,dict,loc,_tmp174,(void*)0,type,_tmp176);goto _LL4F;}}_LL5D: if(_tmp13A == 0){({
void*_tmp178[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen(): typedef definition must be present by now",sizeof(unsigned char),
49),_tag_arr(_tmp178,sizeof(void*),0));});}{struct _tuple12 _tmp17A;struct Cyc_Tcgenrep_RepInfo*
_tmp17B;struct Cyc_Dict_Dict*_tmp17C;struct _tuple12*_tmp179=Cyc_Tcgenrep_lookupRep(
te,dict,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp13A))->v);_tmp17A=*
_tmp179;_LLAF: _tmp17C=_tmp17A.f1;goto _LLAE;_LLAE: _tmp17B=_tmp17A.f2;goto _LLAD;
_LLAD: dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(_tmp17C,type,_tmp17B);goto _LL4F;}_LL5F: if(_tmp13D == 0){({
void*_tmp17D[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen(): struct declaration must be present by now",sizeof(unsigned char),
49),_tag_arr(_tmp17D,sizeof(void*),0));});}{struct Cyc_Absyn_Structdecl*_tmp17E=*((
struct Cyc_Absyn_Structdecl**)_check_null(_tmp13D));if(_tmp17E->fields == 0){
struct _tuple3*_tmp17F=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp182=
_cycalloc(sizeof(struct _tagged_arr));_tmp182[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp182;}));struct Cyc_Core_Opt*_tmp180=({struct Cyc_Core_Opt*_tmp181=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp181->v=Cyc_Tcgenrep_gen_vardecl(
_tmp17F,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str)),0,(void*)3,loc);_tmp181;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp17F,loc),loc),0,1);goto _LL4F;}if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp17E->fields))->v != 0?((int(*)(int(*pred)(
struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp17E->fields))->v): 0){
int _tmp183=((int(*)(int(*f)(int,struct Cyc_Absyn_Structfield*),int accum,struct Cyc_List_List*
x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,0,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp17E->fields))->v);int _tmp184=_tmp183 / 8 + (
_tmp183 % 8 == 0? 0: 1);struct Cyc_List_List*chars=0;{int i=0;for(0;i < _tmp184;i ++){
chars=({struct Cyc_List_List*_tmp185=_cycalloc(sizeof(struct Cyc_List_List));
_tmp185->hd=({struct _tuple6*_tmp186=_cycalloc(sizeof(struct _tuple6));_tmp186->f1=
Cyc_Tcgenrep_tq_none;_tmp186->f2=Cyc_Absyn_uchar_t;_tmp186;});_tmp185->tl=chars;
_tmp185;});}}{void*base_type=(void*)({struct Cyc_Absyn_TupleType_struct*_tmp18B=
_cycalloc(sizeof(struct Cyc_Absyn_TupleType_struct));_tmp18B[0]=({struct Cyc_Absyn_TupleType_struct
_tmp18C;_tmp18C.tag=9;_tmp18C.f1=chars;_tmp18C;});_tmp18B;});struct _tuple12
_tmp188;struct Cyc_Tcgenrep_RepInfo*_tmp189;struct Cyc_Dict_Dict*_tmp18A;struct
_tuple12*_tmp187=Cyc_Tcgenrep_lookupRep(te,dict,loc,base_type);_tmp188=*_tmp187;
_LLB2: _tmp18A=_tmp188.f1;goto _LLB1;_LLB1: _tmp189=_tmp188.f2;goto _LLB0;_LLB0: dict=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(_tmp18A,type,_tmp189);}}else{struct _tuple3*_tmp18D=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp191=_cycalloc(sizeof(struct _tagged_arr));_tmp191[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp191;}));struct Cyc_Core_Opt*_tmp18E=({struct Cyc_Core_Opt*_tmp190=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp190->v=Cyc_Tcgenrep_gen_vardecl(
_tmp18D,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str)),0,(void*)3,loc);_tmp190;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp18D,loc),loc),_tmp18E,0);{struct Cyc_List_List*
_tmp18F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Structfield*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_type,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp17E->fields))->v);dict=Cyc_Tcgenrep_buildRepTuple(
te,dict,loc,_tmp18D,(void*)2,type,_tmp18F);}}goto _LL4F;}_LL61: if(_tmp140 != 0?((
int(*)(int(*pred)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))Cyc_List_forall)(
Cyc_Tcgenrep_has_bitfield,_tmp140): 0){int _tmp192=((int(*)(int(*f)(int,struct Cyc_Absyn_Structfield*),
int accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,
0,_tmp140);int _tmp193=_tmp192 / 8 + (_tmp192 % 8 == 0? 0: 1);void*base_type=(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmp198=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));
_tmp198[0]=({struct Cyc_Absyn_ArrayType_struct _tmp199;_tmp199.tag=7;_tmp199.f1=(
void*)Cyc_Absyn_uchar_t;_tmp199.f2=Cyc_Tcgenrep_tq_none;_tmp199.f3=(struct Cyc_Absyn_Exp*)
Cyc_Tcgenrep_cnst_int(_tmp193,loc);_tmp199;});_tmp198;});struct _tuple12 _tmp195;
struct Cyc_Tcgenrep_RepInfo*_tmp196;struct Cyc_Dict_Dict*_tmp197;struct _tuple12*
_tmp194=Cyc_Tcgenrep_lookupRep(te,dict,loc,base_type);_tmp195=*_tmp194;_LLB5:
_tmp197=_tmp195.f1;goto _LLB4;_LLB4: _tmp196=_tmp195.f2;goto _LLB3;_LLB3: dict=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(_tmp197,type,_tmp196);}else{struct Cyc_List_List*_tmp19A=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_type,_tmp140);struct _tuple3*
_tmp19B=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",
sizeof(unsigned char),4)));dict=Cyc_Tcgenrep_buildRepTuple(te,dict,loc,_tmp19B,(
void*)0,type,_tmp19A);}goto _LL4F;_LL63:({void*_tmp19C[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("found void in gen() expression",sizeof(unsigned char),31),_tag_arr(
_tmp19C,sizeof(void*),0));});goto _LL4F;_LL65:({void*_tmp19D[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("found evar in gen() expression",sizeof(unsigned char),31),_tag_arr(
_tmp19D,sizeof(void*),0));});goto _LL4F;_LL67:({void*_tmp19E[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("found tyvar in gen() expression",sizeof(unsigned char),32),
_tag_arr(_tmp19E,sizeof(void*),0));});goto _LL4F;({void*_tmp19F[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("found variable type in gen() expression",sizeof(unsigned char),40),
_tag_arr(_tmp19F,sizeof(void*),0));});goto _LL4F;_LL69:({void*_tmp1A0[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("found function type in gen() expression",sizeof(unsigned char),40),
_tag_arr(_tmp1A0,sizeof(void*),0));});goto _LL4F;_LL6B: goto _LL6D;_LL6D: goto _LL6F;
_LL6F: goto _LL71;_LL71: goto _LL73;_LL73:({void*_tmp1A1[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("gen(): unhandled region, handle or effect type",sizeof(unsigned char),
47),_tag_arr(_tmp1A1,sizeof(void*),0));});goto _LL4F;_LL75: if(_tmp148->tvs != 0){({
void*_tmp1A2[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen(): tunion type variables not handled yet",sizeof(unsigned char),45),
_tag_arr(_tmp1A2,sizeof(void*),0));});}if(_tmp148->fields == 0){struct _tuple3*
_tmp1A3=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp1A6=_cycalloc(sizeof(
struct _tagged_arr));_tmp1A6[0]=Cyc_Tcgenrep_make_type_cstring(type);_tmp1A6;}));
struct Cyc_Core_Opt*_tmp1A4=({struct Cyc_Core_Opt*_tmp1A5=_cycalloc(sizeof(struct
Cyc_Core_Opt));_tmp1A5->v=Cyc_Tcgenrep_gen_vardecl(_tmp1A3,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tunion_str)),0,(void*)3,loc);_tmp1A5;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1A3,loc),loc),0,1);goto _LL4F;}if(! _tmp148->is_xtunion){struct _tuple3*_tmp1A7=
Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp1AD=_cycalloc(sizeof(struct
_tagged_arr));_tmp1AD[0]=Cyc_Tcgenrep_make_type_cstring(type);_tmp1AD;}));struct
Cyc_Core_Opt*_tmp1A8=({struct Cyc_Core_Opt*_tmp1AC=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp1AC->v=Cyc_Tcgenrep_gen_vardecl(_tmp1A7,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str)),
0,(void*)3,loc);_tmp1AC;});dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp1A7,loc),
loc),_tmp1A8,0);{struct Cyc_List_List*_tmp1A9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp148->fields))->v;struct Cyc_List_List*_tmp1AA=((struct Cyc_List_List*(*)(
int(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_filter)(
Cyc_Tcgenrep_filter_empty_tunionfield,_tmp1A9);struct Cyc_List_List*_tmp1AB=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcgenrep_check_tunionfield_and_get_type,_tmp1AA);dict=Cyc_Tcgenrep_buildRepTunion(
te,dict,loc,_tmp1A7,(void*)2,type,_tmp1AB);}}else{struct _tuple3*_tmp1AE=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp1B4=_cycalloc(sizeof(struct _tagged_arr));_tmp1B4[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp1B4;}));struct Cyc_Core_Opt*_tmp1AF=({struct Cyc_Core_Opt*_tmp1B3=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1B3->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1AE,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_xtunion_str)),0,(void*)3,loc);_tmp1B3;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1AE,loc),loc),_tmp1AF,0);{struct Cyc_List_List*
_tmp1B0=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp148->fields))->v;
struct Cyc_List_List*_tmp1B1=((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_tunionfield,
_tmp1B0);struct Cyc_List_List*_tmp1B2=((struct Cyc_List_List*(*)(struct _tuple11*(*
f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_xtunionfield_and_get_name_type,
_tmp1B1);dict=Cyc_Tcgenrep_buildRepXTunion(te,dict,loc,_tmp1AE,(void*)2,type,
_tmp1B2);}}goto _LL4F;_LL77:({void*_tmp1B5[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunion must be resolved by now",
sizeof(unsigned char),38),_tag_arr(_tmp1B5,sizeof(void*),0));});goto _LL4F;_LL79:
if(_tmp149.targs != 0){({void*_tmp1B6[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield type parameters not handled yet",
sizeof(unsigned char),51),_tag_arr(_tmp1B6,sizeof(void*),0));});}{void*_tmp1B7=(
void*)_tmp149.field_info;struct Cyc_Absyn_Tunionfield*_tmp1B8;struct Cyc_Absyn_Tuniondecl*
_tmp1B9;_LLB7: if(*((int*)_tmp1B7)== 1){_LLBC: _tmp1B9=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp1B7)->f1;goto _LLBB;_LLBB: _tmp1B8=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp1B7)->f2;goto _LLB8;}else{goto _LLB9;}_LLB9: goto _LLBA;_LLB8: if(_tmp1B8->tvs != 
0){({void*_tmp1BA[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen(): tunionfield type parameters not handled yet",sizeof(
unsigned char),51),_tag_arr(_tmp1BA,sizeof(void*),0));});}{struct Cyc_List_List*
_tmp1BB=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple6*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcgenrep_get_second,_tmp1B8->typs);struct Cyc_List_List*
_tmp1BC=({struct Cyc_List_List*_tmp1BE=_cycalloc(sizeof(struct Cyc_List_List));
_tmp1BE->hd=(void*)Cyc_Absyn_uint_t;_tmp1BE->tl=_tmp1BB;_tmp1BE;});struct _tuple3*
_tmp1BD=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",
sizeof(unsigned char),4)));dict=Cyc_Tcgenrep_buildRepTuple(te,dict,loc,_tmp1BD,(
void*)0,type,_tmp1BC);goto _LLB6;}_LLBA:({void*_tmp1BF[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield must be known at this point",
sizeof(unsigned char),47),_tag_arr(_tmp1BF,sizeof(void*),0));});goto _LLB6;_LLB6:;}
goto _LL4F;_LL7B: if(_tmp14A == 0){({void*_tmp1C0[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): union declaration must be present by now",
sizeof(unsigned char),48),_tag_arr(_tmp1C0,sizeof(void*),0));});}{struct Cyc_Absyn_Uniondecl*
_tmp1C1=*((struct Cyc_Absyn_Uniondecl**)_check_null(_tmp14A));if(_tmp1C1->tvs != 0){({
void*_tmp1C2[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen: unions with parameters not yet supported",sizeof(unsigned char),
46),_tag_arr(_tmp1C2,sizeof(void*),0));});}if(_tmp1C1->fields == 0){struct _tuple3*
_tmp1C3=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp1C6=_cycalloc(sizeof(
struct _tagged_arr));_tmp1C6[0]=Cyc_Tcgenrep_make_type_cstring(type);_tmp1C6;}));
struct Cyc_Core_Opt*_tmp1C4=({struct Cyc_Core_Opt*_tmp1C5=_cycalloc(sizeof(struct
Cyc_Core_Opt));_tmp1C5->v=Cyc_Tcgenrep_gen_vardecl(_tmp1C3,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_union_str)),0,(void*)3,loc);_tmp1C5;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1C3,loc),loc),0,1);goto _LL4F;}{struct Cyc_List_List*_tmp1C7=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_type,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1C1->fields))->v);
struct _tuple3*_tmp1C8=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp1C9=
_cycalloc(sizeof(struct _tagged_arr));_tmp1C9[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp1C9;}));dict=Cyc_Tcgenrep_buildRepUnion(te,dict,loc,_tmp1C8,(void*)2,
type,_tmp1C7);goto _LL4F;}}_LL7D: {struct Cyc_List_List*_tmp1CA=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_type,
_tmp14D);struct _tuple3*_tmp1CB=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(unsigned char),4)));dict=Cyc_Tcgenrep_buildRepUnion(te,
dict,loc,_tmp1CB,(void*)0,type,_tmp1CA);goto _LL4F;}_LL7F: goto _LL81;_LL81: {
struct _tuple3*_tmp1CC=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(unsigned char),4)));struct Cyc_Absyn_Decl*_tmp1CD=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_int_str),_tmp1CC,({struct Cyc_Absyn_Exp*_tmp1CF[1];_tmp1CF[0]=Cyc_Tcgenrep_cnst_int(
32,loc);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp1CF,sizeof(struct Cyc_Absyn_Exp*),1));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(
dict,type,({struct Cyc_Absyn_Decl*_tmp1CE[1];_tmp1CE[0]=_tmp1CD;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1CE,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1CC,loc),loc),0,0);goto _LL4F;}_LL83:({void*_tmp1D0[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen() for sizeof_t<-> not yet supported",
sizeof(unsigned char),40),_tag_arr(_tmp1D0,sizeof(void*),0));});goto _LL4F;_LL4F:;}
return({struct _tuple12*_tmp1D1=_cycalloc(sizeof(struct _tuple12));_tmp1D1->f1=
dict;_tmp1D1->f2=((struct Cyc_Tcgenrep_RepInfo*(*)(struct Cyc_Dict_Dict*d,void*k))
Cyc_Dict_lookup)(dict,type);_tmp1D1;});}static int Cyc_Tcgenrep_not_emitted_filter(
struct Cyc_Tcgenrep_RepInfo*ri){return ri->emitted == 0;}static void Cyc_Tcgenrep_mark_emitted(
struct Cyc_Tcgenrep_RepInfo*ri){ri->emitted=1;}struct _tuple15{struct Cyc_Dict_Dict*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple15 Cyc_Tcgenrep_tcGenrep(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
void*type,struct Cyc_Dict_Dict*dict){Cyc_Tcutil_check_valid_toplevel_type(loc,te,
type);Cyc_Tcutil_check_valid_toplevel_type(loc,te,type);{struct _tuple12 _tmp1D3;
struct Cyc_Tcgenrep_RepInfo*_tmp1D4;struct Cyc_Dict_Dict*_tmp1D5;struct _tuple12*
_tmp1D2=Cyc_Tcgenrep_lookupRep(te,dict,loc,type);_tmp1D3=*_tmp1D2;_LLBF: _tmp1D5=
_tmp1D3.f1;goto _LLBE;_LLBE: _tmp1D4=_tmp1D3.f2;goto _LLBD;_LLBD: {struct Cyc_List_List*
_tmp1D6=Cyc_Tcgenrep_dfs(_tmp1D4);return({struct _tuple15 _tmp1D7;_tmp1D7.f1=
_tmp1D5;_tmp1D7.f2=_tmp1D6;_tmp1D7.f3=(struct Cyc_Absyn_Exp*)_check_null(_tmp1D4->exp);
_tmp1D7;});}}}
