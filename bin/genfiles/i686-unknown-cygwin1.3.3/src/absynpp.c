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
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
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
#ifdef NO_CYC_BOUNDS_CHECK
#define _zero_arr_plus(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#else
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
#endif

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
 struct Cyc_Core_Opt{void*v;};struct _tagged_arr Cyc_Core_new_string(unsigned int);
extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc___cycFILE;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _tagged_arr,struct
_tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2);extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*
f);struct _tagged_arr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*
Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();struct Cyc_PP_Doc*Cyc_PP_line_doc();
struct Cyc_PP_Doc*Cyc_PP_text(struct _tagged_arr s);struct Cyc_PP_Doc*Cyc_PP_textptr(
struct _tagged_arr*p);struct Cyc_PP_Doc*Cyc_PP_text_width(struct _tagged_arr s,int w);
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*Cyc_PP_cat(
struct _tagged_arr);struct Cyc_PP_Doc*Cyc_PP_seq(struct _tagged_arr sep,struct Cyc_List_List*
l);struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr
sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_seql(struct _tagged_arr sep,
struct Cyc_List_List*l0);struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(
void*),struct _tagged_arr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_group(
struct _tagged_arr start,struct _tagged_arr stop,struct _tagged_arr sep,struct Cyc_List_List*
l);struct Cyc_PP_Doc*Cyc_PP_egroup(struct _tagged_arr start,struct _tagged_arr stop,
struct _tagged_arr sep,struct Cyc_List_List*l);struct Cyc_Lineno_Pos{struct
_tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[
14];struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};
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
_tuple0*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*
tunion_name;struct _tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
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
int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
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
};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Pat{void*r;
struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*cnst_exp;struct Cyc_Absyn_Stmt*body;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Aggrfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;};struct Cyc_Absyn_Aggrdecl{void*kind;void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _tagged_arr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{
void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct _tagged_arr Cyc_Absyn_attribute2string(
void*);struct _tuple3{void*f1;struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_Buffer_t;unsigned int Cyc_strlen(struct _tagged_arr s);int Cyc_strptrcmp(
struct _tagged_arr*s1,struct _tagged_arr*s2);struct _tagged_arr Cyc_strconcat(struct
_tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_str_sepstr(struct Cyc_List_List*,
struct _tagged_arr);struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char
Cyc_Dict_Absent[11];struct _tuple5{void*f1;void*f2;};struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern int Cyc_Absynpp_print_scopes;void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct _tagged_arr
Cyc_Absynpp_typ2string(void*);struct _tagged_arr Cyc_Absynpp_typ2cstring(void*);
struct _tagged_arr Cyc_Absynpp_kind2string(void*);struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*);struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_tagged_arr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct _tagged_arr Cyc_Absynpp_decllist2string(struct Cyc_List_List*
tdl);struct _tagged_arr Cyc_Absynpp_prim2string(void*p);struct _tagged_arr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p);struct _tagged_arr Cyc_Absynpp_scope2string(void*sc);int Cyc_Absynpp_is_anon_aggrtype(
void*t);extern struct _tagged_arr Cyc_Absynpp_cyc_string;extern struct _tagged_arr*
Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);struct
_tagged_arr Cyc_Absynpp_char_escape(char);struct _tagged_arr Cyc_Absynpp_string_escape(
struct _tagged_arr);struct _tagged_arr Cyc_Absynpp_prim2str(void*p);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);struct _tuple6{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*arg);struct
_tuple7{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple7
Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t);struct _tuple8{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_switchCclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es);
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*);struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,
void*p,struct Cyc_List_List*es);struct _tuple9{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*dopt);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct
Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd);static int Cyc_Absynpp_expand_typedefs;static int Cyc_Absynpp_qvar_to_Cids;
static char _tmp0[4]="Cyc";struct _tagged_arr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,
_tmp0 + 4};struct _tagged_arr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
static int Cyc_Absynpp_add_cyc_prefix;static int Cyc_Absynpp_to_VC;static int Cyc_Absynpp_decls_first;
static int Cyc_Absynpp_rewrite_temp_tvars;static int Cyc_Absynpp_print_all_tvars;
static int Cyc_Absynpp_print_all_kinds;static int Cyc_Absynpp_print_using_stmts;
static int Cyc_Absynpp_print_externC_stmts;static int Cyc_Absynpp_print_full_evars;
static int Cyc_Absynpp_generate_line_directives;static int Cyc_Absynpp_use_curr_namespace;
static int Cyc_Absynpp_print_zeroterm;static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=
0;struct Cyc_Absynpp_Params;void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*
fs){Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=
fs->decls_first;Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;Cyc_Absynpp_print_using_stmts=
fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_print_zeroterm=fs->print_zeroterm;
Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;Cyc_Absynpp_use_curr_namespace=
fs->use_curr_namespace;Cyc_Absynpp_curr_namespace=fs->curr_namespace;}struct Cyc_Absynpp_Params
Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params
Cyc_Absynpp_cyci_params_r={1,0,0,0,0,1,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params
Cyc_Absynpp_tc_params_r={0,0,0,0,0,0,0,0,1,1,0,1,0,0,0};static void Cyc_Absynpp_curr_namespace_add(
struct _tagged_arr*v){Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v;_tmp1->tl=0;
_tmp1;}));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){if(((struct
Cyc_List_List*)_check_null(*((struct Cyc_List_List**)l)))->tl == 0)*((struct Cyc_List_List**)
l)=0;else{Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*((struct
Cyc_List_List**)l)))->tl);}}static void Cyc_Absynpp_curr_namespace_drop(){((void(*)(
struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _tagged_arr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0: return({
const char*_tmp2="\\a";_tag_arr(_tmp2,sizeof(char),_get_zero_arr_size(_tmp2,3));});
case '\b': _LL1: return({const char*_tmp3="\\b";_tag_arr(_tmp3,sizeof(char),
_get_zero_arr_size(_tmp3,3));});case '\f': _LL2: return({const char*_tmp4="\\f";
_tag_arr(_tmp4,sizeof(char),_get_zero_arr_size(_tmp4,3));});case '\n': _LL3: return({
const char*_tmp5="\\n";_tag_arr(_tmp5,sizeof(char),_get_zero_arr_size(_tmp5,3));});
case '\r': _LL4: return({const char*_tmp6="\\r";_tag_arr(_tmp6,sizeof(char),
_get_zero_arr_size(_tmp6,3));});case '\t': _LL5: return({const char*_tmp7="\\t";
_tag_arr(_tmp7,sizeof(char),_get_zero_arr_size(_tmp7,3));});case '\v': _LL6: return({
const char*_tmp8="\\v";_tag_arr(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,3));});
case '\\': _LL7: return({const char*_tmp9="\\\\";_tag_arr(_tmp9,sizeof(char),
_get_zero_arr_size(_tmp9,3));});case '"': _LL8: return({const char*_tmpA="\"";
_tag_arr(_tmpA,sizeof(char),_get_zero_arr_size(_tmpA,2));});case '\'': _LL9: return({
const char*_tmpB="\\'";_tag_arr(_tmpB,sizeof(char),_get_zero_arr_size(_tmpB,3));});
default: _LLA: if(c >= ' '?c <= '~': 0){struct _tagged_arr _tmpC=Cyc_Core_new_string(2);({
struct _tagged_arr _tmpD=_tagged_arr_plus(_tmpC,sizeof(char),0);char _tmpE=*((char*)
_check_unknown_subscript(_tmpD,sizeof(char),0));char _tmpF=c;if(_get_arr_size(
_tmpD,sizeof(char))== 1?_tmpE == '\000'?_tmpF != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmpD.curr)=_tmpF;});return(struct _tagged_arr)_tmpC;}else{struct
_tagged_arr _tmp10=Cyc_Core_new_string(5);int j=0;({struct _tagged_arr _tmp11=
_tagged_arr_plus(_tmp10,sizeof(char),j ++);char _tmp12=*((char*)
_check_unknown_subscript(_tmp11,sizeof(char),0));char _tmp13='\\';if(
_get_arr_size(_tmp11,sizeof(char))== 1?_tmp12 == '\000'?_tmp13 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp11.curr)=_tmp13;});({struct _tagged_arr _tmp14=
_tagged_arr_plus(_tmp10,sizeof(char),j ++);char _tmp15=*((char*)
_check_unknown_subscript(_tmp14,sizeof(char),0));char _tmp16=(char)('0' + ((
unsigned char)c >> 6 & 3));if(_get_arr_size(_tmp14,sizeof(char))== 1?_tmp15 == '\000'?
_tmp16 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp14.curr)=_tmp16;});({
struct _tagged_arr _tmp17=_tagged_arr_plus(_tmp10,sizeof(char),j ++);char _tmp18=*((
char*)_check_unknown_subscript(_tmp17,sizeof(char),0));char _tmp19=(char)('0' + (c
>> 3 & 7));if(_get_arr_size(_tmp17,sizeof(char))== 1?_tmp18 == '\000'?_tmp19 != '\000':
0: 0)_throw_arraybounds();*((char*)_tmp17.curr)=_tmp19;});({struct _tagged_arr
_tmp1A=_tagged_arr_plus(_tmp10,sizeof(char),j ++);char _tmp1B=*((char*)
_check_unknown_subscript(_tmp1A,sizeof(char),0));char _tmp1C=(char)('0' + (c & 7));
if(_get_arr_size(_tmp1A,sizeof(char))== 1?_tmp1B == '\000'?_tmp1C != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp1A.curr)=_tmp1C;});return(struct _tagged_arr)
_tmp10;}}}static int Cyc_Absynpp_special(struct _tagged_arr s){int sz=(int)(
_get_arr_size(s,sizeof(char))- 1);{int i=0;for(0;i < sz;i ++){char c=*((const char*)
_check_unknown_subscript(s,sizeof(char),i));if(((c <= ' '?1: c >= '~')?1: c == '"')?1:
c == '\\')return 1;}}return 0;}struct _tagged_arr Cyc_Absynpp_string_escape(struct
_tagged_arr s){if(!Cyc_Absynpp_special(s))return s;{int n=(int)(_get_arr_size(s,
sizeof(char))- 1);if(n > 0?*((const char*)_check_unknown_subscript(s,sizeof(char),
n))== '\000': 0)n --;{int len=0;{int i=0;for(0;i <= n;i ++){char _tmp1D=*((const char*)
_check_unknown_subscript(s,sizeof(char),i));_LLD: if(_tmp1D != '\a')goto _LLF;_LLE:
goto _LL10;_LLF: if(_tmp1D != '\b')goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')
goto _LL13;_LL12: goto _LL14;_LL13: if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15:
if(_tmp1D != '\r')goto _LL17;_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;
_LL18: goto _LL1A;_LL19: if(_tmp1D != '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D
!= '\\')goto _LL1D;_LL1C: goto _LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;
goto _LLC;_LL1F:;_LL20: if(_tmp1D >= ' '?_tmp1D <= '~': 0)len ++;else{len +=4;}goto _LLC;
_LLC:;}}{struct _tagged_arr t=Cyc_Core_new_string((unsigned int)(len + 1));int j=0;{
int i=0;for(0;i <= n;i ++){char _tmp1E=*((const char*)_check_unknown_subscript(s,
sizeof(char),i));_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:({struct _tagged_arr
_tmp1F=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp20=*((char*)
_check_unknown_subscript(_tmp1F,sizeof(char),0));char _tmp21='\\';if(
_get_arr_size(_tmp1F,sizeof(char))== 1?_tmp20 == '\000'?_tmp21 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp1F.curr)=_tmp21;});({struct _tagged_arr _tmp22=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp23=*((char*)
_check_unknown_subscript(_tmp22,sizeof(char),0));char _tmp24='a';if(_get_arr_size(
_tmp22,sizeof(char))== 1?_tmp23 == '\000'?_tmp24 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp22.curr)=_tmp24;});goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:({
struct _tagged_arr _tmp25=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp26=*((char*)
_check_unknown_subscript(_tmp25,sizeof(char),0));char _tmp27='\\';if(
_get_arr_size(_tmp25,sizeof(char))== 1?_tmp26 == '\000'?_tmp27 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp25.curr)=_tmp27;});({struct _tagged_arr _tmp28=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp29=*((char*)
_check_unknown_subscript(_tmp28,sizeof(char),0));char _tmp2A='b';if(_get_arr_size(
_tmp28,sizeof(char))== 1?_tmp29 == '\000'?_tmp2A != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp28.curr)=_tmp2A;});goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:({
struct _tagged_arr _tmp2B=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp2C=*((char*)
_check_unknown_subscript(_tmp2B,sizeof(char),0));char _tmp2D='\\';if(
_get_arr_size(_tmp2B,sizeof(char))== 1?_tmp2C == '\000'?_tmp2D != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp2B.curr)=_tmp2D;});({struct _tagged_arr _tmp2E=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp2F=*((char*)
_check_unknown_subscript(_tmp2E,sizeof(char),0));char _tmp30='f';if(_get_arr_size(
_tmp2E,sizeof(char))== 1?_tmp2F == '\000'?_tmp30 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp2E.curr)=_tmp30;});goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:({
struct _tagged_arr _tmp31=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp32=*((char*)
_check_unknown_subscript(_tmp31,sizeof(char),0));char _tmp33='\\';if(
_get_arr_size(_tmp31,sizeof(char))== 1?_tmp32 == '\000'?_tmp33 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp31.curr)=_tmp33;});({struct _tagged_arr _tmp34=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp35=*((char*)
_check_unknown_subscript(_tmp34,sizeof(char),0));char _tmp36='n';if(_get_arr_size(
_tmp34,sizeof(char))== 1?_tmp35 == '\000'?_tmp36 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp34.curr)=_tmp36;});goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:({
struct _tagged_arr _tmp37=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp38=*((char*)
_check_unknown_subscript(_tmp37,sizeof(char),0));char _tmp39='\\';if(
_get_arr_size(_tmp37,sizeof(char))== 1?_tmp38 == '\000'?_tmp39 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp37.curr)=_tmp39;});({struct _tagged_arr _tmp3A=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp3B=*((char*)
_check_unknown_subscript(_tmp3A,sizeof(char),0));char _tmp3C='r';if(_get_arr_size(
_tmp3A,sizeof(char))== 1?_tmp3B == '\000'?_tmp3C != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp3A.curr)=_tmp3C;});goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:({
struct _tagged_arr _tmp3D=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp3E=*((char*)
_check_unknown_subscript(_tmp3D,sizeof(char),0));char _tmp3F='\\';if(
_get_arr_size(_tmp3D,sizeof(char))== 1?_tmp3E == '\000'?_tmp3F != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp3D.curr)=_tmp3F;});({struct _tagged_arr _tmp40=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp41=*((char*)
_check_unknown_subscript(_tmp40,sizeof(char),0));char _tmp42='t';if(_get_arr_size(
_tmp40,sizeof(char))== 1?_tmp41 == '\000'?_tmp42 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp40.curr)=_tmp42;});goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:({
struct _tagged_arr _tmp43=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp44=*((char*)
_check_unknown_subscript(_tmp43,sizeof(char),0));char _tmp45='\\';if(
_get_arr_size(_tmp43,sizeof(char))== 1?_tmp44 == '\000'?_tmp45 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp43.curr)=_tmp45;});({struct _tagged_arr _tmp46=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp47=*((char*)
_check_unknown_subscript(_tmp46,sizeof(char),0));char _tmp48='v';if(_get_arr_size(
_tmp46,sizeof(char))== 1?_tmp47 == '\000'?_tmp48 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp46.curr)=_tmp48;});goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:({
struct _tagged_arr _tmp49=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp4A=*((char*)
_check_unknown_subscript(_tmp49,sizeof(char),0));char _tmp4B='\\';if(
_get_arr_size(_tmp49,sizeof(char))== 1?_tmp4A == '\000'?_tmp4B != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp49.curr)=_tmp4B;});({struct _tagged_arr _tmp4C=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp4D=*((char*)
_check_unknown_subscript(_tmp4C,sizeof(char),0));char _tmp4E='\\';if(
_get_arr_size(_tmp4C,sizeof(char))== 1?_tmp4D == '\000'?_tmp4E != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp4C.curr)=_tmp4E;});goto _LL21;_LL32: if(_tmp1E != '"')
goto _LL34;_LL33:({struct _tagged_arr _tmp4F=_tagged_arr_plus(t,sizeof(char),j ++);
char _tmp50=*((char*)_check_unknown_subscript(_tmp4F,sizeof(char),0));char _tmp51='\\';
if(_get_arr_size(_tmp4F,sizeof(char))== 1?_tmp50 == '\000'?_tmp51 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp4F.curr)=_tmp51;});({struct _tagged_arr _tmp52=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp53=*((char*)
_check_unknown_subscript(_tmp52,sizeof(char),0));char _tmp54='"';if(_get_arr_size(
_tmp52,sizeof(char))== 1?_tmp53 == '\000'?_tmp54 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp52.curr)=_tmp54;});goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '?_tmp1E <= '~':
0)({struct _tagged_arr _tmp55=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp56=*((
char*)_check_unknown_subscript(_tmp55,sizeof(char),0));char _tmp57=_tmp1E;if(
_get_arr_size(_tmp55,sizeof(char))== 1?_tmp56 == '\000'?_tmp57 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp55.curr)=_tmp57;});else{({struct _tagged_arr
_tmp58=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp59=*((char*)
_check_unknown_subscript(_tmp58,sizeof(char),0));char _tmp5A='\\';if(
_get_arr_size(_tmp58,sizeof(char))== 1?_tmp59 == '\000'?_tmp5A != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp58.curr)=_tmp5A;});({struct _tagged_arr _tmp5B=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp5C=*((char*)
_check_unknown_subscript(_tmp5B,sizeof(char),0));char _tmp5D=(char)('0' + (_tmp1E
>> 6 & 7));if(_get_arr_size(_tmp5B,sizeof(char))== 1?_tmp5C == '\000'?_tmp5D != '\000':
0: 0)_throw_arraybounds();*((char*)_tmp5B.curr)=_tmp5D;});({struct _tagged_arr
_tmp5E=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp5F=*((char*)
_check_unknown_subscript(_tmp5E,sizeof(char),0));char _tmp60=(char)('0' + (_tmp1E
>> 3 & 7));if(_get_arr_size(_tmp5E,sizeof(char))== 1?_tmp5F == '\000'?_tmp60 != '\000':
0: 0)_throw_arraybounds();*((char*)_tmp5E.curr)=_tmp60;});({struct _tagged_arr
_tmp61=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp62=*((char*)
_check_unknown_subscript(_tmp61,sizeof(char),0));char _tmp63=(char)('0' + (_tmp1E & 
7));if(_get_arr_size(_tmp61,sizeof(char))== 1?_tmp62 == '\000'?_tmp63 != '\000': 0:
0)_throw_arraybounds();*((char*)_tmp61.curr)=_tmp63;});}goto _LL21;_LL21:;}}
return(struct _tagged_arr)t;}}}}static char _tmp64[9]="restrict";static struct
_tagged_arr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _tagged_arr Cyc_Absynpp_volatile_string={_tmp65,
_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _tagged_arr Cyc_Absynpp_const_str={
_tmp66,_tmp66,_tmp66 + 6};static struct _tagged_arr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _tagged_arr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _tagged_arr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict)
l=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->hd=Cyc_Absynpp_restrict_sp;
_tmp67->tl=l;_tmp67;});if(tq.q_volatile)l=({struct Cyc_List_List*_tmp68=_cycalloc(
sizeof(*_tmp68));_tmp68->hd=Cyc_Absynpp_volatile_sp;_tmp68->tl=l;_tmp68;});if(tq.q_const)
l=({struct Cyc_List_List*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->hd=Cyc_Absynpp_const_sp;
_tmp69->tl=l;_tmp69;});return Cyc_PP_egroup(({const char*_tmp6A="";_tag_arr(_tmp6A,
sizeof(char),_get_zero_arr_size(_tmp6A,1));}),({const char*_tmp6B=" ";_tag_arr(
_tmp6B,sizeof(char),_get_zero_arr_size(_tmp6B,2));}),({const char*_tmp6C=" ";
_tag_arr(_tmp6C,sizeof(char),_get_zero_arr_size(_tmp6C,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,l));}struct _tagged_arr Cyc_Absynpp_kind2string(void*k){void*_tmp6D=
k;_LL37: if((int)_tmp6D != 0)goto _LL39;_LL38: return({const char*_tmp6E="A";_tag_arr(
_tmp6E,sizeof(char),_get_zero_arr_size(_tmp6E,2));});_LL39: if((int)_tmp6D != 1)
goto _LL3B;_LL3A: return({const char*_tmp6F="M";_tag_arr(_tmp6F,sizeof(char),
_get_zero_arr_size(_tmp6F,2));});_LL3B: if((int)_tmp6D != 2)goto _LL3D;_LL3C: return({
const char*_tmp70="B";_tag_arr(_tmp70,sizeof(char),_get_zero_arr_size(_tmp70,2));});
_LL3D: if((int)_tmp6D != 3)goto _LL3F;_LL3E: return({const char*_tmp71="R";_tag_arr(
_tmp71,sizeof(char),_get_zero_arr_size(_tmp71,2));});_LL3F: if((int)_tmp6D != 4)
goto _LL41;_LL40: return({const char*_tmp72="E";_tag_arr(_tmp72,sizeof(char),
_get_zero_arr_size(_tmp72,2));});_LL41: if((int)_tmp6D != 5)goto _LL36;_LL42: return({
const char*_tmp73="I";_tag_arr(_tmp73,sizeof(char),_get_zero_arr_size(_tmp73,2));});
_LL36:;}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(void*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(
k));}struct _tagged_arr Cyc_Absynpp_kindbound2string(void*c){void*_tmp74=Cyc_Absyn_compress_kb(
c);void*_tmp75;void*_tmp76;_LL44: if(*((int*)_tmp74)!= 0)goto _LL46;_tmp75=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp74)->f1;_LL45: return Cyc_Absynpp_kind2string(
_tmp75);_LL46: if(*((int*)_tmp74)!= 1)goto _LL48;_LL47: if(Cyc_PP_tex_output)return({
const char*_tmp77="{?}";_tag_arr(_tmp77,sizeof(char),_get_zero_arr_size(_tmp77,4));});
else{return({const char*_tmp78="?";_tag_arr(_tmp78,sizeof(char),
_get_zero_arr_size(_tmp78,2));});}_LL48: if(*((int*)_tmp74)!= 2)goto _LL43;_tmp76=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp74)->f2;_LL49: return Cyc_Absynpp_kind2string(
_tmp76);_LL43:;}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){void*_tmp79=
Cyc_Absyn_compress_kb(c);void*_tmp7A;void*_tmp7B;_LL4B: if(*((int*)_tmp79)!= 0)
goto _LL4D;_tmp7A=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp79)->f1;_LL4C: return
Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7A));_LL4D: if(*((int*)_tmp79)!= 1)goto
_LL4F;_LL4E: if(Cyc_PP_tex_output)return Cyc_PP_text_width(({const char*_tmp7C="{?}";
_tag_arr(_tmp7C,sizeof(char),_get_zero_arr_size(_tmp7C,4));}),1);else{return Cyc_PP_text(({
const char*_tmp7D="?";_tag_arr(_tmp7D,sizeof(char),_get_zero_arr_size(_tmp7D,2));}));}
_LL4F: if(*((int*)_tmp79)!= 2)goto _LL4A;_tmp7B=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp79)->f2;_LL50: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7B));_LL4A:;}
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){return Cyc_PP_egroup(({
const char*_tmp7E="<";_tag_arr(_tmp7E,sizeof(char),_get_zero_arr_size(_tmp7E,2));}),({
const char*_tmp7F=">";_tag_arr(_tmp7F,sizeof(char),_get_zero_arr_size(_tmp7F,2));}),({
const char*_tmp80=",";_tag_arr(_tmp80,sizeof(char),_get_zero_arr_size(_tmp80,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_typ2doc,ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*
tv){void*_tmp81=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp82;void*_tmp83;
void*_tmp84;_LL52: if(*((int*)_tmp81)!= 1)goto _LL54;_LL53: goto _LL55;_LL54: if(*((
int*)_tmp81)!= 0)goto _LL56;_tmp82=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp81)->f1;
if((int)_tmp82 != 2)goto _LL56;_LL55: return Cyc_PP_textptr(tv->name);_LL56: if(*((
int*)_tmp81)!= 2)goto _LL58;_tmp83=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp81)->f2;_LL57: _tmp84=_tmp83;goto _LL59;_LL58: if(*((int*)_tmp81)!= 0)goto _LL51;
_tmp84=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp81)->f1;_LL59: return({struct
Cyc_PP_Doc*_tmp85[3];_tmp85[2]=Cyc_Absynpp_kind2doc(_tmp84);_tmp85[1]=Cyc_PP_text(({
const char*_tmp86="::";_tag_arr(_tmp86,sizeof(char),_get_zero_arr_size(_tmp86,3));}));
_tmp85[0]=Cyc_PP_textptr(tv->name);Cyc_PP_cat(_tag_arr(_tmp85,sizeof(struct Cyc_PP_Doc*),
3));});_LL51:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return Cyc_PP_egroup(({const char*_tmp87="<";_tag_arr(_tmp87,sizeof(char),
_get_zero_arr_size(_tmp87,2));}),({const char*_tmp88=">";_tag_arr(_tmp88,sizeof(
char),_get_zero_arr_size(_tmp88,2));}),({const char*_tmp89=",";_tag_arr(_tmp89,
sizeof(char),_get_zero_arr_size(_tmp89,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,
tvs));}static struct _tagged_arr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);return Cyc_PP_egroup(({
const char*_tmp8A="<";_tag_arr(_tmp8A,sizeof(char),_get_zero_arr_size(_tmp8A,2));}),({
const char*_tmp8B=">";_tag_arr(_tmp8B,sizeof(char),_get_zero_arr_size(_tmp8B,2));}),({
const char*_tmp8C=",";_tag_arr(_tmp8C,sizeof(char),_get_zero_arr_size(_tmp8C,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
tvs)));}struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(
struct _tuple10*t){return Cyc_Absynpp_tqtd2doc((*((struct _tuple10*)t)).f1,(*((
struct _tuple10*)t)).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*
ts){return Cyc_PP_group(({const char*_tmp8D="(";_tag_arr(_tmp8D,sizeof(char),
_get_zero_arr_size(_tmp8D,2));}),({const char*_tmp8E=")";_tag_arr(_tmp8E,sizeof(
char),_get_zero_arr_size(_tmp8E,2));}),({const char*_tmp8F=",";_tag_arr(_tmp8F,
sizeof(char),_get_zero_arr_size(_tmp8F,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmp90=att;
_LL5B: if((int)_tmp90 != 0)goto _LL5D;_LL5C: goto _LL5E;_LL5D: if((int)_tmp90 != 1)goto
_LL5F;_LL5E: goto _LL60;_LL5F: if((int)_tmp90 != 2)goto _LL61;_LL60: return Cyc_PP_nil_doc();
_LL61:;_LL62: return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL5A:;}struct
Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 0;
atts=atts->tl){void*_tmp91=(void*)atts->hd;_LL64: if((int)_tmp91 != 0)goto _LL66;
_LL65: return Cyc_PP_text(({const char*_tmp92=" _stdcall ";_tag_arr(_tmp92,sizeof(
char),_get_zero_arr_size(_tmp92,11));}));_LL66: if((int)_tmp91 != 1)goto _LL68;
_LL67: return Cyc_PP_text(({const char*_tmp93=" _cdecl ";_tag_arr(_tmp93,sizeof(
char),_get_zero_arr_size(_tmp93,9));}));_LL68: if((int)_tmp91 != 2)goto _LL6A;_LL69:
return Cyc_PP_text(({const char*_tmp94=" _fastcall ";_tag_arr(_tmp94,sizeof(char),
_get_zero_arr_size(_tmp94,12));}));_LL6A:;_LL6B: goto _LL63;_LL63:;}return Cyc_PP_nil_doc();}
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){int hasatt=
0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmp95=(
void*)atts2->hd;_LL6D: if((int)_tmp95 != 0)goto _LL6F;_LL6E: goto _LL70;_LL6F: if((int)
_tmp95 != 1)goto _LL71;_LL70: goto _LL72;_LL71: if((int)_tmp95 != 2)goto _LL73;_LL72:
goto _LL6C;_LL73:;_LL74: hasatt=1;goto _LL6C;_LL6C:;}}if(!hasatt)return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmp96[3];_tmp96[2]=Cyc_PP_text(({const char*_tmp9B=")";
_tag_arr(_tmp9B,sizeof(char),_get_zero_arr_size(_tmp9B,2));}));_tmp96[1]=Cyc_PP_group(({
const char*_tmp98="";_tag_arr(_tmp98,sizeof(char),_get_zero_arr_size(_tmp98,1));}),({
const char*_tmp99="";_tag_arr(_tmp99,sizeof(char),_get_zero_arr_size(_tmp99,1));}),({
const char*_tmp9A=" ";_tag_arr(_tmp9A,sizeof(char),_get_zero_arr_size(_tmp9A,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_noncallatt2doc,atts));_tmp96[0]=Cyc_PP_text(({const char*_tmp97="__declspec(";
_tag_arr(_tmp97,sizeof(char),_get_zero_arr_size(_tmp97,12));}));Cyc_PP_cat(
_tag_arr(_tmp96,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(
void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();if(Cyc_Absynpp_to_VC)
return Cyc_Absynpp_noncallconv2doc(atts);return({struct Cyc_PP_Doc*_tmp9C[2];
_tmp9C[1]=Cyc_PP_group(({const char*_tmp9E="((";_tag_arr(_tmp9E,sizeof(char),
_get_zero_arr_size(_tmp9E,3));}),({const char*_tmp9F="))";_tag_arr(_tmp9F,sizeof(
char),_get_zero_arr_size(_tmp9F,3));}),({const char*_tmpA0=",";_tag_arr(_tmpA0,
sizeof(char),_get_zero_arr_size(_tmpA0,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,
atts));_tmp9C[0]=Cyc_PP_text(({const char*_tmp9D=" __attribute__";_tag_arr(_tmp9D,
sizeof(char),_get_zero_arr_size(_tmp9D,15));}));Cyc_PP_cat(_tag_arr(_tmp9C,
sizeof(struct Cyc_PP_Doc*),2));});}int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*
tms){if(tms == 0)return 0;{void*_tmpA1=(void*)tms->hd;_LL76: if(*((int*)_tmpA1)!= 2)
goto _LL78;_LL77: return 1;_LL78: if(*((int*)_tmpA1)!= 5)goto _LL7A;_LL79: if(!Cyc_Absynpp_to_VC)
return 0;return Cyc_Absynpp_next_is_pointer(tms->tl);_LL7A:;_LL7B: return 0;_LL75:;}}
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_question(){if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output)Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(({
const char*_tmpA2="{?}";_tag_arr(_tmpA2,sizeof(char),_get_zero_arr_size(_tmpA2,4));}),
1);else{Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(({const char*
_tmpA3="?";_tag_arr(_tmpA3,sizeof(char),_get_zero_arr_size(_tmpA3,2));}));}}
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}static struct
Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){if(!((
unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpA4="{\\lb}";_tag_arr(_tmpA4,
sizeof(char),_get_zero_arr_size(_tmpA4,6));}),1);else{Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA5="{";_tag_arr(_tmpA5,sizeof(char),
_get_zero_arr_size(_tmpA5,2));}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_rb=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpA6="{\\rb}";_tag_arr(_tmpA6,
sizeof(char),_get_zero_arr_size(_tmpA6,6));}),1);else{Cyc_Absynpp_cache_rb=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA7="}";_tag_arr(_tmpA7,sizeof(char),
_get_zero_arr_size(_tmpA7,2));}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpA8="\\$";_tag_arr(_tmpA8,
sizeof(char),_get_zero_arr_size(_tmpA8,3));}),1);else{Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA9="$";_tag_arr(_tmpA9,sizeof(char),
_get_zero_arr_size(_tmpA9,2));}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _tagged_arr sep,struct Cyc_List_List*
ss){return({struct Cyc_PP_Doc*_tmpAA[3];_tmpAA[2]=Cyc_Absynpp_rb();_tmpAA[1]=Cyc_PP_seq(
sep,ss);_tmpAA[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_arr(_tmpAA,sizeof(struct Cyc_PP_Doc*),
3));});}struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,
struct Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(
0,d,tms->tl);struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmpEB[3];_tmpEB[2]=Cyc_PP_text(({
const char*_tmpED=")";_tag_arr(_tmpED,sizeof(char),_get_zero_arr_size(_tmpED,2));}));
_tmpEB[1]=rest;_tmpEB[0]=Cyc_PP_text(({const char*_tmpEC="(";_tag_arr(_tmpEC,
sizeof(char),_get_zero_arr_size(_tmpEC,2));}));Cyc_PP_cat(_tag_arr(_tmpEB,
sizeof(struct Cyc_PP_Doc*),3));});void*_tmpAB=(void*)tms->hd;struct Cyc_Absyn_Conref*
_tmpAC;struct Cyc_Absyn_Exp*_tmpAD;struct Cyc_Absyn_Conref*_tmpAE;void*_tmpAF;
struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpB1;struct Cyc_Position_Segment*
_tmpB2;int _tmpB3;void*_tmpB4;void*_tmpB5;struct Cyc_Absyn_Tqual _tmpB6;_LL7D: if(*((
int*)_tmpAB)!= 0)goto _LL7F;_tmpAC=((struct Cyc_Absyn_Carray_mod_struct*)_tmpAB)->f1;
_LL7E: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*
_tmpB7[2];_tmpB7[1]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpAC)?Cyc_PP_text(({const char*_tmpB8="[]ZEROTERM ";_tag_arr(_tmpB8,sizeof(
char),_get_zero_arr_size(_tmpB8,12));})): Cyc_PP_text(({const char*_tmpB9="[]";
_tag_arr(_tmpB9,sizeof(char),_get_zero_arr_size(_tmpB9,3));}));_tmpB7[0]=rest;
Cyc_PP_cat(_tag_arr(_tmpB7,sizeof(struct Cyc_PP_Doc*),2));});_LL7F: if(*((int*)
_tmpAB)!= 1)goto _LL81;_tmpAD=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpAB)->f1;
_tmpAE=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpAB)->f2;_LL80: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*_tmpBA[4];_tmpBA[3]=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpAE)?Cyc_PP_text(({const
char*_tmpBC="]ZEROTERM ";_tag_arr(_tmpBC,sizeof(char),_get_zero_arr_size(_tmpBC,
11));})): Cyc_PP_text(({const char*_tmpBD="]";_tag_arr(_tmpBD,sizeof(char),
_get_zero_arr_size(_tmpBD,2));}));_tmpBA[2]=Cyc_Absynpp_exp2doc(_tmpAD);_tmpBA[1]=
Cyc_PP_text(({const char*_tmpBB="[";_tag_arr(_tmpBB,sizeof(char),
_get_zero_arr_size(_tmpBB,2));}));_tmpBA[0]=rest;Cyc_PP_cat(_tag_arr(_tmpBA,
sizeof(struct Cyc_PP_Doc*),4));});_LL81: if(*((int*)_tmpAB)!= 3)goto _LL83;_tmpAF=(
void*)((struct Cyc_Absyn_Function_mod_struct*)_tmpAB)->f1;_LL82: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmpBE=_tmpAF;struct Cyc_List_List*_tmpBF;int _tmpC0;
struct Cyc_Absyn_VarargInfo*_tmpC1;struct Cyc_Core_Opt*_tmpC2;struct Cyc_List_List*
_tmpC3;struct Cyc_List_List*_tmpC4;struct Cyc_Position_Segment*_tmpC5;_LL8A: if(*((
int*)_tmpBE)!= 1)goto _LL8C;_tmpBF=((struct Cyc_Absyn_WithTypes_struct*)_tmpBE)->f1;
_tmpC0=((struct Cyc_Absyn_WithTypes_struct*)_tmpBE)->f2;_tmpC1=((struct Cyc_Absyn_WithTypes_struct*)
_tmpBE)->f3;_tmpC2=((struct Cyc_Absyn_WithTypes_struct*)_tmpBE)->f4;_tmpC3=((
struct Cyc_Absyn_WithTypes_struct*)_tmpBE)->f5;_LL8B: return({struct Cyc_PP_Doc*
_tmpC6[2];_tmpC6[1]=Cyc_Absynpp_funargs2doc(_tmpBF,_tmpC0,_tmpC1,_tmpC2,_tmpC3);
_tmpC6[0]=rest;Cyc_PP_cat(_tag_arr(_tmpC6,sizeof(struct Cyc_PP_Doc*),2));});_LL8C:
if(*((int*)_tmpBE)!= 0)goto _LL89;_tmpC4=((struct Cyc_Absyn_NoTypes_struct*)_tmpBE)->f1;
_tmpC5=((struct Cyc_Absyn_NoTypes_struct*)_tmpBE)->f2;_LL8D: return({struct Cyc_PP_Doc*
_tmpC7[2];_tmpC7[1]=Cyc_PP_group(({const char*_tmpC8="(";_tag_arr(_tmpC8,sizeof(
char),_get_zero_arr_size(_tmpC8,2));}),({const char*_tmpC9=")";_tag_arr(_tmpC9,
sizeof(char),_get_zero_arr_size(_tmpC9,2));}),({const char*_tmpCA=",";_tag_arr(
_tmpCA,sizeof(char),_get_zero_arr_size(_tmpCA,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,_tmpC4));_tmpC7[0]=rest;Cyc_PP_cat(_tag_arr(_tmpC7,sizeof(struct
Cyc_PP_Doc*),2));});_LL89:;}_LL83: if(*((int*)_tmpAB)!= 5)goto _LL85;_tmpB0=((
struct Cyc_Absyn_Attributes_mod_struct*)_tmpAB)->f2;_LL84: if(!Cyc_Absynpp_to_VC){
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*
_tmpCB[2];_tmpCB[1]=Cyc_Absynpp_atts2doc(_tmpB0);_tmpCB[0]=rest;Cyc_PP_cat(
_tag_arr(_tmpCB,sizeof(struct Cyc_PP_Doc*),2));});}else{if(Cyc_Absynpp_next_is_pointer(
tms->tl))return({struct Cyc_PP_Doc*_tmpCC[2];_tmpCC[1]=rest;_tmpCC[0]=Cyc_Absynpp_callconv2doc(
_tmpB0);Cyc_PP_cat(_tag_arr(_tmpCC,sizeof(struct Cyc_PP_Doc*),2));});return rest;}
_LL85: if(*((int*)_tmpAB)!= 4)goto _LL87;_tmpB1=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpAB)->f1;_tmpB2=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpAB)->f2;_tmpB3=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpAB)->f3;_LL86: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;if(_tmpB3)return({struct Cyc_PP_Doc*_tmpCD[2];_tmpCD[1]=Cyc_Absynpp_ktvars2doc(
_tmpB1);_tmpCD[0]=rest;Cyc_PP_cat(_tag_arr(_tmpCD,sizeof(struct Cyc_PP_Doc*),2));});
else{return({struct Cyc_PP_Doc*_tmpCE[2];_tmpCE[1]=Cyc_Absynpp_tvars2doc(_tmpB1);
_tmpCE[0]=rest;Cyc_PP_cat(_tag_arr(_tmpCE,sizeof(struct Cyc_PP_Doc*),2));});}
_LL87: if(*((int*)_tmpAB)!= 2)goto _LL7C;_tmpB4=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpAB)->f1;_tmpB5=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpAB)->f2;
_tmpB6=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpAB)->f3;_LL88: {struct Cyc_PP_Doc*
ptr;{void*_tmpCF=_tmpB4;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Conref*_tmpD1;
struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Conref*_tmpD3;struct Cyc_Absyn_Conref*
_tmpD4;_LL8F: if(*((int*)_tmpCF)!= 1)goto _LL91;_tmpD0=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmpCF)->f1;_tmpD1=((struct Cyc_Absyn_Nullable_ps_struct*)_tmpCF)->f2;_LL90: {
struct _tuple4 pr=Cyc_Evexp_eval_const_uint_exp(_tmpD0);if(pr.f1 == 1?pr.f2: 0)ptr=
Cyc_PP_text(({const char*_tmpD5="*";_tag_arr(_tmpD5,sizeof(char),
_get_zero_arr_size(_tmpD5,2));}));else{ptr=({struct Cyc_PP_Doc*_tmpD6[4];_tmpD6[3]=
Cyc_Absynpp_rb();_tmpD6[2]=Cyc_Absynpp_exp2doc(_tmpD0);_tmpD6[1]=Cyc_Absynpp_lb();
_tmpD6[0]=Cyc_PP_text(({const char*_tmpD7="*";_tag_arr(_tmpD7,sizeof(char),
_get_zero_arr_size(_tmpD7,2));}));Cyc_PP_cat(_tag_arr(_tmpD6,sizeof(struct Cyc_PP_Doc*),
4));});}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr?((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpD1): 0)ptr=({struct Cyc_PP_Doc*_tmpD8[2];_tmpD8[1]=
Cyc_PP_text(({const char*_tmpD9="ZEROTERM ";_tag_arr(_tmpD9,sizeof(char),
_get_zero_arr_size(_tmpD9,10));}));_tmpD8[0]=ptr;Cyc_PP_cat(_tag_arr(_tmpD8,
sizeof(struct Cyc_PP_Doc*),2));});else{if(is_char_ptr?!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpD1): 0)ptr=({struct Cyc_PP_Doc*_tmpDA[2];_tmpDA[1]=
Cyc_PP_text(({const char*_tmpDB="NOZEROTERM ";_tag_arr(_tmpDB,sizeof(char),
_get_zero_arr_size(_tmpDB,12));}));_tmpDA[0]=ptr;Cyc_PP_cat(_tag_arr(_tmpDA,
sizeof(struct Cyc_PP_Doc*),2));});}}goto _LL8E;}_LL91: if(*((int*)_tmpCF)!= 0)goto
_LL93;_tmpD2=((struct Cyc_Absyn_NonNullable_ps_struct*)_tmpCF)->f1;_tmpD3=((
struct Cyc_Absyn_NonNullable_ps_struct*)_tmpCF)->f2;_LL92: {struct _tuple4 pr=Cyc_Evexp_eval_const_uint_exp(
_tmpD2);if(pr.f1 == 1?pr.f2: 0)ptr=Cyc_PP_text(({const char*_tmpDC="@";_tag_arr(
_tmpDC,sizeof(char),_get_zero_arr_size(_tmpDC,2));}));else{ptr=({struct Cyc_PP_Doc*
_tmpDD[4];_tmpDD[3]=Cyc_Absynpp_rb();_tmpDD[2]=Cyc_Absynpp_exp2doc(_tmpD2);
_tmpDD[1]=Cyc_Absynpp_lb();_tmpDD[0]=Cyc_PP_text(({const char*_tmpDE="@";_tag_arr(
_tmpDE,sizeof(char),_get_zero_arr_size(_tmpDE,2));}));Cyc_PP_cat(_tag_arr(_tmpDD,
sizeof(struct Cyc_PP_Doc*),4));});}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr?((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpD3): 0)ptr=({
struct Cyc_PP_Doc*_tmpDF[2];_tmpDF[1]=Cyc_PP_text(({const char*_tmpE0="ZEROTERM ";
_tag_arr(_tmpE0,sizeof(char),_get_zero_arr_size(_tmpE0,10));}));_tmpDF[0]=ptr;
Cyc_PP_cat(_tag_arr(_tmpDF,sizeof(struct Cyc_PP_Doc*),2));});else{if(is_char_ptr?
!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpD3): 0)ptr=({
struct Cyc_PP_Doc*_tmpE1[2];_tmpE1[1]=Cyc_PP_text(({const char*_tmpE2="NOZEROTERM ";
_tag_arr(_tmpE2,sizeof(char),_get_zero_arr_size(_tmpE2,12));}));_tmpE1[0]=ptr;
Cyc_PP_cat(_tag_arr(_tmpE1,sizeof(struct Cyc_PP_Doc*),2));});}}goto _LL8E;}_LL93:
if(*((int*)_tmpCF)!= 2)goto _LL8E;_tmpD4=((struct Cyc_Absyn_TaggedArray_ps_struct*)
_tmpCF)->f1;_LL94: ptr=Cyc_Absynpp_question();if(Cyc_Absynpp_print_zeroterm){if(!
is_char_ptr?((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmpD4): 0)ptr=({struct Cyc_PP_Doc*_tmpE3[2];_tmpE3[1]=Cyc_PP_text(({const char*
_tmpE4="ZEROTERM ";_tag_arr(_tmpE4,sizeof(char),_get_zero_arr_size(_tmpE4,10));}));
_tmpE3[0]=ptr;Cyc_PP_cat(_tag_arr(_tmpE3,sizeof(struct Cyc_PP_Doc*),2));});else{
if(is_char_ptr?!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmpD4): 0)ptr=({struct Cyc_PP_Doc*_tmpE5[2];_tmpE5[1]=Cyc_PP_text(({const char*
_tmpE6="NOZEROTERM ";_tag_arr(_tmpE6,sizeof(char),_get_zero_arr_size(_tmpE6,12));}));
_tmpE5[0]=ptr;Cyc_PP_cat(_tag_arr(_tmpE5,sizeof(struct Cyc_PP_Doc*),2));});}}goto
_LL8E;_LL8E:;}{void*_tmpE7=Cyc_Tcutil_compress(_tmpB5);_LL96: if((int)_tmpE7 != 2)
goto _LL98;_LL97: return({struct Cyc_PP_Doc*_tmpE8[2];_tmpE8[1]=rest;_tmpE8[0]=ptr;
Cyc_PP_cat(_tag_arr(_tmpE8,sizeof(struct Cyc_PP_Doc*),2));});_LL98:;_LL99: return({
struct Cyc_PP_Doc*_tmpE9[4];_tmpE9[3]=rest;_tmpE9[2]=Cyc_PP_text(({const char*
_tmpEA=" ";_tag_arr(_tmpEA,sizeof(char),_get_zero_arr_size(_tmpEA,2));}));_tmpE9[
1]=Cyc_Absynpp_typ2doc(_tmpB5);_tmpE9[0]=ptr;Cyc_PP_cat(_tag_arr(_tmpE9,sizeof(
struct Cyc_PP_Doc*),4));});_LL95:;}}_LL7C:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t){void*_tmpEE=Cyc_Tcutil_compress(t);_LL9B: if((int)_tmpEE != 2)goto _LL9D;
_LL9C: return Cyc_PP_text(({const char*_tmpEF="`H";_tag_arr(_tmpEF,sizeof(char),
_get_zero_arr_size(_tmpEF,3));}));_LL9D:;_LL9E: return Cyc_Absynpp_ntyp2doc(t);
_LL9A:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
effects,void*t){void*_tmpF0=Cyc_Tcutil_compress(t);void*_tmpF1;struct Cyc_List_List*
_tmpF2;_LLA0: if(_tmpF0 <= (void*)3?1:*((int*)_tmpF0)!= 19)goto _LLA2;_tmpF1=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmpF0)->f1;_LLA1:*((struct Cyc_List_List**)
rgions)=({struct Cyc_List_List*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->hd=Cyc_Absynpp_rgn2doc(
_tmpF1);_tmpF3->tl=*((struct Cyc_List_List**)rgions);_tmpF3;});goto _LL9F;_LLA2:
if(_tmpF0 <= (void*)3?1:*((int*)_tmpF0)!= 20)goto _LLA4;_tmpF2=((struct Cyc_Absyn_JoinEff_struct*)
_tmpF0)->f1;_LLA3: for(0;_tmpF2 != 0;_tmpF2=_tmpF2->tl){Cyc_Absynpp_effects2docs(
rgions,effects,(void*)_tmpF2->hd);}goto _LL9F;_LLA4:;_LLA5:*((struct Cyc_List_List**)
effects)=({struct Cyc_List_List*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->hd=Cyc_Absynpp_typ2doc(
t);_tmpF4->tl=*((struct Cyc_List_List**)effects);_tmpF4;});goto _LL9F;_LL9F:;}
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=0;struct
Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);rgions=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);effects=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);if(
rgions == 0?effects != 0: 0)return Cyc_PP_group(({const char*_tmpF5="";_tag_arr(
_tmpF5,sizeof(char),_get_zero_arr_size(_tmpF5,1));}),({const char*_tmpF6="";
_tag_arr(_tmpF6,sizeof(char),_get_zero_arr_size(_tmpF6,1));}),({const char*_tmpF7="+";
_tag_arr(_tmpF7,sizeof(char),_get_zero_arr_size(_tmpF7,2));}),effects);else{
struct Cyc_PP_Doc*_tmpF8=Cyc_Absynpp_group_braces(({const char*_tmpFD=",";_tag_arr(
_tmpFD,sizeof(char),_get_zero_arr_size(_tmpFD,2));}),rgions);return Cyc_PP_group(({
const char*_tmpF9="";_tag_arr(_tmpF9,sizeof(char),_get_zero_arr_size(_tmpF9,1));}),({
const char*_tmpFA="";_tag_arr(_tmpFA,sizeof(char),_get_zero_arr_size(_tmpFA,1));}),({
const char*_tmpFB="+";_tag_arr(_tmpFB,sizeof(char),_get_zero_arr_size(_tmpFB,2));}),((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
effects,({struct Cyc_List_List*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->hd=
_tmpF8;_tmpFC->tl=0;_tmpFC;})));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(
void*k){void*_tmpFE=k;_LLA7: if((int)_tmpFE != 0)goto _LLA9;_LLA8: return Cyc_PP_text(({
const char*_tmpFF="struct ";_tag_arr(_tmpFF,sizeof(char),_get_zero_arr_size(
_tmpFF,8));}));_LLA9: if((int)_tmpFE != 1)goto _LLA6;_LLAA: return Cyc_PP_text(({
const char*_tmp100="union ";_tag_arr(_tmp100,sizeof(char),_get_zero_arr_size(
_tmp100,7));}));_LLA6:;}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*
s;{void*_tmp101=t;struct Cyc_Core_Opt*_tmp102;struct Cyc_Core_Opt*_tmp103;int
_tmp104;struct Cyc_Core_Opt*_tmp105;struct Cyc_Absyn_Tvar*_tmp106;struct Cyc_Absyn_TunionInfo
_tmp107;void*_tmp108;struct Cyc_List_List*_tmp109;void*_tmp10A;struct Cyc_Absyn_TunionFieldInfo
_tmp10B;void*_tmp10C;struct Cyc_List_List*_tmp10D;void*_tmp10E;void*_tmp10F;int
_tmp110;struct Cyc_List_List*_tmp111;struct Cyc_Absyn_AggrInfo _tmp112;void*_tmp113;
struct Cyc_List_List*_tmp114;void*_tmp115;struct Cyc_List_List*_tmp116;struct Cyc_List_List*
_tmp117;struct _tuple0*_tmp118;void*_tmp119;struct _tuple0*_tmp11A;struct Cyc_List_List*
_tmp11B;struct Cyc_Absyn_Typedefdecl*_tmp11C;struct Cyc_Core_Opt*_tmp11D;void*
_tmp11E;int _tmp11F;void*_tmp120;void*_tmp121;_LLAC: if(_tmp101 <= (void*)3?1:*((
int*)_tmp101)!= 7)goto _LLAE;_LLAD: return Cyc_PP_text(({const char*_tmp122="[[[array]]]";
_tag_arr(_tmp122,sizeof(char),_get_zero_arr_size(_tmp122,12));}));_LLAE: if(
_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 8)goto _LLB0;_LLAF: return Cyc_PP_nil_doc();
_LLB0: if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 4)goto _LLB2;_LLB1: return Cyc_PP_nil_doc();
_LLB2: if((int)_tmp101 != 0)goto _LLB4;_LLB3: s=Cyc_PP_text(({const char*_tmp123="void";
_tag_arr(_tmp123,sizeof(char),_get_zero_arr_size(_tmp123,5));}));goto _LLAB;_LLB4:
if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 0)goto _LLB6;_tmp102=((struct Cyc_Absyn_Evar_struct*)
_tmp101)->f1;_tmp103=((struct Cyc_Absyn_Evar_struct*)_tmp101)->f2;_tmp104=((
struct Cyc_Absyn_Evar_struct*)_tmp101)->f3;_tmp105=((struct Cyc_Absyn_Evar_struct*)
_tmp101)->f4;_LLB5: if(_tmp103 != 0)return Cyc_Absynpp_ntyp2doc((void*)_tmp103->v);
else{void*_tmp124=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp102))->v;_LLDF:
if((int)_tmp124 != 3)goto _LLE1;_LLE0: s=({struct Cyc_PP_Doc*_tmp125[2];_tmp125[1]=
Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp129;_tmp129.tag=1;
_tmp129.f1=(unsigned int)_tmp104;{void*_tmp127[1]={& _tmp129};Cyc_aprintf(({const
char*_tmp128="%d";_tag_arr(_tmp128,sizeof(char),_get_zero_arr_size(_tmp128,3));}),
_tag_arr(_tmp127,sizeof(void*),1));}}));_tmp125[0]=Cyc_PP_text(({const char*
_tmp126="`r";_tag_arr(_tmp126,sizeof(char),_get_zero_arr_size(_tmp126,3));}));
Cyc_PP_cat(_tag_arr(_tmp125,sizeof(struct Cyc_PP_Doc*),2));});goto _LLDE;_LLE1:;
_LLE2: s=({struct Cyc_PP_Doc*_tmp12A[6];_tmp12A[5]=_tmp102 == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((void*)_tmp102->v);_tmp12A[4]=Cyc_PP_text(({const char*
_tmp131=")::";_tag_arr(_tmp131,sizeof(char),_get_zero_arr_size(_tmp131,4));}));
_tmp12A[3]=(!Cyc_Absynpp_print_full_evars?1: _tmp105 == 0)?Cyc_PP_text(({const char*
_tmp130="";_tag_arr(_tmp130,sizeof(char),_get_zero_arr_size(_tmp130,1));})): Cyc_Absynpp_tvars2doc((
struct Cyc_List_List*)_tmp105->v);_tmp12A[2]=Cyc_PP_text((struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp12F;_tmp12F.tag=1;_tmp12F.f1=(unsigned int)_tmp104;{
void*_tmp12D[1]={& _tmp12F};Cyc_aprintf(({const char*_tmp12E="%d";_tag_arr(_tmp12E,
sizeof(char),_get_zero_arr_size(_tmp12E,3));}),_tag_arr(_tmp12D,sizeof(void*),1));}}));
_tmp12A[1]=Cyc_PP_text(({const char*_tmp12C="(";_tag_arr(_tmp12C,sizeof(char),
_get_zero_arr_size(_tmp12C,2));}));_tmp12A[0]=Cyc_PP_text(({const char*_tmp12B="%";
_tag_arr(_tmp12B,sizeof(char),_get_zero_arr_size(_tmp12B,2));}));Cyc_PP_cat(
_tag_arr(_tmp12A,sizeof(struct Cyc_PP_Doc*),6));});_LLDE:;}goto _LLAB;_LLB6: if(
_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 1)goto _LLB8;_tmp106=((struct Cyc_Absyn_VarType_struct*)
_tmp101)->f1;_LLB7: s=Cyc_PP_textptr(_tmp106->name);if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp132[3];_tmp132[2]=Cyc_Absynpp_kindbound2doc((void*)
_tmp106->kind);_tmp132[1]=Cyc_PP_text(({const char*_tmp133="::";_tag_arr(_tmp133,
sizeof(char),_get_zero_arr_size(_tmp133,3));}));_tmp132[0]=s;Cyc_PP_cat(_tag_arr(
_tmp132,sizeof(struct Cyc_PP_Doc*),3));});if(Cyc_Absynpp_rewrite_temp_tvars?Cyc_Tcutil_is_temp_tvar(
_tmp106): 0)s=({struct Cyc_PP_Doc*_tmp134[3];_tmp134[2]=Cyc_PP_text(({const char*
_tmp136=" */";_tag_arr(_tmp136,sizeof(char),_get_zero_arr_size(_tmp136,4));}));
_tmp134[1]=s;_tmp134[0]=Cyc_PP_text(({const char*_tmp135="_ /* ";_tag_arr(_tmp135,
sizeof(char),_get_zero_arr_size(_tmp135,6));}));Cyc_PP_cat(_tag_arr(_tmp134,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLAB;_LLB8: if(_tmp101 <= (void*)3?1:*((int*)
_tmp101)!= 2)goto _LLBA;_tmp107=((struct Cyc_Absyn_TunionType_struct*)_tmp101)->f1;
_tmp108=(void*)_tmp107.tunion_info;_tmp109=_tmp107.targs;_tmp10A=(void*)_tmp107.rgn;
_LLB9:{void*_tmp137=_tmp108;struct Cyc_Absyn_UnknownTunionInfo _tmp138;struct
_tuple0*_tmp139;int _tmp13A;struct Cyc_Absyn_Tuniondecl**_tmp13B;struct Cyc_Absyn_Tuniondecl*
_tmp13C;struct Cyc_Absyn_Tuniondecl _tmp13D;struct _tuple0*_tmp13E;int _tmp13F;_LLE4:
if(*((int*)_tmp137)!= 0)goto _LLE6;_tmp138=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp137)->f1;_tmp139=_tmp138.name;_tmp13A=_tmp138.is_xtunion;_LLE5: _tmp13E=
_tmp139;_tmp13F=_tmp13A;goto _LLE7;_LLE6: if(*((int*)_tmp137)!= 1)goto _LLE3;
_tmp13B=((struct Cyc_Absyn_KnownTunion_struct*)_tmp137)->f1;_tmp13C=*_tmp13B;
_tmp13D=*_tmp13C;_tmp13E=_tmp13D.name;_tmp13F=_tmp13D.is_xtunion;_LLE7: {struct
Cyc_PP_Doc*_tmp140=Cyc_PP_text(_tmp13F?({const char*_tmp145="xtunion ";_tag_arr(
_tmp145,sizeof(char),_get_zero_arr_size(_tmp145,9));}):({const char*_tmp146="tunion ";
_tag_arr(_tmp146,sizeof(char),_get_zero_arr_size(_tmp146,8));}));{void*_tmp141=
Cyc_Tcutil_compress(_tmp10A);_LLE9: if((int)_tmp141 != 2)goto _LLEB;_LLEA: s=({
struct Cyc_PP_Doc*_tmp142[3];_tmp142[2]=Cyc_Absynpp_tps2doc(_tmp109);_tmp142[1]=
Cyc_Absynpp_qvar2doc(_tmp13E);_tmp142[0]=_tmp140;Cyc_PP_cat(_tag_arr(_tmp142,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLE8;_LLEB:;_LLEC: s=({struct Cyc_PP_Doc*
_tmp143[5];_tmp143[4]=Cyc_Absynpp_tps2doc(_tmp109);_tmp143[3]=Cyc_Absynpp_qvar2doc(
_tmp13E);_tmp143[2]=Cyc_PP_text(({const char*_tmp144=" ";_tag_arr(_tmp144,sizeof(
char),_get_zero_arr_size(_tmp144,2));}));_tmp143[1]=Cyc_Absynpp_typ2doc(_tmp10A);
_tmp143[0]=_tmp140;Cyc_PP_cat(_tag_arr(_tmp143,sizeof(struct Cyc_PP_Doc*),5));});
goto _LLE8;_LLE8:;}goto _LLE3;}_LLE3:;}goto _LLAB;_LLBA: if(_tmp101 <= (void*)3?1:*((
int*)_tmp101)!= 3)goto _LLBC;_tmp10B=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp101)->f1;_tmp10C=(void*)_tmp10B.field_info;_tmp10D=_tmp10B.targs;_LLBB:{void*
_tmp147=_tmp10C;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp148;struct _tuple0*
_tmp149;struct _tuple0*_tmp14A;int _tmp14B;struct Cyc_Absyn_Tuniondecl*_tmp14C;
struct Cyc_Absyn_Tuniondecl _tmp14D;struct _tuple0*_tmp14E;int _tmp14F;struct Cyc_Absyn_Tunionfield*
_tmp150;struct Cyc_Absyn_Tunionfield _tmp151;struct _tuple0*_tmp152;_LLEE: if(*((int*)
_tmp147)!= 0)goto _LLF0;_tmp148=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp147)->f1;_tmp149=_tmp148.tunion_name;_tmp14A=_tmp148.field_name;_tmp14B=
_tmp148.is_xtunion;_LLEF: _tmp14E=_tmp149;_tmp14F=_tmp14B;_tmp152=_tmp14A;goto
_LLF1;_LLF0: if(*((int*)_tmp147)!= 1)goto _LLED;_tmp14C=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp147)->f1;_tmp14D=*_tmp14C;_tmp14E=_tmp14D.name;_tmp14F=_tmp14D.is_xtunion;
_tmp150=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp147)->f2;_tmp151=*_tmp150;
_tmp152=_tmp151.name;_LLF1: {struct Cyc_PP_Doc*_tmp153=Cyc_PP_text(_tmp14F?({
const char*_tmp156="xtunion ";_tag_arr(_tmp156,sizeof(char),_get_zero_arr_size(
_tmp156,9));}):({const char*_tmp157="tunion ";_tag_arr(_tmp157,sizeof(char),
_get_zero_arr_size(_tmp157,8));}));s=({struct Cyc_PP_Doc*_tmp154[4];_tmp154[3]=
Cyc_Absynpp_qvar2doc(_tmp152);_tmp154[2]=Cyc_PP_text(({const char*_tmp155=".";
_tag_arr(_tmp155,sizeof(char),_get_zero_arr_size(_tmp155,2));}));_tmp154[1]=Cyc_Absynpp_qvar2doc(
_tmp14E);_tmp154[0]=_tmp153;Cyc_PP_cat(_tag_arr(_tmp154,sizeof(struct Cyc_PP_Doc*),
4));});goto _LLED;}_LLED:;}goto _LLAB;_LLBC: if(_tmp101 <= (void*)3?1:*((int*)
_tmp101)!= 5)goto _LLBE;_tmp10E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp101)->f1;
_tmp10F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp101)->f2;_LLBD: {struct
_tagged_arr sns;struct _tagged_arr ts;{void*_tmp158=_tmp10E;_LLF3: if((int)_tmp158 != 
2)goto _LLF5;_LLF4: goto _LLF6;_LLF5: if((int)_tmp158 != 0)goto _LLF7;_LLF6: sns=({
const char*_tmp159="";_tag_arr(_tmp159,sizeof(char),_get_zero_arr_size(_tmp159,1));});
goto _LLF2;_LLF7: if((int)_tmp158 != 1)goto _LLF2;_LLF8: sns=({const char*_tmp15A="unsigned ";
_tag_arr(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,10));});goto _LLF2;_LLF2:;}{
void*_tmp15B=_tmp10F;_LLFA: if((int)_tmp15B != 0)goto _LLFC;_LLFB:{void*_tmp15C=
_tmp10E;_LL103: if((int)_tmp15C != 2)goto _LL105;_LL104: sns=({const char*_tmp15D="";
_tag_arr(_tmp15D,sizeof(char),_get_zero_arr_size(_tmp15D,1));});goto _LL102;
_LL105: if((int)_tmp15C != 0)goto _LL107;_LL106: sns=({const char*_tmp15E="signed ";
_tag_arr(_tmp15E,sizeof(char),_get_zero_arr_size(_tmp15E,8));});goto _LL102;
_LL107: if((int)_tmp15C != 1)goto _LL102;_LL108: sns=({const char*_tmp15F="unsigned ";
_tag_arr(_tmp15F,sizeof(char),_get_zero_arr_size(_tmp15F,10));});goto _LL102;
_LL102:;}ts=({const char*_tmp160="char";_tag_arr(_tmp160,sizeof(char),
_get_zero_arr_size(_tmp160,5));});goto _LLF9;_LLFC: if((int)_tmp15B != 1)goto _LLFE;
_LLFD: ts=({const char*_tmp161="short";_tag_arr(_tmp161,sizeof(char),
_get_zero_arr_size(_tmp161,6));});goto _LLF9;_LLFE: if((int)_tmp15B != 2)goto _LL100;
_LLFF: ts=({const char*_tmp162="int";_tag_arr(_tmp162,sizeof(char),
_get_zero_arr_size(_tmp162,4));});goto _LLF9;_LL100: if((int)_tmp15B != 3)goto _LLF9;
_LL101: ts=Cyc_Absynpp_to_VC?({const char*_tmp163="__int64";_tag_arr(_tmp163,
sizeof(char),_get_zero_arr_size(_tmp163,8));}):({const char*_tmp164="long long";
_tag_arr(_tmp164,sizeof(char),_get_zero_arr_size(_tmp164,10));});goto _LLF9;_LLF9:;}
s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_String_pa_struct _tmp168;_tmp168.tag=
0;_tmp168.f1=(struct _tagged_arr)((struct _tagged_arr)ts);{struct Cyc_String_pa_struct
_tmp167;_tmp167.tag=0;_tmp167.f1=(struct _tagged_arr)((struct _tagged_arr)sns);{
void*_tmp165[2]={& _tmp167,& _tmp168};Cyc_aprintf(({const char*_tmp166="%s%s";
_tag_arr(_tmp166,sizeof(char),_get_zero_arr_size(_tmp166,5));}),_tag_arr(_tmp165,
sizeof(void*),2));}}}));goto _LLAB;}_LLBE: if((int)_tmp101 != 1)goto _LLC0;_LLBF: s=
Cyc_PP_text(({const char*_tmp169="float";_tag_arr(_tmp169,sizeof(char),
_get_zero_arr_size(_tmp169,6));}));goto _LLAB;_LLC0: if(_tmp101 <= (void*)3?1:*((
int*)_tmp101)!= 6)goto _LLC2;_tmp110=((struct Cyc_Absyn_DoubleType_struct*)_tmp101)->f1;
_LLC1: if(_tmp110)s=Cyc_PP_text(({const char*_tmp16A="long double";_tag_arr(
_tmp16A,sizeof(char),_get_zero_arr_size(_tmp16A,12));}));else{s=Cyc_PP_text(({
const char*_tmp16B="double";_tag_arr(_tmp16B,sizeof(char),_get_zero_arr_size(
_tmp16B,7));}));}goto _LLAB;_LLC2: if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 9)
goto _LLC4;_tmp111=((struct Cyc_Absyn_TupleType_struct*)_tmp101)->f1;_LLC3: s=({
struct Cyc_PP_Doc*_tmp16C[2];_tmp16C[1]=Cyc_Absynpp_args2doc(_tmp111);_tmp16C[0]=
Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_arr(_tmp16C,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLAB;_LLC4: if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 10)goto _LLC6;_tmp112=((
struct Cyc_Absyn_AggrType_struct*)_tmp101)->f1;_tmp113=(void*)_tmp112.aggr_info;
_tmp114=_tmp112.targs;_LLC5: {void*_tmp16E;struct _tuple0*_tmp16F;struct _tuple3
_tmp16D=Cyc_Absyn_aggr_kinded_name(_tmp113);_tmp16E=_tmp16D.f1;_tmp16F=_tmp16D.f2;
s=({struct Cyc_PP_Doc*_tmp170[3];_tmp170[2]=Cyc_Absynpp_tps2doc(_tmp114);_tmp170[
1]=Cyc_Absynpp_qvar2doc(_tmp16F);_tmp170[0]=Cyc_Absynpp_aggr_kind2doc(_tmp16E);
Cyc_PP_cat(_tag_arr(_tmp170,sizeof(struct Cyc_PP_Doc*),3));});goto _LLAB;}_LLC6:
if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 11)goto _LLC8;_tmp115=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp101)->f1;_tmp116=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp101)->f2;_LLC7: s=({struct Cyc_PP_Doc*_tmp171[4];_tmp171[3]=Cyc_Absynpp_rb();
_tmp171[2]=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp116));_tmp171[1]=Cyc_Absynpp_lb();
_tmp171[0]=Cyc_Absynpp_aggr_kind2doc(_tmp115);Cyc_PP_cat(_tag_arr(_tmp171,
sizeof(struct Cyc_PP_Doc*),4));});goto _LLAB;_LLC8: if(_tmp101 <= (void*)3?1:*((int*)
_tmp101)!= 13)goto _LLCA;_tmp117=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp101)->f1;
_LLC9: s=({struct Cyc_PP_Doc*_tmp172[4];_tmp172[3]=Cyc_Absynpp_rb();_tmp172[2]=Cyc_PP_nest(
2,Cyc_Absynpp_enumfields2doc(_tmp117));_tmp172[1]=Cyc_Absynpp_lb();_tmp172[0]=
Cyc_PP_text(({const char*_tmp173="enum ";_tag_arr(_tmp173,sizeof(char),
_get_zero_arr_size(_tmp173,6));}));Cyc_PP_cat(_tag_arr(_tmp172,sizeof(struct Cyc_PP_Doc*),
4));});goto _LLAB;_LLCA: if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 12)goto _LLCC;
_tmp118=((struct Cyc_Absyn_EnumType_struct*)_tmp101)->f1;_LLCB: s=({struct Cyc_PP_Doc*
_tmp174[2];_tmp174[1]=Cyc_Absynpp_qvar2doc(_tmp118);_tmp174[0]=Cyc_PP_text(({
const char*_tmp175="enum ";_tag_arr(_tmp175,sizeof(char),_get_zero_arr_size(
_tmp175,6));}));Cyc_PP_cat(_tag_arr(_tmp174,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLAB;_LLCC: if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 14)goto _LLCE;_tmp119=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp101)->f1;_LLCD: s=({struct Cyc_PP_Doc*
_tmp176[3];_tmp176[2]=Cyc_PP_text(({const char*_tmp178=">";_tag_arr(_tmp178,
sizeof(char),_get_zero_arr_size(_tmp178,2));}));_tmp176[1]=Cyc_Absynpp_typ2doc(
_tmp119);_tmp176[0]=Cyc_PP_text(({const char*_tmp177="sizeof_t<";_tag_arr(_tmp177,
sizeof(char),_get_zero_arr_size(_tmp177,10));}));Cyc_PP_cat(_tag_arr(_tmp176,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLAB;_LLCE: if(_tmp101 <= (void*)3?1:*((int*)
_tmp101)!= 16)goto _LLD0;_tmp11A=((struct Cyc_Absyn_TypedefType_struct*)_tmp101)->f1;
_tmp11B=((struct Cyc_Absyn_TypedefType_struct*)_tmp101)->f2;_tmp11C=((struct Cyc_Absyn_TypedefType_struct*)
_tmp101)->f3;_tmp11D=((struct Cyc_Absyn_TypedefType_struct*)_tmp101)->f4;_LLCF: s=({
struct Cyc_PP_Doc*_tmp179[2];_tmp179[1]=Cyc_Absynpp_tps2doc(_tmp11B);_tmp179[0]=
Cyc_Absynpp_qvar2doc(_tmp11A);Cyc_PP_cat(_tag_arr(_tmp179,sizeof(struct Cyc_PP_Doc*),
2));});goto _LLAB;_LLD0: if(_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 15)goto _LLD2;
_tmp11E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp101)->f1;_LLD1: s=({
struct Cyc_PP_Doc*_tmp17A[3];_tmp17A[2]=Cyc_PP_text(({const char*_tmp17C=">";
_tag_arr(_tmp17C,sizeof(char),_get_zero_arr_size(_tmp17C,2));}));_tmp17A[1]=Cyc_Absynpp_rgn2doc(
_tmp11E);_tmp17A[0]=Cyc_PP_text(({const char*_tmp17B="region_t<";_tag_arr(_tmp17B,
sizeof(char),_get_zero_arr_size(_tmp17B,10));}));Cyc_PP_cat(_tag_arr(_tmp17A,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLAB;_LLD2: if(_tmp101 <= (void*)3?1:*((int*)
_tmp101)!= 18)goto _LLD4;_tmp11F=((struct Cyc_Absyn_TypeInt_struct*)_tmp101)->f1;
_LLD3: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp17F;_tmp17F.tag=
1;_tmp17F.f1=(unsigned int)_tmp11F;{void*_tmp17D[1]={& _tmp17F};Cyc_aprintf(({
const char*_tmp17E="`%d";_tag_arr(_tmp17E,sizeof(char),_get_zero_arr_size(_tmp17E,
4));}),_tag_arr(_tmp17D,sizeof(void*),1));}}));goto _LLAB;_LLD4: if(_tmp101 <= (
void*)3?1:*((int*)_tmp101)!= 17)goto _LLD6;_tmp120=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp101)->f1;_LLD5: s=({struct Cyc_PP_Doc*_tmp180[3];_tmp180[2]=Cyc_PP_text(({
const char*_tmp182=">";_tag_arr(_tmp182,sizeof(char),_get_zero_arr_size(_tmp182,2));}));
_tmp180[1]=Cyc_Absynpp_typ2doc(_tmp120);_tmp180[0]=Cyc_PP_text(({const char*
_tmp181="tag_t<";_tag_arr(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,7));}));
Cyc_PP_cat(_tag_arr(_tmp180,sizeof(struct Cyc_PP_Doc*),3));});goto _LLAB;_LLD6: if((
int)_tmp101 != 2)goto _LLD8;_LLD7: s=Cyc_Absynpp_rgn2doc(t);goto _LLAB;_LLD8: if(
_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 21)goto _LLDA;_tmp121=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp101)->f1;_LLD9: s=({struct Cyc_PP_Doc*_tmp183[3];_tmp183[2]=Cyc_PP_text(({
const char*_tmp185=")";_tag_arr(_tmp185,sizeof(char),_get_zero_arr_size(_tmp185,2));}));
_tmp183[1]=Cyc_Absynpp_typ2doc(_tmp121);_tmp183[0]=Cyc_PP_text(({const char*
_tmp184="regions(";_tag_arr(_tmp184,sizeof(char),_get_zero_arr_size(_tmp184,9));}));
Cyc_PP_cat(_tag_arr(_tmp183,sizeof(struct Cyc_PP_Doc*),3));});goto _LLAB;_LLDA: if(
_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 19)goto _LLDC;_LLDB: goto _LLDD;_LLDC: if(
_tmp101 <= (void*)3?1:*((int*)_tmp101)!= 20)goto _LLAB;_LLDD: s=Cyc_Absynpp_eff2doc(
t);goto _LLAB;_LLAB:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*
vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _tagged_arr*)((struct
_tagged_arr*)vo->v)));}struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple5*
cmp){struct _tuple5 _tmp187;void*_tmp188;void*_tmp189;struct _tuple5*_tmp186=cmp;
_tmp187=*_tmp186;_tmp188=_tmp187.f1;_tmp189=_tmp187.f2;return({struct Cyc_PP_Doc*
_tmp18A[3];_tmp18A[2]=Cyc_Absynpp_rgn2doc(_tmp189);_tmp18A[1]=Cyc_PP_text(({
const char*_tmp18B=" > ";_tag_arr(_tmp18B,sizeof(char),_get_zero_arr_size(_tmp18B,
4));}));_tmp18A[0]=Cyc_Absynpp_rgn2doc(_tmp188);Cyc_PP_cat(_tag_arr(_tmp18A,
sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct
Cyc_List_List*po){return Cyc_PP_group(({const char*_tmp18C="";_tag_arr(_tmp18C,
sizeof(char),_get_zero_arr_size(_tmp18C,1));}),({const char*_tmp18D="";_tag_arr(
_tmp18D,sizeof(char),_get_zero_arr_size(_tmp18D,1));}),({const char*_tmp18E=",";
_tag_arr(_tmp18E,sizeof(char),_get_zero_arr_size(_tmp18E,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,
po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple1*t){struct Cyc_Core_Opt*
dopt=(*((struct _tuple1*)t)).f1 == 0?0:({struct Cyc_Core_Opt*_tmp18F=_cycalloc(
sizeof(*_tmp18F));_tmp18F->v=Cyc_PP_text(*((struct _tagged_arr*)((struct
_tagged_arr*)((struct Cyc_Core_Opt*)_check_null((*((struct _tuple1*)t)).f1))->v)));
_tmp18F;});return Cyc_Absynpp_tqtd2doc((*((struct _tuple1*)t)).f2,(*((struct
_tuple1*)t)).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp190=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs)_tmp190=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp190,({struct
Cyc_List_List*_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191->hd=Cyc_PP_text(({
const char*_tmp192="...";_tag_arr(_tmp192,sizeof(char),_get_zero_arr_size(_tmp192,
4));}));_tmp191->tl=0;_tmp191;}));else{if(cyc_varargs != 0){struct Cyc_PP_Doc*
_tmp193=({struct Cyc_PP_Doc*_tmp195[3];_tmp195[2]=Cyc_Absynpp_funarg2doc(({struct
_tuple1*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->f1=cyc_varargs->name;
_tmp199->f2=cyc_varargs->tq;_tmp199->f3=(void*)cyc_varargs->type;_tmp199;}));
_tmp195[1]=cyc_varargs->inject?Cyc_PP_text(({const char*_tmp197=" inject ";
_tag_arr(_tmp197,sizeof(char),_get_zero_arr_size(_tmp197,9));})): Cyc_PP_text(({
const char*_tmp198=" ";_tag_arr(_tmp198,sizeof(char),_get_zero_arr_size(_tmp198,2));}));
_tmp195[0]=Cyc_PP_text(({const char*_tmp196="...";_tag_arr(_tmp196,sizeof(char),
_get_zero_arr_size(_tmp196,4));}));Cyc_PP_cat(_tag_arr(_tmp195,sizeof(struct Cyc_PP_Doc*),
3));});_tmp190=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp190,({struct Cyc_List_List*_tmp194=_cycalloc(sizeof(*
_tmp194));_tmp194->hd=_tmp193;_tmp194->tl=0;_tmp194;}));}}{struct Cyc_PP_Doc*
_tmp19A=Cyc_PP_group(({const char*_tmp1A2="";_tag_arr(_tmp1A2,sizeof(char),
_get_zero_arr_size(_tmp1A2,1));}),({const char*_tmp1A3="";_tag_arr(_tmp1A3,
sizeof(char),_get_zero_arr_size(_tmp1A3,1));}),({const char*_tmp1A4=",";_tag_arr(
_tmp1A4,sizeof(char),_get_zero_arr_size(_tmp1A4,2));}),_tmp190);if(effopt != 0)
_tmp19A=({struct Cyc_PP_Doc*_tmp19B[3];_tmp19B[2]=Cyc_Absynpp_eff2doc((void*)
effopt->v);_tmp19B[1]=Cyc_PP_text(({const char*_tmp19C=";";_tag_arr(_tmp19C,
sizeof(char),_get_zero_arr_size(_tmp19C,2));}));_tmp19B[0]=_tmp19A;Cyc_PP_cat(
_tag_arr(_tmp19B,sizeof(struct Cyc_PP_Doc*),3));});if(rgn_po != 0)_tmp19A=({struct
Cyc_PP_Doc*_tmp19D[3];_tmp19D[2]=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp19D[1]=Cyc_PP_text(({
const char*_tmp19E=":";_tag_arr(_tmp19E,sizeof(char),_get_zero_arr_size(_tmp19E,2));}));
_tmp19D[0]=_tmp19A;Cyc_PP_cat(_tag_arr(_tmp19D,sizeof(struct Cyc_PP_Doc*),3));});
return({struct Cyc_PP_Doc*_tmp19F[3];_tmp19F[2]=Cyc_PP_text(({const char*_tmp1A1=")";
_tag_arr(_tmp1A1,sizeof(char),_get_zero_arr_size(_tmp1A1,2));}));_tmp19F[1]=
_tmp19A;_tmp19F[0]=Cyc_PP_text(({const char*_tmp1A0="(";_tag_arr(_tmp1A0,sizeof(
char),_get_zero_arr_size(_tmp1A0,2));}));Cyc_PP_cat(_tag_arr(_tmp19F,sizeof(
struct Cyc_PP_Doc*),3));});}}struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*
arg){return({struct _tuple1*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5->f1=({
struct Cyc_Core_Opt*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->v=(*((struct
_tuple6*)arg)).f1;_tmp1A6;});_tmp1A5->f2=(*((struct _tuple6*)arg)).f2;_tmp1A5->f3=(*((
struct _tuple6*)arg)).f3;_tmp1A5;});}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_tagged_arr*v){return Cyc_PP_text(*((struct _tagged_arr*)v));}struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*q){struct Cyc_List_List*_tmp1A7=0;int match;{void*_tmp1A8=(*((struct
_tuple0*)q)).f1;struct Cyc_List_List*_tmp1A9;struct Cyc_List_List*_tmp1AA;_LL10A:
if((int)_tmp1A8 != 0)goto _LL10C;_LL10B: _tmp1A9=0;goto _LL10D;_LL10C: if(_tmp1A8 <= (
void*)1?1:*((int*)_tmp1A8)!= 0)goto _LL10E;_tmp1A9=((struct Cyc_Absyn_Rel_n_struct*)
_tmp1A8)->f1;_LL10D: match=0;_tmp1A7=_tmp1A9;goto _LL109;_LL10E: if(_tmp1A8 <= (void*)
1?1:*((int*)_tmp1A8)!= 1)goto _LL109;_tmp1AA=((struct Cyc_Absyn_Abs_n_struct*)
_tmp1A8)->f1;_LL10F: match=Cyc_Absynpp_use_curr_namespace?((int(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1AA,Cyc_Absynpp_curr_namespace): 0;
_tmp1A7=(Cyc_Absynpp_qvar_to_Cids?Cyc_Absynpp_add_cyc_prefix: 0)?({struct Cyc_List_List*
_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->hd=Cyc_Absynpp_cyc_stringptr;
_tmp1AB->tl=_tmp1AA;_tmp1AB;}): _tmp1AA;goto _LL109;_LL109:;}if(Cyc_Absynpp_qvar_to_Cids)
return(struct _tagged_arr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1A7,({struct Cyc_List_List*_tmp1AC=
_cycalloc(sizeof(*_tmp1AC));_tmp1AC->hd=(*((struct _tuple0*)q)).f2;_tmp1AC->tl=0;
_tmp1AC;})),({const char*_tmp1AD="_";_tag_arr(_tmp1AD,sizeof(char),
_get_zero_arr_size(_tmp1AD,2));}));else{if(match)return*((struct _tagged_arr*)(*((
struct _tuple0*)q)).f2);else{return(struct _tagged_arr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1A7,({struct
Cyc_List_List*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));_tmp1AE->hd=(*((struct _tuple0*)
q)).f2;_tmp1AE->tl=0;_tmp1AE;})),({const char*_tmp1AF="::";_tag_arr(_tmp1AF,
sizeof(char),_get_zero_arr_size(_tmp1AF,3));}));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*
Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){struct _tagged_arr _tmp1B0=Cyc_Absynpp_qvar2string(
q);if(Cyc_PP_tex_output)return Cyc_PP_text_width((struct _tagged_arr)Cyc_strconcat((
struct _tagged_arr)Cyc_strconcat(({const char*_tmp1B1="\\textbf{";_tag_arr(_tmp1B1,
sizeof(char),_get_zero_arr_size(_tmp1B1,9));}),(struct _tagged_arr)_tmp1B0),({
const char*_tmp1B2="}";_tag_arr(_tmp1B2,sizeof(char),_get_zero_arr_size(_tmp1B2,2));})),(
int)Cyc_strlen((struct _tagged_arr)_tmp1B0));else{return Cyc_PP_text(_tmp1B0);}}
struct _tagged_arr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)
return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){void*_tmp1B3=(*((
struct _tuple0*)v)).f1;struct Cyc_List_List*_tmp1B4;struct Cyc_List_List*_tmp1B5;
_LL111: if((int)_tmp1B3 != 0)goto _LL113;_LL112: goto _LL114;_LL113: if(_tmp1B3 <= (
void*)1?1:*((int*)_tmp1B3)!= 0)goto _LL115;_tmp1B4=((struct Cyc_Absyn_Rel_n_struct*)
_tmp1B3)->f1;if(_tmp1B4 != 0)goto _LL115;_LL114: return*((struct _tagged_arr*)(*((
struct _tuple0*)v)).f2);_LL115: if(_tmp1B3 <= (void*)1?1:*((int*)_tmp1B3)!= 1)goto
_LL117;_tmp1B5=((struct Cyc_Absyn_Abs_n_struct*)_tmp1B3)->f1;_LL116: if(((int(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1B5,Cyc_Absynpp_curr_namespace)
== 0)return*((struct _tagged_arr*)(*((struct _tuple0*)v)).f2);else{goto _LL118;}
_LL117:;_LL118: return(struct _tagged_arr)Cyc_strconcat(({const char*_tmp1B6="/* bad namespace : */ ";
_tag_arr(_tmp1B6,sizeof(char),_get_zero_arr_size(_tmp1B6,23));}),(struct
_tagged_arr)Cyc_Absynpp_qvar2string(v));_LL110:;}else{return*((struct _tagged_arr*)(*((
struct _tuple0*)v)).f2);}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct
_tuple0*v){return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){struct _tagged_arr _tmp1B7=Cyc_Absynpp_typedef_name2string(
v);if(Cyc_PP_tex_output)return Cyc_PP_text_width((struct _tagged_arr)Cyc_strconcat((
struct _tagged_arr)Cyc_strconcat(({const char*_tmp1B8="\\textbf{";_tag_arr(_tmp1B8,
sizeof(char),_get_zero_arr_size(_tmp1B8,9));}),(struct _tagged_arr)_tmp1B7),({
const char*_tmp1B9="}";_tag_arr(_tmp1B9,sizeof(char),_get_zero_arr_size(_tmp1B9,2));})),(
int)Cyc_strlen((struct _tagged_arr)_tmp1B7));else{return Cyc_PP_text(_tmp1B7);}}
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(),
t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmp1BA=(void*)e->r;
void*_tmp1BB;struct Cyc_Absyn_Exp*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BD;_LL11A: if(*((
int*)_tmp1BA)!= 0)goto _LL11C;_LL11B: goto _LL11D;_LL11C: if(*((int*)_tmp1BA)!= 1)
goto _LL11E;_LL11D: goto _LL11F;_LL11E: if(*((int*)_tmp1BA)!= 2)goto _LL120;_LL11F:
return 10000;_LL120: if(*((int*)_tmp1BA)!= 3)goto _LL122;_tmp1BB=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1BA)->f1;_LL121: {void*_tmp1BE=_tmp1BB;_LL167: if((int)_tmp1BE != 0)goto _LL169;
_LL168: return 100;_LL169: if((int)_tmp1BE != 1)goto _LL16B;_LL16A: return 110;_LL16B:
if((int)_tmp1BE != 2)goto _LL16D;_LL16C: return 100;_LL16D: if((int)_tmp1BE != 3)goto
_LL16F;_LL16E: goto _LL170;_LL16F: if((int)_tmp1BE != 4)goto _LL171;_LL170: return 110;
_LL171: if((int)_tmp1BE != 5)goto _LL173;_LL172: goto _LL174;_LL173: if((int)_tmp1BE != 
6)goto _LL175;_LL174: return 70;_LL175: if((int)_tmp1BE != 7)goto _LL177;_LL176: goto
_LL178;_LL177: if((int)_tmp1BE != 8)goto _LL179;_LL178: goto _LL17A;_LL179: if((int)
_tmp1BE != 9)goto _LL17B;_LL17A: goto _LL17C;_LL17B: if((int)_tmp1BE != 10)goto _LL17D;
_LL17C: return 80;_LL17D: if((int)_tmp1BE != 11)goto _LL17F;_LL17E: goto _LL180;_LL17F:
if((int)_tmp1BE != 12)goto _LL181;_LL180: return 130;_LL181: if((int)_tmp1BE != 13)
goto _LL183;_LL182: return 60;_LL183: if((int)_tmp1BE != 14)goto _LL185;_LL184: return
40;_LL185: if((int)_tmp1BE != 15)goto _LL187;_LL186: return 50;_LL187: if((int)_tmp1BE
!= 16)goto _LL189;_LL188: return 90;_LL189: if((int)_tmp1BE != 17)goto _LL18B;_LL18A:
return 80;_LL18B: if((int)_tmp1BE != 18)goto _LL18D;_LL18C: return 80;_LL18D: if((int)
_tmp1BE != 19)goto _LL166;_LL18E: return 140;_LL166:;}_LL122: if(*((int*)_tmp1BA)!= 4)
goto _LL124;_LL123: return 20;_LL124: if(*((int*)_tmp1BA)!= 5)goto _LL126;_LL125:
return 130;_LL126: if(*((int*)_tmp1BA)!= 6)goto _LL128;_LL127: return 30;_LL128: if(*((
int*)_tmp1BA)!= 7)goto _LL12A;_LL129: return 10;_LL12A: if(*((int*)_tmp1BA)!= 8)goto
_LL12C;_LL12B: goto _LL12D;_LL12C: if(*((int*)_tmp1BA)!= 9)goto _LL12E;_LL12D: return
140;_LL12E: if(*((int*)_tmp1BA)!= 10)goto _LL130;_LL12F: return 130;_LL130: if(*((int*)
_tmp1BA)!= 11)goto _LL132;_tmp1BC=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp1BA)->f1;_LL131: return Cyc_Absynpp_exp_prec(_tmp1BC);_LL132: if(*((int*)
_tmp1BA)!= 12)goto _LL134;_tmp1BD=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1BA)->f1;
_LL133: return Cyc_Absynpp_exp_prec(_tmp1BD);_LL134: if(*((int*)_tmp1BA)!= 13)goto
_LL136;_LL135: return 120;_LL136: if(*((int*)_tmp1BA)!= 15)goto _LL138;_LL137: return
15;_LL138: if(*((int*)_tmp1BA)!= 14)goto _LL13A;_LL139: goto _LL13B;_LL13A: if(*((int*)
_tmp1BA)!= 16)goto _LL13C;_LL13B: goto _LL13D;_LL13C: if(*((int*)_tmp1BA)!= 17)goto
_LL13E;_LL13D: goto _LL13F;_LL13E: if(*((int*)_tmp1BA)!= 18)goto _LL140;_LL13F: goto
_LL141;_LL140: if(*((int*)_tmp1BA)!= 19)goto _LL142;_LL141: goto _LL143;_LL142: if(*((
int*)_tmp1BA)!= 20)goto _LL144;_LL143: return 130;_LL144: if(*((int*)_tmp1BA)!= 21)
goto _LL146;_LL145: goto _LL147;_LL146: if(*((int*)_tmp1BA)!= 22)goto _LL148;_LL147:
goto _LL149;_LL148: if(*((int*)_tmp1BA)!= 23)goto _LL14A;_LL149: return 140;_LL14A:
if(*((int*)_tmp1BA)!= 24)goto _LL14C;_LL14B: return 150;_LL14C: if(*((int*)_tmp1BA)
!= 25)goto _LL14E;_LL14D: goto _LL14F;_LL14E: if(*((int*)_tmp1BA)!= 26)goto _LL150;
_LL14F: goto _LL151;_LL150: if(*((int*)_tmp1BA)!= 27)goto _LL152;_LL151: goto _LL153;
_LL152: if(*((int*)_tmp1BA)!= 28)goto _LL154;_LL153: goto _LL155;_LL154: if(*((int*)
_tmp1BA)!= 29)goto _LL156;_LL155: goto _LL157;_LL156: if(*((int*)_tmp1BA)!= 30)goto
_LL158;_LL157: goto _LL159;_LL158: if(*((int*)_tmp1BA)!= 31)goto _LL15A;_LL159: goto
_LL15B;_LL15A: if(*((int*)_tmp1BA)!= 32)goto _LL15C;_LL15B: goto _LL15D;_LL15C: if(*((
int*)_tmp1BA)!= 33)goto _LL15E;_LL15D: goto _LL15F;_LL15E: if(*((int*)_tmp1BA)!= 34)
goto _LL160;_LL15F: return 140;_LL160: if(*((int*)_tmp1BA)!= 35)goto _LL162;_LL161:
return 10000;_LL162: if(*((int*)_tmp1BA)!= 36)goto _LL164;_LL163: goto _LL165;_LL164:
if(*((int*)_tmp1BA)!= 37)goto _LL119;_LL165: return 140;_LL119:;}struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){int
myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*s;{void*_tmp1BF=(void*)e->r;void*
_tmp1C0;struct _tuple0*_tmp1C1;struct _tuple0*_tmp1C2;void*_tmp1C3;struct Cyc_List_List*
_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C5;struct Cyc_Core_Opt*_tmp1C6;struct Cyc_Absyn_Exp*
_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C8;void*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1CA;
struct Cyc_Absyn_Exp*_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CC;struct Cyc_Absyn_Exp*
_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CE;struct Cyc_Absyn_Exp*_tmp1CF;struct Cyc_List_List*
_tmp1D0;struct Cyc_Absyn_Exp*_tmp1D1;struct Cyc_List_List*_tmp1D2;struct Cyc_Absyn_Exp*
_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D5;void*_tmp1D6;
struct Cyc_Absyn_Exp*_tmp1D7;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_Absyn_Exp*
_tmp1D9;struct Cyc_Absyn_Exp*_tmp1DA;void*_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DC;
void*_tmp1DD;void*_tmp1DE;struct _tagged_arr*_tmp1DF;void*_tmp1E0;void*_tmp1E1;
unsigned int _tmp1E2;struct Cyc_List_List*_tmp1E3;void*_tmp1E4;struct Cyc_Absyn_Exp*
_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E6;struct _tagged_arr*_tmp1E7;struct Cyc_Absyn_Exp*
_tmp1E8;struct _tagged_arr*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_Absyn_Exp*
_tmp1EB;struct Cyc_List_List*_tmp1EC;struct _tuple1*_tmp1ED;struct Cyc_List_List*
_tmp1EE;struct Cyc_List_List*_tmp1EF;struct Cyc_Absyn_Vardecl*_tmp1F0;struct Cyc_Absyn_Exp*
_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;struct _tuple0*_tmp1F3;struct Cyc_List_List*
_tmp1F4;struct Cyc_List_List*_tmp1F5;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*
_tmp1F7;struct Cyc_Absyn_Tunionfield*_tmp1F8;struct _tuple0*_tmp1F9;struct _tuple0*
_tmp1FA;struct Cyc_Absyn_MallocInfo _tmp1FB;int _tmp1FC;struct Cyc_Absyn_Exp*_tmp1FD;
void**_tmp1FE;struct Cyc_Absyn_Exp*_tmp1FF;struct Cyc_Core_Opt*_tmp200;struct Cyc_List_List*
_tmp201;struct Cyc_Absyn_Stmt*_tmp202;struct Cyc_Absyn_Fndecl*_tmp203;struct Cyc_Absyn_Exp*
_tmp204;_LL190: if(*((int*)_tmp1BF)!= 0)goto _LL192;_tmp1C0=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1BF)->f1;_LL191: s=Cyc_Absynpp_cnst2doc(_tmp1C0);goto _LL18F;_LL192: if(*((int*)
_tmp1BF)!= 1)goto _LL194;_tmp1C1=((struct Cyc_Absyn_Var_e_struct*)_tmp1BF)->f1;
_LL193: _tmp1C2=_tmp1C1;goto _LL195;_LL194: if(*((int*)_tmp1BF)!= 2)goto _LL196;
_tmp1C2=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp1BF)->f1;_LL195: s=Cyc_Absynpp_qvar2doc(
_tmp1C2);goto _LL18F;_LL196: if(*((int*)_tmp1BF)!= 3)goto _LL198;_tmp1C3=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp1BF)->f1;_tmp1C4=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1BF)->f2;_LL197: s=Cyc_Absynpp_primapp2doc(myprec,_tmp1C3,_tmp1C4);goto _LL18F;
_LL198: if(*((int*)_tmp1BF)!= 4)goto _LL19A;_tmp1C5=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1BF)->f1;_tmp1C6=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1BF)->f2;_tmp1C7=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp1BF)->f3;_LL199: s=({struct Cyc_PP_Doc*
_tmp205[5];_tmp205[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1C7);_tmp205[3]=Cyc_PP_text(({
const char*_tmp208="= ";_tag_arr(_tmp208,sizeof(char),_get_zero_arr_size(_tmp208,
3));}));_tmp205[2]=_tmp1C6 == 0?Cyc_PP_text(({const char*_tmp207="";_tag_arr(
_tmp207,sizeof(char),_get_zero_arr_size(_tmp207,1));})): Cyc_Absynpp_prim2doc((
void*)_tmp1C6->v);_tmp205[1]=Cyc_PP_text(({const char*_tmp206=" ";_tag_arr(
_tmp206,sizeof(char),_get_zero_arr_size(_tmp206,2));}));_tmp205[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1C5);Cyc_PP_cat(_tag_arr(_tmp205,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL18F;_LL19A: if(*((int*)_tmp1BF)!= 5)goto _LL19C;_tmp1C8=((struct Cyc_Absyn_Increment_e_struct*)
_tmp1BF)->f1;_tmp1C9=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1BF)->f2;
_LL19B: {struct Cyc_PP_Doc*_tmp209=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1C8);{void*
_tmp20A=_tmp1C9;_LL1DF: if((int)_tmp20A != 0)goto _LL1E1;_LL1E0: s=({struct Cyc_PP_Doc*
_tmp20B[2];_tmp20B[1]=_tmp209;_tmp20B[0]=Cyc_PP_text(({const char*_tmp20C="++";
_tag_arr(_tmp20C,sizeof(char),_get_zero_arr_size(_tmp20C,3));}));Cyc_PP_cat(
_tag_arr(_tmp20B,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1DE;_LL1E1: if((int)
_tmp20A != 2)goto _LL1E3;_LL1E2: s=({struct Cyc_PP_Doc*_tmp20D[2];_tmp20D[1]=_tmp209;
_tmp20D[0]=Cyc_PP_text(({const char*_tmp20E="--";_tag_arr(_tmp20E,sizeof(char),
_get_zero_arr_size(_tmp20E,3));}));Cyc_PP_cat(_tag_arr(_tmp20D,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL1DE;_LL1E3: if((int)_tmp20A != 1)goto _LL1E5;_LL1E4: s=({struct Cyc_PP_Doc*
_tmp20F[2];_tmp20F[1]=Cyc_PP_text(({const char*_tmp210="++";_tag_arr(_tmp210,
sizeof(char),_get_zero_arr_size(_tmp210,3));}));_tmp20F[0]=_tmp209;Cyc_PP_cat(
_tag_arr(_tmp20F,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1DE;_LL1E5: if((int)
_tmp20A != 3)goto _LL1DE;_LL1E6: s=({struct Cyc_PP_Doc*_tmp211[2];_tmp211[1]=Cyc_PP_text(({
const char*_tmp212="--";_tag_arr(_tmp212,sizeof(char),_get_zero_arr_size(_tmp212,
3));}));_tmp211[0]=_tmp209;Cyc_PP_cat(_tag_arr(_tmp211,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL1DE;_LL1DE:;}goto _LL18F;}_LL19C: if(*((int*)_tmp1BF)!= 6)goto _LL19E;
_tmp1CA=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1BF)->f1;_tmp1CB=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1BF)->f2;_tmp1CC=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1BF)->f3;_LL19D:{
struct _tuple5 _tmp214=({struct _tuple5 _tmp213;_tmp213.f1=(void*)_tmp1CB->r;_tmp213.f2=(
void*)_tmp1CC->r;_tmp213;});_LL1E8:;_LL1E9: s=({struct Cyc_PP_Doc*_tmp215[5];
_tmp215[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1CC);_tmp215[3]=Cyc_PP_text(({
const char*_tmp217=" : ";_tag_arr(_tmp217,sizeof(char),_get_zero_arr_size(_tmp217,
4));}));_tmp215[2]=Cyc_Absynpp_exp2doc_prec(0,_tmp1CB);_tmp215[1]=Cyc_PP_text(({
const char*_tmp216=" ? ";_tag_arr(_tmp216,sizeof(char),_get_zero_arr_size(_tmp216,
4));}));_tmp215[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1CA);Cyc_PP_cat(_tag_arr(
_tmp215,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1E7;_LL1E7:;}goto _LL18F;_LL19E:
if(*((int*)_tmp1BF)!= 7)goto _LL1A0;_tmp1CD=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1BF)->f1;_tmp1CE=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1BF)->f2;_LL19F: s=({
struct Cyc_PP_Doc*_tmp218[5];_tmp218[4]=Cyc_PP_text(({const char*_tmp21B=")";
_tag_arr(_tmp21B,sizeof(char),_get_zero_arr_size(_tmp21B,2));}));_tmp218[3]=Cyc_Absynpp_exp2doc(
_tmp1CE);_tmp218[2]=Cyc_PP_text(({const char*_tmp21A=", ";_tag_arr(_tmp21A,
sizeof(char),_get_zero_arr_size(_tmp21A,3));}));_tmp218[1]=Cyc_Absynpp_exp2doc(
_tmp1CD);_tmp218[0]=Cyc_PP_text(({const char*_tmp219="(";_tag_arr(_tmp219,sizeof(
char),_get_zero_arr_size(_tmp219,2));}));Cyc_PP_cat(_tag_arr(_tmp218,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL18F;_LL1A0: if(*((int*)_tmp1BF)!= 8)goto _LL1A2;
_tmp1CF=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1BF)->f1;_tmp1D0=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp1BF)->f2;_LL1A1: s=({struct Cyc_PP_Doc*_tmp21C[4];_tmp21C[3]=Cyc_PP_text(({
const char*_tmp21E=")";_tag_arr(_tmp21E,sizeof(char),_get_zero_arr_size(_tmp21E,2));}));
_tmp21C[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp1D0);_tmp21C[1]=Cyc_PP_text(({const
char*_tmp21D="(";_tag_arr(_tmp21D,sizeof(char),_get_zero_arr_size(_tmp21D,2));}));
_tmp21C[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1CF);Cyc_PP_cat(_tag_arr(_tmp21C,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL18F;_LL1A2: if(*((int*)_tmp1BF)!= 9)goto
_LL1A4;_tmp1D1=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1BF)->f1;_tmp1D2=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp1BF)->f2;_LL1A3: s=({struct Cyc_PP_Doc*_tmp21F[4];
_tmp21F[3]=Cyc_PP_text(({const char*_tmp221=")";_tag_arr(_tmp221,sizeof(char),
_get_zero_arr_size(_tmp221,2));}));_tmp21F[2]=Cyc_Absynpp_exps2doc_prec(20,
_tmp1D2);_tmp21F[1]=Cyc_PP_text(({const char*_tmp220="(";_tag_arr(_tmp220,sizeof(
char),_get_zero_arr_size(_tmp220,2));}));_tmp21F[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1D1);Cyc_PP_cat(_tag_arr(_tmp21F,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL18F;_LL1A4: if(*((int*)_tmp1BF)!= 10)goto _LL1A6;_tmp1D3=((struct Cyc_Absyn_Throw_e_struct*)
_tmp1BF)->f1;_LL1A5: s=({struct Cyc_PP_Doc*_tmp222[2];_tmp222[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1D3);_tmp222[0]=Cyc_PP_text(({const char*_tmp223="throw ";_tag_arr(
_tmp223,sizeof(char),_get_zero_arr_size(_tmp223,7));}));Cyc_PP_cat(_tag_arr(
_tmp222,sizeof(struct Cyc_PP_Doc*),2));});goto _LL18F;_LL1A6: if(*((int*)_tmp1BF)!= 
11)goto _LL1A8;_tmp1D4=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1BF)->f1;
_LL1A7: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1D4);goto _LL18F;_LL1A8: if(*((int*)
_tmp1BF)!= 12)goto _LL1AA;_tmp1D5=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1BF)->f1;
_LL1A9: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1D5);goto _LL18F;_LL1AA: if(*((int*)
_tmp1BF)!= 13)goto _LL1AC;_tmp1D6=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1BF)->f1;
_tmp1D7=((struct Cyc_Absyn_Cast_e_struct*)_tmp1BF)->f2;_LL1AB: s=({struct Cyc_PP_Doc*
_tmp224[4];_tmp224[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1D7);_tmp224[2]=Cyc_PP_text(({
const char*_tmp226=")";_tag_arr(_tmp226,sizeof(char),_get_zero_arr_size(_tmp226,2));}));
_tmp224[1]=Cyc_Absynpp_typ2doc(_tmp1D6);_tmp224[0]=Cyc_PP_text(({const char*
_tmp225="(";_tag_arr(_tmp225,sizeof(char),_get_zero_arr_size(_tmp225,2));}));Cyc_PP_cat(
_tag_arr(_tmp224,sizeof(struct Cyc_PP_Doc*),4));});goto _LL18F;_LL1AC: if(*((int*)
_tmp1BF)!= 14)goto _LL1AE;_tmp1D8=((struct Cyc_Absyn_Address_e_struct*)_tmp1BF)->f1;
_LL1AD: s=({struct Cyc_PP_Doc*_tmp227[2];_tmp227[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1D8);_tmp227[0]=Cyc_PP_text(({const char*_tmp228="&";_tag_arr(_tmp228,
sizeof(char),_get_zero_arr_size(_tmp228,2));}));Cyc_PP_cat(_tag_arr(_tmp227,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL18F;_LL1AE: if(*((int*)_tmp1BF)!= 15)goto
_LL1B0;_tmp1D9=((struct Cyc_Absyn_New_e_struct*)_tmp1BF)->f1;_tmp1DA=((struct Cyc_Absyn_New_e_struct*)
_tmp1BF)->f2;_LL1AF: if(_tmp1D9 == 0)s=({struct Cyc_PP_Doc*_tmp229[2];_tmp229[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DA);_tmp229[0]=Cyc_PP_text(({const char*
_tmp22A="new ";_tag_arr(_tmp22A,sizeof(char),_get_zero_arr_size(_tmp22A,5));}));
Cyc_PP_cat(_tag_arr(_tmp229,sizeof(struct Cyc_PP_Doc*),2));});else{s=({struct Cyc_PP_Doc*
_tmp22B[4];_tmp22B[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DA);_tmp22B[2]=Cyc_PP_text(({
const char*_tmp22D=") ";_tag_arr(_tmp22D,sizeof(char),_get_zero_arr_size(_tmp22D,
3));}));_tmp22B[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(_tmp1D9));
_tmp22B[0]=Cyc_PP_text(({const char*_tmp22C="rnew(";_tag_arr(_tmp22C,sizeof(char),
_get_zero_arr_size(_tmp22C,6));}));Cyc_PP_cat(_tag_arr(_tmp22B,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL18F;_LL1B0: if(*((int*)_tmp1BF)!= 16)goto _LL1B2;_tmp1DB=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp1BF)->f1;_LL1B1: s=({struct Cyc_PP_Doc*
_tmp22E[3];_tmp22E[2]=Cyc_PP_text(({const char*_tmp230=")";_tag_arr(_tmp230,
sizeof(char),_get_zero_arr_size(_tmp230,2));}));_tmp22E[1]=Cyc_Absynpp_typ2doc(
_tmp1DB);_tmp22E[0]=Cyc_PP_text(({const char*_tmp22F="sizeof(";_tag_arr(_tmp22F,
sizeof(char),_get_zero_arr_size(_tmp22F,8));}));Cyc_PP_cat(_tag_arr(_tmp22E,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL18F;_LL1B2: if(*((int*)_tmp1BF)!= 17)goto
_LL1B4;_tmp1DC=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1BF)->f1;_LL1B3: s=({
struct Cyc_PP_Doc*_tmp231[3];_tmp231[2]=Cyc_PP_text(({const char*_tmp233=")";
_tag_arr(_tmp233,sizeof(char),_get_zero_arr_size(_tmp233,2));}));_tmp231[1]=Cyc_Absynpp_exp2doc(
_tmp1DC);_tmp231[0]=Cyc_PP_text(({const char*_tmp232="sizeof(";_tag_arr(_tmp232,
sizeof(char),_get_zero_arr_size(_tmp232,8));}));Cyc_PP_cat(_tag_arr(_tmp231,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL18F;_LL1B4: if(*((int*)_tmp1BF)!= 18)goto
_LL1B6;_tmp1DD=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1BF)->f1;_tmp1DE=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1BF)->f2;if(*((int*)_tmp1DE)!= 0)
goto _LL1B6;_tmp1DF=((struct Cyc_Absyn_StructField_struct*)_tmp1DE)->f1;_LL1B5: s=({
struct Cyc_PP_Doc*_tmp234[5];_tmp234[4]=Cyc_PP_text(({const char*_tmp237=")";
_tag_arr(_tmp237,sizeof(char),_get_zero_arr_size(_tmp237,2));}));_tmp234[3]=Cyc_PP_textptr(
_tmp1DF);_tmp234[2]=Cyc_PP_text(({const char*_tmp236=",";_tag_arr(_tmp236,sizeof(
char),_get_zero_arr_size(_tmp236,2));}));_tmp234[1]=Cyc_Absynpp_typ2doc(_tmp1DD);
_tmp234[0]=Cyc_PP_text(({const char*_tmp235="offsetof(";_tag_arr(_tmp235,sizeof(
char),_get_zero_arr_size(_tmp235,10));}));Cyc_PP_cat(_tag_arr(_tmp234,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL18F;_LL1B6: if(*((int*)_tmp1BF)!= 18)goto _LL1B8;
_tmp1E0=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1BF)->f1;_tmp1E1=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp1BF)->f2;if(*((int*)_tmp1E1)!= 1)goto
_LL1B8;_tmp1E2=((struct Cyc_Absyn_TupleIndex_struct*)_tmp1E1)->f1;_LL1B7: s=({
struct Cyc_PP_Doc*_tmp238[5];_tmp238[4]=Cyc_PP_text(({const char*_tmp23E=")";
_tag_arr(_tmp23E,sizeof(char),_get_zero_arr_size(_tmp23E,2));}));_tmp238[3]=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp23D;_tmp23D.tag=1;_tmp23D.f1=(
unsigned int)((int)_tmp1E2);{void*_tmp23B[1]={& _tmp23D};Cyc_aprintf(({const char*
_tmp23C="%d";_tag_arr(_tmp23C,sizeof(char),_get_zero_arr_size(_tmp23C,3));}),
_tag_arr(_tmp23B,sizeof(void*),1));}}));_tmp238[2]=Cyc_PP_text(({const char*
_tmp23A=",";_tag_arr(_tmp23A,sizeof(char),_get_zero_arr_size(_tmp23A,2));}));
_tmp238[1]=Cyc_Absynpp_typ2doc(_tmp1E0);_tmp238[0]=Cyc_PP_text(({const char*
_tmp239="offsetof(";_tag_arr(_tmp239,sizeof(char),_get_zero_arr_size(_tmp239,10));}));
Cyc_PP_cat(_tag_arr(_tmp238,sizeof(struct Cyc_PP_Doc*),5));});goto _LL18F;_LL1B8:
if(*((int*)_tmp1BF)!= 19)goto _LL1BA;_tmp1E3=((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp1BF)->f1;_tmp1E4=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1BF)->f2;
_LL1B9: s=({struct Cyc_PP_Doc*_tmp23F[4];_tmp23F[3]=Cyc_PP_text(({const char*
_tmp241=")";_tag_arr(_tmp241,sizeof(char),_get_zero_arr_size(_tmp241,2));}));
_tmp23F[2]=Cyc_Absynpp_typ2doc(_tmp1E4);_tmp23F[1]=Cyc_Absynpp_tvars2doc(_tmp1E3);
_tmp23F[0]=Cyc_PP_text(({const char*_tmp240="__gen(";_tag_arr(_tmp240,sizeof(char),
_get_zero_arr_size(_tmp240,7));}));Cyc_PP_cat(_tag_arr(_tmp23F,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL18F;_LL1BA: if(*((int*)_tmp1BF)!= 20)goto _LL1BC;_tmp1E5=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1BF)->f1;_LL1BB: s=({struct Cyc_PP_Doc*_tmp242[2];_tmp242[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E5);_tmp242[0]=Cyc_PP_text(({const char*_tmp243="*";_tag_arr(_tmp243,
sizeof(char),_get_zero_arr_size(_tmp243,2));}));Cyc_PP_cat(_tag_arr(_tmp242,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL18F;_LL1BC: if(*((int*)_tmp1BF)!= 21)goto
_LL1BE;_tmp1E6=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1BF)->f1;_tmp1E7=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp1BF)->f2;_LL1BD: s=({struct Cyc_PP_Doc*
_tmp244[3];_tmp244[2]=Cyc_PP_textptr(_tmp1E7);_tmp244[1]=Cyc_PP_text(({const char*
_tmp245=".";_tag_arr(_tmp245,sizeof(char),_get_zero_arr_size(_tmp245,2));}));
_tmp244[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E6);Cyc_PP_cat(_tag_arr(_tmp244,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL18F;_LL1BE: if(*((int*)_tmp1BF)!= 22)goto
_LL1C0;_tmp1E8=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1BF)->f1;_tmp1E9=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1BF)->f2;_LL1BF: s=({struct Cyc_PP_Doc*
_tmp246[3];_tmp246[2]=Cyc_PP_textptr(_tmp1E9);_tmp246[1]=Cyc_PP_text(({const char*
_tmp247="->";_tag_arr(_tmp247,sizeof(char),_get_zero_arr_size(_tmp247,3));}));
_tmp246[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E8);Cyc_PP_cat(_tag_arr(_tmp246,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL18F;_LL1C0: if(*((int*)_tmp1BF)!= 23)goto
_LL1C2;_tmp1EA=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1BF)->f1;_tmp1EB=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp1BF)->f2;_LL1C1: s=({struct Cyc_PP_Doc*
_tmp248[4];_tmp248[3]=Cyc_PP_text(({const char*_tmp24A="]";_tag_arr(_tmp24A,
sizeof(char),_get_zero_arr_size(_tmp24A,2));}));_tmp248[2]=Cyc_Absynpp_exp2doc(
_tmp1EB);_tmp248[1]=Cyc_PP_text(({const char*_tmp249="[";_tag_arr(_tmp249,sizeof(
char),_get_zero_arr_size(_tmp249,2));}));_tmp248[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EA);Cyc_PP_cat(_tag_arr(_tmp248,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL18F;_LL1C2: if(*((int*)_tmp1BF)!= 24)goto _LL1C4;_tmp1EC=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp1BF)->f1;_LL1C3: s=({struct Cyc_PP_Doc*_tmp24B[4];_tmp24B[3]=Cyc_PP_text(({
const char*_tmp24D=")";_tag_arr(_tmp24D,sizeof(char),_get_zero_arr_size(_tmp24D,2));}));
_tmp24B[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp1EC);_tmp24B[1]=Cyc_PP_text(({const
char*_tmp24C="(";_tag_arr(_tmp24C,sizeof(char),_get_zero_arr_size(_tmp24C,2));}));
_tmp24B[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_arr(_tmp24B,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL18F;_LL1C4: if(*((int*)_tmp1BF)!= 25)goto _LL1C6;_tmp1ED=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp1BF)->f1;_tmp1EE=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp1BF)->f2;_LL1C5:
s=({struct Cyc_PP_Doc*_tmp24E[4];_tmp24E[3]=Cyc_Absynpp_group_braces(({const char*
_tmp251=",";_tag_arr(_tmp251,sizeof(char),_get_zero_arr_size(_tmp251,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp1EE));_tmp24E[2]=Cyc_PP_text(({const char*
_tmp250=")";_tag_arr(_tmp250,sizeof(char),_get_zero_arr_size(_tmp250,2));}));
_tmp24E[1]=Cyc_Absynpp_typ2doc((*((struct _tuple1*)_tmp1ED)).f3);_tmp24E[0]=Cyc_PP_text(({
const char*_tmp24F="(";_tag_arr(_tmp24F,sizeof(char),_get_zero_arr_size(_tmp24F,2));}));
Cyc_PP_cat(_tag_arr(_tmp24E,sizeof(struct Cyc_PP_Doc*),4));});goto _LL18F;_LL1C6:
if(*((int*)_tmp1BF)!= 26)goto _LL1C8;_tmp1EF=((struct Cyc_Absyn_Array_e_struct*)
_tmp1BF)->f1;_LL1C7: s=Cyc_Absynpp_group_braces(({const char*_tmp252=",";_tag_arr(
_tmp252,sizeof(char),_get_zero_arr_size(_tmp252,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp1EF));goto _LL18F;_LL1C8: if(*((int*)_tmp1BF)!= 27)goto _LL1CA;_tmp1F0=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1BF)->f1;_tmp1F1=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1BF)->f2;_tmp1F2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1BF)->f3;
_LL1C9: s=({struct Cyc_PP_Doc*_tmp253[8];_tmp253[7]=Cyc_Absynpp_rb();_tmp253[6]=
Cyc_Absynpp_exp2doc(_tmp1F2);_tmp253[5]=Cyc_PP_text(({const char*_tmp256=" : ";
_tag_arr(_tmp256,sizeof(char),_get_zero_arr_size(_tmp256,4));}));_tmp253[4]=Cyc_Absynpp_exp2doc(
_tmp1F1);_tmp253[3]=Cyc_PP_text(({const char*_tmp255=" < ";_tag_arr(_tmp255,
sizeof(char),_get_zero_arr_size(_tmp255,4));}));_tmp253[2]=Cyc_PP_text(*((struct
_tagged_arr*)(*((struct _tuple0*)_tmp1F0->name)).f2));_tmp253[1]=Cyc_PP_text(({
const char*_tmp254="for ";_tag_arr(_tmp254,sizeof(char),_get_zero_arr_size(
_tmp254,5));}));_tmp253[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_arr(_tmp253,sizeof(
struct Cyc_PP_Doc*),8));});goto _LL18F;_LL1CA: if(*((int*)_tmp1BF)!= 28)goto _LL1CC;
_tmp1F3=((struct Cyc_Absyn_Struct_e_struct*)_tmp1BF)->f1;_tmp1F4=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1BF)->f2;_tmp1F5=((struct Cyc_Absyn_Struct_e_struct*)_tmp1BF)->f3;_LL1CB: {
struct Cyc_List_List*_tmp257=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp1F5);
s=({struct Cyc_PP_Doc*_tmp258[2];_tmp258[1]=Cyc_Absynpp_group_braces(({const char*
_tmp259=",";_tag_arr(_tmp259,sizeof(char),_get_zero_arr_size(_tmp259,2));}),
_tmp1F4 != 0?({struct Cyc_List_List*_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A->hd=
Cyc_Absynpp_tps2doc(_tmp1F4);_tmp25A->tl=_tmp257;_tmp25A;}): _tmp257);_tmp258[0]=
Cyc_Absynpp_qvar2doc(_tmp1F3);Cyc_PP_cat(_tag_arr(_tmp258,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL18F;}_LL1CC: if(*((int*)_tmp1BF)!= 29)goto _LL1CE;_tmp1F6=((struct
Cyc_Absyn_AnonStruct_e_struct*)_tmp1BF)->f2;_LL1CD: s=Cyc_Absynpp_group_braces(({
const char*_tmp25B=",";_tag_arr(_tmp25B,sizeof(char),_get_zero_arr_size(_tmp25B,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp1F6));goto _LL18F;_LL1CE: if(*((int*)
_tmp1BF)!= 30)goto _LL1D0;_tmp1F7=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1BF)->f1;
_tmp1F8=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1BF)->f3;_LL1CF: if(_tmp1F7 == 0)s=
Cyc_Absynpp_qvar2doc(_tmp1F8->name);else{s=({struct Cyc_PP_Doc*_tmp25C[2];_tmp25C[
1]=Cyc_PP_egroup(({const char*_tmp25D="(";_tag_arr(_tmp25D,sizeof(char),
_get_zero_arr_size(_tmp25D,2));}),({const char*_tmp25E=")";_tag_arr(_tmp25E,
sizeof(char),_get_zero_arr_size(_tmp25E,2));}),({const char*_tmp25F=",";_tag_arr(
_tmp25F,sizeof(char),_get_zero_arr_size(_tmp25F,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_exp2doc,_tmp1F7));_tmp25C[0]=Cyc_Absynpp_qvar2doc(_tmp1F8->name);Cyc_PP_cat(
_tag_arr(_tmp25C,sizeof(struct Cyc_PP_Doc*),2));});}goto _LL18F;_LL1D0: if(*((int*)
_tmp1BF)!= 31)goto _LL1D2;_tmp1F9=((struct Cyc_Absyn_Enum_e_struct*)_tmp1BF)->f1;
_LL1D1: s=Cyc_Absynpp_qvar2doc(_tmp1F9);goto _LL18F;_LL1D2: if(*((int*)_tmp1BF)!= 
32)goto _LL1D4;_tmp1FA=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp1BF)->f1;_LL1D3: s=
Cyc_Absynpp_qvar2doc(_tmp1FA);goto _LL18F;_LL1D4: if(*((int*)_tmp1BF)!= 33)goto
_LL1D6;_tmp1FB=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1BF)->f1;_tmp1FC=_tmp1FB.is_calloc;
_tmp1FD=_tmp1FB.rgn;_tmp1FE=_tmp1FB.elt_type;_tmp1FF=_tmp1FB.num_elts;_LL1D5: if(
_tmp1FC){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp1FE)),0);if(_tmp1FD == 0)s=({struct Cyc_PP_Doc*_tmp260[5];_tmp260[4]=Cyc_PP_text(({
const char*_tmp263=")";_tag_arr(_tmp263,sizeof(char),_get_zero_arr_size(_tmp263,2));}));
_tmp260[3]=Cyc_Absynpp_exp2doc(st);_tmp260[2]=Cyc_PP_text(({const char*_tmp262=",";
_tag_arr(_tmp262,sizeof(char),_get_zero_arr_size(_tmp262,2));}));_tmp260[1]=Cyc_Absynpp_exp2doc(
_tmp1FF);_tmp260[0]=Cyc_PP_text(({const char*_tmp261="calloc(";_tag_arr(_tmp261,
sizeof(char),_get_zero_arr_size(_tmp261,8));}));Cyc_PP_cat(_tag_arr(_tmp260,
sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp264[7];_tmp264[6]=
Cyc_PP_text(({const char*_tmp268=")";_tag_arr(_tmp268,sizeof(char),
_get_zero_arr_size(_tmp268,2));}));_tmp264[5]=Cyc_Absynpp_exp2doc(st);_tmp264[4]=
Cyc_PP_text(({const char*_tmp267=",";_tag_arr(_tmp267,sizeof(char),
_get_zero_arr_size(_tmp267,2));}));_tmp264[3]=Cyc_Absynpp_exp2doc(_tmp1FF);
_tmp264[2]=Cyc_PP_text(({const char*_tmp266=",";_tag_arr(_tmp266,sizeof(char),
_get_zero_arr_size(_tmp266,2));}));_tmp264[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmp1FD));_tmp264[0]=Cyc_PP_text(({const char*_tmp265="rcalloc(";
_tag_arr(_tmp265,sizeof(char),_get_zero_arr_size(_tmp265,9));}));Cyc_PP_cat(
_tag_arr(_tmp264,sizeof(struct Cyc_PP_Doc*),7));});}}else{struct Cyc_Absyn_Exp*
new_e;if(_tmp1FE == 0)new_e=_tmp1FF;else{new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*((
void**)_tmp1FE),0),_tmp1FF,0);}if(_tmp1FD == 0)s=({struct Cyc_PP_Doc*_tmp269[3];
_tmp269[2]=Cyc_PP_text(({const char*_tmp26B=")";_tag_arr(_tmp26B,sizeof(char),
_get_zero_arr_size(_tmp26B,2));}));_tmp269[1]=Cyc_Absynpp_exp2doc(new_e);_tmp269[
0]=Cyc_PP_text(({const char*_tmp26A="malloc(";_tag_arr(_tmp26A,sizeof(char),
_get_zero_arr_size(_tmp26A,8));}));Cyc_PP_cat(_tag_arr(_tmp269,sizeof(struct Cyc_PP_Doc*),
3));});else{s=({struct Cyc_PP_Doc*_tmp26C[5];_tmp26C[4]=Cyc_PP_text(({const char*
_tmp26F=")";_tag_arr(_tmp26F,sizeof(char),_get_zero_arr_size(_tmp26F,2));}));
_tmp26C[3]=Cyc_Absynpp_exp2doc(new_e);_tmp26C[2]=Cyc_PP_text(({const char*_tmp26E=",";
_tag_arr(_tmp26E,sizeof(char),_get_zero_arr_size(_tmp26E,2));}));_tmp26C[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(_tmp1FD));_tmp26C[0]=Cyc_PP_text(({const char*
_tmp26D="rmalloc(";_tag_arr(_tmp26D,sizeof(char),_get_zero_arr_size(_tmp26D,9));}));
Cyc_PP_cat(_tag_arr(_tmp26C,sizeof(struct Cyc_PP_Doc*),5));});}}goto _LL18F;_LL1D6:
if(*((int*)_tmp1BF)!= 34)goto _LL1D8;_tmp200=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp1BF)->f1;_tmp201=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1BF)->f2;
_LL1D7: s=Cyc_Absynpp_group_braces(({const char*_tmp270=",";_tag_arr(_tmp270,
sizeof(char),_get_zero_arr_size(_tmp270,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp201));goto _LL18F;_LL1D8: if(*((int*)_tmp1BF)!= 35)goto _LL1DA;_tmp202=((struct
Cyc_Absyn_StmtExp_e_struct*)_tmp1BF)->f1;_LL1D9: s=({struct Cyc_PP_Doc*_tmp271[7];
_tmp271[6]=Cyc_PP_text(({const char*_tmp273=")";_tag_arr(_tmp273,sizeof(char),
_get_zero_arr_size(_tmp273,2));}));_tmp271[5]=Cyc_Absynpp_rb();_tmp271[4]=Cyc_PP_blank_doc();
_tmp271[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp202));_tmp271[2]=Cyc_PP_blank_doc();
_tmp271[1]=Cyc_Absynpp_lb();_tmp271[0]=Cyc_PP_text(({const char*_tmp272="(";
_tag_arr(_tmp272,sizeof(char),_get_zero_arr_size(_tmp272,2));}));Cyc_PP_cat(
_tag_arr(_tmp271,sizeof(struct Cyc_PP_Doc*),7));});goto _LL18F;_LL1DA: if(*((int*)
_tmp1BF)!= 36)goto _LL1DC;_tmp203=((struct Cyc_Absyn_Codegen_e_struct*)_tmp1BF)->f1;
_LL1DB: s=({struct Cyc_PP_Doc*_tmp274[3];_tmp274[2]=Cyc_PP_text(({const char*
_tmp279=")";_tag_arr(_tmp279,sizeof(char),_get_zero_arr_size(_tmp279,2));}));
_tmp274[1]=Cyc_PP_nest(2,Cyc_Absynpp_decl2doc(({struct Cyc_Absyn_Decl*_tmp276=
_cycalloc(sizeof(*_tmp276));_tmp276->r=(void*)((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp278;_tmp278.tag=1;_tmp278.f1=_tmp203;_tmp278;});_tmp277;}));_tmp276->loc=e->loc;
_tmp276;})));_tmp274[0]=Cyc_PP_text(({const char*_tmp275="codegen(";_tag_arr(
_tmp275,sizeof(char),_get_zero_arr_size(_tmp275,9));}));Cyc_PP_cat(_tag_arr(
_tmp274,sizeof(struct Cyc_PP_Doc*),3));});goto _LL18F;_LL1DC: if(*((int*)_tmp1BF)!= 
37)goto _LL18F;_tmp204=((struct Cyc_Absyn_Fill_e_struct*)_tmp1BF)->f1;_LL1DD: s=({
struct Cyc_PP_Doc*_tmp27A[3];_tmp27A[2]=Cyc_PP_text(({const char*_tmp27C=")";
_tag_arr(_tmp27C,sizeof(char),_get_zero_arr_size(_tmp27C,2));}));_tmp27A[1]=Cyc_PP_nest(
2,Cyc_Absynpp_exp2doc(_tmp204));_tmp27A[0]=Cyc_PP_text(({const char*_tmp27B="fill(";
_tag_arr(_tmp27B,sizeof(char),_get_zero_arr_size(_tmp27B,6));}));Cyc_PP_cat(
_tag_arr(_tmp27A,sizeof(struct Cyc_PP_Doc*),3));});goto _LL18F;_LL18F:;}if(inprec
>= myprec)s=({struct Cyc_PP_Doc*_tmp27D[3];_tmp27D[2]=Cyc_PP_text(({const char*
_tmp27F=")";_tag_arr(_tmp27F,sizeof(char),_get_zero_arr_size(_tmp27F,2));}));
_tmp27D[1]=s;_tmp27D[0]=Cyc_PP_text(({const char*_tmp27E="(";_tag_arr(_tmp27E,
sizeof(char),_get_zero_arr_size(_tmp27E,2));}));Cyc_PP_cat(_tag_arr(_tmp27D,
sizeof(struct Cyc_PP_Doc*),3));});return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d){void*_tmp280=d;struct Cyc_Absyn_Exp*_tmp281;struct _tagged_arr*_tmp282;
_LL1EB: if(*((int*)_tmp280)!= 0)goto _LL1ED;_tmp281=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp280)->f1;_LL1EC: return({struct Cyc_PP_Doc*_tmp283[3];_tmp283[2]=Cyc_PP_text(({
const char*_tmp285="]";_tag_arr(_tmp285,sizeof(char),_get_zero_arr_size(_tmp285,2));}));
_tmp283[1]=Cyc_Absynpp_exp2doc(_tmp281);_tmp283[0]=Cyc_PP_text(({const char*
_tmp284=".[";_tag_arr(_tmp284,sizeof(char),_get_zero_arr_size(_tmp284,3));}));
Cyc_PP_cat(_tag_arr(_tmp283,sizeof(struct Cyc_PP_Doc*),3));});_LL1ED: if(*((int*)
_tmp280)!= 1)goto _LL1EA;_tmp282=((struct Cyc_Absyn_FieldName_struct*)_tmp280)->f1;
_LL1EE: return({struct Cyc_PP_Doc*_tmp286[2];_tmp286[1]=Cyc_PP_textptr(_tmp282);
_tmp286[0]=Cyc_PP_text(({const char*_tmp287=".";_tag_arr(_tmp287,sizeof(char),
_get_zero_arr_size(_tmp287,2));}));Cyc_PP_cat(_tag_arr(_tmp286,sizeof(struct Cyc_PP_Doc*),
2));});_LL1EA:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de){if((*((
struct _tuple9*)de)).f1 == 0)return Cyc_Absynpp_exp2doc((*((struct _tuple9*)de)).f2);
else{return({struct Cyc_PP_Doc*_tmp288[2];_tmp288[1]=Cyc_Absynpp_exp2doc((*((
struct _tuple9*)de)).f2);_tmp288[0]=Cyc_PP_egroup(({const char*_tmp289="";_tag_arr(
_tmp289,sizeof(char),_get_zero_arr_size(_tmp289,1));}),({const char*_tmp28A="=";
_tag_arr(_tmp28A,sizeof(char),_get_zero_arr_size(_tmp28A,2));}),({const char*
_tmp28B="=";_tag_arr(_tmp28B,sizeof(char),_get_zero_arr_size(_tmp28B,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*((struct _tuple9*)de)).f1));Cyc_PP_cat(_tag_arr(
_tmp288,sizeof(struct Cyc_PP_Doc*),2));});}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(
int inprec,struct Cyc_List_List*es){return Cyc_PP_group(({const char*_tmp28C="";
_tag_arr(_tmp28C,sizeof(char),_get_zero_arr_size(_tmp28C,1));}),({const char*
_tmp28D="";_tag_arr(_tmp28D,sizeof(char),_get_zero_arr_size(_tmp28D,1));}),({
const char*_tmp28E=",";_tag_arr(_tmp28E,sizeof(char),_get_zero_arr_size(_tmp28E,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*c){void*_tmp28F=c;void*_tmp290;char
_tmp291;void*_tmp292;short _tmp293;void*_tmp294;int _tmp295;void*_tmp296;int
_tmp297;void*_tmp298;int _tmp299;void*_tmp29A;long long _tmp29B;struct _tagged_arr
_tmp29C;struct _tagged_arr _tmp29D;_LL1F0: if(_tmp28F <= (void*)1?1:*((int*)_tmp28F)
!= 0)goto _LL1F2;_tmp290=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp28F)->f1;
_tmp291=((struct Cyc_Absyn_Char_c_struct*)_tmp28F)->f2;_LL1F1: return Cyc_PP_text((
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp2A0;_tmp2A0.tag=0;_tmp2A0.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp291));{void*
_tmp29E[1]={& _tmp2A0};Cyc_aprintf(({const char*_tmp29F="'%s'";_tag_arr(_tmp29F,
sizeof(char),_get_zero_arr_size(_tmp29F,5));}),_tag_arr(_tmp29E,sizeof(void*),1));}}));
_LL1F2: if(_tmp28F <= (void*)1?1:*((int*)_tmp28F)!= 1)goto _LL1F4;_tmp292=(void*)((
struct Cyc_Absyn_Short_c_struct*)_tmp28F)->f1;_tmp293=((struct Cyc_Absyn_Short_c_struct*)
_tmp28F)->f2;_LL1F3: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp2A3;_tmp2A3.tag=1;_tmp2A3.f1=(unsigned int)((int)_tmp293);{void*_tmp2A1[1]={&
_tmp2A3};Cyc_aprintf(({const char*_tmp2A2="%d";_tag_arr(_tmp2A2,sizeof(char),
_get_zero_arr_size(_tmp2A2,3));}),_tag_arr(_tmp2A1,sizeof(void*),1));}}));_LL1F4:
if(_tmp28F <= (void*)1?1:*((int*)_tmp28F)!= 2)goto _LL1F6;_tmp294=(void*)((struct
Cyc_Absyn_Int_c_struct*)_tmp28F)->f1;if((int)_tmp294 != 2)goto _LL1F6;_tmp295=((
struct Cyc_Absyn_Int_c_struct*)_tmp28F)->f2;_LL1F5: _tmp297=_tmp295;goto _LL1F7;
_LL1F6: if(_tmp28F <= (void*)1?1:*((int*)_tmp28F)!= 2)goto _LL1F8;_tmp296=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp28F)->f1;if((int)_tmp296 != 0)goto _LL1F8;
_tmp297=((struct Cyc_Absyn_Int_c_struct*)_tmp28F)->f2;_LL1F7: return Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp2A6;_tmp2A6.tag=1;_tmp2A6.f1=(
unsigned int)_tmp297;{void*_tmp2A4[1]={& _tmp2A6};Cyc_aprintf(({const char*_tmp2A5="%d";
_tag_arr(_tmp2A5,sizeof(char),_get_zero_arr_size(_tmp2A5,3));}),_tag_arr(_tmp2A4,
sizeof(void*),1));}}));_LL1F8: if(_tmp28F <= (void*)1?1:*((int*)_tmp28F)!= 2)goto
_LL1FA;_tmp298=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp28F)->f1;if((int)
_tmp298 != 1)goto _LL1FA;_tmp299=((struct Cyc_Absyn_Int_c_struct*)_tmp28F)->f2;
_LL1F9: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp2A9;
_tmp2A9.tag=1;_tmp2A9.f1=(unsigned int)_tmp299;{void*_tmp2A7[1]={& _tmp2A9};Cyc_aprintf(({
const char*_tmp2A8="%u";_tag_arr(_tmp2A8,sizeof(char),_get_zero_arr_size(_tmp2A8,
3));}),_tag_arr(_tmp2A7,sizeof(void*),1));}}));_LL1FA: if(_tmp28F <= (void*)1?1:*((
int*)_tmp28F)!= 3)goto _LL1FC;_tmp29A=(void*)((struct Cyc_Absyn_LongLong_c_struct*)
_tmp28F)->f1;_tmp29B=((struct Cyc_Absyn_LongLong_c_struct*)_tmp28F)->f2;_LL1FB:
return Cyc_PP_text(({const char*_tmp2AA="<<FIX LONG LONG CONSTANT>>";_tag_arr(
_tmp2AA,sizeof(char),_get_zero_arr_size(_tmp2AA,27));}));_LL1FC: if(_tmp28F <= (
void*)1?1:*((int*)_tmp28F)!= 4)goto _LL1FE;_tmp29C=((struct Cyc_Absyn_Float_c_struct*)
_tmp28F)->f1;_LL1FD: return Cyc_PP_text(_tmp29C);_LL1FE: if((int)_tmp28F != 0)goto
_LL200;_LL1FF: return Cyc_PP_text(({const char*_tmp2AB="NULL";_tag_arr(_tmp2AB,
sizeof(char),_get_zero_arr_size(_tmp2AB,5));}));_LL200: if(_tmp28F <= (void*)1?1:*((
int*)_tmp28F)!= 5)goto _LL1EF;_tmp29D=((struct Cyc_Absyn_String_c_struct*)_tmp28F)->f1;
_LL201: return({struct Cyc_PP_Doc*_tmp2AC[3];_tmp2AC[2]=Cyc_PP_text(({const char*
_tmp2AE="\"";_tag_arr(_tmp2AE,sizeof(char),_get_zero_arr_size(_tmp2AE,2));}));
_tmp2AC[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp29D));_tmp2AC[0]=Cyc_PP_text(({
const char*_tmp2AD="\"";_tag_arr(_tmp2AD,sizeof(char),_get_zero_arr_size(_tmp2AD,
2));}));Cyc_PP_cat(_tag_arr(_tmp2AC,sizeof(struct Cyc_PP_Doc*),3));});_LL1EF:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (void*)19){if(es == 0?1: es->tl
!= 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2AF=_cycalloc(
sizeof(*_tmp2AF));_tmp2AF[0]=({struct Cyc_Core_Failure_struct _tmp2B0;_tmp2B0.tag=
Cyc_Core_Failure;_tmp2B0.f1=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp2B3;_tmp2B3.tag=0;_tmp2B3.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_PP_string_of_doc(
ps,72));{void*_tmp2B1[1]={& _tmp2B3};Cyc_aprintf(({const char*_tmp2B2="Absynpp::primapp2doc Size: %s with bad args";
_tag_arr(_tmp2B2,sizeof(char),_get_zero_arr_size(_tmp2B2,44));}),_tag_arr(
_tmp2B1,sizeof(void*),1));}});_tmp2B0;});_tmp2AF;}));{struct Cyc_PP_Doc*_tmp2B4=
Cyc_Absynpp_exp2doc_prec(inprec,(struct Cyc_Absyn_Exp*)es->hd);return({struct Cyc_PP_Doc*
_tmp2B5[2];_tmp2B5[1]=Cyc_PP_text(({const char*_tmp2B6=".size";_tag_arr(_tmp2B6,
sizeof(char),_get_zero_arr_size(_tmp2B6,6));}));_tmp2B5[0]=_tmp2B4;Cyc_PP_cat(
_tag_arr(_tmp2B5,sizeof(struct Cyc_PP_Doc*),2));});}}else{struct Cyc_List_List*ds=((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);if(ds
== 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2B7=_cycalloc(
sizeof(*_tmp2B7));_tmp2B7[0]=({struct Cyc_Core_Failure_struct _tmp2B8;_tmp2B8.tag=
Cyc_Core_Failure;_tmp2B8.f1=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp2BB;_tmp2BB.tag=0;_tmp2BB.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_PP_string_of_doc(
ps,72));{void*_tmp2B9[1]={& _tmp2BB};Cyc_aprintf(({const char*_tmp2BA="Absynpp::primapp2doc: %s with no args";
_tag_arr(_tmp2BA,sizeof(char),_get_zero_arr_size(_tmp2BA,38));}),_tag_arr(
_tmp2B9,sizeof(void*),1));}});_tmp2B8;});_tmp2B7;}));else{if(ds->tl == 0)return({
struct Cyc_PP_Doc*_tmp2BC[3];_tmp2BC[2]=(struct Cyc_PP_Doc*)ds->hd;_tmp2BC[1]=Cyc_PP_text(({
const char*_tmp2BD=" ";_tag_arr(_tmp2BD,sizeof(char),_get_zero_arr_size(_tmp2BD,2));}));
_tmp2BC[0]=ps;Cyc_PP_cat(_tag_arr(_tmp2BC,sizeof(struct Cyc_PP_Doc*),3));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE[0]=({
struct Cyc_Core_Failure_struct _tmp2BF;_tmp2BF.tag=Cyc_Core_Failure;_tmp2BF.f1=(
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp2C2;_tmp2C2.tag=0;_tmp2C2.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_PP_string_of_doc(ps,72));{void*_tmp2C0[
1]={& _tmp2C2};Cyc_aprintf(({const char*_tmp2C1="Absynpp::primapp2doc: %s with more than 2 args";
_tag_arr(_tmp2C1,sizeof(char),_get_zero_arr_size(_tmp2C1,47));}),_tag_arr(
_tmp2C0,sizeof(void*),1));}});_tmp2BF;});_tmp2BE;}));else{return({struct Cyc_PP_Doc*
_tmp2C3[5];_tmp2C3[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;
_tmp2C3[3]=Cyc_PP_text(({const char*_tmp2C5=" ";_tag_arr(_tmp2C5,sizeof(char),
_get_zero_arr_size(_tmp2C5,2));}));_tmp2C3[2]=ps;_tmp2C3[1]=Cyc_PP_text(({const
char*_tmp2C4=" ";_tag_arr(_tmp2C4,sizeof(char),_get_zero_arr_size(_tmp2C4,2));}));
_tmp2C3[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_arr(_tmp2C3,sizeof(struct
Cyc_PP_Doc*),5));});}}}}}struct _tagged_arr Cyc_Absynpp_prim2str(void*p){void*
_tmp2C6=p;_LL203: if((int)_tmp2C6 != 0)goto _LL205;_LL204: return({const char*_tmp2C7="+";
_tag_arr(_tmp2C7,sizeof(char),_get_zero_arr_size(_tmp2C7,2));});_LL205: if((int)
_tmp2C6 != 1)goto _LL207;_LL206: return({const char*_tmp2C8="*";_tag_arr(_tmp2C8,
sizeof(char),_get_zero_arr_size(_tmp2C8,2));});_LL207: if((int)_tmp2C6 != 2)goto
_LL209;_LL208: return({const char*_tmp2C9="-";_tag_arr(_tmp2C9,sizeof(char),
_get_zero_arr_size(_tmp2C9,2));});_LL209: if((int)_tmp2C6 != 3)goto _LL20B;_LL20A:
return({const char*_tmp2CA="/";_tag_arr(_tmp2CA,sizeof(char),_get_zero_arr_size(
_tmp2CA,2));});_LL20B: if((int)_tmp2C6 != 4)goto _LL20D;_LL20C: return({const char*
_tmp2CB="%";_tag_arr(_tmp2CB,sizeof(char),_get_zero_arr_size(_tmp2CB,2));});
_LL20D: if((int)_tmp2C6 != 5)goto _LL20F;_LL20E: return({const char*_tmp2CC="==";
_tag_arr(_tmp2CC,sizeof(char),_get_zero_arr_size(_tmp2CC,3));});_LL20F: if((int)
_tmp2C6 != 6)goto _LL211;_LL210: return({const char*_tmp2CD="!=";_tag_arr(_tmp2CD,
sizeof(char),_get_zero_arr_size(_tmp2CD,3));});_LL211: if((int)_tmp2C6 != 7)goto
_LL213;_LL212: return({const char*_tmp2CE=">";_tag_arr(_tmp2CE,sizeof(char),
_get_zero_arr_size(_tmp2CE,2));});_LL213: if((int)_tmp2C6 != 8)goto _LL215;_LL214:
return({const char*_tmp2CF="<";_tag_arr(_tmp2CF,sizeof(char),_get_zero_arr_size(
_tmp2CF,2));});_LL215: if((int)_tmp2C6 != 9)goto _LL217;_LL216: return({const char*
_tmp2D0=">=";_tag_arr(_tmp2D0,sizeof(char),_get_zero_arr_size(_tmp2D0,3));});
_LL217: if((int)_tmp2C6 != 10)goto _LL219;_LL218: return({const char*_tmp2D1="<=";
_tag_arr(_tmp2D1,sizeof(char),_get_zero_arr_size(_tmp2D1,3));});_LL219: if((int)
_tmp2C6 != 11)goto _LL21B;_LL21A: return({const char*_tmp2D2="!";_tag_arr(_tmp2D2,
sizeof(char),_get_zero_arr_size(_tmp2D2,2));});_LL21B: if((int)_tmp2C6 != 12)goto
_LL21D;_LL21C: return({const char*_tmp2D3="~";_tag_arr(_tmp2D3,sizeof(char),
_get_zero_arr_size(_tmp2D3,2));});_LL21D: if((int)_tmp2C6 != 13)goto _LL21F;_LL21E:
return({const char*_tmp2D4="&";_tag_arr(_tmp2D4,sizeof(char),_get_zero_arr_size(
_tmp2D4,2));});_LL21F: if((int)_tmp2C6 != 14)goto _LL221;_LL220: return({const char*
_tmp2D5="|";_tag_arr(_tmp2D5,sizeof(char),_get_zero_arr_size(_tmp2D5,2));});
_LL221: if((int)_tmp2C6 != 15)goto _LL223;_LL222: return({const char*_tmp2D6="^";
_tag_arr(_tmp2D6,sizeof(char),_get_zero_arr_size(_tmp2D6,2));});_LL223: if((int)
_tmp2C6 != 16)goto _LL225;_LL224: return({const char*_tmp2D7="<<";_tag_arr(_tmp2D7,
sizeof(char),_get_zero_arr_size(_tmp2D7,3));});_LL225: if((int)_tmp2C6 != 17)goto
_LL227;_LL226: return({const char*_tmp2D8=">>";_tag_arr(_tmp2D8,sizeof(char),
_get_zero_arr_size(_tmp2D8,3));});_LL227: if((int)_tmp2C6 != 18)goto _LL229;_LL228:
return({const char*_tmp2D9=">>>";_tag_arr(_tmp2D9,sizeof(char),_get_zero_arr_size(
_tmp2D9,4));});_LL229: if((int)_tmp2C6 != 19)goto _LL202;_LL22A: return({const char*
_tmp2DA="size";_tag_arr(_tmp2DA,sizeof(char),_get_zero_arr_size(_tmp2DA,5));});
_LL202:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(
p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){void*_tmp2DB=(void*)s->r;
_LL22C: if(_tmp2DB <= (void*)1?1:*((int*)_tmp2DB)!= 12)goto _LL22E;_LL22D: return 1;
_LL22E:;_LL22F: return 0;_LL22B:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*
st){struct Cyc_PP_Doc*s;{void*_tmp2DC=(void*)st->r;struct Cyc_Absyn_Exp*_tmp2DD;
struct Cyc_Absyn_Stmt*_tmp2DE;struct Cyc_Absyn_Stmt*_tmp2DF;struct Cyc_Absyn_Exp*
_tmp2E0;struct Cyc_Absyn_Exp*_tmp2E1;struct Cyc_Absyn_Stmt*_tmp2E2;struct Cyc_Absyn_Stmt*
_tmp2E3;struct _tuple2 _tmp2E4;struct Cyc_Absyn_Exp*_tmp2E5;struct Cyc_Absyn_Stmt*
_tmp2E6;struct _tagged_arr*_tmp2E7;struct Cyc_Absyn_Exp*_tmp2E8;struct _tuple2
_tmp2E9;struct Cyc_Absyn_Exp*_tmp2EA;struct _tuple2 _tmp2EB;struct Cyc_Absyn_Exp*
_tmp2EC;struct Cyc_Absyn_Stmt*_tmp2ED;struct Cyc_Absyn_ForArrayInfo _tmp2EE;struct
Cyc_List_List*_tmp2EF;struct _tuple2 _tmp2F0;struct Cyc_Absyn_Exp*_tmp2F1;struct
_tuple2 _tmp2F2;struct Cyc_Absyn_Exp*_tmp2F3;struct Cyc_Absyn_Stmt*_tmp2F4;struct
Cyc_Absyn_Exp*_tmp2F5;struct Cyc_List_List*_tmp2F6;struct Cyc_Absyn_Exp*_tmp2F7;
struct Cyc_List_List*_tmp2F8;struct Cyc_List_List*_tmp2F9;struct Cyc_List_List*
_tmp2FA;struct Cyc_Absyn_Decl*_tmp2FB;struct Cyc_Absyn_Stmt*_tmp2FC;struct Cyc_Absyn_Stmt*
_tmp2FD;struct Cyc_Absyn_Stmt*_tmp2FE;struct _tagged_arr*_tmp2FF;struct Cyc_Absyn_Stmt*
_tmp300;struct Cyc_Absyn_Stmt*_tmp301;struct _tuple2 _tmp302;struct Cyc_Absyn_Exp*
_tmp303;struct Cyc_Absyn_Stmt*_tmp304;struct Cyc_List_List*_tmp305;struct Cyc_Absyn_Tvar*
_tmp306;struct Cyc_Absyn_Vardecl*_tmp307;int _tmp308;struct Cyc_Absyn_Stmt*_tmp309;
struct Cyc_Absyn_Exp*_tmp30A;_LL231: if((int)_tmp2DC != 0)goto _LL233;_LL232: s=Cyc_PP_text(({
const char*_tmp30B=";";_tag_arr(_tmp30B,sizeof(char),_get_zero_arr_size(_tmp30B,2));}));
goto _LL230;_LL233: if(_tmp2DC <= (void*)1?1:*((int*)_tmp2DC)!= 0)goto _LL235;
_tmp2DD=((struct Cyc_Absyn_Exp_s_struct*)_tmp2DC)->f1;_LL234: s=({struct Cyc_PP_Doc*
_tmp30C[2];_tmp30C[1]=Cyc_PP_text(({const char*_tmp30D=";";_tag_arr(_tmp30D,
sizeof(char),_get_zero_arr_size(_tmp30D,2));}));_tmp30C[0]=Cyc_Absynpp_exp2doc(
_tmp2DD);Cyc_PP_cat(_tag_arr(_tmp30C,sizeof(struct Cyc_PP_Doc*),2));});goto _LL230;
_LL235: if(_tmp2DC <= (void*)1?1:*((int*)_tmp2DC)!= 1)goto _LL237;_tmp2DE=((struct
Cyc_Absyn_Seq_s_struct*)_tmp2DC)->f1;_tmp2DF=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2DC)->f2;_LL236: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(
_tmp2DE))s=({struct Cyc_PP_Doc*_tmp30E[7];_tmp30E[6]=Cyc_Absynpp_is_declaration(
_tmp2DF)?({struct Cyc_PP_Doc*_tmp30F[5];_tmp30F[4]=Cyc_PP_line_doc();_tmp30F[3]=
Cyc_Absynpp_rb();_tmp30F[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2DF));_tmp30F[
1]=Cyc_PP_blank_doc();_tmp30F[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_arr(_tmp30F,
sizeof(struct Cyc_PP_Doc*),5));}): Cyc_Absynpp_stmt2doc(_tmp2DF);_tmp30E[5]=Cyc_PP_line_doc();
_tmp30E[4]=Cyc_Absynpp_rb();_tmp30E[3]=Cyc_PP_line_doc();_tmp30E[2]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp2DE));_tmp30E[1]=Cyc_PP_blank_doc();_tmp30E[0]=Cyc_Absynpp_lb();
Cyc_PP_cat(_tag_arr(_tmp30E,sizeof(struct Cyc_PP_Doc*),7));});else{if(Cyc_Absynpp_is_declaration(
_tmp2DF))s=({struct Cyc_PP_Doc*_tmp310[7];_tmp310[6]=Cyc_PP_line_doc();_tmp310[5]=
Cyc_Absynpp_rb();_tmp310[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2DF));_tmp310[
3]=Cyc_PP_blank_doc();_tmp310[2]=Cyc_Absynpp_lb();_tmp310[1]=Cyc_PP_line_doc();
_tmp310[0]=Cyc_Absynpp_stmt2doc(_tmp2DE);Cyc_PP_cat(_tag_arr(_tmp310,sizeof(
struct Cyc_PP_Doc*),7));});else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Stmt*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,({const char*_tmp311="";_tag_arr(_tmp311,sizeof(char),
_get_zero_arr_size(_tmp311,1));}),({struct Cyc_List_List*_tmp312=_cycalloc(
sizeof(*_tmp312));_tmp312->hd=_tmp2DE;_tmp312->tl=({struct Cyc_List_List*_tmp313=
_cycalloc(sizeof(*_tmp313));_tmp313->hd=_tmp2DF;_tmp313->tl=0;_tmp313;});_tmp312;}));}}}
else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,({
const char*_tmp314="";_tag_arr(_tmp314,sizeof(char),_get_zero_arr_size(_tmp314,1));}),({
struct Cyc_List_List*_tmp315=_cycalloc(sizeof(*_tmp315));_tmp315->hd=_tmp2DE;
_tmp315->tl=({struct Cyc_List_List*_tmp316=_cycalloc(sizeof(*_tmp316));_tmp316->hd=
_tmp2DF;_tmp316->tl=0;_tmp316;});_tmp315;}));}goto _LL230;_LL237: if(_tmp2DC <= (
void*)1?1:*((int*)_tmp2DC)!= 2)goto _LL239;_tmp2E0=((struct Cyc_Absyn_Return_s_struct*)
_tmp2DC)->f1;_LL238: if(_tmp2E0 == 0)s=Cyc_PP_text(({const char*_tmp317="return;";
_tag_arr(_tmp317,sizeof(char),_get_zero_arr_size(_tmp317,8));}));else{s=({struct
Cyc_PP_Doc*_tmp318[3];_tmp318[2]=Cyc_PP_text(({const char*_tmp31A=";";_tag_arr(
_tmp31A,sizeof(char),_get_zero_arr_size(_tmp31A,2));}));_tmp318[1]=_tmp2E0 == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(_tmp2E0));
_tmp318[0]=Cyc_PP_text(({const char*_tmp319="return ";_tag_arr(_tmp319,sizeof(
char),_get_zero_arr_size(_tmp319,8));}));Cyc_PP_cat(_tag_arr(_tmp318,sizeof(
struct Cyc_PP_Doc*),3));});}goto _LL230;_LL239: if(_tmp2DC <= (void*)1?1:*((int*)
_tmp2DC)!= 3)goto _LL23B;_tmp2E1=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2DC)->f1;
_tmp2E2=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2DC)->f2;_tmp2E3=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2DC)->f3;_LL23A: {int print_else;{void*_tmp31B=(void*)_tmp2E3->r;_LL260: if((
int)_tmp31B != 0)goto _LL262;_LL261: print_else=0;goto _LL25F;_LL262:;_LL263:
print_else=1;goto _LL25F;_LL25F:;}s=({struct Cyc_PP_Doc*_tmp31C[8];_tmp31C[7]=
print_else?({struct Cyc_PP_Doc*_tmp320[6];_tmp320[5]=Cyc_Absynpp_rb();_tmp320[4]=
Cyc_PP_line_doc();_tmp320[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp322[2];_tmp322[
1]=Cyc_Absynpp_stmt2doc(_tmp2E3);_tmp322[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp322,sizeof(struct Cyc_PP_Doc*),2));}));_tmp320[2]=Cyc_Absynpp_lb();
_tmp320[1]=Cyc_PP_text(({const char*_tmp321="else ";_tag_arr(_tmp321,sizeof(char),
_get_zero_arr_size(_tmp321,6));}));_tmp320[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp320,sizeof(struct Cyc_PP_Doc*),6));}): Cyc_PP_nil_doc();_tmp31C[6]=
Cyc_Absynpp_rb();_tmp31C[5]=Cyc_PP_line_doc();_tmp31C[4]=Cyc_PP_nest(2,({struct
Cyc_PP_Doc*_tmp31F[2];_tmp31F[1]=Cyc_Absynpp_stmt2doc(_tmp2E2);_tmp31F[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp31F,sizeof(struct Cyc_PP_Doc*),2));}));_tmp31C[3]=Cyc_Absynpp_lb();
_tmp31C[2]=Cyc_PP_text(({const char*_tmp31E=") ";_tag_arr(_tmp31E,sizeof(char),
_get_zero_arr_size(_tmp31E,3));}));_tmp31C[1]=Cyc_Absynpp_exp2doc(_tmp2E1);
_tmp31C[0]=Cyc_PP_text(({const char*_tmp31D="if (";_tag_arr(_tmp31D,sizeof(char),
_get_zero_arr_size(_tmp31D,5));}));Cyc_PP_cat(_tag_arr(_tmp31C,sizeof(struct Cyc_PP_Doc*),
8));});goto _LL230;}_LL23B: if(_tmp2DC <= (void*)1?1:*((int*)_tmp2DC)!= 4)goto
_LL23D;_tmp2E4=((struct Cyc_Absyn_While_s_struct*)_tmp2DC)->f1;_tmp2E5=_tmp2E4.f1;
_tmp2E6=((struct Cyc_Absyn_While_s_struct*)_tmp2DC)->f2;_LL23C: s=({struct Cyc_PP_Doc*
_tmp323[7];_tmp323[6]=Cyc_Absynpp_rb();_tmp323[5]=Cyc_PP_line_doc();_tmp323[4]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp326[2];_tmp326[1]=Cyc_Absynpp_stmt2doc(
_tmp2E6);_tmp326[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp326,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp323[3]=Cyc_Absynpp_lb();_tmp323[2]=Cyc_PP_text(({const
char*_tmp325=") ";_tag_arr(_tmp325,sizeof(char),_get_zero_arr_size(_tmp325,3));}));
_tmp323[1]=Cyc_Absynpp_exp2doc(_tmp2E5);_tmp323[0]=Cyc_PP_text(({const char*
_tmp324="while (";_tag_arr(_tmp324,sizeof(char),_get_zero_arr_size(_tmp324,8));}));
Cyc_PP_cat(_tag_arr(_tmp323,sizeof(struct Cyc_PP_Doc*),7));});goto _LL230;_LL23D:
if(_tmp2DC <= (void*)1?1:*((int*)_tmp2DC)!= 5)goto _LL23F;_LL23E: s=Cyc_PP_text(({
const char*_tmp327="break;";_tag_arr(_tmp327,sizeof(char),_get_zero_arr_size(
_tmp327,7));}));goto _LL230;_LL23F: if(_tmp2DC <= (void*)1?1:*((int*)_tmp2DC)!= 6)
goto _LL241;_LL240: s=Cyc_PP_text(({const char*_tmp328="continue;";_tag_arr(_tmp328,
sizeof(char),_get_zero_arr_size(_tmp328,10));}));goto _LL230;_LL241: if(_tmp2DC <= (
void*)1?1:*((int*)_tmp2DC)!= 7)goto _LL243;_tmp2E7=((struct Cyc_Absyn_Goto_s_struct*)
_tmp2DC)->f1;_LL242: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp32B;_tmp32B.tag=0;_tmp32B.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct _tagged_arr*)_tmp2E7));{void*_tmp329[1]={& _tmp32B};Cyc_aprintf(({const char*
_tmp32A="goto %s;";_tag_arr(_tmp32A,sizeof(char),_get_zero_arr_size(_tmp32A,9));}),
_tag_arr(_tmp329,sizeof(void*),1));}}));goto _LL230;_LL243: if(_tmp2DC <= (void*)1?
1:*((int*)_tmp2DC)!= 8)goto _LL245;_tmp2E8=((struct Cyc_Absyn_For_s_struct*)
_tmp2DC)->f1;_tmp2E9=((struct Cyc_Absyn_For_s_struct*)_tmp2DC)->f2;_tmp2EA=
_tmp2E9.f1;_tmp2EB=((struct Cyc_Absyn_For_s_struct*)_tmp2DC)->f3;_tmp2EC=_tmp2EB.f1;
_tmp2ED=((struct Cyc_Absyn_For_s_struct*)_tmp2DC)->f4;_LL244: s=({struct Cyc_PP_Doc*
_tmp32C[11];_tmp32C[10]=Cyc_Absynpp_rb();_tmp32C[9]=Cyc_PP_line_doc();_tmp32C[8]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp331[2];_tmp331[1]=Cyc_Absynpp_stmt2doc(
_tmp2ED);_tmp331[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp331,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp32C[7]=Cyc_Absynpp_lb();_tmp32C[6]=Cyc_PP_text(({const
char*_tmp330=") ";_tag_arr(_tmp330,sizeof(char),_get_zero_arr_size(_tmp330,3));}));
_tmp32C[5]=Cyc_Absynpp_exp2doc(_tmp2EC);_tmp32C[4]=Cyc_PP_text(({const char*
_tmp32F="; ";_tag_arr(_tmp32F,sizeof(char),_get_zero_arr_size(_tmp32F,3));}));
_tmp32C[3]=Cyc_Absynpp_exp2doc(_tmp2EA);_tmp32C[2]=Cyc_PP_text(({const char*
_tmp32E="; ";_tag_arr(_tmp32E,sizeof(char),_get_zero_arr_size(_tmp32E,3));}));
_tmp32C[1]=Cyc_Absynpp_exp2doc(_tmp2E8);_tmp32C[0]=Cyc_PP_text(({const char*
_tmp32D="for(";_tag_arr(_tmp32D,sizeof(char),_get_zero_arr_size(_tmp32D,5));}));
Cyc_PP_cat(_tag_arr(_tmp32C,sizeof(struct Cyc_PP_Doc*),11));});goto _LL230;_LL245:
if(_tmp2DC <= (void*)1?1:*((int*)_tmp2DC)!= 19)goto _LL247;_tmp2EE=((struct Cyc_Absyn_ForArray_s_struct*)
_tmp2DC)->f1;_tmp2EF=_tmp2EE.defns;_tmp2F0=_tmp2EE.condition;_tmp2F1=_tmp2F0.f1;
_tmp2F2=_tmp2EE.delta;_tmp2F3=_tmp2F2.f1;_tmp2F4=_tmp2EE.body;_LL246: s=({struct
Cyc_PP_Doc*_tmp332[10];_tmp332[9]=Cyc_Absynpp_rb();_tmp332[8]=Cyc_PP_line_doc();
_tmp332[7]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp337[2];_tmp337[1]=Cyc_Absynpp_stmt2doc(
_tmp2F4);_tmp337[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp337,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp332[6]=Cyc_Absynpp_lb();_tmp332[5]=Cyc_PP_text(({const
char*_tmp336=") ";_tag_arr(_tmp336,sizeof(char),_get_zero_arr_size(_tmp336,3));}));
_tmp332[4]=Cyc_Absynpp_exp2doc(_tmp2F3);_tmp332[3]=Cyc_PP_text(({const char*
_tmp335="; ";_tag_arr(_tmp335,sizeof(char),_get_zero_arr_size(_tmp335,3));}));
_tmp332[2]=Cyc_Absynpp_exp2doc(_tmp2F1);_tmp332[1]=((struct Cyc_PP_Doc*(*)(struct
Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_vardecl2doc,({const char*_tmp334=" ";_tag_arr(
_tmp334,sizeof(char),_get_zero_arr_size(_tmp334,2));}),_tmp2EF);_tmp332[0]=Cyc_PP_text(({
const char*_tmp333="forarray(";_tag_arr(_tmp333,sizeof(char),_get_zero_arr_size(
_tmp333,10));}));Cyc_PP_cat(_tag_arr(_tmp332,sizeof(struct Cyc_PP_Doc*),10));});
goto _LL230;_LL247: if(_tmp2DC <= (void*)1?1:*((int*)_tmp2DC)!= 9)goto _LL249;
_tmp2F5=((struct Cyc_Absyn_Switch_s_struct*)_tmp2DC)->f1;_tmp2F6=((struct Cyc_Absyn_Switch_s_struct*)
_tmp2DC)->f2;_LL248: s=({struct Cyc_PP_Doc*_tmp338[8];_tmp338[7]=Cyc_Absynpp_rb();
_tmp338[6]=Cyc_PP_line_doc();_tmp338[5]=Cyc_Absynpp_switchclauses2doc(_tmp2F6);
_tmp338[4]=Cyc_PP_line_doc();_tmp338[3]=Cyc_Absynpp_lb();_tmp338[2]=Cyc_PP_text(({
const char*_tmp33A=") ";_tag_arr(_tmp33A,sizeof(char),_get_zero_arr_size(_tmp33A,
3));}));_tmp338[1]=Cyc_Absynpp_exp2doc(_tmp2F5);_tmp338[0]=Cyc_PP_text(({const
char*_tmp339="switch (";_tag_arr(_tmp339,sizeof(char),_get_zero_arr_size(_tmp339,
9));}));Cyc_PP_cat(_tag_arr(_tmp338,sizeof(struct Cyc_PP_Doc*),8));});goto _LL230;
_LL249: if(_tmp2DC <= (void*)1?1:*((int*)_tmp2DC)!= 10)goto _LL24B;_tmp2F7=((struct
Cyc_Absyn_SwitchC_s_struct*)_tmp2DC)->f1;_tmp2F8=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmp2DC)->f2;_LL24A: s=({struct Cyc_PP_Doc*_tmp33B[8];_tmp33B[7]=Cyc_Absynpp_rb();
_tmp33B[6]=Cyc_PP_line_doc();_tmp33B[5]=Cyc_Absynpp_switchCclauses2doc(_tmp2F8);
_tmp33B[4]=Cyc_PP_line_doc();_tmp33B[3]=Cyc_Absynpp_lb();_tmp33B[2]=Cyc_PP_text(({
const char*_tmp33D=") ";_tag_arr(_tmp33D,sizeof(char),_get_zero_arr_size(_tmp33D,
3));}));_tmp33B[1]=Cyc_Absynpp_exp2doc(_tmp2F7);_tmp33B[0]=Cyc_PP_text(({const
char*_tmp33C="switch \"C\" (";_tag_arr(_tmp33C,sizeof(char),_get_zero_arr_size(
_tmp33C,13));}));Cyc_PP_cat(_tag_arr(_tmp33B,sizeof(struct Cyc_PP_Doc*),8));});
goto _LL230;_LL24B: if(_tmp2DC <= (void*)1?1:*((int*)_tmp2DC)!= 11)goto _LL24D;
_tmp2F9=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp2DC)->f1;if(_tmp2F9 != 0)goto
_LL24D;_LL24C: s=Cyc_PP_text(({const char*_tmp33E="fallthru;";_tag_arr(_tmp33E,
sizeof(char),_get_zero_arr_size(_tmp33E,10));}));goto _LL230;_LL24D: if(_tmp2DC <= (
void*)1?1:*((int*)_tmp2DC)!= 11)goto _LL24F;_tmp2FA=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp2DC)->f1;_LL24E: s=({struct Cyc_PP_Doc*_tmp33F[3];_tmp33F[2]=Cyc_PP_text(({
const char*_tmp341=");";_tag_arr(_tmp341,sizeof(char),_get_zero_arr_size(_tmp341,
3));}));_tmp33F[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp2FA);_tmp33F[0]=Cyc_PP_text(({
const char*_tmp340="fallthru(";_tag_arr(_tmp340,sizeof(char),_get_zero_arr_size(
_tmp340,10));}));Cyc_PP_cat(_tag_arr(_tmp33F,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL230;_LL24F: if(_tmp2DC <= (void*)1?1:*((int*)_tmp2DC)!= 12)goto _LL251;
_tmp2FB=((struct Cyc_Absyn_Decl_s_struct*)_tmp2DC)->f1;_tmp2FC=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2DC)->f2;_LL250: s=({struct Cyc_PP_Doc*_tmp342[3];_tmp342[2]=Cyc_Absynpp_stmt2doc(
_tmp2FC);_tmp342[1]=Cyc_PP_line_doc();_tmp342[0]=Cyc_Absynpp_decl2doc(_tmp2FB);
Cyc_PP_cat(_tag_arr(_tmp342,sizeof(struct Cyc_PP_Doc*),3));});goto _LL230;_LL251:
if(_tmp2DC <= (void*)1?1:*((int*)_tmp2DC)!= 13)goto _LL253;_tmp2FD=((struct Cyc_Absyn_Cut_s_struct*)
_tmp2DC)->f1;_LL252: s=({struct Cyc_PP_Doc*_tmp343[2];_tmp343[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2FD));_tmp343[0]=Cyc_PP_text(({const char*_tmp344="cut ";_tag_arr(_tmp344,
sizeof(char),_get_zero_arr_size(_tmp344,5));}));Cyc_PP_cat(_tag_arr(_tmp343,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL230;_LL253: if(_tmp2DC <= (void*)1?1:*((
int*)_tmp2DC)!= 14)goto _LL255;_tmp2FE=((struct Cyc_Absyn_Splice_s_struct*)_tmp2DC)->f1;
_LL254: s=({struct Cyc_PP_Doc*_tmp345[2];_tmp345[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2FE));_tmp345[0]=Cyc_PP_text(({const char*_tmp346="splice ";_tag_arr(_tmp346,
sizeof(char),_get_zero_arr_size(_tmp346,8));}));Cyc_PP_cat(_tag_arr(_tmp345,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL230;_LL255: if(_tmp2DC <= (void*)1?1:*((
int*)_tmp2DC)!= 15)goto _LL257;_tmp2FF=((struct Cyc_Absyn_Label_s_struct*)_tmp2DC)->f1;
_tmp300=((struct Cyc_Absyn_Label_s_struct*)_tmp2DC)->f2;_LL256: if(Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration(_tmp300): 0)s=({struct Cyc_PP_Doc*_tmp347[7];_tmp347[6]=
Cyc_Absynpp_rb();_tmp347[5]=Cyc_PP_line_doc();_tmp347[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp300));_tmp347[3]=Cyc_PP_line_doc();_tmp347[2]=Cyc_Absynpp_lb();_tmp347[1]=
Cyc_PP_text(({const char*_tmp348=": ";_tag_arr(_tmp348,sizeof(char),
_get_zero_arr_size(_tmp348,3));}));_tmp347[0]=Cyc_PP_textptr(_tmp2FF);Cyc_PP_cat(
_tag_arr(_tmp347,sizeof(struct Cyc_PP_Doc*),7));});else{s=({struct Cyc_PP_Doc*
_tmp349[3];_tmp349[2]=Cyc_Absynpp_stmt2doc(_tmp300);_tmp349[1]=Cyc_PP_text(({
const char*_tmp34A=": ";_tag_arr(_tmp34A,sizeof(char),_get_zero_arr_size(_tmp34A,
3));}));_tmp349[0]=Cyc_PP_textptr(_tmp2FF);Cyc_PP_cat(_tag_arr(_tmp349,sizeof(
struct Cyc_PP_Doc*),3));});}goto _LL230;_LL257: if(_tmp2DC <= (void*)1?1:*((int*)
_tmp2DC)!= 16)goto _LL259;_tmp301=((struct Cyc_Absyn_Do_s_struct*)_tmp2DC)->f1;
_tmp302=((struct Cyc_Absyn_Do_s_struct*)_tmp2DC)->f2;_tmp303=_tmp302.f1;_LL258: s=({
struct Cyc_PP_Doc*_tmp34B[9];_tmp34B[8]=Cyc_PP_text(({const char*_tmp34E=");";
_tag_arr(_tmp34E,sizeof(char),_get_zero_arr_size(_tmp34E,3));}));_tmp34B[7]=Cyc_Absynpp_exp2doc(
_tmp303);_tmp34B[6]=Cyc_PP_text(({const char*_tmp34D=" while (";_tag_arr(_tmp34D,
sizeof(char),_get_zero_arr_size(_tmp34D,9));}));_tmp34B[5]=Cyc_Absynpp_rb();
_tmp34B[4]=Cyc_PP_line_doc();_tmp34B[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp301));_tmp34B[2]=Cyc_PP_line_doc();_tmp34B[1]=Cyc_Absynpp_lb();_tmp34B[0]=
Cyc_PP_text(({const char*_tmp34C="do ";_tag_arr(_tmp34C,sizeof(char),
_get_zero_arr_size(_tmp34C,4));}));Cyc_PP_cat(_tag_arr(_tmp34B,sizeof(struct Cyc_PP_Doc*),
9));});goto _LL230;_LL259: if(_tmp2DC <= (void*)1?1:*((int*)_tmp2DC)!= 17)goto
_LL25B;_tmp304=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp2DC)->f1;_tmp305=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp2DC)->f2;_LL25A: s=({struct Cyc_PP_Doc*
_tmp34F[12];_tmp34F[11]=Cyc_Absynpp_rb();_tmp34F[10]=Cyc_PP_line_doc();_tmp34F[9]=
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp305));_tmp34F[8]=Cyc_PP_line_doc();
_tmp34F[7]=Cyc_Absynpp_lb();_tmp34F[6]=Cyc_PP_text(({const char*_tmp351=" catch ";
_tag_arr(_tmp351,sizeof(char),_get_zero_arr_size(_tmp351,8));}));_tmp34F[5]=Cyc_Absynpp_rb();
_tmp34F[4]=Cyc_PP_line_doc();_tmp34F[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp304));_tmp34F[2]=Cyc_PP_line_doc();_tmp34F[1]=Cyc_Absynpp_lb();_tmp34F[0]=
Cyc_PP_text(({const char*_tmp350="try ";_tag_arr(_tmp350,sizeof(char),
_get_zero_arr_size(_tmp350,5));}));Cyc_PP_cat(_tag_arr(_tmp34F,sizeof(struct Cyc_PP_Doc*),
12));});goto _LL230;_LL25B: if(_tmp2DC <= (void*)1?1:*((int*)_tmp2DC)!= 18)goto
_LL25D;_tmp306=((struct Cyc_Absyn_Region_s_struct*)_tmp2DC)->f1;_tmp307=((struct
Cyc_Absyn_Region_s_struct*)_tmp2DC)->f2;_tmp308=((struct Cyc_Absyn_Region_s_struct*)
_tmp2DC)->f3;_tmp309=((struct Cyc_Absyn_Region_s_struct*)_tmp2DC)->f4;_LL25C: s=({
struct Cyc_PP_Doc*_tmp352[11];_tmp352[10]=Cyc_Absynpp_rb();_tmp352[9]=Cyc_PP_line_doc();
_tmp352[8]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp309));_tmp352[7]=Cyc_PP_line_doc();
_tmp352[6]=Cyc_Absynpp_lb();_tmp352[5]=Cyc_Absynpp_qvar2doc(_tmp307->name);
_tmp352[4]=Cyc_PP_text(({const char*_tmp357=">";_tag_arr(_tmp357,sizeof(char),
_get_zero_arr_size(_tmp357,2));}));_tmp352[3]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp306));_tmp352[2]=Cyc_PP_text(({const char*_tmp356="<";_tag_arr(_tmp356,
sizeof(char),_get_zero_arr_size(_tmp356,2));}));_tmp352[1]=_tmp308?Cyc_PP_text(({
const char*_tmp354="";_tag_arr(_tmp354,sizeof(char),_get_zero_arr_size(_tmp354,1));})):
Cyc_PP_text(({const char*_tmp355="[resetable]";_tag_arr(_tmp355,sizeof(char),
_get_zero_arr_size(_tmp355,12));}));_tmp352[0]=Cyc_PP_text(({const char*_tmp353="region";
_tag_arr(_tmp353,sizeof(char),_get_zero_arr_size(_tmp353,7));}));Cyc_PP_cat(
_tag_arr(_tmp352,sizeof(struct Cyc_PP_Doc*),11));});goto _LL230;_LL25D: if(_tmp2DC
<= (void*)1?1:*((int*)_tmp2DC)!= 20)goto _LL230;_tmp30A=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp2DC)->f1;_LL25E: s=({struct Cyc_PP_Doc*_tmp358[3];_tmp358[2]=Cyc_PP_text(({
const char*_tmp35A=");";_tag_arr(_tmp35A,sizeof(char),_get_zero_arr_size(_tmp35A,
3));}));_tmp358[1]=Cyc_Absynpp_exp2doc(_tmp30A);_tmp358[0]=Cyc_PP_text(({const
char*_tmp359="reset_region(";_tag_arr(_tmp359,sizeof(char),_get_zero_arr_size(
_tmp359,14));}));Cyc_PP_cat(_tag_arr(_tmp358,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL230;_LL230:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp35B=(void*)p->r;void*_tmp35C;int _tmp35D;char
_tmp35E;struct _tagged_arr _tmp35F;struct Cyc_Absyn_Vardecl*_tmp360;struct Cyc_List_List*
_tmp361;struct Cyc_Absyn_Pat*_tmp362;struct Cyc_Absyn_Vardecl*_tmp363;struct
_tuple0*_tmp364;struct _tuple0*_tmp365;struct Cyc_List_List*_tmp366;struct Cyc_Absyn_AggrInfo
_tmp367;void*_tmp368;struct Cyc_List_List*_tmp369;struct Cyc_List_List*_tmp36A;
struct Cyc_Absyn_Enumfield*_tmp36B;struct Cyc_Absyn_Enumfield*_tmp36C;struct Cyc_Absyn_Tunionfield*
_tmp36D;struct Cyc_List_List*_tmp36E;struct Cyc_Absyn_Tunionfield*_tmp36F;struct
Cyc_List_List*_tmp370;_LL265: if((int)_tmp35B != 0)goto _LL267;_LL266: s=Cyc_PP_text(({
const char*_tmp371="_";_tag_arr(_tmp371,sizeof(char),_get_zero_arr_size(_tmp371,2));}));
goto _LL264;_LL267: if((int)_tmp35B != 1)goto _LL269;_LL268: s=Cyc_PP_text(({const
char*_tmp372="NULL";_tag_arr(_tmp372,sizeof(char),_get_zero_arr_size(_tmp372,5));}));
goto _LL264;_LL269: if(_tmp35B <= (void*)2?1:*((int*)_tmp35B)!= 1)goto _LL26B;
_tmp35C=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp35B)->f1;_tmp35D=((struct Cyc_Absyn_Int_p_struct*)
_tmp35B)->f2;_LL26A: if(_tmp35C != (void*)1)s=Cyc_PP_text((struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp375;_tmp375.tag=1;_tmp375.f1=(unsigned int)_tmp35D;{
void*_tmp373[1]={& _tmp375};Cyc_aprintf(({const char*_tmp374="%d";_tag_arr(_tmp374,
sizeof(char),_get_zero_arr_size(_tmp374,3));}),_tag_arr(_tmp373,sizeof(void*),1));}}));
else{s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp378;_tmp378.tag=
1;_tmp378.f1=(unsigned int)_tmp35D;{void*_tmp376[1]={& _tmp378};Cyc_aprintf(({
const char*_tmp377="%u";_tag_arr(_tmp377,sizeof(char),_get_zero_arr_size(_tmp377,
3));}),_tag_arr(_tmp376,sizeof(void*),1));}}));}goto _LL264;_LL26B: if(_tmp35B <= (
void*)2?1:*((int*)_tmp35B)!= 2)goto _LL26D;_tmp35E=((struct Cyc_Absyn_Char_p_struct*)
_tmp35B)->f1;_LL26C: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp37B;_tmp37B.tag=0;_tmp37B.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_char_escape(
_tmp35E));{void*_tmp379[1]={& _tmp37B};Cyc_aprintf(({const char*_tmp37A="'%s'";
_tag_arr(_tmp37A,sizeof(char),_get_zero_arr_size(_tmp37A,5));}),_tag_arr(_tmp379,
sizeof(void*),1));}}));goto _LL264;_LL26D: if(_tmp35B <= (void*)2?1:*((int*)_tmp35B)
!= 3)goto _LL26F;_tmp35F=((struct Cyc_Absyn_Float_p_struct*)_tmp35B)->f1;_LL26E: s=
Cyc_PP_text(_tmp35F);goto _LL264;_LL26F: if(_tmp35B <= (void*)2?1:*((int*)_tmp35B)
!= 0)goto _LL271;_tmp360=((struct Cyc_Absyn_Var_p_struct*)_tmp35B)->f1;_LL270: s=
Cyc_Absynpp_qvar2doc(_tmp360->name);goto _LL264;_LL271: if(_tmp35B <= (void*)2?1:*((
int*)_tmp35B)!= 4)goto _LL273;_tmp361=((struct Cyc_Absyn_Tuple_p_struct*)_tmp35B)->f1;
_LL272: s=({struct Cyc_PP_Doc*_tmp37C[4];_tmp37C[3]=Cyc_PP_text(({const char*
_tmp37F=")";_tag_arr(_tmp37F,sizeof(char),_get_zero_arr_size(_tmp37F,2));}));
_tmp37C[2]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,({
const char*_tmp37E=",";_tag_arr(_tmp37E,sizeof(char),_get_zero_arr_size(_tmp37E,2));}),
_tmp361);_tmp37C[1]=Cyc_PP_text(({const char*_tmp37D="(";_tag_arr(_tmp37D,sizeof(
char),_get_zero_arr_size(_tmp37D,2));}));_tmp37C[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(
_tag_arr(_tmp37C,sizeof(struct Cyc_PP_Doc*),4));});goto _LL264;_LL273: if(_tmp35B <= (
void*)2?1:*((int*)_tmp35B)!= 5)goto _LL275;_tmp362=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp35B)->f1;_LL274: s=({struct Cyc_PP_Doc*_tmp380[2];_tmp380[1]=Cyc_Absynpp_pat2doc(
_tmp362);_tmp380[0]=Cyc_PP_text(({const char*_tmp381="&";_tag_arr(_tmp381,sizeof(
char),_get_zero_arr_size(_tmp381,2));}));Cyc_PP_cat(_tag_arr(_tmp380,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL264;_LL275: if(_tmp35B <= (void*)2?1:*((int*)
_tmp35B)!= 6)goto _LL277;_tmp363=((struct Cyc_Absyn_Reference_p_struct*)_tmp35B)->f1;
_LL276: s=({struct Cyc_PP_Doc*_tmp382[2];_tmp382[1]=Cyc_Absynpp_qvar2doc(_tmp363->name);
_tmp382[0]=Cyc_PP_text(({const char*_tmp383="*";_tag_arr(_tmp383,sizeof(char),
_get_zero_arr_size(_tmp383,2));}));Cyc_PP_cat(_tag_arr(_tmp382,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL264;_LL277: if(_tmp35B <= (void*)2?1:*((int*)_tmp35B)!= 11)goto
_LL279;_tmp364=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp35B)->f1;_LL278: s=Cyc_Absynpp_qvar2doc(
_tmp364);goto _LL264;_LL279: if(_tmp35B <= (void*)2?1:*((int*)_tmp35B)!= 12)goto
_LL27B;_tmp365=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp35B)->f1;_tmp366=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp35B)->f2;_LL27A: s=({struct Cyc_PP_Doc*
_tmp384[2];_tmp384[1]=Cyc_PP_group(({const char*_tmp385="(";_tag_arr(_tmp385,
sizeof(char),_get_zero_arr_size(_tmp385,2));}),({const char*_tmp386=")";_tag_arr(
_tmp386,sizeof(char),_get_zero_arr_size(_tmp386,2));}),({const char*_tmp387=",";
_tag_arr(_tmp387,sizeof(char),_get_zero_arr_size(_tmp387,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp366));_tmp384[0]=Cyc_Absynpp_qvar2doc(_tmp365);Cyc_PP_cat(
_tag_arr(_tmp384,sizeof(struct Cyc_PP_Doc*),2));});goto _LL264;_LL27B: if(_tmp35B <= (
void*)2?1:*((int*)_tmp35B)!= 7)goto _LL27D;_tmp367=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp35B)->f1;_tmp368=(void*)_tmp367.aggr_info;_tmp369=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp35B)->f2;_tmp36A=((struct Cyc_Absyn_Aggr_p_struct*)_tmp35B)->f3;_LL27C: {
struct _tuple0*_tmp389;struct _tuple3 _tmp388=Cyc_Absyn_aggr_kinded_name(_tmp368);
_tmp389=_tmp388.f2;s=({struct Cyc_PP_Doc*_tmp38A[4];_tmp38A[3]=Cyc_Absynpp_group_braces(({
const char*_tmp38E=",";_tag_arr(_tmp38E,sizeof(char),_get_zero_arr_size(_tmp38E,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp36A));_tmp38A[2]=Cyc_PP_egroup(({const
char*_tmp38B="[";_tag_arr(_tmp38B,sizeof(char),_get_zero_arr_size(_tmp38B,2));}),({
const char*_tmp38C="]";_tag_arr(_tmp38C,sizeof(char),_get_zero_arr_size(_tmp38C,2));}),({
const char*_tmp38D=",";_tag_arr(_tmp38D,sizeof(char),_get_zero_arr_size(_tmp38D,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
_tmp369)));_tmp38A[1]=Cyc_Absynpp_lb();_tmp38A[0]=Cyc_Absynpp_qvar2doc(_tmp389);
Cyc_PP_cat(_tag_arr(_tmp38A,sizeof(struct Cyc_PP_Doc*),4));});goto _LL264;}_LL27D:
if(_tmp35B <= (void*)2?1:*((int*)_tmp35B)!= 9)goto _LL27F;_tmp36B=((struct Cyc_Absyn_Enum_p_struct*)
_tmp35B)->f2;_LL27E: _tmp36C=_tmp36B;goto _LL280;_LL27F: if(_tmp35B <= (void*)2?1:*((
int*)_tmp35B)!= 10)goto _LL281;_tmp36C=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp35B)->f2;_LL280: s=Cyc_Absynpp_qvar2doc(_tmp36C->name);goto _LL264;_LL281: if(
_tmp35B <= (void*)2?1:*((int*)_tmp35B)!= 8)goto _LL283;_tmp36D=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp35B)->f2;_tmp36E=((struct Cyc_Absyn_Tunion_p_struct*)_tmp35B)->f3;if(_tmp36E
!= 0)goto _LL283;_LL282: s=Cyc_Absynpp_qvar2doc(_tmp36D->name);goto _LL264;_LL283:
if(_tmp35B <= (void*)2?1:*((int*)_tmp35B)!= 8)goto _LL264;_tmp36F=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp35B)->f2;_tmp370=((struct Cyc_Absyn_Tunion_p_struct*)_tmp35B)->f3;_LL284: s=({
struct Cyc_PP_Doc*_tmp38F[2];_tmp38F[1]=Cyc_PP_egroup(({const char*_tmp390="(";
_tag_arr(_tmp390,sizeof(char),_get_zero_arr_size(_tmp390,2));}),({const char*
_tmp391=")";_tag_arr(_tmp391,sizeof(char),_get_zero_arr_size(_tmp391,2));}),({
const char*_tmp392=",";_tag_arr(_tmp392,sizeof(char),_get_zero_arr_size(_tmp392,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp370));_tmp38F[0]=Cyc_Absynpp_qvar2doc(
_tmp36F->name);Cyc_PP_cat(_tag_arr(_tmp38F,sizeof(struct Cyc_PP_Doc*),2));});goto
_LL264;_LL264:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp){
return({struct Cyc_PP_Doc*_tmp393[2];_tmp393[1]=Cyc_Absynpp_pat2doc((*((struct
_tuple8*)dp)).f2);_tmp393[0]=Cyc_PP_egroup(({const char*_tmp394="";_tag_arr(
_tmp394,sizeof(char),_get_zero_arr_size(_tmp394,1));}),({const char*_tmp395="=";
_tag_arr(_tmp395,sizeof(char),_get_zero_arr_size(_tmp395,2));}),({const char*
_tmp396="=";_tag_arr(_tmp396,sizeof(char),_get_zero_arr_size(_tmp396,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*((struct _tuple8*)dp)).f1));Cyc_PP_cat(_tag_arr(
_tmp393,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0?(void*)(c->pattern)->r == (
void*)0: 0)return({struct Cyc_PP_Doc*_tmp397[2];_tmp397[1]=Cyc_PP_nest(2,({struct
Cyc_PP_Doc*_tmp399[2];_tmp399[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp399[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp399,sizeof(struct Cyc_PP_Doc*),2));}));_tmp397[0]=Cyc_PP_text(({
const char*_tmp398="default: ";_tag_arr(_tmp398,sizeof(char),_get_zero_arr_size(
_tmp398,10));}));Cyc_PP_cat(_tag_arr(_tmp397,sizeof(struct Cyc_PP_Doc*),2));});
else{if(c->where_clause == 0)return({struct Cyc_PP_Doc*_tmp39A[4];_tmp39A[3]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp39D[2];_tmp39D[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp39D[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp39D,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp39A[2]=Cyc_PP_text(({const char*_tmp39C=": ";_tag_arr(_tmp39C,sizeof(char),
_get_zero_arr_size(_tmp39C,3));}));_tmp39A[1]=Cyc_Absynpp_pat2doc(c->pattern);
_tmp39A[0]=Cyc_PP_text(({const char*_tmp39B="case ";_tag_arr(_tmp39B,sizeof(char),
_get_zero_arr_size(_tmp39B,6));}));Cyc_PP_cat(_tag_arr(_tmp39A,sizeof(struct Cyc_PP_Doc*),
4));});else{return({struct Cyc_PP_Doc*_tmp39E[6];_tmp39E[5]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp3A2[2];_tmp3A2[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3A2[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3A2,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp39E[4]=Cyc_PP_text(({const char*_tmp3A1=": ";_tag_arr(_tmp3A1,sizeof(char),
_get_zero_arr_size(_tmp3A1,3));}));_tmp39E[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(c->where_clause));_tmp39E[2]=Cyc_PP_text(({const char*_tmp3A0=" && ";
_tag_arr(_tmp3A0,sizeof(char),_get_zero_arr_size(_tmp3A0,5));}));_tmp39E[1]=Cyc_Absynpp_pat2doc(
c->pattern);_tmp39E[0]=Cyc_PP_text(({const char*_tmp39F="case ";_tag_arr(_tmp39F,
sizeof(char),_get_zero_arr_size(_tmp39F,6));}));Cyc_PP_cat(_tag_arr(_tmp39E,
sizeof(struct Cyc_PP_Doc*),6));});}}}struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Switch_clause*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_switchclause2doc,({const char*_tmp3A3="";_tag_arr(_tmp3A3,sizeof(char),
_get_zero_arr_size(_tmp3A3,1));}),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause*c){struct Cyc_Absyn_SwitchC_clause _tmp3A5;struct
Cyc_Absyn_Exp*_tmp3A6;struct Cyc_Absyn_Stmt*_tmp3A7;struct Cyc_Absyn_SwitchC_clause*
_tmp3A4=c;_tmp3A5=*_tmp3A4;_tmp3A6=_tmp3A5.cnst_exp;_tmp3A7=_tmp3A5.body;if(
_tmp3A6 == 0)return({struct Cyc_PP_Doc*_tmp3A8[2];_tmp3A8[1]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp3AA[2];_tmp3AA[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3AA[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3AA,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp3A8[0]=Cyc_PP_text(({const char*_tmp3A9="default: ";_tag_arr(_tmp3A9,sizeof(
char),_get_zero_arr_size(_tmp3A9,10));}));Cyc_PP_cat(_tag_arr(_tmp3A8,sizeof(
struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*_tmp3AB[4];_tmp3AB[3]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3AE[2];_tmp3AE[1]=Cyc_Absynpp_stmt2doc(
_tmp3A7);_tmp3AE[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3AE,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp3AB[2]=Cyc_PP_text(({const char*_tmp3AD=": ";_tag_arr(
_tmp3AD,sizeof(char),_get_zero_arr_size(_tmp3AD,3));}));_tmp3AB[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(_tmp3A6));_tmp3AB[0]=Cyc_PP_text(({const char*
_tmp3AC="case ";_tag_arr(_tmp3AC,sizeof(char),_get_zero_arr_size(_tmp3AC,6));}));
Cyc_PP_cat(_tag_arr(_tmp3AB,sizeof(struct Cyc_PP_Doc*),4));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_switchCclauses2doc(struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_SwitchC_clause*),struct _tagged_arr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchCclause2doc,({const char*
_tmp3AF="";_tag_arr(_tmp3AF,sizeof(char),_get_zero_arr_size(_tmp3AF,1));}),cs);}
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){if(f->tag
== 0)return Cyc_Absynpp_qvar2doc(f->name);else{return({struct Cyc_PP_Doc*_tmp3B0[3];
_tmp3B0[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));
_tmp3B0[1]=Cyc_PP_text(({const char*_tmp3B1=" = ";_tag_arr(_tmp3B1,sizeof(char),
_get_zero_arr_size(_tmp3B1,4));}));_tmp3B0[0]=Cyc_Absynpp_qvar2doc(f->name);Cyc_PP_cat(
_tag_arr(_tmp3B0,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Enumfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_enumfield2doc,({const char*_tmp3B2=",";_tag_arr(_tmp3B2,sizeof(char),
_get_zero_arr_size(_tmp3B2,2));}),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(
struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*
Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){return((struct Cyc_PP_Doc*(*)(struct
Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,({const char*_tmp3B3=",";_tag_arr(_tmp3B3,
sizeof(char),_get_zero_arr_size(_tmp3B3,2));}),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(
struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl _tmp3B5;void*_tmp3B6;struct
_tuple0*_tmp3B7;struct Cyc_Absyn_Tqual _tmp3B8;void*_tmp3B9;struct Cyc_Absyn_Exp*
_tmp3BA;struct Cyc_List_List*_tmp3BB;struct Cyc_Absyn_Vardecl*_tmp3B4=vd;_tmp3B5=*
_tmp3B4;_tmp3B6=(void*)_tmp3B5.sc;_tmp3B7=_tmp3B5.name;_tmp3B8=_tmp3B5.tq;
_tmp3B9=(void*)_tmp3B5.type;_tmp3BA=_tmp3B5.initializer;_tmp3BB=_tmp3B5.attributes;{
struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp3B7);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp3BB);struct Cyc_PP_Doc*
beforenamedoc;if(!Cyc_Absynpp_to_VC)beforenamedoc=attsdoc;else{void*_tmp3BC=Cyc_Tcutil_compress(
_tmp3B9);struct Cyc_Absyn_FnInfo _tmp3BD;struct Cyc_List_List*_tmp3BE;_LL286: if(
_tmp3BC <= (void*)3?1:*((int*)_tmp3BC)!= 8)goto _LL288;_tmp3BD=((struct Cyc_Absyn_FnType_struct*)
_tmp3BC)->f1;_tmp3BE=_tmp3BD.attributes;_LL287: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp3BE);goto _LL285;_LL288:;_LL289: beforenamedoc=Cyc_PP_nil_doc();goto _LL285;
_LL285:;}s=({struct Cyc_PP_Doc*_tmp3BF[5];_tmp3BF[4]=Cyc_PP_text(({const char*
_tmp3C5=";";_tag_arr(_tmp3C5,sizeof(char),_get_zero_arr_size(_tmp3C5,2));}));
_tmp3BF[3]=_tmp3BA == 0?Cyc_PP_text(({const char*_tmp3C2="";_tag_arr(_tmp3C2,
sizeof(char),_get_zero_arr_size(_tmp3C2,1));})):({struct Cyc_PP_Doc*_tmp3C3[2];
_tmp3C3[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(_tmp3BA));
_tmp3C3[0]=Cyc_PP_text(({const char*_tmp3C4=" = ";_tag_arr(_tmp3C4,sizeof(char),
_get_zero_arr_size(_tmp3C4,4));}));Cyc_PP_cat(_tag_arr(_tmp3C3,sizeof(struct Cyc_PP_Doc*),
2));});_tmp3BF[2]=Cyc_Absynpp_tqtd2doc(_tmp3B8,_tmp3B9,({struct Cyc_Core_Opt*
_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0->v=({struct Cyc_PP_Doc*_tmp3C1[2];
_tmp3C1[1]=sn;_tmp3C1[0]=beforenamedoc;Cyc_PP_cat(_tag_arr(_tmp3C1,sizeof(struct
Cyc_PP_Doc*),2));});_tmp3C0;}));_tmp3BF[1]=Cyc_Absynpp_scope2doc(_tmp3B6);
_tmp3BF[0]=Cyc_Absynpp_to_VC?attsdoc: Cyc_PP_nil_doc();Cyc_PP_cat(_tag_arr(
_tmp3BF,sizeof(struct Cyc_PP_Doc*),5));});return s;}}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp3C6=(void*)d->r;struct Cyc_Absyn_Fndecl*
_tmp3C7;struct Cyc_Absyn_Aggrdecl*_tmp3C8;struct Cyc_Absyn_Vardecl*_tmp3C9;struct
Cyc_Absyn_Tuniondecl*_tmp3CA;struct Cyc_Absyn_Tuniondecl _tmp3CB;void*_tmp3CC;
struct _tuple0*_tmp3CD;struct Cyc_List_List*_tmp3CE;struct Cyc_Core_Opt*_tmp3CF;int
_tmp3D0;struct Cyc_Absyn_Pat*_tmp3D1;struct Cyc_Absyn_Exp*_tmp3D2;struct Cyc_List_List*
_tmp3D3;struct Cyc_Absyn_Enumdecl*_tmp3D4;struct Cyc_Absyn_Enumdecl _tmp3D5;void*
_tmp3D6;struct _tuple0*_tmp3D7;struct Cyc_Core_Opt*_tmp3D8;struct Cyc_Absyn_Typedefdecl*
_tmp3D9;struct _tagged_arr*_tmp3DA;struct Cyc_List_List*_tmp3DB;struct _tuple0*
_tmp3DC;struct Cyc_List_List*_tmp3DD;struct Cyc_List_List*_tmp3DE;_LL28B: if(*((int*)
_tmp3C6)!= 1)goto _LL28D;_tmp3C7=((struct Cyc_Absyn_Fn_d_struct*)_tmp3C6)->f1;
_LL28C: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp3E8=_cycalloc(sizeof(*
_tmp3E8));_tmp3E8[0]=({struct Cyc_Absyn_FnType_struct _tmp3E9;_tmp3E9.tag=8;
_tmp3E9.f1=({struct Cyc_Absyn_FnInfo _tmp3EA;_tmp3EA.tvars=_tmp3C7->tvs;_tmp3EA.effect=
_tmp3C7->effect;_tmp3EA.ret_typ=(void*)((void*)_tmp3C7->ret_type);_tmp3EA.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3C7->args);_tmp3EA.c_varargs=_tmp3C7->c_varargs;
_tmp3EA.cyc_varargs=_tmp3C7->cyc_varargs;_tmp3EA.rgn_po=_tmp3C7->rgn_po;_tmp3EA.attributes=
0;_tmp3EA;});_tmp3E9;});_tmp3E8;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp3C7->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp3C7->is_inline){if(Cyc_Absynpp_to_VC)
inlinedoc=Cyc_PP_text(({const char*_tmp3DF="__inline ";_tag_arr(_tmp3DF,sizeof(
char),_get_zero_arr_size(_tmp3DF,10));}));else{inlinedoc=Cyc_PP_text(({const char*
_tmp3E0="inline ";_tag_arr(_tmp3E0,sizeof(char),_get_zero_arr_size(_tmp3E0,8));}));}}
else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((
void*)_tmp3C7->sc);struct Cyc_PP_Doc*beforenamedoc=Cyc_Absynpp_to_VC?Cyc_Absynpp_callconv2doc(
_tmp3C7->attributes): attsdoc;struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp3C7->name);struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(({struct Cyc_Absyn_Tqual
_tmp3E5;_tmp3E5.q_const=0;_tmp3E5.q_volatile=0;_tmp3E5.q_restrict=0;_tmp3E5;}),t,({
struct Cyc_Core_Opt*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));_tmp3E6->v=({struct Cyc_PP_Doc*
_tmp3E7[2];_tmp3E7[1]=namedoc;_tmp3E7[0]=beforenamedoc;Cyc_PP_cat(_tag_arr(
_tmp3E7,sizeof(struct Cyc_PP_Doc*),2));});_tmp3E6;}));struct Cyc_PP_Doc*bodydoc=({
struct Cyc_PP_Doc*_tmp3E3[5];_tmp3E3[4]=Cyc_Absynpp_rb();_tmp3E3[3]=Cyc_PP_line_doc();
_tmp3E3[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3E4[2];_tmp3E4[1]=Cyc_Absynpp_stmt2doc(
_tmp3C7->body);_tmp3E4[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3E4,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp3E3[1]=Cyc_Absynpp_lb();_tmp3E3[0]=Cyc_PP_blank_doc();
Cyc_PP_cat(_tag_arr(_tmp3E3,sizeof(struct Cyc_PP_Doc*),5));});s=({struct Cyc_PP_Doc*
_tmp3E1[4];_tmp3E1[3]=bodydoc;_tmp3E1[2]=tqtddoc;_tmp3E1[1]=scopedoc;_tmp3E1[0]=
inlinedoc;Cyc_PP_cat(_tag_arr(_tmp3E1,sizeof(struct Cyc_PP_Doc*),4));});if(Cyc_Absynpp_to_VC)
s=({struct Cyc_PP_Doc*_tmp3E2[2];_tmp3E2[1]=s;_tmp3E2[0]=attsdoc;Cyc_PP_cat(
_tag_arr(_tmp3E2,sizeof(struct Cyc_PP_Doc*),2));});goto _LL28A;}}_LL28D: if(*((int*)
_tmp3C6)!= 4)goto _LL28F;_tmp3C8=((struct Cyc_Absyn_Aggr_d_struct*)_tmp3C6)->f1;
_LL28E: if(_tmp3C8->impl == 0)s=({struct Cyc_PP_Doc*_tmp3EB[5];_tmp3EB[4]=Cyc_PP_text(({
const char*_tmp3EC=";";_tag_arr(_tmp3EC,sizeof(char),_get_zero_arr_size(_tmp3EC,2));}));
_tmp3EB[3]=Cyc_Absynpp_ktvars2doc(_tmp3C8->tvs);_tmp3EB[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp3C8->name);_tmp3EB[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp3C8->kind);
_tmp3EB[0]=Cyc_Absynpp_scope2doc((void*)_tmp3C8->sc);Cyc_PP_cat(_tag_arr(_tmp3EB,
sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp3ED[12];_tmp3ED[
11]=Cyc_PP_text(({const char*_tmp3F1=";";_tag_arr(_tmp3F1,sizeof(char),
_get_zero_arr_size(_tmp3F1,2));}));_tmp3ED[10]=Cyc_Absynpp_atts2doc(_tmp3C8->attributes);
_tmp3ED[9]=Cyc_Absynpp_rb();_tmp3ED[8]=Cyc_PP_line_doc();_tmp3ED[7]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp3F0[2];_tmp3F0[1]=Cyc_Absynpp_aggrfields2doc(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3C8->impl))->fields);_tmp3F0[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp3F0,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3ED[6]=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3C8->impl))->rgn_po == 0?Cyc_PP_nil_doc():({
struct Cyc_PP_Doc*_tmp3EE[2];_tmp3EE[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3C8->impl))->rgn_po);_tmp3EE[0]=Cyc_PP_text(({const char*_tmp3EF=":";
_tag_arr(_tmp3EF,sizeof(char),_get_zero_arr_size(_tmp3EF,2));}));Cyc_PP_cat(
_tag_arr(_tmp3EE,sizeof(struct Cyc_PP_Doc*),2));});_tmp3ED[5]=Cyc_Absynpp_ktvars2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3C8->impl))->exist_vars);_tmp3ED[4]=
Cyc_Absynpp_lb();_tmp3ED[3]=Cyc_PP_blank_doc();_tmp3ED[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp3C8->name);_tmp3ED[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp3C8->kind);
_tmp3ED[0]=Cyc_Absynpp_scope2doc((void*)_tmp3C8->sc);Cyc_PP_cat(_tag_arr(_tmp3ED,
sizeof(struct Cyc_PP_Doc*),12));});}goto _LL28A;_LL28F: if(*((int*)_tmp3C6)!= 0)
goto _LL291;_tmp3C9=((struct Cyc_Absyn_Var_d_struct*)_tmp3C6)->f1;_LL290: s=Cyc_Absynpp_vardecl2doc(
_tmp3C9);goto _LL28A;_LL291: if(*((int*)_tmp3C6)!= 5)goto _LL293;_tmp3CA=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp3C6)->f1;_tmp3CB=*_tmp3CA;_tmp3CC=(void*)_tmp3CB.sc;
_tmp3CD=_tmp3CB.name;_tmp3CE=_tmp3CB.tvs;_tmp3CF=_tmp3CB.fields;_tmp3D0=_tmp3CB.is_xtunion;
_LL292: if(_tmp3CF == 0)s=({struct Cyc_PP_Doc*_tmp3F2[5];_tmp3F2[4]=Cyc_PP_text(({
const char*_tmp3F5=";";_tag_arr(_tmp3F5,sizeof(char),_get_zero_arr_size(_tmp3F5,2));}));
_tmp3F2[3]=Cyc_Absynpp_ktvars2doc(_tmp3CE);_tmp3F2[2]=_tmp3D0?Cyc_Absynpp_qvar2bolddoc(
_tmp3CD): Cyc_Absynpp_typedef_name2bolddoc(_tmp3CD);_tmp3F2[1]=_tmp3D0?Cyc_PP_text(({
const char*_tmp3F3="xtunion ";_tag_arr(_tmp3F3,sizeof(char),_get_zero_arr_size(
_tmp3F3,9));})): Cyc_PP_text(({const char*_tmp3F4="tunion ";_tag_arr(_tmp3F4,
sizeof(char),_get_zero_arr_size(_tmp3F4,8));}));_tmp3F2[0]=Cyc_Absynpp_scope2doc(
_tmp3CC);Cyc_PP_cat(_tag_arr(_tmp3F2,sizeof(struct Cyc_PP_Doc*),5));});else{s=({
struct Cyc_PP_Doc*_tmp3F6[10];_tmp3F6[9]=Cyc_PP_text(({const char*_tmp3FA=";";
_tag_arr(_tmp3FA,sizeof(char),_get_zero_arr_size(_tmp3FA,2));}));_tmp3F6[8]=Cyc_Absynpp_rb();
_tmp3F6[7]=Cyc_PP_line_doc();_tmp3F6[6]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3F9[
2];_tmp3F9[1]=Cyc_Absynpp_tunionfields2doc((struct Cyc_List_List*)_tmp3CF->v);
_tmp3F9[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3F9,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp3F6[5]=Cyc_Absynpp_lb();_tmp3F6[4]=Cyc_PP_blank_doc();_tmp3F6[3]=Cyc_Absynpp_ktvars2doc(
_tmp3CE);_tmp3F6[2]=_tmp3D0?Cyc_Absynpp_qvar2bolddoc(_tmp3CD): Cyc_Absynpp_typedef_name2bolddoc(
_tmp3CD);_tmp3F6[1]=_tmp3D0?Cyc_PP_text(({const char*_tmp3F7="xtunion ";_tag_arr(
_tmp3F7,sizeof(char),_get_zero_arr_size(_tmp3F7,9));})): Cyc_PP_text(({const char*
_tmp3F8="tunion ";_tag_arr(_tmp3F8,sizeof(char),_get_zero_arr_size(_tmp3F8,8));}));
_tmp3F6[0]=Cyc_Absynpp_scope2doc(_tmp3CC);Cyc_PP_cat(_tag_arr(_tmp3F6,sizeof(
struct Cyc_PP_Doc*),10));});}goto _LL28A;_LL293: if(*((int*)_tmp3C6)!= 2)goto _LL295;
_tmp3D1=((struct Cyc_Absyn_Let_d_struct*)_tmp3C6)->f1;_tmp3D2=((struct Cyc_Absyn_Let_d_struct*)
_tmp3C6)->f3;_LL294: s=({struct Cyc_PP_Doc*_tmp3FB[5];_tmp3FB[4]=Cyc_PP_text(({
const char*_tmp3FE=";";_tag_arr(_tmp3FE,sizeof(char),_get_zero_arr_size(_tmp3FE,2));}));
_tmp3FB[3]=Cyc_Absynpp_exp2doc(_tmp3D2);_tmp3FB[2]=Cyc_PP_text(({const char*
_tmp3FD=" = ";_tag_arr(_tmp3FD,sizeof(char),_get_zero_arr_size(_tmp3FD,4));}));
_tmp3FB[1]=Cyc_Absynpp_pat2doc(_tmp3D1);_tmp3FB[0]=Cyc_PP_text(({const char*
_tmp3FC="let ";_tag_arr(_tmp3FC,sizeof(char),_get_zero_arr_size(_tmp3FC,5));}));
Cyc_PP_cat(_tag_arr(_tmp3FB,sizeof(struct Cyc_PP_Doc*),5));});goto _LL28A;_LL295:
if(*((int*)_tmp3C6)!= 3)goto _LL297;_tmp3D3=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3C6)->f1;_LL296: s=({struct Cyc_PP_Doc*_tmp3FF[3];_tmp3FF[2]=Cyc_PP_text(({
const char*_tmp401=";";_tag_arr(_tmp401,sizeof(char),_get_zero_arr_size(_tmp401,2));}));
_tmp3FF[1]=Cyc_Absynpp_ids2doc(_tmp3D3);_tmp3FF[0]=Cyc_PP_text(({const char*
_tmp400="let ";_tag_arr(_tmp400,sizeof(char),_get_zero_arr_size(_tmp400,5));}));
Cyc_PP_cat(_tag_arr(_tmp3FF,sizeof(struct Cyc_PP_Doc*),3));});goto _LL28A;_LL297:
if(*((int*)_tmp3C6)!= 6)goto _LL299;_tmp3D4=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3C6)->f1;_tmp3D5=*_tmp3D4;_tmp3D6=(void*)_tmp3D5.sc;_tmp3D7=_tmp3D5.name;
_tmp3D8=_tmp3D5.fields;_LL298: if(_tmp3D8 == 0)s=({struct Cyc_PP_Doc*_tmp402[4];
_tmp402[3]=Cyc_PP_text(({const char*_tmp404=";";_tag_arr(_tmp404,sizeof(char),
_get_zero_arr_size(_tmp404,2));}));_tmp402[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3D7);_tmp402[1]=Cyc_PP_text(({const char*_tmp403="enum ";_tag_arr(_tmp403,
sizeof(char),_get_zero_arr_size(_tmp403,6));}));_tmp402[0]=Cyc_Absynpp_scope2doc(
_tmp3D6);Cyc_PP_cat(_tag_arr(_tmp402,sizeof(struct Cyc_PP_Doc*),4));});else{s=({
struct Cyc_PP_Doc*_tmp405[9];_tmp405[8]=Cyc_PP_text(({const char*_tmp408=";";
_tag_arr(_tmp408,sizeof(char),_get_zero_arr_size(_tmp408,2));}));_tmp405[7]=Cyc_Absynpp_rb();
_tmp405[6]=Cyc_PP_line_doc();_tmp405[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp407[
2];_tmp407[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp3D8->v);
_tmp407[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp407,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp405[4]=Cyc_Absynpp_lb();_tmp405[3]=Cyc_PP_blank_doc();_tmp405[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp3D7);_tmp405[1]=Cyc_PP_text(({const char*_tmp406="enum ";_tag_arr(_tmp406,
sizeof(char),_get_zero_arr_size(_tmp406,6));}));_tmp405[0]=Cyc_Absynpp_scope2doc(
_tmp3D6);Cyc_PP_cat(_tag_arr(_tmp405,sizeof(struct Cyc_PP_Doc*),9));});}goto
_LL28A;_LL299: if(*((int*)_tmp3C6)!= 7)goto _LL29B;_tmp3D9=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp3C6)->f1;_LL29A: {void*t;if(_tmp3D9->defn != 0)t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3D9->defn))->v;else{t=Cyc_Absyn_new_evar(_tmp3D9->kind,0);}s=({
struct Cyc_PP_Doc*_tmp409[3];_tmp409[2]=Cyc_PP_text(({const char*_tmp40E=";";
_tag_arr(_tmp40E,sizeof(char),_get_zero_arr_size(_tmp40E,2));}));_tmp409[1]=Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual _tmp40B;_tmp40B.q_const=0;_tmp40B.q_volatile=0;_tmp40B.q_restrict=
0;_tmp40B;}),t,({struct Cyc_Core_Opt*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C->v=({
struct Cyc_PP_Doc*_tmp40D[2];_tmp40D[1]=Cyc_Absynpp_tvars2doc(_tmp3D9->tvs);
_tmp40D[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp3D9->name);Cyc_PP_cat(_tag_arr(
_tmp40D,sizeof(struct Cyc_PP_Doc*),2));});_tmp40C;}));_tmp409[0]=Cyc_PP_text(({
const char*_tmp40A="typedef ";_tag_arr(_tmp40A,sizeof(char),_get_zero_arr_size(
_tmp40A,9));}));Cyc_PP_cat(_tag_arr(_tmp409,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL28A;}_LL29B: if(*((int*)_tmp3C6)!= 8)goto _LL29D;_tmp3DA=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3C6)->f1;_tmp3DB=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3C6)->f2;_LL29C:
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp3DA);s=({
struct Cyc_PP_Doc*_tmp40F[8];_tmp40F[7]=Cyc_Absynpp_rb();_tmp40F[6]=Cyc_PP_line_doc();
_tmp40F[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp411="";_tag_arr(_tmp411,sizeof(char),_get_zero_arr_size(_tmp411,1));}),
_tmp3DB);_tmp40F[4]=Cyc_PP_line_doc();_tmp40F[3]=Cyc_Absynpp_lb();_tmp40F[2]=Cyc_PP_blank_doc();
_tmp40F[1]=Cyc_PP_textptr(_tmp3DA);_tmp40F[0]=Cyc_PP_text(({const char*_tmp410="namespace ";
_tag_arr(_tmp410,sizeof(char),_get_zero_arr_size(_tmp410,11));}));Cyc_PP_cat(
_tag_arr(_tmp40F,sizeof(struct Cyc_PP_Doc*),8));});if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_drop();goto _LL28A;_LL29D: if(*((int*)_tmp3C6)!= 9)goto
_LL29F;_tmp3DC=((struct Cyc_Absyn_Using_d_struct*)_tmp3C6)->f1;_tmp3DD=((struct
Cyc_Absyn_Using_d_struct*)_tmp3C6)->f2;_LL29E: if(Cyc_Absynpp_print_using_stmts)s=({
struct Cyc_PP_Doc*_tmp412[8];_tmp412[7]=Cyc_Absynpp_rb();_tmp412[6]=Cyc_PP_line_doc();
_tmp412[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp414="";_tag_arr(_tmp414,sizeof(char),_get_zero_arr_size(_tmp414,1));}),
_tmp3DD);_tmp412[4]=Cyc_PP_line_doc();_tmp412[3]=Cyc_Absynpp_lb();_tmp412[2]=Cyc_PP_blank_doc();
_tmp412[1]=Cyc_Absynpp_qvar2doc(_tmp3DC);_tmp412[0]=Cyc_PP_text(({const char*
_tmp413="using ";_tag_arr(_tmp413,sizeof(char),_get_zero_arr_size(_tmp413,7));}));
Cyc_PP_cat(_tag_arr(_tmp412,sizeof(struct Cyc_PP_Doc*),8));});else{s=({struct Cyc_PP_Doc*
_tmp415[11];_tmp415[10]=Cyc_PP_text(({const char*_tmp41A=" */";_tag_arr(_tmp41A,
sizeof(char),_get_zero_arr_size(_tmp41A,4));}));_tmp415[9]=Cyc_Absynpp_rb();
_tmp415[8]=Cyc_PP_text(({const char*_tmp419="/* ";_tag_arr(_tmp419,sizeof(char),
_get_zero_arr_size(_tmp419,4));}));_tmp415[7]=Cyc_PP_line_doc();_tmp415[6]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp418="";_tag_arr(_tmp418,sizeof(char),_get_zero_arr_size(_tmp418,1));}),
_tmp3DD);_tmp415[5]=Cyc_PP_line_doc();_tmp415[4]=Cyc_PP_text(({const char*_tmp417=" */";
_tag_arr(_tmp417,sizeof(char),_get_zero_arr_size(_tmp417,4));}));_tmp415[3]=Cyc_Absynpp_lb();
_tmp415[2]=Cyc_PP_blank_doc();_tmp415[1]=Cyc_Absynpp_qvar2doc(_tmp3DC);_tmp415[0]=
Cyc_PP_text(({const char*_tmp416="/* using ";_tag_arr(_tmp416,sizeof(char),
_get_zero_arr_size(_tmp416,10));}));Cyc_PP_cat(_tag_arr(_tmp415,sizeof(struct Cyc_PP_Doc*),
11));});}goto _LL28A;_LL29F: if(*((int*)_tmp3C6)!= 10)goto _LL28A;_tmp3DE=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp3C6)->f1;_LL2A0: if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp41B[6];_tmp41B[5]=Cyc_Absynpp_rb();_tmp41B[4]=Cyc_PP_line_doc();
_tmp41B[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp41D="";_tag_arr(_tmp41D,sizeof(char),_get_zero_arr_size(_tmp41D,1));}),
_tmp3DE);_tmp41B[2]=Cyc_PP_line_doc();_tmp41B[1]=Cyc_Absynpp_lb();_tmp41B[0]=Cyc_PP_text(({
const char*_tmp41C="extern \"C\" ";_tag_arr(_tmp41C,sizeof(char),
_get_zero_arr_size(_tmp41C,12));}));Cyc_PP_cat(_tag_arr(_tmp41B,sizeof(struct Cyc_PP_Doc*),
6));});else{s=({struct Cyc_PP_Doc*_tmp41E[9];_tmp41E[8]=Cyc_PP_text(({const char*
_tmp423=" */";_tag_arr(_tmp423,sizeof(char),_get_zero_arr_size(_tmp423,4));}));
_tmp41E[7]=Cyc_Absynpp_rb();_tmp41E[6]=Cyc_PP_text(({const char*_tmp422="/* ";
_tag_arr(_tmp422,sizeof(char),_get_zero_arr_size(_tmp422,4));}));_tmp41E[5]=Cyc_PP_line_doc();
_tmp41E[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp421="";_tag_arr(_tmp421,sizeof(char),_get_zero_arr_size(_tmp421,1));}),
_tmp3DE);_tmp41E[3]=Cyc_PP_line_doc();_tmp41E[2]=Cyc_PP_text(({const char*_tmp420=" */";
_tag_arr(_tmp420,sizeof(char),_get_zero_arr_size(_tmp420,4));}));_tmp41E[1]=Cyc_Absynpp_lb();
_tmp41E[0]=Cyc_PP_text(({const char*_tmp41F="/* extern \"C\" ";_tag_arr(_tmp41F,
sizeof(char),_get_zero_arr_size(_tmp41F,15));}));Cyc_PP_cat(_tag_arr(_tmp41E,
sizeof(struct Cyc_PP_Doc*),9));});}goto _LL28A;_LL28A:;}return s;}int Cyc_Absynpp_print_scopes=
1;struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*sc){if(!Cyc_Absynpp_print_scopes)
return Cyc_PP_nil_doc();{void*_tmp424=sc;_LL2A2: if((int)_tmp424 != 0)goto _LL2A4;
_LL2A3: return Cyc_PP_text(({const char*_tmp425="static ";_tag_arr(_tmp425,sizeof(
char),_get_zero_arr_size(_tmp425,8));}));_LL2A4: if((int)_tmp424 != 2)goto _LL2A6;
_LL2A5: return Cyc_PP_nil_doc();_LL2A6: if((int)_tmp424 != 3)goto _LL2A8;_LL2A7:
return Cyc_PP_text(({const char*_tmp426="extern ";_tag_arr(_tmp426,sizeof(char),
_get_zero_arr_size(_tmp426,8));}));_LL2A8: if((int)_tmp424 != 4)goto _LL2AA;_LL2A9:
return Cyc_PP_text(({const char*_tmp427="extern \"C\" ";_tag_arr(_tmp427,sizeof(
char),_get_zero_arr_size(_tmp427,12));}));_LL2AA: if((int)_tmp424 != 1)goto _LL2AC;
_LL2AB: return Cyc_PP_text(({const char*_tmp428="abstract ";_tag_arr(_tmp428,
sizeof(char),_get_zero_arr_size(_tmp428,10));}));_LL2AC: if((int)_tmp424 != 5)goto
_LL2A1;_LL2AD: return Cyc_PP_text(({const char*_tmp429="register ";_tag_arr(_tmp429,
sizeof(char),_get_zero_arr_size(_tmp429,10));}));_LL2A1:;}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp42A=t;struct Cyc_Absyn_Tvar*_tmp42B;struct Cyc_List_List*_tmp42C;
_LL2AF: if(_tmp42A <= (void*)3?1:*((int*)_tmp42A)!= 1)goto _LL2B1;_tmp42B=((struct
Cyc_Absyn_VarType_struct*)_tmp42A)->f1;_LL2B0: return Cyc_Tcutil_is_temp_tvar(
_tmp42B);_LL2B1: if(_tmp42A <= (void*)3?1:*((int*)_tmp42A)!= 20)goto _LL2B3;_tmp42C=((
struct Cyc_Absyn_JoinEff_struct*)_tmp42A)->f1;_LL2B2: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp42C);_LL2B3:;_LL2B4: return 0;_LL2AE:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp42D=t;struct Cyc_Core_Opt*_tmp42E;struct Cyc_Core_Opt _tmp42F;void*_tmp430;
_LL2B6: if(_tmp42D <= (void*)3?1:*((int*)_tmp42D)!= 11)goto _LL2B8;_LL2B7: return 1;
_LL2B8: if(_tmp42D <= (void*)3?1:*((int*)_tmp42D)!= 16)goto _LL2BA;_tmp42E=((struct
Cyc_Absyn_TypedefType_struct*)_tmp42D)->f4;if(_tmp42E == 0)goto _LL2BA;_tmp42F=*
_tmp42E;_tmp430=(void*)_tmp42F.v;_LL2B9: return Cyc_Absynpp_is_anon_aggrtype(
_tmp430);_LL2BA:;_LL2BB: return 0;_LL2B5:;}struct _tuple7 Cyc_Absynpp_to_tms(struct
Cyc_Absyn_Tqual tq,void*t){void*_tmp431=t;struct Cyc_Absyn_ArrayInfo _tmp432;void*
_tmp433;struct Cyc_Absyn_Tqual _tmp434;struct Cyc_Absyn_Exp*_tmp435;struct Cyc_Absyn_Conref*
_tmp436;struct Cyc_Absyn_PtrInfo _tmp437;void*_tmp438;void*_tmp439;struct Cyc_Absyn_Conref*
_tmp43A;struct Cyc_Absyn_Tqual _tmp43B;struct Cyc_Absyn_Conref*_tmp43C;struct Cyc_Absyn_Conref*
_tmp43D;struct Cyc_Absyn_FnInfo _tmp43E;struct Cyc_List_List*_tmp43F;struct Cyc_Core_Opt*
_tmp440;void*_tmp441;struct Cyc_List_List*_tmp442;int _tmp443;struct Cyc_Absyn_VarargInfo*
_tmp444;struct Cyc_List_List*_tmp445;struct Cyc_List_List*_tmp446;struct Cyc_Core_Opt*
_tmp447;struct Cyc_Core_Opt*_tmp448;int _tmp449;struct _tuple0*_tmp44A;struct Cyc_List_List*
_tmp44B;struct Cyc_Core_Opt*_tmp44C;_LL2BD: if(_tmp431 <= (void*)3?1:*((int*)
_tmp431)!= 7)goto _LL2BF;_tmp432=((struct Cyc_Absyn_ArrayType_struct*)_tmp431)->f1;
_tmp433=(void*)_tmp432.elt_type;_tmp434=_tmp432.tq;_tmp435=_tmp432.num_elts;
_tmp436=_tmp432.zero_term;_LL2BE: {struct Cyc_Absyn_Tqual _tmp44E;void*_tmp44F;
struct Cyc_List_List*_tmp450;struct _tuple7 _tmp44D=Cyc_Absynpp_to_tms(_tmp434,
_tmp433);_tmp44E=_tmp44D.f1;_tmp44F=_tmp44D.f2;_tmp450=_tmp44D.f3;{void*tm;if(
_tmp435 == 0)tm=(void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp451=_cycalloc(
sizeof(*_tmp451));_tmp451[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp452;_tmp452.tag=
0;_tmp452.f1=_tmp436;_tmp452;});_tmp451;});else{tm=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp453=_cycalloc(sizeof(*_tmp453));_tmp453[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp454;_tmp454.tag=1;_tmp454.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp435);
_tmp454.f2=_tmp436;_tmp454;});_tmp453;});}return({struct _tuple7 _tmp455;_tmp455.f1=
_tmp44E;_tmp455.f2=_tmp44F;_tmp455.f3=({struct Cyc_List_List*_tmp456=_cycalloc(
sizeof(*_tmp456));_tmp456->hd=(void*)tm;_tmp456->tl=_tmp450;_tmp456;});_tmp455;});}}
_LL2BF: if(_tmp431 <= (void*)3?1:*((int*)_tmp431)!= 4)goto _LL2C1;_tmp437=((struct
Cyc_Absyn_PointerType_struct*)_tmp431)->f1;_tmp438=(void*)_tmp437.elt_typ;
_tmp439=(void*)_tmp437.rgn_typ;_tmp43A=_tmp437.nullable;_tmp43B=_tmp437.tq;
_tmp43C=_tmp437.bounds;_tmp43D=_tmp437.zero_term;_LL2C0: {struct Cyc_Absyn_Tqual
_tmp458;void*_tmp459;struct Cyc_List_List*_tmp45A;struct _tuple7 _tmp457=Cyc_Absynpp_to_tms(
_tmp43B,_tmp438);_tmp458=_tmp457.f1;_tmp459=_tmp457.f2;_tmp45A=_tmp457.f3;{void*
ps;{struct _tuple5 _tmp45C=({struct _tuple5 _tmp45B;_tmp45B.f1=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp43A))->v;_tmp45B.f2=(
void*)(Cyc_Absyn_compress_conref(_tmp43C))->v;_tmp45B;});void*_tmp45D;int _tmp45E;
void*_tmp45F;void*_tmp460;struct Cyc_Absyn_Exp*_tmp461;void*_tmp462;int _tmp463;
void*_tmp464;void*_tmp465;struct Cyc_Absyn_Exp*_tmp466;void*_tmp467;void*_tmp468;
_LL2CA: _tmp45D=_tmp45C.f1;if(_tmp45D <= (void*)1?1:*((int*)_tmp45D)!= 0)goto
_LL2CC;_tmp45E=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp45D)->f1;if(_tmp45E
!= 1)goto _LL2CC;_tmp45F=_tmp45C.f2;if(_tmp45F <= (void*)1?1:*((int*)_tmp45F)!= 0)
goto _LL2CC;_tmp460=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp45F)->f1;if(
_tmp460 <= (void*)1?1:*((int*)_tmp460)!= 0)goto _LL2CC;_tmp461=((struct Cyc_Absyn_Upper_b_struct*)
_tmp460)->f1;_LL2CB: ps=(void*)({struct Cyc_Absyn_Nullable_ps_struct*_tmp469=
_cycalloc(sizeof(*_tmp469));_tmp469[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp46A;_tmp46A.tag=1;_tmp46A.f1=_tmp461;_tmp46A.f2=_tmp43D;_tmp46A;});_tmp469;});
goto _LL2C9;_LL2CC: _tmp462=_tmp45C.f1;if(_tmp462 <= (void*)1?1:*((int*)_tmp462)!= 
0)goto _LL2CE;_tmp463=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp462)->f1;if(
_tmp463 != 0)goto _LL2CE;_tmp464=_tmp45C.f2;if(_tmp464 <= (void*)1?1:*((int*)
_tmp464)!= 0)goto _LL2CE;_tmp465=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp464)->f1;if(_tmp465 <= (void*)1?1:*((int*)_tmp465)!= 0)goto _LL2CE;_tmp466=((
struct Cyc_Absyn_Upper_b_struct*)_tmp465)->f1;_LL2CD: ps=(void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp46B=_cycalloc(sizeof(*_tmp46B));_tmp46B[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp46C;_tmp46C.tag=0;_tmp46C.f1=_tmp466;_tmp46C.f2=_tmp43D;_tmp46C;});_tmp46B;});
goto _LL2C9;_LL2CE: _tmp467=_tmp45C.f2;if(_tmp467 <= (void*)1?1:*((int*)_tmp467)!= 
0)goto _LL2D0;_tmp468=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp467)->f1;if((
int)_tmp468 != 0)goto _LL2D0;_LL2CF: ps=(void*)({struct Cyc_Absyn_TaggedArray_ps_struct*
_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D[0]=({struct Cyc_Absyn_TaggedArray_ps_struct
_tmp46E;_tmp46E.tag=2;_tmp46E.f1=_tmp43D;_tmp46E;});_tmp46D;});goto _LL2C9;_LL2D0:;
_LL2D1: ps=(void*)({struct Cyc_Absyn_TaggedArray_ps_struct*_tmp46F=_cycalloc(
sizeof(*_tmp46F));_tmp46F[0]=({struct Cyc_Absyn_TaggedArray_ps_struct _tmp470;
_tmp470.tag=2;_tmp470.f1=Cyc_Absyn_false_conref;_tmp470;});_tmp46F;});goto _LL2C9;
_LL2C9:;}{void*tm=(void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp473=_cycalloc(
sizeof(*_tmp473));_tmp473[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp474;_tmp474.tag=
2;_tmp474.f1=(void*)ps;_tmp474.f2=(void*)_tmp439;_tmp474.f3=tq;_tmp474;});
_tmp473;});return({struct _tuple7 _tmp471;_tmp471.f1=_tmp458;_tmp471.f2=_tmp459;
_tmp471.f3=({struct Cyc_List_List*_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->hd=(
void*)tm;_tmp472->tl=_tmp45A;_tmp472;});_tmp471;});}}}_LL2C1: if(_tmp431 <= (void*)
3?1:*((int*)_tmp431)!= 8)goto _LL2C3;_tmp43E=((struct Cyc_Absyn_FnType_struct*)
_tmp431)->f1;_tmp43F=_tmp43E.tvars;_tmp440=_tmp43E.effect;_tmp441=(void*)_tmp43E.ret_typ;
_tmp442=_tmp43E.args;_tmp443=_tmp43E.c_varargs;_tmp444=_tmp43E.cyc_varargs;
_tmp445=_tmp43E.rgn_po;_tmp446=_tmp43E.attributes;_LL2C2: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp440 == 0?1: Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp440->v)){
_tmp440=0;_tmp43F=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp43F);}{struct Cyc_Absyn_Tqual _tmp476;void*_tmp477;struct Cyc_List_List*_tmp478;
struct _tuple7 _tmp475=Cyc_Absynpp_to_tms(Cyc_Absyn_empty_tqual(),_tmp441);_tmp476=
_tmp475.f1;_tmp477=_tmp475.f2;_tmp478=_tmp475.f3;{struct Cyc_List_List*tms=
_tmp478;if(_tmp446 != 0?!Cyc_Absynpp_to_VC: 0)tms=({struct Cyc_List_List*_tmp479=
_cycalloc(sizeof(*_tmp479));_tmp479->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp47B;_tmp47B.tag=5;_tmp47B.f1=0;_tmp47B.f2=_tmp446;_tmp47B;});_tmp47A;}));
_tmp479->tl=tms;_tmp479;});tms=({struct Cyc_List_List*_tmp47C=_cycalloc(sizeof(*
_tmp47C));_tmp47C->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp47D=_cycalloc(sizeof(*_tmp47D));_tmp47D[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp47E;_tmp47E.tag=3;_tmp47E.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp47F=_cycalloc(sizeof(*_tmp47F));_tmp47F[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp480;_tmp480.tag=1;_tmp480.f1=_tmp442;_tmp480.f2=_tmp443;_tmp480.f3=_tmp444;
_tmp480.f4=_tmp440;_tmp480.f5=_tmp445;_tmp480;});_tmp47F;}));_tmp47E;});_tmp47D;}));
_tmp47C->tl=tms;_tmp47C;});if(Cyc_Absynpp_to_VC)for(0;_tmp446 != 0;_tmp446=
_tmp446->tl){void*_tmp481=(void*)_tmp446->hd;_LL2D3: if((int)_tmp481 != 0)goto
_LL2D5;_LL2D4: goto _LL2D6;_LL2D5: if((int)_tmp481 != 1)goto _LL2D7;_LL2D6: goto _LL2D8;
_LL2D7: if((int)_tmp481 != 2)goto _LL2D9;_LL2D8: tms=({struct Cyc_List_List*_tmp482=
_cycalloc(sizeof(*_tmp482));_tmp482->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp484;_tmp484.tag=5;_tmp484.f1=0;_tmp484.f2=({struct Cyc_List_List*_tmp485=
_cycalloc(sizeof(*_tmp485));_tmp485->hd=(void*)((void*)_tmp446->hd);_tmp485->tl=
0;_tmp485;});_tmp484;});_tmp483;}));_tmp482->tl=tms;_tmp482;});goto AfterAtts;
_LL2D9:;_LL2DA: goto _LL2D2;_LL2D2:;}AfterAtts: if(_tmp43F != 0)tms=({struct Cyc_List_List*
_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp487=_cycalloc(sizeof(*_tmp487));_tmp487[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp488;_tmp488.tag=4;_tmp488.f1=_tmp43F;_tmp488.f2=0;_tmp488.f3=1;_tmp488;});
_tmp487;}));_tmp486->tl=tms;_tmp486;});return({struct _tuple7 _tmp489;_tmp489.f1=
_tmp476;_tmp489.f2=_tmp477;_tmp489.f3=tms;_tmp489;});}}_LL2C3: if(_tmp431 <= (void*)
3?1:*((int*)_tmp431)!= 0)goto _LL2C5;_tmp447=((struct Cyc_Absyn_Evar_struct*)
_tmp431)->f1;_tmp448=((struct Cyc_Absyn_Evar_struct*)_tmp431)->f2;_tmp449=((
struct Cyc_Absyn_Evar_struct*)_tmp431)->f3;_LL2C4: if(_tmp448 == 0)return({struct
_tuple7 _tmp48A;_tmp48A.f1=tq;_tmp48A.f2=t;_tmp48A.f3=0;_tmp48A;});else{return Cyc_Absynpp_to_tms(
tq,(void*)_tmp448->v);}_LL2C5: if(_tmp431 <= (void*)3?1:*((int*)_tmp431)!= 16)goto
_LL2C7;_tmp44A=((struct Cyc_Absyn_TypedefType_struct*)_tmp431)->f1;_tmp44B=((
struct Cyc_Absyn_TypedefType_struct*)_tmp431)->f2;_tmp44C=((struct Cyc_Absyn_TypedefType_struct*)
_tmp431)->f4;_LL2C6: if((_tmp44C == 0?1: !Cyc_Absynpp_expand_typedefs)?1: Cyc_Absynpp_is_anon_aggrtype((
void*)_tmp44C->v))return({struct _tuple7 _tmp48B;_tmp48B.f1=tq;_tmp48B.f2=t;
_tmp48B.f3=0;_tmp48B;});else{return Cyc_Absynpp_to_tms(tq,(void*)_tmp44C->v);}
_LL2C7:;_LL2C8: return({struct _tuple7 _tmp48C;_tmp48C.f1=tq;_tmp48C.f2=t;_tmp48C.f3=
0;_tmp48C;});_LL2BC:;}static int Cyc_Absynpp_is_char_ptr(void*t){void*_tmp48D=t;
struct Cyc_Core_Opt*_tmp48E;struct Cyc_Core_Opt _tmp48F;void*_tmp490;struct Cyc_Absyn_PtrInfo
_tmp491;void*_tmp492;_LL2DC: if(_tmp48D <= (void*)3?1:*((int*)_tmp48D)!= 0)goto
_LL2DE;_tmp48E=((struct Cyc_Absyn_Evar_struct*)_tmp48D)->f2;if(_tmp48E == 0)goto
_LL2DE;_tmp48F=*_tmp48E;_tmp490=(void*)_tmp48F.v;_LL2DD: return Cyc_Absynpp_is_char_ptr(
_tmp490);_LL2DE: if(_tmp48D <= (void*)3?1:*((int*)_tmp48D)!= 4)goto _LL2E0;_tmp491=((
struct Cyc_Absyn_PointerType_struct*)_tmp48D)->f1;_tmp492=(void*)_tmp491.elt_typ;
_LL2DF: L: {void*_tmp493=_tmp492;struct Cyc_Core_Opt*_tmp494;struct Cyc_Core_Opt
_tmp495;void*_tmp496;struct Cyc_Core_Opt*_tmp497;struct Cyc_Core_Opt _tmp498;void*
_tmp499;void*_tmp49A;_LL2E3: if(_tmp493 <= (void*)3?1:*((int*)_tmp493)!= 0)goto
_LL2E5;_tmp494=((struct Cyc_Absyn_Evar_struct*)_tmp493)->f2;if(_tmp494 == 0)goto
_LL2E5;_tmp495=*_tmp494;_tmp496=(void*)_tmp495.v;_LL2E4: _tmp492=_tmp496;goto L;
_LL2E5: if(_tmp493 <= (void*)3?1:*((int*)_tmp493)!= 16)goto _LL2E7;_tmp497=((struct
Cyc_Absyn_TypedefType_struct*)_tmp493)->f4;if(_tmp497 == 0)goto _LL2E7;_tmp498=*
_tmp497;_tmp499=(void*)_tmp498.v;_LL2E6: _tmp492=_tmp499;goto L;_LL2E7: if(_tmp493
<= (void*)3?1:*((int*)_tmp493)!= 5)goto _LL2E9;_tmp49A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp493)->f2;if((int)_tmp49A != 0)goto _LL2E9;_LL2E8: return 1;_LL2E9:;_LL2EA: return
0;_LL2E2:;}_LL2E0:;_LL2E1: return 0;_LL2DB:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){struct Cyc_Absyn_Tqual
_tmp49C;void*_tmp49D;struct Cyc_List_List*_tmp49E;struct _tuple7 _tmp49B=Cyc_Absynpp_to_tms(
tq,typ);_tmp49C=_tmp49B.f1;_tmp49D=_tmp49B.f2;_tmp49E=_tmp49B.f3;_tmp49E=Cyc_List_imp_rev(
_tmp49E);if(_tmp49E == 0?dopt == 0: 0)return({struct Cyc_PP_Doc*_tmp49F[2];_tmp49F[1]=
Cyc_Absynpp_ntyp2doc(_tmp49D);_tmp49F[0]=Cyc_Absynpp_tqual2doc(_tmp49C);Cyc_PP_cat(
_tag_arr(_tmp49F,sizeof(struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*
_tmp4A0[4];_tmp4A0[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?
Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp49E);_tmp4A0[2]=Cyc_PP_text(({
const char*_tmp4A1=" ";_tag_arr(_tmp4A1,sizeof(char),_get_zero_arr_size(_tmp4A1,2));}));
_tmp4A0[1]=Cyc_Absynpp_ntyp2doc(_tmp49D);_tmp4A0[0]=Cyc_Absynpp_tqual2doc(
_tmp49C);Cyc_PP_cat(_tag_arr(_tmp4A0,sizeof(struct Cyc_PP_Doc*),4));});}}struct
Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){if(f->width != 
0)return({struct Cyc_PP_Doc*_tmp4A2[5];_tmp4A2[4]=Cyc_PP_text(({const char*_tmp4A5=";";
_tag_arr(_tmp4A5,sizeof(char),_get_zero_arr_size(_tmp4A5,2));}));_tmp4A2[3]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp4A2[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(
f->width));_tmp4A2[1]=Cyc_PP_text(({const char*_tmp4A4=":";_tag_arr(_tmp4A4,
sizeof(char),_get_zero_arr_size(_tmp4A4,2));}));_tmp4A2[0]=Cyc_Absynpp_tqtd2doc(
f->tq,(void*)f->type,({struct Cyc_Core_Opt*_tmp4A3=_cycalloc(sizeof(*_tmp4A3));
_tmp4A3->v=Cyc_PP_textptr(f->name);_tmp4A3;}));Cyc_PP_cat(_tag_arr(_tmp4A2,
sizeof(struct Cyc_PP_Doc*),5));});else{return({struct Cyc_PP_Doc*_tmp4A6[3];
_tmp4A6[2]=Cyc_PP_text(({const char*_tmp4A8=";";_tag_arr(_tmp4A8,sizeof(char),
_get_zero_arr_size(_tmp4A8,2));}));_tmp4A6[1]=Cyc_Absynpp_atts2doc(f->attributes);
_tmp4A6[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7->v=Cyc_PP_textptr(f->name);_tmp4A7;}));
Cyc_PP_cat(_tag_arr(_tmp4A6,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _tagged_arr sep,struct
Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,({const char*_tmp4A9="";
_tag_arr(_tmp4A9,sizeof(char),_get_zero_arr_size(_tmp4A9,1));}),fields);}struct
Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct Cyc_Absyn_Tunionfield*f){return({
struct Cyc_PP_Doc*_tmp4AA[3];_tmp4AA[2]=f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(
f->typs);_tmp4AA[1]=Cyc_Absynpp_typedef_name2doc(f->name);_tmp4AA[0]=Cyc_Absynpp_scope2doc((
void*)f->sc);Cyc_PP_cat(_tag_arr(_tmp4AA,sizeof(struct Cyc_PP_Doc*),3));});}
struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(struct Cyc_List_List*fields){return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Tunionfield*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_tunionfield2doc,({
const char*_tmp4AB=",";_tag_arr(_tmp4AB,sizeof(char),_get_zero_arr_size(_tmp4AB,2));}),
fields);}void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*
f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)
tdl->hd),72,f);({void*_tmp4AC[0]={};Cyc_fprintf(f,({const char*_tmp4AD="\n";
_tag_arr(_tmp4AD,sizeof(char),_get_zero_arr_size(_tmp4AD,2));}),_tag_arr(_tmp4AC,
sizeof(void*),0));});}}struct _tagged_arr Cyc_Absynpp_decllist2string(struct Cyc_List_List*
tdl){return Cyc_PP_string_of_doc(Cyc_PP_seql(({const char*_tmp4AE="";_tag_arr(
_tmp4AE,sizeof(char),_get_zero_arr_size(_tmp4AE,1));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_decl2doc,tdl)),72);}struct _tagged_arr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}struct
_tagged_arr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(
Cyc_Absynpp_stmt2doc(s),72);}struct _tagged_arr Cyc_Absynpp_typ2string(void*t){
return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}struct _tagged_arr Cyc_Absynpp_typ2cstring(
void*t){int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=0;{struct _tagged_arr s=Cyc_Absynpp_typ2string(
t);Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;}}struct _tagged_arr Cyc_Absynpp_prim2string(void*p){
return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}struct _tagged_arr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _tagged_arr Cyc_Absynpp_scope2string(void*sc){return Cyc_PP_string_of_doc(
Cyc_Absynpp_scope2doc(sc),72);}
