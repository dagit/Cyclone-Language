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
  unsigned total_bytes;
  unsigned free_bytes;
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
extern void   _reset_region(struct _RegionHandle *);

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
extern int _throw(void* e);
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
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (!_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })
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
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
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
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

static inline void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned x, unsigned y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void* _profile_GC_malloc(int,char *file,int lineno);
extern void* _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
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
 void exit(int);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern char
Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct _tagged_arr f1;
};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{char*tag;
struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct
_tagged_arr Cstring_to_string(char*);struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_List_List*
Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[8];
struct Cyc_Arg_Bad_struct{char*tag;struct _tagged_arr f1;};extern char Cyc_Arg_Error[
10];struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};struct Cyc_Arg_FlagString_spec_struct{int
tag;void(*f1)(struct _tagged_arr,struct _tagged_arr);};struct Cyc_Arg_Set_spec_struct{
int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*
f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _tagged_arr);};
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _tagged_arr),
struct _tagged_arr errmsg,struct _tagged_arr args);typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc_Std___mbstate_t;typedef struct{int
__pos;Cyc_Std___mbstate_t __state;}Cyc_Std__G_fpos_t;typedef Cyc_Std__G_fpos_t Cyc_Std_fpos_t;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stdout;extern
struct Cyc_Std___cycFILE*Cyc_Std_stderr;struct Cyc_Std_Cstdio___abstractFILE;
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_Std_fclose(struct Cyc_Std___cycFILE*);int Cyc_Std_feof(struct
Cyc_Std___cycFILE*);int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);struct Cyc_Std___cycFILE*
Cyc_Std_fopen(struct _tagged_arr,struct _tagged_arr);int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,
struct _tagged_arr,struct _tagged_arr);unsigned int Cyc_Std_fread(struct _tagged_arr,
unsigned int,unsigned int,struct Cyc_Std___cycFILE*);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};unsigned int Cyc_Std_fwrite(struct _tagged_arr,unsigned int,
unsigned int,struct Cyc_Std___cycFILE*);int Cyc_Std_printf(struct _tagged_arr,
struct _tagged_arr);int Cyc_Std_remove(struct _tagged_arr);extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};struct Cyc_Std___cycFILE*Cyc_Std_file_open(struct
_tagged_arr,struct _tagged_arr);void Cyc_Std_file_close(struct Cyc_Std___cycFILE*);
typedef struct{int quot;int rem;}Cyc_Std_div_t;typedef struct{int quot;int rem;}Cyc_Std_ldiv_t;
struct _tagged_arr Cyc_Std_getenv(struct _tagged_arr);int Cyc_Std_system(struct
_tagged_arr);unsigned int Cyc_Std_strlen(struct _tagged_arr s);int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_Std_strconcat_l(struct
Cyc_List_List*);struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,struct
_tagged_arr);struct _tagged_arr Cyc_Std_substring(struct _tagged_arr,int ofs,
unsigned int n);struct _tagged_arr Cyc_Std_strchr(struct _tagged_arr s,char c);struct
Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct
Cyc_Iter_Iter,void*);struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern
char Cyc_Dict_Absent[11];struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct _tagged_arr Cyc_Filename_concat(
struct _tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_Filename_chop_extension(
struct _tagged_arr);struct _tagged_arr Cyc_Filename_dirname(struct _tagged_arr);
struct _tagged_arr Cyc_Filename_basename(struct _tagged_arr);int Cyc_Filename_check_suffix(
struct _tagged_arr,struct _tagged_arr);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];void Cyc_Position_reset_position(struct _tagged_arr);struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];int Cyc_Position_error_p();
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};
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
_tuple1*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*
tunion_name;struct _tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple1*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
Cyc_Absyn_Aggrdecl**f1;};struct Cyc_Absyn_AggrInfo{void*aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_TunionType_struct{int tag;struct Cyc_Absyn_TunionInfo f1;};
struct Cyc_Absyn_TunionFieldType_struct{int tag;struct Cyc_Absyn_TunionFieldInfo f1;
};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct
Cyc_Absyn_IntType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;void*f1;struct Cyc_Absyn_Tqual
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo
f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{
int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;
void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct
_tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};
struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeInt_struct{int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{int tag;
void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
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
int tag;void*f1;char f2;};struct Cyc_Absyn_Short_c_struct{int tag;void*f1;short f2;};
struct Cyc_Absyn_Int_c_struct{int tag;void*f1;int f2;};struct Cyc_Absyn_LongLong_c_struct{
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
};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
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
f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_ForArray_s_struct{
int tag;struct Cyc_Absyn_ForArrayInfo f1;};struct Cyc_Absyn_ResetRegion_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;
int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct
Cyc_Absyn_AggrInfo f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Tunion_p_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*
f3;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Pat{void*r;
struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*cnst_exp;struct Cyc_Absyn_Stmt*body;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Aggrfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;};struct Cyc_Absyn_Aggrdecl{void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _tagged_arr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{
void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];extern char Cyc_Lexing_Error[
10];struct Cyc_Lexing_Error_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_List_List*
Cyc_Parse_parse_file(struct Cyc_Std___cycFILE*f);extern int Cyc_Parse_no_register;
struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;int
Cyc_yyparse();extern char Cyc_AbstractDeclarator_tok[27];struct Cyc_AbstractDeclarator_tok_struct{
char*tag;struct Cyc_Abstractdeclarator*f1;};extern char Cyc_AggrFieldDeclListList_tok[
30];struct Cyc_AggrFieldDeclListList_tok_struct{char*tag;struct Cyc_List_List*f1;}
;extern char Cyc_AggrFieldDeclList_tok[26];struct Cyc_AggrFieldDeclList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_AttributeList_tok[22];struct Cyc_AttributeList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Attribute_tok[18];struct Cyc_Attribute_tok_struct{
char*tag;void*f1;};extern char Cyc_Bool_tok[13];struct Cyc_Bool_tok_struct{char*tag;
int f1;};extern char Cyc_Char_tok[13];struct Cyc_Char_tok_struct{char*tag;char f1;};
extern char Cyc_DeclList_tok[17];struct Cyc_DeclList_tok_struct{char*tag;struct Cyc_List_List*
f1;};extern char Cyc_DeclSpec_tok[17];struct Cyc_DeclSpec_tok_struct{char*tag;
struct Cyc_Declaration_spec*f1;};extern char Cyc_Declarator_tok[19];struct Cyc_Declarator_tok_struct{
char*tag;struct Cyc_Declarator*f1;};extern char Cyc_DesignatorList_tok[23];struct
Cyc_DesignatorList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_Designator_tok[
19];struct Cyc_Designator_tok_struct{char*tag;void*f1;};extern char Cyc_EnumfieldList_tok[
22];struct Cyc_EnumfieldList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern
char Cyc_Enumfield_tok[18];struct Cyc_Enumfield_tok_struct{char*tag;struct Cyc_Absyn_Enumfield*
f1;};extern char Cyc_ExpList_tok[16];struct Cyc_ExpList_tok_struct{char*tag;struct
Cyc_List_List*f1;};extern char Cyc_Exp_tok[12];struct Cyc_Exp_tok_struct{char*tag;
struct Cyc_Absyn_Exp*f1;};extern char Cyc_FieldPatternList_tok[25];struct Cyc_FieldPatternList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_FieldPattern_tok[21];struct
_tuple4{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
char*tag;struct _tuple4*f1;};extern char Cyc_FnDecl_tok[15];struct Cyc_FnDecl_tok_struct{
char*tag;struct Cyc_Absyn_Fndecl*f1;};extern char Cyc_IdList_tok[15];struct Cyc_IdList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_InitDeclList_tok[21];struct Cyc_InitDeclList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_InitDecl_tok[17];struct _tuple5{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
char*tag;struct _tuple5*f1;};extern char Cyc_InitializerList_tok[24];struct Cyc_InitializerList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Int_tok[12];struct _tuple6{void*
f1;int f2;};struct Cyc_Int_tok_struct{char*tag;struct _tuple6*f1;};extern char Cyc_Kind_tok[
13];struct Cyc_Kind_tok_struct{char*tag;void*f1;};extern char Cyc_Okay_tok[13];
extern char Cyc_ParamDeclListBool_tok[26];struct _tuple7{struct Cyc_List_List*f1;int
f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};
struct Cyc_ParamDeclListBool_tok_struct{char*tag;struct _tuple7*f1;};extern char Cyc_ParamDeclList_tok[
22];struct Cyc_ParamDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern
char Cyc_ParamDecl_tok[18];struct Cyc_ParamDecl_tok_struct{char*tag;struct _tuple2*
f1;};extern char Cyc_PatternList_tok[20];struct Cyc_PatternList_tok_struct{char*tag;
struct Cyc_List_List*f1;};extern char Cyc_Pattern_tok[16];struct Cyc_Pattern_tok_struct{
char*tag;struct Cyc_Absyn_Pat*f1;};extern char Cyc_Pointer_Sort_tok[21];struct Cyc_Pointer_Sort_tok_struct{
char*tag;void*f1;};extern char Cyc_Primop_tok[15];struct Cyc_Primop_tok_struct{char*
tag;void*f1;};extern char Cyc_Primopopt_tok[18];struct Cyc_Primopopt_tok_struct{
char*tag;struct Cyc_Core_Opt*f1;};extern char Cyc_QualId_tok[15];struct Cyc_QualId_tok_struct{
char*tag;struct _tuple1*f1;};extern char Cyc_QualSpecList_tok[21];struct _tuple8{
struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
Cyc_QualSpecList_tok_struct{char*tag;struct _tuple8*f1;};extern char Cyc_Rgnorder_tok[
17];struct Cyc_Rgnorder_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char
Cyc_Scope_tok[14];struct Cyc_Scope_tok_struct{char*tag;void*f1;};extern char Cyc_Short_tok[
14];struct Cyc_Short_tok_struct{char*tag;short f1;};extern char Cyc_Stmt_tok[13];
struct Cyc_Stmt_tok_struct{char*tag;struct Cyc_Absyn_Stmt*f1;};extern char Cyc_StorageClass_tok[
21];struct Cyc_StorageClass_tok_struct{char*tag;void*f1;};extern char Cyc_String_tok[
15];struct Cyc_String_tok_struct{char*tag;struct _tagged_arr f1;};extern char Cyc_Stringopt_tok[
18];struct Cyc_Stringopt_tok_struct{char*tag;struct Cyc_Core_Opt*f1;};extern char
Cyc_StructOrUnion_tok[22];struct Cyc_StructOrUnion_tok_struct{char*tag;void*f1;};
extern char Cyc_SwitchCClauseList_tok[26];struct Cyc_SwitchCClauseList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_SwitchClauseList_tok[25];struct
Cyc_SwitchClauseList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_TunionFieldList_tok[
24];struct Cyc_TunionFieldList_tok_struct{char*tag;struct Cyc_List_List*f1;};
extern char Cyc_TunionField_tok[20];struct Cyc_TunionField_tok_struct{char*tag;
struct Cyc_Absyn_Tunionfield*f1;};extern char Cyc_TypeList_tok[17];struct Cyc_TypeList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_TypeModifierList_tok[25];struct
Cyc_TypeModifierList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_TypeOpt_tok[
16];struct Cyc_TypeOpt_tok_struct{char*tag;struct Cyc_Core_Opt*f1;};extern char Cyc_TypeQual_tok[
17];struct Cyc_TypeQual_tok_struct{char*tag;struct Cyc_Absyn_Tqual f1;};extern char
Cyc_TypeSpecifier_tok[22];struct Cyc_TypeSpecifier_tok_struct{char*tag;void*f1;};
extern char Cyc_Type_tok[13];struct Cyc_Type_tok_struct{char*tag;void*f1;};int Cyc_yyparse();
extern char Cyc_YY1[8];struct Cyc_YY1_struct{char*tag;struct _tuple7*f1;};extern char
Cyc_YYINITIALSVAL[18];struct Cyc_Yyltype{int timestamp;int first_line;int
first_column;int last_line;int last_column;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f);void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs);void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc_Std___cycFILE*f);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];
struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct
Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct
Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;
};struct Cyc_Tcenv_Frame_struct{int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*
ae;struct Cyc_Core_Opt*le;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();void Cyc_Tcutil_flush_warnings();
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Tovc_tovc(
struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};extern char Cyc_CfFlowInfo_IsZero[11];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_NotZero[12];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[13];struct
Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int
tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int
tag;struct Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds);struct Cyc_Interface_I;struct Cyc_Interface_I*Cyc_Interface_empty();
struct Cyc_Interface_I*Cyc_Interface_final();struct Cyc_Interface_I*Cyc_Interface_extract(
struct Cyc_Dict_Dict*ae);struct _tuple9{struct _tagged_arr f1;struct _tagged_arr f2;};
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*
i2,struct _tuple9*info);struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(
struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*
linfo);struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc_Std___cycFILE*);void
Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc_Std___cycFILE*);struct Cyc_Interface_I*
Cyc_Interface_load(struct Cyc_Std___cycFILE*);void Cyc_Lex_lex_init();int Cyc_Toc_warn_bounds_checks;
static int Cyc_pp_r=0;static int Cyc_noexpand_r=0;static int Cyc_noshake_r=0;static int
Cyc_stop_after_cpp_r=0;static int Cyc_parseonly_r=0;static int Cyc_tc_r=0;static int
Cyc_ic_r=0;static int Cyc_toc_r=0;static int Cyc_stop_after_objectfile_r=0;static int
Cyc_stop_after_asmfile_r=0;static int Cyc_tovc_r=0;static int Cyc_v_r=0;static int Cyc_save_temps_r=
0;static int Cyc_save_c_r=0;static int Cyc_nogc_r=0;static int Cyc_pa_r=0;static int Cyc_pg_r=
0;static int Cyc_nocheck_r=0;static int Cyc_add_cyc_namespace_r=1;static int Cyc_generate_line_directives_r=
0;static int Cyc_print_full_evars_r=0;static int Cyc_print_all_tvars_r=0;static int
Cyc_print_all_kinds_r=0;static struct _tagged_arr*Cyc_output_file=0;static void Cyc_set_output_file(
struct _tagged_arr s){Cyc_output_file=({struct _tagged_arr*_tmp0=_cycalloc(sizeof(*
_tmp0));_tmp0[0]=s;_tmp0;});}extern char*Cdef_inc_path;extern char*Cdef_lib_path;
extern char*Carch;extern char*Ccomp;extern char*Cversion;static char _tmp1[1]="";
static struct _tagged_arr Cyc_cpp={_tmp1,_tmp1,_tmp1 + 1};static void Cyc_set_cpp(
struct _tagged_arr s){Cyc_cpp=s;}static struct Cyc_List_List*Cyc_cppargs=0;static
void Cyc_add_cpparg(struct _tagged_arr s){Cyc_cppargs=({struct Cyc_List_List*_tmp2=
_cycalloc(sizeof(*_tmp2));_tmp2->hd=({struct _tagged_arr*_tmp3=_cycalloc(sizeof(*
_tmp3));_tmp3[0]=s;_tmp3;});_tmp2->tl=Cyc_cppargs;_tmp2;});}static struct
_tagged_arr Cyc_target_arch=(struct _tagged_arr){(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_target_arch(struct _tagged_arr s){Cyc_toc_r=1;Cyc_target_arch=s;}
static void Cyc_print_version(){({struct Cyc_Std_String_pa_struct _tmp5;_tmp5.tag=0;
_tmp5.f1=(struct _tagged_arr)Cstring_to_string(Cversion);{void*_tmp4[1]={& _tmp5};
Cyc_Std_printf(_tag_arr("The Cyclone compiler, version %s\n",sizeof(char),34),
_tag_arr(_tmp4,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct _tmp8;_tmp8.tag=
0;_tmp8.f1=(struct _tagged_arr)Cstring_to_string(Ccomp);{struct Cyc_Std_String_pa_struct
_tmp7;_tmp7.tag=0;_tmp7.f1=(struct _tagged_arr)Cstring_to_string(Carch);{void*
_tmp6[2]={& _tmp7,& _tmp8};Cyc_Std_printf(_tag_arr("Compiled for architecture: %s CC=\"%s\"\n",
sizeof(char),39),_tag_arr(_tmp6,sizeof(void*),2));}}});({struct Cyc_Std_String_pa_struct
_tmpA;_tmpA.tag=0;_tmpA.f1=(struct _tagged_arr)Cstring_to_string(Cdef_lib_path);{
void*_tmp9[1]={& _tmpA};Cyc_Std_printf(_tag_arr("Standard library directory: %s\n",
sizeof(char),32),_tag_arr(_tmp9,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmpC;_tmpC.tag=0;_tmpC.f1=(struct _tagged_arr)Cstring_to_string(Cdef_inc_path);{
void*_tmpB[1]={& _tmpC};Cyc_Std_printf(_tag_arr("Standard include directory: %s\n",
sizeof(char),32),_tag_arr(_tmpB,sizeof(void*),1));}});exit(0);}static int Cyc_is_cyclone_sourcefile(
struct _tagged_arr s){unsigned int _tmpD=Cyc_Std_strlen(s);if(_tmpD <= 4)return 0;
else{return Cyc_Std_strcmp(_tagged_arr_plus(s,sizeof(char),(int)(_tmpD - 4)),
_tag_arr(".cyc",sizeof(char),5))== 0;}}static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _tagged_arr s){Cyc_ccargs=({struct Cyc_List_List*
_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->hd=({struct _tagged_arr*_tmpF=_cycalloc(
sizeof(*_tmpF));_tmpF[0]=s;_tmpF;});_tmpE->tl=Cyc_ccargs;_tmpE;});}static struct
Cyc_List_List*Cyc_cyclone_exec_path=0;static void Cyc_add_cyclone_exec_path(struct
_tagged_arr s){unsigned int _tmp10=Cyc_Std_strlen(s);if(_tmp10 <= 2)return;{struct
_tagged_arr _tmp11=(struct _tagged_arr)Cyc_Std_substring(s,2,_tmp10 - 2);Cyc_cyclone_exec_path=({
struct Cyc_List_List*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->hd=({struct
_tagged_arr*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13[0]=_tmp11;_tmp13;});_tmp12->tl=
Cyc_cyclone_exec_path;_tmp12;});}}static struct Cyc_List_List*Cyc_libargs=0;static
void Cyc_add_libarg(struct _tagged_arr s){if(Cyc_Std_strcmp(s,_tag_arr("-lxml",
sizeof(char),6))== 0)Cyc_add_ccarg(s);else{Cyc_libargs=({struct Cyc_List_List*
_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->hd=({struct _tagged_arr*_tmp15=
_cycalloc(sizeof(*_tmp15));_tmp15[0]=s;_tmp15;});_tmp14->tl=Cyc_libargs;_tmp14;});}}
static void Cyc_set_save_temps(){Cyc_save_temps_r=1;Cyc_add_ccarg(_tag_arr("-save-temps",
sizeof(char),12));}static int Cyc_produce_dependencies=0;static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=1;Cyc_add_cpparg(_tag_arr("-M",
sizeof(char),3));}static struct _tagged_arr*Cyc_dependencies_target=0;static void
Cyc_set_dependencies_target(struct _tagged_arr s){Cyc_dependencies_target=({struct
_tagged_arr*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=s;_tmp16;});}static void
Cyc_set_stop_after_objectfile(){Cyc_stop_after_objectfile_r=1;Cyc_add_ccarg(
_tag_arr("-c",sizeof(char),3));}static void Cyc_set_nocppprecomp(){Cyc_add_cpparg(
_tag_arr("-no-cpp-precomp",sizeof(char),16));Cyc_add_ccarg(_tag_arr("-no-cpp-precomp",
sizeof(char),16));}static void Cyc_set_lineno(){Cyc_generate_line_directives_r=1;
Cyc_set_save_temps();}static void Cyc_set_noboundschecks(){Cyc_add_ccarg(_tag_arr("-DNO_CYC_BOUNDS_CHECKS",
sizeof(char),23));}static void Cyc_set_nonullchecks(){Cyc_add_ccarg(_tag_arr("-DNO_CYC_NULL_CHECKS",
sizeof(char),21));}static void Cyc_set_nochecks(){Cyc_set_noboundschecks();Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=0;Cyc_add_ccarg(
_tag_arr("-DNO_CYC_PREFIX",sizeof(char),16));}static void Cyc_set_pa(){Cyc_pa_r=1;
Cyc_add_ccarg(_tag_arr("-DCYC_REGION_PROFILE",sizeof(char),21));}static void Cyc_set_pg(){
Cyc_pg_r=1;Cyc_add_ccarg(_tag_arr("-pg",sizeof(char),4));}static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;Cyc_add_ccarg(_tag_arr("-S",sizeof(char),3));}static
void*Cyc_intype=(void*)0;static void Cyc_set_inputtype(struct _tagged_arr s){if(Cyc_Std_strcmp(
s,_tag_arr("cyc",sizeof(char),4))== 0)Cyc_intype=(void*)1;else{if(Cyc_Std_strcmp(
s,_tag_arr("none",sizeof(char),5))== 0)Cyc_intype=(void*)0;else{({struct Cyc_Std_String_pa_struct
_tmp18;_tmp18.tag=0;_tmp18.f1=(struct _tagged_arr)s;{void*_tmp17[1]={& _tmp18};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Input type '%s' not supported\n",sizeof(char),31),
_tag_arr(_tmp17,sizeof(void*),1));}});}}}struct _tagged_arr Cyc_make_base_filename(
struct _tagged_arr s,struct _tagged_arr*output_file){struct _tagged_arr _tmp19=Cyc_Filename_dirname(
output_file == 0?s:*output_file);struct _tagged_arr _tmp1A=Cyc_Filename_chop_extension((
struct _tagged_arr)Cyc_Filename_basename(s));struct _tagged_arr _tmp1B=Cyc_Std_strlen((
struct _tagged_arr)_tmp19)> 0?Cyc_Filename_concat((struct _tagged_arr)_tmp19,(
struct _tagged_arr)_tmp1A): _tmp1A;return(struct _tagged_arr)_tmp1B;}static struct
Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(struct _tagged_arr s){
if(Cyc_intype == (void*)1?1: Cyc_is_cyclone_sourcefile(s)){Cyc_cyclone_files=({
struct Cyc_List_List*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->hd=({struct
_tagged_arr*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D[0]=s;_tmp1D;});_tmp1C->tl=
Cyc_cyclone_files;_tmp1C;});{struct _tagged_arr _tmp1E=Cyc_make_base_filename(s,
Cyc_output_file);struct _tagged_arr _tmp1F=Cyc_Std_strconcat(_tmp1E,_tag_arr(".c",
sizeof(char),3));Cyc_add_ccarg((struct _tagged_arr)_tmp1F);}}else{Cyc_add_ccarg(s);}}
static void Cyc_remove_file(struct _tagged_arr s){if(Cyc_save_temps_r)return;else{
Cyc_Std_remove(s);}}int Cyc_compile_failure=0;struct Cyc_Std___cycFILE*Cyc_try_file_open(
struct _tagged_arr filename,struct _tagged_arr mode,struct _tagged_arr msg_part){
struct _handler_cons _tmp20;_push_handler(& _tmp20);{int _tmp22=0;if(setjmp(_tmp20.handler))
_tmp22=1;if(!_tmp22){{struct Cyc_Std___cycFILE*_tmp23=(struct Cyc_Std___cycFILE*)
Cyc_Std_file_open(filename,mode);_npop_handler(0);return _tmp23;};_pop_handler();}
else{void*_tmp21=(void*)_exn_thrown;void*_tmp25=_tmp21;_LL1:;_LL2: Cyc_compile_failure=
1;({struct Cyc_Std_String_pa_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct
_tagged_arr)filename;{struct Cyc_Std_String_pa_struct _tmp27;_tmp27.tag=0;_tmp27.f1=(
struct _tagged_arr)msg_part;{void*_tmp26[2]={& _tmp27,& _tmp28};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\nError: couldn't open %s %s\n",sizeof(char),29),_tag_arr(_tmp26,
sizeof(void*),2));}}});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);
return 0;_LL3:;_LL4:(void)_throw(_tmp25);_LL0:;}}}struct Cyc_List_List*Cyc_do_stage(
struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),
void*env,struct Cyc_List_List*tds){struct _tagged_arr exn_string=_tag_arr("",
sizeof(char),1);struct _tagged_arr explain_string=_tag_arr("",sizeof(char),1);int
other_exn=0;void*ex=(void*)({struct Cyc_Core_Impossible_struct*_tmp39=_cycalloc(
sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Core_Impossible_struct _tmp3A;_tmp3A.tag=
Cyc_Core_Impossible;_tmp3A.f1=_tag_arr("",sizeof(char),1);_tmp3A;});_tmp39;});
struct Cyc_List_List*_tmp29=0;{struct _handler_cons _tmp2A;_push_handler(& _tmp2A);{
int _tmp2C=0;if(setjmp(_tmp2A.handler))_tmp2C=1;if(!_tmp2C){_tmp29=f(env,tds);;
_pop_handler();}else{void*_tmp2B=(void*)_exn_thrown;void*_tmp2E=_tmp2B;struct
_tagged_arr _tmp2F;struct _tagged_arr _tmp30;struct _tagged_arr _tmp31;_LL6: if(*((
void**)_tmp2E)!= Cyc_Core_Impossible)goto _LL8;_tmp2F=((struct Cyc_Core_Impossible_struct*)
_tmp2E)->f1;_LL7: exn_string=_tag_arr("Exception Core::Impossible",sizeof(char),
27);explain_string=_tmp2F;goto _LL5;_LL8: if(_tmp2E != Cyc_Dict_Absent)goto _LLA;
_LL9: exn_string=_tag_arr("Exception Dict::Absent",sizeof(char),23);goto _LL5;_LLA:
if(*((void**)_tmp2E)!= Cyc_Core_Invalid_argument)goto _LLC;_tmp30=((struct Cyc_Core_Invalid_argument_struct*)
_tmp2E)->f1;_LLB: exn_string=_tag_arr("Exception Core::Invalid_argument",sizeof(
char),33);explain_string=_tmp30;goto _LL5;_LLC: if(*((void**)_tmp2E)!= Cyc_Core_Failure)
goto _LLE;_tmp31=((struct Cyc_Core_Failure_struct*)_tmp2E)->f1;_LLD: exn_string=
_tag_arr("Exception Core::Failure",sizeof(char),24);explain_string=_tmp31;goto
_LL5;_LLE:;_LLF: ex=_tmp2E;other_exn=1;exn_string=_tag_arr("Uncaught exception",
sizeof(char),19);goto _LL5;_LL10:;_LL11:(void)_throw(_tmp2E);_LL5:;}}}if(Cyc_Position_error_p())
Cyc_compile_failure=1;if(Cyc_Std_strcmp(exn_string,_tag_arr("",sizeof(char),1))
!= 0){Cyc_compile_failure=1;({struct Cyc_Std_String_pa_struct _tmp35;_tmp35.tag=0;
_tmp35.f1=(struct _tagged_arr)explain_string;{struct Cyc_Std_String_pa_struct
_tmp34;_tmp34.tag=0;_tmp34.f1=(struct _tagged_arr)stage_name;{struct Cyc_Std_String_pa_struct
_tmp33;_tmp33.tag=0;_tmp33.f1=(struct _tagged_arr)exn_string;{void*_tmp32[3]={&
_tmp33,& _tmp34,& _tmp35};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n%s thrown during %s: %s",
sizeof(char),25),_tag_arr(_tmp32,sizeof(void*),3));}}}});}if(Cyc_compile_failure){({
void*_tmp36[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\nCOMPILATION FAILED!\n",
sizeof(char),22),_tag_arr(_tmp36,sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)
Cyc_Std_stderr);if(other_exn)(int)_throw(ex);return _tmp29;}else{if(Cyc_v_r){({
struct Cyc_Std_String_pa_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct _tagged_arr)
stage_name;{void*_tmp37[1]={& _tmp38};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s completed.\n",
sizeof(char),15),_tag_arr(_tmp37,sizeof(void*),1));}});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)
Cyc_Std_stderr);return _tmp29;}}return _tmp29;}struct Cyc_List_List*Cyc_do_parse(
struct Cyc_Std___cycFILE*f,struct Cyc_List_List*ignore){Cyc_Lex_lex_init();{struct
Cyc_List_List*_tmp3B=Cyc_Parse_parse_file(f);Cyc_Lex_lex_init();return _tmp3B;}}
struct Cyc_List_List*Cyc_do_typecheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
tds){Cyc_Tc_tc(te,1,tds);if(!Cyc_noshake_r)tds=Cyc_Tc_treeshake(te,tds);return
tds;}struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){Cyc_NewControlFlow_cf_check(
tds);return tds;}struct _tuple10{struct Cyc_Tcenv_Tenv*f1;struct Cyc_Std___cycFILE*
f2;struct Cyc_Std___cycFILE*f3;};struct Cyc_List_List*Cyc_do_interface(struct
_tuple10*params,struct Cyc_List_List*tds){struct _tuple10 _tmp3D;struct Cyc_Tcenv_Tenv*
_tmp3E;struct Cyc_Std___cycFILE*_tmp3F;struct Cyc_Std___cycFILE*_tmp40;struct
_tuple10*_tmp3C=params;_tmp3D=*_tmp3C;_tmp3E=_tmp3D.f1;_tmp3F=_tmp3D.f2;_tmp40=
_tmp3D.f3;{struct Cyc_Interface_I*_tmp41=Cyc_Interface_extract(_tmp3E->ae);if(
_tmp3F == 0)Cyc_Interface_save(_tmp41,_tmp40);else{struct Cyc_Interface_I*_tmp42=
Cyc_Interface_parse((struct Cyc_Std___cycFILE*)_check_null(_tmp3F));if(!Cyc_Interface_is_subinterface(
_tmp42,_tmp41,({struct _tuple9*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->f1=
_tag_arr("written interface",sizeof(char),18);_tmp43->f2=_tag_arr("maximal interface",
sizeof(char),18);_tmp43;})))Cyc_compile_failure=1;else{Cyc_Interface_save(_tmp42,
_tmp40);}}return tds;}}struct Cyc_List_List*Cyc_do_translate(int ignore,struct Cyc_List_List*
tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*
tds){return Cyc_Tovc_tovc(tds);}static char _tmp44[1]="";static struct _tagged_arr Cyc_cyc_include={
_tmp44,_tmp44,_tmp44 + 1};struct Cyc_List_List*Cyc_do_print(struct Cyc_Std___cycFILE*
out_file,struct Cyc_List_List*tds){struct Cyc_Absynpp_Params params_r;if(Cyc_tc_r)
params_r=Cyc_Absynpp_cyc_params_r;else{params_r=Cyc_Absynpp_c_params_r;}params_r.expand_typedefs=
!Cyc_noexpand_r;params_r.to_VC=Cyc_tovc_r;params_r.add_cyc_prefix=Cyc_add_cyc_namespace_r;
params_r.generate_line_directives=Cyc_generate_line_directives_r;params_r.print_full_evars=
Cyc_print_full_evars_r;params_r.print_all_tvars=Cyc_print_all_tvars_r;params_r.print_all_kinds=
Cyc_print_all_kinds_r;{struct Cyc_Std___cycFILE*_tmp45=Cyc_try_file_open(Cyc_cyc_include,
_tag_arr("r",sizeof(char),2),_tag_arr("internal compiler file",sizeof(char),23));
if(_tmp45 == 0)return tds;{struct Cyc_Std___cycFILE*_tmp46=(struct Cyc_Std___cycFILE*)
_check_null(_tmp45);unsigned int n_read=1024;unsigned int n_written;char buf[1024];{
unsigned int _tmp49=1024;unsigned int i;for(i=0;i < _tmp49;i ++){buf[i]='\000';}}
while(n_read == 1024){n_read=Cyc_Std_fread(_tag_arr(buf,sizeof(char),1024),1,1024,
_tmp46);if(n_read != 1024?!Cyc_Std_feof(_tmp46): 0){Cyc_compile_failure=1;({void*
_tmp47[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\nError: problem copying cyc_include.h\n",
sizeof(char),39),_tag_arr(_tmp47,sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)
Cyc_Std_stderr);return tds;}n_written=Cyc_Std_fwrite(_tag_arr(buf,sizeof(char),
1024),1,n_read,out_file);if(n_read != n_written){Cyc_compile_failure=1;({void*
_tmp48[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\nError: problem copying cyc_include.h\n",
sizeof(char),39),_tag_arr(_tmp48,sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)
Cyc_Std_stderr);return tds;}}Cyc_Std_fclose(_tmp46);if(Cyc_pp_r){Cyc_Absynpp_set_params(&
params_r);Cyc_Absynpp_decllist2file(tds,out_file);}else{Cyc_Absyndump_set_params(&
params_r);Cyc_Absyndump_dumpdecllist2file(tds,out_file);}Cyc_Std_fflush((struct
Cyc_Std___cycFILE*)out_file);return tds;}}}static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles(){if(!Cyc_save_c_r)for(0;Cyc_cfiles != 0;Cyc_cfiles=((
struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){Cyc_remove_file(*((struct
_tagged_arr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}static
struct Cyc_List_List*Cyc_split_by_char(struct _tagged_arr s,char c){if(s.curr == ((
struct _tagged_arr)_tag_arr(0,0,0)).curr)return 0;{struct Cyc_List_List*_tmp4A=0;
unsigned int _tmp4B=Cyc_Std_strlen(s);while(_tmp4B > 0){struct _tagged_arr _tmp4C=
Cyc_Std_strchr(s,c);if(_tmp4C.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr){
_tmp4A=({struct Cyc_List_List*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->hd=({
struct _tagged_arr*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=s;_tmp4E;});_tmp4D->tl=
_tmp4A;_tmp4D;});break;}else{_tmp4A=({struct Cyc_List_List*_tmp4F=_cycalloc(
sizeof(*_tmp4F));_tmp4F->hd=({struct _tagged_arr*_tmp50=_cycalloc(sizeof(*_tmp50));
_tmp50[0]=(struct _tagged_arr)Cyc_Std_substring(s,0,(unsigned int)((((struct
_tagged_arr)_tmp4C).curr - s.curr)/ sizeof(char)));_tmp50;});_tmp4F->tl=_tmp4A;
_tmp4F;});_tmp4B -=(((struct _tagged_arr)_tmp4C).curr - s.curr)/ sizeof(char);s=
_tagged_arr_plus(_tmp4C,sizeof(char),1);}}return((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4A);}}static int Cyc_file_exists(struct
_tagged_arr file){struct Cyc_Std___cycFILE*f=0;{struct _handler_cons _tmp51;
_push_handler(& _tmp51);{int _tmp53=0;if(setjmp(_tmp51.handler))_tmp53=1;if(!
_tmp53){f=(struct Cyc_Std___cycFILE*)Cyc_Std_file_open(file,_tag_arr("r",sizeof(
char),2));;_pop_handler();}else{void*_tmp52=(void*)_exn_thrown;void*_tmp55=
_tmp52;_LL13:;_LL14: goto _LL12;_LL15:;_LL16:(void)_throw(_tmp55);_LL12:;}}}if(f == 
0)return 0;else{Cyc_Std_fclose((struct Cyc_Std___cycFILE*)_check_null(f));return 1;}}
static struct _tagged_arr Cyc_sprint_list(struct Cyc_List_List*dirs){struct
_tagged_arr tmp=_tag_arr("",sizeof(char),1);for(0;dirs != 0;dirs=dirs->tl){struct
_tagged_arr _tmp56=*((struct _tagged_arr*)dirs->hd);if(_tmp56.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?1: Cyc_Std_strlen(_tmp56)== 0)continue;_tmp56=(
struct _tagged_arr)Cyc_Std_strconcat(_tmp56,_tag_arr(":",sizeof(char),2));tmp=(
struct _tagged_arr)Cyc_Std_strconcat(_tmp56,tmp);}return tmp;}static struct
_tagged_arr*Cyc_find(struct Cyc_List_List*dirs,struct _tagged_arr file){if(file.curr
== ((struct _tagged_arr)_tag_arr(0,0,0)).curr)return 0;for(0;dirs != 0;dirs=dirs->tl){
struct _tagged_arr _tmp57=*((struct _tagged_arr*)dirs->hd);if(_tmp57.curr == ((
struct _tagged_arr)_tag_arr(0,0,0)).curr?1: Cyc_Std_strlen(_tmp57)== 0)continue;{
struct _tagged_arr s=(struct _tagged_arr)Cyc_Filename_concat(_tmp57,file);if(Cyc_file_exists(
s))return({struct _tagged_arr*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58[0]=s;
_tmp58;});}}return 0;}static struct _tagged_arr Cyc_do_find(struct Cyc_List_List*dirs,
struct _tagged_arr file){struct _tagged_arr*_tmp59=Cyc_find(dirs,file);if(_tmp59 == 
0){Cyc_compile_failure=1;Cyc_remove_cfiles();(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A[0]=({struct Cyc_Core_Failure_struct
_tmp5B;_tmp5B.tag=Cyc_Core_Failure;_tmp5B.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp5E;_tmp5E.tag=0;_tmp5E.f1=(struct _tagged_arr)Cyc_sprint_list(dirs);{struct
Cyc_Std_String_pa_struct _tmp5D;_tmp5D.tag=0;_tmp5D.f1=(struct _tagged_arr)file;{
void*_tmp5C[2]={& _tmp5D,& _tmp5E};Cyc_Std_aprintf(_tag_arr("Error: can't find internal compiler file %s in path %s\n",
sizeof(char),56),_tag_arr(_tmp5C,sizeof(void*),2));}}});_tmp5B;});_tmp5A;}));}
return*_tmp59;}static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,
struct _tagged_arr subdir){struct Cyc_List_List*_tmp5F=0;for(0;dirs != 0;dirs=dirs->tl){
_tmp5F=({struct Cyc_List_List*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->hd=({
struct _tagged_arr*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61[0]=(struct _tagged_arr)
Cyc_Filename_concat(*((struct _tagged_arr*)dirs->hd),subdir);_tmp61;});_tmp60->tl=
_tmp5F;_tmp60;});}_tmp5F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp5F);return _tmp5F;}static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*
dirs,struct _tagged_arr subdir){struct Cyc_List_List*_tmp62=0;for(0;dirs != 0;dirs=
dirs->tl){_tmp62=({struct Cyc_List_List*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->hd=(
struct _tagged_arr*)dirs->hd;_tmp63->tl=_tmp62;_tmp63;});_tmp62=({struct Cyc_List_List*
_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->hd=({struct _tagged_arr*_tmp65=
_cycalloc(sizeof(*_tmp65));_tmp65[0]=(struct _tagged_arr)Cyc_Filename_concat(*((
struct _tagged_arr*)dirs->hd),subdir);_tmp65;});_tmp64->tl=_tmp62;_tmp64;});}
_tmp62=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp62);
return _tmp62;}static int Cyc_is_other_special(char c){switch(c){case '\\': _LL17: goto
_LL18;case '"': _LL18: goto _LL19;case ';': _LL19: goto _LL1A;case '&': _LL1A: goto _LL1B;
case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto _LL1D;case '|': _LL1D: goto _LL1E;case '^':
_LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;case '>': _LL20: goto _LL21;case ' ': _LL21:
goto _LL22;case '\n': _LL22: goto _LL23;case '\t': _LL23: return 1;default: _LL24: return 0;}}
static struct _tagged_arr Cyc_sh_escape_string(struct _tagged_arr s){unsigned int
_tmp66=Cyc_Std_strlen(s);int _tmp67=0;int _tmp68=0;{int i=0;for(0;i < _tmp66;i ++){
char _tmp69=*((const char*)_check_unknown_subscript(s,sizeof(char),i));if(_tmp69 == '\'')
_tmp67 ++;else{if(Cyc_is_other_special(_tmp69))_tmp68 ++;}}}if(_tmp67 == 0?_tmp68 == 
0: 0)return s;if(_tmp67 == 0)return(struct _tagged_arr)Cyc_Std_strconcat_l(({struct
_tagged_arr*_tmp6A[3];_tmp6A[2]=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"'",
sizeof(char),2);_tmp6A[1]=({struct _tagged_arr*_tmp6C=_cycalloc(sizeof(*_tmp6C));
_tmp6C[0]=(struct _tagged_arr)s;_tmp6C;});_tmp6A[0]=_init_tag_arr(_cycalloc(
sizeof(struct _tagged_arr)),"'",sizeof(char),2);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp6A,sizeof(struct _tagged_arr*),3));}));{
unsigned int _tmp6E=(_tmp66 + _tmp67)+ _tmp68;struct _tagged_arr _tmp6F=({
unsigned int _tmp73=_tmp6E + 1;char*_tmp74=(char*)_cycalloc_atomic(_check_times(
sizeof(char),_tmp73));struct _tagged_arr _tmp76=_tag_arr(_tmp74,sizeof(char),
_tmp6E + 1);{unsigned int _tmp75=_tmp73;unsigned int i;for(i=0;i < _tmp75;i ++){
_tmp74[i]='\000';}}_tmp76;});int _tmp70=0;int _tmp71=0;for(0;_tmp70 < _tmp66;_tmp70
++){char _tmp72=*((const char*)_check_unknown_subscript(s,sizeof(char),_tmp70));
if(_tmp72 == '\''?1: Cyc_is_other_special(_tmp72))*((char*)
_check_unknown_subscript(_tmp6F,sizeof(char),_tmp71 ++))='\\';*((char*)
_check_unknown_subscript(_tmp6F,sizeof(char),_tmp71 ++))=_tmp72;}return(struct
_tagged_arr)_tmp6F;}}static struct _tagged_arr*Cyc_sh_escape_stringptr(struct
_tagged_arr*sp){return({struct _tagged_arr*_tmp77=_cycalloc(sizeof(*_tmp77));
_tmp77[0]=Cyc_sh_escape_string(*sp);_tmp77;});}static void Cyc_process_file(struct
_tagged_arr filename){struct _tagged_arr _tmp78=Cyc_make_base_filename(filename,Cyc_output_file);
struct _tagged_arr _tmp79=Cyc_Std_strconcat(_tmp78,_tag_arr(".cyp",sizeof(char),5));
struct _tagged_arr _tmp7A=Cyc_Std_strconcat(_tmp78,_tag_arr(".cyci",sizeof(char),6));
struct _tagged_arr _tmp7B=Cyc_Std_strconcat(_tmp78,_tag_arr(".cycio",sizeof(char),
7));struct _tagged_arr _tmp7C=Cyc_Std_strconcat(_tmp78,_tag_arr(".c",sizeof(char),
3));if(Cyc_v_r)({struct Cyc_Std_String_pa_struct _tmp7E;_tmp7E.tag=0;_tmp7E.f1=(
struct _tagged_arr)filename;{void*_tmp7D[1]={& _tmp7E};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Compiling %s\n",sizeof(char),14),_tag_arr(_tmp7D,sizeof(void*),1));}});{
struct Cyc_Std___cycFILE*f0=Cyc_try_file_open(filename,_tag_arr("r",sizeof(char),
2),_tag_arr("input file",sizeof(char),11));if(Cyc_compile_failure)return;Cyc_Std_fclose((
struct Cyc_Std___cycFILE*)_check_null(f0));{struct _tagged_arr _tmp7F=Cyc_Std_str_sepstr(({
struct Cyc_List_List*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->hd=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmpA7->tl=((struct Cyc_List_List*(*)(
struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cppargs));_tmpA7;}),_tag_arr(" ",sizeof(char),2));struct _tagged_arr
def_inc_path=(struct _tagged_arr)Cstring_to_string(Cdef_inc_path);struct Cyc_List_List*
_tmp80=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);_tmp80=Cyc_add_subdir(
_tmp80,_tag_arr("include",sizeof(char),8));if(Cyc_Std_strlen(def_inc_path)> 0)
_tmp80=({struct Cyc_List_List*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->hd=({
struct _tagged_arr*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82[0]=def_inc_path;
_tmp82;});_tmp81->tl=_tmp80;_tmp81;});{struct _tagged_arr _tmp83=Cyc_Std_getenv(
_tag_arr("CYCLONE_INCLUDE_PATH",sizeof(char),21));if(_tmp83.curr != (_tag_arr(0,0,
0)).curr)_tmp80=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(Cyc_split_by_char((struct _tagged_arr)_tmp83,':'),_tmp80);{
struct _tagged_arr stdinc_string=(struct _tagged_arr)Cyc_Std_str_sepstr(({struct Cyc_List_List*
_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->hd=_init_tag_arr(_cycalloc(sizeof(
struct _tagged_arr)),"",sizeof(char),1);_tmpA5->tl=((struct Cyc_List_List*(*)(
struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmp80);_tmpA5;}),_tag_arr(" -I",sizeof(char),4));struct
_tagged_arr ofile_string;if(Cyc_stop_after_cpp_r){if(Cyc_output_file != 0)
ofile_string=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp85;_tmp85.tag=
0;_tmp85.f1=(struct _tagged_arr)*((struct _tagged_arr*)_check_null(Cyc_output_file));{
void*_tmp84[1]={& _tmp85};Cyc_Std_aprintf(_tag_arr(" > %s",sizeof(char),6),
_tag_arr(_tmp84,sizeof(void*),1));}});else{ofile_string=_tag_arr("",sizeof(char),
1);}}else{ofile_string=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp87;
_tmp87.tag=0;_tmp87.f1=(struct _tagged_arr)Cyc_sh_escape_string((struct
_tagged_arr)_tmp79);{void*_tmp86[1]={& _tmp87};Cyc_Std_aprintf(_tag_arr(" > %s",
sizeof(char),6),_tag_arr(_tmp86,sizeof(void*),1));}});}{struct _tagged_arr
fixup_string;if(Cyc_produce_dependencies){if(Cyc_dependencies_target == 0)
fixup_string=_tag_arr(" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",sizeof(char),35);
else{fixup_string=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp89;
_tmp89.tag=0;_tmp89.f1=(struct _tagged_arr)*((struct _tagged_arr*)_check_null(Cyc_dependencies_target));{
void*_tmp88[1]={& _tmp89};Cyc_Std_aprintf(_tag_arr(" | sed 's/^.*\\.cyc\\.o:/%s:/'",
sizeof(char),29),_tag_arr(_tmp88,sizeof(void*),1));}});}}else{fixup_string=
_tag_arr("",sizeof(char),1);}{struct _tagged_arr _tmp8A=({struct Cyc_Std_String_pa_struct
_tmpA4;_tmpA4.tag=0;_tmpA4.f1=(struct _tagged_arr)ofile_string;{struct Cyc_Std_String_pa_struct
_tmpA3;_tmpA3.tag=0;_tmpA3.f1=(struct _tagged_arr)fixup_string;{struct Cyc_Std_String_pa_struct
_tmpA2;_tmpA2.tag=0;_tmpA2.f1=(struct _tagged_arr)Cyc_sh_escape_string(filename);{
struct Cyc_Std_String_pa_struct _tmpA1;_tmpA1.tag=0;_tmpA1.f1=(struct _tagged_arr)
stdinc_string;{struct Cyc_Std_String_pa_struct _tmpA0;_tmpA0.tag=0;_tmpA0.f1=(
struct _tagged_arr)_tmp7F;{struct Cyc_Std_String_pa_struct _tmp9F;_tmp9F.tag=0;
_tmp9F.f1=(struct _tagged_arr)Cyc_cpp;{void*_tmp9E[6]={& _tmp9F,& _tmpA0,& _tmpA1,&
_tmpA2,& _tmpA3,& _tmpA4};Cyc_Std_aprintf(_tag_arr("%s %s%s %s%s%s",sizeof(char),
15),_tag_arr(_tmp9E,sizeof(void*),6));}}}}}}});if(Cyc_v_r)({struct Cyc_Std_String_pa_struct
_tmp8C;_tmp8C.tag=0;_tmp8C.f1=(struct _tagged_arr)_tmp8A;{void*_tmp8B[1]={& _tmp8C};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s\n",sizeof(char),4),_tag_arr(_tmp8B,
sizeof(void*),1));}});if(Cyc_Std_system((struct _tagged_arr)_tmp8A)!= 0){Cyc_compile_failure=
1;({void*_tmp8D[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\nError: preprocessing\n",
sizeof(char),23),_tag_arr(_tmp8D,sizeof(void*),0));});return;}if(Cyc_stop_after_cpp_r)
return;Cyc_Position_reset_position((struct _tagged_arr)_tmp79);{struct Cyc_Std___cycFILE*
in_file=Cyc_try_file_open((struct _tagged_arr)_tmp79,_tag_arr("r",sizeof(char),2),
_tag_arr("file",sizeof(char),5));if(Cyc_compile_failure)return;{struct Cyc_List_List*
tds=0;{struct _handler_cons _tmp8E;_push_handler(& _tmp8E);{int _tmp90=0;if(setjmp(
_tmp8E.handler))_tmp90=1;if(!_tmp90){tds=((struct Cyc_List_List*(*)(struct
_tagged_arr stage_name,struct Cyc_List_List*(*f)(struct Cyc_Std___cycFILE*,struct
Cyc_List_List*),struct Cyc_Std___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(
_tag_arr("parsing",sizeof(char),8),Cyc_do_parse,(struct Cyc_Std___cycFILE*)
_check_null(in_file),tds);;_pop_handler();}else{void*_tmp8F=(void*)_exn_thrown;
void*_tmp92=_tmp8F;_LL27:;_LL28: Cyc_Std_file_close((struct Cyc_Std___cycFILE*)
_check_null(in_file));Cyc_remove_file((struct _tagged_arr)_tmp79);(int)_throw(
_tmp92);_LL29:;_LL2A:(void)_throw(_tmp92);_LL26:;}}}Cyc_Std_file_close((struct
Cyc_Std___cycFILE*)_check_null(in_file));if(Cyc_compile_failure){Cyc_remove_file((
struct _tagged_arr)_tmp79);return;}{struct Cyc_Tcenv_Tenv*_tmp93=Cyc_Tcenv_tc_init();
if(Cyc_parseonly_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(struct _tagged_arr
stage_name,struct Cyc_List_List*(*f)(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),
struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*tds))Cyc_do_stage)(_tag_arr("type checking",
sizeof(char),14),Cyc_do_typecheck,_tmp93,tds);if(Cyc_compile_failure){Cyc_remove_file((
struct _tagged_arr)_tmp79);return;}tds=((struct Cyc_List_List*(*)(struct
_tagged_arr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,
struct Cyc_List_List*tds))Cyc_do_stage)(_tag_arr("control-flow checking",sizeof(
char),22),Cyc_do_cfcheck,1,tds);if(Cyc_compile_failure){Cyc_remove_file((struct
_tagged_arr)_tmp79);return;}if(Cyc_ic_r){struct Cyc_Std___cycFILE*inter_file=Cyc_Std_fopen((
struct _tagged_arr)_tmp7A,_tag_arr("r",sizeof(char),2));struct Cyc_Std___cycFILE*
inter_objfile=Cyc_try_file_open((struct _tagged_arr)_tmp7B,_tag_arr("w",sizeof(
char),2),_tag_arr("interface object file",sizeof(char),22));if(inter_objfile == 0){
Cyc_compile_failure=1;return;}Cyc_Position_reset_position((struct _tagged_arr)
_tmp7A);tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*
f)(struct _tuple10*,struct Cyc_List_List*),struct _tuple10*env,struct Cyc_List_List*
tds))Cyc_do_stage)(_tag_arr("interface checking",sizeof(char),19),Cyc_do_interface,({
struct _tuple10*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->f1=_tmp93;_tmp94->f2=
inter_file;_tmp94->f3=(struct Cyc_Std___cycFILE*)_check_null(inter_objfile);
_tmp94;}),tds);if(inter_file != 0)Cyc_Std_file_close((struct Cyc_Std___cycFILE*)
_check_null(inter_file));Cyc_Std_file_close((struct Cyc_Std___cycFILE*)
_check_null(inter_objfile));}if(Cyc_tc_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(
struct _tagged_arr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),
int env,struct Cyc_List_List*tds))Cyc_do_stage)(_tag_arr("translation to C",
sizeof(char),17),Cyc_do_translate,1,tds);if(Cyc_compile_failure){Cyc_remove_file((
struct _tagged_arr)_tmp79);return;}if(!Cyc_compile_failure)Cyc_Tcutil_flush_warnings();
Cyc_remove_file((struct _tagged_arr)_tmp79);if(Cyc_compile_failure)return;if(Cyc_tovc_r)
tds=((struct Cyc_List_List*(*)(struct _tagged_arr stage_name,struct Cyc_List_List*(*
f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(
_tag_arr("post-pass to VC",sizeof(char),16),Cyc_do_tovc,1,tds);if(Cyc_compile_failure)
return;PRINTC: {struct Cyc_Std___cycFILE*out_file;if(Cyc_parseonly_r?1: Cyc_tc_r){
if(Cyc_output_file != 0)out_file=Cyc_try_file_open(*((struct _tagged_arr*)
_check_null(Cyc_output_file)),_tag_arr("w",sizeof(char),2),_tag_arr("output file",
sizeof(char),12));else{out_file=(struct Cyc_Std___cycFILE*)Cyc_Std_stdout;}}else{
if(Cyc_toc_r?Cyc_output_file != 0: 0)out_file=Cyc_try_file_open(*((struct
_tagged_arr*)_check_null(Cyc_output_file)),_tag_arr("w",sizeof(char),2),_tag_arr("output file",
sizeof(char),12));else{out_file=Cyc_try_file_open((struct _tagged_arr)_tmp7C,
_tag_arr("w",sizeof(char),2),_tag_arr("output file",sizeof(char),12));}}if(Cyc_compile_failure)
return;{struct _handler_cons _tmp95;_push_handler(& _tmp95);{int _tmp97=0;if(setjmp(
_tmp95.handler))_tmp97=1;if(!_tmp97){tds=((struct Cyc_List_List*(*)(struct
_tagged_arr stage_name,struct Cyc_List_List*(*f)(struct Cyc_Std___cycFILE*,struct
Cyc_List_List*),struct Cyc_Std___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(
_tag_arr("printing",sizeof(char),9),Cyc_do_print,(struct Cyc_Std___cycFILE*)
_check_null(out_file),tds);;_pop_handler();}else{void*_tmp96=(void*)_exn_thrown;
void*_tmp99=_tmp96;_LL2C:;_LL2D: Cyc_compile_failure=1;Cyc_Std_file_close((struct
Cyc_Std___cycFILE*)_check_null(out_file));Cyc_cfiles=({struct Cyc_List_List*
_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->hd=({struct _tagged_arr*_tmp9B=
_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=(struct _tagged_arr)_tmp7C;_tmp9B;});_tmp9A->tl=
Cyc_cfiles;_tmp9A;});(int)_throw(_tmp99);_LL2E:;_LL2F:(void)_throw(_tmp99);_LL2B:;}}}
Cyc_Std_file_close((struct Cyc_Std___cycFILE*)_check_null(out_file));Cyc_cfiles=({
struct Cyc_List_List*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->hd=({struct
_tagged_arr*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D[0]=(struct _tagged_arr)
_tmp7C;_tmp9D;});_tmp9C->tl=Cyc_cfiles;_tmp9C;});}}}}}}}}}}}static char _tmpA9[8]="<final>";
static struct _tagged_arr Cyc_final_str={_tmpA9,_tmpA9,_tmpA9 + 8};static struct
_tagged_arr*Cyc_final_strptr=& Cyc_final_str;static struct Cyc_Interface_I*Cyc_read_cycio(
struct _tagged_arr*n){if(n == (struct _tagged_arr*)Cyc_final_strptr)return Cyc_Interface_final();{
struct _tagged_arr basename;{struct _handler_cons _tmpAA;_push_handler(& _tmpAA);{int
_tmpAC=0;if(setjmp(_tmpAA.handler))_tmpAC=1;if(!_tmpAC){basename=(struct
_tagged_arr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmpAB=(
void*)_exn_thrown;void*_tmpAE=_tmpAB;_LL31: if(*((void**)_tmpAE)!= Cyc_Core_Invalid_argument)
goto _LL33;_LL32: basename=*n;goto _LL30;_LL33:;_LL34:(void)_throw(_tmpAE);_LL30:;}}}{
struct _tagged_arr _tmpAF=Cyc_Std_strconcat(basename,_tag_arr(".cycio",sizeof(char),
7));struct Cyc_Std___cycFILE*_tmpB0=Cyc_try_file_open((struct _tagged_arr)_tmpAF,
_tag_arr("rb",sizeof(char),3),_tag_arr("interface object file",sizeof(char),22));
if(_tmpB0 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();exit(1);}Cyc_Position_reset_position((
struct _tagged_arr)_tmpAF);{struct Cyc_Interface_I*_tmpB1=Cyc_Interface_load((
struct Cyc_Std___cycFILE*)_check_null(_tmpB0));Cyc_Std_file_close((struct Cyc_Std___cycFILE*)
_check_null(_tmpB0));return _tmpB1;}}}}static int Cyc_is_cfile(struct _tagged_arr*n){
return*((const char*)_check_unknown_subscript(*n,sizeof(char),0))!= '-';}void
GC_blacklist_warn_clear();struct _tuple11{struct _tagged_arr f1;int f2;struct
_tagged_arr f3;void*f4;struct _tagged_arr f5;};int Cyc_main(int argc,struct
_tagged_arr argv){GC_blacklist_warn_clear();{struct _tagged_arr comp=(struct
_tagged_arr)Cstring_to_string(Ccomp);Cyc_set_cpp((struct _tagged_arr)Cyc_Std_strconcat(
comp,_tag_arr(" -x c -E -U__GNUC__ -nostdinc",sizeof(char),30)));{struct Cyc_List_List*
options=({struct _tuple11*_tmpED[50];_tmpED[49]=({struct _tuple11*_tmp181=
_cycalloc(sizeof(*_tmp181));_tmp181->f1=_tag_arr("-noregister",sizeof(char),12);
_tmp181->f2=0;_tmp181->f3=_tag_arr("",sizeof(char),1);_tmp181->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182[0]=({
struct Cyc_Arg_Set_spec_struct _tmp183;_tmp183.tag=3;_tmp183.f1=& Cyc_Parse_no_register;
_tmp183;});_tmp182;});_tmp181->f5=_tag_arr("Treat register storage class as public",
sizeof(char),39);_tmp181;});_tmpED[48]=({struct _tuple11*_tmp17E=_cycalloc(
sizeof(*_tmp17E));_tmp17E->f1=_tag_arr("-warnboundschecks",sizeof(char),18);
_tmp17E->f2=0;_tmp17E->f3=_tag_arr("",sizeof(char),1);_tmp17E->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F[0]=({
struct Cyc_Arg_Set_spec_struct _tmp180;_tmp180.tag=3;_tmp180.f1=& Cyc_Toc_warn_bounds_checks;
_tmp180;});_tmp17F;});_tmp17E->f5=_tag_arr("Warn when bounds checks can't be eliminated",
sizeof(char),44);_tmp17E;});_tmpED[47]=({struct _tuple11*_tmp17B=_cycalloc(
sizeof(*_tmp17B));_tmp17B->f1=_tag_arr("-printfullevars",sizeof(char),16);
_tmp17B->f2=0;_tmp17B->f3=_tag_arr("",sizeof(char),1);_tmp17B->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C[0]=({
struct Cyc_Arg_Set_spec_struct _tmp17D;_tmp17D.tag=3;_tmp17D.f1=& Cyc_print_full_evars_r;
_tmp17D;});_tmp17C;});_tmp17B->f5=_tag_arr("Print full information for evars (type debugging)",
sizeof(char),50);_tmp17B;});_tmpED[46]=({struct _tuple11*_tmp178=_cycalloc(
sizeof(*_tmp178));_tmp178->f1=_tag_arr("-printallkinds",sizeof(char),15);_tmp178->f2=
0;_tmp178->f3=_tag_arr("",sizeof(char),1);_tmp178->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179[0]=({struct Cyc_Arg_Set_spec_struct
_tmp17A;_tmp17A.tag=3;_tmp17A.f1=& Cyc_print_all_kinds_r;_tmp17A;});_tmp179;});
_tmp178->f5=_tag_arr("Always print kinds of type variables",sizeof(char),37);
_tmp178;});_tmpED[45]=({struct _tuple11*_tmp175=_cycalloc(sizeof(*_tmp175));
_tmp175->f1=_tag_arr("-printalltvars",sizeof(char),15);_tmp175->f2=0;_tmp175->f3=
_tag_arr("",sizeof(char),1);_tmp175->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176[0]=({struct Cyc_Arg_Set_spec_struct
_tmp177;_tmp177.tag=3;_tmp177.f1=& Cyc_print_all_tvars_r;_tmp177;});_tmp176;});
_tmp175->f5=_tag_arr("Print all type variables (even implicit default effects)",
sizeof(char),57);_tmp175;});_tmpED[44]=({struct _tuple11*_tmp172=_cycalloc(
sizeof(*_tmp172));_tmp172->f1=_tag_arr("-noexpandtypedefs",sizeof(char),18);
_tmp172->f2=0;_tmp172->f3=_tag_arr("",sizeof(char),1);_tmp172->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp173=_cycalloc(sizeof(*_tmp173));_tmp173[0]=({
struct Cyc_Arg_Set_spec_struct _tmp174;_tmp174.tag=3;_tmp174.f1=& Cyc_noexpand_r;
_tmp174;});_tmp173;});_tmp172->f5=_tag_arr("Don't expand typedefs in pretty printing",
sizeof(char),41);_tmp172;});_tmpED[43]=({struct _tuple11*_tmp16F=_cycalloc(
sizeof(*_tmp16F));_tmp16F->f1=_tag_arr("-noremoveunused",sizeof(char),16);
_tmp16F->f2=0;_tmp16F->f3=_tag_arr("",sizeof(char),1);_tmp16F->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170[0]=({
struct Cyc_Arg_Set_spec_struct _tmp171;_tmp171.tag=3;_tmp171.f1=& Cyc_noshake_r;
_tmp171;});_tmp170;});_tmp16F->f5=_tag_arr("Don't remove externed variables that aren't used",
sizeof(char),49);_tmp16F;});_tmpED[42]=({struct _tuple11*_tmp16C=_cycalloc(
sizeof(*_tmp16C));_tmp16C->f1=_tag_arr("-nogc",sizeof(char),6);_tmp16C->f2=0;
_tmp16C->f3=_tag_arr("",sizeof(char),1);_tmp16C->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D[0]=({struct Cyc_Arg_Set_spec_struct
_tmp16E;_tmp16E.tag=3;_tmp16E.f1=& Cyc_nogc_r;_tmp16E;});_tmp16D;});_tmp16C->f5=
_tag_arr("Don't link in the garbage collector",sizeof(char),36);_tmp16C;});
_tmpED[41]=({struct _tuple11*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->f1=
_tag_arr("-nocyc",sizeof(char),7);_tmp169->f2=0;_tmp169->f3=_tag_arr("",sizeof(
char),1);_tmp169->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp16A=_cycalloc(
sizeof(*_tmp16A));_tmp16A[0]=({struct Cyc_Arg_Unit_spec_struct _tmp16B;_tmp16B.tag=
0;_tmp16B.f1=Cyc_set_nocyc;_tmp16B;});_tmp16A;});_tmp169->f5=_tag_arr("Don't add implicit namespace Cyc",
sizeof(char),33);_tmp169;});_tmpED[40]=({struct _tuple11*_tmp166=_cycalloc(
sizeof(*_tmp166));_tmp166->f1=_tag_arr("-no-cpp-precomp",sizeof(char),16);
_tmp166->f2=0;_tmp166->f3=_tag_arr("",sizeof(char),1);_tmp166->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp168;_tmp168.tag=0;_tmp168.f1=Cyc_set_nocppprecomp;
_tmp168;});_tmp167;});_tmp166->f5=_tag_arr("Don't do smart preprocessing (mac only)",
sizeof(char),40);_tmp166;});_tmpED[39]=({struct _tuple11*_tmp163=_cycalloc(
sizeof(*_tmp163));_tmp163->f1=_tag_arr("-use-cpp",sizeof(char),9);_tmp163->f2=0;
_tmp163->f3=_tag_arr("<path>",sizeof(char),7);_tmp163->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164[0]=({struct Cyc_Arg_String_spec_struct
_tmp165;_tmp165.tag=5;_tmp165.f1=Cyc_set_cpp;_tmp165;});_tmp164;});_tmp163->f5=
_tag_arr("Indicate which preprocessor to use",sizeof(char),35);_tmp163;});_tmpED[
38]=({struct _tuple11*_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160->f1=_tag_arr("--noboundschecks",
sizeof(char),17);_tmp160->f2=0;_tmp160->f3=_tag_arr("",sizeof(char),1);_tmp160->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp161=_cycalloc(sizeof(*_tmp161));
_tmp161[0]=({struct Cyc_Arg_Unit_spec_struct _tmp162;_tmp162.tag=0;_tmp162.f1=Cyc_set_noboundschecks;
_tmp162;});_tmp161;});_tmp160->f5=_tag_arr("Disable bounds checks",sizeof(char),
22);_tmp160;});_tmpED[37]=({struct _tuple11*_tmp15D=_cycalloc(sizeof(*_tmp15D));
_tmp15D->f1=_tag_arr("--nonullchecks",sizeof(char),15);_tmp15D->f2=0;_tmp15D->f3=
_tag_arr("",sizeof(char),1);_tmp15D->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp15F;_tmp15F.tag=0;_tmp15F.f1=Cyc_set_nonullchecks;_tmp15F;});_tmp15E;});
_tmp15D->f5=_tag_arr("Disable null checks",sizeof(char),20);_tmp15D;});_tmpED[36]=({
struct _tuple11*_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A->f1=_tag_arr("--nochecks",
sizeof(char),11);_tmp15A->f2=0;_tmp15A->f3=_tag_arr("",sizeof(char),1);_tmp15A->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp15B=_cycalloc(sizeof(*_tmp15B));
_tmp15B[0]=({struct Cyc_Arg_Unit_spec_struct _tmp15C;_tmp15C.tag=0;_tmp15C.f1=Cyc_set_nochecks;
_tmp15C;});_tmp15B;});_tmp15A->f5=_tag_arr("Disable bounds/null checks",sizeof(
char),27);_tmp15A;});_tmpED[35]=({struct _tuple11*_tmp157=_cycalloc(sizeof(*
_tmp157));_tmp157->f1=_tag_arr("--lineno",sizeof(char),9);_tmp157->f2=0;_tmp157->f3=
_tag_arr("",sizeof(char),1);_tmp157->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp159;_tmp159.tag=0;_tmp159.f1=Cyc_set_lineno;_tmp159;});_tmp158;});_tmp157->f5=
_tag_arr("Generate line numbers for debugging",sizeof(char),36);_tmp157;});
_tmpED[34]=({struct _tuple11*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->f1=
_tag_arr("-save-c",sizeof(char),8);_tmp154->f2=0;_tmp154->f3=_tag_arr("",sizeof(
char),1);_tmp154->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp155=_cycalloc(
sizeof(*_tmp155));_tmp155[0]=({struct Cyc_Arg_Set_spec_struct _tmp156;_tmp156.tag=
3;_tmp156.f1=& Cyc_save_c_r;_tmp156;});_tmp155;});_tmp154->f5=_tag_arr("Don't delete temporary C files",
sizeof(char),31);_tmp154;});_tmpED[33]=({struct _tuple11*_tmp151=_cycalloc(
sizeof(*_tmp151));_tmp151->f1=_tag_arr("-save-temps",sizeof(char),12);_tmp151->f2=
0;_tmp151->f3=_tag_arr("",sizeof(char),1);_tmp151->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp153;_tmp153.tag=0;_tmp153.f1=Cyc_set_save_temps;_tmp153;});_tmp152;});
_tmp151->f5=_tag_arr("Don't delete temporary files",sizeof(char),29);_tmp151;});
_tmpED[32]=({struct _tuple11*_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E->f1=
_tag_arr("-tovc",sizeof(char),6);_tmp14E->f2=0;_tmp14E->f3=_tag_arr("",sizeof(
char),1);_tmp14E->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp14F=_cycalloc(
sizeof(*_tmp14F));_tmp14F[0]=({struct Cyc_Arg_Set_spec_struct _tmp150;_tmp150.tag=
3;_tmp150.f1=& Cyc_tovc_r;_tmp150;});_tmp14F;});_tmp14E->f5=_tag_arr("Avoid gcc extensions in C output",
sizeof(char),33);_tmp14E;});_tmpED[31]=({struct _tuple11*_tmp14B=_cycalloc(
sizeof(*_tmp14B));_tmp14B->f1=_tag_arr("-up",sizeof(char),4);_tmp14B->f2=0;
_tmp14B->f3=_tag_arr("",sizeof(char),1);_tmp14B->f4=(void*)({struct Cyc_Arg_Clear_spec_struct*
_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C[0]=({struct Cyc_Arg_Clear_spec_struct
_tmp14D;_tmp14D.tag=4;_tmp14D.f1=& Cyc_pp_r;_tmp14D;});_tmp14C;});_tmp14B->f5=
_tag_arr("Ugly print",sizeof(char),11);_tmp14B;});_tmpED[30]=({struct _tuple11*
_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148->f1=_tag_arr("-pp",sizeof(char),4);
_tmp148->f2=0;_tmp148->f3=_tag_arr("",sizeof(char),1);_tmp148->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149[0]=({
struct Cyc_Arg_Set_spec_struct _tmp14A;_tmp14A.tag=3;_tmp14A.f1=& Cyc_pp_r;_tmp14A;});
_tmp149;});_tmp148->f5=_tag_arr("Pretty print",sizeof(char),13);_tmp148;});
_tmpED[29]=({struct _tuple11*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->f1=
_tag_arr("-ic",sizeof(char),4);_tmp145->f2=0;_tmp145->f3=_tag_arr("",sizeof(char),
1);_tmp145->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp146=_cycalloc(sizeof(*
_tmp146));_tmp146[0]=({struct Cyc_Arg_Set_spec_struct _tmp147;_tmp147.tag=3;
_tmp147.f1=& Cyc_ic_r;_tmp147;});_tmp146;});_tmp145->f5=_tag_arr("Activate the link-checker",
sizeof(char),26);_tmp145;});_tmpED[28]=({struct _tuple11*_tmp142=_cycalloc(
sizeof(*_tmp142));_tmp142->f1=_tag_arr("-stopafter-toc",sizeof(char),15);_tmp142->f2=
0;_tmp142->f3=_tag_arr("",sizeof(char),1);_tmp142->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143[0]=({struct Cyc_Arg_Set_spec_struct
_tmp144;_tmp144.tag=3;_tmp144.f1=& Cyc_toc_r;_tmp144;});_tmp143;});_tmp142->f5=
_tag_arr("Stop after translation to C",sizeof(char),28);_tmp142;});_tmpED[27]=({
struct _tuple11*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->f1=_tag_arr("-stopafter-tc",
sizeof(char),14);_tmp13F->f2=0;_tmp13F->f3=_tag_arr("",sizeof(char),1);_tmp13F->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp140=_cycalloc(sizeof(*_tmp140));
_tmp140[0]=({struct Cyc_Arg_Set_spec_struct _tmp141;_tmp141.tag=3;_tmp141.f1=& Cyc_tc_r;
_tmp141;});_tmp140;});_tmp13F->f5=_tag_arr("Stop after type checking",sizeof(
char),25);_tmp13F;});_tmpED[26]=({struct _tuple11*_tmp13C=_cycalloc(sizeof(*
_tmp13C));_tmp13C->f1=_tag_arr("-stopafter-parse",sizeof(char),17);_tmp13C->f2=0;
_tmp13C->f3=_tag_arr("",sizeof(char),1);_tmp13C->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D[0]=({struct Cyc_Arg_Set_spec_struct
_tmp13E;_tmp13E.tag=3;_tmp13E.f1=& Cyc_parseonly_r;_tmp13E;});_tmp13D;});_tmp13C->f5=
_tag_arr("Stop after parsing",sizeof(char),19);_tmp13C;});_tmpED[25]=({struct
_tuple11*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->f1=_tag_arr("-E",sizeof(
char),3);_tmp139->f2=0;_tmp139->f3=_tag_arr("",sizeof(char),1);_tmp139->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A[0]=({
struct Cyc_Arg_Set_spec_struct _tmp13B;_tmp13B.tag=3;_tmp13B.f1=& Cyc_stop_after_cpp_r;
_tmp13B;});_tmp13A;});_tmp139->f5=_tag_arr("Stop after preprocessing",sizeof(
char),25);_tmp139;});_tmpED[24]=({struct _tuple11*_tmp136=_cycalloc(sizeof(*
_tmp136));_tmp136->f1=_tag_arr("-b",sizeof(char),3);_tmp136->f2=0;_tmp136->f3=
_tag_arr("<arch>",sizeof(char),7);_tmp136->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137[0]=({struct Cyc_Arg_String_spec_struct
_tmp138;_tmp138.tag=5;_tmp138.f1=Cyc_set_target_arch;_tmp138;});_tmp137;});
_tmp136->f5=_tag_arr("Set target architecture; implies -stopafter-toc",sizeof(
char),48);_tmp136;});_tmpED[23]=({struct _tuple11*_tmp133=_cycalloc(sizeof(*
_tmp133));_tmp133->f1=_tag_arr("-MT",sizeof(char),4);_tmp133->f2=0;_tmp133->f3=
_tag_arr(" <target>",sizeof(char),10);_tmp133->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134[0]=({struct Cyc_Arg_String_spec_struct
_tmp135;_tmp135.tag=5;_tmp135.f1=Cyc_set_dependencies_target;_tmp135;});_tmp134;});
_tmp133->f5=_tag_arr("Give target for dependencies",sizeof(char),29);_tmp133;});
_tmpED[22]=({struct _tuple11*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->f1=
_tag_arr("-MG",sizeof(char),4);_tmp130->f2=0;_tmp130->f3=_tag_arr("",sizeof(char),
1);_tmp130->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp131=_cycalloc(sizeof(*
_tmp131));_tmp131[0]=({struct Cyc_Arg_Flag_spec_struct _tmp132;_tmp132.tag=1;
_tmp132.f1=Cyc_add_cpparg;_tmp132;});_tmp131;});_tmp130->f5=_tag_arr("When producing dependencies assume that missing files are generated",
sizeof(char),68);_tmp130;});_tmpED[21]=({struct _tuple11*_tmp12D=_cycalloc(
sizeof(*_tmp12D));_tmp12D->f1=_tag_arr("-M",sizeof(char),3);_tmp12D->f2=0;
_tmp12D->f3=_tag_arr("",sizeof(char),1);_tmp12D->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp12F;_tmp12F.tag=0;_tmp12F.f1=Cyc_set_produce_dependencies;_tmp12F;});_tmp12E;});
_tmp12D->f5=_tag_arr("Produce dependencies",sizeof(char),21);_tmp12D;});_tmpED[
20]=({struct _tuple11*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->f1=_tag_arr("-S",
sizeof(char),3);_tmp12A->f2=0;_tmp12A->f3=_tag_arr("",sizeof(char),1);_tmp12A->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));
_tmp12B[0]=({struct Cyc_Arg_Unit_spec_struct _tmp12C;_tmp12C.tag=0;_tmp12C.f1=Cyc_set_stop_after_asmfile;
_tmp12C;});_tmp12B;});_tmp12A->f5=_tag_arr("Stop after producing assembly code",
sizeof(char),35);_tmp12A;});_tmpED[19]=({struct _tuple11*_tmp127=_cycalloc(
sizeof(*_tmp127));_tmp127->f1=_tag_arr("-pa",sizeof(char),4);_tmp127->f2=0;
_tmp127->f3=_tag_arr("",sizeof(char),1);_tmp127->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp129;_tmp129.tag=0;_tmp129.f1=Cyc_set_pa;_tmp129;});_tmp128;});_tmp127->f5=
_tag_arr("Compile for profiling with aprof",sizeof(char),33);_tmp127;});_tmpED[
18]=({struct _tuple11*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->f1=_tag_arr("-pg",
sizeof(char),4);_tmp124->f2=0;_tmp124->f3=_tag_arr("",sizeof(char),1);_tmp124->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp125=_cycalloc(sizeof(*_tmp125));
_tmp125[0]=({struct Cyc_Arg_Unit_spec_struct _tmp126;_tmp126.tag=0;_tmp126.f1=Cyc_set_pg;
_tmp126;});_tmp125;});_tmp124->f5=_tag_arr("Compile for profiling with gprof",
sizeof(char),33);_tmp124;});_tmpED[17]=({struct _tuple11*_tmp121=_cycalloc(
sizeof(*_tmp121));_tmp121->f1=_tag_arr("-p",sizeof(char),3);_tmp121->f2=0;
_tmp121->f3=_tag_arr("",sizeof(char),1);_tmp121->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp123;_tmp123.tag=1;_tmp123.f1=Cyc_add_ccarg;_tmp123;});_tmp122;});_tmp121->f5=
_tag_arr("Compile for profiling with prof",sizeof(char),32);_tmp121;});_tmpED[16]=({
struct _tuple11*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->f1=_tag_arr("-g",
sizeof(char),3);_tmp11E->f2=0;_tmp11E->f3=_tag_arr("",sizeof(char),1);_tmp11E->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));
_tmp11F[0]=({struct Cyc_Arg_Flag_spec_struct _tmp120;_tmp120.tag=1;_tmp120.f1=Cyc_add_ccarg;
_tmp120;});_tmp11F;});_tmp11E->f5=_tag_arr("Compile for debugging",sizeof(char),
22);_tmp11E;});_tmpED[15]=({struct _tuple11*_tmp11B=_cycalloc(sizeof(*_tmp11B));
_tmp11B->f1=_tag_arr("-fomit-frame-pointer",sizeof(char),21);_tmp11B->f2=0;
_tmp11B->f3=_tag_arr("",sizeof(char),1);_tmp11B->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp11D;_tmp11D.tag=1;_tmp11D.f1=Cyc_add_ccarg;_tmp11D;});_tmp11C;});_tmp11B->f5=
_tag_arr("Omit frame pointer",sizeof(char),19);_tmp11B;});_tmpED[14]=({struct
_tuple11*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->f1=_tag_arr("-O3",sizeof(
char),4);_tmp118->f2=0;_tmp118->f3=_tag_arr("",sizeof(char),1);_tmp118->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp11A;_tmp11A.tag=1;_tmp11A.f1=Cyc_add_ccarg;
_tmp11A;});_tmp119;});_tmp118->f5=_tag_arr("Even more optimization",sizeof(char),
23);_tmp118;});_tmpED[13]=({struct _tuple11*_tmp115=_cycalloc(sizeof(*_tmp115));
_tmp115->f1=_tag_arr("-O2",sizeof(char),4);_tmp115->f2=0;_tmp115->f3=_tag_arr("",
sizeof(char),1);_tmp115->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp116=
_cycalloc(sizeof(*_tmp116));_tmp116[0]=({struct Cyc_Arg_Flag_spec_struct _tmp117;
_tmp117.tag=1;_tmp117.f1=Cyc_add_ccarg;_tmp117;});_tmp116;});_tmp115->f5=
_tag_arr("A higher level of optimization",sizeof(char),31);_tmp115;});_tmpED[12]=({
struct _tuple11*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->f1=_tag_arr("-O",
sizeof(char),3);_tmp112->f2=0;_tmp112->f3=_tag_arr("",sizeof(char),1);_tmp112->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp113=_cycalloc(sizeof(*_tmp113));
_tmp113[0]=({struct Cyc_Arg_Flag_spec_struct _tmp114;_tmp114.tag=1;_tmp114.f1=Cyc_add_ccarg;
_tmp114;});_tmp113;});_tmp112->f5=_tag_arr("Optimize",sizeof(char),9);_tmp112;});
_tmpED[11]=({struct _tuple11*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->f1=
_tag_arr("-O0",sizeof(char),4);_tmp10F->f2=0;_tmp10F->f3=_tag_arr("",sizeof(char),
1);_tmp10F->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp110=_cycalloc(sizeof(*
_tmp110));_tmp110[0]=({struct Cyc_Arg_Flag_spec_struct _tmp111;_tmp111.tag=1;
_tmp111.f1=Cyc_add_ccarg;_tmp111;});_tmp110;});_tmp10F->f5=_tag_arr("Don't optimize",
sizeof(char),15);_tmp10F;});_tmpED[10]=({struct _tuple11*_tmp10C=_cycalloc(
sizeof(*_tmp10C));_tmp10C->f1=_tag_arr("-s",sizeof(char),3);_tmp10C->f2=0;
_tmp10C->f3=_tag_arr("",sizeof(char),1);_tmp10C->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp10E;_tmp10E.tag=1;_tmp10E.f1=Cyc_add_ccarg;_tmp10E;});_tmp10D;});_tmp10C->f5=
_tag_arr("Remove all symbol table and relocation info from executable",sizeof(
char),60);_tmp10C;});_tmpED[9]=({struct _tuple11*_tmp109=_cycalloc(sizeof(*
_tmp109));_tmp109->f1=_tag_arr("-x",sizeof(char),3);_tmp109->f2=0;_tmp109->f3=
_tag_arr(" <language>",sizeof(char),12);_tmp109->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A[0]=({struct Cyc_Arg_String_spec_struct
_tmp10B;_tmp10B.tag=5;_tmp10B.f1=Cyc_set_inputtype;_tmp10B;});_tmp10A;});_tmp109->f5=
_tag_arr("Specify <language> for the following input files",sizeof(char),49);
_tmp109;});_tmpED[8]=({struct _tuple11*_tmp106=_cycalloc(sizeof(*_tmp106));
_tmp106->f1=_tag_arr("-c",sizeof(char),3);_tmp106->f2=0;_tmp106->f3=_tag_arr("",
sizeof(char),1);_tmp106->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp107=
_cycalloc(sizeof(*_tmp107));_tmp107[0]=({struct Cyc_Arg_Unit_spec_struct _tmp108;
_tmp108.tag=0;_tmp108.f1=Cyc_set_stop_after_objectfile;_tmp108;});_tmp107;});
_tmp106->f5=_tag_arr("Produce an object file instead of an executable; do not link",
sizeof(char),61);_tmp106;});_tmpED[7]=({struct _tuple11*_tmp103=_cycalloc(sizeof(*
_tmp103));_tmp103->f1=_tag_arr("-l",sizeof(char),3);_tmp103->f2=1;_tmp103->f3=
_tag_arr("<file>",sizeof(char),7);_tmp103->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp105;_tmp105.tag=1;_tmp105.f1=Cyc_add_libarg;_tmp105;});_tmp104;});_tmp103->f5=
_tag_arr("Library file",sizeof(char),13);_tmp103;});_tmpED[6]=({struct _tuple11*
_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->f1=_tag_arr("-L",sizeof(char),3);
_tmp100->f2=1;_tmp100->f3=_tag_arr("<dir>",sizeof(char),6);_tmp100->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp102;_tmp102.tag=1;_tmp102.f1=Cyc_add_ccarg;
_tmp102;});_tmp101;});_tmp100->f5=_tag_arr("Add to the list of directories for -l",
sizeof(char),38);_tmp100;});_tmpED[5]=({struct _tuple11*_tmpFD=_cycalloc(sizeof(*
_tmpFD));_tmpFD->f1=_tag_arr("-I",sizeof(char),3);_tmpFD->f2=1;_tmpFD->f3=
_tag_arr("<dir>",sizeof(char),6);_tmpFD->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Arg_Flag_spec_struct
_tmpFF;_tmpFF.tag=1;_tmpFF.f1=Cyc_add_cpparg;_tmpFF;});_tmpFE;});_tmpFD->f5=
_tag_arr("Add to the list of directories to search for include files",sizeof(
char),59);_tmpFD;});_tmpED[4]=({struct _tuple11*_tmpFA=_cycalloc(sizeof(*_tmpFA));
_tmpFA->f1=_tag_arr("-B",sizeof(char),3);_tmpFA->f2=1;_tmpFA->f3=_tag_arr("<file>",
sizeof(char),7);_tmpFA->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmpFB=
_cycalloc(sizeof(*_tmpFB));_tmpFB[0]=({struct Cyc_Arg_Flag_spec_struct _tmpFC;
_tmpFC.tag=1;_tmpFC.f1=Cyc_add_cyclone_exec_path;_tmpFC;});_tmpFB;});_tmpFA->f5=
_tag_arr("Add to the list of directories to search for compiler files",sizeof(
char),60);_tmpFA;});_tmpED[3]=({struct _tuple11*_tmpF7=_cycalloc(sizeof(*_tmpF7));
_tmpF7->f1=_tag_arr("-D",sizeof(char),3);_tmpF7->f2=1;_tmpF7->f3=_tag_arr("<name>[=<value>]",
sizeof(char),17);_tmpF7->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmpF8=
_cycalloc(sizeof(*_tmpF8));_tmpF8[0]=({struct Cyc_Arg_Flag_spec_struct _tmpF9;
_tmpF9.tag=1;_tmpF9.f1=Cyc_add_cpparg;_tmpF9;});_tmpF8;});_tmpF7->f5=_tag_arr("Pass definition to preprocessor",
sizeof(char),32);_tmpF7;});_tmpED[2]=({struct _tuple11*_tmpF4=_cycalloc(sizeof(*
_tmpF4));_tmpF4->f1=_tag_arr("-o",sizeof(char),3);_tmpF4->f2=0;_tmpF4->f3=
_tag_arr(" <file>",sizeof(char),8);_tmpF4->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5[0]=({struct Cyc_Arg_String_spec_struct
_tmpF6;_tmpF6.tag=5;_tmpF6.f1=Cyc_set_output_file;_tmpF6;});_tmpF5;});_tmpF4->f5=
_tag_arr("Set the output file name to <file>",sizeof(char),35);_tmpF4;});_tmpED[
1]=({struct _tuple11*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->f1=_tag_arr("--version",
sizeof(char),10);_tmpF1->f2=0;_tmpF1->f3=_tag_arr("",sizeof(char),1);_tmpF1->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2[
0]=({struct Cyc_Arg_Unit_spec_struct _tmpF3;_tmpF3.tag=0;_tmpF3.f1=Cyc_print_version;
_tmpF3;});_tmpF2;});_tmpF1->f5=_tag_arr("Print version information and exit",
sizeof(char),35);_tmpF1;});_tmpED[0]=({struct _tuple11*_tmpEE=_cycalloc(sizeof(*
_tmpEE));_tmpEE->f1=_tag_arr("-v",sizeof(char),3);_tmpEE->f2=0;_tmpEE->f3=
_tag_arr("",sizeof(char),1);_tmpEE->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Arg_Set_spec_struct
_tmpF0;_tmpF0.tag=3;_tmpF0.f1=& Cyc_v_r;_tmpF0;});_tmpEF;});_tmpEE->f5=_tag_arr("Print compilation stages verbosely",
sizeof(char),35);_tmpEE;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmpED,sizeof(struct _tuple11*),50));});Cyc_Arg_parse(options,Cyc_add_other,
_tag_arr("Options:",sizeof(char),9),argv);if(Cyc_cyclone_files == 0?Cyc_ccargs == 
0: 0){({void*_tmpB2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("missing file\n",
sizeof(char),14),_tag_arr(_tmpB2,sizeof(void*),0));});exit(1);}if(Cyc_target_arch.curr
== ((struct _tagged_arr)_tag_arr(0,0,0)).curr)Cyc_target_arch=(struct _tagged_arr)
Cstring_to_string(Carch);{struct _tagged_arr cyclone_exec_prefix=(struct
_tagged_arr)Cyc_Std_getenv(_tag_arr("CYCLONE_EXEC_PREFIX",sizeof(char),20));if(
cyclone_exec_prefix.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr)Cyc_cyclone_exec_path=({
struct Cyc_List_List*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->hd=({struct
_tagged_arr*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=cyclone_exec_prefix;
_tmpB4;});_tmpB3->tl=Cyc_cyclone_exec_path;_tmpB3;});{struct _tagged_arr
def_lib_path=(struct _tagged_arr)Cstring_to_string(Cdef_lib_path);if(Cyc_Std_strlen(
def_lib_path)> 0)Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmpB5=_cycalloc(
sizeof(*_tmpB5));_tmpB5->hd=({struct _tagged_arr*_tmpB6=_cycalloc(sizeof(*_tmpB6));
_tmpB6[0]=(struct _tagged_arr)Cyc_Filename_concat(def_lib_path,_tag_arr("cyc-lib",
sizeof(char),8));_tmpB6;});_tmpB5->tl=Cyc_cyclone_exec_path;_tmpB5;});Cyc_cyclone_exec_path=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);{
struct Cyc_List_List*_tmpB7=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);
if(!Cyc_stop_after_cpp_r)Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,
_tag_arr("cyc_include.h",sizeof(char),14));{struct Cyc_List_List*_tmpB8=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;
_tmpB8 != 0;_tmpB8=_tmpB8->tl){Cyc_process_file(*((struct _tagged_arr*)_tmpB8->hd));
if(Cyc_compile_failure)return 1;}}if(((Cyc_stop_after_cpp_r?1: Cyc_parseonly_r)?1:
Cyc_tc_r)?1: Cyc_toc_r)return 0;if(Cyc_ccargs == 0)return 0;Cyc_add_ccarg((struct
_tagged_arr)Cyc_Std_strconcat(_tag_arr("-L",sizeof(char),3),def_lib_path));Cyc_ccargs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{struct
_tagged_arr _tmpB9=Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(struct _tagged_arr*(*
f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_ccargs),_tag_arr(" ",sizeof(char),2));Cyc_libargs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{struct _tagged_arr _tmpBA=Cyc_Std_str_sepstr(({
struct Cyc_List_List*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->hd=_init_tag_arr(
_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);_tmpEB->tl=((struct Cyc_List_List*(*)(
struct _tagged_arr*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_libargs);_tmpEB;}),_tag_arr(" ",sizeof(char),2));
struct Cyc_List_List*stdlib;struct _tagged_arr stdlib_string;int _tmpBB=((Cyc_stop_after_asmfile_r?
1: Cyc_stop_after_objectfile_r)?1:(Cyc_output_file != 0?Cyc_Filename_check_suffix(*((
struct _tagged_arr*)_check_null(Cyc_output_file)),_tag_arr(".a",sizeof(char),3)):
0))?1:(Cyc_output_file != 0?Cyc_Filename_check_suffix(*((struct _tagged_arr*)
_check_null(Cyc_output_file)),_tag_arr(".lib",sizeof(char),5)): 0);if(_tmpBB){
stdlib=0;stdlib_string=_tag_arr("",sizeof(char),1);}else{struct _tagged_arr
libcyc_flag;struct _tagged_arr nogc_filename;struct _tagged_arr runtime_filename;
struct _tagged_arr gc_filename;if(Cyc_pa_r){libcyc_flag=_tag_arr("-lcyc_a",sizeof(
char),8);nogc_filename=_tag_arr("nogc_a.a",sizeof(char),9);runtime_filename=
_tag_arr("runtime_cyc_a.o",sizeof(char),16);}else{if(Cyc_nocheck_r){libcyc_flag=
_tag_arr("-lcyc_nocheck",sizeof(char),14);nogc_filename=_tag_arr("nogc_nocheck.a",
sizeof(char),15);runtime_filename=_tag_arr("runtime_cyc.o",sizeof(char),14);}
else{if(Cyc_pg_r){libcyc_flag=_tag_arr("-lcyc_pg",sizeof(char),9);
runtime_filename=_tag_arr("runtime_cyc_pg.a",sizeof(char),17);nogc_filename=
_tag_arr("nogc_pg.a",sizeof(char),10);}else{libcyc_flag=_tag_arr("-lcyc",sizeof(
char),6);nogc_filename=_tag_arr("nogc.a",sizeof(char),7);runtime_filename=
_tag_arr("runtime_cyc.o",sizeof(char),14);}}}gc_filename=_tag_arr("gc.a",sizeof(
char),5);{struct _tagged_arr _tmpBC=Cyc_nogc_r?Cyc_do_find(_tmpB7,nogc_filename):
Cyc_do_find(_tmpB7,gc_filename);struct _tagged_arr _tmpBD=Cyc_do_find(_tmpB7,
runtime_filename);stdlib=0;stdlib_string=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmpC1;_tmpC1.tag=0;_tmpC1.f1=(struct _tagged_arr)_tmpBC;{struct Cyc_Std_String_pa_struct
_tmpC0;_tmpC0.tag=0;_tmpC0.f1=(struct _tagged_arr)libcyc_flag;{struct Cyc_Std_String_pa_struct
_tmpBF;_tmpBF.tag=0;_tmpBF.f1=(struct _tagged_arr)_tmpBD;{void*_tmpBE[3]={& _tmpBF,&
_tmpC0,& _tmpC1};Cyc_Std_aprintf(_tag_arr(" %s %s %s",sizeof(char),10),_tag_arr(
_tmpBE,sizeof(void*),3));}}}});}}if(Cyc_ic_r){struct _handler_cons _tmpC2;
_push_handler(& _tmpC2);{int _tmpC4=0;if(setjmp(_tmpC2.handler))_tmpC4=1;if(!
_tmpC4){Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _tagged_arr*),struct
Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);Cyc_libargs=((struct
Cyc_List_List*(*)(int(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_filter)(
Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*_tmpC5=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmpBB)_tmpC5=({struct Cyc_List_List*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->hd=
Cyc_final_strptr;_tmpC6->tl=_tmpC5;_tmpC6;});{struct Cyc_Interface_I*_tmpC7=((
struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _tagged_arr*),
struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio,_tmpC5,_tmpC5);if(_tmpC7 == 0){({void*_tmpC8[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: interfaces incompatible\n",sizeof(char),32),
_tag_arr(_tmpC8,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmpC9=1;_npop_handler(0);return _tmpC9;}}if(_tmpBB){if(Cyc_output_file != 0){
struct _tagged_arr _tmpCA=({struct Cyc_Std_String_pa_struct _tmpCE;_tmpCE.tag=0;
_tmpCE.f1=(struct _tagged_arr)Cyc_Filename_chop_extension(*((struct _tagged_arr*)
_check_null(Cyc_output_file)));{void*_tmpCD[1]={& _tmpCE};Cyc_Std_aprintf(
_tag_arr("%s.cycio",sizeof(char),9),_tag_arr(_tmpCD,sizeof(void*),1));}});struct
Cyc_Std___cycFILE*_tmpCB=Cyc_try_file_open((struct _tagged_arr)_tmpCA,_tag_arr("wb",
sizeof(char),3),_tag_arr("interface object file",sizeof(char),22));if(_tmpCB == 0){
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmpCC=1;_npop_handler(0);return
_tmpCC;}}Cyc_Interface_save((struct Cyc_Interface_I*)_check_null(_tmpC7),(struct
Cyc_Std___cycFILE*)_check_null(_tmpCB));Cyc_Std_file_close((struct Cyc_Std___cycFILE*)
_check_null(_tmpCB));}}else{if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(
struct Cyc_Interface_I*)_check_null(_tmpC7),({struct _tuple9*_tmpCF=_cycalloc(
sizeof(*_tmpCF));_tmpCF->f1=_tag_arr("empty interface",sizeof(char),16);_tmpCF->f2=
_tag_arr("global interface",sizeof(char),17);_tmpCF;}))){({void*_tmpD0[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: some objects are still undefined\n",sizeof(char),
41),_tag_arr(_tmpD0,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmpD1=1;_npop_handler(0);return _tmpD1;}}}}};_pop_handler();}else{void*_tmpC3=(
void*)_exn_thrown;void*_tmpD3=_tmpC3;struct _tagged_arr _tmpD4;struct _tagged_arr
_tmpD5;struct _tagged_arr _tmpD6;_LL36: if(*((void**)_tmpD3)!= Cyc_Core_Failure)
goto _LL38;_tmpD4=((struct Cyc_Core_Failure_struct*)_tmpD3)->f1;_LL37:({struct Cyc_Std_String_pa_struct
_tmpD8;_tmpD8.tag=0;_tmpD8.f1=(struct _tagged_arr)_tmpD4;{void*_tmpD7[1]={& _tmpD8};
Cyc_Std_printf(_tag_arr("Exception Core::Failure %s\n",sizeof(char),28),_tag_arr(
_tmpD7,sizeof(void*),1));}});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL38: if(*((void**)_tmpD3)!= Cyc_Core_Impossible)goto _LL3A;_tmpD5=((struct Cyc_Core_Impossible_struct*)
_tmpD3)->f1;_LL39:({struct Cyc_Std_String_pa_struct _tmpDA;_tmpDA.tag=0;_tmpDA.f1=(
struct _tagged_arr)_tmpD5;{void*_tmpD9[1]={& _tmpDA};Cyc_Std_printf(_tag_arr("Exception Core::Impossible %s\n",
sizeof(char),31),_tag_arr(_tmpD9,sizeof(void*),1));}});Cyc_compile_failure=1;Cyc_remove_cfiles();
return 1;_LL3A: if(_tmpD3 != Cyc_Dict_Absent)goto _LL3C;_LL3B:({void*_tmpDB[0]={};
Cyc_Std_printf(_tag_arr("Exception Dict::Absent\n",sizeof(char),24),_tag_arr(
_tmpDB,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;
_LL3C: if(*((void**)_tmpD3)!= Cyc_Core_Invalid_argument)goto _LL3E;_tmpD6=((struct
Cyc_Core_Invalid_argument_struct*)_tmpD3)->f1;_LL3D:({struct Cyc_Std_String_pa_struct
_tmpDD;_tmpDD.tag=0;_tmpDD.f1=(struct _tagged_arr)_tmpD6;{void*_tmpDC[1]={& _tmpDD};
Cyc_Std_printf(_tag_arr("Exception Core::Invalid_argument %s\n",sizeof(char),37),
_tag_arr(_tmpDC,sizeof(void*),1));}});Cyc_compile_failure=1;Cyc_remove_cfiles();
return 1;_LL3E:;_LL3F:({void*_tmpDE[0]={};Cyc_Std_printf(_tag_arr("Uncaught exception\n",
sizeof(char),20),_tag_arr(_tmpDE,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();
return 1;_LL40:;_LL41:(void)_throw(_tmpD3);_LL35:;}}}{struct _tagged_arr _tmpDF=({
struct Cyc_Std_String_pa_struct _tmpE8;_tmpE8.tag=0;_tmpE8.f1=(struct _tagged_arr)
_tmpBA;{struct Cyc_Std_String_pa_struct _tmpE7;_tmpE7.tag=0;_tmpE7.f1=(struct
_tagged_arr)stdlib_string;{struct Cyc_Std_String_pa_struct _tmpE6;_tmpE6.tag=0;
_tmpE6.f1=(struct _tagged_arr)_tmpB9;{struct Cyc_Std_String_pa_struct _tmpE5;_tmpE5.tag=
0;_tmpE5.f1=(struct _tagged_arr)(Cyc_output_file == 0?_tag_arr("",sizeof(char),1):(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpEA;_tmpEA.tag=0;_tmpEA.f1=(
struct _tagged_arr)Cyc_sh_escape_string(*((struct _tagged_arr*)_check_null(Cyc_output_file)));{
void*_tmpE9[1]={& _tmpEA};Cyc_Std_aprintf(_tag_arr(" -o %s",sizeof(char),7),
_tag_arr(_tmpE9,sizeof(void*),1));}}));{struct Cyc_Std_String_pa_struct _tmpE4;
_tmpE4.tag=0;_tmpE4.f1=(struct _tagged_arr)comp;{void*_tmpE3[5]={& _tmpE4,& _tmpE5,&
_tmpE6,& _tmpE7,& _tmpE8};Cyc_Std_aprintf(_tag_arr("%s %s %s%s%s",sizeof(char),13),
_tag_arr(_tmpE3,sizeof(void*),5));}}}}}});if(Cyc_v_r)({struct Cyc_Std_String_pa_struct
_tmpE1;_tmpE1.tag=0;_tmpE1.f1=(struct _tagged_arr)_tmpDF;{void*_tmpE0[1]={& _tmpE1};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s\n",sizeof(char),4),_tag_arr(_tmpE0,
sizeof(void*),1));}});if(Cyc_Std_system((struct _tagged_arr)_tmpDF)!= 0){({void*
_tmpE2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: C compiler failed\n",
sizeof(char),26),_tag_arr(_tmpE2,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();
return 1;}Cyc_remove_cfiles();return Cyc_compile_failure?1: 0;}}}}}}}}}
