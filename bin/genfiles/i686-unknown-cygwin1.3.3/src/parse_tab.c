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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{char*
tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Cstdio___abstractFILE;struct
Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr);struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);struct _tagged_arr Cyc_Std_vrprintf(struct
_RegionHandle*r1,struct _tagged_arr fmt,struct _tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct
Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file(struct Cyc_Std___cycFILE*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void*Cyc_List_fold_right(
void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*
x);extern char Cyc_List_Nth[8];struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);unsigned int Cyc_Std_strlen(
struct _tagged_arr s);int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);
int Cyc_Std_zstrcmp(struct _tagged_arr,struct _tagged_arr);int Cyc_Std_zstrptrcmp(
struct _tagged_arr*,struct _tagged_arr*);struct _tagged_arr Cyc_Std_strcat(struct
_tagged_arr dest,struct _tagged_arr src);struct _tagged_arr Cyc_Std_strcpy(struct
_tagged_arr dest,struct _tagged_arr src);struct _tagged_arr Cyc_Std_substring(struct
_tagged_arr,int ofs,unsigned int n);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct Cyc_Position_Segment*Cyc_Position_segment_join(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};struct Cyc_Position_Error*
Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*,struct _tagged_arr);extern
char Cyc_Position_Nocontext[14];void Cyc_Position_post_error(struct Cyc_Position_Error*);
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
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
exist_vars;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;struct
Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[
15];extern void*Cyc_Absyn_rel_ns_null;int Cyc_Absyn_is_qvar_qualified(struct
_tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,
struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_uchar_t;extern
void*Cyc_Absyn_uint_t;extern void*Cyc_Absyn_sint_t;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);extern struct _tuple1*Cyc_Absyn_exn_name;extern
struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;void*Cyc_Absyn_starb_typ(void*t,
void*rgn,struct Cyc_Absyn_Tqual tq,void*b);void*Cyc_Absyn_atb_typ(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,void*b);void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _tagged_arr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _tagged_arr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(
void*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _tagged_arr*lab,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_forarray_stmt(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct
Cyc_Position_Segment*s);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Core_Opt*
t_opt,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_letv_decl(struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple1*n,
struct Cyc_List_List*ts,struct Cyc_Core_Opt*exist_ts,struct Cyc_Core_Opt*fs,struct
Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(
void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int
is_xtunion,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(struct Cyc_List_List*
tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,
struct Cyc_List_List*);struct _tagged_arr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple4{void*f1;void*f2;};struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Tcenv_VarRes_struct{int tag;void*
f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*
f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict*
aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct
Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*
availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;};struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{int tag;void*f1;void*f2;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*
le;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct
_tagged_arr ap);void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_compress(void*t);
void Cyc_Lex_register_typedef(struct _tuple1*s);void Cyc_Lex_enter_namespace(struct
_tagged_arr*);void Cyc_Lex_leave_namespace();void Cyc_Lex_enter_using(struct
_tuple1*);void Cyc_Lex_leave_using();struct Cyc_Parse_Signed_spec_struct{int tag;
struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Unsigned_spec_struct{int tag;
struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Short_spec_struct{int tag;struct
Cyc_Position_Segment*f1;};struct Cyc_Parse_Long_spec_struct{int tag;struct Cyc_Position_Segment*
f1;};struct Cyc_Parse_Type_spec_struct{int tag;void*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Parse_Decl_spec_struct{int tag;struct Cyc_Absyn_Decl*f1;};struct Cyc_Parse_Declaration_spec{
struct Cyc_Core_Opt*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int
is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Declarator{struct
_tuple1*id;struct Cyc_List_List*tms;};struct Cyc_Parse_Abstractdeclarator{struct
Cyc_List_List*tms;};struct _tuple5{void*f1;struct Cyc_Core_Opt*f2;};static struct
_tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc);struct _tuple6{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct
Cyc_List_List*f4;};static struct _tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,
void*,struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Core_Opt*Cyc_Parse_lbuf=
0;static struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(
struct _tagged_arr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(Cyc_Position_mk_err_parse(
sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*sg,struct
_tagged_arr fmt,struct _tagged_arr ap){Cyc_Parse_err((struct _tagged_arr)Cyc_Std_vrprintf(
Cyc_Core_heap_region,fmt,ap),sg);(int)_throw((void*)Cyc_Position_Exit);}static
void Cyc_Parse_unimp(struct _tagged_arr msg,struct Cyc_Position_Segment*sg){({struct
Cyc_Std_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(struct _tagged_arr)msg;{
struct Cyc_Std_String_pa_struct _tmp1;_tmp1.tag=0;_tmp1.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(
sg);{void*_tmp0[2]={& _tmp1,& _tmp2};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: Warning: Cyclone does not yet support %s\n",
sizeof(char),46),_tag_arr(_tmp0,sizeof(void*),2));}}});return;}static int Cyc_Parse_enum_counter=
0;struct _tuple1*Cyc_Parse_gensym_enum(){return({struct _tuple1*_tmp3=_cycalloc(
sizeof(*_tmp3));_tmp3->f1=(void*)({struct Cyc_Absyn_Rel_n_struct*_tmp7=_cycalloc(
sizeof(*_tmp7));_tmp7[0]=({struct Cyc_Absyn_Rel_n_struct _tmp8;_tmp8.tag=0;_tmp8.f1=
0;_tmp8;});_tmp7;});_tmp3->f2=({struct _tagged_arr*_tmp4=_cycalloc(sizeof(*_tmp4));
_tmp4[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp6;_tmp6.tag=1;_tmp6.f1=(
int)((unsigned int)Cyc_Parse_enum_counter ++);{void*_tmp5[1]={& _tmp6};Cyc_Std_aprintf(
_tag_arr("__anonymous_enum_%d__",sizeof(char),22),_tag_arr(_tmp5,sizeof(void*),1));}});
_tmp4;});_tmp3;});}struct _tuple7{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*
f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple8{struct _tuple7*
f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple8*field_info){struct _tuple8 _tmpA;
struct _tuple7*_tmpB;struct _tuple7 _tmpC;struct _tuple1*_tmpD;struct Cyc_Absyn_Tqual
_tmpE;void*_tmpF;struct Cyc_List_List*_tmp10;struct Cyc_List_List*_tmp11;struct Cyc_Absyn_Exp*
_tmp12;struct _tuple8*_tmp9=field_info;_tmpA=*_tmp9;_tmpB=_tmpA.f1;_tmpC=*_tmpB;
_tmpD=_tmpC.f1;_tmpE=_tmpC.f2;_tmpF=_tmpC.f3;_tmp10=_tmpC.f4;_tmp11=_tmpC.f5;
_tmp12=_tmpA.f2;if(_tmp10 != 0)Cyc_Parse_err(_tag_arr("bad type params in struct field",
sizeof(char),32),loc);if(Cyc_Absyn_is_qvar_qualified(_tmpD))Cyc_Parse_err(
_tag_arr("struct or union field cannot be qualified with a namespace",sizeof(
char),59),loc);return({struct Cyc_Absyn_Aggrfield*_tmp13=_cycalloc(sizeof(*_tmp13));
_tmp13->name=(*_tmpD).f2;_tmp13->tq=_tmpE;_tmp13->type=(void*)_tmpF;_tmp13->width=
_tmp12;_tmp13->attributes=_tmp11;_tmp13;});}static void*Cyc_Parse_type_spec(void*
t,struct Cyc_Position_Segment*loc){return(void*)({struct Cyc_Parse_Type_spec_struct*
_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14[0]=({struct Cyc_Parse_Type_spec_struct
_tmp15;_tmp15.tag=4;_tmp15.f1=(void*)t;_tmp15.f2=loc;_tmp15;});_tmp14;});}static
void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp16=t;void*_tmp17;struct Cyc_Absyn_Tqual
_tmp18;struct Cyc_Absyn_Exp*_tmp19;_LL1: if(_tmp16 <= (void*)3?1:*((int*)_tmp16)!= 
7)goto _LL3;_tmp17=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp16)->f1;_tmp18=((
struct Cyc_Absyn_ArrayType_struct*)_tmp16)->f2;_tmp19=((struct Cyc_Absyn_ArrayType_struct*)
_tmp16)->f3;_LL2: return Cyc_Absyn_starb_typ(_tmp17,argposn?Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->v=(void*)((void*)3);
_tmp1A;}),0):(void*)2,_tmp18,_tmp19 == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp19);_tmp1C;});
_tmp1B;}));_LL3:;_LL4: return t;_LL0:;}static void Cyc_Parse_arg_array2ptr(struct
_tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*x).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual
f1;void*f2;};static struct _tuple9*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*
loc,struct _tuple2*t){return({struct _tuple9*_tmp1D=_cycalloc(sizeof(*_tmp1D));
_tmp1D->f1=(*t).f2;_tmp1D->f2=(*t).f3;_tmp1D;});}static struct Cyc_Absyn_Vardecl*
Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){void*_tmp1E=(void*)d->r;struct Cyc_Absyn_Vardecl*
_tmp1F;_LL6: if(*((int*)_tmp1E)!= 0)goto _LL8;_tmp1F=((struct Cyc_Absyn_Var_d_struct*)
_tmp1E)->f1;_LL7: return _tmp1F;_LL8:;_LL9:({void*_tmp20[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(d->loc,_tag_arr("bad declaration in `forarray' statement",
sizeof(char),40),_tag_arr(_tmp20,sizeof(void*),0));});_LL5:;}static int Cyc_Parse_is_typeparam(
void*tm){void*_tmp21=tm;_LLB: if(_tmp21 <= (void*)1?1:*((int*)_tmp21)!= 3)goto _LLD;
_LLC: return 1;_LLD:;_LLE: return 0;_LLA:;}static void*Cyc_Parse_id2type(struct
_tagged_arr s,void*k){if(Cyc_Std_zstrcmp(s,_tag_arr("`H",sizeof(char),3))== 0)
return(void*)2;else{return(void*)({struct Cyc_Absyn_VarType_struct*_tmp22=
_cycalloc(sizeof(*_tmp22));_tmp22[0]=({struct Cyc_Absyn_VarType_struct _tmp23;
_tmp23.tag=1;_tmp23.f1=({struct Cyc_Absyn_Tvar*_tmp24=_cycalloc(sizeof(*_tmp24));
_tmp24->name=({struct _tagged_arr*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25[0]=s;
_tmp25;});_tmp24->identity=0;_tmp24->kind=(void*)k;_tmp24;});_tmp23;});_tmp22;});}}
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){void*k;{
void*_tmp26=Cyc_Absyn_compress_kb((void*)t->kind);void*_tmp27;void*_tmp28;_LL10:
if(*((int*)_tmp26)!= 0)goto _LL12;_tmp27=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp26)->f1;_LL11: k=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp29=_cycalloc(
sizeof(*_tmp29));_tmp29[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp2A;_tmp2A.tag=0;
_tmp2A.f1=(void*)_tmp27;_tmp2A;});_tmp29;});goto _LLF;_LL12: if(*((int*)_tmp26)!= 
1)goto _LL14;_LL13: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp2B=_cycalloc(
sizeof(*_tmp2B));_tmp2B[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp2C;_tmp2C.tag=
1;_tmp2C.f1=0;_tmp2C;});_tmp2B;});goto _LLF;_LL14: if(*((int*)_tmp26)!= 2)goto _LLF;
_tmp28=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp26)->f2;_LL15: k=(void*)({
struct Cyc_Absyn_Less_kb_struct*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D[0]=({
struct Cyc_Absyn_Less_kb_struct _tmp2E;_tmp2E.tag=2;_tmp2E.f1=0;_tmp2E.f2=(void*)
_tmp28;_tmp2E;});_tmp2D;});goto _LLF;_LLF:;}return({struct Cyc_Absyn_Tvar*_tmp2F=
_cycalloc(sizeof(*_tmp2F));_tmp2F->name=t->name;_tmp2F->identity=0;_tmp2F->kind=(
void*)k;_tmp2F;});}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmp30=t;struct Cyc_Absyn_Tvar*_tmp31;_LL17: if(_tmp30 <= (void*)3?
1:*((int*)_tmp30)!= 1)goto _LL19;_tmp31=((struct Cyc_Absyn_VarType_struct*)_tmp30)->f1;
_LL18: return _tmp31;_LL19:;_LL1A:({void*_tmp32[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,_tag_arr("expecting a list of type variables, not types",
sizeof(char),46),_tag_arr(_tmp32,sizeof(void*),0));});_LL16:;}static void*Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar*pr){return(void*)({struct Cyc_Absyn_VarType_struct*_tmp33=
_cycalloc(sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Absyn_VarType_struct _tmp34;
_tmp34.tag=1;_tmp34.f1=pr;_tmp34;});_tmp33;});}static void Cyc_Parse_set_vartyp_kind(
void*t,void*k){void*_tmp35=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp36;
struct Cyc_Absyn_Tvar _tmp37;void*_tmp38;void**_tmp39;_LL1C: if(_tmp35 <= (void*)3?1:*((
int*)_tmp35)!= 1)goto _LL1E;_tmp36=((struct Cyc_Absyn_VarType_struct*)_tmp35)->f1;
_tmp37=*_tmp36;_tmp38=(void*)_tmp37.kind;_tmp39=(void**)&(*((struct Cyc_Absyn_VarType_struct*)
_tmp35)->f1).kind;_LL1D: {void*_tmp3A=Cyc_Absyn_compress_kb(*_tmp39);_LL21: if(*((
int*)_tmp3A)!= 1)goto _LL23;_LL22:*_tmp39=(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp3C;
_tmp3C.tag=0;_tmp3C.f1=(void*)k;_tmp3C;});_tmp3B;});return;_LL23:;_LL24: return;
_LL20:;}_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp3D=(void*)tms->hd;void*_tmp3E;
_LL26: if(_tmp3D <= (void*)1?1:*((int*)_tmp3D)!= 2)goto _LL28;_tmp3E=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmp3D)->f1;_LL27: if(tms->tl == 0?1:(Cyc_Parse_is_typeparam((
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)?((struct Cyc_List_List*)
_check_null(tms->tl))->tl == 0: 0)){void*_tmp3F=_tmp3E;struct Cyc_List_List*_tmp40;
_LL2B: if(*((int*)_tmp3F)!= 1)goto _LL2D;_LL2C:({void*_tmp41[0]={};Cyc_Tcutil_warn(
loc,_tag_arr("function declaration with both new- and old-style parameter declarations; ignoring old-style",
sizeof(char),93),_tag_arr(_tmp41,sizeof(void*),0));});return tms;_LL2D: if(*((int*)
_tmp3F)!= 0)goto _LL2A;_tmp40=((struct Cyc_Absyn_NoTypes_struct*)_tmp3F)->f1;_LL2E:
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp40)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(tds))({void*_tmp42[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,_tag_arr("wrong number of parameter declarations in old-style function declaration",
sizeof(char),73),_tag_arr(_tmp42,sizeof(void*),0));});{struct Cyc_List_List*
_tmp43=0;for(0;_tmp40 != 0;_tmp40=_tmp40->tl){struct Cyc_List_List*_tmp44=tds;for(
0;_tmp44 != 0;_tmp44=_tmp44->tl){struct Cyc_Absyn_Decl*_tmp45=(struct Cyc_Absyn_Decl*)
_tmp44->hd;void*_tmp46=(void*)_tmp45->r;struct Cyc_Absyn_Vardecl*_tmp47;_LL30: if(*((
int*)_tmp46)!= 0)goto _LL32;_tmp47=((struct Cyc_Absyn_Var_d_struct*)_tmp46)->f1;
_LL31: if(Cyc_Std_zstrptrcmp((*_tmp47->name).f2,(struct _tagged_arr*)_tmp40->hd)!= 
0)continue;if(_tmp47->initializer != 0)({void*_tmp48[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp45->loc,
_tag_arr("initializer found in parameter declaration",sizeof(char),43),_tag_arr(
_tmp48,sizeof(void*),0));});if(Cyc_Absyn_is_qvar_qualified(_tmp47->name))({void*
_tmp49[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Parse_abort)(_tmp45->loc,_tag_arr("namespaces forbidden in parameter declarations",
sizeof(char),47),_tag_arr(_tmp49,sizeof(void*),0));});_tmp43=({struct Cyc_List_List*
_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->hd=({struct _tuple2*_tmp4B=_cycalloc(
sizeof(*_tmp4B));_tmp4B->f1=({struct Cyc_Core_Opt*_tmp4C=_cycalloc(sizeof(*_tmp4C));
_tmp4C->v=(*_tmp47->name).f2;_tmp4C;});_tmp4B->f2=_tmp47->tq;_tmp4B->f3=(void*)
_tmp47->type;_tmp4B;});_tmp4A->tl=_tmp43;_tmp4A;});goto L;_LL32:;_LL33:({void*
_tmp4D[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Parse_abort)(_tmp45->loc,_tag_arr("nonvariable declaration in parameter type",
sizeof(char),42),_tag_arr(_tmp4D,sizeof(void*),0));});_LL2F:;}L: if(_tmp44 == 0)({
struct Cyc_Std_String_pa_struct _tmp4F;_tmp4F.tag=0;_tmp4F.f1=(struct _tagged_arr)*((
struct _tagged_arr*)_tmp40->hd);{void*_tmp4E[1]={& _tmp4F};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(loc,_tag_arr("%s is not given a type",
sizeof(char),23),_tag_arr(_tmp4E,sizeof(void*),1));}});}return({struct Cyc_List_List*
_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp52;_tmp52.tag=2;_tmp52.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp54;_tmp54.tag=1;_tmp54.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp43);_tmp54.f2=0;_tmp54.f3=0;_tmp54.f4=0;_tmp54.f5=0;_tmp54;});
_tmp53;}));_tmp52;});_tmp51;}));_tmp50->tl=0;_tmp50;});}_LL2A:;}goto _LL29;_LL28:;
_LL29: return({struct Cyc_List_List*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->hd=(
void*)((void*)tms->hd);_tmp55->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);
_tmp55;});_LL25:;}}struct _tuple10{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct Cyc_Core_Opt*
dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*
body,struct Cyc_Position_Segment*loc){if(tds != 0)d=({struct Cyc_Parse_Declarator*
_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->id=d->id;_tmp56->tms=Cyc_Parse_oldstyle2newstyle(
d->tms,tds,loc);_tmp56;});{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual();int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp57=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL35: if((int)_tmp57 != 1)goto _LL37;
_LL36: sc=(void*)3;goto _LL34;_LL37: if((int)_tmp57 != 3)goto _LL39;_LL38: sc=(void*)0;
goto _LL34;_LL39:;_LL3A: Cyc_Parse_err(_tag_arr("bad storage class on function",
sizeof(char),30),loc);goto _LL34;_LL34:;}}{void*_tmp59;struct Cyc_Core_Opt*_tmp5A;
struct _tuple5 _tmp58=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmp59=_tmp58.f1;
_tmp5A=_tmp58.f2;{struct Cyc_Absyn_Tqual _tmp5C;void*_tmp5D;struct Cyc_List_List*
_tmp5E;struct Cyc_List_List*_tmp5F;struct _tuple6 _tmp5B=Cyc_Parse_apply_tms(tq,
_tmp59,atts,d->tms);_tmp5C=_tmp5B.f1;_tmp5D=_tmp5B.f2;_tmp5E=_tmp5B.f3;_tmp5F=
_tmp5B.f4;if(_tmp5A != 0)({void*_tmp60[0]={};Cyc_Tcutil_warn(loc,_tag_arr("nested type declaration within function prototype",
sizeof(char),50),_tag_arr(_tmp60,sizeof(void*),0));});if(_tmp5E != 0)({void*
_tmp61[0]={};Cyc_Tcutil_warn(loc,_tag_arr("bad type params, ignoring",sizeof(
char),26),_tag_arr(_tmp61,sizeof(void*),0));});{void*_tmp62=_tmp5D;struct Cyc_Absyn_FnInfo
_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_Core_Opt*_tmp65;void*_tmp66;struct
Cyc_List_List*_tmp67;int _tmp68;struct Cyc_Absyn_VarargInfo*_tmp69;struct Cyc_List_List*
_tmp6A;struct Cyc_List_List*_tmp6B;_LL3C: if(_tmp62 <= (void*)3?1:*((int*)_tmp62)!= 
8)goto _LL3E;_tmp63=((struct Cyc_Absyn_FnType_struct*)_tmp62)->f1;_tmp64=_tmp63.tvars;
_tmp65=_tmp63.effect;_tmp66=(void*)_tmp63.ret_typ;_tmp67=_tmp63.args;_tmp68=
_tmp63.c_varargs;_tmp69=_tmp63.cyc_varargs;_tmp6A=_tmp63.rgn_po;_tmp6B=_tmp63.attributes;
_LL3D: {struct Cyc_List_List*_tmp6C=0;{struct Cyc_List_List*_tmp6D=_tmp67;for(0;
_tmp6D != 0;_tmp6D=_tmp6D->tl){struct _tuple2 _tmp6F;struct Cyc_Core_Opt*_tmp70;
struct Cyc_Absyn_Tqual _tmp71;void*_tmp72;struct _tuple2*_tmp6E=(struct _tuple2*)
_tmp6D->hd;_tmp6F=*_tmp6E;_tmp70=_tmp6F.f1;_tmp71=_tmp6F.f2;_tmp72=_tmp6F.f3;if(
_tmp70 == 0){Cyc_Parse_err(_tag_arr("missing argument variable in function prototype",
sizeof(char),48),loc);_tmp6C=({struct Cyc_List_List*_tmp73=_cycalloc(sizeof(*
_tmp73));_tmp73->hd=({struct _tuple10*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->f1=
_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"?",sizeof(char),2);_tmp74->f2=
_tmp71;_tmp74->f3=_tmp72;_tmp74;});_tmp73->tl=_tmp6C;_tmp73;});}else{_tmp6C=({
struct Cyc_List_List*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->hd=({struct
_tuple10*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->f1=(struct _tagged_arr*)_tmp70->v;
_tmp77->f2=_tmp71;_tmp77->f3=_tmp72;_tmp77;});_tmp76->tl=_tmp6C;_tmp76;});}}}
return({struct Cyc_Absyn_Fndecl*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->sc=(
void*)sc;_tmp78->name=d->id;_tmp78->tvs=_tmp64;_tmp78->is_inline=is_inline;
_tmp78->effect=_tmp65;_tmp78->ret_type=(void*)_tmp66;_tmp78->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6C);_tmp78->c_varargs=_tmp68;_tmp78->cyc_varargs=
_tmp69;_tmp78->rgn_po=_tmp6A;_tmp78->body=body;_tmp78->cached_typ=0;_tmp78->param_vardecls=
0;_tmp78->attributes=Cyc_List_append(_tmp6B,_tmp5F);_tmp78;});}_LL3E:;_LL3F:({
void*_tmp79[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Parse_abort)(loc,_tag_arr("declarator is not a function prototype",
sizeof(char),39),_tag_arr(_tmp79,sizeof(void*),0));});_LL3B:;}}}}}static char
_tmp7A[52]="at most one type may appear within a type specifier";static struct
_tagged_arr Cyc_Parse_msg1={_tmp7A,_tmp7A,_tmp7A + 52};static char _tmp7B[63]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={_tmp7B,_tmp7B,_tmp7B + 63};static char
_tmp7C[50]="type specifier includes more than one declaration";static struct
_tagged_arr Cyc_Parse_msg3={_tmp7C,_tmp7C,_tmp7C + 50};static char _tmp7D[60]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={_tmp7D,_tmp7D,_tmp7D + 60};static struct
_tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;
void*t=(void*)0;void*sz=(void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*
last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp7E=(void*)ts->hd;void*_tmp7F;
struct Cyc_Position_Segment*_tmp80;struct Cyc_Position_Segment*_tmp81;struct Cyc_Position_Segment*
_tmp82;struct Cyc_Position_Segment*_tmp83;struct Cyc_Position_Segment*_tmp84;
struct Cyc_Absyn_Decl*_tmp85;_LL41: if(*((int*)_tmp7E)!= 4)goto _LL43;_tmp7F=(void*)((
struct Cyc_Parse_Type_spec_struct*)_tmp7E)->f1;_tmp80=((struct Cyc_Parse_Type_spec_struct*)
_tmp7E)->f2;_LL42: if(seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp80);last_loc=
_tmp80;seen_type=1;t=_tmp7F;goto _LL40;_LL43: if(*((int*)_tmp7E)!= 0)goto _LL45;
_tmp81=((struct Cyc_Parse_Signed_spec_struct*)_tmp7E)->f1;_LL44: if(seen_sign)Cyc_Parse_err(
Cyc_Parse_msg4,_tmp81);if(seen_type)Cyc_Parse_err(_tag_arr("signed qualifier must come before type",
sizeof(char),39),_tmp81);last_loc=_tmp81;seen_sign=1;sgn=(void*)0;goto _LL40;
_LL45: if(*((int*)_tmp7E)!= 1)goto _LL47;_tmp82=((struct Cyc_Parse_Unsigned_spec_struct*)
_tmp7E)->f1;_LL46: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp82);if(seen_type)
Cyc_Parse_err(_tag_arr("signed qualifier must come before type",sizeof(char),39),
_tmp82);last_loc=_tmp82;seen_sign=1;sgn=(void*)1;goto _LL40;_LL47: if(*((int*)
_tmp7E)!= 2)goto _LL49;_tmp83=((struct Cyc_Parse_Short_spec_struct*)_tmp7E)->f1;
_LL48: if(seen_size)Cyc_Parse_err(_tag_arr("integral size may appear only once within a type specifier",
sizeof(char),59),_tmp83);if(seen_type)Cyc_Parse_err(_tag_arr("short modifier must come before base type",
sizeof(char),42),_tmp83);last_loc=_tmp83;seen_size=1;sz=(void*)1;goto _LL40;_LL49:
if(*((int*)_tmp7E)!= 3)goto _LL4B;_tmp84=((struct Cyc_Parse_Long_spec_struct*)
_tmp7E)->f1;_LL4A: if(seen_type)Cyc_Parse_err(_tag_arr("long modifier must come before base type",
sizeof(char),41),_tmp84);if(seen_size){void*_tmp86=sz;_LL4E: if((int)_tmp86 != 2)
goto _LL50;_LL4F: sz=(void*)3;goto _LL4D;_LL50:;_LL51: Cyc_Parse_err(_tag_arr("extra long in type specifier",
sizeof(char),29),_tmp84);goto _LL4D;_LL4D:;}last_loc=_tmp84;seen_size=1;goto _LL40;
_LL4B: if(*((int*)_tmp7E)!= 5)goto _LL40;_tmp85=((struct Cyc_Parse_Decl_spec_struct*)
_tmp7E)->f1;_LL4C: last_loc=_tmp85->loc;if(declopt == 0?!seen_type: 0){seen_type=1;{
void*_tmp87=(void*)_tmp85->r;struct Cyc_Absyn_Aggrdecl*_tmp88;struct Cyc_Absyn_Tuniondecl*
_tmp89;struct Cyc_Absyn_Enumdecl*_tmp8A;_LL53: if(*((int*)_tmp87)!= 4)goto _LL55;
_tmp88=((struct Cyc_Absyn_Aggr_d_struct*)_tmp87)->f1;_LL54: {struct Cyc_List_List*
_tmp8B=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmp88->tvs));t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmp8C=_cycalloc(
sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Absyn_AggrType_struct _tmp8D;_tmp8D.tag=10;
_tmp8D.f1=({struct Cyc_Absyn_AggrInfo _tmp8E;_tmp8E.aggr_info=(void*)((void*)({
struct Cyc_Absyn_UnknownAggr_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({
struct Cyc_Absyn_UnknownAggr_struct _tmp90;_tmp90.tag=0;_tmp90.f1=(void*)((void*)
_tmp88->kind);_tmp90.f2=_tmp88->name;_tmp90;});_tmp8F;}));_tmp8E.targs=_tmp8B;
_tmp8E;});_tmp8D;});_tmp8C;});if(_tmp88->fields != 0)declopt=({struct Cyc_Core_Opt*
_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->v=_tmp85;_tmp91;});goto _LL52;}_LL55:
if(*((int*)_tmp87)!= 5)goto _LL57;_tmp89=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp87)->f1;_LL56: {struct Cyc_List_List*_tmp92=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmp89->tvs));t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[0]=({
struct Cyc_Absyn_TunionType_struct _tmp94;_tmp94.tag=2;_tmp94.f1=({struct Cyc_Absyn_TunionInfo
_tmp95;_tmp95.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp97;_tmp97.tag=1;_tmp97.f1=({struct Cyc_Absyn_Tuniondecl**_tmp98=_cycalloc(
sizeof(*_tmp98));_tmp98[0]=_tmp89;_tmp98;});_tmp97;});_tmp96;}));_tmp95.targs=
_tmp92;_tmp95.rgn=(void*)((void*)2);_tmp95;});_tmp94;});_tmp93;});if(_tmp89->fields
!= 0)declopt=({struct Cyc_Core_Opt*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->v=
_tmp85;_tmp99;});goto _LL52;}_LL57: if(*((int*)_tmp87)!= 6)goto _LL59;_tmp8A=((
struct Cyc_Absyn_Enum_d_struct*)_tmp87)->f1;_LL58: t=(void*)({struct Cyc_Absyn_EnumType_struct*
_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_Absyn_EnumType_struct
_tmp9B;_tmp9B.tag=12;_tmp9B.f1=_tmp8A->name;_tmp9B.f2=0;_tmp9B;});_tmp9A;});
declopt=({struct Cyc_Core_Opt*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->v=_tmp85;
_tmp9C;});goto _LL52;_LL59:;_LL5A:({void*_tmp9D[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmp85->loc,
_tag_arr("bad declaration within type specifier",sizeof(char),38),_tag_arr(
_tmp9D,sizeof(void*),0));});_LL52:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmp85->loc);}
goto _LL40;_LL40:;}if(!seen_type){if(!seen_sign?!seen_size: 0)({void*_tmp9E[0]={};
Cyc_Tcutil_warn(last_loc,_tag_arr("missing type within specifier",sizeof(char),
30),_tag_arr(_tmp9E,sizeof(void*),0));});t=(void*)({struct Cyc_Absyn_IntType_struct*
_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({struct Cyc_Absyn_IntType_struct
_tmpA0;_tmpA0.tag=5;_tmpA0.f1=(void*)sgn;_tmpA0.f2=(void*)sz;_tmpA0;});_tmp9F;});}
else{if(seen_sign){void*_tmpA1=t;void*_tmpA2;_LL5C: if(_tmpA1 <= (void*)3?1:*((int*)
_tmpA1)!= 5)goto _LL5E;_tmpA2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpA1)->f2;
_LL5D: t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));
_tmpA3[0]=({struct Cyc_Absyn_IntType_struct _tmpA4;_tmpA4.tag=5;_tmpA4.f1=(void*)
sgn;_tmpA4.f2=(void*)_tmpA2;_tmpA4;});_tmpA3;});goto _LL5B;_LL5E:;_LL5F: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type",sizeof(char),40),last_loc);
goto _LL5B;_LL5B:;}if(seen_size){void*_tmpA5=t;void*_tmpA6;_LL61: if(_tmpA5 <= (
void*)3?1:*((int*)_tmpA5)!= 5)goto _LL63;_tmpA6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpA5)->f1;_LL62: t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpA7=_cycalloc(
sizeof(*_tmpA7));_tmpA7[0]=({struct Cyc_Absyn_IntType_struct _tmpA8;_tmpA8.tag=5;
_tmpA8.f1=(void*)_tmpA6;_tmpA8.f2=(void*)sz;_tmpA8;});_tmpA7;});goto _LL60;_LL63:
if(_tmpA5 <= (void*)3?1:*((int*)_tmpA5)!= 6)goto _LL65;_LL64: t=(void*)({struct Cyc_Absyn_DoubleType_struct*
_tmpA9=_cycalloc_atomic(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpAA;_tmpAA.tag=6;_tmpAA.f1=1;_tmpAA;});_tmpA9;});goto _LL60;_LL65:;_LL66: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type",sizeof(char),36),last_loc);goto
_LL60;_LL60:;}}return({struct _tuple5 _tmpAB;_tmpAB.f1=t;_tmpAB.f2=declopt;_tmpAB;});}
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct Cyc_Absyn_Tqual tq,void*t,
struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{
struct Cyc_Parse_Declarator*_tmpAC=(struct Cyc_Parse_Declarator*)ds->hd;struct
_tuple1*_tmpAD=_tmpAC->id;struct Cyc_Absyn_Tqual _tmpAF;void*_tmpB0;struct Cyc_List_List*
_tmpB1;struct Cyc_List_List*_tmpB2;struct _tuple6 _tmpAE=Cyc_Parse_apply_tms(tq,t,
shared_atts,_tmpAC->tms);_tmpAF=_tmpAE.f1;_tmpB0=_tmpAE.f2;_tmpB1=_tmpAE.f3;
_tmpB2=_tmpAE.f4;return({struct Cyc_List_List*_tmpB3=_cycalloc(sizeof(*_tmpB3));
_tmpB3->hd=({struct _tuple7*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->f1=_tmpAD;
_tmpB4->f2=_tmpAF;_tmpB4->f3=_tmpB0;_tmpB4->f4=_tmpB1;_tmpB4->f5=_tmpB2;_tmpB4;});
_tmpB3->tl=Cyc_Parse_apply_tmss(_tmpAF,Cyc_Tcutil_copy_type(t),ds->tl,
shared_atts);_tmpB3;});}}static struct _tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){if(tms == 0)return({
struct _tuple6 _tmpB5;_tmpB5.f1=tq;_tmpB5.f2=t;_tmpB5.f3=0;_tmpB5.f4=atts;_tmpB5;});{
void*_tmpB6=(void*)tms->hd;struct Cyc_Absyn_Exp*_tmpB7;void*_tmpB8;struct Cyc_List_List*
_tmpB9;struct Cyc_Position_Segment*_tmpBA;void*_tmpBB;void*_tmpBC;struct Cyc_Absyn_Tqual
_tmpBD;struct Cyc_Position_Segment*_tmpBE;struct Cyc_List_List*_tmpBF;_LL68: if((
int)_tmpB6 != 0)goto _LL6A;_LL69: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),(
void*)({struct Cyc_Absyn_ArrayType_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));
_tmpC0[0]=({struct Cyc_Absyn_ArrayType_struct _tmpC1;_tmpC1.tag=7;_tmpC1.f1=(void*)
t;_tmpC1.f2=tq;_tmpC1.f3=0;_tmpC1;});_tmpC0;}),atts,tms->tl);_LL6A: if(_tmpB6 <= (
void*)1?1:*((int*)_tmpB6)!= 0)goto _LL6C;_tmpB7=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpB6)->f1;_LL6B: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({
struct Cyc_Absyn_ArrayType_struct _tmpC3;_tmpC3.tag=7;_tmpC3.f1=(void*)t;_tmpC3.f2=
tq;_tmpC3.f3=(struct Cyc_Absyn_Exp*)_tmpB7;_tmpC3;});_tmpC2;}),atts,tms->tl);
_LL6C: if(_tmpB6 <= (void*)1?1:*((int*)_tmpB6)!= 2)goto _LL6E;_tmpB8=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmpB6)->f1;_LL6D: {void*_tmpC4=_tmpB8;
struct Cyc_List_List*_tmpC5;int _tmpC6;struct Cyc_Absyn_VarargInfo*_tmpC7;struct Cyc_Core_Opt*
_tmpC8;struct Cyc_List_List*_tmpC9;struct Cyc_Position_Segment*_tmpCA;_LL75: if(*((
int*)_tmpC4)!= 1)goto _LL77;_tmpC5=((struct Cyc_Absyn_WithTypes_struct*)_tmpC4)->f1;
_tmpC6=((struct Cyc_Absyn_WithTypes_struct*)_tmpC4)->f2;_tmpC7=((struct Cyc_Absyn_WithTypes_struct*)
_tmpC4)->f3;_tmpC8=((struct Cyc_Absyn_WithTypes_struct*)_tmpC4)->f4;_tmpC9=((
struct Cyc_Absyn_WithTypes_struct*)_tmpC4)->f5;_LL76: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->hd=(void*)((
void*)as->hd);_tmpCB->tl=fn_atts;_tmpCB;});else{new_atts=({struct Cyc_List_List*
_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->hd=(void*)((void*)as->hd);_tmpCC->tl=
new_atts;_tmpCC;});}}}if(tms->tl != 0){void*_tmpCD=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpCE;_LL7A: if(_tmpCD <= (void*)1?
1:*((int*)_tmpCD)!= 3)goto _LL7C;_tmpCE=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCD)->f1;_LL7B: typvars=_tmpCE;tms=tms->tl;goto _LL79;_LL7C:;_LL7D: goto _LL79;
_LL79:;}if(((((!_tmpC6?_tmpC7 == 0: 0)?_tmpC5 != 0: 0)?_tmpC5->tl == 0: 0)?(*((struct
_tuple2*)_tmpC5->hd)).f1 == 0: 0)?(*((struct _tuple2*)_tmpC5->hd)).f3 == (void*)0: 0)
_tmpC5=0;t=Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpC5);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(),
Cyc_Absyn_function_typ(typvars,_tmpC8,t,_tmpC5,_tmpC6,_tmpC7,_tmpC9,fn_atts),
new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL77: if(*((int*)_tmpC4)
!= 0)goto _LL74;_tmpCA=((struct Cyc_Absyn_NoTypes_struct*)_tmpC4)->f2;_LL78:({void*
_tmpCF[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Parse_abort)(_tmpCA,_tag_arr("function declaration without parameter types",
sizeof(char),45),_tag_arr(_tmpCF,sizeof(void*),0));});_LL74:;}_LL6E: if(_tmpB6 <= (
void*)1?1:*((int*)_tmpB6)!= 3)goto _LL70;_tmpB9=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpB6)->f1;_tmpBA=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpB6)->f2;_LL6F:
if(tms->tl == 0)return({struct _tuple6 _tmpD0;_tmpD0.f1=tq;_tmpD0.f2=t;_tmpD0.f3=
_tmpB9;_tmpD0.f4=atts;_tmpD0;});({void*_tmpD1[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Parse_abort)(_tmpBA,_tag_arr("type parameters must appear before function arguments in declarator",
sizeof(char),68),_tag_arr(_tmpD1,sizeof(void*),0));});_LL70: if(_tmpB6 <= (void*)1?
1:*((int*)_tmpB6)!= 1)goto _LL72;_tmpBB=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpB6)->f1;_tmpBC=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpB6)->f2;
_tmpBD=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpB6)->f3;_LL71: {void*_tmpD2=
_tmpBB;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD4;_LL7F: if(_tmpD2 <= (
void*)1?1:*((int*)_tmpD2)!= 0)goto _LL81;_tmpD3=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmpD2)->f1;_LL80: return Cyc_Parse_apply_tms(_tmpBD,Cyc_Absyn_atb_typ(t,_tmpBC,tq,(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[
0]=({struct Cyc_Absyn_Upper_b_struct _tmpD6;_tmpD6.tag=0;_tmpD6.f1=_tmpD3;_tmpD6;});
_tmpD5;})),atts,tms->tl);_LL81: if(_tmpD2 <= (void*)1?1:*((int*)_tmpD2)!= 1)goto
_LL83;_tmpD4=((struct Cyc_Absyn_Nullable_ps_struct*)_tmpD2)->f1;_LL82: return Cyc_Parse_apply_tms(
_tmpBD,Cyc_Absyn_starb_typ(t,_tmpBC,tq,(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7[0]=({struct Cyc_Absyn_Upper_b_struct
_tmpD8;_tmpD8.tag=0;_tmpD8.f1=_tmpD4;_tmpD8;});_tmpD7;})),atts,tms->tl);_LL83:
if((int)_tmpD2 != 0)goto _LL7E;_LL84: return Cyc_Parse_apply_tms(_tmpBD,Cyc_Absyn_tagged_typ(
t,_tmpBC,tq),atts,tms->tl);_LL7E:;}_LL72: if(_tmpB6 <= (void*)1?1:*((int*)_tmpB6)
!= 4)goto _LL67;_tmpBE=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpB6)->f1;
_tmpBF=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpB6)->f2;_LL73: return Cyc_Parse_apply_tms(
tq,t,Cyc_List_append(atts,_tmpBF),tms->tl);_LL67:;}}void*Cyc_Parse_speclist2typ(
struct Cyc_List_List*tss,struct Cyc_Position_Segment*loc){void*_tmpDA;struct Cyc_Core_Opt*
_tmpDB;struct _tuple5 _tmpD9=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmpDA=
_tmpD9.f1;_tmpDB=_tmpD9.f2;if(_tmpDB != 0)({void*_tmpDC[0]={};Cyc_Tcutil_warn(loc,
_tag_arr("ignoring nested type declaration(s) in specifier list",sizeof(char),54),
_tag_arr(_tmpDC,sizeof(void*),0));});return _tmpDA;}static struct Cyc_Absyn_Decl*
Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*loc,struct _tuple7*t){
struct _tuple7 _tmpDE;struct _tuple1*_tmpDF;struct Cyc_Absyn_Tqual _tmpE0;void*_tmpE1;
struct Cyc_List_List*_tmpE2;struct Cyc_List_List*_tmpE3;struct _tuple7*_tmpDD=t;
_tmpDE=*_tmpDD;_tmpDF=_tmpDE.f1;_tmpE0=_tmpDE.f2;_tmpE1=_tmpDE.f3;_tmpE2=_tmpDE.f4;
_tmpE3=_tmpDE.f5;Cyc_Lex_register_typedef(_tmpDF);if(_tmpE3 != 0)Cyc_Parse_err((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpE5;_tmpE5.tag=0;_tmpE5.f1=(
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)_tmpE3->hd);{void*_tmpE4[1]={&
_tmpE5};Cyc_Std_aprintf(_tag_arr("bad attribute %s within typedef",sizeof(char),
32),_tag_arr(_tmpE4,sizeof(void*),1));}}),loc);{struct Cyc_Core_Opt*kind;struct
Cyc_Core_Opt*type;{void*_tmpE6=_tmpE1;struct Cyc_Core_Opt*_tmpE7;_LL86: if(_tmpE6
<= (void*)3?1:*((int*)_tmpE6)!= 0)goto _LL88;_tmpE7=((struct Cyc_Absyn_Evar_struct*)
_tmpE6)->f1;_LL87: type=0;if(_tmpE7 == 0)kind=({struct Cyc_Core_Opt*_tmpE8=
_cycalloc(sizeof(*_tmpE8));_tmpE8->v=(void*)((void*)2);_tmpE8;});else{kind=
_tmpE7;}goto _LL85;_LL88:;_LL89: kind=0;type=({struct Cyc_Core_Opt*_tmpE9=_cycalloc(
sizeof(*_tmpE9));_tmpE9->v=(void*)_tmpE1;_tmpE9;});goto _LL85;_LL85:;}return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Typedef_d_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));
_tmpEA[0]=({struct Cyc_Absyn_Typedef_d_struct _tmpEB;_tmpEB.tag=7;_tmpEB.f1=({
struct Cyc_Absyn_Typedefdecl*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->name=
_tmpDF;_tmpEC->tvs=_tmpE2;_tmpEC->kind=kind;_tmpEC->defn=type;_tmpEC;});_tmpEB;});
_tmpEA;}),loc);}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*
d,struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpEE;_tmpEE.tag=12;_tmpEE.f1=d;_tmpEE.f2=s;_tmpEE;});_tmpED;}),Cyc_Position_segment_join(
d->loc,s->loc));}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*loc){struct Cyc_Parse_Declaration_spec
_tmpF0;struct Cyc_Absyn_Tqual _tmpF1;struct Cyc_List_List*_tmpF2;struct Cyc_List_List*
_tmpF3;struct Cyc_Parse_Declaration_spec*_tmpEF=ds;_tmpF0=*_tmpEF;_tmpF1=_tmpF0.tq;
_tmpF2=_tmpF0.type_specs;_tmpF3=_tmpF0.attributes;if(ds->is_inline)Cyc_Parse_err(
_tag_arr("inline is allowed only on function definitions",sizeof(char),47),loc);
if(_tmpF2 == 0){Cyc_Parse_err(_tag_arr("missing type specifiers in declaration",
sizeof(char),39),loc);return 0;}{void*s=(void*)2;int istypedef=0;if(ds->sc != 0){
void*_tmpF4=(void*)((struct Cyc_Core_Opt*)_check_null(ds->sc))->v;_LL8B: if((int)
_tmpF4 != 0)goto _LL8D;_LL8C: istypedef=1;goto _LL8A;_LL8D: if((int)_tmpF4 != 1)goto
_LL8F;_LL8E: s=(void*)3;goto _LL8A;_LL8F: if((int)_tmpF4 != 2)goto _LL91;_LL90: s=(
void*)4;goto _LL8A;_LL91: if((int)_tmpF4 != 3)goto _LL93;_LL92: s=(void*)0;goto _LL8A;
_LL93: if((int)_tmpF4 != 4)goto _LL95;_LL94: s=(void*)2;goto _LL8A;_LL95: if((int)
_tmpF4 != 5)goto _LL97;_LL96: s=(void*)2;goto _LL8A;_LL97: if((int)_tmpF4 != 6)goto
_LL8A;_LL98: s=(void*)1;goto _LL8A;_LL8A:;}{struct Cyc_List_List*_tmpF6;struct Cyc_List_List*
_tmpF7;struct _tuple0 _tmpF5=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
ids);_tmpF6=_tmpF5.f1;_tmpF7=_tmpF5.f2;{int exps_empty=1;{struct Cyc_List_List*es=
_tmpF7;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){exps_empty=0;
break;}}}{struct _tuple5 _tmpF8=Cyc_Parse_collapse_type_specifiers(_tmpF2,loc);if(
_tmpF6 == 0){void*_tmpFA;struct Cyc_Core_Opt*_tmpFB;struct _tuple5 _tmpF9=_tmpF8;
_tmpFA=_tmpF9.f1;_tmpFB=_tmpF9.f2;if(_tmpFB != 0){struct Cyc_Absyn_Decl*d=(struct
Cyc_Absyn_Decl*)_tmpFB->v;{void*_tmpFC=(void*)d->r;struct Cyc_Absyn_Enumdecl*
_tmpFD;struct Cyc_Absyn_Aggrdecl*_tmpFE;struct Cyc_Absyn_Tuniondecl*_tmpFF;_LL9A:
if(*((int*)_tmpFC)!= 6)goto _LL9C;_tmpFD=((struct Cyc_Absyn_Enum_d_struct*)_tmpFC)->f1;
_LL9B:(void*)(_tmpFD->sc=(void*)s);if(_tmpF3 != 0)Cyc_Parse_err(_tag_arr("bad attributes on enum",
sizeof(char),23),loc);goto _LL99;_LL9C: if(*((int*)_tmpFC)!= 4)goto _LL9E;_tmpFE=((
struct Cyc_Absyn_Aggr_d_struct*)_tmpFC)->f1;_LL9D:(void*)(_tmpFE->sc=(void*)s);
_tmpFE->attributes=_tmpF3;goto _LL99;_LL9E: if(*((int*)_tmpFC)!= 5)goto _LLA0;
_tmpFF=((struct Cyc_Absyn_Tunion_d_struct*)_tmpFC)->f1;_LL9F:(void*)(_tmpFF->sc=(
void*)s);if(_tmpF3 != 0)Cyc_Parse_err(_tag_arr("bad attributes on tunion",sizeof(
char),25),loc);goto _LL99;_LLA0:;_LLA1: Cyc_Parse_err(_tag_arr("bad declaration",
sizeof(char),16),loc);return 0;_LL99:;}return({struct Cyc_List_List*_tmp100=
_cycalloc(sizeof(*_tmp100));_tmp100->hd=d;_tmp100->tl=0;_tmp100;});}else{void*
_tmp101=_tmpFA;struct Cyc_Absyn_AggrInfo _tmp102;void*_tmp103;void*_tmp104;struct
_tuple1*_tmp105;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_TunionInfo _tmp107;
void*_tmp108;struct Cyc_Absyn_Tuniondecl**_tmp109;struct Cyc_Absyn_TunionInfo
_tmp10A;void*_tmp10B;struct Cyc_Absyn_UnknownTunionInfo _tmp10C;struct _tuple1*
_tmp10D;int _tmp10E;struct Cyc_List_List*_tmp10F;struct _tuple1*_tmp110;struct Cyc_List_List*
_tmp111;_LLA3: if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 10)goto _LLA5;_tmp102=((
struct Cyc_Absyn_AggrType_struct*)_tmp101)->f1;_tmp103=(void*)_tmp102.aggr_info;
if(*((int*)_tmp103)!= 0)goto _LLA5;_tmp104=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp103)->f1;_tmp105=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp103)->f2;_tmp106=
_tmp102.targs;_LLA4: {struct Cyc_List_List*_tmp112=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp106);
struct Cyc_Absyn_Aggrdecl*_tmp113=({struct Cyc_Absyn_Aggrdecl*_tmp117=_cycalloc(
sizeof(*_tmp117));_tmp117->kind=(void*)_tmp104;_tmp117->sc=(void*)s;_tmp117->name=
_tmp105;_tmp117->tvs=_tmp112;_tmp117->exist_vars=0;_tmp117->fields=0;_tmp117->attributes=
0;_tmp117;});if(_tmpF3 != 0)Cyc_Parse_err(_tag_arr("bad attributes on type declaration",
sizeof(char),35),loc);return({struct Cyc_List_List*_tmp114=_cycalloc(sizeof(*
_tmp114));_tmp114->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp116;_tmp116.tag=4;_tmp116.f1=_tmp113;_tmp116;});_tmp115;}),loc);_tmp114->tl=
0;_tmp114;});}_LLA5: if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 2)goto _LLA7;
_tmp107=((struct Cyc_Absyn_TunionType_struct*)_tmp101)->f1;_tmp108=(void*)_tmp107.tunion_info;
if(*((int*)_tmp108)!= 1)goto _LLA7;_tmp109=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp108)->f1;_LLA6: if(_tmpF3 != 0)Cyc_Parse_err(_tag_arr("bad attributes on tunion",
sizeof(char),25),loc);return({struct Cyc_List_List*_tmp118=_cycalloc(sizeof(*
_tmp118));_tmp118->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp11A;_tmp11A.tag=5;_tmp11A.f1=*_tmp109;_tmp11A;});_tmp119;}),loc);_tmp118->tl=
0;_tmp118;});_LLA7: if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 2)goto _LLA9;
_tmp10A=((struct Cyc_Absyn_TunionType_struct*)_tmp101)->f1;_tmp10B=(void*)_tmp10A.tunion_info;
if(*((int*)_tmp10B)!= 0)goto _LLA9;_tmp10C=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp10B)->f1;_tmp10D=_tmp10C.name;_tmp10E=_tmp10C.is_xtunion;_tmp10F=_tmp10A.targs;
_LLA8: {struct Cyc_List_List*_tmp11B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp10F);struct Cyc_Absyn_Decl*_tmp11C=
Cyc_Absyn_tunion_decl(s,_tmp10D,_tmp11B,0,_tmp10E,loc);if(_tmpF3 != 0)Cyc_Parse_err(
_tag_arr("bad attributes on tunion",sizeof(char),25),loc);return({struct Cyc_List_List*
_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->hd=_tmp11C;_tmp11D->tl=0;_tmp11D;});}
_LLA9: if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 12)goto _LLAB;_tmp110=((struct
Cyc_Absyn_EnumType_struct*)_tmp101)->f1;_LLAA: {struct Cyc_Absyn_Enumdecl*_tmp11E=({
struct Cyc_Absyn_Enumdecl*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->sc=(void*)
s;_tmp123->name=_tmp110;_tmp123->fields=0;_tmp123;});if(_tmpF3 != 0)Cyc_Parse_err(
_tag_arr("bad attributes on enum",sizeof(char),23),loc);return({struct Cyc_List_List*
_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->hd=({struct Cyc_Absyn_Decl*_tmp120=
_cycalloc(sizeof(*_tmp120));_tmp120->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp122;_tmp122.tag=6;_tmp122.f1=_tmp11E;_tmp122;});_tmp121;}));_tmp120->loc=loc;
_tmp120;});_tmp11F->tl=0;_tmp11F;});}_LLAB: if(_tmp101 <= (void*)3?1:*((int*)
_tmp101)!= 13)goto _LLAD;_tmp111=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp101)->f1;
_LLAC: {struct Cyc_Absyn_Enumdecl*_tmp124=({struct Cyc_Absyn_Enumdecl*_tmp129=
_cycalloc(sizeof(*_tmp129));_tmp129->sc=(void*)s;_tmp129->name=Cyc_Parse_gensym_enum();
_tmp129->fields=({struct Cyc_Core_Opt*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->v=
_tmp111;_tmp12A;});_tmp129;});if(_tmpF3 != 0)Cyc_Parse_err(_tag_arr("bad attributes on enum",
sizeof(char),23),loc);return({struct Cyc_List_List*_tmp125=_cycalloc(sizeof(*
_tmp125));_tmp125->hd=({struct Cyc_Absyn_Decl*_tmp126=_cycalloc(sizeof(*_tmp126));
_tmp126->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp127=_cycalloc(
sizeof(*_tmp127));_tmp127[0]=({struct Cyc_Absyn_Enum_d_struct _tmp128;_tmp128.tag=
6;_tmp128.f1=_tmp124;_tmp128;});_tmp127;}));_tmp126->loc=loc;_tmp126;});_tmp125->tl=
0;_tmp125;});}_LLAD:;_LLAE: Cyc_Parse_err(_tag_arr("missing declarator",sizeof(
char),19),loc);return 0;_LLA2:;}}else{void*t=_tmpF8.f1;struct Cyc_List_List*
_tmp12B=Cyc_Parse_apply_tmss(_tmpF1,t,_tmpF6,_tmpF3);if(istypedef){if(!
exps_empty)Cyc_Parse_err(_tag_arr("initializer in typedef declaration",sizeof(
char),35),loc);{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*
f)(struct Cyc_Position_Segment*,struct _tuple7*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp12B);
if(_tmpF8.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(_tmpF8.f2)->v;{
void*_tmp12C=(void*)d->r;struct Cyc_Absyn_Aggrdecl*_tmp12D;struct Cyc_Absyn_Tuniondecl*
_tmp12E;struct Cyc_Absyn_Enumdecl*_tmp12F;_LLB0: if(*((int*)_tmp12C)!= 4)goto _LLB2;
_tmp12D=((struct Cyc_Absyn_Aggr_d_struct*)_tmp12C)->f1;_LLB1:(void*)(_tmp12D->sc=(
void*)s);_tmp12D->attributes=_tmpF3;_tmpF3=0;goto _LLAF;_LLB2: if(*((int*)_tmp12C)
!= 5)goto _LLB4;_tmp12E=((struct Cyc_Absyn_Tunion_d_struct*)_tmp12C)->f1;_LLB3:(
void*)(_tmp12E->sc=(void*)s);goto _LLAF;_LLB4: if(*((int*)_tmp12C)!= 6)goto _LLB6;
_tmp12F=((struct Cyc_Absyn_Enum_d_struct*)_tmp12C)->f1;_LLB5:(void*)(_tmp12F->sc=(
void*)s);goto _LLAF;_LLB6:;_LLB7: Cyc_Parse_err(_tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof(char),70),loc);goto _LLAF;_LLAF:;}decls=({struct Cyc_List_List*_tmp130=
_cycalloc(sizeof(*_tmp130));_tmp130->hd=d;_tmp130->tl=decls;_tmp130;});}if(
_tmpF3 != 0)Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp132;_tmp132.tag=0;_tmp132.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmpF3->hd);{void*_tmp131[1]={& _tmp132};Cyc_Std_aprintf(_tag_arr("bad attribute %s in typedef",
sizeof(char),28),_tag_arr(_tmp131,sizeof(void*),1));}}),loc);return decls;}}else{
if(_tmpF8.f2 != 0)Cyc_Parse_unimp(_tag_arr("nested type declaration within declarator",
sizeof(char),42),loc);{struct Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp133=
_tmp12B;for(0;_tmp133 != 0;(_tmp133=_tmp133->tl,_tmpF7=_tmpF7->tl)){struct _tuple7
_tmp135;struct _tuple1*_tmp136;struct Cyc_Absyn_Tqual _tmp137;void*_tmp138;struct
Cyc_List_List*_tmp139;struct Cyc_List_List*_tmp13A;struct _tuple7*_tmp134=(struct
_tuple7*)_tmp133->hd;_tmp135=*_tmp134;_tmp136=_tmp135.f1;_tmp137=_tmp135.f2;
_tmp138=_tmp135.f3;_tmp139=_tmp135.f4;_tmp13A=_tmp135.f5;if(_tmp139 != 0)({void*
_tmp13B[0]={};Cyc_Tcutil_warn(loc,_tag_arr("bad type params, ignoring",sizeof(
char),26),_tag_arr(_tmp13B,sizeof(void*),0));});if(_tmpF7 == 0)({void*_tmp13C[0]={};((
int(*)(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Parse_abort)(loc,_tag_arr("unexpected NULL in parse!",sizeof(char),26),
_tag_arr(_tmp13C,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp13D=(struct Cyc_Absyn_Exp*)
_tmpF7->hd;struct Cyc_Absyn_Vardecl*_tmp13E=Cyc_Absyn_new_vardecl(_tmp136,_tmp138,
_tmp13D);_tmp13E->tq=_tmp137;(void*)(_tmp13E->sc=(void*)s);_tmp13E->attributes=
_tmp13A;{struct Cyc_Absyn_Decl*_tmp13F=({struct Cyc_Absyn_Decl*_tmp141=_cycalloc(
sizeof(*_tmp141));_tmp141->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142[0]=({struct Cyc_Absyn_Var_d_struct
_tmp143;_tmp143.tag=0;_tmp143.f1=_tmp13E;_tmp143;});_tmp142;}));_tmp141->loc=loc;
_tmp141;});decls=({struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));
_tmp140->hd=_tmp13F;_tmp140->tl=decls;_tmp140;});}}}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);}}}}}}}}static void*Cyc_Parse_id_to_kind(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){if(Cyc_Std_strlen(s)== 1)
switch(*((const char*)_check_unknown_subscript(s,sizeof(char),0))){case 'A': _LLB8:
return(void*)0;case 'M': _LLB9: return(void*)1;case 'B': _LLBA: return(void*)2;case 'R':
_LLBB: return(void*)3;case 'E': _LLBC: return(void*)4;case 'I': _LLBD: return(void*)5;
default: _LLBE: break;}Cyc_Parse_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp145;_tmp145.tag=0;_tmp145.f1=(struct _tagged_arr)s;{void*_tmp144[1]={& _tmp145};
Cyc_Std_aprintf(_tag_arr("bad kind: %s",sizeof(char),13),_tag_arr(_tmp144,
sizeof(void*),1));}}),loc);return(void*)2;}static struct Cyc_List_List*Cyc_Parse_attopt_to_tms(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
if(atts == 0)return tms;else{return({struct Cyc_List_List*_tmp146=_cycalloc(sizeof(*
_tmp146));_tmp146->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp148;_tmp148.tag=4;_tmp148.f1=loc;_tmp148.f2=atts;_tmp148;});_tmp147;}));
_tmp146->tl=tms;_tmp146;});}}char Cyc_AbstractDeclarator_tok[27]="\000\000\000\000AbstractDeclarator_tok";
struct Cyc_AbstractDeclarator_tok_struct{char*tag;struct Cyc_Parse_Abstractdeclarator*
f1;};char Cyc_AggrFieldDeclListList_tok[30]="\000\000\000\000AggrFieldDeclListList_tok";
struct Cyc_AggrFieldDeclListList_tok_struct{char*tag;struct Cyc_List_List*f1;};
char Cyc_AggrFieldDeclList_tok[26]="\000\000\000\000AggrFieldDeclList_tok";struct
Cyc_AggrFieldDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_AttributeList_tok[
22]="\000\000\000\000AttributeList_tok";struct Cyc_AttributeList_tok_struct{char*
tag;struct Cyc_List_List*f1;};char Cyc_Attribute_tok[18]="\000\000\000\000Attribute_tok";
struct Cyc_Attribute_tok_struct{char*tag;void*f1;};char Cyc_Bool_tok[13]="\000\000\000\000Bool_tok";
struct Cyc_Bool_tok_struct{char*tag;int f1;};char Cyc_Char_tok[13]="\000\000\000\000Char_tok";
struct Cyc_Char_tok_struct{char*tag;char f1;};char Cyc_DeclList_tok[17]="\000\000\000\000DeclList_tok";
struct Cyc_DeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_DeclSpec_tok[
17]="\000\000\000\000DeclSpec_tok";struct Cyc_DeclSpec_tok_struct{char*tag;struct
Cyc_Parse_Declaration_spec*f1;};char Cyc_Declarator_tok[19]="\000\000\000\000Declarator_tok";
struct Cyc_Declarator_tok_struct{char*tag;struct Cyc_Parse_Declarator*f1;};char Cyc_DesignatorList_tok[
23]="\000\000\000\000DesignatorList_tok";struct Cyc_DesignatorList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_Designator_tok[19]="\000\000\000\000Designator_tok";
struct Cyc_Designator_tok_struct{char*tag;void*f1;};char Cyc_EnumfieldList_tok[22]="\000\000\000\000EnumfieldList_tok";
struct Cyc_EnumfieldList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Enumfield_tok[
18]="\000\000\000\000Enumfield_tok";struct Cyc_Enumfield_tok_struct{char*tag;
struct Cyc_Absyn_Enumfield*f1;};char Cyc_ExpList_tok[16]="\000\000\000\000ExpList_tok";
struct Cyc_ExpList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Exp_tok[
12]="\000\000\000\000Exp_tok";struct Cyc_Exp_tok_struct{char*tag;struct Cyc_Absyn_Exp*
f1;};char Cyc_FieldPatternList_tok[25]="\000\000\000\000FieldPatternList_tok";
struct Cyc_FieldPatternList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_FieldPattern_tok[
21]="\000\000\000\000FieldPattern_tok";struct _tuple11{struct Cyc_List_List*f1;
struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{char*tag;struct
_tuple11*f1;};char Cyc_FnDecl_tok[15]="\000\000\000\000FnDecl_tok";struct Cyc_FnDecl_tok_struct{
char*tag;struct Cyc_Absyn_Fndecl*f1;};char Cyc_IdList_tok[15]="\000\000\000\000IdList_tok";
struct Cyc_IdList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_InitDeclList_tok[
21]="\000\000\000\000InitDeclList_tok";struct Cyc_InitDeclList_tok_struct{char*
tag;struct Cyc_List_List*f1;};char Cyc_InitDecl_tok[17]="\000\000\000\000InitDecl_tok";
struct _tuple12{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
char*tag;struct _tuple12*f1;};char Cyc_InitializerList_tok[24]="\000\000\000\000InitializerList_tok";
struct Cyc_InitializerList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Int_tok[
12]="\000\000\000\000Int_tok";struct _tuple13{void*f1;int f2;};struct Cyc_Int_tok_struct{
char*tag;struct _tuple13*f1;};char Cyc_Kind_tok[13]="\000\000\000\000Kind_tok";
struct Cyc_Kind_tok_struct{char*tag;void*f1;};char Cyc_ParamDeclListBool_tok[26]="\000\000\000\000ParamDeclListBool_tok";
struct _tuple14{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct
Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_ParamDeclListBool_tok_struct{
char*tag;struct _tuple14*f1;};char Cyc_ParamDeclList_tok[22]="\000\000\000\000ParamDeclList_tok";
struct Cyc_ParamDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_ParamDecl_tok[
18]="\000\000\000\000ParamDecl_tok";struct Cyc_ParamDecl_tok_struct{char*tag;
struct _tuple2*f1;};char Cyc_PatternList_tok[20]="\000\000\000\000PatternList_tok";
struct Cyc_PatternList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Pattern_tok[
16]="\000\000\000\000Pattern_tok";struct Cyc_Pattern_tok_struct{char*tag;struct
Cyc_Absyn_Pat*f1;};char Cyc_Pointer_Sort_tok[21]="\000\000\000\000Pointer_Sort_tok";
struct Cyc_Pointer_Sort_tok_struct{char*tag;void*f1;};char Cyc_Primop_tok[15]="\000\000\000\000Primop_tok";
struct Cyc_Primop_tok_struct{char*tag;void*f1;};char Cyc_Primopopt_tok[18]="\000\000\000\000Primopopt_tok";
struct Cyc_Primopopt_tok_struct{char*tag;struct Cyc_Core_Opt*f1;};char Cyc_QualId_tok[
15]="\000\000\000\000QualId_tok";struct Cyc_QualId_tok_struct{char*tag;struct
_tuple1*f1;};char Cyc_QualSpecList_tok[21]="\000\000\000\000QualSpecList_tok";
struct _tuple15{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct Cyc_QualSpecList_tok_struct{char*tag;struct _tuple15*f1;};char Cyc_Rgnorder_tok[
17]="\000\000\000\000Rgnorder_tok";struct Cyc_Rgnorder_tok_struct{char*tag;struct
Cyc_List_List*f1;};char Cyc_Scope_tok[14]="\000\000\000\000Scope_tok";struct Cyc_Scope_tok_struct{
char*tag;void*f1;};char Cyc_Short_tok[14]="\000\000\000\000Short_tok";struct Cyc_Short_tok_struct{
char*tag;short f1;};char Cyc_Stmt_tok[13]="\000\000\000\000Stmt_tok";struct Cyc_Stmt_tok_struct{
char*tag;struct Cyc_Absyn_Stmt*f1;};char Cyc_StorageClass_tok[21]="\000\000\000\000StorageClass_tok";
struct Cyc_StorageClass_tok_struct{char*tag;void*f1;};char Cyc_String_tok[15]="\000\000\000\000String_tok";
struct Cyc_String_tok_struct{char*tag;struct _tagged_arr f1;};char Cyc_Stringopt_tok[
18]="\000\000\000\000Stringopt_tok";struct Cyc_Stringopt_tok_struct{char*tag;
struct Cyc_Core_Opt*f1;};char Cyc_StructOrUnion_tok[22]="\000\000\000\000StructOrUnion_tok";
struct Cyc_StructOrUnion_tok_struct{char*tag;void*f1;};char Cyc_SwitchCClauseList_tok[
26]="\000\000\000\000SwitchCClauseList_tok";struct Cyc_SwitchCClauseList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_SwitchClauseList_tok[25]="\000\000\000\000SwitchClauseList_tok";
struct Cyc_SwitchClauseList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_TunionFieldList_tok[
24]="\000\000\000\000TunionFieldList_tok";struct Cyc_TunionFieldList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_TunionField_tok[20]="\000\000\000\000TunionField_tok";
struct Cyc_TunionField_tok_struct{char*tag;struct Cyc_Absyn_Tunionfield*f1;};char
Cyc_TypeList_tok[17]="\000\000\000\000TypeList_tok";struct Cyc_TypeList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_TypeModifierList_tok[25]="\000\000\000\000TypeModifierList_tok";
struct Cyc_TypeModifierList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_TypeOpt_tok[
16]="\000\000\000\000TypeOpt_tok";struct Cyc_TypeOpt_tok_struct{char*tag;struct
Cyc_Core_Opt*f1;};char Cyc_TypeQual_tok[17]="\000\000\000\000TypeQual_tok";struct
Cyc_TypeQual_tok_struct{char*tag;struct Cyc_Absyn_Tqual f1;};char Cyc_TypeSpecifier_tok[
22]="\000\000\000\000TypeSpecifier_tok";struct Cyc_TypeSpecifier_tok_struct{char*
tag;void*f1;};char Cyc_Type_tok[13]="\000\000\000\000Type_tok";struct Cyc_Type_tok_struct{
char*tag;void*f1;};char Cyc_YY1[8]="\000\000\000\000YY1";struct Cyc_YY1_struct{
char*tag;struct _tuple14*f1;};char Cyc_YYINITIALSVAL[18]="\000\000\000\000YYINITIALSVAL";
char Cyc_Okay_tok[13]="\000\000\000\000Okay_tok";static char _tmp14A[15]="$(sign_t,int)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp14A,
_tmp14A,_tmp14A + 15}};struct _tuple13*Cyc_yyget_Int_tok(void*yy1){struct _tuple13*
yyzzz;{void*_tmp14B=yy1;struct _tuple13*_tmp14C;_LLC1: if(*((void**)_tmp14B)!= Cyc_Int_tok)
goto _LLC3;_tmp14C=((struct Cyc_Int_tok_struct*)_tmp14B)->f1;_LLC2: yyzzz=_tmp14C;
goto _LLC0;_LLC3:;_LLC4:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLC0:;}return
yyzzz;}static char _tmp14E[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp14E,_tmp14E,_tmp14E + 5}};char Cyc_yyget_Char_tok(void*yy1){
char yyzzz;{void*_tmp14F=yy1;char _tmp150;_LLC6: if(*((void**)_tmp14F)!= Cyc_Char_tok)
goto _LLC8;_tmp150=((struct Cyc_Char_tok_struct*)_tmp14F)->f1;_LLC7: yyzzz=_tmp150;
goto _LLC5;_LLC8:;_LLC9:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLC5:;}return
yyzzz;}static char _tmp152[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp152,_tmp152,_tmp152 + 9}};struct _tagged_arr Cyc_yyget_String_tok(
void*yy1){struct _tagged_arr yyzzz;{void*_tmp153=yy1;struct _tagged_arr _tmp154;
_LLCB: if(*((void**)_tmp153)!= Cyc_String_tok)goto _LLCD;_tmp154=((struct Cyc_String_tok_struct*)
_tmp153)->f1;_LLCC: yyzzz=_tmp154;goto _LLCA;_LLCD:;_LLCE:(int)_throw((void*)& Cyc_yyfail_String_tok);
_LLCA:;}return yyzzz;}static char _tmp156[20]="tunion Pointer_Sort";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={Cyc_Core_Failure,{_tmp156,_tmp156,_tmp156 + 20}};
void*Cyc_yyget_Pointer_Sort_tok(void*yy1){void*yyzzz;{void*_tmp157=yy1;void*
_tmp158;_LLD0: if(*((void**)_tmp157)!= Cyc_Pointer_Sort_tok)goto _LLD2;_tmp158=(
void*)((struct Cyc_Pointer_Sort_tok_struct*)_tmp157)->f1;_LLD1: yyzzz=_tmp158;goto
_LLCF;_LLD2:;_LLD3:(int)_throw((void*)& Cyc_yyfail_Pointer_Sort_tok);_LLCF:;}
return yyzzz;}static char _tmp15A[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{_tmp15A,_tmp15A,_tmp15A + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(
void*yy1){struct Cyc_Absyn_Exp*yyzzz;{void*_tmp15B=yy1;struct Cyc_Absyn_Exp*
_tmp15C;_LLD5: if(*((void**)_tmp15B)!= Cyc_Exp_tok)goto _LLD7;_tmp15C=((struct Cyc_Exp_tok_struct*)
_tmp15B)->f1;_LLD6: yyzzz=_tmp15C;goto _LLD4;_LLD7:;_LLD8:(int)_throw((void*)& Cyc_yyfail_Exp_tok);
_LLD4:;}return yyzzz;}static char _tmp15E[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={Cyc_Core_Failure,{_tmp15E,_tmp15E,_tmp15E + 14}};struct
Cyc_List_List*Cyc_yyget_ExpList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp15F=yy1;struct Cyc_List_List*_tmp160;_LLDA: if(*((void**)_tmp15F)!= Cyc_ExpList_tok)
goto _LLDC;_tmp160=((struct Cyc_ExpList_tok_struct*)_tmp15F)->f1;_LLDB: yyzzz=
_tmp160;goto _LLD9;_LLDC:;_LLDD:(int)_throw((void*)& Cyc_yyfail_ExpList_tok);_LLD9:;}
return yyzzz;}static char _tmp162[39]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{
_tmp162,_tmp162,_tmp162 + 39}};struct Cyc_List_List*Cyc_yyget_InitializerList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp163=yy1;struct Cyc_List_List*
_tmp164;_LLDF: if(*((void**)_tmp163)!= Cyc_InitializerList_tok)goto _LLE1;_tmp164=((
struct Cyc_InitializerList_tok_struct*)_tmp163)->f1;_LLE0: yyzzz=_tmp164;goto _LLDE;
_LLE1:;_LLE2:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);_LLDE:;}return
yyzzz;}static char _tmp166[9]="primop_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Primop_tok={
Cyc_Core_Failure,{_tmp166,_tmp166,_tmp166 + 9}};void*Cyc_yyget_Primop_tok(void*
yy1){void*yyzzz;{void*_tmp167=yy1;void*_tmp168;_LLE4: if(*((void**)_tmp167)!= Cyc_Primop_tok)
goto _LLE6;_tmp168=(void*)((struct Cyc_Primop_tok_struct*)_tmp167)->f1;_LLE5: yyzzz=
_tmp168;goto _LLE3;_LLE6:;_LLE7:(int)_throw((void*)& Cyc_yyfail_Primop_tok);_LLE3:;}
return yyzzz;}static char _tmp16A[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp16A,_tmp16A,_tmp16A + 16}};struct
Cyc_Core_Opt*Cyc_yyget_Primopopt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp16B=yy1;struct Cyc_Core_Opt*_tmp16C;_LLE9: if(*((void**)_tmp16B)!= Cyc_Primopopt_tok)
goto _LLEB;_tmp16C=((struct Cyc_Primopopt_tok_struct*)_tmp16B)->f1;_LLEA: yyzzz=
_tmp16C;goto _LLE8;_LLEB:;_LLEC:(int)_throw((void*)& Cyc_yyfail_Primopopt_tok);
_LLE8:;}return yyzzz;}static char _tmp16E[7]="qvar_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp16E,_tmp16E,_tmp16E + 7}};struct
_tuple1*Cyc_yyget_QualId_tok(void*yy1){struct _tuple1*yyzzz;{void*_tmp16F=yy1;
struct _tuple1*_tmp170;_LLEE: if(*((void**)_tmp16F)!= Cyc_QualId_tok)goto _LLF0;
_tmp170=((struct Cyc_QualId_tok_struct*)_tmp16F)->f1;_LLEF: yyzzz=_tmp170;goto
_LLED;_LLF0:;_LLF1:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LLED:;}return
yyzzz;}static char _tmp172[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,{_tmp172,_tmp172,_tmp172 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(
void*yy1){struct Cyc_Absyn_Stmt*yyzzz;{void*_tmp173=yy1;struct Cyc_Absyn_Stmt*
_tmp174;_LLF3: if(*((void**)_tmp173)!= Cyc_Stmt_tok)goto _LLF5;_tmp174=((struct Cyc_Stmt_tok_struct*)
_tmp173)->f1;_LLF4: yyzzz=_tmp174;goto _LLF2;_LLF5:;_LLF6:(int)_throw((void*)& Cyc_yyfail_Stmt_tok);
_LLF2:;}return yyzzz;}static char _tmp176[24]="list_t<switch_clause_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={Cyc_Core_Failure,{
_tmp176,_tmp176,_tmp176 + 24}};struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp177=yy1;struct Cyc_List_List*
_tmp178;_LLF8: if(*((void**)_tmp177)!= Cyc_SwitchClauseList_tok)goto _LLFA;_tmp178=((
struct Cyc_SwitchClauseList_tok_struct*)_tmp177)->f1;_LLF9: yyzzz=_tmp178;goto
_LLF7;_LLFA:;_LLFB:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);_LLF7:;}
return yyzzz;}static char _tmp17A[25]="list_t<switchC_clause_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchCClauseList_tok={Cyc_Core_Failure,{_tmp17A,_tmp17A,_tmp17A + 25}};
struct Cyc_List_List*Cyc_yyget_SwitchCClauseList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp17B=yy1;struct Cyc_List_List*_tmp17C;_LLFD: if(*((void**)_tmp17B)
!= Cyc_SwitchCClauseList_tok)goto _LLFF;_tmp17C=((struct Cyc_SwitchCClauseList_tok_struct*)
_tmp17B)->f1;_LLFE: yyzzz=_tmp17C;goto _LLFC;_LLFF:;_LL100:(int)_throw((void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LLFC:;}return yyzzz;}static char _tmp17E[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp17E,_tmp17E,_tmp17E + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp17F=yy1;
struct Cyc_Absyn_Pat*_tmp180;_LL102: if(*((void**)_tmp17F)!= Cyc_Pattern_tok)goto
_LL104;_tmp180=((struct Cyc_Pattern_tok_struct*)_tmp17F)->f1;_LL103: yyzzz=_tmp180;
goto _LL101;_LL104:;_LL105:(int)_throw((void*)& Cyc_yyfail_Pattern_tok);_LL101:;}
return yyzzz;}static char _tmp182[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={Cyc_Core_Failure,{_tmp182,_tmp182,_tmp182 + 14}};
struct Cyc_List_List*Cyc_yyget_PatternList_tok(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp183=yy1;struct Cyc_List_List*_tmp184;_LL107: if(*((void**)_tmp183)
!= Cyc_PatternList_tok)goto _LL109;_tmp184=((struct Cyc_PatternList_tok_struct*)
_tmp183)->f1;_LL108: yyzzz=_tmp184;goto _LL106;_LL109:;_LL10A:(int)_throw((void*)&
Cyc_yyfail_PatternList_tok);_LL106:;}return yyzzz;}static char _tmp186[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{
_tmp186,_tmp186,_tmp186 + 31}};struct _tuple11*Cyc_yyget_FieldPattern_tok(void*yy1){
struct _tuple11*yyzzz;{void*_tmp187=yy1;struct _tuple11*_tmp188;_LL10C: if(*((void**)
_tmp187)!= Cyc_FieldPattern_tok)goto _LL10E;_tmp188=((struct Cyc_FieldPattern_tok_struct*)
_tmp187)->f1;_LL10D: yyzzz=_tmp188;goto _LL10B;_LL10E:;_LL10F:(int)_throw((void*)&
Cyc_yyfail_FieldPattern_tok);_LL10B:;}return yyzzz;}static char _tmp18A[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp18A,_tmp18A,_tmp18A + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp18B=yy1;struct Cyc_List_List*
_tmp18C;_LL111: if(*((void**)_tmp18B)!= Cyc_FieldPatternList_tok)goto _LL113;
_tmp18C=((struct Cyc_FieldPatternList_tok_struct*)_tmp18B)->f1;_LL112: yyzzz=
_tmp18C;goto _LL110;_LL113:;_LL114:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL110:;}return yyzzz;}static char _tmp18E[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={Cyc_Core_Failure,{_tmp18E,_tmp18E,_tmp18E + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok(void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp18F=yy1;
struct Cyc_Absyn_Fndecl*_tmp190;_LL116: if(*((void**)_tmp18F)!= Cyc_FnDecl_tok)
goto _LL118;_tmp190=((struct Cyc_FnDecl_tok_struct*)_tmp18F)->f1;_LL117: yyzzz=
_tmp190;goto _LL115;_LL118:;_LL119:(int)_throw((void*)& Cyc_yyfail_FnDecl_tok);
_LL115:;}return yyzzz;}static char _tmp192[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={Cyc_Core_Failure,{_tmp192,_tmp192,_tmp192 + 15}};struct
Cyc_List_List*Cyc_yyget_DeclList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp193=yy1;struct Cyc_List_List*_tmp194;_LL11B: if(*((void**)_tmp193)!= Cyc_DeclList_tok)
goto _LL11D;_tmp194=((struct Cyc_DeclList_tok_struct*)_tmp193)->f1;_LL11C: yyzzz=
_tmp194;goto _LL11A;_LL11D:;_LL11E:(int)_throw((void*)& Cyc_yyfail_DeclList_tok);
_LL11A:;}return yyzzz;}static char _tmp196[12]="decl_spec_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={Cyc_Core_Failure,{_tmp196,_tmp196,_tmp196 + 12}};struct
Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(void*yy1){struct Cyc_Parse_Declaration_spec*
yyzzz;{void*_tmp197=yy1;struct Cyc_Parse_Declaration_spec*_tmp198;_LL120: if(*((
void**)_tmp197)!= Cyc_DeclSpec_tok)goto _LL122;_tmp198=((struct Cyc_DeclSpec_tok_struct*)
_tmp197)->f1;_LL121: yyzzz=_tmp198;goto _LL11F;_LL122:;_LL123:(int)_throw((void*)&
Cyc_yyfail_DeclSpec_tok);_LL11F:;}return yyzzz;}static char _tmp19A[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{
_tmp19A,_tmp19A,_tmp19A + 27}};struct _tuple12*Cyc_yyget_InitDecl_tok(void*yy1){
struct _tuple12*yyzzz;{void*_tmp19B=yy1;struct _tuple12*_tmp19C;_LL125: if(*((void**)
_tmp19B)!= Cyc_InitDecl_tok)goto _LL127;_tmp19C=((struct Cyc_InitDecl_tok_struct*)
_tmp19B)->f1;_LL126: yyzzz=_tmp19C;goto _LL124;_LL127:;_LL128:(int)_throw((void*)&
Cyc_yyfail_InitDecl_tok);_LL124:;}return yyzzz;}static char _tmp19E[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp19E,_tmp19E,_tmp19E + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp19F=yy1;struct Cyc_List_List*
_tmp1A0;_LL12A: if(*((void**)_tmp19F)!= Cyc_InitDeclList_tok)goto _LL12C;_tmp1A0=((
struct Cyc_InitDeclList_tok_struct*)_tmp19F)->f1;_LL12B: yyzzz=_tmp1A0;goto _LL129;
_LL12C:;_LL12D:(int)_throw((void*)& Cyc_yyfail_InitDeclList_tok);_LL129:;}return
yyzzz;}static char _tmp1A2[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{_tmp1A2,_tmp1A2,_tmp1A2 + 16}};
void*Cyc_yyget_StorageClass_tok(void*yy1){void*yyzzz;{void*_tmp1A3=yy1;void*
_tmp1A4;_LL12F: if(*((void**)_tmp1A3)!= Cyc_StorageClass_tok)goto _LL131;_tmp1A4=(
void*)((struct Cyc_StorageClass_tok_struct*)_tmp1A3)->f1;_LL130: yyzzz=_tmp1A4;
goto _LL12E;_LL131:;_LL132:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);
_LL12E:;}return yyzzz;}static char _tmp1A6[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{_tmp1A6,_tmp1A6,_tmp1A6 + 17}};
void*Cyc_yyget_TypeSpecifier_tok(void*yy1){void*yyzzz;{void*_tmp1A7=yy1;void*
_tmp1A8;_LL134: if(*((void**)_tmp1A7)!= Cyc_TypeSpecifier_tok)goto _LL136;_tmp1A8=(
void*)((struct Cyc_TypeSpecifier_tok_struct*)_tmp1A7)->f1;_LL135: yyzzz=_tmp1A8;
goto _LL133;_LL136:;_LL137:(int)_throw((void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL133:;}return yyzzz;}static char _tmp1AA[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={Cyc_Core_Failure,{_tmp1AA,_tmp1AA,_tmp1AA + 12}};
void*Cyc_yyget_StructOrUnion_tok(void*yy1){void*yyzzz;{void*_tmp1AB=yy1;void*
_tmp1AC;_LL139: if(*((void**)_tmp1AB)!= Cyc_StructOrUnion_tok)goto _LL13B;_tmp1AC=(
void*)((struct Cyc_StructOrUnion_tok_struct*)_tmp1AB)->f1;_LL13A: yyzzz=_tmp1AC;
goto _LL138;_LL13B:;_LL13C:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);
_LL138:;}return yyzzz;}static char _tmp1AE[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp1AE,_tmp1AE,_tmp1AE + 8}};struct
Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*
_tmp1AF=yy1;struct Cyc_Absyn_Tqual _tmp1B0;_LL13E: if(*((void**)_tmp1AF)!= Cyc_TypeQual_tok)
goto _LL140;_tmp1B0=((struct Cyc_TypeQual_tok_struct*)_tmp1AF)->f1;_LL13F: yyzzz=
_tmp1B0;goto _LL13D;_LL140:;_LL141:(int)_throw((void*)& Cyc_yyfail_TypeQual_tok);
_LL13D:;}return yyzzz;}static char _tmp1B2[20]="list_t<aggrfield_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclList_tok={Cyc_Core_Failure,{
_tmp1B2,_tmp1B2,_tmp1B2 + 20}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B3=yy1;struct Cyc_List_List*
_tmp1B4;_LL143: if(*((void**)_tmp1B3)!= Cyc_AggrFieldDeclList_tok)goto _LL145;
_tmp1B4=((struct Cyc_AggrFieldDeclList_tok_struct*)_tmp1B3)->f1;_LL144: yyzzz=
_tmp1B4;goto _LL142;_LL145:;_LL146:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclList_tok);
_LL142:;}return yyzzz;}static char _tmp1B6[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclListList_tok={Cyc_Core_Failure,{
_tmp1B6,_tmp1B6,_tmp1B6 + 28}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclListList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B7=yy1;struct Cyc_List_List*
_tmp1B8;_LL148: if(*((void**)_tmp1B7)!= Cyc_AggrFieldDeclListList_tok)goto _LL14A;
_tmp1B8=((struct Cyc_AggrFieldDeclListList_tok_struct*)_tmp1B7)->f1;_LL149: yyzzz=
_tmp1B8;goto _LL147;_LL14A:;_LL14B:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclListList_tok);
_LL147:;}return yyzzz;}static char _tmp1BA[24]="list_t<type_modifier_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{
_tmp1BA,_tmp1BA,_tmp1BA + 24}};struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1BB=yy1;struct Cyc_List_List*
_tmp1BC;_LL14D: if(*((void**)_tmp1BB)!= Cyc_TypeModifierList_tok)goto _LL14F;
_tmp1BC=((struct Cyc_TypeModifierList_tok_struct*)_tmp1BB)->f1;_LL14E: yyzzz=
_tmp1BC;goto _LL14C;_LL14F:;_LL150:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);
_LL14C:;}return yyzzz;}static char _tmp1BE[13]="declarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp1BE,_tmp1BE,_tmp1BE + 13}};
struct Cyc_Parse_Declarator*Cyc_yyget_Declarator_tok(void*yy1){struct Cyc_Parse_Declarator*
yyzzz;{void*_tmp1BF=yy1;struct Cyc_Parse_Declarator*_tmp1C0;_LL152: if(*((void**)
_tmp1BF)!= Cyc_Declarator_tok)goto _LL154;_tmp1C0=((struct Cyc_Declarator_tok_struct*)
_tmp1BF)->f1;_LL153: yyzzz=_tmp1C0;goto _LL151;_LL154:;_LL155:(int)_throw((void*)&
Cyc_yyfail_Declarator_tok);_LL151:;}return yyzzz;}static char _tmp1C2[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{
_tmp1C2,_tmp1C2,_tmp1C2 + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(
void*yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp1C3=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp1C4;_LL157: if(*((void**)_tmp1C3)!= Cyc_AbstractDeclarator_tok)goto _LL159;
_tmp1C4=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp1C3)->f1;_LL158: yyzzz=
_tmp1C4;goto _LL156;_LL159:;_LL15A:(int)_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL156:;}return yyzzz;}static char _tmp1C6[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={Cyc_Core_Failure,{_tmp1C6,_tmp1C6,_tmp1C6 + 5}};int Cyc_yyget_Bool_tok(
void*yy1){int yyzzz;{void*_tmp1C7=yy1;int _tmp1C8;_LL15C: if(*((void**)_tmp1C7)!= 
Cyc_Bool_tok)goto _LL15E;_tmp1C8=((struct Cyc_Bool_tok_struct*)_tmp1C7)->f1;_LL15D:
yyzzz=_tmp1C8;goto _LL15B;_LL15E:;_LL15F:(int)_throw((void*)& Cyc_yyfail_Bool_tok);
_LL15B:;}return yyzzz;}static char _tmp1CA[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={Cyc_Core_Failure,{_tmp1CA,_tmp1CA,_tmp1CA + 8}};void*Cyc_yyget_Scope_tok(
void*yy1){void*yyzzz;{void*_tmp1CB=yy1;void*_tmp1CC;_LL161: if(*((void**)_tmp1CB)
!= Cyc_Scope_tok)goto _LL163;_tmp1CC=(void*)((struct Cyc_Scope_tok_struct*)_tmp1CB)->f1;
_LL162: yyzzz=_tmp1CC;goto _LL160;_LL163:;_LL164:(int)_throw((void*)& Cyc_yyfail_Scope_tok);
_LL160:;}return yyzzz;}static char _tmp1CE[14]="tunionfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={Cyc_Core_Failure,{_tmp1CE,_tmp1CE,_tmp1CE + 14}};
struct Cyc_Absyn_Tunionfield*Cyc_yyget_TunionField_tok(void*yy1){struct Cyc_Absyn_Tunionfield*
yyzzz;{void*_tmp1CF=yy1;struct Cyc_Absyn_Tunionfield*_tmp1D0;_LL166: if(*((void**)
_tmp1CF)!= Cyc_TunionField_tok)goto _LL168;_tmp1D0=((struct Cyc_TunionField_tok_struct*)
_tmp1CF)->f1;_LL167: yyzzz=_tmp1D0;goto _LL165;_LL168:;_LL169:(int)_throw((void*)&
Cyc_yyfail_TunionField_tok);_LL165:;}return yyzzz;}static char _tmp1D2[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{
_tmp1D2,_tmp1D2,_tmp1D2 + 22}};struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1D3=yy1;struct Cyc_List_List*
_tmp1D4;_LL16B: if(*((void**)_tmp1D3)!= Cyc_TunionFieldList_tok)goto _LL16D;
_tmp1D4=((struct Cyc_TunionFieldList_tok_struct*)_tmp1D3)->f1;_LL16C: yyzzz=
_tmp1D4;goto _LL16A;_LL16D:;_LL16E:(int)_throw((void*)& Cyc_yyfail_TunionFieldList_tok);
_LL16A:;}return yyzzz;}static char _tmp1D6[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{
_tmp1D6,_tmp1D6,_tmp1D6 + 50}};struct _tuple15*Cyc_yyget_QualSpecList_tok(void*yy1){
struct _tuple15*yyzzz;{void*_tmp1D7=yy1;struct _tuple15*_tmp1D8;_LL170: if(*((void**)
_tmp1D7)!= Cyc_QualSpecList_tok)goto _LL172;_tmp1D8=((struct Cyc_QualSpecList_tok_struct*)
_tmp1D7)->f1;_LL171: yyzzz=_tmp1D8;goto _LL16F;_LL172:;_LL173:(int)_throw((void*)&
Cyc_yyfail_QualSpecList_tok);_LL16F:;}return yyzzz;}static char _tmp1DA[14]="list_t<var_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={Cyc_Core_Failure,{
_tmp1DA,_tmp1DA,_tmp1DA + 14}};struct Cyc_List_List*Cyc_yyget_IdList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp1DB=yy1;struct Cyc_List_List*_tmp1DC;_LL175:
if(*((void**)_tmp1DB)!= Cyc_IdList_tok)goto _LL177;_tmp1DC=((struct Cyc_IdList_tok_struct*)
_tmp1DB)->f1;_LL176: yyzzz=_tmp1DC;goto _LL174;_LL177:;_LL178:(int)_throw((void*)&
Cyc_yyfail_IdList_tok);_LL174:;}return yyzzz;}static char _tmp1DE[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={Cyc_Core_Failure,{
_tmp1DE,_tmp1DE,_tmp1DE + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(void*yy1){
struct _tuple2*yyzzz;{void*_tmp1DF=yy1;struct _tuple2*_tmp1E0;_LL17A: if(*((void**)
_tmp1DF)!= Cyc_ParamDecl_tok)goto _LL17C;_tmp1E0=((struct Cyc_ParamDecl_tok_struct*)
_tmp1DF)->f1;_LL17B: yyzzz=_tmp1E0;goto _LL179;_LL17C:;_LL17D:(int)_throw((void*)&
Cyc_yyfail_ParamDecl_tok);_LL179:;}return yyzzz;}static char _tmp1E2[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp1E2,_tmp1E2,_tmp1E2 + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1E3=yy1;struct Cyc_List_List*
_tmp1E4;_LL17F: if(*((void**)_tmp1E3)!= Cyc_ParamDeclList_tok)goto _LL181;_tmp1E4=((
struct Cyc_ParamDeclList_tok_struct*)_tmp1E3)->f1;_LL180: yyzzz=_tmp1E4;goto _LL17E;
_LL181:;_LL182:(int)_throw((void*)& Cyc_yyfail_ParamDeclList_tok);_LL17E:;}return
yyzzz;}static char _tmp1E6[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1E6,
_tmp1E6,_tmp1E6 + 107}};struct _tuple14*Cyc_yyget_YY1(void*yy1){struct _tuple14*
yyzzz;{void*_tmp1E7=yy1;struct _tuple14*_tmp1E8;_LL184: if(*((void**)_tmp1E7)!= 
Cyc_YY1)goto _LL186;_tmp1E8=((struct Cyc_YY1_struct*)_tmp1E7)->f1;_LL185: yyzzz=
_tmp1E8;goto _LL183;_LL186:;_LL187:(int)_throw((void*)& Cyc_yyfail_YY1);_LL183:;}
return yyzzz;}static char _tmp1EA[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={Cyc_Core_Failure,{_tmp1EA,_tmp1EA,_tmp1EA + 15}};struct
Cyc_List_List*Cyc_yyget_TypeList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*
_tmp1EB=yy1;struct Cyc_List_List*_tmp1EC;_LL189: if(*((void**)_tmp1EB)!= Cyc_TypeList_tok)
goto _LL18B;_tmp1EC=((struct Cyc_TypeList_tok_struct*)_tmp1EB)->f1;_LL18A: yyzzz=
_tmp1EC;goto _LL188;_LL18B:;_LL18C:(int)_throw((void*)& Cyc_yyfail_TypeList_tok);
_LL188:;}return yyzzz;}static char _tmp1EE[21]="list_t<designator_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{_tmp1EE,
_tmp1EE,_tmp1EE + 21}};struct Cyc_List_List*Cyc_yyget_DesignatorList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp1EF=yy1;struct Cyc_List_List*_tmp1F0;_LL18E:
if(*((void**)_tmp1EF)!= Cyc_DesignatorList_tok)goto _LL190;_tmp1F0=((struct Cyc_DesignatorList_tok_struct*)
_tmp1EF)->f1;_LL18F: yyzzz=_tmp1F0;goto _LL18D;_LL190:;_LL191:(int)_throw((void*)&
Cyc_yyfail_DesignatorList_tok);_LL18D:;}return yyzzz;}static char _tmp1F2[13]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={Cyc_Core_Failure,{
_tmp1F2,_tmp1F2,_tmp1F2 + 13}};void*Cyc_yyget_Designator_tok(void*yy1){void*yyzzz;{
void*_tmp1F3=yy1;void*_tmp1F4;_LL193: if(*((void**)_tmp1F3)!= Cyc_Designator_tok)
goto _LL195;_tmp1F4=(void*)((struct Cyc_Designator_tok_struct*)_tmp1F3)->f1;_LL194:
yyzzz=_tmp1F4;goto _LL192;_LL195:;_LL196:(int)_throw((void*)& Cyc_yyfail_Designator_tok);
_LL192:;}return yyzzz;}static char _tmp1F6[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{_tmp1F6,_tmp1F6,_tmp1F6 + 7}};void*Cyc_yyget_Kind_tok(
void*yy1){void*yyzzz;{void*_tmp1F7=yy1;void*_tmp1F8;_LL198: if(*((void**)_tmp1F7)
!= Cyc_Kind_tok)goto _LL19A;_tmp1F8=(void*)((struct Cyc_Kind_tok_struct*)_tmp1F7)->f1;
_LL199: yyzzz=_tmp1F8;goto _LL197;_LL19A:;_LL19B:(int)_throw((void*)& Cyc_yyfail_Kind_tok);
_LL197:;}return yyzzz;}static char _tmp1FA[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Type_tok={Cyc_Core_Failure,{_tmp1FA,_tmp1FA,_tmp1FA + 7}};void*Cyc_yyget_Type_tok(
void*yy1){void*yyzzz;{void*_tmp1FB=yy1;void*_tmp1FC;_LL19D: if(*((void**)_tmp1FB)
!= Cyc_Type_tok)goto _LL19F;_tmp1FC=(void*)((struct Cyc_Type_tok_struct*)_tmp1FB)->f1;
_LL19E: yyzzz=_tmp1FC;goto _LL19C;_LL19F:;_LL1A0:(int)_throw((void*)& Cyc_yyfail_Type_tok);
_LL19C:;}return yyzzz;}static char _tmp1FE[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={Cyc_Core_Failure,{_tmp1FE,
_tmp1FE,_tmp1FE + 20}};struct Cyc_List_List*Cyc_yyget_AttributeList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp1FF=yy1;struct Cyc_List_List*_tmp200;_LL1A2:
if(*((void**)_tmp1FF)!= Cyc_AttributeList_tok)goto _LL1A4;_tmp200=((struct Cyc_AttributeList_tok_struct*)
_tmp1FF)->f1;_LL1A3: yyzzz=_tmp200;goto _LL1A1;_LL1A4:;_LL1A5:(int)_throw((void*)&
Cyc_yyfail_AttributeList_tok);_LL1A1:;}return yyzzz;}static char _tmp202[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{
_tmp202,_tmp202,_tmp202 + 12}};void*Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{
void*_tmp203=yy1;void*_tmp204;_LL1A7: if(*((void**)_tmp203)!= Cyc_Attribute_tok)
goto _LL1A9;_tmp204=(void*)((struct Cyc_Attribute_tok_struct*)_tmp203)->f1;_LL1A8:
yyzzz=_tmp204;goto _LL1A6;_LL1A9:;_LL1AA:(int)_throw((void*)& Cyc_yyfail_Attribute_tok);
_LL1A6:;}return yyzzz;}static char _tmp206[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={Cyc_Core_Failure,{_tmp206,_tmp206,_tmp206 + 12}};struct
Cyc_Absyn_Enumfield*Cyc_yyget_Enumfield_tok(void*yy1){struct Cyc_Absyn_Enumfield*
yyzzz;{void*_tmp207=yy1;struct Cyc_Absyn_Enumfield*_tmp208;_LL1AC: if(*((void**)
_tmp207)!= Cyc_Enumfield_tok)goto _LL1AE;_tmp208=((struct Cyc_Enumfield_tok_struct*)
_tmp207)->f1;_LL1AD: yyzzz=_tmp208;goto _LL1AB;_LL1AE:;_LL1AF:(int)_throw((void*)&
Cyc_yyfail_Enumfield_tok);_LL1AB:;}return yyzzz;}static char _tmp20A[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{
_tmp20A,_tmp20A,_tmp20A + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp20B=yy1;struct Cyc_List_List*
_tmp20C;_LL1B1: if(*((void**)_tmp20B)!= Cyc_EnumfieldList_tok)goto _LL1B3;_tmp20C=((
struct Cyc_EnumfieldList_tok_struct*)_tmp20B)->f1;_LL1B2: yyzzz=_tmp20C;goto _LL1B0;
_LL1B3:;_LL1B4:(int)_throw((void*)& Cyc_yyfail_EnumfieldList_tok);_LL1B0:;}return
yyzzz;}static char _tmp20E[14]="opt_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={Cyc_Core_Failure,{_tmp20E,_tmp20E,_tmp20E + 14}};struct
Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*
_tmp20F=yy1;struct Cyc_Core_Opt*_tmp210;_LL1B6: if(*((void**)_tmp20F)!= Cyc_TypeOpt_tok)
goto _LL1B8;_tmp210=((struct Cyc_TypeOpt_tok_struct*)_tmp20F)->f1;_LL1B7: yyzzz=
_tmp210;goto _LL1B5;_LL1B8:;_LL1B9:(int)_throw((void*)& Cyc_yyfail_TypeOpt_tok);
_LL1B5:;}return yyzzz;}static char _tmp212[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{_tmp212,
_tmp212,_tmp212 + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp213=yy1;struct Cyc_List_List*_tmp214;_LL1BB:
if(*((void**)_tmp213)!= Cyc_Rgnorder_tok)goto _LL1BD;_tmp214=((struct Cyc_Rgnorder_tok_struct*)
_tmp213)->f1;_LL1BC: yyzzz=_tmp214;goto _LL1BA;_LL1BD:;_LL1BE:(int)_throw((void*)&
Cyc_yyfail_Rgnorder_tok);_LL1BA:;}return yyzzz;}struct Cyc_Yyltype{int timestamp;
int first_line;int first_column;int last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp215;_tmp215.timestamp=0;_tmp215.first_line=0;
_tmp215.first_column=0;_tmp215.last_line=0;_tmp215.last_column=0;_tmp215;});}
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[357]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,128,2,2,110,126,122,2,
106,107,116,119,108,121,113,125,2,2,2,2,2,2,2,2,2,2,112,102,111,105,120,118,117,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,114,2,115,124,109,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,103,123,104,127,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101};static char _tmp216[2]="$";
static char _tmp217[6]="error";static char _tmp218[12]="$undefined.";static char
_tmp219[5]="AUTO";static char _tmp21A[9]="REGISTER";static char _tmp21B[7]="STATIC";
static char _tmp21C[7]="EXTERN";static char _tmp21D[8]="TYPEDEF";static char _tmp21E[5]="VOID";
static char _tmp21F[5]="CHAR";static char _tmp220[6]="SHORT";static char _tmp221[4]="INT";
static char _tmp222[5]="LONG";static char _tmp223[6]="FLOAT";static char _tmp224[7]="DOUBLE";
static char _tmp225[7]="SIGNED";static char _tmp226[9]="UNSIGNED";static char _tmp227[
6]="CONST";static char _tmp228[9]="VOLATILE";static char _tmp229[9]="RESTRICT";
static char _tmp22A[7]="STRUCT";static char _tmp22B[6]="UNION";static char _tmp22C[5]="CASE";
static char _tmp22D[8]="DEFAULT";static char _tmp22E[7]="INLINE";static char _tmp22F[7]="SIZEOF";
static char _tmp230[9]="OFFSETOF";static char _tmp231[3]="IF";static char _tmp232[5]="ELSE";
static char _tmp233[7]="SWITCH";static char _tmp234[6]="WHILE";static char _tmp235[3]="DO";
static char _tmp236[4]="FOR";static char _tmp237[5]="GOTO";static char _tmp238[9]="CONTINUE";
static char _tmp239[6]="BREAK";static char _tmp23A[7]="RETURN";static char _tmp23B[5]="ENUM";
static char _tmp23C[8]="NULL_kw";static char _tmp23D[4]="LET";static char _tmp23E[6]="THROW";
static char _tmp23F[4]="TRY";static char _tmp240[6]="CATCH";static char _tmp241[4]="NEW";
static char _tmp242[9]="ABSTRACT";static char _tmp243[9]="FALLTHRU";static char
_tmp244[6]="USING";static char _tmp245[10]="NAMESPACE";static char _tmp246[7]="TUNION";
static char _tmp247[8]="XTUNION";static char _tmp248[9]="FORARRAY";static char _tmp249[
5]="FILL";static char _tmp24A[8]="CODEGEN";static char _tmp24B[4]="CUT";static char
_tmp24C[7]="SPLICE";static char _tmp24D[7]="MALLOC";static char _tmp24E[8]="RMALLOC";
static char _tmp24F[7]="CALLOC";static char _tmp250[8]="RCALLOC";static char _tmp251[9]="REGION_T";
static char _tmp252[9]="SIZEOF_T";static char _tmp253[6]="TAG_T";static char _tmp254[7]="REGION";
static char _tmp255[5]="RNEW";static char _tmp256[8]="REGIONS";static char _tmp257[13]="RESET_REGION";
static char _tmp258[4]="GEN";static char _tmp259[7]="PTR_OP";static char _tmp25A[7]="INC_OP";
static char _tmp25B[7]="DEC_OP";static char _tmp25C[8]="LEFT_OP";static char _tmp25D[9]="RIGHT_OP";
static char _tmp25E[6]="LE_OP";static char _tmp25F[6]="GE_OP";static char _tmp260[6]="EQ_OP";
static char _tmp261[6]="NE_OP";static char _tmp262[7]="AND_OP";static char _tmp263[6]="OR_OP";
static char _tmp264[11]="MUL_ASSIGN";static char _tmp265[11]="DIV_ASSIGN";static char
_tmp266[11]="MOD_ASSIGN";static char _tmp267[11]="ADD_ASSIGN";static char _tmp268[11]="SUB_ASSIGN";
static char _tmp269[12]="LEFT_ASSIGN";static char _tmp26A[13]="RIGHT_ASSIGN";static
char _tmp26B[11]="AND_ASSIGN";static char _tmp26C[11]="XOR_ASSIGN";static char
_tmp26D[10]="OR_ASSIGN";static char _tmp26E[9]="ELLIPSIS";static char _tmp26F[11]="LEFT_RIGHT";
static char _tmp270[12]="COLON_COLON";static char _tmp271[11]="IDENTIFIER";static
char _tmp272[17]="INTEGER_CONSTANT";static char _tmp273[7]="STRING";static char
_tmp274[19]="CHARACTER_CONSTANT";static char _tmp275[18]="FLOATING_CONSTANT";
static char _tmp276[9]="TYPE_VAR";static char _tmp277[13]="TYPEDEF_NAME";static char
_tmp278[16]="QUAL_IDENTIFIER";static char _tmp279[18]="QUAL_TYPEDEF_NAME";static
char _tmp27A[13]="TYPE_INTEGER";static char _tmp27B[10]="ATTRIBUTE";static char
_tmp27C[4]="';'";static char _tmp27D[4]="'{'";static char _tmp27E[4]="'}'";static
char _tmp27F[4]="'='";static char _tmp280[4]="'('";static char _tmp281[4]="')'";
static char _tmp282[4]="','";static char _tmp283[4]="'_'";static char _tmp284[4]="'$'";
static char _tmp285[4]="'<'";static char _tmp286[4]="':'";static char _tmp287[4]="'.'";
static char _tmp288[4]="'['";static char _tmp289[4]="']'";static char _tmp28A[4]="'*'";
static char _tmp28B[4]="'@'";static char _tmp28C[4]="'?'";static char _tmp28D[4]="'+'";
static char _tmp28E[4]="'>'";static char _tmp28F[4]="'-'";static char _tmp290[4]="'&'";
static char _tmp291[4]="'|'";static char _tmp292[4]="'^'";static char _tmp293[4]="'/'";
static char _tmp294[4]="'%'";static char _tmp295[4]="'~'";static char _tmp296[4]="'!'";
static char _tmp297[5]="prog";static char _tmp298[17]="translation_unit";static char
_tmp299[21]="external_declaration";static char _tmp29A[20]="function_definition";
static char _tmp29B[21]="function_definition2";static char _tmp29C[13]="using_action";
static char _tmp29D[15]="unusing_action";static char _tmp29E[17]="namespace_action";
static char _tmp29F[19]="unnamespace_action";static char _tmp2A0[12]="declaration";
static char _tmp2A1[17]="declaration_list";static char _tmp2A2[23]="declaration_specifiers";
static char _tmp2A3[24]="storage_class_specifier";static char _tmp2A4[15]="attributes_opt";
static char _tmp2A5[11]="attributes";static char _tmp2A6[15]="attribute_list";static
char _tmp2A7[10]="attribute";static char _tmp2A8[15]="type_specifier";static char
_tmp2A9[5]="kind";static char _tmp2AA[15]="type_qualifier";static char _tmp2AB[15]="enum_specifier";
static char _tmp2AC[11]="enum_field";static char _tmp2AD[22]="enum_declaration_list";
static char _tmp2AE[26]="struct_or_union_specifier";static char _tmp2AF[16]="type_params_opt";
static char _tmp2B0[16]="struct_or_union";static char _tmp2B1[24]="struct_declaration_list";
static char _tmp2B2[25]="struct_declaration_list0";static char _tmp2B3[21]="init_declarator_list";
static char _tmp2B4[22]="init_declarator_list0";static char _tmp2B5[16]="init_declarator";
static char _tmp2B6[19]="struct_declaration";static char _tmp2B7[25]="specifier_qualifier_list";
static char _tmp2B8[23]="struct_declarator_list";static char _tmp2B9[24]="struct_declarator_list0";
static char _tmp2BA[18]="struct_declarator";static char _tmp2BB[17]="tunion_specifier";
static char _tmp2BC[18]="tunion_or_xtunion";static char _tmp2BD[17]="tunionfield_list";
static char _tmp2BE[18]="tunionfield_scope";static char _tmp2BF[12]="tunionfield";
static char _tmp2C0[11]="declarator";static char _tmp2C1[18]="direct_declarator";
static char _tmp2C2[8]="pointer";static char _tmp2C3[13]="pointer_char";static char
_tmp2C4[8]="rgn_opt";static char _tmp2C5[4]="rgn";static char _tmp2C6[20]="type_qualifier_list";
static char _tmp2C7[20]="parameter_type_list";static char _tmp2C8[9]="type_var";
static char _tmp2C9[16]="optional_effect";static char _tmp2CA[19]="optional_rgn_order";
static char _tmp2CB[10]="rgn_order";static char _tmp2CC[16]="optional_inject";static
char _tmp2CD[11]="effect_set";static char _tmp2CE[14]="atomic_effect";static char
_tmp2CF[11]="region_set";static char _tmp2D0[15]="parameter_list";static char
_tmp2D1[22]="parameter_declaration";static char _tmp2D2[16]="identifier_list";
static char _tmp2D3[17]="identifier_list0";static char _tmp2D4[12]="initializer";
static char _tmp2D5[18]="array_initializer";static char _tmp2D6[17]="initializer_list";
static char _tmp2D7[12]="designation";static char _tmp2D8[16]="designator_list";
static char _tmp2D9[11]="designator";static char _tmp2DA[10]="type_name";static char
_tmp2DB[14]="any_type_name";static char _tmp2DC[15]="type_name_list";static char
_tmp2DD[20]="abstract_declarator";static char _tmp2DE[27]="direct_abstract_declarator";
static char _tmp2DF[10]="statement";static char _tmp2E0[18]="labeled_statement";
static char _tmp2E1[21]="expression_statement";static char _tmp2E2[19]="compound_statement";
static char _tmp2E3[16]="block_item_list";static char _tmp2E4[20]="selection_statement";
static char _tmp2E5[15]="switch_clauses";static char _tmp2E6[16]="switchC_clauses";
static char _tmp2E7[20]="iteration_statement";static char _tmp2E8[15]="jump_statement";
static char _tmp2E9[8]="pattern";static char _tmp2EA[19]="tuple_pattern_list";static
char _tmp2EB[14]="field_pattern";static char _tmp2EC[19]="field_pattern_list";
static char _tmp2ED[11]="expression";static char _tmp2EE[22]="assignment_expression";
static char _tmp2EF[20]="assignment_operator";static char _tmp2F0[23]="conditional_expression";
static char _tmp2F1[20]="constant_expression";static char _tmp2F2[22]="logical_or_expression";
static char _tmp2F3[23]="logical_and_expression";static char _tmp2F4[24]="inclusive_or_expression";
static char _tmp2F5[24]="exclusive_or_expression";static char _tmp2F6[15]="and_expression";
static char _tmp2F7[20]="equality_expression";static char _tmp2F8[22]="relational_expression";
static char _tmp2F9[17]="shift_expression";static char _tmp2FA[20]="additive_expression";
static char _tmp2FB[26]="multiplicative_expression";static char _tmp2FC[16]="cast_expression";
static char _tmp2FD[17]="unary_expression";static char _tmp2FE[15]="unary_operator";
static char _tmp2FF[19]="postfix_expression";static char _tmp300[19]="primary_expression";
static char _tmp301[25]="argument_expression_list";static char _tmp302[26]="argument_expression_list0";
static char _tmp303[9]="constant";static char _tmp304[20]="qual_opt_identifier";
static char _tmp305[17]="qual_opt_typedef";static char _tmp306[18]="struct_union_name";
static char _tmp307[11]="field_name";static char _tmp308[12]="right_angle";static
struct _tagged_arr Cyc_yytname[243]={{_tmp216,_tmp216,_tmp216 + 2},{_tmp217,_tmp217,
_tmp217 + 6},{_tmp218,_tmp218,_tmp218 + 12},{_tmp219,_tmp219,_tmp219 + 5},{_tmp21A,
_tmp21A,_tmp21A + 9},{_tmp21B,_tmp21B,_tmp21B + 7},{_tmp21C,_tmp21C,_tmp21C + 7},{
_tmp21D,_tmp21D,_tmp21D + 8},{_tmp21E,_tmp21E,_tmp21E + 5},{_tmp21F,_tmp21F,
_tmp21F + 5},{_tmp220,_tmp220,_tmp220 + 6},{_tmp221,_tmp221,_tmp221 + 4},{_tmp222,
_tmp222,_tmp222 + 5},{_tmp223,_tmp223,_tmp223 + 6},{_tmp224,_tmp224,_tmp224 + 7},{
_tmp225,_tmp225,_tmp225 + 7},{_tmp226,_tmp226,_tmp226 + 9},{_tmp227,_tmp227,
_tmp227 + 6},{_tmp228,_tmp228,_tmp228 + 9},{_tmp229,_tmp229,_tmp229 + 9},{_tmp22A,
_tmp22A,_tmp22A + 7},{_tmp22B,_tmp22B,_tmp22B + 6},{_tmp22C,_tmp22C,_tmp22C + 5},{
_tmp22D,_tmp22D,_tmp22D + 8},{_tmp22E,_tmp22E,_tmp22E + 7},{_tmp22F,_tmp22F,
_tmp22F + 7},{_tmp230,_tmp230,_tmp230 + 9},{_tmp231,_tmp231,_tmp231 + 3},{_tmp232,
_tmp232,_tmp232 + 5},{_tmp233,_tmp233,_tmp233 + 7},{_tmp234,_tmp234,_tmp234 + 6},{
_tmp235,_tmp235,_tmp235 + 3},{_tmp236,_tmp236,_tmp236 + 4},{_tmp237,_tmp237,
_tmp237 + 5},{_tmp238,_tmp238,_tmp238 + 9},{_tmp239,_tmp239,_tmp239 + 6},{_tmp23A,
_tmp23A,_tmp23A + 7},{_tmp23B,_tmp23B,_tmp23B + 5},{_tmp23C,_tmp23C,_tmp23C + 8},{
_tmp23D,_tmp23D,_tmp23D + 4},{_tmp23E,_tmp23E,_tmp23E + 6},{_tmp23F,_tmp23F,
_tmp23F + 4},{_tmp240,_tmp240,_tmp240 + 6},{_tmp241,_tmp241,_tmp241 + 4},{_tmp242,
_tmp242,_tmp242 + 9},{_tmp243,_tmp243,_tmp243 + 9},{_tmp244,_tmp244,_tmp244 + 6},{
_tmp245,_tmp245,_tmp245 + 10},{_tmp246,_tmp246,_tmp246 + 7},{_tmp247,_tmp247,
_tmp247 + 8},{_tmp248,_tmp248,_tmp248 + 9},{_tmp249,_tmp249,_tmp249 + 5},{_tmp24A,
_tmp24A,_tmp24A + 8},{_tmp24B,_tmp24B,_tmp24B + 4},{_tmp24C,_tmp24C,_tmp24C + 7},{
_tmp24D,_tmp24D,_tmp24D + 7},{_tmp24E,_tmp24E,_tmp24E + 8},{_tmp24F,_tmp24F,
_tmp24F + 7},{_tmp250,_tmp250,_tmp250 + 8},{_tmp251,_tmp251,_tmp251 + 9},{_tmp252,
_tmp252,_tmp252 + 9},{_tmp253,_tmp253,_tmp253 + 6},{_tmp254,_tmp254,_tmp254 + 7},{
_tmp255,_tmp255,_tmp255 + 5},{_tmp256,_tmp256,_tmp256 + 8},{_tmp257,_tmp257,
_tmp257 + 13},{_tmp258,_tmp258,_tmp258 + 4},{_tmp259,_tmp259,_tmp259 + 7},{_tmp25A,
_tmp25A,_tmp25A + 7},{_tmp25B,_tmp25B,_tmp25B + 7},{_tmp25C,_tmp25C,_tmp25C + 8},{
_tmp25D,_tmp25D,_tmp25D + 9},{_tmp25E,_tmp25E,_tmp25E + 6},{_tmp25F,_tmp25F,
_tmp25F + 6},{_tmp260,_tmp260,_tmp260 + 6},{_tmp261,_tmp261,_tmp261 + 6},{_tmp262,
_tmp262,_tmp262 + 7},{_tmp263,_tmp263,_tmp263 + 6},{_tmp264,_tmp264,_tmp264 + 11},{
_tmp265,_tmp265,_tmp265 + 11},{_tmp266,_tmp266,_tmp266 + 11},{_tmp267,_tmp267,
_tmp267 + 11},{_tmp268,_tmp268,_tmp268 + 11},{_tmp269,_tmp269,_tmp269 + 12},{
_tmp26A,_tmp26A,_tmp26A + 13},{_tmp26B,_tmp26B,_tmp26B + 11},{_tmp26C,_tmp26C,
_tmp26C + 11},{_tmp26D,_tmp26D,_tmp26D + 10},{_tmp26E,_tmp26E,_tmp26E + 9},{_tmp26F,
_tmp26F,_tmp26F + 11},{_tmp270,_tmp270,_tmp270 + 12},{_tmp271,_tmp271,_tmp271 + 11},{
_tmp272,_tmp272,_tmp272 + 17},{_tmp273,_tmp273,_tmp273 + 7},{_tmp274,_tmp274,
_tmp274 + 19},{_tmp275,_tmp275,_tmp275 + 18},{_tmp276,_tmp276,_tmp276 + 9},{_tmp277,
_tmp277,_tmp277 + 13},{_tmp278,_tmp278,_tmp278 + 16},{_tmp279,_tmp279,_tmp279 + 18},{
_tmp27A,_tmp27A,_tmp27A + 13},{_tmp27B,_tmp27B,_tmp27B + 10},{_tmp27C,_tmp27C,
_tmp27C + 4},{_tmp27D,_tmp27D,_tmp27D + 4},{_tmp27E,_tmp27E,_tmp27E + 4},{_tmp27F,
_tmp27F,_tmp27F + 4},{_tmp280,_tmp280,_tmp280 + 4},{_tmp281,_tmp281,_tmp281 + 4},{
_tmp282,_tmp282,_tmp282 + 4},{_tmp283,_tmp283,_tmp283 + 4},{_tmp284,_tmp284,
_tmp284 + 4},{_tmp285,_tmp285,_tmp285 + 4},{_tmp286,_tmp286,_tmp286 + 4},{_tmp287,
_tmp287,_tmp287 + 4},{_tmp288,_tmp288,_tmp288 + 4},{_tmp289,_tmp289,_tmp289 + 4},{
_tmp28A,_tmp28A,_tmp28A + 4},{_tmp28B,_tmp28B,_tmp28B + 4},{_tmp28C,_tmp28C,
_tmp28C + 4},{_tmp28D,_tmp28D,_tmp28D + 4},{_tmp28E,_tmp28E,_tmp28E + 4},{_tmp28F,
_tmp28F,_tmp28F + 4},{_tmp290,_tmp290,_tmp290 + 4},{_tmp291,_tmp291,_tmp291 + 4},{
_tmp292,_tmp292,_tmp292 + 4},{_tmp293,_tmp293,_tmp293 + 4},{_tmp294,_tmp294,
_tmp294 + 4},{_tmp295,_tmp295,_tmp295 + 4},{_tmp296,_tmp296,_tmp296 + 4},{_tmp297,
_tmp297,_tmp297 + 5},{_tmp298,_tmp298,_tmp298 + 17},{_tmp299,_tmp299,_tmp299 + 21},{
_tmp29A,_tmp29A,_tmp29A + 20},{_tmp29B,_tmp29B,_tmp29B + 21},{_tmp29C,_tmp29C,
_tmp29C + 13},{_tmp29D,_tmp29D,_tmp29D + 15},{_tmp29E,_tmp29E,_tmp29E + 17},{
_tmp29F,_tmp29F,_tmp29F + 19},{_tmp2A0,_tmp2A0,_tmp2A0 + 12},{_tmp2A1,_tmp2A1,
_tmp2A1 + 17},{_tmp2A2,_tmp2A2,_tmp2A2 + 23},{_tmp2A3,_tmp2A3,_tmp2A3 + 24},{
_tmp2A4,_tmp2A4,_tmp2A4 + 15},{_tmp2A5,_tmp2A5,_tmp2A5 + 11},{_tmp2A6,_tmp2A6,
_tmp2A6 + 15},{_tmp2A7,_tmp2A7,_tmp2A7 + 10},{_tmp2A8,_tmp2A8,_tmp2A8 + 15},{
_tmp2A9,_tmp2A9,_tmp2A9 + 5},{_tmp2AA,_tmp2AA,_tmp2AA + 15},{_tmp2AB,_tmp2AB,
_tmp2AB + 15},{_tmp2AC,_tmp2AC,_tmp2AC + 11},{_tmp2AD,_tmp2AD,_tmp2AD + 22},{
_tmp2AE,_tmp2AE,_tmp2AE + 26},{_tmp2AF,_tmp2AF,_tmp2AF + 16},{_tmp2B0,_tmp2B0,
_tmp2B0 + 16},{_tmp2B1,_tmp2B1,_tmp2B1 + 24},{_tmp2B2,_tmp2B2,_tmp2B2 + 25},{
_tmp2B3,_tmp2B3,_tmp2B3 + 21},{_tmp2B4,_tmp2B4,_tmp2B4 + 22},{_tmp2B5,_tmp2B5,
_tmp2B5 + 16},{_tmp2B6,_tmp2B6,_tmp2B6 + 19},{_tmp2B7,_tmp2B7,_tmp2B7 + 25},{
_tmp2B8,_tmp2B8,_tmp2B8 + 23},{_tmp2B9,_tmp2B9,_tmp2B9 + 24},{_tmp2BA,_tmp2BA,
_tmp2BA + 18},{_tmp2BB,_tmp2BB,_tmp2BB + 17},{_tmp2BC,_tmp2BC,_tmp2BC + 18},{
_tmp2BD,_tmp2BD,_tmp2BD + 17},{_tmp2BE,_tmp2BE,_tmp2BE + 18},{_tmp2BF,_tmp2BF,
_tmp2BF + 12},{_tmp2C0,_tmp2C0,_tmp2C0 + 11},{_tmp2C1,_tmp2C1,_tmp2C1 + 18},{
_tmp2C2,_tmp2C2,_tmp2C2 + 8},{_tmp2C3,_tmp2C3,_tmp2C3 + 13},{_tmp2C4,_tmp2C4,
_tmp2C4 + 8},{_tmp2C5,_tmp2C5,_tmp2C5 + 4},{_tmp2C6,_tmp2C6,_tmp2C6 + 20},{_tmp2C7,
_tmp2C7,_tmp2C7 + 20},{_tmp2C8,_tmp2C8,_tmp2C8 + 9},{_tmp2C9,_tmp2C9,_tmp2C9 + 16},{
_tmp2CA,_tmp2CA,_tmp2CA + 19},{_tmp2CB,_tmp2CB,_tmp2CB + 10},{_tmp2CC,_tmp2CC,
_tmp2CC + 16},{_tmp2CD,_tmp2CD,_tmp2CD + 11},{_tmp2CE,_tmp2CE,_tmp2CE + 14},{
_tmp2CF,_tmp2CF,_tmp2CF + 11},{_tmp2D0,_tmp2D0,_tmp2D0 + 15},{_tmp2D1,_tmp2D1,
_tmp2D1 + 22},{_tmp2D2,_tmp2D2,_tmp2D2 + 16},{_tmp2D3,_tmp2D3,_tmp2D3 + 17},{
_tmp2D4,_tmp2D4,_tmp2D4 + 12},{_tmp2D5,_tmp2D5,_tmp2D5 + 18},{_tmp2D6,_tmp2D6,
_tmp2D6 + 17},{_tmp2D7,_tmp2D7,_tmp2D7 + 12},{_tmp2D8,_tmp2D8,_tmp2D8 + 16},{
_tmp2D9,_tmp2D9,_tmp2D9 + 11},{_tmp2DA,_tmp2DA,_tmp2DA + 10},{_tmp2DB,_tmp2DB,
_tmp2DB + 14},{_tmp2DC,_tmp2DC,_tmp2DC + 15},{_tmp2DD,_tmp2DD,_tmp2DD + 20},{
_tmp2DE,_tmp2DE,_tmp2DE + 27},{_tmp2DF,_tmp2DF,_tmp2DF + 10},{_tmp2E0,_tmp2E0,
_tmp2E0 + 18},{_tmp2E1,_tmp2E1,_tmp2E1 + 21},{_tmp2E2,_tmp2E2,_tmp2E2 + 19},{
_tmp2E3,_tmp2E3,_tmp2E3 + 16},{_tmp2E4,_tmp2E4,_tmp2E4 + 20},{_tmp2E5,_tmp2E5,
_tmp2E5 + 15},{_tmp2E6,_tmp2E6,_tmp2E6 + 16},{_tmp2E7,_tmp2E7,_tmp2E7 + 20},{
_tmp2E8,_tmp2E8,_tmp2E8 + 15},{_tmp2E9,_tmp2E9,_tmp2E9 + 8},{_tmp2EA,_tmp2EA,
_tmp2EA + 19},{_tmp2EB,_tmp2EB,_tmp2EB + 14},{_tmp2EC,_tmp2EC,_tmp2EC + 19},{
_tmp2ED,_tmp2ED,_tmp2ED + 11},{_tmp2EE,_tmp2EE,_tmp2EE + 22},{_tmp2EF,_tmp2EF,
_tmp2EF + 20},{_tmp2F0,_tmp2F0,_tmp2F0 + 23},{_tmp2F1,_tmp2F1,_tmp2F1 + 20},{
_tmp2F2,_tmp2F2,_tmp2F2 + 22},{_tmp2F3,_tmp2F3,_tmp2F3 + 23},{_tmp2F4,_tmp2F4,
_tmp2F4 + 24},{_tmp2F5,_tmp2F5,_tmp2F5 + 24},{_tmp2F6,_tmp2F6,_tmp2F6 + 15},{
_tmp2F7,_tmp2F7,_tmp2F7 + 20},{_tmp2F8,_tmp2F8,_tmp2F8 + 22},{_tmp2F9,_tmp2F9,
_tmp2F9 + 17},{_tmp2FA,_tmp2FA,_tmp2FA + 20},{_tmp2FB,_tmp2FB,_tmp2FB + 26},{
_tmp2FC,_tmp2FC,_tmp2FC + 16},{_tmp2FD,_tmp2FD,_tmp2FD + 17},{_tmp2FE,_tmp2FE,
_tmp2FE + 15},{_tmp2FF,_tmp2FF,_tmp2FF + 19},{_tmp300,_tmp300,_tmp300 + 19},{
_tmp301,_tmp301,_tmp301 + 25},{_tmp302,_tmp302,_tmp302 + 26},{_tmp303,_tmp303,
_tmp303 + 9},{_tmp304,_tmp304,_tmp304 + 20},{_tmp305,_tmp305,_tmp305 + 17},{_tmp306,
_tmp306,_tmp306 + 18},{_tmp307,_tmp307,_tmp307 + 11},{_tmp308,_tmp308,_tmp308 + 12}};
static short Cyc_yyr1[412]={0,129,130,130,130,130,130,130,130,131,131,132,132,132,
132,133,133,134,135,136,137,138,138,138,138,139,139,140,140,140,140,140,140,140,
140,141,141,141,141,141,141,141,142,142,143,144,144,145,145,145,145,145,146,146,
146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,147,
148,148,148,149,149,149,150,150,151,151,152,152,152,153,153,154,154,155,156,156,
157,158,158,159,159,160,161,161,161,161,162,163,163,164,164,164,165,165,165,165,
166,166,167,167,167,167,168,168,168,169,169,170,170,171,171,171,171,171,171,171,
171,171,171,171,172,172,172,172,173,173,173,173,173,174,174,175,175,176,176,177,
177,177,177,178,178,179,179,180,180,181,181,182,182,183,183,184,184,184,184,185,
185,186,186,187,187,187,188,189,189,190,190,191,191,191,191,192,192,192,192,193,
194,194,195,195,196,196,197,197,197,197,197,198,198,199,199,199,200,200,200,200,
200,200,200,200,200,200,200,201,201,201,201,201,201,201,201,201,201,201,201,201,
202,203,203,204,204,205,205,205,205,205,205,206,206,206,206,206,207,207,207,207,
207,207,208,208,208,208,209,209,209,209,209,209,209,209,209,209,209,209,209,209,
209,210,210,210,210,210,210,210,210,211,211,211,211,211,211,211,211,211,211,211,
211,211,212,212,213,213,214,214,215,215,216,216,217,217,217,217,217,217,217,217,
217,217,217,218,218,218,218,218,218,218,219,220,220,221,221,222,222,223,223,224,
224,225,225,225,226,226,226,226,226,227,227,227,228,228,228,229,229,229,229,230,
230,231,231,231,231,231,231,231,231,231,231,231,231,231,231,231,231,232,232,232,
233,233,233,233,233,233,233,233,233,233,233,233,234,234,234,234,234,234,234,234,
234,235,236,236,237,237,237,237,238,238,239,239,240,240,241,241,242,242};static
short Cyc_yyr2[412]={0,1,2,3,5,3,5,6,0,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,1,2,2,3,2,
3,2,3,2,3,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,8,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,2,4,4,
4,1,4,1,1,1,1,5,2,4,1,3,1,3,4,7,3,0,3,1,1,1,1,2,1,1,3,1,3,3,2,3,2,3,1,1,3,1,2,3,
6,4,3,5,1,1,1,2,3,3,0,1,1,2,5,1,2,1,3,3,4,4,5,4,4,2,2,1,3,4,4,5,1,1,4,4,1,0,1,1,
1,1,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,
2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,2,3,3,4,4,3,5,4,4,2,1,1,1,1,1,1,6,3,9,
6,5,2,2,3,1,2,2,3,1,2,1,2,1,2,5,7,7,8,6,0,3,4,5,6,7,0,3,4,5,5,7,6,7,7,8,7,8,8,9,
6,7,7,8,9,3,2,2,2,3,2,4,5,1,3,1,2,1,1,1,1,4,4,5,2,2,1,3,1,2,1,3,1,3,1,3,1,1,1,1,
1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,
3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,1,1,1,1,4,3,4,3,3,2,2,6,7,4,4,1,1,1,3,
2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[848]={0,137,
35,36,37,38,40,52,55,56,57,58,59,60,61,62,74,75,76,89,90,42,0,0,41,0,0,114,115,0,
0,0,402,157,404,403,405,71,0,53,0,142,143,146,1,0,9,0,0,10,0,42,42,42,63,64,0,65,
0,0,125,0,147,66,127,87,39,0,33,43,0,78,289,402,285,288,287,0,283,0,0,0,0,0,180,
0,290,17,19,0,0,0,0,0,0,0,0,0,2,0,0,0,0,21,0,94,95,97,27,29,31,0,406,407,87,150,
0,149,87,38,0,25,0,0,11,159,0,0,135,126,42,148,136,0,67,0,0,34,82,0,80,0,0,0,295,
286,294,24,0,0,87,0,0,0,42,42,198,200,0,0,0,408,409,158,73,128,54,0,0,175,0,0,
401,0,0,0,0,0,0,0,0,0,87,0,0,398,388,399,400,0,0,0,0,373,0,371,372,0,304,317,325,
327,329,331,333,335,338,343,346,349,353,0,355,374,387,386,0,3,0,5,0,22,0,0,0,12,
28,30,32,0,91,92,0,86,87,0,112,39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,402,235,237,0,
243,239,0,241,221,222,223,0,224,225,226,0,302,26,13,97,165,181,0,0,161,159,0,205,
0,129,0,138,0,0,0,0,79,0,0,284,296,0,182,0,0,0,0,201,173,0,100,102,159,0,207,199,
208,411,0,410,69,70,72,0,177,0,179,68,0,0,363,0,319,353,0,320,321,0,0,0,0,0,0,0,
0,0,356,357,0,0,0,0,359,360,358,144,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,307,
308,309,310,311,312,313,314,315,316,306,0,361,0,380,381,0,0,0,390,0,87,145,18,0,
20,0,96,98,184,183,14,84,93,0,0,104,105,107,87,111,87,120,0,0,0,0,0,0,0,276,277,
278,0,0,280,0,0,232,233,0,0,0,0,0,0,244,240,97,242,238,236,0,166,0,0,0,172,160,
167,131,0,0,0,161,133,0,134,130,151,140,139,88,0,48,47,0,45,83,324,81,77,291,0,
23,0,0,0,0,194,298,300,0,292,0,0,202,101,103,0,161,0,211,0,209,159,0,0,220,204,
176,0,0,0,185,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,389,396,0,395,326,0,328,330,332,
334,336,337,341,342,339,340,344,345,347,348,350,351,352,305,379,376,0,378,0,0,0,
4,6,108,99,0,0,0,113,122,121,0,0,116,0,0,0,0,0,0,0,0,275,279,0,0,0,0,228,0,0,0,
234,0,15,303,159,0,169,0,0,0,162,132,165,153,206,152,141,7,0,0,0,297,197,0,299,
193,195,293,0,203,174,216,0,210,213,0,161,0,212,0,362,0,0,186,0,190,384,385,0,
367,0,0,0,0,0,0,0,0,354,392,0,0,377,375,0,0,106,109,0,110,123,120,120,0,0,0,0,0,
0,0,0,0,0,250,281,0,0,0,0,0,16,161,0,170,168,0,161,0,0,0,0,44,46,196,301,215,218,
0,219,214,0,0,0,187,191,0,0,0,0,322,323,366,394,0,397,318,391,393,0,85,0,119,118,
245,0,250,260,0,0,0,0,0,0,0,0,0,0,0,0,282,0,0,0,0,231,155,171,163,154,159,0,49,
50,217,365,364,0,192,368,0,0,382,0,0,0,256,0,0,262,0,0,0,270,0,0,0,0,0,0,0,0,249,
0,227,230,0,0,161,0,0,0,0,383,124,246,0,0,0,247,261,263,264,0,272,271,0,266,0,0,
0,0,250,251,0,0,164,156,0,0,0,0,0,0,248,265,273,267,268,0,0,250,252,0,0,0,188,
369,0,256,257,269,250,253,274,229,51,0,256,258,250,254,370,259,255,0,0,0};static
short Cyc_yydefgoto[114]={845,44,45,46,258,47,393,48,395,49,224,50,51,68,69,465,
466,52,163,53,54,138,139,55,134,56,229,230,104,105,106,231,156,404,405,406,57,58,
560,561,562,59,60,61,62,130,116,460,488,63,489,451,591,443,447,448,303,279,169,
83,84,504,398,505,506,477,478,157,281,282,490,310,261,262,263,264,265,266,729,
788,267,268,294,295,480,481,269,270,380,198,469,199,200,201,202,203,204,205,206,
207,208,209,210,211,212,213,521,522,214,215,65,114,164,314};static short Cyc_yypact[
848]={2065,- -32768,- -32768,- -32768,- -32768,- 75,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- 7,90,588,- -32768,138,92,- -32768,- -32768,106,130,159,- -32768,194,- -32768,
- -32768,- -32768,- -32768,128,262,259,286,295,- -32768,- -32768,1946,- -32768,152,213,
- -32768,257,- 7,- 7,- 7,- -32768,- -32768,211,- -32768,181,2605,233,28,- 16,- -32768,138,
296,322,333,2736,- -32768,138,373,- -32768,172,- -32768,- -32768,- -32768,630,- -32768,
336,353,394,630,386,395,406,324,- -32768,- -32768,4695,4695,4695,263,421,263,4861,
4262,4262,- -32768,1946,2183,1946,2183,- -32768,423,427,- -32768,2538,2736,2736,
2736,4861,- -32768,- -32768,296,- -32768,138,- -32768,47,445,1323,- -32768,2605,257,-
-32768,4638,4695,3252,- -32768,233,- 7,- -32768,- -32768,4695,- -32768,2183,441,-
-32768,444,453,456,138,442,630,- -32768,- -32768,- -32768,- -32768,476,4262,296,630,
471,155,- 7,- 7,337,- -32768,2,2,2,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
398,273,- -32768,4455,501,- -32768,4262,4307,522,524,527,549,556,559,561,296,4500,
4500,- -32768,- -32768,- -32768,- -32768,1696,572,4574,4574,- -32768,4574,- -32768,-
-32768,510,- -32768,- 17,605,570,560,581,490,3,345,222,- 13,- -32768,709,4574,118,63,
- -32768,602,604,- -32768,607,- -32768,611,- -32768,128,3297,2605,- -32768,- -32768,-
-32768,- -32768,613,4861,- -32768,507,603,296,138,616,- -32768,617,61,620,2822,621,
629,627,628,3371,2822,68,625,2822,2822,34,626,622,- -32768,- -32768,632,1449,1449,
257,1449,- -32768,- -32768,- -32768,637,- -32768,- -32768,- -32768,268,- -32768,- -32768,
- -32768,639,642,- -32768,24,641,643,277,649,623,- 29,- -32768,634,204,- 29,655,39,
138,- -32768,4262,656,- -32768,- -32768,383,- -32768,316,550,465,4695,- -32768,653,
658,4861,4861,2422,3445,161,- -32768,250,- -32768,24,- -32768,- -32768,- -32768,-
-32768,2311,- -32768,367,- -32768,- -32768,4861,1696,- -32768,4861,- -32768,- -32768,
2908,- -32768,686,4262,2247,4262,4262,4262,4262,4262,660,1696,- -32768,- -32768,
1449,657,472,4262,- -32768,- -32768,- -32768,- -32768,4574,4262,4574,4574,4574,4574,
4574,4574,4574,4574,4574,4574,4574,4574,4574,4574,4574,4574,4574,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,4262,-
-32768,263,- -32768,- -32768,3490,263,4262,- -32768,661,296,- -32768,- -32768,1946,-
-32768,1946,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,4262,665,663,
- -32768,664,296,- -32768,296,578,4262,662,4262,4262,739,1575,673,- -32768,- -32768,
- -32768,326,736,- -32768,3564,2672,- -32768,- -32768,2822,683,689,4262,2822,1817,-
-32768,- -32768,2538,- -32768,- -32768,- -32768,4262,- -32768,4861,676,185,- -32768,-
-32768,678,- -32768,687,677,4807,643,- -32768,4695,- -32768,- -32768,- -32768,- -32768,
204,- -32768,1946,- -32768,693,694,692,- -32768,- -32768,- -32768,- -32768,- -32768,630,
- -32768,263,4262,630,186,- -32768,- -32768,- -32768,134,- -32768,- 37,706,- -32768,-
-32768,- -32768,696,643,697,- -32768,690,250,4752,4695,3609,- -32768,- -32768,-
-32768,699,701,716,- -32768,- -32768,149,3297,478,703,128,484,704,705,707,487,4861,
710,712,4381,- -32768,- -32768,713,711,605,193,570,560,581,490,3,3,345,345,345,345,
222,222,- 13,- 13,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,715,- -32768,188,
4695,3166,- -32768,- -32768,- -32768,- -32768,507,4262,4861,- -32768,- -32768,- -32768,
719,138,330,489,4262,493,502,720,3683,3757,358,- -32768,- -32768,708,723,721,727,-
-32768,714,717,504,- -32768,2605,- -32768,- -32768,728,4695,- -32768,729,24,724,-
-32768,- -32768,103,- -32768,623,- -32768,- -32768,- -32768,452,731,39,- -32768,-
-32768,726,- -32768,- -32768,- -32768,- -32768,550,- -32768,- -32768,- -32768,732,-
-32768,- -32768,735,643,- 29,- -32768,730,733,245,738,- -32768,2994,- -32768,- -32768,
- -32768,2605,- -32768,4262,806,4262,4307,740,733,743,3166,- -32768,- -32768,4262,
4262,- -32768,- -32768,- 29,215,- -32768,- -32768,742,- -32768,745,38,578,2822,508,749,
2822,4262,3831,370,3876,411,3950,614,- -32768,753,4262,765,4,755,- -32768,643,49,-
-32768,- -32768,762,643,4861,751,756,759,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,760,- -32768,- -32768,761,763,4262,- -32768,- -32768,3297,531,754,764,-
-32768,686,- -32768,- -32768,249,- -32768,- -32768,- -32768,- -32768,3166,- -32768,4861,
- -32768,- -32768,841,768,614,- -32768,539,2822,543,4024,2822,545,4069,4143,416,630,
767,769,- -32768,418,2822,2822,778,- -32768,- -32768,- -32768,774,- -32768,728,785,-
-32768,- -32768,- -32768,- -32768,- -32768,288,- -32768,- -32768,4861,858,- -32768,3080,
562,2822,635,781,784,- -32768,2822,2822,567,- -32768,2822,2822,569,2822,580,4188,
52,1449,- -32768,4262,- -32768,- -32768,770,687,643,779,4262,786,782,- -32768,-
-32768,- -32768,4262,783,787,- -32768,- -32768,- -32768,- -32768,2822,- -32768,- -32768,
2822,- -32768,2822,2822,582,4262,1071,- -32768,584,803,- -32768,- -32768,804,341,790,
4861,356,1449,- -32768,- -32768,- -32768,- -32768,- -32768,2822,375,614,- -32768,2822,
2822,792,- -32768,- -32768,793,1197,- -32768,- -32768,1071,- -32768,- -32768,- -32768,-
-32768,794,635,- -32768,614,- -32768,- -32768,- -32768,- -32768,904,905,- -32768};
static short Cyc_yypgoto[114]={- -32768,36,- -32768,574,- -32768,- -32768,- -32768,-
-32768,- -32768,- 55,- 47,- 59,- -32768,139,- 58,306,- -32768,87,814,7,- -32768,- -32768,
- 128,- -32768,168,- -32768,355,- -32768,- -32768,- -32768,691,679,- 57,- -32768,- -32768,
360,- -32768,- -32768,43,- -32768,- -32768,66,- 34,- 122,- -32768,- -32768,850,- -32768,-
110,- 48,- 100,- 406,140,323,329,608,- 410,- 89,- 290,796,- -32768,- 202,- 173,- 521,- 276,
- -32768,438,- 184,- 50,- 126,51,- 94,170,- -32768,- -32768,- 23,- 242,- -32768,- 605,- 672,
- -32768,- -32768,- 20,771,310,- -32768,217,112,- -32768,- 147,- 383,- 163,573,575,571,
579,577,343,173,339,349,- 105,670,- -32768,- -32768,- -32768,- 348,- -32768,- -32768,8,
870,- -32768,- 327,- 136};static short Cyc_yytable[4972]={123,329,128,85,121,343,168,
286,64,137,117,330,122,292,117,277,435,436,66,438,552,397,476,315,316,278,326,
129,646,1,71,86,499,87,308,588,124,545,167,158,159,160,311,558,559,319,64,594,
123,226,227,228,121,64,232,543,463,142,64,546,350,260,146,123,112,259,118,271,
167,64,610,128,132,311,611,358,359,575,140,455,33,98,312,613,225,86,346,347,444,
348,86,313,604,115,67,733,155,155,155,272,517,351,155,366,93,302,381,64,64,64,64,
757,367,368,360,734,107,704,155,32,33,312,313,361,234,429,35,445,801,1,464,64,
155,155,38,217,218,219,220,500,155,501,- 117,64,468,430,456,603,431,140,461,86,
388,585,413,516,737,839,133,86,235,467,499,459,802,123,843,414,312,271,424,287,
648,232,425,64,154,154,154,453,389,32,154,88,308,382,383,384,35,273,108,109,110,
70,442,319,155,822,154,260,260,400,260,259,259,276,259,309,197,216,- 159,688,154,
154,493,- 159,833,89,320,32,154,16,17,18,385,493,35,841,446,32,64,386,387,318,38,
844,35,155,608,284,64,90,609,410,41,42,43,486,487,167,483,33,497,624,99,100,468,
625,1,301,167,539,540,541,446,167,736,306,64,285,91,739,32,509,- 181,307,154,33,
479,35,- 181,33,233,260,92,129,236,259,273,587,115,606,458,304,305,692,441,140,
407,474,475,441,32,644,626,642,86,155,34,35,36,155,155,155,111,101,102,154,298,
708,41,42,43,709,155,64,437,64,468,155,155,635,155,476,67,399,161,691,597,125,64,
364,162,365,126,168,155,127,32,696,338,67,94,752,161,35,494,753,123,103,162,495,
569,38,496,95,297,123,1,618,440,576,807,41,42,43,441,167,123,276,321,322,121,93,
616,452,167,154,740,96,582,154,154,154,617,167,441,302,97,1,780,64,409,64,154,
595,344,133,468,154,154,416,154,639,583,362,363,423,473,492,427,428,645,695,441,
135,154,150,572,550,151,551,652,696,441,497,302,167,653,136,399,155,143,306,144,
826,511,512,513,441,155,307,602,41,42,43,605,520,32,155,663,700,155,422,441,35,
441,596,829,- 207,64,701,721,317,- 207,- 207,141,696,441,- 207,86,307,689,441,86,672,
145,832,147,32,471,472,542,677,748,706,35,520,598,232,- 178,155,155,148,317,- 178,
- 178,695,1,707,- 178,149,307,724,41,42,43,64,769,441,773,154,155,123,441,221,441,
271,165,803,154,531,532,533,534,222,673,520,237,154,344,446,154,679,680,681,520,
288,507,293,510,695,289,584,514,155,344,290,549,671,821,291,64,155,356,357,781,
296,524,651,123,830,482,472,121,629,556,300,557,519,441,154,154,558,559,627,441,
838,72,479,840,630,441,155,634,441,654,441,32,577,656,441,154,581,547,35,225,325,
620,657,441,670,441,38,349,715,441,86,399,403,407,167,754,41,42,43,72,828,331,
563,332,565,566,333,570,154,727,728,749,441,778,32,74,154,75,76,758,441,35,580,
760,441,764,441,167,334,77,786,787,78,79,399,335,474,475,336,80,337,72,784,322,
81,82,154,793,441,796,441,345,73,74,352,75,76,354,155,35,798,441,819,441,823,441,
353,77,712,713,78,79,529,530,535,536,355,80,390,408,770,391,81,82,392,260,537,
538,394,259,401,155,411,418,32,74,412,75,76,415,417,35,419,420,426,432,442,433,
86,77,399,434,78,79,439,312,260,223,699,80,259,449,457,399,81,82,705,260,450,454,
155,259,462,470,484,485,350,518,154,515,553,564,567,260,554,548,260,259,571,555,
259,573,578,579,655,586,590,592,660,662,369,370,371,372,373,374,375,376,377,378,
589,154,599,601,600,33,612,614,615,621,623,399,622,628,664,631,632,379,633,637,
636,155,641,640,399,643,650,714,665,658,717,666,667,276,698,669,674,668,676,638,
154,682,686,324,684,687,327,327,690,710,702,697,693,703,711,716,340,341,730,732,
735,738,741,750,327,327,742,327,399,743,744,745,755,746,756,751,772,776,718,720,
779,723,771,726,327,777,782,731,789,790,808,811,759,805,814,763,810,824,813,825,
827,154,836,837,842,774,775,846,847,508,683,166,402,747,649,131,396,647,607,678,
806,675,685,498,280,299,523,526,785,113,525,0,0,791,792,528,527,794,795,0,797,
762,0,0,766,768,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,327,0,815,0,0,816,0,817,818,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,800,0,0,831,804,0,0,834,835,0,0,809,0,0,0,0,0,
812,0,0,0,0,0,0,0,0,0,0,0,0,0,0,820,0,327,0,327,327,327,327,327,327,327,327,327,
327,327,327,327,327,327,327,327,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,327,2,3,4,119,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,727,
728,21,170,171,238,0,239,240,241,242,243,244,245,246,22,172,23,173,247,0,174,24,
248,0,0,27,28,249,175,176,250,251,177,178,179,180,29,30,31,252,181,0,253,182,0,
183,184,0,0,0,0,327,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254,185,186,187,188,33,34,35,
36,37,0,255,120,0,0,189,0,0,39,257,0,0,0,0,0,191,327,0,192,0,193,194,0,0,0,0,195,
196,2,3,4,119,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,786,787,21,170,171,238,
327,239,240,241,242,243,244,245,246,22,172,23,173,247,0,174,24,248,0,0,27,28,249,
175,176,250,251,177,178,179,180,29,30,31,252,181,0,253,182,0,183,184,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254,185,186,187,188,33,34,35,36,37,0,255,120,0,0,
189,327,0,39,257,0,0,0,0,327,191,0,0,192,0,193,194,0,0,0,0,195,196,2,3,4,119,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,238,0,239,240,241,242,243,
244,245,246,22,172,23,173,247,0,174,24,248,0,0,27,28,249,175,176,250,251,177,178,
179,180,29,30,31,252,181,0,253,182,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,254,185,186,187,188,33,34,35,36,37,0,255,120,256,0,189,0,0,39,257,0,0,0,0,
0,191,0,0,192,0,193,194,0,0,0,0,195,196,2,3,4,119,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,21,170,171,238,0,239,240,241,242,243,244,245,246,22,172,23,173,
247,0,174,24,248,0,0,27,28,249,175,176,250,251,177,178,179,180,29,30,31,252,181,
0,253,182,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254,185,186,187,
188,33,34,35,36,37,0,255,120,0,0,189,0,0,39,257,0,0,0,0,0,191,0,0,192,0,193,194,
0,0,0,0,195,196,2,3,4,119,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,
171,0,0,0,0,0,0,0,0,0,0,22,172,23,173,0,0,174,24,0,0,0,27,28,0,175,176,0,0,177,
178,179,180,29,30,31,0,181,0,0,182,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,32,185,186,187,188,33,34,35,36,37,0,568,0,0,0,189,0,0,39,257,0,0,0,0,0,191,
0,0,192,0,193,194,0,0,0,0,195,196,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,
170,171,0,0,0,0,0,0,0,0,0,0,22,172,0,173,0,0,174,0,0,0,0,27,28,0,175,176,0,0,177,
178,179,180,29,30,31,0,181,0,0,182,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,32,185,186,187,188,33,34,35,36,37,0,0,342,0,0,189,0,0,39,257,0,0,0,0,0,191,
0,0,192,0,193,194,0,0,0,0,195,196,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,
170,171,0,0,0,0,0,0,0,0,0,0,22,172,0,173,0,0,174,0,0,0,0,27,28,0,175,176,0,0,177,
178,179,180,29,30,31,0,181,0,0,182,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,32,185,186,187,188,33,34,35,36,37,0,0,0,0,0,189,0,0,39,257,0,0,0,0,0,191,0,
0,192,0,193,194,0,0,0,0,195,196,- 8,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,
0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,
0,0,0,0,33,34,35,36,37,0,0,0,- 8,0,38,0,0,39,40,0,0,0,0,0,41,42,43,- 8,1,0,2,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,
0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,33,34,35,36,37,0,0,0,0,0,38,0,0,39,40,0,
0,0,0,0,41,42,43,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,
0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,24,0,25,26,27,28,0,0,0,0,0,0,0,0,0,29,30,31,
0,0,0,1,0,2,3,4,119,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,32,0,0,0,
0,33,34,35,36,37,22,0,0,- 8,0,38,0,24,39,40,0,27,28,0,0,41,42,43,0,0,0,0,29,30,31,
0,0,0,1,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,32,0,0,0,0,
33,34,35,36,37,22,0,0,0,0,38,0,0,39,40,0,27,28,0,0,41,42,43,0,0,0,0,29,30,31,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,274,0,0,32,0,0,0,0,33,34,35,36,
37,0,276,0,0,0,317,- 159,0,39,40,0,- 159,0,307,0,41,42,43,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,27,28,0,0,0,
0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,274,0,0,
0,0,0,0,0,33,34,0,36,37,0,276,0,0,0,306,0,0,39,40,0,0,0,307,0,41,42,43,2,3,4,119,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,
0,0,0,0,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,2,3,4,119,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,33,34,0,36,37,0,0,120,22,223,
23,0,0,39,40,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,2,3,4,119,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,33,34,0,36,37,0,0,120,22,
0,23,0,0,39,40,24,0,0,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,2,3,4,119,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,33,34,0,36,37,22,0,0,0,
0,0,0,24,39,40,0,27,28,0,0,0,0,0,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,34,0,36,37,0,0,0,0,0,0,0,0,39,40,170,
171,238,0,239,240,241,242,243,244,245,246,0,172,0,173,247,0,174,0,248,0,0,0,0,
249,175,176,250,251,177,178,179,180,0,0,0,252,181,0,253,182,0,183,184,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254,185,186,187,188,0,0,35,0,0,0,255,120,0,0,189,
0,0,0,190,170,171,0,0,0,191,0,502,192,0,193,194,0,172,0,173,195,196,174,0,0,0,0,
0,0,0,175,176,0,0,177,178,179,180,0,0,0,0,181,0,0,182,0,183,184,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,32,185,186,187,188,0,0,35,0,0,0,0,328,503,0,189,0,0,0,
190,170,171,474,475,0,191,0,0,192,0,193,194,0,172,0,173,195,196,174,0,0,0,0,0,0,
0,175,176,0,0,177,178,179,180,0,0,0,0,181,0,0,182,0,183,184,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,32,185,186,187,188,0,0,35,0,0,0,0,328,694,0,189,0,0,0,190,
170,171,474,475,0,191,0,0,192,0,193,194,0,172,0,173,195,196,174,0,0,0,0,0,0,0,
175,176,0,0,177,178,179,180,0,0,0,0,181,0,0,182,0,183,184,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,32,185,186,187,188,0,0,35,0,0,0,0,328,783,0,189,0,0,0,190,
170,171,474,475,0,191,0,0,192,0,193,194,0,172,0,173,195,196,174,0,0,0,0,0,0,0,
175,176,0,0,177,178,179,180,0,0,0,0,181,0,0,182,0,183,184,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,32,185,186,187,188,0,0,35,0,0,0,0,328,0,0,189,0,0,0,190,170,
171,474,475,0,191,0,0,192,0,193,194,0,172,0,173,195,196,174,0,0,0,0,0,0,0,175,
176,0,0,177,178,179,180,0,0,0,0,181,0,0,182,0,183,184,170,171,0,0,0,0,0,0,0,0,0,
0,0,172,0,173,0,0,174,0,0,32,185,186,187,188,175,176,35,0,177,178,179,180,0,0,
189,0,181,0,190,182,0,183,184,283,191,0,0,192,0,193,194,0,0,0,0,195,196,0,0,0,0,
0,0,0,32,185,186,187,188,0,0,35,170,171,0,0,328,0,0,189,0,0,0,190,0,172,0,173,0,
191,174,0,192,0,193,194,0,0,175,176,195,196,177,178,179,180,0,0,0,0,181,0,0,182,
0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,185,186,187,188,0,0,35,
170,171,0,421,0,0,0,189,0,0,0,190,0,172,0,173,0,191,174,0,192,0,193,194,0,0,175,
176,195,196,177,178,179,180,0,0,0,0,181,0,0,182,0,183,184,170,171,0,0,0,0,0,0,0,
0,0,0,0,172,0,173,0,0,174,0,0,32,185,186,187,188,175,176,35,0,177,178,179,180,0,
0,189,0,181,0,190,182,0,183,184,491,191,0,0,192,0,193,194,0,0,0,0,195,196,0,0,0,
0,0,0,0,32,185,186,187,188,0,0,35,170,171,0,0,0,0,0,189,544,0,0,190,0,172,0,173,
0,191,174,0,192,0,193,194,0,0,175,176,195,196,177,178,179,180,0,0,0,0,181,0,0,
182,0,183,184,170,171,0,0,0,0,0,0,0,0,0,0,0,172,0,173,0,0,174,0,0,32,185,186,187,
188,175,176,35,0,177,178,179,180,0,0,189,574,181,0,190,182,0,183,184,0,191,0,0,
192,0,193,194,0,0,0,0,195,196,0,0,0,0,0,0,0,32,185,186,187,188,0,0,35,170,171,0,
0,0,0,0,189,0,0,0,190,0,172,0,173,619,191,174,0,192,0,193,194,0,0,175,176,195,
196,177,178,179,180,0,0,0,0,181,0,0,182,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,32,185,186,187,188,0,0,35,170,171,0,659,0,0,0,189,0,0,0,190,0,172,0,
173,0,191,174,0,192,0,193,194,0,0,175,176,195,196,177,178,179,180,0,0,0,0,181,0,
0,182,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,185,186,187,188,0,0,
35,170,171,0,661,0,0,0,189,0,0,0,190,0,172,0,173,0,191,174,0,192,0,193,194,0,0,
175,176,195,196,177,178,179,180,0,0,0,0,181,0,0,182,0,183,184,170,171,0,0,0,0,0,
0,0,0,0,0,0,172,0,173,0,0,174,0,0,32,185,186,187,188,175,176,35,0,177,178,179,
180,0,0,189,719,181,0,190,182,0,183,184,0,191,0,0,192,0,193,194,0,0,0,0,195,196,
0,0,0,0,0,0,0,32,185,186,187,188,0,0,35,170,171,0,0,0,0,0,189,722,0,0,190,0,172,
0,173,0,191,174,0,192,0,193,194,0,0,175,176,195,196,177,178,179,180,0,0,0,0,181,
0,0,182,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,185,186,187,188,0,
0,35,170,171,0,725,0,0,0,189,0,0,0,190,0,172,0,173,0,191,174,0,192,0,193,194,0,0,
175,176,195,196,177,178,179,180,0,0,0,0,181,0,0,182,0,183,184,170,171,0,0,0,0,0,
0,0,0,0,0,0,172,0,173,0,0,174,0,0,32,185,186,187,188,175,176,35,0,177,178,179,
180,0,0,189,761,181,0,190,182,0,183,184,0,191,0,0,192,0,193,194,0,0,0,0,195,196,
0,0,0,0,0,0,0,32,185,186,187,188,0,0,35,170,171,0,0,0,0,0,189,765,0,0,190,0,172,
0,173,0,191,174,0,192,0,193,194,0,0,175,176,195,196,177,178,179,180,0,0,0,0,181,
0,0,182,0,183,184,170,171,0,0,0,0,0,0,0,0,0,0,0,172,0,173,0,0,174,0,0,32,185,186,
187,188,175,176,35,0,177,178,179,180,0,0,189,767,181,0,190,182,0,183,184,0,191,0,
0,192,0,193,194,0,0,0,0,195,196,0,0,0,0,0,0,0,32,185,186,187,188,0,0,35,170,171,
0,0,0,0,0,189,799,0,0,190,0,172,0,173,0,191,174,0,192,0,193,194,0,0,175,176,195,
196,177,178,179,180,0,0,0,0,181,0,0,182,0,183,184,170,171,0,0,0,0,0,0,0,0,0,0,0,
172,0,0,0,0,0,0,0,32,185,186,187,188,175,176,35,0,177,178,179,180,0,0,189,0,0,0,
190,182,0,183,184,0,191,0,0,192,0,193,194,0,0,0,0,195,196,0,0,0,0,0,0,0,32,185,
186,187,188,0,0,35,170,171,0,0,328,0,0,189,0,0,0,190,0,172,0,0,0,191,0,0,192,0,
193,194,0,0,175,176,195,196,177,178,179,180,0,0,0,0,0,0,0,182,0,183,184,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,185,186,187,188,0,0,35,170,171,0,0,638,0,0,
189,0,0,0,190,0,172,0,0,0,191,0,0,192,0,193,194,0,0,175,176,195,196,177,178,179,
180,0,0,0,0,0,0,0,182,0,183,184,170,171,0,0,0,0,0,0,0,0,0,0,0,172,0,0,0,0,0,0,0,
32,185,186,187,188,175,176,35,0,177,178,179,180,0,0,323,0,0,0,190,182,0,183,184,
0,191,0,0,192,0,193,194,0,0,0,0,195,196,0,0,0,0,0,0,0,32,185,186,187,188,0,0,35,
170,171,0,0,0,0,0,339,0,0,0,190,0,172,0,0,0,191,0,0,192,0,193,194,0,0,175,176,
195,196,177,178,179,180,0,0,0,0,0,0,0,182,0,183,184,0,0,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,0,0,32,185,186,187,188,0,0,35,0,0,22,0,0,0,0,189,0,0,0,190,
0,27,28,0,0,191,0,0,192,0,193,194,29,30,31,0,195,196,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,0,0,0,0,0,0,274,0,0,275,0,0,22,0,33,34,0,36,37,0,276,0,0,27,28,
0,0,39,40,0,0,0,0,0,29,30,31,0,0,152,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,33,34,0,36,37,0,0,153,0,27,28,0,0,39,40,0,0,0,0,0,
29,30,31,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,274,0,0,
0,22,0,0,0,33,34,0,36,37,0,276,27,28,0,0,0,0,39,40,0,0,0,29,30,31,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,593,0,0,22,0,0,0,0,33,34,0,36,37,
0,27,28,0,0,0,0,0,39,40,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,33,34,0,36,37,0,0,0,0,0,0,0,0,39,40};static short Cyc_yycheck[
4972]={59,174,60,23,59,189,95,133,0,68,58,174,59,141,62,125,258,259,93,261,403,
223,298,159,160,125,173,61,549,1,22,23,322,25,156,445,59,385,95,89,90,91,71,5,6,
167,38,453,107,108,109,110,107,45,111,382,17,77,50,386,77,120,82,122,56,120,58,
122,125,61,107,129,64,71,484,72,73,425,70,108,96,45,119,489,107,77,191,192,64,
194,82,120,475,109,101,91,89,90,91,122,342,118,95,116,38,153,211,99,100,101,102,
716,125,126,111,111,50,638,111,91,96,119,120,120,116,91,98,103,76,1,91,123,125,
126,106,99,100,101,102,323,133,325,104,135,291,111,282,474,114,141,286,143,89,
443,93,339,107,829,111,151,113,289,452,285,112,224,838,106,119,224,102,135,555,
230,106,167,89,90,91,279,117,91,95,91,306,67,68,69,98,123,51,52,53,103,91,317,
189,802,111,258,259,224,261,258,259,102,261,156,96,97,107,617,125,126,308,112,
821,111,167,91,133,17,18,19,106,319,98,832,276,91,222,113,114,167,106,840,98,230,
104,127,232,111,108,235,116,117,118,304,305,306,300,96,310,104,102,103,403,108,1,
104,317,366,367,368,312,322,672,106,260,130,111,677,91,332,102,114,189,96,298,98,
108,96,114,342,90,319,118,342,222,104,109,105,285,154,155,622,108,289,232,113,
114,108,91,115,506,112,298,300,97,98,99,304,305,306,103,102,103,230,150,104,116,
117,118,108,317,317,260,319,475,322,323,515,325,609,101,223,91,92,460,106,332,
119,97,121,111,434,339,114,91,625,182,101,90,104,91,98,106,108,417,102,97,111,
417,106,114,106,149,426,1,495,102,426,778,116,117,118,108,434,437,102,107,108,
437,317,494,108,443,300,678,103,437,304,305,306,494,452,108,445,103,1,112,393,
234,395,317,455,189,111,555,322,323,241,325,518,437,70,71,247,102,307,250,251,
548,625,108,103,339,103,102,393,106,395,102,709,108,493,484,494,108,106,328,434,
106,106,91,104,334,335,336,108,443,114,472,116,117,118,476,345,91,452,102,634,
455,246,108,98,108,460,112,102,462,634,102,106,107,108,103,753,108,112,472,114,
618,108,476,585,92,112,102,91,107,108,380,593,696,642,98,385,462,556,102,494,495,
108,106,107,108,709,1,645,112,105,114,102,116,117,118,509,102,108,102,434,515,
582,108,102,108,582,107,771,443,358,359,360,361,108,586,425,93,452,323,589,455,
91,92,93,434,106,331,107,333,753,108,441,337,548,339,104,390,582,802,105,554,556,
74,75,750,91,351,561,629,813,107,108,629,509,408,106,410,107,108,494,495,5,6,107,
108,829,38,609,832,107,108,586,107,108,107,108,91,429,107,108,515,433,387,98,629,
106,496,107,108,107,108,106,104,107,108,609,506,112,554,678,711,116,117,118,38,
811,106,412,106,414,415,106,417,548,22,23,107,108,740,91,92,556,94,95,107,108,98,
432,107,108,107,108,711,106,106,22,23,109,110,549,106,113,114,106,116,106,38,107,
108,121,122,586,107,108,107,108,106,91,92,76,94,95,124,678,98,107,108,107,108,
107,108,123,106,652,653,109,110,356,357,362,363,122,116,103,103,727,104,121,122,
104,771,364,365,104,771,104,711,103,91,91,92,106,94,95,106,106,98,102,102,106,
106,91,112,727,106,625,106,109,110,104,119,802,105,633,116,802,107,115,638,121,
122,641,813,112,107,750,813,104,104,108,104,77,107,678,106,102,106,30,829,108,
111,832,829,102,112,832,42,96,91,564,106,96,107,568,569,78,79,80,81,82,83,84,85,
86,87,119,711,106,108,107,96,107,107,115,107,91,696,108,107,103,108,108,105,108,
104,107,811,108,107,709,107,104,654,102,106,657,107,102,102,25,115,104,120,111,
103,750,107,107,170,115,107,173,174,115,104,107,631,111,107,106,103,183,184,102,
91,102,96,108,106,191,192,107,194,753,107,107,107,28,107,103,108,104,96,658,659,
92,661,112,663,211,108,25,667,104,102,108,106,719,120,104,722,107,91,112,92,107,
811,107,107,107,732,733,0,0,332,601,94,230,693,556,62,222,554,477,593,777,589,
609,312,125,151,350,353,755,56,352,- 1,- 1,760,761,355,354,764,765,- 1,767,721,- 1,-
1,724,725,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,291,- 1,793,- 1,- 1,
796,- 1,798,799,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,769,- 1,- 1,819,773,
- 1,- 1,823,824,- 1,- 1,780,- 1,- 1,- 1,- 1,- 1,786,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,801,- 1,350,- 1,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,
367,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,403,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,
48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,68,69,- 1,- 1,- 1,- 1,
475,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,
100,- 1,102,103,- 1,- 1,106,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,518,- 1,119,- 1,121,122,
- 1,- 1,- 1,- 1,127,128,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
25,26,27,555,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,
51,52,53,54,55,56,57,58,59,60,61,62,63,- 1,65,66,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,100,- 1,102,103,
- 1,- 1,106,634,- 1,109,110,- 1,- 1,- 1,- 1,642,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,
127,128,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,
29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,
56,57,58,59,60,61,62,63,- 1,65,66,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,100,- 1,102,103,104,- 1,106,- 1,
- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,128,3,4,5,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,
35,36,37,38,39,40,41,- 1,43,44,45,- 1,- 1,48,49,50,51,52,53,54,55,56,57,58,59,60,61,
62,63,- 1,65,66,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,91,92,93,94,95,96,97,98,99,100,- 1,102,103,- 1,- 1,106,- 1,- 1,109,110,- 1,- 1,- 1,
- 1,- 1,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,128,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,
- 1,43,44,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,- 1,- 1,66,- 1,68,
69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,
96,97,98,99,100,- 1,102,- 1,- 1,- 1,106,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,-
1,121,122,- 1,- 1,- 1,- 1,127,128,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,
25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,
52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,- 1,- 1,66,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,100,- 1,- 1,103,- 1,
- 1,106,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,128,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,37,38,- 1,40,- 1,- 1,43,- 1,- 1,- 1,- 1,48,49,- 1,51,52,- 1,- 1,55,56,57,58,59,60,61,
- 1,63,- 1,- 1,66,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,91,92,93,94,95,96,97,98,99,100,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,109,110,- 1,- 1,- 1,-
1,- 1,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,128,0,1,- 1,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,
- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,
- 1,- 1,96,97,98,99,100,- 1,- 1,- 1,104,- 1,106,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,117,
118,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,96,97,98,99,100,- 1,- 1,- 1,- 1,- 1,106,- 1,
- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,117,118,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,
- 1,46,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,1,- 1,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,91,- 1,- 1,- 1,- 1,96,97,98,99,
100,37,- 1,- 1,104,- 1,106,- 1,44,109,110,- 1,48,49,- 1,- 1,116,117,118,- 1,- 1,- 1,- 1,59,
60,61,- 1,- 1,- 1,1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,96,97,98,99,100,37,- 1,- 1,- 1,- 1,106,- 1,- 1,109,110,- 1,48,
49,- 1,- 1,116,117,118,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,91,- 1,- 1,- 1,- 1,96,97,98,99,100,-
1,102,- 1,- 1,- 1,106,107,- 1,109,110,- 1,112,- 1,114,- 1,116,117,118,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,96,97,- 1,99,100,- 1,102,- 1,- 1,- 1,106,- 1,- 1,109,110,- 1,- 1,- 1,114,- 1,116,117,118,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,24,- 1,- 1,- 1,- 1,96,97,- 1,99,100,- 1,- 1,103,37,105,39,- 1,- 1,109,110,44,- 1,
- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,96,97,- 1,99,100,-
1,- 1,103,37,- 1,39,- 1,- 1,109,110,44,- 1,- 1,- 1,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,
60,61,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,97,- 1,99,100,37,- 1,- 1,- 1,- 1,- 1,- 1,44,109,110,- 1,48,49,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,97,- 1,99,100,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,109,110,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,43,-
1,45,- 1,- 1,- 1,- 1,50,51,52,53,54,55,56,57,58,- 1,- 1,- 1,62,63,- 1,65,66,- 1,68,69,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,
98,- 1,- 1,- 1,102,103,- 1,- 1,106,- 1,- 1,- 1,110,25,26,- 1,- 1,- 1,116,- 1,32,119,- 1,121,
122,- 1,38,- 1,40,127,128,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,
- 1,63,- 1,- 1,66,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,103,104,- 1,106,- 1,- 1,- 1,110,25,26,113,
114,- 1,116,- 1,- 1,119,- 1,121,122,- 1,38,- 1,40,127,128,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,
52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,103,104,
- 1,106,- 1,- 1,- 1,110,25,26,113,114,- 1,116,- 1,- 1,119,- 1,121,122,- 1,38,- 1,40,127,
128,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,
68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,
95,- 1,- 1,98,- 1,- 1,- 1,- 1,103,104,- 1,106,- 1,- 1,- 1,110,25,26,113,114,- 1,116,- 1,- 1,
119,- 1,121,122,- 1,38,- 1,40,127,128,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,- 1,- 1,55,56,57,
58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,110,
25,26,113,114,- 1,116,- 1,- 1,119,- 1,121,122,- 1,38,- 1,40,127,128,43,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,51,52,- 1,- 1,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,68,69,25,26,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,43,- 1,- 1,91,92,93,94,95,51,52,98,- 1,55,56,57,
58,- 1,- 1,106,- 1,63,- 1,110,66,- 1,68,69,115,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,
127,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,25,26,- 1,- 1,103,- 1,- 1,106,-
1,- 1,- 1,110,- 1,38,- 1,40,- 1,116,43,- 1,119,- 1,121,122,- 1,- 1,51,52,127,128,55,56,57,
58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,25,26,- 1,102,- 1,- 1,- 1,106,- 1,- 1,- 1,110,
- 1,38,- 1,40,- 1,116,43,- 1,119,- 1,121,122,- 1,- 1,51,52,127,128,55,56,57,58,- 1,- 1,- 1,
- 1,63,- 1,- 1,66,- 1,68,69,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,43,
- 1,- 1,91,92,93,94,95,51,52,98,- 1,55,56,57,58,- 1,- 1,106,- 1,63,- 1,110,66,- 1,68,69,
115,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,
94,95,- 1,- 1,98,25,26,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,110,- 1,38,- 1,40,- 1,116,43,- 1,
119,- 1,121,122,- 1,- 1,51,52,127,128,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,68,69,
25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,43,- 1,- 1,91,92,93,94,95,51,
52,98,- 1,55,56,57,58,- 1,- 1,106,107,63,- 1,110,66,- 1,68,69,- 1,116,- 1,- 1,119,- 1,121,
122,- 1,- 1,- 1,- 1,127,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,25,26,- 1,- 1,
- 1,- 1,- 1,106,- 1,- 1,- 1,110,- 1,38,- 1,40,115,116,43,- 1,119,- 1,121,122,- 1,- 1,51,52,
127,128,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,25,26,- 1,102,- 1,- 1,-
1,106,- 1,- 1,- 1,110,- 1,38,- 1,40,- 1,116,43,- 1,119,- 1,121,122,- 1,- 1,51,52,127,128,
55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,25,26,- 1,102,- 1,- 1,- 1,106,- 1,
- 1,- 1,110,- 1,38,- 1,40,- 1,116,43,- 1,119,- 1,121,122,- 1,- 1,51,52,127,128,55,56,57,
58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,68,69,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,
40,- 1,- 1,43,- 1,- 1,91,92,93,94,95,51,52,98,- 1,55,56,57,58,- 1,- 1,106,107,63,- 1,110,
66,- 1,68,69,- 1,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
91,92,93,94,95,- 1,- 1,98,25,26,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,110,- 1,38,- 1,40,- 1,
116,43,- 1,119,- 1,121,122,- 1,- 1,51,52,127,128,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,
- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,
94,95,- 1,- 1,98,25,26,- 1,102,- 1,- 1,- 1,106,- 1,- 1,- 1,110,- 1,38,- 1,40,- 1,116,43,- 1,
119,- 1,121,122,- 1,- 1,51,52,127,128,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,68,69,
25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,43,- 1,- 1,91,92,93,94,95,51,
52,98,- 1,55,56,57,58,- 1,- 1,106,107,63,- 1,110,66,- 1,68,69,- 1,116,- 1,- 1,119,- 1,121,
122,- 1,- 1,- 1,- 1,127,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,25,26,- 1,- 1,
- 1,- 1,- 1,106,107,- 1,- 1,110,- 1,38,- 1,40,- 1,116,43,- 1,119,- 1,121,122,- 1,- 1,51,52,
127,128,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,68,69,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,43,- 1,- 1,91,92,93,94,95,51,52,98,- 1,55,56,57,58,- 1,- 1,
106,107,63,- 1,110,66,- 1,68,69,- 1,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,128,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,25,26,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,110,
- 1,38,- 1,40,- 1,116,43,- 1,119,- 1,121,122,- 1,- 1,51,52,127,128,55,56,57,58,- 1,- 1,- 1,
- 1,63,- 1,- 1,66,- 1,68,69,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,91,92,93,94,95,51,52,98,- 1,55,56,57,58,- 1,- 1,106,- 1,- 1,- 1,110,66,- 1,68,69,
- 1,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,
95,- 1,- 1,98,25,26,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,110,- 1,38,- 1,- 1,- 1,116,- 1,- 1,119,
- 1,121,122,- 1,- 1,51,52,127,128,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,
98,25,26,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,110,- 1,38,- 1,- 1,- 1,116,- 1,- 1,119,- 1,121,
122,- 1,- 1,51,52,127,128,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,25,26,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,51,52,98,- 1,55,
56,57,58,- 1,- 1,106,- 1,- 1,- 1,110,66,- 1,68,69,- 1,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,
- 1,127,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,25,26,- 1,- 1,- 1,- 1,- 1,106,
- 1,- 1,- 1,110,- 1,38,- 1,- 1,- 1,116,- 1,- 1,119,- 1,121,122,- 1,- 1,51,52,127,128,55,56,
57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,37,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,
110,- 1,48,49,- 1,- 1,116,- 1,- 1,119,- 1,121,122,59,60,61,- 1,127,128,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,91,- 1,- 1,37,- 1,96,97,
- 1,99,100,- 1,102,- 1,- 1,48,49,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,- 1,96,97,- 1,99,100,- 1,- 1,103,- 1,48,49,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,59,60,61,-
1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,
- 1,- 1,37,- 1,- 1,- 1,96,97,- 1,99,100,- 1,102,48,49,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,59,
60,61,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,88,- 1,- 1,37,- 1,- 1,- 1,- 1,96,97,- 1,99,100,- 1,48,49,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,
59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,97,- 1,99,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110};
char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible";char Cyc_Yystack_overflow[
21]="\000\000\000\000Yystack_overflow";void Cyc_yyerror(struct _tagged_arr);int Cyc_yylex();
static int Cyc_yychar=(int)'\000';void*Cyc_yylval=(void*)Cyc_YYINITIALSVAL;static
int Cyc_yynerrs=0;struct _tuple16{struct _tagged_arr f1;void*f2;};static char _tmp390[
8]="stdcall";static char _tmp391[6]="cdecl";static char _tmp392[9]="fastcall";static
char _tmp393[9]="noreturn";static char _tmp394[6]="const";static char _tmp395[8]="aligned";
static char _tmp396[7]="packed";static char _tmp397[7]="shared";static char _tmp398[7]="unused";
static char _tmp399[5]="weak";static char _tmp39A[10]="dllimport";static char _tmp39B[
10]="dllexport";static char _tmp39C[23]="no_instrument_function";static char _tmp39D[
12]="constructor";static char _tmp39E[11]="destructor";static char _tmp39F[22]="no_check_memory_usage";
struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(){
int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int yyssp_offset;short*yyss=(
short*)({unsigned int _tmp86C=10000;short*_tmp86D=(short*)_cycalloc_atomic(
_check_times(sizeof(short),_tmp86C));{unsigned int _tmp86E=_tmp86C;unsigned int i;
for(i=0;i < _tmp86E;i ++){_tmp86D[i]=(short)0;}}_tmp86D;});int yyvsp_offset;void**
yyvs=(void**)({unsigned int _tmp869=10000;void**_tmp86A=(void**)_cycalloc(
_check_times(sizeof(void*),_tmp869));{unsigned int _tmp86B=_tmp869;unsigned int i;
for(i=0;i < _tmp86B;i ++){_tmp86A[i]=Cyc_yylval;}}_tmp86A;});int yylsp_offset;
struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmp866=10000;struct Cyc_Yyltype*
_tmp867=(struct Cyc_Yyltype*)_cycalloc_atomic(_check_times(sizeof(struct Cyc_Yyltype),
_tmp866));{unsigned int _tmp868=_tmp866;unsigned int i;for(i=0;i < _tmp868;i ++){
_tmp867[i]=Cyc_yynewloc();}}_tmp867;});int yystacksize=10000;void*yyval=Cyc_yylval;
int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;
yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(
10000,++ yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(
yystacksize >= 10000){Cyc_yyerror(_tag_arr("parser stack overflow",sizeof(char),
22));(int)_throw((void*)Cyc_Yystack_overflow);}}goto yybackup;yybackup: yyn=(int)
Cyc_yypact[yystate];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=
Cyc_yylex();if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=(Cyc_yychar > 
0?Cyc_yychar <= 356: 0)?(int)Cyc_yytranslate[_check_known_subscript_notnull(357,
Cyc_yychar)]: 243;}yyn +=yychar1;if((yyn < 0?1: yyn > 4971)?1: Cyc_yycheck[yyn]!= 
yychar1)goto yydefault;yyn=(int)Cyc_yytable[yyn];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 847)
return 0;if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;if(yyerrstatus != 0)yyerrstatus --;yystate=yyn;goto
yynewstate;yydefault: yyn=(int)Cyc_yydefact[yystate];if(yyn == 0)goto yyerrlab;
yyreduce: yylen=(int)Cyc_yyr2[yyn];if(yylen > 0)yyval=yyvs[
_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){case
1: _LL1BF: yyval=yyvs[yyvsp_offset];Cyc_Parse_parse_result=Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]);break;case 2: _LL1C0: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp309=_cycalloc(sizeof(*_tmp309));_tmp309[0]=({struct Cyc_DeclList_tok_struct
_tmp30A;_tmp30A.tag=Cyc_DeclList_tok;_tmp30A.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]));_tmp30A;});_tmp309;});break;case 3: _LL1C1: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B[0]=({
struct Cyc_DeclList_tok_struct _tmp30C;_tmp30C.tag=Cyc_DeclList_tok;_tmp30C.f1=({
struct Cyc_List_List*_tmp30D=_cycalloc(sizeof(*_tmp30D));_tmp30D->hd=({struct Cyc_Absyn_Decl*
_tmp30E=_cycalloc(sizeof(*_tmp30E));_tmp30E->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F[0]=({struct Cyc_Absyn_Using_d_struct
_tmp310;_tmp310.tag=9;_tmp310.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp310.f2=Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]);_tmp310;});_tmp30F;}));_tmp30E->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp30E;});_tmp30D->tl=0;_tmp30D;});_tmp30C;});_tmp30B;});
Cyc_Lex_leave_using();break;case 4: _LL1C2: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp311=_cycalloc(sizeof(*_tmp311));_tmp311[0]=({struct Cyc_DeclList_tok_struct
_tmp312;_tmp312.tag=Cyc_DeclList_tok;_tmp312.f1=({struct Cyc_List_List*_tmp313=
_cycalloc(sizeof(*_tmp313));_tmp313->hd=({struct Cyc_Absyn_Decl*_tmp314=_cycalloc(
sizeof(*_tmp314));_tmp314->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp315=_cycalloc(sizeof(*_tmp315));_tmp315[0]=({struct Cyc_Absyn_Using_d_struct
_tmp316;_tmp316.tag=9;_tmp316.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp316.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp316;});_tmp315;}));
_tmp314->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp314;});_tmp313->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp313;});_tmp312;});_tmp311;});break;case 5: _LL1C3: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317[0]=({
struct Cyc_DeclList_tok_struct _tmp318;_tmp318.tag=Cyc_DeclList_tok;_tmp318.f1=({
struct Cyc_List_List*_tmp319=_cycalloc(sizeof(*_tmp319));_tmp319->hd=({struct Cyc_Absyn_Decl*
_tmp31A=_cycalloc(sizeof(*_tmp31A));_tmp31A->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp31B=_cycalloc(sizeof(*_tmp31B));_tmp31B[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp31C;_tmp31C.tag=8;_tmp31C.f1=({struct _tagged_arr*_tmp31D=_cycalloc(sizeof(*
_tmp31D));_tmp31D[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp31D;});_tmp31C.f2=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp31C;});_tmp31B;}));_tmp31A->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp31A;});_tmp319->tl=0;_tmp319;});_tmp318;});_tmp317;});
Cyc_Lex_leave_namespace();break;case 6: _LL1C4: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp31E=_cycalloc(sizeof(*_tmp31E));_tmp31E[0]=({struct Cyc_DeclList_tok_struct
_tmp31F;_tmp31F.tag=Cyc_DeclList_tok;_tmp31F.f1=({struct Cyc_List_List*_tmp320=
_cycalloc(sizeof(*_tmp320));_tmp320->hd=({struct Cyc_Absyn_Decl*_tmp321=_cycalloc(
sizeof(*_tmp321));_tmp321->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp323;_tmp323.tag=8;_tmp323.f1=({struct _tagged_arr*_tmp324=_cycalloc(sizeof(*
_tmp324));_tmp324[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp324;});_tmp323.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp323;});_tmp322;}));
_tmp321->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp321;});_tmp320->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp320;});_tmp31F;});_tmp31E;});break;case 7: _LL1C5: if(Cyc_Std_strcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tag_arr("C",sizeof(char),2))!= 0)Cyc_Parse_err(_tag_arr("only extern \"C\" { ... } is allowed",
sizeof(char),35),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp325=_cycalloc(sizeof(*_tmp325));_tmp325[0]=({
struct Cyc_DeclList_tok_struct _tmp326;_tmp326.tag=Cyc_DeclList_tok;_tmp326.f1=({
struct Cyc_List_List*_tmp327=_cycalloc(sizeof(*_tmp327));_tmp327->hd=({struct Cyc_Absyn_Decl*
_tmp328=_cycalloc(sizeof(*_tmp328));_tmp328->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp329=_cycalloc(sizeof(*_tmp329));_tmp329[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp32A;_tmp32A.tag=10;_tmp32A.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp32A;});_tmp329;}));
_tmp328->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp328;});_tmp327->tl=Cyc_yyget_DeclList_tok(yyvs[
yyvsp_offset]);_tmp327;});_tmp326;});_tmp325;});break;case 8: _LL1C6: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B[0]=({
struct Cyc_DeclList_tok_struct _tmp32C;_tmp32C.tag=Cyc_DeclList_tok;_tmp32C.f1=0;
_tmp32C;});_tmp32B;});break;case 9: _LL1C7: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp32D=_cycalloc(sizeof(*_tmp32D));_tmp32D[0]=({struct Cyc_DeclList_tok_struct
_tmp32E;_tmp32E.tag=Cyc_DeclList_tok;_tmp32E.f1=({struct Cyc_List_List*_tmp32F=
_cycalloc(sizeof(*_tmp32F));_tmp32F->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp330=_cycalloc(sizeof(*_tmp330));_tmp330[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp331;_tmp331.tag=1;_tmp331.f1=Cyc_yyget_FnDecl_tok(yyvs[yyvsp_offset]);
_tmp331;});_tmp330;}),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp32F->tl=0;_tmp32F;});_tmp32E;});_tmp32D;});
break;case 10: _LL1C8: yyval=yyvs[yyvsp_offset];break;case 11: _LL1C9: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp332=_cycalloc(sizeof(*_tmp332));_tmp332[0]=({
struct Cyc_FnDecl_tok_struct _tmp333;_tmp333.tag=Cyc_FnDecl_tok;_tmp333.f1=Cyc_Parse_make_function(
0,Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp333;});_tmp332;});break;case 12: _LL1CA: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp334=_cycalloc(sizeof(*_tmp334));_tmp334[0]=({
struct Cyc_FnDecl_tok_struct _tmp335;_tmp335.tag=Cyc_FnDecl_tok;_tmp335.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp336=_cycalloc(sizeof(*_tmp336));_tmp336->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp336;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp335;});_tmp334;});break;case 13: _LL1CB: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp337=_cycalloc(sizeof(*_tmp337));_tmp337[0]=({
struct Cyc_FnDecl_tok_struct _tmp338;_tmp338.tag=Cyc_FnDecl_tok;_tmp338.f1=Cyc_Parse_make_function(
0,Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp338;});_tmp337;});break;case 14: _LL1CC: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339[0]=({
struct Cyc_FnDecl_tok_struct _tmp33A;_tmp33A.tag=Cyc_FnDecl_tok;_tmp33A.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp33B=_cycalloc(sizeof(*_tmp33B));_tmp33B->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp33B;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp33A;});_tmp339;});break;case 15: _LL1CD: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C[0]=({
struct Cyc_FnDecl_tok_struct _tmp33D;_tmp33D.tag=Cyc_FnDecl_tok;_tmp33D.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp33E=_cycalloc(sizeof(*_tmp33E));_tmp33E->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp33E;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp33D;});_tmp33C;});break;case 16: _LL1CE: yyval=(void*)({
struct Cyc_FnDecl_tok_struct*_tmp33F=_cycalloc(sizeof(*_tmp33F));_tmp33F[0]=({
struct Cyc_FnDecl_tok_struct _tmp340;_tmp340.tag=Cyc_FnDecl_tok;_tmp340.f1=Cyc_Parse_make_function(({
struct Cyc_Core_Opt*_tmp341=_cycalloc(sizeof(*_tmp341));_tmp341->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp341;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp340;});_tmp33F;});break;case 17: _LL1CF: Cyc_Lex_enter_using(
Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]));yyval=yyvs[yyvsp_offset];break;case 18:
_LL1D0: Cyc_Lex_leave_using();break;case 19: _LL1D1: Cyc_Lex_enter_namespace(({
struct _tagged_arr*_tmp342=_cycalloc(sizeof(*_tmp342));_tmp342[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp342;}));yyval=yyvs[yyvsp_offset];break;case 20: _LL1D2: Cyc_Lex_leave_namespace();
break;case 21: _LL1D3: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp343=
_cycalloc(sizeof(*_tmp343));_tmp343[0]=({struct Cyc_DeclList_tok_struct _tmp344;
_tmp344.tag=Cyc_DeclList_tok;_tmp344.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp344;});
_tmp343;});break;case 22: _LL1D4: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345[0]=({struct Cyc_DeclList_tok_struct
_tmp346;_tmp346.tag=Cyc_DeclList_tok;_tmp346.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp346;});_tmp345;});break;case 23: _LL1D5: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347[0]=({
struct Cyc_DeclList_tok_struct _tmp348;_tmp348.tag=Cyc_DeclList_tok;_tmp348.f1=({
struct Cyc_List_List*_tmp349=_cycalloc(sizeof(*_tmp349));_tmp349->hd=Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
0,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp349->tl=0;
_tmp349;});_tmp348;});_tmp347;});break;case 24: _LL1D6: {struct Cyc_List_List*
_tmp34A=0;{struct Cyc_List_List*_tmp34B=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp34B != 0;
_tmp34B=_tmp34B->tl){struct _tagged_arr*_tmp34C=(struct _tagged_arr*)_tmp34B->hd;
struct _tuple1*_tmp34D=({struct _tuple1*_tmp350=_cycalloc(sizeof(*_tmp350));
_tmp350->f1=Cyc_Absyn_rel_ns_null;_tmp350->f2=_tmp34C;_tmp350;});struct Cyc_Absyn_Vardecl*
_tmp34E=Cyc_Absyn_new_vardecl(_tmp34D,Cyc_Absyn_wildtyp(0),0);_tmp34A=({struct
Cyc_List_List*_tmp34F=_cycalloc(sizeof(*_tmp34F));_tmp34F->hd=_tmp34E;_tmp34F->tl=
_tmp34A;_tmp34F;});}}_tmp34A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp34A);yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp351=
_cycalloc(sizeof(*_tmp351));_tmp351[0]=({struct Cyc_DeclList_tok_struct _tmp352;
_tmp352.tag=Cyc_DeclList_tok;_tmp352.f1=({struct Cyc_List_List*_tmp353=_cycalloc(
sizeof(*_tmp353));_tmp353->hd=Cyc_Absyn_letv_decl(_tmp34A,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp353->tl=0;_tmp353;});_tmp352;});_tmp351;});break;}
case 25: _LL1D7: yyval=yyvs[yyvsp_offset];break;case 26: _LL1D8: yyval=(void*)({struct
Cyc_DeclList_tok_struct*_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354[0]=({struct
Cyc_DeclList_tok_struct _tmp355;_tmp355.tag=Cyc_DeclList_tok;_tmp355.f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_yyget_DeclList_tok(yyvs[yyvsp_offset]));_tmp355;});_tmp354;});break;case 27:
_LL1D9: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp356=_cycalloc(sizeof(*
_tmp356));_tmp356[0]=({struct Cyc_DeclSpec_tok_struct _tmp357;_tmp357.tag=Cyc_DeclSpec_tok;
_tmp357.f1=({struct Cyc_Parse_Declaration_spec*_tmp358=_cycalloc(sizeof(*_tmp358));
_tmp358->sc=({struct Cyc_Core_Opt*_tmp359=_cycalloc(sizeof(*_tmp359));_tmp359->v=(
void*)Cyc_yyget_StorageClass_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp359;});_tmp358->tq=Cyc_Absyn_empty_tqual();_tmp358->type_specs=
0;_tmp358->is_inline=0;_tmp358->attributes=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp358;});_tmp357;});_tmp356;});break;case 28: _LL1DA: if((Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->sc != 0)({void*_tmp35A[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tag_arr("Only one storage class is allowed in a declaration",
sizeof(char),51),_tag_arr(_tmp35A,sizeof(void*),0));});yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp35B=_cycalloc(sizeof(*_tmp35B));_tmp35B[0]=({struct Cyc_DeclSpec_tok_struct
_tmp35C;_tmp35C.tag=Cyc_DeclSpec_tok;_tmp35C.f1=({struct Cyc_Parse_Declaration_spec*
_tmp35D=_cycalloc(sizeof(*_tmp35D));_tmp35D->sc=({struct Cyc_Core_Opt*_tmp35E=
_cycalloc(sizeof(*_tmp35E));_tmp35E->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp35E;});_tmp35D->tq=(
Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq;_tmp35D->type_specs=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp35D->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->is_inline;_tmp35D->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp35D;});_tmp35C;});_tmp35B;});break;case 29:
_LL1DB: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp35F=_cycalloc(sizeof(*
_tmp35F));_tmp35F[0]=({struct Cyc_DeclSpec_tok_struct _tmp360;_tmp360.tag=Cyc_DeclSpec_tok;
_tmp360.f1=({struct Cyc_Parse_Declaration_spec*_tmp361=_cycalloc(sizeof(*_tmp361));
_tmp361->sc=0;_tmp361->tq=Cyc_Absyn_empty_tqual();_tmp361->type_specs=({struct
Cyc_List_List*_tmp362=_cycalloc(sizeof(*_tmp362));_tmp362->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp362->tl=0;
_tmp362;});_tmp361->is_inline=0;_tmp361->attributes=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp361;});_tmp360;});_tmp35F;});break;case 30: _LL1DC: yyval=(
void*)({struct Cyc_DeclSpec_tok_struct*_tmp363=_cycalloc(sizeof(*_tmp363));
_tmp363[0]=({struct Cyc_DeclSpec_tok_struct _tmp364;_tmp364.tag=Cyc_DeclSpec_tok;
_tmp364.f1=({struct Cyc_Parse_Declaration_spec*_tmp365=_cycalloc(sizeof(*_tmp365));
_tmp365->sc=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->sc;_tmp365->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->tq;_tmp365->type_specs=({struct Cyc_List_List*_tmp366=
_cycalloc(sizeof(*_tmp366));_tmp366->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp366->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp366;});_tmp365->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->is_inline;_tmp365->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp365;});_tmp364;});_tmp363;});break;case 31:
_LL1DD: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp367=_cycalloc(sizeof(*
_tmp367));_tmp367[0]=({struct Cyc_DeclSpec_tok_struct _tmp368;_tmp368.tag=Cyc_DeclSpec_tok;
_tmp368.f1=({struct Cyc_Parse_Declaration_spec*_tmp369=_cycalloc(sizeof(*_tmp369));
_tmp369->sc=0;_tmp369->tq=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp369->type_specs=0;
_tmp369->is_inline=0;_tmp369->attributes=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp369;});_tmp368;});_tmp367;});break;case 32: _LL1DE: yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A[0]=({
struct Cyc_DeclSpec_tok_struct _tmp36B;_tmp36B.tag=Cyc_DeclSpec_tok;_tmp36B.f1=({
struct Cyc_Parse_Declaration_spec*_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C->sc=(
Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->sc;_tmp36C->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq);_tmp36C->type_specs=(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->type_specs;_tmp36C->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->is_inline;_tmp36C->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp36C;});_tmp36B;});_tmp36A;});break;case 33:
_LL1DF: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp36D=_cycalloc(sizeof(*
_tmp36D));_tmp36D[0]=({struct Cyc_DeclSpec_tok_struct _tmp36E;_tmp36E.tag=Cyc_DeclSpec_tok;
_tmp36E.f1=({struct Cyc_Parse_Declaration_spec*_tmp36F=_cycalloc(sizeof(*_tmp36F));
_tmp36F->sc=0;_tmp36F->tq=Cyc_Absyn_empty_tqual();_tmp36F->type_specs=0;_tmp36F->is_inline=
1;_tmp36F->attributes=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp36F;});
_tmp36E;});_tmp36D;});break;case 34: _LL1E0: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp370=_cycalloc(sizeof(*_tmp370));_tmp370[0]=({struct Cyc_DeclSpec_tok_struct
_tmp371;_tmp371.tag=Cyc_DeclSpec_tok;_tmp371.f1=({struct Cyc_Parse_Declaration_spec*
_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
yyvsp_offset]))->sc;_tmp372->tq=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->tq;
_tmp372->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[yyvsp_offset]))->type_specs;
_tmp372->is_inline=1;_tmp372->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[yyvsp_offset]))->attributes);_tmp372;});_tmp371;});_tmp370;});break;case 35:
_LL1E1: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp373=_cycalloc(
sizeof(*_tmp373));_tmp373[0]=({struct Cyc_StorageClass_tok_struct _tmp374;_tmp374.tag=
Cyc_StorageClass_tok;_tmp374.f1=(void*)((void*)4);_tmp374;});_tmp373;});break;
case 36: _LL1E2: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp375=_cycalloc(
sizeof(*_tmp375));_tmp375[0]=({struct Cyc_StorageClass_tok_struct _tmp376;_tmp376.tag=
Cyc_StorageClass_tok;_tmp376.f1=(void*)((void*)5);_tmp376;});_tmp375;});break;
case 37: _LL1E3: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp377=_cycalloc(
sizeof(*_tmp377));_tmp377[0]=({struct Cyc_StorageClass_tok_struct _tmp378;_tmp378.tag=
Cyc_StorageClass_tok;_tmp378.f1=(void*)((void*)3);_tmp378;});_tmp377;});break;
case 38: _LL1E4: yyval=(void*)({struct Cyc_StorageClass_tok_struct*_tmp379=_cycalloc(
sizeof(*_tmp379));_tmp379[0]=({struct Cyc_StorageClass_tok_struct _tmp37A;_tmp37A.tag=
Cyc_StorageClass_tok;_tmp37A.f1=(void*)((void*)1);_tmp37A;});_tmp379;});break;
case 39: _LL1E5: if(Cyc_Std_strcmp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),
_tag_arr("C",sizeof(char),2))!= 0)Cyc_Parse_err(_tag_arr("only extern or extern \"C\" is allowed",
sizeof(char),37),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B[0]=({struct Cyc_StorageClass_tok_struct
_tmp37C;_tmp37C.tag=Cyc_StorageClass_tok;_tmp37C.f1=(void*)((void*)2);_tmp37C;});
_tmp37B;});break;case 40: _LL1E6: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp37D=_cycalloc(sizeof(*_tmp37D));_tmp37D[0]=({struct Cyc_StorageClass_tok_struct
_tmp37E;_tmp37E.tag=Cyc_StorageClass_tok;_tmp37E.f1=(void*)((void*)0);_tmp37E;});
_tmp37D;});break;case 41: _LL1E7: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp37F=_cycalloc(sizeof(*_tmp37F));_tmp37F[0]=({struct Cyc_StorageClass_tok_struct
_tmp380;_tmp380.tag=Cyc_StorageClass_tok;_tmp380.f1=(void*)((void*)6);_tmp380;});
_tmp37F;});break;case 42: _LL1E8: yyval=(void*)({struct Cyc_AttributeList_tok_struct*
_tmp381=_cycalloc(sizeof(*_tmp381));_tmp381[0]=({struct Cyc_AttributeList_tok_struct
_tmp382;_tmp382.tag=Cyc_AttributeList_tok;_tmp382.f1=0;_tmp382;});_tmp381;});
break;case 43: _LL1E9: yyval=yyvs[yyvsp_offset];break;case 44: _LL1EA: yyval=(void*)({
struct Cyc_AttributeList_tok_struct*_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383[0]=({
struct Cyc_AttributeList_tok_struct _tmp384;_tmp384.tag=Cyc_AttributeList_tok;
_tmp384.f1=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp384;});_tmp383;});break;case 45: _LL1EB: yyval=(void*)({
struct Cyc_AttributeList_tok_struct*_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385[0]=({
struct Cyc_AttributeList_tok_struct _tmp386;_tmp386.tag=Cyc_AttributeList_tok;
_tmp386.f1=({struct Cyc_List_List*_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387->hd=(
void*)Cyc_yyget_Attribute_tok(yyvs[yyvsp_offset]);_tmp387->tl=0;_tmp387;});
_tmp386;});_tmp385;});break;case 46: _LL1EC: yyval=(void*)({struct Cyc_AttributeList_tok_struct*
_tmp388=_cycalloc(sizeof(*_tmp388));_tmp388[0]=({struct Cyc_AttributeList_tok_struct
_tmp389;_tmp389.tag=Cyc_AttributeList_tok;_tmp389.f1=({struct Cyc_List_List*
_tmp38A=_cycalloc(sizeof(*_tmp38A));_tmp38A->hd=(void*)Cyc_yyget_Attribute_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp38A->tl=Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]);_tmp38A;});_tmp389;});_tmp388;});break;case 47: _LL1ED: {
static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};static struct _tuple16
att_map[16]={{{_tmp390,_tmp390,_tmp390 + 8},(void*)0},{{_tmp391,_tmp391,_tmp391 + 
6},(void*)1},{{_tmp392,_tmp392,_tmp392 + 9},(void*)2},{{_tmp393,_tmp393,_tmp393 + 
9},(void*)3},{{_tmp394,_tmp394,_tmp394 + 6},(void*)4},{{_tmp395,_tmp395,_tmp395 + 
8},(void*)& att_aligned},{{_tmp396,_tmp396,_tmp396 + 7},(void*)5},{{_tmp397,
_tmp397,_tmp397 + 7},(void*)7},{{_tmp398,_tmp398,_tmp398 + 7},(void*)8},{{_tmp399,
_tmp399,_tmp399 + 5},(void*)9},{{_tmp39A,_tmp39A,_tmp39A + 10},(void*)10},{{
_tmp39B,_tmp39B,_tmp39B + 10},(void*)11},{{_tmp39C,_tmp39C,_tmp39C + 23},(void*)12},{{
_tmp39D,_tmp39D,_tmp39D + 12},(void*)13},{{_tmp39E,_tmp39E,_tmp39E + 11},(void*)14},{{
_tmp39F,_tmp39F,_tmp39F + 22},(void*)15}};struct _tagged_arr _tmp38B=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);if((((_get_arr_size(_tmp38B,sizeof(char))> 4?*((const char*)
_check_unknown_subscript(_tmp38B,sizeof(char),0))== '_': 0)?*((const char*)
_check_unknown_subscript(_tmp38B,sizeof(char),1))== '_': 0)?*((const char*)
_check_unknown_subscript(_tmp38B,sizeof(char),(int)(_get_arr_size(_tmp38B,
sizeof(char))- 2)))== '_': 0)?*((const char*)_check_unknown_subscript(_tmp38B,
sizeof(char),(int)(_get_arr_size(_tmp38B,sizeof(char))- 3)))== '_': 0)_tmp38B=(
struct _tagged_arr)Cyc_Std_substring(_tmp38B,2,_get_arr_size(_tmp38B,sizeof(char))
- 5);{int i=0;for(0;i < 16;++ i){if(Cyc_Std_strcmp(_tmp38B,(att_map[i]).f1)== 0){
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp38C=_cycalloc(sizeof(*_tmp38C));
_tmp38C[0]=({struct Cyc_Attribute_tok_struct _tmp38D;_tmp38D.tag=Cyc_Attribute_tok;
_tmp38D.f1=(void*)(att_map[i]).f2;_tmp38D;});_tmp38C;});break;}}if(i == 16){Cyc_Parse_err(
_tag_arr("unrecognized attribute",sizeof(char),23),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E[0]=({
struct Cyc_Attribute_tok_struct _tmp38F;_tmp38F.tag=Cyc_Attribute_tok;_tmp38F.f1=(
void*)((void*)1);_tmp38F;});_tmp38E;});}break;}}case 48: _LL1EE: yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1[0]=({
struct Cyc_Attribute_tok_struct _tmp3A2;_tmp3A2.tag=Cyc_Attribute_tok;_tmp3A2.f1=(
void*)((void*)4);_tmp3A2;});_tmp3A1;});break;case 49: _LL1EF: {struct _tagged_arr
_tmp3A3=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);struct _tuple13 _tmp3A5;int _tmp3A6;struct _tuple13*_tmp3A4=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3A5=*_tmp3A4;
_tmp3A6=_tmp3A5.f2;{void*a;if(Cyc_Std_zstrcmp(_tmp3A3,_tag_arr("regparm",sizeof(
char),8))== 0?1: Cyc_Std_zstrcmp(_tmp3A3,_tag_arr("__regparm__",sizeof(char),12))
== 0){if(_tmp3A6 < 0?1: _tmp3A6 > 3)Cyc_Parse_err(_tag_arr("regparm requires value between 0 and 3",
sizeof(char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp3A7=_cycalloc_atomic(sizeof(*_tmp3A7));
_tmp3A7[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp3A8;_tmp3A8.tag=0;_tmp3A8.f1=
_tmp3A6;_tmp3A8;});_tmp3A7;});}else{if(Cyc_Std_zstrcmp(_tmp3A3,_tag_arr("aligned",
sizeof(char),8))== 0?1: Cyc_Std_zstrcmp(_tmp3A3,_tag_arr("__aligned__",sizeof(
char),12))== 0){if(_tmp3A6 < 0)Cyc_Parse_err(_tag_arr("aligned requires positive power of two",
sizeof(char),39),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp3A6;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(_tag_arr("aligned requires positive power of two",sizeof(char),39),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));a=(void*)({struct Cyc_Absyn_Aligned_att_struct*
_tmp3A9=_cycalloc_atomic(sizeof(*_tmp3A9));_tmp3A9[0]=({struct Cyc_Absyn_Aligned_att_struct
_tmp3AA;_tmp3AA.tag=1;_tmp3AA.f1=_tmp3A6;_tmp3AA;});_tmp3A9;});}}else{Cyc_Parse_err(
_tag_arr("unrecognized attribute",sizeof(char),23),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));
_tmp3AB[0]=({struct Cyc_Attribute_tok_struct _tmp3AC;_tmp3AC.tag=Cyc_Attribute_tok;
_tmp3AC.f1=(void*)a;_tmp3AC;});_tmp3AB;});break;}}case 50: _LL1F0: {struct
_tagged_arr _tmp3AD=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tagged_arr _tmp3AE=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_Std_zstrcmp(
_tmp3AD,_tag_arr("section",sizeof(char),8))== 0?1: Cyc_Std_zstrcmp(_tmp3AD,
_tag_arr("__section__",sizeof(char),12))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF[0]=({struct Cyc_Absyn_Section_att_struct
_tmp3B0;_tmp3B0.tag=2;_tmp3B0.f1=_tmp3AE;_tmp3B0;});_tmp3AF;});else{Cyc_Parse_err(
_tag_arr("unrecognized attribute",sizeof(char),23),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));
_tmp3B1[0]=({struct Cyc_Attribute_tok_struct _tmp3B2;_tmp3B2.tag=Cyc_Attribute_tok;
_tmp3B2.f1=(void*)a;_tmp3B2;});_tmp3B1;});break;}case 51: _LL1F1: {struct
_tagged_arr _tmp3B3=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _tagged_arr _tmp3B4=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp3B6;struct
_tuple13 _tmp3B5=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp3B6=_tmp3B5.f2;{int _tmp3B8;struct _tuple13 _tmp3B7=*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3B8=_tmp3B7.f2;{
void*a=(void*)1;if(Cyc_Std_zstrcmp(_tmp3B3,_tag_arr("format",sizeof(char),7))== 
0?1: Cyc_Std_zstrcmp(_tmp3B3,_tag_arr("__format__",sizeof(char),11))== 0){if(Cyc_Std_zstrcmp(
_tmp3B4,_tag_arr("printf",sizeof(char),7))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*
_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9[0]=({struct Cyc_Absyn_Format_att_struct
_tmp3BA;_tmp3BA.tag=3;_tmp3BA.f1=(void*)((void*)0);_tmp3BA.f2=_tmp3B6;_tmp3BA.f3=
_tmp3B8;_tmp3BA;});_tmp3B9;});else{if(Cyc_Std_zstrcmp(_tmp3B4,_tag_arr("scanf",
sizeof(char),6))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp3BB=
_cycalloc(sizeof(*_tmp3BB));_tmp3BB[0]=({struct Cyc_Absyn_Format_att_struct
_tmp3BC;_tmp3BC.tag=3;_tmp3BC.f1=(void*)((void*)1);_tmp3BC.f2=_tmp3B6;_tmp3BC.f3=
_tmp3B8;_tmp3BC;});_tmp3BB;});else{Cyc_Parse_err(_tag_arr("unrecognized format type",
sizeof(char),25),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(
_tag_arr("unrecognized attribute",sizeof(char),23),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));}yyval=(void*)({struct Cyc_Attribute_tok_struct*
_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD[0]=({struct Cyc_Attribute_tok_struct
_tmp3BE;_tmp3BE.tag=Cyc_Attribute_tok;_tmp3BE.f1=(void*)a;_tmp3BE;});_tmp3BD;});
break;}}}case 52: _LL1F2: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3BF=
_cycalloc(sizeof(*_tmp3BF));_tmp3BF[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3C0;_tmp3C0.tag=Cyc_TypeSpecifier_tok;_tmp3C0.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3C0;});_tmp3BF;});break;case 53: _LL1F3: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3C2;_tmp3C2.tag=Cyc_TypeSpecifier_tok;
_tmp3C2.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3C2;});
_tmp3C1;});break;case 54: _LL1F4: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3C4;_tmp3C4.tag=Cyc_TypeSpecifier_tok;_tmp3C4.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));
_tmp3C5->v=(void*)Cyc_yyget_Kind_tok(yyvs[yyvsp_offset]);_tmp3C5;}),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3C4;});_tmp3C3;});break;case 55: _LL1F5: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3C7;_tmp3C7.tag=Cyc_TypeSpecifier_tok;
_tmp3C7.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_uchar_t,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3C7;});
_tmp3C6;});break;case 56: _LL1F6: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3C9;_tmp3C9.tag=Cyc_TypeSpecifier_tok;_tmp3C9.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*
_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA[0]=({struct Cyc_Parse_Short_spec_struct
_tmp3CB;_tmp3CB.tag=2;_tmp3CB.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp3CB;});_tmp3CA;}));_tmp3C9;});_tmp3C8;});
break;case 57: _LL1F7: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3CC=
_cycalloc(sizeof(*_tmp3CC));_tmp3CC[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3CD;_tmp3CD.tag=Cyc_TypeSpecifier_tok;_tmp3CD.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_t,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3CD;});_tmp3CC;});break;case 58: _LL1F8: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));
_tmp3CE[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3CF;_tmp3CF.tag=Cyc_TypeSpecifier_tok;
_tmp3CF.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*_tmp3D0=_cycalloc(
sizeof(*_tmp3D0));_tmp3D0[0]=({struct Cyc_Parse_Long_spec_struct _tmp3D1;_tmp3D1.tag=
3;_tmp3D1.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line);_tmp3D1;});_tmp3D0;}));_tmp3CF;});_tmp3CE;});break;
case 59: _LL1F9: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3D2=
_cycalloc(sizeof(*_tmp3D2));_tmp3D2[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3D3;_tmp3D3.tag=Cyc_TypeSpecifier_tok;_tmp3D3.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp3D3;});_tmp3D2;});break;case 60: _LL1FA: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));
_tmp3D4[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3D5;_tmp3D5.tag=Cyc_TypeSpecifier_tok;
_tmp3D5.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3D5;});
_tmp3D4;});break;case 61: _LL1FB: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3D7;_tmp3D7.tag=Cyc_TypeSpecifier_tok;_tmp3D7.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*
_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp3D9;_tmp3D9.tag=0;_tmp3D9.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp3D9;});_tmp3D8;}));_tmp3D7;});_tmp3D6;});
break;case 62: _LL1FC: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3DA=
_cycalloc(sizeof(*_tmp3DA));_tmp3DA[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3DB;_tmp3DB.tag=Cyc_TypeSpecifier_tok;_tmp3DB.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp3DD;_tmp3DD.tag=1;_tmp3DD.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp3DD;});_tmp3DC;}));_tmp3DB;});_tmp3DA;});
break;case 63: _LL1FD: yyval=yyvs[yyvsp_offset];break;case 64: _LL1FE: yyval=yyvs[
yyvsp_offset];break;case 65: _LL1FF: yyval=yyvs[yyvsp_offset];break;case 66: _LL200:
yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3DE=_cycalloc(sizeof(*
_tmp3DE));_tmp3DE[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3DF;_tmp3DF.tag=Cyc_TypeSpecifier_tok;
_tmp3DF.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3DF;});
_tmp3DE;});break;case 67: _LL201: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3E0=_cycalloc(sizeof(*_tmp3E0));_tmp3E0[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3E1;_tmp3E1.tag=Cyc_TypeSpecifier_tok;_tmp3E1.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypedefType_struct*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));
_tmp3E2[0]=({struct Cyc_Absyn_TypedefType_struct _tmp3E3;_tmp3E3.tag=16;_tmp3E3.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3E3.f2=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp3E3.f3=0;_tmp3E3.f4=0;
_tmp3E3;});_tmp3E2;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3E1;});_tmp3E0;});break;case 68: _LL202: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3E5;_tmp3E5.tag=Cyc_TypeSpecifier_tok;
_tmp3E5.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TupleType_struct*
_tmp3E6=_cycalloc(sizeof(*_tmp3E6));_tmp3E6[0]=({struct Cyc_Absyn_TupleType_struct
_tmp3E7;_tmp3E7.tag=9;_tmp3E7.f1=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(
struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp3E7;});_tmp3E6;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3E5;});
_tmp3E4;});break;case 69: _LL203: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3E8=_cycalloc(sizeof(*_tmp3E8));_tmp3E8[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3E9;_tmp3E9.tag=Cyc_TypeSpecifier_tok;_tmp3E9.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));
_tmp3EA[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp3EB;_tmp3EB.tag=15;_tmp3EB.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp3EB;});_tmp3EA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3E9;});_tmp3E8;});break;case 70: _LL204: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));_tmp3EC[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp3ED;_tmp3ED.tag=Cyc_TypeSpecifier_tok;
_tmp3ED.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp3EE=_cycalloc(sizeof(*_tmp3EE));_tmp3EE[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp3EF;_tmp3EF.tag=14;_tmp3EF.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3EF;});_tmp3EE;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp3ED;});
_tmp3EC;});break;case 71: _LL205: {struct _tuple13 _tmp3F1;int _tmp3F2;struct _tuple13*
_tmp3F0=Cyc_yyget_Int_tok(yyvs[yyvsp_offset]);_tmp3F1=*_tmp3F0;_tmp3F2=_tmp3F1.f2;
yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp3F3=_cycalloc(sizeof(*
_tmp3F3));_tmp3F3[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp3F4;_tmp3F4.tag=Cyc_TypeSpecifier_tok;
_tmp3F4.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeInt_struct*
_tmp3F5=_cycalloc_atomic(sizeof(*_tmp3F5));_tmp3F5[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp3F6;_tmp3F6.tag=18;_tmp3F6.f1=_tmp3F2;_tmp3F6;});_tmp3F5;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3F4;});
_tmp3F3;});break;}case 72: _LL206: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp3F7=_cycalloc(sizeof(*_tmp3F7));_tmp3F7[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp3F8;_tmp3F8.tag=Cyc_TypeSpecifier_tok;_tmp3F8.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));
_tmp3F9[0]=({struct Cyc_Absyn_TagType_struct _tmp3FA;_tmp3FA.tag=17;_tmp3FA.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp3FA;});_tmp3F9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp3F8;});_tmp3F7;});break;case 73: _LL207: yyval=(void*)({
struct Cyc_Kind_tok_struct*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB[0]=({
struct Cyc_Kind_tok_struct _tmp3FC;_tmp3FC.tag=Cyc_Kind_tok;_tmp3FC.f1=(void*)Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp3FC;});_tmp3FB;});
break;case 74: _LL208: yyval=(void*)({struct Cyc_TypeQual_tok_struct*_tmp3FD=
_cycalloc_atomic(sizeof(*_tmp3FD));_tmp3FD[0]=({struct Cyc_TypeQual_tok_struct
_tmp3FE;_tmp3FE.tag=Cyc_TypeQual_tok;_tmp3FE.f1=({struct Cyc_Absyn_Tqual _tmp3FF;
_tmp3FF.q_const=1;_tmp3FF.q_volatile=0;_tmp3FF.q_restrict=0;_tmp3FF;});_tmp3FE;});
_tmp3FD;});break;case 75: _LL209: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp400=_cycalloc_atomic(sizeof(*_tmp400));_tmp400[0]=({struct Cyc_TypeQual_tok_struct
_tmp401;_tmp401.tag=Cyc_TypeQual_tok;_tmp401.f1=({struct Cyc_Absyn_Tqual _tmp402;
_tmp402.q_const=0;_tmp402.q_volatile=1;_tmp402.q_restrict=0;_tmp402;});_tmp401;});
_tmp400;});break;case 76: _LL20A: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp403=_cycalloc_atomic(sizeof(*_tmp403));_tmp403[0]=({struct Cyc_TypeQual_tok_struct
_tmp404;_tmp404.tag=Cyc_TypeQual_tok;_tmp404.f1=({struct Cyc_Absyn_Tqual _tmp405;
_tmp405.q_const=0;_tmp405.q_volatile=0;_tmp405.q_restrict=1;_tmp405;});_tmp404;});
_tmp403;});break;case 77: _LL20B: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp407;_tmp407.tag=Cyc_TypeSpecifier_tok;_tmp407.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp409;_tmp409.tag=5;_tmp409.f1=({struct Cyc_Absyn_Decl*_tmp40A=_cycalloc(
sizeof(*_tmp40A));_tmp40A->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp40C;_tmp40C.tag=6;_tmp40C.f1=({struct Cyc_Absyn_Enumdecl*_tmp40D=_cycalloc(
sizeof(*_tmp40D));_tmp40D->sc=(void*)((void*)2);_tmp40D->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp40D->fields=({
struct Cyc_Core_Opt*_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E->v=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp40E;});_tmp40D;});
_tmp40C;});_tmp40B;}));_tmp40A->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp40A;});_tmp409;});_tmp408;}));_tmp407;});_tmp406;});
break;case 78: _LL20C: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp40F=
_cycalloc(sizeof(*_tmp40F));_tmp40F[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp410;_tmp410.tag=Cyc_TypeSpecifier_tok;_tmp410.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp411=_cycalloc(sizeof(*_tmp411));
_tmp411[0]=({struct Cyc_Absyn_EnumType_struct _tmp412;_tmp412.tag=12;_tmp412.f1=
Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);_tmp412.f2=0;_tmp412;});_tmp411;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp410;});_tmp40F;});break;case 79: _LL20D: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp413=_cycalloc(sizeof(*_tmp413));_tmp413[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp414;_tmp414.tag=Cyc_TypeSpecifier_tok;
_tmp414.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp415=_cycalloc(
sizeof(*_tmp415));_tmp415[0]=({struct Cyc_Parse_Type_spec_struct _tmp416;_tmp416.tag=
4;_tmp416.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp417=
_cycalloc(sizeof(*_tmp417));_tmp417[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp418;_tmp418.tag=13;_tmp418.f1=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp418;});_tmp417;}));
_tmp416.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line);_tmp416;});
_tmp415;}));_tmp414;});_tmp413;});break;case 80: _LL20E: yyval=(void*)({struct Cyc_Enumfield_tok_struct*
_tmp419=_cycalloc(sizeof(*_tmp419));_tmp419[0]=({struct Cyc_Enumfield_tok_struct
_tmp41A;_tmp41A.tag=Cyc_Enumfield_tok;_tmp41A.f1=({struct Cyc_Absyn_Enumfield*
_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B->name=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp41B->tag=0;_tmp41B->loc=Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);_tmp41B;});_tmp41A;});
_tmp419;});break;case 81: _LL20F: yyval=(void*)({struct Cyc_Enumfield_tok_struct*
_tmp41C=_cycalloc(sizeof(*_tmp41C));_tmp41C[0]=({struct Cyc_Enumfield_tok_struct
_tmp41D;_tmp41D.tag=Cyc_Enumfield_tok;_tmp41D.f1=({struct Cyc_Absyn_Enumfield*
_tmp41E=_cycalloc(sizeof(*_tmp41E));_tmp41E->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp41E->tag=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp41E->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp41E;});_tmp41D;});_tmp41C;});break;case 82: _LL210:
yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*_tmp41F=_cycalloc(sizeof(*
_tmp41F));_tmp41F[0]=({struct Cyc_EnumfieldList_tok_struct _tmp420;_tmp420.tag=Cyc_EnumfieldList_tok;
_tmp420.f1=({struct Cyc_List_List*_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421->hd=
Cyc_yyget_Enumfield_tok(yyvs[yyvsp_offset]);_tmp421->tl=0;_tmp421;});_tmp420;});
_tmp41F;});break;case 83: _LL211: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp423;_tmp423.tag=Cyc_EnumfieldList_tok;_tmp423.f1=({struct Cyc_List_List*
_tmp424=_cycalloc(sizeof(*_tmp424));_tmp424->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp424->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[yyvsp_offset]);_tmp424;});_tmp423;});_tmp422;});break;case 84: _LL212: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp425=_cycalloc(sizeof(*_tmp425));
_tmp425[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp426;_tmp426.tag=Cyc_TypeSpecifier_tok;
_tmp426.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmp427=_cycalloc(sizeof(*_tmp427));_tmp427[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp428;_tmp428.tag=11;_tmp428.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp428.f2=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp428;});_tmp427;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp426;});
_tmp425;});break;case 85: _LL213: {struct Cyc_List_List*_tmp429=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));struct Cyc_List_List*
_tmp42A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp42C;_tmp42C.tag=Cyc_TypeSpecifier_tok;
_tmp42C.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp42D=_cycalloc(
sizeof(*_tmp42D));_tmp42D[0]=({struct Cyc_Parse_Decl_spec_struct _tmp42E;_tmp42E.tag=
5;_tmp42E.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmp429,({struct Cyc_Core_Opt*
_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F->v=_tmp42A;_tmp42F;}),({struct Cyc_Core_Opt*
_tmp430=_cycalloc(sizeof(*_tmp430));_tmp430->v=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp430;}),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp42E;});_tmp42D;}));_tmp42C;});_tmp42B;});break;}
case 86: _LL214: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp431=
_cycalloc(sizeof(*_tmp431));_tmp431[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp432;_tmp432.tag=Cyc_TypeSpecifier_tok;_tmp432.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp433=_cycalloc(sizeof(*_tmp433));
_tmp433[0]=({struct Cyc_Absyn_AggrType_struct _tmp434;_tmp434.tag=10;_tmp434.f1=({
struct Cyc_Absyn_AggrInfo _tmp435;_tmp435.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp437;_tmp437.tag=0;_tmp437.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp437.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp437;});_tmp436;}));
_tmp435.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp435;});_tmp434;});
_tmp433;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp432;});
_tmp431;});break;case 87: _LL215: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438[0]=({struct Cyc_TypeList_tok_struct
_tmp439;_tmp439.tag=Cyc_TypeList_tok;_tmp439.f1=0;_tmp439;});_tmp438;});break;
case 88: _LL216: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp43A=_cycalloc(
sizeof(*_tmp43A));_tmp43A[0]=({struct Cyc_TypeList_tok_struct _tmp43B;_tmp43B.tag=
Cyc_TypeList_tok;_tmp43B.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp43B;});_tmp43A;});
break;case 89: _LL217: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp43C=
_cycalloc(sizeof(*_tmp43C));_tmp43C[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp43D;_tmp43D.tag=Cyc_StructOrUnion_tok;_tmp43D.f1=(void*)((void*)0);_tmp43D;});
_tmp43C;});break;case 90: _LL218: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*
_tmp43E=_cycalloc(sizeof(*_tmp43E));_tmp43E[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp43F;_tmp43F.tag=Cyc_StructOrUnion_tok;_tmp43F.f1=(void*)((void*)1);_tmp43F;});
_tmp43E;});break;case 91: _LL219: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp441;_tmp441.tag=Cyc_AggrFieldDeclList_tok;_tmp441.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_AggrFieldDeclListList_tok(yyvs[yyvsp_offset])));
_tmp441;});_tmp440;});break;case 92: _LL21A: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp443;_tmp443.tag=Cyc_AggrFieldDeclListList_tok;_tmp443.f1=({struct Cyc_List_List*
_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[yyvsp_offset]);_tmp444->tl=0;_tmp444;});_tmp443;});_tmp442;});break;case 93:
_LL21B: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*_tmp445=
_cycalloc(sizeof(*_tmp445));_tmp445[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp446;_tmp446.tag=Cyc_AggrFieldDeclListList_tok;_tmp446.f1=({struct Cyc_List_List*
_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[yyvsp_offset]);_tmp447->tl=Cyc_yyget_AggrFieldDeclListList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp447;});_tmp446;});
_tmp445;});break;case 94: _LL21C: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448[0]=({struct Cyc_InitDeclList_tok_struct
_tmp449;_tmp449.tag=Cyc_InitDeclList_tok;_tmp449.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
yyvsp_offset]));_tmp449;});_tmp448;});break;case 95: _LL21D: yyval=(void*)({struct
Cyc_InitDeclList_tok_struct*_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A[0]=({
struct Cyc_InitDeclList_tok_struct _tmp44B;_tmp44B.tag=Cyc_InitDeclList_tok;
_tmp44B.f1=({struct Cyc_List_List*_tmp44C=_cycalloc(sizeof(*_tmp44C));_tmp44C->hd=
Cyc_yyget_InitDecl_tok(yyvs[yyvsp_offset]);_tmp44C->tl=0;_tmp44C;});_tmp44B;});
_tmp44A;});break;case 96: _LL21E: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D[0]=({struct Cyc_InitDeclList_tok_struct
_tmp44E;_tmp44E.tag=Cyc_InitDeclList_tok;_tmp44E.f1=({struct Cyc_List_List*
_tmp44F=_cycalloc(sizeof(*_tmp44F));_tmp44F->hd=Cyc_yyget_InitDecl_tok(yyvs[
yyvsp_offset]);_tmp44F->tl=Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp44F;});_tmp44E;});
_tmp44D;});break;case 97: _LL21F: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450[0]=({struct Cyc_InitDecl_tok_struct
_tmp451;_tmp451.tag=Cyc_InitDecl_tok;_tmp451.f1=({struct _tuple12*_tmp452=
_cycalloc(sizeof(*_tmp452));_tmp452->f1=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp452->f2=0;_tmp452;});_tmp451;});_tmp450;});break;case 98:
_LL220: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp453=_cycalloc(sizeof(*
_tmp453));_tmp453[0]=({struct Cyc_InitDecl_tok_struct _tmp454;_tmp454.tag=Cyc_InitDecl_tok;
_tmp454.f1=({struct _tuple12*_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455->f1=Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp455->f2=(struct
Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp455;});_tmp454;});
_tmp453;});break;case 99: _LL221: {struct _tuple15 _tmp457;struct Cyc_Absyn_Tqual
_tmp458;struct Cyc_List_List*_tmp459;struct Cyc_List_List*_tmp45A;struct _tuple15*
_tmp456=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp457=*_tmp456;_tmp458=_tmp457.f1;_tmp459=_tmp457.f2;
_tmp45A=_tmp457.f3;{struct Cyc_List_List*_tmp45C;struct Cyc_List_List*_tmp45D;
struct _tuple0 _tmp45B=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
Cyc_yyget_InitDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp45C=_tmp45B.f1;_tmp45D=_tmp45B.f2;{void*_tmp45E=Cyc_Parse_speclist2typ(
_tmp459,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp45F=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(Cyc_Parse_apply_tmss(
_tmp458,_tmp45E,_tmp45C,_tmp45A),_tmp45D);yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp461;_tmp461.tag=Cyc_AggrFieldDeclList_tok;_tmp461.f1=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmp45F);_tmp461;});_tmp460;});break;}}}case 100:
_LL222: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp462=_cycalloc(
sizeof(*_tmp462));_tmp462[0]=({struct Cyc_QualSpecList_tok_struct _tmp463;_tmp463.tag=
Cyc_QualSpecList_tok;_tmp463.f1=({struct _tuple15*_tmp464=_cycalloc(sizeof(*
_tmp464));_tmp464->f1=Cyc_Absyn_empty_tqual();_tmp464->f2=({struct Cyc_List_List*
_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp465->tl=0;
_tmp465;});_tmp464->f3=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp464;});
_tmp463;});_tmp462;});break;case 101: _LL223: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466[0]=({struct Cyc_QualSpecList_tok_struct
_tmp467;_tmp467.tag=Cyc_QualSpecList_tok;_tmp467.f1=({struct _tuple15*_tmp468=
_cycalloc(sizeof(*_tmp468));_tmp468->f1=(*Cyc_yyget_QualSpecList_tok(yyvs[
yyvsp_offset])).f1;_tmp468->f2=({struct Cyc_List_List*_tmp469=_cycalloc(sizeof(*
_tmp469));_tmp469->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp469->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f2;_tmp469;});_tmp468->f3=Cyc_List_append(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f3);_tmp468;});_tmp467;});_tmp466;});break;case 102: _LL224:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp46A=_cycalloc(sizeof(*
_tmp46A));_tmp46A[0]=({struct Cyc_QualSpecList_tok_struct _tmp46B;_tmp46B.tag=Cyc_QualSpecList_tok;
_tmp46B.f1=({struct _tuple15*_tmp46C=_cycalloc(sizeof(*_tmp46C));_tmp46C->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp46C->f2=0;
_tmp46C->f3=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp46C;});_tmp46B;});
_tmp46A;});break;case 103: _LL225: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D[0]=({struct Cyc_QualSpecList_tok_struct
_tmp46E;_tmp46E.tag=Cyc_QualSpecList_tok;_tmp46E.f1=({struct _tuple15*_tmp46F=
_cycalloc(sizeof(*_tmp46F));_tmp46F->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f1);_tmp46F->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
yyvsp_offset])).f2;_tmp46F->f3=Cyc_List_append(Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[yyvsp_offset])).f3);_tmp46F;});_tmp46E;});_tmp46D;});break;case 104: _LL226:
yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp470=_cycalloc(sizeof(*
_tmp470));_tmp470[0]=({struct Cyc_InitDeclList_tok_struct _tmp471;_tmp471.tag=Cyc_InitDeclList_tok;
_tmp471.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok(yyvs[yyvsp_offset]));_tmp471;});_tmp470;});break;case
105: _LL227: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp472=_cycalloc(
sizeof(*_tmp472));_tmp472[0]=({struct Cyc_InitDeclList_tok_struct _tmp473;_tmp473.tag=
Cyc_InitDeclList_tok;_tmp473.f1=({struct Cyc_List_List*_tmp474=_cycalloc(sizeof(*
_tmp474));_tmp474->hd=Cyc_yyget_InitDecl_tok(yyvs[yyvsp_offset]);_tmp474->tl=0;
_tmp474;});_tmp473;});_tmp472;});break;case 106: _LL228: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475[0]=({struct Cyc_InitDeclList_tok_struct
_tmp476;_tmp476.tag=Cyc_InitDeclList_tok;_tmp476.f1=({struct Cyc_List_List*
_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->hd=Cyc_yyget_InitDecl_tok(yyvs[
yyvsp_offset]);_tmp477->tl=Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp477;});_tmp476;});
_tmp475;});break;case 107: _LL229: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478[0]=({struct Cyc_InitDecl_tok_struct
_tmp479;_tmp479.tag=Cyc_InitDecl_tok;_tmp479.f1=({struct _tuple12*_tmp47A=
_cycalloc(sizeof(*_tmp47A));_tmp47A->f1=Cyc_yyget_Declarator_tok(yyvs[
yyvsp_offset]);_tmp47A->f2=0;_tmp47A;});_tmp479;});_tmp478;});break;case 108:
_LL22A: yyval=(void*)({struct Cyc_InitDecl_tok_struct*_tmp47B=_cycalloc(sizeof(*
_tmp47B));_tmp47B[0]=({struct Cyc_InitDecl_tok_struct _tmp47C;_tmp47C.tag=Cyc_InitDecl_tok;
_tmp47C.f1=({struct _tuple12*_tmp47D=_cycalloc(sizeof(*_tmp47D));_tmp47D->f1=({
struct Cyc_Parse_Declarator*_tmp47E=_cycalloc(sizeof(*_tmp47E));_tmp47E->id=({
struct _tuple1*_tmp47F=_cycalloc(sizeof(*_tmp47F));_tmp47F->f1=Cyc_Absyn_rel_ns_null;
_tmp47F->f2=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",sizeof(char),1);
_tmp47F;});_tmp47E->tms=0;_tmp47E;});_tmp47D->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp47D;});_tmp47C;});_tmp47B;});break;case 109: _LL22B: yyval=(
void*)({struct Cyc_InitDecl_tok_struct*_tmp481=_cycalloc(sizeof(*_tmp481));
_tmp481[0]=({struct Cyc_InitDecl_tok_struct _tmp482;_tmp482.tag=Cyc_InitDecl_tok;
_tmp482.f1=({struct _tuple12*_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483->f1=Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp483->f2=(struct
Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);_tmp483;});_tmp482;});
_tmp481;});break;case 110: _LL22C: {struct Cyc_List_List*_tmp484=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp486;_tmp486.tag=Cyc_TypeSpecifier_tok;
_tmp486.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp487=_cycalloc(
sizeof(*_tmp487));_tmp487[0]=({struct Cyc_Parse_Decl_spec_struct _tmp488;_tmp488.tag=
5;_tmp488.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp484,({struct Cyc_Core_Opt*
_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp489;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp488;});_tmp487;}));_tmp486;});_tmp485;});break;}
case 111: _LL22D: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp48A=
_cycalloc(sizeof(*_tmp48A));_tmp48A[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp48B;_tmp48B.tag=Cyc_TypeSpecifier_tok;_tmp48B.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp48C=_cycalloc(sizeof(*_tmp48C));
_tmp48C[0]=({struct Cyc_Absyn_TunionType_struct _tmp48D;_tmp48D.tag=2;_tmp48D.f1=({
struct Cyc_Absyn_TunionInfo _tmp48E;_tmp48E.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp48F=_cycalloc(sizeof(*_tmp48F));_tmp48F[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp490;_tmp490.tag=0;_tmp490.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp491;
_tmp491.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp491.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp491;});_tmp490;});
_tmp48F;}));_tmp48E.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp48E.rgn=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp48E;});_tmp48D;});_tmp48C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp48B;});_tmp48A;});break;case 112: _LL22E: {void*
_tmp492=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp49B=_cycalloc(sizeof(*
_tmp49B));_tmp49B->v=(void*)((void*)3);_tmp49B;}),0);yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp494;_tmp494.tag=Cyc_TypeSpecifier_tok;_tmp494.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp495=_cycalloc(sizeof(*_tmp495));
_tmp495[0]=({struct Cyc_Absyn_TunionType_struct _tmp496;_tmp496.tag=2;_tmp496.f1=({
struct Cyc_Absyn_TunionInfo _tmp497;_tmp497.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp499;_tmp499.tag=0;_tmp499.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp49A;
_tmp49A.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp49A.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp49A;});_tmp499;});
_tmp498;}));_tmp497.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp497.rgn=(
void*)_tmp492;_tmp497;});_tmp496;});_tmp495;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp494;});_tmp493;});break;}case 113: _LL22F: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp49C=_cycalloc(sizeof(*_tmp49C));
_tmp49C[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp49D;_tmp49D.tag=Cyc_TypeSpecifier_tok;
_tmp49D.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp49F;_tmp49F.tag=3;_tmp49F.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp4A0;
_tmp4A0.field_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*
_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp4A2;_tmp4A2.tag=0;_tmp4A2.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp4A3;
_tmp4A3.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp4A3.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A3.is_xtunion=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp4A3;});_tmp4A2;});
_tmp4A1;}));_tmp4A0.targs=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp4A0;});
_tmp49F;});_tmp49E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp49D;});_tmp49C;});break;case 114: _LL230: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp4A4=_cycalloc_atomic(sizeof(*_tmp4A4));
_tmp4A4[0]=({struct Cyc_Bool_tok_struct _tmp4A5;_tmp4A5.tag=Cyc_Bool_tok;_tmp4A5.f1=
0;_tmp4A5;});_tmp4A4;});break;case 115: _LL231: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp4A6=_cycalloc_atomic(sizeof(*_tmp4A6));_tmp4A6[0]=({struct Cyc_Bool_tok_struct
_tmp4A7;_tmp4A7.tag=Cyc_Bool_tok;_tmp4A7.f1=1;_tmp4A7;});_tmp4A6;});break;case
116: _LL232: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp4A8=_cycalloc(
sizeof(*_tmp4A8));_tmp4A8[0]=({struct Cyc_TunionFieldList_tok_struct _tmp4A9;
_tmp4A9.tag=Cyc_TunionFieldList_tok;_tmp4A9.f1=({struct Cyc_List_List*_tmp4AA=
_cycalloc(sizeof(*_tmp4AA));_tmp4AA->hd=Cyc_yyget_TunionField_tok(yyvs[
yyvsp_offset]);_tmp4AA->tl=0;_tmp4AA;});_tmp4A9;});_tmp4A8;});break;case 117:
_LL233: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp4AB=_cycalloc(
sizeof(*_tmp4AB));_tmp4AB[0]=({struct Cyc_TunionFieldList_tok_struct _tmp4AC;
_tmp4AC.tag=Cyc_TunionFieldList_tok;_tmp4AC.f1=({struct Cyc_List_List*_tmp4AD=
_cycalloc(sizeof(*_tmp4AD));_tmp4AD->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4AD->tl=0;_tmp4AD;});
_tmp4AC;});_tmp4AB;});break;case 118: _LL234: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp4AE=_cycalloc(sizeof(*_tmp4AE));_tmp4AE[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp4AF;_tmp4AF.tag=Cyc_TunionFieldList_tok;_tmp4AF.f1=({struct Cyc_List_List*
_tmp4B0=_cycalloc(sizeof(*_tmp4B0));_tmp4B0->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4B0->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[yyvsp_offset]);_tmp4B0;});_tmp4AF;});_tmp4AE;});break;case 119: _LL235: yyval=(
void*)({struct Cyc_TunionFieldList_tok_struct*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));
_tmp4B1[0]=({struct Cyc_TunionFieldList_tok_struct _tmp4B2;_tmp4B2.tag=Cyc_TunionFieldList_tok;
_tmp4B2.f1=({struct Cyc_List_List*_tmp4B3=_cycalloc(sizeof(*_tmp4B3));_tmp4B3->hd=
Cyc_yyget_TunionField_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]);_tmp4B3->tl=Cyc_yyget_TunionFieldList_tok(yyvs[yyvsp_offset]);_tmp4B3;});
_tmp4B2;});_tmp4B1;});break;case 120: _LL236: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4[0]=({struct Cyc_Scope_tok_struct
_tmp4B5;_tmp4B5.tag=Cyc_Scope_tok;_tmp4B5.f1=(void*)((void*)2);_tmp4B5;});
_tmp4B4;});break;case 121: _LL237: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6[0]=({struct Cyc_Scope_tok_struct
_tmp4B7;_tmp4B7.tag=Cyc_Scope_tok;_tmp4B7.f1=(void*)((void*)3);_tmp4B7;});
_tmp4B6;});break;case 122: _LL238: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp4B8=_cycalloc(sizeof(*_tmp4B8));_tmp4B8[0]=({struct Cyc_Scope_tok_struct
_tmp4B9;_tmp4B9.tag=Cyc_Scope_tok;_tmp4B9.f1=(void*)((void*)0);_tmp4B9;});
_tmp4B8;});break;case 123: _LL239: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp4BA=_cycalloc(sizeof(*_tmp4BA));_tmp4BA[0]=({struct Cyc_TunionField_tok_struct
_tmp4BB;_tmp4BB.tag=Cyc_TunionField_tok;_tmp4BB.f1=({struct Cyc_Absyn_Tunionfield*
_tmp4BC=_cycalloc(sizeof(*_tmp4BC));_tmp4BC->name=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp4BC->typs=0;_tmp4BC->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4BC->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4BC;});_tmp4BB;});
_tmp4BA;});break;case 124: _LL23A: {struct Cyc_List_List*_tmp4BD=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE[0]=({
struct Cyc_TunionField_tok_struct _tmp4BF;_tmp4BF.tag=Cyc_TunionField_tok;_tmp4BF.f1=({
struct Cyc_Absyn_Tunionfield*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp4C0->typs=_tmp4BD;_tmp4C0->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4C0->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp4C0;});_tmp4BF;});
_tmp4BE;});break;}case 125: _LL23B: yyval=yyvs[yyvsp_offset];break;case 126: _LL23C:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));
_tmp4C1[0]=({struct Cyc_Declarator_tok_struct _tmp4C2;_tmp4C2.tag=Cyc_Declarator_tok;
_tmp4C2.f1=({struct Cyc_Parse_Declarator*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));
_tmp4C3->id=(Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]))->id;_tmp4C3->tms=Cyc_List_imp_append(
Cyc_yyget_TypeModifierList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]))->tms);_tmp4C3;});
_tmp4C2;});_tmp4C1;});break;case 127: _LL23D: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4C4=_cycalloc(sizeof(*_tmp4C4));_tmp4C4[0]=({struct Cyc_Declarator_tok_struct
_tmp4C5;_tmp4C5.tag=Cyc_Declarator_tok;_tmp4C5.f1=({struct Cyc_Parse_Declarator*
_tmp4C6=_cycalloc(sizeof(*_tmp4C6));_tmp4C6->id=Cyc_yyget_QualId_tok(yyvs[
yyvsp_offset]);_tmp4C6->tms=0;_tmp4C6;});_tmp4C5;});_tmp4C4;});break;case 128:
_LL23E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 129: _LL23F: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4C7=_cycalloc(
sizeof(*_tmp4C7));_tmp4C7[0]=({struct Cyc_Declarator_tok_struct _tmp4C8;_tmp4C8.tag=
Cyc_Declarator_tok;_tmp4C8.f1=({struct Cyc_Parse_Declarator*_tmp4C9=_cycalloc(
sizeof(*_tmp4C9));_tmp4C9->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->id;_tmp4C9->tms=({
struct Cyc_List_List*_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA->hd=(void*)((
void*)0);_tmp4CA->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->tms;_tmp4CA;});_tmp4C9;});
_tmp4C8;});_tmp4C7;});break;case 130: _LL240: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB[0]=({struct Cyc_Declarator_tok_struct
_tmp4CC;_tmp4CC.tag=Cyc_Declarator_tok;_tmp4CC.f1=({struct Cyc_Parse_Declarator*
_tmp4CD=_cycalloc(sizeof(*_tmp4CD));_tmp4CD->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4CD->tms=({
struct Cyc_List_List*_tmp4CE=_cycalloc(sizeof(*_tmp4CE));_tmp4CE->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp4CF=_cycalloc(sizeof(*_tmp4CF));
_tmp4CF[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp4D0;_tmp4D0.tag=0;_tmp4D0.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4D0;});_tmp4CF;}));_tmp4CE->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp4CE;});_tmp4CD;});
_tmp4CC;});_tmp4CB;});break;case 131: _LL241: {struct _tuple14 _tmp4D2;struct Cyc_List_List*
_tmp4D3;int _tmp4D4;struct Cyc_Absyn_VarargInfo*_tmp4D5;struct Cyc_Core_Opt*_tmp4D6;
struct Cyc_List_List*_tmp4D7;struct _tuple14*_tmp4D1=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4D2=*_tmp4D1;_tmp4D3=
_tmp4D2.f1;_tmp4D4=_tmp4D2.f2;_tmp4D5=_tmp4D2.f3;_tmp4D6=_tmp4D2.f4;_tmp4D7=
_tmp4D2.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4D8=_cycalloc(
sizeof(*_tmp4D8));_tmp4D8[0]=({struct Cyc_Declarator_tok_struct _tmp4D9;_tmp4D9.tag=
Cyc_Declarator_tok;_tmp4D9.f1=({struct Cyc_Parse_Declarator*_tmp4DA=_cycalloc(
sizeof(*_tmp4DA));_tmp4DA->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4DA->tms=({
struct Cyc_List_List*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));
_tmp4DC[0]=({struct Cyc_Absyn_Function_mod_struct _tmp4DD;_tmp4DD.tag=2;_tmp4DD.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp4DE=_cycalloc(sizeof(*
_tmp4DE));_tmp4DE[0]=({struct Cyc_Absyn_WithTypes_struct _tmp4DF;_tmp4DF.tag=1;
_tmp4DF.f1=_tmp4D3;_tmp4DF.f2=_tmp4D4;_tmp4DF.f3=_tmp4D5;_tmp4DF.f4=_tmp4D6;
_tmp4DF.f5=_tmp4D7;_tmp4DF;});_tmp4DE;}));_tmp4DD;});_tmp4DC;}));_tmp4DB->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp4DB;});_tmp4DA;});_tmp4D9;});_tmp4D8;});break;}case 132: _LL242:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));
_tmp4E0[0]=({struct Cyc_Declarator_tok_struct _tmp4E1;_tmp4E1.tag=Cyc_Declarator_tok;
_tmp4E1.f1=({struct Cyc_Parse_Declarator*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));
_tmp4E2->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp4E2->tms=({struct Cyc_List_List*_tmp4E3=_cycalloc(
sizeof(*_tmp4E3));_tmp4E3->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp4E5;_tmp4E5.tag=2;_tmp4E5.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp4E6=_cycalloc(sizeof(*_tmp4E6));_tmp4E6[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp4E7;_tmp4E7.tag=1;_tmp4E7.f1=0;_tmp4E7.f2=0;_tmp4E7.f3=0;_tmp4E7.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4E7.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E7;});_tmp4E6;}));
_tmp4E5;});_tmp4E4;}));_tmp4E3->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp4E3;});_tmp4E2;});
_tmp4E1;});_tmp4E0;});break;case 133: _LL243: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4E8=_cycalloc(sizeof(*_tmp4E8));_tmp4E8[0]=({struct Cyc_Declarator_tok_struct
_tmp4E9;_tmp4E9.tag=Cyc_Declarator_tok;_tmp4E9.f1=({struct Cyc_Parse_Declarator*
_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4EA->tms=({
struct Cyc_List_List*_tmp4EB=_cycalloc(sizeof(*_tmp4EB));_tmp4EB->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4EC=_cycalloc(sizeof(*_tmp4EC));
_tmp4EC[0]=({struct Cyc_Absyn_Function_mod_struct _tmp4ED;_tmp4ED.tag=2;_tmp4ED.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp4EE=_cycalloc(sizeof(*_tmp4EE));
_tmp4EE[0]=({struct Cyc_Absyn_NoTypes_struct _tmp4EF;_tmp4EF.tag=0;_tmp4EF.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4EF.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4EF;});_tmp4EE;}));_tmp4ED;});_tmp4EC;}));_tmp4EB->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp4EB;});_tmp4EA;});_tmp4E9;});_tmp4E8;});break;case 134: _LL244: {
struct Cyc_List_List*_tmp4F0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1[0]=({
struct Cyc_Declarator_tok_struct _tmp4F2;_tmp4F2.tag=Cyc_Declarator_tok;_tmp4F2.f1=({
struct Cyc_Parse_Declarator*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp4F3->tms=({
struct Cyc_List_List*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));
_tmp4F5[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp4F6;_tmp4F6.tag=3;_tmp4F6.f1=
_tmp4F0;_tmp4F6.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp4F6.f3=0;_tmp4F6;});_tmp4F5;}));_tmp4F4->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp4F4;});
_tmp4F3;});_tmp4F2;});_tmp4F1;});break;}case 135: _LL245: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7[0]=({struct Cyc_Declarator_tok_struct
_tmp4F8;_tmp4F8.tag=Cyc_Declarator_tok;_tmp4F8.f1=({struct Cyc_Parse_Declarator*
_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp4F9->tms=({
struct Cyc_List_List*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp4FB=_cycalloc(sizeof(*_tmp4FB));
_tmp4FB[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp4FC;_tmp4FC.tag=4;_tmp4FC.f1=
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line);
_tmp4FC.f2=Cyc_yyget_AttributeList_tok(yyvs[yyvsp_offset]);_tmp4FC;});_tmp4FB;}));
_tmp4FA->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp4FA;});_tmp4F9;});_tmp4F8;});_tmp4F7;});break;case
136: _LL246: Cyc_Parse_err(_tag_arr("identifier has not been declared as a typedef",
sizeof(char),46),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(void*)({
struct Cyc_Declarator_tok_struct*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD[0]=({
struct Cyc_Declarator_tok_struct _tmp4FE;_tmp4FE.tag=Cyc_Declarator_tok;_tmp4FE.f1=({
struct Cyc_Parse_Declarator*_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF->id=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp4FF->tms=0;_tmp4FF;});_tmp4FE;});_tmp4FD;});break;case
137: _LL247: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp500=_cycalloc(
sizeof(*_tmp500));_tmp500[0]=({struct Cyc_Declarator_tok_struct _tmp501;_tmp501.tag=
Cyc_Declarator_tok;_tmp501.f1=({struct Cyc_Parse_Declarator*_tmp502=_cycalloc(
sizeof(*_tmp502));_tmp502->id=Cyc_Absyn_exn_name;_tmp502->tms=0;_tmp502;});
_tmp501;});_tmp500;});break;case 138: _LL248: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp503=_cycalloc(sizeof(*_tmp503));_tmp503[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp504;_tmp504.tag=Cyc_TypeModifierList_tok;_tmp504.f1=({struct Cyc_List_List*
_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp507;_tmp507.tag=1;_tmp507.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp507.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp507.f3=Cyc_Absyn_empty_tqual();
_tmp507;});_tmp506;}));_tmp505->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[yyvsp_offset]),0);_tmp505;});_tmp504;});_tmp503;});break;case 139: _LL249:
yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*_tmp508=_cycalloc(sizeof(*
_tmp508));_tmp508[0]=({struct Cyc_TypeModifierList_tok_struct _tmp509;_tmp509.tag=
Cyc_TypeModifierList_tok;_tmp509.f1=({struct Cyc_List_List*_tmp50A=_cycalloc(
sizeof(*_tmp50A));_tmp50A->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp50C;_tmp50C.tag=1;_tmp50C.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp50C.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp50C.f3=Cyc_yyget_TypeQual_tok(
yyvs[yyvsp_offset]);_tmp50C;});_tmp50B;}));_tmp50A->tl=Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0);_tmp50A;});_tmp509;});
_tmp508;});break;case 140: _LL24A: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp50E;_tmp50E.tag=Cyc_TypeModifierList_tok;_tmp50E.f1=({struct Cyc_List_List*
_tmp50F=_cycalloc(sizeof(*_tmp50F));_tmp50F->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp511;_tmp511.tag=1;_tmp511.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp511.f2=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp511.f3=Cyc_Absyn_empty_tqual();
_tmp511;});_tmp510;}));_tmp50F->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp50F;});_tmp50E;});_tmp50D;});break;case 141: _LL24B: yyval=(
void*)({struct Cyc_TypeModifierList_tok_struct*_tmp512=_cycalloc(sizeof(*_tmp512));
_tmp512[0]=({struct Cyc_TypeModifierList_tok_struct _tmp513;_tmp513.tag=Cyc_TypeModifierList_tok;
_tmp513.f1=({struct Cyc_List_List*_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514->hd=(
void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp515=_cycalloc(sizeof(*
_tmp515));_tmp515[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp516;_tmp516.tag=1;
_tmp516.f1=(void*)Cyc_yyget_Pointer_Sort_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp516.f2=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp516.f3=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp516;});_tmp515;}));
_tmp514->tl=Cyc_Parse_attopt_to_tms(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]));_tmp514;});_tmp513;});_tmp512;});break;case 142: _LL24C: yyval=(
void*)({struct Cyc_Pointer_Sort_tok_struct*_tmp517=_cycalloc(sizeof(*_tmp517));
_tmp517[0]=({struct Cyc_Pointer_Sort_tok_struct _tmp518;_tmp518.tag=Cyc_Pointer_Sort_tok;
_tmp518.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*_tmp519=_cycalloc(
sizeof(*_tmp519));_tmp519[0]=({struct Cyc_Absyn_Nullable_ps_struct _tmp51A;_tmp51A.tag=
1;_tmp51A.f1=Cyc_Absyn_exp_unsigned_one;_tmp51A;});_tmp519;}));_tmp518;});
_tmp517;});break;case 143: _LL24D: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp51B=_cycalloc(sizeof(*_tmp51B));_tmp51B[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp51C;_tmp51C.tag=Cyc_Pointer_Sort_tok;_tmp51C.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp51E;_tmp51E.tag=0;_tmp51E.f1=Cyc_Absyn_exp_unsigned_one;_tmp51E;});_tmp51D;}));
_tmp51C;});_tmp51B;});break;case 144: _LL24E: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp51F=_cycalloc(sizeof(*_tmp51F));_tmp51F[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp520;_tmp520.tag=Cyc_Pointer_Sort_tok;_tmp520.f1=(void*)((void*)({struct Cyc_Absyn_Nullable_ps_struct*
_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp522;_tmp522.tag=1;_tmp522.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp522;});_tmp521;}));
_tmp520;});_tmp51F;});break;case 145: _LL24F: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp523=_cycalloc(sizeof(*_tmp523));_tmp523[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp524;_tmp524.tag=Cyc_Pointer_Sort_tok;_tmp524.f1=(void*)((void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp525=_cycalloc(sizeof(*_tmp525));_tmp525[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp526;_tmp526.tag=0;_tmp526.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp526;});_tmp525;}));
_tmp524;});_tmp523;});break;case 146: _LL250: yyval=(void*)({struct Cyc_Pointer_Sort_tok_struct*
_tmp527=_cycalloc(sizeof(*_tmp527));_tmp527[0]=({struct Cyc_Pointer_Sort_tok_struct
_tmp528;_tmp528.tag=Cyc_Pointer_Sort_tok;_tmp528.f1=(void*)((void*)0);_tmp528;});
_tmp527;});break;case 147: _LL251: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp529=
_cycalloc(sizeof(*_tmp529));_tmp529[0]=({struct Cyc_Type_tok_struct _tmp52A;
_tmp52A.tag=Cyc_Type_tok;_tmp52A.f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp52B=_cycalloc(sizeof(*_tmp52B));_tmp52B->v=(void*)((void*)3);_tmp52B;}),0);
_tmp52A;});_tmp529;});break;case 148: _LL252: yyval=yyvs[yyvsp_offset];break;case
149: _LL253: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(
void*)3);yyval=yyvs[yyvsp_offset];break;case 150: _LL254: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C[0]=({struct Cyc_Type_tok_struct
_tmp52D;_tmp52D.tag=Cyc_Type_tok;_tmp52D.f1=(void*)Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->v=(void*)((void*)3);
_tmp52E;}),0);_tmp52D;});_tmp52C;});break;case 151: _LL255: yyval=yyvs[yyvsp_offset];
break;case 152: _LL256: yyval=(void*)({struct Cyc_TypeQual_tok_struct*_tmp52F=
_cycalloc_atomic(sizeof(*_tmp52F));_tmp52F[0]=({struct Cyc_TypeQual_tok_struct
_tmp530;_tmp530.tag=Cyc_TypeQual_tok;_tmp530.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[yyvsp_offset]));_tmp530;});_tmp52F;});break;case 153: _LL257: yyval=(void*)({
struct Cyc_YY1_struct*_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531[0]=({struct Cyc_YY1_struct
_tmp532;_tmp532.tag=Cyc_YY1;_tmp532.f1=({struct _tuple14*_tmp533=_cycalloc(
sizeof(*_tmp533));_tmp533->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp533->f2=0;_tmp533->f3=
0;_tmp533->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp533->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp533;});_tmp532;});_tmp531;});break;case 154: _LL258: yyval=(void*)({struct Cyc_YY1_struct*
_tmp534=_cycalloc(sizeof(*_tmp534));_tmp534[0]=({struct Cyc_YY1_struct _tmp535;
_tmp535.tag=Cyc_YY1;_tmp535.f1=({struct _tuple14*_tmp536=_cycalloc(sizeof(*
_tmp536));_tmp536->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]));_tmp536->f2=1;_tmp536->f3=0;_tmp536->f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp536->f5=Cyc_yyget_Rgnorder_tok(
yyvs[yyvsp_offset]);_tmp536;});_tmp535;});_tmp534;});break;case 155: _LL259: {
struct _tuple2 _tmp538;struct Cyc_Core_Opt*_tmp539;struct Cyc_Absyn_Tqual _tmp53A;
void*_tmp53B;struct _tuple2*_tmp537=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp538=*_tmp537;_tmp539=
_tmp538.f1;_tmp53A=_tmp538.f2;_tmp53B=_tmp538.f3;{struct Cyc_Absyn_VarargInfo*
_tmp53C=({struct Cyc_Absyn_VarargInfo*_tmp540=_cycalloc(sizeof(*_tmp540));_tmp540->name=
_tmp539;_tmp540->tq=_tmp53A;_tmp540->type=(void*)_tmp53B;_tmp540->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp540;});yyval=(
void*)({struct Cyc_YY1_struct*_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D[0]=({
struct Cyc_YY1_struct _tmp53E;_tmp53E.tag=Cyc_YY1;_tmp53E.f1=({struct _tuple14*
_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F->f1=0;_tmp53F->f2=0;_tmp53F->f3=
_tmp53C;_tmp53F->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp53F->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp53F;});_tmp53E;});_tmp53D;});break;}}case 156: _LL25A: {struct _tuple2 _tmp542;
struct Cyc_Core_Opt*_tmp543;struct Cyc_Absyn_Tqual _tmp544;void*_tmp545;struct
_tuple2*_tmp541=Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp542=*_tmp541;_tmp543=_tmp542.f1;_tmp544=_tmp542.f2;
_tmp545=_tmp542.f3;{struct Cyc_Absyn_VarargInfo*_tmp546=({struct Cyc_Absyn_VarargInfo*
_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->name=_tmp543;_tmp54A->tq=_tmp544;
_tmp54A->type=(void*)_tmp545;_tmp54A->inject=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp54A;});yyval=(void*)({
struct Cyc_YY1_struct*_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547[0]=({struct Cyc_YY1_struct
_tmp548;_tmp548.tag=Cyc_YY1;_tmp548.f1=({struct _tuple14*_tmp549=_cycalloc(
sizeof(*_tmp549));_tmp549->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp549->f2=0;_tmp549->f3=
_tmp546;_tmp549->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp549->f5=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);
_tmp549;});_tmp548;});_tmp547;});break;}}case 157: _LL25B: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B[0]=({struct Cyc_Type_tok_struct
_tmp54C;_tmp54C.tag=Cyc_Type_tok;_tmp54C.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp54D=_cycalloc(
sizeof(*_tmp54D));_tmp54D[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp54E;_tmp54E.tag=
1;_tmp54E.f1=0;_tmp54E;});_tmp54D;}));_tmp54C;});_tmp54B;});break;case 158: _LL25C:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp54F=_cycalloc(sizeof(*_tmp54F));
_tmp54F[0]=({struct Cyc_Type_tok_struct _tmp550;_tmp550.tag=Cyc_Type_tok;_tmp550.f1=(
void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp551=_cycalloc(
sizeof(*_tmp551));_tmp551[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp552;_tmp552.tag=0;
_tmp552.f1=(void*)Cyc_yyget_Kind_tok(yyvs[yyvsp_offset]);_tmp552;});_tmp551;}));
_tmp550;});_tmp54F;});break;case 159: _LL25D: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553[0]=({struct Cyc_TypeOpt_tok_struct
_tmp554;_tmp554.tag=Cyc_TypeOpt_tok;_tmp554.f1=0;_tmp554;});_tmp553;});break;
case 160: _LL25E: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp555=_cycalloc(
sizeof(*_tmp555));_tmp555[0]=({struct Cyc_TypeOpt_tok_struct _tmp556;_tmp556.tag=
Cyc_TypeOpt_tok;_tmp556.f1=({struct Cyc_Core_Opt*_tmp557=_cycalloc(sizeof(*
_tmp557));_tmp557->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp558=
_cycalloc(sizeof(*_tmp558));_tmp558[0]=({struct Cyc_Absyn_JoinEff_struct _tmp559;
_tmp559.tag=20;_tmp559.f1=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp559;});
_tmp558;}));_tmp557;});_tmp556;});_tmp555;});break;case 161: _LL25F: yyval=(void*)({
struct Cyc_Rgnorder_tok_struct*_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A[0]=({
struct Cyc_Rgnorder_tok_struct _tmp55B;_tmp55B.tag=Cyc_Rgnorder_tok;_tmp55B.f1=0;
_tmp55B;});_tmp55A;});break;case 162: _LL260: yyval=yyvs[yyvsp_offset];break;case
163: _LL261: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp55C=_cycalloc(
sizeof(*_tmp55C));_tmp55C[0]=({struct Cyc_Rgnorder_tok_struct _tmp55D;_tmp55D.tag=
Cyc_Rgnorder_tok;_tmp55D.f1=({struct Cyc_List_List*_tmp55E=_cycalloc(sizeof(*
_tmp55E));_tmp55E->hd=({struct _tuple4*_tmp55F=_cycalloc(sizeof(*_tmp55F));
_tmp55F->f1=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp563;_tmp563.tag=0;_tmp563.f1=(void*)((void*)3);_tmp563;});_tmp562;}));
_tmp55F->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),(void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp561;_tmp561.tag=0;_tmp561.f1=(void*)((void*)3);
_tmp561;});_tmp560;}));_tmp55F;});_tmp55E->tl=0;_tmp55E;});_tmp55D;});_tmp55C;});
break;case 164: _LL262: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp564=
_cycalloc(sizeof(*_tmp564));_tmp564[0]=({struct Cyc_Rgnorder_tok_struct _tmp565;
_tmp565.tag=Cyc_Rgnorder_tok;_tmp565.f1=({struct Cyc_List_List*_tmp566=_cycalloc(
sizeof(*_tmp566));_tmp566->hd=({struct _tuple4*_tmp567=_cycalloc(sizeof(*_tmp567));
_tmp567->f1=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp56B;_tmp56B.tag=0;_tmp56B.f1=(void*)((void*)3);_tmp56B;});_tmp56A;}));
_tmp567->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp569;_tmp569.tag=0;_tmp569.f1=(void*)((void*)3);_tmp569;});_tmp568;}));
_tmp567;});_tmp566->tl=Cyc_yyget_Rgnorder_tok(yyvs[yyvsp_offset]);_tmp566;});
_tmp565;});_tmp564;});break;case 165: _LL263: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp56C=_cycalloc_atomic(sizeof(*_tmp56C));_tmp56C[0]=({struct Cyc_Bool_tok_struct
_tmp56D;_tmp56D.tag=Cyc_Bool_tok;_tmp56D.f1=0;_tmp56D;});_tmp56C;});break;case
166: _LL264: if(Cyc_Std_zstrcmp(Cyc_yyget_String_tok(yyvs[yyvsp_offset]),_tag_arr("inject",
sizeof(char),7))!= 0)Cyc_Parse_err(_tag_arr("missing type in function declaration",
sizeof(char),37),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Bool_tok_struct*_tmp56E=
_cycalloc_atomic(sizeof(*_tmp56E));_tmp56E[0]=({struct Cyc_Bool_tok_struct _tmp56F;
_tmp56F.tag=Cyc_Bool_tok;_tmp56F.f1=1;_tmp56F;});_tmp56E;});break;case 167: _LL265:
yyval=yyvs[yyvsp_offset];break;case 168: _LL266: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570[0]=({struct Cyc_TypeList_tok_struct
_tmp571;_tmp571.tag=Cyc_TypeList_tok;_tmp571.f1=Cyc_List_imp_append(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeList_tok(
yyvs[yyvsp_offset]));_tmp571;});_tmp570;});break;case 169: _LL267: yyval=(void*)({
struct Cyc_TypeList_tok_struct*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572[0]=({
struct Cyc_TypeList_tok_struct _tmp573;_tmp573.tag=Cyc_TypeList_tok;_tmp573.f1=0;
_tmp573;});_tmp572;});break;case 170: _LL268: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 171: _LL269:
yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp574=_cycalloc(sizeof(*_tmp574));
_tmp574[0]=({struct Cyc_TypeList_tok_struct _tmp575;_tmp575.tag=Cyc_TypeList_tok;
_tmp575.f1=({struct Cyc_List_List*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp577=_cycalloc(sizeof(*_tmp577));
_tmp577[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp578;_tmp578.tag=21;_tmp578.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp578;});_tmp577;}));_tmp576->tl=0;_tmp576;});_tmp575;});_tmp574;});
break;case 172: _LL26A: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
yyvsp_offset]),(void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp579=
_cycalloc(sizeof(*_tmp579));_tmp579[0]=({struct Cyc_TypeList_tok_struct _tmp57A;
_tmp57A.tag=Cyc_TypeList_tok;_tmp57A.f1=({struct Cyc_List_List*_tmp57B=_cycalloc(
sizeof(*_tmp57B));_tmp57B->hd=(void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);
_tmp57B->tl=0;_tmp57B;});_tmp57A;});_tmp579;});break;case 173: _LL26B: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[yyvsp_offset]),(void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp57C=_cycalloc(sizeof(*_tmp57C));_tmp57C[0]=({struct Cyc_TypeList_tok_struct
_tmp57D;_tmp57D.tag=Cyc_TypeList_tok;_tmp57D.f1=({struct Cyc_List_List*_tmp57E=
_cycalloc(sizeof(*_tmp57E));_tmp57E->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp580;_tmp580.tag=19;_tmp580.f1=(void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);
_tmp580;});_tmp57F;}));_tmp57E->tl=0;_tmp57E;});_tmp57D;});_tmp57C;});break;case
174: _LL26C: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(void*)({
struct Cyc_TypeList_tok_struct*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581[0]=({
struct Cyc_TypeList_tok_struct _tmp582;_tmp582.tag=Cyc_TypeList_tok;_tmp582.f1=({
struct Cyc_List_List*_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp584=_cycalloc(sizeof(*_tmp584));
_tmp584[0]=({struct Cyc_Absyn_AccessEff_struct _tmp585;_tmp585.tag=19;_tmp585.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp585;});_tmp584;}));_tmp583->tl=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);
_tmp583;});_tmp582;});_tmp581;});break;case 175: _LL26D: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp587;_tmp587.tag=Cyc_ParamDeclList_tok;_tmp587.f1=({struct Cyc_List_List*
_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588->hd=Cyc_yyget_ParamDecl_tok(yyvs[
yyvsp_offset]);_tmp588->tl=0;_tmp588;});_tmp587;});_tmp586;});break;case 176:
_LL26E: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*_tmp589=_cycalloc(
sizeof(*_tmp589));_tmp589[0]=({struct Cyc_ParamDeclList_tok_struct _tmp58A;_tmp58A.tag=
Cyc_ParamDeclList_tok;_tmp58A.f1=({struct Cyc_List_List*_tmp58B=_cycalloc(sizeof(*
_tmp58B));_tmp58B->hd=Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset]);_tmp58B->tl=
Cyc_yyget_ParamDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp58B;});_tmp58A;});_tmp589;});break;case 177: _LL26F: {
struct _tuple15 _tmp58D;struct Cyc_Absyn_Tqual _tmp58E;struct Cyc_List_List*_tmp58F;
struct Cyc_List_List*_tmp590;struct _tuple15*_tmp58C=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp58D=*_tmp58C;
_tmp58E=_tmp58D.f1;_tmp58F=_tmp58D.f2;_tmp590=_tmp58D.f3;{struct Cyc_Parse_Declarator
_tmp592;struct _tuple1*_tmp593;struct Cyc_List_List*_tmp594;struct Cyc_Parse_Declarator*
_tmp591=Cyc_yyget_Declarator_tok(yyvs[yyvsp_offset]);_tmp592=*_tmp591;_tmp593=
_tmp592.id;_tmp594=_tmp592.tms;{void*_tmp595=Cyc_Parse_speclist2typ(_tmp58F,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual
_tmp597;void*_tmp598;struct Cyc_List_List*_tmp599;struct Cyc_List_List*_tmp59A;
struct _tuple6 _tmp596=Cyc_Parse_apply_tms(_tmp58E,_tmp595,_tmp590,_tmp594);
_tmp597=_tmp596.f1;_tmp598=_tmp596.f2;_tmp599=_tmp596.f3;_tmp59A=_tmp596.f4;if(
_tmp599 != 0)Cyc_Parse_err(_tag_arr("parameter with bad type params",sizeof(char),
31),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));if(Cyc_Absyn_is_qvar_qualified(_tmp593))Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a namespace",sizeof(char),47),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp59B=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp5A0=_cycalloc(sizeof(*
_tmp5A0));_tmp5A0->v=(*_tmp593).f2;_tmp5A0;});if(_tmp59A != 0)({void*_tmp59C[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("extra attributes on parameter, ignoring",
sizeof(char),40),_tag_arr(_tmp59C,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D[0]=({struct Cyc_ParamDecl_tok_struct
_tmp59E;_tmp59E.tag=Cyc_ParamDecl_tok;_tmp59E.f1=({struct _tuple2*_tmp59F=
_cycalloc(sizeof(*_tmp59F));_tmp59F->f1=_tmp59B;_tmp59F->f2=_tmp597;_tmp59F->f3=
_tmp598;_tmp59F;});_tmp59E;});_tmp59D;});break;}}}}case 178: _LL270: {struct
_tuple15 _tmp5A2;struct Cyc_Absyn_Tqual _tmp5A3;struct Cyc_List_List*_tmp5A4;struct
Cyc_List_List*_tmp5A5;struct _tuple15*_tmp5A1=Cyc_yyget_QualSpecList_tok(yyvs[
yyvsp_offset]);_tmp5A2=*_tmp5A1;_tmp5A3=_tmp5A2.f1;_tmp5A4=_tmp5A2.f2;_tmp5A5=
_tmp5A2.f3;{void*_tmp5A6=Cyc_Parse_speclist2typ(_tmp5A4,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));if(_tmp5A5 != 0)({
void*_tmp5A7[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(char),38),_tag_arr(_tmp5A7,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8[0]=({struct Cyc_ParamDecl_tok_struct
_tmp5A9;_tmp5A9.tag=Cyc_ParamDecl_tok;_tmp5A9.f1=({struct _tuple2*_tmp5AA=
_cycalloc(sizeof(*_tmp5AA));_tmp5AA->f1=0;_tmp5AA->f2=_tmp5A3;_tmp5AA->f3=
_tmp5A6;_tmp5AA;});_tmp5A9;});_tmp5A8;});break;}}case 179: _LL271: {struct _tuple15
_tmp5AC;struct Cyc_Absyn_Tqual _tmp5AD;struct Cyc_List_List*_tmp5AE;struct Cyc_List_List*
_tmp5AF;struct _tuple15*_tmp5AB=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5AC=*_tmp5AB;_tmp5AD=
_tmp5AC.f1;_tmp5AE=_tmp5AC.f2;_tmp5AF=_tmp5AC.f3;{void*_tmp5B0=Cyc_Parse_speclist2typ(
_tmp5AE,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp5B1=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[yyvsp_offset]))->tms;struct Cyc_Absyn_Tqual _tmp5B3;void*_tmp5B4;struct Cyc_List_List*
_tmp5B5;struct Cyc_List_List*_tmp5B6;struct _tuple6 _tmp5B2=Cyc_Parse_apply_tms(
_tmp5AD,_tmp5B0,_tmp5AF,_tmp5B1);_tmp5B3=_tmp5B2.f1;_tmp5B4=_tmp5B2.f2;_tmp5B5=
_tmp5B2.f3;_tmp5B6=_tmp5B2.f4;if(_tmp5B5 != 0)({void*_tmp5B7[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad type parameters on formal argument, ignoring",
sizeof(char),49),_tag_arr(_tmp5B7,sizeof(void*),0));});if(_tmp5B6 != 0)({void*
_tmp5B8[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line),_tag_arr("bad attributes on parameter, ignoring",
sizeof(char),38),_tag_arr(_tmp5B8,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9[0]=({struct Cyc_ParamDecl_tok_struct
_tmp5BA;_tmp5BA.tag=Cyc_ParamDecl_tok;_tmp5BA.f1=({struct _tuple2*_tmp5BB=
_cycalloc(sizeof(*_tmp5BB));_tmp5BB->f1=0;_tmp5BB->f2=_tmp5B3;_tmp5BB->f3=
_tmp5B4;_tmp5BB;});_tmp5BA;});_tmp5B9;});break;}}case 180: _LL272: yyval=(void*)({
struct Cyc_IdList_tok_struct*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC[0]=({
struct Cyc_IdList_tok_struct _tmp5BD;_tmp5BD.tag=Cyc_IdList_tok;_tmp5BD.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_IdList_tok(
yyvs[yyvsp_offset]));_tmp5BD;});_tmp5BC;});break;case 181: _LL273: yyval=(void*)({
struct Cyc_IdList_tok_struct*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE[0]=({
struct Cyc_IdList_tok_struct _tmp5BF;_tmp5BF.tag=Cyc_IdList_tok;_tmp5BF.f1=({
struct Cyc_List_List*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0->hd=({struct
_tagged_arr*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp5C1;});_tmp5C0->tl=0;_tmp5C0;});_tmp5BF;});_tmp5BE;});
break;case 182: _LL274: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp5C2=
_cycalloc(sizeof(*_tmp5C2));_tmp5C2[0]=({struct Cyc_IdList_tok_struct _tmp5C3;
_tmp5C3.tag=Cyc_IdList_tok;_tmp5C3.f1=({struct Cyc_List_List*_tmp5C4=_cycalloc(
sizeof(*_tmp5C4));_tmp5C4->hd=({struct _tagged_arr*_tmp5C5=_cycalloc(sizeof(*
_tmp5C5));_tmp5C5[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp5C5;});_tmp5C4->tl=
Cyc_yyget_IdList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5C4;});_tmp5C3;});_tmp5C2;});break;case 183: _LL275: yyval=yyvs[yyvsp_offset];
break;case 184: _LL276: yyval=yyvs[yyvsp_offset];break;case 185: _LL277: yyval=(void*)({
struct Cyc_Exp_tok_struct*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));_tmp5C6[0]=({struct
Cyc_Exp_tok_struct _tmp5C7;_tmp5C7.tag=Cyc_Exp_tok;_tmp5C7.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5C8=_cycalloc(sizeof(*_tmp5C8));
_tmp5C8[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5C9;_tmp5C9.tag=34;
_tmp5C9.f1=0;_tmp5C9.f2=0;_tmp5C9;});_tmp5C8;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5C7;});_tmp5C6;});break;case 186: _LL278: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));_tmp5CA[0]=({
struct Cyc_Exp_tok_struct _tmp5CB;_tmp5CB.tag=Cyc_Exp_tok;_tmp5CB.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));
_tmp5CC[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5CD;_tmp5CD.tag=34;
_tmp5CD.f1=0;_tmp5CD.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp5CD;});_tmp5CC;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5CB;});_tmp5CA;});break;case 187: _LL279: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE[0]=({
struct Cyc_Exp_tok_struct _tmp5CF;_tmp5CF.tag=Cyc_Exp_tok;_tmp5CF.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5D0=_cycalloc(sizeof(*_tmp5D0));
_tmp5D0[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp5D1;_tmp5D1.tag=34;
_tmp5D1.f1=0;_tmp5D1.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp5D1;});_tmp5D0;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp5CF;});_tmp5CE;});break;case 188: _LL27A: {struct
Cyc_Absyn_Vardecl*_tmp5D2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp5D8=
_cycalloc(sizeof(*_tmp5D8));_tmp5D8->f1=(void*)0;_tmp5D8->f2=({struct _tagged_arr*
_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp5D9;});_tmp5D8;}),
Cyc_Absyn_uint_t,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));_tmp5D2->tq=({
struct Cyc_Absyn_Tqual _tmp5D3;_tmp5D3.q_const=1;_tmp5D3.q_volatile=0;_tmp5D3.q_restrict=
1;_tmp5D3;});yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp5D4=_cycalloc(sizeof(*
_tmp5D4));_tmp5D4[0]=({struct Cyc_Exp_tok_struct _tmp5D5;_tmp5D5.tag=Cyc_Exp_tok;
_tmp5D5.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp5D7;_tmp5D7.tag=27;_tmp5D7.f1=_tmp5D2;_tmp5D7.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5D7.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D7;});_tmp5D6;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmp5D5;});
_tmp5D4;});break;}case 189: _LL27B: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA[0]=({struct Cyc_InitializerList_tok_struct
_tmp5DB;_tmp5DB.tag=Cyc_InitializerList_tok;_tmp5DB.f1=({struct Cyc_List_List*
_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC->hd=({struct _tuple17*_tmp5DD=
_cycalloc(sizeof(*_tmp5DD));_tmp5DD->f1=0;_tmp5DD->f2=Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]);_tmp5DD;});_tmp5DC->tl=0;_tmp5DC;});_tmp5DB;});_tmp5DA;});break;
case 190: _LL27C: yyval=(void*)({struct Cyc_InitializerList_tok_struct*_tmp5DE=
_cycalloc(sizeof(*_tmp5DE));_tmp5DE[0]=({struct Cyc_InitializerList_tok_struct
_tmp5DF;_tmp5DF.tag=Cyc_InitializerList_tok;_tmp5DF.f1=({struct Cyc_List_List*
_tmp5E0=_cycalloc(sizeof(*_tmp5E0));_tmp5E0->hd=({struct _tuple17*_tmp5E1=
_cycalloc(sizeof(*_tmp5E1));_tmp5E1->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E1->f2=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp5E1;});_tmp5E0->tl=0;_tmp5E0;});_tmp5DF;});_tmp5DE;});
break;case 191: _LL27D: yyval=(void*)({struct Cyc_InitializerList_tok_struct*_tmp5E2=
_cycalloc(sizeof(*_tmp5E2));_tmp5E2[0]=({struct Cyc_InitializerList_tok_struct
_tmp5E3;_tmp5E3.tag=Cyc_InitializerList_tok;_tmp5E3.f1=({struct Cyc_List_List*
_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->hd=({struct _tuple17*_tmp5E5=
_cycalloc(sizeof(*_tmp5E5));_tmp5E5->f1=0;_tmp5E5->f2=Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]);_tmp5E5;});_tmp5E4->tl=Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5E4;});_tmp5E3;});
_tmp5E2;});break;case 192: _LL27E: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6[0]=({struct Cyc_InitializerList_tok_struct
_tmp5E7;_tmp5E7.tag=Cyc_InitializerList_tok;_tmp5E7.f1=({struct Cyc_List_List*
_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->hd=({struct _tuple17*_tmp5E9=
_cycalloc(sizeof(*_tmp5E9));_tmp5E9->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E9->f2=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp5E9;});_tmp5E8->tl=Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5E8;});_tmp5E7;});
_tmp5E6;});break;case 193: _LL27F: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA[0]=({struct Cyc_DesignatorList_tok_struct
_tmp5EB;_tmp5EB.tag=Cyc_DesignatorList_tok;_tmp5EB.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp5EB;});_tmp5EA;});
break;case 194: _LL280: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp5EC=
_cycalloc(sizeof(*_tmp5EC));_tmp5EC[0]=({struct Cyc_DesignatorList_tok_struct
_tmp5ED;_tmp5ED.tag=Cyc_DesignatorList_tok;_tmp5ED.f1=({struct Cyc_List_List*
_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[yyvsp_offset]);_tmp5EE->tl=0;_tmp5EE;});_tmp5ED;});_tmp5EC;});break;case 195:
_LL281: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp5EF=_cycalloc(
sizeof(*_tmp5EF));_tmp5EF[0]=({struct Cyc_DesignatorList_tok_struct _tmp5F0;
_tmp5F0.tag=Cyc_DesignatorList_tok;_tmp5F0.f1=({struct Cyc_List_List*_tmp5F1=
_cycalloc(sizeof(*_tmp5F1));_tmp5F1->hd=(void*)Cyc_yyget_Designator_tok(yyvs[
yyvsp_offset]);_tmp5F1->tl=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5F1;});_tmp5F0;});
_tmp5EF;});break;case 196: _LL282: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2[0]=({struct Cyc_Designator_tok_struct
_tmp5F3;_tmp5F3.tag=Cyc_Designator_tok;_tmp5F3.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp5F5;_tmp5F5.tag=0;_tmp5F5.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5F5;});_tmp5F4;}));
_tmp5F3;});_tmp5F2;});break;case 197: _LL283: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6[0]=({struct Cyc_Designator_tok_struct
_tmp5F7;_tmp5F7.tag=Cyc_Designator_tok;_tmp5F7.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8[0]=({struct Cyc_Absyn_FieldName_struct
_tmp5F9;_tmp5F9.tag=1;_tmp5F9.f1=({struct _tagged_arr*_tmp5FA=_cycalloc(sizeof(*
_tmp5FA));_tmp5FA[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp5FA;});_tmp5F9;});
_tmp5F8;}));_tmp5F7;});_tmp5F6;});break;case 198: _LL284: {void*_tmp5FB=Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));struct Cyc_List_List*
_tmp5FC=(*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f3;if(_tmp5FC != 0)({
void*_tmp5FD[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("ignoring attributes in type",
sizeof(char),28),_tag_arr(_tmp5FD,sizeof(void*),0));});{struct Cyc_Absyn_Tqual
_tmp5FE=(*Cyc_yyget_QualSpecList_tok(yyvs[yyvsp_offset])).f1;yyval=(void*)({
struct Cyc_ParamDecl_tok_struct*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF[0]=({
struct Cyc_ParamDecl_tok_struct _tmp600;_tmp600.tag=Cyc_ParamDecl_tok;_tmp600.f1=({
struct _tuple2*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->f1=0;_tmp601->f2=
_tmp5FE;_tmp601->f3=_tmp5FB;_tmp601;});_tmp600;});_tmp5FF;});break;}}case 199:
_LL285: {void*_tmp602=Cyc_Parse_speclist2typ((*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp603=(*Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f3;struct Cyc_Absyn_Tqual _tmp604=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f1;struct Cyc_List_List*
_tmp605=(Cyc_yyget_AbstractDeclarator_tok(yyvs[yyvsp_offset]))->tms;struct
_tuple6 _tmp606=Cyc_Parse_apply_tms(_tmp604,_tmp602,_tmp603,_tmp605);if(_tmp606.f3
!= 0)({void*_tmp607[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line),_tag_arr("bad type params, ignoring",
sizeof(char),26),_tag_arr(_tmp607,sizeof(void*),0));});if(_tmp606.f4 != 0)({void*
_tmp608[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line),_tag_arr("bad specifiers, ignoring",sizeof(char),
25),_tag_arr(_tmp608,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609[0]=({struct Cyc_ParamDecl_tok_struct
_tmp60A;_tmp60A.tag=Cyc_ParamDecl_tok;_tmp60A.f1=({struct _tuple2*_tmp60B=
_cycalloc(sizeof(*_tmp60B));_tmp60B->f1=0;_tmp60B->f2=_tmp606.f1;_tmp60B->f3=
_tmp606.f2;_tmp60B;});_tmp60A;});_tmp609;});break;}case 200: _LL286: yyval=(void*)({
struct Cyc_Type_tok_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C[0]=({
struct Cyc_Type_tok_struct _tmp60D;_tmp60D.tag=Cyc_Type_tok;_tmp60D.f1=(void*)(*
Cyc_yyget_ParamDecl_tok(yyvs[yyvsp_offset])).f3;_tmp60D;});_tmp60C;});break;case
201: _LL287: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp60E=_cycalloc(sizeof(*
_tmp60E));_tmp60E[0]=({struct Cyc_Type_tok_struct _tmp60F;_tmp60F.tag=Cyc_Type_tok;
_tmp60F.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp610=_cycalloc(
sizeof(*_tmp610));_tmp610[0]=({struct Cyc_Absyn_JoinEff_struct _tmp611;_tmp611.tag=
20;_tmp611.f1=0;_tmp611;});_tmp610;}));_tmp60F;});_tmp60E;});break;case 202:
_LL288: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp612=_cycalloc(sizeof(*
_tmp612));_tmp612[0]=({struct Cyc_Type_tok_struct _tmp613;_tmp613.tag=Cyc_Type_tok;
_tmp613.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp614=_cycalloc(
sizeof(*_tmp614));_tmp614[0]=({struct Cyc_Absyn_JoinEff_struct _tmp615;_tmp615.tag=
20;_tmp615.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp615;});_tmp614;}));_tmp613;});_tmp612;});break;case 203:
_LL289: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp616=_cycalloc(sizeof(*
_tmp616));_tmp616[0]=({struct Cyc_Type_tok_struct _tmp617;_tmp617.tag=Cyc_Type_tok;
_tmp617.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp618=_cycalloc(
sizeof(*_tmp618));_tmp618[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp619;_tmp619.tag=
21;_tmp619.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp619;});_tmp618;}));_tmp617;});_tmp616;});break;case
204: _LL28A: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp61A=_cycalloc(sizeof(*
_tmp61A));_tmp61A[0]=({struct Cyc_Type_tok_struct _tmp61B;_tmp61B.tag=Cyc_Type_tok;
_tmp61B.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp61C=_cycalloc(
sizeof(*_tmp61C));_tmp61C[0]=({struct Cyc_Absyn_JoinEff_struct _tmp61D;_tmp61D.tag=
20;_tmp61D.f1=({struct Cyc_List_List*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp61E->tl=Cyc_yyget_TypeList_tok(yyvs[yyvsp_offset]);_tmp61E;});_tmp61D;});
_tmp61C;}));_tmp61B;});_tmp61A;});break;case 205: _LL28B: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F[0]=({struct Cyc_TypeList_tok_struct
_tmp620;_tmp620.tag=Cyc_TypeList_tok;_tmp620.f1=({struct Cyc_List_List*_tmp621=
_cycalloc(sizeof(*_tmp621));_tmp621->hd=(void*)Cyc_yyget_Type_tok(yyvs[
yyvsp_offset]);_tmp621->tl=0;_tmp621;});_tmp620;});_tmp61F;});break;case 206:
_LL28C: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp622=_cycalloc(sizeof(*
_tmp622));_tmp622[0]=({struct Cyc_TypeList_tok_struct _tmp623;_tmp623.tag=Cyc_TypeList_tok;
_tmp623.f1=({struct Cyc_List_List*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->hd=(
void*)Cyc_yyget_Type_tok(yyvs[yyvsp_offset]);_tmp624->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp624;});_tmp623;});
_tmp622;});break;case 207: _LL28D: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp626;_tmp626.tag=Cyc_AbstractDeclarator_tok;_tmp626.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[yyvsp_offset]);_tmp627;});_tmp626;});_tmp625;});break;case 208: _LL28E: yyval=
yyvs[yyvsp_offset];break;case 209: _LL28F: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp629;_tmp629.tag=Cyc_AbstractDeclarator_tok;_tmp629.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[yyvsp_offset]))->tms);_tmp62A;});_tmp629;});_tmp628;});break;case 210: _LL290:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 211:
_LL291: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp62B=_cycalloc(
sizeof(*_tmp62B));_tmp62B[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp62C;
_tmp62C.tag=Cyc_AbstractDeclarator_tok;_tmp62C.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D->tms=({struct Cyc_List_List*_tmp62E=
_cycalloc(sizeof(*_tmp62E));_tmp62E->hd=(void*)((void*)0);_tmp62E->tl=0;_tmp62E;});
_tmp62D;});_tmp62C;});_tmp62B;});break;case 212: _LL292: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp630;_tmp630.tag=Cyc_AbstractDeclarator_tok;_tmp630.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->tms=({struct Cyc_List_List*_tmp632=
_cycalloc(sizeof(*_tmp632));_tmp632->hd=(void*)((void*)0);_tmp632->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]))->tms;_tmp632;});
_tmp631;});_tmp630;});_tmp62F;});break;case 213: _LL293: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp634;_tmp634.tag=Cyc_AbstractDeclarator_tok;_tmp634.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635->tms=({struct Cyc_List_List*_tmp636=
_cycalloc(sizeof(*_tmp636));_tmp636->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp638;_tmp638.tag=0;_tmp638.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp638;});_tmp637;}));
_tmp636->tl=0;_tmp636;});_tmp635;});_tmp634;});_tmp633;});break;case 214: _LL294:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp639=_cycalloc(sizeof(*
_tmp639));_tmp639[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp63A;_tmp63A.tag=
Cyc_AbstractDeclarator_tok;_tmp63A.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B->tms=({struct Cyc_List_List*_tmp63C=
_cycalloc(sizeof(*_tmp63C));_tmp63C->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp63E;_tmp63E.tag=0;_tmp63E.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp63E;});_tmp63D;}));
_tmp63C->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp63C;});_tmp63B;});
_tmp63A;});_tmp639;});break;case 215: _LL295: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp640;_tmp640.tag=Cyc_AbstractDeclarator_tok;_tmp640.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->tms=({struct Cyc_List_List*_tmp642=
_cycalloc(sizeof(*_tmp642));_tmp642->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp643=_cycalloc(sizeof(*_tmp643));_tmp643[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp644;_tmp644.tag=2;_tmp644.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp646;_tmp646.tag=1;_tmp646.f1=0;_tmp646.f2=0;_tmp646.f3=0;_tmp646.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp646.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp646;});_tmp645;}));
_tmp644;});_tmp643;}));_tmp642->tl=0;_tmp642;});_tmp641;});_tmp640;});_tmp63F;});
break;case 216: _LL296: {struct _tuple14 _tmp648;struct Cyc_List_List*_tmp649;int
_tmp64A;struct Cyc_Absyn_VarargInfo*_tmp64B;struct Cyc_Core_Opt*_tmp64C;struct Cyc_List_List*
_tmp64D;struct _tuple14*_tmp647=Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp648=*_tmp647;_tmp649=_tmp648.f1;_tmp64A=_tmp648.f2;
_tmp64B=_tmp648.f3;_tmp64C=_tmp648.f4;_tmp64D=_tmp648.f5;yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp64F;_tmp64F.tag=Cyc_AbstractDeclarator_tok;
_tmp64F.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp650=_cycalloc(sizeof(*
_tmp650));_tmp650->tms=({struct Cyc_List_List*_tmp651=_cycalloc(sizeof(*_tmp651));
_tmp651->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp652=
_cycalloc(sizeof(*_tmp652));_tmp652[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp653;_tmp653.tag=2;_tmp653.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp655;_tmp655.tag=1;_tmp655.f1=_tmp649;_tmp655.f2=_tmp64A;_tmp655.f3=_tmp64B;
_tmp655.f4=_tmp64C;_tmp655.f5=_tmp64D;_tmp655;});_tmp654;}));_tmp653;});_tmp652;}));
_tmp651->tl=0;_tmp651;});_tmp650;});_tmp64F;});_tmp64E;});break;}case 217: _LL297:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp656=_cycalloc(sizeof(*
_tmp656));_tmp656[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp657;_tmp657.tag=
Cyc_AbstractDeclarator_tok;_tmp657.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->tms=({struct Cyc_List_List*_tmp659=
_cycalloc(sizeof(*_tmp659));_tmp659->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp65B;_tmp65B.tag=2;_tmp65B.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp65D;_tmp65D.tag=1;_tmp65D.f1=0;_tmp65D.f2=0;_tmp65D.f3=0;_tmp65D.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp65D.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp65D;});_tmp65C;}));
_tmp65B;});_tmp65A;}));_tmp659->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp659;});_tmp658;});
_tmp657;});_tmp656;});break;case 218: _LL298: {struct _tuple14 _tmp65F;struct Cyc_List_List*
_tmp660;int _tmp661;struct Cyc_Absyn_VarargInfo*_tmp662;struct Cyc_Core_Opt*_tmp663;
struct Cyc_List_List*_tmp664;struct _tuple14*_tmp65E=Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp65F=*_tmp65E;_tmp660=
_tmp65F.f1;_tmp661=_tmp65F.f2;_tmp662=_tmp65F.f3;_tmp663=_tmp65F.f4;_tmp664=
_tmp65F.f5;yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp665=
_cycalloc(sizeof(*_tmp665));_tmp665[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp666;_tmp666.tag=Cyc_AbstractDeclarator_tok;_tmp666.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->tms=({struct Cyc_List_List*_tmp668=
_cycalloc(sizeof(*_tmp668));_tmp668->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp66A;_tmp66A.tag=2;_tmp66A.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp66C;_tmp66C.tag=1;_tmp66C.f1=_tmp660;_tmp66C.f2=_tmp661;_tmp66C.f3=_tmp662;
_tmp66C.f4=_tmp663;_tmp66C.f5=_tmp664;_tmp66C;});_tmp66B;}));_tmp66A;});_tmp669;}));
_tmp668->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp668;});_tmp667;});
_tmp666;});_tmp665;});break;}case 219: _LL299: {struct Cyc_List_List*_tmp66D=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line),Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp66E=_cycalloc(sizeof(*
_tmp66E));_tmp66E[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp66F;_tmp66F.tag=
Cyc_AbstractDeclarator_tok;_tmp66F.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670->tms=({struct Cyc_List_List*_tmp671=
_cycalloc(sizeof(*_tmp671));_tmp671->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp673;_tmp673.tag=3;_tmp673.f1=_tmp66D;_tmp673.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp673.f3=0;_tmp673;});_tmp672;}));_tmp671->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp671;});
_tmp670;});_tmp66F;});_tmp66E;});break;}case 220: _LL29A: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp675;_tmp675.tag=Cyc_AbstractDeclarator_tok;_tmp675.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->tms=({struct Cyc_List_List*_tmp677=
_cycalloc(sizeof(*_tmp677));_tmp677->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp679;_tmp679.tag=4;_tmp679.f1=Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line);_tmp679.f2=Cyc_yyget_AttributeList_tok(yyvs[
yyvsp_offset]);_tmp679;});_tmp678;}));_tmp677->tl=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp677;});
_tmp676;});_tmp675;});_tmp674;});break;case 221: _LL29B: yyval=yyvs[yyvsp_offset];
break;case 222: _LL29C: yyval=yyvs[yyvsp_offset];break;case 223: _LL29D: yyval=yyvs[
yyvsp_offset];break;case 224: _LL29E: yyval=yyvs[yyvsp_offset];break;case 225: _LL29F:
yyval=yyvs[yyvsp_offset];break;case 226: _LL2A0: yyval=yyvs[yyvsp_offset];break;
case 227: _LL2A1: if(Cyc_Std_zstrcmp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tag_arr("`H",sizeof(
char),3))== 0)Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",sizeof(
char),33),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684->name=({struct _tagged_arr*_tmp687=
_cycalloc(sizeof(*_tmp687));_tmp687[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp687;});_tmp684->identity=
0;_tmp684->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp685=_cycalloc(
sizeof(*_tmp685));_tmp685[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp686;_tmp686.tag=0;
_tmp686.f1=(void*)((void*)3);_tmp686;});_tmp685;}));_tmp684;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682[0]=({
struct Cyc_Absyn_VarType_struct _tmp683;_tmp683.tag=1;_tmp683.f1=tv;_tmp683;});
_tmp682;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp67A=_cycalloc(sizeof(*
_tmp67A));_tmp67A[0]=({struct Cyc_Stmt_tok_struct _tmp67B;_tmp67B.tag=Cyc_Stmt_tok;
_tmp67B.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp67C=
_cycalloc(sizeof(*_tmp67C));_tmp67C[0]=({struct Cyc_Absyn_Region_s_struct _tmp67D;
_tmp67D.tag=18;_tmp67D.f1=tv;_tmp67D.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E->f1=(void*)0;_tmp67E->f2=({struct
_tagged_arr*_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp67F;});_tmp67E;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp680=_cycalloc(sizeof(*_tmp680));
_tmp680[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp681;_tmp681.tag=15;_tmp681.f1=(
void*)t;_tmp681;});_tmp680;}),0);_tmp67D.f3=0;_tmp67D.f4=Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]);_tmp67D;});_tmp67C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp67B;});_tmp67A;});break;}case 228: _LL2A2: if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tag_arr("H",sizeof(char),2))== 0)Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692->name=({
struct _tagged_arr*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695[0]=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp697;_tmp697.tag=0;_tmp697.f1=(
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);{void*_tmp696[1]={& _tmp697};Cyc_Std_aprintf(_tag_arr("`%s",
sizeof(char),4),_tag_arr(_tmp696,sizeof(void*),1));}});_tmp695;});_tmp692->identity=
0;_tmp692->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp693=_cycalloc(
sizeof(*_tmp693));_tmp693[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp694;_tmp694.tag=0;
_tmp694.f1=(void*)((void*)3);_tmp694;});_tmp693;}));_tmp692;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690[0]=({
struct Cyc_Absyn_VarType_struct _tmp691;_tmp691.tag=1;_tmp691.f1=tv;_tmp691;});
_tmp690;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp688=_cycalloc(sizeof(*
_tmp688));_tmp688[0]=({struct Cyc_Stmt_tok_struct _tmp689;_tmp689.tag=Cyc_Stmt_tok;
_tmp689.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp68A=
_cycalloc(sizeof(*_tmp68A));_tmp68A[0]=({struct Cyc_Absyn_Region_s_struct _tmp68B;
_tmp68B.tag=18;_tmp68B.f1=tv;_tmp68B.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C->f1=(void*)0;_tmp68C->f2=({struct
_tagged_arr*_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp68D;});_tmp68C;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp68E=_cycalloc(sizeof(*_tmp68E));
_tmp68E[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp68F;_tmp68F.tag=15;_tmp68F.f1=(
void*)t;_tmp68F;});_tmp68E;}),0);_tmp68B.f3=0;_tmp68B.f4=Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]);_tmp68B;});_tmp68A;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp689;});_tmp688;});break;}case 229: _LL2A3: if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tag_arr("resetable",sizeof(char),10))!= 0)Cyc_Parse_err(_tag_arr("expecting [resetable]",
sizeof(char),22),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tag_arr("`H",sizeof(char),3))== 0)Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));_tmp6A2->name=({
struct _tagged_arr*_tmp6A5=_cycalloc(sizeof(*_tmp6A5));_tmp6A5[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6A5;});_tmp6A2->identity=
0;_tmp6A2->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6A3=_cycalloc(
sizeof(*_tmp6A3));_tmp6A3[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6A4;_tmp6A4.tag=0;
_tmp6A4.f1=(void*)((void*)3);_tmp6A4;});_tmp6A3;}));_tmp6A2;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0[0]=({
struct Cyc_Absyn_VarType_struct _tmp6A1;_tmp6A1.tag=1;_tmp6A1.f1=tv;_tmp6A1;});
_tmp6A0;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp698=_cycalloc(sizeof(*
_tmp698));_tmp698[0]=({struct Cyc_Stmt_tok_struct _tmp699;_tmp699.tag=Cyc_Stmt_tok;
_tmp699.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp69A=
_cycalloc(sizeof(*_tmp69A));_tmp69A[0]=({struct Cyc_Absyn_Region_s_struct _tmp69B;
_tmp69B.tag=18;_tmp69B.f1=tv;_tmp69B.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C->f1=(void*)0;_tmp69C->f2=({struct
_tagged_arr*_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp69D;});_tmp69C;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp69E=_cycalloc(sizeof(*_tmp69E));
_tmp69E[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp69F;_tmp69F.tag=15;_tmp69F.f1=(
void*)t;_tmp69F;});_tmp69E;}),0);_tmp69B.f3=1;_tmp69B.f4=Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]);_tmp69B;});_tmp69A;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp699;});_tmp698;});break;}case 230: _LL2A4: if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tag_arr("resetable",sizeof(char),10))!= 0)Cyc_Parse_err(_tag_arr("expecting `resetable'",
sizeof(char),22),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_Std_zstrcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tag_arr("H",sizeof(char),2))== 0)Cyc_Parse_err(_tag_arr("bad occurrence of heap region `H",
sizeof(char),33),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));_tmp6B0->name=({
struct _tagged_arr*_tmp6B3=_cycalloc(sizeof(*_tmp6B3));_tmp6B3[0]=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp6B5;_tmp6B5.tag=0;_tmp6B5.f1=(
struct _tagged_arr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);{void*_tmp6B4[1]={& _tmp6B5};Cyc_Std_aprintf(_tag_arr("`%s",
sizeof(char),4),_tag_arr(_tmp6B4,sizeof(void*),1));}});_tmp6B3;});_tmp6B0->identity=
0;_tmp6B0->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6B1=_cycalloc(
sizeof(*_tmp6B1));_tmp6B1[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6B2;_tmp6B2.tag=0;
_tmp6B2.f1=(void*)((void*)3);_tmp6B2;});_tmp6B1;}));_tmp6B0;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));_tmp6AE[0]=({
struct Cyc_Absyn_VarType_struct _tmp6AF;_tmp6AF.tag=1;_tmp6AF.f1=tv;_tmp6AF;});
_tmp6AE;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6A6=_cycalloc(sizeof(*
_tmp6A6));_tmp6A6[0]=({struct Cyc_Stmt_tok_struct _tmp6A7;_tmp6A7.tag=Cyc_Stmt_tok;
_tmp6A7.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp6A8=
_cycalloc(sizeof(*_tmp6A8));_tmp6A8[0]=({struct Cyc_Absyn_Region_s_struct _tmp6A9;
_tmp6A9.tag=18;_tmp6A9.f1=tv;_tmp6A9.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA->f1=(void*)0;_tmp6AA->f2=({struct
_tagged_arr*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));_tmp6AB[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6AB;});_tmp6AA;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));
_tmp6AC[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp6AD;_tmp6AD.tag=15;_tmp6AD.f1=(
void*)t;_tmp6AD;});_tmp6AC;}),0);_tmp6A9.f3=1;_tmp6A9.f4=Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]);_tmp6A9;});_tmp6A8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6A7;});_tmp6A6;});break;}case 231: _LL2A5: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6[0]=({
struct Cyc_Stmt_tok_struct _tmp6B7;_tmp6B7.tag=Cyc_Stmt_tok;_tmp6B7.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_ResetRegion_s_struct*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));
_tmp6B8[0]=({struct Cyc_Absyn_ResetRegion_s_struct _tmp6B9;_tmp6B9.tag=20;_tmp6B9.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp6B9;});_tmp6B8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6B7;});_tmp6B6;});break;case 232: _LL2A6: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA[0]=({
struct Cyc_Stmt_tok_struct _tmp6BB;_tmp6BB.tag=Cyc_Stmt_tok;_tmp6BB.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Cut_s_struct*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC[
0]=({struct Cyc_Absyn_Cut_s_struct _tmp6BD;_tmp6BD.tag=13;_tmp6BD.f1=Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]);_tmp6BD;});_tmp6BC;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6BB;});_tmp6BA;});break;case 233: _LL2A7: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE[0]=({
struct Cyc_Stmt_tok_struct _tmp6BF;_tmp6BF.tag=Cyc_Stmt_tok;_tmp6BF.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));
_tmp6C0[0]=({struct Cyc_Absyn_Splice_s_struct _tmp6C1;_tmp6C1.tag=14;_tmp6C1.f1=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp6C1;});_tmp6C0;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6BF;});_tmp6BE;});break;case 234: _LL2A8: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2[0]=({
struct Cyc_Stmt_tok_struct _tmp6C3;_tmp6C3.tag=Cyc_Stmt_tok;_tmp6C3.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp6C4=_cycalloc(sizeof(*_tmp6C4));
_tmp6C4[0]=({struct Cyc_Absyn_Label_s_struct _tmp6C5;_tmp6C5.tag=15;_tmp6C5.f1=({
struct _tagged_arr*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6C6;});_tmp6C5.f2=
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp6C5;});_tmp6C4;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6C3;});_tmp6C2;});break;case 235: _LL2A9: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7[0]=({
struct Cyc_Stmt_tok_struct _tmp6C8;_tmp6C8.tag=Cyc_Stmt_tok;_tmp6C8.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmp6C8;});_tmp6C7;});break;case 236: _LL2AA: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9[0]=({struct Cyc_Stmt_tok_struct
_tmp6CA;_tmp6CA.tag=Cyc_Stmt_tok;_tmp6CA.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6CA;});_tmp6C9;});break;case 237: _LL2AB: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB[0]=({
struct Cyc_Stmt_tok_struct _tmp6CC;_tmp6CC.tag=Cyc_Stmt_tok;_tmp6CC.f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6CC;});
_tmp6CB;});break;case 238: _LL2AC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 239: _LL2AD: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD[0]=({struct Cyc_Stmt_tok_struct
_tmp6CE;_tmp6CE.tag=Cyc_Stmt_tok;_tmp6CE.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(0));_tmp6CE;});_tmp6CD;});break;case 240:
_LL2AE: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6CF=_cycalloc(sizeof(*
_tmp6CF));_tmp6CF[0]=({struct Cyc_Stmt_tok_struct _tmp6D0;_tmp6D0.tag=Cyc_Stmt_tok;
_tmp6D0.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]));_tmp6D0;});_tmp6CF;});break;case 241: _LL2AF: yyval=yyvs[
yyvsp_offset];break;case 242: _LL2B0: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1[0]=({struct Cyc_Stmt_tok_struct
_tmp6D2;_tmp6D2.tag=Cyc_Stmt_tok;_tmp6D2.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6D2;});_tmp6D1;});break;case 243: _LL2B1: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3[0]=({
struct Cyc_Stmt_tok_struct _tmp6D4;_tmp6D4.tag=Cyc_Stmt_tok;_tmp6D4.f1=Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*_tmp6D5=_cycalloc(
sizeof(*_tmp6D5));_tmp6D5[0]=({struct Cyc_Absyn_Fn_d_struct _tmp6D6;_tmp6D6.tag=1;
_tmp6D6.f1=Cyc_yyget_FnDecl_tok(yyvs[yyvsp_offset]);_tmp6D6;});_tmp6D5;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line)),Cyc_Absyn_skip_stmt(
0));_tmp6D4;});_tmp6D3;});break;case 244: _LL2B2: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7[0]=({struct Cyc_Stmt_tok_struct
_tmp6D8;_tmp6D8.tag=Cyc_Stmt_tok;_tmp6D8.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp6DA;_tmp6DA.tag=1;_tmp6DA.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DA;});_tmp6D9;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]));_tmp6D8;});
_tmp6D7;});break;case 245: _LL2B3: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6DB=
_cycalloc(sizeof(*_tmp6DB));_tmp6DB[0]=({struct Cyc_Stmt_tok_struct _tmp6DC;
_tmp6DC.tag=Cyc_Stmt_tok;_tmp6DC.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[yyvsp_offset]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6DC;});_tmp6DB;});break;case 246: _LL2B4: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD[0]=({
struct Cyc_Stmt_tok_struct _tmp6DE;_tmp6DE.tag=Cyc_Stmt_tok;_tmp6DE.f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6DE;});_tmp6DD;});break;case 247: _LL2B5: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF[0]=({
struct Cyc_Stmt_tok_struct _tmp6E0;_tmp6E0.tag=Cyc_Stmt_tok;_tmp6E0.f1=Cyc_Absyn_switch_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6E0;});_tmp6DF;});break;case 248: _LL2B6: if(Cyc_Std_strcmp(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tag_arr("C",sizeof(char),2))!= 0)Cyc_Parse_err(_tag_arr("only switch \"C\" { ... } is allowed",
sizeof(char),35),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6E1=
_cycalloc(sizeof(*_tmp6E1));_tmp6E1[0]=({struct Cyc_Stmt_tok_struct _tmp6E2;
_tmp6E2.tag=Cyc_Stmt_tok;_tmp6E2.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_SwitchC_s_struct*
_tmp6E3=_cycalloc(sizeof(*_tmp6E3));_tmp6E3[0]=({struct Cyc_Absyn_SwitchC_s_struct
_tmp6E4;_tmp6E4.tag=10;_tmp6E4.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp6E4.f2=Cyc_yyget_SwitchCClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E4;});_tmp6E3;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[yylsp_offset]).last_line));_tmp6E2;});
_tmp6E1;});break;case 249: _LL2B7: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp6E5=
_cycalloc(sizeof(*_tmp6E5));_tmp6E5[0]=({struct Cyc_Stmt_tok_struct _tmp6E6;
_tmp6E6.tag=Cyc_Stmt_tok;_tmp6E6.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp6E6;});_tmp6E5;});break;case 250: _LL2B8: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp6E7=_cycalloc(sizeof(*_tmp6E7));
_tmp6E7[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp6E8;_tmp6E8.tag=Cyc_SwitchClauseList_tok;
_tmp6E8.f1=0;_tmp6E8;});_tmp6E7;});break;case 251: _LL2B9: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6EA;_tmp6EA.tag=Cyc_SwitchClauseList_tok;_tmp6EA.f1=({struct Cyc_List_List*
_tmp6EB=_cycalloc(sizeof(*_tmp6EB));_tmp6EB->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6EC=_cycalloc(sizeof(*_tmp6EC));_tmp6EC->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp6EC->pat_vars=0;_tmp6EC->where_clause=0;
_tmp6EC->body=Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]);_tmp6EC->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6EC;});_tmp6EB->tl=0;_tmp6EB;});_tmp6EA;});_tmp6E9;});
break;case 252: _LL2BA: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6ED=_cycalloc(sizeof(*_tmp6ED));_tmp6ED[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6EE;_tmp6EE.tag=Cyc_SwitchClauseList_tok;_tmp6EE.f1=({struct Cyc_List_List*
_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6F0=_cycalloc(sizeof(*_tmp6F0));_tmp6F0->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6F0->pat_vars=0;
_tmp6F0->where_clause=0;_tmp6F0->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp6F0->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line);_tmp6F0;});_tmp6EF->tl=
Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmp6EF;});_tmp6EE;});_tmp6ED;});
break;case 253: _LL2BB: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6F1=_cycalloc(sizeof(*_tmp6F1));_tmp6F1[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6F2;_tmp6F2.tag=Cyc_SwitchClauseList_tok;_tmp6F2.f1=({struct Cyc_List_List*
_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6F4->pat_vars=0;
_tmp6F4->where_clause=0;_tmp6F4->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6F4->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp6F4;});
_tmp6F3->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[yyvsp_offset]);_tmp6F3;});
_tmp6F2;});_tmp6F1;});break;case 254: _LL2BC: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp6F5=_cycalloc(sizeof(*_tmp6F5));_tmp6F5[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp6F6;_tmp6F6.tag=Cyc_SwitchClauseList_tok;_tmp6F6.f1=({struct Cyc_List_List*
_tmp6F7=_cycalloc(sizeof(*_tmp6F7));_tmp6F7->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6F8=_cycalloc(sizeof(*_tmp6F8));_tmp6F8->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp6F8->pat_vars=0;
_tmp6F8->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6F8->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp6F8->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6F8;});_tmp6F7->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmp6F7;});_tmp6F6;});_tmp6F5;});break;case 255: _LL2BD: yyval=(
void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp6F9=_cycalloc(sizeof(*_tmp6F9));
_tmp6F9[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp6FA;_tmp6FA.tag=Cyc_SwitchClauseList_tok;
_tmp6FA.f1=({struct Cyc_List_List*_tmp6FB=_cycalloc(sizeof(*_tmp6FB));_tmp6FB->hd=({
struct Cyc_Absyn_Switch_clause*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp6FC->pat_vars=0;_tmp6FC->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6FC->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6FC->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp6FC;});_tmp6FB->tl=Cyc_yyget_SwitchClauseList_tok(
yyvs[yyvsp_offset]);_tmp6FB;});_tmp6FA;});_tmp6F9;});break;case 256: _LL2BE: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp6FD=_cycalloc(sizeof(*_tmp6FD));
_tmp6FD[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp6FE;_tmp6FE.tag=Cyc_SwitchCClauseList_tok;
_tmp6FE.f1=0;_tmp6FE;});_tmp6FD;});break;case 257: _LL2BF: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmp700;_tmp700.tag=Cyc_SwitchCClauseList_tok;_tmp700.f1=({struct Cyc_List_List*
_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701->hd=({struct Cyc_Absyn_SwitchC_clause*
_tmp702=_cycalloc(sizeof(*_tmp702));_tmp702->cnst_exp=0;_tmp702->body=Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Absyn_break_stmt(0),0);_tmp702->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line);_tmp702;});_tmp701->tl=
0;_tmp701;});_tmp700;});_tmp6FF;});break;case 258: _LL2C0: yyval=(void*)({struct Cyc_SwitchCClauseList_tok_struct*
_tmp703=_cycalloc(sizeof(*_tmp703));_tmp703[0]=({struct Cyc_SwitchCClauseList_tok_struct
_tmp704;_tmp704.tag=Cyc_SwitchCClauseList_tok;_tmp704.f1=({struct Cyc_List_List*
_tmp705=_cycalloc(sizeof(*_tmp705));_tmp705->hd=({struct Cyc_Absyn_SwitchC_clause*
_tmp706=_cycalloc(sizeof(*_tmp706));_tmp706->cnst_exp=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp706->body=Cyc_Absyn_fallthru_stmt(
0,0);_tmp706->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp706;});_tmp705->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmp705;});_tmp704;});_tmp703;});break;case 259: _LL2C1: yyval=(
void*)({struct Cyc_SwitchCClauseList_tok_struct*_tmp707=_cycalloc(sizeof(*_tmp707));
_tmp707[0]=({struct Cyc_SwitchCClauseList_tok_struct _tmp708;_tmp708.tag=Cyc_SwitchCClauseList_tok;
_tmp708.f1=({struct Cyc_List_List*_tmp709=_cycalloc(sizeof(*_tmp709));_tmp709->hd=({
struct Cyc_Absyn_SwitchC_clause*_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A->cnst_exp=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp70A->body=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Absyn_fallthru_stmt(
0,0),0);_tmp70A->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);_tmp70A;});_tmp709->tl=Cyc_yyget_SwitchCClauseList_tok(
yyvs[yyvsp_offset]);_tmp709;});_tmp708;});_tmp707;});break;case 260: _LL2C2: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp70B=_cycalloc(sizeof(*_tmp70B));_tmp70B[0]=({
struct Cyc_Stmt_tok_struct _tmp70C;_tmp70C.tag=Cyc_Stmt_tok;_tmp70C.f1=Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp70C;});_tmp70B;});break;case 261: _LL2C3: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D[0]=({
struct Cyc_Stmt_tok_struct _tmp70E;_tmp70E.tag=Cyc_Stmt_tok;_tmp70E.f1=Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[yylsp_offset]).last_line));_tmp70E;});
_tmp70D;});break;case 262: _LL2C4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp70F=
_cycalloc(sizeof(*_tmp70F));_tmp70F[0]=({struct Cyc_Stmt_tok_struct _tmp710;
_tmp710.tag=Cyc_Stmt_tok;_tmp710.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));_tmp710;});
_tmp70F;});break;case 263: _LL2C5: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp711=
_cycalloc(sizeof(*_tmp711));_tmp711[0]=({struct Cyc_Stmt_tok_struct _tmp712;
_tmp712.tag=Cyc_Stmt_tok;_tmp712.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp712;});_tmp711;});break;case 264: _LL2C6: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713[0]=({
struct Cyc_Stmt_tok_struct _tmp714;_tmp714.tag=Cyc_Stmt_tok;_tmp714.f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[yylsp_offset]).last_line));_tmp714;});
_tmp713;});break;case 265: _LL2C7: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp715=
_cycalloc(sizeof(*_tmp715));_tmp715[0]=({struct Cyc_Stmt_tok_struct _tmp716;
_tmp716.tag=Cyc_Stmt_tok;_tmp716.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp716;});
_tmp715;});break;case 266: _LL2C8: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp717=
_cycalloc(sizeof(*_tmp717));_tmp717[0]=({struct Cyc_Stmt_tok_struct _tmp718;
_tmp718.tag=Cyc_Stmt_tok;_tmp718.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp718;});_tmp717;});break;case 267: _LL2C9: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp719=_cycalloc(sizeof(*_tmp719));_tmp719[0]=({
struct Cyc_Stmt_tok_struct _tmp71A;_tmp71A.tag=Cyc_Stmt_tok;_tmp71A.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp71A;});_tmp719;});break;case 268: _LL2CA: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp71B=_cycalloc(sizeof(*_tmp71B));_tmp71B[0]=({
struct Cyc_Stmt_tok_struct _tmp71C;_tmp71C.tag=Cyc_Stmt_tok;_tmp71C.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp71C;});_tmp71B;});break;case 269: _LL2CB: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp71D=_cycalloc(sizeof(*_tmp71D));_tmp71D[0]=({
struct Cyc_Stmt_tok_struct _tmp71E;_tmp71E.tag=Cyc_Stmt_tok;_tmp71E.f1=Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp71E;});_tmp71D;});break;case 270: _LL2CC: {struct
Cyc_List_List*_tmp71F=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*_tmp720=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp721=_cycalloc(sizeof(*_tmp721));_tmp721[0]=({
struct Cyc_Stmt_tok_struct _tmp722;_tmp722.tag=Cyc_Stmt_tok;_tmp722.f1=Cyc_Parse_flatten_declarations(
_tmp71F,_tmp720);_tmp722;});_tmp721;});break;}case 271: _LL2CD: {struct Cyc_List_List*
_tmp723=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp724=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp725=
_cycalloc(sizeof(*_tmp725));_tmp725[0]=({struct Cyc_Stmt_tok_struct _tmp726;
_tmp726.tag=Cyc_Stmt_tok;_tmp726.f1=Cyc_Parse_flatten_declarations(_tmp723,
_tmp724);_tmp726;});_tmp725;});break;}case 272: _LL2CE: {struct Cyc_List_List*
_tmp727=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp728=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp729=
_cycalloc(sizeof(*_tmp729));_tmp729[0]=({struct Cyc_Stmt_tok_struct _tmp72A;
_tmp72A.tag=Cyc_Stmt_tok;_tmp72A.f1=Cyc_Parse_flatten_declarations(_tmp727,
_tmp728);_tmp72A;});_tmp729;});break;}case 273: _LL2CF: {struct Cyc_List_List*
_tmp72B=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp72C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
yylsp_offset]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp72D=
_cycalloc(sizeof(*_tmp72D));_tmp72D[0]=({struct Cyc_Stmt_tok_struct _tmp72E;
_tmp72E.tag=Cyc_Stmt_tok;_tmp72E.f1=Cyc_Parse_flatten_declarations(_tmp72B,
_tmp72C);_tmp72E;});_tmp72D;});break;}case 274: _LL2D0: {struct Cyc_List_List*
_tmp72F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_decl2vardecl,Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp730=_cycalloc(sizeof(*_tmp730));_tmp730[0]=({
struct Cyc_Stmt_tok_struct _tmp731;_tmp731.tag=Cyc_Stmt_tok;_tmp731.f1=Cyc_Absyn_forarray_stmt(
_tmp72F,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]),Cyc_yyget_Stmt_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp731;});_tmp730;});break;}case 275: _LL2D1: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732[0]=({
struct Cyc_Stmt_tok_struct _tmp733;_tmp733.tag=Cyc_Stmt_tok;_tmp733.f1=Cyc_Absyn_goto_stmt(({
struct _tagged_arr*_tmp734=_cycalloc(sizeof(*_tmp734));_tmp734[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp734;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp733;});
_tmp732;});break;case 276: _LL2D2: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp735=
_cycalloc(sizeof(*_tmp735));_tmp735[0]=({struct Cyc_Stmt_tok_struct _tmp736;
_tmp736.tag=Cyc_Stmt_tok;_tmp736.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp736;});
_tmp735;});break;case 277: _LL2D3: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp737=
_cycalloc(sizeof(*_tmp737));_tmp737[0]=({struct Cyc_Stmt_tok_struct _tmp738;
_tmp738.tag=Cyc_Stmt_tok;_tmp738.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp738;});
_tmp737;});break;case 278: _LL2D4: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp739=
_cycalloc(sizeof(*_tmp739));_tmp739[0]=({struct Cyc_Stmt_tok_struct _tmp73A;
_tmp73A.tag=Cyc_Stmt_tok;_tmp73A.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp73A;});
_tmp739;});break;case 279: _LL2D5: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp73B=
_cycalloc(sizeof(*_tmp73B));_tmp73B[0]=({struct Cyc_Stmt_tok_struct _tmp73C;
_tmp73C.tag=Cyc_Stmt_tok;_tmp73C.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp73C;});_tmp73B;});break;case 280: _LL2D6: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp73D=_cycalloc(sizeof(*_tmp73D));_tmp73D[0]=({
struct Cyc_Stmt_tok_struct _tmp73E;_tmp73E.tag=Cyc_Stmt_tok;_tmp73E.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp73E;});_tmp73D;});break;case 281: _LL2D7: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp73F=_cycalloc(sizeof(*_tmp73F));_tmp73F[0]=({
struct Cyc_Stmt_tok_struct _tmp740;_tmp740.tag=Cyc_Stmt_tok;_tmp740.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp740;});_tmp73F;});break;case 282: _LL2D8: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741[0]=({
struct Cyc_Stmt_tok_struct _tmp742;_tmp742.tag=Cyc_Stmt_tok;_tmp742.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp742;});_tmp741;});break;case 283: _LL2D9: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp743=_cycalloc(sizeof(*_tmp743));_tmp743[
0]=({struct Cyc_Pattern_tok_struct _tmp744;_tmp744.tag=Cyc_Pattern_tok;_tmp744.f1=
Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp744;});_tmp743;});break;case 284: _LL2DA: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 285: _LL2DB:
yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp745=_cycalloc(sizeof(*_tmp745));
_tmp745[0]=({struct Cyc_Pattern_tok_struct _tmp746;_tmp746.tag=Cyc_Pattern_tok;
_tmp746.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp747=
_cycalloc(sizeof(*_tmp747));_tmp747[0]=({struct Cyc_Absyn_Int_p_struct _tmp748;
_tmp748.tag=1;_tmp748.f1=(void*)(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f1;
_tmp748.f2=(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2;_tmp748;});_tmp747;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp746;});
_tmp745;});break;case 286: _LL2DC: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp749=_cycalloc(sizeof(*_tmp749));_tmp749[0]=({struct Cyc_Pattern_tok_struct
_tmp74A;_tmp74A.tag=Cyc_Pattern_tok;_tmp74A.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Int_p_struct*_tmp74B=_cycalloc(sizeof(*_tmp74B));_tmp74B[0]=({struct
Cyc_Absyn_Int_p_struct _tmp74C;_tmp74C.tag=1;_tmp74C.f1=(void*)((void*)0);_tmp74C.f2=
-(*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2;_tmp74C;});_tmp74B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp74A;});_tmp749;});break;case 287: _LL2DD: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D[
0]=({struct Cyc_Pattern_tok_struct _tmp74E;_tmp74E.tag=Cyc_Pattern_tok;_tmp74E.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*_tmp74F=_cycalloc(
sizeof(*_tmp74F));_tmp74F[0]=({struct Cyc_Absyn_Float_p_struct _tmp750;_tmp750.tag=
3;_tmp750.f1=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp750;});_tmp74F;}),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp74E;});
_tmp74D;});break;case 288: _LL2DE: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp751=_cycalloc(sizeof(*_tmp751));_tmp751[0]=({struct Cyc_Pattern_tok_struct
_tmp752;_tmp752.tag=Cyc_Pattern_tok;_tmp752.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Char_p_struct*_tmp753=_cycalloc_atomic(sizeof(*_tmp753));_tmp753[0]=({
struct Cyc_Absyn_Char_p_struct _tmp754;_tmp754.tag=2;_tmp754.f1=Cyc_yyget_Char_tok(
yyvs[yyvsp_offset]);_tmp754;});_tmp753;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp752;});_tmp751;});
break;case 289: _LL2DF: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp755=
_cycalloc(sizeof(*_tmp755));_tmp755[0]=({struct Cyc_Pattern_tok_struct _tmp756;
_tmp756.tag=Cyc_Pattern_tok;_tmp756.f1=Cyc_Absyn_new_pat((void*)1,Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp756;});
_tmp755;});break;case 290: _LL2E0: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp757=_cycalloc(sizeof(*_tmp757));_tmp757[0]=({struct Cyc_Pattern_tok_struct
_tmp758;_tmp758.tag=Cyc_Pattern_tok;_tmp758.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_UnknownId_p_struct*_tmp759=_cycalloc(sizeof(*_tmp759));_tmp759[0]=({
struct Cyc_Absyn_UnknownId_p_struct _tmp75A;_tmp75A.tag=11;_tmp75A.f1=Cyc_yyget_QualId_tok(
yyvs[yyvsp_offset]);_tmp75A;});_tmp759;}),Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp758;});_tmp757;});
break;case 291: _LL2E1: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp75B=
_cycalloc(sizeof(*_tmp75B));_tmp75B[0]=({struct Cyc_Pattern_tok_struct _tmp75C;
_tmp75C.tag=Cyc_Pattern_tok;_tmp75C.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_struct*
_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D[0]=({struct Cyc_Absyn_Tuple_p_struct
_tmp75E;_tmp75E.tag=4;_tmp75E.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]));_tmp75E;});_tmp75D;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp75C;});_tmp75B;});break;case 292: _LL2E2: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp75F=_cycalloc(sizeof(*_tmp75F));_tmp75F[
0]=({struct Cyc_Pattern_tok_struct _tmp760;_tmp760.tag=Cyc_Pattern_tok;_tmp760.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp761=
_cycalloc(sizeof(*_tmp761));_tmp761[0]=({struct Cyc_Absyn_UnknownCall_p_struct
_tmp762;_tmp762.tag=12;_tmp762.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp762.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_PatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp762;});_tmp761;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp760;});
_tmp75F;});break;case 293: _LL2E3: {struct Cyc_List_List*_tmp763=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp764=_cycalloc(sizeof(*_tmp764));_tmp764[0]=({
struct Cyc_Pattern_tok_struct _tmp765;_tmp765.tag=Cyc_Pattern_tok;_tmp765.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp766=_cycalloc(sizeof(*_tmp766));
_tmp766[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp767;_tmp767.tag=7;_tmp767.f1=({
struct Cyc_Absyn_AggrInfo _tmp768;_tmp768.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp769=_cycalloc(sizeof(*_tmp769));_tmp769[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp76A;_tmp76A.tag=0;_tmp76A.f1=(void*)((void*)0);_tmp76A.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp76A;});_tmp769;}));
_tmp768.targs=0;_tmp768;});_tmp767.f2=_tmp763;_tmp767.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp767;});_tmp766;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp765;});
_tmp764;});break;}case 294: _LL2E4: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp76B=_cycalloc(sizeof(*_tmp76B));_tmp76B[0]=({struct Cyc_Pattern_tok_struct
_tmp76C;_tmp76C.tag=Cyc_Pattern_tok;_tmp76C.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Pointer_p_struct*_tmp76D=_cycalloc(sizeof(*_tmp76D));_tmp76D[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp76E;_tmp76E.tag=5;_tmp76E.f1=Cyc_yyget_Pattern_tok(
yyvs[yyvsp_offset]);_tmp76E;});_tmp76D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp76C;});_tmp76B;});break;case 295: _LL2E5: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp76F=_cycalloc(sizeof(*_tmp76F));_tmp76F[
0]=({struct Cyc_Pattern_tok_struct _tmp770;_tmp770.tag=Cyc_Pattern_tok;_tmp770.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp771=_cycalloc(
sizeof(*_tmp771));_tmp771[0]=({struct Cyc_Absyn_Reference_p_struct _tmp772;_tmp772.tag=
6;_tmp772.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp773=_cycalloc(sizeof(*
_tmp773));_tmp773->f1=(void*)0;_tmp773->f2=({struct _tagged_arr*_tmp774=_cycalloc(
sizeof(*_tmp774));_tmp774[0]=Cyc_yyget_String_tok(yyvs[yyvsp_offset]);_tmp774;});
_tmp773;}),(void*)0,0);_tmp772;});_tmp771;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp770;});_tmp76F;});break;case 296: _LL2E6: yyval=(
void*)({struct Cyc_PatternList_tok_struct*_tmp775=_cycalloc(sizeof(*_tmp775));
_tmp775[0]=({struct Cyc_PatternList_tok_struct _tmp776;_tmp776.tag=Cyc_PatternList_tok;
_tmp776.f1=({struct Cyc_List_List*_tmp777=_cycalloc(sizeof(*_tmp777));_tmp777->hd=
Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmp777->tl=0;_tmp777;});_tmp776;});
_tmp775;});break;case 297: _LL2E7: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp778=_cycalloc(sizeof(*_tmp778));_tmp778[0]=({struct Cyc_PatternList_tok_struct
_tmp779;_tmp779.tag=Cyc_PatternList_tok;_tmp779.f1=({struct Cyc_List_List*_tmp77A=
_cycalloc(sizeof(*_tmp77A));_tmp77A->hd=Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);
_tmp77A->tl=Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp77A;});_tmp779;});_tmp778;});break;case 298: _LL2E8: yyval=(
void*)({struct Cyc_FieldPattern_tok_struct*_tmp77B=_cycalloc(sizeof(*_tmp77B));
_tmp77B[0]=({struct Cyc_FieldPattern_tok_struct _tmp77C;_tmp77C.tag=Cyc_FieldPattern_tok;
_tmp77C.f1=({struct _tuple11*_tmp77D=_cycalloc(sizeof(*_tmp77D));_tmp77D->f1=0;
_tmp77D->f2=Cyc_yyget_Pattern_tok(yyvs[yyvsp_offset]);_tmp77D;});_tmp77C;});
_tmp77B;});break;case 299: _LL2E9: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp77E=_cycalloc(sizeof(*_tmp77E));_tmp77E[0]=({struct Cyc_FieldPattern_tok_struct
_tmp77F;_tmp77F.tag=Cyc_FieldPattern_tok;_tmp77F.f1=({struct _tuple11*_tmp780=
_cycalloc(sizeof(*_tmp780));_tmp780->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp780->f2=Cyc_yyget_Pattern_tok(
yyvs[yyvsp_offset]);_tmp780;});_tmp77F;});_tmp77E;});break;case 300: _LL2EA: yyval=(
void*)({struct Cyc_FieldPatternList_tok_struct*_tmp781=_cycalloc(sizeof(*_tmp781));
_tmp781[0]=({struct Cyc_FieldPatternList_tok_struct _tmp782;_tmp782.tag=Cyc_FieldPatternList_tok;
_tmp782.f1=({struct Cyc_List_List*_tmp783=_cycalloc(sizeof(*_tmp783));_tmp783->hd=
Cyc_yyget_FieldPattern_tok(yyvs[yyvsp_offset]);_tmp783->tl=0;_tmp783;});_tmp782;});
_tmp781;});break;case 301: _LL2EB: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp784=_cycalloc(sizeof(*_tmp784));_tmp784[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp785;_tmp785.tag=Cyc_FieldPatternList_tok;_tmp785.f1=({struct Cyc_List_List*
_tmp786=_cycalloc(sizeof(*_tmp786));_tmp786->hd=Cyc_yyget_FieldPattern_tok(yyvs[
yyvsp_offset]);_tmp786->tl=Cyc_yyget_FieldPatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp786;});_tmp785;});
_tmp784;});break;case 302: _LL2EC: yyval=yyvs[yyvsp_offset];break;case 303: _LL2ED:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp787=_cycalloc(sizeof(*_tmp787));
_tmp787[0]=({struct Cyc_Exp_tok_struct _tmp788;_tmp788.tag=Cyc_Exp_tok;_tmp788.f1=
Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp788;});_tmp787;});break;case 304: _LL2EE: yyval=yyvs[
yyvsp_offset];break;case 305: _LL2EF: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp789=_cycalloc(sizeof(*_tmp789));_tmp789[0]=({struct Cyc_Exp_tok_struct _tmp78A;
_tmp78A.tag=Cyc_Exp_tok;_tmp78A.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp78A;});_tmp789;});break;case 306: _LL2F0: yyval=(
void*)({struct Cyc_Primopopt_tok_struct*_tmp78B=_cycalloc(sizeof(*_tmp78B));
_tmp78B[0]=({struct Cyc_Primopopt_tok_struct _tmp78C;_tmp78C.tag=Cyc_Primopopt_tok;
_tmp78C.f1=0;_tmp78C;});_tmp78B;});break;case 307: _LL2F1: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp78D=_cycalloc(sizeof(*_tmp78D));_tmp78D[0]=({struct Cyc_Primopopt_tok_struct
_tmp78E;_tmp78E.tag=Cyc_Primopopt_tok;_tmp78E.f1=({struct Cyc_Core_Opt*_tmp78F=
_cycalloc(sizeof(*_tmp78F));_tmp78F->v=(void*)((void*)1);_tmp78F;});_tmp78E;});
_tmp78D;});break;case 308: _LL2F2: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp790=_cycalloc(sizeof(*_tmp790));_tmp790[0]=({struct Cyc_Primopopt_tok_struct
_tmp791;_tmp791.tag=Cyc_Primopopt_tok;_tmp791.f1=({struct Cyc_Core_Opt*_tmp792=
_cycalloc(sizeof(*_tmp792));_tmp792->v=(void*)((void*)3);_tmp792;});_tmp791;});
_tmp790;});break;case 309: _LL2F3: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp793=_cycalloc(sizeof(*_tmp793));_tmp793[0]=({struct Cyc_Primopopt_tok_struct
_tmp794;_tmp794.tag=Cyc_Primopopt_tok;_tmp794.f1=({struct Cyc_Core_Opt*_tmp795=
_cycalloc(sizeof(*_tmp795));_tmp795->v=(void*)((void*)4);_tmp795;});_tmp794;});
_tmp793;});break;case 310: _LL2F4: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp796=_cycalloc(sizeof(*_tmp796));_tmp796[0]=({struct Cyc_Primopopt_tok_struct
_tmp797;_tmp797.tag=Cyc_Primopopt_tok;_tmp797.f1=({struct Cyc_Core_Opt*_tmp798=
_cycalloc(sizeof(*_tmp798));_tmp798->v=(void*)((void*)0);_tmp798;});_tmp797;});
_tmp796;});break;case 311: _LL2F5: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp799=_cycalloc(sizeof(*_tmp799));_tmp799[0]=({struct Cyc_Primopopt_tok_struct
_tmp79A;_tmp79A.tag=Cyc_Primopopt_tok;_tmp79A.f1=({struct Cyc_Core_Opt*_tmp79B=
_cycalloc(sizeof(*_tmp79B));_tmp79B->v=(void*)((void*)2);_tmp79B;});_tmp79A;});
_tmp799;});break;case 312: _LL2F6: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp79C=_cycalloc(sizeof(*_tmp79C));_tmp79C[0]=({struct Cyc_Primopopt_tok_struct
_tmp79D;_tmp79D.tag=Cyc_Primopopt_tok;_tmp79D.f1=({struct Cyc_Core_Opt*_tmp79E=
_cycalloc(sizeof(*_tmp79E));_tmp79E->v=(void*)((void*)16);_tmp79E;});_tmp79D;});
_tmp79C;});break;case 313: _LL2F7: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp79F=_cycalloc(sizeof(*_tmp79F));_tmp79F[0]=({struct Cyc_Primopopt_tok_struct
_tmp7A0;_tmp7A0.tag=Cyc_Primopopt_tok;_tmp7A0.f1=({struct Cyc_Core_Opt*_tmp7A1=
_cycalloc(sizeof(*_tmp7A1));_tmp7A1->v=(void*)((void*)17);_tmp7A1;});_tmp7A0;});
_tmp79F;});break;case 314: _LL2F8: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp7A2=_cycalloc(sizeof(*_tmp7A2));_tmp7A2[0]=({struct Cyc_Primopopt_tok_struct
_tmp7A3;_tmp7A3.tag=Cyc_Primopopt_tok;_tmp7A3.f1=({struct Cyc_Core_Opt*_tmp7A4=
_cycalloc(sizeof(*_tmp7A4));_tmp7A4->v=(void*)((void*)13);_tmp7A4;});_tmp7A3;});
_tmp7A2;});break;case 315: _LL2F9: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp7A5=_cycalloc(sizeof(*_tmp7A5));_tmp7A5[0]=({struct Cyc_Primopopt_tok_struct
_tmp7A6;_tmp7A6.tag=Cyc_Primopopt_tok;_tmp7A6.f1=({struct Cyc_Core_Opt*_tmp7A7=
_cycalloc(sizeof(*_tmp7A7));_tmp7A7->v=(void*)((void*)15);_tmp7A7;});_tmp7A6;});
_tmp7A5;});break;case 316: _LL2FA: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp7A8=_cycalloc(sizeof(*_tmp7A8));_tmp7A8[0]=({struct Cyc_Primopopt_tok_struct
_tmp7A9;_tmp7A9.tag=Cyc_Primopopt_tok;_tmp7A9.f1=({struct Cyc_Core_Opt*_tmp7AA=
_cycalloc(sizeof(*_tmp7AA));_tmp7AA->v=(void*)((void*)14);_tmp7AA;});_tmp7A9;});
_tmp7A8;});break;case 317: _LL2FB: yyval=yyvs[yyvsp_offset];break;case 318: _LL2FC:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7AB=_cycalloc(sizeof(*_tmp7AB));
_tmp7AB[0]=({struct Cyc_Exp_tok_struct _tmp7AC;_tmp7AC.tag=Cyc_Exp_tok;_tmp7AC.f1=
Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7AC;});_tmp7AB;});break;case 319: _LL2FD: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7AD=_cycalloc(sizeof(*_tmp7AD));_tmp7AD[0]=({
struct Cyc_Exp_tok_struct _tmp7AE;_tmp7AE.tag=Cyc_Exp_tok;_tmp7AE.f1=Cyc_Absyn_throw_exp(
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7AE;});_tmp7AD;});break;case 320: _LL2FE: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7AF=_cycalloc(sizeof(*_tmp7AF));_tmp7AF[0]=({
struct Cyc_Exp_tok_struct _tmp7B0;_tmp7B0.tag=Cyc_Exp_tok;_tmp7B0.f1=Cyc_Absyn_New_exp(
0,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line));_tmp7B0;});
_tmp7AF;});break;case 321: _LL2FF: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B1=
_cycalloc(sizeof(*_tmp7B1));_tmp7B1[0]=({struct Cyc_Exp_tok_struct _tmp7B2;_tmp7B2.tag=
Cyc_Exp_tok;_tmp7B2.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).last_line));_tmp7B2;});_tmp7B1;});break;case 322: _LL300: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7B3=_cycalloc(sizeof(*_tmp7B3));_tmp7B3[0]=({
struct Cyc_Exp_tok_struct _tmp7B4;_tmp7B4.tag=Cyc_Exp_tok;_tmp7B4.f1=Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7B4;});_tmp7B3;});break;case 323: _LL301: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7B5=_cycalloc(sizeof(*_tmp7B5));_tmp7B5[0]=({
struct Cyc_Exp_tok_struct _tmp7B6;_tmp7B6.tag=Cyc_Exp_tok;_tmp7B6.f1=Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7B6;});_tmp7B5;});break;case 324: _LL302: yyval=yyvs[
yyvsp_offset];break;case 325: _LL303: yyval=yyvs[yyvsp_offset];break;case 326: _LL304:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));
_tmp7B7[0]=({struct Cyc_Exp_tok_struct _tmp7B8;_tmp7B8.tag=Cyc_Exp_tok;_tmp7B8.f1=
Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7B8;});_tmp7B7;});break;case 327: _LL305: yyval=yyvs[
yyvsp_offset];break;case 328: _LL306: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7B9=_cycalloc(sizeof(*_tmp7B9));_tmp7B9[0]=({struct Cyc_Exp_tok_struct _tmp7BA;
_tmp7BA.tag=Cyc_Exp_tok;_tmp7BA.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7BA;});
_tmp7B9;});break;case 329: _LL307: yyval=yyvs[yyvsp_offset];break;case 330: _LL308:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7BB=_cycalloc(sizeof(*_tmp7BB));
_tmp7BB[0]=({struct Cyc_Exp_tok_struct _tmp7BC;_tmp7BC.tag=Cyc_Exp_tok;_tmp7BC.f1=
Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7BC;});
_tmp7BB;});break;case 331: _LL309: yyval=yyvs[yyvsp_offset];break;case 332: _LL30A:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7BD=_cycalloc(sizeof(*_tmp7BD));
_tmp7BD[0]=({struct Cyc_Exp_tok_struct _tmp7BE;_tmp7BE.tag=Cyc_Exp_tok;_tmp7BE.f1=
Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7BE;});
_tmp7BD;});break;case 333: _LL30B: yyval=yyvs[yyvsp_offset];break;case 334: _LL30C:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7BF=_cycalloc(sizeof(*_tmp7BF));
_tmp7BF[0]=({struct Cyc_Exp_tok_struct _tmp7C0;_tmp7C0.tag=Cyc_Exp_tok;_tmp7C0.f1=
Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7C0;});
_tmp7BF;});break;case 335: _LL30D: yyval=yyvs[yyvsp_offset];break;case 336: _LL30E:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7C1=_cycalloc(sizeof(*_tmp7C1));
_tmp7C1[0]=({struct Cyc_Exp_tok_struct _tmp7C2;_tmp7C2.tag=Cyc_Exp_tok;_tmp7C2.f1=
Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C2;});_tmp7C1;});break;case 337: _LL30F: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7C3=_cycalloc(sizeof(*_tmp7C3));_tmp7C3[0]=({
struct Cyc_Exp_tok_struct _tmp7C4;_tmp7C4.tag=Cyc_Exp_tok;_tmp7C4.f1=Cyc_Absyn_neq_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7C4;});_tmp7C3;});break;case 338: _LL310: yyval=yyvs[
yyvsp_offset];break;case 339: _LL311: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7C5=_cycalloc(sizeof(*_tmp7C5));_tmp7C5[0]=({struct Cyc_Exp_tok_struct _tmp7C6;
_tmp7C6.tag=Cyc_Exp_tok;_tmp7C6.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7C6;});
_tmp7C5;});break;case 340: _LL312: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7C7=
_cycalloc(sizeof(*_tmp7C7));_tmp7C7[0]=({struct Cyc_Exp_tok_struct _tmp7C8;_tmp7C8.tag=
Cyc_Exp_tok;_tmp7C8.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7C8;});
_tmp7C7;});break;case 341: _LL313: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7C9=
_cycalloc(sizeof(*_tmp7C9));_tmp7C9[0]=({struct Cyc_Exp_tok_struct _tmp7CA;_tmp7CA.tag=
Cyc_Exp_tok;_tmp7CA.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7CA;});
_tmp7C9;});break;case 342: _LL314: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7CB=
_cycalloc(sizeof(*_tmp7CB));_tmp7CB[0]=({struct Cyc_Exp_tok_struct _tmp7CC;_tmp7CC.tag=
Cyc_Exp_tok;_tmp7CC.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7CC;});
_tmp7CB;});break;case 343: _LL315: yyval=yyvs[yyvsp_offset];break;case 344: _LL316:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7CD=_cycalloc(sizeof(*_tmp7CD));
_tmp7CD[0]=({struct Cyc_Exp_tok_struct _tmp7CE;_tmp7CE.tag=Cyc_Exp_tok;_tmp7CE.f1=
Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7CE;});
_tmp7CD;});break;case 345: _LL317: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7CF=
_cycalloc(sizeof(*_tmp7CF));_tmp7CF[0]=({struct Cyc_Exp_tok_struct _tmp7D0;_tmp7D0.tag=
Cyc_Exp_tok;_tmp7D0.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7D0;});
_tmp7CF;});break;case 346: _LL318: yyval=yyvs[yyvsp_offset];break;case 347: _LL319:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7D1=_cycalloc(sizeof(*_tmp7D1));
_tmp7D1[0]=({struct Cyc_Exp_tok_struct _tmp7D2;_tmp7D2.tag=Cyc_Exp_tok;_tmp7D2.f1=
Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7D2;});
_tmp7D1;});break;case 348: _LL31A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7D3=
_cycalloc(sizeof(*_tmp7D3));_tmp7D3[0]=({struct Cyc_Exp_tok_struct _tmp7D4;_tmp7D4.tag=
Cyc_Exp_tok;_tmp7D4.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7D4;});
_tmp7D3;});break;case 349: _LL31B: yyval=yyvs[yyvsp_offset];break;case 350: _LL31C:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));
_tmp7D5[0]=({struct Cyc_Exp_tok_struct _tmp7D6;_tmp7D6.tag=Cyc_Exp_tok;_tmp7D6.f1=
Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7D6;});
_tmp7D5;});break;case 351: _LL31D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7D7=
_cycalloc(sizeof(*_tmp7D7));_tmp7D7[0]=({struct Cyc_Exp_tok_struct _tmp7D8;_tmp7D8.tag=
Cyc_Exp_tok;_tmp7D8.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7D8;});
_tmp7D7;});break;case 352: _LL31E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7D9=
_cycalloc(sizeof(*_tmp7D9));_tmp7D9[0]=({struct Cyc_Exp_tok_struct _tmp7DA;_tmp7DA.tag=
Cyc_Exp_tok;_tmp7DA.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7DA;});
_tmp7D9;});break;case 353: _LL31F: yyval=yyvs[yyvsp_offset];break;case 354: _LL320:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7DB=_cycalloc(sizeof(*_tmp7DB));
_tmp7DB[0]=({struct Cyc_Exp_tok_struct _tmp7DC;_tmp7DC.tag=Cyc_Exp_tok;_tmp7DC.f1=
Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7DC;});_tmp7DB;});break;case 355: _LL321: yyval=yyvs[
yyvsp_offset];break;case 356: _LL322: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp7DD=_cycalloc(sizeof(*_tmp7DD));_tmp7DD[0]=({struct Cyc_Exp_tok_struct _tmp7DE;
_tmp7DE.tag=Cyc_Exp_tok;_tmp7DE.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7DE;});
_tmp7DD;});break;case 357: _LL323: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7DF=
_cycalloc(sizeof(*_tmp7DF));_tmp7DF[0]=({struct Cyc_Exp_tok_struct _tmp7E0;_tmp7E0.tag=
Cyc_Exp_tok;_tmp7E0.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7E0;});
_tmp7DF;});break;case 358: _LL324: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7E1=
_cycalloc(sizeof(*_tmp7E1));_tmp7E1[0]=({struct Cyc_Exp_tok_struct _tmp7E2;_tmp7E2.tag=
Cyc_Exp_tok;_tmp7E2.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7E2;});
_tmp7E1;});break;case 359: _LL325: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7E3=
_cycalloc(sizeof(*_tmp7E3));_tmp7E3[0]=({struct Cyc_Exp_tok_struct _tmp7E4;_tmp7E4.tag=
Cyc_Exp_tok;_tmp7E4.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7E4;});
_tmp7E3;});break;case 360: _LL326: yyval=yyvs[yyvsp_offset];break;case 361: _LL327:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7E5=_cycalloc(sizeof(*_tmp7E5));
_tmp7E5[0]=({struct Cyc_Exp_tok_struct _tmp7E6;_tmp7E6.tag=Cyc_Exp_tok;_tmp7E6.f1=
Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7E6;});_tmp7E5;});break;case 362: _LL328: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7E7=_cycalloc(sizeof(*_tmp7E7));_tmp7E7[0]=({
struct Cyc_Exp_tok_struct _tmp7E8;_tmp7E8.tag=Cyc_Exp_tok;_tmp7E8.f1=Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)])).f3,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7E8;});
_tmp7E7;});break;case 363: _LL329: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7E9=
_cycalloc(sizeof(*_tmp7E9));_tmp7E9[0]=({struct Cyc_Exp_tok_struct _tmp7EA;_tmp7EA.tag=
Cyc_Exp_tok;_tmp7EA.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7EA;});
_tmp7E9;});break;case 364: _LL32A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp7EB=
_cycalloc(sizeof(*_tmp7EB));_tmp7EB[0]=({struct Cyc_Exp_tok_struct _tmp7EC;_tmp7EC.tag=
Cyc_Exp_tok;_tmp7EC.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp7ED=_cycalloc(sizeof(*_tmp7ED));_tmp7ED[0]=({struct Cyc_Absyn_StructField_struct
_tmp7EE;_tmp7EE.tag=0;_tmp7EE.f1=({struct _tagged_arr*_tmp7EF=_cycalloc(sizeof(*
_tmp7EF));_tmp7EF[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp7EF;});_tmp7EE;});_tmp7ED;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7EC;});_tmp7EB;});break;case 365: _LL32B: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7F0=_cycalloc(sizeof(*_tmp7F0));_tmp7F0[0]=({
struct Cyc_Exp_tok_struct _tmp7F1;_tmp7F1.tag=Cyc_Exp_tok;_tmp7F1.f1=Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp7F2=_cycalloc_atomic(
sizeof(*_tmp7F2));_tmp7F2[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp7F3;_tmp7F3.tag=
1;_tmp7F3.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp7F3;});_tmp7F2;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[yylsp_offset]).last_line));_tmp7F1;});
_tmp7F0;});break;case 366: _LL32C: {struct Cyc_Position_Segment*_tmp7F4=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line);struct Cyc_List_List*_tmp7F5=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp7F4,Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({
struct Cyc_Exp_tok_struct*_tmp7F6=_cycalloc(sizeof(*_tmp7F6));_tmp7F6[0]=({struct
Cyc_Exp_tok_struct _tmp7F7;_tmp7F7.tag=Cyc_Exp_tok;_tmp7F7.f1=Cyc_Absyn_gentyp_exp(
_tmp7F5,(*Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F7;});_tmp7F6;});break;}case 367: _LL32D: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7F8=_cycalloc(sizeof(*_tmp7F8));_tmp7F8[0]=({
struct Cyc_Exp_tok_struct _tmp7F9;_tmp7F9.tag=Cyc_Exp_tok;_tmp7F9.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7FA=_cycalloc(sizeof(*_tmp7FA));
_tmp7FA[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7FB;_tmp7FB.tag=33;_tmp7FB.f1=({
struct Cyc_Absyn_MallocInfo _tmp7FC;_tmp7FC.is_calloc=0;_tmp7FC.rgn=0;_tmp7FC.elt_type=
0;_tmp7FC.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp7FC.fat_result=0;_tmp7FC;});_tmp7FB;});_tmp7FA;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7F9;});_tmp7F8;});break;case 368: _LL32E: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp7FD=_cycalloc(sizeof(*_tmp7FD));_tmp7FD[0]=({
struct Cyc_Exp_tok_struct _tmp7FE;_tmp7FE.tag=Cyc_Exp_tok;_tmp7FE.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7FF=_cycalloc(sizeof(*_tmp7FF));
_tmp7FF[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp800;_tmp800.tag=33;_tmp800.f1=({
struct Cyc_Absyn_MallocInfo _tmp801;_tmp801.is_calloc=0;_tmp801.rgn=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp801.elt_type=0;_tmp801.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp801.fat_result=0;
_tmp801;});_tmp800;});_tmp7FF;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp7FE;});_tmp7FD;});break;case 369: _LL32F: {void*
_tmp803;struct _tuple2 _tmp802=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp803=_tmp802.f3;yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp804=_cycalloc(sizeof(*_tmp804));_tmp804[0]=({
struct Cyc_Exp_tok_struct _tmp805;_tmp805.tag=Cyc_Exp_tok;_tmp805.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp806=_cycalloc(sizeof(*_tmp806));
_tmp806[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp807;_tmp807.tag=33;_tmp807.f1=({
struct Cyc_Absyn_MallocInfo _tmp808;_tmp808.is_calloc=1;_tmp808.rgn=0;_tmp808.elt_type=({
void**_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809[0]=_tmp803;_tmp809;});_tmp808.num_elts=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp808.fat_result=0;_tmp808;});_tmp807;});_tmp806;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp805;});_tmp804;});break;}case 370: _LL330: {void*
_tmp80B;struct _tuple2 _tmp80A=*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp80B=_tmp80A.f3;yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp80C=_cycalloc(sizeof(*_tmp80C));_tmp80C[0]=({
struct Cyc_Exp_tok_struct _tmp80D;_tmp80D.tag=Cyc_Exp_tok;_tmp80D.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp80E=_cycalloc(sizeof(*_tmp80E));
_tmp80E[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp80F;_tmp80F.tag=33;_tmp80F.f1=({
struct Cyc_Absyn_MallocInfo _tmp810;_tmp810.is_calloc=1;_tmp810.rgn=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);
_tmp810.elt_type=({void**_tmp811=_cycalloc(sizeof(*_tmp811));_tmp811[0]=_tmp80B;
_tmp811;});_tmp810.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp810.fat_result=0;
_tmp810;});_tmp80F;});_tmp80E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp80D;});_tmp80C;});break;}case 371: _LL331: yyval=(
void*)({struct Cyc_Primop_tok_struct*_tmp812=_cycalloc(sizeof(*_tmp812));_tmp812[
0]=({struct Cyc_Primop_tok_struct _tmp813;_tmp813.tag=Cyc_Primop_tok;_tmp813.f1=(
void*)((void*)12);_tmp813;});_tmp812;});break;case 372: _LL332: yyval=(void*)({
struct Cyc_Primop_tok_struct*_tmp814=_cycalloc(sizeof(*_tmp814));_tmp814[0]=({
struct Cyc_Primop_tok_struct _tmp815;_tmp815.tag=Cyc_Primop_tok;_tmp815.f1=(void*)((
void*)11);_tmp815;});_tmp814;});break;case 373: _LL333: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816[0]=({struct Cyc_Primop_tok_struct
_tmp817;_tmp817.tag=Cyc_Primop_tok;_tmp817.f1=(void*)((void*)2);_tmp817;});
_tmp816;});break;case 374: _LL334: yyval=yyvs[yyvsp_offset];break;case 375: _LL335:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp818=_cycalloc(sizeof(*_tmp818));
_tmp818[0]=({struct Cyc_Exp_tok_struct _tmp819;_tmp819.tag=Cyc_Exp_tok;_tmp819.f1=
Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp819;});_tmp818;});break;case 376: _LL336: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp81A=_cycalloc(sizeof(*_tmp81A));_tmp81A[0]=({
struct Cyc_Exp_tok_struct _tmp81B;_tmp81B.tag=Cyc_Exp_tok;_tmp81B.f1=Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[yylsp_offset]).last_line));_tmp81B;});
_tmp81A;});break;case 377: _LL337: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp81C=
_cycalloc(sizeof(*_tmp81C));_tmp81C[0]=({struct Cyc_Exp_tok_struct _tmp81D;_tmp81D.tag=
Cyc_Exp_tok;_tmp81D.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp81D;});_tmp81C;});break;case 378: _LL338: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E[0]=({
struct Cyc_Exp_tok_struct _tmp81F;_tmp81F.tag=Cyc_Exp_tok;_tmp81F.f1=Cyc_Absyn_aggrmember_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
struct _tagged_arr*_tmp820=_cycalloc(sizeof(*_tmp820));_tmp820[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp820;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp81F;});_tmp81E;});break;case 379: _LL339: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp821=_cycalloc(sizeof(*_tmp821));_tmp821[0]=({
struct Cyc_Exp_tok_struct _tmp822;_tmp822.tag=Cyc_Exp_tok;_tmp822.f1=Cyc_Absyn_aggrarrow_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
struct _tagged_arr*_tmp823=_cycalloc(sizeof(*_tmp823));_tmp823[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp823;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp822;});_tmp821;});break;case 380: _LL33A: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp824=_cycalloc(sizeof(*_tmp824));_tmp824[0]=({
struct Cyc_Exp_tok_struct _tmp825;_tmp825.tag=Cyc_Exp_tok;_tmp825.f1=Cyc_Absyn_post_inc_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[yylsp_offset]).last_line));_tmp825;});
_tmp824;});break;case 381: _LL33B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp826=
_cycalloc(sizeof(*_tmp826));_tmp826[0]=({struct Cyc_Exp_tok_struct _tmp827;_tmp827.tag=
Cyc_Exp_tok;_tmp827.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp827;});_tmp826;});break;case 382: _LL33C: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp828=_cycalloc(sizeof(*_tmp828));_tmp828[0]=({
struct Cyc_Exp_tok_struct _tmp829;_tmp829.tag=Cyc_Exp_tok;_tmp829.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp82A=_cycalloc(sizeof(*_tmp82A));
_tmp82A[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp82B;_tmp82B.tag=25;_tmp82B.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
4)]);_tmp82B.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp82B;});_tmp82A;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp829;});_tmp828;});break;case 383: _LL33D: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp82C=_cycalloc(sizeof(*_tmp82C));_tmp82C[0]=({
struct Cyc_Exp_tok_struct _tmp82D;_tmp82D.tag=Cyc_Exp_tok;_tmp82D.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp82E=_cycalloc(sizeof(*_tmp82E));
_tmp82E[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp82F;_tmp82F.tag=25;_tmp82F.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmp82F.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp82F;});_tmp82E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp82D;});_tmp82C;});break;case 384: _LL33E: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp830=_cycalloc(sizeof(*_tmp830));_tmp830[0]=({
struct Cyc_Exp_tok_struct _tmp831;_tmp831.tag=Cyc_Exp_tok;_tmp831.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Fill_e_struct*_tmp832=_cycalloc(sizeof(*_tmp832));
_tmp832[0]=({struct Cyc_Absyn_Fill_e_struct _tmp833;_tmp833.tag=37;_tmp833.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp833;});_tmp832;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp831;});
_tmp830;});break;case 385: _LL33F: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp834=
_cycalloc(sizeof(*_tmp834));_tmp834[0]=({struct Cyc_Exp_tok_struct _tmp835;_tmp835.tag=
Cyc_Exp_tok;_tmp835.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Codegen_e_struct*
_tmp836=_cycalloc(sizeof(*_tmp836));_tmp836[0]=({struct Cyc_Absyn_Codegen_e_struct
_tmp837;_tmp837.tag=36;_tmp837.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp837;});_tmp836;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp835;});
_tmp834;});break;case 386: _LL340: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp838=
_cycalloc(sizeof(*_tmp838));_tmp838[0]=({struct Cyc_Exp_tok_struct _tmp839;_tmp839.tag=
Cyc_Exp_tok;_tmp839.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp83A=_cycalloc(sizeof(*_tmp83A));_tmp83A[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp83B;_tmp83B.tag=2;_tmp83B.f1=Cyc_yyget_QualId_tok(yyvs[yyvsp_offset]);
_tmp83B;});_tmp83A;}),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp839;});_tmp838;});break;case 387: _LL341: yyval=
yyvs[yyvsp_offset];break;case 388: _LL342: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp83C=_cycalloc(sizeof(*_tmp83C));_tmp83C[0]=({struct Cyc_Exp_tok_struct _tmp83D;
_tmp83D.tag=Cyc_Exp_tok;_tmp83D.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(
yyls[yylsp_offset]).last_line));_tmp83D;});_tmp83C;});break;case 389: _LL343: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 390: _LL344:
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp83E=_cycalloc(sizeof(*_tmp83E));
_tmp83E[0]=({struct Cyc_Exp_tok_struct _tmp83F;_tmp83F.tag=Cyc_Exp_tok;_tmp83F.f1=
Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp83F;});_tmp83E;});break;case 391: _LL345: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp840=_cycalloc(sizeof(*_tmp840));_tmp840[0]=({
struct Cyc_Exp_tok_struct _tmp841;_tmp841.tag=Cyc_Exp_tok;_tmp841.f1=Cyc_Absyn_instantiate_exp(
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp841;});_tmp840;});break;case 392: _LL346: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp842=_cycalloc(sizeof(*_tmp842));_tmp842[0]=({
struct Cyc_Exp_tok_struct _tmp843;_tmp843.tag=Cyc_Exp_tok;_tmp843.f1=Cyc_Absyn_tuple_exp(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[yylsp_offset]).last_line));_tmp843;});
_tmp842;});break;case 393: _LL347: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp844=
_cycalloc(sizeof(*_tmp844));_tmp844[0]=({struct Cyc_Exp_tok_struct _tmp845;_tmp845.tag=
Cyc_Exp_tok;_tmp845.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp846=_cycalloc(sizeof(*_tmp846));_tmp846[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp847;_tmp847.tag=28;_tmp847.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp847.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp847.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp847.f4=0;
_tmp847;});_tmp846;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
yylsp_offset]).last_line));_tmp845;});_tmp844;});break;case 394: _LL348: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp848=_cycalloc(sizeof(*_tmp848));_tmp848[0]=({
struct Cyc_Exp_tok_struct _tmp849;_tmp849.tag=Cyc_Exp_tok;_tmp849.f1=Cyc_Absyn_stmt_exp(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[yylsp_offset]).last_line));_tmp849;});
_tmp848;});break;case 395: _LL349: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp84A=_cycalloc(sizeof(*_tmp84A));_tmp84A[0]=({struct Cyc_ExpList_tok_struct
_tmp84B;_tmp84B.tag=Cyc_ExpList_tok;_tmp84B.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(yyvs[yyvsp_offset]));
_tmp84B;});_tmp84A;});break;case 396: _LL34A: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmp84C=_cycalloc(sizeof(*_tmp84C));_tmp84C[0]=({struct Cyc_ExpList_tok_struct
_tmp84D;_tmp84D.tag=Cyc_ExpList_tok;_tmp84D.f1=({struct Cyc_List_List*_tmp84E=
_cycalloc(sizeof(*_tmp84E));_tmp84E->hd=Cyc_yyget_Exp_tok(yyvs[yyvsp_offset]);
_tmp84E->tl=0;_tmp84E;});_tmp84D;});_tmp84C;});break;case 397: _LL34B: yyval=(void*)({
struct Cyc_ExpList_tok_struct*_tmp84F=_cycalloc(sizeof(*_tmp84F));_tmp84F[0]=({
struct Cyc_ExpList_tok_struct _tmp850;_tmp850.tag=Cyc_ExpList_tok;_tmp850.f1=({
struct Cyc_List_List*_tmp851=_cycalloc(sizeof(*_tmp851));_tmp851->hd=Cyc_yyget_Exp_tok(
yyvs[yyvsp_offset]);_tmp851->tl=Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp851;});_tmp850;});
_tmp84F;});break;case 398: _LL34C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp852=
_cycalloc(sizeof(*_tmp852));_tmp852[0]=({struct Cyc_Exp_tok_struct _tmp853;_tmp853.tag=
Cyc_Exp_tok;_tmp853.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f1,(*
Cyc_yyget_Int_tok(yyvs[yyvsp_offset])).f2,Cyc_Position_segment_of_abs((yyls[
yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp853;});_tmp852;});
break;case 399: _LL34D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp854=_cycalloc(
sizeof(*_tmp854));_tmp854[0]=({struct Cyc_Exp_tok_struct _tmp855;_tmp855.tag=Cyc_Exp_tok;
_tmp855.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[yyvsp_offset]),Cyc_Position_segment_of_abs((
yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));_tmp855;});
_tmp854;});break;case 400: _LL34E: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp856=
_cycalloc(sizeof(*_tmp856));_tmp856[0]=({struct Cyc_Exp_tok_struct _tmp857;_tmp857.tag=
Cyc_Exp_tok;_tmp857.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
yyvsp_offset]),Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[
yylsp_offset]).last_line));_tmp857;});_tmp856;});break;case 401: _LL34F: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp858=_cycalloc(sizeof(*_tmp858));_tmp858[0]=({
struct Cyc_Exp_tok_struct _tmp859;_tmp859.tag=Cyc_Exp_tok;_tmp859.f1=Cyc_Absyn_null_exp(
Cyc_Position_segment_of_abs((yyls[yylsp_offset]).first_line,(yyls[yylsp_offset]).last_line));
_tmp859;});_tmp858;});break;case 402: _LL350: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp85A=_cycalloc(sizeof(*_tmp85A));_tmp85A[0]=({struct Cyc_QualId_tok_struct
_tmp85B;_tmp85B.tag=Cyc_QualId_tok;_tmp85B.f1=({struct _tuple1*_tmp85C=_cycalloc(
sizeof(*_tmp85C));_tmp85C->f1=Cyc_Absyn_rel_ns_null;_tmp85C->f2=({struct
_tagged_arr*_tmp85D=_cycalloc(sizeof(*_tmp85D));_tmp85D[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp85D;});_tmp85C;});_tmp85B;});_tmp85A;});break;case 403:
_LL351: yyval=yyvs[yyvsp_offset];break;case 404: _LL352: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmp85E=_cycalloc(sizeof(*_tmp85E));_tmp85E[0]=({struct Cyc_QualId_tok_struct
_tmp85F;_tmp85F.tag=Cyc_QualId_tok;_tmp85F.f1=({struct _tuple1*_tmp860=_cycalloc(
sizeof(*_tmp860));_tmp860->f1=Cyc_Absyn_rel_ns_null;_tmp860->f2=({struct
_tagged_arr*_tmp861=_cycalloc(sizeof(*_tmp861));_tmp861[0]=Cyc_yyget_String_tok(
yyvs[yyvsp_offset]);_tmp861;});_tmp860;});_tmp85F;});_tmp85E;});break;case 405:
_LL353: yyval=yyvs[yyvsp_offset];break;case 406: _LL354: yyval=yyvs[yyvsp_offset];
break;case 407: _LL355: yyval=yyvs[yyvsp_offset];break;case 408: _LL356: yyval=yyvs[
yyvsp_offset];break;case 409: _LL357: yyval=yyvs[yyvsp_offset];break;case 410: _LL358:
break;case 411: _LL359:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;break;default: _LL35A: break;}
yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;yylsp_offset ++;if(
yylen == 0){(yyls[yylsp_offset]).first_line=Cyc_yylloc.first_line;(yyls[
yylsp_offset]).first_column=Cyc_yylloc.first_column;(yyls[yylsp_offset]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
yylsp_offset]).last_column=(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_column;}else{(yyls[yylsp_offset]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
yylsp_offset]).last_column=(yyls[_check_known_subscript_notnull(10000,(
yylsp_offset + yylen)- 1)]).last_column;}yyn=(int)Cyc_yyr1[yyn];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(114,yyn - 129)]+ yyss[yyssp_offset];if((yystate >= 
0?yystate <= 4971: 0)?Cyc_yycheck[yystate]== yyss[yyssp_offset]: 0)yystate=(int)Cyc_yytable[
yystate];else{yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(114,yyn - 
129)];}goto yynewstate;yyerrlab: if(yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
yystate];if(yyn > - 32768?yyn < 4971: 0){int sze=0;struct _tagged_arr msg;int x;int count;
count=0;for(x=yyn < 0?- yyn: 0;x < 243 / sizeof(char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(4972,x + yyn)]== x)(sze +=Cyc_Std_strlen(Cyc_yytname[
x])+ 15,count ++);}msg=({unsigned int _tmp862=(unsigned int)(sze + 15);char*_tmp863=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp862));struct _tagged_arr
_tmp865=_tag_arr(_tmp863,sizeof(char),(unsigned int)(sze + 15));{unsigned int
_tmp864=_tmp862;unsigned int i;for(i=0;i < _tmp864;i ++){_tmp863[i]='\000';}}
_tmp865;});Cyc_Std_strcpy(msg,_tag_arr("parse error",sizeof(char),12));if(count < 
5){count=0;for(x=yyn < 0?- yyn: 0;x < 243 / sizeof(char*);x ++){if(Cyc_yycheck[
_check_known_subscript_notnull(4972,x + yyn)]== x){Cyc_Std_strcat(msg,count == 0?
_tag_arr(", expecting `",sizeof(char),14): _tag_arr(" or `",sizeof(char),6));Cyc_Std_strcat(
msg,Cyc_yytname[x]);Cyc_Std_strcat(msg,_tag_arr("'",sizeof(char),2));count ++;}}}
Cyc_yyerror((struct _tagged_arr)msg);}else{Cyc_yyerror(_tag_arr("parse error",
sizeof(char),12));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 
0)return 1;Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0)return 1;yyvsp_offset --;yystate=(int)yyss[
_check_known_subscript_notnull(10000,-- yyssp_offset)];yylsp_offset --;yyerrhandle:
yyn=(int)Cyc_yypact[yystate];if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0?
1: yyn > 4971)?1: Cyc_yycheck[yyn]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){if(yyn == - 32768)goto yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)
goto yyerrpop;}if(yyn == 847)return 0;yyvs[_check_known_subscript_notnull(10000,++
yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;goto yynewstate;}void Cyc_yyprint(int i,void*v){void*
_tmp86F=v;struct _tuple13*_tmp870;struct _tuple13 _tmp871;int _tmp872;char _tmp873;
short _tmp874;struct _tagged_arr _tmp875;struct Cyc_Core_Opt*_tmp876;struct Cyc_Core_Opt*
_tmp877;struct Cyc_Core_Opt _tmp878;struct _tagged_arr*_tmp879;struct _tuple1*
_tmp87A;struct _tuple1 _tmp87B;void*_tmp87C;struct _tagged_arr*_tmp87D;_LL35D: if(*((
void**)_tmp86F)!= Cyc_Int_tok)goto _LL35F;_tmp870=((struct Cyc_Int_tok_struct*)
_tmp86F)->f1;_tmp871=*_tmp870;_tmp872=_tmp871.f2;_LL35E:({struct Cyc_Std_Int_pa_struct
_tmp87F;_tmp87F.tag=1;_tmp87F.f1=(int)((unsigned int)_tmp872);{void*_tmp87E[1]={&
_tmp87F};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%d",sizeof(char),3),_tag_arr(
_tmp87E,sizeof(void*),1));}});goto _LL35C;_LL35F: if(*((void**)_tmp86F)!= Cyc_Char_tok)
goto _LL361;_tmp873=((struct Cyc_Char_tok_struct*)_tmp86F)->f1;_LL360:({struct Cyc_Std_Int_pa_struct
_tmp881;_tmp881.tag=1;_tmp881.f1=(int)((unsigned int)((int)_tmp873));{void*
_tmp880[1]={& _tmp881};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%c",sizeof(char),
3),_tag_arr(_tmp880,sizeof(void*),1));}});goto _LL35C;_LL361: if(*((void**)_tmp86F)
!= Cyc_Short_tok)goto _LL363;_tmp874=((struct Cyc_Short_tok_struct*)_tmp86F)->f1;
_LL362:({struct Cyc_Std_Int_pa_struct _tmp883;_tmp883.tag=1;_tmp883.f1=(int)((
unsigned int)((int)_tmp874));{void*_tmp882[1]={& _tmp883};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%ds",sizeof(char),4),_tag_arr(_tmp882,sizeof(void*),1));}});goto _LL35C;
_LL363: if(*((void**)_tmp86F)!= Cyc_String_tok)goto _LL365;_tmp875=((struct Cyc_String_tok_struct*)
_tmp86F)->f1;_LL364:({struct Cyc_Std_String_pa_struct _tmp885;_tmp885.tag=0;
_tmp885.f1=(struct _tagged_arr)_tmp875;{void*_tmp884[1]={& _tmp885};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("\"%s\"",sizeof(char),5),_tag_arr(_tmp884,sizeof(void*),
1));}});goto _LL35C;_LL365: if(*((void**)_tmp86F)!= Cyc_Stringopt_tok)goto _LL367;
_tmp876=((struct Cyc_Stringopt_tok_struct*)_tmp86F)->f1;if(_tmp876 != 0)goto _LL367;
_LL366:({void*_tmp886[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("NULL",
sizeof(char),5),_tag_arr(_tmp886,sizeof(void*),0));});goto _LL35C;_LL367: if(*((
void**)_tmp86F)!= Cyc_Stringopt_tok)goto _LL369;_tmp877=((struct Cyc_Stringopt_tok_struct*)
_tmp86F)->f1;if(_tmp877 == 0)goto _LL369;_tmp878=*_tmp877;_tmp879=(struct
_tagged_arr*)_tmp878.v;_LL368:({struct Cyc_Std_String_pa_struct _tmp888;_tmp888.tag=
0;_tmp888.f1=(struct _tagged_arr)*_tmp879;{void*_tmp887[1]={& _tmp888};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("\"%s\"",sizeof(char),5),_tag_arr(_tmp887,sizeof(void*),
1));}});goto _LL35C;_LL369: if(*((void**)_tmp86F)!= Cyc_QualId_tok)goto _LL36B;
_tmp87A=((struct Cyc_QualId_tok_struct*)_tmp86F)->f1;_tmp87B=*_tmp87A;_tmp87C=
_tmp87B.f1;_tmp87D=_tmp87B.f2;_LL36A: {struct Cyc_List_List*_tmp889=0;{void*
_tmp88A=_tmp87C;struct Cyc_List_List*_tmp88B;struct Cyc_List_List*_tmp88C;_LL36E:
if(_tmp88A <= (void*)1?1:*((int*)_tmp88A)!= 0)goto _LL370;_tmp88B=((struct Cyc_Absyn_Rel_n_struct*)
_tmp88A)->f1;_LL36F: _tmp889=_tmp88B;goto _LL36D;_LL370: if(_tmp88A <= (void*)1?1:*((
int*)_tmp88A)!= 1)goto _LL372;_tmp88C=((struct Cyc_Absyn_Abs_n_struct*)_tmp88A)->f1;
_LL371: _tmp889=_tmp88C;goto _LL36D;_LL372: if((int)_tmp88A != 0)goto _LL36D;_LL373:
goto _LL36D;_LL36D:;}for(0;_tmp889 != 0;_tmp889=_tmp889->tl){({struct Cyc_Std_String_pa_struct
_tmp88E;_tmp88E.tag=0;_tmp88E.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp889->hd);{void*_tmp88D[1]={& _tmp88E};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",
sizeof(char),5),_tag_arr(_tmp88D,sizeof(void*),1));}});}({struct Cyc_Std_String_pa_struct
_tmp890;_tmp890.tag=0;_tmp890.f1=(struct _tagged_arr)*_tmp87D;{void*_tmp88F[1]={&
_tmp890};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::",sizeof(char),5),_tag_arr(
_tmp88F,sizeof(void*),1));}});goto _LL35C;}_LL36B:;_LL36C:({void*_tmp891[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("?",sizeof(char),2),_tag_arr(_tmp891,
sizeof(void*),0));});goto _LL35C;_LL35C:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc_Std___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*
_tmp892=_cycalloc(sizeof(*_tmp892));_tmp892->v=Cyc_Lexing_from_file(f);_tmp892;});
Cyc_yyparse();return Cyc_Parse_parse_result;}
