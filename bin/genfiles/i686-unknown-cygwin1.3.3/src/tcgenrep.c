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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void *_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void *_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (!_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)cks_ptr) + cks_elt_sz*cks_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#else
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  if (!_cus_arr.base) _throw_null(); \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif

#define _tag_arr(tcurr,elt_sz,num_elts) ({ \
  struct _tagged_arr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void *)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void * _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_arr(arr,elt_sz,num_elts) ((arr).curr)
#else
#define _untag_arr(arr,elt_sz,num_elts) ({ \
  struct _tagged_arr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif

#define _get_arr_size(arr,elt_sz) \
  ({struct _tagged_arr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})

#define _tagged_arr_plus(arr,elt_sz,change) ({ \
  struct _tagged_arr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })

#define _tagged_arr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })

#define _tagged_arr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  struct _tagged_arr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })

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
x,struct Cyc_List_List*y);extern struct Cyc_List_List*Cyc_List_zip3(struct Cyc_List_List*
x,struct Cyc_List_List*y,struct Cyc_List_List*z);struct _tuple0{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};extern struct _tuple0 Cyc_List_split(struct Cyc_List_List*
x);extern struct Cyc_List_List*Cyc_List_tabulate(int n,void*(*f)(int));extern struct
Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];
extern struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));extern struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);extern void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict*d,void*k);extern struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict*d,void*k);extern void Cyc_Dict_iter(void(*f)(void*,void*),
struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*Cyc_Dict_delete(struct Cyc_Dict_Dict*,
void*);extern unsigned int Cyc_Std_strlen(struct _tagged_arr s);extern int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_strdup(
struct _tagged_arr src);struct Cyc_Hashtable_Table;extern struct Cyc_Hashtable_Table*
Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));extern void
Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);extern void*
Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;extern struct Cyc_Position_Segment*
Cyc_Position_segment_of_abs(int,int);struct Cyc_Position_Error{struct _tagged_arr
source;struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
unsigned char Cyc_Position_Nocontext[14];struct Cyc_Typerep_Int_struct{int tag;int
f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*
f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple1{unsigned int
f1;struct _tagged_arr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int tag;struct
_tagged_arr*f1;unsigned int f2;struct _tagged_arr f3;};struct _tuple2{unsigned int f1;
void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;struct _tagged_arr
f2;};struct _tuple3{unsigned int f1;struct _tagged_arr f2;};struct Cyc_Typerep_TUnion_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;struct _tagged_arr f3;};struct
_tuple4{struct _tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{int tag;
struct _tagged_arr f1;struct _tagged_arr f2;};struct Cyc_Typerep_Union_struct{int tag;
struct _tagged_arr*f1;struct _tagged_arr f2;};unsigned int Cyc_Typerep_size_type(
void*rep);struct Cyc_Std_tm{int tm_sec;int tm_min;int tm_hour;int tm_mday;int tm_mon;
int tm_year;int tm_wday;int tm_yday;int tm_isdst;};struct Cyc_Absyn_Rel_n_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple5{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
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
_tuple5*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple5*
tunion_name;struct _tuple5*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
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
Cyc_List_List*f1;};struct Cyc_Absyn_StructType_struct{int tag;struct _tuple5*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Structdecl**f3;};struct Cyc_Absyn_UnionType_struct{
int tag;struct _tuple5*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Uniondecl**f3;};
struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple5*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_AnonStructType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonUnionType_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple5*f1;struct Cyc_List_List*f2;struct Cyc_Core_Opt*f3;};struct Cyc_Absyn_AccessEff_struct{
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
tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple5*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple5*f1;};struct Cyc_Absyn_Primop_e_struct{
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
int tag;struct Cyc_List_List*f1;};struct _tuple6{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple6*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple5*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Tuniondecl*f4;struct Cyc_Absyn_Tunionfield*
f5;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple5*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple5*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Codegen_e_struct{int tag;struct
Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Fill_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct _tuple7{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;struct _tuple7 condition;
struct _tuple7 delta;struct Cyc_Absyn_Stmt*body;};struct Cyc_Absyn_Exp_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple7
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct
Cyc_Absyn_Goto_s_struct{int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple7 f2;
struct _tuple7 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_SwitchC_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Cut_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Splice_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Label_s_struct{int tag;struct _tagged_arr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct _tuple7 f2;};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple5*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple5*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
Cyc_Absyn_UnknownFields_p_struct{int tag;struct _tuple5*f1;struct Cyc_List_List*f2;
struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*
cnst_exp;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
void*sc;struct _tuple5*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct _tuple5*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structfield{
struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structdecl{void*sc;struct
Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Uniondecl{void*sc;struct Cyc_Core_Opt*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple5*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple5*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple5*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple5*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple5*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Struct_d_struct{int tag;struct Cyc_Absyn_Structdecl*
f1;};struct Cyc_Absyn_Union_d_struct{int tag;struct Cyc_Absyn_Uniondecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple5*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[
15];extern int Cyc_Absyn_qvar_cmp(struct _tuple5*,struct _tuple5*);extern void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);extern void*Cyc_Absyn_uchar_t;extern void*Cyc_Absyn_uint_t;
extern void*Cyc_Absyn_const_string_typ(void*rgn);extern void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern void*Cyc_Absyn_at_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq);extern void*Cyc_Absyn_tagged_typ(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq);extern struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(void*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct
Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct
_tuple5*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple5*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Cstdio___sFILE;struct
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
td,struct _tuple5*fieldname){if(td->fields == 0){({void*_tmp8[0]={};((int(*)(
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
unsigned char _tmp20[7]="Struct";static struct _tagged_arr Cyc_Tcgenrep_struct_str={
_tmp20,_tmp20,_tmp20 + 7};static unsigned char _tmp21[7]="TUnion";static struct
_tagged_arr Cyc_Tcgenrep_tunion_str={_tmp21,_tmp21,_tmp21 + 7};static unsigned char
_tmp22[8]="XTUnion";static struct _tagged_arr Cyc_Tcgenrep_xtunion_str={_tmp22,
_tmp22,_tmp22 + 8};static unsigned char _tmp23[6]="Union";static struct _tagged_arr
Cyc_Tcgenrep_union_str={_tmp23,_tmp23,_tmp23 + 6};static unsigned char _tmp24[7]="name_t";
static struct _tagged_arr Cyc_Tcgenrep_name_t_str={_tmp24,_tmp24,_tmp24 + 7};static
struct _tuple5*Cyc_Tcgenrep_typerep_name(struct _tagged_arr*name){return({struct
_tuple5*_tmp25=_cycalloc(sizeof(struct _tuple5));_tmp25->f1=(void*)& Cyc_Tcgenrep_typerep_nmspace;
_tmp25->f2=name;_tmp25;});}static int Cyc_Tcgenrep_gen_id_counter=0;static struct
_tagged_arr*Cyc_Tcgenrep_new_gen_id(struct _tagged_arr name){return({struct
_tagged_arr*_tmp26=_cycalloc(sizeof(struct _tagged_arr));_tmp26[0]=(struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp29;_tmp29.tag=1;_tmp29.f1=(int)((
unsigned int)Cyc_Tcgenrep_gen_id_counter ++);{struct Cyc_Std_String_pa_struct
_tmp28;_tmp28.tag=0;_tmp28.f1=(struct _tagged_arr)name;{void*_tmp27[2]={& _tmp28,&
_tmp29};Cyc_Std_aprintf(_tag_arr("_gen%s_%d",sizeof(unsigned char),10),_tag_arr(
_tmp27,sizeof(void*),2));}}});_tmp26;});}static void Cyc_Tcgenrep_print_params(
struct Cyc_List_List*l){({void*_tmp2A[0]={};Cyc_Std_printf(_tag_arr("<",sizeof(
unsigned char),2),_tag_arr(_tmp2A,sizeof(void*),0));});{struct Cyc_List_List*p=l;
for(0;p != 0;p=p->tl){({struct Cyc_Std_Int_pa_struct _tmp2D;_tmp2D.tag=1;_tmp2D.f1=(
int)((unsigned int)((int)(p->tl != 0?',':' ')));{struct Cyc_Std_String_pa_struct
_tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)p->hd);{
void*_tmp2B[2]={& _tmp2C,& _tmp2D};Cyc_Std_printf(_tag_arr("%s%c",sizeof(
unsigned char),5),_tag_arr(_tmp2B,sizeof(void*),2));}}});}}({void*_tmp2E[0]={};
Cyc_Std_printf(_tag_arr(">\n",sizeof(unsigned char),3),_tag_arr(_tmp2E,sizeof(
void*),0));});}static void Cyc_Tcgenrep_print_tvars(struct Cyc_List_List*l){({void*
_tmp2F[0]={};Cyc_Std_printf(_tag_arr("<",sizeof(unsigned char),2),_tag_arr(
_tmp2F,sizeof(void*),0));});{struct Cyc_List_List*p=l;for(0;p != 0;p=p->tl){({
struct Cyc_Std_Int_pa_struct _tmp32;_tmp32.tag=1;_tmp32.f1=(int)((unsigned int)((
int)(p->tl != 0?',':' ')));{struct Cyc_Std_String_pa_struct _tmp31;_tmp31.tag=0;
_tmp31.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)p->hd)->name;{void*_tmp30[
2]={& _tmp31,& _tmp32};Cyc_Std_printf(_tag_arr("%s%c",sizeof(unsigned char),5),
_tag_arr(_tmp30,sizeof(void*),2));}}});}}({void*_tmp33[0]={};Cyc_Std_printf(
_tag_arr(">\n",sizeof(unsigned char),3),_tag_arr(_tmp33,sizeof(void*),0));});}
static struct Cyc_Absyn_Rel_n_struct Cyc_Tcgenrep_top_nm_0={0,0};static void*Cyc_Tcgenrep_top_nm=(
void*)& Cyc_Tcgenrep_top_nm_0;static struct _tuple5*Cyc_Tcgenrep_toplevel_name(
struct _tagged_arr*name){return({struct _tuple5*_tmp35=_cycalloc(sizeof(struct
_tuple5));_tmp35->f1=Cyc_Tcgenrep_top_nm;_tmp35->f2=(struct _tagged_arr*)name;
_tmp35;});}static struct _tuple5*Cyc_Tcgenrep_relative_name(struct Cyc_List_List*
pathl,struct _tagged_arr*name){struct Cyc_Absyn_Rel_n_struct*path=({struct Cyc_Absyn_Rel_n_struct*
_tmp37=_cycalloc(sizeof(struct Cyc_Absyn_Rel_n_struct));_tmp37[0]=({struct Cyc_Absyn_Rel_n_struct
_tmp38;_tmp38.tag=0;_tmp38.f1=pathl;_tmp38;});_tmp37;});return({struct _tuple5*
_tmp36=_cycalloc(sizeof(struct _tuple5));_tmp36->f1=(void*)path;_tmp36->f2=(
struct _tagged_arr*)name;_tmp36;});}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl(
struct _tagged_arr*name,void*type,struct Cyc_Absyn_Exp*init,struct Cyc_Position_Segment*
seg){struct _tuple5*qvar=Cyc_Tcgenrep_toplevel_name(name);struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(qvar,type,init);(void*)(vd->sc=(void*)((void*)0));{
struct Cyc_Absyn_Var_d_struct*r1=({struct Cyc_Absyn_Var_d_struct*_tmp39=_cycalloc(
sizeof(struct Cyc_Absyn_Var_d_struct));_tmp39[0]=({struct Cyc_Absyn_Var_d_struct
_tmp3A;_tmp3A.tag=0;_tmp3A.f1=vd;_tmp3A;});_tmp39;});void*r2=(void*)r1;return Cyc_Absyn_new_decl(
r2,seg);}}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_vardecl(struct _tuple5*name,
void*type,struct Cyc_Absyn_Exp*init,void*sc,struct Cyc_Position_Segment*seg){
struct _tuple5*topname=Cyc_Tcgenrep_toplevel_name((*name).f2);struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(topname,type,init);(void*)(vd->sc=(void*)sc);{struct Cyc_Absyn_Var_d_struct*
r1=({struct Cyc_Absyn_Var_d_struct*_tmp3B=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_struct));
_tmp3B[0]=({struct Cyc_Absyn_Var_d_struct _tmp3C;_tmp3C.tag=0;_tmp3C.f1=vd;_tmp3C;});
_tmp3B;});void*r2=(void*)r1;return Cyc_Absyn_new_decl(r2,seg);}}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_string(struct _tagged_arr s,struct Cyc_Position_Segment*seg){
return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_String_c_struct*_tmp3D=
_cycalloc(sizeof(struct Cyc_Absyn_String_c_struct));_tmp3D[0]=({struct Cyc_Absyn_String_c_struct
_tmp3E;_tmp3E.tag=5;_tmp3E.f1=s;_tmp3E;});_tmp3D;}),seg);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_string_cls(struct Cyc_Position_Segment*seg,struct _tagged_arr*s){
return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_String_c_struct*_tmp3F=
_cycalloc(sizeof(struct Cyc_Absyn_String_c_struct));_tmp3F[0]=({struct Cyc_Absyn_String_c_struct
_tmp40;_tmp40.tag=5;_tmp40.f1=*s;_tmp40;});_tmp3F;}),seg);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_qvar_string_cls(struct Cyc_Position_Segment*seg,struct _tuple5*s){
return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_String_c_struct*_tmp41=
_cycalloc(sizeof(struct Cyc_Absyn_String_c_struct));_tmp41[0]=({struct Cyc_Absyn_String_c_struct
_tmp42;_tmp42.tag=5;_tmp42.f1=*(*s).f2;_tmp42;});_tmp41;}),seg);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_int(int i,struct Cyc_Position_Segment*seg){return Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Int_c_struct*_tmp43=_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));
_tmp43[0]=({struct Cyc_Absyn_Int_c_struct _tmp44;_tmp44.tag=2;_tmp44.f1=(void*)((
void*)0);_tmp44.f2=i;_tmp44;});_tmp43;}),seg);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int_cls(
struct Cyc_Position_Segment*seg,int i){return Cyc_Absyn_const_exp((void*)({struct
Cyc_Absyn_Int_c_struct*_tmp45=_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));
_tmp45[0]=({struct Cyc_Absyn_Int_c_struct _tmp46;_tmp46.tag=2;_tmp46.f1=(void*)((
void*)0);_tmp46.f2=i;_tmp46;});_tmp45;}),seg);}static int Cyc_Tcgenrep_size_of2int(
void*sz){void*_tmp47=sz;_LL6: if((int)_tmp47 == 0){goto _LL7;}else{goto _LL8;}_LL8:
if((int)_tmp47 == 1){goto _LL9;}else{goto _LLA;}_LLA: if((int)_tmp47 == 2){goto _LLB;}
else{goto _LLC;}_LLC: if((int)_tmp47 == 3){goto _LLD;}else{goto _LL5;}_LL7: return 8;
_LL9: return 16;_LLB: return 32;_LLD: return 64;_LL5:;}static void*Cyc_Tcgenrep_array_typ(
void*ty,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp48=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp48[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp49;_tmp49.tag=7;_tmp49.f1=(void*)ty;_tmp49.f2=tq;_tmp49.f3=e;_tmp49;});
_tmp48;});}static void*Cyc_Tcgenrep_tunion_typ(struct _tuple5*name){return(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp4A=_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));
_tmp4A[0]=({struct Cyc_Absyn_TunionType_struct _tmp4B;_tmp4B.tag=2;_tmp4B.f1=({
struct Cyc_Absyn_TunionInfo _tmp4C;_tmp4C.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp4D=_cycalloc(sizeof(struct Cyc_Absyn_UnknownTunion_struct));_tmp4D[0]=({
struct Cyc_Absyn_UnknownTunion_struct _tmp4E;_tmp4E.tag=0;_tmp4E.f1=({struct Cyc_Absyn_UnknownTunionInfo
_tmp4F;_tmp4F.name=name;_tmp4F.is_xtunion=0;_tmp4F;});_tmp4E;});_tmp4D;}));
_tmp4C.targs=0;_tmp4C.rgn=(void*)((void*)2);_tmp4C;});_tmp4B;});_tmp4A;});}
static void*Cyc_Tcgenrep_tunionfield_typ(struct _tuple5*name,struct _tuple5*
fieldname){return(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp50=
_cycalloc(sizeof(struct Cyc_Absyn_TunionFieldType_struct));_tmp50[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp51;_tmp51.tag=3;_tmp51.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp52;_tmp52.field_info=(
void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*_tmp53=_cycalloc(
sizeof(struct Cyc_Absyn_UnknownTunionfield_struct));_tmp53[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp54;_tmp54.tag=0;_tmp54.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp55;
_tmp55.tunion_name=name;_tmp55.field_name=fieldname;_tmp55.is_xtunion=0;_tmp55;});
_tmp54;});_tmp53;}));_tmp52.targs=0;_tmp52;});_tmp51;});_tmp50;});}static struct
Cyc_Absyn_Exp*Cyc_Tcgenrep_call_exp(struct _tuple5*name,struct Cyc_List_List*args,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_unknowncall_exp(Cyc_Absyn_unknownid_exp(
name,loc),args,loc);}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_tunion_constructor_decl(
struct _tuple5*tunionname,struct _tuple5*fieldname,struct _tuple5*varname,struct Cyc_List_List*
args,void*sc,struct Cyc_Position_Segment*loc){void*_tmp56=Cyc_Tcgenrep_tunionfield_typ(
tunionname,fieldname);struct Cyc_Absyn_Exp*_tmp57=Cyc_Tcgenrep_call_exp(fieldname,
args,loc);struct Cyc_Absyn_Decl*_tmp58=Cyc_Tcgenrep_gen_vardecl(varname,_tmp56,(
struct Cyc_Absyn_Exp*)_tmp57,sc,loc);return _tmp58;}struct _tuple8{struct Cyc_Absyn_Tqual
f1;void*f2;};static void*Cyc_Tcgenrep_get_second(struct _tuple8*pair){return(*pair).f2;}
struct _tuple9{struct _tagged_arr*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tagged_arr*Cyc_Tcgenrep_get_first(struct _tuple9*pair){return(*pair).f1;}static
unsigned char _tmp59[5]="list";static struct _tagged_arr Cyc_Tcgenrep_list_str={
_tmp59,_tmp59,_tmp59 + 5};static unsigned char _tmp5A[5]="List";static struct
_tagged_arr Cyc_Tcgenrep_List_str={_tmp5A,_tmp5A,_tmp5A + 5};struct _tuple10{struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple2_exp_cls(
struct Cyc_Position_Segment*loc,struct _tuple10*es){struct _tuple10 _tmp5C;struct Cyc_Absyn_Exp*
_tmp5D;struct Cyc_Absyn_Exp*_tmp5E;struct _tuple10*_tmp5B=es;_tmp5C=*_tmp5B;_LL10:
_tmp5E=_tmp5C.f1;goto _LLF;_LLF: _tmp5D=_tmp5C.f2;goto _LLE;_LLE: return Cyc_Absyn_tuple_exp(({
struct Cyc_Absyn_Exp*_tmp5F[2];_tmp5F[1]=_tmp5D;_tmp5F[0]=_tmp5E;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp5F,sizeof(struct Cyc_Absyn_Exp*),2));}),
loc);}struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*
f3;};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple3_exp_cls(struct Cyc_Position_Segment*
loc,struct _tuple11*es){struct _tuple11 _tmp61;struct Cyc_Absyn_Exp*_tmp62;struct Cyc_Absyn_Exp*
_tmp63;struct Cyc_Absyn_Exp*_tmp64;struct _tuple11*_tmp60=es;_tmp61=*_tmp60;_LL14:
_tmp64=_tmp61.f1;goto _LL13;_LL13: _tmp63=_tmp61.f2;goto _LL12;_LL12: _tmp62=_tmp61.f3;
goto _LL11;_LL11: return Cyc_Absyn_tuple_exp(({struct Cyc_Absyn_Exp*_tmp65[3];_tmp65[
2]=_tmp62;_tmp65[1]=_tmp63;_tmp65[0]=_tmp64;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp65,sizeof(struct Cyc_Absyn_Exp*),3));}),
loc);}static unsigned char _tmp66[5]="NULL";static struct _tagged_arr Cyc_Tcgenrep_null_str={
_tmp66,_tmp66,_tmp66 + 5};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_list_exp(struct
Cyc_List_List*l,struct Cyc_Position_Segment*loc){if(l == 0){return Cyc_Absyn_null_exp(
loc);}return Cyc_Tcgenrep_call_exp(Cyc_Tcgenrep_toplevel_name(& Cyc_Tcgenrep_list_str),
l,loc);}struct _tuple12{void*f1;struct Cyc_Position_Segment*f2;};static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_make_offsetof_exp(struct _tuple12*typeloc,int index){struct _tuple12
_tmp68;struct Cyc_Position_Segment*_tmp69;void*_tmp6A;struct _tuple12*_tmp67=
typeloc;_tmp68=*_tmp67;_LL17: _tmp6A=_tmp68.f1;goto _LL16;_LL16: _tmp69=_tmp68.f2;
goto _LL15;_LL15: return Cyc_Absyn_offsetof_exp(_tmp6A,(void*)({struct Cyc_Absyn_TupleIndex_struct*
_tmp6B=_cycalloc_atomic(sizeof(struct Cyc_Absyn_TupleIndex_struct));_tmp6B[0]=({
struct Cyc_Absyn_TupleIndex_struct _tmp6C;_tmp6C.tag=1;_tmp6C.f1=(unsigned int)
index;_tmp6C;});_tmp6B;}),_tmp69);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_get_and_cast_ri_exp(
struct Cyc_Position_Segment*loc,struct Cyc_Tcgenrep_RepInfo*info){return Cyc_Absyn_cast_exp(
Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str)),(
struct Cyc_Absyn_Exp*)_check_null(info->exp),loc);}static struct _tuple9*Cyc_Tcgenrep_gen_id_for_exp(
struct Cyc_Absyn_Exp*e){return({struct _tuple9*_tmp6D=_cycalloc(sizeof(struct
_tuple9));_tmp6D->f1=Cyc_Tcgenrep_new_gen_id(_tag_arr("tuple",sizeof(
unsigned char),6));_tmp6D->f2=e;_tmp6D;});}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl_cls(
struct _tuple12*env,struct _tuple9*name_exp){struct _tuple12 _tmp6F;struct Cyc_Position_Segment*
_tmp70;void*_tmp71;struct _tuple12*_tmp6E=env;_tmp6F=*_tmp6E;_LL1A: _tmp71=_tmp6F.f1;
goto _LL19;_LL19: _tmp70=_tmp6F.f2;goto _LL18;_LL18: {struct _tuple9 _tmp73;struct Cyc_Absyn_Exp*
_tmp74;struct _tagged_arr*_tmp75;struct _tuple9*_tmp72=name_exp;_tmp73=*_tmp72;
_LL1D: _tmp75=_tmp73.f1;goto _LL1C;_LL1C: _tmp74=_tmp73.f2;goto _LL1B;_LL1B: return
Cyc_Tcgenrep_gen_decl(_tmp75,_tmp71,(struct Cyc_Absyn_Exp*)_tmp74,_tmp70);}}
struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple13*Cyc_Tcgenrep_null_designator_exp(struct Cyc_Absyn_Exp*e){return({struct
_tuple13*_tmp76=_cycalloc(sizeof(struct _tuple13));_tmp76->f1=0;_tmp76->f2=e;
_tmp76;});}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_arr_init_exp(struct Cyc_List_List*
l,struct Cyc_Position_Segment*loc){struct Cyc_List_List*_tmp77=((struct Cyc_List_List*(*)(
struct _tuple13*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_null_designator_exp,l);return Cyc_Absyn_unresolvedmem_exp(0,_tmp77,
loc);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_address_exp_cls(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_address_exp(e,loc);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_unknownid_exp_cls(struct Cyc_Position_Segment*loc,struct _tuple5*e){
return Cyc_Absyn_unknownid_exp(e,loc);}static int Cyc_Tcgenrep_has_bitfield(struct
Cyc_Absyn_Structfield*sf){return sf->width != 0;}static int Cyc_Tcgenrep_add_bitfield_sizes(
int total,struct Cyc_Absyn_Structfield*sf){return(int)(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(sf->width))+ total);}static void*Cyc_Tcgenrep_select_structfield_type(
struct Cyc_Absyn_Structfield*sf){if(Cyc_Std_strcmp(*sf->name,_tag_arr("",sizeof(
unsigned char),1))== 0){({void*_tmp78[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): anonymous (padding) structfield not handled yet",
sizeof(unsigned char),55),_tag_arr(_tmp78,sizeof(void*),0));});}if(Cyc_Tcgenrep_has_bitfield(
sf)){({void*_tmp79[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen(): mixed struct bitfields and union bitfields not handled",sizeof(
unsigned char),62),_tag_arr(_tmp79,sizeof(void*),0));});}return(void*)sf->type;}
struct _tuple14{struct _tagged_arr*f1;void*f2;};static struct _tuple14*Cyc_Tcgenrep_select_structfield_nmtype(
struct Cyc_Absyn_Structfield*sf){if(Cyc_Std_strcmp(*sf->name,_tag_arr("",sizeof(
unsigned char),1))== 0){({void*_tmp7A[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): anonymous (padding) structfield not handled yet",
sizeof(unsigned char),55),_tag_arr(_tmp7A,sizeof(void*),0));});}if(Cyc_Tcgenrep_has_bitfield(
sf)){({void*_tmp7B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen(): mixed struct bitfields and union bitfields not handled",sizeof(
unsigned char),62),_tag_arr(_tmp7B,sizeof(void*),0));});}return({struct _tuple14*
_tmp7C=_cycalloc(sizeof(struct _tuple14));_tmp7C->f1=sf->name;_tmp7C->f2=(void*)
sf->type;_tmp7C;});}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_update_info(struct Cyc_Dict_Dict*
dict,void*type,struct Cyc_List_List*decls,struct Cyc_Absyn_Exp*exp,struct Cyc_List_List*
dependencies,struct Cyc_Core_Opt*fwd_decl){struct Cyc_Core_Opt*_tmp7D=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(dict,type);if(_tmp7D != 0){if(((
struct Cyc_Tcgenrep_RepInfo*)_tmp7D->v)->decls != 0){Cyc_Tcgenrep_print_typerep_dict(
dict);({void*_tmp7E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("Updating non-forward declaration",sizeof(unsigned char),33),_tag_arr(
_tmp7E,sizeof(void*),0));});}((struct Cyc_Tcgenrep_RepInfo*)_tmp7D->v)->decls=
decls;((struct Cyc_Tcgenrep_RepInfo*)_tmp7D->v)->exp=exp;((struct Cyc_Tcgenrep_RepInfo*)
_tmp7D->v)->dependencies=dependencies;return dict;}else{return((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(dict,
type,({struct Cyc_Tcgenrep_RepInfo*_tmp7F=_cycalloc(sizeof(struct Cyc_Tcgenrep_RepInfo));
_tmp7F->decls=decls;_tmp7F->exp=exp;_tmp7F->dependencies=dependencies;_tmp7F->fwd_decl=
fwd_decl;_tmp7F->emitted=0;_tmp7F->is_extern=0;_tmp7F;}));}}static struct Cyc_Dict_Dict*
Cyc_Tcgenrep_make_fwd_decl_info(struct Cyc_Dict_Dict*dict,void*type,struct Cyc_Absyn_Exp*
exp,struct Cyc_Core_Opt*fwd_decl,int is_extern){struct Cyc_Core_Opt*_tmp80=((struct
Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(dict,type);
if(_tmp80 != 0){({struct Cyc_Std_String_pa_struct _tmp82;_tmp82.tag=0;_tmp82.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(type);{void*_tmp81[1]={& _tmp82};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Repinfo for fwd declaration of %s already exists",
sizeof(unsigned char),49),_tag_arr(_tmp81,sizeof(void*),1));}});return dict;}
else{return((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(dict,type,({struct Cyc_Tcgenrep_RepInfo*_tmp83=_cycalloc(
sizeof(struct Cyc_Tcgenrep_RepInfo));_tmp83->decls=0;_tmp83->exp=exp;_tmp83->dependencies=
0;_tmp83->fwd_decl=fwd_decl;_tmp83->emitted=0;_tmp83->is_extern=is_extern;_tmp83;}));}}
static struct Cyc_Absyn_Tqual Cyc_Tcgenrep_tq_none={0,0,0};static struct _tuple8*Cyc_Tcgenrep_tqual_type(
struct Cyc_Absyn_Tqual*tq,void*type){return({struct _tuple8*_tmp84=_cycalloc(
sizeof(struct _tuple8));_tmp84->f1=*((struct Cyc_Absyn_Tqual*)_check_null(tq));
_tmp84->f2=type;_tmp84;});}static void*Cyc_Tcgenrep_tuple_typ(struct Cyc_List_List*
types){struct Cyc_List_List*_tmp85=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tqual*,void*),struct Cyc_Absyn_Tqual*env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Tcgenrep_tqual_type,(struct Cyc_Absyn_Tqual*)& Cyc_Tcgenrep_tq_none,
types);struct Cyc_Absyn_TupleType_struct*tuple_type_base=({struct Cyc_Absyn_TupleType_struct*
_tmp86=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_struct));_tmp86[0]=({struct Cyc_Absyn_TupleType_struct
_tmp87;_tmp87.tag=9;_tmp87.f1=_tmp85;_tmp87;});_tmp86;});void*tuple_type=(void*)
tuple_type_base;return tuple_type;}static struct _tuple13*Cyc_Tcgenrep_array_decls(
void*type,struct Cyc_List_List*exps,struct Cyc_Position_Segment*loc){struct Cyc_List_List*
_tmp88=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_gen_id_for_exp,exps);struct Cyc_List_List*
_tmp89=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(struct _tuple12*,
struct _tuple9*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_gen_decl_cls,({
struct _tuple12*_tmp96=_cycalloc(sizeof(struct _tuple12));_tmp96->f1=type;_tmp96->f2=
loc;_tmp96;}),_tmp88);struct Cyc_List_List*_tmp8A=((struct Cyc_List_List*(*)(
struct _tagged_arr*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_first,
_tmp88);struct Cyc_List_List*_tmp8B=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(
struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_toplevel_name,
_tmp8A);struct Cyc_List_List*_tmp8C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple5*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_unknownid_exp_cls,loc,_tmp8B);
struct Cyc_List_List*_tmp8D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Exp*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_address_exp_cls,loc,_tmp8C);
struct Cyc_Absyn_Exp*_tmp8E=Cyc_Tcgenrep_arr_init_exp(_tmp8D,loc);struct
_tagged_arr*_tmp8F=Cyc_Tcgenrep_new_gen_id(_tag_arr("arr",sizeof(unsigned char),
4));void*_tmp90=Cyc_Absyn_at_typ(type,(void*)2,Cyc_Tcgenrep_tq_none);void*_tmp91=
Cyc_Tcgenrep_array_typ(_tmp90,Cyc_Tcgenrep_tq_none,0);struct Cyc_Absyn_Decl*
_tmp92=Cyc_Tcgenrep_gen_decl(_tmp8F,_tmp91,(struct Cyc_Absyn_Exp*)_tmp8E,loc);
struct Cyc_Absyn_Exp*_tmp93=Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmp8F),loc);return({struct _tuple13*_tmp94=_cycalloc(sizeof(struct _tuple13));
_tmp94->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp89,({struct Cyc_Absyn_Decl*_tmp95[1];_tmp95[0]=_tmp92;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp95,
sizeof(struct Cyc_Absyn_Decl*),1));}));_tmp94->f2=_tmp93;_tmp94;});}static void*
Cyc_Tcgenrep_check_tunionfield_and_get_type(struct Cyc_Absyn_Tunionfield*tuf){if(
tuf->tvs != 0){({void*_tmp97[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield type variables not handled",
sizeof(unsigned char),46),_tag_arr(_tmp97,sizeof(void*),0));});}return(void*)({
struct Cyc_Absyn_TupleType_struct*_tmp98=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_struct));
_tmp98[0]=({struct Cyc_Absyn_TupleType_struct _tmp99;_tmp99.tag=9;_tmp99.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(({
struct _tuple8*_tmp9A[1];_tmp9A[0]=({struct _tuple8*_tmp9B=_cycalloc(sizeof(struct
_tuple8));_tmp9B->f1=Cyc_Tcgenrep_tq_none;_tmp9B->f2=Cyc_Absyn_uint_t;_tmp9B;});((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp9A,
sizeof(struct _tuple8*),1));}),tuf->typs);_tmp99;});_tmp98;});}static struct
_tuple5*Cyc_Tcgenrep_check_tunionfield_and_get_name(struct Cyc_Absyn_Tunionfield*
tuf){if(tuf->tvs != 0){({void*_tmp9C[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield type variables not handled",
sizeof(unsigned char),46),_tag_arr(_tmp9C,sizeof(void*),0));});}return tuf->name;}
struct _tuple15{struct _tuple5*f1;void*f2;};static struct _tuple15*Cyc_Tcgenrep_check_tunionfield_and_get_nmtype(
struct Cyc_Absyn_Tunionfield*tuf){if(tuf->tvs != 0){({void*_tmp9D[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield type variables not handled",
sizeof(unsigned char),46),_tag_arr(_tmp9D,sizeof(void*),0));});}return({struct
_tuple15*_tmp9E=_cycalloc(sizeof(struct _tuple15));_tmp9E->f1=tuf->name;_tmp9E->f2=(
void*)({struct Cyc_Absyn_TupleType_struct*_tmp9F=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_struct));
_tmp9F[0]=({struct Cyc_Absyn_TupleType_struct _tmpA0;_tmpA0.tag=9;_tmpA0.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(({
struct _tuple8*_tmpA1[1];_tmpA1[0]=({struct _tuple8*_tmpA2=_cycalloc(sizeof(struct
_tuple8));_tmpA2->f1=Cyc_Tcgenrep_tq_none;_tmpA2->f2=Cyc_Absyn_uint_t;_tmpA2;});((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpA1,
sizeof(struct _tuple8*),1));}),tuf->typs);_tmpA0;});_tmp9F;});_tmp9E;});}static
struct _tuple14*Cyc_Tcgenrep_check_xtunionfield_and_get_name_type(struct Cyc_Absyn_Tunionfield*
tuf){if(tuf->tvs != 0){({void*_tmpA3[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield type varaibles not handled",
sizeof(unsigned char),46),_tag_arr(_tmpA3,sizeof(void*),0));});}return({struct
_tuple14*_tmpA4=_cycalloc(sizeof(struct _tuple14));_tmpA4->f1=(*tuf->name).f2;
_tmpA4->f2=(void*)({struct Cyc_Absyn_TupleType_struct*_tmpA5=_cycalloc(sizeof(
struct Cyc_Absyn_TupleType_struct));_tmpA5[0]=({struct Cyc_Absyn_TupleType_struct
_tmpA6;_tmpA6.tag=9;_tmpA6.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple8*_tmpA7[1];_tmpA7[0]=({
struct _tuple8*_tmpA8=_cycalloc(sizeof(struct _tuple8));_tmpA8->f1=Cyc_Tcgenrep_tq_none;
_tmpA8->f2=Cyc_Absyn_star_typ(Cyc_Absyn_uchar_t,(void*)2,Cyc_Tcgenrep_tq_none);
_tmpA8;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmpA7,sizeof(struct _tuple8*),1));}),tuf->typs);_tmpA6;});_tmpA5;});_tmpA4;});}
static int Cyc_Tcgenrep_filter_empty_tunionfield(struct Cyc_Absyn_Tunionfield*tuf){
return tuf->typs != 0;}static int Cyc_Tcgenrep_filter_nonempty_tunionfield(struct Cyc_Absyn_Tunionfield*
tuf){return tuf->typs == 0;}static struct _tagged_arr*Cyc_Tcgenrep_get_tunionfield_name(
struct Cyc_Absyn_Tunionfield*tuf){return(*tuf->name).f2;}struct Cyc_Absyn_Structfield*
Cyc_Tcgenrep_substitute_structfield_type(struct Cyc_List_List*subst,struct Cyc_Absyn_Structfield*
sf){return({struct Cyc_Absyn_Structfield*_tmpA9=_cycalloc(sizeof(struct Cyc_Absyn_Structfield));
_tmpA9->name=sf->name;_tmpA9->tq=sf->tq;_tmpA9->type=(void*)Cyc_Tcutil_substitute(
subst,(void*)sf->type);_tmpA9->width=sf->width;_tmpA9->attributes=sf->attributes;
_tmpA9;});}struct _tuple8*Cyc_Tcgenrep_substitute_tqual_type(struct Cyc_List_List*
subst,struct _tuple8*pair){struct _tuple8 _tmpAB;void*_tmpAC;struct Cyc_Absyn_Tqual
_tmpAD;struct _tuple8*_tmpAA=pair;_tmpAB=*_tmpAA;_LL20: _tmpAD=_tmpAB.f1;goto _LL1F;
_LL1F: _tmpAC=_tmpAB.f2;goto _LL1E;_LL1E: return({struct _tuple8*_tmpAE=_cycalloc(
sizeof(struct _tuple8));_tmpAE->f1=_tmpAD;_tmpAE->f2=Cyc_Tcutil_substitute(subst,
_tmpAC);_tmpAE;});}struct Cyc_Absyn_Tunionfield*Cyc_Tcgenrep_substitute_tunionfield_type(
struct Cyc_List_List*subst,struct Cyc_Absyn_Tunionfield*tf){return({struct Cyc_Absyn_Tunionfield*
_tmpAF=_cycalloc(sizeof(struct Cyc_Absyn_Tunionfield));_tmpAF->name=tf->name;
_tmpAF->tvs=tf->tvs;_tmpAF->typs=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_List_List*,struct _tuple8*),struct Cyc_List_List*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_tqual_type,subst,tf->typs);_tmpAF->loc=
tf->loc;_tmpAF->sc=(void*)((void*)tf->sc);_tmpAF;});}void*Cyc_Tcgenrep_monomorphize_type(
void*type){void*_tmpB0=Cyc_Tcutil_compress(type);struct Cyc_Absyn_Structdecl**
_tmpB1;struct Cyc_List_List*_tmpB2;struct _tuple5*_tmpB3;struct Cyc_Absyn_Uniondecl**
_tmpB4;struct Cyc_List_List*_tmpB5;struct _tuple5*_tmpB6;struct Cyc_Absyn_TunionInfo
_tmpB7;void*_tmpB8;struct Cyc_List_List*_tmpB9;void*_tmpBA;struct Cyc_Absyn_Tuniondecl**
_tmpBB;struct Cyc_Absyn_Tuniondecl*_tmpBC;_LL22: if((unsigned int)_tmpB0 > 3?*((int*)
_tmpB0)== 10: 0){_LL2C: _tmpB3=((struct Cyc_Absyn_StructType_struct*)_tmpB0)->f1;
goto _LL2B;_LL2B: _tmpB2=((struct Cyc_Absyn_StructType_struct*)_tmpB0)->f2;goto
_LL2A;_LL2A: _tmpB1=((struct Cyc_Absyn_StructType_struct*)_tmpB0)->f3;goto _LL23;}
else{goto _LL24;}_LL24: if((unsigned int)_tmpB0 > 3?*((int*)_tmpB0)== 11: 0){_LL2F:
_tmpB6=((struct Cyc_Absyn_UnionType_struct*)_tmpB0)->f1;goto _LL2E;_LL2E: _tmpB5=((
struct Cyc_Absyn_UnionType_struct*)_tmpB0)->f2;goto _LL2D;_LL2D: _tmpB4=((struct Cyc_Absyn_UnionType_struct*)
_tmpB0)->f3;goto _LL25;}else{goto _LL26;}_LL26: if((unsigned int)_tmpB0 > 3?*((int*)
_tmpB0)== 2: 0){_LL30: _tmpB7=((struct Cyc_Absyn_TunionType_struct*)_tmpB0)->f1;
_LL33: _tmpBA=(void*)_tmpB7.tunion_info;if(*((int*)_tmpBA)== 1){_LL34: _tmpBB=((
struct Cyc_Absyn_KnownTunion_struct*)_tmpBA)->f1;_tmpBC=*_tmpBB;goto _LL32;}else{
goto _LL28;}_LL32: _tmpB9=_tmpB7.targs;goto _LL31;_LL31: _tmpB8=(void*)_tmpB7.rgn;
goto _LL27;}else{goto _LL28;}_LL28: goto _LL29;_LL23: if(_tmpB1 == 0){({void*_tmpBD[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("struct decl must exist at this point",
sizeof(unsigned char),37),_tag_arr(_tmpBD,sizeof(void*),0));});}{struct Cyc_Absyn_Structdecl*
_tmpBE=*((struct Cyc_Absyn_Structdecl**)_check_null(_tmpB1));struct Cyc_List_List*
_tmpBF=_tmpBE->tvs;if(Cyc_List_length(_tmpB2)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmpBF)){({struct Cyc_Std_Int_pa_struct _tmpC2;_tmpC2.tag=1;
_tmpC2.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpBF));{struct Cyc_Std_Int_pa_struct _tmpC1;_tmpC1.tag=1;_tmpC1.f1=(int)((
unsigned int)Cyc_List_length(_tmpB2));{void*_tmpC0[2]={& _tmpC1,& _tmpC2};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof(unsigned char),60),_tag_arr(_tmpC0,sizeof(void*),2));}}});}{struct Cyc_List_List*
_tmpC3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmpBF,_tmpB2);struct Cyc_Core_Opt*fields=0;if(_tmpBE->fields != 0){
fields=({struct Cyc_Core_Opt*_tmpC4=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpC4->v=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_structfield_type,_tmpC3,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpBE->fields))->v);_tmpC4;});}{struct Cyc_Absyn_Structdecl*
stdecl2=({struct Cyc_Absyn_Structdecl*_tmpCA=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));
_tmpCA->sc=(void*)((void*)_tmpBE->sc);_tmpCA->name=_tmpBE->name;_tmpCA->tvs=0;
_tmpCA->fields=fields;_tmpCA->attributes=_tmpBE->attributes;_tmpCA;});struct Cyc_Absyn_Structdecl**
_tmpC5=({struct Cyc_Absyn_Structdecl**_tmpC9=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl*));
_tmpC9[0]=stdecl2;_tmpC9;});struct Cyc_Absyn_StructType_struct*_tmpC6=({struct Cyc_Absyn_StructType_struct*
_tmpC7=_cycalloc(sizeof(struct Cyc_Absyn_StructType_struct));_tmpC7[0]=({struct
Cyc_Absyn_StructType_struct _tmpC8;_tmpC8.tag=10;_tmpC8.f1=_tmpB3;_tmpC8.f2=
_tmpB2;_tmpC8.f3=_tmpC5;_tmpC8;});_tmpC7;});return(void*)_tmpC6;}}}_LL25: if(
_tmpB4 == 0){({void*_tmpCB[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(_tag_arr("struct decl must exist at this point",sizeof(
unsigned char),37),_tag_arr(_tmpCB,sizeof(void*),0));});}{struct Cyc_Absyn_Uniondecl*
_tmpCC=*((struct Cyc_Absyn_Uniondecl**)_check_null(_tmpB4));struct Cyc_List_List*
_tmpCD=_tmpCC->tvs;if(Cyc_List_length(_tmpB5)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmpCD)){({struct Cyc_Std_Int_pa_struct _tmpD0;_tmpD0.tag=1;
_tmpD0.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpCD));{struct Cyc_Std_Int_pa_struct _tmpCF;_tmpCF.tag=1;_tmpCF.f1=(int)((
unsigned int)Cyc_List_length(_tmpB5));{void*_tmpCE[2]={& _tmpCF,& _tmpD0};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof(unsigned char),60),_tag_arr(_tmpCE,sizeof(void*),2));}}});}{struct Cyc_List_List*
_tmpD1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmpCD,_tmpB5);struct Cyc_Core_Opt*fields=0;if(_tmpCC->fields != 0){
fields=({struct Cyc_Core_Opt*_tmpD2=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpD2->v=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_structfield_type,_tmpD1,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpCC->fields))->v);_tmpD2;});}{struct Cyc_Absyn_Uniondecl*
udecl2=({struct Cyc_Absyn_Uniondecl*_tmpD8=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl));
_tmpD8->sc=(void*)((void*)_tmpCC->sc);_tmpD8->name=_tmpCC->name;_tmpD8->tvs=0;
_tmpD8->fields=fields;_tmpD8->attributes=_tmpCC->attributes;_tmpD8;});struct Cyc_Absyn_Uniondecl**
_tmpD3=({struct Cyc_Absyn_Uniondecl**_tmpD7=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl*));
_tmpD7[0]=udecl2;_tmpD7;});struct Cyc_Absyn_UnionType_struct*_tmpD4=({struct Cyc_Absyn_UnionType_struct*
_tmpD5=_cycalloc(sizeof(struct Cyc_Absyn_UnionType_struct));_tmpD5[0]=({struct Cyc_Absyn_UnionType_struct
_tmpD6;_tmpD6.tag=11;_tmpD6.f1=_tmpB6;_tmpD6.f2=_tmpB5;_tmpD6.f3=_tmpD3;_tmpD6;});
_tmpD5;});return(void*)_tmpD4;}}}_LL27: {struct Cyc_List_List*_tmpD9=_tmpBC->tvs;
if(Cyc_List_length(_tmpB9)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpD9)){({struct Cyc_Std_Int_pa_struct _tmpDC;_tmpDC.tag=1;_tmpDC.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD9));{struct
Cyc_Std_Int_pa_struct _tmpDB;_tmpDB.tag=1;_tmpDB.f1=(int)((unsigned int)Cyc_List_length(
_tmpB9));{void*_tmpDA[2]={& _tmpDB,& _tmpDC};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof(unsigned char),60),_tag_arr(_tmpDA,sizeof(void*),2));}}});}{struct Cyc_List_List*
_tmpDD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmpD9,_tmpB9);struct Cyc_Core_Opt*fields=0;if(_tmpBC->fields != 0){
fields=({struct Cyc_Core_Opt*_tmpDE=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpDE->v=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tunionfield*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_tunionfield_type,_tmpDD,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpBC->fields))->v);_tmpDE;});}{struct Cyc_Absyn_Tuniondecl*
_tmpDF=({struct Cyc_Absyn_Tuniondecl*_tmpE8=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));
_tmpE8->sc=(void*)((void*)_tmpBC->sc);_tmpE8->name=_tmpBC->name;_tmpE8->tvs=0;
_tmpE8->fields=fields;_tmpE8->is_xtunion=_tmpBC->is_xtunion;_tmpE8;});struct Cyc_Absyn_TunionType_struct*
_tmpE0=({struct Cyc_Absyn_TunionType_struct*_tmpE2=_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));
_tmpE2[0]=({struct Cyc_Absyn_TunionType_struct _tmpE3;_tmpE3.tag=2;_tmpE3.f1=({
struct Cyc_Absyn_TunionInfo _tmpE4;_tmpE4.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmpE5=_cycalloc(sizeof(struct Cyc_Absyn_KnownTunion_struct));_tmpE5[0]=({struct
Cyc_Absyn_KnownTunion_struct _tmpE6;_tmpE6.tag=1;_tmpE6.f1=({struct Cyc_Absyn_Tuniondecl**
_tmpE7=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl*));_tmpE7[0]=_tmpDF;_tmpE7;});
_tmpE6;});_tmpE5;}));_tmpE4.targs=_tmpB9;_tmpE4.rgn=(void*)_tmpB8;_tmpE4;});
_tmpE3;});_tmpE2;});struct Cyc_Position_Segment*_tmpE1=Cyc_Position_segment_of_abs(
0,0);return(void*)_tmpE0;}}}_LL29: return type;_LL21:;}struct _tagged_arr Cyc_Tcgenrep_make_type_cstring(
void*t){struct _tagged_arr s=Cyc_Std_strdup(Cyc_Absynpp_typ2cstring(t));{int i=0;
for(0;i < Cyc_Std_strlen((struct _tagged_arr)s);i ++){if(((unsigned char*)s.curr)[i]
== ' '){((unsigned char*)s.curr)[i]='_';}else{if(! isalnum((int)((unsigned char*)
s.curr)[i])?((unsigned char*)s.curr)[i]!= '_': 0){((unsigned char*)s.curr)[i]=(
unsigned char)('0' + ((unsigned char*)s.curr)[i]% 10);}}}}return(struct
_tagged_arr)Cyc_Std_strconcat((struct _tagged_arr)s,_tag_arr("_rep",sizeof(
unsigned char),5));}struct _tuple16{struct Cyc_Dict_Dict*f1;struct Cyc_Tcgenrep_RepInfo*
f2;};static struct _tuple16*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,void*type);struct _tuple17{
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};struct _tuple18{struct Cyc_Tcenv_Tenv*
f1;struct Cyc_Position_Segment*f2;};static struct _tuple17*Cyc_Tcgenrep_lookupRepsCls(
struct _tuple18*env,void*type,struct _tuple17*carry){struct _tuple16 _tmpEA;struct
Cyc_Tcgenrep_RepInfo*_tmpEB;struct Cyc_Dict_Dict*_tmpEC;struct _tuple16*_tmpE9=Cyc_Tcgenrep_lookupRep((*
env).f1,(*carry).f1,(*env).f2,type);_tmpEA=*_tmpE9;_LL37: _tmpEC=_tmpEA.f1;goto
_LL36;_LL36: _tmpEB=_tmpEA.f2;goto _LL35;_LL35: return({struct _tuple17*_tmpED=
_cycalloc(sizeof(struct _tuple17));_tmpED->f1=_tmpEC;_tmpED->f2=({struct Cyc_List_List*
_tmpEE=_cycalloc(sizeof(struct Cyc_List_List));_tmpEE->hd=_tmpEB;_tmpEE->tl=(*
carry).f2;_tmpEE;});_tmpED;});}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepTuple(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,
struct _tuple5*varname,void*sc,void*type,struct Cyc_List_List*types){struct Cyc_Absyn_Exp*
_tmpEF=Cyc_Absyn_sizeoftyp_exp(type,loc);struct _tuple18*_tmpF0=({struct _tuple18*
_tmp105=_cycalloc(sizeof(struct _tuple18));_tmp105->f1=te;_tmp105->f2=loc;_tmp105;});
struct _tuple17*_tmpF1=({struct _tuple17*_tmp104=_cycalloc(sizeof(struct _tuple17));
_tmp104->f1=dict;_tmp104->f2=0;_tmp104;});struct _tuple17 _tmpF3;struct Cyc_List_List*
_tmpF4;struct Cyc_Dict_Dict*_tmpF5;struct _tuple17*_tmpF2=((struct _tuple17*(*)(
struct _tuple17*(*f)(struct _tuple18*,void*,struct _tuple17*),struct _tuple18*,
struct Cyc_List_List*x,struct _tuple17*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmpF0,types,_tmpF1);_tmpF3=*_tmpF2;_LL3A: _tmpF5=_tmpF3.f1;goto _LL39;_LL39:
_tmpF4=_tmpF3.f2;goto _LL38;_LL38: dict=_tmpF5;{struct Cyc_List_List*_tmpF6=((
struct Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(Cyc_List_length(
types),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmpF7=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple12*,int),struct _tuple12*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({struct _tuple12*_tmp103=
_cycalloc(sizeof(struct _tuple12));_tmp103->f1=type;_tmp103->f2=loc;_tmp103;}),
_tmpF6);struct Cyc_List_List*_tmpF8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmpF4);struct Cyc_List_List*_tmpF9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple10*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmpF7,_tmpF8));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmp102[2];_tmp102[
1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp102[0]=Cyc_Absyn_uint_t;Cyc_List_list(_tag_arr(_tmp102,sizeof(void*),2));}));
struct _tuple13 _tmpFB;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_List_List*_tmpFD;
struct _tuple13*_tmpFA=Cyc_Tcgenrep_array_decls(tuple_type,_tmpF9,loc);_tmpFB=*
_tmpFA;_LL3D: _tmpFD=_tmpFB.f1;goto _LL3C;_LL3C: _tmpFC=_tmpFB.f2;goto _LL3B;_LL3B: {
struct Cyc_Absyn_Decl*_tmpFE=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str),
varname,({struct Cyc_Absyn_Exp*_tmp101[2];_tmp101[1]=_tmpFC;_tmp101[0]=_tmpEF;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp101,
sizeof(struct Cyc_Absyn_Exp*),2));}),sc,loc);struct Cyc_List_List*_tmpFF=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmpFD,({struct Cyc_Absyn_Decl*_tmp100[1];_tmp100[0]=_tmpFE;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp100,sizeof(struct Cyc_Absyn_Decl*),
1));}));return Cyc_Tcgenrep_update_info(dict,type,_tmpFF,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmpF4,0);}}}
static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepStruct(struct Cyc_Core_Opt*sname,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,
struct _tuple5*varname,void*sc,void*type,struct Cyc_List_List*nmtypes){struct Cyc_Absyn_Exp*
_tmp106=Cyc_Absyn_sizeoftyp_exp(type,loc);struct _tuple18*_tmp107=({struct
_tuple18*_tmp123=_cycalloc(sizeof(struct _tuple18));_tmp123->f1=te;_tmp123->f2=
loc;_tmp123;});struct _tuple17*_tmp108=({struct _tuple17*_tmp122=_cycalloc(sizeof(
struct _tuple17));_tmp122->f1=dict;_tmp122->f2=0;_tmp122;});struct Cyc_List_List*
_tmp10A;struct Cyc_List_List*_tmp10B;struct _tuple0 _tmp109=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(nmtypes);_LL40: _tmp10B=_tmp109.f1;goto
_LL3F;_LL3F: _tmp10A=_tmp109.f2;goto _LL3E;_LL3E: {struct _tuple17 _tmp10D;struct Cyc_List_List*
_tmp10E;struct Cyc_Dict_Dict*_tmp10F;struct _tuple17*_tmp10C=((struct _tuple17*(*)(
struct _tuple17*(*f)(struct _tuple18*,void*,struct _tuple17*),struct _tuple18*,
struct Cyc_List_List*x,struct _tuple17*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp107,_tmp10A,_tmp108);_tmp10D=*_tmp10C;_LL43: _tmp10F=_tmp10D.f1;goto _LL42;
_LL42: _tmp10E=_tmp10D.f2;goto _LL41;_LL41: dict=_tmp10F;{struct Cyc_List_List*
_tmp110=((struct Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(Cyc_List_length(
_tmp10A),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp111=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple12*,int),struct _tuple12*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({struct _tuple12*_tmp121=
_cycalloc(sizeof(struct _tuple12));_tmp121->f1=type;_tmp121->f2=loc;_tmp121;}),
_tmp110);struct Cyc_List_List*_tmp112=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tagged_arr*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,
_tmp10B);struct Cyc_List_List*_tmp113=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp10E);struct Cyc_List_List*_tmp114=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple11*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple3_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*
z))Cyc_List_zip3)(_tmp111,_tmp112,_tmp113));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({
void*_tmp120[3];_tmp120[2]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str));_tmp120[1]=Cyc_Absyn_const_string_typ((void*)2);
_tmp120[0]=Cyc_Absyn_uint_t;Cyc_List_list(_tag_arr(_tmp120,sizeof(void*),3));}));
struct _tuple13 _tmp116;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_List_List*_tmp118;
struct _tuple13*_tmp115=Cyc_Tcgenrep_array_decls(tuple_type,_tmp114,loc);_tmp116=*
_tmp115;_LL46: _tmp118=_tmp116.f1;goto _LL45;_LL45: _tmp117=_tmp116.f2;goto _LL44;
_LL44: {struct Cyc_Absyn_Exp*name;if(sname == 0){name=Cyc_Absyn_null_exp(loc);}
else{struct _tagged_arr*_tmp119=Cyc_Tcgenrep_new_gen_id(_tag_arr("name",sizeof(
unsigned char),5));struct Cyc_Absyn_Decl*_tmp11A=Cyc_Tcgenrep_gen_decl(_tmp119,
Cyc_Absyn_const_string_typ((void*)2),(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*((
struct _tuple5*)sname->v)).f2,loc),loc);_tmp118=({struct Cyc_List_List*_tmp11B=
_cycalloc(sizeof(struct Cyc_List_List));_tmp11B->hd=_tmp11A;_tmp11B->tl=_tmp118;
_tmp11B;});name=Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmp119),loc),loc);}{struct Cyc_Absyn_Decl*_tmp11C=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str),varname,({struct Cyc_Absyn_Exp*_tmp11F[3];_tmp11F[2]=
_tmp117;_tmp11F[1]=_tmp106;_tmp11F[0]=name;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp11F,sizeof(struct Cyc_Absyn_Exp*),3));}),
sc,loc);struct Cyc_List_List*_tmp11D=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp118,({struct Cyc_Absyn_Decl*
_tmp11E[1];_tmp11E[0]=_tmp11C;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp11E,sizeof(struct Cyc_Absyn_Decl*),1));}));return Cyc_Tcgenrep_update_info(
dict,type,_tmp11D,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp10E,0);}}}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepUnion(
struct Cyc_Core_Opt*uname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct
Cyc_Position_Segment*loc,struct _tuple5*varname,void*sc,void*type,struct Cyc_List_List*
nmtypes){struct _tuple18*_tmp124=({struct _tuple18*_tmp13D=_cycalloc(sizeof(struct
_tuple18));_tmp13D->f1=te;_tmp13D->f2=loc;_tmp13D;});struct _tuple17*_tmp125=({
struct _tuple17*_tmp13C=_cycalloc(sizeof(struct _tuple17));_tmp13C->f1=dict;
_tmp13C->f2=0;_tmp13C;});struct Cyc_List_List*_tmp127;struct Cyc_List_List*_tmp128;
struct _tuple0 _tmp126=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
nmtypes);_LL49: _tmp128=_tmp126.f1;goto _LL48;_LL48: _tmp127=_tmp126.f2;goto _LL47;
_LL47: {struct _tuple17 _tmp12A;struct Cyc_List_List*_tmp12B;struct Cyc_Dict_Dict*
_tmp12C;struct _tuple17*_tmp129=((struct _tuple17*(*)(struct _tuple17*(*f)(struct
_tuple18*,void*,struct _tuple17*),struct _tuple18*,struct Cyc_List_List*x,struct
_tuple17*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp124,
_tmp127,_tmp125);_tmp12A=*_tmp129;_LL4C: _tmp12C=_tmp12A.f1;goto _LL4B;_LL4B:
_tmp12B=_tmp12A.f2;goto _LL4A;_LL4A: dict=_tmp12C;{struct Cyc_List_List*_tmp12D=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tagged_arr*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_cnst_string_cls,loc,_tmp128);struct Cyc_List_List*_tmp12E=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,
loc,_tmp12B);struct Cyc_List_List*_tmp12F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple10*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp12D,_tmp12E));void*_tmp130=Cyc_Tcgenrep_tuple_typ(({void*_tmp13B[2];_tmp13B[
1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp13B[0]=Cyc_Absyn_const_string_typ((void*)2);Cyc_List_list(_tag_arr(_tmp13B,
sizeof(void*),2));}));struct _tuple13 _tmp132;struct Cyc_Absyn_Exp*_tmp133;struct
Cyc_List_List*_tmp134;struct _tuple13*_tmp131=Cyc_Tcgenrep_array_decls(_tmp130,
_tmp12F,loc);_tmp132=*_tmp131;_LL4F: _tmp134=_tmp132.f1;goto _LL4E;_LL4E: _tmp133=
_tmp132.f2;goto _LL4D;_LL4D: {struct Cyc_Absyn_Exp*name;if(uname == 0){name=Cyc_Absyn_null_exp(
loc);}else{struct _tagged_arr*_tmp135=Cyc_Tcgenrep_new_gen_id(_tag_arr("name",
sizeof(unsigned char),5));struct Cyc_Absyn_Decl*_tmp136=Cyc_Tcgenrep_gen_decl(
_tmp135,Cyc_Absyn_const_string_typ((void*)2),(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*((
struct _tuple5*)uname->v)).f2,loc),loc);_tmp134=({struct Cyc_List_List*_tmp137=
_cycalloc(sizeof(struct Cyc_List_List));_tmp137->hd=_tmp136;_tmp137->tl=_tmp134;
_tmp137;});name=Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmp135),loc),loc);}{struct Cyc_Absyn_Decl*_tmp138=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_union_str),varname,({struct Cyc_Absyn_Exp*_tmp13A[2];_tmp13A[1]=
_tmp133;_tmp13A[0]=name;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp13A,sizeof(struct Cyc_Absyn_Exp*),2));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp134,({struct Cyc_Absyn_Decl*_tmp139[1];_tmp139[0]=
_tmp138;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp139,sizeof(struct Cyc_Absyn_Decl*),1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp12B,0);}}}}}static struct Cyc_Dict_Dict*
Cyc_Tcgenrep_buildRepTunion(struct _tuple5*tname,struct Cyc_Tcenv_Tenv*te,struct
Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,struct _tuple5*varname,void*sc,
void*type,struct Cyc_List_List*tonames,struct Cyc_List_List*nmtypes){struct Cyc_List_List*
_tmp13E=((struct Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(tonames),(int(*)(int))Cyc_Core_identity);
struct Cyc_List_List*_tmp13F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp13E);struct Cyc_List_List*
_tmp140=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple5*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_cnst_qvar_string_cls,loc,tonames);struct Cyc_List_List*_tmp141=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp13F,_tmp140);struct Cyc_List_List*_tmp142=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple10*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp141);
void*_tmp143=Cyc_Tcgenrep_tuple_typ(({void*_tmp162[2];_tmp162[1]=Cyc_Absyn_const_string_typ((
void*)2);_tmp162[0]=Cyc_Absyn_uint_t;Cyc_List_list(_tag_arr(_tmp162,sizeof(void*),
2));}));struct _tuple13 _tmp145;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_List_List*
_tmp147;struct _tuple13*_tmp144=Cyc_Tcgenrep_array_decls(_tmp143,_tmp142,loc);
_tmp145=*_tmp144;_LL52: _tmp147=_tmp145.f1;goto _LL51;_LL51: _tmp146=_tmp145.f2;
goto _LL50;_LL50: {struct _tuple18*_tmp148=({struct _tuple18*_tmp161=_cycalloc(
sizeof(struct _tuple18));_tmp161->f1=te;_tmp161->f2=loc;_tmp161;});struct _tuple17*
_tmp149=({struct _tuple17*_tmp160=_cycalloc(sizeof(struct _tuple17));_tmp160->f1=
dict;_tmp160->f2=0;_tmp160;});struct Cyc_List_List*_tmp14B;struct Cyc_List_List*
_tmp14C;struct _tuple0 _tmp14A=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
nmtypes);_LL55: _tmp14C=_tmp14A.f1;goto _LL54;_LL54: _tmp14B=_tmp14A.f2;goto _LL53;
_LL53: {struct _tuple17 _tmp14E;struct Cyc_List_List*_tmp14F;struct Cyc_Dict_Dict*
_tmp150;struct _tuple17*_tmp14D=((struct _tuple17*(*)(struct _tuple17*(*f)(struct
_tuple18*,void*,struct _tuple17*),struct _tuple18*,struct Cyc_List_List*x,struct
_tuple17*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp148,
_tmp14B,_tmp149);_tmp14E=*_tmp14D;_LL58: _tmp150=_tmp14E.f1;goto _LL57;_LL57:
_tmp14F=_tmp14E.f2;goto _LL56;_LL56: dict=_tmp150;{struct Cyc_List_List*_tmp151=((
struct Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp14F),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*
_tmp152=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
int),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,
loc,_tmp151);struct Cyc_List_List*_tmp153=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple5*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
_tmp14C);struct Cyc_List_List*_tmp154=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp14F);struct Cyc_List_List*_tmp155=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y,struct Cyc_List_List*z))Cyc_List_zip3)(_tmp152,_tmp153,
_tmp154);struct Cyc_List_List*_tmp156=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple11*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple3_exp_cls,loc,_tmp155);
void*tuple_type2=Cyc_Tcgenrep_tuple_typ(({void*_tmp15F[3];_tmp15F[2]=Cyc_Tcgenrep_tunion_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));_tmp15F[1]=Cyc_Absyn_const_string_typ((
void*)2);_tmp15F[0]=Cyc_Absyn_uint_t;Cyc_List_list(_tag_arr(_tmp15F,sizeof(void*),
3));}));struct _tuple13 _tmp158;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_List_List*
_tmp15A;struct _tuple13*_tmp157=Cyc_Tcgenrep_array_decls(tuple_type2,_tmp156,loc);
_tmp158=*_tmp157;_LL5B: _tmp15A=_tmp158.f1;goto _LL5A;_LL5A: _tmp159=_tmp158.f2;
goto _LL59;_LL59: {struct Cyc_Absyn_Exp*_tmp15B=Cyc_Tcgenrep_cnst_string(*(*tname).f2,
loc);struct Cyc_Absyn_Decl*_tmp15C=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str),
varname,({struct Cyc_Absyn_Exp*_tmp15E[3];_tmp15E[2]=_tmp159;_tmp15E[1]=_tmp146;
_tmp15E[0]=_tmp15B;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp15E,sizeof(struct Cyc_Absyn_Exp*),3));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp147,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp15A,({struct Cyc_Absyn_Decl*
_tmp15D[1];_tmp15D[0]=_tmp15C;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp15D,sizeof(struct Cyc_Absyn_Decl*),1));}))),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp14F,0);
return dict;}}}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepXTunion(struct
_tuple5*xname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*
loc,struct _tuple5*varname,void*sc,void*type,struct Cyc_List_List*nametypes){
struct Cyc_List_List*_tmp164;struct Cyc_List_List*_tmp165;struct _tuple0 _tmp163=((
struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(nametypes);_LL5E: _tmp165=
_tmp163.f1;goto _LL5D;_LL5D: _tmp164=_tmp163.f2;goto _LL5C;_LL5C: {struct _tuple18*
_tmp166=({struct _tuple18*_tmp17B=_cycalloc(sizeof(struct _tuple18));_tmp17B->f1=
te;_tmp17B->f2=loc;_tmp17B;});struct _tuple17*_tmp167=({struct _tuple17*_tmp17A=
_cycalloc(sizeof(struct _tuple17));_tmp17A->f1=dict;_tmp17A->f2=0;_tmp17A;});
struct _tuple17 _tmp169;struct Cyc_List_List*_tmp16A;struct Cyc_Dict_Dict*_tmp16B;
struct _tuple17*_tmp168=((struct _tuple17*(*)(struct _tuple17*(*f)(struct _tuple18*,
void*,struct _tuple17*),struct _tuple18*,struct Cyc_List_List*x,struct _tuple17*
accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp166,_tmp164,_tmp167);
_tmp169=*_tmp168;_LL61: _tmp16B=_tmp169.f1;goto _LL60;_LL60: _tmp16A=_tmp169.f2;
goto _LL5F;_LL5F: dict=_tmp16B;{struct Cyc_List_List*_tmp16C=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct _tagged_arr*),struct
Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,
loc,_tmp165);struct Cyc_List_List*_tmp16D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp16A);struct Cyc_List_List*_tmp16E=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp16C,_tmp16D);struct Cyc_List_List*
_tmp16F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple10*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp16E);void*name_type=Cyc_Absyn_tagged_typ(Cyc_Absyn_uchar_t,(
void*)2,({struct Cyc_Absyn_Tqual _tmp179;_tmp179.q_const=1;_tmp179.q_volatile=0;
_tmp179.q_restrict=0;_tmp179;}));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*
_tmp178[2];_tmp178[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp178[0]=name_type;Cyc_List_list(_tag_arr(_tmp178,sizeof(void*),2));}));struct
_tuple13 _tmp171;struct Cyc_Absyn_Exp*_tmp172;struct Cyc_List_List*_tmp173;struct
_tuple13*_tmp170=Cyc_Tcgenrep_array_decls(tuple_type,_tmp16F,loc);_tmp171=*
_tmp170;_LL64: _tmp173=_tmp171.f1;goto _LL63;_LL63: _tmp172=_tmp171.f2;goto _LL62;
_LL62: {struct Cyc_Absyn_Exp*_tmp174=Cyc_Tcgenrep_cnst_string(*(*xname).f2,loc);
struct Cyc_Absyn_Decl*_tmp175=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_xtunion_str),
varname,({struct Cyc_Absyn_Exp*_tmp177[2];_tmp177[1]=_tmp172;_tmp177[0]=_tmp174;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp177,
sizeof(struct Cyc_Absyn_Exp*),2));}),sc,loc);return Cyc_Tcgenrep_update_info(dict,
type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp173,({struct Cyc_Absyn_Decl*_tmp176[1];_tmp176[0]=_tmp175;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp176,sizeof(struct Cyc_Absyn_Decl*),
1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp16A,0);return dict;}}}}static struct _tuple16*Cyc_Tcgenrep_lookupRep(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,
void*type){struct Cyc_Core_Opt*info=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
d,void*k))Cyc_Dict_lookup_opt)(dict,type);if(info != 0){if(((struct Cyc_Tcgenrep_RepInfo*)
info->v)->is_extern){dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,void*))
Cyc_Dict_delete)(dict,type);}else{struct _tuple16*_tmp17C=({struct _tuple16*
_tmp17D=_cycalloc(sizeof(struct _tuple16));_tmp17D->f1=dict;_tmp17D->f2=(struct
Cyc_Tcgenrep_RepInfo*)info->v;_tmp17D;});return _tmp17C;}}{void*_tmp17E=Cyc_Tcutil_compress(
Cyc_Tcgenrep_monomorphize_type(type));void*_tmp17F;void*_tmp180;int _tmp181;
struct Cyc_Absyn_PtrInfo _tmp182;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Tqual
_tmp184;void*_tmp185;struct Cyc_List_List*_tmp186;struct Cyc_Core_Opt*_tmp187;
struct Cyc_List_List*_tmp188;struct _tuple5*_tmp189;struct Cyc_Absyn_Structdecl**
_tmp18A;struct Cyc_List_List*_tmp18B;struct _tuple5*_tmp18C;struct Cyc_List_List*
_tmp18D;struct Cyc_Absyn_Tvar*_tmp18E;struct Cyc_Absyn_FnInfo _tmp18F;struct Cyc_Absyn_TunionInfo
_tmp190;void*_tmp191;struct Cyc_List_List*_tmp192;void*_tmp193;struct Cyc_Absyn_Tuniondecl**
_tmp194;struct Cyc_Absyn_Tuniondecl*_tmp195;struct Cyc_Absyn_TunionFieldInfo
_tmp196;struct Cyc_Absyn_Uniondecl**_tmp197;struct Cyc_List_List*_tmp198;struct
_tuple5*_tmp199;struct Cyc_List_List*_tmp19A;_LL66: if((unsigned int)_tmp17E > 3?*((
int*)_tmp17E)== 5: 0){_LL9B: _tmp180=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp17E)->f1;goto _LL9A;_LL9A: _tmp17F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp17E)->f2;goto _LL67;}else{goto _LL68;}_LL68: if((int)_tmp17E == 1){goto _LL69;}
else{goto _LL6A;}_LL6A: if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 6: 0){_LL9C:
_tmp181=((struct Cyc_Absyn_DoubleType_struct*)_tmp17E)->f1;goto _LL6B;}else{goto
_LL6C;}_LL6C: if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 4: 0){_LL9D: _tmp182=((
struct Cyc_Absyn_PointerType_struct*)_tmp17E)->f1;goto _LL6D;}else{goto _LL6E;}
_LL6E: if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 7: 0){_LLA0: _tmp185=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp17E)->f1;goto _LL9F;_LL9F: _tmp184=((struct
Cyc_Absyn_ArrayType_struct*)_tmp17E)->f2;goto _LL9E;_LL9E: _tmp183=((struct Cyc_Absyn_ArrayType_struct*)
_tmp17E)->f3;goto _LL6F;}else{goto _LL70;}_LL70: if((unsigned int)_tmp17E > 3?*((int*)
_tmp17E)== 9: 0){_LLA1: _tmp186=((struct Cyc_Absyn_TupleType_struct*)_tmp17E)->f1;
goto _LL71;}else{goto _LL72;}_LL72: if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 
18: 0){_LLA4: _tmp189=((struct Cyc_Absyn_TypedefType_struct*)_tmp17E)->f1;goto _LLA3;
_LLA3: _tmp188=((struct Cyc_Absyn_TypedefType_struct*)_tmp17E)->f2;goto _LLA2;_LLA2:
_tmp187=((struct Cyc_Absyn_TypedefType_struct*)_tmp17E)->f3;goto _LL73;}else{goto
_LL74;}_LL74: if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 10: 0){_LLA7: _tmp18C=((
struct Cyc_Absyn_StructType_struct*)_tmp17E)->f1;goto _LLA6;_LLA6: _tmp18B=((struct
Cyc_Absyn_StructType_struct*)_tmp17E)->f2;goto _LLA5;_LLA5: _tmp18A=((struct Cyc_Absyn_StructType_struct*)
_tmp17E)->f3;goto _LL75;}else{goto _LL76;}_LL76: if((unsigned int)_tmp17E > 3?*((int*)
_tmp17E)== 14: 0){_LLA8: _tmp18D=((struct Cyc_Absyn_AnonStructType_struct*)_tmp17E)->f1;
goto _LL77;}else{goto _LL78;}_LL78: if((int)_tmp17E == 0){goto _LL79;}else{goto _LL7A;}
_LL7A: if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 0: 0){goto _LL7B;}else{goto
_LL7C;}_LL7C: if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 1: 0){_LLA9: _tmp18E=((
struct Cyc_Absyn_VarType_struct*)_tmp17E)->f1;goto _LL7D;}else{goto _LL7E;}_LL7E:
if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 8: 0){_LLAA: _tmp18F=((struct Cyc_Absyn_FnType_struct*)
_tmp17E)->f1;goto _LL7F;}else{goto _LL80;}_LL80: if((unsigned int)_tmp17E > 3?*((int*)
_tmp17E)== 17: 0){goto _LL81;}else{goto _LL82;}_LL82: if((int)_tmp17E == 2){goto _LL83;}
else{goto _LL84;}_LL84: if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 19: 0){goto
_LL85;}else{goto _LL86;}_LL86: if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 20: 0){
goto _LL87;}else{goto _LL88;}_LL88: if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 
21: 0){goto _LL89;}else{goto _LL8A;}_LL8A: if((unsigned int)_tmp17E > 3?*((int*)
_tmp17E)== 2: 0){_LLAB: _tmp190=((struct Cyc_Absyn_TunionType_struct*)_tmp17E)->f1;
_LLAE: _tmp193=(void*)_tmp190.tunion_info;if(*((int*)_tmp193)== 1){_LLAF: _tmp194=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp193)->f1;_tmp195=*_tmp194;goto _LLAD;}
else{goto _LL8C;}_LLAD: _tmp192=_tmp190.targs;goto _LLAC;_LLAC: _tmp191=(void*)
_tmp190.rgn;goto _LL8B;}else{goto _LL8C;}_LL8C: if((unsigned int)_tmp17E > 3?*((int*)
_tmp17E)== 2: 0){goto _LL8D;}else{goto _LL8E;}_LL8E: if((unsigned int)_tmp17E > 3?*((
int*)_tmp17E)== 3: 0){_LLB0: _tmp196=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp17E)->f1;goto _LL8F;}else{goto _LL90;}_LL90: if((unsigned int)_tmp17E > 3?*((int*)
_tmp17E)== 11: 0){_LLB3: _tmp199=((struct Cyc_Absyn_UnionType_struct*)_tmp17E)->f1;
goto _LLB2;_LLB2: _tmp198=((struct Cyc_Absyn_UnionType_struct*)_tmp17E)->f2;goto
_LLB1;_LLB1: _tmp197=((struct Cyc_Absyn_UnionType_struct*)_tmp17E)->f3;goto _LL91;}
else{goto _LL92;}_LL92: if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 15: 0){_LLB4:
_tmp19A=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp17E)->f1;goto _LL93;}else{
goto _LL94;}_LL94: if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 12: 0){goto _LL95;}
else{goto _LL96;}_LL96: if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 16: 0){goto
_LL97;}else{goto _LL98;}_LL98: if((unsigned int)_tmp17E > 3?*((int*)_tmp17E)== 13: 0){
goto _LL99;}else{goto _LL65;}_LL67: {struct _tuple5*_tmp19B=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",sizeof(unsigned char),4)));struct Cyc_Absyn_Decl*
_tmp19C=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_int_str),_tmp19B,({struct Cyc_Absyn_Exp*
_tmp19E[2];_tmp19E[1]=Cyc_Tcgenrep_cnst_int(Cyc_Tcgenrep_size_of2int(_tmp17F),
loc);_tmp19E[0]=Cyc_Tcgenrep_cnst_int(_tmp180 == (void*)0? 1: 0,loc);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp19E,sizeof(struct Cyc_Absyn_Exp*),
2));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*
_tmp19D[1];_tmp19D[0]=_tmp19C;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp19D,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp19B,loc),loc),0,0);goto _LL65;}_LL69: dict=Cyc_Tcgenrep_update_info(
dict,type,0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_float_str),loc),0,0);goto _LL65;_LL6B: dict=Cyc_Tcgenrep_update_info(
dict,type,0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_double_str),loc),0,0);goto _LL65;_LL6D:{void*_tmp19F=Cyc_Absyn_conref_val(
_tmp182.bounds);struct Cyc_Absyn_Exp*_tmp1A0;_LLB6: if((unsigned int)_tmp19F > 1?*((
int*)_tmp19F)== 0: 0){_LLBA: _tmp1A0=((struct Cyc_Absyn_Upper_b_struct*)_tmp19F)->f1;
goto _LLB7;}else{goto _LLB8;}_LLB8: if((int)_tmp19F == 0){goto _LLB9;}else{goto _LLB5;}
_LLB7: {struct _tuple5*_tmp1A1=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(unsigned char),4)));struct Cyc_Core_Opt*_tmp1A2=({struct Cyc_Core_Opt*
_tmp1AB=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1AB->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1A1,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str)),0,(void*)3,loc);_tmp1AB;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1A1,loc),loc),_tmp1A2,0);{struct _tuple16 _tmp1A4;
struct Cyc_Tcgenrep_RepInfo*_tmp1A5;struct Cyc_Dict_Dict*_tmp1A6;struct _tuple16*
_tmp1A3=Cyc_Tcgenrep_lookupRep(te,dict,loc,(void*)_tmp182.elt_typ);_tmp1A4=*
_tmp1A3;_LLBD: _tmp1A6=_tmp1A4.f1;goto _LLBC;_LLBC: _tmp1A5=_tmp1A4.f2;goto _LLBB;
_LLBB: dict=_tmp1A6;{unsigned int bound=Cyc_Evexp_eval_const_uint_exp(_tmp1A0);
struct Cyc_Absyn_Decl*_tmp1A7=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str),
_tmp1A1,({struct Cyc_Absyn_Exp*_tmp1AA[2];_tmp1AA[1]=(struct Cyc_Absyn_Exp*)
_check_null(_tmp1A5->exp);_tmp1AA[0]=Cyc_Tcgenrep_cnst_int((int)bound,loc);((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1AA,
sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(
dict,type,({struct Cyc_Absyn_Decl*_tmp1A8[1];_tmp1A8[0]=_tmp1A7;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1A8,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1A1,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp1A9[1];_tmp1A9[0]=_tmp1A5;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1A9,
sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LLB5;}}}_LLB9: {struct _tuple5*
_tmp1AC=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",
sizeof(unsigned char),4)));struct Cyc_Core_Opt*_tmp1AD=({struct Cyc_Core_Opt*
_tmp1B6=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1B6->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1AC,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str)),0,(void*)3,loc);_tmp1B6;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1AC,loc),loc),_tmp1AD,0);{struct _tuple16 _tmp1AF;
struct Cyc_Tcgenrep_RepInfo*_tmp1B0;struct Cyc_Dict_Dict*_tmp1B1;struct _tuple16*
_tmp1AE=Cyc_Tcgenrep_lookupRep(te,dict,loc,(void*)_tmp182.elt_typ);_tmp1AF=*
_tmp1AE;_LLC0: _tmp1B1=_tmp1AF.f1;goto _LLBF;_LLBF: _tmp1B0=_tmp1AF.f2;goto _LLBE;
_LLBE: dict=_tmp1B1;{struct Cyc_Absyn_Decl*_tmp1B2=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_fatptr_str),_tmp1AC,({struct Cyc_Absyn_Exp*_tmp1B5[1];_tmp1B5[0]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp1B0->exp);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp1B5,sizeof(struct Cyc_Absyn_Exp*),1));}),(
void*)0,loc);dict=Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*
_tmp1B3[1];_tmp1B3[0]=_tmp1B2;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1B3,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1AC,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp1B4[1];
_tmp1B4[0]=_tmp1B0;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1B4,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LLB5;}}}
_LLB5:;}goto _LL65;_LL6F: if(_tmp183 == 0){({void*_tmp1B7[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcgenrep:At this point, array bounds must be constants",
sizeof(unsigned char),55),_tag_arr(_tmp1B7,sizeof(void*),0));});}{struct _tuple16
_tmp1B9;struct Cyc_Tcgenrep_RepInfo*_tmp1BA;struct Cyc_Dict_Dict*_tmp1BB;struct
_tuple16*_tmp1B8=Cyc_Tcgenrep_lookupRep(te,dict,loc,_tmp185);_tmp1B9=*_tmp1B8;
_LLC3: _tmp1BB=_tmp1B9.f1;goto _LLC2;_LLC2: _tmp1BA=_tmp1B9.f2;goto _LLC1;_LLC1: dict=
_tmp1BB;{unsigned int bound=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp183));struct _tuple5*_tmp1BC=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(unsigned char),4)));struct Cyc_Absyn_Decl*_tmp1BD=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str),_tmp1BC,({struct Cyc_Absyn_Exp*_tmp1C0[2];_tmp1C0[1]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp1BA->exp);_tmp1C0[0]=Cyc_Tcgenrep_cnst_int((
int)bound,loc);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1C0,sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(
dict,type,({struct Cyc_Absyn_Decl*_tmp1BE[1];_tmp1BE[0]=_tmp1BD;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1BE,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1BC,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp1BF[1];_tmp1BF[0]=_tmp1BA;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1BF,
sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL65;}}_LL71: {struct _tuple5*
_tmp1C1=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",
sizeof(unsigned char),4)));struct Cyc_Core_Opt*_tmp1C2=({struct Cyc_Core_Opt*
_tmp1C4=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1C4->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1C1,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str)),0,(void*)3,loc);_tmp1C4;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1C1,loc),loc),_tmp1C2,0);{struct Cyc_List_List*
_tmp1C3=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcgenrep_get_second,_tmp186);dict=Cyc_Tcgenrep_buildRepTuple(
te,dict,loc,_tmp1C1,(void*)0,type,_tmp1C3);goto _LL65;}}_LL73: if(_tmp187 == 0){({
void*_tmp1C5[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen(): typedef definition must be present by now",sizeof(unsigned char),
49),_tag_arr(_tmp1C5,sizeof(void*),0));});}{struct _tuple16 _tmp1C7;struct Cyc_Tcgenrep_RepInfo*
_tmp1C8;struct Cyc_Dict_Dict*_tmp1C9;struct _tuple16*_tmp1C6=Cyc_Tcgenrep_lookupRep(
te,dict,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp187))->v);_tmp1C7=*
_tmp1C6;_LLC6: _tmp1C9=_tmp1C7.f1;goto _LLC5;_LLC5: _tmp1C8=_tmp1C7.f2;goto _LLC4;
_LLC4: dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(_tmp1C9,type,_tmp1C8);goto _LL65;}_LL75: if(_tmp18A == 0){({
void*_tmp1CA[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen(): struct declaration must be present by now",sizeof(unsigned char),
49),_tag_arr(_tmp1CA,sizeof(void*),0));});}{struct Cyc_Absyn_Structdecl*_tmp1CB=*((
struct Cyc_Absyn_Structdecl**)_check_null(_tmp18A));if(_tmp1CB->fields == 0){
struct _tuple5*_tmp1CC=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp1CF=
_cycalloc(sizeof(struct _tagged_arr));_tmp1CF[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp1CF;}));struct Cyc_Core_Opt*_tmp1CD=({struct Cyc_Core_Opt*_tmp1CE=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1CE->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1CC,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_struct_str)),0,(void*)3,loc);_tmp1CE;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1CC,loc),loc),0,1);goto _LL65;}if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1CB->fields))->v != 0?((int(*)(int(*pred)(
struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1CB->fields))->v): 0){
int _tmp1D0=((int(*)(int(*f)(int,struct Cyc_Absyn_Structfield*),int accum,struct Cyc_List_List*
x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,0,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1CB->fields))->v);int _tmp1D1=_tmp1D0 / 8 + (
_tmp1D0 % 8 == 0? 0: 1);struct Cyc_List_List*chars=0;{int i=0;for(0;i < _tmp1D1;i ++){
chars=({struct Cyc_List_List*_tmp1D2=_cycalloc(sizeof(struct Cyc_List_List));
_tmp1D2->hd=({struct _tuple8*_tmp1D3=_cycalloc(sizeof(struct _tuple8));_tmp1D3->f1=
Cyc_Tcgenrep_tq_none;_tmp1D3->f2=Cyc_Absyn_uchar_t;_tmp1D3;});_tmp1D2->tl=chars;
_tmp1D2;});}}{void*base_type=(void*)({struct Cyc_Absyn_TupleType_struct*_tmp1D8=
_cycalloc(sizeof(struct Cyc_Absyn_TupleType_struct));_tmp1D8[0]=({struct Cyc_Absyn_TupleType_struct
_tmp1D9;_tmp1D9.tag=9;_tmp1D9.f1=chars;_tmp1D9;});_tmp1D8;});struct _tuple16
_tmp1D5;struct Cyc_Tcgenrep_RepInfo*_tmp1D6;struct Cyc_Dict_Dict*_tmp1D7;struct
_tuple16*_tmp1D4=Cyc_Tcgenrep_lookupRep(te,dict,loc,base_type);_tmp1D5=*_tmp1D4;
_LLC9: _tmp1D7=_tmp1D5.f1;goto _LLC8;_LLC8: _tmp1D6=_tmp1D5.f2;goto _LLC7;_LLC7: dict=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(_tmp1D7,type,_tmp1D6);}}else{struct _tuple5*_tmp1DA=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp1DE=_cycalloc(sizeof(struct _tagged_arr));_tmp1DE[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp1DE;}));struct Cyc_Core_Opt*_tmp1DB=({struct Cyc_Core_Opt*_tmp1DD=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1DD->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1DA,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_struct_str)),0,(void*)3,loc);_tmp1DD;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1DA,loc),loc),_tmp1DB,0);{struct Cyc_List_List*
_tmp1DC=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct Cyc_Absyn_Structfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_nmtype,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1CB->fields))->v);
dict=Cyc_Tcgenrep_buildRepStruct(_tmp1CB->name,te,dict,loc,_tmp1DA,(void*)2,type,
_tmp1DC);}}goto _LL65;}_LL77: if(_tmp18D != 0?((int(*)(int(*pred)(struct Cyc_Absyn_Structfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,_tmp18D): 0){
int _tmp1DF=((int(*)(int(*f)(int,struct Cyc_Absyn_Structfield*),int accum,struct Cyc_List_List*
x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,0,_tmp18D);int _tmp1E0=
_tmp1DF / 8 + (_tmp1DF % 8 == 0? 0: 1);void*base_type=(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp1E5=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp1E5[0]=({struct
Cyc_Absyn_ArrayType_struct _tmp1E6;_tmp1E6.tag=7;_tmp1E6.f1=(void*)Cyc_Absyn_uchar_t;
_tmp1E6.f2=Cyc_Tcgenrep_tq_none;_tmp1E6.f3=(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_int(
_tmp1E0,loc);_tmp1E6;});_tmp1E5;});struct _tuple16 _tmp1E2;struct Cyc_Tcgenrep_RepInfo*
_tmp1E3;struct Cyc_Dict_Dict*_tmp1E4;struct _tuple16*_tmp1E1=Cyc_Tcgenrep_lookupRep(
te,dict,loc,base_type);_tmp1E2=*_tmp1E1;_LLCC: _tmp1E4=_tmp1E2.f1;goto _LLCB;_LLCB:
_tmp1E3=_tmp1E2.f2;goto _LLCA;_LLCA: dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(_tmp1E4,type,_tmp1E3);}
else{struct Cyc_List_List*_tmp1E7=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(
struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_nmtype,
_tmp18D);struct _tuple5*_tmp1E8=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(unsigned char),4)));dict=Cyc_Tcgenrep_buildRepStruct(0,te,
dict,loc,_tmp1E8,(void*)0,type,_tmp1E7);}goto _LL65;_LL79:({void*_tmp1E9[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("found void in gen() expression",sizeof(
unsigned char),31),_tag_arr(_tmp1E9,sizeof(void*),0));});goto _LL65;_LL7B:({void*
_tmp1EA[0]={};Cyc_Tcutil_terr(loc,_tag_arr("found evar in gen() expression",
sizeof(unsigned char),31),_tag_arr(_tmp1EA,sizeof(void*),0));});goto _LL65;_LL7D:({
void*_tmp1EB[0]={};Cyc_Tcutil_terr(loc,_tag_arr("found tyvar in gen() expression",
sizeof(unsigned char),32),_tag_arr(_tmp1EB,sizeof(void*),0));});goto _LL65;({void*
_tmp1EC[0]={};Cyc_Tcutil_terr(loc,_tag_arr("found variable type in gen() expression",
sizeof(unsigned char),40),_tag_arr(_tmp1EC,sizeof(void*),0));});goto _LL65;_LL7F:({
void*_tmp1ED[0]={};Cyc_Tcutil_terr(loc,_tag_arr("found function type in gen() expression",
sizeof(unsigned char),40),_tag_arr(_tmp1ED,sizeof(void*),0));});goto _LL65;_LL81:
goto _LL83;_LL83: goto _LL85;_LL85: goto _LL87;_LL87: goto _LL89;_LL89:({void*_tmp1EE[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("gen(): unhandled region, handle or effect type",
sizeof(unsigned char),47),_tag_arr(_tmp1EE,sizeof(void*),0));});goto _LL65;_LL8B:
if(_tmp195->tvs != 0){({void*_tmp1EF[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunion type variables not handled yet",
sizeof(unsigned char),45),_tag_arr(_tmp1EF,sizeof(void*),0));});}if(_tmp195->fields
== 0){struct _tuple5*_tmp1F0=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*
_tmp1F3=_cycalloc(sizeof(struct _tagged_arr));_tmp1F3[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp1F3;}));struct Cyc_Core_Opt*_tmp1F1=({struct Cyc_Core_Opt*_tmp1F2=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1F2->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1F0,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str)),0,(void*)3,loc);_tmp1F2;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1F0,loc),loc),0,1);goto _LL65;}if(! _tmp195->is_xtunion){
struct _tuple5*_tmp1F4=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp1FC=
_cycalloc(sizeof(struct _tagged_arr));_tmp1FC[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp1FC;}));struct Cyc_Core_Opt*_tmp1F5=({struct Cyc_Core_Opt*_tmp1FB=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1FB->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1F4,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str)),0,(void*)3,loc);_tmp1FB;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1F4,loc),loc),_tmp1F5,0);{struct Cyc_List_List*
_tmp1F6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp195->fields))->v;
struct Cyc_List_List*_tmp1F7=((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_tunionfield,
_tmp1F6);struct Cyc_List_List*_tmp1F8=((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_Tcgenrep_filter_nonempty_tunionfield,
_tmp1F6);struct Cyc_List_List*_tmp1F9=((struct Cyc_List_List*(*)(struct _tuple15*(*
f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_tunionfield_and_get_nmtype,
_tmp1F7);struct Cyc_List_List*_tmp1FA=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(
struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_tunionfield_and_get_name,
_tmp1F8);dict=Cyc_Tcgenrep_buildRepTunion(_tmp195->name,te,dict,loc,_tmp1F4,(
void*)2,type,_tmp1FA,_tmp1F9);}}else{struct _tuple5*_tmp1FD=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp203=_cycalloc(sizeof(struct _tagged_arr));_tmp203[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp203;}));struct Cyc_Core_Opt*_tmp1FE=({struct Cyc_Core_Opt*_tmp202=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp202->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1FD,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_xtunion_str)),0,(void*)3,loc);_tmp202;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1FD,loc),loc),_tmp1FE,0);{struct Cyc_List_List*
_tmp1FF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp195->fields))->v;
struct Cyc_List_List*_tmp200=((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_tunionfield,
_tmp1FF);struct Cyc_List_List*_tmp201=((struct Cyc_List_List*(*)(struct _tuple14*(*
f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_xtunionfield_and_get_name_type,
_tmp200);dict=Cyc_Tcgenrep_buildRepXTunion(_tmp195->name,te,dict,loc,_tmp1FD,(
void*)2,type,_tmp201);}}goto _LL65;_LL8D:({void*_tmp204[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunion must be resolved by now",
sizeof(unsigned char),38),_tag_arr(_tmp204,sizeof(void*),0));});goto _LL65;_LL8F:
if(_tmp196.targs != 0){({void*_tmp205[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield type parameters not handled yet",
sizeof(unsigned char),51),_tag_arr(_tmp205,sizeof(void*),0));});}{void*_tmp206=(
void*)_tmp196.field_info;struct Cyc_Absyn_Tunionfield*_tmp207;struct Cyc_Absyn_Tuniondecl*
_tmp208;_LLCE: if(*((int*)_tmp206)== 1){_LLD3: _tmp208=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp206)->f1;goto _LLD2;_LLD2: _tmp207=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp206)->f2;goto _LLCF;}else{goto _LLD0;}_LLD0: goto _LLD1;_LLCF: if(_tmp207->tvs != 
0){({void*_tmp209[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen(): tunionfield type parameters not handled yet",sizeof(
unsigned char),51),_tag_arr(_tmp209,sizeof(void*),0));});}{struct Cyc_List_List*
_tmp20A=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcgenrep_get_second,_tmp207->typs);struct Cyc_List_List*
_tmp20B=({struct Cyc_List_List*_tmp20D=_cycalloc(sizeof(struct Cyc_List_List));
_tmp20D->hd=(void*)Cyc_Absyn_uint_t;_tmp20D->tl=_tmp20A;_tmp20D;});struct _tuple5*
_tmp20C=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",
sizeof(unsigned char),4)));dict=Cyc_Tcgenrep_buildRepTuple(te,dict,loc,_tmp20C,(
void*)0,type,_tmp20B);goto _LLCD;}_LLD1:({void*_tmp20E[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield must be known at this point",
sizeof(unsigned char),47),_tag_arr(_tmp20E,sizeof(void*),0));});goto _LLCD;_LLCD:;}
goto _LL65;_LL91: if(_tmp197 == 0){({void*_tmp20F[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): union declaration must be present by now",
sizeof(unsigned char),48),_tag_arr(_tmp20F,sizeof(void*),0));});}{struct Cyc_Absyn_Uniondecl*
_tmp210=*((struct Cyc_Absyn_Uniondecl**)_check_null(_tmp197));if(_tmp210->tvs != 0){({
void*_tmp211[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen: unions with parameters not yet supported",sizeof(unsigned char),
46),_tag_arr(_tmp211,sizeof(void*),0));});}if(_tmp210->fields == 0){struct _tuple5*
_tmp212=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp215=_cycalloc(sizeof(
struct _tagged_arr));_tmp215[0]=Cyc_Tcgenrep_make_type_cstring(type);_tmp215;}));
struct Cyc_Core_Opt*_tmp213=({struct Cyc_Core_Opt*_tmp214=_cycalloc(sizeof(struct
Cyc_Core_Opt));_tmp214->v=Cyc_Tcgenrep_gen_vardecl(_tmp212,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_union_str)),0,(void*)3,loc);_tmp214;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp212,loc),loc),0,1);goto _LL65;}{struct Cyc_List_List*_tmp216=((struct Cyc_List_List*(*)(
struct _tuple14*(*f)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp210->fields))->v);struct _tuple5*_tmp217=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp218=_cycalloc(sizeof(struct _tagged_arr));_tmp218[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp218;}));dict=Cyc_Tcgenrep_buildRepUnion(_tmp210->name,te,dict,loc,
_tmp217,(void*)2,type,_tmp216);goto _LL65;}}_LL93: {struct Cyc_List_List*_tmp219=((
struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct Cyc_Absyn_Structfield*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_nmtype,_tmp19A);
struct _tuple5*_tmp21A=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(unsigned char),4)));dict=Cyc_Tcgenrep_buildRepUnion(0,te,
dict,loc,_tmp21A,(void*)0,type,_tmp219);goto _LL65;}_LL95: goto _LL97;_LL97: {
struct _tuple5*_tmp21B=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(unsigned char),4)));struct Cyc_Absyn_Decl*_tmp21C=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_int_str),_tmp21B,({struct Cyc_Absyn_Exp*_tmp21E[2];_tmp21E[1]=Cyc_Tcgenrep_cnst_int(
32,loc);_tmp21E[0]=Cyc_Tcgenrep_cnst_int(0,loc);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp21E,sizeof(struct Cyc_Absyn_Exp*),2));}),(
void*)0,loc);dict=Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*
_tmp21D[1];_tmp21D[0]=_tmp21C;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp21D,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp21B,loc),loc),0,0);goto _LL65;}_LL99:({void*_tmp21F[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen() for sizeof_t<-> not yet supported",
sizeof(unsigned char),40),_tag_arr(_tmp21F,sizeof(void*),0));});goto _LL65;_LL65:;}
return({struct _tuple16*_tmp220=_cycalloc(sizeof(struct _tuple16));_tmp220->f1=
dict;_tmp220->f2=((struct Cyc_Tcgenrep_RepInfo*(*)(struct Cyc_Dict_Dict*d,void*k))
Cyc_Dict_lookup)(dict,type);_tmp220;});}static int Cyc_Tcgenrep_not_emitted_filter(
struct Cyc_Tcgenrep_RepInfo*ri){return ri->emitted == 0;}static void Cyc_Tcgenrep_mark_emitted(
struct Cyc_Tcgenrep_RepInfo*ri){ri->emitted=1;}struct _tuple19{struct Cyc_Dict_Dict*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple19 Cyc_Tcgenrep_tcGenrep(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
void*type,struct Cyc_Dict_Dict*dict){Cyc_Tcutil_check_valid_toplevel_type(loc,te,
type);Cyc_Tcutil_check_valid_toplevel_type(loc,te,type);{struct _tuple16 _tmp222;
struct Cyc_Tcgenrep_RepInfo*_tmp223;struct Cyc_Dict_Dict*_tmp224;struct _tuple16*
_tmp221=Cyc_Tcgenrep_lookupRep(te,dict,loc,type);_tmp222=*_tmp221;_LLD6: _tmp224=
_tmp222.f1;goto _LLD5;_LLD5: _tmp223=_tmp222.f2;goto _LLD4;_LLD4: {struct Cyc_List_List*
_tmp225=Cyc_Tcgenrep_dfs(_tmp223);return({struct _tuple19 _tmp226;_tmp226.f1=
_tmp224;_tmp226.f2=_tmp225;_tmp226.f3=(struct Cyc_Absyn_Exp*)_check_null(_tmp223->exp);
_tmp226;});}}}
