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
 extern void exit(int);struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;
};extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
extern struct _tagged_arr Cstring_to_string(unsigned char*);struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);extern unsigned char Cyc_List_List_mismatch[18];extern struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern unsigned char Cyc_List_Nth[8];extern struct Cyc_List_List*Cyc_List_filter(
int(*f)(void*),struct Cyc_List_List*x);extern unsigned char Cyc_Arg_Bad[8];struct
Cyc_Arg_Bad_struct{unsigned char*tag;struct _tagged_arr f1;};extern unsigned char
Cyc_Arg_Error[10];static const int Cyc_Arg_Unit_spec=0;struct Cyc_Arg_Unit_spec_struct{
int tag;void(*f1)();};static const int Cyc_Arg_Flag_spec=1;struct Cyc_Arg_Flag_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};static const int Cyc_Arg_FlagString_spec=2;
struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct _tagged_arr,struct
_tagged_arr);};static const int Cyc_Arg_Set_spec=3;struct Cyc_Arg_Set_spec_struct{
int tag;int*f1;};static const int Cyc_Arg_Clear_spec=4;struct Cyc_Arg_Clear_spec_struct{
int tag;int*f1;};static const int Cyc_Arg_String_spec=5;struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};static const int Cyc_Arg_Int_spec=6;struct Cyc_Arg_Int_spec_struct{
int tag;void(*f1)(int);};static const int Cyc_Arg_Rest_spec=7;struct Cyc_Arg_Rest_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};extern void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _tagged_arr),struct _tagged_arr errmsg,struct
_tagged_arr args);struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;extern struct Cyc_Std___sFILE*
Cyc_Std_stdout;extern struct Cyc_Std___sFILE*Cyc_Std_stderr;extern int Cyc_Std_remove(
struct _tagged_arr);extern int Cyc_Std_fclose(struct Cyc_Std___sFILE*);extern int Cyc_Std_fflush(
struct Cyc_Std___sFILE*);extern struct Cyc_Std___sFILE*Cyc_Std_fopen(struct
_tagged_arr __filename,struct _tagged_arr __modes);extern unsigned int Cyc_Std_fread(
struct _tagged_arr __ptr,unsigned int __size,unsigned int __n,struct Cyc_Std___sFILE*
__stream);extern unsigned int Cyc_Std_fwrite(struct _tagged_arr __ptr,unsigned int
__size,unsigned int __n,struct Cyc_Std___sFILE*__s);extern int Cyc_Std_feof(struct
Cyc_Std___sFILE*__stream);extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};extern struct Cyc_Std___sFILE*Cyc_Std_file_open(
struct _tagged_arr fname,struct _tagged_arr mode);extern void Cyc_Std_file_close(
struct Cyc_Std___sFILE*);static const int Cyc_Std_String_pa=0;struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Std_Int_pa=1;struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};static const int Cyc_Std_Double_pa=2;struct Cyc_Std_Double_pa_struct{
int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=3;struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=4;struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern int Cyc_Std_printf(struct _tagged_arr fmt,
struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,
struct _tagged_arr);static const int Cyc_Std_ShortPtr_sa=0;struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};static const int Cyc_Std_UShortPtr_sa=1;struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=2;struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=3;struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=4;struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};extern unsigned char Cyc_Array_Array_mismatch[19];struct Cyc_Std__Div{
int quot;int rem;};struct Cyc_Std__Ldiv{int quot;int rem;};extern int abs(int __x);
extern int atexit(void(*__func)());extern struct Cyc_Std__Div div(int __numer,int
__denom);extern struct Cyc_Std__Ldiv ldiv(int __numer,int __denom);extern int random();
extern void srandom(unsigned int __seed);extern int rand();extern void srand(
unsigned int __seed);extern int rand_r(unsigned int*__seed);extern int grantpt(int
__fd);extern int unlockpt(int __fd);extern struct _tagged_arr Cyc_Std_getenv(struct
_tagged_arr);extern int Cyc_Std_system(struct _tagged_arr);extern unsigned int Cyc_Std_strlen(
struct _tagged_arr s);extern int Cyc_Std_strcmp(struct _tagged_arr s1,struct
_tagged_arr s2);extern struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct
_tagged_arr);extern struct _tagged_arr Cyc_Std_strconcat_l(struct Cyc_List_List*);
extern struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,struct _tagged_arr);
extern struct _tagged_arr Cyc_Std_substring(struct _tagged_arr,int ofs,unsigned int n);
extern struct _tagged_arr Cyc_Std_strchr(struct _tagged_arr s,unsigned char c);struct
Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[
11];extern struct _tagged_arr Cyc_Filename_concat(struct _tagged_arr,struct
_tagged_arr);extern struct _tagged_arr Cyc_Filename_chop_extension(struct
_tagged_arr);extern struct _tagged_arr Cyc_Filename_dirname(struct _tagged_arr);
extern struct _tagged_arr Cyc_Filename_basename(struct _tagged_arr);extern int Cyc_Filename_check_suffix(
struct _tagged_arr,struct _tagged_arr);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];extern void Cyc_Position_reset_position(struct _tagged_arr);struct Cyc_Position_Segment;
static const int Cyc_Position_Lex=0;static const int Cyc_Position_Parse=1;static const
int Cyc_Position_Elab=2;struct Cyc_Position_Error{struct _tagged_arr source;struct
Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern unsigned char
Cyc_Position_Nocontext[14];extern int Cyc_Position_error_p();static const int Cyc_Absyn_Loc_n=
0;static const int Cyc_Absyn_Rel_n=0;struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
f1;};static const int Cyc_Absyn_Abs_n=1;struct Cyc_Absyn_Abs_n_struct{int tag;struct
Cyc_List_List*f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;
static const int Cyc_Absyn_Static=0;static const int Cyc_Absyn_Abstract=1;static const
int Cyc_Absyn_Public=2;static const int Cyc_Absyn_Extern=3;static const int Cyc_Absyn_ExternC=
4;struct Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int q_restrict: 1;};static
const int Cyc_Absyn_B1=0;static const int Cyc_Absyn_B2=1;static const int Cyc_Absyn_B4=2;
static const int Cyc_Absyn_B8=3;static const int Cyc_Absyn_AnyKind=0;static const int Cyc_Absyn_MemKind=
1;static const int Cyc_Absyn_BoxKind=2;static const int Cyc_Absyn_RgnKind=3;static
const int Cyc_Absyn_EffKind=4;static const int Cyc_Absyn_Signed=0;static const int Cyc_Absyn_Unsigned=
1;struct Cyc_Absyn_Conref{void*v;};static const int Cyc_Absyn_Eq_constr=0;struct Cyc_Absyn_Eq_constr_struct{
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
_tuple0*name;int is_xtunion;};static const int Cyc_Absyn_UnknownTunion=0;struct Cyc_Absyn_UnknownTunion_struct{
int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};static const int Cyc_Absyn_KnownTunion=
1;struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};
struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*targs;void*rgn;}
;struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*tunion_name;struct _tuple0*
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Structdecl**f3;};
static const int Cyc_Absyn_UnionType=11;struct Cyc_Absyn_UnionType_struct{int tag;
struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Uniondecl**f3;};static
const int Cyc_Absyn_EnumType=12;struct Cyc_Absyn_EnumType_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};static const int Cyc_Absyn_SizeofType=13;
struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};static const int Cyc_Absyn_AnonStructType=
14;struct Cyc_Absyn_AnonStructType_struct{int tag;struct Cyc_List_List*f1;};static
const int Cyc_Absyn_AnonUnionType=15;struct Cyc_Absyn_AnonUnionType_struct{int tag;
struct Cyc_List_List*f1;};static const int Cyc_Absyn_AnonEnumType=16;struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_RgnHandleType=17;struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};static const int Cyc_Absyn_TypedefType=
18;struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
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
int Cyc_Absyn_Var_e=1;struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*f1;void*
f2;};static const int Cyc_Absyn_UnknownId_e=2;struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple0*f1;};static const int Cyc_Absyn_Primop_e=3;struct Cyc_Absyn_Primop_e_struct{
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
struct Cyc_List_List*f1;};static const int Cyc_Absyn_CompoundLit_e=25;struct _tuple1{
struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Array_e=
26;struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};static const int
Cyc_Absyn_Comprehension_e=27;struct Cyc_Absyn_Comprehension_e_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
static const int Cyc_Absyn_Struct_e=28;struct Cyc_Absyn_Struct_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl*
f4;};static const int Cyc_Absyn_AnonStruct_e=29;struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Tunion_e=30;
struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Tuniondecl*f4;struct Cyc_Absyn_Tunionfield*
f5;};static const int Cyc_Absyn_Enum_e=31;struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
static const int Cyc_Absyn_AnonEnum_e=32;struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static const int Cyc_Absyn_Malloc_e=
33;struct Cyc_Absyn_Malloc_e_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};static
const int Cyc_Absyn_UnresolvedMem_e=34;struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_StmtExp_e=
35;struct Cyc_Absyn_StmtExp_e_struct{int tag;struct Cyc_Absyn_Stmt*f1;};static const
int Cyc_Absyn_Codegen_e=36;struct Cyc_Absyn_Codegen_e_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};static const int Cyc_Absyn_Fill_e=37;struct Cyc_Absyn_Fill_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;
struct Cyc_Position_Segment*loc;void*annot;};struct _tuple2{struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;
struct _tuple2 condition;struct _tuple2 delta;struct Cyc_Absyn_Stmt*body;};static
const int Cyc_Absyn_Skip_s=0;static const int Cyc_Absyn_Exp_s=0;struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_Seq_s=1;struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_Return_s=
2;struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};static const int
Cyc_Absyn_IfThenElse_s=3;struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};static const int Cyc_Absyn_While_s=
4;struct Cyc_Absyn_While_s_struct{int tag;struct _tuple2 f1;struct Cyc_Absyn_Stmt*f2;
};static const int Cyc_Absyn_Break_s=5;struct Cyc_Absyn_Break_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};static const int Cyc_Absyn_Continue_s=6;struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};static const int Cyc_Absyn_Goto_s=7;struct Cyc_Absyn_Goto_s_struct{
int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_For_s=
8;struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple2 f2;
struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;};static const int Cyc_Absyn_Switch_s=9;
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
struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple2 f2;};
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
int tag;struct _tuple0*f1;};static const int Cyc_Absyn_UnknownCall_p=12;struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};static
const int Cyc_Absyn_UnknownFields_p=13;struct Cyc_Absyn_UnknownFields_p_struct{int
tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{
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
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Structfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structdecl{
void*sc;struct Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Uniondecl{void*sc;struct
Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_List_List*tvs;void*defn;};static const int Cyc_Absyn_Var_d=0;
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
10;struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;
};static const int Cyc_Absyn_ExternC_d=11;struct Cyc_Absyn_ExternC_d_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};static const int Cyc_Absyn_ArrayElement=0;struct Cyc_Absyn_ArrayElement_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_FieldName=1;struct Cyc_Absyn_FieldName_struct{
int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[15];extern
unsigned char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{unsigned char*
tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _tagged_arr lex_buffer;int lex_buffer_len;int lex_abs_pos;
int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int
lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct
_tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct
_tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr lex_default;
struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};extern struct Cyc_List_List*
Cyc_Parse_parse_file(struct Cyc_Std___sFILE*f);struct Cyc_Declaration_spec;struct
Cyc_Declarator;struct Cyc_Abstractdeclarator;extern int Cyc_yyparse();extern
unsigned char Cyc_AbstractDeclarator_tok[27];struct Cyc_AbstractDeclarator_tok_struct{
unsigned char*tag;struct Cyc_Abstractdeclarator*f1;};extern unsigned char Cyc_AttributeList_tok[
22];struct Cyc_AttributeList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;
};extern unsigned char Cyc_Attribute_tok[18];struct Cyc_Attribute_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Bool_tok[13];struct Cyc_Bool_tok_struct{
unsigned char*tag;int f1;};extern unsigned char Cyc_Char_tok[13];struct Cyc_Char_tok_struct{
unsigned char*tag;unsigned char f1;};extern unsigned char Cyc_DeclList_tok[17];
struct Cyc_DeclList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_DeclSpec_tok[17];struct Cyc_DeclSpec_tok_struct{unsigned char*
tag;struct Cyc_Declaration_spec*f1;};extern unsigned char Cyc_Declarator_tok[19];
struct Cyc_Declarator_tok_struct{unsigned char*tag;struct Cyc_Declarator*f1;};
extern unsigned char Cyc_DesignatorList_tok[23];struct Cyc_DesignatorList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Designator_tok[
19];struct Cyc_Designator_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_EnumfieldList_tok[22];struct Cyc_EnumfieldList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Enumfield_tok[
18];struct Cyc_Enumfield_tok_struct{unsigned char*tag;struct Cyc_Absyn_Enumfield*
f1;};extern unsigned char Cyc_ExpList_tok[16];struct Cyc_ExpList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Exp_tok[12];
struct Cyc_Exp_tok_struct{unsigned char*tag;struct Cyc_Absyn_Exp*f1;};extern
unsigned char Cyc_FieldPatternList_tok[25];struct Cyc_FieldPatternList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_FieldPattern_tok[
21];struct _tuple3{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
unsigned char*tag;struct _tuple3*f1;};extern unsigned char Cyc_FnDecl_tok[15];
struct Cyc_FnDecl_tok_struct{unsigned char*tag;struct Cyc_Absyn_Fndecl*f1;};extern
unsigned char Cyc_IdList_tok[15];struct Cyc_IdList_tok_struct{unsigned char*tag;
struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDeclList_tok[21];struct Cyc_InitDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDecl_tok[
17];struct _tuple4{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
unsigned char*tag;struct _tuple4*f1;};extern unsigned char Cyc_InitializerList_tok[
24];struct Cyc_InitializerList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_Int_tok[12];struct _tuple5{void*f1;int f2;};struct Cyc_Int_tok_struct{
unsigned char*tag;struct _tuple5*f1;};extern unsigned char Cyc_Kind_tok[13];struct
Cyc_Kind_tok_struct{unsigned char*tag;void*f1;};extern unsigned char Cyc_Okay_tok[
13];extern unsigned char Cyc_ParamDeclListBool_tok[26];struct _tuple6{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct Cyc_ParamDeclListBool_tok_struct{unsigned char*tag;struct _tuple6*f1;}
;extern unsigned char Cyc_ParamDeclList_tok[22];struct Cyc_ParamDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_ParamDecl_tok[
18];struct Cyc_ParamDecl_tok_struct{unsigned char*tag;struct _tuple1*f1;};extern
unsigned char Cyc_PatternList_tok[20];struct Cyc_PatternList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Pattern_tok[16];
struct Cyc_Pattern_tok_struct{unsigned char*tag;struct Cyc_Absyn_Pat*f1;};extern
unsigned char Cyc_Pointer_Sort_tok[21];struct Cyc_Pointer_Sort_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Primop_tok[15];struct Cyc_Primop_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Primopopt_tok[18];struct Cyc_Primopopt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_QualId_tok[15];
struct Cyc_QualId_tok_struct{unsigned char*tag;struct _tuple0*f1;};extern
unsigned char Cyc_QualSpecList_tok[21];struct _tuple7{struct Cyc_Absyn_Tqual f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_QualSpecList_tok_struct{
unsigned char*tag;struct _tuple7*f1;};extern unsigned char Cyc_Rgnorder_tok[17];
struct Cyc_Rgnorder_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_Scope_tok[14];struct Cyc_Scope_tok_struct{unsigned char*tag;void*
f1;};extern unsigned char Cyc_Short_tok[14];struct Cyc_Short_tok_struct{
unsigned char*tag;short f1;};extern unsigned char Cyc_Stmt_tok[13];struct Cyc_Stmt_tok_struct{
unsigned char*tag;struct Cyc_Absyn_Stmt*f1;};extern unsigned char Cyc_StorageClass_tok[
21];struct Cyc_StorageClass_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_String_tok[15];struct Cyc_String_tok_struct{unsigned char*tag;
struct _tagged_arr f1;};extern unsigned char Cyc_Stringopt_tok[18];struct Cyc_Stringopt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_StructFieldDeclListList_tok[
32];struct Cyc_StructFieldDeclListList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_StructFieldDeclList_tok[28];struct Cyc_StructFieldDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_StructOrUnion_tok[
22];struct Cyc_StructOrUnion_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_SwitchCClauseList_tok[26];struct Cyc_SwitchCClauseList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_SwitchClauseList_tok[
25];struct Cyc_SwitchClauseList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_TunionFieldList_tok[24];struct Cyc_TunionFieldList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_TunionField_tok[
20];struct Cyc_TunionField_tok_struct{unsigned char*tag;struct Cyc_Absyn_Tunionfield*
f1;};extern unsigned char Cyc_TypeList_tok[17];struct Cyc_TypeList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_TypeModifierList_tok[
25];struct Cyc_TypeModifierList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_TypeOpt_tok[16];struct Cyc_TypeOpt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_TypeQual_tok[17];
struct Cyc_TypeQual_tok_struct{unsigned char*tag;struct Cyc_Absyn_Tqual f1;};extern
unsigned char Cyc_TypeSpecifier_tok[22];struct Cyc_TypeSpecifier_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Type_tok[13];struct Cyc_Type_tok_struct{
unsigned char*tag;void*f1;};extern int Cyc_yyparse();extern unsigned char Cyc_YY1[8];
struct Cyc_YY1_struct{unsigned char*tag;struct _tuple6*f1;};extern unsigned char Cyc_YYINITIALSVAL[
18];struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;
int last_column;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct
Cyc_List_List*curr_namespace;};extern void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*
fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r;extern void Cyc_Absynpp_decllist2file(struct Cyc_List_List*
tdl,struct Cyc_Std___sFILE*f);extern void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*
fs);extern void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc_Std___sFILE*
f);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];static const int Cyc_Tcenv_VarRes=
0;struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};static const int Cyc_Tcenv_StructRes=
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
ae;struct Cyc_Core_Opt*le;};extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();extern
unsigned char Cyc_Tcutil_TypeErr[12];extern void Cyc_Tcutil_flush_warnings();extern
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
extern struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
extern struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);extern struct Cyc_List_List*
Cyc_Tovc_tovc(struct Cyc_List_List*decls);static const int Cyc_CfFlowInfo_VarRoot=0;
struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};static
const int Cyc_CfFlowInfo_MallocPt=1;struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};static const int Cyc_CfFlowInfo_NoneIL=0;static const int Cyc_CfFlowInfo_ThisIL=
1;static const int Cyc_CfFlowInfo_AllIL=2;static const int Cyc_CfFlowInfo_EqualConst=0;
struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};static const int Cyc_CfFlowInfo_LessVar=
1;struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};static
const int Cyc_CfFlowInfo_LessSize=2;struct Cyc_CfFlowInfo_LessSize_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_CfFlowInfo_LessConst=3;struct Cyc_CfFlowInfo_LessConst_struct{
int tag;unsigned int f1;};static const int Cyc_CfFlowInfo_LessEqSize=4;struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};extern unsigned char Cyc_CfFlowInfo_IsZero[11];struct Cyc_CfFlowInfo_IsZero_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_CfFlowInfo_NotZero[
12];struct Cyc_CfFlowInfo_NotZero_struct{unsigned char*tag;struct Cyc_List_List*f1;
};extern unsigned char Cyc_CfFlowInfo_UnknownZ[13];struct Cyc_CfFlowInfo_UnknownZ_struct{
unsigned char*tag;struct Cyc_List_List*f1;};static const int Cyc_CfFlowInfo_PlaceL=0;
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};static
const int Cyc_CfFlowInfo_UnknownL=0;static const int Cyc_CfFlowInfo_Zero=0;static
const int Cyc_CfFlowInfo_NotZeroAll=1;static const int Cyc_CfFlowInfo_NotZeroThis=2;
static const int Cyc_CfFlowInfo_UnknownR=0;struct Cyc_CfFlowInfo_UnknownR_struct{int
tag;void*f1;};static const int Cyc_CfFlowInfo_Esc=1;struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};static const int Cyc_CfFlowInfo_AddressOf=2;struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};static const int Cyc_CfFlowInfo_Aggregate=3;
struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_Dict_Dict*f1;};static
const int Cyc_CfFlowInfo_BottomFL=0;static const int Cyc_CfFlowInfo_ReachableFL=0;
struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict*f1;struct Cyc_List_List*
f2;};extern void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);struct Cyc_Interface_I;
extern struct Cyc_Interface_I*Cyc_Interface_empty();extern struct Cyc_Interface_I*
Cyc_Interface_final();extern struct Cyc_Interface_I*Cyc_Interface_extract(struct
Cyc_Dict_Dict*ae);struct _tuple8{struct _tagged_arr f1;struct _tagged_arr f2;};extern
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*
i2,struct _tuple8*info);extern struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(
struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*
linfo);extern struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc_Std___sFILE*);
extern void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc_Std___sFILE*);
extern struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc_Std___sFILE*);extern
void Cyc_Lex_lex_init();int Cyc_Toc_warn_bounds_checks;static int Cyc_pp_r=0;static
int Cyc_noexpand_r=0;static int Cyc_noshake_r=0;static int Cyc_stop_after_cpp_r=0;
static int Cyc_parseonly_r=0;static int Cyc_tc_r=0;static int Cyc_ic_r=0;static int Cyc_toc_r=
0;static int Cyc_stop_after_objectfile_r=0;static int Cyc_stop_after_asmfile_r=0;
static int Cyc_tovc_r=0;static int Cyc_v_r=0;static int Cyc_save_temps_r=0;static int
Cyc_save_c_r=0;static int Cyc_nogc_r=0;static int Cyc_pa_r=0;static int Cyc_nocheck_r=
0;static int Cyc_add_cyc_namespace_r=1;static int Cyc_generate_line_directives_r=0;
static int Cyc_print_full_evars_r=0;static int Cyc_print_all_tvars_r=0;static int Cyc_print_all_kinds_r=
0;static struct _tagged_arr*Cyc_output_file=0;static void Cyc_set_output_file(struct
_tagged_arr s){Cyc_output_file=({struct _tagged_arr*_tmp0=_cycalloc(sizeof(struct
_tagged_arr));_tmp0[0]=s;_tmp0;});}extern unsigned char*Cdef_inc_path;extern
unsigned char*Cdef_lib_path;extern unsigned char*Carch;extern unsigned char*Ccomp;
extern unsigned char*Cversion;static unsigned char _tmp1[1]="";static struct
_tagged_arr Cyc_cpp={_tmp1,_tmp1,_tmp1 + 1};static void Cyc_set_cpp(struct
_tagged_arr s){Cyc_cpp=s;}static struct Cyc_List_List*Cyc_cppargs=0;static void Cyc_add_cpparg(
struct _tagged_arr s){Cyc_cppargs=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(
struct Cyc_List_List));_tmp2->hd=({struct _tagged_arr*_tmp3=_cycalloc(sizeof(
struct _tagged_arr));_tmp3[0]=s;_tmp3;});_tmp2->tl=Cyc_cppargs;_tmp2;});}static
void Cyc_print_version(){({struct Cyc_Std_String_pa_struct _tmp5;_tmp5.tag=Cyc_Std_String_pa;
_tmp5.f1=(struct _tagged_arr)Cstring_to_string(Cversion);{void*_tmp4[1]={& _tmp5};
Cyc_Std_printf(_tag_arr("The Cyclone compiler, version %s\n",sizeof(
unsigned char),34),_tag_arr(_tmp4,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp8;_tmp8.tag=Cyc_Std_String_pa;_tmp8.f1=(struct _tagged_arr)Cstring_to_string(
Ccomp);{struct Cyc_Std_String_pa_struct _tmp7;_tmp7.tag=Cyc_Std_String_pa;_tmp7.f1=(
struct _tagged_arr)Cstring_to_string(Carch);{void*_tmp6[2]={& _tmp7,& _tmp8};Cyc_Std_printf(
_tag_arr("Compiled for architecture: %s CC=\"%s\"\n",sizeof(unsigned char),39),
_tag_arr(_tmp6,sizeof(void*),2));}}});({struct Cyc_Std_String_pa_struct _tmpA;
_tmpA.tag=Cyc_Std_String_pa;_tmpA.f1=(struct _tagged_arr)Cstring_to_string(
Cdef_lib_path);{void*_tmp9[1]={& _tmpA};Cyc_Std_printf(_tag_arr("Standard library directory: %s\n",
sizeof(unsigned char),32),_tag_arr(_tmp9,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmpC;_tmpC.tag=Cyc_Std_String_pa;_tmpC.f1=(struct _tagged_arr)Cstring_to_string(
Cdef_inc_path);{void*_tmpB[1]={& _tmpC};Cyc_Std_printf(_tag_arr("Standard include directory: %s\n",
sizeof(unsigned char),32),_tag_arr(_tmpB,sizeof(void*),1));}});exit(0);}static
int Cyc_is_cyclone_sourcefile(struct _tagged_arr s){unsigned int _tmpD=Cyc_Std_strlen(
s);if(_tmpD <= 4){return 0;}else{return Cyc_Std_strcmp(_tagged_arr_plus(s,sizeof(
unsigned char),(int)(_tmpD - 4)),_tag_arr(".cyc",sizeof(unsigned char),5))== 0;}}
static struct Cyc_List_List*Cyc_ccargs=0;static void Cyc_add_ccarg(struct _tagged_arr
s){Cyc_ccargs=({struct Cyc_List_List*_tmpE=_cycalloc(sizeof(struct Cyc_List_List));
_tmpE->hd=({struct _tagged_arr*_tmpF=_cycalloc(sizeof(struct _tagged_arr));_tmpF[0]=
s;_tmpF;});_tmpE->tl=Cyc_ccargs;_tmpE;});}static struct Cyc_List_List*Cyc_cyclone_lib_path=
0;static void Cyc_add_cyclone_lib_path(struct _tagged_arr s){unsigned int _tmp10=Cyc_Std_strlen(
s);if(_tmp10 <= 2){return;}{struct _tagged_arr _tmp11=(struct _tagged_arr)Cyc_Std_substring(
s,2,_tmp10 - 2);Cyc_cyclone_lib_path=({struct Cyc_List_List*_tmp12=_cycalloc(
sizeof(struct Cyc_List_List));_tmp12->hd=({struct _tagged_arr*_tmp13=_cycalloc(
sizeof(struct _tagged_arr));_tmp13[0]=_tmp11;_tmp13;});_tmp12->tl=Cyc_cyclone_lib_path;
_tmp12;});}}static struct Cyc_List_List*Cyc_libargs=0;static void Cyc_add_libarg(
struct _tagged_arr s){if(Cyc_Std_strcmp(s,_tag_arr("-lxml",sizeof(unsigned char),6))
== 0){Cyc_add_ccarg(s);}else{Cyc_libargs=({struct Cyc_List_List*_tmp14=_cycalloc(
sizeof(struct Cyc_List_List));_tmp14->hd=({struct _tagged_arr*_tmp15=_cycalloc(
sizeof(struct _tagged_arr));_tmp15[0]=s;_tmp15;});_tmp14->tl=Cyc_libargs;_tmp14;});}}
static void Cyc_set_save_temps(){Cyc_save_temps_r=1;Cyc_add_ccarg(_tag_arr("-save-temps",
sizeof(unsigned char),12));}static int Cyc_produce_dependencies=0;static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=1;Cyc_add_cpparg(_tag_arr("-M",
sizeof(unsigned char),3));}static struct _tagged_arr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _tagged_arr s){Cyc_dependencies_target=({
struct _tagged_arr*_tmp16=_cycalloc(sizeof(struct _tagged_arr));_tmp16[0]=s;_tmp16;});}
static void Cyc_set_stop_after_objectfile(){Cyc_stop_after_objectfile_r=1;Cyc_add_ccarg(
_tag_arr("-c",sizeof(unsigned char),3));}static void Cyc_set_nocppprecomp(){Cyc_add_cpparg(
_tag_arr("-no-cpp-precomp",sizeof(unsigned char),16));Cyc_add_ccarg(_tag_arr("-no-cpp-precomp",
sizeof(unsigned char),16));}static void Cyc_set_lineno(){Cyc_generate_line_directives_r=
1;Cyc_set_save_temps();}static void Cyc_set_noboundschecks(){Cyc_add_ccarg(
_tag_arr("-DNO_CYC_BOUNDS_CHECKS",sizeof(unsigned char),23));}static void Cyc_set_nonullchecks(){
Cyc_add_ccarg(_tag_arr("-DNO_CYC_NULL_CHECKS",sizeof(unsigned char),21));}static
void Cyc_set_nochecks(){Cyc_set_noboundschecks();Cyc_set_nonullchecks();Cyc_nocheck_r=
1;}static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=0;Cyc_add_ccarg(_tag_arr("-DNO_CYC_PREFIX",
sizeof(unsigned char),16));}static void Cyc_set_pa(){Cyc_pa_r=1;Cyc_add_ccarg(
_tag_arr("-DCYC_REGION_PROFILE",sizeof(unsigned char),21));}static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;Cyc_add_ccarg(_tag_arr("-S",sizeof(unsigned char),3));}
static const int Cyc_DEFAULTINPUT=0;static const int Cyc_CYCLONEFILE=1;static void*Cyc_intype=(
void*)0;static void Cyc_set_inputtype(struct _tagged_arr s){if(Cyc_Std_strcmp(s,
_tag_arr("cyc",sizeof(unsigned char),4))== 0){Cyc_intype=(void*)Cyc_CYCLONEFILE;}
else{if(Cyc_Std_strcmp(s,_tag_arr("none",sizeof(unsigned char),5))== 0){Cyc_intype=(
void*)Cyc_DEFAULTINPUT;}else{({struct Cyc_Std_String_pa_struct _tmp18;_tmp18.tag=
Cyc_Std_String_pa;_tmp18.f1=(struct _tagged_arr)s;{void*_tmp17[1]={& _tmp18};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Input type '%s' not supported\n",sizeof(unsigned char),
31),_tag_arr(_tmp17,sizeof(void*),1));}});}}}struct _tagged_arr Cyc_make_base_filename(
struct _tagged_arr s,struct _tagged_arr*output_file){struct _tagged_arr _tmp19=Cyc_Filename_dirname(
output_file == 0? s:*output_file);struct _tagged_arr _tmp1A=Cyc_Filename_chop_extension((
struct _tagged_arr)Cyc_Filename_basename(s));struct _tagged_arr _tmp1B=Cyc_Std_strlen((
struct _tagged_arr)_tmp19)> 0? Cyc_Filename_concat((struct _tagged_arr)_tmp19,(
struct _tagged_arr)_tmp1A): _tmp1A;return(struct _tagged_arr)_tmp1B;}static struct
Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(struct _tagged_arr s){
if(Cyc_intype == (void*)Cyc_CYCLONEFILE? 1: Cyc_is_cyclone_sourcefile(s)){Cyc_cyclone_files=({
struct Cyc_List_List*_tmp1C=_cycalloc(sizeof(struct Cyc_List_List));_tmp1C->hd=({
struct _tagged_arr*_tmp1D=_cycalloc(sizeof(struct _tagged_arr));_tmp1D[0]=s;_tmp1D;});
_tmp1C->tl=Cyc_cyclone_files;_tmp1C;});{struct _tagged_arr _tmp1E=Cyc_make_base_filename(
s,Cyc_output_file);struct _tagged_arr _tmp1F=Cyc_Std_strconcat(_tmp1E,_tag_arr(".c",
sizeof(unsigned char),3));Cyc_add_ccarg((struct _tagged_arr)_tmp1F);}}else{Cyc_add_ccarg(
s);}}static void Cyc_remove_file(struct _tagged_arr s){if(Cyc_save_temps_r){return;}
else{Cyc_Std_remove(s);}}int Cyc_compile_failure=0;struct Cyc_Std___sFILE*Cyc_try_file_open(
struct _tagged_arr filename,struct _tagged_arr mode,struct _tagged_arr msg_part){
struct _handler_cons _tmp20;_push_handler(& _tmp20);{int _tmp22=0;if(setjmp(_tmp20.handler)){
_tmp22=1;}if(! _tmp22){{struct Cyc_Std___sFILE*_tmp23=(struct Cyc_Std___sFILE*)Cyc_Std_file_open(
filename,mode);_npop_handler(0);return _tmp23;};_pop_handler();}else{void*_tmp21=(
void*)_exn_thrown;void*_tmp25=_tmp21;_LL39: goto _LL40;_LL41: goto _LL42;_LL40: Cyc_compile_failure=
1;({struct Cyc_Std_String_pa_struct _tmp2D;_tmp2D.tag=Cyc_Std_String_pa;_tmp2D.f1=(
struct _tagged_arr)filename;{struct Cyc_Std_String_pa_struct _tmp2C;_tmp2C.tag=Cyc_Std_String_pa;
_tmp2C.f1=(struct _tagged_arr)msg_part;{void*_tmp2B[2]={& _tmp2C,& _tmp2D};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("\nError: couldn't open %s %s\n",sizeof(unsigned char),
29),_tag_arr(_tmp2B,sizeof(void*),2));}}});Cyc_Std_fflush((struct Cyc_Std___sFILE*)
Cyc_Std_stderr);return 0;_LL42:(void)_throw(_tmp25);_LL38:;}}}struct Cyc_List_List*
Cyc_do_stage(struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(void*,struct
Cyc_List_List*),void*env,struct Cyc_List_List*tds){struct _tagged_arr exn_string=
_tag_arr("",sizeof(unsigned char),1);struct _tagged_arr explain_string=_tag_arr("",
sizeof(unsigned char),1);int other_exn=0;void*ex=(void*)({struct Cyc_Core_Impossible_struct*
_tmp4E=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp4E[0]=({struct Cyc_Core_Impossible_struct
_tmp4F;_tmp4F.tag=Cyc_Core_Impossible;_tmp4F.f1=_tag_arr("",sizeof(unsigned char),
1);_tmp4F;});_tmp4E;});struct Cyc_List_List*_tmp2E=0;{struct _handler_cons _tmp2F;
_push_handler(& _tmp2F);{int _tmp31=0;if(setjmp(_tmp2F.handler)){_tmp31=1;}if(!
_tmp31){_tmp2E=f(env,tds);;_pop_handler();}else{void*_tmp30=(void*)_exn_thrown;
void*_tmp33=_tmp30;struct _tagged_arr _tmp41;struct _tagged_arr _tmp43;struct
_tagged_arr _tmp45;_LL53: if(*((void**)_tmp33)== Cyc_Core_Impossible){_LL66: _tmp41=((
struct Cyc_Core_Impossible_struct*)_tmp33)->f1;goto _LL54;}else{goto _LL55;}_LL55:
if(_tmp33 == Cyc_Dict_Absent){goto _LL56;}else{goto _LL57;}_LL57: if(*((void**)
_tmp33)== Cyc_Core_Invalid_argument){_LL68: _tmp43=((struct Cyc_Core_Invalid_argument_struct*)
_tmp33)->f1;goto _LL58;}else{goto _LL59;}_LL59: if(*((void**)_tmp33)== Cyc_Core_Failure){
_LL70: _tmp45=((struct Cyc_Core_Failure_struct*)_tmp33)->f1;goto _LL60;}else{goto
_LL61;}_LL61: goto _LL62;_LL63: goto _LL64;_LL54: exn_string=_tag_arr("Exception Core::Impossible",
sizeof(unsigned char),27);explain_string=_tmp41;goto _LL52;_LL56: exn_string=
_tag_arr("Exception Dict::Absent",sizeof(unsigned char),23);goto _LL52;_LL58:
exn_string=_tag_arr("Exception Core::Invalid_argument",sizeof(unsigned char),33);
explain_string=_tmp43;goto _LL52;_LL60: exn_string=_tag_arr("Exception Core::Failure",
sizeof(unsigned char),24);explain_string=_tmp45;goto _LL52;_LL62: ex=_tmp33;
other_exn=1;exn_string=_tag_arr("Uncaught exception",sizeof(unsigned char),19);
goto _LL52;_LL64:(void)_throw(_tmp33);_LL52:;}}}if(Cyc_Position_error_p()){Cyc_compile_failure=
1;}if(Cyc_Std_strcmp(exn_string,_tag_arr("",sizeof(unsigned char),1))!= 0){Cyc_compile_failure=
1;({struct Cyc_Std_String_pa_struct _tmp4A;_tmp4A.tag=Cyc_Std_String_pa;_tmp4A.f1=(
struct _tagged_arr)explain_string;{struct Cyc_Std_String_pa_struct _tmp49;_tmp49.tag=
Cyc_Std_String_pa;_tmp49.f1=(struct _tagged_arr)stage_name;{struct Cyc_Std_String_pa_struct
_tmp48;_tmp48.tag=Cyc_Std_String_pa;_tmp48.f1=(struct _tagged_arr)exn_string;{
void*_tmp47[3]={& _tmp48,& _tmp49,& _tmp4A};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n%s thrown during %s: %s",
sizeof(unsigned char),25),_tag_arr(_tmp47,sizeof(void*),3));}}}});}if(Cyc_compile_failure){({
void*_tmp4B[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\nCOMPILATION FAILED!\n",
sizeof(unsigned char),22),_tag_arr(_tmp4B,sizeof(void*),0));});Cyc_Std_fflush((
struct Cyc_Std___sFILE*)Cyc_Std_stderr);if(other_exn){(int)_throw(ex);}return
_tmp2E;}else{if(Cyc_v_r){({struct Cyc_Std_String_pa_struct _tmp4D;_tmp4D.tag=Cyc_Std_String_pa;
_tmp4D.f1=(struct _tagged_arr)stage_name;{void*_tmp4C[1]={& _tmp4D};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s completed.\n",sizeof(unsigned char),15),_tag_arr(
_tmp4C,sizeof(void*),1));}});Cyc_Std_fflush((struct Cyc_Std___sFILE*)Cyc_Std_stderr);
return _tmp2E;}}return _tmp2E;}struct Cyc_List_List*Cyc_do_parse(struct Cyc_Std___sFILE*
f,struct Cyc_List_List*ignore){Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp50=Cyc_Parse_parse_file(
f);Cyc_Lex_lex_init();return _tmp50;}}struct Cyc_List_List*Cyc_do_typecheck(struct
Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tds){Cyc_Tc_tc(te,1,tds);if(! Cyc_noshake_r){
tds=Cyc_Tc_treeshake(te,tds);}return tds;}struct Cyc_List_List*Cyc_do_cfcheck(int
ignore,struct Cyc_List_List*tds){Cyc_NewControlFlow_cf_check(tds);return tds;}
struct _tuple9{struct Cyc_Tcenv_Tenv*f1;struct Cyc_Std___sFILE*f2;struct Cyc_Std___sFILE*
f3;};struct Cyc_List_List*Cyc_do_interface(struct _tuple9*params,struct Cyc_List_List*
tds){struct _tuple9 _tmp53;struct Cyc_Std___sFILE*_tmp54;struct Cyc_Std___sFILE*
_tmp56;struct Cyc_Tcenv_Tenv*_tmp58;struct _tuple9*_tmp51=params;_tmp53=*_tmp51;
_LL89: _tmp58=_tmp53.f1;goto _LL87;_LL87: _tmp56=_tmp53.f2;goto _LL85;_LL85: _tmp54=
_tmp53.f3;goto _LL82;_LL82: {struct Cyc_Interface_I*_tmp5A=Cyc_Interface_extract(
_tmp58->ae);if(_tmp56 == 0){Cyc_Interface_save(_tmp5A,_tmp54);}else{struct Cyc_Interface_I*
_tmp5B=Cyc_Interface_parse((struct Cyc_Std___sFILE*)_check_null(_tmp56));if(! Cyc_Interface_is_subinterface(
_tmp5B,_tmp5A,({struct _tuple8*_tmp5C=_cycalloc(sizeof(struct _tuple8));_tmp5C->f1=
_tag_arr("written interface",sizeof(unsigned char),18);_tmp5C->f2=_tag_arr("maximal interface",
sizeof(unsigned char),18);_tmp5C;}))){Cyc_compile_failure=1;}else{Cyc_Interface_save(
_tmp5B,_tmp54);}}return tds;}}struct Cyc_List_List*Cyc_do_translate(int ignore,
struct Cyc_List_List*tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*Cyc_do_tovc(
int ignore,struct Cyc_List_List*tds){return Cyc_Tovc_tovc(tds);}static unsigned char
_tmp5D[1]="";static struct _tagged_arr Cyc_cyc_include={_tmp5D,_tmp5D,_tmp5D + 1};
struct Cyc_List_List*Cyc_do_print(struct Cyc_Std___sFILE*out_file,struct Cyc_List_List*
tds){struct Cyc_Absynpp_Params params_r;if(Cyc_tc_r){params_r=Cyc_Absynpp_cyc_params_r;}
else{params_r=Cyc_Absynpp_c_params_r;}params_r.expand_typedefs=! Cyc_noexpand_r;
params_r.to_VC=Cyc_tovc_r;params_r.add_cyc_prefix=Cyc_add_cyc_namespace_r;
params_r.generate_line_directives=Cyc_generate_line_directives_r;params_r.print_full_evars=
Cyc_print_full_evars_r;params_r.print_all_tvars=Cyc_print_all_tvars_r;params_r.print_all_kinds=
Cyc_print_all_kinds_r;{struct Cyc_Std___sFILE*_tmp5E=Cyc_try_file_open(Cyc_cyc_include,
_tag_arr("r",sizeof(unsigned char),2),_tag_arr("internal compiler file",sizeof(
unsigned char),23));if(_tmp5E == 0){return tds;}{struct Cyc_Std___sFILE*_tmp5F=(
struct Cyc_Std___sFILE*)_check_null(_tmp5E);unsigned int n_read=1024;unsigned int
n_written;unsigned char buf[1024];{unsigned int _tmp62=1024;unsigned int i;for(i=0;
i < _tmp62;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=Cyc_Std_fread(
_tag_arr(buf,sizeof(unsigned char),1024),1,1024,_tmp5F);if(n_read != 1024? ! Cyc_Std_feof(
_tmp5F): 0){Cyc_compile_failure=1;({void*_tmp60[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\nError: problem copying cyc_include.h\n",sizeof(unsigned char),39),
_tag_arr(_tmp60,sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___sFILE*)Cyc_Std_stderr);
return tds;}n_written=Cyc_Std_fwrite(_tag_arr(buf,sizeof(unsigned char),1024),1,
n_read,out_file);if(n_read != n_written){Cyc_compile_failure=1;({void*_tmp61[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\nError: problem copying cyc_include.h\n",
sizeof(unsigned char),39),_tag_arr(_tmp61,sizeof(void*),0));});Cyc_Std_fflush((
struct Cyc_Std___sFILE*)Cyc_Std_stderr);return tds;}}Cyc_Std_fclose(_tmp5F);if(Cyc_pp_r){
Cyc_Absynpp_set_params(& params_r);Cyc_Absynpp_decllist2file(tds,out_file);}else{
Cyc_Absyndump_set_params(& params_r);Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
Cyc_Std_fflush((struct Cyc_Std___sFILE*)out_file);return tds;}}}static struct Cyc_List_List*
Cyc_cfiles=0;static void Cyc_remove_cfiles(){if(! Cyc_save_c_r){for(0;Cyc_cfiles != 
0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){Cyc_remove_file(*((
struct _tagged_arr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}}
static struct Cyc_List_List*Cyc_split_by_char(struct _tagged_arr s,unsigned char c){
if(s.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr){return 0;}{struct Cyc_List_List*
_tmp63=0;unsigned int _tmp64=Cyc_Std_strlen(s);while(_tmp64 > 0){struct _tagged_arr
_tmp65=Cyc_Std_strchr(s,c);if(_tmp65.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr){
_tmp63=({struct Cyc_List_List*_tmp66=_cycalloc(sizeof(struct Cyc_List_List));
_tmp66->hd=({struct _tagged_arr*_tmp67=_cycalloc(sizeof(struct _tagged_arr));
_tmp67[0]=s;_tmp67;});_tmp66->tl=_tmp63;_tmp66;});break;}else{_tmp63=({struct Cyc_List_List*
_tmp68=_cycalloc(sizeof(struct Cyc_List_List));_tmp68->hd=({struct _tagged_arr*
_tmp69=_cycalloc(sizeof(struct _tagged_arr));_tmp69[0]=(struct _tagged_arr)Cyc_Std_substring(
s,0,(unsigned int)((((struct _tagged_arr)_tmp65).curr - s.curr)/ sizeof(
unsigned char)));_tmp69;});_tmp68->tl=_tmp63;_tmp68;});_tmp64 -=(((struct
_tagged_arr)_tmp65).curr - s.curr)/ sizeof(unsigned char);s=_tagged_arr_plus(
_tmp65,sizeof(unsigned char),1);}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp63);}}static int Cyc_file_exists(struct _tagged_arr file){
struct Cyc_Std___sFILE*f=0;{struct _handler_cons _tmp6A;_push_handler(& _tmp6A);{int
_tmp6C=0;if(setjmp(_tmp6A.handler)){_tmp6C=1;}if(! _tmp6C){f=(struct Cyc_Std___sFILE*)
Cyc_Std_file_open(file,_tag_arr("r",sizeof(unsigned char),2));;_pop_handler();}
else{void*_tmp6B=(void*)_exn_thrown;void*_tmp6E=_tmp6B;_LL112: goto _LL113;_LL114:
goto _LL115;_LL113: goto _LL111;_LL115:(void)_throw(_tmp6E);_LL111:;}}}if(f == 0){
return 0;}else{Cyc_Std_fclose((struct Cyc_Std___sFILE*)_check_null(f));return 1;}}
static struct _tagged_arr Cyc_sprint_list(struct Cyc_List_List*dirs){struct
_tagged_arr tmp=_tag_arr("",sizeof(unsigned char),1);for(0;dirs != 0;dirs=dirs->tl){
struct _tagged_arr _tmp74=*((struct _tagged_arr*)dirs->hd);if(_tmp74.curr == ((
struct _tagged_arr)_tag_arr(0,0,0)).curr? 1: Cyc_Std_strlen(_tmp74)== 0){continue;}
_tmp74=(struct _tagged_arr)Cyc_Std_strconcat(_tmp74,_tag_arr(":",sizeof(
unsigned char),2));tmp=(struct _tagged_arr)Cyc_Std_strconcat(_tmp74,tmp);}return
tmp;}static struct _tagged_arr*Cyc_find(struct Cyc_List_List*dirs,struct _tagged_arr
file){if(file.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr){return 0;}for(0;
dirs != 0;dirs=dirs->tl){struct _tagged_arr _tmp75=*((struct _tagged_arr*)dirs->hd);
if(_tmp75.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr? 1: Cyc_Std_strlen(
_tmp75)== 0){continue;}{struct _tagged_arr s=(struct _tagged_arr)Cyc_Filename_concat(
_tmp75,file);if(Cyc_file_exists(s)){return({struct _tagged_arr*_tmp76=_cycalloc(
sizeof(struct _tagged_arr));_tmp76[0]=s;_tmp76;});}}}return 0;}static struct
_tagged_arr Cyc_do_find(struct Cyc_List_List*dirs,struct _tagged_arr file){struct
_tagged_arr*_tmp77=Cyc_find(dirs,file);if(_tmp77 == 0){({struct Cyc_Std_String_pa_struct
_tmp7A;_tmp7A.tag=Cyc_Std_String_pa;_tmp7A.f1=(struct _tagged_arr)Cyc_sprint_list(
dirs);{struct Cyc_Std_String_pa_struct _tmp79;_tmp79.tag=Cyc_Std_String_pa;_tmp79.f1=(
struct _tagged_arr)file;{void*_tmp78[2]={& _tmp79,& _tmp7A};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: can't find internal compiler file %s in path %s\n",sizeof(
unsigned char),56),_tag_arr(_tmp78,sizeof(void*),2));}}});Cyc_compile_failure=1;
Cyc_remove_cfiles();exit(1);}return*_tmp77;}static int Cyc_is_other_special(
unsigned char c){switch(c){case '\\': _LL123: goto _LL124;case '"': _LL124: goto _LL125;
case ';': _LL125: goto _LL126;case '&': _LL126: goto _LL127;case '(': _LL127: goto _LL128;
case ')': _LL128: goto _LL129;case '|': _LL129: goto _LL130;case '^': _LL130: goto _LL131;
case '<': _LL131: goto _LL132;case '>': _LL132: goto _LL133;case ' ': _LL133: goto _LL134;
case '\n': _LL134: goto _LL135;case '\t': _LL135: return 1;default: _LL136: return 0;}}
static struct _tagged_arr Cyc_sh_escape_string(struct _tagged_arr s){unsigned int
_tmp8A=Cyc_Std_strlen(s);int _tmp8B=0;int _tmp8C=0;{int i=0;for(0;i < _tmp8A;i ++){
unsigned char _tmp8D=((const unsigned char*)s.curr)[i];if(_tmp8D == '\''){_tmp8B ++;}
else{if(Cyc_is_other_special(_tmp8D)){_tmp8C ++;}}}}if(_tmp8B == 0? _tmp8C == 0: 0){
return s;}if(_tmp8B == 0){return(struct _tagged_arr)Cyc_Std_strconcat_l(({struct
_tagged_arr*_tmp8E[3];_tmp8E[2]=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",
sizeof(unsigned char),2);_tmp8E[1]=({struct _tagged_arr*_tmp8F=_cycalloc(sizeof(
struct _tagged_arr));_tmp8F[0]=(struct _tagged_arr)s;_tmp8F;});_tmp8E[0]=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",sizeof(unsigned char),2);((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp8E,
sizeof(struct _tagged_arr*),3));}));}{unsigned int _tmp90=(_tmp8A + _tmp8B)+ _tmp8C;
struct _tagged_arr _tmp91=({unsigned int _tmp95=_tmp90 + 1;unsigned char*_tmp96=(
unsigned char*)_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmp95));
struct _tagged_arr _tmp98=_tag_arr(_tmp96,sizeof(unsigned char),_tmp90 + 1);{
unsigned int _tmp97=_tmp95;unsigned int i;for(i=0;i < _tmp97;i ++){_tmp96[i]='\000';}};
_tmp98;});int _tmp92=0;int _tmp93=0;for(0;_tmp92 < _tmp8A;_tmp92 ++){unsigned char
_tmp94=((const unsigned char*)s.curr)[_tmp92];if(_tmp94 == '\''? 1: Cyc_is_other_special(
_tmp94)){*((unsigned char*)_check_unknown_subscript(_tmp91,sizeof(unsigned char),
_tmp93 ++))='\\';}*((unsigned char*)_check_unknown_subscript(_tmp91,sizeof(
unsigned char),_tmp93 ++))=_tmp94;}return(struct _tagged_arr)_tmp91;}}static struct
_tagged_arr*Cyc_sh_escape_stringptr(struct _tagged_arr*sp){return({struct
_tagged_arr*_tmp99=_cycalloc(sizeof(struct _tagged_arr));_tmp99[0]=Cyc_sh_escape_string(*
sp);_tmp99;});}static void Cyc_process_file(struct _tagged_arr filename){struct
_tagged_arr _tmp9A=Cyc_make_base_filename(filename,Cyc_output_file);struct
_tagged_arr _tmp9B=Cyc_Std_strconcat(_tmp9A,_tag_arr(".cyp",sizeof(unsigned char),
5));struct _tagged_arr _tmp9C=Cyc_Std_strconcat(_tmp9A,_tag_arr(".cyci",sizeof(
unsigned char),6));struct _tagged_arr _tmp9D=Cyc_Std_strconcat(_tmp9A,_tag_arr(".cycio",
sizeof(unsigned char),7));struct _tagged_arr _tmp9E=Cyc_Std_strconcat(_tmp9A,
_tag_arr(".c",sizeof(unsigned char),3));if(Cyc_v_r){({struct Cyc_Std_String_pa_struct
_tmpA0;_tmpA0.tag=Cyc_Std_String_pa;_tmpA0.f1=(struct _tagged_arr)filename;{void*
_tmp9F[1]={& _tmpA0};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Compiling %s\n",
sizeof(unsigned char),14),_tag_arr(_tmp9F,sizeof(void*),1));}});}{struct Cyc_Std___sFILE*
f0=Cyc_try_file_open(filename,_tag_arr("r",sizeof(unsigned char),2),_tag_arr("input file",
sizeof(unsigned char),11));if(Cyc_compile_failure){return;}Cyc_Std_fclose((
struct Cyc_Std___sFILE*)_check_null(f0));{struct _tagged_arr _tmpA1=Cyc_Std_str_sepstr(({
struct Cyc_List_List*_tmpD6=_cycalloc(sizeof(struct Cyc_List_List));_tmpD6->hd=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(unsigned char),1);
_tmpD6->tl=((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmpD6;}),_tag_arr(" ",
sizeof(unsigned char),2));struct _tagged_arr stdinc_string;struct _tagged_arr
def_inc_path=(struct _tagged_arr)Cstring_to_string(Cdef_inc_path);if(Cyc_Std_strlen(
def_inc_path)> 0){stdinc_string=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmpA3;_tmpA3.tag=Cyc_Std_String_pa;_tmpA3.f1=(struct _tagged_arr)def_inc_path;{
void*_tmpA2[1]={& _tmpA3};Cyc_Std_aprintf(_tag_arr(" -I%s",sizeof(unsigned char),
6),_tag_arr(_tmpA2,sizeof(void*),1));}});}else{stdinc_string=_tag_arr("",sizeof(
unsigned char),1);}{struct _tagged_arr _tmpA4=Cyc_Std_getenv(_tag_arr("CYCLONE_INCLUDE_PATH",
sizeof(unsigned char),21));if(_tmpA4.curr != (_tag_arr(0,0,0)).curr){struct Cyc_List_List*
_tmpA5=Cyc_split_by_char((struct _tagged_arr)_tmpA4,':');struct _tagged_arr _tmpA6=
Cyc_Std_str_sepstr(({struct Cyc_List_List*_tmpAA=_cycalloc(sizeof(struct Cyc_List_List));
_tmpAA->hd=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(
unsigned char),1);_tmpAA->tl=((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
_tmpA5);_tmpAA;}),_tag_arr(" -I",sizeof(unsigned char),4));stdinc_string=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmpA9;_tmpA9.tag=Cyc_Std_String_pa;
_tmpA9.f1=(struct _tagged_arr)stdinc_string;{struct Cyc_Std_String_pa_struct _tmpA8;
_tmpA8.tag=Cyc_Std_String_pa;_tmpA8.f1=(struct _tagged_arr)_tmpA6;{void*_tmpA7[2]={&
_tmpA8,& _tmpA9};Cyc_Std_aprintf(_tag_arr("%s%s",sizeof(unsigned char),5),
_tag_arr(_tmpA7,sizeof(void*),2));}}});}{struct _tagged_arr ofile_string;if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){ofile_string=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmpAC;_tmpAC.tag=Cyc_Std_String_pa;_tmpAC.f1=(struct _tagged_arr)*((struct
_tagged_arr*)_check_null(Cyc_output_file));{void*_tmpAB[1]={& _tmpAC};Cyc_Std_aprintf(
_tag_arr(" > %s",sizeof(unsigned char),6),_tag_arr(_tmpAB,sizeof(void*),1));}});}
else{ofile_string=_tag_arr("",sizeof(unsigned char),1);}}else{ofile_string=(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpAE;_tmpAE.tag=Cyc_Std_String_pa;
_tmpAE.f1=(struct _tagged_arr)Cyc_sh_escape_string((struct _tagged_arr)_tmp9B);{
void*_tmpAD[1]={& _tmpAE};Cyc_Std_aprintf(_tag_arr(" > %s",sizeof(unsigned char),
6),_tag_arr(_tmpAD,sizeof(void*),1));}});}{struct _tagged_arr fixup_string;if(Cyc_produce_dependencies){
if(Cyc_dependencies_target == 0){fixup_string=_tag_arr(" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
sizeof(unsigned char),35);}else{fixup_string=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmpB0;_tmpB0.tag=Cyc_Std_String_pa;_tmpB0.f1=(struct _tagged_arr)*((struct
_tagged_arr*)_check_null(Cyc_dependencies_target));{void*_tmpAF[1]={& _tmpB0};Cyc_Std_aprintf(
_tag_arr(" | sed 's/^.*\\.cyc\\.o:/%s:/'",sizeof(unsigned char),29),_tag_arr(
_tmpAF,sizeof(void*),1));}});}}else{fixup_string=_tag_arr("",sizeof(
unsigned char),1);}{struct _tagged_arr _tmpB1=({struct Cyc_Std_String_pa_struct
_tmpD5;_tmpD5.tag=Cyc_Std_String_pa;_tmpD5.f1=(struct _tagged_arr)ofile_string;{
struct Cyc_Std_String_pa_struct _tmpD4;_tmpD4.tag=Cyc_Std_String_pa;_tmpD4.f1=(
struct _tagged_arr)fixup_string;{struct Cyc_Std_String_pa_struct _tmpD3;_tmpD3.tag=
Cyc_Std_String_pa;_tmpD3.f1=(struct _tagged_arr)Cyc_sh_escape_string(filename);{
struct Cyc_Std_String_pa_struct _tmpD2;_tmpD2.tag=Cyc_Std_String_pa;_tmpD2.f1=(
struct _tagged_arr)stdinc_string;{struct Cyc_Std_String_pa_struct _tmpD1;_tmpD1.tag=
Cyc_Std_String_pa;_tmpD1.f1=(struct _tagged_arr)_tmpA1;{struct Cyc_Std_String_pa_struct
_tmpD0;_tmpD0.tag=Cyc_Std_String_pa;_tmpD0.f1=(struct _tagged_arr)Cyc_cpp;{void*
_tmpCF[6]={& _tmpD0,& _tmpD1,& _tmpD2,& _tmpD3,& _tmpD4,& _tmpD5};Cyc_Std_aprintf(
_tag_arr("%s %s%s %s%s%s",sizeof(unsigned char),15),_tag_arr(_tmpCF,sizeof(void*),
6));}}}}}}});if(Cyc_v_r){({struct Cyc_Std_String_pa_struct _tmpB3;_tmpB3.tag=Cyc_Std_String_pa;
_tmpB3.f1=(struct _tagged_arr)_tmpB1;{void*_tmpB2[1]={& _tmpB3};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s\n",sizeof(unsigned char),4),_tag_arr(_tmpB2,sizeof(
void*),1));}});}if(Cyc_Std_system((struct _tagged_arr)_tmpB1)!= 0){Cyc_compile_failure=
1;({void*_tmpB4[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\nError: preprocessing\n",
sizeof(unsigned char),23),_tag_arr(_tmpB4,sizeof(void*),0));});return;}if(Cyc_stop_after_cpp_r){
return;}Cyc_Position_reset_position((struct _tagged_arr)_tmp9B);{struct Cyc_Std___sFILE*
in_file=Cyc_try_file_open((struct _tagged_arr)_tmp9B,_tag_arr("r",sizeof(
unsigned char),2),_tag_arr("file",sizeof(unsigned char),5));if(Cyc_compile_failure){
return;}{struct Cyc_List_List*tds=0;{struct _handler_cons _tmpB5;_push_handler(&
_tmpB5);{int _tmpB7=0;if(setjmp(_tmpB5.handler)){_tmpB7=1;}if(! _tmpB7){tds=((
struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(
struct Cyc_Std___sFILE*,struct Cyc_List_List*),struct Cyc_Std___sFILE*env,struct Cyc_List_List*
tds))Cyc_do_stage)(_tag_arr("parsing",sizeof(unsigned char),8),Cyc_do_parse,(
struct Cyc_Std___sFILE*)_check_null(in_file),tds);;_pop_handler();}else{void*
_tmpB6=(void*)_exn_thrown;void*_tmpB9=_tmpB6;_LL187: goto _LL188;_LL189: goto _LL190;
_LL188: Cyc_Std_file_close((struct Cyc_Std___sFILE*)_check_null(in_file));Cyc_remove_file((
struct _tagged_arr)_tmp9B);(int)_throw(_tmpB9);_LL190:(void)_throw(_tmpB9);_LL186:;}}}
Cyc_Std_file_close((struct Cyc_Std___sFILE*)_check_null(in_file));if(Cyc_compile_failure){
Cyc_remove_file((struct _tagged_arr)_tmp9B);return;}{struct Cyc_Tcenv_Tenv*_tmpBF=
Cyc_Tcenv_tc_init();if(Cyc_parseonly_r){goto PRINTC;}tds=((struct Cyc_List_List*(*)(
struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*tds))Cyc_do_stage)(
_tag_arr("type checking",sizeof(unsigned char),14),Cyc_do_typecheck,_tmpBF,tds);
if(Cyc_compile_failure){Cyc_remove_file((struct _tagged_arr)_tmp9B);return;}tds=((
struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(int,
struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(_tag_arr("control-flow checking",
sizeof(unsigned char),22),Cyc_do_cfcheck,1,tds);if(Cyc_compile_failure){Cyc_remove_file((
struct _tagged_arr)_tmp9B);return;}if(Cyc_ic_r){struct Cyc_Std___sFILE*inter_file=
Cyc_Std_fopen((struct _tagged_arr)_tmp9C,_tag_arr("r",sizeof(unsigned char),2));
struct Cyc_Std___sFILE*inter_objfile=Cyc_try_file_open((struct _tagged_arr)_tmp9D,
_tag_arr("w",sizeof(unsigned char),2),_tag_arr("interface object file",sizeof(
unsigned char),22));if(inter_objfile == 0){Cyc_compile_failure=1;return;}Cyc_Position_reset_position((
struct _tagged_arr)_tmp9C);tds=((struct Cyc_List_List*(*)(struct _tagged_arr
stage_name,struct Cyc_List_List*(*f)(struct _tuple9*,struct Cyc_List_List*),struct
_tuple9*env,struct Cyc_List_List*tds))Cyc_do_stage)(_tag_arr("interface checking",
sizeof(unsigned char),19),Cyc_do_interface,({struct _tuple9*_tmpC0=_cycalloc(
sizeof(struct _tuple9));_tmpC0->f1=_tmpBF;_tmpC0->f2=inter_file;_tmpC0->f3=(
struct Cyc_Std___sFILE*)_check_null(inter_objfile);_tmpC0;}),tds);if(inter_file != 
0){Cyc_Std_file_close((struct Cyc_Std___sFILE*)_check_null(inter_file));}Cyc_Std_file_close((
struct Cyc_Std___sFILE*)_check_null(inter_objfile));}if(Cyc_tc_r){goto PRINTC;}tds=((
struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(int,
struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(_tag_arr("translation to C",
sizeof(unsigned char),17),Cyc_do_translate,1,tds);if(Cyc_compile_failure){Cyc_remove_file((
struct _tagged_arr)_tmp9B);return;}if(! Cyc_compile_failure){Cyc_Tcutil_flush_warnings();}
Cyc_remove_file((struct _tagged_arr)_tmp9B);if(Cyc_compile_failure){return;}if(
Cyc_tovc_r){tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*
f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(
_tag_arr("post-pass to VC",sizeof(unsigned char),16),Cyc_do_tovc,1,tds);}if(Cyc_compile_failure){
return;}PRINTC: {struct Cyc_Std___sFILE*out_file;if(Cyc_parseonly_r? 1: Cyc_tc_r){
if(Cyc_output_file != 0){out_file=Cyc_try_file_open(*((struct _tagged_arr*)
_check_null(Cyc_output_file)),_tag_arr("w",sizeof(unsigned char),2),_tag_arr("output file",
sizeof(unsigned char),12));}else{out_file=(struct Cyc_Std___sFILE*)Cyc_Std_stdout;}}
else{if(Cyc_toc_r? Cyc_output_file != 0: 0){out_file=Cyc_try_file_open(*((struct
_tagged_arr*)_check_null(Cyc_output_file)),_tag_arr("w",sizeof(unsigned char),2),
_tag_arr("output file",sizeof(unsigned char),12));}else{out_file=Cyc_try_file_open((
struct _tagged_arr)_tmp9E,_tag_arr("w",sizeof(unsigned char),2),_tag_arr("output file",
sizeof(unsigned char),12));}}if(Cyc_compile_failure){return;}{struct
_handler_cons _tmpC1;_push_handler(& _tmpC1);{int _tmpC3=0;if(setjmp(_tmpC1.handler)){
_tmpC3=1;}if(! _tmpC3){tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,
struct Cyc_List_List*(*f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*),struct Cyc_Std___sFILE*
env,struct Cyc_List_List*tds))Cyc_do_stage)(_tag_arr("printing",sizeof(
unsigned char),9),Cyc_do_print,(struct Cyc_Std___sFILE*)_check_null(out_file),tds);;
_pop_handler();}else{void*_tmpC2=(void*)_exn_thrown;void*_tmpC5=_tmpC2;_LL199:
goto _LL200;_LL201: goto _LL202;_LL200: Cyc_compile_failure=1;Cyc_Std_file_close((
struct Cyc_Std___sFILE*)_check_null(out_file));Cyc_cfiles=({struct Cyc_List_List*
_tmpCB=_cycalloc(sizeof(struct Cyc_List_List));_tmpCB->hd=({struct _tagged_arr*
_tmpCC=_cycalloc(sizeof(struct _tagged_arr));_tmpCC[0]=(struct _tagged_arr)_tmp9E;
_tmpCC;});_tmpCB->tl=Cyc_cfiles;_tmpCB;});(int)_throw(_tmpC5);_LL202:(void)
_throw(_tmpC5);_LL198:;}}}Cyc_Std_file_close((struct Cyc_Std___sFILE*)_check_null(
out_file));Cyc_cfiles=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(struct Cyc_List_List));
_tmpCD->hd=({struct _tagged_arr*_tmpCE=_cycalloc(sizeof(struct _tagged_arr));
_tmpCE[0]=(struct _tagged_arr)_tmp9E;_tmpCE;});_tmpCD->tl=Cyc_cfiles;_tmpCD;});}}}}}}}}}}}
static unsigned char _tmpD7[8]="<final>";static struct _tagged_arr Cyc_final_str={
_tmpD7,_tmpD7,_tmpD7 + 8};static struct _tagged_arr*Cyc_final_strptr=& Cyc_final_str;
static struct Cyc_Interface_I*Cyc_read_cycio(struct _tagged_arr*n){if(n == (struct
_tagged_arr*)Cyc_final_strptr){return Cyc_Interface_final();}{struct _tagged_arr
basename;{struct _handler_cons _tmpD8;_push_handler(& _tmpD8);{int _tmpDA=0;if(
setjmp(_tmpD8.handler)){_tmpDA=1;}if(! _tmpDA){basename=(struct _tagged_arr)Cyc_Filename_chop_extension(*
n);;_pop_handler();}else{void*_tmpD9=(void*)_exn_thrown;void*_tmpDC=_tmpD9;
_LL222: if(*((void**)_tmpDC)== Cyc_Core_Invalid_argument){goto _LL223;}else{goto
_LL224;}_LL224: goto _LL225;_LL223: basename=*n;goto _LL221;_LL225:(void)_throw(
_tmpDC);_LL221:;}}}{struct _tagged_arr _tmpE2=Cyc_Std_strconcat(basename,_tag_arr(".cycio",
sizeof(unsigned char),7));struct Cyc_Std___sFILE*_tmpE3=Cyc_try_file_open((struct
_tagged_arr)_tmpE2,_tag_arr("r",sizeof(unsigned char),2),_tag_arr("interface object file",
sizeof(unsigned char),22));if(_tmpE3 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();
exit(1);}Cyc_Position_reset_position((struct _tagged_arr)_tmpE2);{struct Cyc_Interface_I*
_tmpE4=Cyc_Interface_load((struct Cyc_Std___sFILE*)_check_null(_tmpE3));Cyc_Std_file_close((
struct Cyc_Std___sFILE*)_check_null(_tmpE3));return _tmpE4;}}}}static int Cyc_is_cfile(
struct _tagged_arr*n){return*((const unsigned char*)_check_unknown_subscript(*n,
sizeof(unsigned char),0))!= '-';}extern void GC_blacklist_warn_clear();struct
_tuple10{struct _tagged_arr f1;int f2;struct _tagged_arr f3;void*f4;struct _tagged_arr
f5;};int Cyc_main(int argc,struct _tagged_arr argv){GC_blacklist_warn_clear();{
struct _tagged_arr comp=(struct _tagged_arr)Cstring_to_string(Ccomp);Cyc_set_cpp((
struct _tagged_arr)Cyc_Std_strconcat(comp,_tag_arr(" -x c -E -U__GNUC__ -nostdinc",
sizeof(unsigned char),30)));{struct Cyc_List_List*options=({struct _tuple10*
_tmp12F[48];_tmp12F[47]=({struct _tuple10*_tmp1BD=_cycalloc(sizeof(struct _tuple10));
_tmp1BD->f1=_tag_arr("-warnboundschecks",sizeof(unsigned char),18);_tmp1BD->f2=0;
_tmp1BD->f3=_tag_arr("",sizeof(unsigned char),1);_tmp1BD->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp1BE=_cycalloc(sizeof(struct Cyc_Arg_Set_spec_struct));_tmp1BE[0]=({struct Cyc_Arg_Set_spec_struct
_tmp1BF;_tmp1BF.tag=Cyc_Arg_Set_spec;_tmp1BF.f1=& Cyc_Toc_warn_bounds_checks;
_tmp1BF;});_tmp1BE;});_tmp1BD->f5=_tag_arr("Warn when bounds checks can't be eliminated",
sizeof(unsigned char),44);_tmp1BD;});_tmp12F[46]=({struct _tuple10*_tmp1BA=
_cycalloc(sizeof(struct _tuple10));_tmp1BA->f1=_tag_arr("-printfullevars",sizeof(
unsigned char),16);_tmp1BA->f2=0;_tmp1BA->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp1BA->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp1BB=_cycalloc(sizeof(
struct Cyc_Arg_Set_spec_struct));_tmp1BB[0]=({struct Cyc_Arg_Set_spec_struct
_tmp1BC;_tmp1BC.tag=Cyc_Arg_Set_spec;_tmp1BC.f1=& Cyc_print_full_evars_r;_tmp1BC;});
_tmp1BB;});_tmp1BA->f5=_tag_arr("Print full information for evars (type debugging)",
sizeof(unsigned char),50);_tmp1BA;});_tmp12F[45]=({struct _tuple10*_tmp1B7=
_cycalloc(sizeof(struct _tuple10));_tmp1B7->f1=_tag_arr("-printallkinds",sizeof(
unsigned char),15);_tmp1B7->f2=0;_tmp1B7->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp1B7->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp1B8=_cycalloc(sizeof(
struct Cyc_Arg_Set_spec_struct));_tmp1B8[0]=({struct Cyc_Arg_Set_spec_struct
_tmp1B9;_tmp1B9.tag=Cyc_Arg_Set_spec;_tmp1B9.f1=& Cyc_print_all_kinds_r;_tmp1B9;});
_tmp1B8;});_tmp1B7->f5=_tag_arr("Always print kinds of type variables",sizeof(
unsigned char),37);_tmp1B7;});_tmp12F[44]=({struct _tuple10*_tmp1B4=_cycalloc(
sizeof(struct _tuple10));_tmp1B4->f1=_tag_arr("-printalltvars",sizeof(
unsigned char),15);_tmp1B4->f2=0;_tmp1B4->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp1B4->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp1B5=_cycalloc(sizeof(
struct Cyc_Arg_Set_spec_struct));_tmp1B5[0]=({struct Cyc_Arg_Set_spec_struct
_tmp1B6;_tmp1B6.tag=Cyc_Arg_Set_spec;_tmp1B6.f1=& Cyc_print_all_tvars_r;_tmp1B6;});
_tmp1B5;});_tmp1B4->f5=_tag_arr("Print all type variables (even implicit default effects)",
sizeof(unsigned char),57);_tmp1B4;});_tmp12F[43]=({struct _tuple10*_tmp1B1=
_cycalloc(sizeof(struct _tuple10));_tmp1B1->f1=_tag_arr("-noexpandtypedefs",
sizeof(unsigned char),18);_tmp1B1->f2=0;_tmp1B1->f3=_tag_arr("",sizeof(
unsigned char),1);_tmp1B1->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp1B2=
_cycalloc(sizeof(struct Cyc_Arg_Set_spec_struct));_tmp1B2[0]=({struct Cyc_Arg_Set_spec_struct
_tmp1B3;_tmp1B3.tag=Cyc_Arg_Set_spec;_tmp1B3.f1=& Cyc_noexpand_r;_tmp1B3;});
_tmp1B2;});_tmp1B1->f5=_tag_arr("Don't expand typedefs in pretty printing",
sizeof(unsigned char),41);_tmp1B1;});_tmp12F[42]=({struct _tuple10*_tmp1AE=
_cycalloc(sizeof(struct _tuple10));_tmp1AE->f1=_tag_arr("-noremoveunused",sizeof(
unsigned char),16);_tmp1AE->f2=0;_tmp1AE->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp1AE->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp1AF=_cycalloc(sizeof(
struct Cyc_Arg_Set_spec_struct));_tmp1AF[0]=({struct Cyc_Arg_Set_spec_struct
_tmp1B0;_tmp1B0.tag=Cyc_Arg_Set_spec;_tmp1B0.f1=& Cyc_noshake_r;_tmp1B0;});
_tmp1AF;});_tmp1AE->f5=_tag_arr("Don't remove externed variables that aren't used",
sizeof(unsigned char),49);_tmp1AE;});_tmp12F[41]=({struct _tuple10*_tmp1AB=
_cycalloc(sizeof(struct _tuple10));_tmp1AB->f1=_tag_arr("-nogc",sizeof(
unsigned char),6);_tmp1AB->f2=0;_tmp1AB->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp1AB->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp1AC=_cycalloc(sizeof(
struct Cyc_Arg_Set_spec_struct));_tmp1AC[0]=({struct Cyc_Arg_Set_spec_struct
_tmp1AD;_tmp1AD.tag=Cyc_Arg_Set_spec;_tmp1AD.f1=& Cyc_nogc_r;_tmp1AD;});_tmp1AC;});
_tmp1AB->f5=_tag_arr("Don't link in the garbage collector",sizeof(unsigned char),
36);_tmp1AB;});_tmp12F[40]=({struct _tuple10*_tmp1A8=_cycalloc(sizeof(struct
_tuple10));_tmp1A8->f1=_tag_arr("-nocyc",sizeof(unsigned char),7);_tmp1A8->f2=0;
_tmp1A8->f3=_tag_arr("",sizeof(unsigned char),1);_tmp1A8->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp1A9=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_struct));_tmp1A9[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1AA;_tmp1AA.tag=Cyc_Arg_Unit_spec;_tmp1AA.f1=Cyc_set_nocyc;_tmp1AA;});
_tmp1A9;});_tmp1A8->f5=_tag_arr("Don't add implicit namespace Cyc",sizeof(
unsigned char),33);_tmp1A8;});_tmp12F[39]=({struct _tuple10*_tmp1A5=_cycalloc(
sizeof(struct _tuple10));_tmp1A5->f1=_tag_arr("-no-cpp-precomp",sizeof(
unsigned char),16);_tmp1A5->f2=0;_tmp1A5->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp1A5->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp1A6=_cycalloc(sizeof(
struct Cyc_Arg_Unit_spec_struct));_tmp1A6[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1A7;_tmp1A7.tag=Cyc_Arg_Unit_spec;_tmp1A7.f1=Cyc_set_nocppprecomp;_tmp1A7;});
_tmp1A6;});_tmp1A5->f5=_tag_arr("Don't do smart preprocessing (mac only)",
sizeof(unsigned char),40);_tmp1A5;});_tmp12F[38]=({struct _tuple10*_tmp1A2=
_cycalloc(sizeof(struct _tuple10));_tmp1A2->f1=_tag_arr("-use-cpp",sizeof(
unsigned char),9);_tmp1A2->f2=0;_tmp1A2->f3=_tag_arr("<path>",sizeof(
unsigned char),7);_tmp1A2->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp1A3=
_cycalloc(sizeof(struct Cyc_Arg_String_spec_struct));_tmp1A3[0]=({struct Cyc_Arg_String_spec_struct
_tmp1A4;_tmp1A4.tag=Cyc_Arg_String_spec;_tmp1A4.f1=Cyc_set_cpp;_tmp1A4;});
_tmp1A3;});_tmp1A2->f5=_tag_arr("Indicate which preprocessor to use",sizeof(
unsigned char),35);_tmp1A2;});_tmp12F[37]=({struct _tuple10*_tmp19F=_cycalloc(
sizeof(struct _tuple10));_tmp19F->f1=_tag_arr("--noboundschecks",sizeof(
unsigned char),17);_tmp19F->f2=0;_tmp19F->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp19F->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp1A0=_cycalloc(sizeof(
struct Cyc_Arg_Unit_spec_struct));_tmp1A0[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1A1;_tmp1A1.tag=Cyc_Arg_Unit_spec;_tmp1A1.f1=Cyc_set_noboundschecks;_tmp1A1;});
_tmp1A0;});_tmp19F->f5=_tag_arr("Disable bounds checks",sizeof(unsigned char),22);
_tmp19F;});_tmp12F[36]=({struct _tuple10*_tmp19C=_cycalloc(sizeof(struct _tuple10));
_tmp19C->f1=_tag_arr("--nonullchecks",sizeof(unsigned char),15);_tmp19C->f2=0;
_tmp19C->f3=_tag_arr("",sizeof(unsigned char),1);_tmp19C->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp19D=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_struct));_tmp19D[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp19E;_tmp19E.tag=Cyc_Arg_Unit_spec;_tmp19E.f1=Cyc_set_nonullchecks;_tmp19E;});
_tmp19D;});_tmp19C->f5=_tag_arr("Disable null checks",sizeof(unsigned char),20);
_tmp19C;});_tmp12F[35]=({struct _tuple10*_tmp199=_cycalloc(sizeof(struct _tuple10));
_tmp199->f1=_tag_arr("--nochecks",sizeof(unsigned char),11);_tmp199->f2=0;
_tmp199->f3=_tag_arr("",sizeof(unsigned char),1);_tmp199->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp19A=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_struct));_tmp19A[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp19B;_tmp19B.tag=Cyc_Arg_Unit_spec;_tmp19B.f1=Cyc_set_nochecks;_tmp19B;});
_tmp19A;});_tmp199->f5=_tag_arr("Disable bounds/null checks",sizeof(
unsigned char),27);_tmp199;});_tmp12F[34]=({struct _tuple10*_tmp196=_cycalloc(
sizeof(struct _tuple10));_tmp196->f1=_tag_arr("--lineno",sizeof(unsigned char),9);
_tmp196->f2=0;_tmp196->f3=_tag_arr("",sizeof(unsigned char),1);_tmp196->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp197=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_struct));
_tmp197[0]=({struct Cyc_Arg_Unit_spec_struct _tmp198;_tmp198.tag=Cyc_Arg_Unit_spec;
_tmp198.f1=Cyc_set_lineno;_tmp198;});_tmp197;});_tmp196->f5=_tag_arr("Generate line numbers for debugging",
sizeof(unsigned char),36);_tmp196;});_tmp12F[33]=({struct _tuple10*_tmp193=
_cycalloc(sizeof(struct _tuple10));_tmp193->f1=_tag_arr("-save-c",sizeof(
unsigned char),8);_tmp193->f2=0;_tmp193->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp193->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp194=_cycalloc(sizeof(
struct Cyc_Arg_Set_spec_struct));_tmp194[0]=({struct Cyc_Arg_Set_spec_struct
_tmp195;_tmp195.tag=Cyc_Arg_Set_spec;_tmp195.f1=& Cyc_save_c_r;_tmp195;});_tmp194;});
_tmp193->f5=_tag_arr("Don't delete temporary C files",sizeof(unsigned char),31);
_tmp193;});_tmp12F[32]=({struct _tuple10*_tmp190=_cycalloc(sizeof(struct _tuple10));
_tmp190->f1=_tag_arr("-save-temps",sizeof(unsigned char),12);_tmp190->f2=0;
_tmp190->f3=_tag_arr("",sizeof(unsigned char),1);_tmp190->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp191=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_struct));_tmp191[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp192;_tmp192.tag=Cyc_Arg_Unit_spec;_tmp192.f1=Cyc_set_save_temps;_tmp192;});
_tmp191;});_tmp190->f5=_tag_arr("Don't delete temporary files",sizeof(
unsigned char),29);_tmp190;});_tmp12F[31]=({struct _tuple10*_tmp18D=_cycalloc(
sizeof(struct _tuple10));_tmp18D->f1=_tag_arr("-tovc",sizeof(unsigned char),6);
_tmp18D->f2=0;_tmp18D->f3=_tag_arr("",sizeof(unsigned char),1);_tmp18D->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp18E=_cycalloc(sizeof(struct Cyc_Arg_Set_spec_struct));
_tmp18E[0]=({struct Cyc_Arg_Set_spec_struct _tmp18F;_tmp18F.tag=Cyc_Arg_Set_spec;
_tmp18F.f1=& Cyc_tovc_r;_tmp18F;});_tmp18E;});_tmp18D->f5=_tag_arr("Avoid gcc extensions in C output",
sizeof(unsigned char),33);_tmp18D;});_tmp12F[30]=({struct _tuple10*_tmp18A=
_cycalloc(sizeof(struct _tuple10));_tmp18A->f1=_tag_arr("-up",sizeof(
unsigned char),4);_tmp18A->f2=0;_tmp18A->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp18A->f4=(void*)({struct Cyc_Arg_Clear_spec_struct*_tmp18B=_cycalloc(sizeof(
struct Cyc_Arg_Clear_spec_struct));_tmp18B[0]=({struct Cyc_Arg_Clear_spec_struct
_tmp18C;_tmp18C.tag=Cyc_Arg_Clear_spec;_tmp18C.f1=& Cyc_pp_r;_tmp18C;});_tmp18B;});
_tmp18A->f5=_tag_arr("Ugly print",sizeof(unsigned char),11);_tmp18A;});_tmp12F[
29]=({struct _tuple10*_tmp187=_cycalloc(sizeof(struct _tuple10));_tmp187->f1=
_tag_arr("-pp",sizeof(unsigned char),4);_tmp187->f2=0;_tmp187->f3=_tag_arr("",
sizeof(unsigned char),1);_tmp187->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp188=_cycalloc(sizeof(struct Cyc_Arg_Set_spec_struct));_tmp188[0]=({struct Cyc_Arg_Set_spec_struct
_tmp189;_tmp189.tag=Cyc_Arg_Set_spec;_tmp189.f1=& Cyc_pp_r;_tmp189;});_tmp188;});
_tmp187->f5=_tag_arr("Pretty print",sizeof(unsigned char),13);_tmp187;});_tmp12F[
28]=({struct _tuple10*_tmp184=_cycalloc(sizeof(struct _tuple10));_tmp184->f1=
_tag_arr("-ic",sizeof(unsigned char),4);_tmp184->f2=0;_tmp184->f3=_tag_arr("",
sizeof(unsigned char),1);_tmp184->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp185=_cycalloc(sizeof(struct Cyc_Arg_Set_spec_struct));_tmp185[0]=({struct Cyc_Arg_Set_spec_struct
_tmp186;_tmp186.tag=Cyc_Arg_Set_spec;_tmp186.f1=& Cyc_ic_r;_tmp186;});_tmp185;});
_tmp184->f5=_tag_arr("Activate the link-checker",sizeof(unsigned char),26);
_tmp184;});_tmp12F[27]=({struct _tuple10*_tmp181=_cycalloc(sizeof(struct _tuple10));
_tmp181->f1=_tag_arr("-stopafter-toc",sizeof(unsigned char),15);_tmp181->f2=0;
_tmp181->f3=_tag_arr("",sizeof(unsigned char),1);_tmp181->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp182=_cycalloc(sizeof(struct Cyc_Arg_Set_spec_struct));_tmp182[0]=({struct Cyc_Arg_Set_spec_struct
_tmp183;_tmp183.tag=Cyc_Arg_Set_spec;_tmp183.f1=& Cyc_toc_r;_tmp183;});_tmp182;});
_tmp181->f5=_tag_arr("Stop after translation to C",sizeof(unsigned char),28);
_tmp181;});_tmp12F[26]=({struct _tuple10*_tmp17E=_cycalloc(sizeof(struct _tuple10));
_tmp17E->f1=_tag_arr("-stopafter-tc",sizeof(unsigned char),14);_tmp17E->f2=0;
_tmp17E->f3=_tag_arr("",sizeof(unsigned char),1);_tmp17E->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp17F=_cycalloc(sizeof(struct Cyc_Arg_Set_spec_struct));_tmp17F[0]=({struct Cyc_Arg_Set_spec_struct
_tmp180;_tmp180.tag=Cyc_Arg_Set_spec;_tmp180.f1=& Cyc_tc_r;_tmp180;});_tmp17F;});
_tmp17E->f5=_tag_arr("Stop after type checking",sizeof(unsigned char),25);
_tmp17E;});_tmp12F[25]=({struct _tuple10*_tmp17B=_cycalloc(sizeof(struct _tuple10));
_tmp17B->f1=_tag_arr("-stopafter-parse",sizeof(unsigned char),17);_tmp17B->f2=0;
_tmp17B->f3=_tag_arr("",sizeof(unsigned char),1);_tmp17B->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp17C=_cycalloc(sizeof(struct Cyc_Arg_Set_spec_struct));_tmp17C[0]=({struct Cyc_Arg_Set_spec_struct
_tmp17D;_tmp17D.tag=Cyc_Arg_Set_spec;_tmp17D.f1=& Cyc_parseonly_r;_tmp17D;});
_tmp17C;});_tmp17B->f5=_tag_arr("Stop after parsing",sizeof(unsigned char),19);
_tmp17B;});_tmp12F[24]=({struct _tuple10*_tmp178=_cycalloc(sizeof(struct _tuple10));
_tmp178->f1=_tag_arr("-E",sizeof(unsigned char),3);_tmp178->f2=0;_tmp178->f3=
_tag_arr("",sizeof(unsigned char),1);_tmp178->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp179=_cycalloc(sizeof(struct Cyc_Arg_Set_spec_struct));_tmp179[0]=({struct Cyc_Arg_Set_spec_struct
_tmp17A;_tmp17A.tag=Cyc_Arg_Set_spec;_tmp17A.f1=& Cyc_stop_after_cpp_r;_tmp17A;});
_tmp179;});_tmp178->f5=_tag_arr("Stop after preprocessing",sizeof(unsigned char),
25);_tmp178;});_tmp12F[23]=({struct _tuple10*_tmp175=_cycalloc(sizeof(struct
_tuple10));_tmp175->f1=_tag_arr("-MT",sizeof(unsigned char),4);_tmp175->f2=0;
_tmp175->f3=_tag_arr(" <target>",sizeof(unsigned char),10);_tmp175->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp176=_cycalloc(sizeof(struct Cyc_Arg_String_spec_struct));
_tmp176[0]=({struct Cyc_Arg_String_spec_struct _tmp177;_tmp177.tag=Cyc_Arg_String_spec;
_tmp177.f1=Cyc_set_dependencies_target;_tmp177;});_tmp176;});_tmp175->f5=
_tag_arr("Give target for dependencies",sizeof(unsigned char),29);_tmp175;});
_tmp12F[22]=({struct _tuple10*_tmp172=_cycalloc(sizeof(struct _tuple10));_tmp172->f1=
_tag_arr("-MG",sizeof(unsigned char),4);_tmp172->f2=0;_tmp172->f3=_tag_arr("",
sizeof(unsigned char),1);_tmp172->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp173=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));_tmp173[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp174;_tmp174.tag=Cyc_Arg_Flag_spec;_tmp174.f1=Cyc_add_cpparg;_tmp174;});
_tmp173;});_tmp172->f5=_tag_arr("When producing dependencies assume that missing files are generated",
sizeof(unsigned char),68);_tmp172;});_tmp12F[21]=({struct _tuple10*_tmp16F=
_cycalloc(sizeof(struct _tuple10));_tmp16F->f1=_tag_arr("-M",sizeof(unsigned char),
3);_tmp16F->f2=0;_tmp16F->f3=_tag_arr("",sizeof(unsigned char),1);_tmp16F->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp170=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_struct));
_tmp170[0]=({struct Cyc_Arg_Unit_spec_struct _tmp171;_tmp171.tag=Cyc_Arg_Unit_spec;
_tmp171.f1=Cyc_set_produce_dependencies;_tmp171;});_tmp170;});_tmp16F->f5=
_tag_arr("Produce dependencies",sizeof(unsigned char),21);_tmp16F;});_tmp12F[20]=({
struct _tuple10*_tmp16C=_cycalloc(sizeof(struct _tuple10));_tmp16C->f1=_tag_arr("-S",
sizeof(unsigned char),3);_tmp16C->f2=0;_tmp16C->f3=_tag_arr("",sizeof(
unsigned char),1);_tmp16C->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp16D=
_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_struct));_tmp16D[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp16E;_tmp16E.tag=Cyc_Arg_Unit_spec;_tmp16E.f1=Cyc_set_stop_after_asmfile;
_tmp16E;});_tmp16D;});_tmp16C->f5=_tag_arr("Stop after producing assembly code",
sizeof(unsigned char),35);_tmp16C;});_tmp12F[19]=({struct _tuple10*_tmp169=
_cycalloc(sizeof(struct _tuple10));_tmp169->f1=_tag_arr("-pa",sizeof(
unsigned char),4);_tmp169->f2=0;_tmp169->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp169->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp16A=_cycalloc(sizeof(
struct Cyc_Arg_Unit_spec_struct));_tmp16A[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp16B;_tmp16B.tag=Cyc_Arg_Unit_spec;_tmp16B.f1=Cyc_set_pa;_tmp16B;});_tmp16A;});
_tmp169->f5=_tag_arr("Compile for profiling with aprof",sizeof(unsigned char),33);
_tmp169;});_tmp12F[18]=({struct _tuple10*_tmp166=_cycalloc(sizeof(struct _tuple10));
_tmp166->f1=_tag_arr("-pg",sizeof(unsigned char),4);_tmp166->f2=0;_tmp166->f3=
_tag_arr("",sizeof(unsigned char),1);_tmp166->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp167=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));_tmp167[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp168;_tmp168.tag=Cyc_Arg_Flag_spec;_tmp168.f1=Cyc_add_ccarg;_tmp168;});
_tmp167;});_tmp166->f5=_tag_arr("Compile for profiling with gprof",sizeof(
unsigned char),33);_tmp166;});_tmp12F[17]=({struct _tuple10*_tmp163=_cycalloc(
sizeof(struct _tuple10));_tmp163->f1=_tag_arr("-p",sizeof(unsigned char),3);
_tmp163->f2=0;_tmp163->f3=_tag_arr("",sizeof(unsigned char),1);_tmp163->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp164=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));
_tmp164[0]=({struct Cyc_Arg_Flag_spec_struct _tmp165;_tmp165.tag=Cyc_Arg_Flag_spec;
_tmp165.f1=Cyc_add_ccarg;_tmp165;});_tmp164;});_tmp163->f5=_tag_arr("Compile for profiling with prof",
sizeof(unsigned char),32);_tmp163;});_tmp12F[16]=({struct _tuple10*_tmp160=
_cycalloc(sizeof(struct _tuple10));_tmp160->f1=_tag_arr("-g",sizeof(unsigned char),
3);_tmp160->f2=0;_tmp160->f3=_tag_arr("",sizeof(unsigned char),1);_tmp160->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp161=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));
_tmp161[0]=({struct Cyc_Arg_Flag_spec_struct _tmp162;_tmp162.tag=Cyc_Arg_Flag_spec;
_tmp162.f1=Cyc_add_ccarg;_tmp162;});_tmp161;});_tmp160->f5=_tag_arr("Compile for debugging",
sizeof(unsigned char),22);_tmp160;});_tmp12F[15]=({struct _tuple10*_tmp15D=
_cycalloc(sizeof(struct _tuple10));_tmp15D->f1=_tag_arr("-fomit-frame-pointer",
sizeof(unsigned char),21);_tmp15D->f2=0;_tmp15D->f3=_tag_arr("",sizeof(
unsigned char),1);_tmp15D->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp15E=
_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));_tmp15E[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp15F;_tmp15F.tag=Cyc_Arg_Flag_spec;_tmp15F.f1=Cyc_add_ccarg;_tmp15F;});
_tmp15E;});_tmp15D->f5=_tag_arr("Omit frame pointer",sizeof(unsigned char),19);
_tmp15D;});_tmp12F[14]=({struct _tuple10*_tmp15A=_cycalloc(sizeof(struct _tuple10));
_tmp15A->f1=_tag_arr("-O3",sizeof(unsigned char),4);_tmp15A->f2=0;_tmp15A->f3=
_tag_arr("",sizeof(unsigned char),1);_tmp15A->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp15B=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));_tmp15B[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp15C;_tmp15C.tag=Cyc_Arg_Flag_spec;_tmp15C.f1=Cyc_add_ccarg;_tmp15C;});
_tmp15B;});_tmp15A->f5=_tag_arr("Even more optimization",sizeof(unsigned char),
23);_tmp15A;});_tmp12F[13]=({struct _tuple10*_tmp157=_cycalloc(sizeof(struct
_tuple10));_tmp157->f1=_tag_arr("-O2",sizeof(unsigned char),4);_tmp157->f2=0;
_tmp157->f3=_tag_arr("",sizeof(unsigned char),1);_tmp157->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp158=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));_tmp158[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp159;_tmp159.tag=Cyc_Arg_Flag_spec;_tmp159.f1=Cyc_add_ccarg;_tmp159;});
_tmp158;});_tmp157->f5=_tag_arr("A higher level of optimization",sizeof(
unsigned char),31);_tmp157;});_tmp12F[12]=({struct _tuple10*_tmp154=_cycalloc(
sizeof(struct _tuple10));_tmp154->f1=_tag_arr("-O",sizeof(unsigned char),3);
_tmp154->f2=0;_tmp154->f3=_tag_arr("",sizeof(unsigned char),1);_tmp154->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp155=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));
_tmp155[0]=({struct Cyc_Arg_Flag_spec_struct _tmp156;_tmp156.tag=Cyc_Arg_Flag_spec;
_tmp156.f1=Cyc_add_ccarg;_tmp156;});_tmp155;});_tmp154->f5=_tag_arr("Optimize",
sizeof(unsigned char),9);_tmp154;});_tmp12F[11]=({struct _tuple10*_tmp151=
_cycalloc(sizeof(struct _tuple10));_tmp151->f1=_tag_arr("-O0",sizeof(
unsigned char),4);_tmp151->f2=0;_tmp151->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp151->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp152=_cycalloc(sizeof(
struct Cyc_Arg_Flag_spec_struct));_tmp152[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp153;_tmp153.tag=Cyc_Arg_Flag_spec;_tmp153.f1=Cyc_add_ccarg;_tmp153;});
_tmp152;});_tmp151->f5=_tag_arr("Don't optimize",sizeof(unsigned char),15);
_tmp151;});_tmp12F[10]=({struct _tuple10*_tmp14E=_cycalloc(sizeof(struct _tuple10));
_tmp14E->f1=_tag_arr("-s",sizeof(unsigned char),3);_tmp14E->f2=0;_tmp14E->f3=
_tag_arr("",sizeof(unsigned char),1);_tmp14E->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp14F=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));_tmp14F[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp150;_tmp150.tag=Cyc_Arg_Flag_spec;_tmp150.f1=Cyc_add_ccarg;_tmp150;});
_tmp14F;});_tmp14E->f5=_tag_arr("Remove all symbol table and relocation info from executable",
sizeof(unsigned char),60);_tmp14E;});_tmp12F[9]=({struct _tuple10*_tmp14B=
_cycalloc(sizeof(struct _tuple10));_tmp14B->f1=_tag_arr("-x",sizeof(unsigned char),
3);_tmp14B->f2=0;_tmp14B->f3=_tag_arr(" <language>",sizeof(unsigned char),12);
_tmp14B->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp14C=_cycalloc(sizeof(
struct Cyc_Arg_String_spec_struct));_tmp14C[0]=({struct Cyc_Arg_String_spec_struct
_tmp14D;_tmp14D.tag=Cyc_Arg_String_spec;_tmp14D.f1=Cyc_set_inputtype;_tmp14D;});
_tmp14C;});_tmp14B->f5=_tag_arr("Specify <language> for the following input files",
sizeof(unsigned char),49);_tmp14B;});_tmp12F[8]=({struct _tuple10*_tmp148=
_cycalloc(sizeof(struct _tuple10));_tmp148->f1=_tag_arr("-c",sizeof(unsigned char),
3);_tmp148->f2=0;_tmp148->f3=_tag_arr("",sizeof(unsigned char),1);_tmp148->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp149=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_struct));
_tmp149[0]=({struct Cyc_Arg_Unit_spec_struct _tmp14A;_tmp14A.tag=Cyc_Arg_Unit_spec;
_tmp14A.f1=Cyc_set_stop_after_objectfile;_tmp14A;});_tmp149;});_tmp148->f5=
_tag_arr("Produce an object file instead of an executable; do not link",sizeof(
unsigned char),61);_tmp148;});_tmp12F[7]=({struct _tuple10*_tmp145=_cycalloc(
sizeof(struct _tuple10));_tmp145->f1=_tag_arr("-l",sizeof(unsigned char),3);
_tmp145->f2=1;_tmp145->f3=_tag_arr("<file>",sizeof(unsigned char),7);_tmp145->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp146=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));
_tmp146[0]=({struct Cyc_Arg_Flag_spec_struct _tmp147;_tmp147.tag=Cyc_Arg_Flag_spec;
_tmp147.f1=Cyc_add_libarg;_tmp147;});_tmp146;});_tmp145->f5=_tag_arr("Library file",
sizeof(unsigned char),13);_tmp145;});_tmp12F[6]=({struct _tuple10*_tmp142=
_cycalloc(sizeof(struct _tuple10));_tmp142->f1=_tag_arr("-L",sizeof(unsigned char),
3);_tmp142->f2=1;_tmp142->f3=_tag_arr("<dir>",sizeof(unsigned char),6);_tmp142->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp143=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));
_tmp143[0]=({struct Cyc_Arg_Flag_spec_struct _tmp144;_tmp144.tag=Cyc_Arg_Flag_spec;
_tmp144.f1=Cyc_add_cyclone_lib_path;_tmp144;});_tmp143;});_tmp142->f5=_tag_arr("Add to the list of directories for -l",
sizeof(unsigned char),38);_tmp142;});_tmp12F[5]=({struct _tuple10*_tmp13F=
_cycalloc(sizeof(struct _tuple10));_tmp13F->f1=_tag_arr("-I",sizeof(unsigned char),
3);_tmp13F->f2=1;_tmp13F->f3=_tag_arr("<dir>",sizeof(unsigned char),6);_tmp13F->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp140=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));
_tmp140[0]=({struct Cyc_Arg_Flag_spec_struct _tmp141;_tmp141.tag=Cyc_Arg_Flag_spec;
_tmp141.f1=Cyc_add_cpparg;_tmp141;});_tmp140;});_tmp13F->f5=_tag_arr("Add to the list of directories to search for include files",
sizeof(unsigned char),59);_tmp13F;});_tmp12F[4]=({struct _tuple10*_tmp13C=
_cycalloc(sizeof(struct _tuple10));_tmp13C->f1=_tag_arr("-B",sizeof(unsigned char),
3);_tmp13C->f2=1;_tmp13C->f3=_tag_arr("<file>",sizeof(unsigned char),7);_tmp13C->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp13D=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_struct));
_tmp13D[0]=({struct Cyc_Arg_Flag_spec_struct _tmp13E;_tmp13E.tag=Cyc_Arg_Flag_spec;
_tmp13E.f1=Cyc_add_cyclone_lib_path;_tmp13E;});_tmp13D;});_tmp13C->f5=_tag_arr("Add to the list of directories to search for compiler files",
sizeof(unsigned char),60);_tmp13C;});_tmp12F[3]=({struct _tuple10*_tmp139=
_cycalloc(sizeof(struct _tuple10));_tmp139->f1=_tag_arr("-D",sizeof(unsigned char),
3);_tmp139->f2=1;_tmp139->f3=_tag_arr("<name>[=<value>]",sizeof(unsigned char),
17);_tmp139->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp13A=_cycalloc(
sizeof(struct Cyc_Arg_Flag_spec_struct));_tmp13A[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp13B;_tmp13B.tag=Cyc_Arg_Flag_spec;_tmp13B.f1=Cyc_add_cpparg;_tmp13B;});
_tmp13A;});_tmp139->f5=_tag_arr("Pass definition to preprocessor",sizeof(
unsigned char),32);_tmp139;});_tmp12F[2]=({struct _tuple10*_tmp136=_cycalloc(
sizeof(struct _tuple10));_tmp136->f1=_tag_arr("-o",sizeof(unsigned char),3);
_tmp136->f2=0;_tmp136->f3=_tag_arr(" <file>",sizeof(unsigned char),8);_tmp136->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp137=_cycalloc(sizeof(struct Cyc_Arg_String_spec_struct));
_tmp137[0]=({struct Cyc_Arg_String_spec_struct _tmp138;_tmp138.tag=Cyc_Arg_String_spec;
_tmp138.f1=Cyc_set_output_file;_tmp138;});_tmp137;});_tmp136->f5=_tag_arr("Set the output file name to <file>",
sizeof(unsigned char),35);_tmp136;});_tmp12F[1]=({struct _tuple10*_tmp133=
_cycalloc(sizeof(struct _tuple10));_tmp133->f1=_tag_arr("--version",sizeof(
unsigned char),10);_tmp133->f2=0;_tmp133->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp133->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp134=_cycalloc(sizeof(
struct Cyc_Arg_Unit_spec_struct));_tmp134[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp135;_tmp135.tag=Cyc_Arg_Unit_spec;_tmp135.f1=Cyc_print_version;_tmp135;});
_tmp134;});_tmp133->f5=_tag_arr("Print version information and exit",sizeof(
unsigned char),35);_tmp133;});_tmp12F[0]=({struct _tuple10*_tmp130=_cycalloc(
sizeof(struct _tuple10));_tmp130->f1=_tag_arr("-v",sizeof(unsigned char),3);
_tmp130->f2=0;_tmp130->f3=_tag_arr("",sizeof(unsigned char),1);_tmp130->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp131=_cycalloc(sizeof(struct Cyc_Arg_Set_spec_struct));
_tmp131[0]=({struct Cyc_Arg_Set_spec_struct _tmp132;_tmp132.tag=Cyc_Arg_Set_spec;
_tmp132.f1=& Cyc_v_r;_tmp132;});_tmp131;});_tmp130->f5=_tag_arr("Print compilation stages verbosely",
sizeof(unsigned char),35);_tmp130;});((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp12F,sizeof(struct _tuple10*),48));});Cyc_Arg_parse(
options,Cyc_add_other,_tag_arr("Options:",sizeof(unsigned char),9),argv);if(Cyc_cyclone_files
== 0? Cyc_ccargs == 0: 0){({void*_tmpE5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("missing file",sizeof(unsigned char),13),_tag_arr(_tmpE5,sizeof(void*),
0));});exit(1);}{struct _tagged_arr cyclone_exec_prefix=(struct _tagged_arr)Cyc_Std_getenv(
_tag_arr("CYCLONE_EXEC_PREFIX",sizeof(unsigned char),20));if(cyclone_exec_prefix.curr
!= ((struct _tagged_arr)_tag_arr(0,0,0)).curr){Cyc_cyclone_lib_path=({struct Cyc_List_List*
_tmpE6=_cycalloc(sizeof(struct Cyc_List_List));_tmpE6->hd=({struct _tagged_arr*
_tmpE7=_cycalloc(sizeof(struct _tagged_arr));_tmpE7[0]=cyclone_exec_prefix;_tmpE7;});
_tmpE6->tl=Cyc_cyclone_lib_path;_tmpE6;});}{struct _tagged_arr def_lib_path=(
struct _tagged_arr)Cstring_to_string(Cdef_lib_path);if(Cyc_Std_strlen(
def_lib_path)> 0){Cyc_cyclone_lib_path=({struct Cyc_List_List*_tmpE8=_cycalloc(
sizeof(struct Cyc_List_List));_tmpE8->hd=({struct _tagged_arr*_tmpE9=_cycalloc(
sizeof(struct _tagged_arr));_tmpE9[0]=def_lib_path;_tmpE9;});_tmpE8->tl=Cyc_cyclone_lib_path;
_tmpE8;});}{struct Cyc_List_List*pp=Cyc_cyclone_lib_path;while(pp != 0){Cyc_add_ccarg((
struct _tagged_arr)Cyc_Std_strconcat(_tag_arr("-L",sizeof(unsigned char),3),*((
struct _tagged_arr*)pp->hd)));pp=pp->tl;}Cyc_cyclone_lib_path=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_lib_path);Cyc_cyc_include=
Cyc_do_find(Cyc_cyclone_lib_path,_tag_arr("include/cyc_include.h",sizeof(
unsigned char),22));{struct Cyc_List_List*_tmpEA=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmpEA != 0;_tmpEA=_tmpEA->tl){
Cyc_process_file(*((struct _tagged_arr*)_tmpEA->hd));if(Cyc_compile_failure){
return 1;}}}if(((Cyc_stop_after_cpp_r? 1: Cyc_parseonly_r)? 1: Cyc_tc_r)? 1: Cyc_toc_r){
return 0;}if(Cyc_ccargs == 0){return 0;}Cyc_ccargs=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{struct _tagged_arr _tmpEB=Cyc_Std_str_sepstr(((
struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs),_tag_arr(" ",sizeof(
unsigned char),2));Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(Cyc_libargs);{struct _tagged_arr _tmpEC=Cyc_Std_str_sepstr(({struct
Cyc_List_List*_tmp12E=_cycalloc(sizeof(struct Cyc_List_List));_tmp12E->hd=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(unsigned char),1);
_tmp12E->tl=((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs);
_tmp12E;}),_tag_arr(" ",sizeof(unsigned char),2));struct Cyc_List_List*stdlib;
struct _tagged_arr stdlib_string;int _tmpED=((Cyc_stop_after_asmfile_r? 1: Cyc_stop_after_objectfile_r)?
1:(Cyc_output_file != 0? Cyc_Filename_check_suffix(*((struct _tagged_arr*)
_check_null(Cyc_output_file)),_tag_arr(".a",sizeof(unsigned char),3)): 0))? 1:(Cyc_output_file
!= 0? Cyc_Filename_check_suffix(*((struct _tagged_arr*)_check_null(Cyc_output_file)),
_tag_arr(".lib",sizeof(unsigned char),5)): 0);if(_tmpED){stdlib=0;stdlib_string=
_tag_arr("",sizeof(unsigned char),1);}else{struct _tagged_arr libcyc_filename;
struct _tagged_arr nogc_filename;struct _tagged_arr gc_filename;if(Cyc_pa_r){
libcyc_filename=_tag_arr("libcyc_a.a",sizeof(unsigned char),11);nogc_filename=
_tag_arr("nogc_a.a",sizeof(unsigned char),9);}else{if(Cyc_nocheck_r){
libcyc_filename=_tag_arr("libcyc_nocheck.a",sizeof(unsigned char),17);
nogc_filename=_tag_arr("nogc_nocheck.a",sizeof(unsigned char),15);}else{
libcyc_filename=_tag_arr("libcyc.a",sizeof(unsigned char),9);nogc_filename=
_tag_arr("nogc.a",sizeof(unsigned char),7);}}gc_filename=_tag_arr("gc.a",sizeof(
unsigned char),5);{struct _tagged_arr _tmpEE=Cyc_do_find(Cyc_cyclone_lib_path,
libcyc_filename);struct _tagged_arr _tmpEF=Cyc_nogc_r? Cyc_do_find(Cyc_cyclone_lib_path,
nogc_filename): Cyc_do_find(Cyc_cyclone_lib_path,gc_filename);stdlib=({struct
_tagged_arr*_tmpF0[1];_tmpF0[0]=({struct _tagged_arr*_tmpF1=_cycalloc(sizeof(
struct _tagged_arr));_tmpF1[0]=_tmpEE;_tmpF1;});((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmpF0,sizeof(struct _tagged_arr*),1));});
stdlib_string=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpF4;_tmpF4.tag=
Cyc_Std_String_pa;_tmpF4.f1=(struct _tagged_arr)_tmpEF;{struct Cyc_Std_String_pa_struct
_tmpF3;_tmpF3.tag=Cyc_Std_String_pa;_tmpF3.f1=(struct _tagged_arr)_tmpEE;{void*
_tmpF2[2]={& _tmpF3,& _tmpF4};Cyc_Std_aprintf(_tag_arr(" %s %s",sizeof(
unsigned char),7),_tag_arr(_tmpF2,sizeof(void*),2));}}});}}if(Cyc_ic_r){struct
_handler_cons _tmpF5;_push_handler(& _tmpF5);{int _tmpF7=0;if(setjmp(_tmpF5.handler)){
_tmpF7=1;}if(! _tmpF7){Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct
_tagged_arr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*_tmpF8=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(Cyc_ccargs,Cyc_libargs));if(! _tmpED){_tmpF8=({struct Cyc_List_List*
_tmpF9=_cycalloc(sizeof(struct Cyc_List_List));_tmpF9->hd=Cyc_final_strptr;_tmpF9->tl=
_tmpF8;_tmpF9;});}{struct Cyc_Interface_I*_tmpFA=((struct Cyc_Interface_I*(*)(
struct Cyc_Interface_I*(*get)(struct _tagged_arr*),struct Cyc_List_List*la,struct
Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,_tmpF8,
_tmpF8);if(_tmpFA == 0){({void*_tmpFB[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: interfaces incompatible\n",sizeof(unsigned char),32),_tag_arr(
_tmpFB,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmpFC=
1;_npop_handler(0);return _tmpFC;}}if(_tmpED){if(Cyc_output_file != 0){struct
_tagged_arr _tmpFD=({struct Cyc_Std_String_pa_struct _tmp101;_tmp101.tag=Cyc_Std_String_pa;
_tmp101.f1=(struct _tagged_arr)Cyc_Filename_chop_extension(*((struct _tagged_arr*)
_check_null(Cyc_output_file)));{void*_tmp100[1]={& _tmp101};Cyc_Std_aprintf(
_tag_arr("%s.cycio",sizeof(unsigned char),9),_tag_arr(_tmp100,sizeof(void*),1));}});
struct Cyc_Std___sFILE*_tmpFE=Cyc_try_file_open((struct _tagged_arr)_tmpFD,
_tag_arr("w",sizeof(unsigned char),2),_tag_arr("interface object file",sizeof(
unsigned char),22));if(_tmpFE == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmpFF=1;_npop_handler(0);return _tmpFF;}}Cyc_Interface_save((struct Cyc_Interface_I*)
_check_null(_tmpFA),(struct Cyc_Std___sFILE*)_check_null(_tmpFE));Cyc_Std_file_close((
struct Cyc_Std___sFILE*)_check_null(_tmpFE));}}else{if(! Cyc_Interface_is_subinterface(
Cyc_Interface_empty(),(struct Cyc_Interface_I*)_check_null(_tmpFA),({struct
_tuple8*_tmp102=_cycalloc(sizeof(struct _tuple8));_tmp102->f1=_tag_arr("empty interface",
sizeof(unsigned char),16);_tmp102->f2=_tag_arr("global interface",sizeof(
unsigned char),17);_tmp102;}))){({void*_tmp103[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: some objects are still undefined\n",sizeof(unsigned char),41),
_tag_arr(_tmp103,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp104=1;_npop_handler(0);return _tmp104;}}}}};_pop_handler();}else{void*
_tmpF6=(void*)_exn_thrown;void*_tmp106=_tmpF6;struct _tagged_arr _tmp114;struct
_tagged_arr _tmp116;struct _tagged_arr _tmp118;_LL264: if(*((void**)_tmp106)== Cyc_Core_Failure){
_LL277: _tmp114=((struct Cyc_Core_Failure_struct*)_tmp106)->f1;goto _LL265;}else{
goto _LL266;}_LL266: if(*((void**)_tmp106)== Cyc_Core_Impossible){_LL279: _tmp116=((
struct Cyc_Core_Impossible_struct*)_tmp106)->f1;goto _LL267;}else{goto _LL268;}
_LL268: if(_tmp106 == Cyc_Dict_Absent){goto _LL269;}else{goto _LL270;}_LL270: if(*((
void**)_tmp106)== Cyc_Core_Invalid_argument){_LL281: _tmp118=((struct Cyc_Core_Invalid_argument_struct*)
_tmp106)->f1;goto _LL271;}else{goto _LL272;}_LL272: goto _LL273;_LL274: goto _LL275;
_LL265:({struct Cyc_Std_String_pa_struct _tmp11B;_tmp11B.tag=Cyc_Std_String_pa;
_tmp11B.f1=(struct _tagged_arr)_tmp114;{void*_tmp11A[1]={& _tmp11B};Cyc_Std_printf(
_tag_arr("Exception Core::Failure %s\n",sizeof(unsigned char),28),_tag_arr(
_tmp11A,sizeof(void*),1));}});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL267:({struct Cyc_Std_String_pa_struct _tmp11D;_tmp11D.tag=Cyc_Std_String_pa;
_tmp11D.f1=(struct _tagged_arr)_tmp116;{void*_tmp11C[1]={& _tmp11D};Cyc_Std_printf(
_tag_arr("Exception Core::Impossible %s\n",sizeof(unsigned char),31),_tag_arr(
_tmp11C,sizeof(void*),1));}});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL269:({void*_tmp11E[0]={};Cyc_Std_printf(_tag_arr("Exception Dict::Absent\n",
sizeof(unsigned char),24),_tag_arr(_tmp11E,sizeof(void*),0));});Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;_LL271:({struct Cyc_Std_String_pa_struct _tmp120;
_tmp120.tag=Cyc_Std_String_pa;_tmp120.f1=(struct _tagged_arr)_tmp118;{void*
_tmp11F[1]={& _tmp120};Cyc_Std_printf(_tag_arr("Exception Core::Invalid_argument %s\n",
sizeof(unsigned char),37),_tag_arr(_tmp11F,sizeof(void*),1));}});Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;_LL273:({void*_tmp121[0]={};Cyc_Std_printf(
_tag_arr("Uncaught exception\n",sizeof(unsigned char),20),_tag_arr(_tmp121,
sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;_LL275:(
void)_throw(_tmp106);_LL263:;}}}{struct _tagged_arr _tmp122=({struct Cyc_Std_String_pa_struct
_tmp12B;_tmp12B.tag=Cyc_Std_String_pa;_tmp12B.f1=(struct _tagged_arr)_tmpEC;{
struct Cyc_Std_String_pa_struct _tmp12A;_tmp12A.tag=Cyc_Std_String_pa;_tmp12A.f1=(
struct _tagged_arr)stdlib_string;{struct Cyc_Std_String_pa_struct _tmp129;_tmp129.tag=
Cyc_Std_String_pa;_tmp129.f1=(struct _tagged_arr)_tmpEB;{struct Cyc_Std_String_pa_struct
_tmp128;_tmp128.tag=Cyc_Std_String_pa;_tmp128.f1=(struct _tagged_arr)(Cyc_output_file
== 0? _tag_arr("",sizeof(unsigned char),1):(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp12D;_tmp12D.tag=Cyc_Std_String_pa;_tmp12D.f1=(struct _tagged_arr)Cyc_sh_escape_string(*((
struct _tagged_arr*)_check_null(Cyc_output_file)));{void*_tmp12C[1]={& _tmp12D};
Cyc_Std_aprintf(_tag_arr(" -o %s",sizeof(unsigned char),7),_tag_arr(_tmp12C,
sizeof(void*),1));}}));{struct Cyc_Std_String_pa_struct _tmp127;_tmp127.tag=Cyc_Std_String_pa;
_tmp127.f1=(struct _tagged_arr)comp;{void*_tmp126[5]={& _tmp127,& _tmp128,& _tmp129,&
_tmp12A,& _tmp12B};Cyc_Std_aprintf(_tag_arr("%s %s %s%s%s",sizeof(unsigned char),
13),_tag_arr(_tmp126,sizeof(void*),5));}}}}}});if(Cyc_v_r){({struct Cyc_Std_String_pa_struct
_tmp124;_tmp124.tag=Cyc_Std_String_pa;_tmp124.f1=(struct _tagged_arr)_tmp122;{
void*_tmp123[1]={& _tmp124};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s\n",
sizeof(unsigned char),4),_tag_arr(_tmp123,sizeof(void*),1));}});}if(Cyc_Std_system((
struct _tagged_arr)_tmp122)!= 0){({void*_tmp125[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: C compiler failed\n",sizeof(unsigned char),26),_tag_arr(_tmp125,
sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;}Cyc_remove_cfiles();
return Cyc_compile_failure? 1: 0;}}}}}}}}}
