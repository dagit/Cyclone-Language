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
extern int isalnum(int);extern int isalpha(int);extern int isascii(int);extern int
iscntrl(int);extern int isdigit(int);extern int isgraph(int);extern int islower(int);
extern int isprint(int);extern int ispunct(int);extern int isspace(int);extern int
isupper(int);extern int isxdigit(int);extern int toascii(int);extern int tolower(int);
extern int toupper(int);extern int _tolower(int);extern int _toupper(int);struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);extern int Cyc_List_length(struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern
unsigned char Cyc_List_List_mismatch[18];extern void*Cyc_List_fold_left(void*(*f)(
void*,void*),void*accum,struct Cyc_List_List*x);extern void*Cyc_List_fold_right_c(
void*(*f)(void*,void*,void*),void*,struct Cyc_List_List*x,void*accum);extern
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x);extern
unsigned char Cyc_List_Nth[8];extern int Cyc_List_forall(int(*pred)(void*),struct
Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*
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
int,int);static const int Cyc_Position_Lex=0;static const int Cyc_Position_Parse=1;
static const int Cyc_Position_Elab=2;struct Cyc_Position_Error{struct _tagged_arr
source;struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
unsigned char Cyc_Position_Nocontext[14];static const int Cyc_Typerep_Var=0;struct
Cyc_Typerep_Var_struct{int tag;struct _tagged_arr*f1;};static const int Cyc_Typerep_Int=
1;struct Cyc_Typerep_Int_struct{int tag;unsigned int f1;};static const int Cyc_Typerep_Float=
0;static const int Cyc_Typerep_Double=1;static const int Cyc_Typerep_ThinPtr=2;struct
Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*f2;};static const int Cyc_Typerep_FatPtr=
3;struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};static const int Cyc_Typerep_Tuple=
4;struct _tuple1{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;
unsigned int f1;struct _tagged_arr f2;};static const int Cyc_Typerep_TUnion=5;struct
Cyc_Typerep_TUnion_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Typerep_XTUnion=
6;struct _tuple2{struct _tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Typerep_Union=7;struct Cyc_Typerep_Union_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Typerep_Forall=8;struct Cyc_Typerep_Forall_struct{
int tag;struct _tagged_arr f1;void**f2;};static const int Cyc_Typerep_App=9;struct Cyc_Typerep_App_struct{
int tag;void*f1;struct _tagged_arr f2;};unsigned int Cyc_Typerep_size_type(void*rep);
struct Cyc_Std_tm{int tm_sec;int tm_min;int tm_hour;int tm_mday;int tm_mon;int tm_year;
int tm_wday;int tm_yday;int tm_isdst;};static const int Cyc_Absyn_Loc_n=0;static const
int Cyc_Absyn_Rel_n=0;struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;
};static const int Cyc_Absyn_Abs_n=1;struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple3{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;static
const int Cyc_Absyn_Static=0;static const int Cyc_Absyn_Abstract=1;static const int Cyc_Absyn_Public=
2;static const int Cyc_Absyn_Extern=3;static const int Cyc_Absyn_ExternC=4;struct Cyc_Absyn_Tqual{
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
_tuple3*name;int is_xtunion;};static const int Cyc_Absyn_UnknownTunion=0;struct Cyc_Absyn_UnknownTunion_struct{
int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};static const int Cyc_Absyn_KnownTunion=
1;struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};
struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*targs;void*rgn;}
;struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple3*tunion_name;struct _tuple3*
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
int tag;struct _tuple3*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Structdecl**f3;};
static const int Cyc_Absyn_UnionType=11;struct Cyc_Absyn_UnionType_struct{int tag;
struct _tuple3*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Uniondecl**f3;};static
const int Cyc_Absyn_EnumType=12;struct Cyc_Absyn_EnumType_struct{int tag;struct
_tuple3*f1;struct Cyc_Absyn_Enumdecl*f2;};static const int Cyc_Absyn_SizeofType=13;
struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};static const int Cyc_Absyn_AnonStructType=
14;struct Cyc_Absyn_AnonStructType_struct{int tag;struct Cyc_List_List*f1;};static
const int Cyc_Absyn_AnonUnionType=15;struct Cyc_Absyn_AnonUnionType_struct{int tag;
struct Cyc_List_List*f1;};static const int Cyc_Absyn_AnonEnumType=16;struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_RgnHandleType=17;struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};static const int Cyc_Absyn_TypedefType=
18;struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple3*f1;struct Cyc_List_List*
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
int Cyc_Absyn_Var_e=1;struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple3*f1;void*
f2;};static const int Cyc_Absyn_UnknownId_e=2;struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple3*f1;};static const int Cyc_Absyn_Primop_e=3;struct Cyc_Absyn_Primop_e_struct{
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
struct Cyc_List_List*f1;};static const int Cyc_Absyn_CompoundLit_e=25;struct _tuple4{
struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{
int tag;struct _tuple4*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Array_e=
26;struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};static const int
Cyc_Absyn_Comprehension_e=27;struct Cyc_Absyn_Comprehension_e_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
static const int Cyc_Absyn_Struct_e=28;struct Cyc_Absyn_Struct_e_struct{int tag;
struct _tuple3*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl*
f4;};static const int Cyc_Absyn_AnonStruct_e=29;struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Tunion_e=30;
struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Tuniondecl*f4;struct Cyc_Absyn_Tunionfield*
f5;};static const int Cyc_Absyn_Enum_e=31;struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple3*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
static const int Cyc_Absyn_AnonEnum_e=32;struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple3*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static const int Cyc_Absyn_Malloc_e=
33;struct Cyc_Absyn_Malloc_e_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};static
const int Cyc_Absyn_UnresolvedMem_e=34;struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_StmtExp_e=
35;struct Cyc_Absyn_StmtExp_e_struct{int tag;struct Cyc_Absyn_Stmt*f1;};static const
int Cyc_Absyn_Codegen_e=36;struct Cyc_Absyn_Codegen_e_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};static const int Cyc_Absyn_Fill_e=37;struct Cyc_Absyn_Fill_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;
struct Cyc_Position_Segment*loc;void*annot;};struct _tuple5{struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;
struct _tuple5 condition;struct _tuple5 delta;struct Cyc_Absyn_Stmt*body;};static
const int Cyc_Absyn_Skip_s=0;static const int Cyc_Absyn_Exp_s=0;struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_Seq_s=1;struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_Return_s=
2;struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};static const int
Cyc_Absyn_IfThenElse_s=3;struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};static const int Cyc_Absyn_While_s=
4;struct Cyc_Absyn_While_s_struct{int tag;struct _tuple5 f1;struct Cyc_Absyn_Stmt*f2;
};static const int Cyc_Absyn_Break_s=5;struct Cyc_Absyn_Break_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};static const int Cyc_Absyn_Continue_s=6;struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};static const int Cyc_Absyn_Goto_s=7;struct Cyc_Absyn_Goto_s_struct{
int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_For_s=
8;struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple5 f2;
struct _tuple5 f3;struct Cyc_Absyn_Stmt*f4;};static const int Cyc_Absyn_Switch_s=9;
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
struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple5 f2;};
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
int tag;struct _tuple3*f1;};static const int Cyc_Absyn_UnknownCall_p=12;struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple3*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};static
const int Cyc_Absyn_UnknownFields_p=13;struct Cyc_Absyn_UnknownFields_p_struct{int
tag;struct _tuple3*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{
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
_tuple3*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple3*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Structfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structdecl{
void*sc;struct Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Uniondecl{void*sc;struct
Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple3*name;struct Cyc_List_List*
tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple3*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;};struct Cyc_Absyn_Enumfield{struct _tuple3*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple3*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple3*name;struct Cyc_List_List*tvs;void*defn;};static const int Cyc_Absyn_Var_d=0;
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
10;struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple3*f1;struct Cyc_List_List*f2;
};static const int Cyc_Absyn_ExternC_d=11;struct Cyc_Absyn_ExternC_d_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};static const int Cyc_Absyn_ArrayElement=0;struct Cyc_Absyn_ArrayElement_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_FieldName=1;struct Cyc_Absyn_FieldName_struct{
int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[15];extern
int Cyc_Absyn_qvar_cmp(struct _tuple3*,struct _tuple3*);extern void*Cyc_Absyn_conref_val(
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
unsigned char*tag;struct _tagged_arr f1;};static const int Cyc_Std_String_pa=0;struct
Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Std_Int_pa=
1;struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};static const int Cyc_Std_Double_pa=
2;struct Cyc_Std_Double_pa_struct{int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=
3;struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=
4;struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int Cyc_Std_printf(
struct _tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(
struct _tagged_arr fmt,struct _tagged_arr);static const int Cyc_Std_ShortPtr_sa=0;
struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};static const int Cyc_Std_UShortPtr_sa=
1;struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=
2;struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=
3;struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=
4;struct Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};static const int
Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};
static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{int tag;
float*f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_typ2string(void*);extern
struct _tagged_arr Cyc_Absynpp_typ2cstring(void*);extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];
static const int Cyc_Tcenv_VarRes=0;struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};
static const int Cyc_Tcenv_StructRes=1;struct Cyc_Tcenv_StructRes_struct{int tag;
struct Cyc_Absyn_Structdecl*f1;};static const int Cyc_Tcenv_TunionRes=2;struct Cyc_Tcenv_TunionRes_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};static
const int Cyc_Tcenv_EnumRes=3;struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};static const int Cyc_Tcenv_AnonEnumRes=4;struct
Cyc_Tcenv_AnonEnumRes_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict*
structdecls;struct Cyc_Dict_Dict*uniondecls;struct Cyc_Dict_Dict*tuniondecls;
struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;static const int
Cyc_Tcenv_NotLoop_j=0;static const int Cyc_Tcenv_CaseEnd_j=1;static const int Cyc_Tcenv_FnEnd_j=
2;static const int Cyc_Tcenv_Stmt_j=0;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};static const int Cyc_Tcenv_Outermost=0;struct Cyc_Tcenv_Outermost_struct{
int tag;void*f1;};static const int Cyc_Tcenv_Frame=1;struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};static const int Cyc_Tcenv_Hidden=2;struct Cyc_Tcenv_Hidden_struct{
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
_tmp3.tag=Cyc_Std_Int_pa;_tmp3.f1=(int)((unsigned int)info->emitted);{struct Cyc_Std_Int_pa_struct
_tmp2;_tmp2.tag=Cyc_Std_Int_pa;_tmp2.f1=(unsigned int)info;{struct Cyc_Std_String_pa_struct
_tmp1;_tmp1.tag=Cyc_Std_String_pa;_tmp1.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
type);{void*_tmp0[3]={& _tmp1,& _tmp2,& _tmp3};Cyc_Std_printf(_tag_arr("(%s,%x:%d,",
sizeof(unsigned char),11),_tag_arr(_tmp0,sizeof(void*),3));}}}});{struct Cyc_List_List*
p=info->dependencies;for(0;p != 0;p=p->tl){({struct Cyc_Std_Int_pa_struct _tmp5;
_tmp5.tag=Cyc_Std_Int_pa;_tmp5.f1=(unsigned int)((struct Cyc_Tcgenrep_RepInfo*)p->hd);{
void*_tmp4[1]={& _tmp5};Cyc_Std_printf(_tag_arr("%x",sizeof(unsigned char),3),
_tag_arr(_tmp4,sizeof(void*),1));}});if(p->tl != 0){({void*_tmp6[0]={};Cyc_Std_printf(
_tag_arr(",",sizeof(unsigned char),2),_tag_arr(_tmp6,sizeof(void*),0));});}}}({
void*_tmp7[0]={};Cyc_Std_printf(_tag_arr(")\n",sizeof(unsigned char),3),_tag_arr(
_tmp7,sizeof(void*),0));});}void Cyc_Tcgenrep_print_typerep_dict(struct Cyc_Dict_Dict*
dict){((void(*)(void(*f)(void*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Dict_Dict*
d))Cyc_Dict_iter)(Cyc_Tcgenrep_print_dict_entry,dict);}static int Cyc_Tcgenrep_rephash(
struct Cyc_Tcgenrep_RepInfo*ri){return(int)ri;}static int Cyc_Tcgenrep_repcmp(
struct Cyc_Tcgenrep_RepInfo*r1,struct Cyc_Tcgenrep_RepInfo*r2){unsigned int r1p;
unsigned int r2p;r1p=(unsigned int)r1;r2p=(unsigned int)r2;if(r1 < r2){return - 1;}
else{if(r1 == r2){return 0;}else{return 1;}}}static struct Cyc_Absyn_Tunionfield*Cyc_Tcgenrep_getField(
struct Cyc_Absyn_Tuniondecl*td,struct _tuple3*fieldname){if(td->fields == 0){({void*
_tmp8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("Could not find field in tuniondecl",sizeof(unsigned char),35),_tag_arr(
_tmp8,sizeof(void*),0));});}else{struct Cyc_List_List*l=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;l != 0;l=l->tl){if(! Cyc_Absyn_qvar_cmp(((
struct Cyc_Absyn_Tunionfield*)l->hd)->name,fieldname)){return(struct Cyc_Absyn_Tunionfield*)
l->hd;}}}({void*_tmp9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("Could not find field in tuniondecl",sizeof(
unsigned char),35),_tag_arr(_tmp9,sizeof(void*),0));});(int)_throw((void*)Cyc_Tcutil_TypeErr);}
static unsigned char _tmpA[9]="Tcgenrep";static struct _tagged_arr Cyc_Tcgenrep_tcgenrep_nm={
_tmpA,_tmpA,_tmpA + 9};static struct Cyc_List_List Cyc_Tcgenrep_l={(void*)& Cyc_Tcgenrep_tcgenrep_nm,
0};static struct Cyc_Absyn_Abs_n_struct Cyc_Tcgenrep_tcgenrep_nmspace={1,(struct Cyc_List_List*)&
Cyc_Tcgenrep_l};static void*Cyc_Tcgenrep_tcgenrep_nmspace2=(void*)& Cyc_Tcgenrep_tcgenrep_nmspace;
static unsigned char _tmpC[8]="Typerep";static struct _tagged_arr Cyc_Tcgenrep_typerep_nm={
_tmpC,_tmpC,_tmpC + 8};static struct Cyc_List_List Cyc_Tcgenrep_l2={(void*)& Cyc_Tcgenrep_typerep_nm,
0};static struct Cyc_Absyn_Abs_n_struct Cyc_Tcgenrep_typerep_nmspace={1,(struct Cyc_List_List*)&
Cyc_Tcgenrep_l2};static struct Cyc_List_List*Cyc_Tcgenrep_dfsvisit(struct Cyc_Tcgenrep_RepInfo*
ri,struct Cyc_Hashtable_Table*visited){struct _handler_cons _tmpE;_push_handler(&
_tmpE);{int _tmp10=0;if(setjmp(_tmpE.handler)){_tmp10=1;}if(! _tmp10){((int(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Tcgenrep_RepInfo*key))Cyc_Hashtable_lookup)(
visited,ri);{struct Cyc_List_List*_tmp11=0;_npop_handler(0);return _tmp11;};
_pop_handler();}else{void*_tmpF=(void*)_exn_thrown;void*_tmp13=_tmpF;_LL21: if(
_tmp13 == Cyc_Core_Not_found){goto _LL22;}else{goto _LL23;}_LL23: goto _LL24;_LL22:((
void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Tcgenrep_RepInfo*key,int val))Cyc_Hashtable_insert)(
visited,ri,1);{struct Cyc_List_List*ds=0;{struct Cyc_List_List*l=ri->dependencies;
for(0;l != 0;l=l->tl){ds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_imp_append)(ds,Cyc_Tcgenrep_dfsvisit((struct Cyc_Tcgenrep_RepInfo*)
l->hd,visited));}}if(ri->emitted == 0){if(ri->fwd_decl != 0){ds=({struct Cyc_List_List*
_tmp19=_cycalloc(sizeof(struct Cyc_List_List));_tmp19->hd=({struct Cyc_Absyn_Decl*
_tmp1A[1];_tmp1A[0]=(struct Cyc_Absyn_Decl*)((struct Cyc_Core_Opt*)_check_null(ri->fwd_decl))->v;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1A,
sizeof(struct Cyc_Absyn_Decl*),1));});_tmp19->tl=ds;_tmp19;});}{struct Cyc_List_List*
_tmp1B=({struct Cyc_List_List*_tmp1C=_cycalloc(sizeof(struct Cyc_List_List));
_tmp1C->hd=ri->decls;_tmp1C->tl=0;_tmp1C;});ri->emitted=1;return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(ds,_tmp1B);}}
else{return ds;}}_LL24:(void)_throw(_tmp13);_LL20:;}}}static struct Cyc_List_List*
Cyc_Tcgenrep_dfs(struct Cyc_Tcgenrep_RepInfo*ri){struct Cyc_Hashtable_Table*tab=((
struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct Cyc_Tcgenrep_RepInfo*,struct
Cyc_Tcgenrep_RepInfo*),int(*hash)(struct Cyc_Tcgenrep_RepInfo*)))Cyc_Hashtable_create)(
53,Cyc_Tcgenrep_repcmp,Cyc_Tcgenrep_rephash);struct Cyc_List_List*ds=Cyc_Tcgenrep_dfsvisit(
ri,tab);return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(
ds);}static unsigned char _tmp1D[11]="Typestruct";static struct _tagged_arr Cyc_Tcgenrep_typestruct_str={
_tmp1D,_tmp1D,_tmp1D + 11};static unsigned char _tmp1E[4]="Var";static struct
_tagged_arr Cyc_Tcgenrep_var_str={_tmp1E,_tmp1E,_tmp1E + 4};static unsigned char
_tmp1F[4]="Int";static struct _tagged_arr Cyc_Tcgenrep_int_str={_tmp1F,_tmp1F,
_tmp1F + 4};static unsigned char _tmp20[6]="Float";static struct _tagged_arr Cyc_Tcgenrep_float_str={
_tmp20,_tmp20,_tmp20 + 6};static unsigned char _tmp21[7]="Double";static struct
_tagged_arr Cyc_Tcgenrep_double_str={_tmp21,_tmp21,_tmp21 + 7};static unsigned char
_tmp22[8]="ThinPtr";static struct _tagged_arr Cyc_Tcgenrep_thinptr_str={_tmp22,
_tmp22,_tmp22 + 8};static unsigned char _tmp23[7]="FatPtr";static struct _tagged_arr
Cyc_Tcgenrep_fatptr_str={_tmp23,_tmp23,_tmp23 + 7};static unsigned char _tmp24[6]="Tuple";
static struct _tagged_arr Cyc_Tcgenrep_tuple_str={_tmp24,_tmp24,_tmp24 + 6};static
unsigned char _tmp25[7]="TUnion";static struct _tagged_arr Cyc_Tcgenrep_tunion_str={
_tmp25,_tmp25,_tmp25 + 7};static unsigned char _tmp26[8]="XTUnion";static struct
_tagged_arr Cyc_Tcgenrep_xtunion_str={_tmp26,_tmp26,_tmp26 + 8};static
unsigned char _tmp27[6]="Union";static struct _tagged_arr Cyc_Tcgenrep_union_str={
_tmp27,_tmp27,_tmp27 + 6};static unsigned char _tmp28[7]="name_t";static struct
_tagged_arr Cyc_Tcgenrep_name_t_str={_tmp28,_tmp28,_tmp28 + 7};static struct _tuple3*
Cyc_Tcgenrep_typerep_name(struct _tagged_arr*name){return({struct _tuple3*_tmp29=
_cycalloc(sizeof(struct _tuple3));_tmp29->f1=(void*)& Cyc_Tcgenrep_typerep_nmspace;
_tmp29->f2=name;_tmp29;});}static int Cyc_Tcgenrep_gen_id_counter=0;static struct
_tagged_arr*Cyc_Tcgenrep_new_gen_id(struct _tagged_arr name){return({struct
_tagged_arr*_tmp2A=_cycalloc(sizeof(struct _tagged_arr));_tmp2A[0]=(struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp2D;_tmp2D.tag=Cyc_Std_Int_pa;_tmp2D.f1=(
int)((unsigned int)Cyc_Tcgenrep_gen_id_counter ++);{struct Cyc_Std_String_pa_struct
_tmp2C;_tmp2C.tag=Cyc_Std_String_pa;_tmp2C.f1=(struct _tagged_arr)name;{void*
_tmp2B[2]={& _tmp2C,& _tmp2D};Cyc_Std_aprintf(_tag_arr("_gen%s_%d",sizeof(
unsigned char),10),_tag_arr(_tmp2B,sizeof(void*),2));}}});_tmp2A;});}static void
Cyc_Tcgenrep_print_params(struct Cyc_List_List*l){({void*_tmp2E[0]={};Cyc_Std_printf(
_tag_arr("<",sizeof(unsigned char),2),_tag_arr(_tmp2E,sizeof(void*),0));});{
struct Cyc_List_List*p=l;for(0;p != 0;p=p->tl){({struct Cyc_Std_Int_pa_struct _tmp31;
_tmp31.tag=Cyc_Std_Int_pa;_tmp31.f1=(int)((unsigned int)((int)(p->tl != 0?',':' ')));{
struct Cyc_Std_String_pa_struct _tmp30;_tmp30.tag=Cyc_Std_String_pa;_tmp30.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)p->hd);{void*_tmp2F[2]={& _tmp30,&
_tmp31};Cyc_Std_printf(_tag_arr("%s%c",sizeof(unsigned char),5),_tag_arr(_tmp2F,
sizeof(void*),2));}}});}}({void*_tmp32[0]={};Cyc_Std_printf(_tag_arr(">\n",
sizeof(unsigned char),3),_tag_arr(_tmp32,sizeof(void*),0));});}static void Cyc_Tcgenrep_print_tvars(
struct Cyc_List_List*l){({void*_tmp33[0]={};Cyc_Std_printf(_tag_arr("<",sizeof(
unsigned char),2),_tag_arr(_tmp33,sizeof(void*),0));});{struct Cyc_List_List*p=l;
for(0;p != 0;p=p->tl){({struct Cyc_Std_Int_pa_struct _tmp36;_tmp36.tag=Cyc_Std_Int_pa;
_tmp36.f1=(int)((unsigned int)((int)(p->tl != 0?',':' ')));{struct Cyc_Std_String_pa_struct
_tmp35;_tmp35.tag=Cyc_Std_String_pa;_tmp35.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)
p->hd)->name;{void*_tmp34[2]={& _tmp35,& _tmp36};Cyc_Std_printf(_tag_arr("%s%c",
sizeof(unsigned char),5),_tag_arr(_tmp34,sizeof(void*),2));}}});}}({void*_tmp37[
0]={};Cyc_Std_printf(_tag_arr(">\n",sizeof(unsigned char),3),_tag_arr(_tmp37,
sizeof(void*),0));});}static struct Cyc_Absyn_Rel_n_struct Cyc_Tcgenrep_top_nm_0={0,
0};static void*Cyc_Tcgenrep_top_nm=(void*)& Cyc_Tcgenrep_top_nm_0;static struct
_tuple3*Cyc_Tcgenrep_toplevel_name(struct _tagged_arr*name){return({struct _tuple3*
_tmp39=_cycalloc(sizeof(struct _tuple3));_tmp39->f1=Cyc_Tcgenrep_top_nm;_tmp39->f2=(
struct _tagged_arr*)name;_tmp39;});}static struct _tuple3*Cyc_Tcgenrep_relative_name(
struct Cyc_List_List*pathl,struct _tagged_arr*name){struct Cyc_Absyn_Rel_n_struct*
path=({struct Cyc_Absyn_Rel_n_struct*_tmp3B=_cycalloc(sizeof(struct Cyc_Absyn_Rel_n_struct));
_tmp3B[0]=({struct Cyc_Absyn_Rel_n_struct _tmp3C;_tmp3C.tag=Cyc_Absyn_Rel_n;_tmp3C.f1=
pathl;_tmp3C;});_tmp3B;});return({struct _tuple3*_tmp3A=_cycalloc(sizeof(struct
_tuple3));_tmp3A->f1=(void*)path;_tmp3A->f2=(struct _tagged_arr*)name;_tmp3A;});}
static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl(struct _tagged_arr*name,void*
type,struct Cyc_Absyn_Exp*init,struct Cyc_Position_Segment*seg){struct _tuple3*qvar=
Cyc_Tcgenrep_toplevel_name(name);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(
qvar,type,init);(void*)(vd->sc=(void*)((void*)Cyc_Absyn_Static));{struct Cyc_Absyn_Var_d_struct*
r1=({struct Cyc_Absyn_Var_d_struct*_tmp3D=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_struct));
_tmp3D[0]=({struct Cyc_Absyn_Var_d_struct _tmp3E;_tmp3E.tag=Cyc_Absyn_Var_d;_tmp3E.f1=
vd;_tmp3E;});_tmp3D;});void*r2=(void*)r1;return Cyc_Absyn_new_decl(r2,seg);}}
static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_vardecl(struct _tuple3*name,void*type,
struct Cyc_Absyn_Exp*init,void*sc,struct Cyc_Position_Segment*seg){struct _tuple3*
topname=Cyc_Tcgenrep_toplevel_name((*name).f2);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(
topname,type,init);(void*)(vd->sc=(void*)sc);{struct Cyc_Absyn_Var_d_struct*r1=({
struct Cyc_Absyn_Var_d_struct*_tmp3F=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_struct));
_tmp3F[0]=({struct Cyc_Absyn_Var_d_struct _tmp40;_tmp40.tag=Cyc_Absyn_Var_d;_tmp40.f1=
vd;_tmp40;});_tmp3F;});void*r2=(void*)r1;return Cyc_Absyn_new_decl(r2,seg);}}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_string(struct _tagged_arr s,struct Cyc_Position_Segment*
seg){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_String_c_struct*_tmp41=
_cycalloc(sizeof(struct Cyc_Absyn_String_c_struct));_tmp41[0]=({struct Cyc_Absyn_String_c_struct
_tmp42;_tmp42.tag=Cyc_Absyn_String_c;_tmp42.f1=s;_tmp42;});_tmp41;}),seg);}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_string_cls(struct Cyc_Position_Segment*
seg,struct _tagged_arr*s){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_String_c_struct*
_tmp43=_cycalloc(sizeof(struct Cyc_Absyn_String_c_struct));_tmp43[0]=({struct Cyc_Absyn_String_c_struct
_tmp44;_tmp44.tag=Cyc_Absyn_String_c;_tmp44.f1=*s;_tmp44;});_tmp43;}),seg);}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int(int i,struct Cyc_Position_Segment*
seg){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*_tmp45=
_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));_tmp45[0]=({struct Cyc_Absyn_Int_c_struct
_tmp46;_tmp46.tag=Cyc_Absyn_Int_c;_tmp46.f1=(void*)((void*)Cyc_Absyn_Signed);
_tmp46.f2=i;_tmp46;});_tmp45;}),seg);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int_cls(
struct Cyc_Position_Segment*seg,int i){return Cyc_Absyn_const_exp((void*)({struct
Cyc_Absyn_Int_c_struct*_tmp47=_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));
_tmp47[0]=({struct Cyc_Absyn_Int_c_struct _tmp48;_tmp48.tag=Cyc_Absyn_Int_c;_tmp48.f1=(
void*)((void*)Cyc_Absyn_Signed);_tmp48.f2=i;_tmp48;});_tmp47;}),seg);}static int
Cyc_Tcgenrep_size_of2int(void*sz){void*_tmp49=sz;_LL75: if(_tmp49 == (void*)Cyc_Absyn_B1){
goto _LL76;}else{goto _LL77;}_LL77: if(_tmp49 == (void*)Cyc_Absyn_B2){goto _LL78;}
else{goto _LL79;}_LL79: if(_tmp49 == (void*)Cyc_Absyn_B4){goto _LL80;}else{goto _LL81;}
_LL81: if(_tmp49 == (void*)Cyc_Absyn_B8){goto _LL82;}else{goto _LL74;}_LL76: return 8;
_LL78: return 16;_LL80: return 32;_LL82: return 64;_LL74:;}static void*Cyc_Tcgenrep_array_typ(
void*ty,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp53=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp53[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp54;_tmp54.tag=Cyc_Absyn_ArrayType;_tmp54.f1=(void*)ty;_tmp54.f2=tq;_tmp54.f3=
e;_tmp54;});_tmp53;});}static void*Cyc_Tcgenrep_tunion_typ(struct _tuple3*name){
return(void*)({struct Cyc_Absyn_TunionType_struct*_tmp55=_cycalloc(sizeof(struct
Cyc_Absyn_TunionType_struct));_tmp55[0]=({struct Cyc_Absyn_TunionType_struct
_tmp56;_tmp56.tag=Cyc_Absyn_TunionType;_tmp56.f1=({struct Cyc_Absyn_TunionInfo
_tmp57;_tmp57.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp58=_cycalloc(sizeof(struct Cyc_Absyn_UnknownTunion_struct));_tmp58[0]=({
struct Cyc_Absyn_UnknownTunion_struct _tmp59;_tmp59.tag=Cyc_Absyn_UnknownTunion;
_tmp59.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp5A;_tmp5A.name=name;_tmp5A.is_xtunion=
0;_tmp5A;});_tmp59;});_tmp58;}));_tmp57.targs=0;_tmp57.rgn=(void*)((void*)Cyc_Absyn_HeapRgn);
_tmp57;});_tmp56;});_tmp55;});}static void*Cyc_Tcgenrep_tunionfield_typ(struct
_tuple3*name,struct _tuple3*fieldname){return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp5B=_cycalloc(sizeof(struct Cyc_Absyn_TunionFieldType_struct));_tmp5B[0]=({
struct Cyc_Absyn_TunionFieldType_struct _tmp5C;_tmp5C.tag=Cyc_Absyn_TunionFieldType;
_tmp5C.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp5D;_tmp5D.field_info=(void*)((
void*)({struct Cyc_Absyn_UnknownTunionfield_struct*_tmp5E=_cycalloc(sizeof(struct
Cyc_Absyn_UnknownTunionfield_struct));_tmp5E[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp5F;_tmp5F.tag=Cyc_Absyn_UnknownTunionfield;_tmp5F.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp60;_tmp60.tunion_name=name;_tmp60.field_name=fieldname;_tmp60.is_xtunion=0;
_tmp60;});_tmp5F;});_tmp5E;}));_tmp5D.targs=0;_tmp5D;});_tmp5C;});_tmp5B;});}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_call_exp(struct _tuple3*name,struct Cyc_List_List*
args,struct Cyc_Position_Segment*loc){return Cyc_Absyn_unknowncall_exp(Cyc_Absyn_unknownid_exp(
name,loc),args,loc);}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_tunion_constructor_decl(
struct _tuple3*tunionname,struct _tuple3*fieldname,struct _tuple3*varname,struct Cyc_List_List*
args,void*sc,struct Cyc_Position_Segment*loc){void*_tmp61=Cyc_Tcgenrep_tunionfield_typ(
tunionname,fieldname);struct Cyc_Absyn_Exp*_tmp62=Cyc_Tcgenrep_call_exp(fieldname,
args,loc);struct Cyc_Absyn_Decl*_tmp63=Cyc_Tcgenrep_gen_vardecl(varname,_tmp61,(
struct Cyc_Absyn_Exp*)_tmp62,sc,loc);return _tmp63;}struct _tuple6{struct Cyc_Absyn_Tqual
f1;void*f2;};static void*Cyc_Tcgenrep_get_second(struct _tuple6*pair){return(*pair).f2;}
struct _tuple7{struct _tagged_arr*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tagged_arr*Cyc_Tcgenrep_get_first(struct _tuple7*pair){return(*pair).f1;}static
unsigned char _tmp64[5]="list";static struct _tagged_arr Cyc_Tcgenrep_list_str={
_tmp64,_tmp64,_tmp64 + 5};static unsigned char _tmp65[5]="List";static struct
_tagged_arr Cyc_Tcgenrep_List_str={_tmp65,_tmp65,_tmp65 + 5};struct _tuple8{struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple2_exp_cls(
struct Cyc_Position_Segment*loc,struct _tuple8*es){struct _tuple8 _tmp68;struct Cyc_Absyn_Exp*
_tmp69;struct Cyc_Absyn_Exp*_tmp6B;struct _tuple8*_tmp66=es;_tmp68=*_tmp66;_LL108:
_tmp6B=_tmp68.f1;goto _LL106;_LL106: _tmp69=_tmp68.f2;goto _LL103;_LL103: return Cyc_Absyn_tuple_exp(({
struct Cyc_Absyn_Exp*_tmp6D[2];_tmp6D[1]=_tmp69;_tmp6D[0]=_tmp6B;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp6D,sizeof(struct Cyc_Absyn_Exp*),2));}),
loc);}static unsigned char _tmp6E[5]="NULL";static struct _tagged_arr Cyc_Tcgenrep_null_str={
_tmp6E,_tmp6E,_tmp6E + 5};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_list_exp(struct
Cyc_List_List*l,struct Cyc_Position_Segment*loc){if(l == 0){return Cyc_Absyn_null_exp(
loc);}return Cyc_Tcgenrep_call_exp(Cyc_Tcgenrep_toplevel_name(& Cyc_Tcgenrep_list_str),
l,loc);}struct _tuple9{void*f1;struct Cyc_Position_Segment*f2;};static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_make_offsetof_exp(struct _tuple9*typeloc,int index){struct _tuple9
_tmp71;struct Cyc_Position_Segment*_tmp72;void*_tmp74;struct _tuple9*_tmp6F=
typeloc;_tmp71=*_tmp6F;_LL117: _tmp74=_tmp71.f1;goto _LL115;_LL115: _tmp72=_tmp71.f2;
goto _LL112;_LL112: return Cyc_Absyn_offsetof_exp(_tmp74,(void*)({struct Cyc_Absyn_TupleIndex_struct*
_tmp76=_cycalloc_atomic(sizeof(struct Cyc_Absyn_TupleIndex_struct));_tmp76[0]=({
struct Cyc_Absyn_TupleIndex_struct _tmp77;_tmp77.tag=Cyc_Absyn_TupleIndex;_tmp77.f1=(
unsigned int)index;_tmp77;});_tmp76;}),_tmp72);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_get_and_cast_ri_exp(
struct Cyc_Position_Segment*loc,struct Cyc_Tcgenrep_RepInfo*info){return Cyc_Absyn_cast_exp(
Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str)),(
struct Cyc_Absyn_Exp*)_check_null(info->exp),loc);}static struct _tuple7*Cyc_Tcgenrep_gen_id_for_exp(
struct Cyc_Absyn_Exp*e){return({struct _tuple7*_tmp78=_cycalloc(sizeof(struct
_tuple7));_tmp78->f1=Cyc_Tcgenrep_new_gen_id(_tag_arr("tuple",sizeof(
unsigned char),6));_tmp78->f2=e;_tmp78;});}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl_cls(
struct _tuple9*env,struct _tuple7*name_exp){struct _tuple9 _tmp7B;struct Cyc_Position_Segment*
_tmp7C;void*_tmp7E;struct _tuple9*_tmp79=env;_tmp7B=*_tmp79;_LL127: _tmp7E=_tmp7B.f1;
goto _LL125;_LL125: _tmp7C=_tmp7B.f2;goto _LL122;_LL122: {struct _tuple7 _tmp82;
struct Cyc_Absyn_Exp*_tmp83;struct _tagged_arr*_tmp85;struct _tuple7*_tmp80=
name_exp;_tmp82=*_tmp80;_LL134: _tmp85=_tmp82.f1;goto _LL132;_LL132: _tmp83=_tmp82.f2;
goto _LL129;_LL129: return Cyc_Tcgenrep_gen_decl(_tmp85,_tmp7E,(struct Cyc_Absyn_Exp*)
_tmp83,_tmp7C);}}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
static struct _tuple10*Cyc_Tcgenrep_null_designator_exp(struct Cyc_Absyn_Exp*e){
return({struct _tuple10*_tmp87=_cycalloc(sizeof(struct _tuple10));_tmp87->f1=0;
_tmp87->f2=e;_tmp87;});}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_arr_init_exp(
struct Cyc_List_List*l,struct Cyc_Position_Segment*loc){struct Cyc_List_List*_tmp88=((
struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcgenrep_null_designator_exp,l);return Cyc_Absyn_unresolvedmem_exp(
0,_tmp88,loc);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_address_exp_cls(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_address_exp(e,loc);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_unknownid_exp_cls(struct Cyc_Position_Segment*loc,struct _tuple3*e){
return Cyc_Absyn_unknownid_exp(e,loc);}static int Cyc_Tcgenrep_has_bitfield(struct
Cyc_Absyn_Structfield*sf){return sf->width != 0;}static int Cyc_Tcgenrep_add_bitfield_sizes(
int total,struct Cyc_Absyn_Structfield*sf){return(int)(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(sf->width))+ total);}static void*Cyc_Tcgenrep_select_structfield_type(
struct Cyc_Absyn_Structfield*sf){if(Cyc_Std_strcmp(*sf->name,_tag_arr("",sizeof(
unsigned char),1))== 0){({void*_tmp89[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): anonymous (padding) structfield not handled yet",
sizeof(unsigned char),55),_tag_arr(_tmp89,sizeof(void*),0));});}if(Cyc_Tcgenrep_has_bitfield(
sf)){({void*_tmp8A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen(): mixed struct bitfields and union bitfields not handled",sizeof(
unsigned char),62),_tag_arr(_tmp8A,sizeof(void*),0));});}return(void*)sf->type;}
static struct Cyc_Dict_Dict*Cyc_Tcgenrep_update_info(struct Cyc_Dict_Dict*dict,void*
type,struct Cyc_List_List*decls,struct Cyc_Absyn_Exp*exp,struct Cyc_List_List*
dependencies,struct Cyc_Core_Opt*fwd_decl){struct Cyc_Core_Opt*_tmp8B=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(dict,type);if(_tmp8B != 0){if(((
struct Cyc_Tcgenrep_RepInfo*)_tmp8B->v)->decls != 0){Cyc_Tcgenrep_print_typerep_dict(
dict);({void*_tmp8C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("Updating non-forward declaration",sizeof(unsigned char),33),_tag_arr(
_tmp8C,sizeof(void*),0));});}((struct Cyc_Tcgenrep_RepInfo*)_tmp8B->v)->decls=
decls;((struct Cyc_Tcgenrep_RepInfo*)_tmp8B->v)->exp=exp;((struct Cyc_Tcgenrep_RepInfo*)
_tmp8B->v)->dependencies=dependencies;return dict;}else{return((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(dict,
type,({struct Cyc_Tcgenrep_RepInfo*_tmp8D=_cycalloc(sizeof(struct Cyc_Tcgenrep_RepInfo));
_tmp8D->decls=decls;_tmp8D->exp=exp;_tmp8D->dependencies=dependencies;_tmp8D->fwd_decl=
fwd_decl;_tmp8D->emitted=0;_tmp8D->is_extern=0;_tmp8D;}));}}static struct Cyc_Dict_Dict*
Cyc_Tcgenrep_make_fwd_decl_info(struct Cyc_Dict_Dict*dict,void*type,struct Cyc_Absyn_Exp*
exp,struct Cyc_Core_Opt*fwd_decl,int is_extern){struct Cyc_Core_Opt*_tmp8E=((struct
Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(dict,type);
if(_tmp8E != 0){({struct Cyc_Std_String_pa_struct _tmp90;_tmp90.tag=Cyc_Std_String_pa;
_tmp90.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(type);{void*_tmp8F[1]={&
_tmp90};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("Repinfo for fwd declaration of %s already exists",sizeof(unsigned char),
49),_tag_arr(_tmp8F,sizeof(void*),1));}});return dict;}else{return((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(dict,
type,({struct Cyc_Tcgenrep_RepInfo*_tmp91=_cycalloc(sizeof(struct Cyc_Tcgenrep_RepInfo));
_tmp91->decls=0;_tmp91->exp=exp;_tmp91->dependencies=0;_tmp91->fwd_decl=fwd_decl;
_tmp91->emitted=0;_tmp91->is_extern=is_extern;_tmp91;}));}}static struct Cyc_Absyn_Tqual
Cyc_Tcgenrep_tq_none={0,0,0};static struct _tuple6*Cyc_Tcgenrep_tqual_type(struct
Cyc_Absyn_Tqual*tq,void*type){return({struct _tuple6*_tmp92=_cycalloc(sizeof(
struct _tuple6));_tmp92->f1=*((struct Cyc_Absyn_Tqual*)_check_null(tq));_tmp92->f2=
type;_tmp92;});}static void*Cyc_Tcgenrep_tuple_typ(struct Cyc_List_List*types){
struct Cyc_List_List*_tmp93=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(struct
Cyc_Absyn_Tqual*,void*),struct Cyc_Absyn_Tqual*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tqual_type,(struct Cyc_Absyn_Tqual*)& Cyc_Tcgenrep_tq_none,types);
struct Cyc_Absyn_TupleType_struct*tuple_type_base=({struct Cyc_Absyn_TupleType_struct*
_tmp94=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_struct));_tmp94[0]=({struct Cyc_Absyn_TupleType_struct
_tmp95;_tmp95.tag=Cyc_Absyn_TupleType;_tmp95.f1=_tmp93;_tmp95;});_tmp94;});void*
tuple_type=(void*)tuple_type_base;return tuple_type;}static struct _tuple10*Cyc_Tcgenrep_array_decls(
void*type,struct Cyc_List_List*exps,struct Cyc_Position_Segment*loc){struct Cyc_List_List*
_tmp96=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_gen_id_for_exp,exps);struct Cyc_List_List*
_tmp97=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(struct _tuple9*,struct
_tuple7*),struct _tuple9*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_gen_decl_cls,({
struct _tuple9*_tmpA4=_cycalloc(sizeof(struct _tuple9));_tmpA4->f1=type;_tmpA4->f2=
loc;_tmpA4;}),_tmp96);struct Cyc_List_List*_tmp98=((struct Cyc_List_List*(*)(
struct _tagged_arr*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_first,
_tmp96);struct Cyc_List_List*_tmp99=((struct Cyc_List_List*(*)(struct _tuple3*(*f)(
struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_toplevel_name,
_tmp98);struct Cyc_List_List*_tmp9A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple3*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_unknownid_exp_cls,loc,_tmp99);
struct Cyc_List_List*_tmp9B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Exp*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_address_exp_cls,loc,_tmp9A);
struct Cyc_Absyn_Exp*_tmp9C=Cyc_Tcgenrep_arr_init_exp(_tmp9B,loc);struct
_tagged_arr*_tmp9D=Cyc_Tcgenrep_new_gen_id(_tag_arr("arr",sizeof(unsigned char),
4));void*_tmp9E=Cyc_Absyn_at_typ(type,(void*)Cyc_Absyn_HeapRgn,Cyc_Tcgenrep_tq_none);
void*_tmp9F=Cyc_Tcgenrep_array_typ(_tmp9E,Cyc_Tcgenrep_tq_none,0);struct Cyc_Absyn_Decl*
_tmpA0=Cyc_Tcgenrep_gen_decl(_tmp9D,_tmp9F,(struct Cyc_Absyn_Exp*)_tmp9C,loc);
struct Cyc_Absyn_Exp*_tmpA1=Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmp9D),loc);return({struct _tuple10*_tmpA2=_cycalloc(sizeof(struct _tuple10));
_tmpA2->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp97,({struct Cyc_Absyn_Decl*_tmpA3[1];_tmpA3[0]=_tmpA0;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpA3,
sizeof(struct Cyc_Absyn_Decl*),1));}));_tmpA2->f2=_tmpA1;_tmpA2;});}static void*
Cyc_Tcgenrep_check_tunionfield_and_get_type(struct Cyc_Absyn_Tunionfield*tuf){if(
tuf->tvs != 0){({void*_tmpA5[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield type varaibles not handled",
sizeof(unsigned char),46),_tag_arr(_tmpA5,sizeof(void*),0));});}return(void*)({
struct Cyc_Absyn_TupleType_struct*_tmpA6=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_struct));
_tmpA6[0]=({struct Cyc_Absyn_TupleType_struct _tmpA7;_tmpA7.tag=Cyc_Absyn_TupleType;
_tmpA7.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(({struct _tuple6*_tmpA8[1];_tmpA8[0]=({struct _tuple6*_tmpA9=
_cycalloc(sizeof(struct _tuple6));_tmpA9->f1=Cyc_Tcgenrep_tq_none;_tmpA9->f2=Cyc_Absyn_uint_t;
_tmpA9;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmpA8,sizeof(struct _tuple6*),1));}),tuf->typs);_tmpA7;});_tmpA6;});}struct
_tuple11{struct _tagged_arr*f1;void*f2;};static struct _tuple11*Cyc_Tcgenrep_check_xtunionfield_and_get_name_type(
struct Cyc_Absyn_Tunionfield*tuf){if(tuf->tvs != 0){({void*_tmpAA[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield type varaibles not handled",
sizeof(unsigned char),46),_tag_arr(_tmpAA,sizeof(void*),0));});}return({struct
_tuple11*_tmpAB=_cycalloc(sizeof(struct _tuple11));_tmpAB->f1=(*tuf->name).f2;
_tmpAB->f2=(void*)({struct Cyc_Absyn_TupleType_struct*_tmpAC=_cycalloc(sizeof(
struct Cyc_Absyn_TupleType_struct));_tmpAC[0]=({struct Cyc_Absyn_TupleType_struct
_tmpAD;_tmpAD.tag=Cyc_Absyn_TupleType;_tmpAD.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple6*
_tmpAE[1];_tmpAE[0]=({struct _tuple6*_tmpAF=_cycalloc(sizeof(struct _tuple6));
_tmpAF->f1=Cyc_Tcgenrep_tq_none;_tmpAF->f2=Cyc_Absyn_star_typ(Cyc_Absyn_uchar_t,(
void*)Cyc_Absyn_HeapRgn,Cyc_Tcgenrep_tq_none);_tmpAF;});((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpAE,sizeof(struct _tuple6*),1));}),
tuf->typs);_tmpAD;});_tmpAC;});_tmpAB;});}static int Cyc_Tcgenrep_filter_empty_tunionfield(
struct Cyc_Absyn_Tunionfield*tuf){return tuf->typs != 0;}static struct _tagged_arr*
Cyc_Tcgenrep_get_tunionfield_name(struct Cyc_Absyn_Tunionfield*tuf){return(*tuf->name).f2;}
struct Cyc_Absyn_Structfield*Cyc_Tcgenrep_substitute_structfield_type(struct Cyc_List_List*
subst,struct Cyc_Absyn_Structfield*sf){return({struct Cyc_Absyn_Structfield*_tmpB0=
_cycalloc(sizeof(struct Cyc_Absyn_Structfield));_tmpB0->name=sf->name;_tmpB0->tq=
sf->tq;_tmpB0->type=(void*)Cyc_Tcutil_substitute(subst,(void*)sf->type);_tmpB0->width=
sf->width;_tmpB0->attributes=sf->attributes;_tmpB0;});}struct _tuple6*Cyc_Tcgenrep_substitute_tqual_type(
struct Cyc_List_List*subst,struct _tuple6*pair){struct _tuple6 _tmpB3;void*_tmpB4;
struct Cyc_Absyn_Tqual _tmpB6;struct _tuple6*_tmpB1=pair;_tmpB3=*_tmpB1;_LL183:
_tmpB6=_tmpB3.f1;goto _LL181;_LL181: _tmpB4=_tmpB3.f2;goto _LL178;_LL178: return({
struct _tuple6*_tmpB8=_cycalloc(sizeof(struct _tuple6));_tmpB8->f1=_tmpB6;_tmpB8->f2=
Cyc_Tcutil_substitute(subst,_tmpB4);_tmpB8;});}struct Cyc_Absyn_Tunionfield*Cyc_Tcgenrep_substitute_tunionfield_type(
struct Cyc_List_List*subst,struct Cyc_Absyn_Tunionfield*tf){return({struct Cyc_Absyn_Tunionfield*
_tmpB9=_cycalloc(sizeof(struct Cyc_Absyn_Tunionfield));_tmpB9->name=tf->name;
_tmpB9->tvs=tf->tvs;_tmpB9->typs=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(
struct Cyc_List_List*,struct _tuple6*),struct Cyc_List_List*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_tqual_type,subst,tf->typs);_tmpB9->loc=
tf->loc;_tmpB9->sc=(void*)((void*)tf->sc);_tmpB9;});}void*Cyc_Tcgenrep_monomorphize_type(
void*type){void*_tmpBA=Cyc_Tcutil_compress(type);struct Cyc_Absyn_Structdecl**
_tmpC4;struct Cyc_List_List*_tmpC6;struct _tuple3*_tmpC8;struct Cyc_Absyn_Uniondecl**
_tmpCA;struct Cyc_List_List*_tmpCC;struct _tuple3*_tmpCE;struct Cyc_Absyn_TunionInfo
_tmpD0;void*_tmpD2;struct Cyc_List_List*_tmpD4;void*_tmpD6;struct Cyc_Absyn_Tuniondecl**
_tmpD8;struct Cyc_Absyn_Tuniondecl*_tmpDA;_LL188: if((unsigned int)_tmpBA > 3?*((
int*)_tmpBA)== Cyc_Absyn_StructType: 0){_LL201: _tmpC8=((struct Cyc_Absyn_StructType_struct*)
_tmpBA)->f1;goto _LL199;_LL199: _tmpC6=((struct Cyc_Absyn_StructType_struct*)_tmpBA)->f2;
goto _LL197;_LL197: _tmpC4=((struct Cyc_Absyn_StructType_struct*)_tmpBA)->f3;goto
_LL189;}else{goto _LL190;}_LL190: if((unsigned int)_tmpBA > 3?*((int*)_tmpBA)== Cyc_Absyn_UnionType:
0){_LL207: _tmpCE=((struct Cyc_Absyn_UnionType_struct*)_tmpBA)->f1;goto _LL205;
_LL205: _tmpCC=((struct Cyc_Absyn_UnionType_struct*)_tmpBA)->f2;goto _LL203;_LL203:
_tmpCA=((struct Cyc_Absyn_UnionType_struct*)_tmpBA)->f3;goto _LL191;}else{goto
_LL192;}_LL192: if((unsigned int)_tmpBA > 3?*((int*)_tmpBA)== Cyc_Absyn_TunionType:
0){_LL209: _tmpD0=((struct Cyc_Absyn_TunionType_struct*)_tmpBA)->f1;_LL215: _tmpD6=(
void*)_tmpD0.tunion_info;if(*((int*)_tmpD6)== Cyc_Absyn_KnownTunion){_LL217:
_tmpD8=((struct Cyc_Absyn_KnownTunion_struct*)_tmpD6)->f1;_tmpDA=*_tmpD8;goto
_LL213;}else{goto _LL194;}_LL213: _tmpD4=_tmpD0.targs;goto _LL211;_LL211: _tmpD2=(
void*)_tmpD0.rgn;goto _LL193;}else{goto _LL194;}_LL194: goto _LL195;_LL189: if(_tmpC4
== 0){({void*_tmpDB[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("struct decl must exist at this point",sizeof(unsigned char),37),
_tag_arr(_tmpDB,sizeof(void*),0));});}{struct Cyc_Absyn_Structdecl*_tmpDC=*((
struct Cyc_Absyn_Structdecl**)_check_null(_tmpC4));struct Cyc_List_List*_tmpDD=
_tmpDC->tvs;if(Cyc_List_length(_tmpC6)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpDD)){({struct Cyc_Std_Int_pa_struct _tmpE0;_tmpE0.tag=Cyc_Std_Int_pa;_tmpE0.f1=(
int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpDD));{
struct Cyc_Std_Int_pa_struct _tmpDF;_tmpDF.tag=Cyc_Std_Int_pa;_tmpDF.f1=(int)((
unsigned int)Cyc_List_length(_tmpC6));{void*_tmpDE[2]={& _tmpDF,& _tmpE0};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof(unsigned char),60),_tag_arr(_tmpDE,sizeof(void*),2));}}});}{struct Cyc_List_List*
_tmpE1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmpDD,_tmpC6);struct Cyc_Core_Opt*fields=0;if(_tmpDC->fields != 0){
fields=({struct Cyc_Core_Opt*_tmpE2=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpE2->v=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_structfield_type,_tmpE1,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpDC->fields))->v);_tmpE2;});}{struct Cyc_Absyn_Structdecl*
stdecl2=({struct Cyc_Absyn_Structdecl*_tmpE8=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));
_tmpE8->sc=(void*)((void*)_tmpDC->sc);_tmpE8->name=_tmpDC->name;_tmpE8->tvs=0;
_tmpE8->fields=fields;_tmpE8->attributes=_tmpDC->attributes;_tmpE8;});struct Cyc_Absyn_Structdecl**
_tmpE3=({struct Cyc_Absyn_Structdecl**_tmpE7=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl*));
_tmpE7[0]=stdecl2;_tmpE7;});struct Cyc_Absyn_StructType_struct*_tmpE4=({struct Cyc_Absyn_StructType_struct*
_tmpE5=_cycalloc(sizeof(struct Cyc_Absyn_StructType_struct));_tmpE5[0]=({struct
Cyc_Absyn_StructType_struct _tmpE6;_tmpE6.tag=Cyc_Absyn_StructType;_tmpE6.f1=
_tmpC8;_tmpE6.f2=_tmpC6;_tmpE6.f3=_tmpE3;_tmpE6;});_tmpE5;});return(void*)_tmpE4;}}}
_LL191: if(_tmpCA == 0){({void*_tmpE9[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("struct decl must exist at this point",
sizeof(unsigned char),37),_tag_arr(_tmpE9,sizeof(void*),0));});}{struct Cyc_Absyn_Uniondecl*
_tmpEA=*((struct Cyc_Absyn_Uniondecl**)_check_null(_tmpCA));struct Cyc_List_List*
_tmpEB=_tmpEA->tvs;if(Cyc_List_length(_tmpCC)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmpEB)){({struct Cyc_Std_Int_pa_struct _tmpEE;_tmpEE.tag=Cyc_Std_Int_pa;
_tmpEE.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpEB));{struct Cyc_Std_Int_pa_struct _tmpED;_tmpED.tag=Cyc_Std_Int_pa;_tmpED.f1=(
int)((unsigned int)Cyc_List_length(_tmpCC));{void*_tmpEC[2]={& _tmpED,& _tmpEE};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof(unsigned char),60),_tag_arr(_tmpEC,sizeof(void*),2));}}});}{struct Cyc_List_List*
_tmpEF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmpEB,_tmpCC);struct Cyc_Core_Opt*fields=0;if(_tmpEA->fields != 0){
fields=({struct Cyc_Core_Opt*_tmpF0=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpF0->v=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_structfield_type,_tmpEF,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpEA->fields))->v);_tmpF0;});}{struct Cyc_Absyn_Uniondecl*
udecl2=({struct Cyc_Absyn_Uniondecl*_tmpF6=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl));
_tmpF6->sc=(void*)((void*)_tmpEA->sc);_tmpF6->name=_tmpEA->name;_tmpF6->tvs=0;
_tmpF6->fields=fields;_tmpF6->attributes=_tmpEA->attributes;_tmpF6;});struct Cyc_Absyn_Uniondecl**
_tmpF1=({struct Cyc_Absyn_Uniondecl**_tmpF5=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl*));
_tmpF5[0]=udecl2;_tmpF5;});struct Cyc_Absyn_UnionType_struct*_tmpF2=({struct Cyc_Absyn_UnionType_struct*
_tmpF3=_cycalloc(sizeof(struct Cyc_Absyn_UnionType_struct));_tmpF3[0]=({struct Cyc_Absyn_UnionType_struct
_tmpF4;_tmpF4.tag=Cyc_Absyn_UnionType;_tmpF4.f1=_tmpCE;_tmpF4.f2=_tmpCC;_tmpF4.f3=
_tmpF1;_tmpF4;});_tmpF3;});return(void*)_tmpF2;}}}_LL193: {struct Cyc_List_List*
_tmpF7=_tmpDA->tvs;if(Cyc_List_length(_tmpD4)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmpF7)){({struct Cyc_Std_Int_pa_struct _tmpFA;_tmpFA.tag=Cyc_Std_Int_pa;
_tmpFA.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpF7));{struct Cyc_Std_Int_pa_struct _tmpF9;_tmpF9.tag=Cyc_Std_Int_pa;_tmpF9.f1=(
int)((unsigned int)Cyc_List_length(_tmpD4));{void*_tmpF8[2]={& _tmpF9,& _tmpFA};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof(unsigned char),60),_tag_arr(_tmpF8,sizeof(void*),2));}}});}{struct Cyc_List_List*
_tmpFB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmpF7,_tmpD4);struct Cyc_Core_Opt*fields=0;if(_tmpDA->fields != 0){
fields=({struct Cyc_Core_Opt*_tmpFC=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpFC->v=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tunionfield*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_tunionfield_type,_tmpFB,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpDA->fields))->v);_tmpFC;});}{struct Cyc_Absyn_Tuniondecl*
_tmpFD=({struct Cyc_Absyn_Tuniondecl*_tmp106=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));
_tmp106->sc=(void*)((void*)_tmpDA->sc);_tmp106->name=_tmpDA->name;_tmp106->tvs=0;
_tmp106->fields=fields;_tmp106->is_xtunion=_tmpDA->is_xtunion;_tmp106;});struct
Cyc_Absyn_TunionType_struct*_tmpFE=({struct Cyc_Absyn_TunionType_struct*_tmp100=
_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));_tmp100[0]=({struct Cyc_Absyn_TunionType_struct
_tmp101;_tmp101.tag=Cyc_Absyn_TunionType;_tmp101.f1=({struct Cyc_Absyn_TunionInfo
_tmp102;_tmp102.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp103=_cycalloc(sizeof(struct Cyc_Absyn_KnownTunion_struct));_tmp103[0]=({
struct Cyc_Absyn_KnownTunion_struct _tmp104;_tmp104.tag=Cyc_Absyn_KnownTunion;
_tmp104.f1=({struct Cyc_Absyn_Tuniondecl**_tmp105=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl*));
_tmp105[0]=_tmpFD;_tmp105;});_tmp104;});_tmp103;}));_tmp102.targs=_tmpD4;_tmp102.rgn=(
void*)_tmpD2;_tmp102;});_tmp101;});_tmp100;});struct Cyc_Position_Segment*_tmpFF=
Cyc_Position_segment_of_abs(0,0);return(void*)_tmpFE;}}}_LL195: return type;_LL187:;}
struct _tagged_arr Cyc_Tcgenrep_make_type_cstring(void*t){struct _tagged_arr s=Cyc_Std_strdup(
Cyc_Absynpp_typ2cstring(t));{int i=0;for(0;i < Cyc_Std_strlen((struct _tagged_arr)s);
i ++){if(((unsigned char*)s.curr)[i]== ' '){((unsigned char*)s.curr)[i]='_';}
else{if(! isalnum((int)((unsigned char*)s.curr)[i])?((unsigned char*)s.curr)[i]!= '_':
0){((unsigned char*)s.curr)[i]=(unsigned char)('0' + ((unsigned char*)s.curr)[i]% 
10);}}}}return(struct _tagged_arr)Cyc_Std_strconcat((struct _tagged_arr)s,_tag_arr("_rep",
sizeof(unsigned char),5));}struct _tuple12{struct Cyc_Dict_Dict*f1;struct Cyc_Tcgenrep_RepInfo*
f2;};static struct _tuple12*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,void*type);struct _tuple13{
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};struct _tuple14{struct Cyc_Tcenv_Tenv*
f1;struct Cyc_Position_Segment*f2;};static struct _tuple13*Cyc_Tcgenrep_lookupRepsCls(
struct _tuple14*env,void*type,struct _tuple13*carry){struct _tuple12 _tmp109;struct
Cyc_Tcgenrep_RepInfo*_tmp10A;struct Cyc_Dict_Dict*_tmp10C;struct _tuple12*_tmp107=
Cyc_Tcgenrep_lookupRep((*env).f1,(*carry).f1,(*env).f2,type);_tmp109=*_tmp107;
_LL269: _tmp10C=_tmp109.f1;goto _LL267;_LL267: _tmp10A=_tmp109.f2;goto _LL264;_LL264:
return({struct _tuple13*_tmp10E=_cycalloc(sizeof(struct _tuple13));_tmp10E->f1=
_tmp10C;_tmp10E->f2=({struct Cyc_List_List*_tmp10F=_cycalloc(sizeof(struct Cyc_List_List));
_tmp10F->hd=_tmp10A;_tmp10F->tl=(*carry).f2;_tmp10F;});_tmp10E;});}static struct
Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*
dict,struct Cyc_Position_Segment*loc,struct _tuple3*varname,void*sc,void*type,
struct Cyc_List_List*types){struct Cyc_Absyn_Exp*_tmp110=Cyc_Absyn_sizeoftyp_exp(
type,loc);struct _tuple14*_tmp111=({struct _tuple14*_tmp12C=_cycalloc(sizeof(
struct _tuple14));_tmp12C->f1=te;_tmp12C->f2=loc;_tmp12C;});struct _tuple13*
_tmp112=({struct _tuple13*_tmp12B=_cycalloc(sizeof(struct _tuple13));_tmp12B->f1=
dict;_tmp12B->f2=0;_tmp12B;});struct _tuple13 _tmp115;struct Cyc_List_List*_tmp116;
struct Cyc_Dict_Dict*_tmp118;struct _tuple13*_tmp113=((struct _tuple13*(*)(struct
_tuple13*(*f)(struct _tuple14*,void*,struct _tuple13*),struct _tuple14*,struct Cyc_List_List*
x,struct _tuple13*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp111,types,_tmp112);_tmp115=*_tmp113;_LL281: _tmp118=_tmp115.f1;goto _LL279;
_LL279: _tmp116=_tmp115.f2;goto _LL276;_LL276: dict=_tmp118;{struct Cyc_List_List*
_tmp11A=((struct Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(Cyc_List_length(
types),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp11B=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple9*,int),struct _tuple9*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({struct _tuple9*_tmp12A=
_cycalloc(sizeof(struct _tuple9));_tmp12A->f1=type;_tmp12A->f2=loc;_tmp12A;}),
_tmp11A);struct Cyc_List_List*_tmp11C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp116);struct Cyc_List_List*_tmp11D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp11B,_tmp11C));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmp129[2];
_tmp129[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp129[0]=Cyc_Absyn_uint_t;Cyc_List_list(_tag_arr(_tmp129,sizeof(void*),2));}));
struct _tuple10 _tmp120;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_List_List*_tmp123;
struct _tuple10*_tmp11E=Cyc_Tcgenrep_array_decls(tuple_type,_tmp11D,loc);_tmp120=*
_tmp11E;_LL292: _tmp123=_tmp120.f1;goto _LL290;_LL290: _tmp121=_tmp120.f2;goto
_LL287;_LL287: {struct Cyc_Absyn_Decl*_tmp125=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tuple_str),varname,({struct Cyc_Absyn_Exp*_tmp128[2];_tmp128[1]=
_tmp121;_tmp128[0]=_tmp110;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp128,sizeof(struct Cyc_Absyn_Exp*),2));}),sc,loc);struct Cyc_List_List*
_tmp126=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp123,({struct Cyc_Absyn_Decl*_tmp127[1];_tmp127[0]=
_tmp125;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp127,sizeof(struct Cyc_Absyn_Decl*),1));}));return Cyc_Tcgenrep_update_info(
dict,type,_tmp126,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp116,0);}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepUnion(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,
struct _tuple3*varname,void*sc,void*type,struct Cyc_List_List*types){struct
_tuple14*_tmp12D=({struct _tuple14*_tmp142=_cycalloc(sizeof(struct _tuple14));
_tmp142->f1=te;_tmp142->f2=loc;_tmp142;});struct _tuple13*_tmp12E=({struct
_tuple13*_tmp141=_cycalloc(sizeof(struct _tuple13));_tmp141->f1=dict;_tmp141->f2=
0;_tmp141;});struct _tuple13 _tmp131;struct Cyc_List_List*_tmp132;struct Cyc_Dict_Dict*
_tmp134;struct _tuple13*_tmp12F=((struct _tuple13*(*)(struct _tuple13*(*f)(struct
_tuple14*,void*,struct _tuple13*),struct _tuple14*,struct Cyc_List_List*x,struct
_tuple13*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp12D,types,
_tmp12E);_tmp131=*_tmp12F;_LL309: _tmp134=_tmp131.f1;goto _LL307;_LL307: _tmp132=
_tmp131.f2;goto _LL304;_LL304: dict=_tmp134;{struct Cyc_List_List*_tmp136=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,
loc,_tmp132);struct _tuple10 _tmp139;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_List_List*
_tmp13C;struct _tuple10*_tmp137=Cyc_Tcgenrep_array_decls(Cyc_Tcgenrep_tunion_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str)),_tmp136,loc);_tmp139=*
_tmp137;_LL317: _tmp13C=_tmp139.f1;goto _LL315;_LL315: _tmp13A=_tmp139.f2;goto
_LL312;_LL312: {struct Cyc_Absyn_Decl*_tmp13E=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_union_str),varname,({struct Cyc_Absyn_Exp*_tmp140[1];_tmp140[0]=
_tmp13A;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp140,sizeof(struct Cyc_Absyn_Exp*),1));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp13C,({struct Cyc_Absyn_Decl*_tmp13F[1];_tmp13F[0]=
_tmp13E;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp13F,sizeof(struct Cyc_Absyn_Decl*),1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp132,0);}}}static struct Cyc_Dict_Dict*
Cyc_Tcgenrep_buildRepTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,
struct Cyc_Position_Segment*loc,struct _tuple3*varname,void*sc,void*type,struct Cyc_List_List*
types){struct _tuple14*_tmp143=({struct _tuple14*_tmp15D=_cycalloc(sizeof(struct
_tuple14));_tmp15D->f1=te;_tmp15D->f2=loc;_tmp15D;});struct _tuple13*_tmp144=({
struct _tuple13*_tmp15C=_cycalloc(sizeof(struct _tuple13));_tmp15C->f1=dict;
_tmp15C->f2=0;_tmp15C;});struct _tuple13 _tmp147;struct Cyc_List_List*_tmp148;
struct Cyc_Dict_Dict*_tmp14A;struct _tuple13*_tmp145=((struct _tuple13*(*)(struct
_tuple13*(*f)(struct _tuple14*,void*,struct _tuple13*),struct _tuple14*,struct Cyc_List_List*
x,struct _tuple13*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp143,types,_tmp144);_tmp147=*_tmp145;_LL331: _tmp14A=_tmp147.f1;goto _LL329;
_LL329: _tmp148=_tmp147.f2;goto _LL326;_LL326: dict=_tmp14A;{struct Cyc_List_List*
_tmp14C=((struct Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp148),(int(*)(int))Cyc_Core_identity);
struct Cyc_List_List*_tmp14D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp14C);struct Cyc_List_List*
_tmp14E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,_tmp148);struct Cyc_List_List*
_tmp14F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmp14D,_tmp14E);struct Cyc_List_List*_tmp150=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,
_tmp14F);void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmp15B[2];_tmp15B[1]=Cyc_Tcgenrep_tunion_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));_tmp15B[0]=Cyc_Absyn_uint_t;
Cyc_List_list(_tag_arr(_tmp15B,sizeof(void*),2));}));struct _tuple10 _tmp153;
struct Cyc_Absyn_Exp*_tmp154;struct Cyc_List_List*_tmp156;struct _tuple10*_tmp151=
Cyc_Tcgenrep_array_decls(tuple_type,_tmp150,loc);_tmp153=*_tmp151;_LL343: _tmp156=
_tmp153.f1;goto _LL341;_LL341: _tmp154=_tmp153.f2;goto _LL338;_LL338: {struct Cyc_Absyn_Decl*
_tmp158=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str),varname,({struct Cyc_Absyn_Exp*
_tmp15A[1];_tmp15A[0]=_tmp154;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp15A,sizeof(struct Cyc_Absyn_Exp*),1));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp156,({struct Cyc_Absyn_Decl*_tmp159[1];_tmp159[0]=
_tmp158;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp159,sizeof(struct Cyc_Absyn_Decl*),1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp148,0);return dict;}}}static struct
Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepXTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*
dict,struct Cyc_Position_Segment*loc,struct _tuple3*varname,void*sc,void*type,
struct Cyc_List_List*nametypes){struct Cyc_List_List*_tmp160;struct Cyc_List_List*
_tmp162;struct _tuple0 _tmp15E=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
nametypes);_LL355: _tmp162=_tmp15E.f1;goto _LL353;_LL353: _tmp160=_tmp15E.f2;goto
_LL351;_LL351: {struct _tuple14*_tmp164=({struct _tuple14*_tmp17E=_cycalloc(
sizeof(struct _tuple14));_tmp17E->f1=te;_tmp17E->f2=loc;_tmp17E;});struct _tuple13*
_tmp165=({struct _tuple13*_tmp17D=_cycalloc(sizeof(struct _tuple13));_tmp17D->f1=
dict;_tmp17D->f2=0;_tmp17D;});struct _tuple13 _tmp168;struct Cyc_List_List*_tmp169;
struct Cyc_Dict_Dict*_tmp16B;struct _tuple13*_tmp166=((struct _tuple13*(*)(struct
_tuple13*(*f)(struct _tuple14*,void*,struct _tuple13*),struct _tuple14*,struct Cyc_List_List*
x,struct _tuple13*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp164,_tmp160,_tmp165);_tmp168=*_tmp166;_LL364: _tmp16B=_tmp168.f1;goto _LL362;
_LL362: _tmp169=_tmp168.f2;goto _LL359;_LL359: dict=_tmp16B;{struct Cyc_List_List*
_tmp16D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tagged_arr*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_cnst_string_cls,loc,_tmp162);struct Cyc_List_List*_tmp16E=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,
loc,_tmp169);struct Cyc_List_List*_tmp16F=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp16D,_tmp16E);struct Cyc_List_List*
_tmp170=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp16F);void*name_type=Cyc_Absyn_tagged_typ(Cyc_Absyn_uchar_t,(
void*)Cyc_Absyn_HeapRgn,({struct Cyc_Absyn_Tqual _tmp17C;_tmp17C.q_const=1;_tmp17C.q_volatile=
0;_tmp17C.q_restrict=0;_tmp17C;}));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*
_tmp17B[2];_tmp17B[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp17B[0]=name_type;Cyc_List_list(_tag_arr(_tmp17B,sizeof(void*),2));}));struct
_tuple10 _tmp173;struct Cyc_Absyn_Exp*_tmp174;struct Cyc_List_List*_tmp176;struct
_tuple10*_tmp171=Cyc_Tcgenrep_array_decls(tuple_type,_tmp170,loc);_tmp173=*
_tmp171;_LL375: _tmp176=_tmp173.f1;goto _LL373;_LL373: _tmp174=_tmp173.f2;goto
_LL370;_LL370: {struct Cyc_Absyn_Decl*_tmp178=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_xtunion_str),varname,({struct Cyc_Absyn_Exp*_tmp17A[1];_tmp17A[0]=
_tmp174;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp17A,sizeof(struct Cyc_Absyn_Exp*),1));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp176,({struct Cyc_Absyn_Decl*_tmp179[1];_tmp179[0]=
_tmp178;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp179,sizeof(struct Cyc_Absyn_Decl*),1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp169,0);return dict;}}}}static struct
_tuple12*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,
struct Cyc_Position_Segment*loc,void*type){struct Cyc_Core_Opt*info=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(dict,type);if(info != 0){if(((
struct Cyc_Tcgenrep_RepInfo*)info->v)->is_extern){dict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*,void*))Cyc_Dict_delete)(dict,type);}else{struct _tuple12*
_tmp17F=({struct _tuple12*_tmp180=_cycalloc(sizeof(struct _tuple12));_tmp180->f1=
dict;_tmp180->f2=(struct Cyc_Tcgenrep_RepInfo*)info->v;_tmp180;});return _tmp17F;}}{
void*_tmp181=Cyc_Tcutil_compress(Cyc_Tcgenrep_monomorphize_type(type));void*
_tmp1B7;int _tmp1B9;struct Cyc_Absyn_PtrInfo _tmp1BB;struct Cyc_Absyn_Exp*_tmp1BD;
struct Cyc_Absyn_Tqual _tmp1BF;void*_tmp1C1;struct Cyc_List_List*_tmp1C3;struct Cyc_Core_Opt*
_tmp1C5;struct Cyc_List_List*_tmp1C7;struct _tuple3*_tmp1C9;struct Cyc_Absyn_Structdecl**
_tmp1CB;struct Cyc_List_List*_tmp1CD;struct _tuple3*_tmp1CF;struct Cyc_List_List*
_tmp1D1;struct Cyc_Absyn_Tvar*_tmp1D3;struct Cyc_Absyn_FnInfo _tmp1D5;struct Cyc_Absyn_TunionInfo
_tmp1D7;void*_tmp1D9;struct Cyc_List_List*_tmp1DB;void*_tmp1DD;struct Cyc_Absyn_Tuniondecl**
_tmp1DF;struct Cyc_Absyn_Tuniondecl*_tmp1E1;struct Cyc_Absyn_TunionFieldInfo
_tmp1E2;struct Cyc_Absyn_Uniondecl**_tmp1E4;struct Cyc_List_List*_tmp1E6;struct
_tuple3*_tmp1E8;struct Cyc_List_List*_tmp1EA;_LL387: if((unsigned int)_tmp181 > 3?*((
int*)_tmp181)== Cyc_Absyn_IntType: 0){_LL440: _tmp1B7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp181)->f2;goto _LL388;}else{goto _LL389;}_LL389: if(_tmp181 == (void*)Cyc_Absyn_FloatType){
goto _LL390;}else{goto _LL391;}_LL391: if((unsigned int)_tmp181 > 3?*((int*)_tmp181)
== Cyc_Absyn_DoubleType: 0){_LL442: _tmp1B9=((struct Cyc_Absyn_DoubleType_struct*)
_tmp181)->f1;goto _LL392;}else{goto _LL393;}_LL393: if((unsigned int)_tmp181 > 3?*((
int*)_tmp181)== Cyc_Absyn_PointerType: 0){_LL444: _tmp1BB=((struct Cyc_Absyn_PointerType_struct*)
_tmp181)->f1;goto _LL394;}else{goto _LL395;}_LL395: if((unsigned int)_tmp181 > 3?*((
int*)_tmp181)== Cyc_Absyn_ArrayType: 0){_LL450: _tmp1C1=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp181)->f1;goto _LL448;_LL448: _tmp1BF=((struct Cyc_Absyn_ArrayType_struct*)
_tmp181)->f2;goto _LL446;_LL446: _tmp1BD=((struct Cyc_Absyn_ArrayType_struct*)
_tmp181)->f3;goto _LL396;}else{goto _LL397;}_LL397: if((unsigned int)_tmp181 > 3?*((
int*)_tmp181)== Cyc_Absyn_TupleType: 0){_LL452: _tmp1C3=((struct Cyc_Absyn_TupleType_struct*)
_tmp181)->f1;goto _LL398;}else{goto _LL399;}_LL399: if((unsigned int)_tmp181 > 3?*((
int*)_tmp181)== Cyc_Absyn_TypedefType: 0){_LL458: _tmp1C9=((struct Cyc_Absyn_TypedefType_struct*)
_tmp181)->f1;goto _LL456;_LL456: _tmp1C7=((struct Cyc_Absyn_TypedefType_struct*)
_tmp181)->f2;goto _LL454;_LL454: _tmp1C5=((struct Cyc_Absyn_TypedefType_struct*)
_tmp181)->f3;goto _LL400;}else{goto _LL401;}_LL401: if((unsigned int)_tmp181 > 3?*((
int*)_tmp181)== Cyc_Absyn_StructType: 0){_LL464: _tmp1CF=((struct Cyc_Absyn_StructType_struct*)
_tmp181)->f1;goto _LL462;_LL462: _tmp1CD=((struct Cyc_Absyn_StructType_struct*)
_tmp181)->f2;goto _LL460;_LL460: _tmp1CB=((struct Cyc_Absyn_StructType_struct*)
_tmp181)->f3;goto _LL402;}else{goto _LL403;}_LL403: if((unsigned int)_tmp181 > 3?*((
int*)_tmp181)== Cyc_Absyn_AnonStructType: 0){_LL466: _tmp1D1=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp181)->f1;goto _LL404;}else{goto _LL405;}_LL405: if(_tmp181 == (void*)Cyc_Absyn_VoidType){
goto _LL406;}else{goto _LL407;}_LL407: if((unsigned int)_tmp181 > 3?*((int*)_tmp181)
== Cyc_Absyn_Evar: 0){goto _LL408;}else{goto _LL409;}_LL409: if((unsigned int)
_tmp181 > 3?*((int*)_tmp181)== Cyc_Absyn_VarType: 0){_LL468: _tmp1D3=((struct Cyc_Absyn_VarType_struct*)
_tmp181)->f1;goto _LL410;}else{goto _LL411;}_LL411: if((unsigned int)_tmp181 > 3?*((
int*)_tmp181)== Cyc_Absyn_FnType: 0){_LL470: _tmp1D5=((struct Cyc_Absyn_FnType_struct*)
_tmp181)->f1;goto _LL412;}else{goto _LL413;}_LL413: if((unsigned int)_tmp181 > 3?*((
int*)_tmp181)== Cyc_Absyn_RgnHandleType: 0){goto _LL414;}else{goto _LL415;}_LL415:
if(_tmp181 == (void*)Cyc_Absyn_HeapRgn){goto _LL416;}else{goto _LL417;}_LL417: if((
unsigned int)_tmp181 > 3?*((int*)_tmp181)== Cyc_Absyn_AccessEff: 0){goto _LL418;}
else{goto _LL419;}_LL419: if((unsigned int)_tmp181 > 3?*((int*)_tmp181)== Cyc_Absyn_JoinEff:
0){goto _LL420;}else{goto _LL421;}_LL421: if((unsigned int)_tmp181 > 3?*((int*)
_tmp181)== Cyc_Absyn_RgnsEff: 0){goto _LL422;}else{goto _LL423;}_LL423: if((
unsigned int)_tmp181 > 3?*((int*)_tmp181)== Cyc_Absyn_TunionType: 0){_LL472:
_tmp1D7=((struct Cyc_Absyn_TunionType_struct*)_tmp181)->f1;_LL478: _tmp1DD=(void*)
_tmp1D7.tunion_info;if(*((int*)_tmp1DD)== Cyc_Absyn_KnownTunion){_LL480: _tmp1DF=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp1DD)->f1;_tmp1E1=*_tmp1DF;goto _LL476;}
else{goto _LL425;}_LL476: _tmp1DB=_tmp1D7.targs;goto _LL474;_LL474: _tmp1D9=(void*)
_tmp1D7.rgn;goto _LL424;}else{goto _LL425;}_LL425: if((unsigned int)_tmp181 > 3?*((
int*)_tmp181)== Cyc_Absyn_TunionType: 0){goto _LL426;}else{goto _LL427;}_LL427: if((
unsigned int)_tmp181 > 3?*((int*)_tmp181)== Cyc_Absyn_TunionFieldType: 0){_LL483:
_tmp1E2=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp181)->f1;goto _LL428;}else{
goto _LL429;}_LL429: if((unsigned int)_tmp181 > 3?*((int*)_tmp181)== Cyc_Absyn_UnionType:
0){_LL489: _tmp1E8=((struct Cyc_Absyn_UnionType_struct*)_tmp181)->f1;goto _LL487;
_LL487: _tmp1E6=((struct Cyc_Absyn_UnionType_struct*)_tmp181)->f2;goto _LL485;
_LL485: _tmp1E4=((struct Cyc_Absyn_UnionType_struct*)_tmp181)->f3;goto _LL430;}
else{goto _LL431;}_LL431: if((unsigned int)_tmp181 > 3?*((int*)_tmp181)== Cyc_Absyn_AnonUnionType:
0){_LL491: _tmp1EA=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp181)->f1;goto
_LL432;}else{goto _LL433;}_LL433: if((unsigned int)_tmp181 > 3?*((int*)_tmp181)== 
Cyc_Absyn_EnumType: 0){goto _LL434;}else{goto _LL435;}_LL435: if((unsigned int)
_tmp181 > 3?*((int*)_tmp181)== Cyc_Absyn_AnonEnumType: 0){goto _LL436;}else{goto
_LL437;}_LL437: if((unsigned int)_tmp181 > 3?*((int*)_tmp181)== Cyc_Absyn_SizeofType:
0){goto _LL438;}else{goto _LL386;}_LL388: {struct _tuple3*_tmp1EC=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",sizeof(unsigned char),4)));struct Cyc_Absyn_Decl*
_tmp1ED=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_int_str),_tmp1EC,({struct Cyc_Absyn_Exp*
_tmp1EF[1];_tmp1EF[0]=Cyc_Tcgenrep_cnst_int(Cyc_Tcgenrep_size_of2int(_tmp1B7),
loc);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp1EF,sizeof(struct Cyc_Absyn_Exp*),1));}),(void*)Cyc_Absyn_Static,loc);dict=
Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*_tmp1EE[1];_tmp1EE[0]=
_tmp1ED;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp1EE,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1EC,loc),loc),0,0);goto _LL386;}_LL390: dict=Cyc_Tcgenrep_update_info(
dict,type,0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_float_str),loc),0,0);goto _LL386;_LL392: dict=Cyc_Tcgenrep_update_info(
dict,type,0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_double_str),loc),0,0);goto _LL386;_LL394:{void*_tmp1F0=Cyc_Absyn_conref_val(
_tmp1BB.bounds);struct Cyc_Absyn_Exp*_tmp1F6;_LL498: if((unsigned int)_tmp1F0 > 1?*((
int*)_tmp1F0)== Cyc_Absyn_Upper_b: 0){_LL503: _tmp1F6=((struct Cyc_Absyn_Upper_b_struct*)
_tmp1F0)->f1;goto _LL499;}else{goto _LL500;}_LL500: if(_tmp1F0 == (void*)Cyc_Absyn_Unknown_b){
goto _LL501;}else{goto _LL497;}_LL499: {struct _tuple3*_tmp1F8=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",sizeof(unsigned char),4)));struct Cyc_Core_Opt*
_tmp1F9=({struct Cyc_Core_Opt*_tmp205=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp205->v=Cyc_Tcgenrep_gen_vardecl(_tmp1F8,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str)),
0,(void*)Cyc_Absyn_Extern,loc);_tmp205;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1F8,loc),loc),_tmp1F9,0);{struct _tuple12 _tmp1FC;struct Cyc_Tcgenrep_RepInfo*
_tmp1FD;struct Cyc_Dict_Dict*_tmp1FF;struct _tuple12*_tmp1FA=Cyc_Tcgenrep_lookupRep(
te,dict,loc,(void*)_tmp1BB.elt_typ);_tmp1FC=*_tmp1FA;_LL512: _tmp1FF=_tmp1FC.f1;
goto _LL510;_LL510: _tmp1FD=_tmp1FC.f2;goto _LL507;_LL507: dict=_tmp1FF;{
unsigned int bound=Cyc_Evexp_eval_const_uint_exp(_tmp1F6);struct Cyc_Absyn_Decl*
_tmp201=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str),_tmp1F8,({struct Cyc_Absyn_Exp*
_tmp204[2];_tmp204[1]=(struct Cyc_Absyn_Exp*)_check_null(_tmp1FD->exp);_tmp204[0]=
Cyc_Tcgenrep_cnst_int((int)bound,loc);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp204,sizeof(struct Cyc_Absyn_Exp*),2));}),(
void*)Cyc_Absyn_Static,loc);dict=Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*
_tmp202[1];_tmp202[0]=_tmp201;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp202,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1F8,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp203[1];
_tmp203[0]=_tmp1FD;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp203,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL497;}}}
_LL501: {struct _tuple3*_tmp206=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(unsigned char),4)));struct Cyc_Core_Opt*_tmp207=({struct Cyc_Core_Opt*
_tmp213=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp213->v=Cyc_Tcgenrep_gen_vardecl(
_tmp206,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str)),0,(void*)Cyc_Absyn_Extern,
loc);_tmp213;});dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp206,loc),loc),_tmp207,0);{
struct _tuple12 _tmp20A;struct Cyc_Tcgenrep_RepInfo*_tmp20B;struct Cyc_Dict_Dict*
_tmp20D;struct _tuple12*_tmp208=Cyc_Tcgenrep_lookupRep(te,dict,loc,(void*)_tmp1BB.elt_typ);
_tmp20A=*_tmp208;_LL526: _tmp20D=_tmp20A.f1;goto _LL524;_LL524: _tmp20B=_tmp20A.f2;
goto _LL521;_LL521: dict=_tmp20D;{struct Cyc_Absyn_Decl*_tmp20F=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_fatptr_str),_tmp206,({struct Cyc_Absyn_Exp*_tmp212[1];_tmp212[0]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp20B->exp);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp212,sizeof(struct Cyc_Absyn_Exp*),1));}),(
void*)Cyc_Absyn_Static,loc);dict=Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*
_tmp210[1];_tmp210[0]=_tmp20F;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp210,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp206,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp211[1];
_tmp211[0]=_tmp20B;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp211,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL497;}}}
_LL497:;}goto _LL386;_LL396: if(_tmp1BD == 0){({void*_tmp214[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcgenrep:At this point, array bounds must be constants",
sizeof(unsigned char),55),_tag_arr(_tmp214,sizeof(void*),0));});}{struct _tuple12
_tmp217;struct Cyc_Tcgenrep_RepInfo*_tmp218;struct Cyc_Dict_Dict*_tmp21A;struct
_tuple12*_tmp215=Cyc_Tcgenrep_lookupRep(te,dict,loc,_tmp1C1);_tmp217=*_tmp215;
_LL539: _tmp21A=_tmp217.f1;goto _LL537;_LL537: _tmp218=_tmp217.f2;goto _LL534;_LL534:
dict=_tmp21A;{unsigned int bound=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp1BD));struct _tuple3*_tmp21C=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(unsigned char),4)));struct Cyc_Absyn_Decl*_tmp21D=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str),_tmp21C,({struct Cyc_Absyn_Exp*_tmp220[2];_tmp220[1]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp218->exp);_tmp220[0]=Cyc_Tcgenrep_cnst_int((
int)bound,loc);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp220,sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)Cyc_Absyn_Static,loc);
dict=Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*_tmp21E[1];
_tmp21E[0]=_tmp21D;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp21E,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp21C,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp21F[1];
_tmp21F[0]=_tmp218;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp21F,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL386;}}
_LL398: {struct _tuple3*_tmp221=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(unsigned char),4)));struct Cyc_Core_Opt*_tmp222=({struct Cyc_Core_Opt*
_tmp224=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp224->v=Cyc_Tcgenrep_gen_vardecl(
_tmp221,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str)),0,(void*)Cyc_Absyn_Extern,
loc);_tmp224;});dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp221,loc),loc),_tmp222,0);{
struct Cyc_List_List*_tmp223=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple6*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_second,_tmp1C3);dict=Cyc_Tcgenrep_buildRepTuple(
te,dict,loc,_tmp221,(void*)Cyc_Absyn_Static,type,_tmp223);goto _LL386;}}_LL400:
if(_tmp1C5 == 0){({void*_tmp225[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): typedef definition must be present by now",
sizeof(unsigned char),49),_tag_arr(_tmp225,sizeof(void*),0));});}{struct _tuple12
_tmp228;struct Cyc_Tcgenrep_RepInfo*_tmp229;struct Cyc_Dict_Dict*_tmp22B;struct
_tuple12*_tmp226=Cyc_Tcgenrep_lookupRep(te,dict,loc,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1C5))->v);_tmp228=*_tmp226;_LL556: _tmp22B=_tmp228.f1;goto _LL554;
_LL554: _tmp229=_tmp228.f2;goto _LL551;_LL551: dict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(
_tmp22B,type,_tmp229);goto _LL386;}_LL402: if(_tmp1CB == 0){({void*_tmp22D[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): struct declaration must be present by now",
sizeof(unsigned char),49),_tag_arr(_tmp22D,sizeof(void*),0));});}{struct Cyc_Absyn_Structdecl*
_tmp22E=*((struct Cyc_Absyn_Structdecl**)_check_null(_tmp1CB));if(_tmp22E->fields
== 0){struct _tuple3*_tmp22F=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*
_tmp232=_cycalloc(sizeof(struct _tagged_arr));_tmp232[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp232;}));struct Cyc_Core_Opt*_tmp230=({struct Cyc_Core_Opt*_tmp231=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp231->v=Cyc_Tcgenrep_gen_vardecl(
_tmp22F,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str)),0,(void*)Cyc_Absyn_Extern,
loc);_tmp231;});dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp22F,loc),loc),0,1);goto _LL386;}
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp22E->fields))->v
!= 0?((int(*)(int(*pred)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))
Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp22E->fields))->v): 0){int _tmp233=((int(*)(int(*f)(int,struct Cyc_Absyn_Structfield*),
int accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,
0,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp22E->fields))->v);
int _tmp234=_tmp233 / 8 + (_tmp233 % 8 == 0? 0: 1);struct Cyc_List_List*chars=0;{int i=0;
for(0;i < _tmp234;i ++){chars=({struct Cyc_List_List*_tmp235=_cycalloc(sizeof(
struct Cyc_List_List));_tmp235->hd=({struct _tuple6*_tmp236=_cycalloc(sizeof(
struct _tuple6));_tmp236->f1=Cyc_Tcgenrep_tq_none;_tmp236->f2=Cyc_Absyn_uchar_t;
_tmp236;});_tmp235->tl=chars;_tmp235;});}}{void*base_type=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp23E=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_struct));_tmp23E[0]=({struct
Cyc_Absyn_TupleType_struct _tmp23F;_tmp23F.tag=Cyc_Absyn_TupleType;_tmp23F.f1=
chars;_tmp23F;});_tmp23E;});struct _tuple12 _tmp239;struct Cyc_Tcgenrep_RepInfo*
_tmp23A;struct Cyc_Dict_Dict*_tmp23C;struct _tuple12*_tmp237=Cyc_Tcgenrep_lookupRep(
te,dict,loc,base_type);_tmp239=*_tmp237;_LL573: _tmp23C=_tmp239.f1;goto _LL571;
_LL571: _tmp23A=_tmp239.f2;goto _LL568;_LL568: dict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(
_tmp23C,type,_tmp23A);}}else{struct _tuple3*_tmp240=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp244=_cycalloc(sizeof(struct _tagged_arr));_tmp244[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp244;}));struct Cyc_Core_Opt*_tmp241=({struct Cyc_Core_Opt*_tmp243=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp243->v=Cyc_Tcgenrep_gen_vardecl(
_tmp240,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str)),0,(void*)Cyc_Absyn_Extern,
loc);_tmp243;});dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp240,loc),loc),_tmp241,0);{
struct Cyc_List_List*_tmp242=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Structfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_type,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp22E->fields))->v);
dict=Cyc_Tcgenrep_buildRepTuple(te,dict,loc,_tmp240,(void*)Cyc_Absyn_Public,type,
_tmp242);}}goto _LL386;}_LL404: if(_tmp1D1 != 0?((int(*)(int(*pred)(struct Cyc_Absyn_Structfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,_tmp1D1): 0){
int _tmp245=((int(*)(int(*f)(int,struct Cyc_Absyn_Structfield*),int accum,struct Cyc_List_List*
x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,0,_tmp1D1);int _tmp246=
_tmp245 / 8 + (_tmp245 % 8 == 0? 0: 1);void*base_type=(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp24E=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp24E[0]=({struct
Cyc_Absyn_ArrayType_struct _tmp24F;_tmp24F.tag=Cyc_Absyn_ArrayType;_tmp24F.f1=(
void*)Cyc_Absyn_uchar_t;_tmp24F.f2=Cyc_Tcgenrep_tq_none;_tmp24F.f3=(struct Cyc_Absyn_Exp*)
Cyc_Tcgenrep_cnst_int(_tmp246,loc);_tmp24F;});_tmp24E;});struct _tuple12 _tmp249;
struct Cyc_Tcgenrep_RepInfo*_tmp24A;struct Cyc_Dict_Dict*_tmp24C;struct _tuple12*
_tmp247=Cyc_Tcgenrep_lookupRep(te,dict,loc,base_type);_tmp249=*_tmp247;_LL589:
_tmp24C=_tmp249.f1;goto _LL587;_LL587: _tmp24A=_tmp249.f2;goto _LL584;_LL584: dict=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(_tmp24C,type,_tmp24A);}else{struct Cyc_List_List*_tmp250=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_type,_tmp1D1);struct _tuple3*
_tmp251=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",
sizeof(unsigned char),4)));dict=Cyc_Tcgenrep_buildRepTuple(te,dict,loc,_tmp251,(
void*)Cyc_Absyn_Static,type,_tmp250);}goto _LL386;_LL406:({void*_tmp252[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("found void in gen() expression",sizeof(unsigned char),31),_tag_arr(
_tmp252,sizeof(void*),0));});goto _LL386;_LL408:({void*_tmp253[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("found evar in gen() expression",sizeof(unsigned char),31),_tag_arr(
_tmp253,sizeof(void*),0));});goto _LL386;_LL410:({void*_tmp254[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("found tyvar in gen() expression",sizeof(unsigned char),32),
_tag_arr(_tmp254,sizeof(void*),0));});goto _LL386;({void*_tmp255[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("found variable type in gen() expression",sizeof(unsigned char),40),
_tag_arr(_tmp255,sizeof(void*),0));});goto _LL386;_LL412:({void*_tmp256[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("found function type in gen() expression",sizeof(unsigned char),40),
_tag_arr(_tmp256,sizeof(void*),0));});goto _LL386;_LL414: goto _LL416;_LL416: goto
_LL418;_LL418: goto _LL420;_LL420: goto _LL422;_LL422:({void*_tmp257[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("gen(): unhandled region, handle or effect type",sizeof(
unsigned char),47),_tag_arr(_tmp257,sizeof(void*),0));});goto _LL386;_LL424: if(
_tmp1E1->tvs != 0){({void*_tmp258[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunion type variables not handled yet",
sizeof(unsigned char),45),_tag_arr(_tmp258,sizeof(void*),0));});}if(_tmp1E1->fields
== 0){struct _tuple3*_tmp259=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*
_tmp25C=_cycalloc(sizeof(struct _tagged_arr));_tmp25C[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp25C;}));struct Cyc_Core_Opt*_tmp25A=({struct Cyc_Core_Opt*_tmp25B=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp25B->v=Cyc_Tcgenrep_gen_vardecl(
_tmp259,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str)),0,(void*)Cyc_Absyn_Extern,
loc);_tmp25B;});dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp259,loc),loc),0,1);goto _LL386;}
if(! _tmp1E1->is_xtunion){struct _tuple3*_tmp25D=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp263=_cycalloc(sizeof(struct _tagged_arr));_tmp263[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp263;}));struct Cyc_Core_Opt*_tmp25E=({struct Cyc_Core_Opt*_tmp262=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp262->v=Cyc_Tcgenrep_gen_vardecl(
_tmp25D,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str)),0,(void*)Cyc_Absyn_Extern,
loc);_tmp262;});dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp25D,loc),loc),_tmp25E,0);{
struct Cyc_List_List*_tmp25F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1E1->fields))->v;struct Cyc_List_List*_tmp260=((struct Cyc_List_List*(*)(
int(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_filter)(
Cyc_Tcgenrep_filter_empty_tunionfield,_tmp25F);struct Cyc_List_List*_tmp261=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcgenrep_check_tunionfield_and_get_type,_tmp260);dict=Cyc_Tcgenrep_buildRepTunion(
te,dict,loc,_tmp25D,(void*)Cyc_Absyn_Public,type,_tmp261);}}else{struct _tuple3*
_tmp264=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp26A=_cycalloc(sizeof(
struct _tagged_arr));_tmp26A[0]=Cyc_Tcgenrep_make_type_cstring(type);_tmp26A;}));
struct Cyc_Core_Opt*_tmp265=({struct Cyc_Core_Opt*_tmp269=_cycalloc(sizeof(struct
Cyc_Core_Opt));_tmp269->v=Cyc_Tcgenrep_gen_vardecl(_tmp264,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_xtunion_str)),0,(void*)Cyc_Absyn_Extern,loc);_tmp269;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp264,loc),loc),_tmp265,0);{struct Cyc_List_List*_tmp266=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E1->fields))->v;struct Cyc_List_List*_tmp267=((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*
x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_tunionfield,_tmp266);struct Cyc_List_List*
_tmp268=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_xtunionfield_and_get_name_type,
_tmp267);dict=Cyc_Tcgenrep_buildRepXTunion(te,dict,loc,_tmp264,(void*)Cyc_Absyn_Public,
type,_tmp268);}}goto _LL386;_LL426:({void*_tmp26B[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunion must be resolved by now",
sizeof(unsigned char),38),_tag_arr(_tmp26B,sizeof(void*),0));});goto _LL386;
_LL428: if(_tmp1E2.targs != 0){({void*_tmp26C[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield type parameters not handled yet",
sizeof(unsigned char),51),_tag_arr(_tmp26C,sizeof(void*),0));});}{void*_tmp26D=(
void*)_tmp1E2.field_info;struct Cyc_Absyn_Tunionfield*_tmp273;struct Cyc_Absyn_Tuniondecl*
_tmp275;_LL623: if(*((int*)_tmp26D)== Cyc_Absyn_KnownTunionfield){_LL630: _tmp275=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp26D)->f1;goto _LL628;_LL628: _tmp273=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp26D)->f2;goto _LL624;}else{goto
_LL625;}_LL625: goto _LL626;_LL624: if(_tmp273->tvs != 0){({void*_tmp277[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield type parameters not handled yet",
sizeof(unsigned char),51),_tag_arr(_tmp277,sizeof(void*),0));});}{struct Cyc_List_List*
_tmp278=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple6*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcgenrep_get_second,_tmp273->typs);struct Cyc_List_List*
_tmp279=({struct Cyc_List_List*_tmp27B=_cycalloc(sizeof(struct Cyc_List_List));
_tmp27B->hd=(void*)Cyc_Absyn_uint_t;_tmp27B->tl=_tmp278;_tmp27B;});struct _tuple3*
_tmp27A=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",
sizeof(unsigned char),4)));dict=Cyc_Tcgenrep_buildRepTuple(te,dict,loc,_tmp27A,(
void*)Cyc_Absyn_Static,type,_tmp279);goto _LL622;}_LL626:({void*_tmp27C[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield must be known at this point",
sizeof(unsigned char),47),_tag_arr(_tmp27C,sizeof(void*),0));});goto _LL622;
_LL622:;}goto _LL386;_LL430: if(_tmp1E4 == 0){({void*_tmp27D[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): union declaration must be present by now",
sizeof(unsigned char),48),_tag_arr(_tmp27D,sizeof(void*),0));});}{struct Cyc_Absyn_Uniondecl*
_tmp27E=*((struct Cyc_Absyn_Uniondecl**)_check_null(_tmp1E4));if(_tmp27E->tvs != 0){({
void*_tmp27F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen: unions with parameters not yet supported",sizeof(unsigned char),
46),_tag_arr(_tmp27F,sizeof(void*),0));});}if(_tmp27E->fields == 0){struct _tuple3*
_tmp280=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp283=_cycalloc(sizeof(
struct _tagged_arr));_tmp283[0]=Cyc_Tcgenrep_make_type_cstring(type);_tmp283;}));
struct Cyc_Core_Opt*_tmp281=({struct Cyc_Core_Opt*_tmp282=_cycalloc(sizeof(struct
Cyc_Core_Opt));_tmp282->v=Cyc_Tcgenrep_gen_vardecl(_tmp280,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_union_str)),0,(void*)Cyc_Absyn_Extern,loc);_tmp282;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp280,loc),loc),0,1);goto _LL386;}{struct Cyc_List_List*_tmp284=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_type,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp27E->fields))->v);
struct _tuple3*_tmp285=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp286=
_cycalloc(sizeof(struct _tagged_arr));_tmp286[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp286;}));dict=Cyc_Tcgenrep_buildRepUnion(te,dict,loc,_tmp285,(void*)Cyc_Absyn_Public,
type,_tmp284);goto _LL386;}}_LL432: {struct Cyc_List_List*_tmp287=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_type,
_tmp1EA);struct _tuple3*_tmp288=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(unsigned char),4)));dict=Cyc_Tcgenrep_buildRepUnion(te,
dict,loc,_tmp288,(void*)Cyc_Absyn_Static,type,_tmp287);goto _LL386;}_LL434: goto
_LL436;_LL436: {struct _tuple3*_tmp289=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(unsigned char),4)));struct Cyc_Absyn_Decl*_tmp28A=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_int_str),_tmp289,({struct Cyc_Absyn_Exp*_tmp28C[1];_tmp28C[0]=Cyc_Tcgenrep_cnst_int(
32,loc);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp28C,sizeof(struct Cyc_Absyn_Exp*),1));}),(void*)Cyc_Absyn_Static,loc);dict=
Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*_tmp28B[1];_tmp28B[0]=
_tmp28A;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp28B,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp289,loc),loc),0,0);goto _LL386;}_LL438:({void*_tmp28D[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen() for sizeof_t<-> not yet supported",sizeof(unsigned char),40),
_tag_arr(_tmp28D,sizeof(void*),0));});goto _LL386;_LL386:;}return({struct _tuple12*
_tmp28E=_cycalloc(sizeof(struct _tuple12));_tmp28E->f1=dict;_tmp28E->f2=((struct
Cyc_Tcgenrep_RepInfo*(*)(struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup)(dict,
type);_tmp28E;});}static int Cyc_Tcgenrep_not_emitted_filter(struct Cyc_Tcgenrep_RepInfo*
ri){return ri->emitted == 0;}static void Cyc_Tcgenrep_mark_emitted(struct Cyc_Tcgenrep_RepInfo*
ri){ri->emitted=1;}struct _tuple15{struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;
struct Cyc_Absyn_Exp*f3;};struct _tuple15 Cyc_Tcgenrep_tcGenrep(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,void*type,struct Cyc_Dict_Dict*
dict){Cyc_Tcutil_check_valid_toplevel_type(loc,te,type);Cyc_Tcutil_check_valid_toplevel_type(
loc,te,type);{struct _tuple12 _tmp291;struct Cyc_Tcgenrep_RepInfo*_tmp292;struct Cyc_Dict_Dict*
_tmp294;struct _tuple12*_tmp28F=Cyc_Tcgenrep_lookupRep(te,dict,loc,type);_tmp291=*
_tmp28F;_LL661: _tmp294=_tmp291.f1;goto _LL659;_LL659: _tmp292=_tmp291.f2;goto
_LL656;_LL656: {struct Cyc_List_List*_tmp296=Cyc_Tcgenrep_dfs(_tmp292);return({
struct _tuple15 _tmp297;_tmp297.f1=_tmp294;_tmp297.f2=_tmp296;_tmp297.f3=(struct
Cyc_Absyn_Exp*)_check_null(_tmp292->exp);_tmp297;});}}}
