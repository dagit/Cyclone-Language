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
  if ((_czs_x) == NULL) _throw_null(); \
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
  if (_gres_x != NULL) { \
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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};typedef
struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc_Std___mbstate_t;
typedef struct{int __pos;Cyc_Std___mbstate_t __state;}Cyc_Std__G_fpos_t;typedef Cyc_Std__G_fpos_t
Cyc_Std_fpos_t;struct Cyc_Std___cycFILE;struct Cyc_Std_Cstdio___abstractFILE;
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct _tagged_arr,
struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2);extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc_Std___cycFILE*
f);struct _tagged_arr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*
Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();struct Cyc_PP_Doc*Cyc_PP_line_doc();
struct Cyc_PP_Doc*Cyc_PP_text(struct _tagged_arr s);struct Cyc_PP_Doc*Cyc_PP_textptr(
struct _tagged_arr*p);struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
struct Cyc_PP_Doc*Cyc_PP_cat(struct _tagged_arr);struct Cyc_PP_Doc*Cyc_PP_seq(
struct _tagged_arr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_ppseq(struct
Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*
Cyc_PP_seql(struct _tagged_arr sep,struct Cyc_List_List*l0);struct Cyc_PP_Doc*Cyc_PP_ppseql(
struct Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr sep,struct Cyc_List_List*l);struct
Cyc_PP_Doc*Cyc_PP_group(struct _tagged_arr start,struct _tagged_arr stop,struct
_tagged_arr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_egroup(struct
_tagged_arr start,struct _tagged_arr stop,struct _tagged_arr sep,struct Cyc_List_List*
l);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
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
Cyc_Absyn_UnknownTunionInfo{struct _tuple0*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{
int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{
int tag;struct Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*
tunion_info;struct Cyc_List_List*targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple0*tunion_name;struct _tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
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
void*);struct Cyc_Buffer_t;int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct
_tagged_arr*s2);struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct
_tagged_arr);struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,struct
_tagged_arr);struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
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
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f);
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
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;struct Cyc_Absynpp_Params;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){Cyc_Absynpp_expand_typedefs=
fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;Cyc_Absynpp_print_using_stmts=
fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_generate_line_directives=
fs->generate_line_directives;Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;
Cyc_Absynpp_curr_namespace=fs->curr_namespace;}struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={
0,0,0,0,0,1,0,0,1,1,0,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={
1,0,0,0,0,1,0,0,1,1,0,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,
1,0,1,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,
0,0,0,0,0,1,1,0,0,0,0};static void Cyc_Absynpp_curr_namespace_add(struct
_tagged_arr*v){Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,({
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
_LL36:;}struct _tagged_arr Cyc_Absynpp_kindbound2string(void*c){void*_tmp74=Cyc_Absyn_compress_kb(
c);void*_tmp75;void*_tmp76;_LL44: if(*((int*)_tmp74)!= 0)goto _LL46;_tmp75=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp74)->f1;_LL45: return Cyc_Absynpp_kind2string(
_tmp75);_LL46: if(*((int*)_tmp74)!= 1)goto _LL48;_LL47: if(Cyc_PP_tex_output)return({
const char*_tmp77="{?}";_tag_arr(_tmp77,sizeof(char),_get_zero_arr_size(_tmp77,4));});
else{return({const char*_tmp78="?";_tag_arr(_tmp78,sizeof(char),
_get_zero_arr_size(_tmp78,2));});}_LL48: if(*((int*)_tmp74)!= 2)goto _LL43;_tmp76=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp74)->f2;_LL49: return Cyc_Absynpp_kind2string(
_tmp76);_LL43:;}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(void*k){return Cyc_PP_text(
Cyc_Absynpp_kind2string(k));}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
return Cyc_PP_text(Cyc_Absynpp_kindbound2string(c));}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(
struct Cyc_List_List*ts){return Cyc_PP_egroup(({const char*_tmp79="<";_tag_arr(
_tmp79,sizeof(char),_get_zero_arr_size(_tmp79,2));}),({const char*_tmp7A=">";
_tag_arr(_tmp7A,sizeof(char),_get_zero_arr_size(_tmp7A,2));}),({const char*_tmp7B=",";
_tag_arr(_tmp7B,sizeof(char),_get_zero_arr_size(_tmp7B,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*
_tmp7C=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp7D;void*_tmp7E;void*
_tmp7F;_LL4B: if(*((int*)_tmp7C)!= 1)goto _LL4D;_LL4C: goto _LL4E;_LL4D: if(*((int*)
_tmp7C)!= 0)goto _LL4F;_tmp7D=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7C)->f1;
if((int)_tmp7D != 2)goto _LL4F;_LL4E: return Cyc_PP_textptr(tv->name);_LL4F: if(*((
int*)_tmp7C)!= 2)goto _LL51;_tmp7E=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp7C)->f2;_LL50: _tmp7F=_tmp7E;goto _LL52;_LL51: if(*((int*)_tmp7C)!= 0)goto _LL4A;
_tmp7F=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7C)->f1;_LL52: return({struct
Cyc_PP_Doc*_tmp80[3];_tmp80[2]=Cyc_Absynpp_kind2doc(_tmp7F);_tmp80[1]=Cyc_PP_text(({
const char*_tmp81="::";_tag_arr(_tmp81,sizeof(char),_get_zero_arr_size(_tmp81,3));}));
_tmp80[0]=Cyc_PP_textptr(tv->name);Cyc_PP_cat(_tag_arr(_tmp80,sizeof(struct Cyc_PP_Doc*),
3));});_LL4A:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return Cyc_PP_egroup(({const char*_tmp82="<";_tag_arr(_tmp82,sizeof(char),
_get_zero_arr_size(_tmp82,2));}),({const char*_tmp83=">";_tag_arr(_tmp83,sizeof(
char),_get_zero_arr_size(_tmp83,2));}),({const char*_tmp84=",";_tag_arr(_tmp84,
sizeof(char),_get_zero_arr_size(_tmp84,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,
tvs));}static struct _tagged_arr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);return Cyc_PP_egroup(({
const char*_tmp85="<";_tag_arr(_tmp85,sizeof(char),_get_zero_arr_size(_tmp85,2));}),({
const char*_tmp86=">";_tag_arr(_tmp86,sizeof(char),_get_zero_arr_size(_tmp86,2));}),({
const char*_tmp87=",";_tag_arr(_tmp87,sizeof(char),_get_zero_arr_size(_tmp87,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
tvs)));}struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(
struct _tuple10*t){return Cyc_Absynpp_tqtd2doc((*((struct _tuple10*)t)).f1,(*((
struct _tuple10*)t)).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*
ts){return Cyc_PP_group(({const char*_tmp88="(";_tag_arr(_tmp88,sizeof(char),
_get_zero_arr_size(_tmp88,2));}),({const char*_tmp89=")";_tag_arr(_tmp89,sizeof(
char),_get_zero_arr_size(_tmp89,2));}),({const char*_tmp8A=",";_tag_arr(_tmp8A,
sizeof(char),_get_zero_arr_size(_tmp8A,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmp8B=att;
_LL54: if((int)_tmp8B != 0)goto _LL56;_LL55: goto _LL57;_LL56: if((int)_tmp8B != 1)goto
_LL58;_LL57: goto _LL59;_LL58: if((int)_tmp8B != 2)goto _LL5A;_LL59: return Cyc_PP_nil_doc();
_LL5A:;_LL5B: return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL53:;}struct
Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 0;
atts=atts->tl){void*_tmp8C=(void*)atts->hd;_LL5D: if((int)_tmp8C != 0)goto _LL5F;
_LL5E: return Cyc_PP_text(({const char*_tmp8D=" _stdcall ";_tag_arr(_tmp8D,sizeof(
char),_get_zero_arr_size(_tmp8D,11));}));_LL5F: if((int)_tmp8C != 1)goto _LL61;
_LL60: return Cyc_PP_text(({const char*_tmp8E=" _cdecl ";_tag_arr(_tmp8E,sizeof(
char),_get_zero_arr_size(_tmp8E,9));}));_LL61: if((int)_tmp8C != 2)goto _LL63;_LL62:
return Cyc_PP_text(({const char*_tmp8F=" _fastcall ";_tag_arr(_tmp8F,sizeof(char),
_get_zero_arr_size(_tmp8F,12));}));_LL63:;_LL64: goto _LL5C;_LL5C:;}return Cyc_PP_nil_doc();}
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){int hasatt=
0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmp90=(
void*)atts2->hd;_LL66: if((int)_tmp90 != 0)goto _LL68;_LL67: goto _LL69;_LL68: if((int)
_tmp90 != 1)goto _LL6A;_LL69: goto _LL6B;_LL6A: if((int)_tmp90 != 2)goto _LL6C;_LL6B:
goto _LL65;_LL6C:;_LL6D: hasatt=1;goto _LL65;_LL65:;}}if(!hasatt)return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmp91[3];_tmp91[2]=Cyc_PP_text(({const char*_tmp96=")";
_tag_arr(_tmp96,sizeof(char),_get_zero_arr_size(_tmp96,2));}));_tmp91[1]=Cyc_PP_group(({
const char*_tmp93="";_tag_arr(_tmp93,sizeof(char),_get_zero_arr_size(_tmp93,1));}),({
const char*_tmp94="";_tag_arr(_tmp94,sizeof(char),_get_zero_arr_size(_tmp94,1));}),({
const char*_tmp95=" ";_tag_arr(_tmp95,sizeof(char),_get_zero_arr_size(_tmp95,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_noncallatt2doc,atts));_tmp91[0]=Cyc_PP_text(({const char*_tmp92="__declspec(";
_tag_arr(_tmp92,sizeof(char),_get_zero_arr_size(_tmp92,12));}));Cyc_PP_cat(
_tag_arr(_tmp91,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(
void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();if(Cyc_Absynpp_to_VC)
return Cyc_Absynpp_noncallconv2doc(atts);return({struct Cyc_PP_Doc*_tmp97[2];
_tmp97[1]=Cyc_PP_group(({const char*_tmp99="((";_tag_arr(_tmp99,sizeof(char),
_get_zero_arr_size(_tmp99,3));}),({const char*_tmp9A="))";_tag_arr(_tmp9A,sizeof(
char),_get_zero_arr_size(_tmp9A,3));}),({const char*_tmp9B=",";_tag_arr(_tmp9B,
sizeof(char),_get_zero_arr_size(_tmp9B,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,
atts));_tmp97[0]=Cyc_PP_text(({const char*_tmp98=" __attribute__";_tag_arr(_tmp98,
sizeof(char),_get_zero_arr_size(_tmp98,15));}));Cyc_PP_cat(_tag_arr(_tmp97,
sizeof(struct Cyc_PP_Doc*),2));});}int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*
tms){if(tms == 0)return 0;{void*_tmp9C=(void*)tms->hd;_LL6F: if(*((int*)_tmp9C)!= 2)
goto _LL71;_LL70: return 1;_LL71: if(*((int*)_tmp9C)!= 5)goto _LL73;_LL72: if(!Cyc_Absynpp_to_VC)
return 0;return Cyc_Absynpp_next_is_pointer(tms->tl);_LL73:;_LL74: return 0;_LL6E:;}}
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_question(){if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output)Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(({
const char*_tmp9D="{?}";_tag_arr(_tmp9D,sizeof(char),_get_zero_arr_size(_tmp9D,4));}));
else{Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(({const char*
_tmp9E="?";_tag_arr(_tmp9E,sizeof(char),_get_zero_arr_size(_tmp9E,2));}));}}
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}static struct
Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){if(!((
unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmp9F="{\\lb}";_tag_arr(_tmp9F,
sizeof(char),_get_zero_arr_size(_tmp9F,6));}));else{Cyc_Absynpp_cache_lb=(struct
Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA0="{";_tag_arr(_tmpA0,sizeof(char),
_get_zero_arr_size(_tmpA0,2));}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_rb=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA1="{\\rb}";_tag_arr(_tmpA1,
sizeof(char),_get_zero_arr_size(_tmpA1,6));}));else{Cyc_Absynpp_cache_rb=(struct
Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA2="}";_tag_arr(_tmpA2,sizeof(char),
_get_zero_arr_size(_tmpA2,2));}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA3="\\$";_tag_arr(_tmpA3,sizeof(
char),_get_zero_arr_size(_tmpA3,3));}));else{Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)
Cyc_PP_text(({const char*_tmpA4="$";_tag_arr(_tmpA4,sizeof(char),
_get_zero_arr_size(_tmpA4,2));}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _tagged_arr sep,struct Cyc_List_List*
ss){return({struct Cyc_PP_Doc*_tmpA5[3];_tmpA5[2]=Cyc_Absynpp_rb();_tmpA5[1]=Cyc_PP_seq(
sep,ss);_tmpA5[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_arr(_tmpA5,sizeof(struct Cyc_PP_Doc*),
3));});}struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(struct Cyc_PP_Doc*d,struct Cyc_List_List*
tms){if(tms == 0)return d;{struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmpE0[3];_tmpE0[2]=Cyc_PP_text(({
const char*_tmpE2=")";_tag_arr(_tmpE2,sizeof(char),_get_zero_arr_size(_tmpE2,2));}));
_tmpE0[1]=rest;_tmpE0[0]=Cyc_PP_text(({const char*_tmpE1="(";_tag_arr(_tmpE1,
sizeof(char),_get_zero_arr_size(_tmpE1,2));}));Cyc_PP_cat(_tag_arr(_tmpE0,
sizeof(struct Cyc_PP_Doc*),3));});void*_tmpA6=(void*)tms->hd;struct Cyc_Absyn_Conref*
_tmpA7;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_Absyn_Conref*_tmpA9;void*_tmpAA;
struct Cyc_List_List*_tmpAB;struct Cyc_List_List*_tmpAC;struct Cyc_Position_Segment*
_tmpAD;int _tmpAE;void*_tmpAF;void*_tmpB0;struct Cyc_Absyn_Tqual _tmpB1;_LL76: if(*((
int*)_tmpA6)!= 0)goto _LL78;_tmpA7=((struct Cyc_Absyn_Carray_mod_struct*)_tmpA6)->f1;
_LL77: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*
_tmpB2[2];_tmpB2[1]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA7)?Cyc_PP_text(({const char*_tmpB3="[]ZEROTERM ";_tag_arr(_tmpB3,sizeof(
char),_get_zero_arr_size(_tmpB3,12));})): Cyc_PP_text(({const char*_tmpB4="[]";
_tag_arr(_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,3));}));_tmpB2[0]=rest;
Cyc_PP_cat(_tag_arr(_tmpB2,sizeof(struct Cyc_PP_Doc*),2));});_LL78: if(*((int*)
_tmpA6)!= 1)goto _LL7A;_tmpA8=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpA6)->f1;
_tmpA9=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpA6)->f2;_LL79: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*_tmpB5[4];_tmpB5[3]=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpA9)?Cyc_PP_text(({const
char*_tmpB7="]ZEROTERM ";_tag_arr(_tmpB7,sizeof(char),_get_zero_arr_size(_tmpB7,
11));})): Cyc_PP_text(({const char*_tmpB8="]";_tag_arr(_tmpB8,sizeof(char),
_get_zero_arr_size(_tmpB8,2));}));_tmpB5[2]=Cyc_Absynpp_exp2doc(_tmpA8);_tmpB5[1]=
Cyc_PP_text(({const char*_tmpB6="[";_tag_arr(_tmpB6,sizeof(char),
_get_zero_arr_size(_tmpB6,2));}));_tmpB5[0]=rest;Cyc_PP_cat(_tag_arr(_tmpB5,
sizeof(struct Cyc_PP_Doc*),4));});_LL7A: if(*((int*)_tmpA6)!= 3)goto _LL7C;_tmpAA=(
void*)((struct Cyc_Absyn_Function_mod_struct*)_tmpA6)->f1;_LL7B: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmpB9=_tmpAA;struct Cyc_List_List*_tmpBA;int _tmpBB;
struct Cyc_Absyn_VarargInfo*_tmpBC;struct Cyc_Core_Opt*_tmpBD;struct Cyc_List_List*
_tmpBE;struct Cyc_List_List*_tmpBF;struct Cyc_Position_Segment*_tmpC0;_LL83: if(*((
int*)_tmpB9)!= 1)goto _LL85;_tmpBA=((struct Cyc_Absyn_WithTypes_struct*)_tmpB9)->f1;
_tmpBB=((struct Cyc_Absyn_WithTypes_struct*)_tmpB9)->f2;_tmpBC=((struct Cyc_Absyn_WithTypes_struct*)
_tmpB9)->f3;_tmpBD=((struct Cyc_Absyn_WithTypes_struct*)_tmpB9)->f4;_tmpBE=((
struct Cyc_Absyn_WithTypes_struct*)_tmpB9)->f5;_LL84: return({struct Cyc_PP_Doc*
_tmpC1[2];_tmpC1[1]=Cyc_Absynpp_funargs2doc(_tmpBA,_tmpBB,_tmpBC,_tmpBD,_tmpBE);
_tmpC1[0]=rest;Cyc_PP_cat(_tag_arr(_tmpC1,sizeof(struct Cyc_PP_Doc*),2));});_LL85:
if(*((int*)_tmpB9)!= 0)goto _LL82;_tmpBF=((struct Cyc_Absyn_NoTypes_struct*)_tmpB9)->f1;
_tmpC0=((struct Cyc_Absyn_NoTypes_struct*)_tmpB9)->f2;_LL86: return({struct Cyc_PP_Doc*
_tmpC2[2];_tmpC2[1]=Cyc_PP_group(({const char*_tmpC3="(";_tag_arr(_tmpC3,sizeof(
char),_get_zero_arr_size(_tmpC3,2));}),({const char*_tmpC4=")";_tag_arr(_tmpC4,
sizeof(char),_get_zero_arr_size(_tmpC4,2));}),({const char*_tmpC5=",";_tag_arr(
_tmpC5,sizeof(char),_get_zero_arr_size(_tmpC5,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,_tmpBF));_tmpC2[0]=rest;Cyc_PP_cat(_tag_arr(_tmpC2,sizeof(struct
Cyc_PP_Doc*),2));});_LL82:;}_LL7C: if(*((int*)_tmpA6)!= 5)goto _LL7E;_tmpAB=((
struct Cyc_Absyn_Attributes_mod_struct*)_tmpA6)->f2;_LL7D: if(!Cyc_Absynpp_to_VC){
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*
_tmpC6[2];_tmpC6[1]=Cyc_Absynpp_atts2doc(_tmpAB);_tmpC6[0]=rest;Cyc_PP_cat(
_tag_arr(_tmpC6,sizeof(struct Cyc_PP_Doc*),2));});}else{if(Cyc_Absynpp_next_is_pointer(
tms->tl))return({struct Cyc_PP_Doc*_tmpC7[2];_tmpC7[1]=rest;_tmpC7[0]=Cyc_Absynpp_callconv2doc(
_tmpAB);Cyc_PP_cat(_tag_arr(_tmpC7,sizeof(struct Cyc_PP_Doc*),2));});return rest;}
_LL7E: if(*((int*)_tmpA6)!= 4)goto _LL80;_tmpAC=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpA6)->f1;_tmpAD=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpA6)->f2;_tmpAE=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpA6)->f3;_LL7F: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;if(_tmpAE)return({struct Cyc_PP_Doc*_tmpC8[2];_tmpC8[1]=Cyc_Absynpp_ktvars2doc(
_tmpAC);_tmpC8[0]=rest;Cyc_PP_cat(_tag_arr(_tmpC8,sizeof(struct Cyc_PP_Doc*),2));});
else{return({struct Cyc_PP_Doc*_tmpC9[2];_tmpC9[1]=Cyc_Absynpp_tvars2doc(_tmpAC);
_tmpC9[0]=rest;Cyc_PP_cat(_tag_arr(_tmpC9,sizeof(struct Cyc_PP_Doc*),2));});}
_LL80: if(*((int*)_tmpA6)!= 2)goto _LL75;_tmpAF=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpA6)->f1;_tmpB0=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmpA6)->f2;
_tmpB1=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpA6)->f3;_LL81: {struct Cyc_PP_Doc*
ptr;{void*_tmpCA=_tmpAF;struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_Absyn_Conref*_tmpCC;
struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Conref*_tmpCE;struct Cyc_Absyn_Conref*
_tmpCF;_LL88: if(*((int*)_tmpCA)!= 1)goto _LL8A;_tmpCB=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmpCA)->f1;_tmpCC=((struct Cyc_Absyn_Nullable_ps_struct*)_tmpCA)->f2;_LL89: {
struct _tuple4 pr=Cyc_Evexp_eval_const_uint_exp(_tmpCB);if(pr.f1 == 1?pr.f2: 0)ptr=
Cyc_PP_text(({const char*_tmpD0="*";_tag_arr(_tmpD0,sizeof(char),
_get_zero_arr_size(_tmpD0,2));}));else{ptr=({struct Cyc_PP_Doc*_tmpD1[4];_tmpD1[3]=
Cyc_Absynpp_rb();_tmpD1[2]=Cyc_Absynpp_exp2doc(_tmpCB);_tmpD1[1]=Cyc_Absynpp_lb();
_tmpD1[0]=Cyc_PP_text(({const char*_tmpD2="*";_tag_arr(_tmpD2,sizeof(char),
_get_zero_arr_size(_tmpD2,2));}));Cyc_PP_cat(_tag_arr(_tmpD1,sizeof(struct Cyc_PP_Doc*),
4));});}if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpCC))
ptr=({struct Cyc_PP_Doc*_tmpD3[2];_tmpD3[1]=Cyc_PP_text(({const char*_tmpD4="ZEROTERM ";
_tag_arr(_tmpD4,sizeof(char),_get_zero_arr_size(_tmpD4,10));}));_tmpD3[0]=ptr;
Cyc_PP_cat(_tag_arr(_tmpD3,sizeof(struct Cyc_PP_Doc*),2));});goto _LL87;}_LL8A: if(*((
int*)_tmpCA)!= 0)goto _LL8C;_tmpCD=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmpCA)->f1;_tmpCE=((struct Cyc_Absyn_NonNullable_ps_struct*)_tmpCA)->f2;_LL8B: {
struct _tuple4 pr=Cyc_Evexp_eval_const_uint_exp(_tmpCD);if(pr.f1 == 1?pr.f2: 0)ptr=
Cyc_PP_text(({const char*_tmpD5="@";_tag_arr(_tmpD5,sizeof(char),
_get_zero_arr_size(_tmpD5,2));}));else{ptr=({struct Cyc_PP_Doc*_tmpD6[4];_tmpD6[3]=
Cyc_Absynpp_rb();_tmpD6[2]=Cyc_Absynpp_exp2doc(_tmpCD);_tmpD6[1]=Cyc_Absynpp_lb();
_tmpD6[0]=Cyc_PP_text(({const char*_tmpD7="@";_tag_arr(_tmpD7,sizeof(char),
_get_zero_arr_size(_tmpD7,2));}));Cyc_PP_cat(_tag_arr(_tmpD6,sizeof(struct Cyc_PP_Doc*),
4));});}if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpCE))
ptr=({struct Cyc_PP_Doc*_tmpD8[2];_tmpD8[1]=Cyc_PP_text(({const char*_tmpD9="ZEROTERM ";
_tag_arr(_tmpD9,sizeof(char),_get_zero_arr_size(_tmpD9,10));}));_tmpD8[0]=ptr;
Cyc_PP_cat(_tag_arr(_tmpD8,sizeof(struct Cyc_PP_Doc*),2));});goto _LL87;}_LL8C: if(*((
int*)_tmpCA)!= 2)goto _LL87;_tmpCF=((struct Cyc_Absyn_TaggedArray_ps_struct*)
_tmpCA)->f1;_LL8D: ptr=Cyc_Absynpp_question();if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpCF))ptr=({struct Cyc_PP_Doc*_tmpDA[2];_tmpDA[1]=
Cyc_PP_text(({const char*_tmpDB="ZEROTERM ";_tag_arr(_tmpDB,sizeof(char),
_get_zero_arr_size(_tmpDB,10));}));_tmpDA[0]=ptr;Cyc_PP_cat(_tag_arr(_tmpDA,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL87;_LL87:;}{void*_tmpDC=Cyc_Tcutil_compress(
_tmpB0);_LL8F: if((int)_tmpDC != 2)goto _LL91;_LL90: return({struct Cyc_PP_Doc*_tmpDD[
2];_tmpDD[1]=rest;_tmpDD[0]=ptr;Cyc_PP_cat(_tag_arr(_tmpDD,sizeof(struct Cyc_PP_Doc*),
2));});_LL91:;_LL92: return({struct Cyc_PP_Doc*_tmpDE[4];_tmpDE[3]=rest;_tmpDE[2]=
Cyc_PP_text(({const char*_tmpDF=" ";_tag_arr(_tmpDF,sizeof(char),
_get_zero_arr_size(_tmpDF,2));}));_tmpDE[1]=Cyc_Absynpp_typ2doc(_tmpB0);_tmpDE[0]=
ptr;Cyc_PP_cat(_tag_arr(_tmpDE,sizeof(struct Cyc_PP_Doc*),4));});_LL8E:;}}_LL75:;}}
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmpE3=Cyc_Tcutil_compress(t);
_LL94: if((int)_tmpE3 != 2)goto _LL96;_LL95: return Cyc_PP_text(({const char*_tmpE4="`H";
_tag_arr(_tmpE4,sizeof(char),_get_zero_arr_size(_tmpE4,3));}));_LL96:;_LL97:
return Cyc_Absynpp_ntyp2doc(t);_LL93:;}static void Cyc_Absynpp_effects2docs(struct
Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){void*_tmpE5=Cyc_Tcutil_compress(
t);void*_tmpE6;struct Cyc_List_List*_tmpE7;_LL99: if(_tmpE5 <= (void*)3?1:*((int*)
_tmpE5)!= 19)goto _LL9B;_tmpE6=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmpE5)->f1;
_LL9A:*((struct Cyc_List_List**)rgions)=({struct Cyc_List_List*_tmpE8=_cycalloc(
sizeof(*_tmpE8));_tmpE8->hd=Cyc_Absynpp_rgn2doc(_tmpE6);_tmpE8->tl=*((struct Cyc_List_List**)
rgions);_tmpE8;});goto _LL98;_LL9B: if(_tmpE5 <= (void*)3?1:*((int*)_tmpE5)!= 20)
goto _LL9D;_tmpE7=((struct Cyc_Absyn_JoinEff_struct*)_tmpE5)->f1;_LL9C: for(0;
_tmpE7 != 0;_tmpE7=_tmpE7->tl){Cyc_Absynpp_effects2docs(rgions,effects,(void*)
_tmpE7->hd);}goto _LL98;_LL9D:;_LL9E:*((struct Cyc_List_List**)effects)=({struct
Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->hd=Cyc_Absynpp_typ2doc(t);
_tmpE9->tl=*((struct Cyc_List_List**)effects);_tmpE9;});goto _LL98;_LL98:;}struct
Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*
effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);rgions=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);effects=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);if(rgions == 0?effects != 0: 0)
return Cyc_PP_group(({const char*_tmpEA="";_tag_arr(_tmpEA,sizeof(char),
_get_zero_arr_size(_tmpEA,1));}),({const char*_tmpEB="";_tag_arr(_tmpEB,sizeof(
char),_get_zero_arr_size(_tmpEB,1));}),({const char*_tmpEC="+";_tag_arr(_tmpEC,
sizeof(char),_get_zero_arr_size(_tmpEC,2));}),effects);else{struct Cyc_PP_Doc*
_tmpED=Cyc_Absynpp_group_braces(({const char*_tmpF2=",";_tag_arr(_tmpF2,sizeof(
char),_get_zero_arr_size(_tmpF2,2));}),rgions);return Cyc_PP_group(({const char*
_tmpEE="";_tag_arr(_tmpEE,sizeof(char),_get_zero_arr_size(_tmpEE,1));}),({const
char*_tmpEF="";_tag_arr(_tmpEF,sizeof(char),_get_zero_arr_size(_tmpEF,1));}),({
const char*_tmpF0="+";_tag_arr(_tmpF0,sizeof(char),_get_zero_arr_size(_tmpF0,2));}),((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
effects,({struct Cyc_List_List*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->hd=
_tmpED;_tmpF1->tl=0;_tmpF1;})));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(
void*k){void*_tmpF3=k;_LLA0: if((int)_tmpF3 != 0)goto _LLA2;_LLA1: return Cyc_PP_text(({
const char*_tmpF4="struct ";_tag_arr(_tmpF4,sizeof(char),_get_zero_arr_size(
_tmpF4,8));}));_LLA2: if((int)_tmpF3 != 1)goto _LL9F;_LLA3: return Cyc_PP_text(({
const char*_tmpF5="union ";_tag_arr(_tmpF5,sizeof(char),_get_zero_arr_size(_tmpF5,
7));}));_LL9F:;}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{
void*_tmpF6=t;struct Cyc_Core_Opt*_tmpF7;struct Cyc_Core_Opt*_tmpF8;int _tmpF9;
struct Cyc_Core_Opt*_tmpFA;struct Cyc_Absyn_Tvar*_tmpFB;struct Cyc_Absyn_TunionInfo
_tmpFC;void*_tmpFD;struct Cyc_List_List*_tmpFE;void*_tmpFF;struct Cyc_Absyn_TunionFieldInfo
_tmp100;void*_tmp101;struct Cyc_List_List*_tmp102;void*_tmp103;void*_tmp104;int
_tmp105;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_AggrInfo _tmp107;void*_tmp108;
struct Cyc_List_List*_tmp109;void*_tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_List_List*
_tmp10C;struct _tuple0*_tmp10D;void*_tmp10E;struct _tuple0*_tmp10F;struct Cyc_List_List*
_tmp110;struct Cyc_Absyn_Typedefdecl*_tmp111;struct Cyc_Core_Opt*_tmp112;void*
_tmp113;int _tmp114;void*_tmp115;void*_tmp116;_LLA5: if(_tmpF6 <= (void*)3?1:*((int*)
_tmpF6)!= 7)goto _LLA7;_LLA6: return Cyc_PP_text(({const char*_tmp117="[[[array]]]";
_tag_arr(_tmp117,sizeof(char),_get_zero_arr_size(_tmp117,12));}));_LLA7: if(
_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 8)goto _LLA9;_LLA8: return Cyc_PP_nil_doc();
_LLA9: if(_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 4)goto _LLAB;_LLAA: return Cyc_PP_nil_doc();
_LLAB: if((int)_tmpF6 != 0)goto _LLAD;_LLAC: s=Cyc_PP_text(({const char*_tmp118="void";
_tag_arr(_tmp118,sizeof(char),_get_zero_arr_size(_tmp118,5));}));goto _LLA4;_LLAD:
if(_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 0)goto _LLAF;_tmpF7=((struct Cyc_Absyn_Evar_struct*)
_tmpF6)->f1;_tmpF8=((struct Cyc_Absyn_Evar_struct*)_tmpF6)->f2;_tmpF9=((struct Cyc_Absyn_Evar_struct*)
_tmpF6)->f3;_tmpFA=((struct Cyc_Absyn_Evar_struct*)_tmpF6)->f4;_LLAE: if(_tmpF8 != 
0)return Cyc_Absynpp_ntyp2doc((void*)_tmpF8->v);else{void*_tmp119=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmpF7))->v;_LLD8: if((int)_tmp119 != 3)goto _LLDA;_LLD9:
s=({struct Cyc_PP_Doc*_tmp11A[2];_tmp11A[1]=Cyc_PP_text((struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp11E;_tmp11E.tag=1;_tmp11E.f1=(int)((unsigned int)
_tmpF9);{void*_tmp11C[1]={& _tmp11E};Cyc_Std_aprintf(({const char*_tmp11D="%d";
_tag_arr(_tmp11D,sizeof(char),_get_zero_arr_size(_tmp11D,3));}),_tag_arr(_tmp11C,
sizeof(void*),1));}}));_tmp11A[0]=Cyc_PP_text(({const char*_tmp11B="`r";_tag_arr(
_tmp11B,sizeof(char),_get_zero_arr_size(_tmp11B,3));}));Cyc_PP_cat(_tag_arr(
_tmp11A,sizeof(struct Cyc_PP_Doc*),2));});goto _LLD7;_LLDA:;_LLDB: s=({struct Cyc_PP_Doc*
_tmp11F[6];_tmp11F[5]=_tmpF7 == 0?Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((
void*)_tmpF7->v);_tmp11F[4]=Cyc_PP_text(({const char*_tmp126=")::";_tag_arr(
_tmp126,sizeof(char),_get_zero_arr_size(_tmp126,4));}));_tmp11F[3]=(!Cyc_Absynpp_print_full_evars?
1: _tmpFA == 0)?Cyc_PP_text(({const char*_tmp125="";_tag_arr(_tmp125,sizeof(char),
_get_zero_arr_size(_tmp125,1));})): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmpFA->v);_tmp11F[2]=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp124;_tmp124.tag=1;_tmp124.f1=(int)((unsigned int)_tmpF9);{void*_tmp122[1]={&
_tmp124};Cyc_Std_aprintf(({const char*_tmp123="%d";_tag_arr(_tmp123,sizeof(char),
_get_zero_arr_size(_tmp123,3));}),_tag_arr(_tmp122,sizeof(void*),1));}}));
_tmp11F[1]=Cyc_PP_text(({const char*_tmp121="(";_tag_arr(_tmp121,sizeof(char),
_get_zero_arr_size(_tmp121,2));}));_tmp11F[0]=Cyc_PP_text(({const char*_tmp120="%";
_tag_arr(_tmp120,sizeof(char),_get_zero_arr_size(_tmp120,2));}));Cyc_PP_cat(
_tag_arr(_tmp11F,sizeof(struct Cyc_PP_Doc*),6));});_LLD7:;}goto _LLA4;_LLAF: if(
_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 1)goto _LLB1;_tmpFB=((struct Cyc_Absyn_VarType_struct*)
_tmpF6)->f1;_LLB0: s=Cyc_PP_textptr(_tmpFB->name);if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp127[3];_tmp127[2]=Cyc_Absynpp_kindbound2doc((void*)
_tmpFB->kind);_tmp127[1]=Cyc_PP_text(({const char*_tmp128="::";_tag_arr(_tmp128,
sizeof(char),_get_zero_arr_size(_tmp128,3));}));_tmp127[0]=s;Cyc_PP_cat(_tag_arr(
_tmp127,sizeof(struct Cyc_PP_Doc*),3));});if(Cyc_Absynpp_rewrite_temp_tvars?Cyc_Tcutil_is_temp_tvar(
_tmpFB): 0)s=({struct Cyc_PP_Doc*_tmp129[3];_tmp129[2]=Cyc_PP_text(({const char*
_tmp12B=" */";_tag_arr(_tmp12B,sizeof(char),_get_zero_arr_size(_tmp12B,4));}));
_tmp129[1]=s;_tmp129[0]=Cyc_PP_text(({const char*_tmp12A="_ /* ";_tag_arr(_tmp12A,
sizeof(char),_get_zero_arr_size(_tmp12A,6));}));Cyc_PP_cat(_tag_arr(_tmp129,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLA4;_LLB1: if(_tmpF6 <= (void*)3?1:*((int*)
_tmpF6)!= 2)goto _LLB3;_tmpFC=((struct Cyc_Absyn_TunionType_struct*)_tmpF6)->f1;
_tmpFD=(void*)_tmpFC.tunion_info;_tmpFE=_tmpFC.targs;_tmpFF=(void*)_tmpFC.rgn;
_LLB2:{void*_tmp12C=_tmpFD;struct Cyc_Absyn_UnknownTunionInfo _tmp12D;struct
_tuple0*_tmp12E;int _tmp12F;struct Cyc_Absyn_Tuniondecl**_tmp130;struct Cyc_Absyn_Tuniondecl*
_tmp131;struct Cyc_Absyn_Tuniondecl _tmp132;struct _tuple0*_tmp133;int _tmp134;_LLDD:
if(*((int*)_tmp12C)!= 0)goto _LLDF;_tmp12D=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp12C)->f1;_tmp12E=_tmp12D.name;_tmp12F=_tmp12D.is_xtunion;_LLDE: _tmp133=
_tmp12E;_tmp134=_tmp12F;goto _LLE0;_LLDF: if(*((int*)_tmp12C)!= 1)goto _LLDC;
_tmp130=((struct Cyc_Absyn_KnownTunion_struct*)_tmp12C)->f1;_tmp131=*_tmp130;
_tmp132=*_tmp131;_tmp133=_tmp132.name;_tmp134=_tmp132.is_xtunion;_LLE0: {struct
Cyc_PP_Doc*_tmp135=Cyc_PP_text(_tmp134?({const char*_tmp13A="xtunion ";_tag_arr(
_tmp13A,sizeof(char),_get_zero_arr_size(_tmp13A,9));}):({const char*_tmp13B="tunion ";
_tag_arr(_tmp13B,sizeof(char),_get_zero_arr_size(_tmp13B,8));}));{void*_tmp136=
Cyc_Tcutil_compress(_tmpFF);_LLE2: if((int)_tmp136 != 2)goto _LLE4;_LLE3: s=({struct
Cyc_PP_Doc*_tmp137[3];_tmp137[2]=Cyc_Absynpp_tps2doc(_tmpFE);_tmp137[1]=Cyc_Absynpp_qvar2doc(
_tmp133);_tmp137[0]=_tmp135;Cyc_PP_cat(_tag_arr(_tmp137,sizeof(struct Cyc_PP_Doc*),
3));});goto _LLE1;_LLE4:;_LLE5: s=({struct Cyc_PP_Doc*_tmp138[5];_tmp138[4]=Cyc_Absynpp_tps2doc(
_tmpFE);_tmp138[3]=Cyc_Absynpp_qvar2doc(_tmp133);_tmp138[2]=Cyc_PP_text(({const
char*_tmp139=" ";_tag_arr(_tmp139,sizeof(char),_get_zero_arr_size(_tmp139,2));}));
_tmp138[1]=Cyc_Absynpp_typ2doc(_tmpFF);_tmp138[0]=_tmp135;Cyc_PP_cat(_tag_arr(
_tmp138,sizeof(struct Cyc_PP_Doc*),5));});goto _LLE1;_LLE1:;}goto _LLDC;}_LLDC:;}
goto _LLA4;_LLB3: if(_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 3)goto _LLB5;_tmp100=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmpF6)->f1;_tmp101=(void*)_tmp100.field_info;
_tmp102=_tmp100.targs;_LLB4:{void*_tmp13C=_tmp101;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp13D;struct _tuple0*_tmp13E;struct _tuple0*_tmp13F;int _tmp140;struct Cyc_Absyn_Tuniondecl*
_tmp141;struct Cyc_Absyn_Tuniondecl _tmp142;struct _tuple0*_tmp143;int _tmp144;
struct Cyc_Absyn_Tunionfield*_tmp145;struct Cyc_Absyn_Tunionfield _tmp146;struct
_tuple0*_tmp147;_LLE7: if(*((int*)_tmp13C)!= 0)goto _LLE9;_tmp13D=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp13C)->f1;_tmp13E=_tmp13D.tunion_name;_tmp13F=_tmp13D.field_name;_tmp140=
_tmp13D.is_xtunion;_LLE8: _tmp143=_tmp13E;_tmp144=_tmp140;_tmp147=_tmp13F;goto
_LLEA;_LLE9: if(*((int*)_tmp13C)!= 1)goto _LLE6;_tmp141=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp13C)->f1;_tmp142=*_tmp141;_tmp143=_tmp142.name;_tmp144=_tmp142.is_xtunion;
_tmp145=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp13C)->f2;_tmp146=*_tmp145;
_tmp147=_tmp146.name;_LLEA: {struct Cyc_PP_Doc*_tmp148=Cyc_PP_text(_tmp144?({
const char*_tmp14B="xtunion ";_tag_arr(_tmp14B,sizeof(char),_get_zero_arr_size(
_tmp14B,9));}):({const char*_tmp14C="tunion ";_tag_arr(_tmp14C,sizeof(char),
_get_zero_arr_size(_tmp14C,8));}));s=({struct Cyc_PP_Doc*_tmp149[4];_tmp149[3]=
Cyc_Absynpp_qvar2doc(_tmp147);_tmp149[2]=Cyc_PP_text(({const char*_tmp14A=".";
_tag_arr(_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,2));}));_tmp149[1]=Cyc_Absynpp_qvar2doc(
_tmp143);_tmp149[0]=_tmp148;Cyc_PP_cat(_tag_arr(_tmp149,sizeof(struct Cyc_PP_Doc*),
4));});goto _LLE6;}_LLE6:;}goto _LLA4;_LLB5: if(_tmpF6 <= (void*)3?1:*((int*)_tmpF6)
!= 5)goto _LLB7;_tmp103=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpF6)->f1;
_tmp104=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpF6)->f2;_LLB6: {struct
_tagged_arr sns;struct _tagged_arr ts;{void*_tmp14D=_tmp103;_LLEC: if((int)_tmp14D != 
2)goto _LLEE;_LLED: goto _LLEF;_LLEE: if((int)_tmp14D != 0)goto _LLF0;_LLEF: sns=({
const char*_tmp14E="";_tag_arr(_tmp14E,sizeof(char),_get_zero_arr_size(_tmp14E,1));});
goto _LLEB;_LLF0: if((int)_tmp14D != 1)goto _LLEB;_LLF1: sns=({const char*_tmp14F="unsigned ";
_tag_arr(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,10));});goto _LLEB;_LLEB:;}{
void*_tmp150=_tmp104;_LLF3: if((int)_tmp150 != 0)goto _LLF5;_LLF4:{void*_tmp151=
_tmp103;_LLFC: if((int)_tmp151 != 2)goto _LLFE;_LLFD: sns=({const char*_tmp152="";
_tag_arr(_tmp152,sizeof(char),_get_zero_arr_size(_tmp152,1));});goto _LLFB;_LLFE:
if((int)_tmp151 != 0)goto _LL100;_LLFF: sns=({const char*_tmp153="signed ";_tag_arr(
_tmp153,sizeof(char),_get_zero_arr_size(_tmp153,8));});goto _LLFB;_LL100: if((int)
_tmp151 != 1)goto _LLFB;_LL101: sns=({const char*_tmp154="unsigned ";_tag_arr(
_tmp154,sizeof(char),_get_zero_arr_size(_tmp154,10));});goto _LLFB;_LLFB:;}ts=({
const char*_tmp155="char";_tag_arr(_tmp155,sizeof(char),_get_zero_arr_size(
_tmp155,5));});goto _LLF2;_LLF5: if((int)_tmp150 != 1)goto _LLF7;_LLF6: ts=({const
char*_tmp156="short";_tag_arr(_tmp156,sizeof(char),_get_zero_arr_size(_tmp156,6));});
goto _LLF2;_LLF7: if((int)_tmp150 != 2)goto _LLF9;_LLF8: ts=({const char*_tmp157="int";
_tag_arr(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,4));});goto _LLF2;_LLF9:
if((int)_tmp150 != 3)goto _LLF2;_LLFA: ts=Cyc_Absynpp_to_VC?({const char*_tmp158="__int64";
_tag_arr(_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,8));}):({const char*
_tmp159="long long";_tag_arr(_tmp159,sizeof(char),_get_zero_arr_size(_tmp159,10));});
goto _LLF2;_LLF2:;}s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp15D;_tmp15D.tag=0;_tmp15D.f1=(struct _tagged_arr)ts;{struct Cyc_Std_String_pa_struct
_tmp15C;_tmp15C.tag=0;_tmp15C.f1=(struct _tagged_arr)sns;{void*_tmp15A[2]={&
_tmp15C,& _tmp15D};Cyc_Std_aprintf(({const char*_tmp15B="%s%s";_tag_arr(_tmp15B,
sizeof(char),_get_zero_arr_size(_tmp15B,5));}),_tag_arr(_tmp15A,sizeof(void*),2));}}}));
goto _LLA4;}_LLB7: if((int)_tmpF6 != 1)goto _LLB9;_LLB8: s=Cyc_PP_text(({const char*
_tmp15E="float";_tag_arr(_tmp15E,sizeof(char),_get_zero_arr_size(_tmp15E,6));}));
goto _LLA4;_LLB9: if(_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 6)goto _LLBB;_tmp105=((
struct Cyc_Absyn_DoubleType_struct*)_tmpF6)->f1;_LLBA: if(_tmp105)s=Cyc_PP_text(({
const char*_tmp15F="long double";_tag_arr(_tmp15F,sizeof(char),_get_zero_arr_size(
_tmp15F,12));}));else{s=Cyc_PP_text(({const char*_tmp160="double";_tag_arr(
_tmp160,sizeof(char),_get_zero_arr_size(_tmp160,7));}));}goto _LLA4;_LLBB: if(
_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 9)goto _LLBD;_tmp106=((struct Cyc_Absyn_TupleType_struct*)
_tmpF6)->f1;_LLBC: s=({struct Cyc_PP_Doc*_tmp161[2];_tmp161[1]=Cyc_Absynpp_args2doc(
_tmp106);_tmp161[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_arr(_tmp161,sizeof(
struct Cyc_PP_Doc*),2));});goto _LLA4;_LLBD: if(_tmpF6 <= (void*)3?1:*((int*)_tmpF6)
!= 10)goto _LLBF;_tmp107=((struct Cyc_Absyn_AggrType_struct*)_tmpF6)->f1;_tmp108=(
void*)_tmp107.aggr_info;_tmp109=_tmp107.targs;_LLBE: {void*_tmp163;struct _tuple0*
_tmp164;struct _tuple3 _tmp162=Cyc_Absyn_aggr_kinded_name(_tmp108);_tmp163=_tmp162.f1;
_tmp164=_tmp162.f2;s=({struct Cyc_PP_Doc*_tmp165[3];_tmp165[2]=Cyc_Absynpp_tps2doc(
_tmp109);_tmp165[1]=Cyc_Absynpp_qvar2doc(_tmp164);_tmp165[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp163);Cyc_PP_cat(_tag_arr(_tmp165,sizeof(struct Cyc_PP_Doc*),3));});goto _LLA4;}
_LLBF: if(_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 11)goto _LLC1;_tmp10A=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpF6)->f1;_tmp10B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF6)->f2;_LLC0: s=({struct Cyc_PP_Doc*_tmp166[4];_tmp166[3]=Cyc_Absynpp_rb();
_tmp166[2]=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp10B));_tmp166[1]=Cyc_Absynpp_lb();
_tmp166[0]=Cyc_Absynpp_aggr_kind2doc(_tmp10A);Cyc_PP_cat(_tag_arr(_tmp166,
sizeof(struct Cyc_PP_Doc*),4));});goto _LLA4;_LLC1: if(_tmpF6 <= (void*)3?1:*((int*)
_tmpF6)!= 13)goto _LLC3;_tmp10C=((struct Cyc_Absyn_AnonEnumType_struct*)_tmpF6)->f1;
_LLC2: s=({struct Cyc_PP_Doc*_tmp167[4];_tmp167[3]=Cyc_Absynpp_rb();_tmp167[2]=Cyc_PP_nest(
2,Cyc_Absynpp_enumfields2doc(_tmp10C));_tmp167[1]=Cyc_Absynpp_lb();_tmp167[0]=
Cyc_PP_text(({const char*_tmp168="enum ";_tag_arr(_tmp168,sizeof(char),
_get_zero_arr_size(_tmp168,6));}));Cyc_PP_cat(_tag_arr(_tmp167,sizeof(struct Cyc_PP_Doc*),
4));});goto _LLA4;_LLC3: if(_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 12)goto _LLC5;
_tmp10D=((struct Cyc_Absyn_EnumType_struct*)_tmpF6)->f1;_LLC4: s=({struct Cyc_PP_Doc*
_tmp169[2];_tmp169[1]=Cyc_Absynpp_qvar2doc(_tmp10D);_tmp169[0]=Cyc_PP_text(({
const char*_tmp16A="enum ";_tag_arr(_tmp16A,sizeof(char),_get_zero_arr_size(
_tmp16A,6));}));Cyc_PP_cat(_tag_arr(_tmp169,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLA4;_LLC5: if(_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 14)goto _LLC7;_tmp10E=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmpF6)->f1;_LLC6: s=({struct Cyc_PP_Doc*
_tmp16B[3];_tmp16B[2]=Cyc_PP_text(({const char*_tmp16D=">";_tag_arr(_tmp16D,
sizeof(char),_get_zero_arr_size(_tmp16D,2));}));_tmp16B[1]=Cyc_Absynpp_typ2doc(
_tmp10E);_tmp16B[0]=Cyc_PP_text(({const char*_tmp16C="sizeof_t<";_tag_arr(_tmp16C,
sizeof(char),_get_zero_arr_size(_tmp16C,10));}));Cyc_PP_cat(_tag_arr(_tmp16B,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLA4;_LLC7: if(_tmpF6 <= (void*)3?1:*((int*)
_tmpF6)!= 16)goto _LLC9;_tmp10F=((struct Cyc_Absyn_TypedefType_struct*)_tmpF6)->f1;
_tmp110=((struct Cyc_Absyn_TypedefType_struct*)_tmpF6)->f2;_tmp111=((struct Cyc_Absyn_TypedefType_struct*)
_tmpF6)->f3;_tmp112=((struct Cyc_Absyn_TypedefType_struct*)_tmpF6)->f4;_LLC8: s=({
struct Cyc_PP_Doc*_tmp16E[2];_tmp16E[1]=Cyc_Absynpp_tps2doc(_tmp110);_tmp16E[0]=
Cyc_Absynpp_qvar2doc(_tmp10F);Cyc_PP_cat(_tag_arr(_tmp16E,sizeof(struct Cyc_PP_Doc*),
2));});goto _LLA4;_LLC9: if(_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 15)goto _LLCB;
_tmp113=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmpF6)->f1;_LLCA: s=({
struct Cyc_PP_Doc*_tmp16F[3];_tmp16F[2]=Cyc_PP_text(({const char*_tmp171=">";
_tag_arr(_tmp171,sizeof(char),_get_zero_arr_size(_tmp171,2));}));_tmp16F[1]=Cyc_Absynpp_rgn2doc(
_tmp113);_tmp16F[0]=Cyc_PP_text(({const char*_tmp170="region_t<";_tag_arr(_tmp170,
sizeof(char),_get_zero_arr_size(_tmp170,10));}));Cyc_PP_cat(_tag_arr(_tmp16F,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLA4;_LLCB: if(_tmpF6 <= (void*)3?1:*((int*)
_tmpF6)!= 18)goto _LLCD;_tmp114=((struct Cyc_Absyn_TypeInt_struct*)_tmpF6)->f1;
_LLCC: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp174;
_tmp174.tag=1;_tmp174.f1=(int)((unsigned int)_tmp114);{void*_tmp172[1]={& _tmp174};
Cyc_Std_aprintf(({const char*_tmp173="`%d";_tag_arr(_tmp173,sizeof(char),
_get_zero_arr_size(_tmp173,4));}),_tag_arr(_tmp172,sizeof(void*),1));}}));goto
_LLA4;_LLCD: if(_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 17)goto _LLCF;_tmp115=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmpF6)->f1;_LLCE: s=({struct Cyc_PP_Doc*_tmp175[3];
_tmp175[2]=Cyc_PP_text(({const char*_tmp177=">";_tag_arr(_tmp177,sizeof(char),
_get_zero_arr_size(_tmp177,2));}));_tmp175[1]=Cyc_Absynpp_typ2doc(_tmp115);
_tmp175[0]=Cyc_PP_text(({const char*_tmp176="tag_t<";_tag_arr(_tmp176,sizeof(char),
_get_zero_arr_size(_tmp176,7));}));Cyc_PP_cat(_tag_arr(_tmp175,sizeof(struct Cyc_PP_Doc*),
3));});goto _LLA4;_LLCF: if((int)_tmpF6 != 2)goto _LLD1;_LLD0: s=Cyc_Absynpp_rgn2doc(
t);goto _LLA4;_LLD1: if(_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 21)goto _LLD3;_tmp116=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmpF6)->f1;_LLD2: s=({struct Cyc_PP_Doc*
_tmp178[3];_tmp178[2]=Cyc_PP_text(({const char*_tmp17A=")";_tag_arr(_tmp17A,
sizeof(char),_get_zero_arr_size(_tmp17A,2));}));_tmp178[1]=Cyc_Absynpp_typ2doc(
_tmp116);_tmp178[0]=Cyc_PP_text(({const char*_tmp179="regions(";_tag_arr(_tmp179,
sizeof(char),_get_zero_arr_size(_tmp179,9));}));Cyc_PP_cat(_tag_arr(_tmp178,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLA4;_LLD3: if(_tmpF6 <= (void*)3?1:*((int*)
_tmpF6)!= 19)goto _LLD5;_LLD4: goto _LLD6;_LLD5: if(_tmpF6 <= (void*)3?1:*((int*)
_tmpF6)!= 20)goto _LLA4;_LLD6: s=Cyc_Absynpp_eff2doc(t);goto _LLA4;_LLA4:;}return s;}
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc():
Cyc_PP_text(*((struct _tagged_arr*)((struct _tagged_arr*)vo->v)));}struct Cyc_PP_Doc*
Cyc_Absynpp_rgn_cmp2doc(struct _tuple5*cmp){struct _tuple5 _tmp17C;void*_tmp17D;
void*_tmp17E;struct _tuple5*_tmp17B=cmp;_tmp17C=*_tmp17B;_tmp17D=_tmp17C.f1;
_tmp17E=_tmp17C.f2;return({struct Cyc_PP_Doc*_tmp17F[3];_tmp17F[2]=Cyc_Absynpp_rgn2doc(
_tmp17E);_tmp17F[1]=Cyc_PP_text(({const char*_tmp180=" > ";_tag_arr(_tmp180,
sizeof(char),_get_zero_arr_size(_tmp180,4));}));_tmp17F[0]=Cyc_Absynpp_rgn2doc(
_tmp17D);Cyc_PP_cat(_tag_arr(_tmp17F,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*
Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){return Cyc_PP_group(({const char*
_tmp181="";_tag_arr(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,1));}),({
const char*_tmp182="";_tag_arr(_tmp182,sizeof(char),_get_zero_arr_size(_tmp182,1));}),({
const char*_tmp183=",";_tag_arr(_tmp183,sizeof(char),_get_zero_arr_size(_tmp183,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple5*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(
struct _tuple1*t){struct Cyc_Core_Opt*dopt=(*((struct _tuple1*)t)).f1 == 0?0:({
struct Cyc_Core_Opt*_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184->v=Cyc_PP_text(*((
struct _tagged_arr*)((struct _tagged_arr*)((struct Cyc_Core_Opt*)_check_null((*((
struct _tuple1*)t)).f1))->v)));_tmp184;});return Cyc_Absynpp_tqtd2doc((*((struct
_tuple1*)t)).f2,(*((struct _tuple1*)t)).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*
_tmp185=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_PP_Doc*
eff_doc;if(c_varargs)_tmp185=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmp185,({struct Cyc_List_List*_tmp186=
_cycalloc(sizeof(*_tmp186));_tmp186->hd=Cyc_PP_text(({const char*_tmp187="...";
_tag_arr(_tmp187,sizeof(char),_get_zero_arr_size(_tmp187,4));}));_tmp186->tl=0;
_tmp186;}));else{if(cyc_varargs != 0){struct Cyc_PP_Doc*_tmp188=({struct Cyc_PP_Doc*
_tmp18A[3];_tmp18A[2]=Cyc_Absynpp_funarg2doc(({struct _tuple1*_tmp18E=_cycalloc(
sizeof(*_tmp18E));_tmp18E->f1=cyc_varargs->name;_tmp18E->f2=cyc_varargs->tq;
_tmp18E->f3=(void*)cyc_varargs->type;_tmp18E;}));_tmp18A[1]=cyc_varargs->inject?
Cyc_PP_text(({const char*_tmp18C=" inject ";_tag_arr(_tmp18C,sizeof(char),
_get_zero_arr_size(_tmp18C,9));})): Cyc_PP_text(({const char*_tmp18D=" ";_tag_arr(
_tmp18D,sizeof(char),_get_zero_arr_size(_tmp18D,2));}));_tmp18A[0]=Cyc_PP_text(({
const char*_tmp18B="...";_tag_arr(_tmp18B,sizeof(char),_get_zero_arr_size(_tmp18B,
4));}));Cyc_PP_cat(_tag_arr(_tmp18A,sizeof(struct Cyc_PP_Doc*),3));});_tmp185=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp185,({struct Cyc_List_List*_tmp189=_cycalloc(sizeof(*_tmp189));_tmp189->hd=
_tmp188;_tmp189->tl=0;_tmp189;}));}}{struct Cyc_PP_Doc*_tmp18F=Cyc_PP_group(({
const char*_tmp197="";_tag_arr(_tmp197,sizeof(char),_get_zero_arr_size(_tmp197,1));}),({
const char*_tmp198="";_tag_arr(_tmp198,sizeof(char),_get_zero_arr_size(_tmp198,1));}),({
const char*_tmp199=",";_tag_arr(_tmp199,sizeof(char),_get_zero_arr_size(_tmp199,2));}),
_tmp185);if(effopt != 0)_tmp18F=({struct Cyc_PP_Doc*_tmp190[3];_tmp190[2]=Cyc_Absynpp_eff2doc((
void*)effopt->v);_tmp190[1]=Cyc_PP_text(({const char*_tmp191=";";_tag_arr(_tmp191,
sizeof(char),_get_zero_arr_size(_tmp191,2));}));_tmp190[0]=_tmp18F;Cyc_PP_cat(
_tag_arr(_tmp190,sizeof(struct Cyc_PP_Doc*),3));});if(rgn_po != 0)_tmp18F=({struct
Cyc_PP_Doc*_tmp192[3];_tmp192[2]=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp192[1]=Cyc_PP_text(({
const char*_tmp193=":";_tag_arr(_tmp193,sizeof(char),_get_zero_arr_size(_tmp193,2));}));
_tmp192[0]=_tmp18F;Cyc_PP_cat(_tag_arr(_tmp192,sizeof(struct Cyc_PP_Doc*),3));});
return({struct Cyc_PP_Doc*_tmp194[3];_tmp194[2]=Cyc_PP_text(({const char*_tmp196=")";
_tag_arr(_tmp196,sizeof(char),_get_zero_arr_size(_tmp196,2));}));_tmp194[1]=
_tmp18F;_tmp194[0]=Cyc_PP_text(({const char*_tmp195="(";_tag_arr(_tmp195,sizeof(
char),_get_zero_arr_size(_tmp195,2));}));Cyc_PP_cat(_tag_arr(_tmp194,sizeof(
struct Cyc_PP_Doc*),3));});}}struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*
arg){return({struct _tuple1*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->f1=({
struct Cyc_Core_Opt*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->v=(*((struct
_tuple6*)arg)).f1;_tmp19B;});_tmp19A->f2=(*((struct _tuple6*)arg)).f2;_tmp19A->f3=(*((
struct _tuple6*)arg)).f3;_tmp19A;});}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_tagged_arr*v){return Cyc_PP_text(*((struct _tagged_arr*)v));}struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*q){struct Cyc_List_List*_tmp19C=0;int match;{void*_tmp19D=(*((struct
_tuple0*)q)).f1;struct Cyc_List_List*_tmp19E;struct Cyc_List_List*_tmp19F;_LL103:
if((int)_tmp19D != 0)goto _LL105;_LL104: _tmp19E=0;goto _LL106;_LL105: if(_tmp19D <= (
void*)1?1:*((int*)_tmp19D)!= 0)goto _LL107;_tmp19E=((struct Cyc_Absyn_Rel_n_struct*)
_tmp19D)->f1;_LL106: match=0;_tmp19C=_tmp19E;goto _LL102;_LL107: if(_tmp19D <= (void*)
1?1:*((int*)_tmp19D)!= 1)goto _LL102;_tmp19F=((struct Cyc_Absyn_Abs_n_struct*)
_tmp19D)->f1;_LL108: match=Cyc_Absynpp_use_curr_namespace?((int(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_prefix)(Cyc_Std_strptrcmp,_tmp19F,Cyc_Absynpp_curr_namespace):
0;_tmp19C=(Cyc_Absynpp_qvar_to_Cids?Cyc_Absynpp_add_cyc_prefix: 0)?({struct Cyc_List_List*
_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0->hd=Cyc_Absynpp_cyc_stringptr;
_tmp1A0->tl=_tmp19F;_tmp1A0;}): _tmp19F;goto _LL102;_LL102:;}if(Cyc_Absynpp_qvar_to_Cids)
return(struct _tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp19C,({struct Cyc_List_List*_tmp1A1=
_cycalloc(sizeof(*_tmp1A1));_tmp1A1->hd=(*((struct _tuple0*)q)).f2;_tmp1A1->tl=0;
_tmp1A1;})),({const char*_tmp1A2="_";_tag_arr(_tmp1A2,sizeof(char),
_get_zero_arr_size(_tmp1A2,2));}));else{if(match)return*((struct _tagged_arr*)(*((
struct _tuple0*)q)).f2);else{return(struct _tagged_arr)Cyc_Std_str_sepstr(((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp19C,({struct Cyc_List_List*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->hd=(*((
struct _tuple0*)q)).f2;_tmp1A3->tl=0;_tmp1A3;})),({const char*_tmp1A4="::";
_tag_arr(_tmp1A4,sizeof(char),_get_zero_arr_size(_tmp1A4,3));}));}}}struct Cyc_PP_Doc*
Cyc_Absynpp_qvar2doc(struct _tuple0*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(
q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){if(Cyc_PP_tex_output)
return({struct Cyc_PP_Doc*_tmp1A5[3];_tmp1A5[2]=Cyc_Absynpp_rb();_tmp1A5[1]=Cyc_PP_text(
Cyc_Absynpp_qvar2string(q));_tmp1A5[0]=Cyc_PP_text(({const char*_tmp1A6="\\textbf{";
_tag_arr(_tmp1A6,sizeof(char),_get_zero_arr_size(_tmp1A6,9));}));Cyc_PP_cat(
_tag_arr(_tmp1A5,sizeof(struct Cyc_PP_Doc*),3));});else{return Cyc_PP_text(Cyc_Absynpp_qvar2string(
q));}}struct _tagged_arr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)
return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){void*_tmp1A7=(*((
struct _tuple0*)v)).f1;struct Cyc_List_List*_tmp1A8;struct Cyc_List_List*_tmp1A9;
_LL10A: if((int)_tmp1A7 != 0)goto _LL10C;_LL10B: goto _LL10D;_LL10C: if(_tmp1A7 <= (
void*)1?1:*((int*)_tmp1A7)!= 0)goto _LL10E;_tmp1A8=((struct Cyc_Absyn_Rel_n_struct*)
_tmp1A7)->f1;if(_tmp1A8 != 0)goto _LL10E;_LL10D: return*((struct _tagged_arr*)(*((
struct _tuple0*)v)).f2);_LL10E: if(_tmp1A7 <= (void*)1?1:*((int*)_tmp1A7)!= 1)goto
_LL110;_tmp1A9=((struct Cyc_Absyn_Abs_n_struct*)_tmp1A7)->f1;_LL10F: if(((int(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Std_strptrcmp,_tmp1A9,Cyc_Absynpp_curr_namespace)
== 0)return*((struct _tagged_arr*)(*((struct _tuple0*)v)).f2);else{goto _LL111;}
_LL110:;_LL111: return(struct _tagged_arr)Cyc_Std_strconcat(({const char*_tmp1AA="/* bad namespace : */ ";
_tag_arr(_tmp1AA,sizeof(char),_get_zero_arr_size(_tmp1AA,23));}),Cyc_Absynpp_qvar2string(
v));_LL109:;}else{return*((struct _tagged_arr*)(*((struct _tuple0*)v)).f2);}}
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){return Cyc_PP_text(
Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v){if(Cyc_PP_tex_output)return({struct Cyc_PP_Doc*_tmp1AB[3];
_tmp1AB[2]=Cyc_Absynpp_rb();_tmp1AB[1]=Cyc_PP_text(Cyc_Absynpp_typedef_name2string(
v));_tmp1AB[0]=Cyc_PP_text(({const char*_tmp1AC="\\textbf{";_tag_arr(_tmp1AC,
sizeof(char),_get_zero_arr_size(_tmp1AC,9));}));Cyc_PP_cat(_tag_arr(_tmp1AB,
sizeof(struct Cyc_PP_Doc*),3));});else{return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(
v));}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(),t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp1AD=(void*)e->r;void*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1AF;struct Cyc_Absyn_Exp*
_tmp1B0;_LL113: if(*((int*)_tmp1AD)!= 0)goto _LL115;_LL114: goto _LL116;_LL115: if(*((
int*)_tmp1AD)!= 1)goto _LL117;_LL116: goto _LL118;_LL117: if(*((int*)_tmp1AD)!= 2)
goto _LL119;_LL118: return 10000;_LL119: if(*((int*)_tmp1AD)!= 3)goto _LL11B;_tmp1AE=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1AD)->f1;_LL11A: {void*_tmp1B1=
_tmp1AE;_LL160: if((int)_tmp1B1 != 0)goto _LL162;_LL161: return 100;_LL162: if((int)
_tmp1B1 != 1)goto _LL164;_LL163: return 110;_LL164: if((int)_tmp1B1 != 2)goto _LL166;
_LL165: return 100;_LL166: if((int)_tmp1B1 != 3)goto _LL168;_LL167: goto _LL169;_LL168:
if((int)_tmp1B1 != 4)goto _LL16A;_LL169: return 110;_LL16A: if((int)_tmp1B1 != 5)goto
_LL16C;_LL16B: goto _LL16D;_LL16C: if((int)_tmp1B1 != 6)goto _LL16E;_LL16D: return 70;
_LL16E: if((int)_tmp1B1 != 7)goto _LL170;_LL16F: goto _LL171;_LL170: if((int)_tmp1B1 != 
8)goto _LL172;_LL171: goto _LL173;_LL172: if((int)_tmp1B1 != 9)goto _LL174;_LL173: goto
_LL175;_LL174: if((int)_tmp1B1 != 10)goto _LL176;_LL175: return 80;_LL176: if((int)
_tmp1B1 != 11)goto _LL178;_LL177: goto _LL179;_LL178: if((int)_tmp1B1 != 12)goto _LL17A;
_LL179: return 130;_LL17A: if((int)_tmp1B1 != 13)goto _LL17C;_LL17B: return 60;_LL17C:
if((int)_tmp1B1 != 14)goto _LL17E;_LL17D: return 40;_LL17E: if((int)_tmp1B1 != 15)goto
_LL180;_LL17F: return 50;_LL180: if((int)_tmp1B1 != 16)goto _LL182;_LL181: return 90;
_LL182: if((int)_tmp1B1 != 17)goto _LL184;_LL183: return 80;_LL184: if((int)_tmp1B1 != 
18)goto _LL186;_LL185: return 80;_LL186: if((int)_tmp1B1 != 19)goto _LL15F;_LL187:
return 140;_LL15F:;}_LL11B: if(*((int*)_tmp1AD)!= 4)goto _LL11D;_LL11C: return 20;
_LL11D: if(*((int*)_tmp1AD)!= 5)goto _LL11F;_LL11E: return 130;_LL11F: if(*((int*)
_tmp1AD)!= 6)goto _LL121;_LL120: return 30;_LL121: if(*((int*)_tmp1AD)!= 7)goto
_LL123;_LL122: return 10;_LL123: if(*((int*)_tmp1AD)!= 8)goto _LL125;_LL124: goto
_LL126;_LL125: if(*((int*)_tmp1AD)!= 9)goto _LL127;_LL126: return 140;_LL127: if(*((
int*)_tmp1AD)!= 10)goto _LL129;_LL128: return 130;_LL129: if(*((int*)_tmp1AD)!= 11)
goto _LL12B;_tmp1AF=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1AD)->f1;_LL12A:
return Cyc_Absynpp_exp_prec(_tmp1AF);_LL12B: if(*((int*)_tmp1AD)!= 12)goto _LL12D;
_tmp1B0=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1AD)->f1;_LL12C: return Cyc_Absynpp_exp_prec(
_tmp1B0);_LL12D: if(*((int*)_tmp1AD)!= 13)goto _LL12F;_LL12E: return 120;_LL12F: if(*((
int*)_tmp1AD)!= 15)goto _LL131;_LL130: return 15;_LL131: if(*((int*)_tmp1AD)!= 14)
goto _LL133;_LL132: goto _LL134;_LL133: if(*((int*)_tmp1AD)!= 16)goto _LL135;_LL134:
goto _LL136;_LL135: if(*((int*)_tmp1AD)!= 17)goto _LL137;_LL136: goto _LL138;_LL137:
if(*((int*)_tmp1AD)!= 18)goto _LL139;_LL138: goto _LL13A;_LL139: if(*((int*)_tmp1AD)
!= 19)goto _LL13B;_LL13A: goto _LL13C;_LL13B: if(*((int*)_tmp1AD)!= 20)goto _LL13D;
_LL13C: return 130;_LL13D: if(*((int*)_tmp1AD)!= 21)goto _LL13F;_LL13E: goto _LL140;
_LL13F: if(*((int*)_tmp1AD)!= 22)goto _LL141;_LL140: goto _LL142;_LL141: if(*((int*)
_tmp1AD)!= 23)goto _LL143;_LL142: return 140;_LL143: if(*((int*)_tmp1AD)!= 24)goto
_LL145;_LL144: return 150;_LL145: if(*((int*)_tmp1AD)!= 25)goto _LL147;_LL146: goto
_LL148;_LL147: if(*((int*)_tmp1AD)!= 26)goto _LL149;_LL148: goto _LL14A;_LL149: if(*((
int*)_tmp1AD)!= 27)goto _LL14B;_LL14A: goto _LL14C;_LL14B: if(*((int*)_tmp1AD)!= 28)
goto _LL14D;_LL14C: goto _LL14E;_LL14D: if(*((int*)_tmp1AD)!= 29)goto _LL14F;_LL14E:
goto _LL150;_LL14F: if(*((int*)_tmp1AD)!= 30)goto _LL151;_LL150: goto _LL152;_LL151:
if(*((int*)_tmp1AD)!= 31)goto _LL153;_LL152: goto _LL154;_LL153: if(*((int*)_tmp1AD)
!= 32)goto _LL155;_LL154: goto _LL156;_LL155: if(*((int*)_tmp1AD)!= 33)goto _LL157;
_LL156: goto _LL158;_LL157: if(*((int*)_tmp1AD)!= 34)goto _LL159;_LL158: return 140;
_LL159: if(*((int*)_tmp1AD)!= 35)goto _LL15B;_LL15A: return 10000;_LL15B: if(*((int*)
_tmp1AD)!= 36)goto _LL15D;_LL15C: goto _LL15E;_LL15D: if(*((int*)_tmp1AD)!= 37)goto
_LL112;_LL15E: return 140;_LL112:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*
e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp1B2=(void*)e->r;void*_tmp1B3;struct _tuple0*_tmp1B4;struct _tuple0*
_tmp1B5;void*_tmp1B6;struct Cyc_List_List*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B8;
struct Cyc_Core_Opt*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1BA;struct Cyc_Absyn_Exp*
_tmp1BB;void*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BD;struct Cyc_Absyn_Exp*_tmp1BE;
struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Exp*
_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_Absyn_Exp*
_tmp1C4;struct Cyc_List_List*_tmp1C5;struct Cyc_Absyn_Exp*_tmp1C6;struct Cyc_Absyn_Exp*
_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C8;void*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1CA;
struct Cyc_Absyn_Exp*_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CC;struct Cyc_Absyn_Exp*
_tmp1CD;void*_tmp1CE;struct Cyc_Absyn_Exp*_tmp1CF;void*_tmp1D0;void*_tmp1D1;
struct _tagged_arr*_tmp1D2;void*_tmp1D3;void*_tmp1D4;unsigned int _tmp1D5;struct
Cyc_List_List*_tmp1D6;void*_tmp1D7;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_Absyn_Exp*
_tmp1D9;struct _tagged_arr*_tmp1DA;struct Cyc_Absyn_Exp*_tmp1DB;struct _tagged_arr*
_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DD;struct Cyc_Absyn_Exp*_tmp1DE;struct Cyc_List_List*
_tmp1DF;struct _tuple1*_tmp1E0;struct Cyc_List_List*_tmp1E1;struct Cyc_List_List*
_tmp1E2;struct Cyc_Absyn_Vardecl*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_Absyn_Exp*
_tmp1E5;struct _tuple0*_tmp1E6;struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*
_tmp1E8;struct Cyc_List_List*_tmp1E9;struct Cyc_List_List*_tmp1EA;struct Cyc_Absyn_Tunionfield*
_tmp1EB;struct _tuple0*_tmp1EC;struct _tuple0*_tmp1ED;struct Cyc_Absyn_MallocInfo
_tmp1EE;int _tmp1EF;struct Cyc_Absyn_Exp*_tmp1F0;void**_tmp1F1;struct Cyc_Absyn_Exp*
_tmp1F2;struct Cyc_Core_Opt*_tmp1F3;struct Cyc_List_List*_tmp1F4;struct Cyc_Absyn_Stmt*
_tmp1F5;struct Cyc_Absyn_Fndecl*_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F7;_LL189: if(*((
int*)_tmp1B2)!= 0)goto _LL18B;_tmp1B3=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1B2)->f1;_LL18A: s=Cyc_Absynpp_cnst2doc(_tmp1B3);goto _LL188;_LL18B: if(*((int*)
_tmp1B2)!= 1)goto _LL18D;_tmp1B4=((struct Cyc_Absyn_Var_e_struct*)_tmp1B2)->f1;
_LL18C: _tmp1B5=_tmp1B4;goto _LL18E;_LL18D: if(*((int*)_tmp1B2)!= 2)goto _LL18F;
_tmp1B5=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp1B2)->f1;_LL18E: s=Cyc_Absynpp_qvar2doc(
_tmp1B5);goto _LL188;_LL18F: if(*((int*)_tmp1B2)!= 3)goto _LL191;_tmp1B6=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp1B2)->f1;_tmp1B7=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1B2)->f2;_LL190: s=Cyc_Absynpp_primapp2doc(myprec,_tmp1B6,_tmp1B7);goto _LL188;
_LL191: if(*((int*)_tmp1B2)!= 4)goto _LL193;_tmp1B8=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1B2)->f1;_tmp1B9=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1B2)->f2;_tmp1BA=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp1B2)->f3;_LL192: s=({struct Cyc_PP_Doc*
_tmp1F8[5];_tmp1F8[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1BA);_tmp1F8[3]=Cyc_PP_text(({
const char*_tmp1FB="= ";_tag_arr(_tmp1FB,sizeof(char),_get_zero_arr_size(_tmp1FB,
3));}));_tmp1F8[2]=_tmp1B9 == 0?Cyc_PP_text(({const char*_tmp1FA="";_tag_arr(
_tmp1FA,sizeof(char),_get_zero_arr_size(_tmp1FA,1));})): Cyc_Absynpp_prim2doc((
void*)_tmp1B9->v);_tmp1F8[1]=Cyc_PP_text(({const char*_tmp1F9=" ";_tag_arr(
_tmp1F9,sizeof(char),_get_zero_arr_size(_tmp1F9,2));}));_tmp1F8[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1B8);Cyc_PP_cat(_tag_arr(_tmp1F8,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL188;_LL193: if(*((int*)_tmp1B2)!= 5)goto _LL195;_tmp1BB=((struct Cyc_Absyn_Increment_e_struct*)
_tmp1B2)->f1;_tmp1BC=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1B2)->f2;
_LL194: {struct Cyc_PP_Doc*_tmp1FC=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1BB);{void*
_tmp1FD=_tmp1BC;_LL1D8: if((int)_tmp1FD != 0)goto _LL1DA;_LL1D9: s=({struct Cyc_PP_Doc*
_tmp1FE[2];_tmp1FE[1]=_tmp1FC;_tmp1FE[0]=Cyc_PP_text(({const char*_tmp1FF="++";
_tag_arr(_tmp1FF,sizeof(char),_get_zero_arr_size(_tmp1FF,3));}));Cyc_PP_cat(
_tag_arr(_tmp1FE,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1D7;_LL1DA: if((int)
_tmp1FD != 2)goto _LL1DC;_LL1DB: s=({struct Cyc_PP_Doc*_tmp200[2];_tmp200[1]=_tmp1FC;
_tmp200[0]=Cyc_PP_text(({const char*_tmp201="--";_tag_arr(_tmp201,sizeof(char),
_get_zero_arr_size(_tmp201,3));}));Cyc_PP_cat(_tag_arr(_tmp200,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL1D7;_LL1DC: if((int)_tmp1FD != 1)goto _LL1DE;_LL1DD: s=({struct Cyc_PP_Doc*
_tmp202[2];_tmp202[1]=Cyc_PP_text(({const char*_tmp203="++";_tag_arr(_tmp203,
sizeof(char),_get_zero_arr_size(_tmp203,3));}));_tmp202[0]=_tmp1FC;Cyc_PP_cat(
_tag_arr(_tmp202,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1D7;_LL1DE: if((int)
_tmp1FD != 3)goto _LL1D7;_LL1DF: s=({struct Cyc_PP_Doc*_tmp204[2];_tmp204[1]=Cyc_PP_text(({
const char*_tmp205="--";_tag_arr(_tmp205,sizeof(char),_get_zero_arr_size(_tmp205,
3));}));_tmp204[0]=_tmp1FC;Cyc_PP_cat(_tag_arr(_tmp204,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL1D7;_LL1D7:;}goto _LL188;}_LL195: if(*((int*)_tmp1B2)!= 6)goto _LL197;
_tmp1BD=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1B2)->f1;_tmp1BE=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1B2)->f2;_tmp1BF=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1B2)->f3;_LL196:{
struct _tuple5 _tmp207=({struct _tuple5 _tmp206;_tmp206.f1=(void*)_tmp1BE->r;_tmp206.f2=(
void*)_tmp1BF->r;_tmp206;});_LL1E1:;_LL1E2: s=({struct Cyc_PP_Doc*_tmp208[5];
_tmp208[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1BF);_tmp208[3]=Cyc_PP_text(({
const char*_tmp20A=" : ";_tag_arr(_tmp20A,sizeof(char),_get_zero_arr_size(_tmp20A,
4));}));_tmp208[2]=Cyc_Absynpp_exp2doc_prec(0,_tmp1BE);_tmp208[1]=Cyc_PP_text(({
const char*_tmp209=" ? ";_tag_arr(_tmp209,sizeof(char),_get_zero_arr_size(_tmp209,
4));}));_tmp208[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1BD);Cyc_PP_cat(_tag_arr(
_tmp208,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1E0;_LL1E0:;}goto _LL188;_LL197:
if(*((int*)_tmp1B2)!= 7)goto _LL199;_tmp1C0=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1B2)->f1;_tmp1C1=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1B2)->f2;_LL198: s=({
struct Cyc_PP_Doc*_tmp20B[5];_tmp20B[4]=Cyc_PP_text(({const char*_tmp20E=")";
_tag_arr(_tmp20E,sizeof(char),_get_zero_arr_size(_tmp20E,2));}));_tmp20B[3]=Cyc_Absynpp_exp2doc(
_tmp1C1);_tmp20B[2]=Cyc_PP_text(({const char*_tmp20D=", ";_tag_arr(_tmp20D,
sizeof(char),_get_zero_arr_size(_tmp20D,3));}));_tmp20B[1]=Cyc_Absynpp_exp2doc(
_tmp1C0);_tmp20B[0]=Cyc_PP_text(({const char*_tmp20C="(";_tag_arr(_tmp20C,sizeof(
char),_get_zero_arr_size(_tmp20C,2));}));Cyc_PP_cat(_tag_arr(_tmp20B,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL188;_LL199: if(*((int*)_tmp1B2)!= 8)goto _LL19B;
_tmp1C2=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1B2)->f1;_tmp1C3=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp1B2)->f2;_LL19A: s=({struct Cyc_PP_Doc*_tmp20F[4];_tmp20F[3]=Cyc_PP_text(({
const char*_tmp211=")";_tag_arr(_tmp211,sizeof(char),_get_zero_arr_size(_tmp211,2));}));
_tmp20F[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp1C3);_tmp20F[1]=Cyc_PP_text(({const
char*_tmp210="(";_tag_arr(_tmp210,sizeof(char),_get_zero_arr_size(_tmp210,2));}));
_tmp20F[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1C2);Cyc_PP_cat(_tag_arr(_tmp20F,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL188;_LL19B: if(*((int*)_tmp1B2)!= 9)goto
_LL19D;_tmp1C4=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1B2)->f1;_tmp1C5=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp1B2)->f2;_LL19C: s=({struct Cyc_PP_Doc*_tmp212[4];
_tmp212[3]=Cyc_PP_text(({const char*_tmp214=")";_tag_arr(_tmp214,sizeof(char),
_get_zero_arr_size(_tmp214,2));}));_tmp212[2]=Cyc_Absynpp_exps2doc_prec(20,
_tmp1C5);_tmp212[1]=Cyc_PP_text(({const char*_tmp213="(";_tag_arr(_tmp213,sizeof(
char),_get_zero_arr_size(_tmp213,2));}));_tmp212[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1C4);Cyc_PP_cat(_tag_arr(_tmp212,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL188;_LL19D: if(*((int*)_tmp1B2)!= 10)goto _LL19F;_tmp1C6=((struct Cyc_Absyn_Throw_e_struct*)
_tmp1B2)->f1;_LL19E: s=({struct Cyc_PP_Doc*_tmp215[2];_tmp215[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1C6);_tmp215[0]=Cyc_PP_text(({const char*_tmp216="throw ";_tag_arr(
_tmp216,sizeof(char),_get_zero_arr_size(_tmp216,7));}));Cyc_PP_cat(_tag_arr(
_tmp215,sizeof(struct Cyc_PP_Doc*),2));});goto _LL188;_LL19F: if(*((int*)_tmp1B2)!= 
11)goto _LL1A1;_tmp1C7=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1B2)->f1;
_LL1A0: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1C7);goto _LL188;_LL1A1: if(*((int*)
_tmp1B2)!= 12)goto _LL1A3;_tmp1C8=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1B2)->f1;
_LL1A2: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1C8);goto _LL188;_LL1A3: if(*((int*)
_tmp1B2)!= 13)goto _LL1A5;_tmp1C9=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1B2)->f1;
_tmp1CA=((struct Cyc_Absyn_Cast_e_struct*)_tmp1B2)->f2;_LL1A4: s=({struct Cyc_PP_Doc*
_tmp217[4];_tmp217[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1CA);_tmp217[2]=Cyc_PP_text(({
const char*_tmp219=")";_tag_arr(_tmp219,sizeof(char),_get_zero_arr_size(_tmp219,2));}));
_tmp217[1]=Cyc_Absynpp_typ2doc(_tmp1C9);_tmp217[0]=Cyc_PP_text(({const char*
_tmp218="(";_tag_arr(_tmp218,sizeof(char),_get_zero_arr_size(_tmp218,2));}));Cyc_PP_cat(
_tag_arr(_tmp217,sizeof(struct Cyc_PP_Doc*),4));});goto _LL188;_LL1A5: if(*((int*)
_tmp1B2)!= 14)goto _LL1A7;_tmp1CB=((struct Cyc_Absyn_Address_e_struct*)_tmp1B2)->f1;
_LL1A6: s=({struct Cyc_PP_Doc*_tmp21A[2];_tmp21A[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1CB);_tmp21A[0]=Cyc_PP_text(({const char*_tmp21B="&";_tag_arr(_tmp21B,
sizeof(char),_get_zero_arr_size(_tmp21B,2));}));Cyc_PP_cat(_tag_arr(_tmp21A,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL188;_LL1A7: if(*((int*)_tmp1B2)!= 15)goto
_LL1A9;_tmp1CC=((struct Cyc_Absyn_New_e_struct*)_tmp1B2)->f1;_tmp1CD=((struct Cyc_Absyn_New_e_struct*)
_tmp1B2)->f2;_LL1A8: if(_tmp1CC == 0)s=({struct Cyc_PP_Doc*_tmp21C[2];_tmp21C[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1CD);_tmp21C[0]=Cyc_PP_text(({const char*
_tmp21D="new ";_tag_arr(_tmp21D,sizeof(char),_get_zero_arr_size(_tmp21D,5));}));
Cyc_PP_cat(_tag_arr(_tmp21C,sizeof(struct Cyc_PP_Doc*),2));});else{s=({struct Cyc_PP_Doc*
_tmp21E[4];_tmp21E[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1CD);_tmp21E[2]=Cyc_PP_text(({
const char*_tmp220=") ";_tag_arr(_tmp220,sizeof(char),_get_zero_arr_size(_tmp220,
3));}));_tmp21E[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(_tmp1CC));
_tmp21E[0]=Cyc_PP_text(({const char*_tmp21F="rnew(";_tag_arr(_tmp21F,sizeof(char),
_get_zero_arr_size(_tmp21F,6));}));Cyc_PP_cat(_tag_arr(_tmp21E,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL188;_LL1A9: if(*((int*)_tmp1B2)!= 16)goto _LL1AB;_tmp1CE=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp1B2)->f1;_LL1AA: s=({struct Cyc_PP_Doc*
_tmp221[3];_tmp221[2]=Cyc_PP_text(({const char*_tmp223=")";_tag_arr(_tmp223,
sizeof(char),_get_zero_arr_size(_tmp223,2));}));_tmp221[1]=Cyc_Absynpp_typ2doc(
_tmp1CE);_tmp221[0]=Cyc_PP_text(({const char*_tmp222="sizeof(";_tag_arr(_tmp222,
sizeof(char),_get_zero_arr_size(_tmp222,8));}));Cyc_PP_cat(_tag_arr(_tmp221,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL188;_LL1AB: if(*((int*)_tmp1B2)!= 17)goto
_LL1AD;_tmp1CF=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1B2)->f1;_LL1AC: s=({
struct Cyc_PP_Doc*_tmp224[3];_tmp224[2]=Cyc_PP_text(({const char*_tmp226=")";
_tag_arr(_tmp226,sizeof(char),_get_zero_arr_size(_tmp226,2));}));_tmp224[1]=Cyc_Absynpp_exp2doc(
_tmp1CF);_tmp224[0]=Cyc_PP_text(({const char*_tmp225="sizeof(";_tag_arr(_tmp225,
sizeof(char),_get_zero_arr_size(_tmp225,8));}));Cyc_PP_cat(_tag_arr(_tmp224,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL188;_LL1AD: if(*((int*)_tmp1B2)!= 18)goto
_LL1AF;_tmp1D0=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1B2)->f1;_tmp1D1=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1B2)->f2;if(*((int*)_tmp1D1)!= 0)
goto _LL1AF;_tmp1D2=((struct Cyc_Absyn_StructField_struct*)_tmp1D1)->f1;_LL1AE: s=({
struct Cyc_PP_Doc*_tmp227[5];_tmp227[4]=Cyc_PP_text(({const char*_tmp22A=")";
_tag_arr(_tmp22A,sizeof(char),_get_zero_arr_size(_tmp22A,2));}));_tmp227[3]=Cyc_PP_textptr(
_tmp1D2);_tmp227[2]=Cyc_PP_text(({const char*_tmp229=",";_tag_arr(_tmp229,sizeof(
char),_get_zero_arr_size(_tmp229,2));}));_tmp227[1]=Cyc_Absynpp_typ2doc(_tmp1D0);
_tmp227[0]=Cyc_PP_text(({const char*_tmp228="offsetof(";_tag_arr(_tmp228,sizeof(
char),_get_zero_arr_size(_tmp228,10));}));Cyc_PP_cat(_tag_arr(_tmp227,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL188;_LL1AF: if(*((int*)_tmp1B2)!= 18)goto _LL1B1;
_tmp1D3=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1B2)->f1;_tmp1D4=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp1B2)->f2;if(*((int*)_tmp1D4)!= 1)goto
_LL1B1;_tmp1D5=((struct Cyc_Absyn_TupleIndex_struct*)_tmp1D4)->f1;_LL1B0: s=({
struct Cyc_PP_Doc*_tmp22B[5];_tmp22B[4]=Cyc_PP_text(({const char*_tmp231=")";
_tag_arr(_tmp231,sizeof(char),_get_zero_arr_size(_tmp231,2));}));_tmp22B[3]=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp230;_tmp230.tag=1;_tmp230.f1=(
int)_tmp1D5;{void*_tmp22E[1]={& _tmp230};Cyc_Std_aprintf(({const char*_tmp22F="%d";
_tag_arr(_tmp22F,sizeof(char),_get_zero_arr_size(_tmp22F,3));}),_tag_arr(_tmp22E,
sizeof(void*),1));}}));_tmp22B[2]=Cyc_PP_text(({const char*_tmp22D=",";_tag_arr(
_tmp22D,sizeof(char),_get_zero_arr_size(_tmp22D,2));}));_tmp22B[1]=Cyc_Absynpp_typ2doc(
_tmp1D3);_tmp22B[0]=Cyc_PP_text(({const char*_tmp22C="offsetof(";_tag_arr(_tmp22C,
sizeof(char),_get_zero_arr_size(_tmp22C,10));}));Cyc_PP_cat(_tag_arr(_tmp22B,
sizeof(struct Cyc_PP_Doc*),5));});goto _LL188;_LL1B1: if(*((int*)_tmp1B2)!= 19)goto
_LL1B3;_tmp1D6=((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1B2)->f1;_tmp1D7=(void*)((
struct Cyc_Absyn_Gentyp_e_struct*)_tmp1B2)->f2;_LL1B2: s=({struct Cyc_PP_Doc*
_tmp232[4];_tmp232[3]=Cyc_PP_text(({const char*_tmp234=")";_tag_arr(_tmp234,
sizeof(char),_get_zero_arr_size(_tmp234,2));}));_tmp232[2]=Cyc_Absynpp_typ2doc(
_tmp1D7);_tmp232[1]=Cyc_Absynpp_tvars2doc(_tmp1D6);_tmp232[0]=Cyc_PP_text(({
const char*_tmp233="__gen(";_tag_arr(_tmp233,sizeof(char),_get_zero_arr_size(
_tmp233,7));}));Cyc_PP_cat(_tag_arr(_tmp232,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL188;_LL1B3: if(*((int*)_tmp1B2)!= 20)goto _LL1B5;_tmp1D8=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1B2)->f1;_LL1B4: s=({struct Cyc_PP_Doc*_tmp235[2];_tmp235[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1D8);_tmp235[0]=Cyc_PP_text(({const char*_tmp236="*";_tag_arr(_tmp236,
sizeof(char),_get_zero_arr_size(_tmp236,2));}));Cyc_PP_cat(_tag_arr(_tmp235,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL188;_LL1B5: if(*((int*)_tmp1B2)!= 21)goto
_LL1B7;_tmp1D9=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1B2)->f1;_tmp1DA=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp1B2)->f2;_LL1B6: s=({struct Cyc_PP_Doc*
_tmp237[3];_tmp237[2]=Cyc_PP_textptr(_tmp1DA);_tmp237[1]=Cyc_PP_text(({const char*
_tmp238=".";_tag_arr(_tmp238,sizeof(char),_get_zero_arr_size(_tmp238,2));}));
_tmp237[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1D9);Cyc_PP_cat(_tag_arr(_tmp237,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL188;_LL1B7: if(*((int*)_tmp1B2)!= 22)goto
_LL1B9;_tmp1DB=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1B2)->f1;_tmp1DC=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1B2)->f2;_LL1B8: s=({struct Cyc_PP_Doc*
_tmp239[3];_tmp239[2]=Cyc_PP_textptr(_tmp1DC);_tmp239[1]=Cyc_PP_text(({const char*
_tmp23A="->";_tag_arr(_tmp23A,sizeof(char),_get_zero_arr_size(_tmp23A,3));}));
_tmp239[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DB);Cyc_PP_cat(_tag_arr(_tmp239,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL188;_LL1B9: if(*((int*)_tmp1B2)!= 23)goto
_LL1BB;_tmp1DD=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1B2)->f1;_tmp1DE=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp1B2)->f2;_LL1BA: s=({struct Cyc_PP_Doc*
_tmp23B[4];_tmp23B[3]=Cyc_PP_text(({const char*_tmp23D="]";_tag_arr(_tmp23D,
sizeof(char),_get_zero_arr_size(_tmp23D,2));}));_tmp23B[2]=Cyc_Absynpp_exp2doc(
_tmp1DE);_tmp23B[1]=Cyc_PP_text(({const char*_tmp23C="[";_tag_arr(_tmp23C,sizeof(
char),_get_zero_arr_size(_tmp23C,2));}));_tmp23B[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1DD);Cyc_PP_cat(_tag_arr(_tmp23B,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL188;_LL1BB: if(*((int*)_tmp1B2)!= 24)goto _LL1BD;_tmp1DF=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp1B2)->f1;_LL1BC: s=({struct Cyc_PP_Doc*_tmp23E[4];_tmp23E[3]=Cyc_PP_text(({
const char*_tmp240=")";_tag_arr(_tmp240,sizeof(char),_get_zero_arr_size(_tmp240,2));}));
_tmp23E[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp1DF);_tmp23E[1]=Cyc_PP_text(({const
char*_tmp23F="(";_tag_arr(_tmp23F,sizeof(char),_get_zero_arr_size(_tmp23F,2));}));
_tmp23E[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_arr(_tmp23E,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL188;_LL1BD: if(*((int*)_tmp1B2)!= 25)goto _LL1BF;_tmp1E0=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp1B2)->f1;_tmp1E1=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp1B2)->f2;_LL1BE:
s=({struct Cyc_PP_Doc*_tmp241[4];_tmp241[3]=Cyc_Absynpp_group_braces(({const char*
_tmp244=",";_tag_arr(_tmp244,sizeof(char),_get_zero_arr_size(_tmp244,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp1E1));_tmp241[2]=Cyc_PP_text(({const char*
_tmp243=")";_tag_arr(_tmp243,sizeof(char),_get_zero_arr_size(_tmp243,2));}));
_tmp241[1]=Cyc_Absynpp_typ2doc((*((struct _tuple1*)_tmp1E0)).f3);_tmp241[0]=Cyc_PP_text(({
const char*_tmp242="(";_tag_arr(_tmp242,sizeof(char),_get_zero_arr_size(_tmp242,2));}));
Cyc_PP_cat(_tag_arr(_tmp241,sizeof(struct Cyc_PP_Doc*),4));});goto _LL188;_LL1BF:
if(*((int*)_tmp1B2)!= 26)goto _LL1C1;_tmp1E2=((struct Cyc_Absyn_Array_e_struct*)
_tmp1B2)->f1;_LL1C0: s=Cyc_Absynpp_group_braces(({const char*_tmp245=",";_tag_arr(
_tmp245,sizeof(char),_get_zero_arr_size(_tmp245,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp1E2));goto _LL188;_LL1C1: if(*((int*)_tmp1B2)!= 27)goto _LL1C3;_tmp1E3=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1B2)->f1;_tmp1E4=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1B2)->f2;_tmp1E5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1B2)->f3;
_LL1C2: s=({struct Cyc_PP_Doc*_tmp246[8];_tmp246[7]=Cyc_Absynpp_rb();_tmp246[6]=
Cyc_Absynpp_exp2doc(_tmp1E5);_tmp246[5]=Cyc_PP_text(({const char*_tmp249=" : ";
_tag_arr(_tmp249,sizeof(char),_get_zero_arr_size(_tmp249,4));}));_tmp246[4]=Cyc_Absynpp_exp2doc(
_tmp1E4);_tmp246[3]=Cyc_PP_text(({const char*_tmp248=" < ";_tag_arr(_tmp248,
sizeof(char),_get_zero_arr_size(_tmp248,4));}));_tmp246[2]=Cyc_PP_text(*((struct
_tagged_arr*)(*((struct _tuple0*)_tmp1E3->name)).f2));_tmp246[1]=Cyc_PP_text(({
const char*_tmp247="for ";_tag_arr(_tmp247,sizeof(char),_get_zero_arr_size(
_tmp247,5));}));_tmp246[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_arr(_tmp246,sizeof(
struct Cyc_PP_Doc*),8));});goto _LL188;_LL1C3: if(*((int*)_tmp1B2)!= 28)goto _LL1C5;
_tmp1E6=((struct Cyc_Absyn_Struct_e_struct*)_tmp1B2)->f1;_tmp1E7=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1B2)->f2;_tmp1E8=((struct Cyc_Absyn_Struct_e_struct*)_tmp1B2)->f3;_LL1C4: {
struct Cyc_List_List*_tmp24A=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp1E8);
s=({struct Cyc_PP_Doc*_tmp24B[2];_tmp24B[1]=Cyc_Absynpp_group_braces(({const char*
_tmp24C=",";_tag_arr(_tmp24C,sizeof(char),_get_zero_arr_size(_tmp24C,2));}),
_tmp1E7 != 0?({struct Cyc_List_List*_tmp24D=_cycalloc(sizeof(*_tmp24D));_tmp24D->hd=
Cyc_Absynpp_tps2doc(_tmp1E7);_tmp24D->tl=_tmp24A;_tmp24D;}): _tmp24A);_tmp24B[0]=
Cyc_Absynpp_qvar2doc(_tmp1E6);Cyc_PP_cat(_tag_arr(_tmp24B,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL188;}_LL1C5: if(*((int*)_tmp1B2)!= 29)goto _LL1C7;_tmp1E9=((struct
Cyc_Absyn_AnonStruct_e_struct*)_tmp1B2)->f2;_LL1C6: s=Cyc_Absynpp_group_braces(({
const char*_tmp24E=",";_tag_arr(_tmp24E,sizeof(char),_get_zero_arr_size(_tmp24E,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp1E9));goto _LL188;_LL1C7: if(*((int*)
_tmp1B2)!= 30)goto _LL1C9;_tmp1EA=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1B2)->f1;
_tmp1EB=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1B2)->f3;_LL1C8: if(_tmp1EA == 0)s=
Cyc_Absynpp_qvar2doc(_tmp1EB->name);else{s=({struct Cyc_PP_Doc*_tmp24F[2];_tmp24F[
1]=Cyc_PP_egroup(({const char*_tmp250="(";_tag_arr(_tmp250,sizeof(char),
_get_zero_arr_size(_tmp250,2));}),({const char*_tmp251=")";_tag_arr(_tmp251,
sizeof(char),_get_zero_arr_size(_tmp251,2));}),({const char*_tmp252=",";_tag_arr(
_tmp252,sizeof(char),_get_zero_arr_size(_tmp252,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_exp2doc,_tmp1EA));_tmp24F[0]=Cyc_Absynpp_qvar2doc(_tmp1EB->name);Cyc_PP_cat(
_tag_arr(_tmp24F,sizeof(struct Cyc_PP_Doc*),2));});}goto _LL188;_LL1C9: if(*((int*)
_tmp1B2)!= 31)goto _LL1CB;_tmp1EC=((struct Cyc_Absyn_Enum_e_struct*)_tmp1B2)->f1;
_LL1CA: s=Cyc_Absynpp_qvar2doc(_tmp1EC);goto _LL188;_LL1CB: if(*((int*)_tmp1B2)!= 
32)goto _LL1CD;_tmp1ED=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp1B2)->f1;_LL1CC: s=
Cyc_Absynpp_qvar2doc(_tmp1ED);goto _LL188;_LL1CD: if(*((int*)_tmp1B2)!= 33)goto
_LL1CF;_tmp1EE=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1B2)->f1;_tmp1EF=_tmp1EE.is_calloc;
_tmp1F0=_tmp1EE.rgn;_tmp1F1=_tmp1EE.elt_type;_tmp1F2=_tmp1EE.num_elts;_LL1CE: if(
_tmp1EF){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp1F1)),0);if(_tmp1F0 == 0)s=({struct Cyc_PP_Doc*_tmp253[5];_tmp253[4]=Cyc_PP_text(({
const char*_tmp256=")";_tag_arr(_tmp256,sizeof(char),_get_zero_arr_size(_tmp256,2));}));
_tmp253[3]=Cyc_Absynpp_exp2doc(st);_tmp253[2]=Cyc_PP_text(({const char*_tmp255=",";
_tag_arr(_tmp255,sizeof(char),_get_zero_arr_size(_tmp255,2));}));_tmp253[1]=Cyc_Absynpp_exp2doc(
_tmp1F2);_tmp253[0]=Cyc_PP_text(({const char*_tmp254="calloc(";_tag_arr(_tmp254,
sizeof(char),_get_zero_arr_size(_tmp254,8));}));Cyc_PP_cat(_tag_arr(_tmp253,
sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp257[7];_tmp257[6]=
Cyc_PP_text(({const char*_tmp25B=")";_tag_arr(_tmp25B,sizeof(char),
_get_zero_arr_size(_tmp25B,2));}));_tmp257[5]=Cyc_Absynpp_exp2doc(st);_tmp257[4]=
Cyc_PP_text(({const char*_tmp25A=",";_tag_arr(_tmp25A,sizeof(char),
_get_zero_arr_size(_tmp25A,2));}));_tmp257[3]=Cyc_Absynpp_exp2doc(_tmp1F2);
_tmp257[2]=Cyc_PP_text(({const char*_tmp259=",";_tag_arr(_tmp259,sizeof(char),
_get_zero_arr_size(_tmp259,2));}));_tmp257[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmp1F0));_tmp257[0]=Cyc_PP_text(({const char*_tmp258="rcalloc(";
_tag_arr(_tmp258,sizeof(char),_get_zero_arr_size(_tmp258,9));}));Cyc_PP_cat(
_tag_arr(_tmp257,sizeof(struct Cyc_PP_Doc*),7));});}}else{struct Cyc_Absyn_Exp*
new_e;if(_tmp1F1 == 0)new_e=_tmp1F2;else{new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*((
void**)_tmp1F1),0),_tmp1F2,0);}if(_tmp1F0 == 0)s=({struct Cyc_PP_Doc*_tmp25C[3];
_tmp25C[2]=Cyc_PP_text(({const char*_tmp25E=")";_tag_arr(_tmp25E,sizeof(char),
_get_zero_arr_size(_tmp25E,2));}));_tmp25C[1]=Cyc_Absynpp_exp2doc(new_e);_tmp25C[
0]=Cyc_PP_text(({const char*_tmp25D="malloc(";_tag_arr(_tmp25D,sizeof(char),
_get_zero_arr_size(_tmp25D,8));}));Cyc_PP_cat(_tag_arr(_tmp25C,sizeof(struct Cyc_PP_Doc*),
3));});else{s=({struct Cyc_PP_Doc*_tmp25F[5];_tmp25F[4]=Cyc_PP_text(({const char*
_tmp262=")";_tag_arr(_tmp262,sizeof(char),_get_zero_arr_size(_tmp262,2));}));
_tmp25F[3]=Cyc_Absynpp_exp2doc(new_e);_tmp25F[2]=Cyc_PP_text(({const char*_tmp261=",";
_tag_arr(_tmp261,sizeof(char),_get_zero_arr_size(_tmp261,2));}));_tmp25F[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(_tmp1F0));_tmp25F[0]=Cyc_PP_text(({const char*
_tmp260="rmalloc(";_tag_arr(_tmp260,sizeof(char),_get_zero_arr_size(_tmp260,9));}));
Cyc_PP_cat(_tag_arr(_tmp25F,sizeof(struct Cyc_PP_Doc*),5));});}}goto _LL188;_LL1CF:
if(*((int*)_tmp1B2)!= 34)goto _LL1D1;_tmp1F3=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp1B2)->f1;_tmp1F4=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1B2)->f2;
_LL1D0: s=Cyc_Absynpp_group_braces(({const char*_tmp263=",";_tag_arr(_tmp263,
sizeof(char),_get_zero_arr_size(_tmp263,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp1F4));goto _LL188;_LL1D1: if(*((int*)_tmp1B2)!= 35)goto _LL1D3;_tmp1F5=((struct
Cyc_Absyn_StmtExp_e_struct*)_tmp1B2)->f1;_LL1D2: s=({struct Cyc_PP_Doc*_tmp264[7];
_tmp264[6]=Cyc_PP_text(({const char*_tmp266=")";_tag_arr(_tmp266,sizeof(char),
_get_zero_arr_size(_tmp266,2));}));_tmp264[5]=Cyc_Absynpp_rb();_tmp264[4]=Cyc_PP_blank_doc();
_tmp264[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp1F5));_tmp264[2]=Cyc_PP_blank_doc();
_tmp264[1]=Cyc_Absynpp_lb();_tmp264[0]=Cyc_PP_text(({const char*_tmp265="(";
_tag_arr(_tmp265,sizeof(char),_get_zero_arr_size(_tmp265,2));}));Cyc_PP_cat(
_tag_arr(_tmp264,sizeof(struct Cyc_PP_Doc*),7));});goto _LL188;_LL1D3: if(*((int*)
_tmp1B2)!= 36)goto _LL1D5;_tmp1F6=((struct Cyc_Absyn_Codegen_e_struct*)_tmp1B2)->f1;
_LL1D4: s=({struct Cyc_PP_Doc*_tmp267[3];_tmp267[2]=Cyc_PP_text(({const char*
_tmp26C=")";_tag_arr(_tmp26C,sizeof(char),_get_zero_arr_size(_tmp26C,2));}));
_tmp267[1]=Cyc_PP_nest(2,Cyc_Absynpp_decl2doc(({struct Cyc_Absyn_Decl*_tmp269=
_cycalloc(sizeof(*_tmp269));_tmp269->r=(void*)((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp26B;_tmp26B.tag=1;_tmp26B.f1=_tmp1F6;_tmp26B;});_tmp26A;}));_tmp269->loc=e->loc;
_tmp269;})));_tmp267[0]=Cyc_PP_text(({const char*_tmp268="codegen(";_tag_arr(
_tmp268,sizeof(char),_get_zero_arr_size(_tmp268,9));}));Cyc_PP_cat(_tag_arr(
_tmp267,sizeof(struct Cyc_PP_Doc*),3));});goto _LL188;_LL1D5: if(*((int*)_tmp1B2)!= 
37)goto _LL188;_tmp1F7=((struct Cyc_Absyn_Fill_e_struct*)_tmp1B2)->f1;_LL1D6: s=({
struct Cyc_PP_Doc*_tmp26D[3];_tmp26D[2]=Cyc_PP_text(({const char*_tmp26F=")";
_tag_arr(_tmp26F,sizeof(char),_get_zero_arr_size(_tmp26F,2));}));_tmp26D[1]=Cyc_PP_nest(
2,Cyc_Absynpp_exp2doc(_tmp1F7));_tmp26D[0]=Cyc_PP_text(({const char*_tmp26E="fill(";
_tag_arr(_tmp26E,sizeof(char),_get_zero_arr_size(_tmp26E,6));}));Cyc_PP_cat(
_tag_arr(_tmp26D,sizeof(struct Cyc_PP_Doc*),3));});goto _LL188;_LL188:;}if(inprec
>= myprec)s=({struct Cyc_PP_Doc*_tmp270[3];_tmp270[2]=Cyc_PP_text(({const char*
_tmp272=")";_tag_arr(_tmp272,sizeof(char),_get_zero_arr_size(_tmp272,2));}));
_tmp270[1]=s;_tmp270[0]=Cyc_PP_text(({const char*_tmp271="(";_tag_arr(_tmp271,
sizeof(char),_get_zero_arr_size(_tmp271,2));}));Cyc_PP_cat(_tag_arr(_tmp270,
sizeof(struct Cyc_PP_Doc*),3));});return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d){void*_tmp273=d;struct Cyc_Absyn_Exp*_tmp274;struct _tagged_arr*_tmp275;
_LL1E4: if(*((int*)_tmp273)!= 0)goto _LL1E6;_tmp274=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp273)->f1;_LL1E5: return({struct Cyc_PP_Doc*_tmp276[3];_tmp276[2]=Cyc_PP_text(({
const char*_tmp278="]";_tag_arr(_tmp278,sizeof(char),_get_zero_arr_size(_tmp278,2));}));
_tmp276[1]=Cyc_Absynpp_exp2doc(_tmp274);_tmp276[0]=Cyc_PP_text(({const char*
_tmp277=".[";_tag_arr(_tmp277,sizeof(char),_get_zero_arr_size(_tmp277,3));}));
Cyc_PP_cat(_tag_arr(_tmp276,sizeof(struct Cyc_PP_Doc*),3));});_LL1E6: if(*((int*)
_tmp273)!= 1)goto _LL1E3;_tmp275=((struct Cyc_Absyn_FieldName_struct*)_tmp273)->f1;
_LL1E7: return({struct Cyc_PP_Doc*_tmp279[2];_tmp279[1]=Cyc_PP_textptr(_tmp275);
_tmp279[0]=Cyc_PP_text(({const char*_tmp27A=".";_tag_arr(_tmp27A,sizeof(char),
_get_zero_arr_size(_tmp27A,2));}));Cyc_PP_cat(_tag_arr(_tmp279,sizeof(struct Cyc_PP_Doc*),
2));});_LL1E3:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de){if((*((
struct _tuple9*)de)).f1 == 0)return Cyc_Absynpp_exp2doc((*((struct _tuple9*)de)).f2);
else{return({struct Cyc_PP_Doc*_tmp27B[2];_tmp27B[1]=Cyc_Absynpp_exp2doc((*((
struct _tuple9*)de)).f2);_tmp27B[0]=Cyc_PP_egroup(({const char*_tmp27C="";_tag_arr(
_tmp27C,sizeof(char),_get_zero_arr_size(_tmp27C,1));}),({const char*_tmp27D="=";
_tag_arr(_tmp27D,sizeof(char),_get_zero_arr_size(_tmp27D,2));}),({const char*
_tmp27E="=";_tag_arr(_tmp27E,sizeof(char),_get_zero_arr_size(_tmp27E,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*((struct _tuple9*)de)).f1));Cyc_PP_cat(_tag_arr(
_tmp27B,sizeof(struct Cyc_PP_Doc*),2));});}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(
int inprec,struct Cyc_List_List*es){return Cyc_PP_group(({const char*_tmp27F="";
_tag_arr(_tmp27F,sizeof(char),_get_zero_arr_size(_tmp27F,1));}),({const char*
_tmp280="";_tag_arr(_tmp280,sizeof(char),_get_zero_arr_size(_tmp280,1));}),({
const char*_tmp281=",";_tag_arr(_tmp281,sizeof(char),_get_zero_arr_size(_tmp281,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*c){void*_tmp282=c;void*_tmp283;char
_tmp284;void*_tmp285;short _tmp286;void*_tmp287;int _tmp288;void*_tmp289;int
_tmp28A;void*_tmp28B;int _tmp28C;void*_tmp28D;long long _tmp28E;struct _tagged_arr
_tmp28F;struct _tagged_arr _tmp290;_LL1E9: if(_tmp282 <= (void*)1?1:*((int*)_tmp282)
!= 0)goto _LL1EB;_tmp283=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp282)->f1;
_tmp284=((struct Cyc_Absyn_Char_c_struct*)_tmp282)->f2;_LL1EA: return Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp293;_tmp293.tag=0;_tmp293.f1=(
struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp284);{void*_tmp291[1]={& _tmp293};
Cyc_Std_aprintf(({const char*_tmp292="'%s'";_tag_arr(_tmp292,sizeof(char),
_get_zero_arr_size(_tmp292,5));}),_tag_arr(_tmp291,sizeof(void*),1));}}));_LL1EB:
if(_tmp282 <= (void*)1?1:*((int*)_tmp282)!= 1)goto _LL1ED;_tmp285=(void*)((struct
Cyc_Absyn_Short_c_struct*)_tmp282)->f1;_tmp286=((struct Cyc_Absyn_Short_c_struct*)
_tmp282)->f2;_LL1EC: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp296;_tmp296.tag=1;_tmp296.f1=(int)((unsigned int)((int)_tmp286));{void*
_tmp294[1]={& _tmp296};Cyc_Std_aprintf(({const char*_tmp295="%d";_tag_arr(_tmp295,
sizeof(char),_get_zero_arr_size(_tmp295,3));}),_tag_arr(_tmp294,sizeof(void*),1));}}));
_LL1ED: if(_tmp282 <= (void*)1?1:*((int*)_tmp282)!= 2)goto _LL1EF;_tmp287=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp282)->f1;if((int)_tmp287 != 2)goto _LL1EF;
_tmp288=((struct Cyc_Absyn_Int_c_struct*)_tmp282)->f2;_LL1EE: _tmp28A=_tmp288;goto
_LL1F0;_LL1EF: if(_tmp282 <= (void*)1?1:*((int*)_tmp282)!= 2)goto _LL1F1;_tmp289=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp282)->f1;if((int)_tmp289 != 0)goto
_LL1F1;_tmp28A=((struct Cyc_Absyn_Int_c_struct*)_tmp282)->f2;_LL1F0: return Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp299;_tmp299.tag=1;_tmp299.f1=(
int)((unsigned int)_tmp28A);{void*_tmp297[1]={& _tmp299};Cyc_Std_aprintf(({const
char*_tmp298="%d";_tag_arr(_tmp298,sizeof(char),_get_zero_arr_size(_tmp298,3));}),
_tag_arr(_tmp297,sizeof(void*),1));}}));_LL1F1: if(_tmp282 <= (void*)1?1:*((int*)
_tmp282)!= 2)goto _LL1F3;_tmp28B=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp282)->f1;
if((int)_tmp28B != 1)goto _LL1F3;_tmp28C=((struct Cyc_Absyn_Int_c_struct*)_tmp282)->f2;
_LL1F2: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp29C;
_tmp29C.tag=1;_tmp29C.f1=(unsigned int)_tmp28C;{void*_tmp29A[1]={& _tmp29C};Cyc_Std_aprintf(({
const char*_tmp29B="%u";_tag_arr(_tmp29B,sizeof(char),_get_zero_arr_size(_tmp29B,
3));}),_tag_arr(_tmp29A,sizeof(void*),1));}}));_LL1F3: if(_tmp282 <= (void*)1?1:*((
int*)_tmp282)!= 3)goto _LL1F5;_tmp28D=(void*)((struct Cyc_Absyn_LongLong_c_struct*)
_tmp282)->f1;_tmp28E=((struct Cyc_Absyn_LongLong_c_struct*)_tmp282)->f2;_LL1F4:
return Cyc_PP_text(({const char*_tmp29D="<<FIX LONG LONG CONSTANT>>";_tag_arr(
_tmp29D,sizeof(char),_get_zero_arr_size(_tmp29D,27));}));_LL1F5: if(_tmp282 <= (
void*)1?1:*((int*)_tmp282)!= 4)goto _LL1F7;_tmp28F=((struct Cyc_Absyn_Float_c_struct*)
_tmp282)->f1;_LL1F6: return Cyc_PP_text(_tmp28F);_LL1F7: if((int)_tmp282 != 0)goto
_LL1F9;_LL1F8: return Cyc_PP_text(({const char*_tmp29E="NULL";_tag_arr(_tmp29E,
sizeof(char),_get_zero_arr_size(_tmp29E,5));}));_LL1F9: if(_tmp282 <= (void*)1?1:*((
int*)_tmp282)!= 5)goto _LL1E8;_tmp290=((struct Cyc_Absyn_String_c_struct*)_tmp282)->f1;
_LL1FA: return({struct Cyc_PP_Doc*_tmp29F[3];_tmp29F[2]=Cyc_PP_text(({const char*
_tmp2A1="\"";_tag_arr(_tmp2A1,sizeof(char),_get_zero_arr_size(_tmp2A1,2));}));
_tmp29F[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp290));_tmp29F[0]=Cyc_PP_text(({
const char*_tmp2A0="\"";_tag_arr(_tmp2A0,sizeof(char),_get_zero_arr_size(_tmp2A0,
2));}));Cyc_PP_cat(_tag_arr(_tmp29F,sizeof(struct Cyc_PP_Doc*),3));});_LL1E8:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (void*)19){if(es == 0?1: es->tl
!= 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2A2=_cycalloc(
sizeof(*_tmp2A2));_tmp2A2[0]=({struct Cyc_Core_Failure_struct _tmp2A3;_tmp2A3.tag=
Cyc_Core_Failure;_tmp2A3.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp2A6;_tmp2A6.tag=0;_tmp2A6.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp2A4[1]={& _tmp2A6};Cyc_Std_aprintf(({const char*_tmp2A5="Absynpp::primapp2doc Size: %s with bad args";
_tag_arr(_tmp2A5,sizeof(char),_get_zero_arr_size(_tmp2A5,44));}),_tag_arr(
_tmp2A4,sizeof(void*),1));}});_tmp2A3;});_tmp2A2;}));{struct Cyc_PP_Doc*_tmp2A7=
Cyc_Absynpp_exp2doc_prec(inprec,(struct Cyc_Absyn_Exp*)es->hd);return({struct Cyc_PP_Doc*
_tmp2A8[2];_tmp2A8[1]=Cyc_PP_text(({const char*_tmp2A9=".size";_tag_arr(_tmp2A9,
sizeof(char),_get_zero_arr_size(_tmp2A9,6));}));_tmp2A8[0]=_tmp2A7;Cyc_PP_cat(
_tag_arr(_tmp2A8,sizeof(struct Cyc_PP_Doc*),2));});}}else{struct Cyc_List_List*ds=((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);if(ds
== 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2AA=_cycalloc(
sizeof(*_tmp2AA));_tmp2AA[0]=({struct Cyc_Core_Failure_struct _tmp2AB;_tmp2AB.tag=
Cyc_Core_Failure;_tmp2AB.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp2AE;_tmp2AE.tag=0;_tmp2AE.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp2AC[1]={& _tmp2AE};Cyc_Std_aprintf(({const char*_tmp2AD="Absynpp::primapp2doc: %s with no args";
_tag_arr(_tmp2AD,sizeof(char),_get_zero_arr_size(_tmp2AD,38));}),_tag_arr(
_tmp2AC,sizeof(void*),1));}});_tmp2AB;});_tmp2AA;}));else{if(ds->tl == 0)return({
struct Cyc_PP_Doc*_tmp2AF[3];_tmp2AF[2]=(struct Cyc_PP_Doc*)ds->hd;_tmp2AF[1]=Cyc_PP_text(({
const char*_tmp2B0=" ";_tag_arr(_tmp2B0,sizeof(char),_get_zero_arr_size(_tmp2B0,2));}));
_tmp2AF[0]=ps;Cyc_PP_cat(_tag_arr(_tmp2AF,sizeof(struct Cyc_PP_Doc*),3));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));_tmp2B1[0]=({
struct Cyc_Core_Failure_struct _tmp2B2;_tmp2B2.tag=Cyc_Core_Failure;_tmp2B2.f1=(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp2B5;_tmp2B5.tag=0;_tmp2B5.f1=(
struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{void*_tmp2B3[1]={& _tmp2B5};Cyc_Std_aprintf(({
const char*_tmp2B4="Absynpp::primapp2doc: %s with more than 2 args";_tag_arr(
_tmp2B4,sizeof(char),_get_zero_arr_size(_tmp2B4,47));}),_tag_arr(_tmp2B3,sizeof(
void*),1));}});_tmp2B2;});_tmp2B1;}));else{return({struct Cyc_PP_Doc*_tmp2B6[5];
_tmp2B6[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;
_tmp2B6[3]=Cyc_PP_text(({const char*_tmp2B8=" ";_tag_arr(_tmp2B8,sizeof(char),
_get_zero_arr_size(_tmp2B8,2));}));_tmp2B6[2]=ps;_tmp2B6[1]=Cyc_PP_text(({const
char*_tmp2B7=" ";_tag_arr(_tmp2B7,sizeof(char),_get_zero_arr_size(_tmp2B7,2));}));
_tmp2B6[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_arr(_tmp2B6,sizeof(struct
Cyc_PP_Doc*),5));});}}}}}struct _tagged_arr Cyc_Absynpp_prim2str(void*p){void*
_tmp2B9=p;_LL1FC: if((int)_tmp2B9 != 0)goto _LL1FE;_LL1FD: return({const char*_tmp2BA="+";
_tag_arr(_tmp2BA,sizeof(char),_get_zero_arr_size(_tmp2BA,2));});_LL1FE: if((int)
_tmp2B9 != 1)goto _LL200;_LL1FF: return({const char*_tmp2BB="*";_tag_arr(_tmp2BB,
sizeof(char),_get_zero_arr_size(_tmp2BB,2));});_LL200: if((int)_tmp2B9 != 2)goto
_LL202;_LL201: return({const char*_tmp2BC="-";_tag_arr(_tmp2BC,sizeof(char),
_get_zero_arr_size(_tmp2BC,2));});_LL202: if((int)_tmp2B9 != 3)goto _LL204;_LL203:
return({const char*_tmp2BD="/";_tag_arr(_tmp2BD,sizeof(char),_get_zero_arr_size(
_tmp2BD,2));});_LL204: if((int)_tmp2B9 != 4)goto _LL206;_LL205: return({const char*
_tmp2BE="%";_tag_arr(_tmp2BE,sizeof(char),_get_zero_arr_size(_tmp2BE,2));});
_LL206: if((int)_tmp2B9 != 5)goto _LL208;_LL207: return({const char*_tmp2BF="==";
_tag_arr(_tmp2BF,sizeof(char),_get_zero_arr_size(_tmp2BF,3));});_LL208: if((int)
_tmp2B9 != 6)goto _LL20A;_LL209: return({const char*_tmp2C0="!=";_tag_arr(_tmp2C0,
sizeof(char),_get_zero_arr_size(_tmp2C0,3));});_LL20A: if((int)_tmp2B9 != 7)goto
_LL20C;_LL20B: return({const char*_tmp2C1=">";_tag_arr(_tmp2C1,sizeof(char),
_get_zero_arr_size(_tmp2C1,2));});_LL20C: if((int)_tmp2B9 != 8)goto _LL20E;_LL20D:
return({const char*_tmp2C2="<";_tag_arr(_tmp2C2,sizeof(char),_get_zero_arr_size(
_tmp2C2,2));});_LL20E: if((int)_tmp2B9 != 9)goto _LL210;_LL20F: return({const char*
_tmp2C3=">=";_tag_arr(_tmp2C3,sizeof(char),_get_zero_arr_size(_tmp2C3,3));});
_LL210: if((int)_tmp2B9 != 10)goto _LL212;_LL211: return({const char*_tmp2C4="<=";
_tag_arr(_tmp2C4,sizeof(char),_get_zero_arr_size(_tmp2C4,3));});_LL212: if((int)
_tmp2B9 != 11)goto _LL214;_LL213: return({const char*_tmp2C5="!";_tag_arr(_tmp2C5,
sizeof(char),_get_zero_arr_size(_tmp2C5,2));});_LL214: if((int)_tmp2B9 != 12)goto
_LL216;_LL215: return({const char*_tmp2C6="~";_tag_arr(_tmp2C6,sizeof(char),
_get_zero_arr_size(_tmp2C6,2));});_LL216: if((int)_tmp2B9 != 13)goto _LL218;_LL217:
return({const char*_tmp2C7="&";_tag_arr(_tmp2C7,sizeof(char),_get_zero_arr_size(
_tmp2C7,2));});_LL218: if((int)_tmp2B9 != 14)goto _LL21A;_LL219: return({const char*
_tmp2C8="|";_tag_arr(_tmp2C8,sizeof(char),_get_zero_arr_size(_tmp2C8,2));});
_LL21A: if((int)_tmp2B9 != 15)goto _LL21C;_LL21B: return({const char*_tmp2C9="^";
_tag_arr(_tmp2C9,sizeof(char),_get_zero_arr_size(_tmp2C9,2));});_LL21C: if((int)
_tmp2B9 != 16)goto _LL21E;_LL21D: return({const char*_tmp2CA="<<";_tag_arr(_tmp2CA,
sizeof(char),_get_zero_arr_size(_tmp2CA,3));});_LL21E: if((int)_tmp2B9 != 17)goto
_LL220;_LL21F: return({const char*_tmp2CB=">>";_tag_arr(_tmp2CB,sizeof(char),
_get_zero_arr_size(_tmp2CB,3));});_LL220: if((int)_tmp2B9 != 18)goto _LL222;_LL221:
return({const char*_tmp2CC=">>>";_tag_arr(_tmp2CC,sizeof(char),_get_zero_arr_size(
_tmp2CC,4));});_LL222: if((int)_tmp2B9 != 19)goto _LL1FB;_LL223: return({const char*
_tmp2CD="size";_tag_arr(_tmp2CD,sizeof(char),_get_zero_arr_size(_tmp2CD,5));});
_LL1FB:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(
p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){void*_tmp2CE=(void*)s->r;
_LL225: if(_tmp2CE <= (void*)1?1:*((int*)_tmp2CE)!= 12)goto _LL227;_LL226: return 1;
_LL227:;_LL228: return 0;_LL224:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*
st){struct Cyc_PP_Doc*s;{void*_tmp2CF=(void*)st->r;struct Cyc_Absyn_Exp*_tmp2D0;
struct Cyc_Absyn_Stmt*_tmp2D1;struct Cyc_Absyn_Stmt*_tmp2D2;struct Cyc_Absyn_Exp*
_tmp2D3;struct Cyc_Absyn_Exp*_tmp2D4;struct Cyc_Absyn_Stmt*_tmp2D5;struct Cyc_Absyn_Stmt*
_tmp2D6;struct _tuple2 _tmp2D7;struct Cyc_Absyn_Exp*_tmp2D8;struct Cyc_Absyn_Stmt*
_tmp2D9;struct _tagged_arr*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2DB;struct _tuple2
_tmp2DC;struct Cyc_Absyn_Exp*_tmp2DD;struct _tuple2 _tmp2DE;struct Cyc_Absyn_Exp*
_tmp2DF;struct Cyc_Absyn_Stmt*_tmp2E0;struct Cyc_Absyn_ForArrayInfo _tmp2E1;struct
Cyc_List_List*_tmp2E2;struct _tuple2 _tmp2E3;struct Cyc_Absyn_Exp*_tmp2E4;struct
_tuple2 _tmp2E5;struct Cyc_Absyn_Exp*_tmp2E6;struct Cyc_Absyn_Stmt*_tmp2E7;struct
Cyc_Absyn_Exp*_tmp2E8;struct Cyc_List_List*_tmp2E9;struct Cyc_Absyn_Exp*_tmp2EA;
struct Cyc_List_List*_tmp2EB;struct Cyc_List_List*_tmp2EC;struct Cyc_List_List*
_tmp2ED;struct Cyc_Absyn_Decl*_tmp2EE;struct Cyc_Absyn_Stmt*_tmp2EF;struct Cyc_Absyn_Stmt*
_tmp2F0;struct Cyc_Absyn_Stmt*_tmp2F1;struct _tagged_arr*_tmp2F2;struct Cyc_Absyn_Stmt*
_tmp2F3;struct Cyc_Absyn_Stmt*_tmp2F4;struct _tuple2 _tmp2F5;struct Cyc_Absyn_Exp*
_tmp2F6;struct Cyc_Absyn_Stmt*_tmp2F7;struct Cyc_List_List*_tmp2F8;struct Cyc_Absyn_Tvar*
_tmp2F9;struct Cyc_Absyn_Vardecl*_tmp2FA;int _tmp2FB;struct Cyc_Absyn_Stmt*_tmp2FC;
struct Cyc_Absyn_Exp*_tmp2FD;_LL22A: if((int)_tmp2CF != 0)goto _LL22C;_LL22B: s=Cyc_PP_text(({
const char*_tmp2FE=";";_tag_arr(_tmp2FE,sizeof(char),_get_zero_arr_size(_tmp2FE,2));}));
goto _LL229;_LL22C: if(_tmp2CF <= (void*)1?1:*((int*)_tmp2CF)!= 0)goto _LL22E;
_tmp2D0=((struct Cyc_Absyn_Exp_s_struct*)_tmp2CF)->f1;_LL22D: s=({struct Cyc_PP_Doc*
_tmp2FF[2];_tmp2FF[1]=Cyc_PP_text(({const char*_tmp300=";";_tag_arr(_tmp300,
sizeof(char),_get_zero_arr_size(_tmp300,2));}));_tmp2FF[0]=Cyc_Absynpp_exp2doc(
_tmp2D0);Cyc_PP_cat(_tag_arr(_tmp2FF,sizeof(struct Cyc_PP_Doc*),2));});goto _LL229;
_LL22E: if(_tmp2CF <= (void*)1?1:*((int*)_tmp2CF)!= 1)goto _LL230;_tmp2D1=((struct
Cyc_Absyn_Seq_s_struct*)_tmp2CF)->f1;_tmp2D2=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2CF)->f2;_LL22F: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(
_tmp2D1))s=({struct Cyc_PP_Doc*_tmp301[7];_tmp301[6]=Cyc_Absynpp_is_declaration(
_tmp2D2)?({struct Cyc_PP_Doc*_tmp302[5];_tmp302[4]=Cyc_PP_line_doc();_tmp302[3]=
Cyc_Absynpp_rb();_tmp302[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2D2));_tmp302[
1]=Cyc_PP_blank_doc();_tmp302[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_arr(_tmp302,
sizeof(struct Cyc_PP_Doc*),5));}): Cyc_Absynpp_stmt2doc(_tmp2D2);_tmp301[5]=Cyc_PP_line_doc();
_tmp301[4]=Cyc_Absynpp_rb();_tmp301[3]=Cyc_PP_line_doc();_tmp301[2]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp2D1));_tmp301[1]=Cyc_PP_blank_doc();_tmp301[0]=Cyc_Absynpp_lb();
Cyc_PP_cat(_tag_arr(_tmp301,sizeof(struct Cyc_PP_Doc*),7));});else{if(Cyc_Absynpp_is_declaration(
_tmp2D2))s=({struct Cyc_PP_Doc*_tmp303[7];_tmp303[6]=Cyc_PP_line_doc();_tmp303[5]=
Cyc_Absynpp_rb();_tmp303[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2D2));_tmp303[
3]=Cyc_PP_blank_doc();_tmp303[2]=Cyc_Absynpp_lb();_tmp303[1]=Cyc_PP_line_doc();
_tmp303[0]=Cyc_Absynpp_stmt2doc(_tmp2D1);Cyc_PP_cat(_tag_arr(_tmp303,sizeof(
struct Cyc_PP_Doc*),7));});else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Stmt*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,({const char*_tmp304="";_tag_arr(_tmp304,sizeof(char),
_get_zero_arr_size(_tmp304,1));}),({struct Cyc_List_List*_tmp305=_cycalloc(
sizeof(*_tmp305));_tmp305->hd=_tmp2D1;_tmp305->tl=({struct Cyc_List_List*_tmp306=
_cycalloc(sizeof(*_tmp306));_tmp306->hd=_tmp2D2;_tmp306->tl=0;_tmp306;});_tmp305;}));}}}
else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,({
const char*_tmp307="";_tag_arr(_tmp307,sizeof(char),_get_zero_arr_size(_tmp307,1));}),({
struct Cyc_List_List*_tmp308=_cycalloc(sizeof(*_tmp308));_tmp308->hd=_tmp2D1;
_tmp308->tl=({struct Cyc_List_List*_tmp309=_cycalloc(sizeof(*_tmp309));_tmp309->hd=
_tmp2D2;_tmp309->tl=0;_tmp309;});_tmp308;}));}goto _LL229;_LL230: if(_tmp2CF <= (
void*)1?1:*((int*)_tmp2CF)!= 2)goto _LL232;_tmp2D3=((struct Cyc_Absyn_Return_s_struct*)
_tmp2CF)->f1;_LL231: if(_tmp2D3 == 0)s=Cyc_PP_text(({const char*_tmp30A="return;";
_tag_arr(_tmp30A,sizeof(char),_get_zero_arr_size(_tmp30A,8));}));else{s=({struct
Cyc_PP_Doc*_tmp30B[3];_tmp30B[2]=Cyc_PP_text(({const char*_tmp30D=";";_tag_arr(
_tmp30D,sizeof(char),_get_zero_arr_size(_tmp30D,2));}));_tmp30B[1]=_tmp2D3 == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(_tmp2D3));
_tmp30B[0]=Cyc_PP_text(({const char*_tmp30C="return ";_tag_arr(_tmp30C,sizeof(
char),_get_zero_arr_size(_tmp30C,8));}));Cyc_PP_cat(_tag_arr(_tmp30B,sizeof(
struct Cyc_PP_Doc*),3));});}goto _LL229;_LL232: if(_tmp2CF <= (void*)1?1:*((int*)
_tmp2CF)!= 3)goto _LL234;_tmp2D4=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2CF)->f1;
_tmp2D5=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2CF)->f2;_tmp2D6=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2CF)->f3;_LL233: {int print_else;{void*_tmp30E=(void*)_tmp2D6->r;_LL259: if((
int)_tmp30E != 0)goto _LL25B;_LL25A: print_else=0;goto _LL258;_LL25B:;_LL25C:
print_else=1;goto _LL258;_LL258:;}s=({struct Cyc_PP_Doc*_tmp30F[8];_tmp30F[7]=
print_else?({struct Cyc_PP_Doc*_tmp313[6];_tmp313[5]=Cyc_Absynpp_rb();_tmp313[4]=
Cyc_PP_line_doc();_tmp313[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp315[2];_tmp315[
1]=Cyc_Absynpp_stmt2doc(_tmp2D6);_tmp315[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp315,sizeof(struct Cyc_PP_Doc*),2));}));_tmp313[2]=Cyc_Absynpp_lb();
_tmp313[1]=Cyc_PP_text(({const char*_tmp314="else ";_tag_arr(_tmp314,sizeof(char),
_get_zero_arr_size(_tmp314,6));}));_tmp313[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp313,sizeof(struct Cyc_PP_Doc*),6));}): Cyc_PP_nil_doc();_tmp30F[6]=
Cyc_Absynpp_rb();_tmp30F[5]=Cyc_PP_line_doc();_tmp30F[4]=Cyc_PP_nest(2,({struct
Cyc_PP_Doc*_tmp312[2];_tmp312[1]=Cyc_Absynpp_stmt2doc(_tmp2D5);_tmp312[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp312,sizeof(struct Cyc_PP_Doc*),2));}));_tmp30F[3]=Cyc_Absynpp_lb();
_tmp30F[2]=Cyc_PP_text(({const char*_tmp311=") ";_tag_arr(_tmp311,sizeof(char),
_get_zero_arr_size(_tmp311,3));}));_tmp30F[1]=Cyc_Absynpp_exp2doc(_tmp2D4);
_tmp30F[0]=Cyc_PP_text(({const char*_tmp310="if (";_tag_arr(_tmp310,sizeof(char),
_get_zero_arr_size(_tmp310,5));}));Cyc_PP_cat(_tag_arr(_tmp30F,sizeof(struct Cyc_PP_Doc*),
8));});goto _LL229;}_LL234: if(_tmp2CF <= (void*)1?1:*((int*)_tmp2CF)!= 4)goto
_LL236;_tmp2D7=((struct Cyc_Absyn_While_s_struct*)_tmp2CF)->f1;_tmp2D8=_tmp2D7.f1;
_tmp2D9=((struct Cyc_Absyn_While_s_struct*)_tmp2CF)->f2;_LL235: s=({struct Cyc_PP_Doc*
_tmp316[7];_tmp316[6]=Cyc_Absynpp_rb();_tmp316[5]=Cyc_PP_line_doc();_tmp316[4]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp319[2];_tmp319[1]=Cyc_Absynpp_stmt2doc(
_tmp2D9);_tmp319[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp319,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp316[3]=Cyc_Absynpp_lb();_tmp316[2]=Cyc_PP_text(({const
char*_tmp318=") ";_tag_arr(_tmp318,sizeof(char),_get_zero_arr_size(_tmp318,3));}));
_tmp316[1]=Cyc_Absynpp_exp2doc(_tmp2D8);_tmp316[0]=Cyc_PP_text(({const char*
_tmp317="while (";_tag_arr(_tmp317,sizeof(char),_get_zero_arr_size(_tmp317,8));}));
Cyc_PP_cat(_tag_arr(_tmp316,sizeof(struct Cyc_PP_Doc*),7));});goto _LL229;_LL236:
if(_tmp2CF <= (void*)1?1:*((int*)_tmp2CF)!= 5)goto _LL238;_LL237: s=Cyc_PP_text(({
const char*_tmp31A="break;";_tag_arr(_tmp31A,sizeof(char),_get_zero_arr_size(
_tmp31A,7));}));goto _LL229;_LL238: if(_tmp2CF <= (void*)1?1:*((int*)_tmp2CF)!= 6)
goto _LL23A;_LL239: s=Cyc_PP_text(({const char*_tmp31B="continue;";_tag_arr(_tmp31B,
sizeof(char),_get_zero_arr_size(_tmp31B,10));}));goto _LL229;_LL23A: if(_tmp2CF <= (
void*)1?1:*((int*)_tmp2CF)!= 7)goto _LL23C;_tmp2DA=((struct Cyc_Absyn_Goto_s_struct*)
_tmp2CF)->f1;_LL23B: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp31E;_tmp31E.tag=0;_tmp31E.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp2DA);{void*_tmp31C[1]={& _tmp31E};Cyc_Std_aprintf(({const char*_tmp31D="goto %s;";
_tag_arr(_tmp31D,sizeof(char),_get_zero_arr_size(_tmp31D,9));}),_tag_arr(_tmp31C,
sizeof(void*),1));}}));goto _LL229;_LL23C: if(_tmp2CF <= (void*)1?1:*((int*)_tmp2CF)
!= 8)goto _LL23E;_tmp2DB=((struct Cyc_Absyn_For_s_struct*)_tmp2CF)->f1;_tmp2DC=((
struct Cyc_Absyn_For_s_struct*)_tmp2CF)->f2;_tmp2DD=_tmp2DC.f1;_tmp2DE=((struct
Cyc_Absyn_For_s_struct*)_tmp2CF)->f3;_tmp2DF=_tmp2DE.f1;_tmp2E0=((struct Cyc_Absyn_For_s_struct*)
_tmp2CF)->f4;_LL23D: s=({struct Cyc_PP_Doc*_tmp31F[11];_tmp31F[10]=Cyc_Absynpp_rb();
_tmp31F[9]=Cyc_PP_line_doc();_tmp31F[8]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp324[
2];_tmp324[1]=Cyc_Absynpp_stmt2doc(_tmp2E0);_tmp324[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp324,sizeof(struct Cyc_PP_Doc*),2));}));_tmp31F[7]=Cyc_Absynpp_lb();
_tmp31F[6]=Cyc_PP_text(({const char*_tmp323=") ";_tag_arr(_tmp323,sizeof(char),
_get_zero_arr_size(_tmp323,3));}));_tmp31F[5]=Cyc_Absynpp_exp2doc(_tmp2DF);
_tmp31F[4]=Cyc_PP_text(({const char*_tmp322="; ";_tag_arr(_tmp322,sizeof(char),
_get_zero_arr_size(_tmp322,3));}));_tmp31F[3]=Cyc_Absynpp_exp2doc(_tmp2DD);
_tmp31F[2]=Cyc_PP_text(({const char*_tmp321="; ";_tag_arr(_tmp321,sizeof(char),
_get_zero_arr_size(_tmp321,3));}));_tmp31F[1]=Cyc_Absynpp_exp2doc(_tmp2DB);
_tmp31F[0]=Cyc_PP_text(({const char*_tmp320="for(";_tag_arr(_tmp320,sizeof(char),
_get_zero_arr_size(_tmp320,5));}));Cyc_PP_cat(_tag_arr(_tmp31F,sizeof(struct Cyc_PP_Doc*),
11));});goto _LL229;_LL23E: if(_tmp2CF <= (void*)1?1:*((int*)_tmp2CF)!= 19)goto
_LL240;_tmp2E1=((struct Cyc_Absyn_ForArray_s_struct*)_tmp2CF)->f1;_tmp2E2=_tmp2E1.defns;
_tmp2E3=_tmp2E1.condition;_tmp2E4=_tmp2E3.f1;_tmp2E5=_tmp2E1.delta;_tmp2E6=
_tmp2E5.f1;_tmp2E7=_tmp2E1.body;_LL23F: s=({struct Cyc_PP_Doc*_tmp325[10];_tmp325[
9]=Cyc_Absynpp_rb();_tmp325[8]=Cyc_PP_line_doc();_tmp325[7]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp32A[2];_tmp32A[1]=Cyc_Absynpp_stmt2doc(_tmp2E7);_tmp32A[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp32A,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp325[6]=Cyc_Absynpp_lb();_tmp325[5]=Cyc_PP_text(({const char*_tmp329=") ";
_tag_arr(_tmp329,sizeof(char),_get_zero_arr_size(_tmp329,3));}));_tmp325[4]=Cyc_Absynpp_exp2doc(
_tmp2E6);_tmp325[3]=Cyc_PP_text(({const char*_tmp328="; ";_tag_arr(_tmp328,
sizeof(char),_get_zero_arr_size(_tmp328,3));}));_tmp325[2]=Cyc_Absynpp_exp2doc(
_tmp2E4);_tmp325[1]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_vardecl2doc,({
const char*_tmp327=" ";_tag_arr(_tmp327,sizeof(char),_get_zero_arr_size(_tmp327,2));}),
_tmp2E2);_tmp325[0]=Cyc_PP_text(({const char*_tmp326="forarray(";_tag_arr(_tmp326,
sizeof(char),_get_zero_arr_size(_tmp326,10));}));Cyc_PP_cat(_tag_arr(_tmp325,
sizeof(struct Cyc_PP_Doc*),10));});goto _LL229;_LL240: if(_tmp2CF <= (void*)1?1:*((
int*)_tmp2CF)!= 9)goto _LL242;_tmp2E8=((struct Cyc_Absyn_Switch_s_struct*)_tmp2CF)->f1;
_tmp2E9=((struct Cyc_Absyn_Switch_s_struct*)_tmp2CF)->f2;_LL241: s=({struct Cyc_PP_Doc*
_tmp32B[8];_tmp32B[7]=Cyc_Absynpp_rb();_tmp32B[6]=Cyc_PP_line_doc();_tmp32B[5]=
Cyc_Absynpp_switchclauses2doc(_tmp2E9);_tmp32B[4]=Cyc_PP_line_doc();_tmp32B[3]=
Cyc_Absynpp_lb();_tmp32B[2]=Cyc_PP_text(({const char*_tmp32D=") ";_tag_arr(
_tmp32D,sizeof(char),_get_zero_arr_size(_tmp32D,3));}));_tmp32B[1]=Cyc_Absynpp_exp2doc(
_tmp2E8);_tmp32B[0]=Cyc_PP_text(({const char*_tmp32C="switch (";_tag_arr(_tmp32C,
sizeof(char),_get_zero_arr_size(_tmp32C,9));}));Cyc_PP_cat(_tag_arr(_tmp32B,
sizeof(struct Cyc_PP_Doc*),8));});goto _LL229;_LL242: if(_tmp2CF <= (void*)1?1:*((
int*)_tmp2CF)!= 10)goto _LL244;_tmp2EA=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmp2CF)->f1;_tmp2EB=((struct Cyc_Absyn_SwitchC_s_struct*)_tmp2CF)->f2;_LL243: s=({
struct Cyc_PP_Doc*_tmp32E[8];_tmp32E[7]=Cyc_Absynpp_rb();_tmp32E[6]=Cyc_PP_line_doc();
_tmp32E[5]=Cyc_Absynpp_switchCclauses2doc(_tmp2EB);_tmp32E[4]=Cyc_PP_line_doc();
_tmp32E[3]=Cyc_Absynpp_lb();_tmp32E[2]=Cyc_PP_text(({const char*_tmp330=") ";
_tag_arr(_tmp330,sizeof(char),_get_zero_arr_size(_tmp330,3));}));_tmp32E[1]=Cyc_Absynpp_exp2doc(
_tmp2EA);_tmp32E[0]=Cyc_PP_text(({const char*_tmp32F="switch \"C\" (";_tag_arr(
_tmp32F,sizeof(char),_get_zero_arr_size(_tmp32F,13));}));Cyc_PP_cat(_tag_arr(
_tmp32E,sizeof(struct Cyc_PP_Doc*),8));});goto _LL229;_LL244: if(_tmp2CF <= (void*)1?
1:*((int*)_tmp2CF)!= 11)goto _LL246;_tmp2EC=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp2CF)->f1;if(_tmp2EC != 0)goto _LL246;_LL245: s=Cyc_PP_text(({const char*_tmp331="fallthru;";
_tag_arr(_tmp331,sizeof(char),_get_zero_arr_size(_tmp331,10));}));goto _LL229;
_LL246: if(_tmp2CF <= (void*)1?1:*((int*)_tmp2CF)!= 11)goto _LL248;_tmp2ED=((struct
Cyc_Absyn_Fallthru_s_struct*)_tmp2CF)->f1;_LL247: s=({struct Cyc_PP_Doc*_tmp332[3];
_tmp332[2]=Cyc_PP_text(({const char*_tmp334=");";_tag_arr(_tmp334,sizeof(char),
_get_zero_arr_size(_tmp334,3));}));_tmp332[1]=Cyc_Absynpp_exps2doc_prec(20,
_tmp2ED);_tmp332[0]=Cyc_PP_text(({const char*_tmp333="fallthru(";_tag_arr(_tmp333,
sizeof(char),_get_zero_arr_size(_tmp333,10));}));Cyc_PP_cat(_tag_arr(_tmp332,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL229;_LL248: if(_tmp2CF <= (void*)1?1:*((
int*)_tmp2CF)!= 12)goto _LL24A;_tmp2EE=((struct Cyc_Absyn_Decl_s_struct*)_tmp2CF)->f1;
_tmp2EF=((struct Cyc_Absyn_Decl_s_struct*)_tmp2CF)->f2;_LL249: s=({struct Cyc_PP_Doc*
_tmp335[3];_tmp335[2]=Cyc_Absynpp_stmt2doc(_tmp2EF);_tmp335[1]=Cyc_PP_line_doc();
_tmp335[0]=Cyc_Absynpp_decl2doc(_tmp2EE);Cyc_PP_cat(_tag_arr(_tmp335,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL229;_LL24A: if(_tmp2CF <= (void*)1?1:*((int*)
_tmp2CF)!= 13)goto _LL24C;_tmp2F0=((struct Cyc_Absyn_Cut_s_struct*)_tmp2CF)->f1;
_LL24B: s=({struct Cyc_PP_Doc*_tmp336[2];_tmp336[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2F0));_tmp336[0]=Cyc_PP_text(({const char*_tmp337="cut ";_tag_arr(_tmp337,
sizeof(char),_get_zero_arr_size(_tmp337,5));}));Cyc_PP_cat(_tag_arr(_tmp336,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL229;_LL24C: if(_tmp2CF <= (void*)1?1:*((
int*)_tmp2CF)!= 14)goto _LL24E;_tmp2F1=((struct Cyc_Absyn_Splice_s_struct*)_tmp2CF)->f1;
_LL24D: s=({struct Cyc_PP_Doc*_tmp338[2];_tmp338[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2F1));_tmp338[0]=Cyc_PP_text(({const char*_tmp339="splice ";_tag_arr(_tmp339,
sizeof(char),_get_zero_arr_size(_tmp339,8));}));Cyc_PP_cat(_tag_arr(_tmp338,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL229;_LL24E: if(_tmp2CF <= (void*)1?1:*((
int*)_tmp2CF)!= 15)goto _LL250;_tmp2F2=((struct Cyc_Absyn_Label_s_struct*)_tmp2CF)->f1;
_tmp2F3=((struct Cyc_Absyn_Label_s_struct*)_tmp2CF)->f2;_LL24F: if(Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration(_tmp2F3): 0)s=({struct Cyc_PP_Doc*_tmp33A[7];_tmp33A[6]=
Cyc_Absynpp_rb();_tmp33A[5]=Cyc_PP_line_doc();_tmp33A[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2F3));_tmp33A[3]=Cyc_PP_line_doc();_tmp33A[2]=Cyc_Absynpp_lb();_tmp33A[1]=
Cyc_PP_text(({const char*_tmp33B=": ";_tag_arr(_tmp33B,sizeof(char),
_get_zero_arr_size(_tmp33B,3));}));_tmp33A[0]=Cyc_PP_textptr(_tmp2F2);Cyc_PP_cat(
_tag_arr(_tmp33A,sizeof(struct Cyc_PP_Doc*),7));});else{s=({struct Cyc_PP_Doc*
_tmp33C[3];_tmp33C[2]=Cyc_Absynpp_stmt2doc(_tmp2F3);_tmp33C[1]=Cyc_PP_text(({
const char*_tmp33D=": ";_tag_arr(_tmp33D,sizeof(char),_get_zero_arr_size(_tmp33D,
3));}));_tmp33C[0]=Cyc_PP_textptr(_tmp2F2);Cyc_PP_cat(_tag_arr(_tmp33C,sizeof(
struct Cyc_PP_Doc*),3));});}goto _LL229;_LL250: if(_tmp2CF <= (void*)1?1:*((int*)
_tmp2CF)!= 16)goto _LL252;_tmp2F4=((struct Cyc_Absyn_Do_s_struct*)_tmp2CF)->f1;
_tmp2F5=((struct Cyc_Absyn_Do_s_struct*)_tmp2CF)->f2;_tmp2F6=_tmp2F5.f1;_LL251: s=({
struct Cyc_PP_Doc*_tmp33E[9];_tmp33E[8]=Cyc_PP_text(({const char*_tmp341=");";
_tag_arr(_tmp341,sizeof(char),_get_zero_arr_size(_tmp341,3));}));_tmp33E[7]=Cyc_Absynpp_exp2doc(
_tmp2F6);_tmp33E[6]=Cyc_PP_text(({const char*_tmp340=" while (";_tag_arr(_tmp340,
sizeof(char),_get_zero_arr_size(_tmp340,9));}));_tmp33E[5]=Cyc_Absynpp_rb();
_tmp33E[4]=Cyc_PP_line_doc();_tmp33E[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2F4));_tmp33E[2]=Cyc_PP_line_doc();_tmp33E[1]=Cyc_Absynpp_lb();_tmp33E[0]=
Cyc_PP_text(({const char*_tmp33F="do ";_tag_arr(_tmp33F,sizeof(char),
_get_zero_arr_size(_tmp33F,4));}));Cyc_PP_cat(_tag_arr(_tmp33E,sizeof(struct Cyc_PP_Doc*),
9));});goto _LL229;_LL252: if(_tmp2CF <= (void*)1?1:*((int*)_tmp2CF)!= 17)goto
_LL254;_tmp2F7=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp2CF)->f1;_tmp2F8=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp2CF)->f2;_LL253: s=({struct Cyc_PP_Doc*
_tmp342[12];_tmp342[11]=Cyc_Absynpp_rb();_tmp342[10]=Cyc_PP_line_doc();_tmp342[9]=
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp2F8));_tmp342[8]=Cyc_PP_line_doc();
_tmp342[7]=Cyc_Absynpp_lb();_tmp342[6]=Cyc_PP_text(({const char*_tmp344=" catch ";
_tag_arr(_tmp344,sizeof(char),_get_zero_arr_size(_tmp344,8));}));_tmp342[5]=Cyc_Absynpp_rb();
_tmp342[4]=Cyc_PP_line_doc();_tmp342[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2F7));_tmp342[2]=Cyc_PP_line_doc();_tmp342[1]=Cyc_Absynpp_lb();_tmp342[0]=
Cyc_PP_text(({const char*_tmp343="try ";_tag_arr(_tmp343,sizeof(char),
_get_zero_arr_size(_tmp343,5));}));Cyc_PP_cat(_tag_arr(_tmp342,sizeof(struct Cyc_PP_Doc*),
12));});goto _LL229;_LL254: if(_tmp2CF <= (void*)1?1:*((int*)_tmp2CF)!= 18)goto
_LL256;_tmp2F9=((struct Cyc_Absyn_Region_s_struct*)_tmp2CF)->f1;_tmp2FA=((struct
Cyc_Absyn_Region_s_struct*)_tmp2CF)->f2;_tmp2FB=((struct Cyc_Absyn_Region_s_struct*)
_tmp2CF)->f3;_tmp2FC=((struct Cyc_Absyn_Region_s_struct*)_tmp2CF)->f4;_LL255: s=({
struct Cyc_PP_Doc*_tmp345[11];_tmp345[10]=Cyc_Absynpp_rb();_tmp345[9]=Cyc_PP_line_doc();
_tmp345[8]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2FC));_tmp345[7]=Cyc_PP_line_doc();
_tmp345[6]=Cyc_Absynpp_lb();_tmp345[5]=Cyc_Absynpp_qvar2doc(_tmp2FA->name);
_tmp345[4]=Cyc_PP_text(({const char*_tmp34A=">";_tag_arr(_tmp34A,sizeof(char),
_get_zero_arr_size(_tmp34A,2));}));_tmp345[3]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp2F9));_tmp345[2]=Cyc_PP_text(({const char*_tmp349="<";_tag_arr(_tmp349,
sizeof(char),_get_zero_arr_size(_tmp349,2));}));_tmp345[1]=_tmp2FB?Cyc_PP_text(({
const char*_tmp347="";_tag_arr(_tmp347,sizeof(char),_get_zero_arr_size(_tmp347,1));})):
Cyc_PP_text(({const char*_tmp348="[resetable]";_tag_arr(_tmp348,sizeof(char),
_get_zero_arr_size(_tmp348,12));}));_tmp345[0]=Cyc_PP_text(({const char*_tmp346="region";
_tag_arr(_tmp346,sizeof(char),_get_zero_arr_size(_tmp346,7));}));Cyc_PP_cat(
_tag_arr(_tmp345,sizeof(struct Cyc_PP_Doc*),11));});goto _LL229;_LL256: if(_tmp2CF
<= (void*)1?1:*((int*)_tmp2CF)!= 20)goto _LL229;_tmp2FD=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp2CF)->f1;_LL257: s=({struct Cyc_PP_Doc*_tmp34B[3];_tmp34B[2]=Cyc_PP_text(({
const char*_tmp34D=");";_tag_arr(_tmp34D,sizeof(char),_get_zero_arr_size(_tmp34D,
3));}));_tmp34B[1]=Cyc_Absynpp_exp2doc(_tmp2FD);_tmp34B[0]=Cyc_PP_text(({const
char*_tmp34C="reset_region(";_tag_arr(_tmp34C,sizeof(char),_get_zero_arr_size(
_tmp34C,14));}));Cyc_PP_cat(_tag_arr(_tmp34B,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL229;_LL229:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp34E=(void*)p->r;void*_tmp34F;int _tmp350;char
_tmp351;struct _tagged_arr _tmp352;struct Cyc_Absyn_Vardecl*_tmp353;struct Cyc_List_List*
_tmp354;struct Cyc_Absyn_Pat*_tmp355;struct Cyc_Absyn_Vardecl*_tmp356;struct
_tuple0*_tmp357;struct _tuple0*_tmp358;struct Cyc_List_List*_tmp359;struct Cyc_Absyn_AggrInfo
_tmp35A;void*_tmp35B;struct Cyc_List_List*_tmp35C;struct Cyc_List_List*_tmp35D;
struct Cyc_Absyn_Enumfield*_tmp35E;struct Cyc_Absyn_Enumfield*_tmp35F;struct Cyc_Absyn_Tunionfield*
_tmp360;struct Cyc_List_List*_tmp361;struct Cyc_Absyn_Tunionfield*_tmp362;struct
Cyc_List_List*_tmp363;_LL25E: if((int)_tmp34E != 0)goto _LL260;_LL25F: s=Cyc_PP_text(({
const char*_tmp364="_";_tag_arr(_tmp364,sizeof(char),_get_zero_arr_size(_tmp364,2));}));
goto _LL25D;_LL260: if((int)_tmp34E != 1)goto _LL262;_LL261: s=Cyc_PP_text(({const
char*_tmp365="NULL";_tag_arr(_tmp365,sizeof(char),_get_zero_arr_size(_tmp365,5));}));
goto _LL25D;_LL262: if(_tmp34E <= (void*)2?1:*((int*)_tmp34E)!= 1)goto _LL264;
_tmp34F=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp34E)->f1;_tmp350=((struct Cyc_Absyn_Int_p_struct*)
_tmp34E)->f2;_LL263: if(_tmp34F != (void*)1)s=Cyc_PP_text((struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp368;_tmp368.tag=1;_tmp368.f1=(int)((unsigned int)
_tmp350);{void*_tmp366[1]={& _tmp368};Cyc_Std_aprintf(({const char*_tmp367="%d";
_tag_arr(_tmp367,sizeof(char),_get_zero_arr_size(_tmp367,3));}),_tag_arr(_tmp366,
sizeof(void*),1));}}));else{s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp36B;_tmp36B.tag=1;_tmp36B.f1=(unsigned int)_tmp350;{void*_tmp369[1]={&
_tmp36B};Cyc_Std_aprintf(({const char*_tmp36A="%u";_tag_arr(_tmp36A,sizeof(char),
_get_zero_arr_size(_tmp36A,3));}),_tag_arr(_tmp369,sizeof(void*),1));}}));}goto
_LL25D;_LL264: if(_tmp34E <= (void*)2?1:*((int*)_tmp34E)!= 2)goto _LL266;_tmp351=((
struct Cyc_Absyn_Char_p_struct*)_tmp34E)->f1;_LL265: s=Cyc_PP_text((struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp36E;_tmp36E.tag=0;_tmp36E.f1=(
struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp351);{void*_tmp36C[1]={& _tmp36E};
Cyc_Std_aprintf(({const char*_tmp36D="'%s'";_tag_arr(_tmp36D,sizeof(char),
_get_zero_arr_size(_tmp36D,5));}),_tag_arr(_tmp36C,sizeof(void*),1));}}));goto
_LL25D;_LL266: if(_tmp34E <= (void*)2?1:*((int*)_tmp34E)!= 3)goto _LL268;_tmp352=((
struct Cyc_Absyn_Float_p_struct*)_tmp34E)->f1;_LL267: s=Cyc_PP_text(_tmp352);goto
_LL25D;_LL268: if(_tmp34E <= (void*)2?1:*((int*)_tmp34E)!= 0)goto _LL26A;_tmp353=((
struct Cyc_Absyn_Var_p_struct*)_tmp34E)->f1;_LL269: s=Cyc_Absynpp_qvar2doc(_tmp353->name);
goto _LL25D;_LL26A: if(_tmp34E <= (void*)2?1:*((int*)_tmp34E)!= 4)goto _LL26C;
_tmp354=((struct Cyc_Absyn_Tuple_p_struct*)_tmp34E)->f1;_LL26B: s=({struct Cyc_PP_Doc*
_tmp36F[4];_tmp36F[3]=Cyc_PP_text(({const char*_tmp372=")";_tag_arr(_tmp372,
sizeof(char),_get_zero_arr_size(_tmp372,2));}));_tmp36F[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,({const char*_tmp371=",";_tag_arr(_tmp371,
sizeof(char),_get_zero_arr_size(_tmp371,2));}),_tmp354);_tmp36F[1]=Cyc_PP_text(({
const char*_tmp370="(";_tag_arr(_tmp370,sizeof(char),_get_zero_arr_size(_tmp370,2));}));
_tmp36F[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_arr(_tmp36F,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL25D;_LL26C: if(_tmp34E <= (void*)2?1:*((int*)_tmp34E)!= 5)goto _LL26E;
_tmp355=((struct Cyc_Absyn_Pointer_p_struct*)_tmp34E)->f1;_LL26D: s=({struct Cyc_PP_Doc*
_tmp373[2];_tmp373[1]=Cyc_Absynpp_pat2doc(_tmp355);_tmp373[0]=Cyc_PP_text(({
const char*_tmp374="&";_tag_arr(_tmp374,sizeof(char),_get_zero_arr_size(_tmp374,2));}));
Cyc_PP_cat(_tag_arr(_tmp373,sizeof(struct Cyc_PP_Doc*),2));});goto _LL25D;_LL26E:
if(_tmp34E <= (void*)2?1:*((int*)_tmp34E)!= 6)goto _LL270;_tmp356=((struct Cyc_Absyn_Reference_p_struct*)
_tmp34E)->f1;_LL26F: s=({struct Cyc_PP_Doc*_tmp375[2];_tmp375[1]=Cyc_Absynpp_qvar2doc(
_tmp356->name);_tmp375[0]=Cyc_PP_text(({const char*_tmp376="*";_tag_arr(_tmp376,
sizeof(char),_get_zero_arr_size(_tmp376,2));}));Cyc_PP_cat(_tag_arr(_tmp375,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL25D;_LL270: if(_tmp34E <= (void*)2?1:*((
int*)_tmp34E)!= 11)goto _LL272;_tmp357=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp34E)->f1;_LL271: s=Cyc_Absynpp_qvar2doc(_tmp357);goto _LL25D;_LL272: if(_tmp34E
<= (void*)2?1:*((int*)_tmp34E)!= 12)goto _LL274;_tmp358=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp34E)->f1;_tmp359=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp34E)->f2;_LL273:
s=({struct Cyc_PP_Doc*_tmp377[2];_tmp377[1]=Cyc_PP_group(({const char*_tmp378="(";
_tag_arr(_tmp378,sizeof(char),_get_zero_arr_size(_tmp378,2));}),({const char*
_tmp379=")";_tag_arr(_tmp379,sizeof(char),_get_zero_arr_size(_tmp379,2));}),({
const char*_tmp37A=",";_tag_arr(_tmp37A,sizeof(char),_get_zero_arr_size(_tmp37A,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp359));_tmp377[0]=Cyc_Absynpp_qvar2doc(
_tmp358);Cyc_PP_cat(_tag_arr(_tmp377,sizeof(struct Cyc_PP_Doc*),2));});goto _LL25D;
_LL274: if(_tmp34E <= (void*)2?1:*((int*)_tmp34E)!= 7)goto _LL276;_tmp35A=((struct
Cyc_Absyn_Aggr_p_struct*)_tmp34E)->f1;_tmp35B=(void*)_tmp35A.aggr_info;_tmp35C=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp34E)->f2;_tmp35D=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp34E)->f3;_LL275: {struct _tuple0*_tmp37C;struct _tuple3 _tmp37B=Cyc_Absyn_aggr_kinded_name(
_tmp35B);_tmp37C=_tmp37B.f2;s=({struct Cyc_PP_Doc*_tmp37D[4];_tmp37D[3]=Cyc_Absynpp_group_braces(({
const char*_tmp381=",";_tag_arr(_tmp381,sizeof(char),_get_zero_arr_size(_tmp381,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp35D));_tmp37D[2]=Cyc_PP_egroup(({const
char*_tmp37E="[";_tag_arr(_tmp37E,sizeof(char),_get_zero_arr_size(_tmp37E,2));}),({
const char*_tmp37F="]";_tag_arr(_tmp37F,sizeof(char),_get_zero_arr_size(_tmp37F,2));}),({
const char*_tmp380=",";_tag_arr(_tmp380,sizeof(char),_get_zero_arr_size(_tmp380,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
_tmp35C)));_tmp37D[1]=Cyc_Absynpp_lb();_tmp37D[0]=Cyc_Absynpp_qvar2doc(_tmp37C);
Cyc_PP_cat(_tag_arr(_tmp37D,sizeof(struct Cyc_PP_Doc*),4));});goto _LL25D;}_LL276:
if(_tmp34E <= (void*)2?1:*((int*)_tmp34E)!= 9)goto _LL278;_tmp35E=((struct Cyc_Absyn_Enum_p_struct*)
_tmp34E)->f2;_LL277: _tmp35F=_tmp35E;goto _LL279;_LL278: if(_tmp34E <= (void*)2?1:*((
int*)_tmp34E)!= 10)goto _LL27A;_tmp35F=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp34E)->f2;_LL279: s=Cyc_Absynpp_qvar2doc(_tmp35F->name);goto _LL25D;_LL27A: if(
_tmp34E <= (void*)2?1:*((int*)_tmp34E)!= 8)goto _LL27C;_tmp360=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp34E)->f2;_tmp361=((struct Cyc_Absyn_Tunion_p_struct*)_tmp34E)->f3;if(_tmp361
!= 0)goto _LL27C;_LL27B: s=Cyc_Absynpp_qvar2doc(_tmp360->name);goto _LL25D;_LL27C:
if(_tmp34E <= (void*)2?1:*((int*)_tmp34E)!= 8)goto _LL25D;_tmp362=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp34E)->f2;_tmp363=((struct Cyc_Absyn_Tunion_p_struct*)_tmp34E)->f3;_LL27D: s=({
struct Cyc_PP_Doc*_tmp382[2];_tmp382[1]=Cyc_PP_egroup(({const char*_tmp383="(";
_tag_arr(_tmp383,sizeof(char),_get_zero_arr_size(_tmp383,2));}),({const char*
_tmp384=")";_tag_arr(_tmp384,sizeof(char),_get_zero_arr_size(_tmp384,2));}),({
const char*_tmp385=",";_tag_arr(_tmp385,sizeof(char),_get_zero_arr_size(_tmp385,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp363));_tmp382[0]=Cyc_Absynpp_qvar2doc(
_tmp362->name);Cyc_PP_cat(_tag_arr(_tmp382,sizeof(struct Cyc_PP_Doc*),2));});goto
_LL25D;_LL25D:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp){
return({struct Cyc_PP_Doc*_tmp386[2];_tmp386[1]=Cyc_Absynpp_pat2doc((*((struct
_tuple8*)dp)).f2);_tmp386[0]=Cyc_PP_egroup(({const char*_tmp387="";_tag_arr(
_tmp387,sizeof(char),_get_zero_arr_size(_tmp387,1));}),({const char*_tmp388="=";
_tag_arr(_tmp388,sizeof(char),_get_zero_arr_size(_tmp388,2));}),({const char*
_tmp389="=";_tag_arr(_tmp389,sizeof(char),_get_zero_arr_size(_tmp389,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*((struct _tuple8*)dp)).f1));Cyc_PP_cat(_tag_arr(
_tmp386,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0?(void*)(c->pattern)->r == (
void*)0: 0)return({struct Cyc_PP_Doc*_tmp38A[2];_tmp38A[1]=Cyc_PP_nest(2,({struct
Cyc_PP_Doc*_tmp38C[2];_tmp38C[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp38C[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp38C,sizeof(struct Cyc_PP_Doc*),2));}));_tmp38A[0]=Cyc_PP_text(({
const char*_tmp38B="default: ";_tag_arr(_tmp38B,sizeof(char),_get_zero_arr_size(
_tmp38B,10));}));Cyc_PP_cat(_tag_arr(_tmp38A,sizeof(struct Cyc_PP_Doc*),2));});
else{if(c->where_clause == 0)return({struct Cyc_PP_Doc*_tmp38D[4];_tmp38D[3]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp390[2];_tmp390[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp390[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp390,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp38D[2]=Cyc_PP_text(({const char*_tmp38F=": ";_tag_arr(_tmp38F,sizeof(char),
_get_zero_arr_size(_tmp38F,3));}));_tmp38D[1]=Cyc_Absynpp_pat2doc(c->pattern);
_tmp38D[0]=Cyc_PP_text(({const char*_tmp38E="case ";_tag_arr(_tmp38E,sizeof(char),
_get_zero_arr_size(_tmp38E,6));}));Cyc_PP_cat(_tag_arr(_tmp38D,sizeof(struct Cyc_PP_Doc*),
4));});else{return({struct Cyc_PP_Doc*_tmp391[6];_tmp391[5]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp395[2];_tmp395[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp395[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp395,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp391[4]=Cyc_PP_text(({const char*_tmp394=": ";_tag_arr(_tmp394,sizeof(char),
_get_zero_arr_size(_tmp394,3));}));_tmp391[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(c->where_clause));_tmp391[2]=Cyc_PP_text(({const char*_tmp393=" && ";
_tag_arr(_tmp393,sizeof(char),_get_zero_arr_size(_tmp393,5));}));_tmp391[1]=Cyc_Absynpp_pat2doc(
c->pattern);_tmp391[0]=Cyc_PP_text(({const char*_tmp392="case ";_tag_arr(_tmp392,
sizeof(char),_get_zero_arr_size(_tmp392,6));}));Cyc_PP_cat(_tag_arr(_tmp391,
sizeof(struct Cyc_PP_Doc*),6));});}}}struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Switch_clause*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_switchclause2doc,({const char*_tmp396="";_tag_arr(_tmp396,sizeof(char),
_get_zero_arr_size(_tmp396,1));}),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause*c){struct Cyc_Absyn_SwitchC_clause _tmp398;struct
Cyc_Absyn_Exp*_tmp399;struct Cyc_Absyn_Stmt*_tmp39A;struct Cyc_Absyn_SwitchC_clause*
_tmp397=c;_tmp398=*_tmp397;_tmp399=_tmp398.cnst_exp;_tmp39A=_tmp398.body;if(
_tmp399 == 0)return({struct Cyc_PP_Doc*_tmp39B[2];_tmp39B[1]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp39D[2];_tmp39D[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp39D[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp39D,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp39B[0]=Cyc_PP_text(({const char*_tmp39C="default: ";_tag_arr(_tmp39C,sizeof(
char),_get_zero_arr_size(_tmp39C,10));}));Cyc_PP_cat(_tag_arr(_tmp39B,sizeof(
struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*_tmp39E[4];_tmp39E[3]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3A1[2];_tmp3A1[1]=Cyc_Absynpp_stmt2doc(
_tmp39A);_tmp3A1[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3A1,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp39E[2]=Cyc_PP_text(({const char*_tmp3A0=": ";_tag_arr(
_tmp3A0,sizeof(char),_get_zero_arr_size(_tmp3A0,3));}));_tmp39E[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(_tmp399));_tmp39E[0]=Cyc_PP_text(({const char*
_tmp39F="case ";_tag_arr(_tmp39F,sizeof(char),_get_zero_arr_size(_tmp39F,6));}));
Cyc_PP_cat(_tag_arr(_tmp39E,sizeof(struct Cyc_PP_Doc*),4));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_switchCclauses2doc(struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_SwitchC_clause*),struct _tagged_arr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchCclause2doc,({const char*
_tmp3A2="";_tag_arr(_tmp3A2,sizeof(char),_get_zero_arr_size(_tmp3A2,1));}),cs);}
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){if(f->tag
== 0)return Cyc_Absynpp_qvar2doc(f->name);else{return({struct Cyc_PP_Doc*_tmp3A3[3];
_tmp3A3[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));
_tmp3A3[1]=Cyc_PP_text(({const char*_tmp3A4=" = ";_tag_arr(_tmp3A4,sizeof(char),
_get_zero_arr_size(_tmp3A4,4));}));_tmp3A3[0]=Cyc_Absynpp_qvar2doc(f->name);Cyc_PP_cat(
_tag_arr(_tmp3A3,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Enumfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_enumfield2doc,({const char*_tmp3A5=",";_tag_arr(_tmp3A5,sizeof(char),
_get_zero_arr_size(_tmp3A5,2));}),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(
struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*
Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){return((struct Cyc_PP_Doc*(*)(struct
Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,({const char*_tmp3A6=",";_tag_arr(_tmp3A6,
sizeof(char),_get_zero_arr_size(_tmp3A6,2));}),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(
struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl _tmp3A8;void*_tmp3A9;struct
_tuple0*_tmp3AA;struct Cyc_Absyn_Tqual _tmp3AB;void*_tmp3AC;struct Cyc_Absyn_Exp*
_tmp3AD;struct Cyc_List_List*_tmp3AE;struct Cyc_Absyn_Vardecl*_tmp3A7=vd;_tmp3A8=*
_tmp3A7;_tmp3A9=(void*)_tmp3A8.sc;_tmp3AA=_tmp3A8.name;_tmp3AB=_tmp3A8.tq;
_tmp3AC=(void*)_tmp3A8.type;_tmp3AD=_tmp3A8.initializer;_tmp3AE=_tmp3A8.attributes;{
struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2doc(_tmp3AA);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp3AE);struct Cyc_PP_Doc*
beforenamedoc;if(!Cyc_Absynpp_to_VC)beforenamedoc=attsdoc;else{void*_tmp3AF=Cyc_Tcutil_compress(
_tmp3AC);struct Cyc_Absyn_FnInfo _tmp3B0;struct Cyc_List_List*_tmp3B1;_LL27F: if(
_tmp3AF <= (void*)3?1:*((int*)_tmp3AF)!= 8)goto _LL281;_tmp3B0=((struct Cyc_Absyn_FnType_struct*)
_tmp3AF)->f1;_tmp3B1=_tmp3B0.attributes;_LL280: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp3B1);goto _LL27E;_LL281:;_LL282: beforenamedoc=Cyc_PP_nil_doc();goto _LL27E;
_LL27E:;}s=({struct Cyc_PP_Doc*_tmp3B2[5];_tmp3B2[4]=Cyc_PP_text(({const char*
_tmp3B8=";";_tag_arr(_tmp3B8,sizeof(char),_get_zero_arr_size(_tmp3B8,2));}));
_tmp3B2[3]=_tmp3AD == 0?Cyc_PP_text(({const char*_tmp3B5="";_tag_arr(_tmp3B5,
sizeof(char),_get_zero_arr_size(_tmp3B5,1));})):({struct Cyc_PP_Doc*_tmp3B6[2];
_tmp3B6[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(_tmp3AD));
_tmp3B6[0]=Cyc_PP_text(({const char*_tmp3B7=" = ";_tag_arr(_tmp3B7,sizeof(char),
_get_zero_arr_size(_tmp3B7,4));}));Cyc_PP_cat(_tag_arr(_tmp3B6,sizeof(struct Cyc_PP_Doc*),
2));});_tmp3B2[2]=Cyc_Absynpp_tqtd2doc(_tmp3AB,_tmp3AC,({struct Cyc_Core_Opt*
_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3->v=({struct Cyc_PP_Doc*_tmp3B4[2];
_tmp3B4[1]=sn;_tmp3B4[0]=beforenamedoc;Cyc_PP_cat(_tag_arr(_tmp3B4,sizeof(struct
Cyc_PP_Doc*),2));});_tmp3B3;}));_tmp3B2[1]=Cyc_Absynpp_scope2doc(_tmp3A9);
_tmp3B2[0]=Cyc_Absynpp_to_VC?attsdoc: Cyc_PP_nil_doc();Cyc_PP_cat(_tag_arr(
_tmp3B2,sizeof(struct Cyc_PP_Doc*),5));});return s;}}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp3B9=(void*)d->r;struct Cyc_Absyn_Fndecl*
_tmp3BA;struct Cyc_Absyn_Aggrdecl*_tmp3BB;struct Cyc_Absyn_Vardecl*_tmp3BC;struct
Cyc_Absyn_Tuniondecl*_tmp3BD;struct Cyc_Absyn_Tuniondecl _tmp3BE;void*_tmp3BF;
struct _tuple0*_tmp3C0;struct Cyc_List_List*_tmp3C1;struct Cyc_Core_Opt*_tmp3C2;int
_tmp3C3;struct Cyc_Absyn_Pat*_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C5;struct Cyc_List_List*
_tmp3C6;struct Cyc_Absyn_Enumdecl*_tmp3C7;struct Cyc_Absyn_Enumdecl _tmp3C8;void*
_tmp3C9;struct _tuple0*_tmp3CA;struct Cyc_Core_Opt*_tmp3CB;struct Cyc_Absyn_Typedefdecl*
_tmp3CC;struct _tagged_arr*_tmp3CD;struct Cyc_List_List*_tmp3CE;struct _tuple0*
_tmp3CF;struct Cyc_List_List*_tmp3D0;struct Cyc_List_List*_tmp3D1;_LL284: if(*((int*)
_tmp3B9)!= 1)goto _LL286;_tmp3BA=((struct Cyc_Absyn_Fn_d_struct*)_tmp3B9)->f1;
_LL285: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp3DB=_cycalloc(sizeof(*
_tmp3DB));_tmp3DB[0]=({struct Cyc_Absyn_FnType_struct _tmp3DC;_tmp3DC.tag=8;
_tmp3DC.f1=({struct Cyc_Absyn_FnInfo _tmp3DD;_tmp3DD.tvars=_tmp3BA->tvs;_tmp3DD.effect=
_tmp3BA->effect;_tmp3DD.ret_typ=(void*)((void*)_tmp3BA->ret_type);_tmp3DD.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3BA->args);_tmp3DD.c_varargs=_tmp3BA->c_varargs;
_tmp3DD.cyc_varargs=_tmp3BA->cyc_varargs;_tmp3DD.rgn_po=_tmp3BA->rgn_po;_tmp3DD.attributes=
0;_tmp3DD;});_tmp3DC;});_tmp3DB;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp3BA->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp3BA->is_inline){if(Cyc_Absynpp_to_VC)
inlinedoc=Cyc_PP_text(({const char*_tmp3D2="__inline ";_tag_arr(_tmp3D2,sizeof(
char),_get_zero_arr_size(_tmp3D2,10));}));else{inlinedoc=Cyc_PP_text(({const char*
_tmp3D3="inline ";_tag_arr(_tmp3D3,sizeof(char),_get_zero_arr_size(_tmp3D3,8));}));}}
else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((
void*)_tmp3BA->sc);struct Cyc_PP_Doc*beforenamedoc=Cyc_Absynpp_to_VC?Cyc_Absynpp_callconv2doc(
_tmp3BA->attributes): attsdoc;struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp3BA->name);struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(({struct Cyc_Absyn_Tqual
_tmp3D8;_tmp3D8.q_const=0;_tmp3D8.q_volatile=0;_tmp3D8.q_restrict=0;_tmp3D8;}),t,({
struct Cyc_Core_Opt*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9->v=({struct Cyc_PP_Doc*
_tmp3DA[2];_tmp3DA[1]=namedoc;_tmp3DA[0]=beforenamedoc;Cyc_PP_cat(_tag_arr(
_tmp3DA,sizeof(struct Cyc_PP_Doc*),2));});_tmp3D9;}));struct Cyc_PP_Doc*bodydoc=({
struct Cyc_PP_Doc*_tmp3D6[5];_tmp3D6[4]=Cyc_Absynpp_rb();_tmp3D6[3]=Cyc_PP_line_doc();
_tmp3D6[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3D7[2];_tmp3D7[1]=Cyc_Absynpp_stmt2doc(
_tmp3BA->body);_tmp3D7[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3D7,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp3D6[1]=Cyc_Absynpp_lb();_tmp3D6[0]=Cyc_PP_blank_doc();
Cyc_PP_cat(_tag_arr(_tmp3D6,sizeof(struct Cyc_PP_Doc*),5));});s=({struct Cyc_PP_Doc*
_tmp3D4[4];_tmp3D4[3]=bodydoc;_tmp3D4[2]=tqtddoc;_tmp3D4[1]=scopedoc;_tmp3D4[0]=
inlinedoc;Cyc_PP_cat(_tag_arr(_tmp3D4,sizeof(struct Cyc_PP_Doc*),4));});if(Cyc_Absynpp_to_VC)
s=({struct Cyc_PP_Doc*_tmp3D5[2];_tmp3D5[1]=s;_tmp3D5[0]=attsdoc;Cyc_PP_cat(
_tag_arr(_tmp3D5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL283;}}_LL286: if(*((int*)
_tmp3B9)!= 4)goto _LL288;_tmp3BB=((struct Cyc_Absyn_Aggr_d_struct*)_tmp3B9)->f1;
_LL287: if(_tmp3BB->impl == 0)s=({struct Cyc_PP_Doc*_tmp3DE[5];_tmp3DE[4]=Cyc_PP_text(({
const char*_tmp3DF=";";_tag_arr(_tmp3DF,sizeof(char),_get_zero_arr_size(_tmp3DF,2));}));
_tmp3DE[3]=Cyc_Absynpp_ktvars2doc(_tmp3BB->tvs);_tmp3DE[2]=Cyc_Absynpp_qvar2doc(
_tmp3BB->name);_tmp3DE[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp3BB->kind);
_tmp3DE[0]=Cyc_Absynpp_scope2doc((void*)_tmp3BB->sc);Cyc_PP_cat(_tag_arr(_tmp3DE,
sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp3E0[12];_tmp3E0[
11]=Cyc_PP_text(({const char*_tmp3E4=";";_tag_arr(_tmp3E4,sizeof(char),
_get_zero_arr_size(_tmp3E4,2));}));_tmp3E0[10]=Cyc_Absynpp_atts2doc(_tmp3BB->attributes);
_tmp3E0[9]=Cyc_Absynpp_rb();_tmp3E0[8]=Cyc_PP_line_doc();_tmp3E0[7]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp3E3[2];_tmp3E3[1]=Cyc_Absynpp_aggrfields2doc(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3BB->impl))->fields);_tmp3E3[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp3E3,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3E0[6]=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3BB->impl))->rgn_po == 0?Cyc_PP_nil_doc():({
struct Cyc_PP_Doc*_tmp3E1[2];_tmp3E1[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3BB->impl))->rgn_po);_tmp3E1[0]=Cyc_PP_text(({const char*_tmp3E2=":";
_tag_arr(_tmp3E2,sizeof(char),_get_zero_arr_size(_tmp3E2,2));}));Cyc_PP_cat(
_tag_arr(_tmp3E1,sizeof(struct Cyc_PP_Doc*),2));});_tmp3E0[5]=Cyc_Absynpp_ktvars2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3BB->impl))->exist_vars);_tmp3E0[4]=
Cyc_Absynpp_lb();_tmp3E0[3]=Cyc_PP_blank_doc();_tmp3E0[2]=Cyc_Absynpp_qvar2doc(
_tmp3BB->name);_tmp3E0[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp3BB->kind);
_tmp3E0[0]=Cyc_Absynpp_scope2doc((void*)_tmp3BB->sc);Cyc_PP_cat(_tag_arr(_tmp3E0,
sizeof(struct Cyc_PP_Doc*),12));});}goto _LL283;_LL288: if(*((int*)_tmp3B9)!= 0)
goto _LL28A;_tmp3BC=((struct Cyc_Absyn_Var_d_struct*)_tmp3B9)->f1;_LL289: s=Cyc_Absynpp_vardecl2doc(
_tmp3BC);goto _LL283;_LL28A: if(*((int*)_tmp3B9)!= 5)goto _LL28C;_tmp3BD=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp3B9)->f1;_tmp3BE=*_tmp3BD;_tmp3BF=(void*)_tmp3BE.sc;
_tmp3C0=_tmp3BE.name;_tmp3C1=_tmp3BE.tvs;_tmp3C2=_tmp3BE.fields;_tmp3C3=_tmp3BE.is_xtunion;
_LL28B: if(_tmp3C2 == 0)s=({struct Cyc_PP_Doc*_tmp3E5[5];_tmp3E5[4]=Cyc_PP_text(({
const char*_tmp3E8=";";_tag_arr(_tmp3E8,sizeof(char),_get_zero_arr_size(_tmp3E8,2));}));
_tmp3E5[3]=Cyc_Absynpp_ktvars2doc(_tmp3C1);_tmp3E5[2]=_tmp3C3?Cyc_Absynpp_qvar2doc(
_tmp3C0): Cyc_Absynpp_typedef_name2doc(_tmp3C0);_tmp3E5[1]=_tmp3C3?Cyc_PP_text(({
const char*_tmp3E6="xtunion ";_tag_arr(_tmp3E6,sizeof(char),_get_zero_arr_size(
_tmp3E6,9));})): Cyc_PP_text(({const char*_tmp3E7="tunion ";_tag_arr(_tmp3E7,
sizeof(char),_get_zero_arr_size(_tmp3E7,8));}));_tmp3E5[0]=Cyc_Absynpp_scope2doc(
_tmp3BF);Cyc_PP_cat(_tag_arr(_tmp3E5,sizeof(struct Cyc_PP_Doc*),5));});else{s=({
struct Cyc_PP_Doc*_tmp3E9[10];_tmp3E9[9]=Cyc_PP_text(({const char*_tmp3ED=";";
_tag_arr(_tmp3ED,sizeof(char),_get_zero_arr_size(_tmp3ED,2));}));_tmp3E9[8]=Cyc_Absynpp_rb();
_tmp3E9[7]=Cyc_PP_line_doc();_tmp3E9[6]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3EC[
2];_tmp3EC[1]=Cyc_Absynpp_tunionfields2doc((struct Cyc_List_List*)_tmp3C2->v);
_tmp3EC[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3EC,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp3E9[5]=Cyc_Absynpp_lb();_tmp3E9[4]=Cyc_PP_blank_doc();_tmp3E9[3]=Cyc_Absynpp_ktvars2doc(
_tmp3C1);_tmp3E9[2]=_tmp3C3?Cyc_Absynpp_qvar2doc(_tmp3C0): Cyc_Absynpp_typedef_name2doc(
_tmp3C0);_tmp3E9[1]=_tmp3C3?Cyc_PP_text(({const char*_tmp3EA="xtunion ";_tag_arr(
_tmp3EA,sizeof(char),_get_zero_arr_size(_tmp3EA,9));})): Cyc_PP_text(({const char*
_tmp3EB="tunion ";_tag_arr(_tmp3EB,sizeof(char),_get_zero_arr_size(_tmp3EB,8));}));
_tmp3E9[0]=Cyc_Absynpp_scope2doc(_tmp3BF);Cyc_PP_cat(_tag_arr(_tmp3E9,sizeof(
struct Cyc_PP_Doc*),10));});}goto _LL283;_LL28C: if(*((int*)_tmp3B9)!= 2)goto _LL28E;
_tmp3C4=((struct Cyc_Absyn_Let_d_struct*)_tmp3B9)->f1;_tmp3C5=((struct Cyc_Absyn_Let_d_struct*)
_tmp3B9)->f3;_LL28D: s=({struct Cyc_PP_Doc*_tmp3EE[5];_tmp3EE[4]=Cyc_PP_text(({
const char*_tmp3F1=";";_tag_arr(_tmp3F1,sizeof(char),_get_zero_arr_size(_tmp3F1,2));}));
_tmp3EE[3]=Cyc_Absynpp_exp2doc(_tmp3C5);_tmp3EE[2]=Cyc_PP_text(({const char*
_tmp3F0=" = ";_tag_arr(_tmp3F0,sizeof(char),_get_zero_arr_size(_tmp3F0,4));}));
_tmp3EE[1]=Cyc_Absynpp_pat2doc(_tmp3C4);_tmp3EE[0]=Cyc_PP_text(({const char*
_tmp3EF="let ";_tag_arr(_tmp3EF,sizeof(char),_get_zero_arr_size(_tmp3EF,5));}));
Cyc_PP_cat(_tag_arr(_tmp3EE,sizeof(struct Cyc_PP_Doc*),5));});goto _LL283;_LL28E:
if(*((int*)_tmp3B9)!= 3)goto _LL290;_tmp3C6=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3B9)->f1;_LL28F: s=({struct Cyc_PP_Doc*_tmp3F2[3];_tmp3F2[2]=Cyc_PP_text(({
const char*_tmp3F4=";";_tag_arr(_tmp3F4,sizeof(char),_get_zero_arr_size(_tmp3F4,2));}));
_tmp3F2[1]=Cyc_Absynpp_ids2doc(_tmp3C6);_tmp3F2[0]=Cyc_PP_text(({const char*
_tmp3F3="let ";_tag_arr(_tmp3F3,sizeof(char),_get_zero_arr_size(_tmp3F3,5));}));
Cyc_PP_cat(_tag_arr(_tmp3F2,sizeof(struct Cyc_PP_Doc*),3));});goto _LL283;_LL290:
if(*((int*)_tmp3B9)!= 6)goto _LL292;_tmp3C7=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3B9)->f1;_tmp3C8=*_tmp3C7;_tmp3C9=(void*)_tmp3C8.sc;_tmp3CA=_tmp3C8.name;
_tmp3CB=_tmp3C8.fields;_LL291: if(_tmp3CB == 0)s=({struct Cyc_PP_Doc*_tmp3F5[4];
_tmp3F5[3]=Cyc_PP_text(({const char*_tmp3F7=";";_tag_arr(_tmp3F7,sizeof(char),
_get_zero_arr_size(_tmp3F7,2));}));_tmp3F5[2]=Cyc_Absynpp_typedef_name2doc(
_tmp3CA);_tmp3F5[1]=Cyc_PP_text(({const char*_tmp3F6="enum ";_tag_arr(_tmp3F6,
sizeof(char),_get_zero_arr_size(_tmp3F6,6));}));_tmp3F5[0]=Cyc_Absynpp_scope2doc(
_tmp3C9);Cyc_PP_cat(_tag_arr(_tmp3F5,sizeof(struct Cyc_PP_Doc*),4));});else{s=({
struct Cyc_PP_Doc*_tmp3F8[9];_tmp3F8[8]=Cyc_PP_text(({const char*_tmp3FB=";";
_tag_arr(_tmp3FB,sizeof(char),_get_zero_arr_size(_tmp3FB,2));}));_tmp3F8[7]=Cyc_Absynpp_rb();
_tmp3F8[6]=Cyc_PP_line_doc();_tmp3F8[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3FA[
2];_tmp3FA[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp3CB->v);
_tmp3FA[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3FA,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp3F8[4]=Cyc_Absynpp_lb();_tmp3F8[3]=Cyc_PP_blank_doc();_tmp3F8[2]=Cyc_Absynpp_qvar2doc(
_tmp3CA);_tmp3F8[1]=Cyc_PP_text(({const char*_tmp3F9="enum ";_tag_arr(_tmp3F9,
sizeof(char),_get_zero_arr_size(_tmp3F9,6));}));_tmp3F8[0]=Cyc_Absynpp_scope2doc(
_tmp3C9);Cyc_PP_cat(_tag_arr(_tmp3F8,sizeof(struct Cyc_PP_Doc*),9));});}goto
_LL283;_LL292: if(*((int*)_tmp3B9)!= 7)goto _LL294;_tmp3CC=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp3B9)->f1;_LL293: {void*t;if(_tmp3CC->defn != 0)t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3CC->defn))->v;else{t=Cyc_Absyn_new_evar(_tmp3CC->kind,0);}s=({
struct Cyc_PP_Doc*_tmp3FC[3];_tmp3FC[2]=Cyc_PP_text(({const char*_tmp401=";";
_tag_arr(_tmp401,sizeof(char),_get_zero_arr_size(_tmp401,2));}));_tmp3FC[1]=Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual _tmp3FE;_tmp3FE.q_const=0;_tmp3FE.q_volatile=0;_tmp3FE.q_restrict=
0;_tmp3FE;}),t,({struct Cyc_Core_Opt*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF->v=({
struct Cyc_PP_Doc*_tmp400[2];_tmp400[1]=Cyc_Absynpp_tvars2doc(_tmp3CC->tvs);
_tmp400[0]=Cyc_Absynpp_typedef_name2doc(_tmp3CC->name);Cyc_PP_cat(_tag_arr(
_tmp400,sizeof(struct Cyc_PP_Doc*),2));});_tmp3FF;}));_tmp3FC[0]=Cyc_PP_text(({
const char*_tmp3FD="typedef ";_tag_arr(_tmp3FD,sizeof(char),_get_zero_arr_size(
_tmp3FD,9));}));Cyc_PP_cat(_tag_arr(_tmp3FC,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL283;}_LL294: if(*((int*)_tmp3B9)!= 8)goto _LL296;_tmp3CD=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3B9)->f1;_tmp3CE=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3B9)->f2;_LL295:
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp3CD);s=({
struct Cyc_PP_Doc*_tmp402[8];_tmp402[7]=Cyc_Absynpp_rb();_tmp402[6]=Cyc_PP_line_doc();
_tmp402[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp404="";_tag_arr(_tmp404,sizeof(char),_get_zero_arr_size(_tmp404,1));}),
_tmp3CE);_tmp402[4]=Cyc_PP_line_doc();_tmp402[3]=Cyc_Absynpp_lb();_tmp402[2]=Cyc_PP_blank_doc();
_tmp402[1]=Cyc_PP_textptr(_tmp3CD);_tmp402[0]=Cyc_PP_text(({const char*_tmp403="namespace ";
_tag_arr(_tmp403,sizeof(char),_get_zero_arr_size(_tmp403,11));}));Cyc_PP_cat(
_tag_arr(_tmp402,sizeof(struct Cyc_PP_Doc*),8));});if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_drop();goto _LL283;_LL296: if(*((int*)_tmp3B9)!= 9)goto
_LL298;_tmp3CF=((struct Cyc_Absyn_Using_d_struct*)_tmp3B9)->f1;_tmp3D0=((struct
Cyc_Absyn_Using_d_struct*)_tmp3B9)->f2;_LL297: if(Cyc_Absynpp_print_using_stmts)s=({
struct Cyc_PP_Doc*_tmp405[8];_tmp405[7]=Cyc_Absynpp_rb();_tmp405[6]=Cyc_PP_line_doc();
_tmp405[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp407="";_tag_arr(_tmp407,sizeof(char),_get_zero_arr_size(_tmp407,1));}),
_tmp3D0);_tmp405[4]=Cyc_PP_line_doc();_tmp405[3]=Cyc_Absynpp_lb();_tmp405[2]=Cyc_PP_blank_doc();
_tmp405[1]=Cyc_Absynpp_qvar2doc(_tmp3CF);_tmp405[0]=Cyc_PP_text(({const char*
_tmp406="using ";_tag_arr(_tmp406,sizeof(char),_get_zero_arr_size(_tmp406,7));}));
Cyc_PP_cat(_tag_arr(_tmp405,sizeof(struct Cyc_PP_Doc*),8));});else{s=({struct Cyc_PP_Doc*
_tmp408[11];_tmp408[10]=Cyc_PP_text(({const char*_tmp40D=" */";_tag_arr(_tmp40D,
sizeof(char),_get_zero_arr_size(_tmp40D,4));}));_tmp408[9]=Cyc_Absynpp_rb();
_tmp408[8]=Cyc_PP_text(({const char*_tmp40C="/* ";_tag_arr(_tmp40C,sizeof(char),
_get_zero_arr_size(_tmp40C,4));}));_tmp408[7]=Cyc_PP_line_doc();_tmp408[6]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp40B="";_tag_arr(_tmp40B,sizeof(char),_get_zero_arr_size(_tmp40B,1));}),
_tmp3D0);_tmp408[5]=Cyc_PP_line_doc();_tmp408[4]=Cyc_PP_text(({const char*_tmp40A=" */";
_tag_arr(_tmp40A,sizeof(char),_get_zero_arr_size(_tmp40A,4));}));_tmp408[3]=Cyc_Absynpp_lb();
_tmp408[2]=Cyc_PP_blank_doc();_tmp408[1]=Cyc_Absynpp_qvar2doc(_tmp3CF);_tmp408[0]=
Cyc_PP_text(({const char*_tmp409="/* using ";_tag_arr(_tmp409,sizeof(char),
_get_zero_arr_size(_tmp409,10));}));Cyc_PP_cat(_tag_arr(_tmp408,sizeof(struct Cyc_PP_Doc*),
11));});}goto _LL283;_LL298: if(*((int*)_tmp3B9)!= 10)goto _LL283;_tmp3D1=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp3B9)->f1;_LL299: if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp40E[6];_tmp40E[5]=Cyc_Absynpp_rb();_tmp40E[4]=Cyc_PP_line_doc();
_tmp40E[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp410="";_tag_arr(_tmp410,sizeof(char),_get_zero_arr_size(_tmp410,1));}),
_tmp3D1);_tmp40E[2]=Cyc_PP_line_doc();_tmp40E[1]=Cyc_Absynpp_lb();_tmp40E[0]=Cyc_PP_text(({
const char*_tmp40F="extern \"C\" ";_tag_arr(_tmp40F,sizeof(char),
_get_zero_arr_size(_tmp40F,12));}));Cyc_PP_cat(_tag_arr(_tmp40E,sizeof(struct Cyc_PP_Doc*),
6));});else{s=({struct Cyc_PP_Doc*_tmp411[9];_tmp411[8]=Cyc_PP_text(({const char*
_tmp416=" */";_tag_arr(_tmp416,sizeof(char),_get_zero_arr_size(_tmp416,4));}));
_tmp411[7]=Cyc_Absynpp_rb();_tmp411[6]=Cyc_PP_text(({const char*_tmp415="/* ";
_tag_arr(_tmp415,sizeof(char),_get_zero_arr_size(_tmp415,4));}));_tmp411[5]=Cyc_PP_line_doc();
_tmp411[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp414="";_tag_arr(_tmp414,sizeof(char),_get_zero_arr_size(_tmp414,1));}),
_tmp3D1);_tmp411[3]=Cyc_PP_line_doc();_tmp411[2]=Cyc_PP_text(({const char*_tmp413=" */";
_tag_arr(_tmp413,sizeof(char),_get_zero_arr_size(_tmp413,4));}));_tmp411[1]=Cyc_Absynpp_lb();
_tmp411[0]=Cyc_PP_text(({const char*_tmp412="/* extern \"C\" ";_tag_arr(_tmp412,
sizeof(char),_get_zero_arr_size(_tmp412,15));}));Cyc_PP_cat(_tag_arr(_tmp411,
sizeof(struct Cyc_PP_Doc*),9));});}goto _LL283;_LL283:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(void*sc){void*_tmp417=sc;_LL29B: if((int)_tmp417 != 0)goto
_LL29D;_LL29C: return Cyc_PP_text(({const char*_tmp418="static ";_tag_arr(_tmp418,
sizeof(char),_get_zero_arr_size(_tmp418,8));}));_LL29D: if((int)_tmp417 != 2)goto
_LL29F;_LL29E: return Cyc_PP_nil_doc();_LL29F: if((int)_tmp417 != 3)goto _LL2A1;
_LL2A0: return Cyc_PP_text(({const char*_tmp419="extern ";_tag_arr(_tmp419,sizeof(
char),_get_zero_arr_size(_tmp419,8));}));_LL2A1: if((int)_tmp417 != 4)goto _LL2A3;
_LL2A2: return Cyc_PP_text(({const char*_tmp41A="extern \"C\" ";_tag_arr(_tmp41A,
sizeof(char),_get_zero_arr_size(_tmp41A,12));}));_LL2A3: if((int)_tmp417 != 1)goto
_LL2A5;_LL2A4: return Cyc_PP_text(({const char*_tmp41B="abstract ";_tag_arr(_tmp41B,
sizeof(char),_get_zero_arr_size(_tmp41B,10));}));_LL2A5: if((int)_tmp417 != 5)goto
_LL29A;_LL2A6: return Cyc_PP_text(({const char*_tmp41C="register ";_tag_arr(_tmp41C,
sizeof(char),_get_zero_arr_size(_tmp41C,10));}));_LL29A:;}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp41D=t;struct Cyc_Absyn_Tvar*_tmp41E;struct Cyc_List_List*_tmp41F;
_LL2A8: if(_tmp41D <= (void*)3?1:*((int*)_tmp41D)!= 1)goto _LL2AA;_tmp41E=((struct
Cyc_Absyn_VarType_struct*)_tmp41D)->f1;_LL2A9: return Cyc_Tcutil_is_temp_tvar(
_tmp41E);_LL2AA: if(_tmp41D <= (void*)3?1:*((int*)_tmp41D)!= 20)goto _LL2AC;_tmp41F=((
struct Cyc_Absyn_JoinEff_struct*)_tmp41D)->f1;_LL2AB: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp41F);_LL2AC:;_LL2AD: return 0;_LL2A7:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp420=t;struct Cyc_Core_Opt*_tmp421;struct Cyc_Core_Opt _tmp422;void*_tmp423;
_LL2AF: if(_tmp420 <= (void*)3?1:*((int*)_tmp420)!= 11)goto _LL2B1;_LL2B0: return 1;
_LL2B1: if(_tmp420 <= (void*)3?1:*((int*)_tmp420)!= 16)goto _LL2B3;_tmp421=((struct
Cyc_Absyn_TypedefType_struct*)_tmp420)->f4;if(_tmp421 == 0)goto _LL2B3;_tmp422=*
_tmp421;_tmp423=(void*)_tmp422.v;_LL2B2: return Cyc_Absynpp_is_anon_aggrtype(
_tmp423);_LL2B3:;_LL2B4: return 0;_LL2AE:;}struct _tuple7 Cyc_Absynpp_to_tms(struct
Cyc_Absyn_Tqual tq,void*t){void*_tmp424=t;struct Cyc_Absyn_ArrayInfo _tmp425;void*
_tmp426;struct Cyc_Absyn_Tqual _tmp427;struct Cyc_Absyn_Exp*_tmp428;struct Cyc_Absyn_Conref*
_tmp429;struct Cyc_Absyn_PtrInfo _tmp42A;void*_tmp42B;void*_tmp42C;struct Cyc_Absyn_Conref*
_tmp42D;struct Cyc_Absyn_Tqual _tmp42E;struct Cyc_Absyn_Conref*_tmp42F;struct Cyc_Absyn_Conref*
_tmp430;struct Cyc_Absyn_FnInfo _tmp431;struct Cyc_List_List*_tmp432;struct Cyc_Core_Opt*
_tmp433;void*_tmp434;struct Cyc_List_List*_tmp435;int _tmp436;struct Cyc_Absyn_VarargInfo*
_tmp437;struct Cyc_List_List*_tmp438;struct Cyc_List_List*_tmp439;struct Cyc_Core_Opt*
_tmp43A;struct Cyc_Core_Opt*_tmp43B;int _tmp43C;struct _tuple0*_tmp43D;struct Cyc_List_List*
_tmp43E;struct Cyc_Core_Opt*_tmp43F;_LL2B6: if(_tmp424 <= (void*)3?1:*((int*)
_tmp424)!= 7)goto _LL2B8;_tmp425=((struct Cyc_Absyn_ArrayType_struct*)_tmp424)->f1;
_tmp426=(void*)_tmp425.elt_type;_tmp427=_tmp425.tq;_tmp428=_tmp425.num_elts;
_tmp429=_tmp425.zero_term;_LL2B7: {struct Cyc_Absyn_Tqual _tmp441;void*_tmp442;
struct Cyc_List_List*_tmp443;struct _tuple7 _tmp440=Cyc_Absynpp_to_tms(_tmp427,
_tmp426);_tmp441=_tmp440.f1;_tmp442=_tmp440.f2;_tmp443=_tmp440.f3;{void*tm;if(
_tmp428 == 0)tm=(void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp444=_cycalloc(
sizeof(*_tmp444));_tmp444[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp445;_tmp445.tag=
0;_tmp445.f1=_tmp429;_tmp445;});_tmp444;});else{tm=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp446=_cycalloc(sizeof(*_tmp446));_tmp446[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp447;_tmp447.tag=1;_tmp447.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp428);
_tmp447.f2=_tmp429;_tmp447;});_tmp446;});}return({struct _tuple7 _tmp448;_tmp448.f1=
_tmp441;_tmp448.f2=_tmp442;_tmp448.f3=({struct Cyc_List_List*_tmp449=_cycalloc(
sizeof(*_tmp449));_tmp449->hd=(void*)tm;_tmp449->tl=_tmp443;_tmp449;});_tmp448;});}}
_LL2B8: if(_tmp424 <= (void*)3?1:*((int*)_tmp424)!= 4)goto _LL2BA;_tmp42A=((struct
Cyc_Absyn_PointerType_struct*)_tmp424)->f1;_tmp42B=(void*)_tmp42A.elt_typ;
_tmp42C=(void*)_tmp42A.rgn_typ;_tmp42D=_tmp42A.nullable;_tmp42E=_tmp42A.tq;
_tmp42F=_tmp42A.bounds;_tmp430=_tmp42A.zero_term;_LL2B9: {struct Cyc_Absyn_Tqual
_tmp44B;void*_tmp44C;struct Cyc_List_List*_tmp44D;struct _tuple7 _tmp44A=Cyc_Absynpp_to_tms(
_tmp42E,_tmp42B);_tmp44B=_tmp44A.f1;_tmp44C=_tmp44A.f2;_tmp44D=_tmp44A.f3;{void*
ps;{struct _tuple5 _tmp44F=({struct _tuple5 _tmp44E;_tmp44E.f1=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp42D))->v;_tmp44E.f2=(
void*)(Cyc_Absyn_compress_conref(_tmp42F))->v;_tmp44E;});void*_tmp450;int _tmp451;
void*_tmp452;void*_tmp453;struct Cyc_Absyn_Exp*_tmp454;void*_tmp455;int _tmp456;
void*_tmp457;void*_tmp458;struct Cyc_Absyn_Exp*_tmp459;void*_tmp45A;void*_tmp45B;
_LL2C3: _tmp450=_tmp44F.f1;if(_tmp450 <= (void*)1?1:*((int*)_tmp450)!= 0)goto
_LL2C5;_tmp451=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp450)->f1;if(_tmp451
!= 1)goto _LL2C5;_tmp452=_tmp44F.f2;if(_tmp452 <= (void*)1?1:*((int*)_tmp452)!= 0)
goto _LL2C5;_tmp453=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp452)->f1;if(
_tmp453 <= (void*)1?1:*((int*)_tmp453)!= 0)goto _LL2C5;_tmp454=((struct Cyc_Absyn_Upper_b_struct*)
_tmp453)->f1;_LL2C4: ps=(void*)({struct Cyc_Absyn_Nullable_ps_struct*_tmp45C=
_cycalloc(sizeof(*_tmp45C));_tmp45C[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp45D;_tmp45D.tag=1;_tmp45D.f1=_tmp454;_tmp45D.f2=_tmp430;_tmp45D;});_tmp45C;});
goto _LL2C2;_LL2C5: _tmp455=_tmp44F.f1;if(_tmp455 <= (void*)1?1:*((int*)_tmp455)!= 
0)goto _LL2C7;_tmp456=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp455)->f1;if(
_tmp456 != 0)goto _LL2C7;_tmp457=_tmp44F.f2;if(_tmp457 <= (void*)1?1:*((int*)
_tmp457)!= 0)goto _LL2C7;_tmp458=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp457)->f1;if(_tmp458 <= (void*)1?1:*((int*)_tmp458)!= 0)goto _LL2C7;_tmp459=((
struct Cyc_Absyn_Upper_b_struct*)_tmp458)->f1;_LL2C6: ps=(void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp45E=_cycalloc(sizeof(*_tmp45E));_tmp45E[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp45F;_tmp45F.tag=0;_tmp45F.f1=_tmp459;_tmp45F.f2=_tmp430;_tmp45F;});_tmp45E;});
goto _LL2C2;_LL2C7: _tmp45A=_tmp44F.f2;if(_tmp45A <= (void*)1?1:*((int*)_tmp45A)!= 
0)goto _LL2C9;_tmp45B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp45A)->f1;if((
int)_tmp45B != 0)goto _LL2C9;_LL2C8: ps=(void*)({struct Cyc_Absyn_TaggedArray_ps_struct*
_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460[0]=({struct Cyc_Absyn_TaggedArray_ps_struct
_tmp461;_tmp461.tag=2;_tmp461.f1=_tmp430;_tmp461;});_tmp460;});goto _LL2C2;_LL2C9:;
_LL2CA: ps=(void*)({struct Cyc_Absyn_TaggedArray_ps_struct*_tmp462=_cycalloc(
sizeof(*_tmp462));_tmp462[0]=({struct Cyc_Absyn_TaggedArray_ps_struct _tmp463;
_tmp463.tag=2;_tmp463.f1=Cyc_Absyn_false_conref;_tmp463;});_tmp462;});goto _LL2C2;
_LL2C2:;}{void*tm=(void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp466=_cycalloc(
sizeof(*_tmp466));_tmp466[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp467;_tmp467.tag=
2;_tmp467.f1=(void*)ps;_tmp467.f2=(void*)_tmp42C;_tmp467.f3=tq;_tmp467;});
_tmp466;});return({struct _tuple7 _tmp464;_tmp464.f1=_tmp44B;_tmp464.f2=_tmp44C;
_tmp464.f3=({struct Cyc_List_List*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->hd=(
void*)tm;_tmp465->tl=_tmp44D;_tmp465;});_tmp464;});}}}_LL2BA: if(_tmp424 <= (void*)
3?1:*((int*)_tmp424)!= 8)goto _LL2BC;_tmp431=((struct Cyc_Absyn_FnType_struct*)
_tmp424)->f1;_tmp432=_tmp431.tvars;_tmp433=_tmp431.effect;_tmp434=(void*)_tmp431.ret_typ;
_tmp435=_tmp431.args;_tmp436=_tmp431.c_varargs;_tmp437=_tmp431.cyc_varargs;
_tmp438=_tmp431.rgn_po;_tmp439=_tmp431.attributes;_LL2BB: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp433 == 0?1: Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp433->v)){
_tmp433=0;_tmp432=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp432);}{struct Cyc_Absyn_Tqual _tmp469;void*_tmp46A;struct Cyc_List_List*_tmp46B;
struct _tuple7 _tmp468=Cyc_Absynpp_to_tms(Cyc_Absyn_empty_tqual(),_tmp434);_tmp469=
_tmp468.f1;_tmp46A=_tmp468.f2;_tmp46B=_tmp468.f3;{struct Cyc_List_List*tms=
_tmp46B;if(_tmp439 != 0?!Cyc_Absynpp_to_VC: 0)tms=({struct Cyc_List_List*_tmp46C=
_cycalloc(sizeof(*_tmp46C));_tmp46C->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp46E;_tmp46E.tag=5;_tmp46E.f1=0;_tmp46E.f2=_tmp439;_tmp46E;});_tmp46D;}));
_tmp46C->tl=tms;_tmp46C;});tms=({struct Cyc_List_List*_tmp46F=_cycalloc(sizeof(*
_tmp46F));_tmp46F->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp471;_tmp471.tag=3;_tmp471.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp473;_tmp473.tag=1;_tmp473.f1=_tmp435;_tmp473.f2=_tmp436;_tmp473.f3=_tmp437;
_tmp473.f4=_tmp433;_tmp473.f5=_tmp438;_tmp473;});_tmp472;}));_tmp471;});_tmp470;}));
_tmp46F->tl=tms;_tmp46F;});if(Cyc_Absynpp_to_VC)for(0;_tmp439 != 0;_tmp439=
_tmp439->tl){void*_tmp474=(void*)_tmp439->hd;_LL2CC: if((int)_tmp474 != 0)goto
_LL2CE;_LL2CD: goto _LL2CF;_LL2CE: if((int)_tmp474 != 1)goto _LL2D0;_LL2CF: goto _LL2D1;
_LL2D0: if((int)_tmp474 != 2)goto _LL2D2;_LL2D1: tms=({struct Cyc_List_List*_tmp475=
_cycalloc(sizeof(*_tmp475));_tmp475->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp476=_cycalloc(sizeof(*_tmp476));_tmp476[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp477;_tmp477.tag=5;_tmp477.f1=0;_tmp477.f2=({struct Cyc_List_List*_tmp478=
_cycalloc(sizeof(*_tmp478));_tmp478->hd=(void*)((void*)_tmp439->hd);_tmp478->tl=
0;_tmp478;});_tmp477;});_tmp476;}));_tmp475->tl=tms;_tmp475;});goto AfterAtts;
_LL2D2:;_LL2D3: goto _LL2CB;_LL2CB:;}AfterAtts: if(_tmp432 != 0)tms=({struct Cyc_List_List*
_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp47B;_tmp47B.tag=4;_tmp47B.f1=_tmp432;_tmp47B.f2=0;_tmp47B.f3=1;_tmp47B;});
_tmp47A;}));_tmp479->tl=tms;_tmp479;});return({struct _tuple7 _tmp47C;_tmp47C.f1=
_tmp469;_tmp47C.f2=_tmp46A;_tmp47C.f3=tms;_tmp47C;});}}_LL2BC: if(_tmp424 <= (void*)
3?1:*((int*)_tmp424)!= 0)goto _LL2BE;_tmp43A=((struct Cyc_Absyn_Evar_struct*)
_tmp424)->f1;_tmp43B=((struct Cyc_Absyn_Evar_struct*)_tmp424)->f2;_tmp43C=((
struct Cyc_Absyn_Evar_struct*)_tmp424)->f3;_LL2BD: if(_tmp43B == 0)return({struct
_tuple7 _tmp47D;_tmp47D.f1=tq;_tmp47D.f2=t;_tmp47D.f3=0;_tmp47D;});else{return Cyc_Absynpp_to_tms(
tq,(void*)_tmp43B->v);}_LL2BE: if(_tmp424 <= (void*)3?1:*((int*)_tmp424)!= 16)goto
_LL2C0;_tmp43D=((struct Cyc_Absyn_TypedefType_struct*)_tmp424)->f1;_tmp43E=((
struct Cyc_Absyn_TypedefType_struct*)_tmp424)->f2;_tmp43F=((struct Cyc_Absyn_TypedefType_struct*)
_tmp424)->f4;_LL2BF: if((_tmp43F == 0?1: !Cyc_Absynpp_expand_typedefs)?1: Cyc_Absynpp_is_anon_aggrtype((
void*)_tmp43F->v))return({struct _tuple7 _tmp47E;_tmp47E.f1=tq;_tmp47E.f2=t;
_tmp47E.f3=0;_tmp47E;});else{return Cyc_Absynpp_to_tms(tq,(void*)_tmp43F->v);}
_LL2C0:;_LL2C1: return({struct _tuple7 _tmp47F;_tmp47F.f1=tq;_tmp47F.f2=t;_tmp47F.f3=
0;_tmp47F;});_LL2B5:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_Core_Opt*dopt){struct Cyc_Absyn_Tqual _tmp481;void*_tmp482;
struct Cyc_List_List*_tmp483;struct _tuple7 _tmp480=Cyc_Absynpp_to_tms(tq,t);
_tmp481=_tmp480.f1;_tmp482=_tmp480.f2;_tmp483=_tmp480.f3;_tmp483=Cyc_List_imp_rev(
_tmp483);if(_tmp483 == 0?dopt == 0: 0)return({struct Cyc_PP_Doc*_tmp484[2];_tmp484[1]=
Cyc_Absynpp_ntyp2doc(_tmp482);_tmp484[0]=Cyc_Absynpp_tqual2doc(_tmp481);Cyc_PP_cat(
_tag_arr(_tmp484,sizeof(struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*
_tmp485[4];_tmp485[3]=Cyc_Absynpp_dtms2doc(dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)
dopt->v,_tmp483);_tmp485[2]=Cyc_PP_text(({const char*_tmp486=" ";_tag_arr(_tmp486,
sizeof(char),_get_zero_arr_size(_tmp486,2));}));_tmp485[1]=Cyc_Absynpp_ntyp2doc(
_tmp482);_tmp485[0]=Cyc_Absynpp_tqual2doc(_tmp481);Cyc_PP_cat(_tag_arr(_tmp485,
sizeof(struct Cyc_PP_Doc*),4));});}}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(
struct Cyc_Absyn_Aggrfield*f){if(f->width != 0)return({struct Cyc_PP_Doc*_tmp487[5];
_tmp487[4]=Cyc_PP_text(({const char*_tmp48A=";";_tag_arr(_tmp48A,sizeof(char),
_get_zero_arr_size(_tmp48A,2));}));_tmp487[3]=Cyc_Absynpp_atts2doc(f->attributes);
_tmp487[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));
_tmp487[1]=Cyc_PP_text(({const char*_tmp489=":";_tag_arr(_tmp489,sizeof(char),
_get_zero_arr_size(_tmp489,2));}));_tmp487[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)
f->type,({struct Cyc_Core_Opt*_tmp488=_cycalloc(sizeof(*_tmp488));_tmp488->v=Cyc_PP_textptr(
f->name);_tmp488;}));Cyc_PP_cat(_tag_arr(_tmp487,sizeof(struct Cyc_PP_Doc*),5));});
else{return({struct Cyc_PP_Doc*_tmp48B[3];_tmp48B[2]=Cyc_PP_text(({const char*
_tmp48D=";";_tag_arr(_tmp48D,sizeof(char),_get_zero_arr_size(_tmp48D,2));}));
_tmp48B[1]=Cyc_Absynpp_atts2doc(f->attributes);_tmp48B[0]=Cyc_Absynpp_tqtd2doc(f->tq,(
void*)f->type,({struct Cyc_Core_Opt*_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C->v=
Cyc_PP_textptr(f->name);_tmp48C;}));Cyc_PP_cat(_tag_arr(_tmp48B,sizeof(struct Cyc_PP_Doc*),
3));});}}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,({
const char*_tmp48E="";_tag_arr(_tmp48E,sizeof(char),_get_zero_arr_size(_tmp48E,1));}),
fields);}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct Cyc_Absyn_Tunionfield*
f){return({struct Cyc_PP_Doc*_tmp48F[3];_tmp48F[2]=f->typs == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc(f->typs);_tmp48F[1]=Cyc_Absynpp_typedef_name2doc(f->name);
_tmp48F[0]=Cyc_Absynpp_scope2doc((void*)f->sc);Cyc_PP_cat(_tag_arr(_tmp48F,
sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Tunionfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_tunionfield2doc,({const char*_tmp490=",";_tag_arr(_tmp490,sizeof(char),
_get_zero_arr_size(_tmp490,2));}),fields);}void Cyc_Absynpp_decllist2file(struct
Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);({void*_tmp491[0]={};
Cyc_Std_fprintf(f,({const char*_tmp492="\n";_tag_arr(_tmp492,sizeof(char),
_get_zero_arr_size(_tmp492,2));}),_tag_arr(_tmp491,sizeof(void*),0));});}}struct
_tagged_arr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){return Cyc_PP_string_of_doc(
Cyc_PP_seql(({const char*_tmp493="";_tag_arr(_tmp493,sizeof(char),
_get_zero_arr_size(_tmp493,1));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,
tdl)),72);}struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return
Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}struct _tagged_arr Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s),72);}
struct _tagged_arr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(
t),72);}struct _tagged_arr Cyc_Absynpp_typ2cstring(void*t){int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=
0;{struct _tagged_arr s=Cyc_Absynpp_typ2string(t);Cyc_Absynpp_qvar_to_Cids=
old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;return s;}}struct
_tagged_arr Cyc_Absynpp_prim2string(void*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(
p),72);}struct _tagged_arr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(
Cyc_Absynpp_pat2doc(p),72);}struct _tagged_arr Cyc_Absynpp_scope2string(void*sc){
return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
