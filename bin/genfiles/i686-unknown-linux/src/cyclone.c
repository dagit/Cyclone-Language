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
Cyc_Arg_Error[10];struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};struct Cyc_Arg_FlagString_spec_struct{int
tag;void(*f1)(struct _tagged_arr,struct _tagged_arr);};struct Cyc_Arg_Set_spec_struct{
int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*
f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _tagged_arr);};
extern void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct
_tagged_arr),struct _tagged_arr errmsg,struct _tagged_arr args);struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE;extern struct Cyc_Std___sFILE*Cyc_Std_stdout;extern struct Cyc_Std___sFILE*
Cyc_Std_stderr;extern int Cyc_Std_remove(struct _tagged_arr);extern int Cyc_Std_fclose(
struct Cyc_Std___sFILE*);extern int Cyc_Std_fflush(struct Cyc_Std___sFILE*);extern
struct Cyc_Std___sFILE*Cyc_Std_fopen(struct _tagged_arr __filename,struct
_tagged_arr __modes);extern unsigned int Cyc_Std_fread(struct _tagged_arr __ptr,
unsigned int __size,unsigned int __n,struct Cyc_Std___sFILE*__stream);extern
unsigned int Cyc_Std_fwrite(struct _tagged_arr __ptr,unsigned int __size,
unsigned int __n,struct Cyc_Std___sFILE*__s);extern int Cyc_Std_feof(struct Cyc_Std___sFILE*
__stream);extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
extern struct Cyc_Std___sFILE*Cyc_Std_file_open(struct _tagged_arr fname,struct
_tagged_arr mode);extern void Cyc_Std_file_close(struct Cyc_Std___sFILE*);struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern
int Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct _tagged_arr fmt,struct _tagged_arr);
extern int Cyc_Std_printf(struct _tagged_arr fmt,struct _tagged_arr);extern struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};extern unsigned char Cyc_Array_Array_mismatch[19];struct Cyc_Std__Div{
int quot;int rem;};struct Cyc_Std__Ldiv{int quot;int rem;};extern struct _tagged_arr Cyc_Std_getenv(
struct _tagged_arr);extern int Cyc_Std_system(struct _tagged_arr);extern unsigned int
Cyc_Std_strlen(struct _tagged_arr s);extern int Cyc_Std_strcmp(struct _tagged_arr s1,
struct _tagged_arr s2);extern struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,
struct _tagged_arr);extern struct _tagged_arr Cyc_Std_strconcat_l(struct Cyc_List_List*);
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
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];
extern int Cyc_Position_error_p();struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple0{
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
15];extern unsigned char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{void(*
refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};extern struct
Cyc_List_List*Cyc_Parse_parse_file(struct Cyc_Std___sFILE*f);struct Cyc_Declaration_spec;
struct Cyc_Declarator;struct Cyc_Abstractdeclarator;extern int Cyc_yyparse();extern
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
f);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];struct Cyc_Tcenv_VarRes_struct{
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
le;};extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();extern unsigned char Cyc_Tcutil_TypeErr[
12];extern void Cyc_Tcutil_flush_warnings();extern void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*
te,int var_default_init,struct Cyc_List_List*ds);extern struct Cyc_List_List*Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);extern struct Cyc_List_List*Cyc_Toc_toc(
struct Cyc_List_List*ds);extern struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*
decls);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_CfFlowInfo_Place{
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
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};extern void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds);struct Cyc_Interface_I;extern struct Cyc_Interface_I*Cyc_Interface_empty();
extern struct Cyc_Interface_I*Cyc_Interface_final();extern struct Cyc_Interface_I*
Cyc_Interface_extract(struct Cyc_Dict_Dict*ae);struct _tuple8{struct _tagged_arr f1;
struct _tagged_arr f2;};extern int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*
i1,struct Cyc_Interface_I*i2,struct _tuple8*info);extern struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(
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
_tagged_arr s){Cyc_output_file=({struct _tagged_arr*_tmp0=_cycalloc(sizeof(*_tmp0));
_tmp0[0]=s;_tmp0;});}extern unsigned char*Cdef_inc_path;extern unsigned char*
Cdef_lib_path;extern unsigned char*Carch;extern unsigned char*Ccomp;extern
unsigned char*Cversion;static unsigned char _tmp1[1]="";static struct _tagged_arr Cyc_cpp={
_tmp1,_tmp1,_tmp1 + 1};static void Cyc_set_cpp(struct _tagged_arr s){Cyc_cpp=s;}
static struct Cyc_List_List*Cyc_cppargs=0;static void Cyc_add_cpparg(struct
_tagged_arr s){Cyc_cppargs=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));
_tmp2->hd=({struct _tagged_arr*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3[0]=s;_tmp3;});
_tmp2->tl=Cyc_cppargs;_tmp2;});}static void Cyc_print_version(){({struct Cyc_Std_String_pa_struct
_tmp5;_tmp5.tag=0;_tmp5.f1=(struct _tagged_arr)Cstring_to_string(Cversion);{void*
_tmp4[1]={& _tmp5};Cyc_Std_printf(_tag_arr("The Cyclone compiler, version %s\n",
sizeof(unsigned char),34),_tag_arr(_tmp4,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp8;_tmp8.tag=0;_tmp8.f1=(struct _tagged_arr)Cstring_to_string(Ccomp);{struct
Cyc_Std_String_pa_struct _tmp7;_tmp7.tag=0;_tmp7.f1=(struct _tagged_arr)
Cstring_to_string(Carch);{void*_tmp6[2]={& _tmp7,& _tmp8};Cyc_Std_printf(_tag_arr("Compiled for architecture: %s CC=\"%s\"\n",
sizeof(unsigned char),39),_tag_arr(_tmp6,sizeof(void*),2));}}});({struct Cyc_Std_String_pa_struct
_tmpA;_tmpA.tag=0;_tmpA.f1=(struct _tagged_arr)Cstring_to_string(Cdef_lib_path);{
void*_tmp9[1]={& _tmpA};Cyc_Std_printf(_tag_arr("Standard library directory: %s\n",
sizeof(unsigned char),32),_tag_arr(_tmp9,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmpC;_tmpC.tag=0;_tmpC.f1=(struct _tagged_arr)Cstring_to_string(Cdef_inc_path);{
void*_tmpB[1]={& _tmpC};Cyc_Std_printf(_tag_arr("Standard include directory: %s\n",
sizeof(unsigned char),32),_tag_arr(_tmpB,sizeof(void*),1));}});exit(0);}static
int Cyc_is_cyclone_sourcefile(struct _tagged_arr s){unsigned int _tmpD=Cyc_Std_strlen(
s);if(_tmpD <= 4){return 0;}else{return Cyc_Std_strcmp(_tagged_arr_plus(s,sizeof(
unsigned char),(int)(_tmpD - 4)),_tag_arr(".cyc",sizeof(unsigned char),5))== 0;}}
static struct Cyc_List_List*Cyc_ccargs=0;static void Cyc_add_ccarg(struct _tagged_arr
s){Cyc_ccargs=({struct Cyc_List_List*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->hd=({
struct _tagged_arr*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=s;_tmpF;});_tmpE->tl=
Cyc_ccargs;_tmpE;});}static struct Cyc_List_List*Cyc_cyclone_lib_path=0;static void
Cyc_add_cyclone_lib_path(struct _tagged_arr s){unsigned int _tmp10=Cyc_Std_strlen(s);
if(_tmp10 <= 2){return;}{struct _tagged_arr _tmp11=(struct _tagged_arr)Cyc_Std_substring(
s,2,_tmp10 - 2);Cyc_cyclone_lib_path=({struct Cyc_List_List*_tmp12=_cycalloc(
sizeof(*_tmp12));_tmp12->hd=({struct _tagged_arr*_tmp13=_cycalloc(sizeof(*_tmp13));
_tmp13[0]=_tmp11;_tmp13;});_tmp12->tl=Cyc_cyclone_lib_path;_tmp12;});}}static
struct Cyc_List_List*Cyc_libargs=0;static void Cyc_add_libarg(struct _tagged_arr s){
if(Cyc_Std_strcmp(s,_tag_arr("-lxml",sizeof(unsigned char),6))== 0){Cyc_add_ccarg(
s);}else{Cyc_libargs=({struct Cyc_List_List*_tmp14=_cycalloc(sizeof(*_tmp14));
_tmp14->hd=({struct _tagged_arr*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15[0]=s;
_tmp15;});_tmp14->tl=Cyc_libargs;_tmp14;});}}static void Cyc_set_save_temps(){Cyc_save_temps_r=
1;Cyc_add_ccarg(_tag_arr("-save-temps",sizeof(unsigned char),12));}static int Cyc_produce_dependencies=
0;static void Cyc_set_produce_dependencies(){Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=
1;Cyc_add_cpparg(_tag_arr("-M",sizeof(unsigned char),3));}static struct
_tagged_arr*Cyc_dependencies_target=0;static void Cyc_set_dependencies_target(
struct _tagged_arr s){Cyc_dependencies_target=({struct _tagged_arr*_tmp16=_cycalloc(
sizeof(*_tmp16));_tmp16[0]=s;_tmp16;});}static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;Cyc_add_ccarg(_tag_arr("-c",sizeof(unsigned char),
3));}static void Cyc_set_nocppprecomp(){Cyc_add_cpparg(_tag_arr("-no-cpp-precomp",
sizeof(unsigned char),16));Cyc_add_ccarg(_tag_arr("-no-cpp-precomp",sizeof(
unsigned char),16));}static void Cyc_set_lineno(){Cyc_generate_line_directives_r=1;
Cyc_set_save_temps();}static void Cyc_set_noboundschecks(){Cyc_add_ccarg(_tag_arr("-DNO_CYC_BOUNDS_CHECKS",
sizeof(unsigned char),23));}static void Cyc_set_nonullchecks(){Cyc_add_ccarg(
_tag_arr("-DNO_CYC_NULL_CHECKS",sizeof(unsigned char),21));}static void Cyc_set_nochecks(){
Cyc_set_noboundschecks();Cyc_set_nonullchecks();Cyc_nocheck_r=1;}static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r=0;Cyc_add_ccarg(_tag_arr("-DNO_CYC_PREFIX",sizeof(
unsigned char),16));}static void Cyc_set_pa(){Cyc_pa_r=1;Cyc_add_ccarg(_tag_arr("-DCYC_REGION_PROFILE",
sizeof(unsigned char),21));}static void Cyc_set_stop_after_asmfile(){Cyc_stop_after_asmfile_r=
1;Cyc_add_ccarg(_tag_arr("-S",sizeof(unsigned char),3));}static void*Cyc_intype=(
void*)0;static void Cyc_set_inputtype(struct _tagged_arr s){if(Cyc_Std_strcmp(s,
_tag_arr("cyc",sizeof(unsigned char),4))== 0){Cyc_intype=(void*)1;}else{if(Cyc_Std_strcmp(
s,_tag_arr("none",sizeof(unsigned char),5))== 0){Cyc_intype=(void*)0;}else{({
struct Cyc_Std_String_pa_struct _tmp18;_tmp18.tag=0;_tmp18.f1=(struct _tagged_arr)s;{
void*_tmp17[1]={& _tmp18};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Input type '%s' not supported\n",
sizeof(unsigned char),31),_tag_arr(_tmp17,sizeof(void*),1));}});}}}struct
_tagged_arr Cyc_make_base_filename(struct _tagged_arr s,struct _tagged_arr*
output_file){struct _tagged_arr _tmp19=Cyc_Filename_dirname(output_file == 0? s:*
output_file);struct _tagged_arr _tmp1A=Cyc_Filename_chop_extension((struct
_tagged_arr)Cyc_Filename_basename(s));struct _tagged_arr _tmp1B=Cyc_Std_strlen((
struct _tagged_arr)_tmp19)> 0? Cyc_Filename_concat((struct _tagged_arr)_tmp19,(
struct _tagged_arr)_tmp1A): _tmp1A;return(struct _tagged_arr)_tmp1B;}static struct
Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(struct _tagged_arr s){
if(Cyc_intype == (void*)1? 1: Cyc_is_cyclone_sourcefile(s)){Cyc_cyclone_files=({
struct Cyc_List_List*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->hd=({struct
_tagged_arr*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D[0]=s;_tmp1D;});_tmp1C->tl=
Cyc_cyclone_files;_tmp1C;});{struct _tagged_arr _tmp1E=Cyc_make_base_filename(s,
Cyc_output_file);struct _tagged_arr _tmp1F=Cyc_Std_strconcat(_tmp1E,_tag_arr(".c",
sizeof(unsigned char),3));Cyc_add_ccarg((struct _tagged_arr)_tmp1F);}}else{Cyc_add_ccarg(
s);}}static void Cyc_remove_file(struct _tagged_arr s){if(Cyc_save_temps_r){return;}
else{Cyc_Std_remove(s);}}int Cyc_compile_failure=0;struct Cyc_Std___sFILE*Cyc_try_file_open(
struct _tagged_arr filename,struct _tagged_arr mode,struct _tagged_arr msg_part){
struct _handler_cons _tmp20;_push_handler(& _tmp20);{int _tmp22=0;if(setjmp(_tmp20.handler)){
_tmp22=1;}if(! _tmp22){{struct Cyc_Std___sFILE*_tmp23=(struct Cyc_Std___sFILE*)Cyc_Std_file_open(
filename,mode);_npop_handler(0);return _tmp23;};_pop_handler();}else{void*_tmp21=(
void*)_exn_thrown;void*_tmp25=_tmp21;_LL1: goto _LL2;_LL3: goto _LL4;_LL2: Cyc_compile_failure=
1;({struct Cyc_Std_String_pa_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct
_tagged_arr)filename;{struct Cyc_Std_String_pa_struct _tmp27;_tmp27.tag=0;_tmp27.f1=(
struct _tagged_arr)msg_part;{void*_tmp26[2]={& _tmp27,& _tmp28};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\nError: couldn't open %s %s\n",sizeof(unsigned char),29),_tag_arr(
_tmp26,sizeof(void*),2));}}});Cyc_Std_fflush((struct Cyc_Std___sFILE*)Cyc_Std_stderr);
return 0;_LL4:(void)_throw(_tmp25);_LL0:;}}}struct Cyc_List_List*Cyc_do_stage(
struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),
void*env,struct Cyc_List_List*tds){struct _tagged_arr exn_string=_tag_arr("",
sizeof(unsigned char),1);struct _tagged_arr explain_string=_tag_arr("",sizeof(
unsigned char),1);int other_exn=0;void*ex=(void*)({struct Cyc_Core_Impossible_struct*
_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Core_Impossible_struct
_tmp3A;_tmp3A.tag=Cyc_Core_Impossible;_tmp3A.f1=_tag_arr("",sizeof(unsigned char),
1);_tmp3A;});_tmp39;});struct Cyc_List_List*_tmp29=0;{struct _handler_cons _tmp2A;
_push_handler(& _tmp2A);{int _tmp2C=0;if(setjmp(_tmp2A.handler)){_tmp2C=1;}if(!
_tmp2C){_tmp29=f(env,tds);;_pop_handler();}else{void*_tmp2B=(void*)_exn_thrown;
void*_tmp2E=_tmp2B;struct _tagged_arr _tmp2F;struct _tagged_arr _tmp30;struct
_tagged_arr _tmp31;_LL6: if(*((void**)_tmp2E)== Cyc_Core_Impossible){_LL12: _tmp2F=((
struct Cyc_Core_Impossible_struct*)_tmp2E)->f1;goto _LL7;}else{goto _LL8;}_LL8: if(
_tmp2E == Cyc_Dict_Absent){goto _LL9;}else{goto _LLA;}_LLA: if(*((void**)_tmp2E)== 
Cyc_Core_Invalid_argument){_LL13: _tmp30=((struct Cyc_Core_Invalid_argument_struct*)
_tmp2E)->f1;goto _LLB;}else{goto _LLC;}_LLC: if(*((void**)_tmp2E)== Cyc_Core_Failure){
_LL14: _tmp31=((struct Cyc_Core_Failure_struct*)_tmp2E)->f1;goto _LLD;}else{goto
_LLE;}_LLE: goto _LLF;_LL10: goto _LL11;_LL7: exn_string=_tag_arr("Exception Core::Impossible",
sizeof(unsigned char),27);explain_string=_tmp2F;goto _LL5;_LL9: exn_string=
_tag_arr("Exception Dict::Absent",sizeof(unsigned char),23);goto _LL5;_LLB:
exn_string=_tag_arr("Exception Core::Invalid_argument",sizeof(unsigned char),33);
explain_string=_tmp30;goto _LL5;_LLD: exn_string=_tag_arr("Exception Core::Failure",
sizeof(unsigned char),24);explain_string=_tmp31;goto _LL5;_LLF: ex=_tmp2E;
other_exn=1;exn_string=_tag_arr("Uncaught exception",sizeof(unsigned char),19);
goto _LL5;_LL11:(void)_throw(_tmp2E);_LL5:;}}}if(Cyc_Position_error_p()){Cyc_compile_failure=
1;}if(Cyc_Std_strcmp(exn_string,_tag_arr("",sizeof(unsigned char),1))!= 0){Cyc_compile_failure=
1;({struct Cyc_Std_String_pa_struct _tmp35;_tmp35.tag=0;_tmp35.f1=(struct
_tagged_arr)explain_string;{struct Cyc_Std_String_pa_struct _tmp34;_tmp34.tag=0;
_tmp34.f1=(struct _tagged_arr)stage_name;{struct Cyc_Std_String_pa_struct _tmp33;
_tmp33.tag=0;_tmp33.f1=(struct _tagged_arr)exn_string;{void*_tmp32[3]={& _tmp33,&
_tmp34,& _tmp35};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n%s thrown during %s: %s",
sizeof(unsigned char),25),_tag_arr(_tmp32,sizeof(void*),3));}}}});}if(Cyc_compile_failure){({
void*_tmp36[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\nCOMPILATION FAILED!\n",
sizeof(unsigned char),22),_tag_arr(_tmp36,sizeof(void*),0));});Cyc_Std_fflush((
struct Cyc_Std___sFILE*)Cyc_Std_stderr);if(other_exn){(int)_throw(ex);}return
_tmp29;}else{if(Cyc_v_r){({struct Cyc_Std_String_pa_struct _tmp38;_tmp38.tag=0;
_tmp38.f1=(struct _tagged_arr)stage_name;{void*_tmp37[1]={& _tmp38};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s completed.\n",sizeof(unsigned char),15),_tag_arr(
_tmp37,sizeof(void*),1));}});Cyc_Std_fflush((struct Cyc_Std___sFILE*)Cyc_Std_stderr);
return _tmp29;}}return _tmp29;}struct Cyc_List_List*Cyc_do_parse(struct Cyc_Std___sFILE*
f,struct Cyc_List_List*ignore){Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp3B=Cyc_Parse_parse_file(
f);Cyc_Lex_lex_init();return _tmp3B;}}struct Cyc_List_List*Cyc_do_typecheck(struct
Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tds){Cyc_Tc_tc(te,1,tds);if(! Cyc_noshake_r){
tds=Cyc_Tc_treeshake(te,tds);}return tds;}struct Cyc_List_List*Cyc_do_cfcheck(int
ignore,struct Cyc_List_List*tds){Cyc_NewControlFlow_cf_check(tds);return tds;}
struct _tuple9{struct Cyc_Tcenv_Tenv*f1;struct Cyc_Std___sFILE*f2;struct Cyc_Std___sFILE*
f3;};struct Cyc_List_List*Cyc_do_interface(struct _tuple9*params,struct Cyc_List_List*
tds){struct _tuple9 _tmp3D;struct Cyc_Std___sFILE*_tmp3E;struct Cyc_Std___sFILE*
_tmp3F;struct Cyc_Tcenv_Tenv*_tmp40;struct _tuple9*_tmp3C=params;_tmp3D=*_tmp3C;
_LL18: _tmp40=_tmp3D.f1;goto _LL17;_LL17: _tmp3F=_tmp3D.f2;goto _LL16;_LL16: _tmp3E=
_tmp3D.f3;goto _LL15;_LL15: {struct Cyc_Interface_I*_tmp41=Cyc_Interface_extract(
_tmp40->ae);if(_tmp3F == 0){Cyc_Interface_save(_tmp41,_tmp3E);}else{struct Cyc_Interface_I*
_tmp42=Cyc_Interface_parse((struct Cyc_Std___sFILE*)_check_null(_tmp3F));if(! Cyc_Interface_is_subinterface(
_tmp42,_tmp41,({struct _tuple8*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->f1=
_tag_arr("written interface",sizeof(unsigned char),18);_tmp43->f2=_tag_arr("maximal interface",
sizeof(unsigned char),18);_tmp43;}))){Cyc_compile_failure=1;}else{Cyc_Interface_save(
_tmp42,_tmp3E);}}return tds;}}struct Cyc_List_List*Cyc_do_translate(int ignore,
struct Cyc_List_List*tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*Cyc_do_tovc(
int ignore,struct Cyc_List_List*tds){return Cyc_Tovc_tovc(tds);}static unsigned char
_tmp44[1]="";static struct _tagged_arr Cyc_cyc_include={_tmp44,_tmp44,_tmp44 + 1};
struct Cyc_List_List*Cyc_do_print(struct Cyc_Std___sFILE*out_file,struct Cyc_List_List*
tds){struct Cyc_Absynpp_Params params_r;if(Cyc_tc_r){params_r=Cyc_Absynpp_cyc_params_r;}
else{params_r=Cyc_Absynpp_c_params_r;}params_r.expand_typedefs=! Cyc_noexpand_r;
params_r.to_VC=Cyc_tovc_r;params_r.add_cyc_prefix=Cyc_add_cyc_namespace_r;
params_r.generate_line_directives=Cyc_generate_line_directives_r;params_r.print_full_evars=
Cyc_print_full_evars_r;params_r.print_all_tvars=Cyc_print_all_tvars_r;params_r.print_all_kinds=
Cyc_print_all_kinds_r;{struct Cyc_Std___sFILE*_tmp45=Cyc_try_file_open(Cyc_cyc_include,
_tag_arr("r",sizeof(unsigned char),2),_tag_arr("internal compiler file",sizeof(
unsigned char),23));if(_tmp45 == 0){return tds;}{struct Cyc_Std___sFILE*_tmp46=(
struct Cyc_Std___sFILE*)_check_null(_tmp45);unsigned int n_read=1024;unsigned int
n_written;unsigned char buf[1024];{unsigned int _tmp49=1024;unsigned int i;for(i=0;
i < _tmp49;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=Cyc_Std_fread(
_tag_arr(buf,sizeof(unsigned char),1024),1,1024,_tmp46);if(n_read != 1024? ! Cyc_Std_feof(
_tmp46): 0){Cyc_compile_failure=1;({void*_tmp47[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\nError: problem copying cyc_include.h\n",sizeof(unsigned char),39),
_tag_arr(_tmp47,sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___sFILE*)Cyc_Std_stderr);
return tds;}n_written=Cyc_Std_fwrite(_tag_arr(buf,sizeof(unsigned char),1024),1,
n_read,out_file);if(n_read != n_written){Cyc_compile_failure=1;({void*_tmp48[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\nError: problem copying cyc_include.h\n",
sizeof(unsigned char),39),_tag_arr(_tmp48,sizeof(void*),0));});Cyc_Std_fflush((
struct Cyc_Std___sFILE*)Cyc_Std_stderr);return tds;}}Cyc_Std_fclose(_tmp46);if(Cyc_pp_r){
Cyc_Absynpp_set_params(& params_r);Cyc_Absynpp_decllist2file(tds,out_file);}else{
Cyc_Absyndump_set_params(& params_r);Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
Cyc_Std_fflush((struct Cyc_Std___sFILE*)out_file);return tds;}}}static struct Cyc_List_List*
Cyc_cfiles=0;static void Cyc_remove_cfiles(){if(! Cyc_save_c_r){for(0;Cyc_cfiles != 
0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){Cyc_remove_file(*((
struct _tagged_arr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}}
static struct Cyc_List_List*Cyc_split_by_char(struct _tagged_arr s,unsigned char c){
if(s.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr){return 0;}{struct Cyc_List_List*
_tmp4A=0;unsigned int _tmp4B=Cyc_Std_strlen(s);while(_tmp4B > 0){struct _tagged_arr
_tmp4C=Cyc_Std_strchr(s,c);if(_tmp4C.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr){
_tmp4A=({struct Cyc_List_List*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->hd=({
struct _tagged_arr*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=s;_tmp4E;});_tmp4D->tl=
_tmp4A;_tmp4D;});break;}else{_tmp4A=({struct Cyc_List_List*_tmp4F=_cycalloc(
sizeof(*_tmp4F));_tmp4F->hd=({struct _tagged_arr*_tmp50=_cycalloc(sizeof(*_tmp50));
_tmp50[0]=(struct _tagged_arr)Cyc_Std_substring(s,0,(unsigned int)((((struct
_tagged_arr)_tmp4C).curr - s.curr)/ sizeof(unsigned char)));_tmp50;});_tmp4F->tl=
_tmp4A;_tmp4F;});_tmp4B -=(((struct _tagged_arr)_tmp4C).curr - s.curr)/ sizeof(
unsigned char);s=_tagged_arr_plus(_tmp4C,sizeof(unsigned char),1);}}return((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4A);}}
static int Cyc_file_exists(struct _tagged_arr file){struct Cyc_Std___sFILE*f=0;{
struct _handler_cons _tmp51;_push_handler(& _tmp51);{int _tmp53=0;if(setjmp(_tmp51.handler)){
_tmp53=1;}if(! _tmp53){f=(struct Cyc_Std___sFILE*)Cyc_Std_file_open(file,_tag_arr("r",
sizeof(unsigned char),2));;_pop_handler();}else{void*_tmp52=(void*)_exn_thrown;
void*_tmp55=_tmp52;_LL1A: goto _LL1B;_LL1C: goto _LL1D;_LL1B: goto _LL19;_LL1D:(void)
_throw(_tmp55);_LL19:;}}}if(f == 0){return 0;}else{Cyc_Std_fclose((struct Cyc_Std___sFILE*)
_check_null(f));return 1;}}static struct _tagged_arr Cyc_sprint_list(struct Cyc_List_List*
dirs){struct _tagged_arr tmp=_tag_arr("",sizeof(unsigned char),1);for(0;dirs != 0;
dirs=dirs->tl){struct _tagged_arr _tmp56=*((struct _tagged_arr*)dirs->hd);if(_tmp56.curr
== ((struct _tagged_arr)_tag_arr(0,0,0)).curr? 1: Cyc_Std_strlen(_tmp56)== 0){
continue;}_tmp56=(struct _tagged_arr)Cyc_Std_strconcat(_tmp56,_tag_arr(":",
sizeof(unsigned char),2));tmp=(struct _tagged_arr)Cyc_Std_strconcat(_tmp56,tmp);}
return tmp;}static struct _tagged_arr*Cyc_find(struct Cyc_List_List*dirs,struct
_tagged_arr file){if(file.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr){
return 0;}for(0;dirs != 0;dirs=dirs->tl){struct _tagged_arr _tmp57=*((struct
_tagged_arr*)dirs->hd);if(_tmp57.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?
1: Cyc_Std_strlen(_tmp57)== 0){continue;}{struct _tagged_arr s=(struct _tagged_arr)
Cyc_Filename_concat(_tmp57,file);if(Cyc_file_exists(s)){return({struct
_tagged_arr*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58[0]=s;_tmp58;});}}}return 0;}
static struct _tagged_arr Cyc_do_find(struct Cyc_List_List*dirs,struct _tagged_arr
file){struct _tagged_arr*_tmp59=Cyc_find(dirs,file);if(_tmp59 == 0){({struct Cyc_Std_String_pa_struct
_tmp5C;_tmp5C.tag=0;_tmp5C.f1=(struct _tagged_arr)Cyc_sprint_list(dirs);{struct
Cyc_Std_String_pa_struct _tmp5B;_tmp5B.tag=0;_tmp5B.f1=(struct _tagged_arr)file;{
void*_tmp5A[2]={& _tmp5B,& _tmp5C};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: can't find internal compiler file %s in path %s\n",
sizeof(unsigned char),56),_tag_arr(_tmp5A,sizeof(void*),2));}}});Cyc_compile_failure=
1;Cyc_remove_cfiles();exit(1);}return*_tmp59;}static int Cyc_is_other_special(
unsigned char c){switch(c){case '\\': _LL1E: goto _LL1F;case '"': _LL1F: goto _LL20;case ';':
_LL20: goto _LL21;case '&': _LL21: goto _LL22;case '(': _LL22: goto _LL23;case ')': _LL23:
goto _LL24;case '|': _LL24: goto _LL25;case '^': _LL25: goto _LL26;case '<': _LL26: goto
_LL27;case '>': _LL27: goto _LL28;case ' ': _LL28: goto _LL29;case '\n': _LL29: goto _LL2A;
case '\t': _LL2A: return 1;default: _LL2B: return 0;}}static struct _tagged_arr Cyc_sh_escape_string(
struct _tagged_arr s){unsigned int _tmp5D=Cyc_Std_strlen(s);int _tmp5E=0;int _tmp5F=0;{
int i=0;for(0;i < _tmp5D;i ++){unsigned char _tmp60=((const unsigned char*)s.curr)[i];
if(_tmp60 == '\''){_tmp5E ++;}else{if(Cyc_is_other_special(_tmp60)){_tmp5F ++;}}}}
if(_tmp5E == 0? _tmp5F == 0: 0){return s;}if(_tmp5E == 0){return(struct _tagged_arr)Cyc_Std_strconcat_l(({
struct _tagged_arr*_tmp61[3];_tmp61[2]=_init_tag_arr(_cycalloc(sizeof(struct
_tagged_arr)),"'",sizeof(unsigned char),2);_tmp61[1]=({struct _tagged_arr*_tmp63=
_cycalloc(sizeof(*_tmp63));_tmp63[0]=(struct _tagged_arr)s;_tmp63;});_tmp61[0]=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",sizeof(unsigned char),2);((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp61,
sizeof(struct _tagged_arr*),3));}));}{unsigned int _tmp65=(_tmp5D + _tmp5E)+ _tmp5F;
struct _tagged_arr _tmp66=({unsigned int _tmp6A=_tmp65 + 1;unsigned char*_tmp6B=(
unsigned char*)_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmp6A));
struct _tagged_arr _tmp6D=_tag_arr(_tmp6B,sizeof(unsigned char),_tmp65 + 1);{
unsigned int _tmp6C=_tmp6A;unsigned int i;for(i=0;i < _tmp6C;i ++){_tmp6B[i]='\000';}};
_tmp6D;});int _tmp67=0;int _tmp68=0;for(0;_tmp67 < _tmp5D;_tmp67 ++){unsigned char
_tmp69=((const unsigned char*)s.curr)[_tmp67];if(_tmp69 == '\''? 1: Cyc_is_other_special(
_tmp69)){*((unsigned char*)_check_unknown_subscript(_tmp66,sizeof(unsigned char),
_tmp68 ++))='\\';}*((unsigned char*)_check_unknown_subscript(_tmp66,sizeof(
unsigned char),_tmp68 ++))=_tmp69;}return(struct _tagged_arr)_tmp66;}}static struct
_tagged_arr*Cyc_sh_escape_stringptr(struct _tagged_arr*sp){return({struct
_tagged_arr*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E[0]=Cyc_sh_escape_string(*sp);
_tmp6E;});}static void Cyc_process_file(struct _tagged_arr filename){struct
_tagged_arr _tmp6F=Cyc_make_base_filename(filename,Cyc_output_file);struct
_tagged_arr _tmp70=Cyc_Std_strconcat(_tmp6F,_tag_arr(".cyp",sizeof(unsigned char),
5));struct _tagged_arr _tmp71=Cyc_Std_strconcat(_tmp6F,_tag_arr(".cyci",sizeof(
unsigned char),6));struct _tagged_arr _tmp72=Cyc_Std_strconcat(_tmp6F,_tag_arr(".cycio",
sizeof(unsigned char),7));struct _tagged_arr _tmp73=Cyc_Std_strconcat(_tmp6F,
_tag_arr(".c",sizeof(unsigned char),3));if(Cyc_v_r){({struct Cyc_Std_String_pa_struct
_tmp75;_tmp75.tag=0;_tmp75.f1=(struct _tagged_arr)filename;{void*_tmp74[1]={&
_tmp75};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Compiling %s\n",sizeof(
unsigned char),14),_tag_arr(_tmp74,sizeof(void*),1));}});}{struct Cyc_Std___sFILE*
f0=Cyc_try_file_open(filename,_tag_arr("r",sizeof(unsigned char),2),_tag_arr("input file",
sizeof(unsigned char),11));if(Cyc_compile_failure){return;}Cyc_Std_fclose((
struct Cyc_Std___sFILE*)_check_null(f0));{struct _tagged_arr _tmp76=Cyc_Std_str_sepstr(({
struct Cyc_List_List*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->hd=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(unsigned char),1);_tmpA2->tl=((
struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_cppargs));_tmpA2;}),_tag_arr(" ",sizeof(unsigned char),2));
struct _tagged_arr stdinc_string;struct _tagged_arr def_inc_path=(struct _tagged_arr)
Cstring_to_string(Cdef_inc_path);if(Cyc_Std_strlen(def_inc_path)> 0){
stdinc_string=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp78;_tmp78.tag=
0;_tmp78.f1=(struct _tagged_arr)def_inc_path;{void*_tmp77[1]={& _tmp78};Cyc_Std_aprintf(
_tag_arr(" -I%s",sizeof(unsigned char),6),_tag_arr(_tmp77,sizeof(void*),1));}});}
else{stdinc_string=_tag_arr("",sizeof(unsigned char),1);}{struct _tagged_arr
_tmp79=Cyc_Std_getenv(_tag_arr("CYCLONE_INCLUDE_PATH",sizeof(unsigned char),21));
if(_tmp79.curr != (_tag_arr(0,0,0)).curr){struct Cyc_List_List*_tmp7A=Cyc_split_by_char((
struct _tagged_arr)_tmp79,':');struct _tagged_arr _tmp7B=Cyc_Std_str_sepstr(({
struct Cyc_List_List*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->hd=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(unsigned char),1);_tmp7F->tl=((
struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_sh_escape_stringptr,_tmp7A);_tmp7F;}),_tag_arr(" -I",
sizeof(unsigned char),4));stdinc_string=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp7E;_tmp7E.tag=0;_tmp7E.f1=(struct _tagged_arr)stdinc_string;{struct Cyc_Std_String_pa_struct
_tmp7D;_tmp7D.tag=0;_tmp7D.f1=(struct _tagged_arr)_tmp7B;{void*_tmp7C[2]={& _tmp7D,&
_tmp7E};Cyc_Std_aprintf(_tag_arr("%s%s",sizeof(unsigned char),5),_tag_arr(_tmp7C,
sizeof(void*),2));}}});}{struct _tagged_arr ofile_string;if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){ofile_string=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp82;_tmp82.tag=0;_tmp82.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_check_null(Cyc_output_file));{void*_tmp81[1]={& _tmp82};Cyc_Std_aprintf(_tag_arr(" > %s",
sizeof(unsigned char),6),_tag_arr(_tmp81,sizeof(void*),1));}});}else{
ofile_string=_tag_arr("",sizeof(unsigned char),1);}}else{ofile_string=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp84;_tmp84.tag=0;_tmp84.f1=(
struct _tagged_arr)Cyc_sh_escape_string((struct _tagged_arr)_tmp70);{void*_tmp83[1]={&
_tmp84};Cyc_Std_aprintf(_tag_arr(" > %s",sizeof(unsigned char),6),_tag_arr(
_tmp83,sizeof(void*),1));}});}{struct _tagged_arr fixup_string;if(Cyc_produce_dependencies){
if(Cyc_dependencies_target == 0){fixup_string=_tag_arr(" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
sizeof(unsigned char),35);}else{fixup_string=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp86;_tmp86.tag=0;_tmp86.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_check_null(Cyc_dependencies_target));{void*_tmp85[1]={& _tmp86};Cyc_Std_aprintf(
_tag_arr(" | sed 's/^.*\\.cyc\\.o:/%s:/'",sizeof(unsigned char),29),_tag_arr(
_tmp85,sizeof(void*),1));}});}}else{fixup_string=_tag_arr("",sizeof(
unsigned char),1);}{struct _tagged_arr _tmp87=({struct Cyc_Std_String_pa_struct
_tmpA1;_tmpA1.tag=0;_tmpA1.f1=(struct _tagged_arr)ofile_string;{struct Cyc_Std_String_pa_struct
_tmpA0;_tmpA0.tag=0;_tmpA0.f1=(struct _tagged_arr)fixup_string;{struct Cyc_Std_String_pa_struct
_tmp9F;_tmp9F.tag=0;_tmp9F.f1=(struct _tagged_arr)Cyc_sh_escape_string(filename);{
struct Cyc_Std_String_pa_struct _tmp9E;_tmp9E.tag=0;_tmp9E.f1=(struct _tagged_arr)
stdinc_string;{struct Cyc_Std_String_pa_struct _tmp9D;_tmp9D.tag=0;_tmp9D.f1=(
struct _tagged_arr)_tmp76;{struct Cyc_Std_String_pa_struct _tmp9C;_tmp9C.tag=0;
_tmp9C.f1=(struct _tagged_arr)Cyc_cpp;{void*_tmp9B[6]={& _tmp9C,& _tmp9D,& _tmp9E,&
_tmp9F,& _tmpA0,& _tmpA1};Cyc_Std_aprintf(_tag_arr("%s %s%s %s%s%s",sizeof(
unsigned char),15),_tag_arr(_tmp9B,sizeof(void*),6));}}}}}}});if(Cyc_v_r){({
struct Cyc_Std_String_pa_struct _tmp89;_tmp89.tag=0;_tmp89.f1=(struct _tagged_arr)
_tmp87;{void*_tmp88[1]={& _tmp89};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s\n",
sizeof(unsigned char),4),_tag_arr(_tmp88,sizeof(void*),1));}});}if(Cyc_Std_system((
struct _tagged_arr)_tmp87)!= 0){Cyc_compile_failure=1;({void*_tmp8A[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("\nError: preprocessing\n",sizeof(unsigned char),23),
_tag_arr(_tmp8A,sizeof(void*),0));});return;}if(Cyc_stop_after_cpp_r){return;}
Cyc_Position_reset_position((struct _tagged_arr)_tmp70);{struct Cyc_Std___sFILE*
in_file=Cyc_try_file_open((struct _tagged_arr)_tmp70,_tag_arr("r",sizeof(
unsigned char),2),_tag_arr("file",sizeof(unsigned char),5));if(Cyc_compile_failure){
return;}{struct Cyc_List_List*tds=0;{struct _handler_cons _tmp8B;_push_handler(&
_tmp8B);{int _tmp8D=0;if(setjmp(_tmp8B.handler)){_tmp8D=1;}if(! _tmp8D){tds=((
struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(
struct Cyc_Std___sFILE*,struct Cyc_List_List*),struct Cyc_Std___sFILE*env,struct Cyc_List_List*
tds))Cyc_do_stage)(_tag_arr("parsing",sizeof(unsigned char),8),Cyc_do_parse,(
struct Cyc_Std___sFILE*)_check_null(in_file),tds);;_pop_handler();}else{void*
_tmp8C=(void*)_exn_thrown;void*_tmp8F=_tmp8C;_LL2E: goto _LL2F;_LL30: goto _LL31;
_LL2F: Cyc_Std_file_close((struct Cyc_Std___sFILE*)_check_null(in_file));Cyc_remove_file((
struct _tagged_arr)_tmp70);(int)_throw(_tmp8F);_LL31:(void)_throw(_tmp8F);_LL2D:;}}}
Cyc_Std_file_close((struct Cyc_Std___sFILE*)_check_null(in_file));if(Cyc_compile_failure){
Cyc_remove_file((struct _tagged_arr)_tmp70);return;}{struct Cyc_Tcenv_Tenv*_tmp90=
Cyc_Tcenv_tc_init();if(Cyc_parseonly_r){goto PRINTC;}tds=((struct Cyc_List_List*(*)(
struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*tds))Cyc_do_stage)(
_tag_arr("type checking",sizeof(unsigned char),14),Cyc_do_typecheck,_tmp90,tds);
if(Cyc_compile_failure){Cyc_remove_file((struct _tagged_arr)_tmp70);return;}tds=((
struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(int,
struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(_tag_arr("control-flow checking",
sizeof(unsigned char),22),Cyc_do_cfcheck,1,tds);if(Cyc_compile_failure){Cyc_remove_file((
struct _tagged_arr)_tmp70);return;}if(Cyc_ic_r){struct Cyc_Std___sFILE*inter_file=
Cyc_Std_fopen((struct _tagged_arr)_tmp71,_tag_arr("r",sizeof(unsigned char),2));
struct Cyc_Std___sFILE*inter_objfile=Cyc_try_file_open((struct _tagged_arr)_tmp72,
_tag_arr("w",sizeof(unsigned char),2),_tag_arr("interface object file",sizeof(
unsigned char),22));if(inter_objfile == 0){Cyc_compile_failure=1;return;}Cyc_Position_reset_position((
struct _tagged_arr)_tmp71);tds=((struct Cyc_List_List*(*)(struct _tagged_arr
stage_name,struct Cyc_List_List*(*f)(struct _tuple9*,struct Cyc_List_List*),struct
_tuple9*env,struct Cyc_List_List*tds))Cyc_do_stage)(_tag_arr("interface checking",
sizeof(unsigned char),19),Cyc_do_interface,({struct _tuple9*_tmp91=_cycalloc(
sizeof(*_tmp91));_tmp91->f1=_tmp90;_tmp91->f2=inter_file;_tmp91->f3=(struct Cyc_Std___sFILE*)
_check_null(inter_objfile);_tmp91;}),tds);if(inter_file != 0){Cyc_Std_file_close((
struct Cyc_Std___sFILE*)_check_null(inter_file));}Cyc_Std_file_close((struct Cyc_Std___sFILE*)
_check_null(inter_objfile));}if(Cyc_tc_r){goto PRINTC;}tds=((struct Cyc_List_List*(*)(
struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),
int env,struct Cyc_List_List*tds))Cyc_do_stage)(_tag_arr("translation to C",
sizeof(unsigned char),17),Cyc_do_translate,1,tds);if(Cyc_compile_failure){Cyc_remove_file((
struct _tagged_arr)_tmp70);return;}if(! Cyc_compile_failure){Cyc_Tcutil_flush_warnings();}
Cyc_remove_file((struct _tagged_arr)_tmp70);if(Cyc_compile_failure){return;}if(
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
struct _tagged_arr)_tmp73,_tag_arr("w",sizeof(unsigned char),2),_tag_arr("output file",
sizeof(unsigned char),12));}}if(Cyc_compile_failure){return;}{struct
_handler_cons _tmp92;_push_handler(& _tmp92);{int _tmp94=0;if(setjmp(_tmp92.handler)){
_tmp94=1;}if(! _tmp94){tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,
struct Cyc_List_List*(*f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*),struct Cyc_Std___sFILE*
env,struct Cyc_List_List*tds))Cyc_do_stage)(_tag_arr("printing",sizeof(
unsigned char),9),Cyc_do_print,(struct Cyc_Std___sFILE*)_check_null(out_file),tds);;
_pop_handler();}else{void*_tmp93=(void*)_exn_thrown;void*_tmp96=_tmp93;_LL33:
goto _LL34;_LL35: goto _LL36;_LL34: Cyc_compile_failure=1;Cyc_Std_file_close((struct
Cyc_Std___sFILE*)_check_null(out_file));Cyc_cfiles=({struct Cyc_List_List*_tmp97=
_cycalloc(sizeof(*_tmp97));_tmp97->hd=({struct _tagged_arr*_tmp98=_cycalloc(
sizeof(*_tmp98));_tmp98[0]=(struct _tagged_arr)_tmp73;_tmp98;});_tmp97->tl=Cyc_cfiles;
_tmp97;});(int)_throw(_tmp96);_LL36:(void)_throw(_tmp96);_LL32:;}}}Cyc_Std_file_close((
struct Cyc_Std___sFILE*)_check_null(out_file));Cyc_cfiles=({struct Cyc_List_List*
_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->hd=({struct _tagged_arr*_tmp9A=
_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=(struct _tagged_arr)_tmp73;_tmp9A;});_tmp99->tl=
Cyc_cfiles;_tmp99;});}}}}}}}}}}}static unsigned char _tmpA4[8]="<final>";static
struct _tagged_arr Cyc_final_str={_tmpA4,_tmpA4,_tmpA4 + 8};static struct _tagged_arr*
Cyc_final_strptr=& Cyc_final_str;static struct Cyc_Interface_I*Cyc_read_cycio(
struct _tagged_arr*n){if(n == (struct _tagged_arr*)Cyc_final_strptr){return Cyc_Interface_final();}{
struct _tagged_arr basename;{struct _handler_cons _tmpA5;_push_handler(& _tmpA5);{int
_tmpA7=0;if(setjmp(_tmpA5.handler)){_tmpA7=1;}if(! _tmpA7){basename=(struct
_tagged_arr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmpA6=(
void*)_exn_thrown;void*_tmpA9=_tmpA6;_LL38: if(*((void**)_tmpA9)== Cyc_Core_Invalid_argument){
goto _LL39;}else{goto _LL3A;}_LL3A: goto _LL3B;_LL39: basename=*n;goto _LL37;_LL3B:(
void)_throw(_tmpA9);_LL37:;}}}{struct _tagged_arr _tmpAA=Cyc_Std_strconcat(
basename,_tag_arr(".cycio",sizeof(unsigned char),7));struct Cyc_Std___sFILE*
_tmpAB=Cyc_try_file_open((struct _tagged_arr)_tmpAA,_tag_arr("r",sizeof(
unsigned char),2),_tag_arr("interface object file",sizeof(unsigned char),22));
if(_tmpAB == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();exit(1);}Cyc_Position_reset_position((
struct _tagged_arr)_tmpAA);{struct Cyc_Interface_I*_tmpAC=Cyc_Interface_load((
struct Cyc_Std___sFILE*)_check_null(_tmpAB));Cyc_Std_file_close((struct Cyc_Std___sFILE*)
_check_null(_tmpAB));return _tmpAC;}}}}static int Cyc_is_cfile(struct _tagged_arr*n){
return*((const unsigned char*)_check_unknown_subscript(*n,sizeof(unsigned char),0))
!= '-';}extern void GC_blacklist_warn_clear();struct _tuple10{struct _tagged_arr f1;
int f2;struct _tagged_arr f3;void*f4;struct _tagged_arr f5;};int Cyc_main(int argc,
struct _tagged_arr argv){GC_blacklist_warn_clear();{struct _tagged_arr comp=(struct
_tagged_arr)Cstring_to_string(Ccomp);Cyc_set_cpp((struct _tagged_arr)Cyc_Std_strconcat(
comp,_tag_arr(" -x c -E -U__GNUC__ -nostdinc",sizeof(unsigned char),30)));{
struct Cyc_List_List*options=({struct _tuple10*_tmpE8[48];_tmpE8[47]=({struct
_tuple10*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->f1=_tag_arr("-warnboundschecks",
sizeof(unsigned char),18);_tmp176->f2=0;_tmp176->f3=_tag_arr("",sizeof(
unsigned char),1);_tmp176->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp177=
_cycalloc(sizeof(*_tmp177));_tmp177[0]=({struct Cyc_Arg_Set_spec_struct _tmp178;
_tmp178.tag=3;_tmp178.f1=& Cyc_Toc_warn_bounds_checks;_tmp178;});_tmp177;});
_tmp176->f5=_tag_arr("Warn when bounds checks can't be eliminated",sizeof(
unsigned char),44);_tmp176;});_tmpE8[46]=({struct _tuple10*_tmp173=_cycalloc(
sizeof(*_tmp173));_tmp173->f1=_tag_arr("-printfullevars",sizeof(unsigned char),
16);_tmp173->f2=0;_tmp173->f3=_tag_arr("",sizeof(unsigned char),1);_tmp173->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp174=_cycalloc(sizeof(*_tmp174));
_tmp174[0]=({struct Cyc_Arg_Set_spec_struct _tmp175;_tmp175.tag=3;_tmp175.f1=& Cyc_print_full_evars_r;
_tmp175;});_tmp174;});_tmp173->f5=_tag_arr("Print full information for evars (type debugging)",
sizeof(unsigned char),50);_tmp173;});_tmpE8[45]=({struct _tuple10*_tmp170=
_cycalloc(sizeof(*_tmp170));_tmp170->f1=_tag_arr("-printallkinds",sizeof(
unsigned char),15);_tmp170->f2=0;_tmp170->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp170->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp171=_cycalloc(sizeof(*
_tmp171));_tmp171[0]=({struct Cyc_Arg_Set_spec_struct _tmp172;_tmp172.tag=3;
_tmp172.f1=& Cyc_print_all_kinds_r;_tmp172;});_tmp171;});_tmp170->f5=_tag_arr("Always print kinds of type variables",
sizeof(unsigned char),37);_tmp170;});_tmpE8[44]=({struct _tuple10*_tmp16D=
_cycalloc(sizeof(*_tmp16D));_tmp16D->f1=_tag_arr("-printalltvars",sizeof(
unsigned char),15);_tmp16D->f2=0;_tmp16D->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp16D->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp16E=_cycalloc(sizeof(*
_tmp16E));_tmp16E[0]=({struct Cyc_Arg_Set_spec_struct _tmp16F;_tmp16F.tag=3;
_tmp16F.f1=& Cyc_print_all_tvars_r;_tmp16F;});_tmp16E;});_tmp16D->f5=_tag_arr("Print all type variables (even implicit default effects)",
sizeof(unsigned char),57);_tmp16D;});_tmpE8[43]=({struct _tuple10*_tmp16A=
_cycalloc(sizeof(*_tmp16A));_tmp16A->f1=_tag_arr("-noexpandtypedefs",sizeof(
unsigned char),18);_tmp16A->f2=0;_tmp16A->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp16A->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp16B=_cycalloc(sizeof(*
_tmp16B));_tmp16B[0]=({struct Cyc_Arg_Set_spec_struct _tmp16C;_tmp16C.tag=3;
_tmp16C.f1=& Cyc_noexpand_r;_tmp16C;});_tmp16B;});_tmp16A->f5=_tag_arr("Don't expand typedefs in pretty printing",
sizeof(unsigned char),41);_tmp16A;});_tmpE8[42]=({struct _tuple10*_tmp167=
_cycalloc(sizeof(*_tmp167));_tmp167->f1=_tag_arr("-noremoveunused",sizeof(
unsigned char),16);_tmp167->f2=0;_tmp167->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp167->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp168=_cycalloc(sizeof(*
_tmp168));_tmp168[0]=({struct Cyc_Arg_Set_spec_struct _tmp169;_tmp169.tag=3;
_tmp169.f1=& Cyc_noshake_r;_tmp169;});_tmp168;});_tmp167->f5=_tag_arr("Don't remove externed variables that aren't used",
sizeof(unsigned char),49);_tmp167;});_tmpE8[41]=({struct _tuple10*_tmp164=
_cycalloc(sizeof(*_tmp164));_tmp164->f1=_tag_arr("-nogc",sizeof(unsigned char),6);
_tmp164->f2=0;_tmp164->f3=_tag_arr("",sizeof(unsigned char),1);_tmp164->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165[0]=({
struct Cyc_Arg_Set_spec_struct _tmp166;_tmp166.tag=3;_tmp166.f1=& Cyc_nogc_r;
_tmp166;});_tmp165;});_tmp164->f5=_tag_arr("Don't link in the garbage collector",
sizeof(unsigned char),36);_tmp164;});_tmpE8[40]=({struct _tuple10*_tmp161=
_cycalloc(sizeof(*_tmp161));_tmp161->f1=_tag_arr("-nocyc",sizeof(unsigned char),
7);_tmp161->f2=0;_tmp161->f3=_tag_arr("",sizeof(unsigned char),1);_tmp161->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp162=_cycalloc(sizeof(*_tmp162));
_tmp162[0]=({struct Cyc_Arg_Unit_spec_struct _tmp163;_tmp163.tag=0;_tmp163.f1=Cyc_set_nocyc;
_tmp163;});_tmp162;});_tmp161->f5=_tag_arr("Don't add implicit namespace Cyc",
sizeof(unsigned char),33);_tmp161;});_tmpE8[39]=({struct _tuple10*_tmp15E=
_cycalloc(sizeof(*_tmp15E));_tmp15E->f1=_tag_arr("-no-cpp-precomp",sizeof(
unsigned char),16);_tmp15E->f2=0;_tmp15E->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp15E->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp15F=_cycalloc(sizeof(*
_tmp15F));_tmp15F[0]=({struct Cyc_Arg_Unit_spec_struct _tmp160;_tmp160.tag=0;
_tmp160.f1=Cyc_set_nocppprecomp;_tmp160;});_tmp15F;});_tmp15E->f5=_tag_arr("Don't do smart preprocessing (mac only)",
sizeof(unsigned char),40);_tmp15E;});_tmpE8[38]=({struct _tuple10*_tmp15B=
_cycalloc(sizeof(*_tmp15B));_tmp15B->f1=_tag_arr("-use-cpp",sizeof(unsigned char),
9);_tmp15B->f2=0;_tmp15B->f3=_tag_arr("<path>",sizeof(unsigned char),7);_tmp15B->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp15C=_cycalloc(sizeof(*_tmp15C));
_tmp15C[0]=({struct Cyc_Arg_String_spec_struct _tmp15D;_tmp15D.tag=5;_tmp15D.f1=
Cyc_set_cpp;_tmp15D;});_tmp15C;});_tmp15B->f5=_tag_arr("Indicate which preprocessor to use",
sizeof(unsigned char),35);_tmp15B;});_tmpE8[37]=({struct _tuple10*_tmp158=
_cycalloc(sizeof(*_tmp158));_tmp158->f1=_tag_arr("--noboundschecks",sizeof(
unsigned char),17);_tmp158->f2=0;_tmp158->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp158->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp159=_cycalloc(sizeof(*
_tmp159));_tmp159[0]=({struct Cyc_Arg_Unit_spec_struct _tmp15A;_tmp15A.tag=0;
_tmp15A.f1=Cyc_set_noboundschecks;_tmp15A;});_tmp159;});_tmp158->f5=_tag_arr("Disable bounds checks",
sizeof(unsigned char),22);_tmp158;});_tmpE8[36]=({struct _tuple10*_tmp155=
_cycalloc(sizeof(*_tmp155));_tmp155->f1=_tag_arr("--nonullchecks",sizeof(
unsigned char),15);_tmp155->f2=0;_tmp155->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp155->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp156=_cycalloc(sizeof(*
_tmp156));_tmp156[0]=({struct Cyc_Arg_Unit_spec_struct _tmp157;_tmp157.tag=0;
_tmp157.f1=Cyc_set_nonullchecks;_tmp157;});_tmp156;});_tmp155->f5=_tag_arr("Disable null checks",
sizeof(unsigned char),20);_tmp155;});_tmpE8[35]=({struct _tuple10*_tmp152=
_cycalloc(sizeof(*_tmp152));_tmp152->f1=_tag_arr("--nochecks",sizeof(
unsigned char),11);_tmp152->f2=0;_tmp152->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp152->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp153=_cycalloc(sizeof(*
_tmp153));_tmp153[0]=({struct Cyc_Arg_Unit_spec_struct _tmp154;_tmp154.tag=0;
_tmp154.f1=Cyc_set_nochecks;_tmp154;});_tmp153;});_tmp152->f5=_tag_arr("Disable bounds/null checks",
sizeof(unsigned char),27);_tmp152;});_tmpE8[34]=({struct _tuple10*_tmp14F=
_cycalloc(sizeof(*_tmp14F));_tmp14F->f1=_tag_arr("--lineno",sizeof(unsigned char),
9);_tmp14F->f2=0;_tmp14F->f3=_tag_arr("",sizeof(unsigned char),1);_tmp14F->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp150=_cycalloc(sizeof(*_tmp150));
_tmp150[0]=({struct Cyc_Arg_Unit_spec_struct _tmp151;_tmp151.tag=0;_tmp151.f1=Cyc_set_lineno;
_tmp151;});_tmp150;});_tmp14F->f5=_tag_arr("Generate line numbers for debugging",
sizeof(unsigned char),36);_tmp14F;});_tmpE8[33]=({struct _tuple10*_tmp14C=
_cycalloc(sizeof(*_tmp14C));_tmp14C->f1=_tag_arr("-save-c",sizeof(unsigned char),
8);_tmp14C->f2=0;_tmp14C->f3=_tag_arr("",sizeof(unsigned char),1);_tmp14C->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp14D=_cycalloc(sizeof(*_tmp14D));
_tmp14D[0]=({struct Cyc_Arg_Set_spec_struct _tmp14E;_tmp14E.tag=3;_tmp14E.f1=& Cyc_save_c_r;
_tmp14E;});_tmp14D;});_tmp14C->f5=_tag_arr("Don't delete temporary C files",
sizeof(unsigned char),31);_tmp14C;});_tmpE8[32]=({struct _tuple10*_tmp149=
_cycalloc(sizeof(*_tmp149));_tmp149->f1=_tag_arr("-save-temps",sizeof(
unsigned char),12);_tmp149->f2=0;_tmp149->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp149->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp14A=_cycalloc(sizeof(*
_tmp14A));_tmp14A[0]=({struct Cyc_Arg_Unit_spec_struct _tmp14B;_tmp14B.tag=0;
_tmp14B.f1=Cyc_set_save_temps;_tmp14B;});_tmp14A;});_tmp149->f5=_tag_arr("Don't delete temporary files",
sizeof(unsigned char),29);_tmp149;});_tmpE8[31]=({struct _tuple10*_tmp146=
_cycalloc(sizeof(*_tmp146));_tmp146->f1=_tag_arr("-tovc",sizeof(unsigned char),6);
_tmp146->f2=0;_tmp146->f3=_tag_arr("",sizeof(unsigned char),1);_tmp146->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147[0]=({
struct Cyc_Arg_Set_spec_struct _tmp148;_tmp148.tag=3;_tmp148.f1=& Cyc_tovc_r;
_tmp148;});_tmp147;});_tmp146->f5=_tag_arr("Avoid gcc extensions in C output",
sizeof(unsigned char),33);_tmp146;});_tmpE8[30]=({struct _tuple10*_tmp143=
_cycalloc(sizeof(*_tmp143));_tmp143->f1=_tag_arr("-up",sizeof(unsigned char),4);
_tmp143->f2=0;_tmp143->f3=_tag_arr("",sizeof(unsigned char),1);_tmp143->f4=(void*)({
struct Cyc_Arg_Clear_spec_struct*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144[0]=({
struct Cyc_Arg_Clear_spec_struct _tmp145;_tmp145.tag=4;_tmp145.f1=& Cyc_pp_r;
_tmp145;});_tmp144;});_tmp143->f5=_tag_arr("Ugly print",sizeof(unsigned char),11);
_tmp143;});_tmpE8[29]=({struct _tuple10*_tmp140=_cycalloc(sizeof(*_tmp140));
_tmp140->f1=_tag_arr("-pp",sizeof(unsigned char),4);_tmp140->f2=0;_tmp140->f3=
_tag_arr("",sizeof(unsigned char),1);_tmp140->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141[0]=({struct Cyc_Arg_Set_spec_struct
_tmp142;_tmp142.tag=3;_tmp142.f1=& Cyc_pp_r;_tmp142;});_tmp141;});_tmp140->f5=
_tag_arr("Pretty print",sizeof(unsigned char),13);_tmp140;});_tmpE8[28]=({struct
_tuple10*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->f1=_tag_arr("-ic",sizeof(
unsigned char),4);_tmp13D->f2=0;_tmp13D->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp13D->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp13E=_cycalloc(sizeof(*
_tmp13E));_tmp13E[0]=({struct Cyc_Arg_Set_spec_struct _tmp13F;_tmp13F.tag=3;
_tmp13F.f1=& Cyc_ic_r;_tmp13F;});_tmp13E;});_tmp13D->f5=_tag_arr("Activate the link-checker",
sizeof(unsigned char),26);_tmp13D;});_tmpE8[27]=({struct _tuple10*_tmp13A=
_cycalloc(sizeof(*_tmp13A));_tmp13A->f1=_tag_arr("-stopafter-toc",sizeof(
unsigned char),15);_tmp13A->f2=0;_tmp13A->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp13A->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp13B=_cycalloc(sizeof(*
_tmp13B));_tmp13B[0]=({struct Cyc_Arg_Set_spec_struct _tmp13C;_tmp13C.tag=3;
_tmp13C.f1=& Cyc_toc_r;_tmp13C;});_tmp13B;});_tmp13A->f5=_tag_arr("Stop after translation to C",
sizeof(unsigned char),28);_tmp13A;});_tmpE8[26]=({struct _tuple10*_tmp137=
_cycalloc(sizeof(*_tmp137));_tmp137->f1=_tag_arr("-stopafter-tc",sizeof(
unsigned char),14);_tmp137->f2=0;_tmp137->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp137->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp138=_cycalloc(sizeof(*
_tmp138));_tmp138[0]=({struct Cyc_Arg_Set_spec_struct _tmp139;_tmp139.tag=3;
_tmp139.f1=& Cyc_tc_r;_tmp139;});_tmp138;});_tmp137->f5=_tag_arr("Stop after type checking",
sizeof(unsigned char),25);_tmp137;});_tmpE8[25]=({struct _tuple10*_tmp134=
_cycalloc(sizeof(*_tmp134));_tmp134->f1=_tag_arr("-stopafter-parse",sizeof(
unsigned char),17);_tmp134->f2=0;_tmp134->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp134->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp135=_cycalloc(sizeof(*
_tmp135));_tmp135[0]=({struct Cyc_Arg_Set_spec_struct _tmp136;_tmp136.tag=3;
_tmp136.f1=& Cyc_parseonly_r;_tmp136;});_tmp135;});_tmp134->f5=_tag_arr("Stop after parsing",
sizeof(unsigned char),19);_tmp134;});_tmpE8[24]=({struct _tuple10*_tmp131=
_cycalloc(sizeof(*_tmp131));_tmp131->f1=_tag_arr("-E",sizeof(unsigned char),3);
_tmp131->f2=0;_tmp131->f3=_tag_arr("",sizeof(unsigned char),1);_tmp131->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132[0]=({
struct Cyc_Arg_Set_spec_struct _tmp133;_tmp133.tag=3;_tmp133.f1=& Cyc_stop_after_cpp_r;
_tmp133;});_tmp132;});_tmp131->f5=_tag_arr("Stop after preprocessing",sizeof(
unsigned char),25);_tmp131;});_tmpE8[23]=({struct _tuple10*_tmp12E=_cycalloc(
sizeof(*_tmp12E));_tmp12E->f1=_tag_arr("-MT",sizeof(unsigned char),4);_tmp12E->f2=
0;_tmp12E->f3=_tag_arr(" <target>",sizeof(unsigned char),10);_tmp12E->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F[0]=({
struct Cyc_Arg_String_spec_struct _tmp130;_tmp130.tag=5;_tmp130.f1=Cyc_set_dependencies_target;
_tmp130;});_tmp12F;});_tmp12E->f5=_tag_arr("Give target for dependencies",
sizeof(unsigned char),29);_tmp12E;});_tmpE8[22]=({struct _tuple10*_tmp12B=
_cycalloc(sizeof(*_tmp12B));_tmp12B->f1=_tag_arr("-MG",sizeof(unsigned char),4);
_tmp12B->f2=0;_tmp12B->f3=_tag_arr("",sizeof(unsigned char),1);_tmp12B->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp12D;_tmp12D.tag=1;_tmp12D.f1=Cyc_add_cpparg;
_tmp12D;});_tmp12C;});_tmp12B->f5=_tag_arr("When producing dependencies assume that missing files are generated",
sizeof(unsigned char),68);_tmp12B;});_tmpE8[21]=({struct _tuple10*_tmp128=
_cycalloc(sizeof(*_tmp128));_tmp128->f1=_tag_arr("-M",sizeof(unsigned char),3);
_tmp128->f2=0;_tmp128->f3=_tag_arr("",sizeof(unsigned char),1);_tmp128->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp12A;_tmp12A.tag=0;_tmp12A.f1=Cyc_set_produce_dependencies;
_tmp12A;});_tmp129;});_tmp128->f5=_tag_arr("Produce dependencies",sizeof(
unsigned char),21);_tmp128;});_tmpE8[20]=({struct _tuple10*_tmp125=_cycalloc(
sizeof(*_tmp125));_tmp125->f1=_tag_arr("-S",sizeof(unsigned char),3);_tmp125->f2=
0;_tmp125->f3=_tag_arr("",sizeof(unsigned char),1);_tmp125->f4=(void*)({struct
Cyc_Arg_Unit_spec_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126[0]=({struct
Cyc_Arg_Unit_spec_struct _tmp127;_tmp127.tag=0;_tmp127.f1=Cyc_set_stop_after_asmfile;
_tmp127;});_tmp126;});_tmp125->f5=_tag_arr("Stop after producing assembly code",
sizeof(unsigned char),35);_tmp125;});_tmpE8[19]=({struct _tuple10*_tmp122=
_cycalloc(sizeof(*_tmp122));_tmp122->f1=_tag_arr("-pa",sizeof(unsigned char),4);
_tmp122->f2=0;_tmp122->f3=_tag_arr("",sizeof(unsigned char),1);_tmp122->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp124;_tmp124.tag=0;_tmp124.f1=Cyc_set_pa;
_tmp124;});_tmp123;});_tmp122->f5=_tag_arr("Compile for profiling with aprof",
sizeof(unsigned char),33);_tmp122;});_tmpE8[18]=({struct _tuple10*_tmp11F=
_cycalloc(sizeof(*_tmp11F));_tmp11F->f1=_tag_arr("-pg",sizeof(unsigned char),4);
_tmp11F->f2=0;_tmp11F->f3=_tag_arr("",sizeof(unsigned char),1);_tmp11F->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp121;_tmp121.tag=1;_tmp121.f1=Cyc_add_ccarg;
_tmp121;});_tmp120;});_tmp11F->f5=_tag_arr("Compile for profiling with gprof",
sizeof(unsigned char),33);_tmp11F;});_tmpE8[17]=({struct _tuple10*_tmp11C=
_cycalloc(sizeof(*_tmp11C));_tmp11C->f1=_tag_arr("-p",sizeof(unsigned char),3);
_tmp11C->f2=0;_tmp11C->f3=_tag_arr("",sizeof(unsigned char),1);_tmp11C->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp11E;_tmp11E.tag=1;_tmp11E.f1=Cyc_add_ccarg;
_tmp11E;});_tmp11D;});_tmp11C->f5=_tag_arr("Compile for profiling with prof",
sizeof(unsigned char),32);_tmp11C;});_tmpE8[16]=({struct _tuple10*_tmp119=
_cycalloc(sizeof(*_tmp119));_tmp119->f1=_tag_arr("-g",sizeof(unsigned char),3);
_tmp119->f2=0;_tmp119->f3=_tag_arr("",sizeof(unsigned char),1);_tmp119->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp11B;_tmp11B.tag=1;_tmp11B.f1=Cyc_add_ccarg;
_tmp11B;});_tmp11A;});_tmp119->f5=_tag_arr("Compile for debugging",sizeof(
unsigned char),22);_tmp119;});_tmpE8[15]=({struct _tuple10*_tmp116=_cycalloc(
sizeof(*_tmp116));_tmp116->f1=_tag_arr("-fomit-frame-pointer",sizeof(
unsigned char),21);_tmp116->f2=0;_tmp116->f3=_tag_arr("",sizeof(unsigned char),1);
_tmp116->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp117=_cycalloc(sizeof(*
_tmp117));_tmp117[0]=({struct Cyc_Arg_Flag_spec_struct _tmp118;_tmp118.tag=1;
_tmp118.f1=Cyc_add_ccarg;_tmp118;});_tmp117;});_tmp116->f5=_tag_arr("Omit frame pointer",
sizeof(unsigned char),19);_tmp116;});_tmpE8[14]=({struct _tuple10*_tmp113=
_cycalloc(sizeof(*_tmp113));_tmp113->f1=_tag_arr("-O3",sizeof(unsigned char),4);
_tmp113->f2=0;_tmp113->f3=_tag_arr("",sizeof(unsigned char),1);_tmp113->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp115;_tmp115.tag=1;_tmp115.f1=Cyc_add_ccarg;
_tmp115;});_tmp114;});_tmp113->f5=_tag_arr("Even more optimization",sizeof(
unsigned char),23);_tmp113;});_tmpE8[13]=({struct _tuple10*_tmp110=_cycalloc(
sizeof(*_tmp110));_tmp110->f1=_tag_arr("-O2",sizeof(unsigned char),4);_tmp110->f2=
0;_tmp110->f3=_tag_arr("",sizeof(unsigned char),1);_tmp110->f4=(void*)({struct
Cyc_Arg_Flag_spec_struct*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111[0]=({struct
Cyc_Arg_Flag_spec_struct _tmp112;_tmp112.tag=1;_tmp112.f1=Cyc_add_ccarg;_tmp112;});
_tmp111;});_tmp110->f5=_tag_arr("A higher level of optimization",sizeof(
unsigned char),31);_tmp110;});_tmpE8[12]=({struct _tuple10*_tmp10D=_cycalloc(
sizeof(*_tmp10D));_tmp10D->f1=_tag_arr("-O",sizeof(unsigned char),3);_tmp10D->f2=
0;_tmp10D->f3=_tag_arr("",sizeof(unsigned char),1);_tmp10D->f4=(void*)({struct
Cyc_Arg_Flag_spec_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=({struct
Cyc_Arg_Flag_spec_struct _tmp10F;_tmp10F.tag=1;_tmp10F.f1=Cyc_add_ccarg;_tmp10F;});
_tmp10E;});_tmp10D->f5=_tag_arr("Optimize",sizeof(unsigned char),9);_tmp10D;});
_tmpE8[11]=({struct _tuple10*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->f1=
_tag_arr("-O0",sizeof(unsigned char),4);_tmp10A->f2=0;_tmp10A->f3=_tag_arr("",
sizeof(unsigned char),1);_tmp10A->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp10C;_tmp10C.tag=1;_tmp10C.f1=Cyc_add_ccarg;_tmp10C;});_tmp10B;});_tmp10A->f5=
_tag_arr("Don't optimize",sizeof(unsigned char),15);_tmp10A;});_tmpE8[10]=({
struct _tuple10*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->f1=_tag_arr("-s",
sizeof(unsigned char),3);_tmp107->f2=0;_tmp107->f3=_tag_arr("",sizeof(
unsigned char),1);_tmp107->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp108=
_cycalloc(sizeof(*_tmp108));_tmp108[0]=({struct Cyc_Arg_Flag_spec_struct _tmp109;
_tmp109.tag=1;_tmp109.f1=Cyc_add_ccarg;_tmp109;});_tmp108;});_tmp107->f5=
_tag_arr("Remove all symbol table and relocation info from executable",sizeof(
unsigned char),60);_tmp107;});_tmpE8[9]=({struct _tuple10*_tmp104=_cycalloc(
sizeof(*_tmp104));_tmp104->f1=_tag_arr("-x",sizeof(unsigned char),3);_tmp104->f2=
0;_tmp104->f3=_tag_arr(" <language>",sizeof(unsigned char),12);_tmp104->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105[0]=({
struct Cyc_Arg_String_spec_struct _tmp106;_tmp106.tag=5;_tmp106.f1=Cyc_set_inputtype;
_tmp106;});_tmp105;});_tmp104->f5=_tag_arr("Specify <language> for the following input files",
sizeof(unsigned char),49);_tmp104;});_tmpE8[8]=({struct _tuple10*_tmp101=
_cycalloc(sizeof(*_tmp101));_tmp101->f1=_tag_arr("-c",sizeof(unsigned char),3);
_tmp101->f2=0;_tmp101->f3=_tag_arr("",sizeof(unsigned char),1);_tmp101->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp103;_tmp103.tag=0;_tmp103.f1=Cyc_set_stop_after_objectfile;
_tmp103;});_tmp102;});_tmp101->f5=_tag_arr("Produce an object file instead of an executable; do not link",
sizeof(unsigned char),61);_tmp101;});_tmpE8[7]=({struct _tuple10*_tmpFE=_cycalloc(
sizeof(*_tmpFE));_tmpFE->f1=_tag_arr("-l",sizeof(unsigned char),3);_tmpFE->f2=1;
_tmpFE->f3=_tag_arr("<file>",sizeof(unsigned char),7);_tmpFE->f4=(void*)({struct
Cyc_Arg_Flag_spec_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp100;_tmp100.tag=1;_tmp100.f1=Cyc_add_libarg;_tmp100;});_tmpFF;});_tmpFE->f5=
_tag_arr("Library file",sizeof(unsigned char),13);_tmpFE;});_tmpE8[6]=({struct
_tuple10*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->f1=_tag_arr("-L",sizeof(
unsigned char),3);_tmpFB->f2=1;_tmpFB->f3=_tag_arr("<dir>",sizeof(unsigned char),
6);_tmpFB->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmpFC=_cycalloc(sizeof(*
_tmpFC));_tmpFC[0]=({struct Cyc_Arg_Flag_spec_struct _tmpFD;_tmpFD.tag=1;_tmpFD.f1=
Cyc_add_cyclone_lib_path;_tmpFD;});_tmpFC;});_tmpFB->f5=_tag_arr("Add to the list of directories for -l",
sizeof(unsigned char),38);_tmpFB;});_tmpE8[5]=({struct _tuple10*_tmpF8=_cycalloc(
sizeof(*_tmpF8));_tmpF8->f1=_tag_arr("-I",sizeof(unsigned char),3);_tmpF8->f2=1;
_tmpF8->f3=_tag_arr("<dir>",sizeof(unsigned char),6);_tmpF8->f4=(void*)({struct
Cyc_Arg_Flag_spec_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({struct Cyc_Arg_Flag_spec_struct
_tmpFA;_tmpFA.tag=1;_tmpFA.f1=Cyc_add_cpparg;_tmpFA;});_tmpF9;});_tmpF8->f5=
_tag_arr("Add to the list of directories to search for include files",sizeof(
unsigned char),59);_tmpF8;});_tmpE8[4]=({struct _tuple10*_tmpF5=_cycalloc(sizeof(*
_tmpF5));_tmpF5->f1=_tag_arr("-B",sizeof(unsigned char),3);_tmpF5->f2=1;_tmpF5->f3=
_tag_arr("<file>",sizeof(unsigned char),7);_tmpF5->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=({struct Cyc_Arg_Flag_spec_struct
_tmpF7;_tmpF7.tag=1;_tmpF7.f1=Cyc_add_cyclone_lib_path;_tmpF7;});_tmpF6;});
_tmpF5->f5=_tag_arr("Add to the list of directories to search for compiler files",
sizeof(unsigned char),60);_tmpF5;});_tmpE8[3]=({struct _tuple10*_tmpF2=_cycalloc(
sizeof(*_tmpF2));_tmpF2->f1=_tag_arr("-D",sizeof(unsigned char),3);_tmpF2->f2=1;
_tmpF2->f3=_tag_arr("<name>[=<value>]",sizeof(unsigned char),17);_tmpF2->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[
0]=({struct Cyc_Arg_Flag_spec_struct _tmpF4;_tmpF4.tag=1;_tmpF4.f1=Cyc_add_cpparg;
_tmpF4;});_tmpF3;});_tmpF2->f5=_tag_arr("Pass definition to preprocessor",
sizeof(unsigned char),32);_tmpF2;});_tmpE8[2]=({struct _tuple10*_tmpEF=_cycalloc(
sizeof(*_tmpEF));_tmpEF->f1=_tag_arr("-o",sizeof(unsigned char),3);_tmpEF->f2=0;
_tmpEF->f3=_tag_arr(" <file>",sizeof(unsigned char),8);_tmpEF->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=({
struct Cyc_Arg_String_spec_struct _tmpF1;_tmpF1.tag=5;_tmpF1.f1=Cyc_set_output_file;
_tmpF1;});_tmpF0;});_tmpEF->f5=_tag_arr("Set the output file name to <file>",
sizeof(unsigned char),35);_tmpEF;});_tmpE8[1]=({struct _tuple10*_tmpEC=_cycalloc(
sizeof(*_tmpEC));_tmpEC->f1=_tag_arr("--version",sizeof(unsigned char),10);
_tmpEC->f2=0;_tmpEC->f3=_tag_arr("",sizeof(unsigned char),1);_tmpEC->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED[0]=({
struct Cyc_Arg_Unit_spec_struct _tmpEE;_tmpEE.tag=0;_tmpEE.f1=Cyc_print_version;
_tmpEE;});_tmpED;});_tmpEC->f5=_tag_arr("Print version information and exit",
sizeof(unsigned char),35);_tmpEC;});_tmpE8[0]=({struct _tuple10*_tmpE9=_cycalloc(
sizeof(*_tmpE9));_tmpE9->f1=_tag_arr("-v",sizeof(unsigned char),3);_tmpE9->f2=0;
_tmpE9->f3=_tag_arr("",sizeof(unsigned char),1);_tmpE9->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=({struct Cyc_Arg_Set_spec_struct
_tmpEB;_tmpEB.tag=3;_tmpEB.f1=& Cyc_v_r;_tmpEB;});_tmpEA;});_tmpE9->f5=_tag_arr("Print compilation stages verbosely",
sizeof(unsigned char),35);_tmpE9;});((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmpE8,sizeof(struct _tuple10*),48));});Cyc_Arg_parse(
options,Cyc_add_other,_tag_arr("Options:",sizeof(unsigned char),9),argv);if(Cyc_cyclone_files
== 0? Cyc_ccargs == 0: 0){({void*_tmpAD[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("missing file",sizeof(unsigned char),13),_tag_arr(_tmpAD,sizeof(void*),
0));});exit(1);}{struct _tagged_arr cyclone_exec_prefix=(struct _tagged_arr)Cyc_Std_getenv(
_tag_arr("CYCLONE_EXEC_PREFIX",sizeof(unsigned char),20));if(cyclone_exec_prefix.curr
!= ((struct _tagged_arr)_tag_arr(0,0,0)).curr){Cyc_cyclone_lib_path=({struct Cyc_List_List*
_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->hd=({struct _tagged_arr*_tmpAF=
_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=cyclone_exec_prefix;_tmpAF;});_tmpAE->tl=
Cyc_cyclone_lib_path;_tmpAE;});}{struct _tagged_arr def_lib_path=(struct
_tagged_arr)Cstring_to_string(Cdef_lib_path);if(Cyc_Std_strlen(def_lib_path)> 0){
Cyc_cyclone_lib_path=({struct Cyc_List_List*_tmpB0=_cycalloc(sizeof(*_tmpB0));
_tmpB0->hd=({struct _tagged_arr*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=
def_lib_path;_tmpB1;});_tmpB0->tl=Cyc_cyclone_lib_path;_tmpB0;});}{struct Cyc_List_List*
pp=Cyc_cyclone_lib_path;while(pp != 0){Cyc_add_ccarg((struct _tagged_arr)Cyc_Std_strconcat(
_tag_arr("-L",sizeof(unsigned char),3),*((struct _tagged_arr*)pp->hd)));pp=pp->tl;}
Cyc_cyclone_lib_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_cyclone_lib_path);Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_lib_path,_tag_arr("include/cyc_include.h",
sizeof(unsigned char),22));{struct Cyc_List_List*_tmpB2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmpB2 != 0;_tmpB2=
_tmpB2->tl){Cyc_process_file(*((struct _tagged_arr*)_tmpB2->hd));if(Cyc_compile_failure){
return 1;}}}if(((Cyc_stop_after_cpp_r? 1: Cyc_parseonly_r)? 1: Cyc_tc_r)? 1: Cyc_toc_r){
return 0;}if(Cyc_ccargs == 0){return 0;}Cyc_ccargs=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{struct _tagged_arr _tmpB3=Cyc_Std_str_sepstr(((
struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs),_tag_arr(" ",sizeof(
unsigned char),2));Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(Cyc_libargs);{struct _tagged_arr _tmpB4=Cyc_Std_str_sepstr(({struct
Cyc_List_List*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->hd=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(unsigned char),1);_tmpE6->tl=((
struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs);_tmpE6;}),_tag_arr(" ",
sizeof(unsigned char),2));struct Cyc_List_List*stdlib;struct _tagged_arr
stdlib_string;int _tmpB5=((Cyc_stop_after_asmfile_r? 1: Cyc_stop_after_objectfile_r)?
1:(Cyc_output_file != 0? Cyc_Filename_check_suffix(*((struct _tagged_arr*)
_check_null(Cyc_output_file)),_tag_arr(".a",sizeof(unsigned char),3)): 0))? 1:(Cyc_output_file
!= 0? Cyc_Filename_check_suffix(*((struct _tagged_arr*)_check_null(Cyc_output_file)),
_tag_arr(".lib",sizeof(unsigned char),5)): 0);if(_tmpB5){stdlib=0;stdlib_string=
_tag_arr("",sizeof(unsigned char),1);}else{struct _tagged_arr libcyc_filename;
struct _tagged_arr nogc_filename;struct _tagged_arr gc_filename;if(Cyc_pa_r){
libcyc_filename=_tag_arr("libcyc_a.a",sizeof(unsigned char),11);nogc_filename=
_tag_arr("nogc_a.a",sizeof(unsigned char),9);}else{if(Cyc_nocheck_r){
libcyc_filename=_tag_arr("libcyc_nocheck.a",sizeof(unsigned char),17);
nogc_filename=_tag_arr("nogc_nocheck.a",sizeof(unsigned char),15);}else{
libcyc_filename=_tag_arr("libcyc.a",sizeof(unsigned char),9);nogc_filename=
_tag_arr("nogc.a",sizeof(unsigned char),7);}}gc_filename=_tag_arr("gc.a",sizeof(
unsigned char),5);{struct _tagged_arr _tmpB6=Cyc_do_find(Cyc_cyclone_lib_path,
libcyc_filename);struct _tagged_arr _tmpB7=Cyc_nogc_r? Cyc_do_find(Cyc_cyclone_lib_path,
nogc_filename): Cyc_do_find(Cyc_cyclone_lib_path,gc_filename);stdlib=({struct
_tagged_arr*_tmpB8[1];_tmpB8[0]=({struct _tagged_arr*_tmpB9=_cycalloc(sizeof(*
_tmpB9));_tmpB9[0]=_tmpB6;_tmpB9;});((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmpB8,sizeof(struct _tagged_arr*),1));});stdlib_string=(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpBC;_tmpBC.tag=0;_tmpBC.f1=(
struct _tagged_arr)_tmpB7;{struct Cyc_Std_String_pa_struct _tmpBB;_tmpBB.tag=0;
_tmpBB.f1=(struct _tagged_arr)_tmpB6;{void*_tmpBA[2]={& _tmpBB,& _tmpBC};Cyc_Std_aprintf(
_tag_arr(" %s %s",sizeof(unsigned char),7),_tag_arr(_tmpBA,sizeof(void*),2));}}});}}
if(Cyc_ic_r){struct _handler_cons _tmpBD;_push_handler(& _tmpBD);{int _tmpBF=0;if(
setjmp(_tmpBD.handler)){_tmpBF=1;}if(! _tmpBF){Cyc_ccargs=((struct Cyc_List_List*(*)(
int(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,
Cyc_ccargs);Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _tagged_arr*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*
_tmpC0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));if(! _tmpB5){_tmpC0=({
struct Cyc_List_List*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->hd=Cyc_final_strptr;
_tmpC1->tl=_tmpC0;_tmpC1;});}{struct Cyc_Interface_I*_tmpC2=((struct Cyc_Interface_I*(*)(
struct Cyc_Interface_I*(*get)(struct _tagged_arr*),struct Cyc_List_List*la,struct
Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,_tmpC0,
_tmpC0);if(_tmpC2 == 0){({void*_tmpC3[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: interfaces incompatible\n",sizeof(unsigned char),32),_tag_arr(
_tmpC3,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmpC4=
1;_npop_handler(0);return _tmpC4;}}if(_tmpB5){if(Cyc_output_file != 0){struct
_tagged_arr _tmpC5=({struct Cyc_Std_String_pa_struct _tmpC9;_tmpC9.tag=0;_tmpC9.f1=(
struct _tagged_arr)Cyc_Filename_chop_extension(*((struct _tagged_arr*)_check_null(
Cyc_output_file)));{void*_tmpC8[1]={& _tmpC9};Cyc_Std_aprintf(_tag_arr("%s.cycio",
sizeof(unsigned char),9),_tag_arr(_tmpC8,sizeof(void*),1));}});struct Cyc_Std___sFILE*
_tmpC6=Cyc_try_file_open((struct _tagged_arr)_tmpC5,_tag_arr("w",sizeof(
unsigned char),2),_tag_arr("interface object file",sizeof(unsigned char),22));
if(_tmpC6 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmpC7=1;
_npop_handler(0);return _tmpC7;}}Cyc_Interface_save((struct Cyc_Interface_I*)
_check_null(_tmpC2),(struct Cyc_Std___sFILE*)_check_null(_tmpC6));Cyc_Std_file_close((
struct Cyc_Std___sFILE*)_check_null(_tmpC6));}}else{if(! Cyc_Interface_is_subinterface(
Cyc_Interface_empty(),(struct Cyc_Interface_I*)_check_null(_tmpC2),({struct
_tuple8*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->f1=_tag_arr("empty interface",
sizeof(unsigned char),16);_tmpCA->f2=_tag_arr("global interface",sizeof(
unsigned char),17);_tmpCA;}))){({void*_tmpCB[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: some objects are still undefined\n",sizeof(unsigned char),41),
_tag_arr(_tmpCB,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmpCC=1;_npop_handler(0);return _tmpCC;}}}}};_pop_handler();}else{void*_tmpBE=(
void*)_exn_thrown;void*_tmpCE=_tmpBE;struct _tagged_arr _tmpCF;struct _tagged_arr
_tmpD0;struct _tagged_arr _tmpD1;_LL3D: if(*((void**)_tmpCE)== Cyc_Core_Failure){
_LL49: _tmpCF=((struct Cyc_Core_Failure_struct*)_tmpCE)->f1;goto _LL3E;}else{goto
_LL3F;}_LL3F: if(*((void**)_tmpCE)== Cyc_Core_Impossible){_LL4A: _tmpD0=((struct
Cyc_Core_Impossible_struct*)_tmpCE)->f1;goto _LL40;}else{goto _LL41;}_LL41: if(
_tmpCE == Cyc_Dict_Absent){goto _LL42;}else{goto _LL43;}_LL43: if(*((void**)_tmpCE)
== Cyc_Core_Invalid_argument){_LL4B: _tmpD1=((struct Cyc_Core_Invalid_argument_struct*)
_tmpCE)->f1;goto _LL44;}else{goto _LL45;}_LL45: goto _LL46;_LL47: goto _LL48;_LL3E:({
struct Cyc_Std_String_pa_struct _tmpD3;_tmpD3.tag=0;_tmpD3.f1=(struct _tagged_arr)
_tmpCF;{void*_tmpD2[1]={& _tmpD3};Cyc_Std_printf(_tag_arr("Exception Core::Failure %s\n",
sizeof(unsigned char),28),_tag_arr(_tmpD2,sizeof(void*),1));}});Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;_LL40:({struct Cyc_Std_String_pa_struct _tmpD5;
_tmpD5.tag=0;_tmpD5.f1=(struct _tagged_arr)_tmpD0;{void*_tmpD4[1]={& _tmpD5};Cyc_Std_printf(
_tag_arr("Exception Core::Impossible %s\n",sizeof(unsigned char),31),_tag_arr(
_tmpD4,sizeof(void*),1));}});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL42:({void*_tmpD6[0]={};Cyc_Std_printf(_tag_arr("Exception Dict::Absent\n",
sizeof(unsigned char),24),_tag_arr(_tmpD6,sizeof(void*),0));});Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;_LL44:({struct Cyc_Std_String_pa_struct _tmpD8;
_tmpD8.tag=0;_tmpD8.f1=(struct _tagged_arr)_tmpD1;{void*_tmpD7[1]={& _tmpD8};Cyc_Std_printf(
_tag_arr("Exception Core::Invalid_argument %s\n",sizeof(unsigned char),37),
_tag_arr(_tmpD7,sizeof(void*),1));}});Cyc_compile_failure=1;Cyc_remove_cfiles();
return 1;_LL46:({void*_tmpD9[0]={};Cyc_Std_printf(_tag_arr("Uncaught exception\n",
sizeof(unsigned char),20),_tag_arr(_tmpD9,sizeof(void*),0));});Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;_LL48:(void)_throw(_tmpCE);_LL3C:;}}}{struct
_tagged_arr _tmpDA=({struct Cyc_Std_String_pa_struct _tmpE3;_tmpE3.tag=0;_tmpE3.f1=(
struct _tagged_arr)_tmpB4;{struct Cyc_Std_String_pa_struct _tmpE2;_tmpE2.tag=0;
_tmpE2.f1=(struct _tagged_arr)stdlib_string;{struct Cyc_Std_String_pa_struct _tmpE1;
_tmpE1.tag=0;_tmpE1.f1=(struct _tagged_arr)_tmpB3;{struct Cyc_Std_String_pa_struct
_tmpE0;_tmpE0.tag=0;_tmpE0.f1=(struct _tagged_arr)(Cyc_output_file == 0? _tag_arr("",
sizeof(unsigned char),1):(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmpE5;_tmpE5.tag=0;_tmpE5.f1=(struct _tagged_arr)Cyc_sh_escape_string(*((struct
_tagged_arr*)_check_null(Cyc_output_file)));{void*_tmpE4[1]={& _tmpE5};Cyc_Std_aprintf(
_tag_arr(" -o %s",sizeof(unsigned char),7),_tag_arr(_tmpE4,sizeof(void*),1));}}));{
struct Cyc_Std_String_pa_struct _tmpDF;_tmpDF.tag=0;_tmpDF.f1=(struct _tagged_arr)
comp;{void*_tmpDE[5]={& _tmpDF,& _tmpE0,& _tmpE1,& _tmpE2,& _tmpE3};Cyc_Std_aprintf(
_tag_arr("%s %s %s%s%s",sizeof(unsigned char),13),_tag_arr(_tmpDE,sizeof(void*),
5));}}}}}});if(Cyc_v_r){({struct Cyc_Std_String_pa_struct _tmpDC;_tmpDC.tag=0;
_tmpDC.f1=(struct _tagged_arr)_tmpDA;{void*_tmpDB[1]={& _tmpDC};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s\n",sizeof(unsigned char),4),_tag_arr(_tmpDB,sizeof(
void*),1));}});}if(Cyc_Std_system((struct _tagged_arr)_tmpDA)!= 0){({void*_tmpDD[
0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: C compiler failed\n",
sizeof(unsigned char),26),_tag_arr(_tmpDD,sizeof(void*),0));});Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;}Cyc_remove_cfiles();return Cyc_compile_failure? 1: 0;}}}}}}}}}
