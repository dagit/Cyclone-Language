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
 struct Cyc_Core_Opt{void*v;};void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};int isalnum(
int);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_list(struct _tagged_arr);int Cyc_List_length(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void*Cyc_List_fold_left(void*(*f)(void*,
void*),void*accum,struct Cyc_List_List*x);void*Cyc_List_fold_right_c(void*(*f)(
void*,void*,void*),void*,struct Cyc_List_List*x,void*accum);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_flatten(struct Cyc_List_List*x);extern char Cyc_List_Nth[8];int Cyc_List_forall(
int(*pred)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct
Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_zip3(struct
Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);struct _tuple0{
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_split(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tabulate(int n,void*(*f)(int));
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct
Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct
Cyc_Iter_Iter,void*);struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;
extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[18];struct
Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};int Cyc_Std_printf(
struct _tagged_arr fmt,struct _tagged_arr);struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_Dict_Dict;
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*
Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*k);
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict*d);struct _tuple1{
void*f1;void*f2;};struct _tuple1*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d);struct _tuple1*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
struct Cyc_Dict_Dict*Cyc_Dict_delete(struct Cyc_Dict_Dict*,void*);unsigned int Cyc_Std_strlen(
struct _tagged_arr s);int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);
struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);struct
_tagged_arr Cyc_Std_strdup(struct _tagged_arr src);struct Cyc_Hashtable_Table;struct
Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(
void*));void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*
Cyc_Position_segment_of_abs(int,int);struct Cyc_Position_Error{struct _tagged_arr
source;struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
char Cyc_Position_Nocontext[14];struct Cyc_Typerep_Int_struct{int tag;int f1;
unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*f2;
};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple2{unsigned int f1;
struct _tagged_arr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int tag;struct
_tagged_arr*f1;unsigned int f2;struct _tagged_arr f3;};struct _tuple3{unsigned int f1;
void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;struct _tagged_arr
f2;};struct _tuple4{unsigned int f1;struct _tagged_arr f2;};struct Cyc_Typerep_TUnion_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;struct _tagged_arr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;unsigned int f3;struct _tagged_arr
f4;};struct _tuple5{struct _tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);struct Cyc_Std_tm{int tm_sec;int tm_min;int tm_hour;int tm_mday;int tm_mon;
int tm_year;int tm_wday;int tm_yday;int tm_isdst;};struct Cyc_Absyn_Rel_n_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple6{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
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
_tuple6*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple6*
tunion_name;struct _tuple6*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple6*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
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
_tuple6*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};
struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple6*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
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
tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple6*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple6*f1;};struct Cyc_Absyn_Primop_e_struct{
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
int tag;struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple7*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple6*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple6*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple6*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Codegen_e_struct{int tag;struct
Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Fill_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct _tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;struct _tuple8 condition;
struct _tuple8 delta;struct Cyc_Absyn_Stmt*body;};struct Cyc_Absyn_Exp_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple8
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct
Cyc_Absyn_Goto_s_struct{int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple8 f2;
struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_SwitchC_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Cut_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Splice_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Label_s_struct{int tag;struct _tagged_arr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct _tuple8 f2;};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple6*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple6*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Pat{void*r;
struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*cnst_exp;struct Cyc_Absyn_Stmt*body;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple6*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple6*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Aggrfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;};struct Cyc_Absyn_Aggrdecl{void*kind;void*sc;struct _tuple6*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple6*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple6*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple6*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple6*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple6*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _tagged_arr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple6*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{
void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(struct
_tuple6*,struct _tuple6*);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;void*Cyc_Absyn_const_string_typ(
void*rgn);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);void*
Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_tagged_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple6*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple6*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
void*info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct
_tagged_arr Cyc_Absynpp_typ2cstring(void*);unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,
struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*
rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*
po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr
fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_typecmp(
void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcgenrep_RepInfo{
struct Cyc_List_List*decls;struct Cyc_Absyn_Exp*exp;struct Cyc_List_List*
dependencies;struct Cyc_Core_Opt*fwd_decl;int emitted;int is_extern;};struct Cyc_Dict_Dict*
Cyc_Tcgenrep_empty_typerep_dict(){return((struct Cyc_Dict_Dict*(*)(int(*cmp)(void*,
void*)))Cyc_Dict_empty)(Cyc_Tcutil_typecmp);}void Cyc_Tcgenrep_print_dict_entry(
void*type,struct Cyc_Tcgenrep_RepInfo*info){({struct Cyc_Std_Int_pa_struct _tmp3;
_tmp3.tag=1;_tmp3.f1=(int)((unsigned int)info->emitted);{struct Cyc_Std_Int_pa_struct
_tmp2;_tmp2.tag=1;_tmp2.f1=(unsigned int)info;{struct Cyc_Std_String_pa_struct
_tmp1;_tmp1.tag=0;_tmp1.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(type);{void*
_tmp0[3]={& _tmp1,& _tmp2,& _tmp3};Cyc_Std_printf(_tag_arr("(%s,%x:%d,",sizeof(char),
11),_tag_arr(_tmp0,sizeof(void*),3));}}}});{struct Cyc_List_List*p=info->dependencies;
for(0;p != 0;p=p->tl){({struct Cyc_Std_Int_pa_struct _tmp5;_tmp5.tag=1;_tmp5.f1=(
unsigned int)((struct Cyc_Tcgenrep_RepInfo*)p->hd);{void*_tmp4[1]={& _tmp5};Cyc_Std_printf(
_tag_arr("%x",sizeof(char),3),_tag_arr(_tmp4,sizeof(void*),1));}});if(p->tl != 0)({
void*_tmp6[0]={};Cyc_Std_printf(_tag_arr(",",sizeof(char),2),_tag_arr(_tmp6,
sizeof(void*),0));});}}({void*_tmp7[0]={};Cyc_Std_printf(_tag_arr(")\n",sizeof(
char),3),_tag_arr(_tmp7,sizeof(void*),0));});}void Cyc_Tcgenrep_print_typerep_dict(
struct Cyc_Dict_Dict*dict){((void(*)(void(*f)(void*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Dict_Dict*d))Cyc_Dict_iter)(Cyc_Tcgenrep_print_dict_entry,dict);}
static int Cyc_Tcgenrep_rephash(struct Cyc_Tcgenrep_RepInfo*ri){return(int)ri;}
static int Cyc_Tcgenrep_repcmp(struct Cyc_Tcgenrep_RepInfo*r1,struct Cyc_Tcgenrep_RepInfo*
r2){unsigned int r1p;unsigned int r2p;r1p=(unsigned int)r1;r2p=(unsigned int)r2;
if(r1 < r2)return - 1;else{if(r1 == r2)return 0;else{return 1;}}}static struct Cyc_Absyn_Tunionfield*
Cyc_Tcgenrep_getField(struct Cyc_Absyn_Tuniondecl*td,struct _tuple6*fieldname){if(
td->fields == 0)({void*_tmp8[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Could not find field in tuniondecl",
sizeof(char),35),_tag_arr(_tmp8,sizeof(void*),0));});else{struct Cyc_List_List*l=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;l != 
0;l=l->tl){if(!Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Tunionfield*)l->hd)->name,
fieldname))return(struct Cyc_Absyn_Tunionfield*)l->hd;}}({void*_tmp9[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Could not find field in tuniondecl",
sizeof(char),35),_tag_arr(_tmp9,sizeof(void*),0));});}static char _tmpA[9]="Tcgenrep";
static struct _tagged_arr Cyc_Tcgenrep_tcgenrep_nm={_tmpA,_tmpA,_tmpA + 9};static
struct Cyc_List_List Cyc_Tcgenrep_l={(void*)& Cyc_Tcgenrep_tcgenrep_nm,0};static
struct Cyc_Absyn_Abs_n_struct Cyc_Tcgenrep_tcgenrep_nmspace={1,(struct Cyc_List_List*)&
Cyc_Tcgenrep_l};static void*Cyc_Tcgenrep_tcgenrep_nmspace2=(void*)& Cyc_Tcgenrep_tcgenrep_nmspace;
static char _tmpC[8]="Typerep";static struct _tagged_arr Cyc_Tcgenrep_typerep_nm={
_tmpC,_tmpC,_tmpC + 8};static struct Cyc_List_List Cyc_Tcgenrep_l2={(void*)& Cyc_Tcgenrep_typerep_nm,
0};static struct Cyc_Absyn_Abs_n_struct Cyc_Tcgenrep_typerep_nmspace={1,(struct Cyc_List_List*)&
Cyc_Tcgenrep_l2};static struct Cyc_List_List*Cyc_Tcgenrep_dfsvisit(struct Cyc_Tcgenrep_RepInfo*
ri,struct Cyc_Hashtable_Table*visited){struct _handler_cons _tmpE;_push_handler(&
_tmpE);{int _tmp10=0;if(setjmp(_tmpE.handler))_tmp10=1;if(!_tmp10){((int(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Tcgenrep_RepInfo*key))Cyc_Hashtable_lookup)(
visited,ri);{struct Cyc_List_List*_tmp11=0;_npop_handler(0);return _tmp11;};
_pop_handler();}else{void*_tmpF=(void*)_exn_thrown;void*_tmp13=_tmpF;_LL1: if(
_tmp13 != Cyc_Core_Not_found)goto _LL3;_LL2:((void(*)(struct Cyc_Hashtable_Table*t,
struct Cyc_Tcgenrep_RepInfo*key,int val))Cyc_Hashtable_insert)(visited,ri,1);{
struct Cyc_List_List*ds=0;{struct Cyc_List_List*l=ri->dependencies;for(0;l != 0;l=l->tl){
ds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
ds,Cyc_Tcgenrep_dfsvisit((struct Cyc_Tcgenrep_RepInfo*)l->hd,visited));}}if(ri->emitted
== 0){if(ri->fwd_decl != 0)ds=({struct Cyc_List_List*_tmp14=_cycalloc(sizeof(*
_tmp14));_tmp14->hd=({struct Cyc_Absyn_Decl*_tmp15[1];_tmp15[0]=(struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*)_check_null(ri->fwd_decl))->v;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp15,sizeof(struct Cyc_Absyn_Decl*),
1));});_tmp14->tl=ds;_tmp14;});{struct Cyc_List_List*_tmp16=({struct Cyc_List_List*
_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->hd=ri->decls;_tmp17->tl=0;_tmp17;});ri->emitted=
1;return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(ds,_tmp16);}}else{return ds;}}_LL3:;_LL4:(void)_throw(_tmp13);
_LL0:;}}}static struct Cyc_List_List*Cyc_Tcgenrep_dfs(struct Cyc_Tcgenrep_RepInfo*
ri){struct Cyc_Hashtable_Table*tab=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(
struct Cyc_Tcgenrep_RepInfo*,struct Cyc_Tcgenrep_RepInfo*),int(*hash)(struct Cyc_Tcgenrep_RepInfo*)))
Cyc_Hashtable_create)(53,Cyc_Tcgenrep_repcmp,Cyc_Tcgenrep_rephash);struct Cyc_List_List*
ds=Cyc_Tcgenrep_dfsvisit(ri,tab);return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_flatten)(ds);}static char _tmp18[11]="Typestruct";static struct
_tagged_arr Cyc_Tcgenrep_typestruct_str={_tmp18,_tmp18,_tmp18 + 11};static char
_tmp19[4]="Var";static struct _tagged_arr Cyc_Tcgenrep_var_str={_tmp19,_tmp19,
_tmp19 + 4};static char _tmp1A[4]="Int";static struct _tagged_arr Cyc_Tcgenrep_int_str={
_tmp1A,_tmp1A,_tmp1A + 4};static char _tmp1B[6]="Float";static struct _tagged_arr Cyc_Tcgenrep_float_str={
_tmp1B,_tmp1B,_tmp1B + 6};static char _tmp1C[7]="Double";static struct _tagged_arr Cyc_Tcgenrep_double_str={
_tmp1C,_tmp1C,_tmp1C + 7};static char _tmp1D[8]="ThinPtr";static struct _tagged_arr
Cyc_Tcgenrep_thinptr_str={_tmp1D,_tmp1D,_tmp1D + 8};static char _tmp1E[7]="FatPtr";
static struct _tagged_arr Cyc_Tcgenrep_fatptr_str={_tmp1E,_tmp1E,_tmp1E + 7};static
char _tmp1F[6]="Tuple";static struct _tagged_arr Cyc_Tcgenrep_tuple_str={_tmp1F,
_tmp1F,_tmp1F + 6};static char _tmp20[12]="TUnionField";static struct _tagged_arr Cyc_Tcgenrep_tunionfield_str={
_tmp20,_tmp20,_tmp20 + 12};static char _tmp21[7]="Struct";static struct _tagged_arr
Cyc_Tcgenrep_struct_str={_tmp21,_tmp21,_tmp21 + 7};static char _tmp22[7]="TUnion";
static struct _tagged_arr Cyc_Tcgenrep_tunion_str={_tmp22,_tmp22,_tmp22 + 7};static
char _tmp23[8]="XTUnion";static struct _tagged_arr Cyc_Tcgenrep_xtunion_str={_tmp23,
_tmp23,_tmp23 + 8};static char _tmp24[6]="Union";static struct _tagged_arr Cyc_Tcgenrep_union_str={
_tmp24,_tmp24,_tmp24 + 6};static char _tmp25[5]="Enum";static struct _tagged_arr Cyc_Tcgenrep_enum_str={
_tmp25,_tmp25,_tmp25 + 5};static char _tmp26[7]="name_t";static struct _tagged_arr Cyc_Tcgenrep_name_t_str={
_tmp26,_tmp26,_tmp26 + 7};static struct _tuple6*Cyc_Tcgenrep_typerep_name(struct
_tagged_arr*name){return({struct _tuple6*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->f1=(
void*)& Cyc_Tcgenrep_typerep_nmspace;_tmp27->f2=name;_tmp27;});}static int Cyc_Tcgenrep_gen_id_counter=
0;static struct _tagged_arr*Cyc_Tcgenrep_new_gen_id(struct _tagged_arr name){return({
struct _tagged_arr*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[0]=(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp2B;_tmp2B.tag=1;_tmp2B.f1=(int)((unsigned int)Cyc_Tcgenrep_gen_id_counter
++);{struct Cyc_Std_String_pa_struct _tmp2A;_tmp2A.tag=0;_tmp2A.f1=(struct
_tagged_arr)name;{void*_tmp29[2]={& _tmp2A,& _tmp2B};Cyc_Std_aprintf(_tag_arr("_gen%s_%d",
sizeof(char),10),_tag_arr(_tmp29,sizeof(void*),2));}}});_tmp28;});}static void Cyc_Tcgenrep_print_params(
struct Cyc_List_List*l){({void*_tmp2C[0]={};Cyc_Std_printf(_tag_arr("<",sizeof(
char),2),_tag_arr(_tmp2C,sizeof(void*),0));});{struct Cyc_List_List*p=l;for(0;p != 
0;p=p->tl){({struct Cyc_Std_Int_pa_struct _tmp2F;_tmp2F.tag=1;_tmp2F.f1=(int)((
unsigned int)((int)(p->tl != 0?',':' ')));{struct Cyc_Std_String_pa_struct _tmp2E;
_tmp2E.tag=0;_tmp2E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)p->hd);{
void*_tmp2D[2]={& _tmp2E,& _tmp2F};Cyc_Std_printf(_tag_arr("%s%c",sizeof(char),5),
_tag_arr(_tmp2D,sizeof(void*),2));}}});}}({void*_tmp30[0]={};Cyc_Std_printf(
_tag_arr(">\n",sizeof(char),3),_tag_arr(_tmp30,sizeof(void*),0));});}static void
Cyc_Tcgenrep_print_tvars(struct Cyc_List_List*l){({void*_tmp31[0]={};Cyc_Std_printf(
_tag_arr("<",sizeof(char),2),_tag_arr(_tmp31,sizeof(void*),0));});{struct Cyc_List_List*
p=l;for(0;p != 0;p=p->tl){({struct Cyc_Std_Int_pa_struct _tmp34;_tmp34.tag=1;_tmp34.f1=(
int)((unsigned int)((int)(p->tl != 0?',':' ')));{struct Cyc_Std_String_pa_struct
_tmp33;_tmp33.tag=0;_tmp33.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)p->hd)->name;{
void*_tmp32[2]={& _tmp33,& _tmp34};Cyc_Std_printf(_tag_arr("%s%c",sizeof(char),5),
_tag_arr(_tmp32,sizeof(void*),2));}}});}}({void*_tmp35[0]={};Cyc_Std_printf(
_tag_arr(">\n",sizeof(char),3),_tag_arr(_tmp35,sizeof(void*),0));});}static
struct Cyc_Absyn_Rel_n_struct Cyc_Tcgenrep_top_nm_0={0,0};static void*Cyc_Tcgenrep_top_nm=(
void*)& Cyc_Tcgenrep_top_nm_0;static struct _tuple6*Cyc_Tcgenrep_toplevel_name(
struct _tagged_arr*name){return({struct _tuple6*_tmp37=_cycalloc(sizeof(*_tmp37));
_tmp37->f1=Cyc_Tcgenrep_top_nm;_tmp37->f2=(struct _tagged_arr*)name;_tmp37;});}
static struct _tuple6*Cyc_Tcgenrep_relative_name(struct Cyc_List_List*pathl,struct
_tagged_arr*name){struct Cyc_Absyn_Rel_n_struct*path=({struct Cyc_Absyn_Rel_n_struct*
_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Absyn_Rel_n_struct _tmp3A;
_tmp3A.tag=0;_tmp3A.f1=pathl;_tmp3A;});_tmp39;});return({struct _tuple6*_tmp38=
_cycalloc(sizeof(*_tmp38));_tmp38->f1=(void*)path;_tmp38->f2=(struct _tagged_arr*)
name;_tmp38;});}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl(struct
_tagged_arr*name,void*type,struct Cyc_Absyn_Exp*init,struct Cyc_Position_Segment*
seg){struct _tuple6*qvar=Cyc_Tcgenrep_toplevel_name(name);struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(qvar,type,init);(void*)(vd->sc=(void*)((void*)0));{
struct Cyc_Absyn_Var_d_struct*r1=({struct Cyc_Absyn_Var_d_struct*_tmp3B=_cycalloc(
sizeof(*_tmp3B));_tmp3B[0]=({struct Cyc_Absyn_Var_d_struct _tmp3C;_tmp3C.tag=0;
_tmp3C.f1=vd;_tmp3C;});_tmp3B;});void*r2=(void*)r1;return Cyc_Absyn_new_decl(r2,
seg);}}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_vardecl(struct _tuple6*name,
void*type,struct Cyc_Absyn_Exp*init,void*sc,struct Cyc_Position_Segment*seg){
struct _tuple6*topname=Cyc_Tcgenrep_toplevel_name((*name).f2);struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(topname,type,init);(void*)(vd->sc=(void*)sc);{struct Cyc_Absyn_Var_d_struct*
r1=({struct Cyc_Absyn_Var_d_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({
struct Cyc_Absyn_Var_d_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=vd;_tmp3E;});_tmp3D;});
void*r2=(void*)r1;return Cyc_Absyn_new_decl(r2,seg);}}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_string(struct _tagged_arr s,struct Cyc_Position_Segment*seg){
return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_String_c_struct*_tmp3F=
_cycalloc(sizeof(*_tmp3F));_tmp3F[0]=({struct Cyc_Absyn_String_c_struct _tmp40;
_tmp40.tag=5;_tmp40.f1=s;_tmp40;});_tmp3F;}),seg);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_string_cls(
struct Cyc_Position_Segment*seg,struct _tagged_arr*s){return Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_String_c_struct*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41[
0]=({struct Cyc_Absyn_String_c_struct _tmp42;_tmp42.tag=5;_tmp42.f1=*s;_tmp42;});
_tmp41;}),seg);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_qvar_string_cls(
struct Cyc_Position_Segment*seg,struct _tuple6*s){return Cyc_Absyn_const_exp((void*)({
struct Cyc_Absyn_String_c_struct*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=({
struct Cyc_Absyn_String_c_struct _tmp44;_tmp44.tag=5;_tmp44.f1=*(*s).f2;_tmp44;});
_tmp43;}),seg);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int(int i,struct Cyc_Position_Segment*
seg){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*_tmp45=
_cycalloc(sizeof(*_tmp45));_tmp45[0]=({struct Cyc_Absyn_Int_c_struct _tmp46;_tmp46.tag=
2;_tmp46.f1=(void*)((void*)0);_tmp46.f2=i;_tmp46;});_tmp45;}),seg);}static struct
Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int_cls(struct Cyc_Position_Segment*seg,int i){
return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*_tmp47=_cycalloc(
sizeof(*_tmp47));_tmp47[0]=({struct Cyc_Absyn_Int_c_struct _tmp48;_tmp48.tag=2;
_tmp48.f1=(void*)((void*)0);_tmp48.f2=i;_tmp48;});_tmp47;}),seg);}static int Cyc_Tcgenrep_size_of2int(
void*sz){void*_tmp49=sz;_LL6: if((int)_tmp49 != 0)goto _LL8;_LL7: return 8;_LL8: if((
int)_tmp49 != 1)goto _LLA;_LL9: return 16;_LLA: if((int)_tmp49 != 2)goto _LLC;_LLB:
return 32;_LLC: if((int)_tmp49 != 3)goto _LL5;_LLD: return 64;_LL5:;}static void*Cyc_Tcgenrep_array_typ(
void*ty,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp4B;_tmp4B.tag=7;_tmp4B.f1=(void*)ty;_tmp4B.f2=tq;_tmp4B.f3=e;_tmp4B;});
_tmp4A;});}static void*Cyc_Tcgenrep_tunion_typ(struct _tuple6*name){return(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=({
struct Cyc_Absyn_TunionType_struct _tmp4D;_tmp4D.tag=2;_tmp4D.f1=({struct Cyc_Absyn_TunionInfo
_tmp4E;_tmp4E.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp50;_tmp50.tag=0;_tmp50.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp51;_tmp51.name=
name;_tmp51.is_xtunion=0;_tmp51;});_tmp50;});_tmp4F;}));_tmp4E.targs=0;_tmp4E.rgn=(
void*)((void*)2);_tmp4E;});_tmp4D;});_tmp4C;});}static void*Cyc_Tcgenrep_tunionfield_typ(
struct _tuple6*name,struct _tuple6*fieldname){return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp53;_tmp53.tag=3;_tmp53.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp54;_tmp54.field_info=(
void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*_tmp55=_cycalloc(
sizeof(*_tmp55));_tmp55[0]=({struct Cyc_Absyn_UnknownTunionfield_struct _tmp56;
_tmp56.tag=0;_tmp56.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp57;_tmp57.tunion_name=
name;_tmp57.field_name=fieldname;_tmp57.is_xtunion=0;_tmp57;});_tmp56;});_tmp55;}));
_tmp54.targs=0;_tmp54;});_tmp53;});_tmp52;});}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_call_exp(
struct _tuple6*name,struct Cyc_List_List*args,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_unknowncall_exp(Cyc_Absyn_unknownid_exp(name,loc),args,loc);}
static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_tunion_constructor_decl(struct _tuple6*
tunionname,struct _tuple6*fieldname,struct _tuple6*varname,struct Cyc_List_List*
args,void*sc,struct Cyc_Position_Segment*loc){void*_tmp58=Cyc_Tcgenrep_tunionfield_typ(
tunionname,fieldname);struct Cyc_Absyn_Exp*_tmp59=Cyc_Tcgenrep_call_exp(fieldname,
args,loc);struct Cyc_Absyn_Decl*_tmp5A=Cyc_Tcgenrep_gen_vardecl(varname,_tmp58,(
struct Cyc_Absyn_Exp*)_tmp59,sc,loc);return _tmp5A;}struct _tuple9{struct Cyc_Absyn_Tqual
f1;void*f2;};static void*Cyc_Tcgenrep_get_second(struct _tuple9*pair){return(*pair).f2;}
struct _tuple10{struct _tagged_arr*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tagged_arr*Cyc_Tcgenrep_get_first(struct _tuple10*pair){return(*pair).f1;}static
char _tmp5B[5]="list";static struct _tagged_arr Cyc_Tcgenrep_list_str={_tmp5B,_tmp5B,
_tmp5B + 5};static char _tmp5C[5]="List";static struct _tagged_arr Cyc_Tcgenrep_List_str={
_tmp5C,_tmp5C,_tmp5C + 5};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple2_exp_cls(struct Cyc_Position_Segment*
loc,struct _tuple11*es){struct _tuple11 _tmp5E;struct Cyc_Absyn_Exp*_tmp5F;struct Cyc_Absyn_Exp*
_tmp60;struct _tuple11*_tmp5D=es;_tmp5E=*_tmp5D;_tmp5F=_tmp5E.f1;_tmp60=_tmp5E.f2;
return Cyc_Absyn_tuple_exp(({struct Cyc_Absyn_Exp*_tmp61[2];_tmp61[1]=_tmp60;
_tmp61[0]=_tmp5F;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp61,sizeof(struct Cyc_Absyn_Exp*),2));}),loc);}struct _tuple12{struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_tuple3_exp_cls(struct Cyc_Position_Segment*loc,struct _tuple12*es){
struct _tuple12 _tmp63;struct Cyc_Absyn_Exp*_tmp64;struct Cyc_Absyn_Exp*_tmp65;
struct Cyc_Absyn_Exp*_tmp66;struct _tuple12*_tmp62=es;_tmp63=*_tmp62;_tmp64=_tmp63.f1;
_tmp65=_tmp63.f2;_tmp66=_tmp63.f3;return Cyc_Absyn_tuple_exp(({struct Cyc_Absyn_Exp*
_tmp67[3];_tmp67[2]=_tmp66;_tmp67[1]=_tmp65;_tmp67[0]=_tmp64;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp67,sizeof(struct Cyc_Absyn_Exp*),3));}),
loc);}static char _tmp68[5]="NULL";static struct _tagged_arr Cyc_Tcgenrep_null_str={
_tmp68,_tmp68,_tmp68 + 5};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_list_exp(struct
Cyc_List_List*l,struct Cyc_Position_Segment*loc){if(l == 0)return Cyc_Absyn_null_exp(
loc);return Cyc_Tcgenrep_call_exp(Cyc_Tcgenrep_toplevel_name(& Cyc_Tcgenrep_list_str),
l,loc);}struct _tuple13{void*f1;struct Cyc_Position_Segment*f2;};static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_make_offsetof_exp(struct _tuple13*typeloc,int index){struct _tuple13
_tmp6A;void*_tmp6B;struct Cyc_Position_Segment*_tmp6C;struct _tuple13*_tmp69=
typeloc;_tmp6A=*_tmp69;_tmp6B=_tmp6A.f1;_tmp6C=_tmp6A.f2;return Cyc_Absyn_offsetof_exp(
_tmp6B,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp6D=_cycalloc_atomic(
sizeof(*_tmp6D));_tmp6D[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp6E;_tmp6E.tag=
1;_tmp6E.f1=(unsigned int)index;_tmp6E;});_tmp6D;}),_tmp6C);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_get_and_cast_ri_exp(struct Cyc_Position_Segment*loc,struct Cyc_Tcgenrep_RepInfo*
info){return Cyc_Absyn_cast_exp(Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str)),(struct Cyc_Absyn_Exp*)_check_null(info->exp),loc);}
static struct _tuple10*Cyc_Tcgenrep_gen_id_for_exp(struct Cyc_Absyn_Exp*e){return({
struct _tuple10*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->f1=Cyc_Tcgenrep_new_gen_id(
_tag_arr("tuple",sizeof(char),6));_tmp6F->f2=e;_tmp6F;});}static struct Cyc_Absyn_Decl*
Cyc_Tcgenrep_gen_decl_cls(struct _tuple13*env,struct _tuple10*name_exp){struct
_tuple13 _tmp71;void*_tmp72;struct Cyc_Position_Segment*_tmp73;struct _tuple13*
_tmp70=env;_tmp71=*_tmp70;_tmp72=_tmp71.f1;_tmp73=_tmp71.f2;{struct _tuple10
_tmp75;struct _tagged_arr*_tmp76;struct Cyc_Absyn_Exp*_tmp77;struct _tuple10*_tmp74=
name_exp;_tmp75=*_tmp74;_tmp76=_tmp75.f1;_tmp77=_tmp75.f2;return Cyc_Tcgenrep_gen_decl(
_tmp76,_tmp72,(struct Cyc_Absyn_Exp*)_tmp77,_tmp73);}}struct _tuple14{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple14*Cyc_Tcgenrep_null_designator_exp(
struct Cyc_Absyn_Exp*e){return({struct _tuple14*_tmp78=_cycalloc(sizeof(*_tmp78));
_tmp78->f1=0;_tmp78->f2=e;_tmp78;});}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_arr_init_exp(
struct Cyc_List_List*l,struct Cyc_Position_Segment*loc){struct Cyc_List_List*_tmp79=((
struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcgenrep_null_designator_exp,l);return Cyc_Absyn_unresolvedmem_exp(
0,_tmp79,loc);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_address_exp_cls(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_address_exp(e,loc);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_unknownid_exp_cls(struct Cyc_Position_Segment*loc,struct _tuple6*e){
return Cyc_Absyn_unknownid_exp(e,loc);}static int Cyc_Tcgenrep_has_bitfield(struct
Cyc_Absyn_Aggrfield*sf){return sf->width != 0;}static int Cyc_Tcgenrep_add_bitfield_sizes(
int total,struct Cyc_Absyn_Aggrfield*sf){return(int)(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(sf->width))+ total);}static void*Cyc_Tcgenrep_select_structfield_type(
struct Cyc_Absyn_Aggrfield*sf){if(Cyc_Std_strcmp(*sf->name,_tag_arr("",sizeof(
char),1))== 0)({void*_tmp7A[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): anonymous (padding) structfield not handled yet",
sizeof(char),55),_tag_arr(_tmp7A,sizeof(void*),0));});if(Cyc_Tcgenrep_has_bitfield(
sf))({void*_tmp7B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen(): mixed struct bitfields and union bitfields not handled",sizeof(
char),62),_tag_arr(_tmp7B,sizeof(void*),0));});return(void*)sf->type;}struct
_tuple15{struct _tagged_arr*f1;void*f2;};static struct _tuple15*Cyc_Tcgenrep_select_structfield_nmtype(
struct Cyc_Absyn_Aggrfield*sf){if(Cyc_Std_strcmp(*sf->name,_tag_arr("",sizeof(
char),1))== 0)({void*_tmp7C[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): anonymous (padding) structfield not handled yet",
sizeof(char),55),_tag_arr(_tmp7C,sizeof(void*),0));});if(Cyc_Tcgenrep_has_bitfield(
sf))({void*_tmp7D[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen(): mixed struct bitfields and union bitfields not handled",sizeof(
char),62),_tag_arr(_tmp7D,sizeof(void*),0));});return({struct _tuple15*_tmp7E=
_cycalloc(sizeof(*_tmp7E));_tmp7E->f1=sf->name;_tmp7E->f2=(void*)sf->type;_tmp7E;});}
struct _tuple16{int f1;struct _tuple6*f2;};struct _tuple16*Cyc_Tcgenrep_select_enumfield_tagnm(
struct Cyc_Absyn_Enumfield*ef){if(ef->tag == 0)({void*_tmp7F[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Enum tag exp should be filled in by now",
sizeof(char),40),_tag_arr(_tmp7F,sizeof(void*),0));});return({struct _tuple16*
_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->f1=(int)Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(ef->tag));_tmp80->f2=ef->name;_tmp80;});}static
struct Cyc_Dict_Dict*Cyc_Tcgenrep_update_info(struct Cyc_Dict_Dict*dict,void*type,
struct Cyc_List_List*decls,struct Cyc_Absyn_Exp*exp,struct Cyc_List_List*
dependencies,struct Cyc_Core_Opt*fwd_decl){struct Cyc_Core_Opt*_tmp81=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(dict,type);if(_tmp81 != 0){if(((
struct Cyc_Tcgenrep_RepInfo*)_tmp81->v)->decls != 0){Cyc_Tcgenrep_print_typerep_dict(
dict);({void*_tmp82[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("Updating non-forward declaration",sizeof(char),33),_tag_arr(_tmp82,
sizeof(void*),0));});}((struct Cyc_Tcgenrep_RepInfo*)_tmp81->v)->decls=decls;((
struct Cyc_Tcgenrep_RepInfo*)_tmp81->v)->exp=exp;((struct Cyc_Tcgenrep_RepInfo*)
_tmp81->v)->dependencies=dependencies;return dict;}else{return((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(dict,
type,({struct Cyc_Tcgenrep_RepInfo*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->decls=
decls;_tmp83->exp=exp;_tmp83->dependencies=dependencies;_tmp83->fwd_decl=
fwd_decl;_tmp83->emitted=0;_tmp83->is_extern=0;_tmp83;}));}}static struct Cyc_Dict_Dict*
Cyc_Tcgenrep_make_fwd_decl_info(struct Cyc_Dict_Dict*dict,void*type,struct Cyc_Absyn_Exp*
exp,struct Cyc_Core_Opt*fwd_decl,int is_extern){struct Cyc_Core_Opt*_tmp84=((struct
Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(dict,type);
if(_tmp84 != 0){({struct Cyc_Std_String_pa_struct _tmp86;_tmp86.tag=0;_tmp86.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(type);{void*_tmp85[1]={& _tmp86};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Repinfo for fwd declaration of %s already exists",
sizeof(char),49),_tag_arr(_tmp85,sizeof(void*),1));}});return dict;}else{return((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(dict,type,({struct Cyc_Tcgenrep_RepInfo*_tmp87=_cycalloc(
sizeof(*_tmp87));_tmp87->decls=0;_tmp87->exp=exp;_tmp87->dependencies=0;_tmp87->fwd_decl=
fwd_decl;_tmp87->emitted=0;_tmp87->is_extern=is_extern;_tmp87;}));}}static struct
Cyc_Absyn_Tqual Cyc_Tcgenrep_tq_none={0,0,0};static struct _tuple9*Cyc_Tcgenrep_tqual_type(
struct Cyc_Absyn_Tqual*tq,void*type){return({struct _tuple9*_tmp88=_cycalloc(
sizeof(*_tmp88));_tmp88->f1=*((struct Cyc_Absyn_Tqual*)_check_null(tq));_tmp88->f2=
type;_tmp88;});}static void*Cyc_Tcgenrep_tuple_typ(struct Cyc_List_List*types){
struct Cyc_List_List*_tmp89=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct
Cyc_Absyn_Tqual*,void*),struct Cyc_Absyn_Tqual*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tqual_type,(struct Cyc_Absyn_Tqual*)& Cyc_Tcgenrep_tq_none,types);
struct Cyc_Absyn_TupleType_struct*tuple_type_base=({struct Cyc_Absyn_TupleType_struct*
_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A[0]=({struct Cyc_Absyn_TupleType_struct
_tmp8B;_tmp8B.tag=9;_tmp8B.f1=_tmp89;_tmp8B;});_tmp8A;});void*tuple_type=(void*)
tuple_type_base;return tuple_type;}static struct _tuple14*Cyc_Tcgenrep_array_decls(
void*type,struct Cyc_List_List*exps,struct Cyc_Position_Segment*loc){struct Cyc_List_List*
_tmp8C=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_gen_id_for_exp,exps);struct Cyc_List_List*
_tmp8D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(struct _tuple13*,
struct _tuple10*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_gen_decl_cls,({
struct _tuple13*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->f1=type;_tmp9A->f2=loc;
_tmp9A;}),_tmp8C);struct Cyc_List_List*_tmp8E=((struct Cyc_List_List*(*)(struct
_tagged_arr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_first,
_tmp8C);struct Cyc_List_List*_tmp8F=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(
struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_toplevel_name,
_tmp8E);struct Cyc_List_List*_tmp90=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_unknownid_exp_cls,loc,_tmp8F);
struct Cyc_List_List*_tmp91=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Exp*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_address_exp_cls,loc,_tmp90);
struct Cyc_Absyn_Exp*_tmp92=Cyc_Tcgenrep_arr_init_exp(_tmp91,loc);struct
_tagged_arr*_tmp93=Cyc_Tcgenrep_new_gen_id(_tag_arr("arr",sizeof(char),4));void*
_tmp94=Cyc_Absyn_at_typ(type,(void*)2,Cyc_Tcgenrep_tq_none);void*_tmp95=Cyc_Tcgenrep_array_typ(
_tmp94,Cyc_Tcgenrep_tq_none,0);struct Cyc_Absyn_Decl*_tmp96=Cyc_Tcgenrep_gen_decl(
_tmp93,_tmp95,(struct Cyc_Absyn_Exp*)_tmp92,loc);struct Cyc_Absyn_Exp*_tmp97=Cyc_Absyn_unknownid_exp(
Cyc_Tcgenrep_toplevel_name(_tmp93),loc);return({struct _tuple14*_tmp98=_cycalloc(
sizeof(*_tmp98));_tmp98->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8D,({struct Cyc_Absyn_Decl*_tmp99[
1];_tmp99[0]=_tmp96;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp99,sizeof(struct Cyc_Absyn_Decl*),1));}));_tmp98->f2=_tmp97;_tmp98;});}
static void*Cyc_Tcgenrep_check_tunionfield_and_get_type(struct Cyc_Absyn_Tunionfield*
tuf){return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp9B=_cycalloc(sizeof(*
_tmp9B));_tmp9B[0]=({struct Cyc_Absyn_TupleType_struct _tmp9C;_tmp9C.tag=9;_tmp9C.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(({
struct _tuple9*_tmp9D[1];_tmp9D[0]=({struct _tuple9*_tmp9E=_cycalloc(sizeof(*
_tmp9E));_tmp9E->f1=Cyc_Tcgenrep_tq_none;_tmp9E->f2=Cyc_Absyn_uint_typ;_tmp9E;});((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp9D,
sizeof(struct _tuple9*),1));}),tuf->typs);_tmp9C;});_tmp9B;});}static struct
_tuple6*Cyc_Tcgenrep_check_tunionfield_and_get_name(struct Cyc_Absyn_Tunionfield*
tuf){return tuf->name;}struct _tuple17{struct _tuple6*f1;void*f2;};static struct
_tuple17*Cyc_Tcgenrep_check_tunionfield_and_get_nmtype(struct Cyc_Absyn_Tunionfield*
tuf){return({struct _tuple17*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->f1=tuf->name;
_tmp9F->f2=(void*)({struct Cyc_Absyn_TupleType_struct*_tmpA0=_cycalloc(sizeof(*
_tmpA0));_tmpA0[0]=({struct Cyc_Absyn_TupleType_struct _tmpA1;_tmpA1.tag=9;_tmpA1.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(({
struct _tuple9*_tmpA2[1];_tmpA2[0]=({struct _tuple9*_tmpA3=_cycalloc(sizeof(*
_tmpA3));_tmpA3->f1=Cyc_Tcgenrep_tq_none;_tmpA3->f2=Cyc_Absyn_uint_typ;_tmpA3;});((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpA2,
sizeof(struct _tuple9*),1));}),tuf->typs);_tmpA1;});_tmpA0;});_tmp9F;});}static
struct _tuple15*Cyc_Tcgenrep_check_xtunionfield_and_get_name_type(struct Cyc_Absyn_Tunionfield*
tuf){return({struct _tuple15*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->f1=(*tuf->name).f2;
_tmpA4->f2=(void*)({struct Cyc_Absyn_TupleType_struct*_tmpA5=_cycalloc(sizeof(*
_tmpA5));_tmpA5[0]=({struct Cyc_Absyn_TupleType_struct _tmpA6;_tmpA6.tag=9;_tmpA6.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(({
struct _tuple9*_tmpA7[1];_tmpA7[0]=({struct _tuple9*_tmpA8=_cycalloc(sizeof(*
_tmpA8));_tmpA8->f1=Cyc_Tcgenrep_tq_none;_tmpA8->f2=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Tcgenrep_tq_none);_tmpA8;});((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmpA7,sizeof(struct _tuple9*),1));}),tuf->typs);
_tmpA6;});_tmpA5;});_tmpA4;});}static int Cyc_Tcgenrep_filter_empty_tunionfield(
struct Cyc_Absyn_Tunionfield*tuf){return tuf->typs != 0;}static int Cyc_Tcgenrep_filter_nonempty_tunionfield(
struct Cyc_Absyn_Tunionfield*tuf){return tuf->typs == 0;}static struct _tagged_arr*
Cyc_Tcgenrep_get_tunionfield_name(struct Cyc_Absyn_Tunionfield*tuf){return(*tuf->name).f2;}
struct Cyc_Absyn_Aggrfield*Cyc_Tcgenrep_substitute_structfield_type(struct Cyc_List_List*
subst,struct Cyc_Absyn_Aggrfield*sf){return({struct Cyc_Absyn_Aggrfield*_tmpA9=
_cycalloc(sizeof(*_tmpA9));_tmpA9->name=sf->name;_tmpA9->tq=sf->tq;_tmpA9->type=(
void*)Cyc_Tcutil_substitute(subst,(void*)sf->type);_tmpA9->width=sf->width;
_tmpA9->attributes=sf->attributes;_tmpA9;});}struct _tuple9*Cyc_Tcgenrep_substitute_tqual_type(
struct Cyc_List_List*subst,struct _tuple9*pair){struct _tuple9 _tmpAB;struct Cyc_Absyn_Tqual
_tmpAC;void*_tmpAD;struct _tuple9*_tmpAA=pair;_tmpAB=*_tmpAA;_tmpAC=_tmpAB.f1;
_tmpAD=_tmpAB.f2;return({struct _tuple9*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->f1=
_tmpAC;_tmpAE->f2=Cyc_Tcutil_substitute(subst,_tmpAD);_tmpAE;});}struct Cyc_Absyn_Tunionfield*
Cyc_Tcgenrep_substitute_tunionfield_type(struct Cyc_List_List*subst,struct Cyc_Absyn_Tunionfield*
tf){return({struct Cyc_Absyn_Tunionfield*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->name=
tf->name;_tmpAF->typs=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_List_List*,
struct _tuple9*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_tqual_type,subst,tf->typs);_tmpAF->loc=tf->loc;_tmpAF->sc=(
void*)((void*)tf->sc);_tmpAF;});}void*Cyc_Tcgenrep_monomorphize_type(void*type){
void*_tmpB0=Cyc_Tcutil_compress(type);struct Cyc_Absyn_AggrInfo _tmpB1;void*_tmpB2;
struct Cyc_List_List*_tmpB3;struct Cyc_Absyn_TunionInfo _tmpB4;void*_tmpB5;struct
Cyc_Absyn_Tuniondecl**_tmpB6;struct Cyc_Absyn_Tuniondecl*_tmpB7;struct Cyc_List_List*
_tmpB8;void*_tmpB9;_LLF: if(_tmpB0 <= (void*)3?1:*((int*)_tmpB0)!= 10)goto _LL11;
_tmpB1=((struct Cyc_Absyn_AggrType_struct*)_tmpB0)->f1;_tmpB2=(void*)_tmpB1.aggr_info;
_tmpB3=_tmpB1.targs;_LL10: {struct Cyc_Absyn_Aggrdecl*_tmpBA=Cyc_Absyn_get_known_aggrdecl(
_tmpB2);struct Cyc_List_List*_tmpBB=_tmpBA->tvs;if(Cyc_List_length(_tmpB3)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBB))({struct Cyc_Std_Int_pa_struct
_tmpBE;_tmpBE.tag=1;_tmpBE.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmpBB));{struct Cyc_Std_Int_pa_struct _tmpBD;_tmpBD.tag=1;_tmpBD.f1=(
int)((unsigned int)Cyc_List_length(_tmpB3));{void*_tmpBC[2]={& _tmpBD,& _tmpBE};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof(char),60),_tag_arr(_tmpBC,sizeof(void*),2));}}});{struct Cyc_List_List*
_tmpBF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmpBB,_tmpB3);struct Cyc_List_List*fields=0;if(_tmpBA->impl != 0)
fields=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_structfield_type,_tmpBF,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpBA->impl))->fields);{struct Cyc_Absyn_AggrdeclImpl*_tmpC0=_tmpBA->impl
== 0?0:({struct Cyc_Absyn_AggrdeclImpl*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->exist_vars=
0;_tmpC8->rgn_po=0;_tmpC8->fields=fields;_tmpC8;});struct Cyc_Absyn_Aggrdecl*ad2=({
struct Cyc_Absyn_Aggrdecl*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->kind=(void*)((
void*)_tmpBA->kind);_tmpC7->sc=(void*)((void*)_tmpBA->sc);_tmpC7->name=_tmpBA->name;
_tmpC7->tvs=0;_tmpC7->impl=_tmpC0;_tmpC7->attributes=_tmpBA->attributes;_tmpC7;});
return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));
_tmpC1[0]=({struct Cyc_Absyn_AggrType_struct _tmpC2;_tmpC2.tag=10;_tmpC2.f1=({
struct Cyc_Absyn_AggrInfo _tmpC3;_tmpC3.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_KnownAggr_struct
_tmpC5;_tmpC5.tag=1;_tmpC5.f1=({struct Cyc_Absyn_Aggrdecl**_tmpC6=_cycalloc(
sizeof(*_tmpC6));_tmpC6[0]=ad2;_tmpC6;});_tmpC5;});_tmpC4;}));_tmpC3.targs=0;
_tmpC3;});_tmpC2;});_tmpC1;});}}}_LL11: if(_tmpB0 <= (void*)3?1:*((int*)_tmpB0)!= 
2)goto _LL13;_tmpB4=((struct Cyc_Absyn_TunionType_struct*)_tmpB0)->f1;_tmpB5=(void*)
_tmpB4.tunion_info;if(*((int*)_tmpB5)!= 1)goto _LL13;_tmpB6=((struct Cyc_Absyn_KnownTunion_struct*)
_tmpB5)->f1;_tmpB7=*_tmpB6;_tmpB8=_tmpB4.targs;_tmpB9=(void*)_tmpB4.rgn;_LL12: {
struct Cyc_List_List*_tmpC9=_tmpB7->tvs;if(Cyc_List_length(_tmpB8)!= ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpC9))({struct Cyc_Std_Int_pa_struct
_tmpCC;_tmpCC.tag=1;_tmpCC.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmpC9));{struct Cyc_Std_Int_pa_struct _tmpCB;_tmpCB.tag=1;_tmpCB.f1=(
int)((unsigned int)Cyc_List_length(_tmpB8));{void*_tmpCA[2]={& _tmpCB,& _tmpCC};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof(char),60),_tag_arr(_tmpCA,sizeof(void*),2));}}});{struct Cyc_List_List*
_tmpCD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmpC9,_tmpB8);struct Cyc_Core_Opt*fields=0;if(_tmpB7->fields != 0)
fields=({struct Cyc_Core_Opt*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->v=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Tunionfield*(*f)(struct Cyc_List_List*,struct
Cyc_Absyn_Tunionfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_tunionfield_type,_tmpCD,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpB7->fields))->v);_tmpCE;});{struct Cyc_Absyn_Tuniondecl*
_tmpCF=({struct Cyc_Absyn_Tuniondecl*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->sc=(
void*)((void*)_tmpB7->sc);_tmpD8->name=_tmpB7->name;_tmpD8->tvs=0;_tmpD8->fields=
fields;_tmpD8->is_xtunion=_tmpB7->is_xtunion;_tmpD8;});struct Cyc_Absyn_TunionType_struct*
_tmpD0=({struct Cyc_Absyn_TunionType_struct*_tmpD2=_cycalloc(sizeof(*_tmpD2));
_tmpD2[0]=({struct Cyc_Absyn_TunionType_struct _tmpD3;_tmpD3.tag=2;_tmpD3.f1=({
struct Cyc_Absyn_TunionInfo _tmpD4;_tmpD4.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmpD6;_tmpD6.tag=1;_tmpD6.f1=({struct Cyc_Absyn_Tuniondecl**_tmpD7=_cycalloc(
sizeof(*_tmpD7));_tmpD7[0]=_tmpCF;_tmpD7;});_tmpD6;});_tmpD5;}));_tmpD4.targs=
_tmpB8;_tmpD4.rgn=(void*)_tmpB9;_tmpD4;});_tmpD3;});_tmpD2;});struct Cyc_Position_Segment*
_tmpD1=Cyc_Position_segment_of_abs(0,0);return(void*)_tmpD0;}}}_LL13:;_LL14:
return type;_LLE:;}struct _tagged_arr Cyc_Tcgenrep_make_type_cstring(void*t){struct
_tagged_arr s=Cyc_Std_strdup(Cyc_Absynpp_typ2cstring(t));{int i=0;for(0;i < Cyc_Std_strlen((
struct _tagged_arr)s);i ++){if(*((char*)_check_unknown_subscript(s,sizeof(char),i))
== ' ')*((char*)_check_unknown_subscript(s,sizeof(char),i))='_';else{if(!isalnum((
int)*((char*)_check_unknown_subscript(s,sizeof(char),i)))?*((char*)
_check_unknown_subscript(s,sizeof(char),i))!= '_': 0)*((char*)
_check_unknown_subscript(s,sizeof(char),i))=(char)('0' + *((char*)
_check_unknown_subscript(s,sizeof(char),i))% 10);}}}return(struct _tagged_arr)Cyc_Std_strconcat((
struct _tagged_arr)s,_tag_arr("_rep",sizeof(char),5));}struct _tuple18{struct Cyc_Dict_Dict*
f1;struct Cyc_Tcgenrep_RepInfo*f2;};static struct _tuple18*Cyc_Tcgenrep_lookupRep(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,
void*type);struct _tuple19{struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};struct
_tuple20{struct Cyc_Tcenv_Tenv*f1;struct Cyc_Position_Segment*f2;};static struct
_tuple19*Cyc_Tcgenrep_lookupRepsCls(struct _tuple20*env,void*type,struct _tuple19*
carry){struct _tuple18 _tmpDA;struct Cyc_Dict_Dict*_tmpDB;struct Cyc_Tcgenrep_RepInfo*
_tmpDC;struct _tuple18*_tmpD9=Cyc_Tcgenrep_lookupRep((*env).f1,(*carry).f1,(*env).f2,
type);_tmpDA=*_tmpD9;_tmpDB=_tmpDA.f1;_tmpDC=_tmpDA.f2;return({struct _tuple19*
_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->f1=_tmpDB;_tmpDD->f2=({struct Cyc_List_List*
_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->hd=_tmpDC;_tmpDE->tl=(*carry).f2;
_tmpDE;});_tmpDD;});}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepTuple(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,struct
_tuple6*varname,void*sc,void*type,struct Cyc_List_List*types){struct Cyc_Absyn_Exp*
_tmpDF=Cyc_Absyn_sizeoftyp_exp(type,loc);struct _tuple20*_tmpE0=({struct _tuple20*
_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->f1=te;_tmpF5->f2=loc;_tmpF5;});struct
_tuple19*_tmpE1=({struct _tuple19*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->f1=
dict;_tmpF4->f2=0;_tmpF4;});struct _tuple19 _tmpE3;struct Cyc_Dict_Dict*_tmpE4;
struct Cyc_List_List*_tmpE5;struct _tuple19*_tmpE2=((struct _tuple19*(*)(struct
_tuple19*(*f)(struct _tuple20*,void*,struct _tuple19*),struct _tuple20*,struct Cyc_List_List*
x,struct _tuple19*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmpE0,
types,_tmpE1);_tmpE3=*_tmpE2;_tmpE4=_tmpE3.f1;_tmpE5=_tmpE3.f2;dict=_tmpE4;{
struct Cyc_List_List*_tmpE6=((struct Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(
Cyc_List_length(types),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*
_tmpE7=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple13*,int),
struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({
struct _tuple13*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->f1=type;_tmpF3->f2=loc;
_tmpF3;}),_tmpE6);struct Cyc_List_List*_tmpE8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmpE5);struct Cyc_List_List*_tmpE9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple11*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmpE7,_tmpE8));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmpF2[2];_tmpF2[1]=
Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmpF2[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_arr(_tmpF2,sizeof(void*),2));}));
struct _tuple14 _tmpEB;struct Cyc_List_List*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;
struct _tuple14*_tmpEA=Cyc_Tcgenrep_array_decls(tuple_type,_tmpE9,loc);_tmpEB=*
_tmpEA;_tmpEC=_tmpEB.f1;_tmpED=_tmpEB.f2;{struct Cyc_Absyn_Decl*_tmpEE=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tuple_str),varname,({struct Cyc_Absyn_Exp*_tmpF1[2];_tmpF1[1]=_tmpED;
_tmpF1[0]=_tmpDF;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmpF1,sizeof(struct Cyc_Absyn_Exp*),2));}),sc,loc);struct Cyc_List_List*
_tmpEF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmpEC,({struct Cyc_Absyn_Decl*_tmpF0[1];_tmpF0[0]=_tmpEE;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpF0,
sizeof(struct Cyc_Absyn_Decl*),1));}));return Cyc_Tcgenrep_update_info(dict,type,
_tmpEF,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmpE5,0);}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepTunionfield(
struct _tuple6*tname,struct _tuple6*fname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*
dict,struct Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,
struct Cyc_List_List*types){struct Cyc_Absyn_Exp*_tmpF6=Cyc_Absyn_sizeoftyp_exp(
type,loc);struct _tuple20*_tmpF7=({struct _tuple20*_tmp10E=_cycalloc(sizeof(*
_tmp10E));_tmp10E->f1=te;_tmp10E->f2=loc;_tmp10E;});struct _tuple19*_tmpF8=({
struct _tuple19*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->f1=dict;_tmp10D->f2=
0;_tmp10D;});struct _tuple19 _tmpFA;struct Cyc_Dict_Dict*_tmpFB;struct Cyc_List_List*
_tmpFC;struct _tuple19*_tmpF9=((struct _tuple19*(*)(struct _tuple19*(*f)(struct
_tuple20*,void*,struct _tuple19*),struct _tuple20*,struct Cyc_List_List*x,struct
_tuple19*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmpF7,types,
_tmpF8);_tmpFA=*_tmpF9;_tmpFB=_tmpFA.f1;_tmpFC=_tmpFA.f2;dict=_tmpFB;{struct Cyc_List_List*
_tmpFD=((struct Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(Cyc_List_length(
types),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmpFE=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple13*,int),struct _tuple13*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({struct _tuple13*_tmp10C=
_cycalloc(sizeof(*_tmp10C));_tmp10C->f1=type;_tmp10C->f2=loc;_tmp10C;}),_tmpFD);
struct Cyc_List_List*_tmpFF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmpFC);struct Cyc_List_List*_tmp100=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple11*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmpFE,_tmpFF));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmp10B[2];_tmp10B[
1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp10B[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_arr(_tmp10B,sizeof(void*),2));}));
struct Cyc_Absyn_Exp*_tmp101=Cyc_Tcgenrep_cnst_string(*(*tname).f2,loc);struct Cyc_Absyn_Exp*
_tmp102=Cyc_Tcgenrep_cnst_string(*(*fname).f2,loc);struct _tuple14 _tmp104;struct
Cyc_List_List*_tmp105;struct Cyc_Absyn_Exp*_tmp106;struct _tuple14*_tmp103=Cyc_Tcgenrep_array_decls(
tuple_type,_tmp100,loc);_tmp104=*_tmp103;_tmp105=_tmp104.f1;_tmp106=_tmp104.f2;{
struct Cyc_Absyn_Decl*_tmp107=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunionfield_str),
varname,({struct Cyc_Absyn_Exp*_tmp10A[4];_tmp10A[3]=_tmp106;_tmp10A[2]=_tmpF6;
_tmp10A[1]=_tmp102;_tmp10A[0]=_tmp101;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp10A,sizeof(struct Cyc_Absyn_Exp*),4));}),
sc,loc);struct Cyc_List_List*_tmp108=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp105,({struct Cyc_Absyn_Decl*
_tmp109[1];_tmp109[0]=_tmp107;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp109,sizeof(struct Cyc_Absyn_Decl*),1));}));return Cyc_Tcgenrep_update_info(
dict,type,_tmp108,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmpFC,0);}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepStruct(
struct Cyc_Core_Opt*sname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct
Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*
nmtypes){struct Cyc_Absyn_Exp*_tmp10F=Cyc_Absyn_sizeoftyp_exp(type,loc);struct
_tuple20*_tmp110=({struct _tuple20*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->f1=
te;_tmp12C->f2=loc;_tmp12C;});struct _tuple19*_tmp111=({struct _tuple19*_tmp12B=
_cycalloc(sizeof(*_tmp12B));_tmp12B->f1=dict;_tmp12B->f2=0;_tmp12B;});struct Cyc_List_List*
_tmp113;struct Cyc_List_List*_tmp114;struct _tuple0 _tmp112=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(nmtypes);_tmp113=_tmp112.f1;_tmp114=
_tmp112.f2;{struct _tuple19 _tmp116;struct Cyc_Dict_Dict*_tmp117;struct Cyc_List_List*
_tmp118;struct _tuple19*_tmp115=((struct _tuple19*(*)(struct _tuple19*(*f)(struct
_tuple20*,void*,struct _tuple19*),struct _tuple20*,struct Cyc_List_List*x,struct
_tuple19*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp110,
_tmp114,_tmp111);_tmp116=*_tmp115;_tmp117=_tmp116.f1;_tmp118=_tmp116.f2;dict=
_tmp117;{struct Cyc_List_List*_tmp119=((struct Cyc_List_List*(*)(int n,int(*f)(int)))
Cyc_List_tabulate)(Cyc_List_length(_tmp114),(int(*)(int))Cyc_Core_identity);
struct Cyc_List_List*_tmp11A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct _tuple13*,int),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_make_offsetof_exp,({struct _tuple13*_tmp12A=_cycalloc(sizeof(*
_tmp12A));_tmp12A->f1=type;_tmp12A->f2=loc;_tmp12A;}),_tmp119);struct Cyc_List_List*
_tmp11B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tagged_arr*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_cnst_string_cls,loc,_tmp113);struct Cyc_List_List*_tmp11C=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,
loc,_tmp118);struct Cyc_List_List*_tmp11D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple3_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*
z))Cyc_List_zip3)(_tmp11A,_tmp11B,_tmp11C));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({
void*_tmp129[3];_tmp129[2]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str));_tmp129[1]=Cyc_Absyn_const_string_typ((void*)2);
_tmp129[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_arr(_tmp129,sizeof(void*),3));}));
struct _tuple14 _tmp11F;struct Cyc_List_List*_tmp120;struct Cyc_Absyn_Exp*_tmp121;
struct _tuple14*_tmp11E=Cyc_Tcgenrep_array_decls(tuple_type,_tmp11D,loc);_tmp11F=*
_tmp11E;_tmp120=_tmp11F.f1;_tmp121=_tmp11F.f2;{struct Cyc_Absyn_Exp*name;if(sname
== 0)name=Cyc_Absyn_null_exp(loc);else{struct _tagged_arr*_tmp122=Cyc_Tcgenrep_new_gen_id(
_tag_arr("name",sizeof(char),5));struct Cyc_Absyn_Decl*_tmp123=Cyc_Tcgenrep_gen_decl(
_tmp122,Cyc_Absyn_const_string_typ((void*)2),(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*((
struct _tuple6*)sname->v)).f2,loc),loc);_tmp120=({struct Cyc_List_List*_tmp124=
_cycalloc(sizeof(*_tmp124));_tmp124->hd=_tmp123;_tmp124->tl=_tmp120;_tmp124;});
name=Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmp122),loc),loc);}{struct Cyc_Absyn_Decl*_tmp125=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str),varname,({struct Cyc_Absyn_Exp*_tmp128[3];_tmp128[2]=
_tmp121;_tmp128[1]=_tmp10F;_tmp128[0]=name;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp128,sizeof(struct Cyc_Absyn_Exp*),3));}),
sc,loc);struct Cyc_List_List*_tmp126=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp120,({struct Cyc_Absyn_Decl*
_tmp127[1];_tmp127[0]=_tmp125;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp127,sizeof(struct Cyc_Absyn_Decl*),1));}));return Cyc_Tcgenrep_update_info(
dict,type,_tmp126,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp118,0);}}}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepUnion(
struct Cyc_Core_Opt*uname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct
Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*
nmtypes){struct Cyc_Absyn_Exp*_tmp12D=Cyc_Absyn_sizeoftyp_exp(type,loc);struct
_tuple20*_tmp12E=({struct _tuple20*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->f1=
te;_tmp147->f2=loc;_tmp147;});struct _tuple19*_tmp12F=({struct _tuple19*_tmp146=
_cycalloc(sizeof(*_tmp146));_tmp146->f1=dict;_tmp146->f2=0;_tmp146;});struct Cyc_List_List*
_tmp131;struct Cyc_List_List*_tmp132;struct _tuple0 _tmp130=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(nmtypes);_tmp131=_tmp130.f1;_tmp132=
_tmp130.f2;{struct _tuple19 _tmp134;struct Cyc_Dict_Dict*_tmp135;struct Cyc_List_List*
_tmp136;struct _tuple19*_tmp133=((struct _tuple19*(*)(struct _tuple19*(*f)(struct
_tuple20*,void*,struct _tuple19*),struct _tuple20*,struct Cyc_List_List*x,struct
_tuple19*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp12E,
_tmp132,_tmp12F);_tmp134=*_tmp133;_tmp135=_tmp134.f1;_tmp136=_tmp134.f2;dict=
_tmp135;{struct Cyc_List_List*_tmp137=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tagged_arr*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,
_tmp131);struct Cyc_List_List*_tmp138=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp136);struct Cyc_List_List*_tmp139=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple11*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp137,_tmp138));void*_tmp13A=Cyc_Tcgenrep_tuple_typ(({void*_tmp145[2];_tmp145[
1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp145[0]=Cyc_Absyn_const_string_typ((void*)2);Cyc_List_list(_tag_arr(_tmp145,
sizeof(void*),2));}));struct _tuple14 _tmp13C;struct Cyc_List_List*_tmp13D;struct
Cyc_Absyn_Exp*_tmp13E;struct _tuple14*_tmp13B=Cyc_Tcgenrep_array_decls(_tmp13A,
_tmp139,loc);_tmp13C=*_tmp13B;_tmp13D=_tmp13C.f1;_tmp13E=_tmp13C.f2;{struct Cyc_Absyn_Exp*
name;if(uname == 0)name=Cyc_Absyn_null_exp(loc);else{struct _tagged_arr*_tmp13F=
Cyc_Tcgenrep_new_gen_id(_tag_arr("name",sizeof(char),5));struct Cyc_Absyn_Decl*
_tmp140=Cyc_Tcgenrep_gen_decl(_tmp13F,Cyc_Absyn_const_string_typ((void*)2),(
struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*((struct _tuple6*)uname->v)).f2,
loc),loc);_tmp13D=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));
_tmp141->hd=_tmp140;_tmp141->tl=_tmp13D;_tmp141;});name=Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(_tmp13F),loc),loc);}{struct
Cyc_Absyn_Decl*_tmp142=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_union_str),
varname,({struct Cyc_Absyn_Exp*_tmp144[3];_tmp144[2]=_tmp13E;_tmp144[1]=_tmp12D;
_tmp144[0]=name;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp144,sizeof(struct Cyc_Absyn_Exp*),3));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp13D,({struct Cyc_Absyn_Decl*_tmp143[1];_tmp143[0]=
_tmp142;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp143,sizeof(struct Cyc_Absyn_Decl*),1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp136,0);}}}}}static struct Cyc_Dict_Dict*
Cyc_Tcgenrep_buildRepEnum(struct _tuple6**ename,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*
dict,struct Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,
struct Cyc_List_List*tagnms){struct Cyc_Absyn_Exp*_tmp148=Cyc_Absyn_sizeoftyp_exp(
type,loc);struct Cyc_List_List*_tmp14A;struct Cyc_List_List*_tmp14B;struct _tuple0
_tmp149=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(tagnms);
_tmp14A=_tmp149.f1;_tmp14B=_tmp149.f2;{struct Cyc_List_List*_tmp14C=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
_tmp14B);struct Cyc_List_List*_tmp14D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp14A);struct Cyc_List_List*
_tmp14E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple11*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(_tmp14D,_tmp14C));void*_tmp14F=Cyc_Tcgenrep_tuple_typ(({
void*_tmp15A[2];_tmp15A[1]=Cyc_Absyn_const_string_typ((void*)2);_tmp15A[0]=Cyc_Absyn_uint_typ;
Cyc_List_list(_tag_arr(_tmp15A,sizeof(void*),2));}));struct _tuple14 _tmp151;
struct Cyc_List_List*_tmp152;struct Cyc_Absyn_Exp*_tmp153;struct _tuple14*_tmp150=
Cyc_Tcgenrep_array_decls(_tmp14F,_tmp14E,loc);_tmp151=*_tmp150;_tmp152=_tmp151.f1;
_tmp153=_tmp151.f2;{struct Cyc_Absyn_Exp*name;if(ename == 0)name=Cyc_Absyn_null_exp(
loc);else{struct _tagged_arr*_tmp154=Cyc_Tcgenrep_new_gen_id(_tag_arr("name",
sizeof(char),5));struct Cyc_Absyn_Decl*_tmp155=Cyc_Tcgenrep_gen_decl(_tmp154,Cyc_Absyn_const_string_typ((
void*)2),(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*(*ename)).f2,loc),loc);
_tmp152=({struct Cyc_List_List*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->hd=
_tmp155;_tmp156->tl=_tmp152;_tmp156;});name=Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
Cyc_Tcgenrep_toplevel_name(_tmp154),loc),loc);}{struct Cyc_Absyn_Decl*_tmp157=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_enum_str),varname,({struct Cyc_Absyn_Exp*_tmp159[3];_tmp159[2]=
_tmp153;_tmp159[1]=_tmp148;_tmp159[0]=name;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp159,sizeof(struct Cyc_Absyn_Exp*),3));}),
sc,loc);return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp152,({
struct Cyc_Absyn_Decl*_tmp158[1];_tmp158[0]=_tmp157;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp158,sizeof(struct Cyc_Absyn_Decl*),
1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),0,0);}}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepTunion(
struct _tuple6*tname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*tonames,struct
Cyc_List_List*nmtypes){struct Cyc_List_List*_tmp15B=((struct Cyc_List_List*(*)(int
n,int(*f)(int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tonames),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp15C=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp15B);
struct Cyc_List_List*_tmp15D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
tonames);struct Cyc_List_List*_tmp15E=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp15C,_tmp15D);struct Cyc_List_List*
_tmp15F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple11*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp15E);void*_tmp160=Cyc_Tcgenrep_tuple_typ(({
void*_tmp17F[2];_tmp17F[1]=Cyc_Absyn_const_string_typ((void*)2);_tmp17F[0]=Cyc_Absyn_uint_typ;
Cyc_List_list(_tag_arr(_tmp17F,sizeof(void*),2));}));struct _tuple14 _tmp162;
struct Cyc_List_List*_tmp163;struct Cyc_Absyn_Exp*_tmp164;struct _tuple14*_tmp161=
Cyc_Tcgenrep_array_decls(_tmp160,_tmp15F,loc);_tmp162=*_tmp161;_tmp163=_tmp162.f1;
_tmp164=_tmp162.f2;{struct _tuple20*_tmp165=({struct _tuple20*_tmp17E=_cycalloc(
sizeof(*_tmp17E));_tmp17E->f1=te;_tmp17E->f2=loc;_tmp17E;});struct _tuple19*
_tmp166=({struct _tuple19*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->f1=dict;
_tmp17D->f2=0;_tmp17D;});struct Cyc_List_List*_tmp168;struct Cyc_List_List*_tmp169;
struct _tuple0 _tmp167=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
nmtypes);_tmp168=_tmp167.f1;_tmp169=_tmp167.f2;{struct _tuple19 _tmp16B;struct Cyc_Dict_Dict*
_tmp16C;struct Cyc_List_List*_tmp16D;struct _tuple19*_tmp16A=((struct _tuple19*(*)(
struct _tuple19*(*f)(struct _tuple20*,void*,struct _tuple19*),struct _tuple20*,
struct Cyc_List_List*x,struct _tuple19*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp165,_tmp169,_tmp166);_tmp16B=*_tmp16A;_tmp16C=_tmp16B.f1;_tmp16D=_tmp16B.f2;
dict=_tmp16C;{struct Cyc_List_List*_tmp16E=((struct Cyc_List_List*(*)(int n,int(*f)(
int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp16D),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp16F=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp16E);
struct Cyc_List_List*_tmp170=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
_tmp168);struct Cyc_List_List*_tmp171=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp16D);struct Cyc_List_List*_tmp172=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y,struct Cyc_List_List*z))Cyc_List_zip3)(_tmp16F,_tmp170,
_tmp171);struct Cyc_List_List*_tmp173=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple3_exp_cls,loc,_tmp172);
void*tuple_type2=Cyc_Tcgenrep_tuple_typ(({void*_tmp17C[3];_tmp17C[2]=Cyc_Tcgenrep_tunion_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));_tmp17C[1]=Cyc_Absyn_const_string_typ((
void*)2);_tmp17C[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_arr(_tmp17C,sizeof(
void*),3));}));struct _tuple14 _tmp175;struct Cyc_List_List*_tmp176;struct Cyc_Absyn_Exp*
_tmp177;struct _tuple14*_tmp174=Cyc_Tcgenrep_array_decls(tuple_type2,_tmp173,loc);
_tmp175=*_tmp174;_tmp176=_tmp175.f1;_tmp177=_tmp175.f2;{struct Cyc_Absyn_Exp*
_tmp178=Cyc_Tcgenrep_cnst_string(*(*tname).f2,loc);struct Cyc_Absyn_Decl*_tmp179=
Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str),varname,({struct Cyc_Absyn_Exp*
_tmp17B[3];_tmp17B[2]=_tmp177;_tmp17B[1]=_tmp164;_tmp17B[0]=_tmp178;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp17B,sizeof(struct Cyc_Absyn_Exp*),
3));}),sc,loc);return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp163,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp176,({struct Cyc_Absyn_Decl*_tmp17A[1];_tmp17A[0]=_tmp179;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp17A,sizeof(struct Cyc_Absyn_Decl*),
1));}))),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp16D,0);return dict;}}}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepXTunion(
struct _tuple6*xname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*nametypes){
struct Cyc_List_List*_tmp181;struct Cyc_List_List*_tmp182;struct _tuple0 _tmp180=((
struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(nametypes);_tmp181=
_tmp180.f1;_tmp182=_tmp180.f2;{struct _tuple20*_tmp183=({struct _tuple20*_tmp198=
_cycalloc(sizeof(*_tmp198));_tmp198->f1=te;_tmp198->f2=loc;_tmp198;});struct
_tuple19*_tmp184=({struct _tuple19*_tmp197=_cycalloc(sizeof(*_tmp197));_tmp197->f1=
dict;_tmp197->f2=0;_tmp197;});struct _tuple19 _tmp186;struct Cyc_Dict_Dict*_tmp187;
struct Cyc_List_List*_tmp188;struct _tuple19*_tmp185=((struct _tuple19*(*)(struct
_tuple19*(*f)(struct _tuple20*,void*,struct _tuple19*),struct _tuple20*,struct Cyc_List_List*
x,struct _tuple19*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp183,_tmp182,_tmp184);_tmp186=*_tmp185;_tmp187=_tmp186.f1;_tmp188=_tmp186.f2;
dict=_tmp187;{struct Cyc_List_List*_tmp189=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tagged_arr*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,
_tmp181);struct Cyc_List_List*_tmp18A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp188);struct Cyc_List_List*_tmp18B=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp189,_tmp18A);struct Cyc_List_List*
_tmp18C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple11*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp18B);void*name_type=Cyc_Absyn_tagged_typ(Cyc_Absyn_char_typ,(
void*)2,({struct Cyc_Absyn_Tqual _tmp196;_tmp196.q_const=1;_tmp196.q_volatile=0;
_tmp196.q_restrict=0;_tmp196;}));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*
_tmp195[2];_tmp195[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp195[0]=name_type;Cyc_List_list(_tag_arr(_tmp195,sizeof(void*),2));}));struct
_tuple14 _tmp18E;struct Cyc_List_List*_tmp18F;struct Cyc_Absyn_Exp*_tmp190;struct
_tuple14*_tmp18D=Cyc_Tcgenrep_array_decls(tuple_type,_tmp18C,loc);_tmp18E=*
_tmp18D;_tmp18F=_tmp18E.f1;_tmp190=_tmp18E.f2;{struct Cyc_Absyn_Exp*_tmp191=Cyc_Tcgenrep_cnst_string(*(*
xname).f2,loc);struct Cyc_Absyn_Decl*_tmp192=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_xtunion_str),varname,({struct Cyc_Absyn_Exp*_tmp194[2];_tmp194[1]=
_tmp190;_tmp194[0]=_tmp191;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp194,sizeof(struct Cyc_Absyn_Exp*),2));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp18F,({struct Cyc_Absyn_Decl*_tmp193[1];_tmp193[0]=
_tmp192;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp193,sizeof(struct Cyc_Absyn_Decl*),1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp188,0);return dict;}}}}static struct
_tuple18*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,
struct Cyc_Position_Segment*loc,void*type){struct Cyc_Core_Opt*info=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(dict,type);if(info != 0){if(((
struct Cyc_Tcgenrep_RepInfo*)info->v)->is_extern)dict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*,void*))Cyc_Dict_delete)(dict,type);else{struct _tuple18*
_tmp199=({struct _tuple18*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->f1=dict;
_tmp19A->f2=(struct Cyc_Tcgenrep_RepInfo*)info->v;_tmp19A;});return _tmp199;}}{
void*_tmp19B=Cyc_Tcutil_compress(Cyc_Tcgenrep_monomorphize_type(type));void*
_tmp19C;void*_tmp19D;int _tmp19E;struct Cyc_Absyn_PtrInfo _tmp19F;void*_tmp1A0;
struct Cyc_Absyn_Tqual _tmp1A1;struct Cyc_Absyn_Exp*_tmp1A2;struct Cyc_List_List*
_tmp1A3;struct _tuple6*_tmp1A4;struct Cyc_List_List*_tmp1A5;struct Cyc_Absyn_Typedefdecl*
_tmp1A6;struct Cyc_Core_Opt*_tmp1A7;struct Cyc_Absyn_Tvar*_tmp1A8;struct Cyc_Absyn_FnInfo
_tmp1A9;struct Cyc_Absyn_TunionInfo _tmp1AA;void*_tmp1AB;struct Cyc_Absyn_Tuniondecl**
_tmp1AC;struct Cyc_Absyn_Tuniondecl*_tmp1AD;struct Cyc_List_List*_tmp1AE;void*
_tmp1AF;struct Cyc_Absyn_TunionFieldInfo _tmp1B0;struct Cyc_Absyn_AggrInfo _tmp1B1;
void*_tmp1B2;struct Cyc_List_List*_tmp1B3;void*_tmp1B4;struct Cyc_List_List*
_tmp1B5;void*_tmp1B6;struct Cyc_List_List*_tmp1B7;struct Cyc_Absyn_Enumdecl*
_tmp1B8;struct Cyc_List_List*_tmp1B9;_LL16: if(_tmp19B <= (void*)3?1:*((int*)
_tmp19B)!= 5)goto _LL18;_tmp19C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp19B)->f1;
_tmp19D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp19B)->f2;_LL17: {struct
_tuple6*_tmp1BA=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",
sizeof(char),4)));struct Cyc_Absyn_Decl*_tmp1BB=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_int_str),_tmp1BA,({struct Cyc_Absyn_Exp*_tmp1BD[2];_tmp1BD[1]=Cyc_Tcgenrep_cnst_int(
Cyc_Tcgenrep_size_of2int(_tmp19D),loc);_tmp1BD[0]=Cyc_Tcgenrep_cnst_int(_tmp19C
== (void*)0?1: 0,loc);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1BD,sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(
dict,type,({struct Cyc_Absyn_Decl*_tmp1BC[1];_tmp1BC[0]=_tmp1BB;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1BC,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1BA,loc),loc),0,0);goto _LL15;}_LL18: if((int)_tmp19B != 1)goto _LL1A;_LL19: dict=
Cyc_Tcgenrep_update_info(dict,type,0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unknownid_exp(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_float_str),loc),0,0);goto _LL15;_LL1A: if(
_tmp19B <= (void*)3?1:*((int*)_tmp19B)!= 6)goto _LL1C;_tmp19E=((struct Cyc_Absyn_DoubleType_struct*)
_tmp19B)->f1;_LL1B: dict=Cyc_Tcgenrep_update_info(dict,type,0,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_double_str),loc),
0,0);goto _LL15;_LL1C: if(_tmp19B <= (void*)3?1:*((int*)_tmp19B)!= 4)goto _LL1E;
_tmp19F=((struct Cyc_Absyn_PointerType_struct*)_tmp19B)->f1;_LL1D:{void*_tmp1BE=
Cyc_Absyn_conref_val(_tmp19F.bounds);struct Cyc_Absyn_Exp*_tmp1BF;_LL4D: if(
_tmp1BE <= (void*)1?1:*((int*)_tmp1BE)!= 0)goto _LL4F;_tmp1BF=((struct Cyc_Absyn_Upper_b_struct*)
_tmp1BE)->f1;_LL4E: {struct _tuple6*_tmp1C0=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(char),4)));struct Cyc_Core_Opt*_tmp1C1=({struct Cyc_Core_Opt*
_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->v=Cyc_Tcgenrep_gen_vardecl(_tmp1C0,
Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str)),0,(void*)3,loc);_tmp1CA;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1C0,loc),loc),_tmp1C1,0);{struct _tuple18 _tmp1C3;
struct Cyc_Dict_Dict*_tmp1C4;struct Cyc_Tcgenrep_RepInfo*_tmp1C5;struct _tuple18*
_tmp1C2=Cyc_Tcgenrep_lookupRep(te,dict,loc,(void*)_tmp19F.elt_typ);_tmp1C3=*
_tmp1C2;_tmp1C4=_tmp1C3.f1;_tmp1C5=_tmp1C3.f2;dict=_tmp1C4;{unsigned int bound=
Cyc_Evexp_eval_const_uint_exp(_tmp1BF);struct Cyc_Absyn_Decl*_tmp1C6=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str),_tmp1C0,({struct Cyc_Absyn_Exp*_tmp1C9[2];_tmp1C9[1]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp1C5->exp);_tmp1C9[0]=Cyc_Tcgenrep_cnst_int((
int)bound,loc);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1C9,sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(
dict,type,({struct Cyc_Absyn_Decl*_tmp1C7[1];_tmp1C7[0]=_tmp1C6;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1C7,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1C0,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp1C8[1];_tmp1C8[0]=_tmp1C5;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1C8,
sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL4C;}}}_LL4F: if((int)_tmp1BE
!= 0)goto _LL4C;_LL50: {struct _tuple6*_tmp1CB=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(char),4)));struct Cyc_Core_Opt*_tmp1CC=({struct Cyc_Core_Opt*
_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->v=Cyc_Tcgenrep_gen_vardecl(_tmp1CB,
Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str)),0,(void*)3,loc);_tmp1D5;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1CB,loc),loc),_tmp1CC,0);{struct _tuple18 _tmp1CE;
struct Cyc_Dict_Dict*_tmp1CF;struct Cyc_Tcgenrep_RepInfo*_tmp1D0;struct _tuple18*
_tmp1CD=Cyc_Tcgenrep_lookupRep(te,dict,loc,(void*)_tmp19F.elt_typ);_tmp1CE=*
_tmp1CD;_tmp1CF=_tmp1CE.f1;_tmp1D0=_tmp1CE.f2;dict=_tmp1CF;{struct Cyc_Absyn_Decl*
_tmp1D1=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str),_tmp1CB,({struct Cyc_Absyn_Exp*
_tmp1D4[1];_tmp1D4[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp1D0->exp);((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1D4,sizeof(
struct Cyc_Absyn_Exp*),1));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(dict,
type,({struct Cyc_Absyn_Decl*_tmp1D2[1];_tmp1D2[0]=_tmp1D1;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1D2,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1CB,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp1D3[1];_tmp1D3[0]=_tmp1D0;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1D3,
sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL4C;}}}_LL4C:;}goto _LL15;
_LL1E: if(_tmp19B <= (void*)3?1:*((int*)_tmp19B)!= 7)goto _LL20;_tmp1A0=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp19B)->f1;_tmp1A1=((struct Cyc_Absyn_ArrayType_struct*)
_tmp19B)->f2;_tmp1A2=((struct Cyc_Absyn_ArrayType_struct*)_tmp19B)->f3;_LL1F: if(
_tmp1A2 == 0)({void*_tmp1D6[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(_tag_arr("tcgenrep:At this point, array bounds must be constants",
sizeof(char),55),_tag_arr(_tmp1D6,sizeof(void*),0));});{struct _tuple18 _tmp1D8;
struct Cyc_Dict_Dict*_tmp1D9;struct Cyc_Tcgenrep_RepInfo*_tmp1DA;struct _tuple18*
_tmp1D7=Cyc_Tcgenrep_lookupRep(te,dict,loc,_tmp1A0);_tmp1D8=*_tmp1D7;_tmp1D9=
_tmp1D8.f1;_tmp1DA=_tmp1D8.f2;dict=_tmp1D9;{unsigned int bound=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp1A2));struct _tuple6*_tmp1DB=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",sizeof(char),4)));struct Cyc_Absyn_Decl*
_tmp1DC=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str),_tmp1DB,({struct Cyc_Absyn_Exp*
_tmp1DF[2];_tmp1DF[1]=(struct Cyc_Absyn_Exp*)_check_null(_tmp1DA->exp);_tmp1DF[0]=
Cyc_Tcgenrep_cnst_int((int)bound,loc);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp1DF,sizeof(struct Cyc_Absyn_Exp*),2));}),(
void*)0,loc);dict=Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*
_tmp1DD[1];_tmp1DD[0]=_tmp1DC;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1DD,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1DB,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp1DE[1];
_tmp1DE[0]=_tmp1DA;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1DE,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL15;}}_LL20:
if(_tmp19B <= (void*)3?1:*((int*)_tmp19B)!= 9)goto _LL22;_tmp1A3=((struct Cyc_Absyn_TupleType_struct*)
_tmp19B)->f1;_LL21: {struct _tuple6*_tmp1E0=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(char),4)));struct Cyc_Core_Opt*_tmp1E1=({struct Cyc_Core_Opt*
_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3->v=Cyc_Tcgenrep_gen_vardecl(_tmp1E0,
Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str)),0,(void*)3,loc);_tmp1E3;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1E0,loc),loc),_tmp1E1,0);{struct Cyc_List_List*
_tmp1E2=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcgenrep_get_second,_tmp1A3);dict=Cyc_Tcgenrep_buildRepTuple(
te,dict,loc,_tmp1E0,(void*)0,type,_tmp1E2);goto _LL15;}}_LL22: if(_tmp19B <= (void*)
3?1:*((int*)_tmp19B)!= 16)goto _LL24;_tmp1A4=((struct Cyc_Absyn_TypedefType_struct*)
_tmp19B)->f1;_tmp1A5=((struct Cyc_Absyn_TypedefType_struct*)_tmp19B)->f2;_tmp1A6=((
struct Cyc_Absyn_TypedefType_struct*)_tmp19B)->f3;_tmp1A7=((struct Cyc_Absyn_TypedefType_struct*)
_tmp19B)->f4;_LL23: if(_tmp1A7 == 0)({void*_tmp1E4[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): can't gen for abstract typedef",
sizeof(char),38),_tag_arr(_tmp1E4,sizeof(void*),0));});{struct _tuple18 _tmp1E6;
struct Cyc_Dict_Dict*_tmp1E7;struct Cyc_Tcgenrep_RepInfo*_tmp1E8;struct _tuple18*
_tmp1E5=Cyc_Tcgenrep_lookupRep(te,dict,loc,(void*)_tmp1A7->v);_tmp1E6=*_tmp1E5;
_tmp1E7=_tmp1E6.f1;_tmp1E8=_tmp1E6.f2;dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(_tmp1E7,type,_tmp1E8);
goto _LL15;}_LL24: if((int)_tmp19B != 0)goto _LL26;_LL25:({void*_tmp1E9[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("found void in gen() expression",sizeof(char),31),_tag_arr(_tmp1E9,
sizeof(void*),0));});goto _LL15;_LL26: if(_tmp19B <= (void*)3?1:*((int*)_tmp19B)!= 
0)goto _LL28;_LL27:({void*_tmp1EA[0]={};Cyc_Tcutil_terr(loc,_tag_arr("found evar in gen() expression",
sizeof(char),31),_tag_arr(_tmp1EA,sizeof(void*),0));});goto _LL15;_LL28: if(
_tmp19B <= (void*)3?1:*((int*)_tmp19B)!= 1)goto _LL2A;_tmp1A8=((struct Cyc_Absyn_VarType_struct*)
_tmp19B)->f1;_LL29:({void*_tmp1EB[0]={};Cyc_Tcutil_terr(loc,_tag_arr("found tyvar in gen() expression",
sizeof(char),32),_tag_arr(_tmp1EB,sizeof(void*),0));});goto _LL15;({void*_tmp1EC[
0]={};Cyc_Tcutil_terr(loc,_tag_arr("found variable type in gen() expression",
sizeof(char),40),_tag_arr(_tmp1EC,sizeof(void*),0));});goto _LL15;_LL2A: if(
_tmp19B <= (void*)3?1:*((int*)_tmp19B)!= 8)goto _LL2C;_tmp1A9=((struct Cyc_Absyn_FnType_struct*)
_tmp19B)->f1;_LL2B:({void*_tmp1ED[0]={};Cyc_Tcutil_terr(loc,_tag_arr("found function type in gen() expression",
sizeof(char),40),_tag_arr(_tmp1ED,sizeof(void*),0));});goto _LL15;_LL2C: if(
_tmp19B <= (void*)3?1:*((int*)_tmp19B)!= 15)goto _LL2E;_LL2D: goto _LL2F;_LL2E: if((
int)_tmp19B != 2)goto _LL30;_LL2F: goto _LL31;_LL30: if(_tmp19B <= (void*)3?1:*((int*)
_tmp19B)!= 19)goto _LL32;_LL31: goto _LL33;_LL32: if(_tmp19B <= (void*)3?1:*((int*)
_tmp19B)!= 20)goto _LL34;_LL33: goto _LL35;_LL34: if(_tmp19B <= (void*)3?1:*((int*)
_tmp19B)!= 21)goto _LL36;_LL35:({void*_tmp1EE[0]={};Cyc_Tcutil_terr(loc,_tag_arr("gen(): unhandled region, handle or effect type",
sizeof(char),47),_tag_arr(_tmp1EE,sizeof(void*),0));});goto _LL15;_LL36: if(
_tmp19B <= (void*)3?1:*((int*)_tmp19B)!= 2)goto _LL38;_tmp1AA=((struct Cyc_Absyn_TunionType_struct*)
_tmp19B)->f1;_tmp1AB=(void*)_tmp1AA.tunion_info;if(*((int*)_tmp1AB)!= 1)goto
_LL38;_tmp1AC=((struct Cyc_Absyn_KnownTunion_struct*)_tmp1AB)->f1;_tmp1AD=*
_tmp1AC;_tmp1AE=_tmp1AA.targs;_tmp1AF=(void*)_tmp1AA.rgn;_LL37: if(_tmp1AD->tvs != 
0)({void*_tmp1EF[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen(): tunion type variables not handled yet",sizeof(char),45),
_tag_arr(_tmp1EF,sizeof(void*),0));});if(_tmp1AD->fields == 0){struct _tuple6*
_tmp1F0=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp1F3=_cycalloc(sizeof(*
_tmp1F3));_tmp1F3[0]=Cyc_Tcgenrep_make_type_cstring(type);_tmp1F3;}));struct Cyc_Core_Opt*
_tmp1F1=({struct Cyc_Core_Opt*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1F0,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str)),0,(void*)3,loc);_tmp1F2;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1F0,loc),loc),_tmp1F1,1);goto _LL15;}if(!_tmp1AD->is_xtunion){
struct _tuple6*_tmp1F4=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp1FC=
_cycalloc(sizeof(*_tmp1FC));_tmp1FC[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp1FC;}));struct Cyc_Core_Opt*_tmp1F5=({struct Cyc_Core_Opt*_tmp1FB=_cycalloc(
sizeof(*_tmp1FB));_tmp1FB->v=Cyc_Tcgenrep_gen_vardecl(_tmp1F4,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tunion_str)),0,(void*)3,loc);_tmp1FB;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1F4,loc),loc),_tmp1F5,0);{struct Cyc_List_List*_tmp1F6=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1AD->fields))->v;struct Cyc_List_List*_tmp1F7=((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*
x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_tunionfield,_tmp1F6);struct Cyc_List_List*
_tmp1F8=((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),struct
Cyc_List_List*x))Cyc_List_filter)(Cyc_Tcgenrep_filter_nonempty_tunionfield,
_tmp1F6);struct Cyc_List_List*_tmp1F9=((struct Cyc_List_List*(*)(struct _tuple17*(*
f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_tunionfield_and_get_nmtype,
_tmp1F7);struct Cyc_List_List*_tmp1FA=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(
struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_tunionfield_and_get_name,
_tmp1F8);dict=Cyc_Tcgenrep_buildRepTunion(_tmp1AD->name,te,dict,loc,_tmp1F4,(
void*)2,type,_tmp1FA,_tmp1F9);}}else{struct _tuple6*_tmp1FD=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp203;}));struct Cyc_Core_Opt*_tmp1FE=({struct Cyc_Core_Opt*_tmp202=
_cycalloc(sizeof(*_tmp202));_tmp202->v=Cyc_Tcgenrep_gen_vardecl(_tmp1FD,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_xtunion_str)),0,(void*)3,loc);_tmp202;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1FD,loc),loc),_tmp1FE,0);{struct Cyc_List_List*_tmp1FF=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1AD->fields))->v;struct Cyc_List_List*_tmp200=((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*
x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_tunionfield,_tmp1FF);struct Cyc_List_List*
_tmp201=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_xtunionfield_and_get_name_type,
_tmp200);dict=Cyc_Tcgenrep_buildRepXTunion(_tmp1AD->name,te,dict,loc,_tmp1FD,(
void*)2,type,_tmp201);}}goto _LL15;_LL38: if(_tmp19B <= (void*)3?1:*((int*)_tmp19B)
!= 2)goto _LL3A;_LL39:({void*_tmp204[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunion must be resolved by now",
sizeof(char),38),_tag_arr(_tmp204,sizeof(void*),0));});_LL3A: if(_tmp19B <= (void*)
3?1:*((int*)_tmp19B)!= 3)goto _LL3C;_tmp1B0=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp19B)->f1;_LL3B: if(_tmp1B0.targs != 0)({void*_tmp205[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): tunionfield type parameters not handled yet",
sizeof(char),51),_tag_arr(_tmp205,sizeof(void*),0));});{void*_tmp206=(void*)
_tmp1B0.field_info;struct Cyc_Absyn_Tuniondecl*_tmp207;struct Cyc_Absyn_Tunionfield*
_tmp208;_LL52: if(*((int*)_tmp206)!= 1)goto _LL54;_tmp207=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp206)->f1;_tmp208=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp206)->f2;
_LL53: {struct Cyc_List_List*_tmp209=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_second,_tmp208->typs);
struct Cyc_List_List*_tmp20A=({struct Cyc_List_List*_tmp20C=_cycalloc(sizeof(*
_tmp20C));_tmp20C->hd=(void*)Cyc_Absyn_uint_typ;_tmp20C->tl=_tmp209;_tmp20C;});
struct _tuple6*_tmp20B=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep",sizeof(char),4)));dict=Cyc_Tcgenrep_buildRepTunionfield(_tmp207->name,
_tmp208->name,te,dict,loc,_tmp20B,(void*)0,type,_tmp20A);goto _LL51;}_LL54:;_LL55:({
void*_tmp20D[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("gen(): tunionfield must be known at this point",sizeof(char),47),
_tag_arr(_tmp20D,sizeof(void*),0));});_LL51:;}goto _LL15;_LL3C: if(_tmp19B <= (void*)
3?1:*((int*)_tmp19B)!= 10)goto _LL3E;_tmp1B1=((struct Cyc_Absyn_AggrType_struct*)
_tmp19B)->f1;_tmp1B2=(void*)_tmp1B1.aggr_info;_tmp1B3=_tmp1B1.targs;_LL3D: {
struct Cyc_Absyn_Aggrdecl*_tmp20E=Cyc_Absyn_get_known_aggrdecl(_tmp1B2);if(
_tmp20E->impl != 0?((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20E->impl))->exist_vars
!= 0: 0)({void*_tmp20F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("gen(): existential type variables not yet supported",
sizeof(char),52),_tag_arr(_tmp20F,sizeof(void*),0));});if((void*)_tmp20E->kind == (
void*)0){struct Cyc_Absyn_Aggrdecl*_tmp210=_tmp20E;if(_tmp210->impl == 0){struct
_tuple6*_tmp211=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp214=_cycalloc(
sizeof(*_tmp214));_tmp214[0]=Cyc_Tcgenrep_make_type_cstring(type);_tmp214;}));
struct Cyc_Core_Opt*_tmp212=({struct Cyc_Core_Opt*_tmp213=_cycalloc(sizeof(*
_tmp213));_tmp213->v=Cyc_Tcgenrep_gen_vardecl(_tmp211,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str)),0,(void*)3,loc);_tmp213;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp211,loc),loc),_tmp212,1);goto _LL15;}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp210->impl))->fields != 0?((int(*)(int(*pred)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp210->impl))->fields): 0){int _tmp215=((int(*)(int(*f)(int,struct
Cyc_Absyn_Aggrfield*),int accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,
0,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp210->impl))->fields);int
_tmp216=_tmp215 / 8 + (_tmp215 % 8 == 0?0: 1);struct Cyc_List_List*chars=0;{int i=0;
for(0;i < _tmp216;i ++){chars=({struct Cyc_List_List*_tmp217=_cycalloc(sizeof(*
_tmp217));_tmp217->hd=({struct _tuple9*_tmp218=_cycalloc(sizeof(*_tmp218));
_tmp218->f1=Cyc_Tcgenrep_tq_none;_tmp218->f2=Cyc_Absyn_char_typ;_tmp218;});
_tmp217->tl=chars;_tmp217;});}}{void*base_type=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp21D=_cycalloc(sizeof(*_tmp21D));_tmp21D[0]=({struct Cyc_Absyn_TupleType_struct
_tmp21E;_tmp21E.tag=9;_tmp21E.f1=chars;_tmp21E;});_tmp21D;});struct _tuple18
_tmp21A;struct Cyc_Dict_Dict*_tmp21B;struct Cyc_Tcgenrep_RepInfo*_tmp21C;struct
_tuple18*_tmp219=Cyc_Tcgenrep_lookupRep(te,dict,loc,base_type);_tmp21A=*_tmp219;
_tmp21B=_tmp21A.f1;_tmp21C=_tmp21A.f2;dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(_tmp21B,type,_tmp21C);}}
else{struct _tuple6*_tmp21F=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*
_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp224;}));struct Cyc_Core_Opt*_tmp220=({struct Cyc_Core_Opt*_tmp223=
_cycalloc(sizeof(*_tmp223));_tmp223->v=Cyc_Tcgenrep_gen_vardecl(_tmp21F,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str)),0,(void*)3,loc);_tmp223;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp21F,loc),loc),_tmp220,0);{struct Cyc_List_List*_tmp221=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp210->impl))->fields);dict=Cyc_Tcgenrep_buildRepStruct(({struct
Cyc_Core_Opt*_tmp222=_cycalloc(sizeof(*_tmp222));_tmp222->v=_tmp210->name;
_tmp222;}),te,dict,loc,_tmp21F,(void*)2,type,_tmp221);}}}else{struct Cyc_Absyn_Aggrdecl*
_tmp225=_tmp20E;if(_tmp225->tvs != 0)({void*_tmp226[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen: unions with parameters not yet supported",
sizeof(char),46),_tag_arr(_tmp226,sizeof(void*),0));});if(_tmp225->impl == 0){
struct _tuple6*_tmp227=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp22A=
_cycalloc(sizeof(*_tmp22A));_tmp22A[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp22A;}));struct Cyc_Core_Opt*_tmp228=({struct Cyc_Core_Opt*_tmp229=_cycalloc(
sizeof(*_tmp229));_tmp229->v=Cyc_Tcgenrep_gen_vardecl(_tmp227,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_union_str)),0,(void*)3,loc);_tmp229;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp227,loc),loc),_tmp228,1);goto _LL15;}{struct Cyc_List_List*_tmp22B=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp225->impl))->fields);struct _tuple6*_tmp22C=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp22E;}));dict=Cyc_Tcgenrep_buildRepUnion(({struct Cyc_Core_Opt*_tmp22D=
_cycalloc(sizeof(*_tmp22D));_tmp22D->v=_tmp225->name;_tmp22D;}),te,dict,loc,
_tmp22C,(void*)2,type,_tmp22B);}}goto _LL15;}_LL3E: if(_tmp19B <= (void*)3?1:*((int*)
_tmp19B)!= 11)goto _LL40;_tmp1B4=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp19B)->f1;if((int)_tmp1B4 != 0)goto _LL40;_tmp1B5=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp19B)->f2;_LL3F: if(_tmp1B5 != 0?((int(*)(int(*pred)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,_tmp1B5): 0){
int _tmp22F=((int(*)(int(*f)(int,struct Cyc_Absyn_Aggrfield*),int accum,struct Cyc_List_List*
x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,0,_tmp1B5);int _tmp230=
_tmp22F / 8 + (_tmp22F % 8 == 0?0: 1);void*base_type=(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp235=_cycalloc(sizeof(*_tmp235));_tmp235[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp236;_tmp236.tag=7;_tmp236.f1=(void*)Cyc_Absyn_char_typ;_tmp236.f2=Cyc_Tcgenrep_tq_none;
_tmp236.f3=(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_int(_tmp230,loc);_tmp236;});
_tmp235;});struct _tuple18 _tmp232;struct Cyc_Dict_Dict*_tmp233;struct Cyc_Tcgenrep_RepInfo*
_tmp234;struct _tuple18*_tmp231=Cyc_Tcgenrep_lookupRep(te,dict,loc,base_type);
_tmp232=*_tmp231;_tmp233=_tmp232.f1;_tmp234=_tmp232.f2;dict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(
_tmp233,type,_tmp234);}else{struct Cyc_List_List*_tmp237=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,_tmp1B5);struct _tuple6*_tmp238=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",sizeof(char),4)));dict=Cyc_Tcgenrep_buildRepStruct(
0,te,dict,loc,_tmp238,(void*)0,type,_tmp237);}goto _LL15;_LL40: if(_tmp19B <= (void*)
3?1:*((int*)_tmp19B)!= 11)goto _LL42;_tmp1B6=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp19B)->f1;if((int)_tmp1B6 != 1)goto _LL42;_tmp1B7=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp19B)->f2;_LL41: {struct Cyc_List_List*_tmp239=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,_tmp1B7);struct _tuple6*_tmp23A=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",sizeof(char),4)));dict=Cyc_Tcgenrep_buildRepUnion(
0,te,dict,loc,_tmp23A,(void*)0,type,_tmp239);goto _LL15;}_LL42: if(_tmp19B <= (void*)
3?1:*((int*)_tmp19B)!= 12)goto _LL44;_tmp1B8=((struct Cyc_Absyn_EnumType_struct*)
_tmp19B)->f2;_LL43: if(_tmp1B8 == 0)({void*_tmp23B[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen(): enum declaration must be present by now",
sizeof(char),47),_tag_arr(_tmp23B,sizeof(void*),0));});{struct Cyc_Absyn_Enumdecl
_tmp23C=*_tmp1B8;if(_tmp23C.fields == 0){struct _tuple6*_tmp23D=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp240=_cycalloc(sizeof(*_tmp240));_tmp240[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp240;}));struct Cyc_Core_Opt*_tmp23E=({struct Cyc_Core_Opt*_tmp23F=
_cycalloc(sizeof(*_tmp23F));_tmp23F->v=Cyc_Tcgenrep_gen_vardecl(_tmp23D,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_enum_str)),0,(void*)3,loc);_tmp23F;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp23D,loc),loc),_tmp23E,1);goto _LL15;}{struct Cyc_List_List*_tmp241=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_enumfield_tagnm,(struct Cyc_List_List*)(_tmp23C.fields)->v);
struct _tuple6*_tmp242=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp243=
_cycalloc(sizeof(*_tmp243));_tmp243[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp243;}));dict=Cyc_Tcgenrep_buildRepEnum((struct _tuple6**)& _tmp23C.name,te,
dict,loc,_tmp242,(void*)2,type,_tmp241);goto _LL15;}}_LL44: if(_tmp19B <= (void*)3?
1:*((int*)_tmp19B)!= 13)goto _LL46;_tmp1B9=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp19B)->f1;_LL45: {struct Cyc_List_List*_tmp244=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_enumfield_tagnm,_tmp1B9);struct _tuple6*_tmp245=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(_tag_arr("rep",sizeof(char),4)));dict=Cyc_Tcgenrep_buildRepEnum(
0,te,dict,loc,_tmp245,(void*)0,type,_tmp244);goto _LL15;}_LL46: if(_tmp19B <= (void*)
3?1:*((int*)_tmp19B)!= 14)goto _LL48;_LL47:({void*_tmp246[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen() for sizeof_t<-> not yet supported",
sizeof(char),40),_tag_arr(_tmp246,sizeof(void*),0));});_LL48: if(_tmp19B <= (void*)
3?1:*((int*)_tmp19B)!= 18)goto _LL4A;_LL49:({void*_tmp247[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen() for `i not yet supported",
sizeof(char),31),_tag_arr(_tmp247,sizeof(void*),0));});_LL4A: if(_tmp19B <= (void*)
3?1:*((int*)_tmp19B)!= 17)goto _LL15;_LL4B:({void*_tmp248[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("gen() for tag_t<-> not yet supported",
sizeof(char),37),_tag_arr(_tmp248,sizeof(void*),0));});_LL15:;}return({struct
_tuple18*_tmp249=_cycalloc(sizeof(*_tmp249));_tmp249->f1=dict;_tmp249->f2=((
struct Cyc_Tcgenrep_RepInfo*(*)(struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup)(
dict,type);_tmp249;});}static int Cyc_Tcgenrep_not_emitted_filter(struct Cyc_Tcgenrep_RepInfo*
ri){return ri->emitted == 0;}static void Cyc_Tcgenrep_mark_emitted(struct Cyc_Tcgenrep_RepInfo*
ri){ri->emitted=1;}struct _tuple21{struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;
struct Cyc_Absyn_Exp*f3;};struct _tuple21 Cyc_Tcgenrep_tcGenrep(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,void*type,struct Cyc_Dict_Dict*
dict){Cyc_Tcutil_check_valid_toplevel_type(loc,te,type);Cyc_Tcutil_check_valid_toplevel_type(
loc,te,type);{struct _tuple18 _tmp24B;struct Cyc_Dict_Dict*_tmp24C;struct Cyc_Tcgenrep_RepInfo*
_tmp24D;struct _tuple18*_tmp24A=Cyc_Tcgenrep_lookupRep(te,dict,loc,type);_tmp24B=*
_tmp24A;_tmp24C=_tmp24B.f1;_tmp24D=_tmp24B.f2;{struct Cyc_List_List*_tmp24E=Cyc_Tcgenrep_dfs(
_tmp24D);return({struct _tuple21 _tmp24F;_tmp24F.f1=_tmp24C;_tmp24F.f2=_tmp24E;
_tmp24F.f3=(struct Cyc_Absyn_Exp*)_check_null(_tmp24D->exp);_tmp24F;});}}}
