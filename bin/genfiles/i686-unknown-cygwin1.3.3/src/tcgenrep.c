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

// Decrease the upper bound on a fat pointer by numelts where sz is
// the size of the pointer's type.  Note that this can't be a macro
// if we're to get initializers right.
static struct _tagged_arr _tagged_ptr_decrease_size(struct _tagged_arr x,
                                                    unsigned int sz,
                                                    unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}

// Add i to zero-terminated pointer x.  Checks for x being null and
// ensures that x[0..i-1] are not 0.
#define _zero_arr_plus(orig_x,orig_sz,orig_i) ({ \
  typedef _czs_tx = (*orig_x); \
  _czs_tx *_czs_x = (_czs_tx *)(orig_x); \
  unsigned int _czs_sz = (orig_sz); \
  int _czs_i = (orig_i); \
  unsigned int _czs_temp; \
  if ((_czs_x) == 0) _throw_null(); \
  if (_czs_i < 0) _throw_arraybounds(); \
  for (_czs_temp=_czs_sz; _czs_temp < _czs_i; _czs_temp++) \
    if (_czs_x[_czs_temp] == 0) _throw_arraybounds(); \
  _czs_x+_czs_i; })

// Calculates the number of elements in a zero-terminated, thin array.
// If non-null, the array is guaranteed to have orig_offset elements.
#define _get_zero_arr_size(orig_x,orig_offset) ({ \
  typedef _gres_tx = (*orig_x); \
  _gres_tx *_gres_x = (_gres_tx *)(orig_x); \
  unsigned int _gres_offset = (orig_offset); \
  unsigned int _gres = 0; \
  if (_gres_x != 0) { \
     _gres = _gres_offset; \
     _gres_x += _gres_offset - 1; \
     while (*_gres_x != 0) { _gres_x++; _gres++; } \
  } _gres; })

// Does in-place addition of a zero-terminated pointer (x += e and ++x).  
// Note that this expands to call _zero_arr_plus.
#define _zero_arr_inplace_plus(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  *_zap_x = _zero_arr_plus(*_zap_x,1,(orig_i)); })

// Does in-place increment of a zero-terminated pointer (e.g., x++).
// Note that this expands to call _zero_arr_plus.
#define _zero_arr_inplace_plus_post(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  _zap_tx *_zap_res = *_zap_x; \
  *_zap_x = _zero_arr_plus(_zap_res,1,(orig_i)); \
  _zap_res; })
  
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
Cyc_Iter_Iter,void*);struct Cyc_Std___cycFILE;struct Cyc_Std_Cstdio___abstractFILE;
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,struct
_tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};int Cyc_Std_printf(struct
_tagged_arr,struct _tagged_arr);extern char Cyc_Std_FileCloseError[19];extern char
Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*tag;struct
_tagged_arr f1;};struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[
11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*
d,void*k);void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict*d);struct
_tuple1{void*f1;void*f2;};struct _tuple1*Cyc_Dict_rchoose(struct _RegionHandle*r,
struct Cyc_Dict_Dict*d);struct _tuple1*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_delete(struct Cyc_Dict_Dict*,
void*);unsigned int Cyc_Std_strlen(struct _tagged_arr s);int Cyc_Std_strcmp(struct
_tagged_arr s1,struct _tagged_arr s2);struct _tagged_arr Cyc_Std_strconcat(struct
_tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_Std_strdup(struct _tagged_arr
src);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*Cyc_Hashtable_create(
int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(struct
Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*
t,void*key);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr
line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(int,int);struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Typerep_Int_struct{
int tag;int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;
unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct
_tuple2{unsigned int f1;struct _tagged_arr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
int tag;struct _tagged_arr*f1;unsigned int f2;struct _tagged_arr f3;};struct _tuple3{
unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;
struct _tagged_arr f2;};struct _tuple4{unsigned int f1;struct _tagged_arr f2;};struct
Cyc_Typerep_TUnion_struct{int tag;struct _tagged_arr f1;struct _tagged_arr f2;struct
_tagged_arr f3;};struct Cyc_Typerep_TUnionField_struct{int tag;struct _tagged_arr f1;
struct _tagged_arr f2;unsigned int f3;struct _tagged_arr f4;};struct _tuple5{struct
_tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{int tag;struct
_tagged_arr f1;struct _tagged_arr f2;};struct Cyc_Typerep_Union_struct{int tag;struct
_tagged_arr*f1;int f2;struct _tagged_arr f3;};struct Cyc_Typerep_Enum_struct{int tag;
struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);struct Cyc_Std_timespec{int tv_sec;int tv_nsec;};struct Cyc_Std_itimerspec{
struct Cyc_Std_timespec it_interval;struct Cyc_Std_timespec it_value;};struct Cyc_Std_tm{
int tm_sec;int tm_min;int tm_hour;int tm_mday;int tm_mon;int tm_year;int tm_wday;int
tm_yday;int tm_isdst;};struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple6{
void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};
struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*
name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_PtrInfo{void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*
nullable;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownTunionInfo{struct _tuple6*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{
int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{
int tag;struct Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*
tunion_info;struct Cyc_List_List*targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple6*tunion_name;struct _tuple6*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple6*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
Cyc_Absyn_Aggrdecl**f1;};struct Cyc_Absyn_AggrInfo{void*aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;struct Cyc_Absyn_Conref*zero_term;};struct Cyc_Absyn_Evar_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;}
;struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_TunionType_struct{
int tag;struct Cyc_Absyn_TunionInfo f1;};struct Cyc_Absyn_TunionFieldType_struct{int
tag;struct Cyc_Absyn_TunionFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{int
tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{
int tag;struct _tuple6*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple6*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeInt_struct{int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{int tag;
void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_NonNullable_ps_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_Nullable_ps_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_TaggedArray_ps_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Regparm_att_struct{int tag;int
f1;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Carray_mod_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;void*f1;void*f2;struct Cyc_Absyn_Tqual f3;};struct Cyc_Absyn_Function_mod_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{
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
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
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
extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_star_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_tagged_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
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
_tagged_arr Cyc_Absynpp_typ2cstring(void*);struct _tuple9{unsigned int f1;int f2;};
struct _tuple9 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*
effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
void*type,struct Cyc_Tcgenrep_RepInfo*info){({struct Cyc_Std_Int_pa_struct _tmp4;
_tmp4.tag=1;_tmp4.f1=(int)((unsigned int)info->emitted);{struct Cyc_Std_Int_pa_struct
_tmp3;_tmp3.tag=1;_tmp3.f1=(unsigned int)info;{struct Cyc_Std_String_pa_struct
_tmp2;_tmp2.tag=0;_tmp2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(type);{void*
_tmp0[3]={& _tmp2,& _tmp3,& _tmp4};Cyc_Std_printf(({const char*_tmp1="(%s,%x:%d,";
_tag_arr(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,11));}),_tag_arr(_tmp0,
sizeof(void*),3));}}}});{struct Cyc_List_List*p=info->dependencies;for(0;p != 0;p=
p->tl){({struct Cyc_Std_Int_pa_struct _tmp7;_tmp7.tag=1;_tmp7.f1=(unsigned int)((
struct Cyc_Tcgenrep_RepInfo*)p->hd);{void*_tmp5[1]={& _tmp7};Cyc_Std_printf(({
const char*_tmp6="%x";_tag_arr(_tmp6,sizeof(char),_get_zero_arr_size(_tmp6,3));}),
_tag_arr(_tmp5,sizeof(void*),1));}});if(p->tl != 0)({void*_tmp8[0]={};Cyc_Std_printf(({
const char*_tmp9=",";_tag_arr(_tmp9,sizeof(char),_get_zero_arr_size(_tmp9,2));}),
_tag_arr(_tmp8,sizeof(void*),0));});}}({void*_tmpA[0]={};Cyc_Std_printf(({const
char*_tmpB=")\n";_tag_arr(_tmpB,sizeof(char),_get_zero_arr_size(_tmpB,3));}),
_tag_arr(_tmpA,sizeof(void*),0));});}void Cyc_Tcgenrep_print_typerep_dict(struct
Cyc_Dict_Dict*dict){((void(*)(void(*f)(void*,struct Cyc_Tcgenrep_RepInfo*),struct
Cyc_Dict_Dict*d))Cyc_Dict_iter)(Cyc_Tcgenrep_print_dict_entry,dict);}static int
Cyc_Tcgenrep_rephash(struct Cyc_Tcgenrep_RepInfo*ri){return(int)ri;}static int Cyc_Tcgenrep_repcmp(
struct Cyc_Tcgenrep_RepInfo*r1,struct Cyc_Tcgenrep_RepInfo*r2){unsigned int r1p;
unsigned int r2p;r1p=(unsigned int)r1;r2p=(unsigned int)r2;if(r1 < r2)return - 1;
else{if(r1 == r2)return 0;else{return 1;}}}static struct Cyc_Absyn_Tunionfield*Cyc_Tcgenrep_getField(
struct Cyc_Absyn_Tuniondecl*td,struct _tuple6*fieldname){if(td->fields == 0)({void*
_tmpC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmpD="Could not find field in tuniondecl";_tag_arr(_tmpD,sizeof(char),
_get_zero_arr_size(_tmpD,35));}),_tag_arr(_tmpC,sizeof(void*),0));});else{struct
Cyc_List_List*l=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
for(0;l != 0;l=l->tl){if(!Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Tunionfield*)l->hd)->name,
fieldname))return(struct Cyc_Absyn_Tunionfield*)l->hd;}}({void*_tmpE[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpF="Could not find field in tuniondecl";
_tag_arr(_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,35));}),_tag_arr(_tmpE,
sizeof(void*),0));});}static char _tmp10[9]="Tcgenrep";static struct _tagged_arr Cyc_Tcgenrep_tcgenrep_nm={
_tmp10,_tmp10,_tmp10 + 9};static struct Cyc_List_List Cyc_Tcgenrep_l={(void*)& Cyc_Tcgenrep_tcgenrep_nm,
0};static struct Cyc_Absyn_Abs_n_struct Cyc_Tcgenrep_tcgenrep_nmspace={1,(struct Cyc_List_List*)&
Cyc_Tcgenrep_l};static void*Cyc_Tcgenrep_tcgenrep_nmspace2=(void*)& Cyc_Tcgenrep_tcgenrep_nmspace;
static char _tmp12[8]="Typerep";static struct _tagged_arr Cyc_Tcgenrep_typerep_nm={
_tmp12,_tmp12,_tmp12 + 8};static struct Cyc_List_List Cyc_Tcgenrep_l2={(void*)& Cyc_Tcgenrep_typerep_nm,
0};static struct Cyc_Absyn_Abs_n_struct Cyc_Tcgenrep_typerep_nmspace={1,(struct Cyc_List_List*)&
Cyc_Tcgenrep_l2};static struct Cyc_List_List*Cyc_Tcgenrep_dfsvisit(struct Cyc_Tcgenrep_RepInfo*
ri,struct Cyc_Hashtable_Table*visited){struct _handler_cons _tmp14;_push_handler(&
_tmp14);{int _tmp16=0;if(setjmp(_tmp14.handler))_tmp16=1;if(!_tmp16){((int(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Tcgenrep_RepInfo*key))Cyc_Hashtable_lookup)(
visited,ri);{struct Cyc_List_List*_tmp17=0;_npop_handler(0);return _tmp17;};
_pop_handler();}else{void*_tmp15=(void*)_exn_thrown;void*_tmp19=_tmp15;_LL1: if(
_tmp19 != Cyc_Core_Not_found)goto _LL3;_LL2:((void(*)(struct Cyc_Hashtable_Table*t,
struct Cyc_Tcgenrep_RepInfo*key,int val))Cyc_Hashtable_insert)(visited,ri,1);{
struct Cyc_List_List*ds=0;{struct Cyc_List_List*l=ri->dependencies;for(0;l != 0;l=l->tl){
ds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
ds,Cyc_Tcgenrep_dfsvisit((struct Cyc_Tcgenrep_RepInfo*)l->hd,visited));}}if(ri->emitted
== 0){if(ri->fwd_decl != 0)ds=({struct Cyc_List_List*_tmp1A=_cycalloc(sizeof(*
_tmp1A));_tmp1A->hd=({struct Cyc_Absyn_Decl*_tmp1B[1];_tmp1B[0]=(struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*)_check_null(ri->fwd_decl))->v;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1B,sizeof(struct Cyc_Absyn_Decl*),
1));});_tmp1A->tl=ds;_tmp1A;});{struct Cyc_List_List*_tmp1C=({struct Cyc_List_List*
_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->hd=ri->decls;_tmp1D->tl=0;_tmp1D;});ri->emitted=
1;return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(ds,_tmp1C);}}else{return ds;}}_LL3:;_LL4:(void)_throw(_tmp19);
_LL0:;}}}static struct Cyc_List_List*Cyc_Tcgenrep_dfs(struct Cyc_Tcgenrep_RepInfo*
ri){struct Cyc_Hashtable_Table*tab=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(
struct Cyc_Tcgenrep_RepInfo*,struct Cyc_Tcgenrep_RepInfo*),int(*hash)(struct Cyc_Tcgenrep_RepInfo*)))
Cyc_Hashtable_create)(53,Cyc_Tcgenrep_repcmp,Cyc_Tcgenrep_rephash);struct Cyc_List_List*
ds=Cyc_Tcgenrep_dfsvisit(ri,tab);return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_flatten)(ds);}static char _tmp1E[11]="Typestruct";static struct
_tagged_arr Cyc_Tcgenrep_typestruct_str={_tmp1E,_tmp1E,_tmp1E + 11};static char
_tmp1F[4]="Var";static struct _tagged_arr Cyc_Tcgenrep_var_str={_tmp1F,_tmp1F,
_tmp1F + 4};static char _tmp20[4]="Int";static struct _tagged_arr Cyc_Tcgenrep_int_str={
_tmp20,_tmp20,_tmp20 + 4};static char _tmp21[6]="Float";static struct _tagged_arr Cyc_Tcgenrep_float_str={
_tmp21,_tmp21,_tmp21 + 6};static char _tmp22[7]="Double";static struct _tagged_arr Cyc_Tcgenrep_double_str={
_tmp22,_tmp22,_tmp22 + 7};static char _tmp23[8]="ThinPtr";static struct _tagged_arr
Cyc_Tcgenrep_thinptr_str={_tmp23,_tmp23,_tmp23 + 8};static char _tmp24[7]="FatPtr";
static struct _tagged_arr Cyc_Tcgenrep_fatptr_str={_tmp24,_tmp24,_tmp24 + 7};static
char _tmp25[6]="Tuple";static struct _tagged_arr Cyc_Tcgenrep_tuple_str={_tmp25,
_tmp25,_tmp25 + 6};static char _tmp26[12]="TUnionField";static struct _tagged_arr Cyc_Tcgenrep_tunionfield_str={
_tmp26,_tmp26,_tmp26 + 12};static char _tmp27[7]="Struct";static struct _tagged_arr
Cyc_Tcgenrep_struct_str={_tmp27,_tmp27,_tmp27 + 7};static char _tmp28[7]="TUnion";
static struct _tagged_arr Cyc_Tcgenrep_tunion_str={_tmp28,_tmp28,_tmp28 + 7};static
char _tmp29[8]="XTUnion";static struct _tagged_arr Cyc_Tcgenrep_xtunion_str={_tmp29,
_tmp29,_tmp29 + 8};static char _tmp2A[6]="Union";static struct _tagged_arr Cyc_Tcgenrep_union_str={
_tmp2A,_tmp2A,_tmp2A + 6};static char _tmp2B[5]="Enum";static struct _tagged_arr Cyc_Tcgenrep_enum_str={
_tmp2B,_tmp2B,_tmp2B + 5};static char _tmp2C[7]="name_t";static struct _tagged_arr Cyc_Tcgenrep_name_t_str={
_tmp2C,_tmp2C,_tmp2C + 7};static struct _tuple6*Cyc_Tcgenrep_typerep_name(struct
_tagged_arr*name){return({struct _tuple6*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->f1=(
void*)& Cyc_Tcgenrep_typerep_nmspace;_tmp2D->f2=name;_tmp2D;});}static int Cyc_Tcgenrep_gen_id_counter=
0;static struct _tagged_arr*Cyc_Tcgenrep_new_gen_id(struct _tagged_arr name){return({
struct _tagged_arr*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E[0]=(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp32;_tmp32.tag=1;_tmp32.f1=(int)((unsigned int)Cyc_Tcgenrep_gen_id_counter
++);{struct Cyc_Std_String_pa_struct _tmp31;_tmp31.tag=0;_tmp31.f1=(struct
_tagged_arr)name;{void*_tmp2F[2]={& _tmp31,& _tmp32};Cyc_Std_aprintf(({const char*
_tmp30="_gen%s_%d";_tag_arr(_tmp30,sizeof(char),_get_zero_arr_size(_tmp30,10));}),
_tag_arr(_tmp2F,sizeof(void*),2));}}});_tmp2E;});}static void Cyc_Tcgenrep_print_params(
struct Cyc_List_List*l){({void*_tmp33[0]={};Cyc_Std_printf(({const char*_tmp34="<";
_tag_arr(_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,2));}),_tag_arr(_tmp33,
sizeof(void*),0));});{struct Cyc_List_List*p=l;for(0;p != 0;p=p->tl){({struct Cyc_Std_Int_pa_struct
_tmp38;_tmp38.tag=1;_tmp38.f1=(int)((unsigned int)((int)(p->tl != 0?',':' ')));{
struct Cyc_Std_String_pa_struct _tmp37;_tmp37.tag=0;_tmp37.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string((void*)p->hd);{void*_tmp35[2]={& _tmp37,& _tmp38};Cyc_Std_printf(({
const char*_tmp36="%s%c";_tag_arr(_tmp36,sizeof(char),_get_zero_arr_size(_tmp36,5));}),
_tag_arr(_tmp35,sizeof(void*),2));}}});}}({void*_tmp39[0]={};Cyc_Std_printf(({
const char*_tmp3A=">\n";_tag_arr(_tmp3A,sizeof(char),_get_zero_arr_size(_tmp3A,3));}),
_tag_arr(_tmp39,sizeof(void*),0));});}static void Cyc_Tcgenrep_print_tvars(struct
Cyc_List_List*l){({void*_tmp3B[0]={};Cyc_Std_printf(({const char*_tmp3C="<";
_tag_arr(_tmp3C,sizeof(char),_get_zero_arr_size(_tmp3C,2));}),_tag_arr(_tmp3B,
sizeof(void*),0));});{struct Cyc_List_List*p=l;for(0;p != 0;p=p->tl){({struct Cyc_Std_Int_pa_struct
_tmp40;_tmp40.tag=1;_tmp40.f1=(int)((unsigned int)((int)(p->tl != 0?',':' ')));{
struct Cyc_Std_String_pa_struct _tmp3F;_tmp3F.tag=0;_tmp3F.f1=(struct _tagged_arr)*((
struct _tagged_arr*)((struct Cyc_Absyn_Tvar*)p->hd)->name);{void*_tmp3D[2]={&
_tmp3F,& _tmp40};Cyc_Std_printf(({const char*_tmp3E="%s%c";_tag_arr(_tmp3E,sizeof(
char),_get_zero_arr_size(_tmp3E,5));}),_tag_arr(_tmp3D,sizeof(void*),2));}}});}}({
void*_tmp41[0]={};Cyc_Std_printf(({const char*_tmp42=">\n";_tag_arr(_tmp42,
sizeof(char),_get_zero_arr_size(_tmp42,3));}),_tag_arr(_tmp41,sizeof(void*),0));});}
static struct Cyc_Absyn_Rel_n_struct Cyc_Tcgenrep_top_nm_0={0,0};static void*Cyc_Tcgenrep_top_nm=(
void*)& Cyc_Tcgenrep_top_nm_0;static struct _tuple6*Cyc_Tcgenrep_toplevel_name(
struct _tagged_arr*name){return({struct _tuple6*_tmp44=_cycalloc(sizeof(*_tmp44));
_tmp44->f1=Cyc_Tcgenrep_top_nm;_tmp44->f2=(struct _tagged_arr*)name;_tmp44;});}
static struct _tuple6*Cyc_Tcgenrep_relative_name(struct Cyc_List_List*pathl,struct
_tagged_arr*name){struct Cyc_Absyn_Rel_n_struct*path=({struct Cyc_Absyn_Rel_n_struct*
_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46[0]=({struct Cyc_Absyn_Rel_n_struct _tmp47;
_tmp47.tag=0;_tmp47.f1=pathl;_tmp47;});_tmp46;});return({struct _tuple6*_tmp45=
_cycalloc(sizeof(*_tmp45));_tmp45->f1=(void*)path;_tmp45->f2=(struct _tagged_arr*)
name;_tmp45;});}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl(struct
_tagged_arr*name,void*type,struct Cyc_Absyn_Exp*init,struct Cyc_Position_Segment*
seg){struct _tuple6*qvar=Cyc_Tcgenrep_toplevel_name(name);struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(qvar,type,init);(void*)(vd->sc=(void*)((void*)0));{
struct Cyc_Absyn_Var_d_struct*r1=({struct Cyc_Absyn_Var_d_struct*_tmp48=_cycalloc(
sizeof(*_tmp48));_tmp48[0]=({struct Cyc_Absyn_Var_d_struct _tmp49;_tmp49.tag=0;
_tmp49.f1=vd;_tmp49;});_tmp48;});void*r2=(void*)r1;return Cyc_Absyn_new_decl(r2,
seg);}}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_vardecl(struct _tuple6*name,
void*type,struct Cyc_Absyn_Exp*init,void*sc,struct Cyc_Position_Segment*seg){
struct _tuple6*topname=Cyc_Tcgenrep_toplevel_name((*((struct _tuple6*)name)).f2);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(topname,type,init);(void*)(vd->sc=(
void*)sc);{struct Cyc_Absyn_Var_d_struct*r1=({struct Cyc_Absyn_Var_d_struct*_tmp4A=
_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Absyn_Var_d_struct _tmp4B;_tmp4B.tag=
0;_tmp4B.f1=vd;_tmp4B;});_tmp4A;});void*r2=(void*)r1;return Cyc_Absyn_new_decl(r2,
seg);}}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_string(struct _tagged_arr s,
struct Cyc_Position_Segment*seg){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_String_c_struct*
_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=({struct Cyc_Absyn_String_c_struct
_tmp4D;_tmp4D.tag=5;_tmp4D.f1=s;_tmp4D;});_tmp4C;}),seg);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_string_cls(struct Cyc_Position_Segment*seg,struct _tagged_arr*s){
return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_String_c_struct*_tmp4E=
_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({struct Cyc_Absyn_String_c_struct _tmp4F;
_tmp4F.tag=5;_tmp4F.f1=*((struct _tagged_arr*)s);_tmp4F;});_tmp4E;}),seg);}static
struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_qvar_string_cls(struct Cyc_Position_Segment*
seg,struct _tuple6*s){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_String_c_struct*
_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50[0]=({struct Cyc_Absyn_String_c_struct
_tmp51;_tmp51.tag=5;_tmp51.f1=*((struct _tagged_arr*)(*((struct _tuple6*)s)).f2);
_tmp51;});_tmp50;}),seg);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int(int i,
struct Cyc_Position_Segment*seg){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*
_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52[0]=({struct Cyc_Absyn_Int_c_struct _tmp53;
_tmp53.tag=2;_tmp53.f1=(void*)((void*)0);_tmp53.f2=i;_tmp53;});_tmp52;}),seg);}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int_cls(struct Cyc_Position_Segment*
seg,int i){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*_tmp54=
_cycalloc(sizeof(*_tmp54));_tmp54[0]=({struct Cyc_Absyn_Int_c_struct _tmp55;_tmp55.tag=
2;_tmp55.f1=(void*)((void*)0);_tmp55.f2=i;_tmp55;});_tmp54;}),seg);}static int Cyc_Tcgenrep_size_of2int(
void*sz){void*_tmp56=sz;_LL6: if((int)_tmp56 != 0)goto _LL8;_LL7: return 8;_LL8: if((
int)_tmp56 != 1)goto _LLA;_LL9: return 16;_LLA: if((int)_tmp56 != 2)goto _LLC;_LLB:
return 32;_LLC: if((int)_tmp56 != 3)goto _LL5;_LLD: return 64;_LL5:;}static void*Cyc_Tcgenrep_tunion_typ(
struct _tuple6*name){return(void*)({struct Cyc_Absyn_TunionType_struct*_tmp57=
_cycalloc(sizeof(*_tmp57));_tmp57[0]=({struct Cyc_Absyn_TunionType_struct _tmp58;
_tmp58.tag=2;_tmp58.f1=({struct Cyc_Absyn_TunionInfo _tmp59;_tmp59.tunion_info=(
void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*_tmp5A=_cycalloc(sizeof(*
_tmp5A));_tmp5A[0]=({struct Cyc_Absyn_UnknownTunion_struct _tmp5B;_tmp5B.tag=0;
_tmp5B.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp5C;_tmp5C.name=name;_tmp5C.is_xtunion=
0;_tmp5C;});_tmp5B;});_tmp5A;}));_tmp59.targs=0;_tmp59.rgn=(void*)((void*)2);
_tmp59;});_tmp58;});_tmp57;});}static void*Cyc_Tcgenrep_tunionfield_typ(struct
_tuple6*name,struct _tuple6*fieldname){return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp5E;_tmp5E.tag=3;_tmp5E.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp5F;_tmp5F.field_info=(
void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*_tmp60=_cycalloc(
sizeof(*_tmp60));_tmp60[0]=({struct Cyc_Absyn_UnknownTunionfield_struct _tmp61;
_tmp61.tag=0;_tmp61.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp62;_tmp62.tunion_name=
name;_tmp62.field_name=fieldname;_tmp62.is_xtunion=0;_tmp62;});_tmp61;});_tmp60;}));
_tmp5F.targs=0;_tmp5F;});_tmp5E;});_tmp5D;});}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_call_exp(
struct _tuple6*name,struct Cyc_List_List*args,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_unknowncall_exp(Cyc_Absyn_unknownid_exp(name,loc),args,loc);}
static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_tunion_constructor_decl(struct _tuple6*
tunionname,struct _tuple6*fieldname,struct _tuple6*varname,struct Cyc_List_List*
args,void*sc,struct Cyc_Position_Segment*loc){void*_tmp63=Cyc_Tcgenrep_tunionfield_typ(
tunionname,fieldname);struct Cyc_Absyn_Exp*_tmp64=Cyc_Tcgenrep_call_exp(fieldname,
args,loc);struct Cyc_Absyn_Decl*_tmp65=Cyc_Tcgenrep_gen_vardecl(varname,_tmp63,(
struct Cyc_Absyn_Exp*)_tmp64,sc,loc);return _tmp65;}struct _tuple10{struct Cyc_Absyn_Tqual
f1;void*f2;};static void*Cyc_Tcgenrep_get_second(struct _tuple10*pair){return(*((
struct _tuple10*)pair)).f2;}struct _tuple11{struct _tagged_arr*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tagged_arr*Cyc_Tcgenrep_get_first(struct _tuple11*pair){return(*((
struct _tuple11*)pair)).f1;}static char _tmp66[5]="list";static struct _tagged_arr Cyc_Tcgenrep_list_str={
_tmp66,_tmp66,_tmp66 + 5};static char _tmp67[5]="List";static struct _tagged_arr Cyc_Tcgenrep_List_str={
_tmp67,_tmp67,_tmp67 + 5};struct _tuple12{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple2_exp_cls(struct Cyc_Position_Segment*
loc,struct _tuple12*es){struct _tuple12 _tmp69;struct Cyc_Absyn_Exp*_tmp6A;struct Cyc_Absyn_Exp*
_tmp6B;struct _tuple12*_tmp68=es;_tmp69=*_tmp68;_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;
return Cyc_Absyn_tuple_exp(({struct Cyc_Absyn_Exp*_tmp6C[2];_tmp6C[1]=_tmp6B;
_tmp6C[0]=_tmp6A;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp6C,sizeof(struct Cyc_Absyn_Exp*),2));}),loc);}struct _tuple13{struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_tuple3_exp_cls(struct Cyc_Position_Segment*loc,struct _tuple13*es){
struct _tuple13 _tmp6E;struct Cyc_Absyn_Exp*_tmp6F;struct Cyc_Absyn_Exp*_tmp70;
struct Cyc_Absyn_Exp*_tmp71;struct _tuple13*_tmp6D=es;_tmp6E=*_tmp6D;_tmp6F=_tmp6E.f1;
_tmp70=_tmp6E.f2;_tmp71=_tmp6E.f3;return Cyc_Absyn_tuple_exp(({struct Cyc_Absyn_Exp*
_tmp72[3];_tmp72[2]=_tmp71;_tmp72[1]=_tmp70;_tmp72[0]=_tmp6F;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp72,sizeof(struct Cyc_Absyn_Exp*),3));}),
loc);}static char _tmp73[5]="NULL";static struct _tagged_arr Cyc_Tcgenrep_null_str={
_tmp73,_tmp73,_tmp73 + 5};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_list_exp(struct
Cyc_List_List*l,struct Cyc_Position_Segment*loc){if(l == 0)return Cyc_Absyn_null_exp(
loc);return Cyc_Tcgenrep_call_exp(Cyc_Tcgenrep_toplevel_name(& Cyc_Tcgenrep_list_str),
l,loc);}struct _tuple14{void*f1;struct Cyc_Position_Segment*f2;};static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_make_offsetof_exp(struct _tuple14*typeloc,int index){struct _tuple14
_tmp75;void*_tmp76;struct Cyc_Position_Segment*_tmp77;struct _tuple14*_tmp74=
typeloc;_tmp75=*_tmp74;_tmp76=_tmp75.f1;_tmp77=_tmp75.f2;return Cyc_Absyn_offsetof_exp(
_tmp76,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp78=_cycalloc_atomic(
sizeof(*_tmp78));_tmp78[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp79;_tmp79.tag=
1;_tmp79.f1=(unsigned int)index;_tmp79;});_tmp78;}),_tmp77);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_get_and_cast_ri_exp(struct Cyc_Position_Segment*loc,struct Cyc_Tcgenrep_RepInfo*
info){return Cyc_Absyn_cast_exp(Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str)),(struct Cyc_Absyn_Exp*)_check_null(info->exp),loc);}
static struct _tuple11*Cyc_Tcgenrep_gen_id_for_exp(struct Cyc_Absyn_Exp*e){return({
struct _tuple11*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->f1=Cyc_Tcgenrep_new_gen_id(({
const char*_tmp7B="tuple";_tag_arr(_tmp7B,sizeof(char),_get_zero_arr_size(_tmp7B,
6));}));_tmp7A->f2=e;_tmp7A;});}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl_cls(
struct _tuple14*env,struct _tuple11*name_exp){struct _tuple14 _tmp7D;void*_tmp7E;
struct Cyc_Position_Segment*_tmp7F;struct _tuple14*_tmp7C=env;_tmp7D=*_tmp7C;
_tmp7E=_tmp7D.f1;_tmp7F=_tmp7D.f2;{struct _tuple11 _tmp81;struct _tagged_arr*_tmp82;
struct Cyc_Absyn_Exp*_tmp83;struct _tuple11*_tmp80=name_exp;_tmp81=*_tmp80;_tmp82=
_tmp81.f1;_tmp83=_tmp81.f2;return Cyc_Tcgenrep_gen_decl(_tmp82,_tmp7E,(struct Cyc_Absyn_Exp*)
_tmp83,_tmp7F);}}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
static struct _tuple15*Cyc_Tcgenrep_null_designator_exp(struct Cyc_Absyn_Exp*e){
return({struct _tuple15*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->f1=0;_tmp84->f2=
e;_tmp84;});}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_arr_init_exp(struct Cyc_List_List*
l,struct Cyc_Position_Segment*loc){struct Cyc_List_List*_tmp85=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_null_designator_exp,l);return Cyc_Absyn_unresolvedmem_exp(0,_tmp85,
loc);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_address_exp_cls(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_address_exp(e,loc);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_unknownid_exp_cls(struct Cyc_Position_Segment*loc,struct _tuple6*e){
return Cyc_Absyn_unknownid_exp(e,loc);}static int Cyc_Tcgenrep_has_bitfield(struct
Cyc_Absyn_Aggrfield*sf){return sf->width != 0;}static int Cyc_Tcgenrep_add_bitfield_sizes(
int total,struct Cyc_Absyn_Aggrfield*sf){unsigned int _tmp87;int _tmp88;struct
_tuple9 _tmp86=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(sf->width));
_tmp87=_tmp86.f1;_tmp88=_tmp86.f2;if(!_tmp88)({void*_tmp89[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8A="add_bitfield_sizes: sizeof or offsetof in bitfield size";
_tag_arr(_tmp8A,sizeof(char),_get_zero_arr_size(_tmp8A,56));}),_tag_arr(_tmp89,
sizeof(void*),0));});return(int)(_tmp87 + total);}static void*Cyc_Tcgenrep_select_structfield_type(
struct Cyc_Absyn_Aggrfield*sf){if(Cyc_Std_strcmp(*((struct _tagged_arr*)sf->name),({
const char*_tmp8B="";_tag_arr(_tmp8B,sizeof(char),_get_zero_arr_size(_tmp8B,1));}))
== 0)({void*_tmp8C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp8D="gen(): anonymous (padding) structfield not handled yet";
_tag_arr(_tmp8D,sizeof(char),_get_zero_arr_size(_tmp8D,55));}),_tag_arr(_tmp8C,
sizeof(void*),0));});if(Cyc_Tcgenrep_has_bitfield(sf))({void*_tmp8E[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8F="gen(): mixed struct bitfields and union bitfields not handled";
_tag_arr(_tmp8F,sizeof(char),_get_zero_arr_size(_tmp8F,62));}),_tag_arr(_tmp8E,
sizeof(void*),0));});return(void*)sf->type;}struct _tuple16{struct _tagged_arr*f1;
void*f2;};static struct _tuple16*Cyc_Tcgenrep_select_structfield_nmtype(struct Cyc_Absyn_Aggrfield*
sf){if(Cyc_Std_strcmp(*((struct _tagged_arr*)sf->name),({const char*_tmp90="";
_tag_arr(_tmp90,sizeof(char),_get_zero_arr_size(_tmp90,1));}))== 0)({void*_tmp91[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp92="gen(): anonymous (padding) structfield not handled yet";
_tag_arr(_tmp92,sizeof(char),_get_zero_arr_size(_tmp92,55));}),_tag_arr(_tmp91,
sizeof(void*),0));});if(Cyc_Tcgenrep_has_bitfield(sf))({void*_tmp93[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp94="gen(): mixed struct bitfields and union bitfields not handled";
_tag_arr(_tmp94,sizeof(char),_get_zero_arr_size(_tmp94,62));}),_tag_arr(_tmp93,
sizeof(void*),0));});return({struct _tuple16*_tmp95=_cycalloc(sizeof(*_tmp95));
_tmp95->f1=sf->name;_tmp95->f2=(void*)sf->type;_tmp95;});}struct _tuple17{int f1;
struct _tuple6*f2;};struct _tuple17*Cyc_Tcgenrep_select_enumfield_tagnm(struct Cyc_Absyn_Enumfield*
ef){if(ef->tag == 0)({void*_tmp96[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp97="Enum tag exp should be filled in by now";
_tag_arr(_tmp97,sizeof(char),_get_zero_arr_size(_tmp97,40));}),_tag_arr(_tmp96,
sizeof(void*),0));});return({struct _tuple17*_tmp98=_cycalloc(sizeof(*_tmp98));
_tmp98->f1=(int)(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
ef->tag))).f1;_tmp98->f2=ef->name;_tmp98;});}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_update_info(
struct Cyc_Dict_Dict*dict,void*type,struct Cyc_List_List*decls,struct Cyc_Absyn_Exp*
exp,struct Cyc_List_List*dependencies,struct Cyc_Core_Opt*fwd_decl){struct Cyc_Core_Opt*
_tmp99=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(
dict,type);if(_tmp99 != 0){if(((struct Cyc_Tcgenrep_RepInfo*)_tmp99->v)->decls != 0){
Cyc_Tcgenrep_print_typerep_dict(dict);({void*_tmp9A[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp9B="Updating non-forward declaration";
_tag_arr(_tmp9B,sizeof(char),_get_zero_arr_size(_tmp9B,33));}),_tag_arr(_tmp9A,
sizeof(void*),0));});}((struct Cyc_Tcgenrep_RepInfo*)_tmp99->v)->decls=decls;((
struct Cyc_Tcgenrep_RepInfo*)_tmp99->v)->exp=exp;((struct Cyc_Tcgenrep_RepInfo*)
_tmp99->v)->dependencies=dependencies;return dict;}else{return((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(dict,
type,({struct Cyc_Tcgenrep_RepInfo*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->decls=
decls;_tmp9C->exp=exp;_tmp9C->dependencies=dependencies;_tmp9C->fwd_decl=
fwd_decl;_tmp9C->emitted=0;_tmp9C->is_extern=0;_tmp9C;}));}}static struct Cyc_Dict_Dict*
Cyc_Tcgenrep_make_fwd_decl_info(struct Cyc_Dict_Dict*dict,void*type,struct Cyc_Absyn_Exp*
exp,struct Cyc_Core_Opt*fwd_decl,int is_extern){struct Cyc_Core_Opt*_tmp9D=((struct
Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(dict,type);
if(_tmp9D != 0){({struct Cyc_Std_String_pa_struct _tmpA0;_tmpA0.tag=0;_tmpA0.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(type);{void*_tmp9E[1]={& _tmpA0};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp9F="Repinfo for fwd declaration of %s already exists";
_tag_arr(_tmp9F,sizeof(char),_get_zero_arr_size(_tmp9F,49));}),_tag_arr(_tmp9E,
sizeof(void*),1));}});return dict;}else{return((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(dict,type,({struct Cyc_Tcgenrep_RepInfo*
_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->decls=0;_tmpA1->exp=exp;_tmpA1->dependencies=
0;_tmpA1->fwd_decl=fwd_decl;_tmpA1->emitted=0;_tmpA1->is_extern=is_extern;_tmpA1;}));}}
static struct Cyc_Absyn_Tqual Cyc_Tcgenrep_tq_none={0,0,0};static struct _tuple10*Cyc_Tcgenrep_tqual_type(
struct Cyc_Absyn_Tqual*tq,void*type){return({struct _tuple10*_tmpA2=_cycalloc(
sizeof(*_tmpA2));_tmpA2->f1=*((struct Cyc_Absyn_Tqual*)_check_null(tq));_tmpA2->f2=
type;_tmpA2;});}static void*Cyc_Tcgenrep_tuple_typ(struct Cyc_List_List*types){
struct Cyc_List_List*_tmpA3=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct
Cyc_Absyn_Tqual*,void*),struct Cyc_Absyn_Tqual*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tqual_type,(struct Cyc_Absyn_Tqual*)& Cyc_Tcgenrep_tq_none,types);
struct Cyc_Absyn_TupleType_struct*tuple_type_base=({struct Cyc_Absyn_TupleType_struct*
_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({struct Cyc_Absyn_TupleType_struct
_tmpA5;_tmpA5.tag=9;_tmpA5.f1=_tmpA3;_tmpA5;});_tmpA4;});void*tuple_type=(void*)
tuple_type_base;return tuple_type;}static struct _tuple15*Cyc_Tcgenrep_array_decls(
void*type,struct Cyc_List_List*exps,struct Cyc_Position_Segment*loc){struct Cyc_List_List*
_tmpA6=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_gen_id_for_exp,exps);struct Cyc_List_List*
_tmpA7=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(struct _tuple14*,
struct _tuple11*),struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_gen_decl_cls,({
struct _tuple14*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->f1=type;_tmpB5->f2=loc;
_tmpB5;}),_tmpA6);struct Cyc_List_List*_tmpA8=((struct Cyc_List_List*(*)(struct
_tagged_arr*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_first,
_tmpA6);struct Cyc_List_List*_tmpA9=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(
struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_toplevel_name,
_tmpA8);struct Cyc_List_List*_tmpAA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_unknownid_exp_cls,loc,_tmpA9);
struct Cyc_List_List*_tmpAB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Exp*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_address_exp_cls,loc,_tmpAA);
struct Cyc_Absyn_Exp*_tmpAC=Cyc_Tcgenrep_arr_init_exp(_tmpAB,loc);struct
_tagged_arr*_tmpAD=Cyc_Tcgenrep_new_gen_id(({const char*_tmpB4="arr";_tag_arr(
_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,4));}));void*_tmpAE=Cyc_Absyn_at_typ(
type,(void*)2,Cyc_Tcgenrep_tq_none,Cyc_Absyn_false_conref);void*_tmpAF=Cyc_Absyn_array_typ(
_tmpAE,Cyc_Tcgenrep_tq_none,0,Cyc_Absyn_false_conref);struct Cyc_Absyn_Decl*
_tmpB0=Cyc_Tcgenrep_gen_decl(_tmpAD,_tmpAF,(struct Cyc_Absyn_Exp*)_tmpAC,loc);
struct Cyc_Absyn_Exp*_tmpB1=Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmpAD),loc);return({struct _tuple15*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmpA7,({struct Cyc_Absyn_Decl*_tmpB3[1];_tmpB3[0]=_tmpB0;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpB3,sizeof(struct Cyc_Absyn_Decl*),
1));}));_tmpB2->f2=_tmpB1;_tmpB2;});}static void*Cyc_Tcgenrep_check_tunionfield_and_get_type(
struct Cyc_Absyn_Tunionfield*tuf){return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_TupleType_struct
_tmpB7;_tmpB7.tag=9;_tmpB7.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple10*_tmpB8[1];_tmpB8[0]=({
struct _tuple10*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->f1=Cyc_Tcgenrep_tq_none;
_tmpB9->f2=Cyc_Absyn_uint_typ;_tmpB9;});((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmpB8,sizeof(struct _tuple10*),1));}),tuf->typs);
_tmpB7;});_tmpB6;});}static struct _tuple6*Cyc_Tcgenrep_check_tunionfield_and_get_name(
struct Cyc_Absyn_Tunionfield*tuf){return tuf->name;}struct _tuple18{struct _tuple6*
f1;void*f2;};static struct _tuple18*Cyc_Tcgenrep_check_tunionfield_and_get_nmtype(
struct Cyc_Absyn_Tunionfield*tuf){return({struct _tuple18*_tmpBA=_cycalloc(sizeof(*
_tmpBA));_tmpBA->f1=tuf->name;_tmpBA->f2=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_TupleType_struct
_tmpBC;_tmpBC.tag=9;_tmpBC.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple10*_tmpBD[1];_tmpBD[0]=({
struct _tuple10*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->f1=Cyc_Tcgenrep_tq_none;
_tmpBE->f2=Cyc_Absyn_uint_typ;_tmpBE;});((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmpBD,sizeof(struct _tuple10*),1));}),tuf->typs);
_tmpBC;});_tmpBB;});_tmpBA;});}static struct _tuple16*Cyc_Tcgenrep_check_xtunionfield_and_get_name_type(
struct Cyc_Absyn_Tunionfield*tuf){return({struct _tuple16*_tmpBF=_cycalloc(sizeof(*
_tmpBF));_tmpBF->f1=(*((struct _tuple6*)tuf->name)).f2;_tmpBF->f2=(void*)({struct
Cyc_Absyn_TupleType_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({struct
Cyc_Absyn_TupleType_struct _tmpC1;_tmpC1.tag=9;_tmpC1.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(({struct
_tuple10*_tmpC2[1];_tmpC2[0]=({struct _tuple10*_tmpC3=_cycalloc(sizeof(*_tmpC3));
_tmpC3->f1=Cyc_Tcgenrep_tq_none;_tmpC3->f2=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Tcgenrep_tq_none,Cyc_Absyn_true_conref);_tmpC3;});((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpC2,sizeof(struct _tuple10*),1));}),
tuf->typs);_tmpC1;});_tmpC0;});_tmpBF;});}static int Cyc_Tcgenrep_filter_empty_tunionfield(
struct Cyc_Absyn_Tunionfield*tuf){return tuf->typs != 0;}static int Cyc_Tcgenrep_filter_nonempty_tunionfield(
struct Cyc_Absyn_Tunionfield*tuf){return tuf->typs == 0;}static struct _tagged_arr*
Cyc_Tcgenrep_get_tunionfield_name(struct Cyc_Absyn_Tunionfield*tuf){return(*((
struct _tuple6*)tuf->name)).f2;}struct Cyc_Absyn_Aggrfield*Cyc_Tcgenrep_substitute_structfield_type(
struct Cyc_List_List*subst,struct Cyc_Absyn_Aggrfield*sf){return({struct Cyc_Absyn_Aggrfield*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->name=sf->name;_tmpC4->tq=sf->tq;_tmpC4->type=(
void*)Cyc_Tcutil_substitute(subst,(void*)sf->type);_tmpC4->width=sf->width;
_tmpC4->attributes=sf->attributes;_tmpC4;});}struct _tuple10*Cyc_Tcgenrep_substitute_tqual_type(
struct Cyc_List_List*subst,struct _tuple10*pair){struct _tuple10 _tmpC6;struct Cyc_Absyn_Tqual
_tmpC7;void*_tmpC8;struct _tuple10*_tmpC5=pair;_tmpC6=*_tmpC5;_tmpC7=_tmpC6.f1;
_tmpC8=_tmpC6.f2;return({struct _tuple10*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->f1=
_tmpC7;_tmpC9->f2=Cyc_Tcutil_substitute(subst,_tmpC8);_tmpC9;});}struct Cyc_Absyn_Tunionfield*
Cyc_Tcgenrep_substitute_tunionfield_type(struct Cyc_List_List*subst,struct Cyc_Absyn_Tunionfield*
tf){return({struct Cyc_Absyn_Tunionfield*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->name=
tf->name;_tmpCA->typs=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_List_List*,
struct _tuple10*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_tqual_type,subst,tf->typs);_tmpCA->loc=tf->loc;_tmpCA->sc=(
void*)((void*)tf->sc);_tmpCA;});}void*Cyc_Tcgenrep_monomorphize_type(void*type){
void*_tmpCB=Cyc_Tcutil_compress(type);struct Cyc_Absyn_AggrInfo _tmpCC;void*_tmpCD;
struct Cyc_List_List*_tmpCE;struct Cyc_Absyn_TunionInfo _tmpCF;void*_tmpD0;struct
Cyc_Absyn_Tuniondecl**_tmpD1;struct Cyc_Absyn_Tuniondecl*_tmpD2;struct Cyc_List_List*
_tmpD3;void*_tmpD4;_LLF: if(_tmpCB <= (void*)3?1:*((int*)_tmpCB)!= 10)goto _LL11;
_tmpCC=((struct Cyc_Absyn_AggrType_struct*)_tmpCB)->f1;_tmpCD=(void*)_tmpCC.aggr_info;
_tmpCE=_tmpCC.targs;_LL10: {struct Cyc_Absyn_Aggrdecl*_tmpD5=Cyc_Absyn_get_known_aggrdecl(
_tmpCD);struct Cyc_List_List*_tmpD6=_tmpD5->tvs;if(Cyc_List_length(_tmpCE)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD6))({struct Cyc_Std_Int_pa_struct
_tmpDA;_tmpDA.tag=1;_tmpDA.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmpD6));{struct Cyc_Std_Int_pa_struct _tmpD9;_tmpD9.tag=1;_tmpD9.f1=(
int)((unsigned int)Cyc_List_length(_tmpCE));{void*_tmpD7[2]={& _tmpD9,& _tmpDA};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmpD8="gen(): number of params %d differs from number of tyvars %d";_tag_arr(
_tmpD8,sizeof(char),_get_zero_arr_size(_tmpD8,60));}),_tag_arr(_tmpD7,sizeof(
void*),2));}}});{struct Cyc_List_List*_tmpDB=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmpD6,_tmpCE);struct Cyc_List_List*fields=
0;if(_tmpD5->impl != 0)fields=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_structfield_type,
_tmpDB,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD5->impl))->fields);{
struct Cyc_Absyn_AggrdeclImpl*_tmpDC=_tmpD5->impl == 0?0:({struct Cyc_Absyn_AggrdeclImpl*
_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->exist_vars=0;_tmpE4->rgn_po=0;_tmpE4->fields=
fields;_tmpE4;});struct Cyc_Absyn_Aggrdecl*ad2=({struct Cyc_Absyn_Aggrdecl*_tmpE3=
_cycalloc(sizeof(*_tmpE3));_tmpE3->kind=(void*)((void*)_tmpD5->kind);_tmpE3->sc=(
void*)((void*)_tmpD5->sc);_tmpE3->name=_tmpD5->name;_tmpE3->tvs=0;_tmpE3->impl=
_tmpDC;_tmpE3->attributes=_tmpD5->attributes;_tmpE3;});return(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD[0]=({struct Cyc_Absyn_AggrType_struct
_tmpDE;_tmpDE.tag=10;_tmpDE.f1=({struct Cyc_Absyn_AggrInfo _tmpDF;_tmpDF.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpE0=_cycalloc(sizeof(*_tmpE0));
_tmpE0[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpE1;_tmpE1.tag=1;_tmpE1.f1=({
struct Cyc_Absyn_Aggrdecl**_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2[0]=ad2;_tmpE2;});
_tmpE1;});_tmpE0;}));_tmpDF.targs=0;_tmpDF;});_tmpDE;});_tmpDD;});}}}_LL11: if(
_tmpCB <= (void*)3?1:*((int*)_tmpCB)!= 2)goto _LL13;_tmpCF=((struct Cyc_Absyn_TunionType_struct*)
_tmpCB)->f1;_tmpD0=(void*)_tmpCF.tunion_info;if(*((int*)_tmpD0)!= 1)goto _LL13;
_tmpD1=((struct Cyc_Absyn_KnownTunion_struct*)_tmpD0)->f1;_tmpD2=*_tmpD1;_tmpD3=
_tmpCF.targs;_tmpD4=(void*)_tmpCF.rgn;_LL12: {struct Cyc_List_List*_tmpE5=_tmpD2->tvs;
if(Cyc_List_length(_tmpD3)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpE5))({struct Cyc_Std_Int_pa_struct _tmpE9;_tmpE9.tag=1;_tmpE9.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE5));{struct
Cyc_Std_Int_pa_struct _tmpE8;_tmpE8.tag=1;_tmpE8.f1=(int)((unsigned int)Cyc_List_length(
_tmpD3));{void*_tmpE6[2]={& _tmpE8,& _tmpE9};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpE7="gen(): number of params %d differs from number of tyvars %d";
_tag_arr(_tmpE7,sizeof(char),_get_zero_arr_size(_tmpE7,60));}),_tag_arr(_tmpE6,
sizeof(void*),2));}}});{struct Cyc_List_List*_tmpEA=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmpE5,_tmpD3);struct
Cyc_Core_Opt*fields=0;if(_tmpD2->fields != 0)fields=({struct Cyc_Core_Opt*_tmpEB=
_cycalloc(sizeof(*_tmpEB));_tmpEB->v=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tunionfield*(*
f)(struct Cyc_List_List*,struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_tunionfield_type,
_tmpEA,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpD2->fields))->v);
_tmpEB;});{struct Cyc_Absyn_Tuniondecl*_tmpEC=({struct Cyc_Absyn_Tuniondecl*_tmpF5=
_cycalloc(sizeof(*_tmpF5));_tmpF5->sc=(void*)((void*)_tmpD2->sc);_tmpF5->name=
_tmpD2->name;_tmpF5->tvs=0;_tmpF5->fields=fields;_tmpF5->is_xtunion=_tmpD2->is_xtunion;
_tmpF5;});struct Cyc_Absyn_TunionType_struct*_tmpED=({struct Cyc_Absyn_TunionType_struct*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Absyn_TunionType_struct
_tmpF0;_tmpF0.tag=2;_tmpF0.f1=({struct Cyc_Absyn_TunionInfo _tmpF1;_tmpF1.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmpF2=_cycalloc(sizeof(*
_tmpF2));_tmpF2[0]=({struct Cyc_Absyn_KnownTunion_struct _tmpF3;_tmpF3.tag=1;
_tmpF3.f1=({struct Cyc_Absyn_Tuniondecl**_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[
0]=_tmpEC;_tmpF4;});_tmpF3;});_tmpF2;}));_tmpF1.targs=_tmpD3;_tmpF1.rgn=(void*)
_tmpD4;_tmpF1;});_tmpF0;});_tmpEF;});struct Cyc_Position_Segment*_tmpEE=Cyc_Position_segment_of_abs(
0,0);return(void*)_tmpED;}}}_LL13:;_LL14: return type;_LLE:;}struct _tagged_arr Cyc_Tcgenrep_make_type_cstring(
void*t){struct _tagged_arr s=Cyc_Std_strdup(Cyc_Absynpp_typ2cstring(t));{int i=0;
for(0;i < Cyc_Std_strlen((struct _tagged_arr)s);i ++){if(*((char*)
_check_unknown_subscript(s,sizeof(char),i))== ' ')({struct _tagged_arr _tmpF6=
_tagged_arr_plus(s,sizeof(char),i);char _tmpF7=*((char*)_check_unknown_subscript(
_tmpF6,sizeof(char),0));char _tmpF8='_';if(_get_arr_size(_tmpF6,sizeof(char))== 1?
_tmpF7 == '\000'?_tmpF8 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmpF6.curr)=
_tmpF8;});else{if(!isalnum((int)*((char*)_check_unknown_subscript(s,sizeof(char),
i)))?*((char*)_check_unknown_subscript(s,sizeof(char),i))!= '_': 0)({struct
_tagged_arr _tmpF9=_tagged_arr_plus(s,sizeof(char),i);char _tmpFA=*((char*)
_check_unknown_subscript(_tmpF9,sizeof(char),0));char _tmpFB=(char)('0' + *((char*)
_check_unknown_subscript(s,sizeof(char),i))% 10);if(_get_arr_size(_tmpF9,sizeof(
char))== 1?_tmpFA == '\000'?_tmpFB != '\000': 0: 0)_throw_arraybounds();*((char*)
_tmpF9.curr)=_tmpFB;});}}}return(struct _tagged_arr)Cyc_Std_strconcat((struct
_tagged_arr)s,({const char*_tmpFC="_rep";_tag_arr(_tmpFC,sizeof(char),
_get_zero_arr_size(_tmpFC,5));}));}struct _tuple19{struct Cyc_Dict_Dict*f1;struct
Cyc_Tcgenrep_RepInfo*f2;};static struct _tuple19*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,void*type);struct
_tuple20{struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};struct _tuple21{struct
Cyc_Tcenv_Tenv*f1;struct Cyc_Position_Segment*f2;};static struct _tuple20*Cyc_Tcgenrep_lookupRepsCls(
struct _tuple21*env,void*type,struct _tuple20*carry){struct _tuple19 _tmpFE;struct
Cyc_Dict_Dict*_tmpFF;struct Cyc_Tcgenrep_RepInfo*_tmp100;struct _tuple19*_tmpFD=
Cyc_Tcgenrep_lookupRep((*((struct _tuple21*)env)).f1,(*((struct _tuple20*)carry)).f1,(*((
struct _tuple21*)env)).f2,type);_tmpFE=*_tmpFD;_tmpFF=_tmpFE.f1;_tmp100=_tmpFE.f2;
return({struct _tuple20*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->f1=_tmpFF;
_tmp101->f2=({struct Cyc_List_List*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->hd=
_tmp100;_tmp102->tl=(*((struct _tuple20*)carry)).f2;_tmp102;});_tmp101;});}static
struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*
dict,struct Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,
struct Cyc_List_List*types){struct Cyc_Absyn_Exp*_tmp103=Cyc_Absyn_sizeoftyp_exp(
type,loc);struct _tuple21*_tmp104=({struct _tuple21*_tmp119=_cycalloc(sizeof(*
_tmp119));_tmp119->f1=te;_tmp119->f2=loc;_tmp119;});struct _tuple20*_tmp105=({
struct _tuple20*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->f1=dict;_tmp118->f2=
0;_tmp118;});struct _tuple20 _tmp107;struct Cyc_Dict_Dict*_tmp108;struct Cyc_List_List*
_tmp109;struct _tuple20*_tmp106=((struct _tuple20*(*)(struct _tuple20*(*f)(struct
_tuple21*,void*,struct _tuple20*),struct _tuple21*,struct Cyc_List_List*x,struct
_tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp104,types,
_tmp105);_tmp107=*_tmp106;_tmp108=_tmp107.f1;_tmp109=_tmp107.f2;dict=_tmp108;{
struct Cyc_List_List*_tmp10A=((struct Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(
Cyc_List_length(types),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*
_tmp10B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple14*,int),
struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({
struct _tuple14*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->f1=type;_tmp117->f2=
loc;_tmp117;}),_tmp10A);struct Cyc_List_List*_tmp10C=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,
loc,_tmp109);struct Cyc_List_List*_tmp10D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp10B,_tmp10C));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmp116[2];
_tmp116[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp116[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_arr(_tmp116,sizeof(void*),2));}));
struct _tuple15 _tmp10F;struct Cyc_List_List*_tmp110;struct Cyc_Absyn_Exp*_tmp111;
struct _tuple15*_tmp10E=Cyc_Tcgenrep_array_decls(tuple_type,_tmp10D,loc);_tmp10F=*
_tmp10E;_tmp110=_tmp10F.f1;_tmp111=_tmp10F.f2;{struct Cyc_Absyn_Decl*_tmp112=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tuple_str),varname,({struct Cyc_Absyn_Exp*_tmp115[2];_tmp115[1]=
_tmp111;_tmp115[0]=_tmp103;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp115,sizeof(struct Cyc_Absyn_Exp*),2));}),sc,loc);struct Cyc_List_List*
_tmp113=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp110,({struct Cyc_Absyn_Decl*_tmp114[1];_tmp114[0]=
_tmp112;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp114,sizeof(struct Cyc_Absyn_Decl*),1));}));return Cyc_Tcgenrep_update_info(
dict,type,_tmp113,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp109,0);}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepTunionfield(
struct _tuple6*tname,struct _tuple6*fname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*
dict,struct Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,
struct Cyc_List_List*types){struct Cyc_Absyn_Exp*_tmp11A=Cyc_Absyn_sizeoftyp_exp(
type,loc);struct _tuple21*_tmp11B=({struct _tuple21*_tmp132=_cycalloc(sizeof(*
_tmp132));_tmp132->f1=te;_tmp132->f2=loc;_tmp132;});struct _tuple20*_tmp11C=({
struct _tuple20*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->f1=dict;_tmp131->f2=
0;_tmp131;});struct _tuple20 _tmp11E;struct Cyc_Dict_Dict*_tmp11F;struct Cyc_List_List*
_tmp120;struct _tuple20*_tmp11D=((struct _tuple20*(*)(struct _tuple20*(*f)(struct
_tuple21*,void*,struct _tuple20*),struct _tuple21*,struct Cyc_List_List*x,struct
_tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp11B,types,
_tmp11C);_tmp11E=*_tmp11D;_tmp11F=_tmp11E.f1;_tmp120=_tmp11E.f2;dict=_tmp11F;{
struct Cyc_List_List*_tmp121=((struct Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(
Cyc_List_length(types),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*
_tmp122=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple14*,int),
struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({
struct _tuple14*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->f1=type;_tmp130->f2=
loc;_tmp130;}),_tmp121);struct Cyc_List_List*_tmp123=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,
loc,_tmp120);struct Cyc_List_List*_tmp124=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp122,_tmp123));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmp12F[2];
_tmp12F[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp12F[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_arr(_tmp12F,sizeof(void*),2));}));
struct Cyc_Absyn_Exp*_tmp125=Cyc_Tcgenrep_cnst_string(*((struct _tagged_arr*)(*((
struct _tuple6*)tname)).f2),loc);struct Cyc_Absyn_Exp*_tmp126=Cyc_Tcgenrep_cnst_string(*((
struct _tagged_arr*)(*((struct _tuple6*)fname)).f2),loc);struct _tuple15 _tmp128;
struct Cyc_List_List*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;struct _tuple15*_tmp127=
Cyc_Tcgenrep_array_decls(tuple_type,_tmp124,loc);_tmp128=*_tmp127;_tmp129=
_tmp128.f1;_tmp12A=_tmp128.f2;{struct Cyc_Absyn_Decl*_tmp12B=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tunionfield_str),varname,({struct Cyc_Absyn_Exp*_tmp12E[4];_tmp12E[3]=
_tmp12A;_tmp12E[2]=_tmp11A;_tmp12E[1]=_tmp126;_tmp12E[0]=_tmp125;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp12E,sizeof(struct Cyc_Absyn_Exp*),
4));}),sc,loc);struct Cyc_List_List*_tmp12C=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp129,({struct Cyc_Absyn_Decl*
_tmp12D[1];_tmp12D[0]=_tmp12B;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp12D,sizeof(struct Cyc_Absyn_Decl*),1));}));return Cyc_Tcgenrep_update_info(
dict,type,_tmp12C,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp120,0);}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepStruct(
struct Cyc_Core_Opt*sname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct
Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*
nmtypes){struct Cyc_Absyn_Exp*_tmp133=Cyc_Absyn_sizeoftyp_exp(type,loc);struct
_tuple21*_tmp134=({struct _tuple21*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->f1=
te;_tmp151->f2=loc;_tmp151;});struct _tuple20*_tmp135=({struct _tuple20*_tmp150=
_cycalloc(sizeof(*_tmp150));_tmp150->f1=dict;_tmp150->f2=0;_tmp150;});struct Cyc_List_List*
_tmp137;struct Cyc_List_List*_tmp138;struct _tuple0 _tmp136=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(nmtypes);_tmp137=_tmp136.f1;_tmp138=
_tmp136.f2;{struct _tuple20 _tmp13A;struct Cyc_Dict_Dict*_tmp13B;struct Cyc_List_List*
_tmp13C;struct _tuple20*_tmp139=((struct _tuple20*(*)(struct _tuple20*(*f)(struct
_tuple21*,void*,struct _tuple20*),struct _tuple21*,struct Cyc_List_List*x,struct
_tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp134,
_tmp138,_tmp135);_tmp13A=*_tmp139;_tmp13B=_tmp13A.f1;_tmp13C=_tmp13A.f2;dict=
_tmp13B;{struct Cyc_List_List*_tmp13D=((struct Cyc_List_List*(*)(int n,int(*f)(int)))
Cyc_List_tabulate)(Cyc_List_length(_tmp138),(int(*)(int))Cyc_Core_identity);
struct Cyc_List_List*_tmp13E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct _tuple14*,int),struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_make_offsetof_exp,({struct _tuple14*_tmp14F=_cycalloc(sizeof(*
_tmp14F));_tmp14F->f1=type;_tmp14F->f2=loc;_tmp14F;}),_tmp13D);struct Cyc_List_List*
_tmp13F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tagged_arr*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_cnst_string_cls,loc,_tmp137);struct Cyc_List_List*_tmp140=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,
loc,_tmp13C);struct Cyc_List_List*_tmp141=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple13*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple3_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*
z))Cyc_List_zip3)(_tmp13E,_tmp13F,_tmp140));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({
void*_tmp14E[3];_tmp14E[2]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str));_tmp14E[1]=Cyc_Absyn_const_string_typ((void*)2);
_tmp14E[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_arr(_tmp14E,sizeof(void*),3));}));
struct _tuple15 _tmp143;struct Cyc_List_List*_tmp144;struct Cyc_Absyn_Exp*_tmp145;
struct _tuple15*_tmp142=Cyc_Tcgenrep_array_decls(tuple_type,_tmp141,loc);_tmp143=*
_tmp142;_tmp144=_tmp143.f1;_tmp145=_tmp143.f2;{struct Cyc_Absyn_Exp*name;if(sname
== 0)name=Cyc_Absyn_null_exp(loc);else{struct _tagged_arr*_tmp146=Cyc_Tcgenrep_new_gen_id(({
const char*_tmp149="name";_tag_arr(_tmp149,sizeof(char),_get_zero_arr_size(
_tmp149,5));}));struct Cyc_Absyn_Decl*_tmp147=Cyc_Tcgenrep_gen_decl(_tmp146,Cyc_Absyn_const_string_typ((
void*)2),(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*((struct _tagged_arr*)(*((
struct _tuple6*)((struct _tuple6*)sname->v))).f2),loc),loc);_tmp144=({struct Cyc_List_List*
_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148->hd=_tmp147;_tmp148->tl=_tmp144;
_tmp148;});name=Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmp146),loc),loc);}{struct Cyc_Absyn_Decl*_tmp14A=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str),varname,({struct Cyc_Absyn_Exp*_tmp14D[3];_tmp14D[2]=
_tmp145;_tmp14D[1]=_tmp133;_tmp14D[0]=name;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp14D,sizeof(struct Cyc_Absyn_Exp*),3));}),
sc,loc);struct Cyc_List_List*_tmp14B=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp144,({struct Cyc_Absyn_Decl*
_tmp14C[1];_tmp14C[0]=_tmp14A;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp14C,sizeof(struct Cyc_Absyn_Decl*),1));}));return Cyc_Tcgenrep_update_info(
dict,type,_tmp14B,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp13C,0);}}}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepUnion(
struct Cyc_Core_Opt*uname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct
Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*
nmtypes){struct Cyc_Absyn_Exp*_tmp152=Cyc_Absyn_sizeoftyp_exp(type,loc);struct
_tuple21*_tmp153=({struct _tuple21*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->f1=
te;_tmp16D->f2=loc;_tmp16D;});struct _tuple20*_tmp154=({struct _tuple20*_tmp16C=
_cycalloc(sizeof(*_tmp16C));_tmp16C->f1=dict;_tmp16C->f2=0;_tmp16C;});struct Cyc_List_List*
_tmp156;struct Cyc_List_List*_tmp157;struct _tuple0 _tmp155=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(nmtypes);_tmp156=_tmp155.f1;_tmp157=
_tmp155.f2;{struct _tuple20 _tmp159;struct Cyc_Dict_Dict*_tmp15A;struct Cyc_List_List*
_tmp15B;struct _tuple20*_tmp158=((struct _tuple20*(*)(struct _tuple20*(*f)(struct
_tuple21*,void*,struct _tuple20*),struct _tuple21*,struct Cyc_List_List*x,struct
_tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp153,
_tmp157,_tmp154);_tmp159=*_tmp158;_tmp15A=_tmp159.f1;_tmp15B=_tmp159.f2;dict=
_tmp15A;{struct Cyc_List_List*_tmp15C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tagged_arr*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,
_tmp156);struct Cyc_List_List*_tmp15D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp15B);struct Cyc_List_List*_tmp15E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp15C,_tmp15D));void*_tmp15F=Cyc_Tcgenrep_tuple_typ(({void*_tmp16B[2];_tmp16B[
1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp16B[0]=Cyc_Absyn_const_string_typ((void*)2);Cyc_List_list(_tag_arr(_tmp16B,
sizeof(void*),2));}));struct _tuple15 _tmp161;struct Cyc_List_List*_tmp162;struct
Cyc_Absyn_Exp*_tmp163;struct _tuple15*_tmp160=Cyc_Tcgenrep_array_decls(_tmp15F,
_tmp15E,loc);_tmp161=*_tmp160;_tmp162=_tmp161.f1;_tmp163=_tmp161.f2;{struct Cyc_Absyn_Exp*
name;if(uname == 0)name=Cyc_Absyn_null_exp(loc);else{struct _tagged_arr*_tmp164=
Cyc_Tcgenrep_new_gen_id(({const char*_tmp167="name";_tag_arr(_tmp167,sizeof(char),
_get_zero_arr_size(_tmp167,5));}));struct Cyc_Absyn_Decl*_tmp165=Cyc_Tcgenrep_gen_decl(
_tmp164,Cyc_Absyn_const_string_typ((void*)2),(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*((
struct _tagged_arr*)(*((struct _tuple6*)((struct _tuple6*)uname->v))).f2),loc),loc);
_tmp162=({struct Cyc_List_List*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->hd=
_tmp165;_tmp166->tl=_tmp162;_tmp166;});name=Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
Cyc_Tcgenrep_toplevel_name(_tmp164),loc),loc);}{struct Cyc_Absyn_Decl*_tmp168=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_union_str),varname,({struct Cyc_Absyn_Exp*_tmp16A[3];_tmp16A[2]=
_tmp163;_tmp16A[1]=_tmp152;_tmp16A[0]=name;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp16A,sizeof(struct Cyc_Absyn_Exp*),3));}),
sc,loc);return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp162,({
struct Cyc_Absyn_Decl*_tmp169[1];_tmp169[0]=_tmp168;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp169,sizeof(struct Cyc_Absyn_Decl*),
1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp15B,0);}}}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepEnum(
struct _tuple6**ename,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*tagnms){struct
Cyc_Absyn_Exp*_tmp16E=Cyc_Absyn_sizeoftyp_exp(type,loc);struct Cyc_List_List*
_tmp170;struct Cyc_List_List*_tmp171;struct _tuple0 _tmp16F=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(tagnms);_tmp170=_tmp16F.f1;_tmp171=
_tmp16F.f2;{struct Cyc_List_List*_tmp172=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
_tmp171);struct Cyc_List_List*_tmp173=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp170);struct Cyc_List_List*
_tmp174=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple12*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(_tmp173,_tmp172));void*_tmp175=Cyc_Tcgenrep_tuple_typ(({
void*_tmp181[2];_tmp181[1]=Cyc_Absyn_const_string_typ((void*)2);_tmp181[0]=Cyc_Absyn_uint_typ;
Cyc_List_list(_tag_arr(_tmp181,sizeof(void*),2));}));struct _tuple15 _tmp177;
struct Cyc_List_List*_tmp178;struct Cyc_Absyn_Exp*_tmp179;struct _tuple15*_tmp176=
Cyc_Tcgenrep_array_decls(_tmp175,_tmp174,loc);_tmp177=*_tmp176;_tmp178=_tmp177.f1;
_tmp179=_tmp177.f2;{struct Cyc_Absyn_Exp*name;if(ename == 0)name=Cyc_Absyn_null_exp(
loc);else{struct _tagged_arr*_tmp17A=Cyc_Tcgenrep_new_gen_id(({const char*_tmp17D="name";
_tag_arr(_tmp17D,sizeof(char),_get_zero_arr_size(_tmp17D,5));}));struct Cyc_Absyn_Decl*
_tmp17B=Cyc_Tcgenrep_gen_decl(_tmp17A,Cyc_Absyn_const_string_typ((void*)2),(
struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*((struct _tagged_arr*)(*((struct
_tuple6*)*((struct _tuple6**)ename))).f2),loc),loc);_tmp178=({struct Cyc_List_List*
_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->hd=_tmp17B;_tmp17C->tl=_tmp178;
_tmp17C;});name=Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmp17A),loc),loc);}{struct Cyc_Absyn_Decl*_tmp17E=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_enum_str),varname,({struct Cyc_Absyn_Exp*_tmp180[3];_tmp180[2]=
_tmp179;_tmp180[1]=_tmp16E;_tmp180[0]=name;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp180,sizeof(struct Cyc_Absyn_Exp*),3));}),
sc,loc);return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp178,({
struct Cyc_Absyn_Decl*_tmp17F[1];_tmp17F[0]=_tmp17E;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp17F,sizeof(struct Cyc_Absyn_Decl*),
1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),0,0);}}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepTunion(
struct _tuple6*tname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*tonames,struct
Cyc_List_List*nmtypes){struct Cyc_List_List*_tmp182=((struct Cyc_List_List*(*)(int
n,int(*f)(int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tonames),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp183=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp182);
struct Cyc_List_List*_tmp184=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
tonames);struct Cyc_List_List*_tmp185=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp183,_tmp184);struct Cyc_List_List*
_tmp186=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple12*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp185);void*_tmp187=Cyc_Tcgenrep_tuple_typ(({
void*_tmp1A6[2];_tmp1A6[1]=Cyc_Absyn_const_string_typ((void*)2);_tmp1A6[0]=Cyc_Absyn_uint_typ;
Cyc_List_list(_tag_arr(_tmp1A6,sizeof(void*),2));}));struct _tuple15 _tmp189;
struct Cyc_List_List*_tmp18A;struct Cyc_Absyn_Exp*_tmp18B;struct _tuple15*_tmp188=
Cyc_Tcgenrep_array_decls(_tmp187,_tmp186,loc);_tmp189=*_tmp188;_tmp18A=_tmp189.f1;
_tmp18B=_tmp189.f2;{struct _tuple21*_tmp18C=({struct _tuple21*_tmp1A5=_cycalloc(
sizeof(*_tmp1A5));_tmp1A5->f1=te;_tmp1A5->f2=loc;_tmp1A5;});struct _tuple20*
_tmp18D=({struct _tuple20*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4->f1=dict;
_tmp1A4->f2=0;_tmp1A4;});struct Cyc_List_List*_tmp18F;struct Cyc_List_List*_tmp190;
struct _tuple0 _tmp18E=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
nmtypes);_tmp18F=_tmp18E.f1;_tmp190=_tmp18E.f2;{struct _tuple20 _tmp192;struct Cyc_Dict_Dict*
_tmp193;struct Cyc_List_List*_tmp194;struct _tuple20*_tmp191=((struct _tuple20*(*)(
struct _tuple20*(*f)(struct _tuple21*,void*,struct _tuple20*),struct _tuple21*,
struct Cyc_List_List*x,struct _tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp18C,_tmp190,_tmp18D);_tmp192=*_tmp191;_tmp193=_tmp192.f1;_tmp194=_tmp192.f2;
dict=_tmp193;{struct Cyc_List_List*_tmp195=((struct Cyc_List_List*(*)(int n,int(*f)(
int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp194),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp196=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp195);
struct Cyc_List_List*_tmp197=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
_tmp18F);struct Cyc_List_List*_tmp198=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp194);struct Cyc_List_List*_tmp199=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y,struct Cyc_List_List*z))Cyc_List_zip3)(_tmp196,_tmp197,
_tmp198);struct Cyc_List_List*_tmp19A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple13*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple3_exp_cls,loc,_tmp199);
void*tuple_type2=Cyc_Tcgenrep_tuple_typ(({void*_tmp1A3[3];_tmp1A3[2]=Cyc_Tcgenrep_tunion_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));_tmp1A3[1]=Cyc_Absyn_const_string_typ((
void*)2);_tmp1A3[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_arr(_tmp1A3,sizeof(
void*),3));}));struct _tuple15 _tmp19C;struct Cyc_List_List*_tmp19D;struct Cyc_Absyn_Exp*
_tmp19E;struct _tuple15*_tmp19B=Cyc_Tcgenrep_array_decls(tuple_type2,_tmp19A,loc);
_tmp19C=*_tmp19B;_tmp19D=_tmp19C.f1;_tmp19E=_tmp19C.f2;{struct Cyc_Absyn_Exp*
_tmp19F=Cyc_Tcgenrep_cnst_string(*((struct _tagged_arr*)(*((struct _tuple6*)tname)).f2),
loc);struct Cyc_Absyn_Decl*_tmp1A0=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str),
varname,({struct Cyc_Absyn_Exp*_tmp1A2[3];_tmp1A2[2]=_tmp19E;_tmp1A2[1]=_tmp18B;
_tmp1A2[0]=_tmp19F;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1A2,sizeof(struct Cyc_Absyn_Exp*),3));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp18A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp19D,({struct Cyc_Absyn_Decl*
_tmp1A1[1];_tmp1A1[0]=_tmp1A0;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1A1,sizeof(struct Cyc_Absyn_Decl*),1));}))),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp194,0);
return dict;}}}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepXTunion(struct
_tuple6*xname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*nametypes){
struct Cyc_List_List*_tmp1A8;struct Cyc_List_List*_tmp1A9;struct _tuple0 _tmp1A7=((
struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(nametypes);_tmp1A8=
_tmp1A7.f1;_tmp1A9=_tmp1A7.f2;{struct _tuple21*_tmp1AA=({struct _tuple21*_tmp1BF=
_cycalloc(sizeof(*_tmp1BF));_tmp1BF->f1=te;_tmp1BF->f2=loc;_tmp1BF;});struct
_tuple20*_tmp1AB=({struct _tuple20*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->f1=
dict;_tmp1BE->f2=0;_tmp1BE;});struct _tuple20 _tmp1AD;struct Cyc_Dict_Dict*_tmp1AE;
struct Cyc_List_List*_tmp1AF;struct _tuple20*_tmp1AC=((struct _tuple20*(*)(struct
_tuple20*(*f)(struct _tuple21*,void*,struct _tuple20*),struct _tuple21*,struct Cyc_List_List*
x,struct _tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp1AA,_tmp1A9,_tmp1AB);_tmp1AD=*_tmp1AC;_tmp1AE=_tmp1AD.f1;_tmp1AF=_tmp1AD.f2;
dict=_tmp1AE;{struct Cyc_List_List*_tmp1B0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tagged_arr*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,
_tmp1A8);struct Cyc_List_List*_tmp1B1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp1AF);struct Cyc_List_List*_tmp1B2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp1B0,_tmp1B1);struct Cyc_List_List*
_tmp1B3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple12*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp1B2);void*name_type=Cyc_Absyn_tagged_typ(Cyc_Absyn_char_typ,(
void*)2,({struct Cyc_Absyn_Tqual _tmp1BD;_tmp1BD.q_const=1;_tmp1BD.q_volatile=0;
_tmp1BD.q_restrict=0;_tmp1BD;}),Cyc_Absyn_true_conref);void*tuple_type=Cyc_Tcgenrep_tuple_typ(({
void*_tmp1BC[2];_tmp1BC[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str));_tmp1BC[0]=name_type;Cyc_List_list(_tag_arr(
_tmp1BC,sizeof(void*),2));}));struct _tuple15 _tmp1B5;struct Cyc_List_List*_tmp1B6;
struct Cyc_Absyn_Exp*_tmp1B7;struct _tuple15*_tmp1B4=Cyc_Tcgenrep_array_decls(
tuple_type,_tmp1B3,loc);_tmp1B5=*_tmp1B4;_tmp1B6=_tmp1B5.f1;_tmp1B7=_tmp1B5.f2;{
struct Cyc_Absyn_Exp*_tmp1B8=Cyc_Tcgenrep_cnst_string(*((struct _tagged_arr*)(*((
struct _tuple6*)xname)).f2),loc);struct Cyc_Absyn_Decl*_tmp1B9=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_xtunion_str),varname,({struct Cyc_Absyn_Exp*_tmp1BB[2];_tmp1BB[1]=
_tmp1B7;_tmp1BB[0]=_tmp1B8;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1BB,sizeof(struct Cyc_Absyn_Exp*),2));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp1B6,({struct Cyc_Absyn_Decl*_tmp1BA[1];_tmp1BA[0]=
_tmp1B9;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp1BA,sizeof(struct Cyc_Absyn_Decl*),1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp1AF,0);return dict;}}}}static struct
_tuple19*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,
struct Cyc_Position_Segment*loc,void*type){struct Cyc_Core_Opt*info=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(dict,type);if(info != 0){if(((
struct Cyc_Tcgenrep_RepInfo*)info->v)->is_extern)dict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*,void*))Cyc_Dict_delete)(dict,type);else{struct _tuple19*
_tmp1C0=({struct _tuple19*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->f1=dict;
_tmp1C1->f2=(struct Cyc_Tcgenrep_RepInfo*)info->v;_tmp1C1;});return _tmp1C0;}}{
void*_tmp1C2=Cyc_Tcutil_compress(Cyc_Tcgenrep_monomorphize_type(type));void*
_tmp1C3;void*_tmp1C4;int _tmp1C5;struct Cyc_Absyn_PtrInfo _tmp1C6;struct Cyc_Absyn_ArrayInfo
_tmp1C7;void*_tmp1C8;struct Cyc_Absyn_Tqual _tmp1C9;struct Cyc_Absyn_Exp*_tmp1CA;
struct Cyc_Absyn_Conref*_tmp1CB;struct Cyc_List_List*_tmp1CC;struct _tuple6*_tmp1CD;
struct Cyc_List_List*_tmp1CE;struct Cyc_Absyn_Typedefdecl*_tmp1CF;struct Cyc_Core_Opt*
_tmp1D0;struct Cyc_Absyn_Tvar*_tmp1D1;struct Cyc_Absyn_FnInfo _tmp1D2;struct Cyc_Absyn_TunionInfo
_tmp1D3;void*_tmp1D4;struct Cyc_Absyn_Tuniondecl**_tmp1D5;struct Cyc_Absyn_Tuniondecl*
_tmp1D6;struct Cyc_List_List*_tmp1D7;void*_tmp1D8;struct Cyc_Absyn_TunionFieldInfo
_tmp1D9;struct Cyc_Absyn_AggrInfo _tmp1DA;void*_tmp1DB;struct Cyc_List_List*_tmp1DC;
void*_tmp1DD;struct Cyc_List_List*_tmp1DE;void*_tmp1DF;struct Cyc_List_List*
_tmp1E0;struct Cyc_Absyn_Enumdecl*_tmp1E1;struct Cyc_List_List*_tmp1E2;_LL16: if(
_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)!= 5)goto _LL18;_tmp1C3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1C2)->f1;_tmp1C4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1C2)->f2;_LL17: {
struct _tuple6*_tmp1E3=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const
char*_tmp1E7="rep";_tag_arr(_tmp1E7,sizeof(char),_get_zero_arr_size(_tmp1E7,4));})));
struct Cyc_Absyn_Decl*_tmp1E4=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_int_str),
_tmp1E3,({struct Cyc_Absyn_Exp*_tmp1E6[2];_tmp1E6[1]=Cyc_Tcgenrep_cnst_int(Cyc_Tcgenrep_size_of2int(
_tmp1C4),loc);_tmp1E6[0]=Cyc_Tcgenrep_cnst_int(_tmp1C3 == (void*)0?1: 0,loc);((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1E6,
sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(
dict,type,({struct Cyc_Absyn_Decl*_tmp1E5[1];_tmp1E5[0]=_tmp1E4;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1E5,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1E3,loc),loc),0,0);goto _LL15;}_LL18: if((int)_tmp1C2 != 1)goto _LL1A;_LL19: dict=
Cyc_Tcgenrep_update_info(dict,type,0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unknownid_exp(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_float_str),loc),0,0);goto _LL15;_LL1A: if(
_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)!= 6)goto _LL1C;_tmp1C5=((struct Cyc_Absyn_DoubleType_struct*)
_tmp1C2)->f1;_LL1B: dict=Cyc_Tcgenrep_update_info(dict,type,0,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_double_str),loc),
0,0);goto _LL15;_LL1C: if(_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)!= 4)goto _LL1E;
_tmp1C6=((struct Cyc_Absyn_PointerType_struct*)_tmp1C2)->f1;_LL1D:{void*_tmp1E8=
Cyc_Absyn_conref_val(_tmp1C6.bounds);struct Cyc_Absyn_Exp*_tmp1E9;_LL4D: if(
_tmp1E8 <= (void*)1?1:*((int*)_tmp1E8)!= 0)goto _LL4F;_tmp1E9=((struct Cyc_Absyn_Upper_b_struct*)
_tmp1E8)->f1;_LL4E: {struct _tuple6*_tmp1EA=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({
const char*_tmp1F5="rep";_tag_arr(_tmp1F5,sizeof(char),_get_zero_arr_size(_tmp1F5,
4));})));struct Cyc_Core_Opt*_tmp1EB=({struct Cyc_Core_Opt*_tmp1F4=_cycalloc(
sizeof(*_tmp1F4));_tmp1F4->v=Cyc_Tcgenrep_gen_vardecl(_tmp1EA,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str)),0,(void*)3,loc);_tmp1F4;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1EA,loc),loc),_tmp1EB,0);{struct _tuple19 _tmp1ED;struct Cyc_Dict_Dict*_tmp1EE;
struct Cyc_Tcgenrep_RepInfo*_tmp1EF;struct _tuple19*_tmp1EC=Cyc_Tcgenrep_lookupRep(
te,dict,loc,(void*)_tmp1C6.elt_typ);_tmp1ED=*_tmp1EC;_tmp1EE=_tmp1ED.f1;_tmp1EF=
_tmp1ED.f2;dict=_tmp1EE;{struct Cyc_Absyn_Decl*_tmp1F0=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str),_tmp1EA,({struct Cyc_Absyn_Exp*_tmp1F3[2];_tmp1F3[1]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp1EF->exp);_tmp1F3[0]=_tmp1E9;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1F3,sizeof(struct Cyc_Absyn_Exp*),
2));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*
_tmp1F1[1];_tmp1F1[0]=_tmp1F0;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1F1,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1EA,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp1F2[1];
_tmp1F2[0]=_tmp1EF;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1F2,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL4C;}}}
_LL4F: if((int)_tmp1E8 != 0)goto _LL4C;_LL50: {struct _tuple6*_tmp1F6=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(({const char*_tmp201="rep";_tag_arr(_tmp201,sizeof(char),
_get_zero_arr_size(_tmp201,4));})));struct Cyc_Core_Opt*_tmp1F7=({struct Cyc_Core_Opt*
_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200->v=Cyc_Tcgenrep_gen_vardecl(_tmp1F6,
Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str)),0,(void*)3,loc);_tmp200;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1F6,loc),loc),_tmp1F7,0);{struct _tuple19 _tmp1F9;
struct Cyc_Dict_Dict*_tmp1FA;struct Cyc_Tcgenrep_RepInfo*_tmp1FB;struct _tuple19*
_tmp1F8=Cyc_Tcgenrep_lookupRep(te,dict,loc,(void*)_tmp1C6.elt_typ);_tmp1F9=*
_tmp1F8;_tmp1FA=_tmp1F9.f1;_tmp1FB=_tmp1F9.f2;dict=_tmp1FA;{struct Cyc_Absyn_Decl*
_tmp1FC=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str),_tmp1F6,({struct Cyc_Absyn_Exp*
_tmp1FF[1];_tmp1FF[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp1FB->exp);((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1FF,sizeof(
struct Cyc_Absyn_Exp*),1));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(dict,
type,({struct Cyc_Absyn_Decl*_tmp1FD[1];_tmp1FD[0]=_tmp1FC;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1FD,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1F6,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp1FE[1];_tmp1FE[0]=_tmp1FB;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1FE,
sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL4C;}}}_LL4C:;}goto _LL15;
_LL1E: if(_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)!= 7)goto _LL20;_tmp1C7=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1C2)->f1;_tmp1C8=(void*)_tmp1C7.elt_type;_tmp1C9=_tmp1C7.tq;_tmp1CA=_tmp1C7.num_elts;
_tmp1CB=_tmp1C7.zero_term;_LL1F: if(_tmp1CA == 0)({void*_tmp202[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp203="tcgenrep:At this point, array bounds must be constants";
_tag_arr(_tmp203,sizeof(char),_get_zero_arr_size(_tmp203,55));}),_tag_arr(
_tmp202,sizeof(void*),0));});{struct _tuple19 _tmp205;struct Cyc_Dict_Dict*_tmp206;
struct Cyc_Tcgenrep_RepInfo*_tmp207;struct _tuple19*_tmp204=Cyc_Tcgenrep_lookupRep(
te,dict,loc,_tmp1C8);_tmp205=*_tmp204;_tmp206=_tmp205.f1;_tmp207=_tmp205.f2;dict=
_tmp206;{struct _tuple6*_tmp208=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({
const char*_tmp20D="rep";_tag_arr(_tmp20D,sizeof(char),_get_zero_arr_size(_tmp20D,
4));})));struct Cyc_Absyn_Decl*_tmp209=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str),
_tmp208,({struct Cyc_Absyn_Exp*_tmp20C[2];_tmp20C[1]=(struct Cyc_Absyn_Exp*)
_check_null(_tmp207->exp);_tmp20C[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp1CA);((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp20C,
sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(
dict,type,({struct Cyc_Absyn_Decl*_tmp20A[1];_tmp20A[0]=_tmp209;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp20A,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp208,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp20B[1];_tmp20B[0]=_tmp207;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp20B,
sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL15;}}_LL20: if(_tmp1C2 <= (
void*)3?1:*((int*)_tmp1C2)!= 9)goto _LL22;_tmp1CC=((struct Cyc_Absyn_TupleType_struct*)
_tmp1C2)->f1;_LL21: {struct _tuple6*_tmp20E=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({
const char*_tmp212="rep";_tag_arr(_tmp212,sizeof(char),_get_zero_arr_size(_tmp212,
4));})));struct Cyc_Core_Opt*_tmp20F=({struct Cyc_Core_Opt*_tmp211=_cycalloc(
sizeof(*_tmp211));_tmp211->v=Cyc_Tcgenrep_gen_vardecl(_tmp20E,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tuple_str)),0,(void*)3,loc);_tmp211;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp20E,loc),loc),_tmp20F,0);{struct Cyc_List_List*_tmp210=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_second,
_tmp1CC);dict=Cyc_Tcgenrep_buildRepTuple(te,dict,loc,_tmp20E,(void*)0,type,
_tmp210);goto _LL15;}}_LL22: if(_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)!= 16)goto
_LL24;_tmp1CD=((struct Cyc_Absyn_TypedefType_struct*)_tmp1C2)->f1;_tmp1CE=((
struct Cyc_Absyn_TypedefType_struct*)_tmp1C2)->f2;_tmp1CF=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1C2)->f3;_tmp1D0=((struct Cyc_Absyn_TypedefType_struct*)_tmp1C2)->f4;_LL23:
if(_tmp1D0 == 0)({void*_tmp213[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp214="gen(): can't gen for abstract typedef";
_tag_arr(_tmp214,sizeof(char),_get_zero_arr_size(_tmp214,38));}),_tag_arr(
_tmp213,sizeof(void*),0));});{struct _tuple19 _tmp216;struct Cyc_Dict_Dict*_tmp217;
struct Cyc_Tcgenrep_RepInfo*_tmp218;struct _tuple19*_tmp215=Cyc_Tcgenrep_lookupRep(
te,dict,loc,(void*)_tmp1D0->v);_tmp216=*_tmp215;_tmp217=_tmp216.f1;_tmp218=
_tmp216.f2;dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(_tmp217,type,_tmp218);goto _LL15;}_LL24: if((int)_tmp1C2 != 0)
goto _LL26;_LL25:({void*_tmp219[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp21A="found void in gen() expression";
_tag_arr(_tmp21A,sizeof(char),_get_zero_arr_size(_tmp21A,31));}),_tag_arr(
_tmp219,sizeof(void*),0));});goto _LL15;_LL26: if(_tmp1C2 <= (void*)3?1:*((int*)
_tmp1C2)!= 0)goto _LL28;_LL27:({void*_tmp21B[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp21C="found evar in gen() expression";_tag_arr(_tmp21C,sizeof(char),
_get_zero_arr_size(_tmp21C,31));}),_tag_arr(_tmp21B,sizeof(void*),0));});goto
_LL15;_LL28: if(_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)!= 1)goto _LL2A;_tmp1D1=((
struct Cyc_Absyn_VarType_struct*)_tmp1C2)->f1;_LL29:({void*_tmp21D[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp21E="found tyvar in gen() expression";_tag_arr(_tmp21E,
sizeof(char),_get_zero_arr_size(_tmp21E,32));}),_tag_arr(_tmp21D,sizeof(void*),0));});
goto _LL15;({void*_tmp21F[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp220="found variable type in gen() expression";
_tag_arr(_tmp220,sizeof(char),_get_zero_arr_size(_tmp220,40));}),_tag_arr(
_tmp21F,sizeof(void*),0));});goto _LL15;_LL2A: if(_tmp1C2 <= (void*)3?1:*((int*)
_tmp1C2)!= 8)goto _LL2C;_tmp1D2=((struct Cyc_Absyn_FnType_struct*)_tmp1C2)->f1;
_LL2B:({void*_tmp221[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp222="found function type in gen() expression";
_tag_arr(_tmp222,sizeof(char),_get_zero_arr_size(_tmp222,40));}),_tag_arr(
_tmp221,sizeof(void*),0));});goto _LL15;_LL2C: if(_tmp1C2 <= (void*)3?1:*((int*)
_tmp1C2)!= 15)goto _LL2E;_LL2D: goto _LL2F;_LL2E: if((int)_tmp1C2 != 2)goto _LL30;
_LL2F: goto _LL31;_LL30: if(_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)!= 19)goto _LL32;
_LL31: goto _LL33;_LL32: if(_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)!= 20)goto _LL34;
_LL33: goto _LL35;_LL34: if(_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)!= 21)goto _LL36;
_LL35:({void*_tmp223[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp224="gen(): unhandled region, handle or effect type";
_tag_arr(_tmp224,sizeof(char),_get_zero_arr_size(_tmp224,47));}),_tag_arr(
_tmp223,sizeof(void*),0));});goto _LL15;_LL36: if(_tmp1C2 <= (void*)3?1:*((int*)
_tmp1C2)!= 2)goto _LL38;_tmp1D3=((struct Cyc_Absyn_TunionType_struct*)_tmp1C2)->f1;
_tmp1D4=(void*)_tmp1D3.tunion_info;if(*((int*)_tmp1D4)!= 1)goto _LL38;_tmp1D5=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp1D4)->f1;_tmp1D6=*_tmp1D5;_tmp1D7=
_tmp1D3.targs;_tmp1D8=(void*)_tmp1D3.rgn;_LL37: if(_tmp1D6->tvs != 0)({void*
_tmp225[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp226="gen(): tunion type variables not handled yet";_tag_arr(
_tmp226,sizeof(char),_get_zero_arr_size(_tmp226,45));}),_tag_arr(_tmp225,sizeof(
void*),0));});if(_tmp1D6->fields == 0){struct _tuple6*_tmp227=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp22A=_cycalloc(sizeof(*_tmp22A));_tmp22A[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp22A;}));struct Cyc_Core_Opt*_tmp228=({struct Cyc_Core_Opt*_tmp229=
_cycalloc(sizeof(*_tmp229));_tmp229->v=Cyc_Tcgenrep_gen_vardecl(_tmp227,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tunion_str)),0,(void*)3,loc);_tmp229;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp227,loc),loc),_tmp228,1);goto _LL15;}if(!_tmp1D6->is_xtunion){struct _tuple6*
_tmp22B=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp233=_cycalloc(sizeof(*
_tmp233));_tmp233[0]=Cyc_Tcgenrep_make_type_cstring(type);_tmp233;}));struct Cyc_Core_Opt*
_tmp22C=({struct Cyc_Core_Opt*_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232->v=Cyc_Tcgenrep_gen_vardecl(
_tmp22B,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str)),0,(void*)3,loc);_tmp232;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp22B,loc),loc),_tmp22C,0);{struct Cyc_List_List*
_tmp22D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1D6->fields))->v;
struct Cyc_List_List*_tmp22E=((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_tunionfield,
_tmp22D);struct Cyc_List_List*_tmp22F=((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_Tcgenrep_filter_nonempty_tunionfield,
_tmp22D);struct Cyc_List_List*_tmp230=((struct Cyc_List_List*(*)(struct _tuple18*(*
f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_tunionfield_and_get_nmtype,
_tmp22E);struct Cyc_List_List*_tmp231=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(
struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_tunionfield_and_get_name,
_tmp22F);dict=Cyc_Tcgenrep_buildRepTunion(_tmp1D6->name,te,dict,loc,_tmp22B,(
void*)2,type,_tmp231,_tmp230);}}else{struct _tuple6*_tmp234=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp23A;}));struct Cyc_Core_Opt*_tmp235=({struct Cyc_Core_Opt*_tmp239=
_cycalloc(sizeof(*_tmp239));_tmp239->v=Cyc_Tcgenrep_gen_vardecl(_tmp234,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_xtunion_str)),0,(void*)3,loc);_tmp239;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp234,loc),loc),_tmp235,0);{struct Cyc_List_List*_tmp236=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1D6->fields))->v;struct Cyc_List_List*_tmp237=((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*
x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_tunionfield,_tmp236);struct Cyc_List_List*
_tmp238=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_xtunionfield_and_get_name_type,
_tmp237);dict=Cyc_Tcgenrep_buildRepXTunion(_tmp1D6->name,te,dict,loc,_tmp234,(
void*)2,type,_tmp238);}}goto _LL15;_LL38: if(_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)
!= 2)goto _LL3A;_LL39:({void*_tmp23B[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp23C="gen(): tunion must be resolved by now";
_tag_arr(_tmp23C,sizeof(char),_get_zero_arr_size(_tmp23C,38));}),_tag_arr(
_tmp23B,sizeof(void*),0));});_LL3A: if(_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)!= 3)
goto _LL3C;_tmp1D9=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1C2)->f1;_LL3B:
if(_tmp1D9.targs != 0)({void*_tmp23D[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp23E="gen(): tunionfield type parameters not handled yet";
_tag_arr(_tmp23E,sizeof(char),_get_zero_arr_size(_tmp23E,51));}),_tag_arr(
_tmp23D,sizeof(void*),0));});{void*_tmp23F=(void*)_tmp1D9.field_info;struct Cyc_Absyn_Tuniondecl*
_tmp240;struct Cyc_Absyn_Tunionfield*_tmp241;_LL52: if(*((int*)_tmp23F)!= 1)goto
_LL54;_tmp240=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp23F)->f1;_tmp241=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp23F)->f2;_LL53: {struct Cyc_List_List*
_tmp242=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple10*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcgenrep_get_second,_tmp241->typs);struct Cyc_List_List*
_tmp243=({struct Cyc_List_List*_tmp246=_cycalloc(sizeof(*_tmp246));_tmp246->hd=(
void*)Cyc_Absyn_uint_typ;_tmp246->tl=_tmp242;_tmp246;});struct _tuple6*_tmp244=
Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const char*_tmp245="rep";
_tag_arr(_tmp245,sizeof(char),_get_zero_arr_size(_tmp245,4));})));dict=Cyc_Tcgenrep_buildRepTunionfield(
_tmp240->name,_tmp241->name,te,dict,loc,_tmp244,(void*)0,type,_tmp243);goto _LL51;}
_LL54:;_LL55:({void*_tmp247[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp248="gen(): tunionfield must be known at this point";
_tag_arr(_tmp248,sizeof(char),_get_zero_arr_size(_tmp248,47));}),_tag_arr(
_tmp247,sizeof(void*),0));});_LL51:;}goto _LL15;_LL3C: if(_tmp1C2 <= (void*)3?1:*((
int*)_tmp1C2)!= 10)goto _LL3E;_tmp1DA=((struct Cyc_Absyn_AggrType_struct*)_tmp1C2)->f1;
_tmp1DB=(void*)_tmp1DA.aggr_info;_tmp1DC=_tmp1DA.targs;_LL3D: {struct Cyc_Absyn_Aggrdecl*
_tmp249=Cyc_Absyn_get_known_aggrdecl(_tmp1DB);if(_tmp249->impl != 0?((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp249->impl))->exist_vars != 0: 0)({void*_tmp24A[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp24B="gen(): existential type variables not yet supported";
_tag_arr(_tmp24B,sizeof(char),_get_zero_arr_size(_tmp24B,52));}),_tag_arr(
_tmp24A,sizeof(void*),0));});if((void*)_tmp249->kind == (void*)0){struct Cyc_Absyn_Aggrdecl*
_tmp24C=_tmp249;if(_tmp24C->impl == 0){struct _tuple6*_tmp24D=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp250=_cycalloc(sizeof(*_tmp250));_tmp250[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp250;}));struct Cyc_Core_Opt*_tmp24E=({struct Cyc_Core_Opt*_tmp24F=
_cycalloc(sizeof(*_tmp24F));_tmp24F->v=Cyc_Tcgenrep_gen_vardecl(_tmp24D,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str)),0,(void*)3,loc);_tmp24F;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp24D,loc),loc),_tmp24E,1);goto _LL15;}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp24C->impl))->fields != 0?((int(*)(int(*pred)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp24C->impl))->fields): 0){int _tmp251=((int(*)(int(*f)(int,struct
Cyc_Absyn_Aggrfield*),int accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,
0,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp24C->impl))->fields);int
_tmp252=_tmp251 / 8 + (_tmp251 % 8 == 0?0: 1);struct Cyc_List_List*chars=0;{int i=0;
for(0;i < _tmp252;i ++){chars=({struct Cyc_List_List*_tmp253=_cycalloc(sizeof(*
_tmp253));_tmp253->hd=({struct _tuple10*_tmp254=_cycalloc(sizeof(*_tmp254));
_tmp254->f1=Cyc_Tcgenrep_tq_none;_tmp254->f2=Cyc_Absyn_char_typ;_tmp254;});
_tmp253->tl=chars;_tmp253;});}}{void*base_type=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259[0]=({struct Cyc_Absyn_TupleType_struct
_tmp25A;_tmp25A.tag=9;_tmp25A.f1=chars;_tmp25A;});_tmp259;});struct _tuple19
_tmp256;struct Cyc_Dict_Dict*_tmp257;struct Cyc_Tcgenrep_RepInfo*_tmp258;struct
_tuple19*_tmp255=Cyc_Tcgenrep_lookupRep(te,dict,loc,base_type);_tmp256=*_tmp255;
_tmp257=_tmp256.f1;_tmp258=_tmp256.f2;dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(_tmp257,type,_tmp258);}}
else{struct _tuple6*_tmp25B=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*
_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp260;}));struct Cyc_Core_Opt*_tmp25C=({struct Cyc_Core_Opt*_tmp25F=
_cycalloc(sizeof(*_tmp25F));_tmp25F->v=Cyc_Tcgenrep_gen_vardecl(_tmp25B,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str)),0,(void*)3,loc);_tmp25F;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp25B,loc),loc),_tmp25C,0);{struct Cyc_List_List*_tmp25D=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp24C->impl))->fields);dict=Cyc_Tcgenrep_buildRepStruct(({struct
Cyc_Core_Opt*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->v=_tmp24C->name;
_tmp25E;}),te,dict,loc,_tmp25B,(void*)2,type,_tmp25D);}}}else{struct Cyc_Absyn_Aggrdecl*
_tmp261=_tmp249;if(_tmp261->tvs != 0)({void*_tmp262[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp263="gen: unions with parameters not yet supported";
_tag_arr(_tmp263,sizeof(char),_get_zero_arr_size(_tmp263,46));}),_tag_arr(
_tmp262,sizeof(void*),0));});if(_tmp261->impl == 0){struct _tuple6*_tmp264=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp267;}));struct Cyc_Core_Opt*_tmp265=({struct Cyc_Core_Opt*_tmp266=
_cycalloc(sizeof(*_tmp266));_tmp266->v=Cyc_Tcgenrep_gen_vardecl(_tmp264,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_union_str)),0,(void*)3,loc);_tmp266;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp264,loc),loc),_tmp265,1);goto _LL15;}{struct Cyc_List_List*_tmp268=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp261->impl))->fields);struct _tuple6*_tmp269=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp26B;}));dict=Cyc_Tcgenrep_buildRepUnion(({struct Cyc_Core_Opt*_tmp26A=
_cycalloc(sizeof(*_tmp26A));_tmp26A->v=_tmp261->name;_tmp26A;}),te,dict,loc,
_tmp269,(void*)2,type,_tmp268);}}goto _LL15;}_LL3E: if(_tmp1C2 <= (void*)3?1:*((int*)
_tmp1C2)!= 11)goto _LL40;_tmp1DD=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1C2)->f1;if((int)_tmp1DD != 0)goto _LL40;_tmp1DE=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1C2)->f2;_LL3F: if(_tmp1DE != 0?((int(*)(int(*pred)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,_tmp1DE): 0){
int _tmp26C=((int(*)(int(*f)(int,struct Cyc_Absyn_Aggrfield*),int accum,struct Cyc_List_List*
x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,0,_tmp1DE);int _tmp26D=
_tmp26C / 8 + (_tmp26C % 8 == 0?0: 1);void*base_type=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
Cyc_Tcgenrep_tq_none,(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_int(_tmp26D,loc),
Cyc_Absyn_true_conref);struct _tuple19 _tmp26F;struct Cyc_Dict_Dict*_tmp270;struct
Cyc_Tcgenrep_RepInfo*_tmp271;struct _tuple19*_tmp26E=Cyc_Tcgenrep_lookupRep(te,
dict,loc,base_type);_tmp26F=*_tmp26E;_tmp270=_tmp26F.f1;_tmp271=_tmp26F.f2;dict=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(_tmp270,type,_tmp271);}else{struct Cyc_List_List*_tmp272=((
struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Aggrfield*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_nmtype,_tmp1DE);
struct _tuple6*_tmp273=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const
char*_tmp274="rep";_tag_arr(_tmp274,sizeof(char),_get_zero_arr_size(_tmp274,4));})));
dict=Cyc_Tcgenrep_buildRepStruct(0,te,dict,loc,_tmp273,(void*)0,type,_tmp272);}
goto _LL15;_LL40: if(_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)!= 11)goto _LL42;_tmp1DF=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1C2)->f1;if((int)_tmp1DF != 1)
goto _LL42;_tmp1E0=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1C2)->f2;_LL41: {
struct Cyc_List_List*_tmp275=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct
Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_nmtype,
_tmp1E0);struct _tuple6*_tmp276=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({
const char*_tmp277="rep";_tag_arr(_tmp277,sizeof(char),_get_zero_arr_size(_tmp277,
4));})));dict=Cyc_Tcgenrep_buildRepUnion(0,te,dict,loc,_tmp276,(void*)0,type,
_tmp275);goto _LL15;}_LL42: if(_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)!= 12)goto
_LL44;_tmp1E1=((struct Cyc_Absyn_EnumType_struct*)_tmp1C2)->f2;_LL43: if(_tmp1E1 == 
0)({void*_tmp278[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp279="gen(): enum declaration must be present by now";_tag_arr(
_tmp279,sizeof(char),_get_zero_arr_size(_tmp279,47));}),_tag_arr(_tmp278,sizeof(
void*),0));});{struct Cyc_Absyn_Enumdecl _tmp27A=*((struct Cyc_Absyn_Enumdecl*)
_tmp1E1);if(_tmp27A.fields == 0){struct _tuple6*_tmp27B=Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr*_tmp27E=_cycalloc(sizeof(*_tmp27E));_tmp27E[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp27E;}));struct Cyc_Core_Opt*_tmp27C=({struct Cyc_Core_Opt*_tmp27D=
_cycalloc(sizeof(*_tmp27D));_tmp27D->v=Cyc_Tcgenrep_gen_vardecl(_tmp27B,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_enum_str)),0,(void*)3,loc);_tmp27D;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp27B,loc),loc),_tmp27C,1);goto _LL15;}{struct Cyc_List_List*_tmp27F=((struct Cyc_List_List*(*)(
struct _tuple17*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_enumfield_tagnm,(struct Cyc_List_List*)(_tmp27A.fields)->v);
struct _tuple6*_tmp280=Cyc_Tcgenrep_toplevel_name(({struct _tagged_arr*_tmp281=
_cycalloc(sizeof(*_tmp281));_tmp281[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp281;}));dict=Cyc_Tcgenrep_buildRepEnum((struct _tuple6**)& _tmp27A.name,te,
dict,loc,_tmp280,(void*)2,type,_tmp27F);goto _LL15;}}_LL44: if(_tmp1C2 <= (void*)3?
1:*((int*)_tmp1C2)!= 13)goto _LL46;_tmp1E2=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1C2)->f1;_LL45: {struct Cyc_List_List*_tmp282=((struct Cyc_List_List*(*)(
struct _tuple17*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_enumfield_tagnm,_tmp1E2);struct _tuple6*_tmp283=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(({const char*_tmp284="rep";_tag_arr(_tmp284,sizeof(char),
_get_zero_arr_size(_tmp284,4));})));dict=Cyc_Tcgenrep_buildRepEnum(0,te,dict,loc,
_tmp283,(void*)0,type,_tmp282);goto _LL15;}_LL46: if(_tmp1C2 <= (void*)3?1:*((int*)
_tmp1C2)!= 14)goto _LL48;_LL47:({void*_tmp285[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp286="gen() for sizeof_t<-> not yet supported";
_tag_arr(_tmp286,sizeof(char),_get_zero_arr_size(_tmp286,40));}),_tag_arr(
_tmp285,sizeof(void*),0));});_LL48: if(_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)!= 18)
goto _LL4A;_LL49:({void*_tmp287[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp288="gen() for `i not yet supported";
_tag_arr(_tmp288,sizeof(char),_get_zero_arr_size(_tmp288,31));}),_tag_arr(
_tmp287,sizeof(void*),0));});_LL4A: if(_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)!= 17)
goto _LL15;_LL4B:({void*_tmp289[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp28A="gen() for tag_t<-> not yet supported";
_tag_arr(_tmp28A,sizeof(char),_get_zero_arr_size(_tmp28A,37));}),_tag_arr(
_tmp289,sizeof(void*),0));});_LL15:;}return({struct _tuple19*_tmp28B=_cycalloc(
sizeof(*_tmp28B));_tmp28B->f1=dict;_tmp28B->f2=((struct Cyc_Tcgenrep_RepInfo*(*)(
struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup)(dict,type);_tmp28B;});}static int
Cyc_Tcgenrep_not_emitted_filter(struct Cyc_Tcgenrep_RepInfo*ri){return ri->emitted
== 0;}static void Cyc_Tcgenrep_mark_emitted(struct Cyc_Tcgenrep_RepInfo*ri){ri->emitted=
1;}struct _tuple22{struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*
f3;};struct _tuple22 Cyc_Tcgenrep_tcGenrep(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,void*type,struct Cyc_Dict_Dict*dict){Cyc_Tcutil_check_valid_toplevel_type(
loc,te,type);Cyc_Tcutil_check_valid_toplevel_type(loc,te,type);{struct _tuple19
_tmp28D;struct Cyc_Dict_Dict*_tmp28E;struct Cyc_Tcgenrep_RepInfo*_tmp28F;struct
_tuple19*_tmp28C=Cyc_Tcgenrep_lookupRep(te,dict,loc,type);_tmp28D=*_tmp28C;
_tmp28E=_tmp28D.f1;_tmp28F=_tmp28D.f2;{struct Cyc_List_List*_tmp290=Cyc_Tcgenrep_dfs(
_tmp28F);return({struct _tuple22 _tmp291;_tmp291.f1=_tmp28E;_tmp291.f2=_tmp290;
_tmp291.f3=(struct Cyc_Absyn_Exp*)_check_null(_tmp28F->exp);_tmp291;});}}}
