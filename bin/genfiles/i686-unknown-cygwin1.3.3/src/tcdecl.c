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
unsigned char*tag;struct _tagged_arr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;extern struct Cyc_Std___sFILE*Cyc_Std_stderr;
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_vrprintf(struct
_RegionHandle*r1,struct _tagged_arr fmt,struct _tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern int
Cyc_List_length(struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
extern unsigned char Cyc_List_Nth[8];extern int Cyc_List_exists_c(int(*pred)(void*,
void*),void*env,struct Cyc_List_List*x);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern struct Cyc_Position_Error*
Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,struct _tagged_arr);extern
unsigned char Cyc_Position_Nocontext[14];extern void Cyc_Position_post_error(struct
Cyc_Position_Error*);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple0{
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
15];extern int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual();extern void*Cyc_Absyn_force_kb(void*kb);extern struct
_tagged_arr Cyc_Absyn_attribute2string(void*);extern unsigned char Cyc_Tcdecl_Incompatible[
17];struct Cyc_Tcdecl_Xtunionfielddecl{struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*
field;};extern void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*loc,struct
_tagged_arr*msg1,struct _tagged_arr fmt,struct _tagged_arr ap);struct _tuple3{void*f1;
int f2;};extern struct _tuple3 Cyc_Tcdecl_merge_scope(void*s0,void*s1,struct
_tagged_arr t,struct _tagged_arr v,struct Cyc_Position_Segment*loc,struct _tagged_arr*
msg);extern struct Cyc_Absyn_Structdecl*Cyc_Tcdecl_merge_structdecl(struct Cyc_Absyn_Structdecl*
d0,struct Cyc_Absyn_Structdecl*d1,struct Cyc_Position_Segment*loc,struct
_tagged_arr*msg);extern struct Cyc_Absyn_Uniondecl*Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl*d0,struct Cyc_Absyn_Uniondecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Tuniondecl*Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_Tcdecl_Xtunionfielddecl*
Cyc_Tcdecl_merge_xtunionfielddecl(struct Cyc_Tcdecl_Xtunionfielddecl*d0,struct Cyc_Tcdecl_Xtunionfielddecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_List_List*
Cyc_Tcdecl_sort_xtunion_fields(struct Cyc_List_List*f,int*res,struct _tagged_arr*v,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Dict_Dict;extern
unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];extern
int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*s2);extern int Cyc_Std_zstrptrcmp(
struct _tagged_arr*,struct _tagged_arr*);extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr,struct _tagged_arr);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[
11];struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_StructRes_struct{
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
le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void Cyc_Tcutil_explain_failure();
extern int Cyc_Tcutil_unify(void*,void*);extern void*Cyc_Tcutil_substitute(struct
Cyc_List_List*,void*);extern int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2);extern int Cyc_Tcutil_same_atts(struct Cyc_List_List*,
struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern struct
_tagged_arr Cyc_Absynpp_typ2string(void*);extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*);extern struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple0*);extern
struct _tagged_arr Cyc_Absynpp_scope2string(void*sc);unsigned char Cyc_Tcdecl_Incompatible[
17]="\000\000\000\000Incompatible";void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg1,struct _tagged_arr fmt,struct _tagged_arr ap){if(msg1 == 0){(
int)_throw((void*)Cyc_Tcdecl_Incompatible);}{struct _tagged_arr fmt2=(struct
_tagged_arr)Cyc_Std_strconcat(_tag_arr("%s ",sizeof(unsigned char),4),fmt);
struct _tagged_arr ap2=({unsigned int _tmp0=_get_arr_size(ap,sizeof(void*))+ 1;void**
_tmp1=(void**)_cycalloc(_check_times(sizeof(void*),_tmp0));struct _tagged_arr
_tmp5=_tag_arr(_tmp1,sizeof(void*),_get_arr_size(ap,sizeof(void*))+ 1);{
unsigned int _tmp2=_tmp0;unsigned int i;for(i=0;i < _tmp2;i ++){_tmp1[i]=i == 0?(void*)({
struct Cyc_Std_String_pa_struct*_tmp3=_cycalloc(sizeof(struct Cyc_Std_String_pa_struct));
_tmp3[0]=({struct Cyc_Std_String_pa_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(struct
_tagged_arr)*msg1;_tmp4;});_tmp3;}):*((void**)_check_unknown_subscript(ap,
sizeof(void*),(int)(i - 1)));}};_tmp5;});Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,fmt2,ap2)));}}
static void Cyc_Tcdecl_merge_scope_err(void*s0,void*s1,struct _tagged_arr t,struct
_tagged_arr v,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){({struct Cyc_Std_String_pa_struct
_tmpA;_tmpA.tag=0;_tmpA.f1=(struct _tagged_arr)Cyc_Absynpp_scope2string(s0);{
struct Cyc_Std_String_pa_struct _tmp9;_tmp9.tag=0;_tmp9.f1=(struct _tagged_arr)Cyc_Absynpp_scope2string(
s1);{struct Cyc_Std_String_pa_struct _tmp8;_tmp8.tag=0;_tmp8.f1=(struct _tagged_arr)
v;{struct Cyc_Std_String_pa_struct _tmp7;_tmp7.tag=0;_tmp7.f1=(struct _tagged_arr)t;{
void*_tmp6[4]={& _tmp7,& _tmp8,& _tmp9,& _tmpA};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s is %s whereas expected scope is %s",
sizeof(unsigned char),41),_tag_arr(_tmp6,sizeof(void*),4));}}}}});}struct _tuple4{
void*f1;void*f2;};struct _tuple3 Cyc_Tcdecl_merge_scope(void*s0,void*s1,struct
_tagged_arr t,struct _tagged_arr v,struct Cyc_Position_Segment*loc,struct _tagged_arr*
msg){{struct _tuple4 _tmpC=({struct _tuple4 _tmpB;_tmpB.f1=s0;_tmpB.f2=s1;_tmpB;});
void*_tmpD;void*_tmpE;void*_tmpF;void*_tmp10;void*_tmp11;void*_tmp12;void*_tmp13;
void*_tmp14;void*_tmp15;void*_tmp16;void*_tmp17;void*_tmp18;_LL1: _LL14: _tmpE=
_tmpC.f1;if((int)_tmpE == 4){goto _LL13;}else{goto _LL3;}_LL13: _tmpD=_tmpC.f2;if((
int)_tmpD == 4){goto _LL2;}else{goto _LL3;}_LL3: _LL15: _tmpF=_tmpC.f1;if((int)_tmpF
== 4){goto _LL4;}else{goto _LL5;}_LL5: _LL16: _tmp10=_tmpC.f2;if((int)_tmp10 == 4){
goto _LL6;}else{goto _LL7;}_LL7: _LL17: _tmp11=_tmpC.f2;if((int)_tmp11 == 3){goto _LL8;}
else{goto _LL9;}_LL9: _LL18: _tmp12=_tmpC.f1;if((int)_tmp12 == 3){goto _LLA;}else{
goto _LLB;}_LLB: _LL1A: _tmp14=_tmpC.f1;if((int)_tmp14 == 0){goto _LL19;}else{goto
_LLD;}_LL19: _tmp13=_tmpC.f2;if((int)_tmp13 == 0){goto _LLC;}else{goto _LLD;}_LLD:
_LL1C: _tmp16=_tmpC.f1;if((int)_tmp16 == 2){goto _LL1B;}else{goto _LLF;}_LL1B: _tmp15=
_tmpC.f2;if((int)_tmp15 == 2){goto _LLE;}else{goto _LLF;}_LLF: _LL1E: _tmp18=_tmpC.f1;
if((int)_tmp18 == 1){goto _LL1D;}else{goto _LL11;}_LL1D: _tmp17=_tmpC.f2;if((int)
_tmp17 == 1){goto _LL10;}else{goto _LL11;}_LL11: goto _LL12;_LL2: goto _LL0;_LL4: goto
_LL6;_LL6: Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple3
_tmp19;_tmp19.f1=s1;_tmp19.f2=0;_tmp19;});_LL8: s1=s0;goto _LL0;_LLA: goto _LL0;_LLC:
goto _LLE;_LLE: goto _LL10;_LL10: goto _LL0;_LL12: Cyc_Tcdecl_merge_scope_err(s0,s1,t,
v,loc,msg);return({struct _tuple3 _tmp1A;_tmp1A.f1=s1;_tmp1A.f2=0;_tmp1A;});_LL0:;}
return({struct _tuple3 _tmp1B;_tmp1B.f1=s1;_tmp1B.f2=1;_tmp1B;});}static int Cyc_Tcdecl_check_type(
void*t0,void*t1){return Cyc_Tcutil_unify(t0,t1);}static unsigned int Cyc_Tcdecl_get_uint_const_value(
struct Cyc_Absyn_Exp*e){void*_tmp1C=(void*)e->r;void*_tmp1D;int _tmp1E;_LL20: if(*((
int*)_tmp1C)== 0){_LL24: _tmp1D=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp1C)->f1;
if((unsigned int)_tmp1D > 1?*((int*)_tmp1D)== 2: 0){_LL25: _tmp1E=((struct Cyc_Absyn_Int_c_struct*)
_tmp1D)->f2;goto _LL21;}else{goto _LL22;}}else{goto _LL22;}_LL22: goto _LL23;_LL21:
return(unsigned int)_tmp1E;_LL23:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp1F=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));_tmp1F[0]=({
struct Cyc_Core_Invalid_argument_struct _tmp20;_tmp20.tag=Cyc_Core_Invalid_argument;
_tmp20.f1=_tag_arr("Tcdecl::get_uint_const_value",sizeof(unsigned char),29);
_tmp20;});_tmp1F;}));_LL1F:;}inline static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*
tvs0,struct Cyc_List_List*tvs1,struct _tagged_arr t,struct _tagged_arr v,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg){if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tvs0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1)){({struct Cyc_Std_String_pa_struct
_tmp23;_tmp23.tag=0;_tmp23.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmp22;_tmp22.tag=0;_tmp22.f1=(struct _tagged_arr)t;{void*_tmp21[2]={& _tmp22,&
_tmp23};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s has a different number of type parameters",
sizeof(unsigned char),48),_tag_arr(_tmp21,sizeof(void*),2));}}});return 0;}{
struct Cyc_List_List*_tmp24=tvs0;struct Cyc_List_List*_tmp25=tvs1;for(0;_tmp24 != 0;(
_tmp24=_tmp24->tl,_tmp25=_tmp25->tl)){void*_tmp26=Cyc_Absyn_force_kb((void*)((
struct Cyc_Absyn_Tvar*)_tmp24->hd)->kind);void*_tmp27=Cyc_Absyn_force_kb((void*)((
struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp25))->hd)->kind);
if(_tmp26 == _tmp27){continue;}({struct Cyc_Std_String_pa_struct _tmp2D;_tmp2D.tag=
0;_tmp2D.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp27);{struct Cyc_Std_String_pa_struct
_tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)_tmp24->hd)->name;{
struct Cyc_Std_String_pa_struct _tmp2B;_tmp2B.tag=0;_tmp2B.f1=(struct _tagged_arr)
Cyc_Absynpp_kind2string(_tmp26);{struct Cyc_Std_String_pa_struct _tmp2A;_tmp2A.tag=
0;_tmp2A.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct _tmp29;_tmp29.tag=
0;_tmp29.f1=(struct _tagged_arr)t;{void*_tmp28[5]={& _tmp29,& _tmp2A,& _tmp2B,&
_tmp2C,& _tmp2D};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s has a different kind (%s) for type parameter %s (%s)",
sizeof(unsigned char),59),_tag_arr(_tmp28,sizeof(void*),5));}}}}}});return 0;}
return 1;}}static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,struct Cyc_List_List*
atts1,struct _tagged_arr t,struct _tagged_arr v,struct Cyc_Position_Segment*loc,
struct _tagged_arr*msg){if(! Cyc_Tcutil_same_atts(atts0,atts1)){({struct Cyc_Std_String_pa_struct
_tmp30;_tmp30.tag=0;_tmp30.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct _tagged_arr)t;{void*_tmp2E[2]={& _tmp2F,&
_tmp30};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s has different attributes",
sizeof(unsigned char),31),_tag_arr(_tmp2E,sizeof(void*),2));}}});return 0;}return
1;}struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(
struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){struct Cyc_List_List*_tmp31=0;
for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){_tmp31=({struct Cyc_List_List*
_tmp32=_cycalloc(sizeof(struct Cyc_List_List));_tmp32->hd=({struct _tuple5*_tmp33=
_cycalloc(sizeof(struct _tuple5));_tmp33->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(tvs1))->hd;_tmp33->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp34=
_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));_tmp34[0]=({struct Cyc_Absyn_VarType_struct
_tmp35;_tmp35.tag=1;_tmp35.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd;_tmp35;});_tmp34;});
_tmp33;});_tmp32->tl=_tmp31;_tmp32;});}return _tmp31;}struct _tuple6{struct Cyc_Core_Opt*
f1;struct Cyc_Core_Opt*f2;};static struct Cyc_Absyn_Structdecl*Cyc_Tcdecl_merge_struct_or_union_decl(
struct Cyc_Absyn_Structdecl*d0,struct Cyc_Absyn_Structdecl*d1,struct _tagged_arr t,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){struct _tagged_arr _tmp36=
Cyc_Absynpp_qvar2string((struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(d0->name))->v);
int _tmp37=1;if(! Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,t,_tmp36,loc,msg)){return 0;}{
int _tmp39;void*_tmp3A;struct _tuple3 _tmp38=Cyc_Tcdecl_merge_scope((void*)d0->sc,(
void*)d1->sc,t,_tmp36,loc,msg);_LL28: _tmp3A=_tmp38.f1;goto _LL27;_LL27: _tmp39=
_tmp38.f2;goto _LL26;_LL26: if(! _tmp39){_tmp37=0;}if(! Cyc_Tcdecl_check_atts(d0->attributes,
d1->attributes,t,_tmp36,loc,msg)){_tmp37=0;}{struct Cyc_Absyn_Structdecl*d2;{
struct _tuple6 _tmp3C=({struct _tuple6 _tmp3B;_tmp3B.f1=d0->fields;_tmp3B.f2=d1->fields;
_tmp3B;});struct Cyc_Core_Opt*_tmp3D;struct Cyc_Core_Opt*_tmp3E;struct Cyc_Core_Opt*
_tmp3F;struct Cyc_Core_Opt _tmp40;struct Cyc_List_List*_tmp41;struct Cyc_Core_Opt*
_tmp42;struct Cyc_Core_Opt _tmp43;struct Cyc_List_List*_tmp44;_LL2A: _LL30: _tmp3D=
_tmp3C.f2;if(_tmp3D == 0){goto _LL2B;}else{goto _LL2C;}_LL2C: _LL31: _tmp3E=_tmp3C.f1;
if(_tmp3E == 0){goto _LL2D;}else{goto _LL2E;}_LL2E: _LL34: _tmp42=_tmp3C.f1;if(_tmp42
== 0){goto _LL29;}else{_tmp43=*_tmp42;_LL35: _tmp44=(struct Cyc_List_List*)_tmp43.v;
goto _LL32;}_LL32: _tmp3F=_tmp3C.f2;if(_tmp3F == 0){goto _LL29;}else{_tmp40=*_tmp3F;
_LL33: _tmp41=(struct Cyc_List_List*)_tmp40.v;goto _LL2F;}_LL2B: d2=d0;goto _LL29;
_LL2D: d2=d1;goto _LL29;_LL2F: {struct Cyc_List_List*_tmp45=Cyc_Tcdecl_build_tvs_map(
d0->tvs,d1->tvs);for(0;_tmp44 != 0? _tmp41 != 0: 0;(_tmp44=_tmp44->tl,_tmp41=_tmp41->tl)){
struct Cyc_Absyn_Structfield _tmp47;struct Cyc_List_List*_tmp48;struct Cyc_Absyn_Exp*
_tmp49;void*_tmp4A;struct Cyc_Absyn_Tqual _tmp4B;struct _tagged_arr*_tmp4C;struct
Cyc_Absyn_Structfield*_tmp46=(struct Cyc_Absyn_Structfield*)_tmp44->hd;_tmp47=*
_tmp46;_LL3B: _tmp4C=_tmp47.name;goto _LL3A;_LL3A: _tmp4B=_tmp47.tq;goto _LL39;_LL39:
_tmp4A=(void*)_tmp47.type;goto _LL38;_LL38: _tmp49=_tmp47.width;goto _LL37;_LL37:
_tmp48=_tmp47.attributes;goto _LL36;_LL36: {struct Cyc_Absyn_Structfield _tmp4E;
struct Cyc_List_List*_tmp4F;struct Cyc_Absyn_Exp*_tmp50;void*_tmp51;struct Cyc_Absyn_Tqual
_tmp52;struct _tagged_arr*_tmp53;struct Cyc_Absyn_Structfield*_tmp4D=(struct Cyc_Absyn_Structfield*)
_tmp41->hd;_tmp4E=*_tmp4D;_LL41: _tmp53=_tmp4E.name;goto _LL40;_LL40: _tmp52=_tmp4E.tq;
goto _LL3F;_LL3F: _tmp51=(void*)_tmp4E.type;goto _LL3E;_LL3E: _tmp50=_tmp4E.width;
goto _LL3D;_LL3D: _tmp4F=_tmp4E.attributes;goto _LL3C;_LL3C: if(Cyc_Std_zstrptrcmp(
_tmp4C,_tmp53)!= 0){({struct Cyc_Std_String_pa_struct _tmp58;_tmp58.tag=0;_tmp58.f1=(
struct _tagged_arr)*_tmp53;{struct Cyc_Std_String_pa_struct _tmp57;_tmp57.tag=0;
_tmp57.f1=(struct _tagged_arr)*_tmp4C;{struct Cyc_Std_String_pa_struct _tmp56;
_tmp56.tag=0;_tmp56.f1=(struct _tagged_arr)_tmp36;{struct Cyc_Std_String_pa_struct
_tmp55;_tmp55.tag=0;_tmp55.f1=(struct _tagged_arr)t;{void*_tmp54[4]={& _tmp55,&
_tmp56,& _tmp57,& _tmp58};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s : field name mismatch %s != %s",
sizeof(unsigned char),37),_tag_arr(_tmp54,sizeof(void*),4));}}}}});return 0;}if(!
Cyc_Tcutil_same_atts(_tmp48,_tmp4F)){({struct Cyc_Std_String_pa_struct _tmp5C;
_tmp5C.tag=0;_tmp5C.f1=(struct _tagged_arr)*_tmp4C;{struct Cyc_Std_String_pa_struct
_tmp5B;_tmp5B.tag=0;_tmp5B.f1=(struct _tagged_arr)_tmp36;{struct Cyc_Std_String_pa_struct
_tmp5A;_tmp5A.tag=0;_tmp5A.f1=(struct _tagged_arr)t;{void*_tmp59[3]={& _tmp5A,&
_tmp5B,& _tmp5C};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s : attribute mismatch on field %s",
sizeof(unsigned char),39),_tag_arr(_tmp59,sizeof(void*),3));}}}});_tmp37=0;}if(!
Cyc_Tcutil_equal_tqual(_tmp4B,_tmp52)){({struct Cyc_Std_String_pa_struct _tmp60;
_tmp60.tag=0;_tmp60.f1=(struct _tagged_arr)*_tmp4C;{struct Cyc_Std_String_pa_struct
_tmp5F;_tmp5F.tag=0;_tmp5F.f1=(struct _tagged_arr)_tmp36;{struct Cyc_Std_String_pa_struct
_tmp5E;_tmp5E.tag=0;_tmp5E.f1=(struct _tagged_arr)t;{void*_tmp5D[3]={& _tmp5E,&
_tmp5F,& _tmp60};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s : qualifier mismatch on field %s",
sizeof(unsigned char),39),_tag_arr(_tmp5D,sizeof(void*),3));}}}});_tmp37=0;}if((((
_tmp49 != 0? _tmp50 != 0: 0)? Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)
_check_null(_tmp49))!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)
_check_null(_tmp50)): 0)? 1:(_tmp49 == 0? _tmp50 != 0: 0))? 1:(_tmp49 != 0? _tmp50 == 0: 0)){({
struct Cyc_Std_String_pa_struct _tmp64;_tmp64.tag=0;_tmp64.f1=(struct _tagged_arr)*
_tmp4C;{struct Cyc_Std_String_pa_struct _tmp63;_tmp63.tag=0;_tmp63.f1=(struct
_tagged_arr)_tmp36;{struct Cyc_Std_String_pa_struct _tmp62;_tmp62.tag=0;_tmp62.f1=(
struct _tagged_arr)t;{void*_tmp61[3]={& _tmp62,& _tmp63,& _tmp64};Cyc_Tcdecl_merr(
loc,msg,_tag_arr("%s %s : bitfield mismatch on field %s",sizeof(unsigned char),
38),_tag_arr(_tmp61,sizeof(void*),3));}}}});_tmp37=0;}{void*subst_t1=Cyc_Tcutil_substitute(
_tmp45,_tmp51);if(! Cyc_Tcdecl_check_type(_tmp4A,subst_t1)){({struct Cyc_Std_String_pa_struct
_tmp6A;_tmp6A.tag=0;_tmp6A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(subst_t1);{
struct Cyc_Std_String_pa_struct _tmp69;_tmp69.tag=0;_tmp69.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(_tmp4A);{struct Cyc_Std_String_pa_struct _tmp68;_tmp68.tag=
0;_tmp68.f1=(struct _tagged_arr)*_tmp4C;{struct Cyc_Std_String_pa_struct _tmp67;
_tmp67.tag=0;_tmp67.f1=(struct _tagged_arr)_tmp36;{struct Cyc_Std_String_pa_struct
_tmp66;_tmp66.tag=0;_tmp66.f1=(struct _tagged_arr)t;{void*_tmp65[5]={& _tmp66,&
_tmp67,& _tmp68,& _tmp69,& _tmp6A};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s : type mismatch on field %s: %s != %s",
sizeof(unsigned char),44),_tag_arr(_tmp65,sizeof(void*),5));}}}}}});Cyc_Tcutil_explain_failure();
_tmp37=0;}}}}if(_tmp44 != 0){({struct Cyc_Std_String_pa_struct _tmp6E;_tmp6E.tag=0;
_tmp6E.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Structfield*)_tmp44->hd)->name;{
struct Cyc_Std_String_pa_struct _tmp6D;_tmp6D.tag=0;_tmp6D.f1=(struct _tagged_arr)
_tmp36;{struct Cyc_Std_String_pa_struct _tmp6C;_tmp6C.tag=0;_tmp6C.f1=(struct
_tagged_arr)t;{void*_tmp6B[3]={& _tmp6C,& _tmp6D,& _tmp6E};Cyc_Tcdecl_merr(loc,msg,
_tag_arr("%s %s is missing field %s",sizeof(unsigned char),26),_tag_arr(_tmp6B,
sizeof(void*),3));}}}});_tmp37=0;}if(_tmp41 != 0){({struct Cyc_Std_String_pa_struct
_tmp72;_tmp72.tag=0;_tmp72.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Structfield*)
_tmp41->hd)->name;{struct Cyc_Std_String_pa_struct _tmp71;_tmp71.tag=0;_tmp71.f1=(
struct _tagged_arr)_tmp36;{struct Cyc_Std_String_pa_struct _tmp70;_tmp70.tag=0;
_tmp70.f1=(struct _tagged_arr)t;{void*_tmp6F[3]={& _tmp70,& _tmp71,& _tmp72};Cyc_Tcdecl_merr(
loc,msg,_tag_arr("%s %s has extra field %s",sizeof(unsigned char),25),_tag_arr(
_tmp6F,sizeof(void*),3));}}}});_tmp37=0;}d2=d0;goto _LL29;}_LL29:;}if(! _tmp37){
return 0;}if(_tmp3A == (void*)d2->sc){return(struct Cyc_Absyn_Structdecl*)d2;}else{
d2=({struct Cyc_Absyn_Structdecl*_tmp73=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));
_tmp73[0]=*d2;_tmp73;});(void*)(d2->sc=(void*)_tmp3A);return(struct Cyc_Absyn_Structdecl*)
d2;}}}}struct Cyc_Absyn_Structdecl*Cyc_Tcdecl_merge_structdecl(struct Cyc_Absyn_Structdecl*
d0,struct Cyc_Absyn_Structdecl*d1,struct Cyc_Position_Segment*loc,struct
_tagged_arr*msg){return Cyc_Tcdecl_merge_struct_or_union_decl(d0,d1,_tag_arr("struct",
sizeof(unsigned char),7),loc,msg);}struct Cyc_Absyn_Uniondecl*Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl*d0,struct Cyc_Absyn_Uniondecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg){return(struct Cyc_Absyn_Uniondecl*)Cyc_Tcdecl_merge_struct_or_union_decl((
struct Cyc_Absyn_Structdecl*)d0,(struct Cyc_Absyn_Structdecl*)d1,_tag_arr("union",
sizeof(unsigned char),6),loc,msg);}inline static struct _tagged_arr Cyc_Tcdecl_is_x2string(
int is_x){return is_x? _tag_arr("xtunion",sizeof(unsigned char),8): _tag_arr("tunion",
sizeof(unsigned char),7);}struct _tuple7{struct Cyc_Absyn_Tqual f1;void*f2;};static
struct Cyc_Absyn_Tunionfield*Cyc_Tcdecl_merge_tunionfield(struct Cyc_Absyn_Tunionfield*
f0,struct Cyc_Absyn_Tunionfield*f1,struct Cyc_List_List*inst,struct _tagged_arr t,
struct _tagged_arr v,struct _tagged_arr*msg){struct Cyc_Position_Segment*loc=f1->loc;
if(Cyc_Std_zstrptrcmp((*f0->name).f2,(*f1->name).f2)!= 0){({struct Cyc_Std_String_pa_struct
_tmp78;_tmp78.tag=0;_tmp78.f1=(struct _tagged_arr)*(*f0->name).f2;{struct Cyc_Std_String_pa_struct
_tmp77;_tmp77.tag=0;_tmp77.f1=(struct _tagged_arr)*(*f1->name).f2;{struct Cyc_Std_String_pa_struct
_tmp76;_tmp76.tag=0;_tmp76.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmp75;_tmp75.tag=0;_tmp75.f1=(struct _tagged_arr)t;{void*_tmp74[4]={& _tmp75,&
_tmp76,& _tmp77,& _tmp78};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s: field name mismatch %s != %s",
sizeof(unsigned char),36),_tag_arr(_tmp74,sizeof(void*),4));}}}}});return 0;}{
struct _tagged_arr _tmp79=*(*f0->name).f2;int _tmp7E;void*_tmp7F;struct _tuple3
_tmp7D=Cyc_Tcdecl_merge_scope((void*)f0->sc,(void*)f1->sc,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp7C;_tmp7C.tag=0;_tmp7C.f1=(struct _tagged_arr)v;{
struct Cyc_Std_String_pa_struct _tmp7B;_tmp7B.tag=0;_tmp7B.f1=(struct _tagged_arr)t;{
void*_tmp7A[2]={& _tmp7B,& _tmp7C};Cyc_Std_aprintf(_tag_arr("in %s %s, field",
sizeof(unsigned char),16),_tag_arr(_tmp7A,sizeof(void*),2));}}}),_tmp79,loc,msg);
_LL44: _tmp7F=_tmp7D.f1;goto _LL43;_LL43: _tmp7E=_tmp7D.f2;goto _LL42;_LL42: {struct
Cyc_List_List*_tmp80=f0->tvs;struct Cyc_List_List*_tmp81=f1->tvs;if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp80)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp81)){({struct Cyc_Std_String_pa_struct _tmp85;_tmp85.tag=0;
_tmp85.f1=(struct _tagged_arr)_tmp79;{struct Cyc_Std_String_pa_struct _tmp84;_tmp84.tag=
0;_tmp84.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct _tmp83;_tmp83.tag=
0;_tmp83.f1=(struct _tagged_arr)t;{void*_tmp82[3]={& _tmp83,& _tmp84,& _tmp85};Cyc_Tcdecl_merr(
loc,msg,_tag_arr("%s %s, field %s: type parameter number mismatch",sizeof(
unsigned char),48),_tag_arr(_tmp82,sizeof(void*),3));}}}});return 0;}{struct Cyc_List_List*
_tmp86=Cyc_Tcdecl_build_tvs_map(_tmp80,_tmp81);struct Cyc_List_List*_tmp87=f0->typs;
struct Cyc_List_List*_tmp88=f1->typs;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp87)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp88)){({struct Cyc_Std_String_pa_struct
_tmp8C;_tmp8C.tag=0;_tmp8C.f1=(struct _tagged_arr)_tmp79;{struct Cyc_Std_String_pa_struct
_tmp8B;_tmp8B.tag=0;_tmp8B.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmp8A;_tmp8A.tag=0;_tmp8A.f1=(struct _tagged_arr)t;{void*_tmp89[3]={& _tmp8A,&
_tmp8B,& _tmp8C};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s, field %s: parameter number mismatch",
sizeof(unsigned char),43),_tag_arr(_tmp89,sizeof(void*),3));}}}});_tmp7E=0;}for(
0;_tmp87 != 0;(_tmp87=_tmp87->tl,_tmp88=_tmp88->tl)){if(! Cyc_Tcutil_equal_tqual((*((
struct _tuple7*)_tmp87->hd)).f1,(*((struct _tuple7*)((struct Cyc_List_List*)
_check_null(_tmp88))->hd)).f1)){({struct Cyc_Std_String_pa_struct _tmp90;_tmp90.tag=
0;_tmp90.f1=(struct _tagged_arr)_tmp79;{struct Cyc_Std_String_pa_struct _tmp8F;
_tmp8F.tag=0;_tmp8F.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmp8E;_tmp8E.tag=0;_tmp8E.f1=(struct _tagged_arr)t;{void*_tmp8D[3]={& _tmp8E,&
_tmp8F,& _tmp90};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s, field %s: parameter qualifier",
sizeof(unsigned char),37),_tag_arr(_tmp8D,sizeof(void*),3));}}}});_tmp7E=0;}{
void*subst_t1=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp86,inst),(*((struct _tuple7*)_tmp88->hd)).f2);
if(! Cyc_Tcdecl_check_type((*((struct _tuple7*)_tmp87->hd)).f2,subst_t1)){({struct
Cyc_Std_String_pa_struct _tmp96;_tmp96.tag=0;_tmp96.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
subst_t1);{struct Cyc_Std_String_pa_struct _tmp95;_tmp95.tag=0;_tmp95.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((*((struct _tuple7*)_tmp87->hd)).f2);{struct
Cyc_Std_String_pa_struct _tmp94;_tmp94.tag=0;_tmp94.f1=(struct _tagged_arr)_tmp79;{
struct Cyc_Std_String_pa_struct _tmp93;_tmp93.tag=0;_tmp93.f1=(struct _tagged_arr)v;{
struct Cyc_Std_String_pa_struct _tmp92;_tmp92.tag=0;_tmp92.f1=(struct _tagged_arr)t;{
void*_tmp91[5]={& _tmp92,& _tmp93,& _tmp94,& _tmp95,& _tmp96};Cyc_Tcdecl_merr(loc,msg,
_tag_arr("%s %s, field %s: parameter type mismatch %s != %s",sizeof(
unsigned char),50),_tag_arr(_tmp91,sizeof(void*),5));}}}}}});Cyc_Tcutil_explain_failure();
_tmp7E=0;}}}if(! _tmp7E){return 0;}if((void*)f0->sc != _tmp7F){struct Cyc_Absyn_Tunionfield*
_tmp97=({struct Cyc_Absyn_Tunionfield*_tmp98=_cycalloc(sizeof(struct Cyc_Absyn_Tunionfield));
_tmp98[0]=*f0;_tmp98;});(void*)(((struct Cyc_Absyn_Tunionfield*)_check_null(
_tmp97))->sc=(void*)_tmp7F);return _tmp97;}else{return(struct Cyc_Absyn_Tunionfield*)
f0;}}}}}static int Cyc_Tcdecl_substitute_tunionfield_f1(struct _tagged_arr*name,
struct Cyc_Absyn_Tvar*x){return Cyc_Std_strptrcmp(x->name,name)== 0;}static struct
_tuple7*Cyc_Tcdecl_substitute_tunionfield_f2(struct Cyc_List_List*inst,struct
_tuple7*x){struct _tuple7 _tmp9A;void*_tmp9B;struct Cyc_Absyn_Tqual _tmp9C;struct
_tuple7*_tmp99=x;_tmp9A=*_tmp99;_LL47: _tmp9C=_tmp9A.f1;goto _LL46;_LL46: _tmp9B=
_tmp9A.f2;goto _LL45;_LL45: return({struct _tuple7*_tmp9D=_cycalloc(sizeof(struct
_tuple7));_tmp9D->f1=_tmp9C;_tmp9D->f2=Cyc_Tcutil_substitute(inst,_tmp9B);_tmp9D;});}
static struct Cyc_Absyn_Tunionfield*Cyc_Tcdecl_substitute_tunionfield(struct Cyc_List_List*
d0tvs,struct Cyc_List_List*d1tvs,struct Cyc_List_List*inst1,struct Cyc_Absyn_Tunionfield*
f1){struct Cyc_Absyn_Tunionfield*_tmp9E=({struct Cyc_Absyn_Tunionfield*_tmpA8=
_cycalloc(sizeof(struct Cyc_Absyn_Tunionfield));_tmpA8[0]=*f1;_tmpA8;});struct Cyc_List_List*
_tmp9F=0;{struct Cyc_List_List*_tmpA0=f1->tvs;for(0;_tmpA0 != 0;_tmpA0=_tmpA0->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)_tmpA0->hd;if(((int(*)(int(*pred)(
struct _tagged_arr*,struct Cyc_Absyn_Tvar*),struct _tagged_arr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcdecl_substitute_tunionfield_f1,tv->name,d0tvs)? 1:((
int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Tvar*),struct _tagged_arr*
env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name,d1tvs)){int i=0;struct _tagged_arr base=*tv->name;struct _tagged_arr*
new_name;do {new_name=({struct _tagged_arr*_tmpA1=_cycalloc(sizeof(struct
_tagged_arr));_tmpA1[0]=(struct _tagged_arr)Cyc_Std_strconcat(base,(struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpA3;_tmpA3.tag=1;_tmpA3.f1=(int)((
unsigned int)i);{void*_tmpA2[1]={& _tmpA3};Cyc_Std_aprintf(_tag_arr("%d",sizeof(
unsigned char),3),_tag_arr(_tmpA2,sizeof(void*),1));}}));_tmpA1;});i ++;} while (((
int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Tvar*),struct _tagged_arr*
env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcdecl_substitute_tunionfield_f1,
new_name,d0tvs)? 1:((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Tvar*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name,d1tvs));tv=({struct Cyc_Absyn_Tvar*_tmpA4=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_tmpA4[0]=*tv;_tmpA4;});tv->name=new_name;}_tmp9F=({struct Cyc_List_List*_tmpA5=
_cycalloc(sizeof(struct Cyc_List_List));_tmpA5->hd=tv;_tmpA5->tl=_tmp9F;_tmpA5;});}}
_tmp9F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp9F);
_tmp9E->tvs=_tmp9F;{struct Cyc_List_List*_tmpA6=Cyc_Tcdecl_build_tvs_map(_tmp9F,
f1->tvs);struct Cyc_List_List*_tmpA7=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA6,inst1);_tmp9E->typs=((struct Cyc_List_List*(*)(
struct _tuple7*(*f)(struct Cyc_List_List*,struct _tuple7*),struct Cyc_List_List*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_tunionfield_f2,
_tmpA7,f1->typs);return _tmp9E;}}static struct Cyc_List_List*Cyc_Tcdecl_merge_xtunion_fields(
struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct
Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct _tagged_arr t,
struct _tagged_arr v,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){struct
Cyc_List_List**f2sp=({struct Cyc_List_List**_tmpB1=_cycalloc(sizeof(struct Cyc_List_List*));
_tmpB1[0]=0;_tmpB1;});struct Cyc_List_List**_tmpA9=f2sp;int cmp=- 1;for(0;f0s != 0?
f1s != 0: 0;f1s=f1s->tl){while(f0s != 0?(cmp=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Tunionfield*)
f0s->hd)->name,((struct Cyc_Absyn_Tunionfield*)f1s->hd)->name))< 0: 0){struct Cyc_List_List*
_tmpAA=({struct Cyc_List_List*_tmpAB=_cycalloc(sizeof(struct Cyc_List_List));
_tmpAB->hd=(struct Cyc_Absyn_Tunionfield*)f0s->hd;_tmpAB->tl=0;_tmpAB;});*_tmpA9=
_tmpAA;_tmpA9=&((struct Cyc_List_List*)_check_null(_tmpAA))->tl;f0s=f0s->tl;}if(
f0s == 0? 1: cmp > 0){*incl=0;{struct Cyc_List_List*_tmpAC=({struct Cyc_List_List*
_tmpAD=_cycalloc(sizeof(struct Cyc_List_List));_tmpAD->hd=Cyc_Tcdecl_substitute_tunionfield(
tvs0,tvs1,inst,(struct Cyc_Absyn_Tunionfield*)f1s->hd);_tmpAD->tl=0;_tmpAD;});*
_tmpA9=_tmpAC;_tmpA9=&((struct Cyc_List_List*)_check_null(_tmpAC))->tl;}}else{
struct Cyc_Absyn_Tunionfield*_tmpAE=Cyc_Tcdecl_merge_tunionfield((struct Cyc_Absyn_Tunionfield*)
f0s->hd,(struct Cyc_Absyn_Tunionfield*)f1s->hd,inst,t,v,msg);if(_tmpAE != 0){if(
_tmpAE != (struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)f0s->hd)){*
incl=0;}{struct Cyc_List_List*_tmpAF=({struct Cyc_List_List*_tmpB0=_cycalloc(
sizeof(struct Cyc_List_List));_tmpB0->hd=(struct Cyc_Absyn_Tunionfield*)
_check_null(_tmpAE);_tmpB0->tl=0;_tmpB0;});*_tmpA9=_tmpAF;_tmpA9=&((struct Cyc_List_List*)
_check_null(_tmpAF))->tl;}}else{*res=0;}f0s=f0s->tl;}}if(f1s != 0){*incl=0;*
_tmpA9=f1s;}else{*_tmpA9=f0s;}return*f2sp;}struct _tuple8{struct Cyc_List_List*f1;
struct Cyc_List_List*f2;};static struct _tuple8 Cyc_Tcdecl_split(struct Cyc_List_List*
f){if(f == 0){return({struct _tuple8 _tmpB2;_tmpB2.f1=0;_tmpB2.f2=0;_tmpB2;});}if(f->tl
== 0){return({struct _tuple8 _tmpB3;_tmpB3.f1=f;_tmpB3.f2=0;_tmpB3;});}{struct Cyc_List_List*
_tmpB5;struct Cyc_List_List*_tmpB6;struct _tuple8 _tmpB4=Cyc_Tcdecl_split(((struct
Cyc_List_List*)_check_null(f->tl))->tl);_LL4A: _tmpB6=_tmpB4.f1;goto _LL49;_LL49:
_tmpB5=_tmpB4.f2;goto _LL48;_LL48: return({struct _tuple8 _tmpB7;_tmpB7.f1=({struct
Cyc_List_List*_tmpB9=_cycalloc(sizeof(struct Cyc_List_List));_tmpB9->hd=(void*)((
void*)f->hd);_tmpB9->tl=_tmpB6;_tmpB9;});_tmpB7.f2=({struct Cyc_List_List*_tmpB8=
_cycalloc(sizeof(struct Cyc_List_List));_tmpB8->hd=(void*)((void*)((struct Cyc_List_List*)
_check_null(f->tl))->hd);_tmpB8->tl=_tmpB5;_tmpB8;});_tmpB7;});}}struct Cyc_List_List*
Cyc_Tcdecl_sort_xtunion_fields(struct Cyc_List_List*f,int*res,struct _tagged_arr*v,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){struct Cyc_List_List*_tmpBB;
struct Cyc_List_List*_tmpBC;struct _tuple8 _tmpBA=((struct _tuple8(*)(struct Cyc_List_List*
f))Cyc_Tcdecl_split)(f);_LL4D: _tmpBC=_tmpBA.f1;goto _LL4C;_LL4C: _tmpBB=_tmpBA.f2;
goto _LL4B;_LL4B: if(_tmpBC != 0? _tmpBC->tl != 0: 0){_tmpBC=Cyc_Tcdecl_sort_xtunion_fields(
_tmpBC,res,v,loc,msg);}if(_tmpBB != 0? _tmpBB->tl != 0: 0){_tmpBB=Cyc_Tcdecl_sort_xtunion_fields(
_tmpBB,res,v,loc,msg);}return Cyc_Tcdecl_merge_xtunion_fields(_tmpBC,_tmpBB,0,0,0,
res,({int*_tmpBD=_cycalloc_atomic(sizeof(int));_tmpBD[0]=1;_tmpBD;}),_tag_arr("xtunion",
sizeof(unsigned char),8),*v,loc,msg);}struct Cyc_Absyn_Tuniondecl*Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg){struct _tagged_arr _tmpBE=Cyc_Absynpp_qvar2string(d0->name);
struct _tagged_arr t=_tag_arr("[x]tunion",sizeof(unsigned char),10);int _tmpBF=1;
if(d0->is_xtunion != d1->is_xtunion){({struct Cyc_Std_String_pa_struct _tmpC3;
_tmpC3.tag=0;_tmpC3.f1=(struct _tagged_arr)Cyc_Tcdecl_is_x2string(d1->is_xtunion);{
struct Cyc_Std_String_pa_struct _tmpC2;_tmpC2.tag=0;_tmpC2.f1=(struct _tagged_arr)
Cyc_Tcdecl_is_x2string(d0->is_xtunion);{struct Cyc_Std_String_pa_struct _tmpC1;
_tmpC1.tag=0;_tmpC1.f1=(struct _tagged_arr)_tmpBE;{void*_tmpC0[3]={& _tmpC1,&
_tmpC2,& _tmpC3};Cyc_Tcdecl_merr(loc,msg,_tag_arr("expected %s to be a %s instead of a %s",
sizeof(unsigned char),39),_tag_arr(_tmpC0,sizeof(void*),3));}}}});_tmpBF=0;}
else{t=Cyc_Tcdecl_is_x2string(d0->is_xtunion);}if(! Cyc_Tcdecl_check_tvs(d0->tvs,
d1->tvs,t,_tmpBE,loc,msg)){return 0;}{int _tmpC5;void*_tmpC6;struct _tuple3 _tmpC4=
Cyc_Tcdecl_merge_scope((void*)d0->sc,(void*)d1->sc,t,_tmpBE,loc,msg);_LL50:
_tmpC6=_tmpC4.f1;goto _LL4F;_LL4F: _tmpC5=_tmpC4.f2;goto _LL4E;_LL4E: if(! _tmpC5){
_tmpBF=0;}{struct Cyc_Absyn_Tuniondecl*d2;{struct _tuple6 _tmpC8=({struct _tuple6
_tmpC7;_tmpC7.f1=d0->fields;_tmpC7.f2=d1->fields;_tmpC7;});struct Cyc_Core_Opt*
_tmpC9;struct Cyc_Core_Opt*_tmpCA;struct Cyc_Core_Opt*_tmpCB;struct Cyc_Core_Opt
_tmpCC;struct Cyc_List_List*_tmpCD;struct Cyc_Core_Opt*_tmpCE;struct Cyc_Core_Opt
_tmpCF;struct Cyc_List_List*_tmpD0;_LL52: _LL58: _tmpC9=_tmpC8.f2;if(_tmpC9 == 0){
goto _LL53;}else{goto _LL54;}_LL54: _LL59: _tmpCA=_tmpC8.f1;if(_tmpCA == 0){goto _LL55;}
else{goto _LL56;}_LL56: _LL5C: _tmpCE=_tmpC8.f1;if(_tmpCE == 0){goto _LL51;}else{
_tmpCF=*_tmpCE;_LL5D: _tmpD0=(struct Cyc_List_List*)_tmpCF.v;goto _LL5A;}_LL5A:
_tmpCB=_tmpC8.f2;if(_tmpCB == 0){goto _LL51;}else{_tmpCC=*_tmpCB;_LL5B: _tmpCD=(
struct Cyc_List_List*)_tmpCC.v;goto _LL57;}_LL53: d2=d0;goto _LL51;_LL55: d2=d1;goto
_LL51;_LL57: {struct Cyc_List_List*_tmpD1=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
if(d0->is_xtunion){int _tmpD2=1;struct Cyc_List_List*_tmpD3=Cyc_Tcdecl_merge_xtunion_fields(
_tmpD0,_tmpCD,_tmpD1,d0->tvs,d1->tvs,& _tmpBF,& _tmpD2,t,_tmpBE,loc,msg);if(_tmpD2){
d2=d0;}else{d2=({struct Cyc_Absyn_Tuniondecl*_tmpD4=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));
_tmpD4[0]=*d0;_tmpD4;});(void*)(d2->sc=(void*)_tmpC6);d2->fields=({struct Cyc_Core_Opt*
_tmpD5=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpD5->v=_tmpD3;_tmpD5;});}}else{
for(0;_tmpD0 != 0? _tmpCD != 0: 0;(_tmpD0=_tmpD0->tl,_tmpCD=_tmpCD->tl)){Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)_tmpD0->hd,(struct Cyc_Absyn_Tunionfield*)_tmpCD->hd,
_tmpD1,t,_tmpBE,msg);}if(_tmpD0 != 0){({struct Cyc_Std_String_pa_struct _tmpD9;
_tmpD9.tag=0;_tmpD9.f1=(struct _tagged_arr)*(*((struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*)_check_null(_tmpCD))->hd)->name).f2;{struct Cyc_Std_String_pa_struct
_tmpD8;_tmpD8.tag=0;_tmpD8.f1=(struct _tagged_arr)_tmpBE;{struct Cyc_Std_String_pa_struct
_tmpD7;_tmpD7.tag=0;_tmpD7.f1=(struct _tagged_arr)t;{void*_tmpD6[3]={& _tmpD7,&
_tmpD8,& _tmpD9};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s has extra field %s",
sizeof(unsigned char),25),_tag_arr(_tmpD6,sizeof(void*),3));}}}});_tmpBF=0;}if(
_tmpCD != 0){({struct Cyc_Std_String_pa_struct _tmpDD;_tmpDD.tag=0;_tmpDD.f1=(
struct _tagged_arr)*(*((struct Cyc_Absyn_Tunionfield*)_tmpCD->hd)->name).f2;{
struct Cyc_Std_String_pa_struct _tmpDC;_tmpDC.tag=0;_tmpDC.f1=(struct _tagged_arr)
_tmpBE;{struct Cyc_Std_String_pa_struct _tmpDB;_tmpDB.tag=0;_tmpDB.f1=(struct
_tagged_arr)t;{void*_tmpDA[3]={& _tmpDB,& _tmpDC,& _tmpDD};Cyc_Tcdecl_merr(loc,msg,
_tag_arr("%s %s is missing field %s",sizeof(unsigned char),26),_tag_arr(_tmpDA,
sizeof(void*),3));}}}});_tmpBF=0;}d2=d0;}goto _LL51;}_LL51:;}if(! _tmpBF){return 0;}
if(_tmpC6 == (void*)d2->sc){return(struct Cyc_Absyn_Tuniondecl*)d2;}else{d2=({
struct Cyc_Absyn_Tuniondecl*_tmpDE=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));
_tmpDE[0]=*d2;_tmpDE;});(void*)(d2->sc=(void*)_tmpC6);return(struct Cyc_Absyn_Tuniondecl*)
d2;}}}}struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*
d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*
msg){struct _tagged_arr _tmpDF=Cyc_Absynpp_qvar2string(d0->name);int _tmpE0=1;int
_tmpE2;void*_tmpE3;struct _tuple3 _tmpE1=Cyc_Tcdecl_merge_scope((void*)d0->sc,(
void*)d1->sc,_tag_arr("enum",sizeof(unsigned char),5),_tmpDF,loc,msg);_LL60:
_tmpE3=_tmpE1.f1;goto _LL5F;_LL5F: _tmpE2=_tmpE1.f2;goto _LL5E;_LL5E: if(! _tmpE2){
_tmpE0=0;}{struct Cyc_Absyn_Enumdecl*d2;{struct _tuple6 _tmpE5=({struct _tuple6
_tmpE4;_tmpE4.f1=d0->fields;_tmpE4.f2=d1->fields;_tmpE4;});struct Cyc_Core_Opt*
_tmpE6;struct Cyc_Core_Opt*_tmpE7;struct Cyc_Core_Opt*_tmpE8;struct Cyc_Core_Opt
_tmpE9;struct Cyc_List_List*_tmpEA;struct Cyc_Core_Opt*_tmpEB;struct Cyc_Core_Opt
_tmpEC;struct Cyc_List_List*_tmpED;_LL62: _LL68: _tmpE6=_tmpE5.f2;if(_tmpE6 == 0){
goto _LL63;}else{goto _LL64;}_LL64: _LL69: _tmpE7=_tmpE5.f1;if(_tmpE7 == 0){goto _LL65;}
else{goto _LL66;}_LL66: _LL6C: _tmpEB=_tmpE5.f1;if(_tmpEB == 0){goto _LL61;}else{
_tmpEC=*_tmpEB;_LL6D: _tmpED=(struct Cyc_List_List*)_tmpEC.v;goto _LL6A;}_LL6A:
_tmpE8=_tmpE5.f2;if(_tmpE8 == 0){goto _LL61;}else{_tmpE9=*_tmpE8;_LL6B: _tmpEA=(
struct Cyc_List_List*)_tmpE9.v;goto _LL67;}_LL63: d2=d0;goto _LL61;_LL65: d2=d1;goto
_LL61;_LL67: for(0;_tmpED != 0? _tmpEA != 0: 0;(_tmpED=_tmpED->tl,_tmpEA=_tmpEA->tl)){
struct Cyc_Absyn_Enumfield _tmpEF;struct Cyc_Position_Segment*_tmpF0;struct Cyc_Absyn_Exp*
_tmpF1;struct _tuple0*_tmpF2;struct _tuple0 _tmpF3;struct _tagged_arr*_tmpF4;struct
Cyc_Absyn_Enumfield*_tmpEE=(struct Cyc_Absyn_Enumfield*)_tmpED->hd;_tmpEF=*_tmpEE;
_LL71: _tmpF2=_tmpEF.name;_tmpF3=*_tmpF2;_LL72: _tmpF4=_tmpF3.f2;goto _LL70;_LL70:
_tmpF1=_tmpEF.tag;goto _LL6F;_LL6F: _tmpF0=_tmpEF.loc;goto _LL6E;_LL6E: {struct Cyc_Absyn_Enumfield
_tmpF6;struct Cyc_Position_Segment*_tmpF7;struct Cyc_Absyn_Exp*_tmpF8;struct
_tuple0*_tmpF9;struct _tuple0 _tmpFA;struct _tagged_arr*_tmpFB;struct Cyc_Absyn_Enumfield*
_tmpF5=(struct Cyc_Absyn_Enumfield*)_tmpEA->hd;_tmpF6=*_tmpF5;_LL76: _tmpF9=_tmpF6.name;
_tmpFA=*_tmpF9;_LL77: _tmpFB=_tmpFA.f2;goto _LL75;_LL75: _tmpF8=_tmpF6.tag;goto
_LL74;_LL74: _tmpF7=_tmpF6.loc;goto _LL73;_LL73: if(Cyc_Std_zstrptrcmp(_tmpF4,
_tmpFB)!= 0){({struct Cyc_Std_String_pa_struct _tmpFF;_tmpFF.tag=0;_tmpFF.f1=(
struct _tagged_arr)*_tmpFB;{struct Cyc_Std_String_pa_struct _tmpFE;_tmpFE.tag=0;
_tmpFE.f1=(struct _tagged_arr)*_tmpF4;{struct Cyc_Std_String_pa_struct _tmpFD;
_tmpFD.tag=0;_tmpFD.f1=(struct _tagged_arr)_tmpDF;{void*_tmpFC[3]={& _tmpFD,&
_tmpFE,& _tmpFF};Cyc_Tcdecl_merr(loc,msg,_tag_arr("enum %s: field name mismatch %s != %s",
sizeof(unsigned char),38),_tag_arr(_tmpFC,sizeof(void*),3));}}}});_tmpE0=0;}if(
Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmpF1))!= Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)_check_null(_tmpF8))){({struct Cyc_Std_String_pa_struct
_tmp102;_tmp102.tag=0;_tmp102.f1=(struct _tagged_arr)*_tmpFB;{struct Cyc_Std_String_pa_struct
_tmp101;_tmp101.tag=0;_tmp101.f1=(struct _tagged_arr)_tmpDF;{void*_tmp100[2]={&
_tmp101,& _tmp102};Cyc_Tcdecl_merr(loc,msg,_tag_arr("enum %s, field %s, value mismatch",
sizeof(unsigned char),34),_tag_arr(_tmp100,sizeof(void*),2));}}});_tmpE0=0;}}}d2=
d0;goto _LL61;_LL61:;}if(! _tmpE0){return 0;}if((void*)d2->sc == _tmpE3){return(
struct Cyc_Absyn_Enumdecl*)d2;}else{d2=({struct Cyc_Absyn_Enumdecl*_tmp103=
_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_tmp103[0]=*d2;_tmp103;});(void*)(d2->sc=(
void*)_tmpE3);return(struct Cyc_Absyn_Enumdecl*)d2;}}}inline static struct _tuple3
Cyc_Tcdecl_check_var_or_fn_decl(void*sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct
Cyc_List_List*atts0,void*sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*
atts1,struct _tagged_arr t,struct _tagged_arr v,struct Cyc_Position_Segment*loc,
struct _tagged_arr*msg){int _tmp104=1;int _tmp106;void*_tmp107;struct _tuple3 _tmp105=
Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg);_LL7A: _tmp107=_tmp105.f1;goto _LL79;
_LL79: _tmp106=_tmp105.f2;goto _LL78;_LL78: if(! _tmp106){_tmp104=0;}if(! Cyc_Tcdecl_check_type(
t0,t1)){({struct Cyc_Std_String_pa_struct _tmp10C;_tmp10C.tag=0;_tmp10C.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t0);{struct Cyc_Std_String_pa_struct _tmp10B;
_tmp10B.tag=0;_tmp10B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{struct
Cyc_Std_String_pa_struct _tmp10A;_tmp10A.tag=0;_tmp10A.f1=(struct _tagged_arr)v;{
struct Cyc_Std_String_pa_struct _tmp109;_tmp109.tag=0;_tmp109.f1=(struct
_tagged_arr)t;{void*_tmp108[4]={& _tmp109,& _tmp10A,& _tmp10B,& _tmp10C};Cyc_Tcdecl_merr(
loc,msg,_tag_arr("%s %s has type \n%s\n instead of \n%s\n",sizeof(unsigned char),
36),_tag_arr(_tmp108,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();
_tmp104=0;}if(! Cyc_Tcutil_equal_tqual(tq0,tq1)){({struct Cyc_Std_String_pa_struct
_tmp10F;_tmp10F.tag=0;_tmp10F.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmp10E;_tmp10E.tag=0;_tmp10E.f1=(struct _tagged_arr)t;{void*_tmp10D[2]={& _tmp10E,&
_tmp10F};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s has different type qualifiers",
sizeof(unsigned char),36),_tag_arr(_tmp10D,sizeof(void*),2));}}});_tmp104=0;}if(
! Cyc_Tcutil_same_atts(atts0,atts1)){({struct Cyc_Std_String_pa_struct _tmp112;
_tmp112.tag=0;_tmp112.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmp111;_tmp111.tag=0;_tmp111.f1=(struct _tagged_arr)t;{void*_tmp110[2]={& _tmp111,&
_tmp112};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s has different attributes",
sizeof(unsigned char),31),_tag_arr(_tmp110,sizeof(void*),2));}}});({void*_tmp113[
0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("previous attributes: ",sizeof(
unsigned char),22),_tag_arr(_tmp113,sizeof(void*),0));});for(0;atts0 != 0;atts0=
atts0->tl){({struct Cyc_Std_String_pa_struct _tmp115;_tmp115.tag=0;_tmp115.f1=(
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts0->hd);{void*_tmp114[1]={&
_tmp115};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s ",sizeof(unsigned char),4),
_tag_arr(_tmp114,sizeof(void*),1));}});}({void*_tmp116[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\ncurrent attributes: ",sizeof(unsigned char),22),_tag_arr(_tmp116,
sizeof(void*),0));});for(0;atts1 != 0;atts1=atts1->tl){({struct Cyc_Std_String_pa_struct
_tmp118;_tmp118.tag=0;_tmp118.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts1->hd);{void*_tmp117[1]={& _tmp118};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%s ",sizeof(unsigned char),4),_tag_arr(_tmp117,sizeof(void*),1));}});}({
void*_tmp119[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n",sizeof(
unsigned char),2),_tag_arr(_tmp119,sizeof(void*),0));});_tmp104=0;}return({
struct _tuple3 _tmp11A;_tmp11A.f1=_tmp107;_tmp11A.f2=_tmp104;_tmp11A;});}struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){struct _tagged_arr _tmp11B=
Cyc_Absynpp_qvar2string(d0->name);int _tmp11D;void*_tmp11E;struct _tuple3 _tmp11C=
Cyc_Tcdecl_check_var_or_fn_decl((void*)d0->sc,(void*)d0->type,d0->tq,d0->attributes,(
void*)d1->sc,(void*)d1->type,d1->tq,d1->attributes,_tag_arr("variable",sizeof(
unsigned char),9),_tmp11B,loc,msg);_LL7D: _tmp11E=_tmp11C.f1;goto _LL7C;_LL7C:
_tmp11D=_tmp11C.f2;goto _LL7B;_LL7B: if(! _tmp11D){return 0;}if((void*)d0->sc == 
_tmp11E){return(struct Cyc_Absyn_Vardecl*)d0;}else{struct Cyc_Absyn_Vardecl*
_tmp11F=({struct Cyc_Absyn_Vardecl*_tmp120=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));
_tmp120[0]=*d0;_tmp120;});(void*)(((struct Cyc_Absyn_Vardecl*)_check_null(_tmp11F))->sc=(
void*)_tmp11E);return _tmp11F;}}struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg){struct _tagged_arr _tmp121=Cyc_Absynpp_qvar2string(d0->name);
if(! Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,_tag_arr("typedef",sizeof(unsigned char),
8),_tmp121,loc,msg)){return 0;}{struct Cyc_List_List*_tmp122=Cyc_Tcdecl_build_tvs_map(
d0->tvs,d1->tvs);void*subst_defn1=Cyc_Tcutil_substitute(_tmp122,(void*)d1->defn);
if(! Cyc_Tcdecl_check_type((void*)d0->defn,subst_defn1)){({struct Cyc_Std_String_pa_struct
_tmp126;_tmp126.tag=0;_tmp126.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)
d0->defn);{struct Cyc_Std_String_pa_struct _tmp125;_tmp125.tag=0;_tmp125.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(subst_defn1);{struct Cyc_Std_String_pa_struct
_tmp124;_tmp124.tag=0;_tmp124.f1=(struct _tagged_arr)_tmp121;{void*_tmp123[3]={&
_tmp124,& _tmp125,& _tmp126};Cyc_Tcdecl_merr(loc,msg,_tag_arr("typedef %s does not refer to the same type: %s != %s",
sizeof(unsigned char),53),_tag_arr(_tmp123,sizeof(void*),3));}}}});return 0;}
return(struct Cyc_Absyn_Typedefdecl*)d0;}}void*Cyc_Tcdecl_merge_binding(void*b0,
void*b1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){struct _tuple4
_tmp128=({struct _tuple4 _tmp127;_tmp127.f1=b0;_tmp127.f2=b1;_tmp127;});void*
_tmp129;void*_tmp12A;void*_tmp12B;struct Cyc_Absyn_Vardecl*_tmp12C;void*_tmp12D;
struct Cyc_Absyn_Vardecl*_tmp12E;void*_tmp12F;struct Cyc_Absyn_Fndecl*_tmp130;void*
_tmp131;struct Cyc_Absyn_Vardecl*_tmp132;void*_tmp133;struct Cyc_Absyn_Fndecl*
_tmp134;void*_tmp135;void*_tmp136;struct Cyc_Absyn_Vardecl*_tmp137;void*_tmp138;
struct Cyc_Absyn_Fndecl*_tmp139;_LL7F: _LL8C: _tmp12A=_tmp128.f1;if((int)_tmp12A == 
0){goto _LL8B;}else{goto _LL81;}_LL8B: _tmp129=_tmp128.f2;if((int)_tmp129 == 0){goto
_LL80;}else{goto _LL81;}_LL81: _LL8F: _tmp12D=_tmp128.f1;if((unsigned int)_tmp12D > 
1?*((int*)_tmp12D)== 0: 0){_LL90: _tmp12E=((struct Cyc_Absyn_Global_b_struct*)
_tmp12D)->f1;goto _LL8D;}else{goto _LL83;}_LL8D: _tmp12B=_tmp128.f2;if((
unsigned int)_tmp12B > 1?*((int*)_tmp12B)== 0: 0){_LL8E: _tmp12C=((struct Cyc_Absyn_Global_b_struct*)
_tmp12B)->f1;goto _LL82;}else{goto _LL83;}_LL83: _LL93: _tmp131=_tmp128.f1;if((
unsigned int)_tmp131 > 1?*((int*)_tmp131)== 0: 0){_LL94: _tmp132=((struct Cyc_Absyn_Global_b_struct*)
_tmp131)->f1;goto _LL91;}else{goto _LL85;}_LL91: _tmp12F=_tmp128.f2;if((
unsigned int)_tmp12F > 1?*((int*)_tmp12F)== 1: 0){_LL92: _tmp130=((struct Cyc_Absyn_Funname_b_struct*)
_tmp12F)->f1;goto _LL84;}else{goto _LL85;}_LL85: _LL97: _tmp135=_tmp128.f1;if((
unsigned int)_tmp135 > 1?*((int*)_tmp135)== 1: 0){goto _LL95;}else{goto _LL87;}_LL95:
_tmp133=_tmp128.f2;if((unsigned int)_tmp133 > 1?*((int*)_tmp133)== 1: 0){_LL96:
_tmp134=((struct Cyc_Absyn_Funname_b_struct*)_tmp133)->f1;goto _LL86;}else{goto
_LL87;}_LL87: _LL9A: _tmp138=_tmp128.f1;if((unsigned int)_tmp138 > 1?*((int*)
_tmp138)== 1: 0){_LL9B: _tmp139=((struct Cyc_Absyn_Funname_b_struct*)_tmp138)->f1;
goto _LL98;}else{goto _LL89;}_LL98: _tmp136=_tmp128.f2;if((unsigned int)_tmp136 > 1?*((
int*)_tmp136)== 0: 0){_LL99: _tmp137=((struct Cyc_Absyn_Global_b_struct*)_tmp136)->f1;
goto _LL88;}else{goto _LL89;}_LL89: goto _LL8A;_LL80: return(void*)0;_LL82: {struct
Cyc_Absyn_Vardecl*_tmp13A=Cyc_Tcdecl_merge_vardecl(_tmp12E,_tmp12C,loc,msg);if(
_tmp13A == 0){return(void*)0;}if((struct Cyc_Absyn_Vardecl*)_tmp12E == _tmp13A){
return b0;}else{if((struct Cyc_Absyn_Vardecl*)_tmp12C == _tmp13A){return b1;}else{
return(void*)({struct Cyc_Absyn_Global_b_struct*_tmp13B=_cycalloc(sizeof(struct
Cyc_Absyn_Global_b_struct));_tmp13B[0]=({struct Cyc_Absyn_Global_b_struct _tmp13C;
_tmp13C.tag=0;_tmp13C.f1=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp13A);_tmp13C;});
_tmp13B;});}}}_LL84: {int _tmp13E;struct _tuple3 _tmp13D=Cyc_Tcdecl_check_var_or_fn_decl((
void*)_tmp132->sc,(void*)_tmp132->type,_tmp132->tq,_tmp132->attributes,(void*)
_tmp130->sc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp130->cached_typ))->v,
Cyc_Absyn_empty_tqual(),_tmp130->attributes,_tag_arr("function",sizeof(
unsigned char),9),Cyc_Absynpp_qvar2string(_tmp132->name),loc,msg);_LL9D: _tmp13E=
_tmp13D.f2;goto _LL9C;_LL9C: return _tmp13E? b1:(void*)0;}_LL86:({struct Cyc_Std_String_pa_struct
_tmp140;_tmp140.tag=0;_tmp140.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp134->name);{void*_tmp13F[1]={& _tmp140};Cyc_Tcdecl_merr(loc,msg,_tag_arr("redefinition of function %s",
sizeof(unsigned char),28),_tag_arr(_tmp13F,sizeof(void*),1));}});return(void*)0;
_LL88: {int _tmp142;struct _tuple3 _tmp141=Cyc_Tcdecl_check_var_or_fn_decl((void*)
_tmp139->sc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp139->cached_typ))->v,
Cyc_Absyn_empty_tqual(),_tmp139->attributes,(void*)_tmp137->sc,(void*)_tmp137->type,
_tmp137->tq,_tmp137->attributes,_tag_arr("variable",sizeof(unsigned char),9),Cyc_Absynpp_qvar2string(
_tmp139->name),loc,msg);_LL9F: _tmp142=_tmp141.f2;goto _LL9E;_LL9E: return _tmp142?
b0:(void*)0;}_LL8A:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp143=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));_tmp143[0]=({
struct Cyc_Core_Invalid_argument_struct _tmp144;_tmp144.tag=Cyc_Core_Invalid_argument;
_tmp144.f1=_tag_arr("Tcdecl::merge_binding",sizeof(unsigned char),22);_tmp144;});
_tmp143;}));_LL7E:;}struct Cyc_Tcdecl_Xtunionfielddecl*Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl*d0,struct Cyc_Tcdecl_Xtunionfielddecl*d1,struct
Cyc_Position_Segment*loc,struct _tagged_arr*msg){struct Cyc_Tcdecl_Xtunionfielddecl
_tmp146;struct Cyc_Absyn_Tunionfield*_tmp147;struct Cyc_Absyn_Tuniondecl*_tmp148;
struct Cyc_Tcdecl_Xtunionfielddecl*_tmp145=d0;_tmp146=*_tmp145;_LLA2: _tmp148=
_tmp146.base;goto _LLA1;_LLA1: _tmp147=_tmp146.field;goto _LLA0;_LLA0: {struct Cyc_Tcdecl_Xtunionfielddecl
_tmp14A;struct Cyc_Absyn_Tunionfield*_tmp14B;struct Cyc_Absyn_Tuniondecl*_tmp14C;
struct Cyc_Tcdecl_Xtunionfielddecl*_tmp149=d1;_tmp14A=*_tmp149;_LLA5: _tmp14C=
_tmp14A.base;goto _LLA4;_LLA4: _tmp14B=_tmp14A.field;goto _LLA3;_LLA3: {struct
_tagged_arr _tmp14D=Cyc_Absynpp_qvar2string(_tmp147->name);if(Cyc_Absyn_qvar_cmp(
_tmp148->name,_tmp14C->name)!= 0){({struct Cyc_Std_String_pa_struct _tmp151;
_tmp151.tag=0;_tmp151.f1=(struct _tagged_arr)_tmp14D;{struct Cyc_Std_String_pa_struct
_tmp150;_tmp150.tag=0;_tmp150.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp14C->name);{struct Cyc_Std_String_pa_struct _tmp14F;_tmp14F.tag=0;_tmp14F.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp148->name);{void*_tmp14E[3]={&
_tmp14F,& _tmp150,& _tmp151};Cyc_Tcdecl_merr(loc,msg,_tag_arr("xtunions %s and %s have both a field named %s",
sizeof(unsigned char),46),_tag_arr(_tmp14E,sizeof(void*),3));}}}});return 0;}if(!
Cyc_Tcdecl_check_tvs(_tmp148->tvs,_tmp14C->tvs,_tag_arr("xtunion",sizeof(
unsigned char),8),Cyc_Absynpp_qvar2string(_tmp148->name),loc,msg)){return 0;}{
struct Cyc_List_List*_tmp152=Cyc_Tcdecl_build_tvs_map(_tmp148->tvs,_tmp14C->tvs);
struct Cyc_Absyn_Tunionfield*_tmp153=Cyc_Tcdecl_merge_tunionfield(_tmp147,_tmp14B,
_tmp152,_tag_arr("xtunionfield",sizeof(unsigned char),13),_tmp14D,msg);if(
_tmp153 == 0){return 0;}if(_tmp153 == (struct Cyc_Absyn_Tunionfield*)_tmp147){return(
struct Cyc_Tcdecl_Xtunionfielddecl*)d0;}else{return({struct Cyc_Tcdecl_Xtunionfielddecl*
_tmp154=_cycalloc(sizeof(struct Cyc_Tcdecl_Xtunionfielddecl));_tmp154->base=
_tmp148;_tmp154->field=(struct Cyc_Absyn_Tunionfield*)_check_null(_tmp153);
_tmp154;});}}}}}
