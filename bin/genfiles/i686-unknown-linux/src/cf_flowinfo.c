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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};void*Cyc_List_hd(struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[
8];int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];struct Cyc_Iter_Iter
Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);typedef struct{int
__count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc_Std___mbstate_t;
typedef struct{int __pos;Cyc_Std___mbstate_t __state;}Cyc_Std__G_fpos_t;typedef Cyc_Std__G_fpos_t
Cyc_Std_fpos_t;struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;
struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_Buffer_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*
f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,struct _tagged_arr);int
Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct _tagged_arr,struct _tagged_arr);
struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_Std_CharPtr_sa_struct{
int tag;struct _tagged_arr f1;};extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Dict_Dict;
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*
Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct
Cyc_Dict_Dict*d,void*accum);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*
env,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_map_c(void*(*f)(void*,
void*),void*env,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_union_two_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2);struct Cyc_Dict_Dict*Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),
void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);int Cyc_Dict_forall_c(int(*
f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);struct
_tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,
struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict*d);int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct
_tagged_arr*s2);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{
void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
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
Cyc_Absyn_UnknownTunionInfo{struct _tuple1*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{
int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{
int tag;struct Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*
tunion_info;struct Cyc_List_List*targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple1*tunion_name;struct _tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_tvar_cmp(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct _tagged_arr*Cyc_Absyn_fieldname(int);
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);int Cyc_Absyn_is_union_type(
void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
ae;struct Cyc_Core_Opt*le;};void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple1*);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};char Cyc_CfFlowInfo_IsZero[11]="\000\000\000\000IsZero\000";struct
Cyc_CfFlowInfo_IsZero_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_NotZero[
12]="\000\000\000\000NotZero\000";struct Cyc_CfFlowInfo_NotZero_struct{char*tag;
struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[13]="\000\000\000\000UnknownZ\000";
struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int
tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int
tag;struct Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();
extern void*Cyc_CfFlowInfo_unknown_none;extern void*Cyc_CfFlowInfo_unknown_this;
extern void*Cyc_CfFlowInfo_unknown_all;extern void*Cyc_CfFlowInfo_esc_none;extern
void*Cyc_CfFlowInfo_esc_this;extern void*Cyc_CfFlowInfo_esc_all;int Cyc_CfFlowInfo_root_cmp(
void*,void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict*d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(void*
f1,void*f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,
struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,void*r);struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,
struct Cyc_Set_Set**all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**,void*,void*);void*Cyc_CfFlowInfo_after_flow(struct Cyc_Set_Set**,
void*,void*,struct Cyc_Set_Set*,struct Cyc_Set_Set*);void*Cyc_CfFlowInfo_kill_flow_region(
void*f,void*rgn);static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_none_v={
0,(void*)((void*)0)};static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_this_v={
0,(void*)((void*)1)};static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_all_v={
0,(void*)((void*)2)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_none_v={
1,(void*)((void*)0)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_this_v={
1,(void*)((void*)1)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_all_v={
1,(void*)((void*)2)};void*Cyc_CfFlowInfo_unknown_none=(void*)& Cyc_CfFlowInfo_unknown_none_v;
void*Cyc_CfFlowInfo_unknown_this=(void*)& Cyc_CfFlowInfo_unknown_this_v;void*Cyc_CfFlowInfo_unknown_all=(
void*)& Cyc_CfFlowInfo_unknown_all_v;void*Cyc_CfFlowInfo_esc_none=(void*)& Cyc_CfFlowInfo_esc_none_v;
void*Cyc_CfFlowInfo_esc_this=(void*)& Cyc_CfFlowInfo_esc_this_v;void*Cyc_CfFlowInfo_esc_all=(
void*)& Cyc_CfFlowInfo_esc_all_v;static struct Cyc_Set_Set**Cyc_CfFlowInfo_mt_place_set_opt=
0;struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set(){if(Cyc_CfFlowInfo_mt_place_set_opt
== 0)Cyc_CfFlowInfo_mt_place_set_opt=({struct Cyc_Set_Set**_tmp6=_cycalloc(
sizeof(*_tmp6));_tmp6[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*)))Cyc_Set_empty)(Cyc_CfFlowInfo_place_cmp);_tmp6;});
return*((struct Cyc_Set_Set**)_check_null(Cyc_CfFlowInfo_mt_place_set_opt));}int
Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{struct
_tuple0 _tmp8=({struct _tuple0 _tmp7;_tmp7.f1=r1;_tmp7.f2=r2;_tmp7;});void*_tmp9;
struct Cyc_Absyn_Vardecl*_tmpA;void*_tmpB;struct Cyc_Absyn_Vardecl*_tmpC;void*
_tmpD;void*_tmpE;void*_tmpF;struct Cyc_Absyn_Exp*_tmp10;void*_tmp11;struct Cyc_Absyn_Exp*
_tmp12;void*_tmp13;void*_tmp14;void*_tmp15;int _tmp16;void*_tmp17;int _tmp18;_LL1:
_tmp9=_tmp8.f1;if(*((int*)_tmp9)!= 0)goto _LL3;_tmpA=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp9)->f1;_tmpB=_tmp8.f2;if(*((int*)_tmpB)!= 0)goto _LL3;_tmpC=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmpB)->f1;_LL2: return(int)_tmpA - (int)_tmpC;_LL3: _tmpD=_tmp8.f1;if(*((int*)
_tmpD)!= 0)goto _LL5;_LL4: return - 1;_LL5: _tmpE=_tmp8.f2;if(*((int*)_tmpE)!= 0)goto
_LL7;_LL6: return 1;_LL7: _tmpF=_tmp8.f1;if(*((int*)_tmpF)!= 1)goto _LL9;_tmp10=((
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmpF)->f1;_tmp11=_tmp8.f2;if(*((int*)
_tmp11)!= 1)goto _LL9;_tmp12=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp11)->f1;
_LL8: return(int)_tmp10 - (int)_tmp12;_LL9: _tmp13=_tmp8.f1;if(*((int*)_tmp13)!= 1)
goto _LLB;_LLA: return - 1;_LLB: _tmp14=_tmp8.f2;if(*((int*)_tmp14)!= 1)goto _LLD;_LLC:
return 1;_LLD: _tmp15=_tmp8.f1;if(*((int*)_tmp15)!= 2)goto _LL0;_tmp16=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp15)->f1;_tmp17=_tmp8.f2;if(*((int*)_tmp17)!= 2)goto _LL0;_tmp18=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp17)->f1;_LLE: return _tmp16 - _tmp18;_LL0:;}}int Cyc_CfFlowInfo_place_cmp(struct
Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)p2)
return 0;{int i=Cyc_CfFlowInfo_root_cmp((void*)p1->root,(void*)p2->root);if(i != 0)
return i;return((int(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Std_strptrcmp,p1->fields,
p2->fields);}}static struct _tagged_arr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*
p){struct Cyc_List_List*sl=0;{void*_tmp19=(void*)p->root;struct Cyc_Absyn_Vardecl*
_tmp1A;struct Cyc_Absyn_Exp*_tmp1B;int _tmp1C;_LL10: if(*((int*)_tmp19)!= 0)goto
_LL12;_tmp1A=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp19)->f1;_LL11: sl=({
struct Cyc_List_List*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->hd=({struct
_tagged_arr*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=({struct Cyc_Std_String_pa_struct
_tmp21;_tmp21.tag=0;_tmp21.f1=(struct _tagged_arr)*((struct _tagged_arr*)(*((
struct _tuple1*)_tmp1A->name)).f2);{void*_tmp1F[1]={& _tmp21};Cyc_Std_aprintf(({
const char*_tmp20="%s";_tag_arr(_tmp20,sizeof(char),_get_zero_arr_size(_tmp20,3));}),
_tag_arr(_tmp1F,sizeof(void*),1));}});_tmp1E;});_tmp1D->tl=sl;_tmp1D;});goto _LLF;
_LL12: if(*((int*)_tmp19)!= 1)goto _LL14;_tmp1B=((struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp19)->f1;_LL13: sl=({struct Cyc_List_List*_tmp22=_cycalloc(sizeof(*_tmp22));
_tmp22->hd=({struct _tagged_arr*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23[0]=({
struct Cyc_Std_Int_pa_struct _tmp26;_tmp26.tag=1;_tmp26.f1=(int)((unsigned int)((
int)_tmp1B));{void*_tmp24[1]={& _tmp26};Cyc_Std_aprintf(({const char*_tmp25="mpt%d";
_tag_arr(_tmp25,sizeof(char),_get_zero_arr_size(_tmp25,6));}),_tag_arr(_tmp24,
sizeof(void*),1));}});_tmp23;});_tmp22->tl=sl;_tmp22;});goto _LLF;_LL14: if(*((int*)
_tmp19)!= 2)goto _LLF;_tmp1C=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp19)->f1;
_LL15: sl=({struct Cyc_List_List*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->hd=({
struct _tagged_arr*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[0]=({struct Cyc_Std_Int_pa_struct
_tmp2B;_tmp2B.tag=1;_tmp2B.f1=(int)((unsigned int)_tmp1C);{void*_tmp29[1]={&
_tmp2B};Cyc_Std_aprintf(({const char*_tmp2A="param%d";_tag_arr(_tmp2A,sizeof(char),
_get_zero_arr_size(_tmp2A,8));}),_tag_arr(_tmp29,sizeof(void*),1));}});_tmp28;});
_tmp27->tl=sl;_tmp27;});goto _LLF;_LLF:;}{struct Cyc_List_List*fields=p->fields;
for(0;fields != 0;fields=fields->tl){sl=({struct Cyc_List_List*_tmp2C=_cycalloc(
sizeof(*_tmp2C));_tmp2C->hd=({struct _tagged_arr*_tmp2D=_cycalloc(sizeof(*_tmp2D));
_tmp2D[0]=({struct Cyc_Std_String_pa_struct _tmp30;_tmp30.tag=0;_tmp30.f1=(struct
_tagged_arr)*((struct _tagged_arr*)((struct _tagged_arr*)fields->hd));{void*_tmp2E[
1]={& _tmp30};Cyc_Std_aprintf(({const char*_tmp2F="%s";_tag_arr(_tmp2F,sizeof(char),
_get_zero_arr_size(_tmp2F,3));}),_tag_arr(_tmp2E,sizeof(void*),1));}});_tmp2D;});
_tmp2C->tl=sl;_tmp2C;});}}{struct _tagged_arr*_tmp31=({struct _tagged_arr*_tmp36=
_cycalloc(sizeof(*_tmp36));_tmp36[0]=({struct Cyc_Std_String_pa_struct _tmp39;
_tmp39.tag=0;_tmp39.f1=({const char*_tmp3A="";_tag_arr(_tmp3A,sizeof(char),
_get_zero_arr_size(_tmp3A,1));});{void*_tmp37[1]={& _tmp39};Cyc_Std_aprintf(({
const char*_tmp38="%s";_tag_arr(_tmp38,sizeof(char),_get_zero_arr_size(_tmp38,3));}),
_tag_arr(_tmp37,sizeof(void*),1));}});_tmp36;});for(0;sl != 0;sl=sl->tl){*((
struct _tagged_arr*)_tmp31)=({struct Cyc_Std_String_pa_struct _tmp35;_tmp35.tag=0;
_tmp35.f1=(struct _tagged_arr)*((struct _tagged_arr*)_tmp31);{struct Cyc_Std_String_pa_struct
_tmp34;_tmp34.tag=0;_tmp34.f1=(struct _tagged_arr)*((struct _tagged_arr*)((struct
_tagged_arr*)sl->hd));{void*_tmp32[2]={& _tmp34,& _tmp35};Cyc_Std_aprintf(({const
char*_tmp33="%s.%s";_tag_arr(_tmp33,sizeof(char),_get_zero_arr_size(_tmp33,6));}),
_tag_arr(_tmp32,sizeof(void*),2));}}});}return _tmp31;}}struct _tuple4{struct Cyc_Absyn_Tqual
f1;void*f2;};void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval){if(!Cyc_Absyn_is_union_type(
t)){void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_TunionFieldInfo _tmp3C;
void*_tmp3D;struct Cyc_Absyn_Tunionfield*_tmp3E;struct Cyc_List_List*_tmp3F;struct
Cyc_Absyn_AggrInfo _tmp40;void*_tmp41;void*_tmp42;struct Cyc_List_List*_tmp43;
_LL17: if(_tmp3B <= (void*)3?1:*((int*)_tmp3B)!= 3)goto _LL19;_tmp3C=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3B)->f1;_tmp3D=(void*)_tmp3C.field_info;if(*((int*)_tmp3D)!= 1)goto _LL19;
_tmp3E=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3D)->f2;_LL18: if(_tmp3E->typs
== 0)return leafval;_tmp3F=_tmp3E->typs;goto _LL1A;_LL19: if(_tmp3B <= (void*)3?1:*((
int*)_tmp3B)!= 9)goto _LL1B;_tmp3F=((struct Cyc_Absyn_TupleType_struct*)_tmp3B)->f1;
_LL1A: {struct Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_strptrcmp);{int i=0;
for(0;_tmp3F != 0;(_tmp3F=_tmp3F->tl,++ i)){d=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(d,Cyc_Absyn_fieldname(i),Cyc_CfFlowInfo_typ_to_absrval((*((
struct _tuple4*)((struct _tuple4*)_tmp3F->hd))).f2,leafval));}}return(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44[0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _tmp45;_tmp45.tag=3;_tmp45.f1=d;_tmp45;});
_tmp44;});}_LL1B: if(_tmp3B <= (void*)3?1:*((int*)_tmp3B)!= 10)goto _LL1D;_tmp40=((
struct Cyc_Absyn_AggrType_struct*)_tmp3B)->f1;_tmp41=(void*)_tmp40.aggr_info;
_LL1C: {struct Cyc_Absyn_Aggrdecl*_tmp46=Cyc_Absyn_get_known_aggrdecl(_tmp41);if(
_tmp46->impl == 0)goto _LL16;_tmp43=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp46->impl))->fields;goto _LL1E;}_LL1D: if(_tmp3B <= (void*)3?1:*((int*)_tmp3B)!= 
11)goto _LL1F;_tmp42=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp3B)->f1;
if((int)_tmp42 != 0)goto _LL1F;_tmp43=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp3B)->f2;_LL1E: {struct Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_strptrcmp);for(0;
_tmp43 != 0;_tmp43=_tmp43->tl){struct Cyc_Absyn_Aggrfield _tmp48;struct _tagged_arr*
_tmp49;void*_tmp4A;struct Cyc_Absyn_Aggrfield*_tmp47=(struct Cyc_Absyn_Aggrfield*)
_tmp43->hd;_tmp48=*_tmp47;_tmp49=_tmp48.name;_tmp4A=(void*)_tmp48.type;if(
_get_arr_size(*((struct _tagged_arr*)_tmp49),sizeof(char))!= 1)d=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(d,_tmp49,Cyc_CfFlowInfo_typ_to_absrval(
_tmp4A,leafval));}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp4B=
_cycalloc(sizeof(*_tmp4B));_tmp4B[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp4C;_tmp4C.tag=3;_tmp4C.f1=d;_tmp4C;});_tmp4B;});}_LL1F:;_LL20: goto _LL16;
_LL16:;}return Cyc_Tcutil_bits_only(t)?Cyc_CfFlowInfo_unknown_all: leafval;}static
int Cyc_CfFlowInfo_prefix_of_member(struct _RegionHandle*r,struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Set_Set*set){struct Cyc_CfFlowInfo_Place*_tmp4D=place;struct Cyc_Iter_Iter
_tmp4E=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(
r,set);while(((int(*)(struct Cyc_Iter_Iter,struct Cyc_CfFlowInfo_Place**))Cyc_Iter_next)(
_tmp4E,& _tmp4D)){if(Cyc_CfFlowInfo_root_cmp((void*)place->root,(void*)_tmp4D->root)
!= 0)continue;{struct Cyc_List_List*_tmp4F=place->fields;struct Cyc_List_List*
_tmp50=_tmp4D->fields;for(0;_tmp4F != 0?_tmp50 != 0: 0;(_tmp4F=_tmp4F->tl,_tmp50=
_tmp50->tl)){if(Cyc_Std_strptrcmp((struct _tagged_arr*)_tmp4F->hd,(struct
_tagged_arr*)_tmp50->hd)!= 0)break;}if(_tmp4F == 0)return 1;}}return 0;}struct Cyc_CfFlowInfo_EscPile{
struct _RegionHandle*rgn;struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(
struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,
place))pile->places=({struct Cyc_List_List*_tmp51=_region_malloc(pile->rgn,
sizeof(*_tmp51));_tmp51->hd=place;_tmp51->tl=pile->places;_tmp51;});}static void
Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*a,void*r){void*
_tmp52=r;struct Cyc_CfFlowInfo_Place*_tmp53;struct Cyc_Dict_Dict*_tmp54;_LL22: if(
_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 2)goto _LL24;_tmp53=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp52)->f1;_LL23: Cyc_CfFlowInfo_add_place(pile,_tmp53);return;_LL24: if(_tmp52 <= (
void*)3?1:*((int*)_tmp52)!= 3)goto _LL26;_tmp54=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp52)->f1;_LL25:((void(*)(void(*f)(struct Cyc_CfFlowInfo_EscPile*,struct
_tagged_arr*,void*),struct Cyc_CfFlowInfo_EscPile*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)((
void(*)(struct Cyc_CfFlowInfo_EscPile*pile,struct _tagged_arr*a,void*r))Cyc_CfFlowInfo_add_places,
pile,_tmp54);return;_LL26:;_LL27: return;_LL21:;}static void*Cyc_CfFlowInfo_insert_place_inner(
void*new_val,void*old_val){void*_tmp55=old_val;struct Cyc_Dict_Dict*_tmp56;_LL29:
if(_tmp55 <= (void*)3?1:*((int*)_tmp55)!= 3)goto _LL2B;_tmp56=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp55)->f1;_LL2A: return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp57=
_cycalloc(sizeof(*_tmp57));_tmp57[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp58;_tmp58.tag=3;_tmp58.f1=((struct Cyc_Dict_Dict*(*)(void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d))Cyc_Dict_map_c)(Cyc_CfFlowInfo_insert_place_inner,
new_val,_tmp56);_tmp58;});_tmp57;});_LL2B:;_LL2C: return new_val;_LL28:;}struct
_tuple5{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
new_val,old_val);{struct _tuple5 _tmp5A=({struct _tuple5 _tmp59;_tmp59.f1=fs;_tmp59.f2=
old_val;_tmp59;});struct Cyc_List_List*_tmp5B;struct Cyc_List_List _tmp5C;struct
_tagged_arr*_tmp5D;struct Cyc_List_List*_tmp5E;void*_tmp5F;struct Cyc_Dict_Dict*
_tmp60;_LL2E: _tmp5B=_tmp5A.f1;if(_tmp5B == 0)goto _LL30;_tmp5C=*_tmp5B;_tmp5D=(
struct _tagged_arr*)_tmp5C.hd;_tmp5E=_tmp5C.tl;_tmp5F=_tmp5A.f2;if(_tmp5F <= (void*)
3?1:*((int*)_tmp5F)!= 3)goto _LL30;_tmp60=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp5F)->f1;_LL2F: {void*_tmp61=Cyc_CfFlowInfo_insert_place_outer(_tmp5E,((void*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp60,_tmp5D),
new_val);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp62=_cycalloc(
sizeof(*_tmp62));_tmp62[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp63;_tmp63.tag=
3;_tmp63.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,
void*v))Cyc_Dict_insert)(_tmp60,_tmp5D,_tmp61);_tmp63;});_tmp62;});}_LL30:;_LL31:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp64=_cycalloc(sizeof(*
_tmp64));_tmp64[0]=({struct Cyc_Core_Impossible_struct _tmp65;_tmp65.tag=Cyc_Core_Impossible;
_tmp65.f1=({const char*_tmp66="bad insert place";_tag_arr(_tmp66,sizeof(char),
_get_zero_arr_size(_tmp66,17));});_tmp65;});_tmp64;}));_LL2D:;}}static struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Set_Set**
all_changed,struct Cyc_Dict_Dict*d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*
_tmp67=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;if(
all_changed != 0)*((struct Cyc_Set_Set**)all_changed)=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
all_changed),_tmp67);{void*oldval;void*newval;{struct _handler_cons _tmp68;
_push_handler(& _tmp68);{int _tmp6A=0;if(setjmp(_tmp68.handler))_tmp6A=1;if(!
_tmp6A){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp67);;_pop_handler();}else{void*
_tmp69=(void*)_exn_thrown;void*_tmp6C=_tmp69;_LL33: if(_tmp6C != Cyc_Dict_Absent)
goto _LL35;_LL34: continue;_LL35:;_LL36:(void)_throw(_tmp6C);_LL32:;}}}{void*
_tmp6D=Cyc_CfFlowInfo_initlevel(d,oldval);_LL38: if((int)_tmp6D != 2)goto _LL3A;
_LL39: newval=Cyc_CfFlowInfo_esc_all;goto _LL37;_LL3A: if((int)_tmp6D != 1)goto _LL3C;
_LL3B: newval=Cyc_CfFlowInfo_esc_this;goto _LL37;_LL3C: if((int)_tmp6D != 0)goto
_LL37;_LL3D: newval=Cyc_CfFlowInfo_esc_none;goto _LL37;_LL37:;}((void(*)(struct Cyc_CfFlowInfo_EscPile*
pile,int a,void*r))Cyc_CfFlowInfo_add_places)(pile,0,oldval);d=Cyc_Dict_insert(d,(
void*)_tmp67->root,Cyc_CfFlowInfo_insert_place_outer(_tmp67->fields,Cyc_Dict_lookup(
d,(void*)_tmp67->root),newval));}}return d;}struct Cyc_CfFlowInfo_InitlevelEnv{
struct Cyc_Dict_Dict*d;struct Cyc_List_List*seen;};static char Cyc_CfFlowInfo_dummy_str_v[
1]="";static struct _tagged_arr Cyc_CfFlowInfo_dummy_str={(void*)((char*)Cyc_CfFlowInfo_dummy_str_v),(
void*)((char*)Cyc_CfFlowInfo_dummy_str_v),(void*)((char*)Cyc_CfFlowInfo_dummy_str_v
+ 1)};static void*Cyc_CfFlowInfo_initlevel_approx(void*r){void*_tmp6E=r;void*
_tmp6F;void*_tmp70;_LL3F: if(_tmp6E <= (void*)3?1:*((int*)_tmp6E)!= 0)goto _LL41;
_tmp6F=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp6E)->f1;_LL40: return
_tmp6F;_LL41: if(_tmp6E <= (void*)3?1:*((int*)_tmp6E)!= 1)goto _LL43;_tmp70=(void*)((
struct Cyc_CfFlowInfo_Esc_struct*)_tmp6E)->f1;_LL42: return _tmp70;_LL43: if((int)
_tmp6E != 0)goto _LL45;_LL44: goto _LL46;_LL45: if((int)_tmp6E != 1)goto _LL47;_LL46:
return(void*)2;_LL47: if((int)_tmp6E != 2)goto _LL49;_LL48: return(void*)1;_LL49:;
_LL4A:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp71=_cycalloc(
sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Core_Impossible_struct _tmp72;_tmp72.tag=
Cyc_Core_Impossible;_tmp72.f1=({const char*_tmp73="initlevel_approx";_tag_arr(
_tmp73,sizeof(char),_get_zero_arr_size(_tmp73,17));});_tmp72;});_tmp71;}));_LL3E:;}
static void*Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,
void*a,void*r,void*acc){void*this_ans;if(acc == (void*)0)return(void*)0;{void*
_tmp74=r;struct Cyc_Dict_Dict*_tmp75;struct Cyc_CfFlowInfo_Place*_tmp76;_LL4C: if(
_tmp74 <= (void*)3?1:*((int*)_tmp74)!= 3)goto _LL4E;_tmp75=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp74)->f1;_LL4D: this_ans=((void*(*)(void*(*f)(struct Cyc_CfFlowInfo_InitlevelEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_InitlevelEnv*env,struct Cyc_Dict_Dict*
d,void*accum))Cyc_Dict_fold_c)((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,
struct _tagged_arr*a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec,env,_tmp75,(
void*)2);goto _LL4B;_LL4E: if(_tmp74 <= (void*)3?1:*((int*)_tmp74)!= 2)goto _LL50;
_tmp76=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp74)->f1;_LL4F: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp76))this_ans=(void*)2;else{env->seen=({struct Cyc_List_List*_tmp77=_cycalloc(
sizeof(*_tmp77));_tmp77->hd=_tmp76;_tmp77->tl=env->seen;_tmp77;});this_ans=((
void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(
env,0,Cyc_CfFlowInfo_lookup_place(env->d,_tmp76),(void*)2);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == (void*)0)this_ans=(void*)1;}goto _LL4B;
_LL50:;_LL51: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL4B:;}if(this_ans == (
void*)0)return(void*)0;if(this_ans == (void*)1?1: acc == (void*)1)return(void*)1;
return(void*)2;}void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,void*r){
struct Cyc_CfFlowInfo_InitlevelEnv _tmp78=({struct Cyc_CfFlowInfo_InitlevelEnv
_tmp79;_tmp79.d=d;_tmp79.seen=0;_tmp79;});return((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*
env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(& _tmp78,0,r,(void*)2);}
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place){struct Cyc_CfFlowInfo_Place _tmp7B;void*_tmp7C;struct Cyc_List_List*_tmp7D;
struct Cyc_CfFlowInfo_Place*_tmp7A=place;_tmp7B=*_tmp7A;_tmp7C=(void*)_tmp7B.root;
_tmp7D=_tmp7B.fields;{void*_tmp7E=Cyc_Dict_lookup(d,_tmp7C);for(0;_tmp7D != 0;
_tmp7D=_tmp7D->tl){struct _tuple1 _tmp80=({struct _tuple1 _tmp7F;_tmp7F.f1=_tmp7E;
_tmp7F.f2=(struct _tagged_arr*)_tmp7D->hd;_tmp7F;});void*_tmp81;struct Cyc_Dict_Dict*
_tmp82;struct _tagged_arr*_tmp83;_LL53: _tmp81=_tmp80.f1;if(_tmp81 <= (void*)3?1:*((
int*)_tmp81)!= 3)goto _LL55;_tmp82=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp81)->f1;_tmp83=_tmp80.f2;_LL54: _tmp7E=((void*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp82,_tmp83);goto _LL52;_LL55:;_LL56:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp84=_cycalloc(sizeof(*_tmp84));
_tmp84[0]=({struct Cyc_Core_Impossible_struct _tmp85;_tmp85.tag=Cyc_Core_Impossible;
_tmp85.f1=({const char*_tmp86="bad lookup_place";_tag_arr(_tmp86,sizeof(char),
_get_zero_arr_size(_tmp86,17));});_tmp85;});_tmp84;}));_LL52:;}return _tmp7E;}}
static int Cyc_CfFlowInfo_is_rval_unescaped(void*a,void*b,void*rval){void*_tmp87=
rval;struct Cyc_Dict_Dict*_tmp88;_LL58: if(_tmp87 <= (void*)3?1:*((int*)_tmp87)!= 1)
goto _LL5A;_LL59: return 0;_LL5A: if(_tmp87 <= (void*)3?1:*((int*)_tmp87)!= 3)goto
_LL5C;_tmp88=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp87)->f1;_LL5B: return((
int(*)(int(*f)(int,struct _tagged_arr*,void*),int env,struct Cyc_Dict_Dict*d))Cyc_Dict_forall_c)((
int(*)(int a,struct _tagged_arr*b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped,0,
_tmp88);_LL5C:;_LL5D: return 1;_LL57:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place){return((int(*)(int a,int b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped)(
0,0,Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,void*r){struct _RegionHandle
_tmp89=_new_region("rgn");struct _RegionHandle*rgn=& _tmp89;_push_region(rgn);{
struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmp8B=
_region_malloc(rgn,sizeof(*_tmp8B));_tmp8B->rgn=rgn;_tmp8B->places=0;_tmp8B;});((
void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,r);{struct Cyc_Dict_Dict*_tmp8A=Cyc_CfFlowInfo_escape_these(pile,
all_changed,d);_npop_handler(0);return _tmp8A;}};_pop_region(rgn);}struct Cyc_CfFlowInfo_AssignEnv{
struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict*d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*a,void*oldval,void*newval){struct _tuple0 _tmp8D=({struct _tuple0 _tmp8C;
_tmp8C.f1=oldval;_tmp8C.f2=newval;_tmp8C;});void*_tmp8E;void*_tmp8F;struct Cyc_CfFlowInfo_Place*
_tmp90;void*_tmp91;void*_tmp92;struct Cyc_Dict_Dict*_tmp93;void*_tmp94;struct Cyc_Dict_Dict*
_tmp95;void*_tmp96;void*_tmp97;_LL5F: _tmp8E=_tmp8D.f1;if(_tmp8E <= (void*)3?1:*((
int*)_tmp8E)!= 1)goto _LL61;_tmp8F=_tmp8D.f2;if(_tmp8F <= (void*)3?1:*((int*)
_tmp8F)!= 2)goto _LL61;_tmp90=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp8F)->f1;
_LL60: Cyc_CfFlowInfo_add_place(env->pile,_tmp90);goto _LL62;_LL61: _tmp91=_tmp8D.f1;
if(_tmp91 <= (void*)3?1:*((int*)_tmp91)!= 1)goto _LL63;_LL62: if(Cyc_CfFlowInfo_initlevel(
env->d,newval)!= (void*)2)({void*_tmp98[0]={};Cyc_Tcutil_terr(env->loc,({const
char*_tmp99="assignment puts possibly-uninitialized data in an escaped location";
_tag_arr(_tmp99,sizeof(char),_get_zero_arr_size(_tmp99,67));}),_tag_arr(_tmp98,
sizeof(void*),0));});return Cyc_CfFlowInfo_esc_all;_LL63: _tmp92=_tmp8D.f1;if(
_tmp92 <= (void*)3?1:*((int*)_tmp92)!= 3)goto _LL65;_tmp93=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp92)->f1;_tmp94=_tmp8D.f2;if(_tmp94 <= (void*)3?1:*((int*)_tmp94)!= 3)goto
_LL65;_tmp95=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp94)->f1;_LL64: {struct
Cyc_Dict_Dict*_tmp9A=((struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_AssignEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(struct Cyc_CfFlowInfo_AssignEnv*
env,struct _tagged_arr*a,void*oldval,void*newval))Cyc_CfFlowInfo_assign_place_inner,
env,_tmp93,_tmp95);if(_tmp9A == _tmp93)return oldval;if(_tmp9A == _tmp95)return
newval;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp9B=_cycalloc(
sizeof(*_tmp9B));_tmp9B[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp9C;_tmp9C.tag=
3;_tmp9C.f1=_tmp9A;_tmp9C;});_tmp9B;});}_LL65: _tmp96=_tmp8D.f2;if(_tmp96 <= (void*)
3?1:*((int*)_tmp96)!= 1)goto _LL67;_tmp97=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp96)->f1;_LL66: {void*_tmp9D=_tmp97;_LL6A: if((int)_tmp9D != 0)goto _LL6C;_LL6B:
return Cyc_CfFlowInfo_unknown_none;_LL6C: if((int)_tmp9D != 1)goto _LL6E;_LL6D:
return Cyc_CfFlowInfo_unknown_this;_LL6E: if((int)_tmp9D != 2)goto _LL69;_LL6F:
return Cyc_CfFlowInfo_unknown_all;_LL69:;}_LL67:;_LL68: return newval;_LL5E:;}
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,
struct Cyc_List_List*fs,void*oldval,void*newval){if(fs == 0)return((void*(*)(
struct Cyc_CfFlowInfo_AssignEnv*env,int a,void*oldval,void*newval))Cyc_CfFlowInfo_assign_place_inner)(
env,0,oldval,newval);{struct _tuple5 _tmp9F=({struct _tuple5 _tmp9E;_tmp9E.f1=fs;
_tmp9E.f2=oldval;_tmp9E;});struct Cyc_List_List*_tmpA0;struct Cyc_List_List _tmpA1;
struct _tagged_arr*_tmpA2;struct Cyc_List_List*_tmpA3;void*_tmpA4;struct Cyc_Dict_Dict*
_tmpA5;_LL71: _tmpA0=_tmp9F.f1;if(_tmpA0 == 0)goto _LL73;_tmpA1=*_tmpA0;_tmpA2=(
struct _tagged_arr*)_tmpA1.hd;_tmpA3=_tmpA1.tl;_tmpA4=_tmp9F.f2;if(_tmpA4 <= (void*)
3?1:*((int*)_tmpA4)!= 3)goto _LL73;_tmpA5=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpA4)->f1;_LL72: {void*_tmpA6=Cyc_CfFlowInfo_assign_place_outer(env,_tmpA3,((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmpA5,
_tmpA2),newval);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpA7=
_cycalloc(sizeof(*_tmpA7));_tmpA7[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpA8;_tmpA8.tag=3;_tmpA8.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(_tmpA5,_tmpA2,_tmpA6);_tmpA8;});
_tmpA7;});}_LL73:;_LL74:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Core_Impossible_struct
_tmpAA;_tmpAA.tag=Cyc_Core_Impossible;_tmpAA.f1=({const char*_tmpAB="bad insert place";
_tag_arr(_tmpAB,sizeof(char),_get_zero_arr_size(_tmpAB,17));});_tmpAA;});_tmpA9;}));
_LL70:;}}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r){if(all_changed != 0)*((struct Cyc_Set_Set**)all_changed)=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
all_changed),place);{struct _RegionHandle _tmpAC=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpAC;_push_region(rgn);{struct Cyc_CfFlowInfo_Place _tmpAE;
void*_tmpAF;struct Cyc_List_List*_tmpB0;struct Cyc_CfFlowInfo_Place*_tmpAD=place;
_tmpAE=*_tmpAD;_tmpAF=(void*)_tmpAE.root;_tmpB0=_tmpAE.fields;{struct Cyc_CfFlowInfo_AssignEnv
env=({struct Cyc_CfFlowInfo_AssignEnv _tmpB2;_tmpB2.pile=({struct Cyc_CfFlowInfo_EscPile*
_tmpB3=_region_malloc(rgn,sizeof(*_tmpB3));_tmpB3->rgn=rgn;_tmpB3->places=0;
_tmpB3;});_tmpB2.d=d;_tmpB2.loc=loc;_tmpB2;});void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmpB0,Cyc_Dict_lookup(d,_tmpAF),r);struct Cyc_Dict_Dict*_tmpB1=Cyc_CfFlowInfo_escape_these(
env.pile,all_changed,Cyc_Dict_insert(d,_tmpAF,newval));_npop_handler(0);return
_tmpB1;}};_pop_region(rgn);}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict*d1;struct Cyc_Dict_Dict*d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Set_Set*
chg1;struct Cyc_Set_Set*chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static void*Cyc_CfFlowInfo_join_absRval(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{
struct _tuple0 _tmpB5=({struct _tuple0 _tmpB4;_tmpB4.f1=r1;_tmpB4.f2=r2;_tmpB4;});
void*_tmpB6;struct Cyc_CfFlowInfo_Place*_tmpB7;void*_tmpB8;struct Cyc_CfFlowInfo_Place*
_tmpB9;void*_tmpBA;struct Cyc_CfFlowInfo_Place*_tmpBB;void*_tmpBC;struct Cyc_CfFlowInfo_Place*
_tmpBD;void*_tmpBE;void*_tmpBF;void*_tmpC0;void*_tmpC1;void*_tmpC2;struct Cyc_Dict_Dict*
_tmpC3;void*_tmpC4;struct Cyc_Dict_Dict*_tmpC5;_LL76: _tmpB6=_tmpB5.f1;if(_tmpB6 <= (
void*)3?1:*((int*)_tmpB6)!= 2)goto _LL78;_tmpB7=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpB6)->f1;_tmpB8=_tmpB5.f2;if(_tmpB8 <= (void*)3?1:*((int*)_tmpB8)!= 2)goto
_LL78;_tmpB9=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpB8)->f1;_LL77: if(Cyc_CfFlowInfo_place_cmp(
_tmpB7,_tmpB9)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmpB7);Cyc_CfFlowInfo_add_place(
env->pile,_tmpB9);goto _LL75;_LL78: _tmpBA=_tmpB5.f1;if(_tmpBA <= (void*)3?1:*((int*)
_tmpBA)!= 2)goto _LL7A;_tmpBB=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpBA)->f1;
_LL79: Cyc_CfFlowInfo_add_place(env->pile,_tmpBB);goto _LL75;_LL7A: _tmpBC=_tmpB5.f2;
if(_tmpBC <= (void*)3?1:*((int*)_tmpBC)!= 2)goto _LL7C;_tmpBD=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpBC)->f1;_LL7B: Cyc_CfFlowInfo_add_place(env->pile,_tmpBD);goto _LL75;_LL7C:
_tmpBE=_tmpB5.f1;if((int)_tmpBE != 1)goto _LL7E;_tmpBF=_tmpB5.f2;if((int)_tmpBF != 
2)goto _LL7E;_LL7D: goto _LL7F;_LL7E: _tmpC0=_tmpB5.f1;if((int)_tmpC0 != 2)goto _LL80;
_tmpC1=_tmpB5.f2;if((int)_tmpC1 != 1)goto _LL80;_LL7F: return(void*)2;_LL80: _tmpC2=
_tmpB5.f1;if(_tmpC2 <= (void*)3?1:*((int*)_tmpC2)!= 3)goto _LL82;_tmpC3=((struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmpC2)->f1;_tmpC4=_tmpB5.f2;if(_tmpC4 <= (void*)
3?1:*((int*)_tmpC4)!= 3)goto _LL82;_tmpC5=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpC4)->f1;_LL81: {struct Cyc_Dict_Dict*_tmpC6=((struct Cyc_Dict_Dict*(*)(void*(*
f)(struct Cyc_CfFlowInfo_JoinEnv*,struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(
struct Cyc_CfFlowInfo_JoinEnv*env,struct _tagged_arr*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,
env,_tmpC3,_tmpC5);if(_tmpC6 == _tmpC3)return r1;if(_tmpC6 == _tmpC5)return r2;
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpC7=_cycalloc(sizeof(*
_tmpC7));_tmpC7[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmpC8;_tmpC8.tag=3;
_tmpC8.f1=_tmpC6;_tmpC8;});_tmpC7;});}_LL82:;_LL83: goto _LL75;_LL75:;}{void*il1=
Cyc_CfFlowInfo_initlevel(env->d1,r1);void*il2=Cyc_CfFlowInfo_initlevel(env->d2,
r2);struct _tuple0 _tmpCA=({struct _tuple0 _tmpC9;_tmpC9.f1=r1;_tmpC9.f2=r2;_tmpC9;});
void*_tmpCB;void*_tmpCC;_LL85: _tmpCB=_tmpCA.f1;if(_tmpCB <= (void*)3?1:*((int*)
_tmpCB)!= 1)goto _LL87;_LL86: goto _LL88;_LL87: _tmpCC=_tmpCA.f2;if(_tmpCC <= (void*)
3?1:*((int*)_tmpCC)!= 1)goto _LL89;_LL88: {struct _tuple0 _tmpCE=({struct _tuple0
_tmpCD;_tmpCD.f1=il1;_tmpCD.f2=il2;_tmpCD;});void*_tmpCF;void*_tmpD0;void*_tmpD1;
void*_tmpD2;_LL8C: _tmpCF=_tmpCE.f2;if((int)_tmpCF != 0)goto _LL8E;_LL8D: goto _LL8F;
_LL8E: _tmpD0=_tmpCE.f1;if((int)_tmpD0 != 0)goto _LL90;_LL8F: return Cyc_CfFlowInfo_esc_none;
_LL90: _tmpD1=_tmpCE.f2;if((int)_tmpD1 != 1)goto _LL92;_LL91: goto _LL93;_LL92: _tmpD2=
_tmpCE.f1;if((int)_tmpD2 != 1)goto _LL94;_LL93: return Cyc_CfFlowInfo_esc_this;_LL94:;
_LL95: return Cyc_CfFlowInfo_esc_all;_LL8B:;}_LL89:;_LL8A: {struct _tuple0 _tmpD4=({
struct _tuple0 _tmpD3;_tmpD3.f1=il1;_tmpD3.f2=il2;_tmpD3;});void*_tmpD5;void*
_tmpD6;void*_tmpD7;void*_tmpD8;_LL97: _tmpD5=_tmpD4.f2;if((int)_tmpD5 != 0)goto
_LL99;_LL98: goto _LL9A;_LL99: _tmpD6=_tmpD4.f1;if((int)_tmpD6 != 0)goto _LL9B;_LL9A:
return Cyc_CfFlowInfo_unknown_none;_LL9B: _tmpD7=_tmpD4.f2;if((int)_tmpD7 != 1)goto
_LL9D;_LL9C: goto _LL9E;_LL9D: _tmpD8=_tmpD4.f1;if((int)_tmpD8 != 1)goto _LL9F;_LL9E:
return Cyc_CfFlowInfo_unknown_this;_LL9F:;_LLA0: return Cyc_CfFlowInfo_unknown_all;
_LL96:;}_LL84:;}}int Cyc_CfFlowInfo_same_relop(void*r1,void*r2){if(r1 == r2)return
1;{struct _tuple0 _tmpDA=({struct _tuple0 _tmpD9;_tmpD9.f1=r1;_tmpD9.f2=r2;_tmpD9;});
void*_tmpDB;unsigned int _tmpDC;void*_tmpDD;unsigned int _tmpDE;void*_tmpDF;struct
Cyc_Absyn_Vardecl*_tmpE0;void*_tmpE1;struct Cyc_Absyn_Vardecl*_tmpE2;void*_tmpE3;
struct Cyc_Absyn_Vardecl*_tmpE4;void*_tmpE5;struct Cyc_Absyn_Vardecl*_tmpE6;void*
_tmpE7;unsigned int _tmpE8;void*_tmpE9;unsigned int _tmpEA;void*_tmpEB;struct Cyc_Absyn_Vardecl*
_tmpEC;void*_tmpED;struct Cyc_Absyn_Vardecl*_tmpEE;_LLA2: _tmpDB=_tmpDA.f1;if(*((
int*)_tmpDB)!= 0)goto _LLA4;_tmpDC=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmpDB)->f1;_tmpDD=_tmpDA.f2;if(*((int*)_tmpDD)!= 0)goto _LLA4;_tmpDE=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmpDD)->f1;_LLA3: return _tmpDC == _tmpDE;_LLA4: _tmpDF=_tmpDA.f1;if(*((int*)_tmpDF)
!= 1)goto _LLA6;_tmpE0=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmpDF)->f1;_tmpE1=
_tmpDA.f2;if(*((int*)_tmpE1)!= 1)goto _LLA6;_tmpE2=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmpE1)->f1;_LLA5: return _tmpE0 == _tmpE2;_LLA6: _tmpE3=_tmpDA.f1;if(*((int*)_tmpE3)
!= 2)goto _LLA8;_tmpE4=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmpE3)->f1;_tmpE5=
_tmpDA.f2;if(*((int*)_tmpE5)!= 2)goto _LLA8;_tmpE6=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmpE5)->f1;_LLA7: return _tmpE4 == _tmpE6;_LLA8: _tmpE7=_tmpDA.f1;if(*((int*)_tmpE7)
!= 3)goto _LLAA;_tmpE8=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmpE7)->f1;
_tmpE9=_tmpDA.f2;if(*((int*)_tmpE9)!= 3)goto _LLAA;_tmpEA=((struct Cyc_CfFlowInfo_LessConst_struct*)
_tmpE9)->f1;_LLA9: return _tmpE8 == _tmpEA;_LLAA: _tmpEB=_tmpDA.f1;if(*((int*)_tmpEB)
!= 4)goto _LLAC;_tmpEC=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmpEB)->f1;
_tmpED=_tmpDA.f2;if(*((int*)_tmpED)!= 4)goto _LLAC;_tmpEE=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmpED)->f1;_LLAB: return _tmpEC == _tmpEE;_LLAC:;_LLAD: return 0;_LLA1:;}}struct Cyc_List_List*
Cyc_CfFlowInfo_join_relns(struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(
r1s == r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*
_tmpEF=r1s;for(0;_tmpEF != 0;_tmpEF=_tmpEF->tl){struct Cyc_CfFlowInfo_Reln*_tmpF0=(
struct Cyc_CfFlowInfo_Reln*)_tmpEF->hd;int found=0;{struct Cyc_List_List*_tmpF1=r2s;
for(0;_tmpF1 != 0;_tmpF1=_tmpF1->tl){struct Cyc_CfFlowInfo_Reln*_tmpF2=(struct Cyc_CfFlowInfo_Reln*)
_tmpF1->hd;if(_tmpF0 == _tmpF2?1:(_tmpF0->vd == _tmpF2->vd?Cyc_CfFlowInfo_same_relop((
void*)_tmpF0->rop,(void*)_tmpF2->rop): 0)){res=({struct Cyc_List_List*_tmpF3=
_cycalloc(sizeof(*_tmpF3));_tmpF3->hd=_tmpF0;_tmpF3->tl=res;_tmpF3;});found=1;
break;}}}if(!found)diff=1;}}if(!diff)res=r1s;return res;}}void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**all_changed,void*f1,void*f2){if(f1 == f2)return f1;{struct
_tuple0 _tmpF5=({struct _tuple0 _tmpF4;_tmpF4.f1=f1;_tmpF4.f2=f2;_tmpF4;});void*
_tmpF6;void*_tmpF7;void*_tmpF8;struct Cyc_Dict_Dict*_tmpF9;struct Cyc_List_List*
_tmpFA;void*_tmpFB;struct Cyc_Dict_Dict*_tmpFC;struct Cyc_List_List*_tmpFD;_LLAF:
_tmpF6=_tmpF5.f1;if((int)_tmpF6 != 0)goto _LLB1;_LLB0: return f2;_LLB1: _tmpF7=_tmpF5.f2;
if((int)_tmpF7 != 0)goto _LLB3;_LLB2: return f1;_LLB3: _tmpF8=_tmpF5.f1;if(_tmpF8 <= (
void*)1?1:*((int*)_tmpF8)!= 0)goto _LLAE;_tmpF9=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpF8)->f1;_tmpFA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpF8)->f2;_tmpFB=
_tmpF5.f2;if(_tmpFB <= (void*)1?1:*((int*)_tmpFB)!= 0)goto _LLAE;_tmpFC=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmpFB)->f1;_tmpFD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpFB)->f2;_LLB4: if(_tmpF9 == _tmpFC?_tmpFA == _tmpFD: 0)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle _tmpFE=_new_region("rgn");struct _RegionHandle*rgn=& _tmpFE;
_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmpFF=({struct Cyc_CfFlowInfo_JoinEnv
_tmp105;_tmp105.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp106=_region_malloc(rgn,
sizeof(*_tmp106));_tmp106->rgn=rgn;_tmp106->places=0;_tmp106;});_tmp105.d1=
_tmpF9;_tmp105.d2=_tmpFC;_tmp105;});struct Cyc_Dict_Dict*_tmp100=((struct Cyc_Dict_Dict*(*)(
void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,&
_tmpFF,_tmpF9,_tmpFC);struct Cyc_List_List*_tmp101=Cyc_CfFlowInfo_join_relns(
_tmpFA,_tmpFD);void*_tmp104=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp103;_tmp103.tag=0;_tmp103.f1=Cyc_CfFlowInfo_escape_these(_tmpFF.pile,
all_changed,_tmp100);_tmp103.f2=_tmp101;_tmp103;});_tmp102;});_npop_handler(0);
return _tmp104;};_pop_region(rgn);}_LLAE:;}}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,struct _tagged_arr*,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){int changed1=env->changed == 
Cyc_CfFlowInfo_One?1:((int(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))
Cyc_Set_member)(env->chg1,env->curr_place);int changed2=env->changed == Cyc_CfFlowInfo_Two?
1:((int(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_member)(
env->chg2,env->curr_place);if(changed1?changed2: 0)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
if(changed1){if(!Cyc_CfFlowInfo_prefix_of_member(((env->joinenv).pile)->rgn,env->curr_place,
env->chg2))return r1;env->changed=Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(((
env->joinenv).pile)->rgn,env->curr_place,env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{
struct _tuple0 _tmp108=({struct _tuple0 _tmp107;_tmp107.f1=r1;_tmp107.f2=r2;_tmp107;});
void*_tmp109;struct Cyc_Dict_Dict*_tmp10A;void*_tmp10B;struct Cyc_Dict_Dict*
_tmp10C;_LLB6: _tmp109=_tmp108.f1;if(_tmp109 <= (void*)3?1:*((int*)_tmp109)!= 3)
goto _LLB8;_tmp10A=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp109)->f1;_tmp10B=
_tmp108.f2;if(_tmp10B <= (void*)3?1:*((int*)_tmp10B)!= 3)goto _LLB8;_tmp10C=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp10B)->f1;_LLB7: {struct Cyc_Dict_Dict*
_tmp10D=((struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_absRval_d,
env,_tmp10A,_tmp10C);if(_tmp10D == _tmp10A)return r1;if(_tmp10D == _tmp10C)return r2;
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp10E=_cycalloc(sizeof(*
_tmp10E));_tmp10E[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp10F;_tmp10F.tag=
3;_tmp10F.f1=_tmp10D;_tmp10F;});_tmp10E;});}_LLB8:;_LLB9:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110[0]=({
struct Cyc_Core_Impossible_struct _tmp111;_tmp111.tag=Cyc_Core_Impossible;_tmp111.f1=({
const char*_tmp112="after_pathinfo -- non-aggregates!";_tag_arr(_tmp112,sizeof(
char),_get_zero_arr_size(_tmp112,34));});_tmp111;});_tmp110;}));_LLB5:;}}static
void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,struct
_tagged_arr*key,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_List_List**
_tmp113=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged _tmp114=env->changed;*((
struct Cyc_List_List**)env->last_field_cell)=({struct Cyc_List_List*_tmp115=
_cycalloc(sizeof(*_tmp115));_tmp115->hd=key;_tmp115->tl=0;_tmp115;});env->last_field_cell=&((
struct Cyc_List_List*)_check_null(*((struct Cyc_List_List**)env->last_field_cell)))->tl;{
void*_tmp116=Cyc_CfFlowInfo_after_absRval(env,r1,r2);env->last_field_cell=
_tmp113;((struct Cyc_List_List*)_check_null(*((struct Cyc_List_List**)env->last_field_cell)))->tl=
0;env->changed=_tmp114;return _tmp116;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;*((
struct Cyc_CfFlowInfo_Place*)env->curr_place)=({struct Cyc_CfFlowInfo_Place _tmp117;
_tmp117.root=(void*)root;_tmp117.fields=0;_tmp117;});env->last_field_cell=&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
void*Cyc_CfFlowInfo_after_flow(struct Cyc_Set_Set**all_changed,void*f1,void*f2,
struct Cyc_Set_Set*chg1,struct Cyc_Set_Set*chg2){static struct Cyc_Absyn_Const_e_struct
dummy_rawexp={0,(void*)((void*)0)};static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((
void*)& dummy_rawexp),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_CfFlowInfo_MallocPt_struct
dummy_root={1,& dummy_exp,(void*)((void*)0)};if(f1 == f2)return f1;{struct _tuple0
_tmp119=({struct _tuple0 _tmp118;_tmp118.f1=f1;_tmp118.f2=f2;_tmp118;});void*
_tmp11A;void*_tmp11B;void*_tmp11C;struct Cyc_Dict_Dict*_tmp11D;struct Cyc_List_List*
_tmp11E;void*_tmp11F;struct Cyc_Dict_Dict*_tmp120;struct Cyc_List_List*_tmp121;
_LLBB: _tmp11A=_tmp119.f1;if((int)_tmp11A != 0)goto _LLBD;_LLBC: goto _LLBE;_LLBD:
_tmp11B=_tmp119.f2;if((int)_tmp11B != 0)goto _LLBF;_LLBE: return(void*)0;_LLBF:
_tmp11C=_tmp119.f1;if(_tmp11C <= (void*)1?1:*((int*)_tmp11C)!= 0)goto _LLBA;
_tmp11D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp11C)->f1;_tmp11E=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp11C)->f2;_tmp11F=_tmp119.f2;if(_tmp11F <= (
void*)1?1:*((int*)_tmp11F)!= 0)goto _LLBA;_tmp120=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp11F)->f1;_tmp121=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp11F)->f2;
_LLC0: if(_tmp11D == _tmp120?_tmp11E == _tmp121: 0)return f1;{struct _RegionHandle
_tmp122=_new_region("rgn");struct _RegionHandle*rgn=& _tmp122;_push_region(rgn);{
struct Cyc_CfFlowInfo_Place*_tmp123=({struct Cyc_CfFlowInfo_Place*_tmp12D=
_cycalloc(sizeof(*_tmp12D));_tmp12D->root=(void*)((void*)& dummy_root);_tmp12D->fields=
0;_tmp12D;});struct Cyc_CfFlowInfo_AfterEnv _tmp124=({struct Cyc_CfFlowInfo_AfterEnv
_tmp12A;_tmp12A.joinenv=({struct Cyc_CfFlowInfo_JoinEnv _tmp12B;_tmp12B.pile=({
struct Cyc_CfFlowInfo_EscPile*_tmp12C=_region_malloc(rgn,sizeof(*_tmp12C));
_tmp12C->rgn=rgn;_tmp12C->places=0;_tmp12C;});_tmp12B.d1=_tmp11D;_tmp12B.d2=
_tmp120;_tmp12B;});_tmp12A.chg1=chg1;_tmp12A.chg2=chg2;_tmp12A.curr_place=
_tmp123;_tmp12A.last_field_cell=& _tmp123->fields;_tmp12A.changed=Cyc_CfFlowInfo_Neither;
_tmp12A;});struct Cyc_Dict_Dict*_tmp125=((struct Cyc_Dict_Dict*(*)(void*(*f)(
struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp124,_tmp11D,_tmp120);struct Cyc_List_List*_tmp126=Cyc_CfFlowInfo_join_relns(
_tmp11E,_tmp121);void*_tmp129=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp128;_tmp128.tag=0;_tmp128.f1=Cyc_CfFlowInfo_escape_these((_tmp124.joinenv).pile,
all_changed,_tmp125);_tmp128.f2=_tmp126;_tmp128;});_tmp127;});_npop_handler(0);
return _tmp129;};_pop_region(rgn);}_LLBA:;}}static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp131=({struct
_tuple0 _tmp130;_tmp130.f1=r1;_tmp130.f2=r2;_tmp130;});void*_tmp132;struct Cyc_CfFlowInfo_Place*
_tmp133;void*_tmp134;struct Cyc_CfFlowInfo_Place*_tmp135;void*_tmp136;void*
_tmp137;void*_tmp138;struct Cyc_Dict_Dict*_tmp139;void*_tmp13A;struct Cyc_Dict_Dict*
_tmp13B;void*_tmp13C;void*_tmp13D;void*_tmp13E;void*_tmp13F;void*_tmp140;void*
_tmp141;_LLC2: _tmp132=_tmp131.f1;if(_tmp132 <= (void*)3?1:*((int*)_tmp132)!= 2)
goto _LLC4;_tmp133=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp132)->f1;_tmp134=
_tmp131.f2;if(_tmp134 <= (void*)3?1:*((int*)_tmp134)!= 2)goto _LLC4;_tmp135=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp134)->f1;_LLC3: return Cyc_CfFlowInfo_place_cmp(
_tmp133,_tmp135)== 0;_LLC4: _tmp136=_tmp131.f1;if(_tmp136 <= (void*)3?1:*((int*)
_tmp136)!= 2)goto _LLC6;_LLC5: goto _LLC7;_LLC6: _tmp137=_tmp131.f2;if(_tmp137 <= (
void*)3?1:*((int*)_tmp137)!= 2)goto _LLC8;_LLC7: return 0;_LLC8: _tmp138=_tmp131.f1;
if(_tmp138 <= (void*)3?1:*((int*)_tmp138)!= 3)goto _LLCA;_tmp139=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp138)->f1;_tmp13A=_tmp131.f2;if(_tmp13A <= (void*)3?1:*((int*)_tmp13A)!= 3)
goto _LLCA;_tmp13B=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp13A)->f1;_LLC9:
return _tmp139 == _tmp13B?1:((int(*)(int(*f)(struct _tagged_arr*,void*,void*),
struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_forall_intersect)((int(*)(
struct _tagged_arr*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp139,_tmp13B);_LLCA: _tmp13C=_tmp131.f2;if((int)_tmp13C != 2)goto _LLCC;_LLCB:
return r1 == (void*)1;_LLCC: _tmp13D=_tmp131.f2;if((int)_tmp13D != 0)goto _LLCE;_LLCD:
goto _LLCF;_LLCE: _tmp13E=_tmp131.f2;if((int)_tmp13E != 1)goto _LLD0;_LLCF: return 0;
_LLD0: _tmp13F=_tmp131.f1;if(_tmp13F <= (void*)3?1:*((int*)_tmp13F)!= 1)goto _LLD2;
_tmp140=_tmp131.f2;if(_tmp140 <= (void*)3?1:*((int*)_tmp140)!= 1)goto _LLD2;_LLD1:
goto _LLC1;_LLD2: _tmp141=_tmp131.f1;if(_tmp141 <= (void*)3?1:*((int*)_tmp141)!= 1)
goto _LLD4;_LLD3: return 0;_LLD4:;_LLD5: goto _LLC1;_LLC1:;}{struct _tuple0 _tmp143=({
struct _tuple0 _tmp142;_tmp142.f1=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp142.f2=
Cyc_CfFlowInfo_initlevel_approx(r2);_tmp142;});void*_tmp144;void*_tmp145;void*
_tmp146;void*_tmp147;void*_tmp148;void*_tmp149;_LLD7: _tmp144=_tmp143.f1;if((int)
_tmp144 != 2)goto _LLD9;_tmp145=_tmp143.f2;if((int)_tmp145 != 2)goto _LLD9;_LLD8:
return 1;_LLD9: _tmp146=_tmp143.f2;if((int)_tmp146 != 0)goto _LLDB;_LLDA: return 1;
_LLDB: _tmp147=_tmp143.f1;if((int)_tmp147 != 0)goto _LLDD;_LLDC: return 0;_LLDD:
_tmp148=_tmp143.f2;if((int)_tmp148 != 1)goto _LLDF;_LLDE: return 1;_LLDF: _tmp149=
_tmp143.f1;if((int)_tmp149 != 1)goto _LLD6;_LLE0: return 0;_LLD6:;}}int Cyc_CfFlowInfo_relns_approx(
struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s
!= 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp14A=(struct Cyc_CfFlowInfo_Reln*)
r1s->hd;int found=0;{struct Cyc_List_List*_tmp14B=r2s;for(0;_tmp14B != 0;_tmp14B=
_tmp14B->tl){struct Cyc_CfFlowInfo_Reln*_tmp14C=(struct Cyc_CfFlowInfo_Reln*)
_tmp14B->hd;if(_tmp14A == _tmp14C?1:(_tmp14A->vd == _tmp14C->vd?Cyc_CfFlowInfo_same_relop((
void*)_tmp14A->rop,(void*)_tmp14C->rop): 0)){found=1;break;}}}if(!found)return 0;}
return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(void*f1,void*f2){if(f1 == f2)
return 1;{struct _tuple0 _tmp14E=({struct _tuple0 _tmp14D;_tmp14D.f1=f1;_tmp14D.f2=f2;
_tmp14D;});void*_tmp14F;void*_tmp150;void*_tmp151;struct Cyc_Dict_Dict*_tmp152;
struct Cyc_List_List*_tmp153;void*_tmp154;struct Cyc_Dict_Dict*_tmp155;struct Cyc_List_List*
_tmp156;_LLE2: _tmp14F=_tmp14E.f1;if((int)_tmp14F != 0)goto _LLE4;_LLE3: return 1;
_LLE4: _tmp150=_tmp14E.f2;if((int)_tmp150 != 0)goto _LLE6;_LLE5: return 0;_LLE6:
_tmp151=_tmp14E.f1;if(_tmp151 <= (void*)1?1:*((int*)_tmp151)!= 0)goto _LLE1;
_tmp152=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp151)->f1;_tmp153=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp151)->f2;_tmp154=_tmp14E.f2;if(_tmp154 <= (
void*)1?1:*((int*)_tmp154)!= 0)goto _LLE1;_tmp155=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp154)->f1;_tmp156=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp154)->f2;
_LLE7: if(_tmp152 == _tmp155?_tmp153 == _tmp156: 0)return 1;return Cyc_Dict_forall_intersect(
Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp152,_tmp155)?Cyc_CfFlowInfo_relns_approx(
_tmp153,_tmp156): 0;_LLE1:;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=
0;for(p=rs;!found?p != 0: 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp157=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp157->vd == v){found=1;break;}{void*_tmp158=(void*)_tmp157->rop;
struct Cyc_Absyn_Vardecl*_tmp159;struct Cyc_Absyn_Vardecl*_tmp15A;struct Cyc_Absyn_Vardecl*
_tmp15B;_LLE9: if(*((int*)_tmp158)!= 1)goto _LLEB;_tmp159=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp158)->f1;_LLEA: _tmp15A=_tmp159;goto _LLEC;_LLEB: if(*((int*)_tmp158)!= 2)goto
_LLED;_tmp15A=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp158)->f1;_LLEC: _tmp15B=
_tmp15A;goto _LLEE;_LLED: if(*((int*)_tmp158)!= 4)goto _LLEF;_tmp15B=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp158)->f1;_LLEE: if(v == _tmp15B)found=1;goto _LLE8;_LLEF:;_LLF0: goto _LLE8;_LLE8:;}}
if(!found)return rs;{struct Cyc_List_List*_tmp15C=0;for(p=rs;p != 0;p=p->tl){struct
Cyc_CfFlowInfo_Reln*_tmp15D=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp15D->vd != v){{
void*_tmp15E=(void*)_tmp15D->rop;struct Cyc_Absyn_Vardecl*_tmp15F;struct Cyc_Absyn_Vardecl*
_tmp160;struct Cyc_Absyn_Vardecl*_tmp161;_LLF2: if(*((int*)_tmp15E)!= 1)goto _LLF4;
_tmp15F=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp15E)->f1;_LLF3: _tmp160=
_tmp15F;goto _LLF5;_LLF4: if(*((int*)_tmp15E)!= 2)goto _LLF6;_tmp160=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp15E)->f1;_LLF5: _tmp161=_tmp160;goto _LLF7;_LLF6: if(*((int*)_tmp15E)!= 4)goto
_LLF8;_tmp161=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp15E)->f1;_LLF7: if(v
== _tmp161)continue;goto _LLF1;_LLF8:;_LLF9: goto _LLF1;_LLF1:;}_tmp15C=({struct Cyc_List_List*
_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162->hd=_tmp15D;_tmp162->tl=_tmp15C;
_tmp162;});}}return _tmp15C;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*_tmp163=(void*)e->r;void*
_tmp164;struct Cyc_Absyn_Vardecl*_tmp165;void*_tmp166;struct Cyc_Absyn_Vardecl*
_tmp167;void*_tmp168;struct Cyc_Absyn_Vardecl*_tmp169;void*_tmp16A;struct Cyc_Absyn_Vardecl*
_tmp16B;_LLFB: if(*((int*)_tmp163)!= 1)goto _LLFD;_tmp164=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp163)->f2;if(_tmp164 <= (void*)1?1:*((int*)_tmp164)!= 0)goto _LLFD;_tmp165=((
struct Cyc_Absyn_Global_b_struct*)_tmp164)->f1;_LLFC: _tmp167=_tmp165;goto _LLFE;
_LLFD: if(*((int*)_tmp163)!= 1)goto _LLFF;_tmp166=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp163)->f2;if(_tmp166 <= (void*)1?1:*((int*)_tmp166)!= 2)goto _LLFF;_tmp167=((
struct Cyc_Absyn_Param_b_struct*)_tmp166)->f1;_LLFE: _tmp169=_tmp167;goto _LL100;
_LLFF: if(*((int*)_tmp163)!= 1)goto _LL101;_tmp168=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp163)->f2;if(_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 3)goto _LL101;_tmp169=((
struct Cyc_Absyn_Local_b_struct*)_tmp168)->f1;_LL100: _tmp16B=_tmp169;goto _LL102;
_LL101: if(*((int*)_tmp163)!= 1)goto _LL103;_tmp16A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp163)->f2;if(_tmp16A <= (void*)1?1:*((int*)_tmp16A)!= 4)goto _LL103;_tmp16B=((
struct Cyc_Absyn_Pat_b_struct*)_tmp16A)->f1;_LL102: if(!_tmp16B->escapes)return Cyc_CfFlowInfo_reln_kill_var(
r,_tmp16B);goto _LLFA;_LL103:;_LL104: goto _LLFA;_LLFA:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,
struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(r,v);{
void*_tmp16C=(void*)e->r;struct Cyc_Absyn_MallocInfo _tmp16D;struct Cyc_Absyn_Exp*
_tmp16E;int _tmp16F;_LL106: if(*((int*)_tmp16C)!= 33)goto _LL108;_tmp16D=((struct
Cyc_Absyn_Malloc_e_struct*)_tmp16C)->f1;_tmp16E=_tmp16D.num_elts;_tmp16F=_tmp16D.fat_result;
if(_tmp16F != 1)goto _LL108;_LL107: malloc_loop: {void*_tmp170=(void*)_tmp16E->r;
struct Cyc_Absyn_Exp*_tmp171;void*_tmp172;struct Cyc_Absyn_Vardecl*_tmp173;void*
_tmp174;struct Cyc_Absyn_Vardecl*_tmp175;void*_tmp176;struct Cyc_Absyn_Vardecl*
_tmp177;void*_tmp178;struct Cyc_Absyn_Vardecl*_tmp179;_LL10B: if(*((int*)_tmp170)
!= 13)goto _LL10D;_tmp171=((struct Cyc_Absyn_Cast_e_struct*)_tmp170)->f2;_LL10C:
_tmp16E=_tmp171;goto malloc_loop;_LL10D: if(*((int*)_tmp170)!= 1)goto _LL10F;
_tmp172=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp170)->f2;if(_tmp172 <= (void*)
1?1:*((int*)_tmp172)!= 4)goto _LL10F;_tmp173=((struct Cyc_Absyn_Pat_b_struct*)
_tmp172)->f1;_LL10E: _tmp175=_tmp173;goto _LL110;_LL10F: if(*((int*)_tmp170)!= 1)
goto _LL111;_tmp174=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp170)->f2;if(
_tmp174 <= (void*)1?1:*((int*)_tmp174)!= 3)goto _LL111;_tmp175=((struct Cyc_Absyn_Local_b_struct*)
_tmp174)->f1;_LL110: _tmp177=_tmp175;goto _LL112;_LL111: if(*((int*)_tmp170)!= 1)
goto _LL113;_tmp176=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp170)->f2;if(
_tmp176 <= (void*)1?1:*((int*)_tmp176)!= 2)goto _LL113;_tmp177=((struct Cyc_Absyn_Param_b_struct*)
_tmp176)->f1;_LL112: _tmp179=_tmp177;goto _LL114;_LL113: if(*((int*)_tmp170)!= 1)
goto _LL115;_tmp178=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp170)->f2;if(
_tmp178 <= (void*)1?1:*((int*)_tmp178)!= 0)goto _LL115;_tmp179=((struct Cyc_Absyn_Global_b_struct*)
_tmp178)->f1;_LL114: if(_tmp179->escapes)return r;return({struct Cyc_List_List*
_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->vd=_tmp179;_tmp17B->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp17C=_cycalloc(sizeof(*_tmp17C));
_tmp17C[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct _tmp17D;_tmp17D.tag=4;_tmp17D.f1=
v;_tmp17D;});_tmp17C;}));_tmp17B;});_tmp17A->tl=r;_tmp17A;});_LL115:;_LL116:
return r;_LL10A:;}_LL108:;_LL109: goto _LL105;_LL105:;}{void*_tmp17E=Cyc_Tcutil_compress((
void*)v->type);_LL118: if(_tmp17E <= (void*)3?1:*((int*)_tmp17E)!= 5)goto _LL11A;
_LL119: goto _LL117;_LL11A:;_LL11B: return r;_LL117:;}loop: {void*_tmp17F=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp180;void*_tmp181;int _tmp182;void*_tmp183;struct Cyc_List_List*
_tmp184;struct Cyc_List_List _tmp185;struct Cyc_List_List*_tmp186;struct Cyc_List_List
_tmp187;struct Cyc_Absyn_Exp*_tmp188;void*_tmp189;struct Cyc_List_List*_tmp18A;
struct Cyc_List_List _tmp18B;struct Cyc_Absyn_Exp*_tmp18C;_LL11D: if(*((int*)_tmp17F)
!= 13)goto _LL11F;_tmp180=((struct Cyc_Absyn_Cast_e_struct*)_tmp17F)->f2;_LL11E: e=
_tmp180;goto loop;_LL11F: if(*((int*)_tmp17F)!= 0)goto _LL121;_tmp181=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp17F)->f1;if(_tmp181 <= (void*)1?1:*((int*)
_tmp181)!= 2)goto _LL121;_tmp182=((struct Cyc_Absyn_Int_c_struct*)_tmp181)->f2;
_LL120: return({struct Cyc_List_List*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->vd=v;
_tmp18E->rop=(void*)((void*)({struct Cyc_CfFlowInfo_EqualConst_struct*_tmp18F=
_cycalloc_atomic(sizeof(*_tmp18F));_tmp18F[0]=({struct Cyc_CfFlowInfo_EqualConst_struct
_tmp190;_tmp190.tag=0;_tmp190.f1=(unsigned int)_tmp182;_tmp190;});_tmp18F;}));
_tmp18E;});_tmp18D->tl=r;_tmp18D;});_LL121: if(*((int*)_tmp17F)!= 3)goto _LL123;
_tmp183=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp17F)->f1;if((int)_tmp183 != 
4)goto _LL123;_tmp184=((struct Cyc_Absyn_Primop_e_struct*)_tmp17F)->f2;if(_tmp184
== 0)goto _LL123;_tmp185=*_tmp184;_tmp186=_tmp185.tl;if(_tmp186 == 0)goto _LL123;
_tmp187=*_tmp186;_tmp188=(struct Cyc_Absyn_Exp*)_tmp187.hd;_LL122:{void*_tmp191=(
void*)_tmp188->r;void*_tmp192;struct Cyc_List_List*_tmp193;struct Cyc_List_List
_tmp194;struct Cyc_Absyn_Exp*_tmp195;_LL128: if(*((int*)_tmp191)!= 3)goto _LL12A;
_tmp192=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp191)->f1;if((int)_tmp192 != 
19)goto _LL12A;_tmp193=((struct Cyc_Absyn_Primop_e_struct*)_tmp191)->f2;if(_tmp193
== 0)goto _LL12A;_tmp194=*_tmp193;_tmp195=(struct Cyc_Absyn_Exp*)_tmp194.hd;_LL129:{
void*_tmp196=(void*)_tmp195->r;void*_tmp197;struct Cyc_Absyn_Vardecl*_tmp198;void*
_tmp199;struct Cyc_Absyn_Vardecl*_tmp19A;void*_tmp19B;struct Cyc_Absyn_Vardecl*
_tmp19C;void*_tmp19D;struct Cyc_Absyn_Vardecl*_tmp19E;_LL12D: if(*((int*)_tmp196)
!= 1)goto _LL12F;_tmp197=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp196)->f2;if(
_tmp197 <= (void*)1?1:*((int*)_tmp197)!= 0)goto _LL12F;_tmp198=((struct Cyc_Absyn_Global_b_struct*)
_tmp197)->f1;_LL12E: _tmp19A=_tmp198;goto _LL130;_LL12F: if(*((int*)_tmp196)!= 1)
goto _LL131;_tmp199=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp196)->f2;if(
_tmp199 <= (void*)1?1:*((int*)_tmp199)!= 3)goto _LL131;_tmp19A=((struct Cyc_Absyn_Local_b_struct*)
_tmp199)->f1;_LL130: _tmp19C=_tmp19A;goto _LL132;_LL131: if(*((int*)_tmp196)!= 1)
goto _LL133;_tmp19B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp196)->f2;if(
_tmp19B <= (void*)1?1:*((int*)_tmp19B)!= 2)goto _LL133;_tmp19C=((struct Cyc_Absyn_Param_b_struct*)
_tmp19B)->f1;_LL132: _tmp19E=_tmp19C;goto _LL134;_LL133: if(*((int*)_tmp196)!= 1)
goto _LL135;_tmp19D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp196)->f2;if(
_tmp19D <= (void*)1?1:*((int*)_tmp19D)!= 4)goto _LL135;_tmp19E=((struct Cyc_Absyn_Pat_b_struct*)
_tmp19D)->f1;_LL134: if(_tmp19E->escapes)return r;return({struct Cyc_List_List*
_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0->vd=v;_tmp1A0->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_LessSize_struct*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1[
0]=({struct Cyc_CfFlowInfo_LessSize_struct _tmp1A2;_tmp1A2.tag=2;_tmp1A2.f1=
_tmp19E;_tmp1A2;});_tmp1A1;}));_tmp1A0;});_tmp19F->tl=r;_tmp19F;});_LL135:;
_LL136: goto _LL12C;_LL12C:;}goto _LL127;_LL12A:;_LL12B: goto _LL127;_LL127:;}goto
_LL11C;_LL123: if(*((int*)_tmp17F)!= 3)goto _LL125;_tmp189=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp17F)->f1;if((int)_tmp189 != 19)goto _LL125;_tmp18A=((struct Cyc_Absyn_Primop_e_struct*)
_tmp17F)->f2;if(_tmp18A == 0)goto _LL125;_tmp18B=*_tmp18A;_tmp18C=(struct Cyc_Absyn_Exp*)
_tmp18B.hd;_LL124:{void*_tmp1A3=(void*)_tmp18C->r;void*_tmp1A4;struct Cyc_Absyn_Vardecl*
_tmp1A5;void*_tmp1A6;struct Cyc_Absyn_Vardecl*_tmp1A7;void*_tmp1A8;struct Cyc_Absyn_Vardecl*
_tmp1A9;void*_tmp1AA;struct Cyc_Absyn_Vardecl*_tmp1AB;_LL138: if(*((int*)_tmp1A3)
!= 1)goto _LL13A;_tmp1A4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1A3)->f2;if(
_tmp1A4 <= (void*)1?1:*((int*)_tmp1A4)!= 0)goto _LL13A;_tmp1A5=((struct Cyc_Absyn_Global_b_struct*)
_tmp1A4)->f1;_LL139: _tmp1A7=_tmp1A5;goto _LL13B;_LL13A: if(*((int*)_tmp1A3)!= 1)
goto _LL13C;_tmp1A6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1A3)->f2;if(
_tmp1A6 <= (void*)1?1:*((int*)_tmp1A6)!= 3)goto _LL13C;_tmp1A7=((struct Cyc_Absyn_Local_b_struct*)
_tmp1A6)->f1;_LL13B: _tmp1A9=_tmp1A7;goto _LL13D;_LL13C: if(*((int*)_tmp1A3)!= 1)
goto _LL13E;_tmp1A8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1A3)->f2;if(
_tmp1A8 <= (void*)1?1:*((int*)_tmp1A8)!= 2)goto _LL13E;_tmp1A9=((struct Cyc_Absyn_Param_b_struct*)
_tmp1A8)->f1;_LL13D: _tmp1AB=_tmp1A9;goto _LL13F;_LL13E: if(*((int*)_tmp1A3)!= 1)
goto _LL140;_tmp1AA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1A3)->f2;if(
_tmp1AA <= (void*)1?1:*((int*)_tmp1AA)!= 4)goto _LL140;_tmp1AB=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1AA)->f1;_LL13F: if(_tmp1AB->escapes)return r;return({struct Cyc_List_List*
_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD->vd=v;_tmp1AD->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));
_tmp1AE[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct _tmp1AF;_tmp1AF.tag=4;_tmp1AF.f1=
_tmp1AB;_tmp1AF;});_tmp1AE;}));_tmp1AD;});_tmp1AC->tl=r;_tmp1AC;});_LL140:;
_LL141: goto _LL137;_LL137:;}goto _LL11C;_LL125:;_LL126: goto _LL11C;_LL11C:;}return r;}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct Cyc_List_List*r,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*_tmp1B0=(void*)e1->r;void*_tmp1B1;
struct Cyc_Absyn_Vardecl*_tmp1B2;void*_tmp1B3;struct Cyc_Absyn_Vardecl*_tmp1B4;
void*_tmp1B5;struct Cyc_Absyn_Vardecl*_tmp1B6;void*_tmp1B7;struct Cyc_Absyn_Vardecl*
_tmp1B8;_LL143: if(*((int*)_tmp1B0)!= 1)goto _LL145;_tmp1B1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1B0)->f2;if(_tmp1B1 <= (void*)1?1:*((int*)_tmp1B1)!= 0)goto _LL145;_tmp1B2=((
struct Cyc_Absyn_Global_b_struct*)_tmp1B1)->f1;_LL144: _tmp1B4=_tmp1B2;goto _LL146;
_LL145: if(*((int*)_tmp1B0)!= 1)goto _LL147;_tmp1B3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1B0)->f2;if(_tmp1B3 <= (void*)1?1:*((int*)_tmp1B3)!= 2)goto _LL147;_tmp1B4=((
struct Cyc_Absyn_Param_b_struct*)_tmp1B3)->f1;_LL146: _tmp1B6=_tmp1B4;goto _LL148;
_LL147: if(*((int*)_tmp1B0)!= 1)goto _LL149;_tmp1B5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1B0)->f2;if(_tmp1B5 <= (void*)1?1:*((int*)_tmp1B5)!= 3)goto _LL149;_tmp1B6=((
struct Cyc_Absyn_Local_b_struct*)_tmp1B5)->f1;_LL148: _tmp1B8=_tmp1B6;goto _LL14A;
_LL149: if(*((int*)_tmp1B0)!= 1)goto _LL14B;_tmp1B7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1B0)->f2;if(_tmp1B7 <= (void*)1?1:*((int*)_tmp1B7)!= 4)goto _LL14B;_tmp1B8=((
struct Cyc_Absyn_Pat_b_struct*)_tmp1B7)->f1;_LL14A: if(!_tmp1B8->escapes)return Cyc_CfFlowInfo_reln_assign_var(
r,_tmp1B8,e2);goto _LL142;_LL14B:;_LL14C: goto _LL142;_LL142:;}return r;}void Cyc_CfFlowInfo_print_reln(
struct Cyc_CfFlowInfo_Reln*r){({struct Cyc_Std_String_pa_struct _tmp1BB;_tmp1BB.tag=
0;_tmp1BB.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((r->vd)->name);{void*
_tmp1B9[1]={& _tmp1BB};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp1BA="%s";
_tag_arr(_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,3));}),_tag_arr(_tmp1B9,
sizeof(void*),1));}});{void*_tmp1BC=(void*)r->rop;unsigned int _tmp1BD;struct Cyc_Absyn_Vardecl*
_tmp1BE;struct Cyc_Absyn_Vardecl*_tmp1BF;unsigned int _tmp1C0;struct Cyc_Absyn_Vardecl*
_tmp1C1;_LL14E: if(*((int*)_tmp1BC)!= 0)goto _LL150;_tmp1BD=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmp1BC)->f1;_LL14F:({struct Cyc_Std_Int_pa_struct _tmp1C4;_tmp1C4.tag=1;_tmp1C4.f1=(
int)_tmp1BD;{void*_tmp1C2[1]={& _tmp1C4};Cyc_Std_fprintf(Cyc_Std_stderr,({const
char*_tmp1C3="==%d";_tag_arr(_tmp1C3,sizeof(char),_get_zero_arr_size(_tmp1C3,5));}),
_tag_arr(_tmp1C2,sizeof(void*),1));}});goto _LL14D;_LL150: if(*((int*)_tmp1BC)!= 1)
goto _LL152;_tmp1BE=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1BC)->f1;_LL151:({
struct Cyc_Std_String_pa_struct _tmp1C7;_tmp1C7.tag=0;_tmp1C7.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp1BE->name);{void*_tmp1C5[1]={& _tmp1C7};
Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp1C6="<%s";_tag_arr(_tmp1C6,
sizeof(char),_get_zero_arr_size(_tmp1C6,4));}),_tag_arr(_tmp1C5,sizeof(void*),1));}});
goto _LL14D;_LL152: if(*((int*)_tmp1BC)!= 2)goto _LL154;_tmp1BF=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp1BC)->f1;_LL153:({struct Cyc_Std_String_pa_struct _tmp1CA;_tmp1CA.tag=0;
_tmp1CA.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1BF->name);{void*
_tmp1C8[1]={& _tmp1CA};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp1C9="<%s.size";
_tag_arr(_tmp1C9,sizeof(char),_get_zero_arr_size(_tmp1C9,9));}),_tag_arr(_tmp1C8,
sizeof(void*),1));}});goto _LL14D;_LL154: if(*((int*)_tmp1BC)!= 3)goto _LL156;
_tmp1C0=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp1BC)->f1;_LL155:({struct Cyc_Std_Int_pa_struct
_tmp1CD;_tmp1CD.tag=1;_tmp1CD.f1=(int)_tmp1C0;{void*_tmp1CB[1]={& _tmp1CD};Cyc_Std_fprintf(
Cyc_Std_stderr,({const char*_tmp1CC="<%d";_tag_arr(_tmp1CC,sizeof(char),
_get_zero_arr_size(_tmp1CC,4));}),_tag_arr(_tmp1CB,sizeof(void*),1));}});goto
_LL14D;_LL156: if(*((int*)_tmp1BC)!= 4)goto _LL14D;_tmp1C1=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp1BC)->f1;_LL157:({struct Cyc_Std_String_pa_struct _tmp1D0;_tmp1D0.tag=0;
_tmp1D0.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1C1->name);{void*
_tmp1CE[1]={& _tmp1D0};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp1CF="<=%s.size";
_tag_arr(_tmp1CF,sizeof(char),_get_zero_arr_size(_tmp1CF,10));}),_tag_arr(
_tmp1CE,sizeof(void*),1));}});goto _LL14D;_LL14D:;}}void Cyc_CfFlowInfo_print_relns(
struct Cyc_List_List*r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)
r->hd);if(r->tl != 0)({void*_tmp1D1[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const
char*_tmp1D2=",";_tag_arr(_tmp1D2,sizeof(char),_get_zero_arr_size(_tmp1D2,2));}),
_tag_arr(_tmp1D1,sizeof(void*),0));});}}static int Cyc_CfFlowInfo_contains_region(
struct Cyc_Absyn_Tvar*rgn,void*t){void*_tmp1D3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp1D4;struct Cyc_Absyn_TunionInfo _tmp1D5;struct Cyc_List_List*_tmp1D6;void*
_tmp1D7;struct Cyc_List_List*_tmp1D8;struct Cyc_Absyn_AggrInfo _tmp1D9;struct Cyc_List_List*
_tmp1DA;struct Cyc_Absyn_TunionFieldInfo _tmp1DB;struct Cyc_List_List*_tmp1DC;
struct Cyc_List_List*_tmp1DD;struct Cyc_Absyn_PtrInfo _tmp1DE;void*_tmp1DF;void*
_tmp1E0;struct Cyc_List_List*_tmp1E1;struct Cyc_List_List*_tmp1E2;struct Cyc_Absyn_ArrayInfo
_tmp1E3;void*_tmp1E4;void*_tmp1E5;void*_tmp1E6;void*_tmp1E7;_LL159: if((int)
_tmp1D3 != 0)goto _LL15B;_LL15A: goto _LL15C;_LL15B: if(_tmp1D3 <= (void*)3?1:*((int*)
_tmp1D3)!= 5)goto _LL15D;_LL15C: goto _LL15E;_LL15D: if((int)_tmp1D3 != 1)goto _LL15F;
_LL15E: goto _LL160;_LL15F: if(_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 6)goto _LL161;
_LL160: goto _LL162;_LL161: if(_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 12)goto
_LL163;_LL162: goto _LL164;_LL163: if(_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 13)
goto _LL165;_LL164: goto _LL166;_LL165: if(_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 
14)goto _LL167;_LL166: goto _LL168;_LL167: if(_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)
!= 17)goto _LL169;_LL168: goto _LL16A;_LL169: if(_tmp1D3 <= (void*)3?1:*((int*)
_tmp1D3)!= 18)goto _LL16B;_LL16A: goto _LL16C;_LL16B: if((int)_tmp1D3 != 2)goto _LL16D;
_LL16C: goto _LL16E;_LL16D: if(_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 0)goto _LL16F;
_LL16E: return 0;_LL16F: if(_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 1)goto _LL171;
_tmp1D4=((struct Cyc_Absyn_VarType_struct*)_tmp1D3)->f1;_LL170: return Cyc_Absyn_tvar_cmp(
_tmp1D4,rgn)== 0;_LL171: if(_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 2)goto _LL173;
_tmp1D5=((struct Cyc_Absyn_TunionType_struct*)_tmp1D3)->f1;_tmp1D6=_tmp1D5.targs;
_tmp1D7=(void*)_tmp1D5.rgn;_LL172: if(Cyc_CfFlowInfo_contains_region(rgn,_tmp1D7))
return 1;_tmp1D8=_tmp1D6;goto _LL174;_LL173: if(_tmp1D3 <= (void*)3?1:*((int*)
_tmp1D3)!= 16)goto _LL175;_tmp1D8=((struct Cyc_Absyn_TypedefType_struct*)_tmp1D3)->f2;
_LL174: _tmp1DA=_tmp1D8;goto _LL176;_LL175: if(_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)
!= 10)goto _LL177;_tmp1D9=((struct Cyc_Absyn_AggrType_struct*)_tmp1D3)->f1;_tmp1DA=
_tmp1D9.targs;_LL176: _tmp1DC=_tmp1DA;goto _LL178;_LL177: if(_tmp1D3 <= (void*)3?1:*((
int*)_tmp1D3)!= 3)goto _LL179;_tmp1DB=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp1D3)->f1;_tmp1DC=_tmp1DB.targs;_LL178: _tmp1DD=_tmp1DC;goto _LL17A;_LL179: if(
_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 20)goto _LL17B;_tmp1DD=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1D3)->f1;_LL17A: return((int(*)(int(*pred)(struct Cyc_Absyn_Tvar*,void*),
struct Cyc_Absyn_Tvar*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,
rgn,_tmp1DD);_LL17B: if(_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 4)goto _LL17D;
_tmp1DE=((struct Cyc_Absyn_PointerType_struct*)_tmp1D3)->f1;_tmp1DF=(void*)
_tmp1DE.elt_typ;_tmp1E0=(void*)_tmp1DE.rgn_typ;_LL17C: return Cyc_CfFlowInfo_contains_region(
rgn,_tmp1E0)?1: Cyc_CfFlowInfo_contains_region(rgn,_tmp1DF);_LL17D: if(_tmp1D3 <= (
void*)3?1:*((int*)_tmp1D3)!= 8)goto _LL17F;_LL17E: return 0;_LL17F: if(_tmp1D3 <= (
void*)3?1:*((int*)_tmp1D3)!= 9)goto _LL181;_tmp1E1=((struct Cyc_Absyn_TupleType_struct*)
_tmp1D3)->f1;_LL180: for(0;_tmp1E1 != 0;_tmp1E1=_tmp1E1->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(*((struct _tuple4*)((struct _tuple4*)_tmp1E1->hd))).f2))return 1;}return 0;
_LL181: if(_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 11)goto _LL183;_tmp1E2=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp1D3)->f2;_LL182: for(0;_tmp1E2 != 0;_tmp1E2=
_tmp1E2->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmp1E2->hd)->type))return 1;}return 0;_LL183: if(_tmp1D3 <= (void*)3?1:*((int*)
_tmp1D3)!= 7)goto _LL185;_tmp1E3=((struct Cyc_Absyn_ArrayType_struct*)_tmp1D3)->f1;
_tmp1E4=(void*)_tmp1E3.elt_type;_LL184: _tmp1E5=_tmp1E4;goto _LL186;_LL185: if(
_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 19)goto _LL187;_tmp1E5=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1D3)->f1;_LL186: _tmp1E6=_tmp1E5;goto _LL188;_LL187: if(_tmp1D3 <= (void*)3?1:*((
int*)_tmp1D3)!= 21)goto _LL189;_tmp1E6=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1D3)->f1;_LL188: return Cyc_CfFlowInfo_contains_region(rgn,_tmp1E6);_LL189: if(
_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 15)goto _LL158;_tmp1E7=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp1D3)->f1;_LL18A: return 0;_LL158:;}struct _tuple6{struct Cyc_Dict_Dict*f1;struct
Cyc_Absyn_Tvar*f2;};static void Cyc_CfFlowInfo_kill_root(struct _tuple6*env,void*
root,void*rval){struct _tuple6 _tmp1E9;struct Cyc_Dict_Dict*_tmp1EA;struct Cyc_Dict_Dict**
_tmp1EB;struct Cyc_Absyn_Tvar*_tmp1EC;struct _tuple6*_tmp1E8=env;_tmp1E9=*_tmp1E8;
_tmp1EA=_tmp1E9.f1;_tmp1EB=(struct Cyc_Dict_Dict**)&(*_tmp1E8).f1;_tmp1EC=_tmp1E9.f2;{
void*_tmp1ED=root;struct Cyc_Absyn_Vardecl*_tmp1EE;void*_tmp1EF;_LL18C: if(*((int*)
_tmp1ED)!= 0)goto _LL18E;_tmp1EE=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp1ED)->f1;
_LL18D: if(Cyc_CfFlowInfo_contains_region(_tmp1EC,(void*)_tmp1EE->type))rval=Cyc_CfFlowInfo_typ_to_absrval((
void*)_tmp1EE->type,Cyc_CfFlowInfo_unknown_none);*((struct Cyc_Dict_Dict**)
_tmp1EB)=Cyc_Dict_insert(*((struct Cyc_Dict_Dict**)_tmp1EB),root,rval);goto _LL18B;
_LL18E: if(*((int*)_tmp1ED)!= 1)goto _LL190;_tmp1EF=(void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp1ED)->f2;_LL18F: if(!Cyc_CfFlowInfo_contains_region(_tmp1EC,_tmp1EF))*((
struct Cyc_Dict_Dict**)_tmp1EB)=Cyc_Dict_insert(*((struct Cyc_Dict_Dict**)_tmp1EB),
root,rval);goto _LL18B;_LL190: if(*((int*)_tmp1ED)!= 2)goto _LL18B;_LL191: goto
_LL18B;_LL18B:;}}static struct Cyc_Dict_Dict*Cyc_CfFlowInfo_kill_flowdict_region(
struct Cyc_Dict_Dict*fd,void*rgn){struct Cyc_Absyn_Tvar*rgn_tvar;{void*_tmp1F0=Cyc_Tcutil_compress(
rgn);struct Cyc_Absyn_Tvar*_tmp1F1;_LL193: if(_tmp1F0 <= (void*)3?1:*((int*)_tmp1F0)
!= 1)goto _LL195;_tmp1F1=((struct Cyc_Absyn_VarType_struct*)_tmp1F0)->f1;_LL194:
rgn_tvar=_tmp1F1;goto _LL192;_LL195:;_LL196:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2[0]=({struct Cyc_Core_Impossible_struct
_tmp1F3;_tmp1F3.tag=Cyc_Core_Impossible;_tmp1F3.f1=({const char*_tmp1F4="kill_flowdict_region";
_tag_arr(_tmp1F4,sizeof(char),_get_zero_arr_size(_tmp1F4,21));});_tmp1F3;});
_tmp1F2;}));goto _LL192;_LL192:;}{struct _tuple6 env=({struct _tuple6 _tmp1F5;_tmp1F5.f1=
Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp1F5.f2=rgn_tvar;_tmp1F5;});((void(*)(
void(*f)(struct _tuple6*,void*,void*),struct _tuple6*env,struct Cyc_Dict_Dict*d))
Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return env.f1;}}static struct
Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*relns,void*
rgn){return 0;}void*Cyc_CfFlowInfo_kill_flow_region(void*f,void*rgn){void*_tmp1F6=
f;struct Cyc_Dict_Dict*_tmp1F7;struct Cyc_List_List*_tmp1F8;_LL198: if((int)_tmp1F6
!= 0)goto _LL19A;_LL199: return f;_LL19A: if(_tmp1F6 <= (void*)1?1:*((int*)_tmp1F6)!= 
0)goto _LL197;_tmp1F7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1F6)->f1;
_tmp1F8=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1F6)->f2;_LL19B: {struct
Cyc_Dict_Dict*_tmp1F9=Cyc_CfFlowInfo_kill_flowdict_region(_tmp1F7,rgn);struct Cyc_List_List*
_tmp1FA=Cyc_CfFlowInfo_kill_relns_region(_tmp1F8,rgn);return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1FB=_cycalloc(sizeof(*_tmp1FB));_tmp1FB[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp1FC;_tmp1FC.tag=0;_tmp1FC.f1=_tmp1F9;_tmp1FC.f2=_tmp1FA;_tmp1FC;});_tmp1FB;});}
_LL197:;}
