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
extern struct Cyc_Std___sFILE*Cyc_Std_stdout;extern int Cyc_Std_fputc(int __c,struct
Cyc_Std___sFILE*__stream);extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};extern int Cyc_Std_file_string_write(
struct Cyc_Std___sFILE*fd,struct _tagged_arr src,int src_offset,int max_count);struct
Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern int Cyc_List_length(struct Cyc_List_List*x);
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
extern unsigned char Cyc_List_List_mismatch[18];extern struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern unsigned char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];extern struct
_tagged_arr Cyc_Position_get_line_directive(struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};
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
15];extern void*Cyc_Absyn_compress_kb(void*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
extern struct _tagged_arr Cyc_Absyn_attribute2string(void*);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
extern int Cyc_Absynpp_is_anon_structtype(void*t);extern struct _tagged_arr*Cyc_Absynpp_cyc_stringptr;
extern int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);extern struct _tagged_arr Cyc_Absynpp_char_escape(
unsigned char);extern struct _tagged_arr Cyc_Absynpp_string_escape(struct
_tagged_arr);extern struct _tagged_arr Cyc_Absynpp_prim2str(void*p);extern int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);struct _tuple3{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};extern struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple3*arg);
struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};extern
struct _tuple4 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t);extern
unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Set_Set;
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
le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_compress(
void*t);static int Cyc_Absyndump_expand_typedefs;static int Cyc_Absyndump_qvar_to_Cids;
static int Cyc_Absyndump_add_cyc_prefix;static int Cyc_Absyndump_generate_line_directives;
static int Cyc_Absyndump_to_VC;void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*
fs){Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;Cyc_Absyndump_qvar_to_Cids=
fs->qvar_to_Cids;Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absyndump_to_VC=
fs->to_VC;Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_set_params(fs);}void Cyc_Absyndump_dumptyp(void*);void Cyc_Absyndump_dumpntyp(
void*t);void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumpexp_prec(
int,struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);void
Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*);void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*);void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(
void*),void*a);void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(
void*),void*);void Cyc_Absyndump_dumpstructfields(struct Cyc_List_List*fields);
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);void Cyc_Absyndump_dumploc(
struct Cyc_Position_Segment*);struct Cyc_Std___sFILE**Cyc_Absyndump_dump_file=& Cyc_Std_stdout;
void Cyc_Absyndump_ignore(void*x){return;}static unsigned int Cyc_Absyndump_pos=0;
static unsigned char Cyc_Absyndump_prev_char='x';int Cyc_Absyndump_need_space_before(){
switch(Cyc_Absyndump_prev_char){case '{': _LL0: goto _LL1;case '}': _LL1: goto _LL2;case
'(': _LL2: goto _LL3;case ')': _LL3: goto _LL4;case '[': _LL4: goto _LL5;case ']': _LL5: goto
_LL6;case ';': _LL6: goto _LL7;case ',': _LL7: goto _LL8;case '=': _LL8: goto _LL9;case ' ':
_LL9: goto _LLA;case '\n': _LLA: goto _LLB;case '*': _LLB: return 0;default: _LLC: return 1;}}
void Cyc_Absyndump_dump(struct _tagged_arr s){int sz=(int)_get_arr_size(s,sizeof(
unsigned char));if(!((int)*((const unsigned char*)_check_unknown_subscript(s,
sizeof(unsigned char),sz - 1)))){-- sz;}Cyc_Absyndump_pos +=sz;if(Cyc_Absyndump_pos
> 80){Cyc_Absyndump_pos=(unsigned int)sz;Cyc_Std_fputc((int)'\n',*Cyc_Absyndump_dump_file);}
else{if(Cyc_Absyndump_need_space_before()){Cyc_Std_fputc((int)' ',*Cyc_Absyndump_dump_file);}}
Cyc_Std_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);if(sz >= 1){Cyc_Absyndump_prev_char=*((
const unsigned char*)_check_unknown_subscript(s,sizeof(unsigned char),sz - 1));}}
void Cyc_Absyndump_dump_nospace(struct _tagged_arr s){int sz=(int)_get_arr_size(s,
sizeof(unsigned char));if(!((int)*((const unsigned char*)_check_unknown_subscript(
s,sizeof(unsigned char),sz - 1)))){-- sz;}Cyc_Absyndump_pos +=sz;Cyc_Std_file_string_write(*
Cyc_Absyndump_dump_file,s,0,sz);if(sz >= 1){Cyc_Absyndump_prev_char=*((const
unsigned char*)_check_unknown_subscript(s,sizeof(unsigned char),sz - 1));}}void
Cyc_Absyndump_dump_char(int c){++ Cyc_Absyndump_pos;Cyc_Std_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(unsigned char)c;}void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*
loc){if(loc == 0){return;}if(! Cyc_Absyndump_generate_line_directives){return;}{
struct _tagged_arr _tmp0=Cyc_Position_get_line_directive(loc);Cyc_Absyndump_dump(
_tmp0);}}void Cyc_Absyndump_dump_str(struct _tagged_arr*s){Cyc_Absyndump_dump(*s);}
void Cyc_Absyndump_dump_semi(){Cyc_Absyndump_dump_char((int)';');}void Cyc_Absyndump_dump_sep(
void(*f)(void*),struct Cyc_List_List*l,struct _tagged_arr sep){if(l == 0){return;}
for(0;l->tl != 0;l=l->tl){f((void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f((
void*)l->hd);}void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct
Cyc_List_List*l,struct _tagged_arr sep){if(l == 0){return;}for(0;l->tl != 0;l=l->tl){
f(env,(void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f(env,(void*)l->hd);}void
Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep){Cyc_Absyndump_dump_nospace(start);Cyc_Absyndump_dump_sep(
f,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_group_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep){Cyc_Absyndump_dump_nospace(start);Cyc_Absyndump_dump_sep_c(
f,env,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_egroup(void(*f)(
void*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep){if(l != 0){Cyc_Absyndump_group(f,l,start,end,sep);}}void Cyc_Absyndump_dumpqvar(
struct _tuple0*v){struct Cyc_List_List*_tmp1=0;{void*_tmp2=(*v).f1;struct Cyc_List_List*
_tmp3;struct Cyc_List_List*_tmp4;_LLF: if((int)_tmp2 == 0){goto _LL10;}else{goto
_LL11;}_LL11: if((unsigned int)_tmp2 > 1?*((int*)_tmp2)== 0: 0){_LL15: _tmp3=((
struct Cyc_Absyn_Rel_n_struct*)_tmp2)->f1;goto _LL12;}else{goto _LL13;}_LL13: if((
unsigned int)_tmp2 > 1?*((int*)_tmp2)== 1: 0){_LL16: _tmp4=((struct Cyc_Absyn_Abs_n_struct*)
_tmp2)->f1;goto _LL14;}else{goto _LLE;}_LL10: _tmp3=0;goto _LL12;_LL12: _tmp1=_tmp3;
goto _LLE;_LL14: _tmp1=(Cyc_Absyndump_qvar_to_Cids? Cyc_Absyndump_add_cyc_prefix: 0)?({
struct Cyc_List_List*_tmp5=_cycalloc(sizeof(struct Cyc_List_List));_tmp5->hd=Cyc_Absynpp_cyc_stringptr;
_tmp5->tl=_tmp4;_tmp5;}): _tmp4;goto _LLE;_LLE:;}if(_tmp1 != 0){Cyc_Absyndump_dump_str((
struct _tagged_arr*)_tmp1->hd);for(_tmp1=_tmp1->tl;_tmp1 != 0;_tmp1=_tmp1->tl){if(
Cyc_Absyndump_qvar_to_Cids){Cyc_Absyndump_dump_char((int)'_');}else{Cyc_Absyndump_dump_nospace(
_tag_arr("::",sizeof(unsigned char),3));}Cyc_Absyndump_dump_nospace(*((struct
_tagged_arr*)_tmp1->hd));}if(Cyc_Absyndump_qvar_to_Cids){Cyc_Absyndump_dump_nospace(
_tag_arr("_",sizeof(unsigned char),2));}else{Cyc_Absyndump_dump_nospace(_tag_arr("::",
sizeof(unsigned char),3));}Cyc_Absyndump_dump_nospace(*(*v).f2);}else{Cyc_Absyndump_dump_str((*
v).f2);}}void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict){Cyc_Absyndump_dump(
_tag_arr("restrict",sizeof(unsigned char),9));}if(tq.q_volatile){Cyc_Absyndump_dump(
_tag_arr("volatile",sizeof(unsigned char),9));}if(tq.q_const){Cyc_Absyndump_dump(
_tag_arr("const",sizeof(unsigned char),6));}}void Cyc_Absyndump_dumpscope(void*sc){
void*_tmp6=sc;_LL18: if((int)_tmp6 == 0){goto _LL19;}else{goto _LL1A;}_LL1A: if((int)
_tmp6 == 2){goto _LL1B;}else{goto _LL1C;}_LL1C: if((int)_tmp6 == 3){goto _LL1D;}else{
goto _LL1E;}_LL1E: if((int)_tmp6 == 4){goto _LL1F;}else{goto _LL20;}_LL20: if((int)
_tmp6 == 1){goto _LL21;}else{goto _LL17;}_LL19: Cyc_Absyndump_dump(_tag_arr("static",
sizeof(unsigned char),7));return;_LL1B: return;_LL1D: Cyc_Absyndump_dump(_tag_arr("extern",
sizeof(unsigned char),7));return;_LL1F: Cyc_Absyndump_dump(_tag_arr("extern \"C\"",
sizeof(unsigned char),11));return;_LL21: Cyc_Absyndump_dump(_tag_arr("abstract",
sizeof(unsigned char),9));return;_LL17:;}void Cyc_Absyndump_dumpkind(void*k){void*
_tmp7=k;_LL23: if((int)_tmp7 == 0){goto _LL24;}else{goto _LL25;}_LL25: if((int)_tmp7
== 1){goto _LL26;}else{goto _LL27;}_LL27: if((int)_tmp7 == 2){goto _LL28;}else{goto
_LL29;}_LL29: if((int)_tmp7 == 3){goto _LL2A;}else{goto _LL2B;}_LL2B: if((int)_tmp7 == 
4){goto _LL2C;}else{goto _LL22;}_LL24: Cyc_Absyndump_dump(_tag_arr("A",sizeof(
unsigned char),2));return;_LL26: Cyc_Absyndump_dump(_tag_arr("M",sizeof(
unsigned char),2));return;_LL28: Cyc_Absyndump_dump(_tag_arr("B",sizeof(
unsigned char),2));return;_LL2A: Cyc_Absyndump_dump(_tag_arr("R",sizeof(
unsigned char),2));return;_LL2C: Cyc_Absyndump_dump(_tag_arr("E",sizeof(
unsigned char),2));return;_LL22:;}void Cyc_Absyndump_dumptps(struct Cyc_List_List*
ts){Cyc_Absyndump_egroup(Cyc_Absyndump_dumptyp,ts,_tag_arr("<",sizeof(
unsigned char),2),_tag_arr(">",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2));}void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);}void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);{void*_tmp8=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp9;void*
_tmpA;_LL2E: if(*((int*)_tmp8)== 0){_LL36: _tmp9=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp8)->f1;if((int)_tmp9 == 2){goto _LL2F;}else{goto _LL30;}}else{goto _LL30;}_LL30:
if(*((int*)_tmp8)== 2){goto _LL31;}else{goto _LL32;}_LL32: if(*((int*)_tmp8)== 1){
goto _LL33;}else{goto _LL34;}_LL34: if(*((int*)_tmp8)== 0){_LL37: _tmpA=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp8)->f1;goto _LL35;}else{goto _LL2D;}_LL2F: goto
_LL31;_LL31: goto _LL33;_LL33: Cyc_Absyndump_dump(_tag_arr("::?",sizeof(
unsigned char),4));goto _LL2D;_LL35: Cyc_Absyndump_dump(_tag_arr("::",sizeof(
unsigned char),3));Cyc_Absyndump_dumpkind(_tmpA);goto _LL2D;_LL2D:;}}void Cyc_Absyndump_dumptvars(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,tvs,_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar,tvs,_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}struct _tuple5{
struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(struct _tuple5*pr){((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(
struct Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple5*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumparg,ts,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dump_callconv(
struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmpB=(void*)atts->hd;
_LL39: if((int)_tmpB == 0){goto _LL3A;}else{goto _LL3B;}_LL3B: if((int)_tmpB == 1){
goto _LL3C;}else{goto _LL3D;}_LL3D: if((int)_tmpB == 2){goto _LL3E;}else{goto _LL3F;}
_LL3F: goto _LL40;_LL3A: Cyc_Absyndump_dump(_tag_arr("_stdcall",sizeof(
unsigned char),9));return;_LL3C: Cyc_Absyndump_dump(_tag_arr("_cdecl",sizeof(
unsigned char),7));return;_LL3E: Cyc_Absyndump_dump(_tag_arr("_fastcall",sizeof(
unsigned char),10));return;_LL40: goto _LL38;_LL38:;}}void Cyc_Absyndump_dump_noncallconv(
struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;
atts2 != 0;atts2=atts2->tl){void*_tmpC=(void*)((struct Cyc_List_List*)_check_null(
atts))->hd;_LL42: if((int)_tmpC == 0){goto _LL43;}else{goto _LL44;}_LL44: if((int)
_tmpC == 1){goto _LL45;}else{goto _LL46;}_LL46: if((int)_tmpC == 2){goto _LL47;}else{
goto _LL48;}_LL48: goto _LL49;_LL43: goto _LL41;_LL45: goto _LL41;_LL47: goto _LL41;_LL49:
hasatt=1;goto _LL41;_LL41:;}}if(! hasatt){return;}Cyc_Absyndump_dump(_tag_arr("__declspec(",
sizeof(unsigned char),12));for(0;atts != 0;atts=atts->tl){void*_tmpD=(void*)atts->hd;
_LL4B: if((int)_tmpD == 0){goto _LL4C;}else{goto _LL4D;}_LL4D: if((int)_tmpD == 1){
goto _LL4E;}else{goto _LL4F;}_LL4F: if((int)_tmpD == 2){goto _LL50;}else{goto _LL51;}
_LL51: goto _LL52;_LL4C: goto _LL4A;_LL4E: goto _LL4A;_LL50: goto _LL4A;_LL52: Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL4A;_LL4A:;}Cyc_Absyndump_dump_char((
int)')');}void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){if(atts == 0){
return;}if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dump_noncallconv(atts);return;}Cyc_Absyndump_dump(
_tag_arr(" __attribute__((",sizeof(unsigned char),17));for(0;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));if(atts->tl != 0){
Cyc_Absyndump_dump(_tag_arr(",",sizeof(unsigned char),2));}}Cyc_Absyndump_dump(
_tag_arr(")) ",sizeof(unsigned char),4));}int Cyc_Absyndump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0){return 0;}{void*_tmpE=(void*)tms->hd;_LL54:
if((unsigned int)_tmpE > 1?*((int*)_tmpE)== 1: 0){goto _LL55;}else{goto _LL56;}_LL56:
goto _LL57;_LL55: return 1;_LL57: return 0;_LL53:;}}static void Cyc_Absyndump_dumprgn(
void*t){void*_tmpF=Cyc_Tcutil_compress(t);_LL59: if((int)_tmpF == 2){goto _LL5A;}
else{goto _LL5B;}_LL5B: goto _LL5C;_LL5A: Cyc_Absyndump_dump(_tag_arr("`H",sizeof(
unsigned char),3));goto _LL58;_LL5C: Cyc_Absyndump_dumpntyp(t);goto _LL58;_LL58:;}
struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static struct
_tuple6 Cyc_Absyndump_effects_split(void*t){struct Cyc_List_List*rgions=0;struct
Cyc_List_List*effects=0;{void*_tmp10=Cyc_Tcutil_compress(t);void*_tmp11;struct
Cyc_List_List*_tmp12;_LL5E: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 19: 0){
_LL64: _tmp11=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp10)->f1;goto _LL5F;}
else{goto _LL60;}_LL60: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 20: 0){_LL65:
_tmp12=((struct Cyc_Absyn_JoinEff_struct*)_tmp10)->f1;goto _LL61;}else{goto _LL62;}
_LL62: goto _LL63;_LL5F: rgions=({struct Cyc_List_List*_tmp13=_cycalloc(sizeof(
struct Cyc_List_List));_tmp13->hd=(void*)_tmp11;_tmp13->tl=rgions;_tmp13;});goto
_LL5D;_LL61: for(0;_tmp12 != 0;_tmp12=_tmp12->tl){struct Cyc_List_List*_tmp15;
struct Cyc_List_List*_tmp16;struct _tuple6 _tmp14=Cyc_Absyndump_effects_split((void*)
_tmp12->hd);_LL68: _tmp16=_tmp14.f1;goto _LL67;_LL67: _tmp15=_tmp14.f2;goto _LL66;
_LL66: rgions=Cyc_List_imp_append(_tmp16,rgions);effects=Cyc_List_imp_append(
_tmp15,effects);}goto _LL5D;_LL63: effects=({struct Cyc_List_List*_tmp17=_cycalloc(
sizeof(struct Cyc_List_List));_tmp17->hd=(void*)t;_tmp17->tl=effects;_tmp17;});
goto _LL5D;_LL5D:;}return({struct _tuple6 _tmp18;_tmp18.f1=rgions;_tmp18.f2=effects;
_tmp18;});}static void Cyc_Absyndump_dumpeff(void*t){struct Cyc_List_List*_tmp1A;
struct Cyc_List_List*_tmp1B;struct _tuple6 _tmp19=Cyc_Absyndump_effects_split(t);
_LL6B: _tmp1B=_tmp19.f1;goto _LL6A;_LL6A: _tmp1A=_tmp19.f2;goto _LL69;_LL69: _tmp1B=
Cyc_List_imp_rev(_tmp1B);_tmp1A=Cyc_List_imp_rev(_tmp1A);for(0;_tmp1A != 0;_tmp1A=
_tmp1A->tl){Cyc_Absyndump_dumpntyp((void*)_tmp1A->hd);Cyc_Absyndump_dump_char((
int)'+');}Cyc_Absyndump_dump_char((int)'{');for(0;_tmp1B != 0;_tmp1B=_tmp1B->tl){
Cyc_Absyndump_dumprgn((void*)_tmp1B->hd);if(_tmp1B->tl != 0){Cyc_Absyndump_dump_char((
int)',');}}Cyc_Absyndump_dump_char((int)'}');}void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp1C=t;struct Cyc_Absyn_Tvar*_tmp1D;int _tmp1E;struct Cyc_Core_Opt*_tmp1F;
struct Cyc_Core_Opt*_tmp20;int _tmp21;struct Cyc_Core_Opt*_tmp22;struct Cyc_Core_Opt
_tmp23;void*_tmp24;struct Cyc_Core_Opt*_tmp25;struct Cyc_Absyn_TunionInfo _tmp26;
void*_tmp27;struct Cyc_List_List*_tmp28;void*_tmp29;struct Cyc_Absyn_TunionFieldInfo
_tmp2A;struct Cyc_List_List*_tmp2B;void*_tmp2C;struct _tuple0*_tmp2D;void*_tmp2E;
void*_tmp2F;void*_tmp30;void*_tmp31;void*_tmp32;void*_tmp33;void*_tmp34;void*
_tmp35;void*_tmp36;void*_tmp37;void*_tmp38;void*_tmp39;void*_tmp3A;void*_tmp3B;
void*_tmp3C;void*_tmp3D;void*_tmp3E;void*_tmp3F;void*_tmp40;void*_tmp41;int
_tmp42;struct Cyc_List_List*_tmp43;struct Cyc_List_List*_tmp44;struct _tuple0*
_tmp45;struct Cyc_List_List*_tmp46;struct _tuple0*_tmp47;struct Cyc_List_List*
_tmp48;struct _tuple0*_tmp49;struct Cyc_List_List*_tmp4A;struct _tuple0*_tmp4B;
struct Cyc_List_List*_tmp4C;struct Cyc_List_List*_tmp4D;struct Cyc_List_List*_tmp4E;
struct Cyc_Core_Opt*_tmp4F;struct Cyc_List_List*_tmp50;struct _tuple0*_tmp51;void*
_tmp52;void*_tmp53;_LL6D: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 7: 0){goto
_LL6E;}else{goto _LL6F;}_LL6F: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 8: 0){
goto _LL70;}else{goto _LL71;}_LL71: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 4:
0){goto _LL72;}else{goto _LL73;}_LL73: if((int)_tmp1C == 0){goto _LL74;}else{goto
_LL75;}_LL75: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 1: 0){_LLB7: _tmp1D=((
struct Cyc_Absyn_VarType_struct*)_tmp1C)->f1;goto _LL76;}else{goto _LL77;}_LL77: if((
unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 0: 0){_LLBA: _tmp20=((struct Cyc_Absyn_Evar_struct*)
_tmp1C)->f1;goto _LLB9;_LLB9: _tmp1F=((struct Cyc_Absyn_Evar_struct*)_tmp1C)->f2;
if(_tmp1F == 0){goto _LLB8;}else{goto _LL79;}_LLB8: _tmp1E=((struct Cyc_Absyn_Evar_struct*)
_tmp1C)->f3;goto _LL78;}else{goto _LL79;}_LL79: if((unsigned int)_tmp1C > 3?*((int*)
_tmp1C)== 0: 0){_LLBE: _tmp25=((struct Cyc_Absyn_Evar_struct*)_tmp1C)->f1;goto _LLBC;
_LLBC: _tmp22=((struct Cyc_Absyn_Evar_struct*)_tmp1C)->f2;if(_tmp22 == 0){goto _LL7B;}
else{_tmp23=*_tmp22;_LLBD: _tmp24=(void*)_tmp23.v;goto _LLBB;}_LLBB: _tmp21=((
struct Cyc_Absyn_Evar_struct*)_tmp1C)->f3;goto _LL7A;}else{goto _LL7B;}_LL7B: if((
unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 2: 0){_LLBF: _tmp26=((struct Cyc_Absyn_TunionType_struct*)
_tmp1C)->f1;_LLC2: _tmp29=(void*)_tmp26.tunion_info;goto _LLC1;_LLC1: _tmp28=_tmp26.targs;
goto _LLC0;_LLC0: _tmp27=(void*)_tmp26.rgn;goto _LL7C;}else{goto _LL7D;}_LL7D: if((
unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 3: 0){_LLC3: _tmp2A=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp1C)->f1;_LLC5: _tmp2C=(void*)_tmp2A.field_info;goto _LLC4;_LLC4: _tmp2B=_tmp2A.targs;
goto _LL7E;}else{goto _LL7F;}_LL7F: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 12:
0){_LLC6: _tmp2D=((struct Cyc_Absyn_EnumType_struct*)_tmp1C)->f1;goto _LL80;}else{
goto _LL81;}_LL81: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 5: 0){_LLC8: _tmp2F=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp1C)->f1;if((int)_tmp2F == 0){goto
_LLC7;}else{goto _LL83;}_LLC7: _tmp2E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1C)->f2;if((int)_tmp2E == 2){goto _LL82;}else{goto _LL83;}}else{goto _LL83;}
_LL83: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 5: 0){_LLCA: _tmp31=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1C)->f1;if((int)_tmp31 == 0){goto _LLC9;}else{
goto _LL85;}_LLC9: _tmp30=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1C)->f2;if((
int)_tmp30 == 0){goto _LL84;}else{goto _LL85;}}else{goto _LL85;}_LL85: if((
unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 5: 0){_LLCC: _tmp33=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1C)->f1;if((int)_tmp33 == 1){goto _LLCB;}else{goto _LL87;}_LLCB: _tmp32=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1C)->f2;if((int)_tmp32 == 0){goto _LL86;}else{
goto _LL87;}}else{goto _LL87;}_LL87: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 5:
0){_LLCE: _tmp35=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1C)->f1;if((int)
_tmp35 == 0){goto _LLCD;}else{goto _LL89;}_LLCD: _tmp34=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1C)->f2;if((int)_tmp34 == 1){goto _LL88;}else{goto _LL89;}}else{goto _LL89;}
_LL89: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 5: 0){_LLD0: _tmp37=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1C)->f1;if((int)_tmp37 == 1){goto _LLCF;}else{
goto _LL8B;}_LLCF: _tmp36=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1C)->f2;if((
int)_tmp36 == 1){goto _LL8A;}else{goto _LL8B;}}else{goto _LL8B;}_LL8B: if((
unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 5: 0){_LLD2: _tmp39=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1C)->f1;if((int)_tmp39 == 1){goto _LLD1;}else{goto _LL8D;}_LLD1: _tmp38=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1C)->f2;if((int)_tmp38 == 2){goto _LL8C;}else{
goto _LL8D;}}else{goto _LL8D;}_LL8D: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 5:
0){_LLD5: _tmp3B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1C)->f1;if((int)
_tmp3B == 0){goto _LLD4;}else{goto _LL8F;}_LLD4: _tmp3A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1C)->f2;if((int)_tmp3A == 3){goto _LLD3;}else{goto _LL8F;}}else{goto _LL8F;}
_LLD3: if(Cyc_Absyndump_to_VC){goto _LL8E;}else{goto _LL8F;}_LL8F: if((unsigned int)
_tmp1C > 3?*((int*)_tmp1C)== 5: 0){_LLD7: _tmp3D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1C)->f1;if((int)_tmp3D == 0){goto _LLD6;}else{goto _LL91;}_LLD6: _tmp3C=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1C)->f2;if((int)_tmp3C == 3){goto _LL90;}else{
goto _LL91;}}else{goto _LL91;}_LL91: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 5:
0){_LLDA: _tmp3F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1C)->f1;if((int)
_tmp3F == 1){goto _LLD9;}else{goto _LL93;}_LLD9: _tmp3E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1C)->f2;if((int)_tmp3E == 3){goto _LLD8;}else{goto _LL93;}}else{goto _LL93;}
_LLD8: if(Cyc_Absyndump_to_VC){goto _LL92;}else{goto _LL93;}_LL93: if((unsigned int)
_tmp1C > 3?*((int*)_tmp1C)== 5: 0){_LLDC: _tmp41=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1C)->f1;if((int)_tmp41 == 1){goto _LLDB;}else{goto _LL95;}_LLDB: _tmp40=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1C)->f2;if((int)_tmp40 == 3){goto _LL94;}else{
goto _LL95;}}else{goto _LL95;}_LL95: if((int)_tmp1C == 1){goto _LL96;}else{goto _LL97;}
_LL97: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 6: 0){_LLDD: _tmp42=((struct Cyc_Absyn_DoubleType_struct*)
_tmp1C)->f1;goto _LL98;}else{goto _LL99;}_LL99: if((unsigned int)_tmp1C > 3?*((int*)
_tmp1C)== 9: 0){_LLDE: _tmp43=((struct Cyc_Absyn_TupleType_struct*)_tmp1C)->f1;goto
_LL9A;}else{goto _LL9B;}_LL9B: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 10: 0){
_LLE0: _tmp45=((struct Cyc_Absyn_StructType_struct*)_tmp1C)->f1;if(_tmp45 == 0){
goto _LLDF;}else{goto _LL9D;}_LLDF: _tmp44=((struct Cyc_Absyn_StructType_struct*)
_tmp1C)->f2;goto _LL9C;}else{goto _LL9D;}_LL9D: if((unsigned int)_tmp1C > 3?*((int*)
_tmp1C)== 10: 0){_LLE2: _tmp47=((struct Cyc_Absyn_StructType_struct*)_tmp1C)->f1;
goto _LLE1;_LLE1: _tmp46=((struct Cyc_Absyn_StructType_struct*)_tmp1C)->f2;goto
_LL9E;}else{goto _LL9F;}_LL9F: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 11: 0){
_LLE4: _tmp49=((struct Cyc_Absyn_UnionType_struct*)_tmp1C)->f1;if(_tmp49 == 0){goto
_LLE3;}else{goto _LLA1;}_LLE3: _tmp48=((struct Cyc_Absyn_UnionType_struct*)_tmp1C)->f2;
goto _LLA0;}else{goto _LLA1;}_LLA1: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 11:
0){_LLE6: _tmp4B=((struct Cyc_Absyn_UnionType_struct*)_tmp1C)->f1;goto _LLE5;_LLE5:
_tmp4A=((struct Cyc_Absyn_UnionType_struct*)_tmp1C)->f2;goto _LLA2;}else{goto _LLA3;}
_LLA3: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 14: 0){_LLE7: _tmp4C=((struct
Cyc_Absyn_AnonStructType_struct*)_tmp1C)->f1;goto _LLA4;}else{goto _LLA5;}_LLA5:
if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 15: 0){_LLE8: _tmp4D=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp1C)->f1;goto _LLA6;}else{goto _LLA7;}_LLA7: if((unsigned int)_tmp1C > 3?*((int*)
_tmp1C)== 16: 0){_LLE9: _tmp4E=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp1C)->f1;
goto _LLA8;}else{goto _LLA9;}_LLA9: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 18:
0){_LLEC: _tmp51=((struct Cyc_Absyn_TypedefType_struct*)_tmp1C)->f1;goto _LLEB;
_LLEB: _tmp50=((struct Cyc_Absyn_TypedefType_struct*)_tmp1C)->f2;goto _LLEA;_LLEA:
_tmp4F=((struct Cyc_Absyn_TypedefType_struct*)_tmp1C)->f3;goto _LLAA;}else{goto
_LLAB;}_LLAB: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 13: 0){_LLED: _tmp52=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp1C)->f1;goto _LLAC;}else{goto _LLAD;}
_LLAD: if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 17: 0){_LLEE: _tmp53=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp1C)->f1;goto _LLAE;}else{goto _LLAF;}
_LLAF: if((int)_tmp1C == 2){goto _LLB0;}else{goto _LLB1;}_LLB1: if((unsigned int)
_tmp1C > 3?*((int*)_tmp1C)== 19: 0){goto _LLB2;}else{goto _LLB3;}_LLB3: if((
unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 21: 0){goto _LLB4;}else{goto _LLB5;}_LLB5:
if((unsigned int)_tmp1C > 3?*((int*)_tmp1C)== 20: 0){goto _LLB6;}else{goto _LL6C;}
_LL6E: return;_LL70: return;_LL72: return;_LL74: Cyc_Absyndump_dump(_tag_arr("void",
sizeof(unsigned char),5));return;_LL76: Cyc_Absyndump_dump_str(_tmp1D->name);
return;_LL78: Cyc_Absyndump_dump(_tag_arr("%",sizeof(unsigned char),2));if(_tmp20
== 0){Cyc_Absyndump_dump(_tag_arr("?",sizeof(unsigned char),2));}else{Cyc_Absyndump_dumpkind((
void*)_tmp20->v);}Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp55;_tmp55.tag=1;_tmp55.f1=(int)((unsigned int)_tmp1E);{void*_tmp54[1]={&
_tmp55};Cyc_Std_aprintf(_tag_arr("(%d)",sizeof(unsigned char),5),_tag_arr(_tmp54,
sizeof(void*),1));}}));return;_LL7A: Cyc_Absyndump_dumpntyp(_tmp24);return;_LL7C:{
void*_tmp56=_tmp29;struct Cyc_Absyn_UnknownTunionInfo _tmp57;int _tmp58;struct
_tuple0*_tmp59;struct Cyc_Absyn_Tuniondecl**_tmp5A;struct Cyc_Absyn_Tuniondecl*
_tmp5B;struct Cyc_Absyn_Tuniondecl _tmp5C;int _tmp5D;struct _tuple0*_tmp5E;_LLF0: if(*((
int*)_tmp56)== 0){_LLF4: _tmp57=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp56)->f1;
_LLF6: _tmp59=_tmp57.name;goto _LLF5;_LLF5: _tmp58=_tmp57.is_xtunion;goto _LLF1;}
else{goto _LLF2;}_LLF2: if(*((int*)_tmp56)== 1){_LLF7: _tmp5A=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp56)->f1;_tmp5B=*_tmp5A;_tmp5C=*_tmp5B;_LLF9: _tmp5E=_tmp5C.name;goto _LLF8;
_LLF8: _tmp5D=_tmp5C.is_xtunion;goto _LLF3;}else{goto _LLEF;}_LLF1: _tmp5E=_tmp59;
_tmp5D=_tmp58;goto _LLF3;_LLF3: if(_tmp5D){Cyc_Absyndump_dump(_tag_arr("xtunion ",
sizeof(unsigned char),9));}else{Cyc_Absyndump_dump(_tag_arr("tunion ",sizeof(
unsigned char),8));}{void*_tmp5F=Cyc_Tcutil_compress(_tmp27);_LLFB: if((int)
_tmp5F == 2){goto _LLFC;}else{goto _LLFD;}_LLFD: goto _LLFE;_LLFC: goto _LLFA;_LLFE: Cyc_Absyndump_dumptyp(
_tmp27);Cyc_Absyndump_dump(_tag_arr(" ",sizeof(unsigned char),2));goto _LLFA;
_LLFA:;}Cyc_Absyndump_dumpqvar(_tmp5E);Cyc_Absyndump_dumptps(_tmp28);goto _LLEF;
_LLEF:;}goto _LL6C;_LL7E:{void*_tmp60=_tmp2C;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp61;int _tmp62;struct _tuple0*_tmp63;struct _tuple0*_tmp64;struct Cyc_Absyn_Tunionfield*
_tmp65;struct Cyc_Absyn_Tunionfield _tmp66;struct _tuple0*_tmp67;struct Cyc_Absyn_Tuniondecl*
_tmp68;struct Cyc_Absyn_Tuniondecl _tmp69;int _tmp6A;struct _tuple0*_tmp6B;_LL100:
if(*((int*)_tmp60)== 0){_LL104: _tmp61=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp60)->f1;_LL107: _tmp64=_tmp61.tunion_name;goto _LL106;_LL106: _tmp63=_tmp61.field_name;
goto _LL105;_LL105: _tmp62=_tmp61.is_xtunion;goto _LL101;}else{goto _LL102;}_LL102:
if(*((int*)_tmp60)== 1){_LL10A: _tmp68=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp60)->f1;_tmp69=*_tmp68;_LL10C: _tmp6B=_tmp69.name;goto _LL10B;_LL10B: _tmp6A=
_tmp69.is_xtunion;goto _LL108;_LL108: _tmp65=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp60)->f2;_tmp66=*_tmp65;_LL109: _tmp67=_tmp66.name;goto _LL103;}else{goto _LLFF;}
_LL101: _tmp6B=_tmp64;_tmp6A=_tmp62;_tmp67=_tmp63;goto _LL103;_LL103: if(_tmp6A){
Cyc_Absyndump_dump(_tag_arr("xtunion ",sizeof(unsigned char),9));}else{Cyc_Absyndump_dump(
_tag_arr("tunion ",sizeof(unsigned char),8));}Cyc_Absyndump_dumpqvar(_tmp6B);Cyc_Absyndump_dump(
_tag_arr(".",sizeof(unsigned char),2));Cyc_Absyndump_dumpqvar(_tmp67);Cyc_Absyndump_dumptps(
_tmp2B);goto _LLFF;_LLFF:;}goto _LL6C;_LL80: Cyc_Absyndump_dump(_tag_arr("enum ",
sizeof(unsigned char),6));Cyc_Absyndump_dumpqvar(_tmp2D);return;_LL82: Cyc_Absyndump_dump(
_tag_arr("int",sizeof(unsigned char),4));return;_LL84: Cyc_Absyndump_dump(
_tag_arr("signed char",sizeof(unsigned char),12));return;_LL86: Cyc_Absyndump_dump(
_tag_arr("unsigned char",sizeof(unsigned char),14));return;_LL88: Cyc_Absyndump_dump(
_tag_arr("short",sizeof(unsigned char),6));return;_LL8A: Cyc_Absyndump_dump(
_tag_arr("unsigned short",sizeof(unsigned char),15));return;_LL8C: Cyc_Absyndump_dump(
_tag_arr("unsigned int",sizeof(unsigned char),13));return;_LL8E: Cyc_Absyndump_dump(
_tag_arr("__int64",sizeof(unsigned char),8));return;_LL90: Cyc_Absyndump_dump(
_tag_arr("long long",sizeof(unsigned char),10));return;_LL92: Cyc_Absyndump_dump(
_tag_arr("unsigned __int64",sizeof(unsigned char),17));return;_LL94: Cyc_Absyndump_dump(
_tag_arr("unsigned long long",sizeof(unsigned char),19));return;_LL96: Cyc_Absyndump_dump(
_tag_arr("float",sizeof(unsigned char),6));return;_LL98: if(_tmp42){Cyc_Absyndump_dump(
_tag_arr("long double",sizeof(unsigned char),12));}else{Cyc_Absyndump_dump(
_tag_arr("double",sizeof(unsigned char),7));}return;_LL9A: Cyc_Absyndump_dump_char((
int)'$');Cyc_Absyndump_dumpargs(_tmp43);return;_LL9C: Cyc_Absyndump_dump(_tag_arr("struct",
sizeof(unsigned char),7));Cyc_Absyndump_dumptps(_tmp44);return;_LL9E: Cyc_Absyndump_dump(
_tag_arr("struct",sizeof(unsigned char),7));Cyc_Absyndump_dumpqvar((struct
_tuple0*)_check_null(_tmp47));Cyc_Absyndump_dumptps(_tmp46);return;_LLA0: Cyc_Absyndump_dump(
_tag_arr("union",sizeof(unsigned char),6));Cyc_Absyndump_dumptps(_tmp48);return;
_LLA2: Cyc_Absyndump_dump(_tag_arr("union",sizeof(unsigned char),6));Cyc_Absyndump_dumpqvar((
struct _tuple0*)_check_null(_tmp4B));Cyc_Absyndump_dumptps(_tmp4A);return;_LLA4:
Cyc_Absyndump_dump(_tag_arr("struct {",sizeof(unsigned char),9));Cyc_Absyndump_dumpstructfields(
_tmp4C);Cyc_Absyndump_dump(_tag_arr("}",sizeof(unsigned char),2));return;_LLA6:
Cyc_Absyndump_dump(_tag_arr("union {",sizeof(unsigned char),8));Cyc_Absyndump_dumpstructfields(
_tmp4D);Cyc_Absyndump_dump(_tag_arr("}",sizeof(unsigned char),2));return;_LLA8:
Cyc_Absyndump_dump(_tag_arr("enum {",sizeof(unsigned char),7));Cyc_Absyndump_dumpenumfields(
_tmp4E);Cyc_Absyndump_dump(_tag_arr("}",sizeof(unsigned char),2));return;_LLAA:(
Cyc_Absyndump_dumpqvar(_tmp51),Cyc_Absyndump_dumptps(_tmp50));return;_LLAC: Cyc_Absyndump_dump(
_tag_arr("sizeof_t<",sizeof(unsigned char),10));Cyc_Absyndump_dumpntyp(_tmp52);
Cyc_Absyndump_dump(_tag_arr(">",sizeof(unsigned char),2));return;_LLAE: Cyc_Absyndump_dumprgn(
_tmp53);return;_LLB0: return;_LLB2: return;_LLB4: return;_LLB6: return;_LL6C:;}void
Cyc_Absyndump_dumpvaropt(struct Cyc_Core_Opt*vo){if(vo != 0){Cyc_Absyndump_dump_str((
struct _tagged_arr*)vo->v);}}void Cyc_Absyndump_dumpfunarg(struct _tuple1*t){((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),struct Cyc_Core_Opt*))
Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}
struct _tuple7{void*f1;void*f2;};void Cyc_Absyndump_dump_rgncmp(struct _tuple7*cmp){
struct _tuple7 _tmp6D;void*_tmp6E;void*_tmp6F;struct _tuple7*_tmp6C=cmp;_tmp6D=*
_tmp6C;_LL10F: _tmp6F=_tmp6D.f1;goto _LL10E;_LL10E: _tmp6E=_tmp6D.f2;goto _LL10D;
_LL10D: Cyc_Absyndump_dumptyp(_tmp6F);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptyp(
_tmp6E);}void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){((void(*)(void(*
f)(struct _tuple7*),struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dump_rgncmp,rgn_po,_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((
int)'(');for(0;args != 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple1*)
args->hd);if((args->tl != 0? 1: c_varargs)? 1: cyc_varargs != 0){Cyc_Absyndump_dump_char((
int)',');}}if(c_varargs){Cyc_Absyndump_dump(_tag_arr("...",sizeof(unsigned char),
4));}else{if(cyc_varargs != 0){struct _tuple1*_tmp70=({struct _tuple1*_tmp71=
_cycalloc(sizeof(struct _tuple1));_tmp71->f1=cyc_varargs->name;_tmp71->f2=
cyc_varargs->tq;_tmp71->f3=(void*)cyc_varargs->type;_tmp71;});Cyc_Absyndump_dump(
_tag_arr("...",sizeof(unsigned char),4));if(cyc_varargs->inject){Cyc_Absyndump_dump(
_tag_arr(" inject ",sizeof(unsigned char),9));}Cyc_Absyndump_dumpfunarg(_tmp70);}}
if(effopt != 0){Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpeff((void*)effopt->v);}
if(rgn_po != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}
Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumptyp(void*t){((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(({struct
Cyc_Absyn_Tqual _tmp72;_tmp72.q_const=0;_tmp72.q_volatile=0;_tmp72.q_restrict=0;
_tmp72;}),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpdesignator(
void*d){void*_tmp73=d;struct Cyc_Absyn_Exp*_tmp74;struct _tagged_arr*_tmp75;_LL111:
if(*((int*)_tmp73)== 0){_LL115: _tmp74=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp73)->f1;goto _LL112;}else{goto _LL113;}_LL113: if(*((int*)_tmp73)== 1){_LL116:
_tmp75=((struct Cyc_Absyn_FieldName_struct*)_tmp73)->f1;goto _LL114;}else{goto
_LL110;}_LL112: Cyc_Absyndump_dump(_tag_arr(".[",sizeof(unsigned char),3));Cyc_Absyndump_dumpexp(
_tmp74);Cyc_Absyndump_dump_char((int)']');goto _LL110;_LL114: Cyc_Absyndump_dump_char((
int)'.');Cyc_Absyndump_dump_nospace(*_tmp75);goto _LL110;_LL110:;}struct _tuple8{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};void Cyc_Absyndump_dumpde(struct
_tuple8*de){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*de).f1,_tag_arr("",
sizeof(unsigned char),1),_tag_arr("=",sizeof(unsigned char),2),_tag_arr("=",
sizeof(unsigned char),2));Cyc_Absyndump_dumpexp((*de).f2);}void Cyc_Absyndump_dumpexps_prec(
int inprec,struct Cyc_List_List*es){((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),
int env,struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,inprec,es,
_tag_arr("",sizeof(unsigned char),1),_tag_arr("",sizeof(unsigned char),1),
_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpexp_prec(int inprec,
struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= myprec){Cyc_Absyndump_dump_nospace(
_tag_arr("(",sizeof(unsigned char),2));}{void*_tmp76=(void*)e->r;void*_tmp77;
unsigned char _tmp78;void*_tmp79;void*_tmp7A;short _tmp7B;void*_tmp7C;void*_tmp7D;
int _tmp7E;void*_tmp7F;void*_tmp80;int _tmp81;void*_tmp82;void*_tmp83;long long
_tmp84;void*_tmp85;void*_tmp86;struct _tagged_arr _tmp87;void*_tmp88;void*_tmp89;
struct _tagged_arr _tmp8A;struct _tuple0*_tmp8B;struct _tuple0*_tmp8C;struct Cyc_List_List*
_tmp8D;void*_tmp8E;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Core_Opt*_tmp90;struct
Cyc_Absyn_Exp*_tmp91;void*_tmp92;struct Cyc_Absyn_Exp*_tmp93;void*_tmp94;struct
Cyc_Absyn_Exp*_tmp95;void*_tmp96;struct Cyc_Absyn_Exp*_tmp97;void*_tmp98;struct
Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_Absyn_Exp*_tmp9B;
struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Exp*_tmp9E;
struct Cyc_List_List*_tmp9F;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_List_List*_tmpA1;
struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_Absyn_Exp*_tmpA3;struct Cyc_Absyn_Exp*_tmpA4;
struct Cyc_Absyn_Exp*_tmpA5;struct Cyc_Absyn_Exp*_tmpA6;void*_tmpA7;struct Cyc_Absyn_Exp*
_tmpA8;struct Cyc_Absyn_Exp*_tmpA9;struct Cyc_Absyn_Exp*_tmpAA;void*_tmpAB;struct
Cyc_Absyn_Exp*_tmpAC;void*_tmpAD;struct _tagged_arr*_tmpAE;void*_tmpAF;void*
_tmpB0;unsigned int _tmpB1;void*_tmpB2;void*_tmpB3;struct Cyc_List_List*_tmpB4;
struct Cyc_Absyn_Exp*_tmpB5;struct _tagged_arr*_tmpB6;struct Cyc_Absyn_Exp*_tmpB7;
struct _tagged_arr*_tmpB8;struct Cyc_Absyn_Exp*_tmpB9;struct Cyc_Absyn_Exp*_tmpBA;
struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_List_List*_tmpBC;struct Cyc_List_List*_tmpBD;
struct _tuple1*_tmpBE;struct Cyc_List_List*_tmpBF;struct Cyc_Absyn_Exp*_tmpC0;
struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC2;struct Cyc_List_List*
_tmpC3;struct _tuple0*_tmpC4;struct Cyc_List_List*_tmpC5;struct Cyc_Absyn_Tunionfield*
_tmpC6;struct Cyc_List_List*_tmpC7;struct _tuple0*_tmpC8;struct _tuple0*_tmpC9;
struct Cyc_Absyn_MallocInfo _tmpCA;struct Cyc_Absyn_Exp*_tmpCB;void**_tmpCC;struct
Cyc_Absyn_Exp*_tmpCD;int _tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_Core_Opt*
_tmpD0;struct Cyc_Absyn_Stmt*_tmpD1;struct Cyc_Absyn_Fndecl*_tmpD2;struct Cyc_Absyn_Exp*
_tmpD3;_LL118: if(*((int*)_tmp76)== 0){_LL17A: _tmp77=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp76)->f1;if((unsigned int)_tmp77 > 1?*((int*)_tmp77)== 0: 0){_LL17C: _tmp79=(
void*)((struct Cyc_Absyn_Char_c_struct*)_tmp77)->f1;goto _LL17B;_LL17B: _tmp78=((
struct Cyc_Absyn_Char_c_struct*)_tmp77)->f2;goto _LL119;}else{goto _LL11A;}}else{
goto _LL11A;}_LL11A: if(*((int*)_tmp76)== 0){_LL17D: _tmp7A=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp76)->f1;if((unsigned int)_tmp7A > 1?*((int*)_tmp7A)== 1: 0){_LL17F: _tmp7C=(
void*)((struct Cyc_Absyn_Short_c_struct*)_tmp7A)->f1;goto _LL17E;_LL17E: _tmp7B=((
struct Cyc_Absyn_Short_c_struct*)_tmp7A)->f2;goto _LL11B;}else{goto _LL11C;}}else{
goto _LL11C;}_LL11C: if(*((int*)_tmp76)== 0){_LL180: _tmp7D=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp76)->f1;if((unsigned int)_tmp7D > 1?*((int*)_tmp7D)== 2: 0){_LL182: _tmp7F=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp7D)->f1;if((int)_tmp7F == 0){goto _LL181;}
else{goto _LL11E;}_LL181: _tmp7E=((struct Cyc_Absyn_Int_c_struct*)_tmp7D)->f2;goto
_LL11D;}else{goto _LL11E;}}else{goto _LL11E;}_LL11E: if(*((int*)_tmp76)== 0){_LL183:
_tmp80=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp76)->f1;if((unsigned int)
_tmp80 > 1?*((int*)_tmp80)== 2: 0){_LL185: _tmp82=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp80)->f1;if((int)_tmp82 == 1){goto _LL184;}else{goto _LL120;}_LL184: _tmp81=((
struct Cyc_Absyn_Int_c_struct*)_tmp80)->f2;goto _LL11F;}else{goto _LL120;}}else{
goto _LL120;}_LL120: if(*((int*)_tmp76)== 0){_LL186: _tmp83=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp76)->f1;if((unsigned int)_tmp83 > 1?*((int*)_tmp83)== 3: 0){_LL188: _tmp85=(
void*)((struct Cyc_Absyn_LongLong_c_struct*)_tmp83)->f1;goto _LL187;_LL187: _tmp84=((
struct Cyc_Absyn_LongLong_c_struct*)_tmp83)->f2;goto _LL121;}else{goto _LL122;}}
else{goto _LL122;}_LL122: if(*((int*)_tmp76)== 0){_LL189: _tmp86=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp76)->f1;if((unsigned int)_tmp86 > 1?*((int*)_tmp86)== 4: 0){_LL18A: _tmp87=((
struct Cyc_Absyn_Float_c_struct*)_tmp86)->f1;goto _LL123;}else{goto _LL124;}}else{
goto _LL124;}_LL124: if(*((int*)_tmp76)== 0){_LL18B: _tmp88=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp76)->f1;if((int)_tmp88 == 0){goto _LL125;}else{goto _LL126;}}else{goto _LL126;}
_LL126: if(*((int*)_tmp76)== 0){_LL18C: _tmp89=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp76)->f1;if((unsigned int)_tmp89 > 1?*((int*)_tmp89)== 5: 0){_LL18D: _tmp8A=((
struct Cyc_Absyn_String_c_struct*)_tmp89)->f1;goto _LL127;}else{goto _LL128;}}else{
goto _LL128;}_LL128: if(*((int*)_tmp76)== 2){_LL18E: _tmp8B=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp76)->f1;goto _LL129;}else{goto _LL12A;}_LL12A: if(*((int*)_tmp76)== 1){_LL18F:
_tmp8C=((struct Cyc_Absyn_Var_e_struct*)_tmp76)->f1;goto _LL12B;}else{goto _LL12C;}
_LL12C: if(*((int*)_tmp76)== 3){_LL191: _tmp8E=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp76)->f1;goto _LL190;_LL190: _tmp8D=((struct Cyc_Absyn_Primop_e_struct*)_tmp76)->f2;
goto _LL12D;}else{goto _LL12E;}_LL12E: if(*((int*)_tmp76)== 4){_LL194: _tmp91=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp76)->f1;goto _LL193;_LL193: _tmp90=((struct
Cyc_Absyn_AssignOp_e_struct*)_tmp76)->f2;goto _LL192;_LL192: _tmp8F=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp76)->f3;goto _LL12F;}else{goto _LL130;}_LL130: if(*((int*)_tmp76)== 5){_LL196:
_tmp93=((struct Cyc_Absyn_Increment_e_struct*)_tmp76)->f1;goto _LL195;_LL195:
_tmp92=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp76)->f2;if((int)_tmp92 == 
0){goto _LL131;}else{goto _LL132;}}else{goto _LL132;}_LL132: if(*((int*)_tmp76)== 5){
_LL198: _tmp95=((struct Cyc_Absyn_Increment_e_struct*)_tmp76)->f1;goto _LL197;
_LL197: _tmp94=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp76)->f2;if((int)
_tmp94 == 2){goto _LL133;}else{goto _LL134;}}else{goto _LL134;}_LL134: if(*((int*)
_tmp76)== 5){_LL19A: _tmp97=((struct Cyc_Absyn_Increment_e_struct*)_tmp76)->f1;
goto _LL199;_LL199: _tmp96=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp76)->f2;
if((int)_tmp96 == 1){goto _LL135;}else{goto _LL136;}}else{goto _LL136;}_LL136: if(*((
int*)_tmp76)== 5){_LL19C: _tmp99=((struct Cyc_Absyn_Increment_e_struct*)_tmp76)->f1;
goto _LL19B;_LL19B: _tmp98=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp76)->f2;
if((int)_tmp98 == 3){goto _LL137;}else{goto _LL138;}}else{goto _LL138;}_LL138: if(*((
int*)_tmp76)== 6){_LL19F: _tmp9C=((struct Cyc_Absyn_Conditional_e_struct*)_tmp76)->f1;
goto _LL19E;_LL19E: _tmp9B=((struct Cyc_Absyn_Conditional_e_struct*)_tmp76)->f2;
goto _LL19D;_LL19D: _tmp9A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp76)->f3;
goto _LL139;}else{goto _LL13A;}_LL13A: if(*((int*)_tmp76)== 7){_LL1A1: _tmp9E=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp76)->f1;goto _LL1A0;_LL1A0: _tmp9D=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp76)->f2;goto _LL13B;}else{goto _LL13C;}_LL13C: if(*((
int*)_tmp76)== 8){_LL1A3: _tmpA0=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp76)->f1;
goto _LL1A2;_LL1A2: _tmp9F=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp76)->f2;
goto _LL13D;}else{goto _LL13E;}_LL13E: if(*((int*)_tmp76)== 9){_LL1A5: _tmpA2=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp76)->f1;goto _LL1A4;_LL1A4: _tmpA1=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp76)->f2;goto _LL13F;}else{goto _LL140;}_LL140: if(*((
int*)_tmp76)== 10){_LL1A6: _tmpA3=((struct Cyc_Absyn_Throw_e_struct*)_tmp76)->f1;
goto _LL141;}else{goto _LL142;}_LL142: if(*((int*)_tmp76)== 11){_LL1A7: _tmpA4=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp76)->f1;goto _LL143;}else{goto _LL144;}
_LL144: if(*((int*)_tmp76)== 12){_LL1A8: _tmpA5=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp76)->f1;goto _LL145;}else{goto _LL146;}_LL146: if(*((int*)_tmp76)== 13){_LL1AA:
_tmpA7=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp76)->f1;goto _LL1A9;_LL1A9:
_tmpA6=((struct Cyc_Absyn_Cast_e_struct*)_tmp76)->f2;goto _LL147;}else{goto _LL148;}
_LL148: if(*((int*)_tmp76)== 14){_LL1AB: _tmpA8=((struct Cyc_Absyn_Address_e_struct*)
_tmp76)->f1;goto _LL149;}else{goto _LL14A;}_LL14A: if(*((int*)_tmp76)== 15){_LL1AD:
_tmpAA=((struct Cyc_Absyn_New_e_struct*)_tmp76)->f1;goto _LL1AC;_LL1AC: _tmpA9=((
struct Cyc_Absyn_New_e_struct*)_tmp76)->f2;goto _LL14B;}else{goto _LL14C;}_LL14C:
if(*((int*)_tmp76)== 16){_LL1AE: _tmpAB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp76)->f1;goto _LL14D;}else{goto _LL14E;}_LL14E: if(*((int*)_tmp76)== 17){_LL1AF:
_tmpAC=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp76)->f1;goto _LL14F;}else{goto
_LL150;}_LL150: if(*((int*)_tmp76)== 18){_LL1B2: _tmpAF=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp76)->f1;goto _LL1B0;_LL1B0: _tmpAD=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp76)->f2;if(*((int*)_tmpAD)== 0){_LL1B1: _tmpAE=((struct Cyc_Absyn_StructField_struct*)
_tmpAD)->f1;goto _LL151;}else{goto _LL152;}}else{goto _LL152;}_LL152: if(*((int*)
_tmp76)== 18){_LL1B5: _tmpB2=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp76)->f1;
goto _LL1B3;_LL1B3: _tmpB0=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp76)->f2;
if(*((int*)_tmpB0)== 1){_LL1B4: _tmpB1=((struct Cyc_Absyn_TupleIndex_struct*)
_tmpB0)->f1;goto _LL153;}else{goto _LL154;}}else{goto _LL154;}_LL154: if(*((int*)
_tmp76)== 19){_LL1B7: _tmpB4=((struct Cyc_Absyn_Gentyp_e_struct*)_tmp76)->f1;goto
_LL1B6;_LL1B6: _tmpB3=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp76)->f2;goto
_LL155;}else{goto _LL156;}_LL156: if(*((int*)_tmp76)== 20){_LL1B8: _tmpB5=((struct
Cyc_Absyn_Deref_e_struct*)_tmp76)->f1;goto _LL157;}else{goto _LL158;}_LL158: if(*((
int*)_tmp76)== 21){_LL1BA: _tmpB7=((struct Cyc_Absyn_StructMember_e_struct*)_tmp76)->f1;
goto _LL1B9;_LL1B9: _tmpB6=((struct Cyc_Absyn_StructMember_e_struct*)_tmp76)->f2;
goto _LL159;}else{goto _LL15A;}_LL15A: if(*((int*)_tmp76)== 22){_LL1BC: _tmpB9=((
struct Cyc_Absyn_StructArrow_e_struct*)_tmp76)->f1;goto _LL1BB;_LL1BB: _tmpB8=((
struct Cyc_Absyn_StructArrow_e_struct*)_tmp76)->f2;goto _LL15B;}else{goto _LL15C;}
_LL15C: if(*((int*)_tmp76)== 23){_LL1BE: _tmpBB=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp76)->f1;goto _LL1BD;_LL1BD: _tmpBA=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp76)->f2;goto _LL15D;}else{goto _LL15E;}_LL15E: if(*((int*)_tmp76)== 24){_LL1BF:
_tmpBC=((struct Cyc_Absyn_Tuple_e_struct*)_tmp76)->f1;goto _LL15F;}else{goto _LL160;}
_LL160: if(*((int*)_tmp76)== 25){_LL1C1: _tmpBE=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp76)->f1;goto _LL1C0;_LL1C0: _tmpBD=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp76)->f2;goto _LL161;}else{goto _LL162;}_LL162: if(*((int*)_tmp76)== 26){_LL1C2:
_tmpBF=((struct Cyc_Absyn_Array_e_struct*)_tmp76)->f1;goto _LL163;}else{goto _LL164;}
_LL164: if(*((int*)_tmp76)== 27){_LL1C5: _tmpC2=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp76)->f1;goto _LL1C4;_LL1C4: _tmpC1=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp76)->f2;goto _LL1C3;_LL1C3: _tmpC0=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp76)->f3;goto _LL165;}else{goto _LL166;}_LL166: if(*((int*)_tmp76)== 28){_LL1C7:
_tmpC4=((struct Cyc_Absyn_Struct_e_struct*)_tmp76)->f1;goto _LL1C6;_LL1C6: _tmpC3=((
struct Cyc_Absyn_Struct_e_struct*)_tmp76)->f3;goto _LL167;}else{goto _LL168;}_LL168:
if(*((int*)_tmp76)== 29){_LL1C8: _tmpC5=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp76)->f2;goto _LL169;}else{goto _LL16A;}_LL16A: if(*((int*)_tmp76)== 30){_LL1CA:
_tmpC7=((struct Cyc_Absyn_Tunion_e_struct*)_tmp76)->f3;goto _LL1C9;_LL1C9: _tmpC6=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp76)->f5;goto _LL16B;}else{goto _LL16C;}_LL16C:
if(*((int*)_tmp76)== 31){_LL1CB: _tmpC8=((struct Cyc_Absyn_Enum_e_struct*)_tmp76)->f1;
goto _LL16D;}else{goto _LL16E;}_LL16E: if(*((int*)_tmp76)== 32){_LL1CC: _tmpC9=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp76)->f1;goto _LL16F;}else{goto _LL170;}
_LL170: if(*((int*)_tmp76)== 33){_LL1CD: _tmpCA=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp76)->f1;_LL1D1: _tmpCE=_tmpCA.is_calloc;goto _LL1D0;_LL1D0: _tmpCD=_tmpCA.rgn;
goto _LL1CF;_LL1CF: _tmpCC=_tmpCA.elt_type;goto _LL1CE;_LL1CE: _tmpCB=_tmpCA.num_elts;
goto _LL171;}else{goto _LL172;}_LL172: if(*((int*)_tmp76)== 34){_LL1D3: _tmpD0=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp76)->f1;goto _LL1D2;_LL1D2: _tmpCF=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp76)->f2;goto _LL173;}else{goto _LL174;}
_LL174: if(*((int*)_tmp76)== 35){_LL1D4: _tmpD1=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp76)->f1;goto _LL175;}else{goto _LL176;}_LL176: if(*((int*)_tmp76)== 36){_LL1D5:
_tmpD2=((struct Cyc_Absyn_Codegen_e_struct*)_tmp76)->f1;goto _LL177;}else{goto
_LL178;}_LL178: if(*((int*)_tmp76)== 37){_LL1D6: _tmpD3=((struct Cyc_Absyn_Fill_e_struct*)
_tmp76)->f1;goto _LL179;}else{goto _LL117;}_LL119: Cyc_Absyndump_dump_char((int)'\'');
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp78));Cyc_Absyndump_dump_char((
int)'\'');goto _LL117;_LL11B: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmpD5;_tmpD5.tag=1;_tmpD5.f1=(int)((unsigned int)((int)_tmp7B));{void*_tmpD4[1]={&
_tmpD5};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmpD4,
sizeof(void*),1));}}));goto _LL117;_LL11D: Cyc_Absyndump_dump((struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmpD7;_tmpD7.tag=1;_tmpD7.f1=(int)((unsigned int)
_tmp7E);{void*_tmpD6[1]={& _tmpD7};Cyc_Std_aprintf(_tag_arr("%d",sizeof(
unsigned char),3),_tag_arr(_tmpD6,sizeof(void*),1));}}));goto _LL117;_LL11F: Cyc_Absyndump_dump((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpD9;_tmpD9.tag=1;_tmpD9.f1=(
unsigned int)_tmp81;{void*_tmpD8[1]={& _tmpD9};Cyc_Std_aprintf(_tag_arr("%u",
sizeof(unsigned char),3),_tag_arr(_tmpD8,sizeof(void*),1));}}));goto _LL117;
_LL121: Cyc_Absyndump_dump(_tag_arr("<<FIX LONG LONG CONSTANT>>",sizeof(
unsigned char),27));goto _LL117;_LL123: Cyc_Absyndump_dump(_tmp87);goto _LL117;
_LL125: Cyc_Absyndump_dump(_tag_arr("NULL",sizeof(unsigned char),5));goto _LL117;
_LL127: Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(
_tmp8A));Cyc_Absyndump_dump_char((int)'"');goto _LL117;_LL129: _tmp8C=_tmp8B;goto
_LL12B;_LL12B: Cyc_Absyndump_dumpqvar(_tmp8C);goto _LL117;_LL12D: {struct
_tagged_arr _tmpDA=Cyc_Absynpp_prim2str(_tmp8E);switch(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp8D)){case 1: _LL1D7: if(_tmp8E == (void*)19){Cyc_Absyndump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp8D))->hd);
Cyc_Absyndump_dump(_tag_arr(".size",sizeof(unsigned char),6));}else{Cyc_Absyndump_dump(
_tmpDA);Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp8D))->hd);}break;case 2: _LL1D8: Cyc_Absyndump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp8D))->hd);Cyc_Absyndump_dump(
_tmpDA);Cyc_Absyndump_dump_char((int)' ');Cyc_Absyndump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp8D->tl))->hd);break;
default: _LL1D9:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpDB=
_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmpDB[0]=({struct Cyc_Core_Failure_struct
_tmpDC;_tmpDC.tag=Cyc_Core_Failure;_tmpDC.f1=_tag_arr("Absyndump -- Bad number of arguments to primop",
sizeof(unsigned char),47);_tmpDC;});_tmpDB;}));}goto _LL117;}_LL12F: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp91);if(_tmp90 != 0){Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((void*)
_tmp90->v));}Cyc_Absyndump_dump_nospace(_tag_arr("=",sizeof(unsigned char),2));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp8F);goto _LL117;_LL131: Cyc_Absyndump_dump(
_tag_arr("++",sizeof(unsigned char),3));Cyc_Absyndump_dumpexp_prec(myprec,_tmp93);
goto _LL117;_LL133: Cyc_Absyndump_dump(_tag_arr("--",sizeof(unsigned char),3));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp95);goto _LL117;_LL135: Cyc_Absyndump_dumpexp_prec(myprec,_tmp97);Cyc_Absyndump_dump(
_tag_arr("++",sizeof(unsigned char),3));goto _LL117;_LL137: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp99);Cyc_Absyndump_dump(_tag_arr("--",sizeof(unsigned char),3));goto
_LL117;_LL139: Cyc_Absyndump_dumpexp_prec(myprec,_tmp9C);Cyc_Absyndump_dump_char((
int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp9B);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp9A);goto _LL117;_LL13B: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumpexp_prec(myprec,_tmp9E);Cyc_Absyndump_dump_char((int)',');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp9D);Cyc_Absyndump_dump_char((int)')');goto
_LL117;_LL13D: _tmpA2=_tmpA0;_tmpA1=_tmp9F;goto _LL13F;_LL13F: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpA2);Cyc_Absyndump_dump_nospace(_tag_arr("(",sizeof(unsigned char),2));
Cyc_Absyndump_dumpexps_prec(20,_tmpA1);Cyc_Absyndump_dump_nospace(_tag_arr(")",
sizeof(unsigned char),2));goto _LL117;_LL141: Cyc_Absyndump_dump(_tag_arr("throw",
sizeof(unsigned char),6));Cyc_Absyndump_dumpexp_prec(myprec,_tmpA3);goto _LL117;
_LL143: _tmpA5=_tmpA4;goto _LL145;_LL145: Cyc_Absyndump_dumpexp_prec(inprec,_tmpA5);
goto _LL117;_LL147: Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmpA7);
Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmpA6);goto
_LL117;_LL149: Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(
myprec,_tmpA8);goto _LL117;_LL14B: Cyc_Absyndump_dump(_tag_arr("new ",sizeof(
unsigned char),5));Cyc_Absyndump_dumpexp_prec(myprec,_tmpA9);goto _LL117;_LL14D:
Cyc_Absyndump_dump(_tag_arr("sizeof(",sizeof(unsigned char),8));Cyc_Absyndump_dumptyp(
_tmpAB);Cyc_Absyndump_dump_char((int)')');goto _LL117;_LL14F: Cyc_Absyndump_dump(
_tag_arr("sizeof(",sizeof(unsigned char),8));Cyc_Absyndump_dumpexp_prec(myprec,
_tmpAC);Cyc_Absyndump_dump_char((int)')');goto _LL117;_LL151: Cyc_Absyndump_dump(
_tag_arr("offsetof(",sizeof(unsigned char),10));Cyc_Absyndump_dumptyp(_tmpAF);
Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmpAE);Cyc_Absyndump_dump_char((
int)')');goto _LL117;_LL153: Cyc_Absyndump_dump(_tag_arr("offsetof(",sizeof(
unsigned char),10));Cyc_Absyndump_dumptyp(_tmpB2);Cyc_Absyndump_dump_char((int)',');
Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpDE;_tmpDE.tag=
1;_tmpDE.f1=(int)_tmpB1;{void*_tmpDD[1]={& _tmpDE};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(unsigned char),3),_tag_arr(_tmpDD,sizeof(void*),1));}}));Cyc_Absyndump_dump_char((
int)')');goto _LL117;_LL155: Cyc_Absyndump_dump(_tag_arr("__gen(",sizeof(
unsigned char),7));Cyc_Absyndump_dumptvars(_tmpB4);Cyc_Absyndump_dumptyp(_tmpB3);
Cyc_Absyndump_dump_char((int)')');goto _LL117;_LL157: Cyc_Absyndump_dump_char((int)'*');
Cyc_Absyndump_dumpexp_prec(myprec,_tmpB5);goto _LL117;_LL159: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpB7);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmpB6);goto _LL117;_LL15B: Cyc_Absyndump_dumpexp_prec(myprec,_tmpB9);Cyc_Absyndump_dump_nospace(
_tag_arr("->",sizeof(unsigned char),3));Cyc_Absyndump_dump_nospace(*_tmpB8);goto
_LL117;_LL15D: Cyc_Absyndump_dumpexp_prec(myprec,_tmpBB);Cyc_Absyndump_dump_char((
int)'[');Cyc_Absyndump_dumpexp(_tmpBA);Cyc_Absyndump_dump_char((int)']');goto
_LL117;_LL15F: Cyc_Absyndump_dump(_tag_arr("$(",sizeof(unsigned char),3));Cyc_Absyndump_dumpexps_prec(
20,_tmpBC);Cyc_Absyndump_dump_char((int)')');goto _LL117;_LL161: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumptyp((*_tmpBE).f3);Cyc_Absyndump_dump_char((int)')');((
void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmpBD,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LL117;_LL163:((void(*)(void(*f)(
struct _tuple8*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr
end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmpBF,
_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),
_tag_arr(",",sizeof(unsigned char),2));goto _LL117;_LL165: Cyc_Absyndump_dump(
_tag_arr("new {for",sizeof(unsigned char),9));Cyc_Absyndump_dump_str((*_tmpC2->name).f2);
Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmpC1);Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dumpexp(_tmpC0);Cyc_Absyndump_dump_char((int)'}');goto
_LL117;_LL167: Cyc_Absyndump_dumpqvar(_tmpC4);((void(*)(void(*f)(struct _tuple8*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmpC3,_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LL117;_LL169:((void(*)(void(*f)(struct _tuple8*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmpC5,_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LL117;_LL16B: Cyc_Absyndump_dumpqvar(_tmpC6->name);
if(_tmpC7 != 0){((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp,_tmpC7,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}goto _LL117;
_LL16D: Cyc_Absyndump_dumpqvar(_tmpC8);goto _LL117;_LL16F: Cyc_Absyndump_dumpqvar(
_tmpC9);goto _LL117;_LL171: if(_tmpCE){if(_tmpCD != 0){Cyc_Absyndump_dump(_tag_arr("rcalloc(",
sizeof(unsigned char),9));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmpCD));Cyc_Absyndump_dump(_tag_arr(",",sizeof(unsigned char),2));}
else{Cyc_Absyndump_dump(_tag_arr("calloc",sizeof(unsigned char),7));}Cyc_Absyndump_dumpexp(
_tmpCB);Cyc_Absyndump_dump(_tag_arr(",",sizeof(unsigned char),2));Cyc_Absyndump_dumpexp(
Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmpCC)),0));Cyc_Absyndump_dump(
_tag_arr(")",sizeof(unsigned char),2));}else{if(_tmpCD != 0){Cyc_Absyndump_dump(
_tag_arr("rmalloc(",sizeof(unsigned char),9));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmpCD));Cyc_Absyndump_dump(_tag_arr(",",sizeof(unsigned char),2));}
else{Cyc_Absyndump_dump(_tag_arr("malloc(",sizeof(unsigned char),8));}if(_tmpCC
!= 0){Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmpCC,
0),_tmpCB,0));}else{Cyc_Absyndump_dumpexp(_tmpCB);}Cyc_Absyndump_dump(_tag_arr(")",
sizeof(unsigned char),2));}goto _LL117;_LL173:((void(*)(void(*f)(struct _tuple8*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmpCF,_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LL117;_LL175: Cyc_Absyndump_dump_nospace(_tag_arr("({",
sizeof(unsigned char),3));Cyc_Absyndump_dumpstmt(_tmpD1);Cyc_Absyndump_dump_nospace(
_tag_arr("})",sizeof(unsigned char),3));goto _LL117;_LL177: Cyc_Absyndump_dump(
_tag_arr("codegen(",sizeof(unsigned char),9));Cyc_Absyndump_dumpdecl(({struct Cyc_Absyn_Decl*
_tmpDF=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_tmpDF->r=(void*)((void*)({struct
Cyc_Absyn_Fn_d_struct*_tmpE0=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_struct));
_tmpE0[0]=({struct Cyc_Absyn_Fn_d_struct _tmpE1;_tmpE1.tag=1;_tmpE1.f1=_tmpD2;
_tmpE1;});_tmpE0;}));_tmpDF->loc=e->loc;_tmpDF;}));Cyc_Absyndump_dump(_tag_arr(")",
sizeof(unsigned char),2));goto _LL117;_LL179: Cyc_Absyndump_dump(_tag_arr("fill(",
sizeof(unsigned char),6));Cyc_Absyndump_dumpexp(_tmpD3);Cyc_Absyndump_dump(
_tag_arr(")",sizeof(unsigned char),2));goto _LL117;_LL117:;}if(inprec >= myprec){
Cyc_Absyndump_dump_char((int)')');}}void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*
e){Cyc_Absyndump_dumpexp_prec(0,e);}void Cyc_Absyndump_dumpswitchclauses(struct
Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmpE2=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmpE2->where_clause == 0?(void*)(
_tmpE2->pattern)->r == (void*)0: 0){Cyc_Absyndump_dump(_tag_arr("default:",sizeof(
unsigned char),9));}else{Cyc_Absyndump_dump(_tag_arr("case",sizeof(unsigned char),
5));Cyc_Absyndump_dumppat(_tmpE2->pattern);if(_tmpE2->where_clause != 0){Cyc_Absyndump_dump(
_tag_arr("&&",sizeof(unsigned char),3));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmpE2->where_clause));}Cyc_Absyndump_dump_nospace(_tag_arr(":",
sizeof(unsigned char),2));}Cyc_Absyndump_dumpstmt(_tmpE2->body);}}void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt*s){void*_tmpE3=(void*)s->r;struct Cyc_Absyn_Exp*_tmpE4;
struct Cyc_Absyn_Stmt*_tmpE5;struct Cyc_Absyn_Stmt*_tmpE6;struct Cyc_Absyn_Exp*
_tmpE7;struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_Absyn_Stmt*_tmpE9;struct Cyc_Absyn_Stmt*
_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Stmt*_tmpEC;struct _tuple2
_tmpED;struct Cyc_Absyn_Exp*_tmpEE;struct _tagged_arr*_tmpEF;struct Cyc_Absyn_Stmt*
_tmpF0;struct _tuple2 _tmpF1;struct Cyc_Absyn_Exp*_tmpF2;struct _tuple2 _tmpF3;struct
Cyc_Absyn_Exp*_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_ForArrayInfo
_tmpF6;struct Cyc_Absyn_Stmt*_tmpF7;struct _tuple2 _tmpF8;struct Cyc_Absyn_Exp*
_tmpF9;struct _tuple2 _tmpFA;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_List_List*_tmpFC;
struct Cyc_List_List*_tmpFD;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Stmt*
_tmpFF;struct Cyc_Absyn_Decl*_tmp100;struct Cyc_Absyn_Stmt*_tmp101;struct
_tagged_arr*_tmp102;struct _tuple2 _tmp103;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Stmt*
_tmp105;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_List_List*
_tmp108;struct Cyc_List_List*_tmp109;struct Cyc_List_List*_tmp10A;struct Cyc_Absyn_Stmt*
_tmp10B;struct Cyc_Absyn_Stmt*_tmp10C;struct Cyc_Absyn_Vardecl*_tmp10D;struct Cyc_Absyn_Tvar*
_tmp10E;struct Cyc_Absyn_Stmt*_tmp10F;struct Cyc_Absyn_Stmt*_tmp110;_LL1DC: if((int)
_tmpE3 == 0){goto _LL1DD;}else{goto _LL1DE;}_LL1DE: if((unsigned int)_tmpE3 > 1?*((
int*)_tmpE3)== 0: 0){_LL20A: _tmpE4=((struct Cyc_Absyn_Exp_s_struct*)_tmpE3)->f1;
goto _LL1DF;}else{goto _LL1E0;}_LL1E0: if((unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 
1: 0){_LL20C: _tmpE6=((struct Cyc_Absyn_Seq_s_struct*)_tmpE3)->f1;goto _LL20B;_LL20B:
_tmpE5=((struct Cyc_Absyn_Seq_s_struct*)_tmpE3)->f2;goto _LL1E1;}else{goto _LL1E2;}
_LL1E2: if((unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 2: 0){_LL20D: _tmpE7=((struct
Cyc_Absyn_Return_s_struct*)_tmpE3)->f1;if(_tmpE7 == 0){goto _LL1E3;}else{goto
_LL1E4;}}else{goto _LL1E4;}_LL1E4: if((unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 2:
0){_LL20E: _tmpE8=((struct Cyc_Absyn_Return_s_struct*)_tmpE3)->f1;goto _LL1E5;}
else{goto _LL1E6;}_LL1E6: if((unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 3: 0){_LL211:
_tmpEB=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpE3)->f1;goto _LL210;_LL210:
_tmpEA=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpE3)->f2;goto _LL20F;_LL20F:
_tmpE9=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpE3)->f3;goto _LL1E7;}else{goto
_LL1E8;}_LL1E8: if((unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 4: 0){_LL213: _tmpED=((
struct Cyc_Absyn_While_s_struct*)_tmpE3)->f1;_LL214: _tmpEE=_tmpED.f1;goto _LL212;
_LL212: _tmpEC=((struct Cyc_Absyn_While_s_struct*)_tmpE3)->f2;goto _LL1E9;}else{
goto _LL1EA;}_LL1EA: if((unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 5: 0){goto _LL1EB;}
else{goto _LL1EC;}_LL1EC: if((unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 6: 0){goto
_LL1ED;}else{goto _LL1EE;}_LL1EE: if((unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 7: 0){
_LL215: _tmpEF=((struct Cyc_Absyn_Goto_s_struct*)_tmpE3)->f1;goto _LL1EF;}else{goto
_LL1F0;}_LL1F0: if((unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 8: 0){_LL21B: _tmpF5=((
struct Cyc_Absyn_For_s_struct*)_tmpE3)->f1;goto _LL219;_LL219: _tmpF3=((struct Cyc_Absyn_For_s_struct*)
_tmpE3)->f2;_LL21A: _tmpF4=_tmpF3.f1;goto _LL217;_LL217: _tmpF1=((struct Cyc_Absyn_For_s_struct*)
_tmpE3)->f3;_LL218: _tmpF2=_tmpF1.f1;goto _LL216;_LL216: _tmpF0=((struct Cyc_Absyn_For_s_struct*)
_tmpE3)->f4;goto _LL1F1;}else{goto _LL1F2;}_LL1F2: if((unsigned int)_tmpE3 > 1?*((
int*)_tmpE3)== 19: 0){_LL21C: _tmpF6=((struct Cyc_Absyn_ForArray_s_struct*)_tmpE3)->f1;
_LL222: _tmpFC=_tmpF6.defns;goto _LL220;_LL220: _tmpFA=_tmpF6.condition;_LL221:
_tmpFB=_tmpFA.f1;goto _LL21E;_LL21E: _tmpF8=_tmpF6.delta;_LL21F: _tmpF9=_tmpF8.f1;
goto _LL21D;_LL21D: _tmpF7=_tmpF6.body;goto _LL1F3;}else{goto _LL1F4;}_LL1F4: if((
unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 9: 0){_LL224: _tmpFE=((struct Cyc_Absyn_Switch_s_struct*)
_tmpE3)->f1;goto _LL223;_LL223: _tmpFD=((struct Cyc_Absyn_Switch_s_struct*)_tmpE3)->f2;
goto _LL1F5;}else{goto _LL1F6;}_LL1F6: if((unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 
12: 0){_LL226: _tmp100=((struct Cyc_Absyn_Decl_s_struct*)_tmpE3)->f1;goto _LL225;
_LL225: _tmpFF=((struct Cyc_Absyn_Decl_s_struct*)_tmpE3)->f2;goto _LL1F7;}else{goto
_LL1F8;}_LL1F8: if((unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 15: 0){_LL228: _tmp102=((
struct Cyc_Absyn_Label_s_struct*)_tmpE3)->f1;goto _LL227;_LL227: _tmp101=((struct
Cyc_Absyn_Label_s_struct*)_tmpE3)->f2;goto _LL1F9;}else{goto _LL1FA;}_LL1FA: if((
unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 16: 0){_LL22B: _tmp105=((struct Cyc_Absyn_Do_s_struct*)
_tmpE3)->f1;goto _LL229;_LL229: _tmp103=((struct Cyc_Absyn_Do_s_struct*)_tmpE3)->f2;
_LL22A: _tmp104=_tmp103.f1;goto _LL1FB;}else{goto _LL1FC;}_LL1FC: if((unsigned int)
_tmpE3 > 1?*((int*)_tmpE3)== 10: 0){_LL22D: _tmp107=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmpE3)->f1;goto _LL22C;_LL22C: _tmp106=((struct Cyc_Absyn_SwitchC_s_struct*)_tmpE3)->f2;
goto _LL1FD;}else{goto _LL1FE;}_LL1FE: if((unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 
11: 0){_LL22E: _tmp108=((struct Cyc_Absyn_Fallthru_s_struct*)_tmpE3)->f1;if(_tmp108
== 0){goto _LL1FF;}else{goto _LL200;}}else{goto _LL200;}_LL200: if((unsigned int)
_tmpE3 > 1?*((int*)_tmpE3)== 11: 0){_LL22F: _tmp109=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmpE3)->f1;goto _LL201;}else{goto _LL202;}_LL202: if((unsigned int)_tmpE3 > 1?*((
int*)_tmpE3)== 17: 0){_LL231: _tmp10B=((struct Cyc_Absyn_TryCatch_s_struct*)_tmpE3)->f1;
goto _LL230;_LL230: _tmp10A=((struct Cyc_Absyn_TryCatch_s_struct*)_tmpE3)->f2;goto
_LL203;}else{goto _LL204;}_LL204: if((unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 18:
0){_LL234: _tmp10E=((struct Cyc_Absyn_Region_s_struct*)_tmpE3)->f1;goto _LL233;
_LL233: _tmp10D=((struct Cyc_Absyn_Region_s_struct*)_tmpE3)->f2;goto _LL232;_LL232:
_tmp10C=((struct Cyc_Absyn_Region_s_struct*)_tmpE3)->f3;goto _LL205;}else{goto
_LL206;}_LL206: if((unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 13: 0){_LL235: _tmp10F=((
struct Cyc_Absyn_Cut_s_struct*)_tmpE3)->f1;goto _LL207;}else{goto _LL208;}_LL208:
if((unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== 14: 0){_LL236: _tmp110=((struct Cyc_Absyn_Splice_s_struct*)
_tmpE3)->f1;goto _LL209;}else{goto _LL1DB;}_LL1DD: Cyc_Absyndump_dump_semi();goto
_LL1DB;_LL1DF: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dumpexp(_tmpE4);Cyc_Absyndump_dump_semi();
goto _LL1DB;_LL1E1: if(Cyc_Absynpp_is_declaration(_tmpE6)){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstmt(_tmpE6);Cyc_Absyndump_dump_char((int)'}');}else{
Cyc_Absyndump_dumpstmt(_tmpE6);}if(Cyc_Absynpp_is_declaration(_tmpE5)){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstmt(_tmpE5);Cyc_Absyndump_dump_char((int)'}');}else{
Cyc_Absyndump_dumpstmt(_tmpE5);}goto _LL1DB;_LL1E3: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(_tag_arr("return;",sizeof(unsigned char),8));goto _LL1DB;
_LL1E5: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("return",
sizeof(unsigned char),7));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmpE8));Cyc_Absyndump_dump_semi();goto _LL1DB;_LL1E7: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("if(",sizeof(unsigned char),4));Cyc_Absyndump_dumpexp(
_tmpEB);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpstmt(
_tmpEA);Cyc_Absyndump_dump_char((int)'}');{void*_tmp111=(void*)_tmpE9->r;_LL238:
if((int)_tmp111 == 0){goto _LL239;}else{goto _LL23A;}_LL23A: goto _LL23B;_LL239: goto
_LL237;_LL23B: Cyc_Absyndump_dump(_tag_arr("else{",sizeof(unsigned char),6));Cyc_Absyndump_dumpstmt(
_tmpE9);Cyc_Absyndump_dump_char((int)'}');goto _LL237;_LL237:;}goto _LL1DB;_LL1E9:
Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("while(",sizeof(
unsigned char),7));Cyc_Absyndump_dumpexp(_tmpEE);Cyc_Absyndump_dump_nospace(
_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpstmt(_tmpEC);Cyc_Absyndump_dump_char((
int)'}');goto _LL1DB;_LL1EB: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("break;",sizeof(unsigned char),7));goto _LL1DB;_LL1ED: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("continue;",sizeof(unsigned char),10));goto
_LL1DB;_LL1EF: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("goto",
sizeof(unsigned char),5));Cyc_Absyndump_dump_str(_tmpEF);Cyc_Absyndump_dump_semi();
goto _LL1DB;_LL1F1: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("for(",
sizeof(unsigned char),5));Cyc_Absyndump_dumpexp(_tmpF5);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmpF4);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(
_tmpF2);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpstmt(
_tmpF0);Cyc_Absyndump_dump_char((int)'}');goto _LL1DB;_LL1F3: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("forarray(",sizeof(unsigned char),10));for(0;
_tmpFC != 0;_tmpFC=_tmpFC->tl){Cyc_Absyndump_dumpvardecl((struct Cyc_Absyn_Vardecl*)
_tmpFC->hd,s->loc);}Cyc_Absyndump_dumpexp(_tmpFB);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(
_tmpF9);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpstmt(
_tmpF7);Cyc_Absyndump_dump_char((int)'}');goto _LL1DB;_LL1F5: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("switch(",sizeof(unsigned char),8));Cyc_Absyndump_dumpexp(
_tmpFE);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpswitchclauses(
_tmpFD);Cyc_Absyndump_dump_char((int)'}');goto _LL1DB;_LL1F7: Cyc_Absyndump_dumpdecl(
_tmp100);Cyc_Absyndump_dumpstmt(_tmpFF);goto _LL1DB;_LL1F9: if(Cyc_Absynpp_is_declaration(
_tmp101)){Cyc_Absyndump_dump_str(_tmp102);Cyc_Absyndump_dump_nospace(_tag_arr(": {",
sizeof(unsigned char),4));Cyc_Absyndump_dumpstmt(_tmp101);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dump_str(_tmp102);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpstmt(_tmp101);}goto _LL1DB;_LL1FB: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(_tag_arr("do {",sizeof(unsigned char),5));Cyc_Absyndump_dumpstmt(
_tmp105);Cyc_Absyndump_dump_nospace(_tag_arr("} while (",sizeof(unsigned char),
10));Cyc_Absyndump_dumpexp(_tmp104);Cyc_Absyndump_dump_nospace(_tag_arr(");",
sizeof(unsigned char),3));goto _LL1DB;_LL1FD: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("switch \"C\" (",sizeof(unsigned char),13));Cyc_Absyndump_dumpexp(
_tmp107);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(unsigned char),3));for(
0;_tmp106 != 0;_tmp106=_tmp106->tl){struct Cyc_Absyn_SwitchC_clause _tmp113;struct
Cyc_Absyn_Stmt*_tmp114;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_SwitchC_clause*
_tmp112=(struct Cyc_Absyn_SwitchC_clause*)_tmp106->hd;_tmp113=*_tmp112;_LL23E:
_tmp115=_tmp113.cnst_exp;goto _LL23D;_LL23D: _tmp114=_tmp113.body;goto _LL23C;
_LL23C: if(_tmp115 == 0){Cyc_Absyndump_dump(_tag_arr("default: ",sizeof(
unsigned char),10));}else{Cyc_Absyndump_dump(_tag_arr("case ",sizeof(
unsigned char),6));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(
_tmp115));Cyc_Absyndump_dump_char((int)':');}Cyc_Absyndump_dumpstmt(_tmp114);}
Cyc_Absyndump_dump_char((int)'}');goto _LL1DB;_LL1FF: Cyc_Absyndump_dump(_tag_arr("fallthru;",
sizeof(unsigned char),10));goto _LL1DB;_LL201: Cyc_Absyndump_dump(_tag_arr("fallthru(",
sizeof(unsigned char),10));Cyc_Absyndump_dumpexps_prec(20,_tmp109);Cyc_Absyndump_dump_nospace(
_tag_arr(");",sizeof(unsigned char),3));goto _LL1DB;_LL203: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("try",sizeof(unsigned char),4));Cyc_Absyndump_dumpstmt(
_tmp10B);Cyc_Absyndump_dump(_tag_arr("catch {",sizeof(unsigned char),8));Cyc_Absyndump_dumpswitchclauses(
_tmp10A);Cyc_Absyndump_dump_char((int)'}');goto _LL1DB;_LL205: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("region<",sizeof(unsigned char),8));Cyc_Absyndump_dumptvar(
_tmp10E);Cyc_Absyndump_dump(_tag_arr("> ",sizeof(unsigned char),3));Cyc_Absyndump_dumpqvar(
_tmp10D->name);Cyc_Absyndump_dump(_tag_arr("{",sizeof(unsigned char),2));Cyc_Absyndump_dumpstmt(
_tmp10C);Cyc_Absyndump_dump(_tag_arr("}",sizeof(unsigned char),2));goto _LL1DB;
_LL207: Cyc_Absyndump_dump(_tag_arr("cut",sizeof(unsigned char),4));Cyc_Absyndump_dumpstmt(
_tmp10F);goto _LL1DB;_LL209: Cyc_Absyndump_dump(_tag_arr("splice",sizeof(
unsigned char),7));Cyc_Absyndump_dumpstmt(_tmp110);goto _LL1DB;_LL1DB:;}struct
_tuple9{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(
struct _tuple9*dp){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*dp).f1,
_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",sizeof(unsigned char),2),
_tag_arr("=",sizeof(unsigned char),2));Cyc_Absyndump_dumppat((*dp).f2);}void Cyc_Absyndump_dumppat(
struct Cyc_Absyn_Pat*p){void*_tmp116=(void*)p->r;int _tmp117;void*_tmp118;int
_tmp119;void*_tmp11A;unsigned char _tmp11B;struct _tagged_arr _tmp11C;struct Cyc_Absyn_Vardecl*
_tmp11D;struct Cyc_List_List*_tmp11E;struct Cyc_Absyn_Pat*_tmp11F;struct Cyc_Absyn_Vardecl*
_tmp120;struct _tuple0*_tmp121;struct Cyc_List_List*_tmp122;struct Cyc_List_List*
_tmp123;struct _tuple0*_tmp124;struct Cyc_List_List*_tmp125;struct Cyc_List_List*
_tmp126;struct _tuple0*_tmp127;struct Cyc_List_List*_tmp128;struct Cyc_List_List*
_tmp129;struct Cyc_Absyn_Structdecl*_tmp12A;struct Cyc_List_List*_tmp12B;struct Cyc_List_List*
_tmp12C;struct Cyc_Absyn_Tunionfield*_tmp12D;struct Cyc_Absyn_Enumfield*_tmp12E;
struct Cyc_Absyn_Enumfield*_tmp12F;_LL240: if((int)_tmp116 == 0){goto _LL241;}else{
goto _LL242;}_LL242: if((int)_tmp116 == 1){goto _LL243;}else{goto _LL244;}_LL244: if((
unsigned int)_tmp116 > 2?*((int*)_tmp116)== 1: 0){_LL263: _tmp118=(void*)((struct
Cyc_Absyn_Int_p_struct*)_tmp116)->f1;if((int)_tmp118 == 0){goto _LL262;}else{goto
_LL246;}_LL262: _tmp117=((struct Cyc_Absyn_Int_p_struct*)_tmp116)->f2;goto _LL245;}
else{goto _LL246;}_LL246: if((unsigned int)_tmp116 > 2?*((int*)_tmp116)== 1: 0){
_LL265: _tmp11A=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp116)->f1;if((int)
_tmp11A == 1){goto _LL264;}else{goto _LL248;}_LL264: _tmp119=((struct Cyc_Absyn_Int_p_struct*)
_tmp116)->f2;goto _LL247;}else{goto _LL248;}_LL248: if((unsigned int)_tmp116 > 2?*((
int*)_tmp116)== 2: 0){_LL266: _tmp11B=((struct Cyc_Absyn_Char_p_struct*)_tmp116)->f1;
goto _LL249;}else{goto _LL24A;}_LL24A: if((unsigned int)_tmp116 > 2?*((int*)_tmp116)
== 3: 0){_LL267: _tmp11C=((struct Cyc_Absyn_Float_p_struct*)_tmp116)->f1;goto _LL24B;}
else{goto _LL24C;}_LL24C: if((unsigned int)_tmp116 > 2?*((int*)_tmp116)== 0: 0){
_LL268: _tmp11D=((struct Cyc_Absyn_Var_p_struct*)_tmp116)->f1;goto _LL24D;}else{
goto _LL24E;}_LL24E: if((unsigned int)_tmp116 > 2?*((int*)_tmp116)== 4: 0){_LL269:
_tmp11E=((struct Cyc_Absyn_Tuple_p_struct*)_tmp116)->f1;goto _LL24F;}else{goto
_LL250;}_LL250: if((unsigned int)_tmp116 > 2?*((int*)_tmp116)== 5: 0){_LL26A:
_tmp11F=((struct Cyc_Absyn_Pointer_p_struct*)_tmp116)->f1;goto _LL251;}else{goto
_LL252;}_LL252: if((unsigned int)_tmp116 > 2?*((int*)_tmp116)== 6: 0){_LL26B:
_tmp120=((struct Cyc_Absyn_Reference_p_struct*)_tmp116)->f1;goto _LL253;}else{goto
_LL254;}_LL254: if((unsigned int)_tmp116 > 2?*((int*)_tmp116)== 11: 0){_LL26C:
_tmp121=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp116)->f1;goto _LL255;}else{goto
_LL256;}_LL256: if((unsigned int)_tmp116 > 2?*((int*)_tmp116)== 12: 0){_LL26F:
_tmp124=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp116)->f1;goto _LL26E;_LL26E:
_tmp123=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp116)->f2;goto _LL26D;_LL26D:
_tmp122=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp116)->f3;goto _LL257;}else{
goto _LL258;}_LL258: if((unsigned int)_tmp116 > 2?*((int*)_tmp116)== 13: 0){_LL272:
_tmp127=((struct Cyc_Absyn_UnknownFields_p_struct*)_tmp116)->f1;goto _LL271;_LL271:
_tmp126=((struct Cyc_Absyn_UnknownFields_p_struct*)_tmp116)->f2;goto _LL270;_LL270:
_tmp125=((struct Cyc_Absyn_UnknownFields_p_struct*)_tmp116)->f3;goto _LL259;}else{
goto _LL25A;}_LL25A: if((unsigned int)_tmp116 > 2?*((int*)_tmp116)== 7: 0){_LL275:
_tmp12A=((struct Cyc_Absyn_Struct_p_struct*)_tmp116)->f1;goto _LL274;_LL274:
_tmp129=((struct Cyc_Absyn_Struct_p_struct*)_tmp116)->f3;goto _LL273;_LL273:
_tmp128=((struct Cyc_Absyn_Struct_p_struct*)_tmp116)->f4;goto _LL25B;}else{goto
_LL25C;}_LL25C: if((unsigned int)_tmp116 > 2?*((int*)_tmp116)== 8: 0){_LL278:
_tmp12D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp116)->f2;goto _LL277;_LL277:
_tmp12C=((struct Cyc_Absyn_Tunion_p_struct*)_tmp116)->f3;goto _LL276;_LL276:
_tmp12B=((struct Cyc_Absyn_Tunion_p_struct*)_tmp116)->f4;goto _LL25D;}else{goto
_LL25E;}_LL25E: if((unsigned int)_tmp116 > 2?*((int*)_tmp116)== 9: 0){_LL279:
_tmp12E=((struct Cyc_Absyn_Enum_p_struct*)_tmp116)->f2;goto _LL25F;}else{goto
_LL260;}_LL260: if((unsigned int)_tmp116 > 2?*((int*)_tmp116)== 10: 0){_LL27A:
_tmp12F=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp116)->f2;goto _LL261;}else{goto
_LL23F;}_LL241: Cyc_Absyndump_dump_char((int)'_');goto _LL23F;_LL243: Cyc_Absyndump_dump(
_tag_arr("NULL",sizeof(unsigned char),5));goto _LL23F;_LL245: Cyc_Absyndump_dump((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp131;_tmp131.tag=1;_tmp131.f1=(
int)((unsigned int)_tmp117);{void*_tmp130[1]={& _tmp131};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(unsigned char),3),_tag_arr(_tmp130,sizeof(void*),1));}}));goto _LL23F;
_LL247: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp133;_tmp133.tag=1;_tmp133.f1=(unsigned int)_tmp119;{void*_tmp132[1]={&
_tmp133};Cyc_Std_aprintf(_tag_arr("%u",sizeof(unsigned char),3),_tag_arr(_tmp132,
sizeof(void*),1));}}));goto _LL23F;_LL249: Cyc_Absyndump_dump(_tag_arr("'",sizeof(
unsigned char),2));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp11B));
Cyc_Absyndump_dump_nospace(_tag_arr("'",sizeof(unsigned char),2));goto _LL23F;
_LL24B: Cyc_Absyndump_dump(_tmp11C);goto _LL23F;_LL24D: Cyc_Absyndump_dumpqvar(
_tmp11D->name);goto _LL23F;_LL24F:((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct
Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp11E,_tag_arr("$(",sizeof(
unsigned char),3),_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2));goto _LL23F;_LL251: Cyc_Absyndump_dump(_tag_arr("&",sizeof(
unsigned char),2));Cyc_Absyndump_dumppat(_tmp11F);goto _LL23F;_LL253: Cyc_Absyndump_dump(
_tag_arr("*",sizeof(unsigned char),2));Cyc_Absyndump_dumpqvar(_tmp120->name);
goto _LL23F;_LL255: Cyc_Absyndump_dumpqvar(_tmp121);goto _LL23F;_LL257: Cyc_Absyndump_dumpqvar(
_tmp124);Cyc_Absyndump_dumptvars(_tmp123);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp122,_tag_arr("(",
sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LL23F;_LL259: Cyc_Absyndump_dumpqvar(_tmp127);Cyc_Absyndump_dumptvars(
_tmp126);((void(*)(void(*f)(struct _tuple9*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp,_tmp125,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));goto _LL23F;
_LL25B: if(_tmp12A->name != 0){Cyc_Absyndump_dumpqvar((struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(_tmp12A->name))->v);}Cyc_Absyndump_dumptvars(_tmp129);((void(*)(void(*
f)(struct _tuple9*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp128,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LL23F;_LL25D: Cyc_Absyndump_dumpqvar(
_tmp12D->name);Cyc_Absyndump_dumptvars(_tmp12C);if(_tmp12B != 0){((void(*)(void(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
_tmp12B,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));}goto _LL23F;_LL25F: Cyc_Absyndump_dumpqvar(
_tmp12E->name);goto _LL23F;_LL261: Cyc_Absyndump_dumpqvar(_tmp12F->name);goto
_LL23F;_LL23F:;}void Cyc_Absyndump_dumptunionfield(struct Cyc_Absyn_Tunionfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);if(ef->typs != 0){Cyc_Absyndump_dumpargs(ef->typs);}}
void Cyc_Absyndump_dumptunionfields(struct Cyc_List_List*fields){((void(*)(void(*f)(
struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumptunionfield,fields,_tag_arr(",",sizeof(unsigned char),2));}
void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){Cyc_Absyndump_dumpqvar(
ef->name);if(ef->tag != 0){Cyc_Absyndump_dump(_tag_arr(" = ",sizeof(unsigned char),
4));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List*fields){((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,
fields,_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpstructfields(
struct Cyc_List_List*fields){for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Structfield
_tmp135;struct Cyc_List_List*_tmp136;struct Cyc_Absyn_Exp*_tmp137;void*_tmp138;
struct Cyc_Absyn_Tqual _tmp139;struct _tagged_arr*_tmp13A;struct Cyc_Absyn_Structfield*
_tmp134=(struct Cyc_Absyn_Structfield*)fields->hd;_tmp135=*_tmp134;_LL280: _tmp13A=
_tmp135.name;goto _LL27F;_LL27F: _tmp139=_tmp135.tq;goto _LL27E;_LL27E: _tmp138=(
void*)_tmp135.type;goto _LL27D;_LL27D: _tmp137=_tmp135.width;goto _LL27C;_LL27C:
_tmp136=_tmp135.attributes;goto _LL27B;_LL27B:((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _tagged_arr*),struct _tagged_arr*))Cyc_Absyndump_dumptqtd)(
_tmp139,_tmp138,Cyc_Absyndump_dump_str,_tmp13A);Cyc_Absyndump_dumpatts(_tmp136);
if(_tmp137 != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp((struct
Cyc_Absyn_Exp*)_check_null(_tmp137));}Cyc_Absyndump_dump_semi();}}void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(td->name);Cyc_Absyndump_dumptvars(
td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple10{void*f1;struct
_tuple0*f2;};static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple10*pr){{void*
_tmp13B=(*pr).f1;_LL282: if((int)_tmp13B == 8){goto _LL283;}else{goto _LL284;}_LL284:
if((int)_tmp13B == 0){goto _LL285;}else{goto _LL286;}_LL286: if((int)_tmp13B == 1){
goto _LL287;}else{goto _LL288;}_LL288: if((int)_tmp13B == 2){goto _LL289;}else{goto
_LL28A;}_LL28A: goto _LL28B;_LL283: goto _LL281;_LL285: Cyc_Absyndump_dump(_tag_arr("_stdcall",
sizeof(unsigned char),9));goto _LL281;_LL287: Cyc_Absyndump_dump(_tag_arr("_cdecl",
sizeof(unsigned char),7));goto _LL281;_LL289: Cyc_Absyndump_dump(_tag_arr("_fastcall",
sizeof(unsigned char),10));goto _LL281;_LL28B: goto _LL281;_LL281:;}Cyc_Absyndump_dumpqvar((*
pr).f2);}static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv(fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}
static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){for(0;vds != 0;vds=vds->tl){
Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl != 0){
Cyc_Absyndump_dump_char((int)',');}}}void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Vardecl _tmp13D;struct Cyc_List_List*
_tmp13E;struct Cyc_Absyn_Exp*_tmp13F;void*_tmp140;struct Cyc_Absyn_Tqual _tmp141;
struct _tuple0*_tmp142;void*_tmp143;struct Cyc_Absyn_Vardecl*_tmp13C=vd;_tmp13D=*
_tmp13C;_LL292: _tmp143=(void*)_tmp13D.sc;goto _LL291;_LL291: _tmp142=_tmp13D.name;
goto _LL290;_LL290: _tmp141=_tmp13D.tq;goto _LL28F;_LL28F: _tmp140=(void*)_tmp13D.type;
goto _LL28E;_LL28E: _tmp13F=_tmp13D.initializer;goto _LL28D;_LL28D: _tmp13E=_tmp13D.attributes;
goto _LL28C;_LL28C: Cyc_Absyndump_dumploc(loc);if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dumpatts(
_tmp13E);Cyc_Absyndump_dumpscope(_tmp143);{struct Cyc_List_List*_tmp145;void*
_tmp146;struct Cyc_Absyn_Tqual _tmp147;struct _tuple4 _tmp144=Cyc_Absynpp_to_tms(
_tmp141,_tmp140);_LL296: _tmp147=_tmp144.f1;goto _LL295;_LL295: _tmp146=_tmp144.f2;
goto _LL294;_LL294: _tmp145=_tmp144.f3;goto _LL293;_LL293: {void*call_conv=(void*)8;{
struct Cyc_List_List*tms2=_tmp145;for(0;tms2 != 0;tms2=tms2->tl){void*_tmp148=(
void*)tms2->hd;struct Cyc_List_List*_tmp149;_LL298: if((unsigned int)_tmp148 > 1?*((
int*)_tmp148)== 4: 0){_LL29C: _tmp149=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmp148)->f2;goto _LL299;}else{goto _LL29A;}_LL29A: goto _LL29B;_LL299: for(0;_tmp149
!= 0;_tmp149=_tmp149->tl){void*_tmp14A=(void*)_tmp149->hd;_LL29E: if((int)_tmp14A
== 0){goto _LL29F;}else{goto _LL2A0;}_LL2A0: if((int)_tmp14A == 1){goto _LL2A1;}else{
goto _LL2A2;}_LL2A2: if((int)_tmp14A == 2){goto _LL2A3;}else{goto _LL2A4;}_LL2A4: goto
_LL2A5;_LL29F: call_conv=(void*)0;goto _LL29D;_LL2A1: call_conv=(void*)1;goto _LL29D;
_LL2A3: call_conv=(void*)2;goto _LL29D;_LL2A5: goto _LL29D;_LL29D:;}goto _LL297;
_LL29B: goto _LL297;_LL297:;}}Cyc_Absyndump_dumptq(_tmp147);Cyc_Absyndump_dumpntyp(
_tmp146);{struct _tuple10 _tmp14B=({struct _tuple10 _tmp14C;_tmp14C.f1=call_conv;
_tmp14C.f2=_tmp142;_tmp14C;});((void(*)(struct Cyc_List_List*tms,void(*f)(struct
_tuple10*),struct _tuple10*a))Cyc_Absyndump_dumptms)(Cyc_List_imp_rev(_tmp145),
Cyc_Absyndump_dump_callconv_qvar,& _tmp14B);}}}}else{Cyc_Absyndump_dumpscope(
_tmp143);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct
_tuple0*))Cyc_Absyndump_dumptqtd)(_tmp141,_tmp140,Cyc_Absyndump_dumpqvar,_tmp142);
Cyc_Absyndump_dumpatts(_tmp13E);}if(_tmp13F != 0){Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp13F));}Cyc_Absyndump_dump_semi();}
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){void*_tmp14D=(void*)d->r;
struct Cyc_Absyn_Fndecl*_tmp14E;struct Cyc_Absyn_Structdecl*_tmp14F;struct Cyc_Absyn_Uniondecl*
_tmp150;struct Cyc_Absyn_Vardecl*_tmp151;struct Cyc_Absyn_Tuniondecl*_tmp152;
struct Cyc_Absyn_Tuniondecl _tmp153;int _tmp154;struct Cyc_Core_Opt*_tmp155;struct
Cyc_List_List*_tmp156;struct _tuple0*_tmp157;void*_tmp158;struct Cyc_Absyn_Enumdecl*
_tmp159;struct Cyc_Absyn_Enumdecl _tmp15A;struct Cyc_Core_Opt*_tmp15B;struct _tuple0*
_tmp15C;void*_tmp15D;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_Absyn_Pat*_tmp15F;
struct Cyc_List_List*_tmp160;struct Cyc_Absyn_Typedefdecl*_tmp161;struct Cyc_List_List*
_tmp162;struct _tagged_arr*_tmp163;struct Cyc_List_List*_tmp164;struct _tuple0*
_tmp165;struct Cyc_List_List*_tmp166;_LL2A7: if(*((int*)_tmp14D)== 1){_LL2BF:
_tmp14E=((struct Cyc_Absyn_Fn_d_struct*)_tmp14D)->f1;goto _LL2A8;}else{goto _LL2A9;}
_LL2A9: if(*((int*)_tmp14D)== 4){_LL2C0: _tmp14F=((struct Cyc_Absyn_Struct_d_struct*)
_tmp14D)->f1;goto _LL2AA;}else{goto _LL2AB;}_LL2AB: if(*((int*)_tmp14D)== 5){_LL2C1:
_tmp150=((struct Cyc_Absyn_Union_d_struct*)_tmp14D)->f1;goto _LL2AC;}else{goto
_LL2AD;}_LL2AD: if(*((int*)_tmp14D)== 0){_LL2C2: _tmp151=((struct Cyc_Absyn_Var_d_struct*)
_tmp14D)->f1;goto _LL2AE;}else{goto _LL2AF;}_LL2AF: if(*((int*)_tmp14D)== 6){_LL2C3:
_tmp152=((struct Cyc_Absyn_Tunion_d_struct*)_tmp14D)->f1;_tmp153=*_tmp152;_LL2C8:
_tmp158=(void*)_tmp153.sc;goto _LL2C7;_LL2C7: _tmp157=_tmp153.name;goto _LL2C6;
_LL2C6: _tmp156=_tmp153.tvs;goto _LL2C5;_LL2C5: _tmp155=_tmp153.fields;goto _LL2C4;
_LL2C4: _tmp154=_tmp153.is_xtunion;goto _LL2B0;}else{goto _LL2B1;}_LL2B1: if(*((int*)
_tmp14D)== 7){_LL2C9: _tmp159=((struct Cyc_Absyn_Enum_d_struct*)_tmp14D)->f1;
_tmp15A=*_tmp159;_LL2CC: _tmp15D=(void*)_tmp15A.sc;goto _LL2CB;_LL2CB: _tmp15C=
_tmp15A.name;goto _LL2CA;_LL2CA: _tmp15B=_tmp15A.fields;goto _LL2B2;}else{goto
_LL2B3;}_LL2B3: if(*((int*)_tmp14D)== 2){_LL2CE: _tmp15F=((struct Cyc_Absyn_Let_d_struct*)
_tmp14D)->f1;goto _LL2CD;_LL2CD: _tmp15E=((struct Cyc_Absyn_Let_d_struct*)_tmp14D)->f4;
goto _LL2B4;}else{goto _LL2B5;}_LL2B5: if(*((int*)_tmp14D)== 3){_LL2CF: _tmp160=((
struct Cyc_Absyn_Letv_d_struct*)_tmp14D)->f1;goto _LL2B6;}else{goto _LL2B7;}_LL2B7:
if(*((int*)_tmp14D)== 8){_LL2D0: _tmp161=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp14D)->f1;goto _LL2B8;}else{goto _LL2B9;}_LL2B9: if(*((int*)_tmp14D)== 9){_LL2D2:
_tmp163=((struct Cyc_Absyn_Namespace_d_struct*)_tmp14D)->f1;goto _LL2D1;_LL2D1:
_tmp162=((struct Cyc_Absyn_Namespace_d_struct*)_tmp14D)->f2;goto _LL2BA;}else{goto
_LL2BB;}_LL2BB: if(*((int*)_tmp14D)== 10){_LL2D4: _tmp165=((struct Cyc_Absyn_Using_d_struct*)
_tmp14D)->f1;goto _LL2D3;_LL2D3: _tmp164=((struct Cyc_Absyn_Using_d_struct*)_tmp14D)->f2;
goto _LL2BC;}else{goto _LL2BD;}_LL2BD: if(*((int*)_tmp14D)== 11){_LL2D5: _tmp166=((
struct Cyc_Absyn_ExternC_d_struct*)_tmp14D)->f1;goto _LL2BE;}else{goto _LL2A6;}
_LL2A8: Cyc_Absyndump_dumploc(d->loc);if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dumpatts(
_tmp14E->attributes);}if(_tmp14E->is_inline){if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dump(
_tag_arr("__inline",sizeof(unsigned char),9));}else{Cyc_Absyndump_dump(_tag_arr("inline",
sizeof(unsigned char),7));}}Cyc_Absyndump_dumpscope((void*)_tmp14E->sc);{void*t=(
void*)({struct Cyc_Absyn_FnType_struct*_tmp168=_cycalloc(sizeof(struct Cyc_Absyn_FnType_struct));
_tmp168[0]=({struct Cyc_Absyn_FnType_struct _tmp169;_tmp169.tag=8;_tmp169.f1=({
struct Cyc_Absyn_FnInfo _tmp16A;_tmp16A.tvars=_tmp14E->tvs;_tmp16A.effect=_tmp14E->effect;
_tmp16A.ret_typ=(void*)((void*)_tmp14E->ret_type);_tmp16A.args=((struct Cyc_List_List*(*)(
struct _tuple1*(*f)(struct _tuple3*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
_tmp14E->args);_tmp16A.c_varargs=_tmp14E->c_varargs;_tmp16A.cyc_varargs=_tmp14E->cyc_varargs;
_tmp16A.rgn_po=_tmp14E->rgn_po;_tmp16A.attributes=0;_tmp16A;});_tmp169;});
_tmp168;});((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp167;
_tmp167.q_const=0;_tmp167.q_volatile=0;_tmp167.q_restrict=0;_tmp167;}),t,Cyc_Absyndump_to_VC?
Cyc_Absyndump_dump_callconv_fdqvar: Cyc_Absyndump_dump_atts_qvar,_tmp14E);Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstmt(_tmp14E->body);Cyc_Absyndump_dump_char((int)'}');
goto _LL2A6;}_LL2AA: Cyc_Absyndump_dumpscope((void*)_tmp14F->sc);Cyc_Absyndump_dump(
_tag_arr("struct",sizeof(unsigned char),7));if(_tmp14F->name != 0){Cyc_Absyndump_dumpqvar((
struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmp14F->name))->v);}Cyc_Absyndump_dumptvars(
_tmp14F->tvs);if(_tmp14F->fields == 0){Cyc_Absyndump_dump_semi();}else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstructfields((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp14F->fields))->v);Cyc_Absyndump_dump(_tag_arr("}",sizeof(
unsigned char),2));Cyc_Absyndump_dumpatts(_tmp14F->attributes);Cyc_Absyndump_dump(
_tag_arr(";",sizeof(unsigned char),2));}goto _LL2A6;_LL2AC: Cyc_Absyndump_dumpscope((
void*)_tmp150->sc);Cyc_Absyndump_dump(_tag_arr("union",sizeof(unsigned char),6));
if(_tmp150->name != 0){Cyc_Absyndump_dumpqvar((struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(_tmp150->name))->v);}Cyc_Absyndump_dumptvars(_tmp150->tvs);if(
_tmp150->fields == 0){Cyc_Absyndump_dump_semi();}else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstructfields((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp150->fields))->v);Cyc_Absyndump_dump(_tag_arr("}",sizeof(
unsigned char),2));Cyc_Absyndump_dumpatts(_tmp150->attributes);Cyc_Absyndump_dump(
_tag_arr(";",sizeof(unsigned char),2));}goto _LL2A6;_LL2AE: Cyc_Absyndump_dumpvardecl(
_tmp151,d->loc);goto _LL2A6;_LL2B0: Cyc_Absyndump_dumpscope(_tmp158);if(_tmp154){
Cyc_Absyndump_dump(_tag_arr("xtunion ",sizeof(unsigned char),9));}else{Cyc_Absyndump_dump(
_tag_arr("tunion ",sizeof(unsigned char),8));}Cyc_Absyndump_dumpqvar(_tmp157);
Cyc_Absyndump_dumptvars(_tmp156);if(_tmp155 == 0){Cyc_Absyndump_dump_semi();}
else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumptunionfields((struct Cyc_List_List*)
_tmp155->v);Cyc_Absyndump_dump_nospace(_tag_arr("};",sizeof(unsigned char),3));}
goto _LL2A6;_LL2B2: Cyc_Absyndump_dumpscope(_tmp15D);Cyc_Absyndump_dump(_tag_arr("enum ",
sizeof(unsigned char),6));Cyc_Absyndump_dumpqvar(_tmp15C);if(_tmp15B == 0){Cyc_Absyndump_dump_semi();}
else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)
_tmp15B->v);Cyc_Absyndump_dump_nospace(_tag_arr("};",sizeof(unsigned char),3));}
return;_LL2B4: Cyc_Absyndump_dump(_tag_arr("let",sizeof(unsigned char),4));Cyc_Absyndump_dumppat(
_tmp15F);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(_tmp15E);Cyc_Absyndump_dump_semi();
goto _LL2A6;_LL2B6: Cyc_Absyndump_dump(_tag_arr("let ",sizeof(unsigned char),5));
Cyc_Absyndump_dumpids(_tmp160);Cyc_Absyndump_dump_semi();goto _LL2A6;_LL2B8: if(!
Cyc_Absyndump_expand_typedefs? 1: Cyc_Absynpp_is_anon_structtype((void*)_tmp161->defn)){
Cyc_Absyndump_dump(_tag_arr("typedef",sizeof(unsigned char),8));((void(*)(struct
Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))
Cyc_Absyndump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp16B;_tmp16B.q_const=0;_tmp16B.q_volatile=
0;_tmp16B.q_restrict=0;_tmp16B;}),(void*)_tmp161->defn,Cyc_Absyndump_dumptypedefname,
_tmp161);Cyc_Absyndump_dump_semi();}goto _LL2A6;_LL2BA: Cyc_Absyndump_dump(
_tag_arr("namespace",sizeof(unsigned char),10));Cyc_Absyndump_dump_str(_tmp163);
Cyc_Absyndump_dump_char((int)'{');for(0;_tmp162 != 0;_tmp162=_tmp162->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)_tmp162->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL2A6;
_LL2BC: Cyc_Absyndump_dump(_tag_arr("using",sizeof(unsigned char),6));Cyc_Absyndump_dumpqvar(
_tmp165);Cyc_Absyndump_dump_char((int)'{');for(0;_tmp164 != 0;_tmp164=_tmp164->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp164->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL2A6;_LL2BE: Cyc_Absyndump_dump(_tag_arr("extern \"C\" {",sizeof(
unsigned char),13));for(0;_tmp166 != 0;_tmp166=_tmp166->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)_tmp166->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL2A6;
_LL2A6:;}static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
unsigned int i=Cyc_Evexp_eval_const_uint_exp(e);if(i != 1){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}void Cyc_Absyndump_dumptms(
struct Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){f(a);return;}{void*
_tmp16C=(void*)tms->hd;struct Cyc_Absyn_Tqual _tmp16D;void*_tmp16E;void*_tmp16F;
struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Tqual _tmp171;void*_tmp172;void*
_tmp173;struct Cyc_Absyn_Exp*_tmp174;struct Cyc_Absyn_Tqual _tmp175;void*_tmp176;
void*_tmp177;struct Cyc_Absyn_Tqual _tmp178;void*_tmp179;struct Cyc_Absyn_Tvar*
_tmp17A;void*_tmp17B;struct Cyc_Absyn_Exp*_tmp17C;struct Cyc_Absyn_Tqual _tmp17D;
void*_tmp17E;struct Cyc_Absyn_Tvar*_tmp17F;void*_tmp180;struct Cyc_Absyn_Exp*
_tmp181;struct Cyc_Absyn_Tqual _tmp182;void*_tmp183;struct Cyc_Absyn_Tvar*_tmp184;
void*_tmp185;_LL2D7: if((unsigned int)_tmp16C > 1?*((int*)_tmp16C)== 1: 0){_LL2E9:
_tmp16F=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp16C)->f1;if((
unsigned int)_tmp16F > 1?*((int*)_tmp16F)== 1: 0){_LL2EA: _tmp170=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmp16F)->f1;goto _LL2E8;}else{goto _LL2D9;}_LL2E8: _tmp16E=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp16C)->f2;if((int)_tmp16E == 2){goto _LL2E7;}else{goto _LL2D9;}_LL2E7: _tmp16D=((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp16C)->f3;goto _LL2D8;}else{goto _LL2D9;}
_LL2D9: if((unsigned int)_tmp16C > 1?*((int*)_tmp16C)== 1: 0){_LL2ED: _tmp173=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp16C)->f1;if((unsigned int)_tmp173 > 1?*((
int*)_tmp173)== 0: 0){_LL2EE: _tmp174=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmp173)->f1;goto _LL2EC;}else{goto _LL2DB;}_LL2EC: _tmp172=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp16C)->f2;if((int)_tmp172 == 2){goto _LL2EB;}else{goto _LL2DB;}_LL2EB: _tmp171=((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp16C)->f3;goto _LL2DA;}else{goto _LL2DB;}
_LL2DB: if((unsigned int)_tmp16C > 1?*((int*)_tmp16C)== 1: 0){_LL2F1: _tmp177=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp16C)->f1;if((int)_tmp177 == 0){goto _LL2F0;}
else{goto _LL2DD;}_LL2F0: _tmp176=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp16C)->f2;if((int)_tmp176 == 2){goto _LL2EF;}else{goto _LL2DD;}_LL2EF: _tmp175=((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp16C)->f3;goto _LL2DC;}else{goto _LL2DD;}
_LL2DD: if((unsigned int)_tmp16C > 1?*((int*)_tmp16C)== 1: 0){_LL2F5: _tmp17B=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp16C)->f1;if((unsigned int)_tmp17B > 1?*((
int*)_tmp17B)== 1: 0){_LL2F6: _tmp17C=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmp17B)->f1;goto _LL2F3;}else{goto _LL2DF;}_LL2F3: _tmp179=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp16C)->f2;if((unsigned int)_tmp179 > 3?*((int*)_tmp179)== 1: 0){_LL2F4: _tmp17A=((
struct Cyc_Absyn_VarType_struct*)_tmp179)->f1;goto _LL2F2;}else{goto _LL2DF;}_LL2F2:
_tmp178=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp16C)->f3;goto _LL2DE;}else{goto
_LL2DF;}_LL2DF: if((unsigned int)_tmp16C > 1?*((int*)_tmp16C)== 1: 0){_LL2FA:
_tmp180=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp16C)->f1;if((
unsigned int)_tmp180 > 1?*((int*)_tmp180)== 0: 0){_LL2FB: _tmp181=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmp180)->f1;goto _LL2F8;}else{goto _LL2E1;}_LL2F8: _tmp17E=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp16C)->f2;if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 1: 0){_LL2F9: _tmp17F=((
struct Cyc_Absyn_VarType_struct*)_tmp17E)->f1;goto _LL2F7;}else{goto _LL2E1;}_LL2F7:
_tmp17D=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp16C)->f3;goto _LL2E0;}else{goto
_LL2E1;}_LL2E1: if((unsigned int)_tmp16C > 1?*((int*)_tmp16C)== 1: 0){_LL2FF:
_tmp185=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp16C)->f1;if((int)
_tmp185 == 0){goto _LL2FD;}else{goto _LL2E3;}_LL2FD: _tmp183=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp16C)->f2;if((unsigned int)_tmp183 > 3?*((int*)_tmp183)== 1: 0){_LL2FE: _tmp184=((
struct Cyc_Absyn_VarType_struct*)_tmp183)->f1;goto _LL2FC;}else{goto _LL2E3;}_LL2FC:
_tmp182=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp16C)->f3;goto _LL2E2;}else{goto
_LL2E3;}_LL2E3: if((unsigned int)_tmp16C > 1?*((int*)_tmp16C)== 1: 0){goto _LL2E4;}
else{goto _LL2E5;}_LL2E5: goto _LL2E6;_LL2D8: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dump_upperbound(
_tmp170);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL2DA: Cyc_Absyndump_dump_char((
int)'@');Cyc_Absyndump_dump_upperbound(_tmp174);Cyc_Absyndump_dumptms(tms->tl,f,
a);return;_LL2DC: Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumptms(tms->tl,
f,a);return;_LL2DE: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dump_upperbound(
_tmp17C);Cyc_Absyndump_dump_str(_tmp17A->name);Cyc_Absyndump_dumptms(tms->tl,f,a);
return;_LL2E0: Cyc_Absyndump_dump_char((int)'@');Cyc_Absyndump_dump_upperbound(
_tmp181);Cyc_Absyndump_dump_str(_tmp17F->name);Cyc_Absyndump_dumptms(tms->tl,f,a);
return;_LL2E2: Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dump_str(_tmp184->name);
Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL2E4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp186=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp186[0]=({struct
Cyc_Core_Impossible_struct _tmp187;_tmp187.tag=Cyc_Core_Impossible;_tmp187.f1=
_tag_arr("dumptms: bad Pointer_mod",sizeof(unsigned char),25);_tmp187;});_tmp186;}));
_LL2E6: {int next_is_pointer=0;if(tms->tl != 0){void*_tmp188=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;_LL301: if((unsigned int)_tmp188 > 1?*((int*)_tmp188)== 
1: 0){goto _LL302;}else{goto _LL303;}_LL303: goto _LL304;_LL302: next_is_pointer=1;
goto _LL300;_LL304: goto _LL300;_LL300:;}if(next_is_pointer){Cyc_Absyndump_dump_char((
int)'(');}Cyc_Absyndump_dumptms(tms->tl,f,a);if(next_is_pointer){Cyc_Absyndump_dump_char((
int)')');}{void*_tmp189=(void*)tms->hd;struct Cyc_Absyn_Exp*_tmp18A;void*_tmp18B;
struct Cyc_List_List*_tmp18C;struct Cyc_Core_Opt*_tmp18D;struct Cyc_Absyn_VarargInfo*
_tmp18E;int _tmp18F;struct Cyc_List_List*_tmp190;void*_tmp191;struct Cyc_Position_Segment*
_tmp192;struct Cyc_List_List*_tmp193;int _tmp194;struct Cyc_Position_Segment*
_tmp195;struct Cyc_List_List*_tmp196;struct Cyc_List_List*_tmp197;void*_tmp198;
void*_tmp199;_LL306: if((int)_tmp189 == 0){goto _LL307;}else{goto _LL308;}_LL308: if((
unsigned int)_tmp189 > 1?*((int*)_tmp189)== 0: 0){_LL314: _tmp18A=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp189)->f1;goto _LL309;}else{goto _LL30A;}_LL30A: if((unsigned int)_tmp189 > 1?*((
int*)_tmp189)== 2: 0){_LL315: _tmp18B=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp189)->f1;if(*((int*)_tmp18B)== 1){_LL31A: _tmp190=((struct Cyc_Absyn_WithTypes_struct*)
_tmp18B)->f1;goto _LL319;_LL319: _tmp18F=((struct Cyc_Absyn_WithTypes_struct*)
_tmp18B)->f2;goto _LL318;_LL318: _tmp18E=((struct Cyc_Absyn_WithTypes_struct*)
_tmp18B)->f3;goto _LL317;_LL317: _tmp18D=((struct Cyc_Absyn_WithTypes_struct*)
_tmp18B)->f4;goto _LL316;_LL316: _tmp18C=((struct Cyc_Absyn_WithTypes_struct*)
_tmp18B)->f5;goto _LL30B;}else{goto _LL30C;}}else{goto _LL30C;}_LL30C: if((
unsigned int)_tmp189 > 1?*((int*)_tmp189)== 2: 0){_LL31B: _tmp191=(void*)((struct
Cyc_Absyn_Function_mod_struct*)_tmp189)->f1;if(*((int*)_tmp191)== 0){_LL31D:
_tmp193=((struct Cyc_Absyn_NoTypes_struct*)_tmp191)->f1;goto _LL31C;_LL31C: _tmp192=((
struct Cyc_Absyn_NoTypes_struct*)_tmp191)->f2;goto _LL30D;}else{goto _LL30E;}}else{
goto _LL30E;}_LL30E: if((unsigned int)_tmp189 > 1?*((int*)_tmp189)== 3: 0){_LL320:
_tmp196=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp189)->f1;goto _LL31F;_LL31F:
_tmp195=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp189)->f2;goto _LL31E;_LL31E:
_tmp194=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp189)->f3;goto _LL30F;}else{
goto _LL310;}_LL310: if((unsigned int)_tmp189 > 1?*((int*)_tmp189)== 4: 0){_LL321:
_tmp197=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp189)->f2;goto _LL311;}else{
goto _LL312;}_LL312: if((unsigned int)_tmp189 > 1?*((int*)_tmp189)== 1: 0){_LL323:
_tmp199=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp189)->f1;goto _LL322;
_LL322: _tmp198=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp189)->f2;goto
_LL313;}else{goto _LL305;}_LL307: Cyc_Absyndump_dump(_tag_arr("[]",sizeof(
unsigned char),3));goto _LL305;_LL309: Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(
_tmp18A);Cyc_Absyndump_dump_char((int)']');goto _LL305;_LL30B: Cyc_Absyndump_dumpfunargs(
_tmp190,_tmp18F,_tmp18E,_tmp18D,_tmp18C);goto _LL305;_LL30D:((void(*)(void(*f)(
struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,
_tmp193,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LL305;_LL30F: if(_tmp194){Cyc_Absyndump_dumpkindedtvars(
_tmp196);}else{Cyc_Absyndump_dumptvars(_tmp196);}goto _LL305;_LL311: Cyc_Absyndump_dumpatts(
_tmp197);goto _LL305;_LL313:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp19A=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp19A[0]=({struct
Cyc_Core_Impossible_struct _tmp19B;_tmp19B.tag=Cyc_Core_Impossible;_tmp19B.f1=
_tag_arr("dumptms",sizeof(unsigned char),8);_tmp19B;});_tmp19A;}));_LL305:;}
return;}_LL2D6:;}}void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*
f)(void*),void*a){struct Cyc_List_List*_tmp19D;void*_tmp19E;struct Cyc_Absyn_Tqual
_tmp19F;struct _tuple4 _tmp19C=Cyc_Absynpp_to_tms(tq,t);_LL327: _tmp19F=_tmp19C.f1;
goto _LL326;_LL326: _tmp19E=_tmp19C.f2;goto _LL325;_LL325: _tmp19D=_tmp19C.f3;goto
_LL324;_LL324: Cyc_Absyndump_dumptq(_tmp19F);Cyc_Absyndump_dumpntyp(_tmp19E);Cyc_Absyndump_dumptms(
Cyc_List_imp_rev(_tmp19D),f,a);}void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc_Std___sFILE*f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=f;for(
0;tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}({
void*_tmp1A0[0]={};Cyc_Std_fprintf(f,_tag_arr("\n",sizeof(unsigned char),2),
_tag_arr(_tmp1A0,sizeof(void*),0));});}
