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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*
Cyc_Std_stderr;struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr,struct _tagged_arr);int Cyc_Std_fflush(
struct Cyc_Std___cycFILE*);int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;
};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};int Cyc_Std_vfprintf(
struct Cyc_Std___cycFILE*,struct _tagged_arr,struct _tagged_arr);extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);int Cyc_List_length(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(
struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*
x,int n);int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);struct
_tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*
Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*k);
struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[
14];struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};
struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int
q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*
f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*name;int*identity;void*kind;
};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrInfo{
void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Tqual
tq;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*zero_term;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
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
int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
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
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(struct
_tuple1*,struct _tuple1*);extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value;
extern void*Cyc_Absyn_rel_ns_null;struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(
void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_tagged_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _tagged_arr*name);void*Cyc_Absyn_strctq(
struct _tuple1*name);void*Cyc_Absyn_unionq_typ(struct _tuple1*name);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*
zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _tagged_arr
f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct
_tagged_arr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple1*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple1*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _tagged_arr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _tagged_arr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*
r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct
Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct _tagged_arr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct
_tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _tagged_arr Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*);struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr
fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_unify(void*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};void*Cyc_Tcutil_snd_tqt(
struct _tuple4*);struct _tuple5{unsigned int f1;int f2;};struct _tuple5 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
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
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};struct Cyc_List_List*Cyc_Toc_toc(
struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();int Cyc_Toc_warn_bounds_checks=
0;unsigned int Cyc_Toc_total_bounds_checks=0;unsigned int Cyc_Toc_bounds_checks_eliminated=
0;static struct Cyc_List_List*Cyc_Toc_result_decls=0;static struct Cyc_List_List*Cyc_Toc_tuple_types=
0;static int Cyc_Toc_tuple_type_counter=0;static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";
char Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*
Cyc_Toc_unimp(struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Std_vfprintf(Cyc_Std_stderr,
fmt,ap);({void*_tmp0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp1="\n";
_tag_arr(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,2));}),_tag_arr(_tmp0,
sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);(
int)_throw((void*)Cyc_Toc_Toc_Unimplemented);}static void*Cyc_Toc_toc_impos(
struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Std_vfprintf(Cyc_Std_stderr,fmt,ap);({
void*_tmp2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp3="\n";_tag_arr(
_tmp3,sizeof(char),_get_zero_arr_size(_tmp3,2));}),_tag_arr(_tmp2,sizeof(void*),
0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);(int)_throw((
void*)Cyc_Toc_Toc_Impossible);}char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error\000";
static char _tmp4[5]="curr";static struct _tagged_arr Cyc_Toc_curr_string={_tmp4,
_tmp4,_tmp4 + 5};static struct _tagged_arr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static char _tmp5[4]="tag";static struct _tagged_arr Cyc_Toc_tag_string={_tmp5,_tmp5,
_tmp5 + 4};static struct _tagged_arr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char
_tmp6[14]="_handler_cons";static struct _tagged_arr Cyc_Toc__handler_cons_string={
_tmp6,_tmp6,_tmp6 + 14};static struct _tagged_arr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp7[8]="handler";static struct _tagged_arr Cyc_Toc_handler_string={
_tmp7,_tmp7,_tmp7 + 8};static struct _tagged_arr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp8[14]="_RegionHandle";static struct _tagged_arr Cyc_Toc__RegionHandle_string={
_tmp8,_tmp8,_tmp8 + 14};static struct _tagged_arr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmp9[7]="_throw";static struct _tagged_arr Cyc_Toc__throw_str={_tmp9,
_tmp9,_tmp9 + 7};static struct _tuple1 Cyc_Toc__throw_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)((void*)&
Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmpB[7]="setjmp";static struct
_tagged_arr Cyc_Toc_setjmp_str={_tmpB,_tmpB,_tmpB + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc_setjmp_ev={0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmpD[14]="_push_handler";
static struct _tagged_arr Cyc_Toc__push_handler_str={_tmpD,_tmpD,_tmpD + 14};static
struct _tuple1 Cyc_Toc__push_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((
void*)& Cyc_Toc__push_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char
_tmpF[13]="_pop_handler";static struct _tagged_arr Cyc_Toc__pop_handler_str={_tmpF,
_tmpF,_tmpF + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmp11[12]="_exn_thrown";static struct _tagged_arr Cyc_Toc__exn_thrown_str={
_tmp11,_tmp11,_tmp11 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={
0,(void*)((void*)& Cyc_Toc__exn_thrown_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static
char _tmp13[14]="_npop_handler";static struct _tagged_arr Cyc_Toc__npop_handler_str={
_tmp13,_tmp13,_tmp13 + 14};static struct _tuple1 Cyc_Toc__npop_handler_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)((void*)& Cyc_Toc__npop_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev;static char _tmp15[12]="_check_null";static struct
_tagged_arr Cyc_Toc__check_null_str={_tmp15,_tmp15,_tmp15 + 12};static struct
_tuple1 Cyc_Toc__check_null_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((
void*)& Cyc_Toc__check_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char
_tmp17[28]="_check_known_subscript_null";static struct _tagged_arr Cyc_Toc__check_known_subscript_null_str={
_tmp17,_tmp17,_tmp17 + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp19[31]="_check_known_subscript_notnull";static struct _tagged_arr Cyc_Toc__check_known_subscript_notnull_str={
_tmp19,_tmp19,_tmp19 + 31};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp1B[25]="_check_unknown_subscript";
static struct _tagged_arr Cyc_Toc__check_unknown_subscript_str={_tmp1B,_tmp1B,
_tmp1B + 25};static struct _tuple1 Cyc_Toc__check_unknown_subscript_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_unknown_subscript_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1,& Cyc_Toc__check_unknown_subscript_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_unknown_subscript_ev={0,(void*)((void*)& Cyc_Toc__check_unknown_subscript_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_unknown_subscript_e=&
Cyc_Toc__check_unknown_subscript_ev;static char _tmp1D[12]="_tagged_arr";static
struct _tagged_arr Cyc_Toc__tagged_arr_str={_tmp1D,_tmp1D,_tmp1D + 12};static struct
_tuple1 Cyc_Toc__tagged_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_re={1,& Cyc_Toc__tagged_arr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={0,(void*)((
void*)& Cyc_Toc__tagged_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_e=& Cyc_Toc__tagged_arr_ev;static char
_tmp1F[9]="_tag_arr";static struct _tagged_arr Cyc_Toc__tag_arr_str={_tmp1F,_tmp1F,
_tmp1F + 9};static struct _tuple1 Cyc_Toc__tag_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_arr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_arr_re={1,&
Cyc_Toc__tag_arr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={
0,(void*)((void*)& Cyc_Toc__tag_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_arr_e=& Cyc_Toc__tag_arr_ev;static char
_tmp21[14]="_init_tag_arr";static struct _tagged_arr Cyc_Toc__init_tag_arr_str={
_tmp21,_tmp21,_tmp21 + 14};static struct _tuple1 Cyc_Toc__init_tag_arr_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__init_tag_arr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__init_tag_arr_re={1,& Cyc_Toc__init_tag_arr_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__init_tag_arr_ev={0,(void*)((void*)& Cyc_Toc__init_tag_arr_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__init_tag_arr_e=&
Cyc_Toc__init_tag_arr_ev;static char _tmp23[11]="_untag_arr";static struct
_tagged_arr Cyc_Toc__untag_arr_str={_tmp23,_tmp23,_tmp23 + 11};static struct _tuple1
Cyc_Toc__untag_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_arr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_arr_re={1,& Cyc_Toc__untag_arr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={0,(void*)((
void*)& Cyc_Toc__untag_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__untag_arr_e=& Cyc_Toc__untag_arr_ev;static char _tmp25[
14]="_get_arr_size";static struct _tagged_arr Cyc_Toc__get_arr_size_str={_tmp25,
_tmp25,_tmp25 + 14};static struct _tuple1 Cyc_Toc__get_arr_size_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__get_arr_size_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_arr_size_re={
1,& Cyc_Toc__get_arr_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_arr_size_ev={
0,(void*)((void*)& Cyc_Toc__get_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_arr_size_e=& Cyc_Toc__get_arr_size_ev;
static char _tmp27[19]="_get_zero_arr_size";static struct _tagged_arr Cyc_Toc__get_zero_arr_size_str={
_tmp27,_tmp27,_tmp27 + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__get_zero_arr_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp29[17]="_tagged_arr_plus";static struct _tagged_arr Cyc_Toc__tagged_arr_plus_str={
_tmp29,_tmp29,_tmp29 + 17};static struct _tuple1 Cyc_Toc__tagged_arr_plus_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tagged_arr_plus_re={1,& Cyc_Toc__tagged_arr_plus_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_plus_ev={0,(void*)((void*)& Cyc_Toc__tagged_arr_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_plus_e=&
Cyc_Toc__tagged_arr_plus_ev;static char _tmp2B[15]="_zero_arr_plus";static struct
_tagged_arr Cyc_Toc__zero_arr_plus_str={_tmp2B,_tmp2B,_tmp2B + 15};static struct
_tuple1 Cyc_Toc__zero_arr_plus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__zero_arr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static
char _tmp2D[25]="_tagged_arr_inplace_plus";static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_str={
_tmp2D,_tmp2D,_tmp2D + 25};static struct _tuple1 Cyc_Toc__tagged_arr_inplace_plus_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={1,& Cyc_Toc__tagged_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__tagged_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_inplace_plus_e=& Cyc_Toc__tagged_arr_inplace_plus_ev;
static char _tmp2F[23]="_zero_arr_inplace_plus";static struct _tagged_arr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp2F,_tmp2F,_tmp2F + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__zero_arr_inplace_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp31[30]="_tagged_arr_inplace_plus_post";static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={
_tmp31,_tmp31,_tmp31 + 30};static struct _tuple1 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={1,&
Cyc_Toc__tagged_arr_inplace_plus_post_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_inplace_plus_post_e=&
Cyc_Toc__tagged_arr_inplace_plus_post_ev;static char _tmp33[28]="_zero_arr_inplace_plus_post";
static struct _tagged_arr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp33,_tmp33,
_tmp33 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__zero_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp35[10]="_cycalloc";static struct _tagged_arr Cyc_Toc__cycalloc_str={
_tmp35,_tmp35,_tmp35 + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char
_tmp37[11]="_cyccalloc";static struct _tagged_arr Cyc_Toc__cyccalloc_str={_tmp37,
_tmp37,_tmp37 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={
1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char
_tmp39[17]="_cycalloc_atomic";static struct _tagged_arr Cyc_Toc__cycalloc_atomic_str={
_tmp39,_tmp39,_tmp39 + 17};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=&
Cyc_Toc__cycalloc_atomic_ev;static char _tmp3B[18]="_cyccalloc_atomic";static
struct _tagged_arr Cyc_Toc__cyccalloc_atomic_str={_tmp3B,_tmp3B,_tmp3B + 18};static
struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={
1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmp3D[15]="_region_malloc";static struct _tagged_arr Cyc_Toc__region_malloc_str={
_tmp3D,_tmp3D,_tmp3D + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((void*)& Cyc_Toc__region_malloc_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev;static char _tmp3F[15]="_region_calloc";static struct
_tagged_arr Cyc_Toc__region_calloc_str={_tmp3F,_tmp3F,_tmp3F + 15};static struct
_tuple1 Cyc_Toc__region_calloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_calloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)((
void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static
char _tmp41[13]="_check_times";static struct _tagged_arr Cyc_Toc__check_times_str={
_tmp41,_tmp41,_tmp41 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static char _tmp43[12]="_new_region";static struct
_tagged_arr Cyc_Toc__new_region_str={_tmp43,_tmp43,_tmp43 + 12};static struct
_tuple1 Cyc_Toc__new_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__new_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)((
void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char
_tmp45[13]="_push_region";static struct _tagged_arr Cyc_Toc__push_region_str={
_tmp45,_tmp45,_tmp45 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static char _tmp47[12]="_pop_region";static struct
_tagged_arr Cyc_Toc__pop_region_str={_tmp47,_tmp47,_tmp47 + 12};static struct
_tuple1 Cyc_Toc__pop_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)((
void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char
_tmp49[14]="_reset_region";static struct _tagged_arr Cyc_Toc__reset_region_str={
_tmp49,_tmp49,_tmp49 + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)((void*)& Cyc_Toc__reset_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=&
Cyc_Toc__reset_region_ev;static char _tmp4B[19]="_throw_arraybounds";static struct
_tagged_arr Cyc_Toc__throw_arraybounds_str={_tmp4B,_tmp4B,_tmp4B + 19};static
struct _tuple1 Cyc_Toc__throw_arraybounds_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={
1,& Cyc_Toc__throw_arraybounds_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={
0,(void*)((void*)& Cyc_Toc__throw_arraybounds_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmp4D[26]="_tagged_ptr_decrease_size";static struct _tagged_arr Cyc_Toc__tagged_ptr_decrease_size_str={
_tmp4D,_tmp4D,_tmp4D + 26};static struct _tuple1 Cyc_Toc__tagged_ptr_decrease_size_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_ptr_decrease_size_re={1,& Cyc_Toc__tagged_ptr_decrease_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_ptr_decrease_size_ev={
0,(void*)((void*)& Cyc_Toc__tagged_ptr_decrease_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_ptr_decrease_size_e=& Cyc_Toc__tagged_ptr_decrease_size_ev;
static char _tmp4F[7]="_bogus";static struct _tagged_arr Cyc_Toc__bogus_str={_tmp4F,
_tmp4F,_tmp4F + 7};static struct _tuple1 Cyc_Toc__bogus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__bogus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__bogus_re={1,& Cyc_Toc__bogus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__bogus_ev={0,(void*)((void*)&
Cyc_Toc__bogus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*
Cyc_Toc__bogus_e=& Cyc_Toc__bogus_ev;static struct Cyc_Absyn_UnknownAggr_struct Cyc_Toc_tagged_arr_typ_u={
0,(void*)((void*)0),& Cyc_Toc__tagged_arr_pr};static struct Cyc_Absyn_AggrType_struct
Cyc_Toc_tagged_arr_typ_v={10,{(void*)((void*)& Cyc_Toc_tagged_arr_typ_u),0}};
static void*Cyc_Toc_tagged_arr_typ=(void*)& Cyc_Toc_tagged_arr_typ_v;static struct
Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(skip_stmt_opt == 0)skip_stmt_opt=({
struct Cyc_Absyn_Stmt**_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=Cyc_Absyn_skip_stmt(
0);_tmp53;});return*((struct Cyc_Absyn_Stmt**)_check_null(skip_stmt_opt));}struct
_tuple6{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp54=(void*)e->r;void*_tmp55;char _tmp56;void*
_tmp57;short _tmp58;void*_tmp59;int _tmp5A;void*_tmp5B;long long _tmp5C;void*_tmp5D;
struct Cyc_Absyn_Exp*_tmp5E;struct Cyc_List_List*_tmp5F;struct Cyc_List_List*_tmp60;
struct Cyc_List_List*_tmp61;struct Cyc_List_List*_tmp62;struct Cyc_List_List*_tmp63;
_LL1: if(*((int*)_tmp54)!= 0)goto _LL3;_tmp55=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp54)->f1;if(_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 0)goto _LL3;_tmp56=((struct
Cyc_Absyn_Char_c_struct*)_tmp55)->f2;_LL2: return _tmp56 == '\000';_LL3: if(*((int*)
_tmp54)!= 0)goto _LL5;_tmp57=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp54)->f1;
if(_tmp57 <= (void*)1?1:*((int*)_tmp57)!= 1)goto _LL5;_tmp58=((struct Cyc_Absyn_Short_c_struct*)
_tmp57)->f2;_LL4: return _tmp58 == 0;_LL5: if(*((int*)_tmp54)!= 0)goto _LL7;_tmp59=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp54)->f1;if(_tmp59 <= (void*)1?1:*((
int*)_tmp59)!= 2)goto _LL7;_tmp5A=((struct Cyc_Absyn_Int_c_struct*)_tmp59)->f2;
_LL6: return _tmp5A == 0;_LL7: if(*((int*)_tmp54)!= 0)goto _LL9;_tmp5B=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp54)->f1;if(_tmp5B <= (void*)1?1:*((int*)
_tmp5B)!= 3)goto _LL9;_tmp5C=((struct Cyc_Absyn_LongLong_c_struct*)_tmp5B)->f2;
_LL8: return _tmp5C == 0;_LL9: if(*((int*)_tmp54)!= 0)goto _LLB;_tmp5D=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp54)->f1;if((int)_tmp5D != 0)goto _LLB;_LLA:
return 1;_LLB: if(*((int*)_tmp54)!= 13)goto _LLD;_tmp5E=((struct Cyc_Absyn_Cast_e_struct*)
_tmp54)->f2;_LLC: return Cyc_Toc_is_zero(_tmp5E);_LLD: if(*((int*)_tmp54)!= 24)goto
_LLF;_tmp5F=((struct Cyc_Absyn_Tuple_e_struct*)_tmp54)->f1;_LLE: return((int(*)(
int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,
_tmp5F);_LLF: if(*((int*)_tmp54)!= 26)goto _LL11;_tmp60=((struct Cyc_Absyn_Array_e_struct*)
_tmp54)->f1;_LL10: _tmp61=_tmp60;goto _LL12;_LL11: if(*((int*)_tmp54)!= 28)goto
_LL13;_tmp61=((struct Cyc_Absyn_Struct_e_struct*)_tmp54)->f3;_LL12: _tmp62=_tmp61;
goto _LL14;_LL13: if(*((int*)_tmp54)!= 25)goto _LL15;_tmp62=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp54)->f2;_LL14: _tmp63=_tmp62;goto _LL16;_LL15: if(*((int*)_tmp54)!= 34)goto
_LL17;_tmp63=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp54)->f2;_LL16: for(0;
_tmp63 != 0;_tmp63=_tmp63->tl){if(!Cyc_Toc_is_zero((*((struct _tuple6*)((struct
_tuple6*)_tmp63->hd))).f2))return 0;}return 1;_LL17:;_LL18: return 0;_LL0:;}static
int Cyc_Toc_is_nullable(void*t){void*_tmp64=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp65;struct Cyc_Absyn_Conref*_tmp66;_LL1A: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)
!= 4)goto _LL1C;_tmp65=((struct Cyc_Absyn_PointerType_struct*)_tmp64)->f1;_tmp66=
_tmp65.nullable;_LL1B: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp66);_LL1C:;_LL1D:({void*_tmp67[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp68="is_nullable";_tag_arr(
_tmp68,sizeof(char),_get_zero_arr_size(_tmp68,12));}),_tag_arr(_tmp67,sizeof(
void*),0));});_LL19:;}static struct _tuple1*Cyc_Toc_collapse_qvar_tag(struct
_tuple1*x,struct _tagged_arr tag){return({struct _tuple1*_tmp69=_cycalloc(sizeof(*
_tmp69));_tmp69->f1=(*((struct _tuple1*)x)).f1;_tmp69->f2=({struct _tagged_arr*
_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A[0]=(struct _tagged_arr)Cyc_Std_strconcat(*((
struct _tagged_arr*)(*((struct _tuple1*)x)).f2),tag);_tmp6A;});_tmp69;});}struct
_tuple7{struct _tagged_arr*f1;struct Cyc_List_List*f2;};static struct _tagged_arr*
Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){{struct Cyc_List_List*_tmp6B=Cyc_Toc_tuple_types;
for(0;_tmp6B != 0;_tmp6B=_tmp6B->tl){struct _tuple7 _tmp6D;struct _tagged_arr*_tmp6E;
struct Cyc_List_List*_tmp6F;struct _tuple7*_tmp6C=(struct _tuple7*)_tmp6B->hd;
_tmp6D=*_tmp6C;_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;{struct Cyc_List_List*_tmp70=
tqs0;for(0;_tmp70 != 0?_tmp6F != 0: 0;(_tmp70=_tmp70->tl,_tmp6F=_tmp6F->tl)){if(!
Cyc_Tcutil_unify((*((struct _tuple4*)((struct _tuple4*)_tmp70->hd))).f2,(void*)
_tmp6F->hd))break;}if(_tmp70 == 0?_tmp6F == 0: 0)return _tmp6E;}}}{struct _tagged_arr*
x=({struct _tagged_arr*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=(struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp80;_tmp80.tag=1;_tmp80.f1=(int)((
unsigned int)Cyc_Toc_tuple_type_counter ++);{void*_tmp7E[1]={& _tmp80};Cyc_Std_aprintf(({
const char*_tmp7F="_tuple%d";_tag_arr(_tmp7F,sizeof(char),_get_zero_arr_size(
_tmp7F,9));}),_tag_arr(_tmp7E,sizeof(void*),1));}});_tmp7D;});struct Cyc_List_List*
ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple4*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_snd_tqt,tqs0);struct Cyc_List_List*_tmp71=0;struct Cyc_List_List*
ts2=ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){_tmp71=({struct Cyc_List_List*
_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->hd=({struct Cyc_Absyn_Aggrfield*_tmp73=
_cycalloc(sizeof(*_tmp73));_tmp73->name=Cyc_Absyn_fieldname(i);_tmp73->tq=Cyc_Toc_mt_tq;
_tmp73->type=(void*)((void*)ts2->hd);_tmp73->width=0;_tmp73->attributes=0;_tmp73;});
_tmp72->tl=_tmp71;_tmp72;});}}_tmp71=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp71);{struct Cyc_Absyn_Aggrdecl*_tmp74=({struct Cyc_Absyn_Aggrdecl*
_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->kind=(void*)((void*)0);_tmp7A->sc=(
void*)((void*)2);_tmp7A->name=({struct _tuple1*_tmp7C=_cycalloc(sizeof(*_tmp7C));
_tmp7C->f1=Cyc_Absyn_rel_ns_null;_tmp7C->f2=x;_tmp7C;});_tmp7A->tvs=0;_tmp7A->impl=({
struct Cyc_Absyn_AggrdeclImpl*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->exist_vars=
0;_tmp7B->rgn_po=0;_tmp7B->fields=_tmp71;_tmp7B;});_tmp7A->attributes=0;_tmp7A;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp75=_cycalloc(sizeof(*_tmp75));
_tmp75->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp76=
_cycalloc(sizeof(*_tmp76));_tmp76[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp77;
_tmp77.tag=4;_tmp77.f1=_tmp74;_tmp77;});_tmp76;}),0);_tmp75->tl=Cyc_Toc_result_decls;
_tmp75;});Cyc_Toc_tuple_types=({struct Cyc_List_List*_tmp78=_cycalloc(sizeof(*
_tmp78));_tmp78->hd=({struct _tuple7*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->f1=
x;_tmp79->f2=ts;_tmp79;});_tmp78->tl=Cyc_Toc_tuple_types;_tmp78;});return x;}}}
struct _tuple1*Cyc_Toc_temp_var(){return({struct _tuple1*_tmp81=_cycalloc(sizeof(*
_tmp81));_tmp81->f1=(void*)0;_tmp81->f2=({struct _tagged_arr*_tmp82=_cycalloc(
sizeof(*_tmp82));_tmp82[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp85;_tmp85.tag=1;_tmp85.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;{void*
_tmp83[1]={& _tmp85};Cyc_Std_aprintf(({const char*_tmp84="_tmp%X";_tag_arr(_tmp84,
sizeof(char),_get_zero_arr_size(_tmp84,7));}),_tag_arr(_tmp83,sizeof(void*),1));}});
_tmp82;});_tmp81;});}static struct _tagged_arr*Cyc_Toc_fresh_label(){return({
struct _tagged_arr*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp89;_tmp89.tag=1;_tmp89.f1=(unsigned int)Cyc_Toc_fresh_label_counter
++;{void*_tmp87[1]={& _tmp89};Cyc_Std_aprintf(({const char*_tmp88="_LL%X";_tag_arr(
_tmp88,sizeof(char),_get_zero_arr_size(_tmp88,6));}),_tag_arr(_tmp87,sizeof(void*),
1));}});_tmp86;});}static struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*
td,struct _tuple1*name,int carries_value){int ans=0;struct Cyc_List_List*_tmp8A=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(
name,((struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp8A))->hd)->name)
!= 0){if((carries_value?((struct Cyc_Absyn_Tunionfield*)_tmp8A->hd)->typs != 0: 0)?
1:(!carries_value?((struct Cyc_Absyn_Tunionfield*)_tmp8A->hd)->typs == 0: 0))++ ans;
_tmp8A=_tmp8A->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static int Cyc_Toc_num_void_tags(
struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*_tmp8B=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;_tmp8B != 0;_tmp8B=_tmp8B->tl){
if(((struct Cyc_Absyn_Tunionfield*)_tmp8B->hd)->typs == 0)++ ans;}}return ans;}
static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*Cyc_Toc_arg_to_c(struct
_tuple2*a){struct Cyc_Core_Opt*_tmp8D;struct Cyc_Absyn_Tqual _tmp8E;void*_tmp8F;
struct _tuple2 _tmp8C=*((struct _tuple2*)a);_tmp8D=_tmp8C.f1;_tmp8E=_tmp8C.f2;
_tmp8F=_tmp8C.f3;return({struct _tuple2*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->f1=
_tmp8D;_tmp90->f2=_tmp8E;_tmp90->f3=Cyc_Toc_typ_to_c(_tmp8F);_tmp90;});}static
struct _tuple4*Cyc_Toc_typ_to_c_f(struct _tuple4*x){struct Cyc_Absyn_Tqual _tmp92;
void*_tmp93;struct _tuple4 _tmp91=*((struct _tuple4*)x);_tmp92=_tmp91.f1;_tmp93=
_tmp91.f2;return({struct _tuple4*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->f1=
_tmp92;_tmp94->f2=Cyc_Toc_typ_to_c(_tmp93);_tmp94;});}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp95=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp96;void*
_tmp97;struct Cyc_Absyn_Tqual _tmp98;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Conref*
_tmp9A;struct Cyc_Core_Opt*_tmp9B;struct Cyc_Core_Opt _tmp9C;void*_tmp9D;_LL1F: if(
_tmp95 <= (void*)3?1:*((int*)_tmp95)!= 7)goto _LL21;_tmp96=((struct Cyc_Absyn_ArrayType_struct*)
_tmp95)->f1;_tmp97=(void*)_tmp96.elt_type;_tmp98=_tmp96.tq;_tmp99=_tmp96.num_elts;
_tmp9A=_tmp96.zero_term;_LL20: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp97),_tmp98,_tmp99,Cyc_Absyn_false_conref);_LL21: if(_tmp95 <= (void*)3?1:*((
int*)_tmp95)!= 0)goto _LL23;_tmp9B=((struct Cyc_Absyn_Evar_struct*)_tmp95)->f2;if(
_tmp9B == 0)goto _LL23;_tmp9C=*_tmp9B;_tmp9D=(void*)_tmp9C.v;_LL22: return Cyc_Toc_typ_to_c_array(
_tmp9D);_LL23:;_LL24: return Cyc_Toc_typ_to_c(t);_LL1E:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->name=f->name;_tmp9E->tq=f->tq;_tmp9E->type=(
void*)Cyc_Toc_typ_to_c((void*)f->type);_tmp9E->width=f->width;_tmp9E->attributes=
f->attributes;_tmp9E;});}static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*
fs){return;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp9F=t;struct Cyc_Core_Opt*
_tmpA0;struct Cyc_Core_Opt*_tmpA1;struct Cyc_Core_Opt _tmpA2;void*_tmpA3;struct Cyc_Absyn_Tvar*
_tmpA4;struct Cyc_Absyn_TunionInfo _tmpA5;void*_tmpA6;struct Cyc_Absyn_TunionFieldInfo
_tmpA7;void*_tmpA8;struct Cyc_Absyn_Tuniondecl*_tmpA9;struct Cyc_Absyn_Tunionfield*
_tmpAA;struct Cyc_Absyn_PtrInfo _tmpAB;void*_tmpAC;struct Cyc_Absyn_Tqual _tmpAD;
struct Cyc_Absyn_Conref*_tmpAE;struct Cyc_Absyn_ArrayInfo _tmpAF;void*_tmpB0;struct
Cyc_Absyn_Tqual _tmpB1;struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_FnInfo _tmpB3;
void*_tmpB4;struct Cyc_List_List*_tmpB5;int _tmpB6;struct Cyc_Absyn_VarargInfo*
_tmpB7;struct Cyc_List_List*_tmpB8;struct Cyc_List_List*_tmpB9;void*_tmpBA;struct
Cyc_List_List*_tmpBB;struct Cyc_Absyn_AggrInfo _tmpBC;void*_tmpBD;struct Cyc_List_List*
_tmpBE;struct _tuple1*_tmpBF;struct Cyc_List_List*_tmpC0;struct _tuple1*_tmpC1;
struct Cyc_List_List*_tmpC2;struct Cyc_Absyn_Typedefdecl*_tmpC3;struct Cyc_Core_Opt*
_tmpC4;void*_tmpC5;_LL26: if((int)_tmp9F != 0)goto _LL28;_LL27: return t;_LL28: if(
_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 0)goto _LL2A;_tmpA0=((struct Cyc_Absyn_Evar_struct*)
_tmp9F)->f2;if(_tmpA0 != 0)goto _LL2A;_LL29: return Cyc_Absyn_sint_typ;_LL2A: if(
_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 0)goto _LL2C;_tmpA1=((struct Cyc_Absyn_Evar_struct*)
_tmp9F)->f2;if(_tmpA1 == 0)goto _LL2C;_tmpA2=*_tmpA1;_tmpA3=(void*)_tmpA2.v;_LL2B:
return Cyc_Toc_typ_to_c(_tmpA3);_LL2C: if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 1)
goto _LL2E;_tmpA4=((struct Cyc_Absyn_VarType_struct*)_tmp9F)->f1;_LL2D: if(Cyc_Tcutil_tvar_kind(
_tmpA4)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}_LL2E:
if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 2)goto _LL30;_tmpA5=((struct Cyc_Absyn_TunionType_struct*)
_tmp9F)->f1;_tmpA6=(void*)_tmpA5.tunion_info;if(*((int*)_tmpA6)!= 1)goto _LL30;
_LL2F: return Cyc_Absyn_void_star_typ();_LL30: if(_tmp9F <= (void*)3?1:*((int*)
_tmp9F)!= 2)goto _LL32;_LL31:({void*_tmpC6[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmpC7="unresolved TunionType";
_tag_arr(_tmpC7,sizeof(char),_get_zero_arr_size(_tmpC7,22));}),_tag_arr(_tmpC6,
sizeof(void*),0));});_LL32: if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 3)goto _LL34;
_tmpA7=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp9F)->f1;_tmpA8=(void*)
_tmpA7.field_info;if(*((int*)_tmpA8)!= 1)goto _LL34;_tmpA9=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpA8)->f1;_tmpAA=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpA8)->f2;_LL33:
if(_tmpAA->typs == 0){if(_tmpA9->is_xtunion)return Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);else{return Cyc_Absyn_uint_typ;}}
else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmpAA->name,({const char*
_tmpC8="_struct";_tag_arr(_tmpC8,sizeof(char),_get_zero_arr_size(_tmpC8,8));})));}
_LL34: if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 3)goto _LL36;_LL35:({void*_tmpC9[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmpCA="unresolved TunionFieldType";_tag_arr(_tmpCA,sizeof(char),
_get_zero_arr_size(_tmpCA,27));}),_tag_arr(_tmpC9,sizeof(void*),0));});_LL36: if(
_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 4)goto _LL38;_tmpAB=((struct Cyc_Absyn_PointerType_struct*)
_tmp9F)->f1;_tmpAC=(void*)_tmpAB.elt_typ;_tmpAD=_tmpAB.tq;_tmpAE=_tmpAB.bounds;
_LL37: _tmpAC=Cyc_Toc_typ_to_c_array(_tmpAC);{void*_tmpCB=(void*)(Cyc_Absyn_compress_conref(
_tmpAE))->v;void*_tmpCC;_LL5F: if(_tmpCB <= (void*)1?1:*((int*)_tmpCB)!= 0)goto
_LL61;_tmpCC=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpCB)->f1;if((int)
_tmpCC != 0)goto _LL61;_LL60: return Cyc_Toc_tagged_arr_typ;_LL61:;_LL62: return Cyc_Absyn_star_typ(
_tmpAC,(void*)2,_tmpAD,Cyc_Absyn_false_conref);_LL5E:;}_LL38: if(_tmp9F <= (void*)
3?1:*((int*)_tmp9F)!= 5)goto _LL3A;_LL39: goto _LL3B;_LL3A: if((int)_tmp9F != 1)goto
_LL3C;_LL3B: goto _LL3D;_LL3C: if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 6)goto _LL3E;
_LL3D: return t;_LL3E: if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 7)goto _LL40;_tmpAF=((
struct Cyc_Absyn_ArrayType_struct*)_tmp9F)->f1;_tmpB0=(void*)_tmpAF.elt_type;
_tmpB1=_tmpAF.tq;_tmpB2=_tmpAF.num_elts;_LL3F: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmpB0),_tmpB1,_tmpB2,Cyc_Absyn_false_conref);_LL40: if(_tmp9F <= (void*)3?1:*((
int*)_tmp9F)!= 8)goto _LL42;_tmpB3=((struct Cyc_Absyn_FnType_struct*)_tmp9F)->f1;
_tmpB4=(void*)_tmpB3.ret_typ;_tmpB5=_tmpB3.args;_tmpB6=_tmpB3.c_varargs;_tmpB7=
_tmpB3.cyc_varargs;_tmpB8=_tmpB3.attributes;_LL41: {struct Cyc_List_List*_tmpCD=0;
for(0;_tmpB8 != 0;_tmpB8=_tmpB8->tl){void*_tmpCE=(void*)_tmpB8->hd;_LL64: if((int)
_tmpCE != 3)goto _LL66;_LL65: goto _LL67;_LL66: if((int)_tmpCE != 4)goto _LL68;_LL67:
goto _LL69;_LL68: if(_tmpCE <= (void*)17?1:*((int*)_tmpCE)!= 3)goto _LL6A;_LL69:
continue;_LL6A: if(_tmpCE <= (void*)17?1:*((int*)_tmpCE)!= 4)goto _LL6C;_LL6B:
continue;_LL6C:;_LL6D: _tmpCD=({struct Cyc_List_List*_tmpCF=_cycalloc(sizeof(*
_tmpCF));_tmpCF->hd=(void*)((void*)_tmpB8->hd);_tmpCF->tl=_tmpCD;_tmpCF;});goto
_LL63;_LL63:;}{struct Cyc_List_List*_tmpD0=((struct Cyc_List_List*(*)(struct
_tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmpB5);if(_tmpB7 != 0){void*_tmpD1=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ((void*)
_tmpB7->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct _tuple2*
_tmpD2=({struct _tuple2*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->f1=_tmpB7->name;
_tmpD4->f2=_tmpB7->tq;_tmpD4->f3=_tmpD1;_tmpD4;});_tmpD0=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpD0,({
struct Cyc_List_List*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->hd=_tmpD2;_tmpD3->tl=
0;_tmpD3;}));}return(void*)({struct Cyc_Absyn_FnType_struct*_tmpD5=_cycalloc(
sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Absyn_FnType_struct _tmpD6;_tmpD6.tag=8;
_tmpD6.f1=({struct Cyc_Absyn_FnInfo _tmpD7;_tmpD7.tvars=0;_tmpD7.effect=0;_tmpD7.ret_typ=(
void*)Cyc_Toc_typ_to_c(_tmpB4);_tmpD7.args=_tmpD0;_tmpD7.c_varargs=_tmpB6;_tmpD7.cyc_varargs=
0;_tmpD7.rgn_po=0;_tmpD7.attributes=_tmpCD;_tmpD7;});_tmpD6;});_tmpD5;});}}_LL42:
if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 9)goto _LL44;_tmpB9=((struct Cyc_Absyn_TupleType_struct*)
_tmp9F)->f1;_LL43: _tmpB9=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmpB9);{
struct _tagged_arr*_tmpD8=Cyc_Toc_add_tuple_type(_tmpB9);return Cyc_Absyn_strct(
_tmpD8);}_LL44: if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 11)goto _LL46;_tmpBA=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp9F)->f1;_tmpBB=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp9F)->f2;_LL45: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpD9=
_cycalloc(sizeof(*_tmpD9));_tmpD9[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpDA;
_tmpDA.tag=11;_tmpDA.f1=(void*)_tmpBA;_tmpDA.f2=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_aggrfield_to_c,_tmpBB);_tmpDA;});_tmpD9;});_LL46: if(_tmp9F <= (void*)3?1:*((
int*)_tmp9F)!= 10)goto _LL48;_tmpBC=((struct Cyc_Absyn_AggrType_struct*)_tmp9F)->f1;
_tmpBD=(void*)_tmpBC.aggr_info;_tmpBE=_tmpBC.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*
_tmpDB=Cyc_Absyn_get_known_aggrdecl(_tmpBD);if((void*)_tmpDB->kind == (void*)1)
return Cyc_Absyn_unionq_typ(_tmpDB->name);return Cyc_Absyn_strctq(_tmpDB->name);}
_LL48: if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 12)goto _LL4A;_tmpBF=((struct Cyc_Absyn_EnumType_struct*)
_tmp9F)->f1;_LL49: return t;_LL4A: if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 13)goto
_LL4C;_tmpC0=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp9F)->f1;_LL4B: Cyc_Toc_enumfields_to_c(
_tmpC0);return t;_LL4C: if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 16)goto _LL4E;
_tmpC1=((struct Cyc_Absyn_TypedefType_struct*)_tmp9F)->f1;_tmpC2=((struct Cyc_Absyn_TypedefType_struct*)
_tmp9F)->f2;_tmpC3=((struct Cyc_Absyn_TypedefType_struct*)_tmp9F)->f3;_tmpC4=((
struct Cyc_Absyn_TypedefType_struct*)_tmp9F)->f4;_LL4D: if(_tmpC4 == 0)return(void*)({
struct Cyc_Absyn_TypedefType_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({
struct Cyc_Absyn_TypedefType_struct _tmpDD;_tmpDD.tag=16;_tmpDD.f1=_tmpC1;_tmpDD.f2=
0;_tmpDD.f3=_tmpC3;_tmpDD.f4=0;_tmpDD;});_tmpDC;});else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_TypedefType_struct
_tmpDF;_tmpDF.tag=16;_tmpDF.f1=_tmpC1;_tmpDF.f2=0;_tmpDF.f3=_tmpC3;_tmpDF.f4=({
struct Cyc_Core_Opt*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->v=(void*)Cyc_Toc_typ_to_c_array((
void*)_tmpC4->v);_tmpE0;});_tmpDF;});_tmpDE;});}_LL4E: if(_tmp9F <= (void*)3?1:*((
int*)_tmp9F)!= 14)goto _LL50;_LL4F: return Cyc_Absyn_uint_typ;_LL50: if(_tmp9F <= (
void*)3?1:*((int*)_tmp9F)!= 17)goto _LL52;_LL51: return Cyc_Absyn_sint_typ;_LL52:
if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 15)goto _LL54;_tmpC5=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp9F)->f1;_LL53: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL54: if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 18)goto _LL56;_LL55:({
void*_tmpE1[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpE2="Toc::typ_to_c: type translation passed a type integer";
_tag_arr(_tmpE2,sizeof(char),_get_zero_arr_size(_tmpE2,54));}),_tag_arr(_tmpE1,
sizeof(void*),0));});_LL56: if((int)_tmp9F != 2)goto _LL58;_LL57:({void*_tmpE3[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmpE4="Toc::typ_to_c: type translation passed the heap region";_tag_arr(_tmpE4,
sizeof(char),_get_zero_arr_size(_tmpE4,55));}),_tag_arr(_tmpE3,sizeof(void*),0));});
_LL58: if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 19)goto _LL5A;_LL59:({void*_tmpE5[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpE6="Toc::typ_to_c: type translation passed the access eff";
_tag_arr(_tmpE6,sizeof(char),_get_zero_arr_size(_tmpE6,54));}),_tag_arr(_tmpE5,
sizeof(void*),0));});_LL5A: if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 20)goto _LL5C;
_LL5B:({void*_tmpE7[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpE8="Toc::typ_to_c: type translation passed the join eff";_tag_arr(
_tmpE8,sizeof(char),_get_zero_arr_size(_tmpE8,52));}),_tag_arr(_tmpE7,sizeof(
void*),0));});_LL5C: if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 21)goto _LL25;_LL5D:({
void*_tmpE9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpEA="Toc::typ_to_c: type translation passed the regions eff";
_tag_arr(_tmpEA,sizeof(char),_get_zero_arr_size(_tmpEA,55));}),_tag_arr(_tmpE9,
sizeof(void*),0));});_LL25:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmpEB=t;struct
Cyc_Absyn_ArrayInfo _tmpEC;void*_tmpED;struct Cyc_Absyn_Tqual _tmpEE;_LL6F: if(
_tmpEB <= (void*)3?1:*((int*)_tmpEB)!= 7)goto _LL71;_tmpEC=((struct Cyc_Absyn_ArrayType_struct*)
_tmpEB)->f1;_tmpED=(void*)_tmpEC.elt_type;_tmpEE=_tmpEC.tq;_LL70: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ(_tmpED,(void*)2,_tmpEE,Cyc_Absyn_false_conref),e,l);_LL71:;
_LL72: return Cyc_Absyn_cast_exp(t,e,l);_LL6E:;}static int Cyc_Toc_atomic_typ(void*t){
void*_tmpEF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpF0;void*_tmpF1;
struct Cyc_Absyn_AggrInfo _tmpF2;void*_tmpF3;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_TunionFieldInfo
_tmpF5;void*_tmpF6;struct Cyc_Absyn_Tuniondecl*_tmpF7;struct Cyc_Absyn_Tunionfield*
_tmpF8;struct Cyc_List_List*_tmpF9;_LL74: if((int)_tmpEF != 0)goto _LL76;_LL75:
return 1;_LL76: if(_tmpEF <= (void*)3?1:*((int*)_tmpEF)!= 1)goto _LL78;_LL77: return 0;
_LL78: if(_tmpEF <= (void*)3?1:*((int*)_tmpEF)!= 5)goto _LL7A;_LL79: goto _LL7B;_LL7A:
if(_tmpEF <= (void*)3?1:*((int*)_tmpEF)!= 12)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if(
_tmpEF <= (void*)3?1:*((int*)_tmpEF)!= 13)goto _LL7E;_LL7D: goto _LL7F;_LL7E: if((int)
_tmpEF != 1)goto _LL80;_LL7F: goto _LL81;_LL80: if(_tmpEF <= (void*)3?1:*((int*)_tmpEF)
!= 6)goto _LL82;_LL81: goto _LL83;_LL82: if(_tmpEF <= (void*)3?1:*((int*)_tmpEF)!= 8)
goto _LL84;_LL83: goto _LL85;_LL84: if(_tmpEF <= (void*)3?1:*((int*)_tmpEF)!= 17)goto
_LL86;_LL85: goto _LL87;_LL86: if(_tmpEF <= (void*)3?1:*((int*)_tmpEF)!= 14)goto
_LL88;_LL87: return 1;_LL88: if(_tmpEF <= (void*)3?1:*((int*)_tmpEF)!= 7)goto _LL8A;
_tmpF0=((struct Cyc_Absyn_ArrayType_struct*)_tmpEF)->f1;_tmpF1=(void*)_tmpF0.elt_type;
_LL89: return Cyc_Toc_atomic_typ(_tmpF1);_LL8A: if(_tmpEF <= (void*)3?1:*((int*)
_tmpEF)!= 10)goto _LL8C;_tmpF2=((struct Cyc_Absyn_AggrType_struct*)_tmpEF)->f1;
_tmpF3=(void*)_tmpF2.aggr_info;_LL8B:{void*_tmpFA=_tmpF3;_LL9B: if(*((int*)_tmpFA)
!= 0)goto _LL9D;_LL9C: return 0;_LL9D:;_LL9E: goto _LL9A;_LL9A:;}{struct Cyc_Absyn_Aggrdecl*
_tmpFB=Cyc_Absyn_get_known_aggrdecl(_tmpF3);if(_tmpFB->impl == 0)return 0;{struct
Cyc_List_List*_tmpFC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFB->impl))->fields;
for(0;_tmpFC != 0;_tmpFC=_tmpFC->tl){if(!Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)
_tmpFC->hd)->type))return 0;}}return 1;}_LL8C: if(_tmpEF <= (void*)3?1:*((int*)
_tmpEF)!= 11)goto _LL8E;_tmpF4=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpEF)->f2;
_LL8D: for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){if(!Cyc_Toc_atomic_typ((void*)((struct
Cyc_Absyn_Aggrfield*)_tmpF4->hd)->type))return 0;}return 1;_LL8E: if(_tmpEF <= (void*)
3?1:*((int*)_tmpEF)!= 3)goto _LL90;_tmpF5=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpEF)->f1;_tmpF6=(void*)_tmpF5.field_info;if(*((int*)_tmpF6)!= 1)goto _LL90;
_tmpF7=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpF6)->f1;_tmpF8=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpF6)->f2;_LL8F: _tmpF9=_tmpF8->typs;goto _LL91;_LL90: if(_tmpEF <= (void*)3?1:*((
int*)_tmpEF)!= 9)goto _LL92;_tmpF9=((struct Cyc_Absyn_TupleType_struct*)_tmpEF)->f1;
_LL91: for(0;_tmpF9 != 0;_tmpF9=_tmpF9->tl){if(!Cyc_Toc_atomic_typ((*((struct
_tuple4*)((struct _tuple4*)_tmpF9->hd))).f2))return 0;}return 1;_LL92: if(_tmpEF <= (
void*)3?1:*((int*)_tmpEF)!= 2)goto _LL94;_LL93: goto _LL95;_LL94: if(_tmpEF <= (void*)
3?1:*((int*)_tmpEF)!= 4)goto _LL96;_LL95: goto _LL97;_LL96: if(_tmpEF <= (void*)3?1:*((
int*)_tmpEF)!= 15)goto _LL98;_LL97: return 0;_LL98:;_LL99:({struct Cyc_Std_String_pa_struct
_tmpFF;_tmpFF.tag=0;_tmpFF.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmpFD[1]={& _tmpFF};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpFE="atomic_typ:  bad type %s";_tag_arr(_tmpFE,sizeof(char),
_get_zero_arr_size(_tmpFE,25));}),_tag_arr(_tmpFD,sizeof(void*),1));}});_LL73:;}
static int Cyc_Toc_is_void_star(void*t){void*_tmp100=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmp101;void*_tmp102;_LLA0: if(_tmp100 <= (void*)3?1:*((int*)
_tmp100)!= 4)goto _LLA2;_tmp101=((struct Cyc_Absyn_PointerType_struct*)_tmp100)->f1;
_tmp102=(void*)_tmp101.elt_typ;_LLA1: {void*_tmp103=Cyc_Tcutil_compress(_tmp102);
_LLA5: if((int)_tmp103 != 0)goto _LLA7;_LLA6: return 1;_LLA7:;_LLA8: return 0;_LLA4:;}
_LLA2:;_LLA3: return 0;_LL9F:;}static int Cyc_Toc_is_poly_field(void*t,struct
_tagged_arr*f){void*_tmp104=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp105;void*_tmp106;struct Cyc_List_List*_tmp107;_LLAA: if(_tmp104 <= (void*)3?1:*((
int*)_tmp104)!= 10)goto _LLAC;_tmp105=((struct Cyc_Absyn_AggrType_struct*)_tmp104)->f1;
_tmp106=(void*)_tmp105.aggr_info;_LLAB: {struct Cyc_Absyn_Aggrdecl*_tmp108=Cyc_Absyn_get_known_aggrdecl(
_tmp106);if(_tmp108->impl == 0)({void*_tmp109[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp10A="is_poly_field: type missing fields";
_tag_arr(_tmp10A,sizeof(char),_get_zero_arr_size(_tmp10A,35));}),_tag_arr(
_tmp109,sizeof(void*),0));});_tmp107=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp108->impl))->fields;goto _LLAD;}_LLAC: if(_tmp104 <= (void*)3?1:*((int*)_tmp104)
!= 11)goto _LLAE;_tmp107=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp104)->f2;
_LLAD: {struct Cyc_Absyn_Aggrfield*_tmp10B=Cyc_Absyn_lookup_field(_tmp107,f);if(
_tmp10B == 0)({struct Cyc_Std_String_pa_struct _tmp10E;_tmp10E.tag=0;_tmp10E.f1=(
struct _tagged_arr)*((struct _tagged_arr*)f);{void*_tmp10C[1]={& _tmp10E};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp10D="is_poly_field: bad field %s";
_tag_arr(_tmp10D,sizeof(char),_get_zero_arr_size(_tmp10D,28));}),_tag_arr(
_tmp10C,sizeof(void*),1));}});return Cyc_Toc_is_void_star((void*)_tmp10B->type);}
_LLAE:;_LLAF:({struct Cyc_Std_String_pa_struct _tmp111;_tmp111.tag=0;_tmp111.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp10F[1]={& _tmp111};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp110="is_poly_field: bad type %s";
_tag_arr(_tmp110,sizeof(char),_get_zero_arr_size(_tmp110,27));}),_tag_arr(
_tmp10F,sizeof(void*),1));}});_LLA9:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*
e){void*_tmp112=(void*)e->r;struct Cyc_Absyn_Exp*_tmp113;struct _tagged_arr*
_tmp114;struct Cyc_Absyn_Exp*_tmp115;struct _tagged_arr*_tmp116;_LLB1: if(*((int*)
_tmp112)!= 21)goto _LLB3;_tmp113=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp112)->f1;
_tmp114=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp112)->f2;_LLB2: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp113->topt))->v,_tmp114);_LLB3: if(*((
int*)_tmp112)!= 22)goto _LLB5;_tmp115=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp112)->f1;_tmp116=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp112)->f2;_LLB4: {
void*_tmp117=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp115->topt))->v);struct Cyc_Absyn_PtrInfo _tmp118;void*_tmp119;_LLB8: if(_tmp117
<= (void*)3?1:*((int*)_tmp117)!= 4)goto _LLBA;_tmp118=((struct Cyc_Absyn_PointerType_struct*)
_tmp117)->f1;_tmp119=(void*)_tmp118.elt_typ;_LLB9: return Cyc_Toc_is_poly_field(
_tmp119,_tmp116);_LLBA:;_LLBB:({struct Cyc_Std_String_pa_struct _tmp11C;_tmp11C.tag=
0;_tmp11C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp115->topt))->v);{void*_tmp11A[1]={& _tmp11C};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp11B="is_poly_project:  bad type %s";
_tag_arr(_tmp11B,sizeof(char),_get_zero_arr_size(_tmp11B,30));}),_tag_arr(
_tmp11A,sizeof(void*),1));}});_LLB7:;}_LLB5:;_LLB6: return 0;_LLB0:;}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp11D=_cycalloc(sizeof(*_tmp11D));
_tmp11D->hd=s;_tmp11D->tl=0;_tmp11D;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({
struct Cyc_List_List*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->hd=s;_tmp11E->tl=
0;_tmp11E;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*
s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*
_tmp11F[2];_tmp11F[1]=s;_tmp11F[0]=rgn;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp11F,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp120[2];_tmp120[1]=n;
_tmp120[0]=s;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp120,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp121[2];_tmp121[1]=n;_tmp121[0]=s;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp121,
sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp122[3];_tmp122[2]=n;_tmp122[1]=
s;_tmp122[0]=rgn;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp122,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({
struct Cyc_List_List*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->hd=e;_tmp123->tl=
0;_tmp123;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_tagged_arr(void*
t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*_tmp124=(
void*)e->r;void*_tmp125;_LLBD: if(*((int*)_tmp124)!= 0)goto _LLBF;_tmp125=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp124)->f1;if(_tmp125 <= (void*)1?1:*((int*)
_tmp125)!= 5)goto _LLBF;_LLBE: is_string=1;goto _LLBC;_LLBF:;_LLC0: goto _LLBC;_LLBC:;}{
struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*
x=Cyc_Toc_temp_var();void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)sz,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127[0]=({
struct Cyc_Absyn_Var_d_struct _tmp128;_tmp128.tag=0;_tmp128.f1=vd;_tmp128;});
_tmp127;}),0);_tmp126->tl=Cyc_Toc_result_decls;_tmp126;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
e,0);xplussz=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,
0),0);}{struct Cyc_Absyn_Exp*urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct
_tuple6*_tmp129[3];_tmp129[2]=({struct _tuple6*_tmp12C=_cycalloc(sizeof(*_tmp12C));
_tmp12C->f1=0;_tmp12C->f2=xplussz;_tmp12C;});_tmp129[1]=({struct _tuple6*_tmp12B=
_cycalloc(sizeof(*_tmp12B));_tmp12B->f1=0;_tmp12B->f2=xexp;_tmp12B;});_tmp129[0]=({
struct _tuple6*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->f1=0;_tmp12A->f2=xexp;
_tmp12A;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp129,sizeof(struct _tuple6*),3));}),0);return urm_exp;}}}struct Cyc_Toc_Env{
struct Cyc_Core_Opt*break_lab;struct Cyc_Core_Opt*continue_lab;struct Cyc_Core_Opt*
fallthru_info;struct Cyc_Dict_Dict*varmap;int toplevel;};static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(){return({struct Cyc_Toc_Env*_tmp12D=_cycalloc(sizeof(*_tmp12D));
_tmp12D->break_lab=0;_tmp12D->continue_lab=0;_tmp12D->fallthru_info=0;_tmp12D->varmap=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp);_tmp12D->toplevel=1;_tmp12D;});}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp12E=_cycalloc(
sizeof(*_tmp12E));_tmp12E[0]=*((struct Cyc_Toc_Env*)e);_tmp12E;});ans->toplevel=0;
return ans;}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct Cyc_Toc_Env*e,struct
_tuple1*x,struct Cyc_Absyn_Exp*y){{void*_tmp12F=(*((struct _tuple1*)x)).f1;_LLC2:
if(_tmp12F <= (void*)1?1:*((int*)_tmp12F)!= 0)goto _LLC4;_LLC3:({struct Cyc_Std_String_pa_struct
_tmp132;_tmp132.tag=0;_tmp132.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(x);{
void*_tmp130[1]={& _tmp132};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp131="Toc::add_varmap on Rel_n: %s\n";_tag_arr(
_tmp131,sizeof(char),_get_zero_arr_size(_tmp131,30));}),_tag_arr(_tmp130,sizeof(
void*),1));}});_LLC4:;_LLC5: goto _LLC1;_LLC1:;}{struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133[0]=*((struct Cyc_Toc_Env*)e);_tmp133;});
ans->varmap=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,
struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(ans->varmap,x,y);return ans;}}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134[0]=*((struct Cyc_Toc_Env*)
e);_tmp134;});ans->break_lab=0;ans->continue_lab=0;return ans;}struct _tuple8{
struct _tagged_arr*f1;struct Cyc_List_List*f2;struct Cyc_Dict_Dict*f3;};static
struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct Cyc_Toc_Env*e,struct
_tagged_arr*break_l,struct _tagged_arr*fallthru_l,struct Cyc_List_List*
fallthru_binders,struct Cyc_Toc_Env*next_case_env){struct Cyc_List_List*_tmp135=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){_tmp135=({
struct Cyc_List_List*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp136->tl=_tmp135;_tmp136;});}_tmp135=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp135);{struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A[0]=*((struct Cyc_Toc_Env*)
e);_tmp13A;});ans->break_lab=({struct Cyc_Core_Opt*_tmp137=_cycalloc(sizeof(*
_tmp137));_tmp137->v=break_l;_tmp137;});ans->fallthru_info=({struct Cyc_Core_Opt*
_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->v=({struct _tuple8*_tmp139=_cycalloc(
sizeof(*_tmp139));_tmp139->f1=fallthru_l;_tmp139->f2=_tmp135;_tmp139->f3=
next_case_env->varmap;_tmp139;});_tmp138;});return ans;}}static struct Cyc_Toc_Env*
Cyc_Toc_last_switchclause_env(struct Cyc_Toc_Env*e,struct _tagged_arr*break_l){
struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp13C=_cycalloc(sizeof(*_tmp13C));
_tmp13C[0]=*((struct Cyc_Toc_Env*)e);_tmp13C;});ans->break_lab=({struct Cyc_Core_Opt*
_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->v=break_l;_tmp13B;});ans->fallthru_info=
0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct Cyc_Toc_Env*
e,struct _tagged_arr*next_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp13F=
_cycalloc(sizeof(*_tmp13F));_tmp13F[0]=*((struct Cyc_Toc_Env*)e);_tmp13F;});ans->break_lab=
0;ans->fallthru_info=({struct Cyc_Core_Opt*_tmp13D=_cycalloc(sizeof(*_tmp13D));
_tmp13D->v=({struct _tuple8*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->f1=
next_l;_tmp13E->f2=0;_tmp13E->f3=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp13E;});_tmp13D;});
return ans;}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static
int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp140=(void*)e->annot;
_LLC7: if(*((void**)_tmp140)!= Cyc_CfFlowInfo_UnknownZ)goto _LLC9;_LLC8: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLC9: if(*((void**)_tmp140)
!= Cyc_CfFlowInfo_NotZero)goto _LLCB;_LLCA: return 0;_LLCB: if(*((void**)_tmp140)!= 
Cyc_CfFlowInfo_IsZero)goto _LLCD;_LLCC:({void*_tmp141[0]={};Cyc_Tcutil_terr(e->loc,({
const char*_tmp142="dereference of NULL pointer";_tag_arr(_tmp142,sizeof(char),
_get_zero_arr_size(_tmp142,28));}),_tag_arr(_tmp141,sizeof(void*),0));});return 0;
_LLCD: if(_tmp140 != Cyc_Absyn_EmptyAnnot)goto _LLCF;_LLCE: return 0;_LLCF:;_LLD0:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp143=_cycalloc(sizeof(*
_tmp143));_tmp143[0]=({struct Cyc_Core_Impossible_struct _tmp144;_tmp144.tag=Cyc_Core_Impossible;
_tmp144.f1=({const char*_tmp145="need_null_check";_tag_arr(_tmp145,sizeof(char),
_get_zero_arr_size(_tmp145,16));});_tmp144;});_tmp143;}));_LLC6:;}static struct
Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*_tmp146=(void*)e->annot;
struct Cyc_List_List*_tmp147;struct Cyc_List_List*_tmp148;_LLD2: if(*((void**)
_tmp146)!= Cyc_CfFlowInfo_UnknownZ)goto _LLD4;_tmp147=((struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmp146)->f1;_LLD3: return _tmp147;_LLD4: if(*((void**)_tmp146)!= Cyc_CfFlowInfo_NotZero)
goto _LLD6;_tmp148=((struct Cyc_CfFlowInfo_NotZero_struct*)_tmp146)->f1;_LLD5:
return _tmp148;_LLD6: if(*((void**)_tmp146)!= Cyc_CfFlowInfo_IsZero)goto _LLD8;
_LLD7:({void*_tmp149[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp14A="dereference of NULL pointer";
_tag_arr(_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,28));}),_tag_arr(
_tmp149,sizeof(void*),0));});return 0;_LLD8: if(_tmp146 != Cyc_Absyn_EmptyAnnot)
goto _LLDA;_LLD9: return 0;_LLDA:;_LLDB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B[0]=({struct Cyc_Core_Impossible_struct
_tmp14C;_tmp14C.tag=Cyc_Core_Impossible;_tmp14C.f1=({const char*_tmp14D="get_relns";
_tag_arr(_tmp14D,sizeof(char),_get_zero_arr_size(_tmp14D,10));});_tmp14C;});
_tmp14B;}));_LLD1:;}static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp14E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp14F;struct Cyc_Absyn_Conref*
_tmp150;struct Cyc_Absyn_Conref*_tmp151;struct Cyc_Absyn_ArrayInfo _tmp152;struct
Cyc_Absyn_Exp*_tmp153;_LLDD: if(_tmp14E <= (void*)3?1:*((int*)_tmp14E)!= 4)goto
_LLDF;_tmp14F=((struct Cyc_Absyn_PointerType_struct*)_tmp14E)->f1;_tmp150=_tmp14F.bounds;
_tmp151=_tmp14F.zero_term;_LLDE: {void*_tmp154=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp150);struct Cyc_Absyn_Exp*_tmp155;_LLE4: if((int)_tmp154 != 0)goto _LLE6;_LLE5:
return 0;_LLE6: if(_tmp154 <= (void*)1?1:*((int*)_tmp154)!= 0)goto _LLE3;_tmp155=((
struct Cyc_Absyn_Upper_b_struct*)_tmp154)->f1;_LLE7: {unsigned int _tmp157;int
_tmp158;struct _tuple5 _tmp156=Cyc_Evexp_eval_const_uint_exp(_tmp155);_tmp157=
_tmp156.f1;_tmp158=_tmp156.f2;return _tmp158?i <= _tmp157: 0;}_LLE3:;}_LLDF: if(
_tmp14E <= (void*)3?1:*((int*)_tmp14E)!= 7)goto _LLE1;_tmp152=((struct Cyc_Absyn_ArrayType_struct*)
_tmp14E)->f1;_tmp153=_tmp152.num_elts;_LLE0: if(_tmp153 == 0)return 0;{unsigned int
_tmp15A;int _tmp15B;struct _tuple5 _tmp159=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp153));_tmp15A=_tmp159.f1;_tmp15B=_tmp159.f2;return _tmp15B?i <= 
_tmp15A: 0;}_LLE1:;_LLE2: return 0;_LLDC:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp15C=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp15C->vd != y)continue;{void*_tmp15D=(void*)_tmp15C->rop;struct
Cyc_Absyn_Vardecl*_tmp15E;struct Cyc_Absyn_Vardecl*_tmp15F;_LLE9: if(*((int*)
_tmp15D)!= 2)goto _LLEB;_tmp15E=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp15D)->f1;
_LLEA: _tmp15F=_tmp15E;goto _LLEC;_LLEB: if(*((int*)_tmp15D)!= 4)goto _LLED;_tmp15F=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp15D)->f1;_LLEC: if(_tmp15F == v)return
1;else{goto _LLE8;}_LLED:;_LLEE: continue;_LLE8:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*
_tmp160=(void*)e->r;void*_tmp161;struct Cyc_Absyn_Vardecl*_tmp162;void*_tmp163;
struct Cyc_Absyn_Vardecl*_tmp164;void*_tmp165;struct Cyc_Absyn_Vardecl*_tmp166;
void*_tmp167;struct Cyc_Absyn_Vardecl*_tmp168;void*_tmp169;struct Cyc_List_List*
_tmp16A;struct Cyc_List_List _tmp16B;struct Cyc_Absyn_Exp*_tmp16C;_LLF0: if(*((int*)
_tmp160)!= 1)goto _LLF2;_tmp161=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp160)->f2;
if(_tmp161 <= (void*)1?1:*((int*)_tmp161)!= 4)goto _LLF2;_tmp162=((struct Cyc_Absyn_Pat_b_struct*)
_tmp161)->f1;_LLF1: _tmp164=_tmp162;goto _LLF3;_LLF2: if(*((int*)_tmp160)!= 1)goto
_LLF4;_tmp163=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp160)->f2;if(_tmp163 <= (
void*)1?1:*((int*)_tmp163)!= 3)goto _LLF4;_tmp164=((struct Cyc_Absyn_Local_b_struct*)
_tmp163)->f1;_LLF3: _tmp166=_tmp164;goto _LLF5;_LLF4: if(*((int*)_tmp160)!= 1)goto
_LLF6;_tmp165=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp160)->f2;if(_tmp165 <= (
void*)1?1:*((int*)_tmp165)!= 0)goto _LLF6;_tmp166=((struct Cyc_Absyn_Global_b_struct*)
_tmp165)->f1;_LLF5: _tmp168=_tmp166;goto _LLF7;_LLF6: if(*((int*)_tmp160)!= 1)goto
_LLF8;_tmp167=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp160)->f2;if(_tmp167 <= (
void*)1?1:*((int*)_tmp167)!= 2)goto _LLF8;_tmp168=((struct Cyc_Absyn_Param_b_struct*)
_tmp167)->f1;_LLF7: if(_tmp168->escapes)return 0;if(Cyc_Toc_check_leq_size_var(
relns,v,_tmp168))return 1;goto _LLEF;_LLF8: if(*((int*)_tmp160)!= 3)goto _LLFA;
_tmp169=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp160)->f1;if((int)_tmp169 != 
19)goto _LLFA;_tmp16A=((struct Cyc_Absyn_Primop_e_struct*)_tmp160)->f2;if(_tmp16A
== 0)goto _LLFA;_tmp16B=*_tmp16A;_tmp16C=(struct Cyc_Absyn_Exp*)_tmp16B.hd;_LLF9:{
void*_tmp16D=(void*)_tmp16C->r;void*_tmp16E;struct Cyc_Absyn_Vardecl*_tmp16F;void*
_tmp170;struct Cyc_Absyn_Vardecl*_tmp171;void*_tmp172;struct Cyc_Absyn_Vardecl*
_tmp173;void*_tmp174;struct Cyc_Absyn_Vardecl*_tmp175;_LLFD: if(*((int*)_tmp16D)!= 
1)goto _LLFF;_tmp16E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp16D)->f2;if(
_tmp16E <= (void*)1?1:*((int*)_tmp16E)!= 4)goto _LLFF;_tmp16F=((struct Cyc_Absyn_Pat_b_struct*)
_tmp16E)->f1;_LLFE: _tmp171=_tmp16F;goto _LL100;_LLFF: if(*((int*)_tmp16D)!= 1)goto
_LL101;_tmp170=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp16D)->f2;if(_tmp170 <= (
void*)1?1:*((int*)_tmp170)!= 3)goto _LL101;_tmp171=((struct Cyc_Absyn_Local_b_struct*)
_tmp170)->f1;_LL100: _tmp173=_tmp171;goto _LL102;_LL101: if(*((int*)_tmp16D)!= 1)
goto _LL103;_tmp172=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp16D)->f2;if(
_tmp172 <= (void*)1?1:*((int*)_tmp172)!= 0)goto _LL103;_tmp173=((struct Cyc_Absyn_Global_b_struct*)
_tmp172)->f1;_LL102: _tmp175=_tmp173;goto _LL104;_LL103: if(*((int*)_tmp16D)!= 1)
goto _LL105;_tmp174=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp16D)->f2;if(
_tmp174 <= (void*)1?1:*((int*)_tmp174)!= 2)goto _LL105;_tmp175=((struct Cyc_Absyn_Param_b_struct*)
_tmp174)->f1;_LL104: return _tmp175 == v;_LL105:;_LL106: goto _LLFC;_LLFC:;}goto _LLEF;
_LLFA:;_LLFB: goto _LLEF;_LLEF:;}return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*_tmp176=(void*)a->r;
void*_tmp177;struct Cyc_Absyn_Vardecl*_tmp178;void*_tmp179;struct Cyc_Absyn_Vardecl*
_tmp17A;void*_tmp17B;struct Cyc_Absyn_Vardecl*_tmp17C;void*_tmp17D;struct Cyc_Absyn_Vardecl*
_tmp17E;_LL108: if(*((int*)_tmp176)!= 1)goto _LL10A;_tmp177=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp176)->f2;if(_tmp177 <= (void*)1?1:*((int*)_tmp177)!= 4)goto _LL10A;_tmp178=((
struct Cyc_Absyn_Pat_b_struct*)_tmp177)->f1;_LL109: _tmp17A=_tmp178;goto _LL10B;
_LL10A: if(*((int*)_tmp176)!= 1)goto _LL10C;_tmp179=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp176)->f2;if(_tmp179 <= (void*)1?1:*((int*)_tmp179)!= 3)goto _LL10C;_tmp17A=((
struct Cyc_Absyn_Local_b_struct*)_tmp179)->f1;_LL10B: _tmp17C=_tmp17A;goto _LL10D;
_LL10C: if(*((int*)_tmp176)!= 1)goto _LL10E;_tmp17B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp176)->f2;if(_tmp17B <= (void*)1?1:*((int*)_tmp17B)!= 0)goto _LL10E;_tmp17C=((
struct Cyc_Absyn_Global_b_struct*)_tmp17B)->f1;_LL10D: _tmp17E=_tmp17C;goto _LL10F;
_LL10E: if(*((int*)_tmp176)!= 1)goto _LL110;_tmp17D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp176)->f2;if(_tmp17D <= (void*)1?1:*((int*)_tmp17D)!= 2)goto _LL110;_tmp17E=((
struct Cyc_Absyn_Param_b_struct*)_tmp17D)->f1;_LL10F: if(_tmp17E->escapes)return 0;
inner_loop: {void*_tmp17F=(void*)i->r;void*_tmp180;struct Cyc_Absyn_Exp*_tmp181;
void*_tmp182;void*_tmp183;int _tmp184;void*_tmp185;void*_tmp186;int _tmp187;void*
_tmp188;void*_tmp189;int _tmp18A;void*_tmp18B;struct Cyc_List_List*_tmp18C;struct
Cyc_List_List _tmp18D;struct Cyc_Absyn_Exp*_tmp18E;struct Cyc_List_List*_tmp18F;
struct Cyc_List_List _tmp190;struct Cyc_Absyn_Exp*_tmp191;void*_tmp192;struct Cyc_Absyn_Vardecl*
_tmp193;void*_tmp194;struct Cyc_Absyn_Vardecl*_tmp195;void*_tmp196;struct Cyc_Absyn_Vardecl*
_tmp197;void*_tmp198;struct Cyc_Absyn_Vardecl*_tmp199;_LL113: if(*((int*)_tmp17F)
!= 13)goto _LL115;_tmp180=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp17F)->f1;
_tmp181=((struct Cyc_Absyn_Cast_e_struct*)_tmp17F)->f2;_LL114: i=_tmp181;goto
inner_loop;_LL115: if(*((int*)_tmp17F)!= 0)goto _LL117;_tmp182=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp17F)->f1;if(_tmp182 <= (void*)1?1:*((int*)_tmp182)!= 2)goto _LL117;_tmp183=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp182)->f1;if((int)_tmp183 != 2)goto
_LL117;_tmp184=((struct Cyc_Absyn_Int_c_struct*)_tmp182)->f2;_LL116: _tmp187=
_tmp184;goto _LL118;_LL117: if(*((int*)_tmp17F)!= 0)goto _LL119;_tmp185=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp17F)->f1;if(_tmp185 <= (void*)1?1:*((int*)
_tmp185)!= 2)goto _LL119;_tmp186=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp185)->f1;
if((int)_tmp186 != 0)goto _LL119;_tmp187=((struct Cyc_Absyn_Int_c_struct*)_tmp185)->f2;
_LL118: return _tmp187 >= 0?Cyc_Toc_check_const_array((unsigned int)(_tmp187 + 1),(
void*)_tmp17E->type): 0;_LL119: if(*((int*)_tmp17F)!= 0)goto _LL11B;_tmp188=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp17F)->f1;if(_tmp188 <= (void*)1?1:*((int*)
_tmp188)!= 2)goto _LL11B;_tmp189=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp188)->f1;
if((int)_tmp189 != 1)goto _LL11B;_tmp18A=((struct Cyc_Absyn_Int_c_struct*)_tmp188)->f2;
_LL11A: return Cyc_Toc_check_const_array((unsigned int)(_tmp18A + 1),(void*)_tmp17E->type);
_LL11B: if(*((int*)_tmp17F)!= 3)goto _LL11D;_tmp18B=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp17F)->f1;if((int)_tmp18B != 4)goto _LL11D;_tmp18C=((struct Cyc_Absyn_Primop_e_struct*)
_tmp17F)->f2;if(_tmp18C == 0)goto _LL11D;_tmp18D=*_tmp18C;_tmp18E=(struct Cyc_Absyn_Exp*)
_tmp18D.hd;_tmp18F=_tmp18D.tl;if(_tmp18F == 0)goto _LL11D;_tmp190=*_tmp18F;_tmp191=(
struct Cyc_Absyn_Exp*)_tmp190.hd;_LL11C: return Cyc_Toc_check_leq_size(relns,
_tmp17E,_tmp191);_LL11D: if(*((int*)_tmp17F)!= 1)goto _LL11F;_tmp192=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp17F)->f2;if(_tmp192 <= (void*)1?1:*((int*)
_tmp192)!= 4)goto _LL11F;_tmp193=((struct Cyc_Absyn_Pat_b_struct*)_tmp192)->f1;
_LL11E: _tmp195=_tmp193;goto _LL120;_LL11F: if(*((int*)_tmp17F)!= 1)goto _LL121;
_tmp194=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp17F)->f2;if(_tmp194 <= (void*)
1?1:*((int*)_tmp194)!= 3)goto _LL121;_tmp195=((struct Cyc_Absyn_Local_b_struct*)
_tmp194)->f1;_LL120: _tmp197=_tmp195;goto _LL122;_LL121: if(*((int*)_tmp17F)!= 1)
goto _LL123;_tmp196=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp17F)->f2;if(
_tmp196 <= (void*)1?1:*((int*)_tmp196)!= 0)goto _LL123;_tmp197=((struct Cyc_Absyn_Global_b_struct*)
_tmp196)->f1;_LL122: _tmp199=_tmp197;goto _LL124;_LL123: if(*((int*)_tmp17F)!= 1)
goto _LL125;_tmp198=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp17F)->f2;if(
_tmp198 <= (void*)1?1:*((int*)_tmp198)!= 2)goto _LL125;_tmp199=((struct Cyc_Absyn_Param_b_struct*)
_tmp198)->f1;_LL124: if(_tmp199->escapes)return 0;{struct Cyc_List_List*_tmp19A=
relns;for(0;_tmp19A != 0;_tmp19A=_tmp19A->tl){struct Cyc_CfFlowInfo_Reln*_tmp19B=(
struct Cyc_CfFlowInfo_Reln*)_tmp19A->hd;if(_tmp19B->vd == _tmp199){void*_tmp19C=(
void*)_tmp19B->rop;struct Cyc_Absyn_Vardecl*_tmp19D;struct Cyc_Absyn_Vardecl*
_tmp19E;unsigned int _tmp19F;_LL128: if(*((int*)_tmp19C)!= 2)goto _LL12A;_tmp19D=((
struct Cyc_CfFlowInfo_LessSize_struct*)_tmp19C)->f1;_LL129: if(_tmp17E == _tmp19D)
return 1;else{goto _LL127;}_LL12A: if(*((int*)_tmp19C)!= 1)goto _LL12C;_tmp19E=((
struct Cyc_CfFlowInfo_LessVar_struct*)_tmp19C)->f1;_LL12B:{struct Cyc_List_List*
_tmp1A0=relns;for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){struct Cyc_CfFlowInfo_Reln*
_tmp1A1=(struct Cyc_CfFlowInfo_Reln*)_tmp1A0->hd;if(_tmp1A1->vd == _tmp19E){void*
_tmp1A2=(void*)_tmp1A1->rop;struct Cyc_Absyn_Vardecl*_tmp1A3;struct Cyc_Absyn_Vardecl*
_tmp1A4;unsigned int _tmp1A5;struct Cyc_Absyn_Vardecl*_tmp1A6;_LL131: if(*((int*)
_tmp1A2)!= 4)goto _LL133;_tmp1A3=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp1A2)->f1;_LL132: _tmp1A4=_tmp1A3;goto _LL134;_LL133: if(*((int*)_tmp1A2)!= 2)
goto _LL135;_tmp1A4=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1A2)->f1;_LL134:
if(_tmp17E == _tmp1A4)return 1;else{goto _LL130;}_LL135: if(*((int*)_tmp1A2)!= 0)
goto _LL137;_tmp1A5=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp1A2)->f1;_LL136:
return Cyc_Toc_check_const_array(_tmp1A5,(void*)_tmp17E->type);_LL137: if(*((int*)
_tmp1A2)!= 1)goto _LL139;_tmp1A6=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1A2)->f1;
_LL138: if(Cyc_Toc_check_leq_size_var(relns,_tmp17E,_tmp1A6))return 1;else{goto
_LL130;}_LL139:;_LL13A: goto _LL130;_LL130:;}}}goto _LL127;_LL12C: if(*((int*)
_tmp19C)!= 3)goto _LL12E;_tmp19F=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp19C)->f1;
_LL12D: return Cyc_Toc_check_const_array(_tmp19F,(void*)_tmp17E->type);_LL12E:;
_LL12F: goto _LL127;_LL127:;}}}goto _LL112;_LL125:;_LL126: goto _LL112;_LL112:;}goto
_LL107;_LL110:;_LL111: goto _LL107;_LL107:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp1A7[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1A8="Missing type in primop ";
_tag_arr(_tmp1A8,sizeof(char),_get_zero_arr_size(_tmp1A8,24));}),_tag_arr(
_tmp1A7,sizeof(void*),0));});return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp1A9[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1AA="Missing type in primop ";
_tag_arr(_tmp1AA,sizeof(char),_get_zero_arr_size(_tmp1AA,24));}),_tag_arr(
_tmp1A9,sizeof(void*),0));});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static struct _tuple4*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct
_tuple4*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->f1=Cyc_Toc_mt_tq;_tmp1AB->f2=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp1AB;});}
static struct _tuple6*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){Cyc_Toc_exp_to_c(nv,e);return({struct _tuple6*_tmp1AC=_cycalloc(sizeof(*
_tmp1AC));_tmp1AC->f1=0;_tmp1AC->f2=e;_tmp1AC;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp1AD=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp1AD):
Cyc_Toc_malloc_ptr(_tmp1AD));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_check_null(rgnopt);Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(
r,_tmp1AD);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp1AE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp1AE != 0;_tmp1AE=_tmp1AE->tl){struct _tuple6 _tmp1B0;struct Cyc_List_List*
_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B2;struct _tuple6*_tmp1AF=(struct _tuple6*)
_tmp1AE->hd;_tmp1B0=*_tmp1AF;_tmp1B1=_tmp1B0.f1;_tmp1B2=_tmp1B0.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp1B1 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp1B1->tl != 0)({void*_tmp1B3[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp1B4="multiple designators in array";
_tag_arr(_tmp1B4,sizeof(char),_get_zero_arr_size(_tmp1B4,30));}),_tag_arr(
_tmp1B3,sizeof(void*),0));});{void*_tmp1B5=(void*)_tmp1B1->hd;void*_tmp1B6=
_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B7;_LL13C: if(*((int*)_tmp1B6)!= 0)goto _LL13E;
_tmp1B7=((struct Cyc_Absyn_ArrayElement_struct*)_tmp1B6)->f1;_LL13D: Cyc_Toc_exp_to_c(
nv,_tmp1B7);e_index=_tmp1B7;goto _LL13B;_LL13E: if(*((int*)_tmp1B6)!= 1)goto _LL13B;
_LL13F:({void*_tmp1B8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_unimp)(({const char*_tmp1B9="field name designators in array";_tag_arr(
_tmp1B9,sizeof(char),_get_zero_arr_size(_tmp1B9,32));}),_tag_arr(_tmp1B8,sizeof(
void*),0));});_LL13B:;}}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,
e_index,0);void*_tmp1BA=(void*)_tmp1B2->r;struct Cyc_List_List*_tmp1BB;struct Cyc_Absyn_Vardecl*
_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BD;struct Cyc_Absyn_Exp*_tmp1BE;int _tmp1BF;void*
_tmp1C0;struct Cyc_List_List*_tmp1C1;_LL141: if(*((int*)_tmp1BA)!= 26)goto _LL143;
_tmp1BB=((struct Cyc_Absyn_Array_e_struct*)_tmp1BA)->f1;_LL142: s=Cyc_Toc_init_array(
nv,lval,_tmp1BB,s);goto _LL140;_LL143: if(*((int*)_tmp1BA)!= 27)goto _LL145;_tmp1BC=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1BA)->f1;_tmp1BD=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1BA)->f2;_tmp1BE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1BA)->f3;
_tmp1BF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1BA)->f4;_LL144: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp1BC,_tmp1BD,_tmp1BE,_tmp1BF,s,0);goto _LL140;_LL145: if(*((int*)
_tmp1BA)!= 29)goto _LL147;_tmp1C0=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1BA)->f1;_tmp1C1=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1BA)->f2;_LL146:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp1C0,_tmp1C1,s);goto _LL140;_LL147:;_LL148:
Cyc_Toc_exp_to_c(nv,_tmp1B2);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp1B2,0),s,0);goto _LL140;_LL140:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp1C2=vd->name;
void*_tmp1C3=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
nv,_tmp1C2,Cyc_Absyn_varb_exp(_tmp1C2,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC[0]=({struct Cyc_Absyn_Local_b_struct
_tmp1CD;_tmp1CD.tag=3;_tmp1CD.f1=vd;_tmp1CD;});_tmp1CC;}),0));struct _tuple1*max=
Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(
_tmp1C2,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp(_tmp1C2,0),Cyc_Absyn_var_exp(max,0),0);struct Cyc_Absyn_Exp*ec=
Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp1C2,0),0);struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp1C2,0),0);struct Cyc_Absyn_Stmt*
body;{void*_tmp1C4=(void*)e2->r;struct Cyc_List_List*_tmp1C5;struct Cyc_Absyn_Vardecl*
_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C8;int _tmp1C9;void*
_tmp1CA;struct Cyc_List_List*_tmp1CB;_LL14A: if(*((int*)_tmp1C4)!= 26)goto _LL14C;
_tmp1C5=((struct Cyc_Absyn_Array_e_struct*)_tmp1C4)->f1;_LL14B: body=Cyc_Toc_init_array(
nv2,lval,_tmp1C5,Cyc_Toc_skip_stmt_dl());goto _LL149;_LL14C: if(*((int*)_tmp1C4)!= 
27)goto _LL14E;_tmp1C6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1C4)->f1;
_tmp1C7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1C4)->f2;_tmp1C8=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1C4)->f3;_tmp1C9=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1C4)->f4;_LL14D: body=Cyc_Toc_init_comprehension(nv2,lval,_tmp1C6,_tmp1C7,
_tmp1C8,_tmp1C9,Cyc_Toc_skip_stmt_dl(),0);goto _LL149;_LL14E: if(*((int*)_tmp1C4)
!= 29)goto _LL150;_tmp1CA=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1C4)->f1;
_tmp1CB=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1C4)->f2;_LL14F: body=Cyc_Toc_init_anon_struct(
nv,lval,_tmp1CA,_tmp1CB,Cyc_Toc_skip_stmt_dl());goto _LL149;_LL150:;_LL151: Cyc_Toc_exp_to_c(
nv2,e2);body=Cyc_Absyn_assign_stmt(lval,e2,0);goto _LL149;_LL149:;}{struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(max,0),0),Cyc_Absyn_cast_exp(
_tmp1C3,Cyc_Absyn_uint_exp(0,0),0),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(
ex,0),0);}return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp1C2,Cyc_Absyn_uint_typ,0,s2,0),
0),s,0);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp1CE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp1CE != 0;_tmp1CE=_tmp1CE->tl){struct _tuple6 _tmp1D0;
struct Cyc_List_List*_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D2;struct _tuple6*_tmp1CF=(
struct _tuple6*)_tmp1CE->hd;_tmp1D0=*_tmp1CF;_tmp1D1=_tmp1D0.f1;_tmp1D2=_tmp1D0.f2;
if(_tmp1D1 == 0)({void*_tmp1D3[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1D4="empty designator list";
_tag_arr(_tmp1D4,sizeof(char),_get_zero_arr_size(_tmp1D4,22));}),_tag_arr(
_tmp1D3,sizeof(void*),0));});if(_tmp1D1->tl != 0)({void*_tmp1D5[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1D6="too many designators in anonymous struct";
_tag_arr(_tmp1D6,sizeof(char),_get_zero_arr_size(_tmp1D6,41));}),_tag_arr(
_tmp1D5,sizeof(void*),0));});{void*_tmp1D7=(void*)_tmp1D1->hd;struct _tagged_arr*
_tmp1D8;_LL153: if(*((int*)_tmp1D7)!= 1)goto _LL155;_tmp1D8=((struct Cyc_Absyn_FieldName_struct*)
_tmp1D7)->f1;_LL154: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,
_tmp1D8,0);{void*_tmp1D9=(void*)_tmp1D2->r;struct Cyc_List_List*_tmp1DA;struct Cyc_Absyn_Vardecl*
_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DD;int _tmp1DE;void*
_tmp1DF;struct Cyc_List_List*_tmp1E0;_LL158: if(*((int*)_tmp1D9)!= 26)goto _LL15A;
_tmp1DA=((struct Cyc_Absyn_Array_e_struct*)_tmp1D9)->f1;_LL159: s=Cyc_Toc_init_array(
nv,lval,_tmp1DA,s);goto _LL157;_LL15A: if(*((int*)_tmp1D9)!= 27)goto _LL15C;_tmp1DB=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D9)->f1;_tmp1DC=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1D9)->f2;_tmp1DD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D9)->f3;
_tmp1DE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D9)->f4;_LL15B: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp1DB,_tmp1DC,_tmp1DD,_tmp1DE,s,0);goto _LL157;_LL15C: if(*((int*)
_tmp1D9)!= 29)goto _LL15E;_tmp1DF=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1D9)->f1;_tmp1E0=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1D9)->f2;_LL15D:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp1DF,_tmp1E0,s);goto _LL157;_LL15E:;_LL15F:
Cyc_Toc_exp_to_c(nv,_tmp1D2);if(Cyc_Toc_is_poly_field(struct_type,_tmp1D8))
_tmp1D2=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),_tmp1D2,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp1D2,0),0),s,0);goto _LL157;
_LL157:;}goto _LL152;}_LL155:;_LL156:({void*_tmp1E1[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1E2="array designator in struct";
_tag_arr(_tmp1E2,sizeof(char),_get_zero_arr_size(_tmp1E2,27));}),_tag_arr(
_tmp1E1,sizeof(void*),0));});_LL152:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(
struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
es){struct _RegionHandle _tmp1E3=_new_region("r");struct _RegionHandle*r=& _tmp1E3;
_push_region(r);{struct Cyc_List_List*_tmp1E4=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);struct _tagged_arr*_tmp1E5=Cyc_Toc_add_tuple_type(
_tmp1E4);struct _tuple1*_tmp1E6=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1E7=
Cyc_Absyn_var_exp(_tmp1E6,0);struct Cyc_Absyn_Stmt*_tmp1E8=Cyc_Absyn_exp_stmt(
_tmp1E7,0);struct Cyc_Absyn_Exp*(*_tmp1E9)(struct Cyc_Absyn_Exp*,struct _tagged_arr*,
struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp1EA=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp1EA);for(0;_tmp1EA != 0;(_tmp1EA=_tmp1EA->tl,
-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp1EA->hd;struct Cyc_Absyn_Exp*
lval=_tmp1E9(_tmp1E7,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic?Cyc_Toc_atomic_typ((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v): 0;{void*_tmp1EB=(void*)e->r;
struct Cyc_List_List*_tmp1EC;struct Cyc_Absyn_Vardecl*_tmp1ED;struct Cyc_Absyn_Exp*
_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;int _tmp1F0;_LL161: if(*((int*)_tmp1EB)!= 26)
goto _LL163;_tmp1EC=((struct Cyc_Absyn_Array_e_struct*)_tmp1EB)->f1;_LL162: _tmp1E8=
Cyc_Toc_init_array(nv,lval,_tmp1EC,_tmp1E8);goto _LL160;_LL163: if(*((int*)_tmp1EB)
!= 27)goto _LL165;_tmp1ED=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1EB)->f1;
_tmp1EE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1EB)->f2;_tmp1EF=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1EB)->f3;_tmp1F0=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1EB)->f4;_LL164: _tmp1E8=Cyc_Toc_init_comprehension(nv,lval,_tmp1ED,_tmp1EE,
_tmp1EF,_tmp1F0,_tmp1E8,0);goto _LL160;_LL165:;_LL166: Cyc_Toc_exp_to_c(nv,e);
_tmp1E8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp1E9(
_tmp1E7,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp1E8,0);goto _LL160;_LL160:;}}}{
struct Cyc_Absyn_Exp*_tmp1F1=Cyc_Toc_make_struct(nv,_tmp1E6,Cyc_Absyn_strct(
_tmp1E5),_tmp1E8,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp1F1;}};
_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct
Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp1F2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp1F3=Cyc_Absyn_var_exp(_tmp1F2,0);struct Cyc_Absyn_Stmt*
_tmp1F4=Cyc_Absyn_exp_stmt(_tmp1F3,0);struct Cyc_Absyn_Exp*(*_tmp1F5)(struct Cyc_Absyn_Exp*,
struct _tagged_arr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp1F6=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp1F7;void*
_tmp1F8;_LL168: if(_tmp1F6 <= (void*)3?1:*((int*)_tmp1F6)!= 10)goto _LL16A;_tmp1F7=((
struct Cyc_Absyn_AggrType_struct*)_tmp1F6)->f1;_tmp1F8=(void*)_tmp1F7.aggr_info;
_LL169: ad=Cyc_Absyn_get_known_aggrdecl(_tmp1F8);goto _LL167;_LL16A:;_LL16B:({void*
_tmp1F9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1FA="init_struct: bad struct type";_tag_arr(_tmp1FA,sizeof(char),
_get_zero_arr_size(_tmp1FA,29));}),_tag_arr(_tmp1F9,sizeof(void*),0));});_LL167:;}{
struct _RegionHandle _tmp1FB=_new_region("r");struct _RegionHandle*r=& _tmp1FB;
_push_region(r);{struct Cyc_List_List*_tmp1FC=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp1FC != 0;
_tmp1FC=_tmp1FC->tl){struct _tuple6 _tmp1FE;struct Cyc_List_List*_tmp1FF;struct Cyc_Absyn_Exp*
_tmp200;struct _tuple6*_tmp1FD=(struct _tuple6*)_tmp1FC->hd;_tmp1FE=*_tmp1FD;
_tmp1FF=_tmp1FE.f1;_tmp200=_tmp1FE.f2;is_atomic=is_atomic?Cyc_Toc_atomic_typ((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp200->topt))->v): 0;if(_tmp1FF == 0)({
void*_tmp201[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp202="empty designator list";_tag_arr(_tmp202,sizeof(char),
_get_zero_arr_size(_tmp202,22));}),_tag_arr(_tmp201,sizeof(void*),0));});if(
_tmp1FF->tl != 0){struct _tuple1*_tmp203=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp204=Cyc_Absyn_var_exp(_tmp203,0);for(0;_tmp1FF != 0;_tmp1FF=_tmp1FF->tl){void*
_tmp205=(void*)_tmp1FF->hd;struct _tagged_arr*_tmp206;_LL16D: if(*((int*)_tmp205)
!= 1)goto _LL16F;_tmp206=((struct Cyc_Absyn_FieldName_struct*)_tmp205)->f1;_LL16E:
if(Cyc_Toc_is_poly_field(struct_type,_tmp206))_tmp204=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp204,0);_tmp1F4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp1F5(_tmp1F3,_tmp206,0),_tmp204,0),0),_tmp1F4,0);goto _LL16C;_LL16F:;_LL170:({
void*_tmp207[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp208="array designator in struct";_tag_arr(_tmp208,sizeof(char),
_get_zero_arr_size(_tmp208,27));}),_tag_arr(_tmp207,sizeof(void*),0));});_LL16C:;}
Cyc_Toc_exp_to_c(nv,_tmp200);_tmp1F4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp204,_tmp200,0),0),_tmp1F4,0);}else{void*_tmp209=(void*)_tmp1FF->hd;struct
_tagged_arr*_tmp20A;_LL172: if(*((int*)_tmp209)!= 1)goto _LL174;_tmp20A=((struct
Cyc_Absyn_FieldName_struct*)_tmp209)->f1;_LL173: {struct Cyc_Absyn_Exp*lval=
_tmp1F5(_tmp1F3,_tmp20A,0);{void*_tmp20B=(void*)_tmp200->r;struct Cyc_List_List*
_tmp20C;struct Cyc_Absyn_Vardecl*_tmp20D;struct Cyc_Absyn_Exp*_tmp20E;struct Cyc_Absyn_Exp*
_tmp20F;int _tmp210;void*_tmp211;struct Cyc_List_List*_tmp212;_LL177: if(*((int*)
_tmp20B)!= 26)goto _LL179;_tmp20C=((struct Cyc_Absyn_Array_e_struct*)_tmp20B)->f1;
_LL178: _tmp1F4=Cyc_Toc_init_array(nv,lval,_tmp20C,_tmp1F4);goto _LL176;_LL179: if(*((
int*)_tmp20B)!= 27)goto _LL17B;_tmp20D=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp20B)->f1;_tmp20E=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp20B)->f2;
_tmp20F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp20B)->f3;_tmp210=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp20B)->f4;_LL17A: _tmp1F4=Cyc_Toc_init_comprehension(
nv,lval,_tmp20D,_tmp20E,_tmp20F,_tmp210,_tmp1F4,0);goto _LL176;_LL17B: if(*((int*)
_tmp20B)!= 29)goto _LL17D;_tmp211=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp20B)->f1;_tmp212=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp20B)->f2;_LL17C:
_tmp1F4=Cyc_Toc_init_anon_struct(nv,lval,_tmp211,_tmp212,_tmp1F4);goto _LL176;
_LL17D:;_LL17E: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp200->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp200);if(Cyc_Toc_is_poly_field(
struct_type,_tmp20A)?!was_ptr_type: 0)_tmp200=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp200,0);if(has_exists)_tmp200=Cyc_Absyn_cast_exp((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(Cyc_Absyn_lookup_decl_field(ad,_tmp20A)))->type,_tmp200,0);_tmp1F4=
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp200,0),0),
_tmp1F4,0);goto _LL176;}_LL176:;}goto _LL171;}_LL174:;_LL175:({void*_tmp213[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmp214="array designator in struct";_tag_arr(_tmp214,sizeof(char),
_get_zero_arr_size(_tmp214,27));}),_tag_arr(_tmp213,sizeof(void*),0));});_LL171:;}}}{
struct Cyc_Absyn_Exp*_tmp215=Cyc_Toc_make_struct(nv,_tmp1F2,Cyc_Absyn_strctq(tdn),
_tmp1F4,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp215;};_pop_region(r);}}
struct _tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple9*pr){return Cyc_Absyn_assignop_exp(
el,(*((struct _tuple9*)pr)).f1,(*((struct _tuple9*)pr)).f2,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*
incr){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216[0]=({struct Cyc_Absyn_Increment_e_struct
_tmp217;_tmp217.tag=5;_tmp217.f1=e1;_tmp217.f2=(void*)incr;_tmp217;});_tmp216;}),
0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp218=(void*)e1->r;struct Cyc_Absyn_Stmt*
_tmp219;void*_tmp21A;struct Cyc_Absyn_Exp*_tmp21B;struct Cyc_Absyn_Exp*_tmp21C;
struct _tagged_arr*_tmp21D;_LL180: if(*((int*)_tmp218)!= 35)goto _LL182;_tmp219=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp218)->f1;_LL181: Cyc_Toc_lvalue_assign_stmt(
_tmp219,fs,f,f_env);goto _LL17F;_LL182: if(*((int*)_tmp218)!= 13)goto _LL184;
_tmp21A=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp218)->f1;_tmp21B=((struct Cyc_Absyn_Cast_e_struct*)
_tmp218)->f2;_LL183: Cyc_Toc_lvalue_assign(_tmp21B,fs,f,f_env);goto _LL17F;_LL184:
if(*((int*)_tmp218)!= 21)goto _LL186;_tmp21C=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp218)->f1;_tmp21D=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp218)->f2;_LL185:(
void*)(e1->r=(void*)((void*)_tmp21C->r));Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp21E=_cycalloc(sizeof(*_tmp21E));_tmp21E->hd=_tmp21D;
_tmp21E->tl=fs;_tmp21E;}),f,f_env);goto _LL17F;_LL186:;_LL187: {struct Cyc_Absyn_Exp*
e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(
e1_copy,(struct _tagged_arr*)fs->hd,e1_copy->loc);}(void*)(e1->r=(void*)((void*)(
f(e1_copy,f_env))->r));goto _LL17F;}_LL17F:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp21F=(void*)s->r;struct Cyc_Absyn_Exp*_tmp220;struct
Cyc_Absyn_Decl*_tmp221;struct Cyc_Absyn_Stmt*_tmp222;struct Cyc_Absyn_Stmt*_tmp223;
_LL189: if(_tmp21F <= (void*)1?1:*((int*)_tmp21F)!= 0)goto _LL18B;_tmp220=((struct
Cyc_Absyn_Exp_s_struct*)_tmp21F)->f1;_LL18A: Cyc_Toc_lvalue_assign(_tmp220,fs,f,
f_env);goto _LL188;_LL18B: if(_tmp21F <= (void*)1?1:*((int*)_tmp21F)!= 12)goto
_LL18D;_tmp221=((struct Cyc_Absyn_Decl_s_struct*)_tmp21F)->f1;_tmp222=((struct Cyc_Absyn_Decl_s_struct*)
_tmp21F)->f2;_LL18C: Cyc_Toc_lvalue_assign_stmt(_tmp222,fs,f,f_env);goto _LL188;
_LL18D: if(_tmp21F <= (void*)1?1:*((int*)_tmp21F)!= 1)goto _LL18F;_tmp223=((struct
Cyc_Absyn_Seq_s_struct*)_tmp21F)->f2;_LL18E: Cyc_Toc_lvalue_assign_stmt(_tmp223,
fs,f,f_env);goto _LL188;_LL18F:;_LL190:({struct Cyc_Std_String_pa_struct _tmp226;
_tmp226.tag=0;_tmp226.f1=(struct _tagged_arr)Cyc_Absynpp_stmt2string(s);{void*
_tmp224[1]={& _tmp226};Cyc_Toc_toc_impos(({const char*_tmp225="lvalue_assign_stmt: %s";
_tag_arr(_tmp225,sizeof(char),_get_zero_arr_size(_tmp225,23));}),_tag_arr(
_tmp224,sizeof(void*),1));}});_LL188:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(
struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({
struct Cyc_List_List*_tmp227=_region_malloc(r2,sizeof(*_tmp227));_tmp227->hd=(
void*)f((void*)x->hd,env);_tmp227->tl=0;_tmp227;});prev=result;for(x=x->tl;x != 0;
x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp228=_region_malloc(r2,sizeof(*_tmp228));_tmp228->hd=(void*)f((void*)x->hd,
env);_tmp228->tl=0;_tmp228;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,
x);}static struct _tuple6*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({struct
_tuple6*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229->f1=0;_tmp229->f2=e;_tmp229;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp22A=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp22B;_LL192: if(_tmp22A <= (void*)3?1:*((int*)_tmp22A)
!= 4)goto _LL194;_tmp22B=((struct Cyc_Absyn_PointerType_struct*)_tmp22A)->f1;
_LL193: return _tmp22B;_LL194:;_LL195:({void*_tmp22C[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp22D="get_ptr_typ: not a pointer!";
_tag_arr(_tmp22D,sizeof(char),_get_zero_arr_size(_tmp22D,28));}),_tag_arr(
_tmp22C,sizeof(void*),0));});_LL191:;}static int Cyc_Toc_is_zero_ptr_type(void*t,
void**ptr_type,int*is_fat,void**elt_type){void*_tmp22E=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp22F;void*_tmp230;struct Cyc_Absyn_Conref*_tmp231;
struct Cyc_Absyn_Conref*_tmp232;struct Cyc_Absyn_ArrayInfo _tmp233;void*_tmp234;
struct Cyc_Absyn_Tqual _tmp235;struct Cyc_Absyn_Exp*_tmp236;struct Cyc_Absyn_Conref*
_tmp237;_LL197: if(_tmp22E <= (void*)3?1:*((int*)_tmp22E)!= 4)goto _LL199;_tmp22F=((
struct Cyc_Absyn_PointerType_struct*)_tmp22E)->f1;_tmp230=(void*)_tmp22F.elt_typ;
_tmp231=_tmp22F.bounds;_tmp232=_tmp22F.zero_term;_LL198: if(((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp232)){*((void**)ptr_type)=t;*((
void**)elt_type)=_tmp230;{void*_tmp238=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp231);_LL19E: if((int)_tmp238 != 0)goto _LL1A0;_LL19F:*((int*)is_fat)=1;goto
_LL19D;_LL1A0:;_LL1A1:*((int*)is_fat)=0;goto _LL19D;_LL19D:;}return 1;}else{return
0;}_LL199: if(_tmp22E <= (void*)3?1:*((int*)_tmp22E)!= 7)goto _LL19B;_tmp233=((
struct Cyc_Absyn_ArrayType_struct*)_tmp22E)->f1;_tmp234=(void*)_tmp233.elt_type;
_tmp235=_tmp233.tq;_tmp236=_tmp233.num_elts;_tmp237=_tmp233.zero_term;_LL19A: if(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp237)){*((void**)
elt_type)=_tmp234;*((int*)is_fat)=0;*((void**)ptr_type)=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp239=_cycalloc(sizeof(*_tmp239));_tmp239[0]=({struct Cyc_Absyn_PointerType_struct
_tmp23A;_tmp23A.tag=4;_tmp23A.f1=({struct Cyc_Absyn_PtrInfo _tmp23B;_tmp23B.elt_typ=(
void*)_tmp234;_tmp23B.rgn_typ=(void*)((void*)2);_tmp23B.nullable=((struct Cyc_Absyn_Conref*(*)(
int x))Cyc_Absyn_new_conref)(0);_tmp23B.tq=_tmp235;_tmp23B.bounds=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp23C=_cycalloc(sizeof(*_tmp23C));
_tmp23C[0]=({struct Cyc_Absyn_Upper_b_struct _tmp23D;_tmp23D.tag=0;_tmp23D.f1=(
struct Cyc_Absyn_Exp*)_check_null(_tmp236);_tmp23D;});_tmp23C;}));_tmp23B.zero_term=
_tmp237;_tmp23B;});_tmp23A;});_tmp239;});return 1;}else{return 0;}_LL19B:;_LL19C:
return 0;_LL196:;}static int Cyc_Toc_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_fat,void**elt_type){void*_tmp23E=(void*)e1->r;struct Cyc_Absyn_Exp*
_tmp23F;struct Cyc_Absyn_Exp*_tmp240;struct Cyc_Absyn_Exp*_tmp241;struct Cyc_Absyn_Exp*
_tmp242;struct Cyc_Absyn_Exp*_tmp243;struct Cyc_Absyn_Exp*_tmp244;_LL1A3: if(*((int*)
_tmp23E)!= 13)goto _LL1A5;_LL1A4:({struct Cyc_Std_String_pa_struct _tmp247;_tmp247.tag=
0;_tmp247.f1=(struct _tagged_arr)Cyc_Absynpp_exp2string(e1);{void*_tmp245[1]={&
_tmp247};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp246="we have a cast in a lhs:  %s";_tag_arr(_tmp246,sizeof(char),
_get_zero_arr_size(_tmp246,29));}),_tag_arr(_tmp245,sizeof(void*),1));}});_LL1A5:
if(*((int*)_tmp23E)!= 20)goto _LL1A7;_tmp23F=((struct Cyc_Absyn_Deref_e_struct*)
_tmp23E)->f1;_LL1A6: _tmp240=_tmp23F;goto _LL1A8;_LL1A7: if(*((int*)_tmp23E)!= 23)
goto _LL1A9;_tmp240=((struct Cyc_Absyn_Subscript_e_struct*)_tmp23E)->f1;_LL1A8:
return Cyc_Toc_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp240->topt))->v,
ptr_type,is_fat,elt_type);_LL1A9: if(*((int*)_tmp23E)!= 22)goto _LL1AB;_tmp241=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp23E)->f1;_LL1AA: _tmp242=_tmp241;goto
_LL1AC;_LL1AB: if(*((int*)_tmp23E)!= 21)goto _LL1AD;_tmp242=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp23E)->f1;_LL1AC: if(Cyc_Toc_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp242->topt))->v,ptr_type,is_fat,elt_type))({struct Cyc_Std_String_pa_struct
_tmp24A;_tmp24A.tag=0;_tmp24A.f1=(struct _tagged_arr)Cyc_Absynpp_exp2string(e1);{
void*_tmp248[1]={& _tmp24A};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp249="found zero pointer aggregate member assignment: %s";
_tag_arr(_tmp249,sizeof(char),_get_zero_arr_size(_tmp249,51));}),_tag_arr(
_tmp248,sizeof(void*),1));}});return 0;_LL1AD: if(*((int*)_tmp23E)!= 12)goto _LL1AF;
_tmp243=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp23E)->f1;_LL1AE: _tmp244=
_tmp243;goto _LL1B0;_LL1AF: if(*((int*)_tmp23E)!= 11)goto _LL1B1;_tmp244=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp23E)->f1;_LL1B0: if(Cyc_Toc_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp244->topt))->v,ptr_type,is_fat,
elt_type))({struct Cyc_Std_String_pa_struct _tmp24D;_tmp24D.tag=0;_tmp24D.f1=(
struct _tagged_arr)Cyc_Absynpp_exp2string(e1);{void*_tmp24B[1]={& _tmp24D};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp24C="found zero pointer instantiate/noinstantiate: %s";
_tag_arr(_tmp24C,sizeof(char),_get_zero_arr_size(_tmp24C,49));}),_tag_arr(
_tmp24B,sizeof(void*),1));}});return 0;_LL1B1: if(*((int*)_tmp23E)!= 1)goto _LL1B3;
_LL1B2: return 0;_LL1B3:;_LL1B4:({struct Cyc_Std_String_pa_struct _tmp250;_tmp250.tag=
0;_tmp250.f1=(struct _tagged_arr)Cyc_Absynpp_exp2string(e1);{void*_tmp24E[1]={&
_tmp250};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp24F="found bad lhs in is_zero_ptr_deref: %s";_tag_arr(_tmp24F,
sizeof(char),_get_zero_arr_size(_tmp24F,39));}),_tag_arr(_tmp24E,sizeof(void*),1));}});
_LL1A2:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp251=Cyc_Tcutil_compress(t);void*_tmp252;void*_tmp253;void*_tmp254;
void*_tmp255;void*_tmp256;void*_tmp257;void*_tmp258;void*_tmp259;_LL1B6: if(
_tmp251 <= (void*)3?1:*((int*)_tmp251)!= 4)goto _LL1B8;_LL1B7: res=Cyc_Absyn_null_exp(
0);goto _LL1B5;_LL1B8: if(_tmp251 <= (void*)3?1:*((int*)_tmp251)!= 5)goto _LL1BA;
_tmp252=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp251)->f1;_tmp253=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp251)->f2;if((int)_tmp253 != 0)goto _LL1BA;
_LL1B9: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Char_c_struct*_tmp25A=
_cycalloc(sizeof(*_tmp25A));_tmp25A[0]=({struct Cyc_Absyn_Char_c_struct _tmp25B;
_tmp25B.tag=0;_tmp25B.f1=(void*)_tmp252;_tmp25B.f2='\000';_tmp25B;});_tmp25A;}),
0);goto _LL1B5;_LL1BA: if(_tmp251 <= (void*)3?1:*((int*)_tmp251)!= 5)goto _LL1BC;
_tmp254=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp251)->f1;_tmp255=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp251)->f2;if((int)_tmp255 != 1)goto _LL1BC;
_LL1BB: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Short_c_struct*_tmp25C=
_cycalloc(sizeof(*_tmp25C));_tmp25C[0]=({struct Cyc_Absyn_Short_c_struct _tmp25D;
_tmp25D.tag=1;_tmp25D.f1=(void*)_tmp254;_tmp25D.f2=0;_tmp25D;});_tmp25C;}),0);
goto _LL1B5;_LL1BC: if(_tmp251 <= (void*)3?1:*((int*)_tmp251)!= 12)goto _LL1BE;
_LL1BD: goto _LL1BF;_LL1BE: if(_tmp251 <= (void*)3?1:*((int*)_tmp251)!= 13)goto
_LL1C0;_LL1BF: _tmp256=(void*)1;goto _LL1C1;_LL1C0: if(_tmp251 <= (void*)3?1:*((int*)
_tmp251)!= 5)goto _LL1C2;_tmp256=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp251)->f1;
_tmp257=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp251)->f2;if((int)_tmp257 != 
2)goto _LL1C2;_LL1C1: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*
_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E[0]=({struct Cyc_Absyn_Int_c_struct
_tmp25F;_tmp25F.tag=2;_tmp25F.f1=(void*)_tmp256;_tmp25F.f2=0;_tmp25F;});_tmp25E;}),
0);goto _LL1B5;_LL1C2: if(_tmp251 <= (void*)3?1:*((int*)_tmp251)!= 5)goto _LL1C4;
_tmp258=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp251)->f1;_tmp259=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp251)->f2;if((int)_tmp259 != 3)goto _LL1C4;
_LL1C3: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_LongLong_c_struct*
_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260[0]=({struct Cyc_Absyn_LongLong_c_struct
_tmp261;_tmp261.tag=3;_tmp261.f1=(void*)_tmp258;_tmp261.f2=(long long)0;_tmp261;});
_tmp260;}),0);goto _LL1B5;_LL1C4: if((int)_tmp251 != 1)goto _LL1C6;_LL1C5: goto _LL1C7;
_LL1C6: if(_tmp251 <= (void*)3?1:*((int*)_tmp251)!= 6)goto _LL1C8;_LL1C7: res=Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Float_c_struct*_tmp262=_cycalloc(sizeof(*_tmp262));
_tmp262[0]=({struct Cyc_Absyn_Float_c_struct _tmp263;_tmp263.tag=4;_tmp263.f1=({
const char*_tmp264="0.0";_tag_arr(_tmp264,sizeof(char),_get_zero_arr_size(_tmp264,
4));});_tmp263;});_tmp262;}),0);goto _LL1B5;_LL1C8:;_LL1C9:({struct Cyc_Std_String_pa_struct
_tmp267;_tmp267.tag=0;_tmp267.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp265[1]={& _tmp267};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp266="found non-zero type %s in generate_zero";
_tag_arr(_tmp266,sizeof(char),_get_zero_arr_size(_tmp266,40));}),_tag_arr(
_tmp265,sizeof(void*),1));}});_LL1B5:;}res->topt=({struct Cyc_Core_Opt*_tmp268=
_cycalloc(sizeof(*_tmp268));_tmp268->v=(void*)t;_tmp268;});return res;}static void
Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,
int is_fat,void*elt_type){void*_tmp269=Cyc_Absyn_tagged_typ(elt_type,(void*)2,Cyc_Toc_mt_tq,
Cyc_Absyn_true_conref);void*_tmp26A=Cyc_Toc_typ_to_c(elt_type);void*_tmp26B=Cyc_Toc_typ_to_c(
_tmp269);void*_tmp26C=Cyc_Absyn_cstar_typ(_tmp26A,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmp26D=({struct Cyc_Core_Opt*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7->v=(
void*)_tmp26C;_tmp2A7;});struct Cyc_Absyn_Exp*xinit;{void*_tmp26E=(void*)e1->r;
struct Cyc_Absyn_Exp*_tmp26F;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_Absyn_Exp*
_tmp271;_LL1CB: if(*((int*)_tmp26E)!= 20)goto _LL1CD;_tmp26F=((struct Cyc_Absyn_Deref_e_struct*)
_tmp26E)->f1;_LL1CC: if(!is_fat)_tmp26F=Cyc_Absyn_cast_exp(_tmp269,_tmp26F,0);
_tmp26F->topt=({struct Cyc_Core_Opt*_tmp272=_cycalloc(sizeof(*_tmp272));_tmp272->v=(
void*)_tmp269;_tmp272;});Cyc_Toc_exp_to_c(nv,_tmp26F);xinit=_tmp26F;goto _LL1CA;
_LL1CD: if(*((int*)_tmp26E)!= 23)goto _LL1CF;_tmp270=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp26E)->f1;_tmp271=((struct Cyc_Absyn_Subscript_e_struct*)_tmp26E)->f2;_LL1CE:
if(!is_fat)_tmp270=Cyc_Absyn_cast_exp(_tmp269,_tmp270,0);_tmp270->topt=({struct
Cyc_Core_Opt*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->v=(void*)_tmp269;
_tmp273;});Cyc_Toc_exp_to_c(nv,_tmp270);Cyc_Toc_exp_to_c(nv,_tmp271);xinit=Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp274[3];_tmp274[2]=_tmp271;
_tmp274[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp274[0]=
_tmp270;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp274,sizeof(struct Cyc_Absyn_Exp*),3));}),0);goto _LL1CA;_LL1CF:;_LL1D0:({void*
_tmp275[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp276="found bad lhs for zero-terminated pointer assignment";
_tag_arr(_tmp276,sizeof(char),_get_zero_arr_size(_tmp276,53));}),_tag_arr(
_tmp275,sizeof(void*),0));});_LL1CA:;}{struct _tuple1*_tmp277=Cyc_Toc_temp_var();
nv=Cyc_Toc_add_varmap(nv,_tmp277,Cyc_Absyn_var_exp(_tmp277,0));{struct Cyc_Absyn_Vardecl*
_tmp278=({struct Cyc_Absyn_Vardecl*_tmp2A6=_cycalloc(sizeof(*_tmp2A6));_tmp2A6->sc=(
void*)((void*)2);_tmp2A6->name=_tmp277;_tmp2A6->tq=Cyc_Toc_mt_tq;_tmp2A6->type=(
void*)_tmp26B;_tmp2A6->initializer=(struct Cyc_Absyn_Exp*)xinit;_tmp2A6->rgn=0;
_tmp2A6->attributes=0;_tmp2A6->escapes=0;_tmp2A6;});struct Cyc_Absyn_Local_b_struct*
_tmp279=({struct Cyc_Absyn_Local_b_struct*_tmp2A4=_cycalloc(sizeof(*_tmp2A4));
_tmp2A4[0]=({struct Cyc_Absyn_Local_b_struct _tmp2A5;_tmp2A5.tag=3;_tmp2A5.f1=
_tmp278;_tmp2A5;});_tmp2A4;});struct Cyc_Absyn_Exp*_tmp27A=Cyc_Absyn_varb_exp(
_tmp277,(void*)_tmp279,0);_tmp27A->topt=({struct Cyc_Core_Opt*_tmp27B=_cycalloc(
sizeof(*_tmp27B));_tmp27B->v=(void*)_tmp269;_tmp27B;});{struct Cyc_Absyn_Exp*
_tmp27C=Cyc_Absyn_deref_exp(_tmp27A,0);_tmp27C->topt=({struct Cyc_Core_Opt*
_tmp27D=_cycalloc(sizeof(*_tmp27D));_tmp27D->v=(void*)elt_type;_tmp27D;});Cyc_Toc_exp_to_c(
nv,_tmp27C);{struct _tuple1*_tmp27E=Cyc_Toc_temp_var();nv=Cyc_Toc_add_varmap(nv,
_tmp27E,Cyc_Absyn_var_exp(_tmp27E,0));{struct Cyc_Absyn_Vardecl*_tmp27F=({struct
Cyc_Absyn_Vardecl*_tmp2A3=_cycalloc(sizeof(*_tmp2A3));_tmp2A3->sc=(void*)((void*)
2);_tmp2A3->name=_tmp27E;_tmp2A3->tq=Cyc_Toc_mt_tq;_tmp2A3->type=(void*)_tmp26A;
_tmp2A3->initializer=(struct Cyc_Absyn_Exp*)_tmp27C;_tmp2A3->rgn=0;_tmp2A3->attributes=
0;_tmp2A3->escapes=0;_tmp2A3;});struct Cyc_Absyn_Local_b_struct*_tmp280=({struct
Cyc_Absyn_Local_b_struct*_tmp2A1=_cycalloc(sizeof(*_tmp2A1));_tmp2A1[0]=({struct
Cyc_Absyn_Local_b_struct _tmp2A2;_tmp2A2.tag=3;_tmp2A2.f1=_tmp27F;_tmp2A2;});
_tmp2A1;});struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*
_tmp281=Cyc_Absyn_varb_exp(_tmp27E,(void*)_tmp280,0);_tmp281->topt=_tmp27C->topt;
z_init=Cyc_Absyn_prim2_exp((void*)popt->v,_tmp281,e2,0);z_init->topt=_tmp281->topt;}
Cyc_Toc_exp_to_c(nv,z_init);{struct _tuple1*_tmp282=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp283=({struct Cyc_Absyn_Vardecl*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0->sc=(
void*)((void*)2);_tmp2A0->name=_tmp282;_tmp2A0->tq=Cyc_Toc_mt_tq;_tmp2A0->type=(
void*)_tmp26A;_tmp2A0->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp2A0->rgn=0;
_tmp2A0->attributes=0;_tmp2A0->escapes=0;_tmp2A0;});struct Cyc_Absyn_Local_b_struct*
_tmp284=({struct Cyc_Absyn_Local_b_struct*_tmp29E=_cycalloc(sizeof(*_tmp29E));
_tmp29E[0]=({struct Cyc_Absyn_Local_b_struct _tmp29F;_tmp29F.tag=3;_tmp29F.f1=
_tmp283;_tmp29F;});_tmp29E;});nv=Cyc_Toc_add_varmap(nv,_tmp282,Cyc_Absyn_var_exp(
_tmp282,0));{struct Cyc_Absyn_Exp*_tmp285=Cyc_Absyn_varb_exp(_tmp27E,(void*)
_tmp280,0);_tmp285->topt=_tmp27C->topt;{struct Cyc_Absyn_Exp*_tmp286=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp287=Cyc_Absyn_prim2_exp((void*)5,_tmp285,
_tmp286,0);_tmp287->topt=({struct Cyc_Core_Opt*_tmp288=_cycalloc(sizeof(*_tmp288));
_tmp288->v=(void*)Cyc_Absyn_sint_typ;_tmp288;});Cyc_Toc_exp_to_c(nv,_tmp287);{
struct Cyc_Absyn_Exp*_tmp289=Cyc_Absyn_varb_exp(_tmp282,(void*)_tmp284,0);_tmp289->topt=
_tmp27C->topt;{struct Cyc_Absyn_Exp*_tmp28A=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp28B=Cyc_Absyn_prim2_exp((void*)6,_tmp289,_tmp28A,0);
_tmp28B->topt=({struct Cyc_Core_Opt*_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->v=(
void*)Cyc_Absyn_sint_typ;_tmp28C;});Cyc_Toc_exp_to_c(nv,_tmp28B);{struct Cyc_Absyn_Exp*
_tmp28D=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp29D[2];_tmp29D[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp29D[0]=Cyc_Absyn_varb_exp(_tmp277,(void*)_tmp279,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp29D,sizeof(struct Cyc_Absyn_Exp*),
2));}),0),Cyc_Absyn_uint_exp(1,0),0);struct Cyc_Absyn_Exp*_tmp28E=Cyc_Absyn_and_exp(
_tmp28D,Cyc_Absyn_and_exp(_tmp287,_tmp28B,0),0);struct Cyc_Absyn_Stmt*_tmp28F=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*
_tmp290=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_varb_exp(_tmp277,(void*)_tmp279,0),
Cyc_Toc_curr_sp,0);_tmp290=Cyc_Absyn_cast_exp(_tmp26C,_tmp290,0);{struct Cyc_Absyn_Exp*
_tmp291=Cyc_Absyn_deref_exp(_tmp290,0);struct Cyc_Absyn_Exp*_tmp292=Cyc_Absyn_assign_exp(
_tmp291,Cyc_Absyn_var_exp(_tmp282,0),0);struct Cyc_Absyn_Stmt*_tmp293=Cyc_Absyn_exp_stmt(
_tmp292,0);_tmp293=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp28E,_tmp28F,
Cyc_Absyn_skip_stmt(0),0),_tmp293,0);_tmp293=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*
_tmp294=_cycalloc(sizeof(*_tmp294));_tmp294->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp295=_cycalloc(sizeof(*_tmp295));_tmp295[0]=({struct Cyc_Absyn_Var_d_struct
_tmp296;_tmp296.tag=0;_tmp296.f1=_tmp283;_tmp296;});_tmp295;}));_tmp294->loc=0;
_tmp294;}),_tmp293,0);_tmp293=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp297=
_cycalloc(sizeof(*_tmp297));_tmp297->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298[0]=({struct Cyc_Absyn_Var_d_struct
_tmp299;_tmp299.tag=0;_tmp299.f1=_tmp27F;_tmp299;});_tmp298;}));_tmp297->loc=0;
_tmp297;}),_tmp293,0);_tmp293=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp29A=
_cycalloc(sizeof(*_tmp29A));_tmp29A->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B[0]=({struct Cyc_Absyn_Var_d_struct
_tmp29C;_tmp29C.tag=0;_tmp29C.f1=_tmp278;_tmp29C;});_tmp29B;}));_tmp29A->loc=0;
_tmp29A;}),_tmp293,0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp293,0))->r));}}}}}}}}}}}}}
struct _tuple10{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple11{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp2A8=(void*)e->r;if(e->topt
== 0)({struct Cyc_Std_String_pa_struct _tmp2AB;_tmp2AB.tag=0;_tmp2AB.f1=(struct
_tagged_arr)Cyc_Absynpp_exp2string(e);{void*_tmp2A9[1]={& _tmp2AB};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp2AA="exp_to_c: no type for %s";
_tag_arr(_tmp2AA,sizeof(char),_get_zero_arr_size(_tmp2AA,25));}),_tag_arr(
_tmp2A9,sizeof(void*),1));}});{void*old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;void*_tmp2AC=_tmp2A8;void*_tmp2AD;struct _tuple1*_tmp2AE;
void*_tmp2AF;struct _tuple1*_tmp2B0;void*_tmp2B1;struct Cyc_List_List*_tmp2B2;
struct Cyc_Absyn_Exp*_tmp2B3;void*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B5;struct Cyc_Core_Opt*
_tmp2B6;struct Cyc_Absyn_Exp*_tmp2B7;struct Cyc_Absyn_Exp*_tmp2B8;struct Cyc_Absyn_Exp*
_tmp2B9;struct Cyc_Absyn_Exp*_tmp2BA;struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_Absyn_Exp*
_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BD;struct Cyc_List_List*_tmp2BE;struct Cyc_Absyn_Exp*
_tmp2BF;struct Cyc_List_List*_tmp2C0;struct Cyc_Absyn_VarargCallInfo*_tmp2C1;
struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_List_List*_tmp2C3;struct Cyc_Absyn_VarargCallInfo*
_tmp2C4;struct Cyc_Absyn_VarargCallInfo _tmp2C5;int _tmp2C6;struct Cyc_List_List*
_tmp2C7;struct Cyc_Absyn_VarargInfo*_tmp2C8;struct Cyc_Absyn_Exp*_tmp2C9;struct Cyc_Absyn_Exp*
_tmp2CA;struct Cyc_Absyn_Exp*_tmp2CB;struct Cyc_List_List*_tmp2CC;void*_tmp2CD;
void**_tmp2CE;struct Cyc_Absyn_Exp*_tmp2CF;struct Cyc_Absyn_Exp*_tmp2D0;struct Cyc_Absyn_Exp*
_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D3;void*_tmp2D4;
void*_tmp2D5;void*_tmp2D6;struct _tagged_arr*_tmp2D7;void*_tmp2D8;void*_tmp2D9;
unsigned int _tmp2DA;struct Cyc_Absyn_Exp*_tmp2DB;struct Cyc_Absyn_Exp*_tmp2DC;
struct _tagged_arr*_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DE;struct _tagged_arr*_tmp2DF;
struct Cyc_Absyn_Exp*_tmp2E0;struct Cyc_Absyn_Exp*_tmp2E1;struct Cyc_List_List*
_tmp2E2;struct Cyc_List_List*_tmp2E3;struct Cyc_Absyn_Vardecl*_tmp2E4;struct Cyc_Absyn_Exp*
_tmp2E5;struct Cyc_Absyn_Exp*_tmp2E6;int _tmp2E7;struct _tuple1*_tmp2E8;struct Cyc_List_List*
_tmp2E9;struct Cyc_List_List*_tmp2EA;struct Cyc_Absyn_Aggrdecl*_tmp2EB;void*
_tmp2EC;struct Cyc_List_List*_tmp2ED;struct Cyc_List_List*_tmp2EE;struct Cyc_Absyn_Tuniondecl*
_tmp2EF;struct Cyc_Absyn_Tunionfield*_tmp2F0;struct Cyc_List_List*_tmp2F1;struct
Cyc_Absyn_Tuniondecl*_tmp2F2;struct Cyc_Absyn_Tunionfield*_tmp2F3;struct Cyc_Absyn_MallocInfo
_tmp2F4;int _tmp2F5;struct Cyc_Absyn_Exp*_tmp2F6;void**_tmp2F7;struct Cyc_Absyn_Exp*
_tmp2F8;int _tmp2F9;struct Cyc_Absyn_Stmt*_tmp2FA;struct Cyc_Absyn_Fndecl*_tmp2FB;
_LL1D2: if(*((int*)_tmp2AC)!= 0)goto _LL1D4;_tmp2AD=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp2AC)->f1;if((int)_tmp2AD != 0)goto _LL1D4;_LL1D3: {struct Cyc_Absyn_Exp*_tmp2FC=
Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){if(nv->toplevel)(
void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_typ,_tmp2FC,
_tmp2FC))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp2FD[3];_tmp2FD[2]=_tmp2FC;_tmp2FD[1]=_tmp2FC;_tmp2FD[0]=
_tmp2FC;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp2FD,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}else{(void*)(e->r=(void*)((
void*)(Cyc_Absyn_signed_int_exp(0,0))->r));}goto _LL1D1;}_LL1D4: if(*((int*)
_tmp2AC)!= 0)goto _LL1D6;_LL1D5: goto _LL1D1;_LL1D6: if(*((int*)_tmp2AC)!= 1)goto
_LL1D8;_tmp2AE=((struct Cyc_Absyn_Var_e_struct*)_tmp2AC)->f1;_tmp2AF=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2AC)->f2;_LL1D7:{struct _handler_cons _tmp2FE;
_push_handler(& _tmp2FE);{int _tmp300=0;if(setjmp(_tmp2FE.handler))_tmp300=1;if(!
_tmp300){(void*)(e->r=(void*)((void*)(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*
d,struct _tuple1*k))Cyc_Dict_lookup)(nv->varmap,_tmp2AE))->r));;_pop_handler();}
else{void*_tmp2FF=(void*)_exn_thrown;void*_tmp302=_tmp2FF;_LL227: if(_tmp302 != 
Cyc_Dict_Absent)goto _LL229;_LL228:({struct Cyc_Std_String_pa_struct _tmp305;
_tmp305.tag=0;_tmp305.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp2AE);{
void*_tmp303[1]={& _tmp305};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp304="Can't find %s in exp_to_c, Var\n";
_tag_arr(_tmp304,sizeof(char),_get_zero_arr_size(_tmp304,32));}),_tag_arr(
_tmp303,sizeof(void*),1));}});_LL229:;_LL22A:(void)_throw(_tmp302);_LL226:;}}}
goto _LL1D1;_LL1D8: if(*((int*)_tmp2AC)!= 2)goto _LL1DA;_tmp2B0=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp2AC)->f1;_LL1D9:({void*_tmp306[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp307="unknownid";_tag_arr(
_tmp307,sizeof(char),_get_zero_arr_size(_tmp307,10));}),_tag_arr(_tmp306,sizeof(
void*),0));});_LL1DA: if(*((int*)_tmp2AC)!= 3)goto _LL1DC;_tmp2B1=(void*)((struct
Cyc_Absyn_Primop_e_struct*)_tmp2AC)->f1;_tmp2B2=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2AC)->f2;_LL1DB: {struct Cyc_List_List*_tmp308=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,
_tmp2B2);((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp2B2);{void*
_tmp309=_tmp2B1;_LL22C: if((int)_tmp309 != 19)goto _LL22E;_LL22D: {struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2B2))->hd;{void*
_tmp30A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);
struct Cyc_Absyn_ArrayInfo _tmp30B;struct Cyc_Absyn_Exp*_tmp30C;struct Cyc_Absyn_PtrInfo
_tmp30D;void*_tmp30E;struct Cyc_Absyn_Conref*_tmp30F;struct Cyc_Absyn_Conref*
_tmp310;struct Cyc_Absyn_Conref*_tmp311;_LL241: if(_tmp30A <= (void*)3?1:*((int*)
_tmp30A)!= 7)goto _LL243;_tmp30B=((struct Cyc_Absyn_ArrayType_struct*)_tmp30A)->f1;
_tmp30C=_tmp30B.num_elts;_LL242:(void*)(e->r=(void*)((void*)((struct Cyc_Absyn_Exp*)
_check_null(_tmp30C))->r));goto _LL240;_LL243: if(_tmp30A <= (void*)3?1:*((int*)
_tmp30A)!= 4)goto _LL245;_tmp30D=((struct Cyc_Absyn_PointerType_struct*)_tmp30A)->f1;
_tmp30E=(void*)_tmp30D.elt_typ;_tmp30F=_tmp30D.nullable;_tmp310=_tmp30D.bounds;
_tmp311=_tmp30D.zero_term;_LL244:{void*_tmp312=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp310);struct Cyc_Absyn_Exp*_tmp313;_LL248: if((int)_tmp312 != 0)goto _LL24A;
_LL249:(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp314[2];_tmp314[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp30E),0);_tmp314[0]=(struct Cyc_Absyn_Exp*)_tmp2B2->hd;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp314,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL247;_LL24A: if(_tmp312 <= (void*)1?1:*((int*)_tmp312)!= 0)
goto _LL247;_tmp313=((struct Cyc_Absyn_Upper_b_struct*)_tmp312)->f1;_LL24B: if(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp311))(void*)(e->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*
_tmp315[2];_tmp315[1]=_tmp313;_tmp315[0]=(struct Cyc_Absyn_Exp*)_tmp2B2->hd;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp315,
sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));else{if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp30F))(void*)(e->r=(void*)((void*)(Cyc_Absyn_conditional_exp(
arg,_tmp313,Cyc_Absyn_uint_exp(0,0),0))->r));else{(void*)(e->r=(void*)((void*)
_tmp313->r));goto _LL247;}}_LL247:;}goto _LL240;_LL245:;_LL246:({struct Cyc_Std_String_pa_struct
_tmp319;_tmp319.tag=0;_tmp319.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp318;_tmp318.tag=0;_tmp318.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v);{void*_tmp316[2]={& _tmp318,&
_tmp319};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp317="size primop applied to non-array %s (%s)";_tag_arr(_tmp317,
sizeof(char),_get_zero_arr_size(_tmp317,41));}),_tag_arr(_tmp316,sizeof(void*),2));}}});
_LL240:;}goto _LL22B;}_LL22E: if((int)_tmp309 != 0)goto _LL230;_LL22F:{void*_tmp31A=
Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp308))->hd);
struct Cyc_Absyn_PtrInfo _tmp31B;void*_tmp31C;struct Cyc_Absyn_Conref*_tmp31D;
struct Cyc_Absyn_Conref*_tmp31E;_LL24D: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 
4)goto _LL24F;_tmp31B=((struct Cyc_Absyn_PointerType_struct*)_tmp31A)->f1;_tmp31C=(
void*)_tmp31B.elt_typ;_tmp31D=_tmp31B.bounds;_tmp31E=_tmp31B.zero_term;_LL24E:{
void*_tmp31F=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp31D);struct Cyc_Absyn_Exp*
_tmp320;_LL252: if((int)_tmp31F != 0)goto _LL254;_LL253: {struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2B2))->hd;struct Cyc_Absyn_Exp*
e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2B2->tl))->hd;(
void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp321[3];_tmp321[2]=e2;_tmp321[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp31C),0);_tmp321[0]=e1;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp321,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));goto _LL251;}_LL254: if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 0)goto
_LL251;_tmp320=((struct Cyc_Absyn_Upper_b_struct*)_tmp31F)->f1;_LL255: if(((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp31E)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2B2))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp2B2->tl))->hd;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp322[3];_tmp322[2]=e2;_tmp322[1]=_tmp320;_tmp322[0]=e1;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp322,
sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}goto _LL251;_LL251:;}goto _LL24C;
_LL24F:;_LL250: goto _LL24C;_LL24C:;}goto _LL22B;_LL230: if((int)_tmp309 != 2)goto
_LL232;_LL231: {void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)((struct Cyc_List_List*)_check_null(_tmp308))->hd,& elt_typ)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2B2))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp2B2->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)
_check_null(_tmp308->tl))->hd)){(void*)(e1->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e2->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,
0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r));}else{(void*)(e->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*
_tmp323[3];_tmp323[2]=Cyc_Absyn_prim1_exp((void*)2,e2,0);_tmp323[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp323[0]=e1;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp323,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));}}goto _LL22B;}_LL232: if((int)_tmp309 != 5)goto _LL234;_LL233: goto _LL235;
_LL234: if((int)_tmp309 != 6)goto _LL236;_LL235: goto _LL237;_LL236: if((int)_tmp309 != 
7)goto _LL238;_LL237: goto _LL239;_LL238: if((int)_tmp309 != 9)goto _LL23A;_LL239: goto
_LL23B;_LL23A: if((int)_tmp309 != 8)goto _LL23C;_LL23B: goto _LL23D;_LL23C: if((int)
_tmp309 != 10)goto _LL23E;_LL23D: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2B2))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2B2->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp308))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp308->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(t1))(void*)(e1->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,
0))->r));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))(void*)(e2->r=(void*)((void*)(
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));
goto _LL22B;}_LL23E:;_LL23F: goto _LL22B;_LL22B:;}goto _LL1D1;}_LL1DC: if(*((int*)
_tmp2AC)!= 5)goto _LL1DE;_tmp2B3=((struct Cyc_Absyn_Increment_e_struct*)_tmp2AC)->f1;
_tmp2B4=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp2AC)->f2;_LL1DD: {void*
e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2B3->topt))->v;void*
ptr_type=(void*)0;void*elt_type=(void*)0;int fat_ptr=0;struct _tagged_arr incr_str=({
const char*_tmp330="increment";_tag_arr(_tmp330,sizeof(char),_get_zero_arr_size(
_tmp330,10));});if(_tmp2B4 == (void*)2?1: _tmp2B4 == (void*)3)incr_str=({const char*
_tmp324="decrement";_tag_arr(_tmp324,sizeof(char),_get_zero_arr_size(_tmp324,10));});
if(Cyc_Toc_is_zero_ptr_deref(_tmp2B3,& ptr_type,& fat_ptr,& elt_type)){({struct Cyc_Std_String_pa_struct
_tmp327;_tmp327.tag=0;_tmp327.f1=(struct _tagged_arr)incr_str;{void*_tmp325[1]={&
_tmp327};Cyc_Tcutil_terr(e->loc,({const char*_tmp326="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_arr(_tmp326,sizeof(char),_get_zero_arr_size(_tmp326,74));}),_tag_arr(
_tmp325,sizeof(void*),1));}});(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp328=_cycalloc(sizeof(*_tmp328));_tmp328[0]=({struct Cyc_Core_Impossible_struct
_tmp329;_tmp329.tag=Cyc_Core_Impossible;_tmp329.f1=({const char*_tmp32A="in-place inc on zero-term";
_tag_arr(_tmp32A,sizeof(char),_get_zero_arr_size(_tmp32A,26));});_tmp329;});
_tmp328;}));}Cyc_Toc_exp_to_c(nv,_tmp2B3);{void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*_tmp32B=(_tmp2B4 == (void*)1?1: _tmp2B4 == (
void*)3)?Cyc_Toc__tagged_arr_inplace_plus_post_e: Cyc_Toc__tagged_arr_inplace_plus_e;
int _tmp32C=(_tmp2B4 == (void*)0?1: _tmp2B4 == (void*)1)?1: - 1;(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(_tmp32B,({struct Cyc_Absyn_Exp*_tmp32D[3];_tmp32D[2]=
Cyc_Absyn_signed_int_exp(_tmp32C,0);_tmp32D[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0);_tmp32D[0]=Cyc_Absyn_address_exp(_tmp2B3,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp32D,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*_tmp32E=_tmp2B4 == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_e:
Cyc_Toc__zero_arr_inplace_plus_e;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
_tmp32E,({struct Cyc_Absyn_Exp*_tmp32F[2];_tmp32F[1]=Cyc_Absyn_signed_int_exp(1,0);
_tmp32F[0]=_tmp2B3;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp32F,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}else{if(elt_typ == (
void*)0?!Cyc_Absyn_is_lvalue(_tmp2B3): 0){Cyc_Toc_lvalue_assign(_tmp2B3,0,Cyc_Toc_incr_lvalue,
_tmp2B4);(void*)(e->r=(void*)((void*)_tmp2B3->r));}}}goto _LL1D1;}}_LL1DE: if(*((
int*)_tmp2AC)!= 4)goto _LL1E0;_tmp2B5=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp2AC)->f1;_tmp2B6=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp2AC)->f2;_tmp2B7=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp2AC)->f3;_LL1DF: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_fat=0;if(Cyc_Toc_is_zero_ptr_deref(_tmp2B5,&
ptr_type,& is_fat,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp2B5,_tmp2B6,
_tmp2B7,ptr_type,is_fat,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp2B5);void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2B5->topt))->v;
void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2B7->topt))->v;Cyc_Toc_exp_to_c(
nv,_tmp2B5);Cyc_Toc_exp_to_c(nv,_tmp2B7);{int done=0;if(_tmp2B6 != 0){void*elt_typ=(
void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;{void*_tmp331=(void*)_tmp2B6->v;_LL257: if((int)_tmp331 != 0)goto _LL259;
_LL258: change=_tmp2B7;goto _LL256;_LL259: if((int)_tmp331 != 2)goto _LL25B;_LL25A:
change=Cyc_Absyn_prim1_exp((void*)2,_tmp2B7,0);goto _LL256;_LL25B:;_LL25C:({void*
_tmp332[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp333="bad t ? pointer arithmetic";_tag_arr(_tmp333,sizeof(char),
_get_zero_arr_size(_tmp333,27));}),_tag_arr(_tmp332,sizeof(void*),0));});_LL256:;}
done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp334[3];_tmp334[2]=change;_tmp334[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp334[0]=Cyc_Absyn_address_exp(_tmp2B5,0);((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp334,
sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp335=(void*)_tmp2B6->v;_LL25E: if((int)_tmp335 != 0)
goto _LL260;_LL25F: done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp336[2];_tmp336[1]=_tmp2B7;_tmp336[0]=_tmp2B5;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp336,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL25D;_LL260:;_LL261:({void*_tmp337[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp338="bad zero-terminated pointer arithmetic";
_tag_arr(_tmp338,sizeof(char),_get_zero_arr_size(_tmp338,39));}),_tag_arr(
_tmp337,sizeof(void*),0));});_LL25D:;}}}if(!done){if(e1_poly)(void*)(_tmp2B7->r=(
void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((
void*)_tmp2B7->r,0),0))->r));if(!Cyc_Absyn_is_lvalue(_tmp2B5)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
struct _tuple9*),struct _tuple9*f_env))Cyc_Toc_lvalue_assign)(_tmp2B5,0,Cyc_Toc_assignop_lvalue,({
struct _tuple9*_tmp339=_cycalloc(sizeof(struct _tuple9)* 1);_tmp339[0]=({struct
_tuple9 _tmp33A;_tmp33A.f1=_tmp2B6;_tmp33A.f2=_tmp2B7;_tmp33A;});_tmp339;}));(
void*)(e->r=(void*)((void*)_tmp2B5->r));}}goto _LL1D1;}}}_LL1E0: if(*((int*)
_tmp2AC)!= 6)goto _LL1E2;_tmp2B8=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2AC)->f1;
_tmp2B9=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2AC)->f2;_tmp2BA=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp2AC)->f3;_LL1E1: Cyc_Toc_exp_to_c(nv,_tmp2B8);Cyc_Toc_exp_to_c(nv,_tmp2B9);
Cyc_Toc_exp_to_c(nv,_tmp2BA);goto _LL1D1;_LL1E2: if(*((int*)_tmp2AC)!= 7)goto
_LL1E4;_tmp2BB=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp2AC)->f1;_tmp2BC=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp2AC)->f2;_LL1E3: Cyc_Toc_exp_to_c(nv,_tmp2BB);Cyc_Toc_exp_to_c(
nv,_tmp2BC);goto _LL1D1;_LL1E4: if(*((int*)_tmp2AC)!= 8)goto _LL1E6;_tmp2BD=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp2AC)->f1;_tmp2BE=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp2AC)->f2;_LL1E5: _tmp2BF=_tmp2BD;_tmp2C0=_tmp2BE;goto _LL1E7;_LL1E6: if(*((int*)
_tmp2AC)!= 9)goto _LL1E8;_tmp2BF=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2AC)->f1;
_tmp2C0=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2AC)->f2;_tmp2C1=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp2AC)->f3;if(_tmp2C1 != 0)goto _LL1E8;_LL1E7: Cyc_Toc_exp_to_c(nv,_tmp2BF);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp2C0);goto _LL1D1;
_LL1E8: if(*((int*)_tmp2AC)!= 9)goto _LL1EA;_tmp2C2=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp2AC)->f1;_tmp2C3=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2AC)->f2;_tmp2C4=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp2AC)->f3;if(_tmp2C4 == 0)goto _LL1EA;_tmp2C5=*
_tmp2C4;_tmp2C6=_tmp2C5.num_varargs;_tmp2C7=_tmp2C5.injectors;_tmp2C8=_tmp2C5.vai;
_LL1E9:{struct _RegionHandle _tmp33B=_new_region("r");struct _RegionHandle*r=&
_tmp33B;_push_region(r);{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp2C6,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp2C8->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp2C3);int num_normargs=num_args - _tmp2C6;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp2C3=_tmp2C3->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2C3))->hd);
new_args=({struct Cyc_List_List*_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C->hd=(
struct Cyc_Absyn_Exp*)_tmp2C3->hd;_tmp33C->tl=new_args;_tmp33C;});}}new_args=({
struct Cyc_List_List*_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp33E[3];_tmp33E[2]=num_varargs_exp;
_tmp33E[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp33E[0]=argvexp;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp33E,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);_tmp33D->tl=new_args;_tmp33D;});new_args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp2C2);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp2C2,new_args,
0),0);if(_tmp2C8->inject){struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp33F=Cyc_Tcutil_compress((
void*)_tmp2C8->type);struct Cyc_Absyn_TunionInfo _tmp340;void*_tmp341;struct Cyc_Absyn_Tuniondecl**
_tmp342;struct Cyc_Absyn_Tuniondecl*_tmp343;_LL263: if(_tmp33F <= (void*)3?1:*((int*)
_tmp33F)!= 2)goto _LL265;_tmp340=((struct Cyc_Absyn_TunionType_struct*)_tmp33F)->f1;
_tmp341=(void*)_tmp340.tunion_info;if(*((int*)_tmp341)!= 1)goto _LL265;_tmp342=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp341)->f1;_tmp343=*_tmp342;_LL264: tud=
_tmp343;goto _LL262;_LL265:;_LL266:({void*_tmp344[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp345="toc: unknown tunion in vararg with inject";
_tag_arr(_tmp345,sizeof(char),_get_zero_arr_size(_tmp345,42));}),_tag_arr(
_tmp344,sizeof(void*),0));});_LL262:;}{struct _tagged_arr vs=({unsigned int _tmp34D=(
unsigned int)_tmp2C6;struct _tuple1**_tmp34E=(struct _tuple1**)_region_malloc(r,
_check_times(sizeof(struct _tuple1*),_tmp34D));struct _tagged_arr _tmp350=_tag_arr(
_tmp34E,sizeof(struct _tuple1*),_tmp34D);{unsigned int _tmp34F=_tmp34D;
unsigned int i;for(i=0;i < _tmp34F;i ++){_tmp34E[i]=Cyc_Toc_temp_var();}}_tmp350;});
struct Cyc_List_List*_tmp346=0;{int i=_tmp2C6 - 1;for(0;i >= 0;-- i){_tmp346=({struct
Cyc_List_List*_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347->hd=Cyc_Toc_make_dle(
Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)
_check_unknown_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp347->tl=
_tmp346;_tmp347;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp(0,_tmp346,0),s,0);{int i=0;for(0;_tmp2C3 != 0;(((
_tmp2C3=_tmp2C3->tl,_tmp2C7=_tmp2C7->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct
Cyc_Absyn_Exp*)_tmp2C3->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_unknown_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp349;struct _tuple1*
_tmp34A;struct Cyc_List_List*_tmp34B;struct Cyc_Absyn_Tunionfield*_tmp348=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp2C7))->hd;_tmp349=*
_tmp348;_tmp34A=_tmp349.name;_tmp34B=_tmp349.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp34B))->hd))).f2);
Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Absyn_cast_exp(
field_typ,arg,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp34A,
1),0),s,0);s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp34A,({const char*_tmp34C="_struct";_tag_arr(_tmp34C,sizeof(char),
_get_zero_arr_size(_tmp34C,8));}))),0,s,0);}}}}}else{{int i=0;for(0;_tmp2C3 != 0;(
_tmp2C3=_tmp2C3->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp2C3->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((
unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp2C3->hd,0),s,0);}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,0,s,0);}(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};
_pop_region(r);}goto _LL1D1;_LL1EA: if(*((int*)_tmp2AC)!= 10)goto _LL1EC;_tmp2C9=((
struct Cyc_Absyn_Throw_e_struct*)_tmp2AC)->f1;_LL1EB: Cyc_Toc_exp_to_c(nv,_tmp2C9);(
void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),
Cyc_Toc_newthrow_exp(_tmp2C9),0))->r));goto _LL1D1;_LL1EC: if(*((int*)_tmp2AC)!= 
11)goto _LL1EE;_tmp2CA=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp2AC)->f1;
_LL1ED: Cyc_Toc_exp_to_c(nv,_tmp2CA);goto _LL1D1;_LL1EE: if(*((int*)_tmp2AC)!= 12)
goto _LL1F0;_tmp2CB=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2AC)->f1;_tmp2CC=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp2AC)->f2;_LL1EF: Cyc_Toc_exp_to_c(nv,
_tmp2CB);for(0;_tmp2CC != 0;_tmp2CC=_tmp2CC->tl){void*k=Cyc_Tcutil_typ_kind((void*)
_tmp2CC->hd);if(k != (void*)4?k != (void*)3: 0){{void*_tmp351=Cyc_Tcutil_compress((
void*)_tmp2CC->hd);_LL268: if(_tmp351 <= (void*)3?1:*((int*)_tmp351)!= 1)goto
_LL26A;_LL269: goto _LL26B;_LL26A: if(_tmp351 <= (void*)3?1:*((int*)_tmp351)!= 2)
goto _LL26C;_LL26B: continue;_LL26C:;_LL26D:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp2CB,
0))->r));goto _LL267;_LL267:;}break;}}goto _LL1D1;_LL1F0: if(*((int*)_tmp2AC)!= 13)
goto _LL1F2;_tmp2CD=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2AC)->f1;_tmp2CE=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2AC)->f1);_tmp2CF=((struct
Cyc_Absyn_Cast_e_struct*)_tmp2AC)->f2;_LL1F1: {void*old_t2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2CF->topt))->v;void*new_typ=*((void**)_tmp2CE);*((void**)_tmp2CE)=
Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp2CF);{struct _tuple0 _tmp353=({
struct _tuple0 _tmp352;_tmp352.f1=Cyc_Tcutil_compress(old_t2);_tmp352.f2=Cyc_Tcutil_compress(
new_typ);_tmp352;});void*_tmp354;struct Cyc_Absyn_PtrInfo _tmp355;void*_tmp356;
struct Cyc_Absyn_PtrInfo _tmp357;void*_tmp358;struct Cyc_Absyn_PtrInfo _tmp359;void*
_tmp35A;_LL26F: _tmp354=_tmp353.f1;if(_tmp354 <= (void*)3?1:*((int*)_tmp354)!= 4)
goto _LL271;_tmp355=((struct Cyc_Absyn_PointerType_struct*)_tmp354)->f1;_tmp356=
_tmp353.f2;if(_tmp356 <= (void*)3?1:*((int*)_tmp356)!= 4)goto _LL271;_tmp357=((
struct Cyc_Absyn_PointerType_struct*)_tmp356)->f1;_LL270: {int _tmp35B=((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp355.nullable);int
_tmp35C=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp357.nullable);
void*_tmp35D=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp355.bounds);void*
_tmp35E=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp357.bounds);int _tmp35F=((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp355.zero_term);
int _tmp360=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp357.zero_term);{struct _tuple0 _tmp362=({struct _tuple0 _tmp361;_tmp361.f1=
_tmp35D;_tmp361.f2=_tmp35E;_tmp361;});void*_tmp363;struct Cyc_Absyn_Exp*_tmp364;
void*_tmp365;struct Cyc_Absyn_Exp*_tmp366;void*_tmp367;struct Cyc_Absyn_Exp*
_tmp368;void*_tmp369;void*_tmp36A;void*_tmp36B;struct Cyc_Absyn_Exp*_tmp36C;void*
_tmp36D;void*_tmp36E;_LL276: _tmp363=_tmp362.f1;if(_tmp363 <= (void*)1?1:*((int*)
_tmp363)!= 0)goto _LL278;_tmp364=((struct Cyc_Absyn_Upper_b_struct*)_tmp363)->f1;
_tmp365=_tmp362.f2;if(_tmp365 <= (void*)1?1:*((int*)_tmp365)!= 0)goto _LL278;
_tmp366=((struct Cyc_Absyn_Upper_b_struct*)_tmp365)->f1;_LL277: if(_tmp35B?!
_tmp35C: 0){if(nv->toplevel)({void*_tmp36F[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp370="can't do NULL-check conversion at top-level";
_tag_arr(_tmp370,sizeof(char),_get_zero_arr_size(_tmp370,44));}),_tag_arr(
_tmp36F,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(*((
void**)_tmp2CE),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp371=_cycalloc(sizeof(*_tmp371));_tmp371->hd=_tmp2CF;_tmp371->tl=0;_tmp371;}),
0),0))->r));}goto _LL275;_LL278: _tmp367=_tmp362.f1;if(_tmp367 <= (void*)1?1:*((int*)
_tmp367)!= 0)goto _LL27A;_tmp368=((struct Cyc_Absyn_Upper_b_struct*)_tmp367)->f1;
_tmp369=_tmp362.f2;if((int)_tmp369 != 0)goto _LL27A;_LL279: if(nv->toplevel){if((
_tmp35F?!(_tmp357.tq).q_const: 0)?!_tmp360: 0)_tmp368=Cyc_Absyn_prim2_exp((void*)2,
_tmp368,Cyc_Absyn_uint_exp(1,0),0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(
old_t2,_tmp368,_tmp2CF))->r));}else{if(_tmp35F){struct _tuple1*_tmp372=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp373=Cyc_Absyn_var_exp(_tmp372,0);struct Cyc_Absyn_Exp*
_tmp374=Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*
_tmp379[2];_tmp379[1]=_tmp368;_tmp379[0]=_tmp373;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp379,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);if(!_tmp360?!(_tmp357.tq).q_const: 0)_tmp374=Cyc_Absyn_prim2_exp((void*)
2,_tmp374,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp375=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp357.elt_typ),0);struct Cyc_Absyn_Exp*_tmp376=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp378[3];_tmp378[2]=_tmp374;_tmp378[1]=
_tmp375;_tmp378[0]=_tmp373;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp378,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp377=Cyc_Absyn_exp_stmt(_tmp376,0);_tmp377=Cyc_Absyn_declare_stmt(_tmp372,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp2CF,_tmp377,0);(void*)(e->r=(void*)((void*)(
Cyc_Absyn_stmt_exp(_tmp377,0))->r));}}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp37A[3];_tmp37A[2]=_tmp368;_tmp37A[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)_tmp357.elt_typ),0);_tmp37A[0]=
_tmp2CF;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp37A,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}goto _LL275;_LL27A: _tmp36A=
_tmp362.f1;if((int)_tmp36A != 0)goto _LL27C;_tmp36B=_tmp362.f2;if(_tmp36B <= (void*)
1?1:*((int*)_tmp36B)!= 0)goto _LL27C;_tmp36C=((struct Cyc_Absyn_Upper_b_struct*)
_tmp36B)->f1;_LL27B: if(nv->toplevel)({void*_tmp37B[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp37C="can't coerce t? to t* or t@ at the top-level";
_tag_arr(_tmp37C,sizeof(char),_get_zero_arr_size(_tmp37C,45));}),_tag_arr(
_tmp37B,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp37D=_tmp36C;if(_tmp35F?!
_tmp360: 0)_tmp37D=Cyc_Absyn_add_exp(_tmp36C,Cyc_Absyn_uint_exp(1,0),0);{struct
Cyc_Absyn_Exp*_tmp37E=Cyc_Absyn_fncall_exp(Cyc_Toc__untag_arr_e,({struct Cyc_Absyn_Exp*
_tmp380[3];_tmp380[2]=_tmp37D;_tmp380[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp355.elt_typ),0);_tmp380[0]=_tmp2CF;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp380,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);if(_tmp35C)(void*)(_tmp37E->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp37F=_cycalloc(sizeof(*_tmp37F));_tmp37F->hd=Cyc_Absyn_copy_exp(
_tmp37E);_tmp37F->tl=0;_tmp37F;}),0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(*((
void**)_tmp2CE),_tmp37E,0))->r));goto _LL275;}}_LL27C: _tmp36D=_tmp362.f1;if((int)
_tmp36D != 0)goto _LL275;_tmp36E=_tmp362.f2;if((int)_tmp36E != 0)goto _LL275;_LL27D:
if((_tmp35F?!_tmp360: 0)?!(_tmp357.tq).q_const: 0){if(nv->toplevel)({void*_tmp381[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const
char*_tmp382="can't coerce a ?ZEROTERM to a non-const ?NOZEROTERM at toplevel";
_tag_arr(_tmp382,sizeof(char),_get_zero_arr_size(_tmp382,64));}),_tag_arr(
_tmp381,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_ptr_decrease_size_e,({struct Cyc_Absyn_Exp*_tmp383[3];_tmp383[2]=
Cyc_Absyn_uint_exp(1,0);_tmp383[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp355.elt_typ),0);_tmp383[0]=_tmp2CF;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp383,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));}_LL275:;}goto _LL26E;}_LL271: _tmp358=_tmp353.f1;if(_tmp358 <= (void*)3?1:*((
int*)_tmp358)!= 4)goto _LL273;_tmp359=((struct Cyc_Absyn_PointerType_struct*)
_tmp358)->f1;_tmp35A=_tmp353.f2;if(_tmp35A <= (void*)3?1:*((int*)_tmp35A)!= 5)
goto _LL273;_LL272:{void*_tmp384=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp359.bounds);_LL27F: if((int)_tmp384 != 0)goto _LL281;_LL280:(void*)(_tmp2CF->r=(
void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)_tmp2CF->r,
_tmp2CF->loc),Cyc_Toc_curr_sp,0))->r));goto _LL27E;_LL281:;_LL282: goto _LL27E;
_LL27E:;}goto _LL26E;_LL273:;_LL274: goto _LL26E;_LL26E:;}goto _LL1D1;}_LL1F2: if(*((
int*)_tmp2AC)!= 14)goto _LL1F4;_tmp2D0=((struct Cyc_Absyn_Address_e_struct*)
_tmp2AC)->f1;_LL1F3:{void*_tmp385=(void*)_tmp2D0->r;struct _tuple1*_tmp386;struct
Cyc_List_List*_tmp387;struct Cyc_List_List*_tmp388;struct Cyc_List_List*_tmp389;
_LL284: if(*((int*)_tmp385)!= 28)goto _LL286;_tmp386=((struct Cyc_Absyn_Struct_e_struct*)
_tmp385)->f1;_tmp387=((struct Cyc_Absyn_Struct_e_struct*)_tmp385)->f2;_tmp388=((
struct Cyc_Absyn_Struct_e_struct*)_tmp385)->f3;_LL285: if(nv->toplevel)({struct Cyc_Std_String_pa_struct
_tmp38C;_tmp38C.tag=0;_tmp38C.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp2D0->loc);{void*_tmp38A[1]={& _tmp38C};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp38B="%s: & on non-identifiers at the top-level";
_tag_arr(_tmp38B,sizeof(char),_get_zero_arr_size(_tmp38B,42));}),_tag_arr(
_tmp38A,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2D0->topt))->v,_tmp387 != 0,1,0,
_tmp388,_tmp386))->r));goto _LL283;_LL286: if(*((int*)_tmp385)!= 24)goto _LL288;
_tmp389=((struct Cyc_Absyn_Tuple_e_struct*)_tmp385)->f1;_LL287: if(nv->toplevel)({
struct Cyc_Std_String_pa_struct _tmp38F;_tmp38F.tag=0;_tmp38F.f1=(struct
_tagged_arr)Cyc_Position_string_of_segment(_tmp2D0->loc);{void*_tmp38D[1]={&
_tmp38F};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp38E="%s: & on non-identifiers at the top-level";_tag_arr(_tmp38E,
sizeof(char),_get_zero_arr_size(_tmp38E,42));}),_tag_arr(_tmp38D,sizeof(void*),1));}});(
void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,0,_tmp389))->r));goto _LL283;
_LL288:;_LL289: Cyc_Toc_exp_to_c(nv,_tmp2D0);if(!Cyc_Absyn_is_lvalue(_tmp2D0)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp2D0,0,Cyc_Toc_address_lvalue,
1);(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp2D0,0))->r));}goto _LL283;
_LL283:;}goto _LL1D1;_LL1F4: if(*((int*)_tmp2AC)!= 15)goto _LL1F6;_tmp2D1=((struct
Cyc_Absyn_New_e_struct*)_tmp2AC)->f1;_tmp2D2=((struct Cyc_Absyn_New_e_struct*)
_tmp2AC)->f2;_LL1F5: if(nv->toplevel)({struct Cyc_Std_String_pa_struct _tmp392;
_tmp392.tag=0;_tmp392.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp2D2->loc);{void*_tmp390[1]={& _tmp392};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp391="%s: new at top-level";
_tag_arr(_tmp391,sizeof(char),_get_zero_arr_size(_tmp391,21));}),_tag_arr(
_tmp390,sizeof(void*),1));}});{void*_tmp393=(void*)_tmp2D2->r;struct Cyc_List_List*
_tmp394;struct Cyc_Absyn_Vardecl*_tmp395;struct Cyc_Absyn_Exp*_tmp396;struct Cyc_Absyn_Exp*
_tmp397;int _tmp398;struct _tuple1*_tmp399;struct Cyc_List_List*_tmp39A;struct Cyc_List_List*
_tmp39B;struct Cyc_List_List*_tmp39C;_LL28B: if(*((int*)_tmp393)!= 26)goto _LL28D;
_tmp394=((struct Cyc_Absyn_Array_e_struct*)_tmp393)->f1;_LL28C: {struct _tuple1*
_tmp39D=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp39E=Cyc_Absyn_var_exp(_tmp39D,
0);struct Cyc_Absyn_Stmt*_tmp39F=Cyc_Toc_init_array(nv,_tmp39E,_tmp394,Cyc_Absyn_exp_stmt(
_tmp39E,0));void*old_elt_typ;{void*_tmp3A0=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_PtrInfo _tmp3A1;void*_tmp3A2;struct Cyc_Absyn_Tqual _tmp3A3;struct Cyc_Absyn_Conref*
_tmp3A4;_LL296: if(_tmp3A0 <= (void*)3?1:*((int*)_tmp3A0)!= 4)goto _LL298;_tmp3A1=((
struct Cyc_Absyn_PointerType_struct*)_tmp3A0)->f1;_tmp3A2=(void*)_tmp3A1.elt_typ;
_tmp3A3=_tmp3A1.tq;_tmp3A4=_tmp3A1.zero_term;_LL297: old_elt_typ=_tmp3A2;goto
_LL295;_LL298:;_LL299: old_elt_typ=({void*_tmp3A5[0]={};Cyc_Toc_toc_impos(({const
char*_tmp3A6="exp_to_c:new array expression doesn't have ptr type";_tag_arr(
_tmp3A6,sizeof(char),_get_zero_arr_size(_tmp3A6,52));}),_tag_arr(_tmp3A5,sizeof(
void*),0));});_LL295:;}{void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp3A7=
Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp3A8=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp394),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp2D1 == 0)e1=Cyc_Toc_malloc_exp(
old_elt_typ,_tmp3A8);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_check_null(_tmp2D1);Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp3A8);}(
void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp39D,
_tmp3A7,(struct Cyc_Absyn_Exp*)e1,_tmp39F,0),0))->r));goto _LL28A;}}_LL28D: if(*((
int*)_tmp393)!= 27)goto _LL28F;_tmp395=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp393)->f1;_tmp396=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp393)->f2;
_tmp397=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp393)->f3;_tmp398=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp393)->f4;_LL28E: {int is_tagged_ptr=0;{void*
_tmp3A9=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp3AA;void*_tmp3AB;
struct Cyc_Absyn_Tqual _tmp3AC;struct Cyc_Absyn_Conref*_tmp3AD;struct Cyc_Absyn_Conref*
_tmp3AE;_LL29B: if(_tmp3A9 <= (void*)3?1:*((int*)_tmp3A9)!= 4)goto _LL29D;_tmp3AA=((
struct Cyc_Absyn_PointerType_struct*)_tmp3A9)->f1;_tmp3AB=(void*)_tmp3AA.elt_typ;
_tmp3AC=_tmp3AA.tq;_tmp3AD=_tmp3AA.bounds;_tmp3AE=_tmp3AA.zero_term;_LL29C:
is_tagged_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp3AD)== (void*)0;goto
_LL29A;_LL29D:;_LL29E:({void*_tmp3AF[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp3B0="exp_to_c: comprehension not an array type";
_tag_arr(_tmp3B0,sizeof(char),_get_zero_arr_size(_tmp3B0,42));}),_tag_arr(
_tmp3AF,sizeof(void*),0));});_LL29A:;}{struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp397->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*
ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp396);{
struct Cyc_Absyn_Exp*_tmp3B1=Cyc_Absyn_var_exp(max,0);if(_tmp398)_tmp3B1=Cyc_Absyn_add_exp(
_tmp3B1,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(
nv,Cyc_Absyn_var_exp(a,0),_tmp395,Cyc_Absyn_var_exp(max,0),_tmp397,_tmp398,Cyc_Toc_skip_stmt_dl(),
1);{struct _RegionHandle _tmp3B2=_new_region("r");struct _RegionHandle*r=& _tmp3B2;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp3C2=
_region_malloc(r,sizeof(*_tmp3C2));_tmp3C2->hd=({struct _tuple10*_tmp3C3=
_region_malloc(r,sizeof(*_tmp3C3));_tmp3C3->f1=max;_tmp3C3->f2=Cyc_Absyn_uint_typ;
_tmp3C3->f3=(struct Cyc_Absyn_Exp*)_tmp396;_tmp3C3;});_tmp3C2->tl=0;_tmp3C2;});
struct Cyc_Absyn_Exp*ai;if(_tmp2D1 == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp3B3[2];_tmp3B3[1]=_tmp3B1;
_tmp3B3[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp3B3,sizeof(struct Cyc_Absyn_Exp*),2));}),
0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(_tmp2D1);Cyc_Toc_exp_to_c(
nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp3B4[2];_tmp3B4[1]=_tmp3B1;_tmp3B4[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp3B4,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct Cyc_Absyn_Exp*ainit=Cyc_Absyn_cast_exp(
ptr_typ,ai,0);decls=({struct Cyc_List_List*_tmp3B5=_region_malloc(r,sizeof(*
_tmp3B5));_tmp3B5->hd=({struct _tuple10*_tmp3B6=_region_malloc(r,sizeof(*_tmp3B6));
_tmp3B6->f1=a;_tmp3B6->f2=ptr_typ;_tmp3B6->f3=(struct Cyc_Absyn_Exp*)ainit;
_tmp3B6;});_tmp3B5->tl=decls;_tmp3B5;});if(is_tagged_ptr){struct _tuple1*_tmp3B7=
Cyc_Toc_temp_var();void*_tmp3B8=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*
_tmp3B9=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp3BC[3];
_tmp3BC[2]=_tmp3B1;_tmp3BC[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp3BC[0]=Cyc_Absyn_var_exp(
a,0);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp3BC,sizeof(struct Cyc_Absyn_Exp*),3));}),0);decls=({struct Cyc_List_List*
_tmp3BA=_region_malloc(r,sizeof(*_tmp3BA));_tmp3BA->hd=({struct _tuple10*_tmp3BB=
_region_malloc(r,sizeof(*_tmp3BB));_tmp3BB->f1=_tmp3B7;_tmp3BB->f2=_tmp3B8;
_tmp3BB->f3=(struct Cyc_Absyn_Exp*)_tmp3B9;_tmp3BB;});_tmp3BA->tl=decls;_tmp3BA;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp3B7,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp3BD=decls;for(0;_tmp3BD != 0;_tmp3BD=_tmp3BD->tl){struct
_tuple1*_tmp3BF;void*_tmp3C0;struct Cyc_Absyn_Exp*_tmp3C1;struct _tuple10 _tmp3BE=*((
struct _tuple10*)((struct _tuple10*)_tmp3BD->hd));_tmp3BF=_tmp3BE.f1;_tmp3C0=
_tmp3BE.f2;_tmp3C1=_tmp3BE.f3;s=Cyc_Absyn_declare_stmt(_tmp3BF,_tmp3C0,_tmp3C1,s,
0);}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}
goto _LL28A;}}}}_LL28F: if(*((int*)_tmp393)!= 28)goto _LL291;_tmp399=((struct Cyc_Absyn_Struct_e_struct*)
_tmp393)->f1;_tmp39A=((struct Cyc_Absyn_Struct_e_struct*)_tmp393)->f2;_tmp39B=((
struct Cyc_Absyn_Struct_e_struct*)_tmp393)->f3;_LL290:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2D2->topt))->v,
_tmp39A != 0,1,_tmp2D1,_tmp39B,_tmp399))->r));goto _LL28A;_LL291: if(*((int*)
_tmp393)!= 24)goto _LL293;_tmp39C=((struct Cyc_Absyn_Tuple_e_struct*)_tmp393)->f1;
_LL292:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,_tmp2D1,_tmp39C))->r));
goto _LL28A;_LL293:;_LL294: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2D2->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct
_tuple1*_tmp3C4=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3C5=Cyc_Absyn_var_exp(
_tmp3C4,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(
_tmp3C5,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp2D1 == 0)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(
_tmp2D1);Cyc_Toc_exp_to_c(nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{
void*_tmp3C6=(void*)_tmp2D2->r;void*_tmp3C7;struct Cyc_Absyn_Exp*_tmp3C8;_LL2A0:
if(*((int*)_tmp3C6)!= 13)goto _LL2A2;_tmp3C7=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp3C6)->f1;_tmp3C8=((struct Cyc_Absyn_Cast_e_struct*)_tmp3C6)->f2;_LL2A1:{
struct _tuple0 _tmp3CA=({struct _tuple0 _tmp3C9;_tmp3C9.f1=Cyc_Tcutil_compress(
_tmp3C7);_tmp3C9.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3C8->topt))->v);_tmp3C9;});void*_tmp3CB;struct Cyc_Absyn_PtrInfo _tmp3CC;void*
_tmp3CD;struct Cyc_Absyn_Conref*_tmp3CE;void*_tmp3CF;struct Cyc_Absyn_PtrInfo
_tmp3D0;struct Cyc_Absyn_Conref*_tmp3D1;_LL2A5: _tmp3CB=_tmp3CA.f1;if(_tmp3CB <= (
void*)3?1:*((int*)_tmp3CB)!= 4)goto _LL2A7;_tmp3CC=((struct Cyc_Absyn_PointerType_struct*)
_tmp3CB)->f1;_tmp3CD=(void*)_tmp3CC.elt_typ;_tmp3CE=_tmp3CC.bounds;_tmp3CF=
_tmp3CA.f2;if(_tmp3CF <= (void*)3?1:*((int*)_tmp3CF)!= 4)goto _LL2A7;_tmp3D0=((
struct Cyc_Absyn_PointerType_struct*)_tmp3CF)->f1;_tmp3D1=_tmp3D0.bounds;_LL2A6:{
struct _tuple0 _tmp3D3=({struct _tuple0 _tmp3D2;_tmp3D2.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp3CE);_tmp3D2.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp3D1);_tmp3D2;});
void*_tmp3D4;void*_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D6;_LL2AA: _tmp3D4=_tmp3D3.f1;
if((int)_tmp3D4 != 0)goto _LL2AC;_tmp3D5=_tmp3D3.f2;if(_tmp3D5 <= (void*)1?1:*((int*)
_tmp3D5)!= 0)goto _LL2AC;_tmp3D6=((struct Cyc_Absyn_Upper_b_struct*)_tmp3D5)->f1;
_LL2AB: Cyc_Toc_exp_to_c(nv,_tmp3C8);(void*)(inner_mexp->r=(void*)((void*)(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0))->r));done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__init_tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp3D7[4];_tmp3D7[3]=_tmp3D6;_tmp3D7[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp3CD),0);_tmp3D7[1]=_tmp3C8;_tmp3D7[0]=mexp;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp3D7,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))->r));goto _LL2A9;_LL2AC:;_LL2AD: goto _LL2A9;_LL2A9:;}goto _LL2A4;_LL2A7:;
_LL2A8: goto _LL2A4;_LL2A4:;}goto _LL29F;_LL2A2:;_LL2A3: goto _LL29F;_LL29F:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp3D8=Cyc_Absyn_exp_stmt(_tmp3C5,0);struct Cyc_Absyn_Exp*
_tmp3D9=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp2D2);_tmp3D8=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp3C5,_tmp3D9,0),_tmp2D2,0),
_tmp3D8,0);{void*_tmp3DA=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp3C4,_tmp3DA,(struct
Cyc_Absyn_Exp*)mexp,_tmp3D8,0),0))->r));}}goto _LL28A;}}_LL28A:;}goto _LL1D1;
_LL1F6: if(*((int*)_tmp2AC)!= 17)goto _LL1F8;_tmp2D3=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp2AC)->f1;_LL1F7: Cyc_Toc_exp_to_c(nv,_tmp2D3);goto _LL1D1;_LL1F8: if(*((int*)
_tmp2AC)!= 16)goto _LL1FA;_tmp2D4=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp2AC)->f1;_LL1F9:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp3DC;_tmp3DC.tag=16;_tmp3DC.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2D4);_tmp3DC;});
_tmp3DB;})));goto _LL1D1;_LL1FA: if(*((int*)_tmp2AC)!= 19)goto _LL1FC;_LL1FB:({void*
_tmp3DD[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3DE="__gen() in code generator";_tag_arr(_tmp3DE,sizeof(char),
_get_zero_arr_size(_tmp3DE,26));}),_tag_arr(_tmp3DD,sizeof(void*),0));});_LL1FC:
if(*((int*)_tmp2AC)!= 18)goto _LL1FE;_tmp2D5=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp2AC)->f1;_tmp2D6=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp2AC)->f2;
if(*((int*)_tmp2D6)!= 0)goto _LL1FE;_tmp2D7=((struct Cyc_Absyn_StructField_struct*)
_tmp2D6)->f1;_LL1FD:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp3E0;_tmp3E0.tag=18;_tmp3E0.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2D5);_tmp3E0.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp3E1=_cycalloc(sizeof(*
_tmp3E1));_tmp3E1[0]=({struct Cyc_Absyn_StructField_struct _tmp3E2;_tmp3E2.tag=0;
_tmp3E2.f1=_tmp2D7;_tmp3E2;});_tmp3E1;}));_tmp3E0;});_tmp3DF;})));goto _LL1D1;
_LL1FE: if(*((int*)_tmp2AC)!= 18)goto _LL200;_tmp2D8=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp2AC)->f1;_tmp2D9=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp2AC)->f2;
if(*((int*)_tmp2D9)!= 1)goto _LL200;_tmp2DA=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp2D9)->f1;_LL1FF:{void*_tmp3E3=Cyc_Tcutil_compress(_tmp2D8);struct Cyc_Absyn_AggrInfo
_tmp3E4;void*_tmp3E5;struct Cyc_List_List*_tmp3E6;_LL2AF: if(_tmp3E3 <= (void*)3?1:*((
int*)_tmp3E3)!= 10)goto _LL2B1;_tmp3E4=((struct Cyc_Absyn_AggrType_struct*)_tmp3E3)->f1;
_tmp3E5=(void*)_tmp3E4.aggr_info;_LL2B0: {struct Cyc_Absyn_Aggrdecl*_tmp3E7=Cyc_Absyn_get_known_aggrdecl(
_tmp3E5);if(_tmp3E7->impl == 0)({void*_tmp3E8[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp3E9="struct fields must be known";
_tag_arr(_tmp3E9,sizeof(char),_get_zero_arr_size(_tmp3E9,28));}),_tag_arr(
_tmp3E8,sizeof(void*),0));});_tmp3E6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3E7->impl))->fields;goto _LL2B2;}_LL2B1: if(_tmp3E3 <= (void*)3?1:*((int*)
_tmp3E3)!= 11)goto _LL2B3;_tmp3E6=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp3E3)->f2;
_LL2B2: {struct Cyc_Absyn_Aggrfield*_tmp3EA=((struct Cyc_Absyn_Aggrfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3E6,(int)_tmp2DA);(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp3EB=_cycalloc(sizeof(*
_tmp3EB));_tmp3EB[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp3EC;_tmp3EC.tag=18;
_tmp3EC.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2D8);_tmp3EC.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED[0]=({
struct Cyc_Absyn_StructField_struct _tmp3EE;_tmp3EE.tag=0;_tmp3EE.f1=_tmp3EA->name;
_tmp3EE;});_tmp3ED;}));_tmp3EC;});_tmp3EB;})));goto _LL2AE;}_LL2B3: if(_tmp3E3 <= (
void*)3?1:*((int*)_tmp3E3)!= 9)goto _LL2B5;_LL2B4:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp3EF=_cycalloc(sizeof(*_tmp3EF));_tmp3EF[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp3F0;_tmp3F0.tag=18;_tmp3F0.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp2D8);_tmp3F0.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp3F1=
_cycalloc(sizeof(*_tmp3F1));_tmp3F1[0]=({struct Cyc_Absyn_StructField_struct
_tmp3F2;_tmp3F2.tag=0;_tmp3F2.f1=Cyc_Absyn_fieldname((int)(_tmp2DA + 1));_tmp3F2;});
_tmp3F1;}));_tmp3F0;});_tmp3EF;})));goto _LL2AE;_LL2B5: if(_tmp3E3 <= (void*)3?1:*((
int*)_tmp3E3)!= 3)goto _LL2B7;_LL2B6: if(_tmp2DA == 0)(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));_tmp3F3[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp3F4;_tmp3F4.tag=18;_tmp3F4.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp2D8);_tmp3F4.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp3F5=
_cycalloc(sizeof(*_tmp3F5));_tmp3F5[0]=({struct Cyc_Absyn_StructField_struct
_tmp3F6;_tmp3F6.tag=0;_tmp3F6.f1=Cyc_Toc_tag_sp;_tmp3F6;});_tmp3F5;}));_tmp3F4;});
_tmp3F3;})));else{(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp3F7=_cycalloc(sizeof(*_tmp3F7));_tmp3F7[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp3F8;_tmp3F8.tag=18;_tmp3F8.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2D8);_tmp3F8.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp3F9=_cycalloc(sizeof(*
_tmp3F9));_tmp3F9[0]=({struct Cyc_Absyn_StructField_struct _tmp3FA;_tmp3FA.tag=0;
_tmp3FA.f1=Cyc_Absyn_fieldname((int)_tmp2DA);_tmp3FA;});_tmp3F9;}));_tmp3F8;});
_tmp3F7;})));}goto _LL2AE;_LL2B7:;_LL2B8:({void*_tmp3FB[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp3FC="impossible type for offsetof tuple index";
_tag_arr(_tmp3FC,sizeof(char),_get_zero_arr_size(_tmp3FC,41));}),_tag_arr(
_tmp3FB,sizeof(void*),0));});_LL2AE:;}goto _LL1D1;_LL200: if(*((int*)_tmp2AC)!= 20)
goto _LL202;_tmp2DB=((struct Cyc_Absyn_Deref_e_struct*)_tmp2AC)->f1;_LL201: {void*
_tmp3FD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2DB->topt))->v);{
void*_tmp3FE=_tmp3FD;struct Cyc_Absyn_PtrInfo _tmp3FF;void*_tmp400;void*_tmp401;
struct Cyc_Absyn_Conref*_tmp402;struct Cyc_Absyn_Tqual _tmp403;struct Cyc_Absyn_Conref*
_tmp404;struct Cyc_Absyn_Conref*_tmp405;_LL2BA: if(_tmp3FE <= (void*)3?1:*((int*)
_tmp3FE)!= 4)goto _LL2BC;_tmp3FF=((struct Cyc_Absyn_PointerType_struct*)_tmp3FE)->f1;
_tmp400=(void*)_tmp3FF.elt_typ;_tmp401=(void*)_tmp3FF.rgn_typ;_tmp402=_tmp3FF.nullable;
_tmp403=_tmp3FF.tq;_tmp404=_tmp3FF.bounds;_tmp405=_tmp3FF.zero_term;_LL2BB:{void*
_tmp406=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp404);_LL2BF: if(_tmp406 <= (
void*)1?1:*((int*)_tmp406)!= 0)goto _LL2C1;_LL2C0: {int do_null_check=Cyc_Toc_need_null_check(
_tmp2DB);Cyc_Toc_exp_to_c(nv,_tmp2DB);if(do_null_check)_tmp2DB=Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp407=_cycalloc(sizeof(*_tmp407));
_tmp407->hd=_tmp2DB;_tmp407->tl=0;_tmp407;}),0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c(_tmp3FD),_tmp2DB,0),0))->r));goto _LL2BE;}
_LL2C1: if((int)_tmp406 != 0)goto _LL2BE;_LL2C2: {struct Cyc_Absyn_Exp*_tmp408=Cyc_Absyn_uint_exp(
0,0);_tmp408->topt=({struct Cyc_Core_Opt*_tmp409=_cycalloc(sizeof(*_tmp409));
_tmp409->v=(void*)Cyc_Absyn_uint_typ;_tmp409;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(
_tmp2DB,_tmp408,0))->r));Cyc_Toc_exp_to_c(nv,e);goto _LL2BE;}_LL2BE:;}goto _LL2B9;
_LL2BC:;_LL2BD:({void*_tmp40A[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp40B="exp_to_c: Deref: non-pointer";
_tag_arr(_tmp40B,sizeof(char),_get_zero_arr_size(_tmp40B,29));}),_tag_arr(
_tmp40A,sizeof(void*),0));});_LL2B9:;}goto _LL1D1;}_LL202: if(*((int*)_tmp2AC)!= 
21)goto _LL204;_tmp2DC=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp2AC)->f1;
_tmp2DD=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp2AC)->f2;_LL203: Cyc_Toc_exp_to_c(
nv,_tmp2DC);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((
void*)e->r,0),0))->r));goto _LL1D1;_LL204: if(*((int*)_tmp2AC)!= 22)goto _LL206;
_tmp2DE=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2AC)->f1;_tmp2DF=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp2AC)->f2;_LL205: {void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2DE->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp2DE);
Cyc_Toc_exp_to_c(nv,_tmp2DE);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp40D;
void*_tmp40E;struct Cyc_Absyn_Conref*_tmp40F;struct Cyc_Absyn_Tqual _tmp410;struct
Cyc_Absyn_Conref*_tmp411;struct Cyc_Absyn_Conref*_tmp412;struct Cyc_Absyn_PtrInfo
_tmp40C=Cyc_Toc_get_ptr_type(e1typ);_tmp40D=(void*)_tmp40C.elt_typ;_tmp40E=(void*)
_tmp40C.rgn_typ;_tmp40F=_tmp40C.nullable;_tmp410=_tmp40C.tq;_tmp411=_tmp40C.bounds;
_tmp412=_tmp40C.zero_term;{void*_tmp413=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp411);struct Cyc_Absyn_Exp*_tmp414;_LL2C4: if(_tmp413 <= (void*)1?1:*((int*)
_tmp413)!= 0)goto _LL2C6;_tmp414=((struct Cyc_Absyn_Upper_b_struct*)_tmp413)->f1;
_LL2C5: {unsigned int _tmp416;int _tmp417;struct _tuple5 _tmp415=Cyc_Evexp_eval_const_uint_exp(
_tmp414);_tmp416=_tmp415.f1;_tmp417=_tmp415.f2;if(_tmp417){if(_tmp416 < 1)({void*
_tmp418[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp419="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_arr(_tmp419,
sizeof(char),_get_zero_arr_size(_tmp419,44));}),_tag_arr(_tmp418,sizeof(void*),0));});
if(do_null_check)(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2DE->topt))->v),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp41A=
_cycalloc(sizeof(*_tmp41A));_tmp41A->hd=_tmp2DE;_tmp41A->tl=0;_tmp41A;}),0),0),
_tmp2DF,0))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2DE->topt))->v),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*
_tmp41B[4];_tmp41B[3]=Cyc_Absyn_uint_exp(0,0);_tmp41B[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp40D,0);_tmp41B[1]=_tmp414;_tmp41B[0]=_tmp2DE;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp41B,sizeof(struct Cyc_Absyn_Exp*),
4));}),0),0),_tmp2DF,0))->r));}goto _LL2C3;}_LL2C6: if((int)_tmp413 != 0)goto _LL2C3;
_LL2C7: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp40D);(void*)(_tmp2DE->r=(void*)((
void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp410),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*_tmp41C[3];_tmp41C[2]=
Cyc_Absyn_uint_exp(0,0);_tmp41C[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp41C[0]=Cyc_Absyn_copy_exp(
_tmp2DE);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp41C,sizeof(struct Cyc_Absyn_Exp*),3));}),0),0))->r));goto _LL2C3;}_LL2C3:;}if(
is_poly)(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,
0),0))->r));goto _LL1D1;}}_LL206: if(*((int*)_tmp2AC)!= 23)goto _LL208;_tmp2E0=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp2AC)->f1;_tmp2E1=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp2AC)->f2;_LL207: {void*_tmp41D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2E0->topt))->v);{void*_tmp41E=_tmp41D;struct Cyc_List_List*
_tmp41F;struct Cyc_Absyn_PtrInfo _tmp420;void*_tmp421;void*_tmp422;struct Cyc_Absyn_Conref*
_tmp423;struct Cyc_Absyn_Tqual _tmp424;struct Cyc_Absyn_Conref*_tmp425;struct Cyc_Absyn_Conref*
_tmp426;_LL2C9: if(_tmp41E <= (void*)3?1:*((int*)_tmp41E)!= 9)goto _LL2CB;_tmp41F=((
struct Cyc_Absyn_TupleType_struct*)_tmp41E)->f1;_LL2CA: Cyc_Toc_exp_to_c(nv,
_tmp2E0);Cyc_Toc_exp_to_c(nv,_tmp2E1);{unsigned int _tmp428;int _tmp429;struct
_tuple5 _tmp427=Cyc_Evexp_eval_const_uint_exp(_tmp2E1);_tmp428=_tmp427.f1;_tmp429=
_tmp427.f2;if(!_tmp429)({void*_tmp42A[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp42B="unknown tuple subscript in translation to C";
_tag_arr(_tmp42B,sizeof(char),_get_zero_arr_size(_tmp42B,44));}),_tag_arr(
_tmp42A,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
_tmp2E0,Cyc_Absyn_fieldname((int)(_tmp428 + 1)),0))->r));goto _LL2C8;}_LL2CB: if(
_tmp41E <= (void*)3?1:*((int*)_tmp41E)!= 4)goto _LL2CD;_tmp420=((struct Cyc_Absyn_PointerType_struct*)
_tmp41E)->f1;_tmp421=(void*)_tmp420.elt_typ;_tmp422=(void*)_tmp420.rgn_typ;
_tmp423=_tmp420.nullable;_tmp424=_tmp420.tq;_tmp425=_tmp420.bounds;_tmp426=
_tmp420.zero_term;_LL2CC: {struct Cyc_List_List*_tmp42C=Cyc_Toc_get_relns(_tmp2E0);
int _tmp42D=Cyc_Toc_check_bounds(_tmp42C,_tmp2E0,_tmp2E1);if(Cyc_Toc_warn_bounds_checks?
!_tmp42D: 0)({struct Cyc_Std_String_pa_struct _tmp430;_tmp430.tag=0;_tmp430.f1=(
struct _tagged_arr)Cyc_Absynpp_exp2string(e);{void*_tmp42E[1]={& _tmp430};Cyc_Tcutil_warn(
e->loc,({const char*_tmp42F="bounds check necessary for %s";_tag_arr(_tmp42F,
sizeof(char),_get_zero_arr_size(_tmp42F,30));}),_tag_arr(_tmp42E,sizeof(void*),1));}});
Cyc_Toc_exp_to_c(nv,_tmp2E0);Cyc_Toc_exp_to_c(nv,_tmp2E1);Cyc_Toc_total_bounds_checks
++;{void*_tmp431=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp425);struct Cyc_Absyn_Exp*
_tmp432;_LL2D0: if(_tmp431 <= (void*)1?1:*((int*)_tmp431)!= 0)goto _LL2D2;_tmp432=((
struct Cyc_Absyn_Upper_b_struct*)_tmp431)->f1;_LL2D1: {int possibly_null=((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp423);void*ta1=Cyc_Toc_typ_to_c(
_tmp421);void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp424);if(_tmp42D)Cyc_Toc_bounds_checks_eliminated
++;else{if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp426))(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp433[3];
_tmp433[2]=_tmp2E1;_tmp433[1]=_tmp432;_tmp433[0]=_tmp2E0;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp433,sizeof(struct Cyc_Absyn_Exp*),
3));}),0),0),0))->r));else{if(possibly_null)(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp434[4];_tmp434[3]=_tmp2E1;_tmp434[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp434[1]=_tmp432;_tmp434[0]=_tmp2E0;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp434,sizeof(struct Cyc_Absyn_Exp*),4));}),
0),0),0))->r));else{(void*)(_tmp2E1->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp435[2];_tmp435[1]=Cyc_Absyn_copy_exp(_tmp2E1);_tmp435[0]=
_tmp432;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp435,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}}}goto _LL2CF;}_LL2D2: if((
int)_tmp431 != 0)goto _LL2CF;_LL2D3: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp421);if(
_tmp42D){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(
Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp424),Cyc_Absyn_aggrmember_exp(
_tmp2E0,Cyc_Toc_curr_sp,0),0),_tmp2E1,0))->r));}else{(void*)(e->r=(void*)((void*)(
Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp424),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*_tmp436[3];_tmp436[2]=
_tmp2E1;_tmp436[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp436[0]=_tmp2E0;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp436,sizeof(struct Cyc_Absyn_Exp*),
3));}),0),0),0))->r));}goto _LL2CF;}_LL2CF:;}goto _LL2C8;}_LL2CD:;_LL2CE:({void*
_tmp437[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp438="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_arr(
_tmp438,sizeof(char),_get_zero_arr_size(_tmp438,49));}),_tag_arr(_tmp437,sizeof(
void*),0));});_LL2C8:;}goto _LL1D1;}_LL208: if(*((int*)_tmp2AC)!= 24)goto _LL20A;
_tmp2E2=((struct Cyc_Absyn_Tuple_e_struct*)_tmp2AC)->f1;_LL209: if(!nv->toplevel)(
void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,0,0,_tmp2E2))->r));else{struct
Cyc_List_List*_tmp439=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp2E2);struct _tagged_arr*
_tmp43A=Cyc_Toc_add_tuple_type(_tmp439);struct Cyc_List_List*dles=0;{int i=1;for(0;
_tmp2E2 != 0;(_tmp2E2=_tmp2E2->tl,i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp2E2->hd);dles=({struct Cyc_List_List*_tmp43B=_cycalloc(sizeof(*_tmp43B));
_tmp43B->hd=({struct _tuple6*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->f1=0;
_tmp43C->f2=(struct Cyc_Absyn_Exp*)_tmp2E2->hd;_tmp43C;});_tmp43B->tl=dles;
_tmp43B;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,dles,0))->r));}
goto _LL1D1;_LL20A: if(*((int*)_tmp2AC)!= 26)goto _LL20C;_tmp2E3=((struct Cyc_Absyn_Array_e_struct*)
_tmp2AC)->f1;_LL20B:(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,
_tmp2E3,0))->r));{struct Cyc_List_List*_tmp43D=_tmp2E3;for(0;_tmp43D != 0;_tmp43D=
_tmp43D->tl){struct _tuple6 _tmp43F;struct Cyc_Absyn_Exp*_tmp440;struct _tuple6*
_tmp43E=(struct _tuple6*)_tmp43D->hd;_tmp43F=*_tmp43E;_tmp440=_tmp43F.f2;Cyc_Toc_exp_to_c(
nv,_tmp440);}}goto _LL1D1;_LL20C: if(*((int*)_tmp2AC)!= 27)goto _LL20E;_tmp2E4=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp2AC)->f1;_tmp2E5=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2AC)->f2;_tmp2E6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2AC)->f3;
_tmp2E7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2AC)->f4;_LL20D: {
unsigned int _tmp442;int _tmp443;struct _tuple5 _tmp441=Cyc_Evexp_eval_const_uint_exp(
_tmp2E5);_tmp442=_tmp441.f1;_tmp443=_tmp441.f2;{void*_tmp444=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp2E6->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp2E6);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp2E6)){if(!_tmp443)({
void*_tmp445[0]={};Cyc_Tcutil_terr(_tmp2E5->loc,({const char*_tmp446="cannot determine value of constant";
_tag_arr(_tmp446,sizeof(char),_get_zero_arr_size(_tmp446,35));}),_tag_arr(
_tmp445,sizeof(void*),0));});{unsigned int i=0;for(0;i < _tmp442;i ++){es=({struct
Cyc_List_List*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->hd=({struct _tuple6*
_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448->f1=0;_tmp448->f2=_tmp2E6;_tmp448;});
_tmp447->tl=es;_tmp447;});}}if(_tmp2E7){struct Cyc_Absyn_Exp*_tmp449=Cyc_Absyn_cast_exp(
_tmp444,Cyc_Absyn_uint_exp(0,0),0);es=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp44A=
_cycalloc(sizeof(*_tmp44A));_tmp44A->hd=({struct _tuple6*_tmp44B=_cycalloc(
sizeof(*_tmp44B));_tmp44B->f1=0;_tmp44B->f2=_tmp449;_tmp44B;});_tmp44A->tl=0;
_tmp44A;}));}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,es,0))->r));
goto _LL1D1;}}}_LL20E: if(*((int*)_tmp2AC)!= 28)goto _LL210;_tmp2E8=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2AC)->f1;_tmp2E9=((struct Cyc_Absyn_Struct_e_struct*)_tmp2AC)->f2;_tmp2EA=((
struct Cyc_Absyn_Struct_e_struct*)_tmp2AC)->f3;_tmp2EB=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2AC)->f4;_LL20F: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp2E9 != 0,0,0,_tmp2EA,_tmp2E8))->r));else{if(_tmp2EB == 0)({void*
_tmp44C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp44D="Struct_e: missing aggrdecl pointer";_tag_arr(_tmp44D,sizeof(
char),_get_zero_arr_size(_tmp44D,35));}),_tag_arr(_tmp44C,sizeof(void*),0));});{
struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_check_null(_tmp2EB);
struct _RegionHandle _tmp44E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp44E;
_push_region(rgn);{struct Cyc_List_List*_tmp44F=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp2EA,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);struct Cyc_List_List*
_tmp450=0;struct Cyc_List_List*_tmp451=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp451 != 0;_tmp451=_tmp451->tl){struct Cyc_List_List*
_tmp452=_tmp44F;for(0;_tmp452 != 0;_tmp452=_tmp452->tl){if((*((struct _tuple11*)((
struct _tuple11*)_tmp452->hd))).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp451->hd){
struct _tuple11 _tmp454;struct Cyc_Absyn_Aggrfield*_tmp455;struct Cyc_Absyn_Exp*
_tmp456;struct _tuple11*_tmp453=(struct _tuple11*)_tmp452->hd;_tmp454=*_tmp453;
_tmp455=_tmp454.f1;_tmp456=_tmp454.f2;{void*_tmp457=(void*)_tmp455->type;Cyc_Toc_exp_to_c(
nv,_tmp456);if(Cyc_Toc_is_void_star(_tmp457))(void*)(_tmp456->r=(void*)((void*)(
Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp456->r,
0),0))->r));_tmp450=({struct Cyc_List_List*_tmp458=_cycalloc(sizeof(*_tmp458));
_tmp458->hd=({struct _tuple6*_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459->f1=0;
_tmp459->f2=_tmp456;_tmp459;});_tmp458->tl=_tmp450;_tmp458;});break;}}}}(void*)(
e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp450),0))->r));};_pop_region(rgn);}}
goto _LL1D1;_LL210: if(*((int*)_tmp2AC)!= 29)goto _LL212;_tmp2EC=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2AC)->f1;_tmp2ED=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2AC)->f2;_LL211: {
struct Cyc_List_List*fs;{void*_tmp45A=Cyc_Tcutil_compress(_tmp2EC);struct Cyc_List_List*
_tmp45B;_LL2D5: if(_tmp45A <= (void*)3?1:*((int*)_tmp45A)!= 11)goto _LL2D7;_tmp45B=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp45A)->f2;_LL2D6: fs=_tmp45B;goto _LL2D4;
_LL2D7:;_LL2D8:({struct Cyc_Std_String_pa_struct _tmp45E;_tmp45E.tag=0;_tmp45E.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp2EC);{void*_tmp45C[1]={& _tmp45E};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmp45D="anon struct has type %s";_tag_arr(_tmp45D,sizeof(char),
_get_zero_arr_size(_tmp45D,24));}),_tag_arr(_tmp45C,sizeof(void*),1));}});_LL2D4:;}{
struct _RegionHandle _tmp45F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp45F;
_push_region(rgn);{struct Cyc_List_List*_tmp460=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp2ED,
fs);for(0;_tmp460 != 0;_tmp460=_tmp460->tl){struct _tuple11 _tmp462;struct Cyc_Absyn_Aggrfield*
_tmp463;struct Cyc_Absyn_Exp*_tmp464;struct _tuple11*_tmp461=(struct _tuple11*)
_tmp460->hd;_tmp462=*_tmp461;_tmp463=_tmp462.f1;_tmp464=_tmp462.f2;{void*_tmp465=(
void*)_tmp463->type;Cyc_Toc_exp_to_c(nv,_tmp464);if(Cyc_Toc_is_void_star(_tmp465))(
void*)(_tmp464->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp464->r,0),0))->r));}}(void*)(e->r=(void*)((void*)(
Cyc_Absyn_unresolvedmem_exp(0,_tmp2ED,0))->r));};_pop_region(rgn);}goto _LL1D1;}
_LL212: if(*((int*)_tmp2AC)!= 30)goto _LL214;_tmp2EE=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp2AC)->f1;if(_tmp2EE != 0)goto _LL214;_tmp2EF=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp2AC)->f2;_tmp2F0=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2AC)->f3;_LL213: {
struct _tuple1*qv=_tmp2F0->name;struct Cyc_Absyn_Exp*tag_exp=_tmp2EF->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp2EF,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL1D1;}_LL214: if(*((int*)_tmp2AC)!= 30)goto _LL216;
_tmp2F1=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2AC)->f1;_tmp2F2=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp2AC)->f2;_tmp2F3=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2AC)->f3;_LL215: {
struct _tuple1*_tmp466=_tmp2F3->name;struct Cyc_List_List*_tmp467=_tmp2F3->typs;
struct _tuple1*_tmp468=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp469=Cyc_Absyn_var_exp(
_tmp468,0);void*_tmp46A=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp466,({
const char*_tmp478="_struct";_tag_arr(_tmp478,sizeof(char),_get_zero_arr_size(
_tmp478,8));})));struct Cyc_Absyn_Exp*tag_exp=_tmp2F2->is_xtunion?Cyc_Absyn_var_exp(
_tmp466,0): Cyc_Toc_tunion_tag(_tmp2F2,_tmp466,1);if(nv->toplevel){struct Cyc_List_List*
dles=0;for(0;_tmp2F1 != 0;(_tmp2F1=_tmp2F1->tl,_tmp467=_tmp467->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp2F1->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp467))->hd))).f2);
Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Absyn_cast_exp(
field_typ,cur_e,0);dles=({struct Cyc_List_List*_tmp46B=_cycalloc(sizeof(*_tmp46B));
_tmp46B->hd=({struct _tuple6*_tmp46C=_cycalloc(sizeof(*_tmp46C));_tmp46C->f1=0;
_tmp46C->f2=cur_e;_tmp46C;});_tmp46B->tl=dles;_tmp46B;});}dles=({struct Cyc_List_List*
_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D->hd=({struct _tuple6*_tmp46E=
_cycalloc(sizeof(*_tmp46E));_tmp46E->f1=0;_tmp46E->f2=tag_exp;_tmp46E;});_tmp46D->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp46D;});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(({struct Cyc_Core_Opt*
_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F->v=Cyc_Toc_collapse_qvar_tag(_tmp466,({
const char*_tmp470="_struct";_tag_arr(_tmp470,sizeof(char),_get_zero_arr_size(
_tmp470,8));}));_tmp46F;}),dles,0))->r));}else{struct Cyc_List_List*_tmp471=({
struct Cyc_List_List*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->hd=Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(_tmp469,Cyc_Toc_tag_sp,0),tag_exp,0);_tmp477->tl=0;
_tmp477;});{int i=1;for(0;_tmp2F1 != 0;(((_tmp2F1=_tmp2F1->tl,i ++)),_tmp467=
_tmp467->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp2F1->hd;void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp467))->hd))).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Absyn_cast_exp(field_typ,cur_e,0);{struct Cyc_Absyn_Stmt*
_tmp472=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp469,Cyc_Absyn_fieldname(
i),0),cur_e,0);_tmp471=({struct Cyc_List_List*_tmp473=_cycalloc(sizeof(*_tmp473));
_tmp473->hd=_tmp472;_tmp473->tl=_tmp471;_tmp473;});}}}{struct Cyc_Absyn_Stmt*
_tmp474=Cyc_Absyn_exp_stmt(_tmp469,0);struct Cyc_Absyn_Stmt*_tmp475=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp476=_cycalloc(sizeof(*_tmp476));_tmp476->hd=_tmp474;_tmp476->tl=_tmp471;
_tmp476;})),0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
_tmp468,_tmp46A,0,_tmp475,0),0))->r));}}goto _LL1D1;}_LL216: if(*((int*)_tmp2AC)!= 
31)goto _LL218;_LL217: goto _LL219;_LL218: if(*((int*)_tmp2AC)!= 32)goto _LL21A;
_LL219: goto _LL1D1;_LL21A: if(*((int*)_tmp2AC)!= 33)goto _LL21C;_tmp2F4=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp2AC)->f1;_tmp2F5=_tmp2F4.is_calloc;_tmp2F6=_tmp2F4.rgn;_tmp2F7=_tmp2F4.elt_type;
_tmp2F8=_tmp2F4.num_elts;_tmp2F9=_tmp2F4.fat_result;_LL21B: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp2F7)));Cyc_Toc_exp_to_c(nv,_tmp2F8);if(_tmp2F9){struct
_tuple1*_tmp479=Cyc_Toc_temp_var();struct _tuple1*_tmp47A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp2F5){xexp=_tmp2F8;if(_tmp2F6 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp2F6);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp479,0));}else{pexp=Cyc_Toc_calloc_exp(*((void**)
_tmp2F7),Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp479,0));}rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp47B[3];_tmp47B[2]=Cyc_Absyn_var_exp(
_tmp479,0);_tmp47B[1]=Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp47B[0]=Cyc_Absyn_var_exp(
_tmp47A,0);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp47B,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp2F8,0);if(_tmp2F6 != 0){struct Cyc_Absyn_Exp*
rgn=(struct Cyc_Absyn_Exp*)_check_null(_tmp2F6);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(
rgn,Cyc_Absyn_var_exp(_tmp479,0));}else{pexp=Cyc_Toc_malloc_exp(*((void**)
_tmp2F7),Cyc_Absyn_var_exp(_tmp479,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp47C[3];_tmp47C[2]=Cyc_Absyn_var_exp(_tmp479,0);_tmp47C[1]=
Cyc_Absyn_uint_exp(1,0);_tmp47C[0]=Cyc_Absyn_var_exp(_tmp47A,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp47C,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}{struct Cyc_Absyn_Stmt*_tmp47D=Cyc_Absyn_declare_stmt(_tmp479,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp47A,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp47D,0))->r));}}else{if(_tmp2F6 != 0){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_check_null(_tmp2F6);Cyc_Toc_exp_to_c(
nv,rgn);(void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*((void**)
_tmp2F7),Cyc_Absyn_sizeoftyp_exp(t_c,0)))->r));}}goto _LL1D1;}_LL21C: if(*((int*)
_tmp2AC)!= 35)goto _LL21E;_tmp2FA=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp2AC)->f1;
_LL21D: Cyc_Toc_stmt_to_c(nv,_tmp2FA);goto _LL1D1;_LL21E: if(*((int*)_tmp2AC)!= 34)
goto _LL220;_LL21F:({void*_tmp47E[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp47F="UnresolvedMem";_tag_arr(
_tmp47F,sizeof(char),_get_zero_arr_size(_tmp47F,14));}),_tag_arr(_tmp47E,sizeof(
void*),0));});_LL220: if(*((int*)_tmp2AC)!= 25)goto _LL222;_LL221:({void*_tmp480[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*
_tmp481="compoundlit";_tag_arr(_tmp481,sizeof(char),_get_zero_arr_size(_tmp481,
12));}),_tag_arr(_tmp480,sizeof(void*),0));});_LL222: if(*((int*)_tmp2AC)!= 36)
goto _LL224;_tmp2FB=((struct Cyc_Absyn_Codegen_e_struct*)_tmp2AC)->f1;_LL223:({
void*_tmp482[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp483="codegen";_tag_arr(_tmp483,sizeof(char),_get_zero_arr_size(
_tmp483,8));}),_tag_arr(_tmp482,sizeof(void*),0));});_LL224: if(*((int*)_tmp2AC)
!= 37)goto _LL1D1;_LL225:({void*_tmp484[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp485="fill";_tag_arr(_tmp485,
sizeof(char),_get_zero_arr_size(_tmp485,5));}),_tag_arr(_tmp484,sizeof(void*),0));});
_LL1D1:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);}struct _tuple12{
struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple13{struct _tuple1*f1;void*f2;};struct _tuple14{struct Cyc_List_List*f1;struct
Cyc_Absyn_Pat*f2;};static struct _tuple12 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,
struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp486=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp487;
struct Cyc_Absyn_Vardecl _tmp488;struct _tuple1*_tmp489;struct Cyc_Absyn_Vardecl*
_tmp48A;void*_tmp48B;int _tmp48C;char _tmp48D;struct _tagged_arr _tmp48E;struct Cyc_Absyn_Enumdecl*
_tmp48F;struct Cyc_Absyn_Enumfield*_tmp490;void*_tmp491;struct Cyc_Absyn_Enumfield*
_tmp492;struct Cyc_Absyn_Tuniondecl*_tmp493;struct Cyc_Absyn_Tunionfield*_tmp494;
struct Cyc_List_List*_tmp495;struct Cyc_Absyn_Pat*_tmp496;struct Cyc_Absyn_Pat
_tmp497;void*_tmp498;struct Cyc_Absyn_Tuniondecl*_tmp499;struct Cyc_Absyn_Tunionfield*
_tmp49A;struct Cyc_List_List*_tmp49B;struct Cyc_List_List*_tmp49C;struct Cyc_List_List*
_tmp49D;struct Cyc_Absyn_AggrInfo _tmp49E;void*_tmp49F;struct Cyc_List_List*_tmp4A0;
struct Cyc_Absyn_Pat*_tmp4A1;_LL2DA: if(_tmp486 <= (void*)2?1:*((int*)_tmp486)!= 0)
goto _LL2DC;_tmp487=((struct Cyc_Absyn_Var_p_struct*)_tmp486)->f1;_tmp488=*_tmp487;
_tmp489=_tmp488.name;_LL2DB: nv=Cyc_Toc_add_varmap(nv,_tmp489,r);goto _LL2DD;
_LL2DC: if((int)_tmp486 != 0)goto _LL2DE;_LL2DD: s=Cyc_Toc_skip_stmt_dl();goto _LL2D9;
_LL2DE: if(_tmp486 <= (void*)2?1:*((int*)_tmp486)!= 6)goto _LL2E0;_tmp48A=((struct
Cyc_Absyn_Reference_p_struct*)_tmp486)->f1;_LL2DF: {struct _tuple1*_tmp4A2=Cyc_Toc_temp_var();
decls=({struct Cyc_List_List*_tmp4A3=_region_malloc(rgn,sizeof(*_tmp4A3));_tmp4A3->hd=({
struct _tuple13*_tmp4A4=_region_malloc(rgn,sizeof(*_tmp4A4));_tmp4A4->f1=_tmp4A2;
_tmp4A4->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp4A4;});
_tmp4A3->tl=decls;_tmp4A3;});nv=Cyc_Toc_add_varmap(nv,_tmp48A->name,Cyc_Absyn_var_exp(
_tmp4A2,0));s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp4A2,0),Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(
path,0),0),0);goto _LL2D9;}_LL2E0: if((int)_tmp486 != 1)goto _LL2E2;_LL2E1: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL2D9;_LL2E2: if(_tmp486 <= (void*)
2?1:*((int*)_tmp486)!= 1)goto _LL2E4;_tmp48B=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp486)->f1;_tmp48C=((struct Cyc_Absyn_Int_p_struct*)_tmp486)->f2;_LL2E3: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_int_exp(_tmp48B,_tmp48C,0),fail_stmt);goto _LL2D9;_LL2E4: if(_tmp486 <= (
void*)2?1:*((int*)_tmp486)!= 2)goto _LL2E6;_tmp48D=((struct Cyc_Absyn_Char_p_struct*)
_tmp486)->f1;_LL2E5: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp48D,0),
fail_stmt);goto _LL2D9;_LL2E6: if(_tmp486 <= (void*)2?1:*((int*)_tmp486)!= 3)goto
_LL2E8;_tmp48E=((struct Cyc_Absyn_Float_p_struct*)_tmp486)->f1;_LL2E7: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_float_exp(_tmp48E,0),fail_stmt);goto _LL2D9;_LL2E8: if(_tmp486 <= (void*)
2?1:*((int*)_tmp486)!= 9)goto _LL2EA;_tmp48F=((struct Cyc_Absyn_Enum_p_struct*)
_tmp486)->f1;_tmp490=((struct Cyc_Absyn_Enum_p_struct*)_tmp486)->f2;_LL2E9: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp4A5=_cycalloc(
sizeof(*_tmp4A5));_tmp4A5[0]=({struct Cyc_Absyn_Enum_e_struct _tmp4A6;_tmp4A6.tag=
31;_tmp4A6.f1=_tmp490->name;_tmp4A6.f2=(struct Cyc_Absyn_Enumdecl*)_tmp48F;
_tmp4A6.f3=(struct Cyc_Absyn_Enumfield*)_tmp490;_tmp4A6;});_tmp4A5;}),0),
fail_stmt);goto _LL2D9;_LL2EA: if(_tmp486 <= (void*)2?1:*((int*)_tmp486)!= 10)goto
_LL2EC;_tmp491=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp486)->f1;_tmp492=((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp486)->f2;_LL2EB: s=Cyc_Toc_if_neq_stmt(r,
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp4A7=_cycalloc(
sizeof(*_tmp4A7));_tmp4A7[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp4A8;_tmp4A8.tag=
32;_tmp4A8.f1=_tmp492->name;_tmp4A8.f2=(void*)_tmp491;_tmp4A8.f3=(struct Cyc_Absyn_Enumfield*)
_tmp492;_tmp4A8;});_tmp4A7;}),0),fail_stmt);goto _LL2D9;_LL2EC: if(_tmp486 <= (void*)
2?1:*((int*)_tmp486)!= 8)goto _LL2EE;_tmp493=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp486)->f1;_tmp494=((struct Cyc_Absyn_Tunion_p_struct*)_tmp486)->f2;_tmp495=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp486)->f3;if(_tmp495 != 0)goto _LL2EE;_LL2ED: {
struct Cyc_Absyn_Exp*cmp_exp;if(_tmp493->is_xtunion)cmp_exp=Cyc_Absyn_var_exp(
_tmp494->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(_tmp493,_tmp494->name,0);r=Cyc_Absyn_cast_exp(
Cyc_Absyn_sint_typ,r,0);}s=Cyc_Toc_if_neq_stmt(r,cmp_exp,fail_stmt);goto _LL2D9;}
_LL2EE: if(_tmp486 <= (void*)2?1:*((int*)_tmp486)!= 5)goto _LL2F0;_tmp496=((struct
Cyc_Absyn_Pointer_p_struct*)_tmp486)->f1;_tmp497=*_tmp496;_tmp498=(void*)_tmp497.r;
if(_tmp498 <= (void*)2?1:*((int*)_tmp498)!= 8)goto _LL2F0;_tmp499=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp498)->f1;_tmp49A=((struct Cyc_Absyn_Tunion_p_struct*)_tmp498)->f2;_tmp49B=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp498)->f3;if(!(_tmp49B != 0))goto _LL2F0;
_LL2EF: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(
_tmp49A->name,({const char*_tmp4B5="_struct";_tag_arr(_tmp4B5,sizeof(char),
_get_zero_arr_size(_tmp4B5,8));}));struct Cyc_Absyn_Exp*rcast=Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r,0);struct Cyc_List_List*
_tmp4A9=_tmp49A->typs;for(0;_tmp49B != 0;(((_tmp49B=_tmp49B->tl,_tmp4A9=((struct
Cyc_List_List*)_check_null(_tmp4A9))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp4AA=(
struct Cyc_Absyn_Pat*)_tmp49B->hd;if((void*)_tmp4AA->r == (void*)0)continue;{void*
_tmp4AB=(*((struct _tuple4*)((struct _tuple4*)((struct Cyc_List_List*)_check_null(
_tmp4A9))->hd))).f2;struct _tuple1*_tmp4AC=Cyc_Toc_temp_var();void*_tmp4AD=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4AA->topt))->v;void*_tmp4AE=Cyc_Toc_typ_to_c(
_tmp4AD);struct Cyc_Absyn_Exp*_tmp4AF=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(
cnt),0);if(Cyc_Toc_is_void_star(Cyc_Toc_typ_to_c(_tmp4AB)))_tmp4AF=Cyc_Absyn_cast_exp(
_tmp4AE,_tmp4AF,0);decls=({struct Cyc_List_List*_tmp4B0=_region_malloc(rgn,
sizeof(*_tmp4B0));_tmp4B0->hd=({struct _tuple13*_tmp4B1=_region_malloc(rgn,
sizeof(*_tmp4B1));_tmp4B1->f1=_tmp4AC;_tmp4B1->f2=_tmp4AE;_tmp4B1;});_tmp4B0->tl=
decls;_tmp4B0;});{struct _tuple12 _tmp4B2=Cyc_Toc_xlate_pat(nv,rgn,_tmp4AD,Cyc_Absyn_var_exp(
_tmp4AC,0),_tmp4AF,_tmp4AA,fail_stmt,decls);nv=_tmp4B2.f1;decls=_tmp4B2.f2;{
struct Cyc_Absyn_Stmt*_tmp4B3=_tmp4B2.f3;struct Cyc_Absyn_Stmt*_tmp4B4=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp4AC,0),_tmp4AF,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp4B4,_tmp4B3,0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(_tmp499->is_xtunion){
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq),r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e2,0);struct Cyc_Absyn_Exp*
e=Cyc_Absyn_var_exp(_tmp49A->name,0);test_exp=Cyc_Absyn_neq_exp(e1,e,0);}else{
int max_tag=Cyc_Toc_num_void_tags(_tmp499);struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((
unsigned int)max_tag,0);struct Cyc_Absyn_Exp*e3=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(
e3,0);struct Cyc_Absyn_Exp*e5=Cyc_Absyn_lte_exp(r,Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
max_tag_exp,0),0);struct Cyc_Absyn_Exp*e=Cyc_Toc_tunion_tag(_tmp499,_tmp49A->name,
1);test_exp=max_tag > 0?Cyc_Absyn_or_exp(e5,Cyc_Absyn_neq_exp(e1,e,0),0): Cyc_Absyn_neq_exp(
e1,e,0);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);goto _LL2D9;}}_LL2F0: if(_tmp486 <= (void*)2?1:*((int*)_tmp486)!= 8)goto
_LL2F2;_tmp49C=((struct Cyc_Absyn_Tunion_p_struct*)_tmp486)->f3;_LL2F1: _tmp49D=
_tmp49C;goto _LL2F3;_LL2F2: if(_tmp486 <= (void*)2?1:*((int*)_tmp486)!= 4)goto
_LL2F4;_tmp49D=((struct Cyc_Absyn_Tuple_p_struct*)_tmp486)->f1;_LL2F3: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;for(0;_tmp49D != 0;(_tmp49D=_tmp49D->tl,++ cnt)){struct Cyc_Absyn_Pat*
_tmp4B6=(struct Cyc_Absyn_Pat*)_tmp49D->hd;if((void*)_tmp4B6->r == (void*)0)
continue;{struct _tuple1*_tmp4B7=Cyc_Toc_temp_var();void*_tmp4B8=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4B6->topt))->v;decls=({struct Cyc_List_List*_tmp4B9=
_region_malloc(rgn,sizeof(*_tmp4B9));_tmp4B9->hd=({struct _tuple13*_tmp4BA=
_region_malloc(rgn,sizeof(*_tmp4BA));_tmp4BA->f1=_tmp4B7;_tmp4BA->f2=Cyc_Toc_typ_to_c(
_tmp4B8);_tmp4BA;});_tmp4B9->tl=decls;_tmp4B9;});{struct _tuple12 _tmp4BB=Cyc_Toc_xlate_pat(
nv,rgn,_tmp4B8,Cyc_Absyn_var_exp(_tmp4B7,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp4B6,fail_stmt,decls);nv=_tmp4BB.f1;decls=_tmp4BB.f2;{struct Cyc_Absyn_Stmt*
_tmp4BC=_tmp4BB.f3;struct Cyc_Absyn_Stmt*_tmp4BD=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp4B7,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp4BD,_tmp4BC,0),0);}}}}goto _LL2D9;}_LL2F4: if(_tmp486 <= (
void*)2?1:*((int*)_tmp486)!= 7)goto _LL2F6;_tmp49E=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp486)->f1;_tmp49F=(void*)_tmp49E.aggr_info;_tmp4A0=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp486)->f3;_LL2F5: {struct Cyc_Absyn_Aggrdecl*_tmp4BE=Cyc_Absyn_get_known_aggrdecl(
_tmp49F);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp4A0 != 0;_tmp4A0=_tmp4A0->tl){struct
_tuple14*_tmp4BF=(struct _tuple14*)_tmp4A0->hd;struct Cyc_Absyn_Pat*_tmp4C0=(*((
struct _tuple14*)_tmp4BF)).f2;if((void*)_tmp4C0->r == (void*)0)continue;{struct
_tagged_arr*f;{void*_tmp4C1=(void*)((struct Cyc_List_List*)_check_null((*((struct
_tuple14*)_tmp4BF)).f1))->hd;struct _tagged_arr*_tmp4C2;_LL2FD: if(*((int*)_tmp4C1)
!= 1)goto _LL2FF;_tmp4C2=((struct Cyc_Absyn_FieldName_struct*)_tmp4C1)->f1;_LL2FE:
f=_tmp4C2;goto _LL2FC;_LL2FF:;_LL300:(int)_throw((void*)Cyc_Toc_Match_error);
_LL2FC:;}{struct _tuple1*_tmp4C3=Cyc_Toc_temp_var();void*_tmp4C4=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4C0->topt))->v;void*_tmp4C5=Cyc_Toc_typ_to_c(
_tmp4C4);decls=({struct Cyc_List_List*_tmp4C6=_region_malloc(rgn,sizeof(*_tmp4C6));
_tmp4C6->hd=({struct _tuple13*_tmp4C7=_region_malloc(rgn,sizeof(*_tmp4C7));
_tmp4C7->f1=_tmp4C3;_tmp4C7->f2=_tmp4C5;_tmp4C7;});_tmp4C6->tl=decls;_tmp4C6;});{
struct _tuple12 _tmp4C8=Cyc_Toc_xlate_pat(nv,rgn,_tmp4C4,Cyc_Absyn_var_exp(_tmp4C3,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp4C0,fail_stmt,decls);nv=_tmp4C8.f1;
decls=_tmp4C8.f2;{struct Cyc_Absyn_Exp*_tmp4C9=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star((void*)((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BE->impl))->fields,f)))->type))
_tmp4C9=Cyc_Absyn_cast_exp(_tmp4C5,_tmp4C9,0);{struct Cyc_Absyn_Stmt*_tmp4CA=
_tmp4C8.f3;struct Cyc_Absyn_Stmt*_tmp4CB=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp4C3,0),_tmp4C9,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp4CB,_tmp4CA,
0),0);}}}}}}goto _LL2D9;}_LL2F6: if(_tmp486 <= (void*)2?1:*((int*)_tmp486)!= 5)goto
_LL2F8;_tmp4A1=((struct Cyc_Absyn_Pointer_p_struct*)_tmp486)->f1;_LL2F7: {struct
_tuple1*_tmp4CC=Cyc_Toc_temp_var();void*_tmp4CD=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4A1->topt))->v;decls=({struct Cyc_List_List*_tmp4CE=
_region_malloc(rgn,sizeof(*_tmp4CE));_tmp4CE->hd=({struct _tuple13*_tmp4CF=
_region_malloc(rgn,sizeof(*_tmp4CF));_tmp4CF->f1=_tmp4CC;_tmp4CF->f2=Cyc_Toc_typ_to_c(
_tmp4CD);_tmp4CF;});_tmp4CE->tl=decls;_tmp4CE;});{struct _tuple12 _tmp4D0=Cyc_Toc_xlate_pat(
nv,rgn,_tmp4CD,Cyc_Absyn_var_exp(_tmp4CC,0),Cyc_Absyn_deref_exp(path,0),_tmp4A1,
fail_stmt,decls);nv=_tmp4D0.f1;decls=_tmp4D0.f2;{struct Cyc_Absyn_Stmt*_tmp4D1=
_tmp4D0.f3;struct Cyc_Absyn_Stmt*_tmp4D2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp4CC,0),Cyc_Absyn_deref_exp(r,0),0),_tmp4D1,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp4D2,0);else{s=_tmp4D2;}goto
_LL2D9;}}}_LL2F8: if(_tmp486 <= (void*)2?1:*((int*)_tmp486)!= 11)goto _LL2FA;_LL2F9:({
void*_tmp4D3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4D4="unknownid";_tag_arr(_tmp4D4,sizeof(char),_get_zero_arr_size(
_tmp4D4,10));}),_tag_arr(_tmp4D3,sizeof(void*),0));});_LL2FA: if(_tmp486 <= (void*)
2?1:*((int*)_tmp486)!= 12)goto _LL2D9;_LL2FB:({void*_tmp4D5[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp4D6="unknowncall";
_tag_arr(_tmp4D6,sizeof(char),_get_zero_arr_size(_tmp4D6,12));}),_tag_arr(
_tmp4D5,sizeof(void*),0));});_LL2D9:;}return({struct _tuple12 _tmp4D7;_tmp4D7.f1=
nv;_tmp4D7.f2=decls;_tmp4D7.f3=s;_tmp4D7;});}struct _tuple15{struct _tagged_arr*f1;
struct _tagged_arr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple15*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){return({struct _tuple15*
_tmp4D8=_region_malloc(r,sizeof(*_tmp4D8));_tmp4D8->f1=Cyc_Toc_fresh_label();
_tmp4D8->f2=Cyc_Toc_fresh_label();_tmp4D8->f3=sc;_tmp4D8;});}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp4D9=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp4DA=Cyc_Tcutil_compress(
_tmp4D9);_LL302: if(_tmp4DA <= (void*)3?1:*((int*)_tmp4DA)!= 5)goto _LL304;_LL303:
goto _LL305;_LL304: if(_tmp4DA <= (void*)3?1:*((int*)_tmp4DA)!= 12)goto _LL306;
_LL305: leave_as_switch=1;goto _LL301;_LL306:;_LL307: leave_as_switch=0;goto _LL301;
_LL301:;}{struct Cyc_List_List*_tmp4DB=scs;for(0;_tmp4DB != 0;_tmp4DB=_tmp4DB->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)
_tmp4DB->hd)->pat_vars))->v != 0?1:((struct Cyc_Absyn_Switch_clause*)_tmp4DB->hd)->where_clause
!= 0){leave_as_switch=0;break;}}}if(leave_as_switch){struct _tagged_arr*next_l=
Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp4DC=scs;for(0;_tmp4DC != 0;_tmp4DC=
_tmp4DC->tl){struct Cyc_Absyn_Stmt*_tmp4DD=((struct Cyc_Absyn_Switch_clause*)
_tmp4DC->hd)->body;((struct Cyc_Absyn_Switch_clause*)_tmp4DC->hd)->body=Cyc_Absyn_label_stmt(
next_l,_tmp4DD,0);next_l=Cyc_Toc_fresh_label();Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
nv,next_l),_tmp4DD);}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct
_tagged_arr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle _tmp4DE=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp4DE;_push_region(rgn);{struct Cyc_List_List*lscs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple15*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;{struct Cyc_List_List*
_tmp4DF=lscs;for(0;_tmp4DF != 0;_tmp4DF=_tmp4DF->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple15*)((struct _tuple15*)_tmp4DF->hd))).f3;struct _tagged_arr*
fail_lab=_tmp4DF->tl == 0?end_l:(*((struct _tuple15*)((struct _tuple15*)((struct Cyc_List_List*)
_check_null(_tmp4DF->tl))->hd))).f1;struct Cyc_Toc_Env*_tmp4E1;struct Cyc_List_List*
_tmp4E2;struct Cyc_Absyn_Stmt*_tmp4E3;struct _tuple12 _tmp4E0=Cyc_Toc_xlate_pat(nv,
rgn,_tmp4D9,r,path,sc->pattern,Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp4E1=
_tmp4E0.f1;_tmp4E2=_tmp4E0.f2;_tmp4E3=_tmp4E0.f3;if(sc->where_clause != 0){struct
Cyc_Absyn_Exp*_tmp4E4=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(
_tmp4E1,_tmp4E4);_tmp4E3=Cyc_Absyn_seq_stmt(_tmp4E3,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp4E4,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp4E2;nvs=({struct Cyc_List_List*_tmp4E5=_region_malloc(rgn,
sizeof(*_tmp4E5));_tmp4E5->hd=_tmp4E1;_tmp4E5->tl=nvs;_tmp4E5;});test_stmts=({
struct Cyc_List_List*_tmp4E6=_region_malloc(rgn,sizeof(*_tmp4E6));_tmp4E6->hd=
_tmp4E3;_tmp4E6->tl=test_stmts;_tmp4E6;});}}nvs=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple15 _tmp4E8;struct _tagged_arr*_tmp4E9;struct _tagged_arr*_tmp4EA;struct
Cyc_Absyn_Switch_clause*_tmp4EB;struct _tuple15*_tmp4E7=(struct _tuple15*)lscs->hd;
_tmp4E8=*_tmp4E7;_tmp4E9=_tmp4E8.f1;_tmp4EA=_tmp4E8.f2;_tmp4EB=_tmp4E8.f3;{
struct Cyc_Toc_Env*_tmp4EC=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp4EB->body;if(lscs->tl != 0){struct _tuple15
_tmp4EE;struct _tagged_arr*_tmp4EF;struct Cyc_Absyn_Switch_clause*_tmp4F0;struct
_tuple15*_tmp4ED=(struct _tuple15*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;
_tmp4EE=*_tmp4ED;_tmp4EF=_tmp4EE.f2;_tmp4F0=_tmp4EE.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
_tmp4EC,end_l,_tmp4EF,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp4F0->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp4EC,
end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp4E9,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp4EA,s,0),0);stmts=({struct Cyc_List_List*_tmp4F1=_region_malloc(rgn,sizeof(*
_tmp4F1));_tmp4F1->hd=s;_tmp4F1->tl=stmts;_tmp4F1;});}}{struct Cyc_Absyn_Stmt*res=
Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),
0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple13 _tmp4F3;struct _tuple1*
_tmp4F4;void*_tmp4F5;struct _tuple13*_tmp4F2=(struct _tuple13*)((struct Cyc_List_List*)
_check_null(decls))->hd;_tmp4F3=*_tmp4F2;_tmp4F4=_tmp4F3.f1;_tmp4F5=_tmp4F3.f2;
res=Cyc_Absyn_declare_stmt(_tmp4F4,_tmp4F5,0,res,0);}(void*)(whole_s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(
rgn);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*
init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));
_tmp4F6->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0);_tmp4F6->tl=0;_tmp4F6;}),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){if(n > 0)(void*)(s->r=(
void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s){while(1){void*_tmp4F7=(void*)s->r;struct Cyc_Absyn_Exp*_tmp4F8;
struct Cyc_Absyn_Stmt*_tmp4F9;struct Cyc_Absyn_Stmt*_tmp4FA;struct Cyc_Absyn_Exp*
_tmp4FB;struct Cyc_Absyn_Exp*_tmp4FC;struct Cyc_Absyn_Stmt*_tmp4FD;struct Cyc_Absyn_Stmt*
_tmp4FE;struct _tuple3 _tmp4FF;struct Cyc_Absyn_Exp*_tmp500;struct Cyc_Absyn_Stmt*
_tmp501;struct Cyc_Absyn_Stmt*_tmp502;struct Cyc_Absyn_Stmt*_tmp503;struct Cyc_Absyn_Stmt*
_tmp504;struct Cyc_Absyn_Exp*_tmp505;struct _tuple3 _tmp506;struct Cyc_Absyn_Exp*
_tmp507;struct _tuple3 _tmp508;struct Cyc_Absyn_Exp*_tmp509;struct Cyc_Absyn_Stmt*
_tmp50A;struct Cyc_Absyn_Exp*_tmp50B;struct Cyc_List_List*_tmp50C;struct Cyc_List_List*
_tmp50D;struct Cyc_Absyn_Switch_clause**_tmp50E;struct Cyc_Absyn_Decl*_tmp50F;
struct Cyc_Absyn_Stmt*_tmp510;struct _tagged_arr*_tmp511;struct Cyc_Absyn_Stmt*
_tmp512;struct Cyc_Absyn_Stmt*_tmp513;struct _tuple3 _tmp514;struct Cyc_Absyn_Exp*
_tmp515;struct Cyc_Absyn_Stmt*_tmp516;struct Cyc_List_List*_tmp517;struct Cyc_Absyn_Tvar*
_tmp518;struct Cyc_Absyn_Vardecl*_tmp519;int _tmp51A;struct Cyc_Absyn_Stmt*_tmp51B;
struct Cyc_Absyn_Exp*_tmp51C;_LL309: if((int)_tmp4F7 != 0)goto _LL30B;_LL30A: return;
_LL30B: if(_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 0)goto _LL30D;_tmp4F8=((struct
Cyc_Absyn_Exp_s_struct*)_tmp4F7)->f1;_LL30C: Cyc_Toc_exp_to_c(nv,_tmp4F8);return;
_LL30D: if(_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 1)goto _LL30F;_tmp4F9=((struct
Cyc_Absyn_Seq_s_struct*)_tmp4F7)->f1;_tmp4FA=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4F7)->f2;_LL30E: Cyc_Toc_stmt_to_c(nv,_tmp4F9);s=_tmp4FA;continue;_LL30F: if(
_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 2)goto _LL311;_tmp4FB=((struct Cyc_Absyn_Return_s_struct*)
_tmp4F7)->f1;_LL310: {struct Cyc_Core_Opt*topt=0;if(_tmp4FB != 0){topt=({struct Cyc_Core_Opt*
_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4FB->topt))->v);_tmp51D;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp4FB));}if(s->try_depth > 0){if(topt != 0){
struct _tuple1*_tmp51E=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp51F=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp51E,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp51E,(void*)topt->v,_tmp4FB,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp51F,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL311: if(_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 3)goto _LL313;_tmp4FC=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4F7)->f1;_tmp4FD=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4F7)->f2;_tmp4FE=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4F7)->f3;_LL312:
Cyc_Toc_exp_to_c(nv,_tmp4FC);Cyc_Toc_stmt_to_c(nv,_tmp4FD);s=_tmp4FE;continue;
_LL313: if(_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 4)goto _LL315;_tmp4FF=((struct
Cyc_Absyn_While_s_struct*)_tmp4F7)->f1;_tmp500=_tmp4FF.f1;_tmp501=((struct Cyc_Absyn_While_s_struct*)
_tmp4F7)->f2;_LL314: Cyc_Toc_exp_to_c(nv,_tmp500);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
nv),_tmp501);return;_LL315: if(_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 5)goto
_LL317;_tmp502=((struct Cyc_Absyn_Break_s_struct*)_tmp4F7)->f1;_LL316: if(nv->break_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->break_lab))->v,0))->r));{int dest_depth=
_tmp502 == 0?0: _tmp502->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,
s);return;}_LL317: if(_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 6)goto _LL319;
_tmp503=((struct Cyc_Absyn_Continue_s_struct*)_tmp4F7)->f1;_LL318: if(nv->continue_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->continue_lab))->v,0))->r));_tmp504=_tmp503;
goto _LL31A;_LL319: if(_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 7)goto _LL31B;
_tmp504=((struct Cyc_Absyn_Goto_s_struct*)_tmp4F7)->f2;_LL31A: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp504))->try_depth,s);
return;_LL31B: if(_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 8)goto _LL31D;_tmp505=((
struct Cyc_Absyn_For_s_struct*)_tmp4F7)->f1;_tmp506=((struct Cyc_Absyn_For_s_struct*)
_tmp4F7)->f2;_tmp507=_tmp506.f1;_tmp508=((struct Cyc_Absyn_For_s_struct*)_tmp4F7)->f3;
_tmp509=_tmp508.f1;_tmp50A=((struct Cyc_Absyn_For_s_struct*)_tmp4F7)->f4;_LL31C:
Cyc_Toc_exp_to_c(nv,_tmp505);Cyc_Toc_exp_to_c(nv,_tmp507);Cyc_Toc_exp_to_c(nv,
_tmp509);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp50A);return;_LL31D: if(
_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 9)goto _LL31F;_tmp50B=((struct Cyc_Absyn_Switch_s_struct*)
_tmp4F7)->f1;_tmp50C=((struct Cyc_Absyn_Switch_s_struct*)_tmp4F7)->f2;_LL31E: Cyc_Toc_xlate_switch(
nv,s,_tmp50B,_tmp50C);return;_LL31F: if(_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 
11)goto _LL321;_tmp50D=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp4F7)->f1;_tmp50E=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp4F7)->f2;_LL320: if(nv->fallthru_info == 0)({
void*_tmp520[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp521="fallthru in unexpected place";_tag_arr(_tmp521,sizeof(char),
_get_zero_arr_size(_tmp521,29));}),_tag_arr(_tmp520,sizeof(void*),0));});{struct
_tuple8 _tmp523;struct _tagged_arr*_tmp524;struct Cyc_List_List*_tmp525;struct Cyc_Dict_Dict*
_tmp526;struct _tuple8*_tmp522=(struct _tuple8*)((struct Cyc_Core_Opt*)_check_null(
nv->fallthru_info))->v;_tmp523=*_tmp522;_tmp524=_tmp523.f1;_tmp525=_tmp523.f2;
_tmp526=_tmp523.f3;{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp524,0);Cyc_Toc_do_npop_before(
s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp50E)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp527=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp525);struct Cyc_List_List*_tmp528=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp50D);for(0;_tmp527 != 0;(_tmp527=_tmp527->tl,
_tmp528=_tmp528->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp528))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup)(
_tmp526,(struct _tuple1*)_tmp527->hd),(struct Cyc_Absyn_Exp*)_tmp528->hd,0),s2,0);}(
void*)(s->r=(void*)((void*)s2->r));return;}}}_LL321: if(_tmp4F7 <= (void*)1?1:*((
int*)_tmp4F7)!= 12)goto _LL323;_tmp50F=((struct Cyc_Absyn_Decl_s_struct*)_tmp4F7)->f1;
_tmp510=((struct Cyc_Absyn_Decl_s_struct*)_tmp4F7)->f2;_LL322:{void*_tmp529=(void*)
_tmp50F->r;struct Cyc_Absyn_Vardecl*_tmp52A;struct Cyc_Absyn_Pat*_tmp52B;struct Cyc_Absyn_Exp*
_tmp52C;struct Cyc_List_List*_tmp52D;_LL336: if(*((int*)_tmp529)!= 0)goto _LL338;
_tmp52A=((struct Cyc_Absyn_Var_d_struct*)_tmp529)->f1;_LL337: {struct Cyc_Toc_Env*
_tmp52E=Cyc_Toc_add_varmap(nv,_tmp52A->name,Cyc_Absyn_varb_exp(_tmp52A->name,(
void*)({struct Cyc_Absyn_Local_b_struct*_tmp52F=_cycalloc(sizeof(*_tmp52F));
_tmp52F[0]=({struct Cyc_Absyn_Local_b_struct _tmp530;_tmp530.tag=3;_tmp530.f1=
_tmp52A;_tmp530;});_tmp52F;}),0));Cyc_Toc_local_decl_to_c(_tmp52E,_tmp52E,
_tmp52A,_tmp510);goto _LL335;}_LL338: if(*((int*)_tmp529)!= 2)goto _LL33A;_tmp52B=((
struct Cyc_Absyn_Let_d_struct*)_tmp529)->f1;_tmp52C=((struct Cyc_Absyn_Let_d_struct*)
_tmp529)->f3;_LL339:{void*_tmp531=(void*)_tmp52B->r;struct Cyc_Absyn_Vardecl*
_tmp532;_LL33F: if(_tmp531 <= (void*)2?1:*((int*)_tmp531)!= 0)goto _LL341;_tmp532=((
struct Cyc_Absyn_Var_p_struct*)_tmp531)->f1;_LL340: {struct _tuple1*old_name=
_tmp532->name;struct _tuple1*new_name=Cyc_Toc_temp_var();_tmp532->name=new_name;
_tmp532->initializer=(struct Cyc_Absyn_Exp*)_tmp52C;(void*)(_tmp50F->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp533=_cycalloc(sizeof(*_tmp533));_tmp533[
0]=({struct Cyc_Absyn_Var_d_struct _tmp534;_tmp534.tag=0;_tmp534.f1=_tmp532;
_tmp534;});_tmp533;})));{struct Cyc_Toc_Env*_tmp535=Cyc_Toc_add_varmap(nv,
old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp536=_cycalloc(sizeof(*_tmp536));_tmp536[0]=({struct Cyc_Absyn_Local_b_struct
_tmp537;_tmp537.tag=3;_tmp537.f1=_tmp532;_tmp537;});_tmp536;}),0));Cyc_Toc_local_decl_to_c(
_tmp535,nv,_tmp532,_tmp510);goto _LL33E;}}_LL341:;_LL342:(void*)(s->r=(void*)((
void*)(Cyc_Toc_letdecl_to_c(nv,_tmp52B,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp52C->topt))->v,_tmp52C,_tmp510))->r));goto _LL33E;_LL33E:;}goto _LL335;_LL33A:
if(*((int*)_tmp529)!= 3)goto _LL33C;_tmp52D=((struct Cyc_Absyn_Letv_d_struct*)
_tmp529)->f1;_LL33B: {struct Cyc_List_List*_tmp538=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp52D);if(_tmp538 == 0)({void*_tmp539[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp53A="empty Letv_d";_tag_arr(_tmp53A,sizeof(char),_get_zero_arr_size(_tmp53A,
13));}),_tag_arr(_tmp539,sizeof(void*),0));});(void*)(_tmp50F->r=(void*)((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp53B=_cycalloc(sizeof(*_tmp53B));_tmp53B[0]=({
struct Cyc_Absyn_Var_d_struct _tmp53C;_tmp53C.tag=0;_tmp53C.f1=(struct Cyc_Absyn_Vardecl*)
_tmp538->hd;_tmp53C;});_tmp53B;})));_tmp538=_tmp538->tl;for(0;_tmp538 != 0;
_tmp538=_tmp538->tl){struct Cyc_Absyn_Decl*_tmp53D=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp53E=_cycalloc(sizeof(*_tmp53E));_tmp53E[0]=({
struct Cyc_Absyn_Var_d_struct _tmp53F;_tmp53F.tag=0;_tmp53F.f1=(struct Cyc_Absyn_Vardecl*)
_tmp538->hd;_tmp53F;});_tmp53E;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp53D,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto
_LL335;}_LL33C:;_LL33D:({void*_tmp540[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp541="bad nested declaration within function";
_tag_arr(_tmp541,sizeof(char),_get_zero_arr_size(_tmp541,39));}),_tag_arr(
_tmp540,sizeof(void*),0));});_LL335:;}return;_LL323: if(_tmp4F7 <= (void*)1?1:*((
int*)_tmp4F7)!= 15)goto _LL325;_tmp511=((struct Cyc_Absyn_Label_s_struct*)_tmp4F7)->f1;
_tmp512=((struct Cyc_Absyn_Label_s_struct*)_tmp4F7)->f2;_LL324: s=_tmp512;
continue;_LL325: if(_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 16)goto _LL327;_tmp513=((
struct Cyc_Absyn_Do_s_struct*)_tmp4F7)->f1;_tmp514=((struct Cyc_Absyn_Do_s_struct*)
_tmp4F7)->f2;_tmp515=_tmp514.f1;_LL326: Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),
_tmp513);Cyc_Toc_exp_to_c(nv,_tmp515);return;_LL327: if(_tmp4F7 <= (void*)1?1:*((
int*)_tmp4F7)!= 17)goto _LL329;_tmp516=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp4F7)->f1;_tmp517=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4F7)->f2;_LL328: {
struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=({struct Cyc_Core_Opt*
_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542->v=(void*)e_typ;_tmp542;});nv=Cyc_Toc_add_varmap(
nv,e_var,e_exp);Cyc_Toc_stmt_to_c(nv,_tmp516);{struct Cyc_Absyn_Stmt*_tmp543=Cyc_Absyn_seq_stmt(
_tmp516,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
struct _tuple1*_tmp544=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp545=Cyc_Absyn_var_exp(
_tmp544,0);struct Cyc_Absyn_Vardecl*_tmp546=Cyc_Absyn_new_vardecl(_tmp544,Cyc_Absyn_exn_typ,
0);_tmp545->topt=({struct Cyc_Core_Opt*_tmp547=_cycalloc(sizeof(*_tmp547));
_tmp547->v=(void*)Cyc_Absyn_exn_typ;_tmp547;});{struct Cyc_Absyn_Pat*_tmp548=({
struct Cyc_Absyn_Pat*_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558->r=(void*)((void*)({
struct Cyc_Absyn_Var_p_struct*_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A[0]=({
struct Cyc_Absyn_Var_p_struct _tmp55B;_tmp55B.tag=0;_tmp55B.f1=_tmp546;_tmp55B;});
_tmp55A;}));_tmp558->topt=({struct Cyc_Core_Opt*_tmp559=_cycalloc(sizeof(*_tmp559));
_tmp559->v=(void*)Cyc_Absyn_exn_typ;_tmp559;});_tmp558->loc=0;_tmp558;});struct
Cyc_Absyn_Exp*_tmp549=Cyc_Absyn_throw_exp(_tmp545,0);_tmp549->topt=({struct Cyc_Core_Opt*
_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->v=(void*)((void*)0);_tmp54A;});{
struct Cyc_Absyn_Stmt*_tmp54B=Cyc_Absyn_exp_stmt(_tmp549,0);struct Cyc_Absyn_Switch_clause*
_tmp54C=({struct Cyc_Absyn_Switch_clause*_tmp555=_cycalloc(sizeof(*_tmp555));
_tmp555->pattern=_tmp548;_tmp555->pat_vars=({struct Cyc_Core_Opt*_tmp556=
_cycalloc(sizeof(*_tmp556));_tmp556->v=({struct Cyc_List_List*_tmp557=_cycalloc(
sizeof(*_tmp557));_tmp557->hd=_tmp546;_tmp557->tl=0;_tmp557;});_tmp556;});
_tmp555->where_clause=0;_tmp555->body=_tmp54B;_tmp555->loc=0;_tmp555;});struct
Cyc_Absyn_Stmt*_tmp54D=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp517,({struct
Cyc_List_List*_tmp554=_cycalloc(sizeof(*_tmp554));_tmp554->hd=_tmp54C;_tmp554->tl=
0;_tmp554;})),0);Cyc_Toc_stmt_to_c(nv,_tmp54D);{struct Cyc_Absyn_Exp*_tmp54E=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp553=_cycalloc(sizeof(*_tmp553));
_tmp553->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp553->tl=0;
_tmp553;}),0);struct Cyc_Absyn_Stmt*_tmp54F=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp552=_cycalloc(sizeof(*_tmp552));
_tmp552->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp552->tl=0;_tmp552;}),0),0);struct
Cyc_Absyn_Exp*_tmp550=Cyc_Absyn_int_exp((void*)0,0,0);struct Cyc_Absyn_Exp*
_tmp551=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp54F,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp550,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp54E,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp551,0),Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),
_tmp543,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_cast_exp(
e_typ,Cyc_Toc__exn_thrown_e,0),_tmp54D,0),0),0),0),0),0))->r));return;}}}}}
_LL329: if(_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 18)goto _LL32B;_tmp518=((struct
Cyc_Absyn_Region_s_struct*)_tmp4F7)->f1;_tmp519=((struct Cyc_Absyn_Region_s_struct*)
_tmp4F7)->f2;_tmp51A=((struct Cyc_Absyn_Region_s_struct*)_tmp4F7)->f3;_tmp51B=((
struct Cyc_Absyn_Region_s_struct*)_tmp4F7)->f4;_LL32A: {void*rh_struct_typ=Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp);void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(
rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*rh_var=Cyc_Toc_temp_var();struct
_tuple1*x_var=_tmp519->name;struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,
0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
nv,x_var,x_exp),_tmp51B);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp55C->tl=0;_tmp55C;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp55D[1];_tmp55D[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp55D,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),Cyc_Absyn_seq_stmt(_tmp51B,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({
struct Cyc_Absyn_Exp*_tmp55E[1];_tmp55E[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp55E,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),0),0),0),0))->r));return;}_LL32B: if(_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)
!= 20)goto _LL32D;_tmp51C=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp4F7)->f1;
_LL32C: Cyc_Toc_exp_to_c(nv,_tmp51C);(void*)(s->r=(void*)((void*)(Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp55F=
_cycalloc(sizeof(*_tmp55F));_tmp55F->hd=_tmp51C;_tmp55F->tl=0;_tmp55F;}),0),0))->r));
return;_LL32D: if(_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 10)goto _LL32F;_LL32E:({
void*_tmp560[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp561="switchC_s";_tag_arr(_tmp561,sizeof(char),_get_zero_arr_size(
_tmp561,10));}),_tag_arr(_tmp560,sizeof(void*),0));});_LL32F: if(_tmp4F7 <= (void*)
1?1:*((int*)_tmp4F7)!= 13)goto _LL331;_LL330:({void*_tmp562[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp563="cut";
_tag_arr(_tmp563,sizeof(char),_get_zero_arr_size(_tmp563,4));}),_tag_arr(_tmp562,
sizeof(void*),0));});_LL331: if(_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 14)goto
_LL333;_LL332:({void*_tmp564[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp565="splice";_tag_arr(_tmp565,
sizeof(char),_get_zero_arr_size(_tmp565,7));}),_tag_arr(_tmp564,sizeof(void*),0));});
_LL333: if(_tmp4F7 <= (void*)1?1:*((int*)_tmp4F7)!= 19)goto _LL308;_LL334:({void*
_tmp566[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp567="forarray";_tag_arr(_tmp567,sizeof(char),_get_zero_arr_size(
_tmp567,9));}),_tag_arr(_tmp566,sizeof(void*),0));});_LL308:;}}struct _tuple16{
struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f){f->tvs=0;f->effect=0;f->rgn_po=0;(
void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((void*)f->ret_type));{struct Cyc_List_List*
_tmp568=f->args;for(0;_tmp568 != 0;_tmp568=_tmp568->tl){(*((struct _tuple16*)((
struct _tuple16*)_tmp568->hd))).f3=Cyc_Toc_typ_to_c((*((struct _tuple16*)((struct
_tuple16*)_tmp568->hd))).f3);{struct _tuple1*_tmp569=({struct _tuple1*_tmp56A=
_cycalloc(sizeof(*_tmp56A));_tmp56A->f1=(void*)0;_tmp56A->f2=(*((struct _tuple16*)((
struct _tuple16*)_tmp568->hd))).f1;_tmp56A;});nv=Cyc_Toc_add_varmap(nv,_tmp569,
Cyc_Absyn_var_exp(_tmp569,0));}}}if((unsigned int)f->cyc_varargs?((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0: 0){struct Cyc_Core_Opt*_tmp56C;struct Cyc_Absyn_Tqual
_tmp56D;void*_tmp56E;int _tmp56F;struct Cyc_Absyn_VarargInfo _tmp56B=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp56C=_tmp56B.name;_tmp56D=_tmp56B.tq;_tmp56E=(
void*)_tmp56B.type;_tmp56F=_tmp56B.inject;{void*_tmp570=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ(
_tmp56E,(void*)2,_tmp56D,Cyc_Absyn_false_conref));struct _tuple1*_tmp571=({struct
_tuple1*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->f1=(void*)0;_tmp574->f2=(
struct _tagged_arr*)((struct Cyc_Core_Opt*)_check_null(_tmp56C))->v;_tmp574;});f->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
f->args,({struct Cyc_List_List*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572->hd=({
struct _tuple16*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->f1=(struct
_tagged_arr*)_tmp56C->v;_tmp573->f2=_tmp56D;_tmp573->f3=_tmp570;_tmp573;});
_tmp572->tl=0;_tmp572;}));nv=Cyc_Toc_add_varmap(nv,_tmp571,Cyc_Absyn_var_exp(
_tmp571,0));f->cyc_varargs=0;}}{struct Cyc_List_List*_tmp575=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp575 != 0;_tmp575=
_tmp575->tl){(void*)(((struct Cyc_Absyn_Vardecl*)_tmp575->hd)->type=(void*)Cyc_Toc_typ_to_c((
void*)((struct Cyc_Absyn_Vardecl*)_tmp575->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(
nv),f->body);}static void*Cyc_Toc_scope_to_c(void*s){void*_tmp576=s;_LL344: if((
int)_tmp576 != 1)goto _LL346;_LL345: return(void*)2;_LL346: if((int)_tmp576 != 4)goto
_LL348;_LL347: return(void*)3;_LL348:;_LL349: return s;_LL343:;}struct Cyc_Core_Opt*
Cyc_Toc_aggrs_so_far=0;static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad){struct _tuple1*_tmp577=ad->name;if(Cyc_Toc_aggrs_so_far == 0)Cyc_Toc_aggrs_so_far=({
struct Cyc_Core_Opt*_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp578;});{int seen_defn_before;struct Cyc_Core_Opt*_tmp579=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp577);if(_tmp579 == 
0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(
Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp577,ad));}else{if(((
struct Cyc_Absyn_Aggrdecl*)_tmp579->v)->impl == 0){(struct Cyc_Dict_Dict*)(((struct
Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,
_tmp577,ad));seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(ad->sc=(void*)((
void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po=
0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*_tmp57A=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp57A != 0;_tmp57A=_tmp57A->tl){(void*)(((
struct Cyc_Absyn_Aggrfield*)_tmp57A->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp57A->hd)->type));}}}}}static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far=0;static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
tud){if(Cyc_Toc_tunions_so_far == 0)Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*
_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->v=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);_tmp57B;});{
struct _tuple1*_tmp57C=tud->name;if(tud->fields == 0?1:((int(*)(struct Cyc_Set_Set*
s,struct _tuple1*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_tunions_so_far))->v,_tmp57C))return;(struct Cyc_Set_Set*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(void*)((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_insert)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp57C));{struct Cyc_List_List*
_tmp57D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;
for(0;_tmp57D != 0;_tmp57D=_tmp57D->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp57D->hd;if(f->typs != 0){struct Cyc_List_List*_tmp57E=0;int i=1;{struct Cyc_List_List*
_tmp57F=f->typs;for(0;_tmp57F != 0;(_tmp57F=_tmp57F->tl,i ++)){struct _tagged_arr*
_tmp580=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*_tmp581=({struct Cyc_Absyn_Aggrfield*
_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->name=_tmp580;_tmp583->tq=(*((struct
_tuple4*)((struct _tuple4*)_tmp57F->hd))).f1;_tmp583->type=(void*)Cyc_Toc_typ_to_c_array((*((
struct _tuple4*)((struct _tuple4*)_tmp57F->hd))).f2);_tmp583->width=0;_tmp583->attributes=
0;_tmp583;});_tmp57E=({struct Cyc_List_List*_tmp582=_cycalloc(sizeof(*_tmp582));
_tmp582->hd=_tmp581;_tmp582->tl=_tmp57E;_tmp582;});}}_tmp57E=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp57E);_tmp57E=({struct Cyc_List_List*
_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->hd=({struct Cyc_Absyn_Aggrfield*
_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->name=Cyc_Toc_tag_sp;_tmp585->tq=Cyc_Toc_mt_tq;
_tmp585->type=(void*)Cyc_Absyn_sint_typ;_tmp585->width=0;_tmp585->attributes=0;
_tmp585;});_tmp584->tl=_tmp57E;_tmp584;});{struct Cyc_Absyn_Aggrdecl*_tmp586=({
struct Cyc_Absyn_Aggrdecl*_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A->kind=(void*)((
void*)0);_tmp58A->sc=(void*)((void*)2);_tmp58A->name=Cyc_Toc_collapse_qvar_tag(f->name,({
const char*_tmp58C="_struct";_tag_arr(_tmp58C,sizeof(char),_get_zero_arr_size(
_tmp58C,8));}));_tmp58A->tvs=0;_tmp58A->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->exist_vars=0;_tmp58B->rgn_po=0;
_tmp58B->fields=_tmp57E;_tmp58B;});_tmp58A->attributes=0;_tmp58A;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp588=_cycalloc(sizeof(*_tmp588));
_tmp588[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp589;_tmp589.tag=4;_tmp589.f1=
_tmp586;_tmp589;});_tmp588;}),0);_tmp587->tl=Cyc_Toc_result_decls;_tmp587;});}}}}}}
static struct Cyc_Core_Opt*Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far == 0)Cyc_Toc_xtunion_fields_so_far=({
struct Cyc_Core_Opt*_tmp58D=_cycalloc(sizeof(*_tmp58D));_tmp58D->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp58D;});if(xd->fields == 0)return;{struct _tuple1*_tmp58E=xd->name;struct Cyc_List_List*
_tmp58F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp58F != 0;_tmp58F=_tmp58F->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp58F->hd;struct _tagged_arr*fn=(*((struct _tuple1*)f->name)).f2;struct Cyc_Absyn_Exp*
_tmp590=Cyc_Absyn_uint_exp(_get_arr_size(*((struct _tagged_arr*)fn),sizeof(char))
+ 4,0);void*_tmp591=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct
Cyc_Absyn_Exp*)_tmp590,Cyc_Absyn_false_conref);struct Cyc_Core_Opt*_tmp592=((
struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name);struct Cyc_Core_Opt _tmp593;int _tmp594;_LL34B: if(_tmp592 != 0)goto _LL34D;
_LL34C: {struct Cyc_Absyn_Exp*initopt=0;if((void*)f->sc != (void*)3){char zero='\000';
initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((struct _tagged_arr)({struct
Cyc_Std_String_pa_struct _tmp59B;_tmp59B.tag=0;_tmp59B.f1=(struct _tagged_arr)*((
struct _tagged_arr*)fn);{struct Cyc_Std_Int_pa_struct _tmp59A;_tmp59A.tag=1;_tmp59A.f1=(
int)((unsigned int)((int)zero));{struct Cyc_Std_Int_pa_struct _tmp599;_tmp599.tag=
1;_tmp599.f1=(int)((unsigned int)((int)zero));{struct Cyc_Std_Int_pa_struct
_tmp598;_tmp598.tag=1;_tmp598.f1=(int)((unsigned int)((int)zero));{struct Cyc_Std_Int_pa_struct
_tmp597;_tmp597.tag=1;_tmp597.f1=(int)((unsigned int)((int)zero));{void*_tmp595[
5]={& _tmp597,& _tmp598,& _tmp599,& _tmp59A,& _tmp59B};Cyc_Std_aprintf(({const char*
_tmp596="%c%c%c%c%s";_tag_arr(_tmp596,sizeof(char),_get_zero_arr_size(_tmp596,11));}),
_tag_arr(_tmp595,sizeof(void*),5));}}}}}}),0);}{struct Cyc_Absyn_Vardecl*_tmp59C=
Cyc_Absyn_new_vardecl(f->name,_tmp591,initopt);(void*)(_tmp59C->sc=(void*)((void*)
f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp59D=_cycalloc(sizeof(*
_tmp59D));_tmp59D->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp59E=_cycalloc(sizeof(*_tmp59E));_tmp59E[0]=({struct Cyc_Absyn_Var_d_struct
_tmp59F;_tmp59F.tag=0;_tmp59F.f1=_tmp59C;_tmp59F;});_tmp59E;}),0);_tmp59D->tl=
Cyc_Toc_result_decls;_tmp59D;});(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,f->name,(void*)
f->sc != (void*)3));if(f->typs != 0){struct Cyc_List_List*fields=0;int i=1;{struct
Cyc_List_List*_tmp5A0=f->typs;for(0;_tmp5A0 != 0;(_tmp5A0=_tmp5A0->tl,i ++)){
struct _tagged_arr*_tmp5A1=({struct _tagged_arr*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));
_tmp5A5[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp5A8;_tmp5A8.tag=1;
_tmp5A8.f1=(int)((unsigned int)i);{void*_tmp5A6[1]={& _tmp5A8};Cyc_Std_aprintf(({
const char*_tmp5A7="f%d";_tag_arr(_tmp5A7,sizeof(char),_get_zero_arr_size(_tmp5A7,
4));}),_tag_arr(_tmp5A6,sizeof(void*),1));}});_tmp5A5;});struct Cyc_Absyn_Aggrfield*
_tmp5A2=({struct Cyc_Absyn_Aggrfield*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4->name=
_tmp5A1;_tmp5A4->tq=(*((struct _tuple4*)((struct _tuple4*)_tmp5A0->hd))).f1;
_tmp5A4->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)((struct _tuple4*)
_tmp5A0->hd))).f2);_tmp5A4->width=0;_tmp5A4->attributes=0;_tmp5A4;});fields=({
struct Cyc_List_List*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->hd=_tmp5A2;
_tmp5A3->tl=fields;_tmp5A3;});}}fields=({struct Cyc_List_List*_tmp5A9=_cycalloc(
sizeof(*_tmp5A9));_tmp5A9->hd=({struct Cyc_Absyn_Aggrfield*_tmp5AA=_cycalloc(
sizeof(*_tmp5AA));_tmp5AA->name=Cyc_Toc_tag_sp;_tmp5AA->tq=Cyc_Toc_mt_tq;_tmp5AA->type=(
void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp5AA->width=0;
_tmp5AA->attributes=0;_tmp5AA;});_tmp5A9->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);_tmp5A9;});{struct Cyc_Absyn_Aggrdecl*_tmp5AB=({
struct Cyc_Absyn_Aggrdecl*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->kind=(void*)((
void*)0);_tmp5AF->sc=(void*)((void*)2);_tmp5AF->name=Cyc_Toc_collapse_qvar_tag(f->name,({
const char*_tmp5B1="_struct";_tag_arr(_tmp5B1,sizeof(char),_get_zero_arr_size(
_tmp5B1,8));}));_tmp5AF->tvs=0;_tmp5AF->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->exist_vars=0;_tmp5B0->rgn_po=0;
_tmp5B0->fields=fields;_tmp5B0;});_tmp5AF->attributes=0;_tmp5AF;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));_tmp5AC->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));
_tmp5AD[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp5AE;_tmp5AE.tag=4;_tmp5AE.f1=
_tmp5AB;_tmp5AE;});_tmp5AD;}),0);_tmp5AC->tl=Cyc_Toc_result_decls;_tmp5AC;});}}
goto _LL34A;}}_LL34D: if(_tmp592 == 0)goto _LL34F;_tmp593=*_tmp592;_tmp594=(int)
_tmp593.v;if(_tmp594 != 0)goto _LL34F;_LL34E: if((void*)f->sc != (void*)3){char zero='\000';
struct Cyc_Absyn_Exp*_tmp5B2=Cyc_Absyn_string_exp((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp5BD;_tmp5BD.tag=0;_tmp5BD.f1=(struct _tagged_arr)*((struct _tagged_arr*)fn);{
struct Cyc_Std_Int_pa_struct _tmp5BC;_tmp5BC.tag=1;_tmp5BC.f1=(int)((unsigned int)((
int)zero));{struct Cyc_Std_Int_pa_struct _tmp5BB;_tmp5BB.tag=1;_tmp5BB.f1=(int)((
unsigned int)((int)zero));{struct Cyc_Std_Int_pa_struct _tmp5BA;_tmp5BA.tag=1;
_tmp5BA.f1=(int)((unsigned int)((int)zero));{struct Cyc_Std_Int_pa_struct _tmp5B9;
_tmp5B9.tag=1;_tmp5B9.f1=(int)((unsigned int)((int)zero));{void*_tmp5B7[5]={&
_tmp5B9,& _tmp5BA,& _tmp5BB,& _tmp5BC,& _tmp5BD};Cyc_Std_aprintf(({const char*_tmp5B8="%c%c%c%c%s";
_tag_arr(_tmp5B8,sizeof(char),_get_zero_arr_size(_tmp5B8,11));}),_tag_arr(
_tmp5B7,sizeof(void*),5));}}}}}}),0);struct Cyc_Absyn_Vardecl*_tmp5B3=Cyc_Absyn_new_vardecl(
f->name,_tmp591,(struct Cyc_Absyn_Exp*)_tmp5B2);(void*)(_tmp5B3->sc=(void*)((void*)
f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp5B4=_cycalloc(sizeof(*
_tmp5B4));_tmp5B4->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5[0]=({struct Cyc_Absyn_Var_d_struct
_tmp5B6;_tmp5B6.tag=0;_tmp5B6.f1=_tmp5B3;_tmp5B6;});_tmp5B5;}),0);_tmp5B4->tl=
Cyc_Toc_result_decls;_tmp5B4;});(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,f->name,1));}
goto _LL34A;_LL34F:;_LL350: goto _LL34A;_LL34A:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));if((void*)vd->sc == (void*)5?Cyc_Tcutil_is_tagged_pointer_typ(old_typ):
0)(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp5BE=(void*)init->r;struct Cyc_Absyn_Vardecl*_tmp5BF;struct Cyc_Absyn_Exp*
_tmp5C0;struct Cyc_Absyn_Exp*_tmp5C1;int _tmp5C2;_LL352: if(*((int*)_tmp5BE)!= 27)
goto _LL354;_tmp5BF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp5BE)->f1;
_tmp5C0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp5BE)->f2;_tmp5C1=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp5BE)->f3;_tmp5C2=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp5BE)->f4;_LL353: vd->initializer=0;(void*)(s->r=(void*)((void*)(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp5BF,_tmp5C0,_tmp5C1,_tmp5C2,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));goto _LL351;_LL354:;_LL355: if((void*)vd->sc == (void*)0){
struct Cyc_Toc_Env*_tmp5C3=({struct Cyc_Toc_Env*_tmp5C4=_cycalloc(sizeof(*_tmp5C4));
_tmp5C4[0]=*((struct Cyc_Toc_Env*)init_nv);_tmp5C4;});_tmp5C3->toplevel=1;Cyc_Toc_exp_to_c(
_tmp5C3,init);}else{Cyc_Toc_exp_to_c(init_nv,init);}goto _LL351;_LL351:;}}static
struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=0;static struct Cyc_Absyn_Stmt*
Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt == 0)Cyc_Toc_throw_match_stmt_opt=({
struct Cyc_Absyn_Stmt**_tmp5C5=_cycalloc(sizeof(*_tmp5C5));_tmp5C5[0]=Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(0)),0);_tmp5C5;});return*((struct
Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{struct _tuple1*x=Cyc_Toc_temp_var();{
struct _RegionHandle _tmp5C6=_new_region("prgn");struct _RegionHandle*prgn=& _tmp5C6;
_push_region(prgn);{struct Cyc_Toc_Env*_tmp5C8;struct Cyc_List_List*_tmp5C9;struct
Cyc_Absyn_Stmt*_tmp5CA;struct _tuple12 _tmp5C7=Cyc_Toc_xlate_pat(nv,prgn,t,Cyc_Absyn_var_exp(
x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),0);_tmp5C8=_tmp5C7.f1;
_tmp5C9=_tmp5C7.f2;_tmp5CA=_tmp5C7.f3;Cyc_Toc_stmt_to_c(_tmp5C8,s);s=Cyc_Absyn_declare_stmt(
x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp5CA,s,0),0);
for(0;_tmp5C9 != 0;_tmp5C9=_tmp5C9->tl){struct _tuple13 _tmp5CC;struct _tuple1*
_tmp5CD;void*_tmp5CE;struct _tuple13*_tmp5CB=(struct _tuple13*)_tmp5C9->hd;_tmp5CC=*
_tmp5CB;_tmp5CD=_tmp5CC.f1;_tmp5CE=_tmp5CC.f2;s=Cyc_Absyn_declare_stmt(_tmp5CD,
_tmp5CE,0,s,0);}};_pop_region(prgn);}return s;}}static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top){for(0;ds != 0;ds=ds->tl){if(!
nv->toplevel)({void*_tmp5CF[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp5D0="decls_to_c: not at toplevel!";
_tag_arr(_tmp5D0,sizeof(char),_get_zero_arr_size(_tmp5D0,29));}),_tag_arr(
_tmp5CF,sizeof(void*),0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp5D1=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp5D2;struct Cyc_Absyn_Fndecl*
_tmp5D3;struct Cyc_Absyn_Aggrdecl*_tmp5D4;struct Cyc_Absyn_Tuniondecl*_tmp5D5;
struct Cyc_Absyn_Enumdecl*_tmp5D6;struct Cyc_Absyn_Typedefdecl*_tmp5D7;struct Cyc_List_List*
_tmp5D8;struct Cyc_List_List*_tmp5D9;struct Cyc_List_List*_tmp5DA;_LL357: if(*((int*)
_tmp5D1)!= 0)goto _LL359;_tmp5D2=((struct Cyc_Absyn_Var_d_struct*)_tmp5D1)->f1;
_LL358: {struct _tuple1*_tmp5DB=_tmp5D2->name;if((void*)_tmp5D2->sc == (void*)4)
_tmp5DB=({struct _tuple1*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC->f1=Cyc_Absyn_rel_ns_null;
_tmp5DC->f2=(*((struct _tuple1*)_tmp5DB)).f2;_tmp5DC;});if(_tmp5D2->initializer != 
0)Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp5D2->initializer));
nv=Cyc_Toc_add_varmap(nv,_tmp5D2->name,Cyc_Absyn_varb_exp(_tmp5DB,(void*)({
struct Cyc_Absyn_Global_b_struct*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD[0]=({
struct Cyc_Absyn_Global_b_struct _tmp5DE;_tmp5DE.tag=0;_tmp5DE.f1=_tmp5D2;_tmp5DE;});
_tmp5DD;}),0));_tmp5D2->name=_tmp5DB;(void*)(_tmp5D2->sc=(void*)Cyc_Toc_scope_to_c((
void*)_tmp5D2->sc));(void*)(_tmp5D2->type=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp5D2->type));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp5DF=_cycalloc(
sizeof(*_tmp5DF));_tmp5DF->hd=d;_tmp5DF->tl=Cyc_Toc_result_decls;_tmp5DF;});goto
_LL356;}_LL359: if(*((int*)_tmp5D1)!= 1)goto _LL35B;_tmp5D3=((struct Cyc_Absyn_Fn_d_struct*)
_tmp5D1)->f1;_LL35A: {struct _tuple1*_tmp5E0=_tmp5D3->name;if((void*)_tmp5D3->sc
== (void*)4)_tmp5E0=({struct _tuple1*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->f1=
Cyc_Absyn_rel_ns_null;_tmp5E1->f2=(*((struct _tuple1*)_tmp5E0)).f2;_tmp5E1;});nv=
Cyc_Toc_add_varmap(nv,_tmp5D3->name,Cyc_Absyn_var_exp(_tmp5E0,0));_tmp5D3->name=
_tmp5E0;Cyc_Toc_fndecl_to_c(nv,_tmp5D3);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp5E2=_cycalloc(sizeof(*_tmp5E2));_tmp5E2->hd=d;_tmp5E2->tl=Cyc_Toc_result_decls;
_tmp5E2;});goto _LL356;}_LL35B: if(*((int*)_tmp5D1)!= 2)goto _LL35D;_LL35C: goto
_LL35E;_LL35D: if(*((int*)_tmp5D1)!= 3)goto _LL35F;_LL35E:({void*_tmp5E3[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmp5E4="letdecl at toplevel";_tag_arr(_tmp5E4,sizeof(char),_get_zero_arr_size(
_tmp5E4,20));}),_tag_arr(_tmp5E3,sizeof(void*),0));});_LL35F: if(*((int*)_tmp5D1)
!= 4)goto _LL361;_tmp5D4=((struct Cyc_Absyn_Aggr_d_struct*)_tmp5D1)->f1;_LL360: Cyc_Toc_aggrdecl_to_c(
_tmp5D4);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp5E5=_cycalloc(sizeof(*
_tmp5E5));_tmp5E5->hd=d;_tmp5E5->tl=Cyc_Toc_result_decls;_tmp5E5;});goto _LL356;
_LL361: if(*((int*)_tmp5D1)!= 5)goto _LL363;_tmp5D5=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp5D1)->f1;_LL362: if(_tmp5D5->is_xtunion)Cyc_Toc_xtuniondecl_to_c(_tmp5D5);
else{Cyc_Toc_tuniondecl_to_c(_tmp5D5);}goto _LL356;_LL363: if(*((int*)_tmp5D1)!= 6)
goto _LL365;_tmp5D6=((struct Cyc_Absyn_Enum_d_struct*)_tmp5D1)->f1;_LL364: Cyc_Toc_enumdecl_to_c(
nv,_tmp5D6);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp5E6=_cycalloc(sizeof(*
_tmp5E6));_tmp5E6->hd=d;_tmp5E6->tl=Cyc_Toc_result_decls;_tmp5E6;});goto _LL356;
_LL365: if(*((int*)_tmp5D1)!= 7)goto _LL367;_tmp5D7=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp5D1)->f1;_LL366: _tmp5D7->name=_tmp5D7->name;_tmp5D7->tvs=0;if(_tmp5D7->defn
!= 0)_tmp5D7->defn=({struct Cyc_Core_Opt*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));
_tmp5E7->v=(void*)Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp5D7->defn))->v);_tmp5E7;});else{void*_tmp5E8=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp5D7->kind))->v;_LL36E: if((int)_tmp5E8 != 2)goto _LL370;_LL36F:
_tmp5D7->defn=({struct Cyc_Core_Opt*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9->v=(
void*)Cyc_Absyn_void_star_typ();_tmp5E9;});goto _LL36D;_LL370:;_LL371: _tmp5D7->defn=({
struct Cyc_Core_Opt*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA->v=(void*)((void*)
0);_tmp5EA;});goto _LL36D;_LL36D:;}Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp5EB=_cycalloc(sizeof(*_tmp5EB));_tmp5EB->hd=d;_tmp5EB->tl=Cyc_Toc_result_decls;
_tmp5EB;});goto _LL356;_LL367: if(*((int*)_tmp5D1)!= 8)goto _LL369;_tmp5D8=((struct
Cyc_Absyn_Namespace_d_struct*)_tmp5D1)->f2;_LL368: _tmp5D9=_tmp5D8;goto _LL36A;
_LL369: if(*((int*)_tmp5D1)!= 9)goto _LL36B;_tmp5D9=((struct Cyc_Absyn_Using_d_struct*)
_tmp5D1)->f2;_LL36A: _tmp5DA=_tmp5D9;goto _LL36C;_LL36B: if(*((int*)_tmp5D1)!= 10)
goto _LL356;_tmp5DA=((struct Cyc_Absyn_ExternC_d_struct*)_tmp5D1)->f1;_LL36C: nv=
Cyc_Toc_decls_to_c(nv,_tmp5DA,top);goto _LL356;_LL356:;}}return nv;}static void Cyc_Toc_init(){
Cyc_Toc_result_decls=0;Cyc_Toc_tuple_types=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=
0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_aggrs_so_far=0;Cyc_Toc_tunions_so_far=0;
Cyc_Toc_xtunion_fields_so_far=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(),ds,1);return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
